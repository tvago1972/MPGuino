"""Execute SWEET64 programs in RAM via the debug monitor and read results.

Execution uses the monitor's ^T (trace) command, which runs against the
terminal's private SWEET64 register set -- the same registers that ^E reads
and writes.  This gives a clean test loop:

    seed input registers (^E)  ->  run program (^T)  ->  read result registers (^E)

The ^T command always traces, so each executed instruction emits a
disassembly line and a full register dump.  That output is verbose but is
ignored here; the authoritative result is the register state read afterward.

Command form:  '<addr>'  '<'  '<count>'  Ctrl+T
  - addr  : RAM program start address (target, set by '<')
  - count : number of lines to trace; 0 means run to completion
"""

from s64terminal import NORMAL_PROMPT_RE, S64TerminalError
from s64assembler import assemble
from s64registers import set_registers, read_registers

CMD_TRACE = '\x14'      # Ctrl+T
CMD_RAM_OVERRIDE = '\x0F'   # Ctrl+O


def enable_ram_override(term, prgm_idx, ram_addr):
    """Redirect SWEET64 program index `prgm_idx` to RAM address `ram_addr`, so
    Call/Jump/CallImplied to that index execute the RAM subroutine there.
    Command: '<ram_addr> < <prgm_idx> ^O'  (target=ram_addr, byte=prgm_idx)."""
    cmd = '{:X}<{:X}{}'.format(ram_addr, prgm_idx, CMD_RAM_OVERRIDE)
    return term.send_command(cmd)


def disable_ram_override(term):
    """Disable the program RAM override (bare ^O)."""
    return term.send_command(CMD_RAM_OVERRIDE)


def run_ram_program(term, address=0, max_lines=0):
    """Run the RAM program at `address` via ^T.  max_lines=0 runs to
    completion.  Returns the raw trace output lines (usually ignored)."""
    cmd = '{:X}<{:X}{}'.format(address, max_lines, CMD_TRACE)
    return term.send_command(cmd, NORMAL_PROMPT_RE)


def run_program(term, instructions, inputs=None, address=0):
    """Assemble, seed inputs, run to completion, and read back registers.

    instructions : list of assembler source lines (the program under test).
                   It must end with an instruction that returns (e.g. 'Done').
    inputs       : optional dict {register_index: value} seeded before the run.
    address      : RAM start address (default 0).

    Returns (by_index, by_label) from read_registers() after execution."""
    assemble(term, address, instructions)

    if inputs:
        # set each requested register; values are full 64-bit (low byte for 8-bit regs)
        for idx, val in sorted(inputs.items()):
            set_registers(term, idx, [val])

    run_ram_program(term, address, max_lines=0)

    return read_registers(term)
