"""Trip-slot index resolution via the debug monitor's R command.

A bare 'R' lists the trip-variable slots as 'INDEX: name' (no value column),
e.g. '0D: terminalIdx'.  Trip-slot indices are NOT stable across builds
(they depend on useDragRaceFunction, trackIdleEOCdata, window/bargraph
options, etc.), so callers resolve a slot by name -- in particular
'terminalIdx', the monitor's own scratch trip variable.

Measurement indices (rvVSSpulseIdx=0, rvVSScycleIdx=1, rvInjPulseIdx=2,
rvInjCycleIdx=3, rvEngCycleIdx=4) are fixed in the firmware and are used as
literal numbers in test programs.
"""

import re

CMD_LIST_TRIPSLOTS = 'R'

_TRIP_RE = re.compile(r'^([0-9A-Fa-f]{2}):\s+(\S+)\s*$')

_index_cache = {}


def read_trip_slots(term):
    """Issue a bare R and return {name: index}."""
    lines = term.send_command(CMD_LIST_TRIPSLOTS)
    slots = {}
    for line in lines:
        m = _TRIP_RE.match(line.strip())
        if m:
            slots[m.group(2)] = int(m.group(1), 16)
    return slots


def find_trip_slot_index(term, name):
    """Resolve a trip slot's index by name, caching for the session."""
    if not _index_cache:
        _index_cache.update(read_trip_slots(term))
    if name not in _index_cache:
        raise KeyError('trip slot {!r} not found in R listing'.format(name))
    return _index_cache[name]


def reset_cache():
    _index_cache.clear()
