"""Debug-monitor EEPROM parameter listing parser.

The firmware remains a plain read/write/list interface; this module owns the
interpretation needed by the host-side EEPROM regression harness.
"""

import re


CMD_PARAMETERS = 'P'

ECS_SOFTWARE_INIT = 0x80
ECS_HARDWARE_INIT = 0x40
ECS_METRIC_MODE = 0x20
ECS_CHANGE_DISPLAY = 0x10
ECS_CALCULATE_FUEL = 0x08
ECS_RESET_BFEVS = 0x04

RANGE_ALL_SETTINGS = None
_DISPLAY_PAGE_LABEL_RE = re.compile(r'^P[0-9A-Fa-f]{2}F[0-9A-Fa-f]{2}$')

_CTRL_ECHO_RE = re.compile(r'^(?:\])?(?:\^[A-Z]|[0-9A-Fa-f]*<?[A-Z])$')
_IGNORED_RE = re.compile(r'^(?:syntax\x07?|decimalFlags=.*|=.*)$')
_PARAM_RE = re.compile(
    r'^(?:\])?'                    # prompt occasionally arrives glued to echo/output
    r'(?:(?P<index>[0-9A-Fa-f]{1,2}):\s*)?'  # optional index
    r'(?P<value>-?\d+|-+)\s+-\s+'  # decimal value, or dashes
    r'(?P<flags>[0-9A-Fa-f]{2})-'
    r'(?P<bits>[0-9A-Fa-f]{2})-'
    r'(?P<addr>[0-9A-Fa-f]{4})'
    r'(?P<tail>.*)$'
)


class EEPROMParameter(object):
    def __init__(self, index, value, flags, bit_length, address, label=None,
                 raw_bytes=None, source_line=None):
        self.index = index
        self.value = value
        self.flags = flags
        self.bit_length = bit_length
        self.address = address
        self.label = label or ('param_{:02X}'.format(index) if index is not None else 'param_unknown')
        self.raw_bytes = raw_bytes or []
        self.source_line = source_line

    @property
    def byte_length(self):
        return (self.bit_length + 7) // 8

    @property
    def action(self):
        return self.flags

    @property
    def raw_value(self):
        value = 0
        for byte in self.raw_bytes:
            value = (value << 8) | byte
        return value if self.raw_bytes else None

    def max_value(self):
        if self.bit_length >= 64:
            return 0xFFFFFFFFFFFFFFFF
        return (1 << self.bit_length) - 1

    def __repr__(self):
        return 'EEPROMParameter(0x{:02X}, {!r}, flags=0x{:02X}, bits={}, addr=0x{:04X})'.format(
            self.index if self.index is not None else 0, self.label, self.flags, self.bit_length, self.address)


def _parse_raw_bytes(field):
    if not field:
        return []
    bytes_out = []
    for token in field.split():
        if len(token) >= 2 and (len(token) & 1) == 0 and re.match(r'^[0-9A-Fa-f]+$', token):
            for x in range(0, len(token), 2):
                bytes_out.append(int(token[x:x + 2], 16))
    return bytes_out


def _parse_tail(field):
    label = None
    raw_field = field
    if ' - ' in field:
        raw_field, label = field.rsplit(' - ', 1)
        label = label.strip() or None
    return _parse_raw_bytes(raw_field), label


def parse_parameter_line(line):
    m = _PARAM_RE.match(line.strip())
    if not m:
        return None
    value_field = m.group('value')
    value = int(value_field, 10) if any(c.isdigit() for c in value_field) else None
    raw_bytes, label = _parse_tail(m.group('tail'))
    return EEPROMParameter(
        index=int(m.group('index'), 16) if m.group('index') is not None else None,
        value=value,
        flags=int(m.group('flags'), 16),
        bit_length=int(m.group('bits'), 16),
        address=int(m.group('addr'), 16),
        raw_bytes=raw_bytes,
        label=label,
        source_line=line.strip(),
    )


def _range_command(range_id):
    if range_id is None:
        return CMD_PARAMETERS
    return '{:X}<{}'.format(range_id, CMD_PARAMETERS)


def read_parameters(term, range_id=RANGE_ALL_SETTINGS):
    lines = term.send_command(_range_command(range_id))
    by_index = {}
    by_label = {}
    unparsed = []
    for line in lines:
        stripped = line.strip()
        if not stripped or _CTRL_ECHO_RE.match(stripped) or _IGNORED_RE.match(stripped):
            continue
        param = parse_parameter_line(stripped)
        if param is None:
            unparsed.append(stripped)
            continue
        if param.index is not None:
            by_index[param.index] = param
        by_label[param.label] = param
    return by_index, by_label, unparsed


def read_parameter(term, index):
    lines = term.send_command('{:X}{}'.format(index, CMD_PARAMETERS))
    unparsed = []
    for line in lines:
        stripped = line.strip()
        if not stripped or _CTRL_ECHO_RE.match(stripped) or _IGNORED_RE.match(stripped):
            continue
        param = parse_parameter_line(stripped)
        if param is None:
            unparsed.append(stripped)
            continue
        if param.index is None:
            param.index = index
        if param.index == index:
            return param, unparsed
        unparsed.append(stripped)
    return None, unparsed


def read_all_parameter_ranges(term):
    params = {}
    unparsed = {}
    _, by_label, skipped = read_parameters(term, RANGE_ALL_SETTINGS)
    if by_label:
        params['settings'] = by_label
    if skipped:
        unparsed['settings'] = skipped
    if 'settings' in params:
        params['saved_trips'] = {
            label: param
            for label, param in params['settings'].items()
            if (
                label.startswith('pCurrTrip')
                or label.startswith('pTankTrip')
                or label.startswith('pCurrIEOC')
                or label.startswith('pTankIEOC')
                or label == 'pRefuelSaveSizeIdx'
            )
        }
        params['display_pages'] = {
            label: param
            for label, param in params['settings'].items()
            if _DISPLAY_PAGE_LABEL_RE.match(label)
        }
        params['display_cursor'] = {
            label: param
            for label, param in params['settings'].items()
            if (('DisplayIdx' in label) or ('displayIdx' in label)) and not label.startswith('p')
        }
        params['menu_height'] = {
            label: param
            for label, param in params['settings'].items()
            if label.endswith('MenuIdx')
        }
    return params, unparsed


def set_parameter(term, index, value):
    if index is None:
        raise ValueError('cannot write parameter without an index')
    cmd = '{:X}{}:{}'.format(index, CMD_PARAMETERS, value & 0xFFFFFFFFFFFFFFFF)
    return term.send_command(cmd)


def parameter_groups(params):
    groups = {
        'hardware_init': [],
        'software_init': [],
        'fuel_calc': [],
        'display_change': [],
        'metric_mode': [],
    }
    for param in sorted(params, key=lambda p: p.index if p.index is not None else 0x100):
        flags = param.flags
        if flags & ECS_HARDWARE_INIT:
            groups['hardware_init'].append(param)
        if flags & ECS_SOFTWARE_INIT:
            groups['software_init'].append(param)
        if flags & ECS_CALCULATE_FUEL:
            groups['fuel_calc'].append(param)
        if flags & ECS_CHANGE_DISPLAY:
            groups['display_change'].append(param)
        if flags & ECS_METRIC_MODE:
            groups['metric_mode'].append(param)
    return groups
