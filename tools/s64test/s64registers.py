import re

# Ctrl+E triggers the ^E register list command
CMD_LIST_REGISTERS = '\x05'

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
    r'(-?\d+)\s+-\s+'            # signed decimal value
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
    dec_value = int(m.group(3), 10)
    label     = m.group(4)
    return S64Register(index, hex_value, dec_value, label)


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
