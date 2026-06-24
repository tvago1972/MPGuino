"""SWEET64 Isqrt (integer square root) regression cases.

Isqrt replaces register X's low 32 bits with the integer square root of those
bits: regX.u32[0] = iSqrt(regX.u32[0]).  Operand is a register byte
(X = low nibble); 'Isqrt 11' operates on reg1.

This opcode is config-gated (useIsqrt), now enabled in the debug-monitor
build (configs.h).  Test values use perfect squares and a non-square to check
floor behavior, plus full 32-bit range values.

Full unsigned 32-bit range is exercised: this regression suite caught the
assembly iSqrt using a signed compare (brlt) where the C reference uses an
unsigned '>=', which gave wrong results above 2^31 (sqrt(0x7FFFFFFF) -> 65535,
sqrt(0xFFFFFFFF) -> 0).  Fixed to brlo (unsigned) in sweet64.ino; these cases
guard against regression.
"""

from s64testcase import S64Case

CASES = [
    S64Case(
        name="Isqrt: sqrt(10000) = 100",
        program=["Isqrt 11", "Done"],
        inputs={1: 10000},
        expect={1: 100},
    ),
    S64Case(
        name="Isqrt: sqrt(144) = 12",
        program=["Isqrt 11", "Done"],
        inputs={1: 144},
        expect={1: 12},
    ),
    S64Case(
        name="Isqrt: sqrt(9999) = 99 (floor)",
        program=["Isqrt 11", "Done"],
        inputs={1: 9999},
        expect={1: 99},
    ),
    S64Case(
        name="Isqrt: sqrt(0) = 0",
        program=["Isqrt 11", "Done"],
        inputs={1: 0},
        expect={1: 0},
    ),
    S64Case(
        name="Isqrt: sqrt(0x7FFFFFFF) = 46340",
        program=["Isqrt 11", "Done"],
        inputs={1: 0x7FFFFFFF},
        expect={1: 46340},
    ),
    S64Case(
        name="Isqrt: sqrt(0xFFFFFFFF) = 65535 (full unsigned 32-bit range)",
        program=["Isqrt 11", "Done"],
        inputs={1: 0xFFFFFFFF},
        expect={1: 65535},
    ),
]
