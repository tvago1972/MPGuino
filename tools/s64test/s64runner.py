"""Single-session runner for SWEET64 regression test cases.

Opens the serial port once (which resets the board to a clean baseline) and
runs every case within that one session, since reopening would reset the
board mid-suite.  Prints a per-case PASS/FAIL line and a summary.
"""

import sys
from s64terminal import S64Terminal, S64TerminalError
from s64testcase import run_case


def run_suite(port, baud, cases):
    """Run all cases in a single session.  Returns (passed, failed)."""
    passed = 0
    failed = 0

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()

        for case in cases:
            result = run_case(term, case)
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


def main(cases):
    """Entry point helper: parse argv and run the given case list."""
    if len(sys.argv) < 3:
        print('Usage: {} <port> <baud>'.format(sys.argv[0]))
        sys.exit(1)

    port = sys.argv[1]
    baud = int(sys.argv[2])

    try:
        _, failed = run_suite(port, baud, cases)
    except S64TerminalError as e:
        print('FATAL: {}'.format(e))
        sys.exit(2)

    sys.exit(1 if failed else 0)
