"""SWEET64 index-register (si64reg8trip) regression cases.

LxdI    : load index = immediate operand (also sets zero/minus from result)
TestIndex: set zero/minus flags from the current index value
CmpIndex : compare immediate operand to index -- carry if operand < index,
           zero if operand == index, minus if (operand - index) & 0x80
AddIndex : add immediate to the index register

The index is read back via expect_reg8 on label 'si64reg8trip'. To isolate
TestIndex/CmpIndex flag effects from the load that precedes them, the opposite
flag state is forced with SetFlag/ClearFlag before the op under test.
"""

from s64testcase import S64Case, FLAG_CARRY, FLAG_ZERO, FLAG_MINUS

CASES = [
    # --- LxdI loads the index ---
    S64Case(
        name="LxdI: load index = 0x05",
        program=["LxdI 05", "Done"],
        expect_reg8={"si64reg8trip": 0x05},
    ),
    # --- AddIndex adds to the index ---
    S64Case(
        name="AddIndex: 0x05 + 0x03 = 0x08",
        program=["LxdI 05", "AddIndex 03", "Done"],
        expect_reg8={"si64reg8trip": 0x08},
    ),
    # --- TestIndex sets flags from the index value ---
    S64Case(
        name="TestIndex: index 0 -> zero set",
        program=["LxdI 00", "ClearFlag 02", "TestIndex", "Done"],
        expect_flags={FLAG_ZERO: True},
    ),
    S64Case(
        name="TestIndex: index 0x05 -> zero clear",
        program=["LxdI 05", "SetFlag 02", "TestIndex", "Done"],
        expect_flags={FLAG_ZERO: False},
    ),
    S64Case(
        name="TestIndex: index 0x80 -> minus set",
        program=["LxdI 80", "ClearFlag 04", "TestIndex", "Done"],
        expect_flags={FLAG_MINUS: True},
    ),
    # --- CmpIndex compares immediate operand to index ---
    S64Case(
        name="CmpIndex: operand 5 < index 0x10 -> carry set, not equal, minus set",
        program=["LxdI 10", "CmpIndex 05", "Done"],
        expect_flags={FLAG_CARRY: True, FLAG_ZERO: False, FLAG_MINUS: True},
    ),
    S64Case(
        name="CmpIndex: operand == index (0x10) -> zero set, carry clear",
        program=["LxdI 10", "CmpIndex 10", "Done"],
        expect_flags={FLAG_CARRY: False, FLAG_ZERO: True, FLAG_MINUS: False},
    ),
    S64Case(
        name="CmpIndex: operand 0x10 > index 0x05 -> carry clear, minus clear",
        program=["LxdI 05", "CmpIndex 10", "Done"],
        expect_flags={FLAG_CARRY: False, FLAG_ZERO: False, FLAG_MINUS: False},
    ),
]
