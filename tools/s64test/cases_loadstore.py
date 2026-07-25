"""SWEET64 load/store program-variable regression cases.

LdRegVariable loads register X from a program variable; StRegVariable stores
register X to one.  The variable-index operand is written as '@name' and the
harness resolves it to the (build-specific) index by label.

Scratch-variable caveat: the value must survive from when we write it to when
we read it.  Two failure modes rule out the obvious candidates ACROSS command
boundaries:
  - VOLATILE (v-prefix) variables (e.g. v32SystemCycleIdx) are ISR/timer
    driven and change continuously.
  - m8MetricModeFlags is recomputed from stored parameters by the main loop
    (parameters.ino), so a value written by one command is overwritten before
    the next command runs.

Both are stable only WITHIN a single ^T run (no main-loop work intervenes).
So the test is a self-contained round-trip that does everything inside one run:
save the original -> write a test value -> read it back -> restore the
original.  This still exercises both instructions independently: a broken
StRegVariable or LdRegVariable both make the read-back differ from the test
value.  m8MetricModeFlags is used as scratch (8-bit; restored), since within a
single run it is stable and writing it is benign.

Register operand: X = low nibble (LdRegVariable 33 -> X=3 -> reg3).
"""

from s64testcase import S64Case

VAR = "@m8MetricModeFlags"

CASES = [
    S64Case(
        name="Ld/St round-trip via m8MetricModeFlags (save/write/read/restore)",
        program=[
            "LdRegVariable 11 {}".format(VAR),   # reg1 = original value (save)
            "StRegVariable 22 {}".format(VAR),   # var = reg2 (test value)
            "LdRegVariable 33 {}".format(VAR),   # reg3 = var (read test value back)
            "StRegVariable 11 {}".format(VAR),   # var = reg1 (restore original)
            "Done",
        ],
        inputs={2: 0x5A},
        expect={3: 0x5A},
    ),
]
