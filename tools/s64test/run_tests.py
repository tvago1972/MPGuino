#!/usr/bin/env python3
"""SWEET64 opcode regression test runner.

Opens the debug-terminal serial port once and runs every registered test
case in that single session (the board resets on port open, so reopening
mid-suite is avoided).

Usage:
    python run_tests.py <port> <baud>

Example:
    python run_tests.py COM3 38400
"""

from s64runner import main

import cases_arithmetic
import cases_flags
import cases_branch
import cases_compare
import cases_flow
import cases_shiftmuldiv
import cases_multibyte
import cases_edge
import cases_setflag
import cases_index
import cases_mode
import cases_loadstore
import cases_bcd
import cases_eeprom
import cases_const
import cases_indexed
import cases_trip
import cases_call

# all case tables to run, in order
ALL_CASES = []
ALL_CASES += cases_arithmetic.CASES
ALL_CASES += cases_flags.CASES
ALL_CASES += cases_branch.CASES
ALL_CASES += cases_compare.CASES
ALL_CASES += cases_flow.CASES
ALL_CASES += cases_shiftmuldiv.CASES
ALL_CASES += cases_multibyte.CASES
ALL_CASES += cases_edge.CASES
ALL_CASES += cases_setflag.CASES
ALL_CASES += cases_index.CASES
ALL_CASES += cases_mode.CASES
ALL_CASES += cases_loadstore.CASES
ALL_CASES += cases_bcd.CASES
ALL_CASES += cases_eeprom.CASES
ALL_CASES += cases_const.CASES
ALL_CASES += cases_indexed.CASES
ALL_CASES += cases_trip.CASES
ALL_CASES += cases_call.CASES


if __name__ == '__main__':
    main(ALL_CASES)
