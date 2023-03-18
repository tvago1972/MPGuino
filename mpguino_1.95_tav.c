//
// MPGuino - open source fuel consumption tracking system
// GPL Software, mass production use rights reserved by opengauge.org
// personal use is perfectly fine
// no warranties expressed or implied
//
// This version of MPGuino is only configured for use with saturation-type fuel injectors. It does not
// support peak and hold fuel injectors, due to the relative complexity of having to add many more
// electrical components to properly detect the peak and hold currents for two separate types of
// peak and hold drivers (analog current and PWM). Look up "jaycar peak hold adapter" on the intertubes
// if you want more information

// Special thanks to the good folks at ecomodder.com, ardunio.cc, avrfreaks.net, cadsoft.de, atmel.com,
// and all the folks who donate their time and resources and share their experiences freely

/* External connections:

Legacy MPGuino hardware is defined as anything that MPGuino was originally designed to run on. This includes
   Arduino Duemilanove, Arduino Uno, JellyBeanDriver board, meelis11 board, iDuino, and any other board based off the
   original MPGuino schematic, that can be found at http://ecomodder.com/wiki/index.php/MPGuino

Vehicle interface pins
  legacy MPGuino hardware
    injector sense open  PD2 (INT0), Digital 0
    injector sense close PD3 (INT1), Digital 1
    speed                PC0 (PCINT8), A0
    (if configured) MAP  PC1 (ADC1), A1
    (if configured) Baro PC2 (ADC2), A2
    (if configured) Valt PC2 (ADC2), A2

  TinkerKit! LCD module
    injector sense open  PD2 (INT2), Digital 0 (note this makes it impossible for the TinkerKit! LCD module to use Serial 1)
    injector sense close PD3 (INT3), Digital 1
    speed                PB7 (PCINT7), Digital 11
    (if configured) MAP  PF5 (ADC5), A2
    (if configured) Baro PF6 (ADC6), A1
    (if configured) Valt PF6 (ADC2), A2

  Arduino Mega 2560
    injector sense open  PE4 (INT4), Digital 2
    injector sense close PE5 (INT5), Digital 3
    speed                PK0 (PCINT16), A8
    (if configured) MAP  PF1 (ADC1), A2
    (if configured) Baro PF2 (ADC2), A3
    (if configured) Valt PF2 (ADC2), A2

-------------------------------------
configuration for alternator voltage input to MPGuino (via meelis11)

                     o------------|<|----------------o V(alternator)
                     |            1N4004
                     |
                     |
                     |  R6      R5
                     o--VVV--o--VVV------------------o ground
                        27k  |  9.6k
                             |
                             |                         R6 (JBD)
                             o-----------------------o PC2 - legacy MPGuino hardware
                                                       PF2 - Arduino Mega 2560
                                                       PF6 - TinkerKit! LCD module

-------------------------------------
sensor configuration for Chrysler MAP/baro sensor

          --------------------------------------------
         |                 MAP SENSOR                 |
         |                                            |
         |   +5V             SIGNAL          MAP      |
         |   SUPPLY          GROUND          SIGNAL   |
         |     3               2               1      |
          --------------------------------------------
               o               o               o
               |               |               |                       R7 (JBD)
               |               |               o----------------vvv--o PC1 - legacy MPGuino hardware
               |               |               |                2.2k   PF1 - Arduino Mega 2560
               |               |               |                       PF5 - TinkerKit! LCD module
               o               o               o
          --------------------------------------------
         |   C1-27           C2-27           C2-23    |
         |   +5V             SIGNAL          MAP      |
         |   SUPPLY          GROUND          SIGNAL   |
         |                                            |
         |                  CHRYSLER                  |
         |       NGC POWERTRAIN CONTROL MODULE        |
          --------------------------------------------
                 (older Chrysler PCMs similar)



          --------------------------------------------
         |          Baro SENSOR (optional)            |
         |         (use a spare MAP sensor)           |
         |                                            |
         |   +5V             SIGNAL          Baro     |
         |   SUPPLY          GROUND          SIGNAL   |
         |     3               2               1      |
          --------------------------------------------
               o               o               o
               |               |               |                       R6 (JBD)
               |               |               o----------------vvv--o PC2 - legacy MPGuino hardware
               |               |                                2.2k   PF2 - Arduino Mega 2560
               |               |                                       PF6 - TinkerKit! LCD module
               o               o
          --------------------------------------------
         |   C1-27           C2-27                    |
         |   +5V             SIGNAL                   |
         |   SUPPLY          GROUND                   |
         |                                            |
         |                  CHRYSLER                  |
         |       NGC POWERTRAIN CONTROL MODULE        |
          --------------------------------------------
                 (older Chrysler PCMs similar)

-------------------------------------

LCD Pins - Legacy
  legacy MPGuino hardware
    DIR        PD4, Digital 4
    DB4        PD7, Digital 7
    DB5        PB0, Digital 8
    DB6        PB4, Digital 12
    DB7        PB5, Digital 13
    Enable     PD5, Digital 5
    Contrast   PD6, Digital 6, controlled by PWM on OC0A
    Brightness PB1, Digital 9, controlled by PWM on OC1A

  TinkerKit! LCD module
    RW         PF0, A5
    DIR        PF1, A4
    DB4        PF4, A3
    DB5        PD4, Digital 4
    DB6        PD6, Digital 12
    DB7        PB4, Digital 8
    Enable     PE6, Digital 7
    Contrast   PB5, Digital 9, controlled by PWM on OC1A
    Brightness PB6, Digital 10, controlled by PWM on OC1B

  Arduino Mega 2560
    DIR        PA4, Digital 26
    DB4        PA3, Digital 25
    DB5        PA2, Digital 24
    DB6        PA1, Digital 23
    DB7        PA0, Digital 22
    Enable     PA5, Digital 27
    Contrast   PB7, Digital 13, controlled by PWM on OC0A
    Brightness PB5, Digital 11, controlled by PWM on OC1A

LCD Pins - Parallax Serial Interface
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1

  Arduino Mega 2560
    RX      E1 (TXD0), Digital 1
    - or -
    RX      D3 (TXD1), Digital 18

LCD Pins - Adafruit RGB LCD Shield
  legacy MPGuino hardware
    SDA     PC4 (SDA), A4
    SCL     PC5 (SCL), A5

  TinkerKit! LCD module
    SDA     PD1 (SDA), Digital 2
    SCL     PD0 (SCL), Digital 3

  Arduino Mega 2560
    SCL     PD0 (SCL), Digital 21
    SDA     PD1 (SDA), Digital 20

-------------------------------------

Buttons - Legacy
* Physical layout

    left   middle   right

  legacy MPGuino hardware
    left    PC3 (PCINT11), A3
    middle  PC4 (PCINT12), A4
    right   PC5 (PCINT13), A5

  Arduino Mega 2560
    left    PK3 (PCINT19), A11
    middle  PK4 (PCINT20), A12
    right   PK5 (PCINT21), A13

-------------------------------------

Buttons - Multiplexed Analog (diagram courtesy of josemapiro)
* physical layout

            extra#1

    left	middle	right

            extra#2

  legacy MPGuino hardware
    left, middle, right, extra#1, extra#2 PC3 (ADC3), A3

  TinkerKit! LCD module
    left, middle, right, extra#1, extra#2 PF7 (ADC7), A0

  Arduino Mega 2560
    left, middle, right, extra#1, extra#2 PF3 (ADC3), A3


             o---------------o---------------o---------------o---------------o--o GND
        R2   |          R3   |          R4   |          R5   |          R6   |
     o--vvv--o       o--VVV--o       o--vvv--o       o--vvv--o       o--vvv--o
     |  2.2k         |  4.7k         |  10k          |  22k          |  47k
     o               o               o               o               o
      /               /               /               /               /
     o left          o middle        o right         o Extra#1       o Extra#2
     |               |               |               |               |
     o---------------o---------------o---------------o---------------o--vvv--o--o 5V
                                                                     | R1 1k
                                                                     o----------o PC3 - legacy MPGuino hardware
                                                                                  PF3 - Arduino Mega 2560
                                                                                  PF7 - TinkerKit! LCD module

Buttons - Parallax 5-position switch (diagram based on josemapiro efforts)
          (or any 5-position switch module with 10k pullup resistors on their switches)
  legacy MPGuino hardware
    left, middle, right, extra#1, extra#2 PC3 (ADC3), A3

  TinkerKit! LCD module
    left, middle, right, extra#1, extra#2 PF7 (ADC7), A0

  Arduino Mega 2560
    left, middle, right, extra#1, extra#2 PF3 (ADC3), A3

                                                                                  PF3 - Arduino Mega 2560
                                                                                  PF7 - TinkerKit! LCD module
                                                                     o----------o PC3 - legacy MPGuino hardware
                                                                     | R1 1k
     o---------------o---------------o---------------o---------------o--vvv--o--o 5V
     |               |               |               |               |
     |  left         | middle        |  right        | Extra#1       | Extra#2
     |               |               |               |               |
     |  R2           |  R3           |  R4           |  R5           |  R6
     o--vvv--o       o--VVV--o       o--vvv--o       o--vvv--o       o--vvv--o
        2.2k |          4.7k |          10k  |          22k  |          47k  |
             |               |               |               |               |
             |               |               |               |               |
             o               o               o               o               o
          -----------------------------------------------------------------------
         |   4               6               2               7               3   |
         |   LT              CTR             RT              UP              DN  |
         |                                                                       |
         |                  VCC             GND                                  |
         |                   5               8                                   |
          -----------------------------------------------------------------------
                             o               o
                             |               |
                             |               |
                             o               o----------------------------------O GND
                            N/C

Buttons - Adafruit RGB LCD Shield
  legacy MPGuino hardware
    SCL     PC5 (SCL), A5
    SDA     PC4 (SDA), A4

  TinkerKit! LCD module
    SDA     PD1 (SDA), Digital 2
    SCL     PD0 (SCL), Digital 3

  Arduino Mega 2560
    SCL     PD0 (SCL), Digital 21
    SDA     PD1 (SDA), Digital 20

-------------------------------------

Expansion outputs
  legacy MPGuino hardware
    EXP1       PB2, Digital 10, controlled by PWM on OC1B
    EXP2       PB3, Digital 11, controlled by PWM on OC2A

  TinkerKit! LCD module
    EXP1       PD7, Digital 6, controlled by PWM on OC4D
    EXP2       PC6, Digital 5, controlled by PWM on OC4A

  Arduino Mega 2560
    EXP1       PB6, Digital 12, controlled by PWM on OC1B
    EXP2       PB4, Digital 10, controlled by PWM on OC2A

-------------------------------------

Logging outputs
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1		(USART)

  Arduino Mega 2560
    RX      E1 (TXD0), Digital 1	(USART0)
    - or -
    RX      D3 (TXD1), Digital 18	(USART1)

  TinkerKit! LCD module
    D-								(USB)
    D+

 Program overview
 set up timer hardware
 set up interrupts
 set up system constants
 load system settings from EEPROM
 set up LCD hardware
 (if configured) set up serial UART output

 create accumulators for raw speed/injector data

 mainloop{
 update instantaneous trip, current trip, tank trip, any other setup trip accumulators with raw data accumulators
 reset raw data accumulators
 (if configured) transmit instantaneous trip accumulators
 display computations
 (if configured) transmit outgoing bluetooth computations
 scan for key presses and perform their function (change screen, reset a trip, goto setup, edit screen, restore trips, etc)
 (if configured) scan for incoming bluetooth commands and perform their function
 pause for remainder of 1/2 second
 }

*/

// this section marks the start of the program

//#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

// This code is meant to be copied and pasted to an Arduino IDE sketch window. It works with Arduino IDE 1.0.5 and later.
//
// If AVR command-line tools are used to compile and upload this code, use these fuse settings:
// lfuse:0xf7 hfuse:0xd9 efuse:0x07
// (via Ettore_M)
//
// the following options sets preconfigured board settings, which determine hardware port usage, hardware support checking, and processor speed
// if all of the below preconfigured board "#define"s are commented out, code will compile for an AVR microprocessor specified in either the Arduino IDE or the avr toolchain
//
//#define useJellyBeanDriverBoard true		// also sets LCD and button settings
#define useLegacyBoard true					// also sets LCD and button settings
//#define useArduinoMega2560 true			// (inw)
//#define useTinkerkitLCDmodule true

// the below line sets the serial output speed
//
const unsigned long myBaudRate0 = 19200ul;	// (most Arduino boards)
const unsigned long myBaudRate1 = 19200ul;	// (Atmega2560 board)

// only one of the below LCD options may be chosen - choosing more than one will cause a compilation error to occur
// if useTinkerkitLCDmodule is used, useLegacyLCD will automatically be used, and the below options will be ignored
//
#define useLegacyLCD true					// select Legacy 4-bit parallel LCD output
//#define useParallaxSerialLCDmodule true		// select Serial Interface LCD module output
//#define useAdafruitRGBLCDshield true		// select Adafruit 4-bit RGB LCD module output
//#define useTWILCD true						// select generic LCD 16x2 module output over TWI

// the below options only work if useParallaxSerialLCDmodule is selected. If useParallaxSerialLCDmodule is not selected, the below options will not be inserted at all
//
//#define useLCDserialPort0 true				// select serial LCD module output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLCDserialPort1 true				// select serial LCD module output on serial port channel 1 (Atmega2560 board)

// the below option only works if useLegacyLCD is selected. If useLegacyLCD is not selected, the below option will not be inserted at all
//
//#define useInvertedLegacyLCDbrightness true	// For alternate LCD backlight connections

// only one of the below button options may be chosen - choosing more than one will cause a compilation error to occur
//
// note: if useAdafruitRGBLCDshield is selected, useLegacyButtons will be ignored
//
#define useLegacyButtons true
//#define useAnalogMuxButtons true
//#define useParallax5PositionSwitch true

// selectable options - any conflicts will be reported at compile time
//
//#define trackIdleEOCdata true				// Ability to track engine idling and EOC modes
//#define useSpiffyTripLabels true			// Ability to use enhanced trip labels on main display screens
//#define useSpiffyBigChars true
//#define useSoftwareClock true				// Shows 24 hour clock driven off of timer0, and provides a means to set it
//#define useBigFE true						// Show big fuel economy displays
//#define useBigDTE true						// Show big distance-to-empty displays
//#define useBigTTE true						// Show big time-to-empty displays
//#define useBarFuelEconVsTime true			// Show Fuel Economy over Time bar graph
//#define useBarFuelEconVsSpeed true			// Show Fuel Economy vs Speed, Fuel Used vs Speed bar graphs
//#define usePartialRefuel true				// Provide means to enter partial refuel amount into MPGuino
//#define useSavedTrips true					// Ability to save current or tank trips to EEPROM
//#define useFuelCost true					// Show fuel cost
//#define useCPUreading true					// Show CPU loading and available RAM usage
//#define useChryslerMAPCorrection true		// Ability to perform on-the-fly fuel injector data correction for late-model Chrysler vehicles
//#define useChryslerBaroSensor true			// allows use of a separate MAP sensor wired to MPGuino to read barometric pressure, for even more accurate correction
//#define useExpansionPins true				// Generate analog 0-5VDC output voltage on expansion pins to drive LEDs or feed signal to external gauges
//#define blankScreenOnMessage true			// Completely blank display screen upon display of message
//#define useImperialGallon true				// when selected, uses Imperial gallons instead of default US gallons
//#define useCarVoltageOutput true			// Ability to display alternator voltage and optional secondary sensor (via meelis11)
//#define useDataLoggingOutput true			// Ability to output 5 basic parameters to a data logger or SD card, once every refresh period (0.5 second)

//#define useDeepSleep true					// (inw) places MPGuino into deep sleep after activity timeout
//#define useDragRaceFunction true			// (inw) Performs "drag race" 0-60 MPH, 1/4 mile time, estimated horsepower functionality
//#define useScreenEditor true				// (inw) Ability to change any of (8 to 14, depending on configuration) existing trip data screens, with 4 configurable figures on each screen
//#define useCalculatedFuelFactor true		// (inw) Ability to calculate that pesky us/gal (or L) factor from easily available published fuel injector data
//#define useJSONoutput true					// (inw) skybolt added to enable and call JSON out routine

// performance enhancement options - all may be chosen independently of one another
//
#define useBufferedLCD true					// Speed up non-serial LCD output
//#define useBufferedSerial0Port true			// Speed up serial port 0 output (including serial LCD)
//#define useBufferedSerial1Port true			// Speed up serial port 1 output (including serial LCD)
#define useWindowTripFilter true			// Smooths out "jumpy" instant FE figures that are caused by modern OBDII engine computers
#define useAssemblyLanguage true			// Speeds up many low-level MPGuino functions

// logging output port options - choose one or the other with the above logging output options, or an error will result
//
//#define useLoggingSerialPort0 true			// select logging output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLoggingSerialPort1 true		// select logging output on serial port channel 1 (Atmega2560 board)
//#define useLoggingUSBserial true			// select logging output on USB CDC output (TinkerKit! LCD module)

// debug terminal I/O port options - choose one or the other with useDebugTerminal, or an error will result
//
//#define useDebugTerminalSerialPort0 true	// select serial port channel 0 for PC-MPGuino terminal interface (most Arduino boards, excluding TinkerKit! LCD module)
//#define useDebugTerminalSerialPort1 true	// select serial port channel 1 for PC-MPGuino terminal interface (Atmega2560 board)
//#define useDebugTerminalUSBserial true		// select USB CDC output for PC-MPGuino terminal interface (TinkerKit! LCD module)

// options that are in progress at the moment - don't enable them unless you want to test them
//
//#define useBluetooth true					// (inw) Use bluetooth interface with Android phone in addition to or instead of LCD/buttons
//#define useABresultViewer true				// (inw) Ability to graphically show current (B) versus stored (A) fuel consumption rates
//#define useCoastDownCalculator true			// (inw) Ability to calculate C(rr) and C(d) from coastdown
//#define useFuelParamCalculator true			// (inw) Ability to calculate microseconds per gallon and fuel injector delay stored parameters
//#define useRealTimeClockModule true			// (inw) Ability to read and display Realtime Clock data from an SPI RTC module

// program measurement and debugging tools
//
//#define useDebugTerminal true					// (inw) terminal interface between PC and MPGuino
//#define useDebugTerminalLabels true				// nice labels for various terminal interface output lists - requires useDebugTerminal
//#define useDebugCPUreading true					// Show enhanced CPU loading and available RAM usage - requires useDebugTerminal
//#define useSWEET64trace true					// Ability to view real-time 64-bit calculations from SWEET64 kernel - requires useDebugTerminal
//#define useTestButtonValues true				// Allows observation of button mapping
//#define useSimulatedFIandVSS true				// forces simulation of VSS and fuel injector events
//#define useStatusLEDpins true					// allows usage of LEDs to show status of certain bits - currently used to indicate when MPGuino is awake
//#define useDebugAnalog true						// forces ADC support to be compiled in, along with a dedicated analog screen
//#define useObsoleteMult64 true					// use obsolete slower bit-based 64-bit multiplication
//#define useSWEET64mult true						// shift mult64 from native C++ to SWEET64 bytecode (saves 36 bytes)
//#define useObsoleteDiv64 true					// use obsolete slower 64-bit division
//#define useSWEET64div true						// shift div64 from native C++ to SWEET64 bytecode (saves 220 bytes)

// these #defines are used to select various features to support the above choices
// do not mess with them, or compilation errors will occur
//
#ifdef useArduinoMega2560
#ifdef useTinkerkitLCDmodule
#error *** CANNOT configure for both ArduinoMega2560 and TinkerkitLCDmodule!!! ***
#endif // useTinkerkitLCDmodule
#endif // useArduinoMega2560

#ifdef useArduinoMega2560
#ifdef useJellyBeanDriverBoard
#error *** CANNOT configure for both ArduinoMega2560 and JellyBeanDriverBoard!!! ***
#endif // useJellyBeanDriverBoard
#endif // useArduinoMega2560

#ifdef useArduinoMega2560
#ifdef useLegacyBoard
#error *** CANNOT configure for both ArduinoMega2560 and LegacyBoard!!! ***
#endif // useLegacyBoard
#endif // useArduinoMega2560

#ifdef useTinkerkitLCDmodule
#ifdef useJellyBeanDriverBoard
#error *** CANNOT configure for both TinkerkitLCDmodule and JellyBeanDriverBoard!!! ***
#endif // useJellyBeanDriverBoard
#endif // useTinkerkitLCDmodule

#ifdef useTinkerkitLCDmodule
#ifdef useLegacyBoard
#error *** CANNOT configure for both TinkerkitLCDmodule and LegacyBoard!!! ***
#endif // useLegacyBoard
#endif // useTinkerkitLCDmodule

#ifdef useJellyBeanDriverBoard
#ifdef useLegacyBoard
#error *** CANNOT configure for both JellyBeanDriverBoard and LegacyBoard!!! ***
#endif // useLegacyBoard
#endif // useJellyBeanDriverBoard

#ifdef useTinkerkitLCDmodule
#ifdef useLegacyButtons
#error *** CANNOT configure for TinkerkitLCDmodule and Legacy Buttons!!! ***
#endif // useLegacyButtons
#endif // useTinkerkitLCDmodule

#ifdef useArduinoMega2560
#if !defined(__AVR_ATmega2560__)
#error *** useArduinoMega2560 requires compilation for ATmega2560 board!!! ***
#endif // !defined(__AVR_ATmega2560__)
#endif // useArduinoMega2560

#ifdef useTinkerkitLCDmodule
#if !defined(__AVR_ATmega32U4__)
#error *** useTinkerkitLCDmodule requires compilation for ATmega32U4 board!!! ***
#endif // !defined(__AVR_ATmega32U4__)
#define useLegacyLCD true
#endif // useTinkerkitLCDmodule

#if defined (useJellyBeanDriverBoard) || defined (useLegacyBoard)
#if !defined(__AVR_ATmega328P__)
#error *** Legacy/JBD boards require compilation for ATmega328 board!!! ***
#endif // !defined(__AVR_ATmega328P__)
#define useLegacyLCD true
#define useLegacyButtons true
#ifdef useJellyBeanDriverBoard
#define F_CPU = 20000000UL; // redefine F_CPU
#endif // useJellyBeanDriverBoard
#undef useParallaxSerialLCDmodule
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useAdafruitRGBLCDshield
#undef useStatusLEDpins
#endif // defined (useJellyBeanDriverBoard) || defined (useLegacyBoard)

#if not(__AVR_ATmega32U4__) && not(__AVR_ATmega2560__) && not(__AVR_ATmega328P__)
#warning *** Unknown board - hardware features may not work! ***
#endif // not(__AVR_ATmega32U4__) && not(__AVR_ATmega2560__) && not(__AVR_ATmega328P__)

#ifdef useSoftwareClock
#define useClockDisplay true
#define useBigTimeDisplay true
#endif // useSoftwareClock

#ifdef useBigTTE
#define useBigTimeDisplay true
#endif // useBigTTE

#ifdef useBigDTE
#define useBigNumberDisplay true
#endif // useBigDTE

#ifdef useBigFE
#define useBigNumberDisplay true
#endif // useBigFE

#ifdef useBigTimeDisplay
#define useBigDigitDisplay true
#endif // useBigTimeDisplay

#ifdef useBigNumberDisplay
#define useBigDigitDisplay true
#endif // useBigDTE

#ifdef useBarFuelEconVsTime
#define useBarGraph true
#endif // useBarFuelEconVsTime

#ifdef useBarFuelEconVsSpeed
#define useBarGraph true
#endif // useBarFuelEconVsSpeed

#if useSpiffyTripLabels || useBigDigitDisplay || useBarGraph
#define useLCDgraphics true
#endif // useSpiffyTripLabels || useBigDigitDisplay || useBarGraph

#ifdef useCalculatedFuelFactor
#define useIsqrt true
#endif // useCalculatedFuelFactor

#ifdef useDebugAnalog
#define useAnalogRead true
#endif // useDebugAnalog

#ifndef useCPUreading
#undef useDebugCPUreading
#endif // useCPUreading

#ifndef useObsoleteMult64
#undef useSWEET64mult
#endif // useObsoleteMult64

#ifdef useObsoleteDiv64
#undef useDiv64Assembly
#else // useObsoleteDiv64
#ifdef useAssemblyLanguage
#define useDiv64Assembly true
#else // useAssemblyLanguage
#undef useDiv64Assembly
#endif // useAssemblyLanguage
#undef useSWEET64div
#endif // useObsoleteMult64

#ifdef useChryslerMAPCorrection
#define useIsqrt true
#define useAnalogRead true
#else // useChryslerMAPCorrection
#undef useChryslerBaroSensor
#endif // useChryslerMAPCorrection

#ifdef useCarVoltageOutput
#define useAnalogRead true
#endif // useCarVoltageOutput

#ifdef useCoastDownCalculator
#define useVehicleMass true
#define useMatrixMath true
#endif // useCoastDownCalculator

#ifdef useFuelParamCalculator
#define useMatrixMath true
#endif // useFuelParamCalculator

#ifdef useParallaxSerialLCDmodule
#define useSerialLCD true
#define useLCDoutput true
#define useBinaryLCDbrightness true
#endif // useParallaxSerialLCDmodule

#if not(useSerialLCD)
#undef useLCDserialPort0
#undef useLCDserialPort1
#endif // not(useSerialLCD)

#if not(useJSONoutput) && not(useDataLoggingOutput)
#undef useLoggingSerialPort0
#undef useLoggingSerialPort1
#undef useLoggingUSBserial
#endif // not(useJSONoutput) && not(useDataLoggingOutput)

#if not(useDebugTerminal)
#undef useDebugTerminalSerialPort0
#undef useDebugTerminalSerialPort1
#undef useDebugTerminalUSBserial
#undef useSWEET64trace
#undef useDebugCPUreading
#endif // not(useDebugTerminal)

#ifdef useLCDserialPort0
#define useSerial0Port true
#define devLCDserial devSerial0
#define LCDserialPort serial0
#ifdef useBufferedSerial0Port
#define LCDserialBuffer serial0Buffer
#endif // useBufferedSerial0Port
#endif // useLCDserialPort0

#ifdef useLCDserialPort1
#define useSerial1Port true
#define devLCDserial devSerial1
#define LCDserialPort serial1
#ifdef useBufferedSerial1Port
#define LCDserialBuffer serial1Buffer
#endif // useBufferedSerial1Port
#endif // useLCDserialPort1

#ifdef useLoggingSerialPort0
#define useSerial0Port true
#define devLogOutput devSerial0
#define outputLoggingSplash true
#endif // useLoggingSerialPort0

#ifdef useLoggingSerialPort1
#define useSerial1Port true
#define devLogOutput devSerial1
#define outputLoggingSplash true
#endif // useLoggingSerialPort1

#ifdef useLoggingUSBserial
#define useUSBserial true
#define devLogOutput devUSB
#define outputLoggingSplash true
#endif // useLoggingUSBserial

#ifdef useDebugTerminalSerialPort0
#define useSerial0Port true
#define useSerial0PortInput true
#define devDebugTerminal devSerial0
#define outputDebugTerminalSplash true
#endif // useDebugTerminalSerialPort0

#ifdef useDebugTerminalSerialPort1
#define useSerial1Port true
#define useSerial1PortInput true
#define devDebugTerminal devSerial1
#define outputDebugTerminalSplash true
#endif // useDebugTerminalSerialPort1

#ifdef useDebugTerminalUSBserial
#define useUSBserial true
#define devDebugTerminal devUSB
#define outputDebugTerminalSplash true
#endif // useDebugTerminalUSBserial

#if defined(useBufferedSerial0Port) || defined(useBufferedSerial1Port) || defined(useBufferedLCD) || defined(useSerial0PortInput) || defined(useSerial1PortInput) || defined(useUSBserial)
#define useBuffering true
#endif // defined(useBufferedSerial0Port) || defined(useBufferedSerial1Port) || defined(useBufferedLCD) || defined(useSerial0PortInput) || defined(useSerial1PortInput) || defined(useUSBserial)

#if useAnalogMuxButtons || useParallax5PositionSwitch
#define useAnalogButtons true
#endif // useAnalogMuxButtons || useParallax5PositionSwitch

#ifdef useLegacyLCD
#define use4BitLCD true
#endif // useLegacyLCD

#ifdef useAdafruitRGBLCDshield
#define useTWILCD true
#ifndef useAnalogButtons
#undef useLegacyButtons
#define useTWIbuttons true
#endif // useAnalogButtons
#endif // useAdafruitRGBLCDshield

#ifdef useTWILCD
#define useTWIsupport true
#define use4BitLCD true
#define useBinaryLCDbrightness true
#endif // useTWILCD

#ifdef use4BitLCD
#define useTimer1Interrupt true
#define useLCDoutput true
#endif // use4BitLCD

#ifdef useAnalogButtons
#define useAnalogRead true
#ifdef useTestButtonValues
#define useTestAnalogButtonIdx true
#endif // useTestButtonValues
#endif // useAnalogButtons

#ifdef useTWIbuttons
#define useTWIsupport true
#endif // useTWIbuttons

#ifdef useSimulatedFIandVSS
#define useTimer1Interrupt true
#endif // useSimulatedFIandVSS

#ifdef useTimer1Interrupt
#define useTimer1 true
#endif // useTimer1Interrupt

#ifdef useExpansionPins
#if defined(__AVR_ATmega32U4__)
#define useTimer4 true
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#define useTimer2 true
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#define useTimer1 true
#define useTimer2 true
#endif // defined(__AVR_ATmega328P__)
#endif // useExpansionPins

#ifdef useDragRaceFunction
#define useVehicleMass true
#endif // useDragRaceFunction

#ifdef useSavedTrips
#define useEEPROMtripStorage true
#endif // useSavedTrips

// this section catches configuration errors

#ifdef useTWIsupport
#ifdef useLegacyButtons
#if !defined(__AVR_ATmega2560__)
#error *** CANNOT configure for Legacy pushbuttons and TwoWire support!!! ***
#endif // !defined(__AVR_ATmega2560__)
#endif // useLegacyButtons
#endif // useTWIsupport

#if not(useLegacyButtons) && not(useAnalogButtons) && not(useTWIbuttons)
#error *** MPGuino button configuration not specified!!! ***
#endif // not(useLegacyButtons) && not(useAnalogButtons) && not(useTWIbuttons)

#if defined(useLegacyButtons) && defined(useAnalogButtons) || defined(useLegacyButtons) && defined(useTWIbuttons)
#error *** Button conflict exists between Legacy buttons and something else!!! ***
#endif // defined(useLegacyButtons) && defined(useAnalogButtons) || defined(useLegacyButtons) && defined(useTWIbuttons)

#if defined(useAnalogButtons) && defined(useTWIbuttons)
#error *** Button conflict exists between Analog buttons and something else!!! ***
#endif // defined(useAnalogButtons) && defined(useTWIbuttons)

#if defined(useLCDserialPort0) && defined(useLoggingSerialPort0) || defined(useLCDserialPort0) && defined(useDebugTerminalSerialPort0)
#error *** Serial Port 0 conflict exists between Serial LCD and something else!!! ***
#endif // defined(useLCDserialPort0) && defined(useLoggingSerialPort0) || defined(useLCDserialPort0) && defined(useDebugTerminalSerialPort0)

#if defined(useLoggingSerialPort0) && defined(useDebugTerminalSerialPort0)
#error *** Serial Port 0 conflict exists between Logging Output and something else!!! ***
#endif // defined(useLoggingSerialPort0) && defined(useDebugTerminalSerialPort0)

#if defined(useLCDserialPort1) && defined(useLoggingSerialPort1) || defined(useLCDserialPort1) && defined(useDebugTerminalSerialPort1)
#error *** Serial Port 1 conflict exists between Serial LCD and something else!!! ***
#endif // defined(useLCDserialPort1) && defined(useLoggingSerialPort1) || defined(useLCDserialPort1) && defined(useDebugTerminalSerialPort1)

#if defined(useLoggingSerialPort1) && defined(useDebugTerminalSerialPort1)
#error *** Serial Port 1 conflict exists between Logging Output and something else!!! ***
#endif // defined(useLoggingSerialPort1) && defined(useDebugTerminalSerialPort1)

#if defined(useDebugTerminalUSBserial) && defined(useLoggingUSBserial)
#error *** USB port conflict exists between Logging Output and something else!!! ***
#endif // defined(useDebugTerminalUSBserial) && defined(useLoggingUSBserial)

#if defined(useJSONoutput) && defined(useDataLoggingOutput)
#error *** CANNOT use useJSONoutput and useDataLoggingOutput!!! ***
#endif // defined(useJSONoutput) && defined(useDataLoggingOutput)

#ifdef useSerialLCD
#if defined(useLCDserialPort0) && defined(useLCDserialPort1)
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial LCD Output!!! ***
#endif // defined(useLCDserialPort0) && defined(useLCDserialPort1)
#if not(useLCDserialPort0) && not(useLCDserialPort1)
#error *** Serial LCD Output needs a serial port assigned to it!!! ***
#endif // not(useLCDserialPort0) && not(useLCDserialPort1)
#endif // useSerialLCD

#ifdef useDebugTerminal
#if useDebugTerminalSerialPort0 && useDebugTerminalSerialPort1 || useDebugTerminalSerialPort0 && useDebugTerminalUSBserial || useDebugTerminalSerialPort1 && useDebugTerminalUSBserial
#error *** CANNOT use more than one I/O port for Terminal Output!!! ***
#endif // useDebugTerminalSerialPort0 && useDebugTerminalSerialPort1 || useDebugTerminalSerialPort0 && useDebugTerminalUSBserial || useDebugTerminalSerialPort1 && useDebugTerminalUSBserial
#if not(useDebugTerminalSerialPort0) && not(useDebugTerminalSerialPort1) && not(useDebugTerminalUSBserial)
#error *** Terminal Output needs an I/O port assigned to it!!! ***
#endif // not(useDebugTerminalSerialPort0) && not(useDebugTerminalSerialPort1) && not(useDebugTerminalUSBserial)
#endif // useDebugTerminal

#if useLoggingSerialPort0 && useLoggingSerialPort1 || useLoggingUSBserial && useLoggingSerialPort0 || useLoggingUSBserial && useLoggingSerialPort1
#ifdef useJSONoutput
#error *** CANNOT use more than one output port for JSON Output!!! ***
#endif // useJSONoutput
#ifdef useDataLoggingOutput
#error *** CANNOT use more than one output port for Logging Output!!! ***
#endif // useDataLoggingOutput
#endif // useLoggingSerialPort0 && useLoggingSerialPort1 || useLoggingUSBserial && useLoggingSerialPort0 || useLoggingUSBserial && useLoggingSerialPort1
#if not(useLoggingSerialPort0) && not(useLoggingSerialPort1) && not(useLoggingUSBserial)
#ifdef useJSONoutput
#error *** JSON Output needs an output port assigned to it!!! ***
#endif // useJSONoutput
#ifdef useDataLoggingOutput
#error *** Logging Output needs an output port assigned to it!!! ***
#endif // useDataLoggingOutput
#endif // not(useLoggingSerialPort0) && not(useLoggingSerialPort1) && not(useLoggingUSBserial)

#ifdef useUSBserial
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useUSBserial!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(__AVR_ATmega2560__)
#error *** Cannot use ATmega2560 hardware and useUSBserial!!! ***
#endif // defined(__AVR_ATmega2560__)
#endif // useUSBserial

#ifdef useSerial1Port
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useSerial1Port!!! ***
#endif // defined(__AVR_ATmega328P__)
#ifdef useTinkerkitLCDmodule
#error *** Serial Port 1 on Tinkerkit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // useTinkerkitLCDmodule
#endif // useSerial1Port

#if useSerial0Port && __AVR_ATmega32U4__
#error *** Cannot use useTinkerkitLCDmodule/ATmega32U4 and useSerial0Port!!! ***
#endif // useSerial0Port && __AVR_ATmega32U4__

// Really, dcb? Really? You couldn't have used something else for LCD data bit 3?
#if useLegacyLCD && __AVR_ATmega328P__ && useStatusLEDpins
#error *** CANNOT use useLegacyLCD and useStatusLEDpins!!! ***
#endif // useLegacyLCD && __AVR_ATmega328P__ && useStatusLEDpins

#if useSoftwareClock && useDeepSleep
#error *** CANNOT use both useSoftwareClock and useDeepSleep!!! ***
#endif // useSoftwareClock && useDeepSleep

#if useCarVoltageOutput && useChryslerBaroSensor
#error *** CANNOT use both useCarVoltageOutput and useChryslerBaroSensor!!! ***
#endif // useCarVoltageOutput && useChryslerBaroSensor

// this is the start of the actual program code

// This is where direct-ported (not analog) pushbutton definitions should go

#ifdef useTWIbuttons
#ifdef useAdafruitRGBLCDshield
#define useButtonCrossConfig true

const uint8_t buttonAddress =	0x20;

const uint8_t buttonLbit = 			0b00010000; // GPIO A bit 4, left button
const uint8_t buttonCbit = 			0b00000001; // GPIO A bit 0, select button
const uint8_t buttonRbit = 			0b00000010; // GPIO A bit 1, right button
const uint8_t buttonUbit = 			0b00001000; // GPIO A bit 3, up button
const uint8_t buttonDbit = 			0b00000100; // GPIO A bit 2, down button

const uint8_t longButtonBit =		0b00100000; // GPIO A bit 5 isn't being used for anything right now

const uint8_t buttonMask =			buttonUbit | buttonDbit | buttonLbit | buttonCbit | buttonRbit;

#else // useAdafruitRGBLCDshield
	// place any TWI button definitions here

#endif // useAdafruitRGBLCDshield
#endif // useTWIbuttons
#ifdef useAnalogButtons
#ifdef useParallax5PositionSwitch
#define useButtonCrossConfig true

const uint8_t buttonLbit = 			0b00000001;
const uint8_t buttonCbit = 			0b00000010;
const uint8_t buttonRbit = 			0b00000100;
const uint8_t buttonUbit = 			0b00001000;
const uint8_t buttonDbit = 			0b00010000;
#endif // useParallax5PositionSwitch
#ifdef useAnalogMuxButtons
#define useButtonCrossConfig true

const uint8_t buttonLbit = 			0b00000001;
const uint8_t buttonCbit = 			0b00000010;
const uint8_t buttonRbit = 			0b00000100;
const uint8_t buttonUbit = 			0b00001000;
const uint8_t buttonDbit = 			0b00010000;
#endif // useAnalogMuxButtons

const uint8_t longButtonBit =		0b10000000;

const uint8_t buttonMask =			buttonUbit | buttonDbit | buttonLbit | buttonCbit | buttonRbit;

#endif // useAnalogButtons
#ifdef useLegacyButtons
#if defined(__AVR_ATmega2560__)
const uint8_t buttonLbit = 			(1 << PINK3);
const uint8_t buttonCbit = 			(1 << PINK4);
const uint8_t buttonRbit = 			(1 << PINK5);

const uint8_t longButtonBit =		(1 << PINK6); // PINK6 isn't being used for anything right now

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
const uint8_t buttonLbit = 			(1 << PINC3);
const uint8_t buttonCbit = 			(1 << PINC4);
const uint8_t buttonRbit = 			(1 << PINC5);

const uint8_t longButtonBit =		(1 << PINC6); // PINC6 is used as the RESET pin, so this value is safe to use for long-press signalling

#endif // defined(__AVR_ATmega328P__)
const uint8_t buttonMask =			buttonLbit | buttonCbit | buttonRbit;

#endif // useLegacyButtons
const uint8_t buttonsUp =			0;
const uint8_t btnShortPressL =		buttonLbit;
const uint8_t btnShortPressC =		buttonCbit;
const uint8_t btnShortPressR =		buttonRbit;
const uint8_t btnShortPressLC =		buttonCbit | buttonLbit;
const uint8_t btnShortPressLR =		buttonRbit | buttonLbit;
const uint8_t btnShortPressCR =		buttonRbit | buttonCbit;
const uint8_t btnShortPressLCR =	buttonRbit | btnShortPressLC;

#ifdef useButtonCrossConfig
const uint8_t btnShortPressU =		buttonUbit;
const uint8_t btnShortPressUL =		buttonUbit | btnShortPressL;
const uint8_t btnShortPressUC =		buttonUbit | btnShortPressC;
const uint8_t btnShortPressULC =	buttonUbit | btnShortPressLC;
const uint8_t btnShortPressUR =		buttonUbit | btnShortPressR;
const uint8_t btnShortPressULR =	buttonUbit | btnShortPressLR;
const uint8_t btnShortPressUCR =	buttonUbit | btnShortPressCR;
const uint8_t btnShortPressULCR =	buttonUbit | btnShortPressLCR;

const uint8_t btnShortPressD =		buttonDbit;
const uint8_t btnShortPressDL =		buttonDbit | btnShortPressL;
const uint8_t btnShortPressDC =		buttonDbit | btnShortPressC;
const uint8_t btnShortPressDLC =	buttonDbit | btnShortPressLC;
const uint8_t btnShortPressDR =		buttonDbit | btnShortPressR;
const uint8_t btnShortPressDLR =	buttonDbit | btnShortPressLR;
const uint8_t btnShortPressDCR =	buttonDbit | btnShortPressCR;
const uint8_t btnShortPressDLCR =	buttonDbit | btnShortPressLCR;

const uint8_t btnShortPressUD =		buttonDbit | btnShortPressU;
const uint8_t btnShortPressUDL =	buttonDbit | btnShortPressUL;
const uint8_t btnShortPressUDC =	buttonDbit | btnShortPressUC;
const uint8_t btnShortPressUDLC =	buttonDbit | btnShortPressULC;
const uint8_t btnShortPressUDR =	buttonDbit | btnShortPressUR;
const uint8_t btnShortPressUDLR =	buttonDbit | btnShortPressULR;
const uint8_t btnShortPressUDCR =	buttonDbit | btnShortPressUCR;
const uint8_t btnShortPressUDCRL =	buttonDbit | btnShortPressULCR;
#endif // useButtonCrossConfig

const uint8_t btnLongPressL =		longButtonBit | btnShortPressL;
const uint8_t btnLongPressC =		longButtonBit | btnShortPressC;
const uint8_t btnLongPressLC =		longButtonBit | btnShortPressLC;
const uint8_t btnLongPressR =		longButtonBit | btnShortPressR;
const uint8_t btnLongPressLR =		longButtonBit | btnShortPressLR;
const uint8_t btnLongPressCR =		longButtonBit | btnShortPressCR;
const uint8_t btnLongPressLCR =		longButtonBit | btnShortPressLCR;
#ifdef useButtonCrossConfig

const uint8_t btnLongPressU =		longButtonBit | btnShortPressU;
const uint8_t btnLongPressUL =		longButtonBit | btnShortPressUL;
const uint8_t btnLongPressUC =		longButtonBit | btnShortPressUC;
const uint8_t btnLongPressULC =		longButtonBit | btnShortPressULC;
const uint8_t btnLongPressUR =		longButtonBit | btnShortPressUR;
const uint8_t btnLongPressULR =		longButtonBit | btnShortPressULR;
const uint8_t btnLongPressUCR =		longButtonBit | btnShortPressUCR;
const uint8_t btnLongPressULCR =	longButtonBit | btnShortPressULCR;

const uint8_t btnLongPressD =		longButtonBit | btnShortPressD;
const uint8_t btnLongPressDL =		longButtonBit | btnShortPressDL;
const uint8_t btnLongPressDC =		longButtonBit | btnShortPressDC;
const uint8_t btnLongPressDLC =		longButtonBit | btnShortPressDLC;
const uint8_t btnLongPressDR =		longButtonBit | btnShortPressDR;
const uint8_t btnLongPressDLR =		longButtonBit | btnShortPressDLR;
const uint8_t btnLongPressDCR =		longButtonBit | btnShortPressDCR;
const uint8_t btnLongPressDLCR =	longButtonBit | btnShortPressDLCR;

const uint8_t btnLongPressUD =		longButtonBit | btnShortPressUD;
const uint8_t btnLongPressUDL =		longButtonBit | btnShortPressUDL;
const uint8_t btnLongPressUDC =		longButtonBit | btnShortPressUDC;
const uint8_t btnLongPressUDLC =	longButtonBit | btnShortPressUDLC;
const uint8_t btnLongPressUDR =		longButtonBit | btnShortPressUDR;
const uint8_t btnLongPressUDLR =	longButtonBit | btnShortPressUDLR;
const uint8_t btnLongPressUDCR =	longButtonBit | btnShortPressUDCR;
const uint8_t btnLongPressUDCRL =	longButtonBit | btnShortPressUDCRL;

#endif // useButtonCrossConfig
#if defined(__AVR_ATmega32U4__)
#ifndef PRTIM4
#define PRTIM4 4

#endif // PRTIM4
#endif // defined(__AVR_ATmega32U4__)
typedef void (* pFunc)(void); // type for display function pointers

const uint8_t loopsPerSecond = 2; // how many times will we try and loop in a second (also for sampling non-critical ADC channels, if configured)
#ifdef useAnalogButtons
const uint8_t analogSamplesPerSecond = 32; // how many times will we try to sample ADC button presses in a second
#endif // useAnalogButtons
#ifdef useTWIbuttons
const uint8_t TWIsamplesPerSecond = 32; // how many times will we try to sample TWI button presses in a second
#endif // useTWIbuttons

const unsigned long t0CyclesPerSecond = (unsigned long)(F_CPU / 64ul); // (systemProcessorSpeed * 1000000 / (timer 0 prescaler))
const unsigned long t0TicksPerSecond = t0CyclesPerSecond / 256ul;

const unsigned int loopTickLength = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(loopsPerSecond * 256ul)) - 1;
const unsigned int buttonLongPressTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul)); // 1 second delay
const unsigned int cursorDelayTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 2ul)) - 1; // display cursor toggle period is 500 milliseconds
const unsigned int swapFEwithFCRdelay = (unsigned int)(ceil)((double)(3ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 3 second delay
const unsigned int holdDelay = (unsigned int)(ceil)((double)(2ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 2 second delay
const unsigned int delay1500msTick = (unsigned int)(ceil)((double)(15ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.5 second delay
const unsigned int delay0005msTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 200ul)) - 1; // 5 millisecond delay
#ifdef useLegacyButtons
const unsigned int buttonDebounceTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 20ul)) - 1; // 50 millisecond delay button debounce
#endif // useLegacyButtons
#ifdef useAnalogButtons
const unsigned int analogSampleTickLength  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(analogSamplesPerSecond * 256ul)) - 1;
#endif // useAnalogButtons
#ifdef useTWIbuttons
const unsigned int TWItickLength  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(TWIsamplesPerSecond * 256ul)) - 1;
#endif // useTWIbuttons
#ifdef useJSONoutput
const unsigned int JSONtickLength = (unsigned int)(ceil)((double)(16ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.6 second delay
#endif // useJSONoutput

union union_16
{

	uint16_t ui;
	uint8_t u8[2];

};

union union_32
{

	uint32_t ul;
	uint16_t ui[2];
	uint8_t u8[4];

};

union union_64
{

	uint64_t ull;
	uint32_t ul[2];
	uint16_t ui[4];
	uint8_t u8[8];

};

uint8_t odvFlagCRLF =				0b00000100;
uint8_t odvFlagShootBlanks =		0b00000010;
uint8_t odvFlagEnableOutput =		0b00000001;

typedef struct
{

	uint8_t controlFlags;
	void (* chrOut)(uint8_t character);
	uint8_t (* chrIn)(void);

} interfaceDevice;

typedef struct
{

	uint8_t isValid;
	uint8_t tripIdx;
	uint8_t tripChar;
	uint8_t calcIdx;
	uint8_t calcFmtIdx;
	uint8_t decimalPlaces;
	uint8_t calcChar;
	char * strBuffer;

} calcFuncObj;

static void updateVSS(unsigned long thisVSStime);
#ifdef useIsqrt
static unsigned int iSqrt(unsigned int n);
#endif // useIsqrt
unsigned long findCycleLength(unsigned long lastCycle, unsigned long thisCycle);
void delay0(unsigned int ms);
const char * findStr(const char * str, uint8_t strIdx);
static void printStatusMessage(const char * s);
static void printStatusMessage(const char * s, uint8_t strIdx);
void doNothing(void);
void noSupport(void);
void initStatusLine(void);
void execStatusLine(void);
static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag);
#ifdef useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
#else // useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos);
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx);
#endif // useSpiffyTripLabels
char findDigit(uint8_t value, char &zeroChar);
unsigned long str2ull(char * strBuffer);
char * ull2str(uint8_t prgmIdx, char * strBuffer, uint8_t decimalPlaces);
char * formatDecimal(char * strBuffer, uint8_t windowLength, uint8_t decimalPlaces, uint8_t decimalFlag);
uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged); /* Main screen section */
void doReturnToMainScreen(void);
void doNextBright(void);
#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void);
#endif // useDeepSleep
#ifdef useScreenEditor // Programmable main display screen edit support section
void doCursorUpdateScreenEdit(void);
void doScreenEditDisplay(void);
void doGoScreenEdit(void);
void doScreenEditBump(void);
void doSaveScreen(void);
void doScreenReturnToMain(void);
#endif // useScreenEditor
#ifdef useWindowTripFilter
void windowTripFilterReset(void);
#endif // useWindowTripFilter
#ifdef useJSONoutput
void doOutputJSONnumber(const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces, const char * labelStr); // SWEET64::runPrgm
void doOutputJSONnumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalPlaces, const char * labelStr); // SWEET64::doCalculate
void doOutputJSONnumber(unsigned long an, uint8_t decimalPlaces, const char * labelStr);
void doOutputJSONnumber(union union_64 * an, uint8_t decimalPlaces, const char * labelStr);
void doOutputJSONnumber(uint8_t decimalPlaces, const char * labelStr); // base
void doOutputJSON(void);
#endif // useJSONoutput
#ifdef useDataLoggingOutput
static void doOutputDataLog(void);
#endif // useDataLoggingOutput
static uint8_t translateTripIndex(const uint8_t tripTranslateList[], uint8_t tripListPos);
static void idleProcess(void); // place all time critical main program internal functionality here - no I/O!
static void performSleepMode(uint8_t sleepMode);
int main(void);

namespace tripVar /* trip data collection basic function prototype */
{

	static void reset(uint8_t tripIdx);
	static void transfer(uint8_t srcTripIdx, uint8_t destTripIdx);
	static void update(uint8_t srcTripIdx, uint8_t destTripIdx);
	static void add64(uint64_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx);
	static void add64(uint64_t collectedArray[], uint32_t value, uint8_t destTripIdx);
	static void add32(uint32_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx);
#ifdef useEEPROMtripStorage
	static uint8_t getBaseEEPROMaddress(uint8_t tripIdx, uint8_t dataIdx);
#endif // useEEPROMtripStorage

};

namespace SWEET64 /* 64-bit pseudo-processor section prototype */
{

#ifdef useSWEET64trace
	static void listProgram(uint8_t calcIdx);
#endif // useSWEET64trace
	static uint32_t doCalculate(uint8_t tripIdx, uint8_t calcIdx);
	static uint32_t runPrgm(const uint8_t * sched, uint8_t tripIdx);
	static uint8_t fetchByte(const uint8_t * &prgmPtr);
	static void copy64(union union_64 * an, union union_64 * ann);
	static void swap64(union union_64 * an, union union_64 * ann);
	static void shr64(union union_64 * an);
	static void shl64(union union_64 * an);
	static void adc64(union union_64 * an, union union_64 * ann);
	static void sbc64(union union_64 * an, union union_64 * ann, uint8_t sbcFlag);
#ifndef useSWEET64mult
	static void mult64(void);
#endif // useSWEET64mult
#ifndef useSWEET64div
	static void div64(void);
#endif // useSWEET64div
	static void init64byt(union union_64 * an, uint8_t byt);
	static void init64(union union_64 * an, uint32_t dWordL);
	static void flagSet(uint8_t condition, uint8_t flag);
	static void flagSet64(uint8_t n, uint8_t z, uint8_t c);
	static void registerTest64(union union_64 * an);

}

namespace peripheral /* MPGuino human interface I/O peripheral device prototype */
{

	static void initMain(void);
	static void shutdownMain(void);
	static void initButton(void);
	static void shutdownButton(void);
	static void changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR);
#if useAnalogButtons || useDebugTerminal
	static void injectButton(uint8_t buttonValue);
#endif // useAnalogButtons || useDebugTerminal
#ifdef useExpansionPins
	static void initExpansionPin(void);
	static void shutdownExpansionPin(void);
	static void outputExpansionPin1(uint8_t pin);
	static void outputExpansionPin2(uint8_t pin);
#endif // useExpansionPins
#if defined(__AVR_ATmega32U4__)
	static void initUSB(void);
	static void shutdownUSB(void);
#endif // defined(__AVR_ATmega32U4__)
#ifdef useStatusLEDpins
	static void initStatusLED(void);
	static void shutdownStatusLED(void);
	static void outputStatusLED(uint8_t val);
#endif // useStatusLEDpins

};

namespace EEPROM /* EEPROM parameter I/O section prototype */
{

	static uint8_t powerUpCheck(void);
	static void initGuino(void);
	static void initGuinoHardware(void);
	static void initGuinoSoftware(void);
	static uint8_t readVal(uint8_t eePtr);
	static void writeVal(uint8_t eePtr, uint32_t value);
	static void read64(union union_64 * an, uint8_t parameterIdx);
	static void write64(union union_64 * an, uint8_t parameterIdx);
	static uint16_t getAddress(uint8_t eePtr);
	static uint8_t getParameterFlags(uint8_t eePtr);
	static uint8_t getLength(uint8_t eePtr);

};

uint8_t menuLevel;
uint8_t topScreenLevel;
uint8_t brightnessIdx;

namespace cursor /* LCD screen cursor manipulation section prototype */
{

	static void screenLevelEntry(const char * str, uint8_t newScreenLevel);
	static void screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel);
	static void moveAbsolute(uint8_t positionY, uint8_t positionX);
	static void moveRelative(uint8_t moveY, uint8_t moveX);
	static void updateAfterMove(uint8_t levelChangeFlag);
	static void shortLeft(void);
	static void shortRight(void);
	static void longLeft(void);
	static void longRight(void);

};

const uint8_t menuExitIdx =				0;								// this call may not even be necessary
const uint8_t menuEntryIdx =			menuExitIdx + 1;				// typically, this call will fall through
const uint8_t menuCursorUpdateIdx =		menuEntryIdx + 1;				// ...to this call, then will fall through
const uint8_t menuOutputDisplayIdx =	menuCursorUpdateIdx + 1;		// ...to this call

typedef uint8_t (* displayHandlerFunc)(uint8_t, uint8_t, uint8_t); // type for various handler functions

namespace menu /* Top-down menu selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void select(void);

};

const uint8_t nesLoadInitial =			0;
const uint8_t nesLoadValue =			nesLoadInitial + 1;
const uint8_t nesOnChange =				nesLoadValue + 1;
const uint8_t nesSaveParameter =		nesOnChange + 1;
const uint8_t nesOutputUpperScreen =	nesSaveParameter + 1;

typedef struct
{

	uint8_t callingScreenLevel; // remember what module called number editor
	uint8_t parameterIdx;
	const char * neStatusMessage;

} numberEditType;

static numberEditType numberEditObj;

namespace parameterEdit /* parameter editor/entry section prototype */
{

	static uint8_t sharedFunctionCall(uint8_t cmd);
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void findLeft(void);
	static void findRight(void);
	static void readInitial(void);
	static void readMinValue(void);
	static void readMaxValue(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void changeDigit(uint8_t dir);
	static void save(void);
	static void cancel(void);
	static uint8_t onEEPROMchange(const uint8_t * sched, uint8_t parameterIdx);

};

namespace settings /* Parameter setting selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

};

#ifdef useClockDisplay
namespace clockSet /* Big Clock Display support section prototype */
{

	static void entry(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void set(void);
	static void cancel(void);

};

#endif // useClockDisplay
#ifdef useBigDigitDisplay
namespace bigDigit /* Big Digit output support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void loadCGRAMnumberFont(void);
#ifdef useBigTimeDisplay
	static void outputTime(char * val, uint8_t blinkFlag, uint8_t blinkPos);
#endif // useBigTimeDisplay
#ifdef useBigNumberDisplay
	static uint8_t outputNumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength);
#endif // useBigNumberDisplay
	static void outputNumberString(char * str);
	static void outputDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar);
	static void displayStatus(const char * str, uint8_t cursorPos);

};

#endif // useBigDigitDisplay
#ifdef useCPUreading

uint32_t mainStart;
uint32_t idleTimerLength;
#ifdef useDebugCPUreading
uint8_t monitorState;
uint32_t idleProcessTimerLength;
uint32_t displayTimerLength;
uint32_t SWEET64timerLength;
#endif // useDebugCPUreading

namespace systemInfo /* CPU loading and RAM availability support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void showCPUload(void);
	static void showCPUloading(void);
	static uint16_t getAvailableRAMsize(void);
	static uint32_t findCycleLength(unsigned long lastCycle, unsigned long thisCycle);
	static uint32_t cycles0(void);

};

extern char __bss_end;
extern char *__brkval;

#endif // useCPUreading
#ifdef useSimulatedFIandVSS
namespace debugReading /* VSS / fuel injector on-board simulator support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void configurePorts(void);

}

#endif // useSimulatedFIandVSS
#ifdef usePartialRefuel
namespace partialRefuel /* partial refuelling support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void select(void);
	static void longSelect(void);

}

#endif // usePartialRefuel
#ifdef useChryslerMAPCorrection
namespace pressureCorrect /* Chrysler returnless fuel pressure correction display section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

}

#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
namespace analogReadViewer /* ADC voltage display section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

}

#endif // useDebugAnalog
#ifdef useTestButtonValues
namespace buttonView /* Button input value viewer section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

}

#endif // useTestButtonValues
#ifdef useDebugTerminal
namespace terminal /* debug terminal section prototype */
{

	static void outputFlags(uint8_t flagRegister, const char * flagStr);
	static void outputTripFunctionValue(uint8_t lineNumber);
	static void outputConstantValue(uint8_t lineNumber);
	static void outputParameterValue(uint8_t lineNumber);
	static void outputParameterExtra(uint8_t lineNumber);
	static void outputVolatileValue(uint8_t lineNumber);
	static void outputMainProgramValue(uint8_t lineNumber);
	static void outputTripVarMeasuredValue(uint8_t lineNumber);
	static void outputTripVarMeasuredExtra(uint8_t lineNumber);

}

#endif // useDebugTerminal
namespace tripSupport /* Trip save/restore/reset support section prototype */
{

#ifdef useSavedTrips
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void goSaveCurrent(void);
	static void goSaveTank(void);
	static void select(void);
	static uint8_t doReadTrip(uint8_t tripSlot);
	static uint8_t doWriteTrip(uint8_t tripSlot);
	static uint8_t doAutoAction(uint8_t taaMode);
#endif // useSavedTrips
	static void displayStatus(uint8_t tripSlot, const char * str);
	static void doResetTrip(uint8_t tripSlot);
	static void resetCurrent(void);
	static void resetTank(void);

};

#ifdef useBarGraph
static const uint8_t bgDataSize = 15;

uint8_t bargraphData[(uint16_t)(bgDataSize)];

namespace barGraphSupport /* Bar Graph support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void displayBarGraphLine(uint8_t lineNumber, uint8_t tripIdx, uint8_t calcIdx);
	static void clearPlot(void);
	static uint8_t rescaleData(uint8_t byt, uint8_t uBound);
	static void graphData(uint8_t cursorPos, uint8_t calcIdx, uint8_t diffFlag);
	static void drawLine(uint8_t color, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
	static void draw(uint8_t color, uint8_t xPos, uint8_t yPos);
//	static int16_t sqrrt(uint16_t num);

};

#endif // useBarGraph
#ifdef useBarFuelEconVsTime
volatile uint8_t FEvTperiodIdx;

uint8_t calcFEvTperiodIdx; // used for calculations, graphing

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
uint8_t FEvSpdTripIdx;

namespace bgFEvsSsupport /* fuel economy over speed histograph support section prototype */
{

	static void reset(void);

};

#endif // useBarFuelEconVsSpeed
#ifdef useDragRaceFunction
namespace accelerationTest /* Acceleration Test support section prototype */
{

	void goDisplay(void);
	void goTrigger(void);
	uint8_t triggerTest(void);

}

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
namespace coastdown /* Coastdown Calculator support section prototype */
{

	void goDisplay(void);
	void goTrigger(void);

}

uint64_t matrix_x[3][3];	// coastdown sample storage
uint64_t matrix_r[3][3];	// inverse matrix
uint64_t matrix_e[3];		// coastdown energy value storage
uint64_t matrix_c[3];		// coastdown coefficient matrix (what we are trying to solve for)

#endif // useCoastDownCalculator
namespace text /* text string output section prototype */
{

	static void gotoXY(interfaceDevice &dev, uint8_t x, uint8_t y);
	static uint8_t charOut(interfaceDevice &dev, uint8_t chr);

	static void setModeOnCondition(interfaceDevice &dev, uint8_t condition, uint8_t odvFlag);
	static void stringOut(interfaceDevice &dev, char * str);
	static void stringOut(interfaceDevice &dev, const char * str);
	static void stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx);
	static void newLine(interfaceDevice &dev);
	static void hexNybbleOut(interfaceDevice &dev, uint8_t val);
	static void hexByteOut(interfaceDevice &dev, uint8_t val);
	static void hexWordOut(interfaceDevice &dev, uint16_t val);
	static void hexDWordOut(interfaceDevice &dev, uint32_t val);
	static void hexLWordOut(interfaceDevice &dev, uint64_t * val);
	static uint8_t numberOut(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag);

};

#ifdef useBuffering
typedef struct
{

	volatile uint8_t * data;
	volatile uint8_t size;
	volatile uint8_t start;
	volatile uint8_t end;
	volatile uint8_t status;

} ringBufferVariable;

namespace ringBuffer // ringBuffer prototype
{

	static void init(ringBufferVariable &bfr, volatile uint8_t * storage);
	static uint8_t isBufferNotEmpty(ringBufferVariable &bfr);
	static void push(ringBufferVariable &bfr, uint8_t value);
	static void pushInterrupt(ringBufferVariable &bfr, uint8_t value);
	static uint8_t pull(ringBufferVariable &bfr);
	static void flush(ringBufferVariable &bfr);

};

#endif // useBuffering
#ifdef useTWIsupport
#include <compat/twi.h>
namespace TWI
{

	static void init(void);
	static void shutdown(void);
	static void openChannelMain(uint8_t address, uint8_t writeFlag);
	static void openChannel(uint8_t address, uint8_t writeFlag);
	static uint8_t writeByte(uint8_t data);
	static void transmitChannel(uint8_t sendStop);

}

#endif // useTWIsupport
#ifdef useAdafruitRGBLCDshield
namespace adafruitRGBLCDsupport
{

	void init(void);
	void writeRegister16Bit(uint8_t registerAddress, union union_16 * registerValue);
	void writeRegister16Bit(uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte);
	void writeRegister8Bit(uint8_t registerAddress, uint8_t portByte);
	void setTransferMode(uint8_t mode);

}

const uint8_t MCP23017_B0_IODIRx =		0x00;
const uint8_t MCP23017_B0_IPOLx =		0x02;
const uint8_t MCP23017_B0_GPINTENx =	0x04;
const uint8_t MCP23017_B0_DEFVALx =		0x06;
const uint8_t MCP23017_B0_INTCONx =		0x08;
const uint8_t MCP23017_B0_IOCON =		0x0A;
const uint8_t MCP23017_B0_GPPUx =		0x0C;
const uint8_t MCP23017_B0_INTFx =		0x0E;
const uint8_t MCP23017_B0_INTCAPx =		0x10;
const uint8_t MCP23017_B0_GPIOx =		0x12;
const uint8_t MCP23017_B0_OLATx =		0x14;

const uint8_t MCP23017_B0_PORTA =		0x00;
const uint8_t MCP23017_B0_PORTB =		0x01;

const uint8_t MCP23017_B1_IODIRA =		0x00;
const uint8_t MCP23017_B1_IPOLA =		0x01;
const uint8_t MCP23017_B1_GPINTENA =	0x02;
const uint8_t MCP23017_B1_DEFVALA =		0x03;
const uint8_t MCP23017_B1_INTCONA =		0x04;
const uint8_t MCP23017_B1_IOCON =		0x05;
const uint8_t MCP23017_B1_GPPUA =		0x06;
const uint8_t MCP23017_B1_INTFA =		0x07;
const uint8_t MCP23017_B1_INTCAPA =		0x08;
const uint8_t MCP23017_B1_GPIOA =		0x09;
const uint8_t MCP23017_B1_OLATA =		0x0A;
const uint8_t MCP23017_B1_IODIRB =		0x10;
const uint8_t MCP23017_B1_IPOLB =		0x11;
const uint8_t MCP23017_B1_GPINTENB =	0x12;
const uint8_t MCP23017_B1_DEFVALB =		0x13;
const uint8_t MCP23017_B1_INTCONB =		0x14;
const uint8_t MCP23017_B1_GPPUB =		0x16;
const uint8_t MCP23017_B1_INTFB =		0x17;
const uint8_t MCP23017_B1_INTCAPB =		0x18;
const uint8_t MCP23017_B1_GPIOB =		0x19;
const uint8_t MCP23017_B1_OLATB =		0x1A;

volatile uint8_t adafruitRGBLCDflags;

const uint8_t afRGBLCDbankMode =		0b10000000;
const uint8_t afRGBLCDmirrorMode =		0b01000000;
const uint8_t afRGBLCDbyteMode =		0b00100000;

const uint8_t adaTWIsequentialMode =	0x00;
const uint8_t adaTWIbyteMode =			0x01;
const uint8_t adaTWItoggleMode =		0x02;

const uint8_t adafruitRGBLCDaddress = 0x20;

#endif // useAdafruitRGBLCDshield
#ifdef useSerial0Port
namespace serial0
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#ifdef useSerial0PortInput
	static uint8_t chrIn(void);
#endif // useSerial0PortInput

};

interfaceDevice devSerial0;

#ifdef useBufferedSerial0Port
ringBufferVariable serial0Buffer;

volatile uint8_t serial0Data[32];

#endif // useBufferedSerial0Port
#ifdef useSerial0PortInput
ringBufferVariable serial0InputBuffer;

volatile uint8_t serial0InputData[16];

#endif // useSerial0PortInput
#endif // useSerial0Port
#ifdef useSerial1Port
namespace serial1
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#ifdef useSerial1PortInput
	static uint8_t chrIn(void);
#endif // useSerial1PortInput

};

interfaceDevice devSerial1;

#ifdef useBufferedSerial1Port
ringBufferVariable serial1Buffer;

volatile uint8_t serial1Data[32];

#endif // useBufferedSerial1Port
#ifdef useSerial1PortInput
ringBufferVariable serial1InputBuffer;

volatile uint8_t serial1InputData[16];

#endif // useSerial1PortInput
#endif // useSerial1Port
#ifdef useUSBserial
namespace usbDevice
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
	static uint8_t chrIn(void);

};

interfaceDevice devUSB;

ringBufferVariable USBoutputBuffer;
ringBufferVariable USBinputBuffer;

volatile uint8_t USBoutputData[256];
volatile uint8_t USBinputData[256];

#endif // useUSBserial
#ifdef useLCDoutput
namespace LCD /* LCD hardware support section prototype */
{

	static void init(void);
	static void shutdown(void);
#ifdef useLCDgraphics
	static void loadCGRAMcharacter(uint8_t chr, char * chrData);
	static void writeCGRAMbyte(uint8_t cgrAddress, uint8_t chr);
	static uint8_t peekCGRAMbyte(uint8_t cgrAddress);
	static void flushCGRAM(void);
#endif // useLCDgraphics
	static void setBrightness(uint8_t idx);
#ifdef useLegacyLCD
	static void setContrast(uint8_t idx);
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	static void setRGBcolor(uint8_t idx);
#endif // useAdafruitRGBLCDshield
	static void writeData(uint8_t value);
#ifdef use4BitLCD
	static void writeCommand(uint8_t value);
	static void writeByte(uint8_t value, uint8_t flags, uint8_t delay);
	static void writeNybble(uint8_t value, uint8_t flags);
	static void outputNybble(uint8_t s);
#if useTWIbuttons && useTWILCD
	static void disableIntSample(void);
	static void enableIntSample(void);
#endif // useTWIbuttons && useTWILCD
#endif // use4BitLCD
};

#ifdef use4BitLCD
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDshield
const uint8_t lcdDirection =		0b01000000; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	0b10000000; // GPIO B
const uint8_t lcdEnable =			0b00100000; // GPIO B
const uint8_t lcdBit3 =				0b00000010; // GPIO B
const uint8_t lcdBit2 =				0b00000100; // GPIO B
const uint8_t lcdBit1 =				0b00001000; // GPIO B
const uint8_t lcdBit0 =				0b00010000; // GPIO B
const uint8_t lcdBrightnessRed =	0b01000000; // GPIO A
const uint8_t lcdBrightnessGreen =	0b10000000; // GPIO A
const uint8_t lcdBrightnessBlue =	0b00000001; // GPIO B

const uint8_t lcdAddress =		0x20;

volatile uint8_t portSwitches; // contains two out of the three LCD backlighting LED pins
#else // useAdafruitRGBLCDshield
const uint8_t lcdDirection =		0b00000010; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	0b00000001;
const uint8_t lcdEnable =			0b00000100;
const uint8_t lcdBit3 =				0b10000000;
const uint8_t lcdBit2 =				0b01000000;
const uint8_t lcdBit1 =				0b00100000;
const uint8_t lcdBit0 =				0b00010000;
const uint8_t lcdBrightness =		0b00001000;

const uint8_t lcdAddress =		0x27;

#endif // useAdafruitRGBLCDshield
volatile uint8_t portLCD; // LCD port register expander byte

#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
const uint8_t lcdDirection =		(1 << PORTF0); // on PORTF, A5, Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	(1 << PORTF1); // on PORTF, A4
const uint8_t lcdEnable =			(1 << PORTE6); // on PORTE, 7
const uint8_t lcdBit3 =				(1 << PORTB4); // on PORTB, 8
const uint8_t lcdBit2 =				(1 << PORTD6); // on PORTD, 12
const uint8_t lcdBit1 =				(1 << PORTD4); // on PORTD, 4
const uint8_t lcdBit0 =				(1 << PORTF4); // on PORTF, A3
const uint8_t lcdBrightness =		(1 << DDB6); // on PORTB, 10
const uint8_t lcdContrast =			(1 << DDB5); // on PORTB, 9

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
const uint8_t lcdRegisterSelect =	(1 << PORTA4); // on PORTA
const uint8_t lcdEnable =			(1 << PORTA5); // on PORTA
const uint8_t lcdBit3 =				(1 << PORTA0); // on PORTA
const uint8_t lcdBit2 =				(1 << PORTA1); // on PORTA
const uint8_t lcdBit1 =				(1 << PORTA2); // on PORTA
const uint8_t lcdBit0 =				(1 << PORTA3); // on PORTA
const uint8_t lcdBrightness =		(1 << DDB5); // on PORTB
const uint8_t lcdContrast =			(1 << DDB7); // on PORTB

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
const uint8_t lcdRegisterSelect =	(1 << PORTD4); // on PORTD, 4
const uint8_t lcdEnable =			(1 << PORTD5); // on PORTD, 5
const uint8_t lcdBit3 =				(1 << PORTB5); // on PORTB, 13
const uint8_t lcdBit2 =				(1 << PORTB4); // on PORTB, 12
const uint8_t lcdBit1 =				(1 << PORTB0); // on PORTB, 8
const uint8_t lcdBit0 =				(1 << PORTD7); // on PORTD, 7
const uint8_t lcdBrightness =		(1 << DDB1); // on PORTB, 9
const uint8_t lcdContrast =			(1 << DDD6); // on PORTD, 6

#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD
const uint8_t lcdNullValue =				0b00000000;

const uint8_t lcdClearDisplay =				0b00000001;

const uint8_t lcdReturnHome =				0b00000010;

const uint8_t lcdEntryModeSet =				0b00000100;
const uint8_t lcdEMSincrement =				0b00000010;		// 1 = increment, 0 = decrement
const uint8_t lcdEMSsetDisplayShift =		0b00000001;		// 1 = display shift, 0 = no display shift

const uint8_t lcdDisplayControl =			0b00001000;
const uint8_t lcdDCdisplayShow =			0b00000100;		// 1 = enable display, 0 = disable display
const uint8_t lcdDCcursorControl =			0b00000010;		// 1 = cursor on, 0 = cursor off
const uint8_t lcdDCcursorBlinkControl =		0b00000001;		// 1 = cursor blink, 0 = cursor steady

const uint8_t lcdShift =					0b00010000;
const uint8_t lcdSdisplayShift =			0b00001000;		// 1 = shift display, 0 = cursor move
const uint8_t lcdSdirection =				0b00000100;		// 1 = move right, 0 = move left

const uint8_t lcdFunctionSet =				0b00100000;
const uint8_t lcdFSdataLength =				0b00010000;		// 1 = 8 bit data, 0 = 4 bit data
const uint8_t lcdFSnumberOfLines =			0b00001000;		// 1 = 2 lines, 0 = 1 line
const uint8_t lcdFScharacterFont =			0b00000100;		// 1 = 5x10 dot character font, 0 = 5x8 dot character font

const uint8_t lcdSetCGRAMaddress =			0b01000000;

const uint8_t lcdSetDDRAMaddress =			0b10000000;

const uint8_t lcdSendNybble =				0b00001000;
const uint8_t lcdDataByte =					0b00000100;
const uint8_t lcdCommandByte =				0b00000000;

const uint8_t lcdDelay0015ms =				0x03;
const uint8_t lcdDelay4100us =				0x02;
const uint8_t lcdDelay0100us =				0x01;
const uint8_t lcdDelay0040us =				0x00;
const uint8_t lcdDelayFlags =				lcdDataByte | 0x03;
const uint8_t lcdSendFlags =				lcdSendNybble | 0x03;

uint8_t LCDgotoXYaddress;
uint8_t LCDaddressX;
uint8_t LCDaddressY;
#endif // use4BitLCD
#ifdef useLegacyLCD
#ifdef useInvertedLegacyLCDbrightness
const uint8_t brightness[] PROGMEM = { 255, 214, 171, 127 }; //middle button cycles through these brightness settings
#else // useInvertedLegacyLCDbrightness
const uint8_t brightness[] PROGMEM = { 0, 41, 84, 128 }; //middle button cycles through these brightness settings
#endif // useInvertedLegacyLCDbrightness

const uint8_t brightnessLength = ( sizeof(brightness) / sizeof(uint8_t) ); // size of brightness table

const char brightString[] PROGMEM = {
	"LOW\x11\r"
	"MED\x10\r"
	"HIGH\x0F\r"
	"OFF\x12\r"
};

#endif // useLegacyLCD
#ifdef useBinaryLCDbrightness
const uint8_t brightnessLength = 2;

const char brightString[] PROGMEM = {
	"ON\x11\r"
	"OFF\x12\r"
};

#endif // useBinaryLCDbrightness

#endif // useLCDoutput
/*

 SWEET64 is a low-level pseudocode interpreter, meant to save a lot of space in program memory for 64-bit calculations

 the opcode definitions below provide means of manipulating MPGuino trip variables, interacting with calculated and EEPROM parameters, and providing basic
 number formatting for interaction with string representations of numbers

 SWEET64 is inspired by an early pseudocode processor named SWEET16, which was written by Steve Wozniak of Apple Computer back in 1977. SWEET16 was written to
    perform 16-bit machine language instructions while running on an 8-bit microprocessor

*/
uint8_t SWEET64processorFlags;

// x < y        - carry flag is clear, zero flag is clear
// x == y       - carry flag is set, zero flag is set
// x > y        - carry flag is set, zero flag is clear
//
// (x - y) >= 0 - minus flag is clear
// (x - y) < 0	- minus flag is set
//
const uint8_t SWEET64carryFlag =			0b00000001;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64zeroFlag =				0b00000010;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64minusFlag =			0b00000100;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64overflowFlag =			0b00001000;			// this is set for arithmetic and branch test operations

const uint8_t SWEET64traceCommandFlag =		0b01000000;			// this flag is ignored if #useSWEET64trace is not used
const uint8_t SWEET64traceFlag =			0b10000000;			// this flag is ignored if #useSWEET64trace is not used

const uint8_t SWEET64traceFlagGroup =	SWEET64traceCommandFlag | SWEET64traceFlag;

#ifdef useDebugTerminal
uint64_t s64reg[6];
#else // useDebugTerminal
uint64_t s64reg[5];
#endif // useDebugTerminal

static const uint8_t s64reg1 = 0;	// general purpose
static const uint8_t s64reg2 = 1;	// output value / general purpose
static const uint8_t s64reg3 = 2;	// general purpose / temporary storage
static const uint8_t s64reg4 = 3;	// used in multiply, divide operations
static const uint8_t s64reg5 = 4;	// used in multiply, divide operations
#ifdef useDebugTerminal
static const uint8_t s64reg6 = 5;	// used for debug terminal
#endif // useDebugTerminal

#define nextAllowedValue 0
static const uint8_t instrTestReg =					nextAllowedValue;						// tests 64-bit register for zero condition or high bit set
static const uint8_t instrTestIndex =				instrTestReg + 1;						// tests primary index for zero condition or high bit set
static const uint8_t instrCmpXtoY =					instrTestIndex + 1;						// compares 64-bit register Y from 64-bit register X
static const uint8_t instrCmpIndex =				instrCmpXtoY + 1;						// compares primary index from operand

static const uint8_t instrBranchIfVclear =			instrCmpIndex + 1;						// branches if result[0..63] is not all 1s
static const uint8_t instrBranchIfVset =			instrBranchIfVclear + 1;				// branches if result[0..63] is all 1s
static const uint8_t instrBranchIfMclear =			instrBranchIfVset + 1;					// branches if result most significant bit is clear
static const uint8_t instrBranchIfMset =			instrBranchIfMclear + 1;				// branches if result most significant bit is set
static const uint8_t instrBranchIfZclear =			instrBranchIfMset + 1;					// branches if result zero flag is clear (X != Y or X != 0)
static const uint8_t instrBranchIfZset =			instrBranchIfZclear + 1;				// branches if result zero flag is set (X == Y or X == 0)
static const uint8_t instrBranchIfCclear =			instrBranchIfZset + 1;					// branches if result carry flag is clear (X <= Y)
static const uint8_t instrBranchIfCset =			instrBranchIfCclear + 1;				// branches if result carry flag is set (X > Y)
static const uint8_t instrBranchIfLT =				instrBranchIfCset + 1;					// branches if X < Y or index < operand
static const uint8_t instrBranchIfGTorE =			instrBranchIfLT + 1;					// branches if X >= Y or index >= operand
static const uint8_t instrBranchIfMetricMode =		instrBranchIfGTorE + 1;					// branches if MPGuino is in metric mode
static const uint8_t instrBranchIfSAEmode =			instrBranchIfMetricMode + 1;			// branches if MPGuino is in SAE mode
static const uint8_t instrBranchIfFuelOverDist =	instrBranchIfSAEmode + 1;				// branches if MPGuino outputs fuel/distance
static const uint8_t instrBranchIfDistOverFuel =	instrBranchIfFuelOverDist + 1;			// branches if MPGuino outputs distance/fuel

static const uint8_t instrSkip =					instrBranchIfDistOverFuel + 1;			// branches always
static const uint8_t instrCall =					instrSkip + 1;							// push current program counter onto stack, then call indexed routine
static const uint8_t instrCallImplied =				instrCall + 1;							// push current program counter onto stack, then call indexed routine stored in jump register
static const uint8_t instrJump =					instrCallImplied + 1;					// jump to indexed routine

/*

	operands specifying 64-bit registers take the form of YX as a hexadecimal value
	
	e.g., instrLdReg, 0x24,

	means "load 64-bit register 4 with the contents of 64-bit register 2"

*/
static const uint8_t instrLdReg =					instrJump + 1;							// load 64-bit register X with contents of 64-bit register Y
static const uint8_t instrLdRegByteFromIndex =		instrLdReg + 1;							// load 64-bit register X with primary index byte value
static const uint8_t instrLdRegByte =				instrLdRegByteFromIndex + 1;			// load 64-bit register X with immediate byte value	
static const uint8_t instrLdRegByteFromY =			instrLdRegByte + 1;						// load 64-bit register X with value of specified byte index into 64-bit register Y
static const uint8_t instrLdRegTripVar =			instrLdRegByteFromY + 1;				// load 64-bit register X with specified trip specified read-in register
static const uint8_t instrLdRegTripVarIndexed =		instrLdRegTripVar + 1;					// load 64-bit register X with indexed trip specified read-in register
static const uint8_t instrLdRegTripVarOffset =		instrLdRegTripVarIndexed + 1;			// load 64-bit register X with offset indexed trip specified read-in register
static const uint8_t instrLdRegTripVarIndexedRV =	instrLdRegTripVarOffset + 1;			// load 64-bit register X with specified trip indexed read-in register
static const uint8_t instrStRegTripVarIndexed =		instrLdRegTripVarIndexedRV + 1;			// store 64-bit register X value to indexed trip specified read-in register
static const uint8_t instrStRegTripVarIndexedRV =	instrStRegTripVarIndexed + 1;			// store 64-bit register X value to specified trip indexed read-in register
static const uint8_t instrLdRegConst =				instrStRegTripVarIndexedRV + 1;			// load 64-bit register X with stored constant value
static const uint8_t instrLdRegConstIndexed =		instrLdRegConst + 1;					// load 64-bit register X with indexed stored constant value
static const uint8_t instrStByteToRegU8 =			instrLdRegConstIndexed + 1;				// store immediate byte value to specified byte index of 64-bit register X
static const uint8_t instrDoBCDadjust =				instrStByteToRegU8 + 1;					// perform BCD-style conversion of 64-bit register X, using format stored in 64-bit register 3
static const uint8_t instrLdRegEEPROM =				instrDoBCDadjust + 1;					// load 64-bit register X with EEPROM parameter value
static const uint8_t instrLdRegEEPROMindexed =		instrLdRegEEPROM + 1;					// load 64-bit register X with indexed EEPROM parameter value
static const uint8_t instrLdRegEEPROMindirect =		instrLdRegEEPROMindexed + 1;			// load 64-bit register X with value of EEPROM parameter indexed in the SAE/metric reference conversion table
static const uint8_t instrLdRegEinit =				instrLdRegEEPROMindirect + 1;			// load 64-bit register X with initial EEPROM parameter value
static const uint8_t instrLdRegEinitIndexed =		instrLdRegEinit + 1;					// load 64-bit register X with indexed initial EEPROM parameter value
static const uint8_t instrStRegEEPROM =				instrLdRegEinitIndexed + 1;				// store 64-bit register X value to EEPROM parameter
static const uint8_t instrStRegEEPROMindexed =		instrStRegEEPROM + 1;					// store 64-bit register X value to indexed EEPROM parameter
static const uint8_t instrStRegEEPROMindirect =		instrStRegEEPROMindexed + 1;			// store 64-bit register X value to EEPROM parameter indexed in the SAE/metric reference conversion table
static const uint8_t instrLdRegMain =				instrStRegEEPROMindirect + 1;			// load 64-bit register X with main program register value
static const uint8_t instrLdRegMainIndexed =		instrLdRegMain + 1;						// load 64-bit register X with indexed main program register value
static const uint8_t instrLdRegMainOffset =			instrLdRegMainIndexed + 1;				// load 64-bit register X with offset indexed main program register value
static const uint8_t instrStRegMain =				instrLdRegMainOffset + 1;				// store 64-bit register X value to main program register
static const uint8_t instrStRegMainIndexed =		instrStRegMain + 1;						// store 64-bit register X value to indexed main program register
static const uint8_t instrLdRegVolatile =			instrStRegMainIndexed + 1;				// load 64-bit register X with volatile register value
static const uint8_t instrLdRegVolatileIndexed =	instrLdRegVolatile + 1;					// load 64-bit register X with indexed volatile register value
static const uint8_t instrStRegVolatile =			instrLdRegVolatileIndexed + 1;			// store 64-bit register X value to volatile register
static const uint8_t instrStRegVolatileIndexed =	instrStRegVolatile + 1;					// store 64-bit register X value to indexed volatile register
static const uint8_t instrLxdI =					instrStRegVolatileIndexed + 1;			// load primary index register with immediate byte value
static const uint8_t instrLxdIEEPROM =				instrLxdI + 1;							// load primary index register with EEPROM parameter value
static const uint8_t instrLxdIEEPROMoffset =		instrLxdIEEPROM + 1;					// load primary index register with offset indexed EEPROM parameter
static const uint8_t instrLxdIParamLength =			instrLxdIEEPROMoffset + 1;				// load primary index register with bit length of EEPROM parameter
static const uint8_t instrLxdIParamLengthIndexed =	instrLxdIParamLength + 1;				// load primary index register with bit length of indexed EEPROM parameter
static const uint8_t instrLdRegNumer =				instrLxdIParamLengthIndexed + 1;		// load 64-bit register X with constant numerator (SAE mode) or denominator (metric mode)
static const uint8_t instrLdRegDenom =				instrLdRegNumer + 1;					// load 64-bit register X with constant denominator (SAE mode) or numerator (metric mode)
static const uint8_t instrSwapReg =					instrLdRegDenom + 1;					// swap contents of 64-bit registers X and Y
static const uint8_t instrSubYfromX =				instrSwapReg + 1;						// subtract 64-bit register Y from 64-bit register X
static const uint8_t instrSubMainFromX =			instrSubYfromX + 1;						// subtract main program register value from 64-bit register X
static const uint8_t instrAddYtoX =					instrSubMainFromX + 1;					// add 64-bit register Y to 64-bit register X
static const uint8_t instrAdjustQuotient =			instrAddYtoX + 1;						// bumps 64-bit register 2 by 1 if last division resulted in remainder * 2 > divisor
static const uint8_t instrAddByteToX =				instrAdjustQuotient + 1;				// add immediate byte value to 64-bit register X
static const uint8_t instrAddConstToX =				instrAddByteToX + 1;					// add stored constant value to 64-bit register X
static const uint8_t instrAddEEPROMtoX =			instrAddConstToX + 1;					// add EEPROM parameter value to 64-bit register X
static const uint8_t instrAddMainToX =				instrAddEEPROMtoX + 1;					// add main program register value to 64-bit register X
static const uint8_t instrAddIndexToX =				instrAddMainToX + 1;					// add contents of primary index to 64-bit register X
static const uint8_t instrMul2by1 =					instrAddIndexToX + 1;					// multiply 64-bit register 2 by contents of 64-bit register 1
static const uint8_t instrMul2byByte =				instrMul2by1 + 1;    					// multiply 64-bit register 2 by immediate byte value
static const uint8_t instrMul2byConst =				instrMul2byByte + 1; 					// multiply 64-bit register 2 by stored constant value
static const uint8_t instrMul2byEEPROM =			instrMul2byConst + 1; 					// multiply 64-bit register 2 by EEPROM parameter value
static const uint8_t instrMul2byMain =				instrMul2byEEPROM + 1;					// multiply 64-bit register 2 by main program register value
static const uint8_t instrMul2byVolatile =			instrMul2byMain + 1;					// multiply 64-bit register 2 by volatile register value
static const uint8_t instrMul2byTripVarIndexed =	instrMul2byVolatile + 1;				// multiply 64-bit register 2 by indexed trip specified read-in register
static const uint8_t instrDiv2by1 =					instrMul2byTripVarIndexed + 1;			// divide 64-bit register 2 by contents of 64-bit register 1
static const uint8_t instrDiv2byConst =				instrDiv2by1 + 1;						// divide 64-bit register 2 by stored constant value
static const uint8_t instrDiv2byEEPROM =			instrDiv2byConst + 1;					// divide 64-bit register 2 by EEPROM parameter value
static const uint8_t instrDiv2byMain =				instrDiv2byEEPROM + 1;					// divide 64-bit register 2 by main program register value
static const uint8_t instrDiv2byTripVarIndexed =	instrDiv2byMain + 1;					// divide 64-bit register 2 by indexed trip specified read-in register
static const uint8_t instrDiv2byByte =				instrDiv2byTripVarIndexed + 1;			// divide 64-bit register 2 by immediate byte value
static const uint8_t instrShiftRegLeft =			instrDiv2byByte + 1;					// shift 64-bit register X one bit left
static const uint8_t instrShiftRegRight =			instrShiftRegLeft + 1;					// shift 64-bit register X one bit right
static const uint8_t instrAddIndex =				instrShiftRegRight + 1;					// add immediate byte value to primary index register
static const uint8_t instrTraceOn =					instrAddIndex + 1;						// turn on SWEET64 trace
static const uint8_t instrTraceOff =				instrTraceOn + 1;						// turn off SWEET64 trace
static const uint8_t instrTraceSave =				instrTraceOff + 1;						// save status of SWEET64 trace, then turn off
static const uint8_t instrTraceRestore =			instrTraceSave + 1;						// restore status of SWEET64 trace
static const uint8_t instrTraceDone =				instrTraceRestore + 1;					// restore status of SWEET64 trace, then return to caller
static const uint8_t instrLdJumpReg =				instrTraceDone + 1;						// load jump register with routine index value
static const uint8_t instrClearFlag =				instrLdJumpReg + 1;						// clear SWEET64 status flag
static const uint8_t instrSetFlag =					instrClearFlag + 1;						// set SWEET64 status flag
#define nextAllowedValue instrSetFlag + 1

#ifdef useIsqrt
static const uint8_t instrIsqrt =					nextAllowedValue;						// perform integer square root on 64-bit register
#define nextAllowedValue instrIsqrt + 1
#endif // useIsqrt

#if defined(useAnalogRead)
static const uint8_t instrLdRegVoltage =			nextAllowedValue;						// load 64-bit register with specified raw 10-bit analog voltage value
static const uint8_t instrLdRegVoltageIndexed =		instrLdRegVoltage + 1;					// load 64-bit register with indexed raw 10-bit analog voltage value
#define nextAllowedValue instrLdRegVoltageIndexed + 1
#endif // useAnalogRead

#ifdef useBarFuelEconVsTime
static const uint8_t instrLdRegTripFEvTindexed =	nextAllowedValue;						// load trip index from fuel econ vs time trip array
#define nextAllowedValue instrLdRegTripFEvTindexed + 1
#endif // useBarFuelEconVsTime

#ifdef useBarGraph
static const uint8_t instrStRegBGdataIndexed =	nextAllowedValue;							// store register byte to bargraph data array
#define nextAllowedValue instrStRegBGdataIndexed + 1
#endif // useBarFuelEconVsTime

static const uint8_t instrDone =					nextAllowedValue;						// return to caller - if at topmost level of stack, return to main program
#define nextAllowedValue instrDone + 1

static const uint8_t maxValidSWEET64instr =			nextAllowedValue;

#ifdef useSWEET64trace
static const char opcodeList[] PROGMEM = {
	"instrTestReg\r"
	"instrTestIndex\r"
	"instrCmpXtoY\r"
	"instrCmpIndex\r"
	"instrBranchIfMclear\r"
	"instrBranchIfMset\r"
	"instrBranchIfZclear\r"
	"instrBranchIfZset\r"
	"instrBranchIfCclear\r"
	"instrBranchIfCset\r"
	"instrBranchIfLT\r"
	"instrBranchIfGTorE\r"
	"instrBranchIfMetricMode\r"
	"instrBranchIfSAEmode\r"
	"instrBranchIfFuelOverDist\r"
	"instrBranchIfDistOverFuel\r"
	"instrSkip\r"
	"instrLdReg\r"
	"instrLdRegByteFromIndex\r"
	"instrLdRegByte\r"
	"instrLdRegByteFromY\r"
	"instrLdRegTripVar\r"
	"instrLdRegTripVarIndexed\r"
	"instrLdRegTripVarOffset\r"
	"instrLdRegTripVarIndexedRV\r"
	"instrStRegTripVarIndexed\r"
	"instrStRegTripVarIndexedRV\r"
	"instrLdRegConst\r"
	"instrStByteToRegU8\r"
	"instrDoBCDadjust\r"
	"instrLdRegEEPROM\r"
	"instrLdRegEEPROMindexed\r"
	"instrLdRegEEPROMindirect\r"
	"instrLdRegEinit\r"
	"instrLdRegEinitIndexed\r"
	"instrStRegEEPROM\r"
	"instrStRegEEPROMindexed\r"
	"instrStRegEEPROMindirect\r"
	"instrLdRegMain\r"
	"instrLdRegMainIndexed\r"
	"instrLdRegMainOffset\r"
	"instrStRegMain\r"
	"instrStRegMainIndexed\r"
	"instrLdRegVolatile\r"
	"instrLdRegVolatileIndexed\r"
	"instrStRegVolatile\r"
	"instrStRegVolatileIndexed\r"
	"instrLxdI\r"
	"instrLxdIEEPROM\r"
	"instrLxdIEEPROMindexed\r"
	"instrLxdIParamLength\r"
	"instrLxdIParamLengthIndexed\r"
	"instrLdRegNumer\r"
	"instrLdRegDenom\r"
	"instrCall\r"
	"instrJump\r"
	"instrSwapReg\r"
	"instrSubYfromX\r"
	"instrSubMainFromX\r"
	"instrAddYtoX\r"
	"instrAdjustQuotient\r"
	"instrAddByteToX\r"
	"instrAddConstToX\r"
	"instrAddEEPROMtoX\r"
	"instrAddMainToX\r"
	"instrAddIndexToX\r"
	"instrMul2by1\r"
	"instrMul2byByte\r"
	"instrMul2byConst\r"
	"instrMul2byEEPROM\r"
	"instrMul2byMain\r"
	"instrMul2byVolatile\r"
	"instrMul2byTripVarIndexed\r"
	"instrDiv2by1\r"
	"instrDiv2byConst\r"
	"instrDiv2byEEPROM\r"
	"instrDiv2byMain\r"
	"instrDiv2byTripVarIndexed\r"
	"instrDiv2byByte\r"
	"instrShiftRegLeft\r"
	"instrShiftRegRight\r"
	"instrAddIndex\r"
	"instrTraceOn\r"
	"instrTraceOff\r"
	"instrTraceSave\r"
	"instrTraceRestore\r"
	"instrTraceDone\r"
	"instrLdJumpReg\r"
	"instrClearFlag\r"
	"instrSetFlag\r"
#ifdef useIsqrt
	"instrIsqrt\r"
#endif // useIsqrt
#if defined(useAnalogRead)
	"instrLdRegVoltage\r"
	"instrLdRegVoltageIndexed\r"
#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
	"instrLdRegTripFEvTindexed\r"
#endif // useBarFuelEconVsTime
#ifdef useBarGraph
	"instrStRegBGdataIndexed\r"
#endif // useBarGraph
	"instrDone\r"
};

#endif // useSWEET64trace
// these #defines can optionally be used to generate SWEET64 bytecodes
//
#define instrBranchIfOverflow		instrBranchIfVset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfPlus			instrBranchIfMclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfMinus			instrBranchIfMset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfNotE			instrBranchIfZclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfE				instrBranchIfZset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfNotZero		instrBranchIfZclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfZero			instrBranchIfZset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfLTorE			instrBranchIfCclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfGT				instrBranchIfCset					// provided as a convenience for aspiring SWEET64 coders

// calculation indexes into SWEET64 S64programList[] for display functions to either screen or logging output
//
// functions are grouped into three categories, in order
//
//    functions that return results that do not require conversion between SI and SAE formats
//    functions that return results in either SI or SAE format, depending on MPGuino settings
//    functions that return results in either SI or SAE format, and have an alternate SI/SAE format, depending on MPGuino settings
//
#define nextAllowedValue 0
static const uint8_t tEngineRunTime =				nextAllowedValue;				// engine runtime (hhmmss)
static const uint8_t tRangeTime =					tEngineRunTime + 1;				// estimated total runtime from full tank (hhmmss)
static const uint8_t tReserveTime =					tRangeTime + 1;					// estimated reserve runtime from full tank (hhmmss)
static const uint8_t tBingoTime =					tReserveTime + 1;				// estimated bingo fuel runtime from full tank (hhmmss)
static const uint8_t tTimeToEmpty =					tBingoTime + 1;					// estimated remaining total tank engine runtime (hhmmss)
static const uint8_t tReserveTimeToEmpty =			tTimeToEmpty + 1;				// estimated remaining reserve engine runtime (hhmmss)
static const uint8_t tBingoTimeToEmpty =			tReserveTimeToEmpty + 1;		// estimated bingo quantity engine runtime (hhmmss)
static const uint8_t tMotionTime =					tBingoTimeToEmpty + 1;			// time vehicle in motion (hhmmss)
#define nextAllowedValue tMotionTime + 1

static const uint8_t dfMaxValHHMMSS =				nextAllowedValue;				// maximum index for function indexes that return results that are not affected by metric conversions

static const uint8_t tInjectorOpenTime =			nextAllowedValue;				// fuel used (microseconds)
static const uint8_t tInjectorTotalTime =			tInjectorOpenTime + 1;			// engine run time (microseconds)
static const uint8_t tVSStotalTime =				tInjectorTotalTime + 1;			// time vehicle in motion (microseconds)
static const uint8_t tEngineSpeed =					tVSStotalTime + 1;				// engine speed (1/m)
static const uint8_t tInjectorPulseCount =			tEngineSpeed + 1;				// fuel injector pulse count
static const uint8_t tVSSpulseCount =				tInjectorPulseCount + 1;		// VSS pulse count
#define nextAllowedValue tVSSpulseCount + 1

#ifdef useFuelCost
static const uint8_t tFuelCostUsed =				nextAllowedValue;				// cost of fuel quantity used
static const uint8_t tFuelCostTank =				tFuelCostUsed + 1;				// full tank fuel cost in currency units
static const uint8_t tFuelCostReserve =				tFuelCostTank + 1;				// reserve fuel quantity fuel cost in currency units
static const uint8_t tFuelCostBingo =				tFuelCostReserve + 1;			// bingo fuel quantity cost in currency units
static const uint8_t tFuelCostRemaining =			tFuelCostBingo + 1;				// value of estimated remaining fuel quantity in currency units
static const uint8_t tFuelCostReserveRemaining =	tFuelCostRemaining + 1;			// value of estimated remaining reserve fuel quantity in currency units
static const uint8_t tFuelCostBingoRemaining =		tFuelCostReserveRemaining + 1;	// value of estimated remaining bingo fuel quantity in currency units
static const uint8_t tFuelRateCost =				tFuelCostBingoRemaining + 1;	// fuel rate cost in currency units
#define nextAllowedValue tFuelRateCost + 1
#endif // useFuelCost

#ifdef useDebugAnalog
static const uint8_t tAnalogChannel =				nextAllowedValue;				// DC voltage
#define nextAllowedValue tAnalogChannel + 1
#endif // useDebugAnalog

#ifdef useCarVoltageOutput
static const uint8_t tAlternatorChannel =			nextAllowedValue;				// DC voltage
#define nextAllowedValue tAlternatorChannel + 1
#endif // useCarVoltageOutput

#ifdef useDragRaceFunction
static const uint8_t tAccelTestTime =				nextAllowedValue;				// acceleration test time (s.s)
#define nextAllowedValue tAccelTestTime + 1
#endif // useDragRaceFunction

static const uint8_t dfMaxValNonConversion =		nextAllowedValue;				// maximum index for function indexes that return results that are not affected by metric conversions

static const uint8_t tFuelUsed =					nextAllowedValue;				// fuel quantity used (SI/SAE)
static const uint8_t tFuelRate =					tFuelUsed + 1;					// fuel consumption rate (SI/SAE)
static const uint8_t tDistance =					tFuelRate + 1;					// vehicle distance traveled (SI/SAE)
static const uint8_t tSpeed =						tDistance + 1;					// vehicle speed (SI/SAE)
static const uint8_t tRemainingFuel =				tSpeed + 1;						// estimated total remaining fuel quantity (SI/SAE)
static const uint8_t tReserveRemainingFuel =		tRemainingFuel + 1;				// estimated reserve remaining fuel quantity (SI/SAE)
static const uint8_t tBingoRemainingFuel =			tReserveRemainingFuel + 1;		// estimated bingo remaining fuel quantity (SI/SAE)
static const uint8_t tRangeDistance =				tBingoRemainingFuel + 1;		// estimated total fuel tank distance (SI/SAE)
static const uint8_t tReserveDistance =				tRangeDistance + 1;				// estimated reserve fuel tank distance (SI/SAE)
static const uint8_t tBingoDistance =				tReserveDistance + 1;			// estimated bingo fuel tank distance (SI/SAE)
static const uint8_t tDistanceToEmpty =				tBingoDistance + 1;				// estimated remaining distance (SI/SAE)
static const uint8_t tReserveDistanceToEmpty =		tDistanceToEmpty + 1;			// estimated reserve remaining distance (SI/SAE)
static const uint8_t tBingoDistanceToEmpty =		tReserveDistanceToEmpty + 1;	// estimated bingo remaining distance (SI/SAE)
#define nextAllowedValue tBingoDistanceToEmpty + 1
#ifdef useFuelCost
static const uint8_t tFuelCostPerDistance =			nextAllowedValue;				// fuel cost per unit distance (SI/SAE)
static const uint8_t tDistancePerFuelCost =			tFuelCostPerDistance + 1;		// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue tDistancePerFuelCost + 1
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
static const uint8_t tPressureChannel =				nextAllowedValue;				// absolute pressure (SI/SAE)
#define nextAllowedValue tPressureChannel + 1
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
static const uint8_t tEstimatedEnginePower =		nextAllowedValue;				// estimated engine power (SI/SAE)
static const uint8_t tDragSpeed =					tEstimatedEnginePower + 1;		// acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#define nextAllowedValue tDragSpeed + 1
#endif // useDragRaceFunction

static const uint8_t dfMaxValSingleFormat =			nextAllowedValue;				// maximum index for function indexes that return results that only have single metric formats

static const uint8_t tFuelEcon =					nextAllowedValue;				// fuel economy (SI/SAE)
#define nextAllowedValue tFuelEcon + 1

static const uint8_t dfMaxValDisplayCount =			nextAllowedValue;				// maximum index for function indexes that return results for display

static const uint8_t tCalculateRemainingTank =		nextAllowedValue;				// calculate estimated remaining fuel quantity in injector open cycles
static const uint8_t tCalculateRemainingReserve =	tCalculateRemainingTank + 1;	// calculate estimated remaining fuel reserve value in injector open cycles
static const uint8_t tCalculateBingoFuel =			tCalculateRemainingReserve + 1;	// calculate estimated fuel bingo value in injector open cycles
static const uint8_t tConvertToMicroSeconds =		tCalculateBingoFuel + 1;
static const uint8_t tCalculateFuelQuantity =		tConvertToMicroSeconds + 1;
static const uint8_t tCalculateFuelDistance =		tCalculateFuelQuantity + 1;
static const uint8_t tCalculateFuelTime =			tCalculateFuelDistance + 1;
static const uint8_t tFormatToTime =				tCalculateFuelTime + 1;
static const uint8_t tFormatToNumber =				tFormatToTime + 1;
static const uint8_t tRoundOffNumber =				tFormatToNumber + 1;
static const uint8_t tLoadTrip =					tRoundOffNumber + 1;
static const uint8_t tSaveTrip =					tLoadTrip + 1;
static const uint8_t tReadTicksToSeconds =			tSaveTrip + 1;
#define nextAllowedValue tReadTicksToSeconds + 1
#ifdef useBarFuelEconVsTime
static const uint8_t tFEvTgetDistance =				nextAllowedValue;
static const uint8_t tFEvTgetConsumedFuel =			tFEvTgetDistance + 1;
static const uint8_t tFEvTgetFuelEconomy =			tFEvTgetConsumedFuel + 1;
#define nextAllowedValue tFEvTgetFuelEconomy + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t tFEvSgetDistance =				nextAllowedValue;
static const uint8_t tFEvSgetConsumedFuel =			tFEvSgetDistance + 1;
static const uint8_t tFEvSgetFuelEconomy =			tFEvSgetConsumedFuel + 1;
#define nextAllowedValue tFEvSgetFuelEconomy + 1
#endif // useBarFuelEconVsSpeed
#ifdef useDebugTerminal
static const uint8_t tParseCharacterToReg =			nextAllowedValue;
#define nextAllowedValue tParseCharacterToReg + 1
#endif //useDebugTerminal

#if (useDebugTerminalLabels) || (useSWEET64trace)
static const char terminalTripFuncNames[] PROGMEM = {
	"tEngineRunTime\r"					// engine runtime (hhmmss)
	"tRangeTime\r"						// estimated total runtime from full tank (hhmmss)
	"tReserveTime\r"					// estimated reserve runtime from full tank (hhmmss)
	"tBingoTime\r"						// estimated bingo fuel runtime from full tank (hhmmss)
	"tTimeToEmpty\r"					// estimated remaining total tank engine runtime (hhmmss)
	"tReserveTimeToEmpty\r"				// estimated remaining reserve engine runtime (hhmmss)
	"tBingoTimeToEmpty\r"				// estimated bingo quantity engine runtime (hhmmss)
	"tMotionTime\r"						// time vehicle in motion (hhmmss)
	"tInjectorOpenTime\r"				// fuel used (microseconds)
	"tInjectorTotalTime\r"				// engine run time (microseconds)
	"tVSStotalTime\r"					// time vehicle in motion (microseconds)
	"tEngineSpeed\r"					// engine speed (1/m)
	"tInjectorPulseCount\r"				// fuel injector pulse count
	"tVSSpulseCount\r"					// VSS pulse count
#ifdef useFuelCost
	"tFuelCostUsed\r"					// cost of fuel quantity used
	"tFuelCostTank\r"					// full tank fuel cost in currency units
	"tFuelCostReserve\r"				// reserve fuel quantity fuel cost in currency units
	"tFuelCostBingo\r"					// bingo fuel quantity cost in currency units
	"tFuelCostRemaining\r"				// value of estimated remaining fuel quantity in currency units
	"tFuelCostReserveRemaining\r"		// value of estimated remaining reserve fuel quantity in currency units
	"tFuelCostBingoRemaining\r"			// value of estimated remaining bingo fuel quantity in currency units
	"tFuelRateCost\r"					// fuel rate cost in currency units
#endif // useFuelCost
#ifdef useDebugAnalog
	"tAnalogChannel\r"					// DC voltage
#endif // useDebugAnalog
#ifdef useCarVoltageOutput
	"tAlternatorChannel\r"				// DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	"tAccelTestTime\r"					// acceleration test time (s.s)
#endif // useDragRaceFunction
	"tFuelUsed\r"						// fuel quantity used (SI/SAE)
	"tFuelRate\r"						// fuel consumption rate (SI/SAE)
	"tDistance\r"						// vehicle distance traveled (SI/SAE)
	"tSpeed\r"							// vehicle speed (SI/SAE)
	"tRemainingFuel\r"					// estimated total remaining fuel quantity (SI/SAE)
	"tReserveRemainingFuel\r"			// estimated reserve remaining fuel quantity (SI/SAE)
	"tBingoRemainingFuel\r"				// estimated bingo remaining fuel quantity (SI/SAE)
	"tRangeDistance\r"					// estimated total fuel tank distance (SI/SAE)
	"tReserveDistance\r"				// estimated reserve fuel tank distance (SI/SAE)
	"tBingoDistance\r"					// estimated bingo fuel tank distance (SI/SAE)
	"tDistanceToEmpty\r"				// estimated remaining distance (SI/SAE)
	"tReserveDistanceToEmpty\r"			// estimated reserve remaining distance (SI/SAE)
	"tBingoDistanceToEmpty\r"			// estimated bingo remaining distance (SI/SAE)
#ifdef useFuelCost
	"tFuelCostPerDistance\r"			// fuel cost per unit distance (SI/SAE)
	"tDistancePerFuelCost\r"			// distance per unit fuel cost (SI/SAE)
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	"tPressureChannel\r"				// absolute pressure (SI/SAE)
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	"tEstimatedEnginePower\r"			// estimated engine power (SI/SAE)
	"tDragSpeed\r"						// acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#endif // useDragRaceFunction
	"tFuelEcon\r"						// fuel economy (SI/SAE)
};

#endif // (useDebugTerminalLabels) || (useSWEET64trace)
const uint8_t rvVSSpulseIdx = 0; 			// distance pulse count
const uint8_t rvVSScycleIdx = 1; 			// time that the vehicle has spent moving
const uint8_t rvInjPulseIdx = 2; 			// engine revolution count
const uint8_t rvInjCycleIdx = 3; 			// time that the fuel injector has been open
const uint8_t rvEngCycleIdx = 4; 			// engine run time

const uint8_t rvMeasuredCount = 5;

#ifdef useDebugTerminalLabels
static const char terminalTripVarLabels[] PROGMEM = {
	"rvVSSpulseIdx\r"
	"rvVSScycleIdx\r"
	"rvInjPulseIdx\r"
	"rvInjCycleIdx\r"
	"rvEngCycleIdx\r"
};

#endif // useDebugTerminalLabels
// trip index definitions for all of the trip variables used within MPGuino

volatile uint8_t rawTripIdx;
#ifdef trackIdleEOCdata
volatile uint8_t rawEOCidleTripIdx;
#endif // trackIdleEOCdata

#ifdef useWindowTripFilter
const uint8_t windowTripFilterSize = 4;
uint8_t wtpCurrentIdx;

#endif // useWindowTripFilter
#define nextAllowedValue 0
const uint8_t raw0tripIdx =				nextAllowedValue;
const uint8_t raw1tripIdx =				raw0tripIdx + 1;
const uint8_t instantIdx =				raw1tripIdx + 1;
const uint8_t currentIdx =				instantIdx + 1;
const uint8_t tankIdx =					currentIdx + 1;
#define nextAllowedValue tankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t raw0eocIdleTripIdx =		nextAllowedValue;
const uint8_t raw1eocIdleTripIdx =		raw0eocIdleTripIdx + 1;
const uint8_t eocIdleInstantIdx =		raw1eocIdleTripIdx + 1;
const uint8_t eocIdleCurrentIdx =		eocIdleInstantIdx + 1;
const uint8_t eocIdleTankIdx =			eocIdleCurrentIdx + 1;
#define nextAllowedValue eocIdleTankIdx + 1
#endif // trackIdleEOCdata

#ifdef useDebugTerminal
const uint8_t terminalIdx =				nextAllowedValue;
#define nextAllowedValue terminalIdx + 1
#endif // useDebugTerminal

#ifdef useDragRaceFunction
const uint8_t dragRawHalfSpeedIdx =		nextAllowedValue;
const uint8_t dragRawFullSpeedIdx =		dragRawHalfSpeedIdx + 1;
const uint8_t dragRawDistanceIdx =		dragRawFullSpeedIdx + 1;
const uint8_t dragHalfSpeedIdx =		dragRawDistanceIdx + 1;
const uint8_t dragFullSpeedIdx =		dragHalfSpeedIdx + 1;
const uint8_t dragDistanceIdx =			dragFullSpeedIdx + 1;
#define nextAllowedValue dragDistanceIdx + 1
#endif // useDragRaceFunction

#ifdef useWindowTripFilter
const uint8_t windowTripFilterIdx =		nextAllowedValue;
#define nextAllowedValue windowTripFilterIdx + windowTripFilterSize
#endif // useWindowTripFilter

const uint8_t tripSlotFullCount =		nextAllowedValue;

#ifdef useBarFuelEconVsTime
const uint8_t FEvsTimeIdx =				nextAllowedValue;
const uint8_t FEvsTimeEndIdx =			FEvsTimeIdx + bgDataSize - 1;
#define nextAllowedValue FEvsTimeEndIdx + 1
#endif // useBarFuelEconVsTime

#ifdef useBarFuelEconVsSpeed
const uint8_t FEvsSpeedIdx =			nextAllowedValue;
#define nextAllowedValue FEvsSpeedIdx + bgDataSize
#endif // useBarFuelEconVsSpeed

const uint8_t tripSlotCount =			nextAllowedValue;

#ifdef useEEPROMtripStorage
const uint8_t EEPROMcurrentIdx =		nextAllowedValue;
const uint8_t EEPROMtankIdx =			EEPROMcurrentIdx + 1;
#define nextAllowedValue EEPROMtankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t EEPROMeocIdleCurrentIdx =	nextAllowedValue;
const uint8_t EEPROMeocIdleTankIdx =	EEPROMeocIdleCurrentIdx + 1;
#define nextAllowedValue EEPROMeocIdleTankIdx + 1
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage

const uint8_t tripSlotTotalCount =		nextAllowedValue;

static const char tripVarChars[(uint16_t)(tripSlotTotalCount)] PROGMEM = {
	 '0'
	,'1'
	,'I'
	,'C'
	,'T'
#ifdef trackIdleEOCdata
	,'\\'
	,'/'
	,'i'
	,'c'
	,'t'
#endif // trackIdleEOCdata
#ifdef useDebugTerminal
	,']'
#endif // useDebugTerminal
#ifdef useDragRaceFunction
	,'h'
	,'f'
	,'d'
	,'H'
	,'F'
	,'D'
#endif // useDragRaceFunction
#ifdef useWindowTripFilter
	,'W'						// ensure there are as many of these as is specified in windowTripFilterSize
	,'W'
	,'W'
	,'W'
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,'P'						// ensure there are as many of these as is specified in bgDataSize
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,'0'						// ensure there are as many of these as is specified in bgDataSize
	,'1'
	,'2'
	,'3'
	,'4'
	,'5'
	,'6'
	,'7'
	,'8'
	,'9'
	,'A'
	,'B'
	,'C'
	,'D'
	,'E'
#endif // useBarFuelEconVsSpeed
#ifdef useEEPROMtripStorage
	,'<'
	,'('
#ifdef trackIdleEOCdata
	,'['
	,'{'
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
};

volatile uint32_t collectedVSSpulseCount[(uint16_t)(tripSlotCount)];
volatile uint64_t collectedInjCycleCount[(uint16_t)(tripSlotCount)];

volatile uint32_t collectedInjPulseCount[(uint16_t)(tripSlotFullCount)];
volatile uint64_t collectedVSScycleCount[(uint16_t)(tripSlotFullCount)];
volatile uint64_t collectedEngCycleCount[(uint16_t)(tripSlotFullCount)];

#ifdef useDebugTerminal
static const char terminalTripVarNames[] PROGMEM = {
	"raw0tripIdx\r"
	"raw1tripIdx\r"
	"instantIdx\r"
	"currentIdx\r"
	"tankIdx\r"
#ifdef trackIdleEOCdata
	"raw0eocIdleTripIdx\r"
	"raw1eocIdleTripIdx\r"
	"eocIdleInstantIdx\r"
	"eocIdleCurrentIdx\r"
	"eocIdleTankIdx\r"
#endif // trackIdleEOCdata
	"terminalIdx\r"
#ifdef useDragRaceFunction
	"dragRawHalfSpeedIdx\r"
	"dragRawFullSpeedIdx\r"
	"dragRawDistanceIdx\r"
	"dragHalfSpeedIdx\r"
	"dragFullSpeedIdx\r"
	"dragDistanceIdx\r"
#endif // useDragRaceFunction
#ifdef useWindowTripFilter
	"windowTripFilterIdx[00]\r"
	"windowTripFilterIdx[01]\r"
	"windowTripFilterIdx[02]\r"
	"windowTripFilterIdx[03]\r"
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	"FEvsTimeIdx[00]\r"
	"FEvsTimeIdx[01]\r"
	"FEvsTimeIdx[02]\r"
	"FEvsTimeIdx[03]\r"
	"FEvsTimeIdx[04]\r"
	"FEvsTimeIdx[05]\r"
	"FEvsTimeIdx[06]\r"
	"FEvsTimeIdx[07]\r"
	"FEvsTimeIdx[08]\r"
	"FEvsTimeIdx[09]\r"
	"FEvsTimeIdx[10]\r"
	"FEvsTimeIdx[11]\r"
	"FEvsTimeIdx[12]\r"
	"FEvsTimeIdx[13]\r"
	"FEvsTimeIdx[14]\r"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"FEvsSpeedIdx[00]\r"
	"FEvsSpeedIdx[01]\r"
	"FEvsSpeedIdx[02]\r"
	"FEvsSpeedIdx[03]\r"
	"FEvsSpeedIdx[04]\r"
	"FEvsSpeedIdx[05]\r"
	"FEvsSpeedIdx[06]\r"
	"FEvsSpeedIdx[07]\r"
	"FEvsSpeedIdx[08]\r"
	"FEvsSpeedIdx[09]\r"
	"FEvsSpeedIdx[10]\r"
	"FEvsSpeedIdx[11]\r"
	"FEvsSpeedIdx[12]\r"
	"FEvsSpeedIdx[13]\r"
	"FEvsSpeedIdx[14]\r"
#endif // useBarFuelEconVsSpeed
#ifdef useEEPROMtripStorage
	"EEPROMcurrentIdx\r"
	"EEPROMtankIdx\r"
#ifdef trackIdleEOCdata
	"EEPROMeocIdleCurrentIdx\r"
	"EEPROMeocIdleTankIdx\r"
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
};

#endif // useDebugTerminal
// volatile variable array index values - these may be referenced inside an interrupt service routine
#define nextAllowedValue 0
#ifdef useCPUreading
static const uint8_t vSystemCycleIdx =				nextAllowedValue;				// system timer tick count
#define nextAllowedValue vSystemCycleIdx + 1
#endif // useCPUreading

#ifdef useSoftwareClock
static const uint8_t vClockCycleIdx =				nextAllowedValue;				// software clock tick count
#define nextAllowedValue vClockCycleIdx + 1
#endif // useSoftwareClock

static const uint8_t vVehicleStopTimeoutIdx =		nextAllowedValue;				// engine idle timeout value in timer0 ticks
static const uint8_t vEngineOffTimeoutIdx =			vVehicleStopTimeoutIdx + 1;		// engine off coasting timeout value in timer0 ticks
static const uint8_t vButtonTimeoutIdx =			vEngineOffTimeoutIdx + 1;		// button press timeout value in timer0 ticks
static const uint8_t vParkTimeoutIdx =				vButtonTimeoutIdx + 1;			// vehicle park (engine off and vehicle stopped) timeout value in timer0 ticks
static const uint8_t vActivityTimeoutIdx =			vParkTimeoutIdx + 1;			// total activity (vehicle park and no button press) timeout value in timer0 ticks
static const uint8_t vDetectVehicleStopIdx =		vActivityTimeoutIdx + 1;		// minimum good vehicle speed in timer0 ticks
static const uint8_t vDetectEngineOffIdx =			vDetectVehicleStopIdx + 1;		// minimum good engine speed in timer0 ticks
static const uint8_t vMaximumVSSperiodIdx =			vDetectEngineOffIdx + 1;		// maximum good VSS period in timer0 cycles
static const uint8_t vMaximumEnginePeriodIdx =		vMaximumVSSperiodIdx + 1;		// maximum good engine period in timer0 cycles
static const uint8_t vInjectorOpenDelayIdx =		vMaximumEnginePeriodIdx + 1;	// injector settle time in timer0 cycles
static const uint8_t vInjectorCloseDelayIdx =		vInjectorOpenDelayIdx + 1;		// injector closing settle time in timer0 cycles
static const uint8_t vInjectorTotalDelayIdx =		vInjectorCloseDelayIdx + 1;		// injector total settle time in timer0 cycles, for injector validity check
static const uint8_t vInjectorValidMaxWidthIdx =	vInjectorTotalDelayIdx + 1;		// maximum valid fuel injector pulse width in timer0 cycles
#define nextAllowedValue vInjectorValidMaxWidthIdx + 1
#ifdef useBarFuelEconVsTime
static const uint8_t vFEvsTimePeriodTimeoutIdx =	nextAllowedValue;				// time period for fuel economy vs time bargraph
#define nextAllowedValue vFEvsTimePeriodTimeoutIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useDebugCPUreading
static const uint8_t vInterruptAccumulatorIdx =		nextAllowedValue;				// interrupt handler stopwatch direct measurement
#define nextAllowedValue vInterruptAccumulatorIdx + 1
#endif // useDebugCPUreading

#ifdef useDragRaceFunction
static const uint8_t vDragInstantSpeedIdx =			nextAllowedValue;
static const uint8_t vAccelHalfPeriodValueIdx =		vDragInstantSpeedIdx + 1;
static const uint8_t vAccelFullPeriodValueIdx =		vAccelHalfPeriodValueIdx + 1;
static const uint8_t vAccelDistanceValueIdx =		vAccelFullPeriodValueIdx + 1;
#define nextAllowedValue vAccelDistanceValueIdx + 1
#endif // useDragRaceFunction

#ifdef useCoastDownCalculator
static const uint8_t vCoastdownPeriodIdx =			nextAllowedValue;
static const uint8_t vCoastdownMeasurement1Idx =	vCoastdownPeriodIdx + 1;
static const uint8_t vCoastdownMeasurement2Idx =	vCoastdownMeasurement1Idx + 1;
static const uint8_t vCoastdownMeasurement3Idx =	vCoastdownMeasurement2Idx + 1;
static const uint8_t vCoastdownMeasurement4Idx =	vCoastdownMeasurement3Idx + 1;
#define nextAllowedValue vCoastdownMeasurement4Idx + 1
#endif // useCoastDownCalculator

#ifdef useDebugTerminal
static const uint8_t vDebugValue1Idx =				nextAllowedValue;
static const uint8_t vDebugValue2Idx =				vDebugValue1Idx + 1;
static const uint8_t vDebugValue3Idx =				vDebugValue2Idx + 1;
static const uint8_t vDebugValue4Idx =				vDebugValue3Idx + 1;
#define nextAllowedValue vDebugValue4Idx + 1
#endif // useDebugTerminal

static const uint8_t vVariableMaxIdx =				nextAllowedValue;

// main program variable array index values - these should NEVER be referenced inside an interrupt service routine
#define nextAllowedValue 0
static const uint8_t mpCyclesPerVolumeIdx =			nextAllowedValue;					// injector conversion factor of unit volume per timer0 cycle
static const uint8_t mpTankSizeIdx =				mpCyclesPerVolumeIdx + 1;			// tank quantity in timer0 cycles
static const uint8_t mpBingoTankSizeIdx =			mpTankSizeIdx + 1;					// bingo fuel quantity in timer0 cycles
#define nextAllowedValue mpBingoTankSizeIdx + 1

#ifdef usePartialRefuel
static const uint8_t mpPartialRefuelTankSize =		nextAllowedValue;					// partial refuel tank quantity in timer0 cycles
#define nextAllowedValue mpPartialRefuelTankSize + 1
#endif // usePartialRefuel

#ifdef useChryslerMAPCorrection
static const uint8_t mpMAPpressureIdx =				nextAllowedValue;
static const uint8_t mpBaroPressureIdx =			mpMAPpressureIdx + 1;
static const uint8_t mpFuelPressureIdx =			mpBaroPressureIdx + 1;
static const uint8_t mpInjPressureIdx =				mpFuelPressureIdx + 1;
static const uint8_t mpInjectorCorrectionIdx =		mpInjPressureIdx + 1;
static const uint8_t mpAnalogMAPfloorIdx =			mpInjectorCorrectionIdx + 1;
static const uint8_t mpAnalogMAPnumerIdx =			mpAnalogMAPfloorIdx + 1;
static const uint8_t mpAnalogMAPdenomIdx =			mpAnalogMAPnumerIdx + 1;
#define nextAllowedValue mpAnalogMAPdenomIdx + 1
#ifdef useChryslerBaroSensor
static const uint8_t mpAnalogBaroFloorIdx =			nextAllowedValue;
static const uint8_t mpAnalogBaroNumerIdx =			mpAnalogBaroFloorIdx + 1;
static const uint8_t mpAnalogBaroDenomIdx =			mpAnalogBaroNumerIdx + 1;
#define nextAllowedValue mpAnalogBaroDenomIdx + 1
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

#ifdef useBarFuelEconVsSpeed
static const uint8_t mpFEvsSpeedMinThresholdIdx =	nextAllowedValue;					// minimum speed for fuel econ vs speed bargraph
static const uint8_t mpFEvsSpeedQuantumIdx =		mpFEvsSpeedMinThresholdIdx + 1;		// speed quantum for each bar in fuel econ vs speed bargraph
#define nextAllowedValue mpFEvsSpeedQuantumIdx + 1
#endif // useBarFuelEconVsSpeed

#ifdef useCPUreading
static const uint8_t mpMainLoopAccumulatorIdx =		nextAllowedValue;					// main loop stopwatch direct measurement
static const uint8_t mpIdleAccumulatorIdx =			mpMainLoopAccumulatorIdx + 1;		// stopwatch direct measurement of time that processor actually did jack and shit
static const uint8_t mpAvailableRAMidx =			mpIdleAccumulatorIdx + 1;			// amount of remaining free RAM
#define nextAllowedValue mpAvailableRAMidx + 1

#ifdef useDebugCPUreading
static const uint8_t mpDebugAccMainLoopIdx =		nextAllowedValue;					// copy of main loop stopwatch direct measurement
static const uint8_t mpDebugAccIdleIdx =			mpDebugAccMainLoopIdx + 1;			// copy of stopwatch direct measurement of time that processor actually did jack and shit
static const uint8_t mpDebugAccIdleProcessIdx =		mpDebugAccIdleIdx + 1;				// idle process stopwatch direct measurement
static const uint8_t mpDebugAccInterruptIdx =		mpDebugAccIdleProcessIdx + 1;		// copy of interrupt handler stopwatch direct measurement
static const uint8_t mpDebugAccDisplayIdx =			mpDebugAccInterruptIdx + 1;			// main display output stopwatch direct measurement
static const uint8_t mpDebugAccSWEET64idx =			mpDebugAccDisplayIdx + 1;			// SWEET64 engine stopwatch direct measurement
static const uint8_t mpDebugAccS64multIdx =			mpDebugAccSWEET64idx + 1;			// mult64 stopwatch direct measurement
static const uint8_t mpDebugCountS64multIdx =		mpDebugAccS64multIdx + 1;			// mult64 direct measurement counter
static const uint8_t mpDebugAccS64divIdx =			mpDebugCountS64multIdx + 1;			// div64 stopwatch direct measurement
static const uint8_t mpDebugCountS64divIdx =		mpDebugAccS64divIdx + 1;			// div64 direct measurement counter
#define nextAllowedValue mpDebugCountS64divIdx + 1
#endif // useDebugCPUreading
#endif // useCPUreading

static const uint8_t mpVariableMaxIdx =				nextAllowedValue;

#ifdef useDebugTerminalLabels
static const char terminalVolatileVarLabels[] PROGMEM = {
#ifdef useCPUreading
	"vSystemCycleIdx\r"						// timer0
#endif // useCPUreading
#ifdef useSoftwareClock
	"vClockCycleIdx\r"						// timer0
#endif // useSoftwareClock
	"vVehicleStopTimeoutIdx\r"				// timer0
	"vEngineOffTimeoutIdx\r"				// timer0
	"vButtonTimeoutIdx\r"					// timer0
	"vParkTimeoutIdx\r"						// timer0
	"vActivityTimeoutIdx\r"					// timer0
	"vDetectVehicleStopIdx\r"				// vss
	"vDetectEngineOffIdx\r"					// fi open
	"vMaximumVSSperiodIdx\r"				// vss
	"vMaximumEnginePeriodIdx\r"				// fi close
	"vInjectorOpenDelayIdx\r"				// fi close
	"vInjectorCloseDelayIdx\r"				// fi close
	"vInjectorTotalDelayIdx\r"				// fi close
	"vInjectorValidMaxWidthIdx\r"			// fi close
#ifdef useBarFuelEconVsTime
	"vFEvsTimePeriodTimeoutIdx\r"			// timer0
#endif // useBarFuelEconVsTime
#ifdef useDebugCPUreading
	"vInterruptAccumulatorIdx\r"			// all interrupts
#endif // useDebugCPUreading
#ifdef useDragRaceFunction
	"vDragInstantSpeedIdx\r"				// vss
	"vAccelHalfPeriodValueIdx\r"			// vss
	"vAccelFullPeriodValueIdx\r"			// vss
	"vAccelDistanceValueIdx\r"				// vss
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	"vCoastdownPeriodIdx\r"					// timer0, vss
	"vCoastdownMeasurement1Idx\r"			// vss
	"vCoastdownMeasurement2Idx\r"			// vss
	"vCoastdownMeasurement3Idx\r"			// vss
	"vCoastdownMeasurement4Idx\r"			// vss
#endif // useCoastDownCalculator
//#ifdef useDebugTerminal
	"vDebugValue1Idx\r"
	"vDebugValue2Idx\r"
	"vDebugValue3Idx\r"
	"vDebugValue4Idx\r"
//#endif // useDebugTerminal
};

static const char terminalMainProgramVarLabels[] PROGMEM = {
	"mpCyclesPerVolumeIdx\r"				// main program only
	"mpTankSizeIdx\r"						// main program only
	"mpBingoTankSizeIdx\r"					// main program only
#ifdef usePartialRefuel
	"mpPartialRefuelTankSize\r"				// main program only
#endif // usePartialRefuel
#ifdef useChryslerMAPCorrection
	"mpMAPpressureIdx\r"					// main program only
	"mpBaroPressureIdx\r"					// main program only
	"mpFuelPressureIdx\r"					// main program only
	"mpInjPressureIdx\r"					// main program only
	"mpInjectorCorrectionIdx\r"				// main program only
	"mpAnalogMAPfloorIdx\r"					// main program only
	"mpAnalogMAPnumerIdx\r"					// main program only
	"mpAnalogMAPdenomIdx\r"					// main program only
#ifdef useChryslerBaroSensor
	"mpAnalogBaroFloorIdx\r"				// main program only
	"mpAnalogBaroNumerIdx\r"				// main program only
	"mpAnalogBaroDenomIdx\r"				// main program only
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	"mpFEvsSpeedMinThresholdIdx\r"			// main program only
	"mpFEvsSpeedQuantumIdx\r"				// main program only
#endif // useBarFuelEconVsSpeed
#ifdef useCPUreading
	"mpMainLoopAccumulatorIdx\r"			// main program only
	"mpIdleAccumulatorIdx\r"				// main program only
	"mpAvailableRAMidx\r"					// main program only
#ifdef useDebugCPUreading
	"mpDebugAccMainLoopIdx\r"				// main program only
	"mpDebugAccIdleIdx\r"					// main program only
	"mpDebugAccIdleProcessIdx\r"			// main program only
	"mpDebugAccInterruptIdx\r"				// main program only
	"mpDebugAccDisplayIdx\r"				// main program only
	"mpDebugAccSWEET64idx\r"				// main program only
	"mpDebugAccS64multIdx\r"				// main program only
	"mpDebugCountS64multIdx\r"				// main program only
	"mpDebugAccS64divIdx\r"					// main program only
	"mpDebugCountS64divIdx\r"				// main program only
#endif // useDebugCPUreading
#endif // useCPUreading
};

#endif // useDebugTerminalLabels
volatile uint32_t volatileVariables[(uint16_t)(vVariableMaxIdx)];
uint32_t mainProgramVariables[(uint16_t)(mpVariableMaxIdx)];

static uint8_t metricFlag;

static const uint8_t softInitGuinoFlag =		0b10000000;
static const uint8_t hardInitGuinoFlag =		0b01000000;
static const uint8_t metricConversionFlag =		0b00100000;
static const uint8_t changeDisplayFlag =		0b00010000;
static const uint8_t calculateFuelParamFlag =	0b00001000;
static const uint8_t EEPROMbulkChangeFlag =		0b00000100;
static const uint8_t alternateFEmode =			0b00000010;
static const uint8_t metricMode =				0b00000001;

static const uint8_t detectEEPROMchangeFlag =	0b11111100;
static const uint8_t fuelEconOutputFlags =		0b00000011;

static char mBuff1[17]; // used by formatDecimal(), translateCalcIdx(), bigDigit::, bar graph routines
static char pBuff[12]; // used by parameterEdit::, clockSet::, function result output routines

static const char titleMPGuino[] PROGMEM = {
	"\xED" "MPGuino v1.95tav\r"
};

static const char dateMPGuino[] PROGMEM = {
	"2023-MAR-18\r"
};

const uint8_t displayPageCount = 9						// count of base number of data screens
#ifdef trackIdleEOCdata
	+ 3													// count of Idle/EOC tracking data screens
#endif // trackIdleEOCdata
;
const uint8_t mainScreenDisplayFormatSize = displayPageCount * 4;

const uint8_t guinosig =		0b10110111;

// start of remarkably long EEPROM stored settings section

/* parameter sizes in bits */

const uint8_t pSizeSignature =				24;

// display settings

#ifdef useLegacyLCD
const uint8_t pSizeContrast =				1;
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t pSizeLCDcolor =				3;
#endif // useAdafruitRGBLCDshield
const uint8_t pSizeMetricMode =				1;
const uint8_t pSizeAlternateFE =			1;
#ifdef useFuelCost
const uint8_t pSizeFuelUnitCost =			24;
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pSizeExpansionPin1Mode =		3;
const uint8_t pSizeExpansionPin2Mode =		3;
#endif // useExpansionPins
#ifdef useCarVoltageOutput
const uint8_t pSizeVoltageOffset =			12;
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
const uint8_t pSizeSerialDataLogging =		1;
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
const uint8_t pSizeWindowTripFilter =		1;
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
const uint8_t pSizeFEvsTime =				16;
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pSizeBarLowSpeedCutoff =		24;
const uint8_t pSizeBarSpeedQuantumIdx =		24;
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
const uint8_t pSizeSysFuelPressure =		32;
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pSizeRefFuelPressure =		32;
const uint8_t pSizeInjectorCount =			8;
const uint8_t pSizeInjectorSize =			20;
#endif // useCalculatedFuelFactor
const uint8_t pSizeMicroSecondsPerGallon =	32;
const uint8_t pSizeInjEdgeTrigger =			1;
const uint8_t pSizeInjectorOpeningTime =	16;
const uint8_t pSizeInjectorClosingTime =	16;
const uint8_t pSizeCrankRevPerInj =			8;
const uint8_t pSizeMinGoodRPM =				16;

// vehicle speed sensor settings

const uint8_t pSizePulsesPerDistance =		18;
const uint8_t pSizeVSSpause =				8;
const uint8_t pSizeMinGoodSpeed =			16;
const uint8_t pSizeTankSize =				24;
const uint8_t pSizeTankBingoSize =			pSizeTankSize;
#ifdef useChryslerMAPCorrection

// Chrysler settings

const uint8_t pSizeMAPsensorFloor =			16;
const uint8_t pSizeMAPsensorCeiling =		16;
const uint8_t pSizeMAPsensorRange =			32;
const uint8_t pSizeMAPsensorOffset =		32;
#ifdef useChryslerBaroSensor
const uint8_t pSizeBaroSensorFloor =		16;
const uint8_t pSizeBaroSensorCeiling =		16;
const uint8_t pSizeBaroSensorRange =		32;
const uint8_t pSizeBaroSensorOffset =		32;
#else // useChryslerBaroSensor
const uint8_t pSizeBarometricPressure =		32;
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
const uint8_t pSizeVehicleMass =			16;
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pSizeVehicleFrontalArea =		16;
const uint8_t pSizeLocustDensity =			16;
const uint8_t pSizeCoefficientD =			16;
const uint8_t pSizeCoefficientV =			16;
const uint8_t pSizeCoefficientRR =			16;
const uint8_t pSizeCoastdownSamplePeriod =	8;
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const uint8_t pSizeDragSpeed =				24;
const uint8_t pSizeDragDistance =			16;
const uint8_t pSizeDragAutoFlag =			1;
#endif // useDragRaceFunction

// timeout settings

const uint8_t pSizeIdleTimeout =			8;
const uint8_t pSizeEOCtimeout =				8;
const uint8_t pSizeButtonTimeout =			8;
const uint8_t pSizeParkTimeout =			8;
const uint8_t pSizeActivityTimeout =		8;
const uint8_t pSizeWakeupResetCurrentOnEngine =	1;
const uint8_t pSizeWakeupResetCurrentOnMove =	1;
#ifdef useSavedTrips
const uint8_t pSizeAutoSaveActive =			1;
#endif // useSavedTrips

// miscellaneous settings

const uint8_t pSizeScratchpad =				32;

// settings inaccessible from the menu

#ifdef usePartialRefuel
const uint8_t pSizeRefuelSize =				pSizeTankSize;
#ifdef useEEPROMtripStorage
const uint8_t pSizeRefuelSaveSizeIdx =		pSizeTankSize;
#endif // useEEPROMtripStorage
#endif // usePartialRefuel

#ifdef useEEPROMtripStorage
const uint8_t pSizeCurrTripSignatureIdx =	8;
const uint8_t pSizeTankTripSignatureIdx =	8;
const uint8_t pSizeCurrTripVSSpulseIdx =	32;
const uint8_t pSizeCurrTripVSScycleIdx =	64;
const uint8_t pSizeCurrTripInjPulseIdx =	32;
const uint8_t pSizeCurrTripInjCycleIdx =	64;
const uint8_t pSizeCurrTripEngCycleIdx =	64;
const uint8_t pSizeTankTripVSSpulseIdx =	32;
const uint8_t pSizeTankTripVSScycleIdx =	64;
const uint8_t pSizeTankTripInjPulseIdx =	32;
const uint8_t pSizeTankTripInjCycleIdx =	64;
const uint8_t pSizeTankTripEngCycleIdx =	64;
#ifdef trackIdleEOCdata
const uint8_t pSizeCurrIEOCvssPulseIdx =	32;
const uint8_t pSizeCurrIEOCvssCycleIdx =	64;
const uint8_t pSizeCurrIEOCinjPulseIdx =	32;
const uint8_t pSizeCurrIEOCinjCycleIdx =	64;
const uint8_t pSizeCurrIEOCengCycleIdx =	64;
const uint8_t pSizeTankIEOCvssPulseIdx =	32;
const uint8_t pSizeTankIEOCvssCycleIdx =	64;
const uint8_t pSizeTankIEOCinjPulseIdx =	32;
const uint8_t pSizeTankIEOCinjCycleIdx =	64;
const uint8_t pSizeTankIEOCengCycleIdx =	64;
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage

#define byteSize(bitLength) ((((bitLength & 0x07) != 0)? 1 : 0) + (bitLength / 8))

/* parameter addresses from sizes defined above */

const uint8_t eeAdrSignature = 0;

#define nextAllowedValue eeAdrSignature
const unsigned int pAddressSignature =					nextAllowedValue;
#define nextAllowedValue pAddressSignature + byteSize(pSizeSignature)

// display settings

#ifdef useLegacyLCD
const unsigned int pAddressContrast =					nextAllowedValue;
#define nextAllowedValue pAddressContrast + byteSize(pSizeContrast)
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const unsigned int pAddressLCDcolor =					nextAllowedValue;
#define nextAllowedValue pAddressLCDcolor + byteSize(pSizeLCDcolor)
#endif // useAdafruitRGBLCDshield
const unsigned int pAddressMetricMode =					nextAllowedValue;
const unsigned int pAddressAlternateFE =				pAddressMetricMode + byteSize(pSizeMetricMode);
#define nextAllowedValue pAddressAlternateFE + byteSize(pSizeAlternateFE)
#ifdef useFuelCost
const unsigned int pAddressFuelUnitCost =				nextAllowedValue;
#define nextAllowedValue pAddressFuelUnitCost + byteSize(pSizeFuelUnitCost)
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pAddressExpansionPin1Mode =				nextAllowedValue;
const uint8_t pAddressExpansionPin2Mode =				pAddressExpansionPin1Mode + byteSize(pSizeExpansionPin1Mode);
#define nextAllowedValue pAddressExpansionPin2Mode + byteSize(pSizeExpansionPin2Mode)
#endif // useExpansionPins
#ifdef useCarVoltageOutput
const unsigned int pAddressVoltageOffset =				nextAllowedValue;
#define nextAllowedValue pAddressVoltageOffset + byteSize(pSizeVoltageOffset)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
const unsigned int pAddressSerialDataLogging =			nextAllowedValue;
#define nextAllowedValue pAddressSerialDataLogging + byteSize(pSizeSerialDataLogging)
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
const unsigned int pAddressWindowTripFilter =			nextAllowedValue;
#define nextAllowedValue pAddressWindowTripFilter + byteSize(pSizeWindowTripFilter)
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
const unsigned int pAddressFEvsTime =					nextAllowedValue;
#define nextAllowedValue pAddressFEvsTime + byteSize(pSizeFEvsTime)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const unsigned int pAddressBarLowSpeedCutoff =			nextAllowedValue;
const unsigned int pAddressBarSpeedQuantumIdx =			pAddressBarLowSpeedCutoff + byteSize(pSizeBarLowSpeedCutoff);
#define nextAllowedValue pAddressBarSpeedQuantumIdx + byteSize(pSizeBarSpeedQuantumIdx)
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
const unsigned int pAddressSysFuelPressure =			nextAllowedValue;
#define nextAllowedValue pAddressSysFuelPressure + byteSize(pSizeSysFuelPressure)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const unsigned int pAddressRefFuelPressure =			nextAllowedValue;
const unsigned int pAddressInjectorCount =				pAddressRefFuelPressure + byteSize(pSizeRefFuelPressure);
const unsigned int pAddressInjectorSize =				pAddressInjectorCount + byteSize(pSizeInjectorCount);
#define nextAllowedValue pAddressInjectorSize + byteSize(pSizeInjectorSize)
#endif // useCalculatedFuelFactor
const unsigned int pAddressMicroSecondsPerGallon =		nextAllowedValue;
const unsigned int pAddressInjEdgeTrigger =				pAddressMicroSecondsPerGallon + byteSize(pSizeMicroSecondsPerGallon);
const unsigned int pAddressInjectorOpeningTime =		pAddressInjEdgeTrigger + byteSize(pSizeInjEdgeTrigger);
const unsigned int pAddressInjectorClosingTime =		pAddressInjectorOpeningTime + byteSize(pSizeInjectorOpeningTime);
const unsigned int pAddressCrankRevPerInj =				pAddressInjectorClosingTime + byteSize(pSizeInjectorClosingTime);
const unsigned int pAddressMinGoodRPM =					pAddressCrankRevPerInj + byteSize(pSizeCrankRevPerInj);
#define nextAllowedValue pAddressMinGoodRPM + byteSize(pSizeMinGoodRPM)

// vehicle speed sensor settings

const unsigned int pAddressPulsesPerDistance =			nextAllowedValue;
const unsigned int pAddressVSSpause =					pAddressPulsesPerDistance + byteSize(pSizePulsesPerDistance);
const unsigned int pAddressMinGoodSpeed =				pAddressVSSpause + byteSize(pSizeVSSpause);
#define nextAllowedValue pAddressMinGoodSpeed + byteSize(pSizeMinGoodSpeed)

// fuel tank size settings

const unsigned int pAddressTankSize =					nextAllowedValue;
const unsigned int pAddressTankBingoSize =				pAddressTankSize + byteSize(pSizeTankSize);
#define nextAllowedValue pAddressTankBingoSize + byteSize(pSizeTankBingoSize)
#ifdef useChryslerMAPCorrection

// Chrysler settings

const unsigned int pAddressMAPsensorFloor =				nextAllowedValue;
const unsigned int pAddressMAPsensorCeiling =			pAddressMAPsensorFloor + byteSize(pSizeMAPsensorFloor);
const unsigned int pAddressMAPsensorRange =				pAddressMAPsensorCeiling + byteSize(pSizeMAPsensorCeiling);
const unsigned int pAddressMAPsensorOffset =			pAddressMAPsensorRange + byteSize(pSizeMAPsensorRange);
#define nextAllowedValue pAddressMAPsensorOffset + byteSize(pSizeMAPsensorOffset)
#ifdef useChryslerBaroSensor
const unsigned int pAddressBaroSensorFloor =			nextAllowedValue;
const unsigned int pAddressBaroSensorCeiling =			pAddressBaroSensorFloor + byteSize(pSizeBaroSensorFloor);
const unsigned int pAddressBaroSensorRange =			pAddressBaroSensorCeiling + byteSize(pSizeBaroSensorCeiling);
const unsigned int pAddressBaroSensorOffset =			pAddressBaroSensorRange + byteSize(pSizeBaroSensorRange);
#define nextAllowedValue pAddressBaroSensorOffset + byteSize(pSizeBaroSensorOffset)
#else // useChryslerBaroSensor
const unsigned int pAddressBarometricPressure =			nextAllowedValue;
#define nextAllowedValue pAddressBarometricPressure + byteSize(pSizeBarometricPressure)
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
const unsigned int pAddressVehicleMass =				nextAllowedValue;
#define nextAllowedValue pAddressVehicleMass + byteSize(pSizeVehicleMass)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const unsigned int pAddressVehicleFrontalArea =			nextAllowedValue;
const unsigned int pAddressLocustDensity =				pAddressVehicleFrontalArea + byteSize(pSizeVehicleFrontalArea);
const unsigned int pAddressCoefficientD =				pAddressLocustDensity + byteSize(pSizeLocustDensity);
const unsigned int pAddressCoefficientV =				pAddressCoefficientD + byteSize(pSizeCoefficientD);
const unsigned int pAddressCoefficientRR =				pAddressCoefficientV + byteSize(pSizeCoefficientV);
const unsigned int pAddressCoastdownSamplePeriod =		pAddressCoefficientRR + byteSize(pSizeCoefficientRR);
#define nextAllowedValue pAddressCoastdownSamplePeriod + byteSize(pSizeCoastdownSamplePeriod)
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const unsigned int pAddressDragSpeed =					nextAllowedValue;
const unsigned int pAddressDragDistance =				pAddressDragSpeed + byteSize(pSizeDragSpeed);
const unsigned int pAddressDragAutoFlag =				pAddressDragDistance + byteSize(pSizeDragDistance);
#define nextAllowedValue pAddressDragAutoFlag + byteSize(pSizeDragAutoFlag)
#endif // useDragRaceFunction

// timeout settings

const unsigned int pAddressIdleTimeout =				nextAllowedValue;
const unsigned int pAddressEOCtimeout =					pAddressIdleTimeout + byteSize(pSizeIdleTimeout);
const unsigned int pAddressButtonTimeout =				pAddressEOCtimeout + byteSize(pSizeEOCtimeout);
const unsigned int pAddressParkTimeout =				pAddressButtonTimeout + byteSize(pSizeButtonTimeout);
const unsigned int pAddressActivityTimeout =			pAddressParkTimeout + byteSize(pSizeParkTimeout);
const unsigned int pAddressWakeupResetCurrentOnEngine =	pAddressActivityTimeout + byteSize(pSizeActivityTimeout);
const unsigned int pAddressWakeupResetCurrentOnMove =	pAddressWakeupResetCurrentOnEngine + byteSize(pSizeWakeupResetCurrentOnEngine);
#define nextAllowedValue pAddressWakeupResetCurrentOnMove + byteSize(pSizeWakeupResetCurrentOnMove)
#ifdef useSavedTrips
const unsigned int pAddressAutoSaveActive =				nextAllowedValue;
#define nextAllowedValue pAddressAutoSaveActive + byteSize(pSizeAutoSaveActive)
#endif // useSavedTrips

// miscellaneous settings

const unsigned int pAddressScratchpad =					nextAllowedValue;
#define nextAllowedValue pAddressScratchpad + byteSize(pSizeScratchpad)

// settings inaccessible from the menu

#ifdef usePartialRefuel
const unsigned int pAddressRefuelSize =					nextAllowedValue;
#define nextAllowedValue pAddressRefuelSize + byteSize(pSizeRefuelSize)
#ifdef useEEPROMtripStorage
const uint8_t pAddressRefuelSaveSizeIdx =				nextAllowedValue;
#define nextAllowedValue pAddressRefuelSaveSizeIdx + byteSize(pSizeRefuelSaveSizeIdx)
#endif // useEEPROMtripStorage
#endif // usePartialRefuel

#ifdef useEEPROMtripStorage
const uint8_t pAddressCurrTripSignatureIdx =			nextAllowedValue;
const uint8_t pAddressTankTripSignatureIdx =			pAddressCurrTripSignatureIdx + byteSize(pSizeCurrTripSignatureIdx);
const uint8_t pAddressCurrTripVSSpulseIdx =				pAddressTankTripSignatureIdx + byteSize(pSizeTankTripSignatureIdx);
const uint8_t pAddressCurrTripVSScycleIdx =				pAddressCurrTripVSSpulseIdx + byteSize(pSizeCurrTripVSSpulseIdx);
const uint8_t pAddressCurrTripInjPulseIdx =				pAddressCurrTripVSScycleIdx + byteSize(pSizeCurrTripVSScycleIdx);
const uint8_t pAddressCurrTripInjCycleIdx =				pAddressCurrTripInjPulseIdx + byteSize(pSizeCurrTripInjPulseIdx);
const uint8_t pAddressCurrTripEngCycleIdx =				pAddressCurrTripInjCycleIdx + byteSize(pSizeCurrTripInjCycleIdx);
const uint8_t pAddressTankTripVSSpulseIdx =				pAddressCurrTripEngCycleIdx + byteSize(pSizeCurrTripEngCycleIdx);
const uint8_t pAddressTankTripVSScycleIdx =				pAddressTankTripVSSpulseIdx + byteSize(pSizeTankTripVSSpulseIdx);
const uint8_t pAddressTankTripInjPulseIdx =				pAddressTankTripVSScycleIdx + byteSize(pSizeTankTripVSScycleIdx);
const uint8_t pAddressTankTripInjCycleIdx =				pAddressTankTripInjPulseIdx + byteSize(pSizeTankTripInjPulseIdx);
const uint8_t pAddressTankTripEngCycleIdx =				pAddressTankTripInjCycleIdx + byteSize(pSizeTankTripInjCycleIdx);
#define nextAllowedValue pAddressTankTripEngCycleIdx + byteSize(pSizeTankTripEngCycleIdx)
#ifdef trackIdleEOCdata
const uint8_t pAddressCurrIEOCvssPulseIdx =				nextAllowedValue;
const uint8_t pAddressCurrIEOCvssCycleIdx =				pAddressCurrIEOCvssPulseIdx + byteSize(pSizeCurrIEOCvssPulseIdx);
const uint8_t pAddressCurrIEOCinjPulseIdx =				pAddressCurrIEOCvssCycleIdx + byteSize(pSizeCurrIEOCvssCycleIdx);
const uint8_t pAddressCurrIEOCinjCycleIdx =				pAddressCurrIEOCinjPulseIdx + byteSize(pSizeCurrIEOCinjPulseIdx);
const uint8_t pAddressCurrIEOCengCycleIdx =				pAddressCurrIEOCinjCycleIdx + byteSize(pSizeCurrIEOCinjCycleIdx);
const uint8_t pAddressTankIEOCvssPulseIdx =				pAddressCurrIEOCengCycleIdx + byteSize(pSizeCurrIEOCengCycleIdx);
const uint8_t pAddressTankIEOCvssCycleIdx =				pAddressTankIEOCvssPulseIdx + byteSize(pSizeTankIEOCvssPulseIdx);
const uint8_t pAddressTankIEOCinjPulseIdx =				pAddressTankIEOCvssCycleIdx + byteSize(pSizeTankIEOCvssCycleIdx);
const uint8_t pAddressTankIEOCinjCycleIdx =				pAddressTankIEOCinjPulseIdx + byteSize(pSizeTankIEOCinjPulseIdx);
const uint8_t pAddressTankIEOCengCycleIdx =				pAddressTankIEOCinjCycleIdx + byteSize(pSizeTankIEOCinjCycleIdx);
#define nextAllowedValue pAddressTankIEOCengCycleIdx + byteSize(pSizeTankIEOCengCycleIdx)
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage

const unsigned int eeAdrSettingsEnd =					nextAllowedValue;

#ifdef useScreenEditor
const unsigned int eeAdrScreensStart =					nextAllowedValue;
const unsigned int eeAdrScreensEnd =					eeAdrScreensStart + 2 * mainScreenDisplayFormatSize;
#define nextAllowedValue eeAdrScreensEnd

#endif // useScreenEditor
const unsigned int eeAdrStorageEnd =					nextAllowedValue;

#define nextAllowedValue 0
const uint8_t pSignatureIdx =			nextAllowedValue;
#define nextAllowedValue pSignatureIdx + 1

/* parameter indexes corresponding to sizes and addresses defined above */

// this is the start of all EEPROM parameters that show up in the general settings screen

const uint8_t eePtrSettingsStart =			nextAllowedValue;

// display settings

const uint8_t eePtrSettingsDispStart =		nextAllowedValue;

#ifdef useLegacyLCD
const uint8_t pContrastIdx =				nextAllowedValue;
#define nextAllowedValue pContrastIdx + 1
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t pLCDcolorIdx =				nextAllowedValue;
#define nextAllowedValue pLCDcolorIdx + 1
#endif // useAdafruitRGBLCDshield
const uint8_t pMetricModeIdx =				nextAllowedValue;
const uint8_t pAlternateFEidx =				pMetricModeIdx + 1;
#define nextAllowedValue pAlternateFEidx + 1
#ifdef useFuelCost
const uint8_t pCostPerQuantity =			nextAllowedValue;
#define nextAllowedValue pCostPerQuantity + 1
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pExpansionPin1Mode =			nextAllowedValue;
const uint8_t pExpansionPin2Mode =			pExpansionPin1Mode + 1;
#define nextAllowedValue pExpansionPin2Mode + 1
#endif // useExpansionPins
#ifdef useCarVoltageOutput
const uint8_t pVoltageOffset =				nextAllowedValue;
#define nextAllowedValue pVoltageOffset + 1
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
const uint8_t pSerialDataLoggingIdx =		nextAllowedValue;
#define nextAllowedValue pSerialDataLoggingIdx + 1
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
const uint8_t pWindowTripFilterIdx =		nextAllowedValue;
#define nextAllowedValue pWindowTripFilterIdx + 1
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
const uint8_t pFEvsTimeIdx =				nextAllowedValue;
#define nextAllowedValue pFEvsTimeIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pBarLowSpeedCutoffIdx =		nextAllowedValue;
const uint8_t pBarSpeedQuantumIdx =			pBarLowSpeedCutoffIdx + 1;
#define nextAllowedValue pBarSpeedQuantumIdx + 1
#endif // useBarFuelEconVsSpeed

const uint8_t eePtrSettingsDispEnd =		nextAllowedValue;
const uint8_t eePtrSettingsDispLen =		eePtrSettingsDispEnd - eePtrSettingsDispStart;

// fuel injection settings

const uint8_t eePtrSettingsInjStart =		nextAllowedValue;

#ifdef useIsqrt
const uint8_t pSysFuelPressureIdx =			nextAllowedValue;
#define nextAllowedValue pSysFuelPressureIdx + 1
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pRefFuelPressureIdx =			nextAllowedValue;
const uint8_t pInjectorCountIdx =			pRefFuelPressureIdx + 1;
const uint8_t pInjectorSizeIdx =			pInjectorCountIdx + 1;
#define nextAllowedValue pInjectorSizeIdx + 1
#endif // useCalculatedFuelFactor
const uint8_t pMicroSecondsPerGallonIdx =	nextAllowedValue;
const uint8_t pInjEdgeTriggerIdx =			pMicroSecondsPerGallonIdx + 1;
const uint8_t pInjectorOpeningTimeIdx =		pInjEdgeTriggerIdx + 1;
const uint8_t pInjectorClosingTimeIdx =		pInjectorOpeningTimeIdx + 1;
const uint8_t pCrankRevPerInjIdx =			pInjectorClosingTimeIdx + 1;
const uint8_t pMinGoodRPMidx =				pCrankRevPerInjIdx + 1;
#define nextAllowedValue pMinGoodRPMidx + 1

const uint8_t eePtrSettingsInjEnd =			nextAllowedValue;
const uint8_t eePtrSettingsInjLen =			eePtrSettingsInjEnd - eePtrSettingsInjStart;

// vehicle speed sensor settings

const uint8_t eePtrSettingsVSSstart =		nextAllowedValue;

const uint8_t pPulsesPerDistanceIdx =		nextAllowedValue;
const uint8_t pVSSpauseIdx =				pPulsesPerDistanceIdx + 1;
const uint8_t pMinGoodSpeedidx =			pVSSpauseIdx + 1;
#define nextAllowedValue pMinGoodSpeedidx + 1

const uint8_t eePtrSettingsVSSend =			nextAllowedValue;
const uint8_t eePtrSettingsVSSlen =			eePtrSettingsVSSend - eePtrSettingsVSSstart;

// fuel tank size settings

const uint8_t eePtrSettingsTankStart =		nextAllowedValue;

const uint8_t pTankSizeIdx =				nextAllowedValue;
const uint8_t pTankBingoSizeIdx =			pTankSizeIdx + 1;
#define nextAllowedValue pTankBingoSizeIdx + 1

const uint8_t eePtrSettingsTankEnd =		nextAllowedValue;
const uint8_t eePtrSettingsTankLen =		eePtrSettingsTankEnd - eePtrSettingsTankStart;
#ifdef useChryslerMAPCorrection

// chrysler settings

const uint8_t eePtrSettingsCRFICstart =		nextAllowedValue;

const uint8_t pMAPsensorFloorIdx =			nextAllowedValue;
const uint8_t pMAPsensorCeilingIdx =		pMAPsensorFloorIdx + 1;
const uint8_t pMAPsensorRangeIdx =			pMAPsensorCeilingIdx + 1;
const uint8_t pMAPsensorOffsetIdx =			pMAPsensorRangeIdx + 1;
#define nextAllowedValue pMAPsensorOffsetIdx + 1
#ifdef useChryslerBaroSensor
const uint8_t pBaroSensorFloorIdx =			nextAllowedValue;
const uint8_t pBaroSensorCeilingIdx =		pBaroSensorFloorIdx + 1;
const uint8_t pBaroSensorRangeIdx =			pBaroSensorCeilingIdx + 1;
const uint8_t pBaroSensorOffsetIdx =		pBaroSensorRangeIdx + 1;
#define nextAllowedValue pBaroSensorOffsetIdx + 1
#else // useChryslerBaroSensor
const uint8_t pBarometricPressureIdx =		nextAllowedValue;
#define nextAllowedValue pBarometricPressureIdx + 1
#endif // useChryslerBaroSensor

const uint8_t eePtrSettingsCRFICend =		nextAllowedValue;
const uint8_t eePtrSettingsCRFIClen =		eePtrSettingsCRFICend - eePtrSettingsCRFICstart;
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

const uint8_t eePtrSettingsACDstart =		nextAllowedValue;

#ifdef useVehicleMass
const uint8_t pVehicleMassIdx =				nextAllowedValue;
#define nextAllowedValue pVehicleMassIdx + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pVehicleFrontalAreaIdx =		nextAllowedValue;
const uint8_t pLocustDensityIdx =			pVehicleFrontalAreaIdx + 1;
const uint8_t pCoefficientDidx =			pLocustDensityIdx + 1;
const uint8_t pCoefficientVidx =			pCoefficientDidx + 1;
const uint8_t pCoefficientRRidx =			pCoefficientVidx + 1;
const uint8_t pCoastdownSamplePeriodIdx =	pCoefficientRRidx + 1;
#define nextAllowedValue pCoastdownSamplePeriodIdx + 1
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const uint8_t pDragSpeedIdx =				nextAllowedValue;
const uint8_t pDragDistanceIdx =			pDragSpeedIdx + 1;
const uint8_t pDragAutoFlagIdx =			pDragDistanceIdx + 1;
#define nextAllowedValue pDragAutoFlagIdx + 1
#endif // useDragRaceFunction

const uint8_t eePtrSettingsACDend =			nextAllowedValue;
const uint8_t eePtrSettingsACDlen =			eePtrSettingsACDend - eePtrSettingsACDstart;

// timeout settings

const uint8_t eePtrSettingsTimeoutStart =	nextAllowedValue;

const uint8_t pIdleTimeoutIdx =				nextAllowedValue;
const uint8_t pEOCtimeoutIdx =				pIdleTimeoutIdx + 1;
const uint8_t pButtonTimeoutIdx =			pEOCtimeoutIdx + 1;
const uint8_t pParkTimeoutIdx =				pButtonTimeoutIdx + 1;
const uint8_t pActivityTimeoutIdx =			pParkTimeoutIdx + 1;
const uint8_t pWakeupResetCurrentOnEngineIdx =	pActivityTimeoutIdx + 1;
const uint8_t pWakeupResetCurrentOnMoveIdx =	pWakeupResetCurrentOnEngineIdx + 1;
#define nextAllowedValue pWakeupResetCurrentOnMoveIdx + 1
#ifdef useSavedTrips
const uint8_t pAutoSaveActiveIdx =			nextAllowedValue;
#define nextAllowedValue pAutoSaveActiveIdx + 1
#endif // useSavedTrips

const uint8_t eePtrSettingsTimeoutEnd =		nextAllowedValue;
const uint8_t eePtrSettingsTimeoutLen =		eePtrSettingsTimeoutEnd - eePtrSettingsTimeoutStart;

// miscellaneous settings

const uint8_t eePtrSettingsMiscStart =		nextAllowedValue;

const uint8_t pScratchpadIdx =				nextAllowedValue;
#define nextAllowedValue pScratchpadIdx + 1

const uint8_t eePtrSettingsMiscEnd =		nextAllowedValue;
const uint8_t eePtrSettingsMiscLen =		eePtrSettingsMiscEnd - eePtrSettingsMiscStart;

const uint8_t eePtrSettingsVisibleEnd =	nextAllowedValue;

// settings inaccessible from the menu

#ifdef usePartialRefuel
const uint8_t pRefuelSizeIdx =				nextAllowedValue;
#define nextAllowedValue pRefuelSizeIdx + 1
#endif // usePartialRefuel

const uint8_t eePtrSettingsEnd =			nextAllowedValue;

#ifdef useEEPROMtripStorage
const uint8_t eePtrSavedTripsStart = 		nextAllowedValue;

#ifdef usePartialRefuel
const uint8_t pRefuelSaveSizeIdx =			nextAllowedValue;
#define nextAllowedValue pRefuelSaveSizeIdx + 1
#endif // usePartialRefuel
const uint8_t pCurrTripSignatureIdx =		nextAllowedValue;
const uint8_t pTankTripSignatureIdx =		pCurrTripSignatureIdx + 1;
const uint8_t pCurrTripVSSpulseIdx =		pTankTripSignatureIdx + 1;
const uint8_t pCurrTripVSScycleIdx =		pCurrTripVSSpulseIdx + 1;
const uint8_t pCurrTripInjPulseIdx =		pCurrTripVSScycleIdx + 1;
const uint8_t pCurrTripInjCycleIdx =		pCurrTripInjPulseIdx + 1;
const uint8_t pCurrTripEngCycleIdx =		pCurrTripInjCycleIdx + 1;
const uint8_t pTankTripVSSpulseIdx =		pCurrTripEngCycleIdx + 1;
const uint8_t pTankTripVSScycleIdx =		pTankTripVSSpulseIdx + 1;
const uint8_t pTankTripInjPulseIdx =		pTankTripVSScycleIdx + 1;
const uint8_t pTankTripInjCycleIdx =		pTankTripInjPulseIdx + 1;
const uint8_t pTankTripEngCycleIdx =		pTankTripInjCycleIdx + 1;
#define nextAllowedValue pTankTripEngCycleIdx + 1
#ifdef trackIdleEOCdata
const uint8_t pCurrIEOCvssPulseIdx =		nextAllowedValue;
const uint8_t pCurrIEOCvssCycleIdx =		pCurrIEOCvssPulseIdx + 1;
const uint8_t pCurrIEOCinjPulseIdx =		pCurrIEOCvssCycleIdx + 1;
const uint8_t pCurrIEOCinjCycleIdx =		pCurrIEOCinjPulseIdx + 1;
const uint8_t pCurrIEOCengCycleIdx =		pCurrIEOCinjCycleIdx + 1;
const uint8_t pTankIEOCvssPulseIdx =		pCurrIEOCengCycleIdx + 1;
const uint8_t pTankIEOCvssCycleIdx =		pTankIEOCvssPulseIdx + 1;
const uint8_t pTankIEOCinjPulseIdx =		pTankIEOCvssCycleIdx + 1;
const uint8_t pTankIEOCinjCycleIdx =		pTankIEOCinjPulseIdx + 1;
const uint8_t pTankIEOCengCycleIdx =		pTankIEOCinjCycleIdx + 1;
#define nextAllowedValue pTankIEOCengCycleIdx + 1
#endif // trackIdleEOCdata

const uint8_t eePtrSavedTripsEnd = 			nextAllowedValue;
const uint8_t eePtrSavedTripsLen = 			eePtrSavedTripsEnd - eePtrSavedTripsStart;

#endif // useEEPROMtripStorage
const uint8_t eePtrStorageEnd =				nextAllowedValue;

#ifdef useScreenEditor
const uint8_t eePtrScreensStart =			nextAllowedValue;
const uint8_t eePtrScreensEnd =				eePtrScreensStart + mainScreenDisplayFormatSize;
#define nextAllowedValue eePtrScreensEnd

#endif // useScreenEditor
const uint8_t eePtrEnd =					nextAllowedValue;

/* labels for parameters definitions above */

const char parmLabels[] PROGMEM = {
	"MPGuinoSig\r"

// display settings

#ifdef useLegacyLCD
	"Contrast\r"
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	"LCD BG Color\r"
#endif // useAdafruitRGBLCDshield
	"Metric 1-Yes\r"
	"AltFEformat 1-Y\r"
#ifdef useFuelCost
#ifdef useImperialGallon
	"Price*1000/" "\xEB" "ImpGa" "\xEC" "L" "\xED" "\r"
#else  // useImperialGallon
	"Price*1000/" "\xEB" "USgal" "\xEC" "\xED" "\r"
#endif // useImperialGallon
#endif // useFuelCost
#ifdef useExpansionPins
	"OutPtPin 1 Mode\r"
	"OutPtPin 2 Mode\r"
#endif // useExpansionPins
#ifdef useCarVoltageOutput
	"V(diode)*1000\r"
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	"DLogSerial 1-Yes\r"
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
	"FE Filter 1-Y\r"
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	"FE/Time Period s\r"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"bgLower*1000 " "\xEB" "MPH" "\xEC" "kph" "\xED" "\r"
	"bgSize*1000 " "\xEB" "MPH" "\xEC" "kph" "\xED" "\r"
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	"P(Fuel) " "\xEB" "psig*1K" "\xEC" "Pa" "\xED" "\r"
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	"P(Ref) " "\xEB" "psig*1K" "\xEC" "Pa" "\xED" "\r"
	"Injector Count\r"
	"InjSiz cc/min*1K\r"
#endif // useCalculatedFuelFactor
	"Microsec/USgal\r"
	"InjTrg 0-Dn 1-Up\r"
	"Inj Delay (us)\r"
	"InjCloseDly (us)\r"
	"Revs/Inj Pulse\r"
	"Min good RPM\r"

// vehicle speed sensor settings

	"VSS Pulses/" "\xEB" "Mile" "\xEC" "km" "\xED" "\r"
	"VSS Delay (ms)\r"
	"Min Speed " "\xEB" "MPH" "\xEC" "KPH" "\xED" "*1K \r"

// fuel tank size settings

#ifdef useImperialGallon
	"Tank*1000 " "\xEB" "ImpGal" "\xEC" "L" "\xED" "\r"
	"Bingo*1000 " "\xEB" "ImpGal" "\xEC" "L" "\xED" "\r"
#else // useImperialGallon
	"Tank*1000 " "\xEB" "USgal" "\xEC" "L" "\xED" "\r"
	"Bingo*1000 " "\xEB" "USgal" "\xEC" "L" "\xED" "\r"
#endif // useImperialGallon
#ifdef useChryslerMAPCorrection

// Chrysler settings

	"MAPfloor (mV)\r"
	"MAPceiling (mV)\r"
	"MAPrng " "\xEB" "psi*1000" "\xEC" "Pa" "\xED" "\r"
	"MAPofst " "\xEB" "psi*1000" "\xEC" "Pa" "\xED" "\r"
#ifdef useChryslerBaroSensor
	"BaroFloor (mV)\r"
	"BaroCeiling (mV)\r"
	"BaroRng " "\xEB" "psi*1000" "\xEC" "Pa" "\xED" "\r"
	"BaroOfst " "\xEB" "psi*1000" "\xEC" "Pa" "\xED" "\r"
#else // useChryslerBaroSensor
	"BaroPrs " "\xEB" "psi*1000" "\xEC" "Pa" "\xED" "\r"
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	"" "\xEB" "Weight" "\xEC" "Mass" "\xED" " (\xeblbs" "\xEC" "kg" "\xED" ")\r"
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	"FrArea*1000 " "\xEB" "ft" "\xEC" "m" "\xED" "^2\r"
	"rho*1000 " "\xEB" "lb/yd" "\xEC" "kg/m" "\xED" "^3\r"
	"C(d) * 1000\r"
	"C(v) * 1000\r"
	"C(rr) * 1000\r"
	"Cd SamplePd (s)\r"
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	"DragSpd " "\xEB" "MPH" "\xEC" "kph" "\xED" "*1000\r"
	"DragDist " "\xEB" "mi" "\xEC" "km" "\xED" "*1000\r"
	"DragAutoTrigger\r"
#endif // useDragRaceFunction

// timeout settings

	"Idle Timeout (s)\r"
	"EOC Timeout (s)\r"
	"BtnPress T/O (s)\r"
	"Park Timeout (s)\r"
	"OFF Timeout (s)\r"
	"WakeEngRst CURR\r"
	"WakeVSSRst CURR\r"
#ifdef useSavedTrips
	"AutoSaveTrip 1-Y\r"
#endif // useSavedTrips

// miscellaneous settings

	"Scratchpad(odo?)\r"

// settings inaccessible from the menu

#ifdef usePartialRefuel
#ifdef useImperialGallon
	"Refill*1K " "\xEB" "ImpGal" "\xEC" "L" "\xED" "\r"
#ifdef useEEPROMtripStorage
	"RefillSave*1K " "\xEB" "ImpGal" "\xEC" "L" "\xED" "\r"
#endif // useEEPROMtripStorage
#else // useImperialGallon
	"Refill*1K " "\xEB" "USgal" "\xEC" "L" "\xED" "\r"
#ifdef useEEPROMtripStorage
	"RefillSave*1K " "\xEB" "USgal" "\xEC" "L" "\xED" "\r"
#endif // useEEPROMtripStorage
#endif // useImperialGallon
#endif // usePartialRefuel
#ifdef useDebugTerminalLabels
#ifdef useEEPROMtripStorage
	"CurrTripSig\r"
	"TankTripSig\r"
	"CurrVSSpulse\r"
	"CurrVSScycle\r"
	"CurrInjPulse\r"
	"CurrInjCycle\r"
	"CurrEngCycle\r"
	"TankVSSpulse\r"
	"TankVSScycle\r"
	"TankInjPulse\r"
	"TankInjCycle\r"
	"TankEngCycle\r"
#ifdef trackIdleEOCdata
	"CurrIEOCvssPulse\r"
	"CurrIEOCvssCycle\r"
	"CurrIEOCinjPulse\r"
	"CurrIEOCinjCycle\r"
	"CurrIEOCengCycle\r"
	"TankIEOCvssPulse\r"
	"TankIEOCvssCycle\r"
	"TankIEOCinjPulse\r"
	"TankIEOCinjCycle\r"
	"TankIEOCengCycle\r"
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
#endif // useDebugTerminalLabels
};

// 0byyy0 0000 - MPGuino parameter action needed that is critical to measurements of fuel injection, VSS, timeouts, and the like
// 0b0000 0xxx - fractional part of parameter size in bits

static const uint8_t pfDoNothing =				0;
static const uint8_t pfSoftwareInitMPGuino =	pfDoNothing + 32;
static const uint8_t pfHardwareInitMPGuino =	pfSoftwareInitMPGuino + 32;
static const uint8_t pfDoMetricModeConversion =	pfHardwareInitMPGuino + 32;
static const uint8_t pfChangeDisplay =			pfDoMetricModeConversion + 32;
static const uint8_t pfCalculateFuelParams =	pfChangeDisplay + 32;

const uint8_t paramsLength[] PROGMEM = {
	 (pSizeSignature & 0x07) | pfHardwareInitMPGuino							// EEPROM MPGuino signature long word

// display settings

#ifdef useLegacyLCD
	,(pSizeContrast & 0x07) | pfChangeDisplay									// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,(pSizeLCDcolor & 0x07) | pfChangeDisplay									// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,(pSizeMetricMode & 0x07) | pfDoMetricModeConversion						// Display Mode (0 - US Display, 1 - Metric Display)
	,(pSizeAlternateFE & 0x07) | pfSoftwareInitMPGuino							// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useFuelCost
	,(pSizeFuelUnitCost & 0x07)													// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,(pSizeExpansionPin1Mode & 0x07)											// Output Pin 1 mode
	,(pSizeExpansionPin2Mode & 0x07)											// Output Pin 2 mode
#endif // useExpansionPins
#ifdef useCarVoltageOutput
	,(pSizeVoltageOffset & 0x07)												// diode offset from V(alternator)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	,(pSizeSerialDataLogging & 0x07)											// Serial Data Logging Enable
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
	,(pSizeWindowTripFilter & 0x07)												// Window Trip Filter Enable
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,(pSizeFEvsTime & 0x07) | pfSoftwareInitMPGuino								// Period Of FE over Time BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,(pSizeBarLowSpeedCutoff & 0x07) | pfSoftwareInitMPGuino					// FE vs Speed Bargraph lower speed
	,(pSizeBarSpeedQuantumIdx & 0x07) | pfSoftwareInitMPGuino					// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	,(pSizeSysFuelPressure & 0x07) | pfSoftwareInitMPGuino						// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,(pSizeRefFuelPressure & 0x07) | pfCalculateFuelParams						// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,(pSizeInjectorCount & 0x07) | pfCalculateFuelParams						// Fuel Injector Count
	,(pSizeInjectorSize & 0x07) | pfCalculateFuelParams							// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,(pSizeMicroSecondsPerGallon & 0x07) | pfSoftwareInitMPGuino				// Microseconds per US gallon
	,(pSizeInjEdgeTrigger & 0x07) | pfHardwareInitMPGuino						// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	,(pSizeInjectorOpeningTime & 0x07) | pfSoftwareInitMPGuino					// Fuel Injector Opening Delay Time (us)
	,(pSizeInjectorClosingTime & 0x07) | pfSoftwareInitMPGuino					// Fuel Injector Closing Delay Time (us)
	,(pSizeCrankRevPerInj & 0x07) | pfSoftwareInitMPGuino						// Crankshaft Revolutions per Fuel Injector Event
	,(pSizeMinGoodRPM & 0x07) | pfSoftwareInitMPGuino							// Minimum Engine Speed For Engine On (RPM)

// vehicle speed sensor settings

	,(pSizePulsesPerDistance & 0x07) | pfSoftwareInitMPGuino					// VSS Pulses (per mile or per km)
	,(pSizeVSSpause & 0x07) | pfHardwareInitMPGuino								// VSS Pause Debounce Count (ms)
	,(pSizeMinGoodSpeed & 0x07) | pfSoftwareInitMPGuino							// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000

// fuel tank size settings

	,(pSizeTankSize & 0x07) | pfSoftwareInitMPGuino								// Tank Capacity * 1000 (gal or L)
	,(pSizeTankBingoSize & 0x07) | pfSoftwareInitMPGuino						// Bingo Fuel (reserve tank) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection

// Chrysler settings

	,(pSizeMAPsensorFloor & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Floor * 1000 (V)
	,(pSizeMAPsensorCeiling & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Ceiling * 1000 (V)
	,(pSizeMAPsensorRange & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Range (Pa or * 1000 psig)
	,(pSizeMAPsensorOffset & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Offset (Pa or * 1000 psia)
#ifdef useChryslerBaroSensor
	,(pSizeBaroSensorFloor & 0x07) | pfSoftwareInitMPGuino						// Barometric Sensor Floor * 1000 (V)
	,(pSizeBaroSensorCeiling & 0x07) | pfSoftwareInitMPGuino					// Barometric Sensor Ceiling * 1000 (V)
	,(pSizeBaroSensorRange & 0x07) | pfSoftwareInitMPGuino						// Barometric Sensor Range (Pa or * 1000 psig)
	,(pSizeBaroSensorOffset & 0x07) | pfSoftwareInitMPGuino						// Barometric Sensor Offset (Pa or * 1000 psia)
#else // useChryslerBaroSensor
	,(pSizeBarometricPressure & 0x07) | pfSoftwareInitMPGuino					// Reference Barometric Pressure
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	,(pSizeVehicleMass & 0x07)													// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,(pSizeVehicleFrontalArea & 0x07)											// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,(pSizeLocustDensity & 0x07)												// Air density (lb/yd^3 or kg/m^3)
	,(pSizeCoefficientD & 0x07)													// Vehicle C(d) * 1000
	,(pSizeCoefficientV & 0x07)													// Vehicle C(v) * 1000
	,(pSizeCoefficientRR & 0x07)												// Vehicle C(rr) * 1000
	,(pSizeCoastdownSamplePeriod & 0x07)										// Sample Period in seconds
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	,(pSizeDragSpeed & 0x07)													// Drag speed cutoff (MPH or kph) * 1000
	,(pSizeDragDistance & 0x07)													// Drag distance cutoff (miles or km) * 1000
	,(pSizeDragAutoFlag & 0x07)													// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction

// timeout settings

	,(pSizeIdleTimeout & 0x07) | pfSoftwareInitMPGuino							// Engine Idle Timeout (s)
	,(pSizeEOCtimeout & 0x07) | pfSoftwareInitMPGuino							// Engine-Off Coasting Timeout (s)
	,(pSizeButtonTimeout & 0x07) | pfSoftwareInitMPGuino						// Button Press Activity Timeout (s)
	,(pSizeParkTimeout & 0x07) | pfSoftwareInitMPGuino							// Vehicle Parked (engine off, no movement) Timeout (s)
	,(pSizeActivityTimeout & 0x07) | pfSoftwareInitMPGuino						// Activity (engine off, no movement, no button press) Timeout (s)
	,(pSizeWakeupResetCurrentOnEngine & 0x07)									// Enable current trip reset upon wakeup due to engine running
	,(pSizeWakeupResetCurrentOnMove & 0x07)										// Enable current trip reset upon wakeup due to vehicle movement
#ifdef useSavedTrips
	,(pSizeAutoSaveActive & 0x07)												// Autosave Active Trip Data Enable
#endif // useSavedTrips

// miscellaneous settings

	,(pSizeScratchpad & 0x07)													// Scratchpad Memory

// settings inaccessible from the menu

#ifdef usePartialRefuel
	,(pSizeRefuelSize & 0x07) | pfSoftwareInitMPGuino							// Partial Refuel amount * 1000 (gal or L)
#ifdef useEEPROMtripStorage
	,(pSizeRefuelSaveSizeIdx & 0x07)											// Partial Refuel save amount * 1000 (gal or L)
#endif // useEEPROMtripStorage
#endif // usePartialRefuel

#ifdef useEEPROMtripStorage
	,(pSizeCurrTripSignatureIdx & 0x07)											// Current Trip signature byte
	,(pSizeTankTripSignatureIdx & 0x07)											// Tank Trip signature byte

	,(pSizeCurrTripVSSpulseIdx & 0x07)											// Current Trip VSS pulse count storage
	,(pSizeCurrTripVSScycleIdx & 0x07)											// Current Trip VSS cycle accumulator storage
	,(pSizeCurrTripInjPulseIdx & 0x07)											// Current Trip injector pulse count storage
	,(pSizeCurrTripInjCycleIdx & 0x07)											// Current Trip injector open cycle accumulator storage
	,(pSizeCurrTripEngCycleIdx & 0x07)											// Current Trip engine revolution cycle accumulator storage
	,(pSizeTankTripVSSpulseIdx & 0x07)											// Tank Trip VSS pulse count storage
	,(pSizeTankTripVSScycleIdx & 0x07)											// Tank Trip VSS cycle accumulator storage
	,(pSizeTankTripInjPulseIdx & 0x07)											// Tank Trip injector pulse count storage
	,(pSizeTankTripInjCycleIdx & 0x07)											// Tank Trip injector open cycle accumulator storage
	,(pSizeTankTripEngCycleIdx & 0x07)											// Tank Trip engine revolution cycle accumulator storage
#ifdef trackIdleEOCdata
	,(pSizeCurrIEOCvssPulseIdx & 0x07)											// Current Idle/EOC Trip VSS pulse count storage
	,(pSizeCurrIEOCvssCycleIdx & 0x07)											// Current Idle/EOC Trip VSS cycle accumulator storage
	,(pSizeCurrIEOCinjPulseIdx & 0x07)											// Current Idle/EOC Trip injector pulse count storage
	,(pSizeCurrIEOCinjCycleIdx & 0x07)											// Current Idle/EOC Trip injector open cycle accumulator storage
	,(pSizeCurrIEOCengCycleIdx & 0x07)											// Current Idle/EOC Trip engine revolution cycle accumulator storage
	,(pSizeTankIEOCvssPulseIdx & 0x07)											// Tank Idle/EOC Trip VSS pulse count storage
	,(pSizeTankIEOCvssCycleIdx & 0x07)											// Tank Idle/EOC Trip VSS cycle accumulator storage
	,(pSizeTankIEOCinjPulseIdx & 0x07)											// Tank Idle/EOC Trip injector pulse count storage
	,(pSizeTankIEOCinjCycleIdx & 0x07)											// Tank Idle/EOC Trip injector open cycle accumulator storage
	,(pSizeTankIEOCengCycleIdx & 0x07)											// Tank Idle/EOC Trip engine revolution cycle accumulator storage
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
};

/* parameter EEPROM addresses for parameter definitions above */

const uint8_t paramAddrs[] PROGMEM = {
	(uint8_t)(pAddressSignature)					// EEPROM signature long word for MPGuino

// display settings

#ifdef useLegacyLCD
	,(uint8_t)(pAddressContrast)					// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,(uint8_t)(pAddressLCDcolor)					// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,(uint8_t)(pAddressMetricMode)					// Display Mode (0 - US Display, 1 - Metric Display)
	,(uint8_t)(pAddressAlternateFE)					// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useFuelCost
	,(uint8_t)(pAddressFuelUnitCost)				// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,(uint8_t)(pAddressExpansionPin1Mode)			// Output Pin 1 mode
	,(uint8_t)(pAddressExpansionPin2Mode)			// Output Pin 2 mode
#endif // useExpansionPins
#ifdef useCarVoltageOutput
	,(uint8_t)(pAddressVoltageOffset)				// diode offset from V(alternator)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	,(uint8_t)(pAddressSerialDataLogging)			// Serial Data Logging Enable
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
	,(uint8_t)(pAddressWindowTripFilter)			// Window Trip Filter Enable
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,(uint8_t)(pAddressFEvsTime)					// Period Of FE over Time Bar Graph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,(uint8_t)(pAddressBarLowSpeedCutoff)			// FE vs Speed Bargraph lower speed
	,(uint8_t)(pAddressBarSpeedQuantumIdx)			// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	,(uint8_t)(pAddressSysFuelPressure)				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,(uint8_t)(pAddressRefFuelPressure)				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,(uint8_t)(pAddressInjectorCount)				// Fuel Injector Count
	,(uint8_t)(pAddressInjectorSize)				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,(uint8_t)(pAddressMicroSecondsPerGallon)		// Microseconds per US gallon
	,(uint8_t)(pAddressInjEdgeTrigger)				// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	,(uint8_t)(pAddressInjectorOpeningTime)			// Fuel Injector Opening Delay Time (us)
	,(uint8_t)(pAddressInjectorClosingTime)			// Fuel Injector Closing Delay Time (us)
	,(uint8_t)(pAddressCrankRevPerInj)				// Crankshaft Revolutions per Fuel Injector Event
	,(uint8_t)(pAddressMinGoodRPM)					// Minimum Engine Speed For Engine On (RPM)

// vehicle speed sensor settings

	,(uint8_t)(pAddressPulsesPerDistance)			// VSS Pulses (per mile or per km)
	,(uint8_t)(pAddressVSSpause)					// VSS Pause Debounce Count (ms)
	,(uint8_t)(pAddressMinGoodSpeed)				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000

// fuel tank size settings

	,(uint8_t)(pAddressTankSize)					// Tank Capacity * 1000 (gal or L)
	,(uint8_t)(pAddressTankBingoSize)				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection

// Chrysler settings

	,(uint8_t)(pAddressMAPsensorFloor)				// MAP Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressMAPsensorCeiling)			// MAP Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressMAPsensorRange)				// MAP Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressMAPsensorOffset)				// MAP Sensor Offset (Pa or * 1000 psia)
#ifdef useChryslerBaroSensor
	,(uint8_t)(pAddressBaroSensorFloor)				// Barometric Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressBaroSensorCeiling)			// Barometric Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressBaroSensorRange)				// Barometric Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressBaroSensorOffset)			// Barometric Sensor Offset (Pa or * 1000 psia)
#else // useChryslerBaroSensor
	,(uint8_t)(pAddressBarometricPressure)			// Reference Barometric Pressure
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	,(uint8_t)(pAddressVehicleMass)					// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,(uint8_t)(pAddressVehicleFrontalArea)			// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,(uint8_t)(pAddressLocustDensity)				// Air density (lb/yd^3 or kg/m^3)
	,(uint8_t)(pAddressCoefficientD)				// Vehicle C(d) * 1000
	,(uint8_t)(pAddressCoefficientV)				// Vehicle C(v) * 1000
	,(uint8_t)(pAddressCoefficientRR)				// Vehicle C(rr) * 1000
	,(uint8_t)(pAddressCoastdownSamplePeriod)		// Sample Period in seconds
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	,(uint8_t)(pAddressDragSpeed)					// Drag speed cutoff (MPH or kph) * 1000
	,(uint8_t)(pAddressDragDistance)				// Drag distance cutoff (miles or km) * 1000
	,(uint8_t)(pAddressDragAutoFlag)				// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction

// timeout settings

	,(uint8_t)(pAddressIdleTimeout)					// Engine Idle Timeout (s)
	,(uint8_t)(pAddressEOCtimeout)					// Engine-Off Coasting Timeout (s)
	,(uint8_t)(pAddressButtonTimeout)				// Button Press Activity Timeout (s)
	,(uint8_t)(pAddressParkTimeout)					// Vehicle Parked (engine off, no movement) Timeout (s)
	,(uint8_t)(pAddressActivityTimeout)				// Activity (engine off, no movement, no button press) Timeout (s)
	,(uint8_t)(pAddressWakeupResetCurrentOnEngine)	// Enable current trip reset upon wakeup due to engine running
	,(uint8_t)(pAddressWakeupResetCurrentOnMove)	// Enable current trip reset upon wakeup due to vehicle movement
#ifdef useSavedTrips
	,(uint8_t)(pAddressAutoSaveActive)				// Autosave Active Trip Data Enable
#endif // useSavedTrips

// miscellaneous settings

	,(uint8_t)(pAddressScratchpad)					// Scratchpad Memory

// settings inaccessible from the menu

#ifdef usePartialRefuel
	,(uint8_t)(pAddressRefuelSize)					// Partial Refuel amount * 1000 (gal or L)
#ifdef useEEPROMtripStorage
	,(uint8_t)(pAddressRefuelSaveSizeIdx)			// Partial Refuel save amount * 1000 (gal or L)
#endif // useEEPROMtripStorage
#endif // usePartialRefuel
#ifdef useEEPROMtripStorage
	,(uint8_t)(pAddressCurrTripSignatureIdx)		// Current Trip signature byte
	,(uint8_t)(pAddressTankTripSignatureIdx)		// Tank Trip signature byte

	,(uint8_t)(pAddressCurrTripVSSpulseIdx)			// Current Trip VSS pulse count storage
	,(uint8_t)(pAddressCurrTripVSScycleIdx)			// Current Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressCurrTripInjPulseIdx)			// Current Trip injector pulse count storage
	,(uint8_t)(pAddressCurrTripInjCycleIdx)			// Current Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressCurrTripEngCycleIdx)			// Current Trip engine revolution cycle accumulator storage
	,(uint8_t)(pAddressTankTripVSSpulseIdx)			// Tank Trip VSS pulse count storage
	,(uint8_t)(pAddressTankTripVSScycleIdx)			// Tank Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressTankTripInjPulseIdx)			// Tank Trip injector pulse count storage
	,(uint8_t)(pAddressTankTripInjCycleIdx)			// Tank Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressTankTripEngCycleIdx)			// Tank Trip engine revolution cycle accumulator storage
#ifdef trackIdleEOCdata
	,(uint8_t)(pAddressCurrIEOCvssPulseIdx)			// Current Idle/EOC Trip VSS pulse count storage
	,(uint8_t)(pAddressCurrIEOCvssCycleIdx)			// Current Idle/EOC Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressCurrIEOCinjPulseIdx)			// Current Idle/EOC Trip injector pulse count storage
	,(uint8_t)(pAddressCurrIEOCinjCycleIdx)			// Current Idle/EOC Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressCurrIEOCengCycleIdx)			// Current Idle/EOC Trip engine revolution cycle accumulator storage
	,(uint8_t)(pAddressTankIEOCvssPulseIdx)			// Tank Idle/EOC Trip VSS pulse count storage
	,(uint8_t)(pAddressTankIEOCvssCycleIdx)			// Tank Idle/EOC Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressTankIEOCinjPulseIdx)			// Tank Idle/EOC Trip injector pulse count storage
	,(uint8_t)(pAddressTankIEOCinjCycleIdx)			// Tank Idle/EOC Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressTankIEOCengCycleIdx)			// Tank Idle/EOC Trip engine revolution cycle accumulator storage
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
};

/* parameter values for parameter definitions above */

const uint32_t newEEPROMsignature = ((uint32_t)(guinosig) << 16) + ((uint32_t)(eePtrEnd) << 8) + (uint32_t)(eeAdrSettingsEnd);

const uint32_t params[] PROGMEM = {
	 newEEPROMsignature	// EEPROM MPGuino signature long word

// display settings

#ifdef useLegacyLCD
	,55					// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,3					// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,0					// Display Mode (0 - US Display, 1 - Metric Display)
	,0					// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useFuelCost
	,2710259			// Price per unit volume of fuel (this prices is for a liter of diesel at 717 HUF / liter)
#endif // useFuelCost
#ifdef useExpansionPins
	,0					// Output pin 1 mode
	,0					// Output pin 2 mode
#endif // useExpansionPins
#ifdef useCarVoltageOutput
	,700				// diode offset from V(alternator) (via meelis11)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	,1					// Serial Data Logging Enable
#endif // useDataLoggingOutput
#ifdef useWindowTripFilter
	,1					// Window Trip Filter Enable
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,5					// Length Of BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,25000				// FE vs Speed Bargraph lower speed
	,5000				// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	,43500				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,43500				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,6					// Fuel Injector Count
	,246000				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,133262651			// Microseconds per US gallon
	,0					// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	,550				// Fuel Injector Response Delay Time (us)
	,2000				// Fuel Injector Closing Delay Time (us)
	,2					// Crankshaft Revolutions per Fuel Injector Event
	,40					// Minimum Engine Speed For Engine On (RPM)

// vehicle speed sensor settings

	,10000				// VSS Pulses (per mile or per km)
	,0					// VSS Pause Debounce Count (ms)
	,300				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000

// fuel tank size settings

	,18000				// Tank Capacity * 1000 (gal or L)
	,2200				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection

// Chrysler settings

	,500				// MAP Sensor Floor * 1000 (V)
	,4000				// MAP Sensor Ceiling * 1000 (V)
	,14145				// Barometric Sensor Range (Pa or * 1000 psig)
	,551				// MAP Sensor Offset (Pa or * 1000 psia)
#ifdef useChryslerBaroSensor
	,500				// Barometric Sensor Floor * 1000 (V)
	,4000				// Barometric Sensor Ceiling * 1000 (V)
	,14145				// Barometric Sensor Range (Pa or * 1000 psig)
	,551				// Barometric Sensor Offset (Pa or * 1000 psia)
#else // useChryslerBaroSensor
	,14696				// Reference Barometric Pressure
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	,4000				// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,25400				// Vehicle Frontal Area (ft^2 or m^2)
	,2065				// Air density (lb/yd^3 or kg/m^3)
	,346				// C(d)
	,1					// C(v)
	,8					// C(rr)
	,3					// Sample Period in seconds
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	,60000				// Drag speed cutoff (MPH or kph) * 1000
	,250				// Drag distance cutoff (miles or km) * 1000
	,0					// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction

// timeout settings

	,15					// Engine Idle Timeout (s)
	,2					// Engine-Off Coasting Timeout (s)
	,5					// Button Press Activity Timeout (s)
	,5					// Vehicle Parked (engine off, no movement) Timeout (s)
	,120				// Activity (engine off, no movement, no button press) Timeout (s)
	,1					// Enable current trip reset upon wakeup due to engine running
	,0					// Enable current trip reset upon wakeup due to button press
#ifdef useSavedTrips
	,1					// Autosave Active Trip Data Enable
#endif // useSavedTrips

// miscellaneous settings

	,0					// Scratchpad Memory

// settings inaccessible from the menu

#ifdef usePartialRefuel
	,0					// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
};

// end of remarkably long EEPROM stored settings section

// SWEET64 stored parameter conversion list
//
const uint8_t convIdx[] PROGMEM = {
	pPulsesPerDistanceIdx
	,pMinGoodSpeedidx
	,pTankSizeIdx
	,pTankBingoSizeIdx
#ifdef usePartialRefuel
	,pRefuelSizeIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,pDragSpeedIdx
	,pDragDistanceIdx
#endif // useDragRaceFunction
#ifdef useVehicleMass
	,pVehicleMassIdx
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,pVehicleFrontalAreaIdx
	,pLocustDensityIdx
#endif // useCoastDownCalculator
#ifdef useCalculatedFuelFactor
	,pSysFuelPressureIdx
	,pRefFuelPressureIdx
#endif // useCalculatedFuelFactor
#ifdef useChryslerMAPCorrection
	,pMAPsensorRangeIdx
	,pMAPsensorOffsetIdx
#ifdef useChryslerBaroSensor
	,pBaroSensorRangeIdx
	,pBaroSensorOffsetIdx
#else // useChryslerBaroSensor
	,pBarometricPressureIdx
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	,pBarLowSpeedCutoffIdx
	,pBarSpeedQuantumIdx
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,pCostPerQuantity
#endif // useFuelCost
};

const uint8_t convSize = (sizeof(convIdx) / sizeof(uint8_t));

// indexes into SWEET64 conversion factor value table
//
#define nextAllowedValue 0
const uint8_t idxNumerDistance =			nextAllowedValue;
const uint8_t idxDenomDistance =			idxNumerDistance + 1;
const uint8_t idxNumerVolume =				idxDenomDistance + 1;
const uint8_t idxDenomVolume =				idxNumerVolume + 1;
const uint8_t idxCycles0PerSecond =			idxDenomVolume + 1;
const uint8_t idxCycles0PerTick =			idxCycles0PerSecond + 1;		// known as the "N" in the (processor speed)/(N * prescaler) for timer0 fast PWM mode
const uint8_t idxTicksPerSecond =			idxCycles0PerTick + 1;
const uint8_t idxMicroSecondsPerSecond =	idxTicksPerSecond + 1;
const uint8_t idxDecimalPoint =				idxMicroSecondsPerSecond + 1;
const uint8_t idxMetricFE =					idxDecimalPoint + 1;
const uint8_t idxSecondsPerHour =			idxMetricFE + 1;
const uint8_t idxNumber500 =				idxSecondsPerHour + 1;
const uint8_t idxBCDdivisor =				idxNumber500 + 1;
#define nextAllowedValue idxBCDdivisor + 1
#ifdef useIsqrt
const uint8_t idxNumerPressure =			nextAllowedValue;
const uint8_t idxDenomPressure =			idxNumerPressure + 1;
#define nextAllowedValue idxDenomPressure + 1
#endif // useIsqrt
#if defined(useAnalogRead)
const uint8_t idxNumerVoltage =				nextAllowedValue;
const uint8_t idxDenomVoltage =				idxNumerVoltage + 1;
#define nextAllowedValue idxDenomVoltage + 1
#endif // useAnalogRead
#ifdef useCarVoltageOutput
const uint8_t idxResistanceR5 =				nextAllowedValue;		// resistor next to ground
const uint8_t idxResistanceR6 =				idxResistanceR5 + 1;	// resistor next to diode
#define nextAllowedValue idxResistanceR6 + 1
#endif // useCarVoltageOutput
#ifdef useVehicleMass
const uint8_t idxNumerMass =				nextAllowedValue;
const uint8_t idxDenomMass =				idxNumerMass + 1;
#define nextAllowedValue idxDenomMass + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t idxNumerArea =				nextAllowedValue;
const uint8_t idxDenomArea =				idxNumerArea + 1;
const uint8_t idxNumerDensity =				idxDenomArea + 1;
const uint8_t idxDenomDensity =				idxNumerDensity + 1;
#define nextAllowedValue idxDenomDensity + 1
#endif // useCoastDownCalculator
#ifdef useClockDisplay
const uint8_t idxSecondsPerDay =			nextAllowedValue;
#define nextAllowedValue idxSecondsPerDay + 1
#endif // useClockDisplay
#ifdef useIsqrt
const uint8_t idxCorrectionFactor =			nextAllowedValue;
#define nextAllowedValue idxCorrectionFactor + 1
#endif // useIsqrt
#ifdef useImperialGallon
const uint8_t idxNumerImperialGallon =		nextAllowedValue;
const uint8_t idxDenomImperialGallon =		idxNumerImperialGallon + 1;
#define nextAllowedValue idxDenomImperialGallon + 1
#endif // useImperialGallon
#ifdef useDragRaceFunction
const uint8_t idxPowerFactor =				nextAllowedValue;
#define nextAllowedValue idxPowerFactor + 1
#endif // useDragRaceFunction

#define idxOneThousand idxDecimalPoint

const uint8_t idxMaxConstant =				nextAllowedValue;

#ifdef useDebugTerminalLabels
static const char terminalConstIdxNames[] PROGMEM = {
	"idxNumerDistance\r"
	"idxDenomDistance\r"
	"idxNumerVolume\r"
	"idxDenomVolume\r"
	"idxCycles0PerSecond\r"
	"idxCycles0PerTick\r"
	"idxTicksPerSecond\r"
	"idxMicroSecondsPerSecond\r"
	"idxDecimalPoint\r"
	"idxMetricFE\r"
	"idxSecondsPerHour\r"
	"idxNumber500\r"
	"idxBCDdivisor\r"
#ifdef useIsqrt
	"idxNumerPressure\r"
	"idxDenomPressure\r"
#endif // useIsqrt
#if defined(useAnalogRead)
	"idxNumerVoltage\r"
	"idxDenomVoltage\r"
#endif // useAnalogRead
#ifdef useCarVoltageOutput
	"idxResistanceR5\r"
	"idxResistanceR6\r"
#endif // useCarVoltageOutput
#ifdef useVehicleMass
	"idxNumerMass\r"
	"idxDenomMass\r"
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	"idxNumerArea\r"
	"idxDenomArea\r"
	"idxNumerDensity\r"
	"idxDenomDensity\r"
#endif // useCoastDownCalculator
#ifdef useClockDisplay
	"idxSecondsPerDay\r"
#endif // useClockDisplay
#ifdef useIsqrt
	"idxCorrectionFactor\r"
#endif // useIsqrt
#ifdef useImperialGallon
	"idxNumerImperialGallon\r"
	"idxDenomImperialGallon\r"
#endif // useImperialGallon
#ifdef useDragRaceFunction
	"idxPowerFactor\r"
#endif // useDragRaceFunction
};

#endif // useDebugTerminalLabels
const uint8_t convNumerIdx[] PROGMEM = {
	 idxDenomDistance						// pPulsesPerDistanceIdx
	,idxNumerDistance						// pMinGoodSpeedidx
	,idxNumerVolume							// pTankSizeIdx
	,idxNumerVolume							// pTankBingoSizeIdx
#ifdef usePartialRefuel
	,idxNumerVolume							// pRefuelSizeIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,idxNumerDistance						// pDragSpeedIdx
	,idxNumerDistance						// pDragDistanceIdx
#endif // useDragRaceFunction
#ifdef useVehicleMass
	,idxNumerMass							// pVehicleMassIdx
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,idxNumerArea							// pVehicleFrontalAreaIdx
	,idxNumerDensity						// pLocustDensityIdx
#endif // useCoastDownCalculator
#ifdef useCalculatedFuelFactor
	,idxNumerPressure						// pSysFuelPressureIdx
	,idxNumerPressure						// pRefFuelPressureIdx
#endif // useCalculatedFuelFactor
#ifdef useChryslerMAPCorrection
	,idxNumerPressure						// pMAPsensorRangeIdx
	,idxNumerPressure						// pMAPsensorOffsetIdx
#ifdef useChryslerBaroSensor
	,idxNumerPressure						// pBaroSensorRangeIdx
	,idxNumerPressure						// pBaroSensorOffsetIdx
#else // useChryslerBaroSensor
	,idxNumerPressure						// pBarometricPressureIdx
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	,idxNumerDistance						// pBarLowSpeedCutoffIdx
	,idxNumerDistance						// pBarSpeedQuantumIdx
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,idxDenomVolume							// pCostPerQuantity
#endif // useFuelCost
};

const uint32_t convNumbers[] PROGMEM = {
	 1609344ul						// idxNumerDistance - numerator to convert miles to kilometers
	,1000000ul						// idxDenomDistance - denominator to convert miles to kilometers
	,3785411784ul					// idxNumerVolume - numerator to convert US gallons to liters
	,1000000000ul					// idxDenomVolume - denominator to convert US gallons to liters
	,t0CyclesPerSecond				// idxCycles0PerSecond - timer0 clock cycles per second
	,256ul							// idxCycles0PerTick - timer0 clock cycles per timer0 tick
	,t0TicksPerSecond				// idxTicksPerSecond - timer0 clock ticks per second
	,1000000ul						// idxMicroSecondsPerSecond - microseconds per second
	,1000ul							// idxDecimalPoint - decimal point format (the basis for all of those '* 1000' parameters)
	,100000ul						// idxMetricFE - decimal point format * 100 for metric FE (L / 100km)
	,3600ul							// idxSecondsPerHour - seconds per hour
	,500ul							// idxNumber500 - round to nearest whole number, for numeric formatting
	,100000000ul					// idxBCDdivisor - divisor to separate lower 4 BCD bytes from 5th byte
#ifdef useIsqrt
	,68947573ul						// idxNumerPressure - numerator to convert psig to kPa
	,10000000ul						// idxDenomPressure - denominator to convert psig to kPa
#endif // useIsqrt
#if defined(useAnalogRead)
	,1024ul							// idxNumerVoltage - numerator to convert volts DC to ADC steps
	,5000ul							// idxDenomVoltage - denominator to convert volts DC to ADC steps
#endif // useAnalogRead
#ifdef useCarVoltageOutput
	,9600ul							// idxResistanceR5 - resistor next to ground (via meelis11)
	,27000ul						// idxResistanceR6 - resistor next to diode  (via meelis11)
#endif // useCarVoltageOutput
#ifdef useVehicleMass
	,1000000000ul					// idxNumerMass - numerator to convert pounds to kilograms
	,2204622621ul					// idxDenomMass - denominator to convert pounds to kilograms
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,9290304ul						// idxNumerArea - numerator to convert square feet to square meters
	,100000000ul					// idxDenomArea - denominator to convert square feet to square meters
	,100000ul						// idxNumerDensity - numerator to convert SAE density to metric density
	,168555ul						// idxDenomDensity - denominator to convert SAE density to metric density
#endif // useCoastDownCalculator
#ifdef useClockDisplay
	,86400ul						// idxSecondsPerDay - number of seconds in a day
#endif // useClockDisplay
#ifdef useIsqrt
	,4096ul							// idxCorrectionFactor - correction factor seed for square root function
#endif // useIsqrt
#ifdef useImperialGallon
	,454609ul						// idxNumerImperialGallon - numerator to convert Imperial gallons to liters
	,100000ul						// idxDenomImperialGallon - denominator to convert Imperial gallons to liters
#endif // useImperialGallon
#ifdef useDragRaceFunction
	,22840ul						// idxPowerFactor - 22.84, or vehicle speed division factor for accel test power estimation function (228.4/10 for internal calculations)
#endif // useDragRaceFunction
};

#ifdef useSerial0Port
static void serial0::init(void)
{

	uint16_t myubbr0;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useBufferedSerial0Port
	ringBuffer::init(serial0Buffer, serial0Data);

#endif // useBufferedSerial0Port
#ifdef useSerial0PortInput
	ringBuffer::init(serial0InputBuffer, serial0InputData);

#endif // useSerial0PortInput
	// turn on USART0 transmitter
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRUSART0);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRUSART0);
#endif // defined(__AVR_ATmega328P__)

	// calculate initial baudrate setting
	myubbr0 = (F_CPU / 4 / myBaudRate0 - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR0A = (1 << U2X0);

	if (myubbr0 > 4095)
	{

		UCSR0A = 0;
		myubbr0 = (F_CPU / 8 / myBaudRate0 - 1) / 2;

	}

	// set USART0 baud rate
	UBRR0H = (uint8_t)(myubbr0 >> 8);
	UBRR0L = (uint8_t)(myubbr0);

	// set for 8 data bits, no parity, and 1 stop bit
	UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));

#ifdef useSerial0PortInput
	UCSR0B |= ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)); // enable USART0 receive complete interrupt, receiver, and transmitter
#else // useSerial0PortInput
	UCSR0B |= (1 << TXEN0); // enable USART0 transmitter
#endif // useSerial0PortInput

	devSerial0.chrOut = chrOut;
#ifdef useSerial0PortInput
	devSerial0.chrIn = chrIn;
#endif // useSerial0PortInput
	devSerial0.controlFlags |= (odvFlagCRLF);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial0::shutdown(void)
{

	UCSR0B &= ~((1 << UDRIE0) | (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)); // disable USART0 I/O and utilized interrupts
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // defined(__AVR_ATmega328P__)

}

static void serial0::chrOut(uint8_t chr)
{

#ifdef useBufferedSerial0Port
	ringBuffer::push(serial0Buffer, chr);
	UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
#else // useBufferedSerial0Port
	while ((UCSR0A & (1 << UDRE0)) == 0) idleProcess(); // wait until USART0 data buffer is empty

	UCSR0A &= ~(1 << TXC0); // clear transmit complete flag
	UDR0 = chr; //send the data
#endif // useBufferedSerial0Port

}

#ifdef useSerial0PortInput
static uint8_t serial0::chrIn(void)
{

	return ringBuffer::pull(serial0InputBuffer);

}

// triggered once serial hardware receives a character
#if defined(__AVR_ATmega2560__)
ISR( USART0_RX_vect ) // called whenever USART receiver gets a character
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( _VECTOR(18) ) // called whenever USART receiver gets a character
#endif // defined(__AVR_ATmega328P__)
{

	uint8_t errFlags;
	uint8_t chr;
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	errFlags = (UCSR0A & ((1 << FE0) | (1 << DOR0) | (1 << UPE0)));

	chr = UDR0; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial0InputBuffer, chr);

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useSerial0PortInput
#ifdef useBufferedSerial0Port
// triggered once serial hardware is ready to receive another character for transmission
#if defined(__AVR_ATmega2560__)
ISR( USART0_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega328P__)
{

#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // useDebugCPUreading
	if (ringBuffer::isBufferNotEmpty(serial0Buffer)) UDR0 = ringBuffer::pull(serial0Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR0B &= ~(1 << UDRIE0); // otherwise, disable data register empty interrupt

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useBufferedSerial0Port
#endif // useSerial0Port
#ifdef useSerial1Port
static void serial1::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#ifdef useBufferedSerial1Port

	ringBuffer::init(serial1Buffer, serial1Data);
#endif // useBufferedSerial1Port
#ifdef useSerial1PortInput
	ringBuffer::init(serial1InputBuffer, serial1InputData);

#endif // useSerial1PortInput
	// turn on USART1 transmitter
	PRR1 &= ~(1 << PRUSART1);

	// calculate initial baudrate setting
	myubbr1 = (F_CPU / 4 / myBaudRate1 - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR1A = (1 << U2X1);

	if (myubbr1 > 4095)
	{

		UCSR1A = 0;
		myubbr1 = (F_CPU / 8 / myBaudRate1 - 1) / 2;

	}

	// set USART1 baud rate
	UBRR1H = (uint8_t)(myubbr1 >> 8);
	UBRR1L = (uint8_t)(myubbr1);

	// set for 8 data bits, no parity, and 1 stop bit
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));

#ifdef useSerial0PortInput
	UCSR1B |= ((1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); // enable USART1 receive complete interrupt, receiver, and transmitter
#else // useSerial0PortInput
	UCSR1B |= (1 << TXEN1); // enable USART1 transmitter
#endif // useSerial0PortInput

	devSerial1.chrOut = chrOut;
#ifdef useSerial1PortInput
	devSerial1.chrIn = chrIn;
#endif // useSerial1PortInput
	devSerial1.controlFlags |= (odvFlagCRLF);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial1::shutdown(void)
{

	UCSR1B &= ~((1 << UDRIE1) | (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); // disable USART0 I/O and utilized interrupts
	PRR1 |= (1 << PRUSART1); // shut off USART1 for power reduction

}

static void serial1::chrOut(uint8_t chr)
{

#ifdef useBufferedSerial1Port
	ringBuffer::push(serial1Buffer, chr);
	UCSR1B |= (1 << UDRIE1); // enable data register empty interrupt
#else // useBufferedSerial1Port
	while ((UCSR1A & (1 << UDRE1)) == 0) idleProcess(); // wait until USART1 data buffer is empty

	UCSR1A &= ~(1 << TXC1); // clear transmit complete flag
	UDR1 = chr; //send the data
#endif // useBufferedSerial1Port

}

#ifdef useSerial1PortInput
static uint8_t serial1::chrIn(void)
{

	return ringBuffer::pull(serial1InputBuffer);

}

// triggered once serial hardware receives a character
ISR( USART1_RX_vect ) // called whenever USART receiver gets a character
{

	uint8_t errFlags;
	uint8_t chr;
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	errFlags = (UCSR1A & ((1 << FE1) | (1 << DOR1) | (1 << UPE1)));

	chr = UDR1; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial1InputBuffer, chr);

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useSerial1PortInput
#ifdef useBufferedSerial1Port
// triggered once serial hardware is ready to receive another character for transmission
#if defined(__AVR_ATmega32U4__)
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( USART1_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega2560__)
{

#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // useDebugCPUreading
	if (ringBuffer::isBufferNotEmpty(serial1Buffer)) UDR1 = ringBuffer::pull(serial1Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR1B &= ~(1 << UDRIE1); // otherwise, disable data register empty interrupt

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useBufferedSerial1Port
#endif // useSerial1Port
#if defined(__AVR_ATmega32U4__)
/* start of baseline ATmega native USB support */

#endif // defined(__AVR_ATmega32U4__)
#ifdef useUSBserial

// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usbConfiguration;

// the time remaining before we transmit any partially full
// packet, or send a zero length packet.
static volatile uint8_t transmit_flush_timer;
static uint8_t transmit_previous_timeout;

// serial port settings (baud rate, control signals, etc) set
// by the PC.  These are ignored, but kept in RAM.
static uint8_t cdc_line_coding[7] = {0x00, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x08};
static uint8_t cdcLineRTSDTR;

// constants corresponding to the various serial parameters
static const uint8_t USB_SERIAL_DTR =			0x01;
static const uint8_t USB_SERIAL_RTS =			0x02;
static const uint8_t USB_SERIAL_1_STOP =		0;
static const uint8_t USB_SERIAL_1_5_STOP =		1;
static const uint8_t USB_SERIAL_2_STOP =		2;
static const uint8_t USB_SERIAL_PARITY_NONE =	0;
static const uint8_t USB_SERIAL_PARITY_ODD =	1;
static const uint8_t USB_SERIAL_PARITY_EVEN =	2;
static const uint8_t USB_SERIAL_PARITY_MARK =	3;
static const uint8_t USB_SERIAL_PARITY_SPACE =	4;
static const uint8_t USB_SERIAL_DCD =			0x01;
static const uint8_t USB_SERIAL_DSR =			0x02;
static const uint8_t USB_SERIAL_BREAK =			0x04;
static const uint8_t USB_SERIAL_RI =			0x08;
static const uint8_t USB_SERIAL_FRAME_ERR =		0x10;
static const uint8_t USB_SERIAL_PARITY_ERR =	0x20;
static const uint8_t USB_SERIAL_OVERRUN_ERR =	0x40;

static const uint8_t EP_TYPE_CONTROL =			0x00;
static const uint8_t EP_TYPE_BULK_IN =			0x81;
static const uint8_t EP_TYPE_BULK_OUT =			0x80;
static const uint8_t EP_TYPE_INTERRUPT_IN =		0xC1;
static const uint8_t EP_TYPE_INTERRUPT_OUT =	0xC0;
static const uint8_t EP_TYPE_ISOCHRONOUS_IN =	0x41;
static const uint8_t EP_TYPE_ISOCHRONOUS_OUT =	0x40;
static const uint8_t EP_SINGLE_BUFFER =			0x02;
static const uint8_t EP_DOUBLE_BUFFER =			0x06;
#define EP_SIZE(s)	((s) == 64 ? 0x30 :	\
			((s) == 32 ? 0x20 :	\
			((s) == 16 ? 0x10 :	\
			             0x00)))

#define MAX_ENDPOINT		4

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

//#define HW_CONFIG() (UHWCON = 0x01)

//#ifdef M1
//	#define PLL_CONFIG() (PLLCSR = 0x02) // fixed to 8MHz clock
//#else
//	#define PLL_CONFIG() (PLLCSR = 0x12) // 0001 0010 For a 16MHz clock
//#endif

//#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
//#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))

// standard control endpoint request types
static const uint8_t GET_STATUS =					0x00;
static const uint8_t CLEAR_FEATURE =				0x01;
static const uint8_t SET_FEATURE =					0x03;
static const uint8_t SET_ADDRESS =					0x05;
static const uint8_t GET_DESCRIPTOR =				0x06;
static const uint8_t GET_CONFIGURATION =			0x08;
static const uint8_t SET_CONFIGURATION =			0x09;
static const uint8_t GET_INTERFACE =				0x0A;
static const uint8_t SET_INTERFACE =				0x0B;

// HID (human interface device);
static const uint8_t HID_GET_REPORT =				0x01;
static const uint8_t HID_GET_PROTOCOL =				0x03;
static const uint8_t HID_SET_REPORT =				0x09;
static const uint8_t HID_SET_IDLE =					0x0A;
static const uint8_t HID_SET_PROTOCOL =				0x0B;

// CDC (communication class device);
static const uint8_t CDC_SET_LINE_CODING =			0x20;
static const uint8_t CDC_GET_LINE_CODING =			0x21;
static const uint8_t CDC_SET_CONTROL_LINE_STATE =	0x22;

static void usbDevice::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	PRR1 &= ~(1 << PRUSB); // turn on USB hardware

	UHWCON = (1 << UVREGE); // enable USB pad regulator

	USBCON = ((1 << USBE) | (1 << FRZCLK)); // enable USB controller, disable clock inputs

	PLLCSR = ((1 << PINDIV) | (1 << PLLE)); // set PLL input prescaler for 16 MHz clock source, and enable PLL

	while (!(PLLCSR & (1 << PLOCK))); // wait for PLL lock

	USBCON = ((1 << USBE) | (1 << OTGPADE)); // keep USB controller enabled, enable clock inputs, enable VBUS pad

	UDCON = 0; // select USB high speed mode

	usbConfiguration = 0;
	cdcLineRTSDTR = 0;

	UDIEN = ((1 << EORSTE) | (1 << SOFE)); // enable End-Of-Reset, Start-Of-Frame interrupts

	ringBuffer::init(USBoutputBuffer, USBoutputData);
	ringBuffer::init(USBinputBuffer, USBinputData);

	devUSB.chrOut = chrOut;
	devUSB.chrIn = chrIn;

	SREG = oldSREG; // restore interrupt flag status

}

static void usbDevice::shutdown(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	usbConfiguration = 0;
	cdcLineRTSDTR = 0;

	UDIEN = 0; // disable all USB interrupts

	USBCON = ((1 << USBE) | (1 << FRZCLK)); // enable USB controller, disable clock inputs
	PLLCSR = (1 << PINDIV);  // disable PLL

	UHWCON = 0; // disable USB pad regulator

	PRR1 |= (1 << PRUSB); // turn off USB hardware to save power

	SREG = oldSREG; // restore interrupt flag status

}

static void usbDevice::chrOut(uint8_t chr)
{

	ringBuffer::push(USBoutputBuffer, chr);

}

static uint8_t usbDevice::chrIn(void)
{

	return ringBuffer::pull(USBinputBuffer);

}

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct USBstringDescriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};

#define USB_MANUFACTURER L"GPL"
#define USB_PRODUCT L"MPGuino"
#define USB_SERIAL_NUMBER L"195"

static const uint16_t USBvendorID = 0x27CD;
static const uint16_t USBproductID = 0x8010;

static const uint8_t PROGMEM USBdeviceDescriptor[] = {
	 18						// bLength
	,1						// bDescriptorType
	,0x00					// bcdUSB
	,0x02
	,2						// bDeviceClass
	,0						// bDeviceSubClass
	,0						// bDeviceProtocol
	,16						// bMaxPacketSize0
	,(USBvendorID & 0xFF)	// idVendor
	,(USBvendorID >> 8)
	,(USBproductID & 0xFF)	// idProduct
	,(USBproductID >> 8)
	,0x00					// bcdDevice
	,0x01
	,1						// iManufacturer
	,2						// iProduct
	,3						// iSerialNumber
	,1						// bNumConfigurations
};

#define CONFIG1_DESC_SIZE (9+9+5+5+4+5+7+9+7+7)
static const uint8_t PROGMEM USBconfigurationDescriptor[CONFIG1_DESC_SIZE] = {
// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	 9 							// bLength;
	,2							// bDescriptorType;
	,(CONFIG1_DESC_SIZE & 0xFF)	// wTotalLength
	,(CONFIG1_DESC_SIZE >> 8)
	,2							// bNumInterfaces
	,1							// bConfigurationValue
	,0							// iConfiguration
	,0xC0						// bmAttributes
	,50							// bMaxPower

// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	,9							// bLength
	,4							// bDescriptorType
	,0							// bInterfaceNumber
	,0							// bAlternateSetting
	,1							// bNumEndpoints
	,0x02						// bInterfaceClass
	,0x02						// bInterfaceSubClass
	,0x01						// bInterfaceProtocol
	,0							// iInterface

// CDC Header Functional Descriptor, CDC Spec 5.2.3.1, Table 26
	,5							// bFunctionLength
	,0x24						// bDescriptorType
	,0x00						// bDescriptorSubtype
	,0x10						// bcdCDC
	,0x01

// Call Management Functional Descriptor, CDC Spec 5.2.3.2, Table 27
	,5							// bFunctionLength
	,0x24						// bDescriptorType
	,0x01						// bDescriptorSubtype
	,0x01						// bmCapabilities
	,1							// bDataInterface

// Abstract Control Management Functional Descriptor, CDC Spec 5.2.3.3, Table 28
	,4							// bFunctionLength
	,0x24						// bDescriptorType
	,0x02						// bDescriptorSubtype
	,0x06						// bmCapabilities

// Union Functional Descriptor, CDC Spec 5.2.3.8, Table 33
	,5							// bFunctionLength
	,0x24						// bDescriptorType
	,0x06						// bDescriptorSubtype
	,0							// bMasterInterface
	,1							// bSlaveInterface0

// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	,7							// bLength
	,5							// bDescriptorType
	,CDC_ACM_ENDPOINT | 0x80	// bEndpointAddress
	,0x03						// bmAttributes (0x03=intr)
	,CDC_ACM_SIZE				// wMaxPacketSize
	,0
	,64							// bInterval

// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	,9							// bLength
	,4							// bDescriptorType
	,1							// bInterfaceNumber
	,0							// bAlternateSetting
	,2							// bNumEndpoints
	,0x0A						// bInterfaceClass
	,0x00						// bInterfaceSubClass
	,0x00						// bInterfaceProtocol
	,0							// iInterface

// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	,7							// bLength
	,5							// bDescriptorType
	,CDC_RX_ENDPOINT			// bEndpointAddress
	,0x02						// bmAttributes (0x02=bulk)
	,CDC_RX_SIZE				// wMaxPacketSize
	,0
	,0							// bInterval

// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	,7							// bLength
	,5							// bDescriptorType
	,CDC_TX_ENDPOINT | 0x80		// bEndpointAddress
	,0x02						// bmAttributes (0x02=bulk)
	,CDC_TX_SIZE				// wMaxPacketSize
	,0
	,0							// bInterval
};

// String descriptors
// All have a common format:
//    [byte] string_length
//   ,[byte] data_type (3 = string)
//   ,UTF-16 encoded string
//
static const struct USBstringDescriptor PROGMEM USBstrLanguage = {
	 4
	,3
	,{0x0409}
};

static const struct USBstringDescriptor PROGMEM USBstrManufacturer = {
	 sizeof(USB_MANUFACTURER)
	,3
	,USB_MANUFACTURER
};

static const struct USBstringDescriptor PROGMEM USBstrProductName = {
	 sizeof(USB_PRODUCT)
	,3
	,USB_PRODUCT
};

static const struct USBstringDescriptor PROGMEM USBstrSerialNumber = {
	 sizeof(USB_SERIAL_NUMBER)
	,3
	,USB_SERIAL_NUMBER
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct endpointDescriptorList {
	uint16_t wValue;
	uint16_t wIndex;
	const uint8_t * addr;
	uint8_t length;
} PROGMEM thisEndpointDescriptorList[] = {
	 {0x0100, 0x0000, USBdeviceDescriptor, sizeof(USBdeviceDescriptor)}
	,{0x0200, 0x0000, USBconfigurationDescriptor, sizeof(USBconfigurationDescriptor)}
	,{0x0300, 0x0000, (const uint8_t *)&USBstrLanguage, 4}
	,{0x0301, 0x0409, (const uint8_t *)&USBstrManufacturer, sizeof(USB_MANUFACTURER)}
	,{0x0302, 0x0409, (const uint8_t *)&USBstrProductName, sizeof(USB_PRODUCT)}
	,{0x0303, 0x0409, (const uint8_t *)&USBstrSerialNumber, sizeof(USB_SERIAL_NUMBER)}
};

#define NUM_DESC_LIST (sizeof(thisEndpointDescriptorList)/sizeof(struct endpointDescriptorList))

// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR( USB_COM_vect )
{

	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t descriptorValue;
	const uint8_t * listPtr;
	const uint8_t * cfg;
	const uint8_t * descriptorPtr;
	uint8_t endpointFlags;
	uint8_t i;
	uint8_t n;
	uint8_t len;
	uint8_t en;
	uint8_t * p;
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t	descriptorLength;

	UENUM = 0;
	endpointFlags = UEINTX;

	if (endpointFlags & (1 << RXSTPI))
	{

		bmRequestType = UEDATX;
		bRequest = UEDATX;

		wValue = UEDATX;
		wValue |= (UEDATX << 8);

		wIndex = UEDATX;
		wIndex |= (UEDATX << 8);

		wLength = UEDATX;
		wLength |= (UEDATX << 8);

		UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));

		if (bRequest == GET_DESCRIPTOR)
		{

			listPtr = (const uint8_t *)(thisEndpointDescriptorList);

			for (i=0; ; i++)
			{

				if (i >= NUM_DESC_LIST)
				{

					UECONX = ((1 << STALLRQ) | (1 << EPEN));  //stall
					return;

				}

				descriptorValue = pgm_read_word(listPtr);
				if (descriptorValue != wValue)
				{

					listPtr += sizeof(struct endpointDescriptorList);
					continue;

				}

				listPtr += 2;
				descriptorValue = pgm_read_word(listPtr);

				if (descriptorValue != wIndex)
				{

					listPtr += sizeof(struct endpointDescriptorList) - 2;
					continue;

				}

				listPtr += 2;
				descriptorPtr = (const uint8_t *)pgm_read_word(listPtr);

				listPtr += 2;
				descriptorLength = pgm_read_byte(listPtr);

				break;

			}

			len = (wLength < 256) ? wLength : 255;
			if (len > descriptorLength) len = descriptorLength;

			do
			{

				// wait for host ready for IN packet
				do
				{

					i = UEINTX;

				} while (!(i & ((1 << TXINI)|(1 << RXOUTI))));

				if (i & (1 << RXOUTI)) return;	// abort

				// send IN packet
				n = (len < ENDPOINT0_SIZE) ? len : ENDPOINT0_SIZE;
				for (i = n; i; i--) UEDATX = pgm_read_byte(descriptorPtr++);

				len -= n;
				UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission

			} while ((len) || (n == ENDPOINT0_SIZE));

			return;

		}

		if (bRequest == SET_ADDRESS)
		{

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission

			UDADDR = wValue | (1 << ADDEN);
			return;

		}

		if (bRequest == SET_CONFIGURATION && bmRequestType == 0)
		{

			usbConfiguration = wValue;
			cdcLineRTSDTR = 0;
			transmit_flush_timer = 0;

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission

			cfg = endpoint_config_table;

			for (i = 1; i < 5; i++)
			{

				UENUM = i;
				en = pgm_read_byte(cfg++);
				UECONX = en;

				if (en)
				{

					UECFG0X = pgm_read_byte(cfg++);
					UECFG1X = pgm_read_byte(cfg++);

				}

			}

			UERST = 0x1E;
			UERST = 0;
			return;

		}

		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80)
		{

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission
			UEDATX = usbConfiguration;
			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == CDC_GET_LINE_CODING && bmRequestType == 0xA1)
		{

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission

			p = cdc_line_coding;
			for (i = 0; i < 7; i++) UEDATX = *p++;

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == CDC_SET_LINE_CODING && bmRequestType == 0x21)
		{

		while (!(UEINTX & (1 << RXOUTI)));

			p = cdc_line_coding;

			for (i=0; i<7; i++) *p++ = UEDATX;

			UEINTX = ~(1 << RXOUTI);
			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == CDC_SET_CONTROL_LINE_STATE && bmRequestType == 0x21)
		{

			cdcLineRTSDTR = wValue;

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == GET_STATUS)
		{

			while (!(UEINTX & (1<<TXINI))); // wait for hardware to finish transmission
			i = 0;

#ifdef supportUSBendpointHalt
			if (bmRequestType == 0x82)
			{

				UENUM = wIndex;
				if (UECONX & (1 << STALLRQ)) i = 1;
				UENUM = 0;

			}

#endif // supportUSBendpointHalt
			UEDATX = i;
			UEDATX = 0;
			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

#ifdef supportUSBendpointHalt
		if (((bRequest == CLEAR_FEATURE) || (bRequest == SET_FEATURE)) && (bmRequestType == 0x02) && (wValue == 0))
		{

			i = wIndex & 0x7F;
			if ((i >= 1) && (i <= MAX_ENDPOINT))
			{

				UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
				UENUM = i;

				if (bRequest == SET_FEATURE)
				{

					UECONX = (1 << STALLRQ)|(1 << EPEN);

				}
				else
				{

					UECONX = ((1 << STALLRQC) | (1 << RSTDT) | (1 << EPEN));
					UERST = (1 << i);
					UERST = 0;

				}

				return;

			}

		}

#endif // supportUSBendpointHalt
	}

	UECONX = ((1 << STALLRQ) | (1 << EPEN));	// stall

}


// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{

	uint8_t intbits;
	uint8_t t;

	intbits = UDINT;
	UDINT = 0;

	if (intbits & (1 << EORSTI))
	{

		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1 << RXSTPE);
		usb_configuration = 0;
		cdc_line_rtsdtr = 0;

	}

	if (intbits & (1 << SOFI))
	{

		if (usb_configuration)
		{

			t = transmit_flush_timer;
			if (t)
			{

				transmit_flush_timer = --t;

				if (!t)
				{

					UENUM = CDC_TX_ENDPOINT;
					UEINTX = 0x3A;

				}

			}

		}

	}

}

#endif // useUSBserial
#ifdef useTWIsupport
const uint8_t TWI_READ =				0;
const uint8_t TWI_WRITE =				1;

const uint8_t TWI_REPEAT_START =		0;
const uint8_t TWI_STOP =				1;

const unsigned int twiFrequency = 100L;
const uint8_t twiDataBufferSize = 16;

static uint8_t twiDataBuffer[twiDataBufferSize];

volatile uint8_t twiStatusFlags;
volatile uint8_t twiSlaveAddress;
volatile uint8_t twiErrorCode;
volatile uint8_t twiDataBufferIdx;
volatile uint8_t twiDataBufferLen;

const uint8_t twiBlockMainProgram =		0b10000000;
const uint8_t twiOpen =					0b01000000;
const uint8_t twiFinished =				0b00100000;
const uint8_t twiClose =				0b00010000;
const uint8_t twiRemainOpen =			0b00001000;
const uint8_t twiErrorFlag =			0b00000100;

const uint8_t twiOpenMain =				(twiOpen | twiBlockMainProgram);

ISR( TWI_vect )
{

	uint8_t twiStatus;
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	twiStatus = TW_STATUS;

	switch (twiStatus)
	{

		case TW_START:     // sent start condition
		case TW_REP_START: // sent repeated start condition
			TWDR = twiSlaveAddress; // copy device address and r/w bit to output register
			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK
			break;

		case TW_MT_SLA_ACK:  // slave receiver ACKed address
		case TW_MT_DATA_ACK: // slave receiver ACKed data
			if(twiDataBufferIdx < twiDataBufferLen) // if there is data to send, send it
			{

				TWDR = twiDataBuffer[(uint16_t)(twiDataBufferIdx++)]; // copy data to output register
				TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK

			}
			else twiStatusFlags |= (twiFinished);

			break;

		case TW_MR_DATA_NACK: // data received, nack sent
			twiDataBuffer[(uint16_t)(twiDataBufferIdx++)] = TWDR; // put final byte into buffer
			twiStatusFlags |= (twiFinished);
			break;

		case TW_MR_DATA_ACK: // data received, ACK sent
			twiDataBuffer[(unsigned int)(twiDataBufferIdx++)] = TWDR; // put byte into buffer
		case TW_MR_SLA_ACK:  // address sent, ACK received
			if(twiDataBufferIdx < twiDataBufferLen) TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK if more bytes are expected
			else TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE)); // otherwise, send NEGATIVE ACK
			break;

		case TW_MR_SLA_NACK: // address sent, NACK received
			twiStatusFlags |= (twiFinished | twiClose);
			break;

		case TW_MT_ARB_LOST: // lost bus arbitration (also handles TW_MR_ARB_LOST)
			twiErrorCode = TW_MT_ARB_LOST;
			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK
			twiStatusFlags &= ~(twiOpen); // set TWI state to ready
			break;

		case TW_MT_SLA_NACK:  // address sent, NACK received
		case TW_MT_DATA_NACK: // data sent, NACK received
		case TW_BUS_ERROR: // bus error, illegal stop/start
			twiStatusFlags |= (twiErrorFlag | twiFinished | twiClose); // set error condition
			twiErrorCode = twiStatus;
			break;

		case TW_NO_INFO: // no state information
		default:
			break;

	}

	if (twiStatusFlags & twiFinished)
	{

		twiStatusFlags &= ~(twiOpen | twiFinished); // set TWI state to ready

		if (twiStatusFlags & twiClose)
		{

			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTO)); // send STOP condition
			while (TWCR & (1 << TWSTO)); // wait for STOP condition to be executed on the TWI bus

			twiStatusFlags &= ~(twiRemainOpen);

		}
		else
		{

			twiStatusFlags |= (twiRemainOpen);	// keep the TWI channel open
			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTA));

		}

	}

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useTWIsupport
volatile uint8_t lastPINxState;

volatile uint8_t VSScount; // for VSS debouncing
volatile uint8_t VSSpause; // for VSS debouncing

volatile uint8_t thisButtonState;
volatile uint8_t lastButtonState;
#ifdef useTestAnalogButtonIdx
volatile uint8_t thisButtonIdx;
#endif // useTestAnalogButtonIdx
volatile uint8_t buttonPress;

volatile uint8_t mainLoopHeartBeat;

volatile unsigned long timer0_overflow_count;

volatile unsigned int watchdogInjectorCount;
volatile unsigned int watchdogVSSCount;
volatile unsigned int timer0DelayCount;
volatile unsigned int displayPauseCount;
#ifdef useLegacyButtons
volatile unsigned int buttonDebounceCount;
#endif // useLegacyButtons

// these flags specifically tell the system timer0 to do something
// main program sets flag, system timer0 acknowledges by clearing flag
volatile uint8_t timer0Command;

const uint8_t t0cResetTimer =			0b10000000;
const uint8_t t0cDoDelay =				0b01000000;
const uint8_t t0cDisplayDelay =			0b00100000;
const uint8_t t0cProcessButton =		0b00010000;
#ifdef useBarFuelEconVsTime
const uint8_t t0cResetFEvTime =			0b00001000;
#endif // useBarFuelEconVsTime
#ifdef useTWIbuttons
const uint8_t t0cEnableTWIsample =		0b00000001;
#endif // useTWIbuttons

// these flags specifically tell the main program to do something
// system timer0 sets flag, main program acknowledges by clearing flag
volatile uint8_t timer0Status;

const uint8_t t0sUpdateDisplay =		0b10000000;
const uint8_t t0sShowCursor =			0b01000000;
const uint8_t t0sReadButton =			0b00100000;
const uint8_t t0sTakeSample =			0b00010000;
#if useDataLoggingOutput || useJSONoutput
const uint8_t t0sOutputLogging =		0b00001000;
#endif // useDataLoggingOutput || useJSONoutput
#ifdef useDragRaceFunction
const uint8_t t0sAccelTestFlag =		0b00000100;
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
const uint8_t t0sCoastdownTestFlag =	0b00000010;
#endif // useCoastDownCalculator
#ifdef useJSONoutput
const uint8_t t0sOutputJSON =			0b00000001;
#endif // useJSONoutput

// these status flags inform the main program about MPGuino awake state
volatile uint8_t awakeFlags;

const uint8_t aAwakeOnInjector =		0b10000000;
const uint8_t aAwakeOnVSS =				0b01000000;
const uint8_t aAwakeOnButton =			0b00100000;
const uint8_t aAwakeEngineRunning =		0b00010000;
const uint8_t aAwakeVehicleMoving =		0b00001000;

const uint8_t aAwake =					(aAwakeOnInjector | aAwakeOnVSS | aAwakeOnButton);
const uint8_t aAwakeOnVehicle =			(aAwakeOnInjector | aAwakeOnVSS);

// these status flags inform the main program about MPGuino sensor activity
volatile uint8_t activityFlags;
volatile uint8_t activityChangeFlags;

const uint8_t afEngineOffFlag =			0b10000000;
const uint8_t afVehicleStoppedFlag =	0b01000000;
const uint8_t afButtonFlag =			0b00100000;
const uint8_t afParkFlag =				0b00010000;
const uint8_t afActivityTimeoutFlag =	0b00001000;
const uint8_t afSwapFEwithFCR =			0b00000100;

const uint8_t afValidFlags =			(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag | afButtonFlag | afActivityTimeoutFlag);
const uint8_t afInputCheckFlags =		(afEngineOffFlag | afVehicleStoppedFlag | afButtonFlag);
const uint8_t afActivityCheckFlags =	(afEngineOffFlag | afVehicleStoppedFlag | afButtonFlag | afParkFlag);
const uint8_t afParkCheckFlags =		(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag);
const uint8_t afNotParkedFlags =		(afEngineOffFlag | afVehicleStoppedFlag);

// these status flags communicate instantaneous vehicle status between the sensor interrupts and the system timer0 interrupt
volatile uint8_t dirty;

const uint8_t dGoodEngineRotationOpen =		0b10000000;
const uint8_t dGoodEngineRotationClose =	0b01000000;
const uint8_t dInjectorReadInProgress =		0b00100000;
const uint8_t dGoodInjectorWidth =			0b00010000;
const uint8_t dGoodInjectorRead =			0b00001000;
const uint8_t dGoodVSSsignal =				0b00000100;
const uint8_t dGoodVSSRead =				0b00000010;
#ifdef useChryslerMAPCorrection
const uint8_t dSampleADC =					0b00000001;
#endif // useChryslerMAPCorrection

const uint8_t dGoodEngineRotation =		(dGoodEngineRotationOpen | dGoodEngineRotationClose);
const uint8_t dGoodEngineRun =			(dGoodEngineRotationOpen | dGoodEngineRotationClose | dInjectorReadInProgress | dGoodInjectorWidth | dGoodInjectorRead);
const uint8_t dGoodVehicleMotion =		(dGoodVSSsignal | dGoodVSSRead);
const uint8_t dGoodVehicleDrive =		(dGoodEngineRun | dGoodVehicleMotion);

#ifdef useDragRaceFunction
volatile uint8_t accelerationFlags;

const uint8_t accelTestActive =				0b10000000;
const uint8_t accelTestTriggered =			0b01000000;
const uint8_t accelTestFullSpeed =			0b00100000;
const uint8_t accelTestHalfSpeed =			0b00010000;
const uint8_t accelTestDistance =			0b00001000;
const uint8_t accelTestCancelled =			0b00000100;
const uint8_t accelTestFinished =			0b00000010;

const uint8_t accelTestMeasurementFlags =	accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance;
const uint8_t accelTestInProgress =			accelTestActive | accelTestTriggered;
const uint8_t accelTestCompleteFlags =		accelTestCancelled | accelTestFinished;
const uint8_t accelTestClearFlags =			accelTestInProgress | accelTestMeasurementFlags | accelTestCompleteFlags;

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
volatile uint8_t coastdownFlags;
volatile uint8_t coastdownState;

const uint8_t cdtActive =				0b10000000;
const uint8_t cdtTriggered =			0b01000000;
const uint8_t cdtCancelled =			0b00100000;
const uint8_t cdtFinished =				0b00010000;
const uint8_t cdSignalStateChange =		0b00001000;
const uint8_t cdTakeSample =			0b00000100;
const uint8_t cdSampleTaken =			0b00000010;

const uint8_t cdtTestInProgress =		cdtActive | cdtTriggered;
const uint8_t cdtTestClearFlags =		cdtTestInProgress | cdtCancelled | cdtFinished | cdSignalStateChange | cdTakeSample | cdSampleTaken;

#endif // useCoastDownCalculator
#if defined(useAnalogRead)
volatile uint8_t analogCommand;

const uint8_t acSampleGround =			0b10000000;
const uint8_t acSampleChannel0 =		0b00100000;
const uint8_t acSampleChannel1 =		0b00010000;
const uint8_t acSampleChannel2 =		0b00001000;
const uint8_t acSampleChannel3 =		0b00000100;
const uint8_t acSampleChannel4 =		0b00000010;
const uint8_t acSampleChannel5 =		0b00000001;

const uint8_t acSampleChannelActive =	0b00111111;
#ifdef useChryslerMAPCorrection
#ifdef useChryslerBaroSensor
#ifdef useAnalogButtons
const uint8_t acSampleChannelInit =		0b00000111;			// MAP sensor, baro sensor, and button input do not get scanned normally
#else // useAnalogButtons
const uint8_t acSampleChannelInit =		0b00001111;			// MAP sensor and baro sensor do not get scanned normally
#endif // useAnalogButtons
#else // useChryslerBaroSensor
#ifdef useAnalogButtons
const uint8_t acSampleChannelInit =		0b00001111;			// MAP sensor and button input do not get scanned normally
#else // useAnalogButtons
const uint8_t acSampleChannelInit =		0b00011111;			// MAP sensor does not get scanned normally
#endif // useAnalogButtons
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
const uint8_t acSampleChannelInit =		0b00011111;			// button input does not get scanned normally
#else // useAnalogButtons
const uint8_t acSampleChannelInit =		0b00111111;			// used to start scanning of all non-critical analog channels
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection

#ifdef useChryslerMAPCorrection
const uint8_t acSampleMAPchannel =		acSampleChannel0;
#ifdef useChryslerBaroSensor
const uint8_t acSampleChrysler =		acSampleChannel0 | acSampleChannel1;
const uint8_t acSampleBaroChannel =		acSampleChannel1;
#ifdef useAnalogButtons
const uint8_t acSampleButtonChannel =	acSampleChannel2;
#endif // useAnalogButtons
#else // useChryslerBaroSensor
const uint8_t acSampleChrysler =		acSampleChannel0;
#ifdef useAnalogButtons
const uint8_t acSampleButtonChannel =	acSampleChannel1;
#endif // useAnalogButtons
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
const uint8_t acSampleButtonChannel =	acSampleChannel0;
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection

volatile uint8_t analogStatus;

const uint8_t asHardwareReady =			0b10000000;
const uint8_t asReadChannel0 =			0b00100000;
const uint8_t asReadChannel1 =			0b00010000;
const uint8_t asReadChannel2 =			0b00001000;
const uint8_t asReadChannel3 =			0b00000100;
const uint8_t asReadChannel4 =			0b00000010;
const uint8_t asReadChannel5 =			0b00000001;
const uint8_t asReadChannelMask =		0b00111111;

#ifdef useChryslerMAPCorrection
const uint8_t asReadMAPchannel =		asReadChannel0;
#ifdef useChryslerBaroSensor
const uint8_t asReadBaroChannel =		asReadChannel1;
#ifdef useAnalogButtons
const uint8_t asReadButtonChannel =		asReadChannel2;
#endif // useAnalogButtons
#else // useChryslerBaroSensor
#ifdef useAnalogButtons
const uint8_t asReadButtonChannel =		asReadChannel1;
#endif // useAnalogButtons
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
const uint8_t asReadButtonChannel =		asReadChannel0;
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection

#ifdef useAnalogButtons
volatile uint8_t analogButton;

#endif // useAnalogButtons
volatile uint8_t analogValueIdx;
volatile uint8_t analogBitmask;

#define nextAllowedValue 0
const uint8_t analog0Idx =					nextAllowedValue;	// highest priority analog channel
const uint8_t analog1Idx =					analog0Idx + 1;
const uint8_t analog2Idx =					analog1Idx + 1;
const uint8_t analog3Idx =					analog2Idx + 1;
const uint8_t analog4Idx =					analog3Idx + 1;
const uint8_t analog5Idx =					analog4Idx + 1;
#define nextAllowedValue analog5Idx + 1

const uint8_t dfMaxValAnalogCount =			nextAllowedValue;

const uint8_t analogGroundIdx =				nextAllowedValue;
#define nextAllowedValue analogGroundIdx + 1

const uint8_t dfMaxAnalogCount =			nextAllowedValue;

#ifdef useChryslerMAPCorrection
const uint8_t analogMAPchannelIdx =			analog0Idx;
#ifdef useChryslerBaroSensor
const uint8_t analogBaroChannelIdx =		analog1Idx;
#ifdef useAnalogButtons
const uint8_t analogButtonChannelIdx =		analog2Idx;
#endif // useAnalogButtons
#else // useChryslerBaroSensor
#ifdef useAnalogButtons
const uint8_t analogButtonChannelIdx =		analog1Idx;
#endif // useAnalogButtons
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
const uint8_t analogButtonChannelIdx =		analog0Idx;
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection
#ifdef useCarVoltageOutput
#if useAnalogButtons || useChryslerMAPCorrection
const uint8_t analogAlternatorChannelIdx =	analog2Idx;
#else // useAnalogButtons || useChryslerMAPCorrection
const uint8_t analogAlternatorChannelIdx =	analog1Idx;
#endif // useAnalogButtons || useChryslerMAPCorrection
#endif useCarVoltageOutput

volatile unsigned int analogValue[(unsigned int)(dfMaxValAnalogCount)];

const uint8_t analogChannelValue[(unsigned int)(dfMaxAnalogCount)] PROGMEM = { // points to the next channel to be read
#if defined(__AVR_ATmega32U4__)
#ifdef useChryslerMAPCorrection
	 (1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)					// 0 PF6 A1 MAP sensor
#ifdef useChryslerBaroSensor
	,(1 << REFS0)|									(1 << MUX2)|					(1 << MUX0)	// 1 PF5 A2 baro sensor
#ifdef useAnalogButtons
	,(1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 2 PF7 A0 analog button input
#else // useAnalogButtons
	,(1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 2 PF7 A0
#endif // useAnalogButtons
#else // useChryslerBaroSensor
#ifdef useAnalogButtons
	,(1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 1 PF7 A0 analog button input
#else // useAnalogButtons
	,(1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 1 PF7 A0
#endif // useAnalogButtons
	,(1 << REFS0)|									(1 << MUX2)|					(1 << MUX0)	// 2 PF5 A2 car voltage
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
	 (1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 0 PF7 A0 analog button input
	,(1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)					// 1 PF6 A1
	,(1 << REFS0)|									(1 << MUX2)|					(1 << MUX0)	// 2 PF5 A2 car voltage
#else // useAnalogButtons
	 (1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 0 PF7 A0
	,(1 << REFS0)|									(1 << MUX2)|	(1 << MUX1)					// 1 PF6 A1
	,(1 << REFS0)|									(1 << MUX2)|					(1 << MUX0)	// 2 PF5 A2 car voltage
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection
	,(1 << REFS0)|	(1 << MUX4)|	(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 3 ground
	,(1 << REFS0)|	(1 << MUX4)|	(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 4 ground
	,(1 << REFS0)|	(1 << MUX4)|	(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 5 ground
	,(1 << REFS0)|	(1 << MUX4)|	(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 6 ground
#endif // defined(__AVR_ATmega32U4__)

#if defined(__AVR_ATmega2560__)
#ifdef useChryslerMAPCorrection
	 (1 << REFS0)|																	(1 << MUX0)	// 0 PF1 A1 MAP sensor
#ifdef useChryslerBaroSensor
	,(1 << REFS0)|													(1 << MUX1)					// 1 PF2 A2 baro sensor
#ifdef useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 2 PF3 A3 analog button input
#else // useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 2 PF3 A3
#endif // useAnalogButtons
#else // useChryslerBaroSensor
#ifdef useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 1 PF3 A3 analog button input
#else // useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 1 PF3 A3
#endif // useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)					// 2 PF2 A2 car voltage
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
	 (1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 0 PF3 A3 analog button input
	,(1 << REFS0)|																	(1 << MUX0)	// 1 PF1 A1
	,(1 << REFS0)|													(1 << MUX1)					// 2 PF2 A2 car voltage
#else // useAnalogButtons
	 (1 << REFS0)|																	(1 << MUX0)	// 0 PF1 A1
	,(1 << REFS0)|													(1 << MUX1)					// 1 PF2 A2 car voltage
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 2 PF3 A3
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection
#ifdef useTWIsupport
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 3 ground
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 4 ground
#else // useTWIsupport
	,(1 << REFS0)|									(1 << MUX2)									// 3 PF4 A4
	,(1 << REFS0)|									(1 << MUX2)|					(1 << MUX0)	// 4 PF5 A5
#endif // useTWIsupport
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 5 ground
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 6 ground
#endif // defined(__AVR_ATmega2560__)

#if defined(__AVR_ATmega328P__)
#ifdef useChryslerMAPCorrection
	 (1 << REFS0)|																	(1 << MUX0)	// 0 PC1 A1 MAP sensor
#ifdef useChryslerBaroSensor
	,(1 << REFS0)|													(1 << MUX1)					// 1 PC2 A2 baro sensor
#ifdef useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 2 PC3 A3 analog button input
#else // useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 2 PC3 A3
#endif // useAnalogButtons
#else // useChryslerBaroSensor
#ifdef useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 1 PC3 A3 analog button input
#else // useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 1 PC3 A3
#endif // useAnalogButtons
	,(1 << REFS0)|													(1 << MUX1)					// 2 PC2 A2 car voltage
#endif // useChryslerBaroSensor
#else // useChryslerMAPCorrection
#ifdef useAnalogButtons
	 (1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 0 PC3 A3 analog button input
	,(1 << REFS0)|																	(1 << MUX0)	// 1 PC1 A1
	,(1 << REFS0)|													(1 << MUX1)					// 2 PC2 A2 car voltage
#else // useAnalogButtons
	 (1 << REFS0)|																	(1 << MUX0)	// 0 PC1 A1
	,(1 << REFS0)|													(1 << MUX1)					// 1 PC2 A2 car voltage
	,(1 << REFS0)|													(1 << MUX1)|	(1 << MUX0)	// 2 PC3 A3
#endif // useAnalogButtons
#endif // useChryslerMAPCorrection
#ifdef useTWIsupport
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0) // 3 ground
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 4 ground
#else // useTWIsupport
	,(1 << REFS0)|									(1 << MUX2)									// 3 PC4 A4
	,(1 << REFS0)|									(1 << MUX2)|					(1 << MUX0)	// 4 PC5 A5
#endif // useTWIsupport
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 5 ground
	,(1 << REFS0)|					(1 << MUX3)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// 6 ground
#endif // defined(__AVR_ATmega328P__)
};

#endif // useAnalogRead
const uint8_t internalOutputButton =		0b10000000;
const uint8_t internalProcessButtonsUp =	0b01000000;
#ifdef useCoastDownCalculator
const uint8_t internalCancelCDT =			0b00000010;
#endif // useCoastDownCalculator

// this ISR gets called every time timer 0 overflows.
//
// f(fast PWM) = f(system clock) / (N * 256)
//
// N - prescaler, which is 64
//
// so this ISR gets called every 256 * 64 / (system clock) seconds (for 20 MHz clock, that is every 0.8192 ms)
//
ISR( TIMER0_OVF_vect ) // system timer interrupt handler
{

	static unsigned long lastTime;
	static unsigned long buttonTimeoutCount;
	static unsigned long parkTimeoutCount;
	static unsigned long activityTimeoutCount;
	static unsigned long swapFEwithFCRcount;
#ifdef useCoastDownCalculator
	static unsigned long coastdownCount;
#endif // useCoastDownCalculator
#ifdef useBarFuelEconVsTime
	static unsigned long FEvTimeCount;
#endif // useBarFuelEconVsTime
	static unsigned int buttonLongPressCount;
	static unsigned int cursorCount;
	static unsigned int loopCount;
#ifdef useAnalogButtons
	static unsigned int analogSampleCount;
#endif // useAnalogButtons
#ifdef useJSONoutput
	static unsigned int JSONtimeoutCount;
#endif // useJSONoutput
	static uint8_t previousActivity;
	static uint8_t internalFlags;
#ifdef useTWIbuttons
	static uint8_t TWIsampleCount;
	static uint8_t TWIsampleState;
#endif // useTWIbuttons
	unsigned long thisTime;

	if (timer0Command & t0cResetTimer)
	{

		timer0Command &= ~(t0cResetTimer);
		timer0_overflow_count = 0; // initialize timer 0 overflow counter
		thisTime = TCNT0;
		timer0Status = 0;
		loopCount = loopTickLength;
		awakeFlags = 0;
		internalFlags = 0;
		mainLoopHeartBeat = 1;
		dirty &= ~(dGoodVehicleDrive);
		activityTimeoutCount = volatileVariables[(uint16_t)(vActivityTimeoutIdx)];
		activityFlags = (afActivityCheckFlags | afSwapFEwithFCR);
		previousActivity = (afActivityCheckFlags);
#ifdef useTWIbuttons
		TWIsampleCount = TWItickLength;
		TWIsampleState = 0;
#endif // useTWIbuttons
#if defined(useAnalogRead)
		analogStatus = asHardwareReady;
#ifdef useAnalogButtons
		analogSampleCount = analogSampleTickLength;
#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useLegacyButtons
		buttonDebounceCount = 0;
#endif // useLegacyButtons
#ifdef useBarFuelEconVsTime
		timer0Command |= (t0cResetFEvTime);
#endif // useBarFuelEconVsTime
		buttonLongPressCount = 0;
		buttonTimeoutCount = 0;
		parkTimeoutCount = 0;
		swapFEwithFCRcount = 0;

	}
	else
	{

		timer0_overflow_count += 256; // update TOV count
		thisTime = timer0_overflow_count | TCNT0; // calculate current cycle count

#ifdef useCPUreading
		volatileVariables[(uint16_t)(vSystemCycleIdx)]++; // systemcycles

#endif // useCPUreading
#ifdef useSoftwareClock
		volatileVariables[(uint16_t)(vClockCycleIdx)]++; // clockcycles

#endif // useSoftwareClock
	}

	lastTime = thisTime; // save cycle count

	if (awakeFlags & aAwakeOnInjector) // if MPGuino is awake on detected fuel injector event
	{

		if (watchdogInjectorCount) // if the fuel injector watchdog timer is running on minimum good engine speed
		{

			watchdogInjectorCount--; // cycle fuel injector watchdog timer down

#ifdef useChryslerMAPCorrection
			if (dirty & dSampleADC) // if injector monitor commanded an analog engine sensor read
			{

				dirty &= ~(dSampleADC); // acknowledge the command
				analogCommand |= (acSampleChrysler);

			}

#endif // useChryslerMAPCorrection
		}
		else // fuel injector watchdog timer has timed out
		{

			awakeFlags &= ~(aAwakeOnInjector); // signal that MPGuino is not awake any more due to no detected injector event during injector watchdog period
			dirty &= ~(dGoodEngineRun); // reset all fuel injector measurement flags
			watchdogInjectorCount = volatileVariables[(uint16_t)(vEngineOffTimeoutIdx)]; // start the fuel injector watchdog for engine off mode

		}

	}
	else // MPGuino is no longer awake due to no detected fuel injector events
	{

		if (awakeFlags & aAwakeEngineRunning) // if MPGuino is still awake due to running engine
		{

			if (watchdogInjectorCount) watchdogInjectorCount--; // cycle fuel injector watchdog timer down for engine off flag mode
			else
			{

				activityFlags |= (afEngineOffFlag); // flag engine as being off
				awakeFlags &= ~(aAwakeEngineRunning); // MPGuino is no longer awake due to engine running

			}

		}

	}

	if (VSScount) // if there is a VSS debounce countdown in progress
	{

		VSScount--; // bump down the VSS count
		if (VSScount == 0) updateVSS(thisTime); // if count has reached zero, go update VSS

	}

	if (awakeFlags & aAwakeOnVSS) // if MPGuino is awake on detected VSS pulse event
	{

		if (watchdogVSSCount) // if the VSS watchdog timer is running on minimum good vehicle speed
		{

			watchdogVSSCount--;

		}
		else // VSS watchdog timer has timed out on minimum good vehicle speed
		{

			awakeFlags &= ~(aAwakeOnVSS); // signal that MPGuino is no longer awake due to no detected VSS pulse event during VSS watchdog period
			dirty &= ~(dGoodVehicleMotion); // reset all VSS measurement flags
			watchdogVSSCount = volatileVariables[(uint16_t)(vVehicleStopTimeoutIdx)]; // start the VSS watchdog for vehicle stopped mode
			swapFEwithFCRcount = swapFEwithFCRdelay; // reset swap timer counter

		}

	}
	else // MPGuino is no longer awake due to no detected VSS pulse events
	{

		if (awakeFlags & aAwakeVehicleMoving) // if MPGuino is awake due to detected vehicle movement
		{

			if (watchdogVSSCount) watchdogVSSCount--;// cycle VSS watchdog timer down for vehicle stopped flag mode
			else
			{

				activityFlags |= (afVehicleStoppedFlag); // flag vehicle as stopped
				awakeFlags &= ~(aAwakeVehicleMoving); // vehicle is no longer awake on detected vehicle movement

			}

		}

		if ((activityFlags & afSwapFEwithFCR) == 0) // if not yet showing fuel consumption rate instead of fuel economy
		{

			if (swapFEwithFCRcount) swapFEwithFCRcount--; // cycle down fuel display watchdog until it zeroes out
			else activityFlags |= (afSwapFEwithFCR); // output fuel consumption rate function instead of fuel economy

		}

#ifdef useDragRaceFunction
		if (accelerationFlags & accelTestInProgress) // if acceleration test has started
		{

			timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);
			accelerationFlags &= ~(accelTestClearFlags); // reset drag race capture flags
			accelerationFlags |= (accelTestCompleteFlags); // signal that drag function is cancelled

		}

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
		internalFlags |= internalCancelCDT; // coastdown test will cancel if vehicle is idling

#endif // useCoastDownCalculator
	}

#ifdef useBarFuelEconVsTime
	if (timer0Command & t0cResetFEvTime) FEvTperiodIdx = FEvsTimeIdx | 0x80; // initialize fuel econ vs time trip index variable
	else
	{

		if (FEvTimeCount) FEvTimeCount--;
		else
		{

			timer0Command |= (t0cResetFEvTime);
			FEvTperiodIdx &= 0x7F; // strip off transfer bit, if present
			FEvTperiodIdx++;
			if (FEvTperiodIdx > FEvsTimeEndIdx) FEvTperiodIdx -= bgDataSize;
			FEvTperiodIdx |= 0x80; // next trip variable interaction will be a transfer

		}

	}

	if (timer0Command & t0cResetFEvTime)
	{

		timer0Command &= ~(t0cResetFEvTime);
		FEvTimeCount = volatileVariables[(uint16_t)(vFEvsTimePeriodTimeoutIdx)];

	}

#endif // useBarFuelEconVsTime
#ifdef useCoastDownCalculator
	if (internalFlags & internalCancelCDT)
	{

		internalFlags &= ~(internalCancelCDT);
		timer0Status |= (t0sCoastdownTestFlag);
		if (coastdownFlags & cdtTestInProgress) // if coastdown test has started
		{

			coastdownFlags &= ~(cdtTestClearFlags); // signal that coastdown test is no longer active
			coastdownFlags |= cdtCancelled | cdtFinished | cdSignalStateChange; // signal that coastdown test is cancelled

		}

	}
	else
	{

		if (coastdownFlags & cdtTestInProgress) // if coastdown test has been requested or is active
		{

			if (coastdownFlags & cdtTriggered) // if coastdown test has been requested
			{

				timer0Status |= (t0sCoastdownTestFlag);
				coastdownFlags &= ~(cdtTriggered); // clear coastdown test state
				coastdownFlags |= cdtActive | cdSignalStateChange; // mark coastdown test as active
				coastdownCount = volatileVariables[(uint16_t)(vCoastdownPeriodIdx)]; // reset coastdown counter

			}
			else
			{

				if (coastdownCount) coastdownCount--; // if coastdown clock hasn't elapsed
				else // perform state action
				{

					timer0Status |= (t0sCoastdownTestFlag);
					if (coastdownFlags & cdSampleTaken)
					{

						coastdownFlags &= ~(cdSampleTaken);
						coastdownFlags |= cdSignalStateChange; // signal coastdown test state change
						coastdownState++; // bump up to next state, for VSS read routine
						if (coastdownState > 2)
						{

							coastdownFlags &= ~(cdtActive); // make coastdown test no longer active
							coastdownFlags |= cdtFinished; // signal that coastdown test finished normally

						}
						else coastdownCount = volatileVariables[(uint16_t)(vCoastdownPeriodIdx)]; // reset coastdown counter

					}
					else coastdownFlags |= cdTakeSample;

				}

			}

		}

	}

#endif // useCoastDownCalculator
#ifdef useTWIbuttons
	if (TWIsampleCount)
	{

		TWIsampleCount--;

		if ((twiStatusFlags & twiOpenMain) == twiBlockMainProgram) // if TWI section is finished processing, and if timer0 TWI sampling is enabled
			switch (TWIsampleState)
			{

				case 0:
#ifdef useAdafruitRGBLCDshield
					TWI::openChannel(buttonAddress, TW_WRITE); // open TWI as master transmitter
					TWI::writeByte(MCP23017_B1_GPIOA); // specify bank A GPIO pin register address
					TWI::transmitChannel(TWI_REPEAT_START); // go write out read request, with repeated start to set up for read
#endif // useAdafruitRGBLCDshield

					TWIsampleState++; // advance to waiting for TWI sample request to finish

					break;

				case 1:
#ifdef useAdafruitRGBLCDshield
					TWI::openChannel(buttonAddress, TW_READ); // open TWI as master receiver
					TWI::transmitChannel(TWI_STOP); // go commit to read, send stop when read is finished
#endif // useAdafruitRGBLCDshield

					TWIsampleState++; // advance to waiting for TWI sample to complete

					break;

				case 2:
					if ((twiStatusFlags & twiErrorFlag) == 0)
					{

#ifdef useAdafruitRGBLCDshield
						thisButtonState = (twiDataBuffer[0] & buttonMask); // fetch button state that was just read in
						timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in
#endif // useAdafruitRGBLCDshield

					}

				default:
					twiStatusFlags &= ~(twiBlockMainProgram);
					break;

		}

	}
	else
	{

		TWIsampleCount = TWItickLength;

		if (timer0Command & t0cEnableTWIsample)
		{

			twiStatusFlags |= (twiBlockMainProgram); // block main program from making any TWI requests
			TWIsampleState = 0; // initialize TWI button read state machine

		}
		else twiStatusFlags &= ~(twiBlockMainProgram);

	}

#endif // useTWIbuttons
#ifdef useAnalogButtons
	if (analogSampleCount) analogSampleCount--;
	else
	{

		analogSampleCount = analogSampleTickLength;
		analogCommand |= (acSampleButtonChannel); // go sample analog button channel

	}

#endif // useAnalogButtons
#ifdef useLegacyButtons
	if (buttonDebounceCount) // if there is a button press debounce countdown in progress
	{

		buttonDebounceCount--;

		if (buttonDebounceCount == 0)
		{

			thisButtonState = (lastPINxState & buttonMask) ^ buttonMask; // strip out all but relevant button bits
			timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

		}

	}

#endif // useLegacyButtons
	if (buttonLongPressCount)
	{

		buttonLongPressCount--; // bump down the button long-press count by one

		if (buttonLongPressCount == 0)
		{

			buttonPress |= longButtonBit; // signal that a "long" button press has been detected
			internalFlags |= internalOutputButton;

		}

	}

	if (timer0Command & t0cProcessButton) // if button hardware reports reading in a button
	{

		timer0Command &= ~(t0cProcessButton); // ack
		if (thisButtonState != lastButtonState) // if there was a button state change since the last button was read in
		{

			if (thisButtonState == buttonsUp) // if it's all buttons being released
			{

				if (internalFlags & internalProcessButtonsUp) internalFlags |= internalOutputButton;

			}
			else
			{

				buttonPress = thisButtonState;
				internalFlags |= (internalProcessButtonsUp);
				buttonLongPressCount = buttonLongPressTick; // start the button long-press timer

			}

		}

		lastButtonState = thisButtonState;

	}

	if (internalFlags & internalOutputButton)
	{

		internalFlags &= ~(internalOutputButton);
		internalFlags &= ~(internalProcessButtonsUp);
		awakeFlags |= (aAwakeOnButton); // set awake status on button pressed
		timer0Command &= ~(t0cDisplayDelay); // shut off display change delay
		if (activityFlags & afActivityTimeoutFlag) timer0Status |= (t0sUpdateDisplay); // simply update the display if MPGuino was asleep
		else timer0Status |= (t0sReadButton | t0sShowCursor | t0sUpdateDisplay); // otherwise, force cursor show bit, and signal that keypress was detected
		buttonLongPressCount = 0; // reset button long-press timer
		cursorCount = cursorDelayTick; // reset cursor count
		activityFlags &= ~(afButtonFlag | afActivityTimeoutFlag);
		buttonTimeoutCount = volatileVariables[(uint16_t)(vButtonTimeoutIdx)];

	}

#ifdef useJSONoutput
	if (JSONtimeoutCount) JSONtimeoutCount--;
	else
	{

		timer0Status |= t0sOutputJSON; // signal to JSON output routine to display next round of subtitles
		JSONtimeoutCount = JSONtickLength; // restart JSON output timeout count

	}

#endif // useJSONoutput
	if (loopCount) loopCount--;
	else
	{

#if useDataLoggingOutput || useJSONoutput
		timer0Status |= (t0sUpdateDisplay | t0sTakeSample | t0sOutputLogging); // signal to main program that a sampling should occur, and to update display
#else // useDataLoggingOutput || useJSONoutput
		timer0Status |= (t0sUpdateDisplay | t0sTakeSample); // signal to main program that a sampling should occur, and to update display
#endif // useDataLoggingOutput || useJSONoutput
		loopCount = loopTickLength; // restart loop count
		mainLoopHeartBeat <<= 1; // cycle the heartbeat bit
		if (mainLoopHeartBeat == 0) mainLoopHeartBeat = 1; // wrap around the heartbeat bit, if necessary
#if defined(useAnalogRead)
		analogCommand |= (acSampleChannelInit); // go sample all non-critical channels
#endif // useAnalogRead

	}

	if (cursorCount) cursorCount--;
	else
	{

		cursorCount = cursorDelayTick; // reset cursor count
		timer0Status ^= t0sShowCursor; // toggle cursor show bit

	}

	if (timer0Command & t0cDisplayDelay) // if display change delay is in effect
	{

		if (displayPauseCount) displayPauseCount--; // update pause counter
		else
		{

			timer0Command &= ~(t0cDisplayDelay); // otherwise, signal that display change delay is over
			timer0Status |= (t0sUpdateDisplay); // tell main program to update the display

		}

	}

	if (timer0Command & t0cDoDelay) // if main program has requested a delay
	{

		if (timer0DelayCount) timer0DelayCount--; // bump timer delay value down by one tick
		else timer0Command &= ~(t0cDoDelay); // signal to main program that delay timer has completed main program request

	}

	if (awakeFlags & aAwakeOnButton)
	{

		if (buttonTimeoutCount) buttonTimeoutCount--;
		else
		{

			awakeFlags &= ~(aAwakeOnButton);
			activityFlags |= (afButtonFlag);

		}

	}

	if ((activityFlags & afParkCheckFlags) == afNotParkedFlags) // if MPGuino has engine stop and vehicle stop flags set, but is not yet parked
	{

		if (parkTimeoutCount) parkTimeoutCount--; // run down park watchdog timer until it expires
		else activityFlags |= (afParkFlag); // set vehicle parked flag

	}

	if ((activityFlags & afValidFlags) == afActivityCheckFlags) // if there is no activity but the activity watchdog hasn't timed out yet
	{

		if (activityTimeoutCount) activityTimeoutCount--; // cycle down the activity timeout watchdog
		else activityFlags |= (afActivityTimeoutFlag); // signal that MPGuino is in a period of inactivity

	}

	previousActivity ^= (activityFlags & afValidFlags); // detect any activity change since last timer0 tick

	if (previousActivity) activityChangeFlags |= (previousActivity); // if there was any activity change at all, signal that the display needs updating

	// reset activity timeout watchdog if any of the fuel injector, VSS pulse, button press, or park flags have changed
	if (previousActivity & afActivityCheckFlags) activityTimeoutCount = volatileVariables[(uint16_t)(vActivityTimeoutIdx)];

	// reset park timeout watchdog if any of the fuel injector or VSS pulse flags have changed
	if (previousActivity & afNotParkedFlags) parkTimeoutCount = volatileVariables[(uint16_t)(vParkTimeoutIdx)];

	previousActivity = (activityFlags & afValidFlags); // save for next timer0 tick

#if defined(useAnalogRead)
	if (analogCommand & acSampleChannelActive)
	{

		if (analogStatus & asHardwareReady)
		{

			analogCommand |= (acSampleGround); // signal to ADC interrupt that the last requested conversion was for internal ground
			analogStatus &= ~(asHardwareReady);

			ADMUX = pgm_read_byte(&analogChannelValue[(unsigned int)(analogGroundIdx)]);
			ADCSRA |= ((1 << ADSC) | (1 << ADIF) | (1 << ADIE)); // start ADC read, enable interrupt, and clear interrupt flag, because this crappy hardware allows the ADC interrupt to alway do free running mode

		}

	}

#endif // useAnalogRead
#ifdef useDebugCPUreading
	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += TCNT0;

#endif // useDebugCPUreading
}

#ifdef useTimer1Interrupt
#ifdef use4BitLCD
interfaceDevice devLCD;

#ifdef useBufferedLCD
ringBufferVariable lcdBuffer;

volatile uint8_t LCDdata[32];

#endif // useBufferedLCD

volatile unsigned int lcdDelayCount;

#endif // use4BitLCD
#ifdef useSimulatedFIandVSS
volatile uint16_t debugVSStickLength;
volatile uint16_t debugFIPtickLength;
volatile uint16_t debugFIPWtickLength;

volatile uint16_t debugVSScount;
volatile uint16_t debugFIPcount;
volatile uint16_t debugFIPWcount;

#endif // useSimulatedFIandVSS
// these flags specifically tell the system timer1 to do something
// main program sets flag, system timer1 acknowledges by clearing flag
volatile uint8_t timer1Command;

const uint8_t t1cResetTimer =			0b10000000;
#ifdef use4BitLCD
const uint8_t t1cDelayLCD =				0b01000000;
#endif // use4BitLCD
#ifdef useSimulatedFIandVSS
const uint8_t t1cEnableDebug =			0b00100000;
#endif // useSimulatedFIandVSS

// these flags specifically tell the main program to do something
// system timer1 sets flag, main program acknowledges by clearing flag
// these flags also have some internal use
volatile uint8_t timer1Status;

#ifdef useTWILCD
const uint8_t t1sLoopFlag =				0b10000000;
const uint8_t t1sDoOutputTWI =			0b01000000;
#endif // useTWILCD
#ifdef useSimulatedFIandVSS
const uint8_t t1sDebugUpdateFIP =		0b00100000;
const uint8_t t1sDebugUpdateVSS =		0b00010000;
#endif // useSimulatedFIandVSS

#ifdef useSimulatedFIandVSS
volatile uint8_t debugFlags;

const uint8_t debugVSSflag =			0b00000001;
const uint8_t debugInjectorFlag =		0b00000010;
const uint8_t debugFIPready =			0b00000100;
const uint8_t debugVSSready =			0b00001000;

const uint8_t debugEnableFlags =		debugVSSflag | debugInjectorFlag;
const uint8_t debugVSreadyFlags =		debugVSSready | debugVSSflag;
const uint8_t debugFIreadyFlags =		debugFIPready | debugInjectorFlag;

const unsigned long debugVSSresetLength = (unsigned long)(ceil)((2ul * F_CPU) / (2ul * 255ul)) - 1; // 2 sec
const unsigned long debugFIPresetLength = (unsigned long)(ceil)((4ul * F_CPU) / (3ul * 2ul * 255ul)) - 1; // 4/3 sec

#endif // useSimulatedFIandVSS
// this ISR gets called every time timer 1 overflows.
//
// f(phase correct PWM) = f(system clock) / (N * 510)
//
// N - prescaler, which is 1
//
// so this ISR gets called every 510 * 1 / (system clock) seconds (for 20 MHz clock, that is every 25.5 us)
ISR( TIMER1_OVF_vect ) // LCD delay interrupt handler
{

#ifdef use4BitLCD
	static uint8_t value;
#endif // use4BitLCD
#ifdef useSimulatedFIandVSS
	static unsigned long debugVSSresetCount;
	static unsigned long debugFIPresetCount;
#endif // useSimulatedFIandVSS
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	if (timer1Command & t1cResetTimer)
	{

		timer1Command &= ~(t1cResetTimer);
		timer1Status = 0;
#ifdef useSimulatedFIandVSS
		debugVSScount = 0;
		debugFIPcount = 0;
		debugFIPWcount = 0;
		debugVSSresetCount = 0;
		debugFIPresetCount = 0;
#endif // useSimulatedFIandVSS

	}

#ifdef useSimulatedFIandVSS
	if (timer1Command & t1cEnableDebug)
	{

		if ((debugFlags & debugVSreadyFlags) == debugVSreadyFlags) // if VSS simulator is ready to output
		{

			if (debugVSScount) debugVSScount--;
			else
			{

				debugVSScount = debugVSStickLength;
#if defined(__AVR_ATmega32U4__)
				PORTB ^= (1 << PINB7); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTK ^= (1 << PINK0); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTC ^= (1 << PINC0); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega328P__)

			}

		}

		if (debugFlags & debugVSSflag) // if VSS simulator is enabled
		{

			if (debugVSSresetCount) debugVSSresetCount--;
			else
			{

				debugVSSresetCount = debugVSSresetLength;
				timer1Status |= (t1sDebugUpdateVSS);

			}

		}

		if ((debugFlags & debugFIreadyFlags) == debugFIreadyFlags) // if fuel injector simulator is ready to output
		{

			if (debugFIPcount)
			{

				debugFIPcount--;

				if (debugFIPWcount) debugFIPWcount--;
				else
				{

#if defined(__AVR_ATmega32U4__)
					PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to generate injector closed interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE |= ((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin high to generate injector closed interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to generate injector closed interrupt
#endif // defined(__AVR_ATmega328P__)

				}

			}
			else
			{

				debugFIPcount = debugFIPtickLength;
				debugFIPWcount = debugFIPWtickLength;
				if (debugFIPWtickLength) // if DFCO is not commanded
				{

#if defined(__AVR_ATmega32U4__)
					PORTD &= ~((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin low to generate injector open interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE &= ~((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin low to generate injector open interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD &= ~((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin low to generate injector open interrupt
#endif // defined(__AVR_ATmega328P__)

				}

			}

		}

		if (debugFlags & debugInjectorFlag) // if injector simulator is enabled
		{

			if (debugFIPresetCount) debugFIPresetCount--;
			else
			{

				debugFIPresetCount = debugFIPresetLength;
				timer1Status |= (t1sDebugUpdateFIP);

			}

		}

	}

#endif // useSimulatedFIandVSS
#ifdef use4BitLCD
	if (timer1Command & t1cDelayLCD)
	{

		if (lcdDelayCount) lcdDelayCount--;
#ifdef useBufferedLCD
		else
		{

			if (ringBuffer::isBufferNotEmpty(lcdBuffer)) // if there's at least one nybble in the LCD send buffer
			{

#ifdef useTWILCD
				// if buffer is not empty and TWI hardware is ready
				if ((twiStatusFlags & twiOpenMain) == 0)
				{

					timer1Status &= ~(t1sDoOutputTWI); // reset TWI master transmission in progress flag
					timer1Status |= (t1sLoopFlag); // set loop flag

					do
					{

						value = ringBuffer::pull(lcdBuffer); // pull a buffered LCD nybble

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((timer1Status & t1sDoOutputTWI) == 0) // if this is the first nybble to be output
							{

								TWI::openChannel(lcdAddress, TW_WRITE); // open TWI as master transmitter
								TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address
								timer1Status |= (t1sDoOutputTWI); // signal to complete TWI master transmission

							}

						}

						LCD::outputNybble(value); // output the nybble and set timing

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((value & lcdSendFlags) == lcdSendNybble) // if sending an ordinary data nybble, check if we can continue looping
							{

								if ((twiDataBufferSize - twiDataBufferLen) < 5) timer1Status &= ~(t1sLoopFlag); // if TWI send buffer is getting low, signal end of loop
								if (ringBuffer::isBufferNotEmpty(lcdBuffer) == 0) timer1Status &= ~(t1sLoopFlag); // if LCD send buffer is empty, signal end of loop

							}
							else timer1Status &= ~(t1sLoopFlag); // otherwise, this is a special (command or reset) nybble, so signal end of loop

						}
						else timer1Status &= ~(t1sLoopFlag); // otherwise, this is just a delay request, so signal end of loop

					}
					while (timer1Status & t1sLoopFlag);

					if (timer1Status & t1sDoOutputTWI) TWI::transmitChannel(TWI_STOP); // commit LCD port expander write, if required

				}
#else // useTWILCD
				value = ringBuffer::pull(lcdBuffer); // pull a buffered LCD byte

				LCD::outputNybble(value); // output byte
#endif // useTWILCD

			}
			else timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

		}
#else // useBufferedLCD
		else timer1Command &= ~(t1cDelayLCD); // turn off LCD delay
#endif // useBufferedLCD

	}

#endif // use4BitLCD
#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useTimer1Interrupt
volatile unsigned long thisInjectorOpenStart;
volatile unsigned long thisEnginePeriodOpen; // engine speed measurement based on fuel injector open event
volatile unsigned long thisEnginePeriodClose; // engine speed measurement based on fuel injector close event

// fuel injector monitor interrupt pair
//
// this pair is responsible to measure fuel injector open pulse width, and engine speed
//
// the fuel injector monitor also performs a few sanity checks
//
// sanity check 1 - the engine revolution period measured must be less than the calculated period corresponding to the minimum acceptable engine RPM speed
//                  if this measured period is greater, then the fuel injector is assumed to be de-energized
//                   - the fuel injector pulse measurement is abandoned
//                   - the engine is also assumed to be turned off (for EOC mode)
//
// sanity check 2 - if a successful pulse measurement is made, the (measured pulse + injector open delay + injector close delay) must be less than the measured engine revolution period
//                  if this is not the case, the fuel injector is operating past its design duty cycle (typically 85% at 7000 RPM or something)
//                   - MPGuino may no longer be able to reliably measure fuel consumption
//                   - the main program is informed

// injector opening event handler
//
// this measures the start of the fuel injector pulse, and is used to calculate engine speed
//
#if defined(__AVR_ATmega32U4__)
ISR( INT2_vect )
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( INT4_vect )
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( INT0_vect )
#endif // defined(__AVR_ATmega328P__)
{

	static unsigned long lastInjectorOpenStart;
	unsigned int a;
#ifdef useDebugCPUreading
	unsigned int b;
#endif // useDebugCPUreading

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjectorOpenStart = timer0_overflow_count + (unsigned long)(a);

	if (dirty & dGoodEngineRotationOpen) thisEnginePeriodOpen = findCycleLength(lastInjectorOpenStart, thisInjectorOpenStart); // calculate length between fuel injector pulse starts
	else thisEnginePeriodOpen = 0;

#ifdef useChryslerMAPCorrection
	dirty |= (dGoodEngineRotationOpen | dInjectorReadInProgress | dSampleADC);
#else // useChryslerMAPCorrection
	dirty |= (dGoodEngineRotationOpen | dInjectorReadInProgress);
#endif // useChryslerMAPCorrection

	lastInjectorOpenStart = thisInjectorOpenStart;

	watchdogInjectorCount = volatileVariables[(uint16_t)(vDetectEngineOffIdx)]; // reset minimum engine speed watchdog timer

#ifdef useDebugCPUreading
	b = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) b = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += b - a;

#endif // useDebugCPUreading
}

// injector opening event handler
//
// this measures the end of the fuel injector pulse, and is used to calculate engine speed
//
// if a fuel injector pulse width measurement is in progress, this also performs the measurement and stores raw fuel consumption data
//
// it will either store one of the good existing engine period measurements or an average of both if both are good
//
#if defined(__AVR_ATmega32U4__)
ISR( INT3_vect )
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( INT5_vect )
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( INT1_vect )
#endif // defined(__AVR_ATmega328P__)
{

	static unsigned long lastInjectorCloseStart;

	uint8_t b;
	unsigned int a;
#ifdef useDebugCPUreading
	unsigned int c;
#endif // useDebugCPUreading
	unsigned long thisInjectorCloseStart;
	unsigned long engineRotationPeriod;
	unsigned long thisInjectorPulseLength;
	unsigned long goodInjectorPulseLength;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjectorCloseStart = timer0_overflow_count + (unsigned long)(a);

	if (dirty & dGoodEngineRotationClose) thisEnginePeriodClose = findCycleLength(lastInjectorCloseStart, thisInjectorCloseStart); // calculate length between fuel injector pulse starts
	else thisEnginePeriodClose = 0;

	if (dirty & dInjectorReadInProgress) // if there was a fuel injector open pulse detected, there's now a fuel injector pulse width to be measured
	{

		dirty &= ~(dInjectorReadInProgress);

		b = (dirty & dGoodEngineRotation);

		switch (b)
		{

			case (dGoodEngineRotationClose):
				engineRotationPeriod = thisEnginePeriodClose;
				b = dGoodInjectorRead;
				break;

			case (dGoodEngineRotationOpen):
				engineRotationPeriod = thisEnginePeriodOpen;
				b = dGoodInjectorRead;
				break;

			case (dGoodEngineRotation):
				engineRotationPeriod = thisEnginePeriodClose + thisEnginePeriodOpen;
				engineRotationPeriod++; // perform pre-emptive rounding up from averaging operation
				engineRotationPeriod >>= 1; // perform average of two measurements
				b = dGoodInjectorRead;
				break;

			default:
				b = 0;
				break;

		}

		// calculate fuel injector pulse length
		thisInjectorPulseLength = findCycleLength(thisInjectorOpenStart, thisInjectorCloseStart) - volatileVariables[(uint16_t)(vInjectorOpenDelayIdx)]; // strip off injector open delay time

		// if this pulse is larger than the maximum good pulse that could happen at the minimum valid engine speed, reject it
		// 1 - pulse could be narrower than vInjectorOpenDelayIdx
		// 2 - pulse could be wider than the maximum allowable pulse width for minimum good engine speed
		if (thisInjectorPulseLength > volatileVariables[(uint16_t)(vInjectorValidMaxWidthIdx)]) dirty &= ~(dGoodInjectorWidth | dGoodInjectorRead);
		else dirty |= (dGoodInjectorWidth);

		if (b) // if we have an engine rotation period measurement
		{

			// calculate good maximum fuel injector open time for injector pulse width sanity check
			goodInjectorPulseLength = engineRotationPeriod - volatileVariables[(uint16_t)(vInjectorTotalDelayIdx)];

			if (thisInjectorPulseLength > goodInjectorPulseLength) dirty &= ~(dGoodInjectorRead); // if measured pulse is larger than largest good pulse, signal that last injector read may be bad
			else dirty |= (dGoodInjectorRead); // signal that last injector read is good

			// if measured engine speed is greater than the specified minimum good engine speed
			if (engineRotationPeriod < volatileVariables[(uint16_t)(vMaximumEnginePeriodIdx)])
			{

				activityFlags &= ~(afEngineOffFlag | afParkFlag | afActivityTimeoutFlag); // signal that engine is running, and vehicle is therefore no longer parked
				awakeFlags |= (aAwakeEngineRunning); // MPGuino is awake due to engine running

			}

#ifdef trackIdleEOCdata
			if (awakeFlags & aAwakeVehicleMoving) tripVar::add64(collectedEngCycleCount, engineRotationPeriod, rawTripIdx); // add to fuel injector total cycle accumulator
			else tripVar::add64(collectedEngCycleCount, engineRotationPeriod, rawEOCidleTripIdx); // add to idle fuel injector total cycle accumulator

#else // trackIdleEOCdata
			tripVar::add64(collectedEngCycleCount, engineRotationPeriod, rawTripIdx); // add to fuel injector total cycle accumulator

#endif // trackIdleEOCdata
#ifdef useDragRaceFunction
			// add to distance acceleration fuel injector total cycle accumulator
			if (accelerationFlags & accelTestActive) tripVar::add64(collectedEngCycleCount, engineRotationPeriod, dragRawDistanceIdx);

#endif // useDragRaceFunction
		}

		// if the injector pulse width is valid
		if (dirty & dGoodInjectorWidth)
		{

			awakeFlags |= (aAwakeOnInjector); // signal that MPGuino is awake due to detected injector

#ifdef useChryslerMAPCorrection
			thisInjectorPulseLength *= volatileVariables[(uint16_t)(mpInjectorCorrectionIdx)]; // multiply by differential fuel pressure correction factor numerator
			thisInjectorPulseLength >>= 12; // divide by differential fuel pressure correction factor denominator

#endif // useChryslerMAPCorrection
#ifdef trackIdleEOCdata
			if (awakeFlags & aAwakeVehicleMoving) // if vehicle is moving, save injector measurements in active raw trip variable
			{

				collectedInjPulseCount[(uint16_t)(rawTripIdx)]++; // add to fuel injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, rawTripIdx); // add to fuel injector open cycle accumulator

			}
			else // if vehicle is not moving, save injector measurements in active idle/EOC raw trip variable
			{

				collectedInjPulseCount[(uint16_t)(rawEOCidleTripIdx)]++; // add to idle fuel injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, rawEOCidleTripIdx); // add to idle fuel injector open cycle accumulator

			}

#else // trackIdleEOCdata
			collectedInjPulseCount[(uint16_t)(rawTripIdx)]++; // add to fuel injector pulse count
			tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, rawTripIdx); // add to fuel injector open cycle accumulator

#endif // trackIdleEOCdata
#ifdef useDragRaceFunction
			if (accelerationFlags & accelTestActive)
			{

				collectedInjPulseCount[(uint16_t)(dragRawDistanceIdx)]++; // update the distance acceleration injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, dragRawDistanceIdx); // add to distance acceleration fuel injector open cycle accumulator

			}

#endif // useDragRaceFunction
		}

	}

	dirty |= (dGoodEngineRotationClose);
	lastInjectorCloseStart = thisInjectorCloseStart;

	watchdogInjectorCount = volatileVariables[(uint16_t)(vDetectEngineOffIdx)]; // reset minimum engine speed watchdog timer

#ifdef useDebugCPUreading
	c = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) c = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c - a;

#endif // useDebugCPUreading
}

#if defined(__AVR_ATmega32U4__)
ISR( PCINT0_vect )
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( PCINT2_vect )
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( PCINT1_vect )
#endif // defined(__AVR_ATmega328P__)
{

	uint8_t p;
	uint8_t q;

	unsigned int a;
#ifdef useDebugCPUreading
	unsigned int c;
#endif // useDebugCPUreading
	unsigned long thisTime;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisTime = timer0_overflow_count + (unsigned long)(a);

#if defined(__AVR_ATmega32U4__)
	p = PINB; // read current input pin
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	p = PINK; // read current input pin
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	p = PINC; // read current input pin
#endif // defined(__AVR_ATmega328P__)
	q = p ^ lastPINxState; // detect any changes from the last time this ISR is called

#if defined(__AVR_ATmega32U4__)
	if (q & (1 << PINB7)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	if (q & (1 << PINK0)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	if (q & (1 << PINC0)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega328P__)
	{

		if (VSSpause) VSScount = VSSpause; // if there is a VSS debounce count defined, set VSS debounce count and let system timer handle the debouncing
		else updateVSS(thisTime); // otherwise, go process VSS pulse

	}

#ifdef useLegacyButtons
	if (q & buttonMask) buttonDebounceCount = buttonDebounceTick; // if a button change was detected, set button press debounce count, and let system timer handle the debouncing

#endif // useLegacyButtons
	lastPINxState = p; // remember the current input pin state for the next time this ISR gets called

#ifdef useDebugCPUreading
	c = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) c = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c - a;

#endif // useDebugCPUreading
}

static void updateVSS(unsigned long thisVSStime)
{

	static unsigned long lastVSStime;

	static unsigned long cycleLength;

	if (dirty & dGoodVSSsignal) // if a valid VSS signal had previously been read in
	{

		dirty |= (dGoodVSSRead); // mark valid VSS pulse measurement
		awakeFlags |= (aAwakeOnVSS); // MPGuino is awake on valid VSS pulse measurement

		cycleLength = findCycleLength(lastVSStime, thisVSStime); // calculate VSS pulse length

		if (cycleLength < volatileVariables[(uint16_t)(vMaximumVSSperiodIdx)]) // if VSS period is less than that for minimum good vehicle speed
		{

			if (activityFlags & afVehicleStoppedFlag) // if vehicle has been previously flagged as not moving
			{

				activityFlags &= ~(afVehicleStoppedFlag | afSwapFEwithFCR | afParkFlag | afActivityTimeoutFlag); // signal that vehicle is moving, and vehicle is therefore no longer parked
				awakeFlags |= (aAwakeVehicleMoving); // MPGuino is awake on vehicle movement

			}

		}

#ifdef trackIdleEOCdata
		if (awakeFlags & aAwakeEngineRunning)
		{

			collectedVSSpulseCount[(unsigned int)(rawTripIdx)]++;
			tripVar::add64(collectedVSScycleCount, cycleLength, rawTripIdx); // add to VSS cycle accumulator

		}
		else // if the engine is not running, vehicle is in EOC mode
		{

			collectedVSSpulseCount[(unsigned int)(rawEOCidleTripIdx)]++;
			tripVar::add64(collectedVSScycleCount, cycleLength, rawEOCidleTripIdx); // add to EOC VSS cycle accumulator

		}

#else // trackIdleEOCdata
		collectedVSSpulseCount[(unsigned int)(rawTripIdx)]++;
		tripVar::add64(collectedVSScycleCount, cycleLength, rawTripIdx); // add to VSS cycle accumulator

#endif // trackIdleEOCdata
#ifdef useCoastDownCalculator
		if (coastdownFlags & (cdtActive | cdTakeSample)) // if coastdown test is active
		{

			coastdownFlags &= ~(cdTakeSample);
			coastdownFlags |= cdSampleTaken;
			volatileVariables[(uint16_t)(vCoastdownMeasurement1Idx + coastdownState)] = cycleLength;

		}

#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
		if (accelerationFlags & accelTestTriggered) // if accel test function is triggered
		{

				accelerationFlags &= ~accelTestTriggered; // switch status from 'triggered' to 'active'
				accelerationFlags |= accelTestActive;
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

		}

		if (accelerationFlags & accelTestActive) // if accel test function is active
		{

			if (volatileVariables[(uint16_t)(vDragInstantSpeedIdx)] > cycleLength) volatileVariables[(uint16_t)(vDragInstantSpeedIdx)] = cycleLength; // if this vehicle speed is larger than previous vehicle speed

			collectedVSSpulseCount[(unsigned int)(dragRawDistanceIdx)]++; // update the accel test distance measurement VSS pulse count
			tripVar::add64(collectedVSScycleCount, cycleLength, dragRawDistanceIdx); // update the accel test distance measurement VSS cycle accumulator

			if (accelerationFlags & accelTestDistance)
			{

				if (volatileVariables[(uint16_t)(vAccelDistanceValueIdx)]) volatileVariables[(uint16_t)(vAccelDistanceValueIdx)]--; // count down drag distance setpoint in VSS pulses
				else
				{

					accelerationFlags &= ~accelTestDistance; // otherwise, mark drag function distance measurement as complete
					timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

				}

			}

			if ((accelerationFlags & accelTestHalfSpeed) && (cycleLength < volatileVariables[(uint16_t)(vAccelHalfPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripVar::transfer(dragRawDistanceIdx, dragRawHalfSpeedIdx);
				accelerationFlags &= ~accelTestHalfSpeed; // mark drag function speed measurement as complete
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

			if ((accelerationFlags & accelTestFullSpeed) && (cycleLength < volatileVariables[(uint16_t)(vAccelFullPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripVar::transfer(dragRawDistanceIdx, dragRawFullSpeedIdx);
				accelerationFlags &= ~accelTestFullSpeed; // mark drag function speed measurement as complete
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

			if ((accelerationFlags & accelTestMeasurementFlags) == 0) // if all drag measurements have completed, mark drag function as complete
			{

				accelerationFlags &= ~accelTestActive; // switch status from 'active' to 'finished'
				accelerationFlags |= accelTestFinished;
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

		}

#endif // useDragRaceFunction
	}

	dirty |= dGoodVSSsignal; // annotate that a valid VSS pulse has been read
	watchdogVSSCount = volatileVariables[(uint16_t)(vDetectVehicleStopIdx)]; // reset minimum engine speed watchdog timer
	lastVSStime = thisVSStime;

}

unsigned long findCycleLength(unsigned long lastCycle, unsigned long thisCycle) // this is only to be meant to be used with interrupt handlers
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

#if defined(useAnalogRead)
// this interrupt is called upon completion of an analog to digital conversion
//
// this interrupt is normally initially called by timer0, and will continue as long as there are analog channel requests present
//
// each analog read is immediately preceded by an analog read of the internal ground, to ensure the successive approximation capacitor network is ready to go
//
// based on a 20 MHz clock, and a prescaler of 128, a single A/D conversion should take about 179 microseconds
//
// it is important to note that even if a conversion is complete, and free-wheeling mode is NOT repeat NOT selected at all,
// the AVR hardware will continue to start another conversion and interrupt itself again (!) and this is not mentioned at all in the fucking manual
//
ISR( ADC_vect )
{

	unsigned int rawRead;
	union union_16 * rawValue = (union union_16 *)(&rawRead);
	uint8_t analogChannelIdx;
	uint8_t analogChannelMask;
	uint8_t flag;
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	rawValue->u8[0] = ADCL; // (locks ADC sample result register from AtMega hardware)
	rawValue->u8[1] = ADCH; // (releases ADC sample result register to AtMega hardware)

	if (analogCommand & acSampleGround)
	{

		analogCommand &= ~(acSampleGround); // signal that internal ground was read

		flag = 1;
		analogChannelMask = acSampleChannel0; // start with highest priority channel
		analogChannelIdx = 0;

		while ((flag) && (analogChannelMask))
		{

			if (analogCommand & analogChannelMask) flag = 0; // if a commanded analog channel was detected, exit the loop
			else
			{

				analogChannelIdx++; // increment the analog channel index
				analogChannelMask >>= 1; // shift mask right one bit

			}

		}

		if (analogChannelMask)
		{

			analogStatus &= ~(analogChannelMask); // main program really doesn't care that a ground was read, it's not useful, so don't signal it
			analogValueIdx = analogChannelIdx; // save the analog index value
			analogBitmask = analogChannelMask; // save the analog bitmask
			analogCommand &= ~(analogChannelMask); // clear the relevant bit in analog command status
			flag = 1;

		}
		else
		{

			analogCommand &= ~(acSampleChannelActive); // an invalid channel was requested, so ignore it
			flag = 0;

		}

	}
	else
	{

		analogValue[(unsigned int)(analogValueIdx)] = rawRead; // save the value just read in
		analogStatus |= (analogBitmask); // signal to main program that an analog channel was read in
		if (analogCommand & acSampleChannelActive)
		{

			analogCommand |= (acSampleGround); // signal that next read is for internal ground
			analogChannelIdx = analogGroundIdx;
			flag = 1;

		}
		else flag = 0;

	}

	if (flag)
	{

		ADMUX = pgm_read_byte(&analogChannelValue[(unsigned int)(analogChannelIdx)]); // select next analog channel to read
		ADCSRA |= ((1 << ADSC) | (1 << ADIF) | (1 << ADIE)); // start ADC read, enable interrupt, and clear interrupt flag

	}
	else
	{

		analogStatus |= (asHardwareReady);
		analogCommand &= ~(acSampleChannelActive); // an invalid channel was requested, so ignore it
		ADCSRA |= (1 << ADIF);
		ADCSRA &= ~(1 << ADIE); // shut off analog interrupt and clear analog interrupt flag

	}

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useAnalogRead
#ifdef useIsqrt
static unsigned int iSqrt(unsigned int n)
{

	unsigned long w = 4096; // square factor guess
	unsigned int t = 4096; // proposed square root
	int d; // difference between guess and proposed
	int od = 0;

	for (uint8_t x = 0; x < 6; x++)
	{

		od = d;
		d = n - (unsigned int)w;
		d >>= 1;
		t += d;

		od += d;

		if ((d == 0) || (od == 0)) break;

		w = (unsigned long)t * (unsigned long)t;
		w >>= 12;

	}

	return t;

}

#endif // useIsqrt
void delay0(unsigned int ms)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	timer0DelayCount = ms; // request a set number of timer tick delays per millisecond
	timer0Command |= t0cDoDelay; // signal request to timer

	SREG = oldSREG; // restore interrupt flag status

	while (timer0Command & t0cDoDelay) idleProcess(); // wait for delay timeout

}

#ifdef useTWIsupport
static void TWI::init(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega32U4__)
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRTWI); // turn on TWI module
	PORTC |= ((1 << PORTC5) | (1 << PORTC4)); // enable port C TWI pin pullup resistors
#endif // defined(__AVR_ATmega328P__)
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // set TWI prescaler to 1
	TWBR = (uint8_t)(((unsigned int)(F_CPU / (twiFrequency * 1000UL)) - 16) / 2); // set TWI frequency
	TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // enable TWI module, acks, and interrupt

	twiStatusFlags = twiClose; // initialize TWI state to ready, default send stop, and no repeated start in progress

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::shutdown(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	while (twiStatusFlags & twiOpenMain) idleProcess(); // wait for all TWI transactions to complete

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	TWCR &= ~((1 << TWEN) | (1 << TWIE)); // disable TWI module and interrupt
#if defined(__AVR_ATmega32U4__)
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PORTC &= ~((1 << PORTC5) | (1 << PORTC4)); // disable port C TWI pin pullup resistors
	PRR |= (1 << PRTWI); // turn off TWI module
#endif // defined(__AVR_ATmega328P__)

	twiStatusFlags = 0;

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::openChannelMain(uint8_t address, uint8_t writeFlag) // this is in main program context
{

	uint8_t oldSREG;

	while (twiStatusFlags & twiOpenMain) idleProcess(); // wait for TWI to become available

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	openChannel(address, writeFlag);

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::openChannel(uint8_t address, uint8_t writeFlag) // this is in interrupt context
{

	twiSlaveAddress = (address << 1); // initial slave address
	twiSlaveAddress |= (writeFlag); // slap on direction

	twiStatusFlags |= (twiOpen); // signal twi is in use
	twiStatusFlags &= ~(twiErrorFlag); // clear error condition
	twiErrorCode = 0x00; // reset error state

	twiDataBufferLen = 0; // initialize buffer length
	twiDataBufferIdx = 0; // initialize buffer index

}

static uint8_t TWI::writeByte(uint8_t data) // this can be in either main program or interrupt context
{

	if(twiDataBufferLen < twiDataBufferSize)
	{

		twiDataBuffer[(unsigned int)(twiDataBufferLen++)] = data;
		return 0;

	}
	else
	{

		peripheral::changeBitFlags(twiStatusFlags, twiOpen, 0); // free up TWI for main program use
		return 1; // signal buffer overflow

	}

}

static void TWI::transmitChannel(uint8_t sendStop) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if ((twiSlaveAddress & TW_WRITE) && (twiDataBufferLen == 0)) twiStatusFlags &= ~(twiOpen);
	else
	{

		if (sendStop) twiStatusFlags |= (twiClose); // if stop was requested as end of transmission, remember it
		else twiStatusFlags &= ~(twiClose); // otherwise, remember to do a repeated start

		// if we're in the repeated start state, then we've already sent the start,
		// and the TWI state machine is just waiting for the address byte.
		// We need to remove ourselves from the repeated start state before we enable interrupts,
		// since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
		// up. Also, don't enable the START interrupt. There may be one pending from the
		// repeated start that we sent outselves, and that would really confuse things.

		if (twiStatusFlags & twiRemainOpen) // if in repeated start state
		{

			twiStatusFlags &= ~(twiRemainOpen); // clear repeated start state
			TWDR = twiSlaveAddress; // set data address to slave address
			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE));  // send ACK

		}
		else TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTA)); // enable INT and send start condition

	}

	SREG = oldSREG; // restore interrupt flag status

}

#endif // useTWIsupport
#ifdef useAdafruitRGBLCDshield
// initialize Adafruit RGB LGB shield to a known initial state
void adafruitRGBLCDsupport::init(void)
{

	uint16_t MCP23017registers;

	adafruitRGBLCDflags = 0; // initially set all control register bits to 0

	// we might have inadvertently entered into MCP23017 bank mode - if we didn't, we'll just end up disabling all of bank B's interrupt enable bits
	writeRegister8Bit(MCP23017_B1_IOCON, adafruitRGBLCDflags); // write initialization value to IO control register

	// if we did happen to enter bank mode prior to initialization, we are now known to not be in bank mode
	writeRegister8Bit(MCP23017_B0_IOCON, adafruitRGBLCDflags); // write initialization value to IO control register

	setTransferMode(adaTWItoggleMode);

	MCP23017registers = 0;

	writeRegister16Bit(MCP23017_B0_GPINTENx, (union_16 *)(&MCP23017registers)); // write out interrupt enable register (the Adafruit RGB LCD shield wasn't wired to support this)
	writeRegister16Bit(MCP23017_B0_IODIRx, (union_16 *)(&MCP23017registers)); // write out port direction (no inputs)
	writeRegister16Bit(MCP23017_B0_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (input bits same as GPIO bits)
	writeRegister16Bit(MCP23017_B0_GPINTENx, (union_16 *)(&MCP23017registers)); // write out GPIO bit interrupt assignment register (no GPIO bits associated with interrupts)
	writeRegister16Bit(MCP23017_B0_DEFVALx, (union_16 *)(&MCP23017registers)); // write out interrupt default value register (all default bits 0)
	writeRegister16Bit(MCP23017_B0_INTCONx, (union_16 *)(&MCP23017registers)); // write out interrupt control register (all interrupts trigger on GPIO input pin change)
	writeRegister16Bit(MCP23017_B0_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (no pull-up resistors)
	writeRegister16Bit(MCP23017_B0_GPIOx, (union_16 *)(&MCP23017registers)); // write out GPIO state (all 0) - also writes out OLAT register

}

void adafruitRGBLCDsupport::writeRegister16Bit(uint8_t registerAddress, union union_16 * registerValue)
{

	writeRegister16Bit(registerAddress, registerValue->u8[0], registerValue->u8[1]);

}

void adafruitRGBLCDsupport::writeRegister16Bit(uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte)
{

	if (adafruitRGBLCDflags & afRGBLCDbankMode) setTransferMode(adaTWItoggleMode); // if address mode isn't set to access 16-bit registers, configure as such

	TWI::openChannelMain(adafruitRGBLCDaddress, TW_WRITE); // open TWI as master transmitter
	TWI::writeByte(registerAddress | MCP23017_B0_PORTA); // specify bank A register address
	TWI::writeByte(portAbyte); // write desired value to register bank A byte
	TWI::writeByte(portBbyte); // write desired value to register bank B byte
	TWI::transmitChannel(TWI_STOP); // go write out register contents

}

void adafruitRGBLCDsupport::writeRegister8Bit(uint8_t registerAddress, uint8_t portByte)
{

	TWI::openChannelMain(adafruitRGBLCDaddress, TW_WRITE); // open TWI as master transmitter
	TWI::writeByte(registerAddress); // specify register address
	TWI::writeByte(portByte); // write desired value to register address
	TWI::transmitChannel(TWI_STOP); // go write out register contents

}

void adafruitRGBLCDsupport::setTransferMode(uint8_t mode)
{

	uint8_t address;
	uint8_t newFlags = adafruitRGBLCDflags;

	switch (mode)
	{

		case adaTWIsequentialMode:
			newFlags &= ~(afRGBLCDbankMode | afRGBLCDbyteMode); // set registers to be sequential A-B byte pairs from 0x00-0x15, enable sequential address update
			break;

		case adaTWItoggleMode: // this weird mode causes successive register accesses to flip between byte A and byte B on the shield
			newFlags &= ~(afRGBLCDbankMode); // set registers to be sequential A-B byte pairs from 0x00-0x15
			newFlags |= (afRGBLCDbyteMode); // disable sequential address update
			break;

		case adaTWIbyteMode:
			newFlags |= (afRGBLCDbankMode | afRGBLCDbyteMode); // set registers to be separated into separate A and B bytes, disable sequential address update
			break;

		default:
			break;

	}

	if (adafruitRGBLCDflags != newFlags) // if a change was detected
	{

		if (adafruitRGBLCDflags & afRGBLCDbankMode) address = MCP23017_B1_IOCON;
		else address = MCP23017_B0_IOCON;

		writeRegister8Bit(address, newFlags); // write new value to IO control register

		adafruitRGBLCDflags = newFlags; // changes take effect just as soon as IOCON is written

	}

}

#endif // useAdafruitRGBLCDshield
#ifdef useLCDoutput
/* LCD hardware support section */

#ifdef useLCDgraphics
const uint8_t cgramFlagDirty =		0b10000000;

uint8_t CGRAMbuffer[64]; // used by LCD output routine

#endif useLCDgraphics
static void LCD::init(void)
{

#ifdef use4BitLCD
	uint8_t oldSREG;

#endif // use4BitLCD
	devLCD.chrOut = LCD::writeData;

#ifdef useLCDgraphics
	for (uint8_t x = 0; x < 64; x++) CGRAMbuffer[(uint16_t)(x)] = cgramFlagDirty;

#endif useLCDgraphics
#ifdef useParallaxSerialLCDmodule
	devLCDserial.controlFlags &= ~(odvFlagCRLF);

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
#ifdef useBufferedLCD
	ringBuffer::init(lcdBuffer, LCDdata);

#endif // useBufferedLCD
#ifdef useTWILCD
#ifdef useTWIbuttons
	disableIntSample(); // disable TWI button sampling

#endif // useTWIbuttons
	portLCD = 0; // reset LCD port byte
#ifdef useAdafruitRGBLCDshield
	portSwitches = 0; // reset button port byte (contains two of the three LCD backlight LED bits)

	adafruitRGBLCDsupport::setTransferMode(adaTWIbyteMode);
#endif // useAdafruitRGBLCDshield

#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
	// set OC1A to non-inverting mode for LCD contrast
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// set OC1B to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdContrast | lcdBrightness);
	DDRD |= (lcdBit2 | lcdBit1);
	DDRE |= lcdEnable;
	DDRF |= (lcdRegisterSelect | lcdBit0 | lcdDirection);
	PORTF &= ~lcdDirection; // write a zero to this pin - MPGuino has no need to read anything from the LCD module

#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here

#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRA |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);
	DDRB |= (lcdBrightness | lcdContrast);

#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here

#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBrightness);
	DDRD |= (lcdBit0 | lcdContrast | lcdEnable | lcdRegisterSelect);

#endif // defined(__AVR_ATmega328P__)
	setContrast(EEPROM::readVal(pContrastIdx));

#endif // useTWILCD
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	lcdDelayCount = 0;
	timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

	SREG = oldSREG; // restore interrupt flag status

	// perform reset by instruction on LCD interface
	// the below 5 lines actually take up less space than using a PROGMEM table / loop to write these values
	writeNybble(lcdNullValue, lcdDelay0015ms); // wait for more than 15 msec
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay4100us); // send (B0011) to DB7-4, then wait for more than 4.1 ms
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(0b00100000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0010) to DB7-4 for 4 bit mode, then wait for more than 40 us

	// ready to use normal LCD output function now!
	writeCommand(lcdFunctionSet | lcdFSnumberOfLines); // 4-bit interface, 2 display lines, 5x8 font

#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // useBufferedLCD
#if useTWIbuttons && useTWILCD
	enableIntSample(); // enable TWI button sampling

#endif // useTWIbuttons && useTWILCD
#endif // use4BitLCD
	writeData(0x11); // set initial brightness
	writeData(0x16); // display control - turn on display, no cursor, no blink
	writeData(0x0C); // clear display, set cursor position to zero

#ifdef useBufferedLCD
#ifdef use4BitLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
	ringBuffer::flush(LCDserialBuffer); // clear the LCD buffer to force the LCD screen to clear

#endif // useParallaxSerialLCDmodule
#endif // useBufferedLCD
}

static void LCD::shutdown(void)
{

	writeData(0x12); // turn off LCD brightness
#ifdef useLegacyLCD
	setContrast(255); // turn off LCD contrast
#endif // useLegacyLCD

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::chrOut(21); // turn off LCD display
#ifdef useBufferedLCD
	ringBuffer::flush(LCDserialBuffer); // flush LCD output buffer

#endif // useBufferedLCD
	LCDserialPort::shutdown(); // shut down LCD serial port

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
	writeCommand(lcdDisplayControl); // turn off LCD display
#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer
#endif // useBufferedLCD

#ifndef useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
	// disable LCD pins
	DDRB &= ~(lcdBit3);
	DDRD &= ~(lcdBit2 | lcdBit1);
	DDRE &= ~(lcdEnable);
	DDRF &= ~(lcdRegisterSelect | lcdBit0 | lcdDirection);

	PORTB |= lcdContrast; // ensure LCD contrast is turned off
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here

#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	// disable LCD pins
	DDRA &= ~(lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);

	PORTB |= lcdContrast; // ensure LCD contrast is turned off
#ifdef useInvertedLegacyLCDbrightness
	PORTB |= lcdBrightness; // ensure LCD brightness is turned off
#else // useInvertedLegacyLCDbrightness
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#endif // useInvertedLegacyLCDbrightness

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here

#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable LCD pins
	DDRB &= ~(lcdBit3 | lcdBit2 | lcdBit1);
	DDRD &= ~(lcdBit0 | lcdEnable | lcdRegisterSelect);

	PORTD |= lcdContrast;
#ifdef useInvertedLegacyLCDbrightness
	PORTB |= lcdBrightness; // ensure LCD brightness is turned off
#else // useInvertedLegacyLCDbrightness
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#endif // useInvertedLegacyLCDbrightness

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD
#endif // use4BitLCD
}

#ifdef useLCDgraphics
static void LCD::loadCGRAMcharacter(uint8_t chr, char * chrData)
{

	uint8_t cgrAddress;

	cgrAddress = ((chr & 0x07) << 3);

	for (uint8_t x = 0; x < 8; x++) writeCGRAMbyte(cgrAddress + x, (*chrData++) & 0x1F);

}

static void LCD::writeCGRAMbyte(uint8_t cgrAddress, uint8_t chr)
{

	// if there is any bit change in the CGRAM byte
	if ((chr ^ CGRAMbuffer[(uint16_t)(cgrAddress)]) & 0x9F) CGRAMbuffer[(uint16_t)(cgrAddress)] = (chr | cgramFlagDirty);

}

static uint8_t LCD::peekCGRAMbyte(uint8_t cgrAddress)
{

	// if there is any bit change in the CGRAM byte
	return CGRAMbuffer[(uint16_t)(cgrAddress)] & 0x1F;

}

static void LCD::flushCGRAM(void)
{

	uint8_t cgrAddress;
	uint8_t f;
	uint8_t y;
	uint8_t b;

#ifdef use4BitLCD
	f = 0;
	y = 255;

	// cycle through the entire CGRAM buffer
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress++)
	{

		b = CGRAMbuffer[(uint16_t)(cgrAddress)];

		if (b & cgramFlagDirty)
		{

			f = 1;

			if (y != cgrAddress)
			{

				y = cgrAddress;
				writeCommand(lcdSetCGRAMaddress | cgrAddress); // set CGRAM address

			}

			writeByte(b & ~(cgramFlagDirty), lcdDataByte, lcdDelay0040us);
			y++;

		}

	}

	if (f) writeCommand(LCDgotoXYaddress); // set DDRAM to whatever the screen position was

#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
	// cycle through all 8 CGRAM characters
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress = cgrAddress + 8)
	{

		f = 0;

		// scan this CGRAM block for any dirty bytes
		for (y = 0; y < 8; y++) if (CGRAMbuffer[(uint16_t)(cgrAddress + y)] & cgramFlagDirty) f = 1;

		if (f)
		{

			LCDserialPort::chrOut(248 + x); // set CGRAM character

			for (y = 0; y < 8; y++)
			{

				f = CGRAMbuffer[(uint16_t)(cgrAddress + y)] & ~(cgramFlagDirty);
				LCDserialPort::chrOut(f); // write the character data to the character generator ram

			}

		}

	}

#endif // useParallaxSerialLCDmodule
	// clear dirty flag on every byte in the buffer
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress++) CGRAMbuffer[(uint16_t)(cgrAddress)] &= ~(cgramFlagDirty);

}

#endif // useLCDgraphics
static void LCD::setBrightness(uint8_t idx)
{

#ifdef use4BitLCD
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDshield
	if (idx) idx = EEPROM::readVal(pLCDcolorIdx); // get LCD backlight color

	setRGBcolor(idx); // set LCD backlight color

#else // useAdafruitRGBLCDshield
	if (idx) portLCD |= lcdBrightness; // turn on LCD backlight
	else portLCD &= ~(lcdBrightness); // turn off LCD backlight
#endif // useAdafruitRGBLCDshield
#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
	OCR1B = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD
#endif // use4BitLCD

}

#ifdef useLegacyLCD
static void LCD::setContrast(uint8_t idx)
{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
	OCR1A = idx;
#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	OCR0A = idx;
#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR0A = idx;
#endif // defined(__AVR_ATmega328P__)

}

#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t RGBcolors[8] PROGMEM =
{

	 0b11000001	// off
	,0b01000001	// green
	,0b10000001	// red
	,0b00000001	// yeller
	,0b11000000	// blue
	,0b01000000	// cyan
	,0b10000000	// magenta
	,0b00000000	// white

};

static void LCD::setRGBcolor(uint8_t idx)
{

	uint8_t RGBbitMask;
	uint8_t byt;

	RGBbitMask = pgm_read_byte(&RGBcolors[(unsigned int)(idx & 0x07)]); // read the LCD backlight color bitmask pattern

	byt = RGBbitMask; // get the color bitmask pattern
	byt ^= portSwitches; // flip the color bitmask by what's stored in the portSwitches register
	byt &= (lcdBrightnessRed | lcdBrightnessGreen); // strip out all but the relevant bits
	byt ^= portSwitches; // flip again - restores non-relevant bits and causes relevant bits to change according to RGBbitMask
	portSwitches = byt; // save the modified portSwitches register

	byt = RGBbitMask; // get the color bitmask
	byt ^= portLCD; // flip the color bitmask by what's stored in the portLCD register
	byt &= (lcdBrightnessBlue); // strip out all but the relevant bit
	byt ^= portLCD; // flip again - restores non-relevant bits and causes relevant bit to change according to RGBbitMask
	portLCD = byt; // save the modified portLCD register

#ifdef useTWIbuttons
	disableIntSample(); // disable TWI button sampling

#endif // useTWIbuttons
	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_OLATx, portSwitches, portLCD); // write out 16-bit register (which sets address mode to toggle)

	adafruitRGBLCDsupport::setTransferMode(adaTWIbyteMode); // set address mode to byte mode

#ifdef useTWIbuttons
	enableIntSample(); // enable TWI button sampling

#endif // useTWIbuttons
}

#endif // useAdafruitRGBLCDshield
#ifdef use4BitLCD
static const uint8_t lcdCharBackwardX =	0b10000000;
static const uint8_t lcdCharBackwardY =	0b01000000;
static const uint8_t lcdCharForwardX =	0b00100000;
static const uint8_t lcdCharForwardY =	0b00010000;
static const uint8_t lcdCharZeroX =		0b00001000;
static const uint8_t lcdCharZeroY =		0b00000100;
static const uint8_t lcdCharGotoXY =	0b00000010;
static const uint8_t lcdCharOutput =	0b00000001;

static const uint8_t lcdCharCalcXY =	(lcdCharBackwardX | lcdCharBackwardY | lcdCharForwardX | lcdCharForwardY | lcdCharZeroX | lcdCharZeroY | lcdCharGotoXY);

static const uint8_t lcdBaseYposition[] PROGMEM = {
	 lcdSetDDRAMaddress
	,lcdSetDDRAMaddress | 0x40
	,lcdSetDDRAMaddress | 0x14
	,lcdSetDDRAMaddress | 0x54
};

static const uint8_t lcdDisplayModes[] PROGMEM = {
	 lcdDisplayControl																		// turn off display
	,lcdDisplayControl | lcdDCdisplayShow													// turn on display, no cursor, no character blink (default)
	,lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorBlinkControl							// turn on display, no cursor, with character blink
	,lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorControl								// turn on display, cursor, no character blink
	,lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorControl | lcdDCcursorBlinkControl	// turn on display, cursor, with character blink
};

#endif // use4BitLCD
static void LCD::writeData(uint8_t value)
{

	uint8_t charFlags;
	uint8_t x;

	charFlags = 0;

	switch (value)
	{

		case 0x08: // go back one character
			charFlags |= (lcdCharGotoXY | lcdCharBackwardX);
			break;

		case 0x09:	// tab (go forward one character)
			charFlags |= (lcdCharGotoXY | lcdCharForwardX);
			break;

		case 0x0D: // carriage return with clreol
#ifdef blankScreenOnMessage
			if ((timer0Command & t0cDisplayDelay) == 0)
#else // blankScreenOnMessage
			if (((timer0Command & t0cDisplayDelay) == 0) || (LCDaddressY > 0))
#endif // blankScreenOnMessage
			{

				while (LCDaddressX < 20)
				{

#ifdef use4BitLCD
					writeByte(' ', lcdDataByte, lcdDelay0040us);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
					LCDserialPort::chrOut(' ');
#endif // useParallaxSerialLCDmodule
					LCDaddressX++;

				}

			}
			charFlags |= (lcdCharGotoXY | lcdCharZeroX);
		case 0x0A: // linefeed
			charFlags |= (lcdCharGotoXY | lcdCharForwardY);
			break;

		case 0x0C: // clear screen
#ifdef use4BitLCD
			writeCommand(lcdClearDisplay); // clear display, set cursor position to zero
			charFlags |= (lcdCharZeroX | lcdCharZeroY);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
			charFlags |= (lcdCharOutput | lcdCharZeroX | lcdCharZeroY);
#endif // useParallaxSerialLCDmodule
			break;

		case 0x80 ... 0xCF: // hijack the gotoxy characters
			value &= 0x7F;
			LCDaddressY = value / 20;
			LCDaddressX = value % 20;
			charFlags |= (lcdCharGotoXY);
			break;

#ifndef useBinaryLCDbrightness
		case 0x0F:	// LCD backlight full bright
		case 0x10:	// LCD backlight medium bright
#endif // useBinaryLCDbrightness
		case 0x11:	// turn on LCD backlight / LCD backlight low bright
		case 0x12:	// turn off LCD backlight
			brightnessIdx = 0x12 - value;
#ifdef use4BitLCD
			setBrightness(brightnessIdx);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
			charFlags |= (lcdCharOutput);
#endif // useParallaxSerialLCDmodule
			break;

		case 0x15:	// turn off display
		case 0x16:	// turn on display with no cursor and no blink (default)
		case 0x17:	// turn on display with no cursor and character blink
		case 0x18:	// turn on display with cursor and no blink
		case 0x19:	// turn on display with cursor and character blink
#ifdef use4BitLCD
			x = value - 0x15;
			writeCommand(pgm_read_byte(&lcdDisplayModes[(unsigned int)(x)])); // set display mode
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
			charFlags |= (lcdCharOutput);
#endif // useParallaxSerialLCDmodule
			break;

		case 0x0B: // undefined character
		case 0x0E: // undefined character
#ifdef useBinaryLCDbrightness
		case 0x0F ... 0x10:	// undefined character range
#endif // useBinaryLCDbrightness
		case 0x13 ... 0x14: // undefined character range
		case 0x1A ... 0x1F: // undefined character range
		case 0xE9 ... 0xF7: // undefined character range
		case 0xF8 ... 0xFF: // CGRAM definition stuff is processed elsewhere
			break;

		case 0x00 ... 0x07: // print defined CGRAM characters 0 through 7
		case 0x20 ... 0x7F: // print normal characters
#ifdef blankScreenOnMessage
			if (timer0Command & t0cDisplayDelay) charFlags |= (lcdCharGotoXY | lcdCharForwardX | lcdCharForwardY);
#else // blankScreenOnMessage
			if ((timer0Command & t0cDisplayDelay) && (LCDaddressY < 1)) charFlags |= (lcdCharGotoXY | lcdCharForwardX | lcdCharForwardY);
#endif // blankScreenOnMessage
			else charFlags |= (lcdCharOutput | lcdCharForwardX | lcdCharForwardY);
			break;

		case 0xD0 ... 0xE8: // apparently, the Parallax LCD can play tunes
			charFlags |= (lcdCharOutput);
			break;

		default:
			charFlags |= (lcdCharOutput);
			break;

	}

	if (charFlags & lcdCharForwardX)
	{

		LCDaddressX++;
		if (LCDaddressX < 20) charFlags &= ~(lcdCharForwardY);
		else charFlags |= (lcdCharZeroX);

	}

	if (charFlags & lcdCharForwardY)
	{

		LCDaddressY++;
		if (LCDaddressY > 3) charFlags |= (lcdCharZeroY);

	}

	if (charFlags & lcdCharZeroX) LCDaddressX = 0;
	if (charFlags & lcdCharZeroY) LCDaddressY = 0;

	if (charFlags & lcdCharBackwardX)
	{

		if (LCDaddressX) LCDaddressX--;
		else LCDaddressX = 19;

	}

	if (charFlags & lcdCharBackwardY)
	{

		if (LCDaddressY) LCDaddressY--;
		else LCDaddressY = 3;

	}

#ifdef use4BitLCD
	if (charFlags & lcdCharCalcXY) LCDgotoXYaddress = pgm_read_byte(&lcdBaseYposition[LCDaddressY]) + LCDaddressX;

#endif // use4BitLCD
	if (charFlags & lcdCharGotoXY)
	{

#ifdef use4BitLCD
		writeCommand(LCDgotoXYaddress);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
		LCDserialPort::chrOut(0x80 + LCDaddressY * 20 + LCDaddressX);
#endif // useParallaxSerialLCDmodule

	}

	if (charFlags & lcdCharOutput)
	{

#ifdef use4BitLCD
		writeByte(value, lcdDataByte, lcdDelay0040us);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
		LCDserialPort::chrOut(value);
		if (value == 0x0C) delay0(delay0005msTick); // wait for 5 ms
#endif // useParallaxSerialLCDmodule

	}

}

#ifdef use4BitLCD
static void LCD::writeCommand(uint8_t value)
{

	uint8_t delay;

	switch (value)
	{

		case lcdClearDisplay:
		case lcdReturnHome:
		case lcdReturnHome | lcdClearDisplay:
			delay = lcdDelay0015ms;
			break;

		default:
			delay = lcdDelay0040us;
			break;

	}

	writeByte(value, lcdCommandByte, delay);

}

static void LCD::writeByte(uint8_t value, uint8_t flags, uint8_t delay)
{

	flags |= lcdSendNybble;

	writeNybble(value, (flags | lcdDelay0040us)); // send the high nybble with standard 40 us delay
	writeNybble(value << 4, (flags | delay)); // send the low nybble with the specified delay

}

static void LCD::writeNybble(uint8_t value, uint8_t flags)
{

#ifdef useBufferedLCD
	ringBuffer::push(lcdBuffer, (value & 0xF0) | (flags & 0x0F));
#else // useBufferedLCD
	uint8_t oldSREG;

	while (timer1Command & t1cDelayLCD) idleProcess(); // wait for LCD timer delay to complete

#ifdef useTWILCD
	if (flags & lcdSendNybble)
	{

#ifdef useTWIbuttons
		disableIntSample(); // disable TWI button sampling
#endif // useTWIbuttons
		TWI::openChannelMain(lcdAddress, TW_WRITE); // open TWI as master transmitter
		TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address

	}

#endif // useTWILCD
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	outputNybble((value & 0xF0) | (flags & 0x0F));

	SREG = oldSREG; // restore interrupt flag status
#ifdef useTWILCD

	if (flags & lcdSendNybble)
	{

		TWI::transmitChannel(TWI_STOP); // commit LCD port expander write
#ifdef useTWIbuttons
		enableIntSample(); // enable TWI button sampling
#endif // useTWIbuttons

	}
#endif // useTWILCD
#endif // useBufferedLCD

	peripheral::changeBitFlags(timer1Command, 0, t1cDelayLCD); // enable LCD delay

}

//const unsigned long t1CyclesPerSecond = (unsigned long)(systemProcessorSpeed * 1000000ul); // (systemProcessorSpeed * 1000000 / (timer 1 prescaler))

//const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // initial LCD delay for 4-bit initialization
//const unsigned int delayLCD04100usTick = (unsigned int)(ceil)((double)(4100ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // secondary LCD delay for 4-bit initialization
//const unsigned int delayLCD00100usTick = (unsigned int)(ceil)((double)(100ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // final LCD delay for 4-bit initialization
//const unsigned int delayLCD00040usTick = (unsigned int)(ceil)((double)(40ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // normal LCD character transmission delay
const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * F_CPU) / (double)(510ul * 1000000ul)) - 1; // initial LCD delay for 4-bit initialization
const unsigned int delayLCD04100usTick = (unsigned int)(ceil)((double)(4100ul * F_CPU) / (double)(510ul * 1000000ul)) - 1; // secondary LCD delay for 4-bit initialization
const unsigned int delayLCD00100usTick = (unsigned int)(ceil)((double)(100ul * F_CPU) / (double)(510ul * 1000000ul)) - 1; // final LCD delay for 4-bit initialization
const unsigned int delayLCD00040usTick = (unsigned int)(ceil)((double)(40ul * F_CPU) / (double)(510ul * 1000000ul)); // normal LCD character transmission delay

static void LCD::outputNybble(uint8_t LCDchar)
{

	uint8_t x;

	if (LCDchar & lcdSendNybble)
	{

#ifdef useTWILCD
		portLCD &= ~(lcdRegisterSelect | lcdDirection | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) portLCD |= lcdRegisterSelect; // set nybble type (command or data)
		if (LCDchar & 0b10000000) portLCD |= lcdBit3; // set bit 3
		if (LCDchar & 0b01000000) portLCD |= lcdBit2; // set bit 2
		if (LCDchar & 0b00100000) portLCD |= lcdBit1; // set bit 1
		if (LCDchar & 0b00010000) portLCD |= lcdBit0; // set bit 0

		portLCD |= lcdEnable; // set enable high

		TWI::writeByte(portLCD); // write LCD port expander register

		portLCD &= ~(lcdEnable); // set enable low to 'tickle' enable bit (it will take TWI subsystem at least 22.5 us to write the update, which is larger than 50 ns required for enable 'tickling'

		TWI::writeByte(portLCD); // write LCD port expander register
#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
		PORTF &= ~(lcdDirection); // set data direction to write

		PORTE |= lcdEnable; // set enable high

		if (LCDchar & lcdDataByte) PORTF |= lcdRegisterSelect; // set nybble type (command or data)
		else PORTF &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTB |= lcdBit3; // set bit 3
		else PORTB &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTD |= lcdBit2; // set bit 2
		else PORTD &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTD |= lcdBit1; // set bit 1
		else PORTD &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTF |= lcdBit0; // set bit 0
		else PORTF &= ~(lcdBit0);

		PORTE &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
		PORTA |= lcdEnable; // set enable high

		PORTA &= ~(lcdRegisterSelect | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) PORTA |= lcdRegisterSelect; // set nybble type (command or data)
		else PORTA &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTA |= lcdBit3; // set bit 3
		else PORTA &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTA |= lcdBit2; // set bit 2
		else PORTA &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTA |= lcdBit1; // set bit 1
		else PORTA &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTA |= lcdBit0; // set bit 0
		else PORTA &= ~(lcdBit0);

		PORTA &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTD |= lcdEnable; // set enable high

		if (LCDchar & lcdDataByte) PORTD |= lcdRegisterSelect; // set nybble type (command or data)
		else PORTD &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTB |= lcdBit3; // set bit 3
		else PORTB &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTB |= lcdBit2; // set bit 2
		else PORTB &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTB |= lcdBit1; // set bit 1
		else PORTB &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTD |= lcdBit0; // set bit 0
		else PORTD &= ~(lcdBit0);

		PORTD &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD

	}

	x = LCDchar & lcdDelayFlags;

	switch (x) // lcdDelayCount is an accumulated tick count due to TWI output mechanism
	{

		case lcdDelay0015ms:
			lcdDelayCount += delayLCD15000usTick;
			break;

		case lcdDelay4100us:
			lcdDelayCount += delayLCD04100usTick;
			break;

		case lcdDelay0100us:
			lcdDelayCount += delayLCD00100usTick;
			break;

		case lcdDelay0040us:
			lcdDelayCount += delayLCD00040usTick;
			break;

		case lcdDataByte | lcdDelay0040us:
			lcdDelayCount += delayLCD00040usTick;
			break;

		default:
			lcdDelayCount += delayLCD04100usTick;
			break;

	}

}

#if useTWIbuttons && useTWILCD
static void LCD::disableIntSample(void)
{

	peripheral::changeBitFlags(timer0Command, t0cEnableTWIsample, 0); // disable TWI button sampling as it interferes with TWI LCD output
	while (twiStatusFlags & twiBlockMainProgram) idleProcess(); // wait for any in-progress TWI button samples to finish

}

static void LCD::enableIntSample(void)
{

	peripheral::changeBitFlags(timer0Command, 0, t0cEnableTWIsample); // re-enable TWI button sampling

}

#endif // useTWIbuttons && useTWILCD
#endif // use4BitLCD
#endif // useLCDoutput
#ifdef useBuffering
const uint8_t bufferIsFull =	0b10000000;
const uint8_t bufferIsEmpty =	0b01000000;

static void ringBuffer::init(ringBufferVariable &bfr, volatile uint8_t * storage)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	bfr.data = storage;
	bfr.size = sizeof(storage) / sizeof(storage[0]);
	bfr.start = 0;
	bfr.end = 0;
	bfr.status = bufferIsEmpty;

	SREG = oldSREG; // restore interrupt flag status

}

static uint8_t ringBuffer::isBufferNotEmpty(ringBufferVariable &bfr)
{

	return ((bfr.status & bufferIsEmpty) == 0);

}

static void ringBuffer::push(ringBufferVariable &bfr, uint8_t value)
{

	uint8_t oldSREG;

	while (bfr.status & bufferIsFull) idleProcess(); // wait for calling routine's buffer to become not full

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	pushInterrupt(bfr, value);

	SREG = oldSREG; // restore interrupt flag status

}

static void ringBuffer::pushInterrupt(ringBufferVariable &bfr, uint8_t value)
{

	bfr.data[(unsigned int)(bfr.start++)] = value; // save a buffered character

	if (bfr.status & bufferIsEmpty) bfr.status &= ~(bufferIsEmpty); // mark buffer as no longer empty
	if (bfr.start == bfr.size) bfr.start = 0; // handle wrap-around
	if (bfr.start == bfr.end) bfr.status |= (bufferIsFull); // test if buffer is full

}

static uint8_t ringBuffer::pull(ringBufferVariable &bfr)
{

	uint8_t value;

	if (bfr.status & bufferIsEmpty) value = 0; // if buffer is empty, return a NULL
	else
	{

		value = bfr.data[(unsigned int)(bfr.end++)]; // pull a buffered character

		if (bfr.status & bufferIsFull) bfr.status &= ~(bufferIsFull); // mark buffer as no longer full
		if (bfr.end == bfr.size) bfr.end = 0; // handle wrap-around
		if (bfr.end == bfr.start) bfr.status |= (bufferIsEmpty); // test if buffer is empty

	}

	return value;

}

static void ringBuffer::flush(ringBufferVariable &bfr)
{

	while ((bfr.status & bufferIsEmpty) == 0) idleProcess(); // wait for calling routine's buffer to become empty

}

#endif // useBuffering
static const uint8_t prgmEngineSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,		// load injector pulse count into register 2
	instrMul2byConst, idxCycles0PerSecond,				// set up for conversion of denominator injector cycle count to time in seconds
	instrMul2byByte, 60,								// set up for conversion of denominator injector time in seconds to time in minutes
	instrMul2byConst, idxDecimalPoint,					// perform output decimal formatting
	instrMul2byEEPROM, pCrankRevPerInjIdx,				// multiply by the number of crank revolutions per injector fire event
	instrDiv2byTripVarIndexed, rvEngCycleIdx,			// divide by the injector pulse time
	instrDone											// exit to caller
};

static const uint8_t prgmMotionTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrDiv2byConst, idxCycles0PerSecond,				// divide by cycles per second value
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

static const uint8_t prgmEngineRunTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,		// load injector pulse cycle value into register 2
	instrDiv2byConst, idxCycles0PerSecond,				// divide by cycles per second value
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

static const uint8_t prgmRangeTime[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmReserveTime[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmBingoTime[] PROGMEM = {
	instrLdRegMain, 0x02, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmReserveTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmBingoTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmCalculateFuelTime[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 10,								// if inputted fuel quantity is zero, exit to caller
	instrMul2byConst, idxMicroSecondsPerSecond,			// shift number upward to retain numeric precision
	instrDiv2byTripVarIndexed, rvInjCycleIdx,			// divide by open injector time in cycles
	instrMul2byTripVarIndexed, rvEngCycleIdx,			// multiply by injector open count
	instrDiv2byConst, idxCycles0PerSecond,				// convert output term from cycles remaining to seconds remaining
	instrDiv2byConst, idxMicroSecondsPerSecond,			// shift number downward - high numeric precision no longer needed

//cont:
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

static const uint8_t prgmDistance[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS cycle value into register 2
	instrMul2byConst, idxDecimalPoint,					// perform output decimal formatting
	instrDiv2byEEPROM, pPulsesPerDistanceIdx,			// divide by pulse per distance value and store in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrBranchIfZero, 14,								// if zero, then speed is also zero
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// set up to convert pulses per unit distance
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS pulse count
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term
	instrMul2byConst, idxCycles0PerSecond,				// set up to convert VSS cycle value to time in seconds
	instrMul2byConst, idxSecondsPerHour,				// set up to convert VSS time in seconds to time in hours
	instrDiv2by1,										// divide to obtain unit distance per hour

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmFuelUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// get amount of consumed fuel in cycles
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity value
};

static const uint8_t prgmRemainingFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel quantity value
};

static const uint8_t prgmReserveRemainingFuel[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the reserve fuel value
};

static const uint8_t prgmBingoRemainingFuel[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the bingo fuel value
};

static const uint8_t prgmCalculateFuelQuantity[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 4,								// if inputted fuel quantity is zero, exit to caller
	instrMul2by1,										// multiply the fuel used value by the formatting term
	instrDiv2byMain, mpCyclesPerVolumeIdx,				// convert fuel value in cycles to fuel value per unit quantity
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmRangeDistance[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmReserveDistance[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelDistance					// go format it
};

#ifdef useJSONoutput
static const uint8_t prgmFindHalfReserveRange[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,			// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRegRight, 0x02,							// shift result right one bit
	instrJump, tCalculateFuelDistance					// go format it
};

#endif // useJSONoutput
static const uint8_t prgmBingoDistance[] PROGMEM = {
	instrLdRegMain, 0x02, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmReserveDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmBingoDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmCalculateFuelDistance[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 18,								// if inputted fuel quantity is zero, exit to caller
	instrMul2byTripVarIndexed, rvVSSpulseIdx,			// multiply input fuel quantity by accumulated distance travelled pulses to get the numerator for distance to empty
	instrLdReg, 0x23,									// move numerator to register 3 for now
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycles
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply accumulated fuel injector open cycles by pulses per unit distance factor
	instrDiv2byConst, idxDecimalPoint,					// divide by decimal point constant to get denominator / decimal factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrLdReg, 0x21,									// move modified denominator to register 1 for now
	instrLdReg, 0x32,									// fetch numerator from register 3
	instrDiv2by1,										// divide to get DTE = (numerator) / (denominator / decimal factor ) = (numerator * decimal factor) / denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone											// exit to caller
};

#ifdef useFuelCost
static const uint8_t prgmFuelCostUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// get amount of consumed fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostTank[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostReserve[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostBingo[] PROGMEM = {
	instrLdRegMain, 0x02, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostRemaining[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

static const uint8_t prgmFuelCostReserveRemaining[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

static const uint8_t prgmFuelCostBingoRemaining[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

static const uint8_t prgmFuelCostPerDistance[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get the denominator for fuel cost per distance
	instrLdReg, 0x21,									// save denominator term
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel cost per distance
	instrMul2byEEPROM, pCostPerQuantity,				// multiply the numerator by the fuel cost per unit quantity
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

static const uint8_t prgmDistancePerFuelCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the denominator for distance per fuel cost
	instrMul2byEEPROM, pCostPerQuantity,				// multiply the denominator by the cost per unit quantity of fuel
	instrLdReg, 0x21,									// save the denominator term
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the numerator for distance per fuel cost
	instrMul2byConst, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2byConst, idxDecimalPoint,					// load the decimal point constant used for output formatting
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

#endif // useFuelCost
static const uint8_t prgmFuelRate[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector open cycle value
	instrBranchIfZero, 11,								// if zero, then fuel rate is also zero
	instrMul2byConst, idxSecondsPerHour,
	instrMul2byConst, idxDecimalPoint,
	instrDiv2byTripVarIndexed, rvEngCycleIdx,
	instrMul2byConst, idxCycles0PerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrDiv2byMain, mpCyclesPerVolumeIdx,				// multiply by quantity/cycle (divide by cycles/quantity)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone
};

#ifdef useFuelCost
static const uint8_t prgmFuelRateCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector open cycle value
	instrBranchIfZero, 11,								// if zero, then fuel rate is also zero
	instrMul2byConst, idxSecondsPerHour,
	instrMul2byEEPROM, pCostPerQuantity,
	instrDiv2byTripVarIndexed, rvEngCycleIdx,
	instrMul2byConst, idxCycles0PerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrDiv2byMain, mpCyclesPerVolumeIdx,				// multiply by quantity/cycle (divide by cycles/quantity)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone
};

#endif // useFuelCost
static const uint8_t prgmFuelEcon[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator for later
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino is in metric mode, skip ahead
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byConst, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or km/L)
	instrSkip, 2,										// go skip ahead

//fecont1:
	instrMul2byConst, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for L/100km or gal/100mi)

//fecont2:
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateRemainingTank[] PROGMEM = {
	instrLdRegTripVar, 0x01, tankIdx, rvInjCycleIdx,	// fetch tank quantity in injector open cycles
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
#ifdef usePartialRefuel
	instrSubMainFromX, 0x01, mpPartialRefuelTankSize,	// subtract calculated partial refuel size in injector open cycles
	instrBranchIfGT, 7,									// if calculated partial refuel size > tank quantity, exit with calculated tank size in result
#endif // usePartialRefuel
	instrSubYfromX, 0x12,								// subtract tank injector time in cycles from calculated tank size in cycles to get remaining fuel in cycles
	instrBranchIfLTorE,	3,								// if tank quantity <= calculated tank size, exit
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateRemainingReserve[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 8,								// if zero, then reserve fuel is also zero
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo tank size in injector open cycles
	instrBranchIfLTorE, 3,								// if remaining fuel >= bingo tank size, exit to caller
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateBingoFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in injector open cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 9,								// if zero, then bingo fuel is also zero
	instrLdRegMain, 0x01, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if tank quantity in injector open cycles <= calculated bingo size in injector open cycles, exit to caller
	instrLdReg, 0x12,									// shift calculated bingo size in injector open cycles to register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmInjectorOpenTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,
	instrJump, tConvertToMicroSeconds
};

static const uint8_t prgmInjectorTotalTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,
	instrJump, tConvertToMicroSeconds
};

static const uint8_t prgmVSStotalTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrJump, tConvertToMicroSeconds
};

static const uint8_t prgmConvertToMicroSeconds[] PROGMEM = {
	instrMul2byConst, idxMicroSecondsPerSecond,			// multiply by conversion factor for microseconds per second
	instrDiv2byConst, idxCycles0PerSecond,				// divide by conversion factor for cycles to seconds
	instrDone											// exit to caller
};

static const uint8_t prgmVSSpulseCount[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,
	instrMul2byConst, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrDone
};

static const uint8_t prgmInjectorPulseCount[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,
	instrMul2byConst, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrDone
};

#ifdef useDebugAnalog
static const uint8_t prgmAnalogChannel[] PROGMEM = {
	instrCmpIndex, dfMaxValAnalogCount,					// is trip index pointing to a valid analog channel?
	instrBranchIfLT, 4,									// if so, skip ahead
	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

//cont1:
	instrLdRegVoltageIndexed, 0x02,						// load analog channel ADC step value
	instrMul2byConst, idxDenomVoltage,					// perform multiply (we're converting from ADC steps to voltage)
	instrDiv2byConst, idxNumerVoltage,					// divide by numerator to convert from voltage to ADC steps
	instrDone											// exit to caller
};

#endif // useDebugAnalog
#ifdef useCarVoltageOutput
static const uint8_t prgmAlternatorChannel[] PROGMEM = {
	instrLdRegVoltage, 0x02, analogAlternatorChannelIdx,	// load analog channel ADC step value
	instrMul2byConst, idxDenomVoltage,					// perform multiply (we're converting from ADC steps to voltage)
	instrDiv2byConst, idxNumerVoltage,					// divide by numerator to get voltage drop V(R5) between R5 and ground
	instrLdRegConst, 0x01, idxResistanceR5,				// figure out V(R5+R6) from V(R5)
	instrAddConstToX, 0x01, idxResistanceR6,
	instrMul2by1,
	instrDiv2byConst, idxResistanceR5,
	instrAddEEPROMtoX, 0x02, pVoltageOffset,			// add diode offset
	instrDone											// exit to caller
};

#endif // useCarVoltageOutput
static const uint8_t prgmRoundOffNumber[] PROGMEM = {
	instrTestReg, 0x02,									// test register 2
	instrBranchIfOverflow, 23,							// if register 2 has overflow value, exit
	instrCmpIndex, 1,									// check if 0 or 1 right-hand digits were specified
	instrBranchIfLT, 11,								// branch if 0 right-hand digits were specified
	instrBranchIfE, 14,									// branch if 1 right-hand digit was specified
	instrCmpIndex, 2,									// check if 2 right-hand digits were specified
	instrBranchIfGT, 13,								// skip if 3 or more right-hand digits were specified
	instrAddByteToX, 0x02, 5,							// round off to nearest 1/100th
	instrSkip, 8,										// skip to exit

//roundint:
	instrAddConstToX, 0x02, idxNumber500,				// round off to nearest whole integer
	instrSkip, 3,										// skip to exit

//round10th:
	instrAddByteToX, 0x02, 50,							// round off to nearest 1/10th

//exit:
	instrJump, tFormatToNumber							// go call prgmFormatToNumber to perform actual formatting
};

static const uint8_t prgmFormatToNumber[] PROGMEM = {
	instrTestReg, 0x02,									// test register 2
	instrBranchIfOverflow, 13,							// if register 2 has overflow value, exit
	instrCmpIndex, 3,									// check if valid number of decimal points were requested
	instrBranchIfGT, 9,									// if too many decimal points were requested, error out with overflow
	instrDiv2byConst, idxBCDdivisor,					// divide by 10^8 - quotient remains in register 2, and remainder goes into register 1
	instrLdRegByte, 0x03, 99,
	instrCmpXtoY, 0x23,									// test if quotient is greater than 99 (remainder will be less than 10^8)
	instrBranchIfLTorE, 5,								// if quotient is 99 or less, continue with processing

//overflow:
	instrStByteToRegU8, 0x03, 255, 6,					// signal overflow by storing total length of 255 into byte 6 of register 3
	instrDone,

//cont:
	instrStByteToRegU8, 0x03, 5, 6,						// store total BCD length into byte 6 of register 3
	instrStByteToRegU8, 0x03, 32, 7,					// store leading zero character into byte 7 of register 3
	instrStByteToRegU8, 0x03, 100, 1,					// store BCD divisor into byte 1 of register 3
	instrStByteToRegU8, 0x03, 100, 2,					// store BCD divisor into byte 2 of register 3
	instrStByteToRegU8, 0x03, 100, 3,					// store BCD divisor into byte 3 of register 3
	instrStByteToRegU8, 0x03, 100, 4,					// store BCD divisor into byte 4 of register 3
	instrDoBCDadjust, 0x13,								// go turn register 1 into a BCD string and store it in register 3
	instrDone											// exit to caller
};

static const uint8_t prgmFetchParameterValue[] PROGMEM = {
	instrLdRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

static const uint8_t prgmWriteParameterValue[] PROGMEM = {
	instrStRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

static const uint8_t prgmFetchInitialParamValue[] PROGMEM = {
	instrLdRegEinitIndexed, 0x02,
	instrDone											// return to caller
};

#ifdef useEEPROMtripStorage
static const uint8_t prgmLoadCurrentFromEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pCurrTripSignatureIdx,
	instrLdRegByte, 0x02, guinosig,
	instrCmpXtoY, 0x21,
	instrBranchIfE, 4,
	instrLdRegByte, 0x02, 0,
	instrDone,

//cont:
	instrLxdI, EEPROMcurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, currentIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, EEPROMeocIdleCurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, eocIdleCurrentIdx,
#endif // trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLdRegByte, 0x02, 1,
	instrDone
};

static const uint8_t prgmSaveCurrentToEEPROM[] PROGMEM = {
	instrLdRegByte, 0x02, guinosig,
	instrStRegEEPROM, 0x02, pCurrTripSignatureIdx,
	instrLxdI, currentIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMcurrentIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, eocIdleCurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMeocIdleCurrentIdx,
#endif // trackIdleEOCdata
	instrJump, tSaveTrip,
};

static const uint8_t prgmLoadTankFromEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pTankTripSignatureIdx,
	instrLdRegByte, 0x02, guinosig,
	instrCmpXtoY, 0x21,
	instrBranchIfE, 4,
	instrLdRegByte, 0x02, 0,
	instrDone,

//cont:
#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSaveSizeIdx,
	instrStRegEEPROM, 0x02, pRefuelSizeIdx,
#endif // usePartialRefuel
	instrLxdI, EEPROMtankIdx,
	instrCall, tLoadTrip,
	instrLxdI, tankIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, EEPROMeocIdleTankIdx,
	instrCall, tLoadTrip,
	instrLxdI, eocIdleTankIdx,
#endif // trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLdRegByte, 0x02, 1,
	instrDone
};

static const uint8_t prgmSaveTankToEEPROM[] PROGMEM = {
	instrLdRegByte, 0x02, guinosig,
	instrStRegEEPROM, 0x02, pTankTripSignatureIdx,
#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,
	instrStRegEEPROM, 0x02, pRefuelSaveSizeIdx,
#endif // usePartialRefuel
	instrLxdI, tankIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMtankIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, eocIdleTankIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMeocIdleTankIdx,
#endif // trackIdleEOCdata
	instrJump, tSaveTrip,
};

#endif // useEEPROMtripStorage
static const uint8_t prgmLoadTrip[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x01, rvVSSpulseIdx,
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdRegTripVarIndexed, 0x03, rvInjPulseIdx,
	instrLdRegTripVarIndexed, 0x04, rvInjCycleIdx,
	instrLdRegTripVarIndexed, 0x05, rvEngCycleIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmSaveTrip[] PROGMEM = {
	instrStRegTripVarIndexed, 0x01, rvVSSpulseIdx,
	instrStRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrStRegTripVarIndexed, 0x03, rvInjPulseIdx,
	instrStRegTripVarIndexed, 0x04, rvInjCycleIdx,
	instrStRegTripVarIndexed, 0x05, rvEngCycleIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmReadTicksToSeconds[] PROGMEM = {
	instrLdRegVolatileIndexed, 0x02,
	instrDiv2byConst, idxTicksPerSecond,
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

#ifdef useBarFuelEconVsTime
static const uint8_t prgmFEvTgetDistance[] PROGMEM = {
	instrLdRegTripFEvTindexed, 0x02, rvVSSpulseIdx,		// load indexed VSS pulses from fuel econ vs time trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvTgetConsumedFuel[] PROGMEM = {
	instrLdRegTripFEvTindexed, 0x02, rvInjCycleIdx,		// load indexed injector cycles from fuel econ vs time trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvTgetFuelEconomy[] PROGMEM = {
	instrLdRegTripFEvTindexed, 0x02, rvVSSpulseIdx,		// load indexed VSS pulses from fuel econ vs time trip variable bank
	instrBranchIfE, 26,
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator (for G/100mi or L/100km) for later
	instrLdRegTripFEvTindexed, 0x02, rvInjCycleIdx,		// load indexed injector cycles from fuel econ vs time trip variable bank
	instrBranchIfE, 22,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino currently outputting fuel econ as fuel over distance, skip
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byConst, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or k/L)
	instrSkip, 2,										// skip ahead

//cont0:
	instrMul2byConst, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for G/100mi or L/100km)
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrClearFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags clear, to signal good data
	instrDone,											// return to caller

//zero0:
	instrBranchIfFuelOverDist, 5,						// if MPGuino currently outputting fuel econ as fuel over distance, denominator is zero
//zero:
	instrClearFlag, SWEET64overflowFlag,				// clear V flag since this would have divided just fine otherwise
	instrDone,											// return to caller

//ovfl0:
	instrBranchIfFuelOverDist, 249,						// if MPGuino currently outputting fuel econ as fuel over distance, numerator is zero
//ovfl:
	instrSetFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags set, to signal unusable data (division by 0 would have resulted)
	instrDone											// return to caller
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t prgmFEvSgetDistance[] PROGMEM = {
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvVSSpulseIdx,	// load indexed VSS pulses from fuel econ vs speed trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvSgetConsumedFuel[] PROGMEM = {
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvInjCycleIdx,	// load indexed injector cycles from fuel econ vs speed trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvSgetFuelEconomy[] PROGMEM = {
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvVSSpulseIdx,	// load indexed VSS pulses from fuel econ vs speed trip variable bank
	instrBranchIfE, 27,
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator (for G/100mi or L/100km) for later
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvInjCycleIdx,	// load indexed injector cycles from fuel econ vs speed trip variable bank
	instrBranchIfE, 22,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino currently outputting fuel econ as fuel over distance, skip
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byConst, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or k/L)
	instrSkip, 2,										// skip ahead

//cont0:
	instrMul2byConst, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for G/100mi or L/100km)
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrClearFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags clear, to signal good data
	instrDone,											// return to caller

//zero0:
	instrBranchIfFuelOverDist, 5,						// if MPGuino currently outputting fuel econ as fuel over distance, denominator is zero
//zero:
	instrClearFlag, SWEET64overflowFlag,				// clear V flag since this would have divided just fine otherwise
	instrDone,											// return to caller

//ovfl0:
	instrBranchIfFuelOverDist, 249,						// if MPGuino currently outputting fuel econ as fuel over distance, numerator is zero
//ovfl:
	instrSetFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags set, to signal unusable data (division by 0 would have resulted)
	instrDone											// return to caller
};

#endif // useBarFuelEconVsSpeed
#ifdef useDebugTerminal
static const uint8_t prgmTerminalWriteParameterValue[] PROGMEM = {
	instrStRegEEPROMindexed, 0x06,
	instrDone											// return to caller
};

static const uint8_t prgmParseCharacterToReg[] PROGMEM = {
	instrAddIndexToX, 0x02,								// add whatever's in the trip variable index to main register
	instrLdReg, 0x26,									// load main register into terminal register
	instrLdRegByte, 0x01, 0xFF,							// is parsed number less than 256?
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,
	instrLdReg, 0x12,
	instrDone											// exit to caller
};

#endif // useDebugTerminal
static const uint8_t prgmFormatToTime[] PROGMEM = {
	instrLdReg, 0x21,									// move time in seconds into register 1
	instrStByteToRegU8, 0x03, 3, 6,						// store total BCD length into byte 6 of register 3
	instrStByteToRegU8, 0x03, 48, 7,					// store leading zero character into byte 7 of register 3
	instrStByteToRegU8, 0x03, 60, 2,					// store hours divisor into byte 2 of register 3
	instrStByteToRegU8, 0x03, 60, 1,					// store minutes divisor into byte 1 of register 3
	instrStByteToRegU8, 0x03, 24, 0,					// store seconds divisor into byte 0 of register 3
	instrDoBCDadjust, 0x13,								// go turn register 1 into a BCD string and store it in register 3
	instrDone											// exit to caller
};

#ifdef useChryslerMAPCorrection
static const uint8_t prgmPressureChannel[] PROGMEM = {
	instrCmpIndex, mpInjectorCorrectionIdx - mpMAPpressureIdx,	// is trip index pointing to a valid pressure element?
	instrBranchIfLT, 4,									// if so, skip ahead
	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

//cont1:
	instrLdRegMainOffset, 0x02, mpMAPpressureIdx,		// load pre-formatted pressure element
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateMAPpressure[] PROGMEM = {
	instrLdRegVoltage, 0x02, analogMAPchannelIdx,		// load analog channel ADC step value
	instrSubMainFromX, 0x02, mpAnalogMAPfloorIdx,		// is reading below MAP sensor voltage floor?
	instrBranchIfGTorE, 3,								// if not, continue
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//cont1:
	instrMul2byMain, mpAnalogMAPnumerIdx,				// perform conversion to get pressure units per volts value
	instrDiv2byMain, mpAnalogMAPdenomIdx,				// divide by pressure units per volts value
	instrAddEEPROMtoX, 0x02, pMAPsensorOffsetIdx,		// add pressure offset value from EEPROM
	instrStRegMain, 0x02, mpMAPpressureIdx,				// store resulting MAP sensor reading
#ifdef useChryslerBaroSensor
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateBaroPressure[] PROGMEM = {
	instrLdRegVoltage, 0x02, analogBaroChannelIdx,		// load analog channel ADC step value
	instrSubMainFromX, 0x02, mpAnalogBaroFloorIdx,		// is reading below barometric sensor voltage floor?
	instrBranchIfGTorE, 3,								// if not, continue
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//cont1:
	instrMul2byMain, mpAnalogBaroNumerIdx,				// convert to obtain pressure units per volts value
	instrDiv2byMain, mpAnalogBaroDenomIdx,				// divide by pressure units per volts value
	instrAddEEPROMtoX, 0x02, pBaroSensorOffsetIdx,		// add pressure offset value from EEPROM
	instrStRegMain, 0x02, mpBaroPressureIdx,			// store resulting barometric sensor reading
#endif // useChryslerBaroSensor
	instrLdRegMain, 0x02, mpFuelPressureIdx,			// get fuel system differential pressure
	instrAddMainToX, 0x02, mpBaroPressureIdx,			// add to reference barometric pressure to get fuel system absolute pressure
	instrSubMainFromX, 0x02, mpMAPpressureIdx,			// subtract MAP to get differential pressure across the fuel injector
	instrStRegMain, 0x02, mpInjPressureIdx,				// store differential pressure across the fuel injector
	instrMul2byConst, idxCorrectionFactor,				// set up for iSqrt
	instrDiv2byMain, mpFuelPressureIdx,					// divide by the fuel system differential pressure
	instrTestReg, 0x02,									// test whether overflow occurred
	instrBranchIfOverflow, 6,							// if overflow occurred, go handle it
	instrIsqrt, 0x02,									// perform square root on result
	instrStRegMain, 0x02, mpInjectorCorrectionIdx,		// save square root of presssure differential ratio as fuel injector correction factor
	instrDone,											// return to caller

//cont3:
	instrLdRegConst, 0x02, idxCorrectionFactor,
	instrStRegMain, 0x02, mpInjectorCorrectionIdx,		// save initial injector correction index for pressure differential calculation
	instrDone											// return to caller
};

#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
static const uint8_t prgmDragSpeed[] PROGMEM = {
	instrLdRegVolatile, 0x02, vDragInstantSpeedIdx,		// load instantaneous drag speed measurement
	instrTestReg, 0x02,									// test speed measurement
	instrBranchIfZero, 12,								// if speed measurement is zero, exit to caller
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// set up to convert pulses per unit distance
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// set up to convert VSS cycle value to time in seconds
	instrMul2byConst, idxDecimalPoint,					// load decimal formatting term
	instrMul2byConst, idxSecondsPerHour,				// set up to convert VSS time in seconds to time in hours
	instrDiv2by1,										// divide to obtain unit distance per hour

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmEstimateEnginePower[] PROGMEM = {
	instrCall, tDragSpeed,								// calculate vehicle speed (this will be the maximum vehicle speed obtained during the acceleration test)
	instrBranchifSAEmode, 5,							// if MPGuino is in SAE mode, skip to power calculation setup

//cont1:
	instrMul2byConst, idxDenomDistance,					// multiply by numerator to convert km/hr to MPH
	instrDiv2byConst, idxNumerDistance					// divide by denominator to convert km/hr to MPH
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont2:
	instrLdReg, 0x21,									// load register 1 with calculated vehicle speed
	instrMul2by1,										// perform cube function on maximum accel test vehicle speed value to obtain initial denominator term
	instrMul2by1,
	instrDiv2byConst, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byEEPROM, pVehicleMassIdx,					// multiply estimate by vehicle weight
	instrDiv2byConst, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrBranchifSAEmode, 5,							// if MPGuino is in SAE mode, skip to power calculation setup

//cont3:
	instrMul2byConst, idxDenomMass,						// multiply by numerator to convert kg to lbf
	instrDiv2byConst, idxNumerMass,						// divide by denominator to convert kg to lbf
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont4:
	instrDiv2byConst, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmAccelTestTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrMul2byConst, idxDecimalPoint,					// load the decimal point constant used for output formatting
	instrDiv2byConst, idxCycles0PerSecond,
	instrDone
};

#endif // useDragRaceFunction
const char overFlowStr[] PROGMEM = "----------";
const char overFlow9Str[] PROGMEM = "9999999999";

const uint8_t dfAdjustWindow =		0b00000001;
const uint8_t dfOverflow9s =		0b10000000;

#ifdef useEEPROMtripStorage
static uint8_t tripVar::getBaseEEPROMaddress(uint8_t tripIdx, uint8_t dataIdx)
{

	uint8_t retVal;

	if (dataIdx < rvMeasuredCount) switch (tripIdx)
		{

			case EEPROMcurrentIdx:
				retVal = pCurrTripVSSpulseIdx + dataIdx;
				break;

			case EEPROMtankIdx:
				retVal = pTankTripVSSpulseIdx + dataIdx;
				break;

#ifdef trackIdleEOCdata
			case EEPROMeocIdleCurrentIdx:
				retVal = pCurrIEOCvssPulseIdx + dataIdx;
				break;

			case EEPROMeocIdleTankIdx:
				retVal = pTankIEOCvssPulseIdx + dataIdx;
				break;

#endif // trackIdleEOCdata
			default:
				retVal = 0;
				break;

		}
	else retVal = 0;

	return retVal;

}

#endif // useEEPROMtripStorage
static void tripVar::reset(uint8_t tripIdx)
{

	if (tripIdx < tripSlotCount)
	{

		collectedVSSpulseCount[(uint16_t)(tripIdx)] = 0;
		SWEET64::init64byt((union union_64 *)&collectedInjCycleCount[(uint16_t)(tripIdx)], 0);

		if (tripIdx < tripSlotFullCount)
		{

			collectedInjPulseCount[(uint16_t)(tripIdx)] = 0;
			SWEET64::init64byt((union union_64 *)&collectedVSScycleCount[(uint16_t)(tripIdx)], 0);
			SWEET64::init64byt((union union_64 *)&collectedEngCycleCount[(uint16_t)(tripIdx)], 0);

		}

	}

}

static void tripVar::transfer(uint8_t srcTripIdx, uint8_t destTripIdx)
{

	if (destTripIdx < tripSlotCount)
	{

		if (srcTripIdx < tripSlotCount)
		{

			collectedVSSpulseCount[(uint16_t)(destTripIdx)] = collectedVSSpulseCount[(uint16_t)(srcTripIdx)];
			SWEET64::copy64((union union_64 *)&collectedInjCycleCount[(uint16_t)(destTripIdx)], (union union_64 *)&collectedInjCycleCount[(uint16_t)(srcTripIdx)]);

			if (destTripIdx < tripSlotFullCount)
			{

				if (srcTripIdx < tripSlotFullCount)
				{

					collectedInjPulseCount[(uint16_t)(destTripIdx)] = collectedInjPulseCount[(uint16_t)(srcTripIdx)];
					SWEET64::copy64((union union_64 *)&collectedVSScycleCount[(uint16_t)(destTripIdx)], (union union_64 *)&collectedVSScycleCount[(uint16_t)(srcTripIdx)]);
					SWEET64::copy64((union union_64 *)&collectedEngCycleCount[(uint16_t)(destTripIdx)], (union union_64 *)&collectedEngCycleCount[(uint16_t)(srcTripIdx)]);

				}
				else
				{

					collectedInjPulseCount[(uint16_t)(destTripIdx)] = 0;
					SWEET64::init64byt((union union_64 *)&collectedVSScycleCount[(uint16_t)(destTripIdx)], 0);
					SWEET64::init64byt((union union_64 *)&collectedEngCycleCount[(uint16_t)(destTripIdx)], 0);

				}

			}

		}

	}

}

static void tripVar::update(uint8_t srcTripIdx, uint8_t destTripIdx)
{

	if (destTripIdx < tripSlotCount)
	{

		if (srcTripIdx < tripSlotCount)
		{

			tripVar::add32(collectedVSSpulseCount, srcTripIdx, destTripIdx);
			tripVar::add64(collectedInjCycleCount, srcTripIdx, destTripIdx);

			if (destTripIdx < tripSlotFullCount)
			{

				if (srcTripIdx < tripSlotFullCount)
				{

					tripVar::add32(collectedInjPulseCount, srcTripIdx, destTripIdx);
					tripVar::add64(collectedVSScycleCount, srcTripIdx, destTripIdx);
					tripVar::add64(collectedEngCycleCount, srcTripIdx, destTripIdx);

				}

			}

		}

	}

}

static void tripVar::add32(uint32_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"   add %A0, %A1            \n"		// 0
		"   adc %B0, %B1            \n"		// 1
		"   adc %C0, %C1            \n"		// 2
		"   adc %D0, %D1            \n"		// 3
		: "+r" (collectedArray[(uint16_t)(destTripIdx)])
		: "r" (collectedArray[(uint16_t)(srcTripIdx)])
	);
#else // useAssemblyLanguage
	collectedArray[(uint16_t)(destTripIdx)] += collectedArray[(uint16_t)(srcTripIdx)];
#endif // useAssemblyLanguage

}

static void tripVar::add64(uint64_t collectedArray[], uint32_t value, uint8_t destTripIdx)
{

#ifdef useAssemblyLanguage
	union union_64 * an;

	an = (union union_64 *)(&collectedArray[(uint16_t)(destTripIdx)]);

	uint8_t x;

	asm volatile(
		"	ld	__tmp_reg__, %a0	\n"		// 0
		"   add __tmp_reg__, %A2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a0	\n"		// 1
		"   adc __tmp_reg__, %B2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a0	\n"		// 2
		"   adc __tmp_reg__, %C2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a0	\n"		// 3
		"   adc __tmp_reg__, %D2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ldi	%A1, 4				\n"		// initialize counter

		"l_add64a%=:				\n"
		"	ld	__tmp_reg__, %a0	\n"		// 4
		"   adc __tmp_reg__, __zero_reg__    \n"
		"	st	%a0+, __tmp_reg__	\n"
		"	dec	%A1					\n"
		"	brne l_add64a%=			\n"

		: "+e" (an), "+r" (x)
		: "r" (value)

	);
#else // useAssemblyLanguage
	collectedArray[(uint16_t)(destTripIdx)] += value;
#endif // useAssemblyLanguage

}

static void tripVar::add64(uint64_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx)
{

	union union_64 * an = (union union_64 *)(&collectedArray[(uint16_t)(destTripIdx)]);
	union union_64 * ann = (union union_64 *)(&collectedArray[(uint16_t)(srcTripIdx)]);

	uint8_t x;
	uint8_t c;
#ifdef useAssemblyLanguage

	asm volatile(
		"	clc						\n"
		"	ldi	%A2, 8				\n"		// initialize counter

		"l_add64b%=:				\n"
		"	ld	__tmp_reg__, %a0	\n"
		"	ld	%A1, %a3+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	dec	%A2					\n"
		"	brne l_add64b%=			\n"
		: "+e" (an), "+r" (c), "+r" (x)
		: "e" (ann)
	);
#else // useAssemblyLanguage
	unsigned int enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[0] = c;
		n->u8[1] = 0;
		enn += an->u8[(uint16_t)(x)];
		enn += ann->u8[(uint16_t)(x)];
		an->u8[(uint16_t)(x)] = n->u8[0];
		c = n->u8[1];

	}
#endif // useAssemblyLanguage

}

#ifdef useSWEET64mult
static const uint8_t prgmMult64[] PROGMEM = {
	instrLdReg, 0x24,									// load multiplier into register 4
	instrLdRegByte, 0x02, 0,							// zero out result (register 2)
	instrCmpXtoY, 0x54,									// is multiplicand greater than multiplier?
	instrBranchIfGT, 12,								// if so, go to multipler test
	instrSwapReg, 0x54,									// otherwise, swap the two so that multiplier is smaller (eg it's faster to add 3 10s than it is to add 10 3s)
	instrSkip, 8,										// skip forward to multiplier test

//multloop:
	instrShiftRegRight, 0x04,							// shift multiplier right by one bit
	instrBranchIfCclear, 2,								// if multiplier bit was clear, don't add multiplicand to result
	instrAddYtoX, 0x52,									// add multiplicand to result

//shiftmultiplicand:
	instrShiftRegLeft, 0x05,							// shift multiplicand left by one bit

//testmultiplier:
	instrTestReg, 0x04,									// test multiplier register
	instrBranchIfNotZero, 244,							// if multiplier is not zero, loop back
	instrTraceDone										// exit to caller
};

#endif // useSWEET64mult
#ifdef useSWEET64div
static const uint8_t prgmDiv64[] PROGMEM = {
	instrLdReg, 0x21,									// initialize remainder with dividend
	instrLdRegByte, 0x04, 1,							// load quotient bitmask with a 1
	instrTestReg, 0x05,									// test divisor register
	instrBranchIfNotZero, 11,							// if divisor is not zero, skip ahead

//divoverflow:
	instrLdRegByte, 0x02, 0,							// zero out quotient
	instrSubYfromX, 0x42,								// set overflow value in quotient
	instrLdReg, 0x21,									// set overflow value in remainder

//divzero:
	instrLdRegByte, 0x05, 0,							// zero out divisor
	instrTraceDone,										// exit to caller

//testdividend:
	instrTestReg, 0x02,									// test dividend register
	instrBranchIfZero, 248,								// if dividend is zero, go exit
	instrLdRegByte, 0x02, 0,							// initialize quotient (register 2)
	instrTestReg, 0x05,									// test dividend register
	instrBranchIfMinus, 6,								// if dividend is already fully shifted left, skip

//divloop0:
	instrShiftRegLeft, 0x04,							// shift quotient bitmask left one bit
	instrShiftRegLeft, 0x05,							// shift divisor left one bit
	instrBranchIfPlus, 250,								// if dividend is not fully shifted left, loop back

//divcomp:
	instrCmpXtoY, 0x51,									// compare divisor to dividend
	instrBranchIfGT, 4,									// if divisor is greater than dividend, skip to bitmask adjustment
	instrSubYfromX, 0x51,								// subtract divisor from dividend
	instrAddYtoX, 0x42,	       							// add quotient bitmask to quotient

//divnext:
	instrShiftRegRight, 0x04,							// shift quotient bitmask right by one bit
	instrBranchIfCset, 4,								// if quotient bitmask empty, skip ahead
	instrShiftRegRight, 0x05,							// shift divisor right by one bit
	instrBranchIfNotZero, 240,							// go back to quotient bitmask test

//divexit:
	instrShiftRegLeft, 0x01,							// shift remainder left one bit to multiply by 2
	instrCmpXtoY, 0x51,									// compare divisor to 2*remainder
	instrBranchIfLTorE, 5,								// if divisor is less than or equal to 2*remainder, skip ahead
	instrLdRegByte, 0x05, 0,							// zero out divisor
	instrSkip, 3,										// skip ahead to restore remainder

//divadjust1:
	instrLdRegByte, 0x05, 1,							// load divisor with a 1

//divexit2:
	instrShiftRegRight, 0x01,							// shift remainder right one bit to restore it
	instrTraceDone										// exit to caller
};

#endif // useSWEET64div
/* 64-bit pseudo-processor section */

const uint8_t * const S64programList[] PROGMEM = {
// these SWEET64 program addresses correspond to the display functions to either screen or logging output
	 prgmEngineRunTime									// tEngineRunTime - engine runtime (hhmmss)
	,prgmRangeTime										// tRangeTime - estimated total runtime from full tank (hhmmss)
	,prgmReserveTime									// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	,prgmBingoTime										// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	,prgmTimeToEmpty									// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	,prgmReserveTimeToEmpty								// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	,prgmBingoTimeToEmpty								// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	,prgmMotionTime										// tMotionTime - time vehicle in motion (hhmmss)
	,prgmInjectorOpenTime								// tInjectorOpenTime - fuel used (microseconds)
	,prgmInjectorTotalTime								// tInjectorTotalTime - engine run time (microseconds)
	,prgmVSStotalTime									// tVSStotalTime - time vehicle in motion (microseconds)
	,prgmEngineSpeed									// tEngineSpeed - engine speed (1/m)
	,prgmInjectorPulseCount								// tInjectorPulseCount - fuel injector pulse count
	,prgmVSSpulseCount									// tVSSpulseCount - VSS pulse count
#ifdef useFuelCost
	,prgmFuelCostUsed									// tFuelCostUsed - cost of fuel quantity used
	,prgmFuelCostTank									// tFuelCostTank - full tank fuel cost in currency units
	,prgmFuelCostReserve								// tFuelCostReserve - reserve fuel quantity fuel cost in currency units
	,prgmFuelCostBingo									// tFuelCostBingo - bingo fuel quantity cost in currency units
	,prgmFuelCostRemaining								// tFuelCostRemaining - value of estimated remaining total fuel quantity in currency units
	,prgmFuelCostReserveRemaining						// tFuelCostReserveRemaining - value of estimated remaining reserve fuel quantity in currency units
	,prgmFuelCostBingoRemaining							// tFuelCostBingoRemaining - value of estimated remaining bingo fuel quantity in currency units
	,prgmFuelRateCost									// tFuelRateCost - fuel rate cost in currency units
#endif // useFuelCost
#ifdef useDebugAnalog
	,prgmAnalogChannel									// tAnalogChannel - DC voltage
#endif // useDebugAnalog
#ifdef useCarVoltageOutput
	,prgmAlternatorChannel								// tAlternatorChannel - DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	,prgmAccelTestTime									// tAccelTestTime - acceleration test time (s.s)
#endif // useDragRaceFunction
	,prgmFuelUsed										// tFuelUsed - fuel quantity used (SI/SAE)
	,prgmFuelRate										// tFuelRate - fuel consumption rate (SI/SAE)
	,prgmDistance										// tDistance - vehicle distance traveled (SI/SAE)
	,prgmSpeed											// tSpeed - vehicle speed (SI/SAE)
	,prgmRemainingFuel									// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	,prgmReserveRemainingFuel							// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	,prgmBingoRemainingFuel								// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
	,prgmRangeDistance									// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	,prgmReserveDistance								// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	,prgmBingoDistance									// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	,prgmDistanceToEmpty								// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	,prgmReserveDistanceToEmpty							// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	,prgmBingoDistanceToEmpty							// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
#ifdef useFuelCost
	,prgmFuelCostPerDistance							// tFuelCostPerDistance - fuel cost per unit distance (SI/SAE)
	,prgmDistancePerFuelCost							// tDistancePerFuelCost - distance per unit fuel cost (SI/SAE)
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,prgmPressureChannel								// tPressureChannel - absolute pressure (SI/SAE)
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,prgmEstimateEnginePower							// tEstimatedEnginePower - estimated engine power (SI/SAE)
	,prgmDragSpeed										// tDragSpeed - acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#endif // useDragRaceFunction
	,prgmFuelEcon										// tFuelEcon - fuel economy (SI/SAE)

// this is the start of the internal SWEET64 index program address list
	,prgmCalculateRemainingTank							// tCalculateRemainingTank - calculate estimated remaining fuel quantity in injector open cycles
	,prgmCalculateRemainingReserve						// tCalculateRemainingReserve - calculate estimated remaining fuel reserve value in injector open cycles
	,prgmCalculateBingoFuel								// tCalculateBingoFuel - calculate estimated fuel bingo value in injector open cycles
	,prgmConvertToMicroSeconds							// tCalculateFuelQuantity
	,prgmCalculateFuelQuantity							// tCalculateFuelQuantity
	,prgmCalculateFuelDistance							// tCalculateFuelDistance
	,prgmCalculateFuelTime								// tCalculateFuelTime
	,prgmFormatToTime									// tFormatToTime
	,prgmFormatToNumber									// tFormatToNumber
	,prgmRoundOffNumber									// tRoundOffNumber
	,prgmLoadTrip
	,prgmSaveTrip
	,prgmReadTicksToSeconds								// tReadTicksToSeconds
#ifdef useBarFuelEconVsTime
	,prgmFEvTgetDistance								// tFEvTgetDistance
	,prgmFEvTgetConsumedFuel							// tFEvTgetConsumedFuel
	,prgmFEvTgetFuelEconomy								// tFEvTgetFuelEconomy
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,prgmFEvSgetDistance								// tFEvSgetDistance
	,prgmFEvSgetConsumedFuel							// tFEvSgetConsumedFuel
	,prgmFEvSgetFuelEconomy								// tFEvSgetFuelEconomy
#endif // useBarFuelEconVsSpeed
#ifdef useDebugTerminal
	,prgmParseCharacterToReg
#endif //useDebugTerminal
};

static const uint8_t r00 =	0;			// do not fetch register operand
static const uint8_t r01 =	r00 + 32;	// fetch rX and rY from program
static const uint8_t r02 =	r01 + 32;	// set rX = r5
static const uint8_t r03 =	r02 + 32;	// set rX = r5, rY = r1
static const uint8_t r04 =	r03 + 32;	// fetch rP and rS from program
static const uint8_t r05 =	r04 + 32;	// set rP = r5, rS = r2
static const uint8_t r06 =	r05 + 32;	// fetch rP from program, set rS = r5, and rX to r5
static const uint8_t r07 =	r06 + 32;	//

static const uint8_t rxxMask =	0b11100000;

static const uint8_t p00 =	0;			// do not fetch primary operand
static const uint8_t p01 =	p00 + 4;	// load primary operand from program
static const uint8_t p02 =	p01 + 4;	// load primary operand from index
static const uint8_t p03 =	p02 + 4;	// load primary operand from program + index
static const uint8_t p04 =	p03 + 4;	// load primary operand with EEPROM indirect[index]
static const uint8_t p05 =	p04 + 4;	//
static const uint8_t p06 =	p05 + 4;	//
static const uint8_t p07 =	p06 + 4;	//

static const uint8_t pxxMask =	0b00011100;

static const uint8_t s00 =	0;			// do not fetch secondary operand
static const uint8_t s01 =	s00 + 1;	// fetch secondary operand from program
static const uint8_t s02 =	s01 + 1;	// fetch secondary operand from index
static const uint8_t s03 =	s02 + 1;	// fetch secondary operand from jump register

static const uint8_t sxxMask =	0b00000011;

// operations where opcode prefix rxx = r00
static const uint8_t e00 =	0;			// no operation
static const uint8_t e01 =	e00 + 1;	// branch c=1 | z=1		(bgte)
static const uint8_t e02 =	e01 + 1;	// branch c=1			(bgt)
static const uint8_t e03 =	e02 + 1;	// branch c=0			(blte)
static const uint8_t e04 =	e03 + 1;	// branch z=0			(bne)
static const uint8_t e05 =	e04 + 1;	// branch z=1			(beq)
static const uint8_t e06 =	e05 + 1;	// branch n=0			(bpl)
static const uint8_t e07 =	e06 + 1;	// branch n=1			(bmi)
static const uint8_t e08 =	e07 + 1;	// branch v=0			(bvc)
static const uint8_t e09 =	e08 + 1;	// branch v=1			(bvs)
static const uint8_t e10 =	e09 + 1;	// branch c=0 & z=0		(blt)
static const uint8_t e11 =	e10 + 1;	// branch if metric mode
static const uint8_t e12 =	e11 + 1;	// branch if SAE mode
static const uint8_t e13 =	e12 + 1;	// branch if output is fuel/distance
static const uint8_t e14 =	e13 + 1;	// branch if output is distance/fuel
static const uint8_t e15 =	e14 + 1;	// branch always
static const uint8_t e16 =	e15 + 1;	// return
static const uint8_t e17 =	e16 + 1;	// return and restore trace flag
static const uint8_t e18 =	e17 + 1;	// test index
static const uint8_t e19 =	e18 + 1;	// trace restore
static const uint8_t e20 =	e19 + 1;	// trace on
static const uint8_t e21 =	e20 + 1;	// trace save
static const uint8_t e22 =	e21 + 1;	// trace off
static const uint8_t e23 =	e22 + 1;	// load index
static const uint8_t e24 =	e23 + 1;	// load index eeprom
static const uint8_t e25 =	e24 + 1;	// compare index
static const uint8_t e26 =	e25 + 1;	// load index eeprom parameter length
static const uint8_t e27 =	e26 + 1;	// call
static const uint8_t e28 =	e27 + 1;	// jump
static const uint8_t e29 =	e28 + 1;	// load jump register
static const uint8_t e30 =	e29 + 1;	// clear register flag
static const uint8_t e31 =	e30 + 1;	// set register flag

// operations where opcode prefix rxx is not r00
static const uint8_t m00 =	0;			// no operation
static const uint8_t m01 =	m00 + 1;	// add			rP = rP + rS
static const uint8_t m02 =	m01 + 1;	// sub			rP = rP - rS
static const uint8_t m03 =	m02 + 1;	// compare		na = rP - rS
static const uint8_t m04 =	m03 + 1;	// test			r5
static const uint8_t m05 =	m04 + 1;	// multiply		r2 = r2 * r5
static const uint8_t m06 =	m05 + 1;	// divide		r2 = r2 / r5 rmdr r1 and qadj r5
static const uint8_t m07 =	m06 + 1;	//

static const uint8_t mxxMask = 0b00000111;

static const uint8_t i00 =	0;			// no operation
static const uint8_t i01 =	i00 + 8;	// load rX with byte
static const uint8_t i02 =	i01 + 8;	// load rX with rY
static const uint8_t i03 =	i02 + 8;	// load rX with eeprom
static const uint8_t i04 =	i03 + 8;	// load rX with main program
static const uint8_t i05 =	i04 + 8;	// load rX with volatile
static const uint8_t i06 =	i05 + 8;	// load rX with trip variable
static const uint8_t i07 =	i06 + 8;	// load rX with const
static const uint8_t i08 =	i07 + 8;	// load rX with eeprom init
static const uint8_t i09 =	i08 + 8;	// load rX with voltage
static const uint8_t i10 =	i09 + 8;	// load rX with numerator for constant
static const uint8_t i11 =	i10 + 8;	// integer square root
static const uint8_t i12 =	i11 + 8;	// load rX with denominator for constant
static const uint8_t i13 =	i12 + 8;	//
static const uint8_t i14 =	i13 + 8;	//
static const uint8_t i15 =	i14 + 8;	// swap rX rY
static const uint8_t i16 =	i15 + 8;	// store rX[extra] operand
static const uint8_t i17 =	i16 + 8;	// store eeprom rX
static const uint8_t i18 =	i17 + 8;	// store volatile rX
static const uint8_t i19 =	i18 + 8;	// store main program rX
static const uint8_t i20 =	i19 + 8;	// store trip variable rX
static const uint8_t i21 =	i20 + 8;	//
static const uint8_t i22 =	i21 + 8;	//
static const uint8_t i23 =	i22 + 8;	// load rX with FEvT trip variable
static const uint8_t i24 =	i23 + 8;	// shift rX left
static const uint8_t i25 =	i24 + 8;	// shift rX right
static const uint8_t i26 =	i25 + 8;	// BCD adjust
static const uint8_t i27 =	i26 + 8;	// store rX byte to bargraph data array
static const uint8_t i28 =	i27 + 8;	// load rX with byte[operand]
static const uint8_t i29 =	i28 + 8;	//
static const uint8_t i30 =	i29 + 8;	//
static const uint8_t i31 =	i30 + 8;	//

static const uint8_t ixxMask = 0b11111000;

static const uint8_t opcodeFetchPrefix[(uint16_t)(maxValidSWEET64instr)] PROGMEM = {
	 r04 | p00 | s00	// instrTestReg
	,r00 | p00 | s00	// instrTestIndex
	,r04 | p00 | s00	// instrCmpXtoY
	,r00 | p01 | s00	// instrCmpIndex
	,r00 | p00 | s01	// instrBranchIfVclear
	,r00 | p00 | s01	// instrBranchIfVset
	,r00 | p00 | s01	// instrBranchIfMclear
	,r00 | p00 | s01	// instrBranchIfMset
	,r00 | p00 | s01	// instrBranchIfZclear
	,r00 | p00 | s01	// instrBranchIfZset
	,r00 | p00 | s01	// instrBranchIfCclear
	,r00 | p00 | s01	// instrBranchIfCset
	,r00 | p00 | s01	// instrBranchIfLT
	,r00 | p00 | s01	// instrBranchIfGTorE
	,r00 | p00 | s01	// instrBranchIfMetricMode
	,r00 | p00 | s01	// instrBranchIfSAEmode
	,r00 | p00 | s01	// instrBranchIfFuelOverDist
	,r00 | p00 | s01	// instrBranchIfDistOverFuel
	,r00 | p00 | s01	// instrSkip
	,r00 | p00 | s01	// instrCall
	,r00 | p00 | s03	// instrCallImplied
	,r00 | p00 | s01	// instrJump
	,r01 | p00 | s00	// instrLdReg
	,r01 | p02 | s00	// instrLdRegByteFromIndex
	,r01 | p01 | s00	// instrLdRegByte
	,r01 | p01 | s00	// instrLdRegByteFromY
	,r04 | p01 | s01	// instrLdRegTripVar
	,r04 | p02 | s01	// instrLdRegTripVarIndexed
	,r04 | p03 | s01	// instrLdRegTripVarOffset
	,r04 | p01 | s02	// instrLdRegTripVarIndexedRV
	,r01 | p02 | s01	// instrStRegTripVarIndexed
	,r01 | p01 | s02	// instrStRegTripVarIndexedRV
	,r01 | p01 | s00	// instrLdRegConst
	,r01 | p02 | s00	// instrLdRegConstIndexed
	,r01 | p01 | s01	// instrStByteToRegU8
	,r01 | p00 | s00	// instrDoBCDadjust
	,r01 | p01 | s00	// instrLdRegEEPROM
	,r01 | p02 | s00	// instrLdRegEEPROMindexed
	,r01 | p04 | s00	// instrLdRegEEPROMindirect
	,r01 | p01 | s00	// instrLdRegEinit
	,r01 | p02 | s00	// instrLdRegEinitIndexed
	,r01 | p01 | s00	// instrStRegEEPROM
	,r01 | p02 | s00	// instrStRegEEPROMindexed
	,r01 | p04 | s00	// instrStRegEEPROMindirect
	,r01 | p01 | s00	// instrLdRegMain
	,r01 | p02 | s00	// instrLdRegMainIndexed
	,r01 | p03 | s00	// instrLdRegMainOffset
	,r01 | p01 | s00	// instrStRegMain
	,r01 | p02 | s00	// instrStRegMainIndexed
	,r01 | p01 | s00	// instrLdRegVolatile
	,r01 | p02 | s00	// instrLdRegVolatileIndexed
	,r01 | p01 | s00	// instrStRegVolatile
	,r01 | p02 | s00	// instrStRegVolatileIndexed
	,r00 | p01 | s00	// instrLxdI
	,r00 | p01 | s00	// instrLxdIEEPROM
	,r00 | p03 | s00	// instrLxdIEEPROMoffset
	,r00 | p01 | s00	// instrLxdIParamLength
	,r00 | p02 | s00	// instrLxdIParamLengthIndexed
	,r01 | p02 | s00	// instrLdRegNumer
	,r01 | p02 | s00	// instrLdRegDenom
	,r01 | p00 | s00	// instrSwapReg
	,r04 | p00 | s00	// instrSubYfromX
	,r06 | p01 | s00	// instrSubMainFromX
	,r04 | p00 | s00	// instrAddYtoX
	,r05 | p00 | s00	// instrAdjustQuotient
	,r06 | p01 | s00	// instrAddByteToX
	,r06 | p01 | s00	// instrAddConstToX
	,r06 | p01 | s00	// instrAddEEPROMtoX
	,r06 | p01 | s00	// instrAddMainToX
	,r06 | p02 | s00	// instrAddIndexToX
	,r03 | p00 | s00	// instrMul2by1
	,r02 | p01 | s00	// instrMul2byByte
	,r02 | p01 | s00	// instrMul2byConst
	,r02 | p01 | s00	// instrMul2byEEPROM
	,r02 | p01 | s00	// instrMul2byMain
	,r02 | p01 | s00	// instrMul2byVolatile
	,r02 | p02 | s01	// instrMul2byTripVarIndexed
	,r03 | p00 | s00	// instrDiv2by1
	,r02 | p01 | s00	// instrDiv2byConst
	,r02 | p01 | s00	// instrDiv2byEEPROM
	,r02 | p01 | s00	// instrDiv2byMain
	,r02 | p02 | s01	// instrDiv2byTripVarIndexed
	,r02 | p01 | s00	// instrDiv2byByte
	,r01 | p00 | s00	// instrShiftRegLeft
	,r01 | p00 | s00	// instrShiftRegRight
	,r00 | p03 | s00	// instrAddIndex
	,r00 | p00 | s00	// instrTraceOn
	,r00 | p00 | s00	// instrTraceOff
	,r00 | p00 | s00	// instrTraceSave
	,r00 | p00 | s00	// instrTraceRestore
	,r00 | p00 | s00	// instrTraceDone
	,r00 | p02 | s00	// instrLdJumpReg
	,r00 | p01 | s00	// instrClearFlag
	,r00 | p01 | s00	// instrSetFlag
#ifdef useIsqrt
	,r01 | p00 | s00	// instrIsqrt
#endif // useIsqrt
#if defined(useAnalogRead)
	,r01 | p01 | s00	// instrLdRegVoltage
	,r01 | p02 | s00	// instrLdRegVoltageIndexed
#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
	,r04 | p02 | s01	// instrLdRegTripFEvTindexed
#endif // useBarFuelEconVsTime
#ifdef useBarGraph
	,r01 | p02 | s00	// instrStRegBGdataIndexed
#endif // useBarGraph
	,r00 | p00 | s00	// instrDone
};

static const uint8_t opcodeFetchSuffix[(uint16_t)(maxValidSWEET64instr)] PROGMEM = {
	 m04 | i00			// instrTestReg
	,e18				// instrTestIndex
	,m03 | i00			// instrCmpXtoY
	,e25				// instrCmpIndex
	,e08				// instrBranchIfVclear
	,e09				// instrBranchIfVset
	,e06				// instrBranchIfMclear
	,e07				// instrBranchIfMset
	,e04				// instrBranchIfZclear
	,e05				// instrBranchIfZset
	,e03				// instrBranchIfCclear
	,e02				// instrBranchIfCset
	,e10				// instrBranchIfLT
	,e01				// instrBranchIfGTorE
	,e11				// instrBranchIfMetricMode
	,e12				// instrBranchIfSAEmode
	,e13				// instrBranchIfFuelOverDist
	,e14				// instrBranchIfDistOverFuel
	,e15				// instrSkip
	,e27				// instrCall
	,e27				// instrCallImplied
	,e28				// instrJump
	,m00 | i02			// instrLdReg
	,m00 | i01			// instrLdRegByteFromIndex
	,m00 | i01			// instrLdRegByte
	,m00 | i28			// instrLdRegByteFromY
	,m04 | i06			// instrLdRegTripVar
	,m04 | i06			// instrLdRegTripVarIndexed
	,m04 | i06			// instrLdRegTripVarOffset
	,m04 | i06			// instrLdRegTripVarIndexedRV
	,m00 | i20			// instrStRegTripVarIndexed
	,m00 | i20			// instrStRegTripVarIndexedRV
	,m00 | i07			// instrLdRegConst
	,m00 | i07			// instrLdRegConstIndexed
	,m00 | i16			// instrStByteToRegU8
	,m00 | i26			// instrDoBCDadjust
	,m00 | i03			// instrLdRegEEPROM
	,m00 | i03			// instrLdRegEEPROMindexed
	,m00 | i03			// instrLdRegEEPROMindirect
	,m00 | i08			// instrLdRegEinit
	,m00 | i08			// instrLdRegEinitIndexed
	,m00 | i17			// instrStRegEEPROM
	,m00 | i17			// instrStRegEEPROMindexed
	,m00 | i17			// instrStRegEEPROMindirect
	,m00 | i04			// instrLdRegMain
	,m00 | i04			// instrLdRegMainIndexed
	,m00 | i04			// instrLdRegMainOffset
	,m00 | i19			// instrStRegMain
	,m00 | i19			// instrStRegMainIndexed
	,m00 | i05			// instrLdRegVolatile
	,m00 | i05			// instrLdRegVolatileIndexed
	,m00 | i18			// instrStRegVolatile
	,m00 | i18			// instrStRegVolatileIndexed
	,e23				// instrLxdI
	,e24				// instrLxdIEEPROM
	,e24				// instrLxdIEEPROMoffset
	,e26				// instrLxdIParamLength
	,e26				// instrLxdIParamLengthIndexed
	,m00 | i10			// instrLdRegNumer
	,m00 | i12			// instrLdRegDenom
	,m00 | i15			// instrSwapReg
	,m02 | i00			// instrSubYfromX
	,m02 | i04			// instrSubMainFromX
	,m01 | i00			// instrAddYtoX
	,m01 | i00			// instrAdjustQuotient
	,m01 | i01			// instrAddByteToX
	,m01 | i07			// instrAddConstToX
	,m01 | i03			// instrAddEEPROMtoX
	,m01 | i04			// instrAddMainToX
	,m01 | i01			// instrAddIndexToX
	,m05 | i02			// instrMul2by1
	,m05 | i01			// instrMul2byByte
	,m05 | i07			// instrMul2byConst
	,m05 | i03			// instrMul2byEEPROM
	,m05 | i04			// instrMul2byMain
	,m05 | i05			// instrMul2byVolatile
	,m05 | i06			// instrMul2byTripVarIndexed
	,m06 | i02			// instrDiv2by1
	,m06 | i07			// instrDiv2byConst
	,m06 | i03			// instrDiv2byEEPROM
	,m06 | i04			// instrDiv2byMain
	,m06 | i06			// instrDiv2byTripVarIndexed
	,m06 | i01			// instrDiv2byByte
	,m00 | i24			// instrShiftRegLeft
	,m00 | i25			// instrShiftRegRight
	,e23				// instrAddIndex
	,e20				// instrTraceOn
	,e22				// instrTraceOff
	,e21				// instrTraceSave
	,e19				// instrTraceRestore
	,e17				// instrTraceDone
	,e29				// instrLdJumpReg
	,e30				// instrClearFlag
	,e31				// instrSetFlag
#ifdef useIsqrt
	,m00 | i11			// instrIsqrt
#endif // useIsqrt
#if defined(useAnalogRead)
	,m00 | i09			// instrLdRegVoltage
	,m00 | i09			// instrLdRegVoltageIndexed
#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
	,m04 | i23			// instrLdRegTripFEvTindexed
#endif // useBarFuelEconVsTime
#ifdef useBarGraph
	,m00 | i27			// instrStRegBGdataIndexed
#endif // useBarGraph
	,e16				// instrDone
};

#ifdef useSWEET64trace
static void SWEET64::listProgram(uint8_t calcIdx)
{
}

#endif // useSWEET64trace
static uint32_t SWEET64::doCalculate(uint8_t tripIdx, uint8_t calcIdx)
{

	return runPrgm((const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(calcIdx)])), tripIdx);

}

static uint32_t SWEET64::runPrgm(const uint8_t * sched, uint8_t tripIdx)
{

	uint8_t oldSREG;
	uint8_t spnt = 0;
	const uint8_t * prgmStack[16];
	uint8_t opcodePrefix;
	uint8_t opcodeSuffix;
	uint8_t instr;
	uint8_t operand;
	uint8_t extra;
	uint8_t loopFlag;
	uint8_t branchFlag;
	uint8_t jumpVal;

	union union_64 * regX;
	union union_64 * regY;
	union union_64 * regP;
	union union_64 * regS;
#ifdef useSWEET64trace
	uint8_t traceSave;
	uint8_t i;
#endif // useSWEET64trace
#ifdef useDebugCPUreading
	uint32_t s64Start;
	uint32_t mathStart;
#endif // useDebugCPUreading

#ifdef useDebugCPUreading
	s64Start = systemInfo::cycles0();

#endif // useDebugCPUreading
	SWEET64processorFlags = 0;
	loopFlag = 1;

	do
	{

#ifdef useSWEET64trace
		i = (SWEET64processorFlags & SWEET64traceFlagGroup);
		if (i == SWEET64traceFlag) SWEET64processorFlags &= ~(SWEET64traceFlag);
		if (i == SWEET64traceCommandFlag) SWEET64processorFlags |= (SWEET64traceFlag);

		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugTerminal, 13);
			text::hexWordOut(devDebugTerminal, (uint16_t)(sched));
			text::charOut(devDebugTerminal, 32);
			text::hexByteOut(devDebugTerminal, tripIdx);
			text::charOut(devDebugTerminal, 32);
			text::hexByteOut(devDebugTerminal, spnt);
			text::charOut(devDebugTerminal, 32);

		}

#endif // useSWEET64trace
		instr = fetchByte(sched);

		if (instr < maxValidSWEET64instr)
		{

			opcodePrefix = pgm_read_byte(&opcodeFetchPrefix[(uint16_t)(instr)]);
			opcodeSuffix = pgm_read_byte(&opcodeFetchSuffix[(uint16_t)(instr)]);

		}
		else loopFlag = 0;

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::stringOut(devDebugTerminal, opcodeList, instr);
			text::charOut(devDebugTerminal, 13);
			text::charOut(devDebugTerminal, 9);

		}

#endif // useSWEET64trace
		branchFlag = 1;
		operand = 0;
		extra = 0;

		switch (opcodePrefix & rxxMask) // categorize opcode according to how it accesses the 64 bit registers
		{

			case r00:	// do not fetch register operand
				opcodePrefix &= ~(rxxMask); // slice off rxx portion of opcode prefix
				branchFlag = 0;
				break;

			case r01:	// fetch rX and rY from program
				operand = fetchByte(sched);
				break;

			case r02:	// set rX = r5
				operand = 0x05;	// signal to move external contents to arithmetic register 5
				break;

			case r03:	// set rX = r5, rY = r1
				operand = 0x15;	// signal to move register 1 contents to arithmetic register 5
				break;

			case r04:	// fetch rP and rS from program
				extra = fetchByte(sched);
				operand = extra;
				break;

			case r05:	// set rP = r5, rS = r2
				extra = 0x52;	// signal to add half remainder flag to quotient
				break;

			case r06:	// fetch rP from program, set rS = r5, and rX to r5
				extra = fetchByte(sched) & 0x07; // ignore the rS nybble
				extra |= 0x50; // set rS to r5
				operand = 0x05;
				break;

			default:	// invalid rxx code detected, exit program
				loopFlag = 0;
				break;

		}

		if (operand & 0x07) regX = (union union_64 *)(&s64reg[(unsigned int)((operand & 0x07) - 1)]);
		if (operand & 0x70) regY = (union union_64 *)(&s64reg[(unsigned int)(((operand & 0x70) >> 4) - 1)]);
		if (extra & 0x07) regP = (union union_64 *)(&s64reg[(unsigned int)((extra & 0x07) - 1)]);
		if (extra & 0x70) regS = (union union_64 *)(&s64reg[(unsigned int)(((extra & 0x70) >> 4) - 1)]);

		operand = 0;

		switch (opcodePrefix & pxxMask) // pull general operand to work with
		{

			case p00:	// do not fetch primary operand
				break;

			case p03:	// load primary operand with program + index
				operand += tripIdx;
			case p01:	// load primary operand from program
				operand += fetchByte(sched);
				break;

			case p02:	// load primary operand from index
				operand += tripIdx;
				break;

			case p04:	// load primary operand with EEPROM indirect[index]
				operand += pgm_read_byte(&convIdx[(unsigned int)(tripIdx)]);
				break;

			default:	// invalid pxx code detected, exit program
				loopFlag = 0;
				break;

		}

		extra = 0;

		switch (opcodePrefix & sxxMask) // pull extra operand to work with
		{

			case s00:	// do not fetch secondary operand
				break;

			case s01:	// fetch secondary operand from program
				extra += fetchByte(sched);
				break;

			case s02:	// fetch secondary operand from index
				extra += tripIdx;
				break;

			case s03:	// fetch secondary operand from jump register
				extra += jumpVal;
				break;

			default:	// invalid sxx code detected, exit program
				loopFlag = 0;
				break;

		}

		if (branchFlag) // opcode does something with the 64 bit registers
		{

			switch (opcodeSuffix & ixxMask) // perform load or store operation, according to ixx
			{

				case i00:	// do nothing
					break;

				case i28:	// load rX with byte[operand]
					operand = regY->u8[(unsigned int)(operand)];
				case i01:	// load rX with byte
					init64byt(regX, operand);
					break;

				case i02:	// load rX with rY
					copy64(regX, regY);
					break;

				case i03:	// load rX with eeprom
					EEPROM::read64(regX, operand);
					break;

				case i04:	// load rX with main program
					init64(regX, mainProgramVariables[(uint16_t)(operand)]);
					break;

				case i05:	// load rX with volatile
					oldSREG = SREG; // save interrupt flag status
					cli(); // disable interrupts

					init64(regX, volatileVariables[(uint16_t)(operand)]);

					SREG = oldSREG; // restore interrupt flag status
					break;

#ifdef useBarGraph
				case i27:	// store rX byte to bargraph data array
					bargraphData[(unsigned int)(operand)] = regX->u8[0];
					break;

#endif // useBarGraph
#ifdef useBarFuelEconVsTime
				case i23:	// load rX with FEvT trip variable
					instr = calcFEvTperiodIdx; // get current fuel econ vs time trip variable
					instr -= FEvsTimeIdx; // translate out of trip index space

					if (operand >= bgDataSize) operand = 0; // shift index
					else operand++;

					operand += instr; // add to trip variable value
					if (operand >= bgDataSize) operand -= bgDataSize; // perform wrap-around if required

					operand += FEvsTimeIdx; // shift back into trip index space
#endif // useBarFuelEconVsTime
				case i06:	// load rX with trip variable
#ifdef useEEPROMtripStorage
					if (operand >= tripSlotCount)
					{

						branchFlag = tripVar::getBaseEEPROMaddress(operand, extra);
						if (branchFlag) EEPROM::read64(regX, branchFlag);
						else init64byt(regX, 0);
						break;

					}

#endif // useEEPROMtripStorage
					oldSREG = SREG; // save interrupt flag status
					cli(); // disable interrupts

					switch (extra)
					{

						case rvVSSpulseIdx:
							if (operand < tripSlotCount) init64(regX, collectedVSSpulseCount[(unsigned int)(operand)]);
							else init64byt(regX, 0);
							break;

						case rvInjPulseIdx:
							if (operand < tripSlotFullCount) init64(regX, collectedInjPulseCount[(unsigned int)(operand)]);
							else init64byt(regX, 0);
							break;

						case rvVSScycleIdx:
							if (operand < tripSlotFullCount) copy64(regX, (union union_64 *)&collectedVSScycleCount[(uint16_t)(operand)]);
							else init64byt(regX, 0);
							break;

						case rvInjCycleIdx:
							if (operand < tripSlotCount) copy64(regX, (union union_64 *)&collectedInjCycleCount[(uint16_t)(operand)]);
							else init64byt(regX, 0);
							break;

						case rvEngCycleIdx:
							if (operand < tripSlotFullCount) copy64(regX, (union union_64 *)&collectedEngCycleCount[(uint16_t)(operand)]);
							else init64byt(regX, 0);
							break;

						default:
							init64byt(regX, 0);
							break;

					}

					SREG = oldSREG; // restore interrupt flag status
					break;

				case i12:	// load rX with denominator for constant
					operand ^= 1;
				case i10:	// load rX with numerator for constant
					if (metricFlag & metricMode) operand ^= 1;
					operand ^= pgm_read_byte(&convNumerIdx[(unsigned int)(operand)]);
				case i07:	// load rX with const
					init64(regX, pgm_read_dword(&convNumbers[(uint16_t)(operand)]));
					break;

				case i08:	// load rX with eeprom init
					init64(regX, pgm_read_dword(&params[(uint16_t)(operand)]));
					break;

#if defined(useAnalogRead)
				case i09:	// load rX with voltage
					init64(regX, analogValue[(unsigned int)(operand)]);
					break;

#endif // useAnalogRead
#ifdef useIsqrt
				case i11:	// integer square root
					regX->ui[0] = iSqrt(regX->ui[0]);
					break;

#endif // useIsqrt
				case i15:	// swap rX rY
					swap64(regX, regY);
					break;

				case i16:	// store rX[index] direct
					regX->u8[(unsigned int)(extra)] = operand;
					break;

				case i17:	// store eeprom rX
					EEPROM::write64(regX, operand);
					break;

				case i18:	// store volatile rX
					oldSREG = SREG; // save interrupt flag status
					cli(); // disable interrupts

					volatileVariables[(uint16_t)(operand)] = regX->ul[0];

					SREG = oldSREG; // restore interrupt flag status
					break;

				case i19:	// store main program rX
					mainProgramVariables[(uint16_t)(operand)] = regX->ul[0];
					break;

				case i20:	// store trip variable rX
#ifdef useEEPROMtripStorage
					if (operand >= tripSlotCount)
					{

						branchFlag = tripVar::getBaseEEPROMaddress(operand, extra);
						if (branchFlag) EEPROM::write64(regX, branchFlag);
						break;

					}

#endif // useEEPROMtripStorage
					oldSREG = SREG; // save interrupt flag status
					cli(); // disable interrupts

					switch (extra)
					{

						case rvVSSpulseIdx:
							if (operand < tripSlotCount) collectedVSSpulseCount[(unsigned int)(operand)] = regX->ul[0];
							break;

						case rvInjPulseIdx:
							if (operand < tripSlotFullCount) collectedInjPulseCount[(unsigned int)(operand)] = regX->ul[0];
							break;

						case rvVSScycleIdx:
							if (operand < tripSlotFullCount) copy64((union union_64 *)&collectedVSScycleCount[(uint16_t)(operand)], regX);
							break;

						case rvInjCycleIdx:
							if (operand < tripSlotCount) copy64((union union_64 *)&collectedInjCycleCount[(uint16_t)(operand)], regX);
							break;

						case rvEngCycleIdx:
							if (operand < tripSlotFullCount) copy64((union union_64 *)&collectedEngCycleCount[(uint16_t)(operand)], regX);
							break;

						default:
							break;

					}

					SREG = oldSREG; // restore interrupt flag status
					break;

				case i24:	// shift rX left
					SWEET64processorFlags &= ~(SWEET64carryFlag);
					shl64(regX);
					break;

				case i25:	// shift rX right
					shr64(regX);
					break;

				case i26:	// BCD adjust
					instr = regX->u8[6]; // fetch length

					if (instr < 4) extra = instr - 1;
					else
					{

						extra = 3;
						regX->u8[0] = ((union union_64 *)(&s64reg[s64reg2]))->u8[0]; // workaround to support 10 decimal digits / 5 BCD bytes

					}

					for (uint8_t x = extra; x < 4; x--)
					{

						operand = regX->u8[(uint16_t)(--instr)]; // get indexed divisor
						if (operand)
						{

							regX->u8[(uint16_t)(instr)] = (uint8_t)(regY->ul[0] % operand); // put result of (source register) mod divisor into indexed byte of (target register)
							regY->ul[0] /= operand; // divide (source register) by divisor

						}
						else regX->u8[(uint16_t)(instr)] = operand;

					}

					break;

				default:	// invalid ixx code detected, exit program
					loopFlag = 0;
					break;

			}

			switch (opcodeSuffix & mxxMask) // perform basic arithmetic operation, given mxx
			{

				case m00:	// non-arithmetic
					break;

				case m01:	// add			rP = rP + rS
					adc64(regP, regS);
					break;

				case m02:	// sub			rP = rP - rS
					sbc64(regP, regS, 1);
					break;

				case m03:	// compare		na = rP - rS
					sbc64(regP, regS, 0);
					break;

				case m04:	// test			r5
					registerTest64(regP);
					break;

				case m05:	// multiply		r2 = r2 * r5
#ifdef useDebugCPUreading
					mathStart = systemInfo::cycles0(); // record starting time
#endif // useDebugCPUreading
#ifdef useSWEET64mult
					prgmStack[(unsigned int)(spnt++)] = sched;
					if (spnt > 15) loopFlag = 0;
					else sched = prgmMult64;
#ifdef useSWEET64trace
					traceSave = SWEET64processorFlags;
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
#else // useSWEET64mult
					mult64();
#endif // useSWEET64mult
#ifdef useDebugCPUreading
					mainProgramVariables[(uint16_t)(mpDebugAccS64multIdx)] += systemInfo::findCycleLength(mathStart, systemInfo::cycles0());
					mainProgramVariables[(uint16_t)(mpDebugCountS64multIdx)]++;
#endif // useDebugCPUreading
					break;

				case m06:	// divide		r2 = r2 / r5 rmdr r1 and qadj r5
#ifdef useDebugCPUreading
					mathStart = systemInfo::cycles0(); // record starting time
#endif // useDebugCPUreading
#ifdef useSWEET64div
					prgmStack[(unsigned int)(spnt++)] = sched;
					if (spnt > 15) loopFlag = 0;
					else sched = prgmDiv64;
#ifdef useSWEET64trace
					traceSave = SWEET64processorFlags;
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
#else // useSWEET64div
					div64();
#endif // useSWEET64div
#ifdef useDebugCPUreading
					mainProgramVariables[(uint16_t)(mpDebugAccS64divIdx)] += systemInfo::findCycleLength(mathStart, systemInfo::cycles0());
					mainProgramVariables[(uint16_t)(mpDebugCountS64divIdx)]++;
#endif // useDebugCPUreading
					break;

				default:	// invalid mxx opcode detected, exit program
					loopFlag = 0;
					break;

			}

		}
		else // opcode does nothing at all with the 64-bit registers
		{

			switch (opcodeSuffix)
			{

				case e00:	// do nothing
					break;

				case e01:	// instrBranchIfGTorE
					branchFlag = ((SWEET64processorFlags & SWEET64zeroFlag) || (SWEET64processorFlags & SWEET64carryFlag));
					break;

				case e02:	// instrBranchIfCset
					branchFlag = (SWEET64processorFlags & SWEET64carryFlag);
					break;

				case e03:	// instrBranchIfCclear
					branchFlag = ((SWEET64processorFlags & SWEET64carryFlag) == 0);
					break;

				case e04:	// instrBranchIfZclear
					branchFlag = ((SWEET64processorFlags & SWEET64zeroFlag) == 0);
					break;

				case e05:	// instrBranchIfZset
					branchFlag = (SWEET64processorFlags & SWEET64zeroFlag);
					break;

				case e06:	// instrBranchIfMclear
					branchFlag = ((SWEET64processorFlags & SWEET64minusFlag) == 0);
					break;

				case e07:	// instrBranchIfMset
					branchFlag = (SWEET64processorFlags & SWEET64minusFlag);
					break;

				case e08:	// instrBranchIfVclear
					branchFlag = ((SWEET64processorFlags & SWEET64overflowFlag) == 0);
					break;

				case e09:	// instrBranchIfVset
					branchFlag = (SWEET64processorFlags & SWEET64overflowFlag);
					break;

				case e10:	// instrBranchIfLT
					branchFlag = (((SWEET64processorFlags & SWEET64zeroFlag) == 0) && ((SWEET64processorFlags & SWEET64carryFlag) == 0));
					break;

				case e11:	// instrBranchIfMetricMode
					branchFlag = (metricFlag & metricMode);
					break;

				case e12:	// instrBranchIfSAEmode
					branchFlag = ((metricFlag & metricMode) == 0);
					break;

				case e13:	// instrBranchIfFuelOverDist (L/100km or G/100mi)
					branchFlag = (((metricFlag & fuelEconOutputFlags) == metricMode) || ((metricFlag & fuelEconOutputFlags) == alternateFEmode));
					break;

				case e14:	// instrBranchIfDistOverFuel (MPG or KPL)
					branchFlag = (((metricFlag & fuelEconOutputFlags) == 0) || ((metricFlag & fuelEconOutputFlags) == fuelEconOutputFlags));
					break;

				case e15:	// instrSkip
					branchFlag = true;
					break;

				case e17:	// instrTraceDone
#ifdef useSWEET64trace
					if (traceSave & SWEET64traceFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
					else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
				case e16:	// instrDone
					if (spnt--) sched = prgmStack[(unsigned int)(spnt)];
					else loopFlag = 0;
					break;

				case e18:	// instrTestIndex
					if (tripIdx) SWEET64processorFlags &= ~(SWEET64zeroFlag);
					else SWEET64processorFlags |= (SWEET64zeroFlag);

					if (tripIdx & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
					else SWEET64processorFlags &= ~(SWEET64minusFlag);

					break;

				case e19:	// instrTraceRestore
#ifdef useSWEET64trace
					if (traceSave & SWEET64traceFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
					else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
					break;

				case e20:	// instrTraceOn
#ifdef useSWEET64trace
					SWEET64processorFlags |= (SWEET64traceCommandFlag);
#endif // useSWEET64trace
					break;

				case e21:	// instrTraceSave
#ifdef useSWEET64trace
					traceSave = SWEET64processorFlags;
#endif // useSWEET64trace
				case e22:	// instrTraceOff
#ifdef useSWEET64trace
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
					break;

				case e23:	// load index
					tripIdx = operand;
					break;

				case e24:	// load index eeprom
					tripIdx = EEPROM::readVal(operand);
					break;

				case e25:	// compare index
					if (operand < tripIdx) SWEET64processorFlags |= (SWEET64carryFlag);
					else SWEET64processorFlags &= ~(SWEET64carryFlag);

					if (operand == tripIdx) SWEET64processorFlags |=(SWEET64zeroFlag);
					else SWEET64processorFlags &= ~(SWEET64zeroFlag);

					if ((operand - tripIdx) & 0x80) SWEET64processorFlags |=(SWEET64minusFlag);
					else SWEET64processorFlags &= ~(SWEET64minusFlag);

					break;

				case e26:	// load index eeprom parameter length
					tripIdx = EEPROM::getLength(operand);
					break;

				case e27:	// call
					prgmStack[(unsigned int)(spnt++)] = sched;
					if (spnt > 15) loopFlag = 0;
				case e28:	// jump
					sched = (const uint8_t *)pgm_read_word(&S64programList[(unsigned int)(extra)]);
					break;

				case e29:	// load jump register
					jumpVal = operand;
					break;

				case e30:	// clear register flag
				case e31:	// set register flag
					flagSet((opcodeSuffix == e31), operand);
					break;

				default:	// invalid sxx code detected, exit program
					loopFlag = 0;
					break;

			}

			if (branchFlag)
			{

				if (extra < 128) sched += extra;
				else sched -= (256 - extra);

			}

		}

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugTerminal, 13);
			text::charOut(devDebugTerminal, 9);
			text::charOut(devDebugTerminal, 9);
			text::hexByteOut(devDebugTerminal, SWEET64processorFlags);
			text::charOut(devDebugTerminal, 32);
			text::hexByteOut(devDebugTerminal, branchFlag);
			text::charOut(devDebugTerminal, 13);

			for (uint8_t x = 0; x < 5; x++)
			{

				text::charOut(devDebugTerminal, 9);
				text::charOut(devDebugTerminal, 9);
				text::charOut(devDebugTerminal, 9);
				text::hexByteOut(devDebugTerminal, (x + 1));
				text::charOut(devDebugTerminal, 32);
				text::hexLWordOut(devDebugTerminal, &s64reg[(uint16_t)(x)]);
				text::charOut(devDebugTerminal, 13);

			}

			text::charOut(devDebugTerminal, 13);

		}

#endif // useSWEET64trace
	}
	while (loopFlag);

#ifdef useDebugCPUreading
	SWEET64timerLength += systemInfo::findCycleLength(s64Start, systemInfo::cycles0());

#endif // useDebugCPUreading
	return ((union union_64 *)(&s64reg[(uint16_t)(s64reg2)]))->ul[0];

}

static uint8_t SWEET64::fetchByte(const uint8_t * &prgmPtr)
{

	uint8_t byt;

	byt = pgm_read_byte(prgmPtr++);

#ifdef useSWEET64trace
	if (SWEET64processorFlags & SWEET64traceFlag)
	{

		text::hexByteOut(devDebugTerminal, byt);
		text::charOut(devDebugTerminal, 32);

	}

#endif // useSWEET64trace
	return byt;

}

static void SWEET64::copy64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		: "+e" (an)
		: "e" (ann)
	);
#else // useAssemblyLanguage
	for (uint8_t x = 0; x < 4; x++) an->ui[(unsigned int)(x)] = ann->ui[(unsigned int)(x)];
#endif // useAssemblyLanguage

}

static void SWEET64::swap64(union union_64 * an, union union_64 * ann) // swap ann and an
{

	uint16_t aing;
	uint8_t x;

#ifdef useAssemblyLanguage
	asm volatile(
		"	ldi	%A3, 8				\n"		// initialize counter

		"l_swap64%=:				\n"
		"	ld	%A2, %a0			\n"
		"	ld	%B2, %a1			\n"
		"	st	%a1+, %A2			\n"
		"	st	%a0+, %B2			\n"
		"	dec	%A3					\n"
		"	brne l_swap64%=			\n"
		: "+e" (ann), "+e" (an), "+r" (aing), "+r" (x)
	);
#else // useAssemblyLanguage
	for (x = 0; x < 4; x++)
	{

		aing = ann->ui[(unsigned int)(x)];
		ann->ui[(unsigned int)(x)] = an->ui[(unsigned int)(x)];
		an->ui[(unsigned int)(x)] = aing;

	}
#endif // useAssemblyLanguage

}

static void SWEET64::shr64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	asm volatile(
		"	clc						\n"
		"	adiw	%a0, 8			\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_shr64%=:					\n"
		"	ld	__tmp_reg__, -%a0	\n"
		"	ror	__tmp_reg__			\n"
		"	st	%a0, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag
		"	dec	%A4					\n"
		"	brne l_shr64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;
	z = 0;

	for (x = 7; x < 8; x--)
	{

		n->u8[1] = an->u8[(uint16_t)(x)];
		n->u8[0] = 0;
		enn >>= 1;
		m = c;
		m |= n->u8[1];
		c = n->u8[0];
		an->u8[(uint16_t)(x)] = m;
		z |= m;

	}

#endif // useAssemblyLanguage
	m = an->u8[7];

	flagSet64(m, z, c);

}

static void SWEET64::shl64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);

	asm volatile(
		"	ror	%A1					\n"		// shift input carry bit into avr carry
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_shl64%=:					\n"
		"	ld	%A3, %a0			\n"
		"	rol	%A3					\n"
		"	st	%a0+, %A3			\n"
		"	or	%A2, %A3			\n"		// update zero flag
		"	dec	%A4					\n"
		"	brne l_shl64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 0x01 : 0x00);
	z = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[0] = an->u8[(uint16_t)(x)];
		n->u8[1] = 0;
		enn <<= 1;
		m = c;
		m |= n->u8[0];
		c = n->u8[1];
		an->u8[(uint16_t)(x)] = m;
		z |= m;

	}
#endif // useAssemblyLanguage

	flagSet64(m, z, c);

}
// SWEET64 64-bit addition instruction
//
// adds ann to an, and stores the result in an
//
// if an + ann results in overflow,		c=1
//						otherwise,		c=0
//
// if an + ann results in zero,			z=1
//						otherwise,		z=0
//
// if an + ann sum has high bit set,	m=1
//						otherwise,		m=0
//
static void SWEET64::adc64(union union_64 * an, union union_64 * ann)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	asm volatile (
		"	clc						\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_adc64%=:					\n"
		"	ld	%A3, %a0			\n"
		"	ld	%A1, %a5+			\n"
		"	adc	%A3, %A1			\n"
		"	st	%a0+, %A3			\n"
		"	or	%A2, %A3			\n"		// update zero flag
		"	dec	%A4					\n"
		"	brne l_adc64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
		: "e" (ann)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;
	z = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[0] = c;
		n->u8[1] = 0;
		enn += an->u8[(uint16_t)(x)];
		enn += ann->u8[(uint16_t)(x)];
		m = n->u8[0];
		c = n->u8[1];
		an->u8[(uint16_t)(x)] = m;
		z |= m;

	}
#endif // useAssemblyLanguage

	flagSet64(m, z, c);

}

// SWEET64 64-bit subtract instruction
//
// subtracts ann from an, and stores the result in an
// doubles as 64-bit compare operation (results are not stored)
//
// if an < ann, 		z=0, c=1
// if an == ann,		z=1, c=0
// if an > ann,			z=0, c=0
// if (an - ann) < 0,	m=1
// if (an - ann) >= 0,	m = 0
static void SWEET64::sbc64(union union_64 * an, union union_64 * ann, uint8_t sbcFlag)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	asm volatile (
		"	clc						\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_sub64%=:					\n"
		"	ld	%A3, %a0			\n"
		"	ld	%A1, %a5+			\n"
		"	sbc	%A3, %A1			\n"
		"	tst	%A6					\n"		// is this a subtract operation
		"	breq	c_sub64%=		\n"		// if not, just a compare, so throw away result
		"	st	%a0, %A3			\n"		// store result

		"c_sub64%=:					\n"
		"	or	%A2, %A3			\n"		// update zero flag
//		"	adiw	%a0, 1			\n"		// this does not work for some reason
		"	ld	__tmp_reg__, %a0+	\n"		// there is apparently no way to directly increment %a0
		"	dec	%A4					\n"
		"	brne l_sub64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
		: "e" (ann), "r" (sbcFlag)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;
	z = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[1] = c;
		n->u8[0] = c;
		enn += an->u8[(uint16_t)(x)];
		enn -= ann->u8[(uint16_t)(x)];
		m = n->u8[0];
		c = n->u8[1];
		if (sbcFlag) an->u8[(uint16_t)(x)] = m;
		z |= m;

	}
#endif // useAssemblyLanguage

	flagSet64(m, z, c);

}

static void SWEET64::registerTest64(union union_64 * an)
{

	uint8_t z;
	uint8_t m;
	uint8_t v;
	uint8_t x;

#ifdef useAssemblyLanguage
	asm volatile(
		"	mov	%A0, __zero_reg__	\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A3, 8				\n"		// initialize counter

		"l_tst64%=:					\n"
		"	ld	%A1, %a4+			\n"		// load temp register with byte of an
		"	or	%A0, %A1			\n"
		"	com	%A1					\n"		// invert temp register
		"	or	%A2, %A1			\n"
		"	dec	%A3					\n"
		"	brne l_tst64%=			\n"

		"	com	%A1					\n"		// restore temp register to original value
		: "+r" (z), "+r" (m), "+r" (v), "+r" (x)
		: "e" (an)
	);
#else // useAssemblyLanguage
	z = 0;
	v = 0;

	for (x = 0; x < 8; x++)
	{

		m = an->u8[(uint16_t)(x)];
		z |= m;
		if (m != 0xFF) v = 1;

	}
#endif // useAssemblyLanguage

	flagSet((m & 0x80), SWEET64minusFlag);
	flagSet((z == 0), SWEET64zeroFlag);
	flagSet((v == 0), SWEET64overflowFlag);

//	if (m & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
//	else SWEET64processorFlags &= ~(SWEET64minusFlag);

//	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
//	else SWEET64processorFlags |= (SWEET64zeroFlag);

//	if (v) SWEET64processorFlags &= ~(SWEET64overflowFlag);
//	else SWEET64processorFlags |= (SWEET64overflowFlag);

}

static void SWEET64::mult64(void)
{

	union union_64 * an = (union union_64 *)(&s64reg[s64reg2]);	// multiplier in an, result to an
	union union_64 * multiplicand = (union union_64 *)(&s64reg[s64reg5]);	// define multiplicand term as register 5
#ifdef useObsoleteMult64
	union union_64 * multiplier = (union union_64 *)(&s64reg[s64reg4]);	// define multiplier term as register 4

	copy64(multiplier, an); // load multiplier into register 4
	init64byt(an, 0); // zero out result

	sbc64(multiplicand, multiplier, 0); // is multiplier less than multiplicand?

	if (SWEET64processorFlags & SWEET64carryFlag) swap64(multiplicand, multiplier); // if not, swap them around

	registerTest64(multiplier);

	while (!(SWEET64processorFlags & SWEET64zeroFlag)) // while multiplier is non-zero
	{

		shr64(multiplier); // shift multiplier right by one bit

		if (SWEET64processorFlags & SWEET64carryFlag) adc64(an, multiplicand); // if the low bit of multiplier is set, add multiplicand to result

		SWEET64processorFlags &= ~(SWEET64carryFlag);
		shl64(multiplicand); // shift multiplicand left by one bit

	}

#else // useObsoleteMult64
#ifdef useAssemblyLanguage

	asm volatile(
		"	push	r0				\n"		// save this register which 'mul' will trash

		"	push	r1				\n"		// save this register which 'mul' will trash

		"	push	r12				\n"		// save result register original contents
		"	push	r13				\n"
		"	push	r14				\n"
		"	push	r15				\n"
		"	push	r16				\n"
		"	push	r17				\n"
		"	push	r18				\n"
		"	push	r19				\n"
		"	push	r20				\n"		// (result byte 9 is thrown away at end)
		"	push	r21				\n"		// (result byte 10 is thrown away at end)

		"	push	r22				\n"		// save multiplication counter original contents

		"	push	r23				\n"		// save secondary counter original contents

		"	push	r24				\n"		// save multiplication workspace

		"	push	r28				\n"		// save Y reg original contents
		"	push	r29				\n"

		"	ldi	r22, 0				\n"		// initialize counter

		"	ldi	r21, 0				\n"		// initialize result register
		"	mov	r20, r21			\n"
		"	mov	r19, r21			\n"
		"	mov	r18, r21			\n"
		"	mov	r17, r21			\n"
		"	mov	r16, r21			\n"
		"	mov	r15, r21			\n"
		"	mov	r14, r21			\n"
		"	mov	r13, r21			\n"
		"	mov	r12, r21			\n"

		"l1_mul64%=:				\n"
		"	push	r26				\n"		// save X reg original contents (multiplier)
		"	push	r27				\n"
		"	push	r30				\n"		// save Z reg original contents (multiplicand)
		"	push	r31				\n"

		"	mov	r23, r22			\n"		// initialize secondary counter
		"	ldi	r28, 12				\n"		// initialize Y reg as result register pointer
		"	ldi	r29, 0				\n"
		"	add	r28, r23			\n"		// point to byte offset in result register
		"	inc	r23					\n"		// bump secondary counter up by one
		"	add	r30, r23			\n"		// set up multiplicand pointer
		"	adc	r31, r29			\n"

		"l2_mul64%=:				\n"
		"	push	r28				\n"		// save Y reg result register pointer
		"	push	r29				\n"
		"	ld	r0, X+				\n"		// load in ascending order the byte of multiplier
		"	ld	r1, -Z				\n"		// load in descending order the byte of multiplicand
		"	mul	r0, r1				\n"		// perform multiplication (since mul will trash r0 and r1 anyway, why not use them for input?)
		"	ld	r24, Y				\n"		// load first intermediate byte of result register
		"	add	r24, r0				\n"		// add low byte of mul result
		"	st	Y+, r24				\n"		// save first intermediate byte of result register
		"	ld	r24, Y				\n"		// load next byte of result register
		"	adc	r24, r1				\n"		// add high byte of mul result
		"	st	Y+, r24				\n"		// save next byte of result register
		"	ld	r24, Y				\n"		// load third byte of result register
		"	adc	r24, r29			\n"		// add impromptu __zero_reg__ with carry
		"	st	Y+, r24				\n"		// save third byte of result register
		"	pop	r29					\n"		// restore Y reg result register pointer
		"	pop	r28					\n"

		"	dec	r23					\n"		// bump down secondary counter
		"	brne	l2_mul64%=		\n"

		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"
		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"

		"	inc	r22					\n"		// bump up multiplication counter
		"	cpi	r22, 8				\n"		// check if done
		"	brlo	l1_mul64%=		\n"		// if not, go process another byte

		"	st	X+, r12				\n"		// save result register to multiplier
		"	st	X+, r13				\n"
		"	st	X+, r14				\n"
		"	st	X+, r15				\n"
		"	st	X+, r16				\n"
		"	st	X+, r17				\n"
		"	st	X+, r18				\n"
		"	st	X+, r19				\n"

		"	pop	r29					\n"		// restore Y reg original contents
		"	pop	r28					\n"

		"	pop	r24					\n"		// restore multiplication workspace

		"	pop	r23					\n"		// restore secondary counter original contents

		"	pop	r22					\n"		// restore counter original contents

		"	pop	r21					\n"		// throw away result byte 10
		"	pop	r20					\n"		// throw away result byte 10
		"	pop	r19					\n"		// restore result register original contents
		"	pop	r18					\n"
		"	pop	r17					\n"
		"	pop	r16					\n"
		"	pop	r15					\n"
		"	pop	r14					\n"
		"	pop	r13					\n"
		"	pop	r12					\n"

		"	pop	r1					\n"		// restore this register which 'mul' trashed
		"	pop	r0					\n"		// restore this register which 'mul' trashed
		:
		: "x" (an), "z" (multiplicand)
	);

#else // useAssemblyLanguage
	union union_64 * multiplier = (union union_64 *)(&s64reg[s64reg4]);	// define multiplier term as register 4

	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	uint16_t pee;
	union union_16 * p = (union union_16 *)(&pee);

	uint8_t c;

	copy64(multiplier, an); // load multiplier
	init64byt(an, 0); // zero out result

	for (uint8_t m = 0; m < 8; m++)
	{

		for (uint8_t x = 0; x <= m; x++)
		{

			enn = multiplier->u8[(uint16_t)(x)] * multiplicand->u8[(uint16_t)(m - x)];

			c = 0;

			for (uint8_t i = 0; i < 2; i++)
			{

				if (m + i < 8)
				{

					p->u8[0] = c;
					p->u8[1] = 0;
					pee += an->u8[(uint16_t)(m + i)];
					pee += n->u8[(uint16_t)(i)];
					an->u8[(uint16_t)(m + i)] = p->u8[0];
					c = p->u8[1];

				}

			}

			if ((c) && (m < 6)) an->u8[(uint16_t)(m + 2)]++;

		}

	}

#endif // useAssemblyLanguage
#endif // useObsoleteMult64
}

#ifndef useSWEET64div
static void SWEET64::div64(void) // uses algorithm for non-restoring hardware division
{

	union union_64 * ann = (union union_64 *)(&s64reg[s64reg1]);	// remainder in ann
	union union_64 * divisor = (union union_64 *)(&s64reg[s64reg5]);
#ifdef useDiv64Assembly

	asm volatile(
		"	push	r28				\n"		// save Y reg original contents
		"	push	r29				\n"
		"	push	r25				\n"

		"	clr	r28					\n"		// point Y reg to start of register contents
		"	clr	r29					\n"
		"d64_iloop%=:				\n"
		"	ld	r25, y+				\n"		// save register original contents
		"	push	r25				\n"
		"	cpi	r28, 25				\n"
		"	brne	d64_iloop%=		\n"

		"	push	r30				\n"		// save Z reg original contents (divisor)
		"	push	r31				\n"
		"	push	r26				\n"		// save X reg original contents (remainder)
		"	push	r27				\n"

		"	clr	r25					\n"		// initialize zero-test register

		"	ldi	r26, 16				\n"		// point X reg to divisor register
		"	clr	r27					\n"
		"d64_cpd%=:					\n"
		"	ld	r24, z				\n"		// load divisor contents into divisor register
		"	or	r25, r24			\n"		// also perform check if divisor is zero
		"	st	x+, r24				\n"
		"	st	z+, r27				\n"		// store a zero in divisor variable
		"	cpi	r26, 24				\n"
		"	brne	d64_cpd%=		\n"

		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"

		"	tst	r25					\n"
		"	brne	d64_cont1%=		\n"

		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"

		"	clr	r24					\n"		// generate overflow value
		"	com	r24					\n"
		"	ldi	r25, 16				\n"		// set remainder contents as overflow value

		"d64_ovfl%=:				\n"
		"	st	x+, r16				\n"		// store overflow value into remainder and quotient
		"	dec	r25					\n"
		"	brne	d64_ovfl%=		\n"
		"	rjmp	d64_exit%=		\n"		// go exit

		"d64_cont1%=:				\n"
		"	push	r26				\n"		// save X reg original contents (remainder)
		"	push	r27				\n"

		"	clr	r0					\n"		// zero out remainder register
		"	clr	r1					\n"
		"	clr	r2					\n"
		"	clr	r3					\n"
		"	clr	r4					\n"
		"	clr	r5					\n"
		"	clr	r6					\n"
		"	clr	r7					\n"

		"	adiw	r26, 8			\n"		// skip over remainder

		"	ldi	r30, 8				\n"		// point Z reg to quotient register
		"	clr	r31					\n"
		"d64_cpq%=:					\n"
		"	ld	r24, x+				\n"		// move quotient contents from quotient variable into quotient register
		"	st	z+, r24				\n"
		"	cpi	r30, 16				\n"
		"	brne	d64_cpq%=		\n"

		"	ldi	r24, 64				\n"		// load bit count into counter

		"d64_loop0%=:				\n"
		"	tst	r15					\n"		// is high byte of quotient zero?
		"	brne	d64_loop1%=		\n"		// if not, proceed with division
		"	mov	r15, r14			\n"		// otherwise, do a byte shift leftward
		"	mov	r14, r13			\n"
		"	mov	r13, r12			\n"
		"	mov	r12, r11			\n"
		"	mov	r11, r10			\n"
		"	mov	r10, r9				\n"
		"	mov	r9, r8				\n"
		"	clr	r8					\n"
		"	subi	r24, 8			\n"		// bump down the bit count by 8
		"	brne	d64_loop0%=		\n"		// if bit count is non-zero, loop back

		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"
		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"
		"	rjmp	d64_cont4%=		\n"		// otherwise, div64 is done

		"d64_add%=:					\n"
		"	add	r0, r16				\n"		// add divisor to remainder
		"	adc	r1, r17				\n"
		"	adc	r2, r18				\n"
		"	adc	r3, r19				\n"
		"	adc	r4, r20				\n"
		"	adc	r5, r21				\n"
		"	adc	r6, r22				\n"
		"	adc	r7, r23				\n"
		"	ret						\n"

		"d64_loop1%=:				\n"
		"	lsl	r8					\n"		// shift quotient left one bit
		"	rol	r9					\n"
		"	rol	r10					\n"
		"	rol	r11					\n"
		"	rol	r12					\n"
		"	rol	r13					\n"
		"	rol	r14					\n"
		"	rol	r15					\n"
		"	rol	r0					\n"		// shift remainder left one bit, shifting in most significant bit of quotient
		"	rol	r1					\n"
		"	rol	r2					\n"
		"	rol	r3					\n"
		"	rol	r4					\n"
		"	rol	r5					\n"
		"	rol	r6					\n"
		"	rol	r7					\n"
		"	brcc	d64_l1s%=		\n"		// if last operation resulted in a positive remainder, jump ahead

		"	rcall	d64_add%=		\n"		// go add divisor to remainder
		"	rjmp	d64_l1b%=		\n"

		"d64_l1s%=:					\n"
		"	sub	r0, r16				\n"		// subtract divisor from remainder
		"	sbc	r1, r17				\n"
		"	sbc	r2, r18				\n"
		"	sbc	r3, r19				\n"
		"	sbc	r4, r20				\n"
		"	sbc	r5, r21				\n"
		"	sbc	r6, r22				\n"
		"	sbc	r7, r23				\n"

		"d64_l1b%=:					\n"
		"	tst	r7					\n"
		"	brmi	d64_l1c%=		\n"
		"	inc	r8					\n"
		"d64_l1c%=:					\n"
		"	dec	r24					\n"
		"	brne	d64_loop1%=		\n"

		"	tst	r7					\n"
		"	brpl	d64_cont2%=		\n"		// if last operation resulted in a positive remainder, jump ahead
		"	rcall	d64_add%=		\n"		// go add divisor to remainder

		"d64_cont2%=:				\n"
		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"
		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"

		"	lsr	r23					\n"		// divide divisor by 2
		"	ror	r22					\n"
		"	ror	r21					\n"
		"	ror	r20					\n"
		"	ror	r19					\n"
		"	ror	r18					\n"
		"	ror	r17					\n"
		"	ror	r16					\n"

		"	cp	r7, r23				\n"		// compare r8[7] to d8[7]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r6, r22				\n"		// compare r8[6] to d8[6]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r5, r21				\n"		// compare r8[5] to d8[5]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r4, r20				\n"		// compare r8[4] to d8[4]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r3, r19				\n"		// compare r8[3] to d8[3]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r2, r18				\n"		// compare r8[2] to d8[2]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r1, r17				\n"		// compare r8[1] to d8[1]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r0, r16				\n"		// compare r8[0] to d8[0]

		"d64_cont3%=:				\n"
		"	brlo	d64_cont4%=		\n"		// if remainder is less than divisor / 2, skip ahead
		"	inc	r24					\n"
		"	st	z, r24				\n"		// store quotient adjust value

		"d64_cont4%=:				\n"
		"	clr	r30					\n"		// point z register to point to remainder register
		"	clr	r31					\n"
		"	ldi	r25, 16				\n"		// initialize counter
		"d64_saveqr%=:				\n"
		"	ld	r24, z+				\n"		// move remainder contents to remainder variable
		"	st	x+, r24				\n"		// also move quotient contents to quotient variable
		"	dec	r25					\n"
		"	brne	d64_saveqr%=	\n"

		"d64_exit%=:				\n"
		"	ldi	r28, 25				\n"		// point Y reg to end of register contents
		"	clr	r29					\n"
		"d64_eloop%=:				\n"
		"	pop	r25					\n"		// restore register original contents
		"	st	-y, r25				\n"
		"	tst	r28					\n"
		"	brne	d64_eloop%=		\n"

		"	pop	r25					\n"
		"	pop	r29					\n"		// restore Y reg original contents
		"	pop	r28					\n"
		:
		: "x" (ann), "z" (divisor)
	);

#else // useDiv64Assembly
	union union_64 * an = (union union_64 *)(&s64reg[s64reg2]);	// quotient in an

	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t s;

	init64byt(ann, 0);							// zero out remainder

	registerTest64(divisor);

	if (SWEET64processorFlags & SWEET64zeroFlag)			// if divisor is zero, mark as overflow, then exit
	{

		init64byt(an, 1); // zero out remainder
		sbc64(ann, an, 1); // subtract 1 from zeroed-out result to generate overflow value
		copy64(an, ann); // copy overflow value to remainder

	}
	else
	{

		x = 64;							// start off with a dividend size of 64 bits

		y = 7; //
		while ((x) && (an->u8[(unsigned int)(y)] == 0))		// examine dividend for leading zero bytes
		{

			y--;						// if this byte is zero, skip to look at next byte
			x -= 8;						// shrink dividend size by 8 bits

		}

		if ((x) && (y < 7)) // if upper-region zero bytes were found, shift quotient left to eliminate them
		{

			for (z = 7; z < 8; z--)
			{

				if (y < 7) an->u8[(unsigned int)(z)] = an->u8[(unsigned int)(y)];
				else an->u8[(unsigned int)(z)] = 0;
				y--;

			}

		}

		s = 0;

		while (x) // while there are still bits in dividend to process
		{

			SWEET64processorFlags &= ~(SWEET64carryFlag); // default is "previous operation resulted in A < 0"
			shl64(an);					// shift Q left one bit
			shl64(ann);					// shift A left one bit

			if (s) adc64(ann, divisor); // add M to A if previous operation resulted in A < 0
			else sbc64(ann, divisor, 1); // subtract M from A if previous operation resulted in A >= 0

			s = (ann->u8[7] & 0x80); // get previous operation sign bit

			if (s == 0) an->u8[0] |= 0x01; // if previous operation resulted in A >= 0, then adjust quotient

			x--; // reduce dividend bit count by one

		}

		if (s) adc64(ann, divisor); // if most recent operation resulted in A < 0, make A (remainder) positive by adding M to it

		// provide a means to round to the next integer
		// if (R / D) < 0.5, then round down (add 0 to quotient)
		// otherwise, if (R / D) >= 0.5, round up (add 1 to quotient)

		shr64(divisor);			// divide divisor by 2 by shifting right one bit
		sbc64(ann, divisor, 0);

		if (SWEET64processorFlags & SWEET64carryFlag) s = 0; // if remainder is less than divisor / 2
		else s = 1;	// otherwise, store a 1

		init64byt(divisor, s); // store quotient adjust term

	}

#endif // useDiv64Assembly
}

#endif // useSWEET64div
static void SWEET64::init64byt(union union_64 * an, uint8_t byt)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"	st	%a0,   %A1	\n"
		"	std	%a0+1, __zero_reg__	\n"
		"	std	%a0+2, __zero_reg__	\n"
		"	std	%a0+3, __zero_reg__	\n"
		"	std	%a0+4, __zero_reg__	\n"
		"	std	%a0+5, __zero_reg__	\n"
		"	std	%a0+6, __zero_reg__	\n"
		"	std	%a0+7, __zero_reg__	\n"
		: "+e" (an)
		: "r" (byt)
	);
#else // useAssemblyLanguage
	for (uint8_t x = 1; x < 8; x++) an->u8[(uint16_t)(x)] = 0;
	an->u8[0] = byt;
#endif // useAssemblyLanguage

}

static void SWEET64::init64(union union_64 * an, unsigned long dWordL)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"	st	%a0,   %A1	\n"
		"	std	%a0+1, %B1	\n"
		"	std	%a0+2, %C1	\n"
		"	std	%a0+3, %D1	\n"
		"	std	%a0+4, __zero_reg__	\n"
		"	std	%a0+5, __zero_reg__	\n"
		"	std	%a0+6, __zero_reg__	\n"
		"	std	%a0+7, __zero_reg__	\n"
		: "+e" (an)
		: "r" (dWordL)
	);
#else // useAssemblyLanguage
	an->ul[1] = 0;
	an->ul[0] = dWordL;
#endif // useAssemblyLanguage

}

static void SWEET64::flagSet(uint8_t condition, const uint8_t flag)
{

	if (condition) SWEET64processorFlags |= (flag);
	else SWEET64processorFlags &= ~(flag);

}

static void SWEET64::flagSet64(uint8_t n, uint8_t z, uint8_t c)
{

	flagSet((n & 0x80), SWEET64minusFlag);
	flagSet((z == 0), SWEET64zeroFlag);
	flagSet((c), SWEET64carryFlag);

//	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
//	else SWEET64processorFlags &= ~(SWEET64minusFlag);

//	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
//	else SWEET64processorFlags |= (SWEET64zeroFlag);

//	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
//	else SWEET64processorFlags &= ~(SWEET64carryFlag);

}

// Menu display / screen cursor support section

// the following screen index defines are for the legacy MPGuino screen displays
#define nextAllowedValue 0
static const uint8_t mainScreenIdx =				nextAllowedValue;
#define nextAllowedValue mainScreenIdx + 1
#ifdef useBigFE
static const uint8_t bigFEscreenIdx =				nextAllowedValue;
#define nextAllowedValue bigFEscreenIdx + 1
#endif // useBigFE
#ifdef useBarFuelEconVsTime
static const uint8_t barFEvTscreenIdx =				nextAllowedValue;
#define nextAllowedValue barFEvTscreenIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t barFEvSscreenIdx =				nextAllowedValue;
#define nextAllowedValue barFEvSscreenIdx + 1
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
static const uint8_t bigDTEscreenIdx =				nextAllowedValue;
#define nextAllowedValue bigDTEscreenIdx + 1
#endif // useBigDTE
#ifdef useBigTTE
static const uint8_t bigTTEscreenIdx =				nextAllowedValue;
#define nextAllowedValue bigTTEscreenIdx + 1
#endif // useBigTTE
#ifdef useCPUreading
static const uint8_t CPUmonScreenIdx =				nextAllowedValue;
#define nextAllowedValue CPUmonScreenIdx + 1
#endif // useCPUreading
#ifdef useClockDisplay
static const uint8_t clockShowScreenIdx =			nextAllowedValue;
#define nextAllowedValue clockShowScreenIdx + 1
#endif // useClockDisplay

static const uint8_t mainScreenSize =				nextAllowedValue;	// this variable is used to figure out how many menu levels the main screen has

// the following screen index defines are for the parameter setting screen, and any other extra features that have been added
static const uint8_t settingScreenIdx =				nextAllowedValue;
static const uint8_t fuelSettingsScreenIdx =		settingScreenIdx + 1;
static const uint8_t VSSsettingsScreenIdx =			fuelSettingsScreenIdx + 1;
static const uint8_t tankSettingsScreenIdx =		VSSsettingsScreenIdx + 1;
#define nextAllowedValue tankSettingsScreenIdx + 1
#ifdef useChryslerMAPCorrection
static const uint8_t CRFICsettingsScreenIdx =		nextAllowedValue;
#define nextAllowedValue CRFICsettingsScreenIdx + 1
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
static const uint8_t acdSettingsScreenIdx =			nextAllowedValue;
#define nextAllowedValue acdSettingsScreenIdx + 1
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
static const uint8_t timeoutSettingsScreenIdx =		nextAllowedValue;
static const uint8_t miscSettingsScreenIdx =		timeoutSettingsScreenIdx + 1;
#define nextAllowedValue miscSettingsScreenIdx + 1
#ifdef useDragRaceFunction
static const uint8_t dragRaceIdx =					nextAllowedValue;
#define nextAllowedValue dragRaceIdx + 1
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const uint8_t coastdownIdx =					nextAllowedValue;
#define nextAllowedValue coastdownIdx + 1
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
static const uint8_t debugReadingIdx =				nextAllowedValue;
#define nextAllowedValue debugReadingIdx + 1
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
static const uint8_t pressureCorrectionIdx =		nextAllowedValue;
#define nextAllowedValue pressureCorrectionIdx + 1
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
static const uint8_t analogReadIdx =				nextAllowedValue;
#define nextAllowedValue analogReadIdx + 1
#endif // useDebugAnalog
#ifdef useTestButtonValues
static const uint8_t buttonViewIdx =				nextAllowedValue;
#define nextAllowedValue buttonViewIdx + 1
#endif // useTestButtonValues

static const char menuVisibleSize =					nextAllowedValue;	// this variable is used to figure out how many total menu levels are visible

// the following screen index defines do not show up in the top-down menu list
const uint8_t menuScreenIdx =						nextAllowedValue;
const uint8_t parameterEditScreenIdx =				menuScreenIdx + 1;
#define nextAllowedValue parameterEditScreenIdx + 1
#ifdef useClockDisplay
const uint8_t clockSetScreenIdx =					nextAllowedValue;
#define nextAllowedValue clockSetScreenIdx + 1
#endif // useClockDisplay
#ifdef useSavedTrips
static const uint8_t tripSaveScreenIdx =			nextAllowedValue;
#define nextAllowedValue tripSaveScreenIdx + 1
#endif // useSavedTrips
#ifdef usePartialRefuel
static const uint8_t partialRefuelScreenIdx =		nextAllowedValue;
#define nextAllowedValue partialRefuelScreenIdx + 1
#endif // usePartialRefuel
#ifdef useScreenEditor
const uint8_t screenEditIdx =						nextAllowedValue;
#define nextAllowedValue screenEditIdx + 1
#endif // useScreenEditor

static const char menuTotalSize =					nextAllowedValue;

typedef struct
{

	uint8_t buttonCode;
	void (* buttonCommand)(void);

} buttonVariable;

typedef struct
{

	uint8_t menuIndex;
	uint8_t modeIndex;
	uint8_t modeYcount;
	uint8_t modeXcount;
	displayHandlerFunc screenDisplayHandler;
//	void (* modeDisplay)(void);
//	void (* modeCursorUpdate)(void);
	const buttonVariable (* modeButtonList);

} modeInformation;

static const buttonVariable bpListMenu[] PROGMEM = {
	 {btnShortPressC,	menu::select}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::shortRight}
		,{btnShortPressU,	cursor::shortLeft}
		,{btnLongPressU,	doNextBright}
		,{btnShortPressL,	doNothing}
		,{btnShortPressR,	menu::select}
#else // useButtonCrossConfig
		,{btnShortPressR,	cursor::shortRight}
		,{btnShortPressL,	cursor::shortLeft}
		,{btnLongPressC,	doNextBright}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

static const buttonVariable bpListMain[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useSavedTrips
		,{btnShortPressDR,	tripSupport::goSaveCurrent}
		,{btnShortPressDL,	tripSupport::goSaveTank}
	#endif // useSavedTrips
	#ifdef useScreenEditor
		,{btnLongPressC,	doGoScreenEdit}
	#endif // useScreenEditor
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useScreenEditor
		,{btnLongPressC,	doGoScreenEdit}
	#endif // useScreenEditor
	#ifdef useCPUreading
		,{btnLongPressLCR,	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

static const buttonVariable bpListSetting[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::entry}
	,{btnShortPressLR,	parameterEdit::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnShortPressD,	cursor::shortRight}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp, 		noSupport}
};

static const buttonVariable bpListParameterEdit[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	parameterEdit::cancel}
	,{btnLongPressLR,	parameterEdit::cancel}
	,{btnLongPressC,	parameterEdit::save}
	,{btnShortPressLR,	parameterEdit::save}
	,{btnLongPressR,	parameterEdit::readInitial}
#ifdef useButtonCrossConfig
		,{btnShortPressUL,	parameterEdit::findLeft}
		,{btnShortPressUR,	parameterEdit::findRight}
		,{btnLongPressD,	parameterEdit::readMinValue}
		,{btnLongPressU,	parameterEdit::readMaxValue}
		,{btnShortPressU,	parameterEdit::changeDigitUp}
		,{btnShortPressD,	parameterEdit::changeDigitDown}
		,{btnShortPressC,	parameterEdit::save}
#else // useButtonCrossConfig
		,{btnShortPressLC,	parameterEdit::findLeft}
		,{btnShortPressCR,	parameterEdit::findRight}
		,{btnLongPressCR,	parameterEdit::readMinValue}
		,{btnLongPressLC,	parameterEdit::readMaxValue}
		,{btnShortPressC,	parameterEdit::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#ifdef useBigDigitDisplay
static const buttonVariable bpListBigNum[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useBigDigitDisplay
#ifdef useClockDisplay
static const buttonVariable bpListClockShow[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
	,{btnLongPressC,	clockSet::entry}
	,{btnShortPressLR,	clockSet::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

static const buttonVariable bpListClockSet[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	clockSet::cancel}
	,{btnLongPressLR,	clockSet::cancel}
	,{btnLongPressC,	clockSet::set}
	,{btnShortPressLR,	clockSet::set}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	clockSet::changeDigitUp}
		,{btnShortPressD,	clockSet::changeDigitDown}
		,{btnShortPressC,	clockSet::set}
#else // useButtonCrossConfig
		,{btnShortPressC,	clockSet::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useClockDisplay
#ifdef useCPUreading
static const buttonVariable bpListCPUmonitor[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useCPUreading
#ifdef usePartialRefuel
static const buttonVariable bpListPartialRefuel[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	doReturnToMainScreen}
	,{btnLongPressLR,	doReturnToMainScreen}
	,{btnLongPressC,	partialRefuel::longSelect}
	,{btnShortPressLR,	partialRefuel::select}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	partialRefuel::select}
		,{btnLongPressU,	partialRefuel::longSelect}
		,{btnShortPressD,	cursor::shortRight}
		,{btnShortPressC,	doReturnToMainScreen}
#else // useButtonCrossConfig
		,{btnShortPressC,	partialRefuel::select}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // usePartialRefuel
#if defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog)
static const buttonVariable bpListMiscViewer[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog)
#ifdef useTestButtonValues
static const buttonVariable bpListButtonView[] PROGMEM = {
	 {buttonsUp,		doNothing}
};

#endif // useTestButtonValues
#ifdef useSavedTrips
static const buttonVariable bpListTripSave[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	doReturnToMainScreen}
	,{btnLongPressLR,	doReturnToMainScreen}
	,{btnShortPressC,	doReturnToMainScreen}
#ifdef useButtonCrossConfig
		,{btnLongPressU,	tripSupport::select}
		,{btnShortPressD,	cursor::shortRight}
#else // useButtonCrossConfig
		,{btnLongPressC,	tripSupport::select}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useSavedTrips
#ifdef useBarGraph
static const buttonVariable bpListBarGraph[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useBarFuelEconVsSpeed
		,{btnLongPressUC,	bgFEvsSsupport::reset}
	#endif // useBarFuelEconVsSpeed
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useBarFuelEconVsSpeed
		,{btnLongPressLCR,	bgFEvsSsupport::reset}
	#endif // useBarFuelEconVsSpeed
	#ifdef useCPUreading
		,{btnLongPressLR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useBarGraph
#ifdef useButtonCrossConfig
#ifdef useScreenEditor
static const buttonVariable bpListScreenEdit[] PROGMEM = {
	{btnShortPressU,	doScreenEditBump}
	,{btnShortPressD,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressU,	doSaveScreen}
	,{buttonsUp,		noSupport}
};

#endif // useScreenEditor
#ifdef useDragRaceFunction
static const buttonVariable bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
	,{buttonsUp,		noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonVariable bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
	,{buttonsUp,		noSupport}
};

#endif // useCoastDownCalculator
#else // useButtonCrossConfig
#ifdef useScreenEditor
static const buttonVariable bpListScreenEdit[] PROGMEM = {
	{btnShortPressC,	doScreenEditBump}
	,{btnLongPressLR,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressC,	doSaveScreen}
	,{buttonsUp,		noSupport}
};

#endif // useScreenEditor
#ifdef useDragRaceFunction
static const buttonVariable bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
	,{buttonsUp,		noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonVariable bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
	,{buttonsUp,		noSupport}
};

#endif // useCoastDownCalculator
#endif //useButtonCrossConfig
static const char menuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
	"Main Display\r"
#ifdef useBigFE
	"Big FuelEcon\r"
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	"FE/Time\r"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"FE/Speed\r"
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	"Big DistToE\r"
#endif // useBigDTE
#ifdef useBigTTE
	"Big TimeToE\r"
#endif // useBigTTE
#ifdef useCPUreading
	"CPU Info\r"
#endif // useCPUreading
#ifdef useClockDisplay
	"Clock\r"
#endif // useClockDisplay
	"Settings Disp\r"
	"Settings Inj\r"
	"Settings VSS\r"
	"SettingsTankQty\r"
#ifdef useChryslerMAPCorrection
	"SettingsChryslr\r"
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	"SettingsVehTest\r"
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	"SettingsTimeout\r"
	"Settings Misc\r"
#ifdef useDragRaceFunction
	"Accel Test\r"
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	"Coastdown\r"
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	"Sensor Sim\r"
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	"Pressures\r"
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	"ADC readings\r"
#endif // useDebugAnalog
#ifdef useTestButtonValues
	"Button Values\r"
#endif // useTestButtonValues
};

uint8_t screenCursor[(unsigned int)(menuTotalSize)];

static const modeInformation screenParameters[(unsigned int)(menuTotalSize)] PROGMEM = {

// the following screen entries are in the top-down menu list

	{mainScreenIdx,					mainScreenIdx,						mainScreenSize,		displayPageCount,			mainScreenDisplayHandler,			bpListMain}
#ifdef useBigFE
	,{bigFEscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							bigDigit::displayHandler,			bpListBigNum}
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,{barFEvTscreenIdx,				mainScreenIdx,						mainScreenSize,		4,							barGraphSupport::displayHandler,	bpListBarGraph}
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,{barFEvSscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							barGraphSupport::displayHandler,	bpListBarGraph}
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,{bigDTEscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							bigDigit::displayHandler,			bpListBigNum}
#endif // useBigDTE
#ifdef useBigTTE
	,{bigTTEscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							bigDigit::displayHandler,			bpListBigNum}
#endif // useBigTTE
#ifdef useCPUreading
	,{CPUmonScreenIdx,				mainScreenIdx | 0x80,				mainScreenSize,		1,							systemInfo::displayHandler,			bpListCPUmonitor}
#endif // useCPUreading
#ifdef useClockDisplay
	,{clockShowScreenIdx,			mainScreenIdx | 0x80,				mainScreenSize,		1,							bigDigit::displayHandler,			bpListClockShow}
#endif // useClockDisplay
	,{settingScreenIdx,				settingScreenIdx | 0x80,			1,					eePtrSettingsDispLen,		settings::displayHandler,			bpListSetting}
	,{fuelSettingsScreenIdx,		fuelSettingsScreenIdx | 0x80,		1,					eePtrSettingsInjLen,		settings::displayHandler,			bpListSetting}
	,{VSSsettingsScreenIdx,			VSSsettingsScreenIdx | 0x80,		1,					eePtrSettingsVSSlen,		settings::displayHandler,			bpListSetting}
	,{tankSettingsScreenIdx,		tankSettingsScreenIdx | 0x80,		1,					eePtrSettingsTankLen,		settings::displayHandler,			bpListSetting}
#ifdef useChryslerMAPCorrection
	,{CRFICsettingsScreenIdx,		CRFICsettingsScreenIdx | 0x80,		1,					eePtrSettingsCRFIClen,		settings::displayHandler,			bpListSetting}
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,{acdSettingsScreenIdx,			acdSettingsScreenIdx | 0x80,		1,					eePtrSettingsACDlen,		settings::displayHandler,			bpListSetting}
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,{timeoutSettingsScreenIdx,		timeoutSettingsScreenIdx | 0x80,	1,					eePtrSettingsTimeoutLen,	settings::displayHandler,			bpListSetting}
	,{miscSettingsScreenIdx,		miscSettingsScreenIdx | 0x80,		1,					eePtrSettingsMiscLen,		settings::displayHandler,			bpListSetting}
#ifdef useDragRaceFunction
	,{dragRaceIdx,	1,	4,	accelerationTest::goDisplay,	doNothing,	bpListDragRace}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,{coastdownIdx | 0x80,	1,	3,	coastdown::goDisplay,	doNothing,	bpListCoastdown}
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,{debugReadingIdx,				debugReadingIdx | 0x80,				1,					4,							debugReading::displayHandler,		bpListMiscViewer}
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	,{pressureCorrectionIdx,		pressureCorrectionIdx | 0x80,		1,					1,							pressureCorrect::displayHandler,	bpListMiscViewer}
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	,{analogReadIdx,				analogReadIdx | 0x80,				1,					1,							analogReadViewer::displayHandler,	bpListMiscViewer}
#endif // useDebugAnalog
#ifdef useTestButtonValues
	,{buttonViewIdx,				buttonViewIdx | 0x80,				1,					1,							buttonView::displayHandler,			bpListButtonView}
#endif // useTestButtonValues

// the following screen entries do not show up in the top-down menu list

	,{0,							menuScreenIdx | 0x80,				1,					menuVisibleSize,			menu::displayHandler,				bpListMenu}
	,{0,							parameterEditScreenIdx | 0x80,		1,					12,							parameterEdit::displayHandler,		bpListParameterEdit}
#ifdef useClockDisplay
	,{0,							clockSetScreenIdx | 0x80,			1,					4,							bigDigit::displayHandler,			bpListClockSet}
#endif // useClockDisplay
#ifdef useSavedTrips
	,{0,							tripSaveScreenIdx,					1,					3,							tripSupport::displayHandler,		bpListTripSave}
#endif // useSavedTrips
#ifdef usePartialRefuel
	,{0,							partialRefuelScreenIdx | 0x80,		1,					3,							partialRefuel::displayHandler,		bpListPartialRefuel}
#endif // usePartialRefuel
#ifdef useScreenEditor
	,{screenEditIdx | 0x80,	1,	mainScreenDisplayFormatSize * 2,	doScreenEditDisplay,	doCursorUpdateScreenEdit,	bpListScreenEdit}
#endif // useScreenEditor
};

volatile uint8_t cursorXdirection;
volatile uint8_t menuTop;

static uint8_t menu::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t i;
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			menuTop = cursorPos;
			if ((cursorXdirection & 0x80) == 0)
			{

				menuTop--;
				if (menuTop > menuVisibleSize) menuTop += menuVisibleSize;

			}

		case menuOutputDisplayIdx:
			i = menuTop;

			for (uint8_t x = 0; x < 2; x++)
			{

				text::gotoXY(devLCD, 0, x);
				text::charOut(devLCD, ((i == cursorPos) ? '>' : ' ' ));
				text::stringOut(devLCD, menuTitles, i);
				i++;
				if (i >= menuVisibleSize) i = 0;

			}
			break;

		default:
			break;

	}

	return retVal;

}

static void menu::entry(void)
{

	cursor::moveAbsolute(menuScreenIdx, 255); // go to the menu screen level

}

static void menu::select(void)
{

	uint8_t i;

	i = screenCursor[(unsigned int)(menuScreenIdx)];

	cursor::moveAbsolute(pgm_read_byte(&screenParameters[(unsigned int)(i)].menuIndex), 255);

}

/* LCD screen cursor manipulation section */

static void cursor::screenLevelEntry(const char * str, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	printStatusMessage(str);

}

static void cursor::screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	printStatusMessage(str, strIdx);

}

static void cursor::moveAbsolute(uint8_t positionY, uint8_t positionX)
{

	uint8_t levelChangeFlag;

	if (menuLevel == positionY) levelChangeFlag = 0; // same level
	else
	{

		levelChangeFlag = 1;
		menuLevel = positionY;

	}

	cursorXdirection = 0;
	positionY = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount);

	switch (positionX)
	{

		case 255:
			break;

		case 254:
			screenCursor[(unsigned int)(menuLevel)] = positionX - 1;
			break;

		default:
			if (positionX < positionY) screenCursor[(unsigned int)(menuLevel)] = positionX;
			break;

	}

	updateAfterMove(levelChangeFlag);

}

static void cursor::moveRelative(uint8_t moveY, uint8_t moveX)
{

	uint8_t wrapAroundFlag;
	uint8_t levelChangeFlag;
	uint8_t v;
	uint8_t maxVal;
	uint8_t x; // base menu level

	x = (pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeIndex) & 0x7F); // base menu level
	levelChangeFlag = 0;

	wrapAroundFlag = 0; // initially, signal that no wrap-around occurred

	if (moveX) // if movement within the screen level is called for
	{

		if (moveX & 0x80) cursorXdirection = 0xFF;
		else cursorXdirection = 0x01;

		maxVal = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount); // "horizontal" size

		v = screenCursor[(unsigned int)(menuLevel)] + cursorXdirection; // perform move

		if (v == maxVal) // if screen cursor bumped up to screen level size
		{

			v = 0; // wrap around
			wrapAroundFlag = 1; // signal wrap-around occurred

		}

		if (v > maxVal) // if screen cursor bumped down past lower limit
		{

			v = maxVal - 1; // wrap around
			wrapAroundFlag = 1;

		}

		if (wrapAroundFlag) moveY = cursorXdirection; // if wrap-around occurred, signal direction to next screen level

		screenCursor[(unsigned int)(menuLevel)] = v;

	}
	else cursorXdirection = 0;

	if (moveY)
	{

		if (moveY & 0x80) moveY = 0xFF;
		else moveY = 0x01;

		maxVal = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeYcount); // "vertical" size

		v = menuLevel - x + moveY;

		if (v == maxVal) v = 0;
		if (v > maxVal) v = maxVal - 1;

		v += x;
		if (menuLevel != v)
		{

			menuLevel = v;
			levelChangeFlag = 1;

		}

		if (wrapAroundFlag) // if wrap-around occurred, look at direction
		{

			if (moveY == 1) v = 0; // if direction was forward, set current cursor to zero
			else v = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount) - 1; // else, set current cursor to end of new menu level

			screenCursor[(unsigned int)(menuLevel)] = v;

		}
		else cursorXdirection = 0;

	}

	updateAfterMove(levelChangeFlag);

}

static void cursor::updateAfterMove(uint8_t levelChangeFlag)
{

	uint8_t cp;
	uint8_t cf;

	cp = screenCursor[(unsigned int)(menuLevel)];

	// call indexed support section cursor update function to update any section-specific data
	if (levelChangeFlag) cf = menuEntryIdx;
	else cf = menuCursorUpdateIdx;

	((displayHandlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(cf, cp, 0);

}

static void cursor::shortLeft(void)
{

	cursor::moveRelative(0, 255); // go to previous option in current level

}

static void cursor::shortRight(void)
{

	cursor::moveRelative(0, 1); // go to next option in current level

}

static void cursor::longLeft(void)
{

	cursor::moveRelative(255, 0); // go to last option in previous level

}

static void cursor::longRight(void)
{

	cursor::moveRelative(1, 0); // go to next option in next level

}

static void printStatusMessage(const char * s)
{

	initStatusLine();
	text::stringOut(devLCD, s);
	execStatusLine();

}

static void printStatusMessage(const char * s, uint8_t strIdx)
{

	printStatusMessage(findStr(s, strIdx));

}

void doNothing(void)
{
}

void noSupport(void)
{

	initStatusLine();
	text::stringOut(devLCD, PSTR("Btn 0x"));
	text::hexByteOut(devLCD, buttonPress);
	text::stringOut(devLCD, PSTR(" Pressed"));
	execStatusLine();

}

void initStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = 0; // end hold delay countdown
	timer0Command &= ~(t0cDisplayDelay); // cancel display delay

	SREG = oldSREG; // restore interrupt flag status

#ifdef blankScreenOnMessage
	text::charOut(devLCD, 0x0C); // clear the entire screen
#else // blankScreenOnMessage
	text::gotoXY(devLCD, 0, 0); // go to the first line
#endif // blankScreenOnMessage

}

void execStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = holdDelay; // start hold delay countdown
	timer0Command |= (t0cDisplayDelay); // make display delay active

	SREG = oldSREG; // restore interrupt flag status

}

const char * findStr(const char * str, uint8_t strIdx)
{

	uint8_t c;

	while (strIdx)
	{

		do c = pgm_read_byte(str++);
		while ((c) && (c != 0x0D));

		strIdx--;

	}

	return str;

}

/* text string output section */

static void text::newLine(interfaceDevice &dev)
{

	charOut(dev, 0x0D);

}

static void text::gotoXY(interfaceDevice &dev, uint8_t xPos, uint8_t yPos)
{

	charOut(dev, 0x80 + yPos * 20 + xPos);

}

static uint8_t text::charOut(interfaceDevice &dev, uint8_t chr)
{

	uint8_t retVal;

	retVal = 1;

	if (dev.chrOut)
	{

		switch (chr)
		{

			case 0x00:	// defined as end-of-string
				dev.controlFlags |= (odvFlagEnableOutput);
				retVal = 0;
				break;

			case 0xEB:	// disable device output for metric mode
				if (metricFlag & metricMode) dev.controlFlags &= ~(odvFlagEnableOutput);
				else dev.controlFlags |= (odvFlagEnableOutput);
				break;

			case 0xEC: // toggle device output enable
				dev.controlFlags ^= (odvFlagEnableOutput);
				break;

			case 0xED: // enable device output
				dev.controlFlags |= (odvFlagEnableOutput);
				break;

			case 0xEE:	// disable device output
				dev.controlFlags &= ~(odvFlagEnableOutput);
				break;

			case 0x0D:	// also defined as end of string
				retVal = 0;
				dev.controlFlags |= (odvFlagEnableOutput);
				dev.chrOut(0x0D);
				if (dev.controlFlags & odvFlagCRLF) dev.chrOut(0x0A);
				break;

			case 0xF0 ... 0xF7: // print CGRAM character
				chr &= 0x07;
			case 0x20 ... 0x7F: // print normal character
				if (dev.controlFlags & odvFlagEnableOutput)
				{

					if (dev.controlFlags & odvFlagShootBlanks) chr = ' ';
					dev.chrOut(chr);

				}
				break;

			default:
				dev.chrOut(chr);
				break;

		}
	}
	else retVal = 0;

	return retVal;

}

static void text::setModeOnCondition(interfaceDevice &dev, uint8_t condition, uint8_t odvFlag)
{

	if (condition) dev.controlFlags |= (odvFlag);
	else dev.controlFlags &= ~(odvFlag);

}

static void text::stringOut(interfaceDevice &dev, char * str)
{

	while (charOut(dev, * str++));

}

static void text::stringOut(interfaceDevice &dev, const char * str)
{

	while (charOut(dev, pgm_read_byte(str++))) ;

}

static void text::stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx)
{

	stringOut(dev, findStr(str, strIdx));

}

static void text::hexNybbleOut(interfaceDevice &dev, uint8_t val)
{

	val &= 0x0F;
	val |= 0x30;
	if (val > 0x39) val += 0x07;
	charOut(dev, val);

}

static void text::hexByteOut(interfaceDevice &dev, uint8_t val)
{

	hexNybbleOut(dev, val >> 4);
	hexNybbleOut(dev, val);

}

static void text::hexWordOut(interfaceDevice &dev, uint16_t val)
{

	union union_16 * vee = (union union_16 *) &val;

	for (uint8_t i = 1; i < 2; i--) hexByteOut(dev, vee->u8[i]);

}

static void text::hexDWordOut(interfaceDevice &dev, uint32_t val)
{

	union union_32 * vee = (union union_32 *) &val;

	for (uint8_t i = 3; i < 4; i--) hexByteOut(dev, vee->u8[i]);

}

static void text::hexLWordOut(interfaceDevice &dev, uint64_t * val)
{

	union union_64 * vee = (union union_64 *) val;

	for (uint8_t i = 7; i < 8; i--) hexByteOut(dev, vee->u8[i]);

}

static uint8_t text::numberOut(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	calcFuncObj thisCalcFuncObj;

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, strBuff, windowLength, decimalFlag); // perform the required decimal formatting
	stringOut(dev, thisCalcFuncObj.strBuffer); // output the number

	return thisCalcFuncObj.calcFmtIdx;

}

#define nextAllowedValue 0 // S64programList
const uint8_t calcFormatTimeHHmmSSIdx =				nextAllowedValue;						// time in HHmmSS format
const uint8_t calcFormatTimeInMillisecondsIdx =		calcFormatTimeHHmmSSIdx + 1;			// time in milliseconds
const uint8_t calcFormatEngineSpeedIdx =			calcFormatTimeInMillisecondsIdx + 1;	// engine speed
const uint8_t calcFormatPulseCountIdx =				calcFormatEngineSpeedIdx + 1;			// pulse count
#define nextAllowedValue calcFormatPulseCountIdx + 1
#ifdef useFuelCost
const uint8_t calcFormatFuelCostIdx =				nextAllowedValue;						// fuel cost
const uint8_t calcFormatFuelRateCostIdx =			calcFormatFuelCostIdx + 1;				// fuel rate cost
#define nextAllowedValue calcFormatFuelRateCostIdx + 1
#endif // useFuelCost
#if defined(useAnalogRead)
const uint8_t calcFormatAnalogReadIdx =				nextAllowedValue;						// voltage
#define nextAllowedValue calcFormatAnalogReadIdx + 1
#endif // useAnalogRead
#ifdef useDragRaceFunction
const uint8_t calcFormatTimeInTenthsSecIdx =		nextAllowedValue;						// time in tenths of seconds
#define nextAllowedValue calcFormatTimeInTenthsSecIdx + 1
#endif // useDragRaceFunction
const uint8_t calcFormatFuelQuantityIdx =			nextAllowedValue;						// fuel quantity (SAE/SI)
const uint8_t calcFormatFuelRateIdx =				calcFormatFuelQuantityIdx + 2;			// fuel rate (SAE/SI)
const uint8_t calcFormatDistanceIdx =				calcFormatFuelRateIdx + 2;				// distance travelled (SAE/SI)
const uint8_t calcFormatSpeedIdx =					calcFormatDistanceIdx + 2;				// speed (SAE/SI)
#define nextAllowedValue calcFormatSpeedIdx + 2
#ifdef useFuelCost
const uint8_t calcFormatFuelCostPerDistanceIdx =	nextAllowedValue;						// fuel cost per unit distance (SI/SAE)
const uint8_t calcFormatDistancePerFuelCostIdx =	calcFormatFuelCostPerDistanceIdx + 2;	// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue calcFormatDistancePerFuelCostIdx + 2
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
const uint8_t calcFormatPressureIdx =				nextAllowedValue;						// absolute pressure (SI/SAE)
#define nextAllowedValue calcFormatPressureIdx + 2
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
const uint8_t calcFormatEstimatedPowerIdx =			nextAllowedValue;						// estimated engine power (SI/SAE)
#define nextAllowedValue calcFormatEstimatedPowerIdx + 2
#endif // useDragRaceFunction
const uint8_t calcFormatFuelEconomyIdx =			nextAllowedValue;						// fuel economy (SAE/SI)
#define nextAllowedValue calcFormatFuelEconomyIdx + 4

const uint8_t calcFormatIdxCount =					nextAllowedValue;

#ifdef useDebugTerminal
static const char terminalFormats[] PROGMEM = {
	"hhmmss\0"
	"ms\0"
	"rev/min\0"
	"pulses\0"
#ifdef useFuelCost
	"cost\0"
	"cost/hour\0"
#endif // useFuelCost
#if defined(useAnalogRead)
	"V(dc)\0"
#endif // useAnalogRead
#ifdef useDragRaceFunction
	"tenths\0"
#endif // useDragRaceFunction
	"gallon\0"
	"liter\0"
	"gallon/hour\0"
	"liter/hour\0"
	"mile\0"
	"km\0"
	"MPH\0"
	"kPH\0"
#ifdef useFuelCost
	"cost/mile\0"
	"cost/km\0"
	"mile/cost\0"
	"km/cost\0"
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	"psia\0"
	"kPa\0"
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	"WHP\0"
	"kW\0"
#endif // useDragRaceFunction
	"MPG\0"
	"L/100km\0"
	"gal/100miles\0"
	"LPK\0"
};

#endif // useDebugTerminal
const uint8_t calcFormatIdx[(unsigned int)(dfMaxValDisplayCount)] PROGMEM = { // S64programList
	 calcFormatTimeHHmmSSIdx					// tEngineRunTime - engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tRangeTime - estimated total runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tMotionTime - time vehicle in motion (hhmmss)

	,calcFormatTimeInMillisecondsIdx			// tInjectorOpenTime - fuel used (milliseconds)
	,calcFormatTimeInMillisecondsIdx			// tInjectorTotalTime - engine run time (milliseconds)
	,calcFormatTimeInMillisecondsIdx			// tVSStotalTime - time vehicle in motion (milliseconds)
	,calcFormatEngineSpeedIdx					// tEngineSpeed - engine speed (1/m)
	,calcFormatPulseCountIdx					// tInjectorPulseCount - fuel injector pulse count
	,calcFormatPulseCountIdx					// tVSSpulseCount - VSS pulse count
#ifdef useFuelCost
	,calcFormatFuelCostIdx						// tFuelCostUsed - cost of fuel quantity used
	,calcFormatFuelCostIdx						// tFuelCostTank - full tank fuel cost in currency units
	,calcFormatFuelCostIdx						// tFuelCostReserve - reserve fuel quantity fuel cost in currency units
	,calcFormatFuelCostIdx						// tFuelCostBingo - bingo fuel quantity cost in currency units
	,calcFormatFuelCostIdx						// tFuelCostRemaining - value of estimated remaining total fuel quantity in currency units
	,calcFormatFuelCostIdx						// tFuelCostReserveRemaining - value of estimated remaining reserve fuel quantity in currency units
	,calcFormatFuelCostIdx						// tFuelCostBingoRemaining - value of estimated remaining bingo fuel quantity in currency units
	,calcFormatFuelRateCostIdx					// tFuelRateCost - fuel rate cost in currency units
#endif // useFuelCost
#ifdef useDebugAnalog
	,calcFormatAnalogReadIdx					// tAnalogChannel - DC voltage
#endif // useDebugAnalog
#ifdef useCarVoltageOutput
	,calcFormatAnalogReadIdx					// tAlternatorChannel - DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	,calcFormatTimeInTenthsSecIdx				// tAccelTestTime - acceleration test time (s.s)
#endif // useDragRaceFunction
	,calcFormatFuelQuantityIdx					// tFuelUsed - fuel quantity used (SI/SAE)
	,calcFormatFuelRateIdx						// tFuelRate - fuel consumption rate (SI/SAE)
	,calcFormatDistanceIdx						// tDistance - vehicle distance traveled (SI/SAE)
	,calcFormatSpeedIdx							// tSpeed - vehicle speed (SI/SAE)
	,calcFormatFuelQuantityIdx					// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	,calcFormatFuelQuantityIdx					// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	,calcFormatFuelQuantityIdx					// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
	,calcFormatDistanceIdx						// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	,calcFormatDistanceIdx						// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	,calcFormatDistanceIdx						// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	,calcFormatDistanceIdx						// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	,calcFormatDistanceIdx						// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	,calcFormatDistanceIdx						// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
#ifdef useFuelCost
	,calcFormatFuelCostPerDistanceIdx			// tFuelCostPerDistance - fuel cost per unit distance (SI/SAE)
	,calcFormatDistancePerFuelCostIdx			// tDistancePerFuelCost - distance per unit fuel cost (SI/SAE)
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,calcFormatPressureIdx						// tPressureChannel - absolute pressure (SI/SAE)
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,calcFormatEstimatedPowerIdx				// tEstimatedEnginePower - estimated engine power (SI/SAE)
	,calcFormatSpeedIdx							// tDragSpeed - acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#endif // useDragRaceFunction
	,calcFormatFuelEconomyIdx					// tFuelEcon - fuel economy (SI/SAE)
};

const uint8_t calcFormatDecimalPlaces[(unsigned int)(calcFormatIdxCount)] PROGMEM = { // S64programList
	 0	// time in HHmmSS format
	,3	// time in milliseconds
	,0	// engine speed
	,0	// pulse count
#ifdef useFuelCost
	,2	// fuel cost
	,2	// fuel rate cost
#endif // useFuelCost
#if defined(useAnalogRead)
	,3	// voltage
#endif // useAnalogRead
#ifdef useDragRaceFunction
	,1	// time in tenths of seconds
#endif // useDragRaceFunction
	,2	// SAE fuel quantity
	,2	// SI fuel quantity
	,2	// SAE fuel rate
	,2	// SI fuel rate
	,3	// SAE distance travelled
	,3	// SI distance travelled
	,1	// SAE speed
	,1	// SI speed
#ifdef useFuelCost
	,2	// SAE fuel cost per unit distance
	,2	// SI fuel cost per unit distance
	,1	// SAE distance per unit fuel cost
	,1	// SI distance per unit fuel cost
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,2	// SAE pressure
	,2	// SI pressure
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,1	// SAE horsepower
	,1	// SI horsepower
#endif // useDragRaceFunction
	,2	// SAE fuel economy
	,2	// SI fuel economy
	,2	// alternate SAE fuel economy
	,2	// alternate SI fuel economy
};

const uint8_t calcFormatLabelText[(unsigned int)(calcFormatIdxCount)] PROGMEM = { // S64programList
	 's'	// time in HHmmSS format
	,'m'	// time in milliseconds
	,'t'	// engine speed
	,'p'	// pulse count
#ifdef useFuelCost
	,'$'	// fuel cost
	,'#'	// fuel rate cost
#endif // useFuelCost
#if defined(useAnalogRead)
	,'V'	// voltage
#endif // useAnalogRead
#ifdef useDragRaceFunction
	,'s'	// time in tenths of seconds
#endif // useDragRaceFunction
	,'G'	// SAE fuel used
	,'L'	// SI fuel used
	,'g'	// SAE fuel rate
	,'l'	// SI rate
	,'m'	// SAE distance travelled
	,'k'	// SI distance travelled
	,'S'	// SAE speed
	,'S'	// SI speed
#ifdef useFuelCost
	,'C'	// SAE fuel cost per distance
	,'C'	// SI fuel cost per distance
	,'D'	// SAE distance per fuel cost
	,'D'	// SI distance per fuel cost
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,'P'	// SAE pressure
	,'P'	// SI pressure
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,'H'	// SAE horsepower
	,'W'	// SI horsepower
#endif // useDragRaceFunction
	,'E'	// SAE fuel economy
	,'E'	// SI fuel economy
	,'E'	// alternate SAE fuel economy
	,'E'	// alternate SI fuel economy
};

static const uint8_t isValidTripIdx =	0b10100000;
static const uint8_t isValidCalcIdx =	0b01100000;
static const uint8_t isValidFlag =		0b00100000;

static const uint8_t isValidCalcObj =	0b11000000;

static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	calcFuncObj thisCalcFuncObj;

	uint8_t calcFmtIdx;

	thisCalcFuncObj.isValid = 0;

	if (tripIdx < tripSlotTotalCount)
	{

		thisCalcFuncObj.isValid ^= (isValidTripIdx);
		thisCalcFuncObj.tripChar = pgm_read_byte(&tripVarChars[(uint16_t)(tripIdx)]);

	}

	if (calcIdx < dfMaxValDisplayCount)
	{

		thisCalcFuncObj.isValid ^= (isValidCalcIdx);

		if (activityFlags & afSwapFEwithFCR) // do fuel consumption rate swap with fuel economy here
		{

			if ((tripIdx == instantIdx) && (calcIdx == tFuelEcon)) calcIdx = tFuelRate;

		}

		thisCalcFuncObj.tripIdx = tripIdx;
		thisCalcFuncObj.calcIdx = calcIdx;

		calcFmtIdx = pgm_read_byte(&calcFormatIdx[(unsigned int)(calcIdx)]);
		if ((calcIdx >= dfMaxValNonConversion) && (metricFlag & metricMode)) calcFmtIdx++; // shift index up one if this is an SI/SAE format
		if ((calcIdx >= dfMaxValSingleFormat) && (metricFlag & alternateFEmode)) calcFmtIdx += 2; // shift index up one if this has two separate formats

		thisCalcFuncObj.calcFmtIdx = calcFmtIdx;
		thisCalcFuncObj.decimalPlaces = pgm_read_byte(&calcFormatDecimalPlaces[(unsigned int)(calcFmtIdx)]);
		thisCalcFuncObj.calcChar = pgm_read_byte(&calcFormatLabelText[(unsigned int)(calcFmtIdx)]);

	}

	if (thisCalcFuncObj.isValid & isValidCalcObj) thisCalcFuncObj.isValid ^= (isValidFlag);

	thisCalcFuncObj.strBuffer = strBuff;

	if (thisCalcFuncObj.isValid)
	{


		if (thisCalcFuncObj.calcFmtIdx == calcFormatTimeHHmmSSIdx) ull2str(thisCalcFuncObj.calcIdx, strBuff, thisCalcFuncObj.tripIdx);
		else
		{

			SWEET64::doCalculate(thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);
			formatDecimal(strBuff, windowLength, thisCalcFuncObj.decimalPlaces, decimalFlag);

		}

	}
	else
	{

		strcpy_P(strBuff, overFlowStr);
		thisCalcFuncObj.strBuffer[(unsigned int)(windowLength)] = 0;

	}

	return thisCalcFuncObj;

}

#ifdef useSpiffyTripLabels
const uint8_t calcFormatLabelCGRAM[(unsigned int)(calcFormatIdxCount)][16] PROGMEM = { // S64programList
	// time in HHmmSS format
	 {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}

	// time in milliseconds
	,{0b00010010, 0b00010010, 0b00010010, 0b00011110, 0b00010000, 0b00110000, 0b01000000, 0b01100000
	, 0b00001110, 0b00010000, 0b00001100, 0b00000010, 0b00011100, 0b00000000, 0b00000000, 0b00000000}

	// engine speed
	,{0b00001100, 0b00010000, 0b00010001, 0b00010010, 0b00000100, 0b00001000, 0b00000000, 0b00000000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011011, 0b00010101, 0b00010101}

	// pulse count
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001000, 0b00011100, 0b00001000, 0b00001000}
#ifdef useFuelCost

	// fuel cost
	,{0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100
	, 0b00000010, 0b00000101, 0b00000100, 0b00001110, 0b00000100, 0b00000100, 0b00000100, 0b00000100}

	// fuel rate cost
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}
#endif // useFuelCost
#if defined(useAnalogRead)

	// voltage
	,{0b00010001, 0b00010001, 0b00010001, 0b00001010, 0b00000100, 0b00000000, 0b00000000, 0b00000000
	, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011010, 0b00011010, 0b00010001}
#endif // useAnalogRead
#ifdef useDragRaceFunction

	// time in tenths of seconds
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}
#endif // useDragRaceFunction

	// gallons
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000101, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000001, 0b00000001, 0b00011001, 0b00011101}

	// liters
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000100, 0b00000100, 0b00000111
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}

	// gallons per hour
	,{0b00001100, 0b00010000, 0b00010100, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// liters per hour
	,{0b00010000, 0b00010000, 0b00010000, 0b00011100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// miles
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00000000, 0b00010000, 0b00010000}

	// kilometers
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000101, 0b00000110, 0b00000101
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00001010, 0b00010101, 0b00010101}

	// miles per hour
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00011100, 0b00010100, 0b00010100}

	// kilometers per hour
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}
#ifdef useFuelCost

	// fuel cost per mile
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101}

	// fuel cost per kilometer
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010100, 0b00011000, 0b00010100}

	// mile per unit fuel cost
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100}

	// kilometer per unit fuel cost
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100}
#endif // useFuelCost
#ifdef useChryslerMAPCorrection

	// psi
	,{0b00001110, 0b00001001, 0b00001001, 0b00001110, 0b00001000, 0b00001000, 0b00001000, 0b00000000
	, 0b00001001, 0b00010101, 0b00010001, 0b00001001, 0b00000101, 0b00010101, 0b00001001, 0b00000000}

	// kPa
	,{0b00001000, 0b00001000, 0b00001001, 0b00001010, 0b00001100, 0b00001010, 0b00001001, 0b00000000
	, 0b00011000, 0b00010100, 0b00010100, 0b00011000, 0b00010010, 0b00010101, 0b00010011, 0b00000000}
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction

	// HP
	,{0b00001001, 0b00001001, 0b00001001, 0b00001111, 0b00001001, 0b00001001, 0b00001001, 0b00000000
	, 0b00100000, 0b01000000, 0b01100000, 0b00011000, 0b00010100, 0b00011000, 0b00010000, 0b00010000}

	// kW
	,{0b00100000, 0b01000000, 0b01110000, 0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000000
	, 0b00010001, 0b00010001, 0b00010101, 0b00010101, 0b00010101, 0b00001010, 0b00001010, 0b00000000}
#endif // useDragRaceFunction

	// miles per gallon
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010100, 0b00001100}

	// liters per 100 km
	,{0b00010000, 0b00010000, 0b00011001, 0b00000010, 0b00000100, 0b00001001, 0b00000001, 0b00000001
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111}

	// gallons per 100 miles
	,{0b00001100, 0b00010000, 0b00010100, 0b00001101, 0b00000010, 0b00000101, 0b00001001, 0b00000001
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111}

	// km per liter
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00011100}
};

#endif // useSpiffyTripLabels
const uint8_t dfBitShift = 5;
const uint8_t dfFunctionMask =		0b00011111;
const uint8_t dfTripMask =			0b11100000;

// local trip indexes for main screen trip function display variables
#define nextAllowedValue 0
const uint8_t msInstantIdx =			nextAllowedValue;
const uint8_t msCurrentIdx =			msInstantIdx + 1;
const uint8_t msTankIdx =				msCurrentIdx + 1;
#define nextAllowedValue msTankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t msEOCidleCurrentIdx =		nextAllowedValue;
const uint8_t msEOCidleTankIdx =		msEOCidleCurrentIdx + 1;
const uint8_t msEOCidleInstantIdx =		msEOCidleTankIdx + 1;
#define nextAllowedValue msEOCidleInstantIdx + 1
#endif // trackIdleEOCdata

#ifdef useScreenEditor
const uint8_t dfMaxTripCount = nextAllowedValue;

#endif // useScreenEditor
const uint8_t lblInstantIdx =			(msInstantIdx << dfBitShift)	| instantIdx;
const uint8_t lblCurrentIdx =			(msCurrentIdx << dfBitShift)	| currentIdx;
const uint8_t lblTankIdx =				(msTankIdx << dfBitShift)		| tankIdx;
#ifdef trackIdleEOCdata
const uint8_t lblEOCidleCurrentIdx =	(msEOCidleCurrentIdx << dfBitShift)	| eocIdleCurrentIdx;
const uint8_t lblEOCidleTankIdx =		(msEOCidleTankIdx << dfBitShift)	| eocIdleTankIdx;
const uint8_t lblEOCidleInstantIdx =	(msEOCidleInstantIdx << dfBitShift)	| eocIdleInstantIdx;
#endif // trackIdleEOCdata

// trip index translation list
const uint8_t msTripList[] PROGMEM = {
	instantIdx
	,currentIdx
	,tankIdx
#ifdef trackIdleEOCdata
	,eocIdleCurrentIdx
	,eocIdleTankIdx
	,eocIdleInstantIdx
#endif // trackIdleEOCdata
};

#ifdef useSpiffyTripLabels
// display variable trip labels
const uint8_t msTripBitPattern[][4] PROGMEM = {
	 {0b00000000, 0b00000111, 0b00000010, 0b00000111} // I
 	,{0b00000000, 0b00000011, 0b00000100, 0b00000011} // C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000010} // T
#ifdef trackIdleEOCdata
	,{0b00000000, 0b00000011, 0b00000100, 0b00000110} // italic C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000100} // italic T
	,{0b00000000, 0b00000011, 0b00000010, 0b00000110} // italic I
#endif // trackIdleEOCdata
};

#endif // useSpiffyTripLabels
const char msTripNameString[] PROGMEM = {
	"INST\0"
	"CURR\0"
	"TANK\0"
#ifdef trackIdleEOCdata
	"cC/I\0"
	"tC/I\0"
	"iC/I\0"
#endif // trackIdleEOCdata
};

#ifdef useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
#else // useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos)
#endif // useSpiffyTripLabels
{

	uint32_t screenFormatValue;
	union union_32 * sfvp = (union union_32 *) &screenFormatValue;
	uint16_t sfIdx;
	uint8_t tripIdx;
	uint8_t calcIdx;

	sfIdx = cursorPos * 4;

	for (uint8_t x = 0; x < 4; x++)
	{

		tripIdx = pgm_read_byte(&localScreenFormatList[sfIdx + x][0]);
		calcIdx = pgm_read_byte(&localScreenFormatList[sfIdx + x][1]);

#ifdef useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, localTripBitmask);
#else // useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx);
#endif // useSpiffyTripLabels

	}

#ifdef useSpiffyTripLabels
	LCD::flushCGRAM();

#endif // useSpiffyTripLabels
}

#ifdef useSpiffyTripLabels
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
#else // useSpiffyTripLabels
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx)
#endif // useSpiffyTripLabels
{

	calcFuncObj thisCalcFuncObj;

	uint8_t x;
	uint8_t y;
#ifdef useSpiffyTripLabels
	uint8_t tripBitmask;
	uint8_t calcBitmask;
	uint8_t tripPart;
	uint8_t calcPart;
	uint8_t translateFlag;
	uint8_t localTripIdx;

	translateFlag = calcIdx & 0x80; // save trip index translation bit

	if (calcIdx & 0x80)
	{

		localTripIdx = tripIdx;
		calcIdx &= 0x7F;

	}
	else
	{

		localTripIdx = (tripIdx & dfTripMask) >> dfBitShift; // extract local trip index
		tripIdx = (tripIdx & dfFunctionMask);  // extract calculation trip index

	}
#endif // useSpiffyTripLabels

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, 6, 0);

	readingIdx &= 3;
	x = (readingIdx & 1) << 3;
	y = (readingIdx & 2) >> 1;

	text::gotoXY(devLCD, x, y);
	text::stringOut(devLCD, thisCalcFuncObj.strBuffer);

#ifdef useSpiffyTripLabels
	readingIdx <<= 1;

	text::charOut(devLCD, 0xF0 + readingIdx);
	text::charOut(devLCD, 0xF1 + readingIdx);

	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function label component should blink or not

	readingIdx <<= 3;

	for (uint8_t x = 0; x < 16; x++)
	{

		calcPart = pgm_read_byte(&calcFormatLabelCGRAM[(uint16_t)(thisCalcFuncObj.calcFmtIdx)][(uint16_t)(x)]); // read a byte of function label bit pattern

		if (translateFlag) tripPart = 0;
		else
		{

			tripPart = (calcPart >> 5) & 0x03; // fetch partial address of trip label component
			tripPart = pgm_read_byte(&localTripBitmask[(uint16_t)(localTripIdx)][(uint16_t)(tripPart)]); // read a byte of trip label bit pattern
			tripPart &= tripBitmask; // provide for blinking trip label component

		}

		calcPart &= calcBitmask; // provide for blinking function label component

		LCD::writeCGRAMbyte(readingIdx + x, calcPart | tripPart); // combine trip label and function label components

	}

#else // useSpiffyTripLabels
	text::charOut(devLCD, thisCalcFuncObj.tripChar);
	text::charOut(devLCD, thisCalcFuncObj.calcChar);

#endif // useSpiffyTripLabels
}

#ifdef useClockDisplay
 /* Big Clock Display support section */

static const uint8_t prgmChangeSoftwareClock[] PROGMEM = {
	instrLdRegVolatile, 0x02, vClockCycleIdx,			// read software clock
	instrDiv2byConst, idxTicksPerSecond,				// convert datetime value from cycles to seconds
	instrDiv2byConst, idxSecondsPerDay,					// divide by number of seconds in a day, to remove the existing time portion from the datetime value
	instrMul2byByte, 24,								// multiply datetime value by 24 (hours per day)
	instrLdRegByteFromY, 0x31, 0,						// add user-defined hours value to datetime value
	instrAddYtoX, 0x12,
	instrMul2byByte, 60,								// multply datetime value by 60 (minutes per hour)
	instrLdRegByteFromY, 0x31, 2,						// add user-defined minutes value to datetime value
	instrAddYtoX, 0x12,
	instrMul2byByte, 60,								// multiply datetime value by 60 (seconds per minute)
	instrLdRegByteFromY, 0x31, 4,						// add user-defined seconds value to datetime value
	instrAddYtoX, 0x12,
	instrMul2byConst, idxTicksPerSecond,				// convert datetime value from seconds to cycles
	instrStRegVolatile, 0x02, vClockCycleIdx,			// write software clock
	instrDone
};

static void clockSet::entry(void)
{

#ifdef useSoftwareClock
	ull2str(tReadTicksToSeconds, pBuff, vClockCycleIdx);
#endif // useSoftwareClock

	cursor::moveAbsolute(clockSetScreenIdx, 0);

}

static void clockSet::changeDigitUp(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])]++;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] > '9') pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] = '0';

	if (pBuff[2] > '5') pBuff[2] = '0'; // this will only happen if clockSetScreenIdx == 2
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '0'; // this will only happen if clockSetScreenIdx == 0 or 1
	if (pBuff[0] > '2') pBuff[0] = '0'; // this will only happen if clockSetScreenIdx == 0

}

static void clockSet::changeDigitDown(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])]--;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] < '0') pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] = '9';

	if (pBuff[0] > '2') pBuff[0] = '2'; // this will only happen if clockSetScreenIdx == 0
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '3'; // this will only happen if clockSetScreenIdx == 0 or 1
	if (pBuff[2] > '5') pBuff[2] = '5'; // this will only happen if clockSetScreenIdx == 2

}

static void clockSet::set(void)
{

#ifdef useSoftwareClock
	uint8_t b;

	pBuff[4] = '0'; // set seconds to zero
	pBuff[5] = '0';

	for (uint8_t x = 4; x < 6; x -= 2) // convert time string in pBuff into time value usable by prgmChangeSoftwareClock
	{

		b = pBuff[(unsigned int)(x)] - '0';
		b *= 10;
		b += pBuff[(unsigned int)(x + 1)] - '0';
		((union union_64 *)(&s64reg[s64reg3]))->u8[(unsigned int)(x)] = b;

	}

	SWEET64::runPrgm(prgmChangeSoftwareClock, 0); // convert time value into timer0 clock cycles

	cursor::screenLevelEntry(PSTR("Time Set"), clockShowScreenIdx);

#endif // useSoftwareClock
}

static void clockSet::cancel(void)
{

	cursor::screenLevelEntry(PSTR("Time NOT Set"), clockShowScreenIdx);

}

#endif // useClockDisplay
#ifdef useBigDigitDisplay
/* Big Digit Output support section */

#ifdef useBigFE
static const char bigFElabels[] PROGMEM = {
	"MPG\r"
	"L100\r"
	"G100\r"
	"KPL\r"
};

#endif // useBigFE
#ifdef useSpiffyBigChars
static const char bigDigitChars1[] PROGMEM = {
	0xF6, 0xF0, 0xF7, 0,
	0xF0, 0xF4, 0x20, 0,
	0xF2, 0xF2, 0xF7, 0,
	0xF0, 0xF2, 0xF7, 0,
	0xF4, 0xF1, 0xF4, 0,
	0xF4, 0xF2, 0xF2, 0,
	0xF6, 0xF2, 0xF2, 0,
	0xF0, 0xF0, 0xF5, 0,
	0xF6, 0xF2, 0xF7, 0,
	0xF6, 0xF2, 0xF7, 0,
	0x20, 0x20, 0x20, 0,
	0xF1, 0xF1, 0xF1, 0
};

static const char bigDigitChars2[] PROGMEM = {
	0xF3, 0xF1, 0xF5, 0,
	0x20, 0xF4, 0x20, 0,
	0xF4, 0xF1, 0xF1, 0,
	0xF1, 0xF1, 0xF5, 0,
	0x20, 0x20, 0xF4, 0,
	0xF1, 0xF1, 0xF5, 0,
	0xF3, 0xF1, 0xF5, 0,
	0x20, 0xF6, 0x20, 0,
	0xF3, 0xF1, 0xF5, 0,
	0xF1, 0xF1, 0xF5, 0,
	0x20, 0x20, 0x20, 0,
	0x20, 0x20, 0x20, 0
};

static const char bigDigitFont[] PROGMEM = {
	8, // number of characters in font

	0b00011111, // char 0xF0
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,

	0b00000000, // char 0xF1
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0xF2
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0xF3
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00001111,
	0b00000111,

	0b00011111, // char 0xF4
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,

	0b00011111, // char 0xF5
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011110,
	0b00011100,

	0b00000111, // char 0xF6
	0b00001111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,

	0b00011100, // char 0xF7
	0b00011110,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111
};

#else // useSpiffyBigChars
static const char bigDigitChars1[] PROGMEM = {
	0xF3, 0xF0, 0xF3, 0,
	0xF0, 0xF3, 0x20, 0,
	0xF2, 0xF2, 0xF3, 0,
	0xF0, 0xF2, 0xF3, 0,
	0xF3, 0xF1, 0xF3, 0,
	0xF3, 0xF2, 0xF2, 0,
	0xF3, 0xF2, 0xF2, 0,
	0xF0, 0xF0, 0xF3, 0,
	0xF3, 0xF2, 0xF3, 0,
	0xF3, 0xF2, 0xF3, 0,
	0x20, 0x20, 0x20, 0,
	0xF1, 0xF1, 0xF1, 0
};

static const char bigDigitChars2[] PROGMEM = {
	0xF3, 0xF1, 0xF3, 0,
	0xF1, 0xF3, 0xF1, 0,
	0xF3, 0xF1, 0xF1, 0,
	0xF1, 0xF1, 0xF3, 0,
	0x20, 0x20, 0xF3, 0,
	0xF1, 0xF1, 0xF3, 0,
	0xF3, 0xF1, 0xF3, 0,
	0x20, 0xF3, 0x20, 0,
	0xF3, 0xF1, 0xF3, 0,
	0xF1, 0xF1, 0xF3, 0,
	0x20, 0x20, 0x20, 0,
	0x20, 0x20, 0x20, 0
};

static const char bigDigitFont[] PROGMEM = {
	4, // number of characters in font

	0b00011111, // char 0xF0
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,

	0b00000000, // char 0xF1
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0xF2
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0xF3
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

#endif // useSpiffyBigChars
static uint8_t bigDigit::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;
	uint8_t thisMenuLevel = menuLevel;
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
	uint8_t tripIdx = pgm_read_byte(&msTripList[(unsigned int)(cursorPos)]);
	char * str;
#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
#ifdef useBigFE
	uint8_t i;
#endif // useBigFE

	switch (thisMenuLevel)
	{

#ifdef useBigDTE
		case bigDTEscreenIdx:
			str = PSTR(" DistToEmpty\r");
			break;

#endif // useBigDTE
#ifdef useBigFE
		case bigFEscreenIdx:
			str = PSTR(" Fuel Econ\r");
			break;

#endif // useBigFE
#ifdef useBigTTE
		case bigTTEscreenIdx:
			str = PSTR(" TimeToEmpty\r");
			break;

#endif // useBigTTE
		default:
#ifdef useClockDisplay
			if (cursorPos == 255) thisMenuLevel = clockShowScreenIdx;
#endif // useClockDisplay
			break;

	}

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			switch (thisMenuLevel)
			{

#ifdef useClockDisplay
				case clockShowScreenIdx:
				case clockSetScreenIdx:
#endif // useClockDisplay
#ifdef useBigDTE
				case bigDTEscreenIdx:
#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
#endif // useBigTTE
					bigDigit::loadCGRAMnumberFont();
					break;

				default:
					break;

			}

		case menuCursorUpdateIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigDTE
				case bigDTEscreenIdx:
#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
#endif // useBigTTE
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
					displayStatus(str, cursorPos);
					break;

#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
#ifdef useClockDisplay
				case clockShowScreenIdx:
					printStatusMessage(PSTR("Clock\r"));
					break;

				case clockSetScreenIdx:
					break;

#endif // useClockDisplay
				default:
					break;

			}

		case menuOutputDisplayIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigDTE
				case bigDTEscreenIdx:
					outputNumber(tripIdx, tDistanceToEmpty, 4);
					break;

#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
					i = outputNumber(tripIdx, tFuelEcon, 3) - calcFormatFuelEconomyIdx;

					text::stringOut(devLCD, msTripNameString, cursorPos);
					text::gotoXY(devLCD, 12, 1);
					text::stringOut(devLCD, bigFElabels, i);
					break;

#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
					outputTime(ull2str(tTimeToEmpty, mBuff1, tripIdx), (mainLoopHeartBeat & 0b01010101), 4);
					break;

#endif // useBigTTE
#ifdef useClockDisplay
				case clockShowScreenIdx:
					outputTime(ull2str(tReadTicksToSeconds, mBuff1, vClockCycleIdx), (mainLoopHeartBeat & 0b01010101), 4);
					break;

				case clockSetScreenIdx:
					outputTime(pBuff, (timer0Status & t0sShowCursor), cursorPos);
					break;

#endif // useClockDisplay
				default:
					break;

			}
			break;

		default:
			break;

	}

	return retVal;

}

static void bigDigit::loadCGRAMnumberFont(void)
{

	uint8_t numChars;
	const char * fontPtr;

	fontPtr = bigDigitFont;
	numChars = pgm_read_byte(fontPtr++); // get the number of characters in the font

	for (uint8_t chr = 0; chr < numChars * 8; chr++) LCD::writeCGRAMbyte(chr, pgm_read_byte(fontPtr++)); // copy the CGRAM character data into RAM

	LCD::flushCGRAM();

}

#ifdef useBigTimeDisplay
static void bigDigit::outputTime(char * val, uint8_t blinkFlag, uint8_t blinkPos)
{

	val[4] = val[0];
	val[5] = val[1];
	val[6] = ':';
	val[7] = val[2];
	val[8] = val[3];
	val[9] = 0;

	if ((blinkFlag) == 0) // if it's time to blink something
	{

		if (blinkPos== 4) val[6] = ';'; // if hh:mm separator is selected, blink it
		else if (blinkPos < 2) val[(unsigned int)(blinkPos + 4)] = ' '; // if digit 0 or 1 is selected, blink it
		else if (blinkPos < 4) val[(unsigned int)(blinkPos + 5)] = ' '; // otherwise, if digit 2 or 3 is selected, blink it

	}

	outputNumberString(&val[4]);

}

#endif // useBigTimeDisplay
#ifdef useBigNumberDisplay
static uint8_t bigDigit::outputNumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength)
{

	calcFuncObj thisCalcFuncObj;

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, windowLength, dfAdjustWindow); // perform the required decimal formatting
	outputNumberString(thisCalcFuncObj.strBuffer); // output the number

	return thisCalcFuncObj.calcFmtIdx; // for big MPG label

}

#endif // useBigNumberDisplay
static void bigDigit::outputNumberString(char * str)
{

	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t x;

	x = 0;
	while (*str)
	{

		c = *str++;
		d = *str;
		e = ' ';

		if ((d == '.') || (d == ':') || (d == ';'))
		{

			if (d == ':') e = '.';

			if (d == ';') d = ' ';
			else d = '.';

			str++;

		}
		else d = ' ';

		c -= '0';

		if (c == 240) c = 10;
		else if (c > 9) c = 11;

		outputDigit(bigDigitChars2, x, 1, c, d);
		outputDigit(bigDigitChars1, x, 0, c, e);
		x += 4;

	}

}

static void bigDigit::outputDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar)
{

	text::gotoXY(devLCD, xPos, yPos);
	text::stringOut(devLCD, digitDefStr, strIdx);
	text::charOut(devLCD, endChar);

}

static void bigDigit::displayStatus(const char * str, uint8_t cursorPos)
{

	initStatusLine();
	text::stringOut(devLCD, msTripNameString, cursorPos);
	text::stringOut(devLCD, str); // briefly display screen name
	execStatusLine();

}

#endif // useBigDigitDisplay
#ifdef useBarFuelEconVsTime
/* fuel economy over time histograph support section */

static const char barFEvTfuncNames[] PROGMEM = {
	"FuelUsed / Time\r"
	"Distance / Time\r"
	"FE / Time\r"
	"DiffFE / Time\r"
};

static const uint8_t barFEvTdisplayFuncs[] PROGMEM = {
	 tFuelUsed
	,tDistance
	,tFuelEcon
	,tFuelEcon
};

static const uint8_t barFEvTgraphFuncs[] PROGMEM = {
	 tFEvTgetConsumedFuel
	,tFEvTgetDistance
	,tFEvTgetFuelEconomy
	,tFEvTgetFuelEconomy
};

static const uint8_t barFEvTdiffFuncs[] PROGMEM = {
	 0
	,0
	,0
	,1
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
/* fuel economy over speed histograph support section */

static const char barFEvSfuncNames[] PROGMEM = {
	"FE / Speed\r"
	"Fuel Used/Speed\r"
	"Distance / Speed\r"
};

static const uint8_t barFEvSdisplayFuncs[] PROGMEM = {
	 tFuelEcon
	,tFuelUsed
	,tDistance
};

static const uint8_t barFEvSgraphFuncs[] PROGMEM = {
	 tFEvSgetFuelEconomy
	,tFEvSgetConsumedFuel
	,tFEvSgetDistance
};

static void bgFEvsSsupport::reset(void)
{

	for (uint8_t x = 0; x < bgDataSize; x++) tripVar::reset(FEvsSpeedIdx + x);

	FEvSpdTripIdx = 255;

}

static const uint8_t prgmFEvsSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrTestReg, 0x02,									// test VSS cycle value
	instrBranchIfZero, 15,								// if zero, then speed is also zero
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS pulse count
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term
	instrMul2byConst, idxCycles0PerSecond,				// set up to convert VSS cycle value to time in seconds
	instrDiv2by1,										// divide to obtain vehicle speed

//cont:
	instrSubMainFromX, 0x02, mpFEvsSpeedMinThresholdIdx,	// compare vehicle speed to minimum threshold
	instrBranchIfLTorE, 4,								// if vehicle speed is above minimum threshold, skip ahead

//badRet:
	instrLdRegByte, 0x02, 0xFF,							// load a 255 into register 2
	instrDone,											// exit to caller

//cont2:
	instrDiv2byMain, mpFEvsSpeedQuantumIdx,				// find trip index offset
	instrLdRegByte, 0x01, bgDataSize - 1,				// is offset greater than the number of available trip slots
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if not, skip ahead
	instrLdReg, 0x12,									// load the last trip slot index

//cont3:
	instrAddByteToX, 0x02, FEvsSpeedIdx,				// obtain working fuel econ vs speed trip index value
	instrDone											// exit to caller
};

#endif // useBarFuelEconVsSpeed
#ifdef useBarGraph
/* Bar Graph support section */

static const uint8_t prgmGenerateHistographData[] PROGMEM = {
	instrLdJumpReg,										// save jump register value of input function
	instrLxdI, bgDataSize,
	instrLdRegByte, 0x03, 0,

//loop:
	instrAddIndex, 255,									// decrement index
	instrCallImplied,									// go call input function
	instrBranchIfE, 10,									// if function result is zero, go skip
	instrTestReg, 0x03,									// test currently saved high value
	instrBranchIfE, 4,									// if high value is zero, go save what the input function returned
	instrCmpXtoY, 0x23,									// otherwise, compare current value to high value
	instrBranchIfLTorE,	2,								// if not higher, skip

//cont0:
	instrLdReg, 0x23,									// save high value

//cont1:
	instrTestIndex,
	instrBranchIfNotE, 238,
	instrLdReg, 0x32,									// load high value
	instrDiv2byByte, 100,								// divide by normalization value
	instrLdReg, 0x23,									// save high value

//loop2:
	instrTestReg, 0x03,									// is high value zero
	instrBranchIfE, 42,									// if so, just store a zero - can't divide by zero
	instrCallImplied,									// go call input function
	instrBranchIfNotE, 12,								// if function result is not zero, go skip
	instrBranchIfOverflow, 5,
	instrLdRegByte, 0x02, 0,							// load zero value into main register
	instrSkip, 9,

//ovfl:
	instrLdRegByte, 0x02, 255,							// load overflow value into main register
	instrSkip, 4,

//cont2:
	instrLdReg, 0x31,									// recall high value
	instrDiv2by1,										// go normalize value
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont3:
	instrStRegBGdataIndexed, 0x02,						// save normalized value
	instrAddIndex, 1,									// bump index up
	instrCmpIndex, bgDataSize,							// processed through all of fuel econ vs time trip variable bank?
	instrBranchIfLT, 225,								// if not, loop back
	instrDone											// return to caller
};

static const char bgSpaces[] PROGMEM = {
	 0x80, ' ', ' ', 0xF0, 0xF2, 0xF4, 0xF6, ' ', ' ', 0
	,0x94, ' ', ' ', 0xF1, 0xF3, 0xF5, 0xF7, ' ', ' ', 0
};

static uint8_t barGraphSupport::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	const char * labelList;

	uint8_t graphCursorPos;
	uint8_t graphCalcIdx;
	uint8_t differentialFlag;

	uint8_t line0CalcIdx;
	uint8_t line1CalcIdx;
	uint8_t line0TripIdx;
	uint8_t line1TripIdx;

	uint8_t i;

	switch (menuLevel)
	{

#ifdef useBarFuelEconVsTime
		case barFEvTscreenIdx:
			labelList = barFEvTfuncNames;

			graphCursorPos = bgDataSize - 1;
			graphCalcIdx = pgm_read_byte(&barFEvTgraphFuncs[(uint16_t)(cursorPos)]);
			line1CalcIdx = pgm_read_byte(&barFEvTdisplayFuncs[(uint16_t)(cursorPos)]);
			differentialFlag = pgm_read_byte(&barFEvTdiffFuncs[(uint16_t)(cursorPos)]);

			line0CalcIdx = line1CalcIdx;
			line0TripIdx = currentIdx;

			line1TripIdx = calcFEvTperiodIdx;
			break;

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
		case barFEvSscreenIdx:
			labelList = barFEvSfuncNames;

			if (FEvSpdTripIdx < tripSlotCount) i = FEvSpdTripIdx - FEvsSpeedIdx + 1;
			else i = 0;

			graphCursorPos = i - 1;
			graphCalcIdx = pgm_read_byte(&barFEvSgraphFuncs[(uint16_t)(cursorPos)]);
			line0CalcIdx = pgm_read_byte(&barFEvSdisplayFuncs[(uint16_t)(cursorPos)]);
			differentialFlag = 0; // no differential graphs for fuel econ vs speed

			if (i)
			{

				line0TripIdx = FEvSpdTripIdx;

				if (mainLoopHeartBeat & 0b11110001)
				{

					line1CalcIdx = tSpeed;
					line1TripIdx = instantIdx;

				}
				else
				{

					line1TripIdx = tankIdx;
					line1CalcIdx = line0CalcIdx;

				}

			}
			else
			{

				line0TripIdx = tankIdx;
				line1CalcIdx = tSpeed;
				line1TripIdx = instantIdx;

			}

			break;

#endif // useBarFuelEconVsSpeed
	}

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(labelList, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			graphData(graphCursorPos, graphCalcIdx, differentialFlag);
			displayBarGraphLine(0, line0TripIdx, line0CalcIdx);
			displayBarGraphLine(1, line1TripIdx, line1CalcIdx);

			LCD::flushCGRAM();

			break;

		default:
			break;

	}

	return retVal;

}

static void barGraphSupport::displayBarGraphLine(uint8_t lineNumber, uint8_t tripIdx, uint8_t calcIdx)
{

	calcFuncObj thisCalcFuncObj;

	text::stringOut(devLCD, bgSpaces, lineNumber);

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, 6, 0);

	if (thisCalcFuncObj.isValid & isValidFlag)
	{

		text::charOut(devLCD, thisCalcFuncObj.tripChar);
		text::charOut(devLCD, thisCalcFuncObj.calcChar);

		text::stringOut(devLCD, thisCalcFuncObj.strBuffer);

	}
	else text::newLine(devLCD);

}

static void barGraphSupport::clearPlot(void)
{

	for (uint8_t x = 0; x < 64; x++) LCD::writeCGRAMbyte(x, 0); // clear the drawing area

}

static uint8_t barGraphSupport::rescaleData(uint8_t byt, uint8_t uBound)
{

	uint16_t worksp;
	uint8_t out;

	if (byt > uBound) out = 15; // this may occur with the cursor position data point
	else
	{

		worksp = 13 * byt / uBound + 1;
		out = worksp;

	}

	return out;

}

/*
static int16_t barGraphSupport::sqrrt(uint16_t num)
{

	int16_t x;
	int16_t c;
	int16_t d;

	x = num;
	c = 0;
	d = (1 << 14);

	while (d > num) d >>= 2;

	while (d)
	{

		if (x >= (c + d))
		{

			x -= (c + d);
			c = (c >> 1) + d;

		}
		else c >>= 1;

		d >>= 2;

	}

	return c;

}
*/

static void barGraphSupport::graphData(uint8_t cursorPos, uint8_t calcIdx, uint8_t diffFlag)
{

	int16_t rollSum;
	uint8_t byt;
	uint8_t mean;
	uint8_t uBound;
	uint8_t yStart;
	uint8_t zCount;
	uint8_t blinkFlag;
	uint8_t fl;

	SWEET64::runPrgm(prgmGenerateHistographData, calcIdx);

	rollSum = 0;
	yStart = 0;
	zCount = 0;
	fl = 1;

	for (uint8_t x = 0; x < bgDataSize; x++) // this is for calculating the mean of the dataset
	{

		byt = bargraphData[(uint16_t)(x)];

		switch (byt)
		{

			case 255: // ignore overflow values
				break;

			case 0: // treat zero values with special consideration
				if (fl) break; // if scan has not yet found a non-zero value, skip without updating data count
				else zCount++; // increment zero count
			default: // determine mean
				yStart++;
				fl = 0;
				zCount = 0; // reset zero count
				rollSum += byt;
				break;

		}

	}

	yStart -= zCount; // take off any trailing zeros from data count

	switch (yStart)
	{

		case 0: // no mean to calculate, use default lBound and uBound
			uBound = 2;
			break;

		default:
			rollSum /= yStart; // calculate mean
			mean = rollSum;
			if (mean) uBound = 2 * mean; // if there is a non-zero mean, use that for uBound calculation
			else uBound = 2; // otherwise, assume an arbitrary uBound
			break;

	}

	clearPlot();

	drawLine(1, 0, 0, 0, 15); // draw y-axis on left of graph

	if (diffFlag)
	{

		yStart = 7; // start in the middle of the graph
		draw(1, 1, yStart);

	}
	else // absolute graph selected
	{

		yStart = 0; // start at bottom of the graph
		draw(1, 1, 14);
		draw(1, 1, 1);

	}

	for (uint8_t x = 0; x < bgDataSize; x++)
	{

		byt = bargraphData[(uint16_t)(x)];
		blinkFlag = ((x == cursorPos) && (mainLoopHeartBeat & 136));

		switch (byt)
		{

			case 0: // handle zero values
				byt = 0;
				break;

			case 255: // handle overflow values
				byt = 15;
				break;

			default: // handle values between lBound and uBound
				byt = rescaleData(byt, uBound);
				break;

		}

		if (byt == yStart)
		{

			if (blinkFlag == 0) draw(1, x + 3, byt);

		}
		else
		{

			if (blinkFlag) draw(1, x + 3, byt);
			else drawLine(1, x + 3, yStart, x + 3, byt);

		}

	}

	byt = rescaleData(mean, uBound);
	draw(1, 1, byt);

}

static void barGraphSupport::drawLine(uint8_t color, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY)
{

	uint8_t f;
	int dx;
	int dy;
	int sx;
	int sy;
	int err;
	int e2;

	dx = abs(endX - startX); // delta X between start and end
	dy = -abs(endY - startY); // delta Y between start and end
	sx = (endX > startX) - (endX < startX);	// sign(dx)
	sy = (endY > startY) - (endY < startY); // sign(dy)
	err = dx + dy; // figure out initial error

	f = 1;

	while (f)
	{

		draw(color, startX, startY); // draw a point of the line

		if ((startX == endX) && (startY == endY)) f = 0; // if start and endpoints are the same, then the line is finished
		else
		{

			e2 = 2 * err;
			if (e2 >= dy) // if error >= delta Y
			{

				if (startX == endX) f = 0; // if x is at endpoint, then line is finished
				else
				{

					err += dy; // update error
					startX += sx; // move x toward endpoint

				}

			}

			if (e2 <= dx) // if error <= delta X
			{

				if (startY == endY) f = 0; // if y is at endpoint, then line is finished
				else
				{

					err += dx; // update error
					startY += sy; // move y toward endpoint

				}

			}

		}

	}

}

static void barGraphSupport::draw(uint8_t color, uint8_t xPos, uint8_t yPos)
{

	uint8_t cgrAddress;
	uint8_t x;
	uint8_t y;
	uint8_t i;
	uint8_t msk;
	uint8_t c;
	uint8_t byt;

	x = xPos / 5;
	y = 15 - yPos;

	cgrAddress = y / 8; // results in 0 (upper) or 1 (lower)
	cgrAddress += 2 * x; // 0, 2, 4, 6 - horizontal component
	cgrAddress <<= 3; // multiply result by 8
	cgrAddress += (y & 0x07); // get byte offset into destination character
	i = xPos % 5; // figure out bitmask
	msk = (16 >> i);

	c = ((color) ? 31 : 0); // translate color to either on or off

	byt = LCD::peekCGRAMbyte(cgrAddress); // get the target byte from CGRAM storage

	c ^= byt; // xor color with target byte
	c &= msk; // strip out all but relevant bit
	c ^= byt; // xor relevant bit with target byte

	LCD::writeCGRAMbyte(cgrAddress, c); // go save target byte with changed bit

}

#endif // useBarGraph
static const uint8_t prgmMultiplyBy100[] PROGMEM = {
	instrMul2byByte, 100,								// multiply result by 100
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static const uint8_t prgmMultiplyBy10[] PROGMEM = {
	instrMul2byByte, 10,								// multiply result by 10
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

unsigned long str2ull(char * strBuffer)
{

	uint8_t c;
	uint8_t n;

	uint8_t x;
	uint8_t f;
	uint8_t loopFlag;

	x = 0;
	n = 0;
	f = 1;
	loopFlag = 1;

	SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0); // initialize 64-bit number to zero

	while ((loopFlag) && (x < 17))
	{

		if ((c = strBuffer[(unsigned int)(x++)])) // if a non-NULL character is read in
		{

			n *= 10; // shift accumulator left one digit
			if (c != ' ') n += (uint8_t)(c) - 48; // if character is not a leading space, go add it to accumulator
			f= 1 - f; // flip-flop the SWEET64 addition flag

			if (f)
			{

				SWEET64::runPrgm(prgmMultiplyBy100, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 100 + n
				n = 0;

			}

		}
		else loopFlag = 0; // otherwise, terminate loop upon receipt of a NULL character

	}

	if (f == 0) SWEET64::runPrgm(prgmMultiplyBy10, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 10 + n

	return ((union union_64 *)(&s64reg[s64reg2]))->ul[0];

}

char findDigit(uint8_t value, char &zeroChar)
{

	if (value)
	{
		value += '0';
		zeroChar = '0';
	}
	else value = zeroChar;

	return value;

}

char * ull2str(uint8_t prgmIdx, char * strBuffer, uint8_t decimalPlaces)
{

	union union_64 * tmpPtr2 = (union union_64 *)(&s64reg[s64reg3]);

	uint8_t b;
	char c;
	uint8_t l;
	uint8_t y;
	char z;

	SWEET64::doCalculate(decimalPlaces, prgmIdx); // call SWEET64 routine to perform decimal point rounding to next nearest decimal place

	l = tmpPtr2->u8[6];	// load total length of binary-coded decimal bytes of converted number

	if (l == 255) strcpy_P(strBuffer, overFlowStr); // if length is 255, this number overflowed
	else
	{

		z = (char)(tmpPtr2->u8[7]);	// load leading zero character
		y = 0;

		for (uint8_t x = 0; x < l; x++) // go through all of the binary-coded decimal bytes of converted number
		{

			b = tmpPtr2->u8[(unsigned int)(x)];	// load a binary-coded decimal byte of number

			c = findDigit(b / 10, z);		// perform leading zero conversion on 10's place digit
			strBuffer[(unsigned int)(y++)] = c;	// store 10's place digit in string buffer

			c = findDigit(b % 10, z);		// perform leading zero conversion on 1's place digit
			strBuffer[(unsigned int)(y++)] = c;	// store 1's place digit in string buffer

		}

		if (c == ' ') strBuffer[(unsigned int)(y - 1)] = '0'; // ensure that at least one numeric digit exists
		strBuffer[(unsigned int)(y)] = 0; // mark end of string buffer with a NULL character

	}

	return strBuffer; // return pointer to string buffer containing the number

}

char * formatDecimal(char * strBuffer, uint8_t windowLength, uint8_t decimalPlaces, uint8_t decimalFlag) // format number for output
{

	uint8_t x;
	uint8_t y;
	uint8_t z; // pointer to end of integer digits in string
	uint8_t c; // temporary character storage
	uint8_t i; // integer digit count

	ull2str(tRoundOffNumber, strBuffer, decimalPlaces); // perform rounding of number to nearest decimal place, then format for ASCII output

	if (windowLength > 11) windowLength = 0; // if window length is larger than the contents of the output string buffer, zero out window length

	if (strBuffer[2] != '-') // if number did not overflow
	{

		x = 6 + decimalPlaces; // source string index
		y = 10; // destination string index
		i = 0;

		while (y < 12) // loop through entire string buffer, insert padding and a decimal point as required, and preserve ending NULL
		{

			if (x < 10)
			{

				c = strBuffer[(unsigned int)(x)]; // read in a digit of formatted number from string buffer, bump down source string index by one

				if ((x > 5) && (c == ' ')) c = '0'; // change 1's place and lower places leading spaces into leading zeros
				if ((x == 6) && (decimalPlaces)) strBuffer[(unsigned int)(y--)] = '.'; // if a decimal position was specified, and at decimal point, insert it
				if ((x < 7) && (c != ' ')) i++; // count the number of non-space integer characters

				x--;

			}
			else c = ' '; // otherwise, pad front of number with extra spaces

			strBuffer[(unsigned int)(y--)] = c; // store processed character back into string buffer, bump down destination string index by one

		}

		if (decimalPlaces) z = 10 - decimalPlaces; // point to either decimal point or to end of string
		else z = 11; // results in pointing to first character past the 1's digit in both cases

		if (windowLength)
		{

			if (i > windowLength) // if window length is smaller than the number of integer digits, overflow
			{

				x = 0; // signal 'do not perform string buffer move'
				strcpy_P(strBuffer, overFlowStr); // copy overflow string to string buffer

			}
			else
			{

				if ((decimalFlag & dfAdjustWindow) && (i < windowLength)) windowLength++; // if variable window length, and window length > the # of integer digits, expand window to include decimal point

				if ((i == windowLength) || ((i + 1) == windowLength)) x = z - windowLength;
				else
				{

					x = z - i;
					if ((x + windowLength) > 10) x = 11 - windowLength;

				}

			}

		}
		else // no window length specified, just eliminate leading spaces
		{

			x = z - i; // point to first integer character in string buffer
			strBuffer[11] = 0; // mark new end of string buffer

		}

		if (x) for (y = 0; y < (12 - x); y++) strBuffer[(unsigned int)(y)] = strBuffer[(unsigned int)(y + x)]; // shift number left to remove unneeded leading spaces

	}
	else
	{

		if (decimalFlag & dfOverflow9s) strcpy_P(strBuffer, overFlow9Str);

	}

	if (windowLength) strBuffer[(unsigned int)(windowLength)] = 0; // mark new end of string buffer

	return strBuffer;

}

#ifdef useSimulatedFIandVSS
const uint8_t debugScreenFormats[4][2] PROGMEM = {
	 {lblInstantIdx,	tInjectorTotalTime} 		// Debug
	,{lblInstantIdx,	tVSStotalTime}
	,{lblInstantIdx,	tInjectorPulseCount}
	,{lblInstantIdx,	tVSSpulseCount}
};

const char debugScreenFuncNames[] PROGMEM = {
	"FI ON   VSS ON\r"
	"FI OFF  VSS ON\r"
	"FI OFF  VSS OFF\r"
	"FI ON   VSS OFF\r"
};

const uint16_t debugVSSvalues[] PROGMEM = {
 65535
,2258
,1128
,752
,564
,451
,375
,322
,281
,250
,225
,204
,187
,173
,160
,150
,140
,132
,124
,118
,112
,107
,102
,97
,93
,89
,86
,83
,80
,77
,74
,72
,70
,67
,65
,64
,62
,60
,58
,57
,55
,54
,53
,52
,50
,49
,48
,47
,46
,45
,44
};

const uint16_t debugFIPvalues[] PROGMEM = {
 313
,326
,341
,358
,375
,395
,417
,442
,470
,501
,537
,578
,626
,683
,752
,836
,940
,1075
,1254
,1505
,1881
,2509
,3764
,7528
,18823
,65535
};

const uint16_t debugFIPWvalues[] PROGMEM = {
 22
,22
,23
,23
,23
,24
,24
,24
,25
,26
,26
,27
,28
,29
,30
,32
,34
,36
,39
,44
,51
,63
,86
,156
,365
,0
};

const uint8_t debugVSSlength = ( sizeof(debugVSSvalues) / sizeof(uint16_t) );
const uint8_t debugFIPlength = ( sizeof(debugFIPvalues) / sizeof(uint16_t) );

uint8_t debugVSSidx;
uint8_t debugFIPidx;

uint8_t debugVSSstate;
uint8_t debugFIPstate;

uint16_t debugFIPWgoodTickLength; // to force debug injector pulse width to a maximum good engine speed-dependent value
uint16_t debugFIPWreadTickLength;

static uint8_t debugReading::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(debugScreenFuncNames, cursorPos); // briefly display screen name
			retVal = (debugFlags & debugEnableFlags);
			switch (cursorPos)
			{

				case 0:
					peripheral::changeBitFlags(debugFlags, 0, (debugInjectorFlag | debugVSSflag));
					break;

				case 1:
					peripheral::changeBitFlags(debugFlags, debugInjectorFlag, debugVSSflag);
					break;

				case 2:
					peripheral::changeBitFlags(debugFlags, (debugInjectorFlag | debugVSSflag), 0);
					break;

				case 3:
					peripheral::changeBitFlags(debugFlags, debugVSSflag, debugInjectorFlag);
					break;

				default:
					break;

			}
			if ((debugFlags & debugEnableFlags) ^ retVal) configurePorts();

		case menuOutputDisplayIdx:
#ifdef useSpiffyTripLabels
			displayMainScreenFunctions(debugScreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
			displayMainScreenFunctions(debugScreenFormats, 0);
#endif // useSpiffyTripLabels
			break;

		default:
			break;

	}

	return retVal;

}

static void debugReading::configurePorts(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// configure VSS pin for either normal operation input or debug output
	if (debugFlags & debugVSSflag)
	{

#if defined(__AVR_ATmega32U4__)
		DDRB |= (1 << DDB7); // enable VSS sense pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRK |= (1 << DDK0); // enable VSS sense pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRC |= (1 << DDC0); // enable VSS sense pin interrupt
#endif // defined(__AVR_ATmega328P__)

		debugVSStickLength = 0;

		debugVSScount = 0;

		debugVSSidx = 0;

		debugVSSstate = 0; // start out by ramping up from 0 MPH to 250 MPH

		debugFlags &= ~(debugVSSready); // reset VSS ready bit

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
		DDRB &= ~(1 << DDB7); // disable VSS sense pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRK &= ~(1 << DDK0); // disable VSS sense pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRC &= ~(1 << DDC0); // disable VSS sense pin interrupt
#endif // defined(__AVR_ATmega328P__)

	}

	// configure fuel injector pins for either normal operation input or debug output
	if (debugFlags & debugInjectorFlag) // configure injector sense pins as outputs
	{

#if defined(__AVR_ATmega32U4__)
		DDRD |= ((1 << DDD3) | (1 << DDD2)); // enable injector sense pin interrupts
		PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRE |= ((1 << DDE4) | (1 << DDE5)); // enable injector sense pin interrupts
		PORTE |= ((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRD |= ((1 << DDD3) | (1 << DDD2)); // enable injector sense pin interrupts
		PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega328P__)

		debugFIPtickLength = 0;
		debugFIPWtickLength = 0;
		debugFIPWgoodTickLength = 0;
		debugFIPWreadTickLength = 0;

		debugFIPcount = 0;
		debugFIPWcount = 0;

		debugFIPidx = 0;

		debugFIPstate = 1; // start out by waiting before ramping up from 0 RPM to 12000 RPM

		debugFlags &= ~(debugFIPready); // reset pulse and pulse width ready bits

	}
	else // configure injector sense pins as inputs
	{

#if defined(__AVR_ATmega32U4__)
		DDRD &= ~((1 << DDD3) | (1 << DDD2)); // disable injector sense pin interrupts
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRE &= ~((1 << DDE4) | (1 << DDE5)); // disable injector sense pin interrupts
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRD &= ~((1 << DDD3) | (1 << DDD2)); // disable injector sense pin interrupts
#endif // defined(__AVR_ATmega328P__)

	}

	if (debugFlags & debugEnableFlags) timer1Command |= (t1cEnableDebug);
	else timer1Command &= ~(t1cEnableDebug);

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
/* Chrysler returnless fuel pressure correction display section */

const uint8_t pressureCorrectScreenFormats[4][2] PROGMEM = {
	 {(mpMAPpressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}		// Pressures
	,{(mpBaroPressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}
	,{(mpFuelPressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}
	,{(mpInjPressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}
};

const char pressureCorrectScreenFuncNames[] PROGMEM = {
	"Pressures\r"
};

static uint8_t pressureCorrect::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(pressureCorrectScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
#ifdef useSpiffyTripLabels
			displayMainScreenFunctions(pressureCorrectScreenFormats, cursorPos, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
			displayMainScreenFunctions(pressureCorrectScreenFormats, cursorPos);
#endif // useSpiffyTripLabels
			break;

		default:
			break;

	}

	return retVal;

}

#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
 /* ADC voltage display section */

const uint8_t analogReadScreenFormats[4][2] PROGMEM = {
	 {analog0Idx,				0x80 | tAnalogChannel}	// Voltages
	,{analog1Idx,				0x80 | tAnalogChannel}
	,{analog2Idx,				0x80 | tAnalogChannel}
	,{analog3Idx,				0x80 | tAnalogChannel}
};

const char analogReadScreenFuncNames[] PROGMEM = {
	"Voltages\r"
};

static uint8_t analogReadViewer::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(analogReadScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
#ifdef useSpiffyTripLabels
			displayMainScreenFunctions(analogReadScreenFormats, cursorPos, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
			displayMainScreenFunctions(analogReadScreenFormats, cursorPos);
#endif // useSpiffyTripLabels
			break;

		default:
			break;

	}

	return retVal;

}

#endif // useDebugAnalog
#ifdef useTestButtonValues
/* Button input value viewer section */

static uint8_t buttonView::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
#ifdef useAnalogButtons
			text::hexWordOut(devLCD, (uint16_t)(analogValue[(unsigned int)(analogButtonChannelIdx)]));
#endif // useAnalogButtons
			text::newLine(devLCD);
			text::hexByteOut(devLCD, thisButtonState);
			text::hexByteOut(devLCD, lastButtonState);
#ifdef useTestAnalogButtonIdx
			text::stringOut(devLCD, PSTR(" pins, "));
			text::hexByteOut(devLCD, thisButtonIdx);
			text::stringOut(devLCD, PSTR(" idx"));
#else // useTestAnalogButtonIdx
			text::stringOut(devLCD, PSTR(" pins"));
			text::newLine(devLCD);
#endif // useTestAnalogButtonIdx
			break;

		default:
			break;

	}

	return retVal;

}

#endif // useTestButtonValues
#ifdef useDebugTerminal
/* debug terminal section */

static uint8_t termRawIdx;

static char termNumberBuff[17];

static const uint8_t tBuffLength = 40;

static const char terminalHelp[] PROGMEM = {
	"   P - list stored parameters\r"
	"   V - list volatile variables\r"
	"   M - list main program variables\r"
	"   T - list read-in trip variable values\r"
	"   O - list program constants\r"
	"   L - list functions for read-in trip variable\r"
#ifdef useSWEET64trace
	"  ^L - list SWEET64 source code for function\r"
#endif // useSWEET64trace
	"[x]R - read trip variable x into terminal trip variable\r"
	"[x]W - write terminal trip variable into trip variable x\r"
	"       if no x specified, lists available trip variables\r"
	"   S - output system status\r"
	"   I - inject button press\r"
#ifdef useLegacyButtons
	"        short (l, c, r)\r"
	"         long (L, C, R)\r"
#else // useLegacyButtons
	"        short (l, c, r, u, d)\r"
	"         long (L, C, R, U, D)\r"
#endif // useLegacyButtons
	"   ? - displays this help\r"
	"\0"
};

static const uint8_t prgmParseDecimalDigit[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 10,								// multiply by 10
	instrJump, tParseCharacterToReg
};

static const uint8_t prgmParseHexDigit[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 16,								// multiply by 16
	instrJump, tParseCharacterToReg
};

static const uint8_t prgmFetchMainProgramValue[] PROGMEM = {
	instrLdRegMainIndexed, 0x02,
	instrDone
};

static const uint8_t prgmFetchConstantValue[] PROGMEM = {
	instrLdRegConstIndexed, 0x02,
	instrDone
};

static const uint8_t prgmFetchVolatileValue[] PROGMEM = {
	instrLdRegVolatileIndexed, 0x02,
	instrDone
};

static const uint8_t prgmFetchTripVarValue[] PROGMEM = {
	instrLdRegTripVarIndexedRV, 0x02, terminalIdx,
	instrDone
};

static const uint8_t prgmWriteMainProgramValue[] PROGMEM = {
	instrStRegMainIndexed, 0x06,
	instrDone
};

static const uint8_t prgmWriteVolatileValue[] PROGMEM = {
	instrStRegVolatileIndexed, 0x06,
	instrDone
};

static const uint8_t prgmWriteTripMeasurementValue[] PROGMEM = {
	instrStRegTripVarIndexedRV, 0x06, terminalIdx,
	instrDone
};

#ifdef useLegacyButtons
static const uint8_t terminalButtonCount = 6;

static const char terminalButtonChars[] PROGMEM = {
	"lcrLCR\0"
};

static const uint8_t terminalButtonValues[] PROGMEM = {
	 btnShortPressL
	,btnShortPressC
	,btnShortPressR
	,btnLongPressL
	,btnLongPressC
	,btnLongPressR
};

#else // useLegacyButtons
static const uint8_t terminalButtonCount = 10;

static const char terminalButtonChars[] PROGMEM = {
	"lcrudLCRUD\0"
};

static const char terminalButtonValues[] PROGMEM = {
	 btnShortPressL
	,btnShortPressC
	,btnShortPressR
	,btnShortPressU
	,btnShortPressD
	,btnLongPressL
	,btnLongPressC
	,btnLongPressR
	,btnLongPressU
	,btnLongPressD
};

#endif // useLegacyButtons
const char terminalActivityFlagStr[] PROGMEM = {
	"activityFlags: \0"
	"EngOff" "\xEC" "running\0"
	"VehStop" "\xEC" "moving\0"
	"NoButtons" "\xEC" "pressed\0"
	"Parked" "\xEC" "notparked\0"
	"Inactive" "\xEC" "active\0"
	"FuelRate" "\xEC" "fuelecon\0"
#ifdef useTWIbuttons
	"TWIsample" "\xEC" "twi\0"
#else // useTWIbuttons
	"1" "\xEC" "0\0"
#endif // useTWIbuttons
	"1" "\xEC" "0\0"
};

static void terminal::outputFlags(uint8_t flagRegister, const char * flagStr)
{

	uint8_t bitMask;
	uint8_t c;

	bitMask = 0x80;

	text::stringOut(devDebugTerminal, flagStr);

	for (uint8_t x = 1; x < 9; x++)
	{

		text::setModeOnCondition(devDebugTerminal, (flagRegister & bitMask), odvFlagEnableOutput);
		text::stringOut(devDebugTerminal, flagStr, x);
		text::setModeOnCondition(devDebugTerminal, 1, odvFlagEnableOutput);

		if (x == 8) c = 0x0D;
		else c = 0x20;
		text::charOut(devDebugTerminal, c);

		bitMask >>= 1;

	}

}

static void terminal::outputTripFunctionValue(uint8_t lineNumber)
{

	uint8_t i;

	i = text::numberOut(devDebugTerminal, terminalIdx, lineNumber, termNumberBuff, 0, dfOverflow9s);
	text::charOut(devDebugTerminal, ' ');
	text::stringOut(devDebugTerminal, terminalFormats, i);

}

static void terminal::outputConstantValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchConstantValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputParameterValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputParameterExtra(uint8_t lineNumber)
{

	text::hexByteOut(devDebugTerminal, EEPROM::getParameterFlags(lineNumber));
	text::charOut(devDebugTerminal, '-');
	text::hexByteOut(devDebugTerminal, EEPROM::getLength(lineNumber));
	text::charOut(devDebugTerminal, '-');
	text::hexByteOut(devDebugTerminal, EEPROM::getAddress(lineNumber));
	if (lineNumber < eePtrSettingsEnd)
	{

		text::stringOut(devDebugTerminal, PSTR(" (orig "));
		SWEET64::runPrgm(prgmFetchInitialParamValue, lineNumber);
		text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));
		text::stringOut(devDebugTerminal, PSTR(")"));

	}
	else
	{

		text::charOut(devDebugTerminal, ' ');
		SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);
		text::hexLWordOut(devDebugTerminal, &s64reg[s64reg2]);
		text::charOut(devDebugTerminal, ' ');

	}

}

static void terminal::outputVolatileValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchVolatileValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputMainProgramValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchMainProgramValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputTripVarMeasuredValue(uint8_t lineNumber)
{


	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputTripVarMeasuredExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::hexLWordOut(devDebugTerminal, &s64reg[s64reg2]);

}

#endif // useDebugTerminal
/* Trip save/restore/reset support section */

const uint8_t tripSelectList[] PROGMEM = {
	 currentIdx
	,tankIdx
#ifdef trackIdleEOCdata
	,eocIdleCurrentIdx
	,eocIdleTankIdx
#endif // trackIdleEOCdata
};

#ifdef useSavedTrips
uint8_t tripSlot;

const uint8_t tripSignatureList[] PROGMEM = {
	 pCurrTripSignatureIdx
	,pTankTripSignatureIdx
};

const char tripSaveFuncNames[] PROGMEM = {
	"Save \0"
	"Load \0"
	"Reset \0"
};

static uint8_t tripSupport::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;
	uint8_t tripIdx;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
			tripIdx = pgm_read_byte(&msTripList[(uint16_t)(tripSlot + 1)]);
			text::stringOut(devLCD, tripSaveFuncNames, cursorPos); // print trip function name at top left
			text::stringOut(devLCD, msTripNameString, tripSlot + 1);
			text::newLine(devLCD);

			text::gotoXY(devLCD, 0, 1); // go to next line

			if (EEPROM::readVal(pgm_read_byte(&tripSignatureList[(uint16_t)(tripSlot)])) == guinosig) text::stringOut(devLCD, PSTR("Present"));
			else text::stringOut(devLCD, PSTR("Empty"));

			text::newLine(devLCD);
			break;

		default:
			break;

	}

	return retVal;

}

static void tripSupport::goSaveTank(void)
{

	tripSlot = 1;
	cursor::moveAbsolute(tripSaveScreenIdx, 0);

}

static void tripSupport::goSaveCurrent(void)
{

	tripSlot = 0;
	cursor::moveAbsolute(tripSaveScreenIdx, 0);

}

static void tripSupport::select(void)
{

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 0: // save
			doWriteTrip(tripSlot);
			displayStatus(tripSlot, PSTR("Save"));
			doReturnToMainScreen();
			break;

		case 1: // load
			doReadTrip(tripSlot);
			displayStatus(tripSlot, PSTR("Load"));
			doReturnToMainScreen();
			break;

		case 2: // reset
			doResetTrip(tripSlot);
			displayStatus(tripSlot, PSTR("Reset"));
			doReturnToMainScreen();
			break;

		default:
			break;

	}

}

static uint8_t tripSupport::doAutoAction(uint8_t taaMode)
{

	uint8_t retVal = 0;

	if (EEPROM::readVal(pAutoSaveActiveIdx))
	{

		for (uint8_t x = 0; x < 2; x++)
			if (taaMode) retVal |= doReadTrip(x);
			else retVal |= doWriteTrip(x);

	}

	return retVal;

}

static uint8_t tripSupport::doReadTrip(uint8_t tripSlot)
{
	uint8_t retVal;

	retVal = 0;

	if (tripSlot) retVal += SWEET64::runPrgm(prgmLoadTankFromEEPROM, 0);
	else retVal += SWEET64::runPrgm(prgmLoadCurrentFromEEPROM, 0);

	return retVal;

}

static uint8_t tripSupport::doWriteTrip(uint8_t tripSlot)
{

	metricFlag &= ~(EEPROMbulkChangeFlag);

	if (tripSlot) SWEET64::runPrgm(prgmSaveTankToEEPROM, 0);
	else SWEET64::runPrgm(prgmSaveCurrentToEEPROM, 0);

	return (metricFlag & EEPROMbulkChangeFlag);

}

#endif // useSavedTrips
static void tripSupport::displayStatus(uint8_t tripSlot, const char * str)
{

	initStatusLine();
	text::stringOut(devLCD, msTripNameString, tripSlot + 1);
	text::stringOut(devLCD, PSTR(" Trip "));
	text::stringOut(devLCD, str);
	execStatusLine();

}

static void tripSupport::doResetTrip(uint8_t tripSlot)
{

#ifdef useSavedTrips
	doWriteTrip(tripSlot);

#endif // useSavedTrips
	tripVar::reset(pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot)]));
#ifdef trackIdleEOCdata
	tripVar::reset(pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot + 2)]));
#endif // trackIdleEOCdata

#if useBarFuelEconVsSpeed || usePartialRefuel
	if (tripSlot)
	{

#ifdef useBarFuelEconVsSpeed
		bgFEvsSsupport::reset();

#endif // useBarFuelEconVsSpeed
#ifdef usePartialRefuel
		EEPROM::writeVal(pRefuelSizeIdx, 0);

#endif // usePartialRefuel
	}

#endif // useBarFuelEconVsSpeed || usePartialRefuel
}

static void tripSupport::resetTank(void)
{

	doResetTrip(1);
	displayStatus(1, PSTR("Reset"));

}

static void tripSupport::resetCurrent(void)
{

	doResetTrip(0);
	displayStatus(0, PSTR("Reset"));

}

/* Main screen section */

const char mainScreenFuncNames[] PROGMEM = {
	"Instrument\r"
	"Custom\r"
	"Instant/Current\r"
	"Instant/Tank\r"
	"Current\r"
	"Tank\r"
#ifdef trackIdleEOCdata
	"EOC/Idle\r"
#endif // trackIdleEOCdata
	"Current Data\r"
	"Tank Data\r"
#ifdef trackIdleEOCdata
	"Current EOC/Idle\r"
	"Tank EOC/Idle\r"
#endif // trackIdleEOCdata
	"Remaining\r"
};

const uint8_t mainDisplayScreenFormats[(unsigned int)(mainScreenDisplayFormatSize)][2] PROGMEM = {
	 {lblInstantIdx,			tSpeed}					// Instrument
	,{lblInstantIdx,			tEngineSpeed}
	,{lblInstantIdx,			tFuelRate}
	,{lblInstantIdx,			tFuelEcon}

	,{lblInstantIdx,			tFuelEcon}				// Custom
	,{lblInstantIdx,			tSpeed}
	,{lblInstantIdx,			tFuelRate}
	,{lblCurrentIdx,			tFuelEcon}

	,{lblInstantIdx,			tFuelEcon}				// Instant / Current
	,{lblInstantIdx,			tSpeed}
	,{lblCurrentIdx,			tFuelEcon}
	,{lblCurrentIdx,			tDistance}

	,{lblInstantIdx,			tFuelEcon}				// Instant / Tank
	,{lblInstantIdx,			tSpeed}
	,{lblTankIdx,				tFuelEcon}
	,{lblTankIdx,				tDistance}

	,{lblCurrentIdx,			tSpeed}					// Current
	,{lblCurrentIdx,			tFuelEcon}
	,{lblCurrentIdx,			tDistance}
	,{lblCurrentIdx,			tFuelUsed}

	,{lblTankIdx,				tSpeed}					// Tank
	,{lblTankIdx,				tFuelEcon}
	,{lblTankIdx,				tDistance}
	,{lblTankIdx,				tFuelUsed}

#ifdef trackIdleEOCdata
	,{lblEOCidleCurrentIdx,		tDistance}				// EOC / Idle
	,{lblEOCidleCurrentIdx,		tFuelUsed}
	,{lblEOCidleTankIdx,		tDistance}
	,{lblEOCidleTankIdx,		tFuelUsed}

#endif // trackIdleEOCdata
	,{lblCurrentIdx,			tEngineRunTime}			// Current data
	,{lblCurrentIdx,			tFuelUsed}
	,{lblCurrentIdx,			tMotionTime}
	,{lblCurrentIdx,			tDistance}

	,{lblTankIdx,				tEngineRunTime}			// Tank data
	,{lblTankIdx,				tFuelUsed}
	,{lblTankIdx,				tMotionTime}
	,{lblTankIdx,				tDistance}

#ifdef trackIdleEOCdata
	,{lblEOCidleCurrentIdx,		tEngineRunTime}			// Current EOC / Idle
	,{lblEOCidleCurrentIdx,		tFuelUsed}
	,{lblEOCidleCurrentIdx,		tMotionTime}
	,{lblEOCidleCurrentIdx,		tDistance}

	,{lblEOCidleTankIdx,		tEngineRunTime}			// Tank EOC / Idle
	,{lblEOCidleTankIdx,		tFuelUsed}
	,{lblEOCidleTankIdx,		tMotionTime}
	,{lblEOCidleTankIdx,		tDistance}

#endif // trackIdleEOCdata
	,{lblTankIdx,				tFuelUsed}				// Remaining
	,{lblTankIdx,				tRemainingFuel}
	,{lblTankIdx,				tTimeToEmpty}
	,{lblTankIdx,				tDistanceToEmpty}
};

#ifdef trackIdleEOCdata
const uint8_t mainEOCscreenFormats[(unsigned int)(4)][2] PROGMEM = {
	 {lblInstantIdx,			tSpeed}
	,{lblCurrentIdx,			tFuelEcon}
	,{lblEOCidleCurrentIdx,		tDistance}
	,{lblEOCidleTankIdx,		tDistance}
};

const uint8_t mainIdleScreenFormats[(unsigned int)(4)][2] PROGMEM = {
	 {lblInstantIdx,			tFuelRate}
	,{lblCurrentIdx,			tFuelEcon}
	,{lblEOCidleCurrentIdx,		tFuelUsed}
	,{lblEOCidleTankIdx,		tFuelUsed}
};

#endif // trackIdleEOCdata
uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

#ifdef useScreenEditor
	uint8_t i;
#endif // useScreenEditor
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			topScreenLevel = menuLevel; // save current menu level for sub-function (param edit, trip load/save, etc) support

		case menuCursorUpdateIdx:
			printStatusMessage(mainScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			retVal = (activityFlags & afActivityCheckFlags);
			switch (retVal)
			{

				case (afVehicleStoppedFlag | afButtonFlag):
#ifdef trackIdleEOCdata
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainIdleScreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainIdleScreenFormats, 0);
#endif // useSpiffyTripLabels
					break;

#endif // trackIdleEOCdata
				case (afEngineOffFlag | afButtonFlag):
#ifdef trackIdleEOCdata
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainEOCscreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainEOCscreenFormats, 0);
#endif // useSpiffyTripLabels
					break;

#endif // trackIdleEOCdata
				default:
#ifdef useScreenEditor
					i = cursorPos;
					i <<= 2;
					i += eePtrScreensStart;

#ifdef useSpiffyTripLabels
					for (uint8_t x = 0; x < 4; x++) displayFunction(x, EEPROM::readVal(i++), EEPROM::readVal(i++), 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					for (uint8_t x = 0; x < 4; x++) displayFunction(x, EEPROM::readVal(i++), EEPROM::readVal(i++));
#endif // useSpiffyTripLabels

#else // useScreenEditor
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos);
#endif // useSpiffyTripLabels
#endif // useScreenEditor
					break;

			}
			retVal = 0;
			break;

		default:
			break;

	}

	return retVal;

}

void doReturnToMainScreen(void)
{

	cursor::moveAbsolute(topScreenLevel, 255);

}

void doNextBright(void)
{

	initStatusLine();
	text::stringOut(devLCD, PSTR("Backlight = "));
	text::stringOut(devLCD, brightString, brightnessIdx);
	execStatusLine();

}

#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void)
{

	peripheral::shutdownMain();

#ifdef useTWIsupport
	TWI::shutdown();

#endif // useTWIsupport
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

	peripheral::initMain(); // restart all peripherals

}

#endif // useDeepSleep
#ifdef useCPUreading
/* CPU loading and RAM availability support section */

static const uint8_t prgmFindCPUutilPercent[] PROGMEM = {
	instrLdRegMain, 0x02, mpMainLoopAccumulatorIdx,
	instrSubMainFromX, 0x02, mpIdleAccumulatorIdx,
	instrMul2byByte, 100,
	instrMul2byConst, idxDecimalPoint,
	instrDiv2byMain, mpMainLoopAccumulatorIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmOutputAvailableRAM[] PROGMEM = {
	instrLdRegMain, 0x02, mpAvailableRAMidx,
	instrMul2byConst, idxDecimalPoint,
	instrDone											// exit to caller
};

static uint8_t systemInfo::displayHandler (uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint16_t availableRAMptr;
	uint8_t retVal = 0;

	if((unsigned int)__brkval == 0) availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)&__bss_end);
	else availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)__brkval);

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx: // display max cpu utilization and RAM
			showCPUload();
			text::stringOut(devLCD, PSTR(" T"));
			text::stringOut(devLCD, ull2str(tReadTicksToSeconds, mBuff1, vSystemCycleIdx)); // output system time (since MPGuino was powered up)

			text::gotoXY(devLCD, 0, 1);
			text::stringOut(devLCD, PSTR("FREE RAM: "));
			mainProgramVariables[(uint16_t)(mpAvailableRAMidx)] = availableRAMptr;
			SWEET64::runPrgm(prgmOutputAvailableRAM, 0);
			text::stringOut(devLCD, formatDecimal(mBuff1, 6, 2, 0));
			break;

		default:
			break;

	}

	return retVal;

}

static void systemInfo::showCPUload(void)
{

	text::stringOut(devLCD, PSTR("C%"));
	SWEET64::runPrgm(prgmFindCPUutilPercent, 0);
	text::stringOut(devLCD, formatDecimal(mBuff1, 6, 2, 0));
#ifdef useDebugCPUreading
	monitorState = 1;
#endif // useDebugCPUreading

}

static void systemInfo::showCPUloading(void)
{

	initStatusLine();
	showCPUload();
	text::charOut(devLCD, 0x0D);
	execStatusLine();

}

static uint16_t systemInfo::getAvailableRAMsize(void)
{

}

static uint32_t systemInfo::findCycleLength(unsigned long lastCycle, unsigned long thisCycle)
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

static uint32_t systemInfo::cycles0(void)
{

	uint8_t oldSREG;
	unsigned long t;
	unsigned int a;

	oldSREG = SREG; // save state of interrupt flag
	cli(); // disable interrupts

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	t = timer0_overflow_count + (unsigned long)(a);

	SREG = oldSREG; // restore state of interrupt flag

	return t;

}

#endif // useCPUreading
#ifdef useDragRaceFunction
// drag race function
//
// upon successful arming, the drag race function will measure times to reach the distance specified by the EEPROM parameter pDragDistanceIdx (preset to 1/4 mile),
// and to reach the speed specified by the EEPROM parameter pDragSpeedIdx (preset to 60 MPH).

uint8_t accelTestStatus;
uint8_t lastAccelTestStatus;

/*

meaning of value contained in accel test state variable accelTestState is as follows:

0  - no status change
1  - accel test ready
2  - accel test armed
3  - accel test finished
4  - accel test cancelled
5  - accel test distance checkpoint reached
6  - accel test half speed checkpoint reached
7  - accel test half speed and distance checkpoints reached
8  - accel test full speed checkpoint reached
9  - accel test full speed and distance checkpoints reached
10 - accel test full speed and half speed checkpoints reached
11 - accel test full speed, half speed, and distance checkpoints reached
12 - invalid accel test state encountered

*/
uint8_t accelTestState;

const char accelTestStateMsgs[] PROGMEM = {
	"\r"
	"Drag Ready\r"
	"Drag Test Start\r"
	"Drag Finished\r"
	"Drag Cancelled\r"
	"DIST\r"
	"     HALF\r"
	"DIST HALF\r"
	"          FULL\r"
	"DIST      FULL\r"
	"     HALF FULL\r"
	"DIST HALF FULL\r"
	"Drag Test Fail\r"
};

// local trip indexes for drag calculation trip function display variables
#define nextAllowedValue 0
const uint8_t drDragHalfSpeedIdx =	nextAllowedValue;
const uint8_t drDragFullSpeedIdx =	drDragHalfSpeedIdx + 1;
const uint8_t drDragDistanceIdx =	drDragFullSpeedIdx + 1;
#define nextAllowedValue drDragDistanceIdx + 1

const uint8_t lblDragHalfSpeedIdx =	(drDragHalfSpeedIdx << dfBitShift)	| dragHalfSpeedIdx;
const uint8_t lblDragFullSpeedIdx =	(drDragFullSpeedIdx << dfBitShift)	| dragFullSpeedIdx;
const uint8_t lblDragDistanceIdx =	(drDragDistanceIdx << dfBitShift)	| dragDistanceIdx;

#ifdef useSpiffyTripLabels
// display variable trip labels
const uint8_t drTripBitPattern[][4] PROGMEM = {
	{0b00000000, 0b00000001, 0b00000010, 0b00000100} // /
	,{0b00000000, 0b00000010, 0b00000101, 0b00000010} // full circle
	,{0b00000000, 0b00000110, 0b00000101, 0b00000110} // D
};

#endif // useSpiffyTripLabels
const uint8_t dragRaceScreenFormats[] PROGMEM = {
	 {lblDragDistanceIdx,	tFuelEcon}
	,{lblDragDistanceIdx,	tDragSpeed}						// for calculations, it really doesn't matter what trip index is used here
	,{lblDragDistanceIdx,	tAccelTestTime}
	,{lblDragDistanceIdx,	0x80 | tEstimatedEnginePower}	// for calculations, it really doesn't matter what trip index is used here

	,{lblDragHalfSpeedIdx,	tAccelTestTime}
	,{lblDragHalfSpeedIdx,	tFuelUsed}
	,{lblDragHalfSpeedIdx,	tDistance}
	,{lblDragHalfSpeedIdx,	tFuelEcon}

	,{lblDragFullSpeedIdx,	tAccelTestTime}
	,{lblDragFullSpeedIdx,	tFuelUsed}
	,{lblDragFullSpeedIdx,	tDistance}
	,{lblDragFullSpeedIdx,	tFuelEcon}

	,{lblDragDistanceIdx,	tAccelTestTime}
	,{lblDragDistanceIdx,	tFuelUsed}
	,{lblDragDistanceIdx,	tDistance}
	,{lblDragDistanceIdx,	tFuelEcon}
};

static const uint8_t prgmInitializeAccelTest[] PROGMEM = {
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// fetch cycles per second constant
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,		// store it in instantaneous accel test speed period variable
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch distance parameter value in VSS pulses
	instrMul2byEEPROM, pDragSpeedIdx,					// multiply by drag function speed parameter value in unit distance per hour
	instrLdReg, 0x21,									// save denominator term in VSS pulses per hour for later
	instrLdRegConst, 0x02, idxSecondsPerHour,			// fetch seconds per hour constant
	instrMul2byConst, idxCycles0PerSecond,				// multply by cycles per second constant
	instrShiftRegLeft, 0x02,							// multiply it by two to obtain accel half speed period value
	instrMul2byConst, idxDecimalPoint,					// multiply to factor out decimal formatting term from accel test speed parameter value
	instrDiv2by1,										// perform conversion
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelFullPeriodValueIdx,	// save it to accel test full-speed period tripwire variable
	instrShiftRegLeft, 0x02,							// shift result right one bit
	instrStRegVolatile, 0x02, vAccelHalfPeriodValueIdx,	// save it to accel test half-speed period tripwire variable
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch drag function distance parameter value in VSS pulses
	instrMul2byConst, pDragDistanceIdx,					// multiply by drag function distance parameter value in unit distance
	instrDiv2byConst, idxDecimalPoint,					// get rid of decimal formatting factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelDistanceValueIdx,	// save it to accel test distanct tripwire variable
	instrDone											// exit to caller
};

static const uint8_t prgmAccelTestCompareFullSpeeds[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrTestReg, 0x01,									// test accel test autotrigger flag
	instrBranchIfZero, 12,								// if zero, then output a 1
	instrLdRegTripVar, 0x01, dragRawFullSpeedIdx, rvVSScycleIdx,	// load raw full speed register elapsed time
	instrLdRegTripVar, 0x02, dragFullSpeedIdx, rvVSScycleIdx,		// load regular full speed register elapsed time
	instrCmpXtoY, 0x21,									// if raw full speed register has a smaller time than regular full speed register, output a 1
	instrBranchIfLTorE, 4,								// if raw full speed register has a smaller time than regular full speed register, output a 1

//oneret:
	instrLdRegByte, 0x02, 1,							// load a 1 into register 2
	instrDone,											// exit to caller

//zeroret:
	instrLdRegByte, 0x02, 0,							// load a 0 into register 2
	instrDone											// exit to caller
};

void accelerationTest::goDisplay(void)
{

	if (accelTestState) // if acceleration test state has changed
	{

		// display status message for new state
		printStatusMessage(accelTestStateMsgs, accelTestState);

	}

#ifdef useSpiffyTripLabels
	displayMainScreenFunctions(dragRaceScreenFormats, screenCursor[(unsigned int)(dragRaceIdx)], 136, 0, drTripBitPattern);
#else // useSpiffyTripLabels
	displayMainScreenFunctions(dragRaceScreenFormats, screenCursor[(unsigned int)(dragRaceIdx)]);
#endif // useSpiffyTripLabels

}

void accelerationTest::goTrigger(void)
{

	switch (triggerTest())
	{

		case 1: // vehicle is still moving - can't trigger drag mode
			printStatusMessage(PSTR("Stop vehicle"));
			break;

		case 2: // vehicle engine has stopped - can't trigger drag mode
			printStatusMessage(PSTR("Start Engine"));
			break;

		case 3: // acceleration test manually cancelled, so force manual trigger mode
			EEPROM::writeVal(pDragAutoFlagIdx, 0);
			break;

		default:
			break;

	}

}

uint8_t accelerationTest::triggerTest(void)
{

	uint8_t oldSREG;

	if (accelerationFlags & accelTestInProgress)
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		accelerationFlags &= ~accelTestClearFlags; // reset drag race capture flags
		accelerationFlags |= (accelTestCompleteFlags); // signal that drag function is cancelled

		SREG = oldSREG; // restore state of interrupt flag

		return 3;

	}
	else
	{

		if (dirty & dGoodVehicleMotion) // if vehicle is still moving, then alert driver
		{

			return 1; // vehicle is still moving - can't trigger drag mode

		}
		else // otherwise, set drag trigger
		{

			if (dirty & dGoodEngineRotationOpen) // if vehicle engine is running, go trigger drag mode
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts

				accelerationFlags &= ~accelTestClearFlags; // turn off all acceleration test functionality in interrupt-land

				SREG = oldSREG; // restore state of interrupt flag

				tripVar::reset(dragRawHalfSpeedIdx); // zero out acceleration 1/2 speed setpoint data
				tripVar::reset(dragRawFullSpeedIdx); // zero out acceleration full speed setpoint data
				tripVar::reset(dragRawDistanceIdx); // zero out acceleration distance setpoint data

				SWEET64::runPrgm(prgmInitializeAccelTest, 0); // initialize low-level values and tripwires

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts

				accelerationFlags |= (accelTestTriggered | accelTestMeasurementFlags); // set drag flags in accelerationFlags register

				SREG = oldSREG; // restore state of interrupt flag

				return 0;

			}
			else
			{

				return 2; // vehicle engine has stopped - can't trigger drag mode

			}

		}

	}

}

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator // Coastdown Calculator support section
uint8_t coastdownCharIdx;

const char coastdownMsgs[] PROGMEM = {
	"Coastdown Active\r"
	"CD Sample 1 read\r"
	"CD Sample 2 read\r"
	"CD Sample 3 read\r"
	"Coastdown Done\r"
	"Coastdown Canx\r"
};

const char coastdownSymbol[] PROGMEM = "|}>}|{<{";

void coastdown::goDisplay(void)
{

	const char * msgPtr = 0;

	uint8_t oldSREG;
	uint8_t i;

	if (coastdownFlags & cdtFinished) // coastdown test has finished - let's find out why
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		i = coastdownFlags; // save coastdown flag state
		coastdownFlags &= ~(cdtTestClearFlags);

		SREG = oldSREG; // restore state of interrupt flag

		if (i & cdtCancelled) // coastdown test has cancelled
		{

			msgPtr = findStr(coastdownMsgs, 5); // signal that test was cancelled

		}
		else // coastdown test finished normally
		{

			msgPtr = findStr(coastdownMsgs, 4); // signal that test finished normally
			// insert coastdown determiniation call here

		}

	}
	else
	{

		if (coastdownFlags & cdSignalStateChange)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts

			coastdownFlags &= ~(cdSignalStateChange);
			i = coastdownState; // fetch current coastdown state

			SREG = oldSREG; // restore state of interrupt flag

			msgPtr = findStr(coastdownMsgs, i); // get appropriate coastdown test message pointer

		}

	}

	if (msgPtr) printStatusMessage(msgPtr);

	if (coastdownFlags & cdtActive) // coastdown test is in progress - display changes accordingly
	{

		coastdownCharIdx &= 0x07;
		text::newLine(devLCD);

#ifdef useSpiffyTripLabels
		displayFunction(2, lblInstantIdx, tSpeed, 136, 0, msTripBitPattern); // call main screen function display routine
#else // useSpiffyTripLabels
		displayFunction(2, lblInstantIdx, tSpeed); // call main screen function display routine
#endif // useSpiffyTripLabels

		text::gotoXY(devLCD, 8, 1);
		text::stringOut(devLCD, PSTR(" ACTIVE"));
		text::charOut(devLCD, pgm_read_byte(&coastdownSymbol[(unsigned int)(coastdownCharIdx++)]));

	}
	else
	{

		i = screenCursor[(unsigned int)(coastdownIdx)] + pCoefficientDidx;

		SWEET64::runPrgm(prgmFetchParameterValue, i);
		ull2str(tFormatToNumber, pBuff, 3);

		text::stringOut(devLCD, parmLabels, i); // print parameter name at top left
		text::stringOut(devLCD, pBuff);

	}

	text::newLine(devLCD);

}

void coastdown::goTrigger(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (coastdownFlags & cdtTestInProgress) // signal that coastdown test is cancelled
	{

		coastdownFlags &= ~(cdtTestClearFlags); // signal that coastdown test is no longer active
		coastdownFlags |= cdtCancelled | cdtFinished | cdSignalStateChange; // signal that coastdown test is cancelled

	}
	else
	{

		coastdownFlags &= ~(cdtTestClearFlags); // signal that coastdown test is no longer active
		coastdownFlags |= cdtTriggered; // set coastdown test flags in coastdownFlags register

	}

	coastdownState = 0; // reset coastdown state

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // useCoastDownCalculator
#ifdef usePartialRefuel
/* partial refuelling support section */

const char partialRefuelFuncNames[] PROGMEM = {
	"Add Partial\r"
	"Zero Partial\r"
	"Reset Tank Trip\r"
};

static const char prStatusMessages[] PROGMEM = {
	"No change\r"
	"Added Partial\r"
	"Canx Partial Add\r"
};

static uint8_t partialRefuel::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			numberEditObj.neStatusMessage = prStatusMessages;
			numberEditObj.parameterIdx = pRefuelSizeIdx;
			parameterEdit::sharedFunctionCall(nesLoadInitial);

		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
			text::stringOut(devLCD, partialRefuelFuncNames, screenCursor[(unsigned int)(menuLevel)]); // print trip function name at top left
			text::stringOut(devLCD, pBuff); // print value
			text::stringOut(devLCD, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" "*1K\r"));
			break;

		default:
			break;

	}

	return retVal;

}

static void partialRefuel::entry(void)
{

	cursor::moveAbsolute(partialRefuelScreenIdx, 0);

}

static void partialRefuel::select(void)
{

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 0: // go edit partial refuel value
			parameterEdit::entry();
			break;

		default:
			noSupport();
			break;

	}

}

static void partialRefuel::longSelect(void)
{

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 1: // reset partial refuel quantity
			SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0); // initialize 64-bit number to zero
			parameterEdit::sharedFunctionCall(nesSaveParameter);
			printStatusMessage(PSTR("PartialFuel RST"));
			doReturnToMainScreen();
			break;

		case 2: // tank trip reset
			tripSupport::resetTank();
			doReturnToMainScreen();
			break;

		default:
			select();
			break;

	}

}

#endif // usePartialRefuel
#ifdef useScreenEditor // Programmable main display screen edit support section
uint8_t screenTripValue = 0;
uint8_t screenFunctionValue = 0;
uint8_t screenEditDirty = 0;

const uint8_t screenEditFlag_dirty =	0x80;

const char seFormatRevertedString[] PROGMEM = "Format reverted";
const char seExitScreenEditString[] PROGMEM = "Screen Display";

void doCursorUpdateScreenEdit(void)
{

	uint8_t b;

	b = screenCursor[(unsigned int)(screenEditIdx)];
	b &= ~screenEditFlag_dirty;

	if ((screenEditDirty & ~screenEditFlag_dirty) ^ b) // if cursor moved to a different screen function altogether
	{

		screenTripValue = EEPROM::readVal(b + eePtrScreensStart);
		screenFunctionValue = EEPROM::readVal(b + eePtrScreensStart + 1);

		if (screenEditDirty & screenEditFlag_dirty) // if previous screen function was changed but not saved
		{

			printStatusMessage(seFormatRevertedString);

		}

		screenEditDirty = b; // save current cursor position of screen function being edited
		screenEditDirty &= ~screenEditFlag_dirty; // mark screen function as not modified

	}

}

void doScreenEditDisplay(void)
{

	uint8_t i;
	uint8_t j;
	uint8_t k;

	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t calcBlink;
	uint8_t tripBlink;

	i = screenCursor[(unsigned int)(screenEditIdx)];

	j = i; // extract whether trip index or trip function is to be edited
	j &= 0x01;

	i >>= 1; // figure out edited screen function placement on screen
	k = i;

	i &= 0xFC; // figure out start of screen page
	k &= 0x03;

	for (uint8_t x = 0; x < 4; x++)
	{

		calcBlink = 0;
		tripBlink = 0;

		if (x == k) // if at
		{

			tripIdx = screenTripValue;
			calcIdx = screenFunctionValue;
			if (j == 1) calcBlink = 170;
			else tripBlink = 170;

		}
		else
		{

			tripIdx = EEPROM::readVal(eePtrScreensStart + (i + x) * 2);
			calcIdx = EEPROM::readVal(eePtrScreensStart + (i + x) * 2 + 1);

		}

#ifdef useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, msTripBitPattern); // call main screen function display routine
#else // useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx); // call main screen function display routine
#endif // useSpiffyTripLabels

	}

}

void doGoScreenEdit(void)
{

	screenEditDirty = screenCursor[(unsigned int)(screenEditIdx)];
	screenEditDirty >>= 1;
	screenEditDirty ^= 0x01; // force update of screen function
	screenEditDirty &= ~screenEditFlag_dirty;

	cursor::moveAbsolute(screenEditIdx, screenCursor[(unsigned int)(mainScreenIdx)] << 3);

	printStatusMessage(PSTR("Screen Editor"));

}

void doScreenEditBump(void)
{

	uint8_t b;

	b = (screenCursor[(unsigned int)(screenEditIdx)] & 0x01); // figure out whether trip variable or trip function is being modified
	screenEditDirty |= screenEditFlag_dirty; // mark current screen function as modified

	if (b) // if trip function is being modified
	{

		screenFunctionValue++;
		if (screenFunctionValue == dfMaxValDisplayCount) screenFunctionValue = 0;

	}
	else // otherwise, trip variable is being modified
	{

		screenTripValue++;
		if (screenTripValue == dfMaxTripCount) screenTripValue = 0;

	}

}

void doSaveScreen(void)
{

	uint8_t b;

	b = screenCursor[(unsigned int)(screenEditIdx)];

	// generate and save new screen function from trip variable and trip function
	EEPROM::writeVal(eePtrScreensStart + b, (unsigned long)(screenTripValue));
	EEPROM::writeVal(eePtrScreensStart + b + 1, (unsigned long)(screenFunctionValue));
	screenEditDirty &= ~screenEditFlag_dirty;

	printStatusMessage(PSTR("Format saved"));

}

void doScreenReturnToMain(void)
{

	const char * str;

	if (screenEditDirty & screenEditFlag_dirty) str = seFormatRevertedString;
	else s = seExitScreenEditString;

	screenCursor[(unsigned int)(mainScreenIdx)] = (screenCursor[(unsigned int)(screenEditIdx)] >> 3);
	cursor::screenLevelEntry(str, topScreenLevel);

}

#endif // useScreenEditor
/* EEPROM parameter I/O section */

static const uint8_t prgmInitEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x02, pSignatureIdx,				// load the MPGuino signature long word currently in EEPROM
	instrLdRegEinit, 0x01, pSignatureIdx,				// load the MPGuino signature long word inside the program
	instrCmpXtoY, 0x21,									// are they the same?
	instrBranchIfNotE, 4,								// if not, go initialize the EEPROM
	instrLdRegByte, 0x02, 0,							// otherwise, signal nothing got initialized
	instrDone,											// exit to caller

//cont:
	instrLxdI, pSignatureIdx,

//loop:
	instrLdRegEinitIndexed, 0x02,						// loop through entire EEPROM space
	instrStRegEEPROMindexed, 0x02,						// store the program stored parameter defaults into EEPROM
	instrAddIndex, 1,
	instrCmpIndex, eePtrSettingsEnd,
	instrBranchIfLT, 246,
#ifdef useEEPROMtripStorage
	instrLdRegByte, 0x02, 0,							// zero out current and tank trip signature bytes
	instrStRegEEPROM, 0x02, pCurrTripSignatureIdx,
	instrStRegEEPROM, 0x02, pTankTripSignatureIdx,
#endif // useEEPROMtripStorage
	instrLdRegByte, 0x02, 1,
	instrDone											// exit to caller
};

static const uint8_t prgmInitMPGuino[] PROGMEM = {
	instrLdRegEEPROM, 0x02, pIdleTimeoutIdx,			// load idle timeout value in seconds
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vVehicleStopTimeoutIdx,	// store idle timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pEOCtimeoutIdx,				// load EOC timeout value in seconds
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vEngineOffTimeoutIdx,		// store EOC timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pMinGoodSpeedidx,			// fetch minimum good vehicle speed value in (distance)(* 1000) / hour
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by distance parameter value in (VSS pulses) / (distance)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegConst, 0x02, idxSecondsPerHour,			// fetch seconds / hour constant
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrMul2byConst, idxDecimalPoint,					// term is now (timer0 cycles)(* 1000) / (hour)
	instrDiv2by1,										// perform conversion, term is now in timer0 cycles / VSS pulse
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumVSSperiodIdx,		// store maximum good VSS period of timer0 cycles / VSS pulse
	instrDiv2byConst, idxCycles0PerTick,				// perform conversion, term is now in timer0 ticks
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vDetectVehicleStopIdx,	// store minimum good vehicle speed of timer0 ticks / VSS pulse

	instrLdRegEEPROM, 0x02, pCrankRevPerInjIdx,			// load number of crank revolutions / injector fire event
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrMul2byByte, 60,								// term is now in (crank revolutions)(timer0 cycles) / (fire event)(minute)
	instrDiv2byEEPROM, pMinGoodRPMidx,					// divide by minimum good engine speed value in crank revolutions / minute
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// store maximum good engine period value in timer0 cycles / fire event
	instrDiv2byConst, idxCycles0PerTick,				// perform conversion, term is now in timer0 ticks
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vDetectEngineOffIdx,		// store minimum good engine speed value in timer0 ticks / fire event

	instrLdRegEEPROM, 0x02, pButtonTimeoutIdx,			// load button press timeout stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vButtonTimeoutIdx,		// store button press timeout timer ticks value

	instrLdRegEEPROM, 0x02, pParkTimeoutIdx,			// load parking timeout stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vParkTimeoutIdx,			// store parking timeout timer ticks value

	instrLdRegEEPROM, 0x02, pActivityTimeoutIdx,		// load activity timeout stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vActivityTimeoutIdx,		// store activity timeout timer ticks value

	instrLdRegEEPROM, 0x02, pInjectorOpeningTimeIdx,	// fetch injector opening settle time in microseconds
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrDiv2byConst, idxMicroSecondsPerSecond,			// divide by microseconds per seconds value
	instrStRegVolatile, 0x02, vInjectorOpenDelayIdx,	// store injector opening settle time value in cycles
	instrLdReg, 0x23,									// save for later injector opening settle time value in register 3

	instrLdRegEEPROM, 0x02, pInjectorClosingTimeIdx,	// fetch injector closing settle time in microseconds
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrDiv2byConst, idxMicroSecondsPerSecond,			// divide by microseconds per seconds value
	instrStRegVolatile, 0x02, vInjectorCloseDelayIdx,	// store injector closing settle time value in cycles
	instrAddYtoX, 0x32,									// add injector opening settle time value to total injector settle time
	instrStRegVolatile, 0x02, vInjectorTotalDelayIdx,	// store injector total settle time value in cycles
	instrLdReg, 0x23,									// save injector total settle time value in register 3
	instrLdRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// load maximum good engine period value in timer0 cycles / fire event
	instrSubYfromX, 0x32,								// subtract total injector settle time from maximum good engine period
	instrStRegVolatile, 0x02, vInjectorValidMaxWidthIdx,	// store maximum valid fuel injector pulse width in timer0 cycles

	instrLdRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,	// fetch injector cycle time in microseconds per US gallon
#ifdef useImperialGallon
	instrBranchIfMetricMode, 5,							// if metric mode set, skip ahead to cycles conversion
	instrMul2byConst, idxNumerImperialGallon,			// perform conversion from microseconds per liter into microseconds per Imperial gallon
	instrDiv2byConst, idxDenomImperialGallon,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont0:
#else	// useImperialGallon
	instrBranchIfSAEmode, 5,							// if metric mode not set, skip ahead to cycles conversion
#endif	// useImperialGallon
	instrMul2byConst, idxDenomVolume,					// perform conversion from microseconds per US gallon into microseconds per liter
	instrDiv2byConst, idxNumerVolume,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont:
	instrMul2byConst, idxCycles0PerSecond,				// multiply to get cycles-microseconds per unit volume-second value
	instrDiv2byConst, idxMicroSecondsPerSecond,			// convert to get cycles per unit volume value
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpCyclesPerVolumeIdx,			// save cycles per unit volume value

	instrLdRegEEPROM, 0x02, pTankSizeIdx,				// fetch tank size in unit volume * formatting factor
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpTankSizeIdx,				// save tank size in cycles

	instrLdRegEEPROM, 0x02, pTankBingoSizeIdx,			// fetch bingo tank size in unit volume
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get bingo tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpBingoTankSizeIdx,			// save bingo tank size in cycles

#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,				// fetch partial refuel tank size in unit volume
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get partial refuel tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpPartialRefuelTankSize,		// save partial refuel tank size in cycles

#endif	// usePartialRefuel
#ifdef useDragRaceFunction
	instrLdRegByte, 0x02, 0,							// zero out accel test top speed and estimated engine power
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,

#endif	// useDragRaceFunction
#ifdef useCoastDownCalculator
	instrLdRegEEPROM, 0x02, pCoastdownSamplePeriodIdx,	// coastdown timer ticks value
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vCoastdownPeriodIdx,		// store coastdown timeout timer ticks value

#endif	// useCoastDownCalculator
#ifdef useChryslerMAPCorrection
	instrLdRegEEPROM, 0x02, pMAPsensorFloorIdx,			// convert pressure sensor voltage floor to equivalent ADC floor value
	instrMul2byConst, idxNumerVoltage,
	instrDiv2byConst, idxDenomVoltage,
	instrStRegMain, 0x02, mpAnalogMAPfloorIdx,			// store it in volatile variable storage

	instrLdRegEEPROM, 0x02, pMAPsensorCeilingIdx,		// calculate pressure sensor ADC step slope denominator
	instrLdRegEEPROM, 0x01, pMAPsensorFloorIdx,
	instrSubYfromX, 0x12,
	instrMul2byConst, idxNumerVoltage,
	instrStRegMain, 0x02, mpAnalogMAPdenomIdx,			// store it in volatile variable storage

	instrLdRegEEPROM, 0x02, pMAPsensorRangeIdx,
	instrMul2byConst, idxDenomVoltage,					// calculate pressure sensor ADC step slope numerator
	instrStRegMain, 0x02, mpAnalogMAPnumerIdx,			// store it in volatile variable storage

#ifdef useChryslerBaroSensor
	instrLdRegEEPROM, 0x02, pBaroSensorFloorIdx,		// convert pressure sensor voltage floor to equivalent ADC floor value
	instrMul2byConst, idxNumerVoltage,
	instrDiv2byConst, idxDenomVoltage,
	instrStRegMain, 0x02, mpAnalogBaroFloorIdx,			// store it in volatile variable storage

	instrLdRegEEPROM, 0x02, pBaroSensorCeilingIdx,		// calculate pressure sensor ADC step slope numerator
	instrLdRegEEPROM, 0x01, pBaroSensorFloorIdx,
	instrSubYfromX, 0x12,
	instrMul2byConst, idxNumerVoltage,
	instrStRegMain, 0x02, mpAnalogBaroDenomIdx,			// store it in volatile variable storage

	instrLdRegConst, 0x02, idxDenomVoltage,				// calculate pressure sensor ADC step slope numerator
	instrMul2byEEPROM, pBaroSensorRangeIdx,
	instrStRegMain, 0x02, mpAnalogBaroNumerIdx,			// store it in volatile variable storage
#else	// useChryslerBaroSensor

	instrLdRegEEPROM, 0x02, pBarometricPressureIdx,		// fetch reference barometric pressure value in psig * 1000
	instrStRegMain, 0x02, mpBaroPressureIdx,			// store it in volatile variable storage
#endif	// useChryslerBaroSensor
	instrLdRegEEPROM, 0x02, pSysFuelPressureIdx,		// fetch base fuel pressure value in psig * 1000
	instrStRegMain, 0x02, mpFuelPressureIdx,			// save base fuel pressure value in psig * 1000

	instrLdRegConst, 0x02, idxCorrectionFactor,
	instrStRegMain, 0x02, mpInjectorCorrectionIdx,		// save initial injector correction index for pressure differential calculation

#endif	// useChryslerMAPCorrection
#ifdef useBarFuelEconVsTime
	instrLdRegEEPROM, 0x02, pFEvsTimeIdx,				// load fuel econ vs time period stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vFEvsTimePeriodTimeoutIdx,	// store fuel econ vs time period timer ticks value

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	instrLdRegEEPROM, 0x02, pBarLowSpeedCutoffIdx,		// obtain low-speed cutoff parameter in (distance)(* 1000) / (hour)
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulses)(* 1000) / (hour)
	instrDiv2byConst, idxSecondsPerHour,				// term is now (VSS pulses)(* 1000) / (second)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpFEvsSpeedMinThresholdIdx,	// store minimum threshold speed in (VSS pulses)(* 1000) / (second)

	instrLdRegEEPROM, 0x02, pBarSpeedQuantumIdx,		// fetch speed quantum parameter in (distance)(* 1000) / hour
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulses)(* 1000) / (hour)
	instrDiv2byConst, idxSecondsPerHour,				// term is now (VSS pulses)(* 1000) / (second)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpFEvsSpeedQuantumIdx,		// store speed quantum in (VSS pulses)(* 1000) / (second)

#endif // useBarFuelEconVsSpeed
#ifdef useCPUreading
	instrLdRegByte, 0x02, 0,
	instrStRegMain, 0x02, mpMainLoopAccumulatorIdx,		// initialize the cpu utilization stopwatch timer values
	instrStRegMain, 0x02, mpIdleAccumulatorIdx,
#ifdef useDebugCPUreading
	instrStRegVolatile, 0x02, vInterruptAccumulatorIdx,
	instrStRegMain, 0x02, mpDebugAccMainLoopIdx,
	instrStRegMain, 0x02, mpDebugAccIdleIdx,
	instrStRegMain, 0x02, mpDebugAccIdleProcessIdx,
	instrStRegMain, 0x02, mpDebugAccInterruptIdx,
	instrStRegMain, 0x02, mpDebugAccDisplayIdx,
	instrStRegMain, 0x02, mpDebugAccSWEET64idx,
	instrStRegMain, 0x02, mpDebugAccS64multIdx,
	instrStRegMain, 0x02, mpDebugCountS64multIdx,
	instrStRegMain, 0x02, mpDebugAccS64divIdx,
	instrStRegMain, 0x02, mpDebugCountS64divIdx,
#endif // useDebugCPUreading

#endif	// useCPUreading
	instrDone											// exit to caller
};

static uint8_t EEPROM::powerUpCheck(void)
{

	uint8_t b;

	b = SWEET64::runPrgm(prgmInitEEPROM, 0); // perform EEPROM initialization if required, and cause MPGuino initialization when done

#ifdef useScreenEditor
	if (b)
	{

		uint8_t t = eePtrScreensStart;
		for (uint8_t x = 0; x < mainScreenDisplayFormatSize; x++) writeVal(t++, (unsigned long)(pgm_read_byte(&mainScreenDisplayFormats[(unsigned int)(x)])));

	}

#endif // useScreenEditor
	initGuino();

	return b;

}

static void EEPROM::initGuinoHardware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega32U4__)
	EIMSK &= ~((1 << INT3) | (1 << INT2)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC31) | (1 << ISC30) | (1 << ISC21) | (1 << ISC20)); // set injector sense pin control
	EICRA &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC30 : ISC20));

	EIFR |= ((1 << INTF3) | (1 << INTF2)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT3) | (1 << INT2)); // enable fuel injector sense interrupts

	PCMSK0 |= (1 << PCINT7); // enable port B VSS pin interrupt

	PCICR |= (1 << PCIE0); // enable selected interrupts on port B

	lastPINxState = PINB;

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	EIMSK &= ~((1 << INT5) | (1 << INT4)); // disable fuel injector sense interrupts

	EICRB |= ((1 << ISC51) | (1 << ISC50) | (1 << ISC41) | (1 << ISC40)); // set injector sense pin control
	EICRB &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC50 : ISC40));

	EIFR |= ((1 << INTF5) | (1 << INTF4)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT5) | (1 << INT4)); // enable fuel injector sense interrupts

	DIDR2 &= ~(1 << ADC8D); // enable digital input on VSS pin

	PCMSK2 |= (1 << PCINT16); // enable port K VSS pin interrupt

	PCICR |= (1 << PCIE2); // enable selected interrupts on port K

	lastPINxState = PINK;

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	EIMSK &= ~((1 << INT1) | (1 << INT0)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00)); // set injector sense pin control
	EICRA &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC10 : ISC00));

	EIFR |= ((1 << INTF1) | (1 << INTF0)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT1) | (1 << INT0)); // enable fuel injector sense interrupts

	DIDR0 &= ~(1 << ADC0D); // enable digital input on VSS pin

	PCMSK1 |= (1 << PCINT8); // enable port C VSS pin interrupt

	PCICR |= (1 << PCIE1); // enable selected interrupts on port C

	lastPINxState = PINC;

#endif // defined(__AVR_ATmega328P__)
#if defined(useAnalogRead)
#if defined(__AVR_ATmega32U4__)
	PRR0 &= ~(1 << PRADC); // turn on ADC module
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRADC); // turn on ADC module
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRADC); // turn on ADC module
#endif // defined(__AVR_ATmega328P__)

	// disable ADC free-running mode
	ADCSRA &= ~(1 << ADATE); // the AVR hardware just ignores this bit, and always goes to free running mode

	// enable ADC
	ADCSRA |= (1 << ADEN);

	// set ADMUX[5] bit to 0
	ADCSRB = 0;

	// set ADC timer frequency to 1/128 of system timer
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

#endif // useAnalogRead
	VSSpause = readVal(pVSSpauseIdx);

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::initGuinoSoftware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (readVal(pMetricModeIdx)) metricFlag |= metricMode;
	else metricFlag &= ~(metricMode);

	if (readVal(pAlternateFEidx)) metricFlag |= alternateFEmode;
	else metricFlag &= ~(alternateFEmode);

	SWEET64::runPrgm(prgmInitMPGuino, 0); // calculate multiple MPGuino system values for use within code

#ifdef useBarFuelEconVsTime
	timer0Command |= (t0cResetFEvTime); // reset fuel economy vs time bargraph mechanism

#endif // useBarFuelEconVsTime
	SREG = oldSREG; // restore interrupt flag status

#ifdef useBarFuelEconVsSpeed
	bgFEvsSsupport::reset();

#endif // useBarFuelEconVsSpeed
#ifdef useWindowTripFilter
	windowTripFilterReset();

#endif // useWindowTripFilter
}

static void EEPROM::initGuino(void) // initialize MPGuino base hardware and basic system settings
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	initGuinoHardware();
	initGuinoSoftware();

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::writeVal(uint8_t eePtr, uint32_t value)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	SWEET64::init64(vee, value);

	write64(vee, eePtr);

}

static uint8_t EEPROM::readVal(uint8_t eePtr)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	read64(vee, eePtr);

	return vee->u8[0];

}

static void EEPROM::read64(union union_64 * an, uint8_t parameterIdx)
{

	uint8_t oldSREG;
	uint16_t t;
	uint16_t u;

	t = getAddress(parameterIdx);
	u = getAddress(++parameterIdx);

	SWEET64::init64byt(an, 0);

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	for (uint16_t x = t; x < u; x++) an->u8[x - t] = eeprom_read_byte((uint8_t *)(x));

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::write64(union union_64 * an, uint8_t parameterIdx)
{

	uint8_t oldSREG;
	uint16_t t;
	uint16_t u;
	uint8_t l;
	uint8_t b;
	uint8_t eByt;
	uint8_t rByt;

	l = getParameterFlags(parameterIdx);
	t = getAddress(parameterIdx);
	u = getAddress(++parameterIdx);

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	b = 0;

	for (uint16_t x = t; x < u; x++)
	{

		eByt = eeprom_read_byte((uint8_t *)(x));
		rByt = an->u8[x - t];
		if (eByt != rByt)
		{

			metricFlag |= (EEPROMbulkChangeFlag);
			b = 1;
			eeprom_write_byte((uint8_t *)(x), rByt);

		}

	}

	if (b)
	{

		switch (l & 0xE0)
		{

			case pfSoftwareInitMPGuino:
				metricFlag |= (softInitGuinoFlag);
				break;

			case pfHardwareInitMPGuino:
				metricFlag |= (hardInitGuinoFlag);
				break;

			case pfDoMetricModeConversion:
				metricFlag |= (metricConversionFlag);
				break;

			case pfChangeDisplay:
				metricFlag |= (changeDisplayFlag);
				break;

			case pfCalculateFuelParams:
				metricFlag |= (calculateFuelParamFlag);
				break;

		}

	}

	SREG = oldSREG; // restore interrupt flag status

}

static uint16_t EEPROM::getAddress(uint8_t eePtr)
{

	uint16_t t;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) t = (uint16_t)(pgm_read_byte(&paramAddrs[eePtr]));
#ifdef useScreenEditor
	else if ((eePtr >= eePtrScreensStart) && (eePtr < eePtrScreensEnd)) t = 2 * (eePtr - eePtrScreensStart) + eeAdrScreensStart;
#endif // useScreenEditor
	else t = eeAdrStorageEnd;

	return t;

}

static uint8_t EEPROM::getParameterFlags(uint8_t eePtr)
{

	uint8_t t;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) t = pgm_read_byte(&paramsLength[(uint16_t)(eePtr)]) & 0xF8;
	else t = 0;

	return t;

}

static uint8_t EEPROM::getLength(uint8_t eePtr)
{

	uint8_t t;
	uint8_t u;
	uint8_t l;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) l = pgm_read_byte(&paramsLength[(uint16_t)(eePtr)]) & 0x07;
	else l = 0;

	t = getAddress(eePtr);
	u = getAddress(++eePtr);
	u -= t;
	if (l) u--;
	l += (u << 3);

	return l;

}

#ifdef useWindowTripFilter
void windowTripFilterReset(void)
{

	wtpCurrentIdx = windowTripFilterIdx;

	for (uint8_t x = 0; x < windowTripFilterSize; x++) tripVar::reset(windowTripFilterIdx + x);

}

#endif // useWindowTripFilter
#ifdef useJSONoutput
const char JSONaccelTestStateMsgs[] PROGMEM = {
	"\0"
	"Drag Ready\0"
	"Testing\0"
	"Drag Finished\0"
	"Drag Cancelled\0"
	"Distance reached\0"
	"Half-speed reached\0"
	"Half-speed and distance reached\0"
	"Full speed reached\0"
	"Full speed and distance reached\0"
	"Full speed and half-speed reached\0"
	"Full speed, half-speed, and distance reached\0"
	"Drag Test Failure\0"
};

static const uint8_t prgmJSON23tankFE[] PROGMEM = {
	instrMul2byByte, 2,									// multiply results by a 2
	instrDiv2byByte, 3,									// divide results by a 3
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmJSON53tankFE[] PROGMEM = {
	instrMul2byByte, 5,									// multiply results by a 5
	instrDiv2byByte, 3,									// divide results by a 3
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmJSONminValue[] PROGMEM = {
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if register 2 contents are less than or equal to register 1 contents, skip
	instrLdReg, 0x12,									// load register 2 with contents of register 1

//ret:
	instrDone											// exit to caller
};

void doOutputJSONnumber(const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::runPrgm(sched, tripIdx);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::doCalculate(tripIdx, calcIdx);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(unsigned long an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::init64((union union_64 *)(&s64reg[s64reg2]), an);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(union union_64 * an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::copy64((union union_64 *)(&s64reg[s64reg2]), an);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(uint8_t decimalPlaces, const char * labelStr)
{

	text::stringOut(devLogOutput, formatDecimal(mBuff1, 0, decimalPlaces, dfOverflow9s)); // output the number part
	text::stringOut(devLogOutput, labelStr); // output the label part

}

void doOutputJSON(void) //skybolt added json output function
{

	static uint8_t subtitleCount1 = 2;
#ifdef useDragRaceFunction
	static uint8_t subtitleCount2 = 3;
#endif // useDragRaceFunction

	uint8_t oldSREG;

	uint64_t instFuelEcon;
	uint64_t tankFuelEcon;
	uint64_t tankFuelEcon53;

	if (timer0Status & t0sOutputJSON) // replaced timerChecker with this because systemInfo::cycles0() is a compile-time option that could break JSON output if #useCPUreading is not set
	{

		peripheral::changeBitFlags(timer0Status, t0sOutputJSON, 0); // clear JSON subtitle change timer command

		if (!(--subtitleCount1)) subtitleCount1 = 2;
#ifdef useDragRaceFunction
		if (!(--subtitleCount2)) subtitleCount2 = 3;
#endif // useDragRaceFunction

	}

	unsigned long reserveQuantity = SWEET64::runPrgm(prgmReserveRemainingFuel, 0);

	unsigned long tankSize = SWEET64::runPrgm(prgmFetchParameterValue, pTankSizeIdx);

	text::stringOut(devLogOutput, PSTR("\n\n[")); // begin JSON payload

	// first graph, fuel
	text::stringOut(devLogOutput, PSTR("{\"title\":\""));

	if (reserveQuantity == 0) text::stringOut(devLogOutput, PSTR("fumes remaining, ")); // fumes left
	else doOutputJSONnumber(prgmReserveRemainingFuel, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " remaining, ")); // reserve remaining fuel left

	doOutputJSONnumber(prgmBingoFuelRemaining, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " e-reserve\",\n")); // bingo remaining fuel left
	//  doOutputJSONnumber(instantIdx, tEngineSpeed, 0, PSTR(" RPM\",\n")); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"fuel used: "));

			doOutputJSONnumber(currentIdx, tFuelUsed, 3, PSTR(" trip " "\xEB" "gal" "\xEC" "L" "\xED" "; ")); // current trip fuel used
			doOutputJSONnumber(tankIdx, tFuelUsed, 3, PSTR(" tank " "\xEB" "gal" "\xEC" "L" "\xED" "; ")); // tank trip fuel used
			doOutputJSONnumber(instantIdx, tFuelRate, 3, PSTR(" using " "\xEB" "gal" "\xEC" "L" "\xED" "/hr\",\n")); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"eco stats: "));
#ifdef trackIdleEOCdata
			doOutputJSONnumber(eocIdleTankIdx, tFuelUsed, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " used@idle, "));
			doOutputJSONnumber(eocIdleTankIdx, tDistance, 1, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " fuel cut\",\n"));
#else // trackIdleEOCdata
			if (reserveQuantity == 0) text::stringOut(devLogOutput, PSTR("fumes remaining, ")); // fumes left
			else doOutputJSONnumber(prgmReserveRemainingFuel, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " remaining, ")); // reserve remaining fuel left
			doOutputJSONnumber(prgmBingoFuelRemaining, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " e-reserve\",\n")); // bingo remaining fuel left
#endif // trackIdleEOCdata

			break;

	}

	reserveQuantity = SWEET64::runPrgm(prgmFetchParameterValue, pTankBingoSizeIdx);

	text::stringOut(devLogOutput, PSTR("\"ranges\":[")); //ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	doOutputJSONnumber(tankSize, 3, PSTR(",")); // largest, full tank size (e.g, 13.8 g)
	doOutputJSONnumber(tankSize - reserveQuantity, 3, PSTR(",")); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	doOutputJSONnumber(reserveQuantity, 3, PSTR("],\n")); // reserve amount (e.g. 2.2g)

	text::stringOut(devLogOutput, PSTR("\"measures\":["));
	doOutputJSONnumber(prgmReserveRemainingFuel, 0, 3, PSTR(",")); // reserve remaining fuel left
	doOutputJSONnumber(prgmRemainingFuel, 0, 3, PSTR("],\n")); // total remaining fuel left

	text::stringOut(devLogOutput, PSTR("\"markers\":["));
	doOutputJSONnumber(instantIdx, tFuelRate, 3, PSTR("]},\n\n")); // current rate of fuel burn in units/time

	// second graph, distance
	text::stringOut(devLogOutput, PSTR("{\"title\":\""));
	doOutputJSONnumber(prgmReserveDistanceToEmpty, tankIdx, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" " to e, ")); // distance to bingo
	doOutputJSONnumber(prgmBingoDistanceToEmpty, tankIdx, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " e-reserve\",\n")); // distance to fully empty tank from bingo

	switch (subtitleCount1)
	{

		case 2:
			//   text::stringOut(devLogOutput, PSTR("\"subtitle\":\"trip/tank distance: "));
			//   doOutputJSONnumber(currentIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" "/")); // current trip distance
			//   doOutputJSONnumber(tankIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" "\",\n")); // current trip distance

			text::stringOut(devLogOutput, PSTR("\"subtitle\":\""));
			doOutputJSONnumber(currentIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " trip distance, ")); // current trip distance
			doOutputJSONnumber(tankIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " tank distance\",\n")); // current trip distance

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\""));
			doOutputJSONnumber(prgmReserveDistance, tankIdx, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " safe range, ")); // reserve range
			doOutputJSONnumber(prgmRangeDistance, tankIdx, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " dry range\",\n")); // distance to fully empty tank

			break;

	}

	text::stringOut(devLogOutput, PSTR("\"ranges\":["));
	doOutputJSONnumber(prgmRangeDistance, tankIdx, 1, PSTR(",")); // maximum range
	doOutputJSONnumber(prgmReserveDistance, tankIdx, 1, PSTR(",")); // range 2, safe range
	doOutputJSONnumber(prgmFindHalfReserveRange, tankIdx, 1, PSTR("],\n")); // range 3, half of safe range

	text::stringOut(devLogOutput, PSTR("\"measures\":["));
	doOutputJSONnumber(prgmBingoDistance, tankIdx, 2, PSTR("],\n")); // shows miles of e-reserve in bar form

	text::stringOut(devLogOutput, PSTR("\"markers\":["));
	doOutputJSONnumber(tankIdx, tDistanceToEmpty, 2, PSTR("]},\n\n")); // line is distance to empty

	// third graph, econ
	text::stringOut(devLogOutput, PSTR("{\"title\":\"")); //\xeb gal \xec L \xed/

#ifdef useDragRaceFunction
	// there was a bug in some of the drag cancel logic when using Adafruit. I disabled some statements
	// to get it to work. (Was tripping at 0.718 or 0.0718 seconds (don't remember exact digits)
	//
	// yah, the fuel injector monitor was causing the accel test to cancel prematurely. That no longer happens
	//
	//some kind of bit mask; full and half-speed seem to be backwards (0 means tripped)
	//unclear on syntax for building if/then statements
	//accelerationFlags & accelTestTriggered
	// 120, waiting
	// 184, testing.
	// 176, distance met.
	// 160, distance and half-speed met. (Full speed then meets all conditions, test ends)
	// 168 half-speed met. (not distance)
	// 136 full-speed met. (Meeting distance then meets all three conditions, race ends).
	// what the hell is 156? Did this actually come up? (it appears to be active | halfspeed | cancelled | finished)
	// lastAccelTestStatus = 120; //use to force trigger

	if (accelTestState) //display if we have encountered a state change in the drag test
	{

		text::stringOut(devLogOutput, findStr(JSONaccelTestStateMsgs, accelTestState));
		text::stringOut(devLogOutput, PSTR(" ...\",\n"));

	}
	else // else not racing or waiting, go to normal
	{

		text::stringOut(devLogOutput, PSTR("trip/tank/inst: ")); //\xebgal\xecL\xed/
		doOutputJSONnumber(currentIdx, tFuelEcon, 3, PSTR("/ ")); // current fuel economy
		doOutputJSONnumber(tankIdx, tFuelEcon, 3, PSTR("/ ")); // tank fuel economy
		reserveQuantity = SWEET64::doCalculate(instantIdx, tFuelEcon);
		if (reserveQuantity > 9999999) text::stringOut(devLogOutput, PSTR("infinite\",\n"));
		else doOutputJSONnumber(instantIdx, tFuelEcon, 3, PSTR("\",\n")); // instantaneous fuel economy

	}

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"accel time: 0-"));
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx) / 2, 0, PSTR("/"));
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx), 0, PSTR(": "));
			doOutputJSONnumber(dragHalfSpeedIdx, tAccelTestTime, 2, PSTR("/")); // 0-30 time
			doOutputJSONnumber(dragFullSpeedIdx, tAccelTestTime, 2, PSTR(", ")); // 0-60 time
			doOutputJSONnumber(dragDistanceIdx, tDistance * 1, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" " in ")); // trap distance
			doOutputJSONnumber(dragDistanceIdx, tAccelTestTime, 2, PSTR(" @")); // trap time
			doOutputJSONnumber(dragDistanceIdx, tSpeed, 3, PSTR("\xEB" "mph" "\xEC" "kph" "\xED" "; ")); // trap speed
			doOutputJSONnumber(dragHalfSpeedIdx, tEstimatedEnginePower, 0, PSTR("\xeB" "hp" "\xEC" "kW" "\xED" " @")); // estimated engine power
			doOutputJSONnumber(dragHalfSpeedIdx, tDragSpeed, 0, PSTR("\xEB" "mph" "\xEC" "kph" "\xED" "\",\n")); // max speed

			break;

		case 2:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"accel fuel: "));

			doOutputJSONnumber(dragHalfSpeedIdx, tFuelUsed, 3, PSTR("\xEB" "ga" "\xEC" "L" "\xED" " to ")); // 0-half fuel
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx) / 2, 0, PSTR(", ")); // 0-half speed
			doOutputJSONnumber(dragFullSpeedIdx, tFuelUsed, 3, PSTR("\xEB" "ga" "\xEC" "L" "\xED" " to ")); // 0-full fuel
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx), 0, PSTR(", ")); // 0-full speed
			doOutputJSONnumber(dragDistanceIdx, tFuelUsed, 3, PSTR("\xEB" "ga" "\xEC" "L" "\xED" " to ")); //trap fuel
			doOutputJSONnumber(dragDistanceIdx,  tDistance * 1, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" "\",\n")); // "to [trap distance]"  \xebft\xecm\xed;

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"accel fuel: "));

			doOutputJSONnumber(dragHalfSpeedIdx, tFuelEcon, 3, PSTR("\xEB" "mpg" "\xEC" "L100" "\xED" " to ")); // 0-30 mpg
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx) / 2, 0, PSTR(", ")); // 0-half speed
			doOutputJSONnumber(dragFullSpeedIdx, tFuelEcon, 3, PSTR("\xEB" "mpg" "\xEC" "L100" "\xED" " to ")); // 0-60 mpg
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx), 0, PSTR(", ")); // 0-60 speed
			doOutputJSONnumber(dragDistanceIdx, tFuelEcon, 3, PSTR("\xEB" "mpg" "\xEC" "L100" "\xED" " to ")); //trap mpg
			doOutputJSONnumber(dragDistanceIdx,  tDistance * 1, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" "\",\n")); // "to [trap distance]"  \xebft\xecm\xed;

			break;

	}

#else // useDragRaceFunction
	text::stringOut(devLogOutput, PSTR("\"subtitle\":\"[this space intentionally left blank]\",\n"));

#endif // useDragRaceFunction
	text::stringOut(devLogOutput, PSTR("\"ranges\":[18,24,"));

	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	doOutputJSONnumber(min(max(40000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 999000), 3, PSTR("],\n"));

	text::stringOut(devLogOutput, PSTR("\"measures\":["));
	doOutputJSONnumber(currentIdx, tFuelEcon, 3, PSTR(",")); // current fuel economy
	doOutputJSONnumber(tankIdx, tFuelEcon, 3, PSTR("],\n")); // tank fuel economy

	text::stringOut(devLogOutput, PSTR("\"markers\":["));

	// instantaneous fuel economy, do not let scale exceed 999
	doOutputJSONnumber(min(999000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 3, PSTR("]"));

	text::stringOut(devLogOutput, PSTR("\}]\n\r")); // end JSON payload, and go trigger read on python.script

} // end sendJson function

#endif // useJSONoutput
/* EEPROM parameter number editor section */

static const uint8_t prgmFetchMaximumParamValue[] PROGMEM = {
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrLxdIParamLengthIndexed,						// load parameter length into index

//loop:
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead
	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

//cont:
	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrSubYfromX, 0x12,								// subtract 1 from register 2 to obtain final maximum parameter value
	instrDone											// return to caller
};

static const uint8_t prgmCompareWithMaximumParamValue[] PROGMEM = {
	instrLdReg, 0x23,									// save converted working number for comparison
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrLxdIParamLengthIndexed,						// load parameter length into index

//loop:
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead
	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

//cont:
	instrCmpXtoY, 0x23,									// compare working number to maximum parameter value
	instrBranchIfLTorE, 4,								// if maximum parameter value <= working number, signal "overflow"
	instrLdRegByte, 0x02, 0,							// signal that working number is good
	instrDone,											// return to caller

//badret:
	instrLdRegByte, 0x02, 255,							// signal that working number is bad
	instrDone											// return to caller
};

static const uint8_t prgmDoEEPROMmetricConversion[] PROGMEM = {
	instrLxdI, 0,										// zero the trip index register

//loop:
	instrLdRegEEPROMindirect, 0x02,						// load an EEPROM parameter, indexed into the convIdx array
	instrLdRegNumer, 0x01,								// load numerator into register 1
	instrMul2by1,										// multiply EEPROM parameter value by numerator
	instrLdRegDenom, 0x01,								// load denominator into register 1
	instrDiv2by1,										// divide EEPROM parameter value by denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegEEPROMindirect, 0x02,						// store converted EEPROM parameter
	instrAddIndex, 1,									// bump conversion index stored in trip index register
	instrCmpIndex, convSize,							// finished processing list of EEPROM parameters?
	instrBranchIfLT, 239,								// if not, loop back
	instrDone											// return to caller
};

#ifdef useCalculatedFuelFactor
static const uint8_t prgmCalculateFuelFactor[] PROGMEM = {
	instrLdRegConst, 0x02, idxCorrectionFactor,			// obtain reference correction factor
	instrMul2byEEPROM, pSysFuelPressureIdx,				// multiply by this vehicle's stored fuel system absolute pressure
	instrDiv2byEEPROM, pRefFuelPressureIdx,				// divide by the fuel injector reference absolute pressure
	instrIsqrt, 0x02,									// perform square root on result
	instrMul2byEEPROM, pInjectorCountIdx,				// multiply by stored number of fuel injectors for this engine
	instrMul2byEEPROM, pInjectorSizeIdx,				// multiply by injector size in cc/minute * decimal formatting factor (L/min * decimal formatting factor * 1000)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegByte, 0x02, 60,							// load seconds per minute into register 2
	instrMul2byConst, idxMicroSecondsPerSecond,			// multiply by microseconds per second into register 1
	instrMul2byConst, idxOneThousand,					// multiply by number of cc's per liter into register 1
	instrMul2byConst, idxDecimalPoint,					// set numerator up to cancel reference correction factor in denominator
	instrMul2byConst, idxCorrectionFactor,				// set numerator up to cancel reference correction factor in denominator
	instrDiv2by1,										// perform comversion for injector flow
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byConst, idxNumerVolume,					// set up denominator to convert injector flow in liters to US gallons
	instrDiv2byConst, idxDenomVolume,					// perform conversion of injector flow in liters to US gallons
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,
	instrDone											// return to caller
};

#endif // useCalculatedFuelFactor
#ifdef usePartialRefuel
static const uint8_t prgmAddToPartialRefuel[] PROGMEM = {
	instrAddEEPROMtoX, 0x02, pRefuelSizeIdx,			// add existing partial refuel size parameter to what's in the result register
	instrDone											// return to caller
};

#endif // usePartialRefuel
static const char numberEditSave[] PROGMEM = {
	" OK"
};

static const char numberEditCancel[] PROGMEM = {
	" XX"
};

static uint8_t parameterEdit::sharedFunctionCall(uint8_t cmd)
{

	uint8_t retVal = 0;
	uint8_t parameterPtr = numberEditObj.parameterIdx;
	uint8_t cp = parameterPtr - eePtrSettingsStart;

	switch (cmd)
	{

		case nesLoadInitial:
			SWEET64::runPrgm(prgmFetchParameterValue, parameterPtr);

		case nesLoadValue:
			ull2str(tFormatToNumber, pBuff, 3);
			parameterEdit::findLeft();

		case nesOnChange:
#ifdef useLegacyLCD
			if (parameterPtr == pContrastIdx) LCD::setContrast((uint8_t)(str2ull(pBuff))); // adjust contrast dynamically
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
			if (parameterPtr == pLCDcolorIdx) LCD::setRGBcolor((uint8_t)(str2ull(pBuff))); // adjust backlight color dynamically
#endif // useAdafruitRGBLCDshield
			break;

		case nesSaveParameter:
			retVal = onEEPROMchange(prgmWriteParameterValue, parameterPtr);
			break;

		case nesOutputUpperScreen:
			text::stringOut(devLCD, parmLabels, cp + 1); // print parameter name at top left
			text::gotoXY(devLCD, 0, 1); // go to next line
			break;

		default:
			break;

	}

	return retVal;

}

static uint8_t parameterEdit::onEEPROMchange(const uint8_t * sched, uint8_t parameterIdx)
{

	uint8_t retVal;

	retVal = 0;

	metricFlag &= ~(detectEEPROMchangeFlag);

	SWEET64::runPrgm(sched, parameterIdx);

#ifdef useCalculatedFuelFactor
	if (metricFlag & calculateFuelParamFlag) SWEET64::runPrgm(prgmCalculateFuelFactor, 0); // calculate and store microseconds per US gallon factor

#endif // useCalculatedFuelFactor
	if (metricFlag & metricConversionFlag) SWEET64::runPrgm(prgmDoEEPROMmetricConversion, 0); // if metric flag has changed

	if (metricFlag & changeDisplayFlag)
	{

#ifdef useLegacyLCD
		if (parameterIdx == pContrastIdx) LCD::setContrast(EEPROM::readVal(pContrastIdx)); // adjust contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
		if ((parameterIdx == pLCDcolorIdx) && (brightnessIdx)) LCD::setRGBcolor(EEPROM::readVal(pLCDcolorIdx)); // adjust backlight color
#endif // useAdafruitRGBLCDshield

	}

	if (metricFlag & hardInitGuinoFlag) EEPROM::initGuinoHardware();

	if (metricFlag & softInitGuinoFlag) EEPROM::initGuinoSoftware();

	if (metricFlag & detectEEPROMchangeFlag) retVal = 1; // if the setting has changed

	return retVal;

}

static uint8_t parameterEdit::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t j;
	uint8_t k;
	uint8_t c;
	uint8_t blinkFlag;
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			findLeft();

		case menuCursorUpdateIdx:
			k = ' '; // initially a leading space
			for (uint8_t x = 0; x < 10; x++) // scan all of numeric buffer from left to right
			{

				c = pBuff[(unsigned int)(x)]; // fetch a character from buffer

				if (x > cursorPos) // if current position is past cursor position, just make sure leading spaces get turned into zero digits
				{

					if (c == ' ') pBuff[(unsigned int)(x)] = '0';

				}
				else // if current position is before or at cursor position, ensure leading spaces up to either cursor position or to first non-zero digit
				{

					if ((c == ' ') || (c == '0')) pBuff[(unsigned int)(x)] = k; // if character is either a space or a zero digit, it may be leading so change it
					else k = '0'; // character is a non-zero digit, so change leading character value to a zero digit

				}

			}

			if (pBuff[9] == ' ') pBuff[9] = '0'; // ensure at least one digit

		case menuOutputDisplayIdx:
			blinkFlag = (timer0Status & t0sShowCursor);
			sharedFunctionCall(nesOutputUpperScreen); // print initial screen information and position cursor

			uint8_t c = pBuff[(unsigned int)(cursorPos)]; // save existing character
			if ((cursorPos < 10) && (blinkFlag)) pBuff[(unsigned int)(cursorPos)] = '_'; // replace character with an underscore
			text::stringOut(devLCD, pBuff); // print number
			pBuff[(unsigned int)(cursorPos)] = c; // restore existing character

			text::setModeOnCondition(devLCD, ((cursorPos == 10) && (blinkFlag)), odvFlagShootBlanks);
			text::stringOut(devLCD, numberEditSave);
			text::setModeOnCondition(devLCD, ((cursorPos == 11) && (blinkFlag)), odvFlagShootBlanks);
			text::stringOut(devLCD, numberEditCancel);
			text::setModeOnCondition(devLCD, 0, odvFlagShootBlanks);
			break;

		default:
			break;

	}

	return retVal;

}

static void parameterEdit::entry(void)
{

	numberEditObj.callingScreenLevel = menuLevel;

	cursor::moveAbsolute(parameterEditScreenIdx, 255);

}

static void parameterEdit::findLeft(void)
{

	screenCursor[(unsigned int)(parameterEditScreenIdx)] = 9;

	// do a nice thing and put the edit cursor at the first non zero number
	for (uint8_t x = 9; x < 10; x--) if (pBuff[(unsigned int)(x)] != ' ') screenCursor[(unsigned int)(parameterEditScreenIdx)] = x;

}

static void parameterEdit::findRight(void)
{

	screenCursor[(unsigned int)(parameterEditScreenIdx)] = 9;

}

static void parameterEdit::readInitial(void)
{

	SWEET64::runPrgm(prgmFetchInitialParamValue, numberEditObj.parameterIdx);
	sharedFunctionCall(nesLoadValue);

}

static void parameterEdit::readMaxValue(void)
{

	SWEET64::runPrgm(prgmFetchMaximumParamValue, numberEditObj.parameterIdx);
	sharedFunctionCall(nesLoadValue);

}

static void parameterEdit::readMinValue(void)
{

	SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0);
	sharedFunctionCall(nesLoadValue);

}

static void parameterEdit::changeDigitUp(void)
{

	changeDigit(1);

}

static void parameterEdit::changeDigitDown(void)
{

	changeDigit(255);

}

static void parameterEdit::changeDigit(uint8_t digitDir)
{

	uint8_t cp = screenCursor[(unsigned int)(parameterEditScreenIdx)];
	uint8_t c = '0';
	uint8_t d = ' ';

	uint8_t w;

	switch (cp)
	{
		case 10:
			save();
			break;

		case 11:
			cancel();
			break;

		default:
			w = pBuff[(unsigned int)(cp)]; // fetch digit from stored numeric string representing parameter to be changed

			if (w == ' ') w = '0'; // if this is a leading space, use 0 as working digit
			w += digitDir; // adjust working digit
			if (w > '9') w = '0'; // handle working digit rollover
			if (w < '0') w = '9'; // handle working digit rollover

			pBuff[(unsigned int)(cp)] = w;

			str2ull(pBuff); // convert parameter buffer string into uint64_t
			w = (uint8_t)(SWEET64::runPrgm(prgmCompareWithMaximumParamValue, numberEditObj.parameterIdx));

			if (w) switch (digitDir)
			{

				case 1:
					readMinValue();
					break;

				case 255:
					readMaxValue();
					break;

				default:
					break;

			}

			sharedFunctionCall(nesOnChange);

			break;

	}

}

static void parameterEdit::save(void)
{

	uint8_t cp = screenCursor[(unsigned int)(parameterEditScreenIdx)];
	uint8_t retVal;

	switch (cp)
	{

		case 10:
			str2ull(pBuff); // convert parameter string to a number
#ifdef usePartialRefuel
			if (numberEditObj.parameterIdx == pRefuelSizeIdx) SWEET64::runPrgm(prgmAddToPartialRefuel, 0);
#endif // usePartialRefuel
			retVal = sharedFunctionCall(nesSaveParameter); // go save parameter and do any required housekeeping

			cursor::screenLevelEntry(numberEditObj.neStatusMessage, retVal, numberEditObj.callingScreenLevel);
			break;

		case 11:
			cancel();
			break;

		default:
			cp = 10;
			screenCursor[(unsigned int)(parameterEditScreenIdx)] = cp;
			displayHandler(menuCursorUpdateIdx, cp, 1);
			break;

	}

}

static void parameterEdit::cancel(void)
{

	uint8_t cp = screenCursor[(unsigned int)(parameterEditScreenIdx)];
	const char * str;

	if (cp != 11)
	{

		cp = 11;
		screenCursor[(unsigned int)(parameterEditScreenIdx)] = cp;
		displayHandler(menuCursorUpdateIdx, cp, 1);

	}
	else
	{

		cursor::screenLevelEntry(numberEditObj.neStatusMessage, 2, numberEditObj.callingScreenLevel);

	}

}

/* MPGuino parameter settings edit section */

static const char pseStatusMessages[] PROGMEM = {
	"Param Unchanged\r"
	"Param Changed\r"
	"Param Reverted\r"
};

static uint8_t settings::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			numberEditObj.neStatusMessage = pseStatusMessages;

		case menuCursorUpdateIdx:
			switch (menuLevel)
			{

				case settingScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsDispStart;
					break;

				case fuelSettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsInjStart;
					break;

				case VSSsettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsVSSstart;
					break;

				case tankSettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsTankStart;
					break;

#ifdef useChryslerMAPCorrection
				case CRFICsettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsCRFICstart;
					break;

#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
				case acdSettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsACDstart;
					break;

#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
				case timeoutSettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsTimeoutStart;
					break;

				case miscSettingsScreenIdx:
					numberEditObj.parameterIdx = cursorPos + eePtrSettingsMiscStart;
					break;

				default:
					break;

			}
			parameterEdit::sharedFunctionCall(nesLoadInitial);

		case menuOutputDisplayIdx:
			parameterEdit::sharedFunctionCall(nesOutputUpperScreen); // print initial screen information and position cursor
			text::stringOut(devLCD, pBuff);
			text::newLine(devLCD);
			break;

		default:
			break;

	}

	return retVal;

}

/* human interface I/O peripheral device support section */

#ifdef useExpansionPins
/*
	Modes
	0	Off
	1	instant fuel economy < current fuel economy (0 - false, 255 - true)
	2	instant fuel economy > current fuel economy (0 - false, 255 - true)
	3	instant fuel economy < tank fuel economy (0 - false, 255 - true)
	4	instant fuel economy > tank fuel economy (0 - false, 255 - true)
	5	estimated tank fuel consumed (0 (none) to 255 (all of the tank), based on tank size)
	6	estimated tank fuel remaining (0 (empty) to 255 (full), based on tank size)
*/
static const uint8_t prgmCalculateOutputPinValue[] PROGMEM = {
	instrCmpIndex, 2,									// is a valid expansion output pin number being requested?
	instrBranchIfGTorE, 13,								// skip to output a zero if not
	instrLxdIEEPROMoffset, pExpansionPin1Mode,			// load the indexed stored parameter index for the expansion output pin setting
	instrTestIndex,										// test pin mode value for zero
	instrBranchIfZero, 8,								// exit out if pin mode is zero
	instrCmpIndex, 4,									// test if pin mode is "fuel economy comparison between instant and whatever"
	instrBranchIfLTorE,	26,								// if so, skip ahead
	instrCmpIndex, 6,									// test if pin mode is analog output tank quantity or quantity remaining
	instrBranchIfLTorE, 4,								// if so, skip ahead

//zeroOutRet:
	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone,											// exit to caller

//analogOut:
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrCmpIndex, 6,									// outputting remaining value?
	instrBranchIfE, 7,									// go do conversion if so
	instrLdReg, 0x21,									// shift contents to register 1
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubYfromX, 0x12,								// subtract remaining fuel value from tank size

//analogOut2:
	instrMul2byByte, 255,								// multiply tank quantity by 255
	instrDiv2byMain, mpTankSizeIdx,						// divide by calculated tank size
	instrDone,											// return to caller

// US (inst FE > accumulated FE) = /(inst FE <= accumulated FE)
//
//  (inst distance)      (accum distance)
//  ---------------  <=  ----------------  >>>  (inst distance) * (accum quantity)  <=  (inst quantity) * (accum distance)
//  (inst quantity)      (accum quantity)
//
//
//
// metric (inst FE > accumulated FE) = /(inst FE <= accumulated FE)
//
//  (inst quantity)      (accum quantity)
//  ---------------  =>  ----------------  >>>  (inst quantity) * (accum distance)  =>  (inst distance) * (accum quantity)
//  (inst distance)      (accum distance)                                           or
//                                              (inst distance) * (accum quantity)  <=  (inst quantity) * (accum distance)
//
// therefore, for (inst FE > accumulated FE) comparison, there is no need to check whether MPGuino is in metric mode or not
// (inst FE < accumulated FE) is treated in a similar way, but the left and right terms are swapped around
//
	instrCmpIndex, 2,									// comparing instant to current?
	instrBranchIfLTorE, 10,								// if so, skip ahead
	instrLdRegTripVar, 0x03, tankIdx, rvInjCycleIdx,	// fetch tank (accum quantity)
	instrLdRegTripVar, 0x02, tankIdx, rvVSSpulseIdx,	// fetch tank (accum distance)
	instrSkip, 8,										// skip ahead
	instrLdRegTripVar, 0x03, currentIdx, rvInjCycleIdx,	// fetch current (accum quantity)
	instrLdRegTripVar, 0x02, currentIdx, rvVSSpulseIdx,	// fetch current (accum distance)
	instrLdRegTripVar, 0x01, instantIdx, rvInjCycleIdx,	// fetch (inst quantity)
	instrMul2by1,										// calculate (inst quantity) * (accum distance) as right term
	instrSwapReg, 0x23,									// swap right term and (accum quantity) values
	instrLdRegTripVar, 0x01, instantIdx, rvVSSpulseIdx,	// fetch (inst distance)
	instrMul2by1,										// calculate (inst distance) * (accum quantity) as left term
	instrCmpIndex, 2,									// testing (inst distance) * (current quantity) <= (inst quantity) * (current distance)?
	instrBranchIfE, 6,									// if so, skip ahead
	instrCmpIndex, 4,									// testing (inst distance) * (tank quantity) <= (inst quantity) * (tank distance)?
	instrBranchIfE, 2,									// if so, skip ahead
	instrSwapReg, 0x23,									// swap left and right terms
	instrCmpXtoY, 0x32,									// compare left term to right term
	instrBranchIfLTorE, 4,								// if left term <= right term, go zero out and return

	instrLdRegByte, 0x02, 255,							// max out result
	instrDone,											// return to caller

	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone											// exit to caller
};

#endif // useExpansionPins
static void peripheral::initMain(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer initialization section - multiple peripherals may use the same timer
#ifdef useTimer1
#if defined(__AVR_ATmega32U4__)
	// turn on timer1 module
	PRR0 &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC1C to disabled
	TCCR1A &= ~((1 << COM1C1) | (1 << COM1C0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer1 module
	PRR0 &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer1 module
	PRR &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// clear timer 1 output compare force bits for OC1A and OC1B
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega328P__)
#endif // useTimer1
#ifdef useTimer2
#if defined(__AVR_ATmega2560__)
	// turn on timer2 module
	PRR0 &= ~(1 << PRTIM2);

	// set timer 2 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR2A &= ~(1 << WGM21);
	TCCR2A |= (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// set timer 2 prescale factor to 64
	TCCR2B &= ~((1 << CS22));
	TCCR2B |= ((1 << CS21) | (1 << CS20));

	// set OC2A to disabled
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

	// set OC2B to disabled
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

	// clear timer 2 output compare force bits for OC2A and OC2B
	TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

	// disable timer 2 interrupts
	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

	// clear timer 2 interrupt flags
	TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer2 module
	PRR &= ~(1 << PRTIM2);

	// set timer 2 to phase correct PWM mode, TOP = 0xFF
	TCCR2A &= ~(1 << WGM21);
	TCCR2A |= (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// set timer 2 prescale factor to 64
	TCCR2B &= ~((1 << CS22));
	TCCR2B |= ((1 << CS21) | (1 << CS20));

	// set OC2A to disabled
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

	// set OC2B to disabled
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

	// clear force bits for OC2A and OC2B
	TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

	// disable timer 2 interrupts
	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

	// clear timer 2 interrupt flags
	TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

#endif // defined(__AVR_ATmega328P__)
#endif // useTimer2
#ifdef useTimer4
#if defined(__AVR_ATmega32U4__)
	// turn on timer4 module
	PRR1 &= ~(1 << PRTIM4);

	// set timer 4 to phase and frequency correct mode
	TCCR4D &= ~(1 << WGM41);
	TCCR4D |= (1 << WGM40);

	// set timer 4 prescale factor to 64
	TCCR4B &= ~(1 << CS43);
	TCCR4B |= ((1 << CS42) | (1 << CS41) | (1 << CS40));

	// clear timer 4 fault protection
	TCCR4D &= ~((1 << FPIE4) | (1 << FPEN4) | (1 << FPNC4) | (1 << FPES4)  | (1 << FPAC4) | (1 << FPF4));

	// set OC4A to disabled
	TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << PWM4A));

	// set OC4B to disabled
	TCCR4A &= ~((1 << COM4B1) | (1 << COM4B0) | (1 << PWM4B));

	// set OC4D to disabled
	TCCR4C &= ~((1 << COM4D1) | (1 << COM4D0) | (1 << PWM4D));

	// clear timer 4 PWM inversion mode
	TCCR4B &= ~(1 << PWM4X);

	// set timer 4 dead time prescaler to 1
	TCCR4B &= ~((1 << DTPS41) | (1 << DTPS40));

	// clear timer 4 output compare force bits for OC4A and OC4B
	TCCR4A &= ~((1 << FOC4A) | (1 << FOC4B));

	// clear timer 4 output compare force bits for OC4D
	TCCR4C &= ~(1 << FOC4D);

	// clear timer 4 update lock, disable timer 4 enhanced compare mode
	TCCR4E &= ~((1 << TLOCK4) | (1 << ENHC4));

	// disable timer 4 interrupts
	TIMSK4 &= ~((1 < OCIE4D) | (1 < OCIE4A) | (1 < OCIE4B) | (1 < TOIE4));

	// clear timer 4 interrupt flags
	TIFR4 |= ((1 << OCF4D) | (1 << OCF4A) | (1 << OCF4B) | (1 << TOV4));

	// set timer 4 dead time to 0
	DT4 = 0;

	// set timer 4 TOP value to 0x00FF, setting 8 bit mode
	TC4H = 0;
	OCR4C = 255;

#endif // defined(__AVR_ATmega32U4__)
#endif // useTimer4
	SREG = oldSREG; // restore interrupt flag status

#ifdef useTWIsupport
	TWI::init();
#ifdef useAdafruitRGBLCDshield
	adafruitRGBLCDsupport::init(); // go init Adafruit RGB LCD shield
#endif // useAdafruitRGBLCDshield
#endif // useTWIsupport
#ifdef useSerial0Port
	serial0::init();
#endif // useSerial0Port
#ifdef useSerial1Port
	serial1::init();
#endif // useSerial1Port
#if defined(__AVR_ATmega32U4__)
	initUSB();
#endif // defined(__AVR_ATmega32U4__)
	initButton();
	LCD::init();
#ifdef useStatusLEDpins
	initStatusLED();
#endif // useStatusLEDpins
#ifdef useExpansionPins
	initExpansionPin();
#endif // useExpansionPins

}

static void peripheral::shutdownMain(void)
{

#ifdef useExpansionPins
	shutdownExpansionPin();
#endif // useExpansionPins
#ifdef useStatusLEDpins
	shutdownStatusLED();
#endif // useStatusLEDpins
	changeBitFlags(timer0Command, t0cDisplayDelay, 0); // cancel any display delays in progress
	LCD::shutdown(); // shut down the LCD display
	shutdownButton();
#if defined(__AVR_ATmega32U4__)
	shutdownUSB();
#endif // defined(__AVR_ATmega32U4__)
#ifdef useSerial1Port
	serial1::shutdown();
#endif // useSerial1Port
#ifdef useSerial0Port
	serial0::shutdown();
#endif // useSerial0Port

#ifdef useTimer4
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM4); // shut off timer4 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)

#endif // useTimer4
#ifdef useTimer2
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer2
#ifdef useTimer1Interrupt
#if defined(__AVR_ATmega32U4__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer1Interrupt
#ifdef useTimer1
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer1
}

// this function is needed since there is no way to perform an atomic bit change of an SRAM byte value
// most MPGuino variables that are shared between main program and interrupt handlers should not need to
//    be treated as atomic (!) because only one side or the other is supposed to change said variables
// however, status flag registers are obviously an exception, and status flag changes are common
//    enough to warrant an explicit function definition
static void peripheral::changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	flagRegister = (flagRegister & ~(maskAND)) | (maskOR); // go perform atomic status flag change

	SREG = oldSREG; // restore interrupt flag status

}

#ifdef useExpansionPins
static void peripheral::initExpansionPin(void)
{

#if defined(__AVR_ATmega32U4__)
	// set OC4A to clear-up/set-down PWM mode for EXP1 option pin
	TCCR4A &= ~(1 << COM4A0);
	TCCR4A |= ((1 << COM4A1) | (1 << PWM4A));

	// set OC4D to clear-up/set-down PWM mode for EXP2 option pin
	TCCR4C &= ~(1 << COM4D0);
	TCCR4C |= ((1 << COM4D1) | (1 << PWM4D));

	// enable EXP1 and EXP2 option pin outputs
	DDRC |= (1 << DDC6); // set OC4A as output
	DDRD |= (1 << DDD7); // set OC4D as output

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// set OC1B to clear-up/set-down for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << PB6) | (1 << PB4));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// set OC1B to non-inverting mode for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << PB3) | (1 << PB2));

#endif // defined(__AVR_ATmega328P__)
	outputExpansionPin1(2); // initially set EXP1 option pin to 0
	outputExpansionPin2(2); // initially set EXP2 option pin to 0

}

static void peripheral::shutdownExpansionPin(void)
{

	outputExpansionPin1(2); // set EXP1 option pin to 0
	outputExpansionPin2(2); // set EXP2 option pin to 0

#if defined(__AVR_ATmega32U4__)
	// disable expansion pin output
	DDRC &= ~(1 << DDC6);
	DDRD &= ~(1 << DDD7);

	// set OC4A to disabled for EXP1 option pin
	TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << PWM4A));

	// set OC4D to disabled for EXP2 option pin
	TCCR4C &= ~((1 << COM4D1) | (1 << COM4D0) | (1 << PWM4D));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// disable expansion pin output
	DDRB &= ~((1 << PB6) | (1 << PB4));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable expansion pin output
	DDRB &= ~((1 << PB3) | (1 << PB2));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

#endif // defined(__AVR_ATmega328P__)
}

// sets EXP1 option pin PWM output from 0-100%
static void peripheral::outputExpansionPin1(uint8_t pin)
{

	uint8_t val = SWEET64::runPrgm(prgmCalculateOutputPinValue, pin);

#if defined(__AVR_ATmega32U4__)
	OCR4A = val;
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	OCR1B = val;
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR1B = val;
#endif // defined(__AVR_ATmega328P__)

}

// sets EXP2 option pin PWM output from 0-100%
static void peripheral::outputExpansionPin2(uint8_t pin)
{

	uint8_t val = SWEET64::runPrgm(prgmCalculateOutputPinValue, pin);

#if defined(__AVR_ATmega32U4__)
	OCR4D = val;
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	OCR2A = val;
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR2A = val;
#endif // defined(__AVR_ATmega328P__)

}

#endif // useExpansionPins
static void peripheral::initButton(void)
{

#ifdef useAdafruitRGBLCDshield
	uint16_t MCP23017registers;
#endif // useAdafruitRGBLCDshield
	uint8_t oldSREG;

#ifdef useAdafruitRGBLCDshield
#if useTWIbuttons && useTWILCD
	LCD::disableIntSample(); // disable TWI button sampling

#endif // useTWIbuttons && useTWILCD
	MCP23017registers = (uint16_t)(buttonMask);

	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_IODIRx, (union_16 *)(&MCP23017registers)); // write out port direction (only buttons as input)
	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (only buttons as input)
	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (only buttons as input)

	adafruitRGBLCDsupport::setTransferMode(adaTWIbyteMode); // ensure address mode is in byte mode

#if useTWIbuttons && useTWILCD
	LCD::enableIntSample(); // enable TWI button sampling

#endif // useTWIbuttons && useTWILCD
#endif // useAdafruitRGBLCDshield
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useLegacyButtons
#if defined(__AVR_ATmega2560__)
	DIDR2 &= ~((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // enable digital input on port K button pins

	PORTK |= ((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // enable port K button pullup resistors

	PCMSK2 |= ((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // enable port K button interrupts

	PCICR |= (1 << PCIE2); // enable selected interrupts on port K

	lastPINxState = PINK; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DIDR0 &= ~((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // enable digital input on port C button pins

	PORTC |= ((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // enable port C button pullup resistors

	PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // enable port C button interrupts

	PCICR |= (1 << PCIE1); // enable selected interrupts on port C

	lastPINxState = PINC; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega328P__)
#endif // useLegacyButtons
	SREG = oldSREG; // restore interrupt flag status

}

static void peripheral::shutdownButton(void)
{
}

#if defined(useAnalogButtons) || defined(useDebugTerminal)
static void peripheral::injectButton(uint8_t buttonValue)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	thisButtonState = buttonValue;
	timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

	SREG = oldSREG; // restore interrupt flag status

}

#endif // useAnalogButtons || useDebugTerminal
#if defined(__AVR_ATmega32U4__)
static void peripheral::initUSB(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	PRR1 &= ~(1 << PRUSB); // turn on USB hardware

	UHWCON = (1 << UVREGE); // enable USB pad regulator

	USBCON = ((1 << USBE) | (1 << FRZCLK)); // enable USB controller, disable clock inputs

#if F_CPU == 16000000UL
	PLLCSR = ((1 << PINDIV) | (1 << PLLE)); // set PLL input prescaler for 16 MHz clock source, and enable PLL
#elif F_CPU == 8000000UL
	PLLCSR = (1 << PLLE); // set PLL input prescaler for 8 MHz clock source, and enable PLL
#else // F_CPU == 16000000UL
#error *** F_CPU clock rate not supported!!! ***
#endif // F_CPU == 16000000UL

	while (!(PLLCSR & (1 << PLOCK))); // wait for PLL lock

	USBCON = ((1 << USBE) | (1 << OTGPADE)); // keep USB controller enabled, enable clock inputs, enable VBUS pad

	UDCON = 0; // select USB high speed mode

	usbConfiguration = 0;

	UDIEN = ((1 << EORSTE) | (1 << SOFE)); // enable End-Of-Reset, Start-Of-Frame interrupts

	SREG = oldSREG; // restore interrupt flag status

}

static void peripheral::shutdownUSB(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	UDIEN = 0; // disable all USB interrupts

	USBCON = 0; // disable USB controller

	PLLCSR = 0; // disable PLL

	UHWCON = 0; // disable USB pad regulator

	PRR1 |= (1 << PRUSB); // turn off USB hardware

	SREG = oldSREG; // restore interrupt flag status

}

#endif // defined(__AVR_ATmega32U4__)
#ifdef useStatusLEDpins
static void peripheral::initStatusLED(void)
{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//	DDRB |= (1 << DDB0); // turn on digital output for RX LED
	DDRC |= (1 << DDC7); // turn on digital output for LED L
//	DDRD |= (1 << DDD5); // turn on digital output for TX LED
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for initializing status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	DDRB |= (1 << DDB7); // turn on digital output for LED L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for initializing status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB |= (1 << DDB5); // turn on digital output for LED L
#endif // defined(__AVR_ATmega328P__)

	outputStatusLED(0); // initially turn off status LED

}

static void peripheral::shutdownStatusLED(void)
{

	outputStatusLED(0); // turn off status LED

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//	DDRB &= ~(1 << DDB0); // turn off digital output for RX LED
	DDRC &= ~(1 << DDC7); // turn off digital output for LED L
//	DDRD &= ~(1 << DDD5); // turn off digital output for TX LED
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for turning off status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	DDRB &= ~(1 << DDB7); // turn off digital output for LED L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for turning off status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB &= ~(1 << DDB5); // turn off digital output for LED L
#endif // defined(__AVR_ATmega328P__)

}

static void peripheral::outputStatusLED(uint8_t val)
{

	if (val)
	{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//		PORTB &= ~(1 << PORTB0); // active low RX
		PORTC |= (1 << PORTC7); // active high L
//		PORTD &= ~(1 << PORTD5); // active low TX
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for turning on status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
		PORTB |= (1 << PORTB7); // active high L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for turning on status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB |= (1 << PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//		PORTB |= (1 << PORTB0); // active low RX
		PORTC &= ~(1 << PORTC7); // active high L
//		PORTD |= (1 << PORTD5); // active low TX
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for turning off status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
		PORTB &= ~(1 << PORTB7); // active high L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for turning off status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB &= ~(1 << PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}

}

#endif // useStatusLEDpins
#ifdef useDataLoggingOutput
static const uint8_t dataLogInstr[][2] PROGMEM = {
	 {lblInstantIdx,	 	tFuelEcon}				// average fuel economy  for the past loop
	,{lblInstantIdx,	 	tSpeed}					// average vehicle speed for the past loop
	,{lblInstantIdx,	 	tInjectorOpenTime}		// fuel injector raw open time for the past loop
	,{lblInstantIdx,	 	tInjectorPulseCount}	// fuel injector pulse count for the past loop
	,{lblInstantIdx,	 	tVSSpulseCount}			// vss pulse count for the past loop
};

static const uint8_t dLIcount = (sizeof(dataLogInstr) / sizeof(uint8_t));

static void doOutputDataLog(void)
{

	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		tripIdx = pgm_read_byte(&dataLogInstr[(unsigned int)(x)][0]);
		calcIdx = pgm_read_byte(&dataLogInstr[(unsigned int)(x)][1]);

		text::numberOut(devLogOutput, tripIdx, calcIdx, mBuff1, 0, dfOverflow9s);
		text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
		text::charOut(devLogOutput, c);

	}

}

#endif // useDataLoggingOutput
#ifdef useParallax5PositionSwitch
const unsigned int analogButtonThreshold[] PROGMEM = {
	0,		// 00
	559,	// 01
	588,	// 02
	618,	// 04
	645,	// 03
	653,	// 05
	664,	// 06
	700,	// 0A
	725,	// 0B
	750,	// 07
	786,	// 0C
	805,	// 0D
	820,	// 09
	834,	// 0E
	858,	// 0F
	897,	// 10
	927,	// 11
	980		// 12
};

const uint8_t analogButtonCount = (sizeof(analogButtonThreshold) / sizeof(unsigned int));

//	btnShortPressDC,	// 08

const uint8_t analogTranslate[(unsigned int)(analogButtonCount)] PROGMEM = {
	buttonsUp,			// 00
	btnShortPressULC,	// 01
	btnShortPressUL,	// 02
	btnShortPressDLC,	// 04
	btnShortPressLC,	// 03
	btnShortPressDL,	// 05
	btnShortPressL,		// 06
	btnShortPressUCR,	// 0A
	btnShortPressUR,	// 0B
	btnShortPressUC,	// 07
	btnShortPressU,		// 0C
	btnShortPressDCR,	// 0D
	btnShortPressCR,	// 09
	btnShortPressDR,	// 0E
	btnShortPressR,		// 0F
	btnShortPressC,		// 10
	btnShortPressD,		// 11
	buttonsUp			// 12
};

#endif // useParallax5PositionSwitch
#ifdef useAnalogMuxButtons
const unsigned int analogButtonThreshold[] PROGMEM = {
	0,		// 00
	556,	// 01
	560,	// 02
	567,	// 03
	574,	// 04
	583,	// 05
	593,	// 06
	601,	// 07
	609,	// 08
	621,	// 09
	634,	// 0A
	644,	// 0B
	653,	// 0C
	665,	// 0D
	677,	// 0E
	687,	// 0F
	698,	// 10
	722,	// 11
	747,	// 12
	759,	// 13
	772,	// 14
	789,	// 15
	806,	// 16
	820,	// 17
	835,	// 18
	859,	// 19
	884,	// 1A
	902,	// 1B
	921,	// 1C
	944,	// 1D
	968,	// 1E
	989,	// 1F
	1012	// 20
};

const uint8_t analogButtonCount = (sizeof(analogButtonThreshold) / sizeof(unsigned int));

const uint8_t analogTranslate[(unsigned int)(analogButtonCount)] PROGMEM = {
	buttonsUp,
	btnShortPressUDCRL,
	btnShortPressULCR,
	btnShortPressDLCR,
	btnShortPressLCR,
	btnShortPressUDLC,
	btnShortPressULC,
	btnShortPressDLC,
	btnShortPressLC,
	btnShortPressUDLR,
	btnShortPressULR,
	btnShortPressDLR,
	btnShortPressLR,
	btnShortPressUDL,
	btnShortPressUL,
	btnShortPressDL,
	btnShortPressL,
	btnShortPressUDCR,
	btnShortPressUCR,
	btnShortPressDCR,
	btnShortPressCR,
	btnShortPressUDC,
	btnShortPressUC,
	btnShortPressDC,
	btnShortPressC,
	btnShortPressUDR,
	btnShortPressUR,
	btnShortPressDR,
	btnShortPressR,
	btnShortPressUD,
	btnShortPressU,
	btnShortPressD,
	buttonsUp
};

#endif // useAnalogMuxButtons
const uint8_t tripUpdateSrcList[] PROGMEM = {
#ifdef trackIdleEOCdata
	 0x7E								// transfer old raw idle trip to idle instant trip
	,0x7E								// update idle tank trip with old raw idle trip
	,0x7E								// update idle current trip with old raw idle trip
	,0x7E								// update old raw trip with old raw idle trip
	,0x7F								// update tank trip with old raw trip
#else // trackIdleEOCdata
	 0x7F								// update tank trip with old raw trip
#endif // trackIdleEOCdata
	,0x7F								// update current trip with old raw trip
#ifdef useWindowTripFilter
	,0x7F								// transfer old raw trip to current window trip
	,windowTripFilterIdx				// transfer window trip 1 to instant trip
	,windowTripFilterIdx + 1			// update instant trip with window trip 2
	,windowTripFilterIdx + 2			// update instant trip with window trip 3
	,windowTripFilterIdx + 3			// update instant trip with window trip 4
#else // useWindowTripFilter
	,0x7F								// transfer old raw trip to instant trip
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,0x7F								// update (or transfer) fuel econ vs time bargraph trip with old raw trip
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,0x7F								// update fuel econ vs speed bargraph trip with old raw trip
#endif // useBarFuelEconVsSpeed
};

const uint8_t tripUpdateDestList[] PROGMEM = {
#ifdef trackIdleEOCdata
	 eocIdleInstantIdx | 0x80			// transfer old raw idle trip to idle instant trip
	,eocIdleTankIdx 					// update idle tank trip with old raw idle trip
	,eocIdleCurrentIdx 					// update idle current trip with old raw idle trip
	,0x7F								// update old raw trip with old raw idle trip
	,tankIdx							// update tank trip with old raw trip
#else // trackIdleEOCdata
	 tankIdx							// update tank trip with old raw trip
#endif // trackIdleEOCdata
	,currentIdx							// update current trip with old raw trip
#ifdef useWindowTripFilter
	,0x7D | 0x80						// transfer old raw trip to current window trip
	,instantIdx | 0x80					// transfer window trip 1 to instant trip
	,instantIdx							// update instant trip with window trip 2
	,instantIdx							// update instant trip with window trip 3
	,instantIdx							// update instant trip with window trip 4
#else // useWindowTripFilter
	,instantIdx | 0x80					// transfer old raw trip to instant trip
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,0x7C	 							// update (or transfer) fuel econ vs time bargraph trip with old raw trip
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,0x7B								// update fuel econ vs speed bargraph trip with old raw trip
#endif // useBarFuelEconVsSpeed
};

const uint8_t tUScount = (sizeof(tripUpdateSrcList) / sizeof(uint8_t));

static uint8_t translateTripIndex(const uint8_t tripTranslateList[], uint8_t tripListPos)
{

#ifdef useBarFuelEconVsTime
	uint8_t oldSREG;
#endif // useBarFuelEconVsTime
	uint8_t i;
	uint8_t j;

	j = pgm_read_byte(&tripTranslateList[(unsigned int)(tripListPos)]);
	i = j & 0x7F; // strip off upper bit for now, to look at the trip index in question

	switch (i)
	{

		case 0x7F:		// replace generic raw trip index with old raw trip index
			i = rawTripIdx ^ (raw0tripIdx ^ raw1tripIdx);
			break;

#ifdef trackIdleEOCdata
		case 0x7E:		// replace generic idle/eoc raw trip index with old idle/eoc raw trip index
			i = rawEOCidleTripIdx ^ (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx);
			break;

#endif // trackIdleEOCdata
#ifdef useWindowTripFilter
		case 0x7D:		// replace generic window trip index with current window trip index
			i = wtpCurrentIdx;
			break;

#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
		case 0x7C:		// replace generic fuel econ vs time trip index with current fuel econ vs time trip index
			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			j = FEvTperiodIdx; // current exchange could either be an update or a transfer
			i = j & 0x7F; // work with below trip index validation mechanism

			FEvTperiodIdx = i; // ensure that future trip variable exchanges are updates
			calcFEvTperiodIdx = i;

			SREG = oldSREG; // restore interrupt flag status
			break;

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
		case 0x7B:	// replace generic fuel econ vs speed trip index with current fuel econ vs speed trip index
			i = FEvSpdTripIdx;
			break;

#endif // useBarFuelEconVsSpeed
		default:
			break;

	}

	if (i < tripSlotCount) i |= (j & 0x80); // restore high bit as it tells whether to update or transfer
	else i = 0; // invalid values get remapped to 0

	return i;

}

// this function is called whenever the main program has to wait for some external condition to occur
// when the main program performs some I/O activity, the target peripheral may take some time to acknowledge the activity
//    and let the main program know that the peripheral is ready for more activity
// oftentimes, the main program has to wait for the peripheral in question, so instead of spinning its wheels, it will simply
//    cause the processor to go to sleep by calling this function
// any interrupt will wake up the processor - this also means the primary timer0
// this function is therefore ideal for placing all purely internal operations that do not require direct interaction with the outside world
//
// - fuel/vehicle speed data transfers
// - acceleration test state changes
// - debug sensor simulation counter updates
// - analog button read translations
// - Chrysler fuel correction factor calculations
// - any other operation not requiring interaction with peripherals that require waiting periods to finish
//
// said internal operations would hopelessly bog down interrupt handlers, were they loaded with these internal operations
//
static void idleProcess(void)
{

	uint8_t oldSREG;
	uint8_t i;
	uint8_t j;
	uint8_t k;
	uint8_t m;
#ifdef useStatusLEDpins
	uint8_t ledSleepVal = 0;
#endif // useStatusLEDpins
#ifdef useCPUreading
	uint32_t idleStart;
#endif // useCPUreading

#ifdef useStatusLEDpins
	peripheral::outputStatusLED(ledSleepVal);

#endif // useStatusLEDpins
#ifdef useCPUreading
	idleStart = systemInfo::cycles0(); // record starting time

#endif // useCPUreading
	performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#ifdef useCPUreading
	idleTimerLength += systemInfo::findCycleLength(idleStart, systemInfo::cycles0());
#ifdef useDebugCPUreading
	idleStart = systemInfo::cycles0(); // record starting time
#endif // useDebugCPUreading

#endif // useCPUreading
#ifdef useStatusLEDpins
	peripheral::outputStatusLED(1 - ledSleepVal);

#endif // useStatusLEDpins
	// this is the part of the main loop that only executes twice a second (or what is defined by loopsPerSecond), to collect and process readings
	if (timer0Status & t0sTakeSample) // if main timer has commanded a sample be taken
	{

		peripheral::changeBitFlags(timer0Status, t0sTakeSample, 0); // acknowledge sample command

#ifdef useCPUreading
		mainProgramVariables[(uint16_t)(mpMainLoopAccumulatorIdx)] = systemInfo::findCycleLength(mainStart, systemInfo::cycles0());
		mainProgramVariables[(uint16_t)(mpIdleAccumulatorIdx)] = idleTimerLength;

		mainStart = systemInfo::cycles0();
		idleTimerLength = 0;

#ifdef useDebugCPUreading
		switch (monitorState)
		{

			case 1:
				idleProcessTimerLength = 0;
				displayTimerLength = 0;
				SWEET64timerLength = 0;

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operations atomic

				volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] = 0;

				SREG = oldSREG; // restore interrupt flag status

				monitorState = 2;
				break;

			case 2:
				mainProgramVariables[(uint16_t)(mpDebugAccMainLoopIdx)] = mainProgramVariables[(uint16_t)(mpMainLoopAccumulatorIdx)];
				mainProgramVariables[(uint16_t)(mpDebugAccIdleIdx)] = mainProgramVariables[(uint16_t)(mpIdleAccumulatorIdx)];
				mainProgramVariables[(uint16_t)(mpDebugAccIdleProcessIdx)] = idleProcessTimerLength;
				mainProgramVariables[(uint16_t)(mpDebugAccDisplayIdx)] = displayTimerLength;
				mainProgramVariables[(uint16_t)(mpDebugAccSWEET64idx)] = SWEET64timerLength;

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operations atomic

				mainProgramVariables[(uint16_t)(mpDebugAccInterruptIdx)] = volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)];

				SREG = oldSREG; // restore interrupt flag status

				monitorState = 0;
				break;

			default:
				monitorState = 0;
				break;

		}

#endif // useDebugCPUreading
#endif // useCPUreading
		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operations atomic

		i = rawTripIdx; // save old trip variable index
		rawTripIdx ^= (raw0tripIdx ^ raw1tripIdx); // set new raw trip variable index

#ifdef trackIdleEOCdata
		j = rawEOCidleTripIdx; // save old EOC/idle trip variable index
		rawEOCidleTripIdx ^= (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set new raw EOC/idle trip variable index

#endif // trackIdleEOCdata
		SREG = oldSREG; // restore interrupt flag status

		for (uint8_t x = 0; x < tUScount; x++)
		{

			k = translateTripIndex(tripUpdateSrcList, x) & 0x7F;
			m = translateTripIndex(tripUpdateDestList, x);

			if (m)
			{

				if (m & 0x80) tripVar::transfer(k, m & 0x7F);
				else tripVar::update(k, m);

			}

		}

		tripVar::reset(i); // reset old raw trip variable
#ifdef trackIdleEOCdata
		tripVar::reset(j); // reset old EOC/idle raw trip variable
#endif // trackIdleEOCdata

		if (awakeFlags & aAwakeOnVehicle)
		{

#ifdef useWindowTripFilter
			if (EEPROM::readVal(pWindowTripFilterIdx))
			{

				wtpCurrentIdx++;
				if (wtpCurrentIdx == windowTripFilterIdx + windowTripFilterSize) wtpCurrentIdx = windowTripFilterIdx;

			}

#endif // useWindowTripFilter
		}

	}

#if defined(useAnalogRead)
#ifdef useChryslerMAPCorrection
	if (analogStatus & asReadMAPchannel)
	{

		peripheral::changeBitFlags(analogStatus, asReadMAPchannel, 0); // acknowledge ADC read completion

		SWEET64::runPrgm(prgmCalculateMAPpressure, 0);

	}

#endif // useChryslerMAPCorrection
#ifdef useChryslerBaroSensor
	if (analogStatus & asReadBaroChannel)
	{

		peripheral::changeBitFlags(analogStatus, asReadBaroChannel, 0); // acknowledge ADC read completion

		SWEET64::runPrgm(prgmCalculateBaroPressure, 0);

	}

#endif // useChryslerBaroSensor
#ifdef useAnalogButtons
	if (analogStatus & asReadButtonChannel)
	{

		peripheral::changeBitFlags(analogStatus, asReadButtonChannel, 0); // acknowledge ADC read completion

		for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
		{

			if (analogValue[(unsigned int)(analogButtonChannelIdx)] >= pgm_read_word(&analogButtonThreshold[(unsigned int)(x)]))
			{

#ifdef useTestAnalogButtonIdx
				thisButtonIdx = x;
#endif //  useTestAnalogButtonIdx
				peripheral::injectButton(pgm_read_byte(&analogTranslate[(unsigned int)(x)]));
				break;

			}

		}

	}

#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useDragRaceFunction
	if (timer0Status & t0sAccelTestFlag)
	{

		peripheral::changeBitFlags(timer0Status, t0sAccelTestFlag, 0); // acknowledge sample command

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operations atomic

		accelTestStatus = lastAccelTestStatus; // copy last loop's accel test flag status to this loop
		lastAccelTestStatus = accelerationFlags; // copy current accel test flag status for next loop

		SREG = oldSREG; // restore interrupt flag status

		accelTestStatus = (lastAccelTestStatus ^ accelTestStatus) & accelTestClearFlags; // detect any drag race flag changes

		if (accelTestStatus)
		{

			accelTestState = accelerationFlags & accelTestClearFlags;

			switch (accelTestState)
			{

				case (accelTestTriggered | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
					accelTestState = 1;
					break;

				case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
					accelTestState = 2;
					break;

				case (accelTestFinished):
					if (SWEET64::runPrgm(prgmAccelTestCompareFullSpeeds, 0))
					{

						tripVar::transfer(dragRawHalfSpeedIdx, dragHalfSpeedIdx);
						tripVar::transfer(dragRawFullSpeedIdx, dragFullSpeedIdx);
						tripVar::transfer(dragRawDistanceIdx, dragDistanceIdx);

					}

					accelTestState = 3;
					break;

				case (accelTestFinished | accelTestCancelled):
					accelTestState = 4;
					break;

				case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed):
					accelTestState = 5;
					break;

				case (accelTestActive | accelTestFullSpeed | accelTestDistance):
					accelTestState = 6;
					break;

				case (accelTestActive | accelTestFullSpeed):
					accelTestState = 7;
					break;

				case (accelTestActive | accelTestHalfSpeed | accelTestDistance):
					accelTestState = 8;
					break;

				case (accelTestActive | accelTestHalfSpeed):
					accelTestState = 9;
					break;

				case (accelTestActive | accelTestDistance):
					accelTestState = 10;
					break;

				case (accelTestActive):
					accelTestState = 11;
					break;

				default:
					accelTestState = 12;
					break;

			}

		}
		else
		{

			accelTestState = 0;

			if (accelerationFlags & accelTestFinished)
			{

				if (EEPROM::readVal(pDragAutoFlagIdx))
				{

					if (accelerationTest::triggerTest() == 0) accelTestState = 1;

				}

			}

		}

	}

#endif // useDragRaceFunction
#ifdef useSimulatedFIandVSS
	if (timer1Status & t1sDebugUpdateFIP) // if debug fuel injector pulse period needs adjustment
	{

		peripheral::changeBitFlags(timer1Status, t1sDebugUpdateFIP, 0); // acknowledge debug update request

		debugFIPidx++;
		if (debugFIPidx >= debugFIPlength)
		{

			debugFIPidx = 0;
			debugFIPstate++;
			debugFIPstate &= 3;

		}

		switch (debugFIPstate)
		{

			case 0:
				debugFIPtickLength = pgm_read_word(&debugFIPvalues[(unsigned int)(debugFIPidx)]);
				debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(unsigned int)(debugFIPidx)]);
				debugFIPWgoodTickLength = debugFIPtickLength - 63;
				peripheral::changeBitFlags(debugFlags, 0, debugFIPready);
				break;

			case 1:
				break;

			case 2:
				debugFIPtickLength = pgm_read_word(&debugFIPvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
				debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
				debugFIPWgoodTickLength = debugFIPtickLength - 63;
				peripheral::changeBitFlags(debugFlags, 0, debugFIPready);
				break;

			case 3:
				break;

			default:
				break;

		}

		if (debugFIPWreadTickLength > debugFIPWgoodTickLength) debugFIPWreadTickLength = debugFIPWgoodTickLength;
		else debugFIPWtickLength = debugFIPWreadTickLength;

	}

	if (timer1Status & t1sDebugUpdateVSS) // if VSS pulse period needs adjustment
	{

		peripheral::changeBitFlags(timer1Status, t1sDebugUpdateVSS, 0); // acknowledge debug update request

		debugVSSidx++;
		if (debugVSSidx >= debugVSSlength)
		{

			debugVSSidx = 0;
			debugVSSstate++;
			debugVSSstate &= 3;

		}

		switch (debugVSSstate)
		{

			case 0:
				debugVSStickLength = pgm_read_word(&debugVSSvalues[(unsigned int)(debugVSSidx)]);
				peripheral::changeBitFlags(debugFlags, 0, debugVSSready);
				break;

			case 1:
				break;

			case 2:
				debugVSStickLength = pgm_read_word(&debugVSSvalues[(unsigned int)(debugVSSlength - debugVSSidx - 1)]);
				peripheral::changeBitFlags(debugFlags, 0, debugVSSready);
				break;

			case 3:
				break;

			default:
				break;

		}

	}

#endif // useSimulatedFIandVSS
#ifdef useDebugCPUreading
	idleProcessTimerLength += systemInfo::findCycleLength(idleStart, systemInfo::cycles0());

#endif // useDebugCPUreading
}

static void performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

}

#ifdef useDebugTerminal
const uint8_t tmHexInput =			0x40;
const uint8_t tmSomethingReadIn =	0x80;

uint8_t terminalMode;
uint8_t nextTerminalState;
uint8_t terminalByte;
uint8_t chr;
uint8_t inpIdx;
uint8_t readIdx;
uint8_t terminalState;
uint8_t terminalAddress;
uint8_t terminalLine;
char terminalBuff[tBuffLength];
uint8_t maxLine;
const char * labelList;
const uint8_t * prgmPtr;
void (* primaryFunc)(uint8_t);
void (* extraFunc)(uint8_t);

#endif // useDebugTerminal
// primary MPGuino processing routine - overwrites Arduino sketch main if compiled in Arduino IDE
int main(void)
{

	uint8_t i;
	uint8_t j;
#ifdef useDebugTerminal
	uint8_t oldSREG;
#endif // useDebugTerminal
#ifdef useDebugCPUreading
	uint32_t displayStart;
#endif // useDebugCPUreading

	const buttonVariable * bpPtr;

	cli(); // disable interrupts while interrupts are being fiddled with

	// timer0 is the taskmaster driving MPGuino's measurement functionality
#if defined(__AVR_ATmega32U4__)
	// turn on timer0 module
	PRR0 &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= (1 << AIN0D);
	DIDR2 |= ((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= ((1 << PRTWI) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRADC));
	PRR1 |= ((1 << PRUSB) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer0 module
	PRR0 &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((1 << ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= ((1 << AIN1D) | (1 << AIN0D));
	DIDR2 |= ((1 << ADC15D) | (1 << ADC14D) | (1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= ((1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC));
	PRR1 |= ((1 << PRTIM5) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer0 module
	PRR &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= ((1 << AIN1D) | (1 << AIN0D));

	// shut off on-board peripherals to reduce power consumption
	PRR |= ((1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC));

#endif // defined(__AVR_ATmega328P__)
	ACSR &= ~(1 << ACIE); // disable analog comparator interrupt
	ACSR |= (1 << ACD); // disable analog comparator module
	ADCSRB &= ~(1 << ACME); // disable analog comparator multiplexer

	timer0Command = t0cResetTimer;
#ifdef useTimer1Interrupt
	timer1Command = t1cResetTimer;
#endif // useTimer1Interrupt

#ifdef useSimulatedFIandVSS
	debugFlags = (debugVSSflag | debugInjectorFlag);
	debugReading::configurePorts();

#endif // useSimulatedFIandVSS
	rawTripIdx = raw0tripIdx;
#ifdef trackIdleEOCdata
	rawEOCidleTripIdx = raw0eocIdleTripIdx;
#endif // trackIdleEOCdata

	for (uint8_t x = 0; x < tripSlotCount; x++) tripVar::reset(x);

#ifdef useDragRaceFunction
	accelerationFlags &= ~accelTestClearFlags;
	lastAccelTestStatus = accelerationFlags;
	accelTestStatus = accelerationFlags;

#endif // useDragRaceFunction
	menuLevel = 255;
	topScreenLevel = 0;

	j = EEPROM::powerUpCheck();

	timer0DelayCount = delay1500msTick; // request a set number of timer tick delays equivalent to 1.5 seconds
	timer0Command |= (t0cDoDelay); // signal request to timer

#ifdef useWindowTripFilter
	windowTripFilterReset();

#endif // useWindowTripFilter
	sei();

	peripheral::initMain(); // initialize all human interface peripherals

#ifdef useSavedTrips
	i = tripSupport::doAutoAction(1);

#endif // useSavedTrips
	text::gotoXY(devLCD, 0, 0);
	text::stringOut(devLCD, titleMPGuino);
	text::stringOut(devLCD, dateMPGuino);

#ifdef outputLoggingSplash
	text::stringOut(devLogOutput, titleMPGuino);
	text::stringOut(devLogOutput, dateMPGuino);

#endif // outputLoggingSplash
#ifdef outputDebugTerminalSplash
	text::stringOut(devDebugTerminal, titleMPGuino);
	text::stringOut(devDebugTerminal, dateMPGuino);

	terminalState = 0;

#endif // outputDebugTerminalSplash
	while (timer0Command & t0cDoDelay) idleProcess(); // show splash screen for 1.5 seconds

#ifdef useTestButtonValues
	cursor::moveAbsolute(buttonViewIdx, 0);
#else // useTestButtonValues
#ifdef useLegacyLCD
	if (j) cursor::moveAbsolute(settingScreenIdx, (pContrastIdx - eePtrSettingsStart)); // go through the initialization screen
#else // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	if (j) cursor::moveAbsolute(settingScreenIdx, (pLCDcolorIdx - eePtrSettingsStart)); // go through the initialization screen
#else // useAdafruitRGBLCDshield
	if (j) cursor::moveAbsolute(settingScreenIdx, (pMetricModeIdx - eePtrSettingsStart)); // go through the initialization screen
#endif // useAdafruitRGBLCDshield
#endif // useLegacyLCD
	else cursor::moveAbsolute(mainScreenIdx, 0);
#endif // useTestButtonValues

#ifdef useSavedTrips
	if (i) printStatusMessage(PSTR("AutoRestore Done"));

#endif // useSavedTrips
#ifdef useCPUreading
	mainStart = systemInfo::cycles0();
	idleTimerLength = 0;
#ifdef useDebugCPUreading
	idleProcessTimerLength = 0;
	monitorState = 0;
	displayTimerLength = 0;
	SWEET64timerLength = 0;
#endif // useDebugCPUreading

#endif // useCPUreading
	while (true)
	{

#ifdef useDebugCPUreading
		displayStart = systemInfo::cycles0();

#endif // useDebugCPUreading
		if (activityChangeFlags & afActivityTimeoutFlag)
		{

			peripheral::changeBitFlags(activityChangeFlags, afActivityTimeoutFlag, 0); // clear activity change activity timeout flag

			if (activityFlags & afActivityTimeoutFlag) // if MPGuino is commanded to go asleep
			{

#ifdef useDeepSleep
				doGoDeepSleep();

#else // useDeepSleep
#ifdef useClockDisplay
				bigDigit::displayHandler(menuEntryIdx, 255, 1); // initialize the software clock
				text::charOut(devLCD, 0x12); // set backlight brightness to zero

#else // useClockDisplay
				LCD::shutdown(); // shut down the LCD display

#endif // useClockDisplay
#endif // useDeepSleep
			}
			else // if MPGuino is commanded to wake up
			{

				LCD::init(); // re-initialize LCD device
				((displayHandlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(menuEntryIdx, screenCursor[(unsigned int)(menuLevel)], 1); // call indexed support section screen initialization function

			}

		}

		if (activityChangeFlags & afEngineOffFlag)
		{

			peripheral::changeBitFlags(activityChangeFlags, afEngineOffFlag, 0); // clear activity change engine off flag

			if (((activityFlags & afEngineOffFlag) == 0) && (EEPROM::readVal(pWakeupResetCurrentOnEngineIdx))) tripSupport::resetCurrent(); // if engine start is detected

		}

		if (activityChangeFlags & afVehicleStoppedFlag)
		{

			peripheral::changeBitFlags(activityChangeFlags, afVehicleStoppedFlag, 0); // clear activity change vehicle stopped flag

			if (((activityFlags & afVehicleStoppedFlag) == 0) && (EEPROM::readVal(pWakeupResetCurrentOnMoveIdx))) tripSupport::resetCurrent(); // if vehicle movement is detected

		}

#if useWindowTripFilter || useSavedTrips
		if (activityChangeFlags & afParkFlag)
		{

			peripheral::changeBitFlags(activityChangeFlags, afParkFlag, 0); // clear activity change park flag

			if (activityFlags & afParkFlag) // if MPGuino is commanded to go park
			{

#ifdef useWindowTripFilter
				windowTripFilterReset(); // reset the window trip filter

#endif // useWindowTripFilter
#ifdef useSavedTrips
				if (tripSupport::doAutoAction(0)) printStatusMessage(PSTR("AutoSave Done"));

#endif // useSavedTrips
			}

		}

#endif // useWindowTripFilter || useSavedTrips
#ifdef useDebugTerminal
/*

	terminal commands:
      P - list stored parameters
      V - list volatile variables
      M - list main program variables
      O - list program constants
      T - list read-in trip variable values
      L - list functions for read-in trip variable
     ^L - list SWEET64 source code for function
   [x]R - read trip variable x into terminal trip variable
   [x]W - write terminal trip variable into trip variable x
          if no x specified, lists available trip variables
      S - output system status
      I - inject button press
            short (l, c, r, u, d)
             long (L, C, R, U, D)
      ? - displays this help

	numbers or button presses are separated by spaces

	ex:
	]1r           << reads instant trip variable into terminal

	]1l 2l 3l     << prints functions 1, 2, and 3 associated with read-in trip variable

	]l            << prints all available functions associated with read-in  trip variable

	]ild          << injects short button press l+d into MPGuino

	]ir r r r     << injects 4 successive short press r into MPGuino

	]irc R LC r   << injects short press r+c, long press r, long press l+c, short press r into MPGuino

*/
		switch (terminalState)
		{

			case 0:		// initialize terminal state machine and output the prompt
				text::charOut(devDebugTerminal, 0x0D);
				text::charOut(devDebugTerminal, ']');
				terminalState++;
				inpIdx = 0;
				readIdx = 0;
				terminalMode = tmHexInput;
				terminalByte = 0;
				terminalAddress = 0;
			case 1:		// get line
				i = devDebugTerminal.chrIn();

				if (i)
				{

					switch (i)
					{

						case 0x7F:
						case 0x08:
							if (inpIdx) inpIdx--;
							else
							{

								text::charOut(devDebugTerminal, 0x0D);
								terminalState = 0;

							}
							break;

						case 0x0D:
							text::charOut(devDebugTerminal, 0x0D);
							terminalState++;
							break;

						case 0x18:
							text::charOut(devDebugTerminal, '\\');
							terminalState = 0;
							break;

#ifdef useSWEET64trace
						case 0x0C:
#endif // useSWEET64trace
						case 0x20 ... 0x7E:
							if (inpIdx == tBuffLength)
							{

								text::stringOut(devDebugTerminal, PSTR("\\\r"));
								terminalState = 0;

							}
							else
							{

								if (i > 31)	text::charOut(devDebugTerminal, i);
								else
								{

									text::charOut(devDebugTerminal, '^');
									text::charOut(devDebugTerminal, i + 64);

								}
								terminalBuff[(uint16_t)(inpIdx++)] = i;

							}
							break;

						default:
							break;

					}

				}
				break;

			case 2:   // process input line
				if (readIdx >= inpIdx) chr = 0x0D;
				else chr = terminalBuff[(uint16_t)(readIdx++)];

				i = 1;
				if ((terminalMode & tmSomethingReadIn) == 0)
				{

					terminalByte = 0;
					SWEET64::init64byt((union union_64 *)(&s64reg[s64reg6]), 0);

				}
				if (chr > 0x5F) j = chr - 0x20;
				else j = chr;
				switch (terminalMode)
				{

					case 'I':										// parse a button press character
					case ('I' | tmSomethingReadIn):					// parse a button press character
						for (uint8_t x = 0; x < terminalButtonCount; x++)
							if (chr == pgm_read_byte(&terminalButtonChars[(uint16_t)(x)]))
							{

								i = 0; // signal that a valid character was already read in
								terminalByte |= (pgm_read_byte(&terminalButtonValues[(uint16_t)(x)]));
								terminalMode |= (tmSomethingReadIn);

							}
						break;

					case ('.' | tmHexInput):						// parse an address range upper limit
#ifdef useSWEET64trace
					case (0x1B | tmHexInput):						// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
					case (0x0D | tmHexInput):						// parse a specific main program variable hexadecimal value input digit
					case (0x10 | tmHexInput):						// parse a specific parameter value input hexadecimal digit
					case (0x14 | tmHexInput):						// parse a specific trip variable measurement hexadecimal value input digit
					case (0x16 | tmHexInput):						// parse a specific volatile variable value input hexadecimal digit
					case (tmHexInput):								// parse a generic hexadecimal digit
					case ('.' | tmSomethingReadIn | tmHexInput):	// parse an address range upper limit
#ifdef useSWEET64trace
					case (0x1B | tmSomethingReadIn | tmHexInput):	// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
					case (0x0D | tmSomethingReadIn | tmHexInput):	// parse a specific main program variable value input hexadecimal digit
					case (0x10 | tmSomethingReadIn | tmHexInput):	// parse a specific parameter value input hexadecimal digit
					case (0x14 | tmSomethingReadIn | tmHexInput):	// parse a specific trip variable measurement value input hexadecimal digit
					case (0x16 | tmSomethingReadIn | tmHexInput):	// parse a specific volatile variable value input hexadecimal digit
					case (tmSomethingReadIn | tmHexInput):			// parse a generic hexadecimal digit
						switch (j)
						{

							case 'A' ... 'F':
								j -= 7;
							case '0' ... '9':
								j -= 48;
								i = 0; // signal that a valid character was already read in
								terminalMode |= (tmSomethingReadIn);
								terminalByte = SWEET64::runPrgm(prgmParseHexDigit, j);
								break;

							default:
								break;

						}
						break;

					case '.':										// parse an address range upper limit
#ifdef useSWEET64trace
					case 0x1B:										// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
					case 0x0D:										// parse a specific main program decimal digit variable input decimal digit or switch to hex mode
					case 0x10:										// parse a specific parameter decimal digit input decimal digit or switch to hex mode
					case 0x14:										// parse a specific trip variable measurement decimal digit input decimal digit or switch to hex mode
					case 0x16:										// parse a specific volatile variable decimal digit input decimal digit or switch to hex mode
					case 0x00:										// parse a generic decimal digit or switch to hex mode
						if ((j == 'X') || (j == '$'))
						{

							terminalMode |= (tmHexInput);
							i = 0; // signal that a valid character was already read in
							break;

						}

					case ('.' | tmSomethingReadIn):					// parse an address range upper limit
#ifdef useSWEET64trace
					case (0x1B | tmSomethingReadIn):				// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
					case (0x0D | tmSomethingReadIn):				// parse a specific main program variable decimal digit
					case (0x10 | tmSomethingReadIn):				// parse a specific parameter decimal digit
					case (0x14 | tmSomethingReadIn):				// parse a specific trip variable measurement decimal digit
					case (0x16 | tmSomethingReadIn):				// parse a specific volatile variable decimal digit
					case (tmSomethingReadIn):						// parse a generic decimal digit
						switch (j)
						{

							case '0' ... '9':
								j -= 48;
								i = 0; // signal that a valid character was already read in
								terminalMode |= (tmSomethingReadIn);
								terminalByte = SWEET64::runPrgm(prgmParseDecimalDigit, j);
								break;

							default:
								break;

						}
					default:										// nothing to parse - could be a possible command
						break;

				}

				if (i) // parsing wasn't valid, so check for a command
				{

					if (chr > 0x5F) chr &= 0x5F;
					nextTerminalState = 2; // get another character from input buffer
					switch (chr)
					{

						case '?':   // display help
							terminalLine = 0; // initialize terminal output line
							terminalMode = tmHexInput; // shift to parsing a generic input value
							terminalState = 12; // this command WILL print a lot of different lines, so handle this command one iteration at a time
							break;

#ifdef useSWEET64trace
						case 0x0C:
#endif // useSWEET64trace
						case 'L':   // list available trip functions from terminalIdx
						case 'M':   // list available main program variables
						case 'O':	// list available program constants
						case 'P':   // list available stored parameters
						case 'T':   // list available trip variable measurements
						case 'V':   // list available volatile variables
							i = terminalMode; // save old terminal mode
							terminalMode = chr; // prepare to shift to parsing an array-specific input value
							chr = '>'; // do unified list output preparation
							break;

						case '.':	// specify address range
							terminalAddress = terminalByte;
							terminalMode = (chr | tmHexInput); // shift to reading button press words
							break;

						case 'I':   // inject button press
							terminalMode = chr; // shift to reading button press words
							break;

						case 'W':	// write trip variable
							terminalLine = terminalByte;
							terminalByte = terminalIdx;
							terminalState = 20;
							break;

						case 'R':   // read trip variable into terminal
							terminalLine = terminalIdx;
							terminalState = 20;
							break;

						case 'S':   // print system status
							terminal::outputFlags(activityFlags, terminalActivityFlagStr);
							terminalMode = tmHexInput; // shift to parsing a generic input value
							break;

						case 0x0D:    // carriage return - treat as a special space
							nextTerminalState = 0; // when finished processing, go back to terminal state 0 - initialize input and print prompt character
						case ' ':   // space character
							switch (terminalMode)
							{

								case ('I' | tmSomethingReadIn):
									peripheral::injectButton(terminalByte); // inject the parsed button press value into timer0
									terminalState = 14;
									terminalMode &= ~(tmSomethingReadIn); // go parse another button press value
									break;

#ifdef useSWEET64trace
								case 0x1B:	// if no address was specified
									primaryFunc = 0;
#endif // useSWEET64trace
								case 'L':	// if no address was specified
								case 'M':	// if no address was specified
								case 'O':	// if no address was specified
								case 'P':	// if no address was specified
								case 'T':	// if no address was specified
								case 'V':	// if no address was specified
								case (0x0D):	// if no values were input
								case (0x10):	// if no values were input
								case (0x14):	// if no values were input
								case (0x16):	// if no values were input
									terminalState = 32; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

								case (0x10 | tmSomethingReadIn | tmHexInput):	// process a specific parameter hexadecimal value input
								case (0x10 | tmSomethingReadIn):				// process a specific parameter decimal value
									if (prgmPtr) parameterEdit::onEEPROMchange(prgmPtr, terminalAddress);
									terminalAddress = 0;
									terminalMode = tmHexInput; // go parse a generic hexadecimal digit
									terminalState = nextTerminalState;
									break;

								case (0x0D | tmSomethingReadIn | tmHexInput):	// process a specific main program variable hexadecimal value input
								case (0x14 | tmSomethingReadIn | tmHexInput):	// process a specific trip variable measurement hexadecimal value input
								case (0x16 | tmSomethingReadIn | tmHexInput):	// process a specific volatile variable hexadecimal value input
								case (0x0D | tmSomethingReadIn):				// process a specific main program variable decimal value
								case (0x14 | tmSomethingReadIn):				// process a specific trip variable measurement decimal value
								case (0x16 | tmSomethingReadIn):				// process a specific volatile variable decimal value
									if (prgmPtr) SWEET64::runPrgm(prgmPtr, terminalAddress);
								default:
									terminalAddress = 0;
									terminalMode = tmHexInput; // go parse a generic hexadecimal digit
									terminalState = nextTerminalState;
									break;

							}
							break;

						default:    // unsupported command
							chr = ' ';
							terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character
							break;

					}

					if (chr == '>')
					{

						primaryFunc = 0;
						extraFunc = 0;
						maxLine = 0;
						prgmPtr = 0;
						labelList = 0;
						switch (terminalMode)
						{

#ifdef useSWEET64trace
							case 0x0C:	// generate SWEET64 source code for a given function
								terminalMode = 0x1B;
#endif // useSWEET64trace
							case 'L':   // list available trip functions from terminalIdx
								maxLine = dfMaxValDisplayCount;
#if (useDebugTerminalLabels) || (useSWEET64trace)
								labelList = terminalTripFuncNames;
#endif // useDebugTerminalLabels
								primaryFunc = terminal::outputTripFunctionValue;
								break;

							case 'M':   // list available main program variables
								maxLine = mpVariableMaxIdx;
#ifdef useDebugTerminalLabels
								labelList = terminalMainProgramVarLabels;
#endif // useDebugTerminalLabels
								primaryFunc = terminal::outputMainProgramValue;
								prgmPtr = prgmWriteMainProgramValue;
								break;

							case 'O':   // list available program constants
								maxLine = idxMaxConstant;
#ifdef useDebugTerminalLabels
								labelList = terminalConstIdxNames;
#endif // useDebugTerminalLabels
								primaryFunc = terminal::outputConstantValue;
								break;

							case 'P':   // list available stored parameters
								maxLine = eePtrStorageEnd;
#ifdef useDebugTerminalLabels
								labelList = parmLabels;
#endif // useDebugTerminalLabels
								primaryFunc = terminal::outputParameterValue;
								extraFunc = terminal::outputParameterExtra;
								prgmPtr = prgmTerminalWriteParameterValue;
								break;

							case 'T':   // list available trip variable measurements
								maxLine = rvMeasuredCount;
#ifdef useDebugTerminalLabels
								labelList = terminalTripVarLabels;
#endif // useDebugTerminalLabels
								primaryFunc = terminal::outputTripVarMeasuredValue;
								extraFunc = terminal::outputTripVarMeasuredExtra;
								prgmPtr = prgmWriteTripMeasurementValue;
								break;

							case 'V':   // list available volatile variables
								maxLine = vVariableMaxIdx;
#ifdef useDebugTerminalLabels
								labelList = terminalVolatileVarLabels;
#endif // useDebugTerminalLabels
								primaryFunc = terminal::outputVolatileValue;
								prgmPtr = prgmWriteVolatileValue;
								break;

							default:
								break;

						}

						if (i & tmSomethingReadIn) // examine old terminal mode to see if a hex byte was read in
						{

							if (terminalByte < maxLine)
							{

								switch (i)
								{

									case (tmSomethingReadIn | tmHexInput):
									case (tmSomethingReadIn):
										terminalLine = terminalByte; // if a hex byte was read in and is less than maxLine, use it
										maxLine = terminalByte; // only output one line

										if (prgmPtr) // if this command could result in storing a value instead
										{

											terminalMode &= 0x1F; // go parse a decimal or hex value for possible saving
											terminalAddress = terminalByte; // save for possible saving

										}
										break;

									case ('.' | tmSomethingReadIn | tmHexInput):
									case ('.' | tmSomethingReadIn):
										if (terminalAddress < terminalByte) terminalLine = terminalAddress;
										else terminalLine = terminalByte;

										maxLine = terminalByte + 1;
										prgmPtr = 0;
										break;

									default:
										break;

								}

							}
							else // otherwise
							{

								text::stringOut(devDebugTerminal, PSTR("input value too large\r"));
								terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

							}

						}
						else
						{

							prgmPtr = 0; // no address was specified, so do not store anything
							switch (i)
							{

								case ('.' | tmHexInput):
								case '.':
									if (terminalAddress < maxLine) terminalLine = terminalAddress;
									else
									{

										text::stringOut(devDebugTerminal, PSTR("input value too large\r"));
										terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

									}
									break;

								default:
									terminalLine = 0;
									break;

							}

						}

					}

				}
				break;

			case 12:	// print a line of help
				text::stringOut(devDebugTerminal, terminalHelp, terminalLine++);
				if (pgm_read_byte(findStr(terminalHelp, terminalLine)) == 0) terminalState = nextTerminalState;
				break;

			case 14:	// wait for injected buttonpress to be accepted into timer0
				if (timer0Command & t0cProcessButton) break;
				terminalState++;
				peripheral::injectButton(buttonsUp); // inject a buttons-up press into timer0
				break;

			case 15:	// wait for injected buttonpress to be accepted into timer0
				if (timer0Command & t0cProcessButton) break;
				terminalState = nextTerminalState;
				break;

			case 20:
				if (terminalMode & 0x80) // if a hex byte was read in
				{

					if (terminalByte >= tripSlotTotalCount)
					{

						text::stringOut(devDebugTerminal, PSTR("Invalid trip variable specified\r"));
						terminalState = 0;
						break;

					}
					else
					{

						oldSREG = SREG; // save interrupt flag status
						cli(); // disable interrupts to make the next operation atomic

						termRawIdx = terminalByte;
						SWEET64::runPrgm(prgmLoadTrip, terminalByte); // this allows direct loading/saving of trips to EEPROM
						SWEET64::runPrgm(prgmSaveTrip, terminalLine);

						SREG = oldSREG; // restore interrupt flag status

						text::stringOut(devDebugTerminal, PSTR("transferred "));
						text::stringOut(devDebugTerminal, terminalTripVarNames, terminalByte);
						text::stringOut(devDebugTerminal, PSTR(" to "));
						text::stringOut(devDebugTerminal, terminalTripVarNames, terminalLine);

						terminalState = nextTerminalState; // go fetch next command
						terminalMode = tmHexInput; // shift to reading hex words
						break;

					}

				}
				terminalState = 32;
				terminalMode = tmHexInput; // shift to reading hex words
				terminalLine = 0;
				maxLine = tripSlotTotalCount;
#ifdef useDebugTerminalLabels
				labelList = terminalTripVarNames;
#else // useDebugTerminalLabels
				labelList = 0;
#endif // useDebugTerminalLabels
				primaryFunc = 0;
				extraFunc = 0;

			case 32:	// output list of selected items
				text::hexByteOut(devDebugTerminal, terminalLine);
				text::stringOut(devDebugTerminal, PSTR(": "));
				if (primaryFunc)
				{

					primaryFunc(terminalLine);
					text::stringOut(devDebugTerminal, PSTR(" - "));

				}
				if (extraFunc)
				{

					extraFunc(terminalLine);
					text::stringOut(devDebugTerminal, PSTR(" - "));

				}
				if (labelList) text::stringOut(devDebugTerminal, labelList, terminalLine);
				else text::newLine(devDebugTerminal);

				terminalLine++;

				if (terminalLine >= maxLine) terminalState = nextTerminalState; // go back to read another command
				break;

			default:
				terminalState = 0;
				break;

		}

#endif // useDebugTerminal
		if (timer0Status & t0sReadButton) // see if any buttons were pressed, process related button function if so
		{

			j = buttonPress; // capture button state
			peripheral::changeBitFlags(timer0Status, t0sReadButton, 0); // acknowledge sample command

			bpPtr = (const buttonVariable *)(pgm_read_word(&(screenParameters[(unsigned int)(menuLevel)].modeButtonList)));

			while (true)
			{

				i = pgm_read_byte(&(bpPtr->buttonCode));

				if ((j == i) || (i == buttonsUp)) break;
				else bpPtr++;

			}

			text::gotoXY(devLCD, 0, 0);
			((pFunc)pgm_read_word(&(bpPtr->buttonCommand)))(); // go perform action

		}

#if useDataLoggingOutput || useJSONoutput
		// this part of the main loop handles logging data output
		if (timer0Status & t0sOutputLogging)
		{

			peripheral::changeBitFlags(timer0Status, t0sOutputLogging, 0); // acknowledge output serial command

#ifdef useDataLoggingOutput
			if (EEPROM::readVal(pSerialDataLoggingIdx)) doOutputDataLog();

#endif // useDataLoggingOutput
#ifdef useJSONoutput
			if (awakeFlags & aAwakeOnVehicle) doOutputJSON();

#endif // useJSONoutput
		}

#endif // useDataLoggingOutput || useJSONoutput
		// this part of the main loop handles screen output to the user
		// it can execute either after the samples are collected and processed above, or after a key has been pressed
		if (timer0Status & t0sUpdateDisplay)
		{

			peripheral::changeBitFlags(timer0Status, t0sUpdateDisplay, 0); // acknowledge update display command

#ifdef useExpansionPins
			peripheral::outputExpansionPin1(0);
			peripheral::outputExpansionPin2(1);

#endif // useExpansionPins
#ifdef useBarFuelEconVsSpeed
			FEvSpdTripIdx = (uint8_t)(SWEET64::runPrgm(prgmFEvsSpeed, instantIdx));

#endif // useBarFuelEconVsSpeed
			// this section handles all MPGuino activity modes
			// engine running and vehicle in motion and a button being pressed
			// engine not running and vehicle in motion (Engine Off Coasting) and a button being pressed
			// engine running and vehicle stopped and a button being pressed
			// engine stopped and vehicle stopped and a button being pressed
			// engine running and vehicle in motion
			// engine not running and vehicle in motion (Engine Off Coasting)
			// engine running and vehicle stopped
			// engine stopped and vehicle stopped
			//
			// in addition, two independent timers are dedicated to these activity modes
			//
			// the park timer counts down from a stored parameter value once MPGuino detects engine not running and vehicle stopped
			// if the park timer goes to zero, the vehicle is said to be parked, and MPGuino goes to parked mode
			// parked mode is exited once MPGuino detects that the engine runs OR the vehicle is in motion
			//
			// the activity timer counts down from a stored parameter value once MPGuino detects engine not running, vehicle stopped, and no button press occurs
			// if the activity timer goes to zero, MPGuino goes to sleep mode
			// sleep mode is exited once MPGuino detects that the engine runs OR the vehicle is in motion OR a button press occurs
			//
			// if the park timer setting in stored parameters is larger than the activity timer, MPGuino will skip parked mode
			i = activityFlags & afValidFlags; // fetch a local copy of activity flags

			if (pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeIndex) & 0x80) // if the currently displayed screen is marked to ignore button presses
			{

				if ((i == (afVehicleStoppedFlag | afButtonFlag)) || (i == (afEngineOffFlag | afButtonFlag))) i &= ~(afButtonFlag); // ignore button presses for this screen

			}

			text::gotoXY(devLCD, 0, 0);
			switch (i)
			{

				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag | afParkFlag): // engine stopped, vehicle stopped, no buttons pressed for a while
				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag): // engine stopped, vehicle stopped for a while
				case (afVehicleStoppedFlag | afButtonFlag): // vehicle stopped, button not pressed
				case (afEngineOffFlag | afButtonFlag): // engine stopped, button not pressed
				case (afVehicleStoppedFlag): // vehicle stopped
				case (afEngineOffFlag): // engine stopped
				case (afVehicleStoppedFlag | afEngineOffFlag): // engine stopped, vehicle stopped
				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag): // engine stopped, vehicle stopped, button not pressed
				case 0: // engine running and vehicle in motion
				case (afButtonFlag): // engine running, vehicle in motion, button not pressed
					((displayHandlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(menuOutputDisplayIdx, screenCursor[(unsigned int)(menuLevel)], 0); // call indexed support section screen refresh function
					break;

				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag | afButtonFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, activity timeout reached
				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, park timeout reached, activity timeout reached
#ifndef useDeepSleep
#ifdef useClockDisplay
					bigDigit::displayHandler(menuOutputDisplayIdx, 255, 0); // display the software clock

#endif // useClockDisplay
#endif // useDeepSleep
					break;

				default: // handle unexpected cases
					text::hexByteOut(devLCD, i);
					break;

			}

		}

#ifdef useDebugCPUreading
		displayTimerLength =+ systemInfo::findCycleLength(displayStart, systemInfo::cycles0());

#endif // useDebugCPUreading
		idleProcess(); // all functions are completed at this point, so wait for an interrupt to occur

	}

}
