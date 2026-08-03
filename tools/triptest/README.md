# Trip Function Test Harness

Host-side regression harness for MPGuino trip-function outputs through the
debug monitor.

The firmware remains a plain developer interface. This tool writes known
values into the terminal trip measurement block with `T`, reads function
outputs with `L`, and compares selected display-function results.

## Usage

The target firmware must include debug terminal labels
(`useDebugTerminalLabels`).

Discovery only:

```text
python run_tests.py COM3 38400 --discover-only
```

Run the current case set:

```text
python run_tests.py COM3 38400
```

## Current Coverage

- Parses terminal trip measurements from `T`.
- Parses terminal trip function output from `L`.
- Writes known values into:
  - `rvVSSpulseIdx`
  - `rvVSScycleIdx`
  - `rvInjPulseIdx`
  - `rvInjCycleIdx`
  - `rvEngCycleIdx`
- Checks the terminal trip measurement round-trip.
- Checks direct function outputs exactly:
  - `tInjectorPulseCount`
  - `tVSSpulseEdgeCount`
- Evaluates every trip function exposed by `L` and checks that each produces a
  parseable numeric display value.

Additional exact cases can layer in unit conversion and EEPROM-dependent
expected values once the broad `L` sweep is stable on hardware.
