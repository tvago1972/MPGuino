"""SWEET64 conditional-branch regression cases.

Branch targets use symbolic labels (resolved by s64assembler.resolve_labels);
a line 'name:' marks a target address.

Each program loads reg2 with a marker (0x55), sets flags from reg1 via
'SubByteFromX 1 00' (subtracting 0 leaves reg1 unchanged but sets zero/carry/
minus from its value), then branches over a second load that would set reg2 to
0xAA.  So reg2 == 0x55 means the branch was taken (the 0xAA load was skipped),
and reg2 == 0xAA means it fell through.
"""

from s64testcase import S64Case


def _branch_program(branch_instr):
    return [
        "LdRegByte 22 55",          # reg2 = 0x55 (taken marker)
        "SubByteFromX 1 00",        # set flags from reg1 (reg1 unchanged)
        "{} taken".format(branch_instr),
        "LdRegByte 22 AA",          # reg2 = 0xAA (fall-through marker)
        "taken:",
        "Done",
    ]


CASES = [
    # --- zero flag ---
    S64Case(
        name="BranchIfZset taken (reg1=0)",
        program=_branch_program("BranchIfZset"),
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfZset not taken (reg1=5)",
        program=_branch_program("BranchIfZset"),
        inputs={1: 0x05},
        expect={2: 0xAA},
    ),
    S64Case(
        name="BranchIfZclear taken (reg1=5)",
        program=_branch_program("BranchIfZclear"),
        inputs={1: 0x05},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfZclear not taken (reg1=0)",
        program=_branch_program("BranchIfZclear"),
        expect={2: 0xAA},
    ),
]


# --- carry flag: set flags via add-wrap instead of subtract ---
def _carry_program(branch_instr):
    return [
        "LdRegByte 22 55",          # reg2 = 0x55 (taken marker)
        "AddByteToX 1 01",          # reg1 += 1 (sets carry on wrap)
        "{} taken".format(branch_instr),
        "LdRegByte 22 AA",          # reg2 = 0xAA (fall-through marker)
        "taken:",
        "Done",
    ]


CASES += [
    S64Case(
        name="BranchIfCset taken (0xFFFF...FF + 1 wraps, carry set)",
        program=_carry_program("BranchIfCset"),
        inputs={1: 0xFFFFFFFFFFFFFFFF},
        expect={2: 0x55, 1: 0x00},
    ),
    S64Case(
        name="BranchIfCset not taken (0x10 + 1, no carry)",
        program=_carry_program("BranchIfCset"),
        inputs={1: 0x10},
        expect={2: 0xAA, 1: 0x11},
    ),
]
