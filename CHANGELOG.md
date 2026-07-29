# Changelog

This file tracks user-facing and developer-facing project changes. It is intentionally brief; Git history remains the detailed source of truth.

## Unreleased

### Added

- Added `useSWEET64devMonitor`, a stripped serial SWEET64 development monitor build for small AVR boards.
- Added SWEET64 RAM program editing, byte dumping, fill, copy-from-flash, tracing, substitution, and paste-ready source listing support.
- Added a SWEET64 miniassembler for monitor-entered RAM programs, including operand parsing and relative branch offset calculation.
- Added SWEET64 runtime error reporting through the debug monitor, including error latching, occurrence counts, and status output.
- Added SWEET64 function length reporting in the debug monitor.
- Added SWEET64 regression testing tools under `tools/s64test`, with full opcode coverage validated against both the assembly and C interpreter builds.
- Added EEPROM regression testing tools under `tools/eepromtest`, covering monitor-discovered parameter ranges, storage round-trips, and selected hardware/software-init side effects.
- Added trip-function regression testing tools under `tools/triptest`, covering terminal trip measurement round-trips and display-visible trip function evaluation through the debug monitor.
- Added by-name resolution of program variables, EEPROM parameters, program constants, and trip slots in the regression harness, so test programs do not depend on build-specific indices.
- Added SWEET64 per-run execution-cycle instrumentation (`m32S64programCyclesIdx` / `m32S64programInstrIdx`) and a `perf_report` tool for cycles-per-instruction measurement and assembly-vs-C performance comparison.
- Added FEvT data-only support through `useFEvTdata`, allowing FEvT SWEET64 support without the LCD bar graph display.
- Added debug monitor status output for selected MPGuino status and activity flags.
- Added RTC fault signalling for DS1307 clock-halt/invalid-time detection.
- Added a TFT primary display for the MPGuino Colour Touch (ILI9341) with XPT2046 resistive touch: startup splash, a live instrument dashboard, and light wake-from-idle on screen tap.
- Added a fully non-blocking TFT touch UI — the settings editor, option dropdown, numeric keypad, and 4-corner touch calibration each run as per-loop screen states, so the main loop (trip processing, datalogging, display refresh) never stalls while a screen is shown.
- Added an activity/sleep countdown bar on every TFT screen (`useTFTsleepBarEverywhere`).
- Added TFT settings-gear gestures: a double-tap opens settings, a sustained hold opens touch calibration.

### Changed

- Renamed `useAtMega328debugMonitor` to `useSWEET64devMonitor`.
- Split FEvT backing data/opcode support from `useBarFuelEconVsTime`.
- Updated SWEET64 program pointer handling for ATmega2560 far-flash behavior.
- Reworked SWEET64 program-list generation to keep program pointers and program lengths together.
- Updated SWEET64 debug monitor help text and developer documentation for RAM-program and dev-monitor workflows.
- Renamed the SWEET64 constant instruction family from `RdOnly` to `Const` terminology; the old source names remain compatibility aliases.
- Reduced the AtMega328P dev-monitor build by stripping LCD-facing and vehicle-facing features that are not needed for SWEET64 development.
- Generalized the debug-monitor status-flag output into a table-driven routine, reducing flash use by roughly 600 bytes.
- Converted the sequential index-constant definitions to `enum` blocks across the ring-buffer, variable, SWEET64, debug-monitor, trip, parameter, display/menu, calculation-format, and function-index families; the index values are preserved and now covered by the SWEET64, EEPROM, and trip-function regression harnesses.
- Added a hidden EEPROM layout version byte and shifted the signature storage slot so future EEPROM layout rewrites deliberately force settings back to defaults instead of accepting stale stored data.
- Reworked EEPROM parameter initialization and addressing so defaults are applied from C++ instead of SWEET64, allowing the EEPROM parameter address ladder to be removed.
- Split the LCD primary display into `feature_lcdmain` (mirroring `feature_tftmain`), leaving `feature_base` with just the base menu.
- Set the repo default `configs.h` to the base MPGuino hardware (`useLegacyBoard`); per-testbed hardware/feature toggles are kept local.

### Removed

- Removed unused SWEET64 trace bytecode instructions (`TraceOn`/`TraceOff`/`TraceSave`/`TraceRestore`/`TraceDone`) and the trace-save processor flag, recovering flash; the monitor still controls tracing directly.

### Fixed

- Fixed SWEET64 `iSqrt` returning incorrect results for inputs at or above 2^31: the hand-written assembly used a signed comparison where its own C reference uses an unsigned one (e.g. `sqrt(0xFFFFFFFF)` returned 0 instead of 65535). Found by the regression harness's full-range testing.

- Fixed ATmega2560 SWEET64 flash pointer retrieval issues.
- Fixed SWEET64 branch offset handling in execution, tracing, and miniassembler output.
- Fixed SWEET64 opcode text/listing alignment issues.
- Fixed monitor RAM byte dump ranges, including single-byte dumps such as `0m`.
- Fixed monitor backspace handling.
- Fixed invalid SWEET64 operand handling so malformed RAM programs stop cleanly.
- Fixed FEvT indexed trip access regression-test expectations for 32-bit trip variables.
- Fixed malformed preprocessor directives that break under certain configs: a stray comma on `#ifdef useCalculatedFuelFactor`, and bare tokens after `#else` in the serial3 device init.
- Fixed the AVR EEPROM include to lowercase `<avr/eeprom.h>` so the build compiles on case-sensitive filesystems (Linux).

## Historical

- MPGuino TAV began as a rewritten port of Dave Brink's original MPGuino.
- SWEET64 replaced many AVR C/C++ 64-bit calculation routines to reduce flash overhead.
- Earlier source archives exist outside Git and should be treated as read-only historical snapshots.
