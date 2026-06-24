"""Drive the MPGuino debug monitor's interactive SWEET64 RAM assembler.

The assembler is entered with '<addr>!', which switches the prompt from the
normal ']' to a per-address 'XX!:' prompt.  Each subsequent line is one
instruction (mnemonic followed by hex operand bytes); on success the monitor
echoes the disassembled line and advances to the next address.  An empty line
exits back to the ']' prompt; a syntax error prints a message and also drops
back to ']'.

Operands are hex bytes (optional '0x', max two digits).  Register operands are
a single nibble-packed byte whose encoding depends on the instruction:
  - single-register ops (e.g. AddByteToX, Mul2byByte): low nibble = register
    number (1-7), upper nibble must be 0.  Write '1', not '11'.
  - two-register / arithmetic ops (LdRegByte, AddYtoX, CmpXtoY, SwapReg...):
    low nibble = X, high nibble = Y, e.g. '21' is X=1, Y=2.  (Confirmed in
    firmware sweet64.ino: regX = reg[(operand & 0x07) - 1], regY = high nibble.)
The monitor rejects a line with 'syntax' if the operand nibbles are invalid
for that instruction's format.

Example:
    from s64assembler import assemble
    assemble(term, 0, ['LdRegByte 12 2A', 'Done'])
"""

from s64terminal import (
    EITHER_PROMPT_RE, NORMAL_PROMPT_RE, is_asm_prompt, S64TerminalError,
)


class S64AssemblerError(Exception):
    pass


def assemble(term, address, instructions):
    """Assemble instruction source lines into SWEET64 program RAM beginning at
    the given byte address.

    instructions: iterable of strings, e.g. ['LdRegByte 12 2A', 'Done'].

    Returns a list of the monitor's disassembly echo lines (one or more per
    assembled instruction).  Raises S64AssemblerError if entry fails or any
    line is rejected."""
    # enter the assembler at the requested address
    _, prompt = term.exchange('{:X}!'.format(address), EITHER_PROMPT_RE)
    if not is_asm_prompt(prompt):
        raise S64AssemblerError(
            'failed to enter assembler at 0x{:02X} (prompt={!r})'.format(
                address, prompt))

    echoes = []
    try:
        for src in instructions:
            lines, prompt = term.exchange(src, EITHER_PROMPT_RE, flush=False)
            if not is_asm_prompt(prompt):
                # dropped back to ']' — the line was rejected
                raise S64AssemblerError(
                    'assembler rejected {!r}; monitor said: {}'.format(
                        src, ' | '.join(lines)))
            echoes.extend(lines)
    finally:
        # always leave the assembler: an empty line returns to the ']' prompt.
        # if we already errored back to ']', this is a harmless no-op command.
        try:
            term.exchange('', NORMAL_PROMPT_RE, flush=False)
        except S64TerminalError:
            term.sync()

    return echoes
