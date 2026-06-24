"""SWEET64 unconditional-flow regression cases: Skip and backward loops.

Skip (e15) is a branch-always with a relative target, so it is testable as a
self-contained RAM program.  A backward conditional branch additionally
exercises negative relative offsets and backward label resolution.

LdJumpReg (e29) copies the index register into the jump register, so it is
tested here by loading the index with LxdI and checking si64reg8jump.

NOTE: Jump (e28), Call (e27), and CallImplied transfer control to a PROGMEM
program by index (prgmPtr = getProgramPC(extra)), not to a relative target
within the current RAM program, so they are not unit-testable as standalone
RAM programs and are intentionally omitted here. Testing them would require
the RAM-program override mechanism and coupling to specific firmware programs.
"""

from s64testcase import S64Case

CASES = [
    S64Case(
        name="Skip forward (branch always over a load)",
        program=[
            "LdRegByte 11 55",      # reg1 = 0x55
            "Skip past",            # unconditionally jump past the next load
            "LdRegByte 11 AA",      # skipped -> reg1 stays 0x55
            "past:",
            "Done",
        ],
        expect={1: 0x55},
    ),
    S64Case(
        name="Backward loop: count reg1 3->0, accumulate reg2 = 3*5 = 0x0F",
        program=[
            "LdRegByte 11 03",      # reg1 = 3 (loop counter)
            "LdRegByte 22 00",      # reg2 = 0 (accumulator)
            "loop:",
            "AddByteToX 2 05",      # reg2 += 5
            "SubByteFromX 1 01",    # reg1 -= 1 (sets zero when it reaches 0)
            "BranchIfZclear loop",  # loop while reg1 != 0 (backward branch)
            "Done",
        ],
        expect={1: 0x00, 2: 0x0F},
    ),
    S64Case(
        name="Skip backward into a loop body, exit via counter",
        program=[
            "LdRegByte 11 02",      # reg1 = 2 (counter)
            "LdRegByte 22 00",      # reg2 = 0
            "Skip body",            # jump forward into the loop body first
            "top:",
            "AddByteToX 2 10",      # reg2 += 0x10
            "body:",
            "AddByteToX 2 01",      # reg2 += 1
            "SubByteFromX 1 01",    # reg1 -= 1
            "BranchIfZclear top",   # loop back to top while reg1 != 0
            "Done",
        ],
        # iter1 (entered at body): reg2 = 0+1 = 1, reg1 = 1
        # iter2 (from top):       reg2 = 1+0x10+1 = 0x12, reg1 = 0 -> exit
        expect={1: 0x00, 2: 0x12},
    ),
    S64Case(
        name="LdJumpReg: index (via LxdI 0x05) copied to jump register",
        program=[
            "LxdI 05",              # load index register = 5
            "LdJumpReg",            # jump register = index = 5
            "Done",
        ],
        expect_reg8={"si64reg8jump": 0x05},
    ),
]
