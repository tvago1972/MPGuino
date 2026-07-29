# EEPROM Test Harness

Host-side EEPROM regression harness for the MPGuino debug monitor.

The firmware stays a plain developer interface: list, read, write. This tool
parses monitor output, chooses parameters to exercise, writes values, checks
readback, inspects program variables, and restores original EEPROM values.

## Usage

The target firmware must include debug terminal parameter labels
(`useDebugTerminalLabels`). The harness resolves parameters and variables by
name because their numeric indexes move between builds.

Discovery only:

```text
python run_tests.py COM3 38400 --discover-only
```

Run the current case set:

```text
python run_tests.py COM3 38400
```

## Current Coverage

- Parses `P` listing metadata: value, flags, bit length, EEPROM address, label.
- Discovers monitor ranges:
  - main settings
  - saved trip EEPROM parameters
  - display page pseudo-parameters
  - display cursor pseudo-parameters
  - menu height pseudo-parameters
- Groups parameters by exposed action flags:
  - hardware init
  - software init
  - fuel calculation
  - display change
  - metric mode conversion
- Runs one storage round-trip per bit-length bucket, comparing the raw byte
  field from `P` rather than the display-formatted decimal value.
- Runs timeout side-effect checks for the known timeout parameters by parsing
  `idxTicks0PerSecond` from `O` and checking the corresponding `v16...`
  runtime variables.
- Runs software-init side-effect checks for tank/fuel-volume parameters:
  - `pMicroSecondsPerGallonIdx`
  - `pTankSizeIdx`
  - `pTankBingoSizeIdx`
  - `m32CyclesPerVolumeIdx`
  - `m64TankSizeIdx`
  - `m64BingoTankSizeIdx`
- Runs hardware-init side-effect checks for stable derived variables:
  - `m32SpeedFactorIdx`
  - `v32MaximumVSSperiodIdx`
  - `v16DetectVehicleStopIdx`
  - `v32MaximumEnginePeriodIdx`
  - `v16DetectEngineOffIdx`
  - `v32InjectorOpenDelayIdx`
  - `v32InjectorValidMaxWidthIdx`
  - `v8VSSdebounceTickIdx`
  - drag/coastdown derived variables when compiled in

If `idxTicks0PerSecond` is not present in the constant listing, timeout cases
fall back to presence-only variable checks.
