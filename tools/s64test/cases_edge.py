"""SWEET64 edge-case regression cases.

Divide-by-zero: the engine does not raise a SWEET64 error or set a flag;
div64 writes an overflow sentinel of 0xFFFF...FFFF into BOTH the quotient
(reg2) and the remainder (reg1). These cases lock in that behavior and also
confirm divide-by-zero is not reported as a SWEET64 error (expect_error
defaults to False).

KNOWN LIMITATION (intended, not a bug): ideally div64 would set the overflow
flag on divide-by-zero, but the hand-written assembly implementation does not
support doing so, so the sentinel value is the only signal. If div64 ever
gains overflow-flag support, add an expect_flags={FLAG_OVERFLOW: True} check
to these cases.

NOTE: genuine si64reg8error triggers (bad opcode, stack overflow, bad PC)
are either rejected by the assembler at assembly time or require a raw
RAM-poke capability the harness does not yet have, so a positive
expect_error case is deferred until that layer exists.
"""

from s64testcase import S64Case

SENTINEL = 0xFFFFFFFFFFFFFFFF

CASES = [
    S64Case(
        name="Div2byByte by zero: 0x1234 / 0 -> overflow sentinel",
        program=["Div2byByte 00", "Done"],
        inputs={2: 0x1234},
        expect={2: SENTINEL, 1: SENTINEL},
    ),
    S64Case(
        name="Div2byByte by zero: 0 / 0 -> overflow sentinel",
        program=["Div2byByte 00", "Done"],
        inputs={2: 0x00},
        expect={2: SENTINEL, 1: SENTINEL},
    ),
    S64Case(
        name="Div2by1 by zero (reg1=0): 0x1234 / 0 -> overflow sentinel",
        program=["Div2by1", "Done"],
        inputs={2: 0x1234},     # reg1 is 0 from the zeroed baseline
        expect={2: SENTINEL, 1: SENTINEL},
    ),
]
