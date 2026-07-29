"""EEPROM harness case definitions.

The cases intentionally describe host-side behavior only. Firmware stays as a
plain monitor: list, read, write.
"""


class EEPROMCase(object):
    def __init__(self, name, parameter, value=None, expect_raw=None,
                 expect_vars=None, restore=True):
        self.name = name
        self.parameter = parameter
        self.value = value
        self.expect_raw = expect_raw
        self.expect_vars = expect_vars or {}
        self.restore = restore


TIMEOUT_VAR_CASES = [
    ('pIdleTimeoutIdx', 'v16VehicleStopTimeoutIdx'),
    ('pEOCtimeoutIdx', 'v16EngineOffTimeoutIdx'),
    ('pButtonTimeoutIdx', 'v16InputTimeoutIdx'),
    ('pParkTimeoutIdx', 'v16ParkTimeoutIdx'),
    ('pActivityTimeoutIdx', 'v16ActivityTimeoutIdx'),
]

ACTION_NAMES = {
    0x01: 'software-init',
    0x02: 'hardware-init',
    0x05: 'fuel-calc',
    0x06: 'hardware-init-bargraph',
    0x07: 'software-init-bargraph',
    0x08: 'hardware-init-fuel-calc',
}


def _round_div(numer, denom):
    if denom == 0:
        return None
    quotient, remainder = divmod(numer, denom)
    if (remainder * 2) >= denom:
        quotient += 1
    return quotient


def _floor_div(numer, denom):
    if denom == 0:
        return None
    return numer // denom


def _const(constants_by_alias, label):
    item = constants_by_alias.get(label)
    return item.value if item is not None else None


def _sample_value(param):
    limit = param.max_value()
    current = param.raw_value
    if limit == 0:
        return 0
    if current is None:
        return min(limit, 1)
    candidate = current + 1
    if candidate > limit:
        candidate = current - 1
    if candidate < 0:
        candidate = 0
    return candidate


def _current_value(params_by_label, label):
    param = params_by_label.get(label)
    return param.raw_value if param is not None else None


def _parameter_values(params_by_label, changed_label, changed_value):
    values = {}
    for label, param in params_by_label.items():
        values[label] = param.raw_value
    values[changed_label] = changed_value
    return values


def _add_if_present(expect, label, value, mask):
    if value is not None:
        expect[label] = value & mask


