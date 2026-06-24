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
    expect: dict = field(default_factory=dict)   # {reg_number(1..7): value}
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

        # assemble and run
        assemble(term, RAM_ADDRESS, case.program)
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
