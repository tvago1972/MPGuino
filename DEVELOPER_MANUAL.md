# MPGuino v1.95tav - Developer Manual
## SWEET64 Engine & Debug Terminal

**Version:** 1.95tav  
**Applies to:** `sweet64.h`, `sweet64.ino`, `feature_debug.h`, `feature_debug.ino`

---

## Table of Contents

1. [SWEET64 Overview](#1-sweet64-overview)
2. [Virtual Machine Architecture](#2-virtual-machine-architecture)
3. [Program Storage](#3-program-storage)
4. [Instruction Encoding](#4-instruction-encoding)
5. [Instruction Set Reference](#5-instruction-set-reference)
6. [BCD Number Formatting](#6-bcd-number-formatting)
7. [Constant Table](#7-constant-table)
8. [Writing a SWEET64 Program](#8-writing-a-sweet64-program)
9. [Calling SWEET64 from C++](#9-calling-sweet64-from-c)
10. [Debug Terminal: Setup](#10-debug-terminal-setup)
11. [Debug Terminal: Command Reference](#11-debug-terminal-command-reference)
12. [SWEET64 Listing & Tracing](#12-sweet64-listing--tracing)
13. [RAM Program Assembler](#13-ram-program-assembler)
14. [SWEET64 Error Codes](#14-sweet64-error-codes)
15. [CPU Loading & Activity LED](#15-cpu-loading--activity-led)
16. [Hardware Timer Architecture](#16-hardware-timer-architecture)

- [Appendix A: Quick Reference - Instruction Aliases](#appendix-a-quick-reference--instruction-aliases)
- [Appendix B: Key `configs.h` Flags for Developers](#appendix-b-key-configsh-flags-for-developers)
- [Appendix C: SWEET64 Dev Monitor (`useSWEET64devMonitor`)](#appendix-c-sweet64-dev-monitor-usesweet64devmonitor)
- [Appendix D: Second Arduino Uno as External Signal Generator](#appendix-d-second-arduino-uno-as-external-signal-generator)

---

## 1. SWEET64 Overview

SWEET64 is a compact bytecode interpreter for 64-bit arithmetic, inspired by Steve Wozniak's SWEET16 (1977). Its purpose is to perform multi-step 64-bit calculations without the ROM overhead of inlining AVR assembly or C code for each formula.

Every display value MPGuino shows - fuel economy, speed, distance, time to empty, engine power - is calculated by a SWEET64 program. Each program lives in AVR flash (`PROGMEM`) as a `static const uint8_t prgmXxx[] PROGMEM = { ... }` array and is invoked by the display or logging layer.

Design goals:
- **Speed** - about 95% as fast as a coded function written in pure C.
- **Dense encoding** - most instructions are 1–4 bytes.
- **64-bit registers** - avoids overflow in intermediate products (e.g. `µs × 3785411784` for volume conversion).
- **Direct access** to EEPROM parameters, trip accumulators, main program variables, and interrupt-visible program variables without C function calls.
- **Metric/SAE branches** - a single program handles both unit systems via either conditional branches, or instructions that automatically fetch values based on the EEPROM metric mode parameter.
- **Traceability** - when the debug terminal is compiled in, every instruction can be listed or single-stepped with register dumps.

Historical Note - SWEET64 evolved into its present form, initially from noticing multiple different display calculations began requiring similar 64-bit arithmetic sequences, and then after new features added required 64-bit precision for intermediate math steps even if their output was 32-bit precision or less.

---

## 2. Virtual Machine Architecture

### 2.1 64-bit Registers

SWEET64 has **5 general-purpose 64-bit registers** (r1–r5), plus 2 additional registers used by the debug terminal (r6–r7):

| Symbol | Index | Role |
|---|---|---|
| `s64reg64_1` (r1) | 0 | General purpose |
| `s64reg64_2` (r2) | 1 | Output / accumulator (result returned to C++) |
| `s64reg64_3` (r3) | 2 | Temporary |
| `s64reg64_4` (r4) | 3 | Multiply / divide workspace |
| `s64reg64_5` (r5) | 4 | Multiply / divide workspace |
| `s64reg64_6` (r6) | 5 | Debug terminal only |
| `s64reg64_7` (r7) | 6 | Debug terminal only |

Instruction operands that refer to two registers use a packed hex byte `0xYX`, where X is the destination (rX) and Y is the source (rY). Example: `instrLdReg, 0x21` loads r1 into r2.

All 64-bit registers are stored in the global array `uint64_t s64reg[]`.

Why 64-bit Registers? Many MPGuino calculations involve multiplying large accumulated values by conversion factors.

Examples:

    Injector microseconds × 3785411784
    Distance pulses × 1609344

32-bit intermediates overflow during these operations, even though the final displayed result may fit in 32 bits. SWEET64 therefore performs calculations using 64-bit intermediates and returns the lower 32 bits of the final result.

### 2.2 8-bit Internal Registers

| Symbol | Role |
|---|---|
| `si64reg8flags` | ALU flags (carry, zero, minus, overflow, error, trace) |
| `si64reg8valid` | Opcode validity / decode scratch |
| `si64reg8trip` | Active trip index (passed in from `runPrgm()`) |
| `si64reg8spnt` | Call stack pointer |
| `si64reg8jump` | Jump register (indirect call target) |
| `si64reg8error` | Error code (debug only) |

### 2.3 ALU Flags

Stored in `si64reg8flags`:

| Bit | Constant | Meaning |
|---|---|---|
| 0 | `SWEET64carryFlag` | Set when result X > Y (or carry from add) |
| 1 | `SWEET64zeroFlag` | Set when result X == Y (or result is zero) |
| 2 | `SWEET64minusFlag` | Set when (X − Y) < 0 |
| 3 | `SWEET64overflowFlag` | Set when all 64 bits of result are 1 |
| 4 | `SWEET64errorFlag` | Set on malformed opcode / operand |
| 5 | `SWEET64traceSaveFlag` | Saved trace state during mul/div |
| 6 | `SWEET64traceCommandFlag` | Trace mode commanded by program |
| 7 | `SWEET64traceFlag` | Trace mode active (output to debug terminal) |

Comparison semantics:

```
X < Y  → carry=0, zero=0
X == Y → carry=1, zero=1
X > Y  → carry=1, zero=0
```

### 2.4 Call Stack

SWEET64 has a **16-level deep** call stack (`s64stack[16]`). `instrCall` pushes the current PC and `instrDone` pops it. Stack overflow sets the error flag.

### 2.5 Index Register

A single 8-bit primary index (`si64reg8trip` is repurposed in some operations) carries the "current trip" index passed in at invocation. It is also used as a loop variable and element selector in indexed instructions.

### 2.6 Jump Register

`si64reg8jump` holds an index value for `instrCallImplied` (indirect call). Loaded by `instrLdJumpReg`.

### 2.7 Trip Variables

Trip variables are the ISR-maintained accumulator arrays that hold every raw measurement MPGuino collects. They are one of SWEET64's three primary data sources alongside EEPROM parameters and program variables.

#### Measured fields (`rv*Idx`)

Each trip slot holds up to five fields (`trip_measurement.h`):

| Index constant | Type | What it accumulates |
|---|---|---|
| `rvVSSpulseIdx` (0) | `uint32_t` | VSS pulse edges — the distance counter |
| `rvVSScycleIdx` (1) | `uint64_t` | Timer0 cycles while vehicle was moving |
| `rvInjPulseIdx` (2) | `uint32_t` | Injector pulse count — engine revolution proxy |
| `rvInjCycleIdx` (3) | `uint64_t` | Timer0 cycles injector was open — the fuel counter |
| `rvEngCycleIdx` (4) | `uint64_t` | Timer0 cycles engine was running |

`rvVSSpulseIdx` and `rvInjCycleIdx` are used in every fuel-economy calculation: FE = `rvVSSpulseIdx` / `rvInjCycleIdx`, scaled by unit conversion constants. Not all fields exist in every slot — slots below `tripSlotFullCount` carry all five; slots from `tripSlotFullCount` to `tripSlotCount` carry only the first two, saving RAM for bar-graph history slots.

`rvVSScycleIdx`, `rvInjCycleIdx`, and `rvEngCycleIdx` hold timer0 cycle counts, not milli- or microsecond values. This is to preserve the accuracy of the gathered time measurements for the trip variables.

#### Trip slot indices

Slots are allocated at compile time via a `#define nextAllowedValue` chain. The three slots used by almost all display programs:

| Slot | Meaning |
|---|---|
| `instantIdx` | Last sample period only (or window-filtered average) |
| `currentIdx` | Running total since last current-trip reset |
| `tankIdx` | Running total since last tank reset / fill-up |

If `trackIdleEOCdata` is selected as a compile-time program option, the following additional trip slots are allocated, and their meanings are slightly modified as compared to the above three slots. `rvVSSpulseIdx` and `rvVSScycleIdx` track vehicle movement while the engine is not detected to be running, while `rvInjPulseIdx`, `rvInjCycleIdx`, and `rvEngCycleIdx` collect engine run data while the vehicle is not detected as moving.

| Slot | Meaning |
|---|---|
| `eocIdleInstantIdx` | Last sample period only |
| `eocIdleCurrentIdx` | Running total since last current-trip reset |
| `eocIdleTankIdx` | Running total since last tank reset / fill-up |

Additional optional slots: `dragHalfSpeedIdx/FullSpeedIdx/DistanceIdx` (`useDragRaceFunction`), `windowTripFilterIdx[0..3]` (`useWindowTripFilter`), bar-graph history buckets (`useBarFuelEconVsTime`, `useBarFuelEconVsSpeed`), and EEPROM mirror slots (`useEEPROMtripStorage`).

#### Raw → processed pipeline

The Timer0 ISR accumulates into one of two raw double-buffer slots (`raw0tripIdx` / `raw1tripIdx`, tracked by `curRawTripIdx`). At each sample tick (2× per second) the main loop atomically swaps the active raw slot, then propagates the old raw slot into the processed slots via `tripUpdateList[]`. Bit 7 of the destination index selects the operation: set = **transfer** (replace), clear = **accumulate** (add). This is how trip measurement accuracy is maintained with a minimum possible chance of a trip related measurement being missed or garbled.

#### Accessing from SWEET64

```
instrLdRegTripVar,        0xXY, <tripIdx>, <rvFieldIdx>   // load from named slot
instrLdRegTripVarIndexed, 0xXY, <rvFieldIdx>              // load using index register as slot
```

See §5.6 for the full trip variable instruction reference.

---

## 3. Program Storage

### 3.1 PROGMEM (flash) Programs

Normal programs live in flash as `PROGMEM` byte arrays:

```cpp
static const uint8_t prgmMyCalc[] PROGMEM = {
    instrLdRegTripVar, 0x02, tripIdx, rvInjCycleIdx, // r2 = inj cycle time
    instrMul2byRdOnly, idxDecimalPoint,               // r2 *= 1000
    instrDiv2byVariable, v32SystemCycleIdx,            // r2 /= elapsed cycles
    instrDone
};
```

The macro `S64_PRGM_PTR(prgmMyCalc)` creates an `s64prgm_ptr_t` pointer compatible with `SWEET64::runPrgm()`.

Each named program is registered in the **function dispatch table** `S64programList[]` (defined via `S64_PROGRAM_ENTRIES` macro in `functions.h`) with a `tXxx` index. The dispatch table allows the display layer to call `SWEET64::runPrgm(SWEET64::getProgramPointer(calcIdx), tripIdx)` without knowing the actual pointer.

### 3.2 RAM Programs (`useSWEET64RAMprograms`)

When `useSWEET64RAMprograms` is defined (requires `useDebugTerminalSWEET64`), a 256-byte RAM buffer `s64programRAM[256]` holds a single mutable program. This enables:
- Interactive assembly via the debug terminal
- Hot-swapping a PROGMEM program with a RAM version for live patching

The PC union `s64pc_t` carries a `source` field distinguishing `s64srcProgmem` from `s64srcRAM`. `readProgramByte()` dispatches accordingly.

**RAM override:** `SWEET64::enableProgramRAMoverride(prgmIdx, ramAddr)` redirects all calls to trip function `prgmIdx` to RAM at `ramAddr` instead of PROGMEM. This survives until `SWEET64::disableProgramRAMoverride()` is called.

---

## 4. Instruction Encoding

Each instruction fetch reads a **1-byte opcode** followed by 0–3 operand bytes. The number and meaning of those bytes is determined by a **fetch-word** looked up from `opcodeFetchWord[]`.

### 4.1 Fetch-Word Layout

The fetch-word (stored in PROGMEM as `uint16_t`) splits into two bytes:

```
High byte: rxx | pxx | sxx
Low byte:  exx  or  (ixx | mxx)
```

| Field | Bits | Meaning |
|---|---|---|
| `rxx` | [7:5] of high byte | Register operand mode |
| `pxx` | [4:2] of high byte | Primary operand mode |
| `sxx` | [1:0] of high byte | Secondary (extra) operand mode |
| `ixx` | [7:3] of low byte | Instruction sub-class |
| `mxx` | [2:0] of low byte | ALU operation |
| `exx` | [5:0] of low byte | Extended opcode (when `rxx == r00`) |

### 4.2 Register Operand Modes (`rxx`)

| Constant | Value | Effect |
|---|---|---|
| `r00` | 0 | No register operand read |
| `r01` | 32 | Read rX and rY from next program byte |
| `r02` | 64 | Set rX = r5 |
| `r03` | 96 | Set rX = r5, rY = r1 |
| `r04` | 128 | Read rP and rS from next program byte |
| `r05` | 160 | Set rP = r5, rS = r2 |
| `r06` | 192 | Read rP from program, set rS = r5, rX = r5 |
| `r07` | 224 | Read rX and rY from program; if metric mode, shift rY to rX |

When `r01` or `r04` is used, the next byte encodes `0xYX` for registers Y and X.

### 4.3 Primary Operand Modes (`pxx`)

| Constant | Effect |
|---|---|
| `p00` | No primary operand |
| `p01` | Load from next program byte |
| `p02` | Load from index register |
| `p03` | Load from (program byte + index) |

### 4.4 Secondary Operand Modes (`sxx`)

| Constant | Effect |
|---|---|
| `s00` | No secondary operand |
| `s01` | Load from next program byte |
| `s02` | Load from index register |
| `s03` | Load from jump register |

### 4.5 ALU Operations (`mxx`)

| Constant | Operation |
|---|---|
| `m00` | No ALU operation |
| `m01` | Add: rP = rP + rS |
| `m02` | Sub: rP = rP − rS |
| `m03` | Compare: flags ← rP − rS |
| `m04` | Test: flags ← rS (no writeback) |
| `m05` | Multiply: r2 = r2 × r5 |
| `m06` | Divide: r2 = r2 / r5; remainder → r1; adj → r5 |

---

## 5. Instruction Set Reference

All instruction constants are defined in `sweet64.h`. The following is a complete list.

### 5.1 Test / Compare

| Instruction | Operands | Description |
|---|---|---|
| `instrTestReg` | `rX` (packed in high nibble of register byte) | Test r5 for zero / high bit |
| `instrTestIndex` | - | Test primary index for zero / high bit |
| `instrCmpXtoY` | `rX`, `rY` | Compare rX to rY; set flags |
| `instrCmpIndex` | primary byte | Compare primary index to immediate byte |

### 5.2 Branch Instructions

All branches take a **1-byte signed relative offset** (added to PC after the branch instruction).

| Instruction | Alias | Branches when… |
|---|---|---|
| `instrBranchIfVclear` | - | overflow clear |
| `instrBranchIfVset` | `instrBranchIfOverflow` | overflow set |
| `instrBranchIfMclear` | `instrBranchIfPlus` | minus clear (result ≥ 0) |
| `instrBranchIfMset` | `instrBranchIfMinus` | minus set (result < 0) |
| `instrBranchIfZclear` | `instrBranchIfNotE`, `instrBranchIfNotZero` | zero clear (X ≠ Y) |
| `instrBranchIfZset` | `instrBranchIfE`, `instrBranchIfZero` | zero set (X == Y) |
| `instrBranchIfCclear` | `instrBranchIfLTorE` | carry clear (X ≤ Y) |
| `instrBranchIfCset` | `instrBranchIfGT` | carry set (X > Y) |
| `instrBranchIfLT` | - | X < Y |
| `instrBranchIfGTorE` | - | X ≥ Y |
| `instrBranchIfMetricMode` | - | metric mode active |
| `instrBranchIfSAEmode` | - | SAE (imperial) mode active |
| `instrBranchIfFuelOverDist` | - | output format is fuel/distance (L/100km) |
| `instrBranchIfDistOverFuel` | - | output format is distance/fuel (MPG or km/L) |
| `instrSkip` | - | always branch (unconditional) |

### 5.3 Subroutine / Jump

| Instruction | Operands | Description |
|---|---|---|
| `instrCall` | function index byte | Push PC, call indexed trip function |
| `instrCallImplied` | - | Push PC, call function in jump register |
| `instrJump` | function index byte | Jump to indexed trip function (no push) |
| `instrLdJumpReg` | index byte | Load jump register from primary index |

### 5.4 Register Load / Store

| Instruction | Operands | Description |
|---|---|---|
| `instrLdReg` | `0xYX` | r[X] ← r[Y] |
| `instrLdRegByte` | `0xX0`, byte | r[X] ← zero-extended byte |
| `instrLdRegByteFromIndex` | `0xX0` | r[X] ← zero-extended primary index |
| `instrSwapReg` | `0xYX` | swap r[X] ↔ r[Y] |
| `instrShiftRegLeft` | `0xX0` | r[X] <<= 1 |
| `instrShiftRegRight` | `0xX0` | r[X] >>= 1 |

### 5.5 EEPROM Parameter Access

| Instruction | Operands | Description |
|---|---|---|
| `instrLdRegEEPROM` | `0xX0`, parmIdx | r[X] ← EEPROM parameter |
| `instrLdRegEEPROMindexed` | `0xX0` | r[X] ← EEPROM param at primary index |
| `instrStRegEEPROM` | `0xX0`, parmIdx | EEPROM parameter ← r[X] |
| `instrStRegEEPROMindexed` | `0xX0` | EEPROM param at primary index ← r[X] |
| `instrLxdI` | parmIdx | primary index ← parmIdx |
| `instrLxdIEEPROM` | parmIdx | primary index ← EEPROM value at parmIdx |
| `instrLxdIEEPROMoffset` | parmIdx | primary index ← EEPROM value at (parmIdx + index) |
| `instrLxdIParamLength` | parmIdx | primary index ← bit-length of EEPROM parmIdx |
| `instrLxdIParamLengthIndexed` | - | primary index ← bit-length of EEPROM param at index |
| `instrAddIndex` | offset byte | primary index += (program byte + old index) |

### 5.6 Trip Variable Access

Trip accumulators store 5 raw measurements per slot: VSS pulses, VSS cycles, injector pulses, injector open cycles, and engine run cycles.

| Instruction | Operands | Description |
|---|---|---|
| `instrLdRegTripVar` | `0xX0`, tripIdx, rvIdx | r[X] ← trip[tripIdx].rv[rvIdx] |
| `instrLdRegTripVarIndexed` | `0xX0`, rvIdx | r[X] ← trip[index].rv[rvIdx] |
| `instrLdRegTripVarOffset` | `0xX0`, rvIdx | r[X] ← trip[index+byte].rv[rvIdx] |
| `instrLdRegTripVarIndexedRV` | `0xX0`, tripIdx | r[X] ← trip[tripIdx].rv[index] |
| `instrStRegTripVarIndexed` | `0xX0`, rvIdx | trip[index].rv[rvIdx] ← r[X] |
| `instrStRegTripVarIndexedRV` | `0xX0`, tripIdx | trip[tripIdx].rv[index] ← r[X] |

**Raw value indices (`rvIdx`):**

| Constant | Value | Meaning |
|---|---|---|
| `rvVSSpulseIdx` | 0 | Distance pulse count |
| `rvVSScycleIdx` | 1 | Time vehicle was moving (timer0 ticks) |
| `rvInjPulseIdx` | 2 | Engine rev count |
| `rvInjCycleIdx` | 3 | Injector open time (timer0 ticks) |
| `rvEngCycleIdx` | 4 | Engine run time (timer0 ticks) |

### 5.7 Program Variable Access

Program variables are `uint32_t` values in the main `v32xxx` / `m32xxx` global arrays (declared in `heart.h`). Indexed by `vXxx` constants.

| Instruction | Operands | Description |
|---|---|---|
| `instrLdRegVariable` | `0xX0`, varIdx | r[X] ← variable[varIdx] |
| `instrLdRegVariableIndexed` | `0xX0` | r[X] ← variable[index] |
| `instrLdRegVariableOffset` | `0xX0`, varIdx | r[X] ← variable[index + varIdx] |
| `instrStRegVariable` | `0xX0`, varIdx | variable[varIdx] ← r[X] |
| `instrStRegVariableIndexed` | `0xX0` | variable[index] ← r[X] |
| `instrStRegVariableOffset` | `0xX0`, varIdx | variable[index + varIdx] ← r[X] |

### 5.8 Read-only Constant Access

| Instruction | Operands | Description |
|---|---|---|
| `instrLdRegRdOnly` | `0xX0`, constIdx | r[X] ← constant table entry |
| `instrLdRegRdOnlyIndexed` | `0xX0` | r[X] ← constant[index] |
| `instrLdRegRdOnlyOffset` | `0xX0`, constIdx | r[X] ← constant[index + constIdx] |
| `instrLdRegRdOnlyMetric` | `0xX0`, saeIdx | r[X] ← constant[saeIdx] (or [saeIdx+1] if metric) |

### 5.9 Arithmetic

| Instruction | Description |
|---|---|
| `instrAddYtoX` | r[X] += r[Y] |
| `instrSubYfromX` | r[X] -= r[Y] |
| `instrAddByteToX` | r[X] += immediate byte |
| `instrSubByteFromX` | r[X] -= immediate byte |
| `instrAddConstToX` | r[X] += constant table value |
| `instrAddEEPROMtoX` | r[X] += EEPROM parameter |
| `instrAddVariableToX` | r[X] += program variable |
| `instrAddIndexToX` | r[X] += primary index |
| `instrSubVariableFromX` | r[X] -= program variable |
| `instrMul2by1` | r2 *= r1 (64-bit, result in r2) |
| `instrMul2byByte` | r2 *= immediate byte |
| `instrMul2byRdOnly` | r2 *= constant table value |
| `instrMul2byEEPROM` | r2 *= EEPROM parameter |
| `instrMul2byVariable` | r2 *= program variable |
| `instrMul2byTripVarIndexed` | r2 *= trip[index].rv[rvIdx] |
| `instrDiv2by1` | r2 /= r1; remainder → r1 |
| `instrDiv2byRdOnly` | r2 /= constant table value |
| `instrDiv2byEEPROM` | r2 /= EEPROM parameter |
| `instrDiv2byVariable` | r2 /= program variable |
| `instrDiv2byTripVarIndexed` | r2 /= trip[index].rv[rvIdx] |
| `instrDiv2byByte` | r2 /= immediate byte |
| `instrAdjustQuotient` | r2 += 1 if (last remainder × 2) > divisor |
| `instrIsqrt` | r2 ← √(lower 32 bits of r2) (requires `useIsqrt`) |

> **Multiply/divide convention:** All multi-argument multiplications and divisions use r2 as the accumulator and r5 as the secondary operand. r1 receives the remainder after division. r4 and r3 hold intermediate products during 64×64 multiply. Calling `instrDiv2by1` then `instrAdjustQuotient` gives correctly rounded integer division.

### 5.10 Flag / Trace Control

| Instruction | Description |
|---|---|
| `instrClearFlag` | Clear a specific ALU flag bit |
| `instrSetFlag` | Set a specific ALU flag bit |
| `instrTraceOn` | Enable trace output to debug terminal |
| `instrTraceOff` | Disable trace output |
| `instrTraceSave` | Save current trace state, then disable trace |
| `instrTraceRestore` | Restore previously saved trace state |
| `instrTraceDone` | Restore trace state, then return (`instrDone`) |

### 5.11 BCD Formatting

| Instruction | Operands | Description |
|---|---|---|
| `instrDoBCDadjust` | `0xZX`, bcdFormatIdx | Convert r[X] to BCD string in r[Z] using named format |

### 5.12 Return

| Instruction | Description |
|---|---|
| `instrDone` | Return to caller; if at top of stack, return to C++ |

---

## 6. BCD Number Formatting

`instrDoBCDadjust` converts a binary integer in a 64-bit register to a BCD string for display. The format is described by an entry in `s64BCDformatList[]`.

### 6.1 Built-in BCD Formats

| Constant | Format | Description |
|---|---|---|
| `bcdFormat10digit` | up to 10 decimal digits | Standard right-justified number with leading spaces |
| `bcdFormatHHMMSS` | `HH:MM:SS` | 24-hour clock (requires `useClockSupport`) |
| `bcdFormatH9MMSS` | `H9:MM:SS` | Engine run time / trip time (hours 0–99) |
| `bcdFormatOverflow` | `---` | Overflow sentinel (produced by `ull2str` when value is too large) |

### 6.2 Format Table Entry Structure

Each entry in `s64BCDformatList[]` is:

```
[total length] [leading-zero char] [BCD byte count / offset] [divisor count]
[divisor0] [divisor1] ...
```

The divisors are applied right-to-left: the least significant group is extracted using `divisor0`, next group by `divisor1`, etc.

### 6.3 `instrDoBCDadjust` Usage

```cpp
// Convert r2 (in timer0 ticks) to hh:mm:ss and store result in r1
instrLdReg, 0x21,                   // r1 = r2 (copy input)
instrDoBCDadjust, 0x12, bcdFormatH9MMSS,  // r2 ← BCD of r1
instrDone
```

The result in `r[Z]` is then passed to `ull2str()` which walks the BCD bytes and produces an ASCII string in `nBuff`.

---

## 7. Constant Table

The constant table `constantNumberList[]` is indexed by `idxXxx` constants. These are "read-only" values loaded by `instrLdRegRdOnly` and the `Mul2byRdOnly` / `Div2byRdOnly` family.

Selected entries:

| Constant | Value | Purpose |
|---|---|---|
| `idxTen` | 10 | Generic ×10 |
| `idxOneThousand` / `idxDecimalPoint` | 1,000 | Decimal point scaling (parameters stored ×1000) |
| `idxOneMillion` / `idxMicroSecondsPerSecond` | 1,000,000 | µs → s conversion |
| `idxNumerDistance` | 1,609,344 | Miles → km numerator |
| `idxDenomDistance` / `idxOneMillion` | 1,000,000 | Miles → km denominator |
| `idxNumerVolume` | 3,785,411,784 | US gallons → mL numerator |
| `idxDenomVolume` / `idxOneBillion` | 1,000,000,000 | US gallons → mL denominator |
| `idxSecondsPerHour` | 3,600 | hr → s |
| `idxTicks0PerSecond` | F_CPU / (256 × prescaler) | Timer0 ticks per second |
| `idxCycles0PerSecond` | F_CPU / prescaler | Timer0 cycles per second |
| `idxNumerPressure` | 68,947,573 | psi → Pa numerator |
| `idxDenomPressure` / `idxTenMillion` | 10,000,000 | psi → Pa denominator |
| `idxNumerMass` / `idxOneBillion` | 1,000,000,000 | lbs → kg numerator |
| `idxDenomMass` | 2,204,622,621 | lbs → kg denominator |
| `idxPowerFactor` | 22,840 | Vehicle power estimation factor (228.4 × 100 internal) |
| `idxCorrectionFactor` | 4,096 | Fuel pressure correction intermediate scale |

> **SAE/Metric pairs:** `instrLdRegRdOnlyMetric` loads `constant[saeIdx]` in imperial mode and `constant[saeIdx+1]` in metric mode. This is how a single program handles both unit systems: place the SAE value at an even index and the metric value immediately after it.

---

## 8. Writing a SWEET64 Program

### 8.1 Anatomy of a Program

```cpp
static const uint8_t prgmCalculateFuelEconomy[] PROGMEM = {
    // r2 = distance (VSS cycles × numerator constant)
    instrLdRegTripVar, 0x02, tripIdx, rvVSSpulseIdx,  // r2 ← VSS pulses
    instrMul2byRdOnly, idxNumerDistance,               // r2 *= 1,609,344 (mi→km)

    // branch if metric
    instrBranchIfSAEmode, 4,                           // if SAE, skip next 4 bytes
    instrDiv2byRdOnly, idxDenomDistance,               // r2 /= 1,000,000

    // r1 = fuel used (injector cycles)
    instrLdRegTripVar, 0x01, tripIdx, rvInjCycleIdx,   // r1 ← injector open time
    instrMul2byEEPROM, pMicroSecondsPerGallonIdx,       // r2 *≠ (uses separate Mul path)

    instrDiv2by1,                                       // r2 /= r1
    instrAdjustQuotient,                                // round
    instrDone                                           // return r2 to caller
};
```

**Rules:**
1. The program **must end with `instrDone`**. Falling off the end is undefined behaviour.
2. `r2` is the **return value** - `runPrgm()` returns `(uint32_t)(s64reg[s64reg64_2])` (lower 32 bits).
3. Branch offsets are **signed bytes relative to the byte immediately after the branch instruction**. Positive = forward, negative = backward.
4. The trip index passed to `runPrgm()` is available in `si64reg8trip`; instructions like `instrLdRegTripVar` substitute this automatically when `tripIdx` is used as the trip operand.
5. All EEPROM parameters are already scaled by ×1000 (the `idxDecimalPoint` convention). When presenting to users, the display layer divides by 1000 for the decimal point.

### 8.2 Common Patterns

**Metric/SAE conversion:**

```cpp
instrBranchIfSAEmode, <offset_to_skip_metric_conversion>,
// metric path
instrMul2byRdOnly, idxNumerDistance,
instrDiv2byRdOnly, idxDenomDistance,
// falls through to...
instrDone

// or use instrLdRegRdOnlyMetric to load SAE/metric pair in one instruction:
instrLdRegRdOnlyMetric, 0x01, idxNumerDistance,  // r1 = 1,609,344 (metric) or 1 (SAE)
```

**Subroutine call:**

```cpp
instrCall, tLoadTrip,  // call the trip-loading helper
// ... use loaded trip ...
instrDone
```

**Null check / guard:**

```cpp
instrLdRegTripVar, 0x01, tankIdx, rvVSSpulseIdx, // r1 ← tank distance pulses
instrTestReg, 0x01,                               // test r1
instrBranchIfZero, <offset_to_overflow_return>,   // bail if zero
```

### 8.3 Registering a New Program

1. Define the `static const uint8_t prgmMyCalc[] PROGMEM = { ... }` array.
2. Add a `tMyCalc` index constant to the `#define nextAllowedValue` chain in `functions.h`.
3. Add a `tMyCalc` entry to the `S64_PROGRAM_ENTRIES(S64_PROGRAM_CASE)` macro expansion (also in `functions.h` or `functions.ino`).
4. Add a display label string to `calcFormatLabels[]` and a format entry to `calcFormatList[]` / `calcFormatDecimalPlaces[]`.

---

## 9. Calling SWEET64 from C++

```cpp
// Call by PROGMEM pointer (most common)
uint32_t result = SWEET64::runPrgm(S64_PRGM_PTR(prgmMyCalc), tripIdx);

// Call by function dispatch index (supports RAM override if active)
s64pc_t pc = SWEET64::getProgramPC(tFuelEcon);  // respects RAM override if active
uint32_t result = SWEET64::runPrgm(pc, tankIdx);

// Call by raw PROGMEM dispatch pointer (bypasses RAM override)
uint32_t result = SWEET64::runPrgm(SWEET64::getProgramPointer(tFuelEcon), currentIdx);
```

`runPrgm()` returns the lower 32 bits of r2 at `instrDone`. If the error flag is set, the returned value is unreliable; check `SWEET64processorFlags & SWEET64errorFlag`.

---

## 10. Debug Terminal: Setup

### 10.1 Enabling

In `configs.h`:

```c
#define useDebugTerminal true
#define useDebugTerminalSerialPort0 true   // or port1/port2/port3
#define useDebugTerminalBufferedOutput true
#define useDebugTerminalSerialBaudRate 38400
```

On ATmega2560, the following are automatically enabled when `useDebugTerminal` is set:

```c
#define useDebugTerminalHelp true        // ? command
#define useDebugTerminalLabels true      // symbolic names in output
#define useDebugTerminalSWEET64 true     // SWEET64 list/trace
#define useSWEET64RAMprograms true       // RAM assembler
#define useIsqrt true
#define usePressure true
#define useBuffering true
```

On ATmega328P, `useDebugTerminal` is normally silently disabled (too little flash). See [Appendix C](#appendix-c-sweet64-dev-monitor-usesweet64devmonitor) for the dedicated SWEET64 development monitor build.

### 10.2 Connecting

Connect a serial terminal (e.g. Arduino Serial Monitor, PuTTY, screen) to the configured UART at the configured baud rate. Line endings should be CR or CR+LF.

The terminal sends a version banner on boot:

```
MPGuino v1.95tav
2026-06-02
```

### 10.3 Input Format

Commands are single characters, optionally preceded by numeric arguments separated by `.`, `<`, or `:`. The general pattern is:

```
[z]<[y].[x]CMD[:value] [value2] ...
```

- `x` - primary argument (start index, address, etc.)
- `y` - secondary argument (end index, destination, etc.)
- `z` - tertiary argument (decimal window width, etc.)
- Values after `:` are written to the target

Numeric input is **hexadecimal by default**. Use `\` to switch the current number entry to decimal. Use `$` or `X` to switch back to hexadecimal.

---

## 11. Debug Terminal: Command Reference

### 11.1 Parameter Commands (`P`)

| Syntax | Description |
|---|---|
| `P` | List all EEPROM parameters |
| `y.xP` | List parameters from index `y` to `x` |
| `xP:v1 v2 ...` | Write value(s) starting at parameter index `x` |

Output columns: index, symbolic name (if `useDebugTerminalLabels`), current raw value, formatted value.

### 11.2 Program Variable Commands (`V`)

| Syntax | Description |
|---|---|
| `V` | List all program (RAM) variables |
| `y.xV` | List variables from `y` to `x` |
| `xV:v1 v2 ...` | Write value(s) starting at variable index `x` |

### 11.3 Trip Variable Commands (`T`)

| Syntax | Description |
|---|---|
| `T` | List all trip slot raw measurements |
| `y.xT` | List trip slots `y` to `x` |
| `xT:v1 v2 ...` | Write value(s) to trip slot `x` |

Each trip slot has 5 raw measurements (VSS pulses, VSS cycles, injector pulses, injector cycles, engine cycles).

### 11.4 Constant Table Commands (`O`)

| Syntax | Description |
|---|---|
| `O` | List all SWEET64 constant table entries |
| `y.xO` | List constants from index `y` to `x` |

### 11.5 Trip Function Output (`L`)

| Syntax | Description |
|---|---|
| `L` | List all trip function computed values |
| `y.xL` | List functions `y` to `x` |
| `z<y.xL` | Same but with decimal window width `z` |

Each line shows the function index, label, and the computed numeric string.

### 11.6 Decimal Number Sandbox (`U`)

| Syntax | Description |
|---|---|
| `U` | Output math accumulator as decimal |
| `z<y.xU` | Output with window `z`, digits `y`, flags `x` |

### 11.7 Math Accumulator

| Syntax | Description |
|---|---|
| `z<y.x` | Load x into accumulator with decimal formatting |
| `+x` | Accumulator += x |
| `-x` | Accumulator -= x |
| `*x` | Accumulator *= x |
| `/x` | Accumulator /= x |
| `=x` | Load x (shorthand) |
| `\|` | Accumulator ← √(accumulator) (requires `useIsqrt`) |

These operations use SWEET64 internally (registers r6/r7 are the debug terminal's accumulators).

### 11.8 Trip Variable Copy (`R`)

| Syntax | Description |
|---|---|
| `R` | List available trip variable names (if `useDebugTerminalLabels`) |
| `y<xR` | Copy trip slot `x` into trip slot `y` |
| `R` | Copy any trip into the terminal trip variable |

### 11.9 Signal Simulator (`S`) - requires `useSimulatedFIandVSS`

| Syntax | Description |
|---|---|
| `S` | List simulator modes |
| `nS` | Set simulator mode `n` |

### 11.10 Button Injection (`I`) - requires `useDebugButtonInjection`

| Syntax | Description |
|---|---|
| `Il` / `Ic` / `Ir` | Inject short press: Left / Centre / Right |
| `IL` / `IC` / `IR` | Inject long press: Left / Centre / Right |
| `Iu` / `Id` / `IU` / `ID` | Short/long Up and Down (cross-config buttons) |

### 11.11 System Status (`^S`)

`^S` means Ctrl-S. It prints a compact system snapshot, including uptime when CPU/debug CPU timing support is compiled in, clock time when `useClockDisplay` is enabled, decimal-format settings, decoded status flags, raw status bytes, SWEET64 RAM override status when enabled, the SWEET64 error latch when enabled, and Bluetooth response state when the BLE shield support is compiled in.

With `useDebugTerminalSWEET64`, the SWEET64 error latch reports the last engine-detected malformed-program error:

```
S64 ERR count=0027 @ 02R last=03 bad reg operand
```

The count increments when the same error repeats at the same SWEET64 program counter. A different error or different program counter resets the count to 1. The `N` command toggles SWEET64 error mute; while muted, repeated runtime error reports are suppressed, but the latch and count remain visible through `^S`.

### 11.12 Help (`?`)

Prints the full command reference (requires `useDebugTerminalHelp`).

---

## 12. SWEET64 Listing & Tracing

These commands require `useDebugTerminalSWEET64`. Commands shown with `^` use control-key notation; for example, `^I` means Ctrl-I, not a literal caret followed by `I`.

### 12.1 Instruction Reference (`^I`)

| Syntax | Description |
|---|---|
| `^I` | List all opcodes and their operand descriptions |
| `y.x^I` | List opcodes from `y` to `x` |

### 12.2 Function Length Table (`^F`) - requires `useDebugTerminalLabels`

| Syntax | Description |
|---|---|
| `^F` | List all trip functions with their byte lengths and PROGMEM addresses |
| `y.x^F` | List functions `y` to `x` |

### 12.3 Program Listing (`^L`)

| Syntax | Description |
|---|---|
| `^L` | Disassemble function 0, print 20 lines |
| `x^L` | Disassemble function `x`, print 20 lines |

Output format (with labels):
```
00:0xnnn  OpcodeName    rX, parmIdx(parmName), rvIdx(rvName)
```

With `useDebugTerminalLabels`, operands are resolved to symbolic names from the label tables.

### 12.4 SWEET64 Registers (`^E`)

| Syntax | Description |
|---|---|
| `^E` | Dump all SWEET64 registers |
| `y.x^E` | Dump registers `y` to `x` |
| `z<y.x^E` | Dump with decimal window `z`, digit count `y`, mode `x` |
| `x^E:v` | Write value `v` to SWEET64 register `x` |

Output shows both the raw 64-bit hex value and a formatted decimal representation.

### 12.5 Tracing (`^T`)

| Syntax | Description |
|---|---|
| `x^T` | Trace function `x` for 1 instruction, dump state |
| `x.y^T` | Trace function `x` for `y` instructions (`y=0` = run to completion) |
| `z<y^T` | Trace RAM program at address `z` for `y` instructions (`y=0` = run to completion; requires `useSWEET64RAMprograms`) |

Each traced instruction line shows:
- Program counter (address)
- Opcode name
- Operands (symbolic if labels enabled)
- Post-execution register file dump when trace remains active
- Decoded SWEET64 flags, including carry, zero, minus, overflow, error, and trace state

Trace output is written to the debug terminal in real time as the program executes.

If a malformed instruction or bad operand is encountered, the trace stops, the SWEET64 error latch is updated, and the terminal prints the error report unless SWEET64 errors are muted with `N`.

---

## 13. RAM Program Assembler

Requires `useSWEET64RAMprograms` (auto-enabled with `useDebugTerminalSWEET64` on ATmega2560).

The RAM assembler exists to permit rapid development and testing of SWEET64 programs without having to recompile and reflash MPGuino.

Typical workflow:

    Copy existing program
    Modify instruction or instructions
    Enable override
    Test
    Revise as necessary
    Retest as necessary
    Export as source

This significantly reduces iteration time when developing or debugging SWEET64 code.

The RAM assembler also permits more accurate determination of conditional branch instruction offset values, which would otherwise have to be figured out by hand. This eliminates a source of bugs.

The assembler provides an interactive text-based environment to write, edit, and run SWEET64 programs without recompiling. Programs are assembled into the 256-byte `s64programRAM[]` buffer.

### 13.1 RAM Memory Commands (`M`)

| Syntax | Description |
|---|---|
| `M` | Dump all 256 bytes of program RAM |
| `y.xM` | Dump bytes from address `y` to `x` |
| `xM:b1 b2 ...` | Write raw bytes starting at address `x` |
| `<M` | Fill all of program RAM with 0x00 |
| `x<M` | Fill all of program RAM with byte `x` |

### 13.2 Copy PROGMEM Function to RAM

```
x<yM
```
Copies the bytes of SWEET64 trip function `y` into program RAM starting at address `x`. This is the starting point for modifying an existing function.

### 13.3 Assembler (`!`)

```
x!
```

Enters the interactive line assembler starting at RAM address `x`. The assembler prompts for one instruction per line in pseudo-assembly syntax:

```
<addr>: OpcodeName [operand1] [operand2] ...
```

- `Enter` on an empty line terminates the assembly session.
- On success, the assembled bytes are written to program RAM and the assembler advances to the next instruction address.
- Operand labels (e.g. `pMicroSecondsPerGallonIdx`, `currentIdx`) are recognised if `useDebugTerminalLabels` is enabled.

**Example session:**

```
0!
00: LdRegTripVar 0x02 currentIdx rvInjCycleIdx
04: Mul2byRdOnly idxDecimalPoint
07: Div2byVariable v32SystemCycleIdx
10: Done
11:
```

### 13.4 Listing Program RAM as Source (`^L` with RAM)

```
x<^L
```

Disassembles program RAM starting at address `x`, printing instructions in the same pseudo-assembly format accepted by the assembler. Useful to review an assembled program.

### 13.5 RAM Trace (`^T` with RAM)

```
z<y^T
```

Traces the RAM program starting at address `z`, for `y` instructions (`y=0` = run to completion). This uses the same trace engine as function tracing, so invalid RAM opcodes, missing operands, stack overflow, and bad operand errors are reported through the SWEET64 error latch.

### 13.6 Export as C Source (`^W`)

```
x.y^W
```

Exports the RAM bytes between addresses `x` and `y` as a `static const uint8_t prgmRAMexport[] PROGMEM = { ... };` C declaration, ready to paste into a `.ino` file. `^W` means Ctrl-W.

### 13.7 Program RAM Override (`^O`)

```
x<y^O
```

Redirects all future calls to trip function `y` to the RAM program at address `x`. This allows live testing of a RAM-assembled program as if it were the real production function.

```
^O
```

Disables the override and reverts to the original PROGMEM function.

---

## 14. SWEET64 Error Codes

When an error is detected, `SWEET64errorFlag` is set in `si64reg8flags` and an error code is stored in `si64reg8error` (debug builds only). The terminal command `^E` shows the error code.

| Constant | Code | Description |
|---|---|---|
| `s64errNone` | 0 | No error |
| `s64errBadProgramCounter` | 1 | PC is null / out of range |
| `s64errBadOpcode` | 2 | Opcode ≥ `maxValidSWEET64instr` |
| `s64errBadRegisterOperand` | 3 | Register index ≥ `s64reg64count` |
| `s64errMissingRegisterOperand` | 4 | Register operand byte not present |
| `s64errMissingPrimaryOperand` | 5 | Primary operand byte not present |
| `s64errMissingExtraOperand` | 6 | Extra operand byte not present |
| `s64errBadExpandedOpcode` | 7 | Extended opcode (exx) out of range |
| `s64errStackOverflow` | 8 | Call depth exceeded 16 |
| `s64errBadOperand` | 9 | General operand validation failure |

The debug terminal displays these symbolically in trace/error output and through the `^S` SWEET64 error latch. In production builds (no debug terminal), execution simply stops at the bad instruction and returns whatever is in r2.

---

## 15. CPU Loading & Activity LED

### 15.1 CPU Loading (`useCPUreading`)

MPGuino measures its own CPU utilisation using timer0 cycle counts. In the main loop, cycle counters are accumulated separately for each phase:

| Phase | Variable | Description |
|---|---|---|
| Main process | `m32CPUworkingMainProcessIdx` | Display updates, button handling |
| Idle | `m32CPUworkingIdleProcessIdx` | Time spent in `SLEEP_MODE_IDLE` |
| Main loop | `m32CPUworkingMainLoopIdx` | Total loop time |

Every sample tick (2× per second), the working counters are snapshotted into `m32CPUsampledXxx` for display, then reset.

The **CPU Info** display screen shows:
- `C%` - main process fraction as a percentage of total loop time
- `T` - system uptime since power-on
- `FREE RAM` - available SRAM (heap top to stack pointer distance)

With `useDebugCPUreading`, the breakdown is finer: devices, activity, sample, output, other, and interrupt processing times each get their own counter.

### 15.2 Activity LED (`useActivityLED`)

An activity LED on the board's built-in LED pin blinks during specific processing phases. Each phase is assigned a bit flag:

| Constant | Bit | Phase |
|---|---|---|
| `arMainProcess` | 7 | Display / button processing |
| `arIdleProcess` | 6 | CPU sleep |
| `arMainDevices` | 5 | Hardware device I/O |
| `arMainActivity` | 4 | Activity change handling |
| `arMainSample` | 3 | Trip sample collection |
| `arMainOutput` | 2 | Output generation |
| `arMainOther` | 1 | Bluetooth / terminal |
| `arSWEET64` | 0 | SWEET64 execution |

Call `activityLED::assert(flag)` to set the LED for a phase and `activityLED::release(flag)` to clear it. The LED output reflects the OR of all currently asserted flags masked against `m8ActivityOutputIdx`.

---

## 16. Hardware Timer Architecture

MPGuino relies on two AVR hardware timers for all time-critical internal functions. Understanding their roles is essential when modifying interrupt handlers, sleep behaviour, display timing, or output pin PWM.

### 16.1 Timer0 - System Heartbeat

Timer0 is MPGuino's primary timebase. It runs continuously at a fixed rate determined by the AVR clock and a prescaler of 64:

```
f_overflow = F_CPU / (prescaler × 256) = 16 MHz / (64 × 256) = 976 Hz
```

The overflow ISR (`TIMER0_OVF_vect`, `heart.ino`) is the heartbeat of the entire system. On every overflow it:

- Counts VSS pulses and injector open-time cycles (the raw trip accumulators)
- Manages the wake/sleep watchdog (see §16.3)
- Drives the sampling ticker: every `delay0TickSampleLoop` overflows, sets `t0saTakeSample`, which tells the main loop to collect and snapshot trip data - this fires **twice per second** (`samplesPerSecond = 2`, `heart.h`)
- Sets `t0saUpdateDisplay` to trigger a display refresh at the same rate
- Manages LCD write-delay timing (`t0saDisplayDelayInit` / `t0saDisplayDelayActive`)
- Toggles the cursor blink flag (`t0saShowCursor`)
- Drives JSON data-logging output timing

The `t0saTakeSample` and `t0saUpdateDisplay` flags in `v8Timer0Status0Idx` are the mechanism by which the ISR signals the main loop without blocking it; the main loop polls these flags and clears them after acting.

`idxTicks0PerSecond` and `idxCycles0PerSecond` in the SWEET64 constant table expose the Timer0 rate to bytecode programs for time-based calculations (e.g. converting injector open cycles to fuel volume).

### 16.2 Timer1 - Secondary Timer and PWM Source

Timer1 runs in **8-bit phase-correct PWM mode with prescaler 1**, configured by `heart.ino`:

```
f_overflow = F_CPU / (2 × prescaler × TOP) = 16 MHz / (2 × 1 × 255) = 31,373 Hz
```

The overflow ISR (`TIMER1_OVF_vect`, `heart.ino`) fires at this rate and handles:

- **LCD write delays** (`t1cDelayLCD`): precise inter-command timing for the LCD controller; the main loop busy-waits on this flag via `performSleepMode(SLEEP_MODE_IDLE)` rather than spinning
- **Debug stopwatch** (`t1cEnableDebug`): accumulates Timer1 overflow counts into `v32WorkingTimer1Idx` for CPU interrupt-load measurement
- **BLE keepalive**: re-enables the Timer1 interrupt when BLE activity requires it

`idxCycles1PerTick = 510` and `idxTicks1PerSecond` in the SWEET64 constant table expose the Timer1 rate to bytecode programs (used by the signal simulator for injector pulse timing).

Timer1's output compare units also drive PWM outputs:

| MCU | Pin | Signal | Use |
|---|---|---|---|
| all | OC1A | LCD backlight brightness | `m_lcd.ino`, written via `OCR1A` |
| ATmega328P | OC1B | EXP1 output pin | `feature_outputpin.ino`, written via `OCR1B` |

Because the LCD backlight and EXP1 share Timer1, the 31,373 Hz PWM frequency is fixed by the timer configuration - it cannot be changed without affecting both functions simultaneously.

### 16.3 Wake / Sleep Mechanism

MPGuino can enter AVR sleep mode to reduce power consumption when the vehicle is not in use. The wake state is tracked in `v8AwakeIdx` using three independent flags:

| Flag | Set when | Cleared when |
|---|---|---|
| `aAwakeOnInjector` | Injector pulse detected | No injector activity for timeout period |
| `aAwakeOnVSS` | VSS pulse detected | No VSS pulses for timeout period |
| `aAwakeOnInput` | Button pressed | Activity timeout expires |

The **activity timeout** watchdog counter (`activityTimeoutCount`) is decremented on every Timer0 overflow. It is reset to `v16ActivityTimeoutIdx` whenever relevant activity is detected. When it reaches zero, the corresponding awake flag is cleared.

MPGuino enters `SLEEP_MODE_IDLE` (Timer0 and Timer1 still running) during idle periods in the main loop via `heart::performSleepMode()`. This keeps the ISRs active while halting the CPU, saving power between events.

### 16.4 Output Pin PWM Frequencies

The EXP1 and EXP2 expansion output pins use hardware PWM for DAC-style analog output (PWM → RC low-pass filter → 0–5 V). The frequencies by MCU are:

| MCU | Timer | Pin | Mode | Prescaler | f_PWM |
|---|---|---|---|---|---|
| ATmega328P | Timer1 | EXP1 (OC1B) | Phase-correct 8-bit | 1 | 31,373 Hz |
| ATmega328P | Timer2 | EXP2 (OC2A) | Phase-correct 8-bit | 1 | 31,373 Hz |
| ATmega2560 | Timer5 | EXP1 (OC5A) + EXP2 (OC5B) | Phase-correct 8-bit | 64 | 490 Hz |
| ATmega32U4 | Timer4 | EXP1 (OC4A) + EXP2 (OC4D) | Set by Arduino core | board-specific | - |

On ATmega328P, Timer2 is explicitly configured to phase-correct 8-bit PWM with prescaler 1 in `outputPin::init()`, overriding the Arduino core's default of fast PWM / prescaler 64 (~977 Hz). This ensures both EXP pins run at the same frequency and can share a single RC filter design.

At 31,373 Hz, a modest RC filter (e.g. 10 kΩ + 100 nF, f_RC ≈ 160 Hz) provides approximately 200:1 attenuation of PWM ripple, leaving under 0.5% residual ripple on the analog output.

---

## Appendix A: Quick Reference - Instruction Aliases

```cpp
#define instrBranchIfOverflow       instrBranchIfVset
#define instrBranchIfPlus           instrBranchIfMclear
#define instrBranchIfMinus          instrBranchIfMset
#define instrBranchIfNotE           instrBranchIfZclear
#define instrBranchIfE              instrBranchIfZset
#define instrBranchIfNotZero        instrBranchIfZclear
#define instrBranchIfZero           instrBranchIfZset
#define instrBranchIfLTorE          instrBranchIfCclear
#define instrBranchIfGT             instrBranchIfCset
```

## Appendix B: Key `configs.h` Flags for Developers

| Flag | Effect |
|---|---|
| `useDebugTerminal` | Enable debug terminal |
| `useDebugTerminalHelp` | `?` command |
| `useDebugTerminalLabels` | Symbolic names in all output |
| `useDebugTerminalSWEET64` | `^L`, `^T`, `^I`, `^F`, `^E` commands |
| `useSWEET64RAMprograms` | `!`, `M`, Ctrl-W / `^W`, Ctrl-O / `^O` commands + RAM assembler |
| `useDebugButtonInjection` | `I` command (requires `useDebugTerminal` + buttons) |
| `useDebugCPUreading` | Fine-grained interrupt-level CPU counters |
| `useSimulatedFIandVSS` | `S` command + injector/VSS simulation |
| `useSWEET64devMonitor` | Dedicated serial SWEET64 development monitor build |
| `useActivityLED` | On-board LED phase indicator |

---

## Appendix C: SWEET64 Dev Monitor (`useSWEET64devMonitor`)

### C.1 Purpose

`useSWEET64devMonitor` is a special build configuration that enables a headless SWEET64 debug terminal on small AVR boards such as the Arduino Uno (ATmega328P). Normally `useDebugTerminal` is suppressed on 328P due to flash constraints (32256 bytes usable with Optiboot bootloader). This flag overrides that suppression and simultaneously strips out enough non-essential features to make the terminal fit.

It is intended as a dedicated development and SWEET64 debugging firmware — not a configuration that would be used in a vehicle.

### C.2 Enabling

This configuration has been compile/upload tested on both Arduino Uno and Arduino Mega2560 hardware. The Mega2560 build can be useful for testing the same monitor personality with more flash headroom, while the Uno build proves the minimum dev-console target still fits.

In `configs.h`, set only this flag:

```c
#define useSWEET64devMonitor true
```

Everything else is configured automatically by the monitor block in `configs.h`. No other flags need to be set or cleared manually.

### C.3 What the Monitor Block Enables

```c
#define useDebugTerminal true
#define useDebugTerminalHelp true        // see flash budget notes below
#define useDebugTerminalLabels true      // symbolic names in output
#define useDebugTerminalSWEET64 true     // ^L, ^T, ^I, ^F, ^E commands
#define useSWEET64RAMprograms true       // RAM assembler
#define useDebugTerminalSerialPort0 true // UART0 (pins 0/1)
```

On Mega2560, the same monitor block also enables `useDebugCPUreading`. On ATmega328P, `useDebugCPUreading` is disabled to preserve flash for the SWEET64 monitor and RAM-program tools.

### C.4 What the Monitor Block Disables

To fit within the 328P flash budget, the monitor block disables all display hardware, button input, clocks, drag race, bar graphs, signal simulation, and other peripherals that are irrelevant to a headless debug session. Key items disabled include `useSimulatedFIandVSS`, `useSoftwareClock`, `useDS1307clock`, `useOutputPins`, `useDragRaceFunction`, all LCD/display options, all button options, Bluetooth/JSON/logging outputs, and board-specific display defines.

### C.5 Flash Budget and Feature Tradeoffs

The 32256-byte usable flash is tight. Representative sizes for key feature combinations:

| Configuration | Flash used |
|---|---|
| Labels only, no help text | ~29728 bytes |
| Labels + help text (attenuated for 328P) | ~32100 bytes |
| Labels + help text + `useSimulatedFIandVSS` | ~34500 bytes |

The help text in `terminalHelp` uses run-length encoding for space characters (`\x01\xNN` expands to N spaces in `text::stringOut`) to reduce flash consumption. For the 328P monitor build, the accumulator math operation entries and the decimal sample output (`U`) command entry are additionally omitted from the help text since they are straightforward enough to use without inline documentation.

The status and trace code is also arranged so the headless monitor can inspect core runtime state without pulling in the LCD/button-facing screens. `^S` remains available for decoded activity/status bytes and SWEET64 error state; uptime appears only when CPU/debug CPU timing support is included.

`useSimulatedFIandVSS` does not fit alongside the full debug terminal on 328P. Use a second Arduino Uno as an external signal generator instead — see [Appendix D](#appendix-d-second-arduino-uno-as-external-signal-generator).

### C.6 Serial Connection

Connect to UART0 (pin 1 = TX, pin 0 = RX) at the configured baud rate, 8N1. The USB-to-serial converter on the Uno board makes this straightforward — use the Arduino IDE Serial Monitor or any terminal program with CR or CR+LF line endings.

### C.7 Primary Workflow

The 328P monitor is meant to support SWEET64 development without an LCD:

1. Use `^F`, `^I`, and `^L` to inspect existing SWEET64 functions.
2. Use `x<yM` to copy a flash function into program RAM.
3. Use `x!` to assemble or patch instructions in RAM.
4. Use `z<y^T` to trace the RAM program.
5. Use `x<y^O` to substitute the RAM program for an existing function.
6. Use `x.y^W` to export the RAM program as paste-ready C source once the behavior is correct.

The same workflow works on Mega2560, but the Uno build is the useful proof that SWEET64 bytecode can be developed on a cheap standalone board.

---

## Appendix D: Second Arduino Uno as External Signal Generator

### D.1 Why

`useSimulatedFIandVSS` — the built-in signal simulator — does not fit in the 328P flash alongside `useSWEET64devMonitor`. A second Uno running dedicated signal-generation firmware is a cleaner solution in any case: it exercises the real hardware interrupt path rather than software-injected signals, and it keeps the device under test running exactly the firmware under development.

### D.2 Signal Characteristics

MPGuino expects two input signals:

**VSS (vehicle speed sensor):**
- A square wave on the VSS interrupt pin
- Frequency proportional to simulated vehicle speed
- Pulse width is not critical; a 50% duty cycle square wave works

**Fuel injector:**
- A pulse train on the injector interrupt pin
- Edge polarity must match `pInjEdgeTriggerIdx`
- The default saturated-injector setting treats falling edge as injector open and rising edge as injector close
- Pulse width proportional to simulated fuel flow
- Frequency should match simulated engine RPM

Both signals are measured in Timer0 cycles by the MPGuino ISRs. Refer to the `idxTicks0PerSecond` and `idxCycles0PerSecond` constant table entries (§7) for the timer rate.

### D.3 Connections

| Signal | Generator Uno pin | MPGuino Uno pin |
|---|---|---|
| VSS | Any digital output | VSS interrupt input |
| Injector | Any digital output | Injector interrupt input |
| GND | GND | GND |

A common ground between the two boards is required.

### D.4 Generator Sketch

A minimal generator sketch uses `tone()` or Timer1 to produce the VSS square wave, and a separate timer or `analogWrite()` channel for the injector pulse train. The exact frequencies needed depend on the VSS pulses-per-mile parameter stored in MPGuino's EEPROM and the injector flow rate parameter — set these in the MPGuino EEPROM via the `P` command first, then size the generator frequencies to produce the desired simulated speed and fuel flow.

---

*This manual was generated from source code analysis of MPGuino v1.95tav (2026-06-02).*
