"""SWEET64 processor-flag regression cases.

Flags are checked on si64reg8flags after the program completes; only the
bits named in expect_flags are tested (trace bits 0xC0 are ignored).
Uses arithmetic ops already validated in cases_arithmetic.

ZERO semantics are unambiguous (result == 0).  MINUS and CARRY are included
as exploratory checks to pin down the engine's exact behavior; adjust if the
hardware disagrees.
"""

from s64testcase import S64Case, FLAG_CARRY, FLAG_ZERO, FLAG_MINUS, FLAG_OVERFLOW

CASES = [
    S64Case(
        name="ZERO set: 0x10 - 0x10 = 0",
        program=["LdRegByte 11 10", "SubByteFromX 1 10", "Done"],
        expect={1: 0x00},
        expect_flags={FLAG_ZERO: True},
    ),
    S64Case(
        name="ZERO clear: 0x10 - 0x05 = 0x0B",
        program=["LdRegByte 11 10", "SubByteFromX 1 05", "Done"],
        expect={1: 0x0B},
        expect_flags={FLAG_ZERO: False},
    ),
    S64Case(
        name="ZERO+CARRY: 0xFFFF...FF + 1 wraps to 0",
        program=["AddByteToX 1 01", "Done"],
        inputs={1: 0xFFFFFFFFFFFFFFFF},
        expect={1: 0x00},
        expect_flags={FLAG_ZERO: True, FLAG_CARRY: True},
    ),
    S64Case(
        name="MINUS: 0x00 - 0x01 goes negative",
        program=["SubByteFromX 1 01", "Done"],
        inputs={1: 0x00},
        expect={1: 0xFFFFFFFFFFFFFFFF},
        expect_flags={FLAG_MINUS: True, FLAG_ZERO: False},
    ),
]
