"""SWEET64 arithmetic with memory/index/trip operands.

Covers the operation x source combinations not already tested with byte/const
operands: add/sub from EEPROM, variable, index register, and trip variable;
multiply/divide reg2 by EEPROM, variable, and trip variable.

Sources are seeded to known values:
  - EEPROM: pScratchpadIdx via set_params (persists across commands).
  - Variable: m8MetricModeFlags, set in-program via StRegVariable (stable only
    within one ^T run, so set + use happen in the same program).
  - Index register: LxdI.
  - Trip variable: terminalIdx measurement, set in-program via
    StRegTripVarIndexedRV.

Single-register ops use the low nibble (e.g. 'AddEEPROMtoX 1'); Mul2by*/Div2by*
operate on reg2 implicitly.
"""

from s64testcase import S64Case

PARAM = "&pScratchpadIdx"
VAR = "@m8MetricModeFlags"
TRIP = "$terminalIdx"
MEAS = "03"             # rvInjCycleIdx (64-bit measurement)

CASES = [
    # --- EEPROM source (scratchpad seeded via set_params) ---
    S64Case(
        name="AddEEPROMtoX: 0x10 + EEPROM(0x05) = 0x15",
        program=["LdRegByte 11 10", "AddEEPROMtoX 1 {}".format(PARAM), "Done"],
        set_params={"pScratchpadIdx": 0x05},
        expect={1: 0x15},
    ),
    S64Case(
        name="Mul2byEEPROM: 0x03 * EEPROM(0x04) = 0x0C",
        program=["LdRegByte 22 03", "Mul2byEEPROM {}".format(PARAM), "Done"],
        set_params={"pScratchpadIdx": 0x04},
        expect={2: 0x0C},
    ),
    S64Case(
        name="Div2byEEPROM: 0x14 / EEPROM(0x04) = 0x05",
        program=["LdRegByte 22 14", "Div2byEEPROM {}".format(PARAM), "Done"],
        set_params={"pScratchpadIdx": 0x04},
        expect={2: 0x05, 1: 0x00},
    ),

    # --- variable source (set + use within one run) ---
    S64Case(
        name="AddVariableToX: 0x10 + var(0x05) = 0x15",
        program=[
            "LdRegByte 22 05", "StRegVariable 22 {}".format(VAR),
            "LdRegByte 11 10", "AddVariableToX 1 {}".format(VAR), "Done",
        ],
        expect={1: 0x15},
    ),
    S64Case(
        name="SubVariableFromX: 0x10 - var(0x03) = 0x0D",
        program=[
            "LdRegByte 22 03", "StRegVariable 22 {}".format(VAR),
            "LdRegByte 11 10", "SubVariableFromX 1 {}".format(VAR), "Done",
        ],
        expect={1: 0x0D},
    ),
    S64Case(
        name="Mul2byVariable: 0x03 * var(0x04) = 0x0C",
        program=[
            "LdRegByte 11 04", "StRegVariable 11 {}".format(VAR),
            "LdRegByte 22 03", "Mul2byVariable {}".format(VAR), "Done",
        ],
        expect={2: 0x0C},
    ),
    S64Case(
        name="Div2byVariable: 0x14 / var(0x04) = 0x05",
        program=[
            "LdRegByte 11 04", "StRegVariable 11 {}".format(VAR),
            "LdRegByte 22 14", "Div2byVariable {}".format(VAR), "Done",
        ],
        expect={2: 0x05, 1: 0x00},
    ),

    # --- index register source ---
    S64Case(
        name="AddIndexToX: 0x10 + index(0x05) = 0x15",
        program=["LxdI 05", "LdRegByte 11 10", "AddIndexToX 1", "Done"],
        expect={1: 0x15},
    ),

    # --- trip variable source (set + use within one run) ---
    S64Case(
        name="Mul2byTripVarIndexed: 0x03 * trip[term].meas(0x04) = 0x0C",
        program=[
            "LdRegByte 11 04", "LxdI {}".format(MEAS),
            "StRegTripVarIndexedRV 11 {}".format(TRIP),   # terminal meas3 = 4
            "LxdI {}".format(TRIP),                       # idxreg = trip slot
            "LdRegByte 22 03",
            "Mul2byTripVarIndexed {}".format(MEAS),       # reg2 *= trip[idxreg].meas3
            "Done",
        ],
        expect={2: 0x0C},
    ),
    S64Case(
        name="Div2byTripVarIndexed: 0x14 / trip[term].meas(0x04) = 0x05",
        program=[
            "LdRegByte 11 04", "LxdI {}".format(MEAS),
            "StRegTripVarIndexedRV 11 {}".format(TRIP),   # terminal meas3 = 4
            "LxdI {}".format(TRIP),
            "LdRegByte 22 14",
            "Div2byTripVarIndexed {}".format(MEAS),
            "Done",
        ],
        expect={2: 0x05, 1: 0x00},
    ),

    # --- cleanup: clear the EEPROM scratchpad and terminal trip measurement ---
    S64Case(
        name="memarith cleanup: clear pScratchpadIdx",
        program=["StRegEEPROM 22 {}".format(PARAM), "LdRegEEPROM 33 {}".format(PARAM), "Done"],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
    S64Case(
        name="memarith cleanup: clear terminalIdx measurement",
        program=[
            "LxdI {}".format(MEAS), "StRegTripVarIndexedRV 22 {}".format(TRIP),
            "LdRegTripVar 33 {} {}".format(TRIP, MEAS), "Done",
        ],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
]
