"""SWEET64 SetFlag/ClearFlag and overflow-branch regression cases.

SetFlag/ClearFlag take a raw flag bitmask operand and OR/AND it into the
processor flags (firmware flagSet: flags |= operand / flags &= ~operand).
Flag bits: carry=0x01, zero=0x02, minus=0x04, overflow=0x08.

These also let us exercise the overflow branches (BranchIfVset/Vclear), which
arithmetic does not readily produce (div-by-zero does not set overflow), by
setting/clearing the overflow flag directly first.

expect_flags checks only the named bits, so the trace bits (0xC0) present
during a ^T run are ignored.
"""

from s64testcase import S64Case, FLAG_CARRY, FLAG_ZERO, FLAG_MINUS, FLAG_OVERFLOW

CASES = [
    # --- SetFlag / ClearFlag directly ---
    S64Case(
        name="SetFlag overflow (0x08)",
        program=["SetFlag 08", "Done"],
        expect_flags={FLAG_OVERFLOW: True},
    ),
    S64Case(
        name="SetFlag all arithmetic flags (0x0F)",
        program=["SetFlag 0F", "Done"],
        expect_flags={FLAG_CARRY: True, FLAG_ZERO: True,
                      FLAG_MINUS: True, FLAG_OVERFLOW: True},
    ),
    S64Case(
        name="ClearFlag minus only: set 0x0F then clear 0x04",
        program=["SetFlag 0F", "ClearFlag 04", "Done"],
        expect_flags={FLAG_MINUS: False, FLAG_CARRY: True,
                      FLAG_ZERO: True, FLAG_OVERFLOW: True},
    ),
]


def _overflow_branch_program(setup_instr, branch_instr):
    return [
        "LdRegByte 22 55",          # reg2 = 0x55 (taken marker)
        setup_instr,                # set or clear the overflow flag
        "{} taken".format(branch_instr),
        "LdRegByte 22 AA",          # reg2 = 0xAA (fall-through marker)
        "taken:",
        "Done",
    ]


CASES += [
    S64Case(
        name="BranchIfVset taken (overflow set)",
        program=_overflow_branch_program("SetFlag 08", "BranchIfVset"),
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfVset not taken (overflow clear)",
        program=_overflow_branch_program("ClearFlag 08", "BranchIfVset"),
        expect={2: 0xAA},
    ),
    S64Case(
        name="BranchIfVclear taken (overflow clear)",
        program=_overflow_branch_program("ClearFlag 08", "BranchIfVclear"),
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfVclear not taken (overflow set)",
        program=_overflow_branch_program("SetFlag 08", "BranchIfVclear"),
        expect={2: 0xAA},
    ),
]
