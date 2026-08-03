"""SWEET64 indexed / offset load-store regression cases.

These address a table entry via the index register (si64reg8trip), set with
LxdI:
  *Indexed : effective index = tripIdx
  *Offset  : effective index = base-operand + tripIdx

The %/&/@ resolvers give the index value to load into the index register, so
e.g. 'LxdI %idxOneThousand' makes the index register point at that constant,
and 'LdRegConstIndexed 11' then loads constant[idxOneThousand] = 1000.

Read-only/constant cases check against known constant values; EEPROM cases use
the pScratchpadIdx scratchpad and verify via hex ^E; the variable-indexed case
runs entirely within one ^T (m8MetricModeFlags as 8-bit scratch).
"""

from s64testcase import S64Case

PARAM = "&pScratchpadIdx"
VAR = "@m8MetricModeFlags"

CASES = [
    # --- constant, indexed (index register = constant index) ---
    S64Case(
        name="LdRegConstIndexed: const[idxOneThousand] = 1000",
        program=["LxdI %idxOneThousand", "LdRegConstIndexed 11", "Done"],
        expect={1: 0x3E8},
    ),
    # --- constant, offset (base idxTen + tripIdx 2 -> idxOneThousand) ---
    S64Case(
        name="LdRegConstOffset: const[idxTen + 2] = idxOneThousand = 1000",
        program=["LxdI 02", "LdRegConstOffset 11 %idxTen", "Done"],
        expect={1: 0x3E8},
    ),
    # --- EEPROM, indexed load (write scratchpad, then index-load it) ---
    S64Case(
        name="LdRegEEPROMindexed: load pScratchpadIdx via index register",
        program=[
            "StRegEEPROM 22 {}".format(PARAM),   # pScratchpadIdx = reg2
            "LxdI {}".format(PARAM),             # index register = scratchpad index
            "LdRegEEPROMindexed 33",             # reg3 = EEPROM[index]
            "Done",
        ],
        inputs={2: 0x0000CAFE},
        expect={3: 0x0000CAFE},
    ),
    # --- EEPROM, indexed store (store via index, read back normally) ---
    S64Case(
        name="StRegEEPROMindexed: store via index register, read back",
        program=[
            "LxdI {}".format(PARAM),             # index register = scratchpad index
            "StRegEEPROMindexed 22",             # EEPROM[index] = reg2
            "LdRegEEPROM 33 {}".format(PARAM),   # reg3 = pScratchpadIdx
            "Done",
        ],
        inputs={2: 0x0000BEEF},
        expect={3: 0x0000BEEF},
    ),
    # --- EEPROM cleanup: clear scratchpad back to 0 ---
    S64Case(
        name="EEPROM cleanup (indexed): clear pScratchpadIdx to 0",
        program=[
            "StRegEEPROM 22 {}".format(PARAM),
            "LdRegEEPROM 33 {}".format(PARAM),
            "Done",
        ],
        inputs={2: 0x00000000},
        expect={3: 0x00000000},
    ),
    # --- variable, indexed (entirely within one ^T run) ---
    S64Case(
        name="LdRegVariableIndexed: store var then index-load it (one run)",
        program=[
            "StRegVariable 22 {}".format(VAR),   # m8MetricModeFlags = reg2 (low byte)
            "LxdI {}".format(VAR),               # index register = variable index
            "LdRegVariableIndexed 33",           # reg3 = variable[index]
            "Done",
        ],
        inputs={2: 0x5A},
        expect={3: 0x5A},
    ),
]
