"""SWEET64 arithmetic / register-move regression cases.

Register operand nibble rules (see s64assembler.py):
  - single-register ops (AddByteToX, SubByteFromX): low nibble = register
  - two-register ops (AddYtoX, SubYfromX, SwapReg): low nibble = X, high = Y
Registers in inputs/expect are 1-based SWEET64 register numbers (1..7).
"""

from s64testcase import S64Case

CASES = [
    S64Case(
        name="LdRegByte: load immediate 0x2A into reg 1",
        program=["LdRegByte 11 2A", "Done"],
        expect={1: 0x2A},
    ),
    S64Case(
        name="AddByteToX: 0x2A + 0x08 = 0x32",
        program=["LdRegByte 11 2A", "AddByteToX 1 08", "Done"],
        expect={1: 0x32},
    ),
    S64Case(
        name="SubByteFromX: 0x50 - 0x10 = 0x40",
        program=["LdRegByte 11 50", "SubByteFromX 1 10", "Done"],
        expect={1: 0x40},
    ),
    S64Case(
        name="AddYtoX: reg1(0x20) + reg2(0x05) = 0x25",
        program=["LdRegByte 11 20", "LdRegByte 22 05", "AddYtoX 21", "Done"],
        expect={1: 0x25, 2: 0x05},
    ),
    S64Case(
        name="SubYfromX: reg1(0x50) - reg2(0x20) = 0x30",
        program=["LdRegByte 11 50", "LdRegByte 22 20", "SubYfromX 21", "Done"],
        expect={1: 0x30, 2: 0x20},
    ),
    S64Case(
        name="SwapReg: swap reg1(0xAA) and reg2(0xBB)",
        program=["LdRegByte 11 AA", "LdRegByte 22 BB", "SwapReg 12", "Done"],
        expect={1: 0xBB, 2: 0xAA},
    ),
    S64Case(
        name="AddByteToX with seeded input: 0xFF + 0x01 = 0x100",
        program=["AddByteToX 1 01", "Done"],
        inputs={1: 0xFF},
        expect={1: 0x100},
    ),
]
