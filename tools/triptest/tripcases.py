"""Trip-function harness case definitions."""


MEASUREMENT_VALUES = {
    'rvVSSpulseIdx': 12345,
    'rvVSScycleIdx': 500000,
    'rvInjPulseIdx': 2345,
    'rvInjCycleIdx': 750000,
    'rvEngCycleIdx': 1000000,
}


DIRECT_FUNCTION_CASES = [
    ('tInjectorPulseCount', 'rvInjPulseIdx'),
    ('tVSSpulseEdgeCount', 'rvVSSpulseIdx'),
]

class TripCase(object):
    def __init__(self, name, function_label, expected_value):
        self.name = name
        self.function_label = function_label
        self.expected_value = expected_value


def build_direct_cases():
    cases = []
    for function_label, measurement_label in DIRECT_FUNCTION_CASES:
        cases.append(TripCase(
            name='direct function output: {}'.format(function_label),
            function_label=function_label,
            expected_value=MEASUREMENT_VALUES[measurement_label],
        ))
    return cases
