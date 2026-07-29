"""SWEET64 remaining-opcode regression cases.

Covers the opcodes not exercised elsewhere:
  TestReg, BranchIfCclear, LdReg, LdRegByteFromIndex, AdjustQuotient,
  LdRegConstMetric, LxdIEEPROM, LxdIEEPROMoffset, LxdIParamLength,
  LxdIParamLengthIndexed, LdRegVariableOffset, StRegVariableIndexed,
  StRegVariableOffset, LdRegTripVarOffset.

LxdI-result cases verify the index register by copying it into the jump
register (LdJumpReg) and checking si64reg8jump.
"""

from s64testcase import S64Case, FLAG_ZERO, FLAG_MINUS, FLAG_CARRY

PARAM = "&pScratchpadIdx"
VAR = "@m8MetricModeFlags"
TRIP = "$terminalIdx"
MEAS = "03"

CASES = [
    # --- TestReg: set flags from a 64-bit register ---
    S64Case(
        name="TestReg: reg1 == 0 -> zero set",
        program=["ClearFlag 02", "TestReg 11", "Done"],
        expect_flags={FLAG_ZERO: True},
    ),
    S64Case(
        name="TestReg: reg1 != 0 -> zero clear",
        program=["LdRegByte 11 05", "SetFlag 02", "TestReg 11", "Done"],
        expect_flags={FLAG_ZERO: False},
    ),
    S64Case(
        name="TestReg: bit 63 set -> minus set",
        program=["ClearFlag 04", "TestReg 11", "Done"],
        inputs={1: 0x8000000000000000},
        expect_flags={FLAG_MINUS: True},
    ),

    # --- BranchIfCclear (taken when carry clear) ---
    S64Case(
        name="BranchIfCclear taken (0x10 + 1, no carry)",
        program=[
            "LdRegByte 22 55", "AddByteToX 1 01",
            "BranchIfCclear taken", "LdRegByte 22 AA", "taken:", "Done",
        ],
        inputs={1: 0x10},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfCclear not taken (wrap sets carry)",
        program=[
            "LdRegByte 22 55", "AddByteToX 1 01",
            "BranchIfCclear taken", "LdRegByte 22 AA", "taken:", "Done",
        ],
        inputs={1: 0xFFFFFFFFFFFFFFFF},
        expect={2: 0xAA},
    ),

    # --- LdReg: reg1 = reg2 ---
    S64Case(
        name="LdReg: reg1 = reg2 (0x1234)",
        program=["LdReg 21", "Done"],
        inputs={2: 0x1234},
        expect={1: 0x1234},
    ),

    # --- LdRegByteFromIndex: regX = index register value ---
    S64Case(
        name="LdRegByteFromIndex: reg1 = index(0x2A)",
        program=["LxdI 2A", "LdRegByteFromIndex 11", "Done"],
        expect={1: 0x2A},
    ),

    # --- AdjustQuotient: round-half-up after a divide ---
    S64Case(
        name="AdjustQuotient: 7 / 2 = 3 r1 -> rounds to 4",
        program=["LdRegByte 22 07", "Div2byByte 02", "AdjustQuotient", "Done"],
        expect={2: 0x04},
    ),
    S64Case(
        name="AdjustQuotient: 4 / 2 = 2 r0 -> stays 2",
        program=["LdRegByte 22 04", "Div2byByte 02", "AdjustQuotient", "Done"],
        expect={2: 0x02},
    ),

    # --- LdRegConstMetric: const routed to reg X (SAE) or Y (metric) ---
    S64Case(
        name="LdRegConstMetric SAE: const -> reg1 (X)",
        program=[
            "LdRegByte 22 00", "StRegVariable 22 {}".format(VAR),   # SAE mode
            "LdRegConstMetric 21 %idxOneThousand", "Done",
        ],
        expect={1: 0x3E8},
    ),
    S64Case(
        name="LdRegConstMetric metric: const -> reg2 (Y)",
        program=[
            "LdRegByte 22 01", "StRegVariable 22 {}".format(VAR),   # metric mode
            "LdRegConstMetric 21 %idxOneThousand", "Done",
        ],
        expect={2: 0x3E8},
    ),

    # --- LxdIEEPROM / LxdIParamLength (verify via jump register) ---
    S64Case(
        name="LxdIEEPROM: index = EEPROM byte (0x07)",
        program=["LxdIEEPROM {}".format(PARAM), "LdJumpReg", "Done"],
        set_params={"pScratchpadIdx": 0x07},
        expect_reg8={"si64reg8jump": 0x07},
    ),
    S64Case(
        name="LxdIEEPROMoffset: index = EEPROM[param + 0] byte (0x09)",
        program=["LxdI 00", "LxdIEEPROMoffset {}".format(PARAM), "LdJumpReg", "Done"],
        set_params={"pScratchpadIdx": 0x09},
        expect_reg8={"si64reg8jump": 0x09},
    ),
    S64Case(
        name="LxdIParamLength: index = bit length of pScratchpadIdx (32)",
        program=["LxdIParamLength {}".format(PARAM), "LdJumpReg", "Done"],
        expect_reg8={"si64reg8jump": 0x20},     # pSizeScratchpad = 32 bits
    ),
    S64Case(
        name="LxdIParamLengthIndexed: bit length of param[idxreg] (32)",
        program=["LxdI {}".format(PARAM), "LxdIParamLengthIndexed", "LdJumpReg", "Done"],
        expect_reg8={"si64reg8jump": 0x20},
    ),

    # --- variable offset / indexed store ---
    S64Case(
        name="LdRegVariableOffset: var[idx + 0] = var (0x5A)",
        program=[
            "LdRegByte 22 5A", "StRegVariable 22 {}".format(VAR),
            "LxdI 00", "LdRegVariableOffset 33 {}".format(VAR), "Done",
        ],
        expect={3: 0x5A},
    ),
    S64Case(
        name="StRegVariableIndexed: store reg2 to var[idxreg], read back",
        program=[
            "LxdI {}".format(VAR), "StRegVariableIndexed 22",
            "LdRegVariable 33 {}".format(VAR), "Done",
        ],
        inputs={2: 0x3C},
        expect={3: 0x3C},
    ),
    S64Case(
        name="StRegVariableOffset: store reg2 to var[idx + 0], read back",
        program=[
            "LxdI 00", "StRegVariableOffset 22 {}".format(VAR),
            "LdRegVariable 33 {}".format(VAR), "Done",
        ],
        inputs={2: 0x2D},
        expect={3: 0x2D},
    ),

    # --- trip variable offset load ---
    S64Case(
        name="LdRegTripVarOffset: trip[term + 0].meas = 0x77",
        program=[
            "LdRegByte 22 77", "LxdI {}".format(MEAS),
            "StRegTripVarIndexedRV 22 {}".format(TRIP),   # terminal meas3 = 0x77
            "LxdI 00",
            "LdRegTripVarOffset 33 {} {}".format(TRIP, MEAS), "Done",
        ],
        expect={3: 0x77},
    ),

    # --- cleanup ---
    S64Case(
        name="misc cleanup: clear pScratchpadIdx",
        program=["StRegEEPROM 22 {}".format(PARAM), "LdRegEEPROM 33 {}".format(PARAM), "Done"],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
    S64Case(
        name="misc cleanup: clear terminalIdx measurement",
        program=[
            "LxdI {}".format(MEAS), "StRegTripVarIndexedRV 22 {}".format(TRIP),
            "LdRegTripVar 33 {} {}".format(TRIP, MEAS), "Done",
        ],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
]