def _hardware_init_expectations(params_by_label, constants_by_alias, changed_label, changed_value):
    values = _parameter_values(params_by_label, changed_label, changed_value)
    expect = {}

    cycles0 = _const(constants_by_alias, 'idxCycles0PerSecond')
    cycles0_per_tick = _const(constants_by_alias, 'idxCycles0PerTick')
    seconds_per_hour = _const(constants_by_alias, 'idxSecondsPerHour')
    decimal = _const(constants_by_alias, 'idxDecimalPoint')
    micros = _const(constants_by_alias, 'idxMicroSecondsPerSecond')

    pulse_edge = values.get('pPulseEdgePerDistanceIdx')
    min_speed = values.get('pMinGoodSpeedidx')
    min_rpm = values.get('pMinGoodRPMidx')
    inj_per_2 = values.get('pInjPer2CrankRevIdx')
    settle = values.get('pInjectorSettleTimeIdx')

    speed_factor = None
    speed_labels = (
        'pPulseEdgePerDistanceIdx',
    )
    vss_labels = (
        'pPulseEdgePerDistanceIdx',
        'pMinGoodSpeedidx',
    )
    engine_labels = (
        'pMinGoodRPMidx',
        'pInjPer2CrankRevIdx',
    )
    injector_labels = (
        'pMinGoodRPMidx',
        'pInjPer2CrankRevIdx',
        'pInjectorSettleTimeIdx',
    )
    drag_speed_labels = (
        'pDragSpeedIdx',
    )
    drag_distance_labels = (
        'pPulseEdgePerDistanceIdx',
        'pDragDistanceIdx',
    )

    if None not in (cycles0, seconds_per_hour, decimal, pulse_edge):
        speed_factor = _floor_div(cycles0 * seconds_per_hour * decimal, pulse_edge)
        if changed_label in speed_labels:
            _add_if_present(expect, 'm32SpeedFactorIdx', speed_factor, 0xFFFFFFFF)

    max_vss = None
    if None not in (speed_factor, min_speed):
        max_vss = _floor_div(speed_factor * 2, min_speed)
        if changed_label in vss_labels:
            _add_if_present(expect, 'v32MaximumVSSperiodIdx', max_vss, 0xFFFFFFFF)

    if None not in (max_vss, cycles0_per_tick):
        detect_stop = _round_div(max_vss, cycles0_per_tick)
        if detect_stop is not None:
            detect_stop = _floor_div(detect_stop, 2)
        if changed_label in vss_labels:
            _add_if_present(expect, 'v16DetectVehicleStopIdx', detect_stop, 0xFFFF)

    max_engine = None
    if None not in (cycles0, min_rpm, inj_per_2):
        max_engine = _round_div(cycles0 * 120, min_rpm * inj_per_2)
        if changed_label in engine_labels:
            _add_if_present(expect, 'v32MaximumEnginePeriodIdx', max_engine, 0xFFFFFFFF)

    if None not in (max_engine, cycles0_per_tick):
        detect_engine = _round_div(max_engine, cycles0_per_tick)
        if changed_label in engine_labels:
            _add_if_present(expect, 'v16DetectEngineOffIdx', detect_engine, 0xFFFF)

    open_delay = None
    if None not in (settle, cycles0, micros):
        open_delay = _floor_div(settle * cycles0, micros)
        if changed_label in injector_labels:
            _add_if_present(expect, 'v32InjectorOpenDelayIdx', open_delay, 0xFFFFFFFF)

    if None not in (max_engine, open_delay):
        if changed_label in engine_labels:
            _add_if_present(expect, 'v32InjectorValidMaxWidthIdx', max_engine - open_delay, 0xFFFFFFFF)

    if changed_label == 'pVSSpauseIdx':
        _add_if_present(expect, 'v8VSSdebounceTickIdx', values.get('pVSSpauseIdx'), 0xFF)

    drag_speed = values.get('pDragSpeedIdx')
    if None not in (speed_factor, drag_speed):
        accel_half = _round_div(speed_factor & 0xFFFFFFFF, drag_speed)
        if changed_label in drag_speed_labels:
            _add_if_present(expect, 'v32AccelHalfPeriodIdx', accel_half, 0xFFFFFFFF)
        if accel_half is not None:
            if changed_label in drag_speed_labels:
                _add_if_present(expect, 'v32AccelFullPeriodIdx', _floor_div(accel_half & 0xFFFFFFFF, 2), 0xFFFFFFFF)

    drag_distance = values.get('pDragDistanceIdx')
    if None not in (pulse_edge, drag_distance, decimal):
        accel_distance = _round_div(pulse_edge * drag_distance, decimal)
        if changed_label in drag_distance_labels:
            _add_if_present(expect, 'v32AccelDistanceValueIdx', accel_distance, 0xFFFFFFFF)

    coastdown = values.get('pCoastdownSamplePeriodIdx')
    if changed_label == 'pCoastdownSamplePeriodIdx' and None not in (coastdown, _const(constants_by_alias, 'idxTicks0PerSecond')):
        _add_if_present(
            expect,
            'v32CoastdownPeriodIdx',
            coastdown * _const(constants_by_alias, 'idxTicks0PerSecond'),
            0xFFFFFFFF)

    return expect


def _software_init_expectations(params_by_label, constants_by_alias, changed_label, changed_value):
    micro = _current_value(params_by_label, 'pMicroSecondsPerGallonIdx')
    tank = _current_value(params_by_label, 'pTankSizeIdx')
    bingo = _current_value(params_by_label, 'pTankBingoSizeIdx')
    metric = _current_value(params_by_label, 'pMetricModeIdx') or 0

    if changed_label == 'pMicroSecondsPerGallonIdx':
        micro = changed_value
    elif changed_label == 'pTankSizeIdx':
        tank = changed_value
    elif changed_label == 'pTankBingoSizeIdx':
        bingo = changed_value

    cycles0 = _const(constants_by_alias, 'idxCycles0PerSecond')
    micros = _const(constants_by_alias, 'idxMicroSecondsPerSecond')
    decimal = _const(constants_by_alias, 'idxDecimalPoint')
    numer_volume = _const(constants_by_alias, 'idxNumerVolume')
    denom_volume = _const(constants_by_alias, 'idxDenomVolume')

    required = [micro, tank, bingo, cycles0, micros, decimal]
    if any(x is None for x in required):
        return {}

    adjusted_micro = micro
    if _const(constants_by_alias, 'idxNumerImperialGallon') is not None:
        if not metric:
            adjusted_micro = _round_div(
                adjusted_micro * _const(constants_by_alias, 'idxNumerImperialGallon'),
                _const(constants_by_alias, 'idxDenomImperialGallon'))
    elif metric:
        if numer_volume is None or denom_volume is None:
            return {}
        adjusted_micro = _round_div(adjusted_micro * denom_volume, numer_volume)

    cycles_per_volume = _round_div(adjusted_micro * cycles0, micros)
    if cycles_per_volume is None:
        return {}

    tank_cycles = _round_div(tank * cycles_per_volume, decimal)
    bingo_cycles = _round_div(bingo * cycles_per_volume, decimal)
    if tank_cycles is None or bingo_cycles is None:
        return {}

    return {
        'm32CyclesPerVolumeIdx': cycles_per_volume & 0xFFFFFFFF,
        'm64TankSizeIdx': tank_cycles & 0xFFFFFFFFFFFFFFFF,
        'm64BingoTankSizeIdx': bingo_cycles & 0xFFFFFFFFFFFFFFFF,
    }


