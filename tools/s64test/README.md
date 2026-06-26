# SWEET64 opcode regression test harness

A Python test harness that drives the MPGuino debug-terminal serial interface
to assemble, run, and verify SWEET64 programs against real hardware. It is used
to catch regressions in the SWEET64 pseudo-processor (opcodes, flags, branches)
as the firmware changes.

## Firmware compile-time requirements

The target firmware must be built with all three of these flags. In the
intended debug-monitor build (`useSWEET64devMonitor`) they are enabled by
default.

| Flag | Why the harness needs it |
|------|--------------------------|
| `useSWEET64RAMprograms` | the `!` mini-assembler and RAM program execution |
| `useDebugTerminalSWEET64` | the `^I` opcode list, `^E` register access, `^T` trace/run |
| `useDebugTerminalLabels` | the register **label** column in `^E` output, and resolving main-program variables (e.g. `m8MetricModeFlags`) **by name** |

### Why labels are required

- `read_registers` parses the ` - label` suffix on each `^E` line and keys
  results by label, so the error/flags/8-bit-register checks can find their
  registers.
- The SWEET64 register file (`s64reg64_*`, `si64reg8*`) has a **stable** layout
  under `useDebugTerminalSWEET64`, so those could in principle be addressed by
  index. But main-program variables such as `m8MetricModeFlags` (reached via the
  `V` command) have **no stable index** across builds/configs, so they must be
  resolved by name. That requirement makes labels mandatory for the harness as a
  whole — never hardcode a program-variable index.

## Hardware behavior

Opening the serial port asserts DTR, which **resets the board** and
reinitializes all SWEET64 registers. The runner therefore opens the port once
and runs the entire suite in that single session (reopening mid-suite would
reset state). Each case re-establishes its own baseline by zeroing the
registers and seeding its inputs.

### Call/Jump RAM override and the main loop

The Call/Jump/CallImplied cases use the program RAM-override (`^O`): a program
index is briefly redirected to a RAM subroutine so the main test program can
call it. While the override is active, the firmware's main loop could also
reach that index (e.g. a display program internally calling it) and run the
RAM subroutine in its place. This is harmless for regression testing: the
substituted subroutine returns cleanly, so the worst case is a transient wrong
value on the unit's own display for a frame or two — it does not affect the
test, which runs in and reads back from the terminal's private register set.
The override is always disabled immediately after the run. Since the goal here
is to regression-test the SWEET64 opcodes (not the live display), this
interaction is not a concern.

## Usage

```
pip install pyserial
python run_tests.py <port> <baud>        # e.g. python run_tests.py COM3 38400
```

Diagnostics / single-purpose scripts:

```
python test_connectivity.py <port> <baud>   # verify link, dump instruction set + registers
python test_connectivity.py --mock          # parser check, no hardware
python raw_command.py <port> <baud> "^E"     # send one command, dump raw response lines
```

## Layout

| File | Role |
|------|------|
| `s64terminal.py` | serial connection, prompt detection, command exchange |
| `s64instructions.py` | `^I` instruction-set discovery and parsing |
| `s64registers.py` | `^E` register read (`read_registers`) and write (`set_registers`) |
| `s64assembler.py` | `!` mini-assembler driver + label resolver |
| `s64exec.py` | `^T` program execution; `run_program` high-level primitive |
| `s64testcase.py` | `S64Case` definition and `run_case` execution/comparison |
| `s64runner.py` | single-session runner and reporting |
| `cases_*.py` | test tables (arithmetic, flags, branch, compare, flow) |
| `run_tests.py` | entry point aggregating all case tables |
| `mock_terminal.py` | offline stand-in for `S64Terminal` (parser tests) |

## Writing cases

```python
S64Case(
    name="AddByteToX: 0x2A + 0x08 = 0x32",
    program=["LdRegByte 11 2A", "AddByteToX 1 08", "Done"],
    inputs={1: 0x100},          # seed registers (1-based SWEET64 number)
    expect={1: 0x32},           # 64-bit register values after run
    expect_reg8={"si64reg8jump": 0x05},   # 8-bit registers, by label
    expect_flags={FLAG_ZERO: True},        # processor flags (specified bits only)
    expect_error=False,
)
```

Notes:
- Registers in `inputs`/`expect` use the **1-based** SWEET64 number (reg 1..7),
  mapped internally to the 0-based `^E` index.
- Every program must end with an instruction that returns (`Done`); `^T` runs to
  completion.
- Register-operand nibbles: single-register ops use the low nibble; two-register
  ops use low=X, high=Y (see `s64assembler.py`).
- Branch targets use labels (`name:` lines); see `cases_flow.py`.
- `CmpXtoY` + `BranchIfLT`/`BranchIfGTorE` are intentionally inverted relative to
  their names — see `cases_compare.py`.
