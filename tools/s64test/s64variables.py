"""Program-variable access via the debug monitor's V command.

The V command lists/stores main-program and volatile variables, in the same
line format as ^E:  'INDEX: HEXVAL - DECIMAL - LABEL'.

Variable indices are NOT guaranteed stable across builds/configs, so callers
must resolve a variable by its label (e.g. 'm8MetricModeFlags') rather than by
a hardcoded index. find_variable_index() does this, caching the full listing
for the session (indices are stable within a running firmware image).
"""

import re

CMD_VARIABLES = 'V'

_CTRL_ECHO_RE = re.compile(r'^\^[A-Z]$')
_VAR_RE = re.compile(
    r'^([0-9A-Fa-f]{2}):\s*'     # index
    r'([0-9A-Fa-f]+)\s+-\s+'     # hex value (width varies by variable size)
    r'(-?\d+|-+)\s+-\s+'         # decimal value, or dashes if not displayable
    r'(\S+)\s*$'                 # label (may include [..] subscripts)
)

# session cache of {label: index}, populated from a full V listing
_index_cache = {}


class S64Variable:
    def __init__(self, index, hex_value, label):
        self.index = index
        self.hex_value = hex_value
        self.label = label

    def __repr__(self):
        return 'S64Variable(0x{:02X}, {!r}, 0x{:X})'.format(
            self.index, self.label, self.hex_value)


def _parse_line(line):
    m = _VAR_RE.match(line.strip())
    if not m:
        return None
    return S64Variable(int(m.group(1), 16), int(m.group(2), 16), m.group(4))


def read_variables(term):
    """Issue a bare V (list all program variables) and return
    (by_index, by_label)."""
    lines = term.send_command(CMD_VARIABLES)
    by_index = []
    by_label = {}
    for line in lines:
        if _CTRL_ECHO_RE.match(line.strip()):
            continue
        var = _parse_line(line)
        if var is None:
            continue
        while len(by_index) < var.index:
            by_index.append(None)
        by_index.append(var)
        by_label[var.label] = var
    return by_index, by_label


def find_variable_index(term, label):
    """Resolve a program variable's index by its label, caching the full
    listing for the session. Raises KeyError if the label is not found."""
    if not _index_cache:
        _, by_label = read_variables(term)
        for name, var in by_label.items():
            _index_cache[name] = var.index
    if label not in _index_cache:
        raise KeyError('program variable {!r} not found in V listing'.format(label))
    return _index_cache[label]


def set_variable(term, index, value):
    """Store a value into the program variable at the given index using
    '<index>V:<value>' (index and value in hex)."""
    cmd = '{:X}{}:{:X}'.format(index, CMD_VARIABLES, value & 0xFFFFFFFFFFFFFFFF)
    return term.send_command(cmd)


def reset_cache():
    """Clear the cached label->index map (e.g. for a new session)."""
    _index_cache.clear()