def build_storage_cases(params):
    """Return one write/readback case per bit-length bucket."""
    by_bits = {}
    for param in params:
        if param.label == 'pSignatureIdx':
            continue
        by_bits.setdefault(param.bit_length, param)

    cases = []
    for bit_length in sorted(by_bits):
        param = by_bits[bit_length]
        value = _sample_value(param)
        cases.append(EEPROMCase(
            name='storage round-trip: {} ({} bit)'.format(param.label, bit_length),
            parameter=param.label,
            value=value,
            expect_raw=value,
        ))
    return cases


def build_action_cases(params, params_by_label=None, constants_by_alias=None):
    params_by_label = params_by_label or {}
    constants_by_alias = constants_by_alias or {}
    cases = []
    for param in sorted(params, key=lambda p: p.index if p.index is not None else 0x100):
        action_name = ACTION_NAMES.get(param.action)
        if action_name is None:
            continue
        if param.label == 'pSignatureIdx':
            continue
        value = _sample_value(param)
        expect_vars = {}
        if action_name.startswith('software-init'):
            expect_vars = _software_init_expectations(
                params_by_label, constants_by_alias, param.label, value)
        elif action_name.startswith('hardware-init'):
            expect_vars = _hardware_init_expectations(
                params_by_label, constants_by_alias, param.label, value)
        cases.append(EEPROMCase(
            name='{} side-effect smoke: {}'.format(action_name, param.label),
            parameter=param.label,
            value=value,
            expect_raw=value,
            expect_vars=expect_vars,
        ))
    return cases


def build_timeout_cases(params_by_label, constants_by_alias=None):
    constants_by_alias = constants_by_alias or {}
    ticks_constant = constants_by_alias.get('idxTicks0PerSecond')
    ticks_per_second = ticks_constant.value if ticks_constant is not None else None

    cases = []
    for param_label, var_label in TIMEOUT_VAR_CASES:
        param = params_by_label.get(param_label)
        if param is None:
            continue
        value = _sample_value(param)
        expected = None
        if ticks_per_second is not None:
            expected = (value * ticks_per_second) & 0xFFFF
        cases.append(EEPROMCase(
            name='timeout side-effect: {} updates {}'.format(param_label, var_label),
            parameter=param_label,
            value=value,
            expect_raw=value,
            expect_vars={var_label: expected},
        ))
    return cases


def build_pseudo_range_cases(range_params):
    cases = []
    for range_name in ('saved_trips', 'display_pages', 'display_cursor', 'menu_height'):
        params = sorted(range_params.get(range_name, {}).values(), key=lambda p: p.index if p.index is not None else 0x100)
        if not params:
            continue
        param = params[0]
        value = _sample_value(param)
        cases.append(EEPROMCase(
            name='{} pseudo-range round-trip: {}'.format(range_name, param.label),
            parameter=param.label,
            value=value,
            expect_raw=value,
        ))
    return cases


def build_cases(params_by_label, range_params=None, constants_by_alias=None):
    range_params = range_params or {}
    params = list(params_by_label.values())
    cases = []
    cases.extend(build_storage_cases(params))
    cases.extend(build_action_cases(params, params_by_label, constants_by_alias))
    cases.extend(build_timeout_cases(params_by_label, constants_by_alias))
    cases.extend(build_pseudo_range_cases(range_params))
    return cases
