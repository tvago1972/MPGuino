"""SWEET64 DoBCDadjust regression cases (10-digit, HHMMSS, H9MMSS formats).

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

# BCD format indices into s64BCDformatList.  HHMMSS is always compiled (it is no
# longer gated on useClockSupport), so these indices are stable on every build.
FMT_10DIGIT = "00"
FMT_HHMMSS = "01"   # 3 bytes, divisors 60/60/24 (hours wrap at 24)
FMT_H9MMSS = "02"   # 3 bytes, divisors 60/60/100 (hours wrap at 100)


def _bcd_program(fmt=FMT_10DIGIT):
    return ["DoBCDadjust 21 {}".format(fmt), "Done"]


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

    # HHMMSS (divisors 60/60/24): seconds -> u08[2], minutes -> u08[1],
    # hours (mod 24) -> u08[0]; leading char '0' (0x30), length 0x03.
    # Result layout: 0x3003_0000_00 <ss> <mm> <hh>.
    S64Case(
        name="DoBCDadjust HHMMSS: 0 -> metadata only",
        program=_bcd_program(FMT_HHMMSS),
        inputs={2: 0},
        expect={1: 0x3003000000000000, 2: 0},
    ),
    S64Case(
        name="DoBCDadjust HHMMSS: 7384s -> 02:03:04",
        program=_bcd_program(FMT_HHMMSS),
        inputs={2: 7384},          # 2h 3m 4s
        expect={1: 0x3003000000040302, 2: 0},
    ),
    S64Case(
        name="DoBCDadjust HHMMSS: 90000s -> 25h wraps to 01h (mod 24)",
        program=_bcd_program(FMT_HHMMSS),
        inputs={2: 90000},         # 25h 0m 0s; hours wrap to 1, leftover 1 in source
        expect={1: 0x3003000000000001, 2: 1},
    ),

    # H9MMSS (divisors 60/60/100): same layout, but hours wrap at 100 not 24,
    # so a 25h value reads back as 0x19 (25) instead of wrapping.
    S64Case(
        name="DoBCDadjust H9MMSS: 7384s -> 02:03:04 (matches HHMMSS in range)",
        program=_bcd_program(FMT_H9MMSS),
        inputs={2: 7384},
        expect={1: 0x3003000000040302, 2: 0},
    ),
    S64Case(
        name="DoBCDadjust H9MMSS: 90000s -> 25h shown as 0x19 (mod 100)",
        program=_bcd_program(FMT_H9MMSS),
        inputs={2: 90000},
        expect={1: 0x3003000000000019, 2: 0},
    ),
]
