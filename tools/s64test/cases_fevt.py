"""SWEET64 FE-vs-time trip-ring regression cases.

LdRegTripFEvTindexed is data-feature gated by useFEvTdata.  It reads from the
FE-vs-time trip ring using:

    slot = (idxreg + 1 + v8FEvTimeTripIdx) % bgDataSize

These cases seed FEvT trip slots with ordinary trip-variable stores, then read
them back through LdRegTripFEvTindexed.  Trip slot and variable indices are
resolved by monitor labels so the tests remain build-config independent.
"""

from s64testcase import S64Case

CURSOR_VAR = "@v8FEvTimeTripIdx"
VSS_PULSE = "00"        # rvVSSpulseIdx
INJ_CYCLE = "03"        # rvInjCycleIdx


def fevt_slot(idx):
    return "$FEvsTimePeriodIdx[{:02d}]".format(idx)


def set_cursor(value):
    return [
        "LdRegByte 55 {:02X}".format(value),
        "StRegVariable 55 {}".format(CURSOR_VAR),
    ]


CASES = [
    S64Case(
        name="LdRegTripFEvTindexed: idx 0 reads next FEvT slot",
        program=set_cursor(0) + [
            "LxdI {}".format(fevt_slot(1)),
            "StRegTripVarIndexed 22 {}".format(VSS_PULSE),
            "LxdI 00",
            "LdRegTripFEvTindexed 33 {}".format(VSS_PULSE),
            "Done",
        ],
        inputs={2: 0x1111222233334444},
        expect={3: 0x33334444},
    ),
    S64Case(
        name="LdRegTripFEvTindexed: idx offset selects later FEvT slot",
        program=set_cursor(0) + [
            "LxdI {}".format(fevt_slot(3)),
            "StRegTripVarIndexed 22 {}".format(VSS_PULSE),
            "LxdI 02",
            "LdRegTripFEvTindexed 33 {}".format(VSS_PULSE),
            "Done",
        ],
        inputs={2: 0x2222333344445555},
        expect={3: 0x44445555},
    ),
    S64Case(
        name="LdRegTripFEvTindexed: ring wrap from last FEvT slot",
        program=set_cursor(14) + [
            "LxdI {}".format(fevt_slot(0)),
            "StRegTripVarIndexed 22 {}".format(VSS_PULSE),
            "LxdI 00",
            "LdRegTripFEvTindexed 33 {}".format(VSS_PULSE),
            "Done",
        ],
        inputs={2: 0x3333444455556666},
        expect={3: 0x55556666},
    ),
    S64Case(
        name="LdRegTripFEvTindexed: measurement operand selects raw value",
        program=set_cursor(0) + [
            "LxdI {}".format(fevt_slot(1)),
            "StRegTripVarIndexed 22 {}".format(VSS_PULSE),
            "StRegTripVarIndexed 44 {}".format(INJ_CYCLE),
            "LxdI 00",
            "LdRegTripFEvTindexed 33 {}".format(INJ_CYCLE),
            "Done",
        ],
        inputs={
            2: 0x4444555566667777,
            4: 0x7777666655554444,
        },
        expect={3: 0x7777666655554444},
    ),
    S64Case(
        name="LdRegTripFEvTindexed: out-of-range idx acts like idx 0",
        program=set_cursor(0) + [
            "LxdI {}".format(fevt_slot(1)),
            "StRegTripVarIndexed 22 {}".format(VSS_PULSE),
            "LxdI 0F",
            "LdRegTripFEvTindexed 33 {}".format(VSS_PULSE),
            "Done",
        ],
        inputs={2: 0x5555666677778888},
        expect={3: 0x77778888},
    ),
]
