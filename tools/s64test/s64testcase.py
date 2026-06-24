"""Test-case definition and execution for SWEET64 opcode regression testing.

A case declares a small program, optional seeded input registers, and the
expected register state (and/or an expected SWEET64 error) after running it.

Registers in `inputs` and `expect` are named by their 1-based SWEET64
assembler number (register 1..7), matching how they appear in program source.
The runner maps register N to the 0-based ^E index N-1 internally.
"""

from dataclasses import dataclass, field

from s64assembler import assemble, S64AssemblerError
from s64registers import set_registers, read_registers
from s64variables import find_variable_index, set_variable, read_variables
from s64parameters import find_parameter_index, set_parameter, read_parameters
from s64exec import run_ram_program
from s64terminal import S64TerminalError

REG64_COUNT = 7                     # SWEET64 64-bit registers (reg 1..7)
ERROR_LABEL = 'si64reg8error'       # 8-bit register holding the last error code
FLAGS_LABEL = 'si64reg8flags'       # 8-bit register holding processor flags
RAM_ADDRESS = 0                     # where test programs are assembled

# SWEET64 processor flag bits (low nibble of si64reg8flags); the high bits
# (0xC0) are trace flags set during ^T and are ignored by flag checks.
FLAG_CARRY    = 0x01
FLAG_ZERO     = 0x02
FLAG_MINUS    = 0x04
FLAG_OVERFLOW = 0x08

_FLAG_NAMES = {
    FLAG_CARRY: 'carry', FLAG_ZERO: 'zero',
    FLAG_MINUS: 'minus', FLAG_OVERFLOW: 'overflow',
}


@dataclass
class S64Case:
    name: str
    program: list                       # assembler source lines
    inputs: dict = field(default_factory=dict)   # {reg_number(1..7): value}
    set_vars: dict = field(default_factory=dict)  # {var_label: value} program vars set before run
    set_params: dict = field(default_factory=dict)  # {param_label: value} EEPROM params set before run
    expect: dict = field(default_factory=dict)   # {reg_number(1..7): value}
    expect_reg8: dict = field(default_factory=dict)   # {label: value} 8-bit regs
    expect_vars: dict = field(default_factory=dict)   # {var_label: value} program vars
    expect_params: dict = field(default_factory=dict)  # {param_label: value} EEPROM params
    expect_flags: dict = field(default_factory=dict)  # {FLAG_xxx: bool}
    expect_error: bool = False          # whether a SWEET64 error is expected


@dataclass
class S64Result:
    name: str
    passed: bool
    failures: list = field(default_factory=list)


def _reg_index(reg_number):
    """Map a 1-based SWEET64 register number to its 0-based ^E index."""
    if not (1 <= reg_number <= REG64_COUNT):
        raise ValueError('register number {} out of range 1..{}'.format(
            reg_number, REG64_COUNT))
    return reg_number - 1


def _resolve_refs(term, program):
    """Replace symbolic operand references with two-hex-digit indices, so
    unstable program-variable / EEPROM-parameter indices never appear
    literally in a case:
      '@name' -> program variable index (V command)
      '&name' -> EEPROM parameter index (P command)
    Other tokens pass through unchanged."""
    out = []
    for line in program:
        tokens = line.split()
        resolved = []
        for t in tokens:
            if t.startswith('@'):
                resolved.append('{:02X}'.format(find_variable_index(term, t[1:])))
            elif t.startswith('&'):
                resolved.append('{:02X}'.format(find_parameter_index(term, t[1:])))
            else:
                resolved.append(t)
        out.append(' '.join(resolved))
    return out


