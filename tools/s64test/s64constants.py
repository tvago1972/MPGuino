"""Program-constant access via the debug monitor's O command.

'O' lists the program constants in the same line format as ^E/V:
  'INDEX: <hex32> - <decimal> - <LABEL>'
where LABEL may be several aliases joined by '/', e.g.
  '24: 000003E8 - 1000 - idxOneThousand/idxDecimalPoint'

Constant indices are NOT stable across builds, so callers resolve a constant
by an alias name via find_constant_index, which caches the listing for the
session.  Matching is done against the exact '/'-split components of each
label (not substring), so 'idxTen' does not accidentally match
'idxTenThousand'.

The hex value column is plain-C output (hexDWordOut) and is exposed for
reference, but const/readonly instruction tests verify results against the
known, build-stable constant values (idxTen=10, idxOneThousand=1000, ...) via
^E hex, independent of any SWEET64 read path.
"""

import re

# '1' selects the constant range; '<' sets it as the list target.
CMD_LIST_CONSTANTS = 'O'

_CTRL_ECHO_RE = re.compile(r'^\^[A-Z]$')
_CONST_RE = re.compile(
    r'^([0-9A-Fa-f]{2}):\s*'
    r'([0-9A-Fa-f]+)\s+-\s+'
    r'(-?\d+|-+)\s+-\s+'
    r'(\S+)\s*$'
)

_index_cache = {}        # alias name -> index


class S64Constant:
    def __init__(self, index, value, label):
        self.index = index
        self.value = value          # int (constant value, from hex column)
        self.label = label          # full '/'-joined label
        self.aliases = label.split('/')

    def __repr__(self):
        return 'S64Constant(0x{:02X}, {!r}, 0x{:X})'.format(
            self.index, self.label, self.value)


def _parse_line(line):
    m = _CONST_RE.match(line.strip())
    if not m:
        return None
    return S64Constant(int(m.group(1), 16), int(m.group(2), 16), m.group(4))


def read_constants(term):
    """Issue 'O' and return (by_index, by_alias)."""
    lines = term.send_command(CMD_LIST_CONSTANTS)
    by_index = []
    by_alias = {}
    for line in lines:
        if _CTRL_ECHO_RE.match(line.strip()):
            continue
        const = _parse_line(line)
        if const is None:
            continue
        while len(by_index) < const.index:
            by_index.append(None)
        by_index.append(const)
        for alias in const.aliases:
            by_alias[alias] = const
    return by_index, by_alias


def find_constant_index(term, name):
    """Resolve a constant's index by an exact alias name, caching for the
    session. Raises KeyError if the alias is not found."""
    if not _index_cache:
        _, by_alias = read_constants(term)
        for alias, const in by_alias.items():
            _index_cache[alias] = const.index
    if name not in _index_cache:
        raise KeyError('program constant {!r} not found in O listing'.format(name))
    return _index_cache[name]


def reset_cache():
    _index_cache.clear()
