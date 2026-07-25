"""SWEET64 multi-byte / carry-boundary regression cases.

The engine performs 64-bit arithmetic byte by byte, so carry/borrow
propagation across byte boundaries and the 32-bit half-boundary is a prime
spot for bugs.  These seed wide values via inputs (LdRegByte only loads a
single byte) and check that carries/borrows propagate correctly.
"""

from s64testcase import S64Case

CASES = [
    # --- add: carry propagation across boundaries ---
    S64Case(
        name="Add carry across 2-byte boundary: 0xFFFF + 1 = 0x10000",
        program=["AddByteToX 1 01", "Done"],
        inputs={1: 0xFFFF},
        expect={1: 0x10000},
    ),
    S64Case(
        name="Add carry across 32-bit boundary: 0xFFFFFFFF + 1 = 0x100000000",
        program=["AddByteToX 1 01", "Done"],
        inputs={1: 0xFFFFFFFF},
        expect={1: 0x100000000},
    ),
    S64Case(
        name="AddYtoX register carry across 32-bit: 0xFFFFFFFF + 1 = 0x100000000",
        program=["AddYtoX 21", "Done"],
        inputs={1: 0xFFFFFFFF, 2: 0x01},
        expect={1: 0x100000000, 2: 0x01},
    ),
    # --- subtract: borrow propagation across boundaries ---
    S64Case(
        name="Sub borrow across 32-bit boundary: 0x100000000 - 1 = 0xFFFFFFFF",
        program=["SubByteFromX 1 01", "Done"],
        inputs={1: 0x100000000},
        expect={1: 0xFFFFFFFF},
    ),
    S64Case(
        name="SubYfromX register borrow across 32-bit: 0x100000000 - 1 = 0xFFFFFFFF",
        program=["SubYfromX 21", "Done"],
        inputs={1: 0x100000000, 2: 0x01},
        expect={1: 0xFFFFFFFF, 2: 0x01},
    ),
    # --- multiply: result spanning multiple bytes / past 32 bits ---
    S64Case(
        name="Mul multi-byte: 0xFFFF * 2 = 0x1FFFE",
        program=["Mul2byByte 02", "Done"],
        inputs={2: 0xFFFF},
        expect={2: 0x1FFFE},
    ),
    S64Case(
        name="Mul across 32-bit: 0xFFFFFFFF * 2 = 0x1FFFFFFFE",
        program=["Mul2byByte 02", "Done"],
        inputs={2: 0xFFFFFFFF},
        expect={2: 0x1FFFFFFFE},
    ),
    # --- divide spanning multiple bytes ---
    S64Case(
        name="Div multi-byte: 0x10000 / 0x10 = 0x1000 r0",
        program=["Div2byByte 10", "Done"],
        inputs={2: 0x10000},
        expect={2: 0x1000, 1: 0x00},
    ),
    # --- shift across boundaries ---
    S64Case(
        name="ShiftRegLeft across 32-bit: 0x80000000 << 1 = 0x100000000",
        program=["ShiftRegLeft 11", "Done"],
        inputs={1: 0x80000000},
        expect={1: 0x100000000},
    ),
    S64Case(
        name="ShiftRegRight across byte boundary: 0x100 >> 1 = 0x80",
        program=["ShiftRegRight 11", "Done"],
        inputs={1: 0x100},
        expect={1: 0x80},
    ),
]
