"""SWEET64 DoBCDadjust regression cases (10-digit format, index 0).

DoBCDadjust converts a binary value in regY (source) into a base-100 "BCD"
byte layout in regX (target), consuming regY (it is divided down to 0).
Operand: 'DoBCDadjust <regbyte> <fmtindex>'; X=low nibble (target),
Y=high nibble (source).  '21' -> target reg1, source reg2.

For the 10-digit format (s64BCDformatList index 0):
  leading-zero char = ' ' (0x20), total BCD byte length = 0x05, 4 divisors of 100.
The engine stores, into the target register's bytes:
  u08[7] = 0x20 (leading-zero char)
  u08[6] = 0x05 (length)
  u08[4],[3],[2],[1] = successive (value % 100), dividing by 100 each step
  u08[5], u08[0] = untouched (0 from the zeroed baseline)
So the 64-bit result is 0x2005_00<d4><d3><d2><d1>_00 where d1..d4 are the
base-100 digit-pairs (low pair in byte 1). regY ends at 0.
"""

from s64testcase import S64Case

FMT_10DIGIT = "00"


def _bcd_program():
    return ["DoBCDadjust 21 {}".format(FMT_10DIGIT), "Done"]


CASES = [
    S64Case(
        name="DoBCDadjust 10-digit: 0 -> metadata only",
        program=_bcd_program(),
        inputs={2: 0},
        expect={1: 0x2005000000000000, 2: 0},
    ),
    S64Case(
        name="DoBCDadjust 10-digit: 1 -> byte[4] = 0x01 (least-significant base-100 pair)",
        program=_bcd_program(),
        inputs={2: 1},
        expect={1: 0x2005000100000000, 2: 0},
    ),
    S64Case(
        name="DoBCDadjust 10-digit: 12345678 -> 78 56 34 12 base-100",
        program=_bcd_program(),
        inputs={2: 12345678},
        expect={1: 0x2005004E38220C00, 2: 0},
    ),
]
