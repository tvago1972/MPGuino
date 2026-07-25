"""SWEET64 comparison- and minus-branch regression cases.

Comparison branches follow CmpXtoY, which computes X - Y (X = low operand
nibble, Y = high nibble) and sets carry on borrow (X < Y), without storing.

IMPORTANT - the LT/GTorE branch mnemonics are INVERTED relative to their
names, and this is intended (confirmed by the author). With CmpXtoY 21
(X=reg1, Y=reg2):
    BranchIfLT     is taken when X >  Y   (NOT "less than")
    BranchIfGTorE  is taken when X <= Y   (NOT "greater or equal")
The firmware logic (sweet64.ino): LT = (!zero && !carry); GTorE = (zero ||
carry); with carry = borrow. MPGuino's own SWEET64 programs rely on this, so
the behavior is correct as implemented; only the naming is counterintuitive.

Comparison tests put operands in reg1 (X) and reg2 (Y) and use reg3 as the
taken/fall-through marker (0x55 = branch taken, 0xAA = fell through).
Minus-flag tests set flags with SubByteFromX and use reg2 as the marker.
"""

from s64testcase import S64Case


def _cmp_program(branch_instr):
    # compare X=reg1, Y=reg2; reg3 holds the marker
    return [
        "LdRegByte 33 55",          # reg3 = 0x55 (taken marker)
        "CmpXtoY 21",               # compare reg1 (X) - reg2 (Y)
        "{} taken".format(branch_instr),
        "LdRegByte 33 AA",          # reg3 = 0xAA (fall-through marker)
        "taken:",
        "Done",
    ]


def _minus_program(branch_instr):
    # subtract sets minus when reg1 - 1 goes negative; reg2 holds the marker
    return [
        "LdRegByte 22 55",          # reg2 = 0x55 (taken marker)
        "SubByteFromX 1 01",        # reg1 -= 1 (sets minus when reg1 was 0)
        "{} taken".format(branch_instr),
        "LdRegByte 22 AA",          # reg2 = 0xAA (fall-through marker)
        "taken:",
        "Done",
    ]


CASES = [
    # --- CmpXtoY + BranchIfLT: taken when X > Y (inverted name, intended) ---
    S64Case(
        name="BranchIfLT taken when X>Y (reg1=0x20, reg2=0x10)",
        program=_cmp_program("BranchIfLT"),
        inputs={1: 0x20, 2: 0x10},
        expect={3: 0x55},
    ),
    S64Case(
        name="BranchIfLT not taken when X<Y (reg1=0x10, reg2=0x20)",
        program=_cmp_program("BranchIfLT"),
        inputs={1: 0x10, 2: 0x20},
        expect={3: 0xAA},
    ),
    S64Case(
        name="BranchIfLT not taken when X==Y (needs strictly greater)",
        program=_cmp_program("BranchIfLT"),
        inputs={1: 0x10, 2: 0x10},
        expect={3: 0xAA},
    ),
    # --- CmpXtoY + BranchIfGTorE: taken when X <= Y (inverted name, intended) ---
    S64Case(
        name="BranchIfGTorE taken when X<Y (reg1=0x10, reg2=0x20)",
        program=_cmp_program("BranchIfGTorE"),
        inputs={1: 0x10, 2: 0x20},
        expect={3: 0x55},
    ),
    S64Case(
        name="BranchIfGTorE taken when X==Y (reg1=0x10, reg2=0x10)",
        program=_cmp_program("BranchIfGTorE"),
        inputs={1: 0x10, 2: 0x10},
        expect={3: 0x55},
    ),
    S64Case(
        name="BranchIfGTorE not taken when X>Y (reg1=0x20, reg2=0x10)",
        program=_cmp_program("BranchIfGTorE"),
        inputs={1: 0x20, 2: 0x10},
        expect={3: 0xAA},
    ),
    # --- minus flag: BranchIfMset (taken when result < 0) ---
    S64Case(
        name="BranchIfMset taken (0x00 - 1 < 0)",
        program=_minus_program("BranchIfMset"),
        inputs={1: 0x00},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfMset not taken (0x10 - 1 >= 0)",
        program=_minus_program("BranchIfMset"),
        inputs={1: 0x10},
        expect={2: 0xAA},
    ),
    # --- minus flag: BranchIfMclear (taken when result >= 0) ---
    S64Case(
        name="BranchIfMclear taken (0x10 - 1 >= 0)",
        program=_minus_program("BranchIfMclear"),
        inputs={1: 0x10},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfMclear not taken (0x00 - 1 < 0)",
        program=_minus_program("BranchIfMclear"),
        inputs={1: 0x00},
        expect={2: 0xAA},
    ),
]
