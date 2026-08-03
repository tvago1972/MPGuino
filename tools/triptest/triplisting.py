"""Debug-monitor terminal trip and trip-function parsers."""

import re


CMD_TRIP_MEASUREMENTS = 'T'
CMD_TRIP_FUNCTIONS = 'L'

_CTRL_ECHO_RE = re.compile(r'^(?:\])?(?:\^[A-Z]|[0-9A-Fa-f:. <\\$-]*[A-Z])$')
_IGNORED_RE = re.compile(r'^(?:syntax\x07?|decimalFlags=.*|=.*)$')
_TRIP_RE = re.compile(
    r'^(?:\])?'
    r'(?P<index>[0-9A-Fa-f]{1,2}):\s*'
    r'(?P<hex>[0-9A-Fa-f]+)\s+-\s+'
    r'(?P<decimal>-?\d+|-+)\s+-\s+'
    r'(?P<label>\S+)\s*$'
)
_FUNC_RE = re.compile(
    r'^(?:\])?'
    r'(?P<index>[0-9A-Fa-f]{1,2}):\s*'
    r'(?P<display>.*?)\s+-\s+'
    r'(?P<label>\S+)\s*$'
)
_NUMBER_RE = re.compile(r'-?\d+')

TRIP_MEASUREMENT_LABELS = {
    0: 'rvVSSpulseIdx',
    1: 'rvVSScycleIdx',
    2: 'rvInjPulseIdx',
    3: 'rvInjCycleIdx',
    4: 'rvEngCycleIdx',
}


class TripMeasurement(object):
    def __init__(self, index, raw_value, decimal_value, label, source_line=None):
        self.index = index
        self.raw_value = raw_value
        self.decimal_value = decimal_value
        self.label = label
        self.source_line = source_line


class TripFunctionOutput(object):
    def __init__(self, index, display, label, source_line=None):
        self.index = index
        self.display = display.strip()
        self.label = label
        self.source_line = source_line

    @property
    def leading_int(self):
        match = _NUMBER_RE.search(self.display)
        return int(match.group(0), 10) if match else None




def _is_ignored(line):
    stripped = line.strip()
    return not stripped or _CTRL_ECHO_RE.match(stripped) or _IGNORED_RE.match(stripped)


def _parse_trip_line(line):
    match = _TRIP_RE.match(line.strip())
    if not match:
        return None
    decimal = match.group('decimal')
    return TripMeasurement(
        index=int(match.group('index'), 16),
        raw_value=int(match.group('hex'), 16),
        decimal_value=int(decimal, 10) if any(c.isdigit() for c in decimal) else None,
        label=match.group('label'),
        source_line=line.strip(),
    )


def _parse_function_line(line):
    match = _FUNC_RE.match(line.strip())
    if not match:
        return None
    return TripFunctionOutput(
        index=int(match.group('index'), 16),
        display=match.group('display'),
        label=match.group('label'),
        source_line=line.strip(),
    )




def read_trip_measurements(term):
    lines = term.send_command(CMD_TRIP_MEASUREMENTS)
    by_index = {}
    by_label = {}
    unparsed = []
    for line in lines:
        if _is_ignored(line):
            continue
        item = _parse_trip_line(line)
        if item is None:
            unparsed.append(line.strip())
            continue
        by_index[item.index] = item
        by_label[item.label] = item
    for index, label in TRIP_MEASUREMENT_LABELS.items():
        if label not in by_label and index in by_index:
            by_index[index].label = label
            by_label[label] = by_index[index]
    return by_index, by_label, unparsed


def set_trip_measurements(term, start_index, values):
    cmd = '{:X}{}:{}'.format(
        start_index,
        CMD_TRIP_MEASUREMENTS,
        ' '.join(str(value & 0xFFFFFFFFFFFFFFFF) for value in values))
    return term.send_command(cmd)


def read_trip_functions(term, window=None):
    cmd = CMD_TRIP_FUNCTIONS if window is None else '{:X}<{}'.format(window, CMD_TRIP_FUNCTIONS)
    lines = term.send_command(cmd)
    by_index = {}
    by_label = {}
    unparsed = []
    for line in lines:
        if _is_ignored(line):
            continue
        item = _parse_function_line(line)
        if item is None:
            unparsed.append(line.strip())
            continue
        by_index[item.index] = item
        by_label[item.label] = item
    return by_index, by_label, unparsed
