"""EEPROM parameter access via the debug monitor's P command.

The P command lists/stores EEPROM parameters in a format DIFFERENT from ^E/V:
  'INDEX:   <decimal value> - <addr/size metadata> (orig <decimal>) - LABEL'
e.g. '12:          0 - 00-20-0021 (orig          0) - pScratchpadIdx'
The value is shown in DECIMAL (SWEET64-formatted), with no clean hex column.
So this module resolves a parameter's INDEX by label (all that is needed for
the '&name' operand) and parses the leading decimal value only as a
convenience; value verification should be done SWEET64-independently by
loading the parameter into a register and reading it back as hex via ^E.

Parameter indices are NOT guaranteed stable across builds, so callers resolve
a parameter by its label (e.g. 'pScratchpadIdx') via find_parameter_index,
which caches the listing for the session.

Unlike RAM variables, EEPROM parameters are non-volatile and (for an unused
scratchpad like pScratchpadIdx) are not rewritten by the firmware, so a value
written by one command persists across later commands and across a reset.
"""

import re

CMD_PARAMETERS = 'P'

_CTRL_ECHO_RE = re.compile(r'^\^[A-Z]$')
# 'INDEX: <decimal value> ... - LABEL' (label is the final ' - '-delimited token)
_PARAM_RE = re.compile(
    r'^([0-9A-Fa-f]{2}):\s*'     # index
    r'(-?\d+|-+)'                # leading decimal value (or dashes)
    r'.* - '                     # addr/size metadata and (orig ...), up to last ' - '
    r'(\S+)\s*$'                 # label
)

_index_cache = {}


class S64Parameter:
    def __init__(self, index, value, label):
        self.index = index
        self.value = value          # decimal int (SWEET64-formatted), or None
        self.label = label

    def __repr__(self):
        return 'S64Parameter(0x{:02X}, {!r}, {})'.format(
            self.index, self.label, self.value)


def _parse_line(line):
    m = _PARAM_RE.match(line.strip())
    if not m:
        return None
    field = m.group(2)
    value = int(field, 10) if any(c.isdigit() for c in field) else None
    return S64Parameter(int(m.group(1), 16), value, m.group(3))


def read_parameters(term):
    """Issue a bare P (list all parameters) and return (by_index, by_label)."""
    lines = term.send_command(CMD_PARAMETERS)
    by_index = []
    by_label = {}
    for line in lines:
        if _CTRL_ECHO_RE.match(line.strip()):
            continue
        param = _parse_line(line)
        if param is None:
            continue
        while len(by_index) < param.index:
            by_index.append(None)
        by_index.append(param)
        by_label[param.label] = param
    return by_index, by_label


def find_parameter_index(term, label):
    """Resolve an EEPROM parameter's index by label, caching for the session."""
    if not _index_cache:
        _, by_label = read_parameters(term)
        for name, param in by_label.items():
            _index_cache[name] = param.index
    if label not in _index_cache:
        raise KeyError('EEPROM parameter {!r} not found in P listing'.format(label))
    return _index_cache[label]


def set_parameter(term, index, value):
    """Store a value into the parameter at the given index using
    '<index>P:<value>' (index and value in hex)."""
    cmd = '{:X}{}:{:X}'.format(index, CMD_PARAMETERS, value & 0xFFFFFFFFFFFFFFFF)
    return term.send_command(cmd)


def reset_cache():
    _index_cache.clear()
