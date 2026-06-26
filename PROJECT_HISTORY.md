# Project History

This document records the broad development eras and design intent behind MPGuino TAV. It is not a strict changelog; use `CHANGELOG.md` and Git history for concrete change tracking.

## Origins

MPGuino TAV is a rewritten port of Dave Brink's original MPGuino. The project keeps the original idea of a standalone Arduino fuel-consumption and trip computer, but substantially reworks the configuration system, display support, calculation engine, and debugging tools.

The core measurement model remains direct hardware capture:

- fuel injector open-time pulses
- vehicle speed sensor pulses
- timer-derived engine/motion/fuel accumulators
- EEPROM-backed settings and trip data

## SWEET64

SWEET64 was introduced to reduce the flash cost of repeated 64-bit trip and display calculations. Earlier versions used separate AVR C/C++ routines for many calculations, which made each formula expensive in program memory.

SWEET64 moved those formulas into compact bytecode programs backed by a small interpreter capable of performing 64-bit arithmetic. That made display calculations easier to share, inspect, and later test.

Important SWEET64 milestones include:

- replacing many native 64-bit calculation routines with bytecode programs
- monitor-visible disassembly and tracing
- ATmega2560 far-flash-safe program pointer handling
- RAM program editing and substitution for live debugging
- a monitor miniassembler for quick branch-safe SWEET64 experiments
- an external regression tester that queries the monitor and exercises the full opcode set
- differential validation of the interpreter, running the same regression suite against both the assembly and C primitive builds and confirming they agree
- per-instruction execution-cycle instrumentation, which quantified the assembly primitives' advantage (notably ~12x for 64-bit divide) and provides a performance-regression baseline

A representative payoff of this tooling: the regression harness uncovered a long-standing `iSqrt` bug where the assembly implementation diverged from its C reference for inputs at or above 2^31, which was then fixed and guarded against regression.

## Debug Monitor

The debug monitor began as a general PC serial diagnostic tool and grew into a SWEET64 development environment.

The current `useSWEET64devMonitor` configuration builds a stripped serial monitor intended for SWEET64 development, including Uno-class AVR boards where a full debug build would not fit. This mode deliberately keeps the maintained SWEET64 test surface available while removing LCD-facing and vehicle-facing features that are not needed for bytecode development.

## Hardware Eras

The project has supported several Arduino-era hardware targets and display combinations:

- Arduino Uno/Nano class ATmega328P boards
- Arduino Mega2560 boards
- ATmega32U4 boards
- parallel LCDs
- serial/I2C/SPI display modules
- Adafruit RGB LCD shield style builds

Mega2560 remains the most comfortable full debug target because of flash/RAM headroom. Uno-class boards remain important because they represent the constrained target that originally motivated SWEET64.

A near-term hardware goal is full support for the **MPGuino Colour Touch board** — an ATmega2560-class design driving an ILI9341 TFT touchscreen (with Bluetooth) instead of a character LCD. This motivates the ongoing work to separate data/model features from LCD-only presentation features (for example the `useFEvTdata` split), so the calculation and SWEET64 layers can drive a pixel/touch UI without the assumptions baked into the character-LCD path.

## Experimental Features

Some features are preserved because they may be useful later, even if they are not current development priorities.

Coastdown work is one example. It depends on vehicle conditions and measurements that may be difficult to make reliable with MPGuino's available inputs. The code is retained as experimental work, but it should not drive the maintained SWEET64 instruction set or regression-test scope.

Matrix-math SWEET64 support is similarly treated as legacy experimental support unless a future contributor revives it with a concrete use case.

## Pre-Git Archives

Older MPGuino TAV file archives predate the current Git history. These are useful for archaeology, but should be preserved as read-only snapshots rather than imported as normal development history.

Recommended handling:

- keep archives outside the active source tree or under a clearly named archival folder
- preserve original timestamps when possible
- name snapshots consistently, such as `mpguino_tav_YYYY-MM-DD.zip`
- document notable recovered ideas in this file rather than mixing archive contents into current source

## Current Direction

The current development direction is to keep MPGuino usable on constrained AVR targets while making SWEET64 easier to test, debug, and extend.

Near-term priorities include:

- bringing up full support for the MPGuino Colour Touch board (ILI9341 TFT touchscreen + Bluetooth)
- separating data/model features from LCD-only presentation features so they can drive a TFT/touch UI
- maintaining the SWEET64 regression test harness
- keeping `useSWEET64devMonitor` small enough for Uno-class development
- avoiding new dependencies that make the final hex file harder to fit on target boards
