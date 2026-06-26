# Changelog

This file tracks user-facing and developer-facing project changes. It is intentionally brief; Git history remains the detailed source of truth.

## Unreleased

### Added

- Added `useSWEET64devMonitor`, a stripped serial SWEET64 development monitor build for small AVR boards.
- Added SWEET64 RAM program editing, byte dumping, fill, copy-from-flash, tracing, substitution, and paste-ready source listing support.
- Added a SWEET64 miniassembler for monitor-entered RAM programs, including operand parsing and relative branch offset calculation.
- Added SWEET64 runtime error reporting through the debug monitor, including error latching, occurrence counts, and status output.
- Added SWEET64 function length reporting in the debug monitor.
- Added SWEET64 regression testing tools under `tools/s64test`.
- Added FEvT data-only support through `useFEvTdata`, allowing FEvT SWEET64 support without the LCD bar graph display.
- Added debug monitor status output for selected MPGuino status and activity flags.
- Added RTC fault signalling for DS1307 clock-halt/invalid-time detection.

### Changed

- Renamed `useAtMega328debugMonitor` to `useSWEET64devMonitor`.
- Split FEvT backing data/opcode support from `useBarFuelEconVsTime`.
- Updated SWEET64 program pointer handling for ATmega2560 far-flash behavior.
- Reworked SWEET64 program-list generation to keep program pointers and program lengths together.
- Updated SWEET64 debug monitor help text and developer documentation for RAM-program and dev-monitor workflows.
- Reduced the AtMega328P dev-monitor build by stripping LCD-facing and vehicle-facing features that are not needed for SWEET64 development.

### Fixed

- Fixed ATmega2560 SWEET64 flash pointer retrieval issues.
- Fixed SWEET64 branch offset handling in execution, tracing, and miniassembler output.
- Fixed SWEET64 opcode text/listing alignment issues.
- Fixed monitor RAM byte dump ranges, including single-byte dumps such as `0m`.
- Fixed monitor backspace handling.
- Fixed invalid SWEET64 operand handling so malformed RAM programs stop cleanly.
- Fixed FEvT indexed trip access regression-test expectations for 32-bit trip variables.

## Historical

- MPGuino TAV began as a rewritten port of Dave Brink's original MPGuino.
- SWEET64 replaced many AVR C/C++ 64-bit calculation routines to reduce flash overhead.
- Earlier source archives exist outside Git and should be treated as read-only historical snapshots.
