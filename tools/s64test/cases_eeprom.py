"""SWEET64 load/store EEPROM regression cases.

LdRegEEPROM loads register X from an EEPROM parameter; StRegEEPROM stores
register X to one. The parameter-index operand is written as '&name' and the
harness resolves it to the (build-specific) index by label via the P command.

Scratch parameter: pScratchpadIdx is a dedicated EEPROM scratchpad
(pSizeScratchpad = 32 bits, default 0) that no other MPGuino function uses,
so writing it is safe.

Verification is SWEET64-INDEPENDENT: the P command reports values only in
SWEET64-formatted decimal, so instead each case stores to EEPROM and loads it
back into a register, checking the register as raw hex via ^E.  A broken
StRegEEPROM or LdRegEEPROM both make the read-back differ from the written
value.  The final case clears the scratchpad back to 0 and confirms it.

Register operand: X = low nibble (LdRegEEPROM 33 -> X=3 -> reg3).
pScratchpadIdx is 32-bit, so test values fit in 32 bits.
"""

from s64testcase import S64Case

PARAM = "&pScratchpadIdx"


def _roundtrip(test_value):
    return dict(
        program=[
            "StRegEEPROM 22 {}".format(PARAM),   # pScratchpadIdx = reg2
            "LdRegEEPROM 33 {}".format(PARAM),   # reg3 = pScratchpadIdx
            "Done",
        ],
        inputs={2: test_value},
        expect={3: test_value},
    )


CASES = [
    S64Case(name="EEPROM Ld/St round-trip: 0x12345678", **_roundtrip(0x12345678)),
    S64Case(name="EEPROM Ld/St round-trip: 0x0BADF00D", **_roundtrip(0x0BADF00D)),
    # clear the scratchpad back to its default and confirm via load-back
    S64Case(
        name="EEPROM cleanup: clear pScratchpadIdx to 0 and verify",
        program=[
            "StRegEEPROM 22 {}".format(PARAM),   # pScratchpadIdx = reg2 (0)
            "LdRegEEPROM 33 {}".format(PARAM),   # reg3 = pScratchpadIdx
            "Done",
        ],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
]
