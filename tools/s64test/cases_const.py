"""SWEET64 const / read-only instruction regression cases.

LdRegRdOnly loads register X from a program constant; AddConstToX adds one to
register X; Mul2byRdOnly / Div2byRdOnly multiply / divide reg2 by one.  The
constant-index operand is written as '%name' and the harness resolves it to
the (build-specific) index by alias via the O command.

Results are checked against the KNOWN, build-stable constant values
(idxTen=10, idxOneHundred=100, idxOneThousand=1000) read back as hex via ^E,
so verification does not depend on the monitor's own constant readout.

Operand notes:
  LdRegRdOnly: 'reg-byte const' (X=low nibble) -> 'LdRegRdOnly 11 %idxTen'
  AddConstToX: single register (low nibble) -> 'AddConstToX 1 %idxOneHundred'
  Mul2byRdOnly / Div2byRdOnly: operate on reg2, one operand (the constant)
"""

from s64testcase import S64Case

CASES = [
    S64Case(
        name="LdRegRdOnly: load idxTen (10) into reg1",
        program=["LdRegRdOnly 11 %idxTen", "Done"],
        expect={1: 0x0A},
    ),
    S64Case(
        name="LdRegRdOnly: load idxOneThousand (1000) into reg1",
        program=["LdRegRdOnly 11 %idxOneThousand", "Done"],
        expect={1: 0x3E8},
    ),
    S64Case(
        name="LdRegRdOnly via idxDecimalPoint alias (1000)",
        program=["LdRegRdOnly 11 %idxDecimalPoint", "Done"],
        expect={1: 0x3E8},
    ),
    S64Case(
        name="AddConstToX: 5 + idxOneHundred (100) = 105",
        program=["LdRegByte 11 05", "AddConstToX 1 %idxOneHundred", "Done"],
        expect={1: 0x69},
    ),
    S64Case(
        name="Mul2byRdOnly: 3 * idxOneThousand (1000) = 3000",
        program=["LdRegByte 22 03", "Mul2byRdOnly %idxOneThousand", "Done"],
        expect={2: 0xBB8},
    ),
    S64Case(
        name="Div2byRdOnly: 5000 / idxOneThousand (1000) = 5 r0",
        program=["Div2byRdOnly %idxOneThousand", "Done"],
        inputs={2: 5000},
        expect={2: 0x05, 1: 0x00},
    ),
]
