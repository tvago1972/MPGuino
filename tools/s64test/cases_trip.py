"""SWEET64 trip-variable load/store regression cases.

Trip variables are addressed by (trip slot, measurement).  These use the
monitor's own scratch trip variable, terminalIdx (resolved by name with
'$terminalIdx'), and measurement rvInjCycleIdx = 3 (a 64-bit copy64
measurement), a fixed measurement index.

Store forms only exist as indexed variants (StRegTripVarIndexed,
StRegTripVarIndexedRV); there is no plain StRegTripVar.  Each case writes a
register to the terminal trip variable and loads it back with a different
load form, checking the round-trip via ^E hex (SWEET64-independent read).
Operand conventions (from ^I):
  LdRegTripVar           reg  trip          meas         (both explicit)
  LdRegTripVarIndexed    reg  (trip=idxreg) meas
  LdRegTripVarIndexedRV  reg  trip          (meas=idxreg)
  StRegTripVarIndexed    reg  (trip=idxreg) meas
  StRegTripVarIndexedRV  reg  trip          (meas=idxreg)
The index register (idxreg) is loaded with LxdI.
"""

from s64testcase import S64Case

TRIP = "$terminalIdx"
MEAS = "03"             # rvInjCycleIdx (64-bit)

CASES = [
    # StRegTripVarIndexedRV (meas from idxreg) + LdRegTripVar (both explicit)
    S64Case(
        name="Trip round-trip: StRegTripVarIndexedRV / LdRegTripVar",
        program=[
            "LxdI {}".format(MEAS),                          # idxreg = measurement
            "StRegTripVarIndexedRV 22 {}".format(TRIP),      # terminal[meas=idxreg] = reg2
            "LdRegTripVar 33 {} {}".format(TRIP, MEAS),      # reg3 = terminal[meas]
            "Done",
        ],
        inputs={2: 0x123456789A},
        expect={3: 0x123456789A},
    ),
    # StRegTripVarIndexedRV (trip explicit) + LdRegTripVarIndexed (trip from idxreg)
    S64Case(
        name="Trip round-trip: StRegTripVarIndexedRV / LdRegTripVarIndexed",
        program=[
            "LxdI {}".format(MEAS),                          # idxreg = measurement
            "StRegTripVarIndexedRV 22 {}".format(TRIP),      # terminal[meas=idxreg] = reg2
            "LxdI {}".format(TRIP),                          # idxreg = trip slot
            "LdRegTripVarIndexed 33 {}".format(MEAS),        # reg3 = trip[idxreg].meas
            "Done",
        ],
        inputs={2: 0xDEADBEEF12},
        expect={3: 0xDEADBEEF12},
    ),
    # StRegTripVarIndexed (trip from idxreg) + LdRegTripVar
    S64Case(
        name="Trip round-trip: StRegTripVarIndexed / LdRegTripVar",
        program=[
            "LxdI {}".format(TRIP),                          # idxreg = trip slot
            "StRegTripVarIndexed 22 {}".format(MEAS),        # trip[idxreg].meas = reg2
            "LdRegTripVar 33 {} {}".format(TRIP, MEAS),      # reg3 = terminal[meas]
            "Done",
        ],
        inputs={2: 0x00CAFE5678},
        expect={3: 0x00CAFE5678},
    ),
    # StRegTripVarIndexedRV + LdRegTripVarIndexedRV (meas from idxreg for both)
    S64Case(
        name="Trip round-trip: StRegTripVarIndexedRV / LdRegTripVarIndexedRV",
        program=[
            "LxdI {}".format(MEAS),                          # idxreg = measurement
            "StRegTripVarIndexedRV 22 {}".format(TRIP),      # terminal[meas=idxreg] = reg2
            "LdRegTripVarIndexedRV 33 {}".format(TRIP),      # reg3 = trip[meas=idxreg]
            "Done",
        ],
        inputs={2: 0x9988776655},
        expect={3: 0x9988776655},
    ),
    # cleanup: clear the terminal trip variable's measurement back to 0
    S64Case(
        name="Trip cleanup: clear terminalIdx measurement to 0",
        program=[
            "LxdI {}".format(MEAS),
            "StRegTripVarIndexedRV 22 {}".format(TRIP),
            "LdRegTripVar 33 {} {}".format(TRIP, MEAS),
            "Done",
        ],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
]
