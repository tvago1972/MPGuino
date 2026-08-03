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


MNEMONIC_ALIASES = {
    'LdRegRdOnly': 'LdRegConst',
    'LdRegRdOnlyIndexed': 'LdRegConstIndexed',
    'LdRegRdOnlyOffset': 'LdRegConstOffset',
    'LdRegRdOnlyMetric': 'LdRegConstMetric',
    'Mul2byRdOnly': 'Mul2byConst',
    'Div2byRdOnly': 'Div2byConst',
}


def resolve_labels(instructions, base_address=0):
    """Resolve symbolic branch-target labels to absolute addresses.

    A line of the form 'name:' (a single token ending in ':') defines a label
    at the current address.  Any operand token matching a defined label is
    replaced with that label's address as a two-hex-digit byte.

    Address computation relies on the fact that each assembled instruction
    occupies exactly one byte per token (opcode byte + one byte per operand),
    so an instruction's length equals its token count.  Forward references are
    supported via a two-pass walk.  Definition lines are removed from output.
    """
    # pass 1: assign an address to each label, advancing by token count
    labels = {}
    addr = base_address
    body = []                       # instruction lines (definitions stripped)
    for line in instructions:
        s = line.strip()
        if not s:
            continue
        tokens = s.split()
        if len(tokens) == 1 and tokens[0].endswith(':'):
            labels[tokens[0][:-1]] = addr
            continue
        tokens[0] = MNEMONIC_ALIASES.get(tokens[0], tokens[0])
        body.append(tokens)
        addr += len(tokens)         # opcode + operands, one byte each

    # pass 2: substitute label references in operands
    out = []
    for tokens in body:
        resolved = [tokens[0]]
        for t in tokens[1:]:
            if t in labels:
                resolved.append('{:02X}'.format(labels[t]))
            else:
                resolved.append(t)
        out.append(' '.join(resolved))
    return out


def assemble(term, address, instructions):
    """Assemble instruction source lines into SWEET64 program RAM beginning at
    the given byte address.

    instructions: iterable of strings, e.g. ['LdRegByte 12 2A', 'Done'].

    Returns a list of the monitor's disassembly echo lines (one or more per
    assembled instruction).  Raises S64AssemblerError if entry fails or any
    line is rejected.

    Symbolic labels (see resolve_labels) are resolved to absolute addresses
    before assembly, so branch targets can be written by name."""
    instructions = resolve_labels(instructions, address)

    # enter the assembler at the requested address; retry once on a sync
    # hiccup (e.g. the first command after connect occasionally lands on ']')
    _, prompt = term.exchange('{:X}!'.format(address), EITHER_PROMPT_RE)
    if not is_asm_prompt(prompt):
        term.sync()
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
                # dropped back to ']' ? the line was rejected
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
