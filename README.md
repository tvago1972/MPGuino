# MPGuino v1.95tav

An open-source, Arduino-based fuel consumption and trip computer for fuel-injected vehicles. MPGuino measures fuel economy, speed, distance, and many derived quantities in real time by counting VSS (vehicle speed sensor) pulses and fuel injector open-time pulses directly from the vehicle's wiring harness — no OBDII required.

This is a completely rewritten port of Dave Brink's original MPGuino, originally found at https://github.com/Magister54/opengauge.

---

## Core Features

- Real-time and trip-averaged fuel economy (MPG, L/100km, or km/L)
- Current trip and tank accumulators with optional save/restore to EEPROM
- Estimated range to empty and time to empty

## Optional Features
- Idle and engine-off coasting (EOC) fuel tracking
- SAE (imperial) and metric unit modes
- Chrysler Returnless Fuel pressure correction
- Drag race / acceleration test (0–60, 1/8 mile, 1/4 mile)
- Fuel economy bar graphs (vs. time and vs. speed)
- Fuel economy and speed status meters
- Window trip filter (rolling average)
- DS1307 real-time clock support
- Bluetooth serial output
- CSV data logging
- Alternator voltage monitoring

## Supported Hardware

| MCU | Board examples |
|---|---|
| ATmega328P | Arduino Uno, Nano, custom shields |
| ATmega32U4 | Arduino Leonardo, Pro Micro |
| ATmega2560 | Arduino Mega 2560 |

**Display options** (selected at compile time):
- Adafruit RGB LCD Shield (I²C, 16×2)
- Standard parallel LCD (16×2 or 20×4)
- SSD1306 OLED (I²C or SPI)
- ILI9341 TFT colour display (SPI)

## Documentation

- [User Manual](USER_MANUAL.md) — installation, display screens, navigation, settings reference
- [Developer Manual](DEVELOPER_MANUAL.md) — SWEET64 bytecode engine, instruction set reference, debug terminal

## Building

Open `mpguino_tav/mpguino_tav.ino` in the Arduino IDE. Before compiling, edit `mpguino_tav/configs.h` to select your target board and enable the features you need. The file contains comments describing each option and `#error` guards that catch incompatible combinations.

Minimum Arduino IDE version: **1.8.x**. No external library dependencies beyond those bundled with the IDE.

## How It Works

MPGuino samples the vehicle's VSS and injector signals via hardware interrupt pins. Every 500 ms the main loop snapshots the accumulators into three trip slots — **Instant**, **Current trip**, and **Tank** — and runs the selected display calculation through the **SWEET64** bytecode interpreter, a compact 64-bit math engine that evaluates each formula in a few hundred bytes of flash. All settings and trip data are stored in EEPROM.

## License

GPL — see [LICENSE](LICENSE).
