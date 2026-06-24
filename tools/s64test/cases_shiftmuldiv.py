"""SWEET64 shift / multiply / divide regression cases.

Mul2by* and Div2by* operate on register 2 (the accumulator):
  - multiply: reg2 = reg2 * operand
  - divide:   reg2 = reg2 / operand, with the REMAINDER left in reg1
    (firmware m06: "r2 = r2 / r5 rmdr r1").
Mul2byByte/Div2byByte take an immediate byte; Mul2by1/Div2by1 use reg1.
Shifts move register X one bit; operand 11 selects reg1 (X=low nibble).
"""

from s64testcase import S64Case

CASES = [
    # --- multiply by immediate byte ---
    S64Case(
        name="Mul2byByte: 0x10 * 3 = 0x30",
        program=["LdRegByte 22 10", "Mul2byByte 03", "Done"],
        expect={2: 0x30},
    ),
    S64Case(
        name="Mul2byByte multi-byte: 0x100 * 0x10 = 0x1000",
        program=["Mul2byByte 10", "Done"],
        inputs={2: 0x100},
        expect={2: 0x1000},
    ),
    # --- divide by immediate byte (quotient -> reg2, remainder -> reg1) ---
    S64Case(
        name="Div2byByte exact: 0x30 / 3 = 0x10 r0",
        program=["LdRegByte 22 30", "Div2byByte 03", "Done"],
        expect={2: 0x10, 1: 0x00},
    ),
    S64Case(
        name="Div2byByte with remainder: 23 / 5 = 4 r3",
        program=["LdRegByte 22 17", "Div2byByte 05", "Done"],
        expect={2: 0x04, 1: 0x03},
    ),
    # --- multiply / divide by reg1 ---
    S64Case(
        name="Mul2by1: reg2(0x07) * reg1(0x06) = 0x2A",
        program=["LdRegByte 22 07", "LdRegByte 11 06", "Mul2by1", "Done"],
        expect={2: 0x2A},
    ),
    S64Case(
        name="Div2by1 with remainder: 23 / 5 = 4 r3 (remainder in reg1)",
        program=["LdRegByte 22 17", "LdRegByte 11 05", "Div2by1", "Done"],
        expect={2: 0x04, 1: 0x03},
    ),
    # --- single-bit shifts on reg1 ---
    S64Case(
        name="ShiftRegLeft: 0x04 << 1 = 0x08",
        program=["LdRegByte 11 04", "ShiftRegLeft 11", "Done"],
        expect={1: 0x08},
    ),
    S64Case(
        name="ShiftRegRight: 0x08 >> 1 = 0x04",
        program=["LdRegByte 11 08", "ShiftRegRight 11", "Done"],
        expect={1: 0x04},
    ),
]
