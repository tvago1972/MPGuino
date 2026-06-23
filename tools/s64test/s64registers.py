import re

# Ctrl+E triggers the ^E register command (list, or store with :values)
CMD_REGISTERS = '\x05'
CMD_LIST_REGISTERS = CMD_REGISTERS   # backward-compatible alias

# register layout (matches firmware: 7x 64-bit regs, then the 8-bit regs)
S64_REG64_COUNT = 7

# control character echoes look like '^X' — skip them silently
_CTRL_ECHO_RE = re.compile(r'^\^[A-Z]$')

# header line: 'decimalFlags=00, windowLen=0A, places=00'
_HEADER_RE = re.compile(r'^decimalFlags=', re.IGNORECASE)

# register line:
#   '00: 0000000000000000 -          0 - s64reg64_1'
#   '07:               00 -          0 - si64reg8flags'
# fields are ' - ' separated; the value field is right-justified hex.
_REG_RE = re.compile(
    r'^([0-9A-Fa-f]{2}):\s*'     # register index
    r'([0-9A-Fa-f]+)\s+-\s+'     # hex value (variable width, space padded)
    r'(-?\d+|-+)\s+-\s+'         # signed decimal value, or dashes if not displayable
    r'(\S+)\s*$'                 # register label
)


class S64Register:
    def __init__(self, index, hex_value, dec_value, label):
        self.index     = index          # int register index
        self.hex_value = hex_value       # int value parsed from hex field
        self.dec_value = dec_value       # int value parsed from decimal field
        self.label     = label           # str register name

    def __repr__(self):
        return 'S64Register(0x{:02X}, {!r}, value=0x{:X})'.format(
            self.index, self.label, self.hex_value)


def _parse_line(line):
    """Parse one register line. Returns S64Register or None."""
    m = _REG_RE.match(line.strip())
    if not m:
        return None
    index     = int(m.group(1), 16)
    hex_value = int(m.group(2), 16)
    dec_field = m.group(3)
    # decimal column shows dashes when the value is too large to display
    dec_value = int(dec_field, 10) if any(c.isdigit() for c in dec_field) else None
    label     = m.group(4)
    return S64Register(index, hex_value, dec_value, label)


def set_registers(term, start_index, values):
    """Store one or more values into SWEET64 registers starting at
    start_index, using the 'x^E:v [v]...' monitor command.

    values: list of ints (or a single int). Values are sent in hex, which
    is the monitor's default numeric input mode. 64-bit registers accept a
    full 64-bit value; 8-bit registers take only the low byte.

    Returns the monitor's response lines (normally just the re-listed
    registers, since ^E echoes the stored state)."""
    if isinstance(values, int):
        values = [values]
    if not values:
        raise ValueError('set_registers requires at least one value')

    # index and values are all hexadecimal in the monitor
    hexvals = ' '.join('{:X}'.format(v & 0xFFFFFFFFFFFFFFFF) for v in values)
    cmd = '{:X}{}:{}'.format(start_index, CMD_REGISTERS, hexvals)
    return term.send_command(cmd)


def read_registers(term):
    """Issue ^E and return (by_index, by_label).

    by_index is a list of S64Register indexed by register index.
    by_label is a dict keyed by register label (e.g. 's64reg64_1').

    The hex_value field is the authoritative full-width register value;
    dec_value is the monitor's signed decimal interpretation."""
    lines = term.send_command(CMD_LIST_REGISTERS)

    by_index = []
    by_label = {}
    skipped  = []

    for line in lines:
        s = line.strip()
        if _CTRL_ECHO_RE.match(s):
            continue
        if _HEADER_RE.match(s):
            continue
        reg = _parse_line(s)
        if reg is None:
            skipped.append(line)
            continue
        while len(by_index) < reg.index:
            by_index.append(None)
        by_index.append(reg)
        by_label[reg.label] = reg

    if skipped:
        import sys
        print('WARNING: {} line(s) from ^E could not be parsed:'.format(len(skipped)),
              file=sys.stderr)
        for s in skipped:
            print('  {!r}'.format(s), file=sys.stderr)

    return by_index, by_label
