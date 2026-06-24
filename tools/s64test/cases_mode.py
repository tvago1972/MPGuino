"""SWEET64 mode-branch regression cases (m8MetricModeFlags).

The mode branches test bits of the m8MetricModeFlags program variable:
  BranchIfMetricMode    taken when mmDisplayMetric (0x01) is set
  BranchIfSAEmode       taken when mmDisplayMetric (0x01) is clear
  BranchIfFuelOverDist  taken when mmDisplayAlternateFE (0x02) is set
  BranchIfDistOverFuel  taken when mmDisplayAlternateFE (0x02) is clear

The flag variable is set before each run via set_vars, resolving
m8MetricModeFlags by name (its index is not stable across builds). Each
program loads reg2 with 0x55 (taken marker) and branches over a load that
would set reg2 to 0xAA (fall-through marker).

The first case is a plain set/read round-trip that validates the V-command
read/write path itself.
"""

from s64testcase import S64Case

METRIC = 0x01           # mmDisplayMetric
ALT_FE = 0x02           # mmDisplayAlternateFE


def _mode_branch_program(branch_instr):
    return [
        "LdRegByte 22 55",          # reg2 = 0x55 (taken marker)
        "{} taken".format(branch_instr),
        "LdRegByte 22 AA",          # reg2 = 0xAA (fall-through marker)
        "taken:",
        "Done",
    ]


CASES = [
    # --- V-command set/read round-trip ---
    S64Case(
        name="V set/read: m8MetricModeFlags = 0x03",
        program=["Done"],
        set_vars={"m8MetricModeFlags": 0x03},
        expect_vars={"m8MetricModeFlags": 0x03},
    ),
    # --- metric / SAE (bit 0x01) ---
    S64Case(
        name="BranchIfMetricMode taken (metric bit set)",
        program=_mode_branch_program("BranchIfMetricMode"),
        set_vars={"m8MetricModeFlags": METRIC},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfMetricMode not taken (metric bit clear)",
        program=_mode_branch_program("BranchIfMetricMode"),
        set_vars={"m8MetricModeFlags": 0x00},
        expect={2: 0xAA},
    ),
    S64Case(
        name="BranchIfSAEmode taken (metric bit clear)",
        program=_mode_branch_program("BranchIfSAEmode"),
        set_vars={"m8MetricModeFlags": 0x00},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfSAEmode not taken (metric bit set)",
        program=_mode_branch_program("BranchIfSAEmode"),
        set_vars={"m8MetricModeFlags": METRIC},
        expect={2: 0xAA},
    ),
    # --- fuel-over-dist / dist-over-fuel (bit 0x02) ---
    S64Case(
        name="BranchIfFuelOverDist taken (alt-FE bit set)",
        program=_mode_branch_program("BranchIfFuelOverDist"),
        set_vars={"m8MetricModeFlags": ALT_FE},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfFuelOverDist not taken (alt-FE bit clear)",
        program=_mode_branch_program("BranchIfFuelOverDist"),
        set_vars={"m8MetricModeFlags": 0x00},
        expect={2: 0xAA},
    ),
    S64Case(
        name="BranchIfDistOverFuel taken (alt-FE bit clear)",
        program=_mode_branch_program("BranchIfDistOverFuel"),
        set_vars={"m8MetricModeFlags": 0x00},
        expect={2: 0x55},
    ),
    S64Case(
        name="BranchIfDistOverFuel not taken (alt-FE bit set)",
        program=_mode_branch_program("BranchIfDistOverFuel"),
        set_vars={"m8MetricModeFlags": ALT_FE},
        expect={2: 0xAA},
    ),
]