def run_case(term, case):
    """Execute one S64Case against the live monitor and return an S64Result.

    Steps: zero all 64-bit registers (clean baseline), seed inputs, assemble,
    run to completion, read back, and compare against expectations.
    Infrastructure errors (assembler rejection, serial trouble) are reported
    as failures rather than raised, so one bad case does not abort the run."""
    failures = []

    try:
        # clean baseline so prior cases cannot leak state into this one
        set_registers(term, 0, [0] * REG64_COUNT)

        # seed inputs
        for reg_number, value in sorted(case.inputs.items()):
            set_registers(term, _reg_index(reg_number), [value])

        # seed program variables (resolved by label, not index)
        for label, value in sorted(case.set_vars.items()):
            set_variable(term, find_variable_index(term, label), value)

        # seed EEPROM parameters (resolved by label, not index)
        for label, value in sorted(case.set_params.items()):
            set_parameter(term, find_parameter_index(term, label), value)

        # assemble and run (resolve @var / &param operand references first)
        program = _resolve_refs(term, case.program)
        assemble(term, RAM_ADDRESS, program)
        run_ram_program(term, RAM_ADDRESS, max_lines=0)

        # read results
        by_index, by_label = read_registers(term)

    except (S64AssemblerError, S64TerminalError, ValueError) as e:
        return S64Result(case.name, False, ['{}: {}'.format(type(e).__name__, e)])

    # check expected register values
    for reg_number, expected in sorted(case.expect.items()):
        idx = _reg_index(reg_number)
        reg = by_index[idx] if idx < len(by_index) else None
        if reg is None:
            failures.append('reg {} (idx 0x{:02X}) not reported'.format(reg_number, idx))
        elif reg.hex_value != expected:
            failures.append('reg {}: got 0x{:X}, expected 0x{:X}'.format(
                reg_number, reg.hex_value, expected))

    # check expected 8-bit register values (addressed by label)
    for label, expected in sorted(case.expect_reg8.items()):
        reg = by_label.get(label)
        if reg is None:
            failures.append('8-bit reg {!r} not reported'.format(label))
        elif reg.hex_value != expected:
            failures.append('reg {}: got 0x{:X}, expected 0x{:X}'.format(
                label, reg.hex_value, expected))

    # check expected program-variable values (read only if requested)
    if case.expect_vars:
        try:
            _, vars_by_label = read_variables(term)
        except S64TerminalError as e:
            failures.append('could not read program variables: {}'.format(e))
            vars_by_label = {}
        for label, expected in sorted(case.expect_vars.items()):
            var = vars_by_label.get(label)
            if var is None:
                failures.append('program var {!r} not reported'.format(label))
            elif var.hex_value != expected:
                failures.append('var {}: got 0x{:X}, expected 0x{:X}'.format(
                    label, var.hex_value, expected))

    # check expected EEPROM parameter values (read only if requested)
    if case.expect_params:
        try:
            _, params_by_label = read_parameters(term)
        except S64TerminalError as e:
            failures.append('could not read EEPROM parameters: {}'.format(e))
            params_by_label = {}
        # NOTE: the P command reports values in SWEET64-formatted decimal, so
        # this check is not SWEET64-independent; prefer verifying EEPROM values
        # by loading them into a register and reading hex via ^E.
        for label, expected in sorted(case.expect_params.items()):
            param = params_by_label.get(label)
            if param is None:
                failures.append('EEPROM param {!r} not reported'.format(label))
            elif param.value != expected:
                failures.append('param {}: got {}, expected {}'.format(
                    label, param.value, expected))

    # check expected processor flags (only the specified bits are tested)
    if case.expect_flags:
        flags_reg = by_label.get(FLAGS_LABEL)
        flags = flags_reg.hex_value if flags_reg is not None else 0
        for bit, want in case.expect_flags.items():
            got = bool(flags & bit)
            if got != want:
                name = _FLAG_NAMES.get(bit, '0x{:02X}'.format(bit))
                failures.append('flag {}: got {}, expected {}'.format(name, got, want))

    # check error expectation
    err = by_label.get(ERROR_LABEL)
    err_code = err.hex_value if err is not None else 0
    if case.expect_error and err_code == 0:
        failures.append('expected a SWEET64 error, but none occurred')
    elif not case.expect_error and err_code != 0:
        failures.append('unexpected SWEET64 error code 0x{:X}'.format(err_code))

    return S64Result(case.name, not failures, failures)
