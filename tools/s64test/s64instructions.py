import re
from s64terminal import S64Terminal, S64TerminalError

# control character echoes look like '^X' — skip them silently
_CTRL_ECHO_RE = re.compile(r'^\^[A-Z]$')

# Ctrl+I triggers the ^I instruction list command
CMD_LIST_INSTRUCTIONS = '\x09'

# format word bit flags (upper byte of opcodeFetchWord entry)
S64V_REGISTER_OPERATION  = 0x80
S64V_READ_IN_REG_BYTE    = 0x40
S64V_READ_IN_OPERAND     = 0x20
S64V_READ_IN_EXTRA       = 0x10
S64V_OPERAND_INDEXED     = 0x08
S64V_EXTRA_INDEXED       = 0x04
S64V_EXTRA_JUMP          = 0x02
S64V_RELATIVE_OPERAND    = 0x01

# regex for a valid instruction line:
#   XX: (FFFF)OO ...   instrSomething ...
# The mnemonic always starts with 'instr'
_LINE_RE = re.compile(
    r'^([0-9A-Fa-f]{2}):\s+'     # opcode index
    r'\(([0-9A-Fa-f]{4})\)'      # format word in parens
    r'([0-9A-Fa-f ]+?)\s+'       # raw bytes (opcode + optional operand bytes)
    r'(instr\S+)'                 # mnemonic
    r'(.*)?$'                    # optional operand description
)


class S64Instruction:
    def __init__(self, index, format_word, mnemonic, raw_bytes, operand_desc):
        self.index        = index           # int opcode index
        self.format_word  = format_word     # int (upper byte = validity flags)
        self.mnemonic     = mnemonic        # str e.g. 'instrDone'
        self.raw_bytes    = raw_bytes       # str of hex bytes as shown
        self.operand_desc = operand_desc    # str operand description or ''

        # convenience flags derived from upper byte of format word
        flags = (format_word >> 8) & 0xFF
        self.has_register_operand = bool(flags & S64V_REGISTER_OPERATION
                                         and flags & S64V_READ_IN_REG_BYTE)
        self.has_primary_operand  = bool(flags & S64V_READ_IN_OPERAND)
        self.has_extra_operand    = bool(flags & S64V_READ_IN_EXTRA)

    def __repr__(self):
        return (
            'S64Instruction(index=0x{:02X}, mnemonic={!r}, '
            'format=0x{:04X})'.format(self.index, self.mnemonic, self.format_word)
        )


def _parse_line(line):
    """Parse one output line from ^I.  Returns S64Instruction or None."""
    m = _LINE_RE.match(line.strip())
    if not m:
        return None
    index        = int(m.group(1), 16)
    format_word  = int(m.group(2), 16)
    raw_bytes    = m.group(3).strip()
    mnemonic     = m.group(4)
    operand_desc = m.group(5).strip() if m.group(5) else ''
    return S64Instruction(index, format_word, mnemonic, raw_bytes, operand_desc)


def fetch_instruction_set(term):
    """Issue ^I to the monitor and return a dict of {mnemonic: S64Instruction}
    plus a list indexed by opcode for direct lookup.

    Returns (by_mnemonic, by_opcode) where by_opcode is a list and
    by_mnemonic is a dict.  Invalid opcodes are omitted from both."""
    lines = term.send_command(CMD_LIST_INSTRUCTIONS)

    by_opcode   = []
    by_mnemonic = {}
    skipped     = []

    for line in lines:
        if _CTRL_ECHO_RE.match(line.strip()):
            continue
        if 'INVALID' in line:
            by_opcode.append(None)
            continue
        instr = _parse_line(line)
        if instr is None:
            skipped.append(line)
            continue
        # fill any gap (should not happen, but be safe)
        while len(by_opcode) < instr.index:
            by_opcode.append(None)
        by_opcode.append(instr)
        by_mnemonic[instr.mnemonic] = instr

    if skipped:
        import sys
        print('WARNING: {} line(s) from ^I could not be parsed:'.format(len(skipped)),
              file=sys.stderr)
        for s in skipped:
            print('  {!r}'.format(s), file=sys.stderr)

    return by_mnemonic, by_opcode
