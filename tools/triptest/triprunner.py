"""Single-session trip-function regression harness runner."""

import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
S64TEST_DIR = os.path.abspath(os.path.join(HERE, '..', 's64test'))
if S64TEST_DIR not in sys.path:
    sys.path.insert(0, S64TEST_DIR)

from s64terminal import S64Terminal, S64TerminalError

from tripcases import MEASUREMENT_VALUES, build_direct_cases
from triplisting import (
    read_trip_measurements,
    read_trip_functions,
    set_trip_measurements,
)


class TripResult(object):
    def __init__(self, name, passed, failures=None):
        self.name = name
        self.passed = passed
        self.failures = failures or []


def _require_labels(by_label, listing_name):
    real_labels = [label for label in by_label if not label.startswith('item_')]
    if not real_labels:
        raise S64TerminalError(
            'trip harness requires useDebugTerminalLabels; {} listing has no labels'.format(listing_name))


def _ordered_measurement_values(measurements_by_label):
    values = []
    missing = []
    for label, value in MEASUREMENT_VALUES.items():
        item = measurements_by_label.get(label)
        if item is None:
            missing.append(label)
        else:
            values.append((item.index, value))
    if missing:
        raise S64TerminalError('trip measurement label(s) not found: {}'.format(', '.join(missing)))
    values.sort()
    start = values[0][0]
    expected_indexes = list(range(start, start + len(values)))
    actual_indexes = [idx for idx, _ in values]
    if actual_indexes != expected_indexes:
        raise S64TerminalError('trip measurement indexes are not contiguous: {}'.format(actual_indexes))
    return start, [value for _, value in values]


def _load_terminal_trip(term):
    _, measurements_by_label, unparsed = read_trip_measurements(term)
    if not measurements_by_label:
        detail = ': {}'.format(', '.join(repr(line) for line in unparsed)) if unparsed else ''
        raise S64TerminalError('T listing produced no parseable trip measurements{}'.format(detail))
    start, values = _ordered_measurement_values(measurements_by_label)
    set_trip_measurements(term, start, values)
    _, updated_by_label, update_unparsed = read_trip_measurements(term)
    return updated_by_label, unparsed + update_unparsed


def _run_measurement_round_trip(measurements_by_label):
    failures = []
    for label, expected in sorted(MEASUREMENT_VALUES.items()):
        item = measurements_by_label.get(label)
        if item is None:
            failures.append('measurement {!r} not reported'.format(label))
        elif item.raw_value != expected:
            failures.append('{}: got 0x{:X}, expected 0x{:X}'.format(label, item.raw_value, expected))
    return TripResult('terminal trip measurement round-trip', not failures, failures)


def _run_function_case(functions_by_label, case):
    item = functions_by_label.get(case.function_label)
    if item is None:
        return TripResult(case.name, False, ['function {!r} not reported'.format(case.function_label)])
    if item.leading_int != case.expected_value:
        return TripResult(case.name, False, [
            '{}: display {!r} parsed as {}, expected {}'.format(
                case.function_label, item.display, item.leading_int, case.expected_value)
        ])
    return TripResult(case.name, True)


def _run_display_function_case(function_output):
    failures = []
    if not function_output.display.strip():
        failures.append('{}: blank display output'.format(function_output.label))
    elif function_output.leading_int is None:
        failures.append('{}: display {!r} contains no numeric value'.format(
            function_output.label, function_output.display))
    return TripResult(
        'display function evaluated: {}'.format(function_output.label),
        not failures,
        failures)


def print_discovery(term):
    _, measurements_by_label, trip_unparsed = read_trip_measurements(term)
    _, functions_by_label, func_unparsed = read_trip_functions(term)
    if measurements_by_label:
        _require_labels(measurements_by_label, 'T')
    if functions_by_label:
        _require_labels(functions_by_label, 'L')

    print('Trip measurements:')
    for item in sorted(measurements_by_label.values(), key=lambda x: x.index):
        print('  {:02X}: {:>16X} {}'.format(item.index, item.raw_value, item.label))

    print()
    print('Trip functions:')
    for item in sorted(functions_by_label.values(), key=lambda x: x.index):
        print('  {:02X}: {:24s} {}'.format(item.index, item.display, item.label))

    unparsed = (
        [('T', line) for line in trip_unparsed] +
        [('L', line) for line in func_unparsed])
    if unparsed:
        print()
        print('Unparsed monitor lines:')
        for name, line in unparsed:
            print('  {}: {!r}'.format(name, line))


def run_suite(port, baud, discover_only=False):
    passed = 0
    failed = 0

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()
        if discover_only:
            print_discovery(term)
            return passed, failed

        measurements_by_label, unparsed = _load_terminal_trip(term)
        _, functions_by_label, func_unparsed = read_trip_functions(term)
        _require_labels(functions_by_label, 'L')

        for line in unparsed:
            print('WARN  unparsed T line: {!r}'.format(line))
        for line in func_unparsed:
            print('WARN  unparsed L line: {!r}'.format(line))

        cases = [_run_measurement_round_trip(measurements_by_label)]
        direct_labels = set()
        for case in build_direct_cases():
            cases.append(_run_function_case(functions_by_label, case))
            direct_labels.add(case.function_label)
        for item in sorted(functions_by_label.values(), key=lambda x: x.index):
            if item.label not in direct_labels:
                cases.append(_run_display_function_case(item))

        print('Running {} trip-function case(s):'.format(len(cases)))
        for result in cases:
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
        return 2
    discover_only = '--discover-only' in argv[3:]
    _, failed = run_suite(argv[1], int(argv[2]), discover_only=discover_only)
    return 1 if failed else 0
