"""SWEET64 Call / Jump / CallImplied regression cases.

These instructions transfer to a program by INDEX (getProgramPC), not to a
RAM address, so testing them uses the program RAM-override: a RAM subroutine
is assembled, a program index is redirected to it (^O), and the main program
Call/Jump/CallImplied's that index.  The harness assembles the subroutine at
RAM 0x00 and the main program at RAM 0x80, enables the override
(override_index -> 0x00), runs main, then disables the override.

Index 0 is redirected; the Call operand in the main program must match
(override_index defaults to 0, written as 'Call 00').  Results are read from
the terminal register set via ^E hex.

- Call: subroutine returns; main continues. reg1 set by the subroutine proves
  the call happened and returned.
- Jump: no return; the subroutine's Done ends the program, so an instruction
  after the Jump must NOT execute.
- CallImplied: calls the program whose index is in the jump register
  (LdJumpReg loads it from the index register).
"""

from s64testcase import S64Case

CASES = [
    S64Case(
        name="Call: main calls overridden index -> subroutine sets reg1=0xAA, returns",
        subroutine=["LdRegByte 11 AA", "Done"],
        override_index=0,
        program=["Call 00", "Done"],
        expect={1: 0xAA},
    ),
    S64Case(
        name="Call returns and main continues (reg2 set after the call)",
        subroutine=["LdRegByte 11 AA", "Done"],
        override_index=0,
        program=["Call 00", "LdRegByte 22 BB", "Done"],
        expect={1: 0xAA, 2: 0xBB},
    ),
    S64Case(
        name="Jump: subroutine ends program; post-Jump instruction not executed",
        subroutine=["LdRegByte 11 CC", "Done"],
        override_index=0,
        program=["Jump 00", "LdRegByte 11 EE", "Done"],
        expect={1: 0xCC},      # 0xEE would mean the Jump fell through
    ),
    S64Case(
        name="CallImplied: calls program index from the jump register",
        subroutine=["LdRegByte 11 DD", "Done"],
        override_index=0,
        program=["LxdI 00", "LdJumpReg", "CallImplied", "Done"],
        expect={1: 0xDD},
    ),
]
