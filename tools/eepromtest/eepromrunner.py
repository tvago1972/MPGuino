"""Single-session EEPROM regression harness runner."""

import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
S64TEST_DIR = os.path.abspath(os.path.join(HERE, '..', 's64test'))
if S64TEST_DIR not in sys.path:
    sys.path.insert(0, S64TEST_DIR)

from s64terminal import S64Terminal, S64TerminalError
from s64variables import read_variables
from s64constants import read_constants

from eepromcases import build_cases
from eepromlisting import (
    read_parameter,
    read_all_parameter_ranges,
    set_parameter,
    parameter_groups,
)


class EEPROMResult(object):
    def __init__(self, name, passed, failures=None):
        self.name = name
        self.passed = passed
        self.failures = failures or []


def _require_labels(params_by_label):
    real_labels = [label for label in params_by_label if not label.startswith('param_')]
    if not real_labels:
        raise S64TerminalError(
            'EEPROM harness requires useDebugTerminalLabels; P listing has no parameter labels')


def _merge_range_labels(range_params):
    merged = {}
    for _, params in sorted(range_params.items()):
        for param in params.values():
            merged[param.label] = param
    return merged


def _refresh_variables(term):
    _, by_label = read_variables(term)
    return by_label


def _read_constants(term):
    _, by_alias = read_constants(term)
    return by_alias


def _run_case(term, case, params_by_label):
    failures = []
    param = params_by_label.get(case.parameter)
    if param is None:
        return EEPROMResult(case.name, False, ['parameter {!r} not found'.format(case.parameter)])

    original = param.raw_value
    restore_value = original
    try:
        set_parameter(term, param.index, case.value)
        updated, unparsed = read_parameter(term, param.index)
        if updated is None:
            failures.append('parameter {!r} disappeared after write'.format(case.parameter))
            for line in unparsed:
                failures.append('single-read unparsed: {!r}'.format(line))
        elif updated.label != case.parameter:
            failures.append('single-read index 0x{:02X}: got label {}, expected {}'.format(
                param.index, updated.label, case.parameter))
        elif updated.raw_value is None:
            failures.append('param {}: raw bytes not reported'.format(case.parameter))
            if updated.source_line:
                failures.append('single-read parsed line: {!r}'.format(updated.source_line))
            for line in unparsed:
                failures.append('single-read unparsed: {!r}'.format(line))
        elif case.expect_raw is not None and updated.raw_value != case.expect_raw:
            failures.append('param {}: raw got 0x{:X}, expected 0x{:X}'.format(
                case.parameter, updated.raw_value, case.expect_raw))

        if case.expect_vars:
            vars_by_label = _refresh_variables(term)
            for label, expected in sorted(case.expect_vars.items()):
                var = vars_by_label.get(label)
                if var is None:
                    failures.append('program var {!r} not reported'.format(label))
                elif expected is not None and var.hex_value != expected:
                    failures.append('var {}: got 0x{:X}, expected 0x{:X}'.format(
                        label, var.hex_value, expected))
    except (S64TerminalError, ValueError) as e:
        failures.append('terminal error: {}'.format(e))
    finally:
        if case.restore and restore_value is not None:
            try:
                set_parameter(term, param.index, restore_value)
            except S64TerminalError as e:
                failures.append('restore failed for {}: {}'.format(case.parameter, e))

    return EEPROMResult(case.name, not failures, failures)


def _update_cached_parameter(term, params_by_label, parameter):
    updated, _ = read_parameter(term, parameter.index)
    if updated is not None:
        params_by_label[updated.label] = updated
        if updated.label != parameter.label:
            params_by_label.pop(parameter.label, None)


def print_discovery(term):
    ranges, unparsed = read_all_parameter_ranges(term)
    settings = list(ranges.get('settings', {}).values())
    if settings:
        _require_labels(ranges.get('settings', {}))
    groups = parameter_groups(settings)

    print('Parameter ranges:')
    for name in ('settings', 'saved_trips', 'display_pages', 'display_cursor', 'menu_height'):
        count = len(ranges.get(name, {}))
        print('  {:14s} {}'.format(name + ':', count))
    if unparsed:
        print()
        print('Unparsed monitor lines:')
        for name, lines in sorted(unparsed.items()):
            for line in lines:
                print('  {}: {!r}'.format(name, line))

    print()
    print('Flag groups from settings range:')
    for name in ('hardware_init', 'software_init', 'fuel_calc', 'display_change', 'metric_mode'):
        labels = [p.label for p in groups[name]]
        print('  {:16s} {:2d} {}'.format(name + ':', len(labels), ', '.join(labels)))
    return ranges, unparsed


def run_suite(port, baud, discover_only=False):
    passed = 0
    failed = 0

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()
        range_params, unparsed_by_range = print_discovery(term)
        if discover_only:
            return passed, failed

        params_by_label = _merge_range_labels(range_params)
        _require_labels(params_by_label)
        constants_by_alias = _read_constants(term)
        for range_name, lines in sorted(unparsed_by_range.items()):
            for line in lines:
                print('WARN  unparsed {} line: {!r}'.format(range_name, line))

        cases = build_cases(params_by_label, range_params, constants_by_alias)
        print()
        print('Running {} EEPROM case(s):'.format(len(cases)))
        for case in cases:
            param = params_by_label.get(case.parameter)
            result = _run_case(term, case, params_by_label)
            if param is not None and ((not case.restore) or (not result.passed)):
                _update_cached_parameter(term, params_by_label, param)
            if result.passed:
                passed += 1
                print('PASS  {}'.format(result.name))
            else:
                failed += 1
                print('FAIL  {}'.format(result.name))
                for msg in result.failures:
                    print('        {}'.format(msg))

    print()
    print('{} passed, {} failed, {} total'.format(passed, failed, passed + failed))
    return passed, failed


def main(argv=None):
    argv = argv or sys.argv
    if len(argv) < 3:
        print('Usage: {} <port> <baud> [--discover-only]'.format(argv[0]))
        return 1
    port = argv[1]
    baud = int(argv[2])
    discover_only = '--discover-only' in argv[3:]
    try:
        _, failed = run_suite(port, baud, discover_only=discover_only)
    except S64TerminalError as e:
        print('FATAL: {}'.format(e))
        return 2
    return 1 if failed else 0
