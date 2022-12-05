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

  TinkerKit! LCD module
    injector sense open  PD2 (INT2), Digital 0 (note this makes it impossible for the TinkerKit! LCD module to use Serial 1)
    injector sense close PD3 (INT3), Digital 1
    speed                PB7 (PCINT7), Digital 11
    (if configured) MAP  PF5 (ADC5), A2
    (if configured) Baro PF6 (ADC6), A1

  Arduino Mega 2560
    injector sense open  PE4 (INT4), Digital 2
    injector sense close PE5 (INT5), Digital 3
    speed                PK0 (PCINT16), A8
    (if configured) MAP  PF1 (ADC1), A2
    (if configured) Baro PF2 (ADC2), A3

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
         |                Baro SENSOR                 |
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

*/

/* Program overview
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

#include <stdlib.h>
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
// if all of the below preconfigured board "#define"s are commented out, code will compile for an AtMega328-series processor
//
//#define useJellyBeanDriverBoard true		// also sets LCD and button settings
//#define useLegacyBoard true					// also sets LCD and button settings
//#define useArduinoMega2560 true
//#define useTinkerkitLCDmodule true

// the below line sets the processor speed for the MPGuino hardware to be used. This option is overridden if any of the above preconfigured boards are used.
// Common values are 16 (legacy) or 20 (JellyBeanDriver)
//
const uint8_t processorSpeed = 16;

// the below line sets the serial output speed
//
const unsigned long myBaudRate0 = 57600ul;	// (most Arduino boards)
const unsigned long myBaudRate1 = 19200ul;	// (Atmega2560 board)

// only one of the below LCD options may be chosen - choosing more than one will cause a compilation error to occur
// if useTinkerkitLCDmodule is used, useLegacyLCD will automatically be used, and the below options will be ignored
//
//#define useLegacyLCD true					// select Legacy 4-bit parallel LCD output
//#define useParallaxSerialLCDmodule true		// select Serial Interface LCD module output
#define useAdafruitRGBLCDshield true		// select Adafruit 4-bit RGB LCD module output
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
//#define useLegacyButtons true
//#define useAnalogMuxButtons true
//#define useParallax5PositionSwitch true

// serial logging options - mutually exclusive - either choose serial data logging or JSON output, but not both
// selecting both will result in a compilation error
//
//#define useJSONoutput true					// skybolt added to enable and call JSON out routine
//#define useSerialDataLogging true			// Ability to output 5 basic parameters to a data logger or SD card, once every refresh period (0.5 second)

// serial logging port options - choose one or the other with the above serial logging options, or an error will result
//
//#define useLoggingSerialPort0 true		// select serial logging output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLoggingSerialPort1 true		// select serial logging output on serial port channel 1 (Atmega2560 board)

// mutually exclusive options - you may choose one or the other, but not both
//
//#define useDeepSleep true					// places MPGuino into deep sleep after activity timeout
#define useSoftwareClock true				// Shows 24 hour clock driven off of timer0, and provides a means to set it

// mutually exclusive options - you may choose one or the other, but not both
//
//#define useChryslerMAPCorrection true		// Ability to perform on-the-fly fuel injector data correction for late-model Chrysler vehicles

// selectable options - all may be chosen independently of one another
//
#define useBigFE true						// Show big fuel economy displays
#define useBigDTE true						// Show big distance-to-empty displays
#define useBigTTE true						// Show big time-to-empty displays
//#define useBarFuelEconVsTime true			// Show Fuel Economy over Time bar graph
//#define useBarFuelEconVsSpeed true			// Show Fuel Economy vs Speed, Fuel Used vs Speed bar graphs
//#define useSavedTrips true					// Ability to save current or tank trips to any one of 10 different trip slots in EEPROM
//#define showSavedTrips true					// Ability to display trips saved in EEPROM, in conjunction with useSavedTrips above
//#define useDragRaceFunction true			// Performs "drag race" 0-60 MPH, 1/4 mile time, estimated horsepower functionality
//#define usePartialRefuel true				// Provide means to enter partial refuel amount into MPGuino
//#define useScreenEditor true				// Ability to change any of (8 to 14, depending on configuration) existing trip data screens, with 4 configurable figures on each screen
#define trackIdleEOCdata true				// Ability to track engine idling and EOC modes
#define useSpiffyBigChars true
//#define useCalculatedFuelFactor true		// Ability to calculate that pesky us/gal (or L) factor from easily available published fuel injector data
//#define useFuelCost true					// Show fuel cost
//#define useImperialGallon true				// when selected, uses Imperial gallons instead of default US gallons
//#define blankScreenOnMessage true			// Completely blank display screen upon display of message
//#define useExpansionPins true				// Generate analog 0-5VDC output voltage on expansion pins to drive LEDs or feed signal to external gauges

// performance enhancement options - all may be chosen independently of one another
//
#define useBufferedLCD true					// Speed up LCD output. Works with either useLegacyLCD or useParallaxSerialLCDmodule or useAdafruitRGBLCDshield or useTWILCD
//#define useBufferedSerial0Port true			// Speed up serial port 0 output
//#define useBufferedSerial1Port true			// Speed up serial port 1 output
#define useWindowTripFilter true			// Smooths out "jumpy" instant FE figures that are caused by modern OBDII engine computers
#define useAssemblyLanguage true			// Speeds up many low-level MPGuino functions
//#define useSWEET64mult true					// shift mult64 from native C++ to SWEET64 bytecode (saves 36 bytes)
//#define useSWEET64div true					// shift div64 from native C++ to SWEET64 bytecode

// options that are in progress at the moment - don't enable them unless you want to test them
//
//#define useBluetooth true					// Use bluetooth interface with Android phone in addition to or instead of LCD/buttons
//#define useABresultViewer true				// Ability to graphically show current (B) versus stored (A) fuel consumption rates
//#define useCoastDownCalculator true			// Ability to calculate C(rr) and C(d) from coastdown
//#define useFuelParamCalculator true			// Ability to calculate microseconds per gallon and fuel injector delay stored parameters
//#define useRealTimeClockModule true			// Ability to read and display Realtime Clock data from an SPI RTC module
//#define useCarVoltageOutput true			// Ability to display alternator voltage and optional secondary sensor (via meelis11)
//#define useTerminal true					// terminal interface between PC and MPGuino
//
//#define useTerminalSerialPort0 true			// select serial port channel 0 for PC-MPGuino terminal interface (most Arduino boards, excluding TinkerKit! LCD module)
//#define useTerminalSerialPort1 true			// select serial port channel 1 for PC-MPGuino terminal interface (Atmega2560 board)

// program measurement and debugging tools
//
//#define useSerialDebugOutput true			// enables serial output for debugging purposes
//#define useTestButtonValues true			// Allows observation of button mapping
#define useCPUreading true					// Show CPU loading and available RAM usage
//#define useBenchMark true					// this is possibly broken - last time I used it was in August 2013
#define useSimulatedFIandVSS true			// forces simulation of VSS and fuel injector events
//#define useSWEET64trace true				// Ability to view real-time 64-bit calculations from SWEET64 kernel
//#define forceEEPROMsettingsInit true		// forces EEPROM reset every time MPGuino is powered up
//#define useEEPROMviewer true				// Ability to directly examine EEPROM
#define useStatusLEDpins true				// allows usage of LEDs to show status of certain bits - currently used to indicate when MPGuino is awake
//#define useDebugValueReadout true			// shows the first screen as selected internal constants instead of MPGuino functions

// serial debugging output port options - choose one or the other with useSerialDebugOutput, or an error will result
//
//#define useDebuggingSerialPort0 true		// select serial debug output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useDebuggingSerialPort1 true		// select serial debug output on serial port channel 1 (Atmega2560 board)

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
#define useATmega2560 true
const uint8_t systemProcessorSpeed = 16;
#endif // useArduinoMega2560

#ifdef useTinkerkitLCDmodule
#define useATmega32U4 true
const uint8_t systemProcessorSpeed = 16;
#define useLegacyLCD true
#endif // useTinkerkitLCDmodule

#ifdef useJellyBeanDriverBoard
#define useATmega128 true
const uint8_t systemProcessorSpeed = 20;
#define useLegacyLCD true
#define useLegacyButtons true
#undef useParallaxSerialLCDmodule
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useAdafruitRGBLCDshield
#undef useStatusLEDpins
#endif // useJellyBeanDriverBoard

#ifdef useLegacyBoard
#define useATmega128 true
const uint8_t systemProcessorSpeed = 16;
#define useLegacyLCD true
#define useLegacyButtons true
#undef useParallaxSerialLCDmodule
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useAdafruitRGBLCDshield
#undef useStatusLEDpins
#endif // useLegacyBoard

#if not(useATmega32U4) && not(useATmega2560) && not(useATmega128)
#define useATmega128 true
const uint8_t systemProcessorSpeed = processorSpeed;
#endif // not(useATmega32U4) && not(useATmega2560) && not(useATmega128)

#if useATmega32U4 && useATmega2560 || useATmega32U4 && useATmega128
#error *** ATmega32U4 conflict with other hardware!!! ***
#endif // useATmega32U4 && useATmega2560 || useATmega32U4 && useATmega128

#if useATmega2560 && useATmega128
#error *** useATmega2560 conflict with other hardware!!! ***
#endif // useATmega2560 && useATmega128

#ifdef useSoftwareClock
#define useClockDisplay true
#define useBigTimeDisplay true
#endif // useSoftwareClock

#ifdef useBigTTE
#define useBigTimeDisplay true
#endif // useBigTTE

#ifdef useBigTimeDisplay
#define useBigNumberDisplay true
#endif // useBigTimeDisplay

#ifdef useBigDTE
#define useBigNumberDisplay true
#endif // useBigDTE

#ifdef useBigFE
#define useBigNumberDisplay true
#endif // useBigFE

#ifdef useCalculatedFuelFactor
#define useIsqrt true
#endif // useCalculatedFuelFactor

#ifdef useChryslerMAPCorrection
#define useIsqrt true
#define useAnalogRead true
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

#ifdef useSWEET64trace
#define useSerialDebugOutput true
#endif // useSWEET64trace

#ifdef useParallaxSerialLCDmodule
#define useSerialLCD true
#define useLCDoutput true
#define useBinaryLCDbrightness true
#endif // useParallaxSerialLCDmodule

#if not(useSWEET64trace) && not(useSerialDebugOutput)
#undef useDebuggingSerialPort0
#undef useDebuggingSerialPort1
#endif // not(useSWEET64trace) && not(useSerialDebugOutput)

#if not(useSerialLCD)
#undef useLCDserialPort0
#undef useLCDserialPort1
#endif // not(useSerialLCD)

#if not(useJSONoutput) && not(useSerialDataLogging)
#undef useLoggingSerialPort0
#undef useLoggingSerialPort1
#endif // not(useJSONoutput) && not(useSerialDataLogging)

#if not(useTerminal)
#undef useTerminalSerialPort0
#undef useTerminalSerialPort1
#endif // not(useTerminal)

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
#define devLogSerial devSerial0
#define useSerialLoggingOutput true
#ifdef useBufferedSerial0Port
#endif // useBufferedSerial0Port
#endif // useLoggingSerialPort0

#ifdef useLoggingSerialPort1
#define useSerial1Port true
#define devLogSerial devSerial1
#define useSerialLoggingOutput true
#ifdef useBufferedSerial1Port
#endif // useBufferedSerial1Port
#endif // useLoggingSerialPort1

#ifdef useTerminalSerialPort0
#define useSerial0Port true
#define devLogTerminal devSerial0
#define useTerminalOutput true
#ifdef useBufferedSerial0Port
#endif // useBufferedSerial0Port
#endif // useTerminalSerialPort0

#ifdef useTerminalSerialPort1
#define useSerial1Port true
#define devLogTerminal devSerial1
#define useTerminalOutput true
#ifdef useBufferedSerial1Port
#endif // useBufferedSerial1Port
#endif // useTerminalSerialPort1

#ifdef useDebuggingSerialPort0
#define useSerial0Port true
#define devDebugSerial devSerial0
#ifdef useBufferedSerial0Port
#endif // useBufferedSerial0Port
#endif // useDebuggingSerialPort0

#ifdef useDebuggingSerialPort1
#define useSerial1Port true
#define devDebugSerial devSerial1
#ifdef useBufferedSerial1Port
#endif // useBufferedSerial1Port
#endif // useDebuggingSerialPort1

#if useBufferedSerial0Port || useBufferedSerial1Port || useBufferedLCD
#define useBuffering true
#endif // useBufferedSerial0Port || useBufferedSerial1Port || useBufferedLCD

#ifdef useBenchMark
#define useCPUreading true
#endif // useBenchMark

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
#define useTimer1 true
#define useLCDoutput true
#endif // use4BitLCD

#ifdef useAnalogButtons
#define useAnalogRead true
#endif // useAnalogButtons

#ifdef useAnalogRead
#define useAnalogInterrupt true
#endif // useAnalogRead

#ifdef useSimulatedFIandVSS
#define useTimer1 true
#endif // useSimulatedFIandVSS

#ifdef useExpansionPins
#ifdef useATmega32U4
#ifdef useTinkerkitLCDmodule
#define useTimer4 true
#else // useTinkerkitLCDmodule
// non-Tinkerkit LCD ATmega32U4 stuff goes here
#endif // useTinkerkitLCDmodule
#endif // useATmega32U4
#ifdef useATmega2560
#define useTimer2 true
#endif // useATmega2560
#ifdef useATmega128
#define useTimer1 true
#define useTimer2 true
#endif // useATmega128
#endif // useExpansionPins

#ifdef useBarFuelEconVsTime
#define useBarGraph true
#endif // useBarFuelEconVsTime

#ifdef useBarFuelEconVsSpeed
#define useBarGraph true
#endif // useBarFuelEconVsSpeed

#ifdef useDragRaceFunction
#define useVehicleMass true
#endif // useDragRaceFunction

#ifdef useSavedTrips
#define useEEPROMtripStorage true
#endif // useSavedTrips

// this section catches configuration errors

#ifdef useTWIsupport
#ifdef useLegacyButtons
#ifndef useATmega2560
#error *** CANNOT configure for Legacy pushbuttons and TwoWire support!!! ***
#endif // useATmega2560
#endif // useLegacyButtons
#endif // useTWIsupport

#if useLCDserialPort0 && useLoggingSerialPort0 || useLCDserialPort0 && useTerminalSerialPort0 || useLCDserialPort0 & useDebuggingSerialPort0
#error *** Serial Port 0 conflict exists between Serial LCD and something else!!! ***
#endif // useLCDserialPort0 && useLoggingSerialPort0 || useLCDserialPort0 && useTerminalSerialPort0 || useLCDserialPort0 & useDebuggingSerialPort0

#if useLoggingSerialPort0 & useTerminalSerialPort0 || useLoggingSerialPort0 && useDebuggingSerialPort0
#error *** Serial Port 0 conflict exists between Serial Logging and something else!!! ***
#endif // useLoggingSerialPort0 & useTerminalSerialPort0 || useLoggingSerialPort0 && useDebuggingSerialPort0

#if useTerminalSerialPort0 && useDebuggingSerialPort0
#error *** Serial Port 0 conflict exists between Terminal and something else!!! ***
#endif // useTerminalSerialPort0 && useDebuggingSerialPort0

#if useLCDserialPort1 && useLoggingSerialPort1 || useLCDserialPort1 && useTerminalSerialPort1 || useLCDserialPort1 & useDebuggingSerialPort1
#error *** Serial Port 1 conflict exists between Serial LCD and something else!!! ***
#endif // useLCDserialPort1 && useLoggingSerialPort1 || useLCDserialPort1 && useTerminalSerialPort1 || useLCDserialPort1 & useDebuggingSerialPort1

#if useLoggingSerialPort1 & useTerminalSerialPort1 || useLoggingSerialPort1 && useDebuggingSerialPort1
#error *** Serial Port 1 conflict exists between Serial Logging and something else!!! ***
#endif // useLoggingSerialPort1 & useTerminalSerialPort1 || useLoggingSerialPort1 && useDebuggingSerialPort1

#if useTerminalSerialPort1 && useDebuggingSerialPort1
#error *** Serial Port 1 conflict exists between Terminal and something else!!! ***
#endif // useTerminalSerialPort1 && useDebuggingSerialPort1

#if defined(useJSONoutput) && defined(useSerialDataLogging)
#error *** CANNOT use Serial Logging Output for both JSON output and Serial Data Logging!!! ***
#endif // defined(useJSONoutput) && defined(useSerialDataLogging)

#ifdef useSerialLCD
#if useLCDserialPort0 && useLCDserialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial LCD Output!!! ***
#endif // useLCDserialPort0 && useLCDserialPort1
#if not(useLCDserialPort0) && not(useLCDserialPort1)
#error *** Serial LCD Output needs a serial port assigned to it!!! ***
#endif // not(useLCDserialPort0) && not(useLCDserialPort1)
#endif // useSerialLCD

#ifdef useTerminal
#if useTerminalSerialPort0 && useTerminalSerialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Terminal Output!!! ***
#endif // useTerminalSerialPort0 && useTerminalSerialPort1
#if not(useTerminalSerialPort0) && not(useTerminalSerialPort1)
#error *** Terminal Output needs a serial port assigned to it!!! ***
#endif // not(useTerminalSerialPort0) && not(useTerminalSerialPort1)
#endif // useTerminal

#if useLoggingSerialPort0 && useLoggingSerialPort1
#ifdef useJSONoutput
#error *** CANNOT use Serial Port 0 and Serial Port 1 for JSON Output!!! ***
#endif // useJSONoutput
#ifdef useSerialDataLogging
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial Logging Output!!! ***
#endif // useSerialDataLogging
#endif // useLoggingSerialPort0 && useLoggingSerialPort1
#if not(useLoggingSerialPort0) && not(useLoggingSerialPort1)
#ifdef useJSONoutput
#error *** JSON Output needs a serial port assigned to it!!! ***
#endif // useJSONoutput
#ifdef useSerialDataLogging
#error *** Serial Logging Output needs a serial port assigned to it!!! ***
#endif // useSerialDataLogging
#endif // not(useLoggingSerialPort0) && not(useLoggingSerialPort1)

#ifdef useSerialDebugOutput
#if useDebuggingSerialPort0 && useDebuggingSerialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial Debugging Output!!! ***
#endif // useDebuggingSerialPort0 && useDebuggingSerialPort1
#if not(useDebuggingSerialPort0) && not(useDebuggingSerialPort1)
#error *** Serial Debugging Output needs a serial port assigned to it!!! ***
#endif // not(useDebuggingSerialPort0) && not(useDebuggingSerialPort1)
#endif // useSerialDebugOutput

#ifdef useSerial1Port
#ifdef useATmega128
#error *** Cannot use ATmega328/128 hardware for Serial Port 1!!! ***
#endif // useATmega128
#ifdef useTinkerkitLCDmodule
#error *** Serial Port 1 on Tinkerkit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // useTinkerkitLCDmodule
#endif // useSerial1Port

#if useSerial0Port && useATmega32U4
#error *** Cannot use Tinkerkit! LCD module/ATmega32u4 hardware for Serial Port 0!!! ***
#endif // useSerial0Port && useTinkerkitLCDmodule

// Really, dcb? Really? You couldn't have used something else for LCD data bit 3?
#if useLegacyLCD && useATmega128 && useStatusLEDpins
#error *** CANNOT use built-in LED pins and Legacy LCD configuration!!! ***
#endif // useLegacyLCD && useATmega128 && useStatusLEDpins

#if useParallax5PositionSwitch && useLegacyButtons || useParallax5PositionSwitch && useAnalogMuxButtons
#error *** Parallax 5-position switch conflict with other buttons!!! ***
#endif // useParallax5PositionSwitch && useLegacyButtons || useParallax5PositionSwitch && useAnalogMuxButtons

#if useLegacyButtons && useAnalogMuxButtons
#error *** Legacy pushbuttons conflict with other buttons!!! ***
#endif // useLegacyButtons && useAnalogMuxButtons

#if useSoftwareClock && useDeepSleep
#error *** CANNOT use both Software Clock and Deep Sleep functionality!!! ***
#endif // useSoftwareClock && useDeepSleep

#if useCarVoltageOutput && useChryslerMAPCorrection
#error *** CANNOT use both Car Voltage reading and Chrysler MAP sensor fuel pressure correction!!! ***
#endif // useCarVoltageOutput && useChryslerMAPCorrection

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
const uint8_t buttonCbit = 			0b00001000;
const uint8_t buttonRbit = 			0b00000100;
const uint8_t buttonUbit = 			0b00000010;
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
#ifdef useATmega2560
const uint8_t buttonLbit = 			(1 << PINK3);
const uint8_t buttonCbit = 			(1 << PINK4);
const uint8_t buttonRbit = 			(1 << PINK5);

const uint8_t longButtonBit =		(1 << PINK6); // PINK6 isn't being used for anything right now

#endif // useATmega2560
#ifdef useATmega128
const uint8_t buttonLbit = 			(1 << PINC3);
const uint8_t buttonCbit = 			(1 << PINC4);
const uint8_t buttonRbit = 			(1 << PINC5);

const uint8_t longButtonBit =		(1 << PINC6); // PINC6 is used as the RESET pin, so this value is safe to use for long-press signalling

#endif // useATmega128
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
#ifdef useTestButtonValues
volatile uint8_t thisButtonIdx;
volatile uint8_t lastButtonIdx;

#endif // useTestButtonValues
#ifdef useATmega32U4
#ifndef PRTIM4
#define PRTIM4 4

#endif // PRTIM4
#endif // useATmega32U4
typedef void (* pFunc)(void); // type for display function pointers
typedef uint8_t (* handlerFunc)(uint8_t, uint8_t); // type for various handler functions
typedef uint8_t (* pSaveFunc)(void);

const uint8_t loopsPerSecond = 2; // how many times will we try and loop in a second
#ifdef useAnalogRead
const uint8_t analogSamplesPerSecond = 50; // how many times will we try to sample the ADC output in a second
#endif // useAnalogRead

const unsigned long t0CyclesPerSecond = (unsigned long)(systemProcessorSpeed * 15625ul); // (systemProcessorSpeed * 1000000 / (timer 0 prescaler))

const unsigned int loopTickLength = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(loopsPerSecond * 256ul)) - 1;
const unsigned int buttonDelayTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul)); // 1 second delay
const unsigned int buttonShortDelayTick = buttonDelayTick - (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 20ul)); // 50 millisecond delay before accepting button presses
const unsigned int cursorDelay = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 2ul)) - 1; // display cursor toggle period is 500 milliseconds
const unsigned int swapFEwithFCRDelay = (unsigned int)(ceil)((double)(3ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 3 second delay
const unsigned int holdDelay = (unsigned int)(ceil)((double)(2ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 2 second delay
const unsigned int delay1500msTick = (unsigned int)(ceil)((double)(15ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.5 second delay
const unsigned int delay0005msTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 200ul)) - 1; // 5 millisecond delay
#ifdef useCPUreading
const unsigned long loopSystemLength = (unsigned long)(ceil)((double)(t0CyclesPerSecond) / (double)(loopsPerSecond));
#endif // useCPUreading
#ifdef useAnalogRead
const unsigned int analogSampleTickLength  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(analogSamplesPerSecond * 256ul)) - 1;
#endif // useAnalogRead
#ifdef useTWIsupport
const unsigned int TWItickLength = (unsigned int)(ceil)((double)(3ul * t0CyclesPerSecond) / (double)(256ul * 100ul)) - 1; // 30 millisecond delay
#endif // useTWIsupport
#ifdef useJSONoutput
const unsigned int JSONtickLength = (unsigned int)(ceil)((double)(16ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.6 second delay
#endif // useJSONoutput

union union_16
{

	unsigned int ui;
	uint8_t u8[2];

};

union union_32
{

	uint32_t ul;
	unsigned int ui[2];
	uint8_t u8[4];

};

union union_64
{

	uint64_t ull;
	uint32_t ul[2];
	uint16_t ui[4];
	uint8_t u8[8];

};

uint8_t odvFlagCRLF =				0b00001000;
uint8_t odvFlagSupportsDelay =		0b00000100;
uint8_t odvFlagShootBlanks =		0b00000010;
uint8_t odvFlagEnableOutput =		0b00000001;

typedef struct
{

	uint8_t xPos;
	uint8_t yPos;
	uint8_t xMax;
	uint8_t controlFlags;
	void (* gotoXY)(uint8_t xPos, uint8_t yPos);
	void (* charOut)(uint8_t character);

} outputDevice;

typedef struct
{

	uint8_t localTripIdx;
	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t calcFmtIdx;
	uint8_t translateFlag;
	uint8_t decimalPlaces;
	uint8_t calcChar;

} calcFuncObj;

static void updateVSS(unsigned long thisVSStime);
#ifdef useIsqrt
static unsigned int iSqrt(unsigned int n);
#endif // useIsqrt
unsigned long findCycleLength(unsigned long lastCycle, unsigned long thisCycle);
void delay0(unsigned int ms);
void initStatusLine(void);
void execStatusLine(void);
const char * findStr(const char * str, uint8_t strIdx);
#ifdef useEEPROMtripStorage
uint8_t getBaseTripPointer(uint8_t tripSlot);
#endif // useEEPROMtripStorage
uint8_t SWEET64_fetchbyte(const uint8_t * &prgmPtr);
unsigned long SWEET64(const uint8_t * sched, uint8_t tripIdx);
void copy64(union union_64 * an, union union_64 * ann);
void tripVarLoad64(union union_64 * an, uint8_t tripIdx, uint8_t dataIdx);
void init64(union union_64 * an, unsigned long dWordL);
void swap64(union union_64 * an, union union_64 * ann);
void shr64(union union_64 * an);
void shl64(union union_64 * an);
void adc64(union union_64 * an, union union_64 * ann);
void sbc64(union union_64 * an, union union_64 * ann);
void cmp64(union union_64 * an, union union_64 * ann);
#ifndef useAssemblyLanguage
void adc64(union union_64 * an, union union_64 * ann, uint8_t mode);
#endif // useAssemblyLanguage
#ifndef useSWEET64mult
void mult64(void);
#endif // useSWEET64mult
#ifndef useSWEET64div
void div64(void);
#endif // useSWEET64div
void registerTest64(union union_64 * an);
static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx);
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
#ifdef useBarGraph
void clearBGplot(uint8_t mode);
uint8_t bgPlotConvert(uint8_t coord);
void bgPlot(uint8_t idx, uint8_t lowerPoint, uint8_t upperPoint, uint8_t mode);
void bgOutputPlot(uint8_t idx, uint8_t yIdx);
uint8_t bgConvert(unsigned long v, unsigned long ll, unsigned long d);
void formatBarGraph(uint8_t bgSize, uint8_t slotIdx, unsigned long centerVal, unsigned long topLimit);
void displayBarGraphLine(uint8_t lineNum, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripIdxChar);
#endif // useBarGraph
unsigned long doCalculate(uint8_t tripIdx, uint8_t calcIdx);
char findDigit(uint8_t value, char &zeroChar);
unsigned long str2ull(char * strBuffer);
char * ull2str(const uint8_t * prgmPtr, char * strBuffer, uint8_t decimalPlaces);
char * formatDecimal(const uint8_t * prgmPtr, char * strBuffer, uint8_t windowLength, uint8_t decimalPlaces, uint8_t decimalFlag);
static char * doFormat(calcFuncObj &thisCalcFuncObj, uint8_t windowLength, uint8_t decimalFlag);
unsigned long convertTime(unsigned long * an);
static void printStatusMessage(const char * s);
static void printStatusMessage(const char * s, uint8_t strIdx);
void doNothing(void);
void noSupport(void);
uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos); /* Main screen section */
void doNextBright(void);
void doTripResetTank(void);
void doTripResetCurrent(void);
#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void);
#endif // useDeepSleep
#ifdef useBarFuelEconVsSpeed // histograph of fuel economy vs. speed support section
void doCursorUpdateBarFEvS(void);
void doBarFEvSdisplay(void);
void doResetBarFEvS(void);
#endif // useBarFuelEconVsSpeed
#ifdef useBarFuelEconVsTime // histograph of fuel economy over time support section
void doResetBarFEvT(void);
void doCursorUpdateBarFEvT(void);
void doBarFEvTdisplay(void);
#endif // useBarFuelEconVsTime
#ifdef useSavedTrips // Trip save/restore/raw data view support section
void doTripSaveDisplay(void);
void tripSaveDisplayOut(const char * menuString);
void doGoTripSaveCurrent(void);
void doGoTripSaveTank(void);
void doTripSelect(void);
void doTripLongSelect(void);
#ifdef showSavedTrips
void doTripShowDisplay(void);
#endif // showSavedTrips
void tripDisplayStatus(uint8_t tripSlot, const char * str);
uint8_t doTripAutoAction(uint8_t taaMode);
#endif // useSavedTrips
#ifdef usePartialRefuel // Partial Refuel support section
void doPartialRefuelDisplay(void);
void doPartialFuelSelect(void);
void doPartialFuelLongSelect(void);
void doGoPartialRefuel(void);
#endif // usePartialRefuel
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
void doOutputNumberJSON(const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces, const char * labelStr);
void doOutputNumberJSON(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalPlaces, const char * labelStr);
void doOutputNumberJSON(unsigned long an, uint8_t decimalPlaces, const char * labelStr);
void doOutputNumberJSON(union union_64 * an, uint8_t decimalPlaces, const char * labelStr);
void doOutputNumberJSON(uint8_t decimalPlaces, const char * labelStr);
void doOutputJSON(void);
#endif // useJSONoutput
#ifdef useSerialDataLogging
void doOutputDataLog(void);
#endif // useSerialDataLogging
static void idleProcess(void); // place all time critical main program internal functionality here - no I/O!
void performSleepMode(uint8_t sleepMode);
int main(void);

uint8_t menuLevel;
uint8_t topScreenLevel;
uint8_t brightnessIdx;

namespace peripheral /* MPGuino human interface I/O peripheral device prototype */
{

	void initMain(void);
	void shutdownMain(void);
	void initButton(void);
	void shutdownButton(void);
#ifdef useExpansionPins
	void initExpansionPin(void);
	void shutdownExpansionPin(void);
	void outputExpansionPin1(uint8_t val);
	void outputExpansionPin2(uint8_t val);
#endif // useExpansionPins
#ifdef useStatusLEDpins
	void initStatusLED(void);
	void shutdownStatusLED(void);
	void outputStatusLED(uint8_t val);
#endif // useStatusLEDpins

};

namespace EEPROM /* EEPROM parameter I/O section prototype */
{

#ifdef useEEPROMviewer
	static void display(void);
#endif // useEEPROMviewer
	static uint8_t powerUpCheck(void);
	static void initGuino(void);
	static uint8_t writeVal(uint8_t eePtr, unsigned long val);
	static void readValToString(uint8_t parameterPtr, char * strBuffer);
	static unsigned long readVal(uint8_t eePtr);
	static unsigned int getAddress(uint8_t eePtr);
	static void changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR);

};

namespace cursor /* LCD screen cursor manipulation section prototype */
{

	static void screenLevelEntry(const char * s, uint8_t newScreenLevel);
	static void moveAbsolute(uint8_t positionY, uint8_t positionX);
	static void moveRelative(uint8_t moveY, uint8_t moveX);
	static void updateAfterMove(uint8_t levelChangeFlag);
	static void shortLeft(void);
	static void shortRight(void);
	static void longLeft(void);
	static void longRight(void);

};

const uint8_t menuExitIdx =				0;
const uint8_t menuEntryIdx =			menuExitIdx + 1;				// typically, this call will fall through
const uint8_t menuCursorUpdateIdx =		menuEntryIdx + 1;				// ...to this call, then will fall through
const uint8_t menuOutputDisplayIdx =	menuCursorUpdateIdx + 1;		// ...to this call

typedef uint8_t (* menuEventHandler)(uint8_t cmd); // type for menu event handler pointer

namespace menu /* Top-down menu selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void doEnter(void);
	static void select(void);

};

typedef uint8_t (* numberEditSupportFunc)(uint8_t cmd); // type for number editor support function pointer

const uint8_t nesLoadInitial =			0;
const uint8_t nesLoadValue =			nesLoadInitial + 1;
const uint8_t nesOnChange =				nesLoadValue + 1;
const uint8_t nesCheckMaxValue =		nesOnChange + 1;
const uint8_t nesFetchMinValue =		nesCheckMaxValue + 1;
const uint8_t nesFetchMaxValue =		nesFetchMinValue + 1;
const uint8_t nesFetchPowerUpValue =	nesFetchMaxValue + 1;
const uint8_t nesSaveNumber =			nesFetchPowerUpValue + 1;
const uint8_t nesOutputUpperScreen =	nesSaveNumber + 1;

typedef struct
{

	uint8_t callingScreenLevel; // remember what module called number editor
	uint8_t * neBuffer; // storage pointer to number string workspace
	const char * neStatusMessage;
	numberEditSupportFunc numberEditMethod;

} numberEditType;

static numberEditType numberEditObj;

namespace numberEdit /* number editor/entry section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void goEditNumber(void);
	static void findLeft(void);
	static void findRight(void);
	static void revert(void);
	static void readMinValue(void);
	static void readMaxValue(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void changeDigit(uint8_t dir);
	static void saveEdit(void);
	static void cancelEdit(void);

};

namespace settings /* Parameter setting selector section prototype */
{

	static uint8_t numberEditorFunc(uint8_t cmd);
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);

};

namespace text /* text string output prototype */
{

	void gotoXY(outputDevice &dev, uint8_t x, uint8_t y);
	uint8_t charOut(outputDevice &dev, uint8_t chr);

	void blinkMode(outputDevice &dev, uint8_t condition);
	void stringOut(outputDevice &dev, char * str);
	void stringOut(outputDevice &dev, const char * str);
	void stringOut(outputDevice &dev, const char * str, uint8_t strIdx);
	void clearEOL(outputDevice &dev);
	void hexNybbleOut(outputDevice &dev, uint8_t val);
	void hexByteOut(outputDevice &dev, uint8_t val);
	void hexWordOut(outputDevice &dev, unsigned int val);
	void hexDWordOut(outputDevice &dev, unsigned long val);
	void hexLWordOut(outputDevice &dev, unsigned long long val);

};

#ifdef useClockDisplay
namespace clockDisplay /* Big Clock Display support section prototype */
{

	static void goSetClock(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void setClock(void);
	static void cancelClockSet(void);

};

#endif // useClockDisplay
#ifdef useBigNumberDisplay
namespace bigNumber /* Big Number output support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void loadCGRAMnumberFont(void);
#ifdef useBigTimeDisplay
	static void outputTime(char * val, uint8_t blinkFlag, uint8_t blinkPos);
#endif // useBigTimeDisplay
	static void outputNumber(const uint8_t * prgmPtr, uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalPlaces);
	static void outputNumberString(char * str);
	static void outputDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar);
	static void displayStatus(const char * str, uint8_t cursorPos);

};

#endif // useBigNumberDisplay
#ifdef useCPUreading
namespace systemInfo /* CPU loading and RAM availability support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void showCPUload(void);
	static void showCPUloading(void);
	static unsigned int getAvailableRAMsize(void);
	static unsigned long cycles0(void);
#ifdef useBenchMark
	static void doBenchMark(void);
#endif // useBenchMark

};

extern char __bss_end;
extern char *__brkval;

#endif // useCPUreading
#ifdef useSimulatedFIandVSS
namespace debugReading /* VSS / fuel injector on-board simulator support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void configurePorts(void);

}

#endif // useSimulatedFIandVSS
#ifdef useDebugValueReadout
namespace debugValueDisplay /* internal volatile variable display support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static uint8_t getSWEET64flags(uint8_t testVal);

}

#endif // useDebugValueReadout
#ifdef usePartialRefuel
namespace partialRefuel /* partial refuelling support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);

}

#endif // usePartialRefuel
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

	void init(ringBufferVariable &bfr, volatile uint8_t * storage);
	uint8_t isBufferNotEmpty(ringBufferVariable &bfr);
	void push(ringBufferVariable &bfr, uint8_t value);
	uint8_t pull(ringBufferVariable &bfr);
	void flush(ringBufferVariable &bfr);

};

#endif // useBuffering
#ifdef useTWIsupport
#include <compat/twi.h>
namespace TWI
{

	void init(void);
	void shutdown(void);
	void setAddress(uint8_t address);
	uint8_t transmit(const uint8_t*, uint8_t);
	uint8_t readDevice(uint8_t address, uint8_t * data, uint8_t length, uint8_t sendStop);
	void initWrite(uint8_t address, uint8_t sendStop, uint8_t interruptFlag);
	uint8_t writeByte(uint8_t data);
	void commitWrite(void);
	void commitWrite(uint8_t waitFlag);
	uint8_t writeDevice(uint8_t address, uint8_t * data, uint8_t length, uint8_t waitFlag, uint8_t sendStop);
	void initMasterDevice(uint8_t address, uint8_t writeFlag, uint8_t sendStop, uint8_t interruptFlag);
	uint8_t commitMasterDevice(uint8_t waitFlag);
	void reply(uint8_t);
	void stopOrRepeatStart(void);
	void hardStop(void);
	void releaseBus(void);
	void (* onSlaveTransmit)(void);
	void (* onSlaveReceive)(volatile uint8_t *, uint8_t);

}

#endif // useTWIsupport
#ifdef useAdafruitRGBLCDshield
namespace adafruitRGBLCDsupport
{

	void init(void);
	void writeRegister16Bit(uint8_t interruptFlag, uint8_t registerAddress, union union_16 * registerValue);
	void writeRegister16Bit(uint8_t interruptFlag, uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte);
	void writeRegister8Bit(uint8_t interruptFlag, uint8_t registerAddress, uint8_t portByte);
	void setTransferMode(uint8_t interruptFlag, uint8_t mode);

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

	void init(void);
	void shutdown(void);
	void charOut(uint8_t chr);

};

const unsigned int myubbr0 = (unsigned int)(ceil)((double)(systemProcessorSpeed * 62500ul) / (double)(myBaudRate0)) - 1;

outputDevice devSerial0;

#ifdef useBufferedSerial0Port
ringBufferVariable serial0Buffer;

volatile uint8_t serial0Data[32];

#endif // useBufferedSerial0Port
#endif // useSerial0Port
#ifdef useSerial1Port
namespace serial1
{

	void init(void);
	void shutdown(void);
	void charOut(uint8_t chr);

};

const unsigned int myubbr1 = (unsigned int)(ceil)((double)(systemProcessorSpeed * 62500ul) / (double)(myBaudRate1)) - 1;

outputDevice devSerial1;

#ifdef useBufferedSerial1Port
ringBufferVariable serial1Buffer;

volatile uint8_t serial1Data[32];

#endif // useBufferedSerial1Port
#endif // useSerial1Port
#ifdef useLCDoutput
namespace LCD // LCD prototype
{

	static void init(void);
	static void shutdown(void);
	static void gotoXY(uint8_t x, uint8_t y);
	static void loadCGRAMcharacter(uint8_t chr, char * chrData);
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
#ifdef useTWILCD
	static void disableIntSample(void);
	static void enableIntSample(void);
#endif // useTWILCD
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
#ifdef useATmega32U4
const uint8_t lcdDirection =		(1 << PORTF0); // on PORTF, A5, Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	(1 << PORTF1); // on PORTF, A4
const uint8_t lcdEnable =			(1 << PORTE6); // on PORTE, 7
const uint8_t lcdBit3 =				(1 << PORTB4); // on PORTB, 8
const uint8_t lcdBit2 =				(1 << PORTD6); // on PORTD, 12
const uint8_t lcdBit1 =				(1 << PORTD4); // on PORTD, 4
const uint8_t lcdBit0 =				(1 << PORTF4); // on PORTF, A3
const uint8_t lcdBrightness =		(1 << DDB6); // on PORTB, 10
const uint8_t lcdContrast =			(1 << DDB5); // on PORTB, 9

#endif // useATmega32U4
#ifdef useATmega2560
const uint8_t lcdRegisterSelect =	(1 << PORTA4); // on PORTA
const uint8_t lcdEnable =			(1 << PORTA5); // on PORTA
const uint8_t lcdBit3 =				(1 << PORTA0); // on PORTA
const uint8_t lcdBit2 =				(1 << PORTA1); // on PORTA
const uint8_t lcdBit1 =				(1 << PORTA2); // on PORTA
const uint8_t lcdBit0 =				(1 << PORTA3); // on PORTA
const uint8_t lcdBrightness =		(1 << DDB5); // on PORTB
const uint8_t lcdContrast =			(1 << DDB7); // on PORTB

#endif // useATmega2560
#ifdef useATmega128
const uint8_t lcdRegisterSelect =	(1 << PORTD4); // on PORTD, 4
const uint8_t lcdEnable =			(1 << PORTD5); // on PORTD, 5
const uint8_t lcdBit3 =				(1 << PORTB5); // on PORTB, 13
const uint8_t lcdBit2 =				(1 << PORTB4); // on PORTB, 12
const uint8_t lcdBit1 =				(1 << PORTB0); // on PORTB, 8
const uint8_t lcdBit0 =				(1 << PORTD7); // on PORTD, 7
const uint8_t lcdBrightness =		(1 << DDB1); // on PORTB, 9
const uint8_t lcdContrast =			(1 << DDD6); // on PORTD, 6

#endif // useATmega128
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
#endif // use4BitLCD
#ifdef useLegacyLCD
#ifdef useInvertedLegacyLCDbrightness
const uint8_t brightness[] PROGMEM = { 255, 214, 171, 127 }; //middle button cycles through these brightness settings
#else // useInvertedLegacyLCDbrightness
const uint8_t brightness[] PROGMEM = { 0, 41, 84, 128 }; //middle button cycles through these brightness settings
#endif // useInvertedLegacyLCDbrightness

const char brightString[] PROGMEM = {
	" OFF\0"
	" LOW\0"
	" MED\0"
	"HIGH\0"
};

const uint8_t brightnessLength = ( sizeof(brightness) / sizeof(uint8_t) ); // size of brightness table

#endif // useLegacyLCD
#ifdef useBinaryLCDbrightness
const uint8_t brightnessLength = 2;
const char brightString[] PROGMEM = {
	" OFF\0"
	"  ON\0"
};

#endif // useBinaryLCDbrightness
#endif // useLCDoutput
// SWEET64 is a low-level pseudocode interpreter, meant to save a lot of space in program memory for 64-bit calculations
//
// the opcode definitions below provide means of manipulating MPGuino trip variables, interacting with calculated and EEPROM parameters, and providing basic
// number formatting for interaction with string representations of numbers
//
// SWEET64 is inspired by an early pseudocode processor named SWEET16, which was written by Steve Wozniak of Apple Computer back in 1977. SWEET16 was written to
//    perform 16-bit machine language instructions while running on an 8-bit microprocessor
//
uint8_t SWEET64processorFlags;

// x < y        - carry flag is clear, zero flag is clear
// x == y       - carry flag is set, zero flag is set
// x > y        - carry flag is set, zero flag is clear
//
// (x - y) >= 0 - minus flag is clear
// (x - y) < 0	- minus flag is set
//
const uint8_t SWEET64carryFlag =		0b00000001;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64zeroFlag =			0b00000010;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64minusFlag =		0b00000100;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64divisionFlag =		0b00100000;			// this is an intermediate division flag used internally by div64()
const uint8_t SWEET64traceCommandFlag =	0b01000000;			// this flag is ignored if #useSWEET64trace is not used
const uint8_t SWEET64traceFlag =		0b10000000;			// this flag is ignored if #useSWEET64trace is not used

const uint8_t SWEET64traceFlagGroup =	SWEET64traceCommandFlag | SWEET64traceFlag;

uint64_t s64reg[5];

const uint8_t s64reg1 = 0;	// general purpose
const uint8_t s64reg2 = 1;	// output value / general purpose
const uint8_t s64reg3 = 2;	// general purpose / temporary storage
const uint8_t s64reg4 = 3;	// used in multiply, divide operations
const uint8_t s64reg5 = 4;	// used in multiply, divide operations

#define nextAllowedValue 0
const uint8_t DNUISinstrTestMetricFlag =		nextAllowedValue;							// test metricFlag bits
const uint8_t DNUISinstrTestReg =				DNUISinstrTestMetricFlag + 1;				// tests specified register for zero condition or high bit set
const uint8_t DNUISinstrTestIndex =				DNUISinstrTestReg + 1;						// tests index for zero condition or high bit set
const uint8_t DNUISinstrCmpReg =				DNUISinstrTestIndex + 1;					// compares register X to register Y
const uint8_t DNUISinstrCmpIndex =				DNUISinstrCmpReg + 1;						// compares operand (X) to index (Y)

const uint8_t DNUISinstrBranchIfMclear =		DNUISinstrCmpIndex + 1;						// branches if result most significant bit is clear
const uint8_t DNUISinstrBranchIfMset =			DNUISinstrBranchIfMclear + 1;				// branches if result most significant bit is set
const uint8_t DNUISinstrBranchIfZclear =		DNUISinstrBranchIfMset + 1;					// branches if result zero flag is clear (X != Y or X != 0)
const uint8_t DNUISinstrBranchIfZset =			DNUISinstrBranchIfZclear + 1;				// branches if result zero flag is set (X == Y or X == 0)
const uint8_t DNUISinstrBranchIfCclear =		DNUISinstrBranchIfZset + 1;					// branches if result carry flag is clear (X <= Y)
const uint8_t DNUISinstrBranchIfCset =			DNUISinstrBranchIfCclear + 1;				// branches if result carry flag is set (X > Y)
const uint8_t DNUISinstrBranchIfLT =			DNUISinstrBranchIfCset + 1;					// branches if X < Y or index < operand
const uint8_t DNUISinstrBranchIfGTorE =			DNUISinstrBranchIfLT + 1;					// branches if X >= Y or index >= operand
const uint8_t DNUISinstrSkip =					DNUISinstrBranchIfGTorE + 1;				// branches always
const uint8_t DNUISinstrCall =					DNUISinstrSkip + 1;
const uint8_t DNUISinstrJump =					DNUISinstrCall + 1;

const uint8_t DNUISinstrLdReg =					DNUISinstrJump + 1;
const uint8_t DNUISinstrLdRegByteFromIndex =	DNUISinstrLdReg + 1;
const uint8_t DNUISinstrLdRegByte =				DNUISinstrLdRegByteFromIndex + 1;
const uint8_t DNUISinstrLdRegByteFromYindexed =	DNUISinstrLdRegByte + 1;
const uint8_t DNUISinstrLdRegTripVar =			DNUISinstrLdRegByteFromYindexed + 1;
const uint8_t DNUISinstrLdRegTripVarIndexed =	DNUISinstrLdRegTripVar + 1;
const uint8_t DNUISinstrLdRegConst =			DNUISinstrLdRegTripVarIndexed + 1;
const uint8_t DNUISinstrStRegByteToIndexedU8 =	DNUISinstrLdRegConst + 1;
const uint8_t DNUISinstrDoBCDadjust =			DNUISinstrStRegByteToIndexedU8 + 1;
const uint8_t DNUISinstrLdRegEEPROM =			DNUISinstrDoBCDadjust + 1;
const uint8_t DNUISinstrLdRegEEPROMindexed =	DNUISinstrLdRegEEPROM + 1;
const uint8_t DNUISinstrLdRegEEPROMindirect =	DNUISinstrLdRegEEPROMindexed + 1;
const uint8_t DNUISinstrStRegEEPROM =			DNUISinstrLdRegEEPROMindirect + 1;
const uint8_t DNUISinstrStRegEEPROMindirect =	DNUISinstrStRegEEPROM + 1;
const uint8_t DNUISinstrLdRegVolatile =			DNUISinstrStRegEEPROMindirect + 1;
const uint8_t DNUISinstrLdRegVolatileIndexed =	DNUISinstrLdRegVolatile + 1;
const uint8_t DNUISinstrStRegVolatile =			DNUISinstrLdRegVolatileIndexed + 1;
const uint8_t DNUISinstrStRegVolatileIndexed =	DNUISinstrStRegVolatile + 1;
const uint8_t DNUISinstrLxdI =					DNUISinstrStRegVolatileIndexed + 1;
const uint8_t DNUISinstrLxdIEEPROM =			DNUISinstrLxdI + 1;
const uint8_t DNUISinstrLxdIEEPROMindexed =		DNUISinstrLxdIEEPROM + 1;
const uint8_t DNUISinstrLdRegNumer =			DNUISinstrLxdIEEPROMindexed + 1;
const uint8_t DNUISinstrLdRegDenom =			DNUISinstrLdRegNumer + 1;
const uint8_t DNUISinstrSwapReg =				DNUISinstrLdRegDenom + 1;
const uint8_t DNUISinstrSubYfromX =				DNUISinstrSwapReg + 1;
const uint8_t DNUISinstrAddYtoX =				DNUISinstrSubYfromX + 1;
const uint8_t DNUISinstrMul2by1 =				DNUISinstrAddYtoX + 1;
const uint8_t DNUISinstrDiv2by1 =				DNUISinstrMul2by1 + 1;
const uint8_t DNUISinstrShiftRegLeft =			DNUISinstrDiv2by1 + 1;
const uint8_t DNUISinstrShiftRegRight =			DNUISinstrShiftRegLeft + 1;
const uint8_t DNUISinstrAddIndex =				DNUISinstrShiftRegRight + 1;
const uint8_t DNUISinstrTraceOn =				DNUISinstrAddIndex + 1;
const uint8_t DNUISinstrTraceOff =				DNUISinstrTraceOn + 1;
const uint8_t DNUISinstrTraceSave =				DNUISinstrTraceOff + 1;
const uint8_t DNUISinstrTraceRestore =			DNUISinstrTraceSave + 1;
const uint8_t DNUISinstrTraceDone =				DNUISinstrTraceRestore + 1;
#define nextAllowedValue DNUISinstrTraceDone + 1
#ifdef useWindowTripFilter
const uint8_t DNUISinstrStRegTripVar =			nextAllowedValue;
#define nextAllowedValue DNUISinstrStRegTripVar + 1
#endif // useWindowTripFilter
#ifdef useIsqrt
const uint8_t DNUISinstrIsqrt =					nextAllowedValue;
#define nextAllowedValue DNUISinstrIsqrt + 1
#endif // useIsqrt
#ifdef useAnalogRead
const uint8_t DNUISinstrLdRegVoltage =			nextAllowedValue;
#define nextAllowedValue DNUISinstrLdRegVoltage + 1
#endif // useAnalogRead

const uint8_t DNUISinstrDone =					nextAllowedValue;							// return to caller - if at topmost level of stack, return to main program

#ifdef useSWEET64trace

static const char opcodeList[] PROGMEM = {
	"instrTestMetricFlag\0"
	"instrTestReg\0"
	"instrTestIndex\0"
	"instrCmpReg\0"
	"instrCmpIndex\0"
	"instrBranchIfMclear\0"
	"instrBranchIfMset\0"
	"instrBranchIfZclear\0"
	"instrBranchIfZset\0"
	"instrBranchIfCclear\0"
	"instrBranchIfCset\0"
	"instrBranchIfLT\0"
	"instrBranchIfGTorE\0"
	"instrSkip\0"
	"instrLdReg\0"
	"instrLdRegByteFromIndex\0"
	"instrLdRegByte\0"
	"instrLdRegByteFromYindexed\0"
	"instrLdRegTripVar\0"
	"instrLdRegTripVarIndexed\0"
	"instrLdRegConst\0"
	"instrStRegByteToIndexedU8\0"
	"instrDoBCDadjust\0"
	"instrLdRegEEPROM\0"
	"instrLdRegEEPROMindexed\0"
	"instrLdRegEEPROMindirect\0"
	"instrStRegEEPROM\0"
	"instrStRegEEPROMindirect\0"
	"instrLdRegVolatile\0"
	"instrLdRegVolatileIndexed\0"
	"instrStRegVolatile\0"
	"instrStRegVolatileIndexed\0"
	"instrLxdI\0"
	"instrLxdIEEPROM\0"
	"instrLxdIEEPROMindexed\0"
	"instrLdRegNumer\0"
	"instrLdRegDenom\0"
	"instrCall\0"
	"instrJump\0"
	"instrSwapReg\0"
	"instrSubYfromX\0"
	"instrAddYtoX\0"
	"instrMul2by1\0"
	"instrDiv2by1\0"
	"instrShiftRegLeft\0"
	"instrShiftRegRight\0"
	"instrAddIndex\0"
	"instrTraceOn\0"
	"instrTraceOff\0"
	"instrTraceSave\0"
	"instrTraceRestore\0"
	"instrTraceDone\0"
#ifdef useWindowTripFilter
	"instrStRegTripVar\0"
#endif // useWindowTripFilter
#ifdef useIsqrt
	"instrIsqrt\0"
#endif // useIsqrt
#ifdef useAnalogRead
	"instrLdRegVoltage\0"
#endif // useAnalogRead
	"instrDone\0"
};

#endif // useSWEET64trace
// these #defines should be used to generate SWEET64 bytecodes
//
// xxx        - base opcode
//     | 0x80 - next byte is an operand byte
//     | 0x40 - next byte specifies which register (or register pair) is to be operated on
//
#define instrDone					 DNUISinstrDone
#define instrTestMetricFlag			(DNUISinstrTestMetricFlag | 0x80)
#define instrTestReg				(DNUISinstrTestReg | 0x40)
#define instrTestIndex 				 DNUISinstrTestIndex
#define instrCmpReg					(DNUISinstrCmpReg | 0x40)
#define instrCmpIndex				(DNUISinstrCmpIndex | 0x80)
#define instrBranchIfMclear			(DNUISinstrBranchIfMclear | 0x80)				// branches if result most significant bit is clear
#define instrBranchIfMset			(DNUISinstrBranchIfMset | 0x80)					// branches if result most significant bit is set
#define instrBranchIfZclear			(DNUISinstrBranchIfZclear | 0x80)				// branches if result zero flag is clear (X != Y or X != 0)
#define instrBranchIfZset			(DNUISinstrBranchIfZset | 0x80)					// branches if result zero flag is set (X == Y or X == 0)
#define instrBranchIfCclear			(DNUISinstrBranchIfCclear | 0x80)				// branches if result carry flag is clear (X <= Y)
#define instrBranchIfCset			(DNUISinstrBranchIfCset | 0x80)					// branches if result carry flag is set (X > Y)
#define instrBranchIfLT				(DNUISinstrBranchIfLT | 0x80)					// branches if X < Y or index < operand
#define instrBranchIfGTorE			(DNUISinstrBranchIfGTorE | 0x80)				// branches if X >= Y or index >= operand
#define instrBranchIfPlus			(DNUISinstrBranchIfMclear | 0x80)				// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfMinus			(DNUISinstrBranchIfMset | 0x80)					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfNotE			(DNUISinstrBranchIfZclear | 0x80)				// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfE				(DNUISinstrBranchIfZset | 0x80)					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfNotZero		(DNUISinstrBranchIfZclear | 0x80)				// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfZero			(DNUISinstrBranchIfZset | 0x80)					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfLTorE			(DNUISinstrBranchIfCclear | 0x80)				// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfGT				(DNUISinstrBranchIfCset | 0x80)					// provided as a convenience for aspiring SWEET64 coders
#define instrSkip					(DNUISinstrSkip | 0x80)							// branches always
#define instrLdReg					(DNUISinstrLdReg | 0x40)
#define instrLdRegByteFromIndex		(DNUISinstrLdRegByteFromIndex | 0x40)
#define instrLdRegByte				(DNUISinstrLdRegByte | 0x80 | 0x40)
#define instrLdRegByteFromYindexed	(DNUISinstrLdRegByteFromYindexed | 0x40)
#define instrLdRegTripVar			(DNUISinstrLdRegTripVar | 0x80 | 0x40)
#define instrLdRegTripVarIndexed	(DNUISinstrLdRegTripVarIndexed | 0x80 | 0x40)
#define instrLdRegConst				(DNUISinstrLdRegConst | 0x80 | 0x40)
#define instrStRegByteToIndexedU8	(DNUISinstrStRegByteToIndexedU8 | 0x80 | 0x40)
#define instrDoBCDadjust			(DNUISinstrDoBCDadjust | 0x40)
#define instrLdRegEEPROM			(DNUISinstrLdRegEEPROM | 0x80 | 0x40)
#define instrLdRegEEPROMindexed		(DNUISinstrLdRegEEPROMindexed | 0x80 | 0x40)
#define instrLdRegEEPROMindirect	(DNUISinstrLdRegEEPROMindirect | 0x40)
#define instrStRegEEPROM			(DNUISinstrStRegEEPROM | 0x80 | 0x40)
#define instrStRegEEPROMindirect	(DNUISinstrStRegEEPROMindirect | 0x40)
#define instrLdRegVolatile			(DNUISinstrLdRegVolatile | 0x80 | 0x40)
#define instrLdRegVolatileIndexed	(DNUISinstrLdRegVolatileIndexed | 0x80 | 0x40)
#define instrStRegVolatile			(DNUISinstrStRegVolatile | 0x80 | 0x40)
#define instrStRegVolatileIndexed	(DNUISinstrStRegVolatileIndexed | 0x80 | 0x40)
#define instrLxdI					(DNUISinstrLxdI | 0x80)
#define instrLxdIEEPROM				(DNUISinstrLxdIEEPROM | 0x80)
#define instrLxdIEEPROMindexed		(DNUISinstrLxdIEEPROMindexed | 0x80)
#define instrLdRegNumer				(DNUISinstrLdRegNumer | 0x40)
#define instrLdRegDenom				(DNUISinstrLdRegDenom | 0x40)
#define instrCall					(DNUISinstrCall | 0x80)
#define instrJump					(DNUISinstrJump | 0x80)
#define instrSwapReg				(DNUISinstrSwapReg | 0x40)
#define instrSubYfromX				(DNUISinstrSubYfromX | 0x40)
#define instrAddYtoX				(DNUISinstrAddYtoX | 0x40)
#define instrMul2by1				 DNUISinstrMul2by1
#define instrDiv2by1				 DNUISinstrDiv2by1
#define instrShiftRegLeft			(DNUISinstrShiftRegLeft | 0x40)
#define instrShiftRegRight			(DNUISinstrShiftRegRight | 0x40)
#define instrAddIndex				(DNUISinstrAddIndex | 0x80)
#define instrTraceOn				 DNUISinstrTraceOn
#define instrTraceOff				 DNUISinstrTraceOff
#define instrTraceSave				 DNUISinstrTraceSave
#define instrTraceRestore			 DNUISinstrTraceRestore
#define instrTraceDone				 DNUISinstrTraceDone
#ifdef useAnalogRead
#define instrLdRegVoltage			(DNUISinstrLdRegVoltage | 0x40)
#endif // useAnalogRead
#ifdef useIsqrt
#define instrIsqrt					(DNUISinstrIsqrt | 0x40)
#endif // useIsqrt
#ifdef useWindowTripFilter
#define instrStRegTripVar			(DNUISinstrStRegTripVar | 0x80 | 0x40)
#endif // useWindowTripFilter

// calculation indexes into SWEET64 S64programList[] for display functions to either screen or logging output
//
// functions are grouped into three categories, in order
//
//    functions that return results of the form "hhmmss"
//    functions that return results that do not require conversion between SI and SAE formats
//    functions that return results in either SI or SAE format, depending on MPGuino settings
//
#define nextAllowedValue 0
const uint8_t tEngineRunTime =				nextAllowedValue;				// engine runtime (hhmmss)
const uint8_t tRangeTime =					tEngineRunTime + 1;				// estimated total runtime from full tank (hhmmss)
const uint8_t tReserveTime =				tRangeTime + 1;					// estimated reserve runtime from full tank (hhmmss)
const uint8_t tBingoTime =					tReserveTime + 1;				// estimated bingo fuel runtime from full tank (hhmmss)
const uint8_t tTimeToEmpty =				tBingoTime + 1;					// estimated remaining total tank engine runtime (hhmmss)
const uint8_t tReserveTimeToEmpty =			tTimeToEmpty + 1;				// estimated remaining reserve engine runtime (hhmmss)
const uint8_t tBingoTimeToEmpty =			tReserveTimeToEmpty + 1;		// estimated bingo quantity engine runtime (hhmmss)
const uint8_t tMotionTime =					tBingoTimeToEmpty + 1;			// time vehicle in motion (hhmmss)
#define nextAllowedValue tMotionTime + 1

const uint8_t dfMaxValHHmmSS =				nextAllowedValue;				// maximum index for function indexes that return hhmmss results

const uint8_t tInjectorOpenTime =			nextAllowedValue;				// fuel used (microseconds)
const uint8_t tInjectorTotalTime =			tInjectorOpenTime + 1;			// engine run time (microseconds)
const uint8_t tVSStotalTime =				tInjectorTotalTime + 1;			// time vehicle in motion (microseconds)
const uint8_t tEngineSpeed =				tVSStotalTime + 1;				// engine speed (1/m)
const uint8_t tInjectorPulseCount =			tEngineSpeed + 1;				// fuel injector pulse count
const uint8_t tVSSpulseCount =				tInjectorPulseCount + 1;		// VSS pulse count
#define nextAllowedValue tVSSpulseCount + 1
#ifdef useFuelCost
const uint8_t tFuelCostUsed =				nextAllowedValue;				// cost of fuel quantity used
const uint8_t tFuelCostTank =				tFuelCostUsed + 1;				// full tank fuel cost in currency units
const uint8_t tFuelCostReserve =			tFuelCostTank + 1;				// reserve fuel quantity fuel cost in currency units
const uint8_t tFuelCostBingo =				tFuelCostReserve + 1;			// bingo fuel quantity cost in currency units
const uint8_t tFuelCostRemaining =			tFuelCostBingo + 1;				// value of estimated remaining fuel quantity in currency units
const uint8_t tFuelCostReserveRemaining =	tFuelCostRemaining + 1;			// value of estimated remaining reserve fuel quantity in currency units
const uint8_t tFuelCostBingoRemaining =		tFuelCostReserveRemaining + 1;	// value of estimated remaining bingo fuel quantity in currency units
const uint8_t tFuelRateCost =				tFuelCostBingoRemaining + 1;	// fuel rate cost in currency units
#define nextAllowedValue tFuelRateCost + 1
#endif // useFuelCost
#ifdef useAnalogRead
const uint8_t tAnalogChannel =				nextAllowedValue;				// DC voltage
#define nextAllowedValue tAnalogChannel + 1
#endif // useAnalogRead
#ifdef useCarVoltageOutput
const uint8_t tAlternatorChannel =			nextAllowedValue;				// DC voltage
const uint8_t tSpareVoltageChannel =		tAlternatorChannel + 1;			// DC voltage
#define nextAllowedValue tSpareVoltageChannel + 1
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
const uint8_t tAccelTestTime =				nextAllowedValue;				// acceleration test time (s.s)
#define nextAllowedValue tAccelTestTime + 1
#endif // useDragRaceFunction
#ifdef useDebugValueReadout
const uint8_t tShowDebugValue =				nextAllowedValue;				// internal volatile variable value
#define nextAllowedValue tShowDebugValue + 1
#endif // useDebugValueReadout

const uint8_t dfMaxValNonConversion =		nextAllowedValue;				// maximum index for function indexes that return results that are not affected by metric conversions

const uint8_t tFuelUsed =					nextAllowedValue;				// fuel quantity used (SI/SAE)
const uint8_t tFuelRate =					tFuelUsed + 1;					// fuel consumption rate (SI/SAE)
const uint8_t tDistance =					tFuelRate + 1;					// vehicle distance traveled (SI/SAE)
const uint8_t tSpeed =						tDistance + 1;					// vehicle speed (SI/SAE)
const uint8_t tFuelEcon =					tSpeed + 1;						// fuel economy (SI/SAE)
const uint8_t tAlternateFuelEcon =			tFuelEcon + 1;					// alternate format fuel economy (SI/SAE)
const uint8_t tRemainingFuel =				tAlternateFuelEcon + 1;			// estimated total remaining fuel quantity (SI/SAE)
const uint8_t tReserveRemainingFuel =		tRemainingFuel + 1;				// estimated reserve remaining fuel quantity (SI/SAE)
const uint8_t tBingoRemainingFuel =			tReserveRemainingFuel + 1;		// estimated bingo remaining fuel quantity (SI/SAE)
const uint8_t tRangeDistance =				tBingoRemainingFuel + 1;		// estimated total fuel tank distance (SI/SAE)
const uint8_t tReserveDistance =			tRangeDistance + 1;				// estimated reserve fuel tank distance (SI/SAE)
const uint8_t tBingoDistance =				tReserveDistance + 1;			// estimated bingo fuel tank distance (SI/SAE)
const uint8_t tDistanceToEmpty =			tBingoDistance + 1;				// estimated remaining distance (SI/SAE)
const uint8_t tReserveDistanceToEmpty =		tDistanceToEmpty + 1;			// estimated reserve remaining distance (SI/SAE)
const uint8_t tBingoDistanceToEmpty =		tReserveDistanceToEmpty + 1;	// estimated bingo remaining distance (SI/SAE)
#define nextAllowedValue tBingoDistanceToEmpty + 1
#ifdef useFuelCost
const uint8_t tFuelCostPerDistance =		nextAllowedValue;				// fuel cost per unit distance (SI/SAE)
const uint8_t tDistancePerFuelCost =		tFuelCostPerDistance + 1;		// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue tDistancePerFuelCost + 1
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
const uint8_t tPressureChannel =			nextAllowedValue;				// absolute pressure (SI/SAE)
#define nextAllowedValue tPressureChannel + 1
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
const uint8_t tEstimatedEnginePower =		nextAllowedValue;				// estimated engine power (SI/SAE)
const uint8_t tDragSpeed =					tEstimatedEnginePower + 1;		// acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#define nextAllowedValue tDragSpeed + 1
#endif // useDragRaceFunction

const uint8_t dfMaxValDisplayCount =		nextAllowedValue;				// maximum index for function indexes that return results for display

const uint8_t tCalculateRemainingTank =		nextAllowedValue;				// calculate estimated remaining fuel quantity in injector open cycles
const uint8_t tCalculateRemainingReserve =	tCalculateRemainingTank + 1;	// calculate estimated remaining fuel reserve value in injector open cycles
const uint8_t tCalculateBingoFuel =			tCalculateRemainingReserve + 1;	// calculate estimated fuel bingo value in injector open cycles
const uint8_t tConvertToMicroSeconds =		tCalculateBingoFuel + 1;
const uint8_t tCalculateFuelQuantity =		tConvertToMicroSeconds + 1;
const uint8_t tCalculateFuelDistance =		tCalculateFuelQuantity + 1;
const uint8_t tCalculateFuelTime =			tCalculateFuelDistance + 1;
const uint8_t tFormatToNumber =				tCalculateFuelTime + 1;
#define nextAllowedValue tFormatToNumber + 1

// indexes into SWEET64 conversion factor value table
//
#define nextAllowedValue 0
const uint8_t idxNumerDistance =			nextAllowedValue;
const uint8_t idxDenomDistance =			idxNumerDistance + 1;
const uint8_t idxNumerVolume =				idxDenomDistance + 1;
const uint8_t idxDenomVolume =				idxNumerVolume + 1;
#define nextAllowedValue idxDenomVolume + 1
#ifdef useCPUreading
const uint8_t idxNumerCPUutil =				nextAllowedValue;
const uint8_t idxDenomCPUutil =				idxNumerCPUutil + 1;
#define nextAllowedValue idxDenomCPUutil + 1
#endif // useCPUreading
#ifdef useIsqrt
const uint8_t idxNumerPressure =			nextAllowedValue;
const uint8_t idxDenomPressure =			idxNumerPressure + 1;
#define nextAllowedValue idxDenomPressure + 1
#endif // useIsqrt
#ifdef useAnalogRead
const uint8_t idxNumerVoltage =				nextAllowedValue;
const uint8_t idxDenomVoltage =				idxNumerVoltage + 1;
#define nextAllowedValue idxDenomVoltage + 1
#endif // useAnalogRead
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
const uint8_t idxCycles0PerSecond =			nextAllowedValue;
const uint8_t idxCycles0PerTick =			idxCycles0PerSecond + 1;		// known as the "N" in the (processor speed)/(N * prescaler) for timer0 fast PWM mode
const uint8_t idxMicroSecondsPerSecond =	idxCycles0PerTick + 1;
const uint8_t idxDecimalPoint =				idxMicroSecondsPerSecond + 1;
const uint8_t idxOneThousand =				idxDecimalPoint;
const uint8_t idxMetricFE =					idxDecimalPoint + 1;
const uint8_t idxSecondsPerHour =			idxMetricFE + 1;
const uint8_t idxNumber500 =				idxSecondsPerHour + 1;
const uint8_t idxBCDdivisor =				idxNumber500 + 1;
#define nextAllowedValue idxBCDdivisor + 1
#ifdef useBigNumberDisplay
const uint8_t idxNumber6nines =				nextAllowedValue;
const uint8_t idxNumber5nines =				idxNumber6nines + 1;
const uint8_t idxNumber4nines =				idxNumber5nines + 1;
#define nextAllowedValue idxNumber4nines + 1
#endif // useBigNumberDisplay
#ifdef useDragRaceFunction
const uint8_t idxPowerFactor =				nextAllowedValue;
#define nextAllowedValue idxPowerFactor + 1
#endif // useDragRaceFunction

const uint32_t convNumbers[] PROGMEM = {
	1609344ul						// numerator to convert miles to kilometers
	,1000000ul						// denominator to convert miles to kilometers
	,3785411784ul					// numerator to convert US gallons to liters
	,1000000000ul					// denominator to convert US gallons to liters
#ifdef useCPUreading
	,100000ul						// numerator to convert loop count to CPU utilization percentage
	,loopSystemLength				// denominator to convert loop count to CPU utilization percentage
#endif // useCPUreading
#ifdef useIsqrt
	,68947573ul						// numerator to convert psig to kPa
	,10000000ul						// denominator to convert psig to kPa
#endif // useIsqrt
#ifdef useAnalogRead
	,1024ul							// numerator to convert volts DC to ADC steps
	,5000ul							// denominator to convert volts DC to ADC steps
#endif // useAnalogRead
#ifdef useVehicleMass
	,1000000000ul					// numerator to convert pounds to kilograms
	,2204622621ul					// denominator to convert pounds to kilograms
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,9290304ul						// numerator to convert square feet to square meters
	,100000000ul					// denominator to convert square feet to square meters
	,100000ul						// numerator to convert SAE density to metric density
	,168555ul						// denominator to convert SAE density to metric density
#endif // useCoastDownCalculator
#ifdef useClockDisplay
	,86400ul						// number of seconds in a day
#endif // useClockDisplay
#ifdef useIsqrt
	,4096ul							// correction factor seed for square root function
#endif // useIsqrt
#ifdef useImperialGallon
	,454609ul						// numerator to convert Imperial gallons to liters
	,100000ul						// denominator to convert Imperial gallons to liters
#endif // useImperialGallon
	,t0CyclesPerSecond				// timer0 clock cycles per second
	,256ul							// timer0 clock cycles per timer0 tick
	,1000000ul						// microseconds per second
	,1000ul							// decimal point format (the basis for all of those '* 1000' parameters)
	,100000ul						// decimal point format * 100 for metric FE (L / 100km)
	,3600ul							// seconds per hour
	,500ul							// round to nearest whole number, for numeric formatting
	,100000000ul					// divisor to separate lower 4 BCD bytes from 5th byte
#ifdef useBigNumberDisplay
	,1000000ul						// 999.999 + 1  - numeric formatting
	,100000ul						// 99.999 + 1   - numeric formatting
	,10000ul						// 9.999 + 1    - numeric formatting
#endif // useBigNumberDisplay
#ifdef useDragRaceFunction
	,22840ul						// 22.84, or vehicle speed division factor for accel test power estimation function ( 228.4/10 for internal calculations)
#endif // useDragRaceFunction
};

const uint8_t rvLength = 8;					// total length of trip variable in bytes

const uint8_t rvVSSpulseIdx = 0; 			// distance pulse count
const uint8_t rvInjPulseIdx = 1; 			// engine revolution count
const uint8_t rvVSScycleIdx = 2; 			// time that the vehicle has spent moving
const uint8_t rvInjCycleIdx = 4; 			// engine run time since this class was reset
const uint8_t rvInjOpenCycleIdx = 6; 		// time that the fuel injector has been open

class tripVar // Trip prototype
{

public:
	unsigned long collectedData[rvLength];

	void reset(void); // reset Trip instance
	void transfer(tripVar t);
	void update(tripVar t); // update with results of another Trip instance
	void add64s(uint8_t calcIdx, unsigned long v);
	void add32(uint8_t calcIdx, unsigned long v);
#ifdef useEEPROMtripStorage
	uint8_t load(uint8_t tripSlotIdx);
	uint8_t save(uint8_t tripSlotIdx);
#endif // useEEPROMtripStorage

};

// trip index definitions for all of the trip variables used within MPGuino

volatile uint8_t rawTripIdx;
#ifdef trackIdleEOCdata
volatile uint8_t rawEOCidleTripIdx;
#endif // trackIdleEOCdata

#ifdef useWindowTripFilter
const uint8_t windowTripFilterSize = 4;
uint8_t wtpCurrentIdx;

#endif // useWindowTripFilter
#ifdef useBarGraph
const uint8_t bgDataSize = 15;

#endif // useBarGraph
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
#ifdef useTerminal
const uint8_t terminalIdx =				nextAllowedValue;
#define nextAllowedValue terminalIdx + 1
#endif // useTerminal
#ifdef useDragRaceFunction
const uint8_t dragRawHalfSpeedIdx =		nextAllowedValue;
const uint8_t dragRawFullSpeedIdx =		dragRawHalfSpeedIdx + 1;
const uint8_t dragRawDistanceIdx =		dragRawFullSpeedIdx + 1;
const uint8_t dragHalfSpeedIdx =		dragRawDistanceIdx + 1;
const uint8_t dragFullSpeedIdx =		dragHalfSpeedIdx + 1;
const uint8_t dragDistanceIdx =			dragFullSpeedIdx + 1;
#define nextAllowedValue dragDistanceIdx + 1
#endif // useDragRaceFunction
#ifdef useBarFuelEconVsTime
const uint8_t periodIdx =				nextAllowedValue;
#define nextAllowedValue periodIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t FEvsSpeedIdx =			nextAllowedValue;
#define nextAllowedValue FEvsSpeedIdx + bgDataSize
#endif // useBarFuelEconVsSpeed
#ifdef useWindowTripFilter
const uint8_t windowTripFilterIdx =		nextAllowedValue;
#define nextAllowedValue windowTripFilterIdx + windowTripFilterSize
#endif // useWindowTripFilter

const uint8_t tripSlotCount =			nextAllowedValue;

tripVar tripArray[(unsigned int)(tripSlotCount)]; // main object we will be working with

// volatile variable array index values
#define nextAllowedValue 0
const uint8_t vVehicleStopTimeoutIdx =		nextAllowedValue;				// engine idle timeout value in timer0 ticks
const uint8_t vEngineOffTimeoutIdx =		vVehicleStopTimeoutIdx + 1;		// engine off coasting timeout value in timer0 ticks
const uint8_t vDetectVehicleStopIdx =		vEngineOffTimeoutIdx + 1;		// minimum good vehicle speed in timer0 ticks
const uint8_t vDetectEngineOffIdx =			vDetectVehicleStopIdx + 1;		// minimum good engine speed in timer0 ticks
const uint8_t vMaximumVSSperiodIdx =		vDetectEngineOffIdx + 1;		// maximum good VSS period in timer0 cycles
const uint8_t vMaximumEnginePeriodIdx =		vMaximumVSSperiodIdx + 1;		// maximum good engine period in timer0 cycles
const uint8_t vInjectorOpenDelayIdx =		vMaximumEnginePeriodIdx + 1;	// injector settle time in timer0 cycles
const uint8_t vInjectorCloseDelayIdx =		vInjectorOpenDelayIdx + 1;		// injector closing settle time in timer0 cycles
const uint8_t vInjectorTotalDelayIdx =		vInjectorCloseDelayIdx + 1;		// injector total settle time in timer0 cycles, for injector validity check
const uint8_t vInjectorValidMaxWidthIdx =	vInjectorTotalDelayIdx + 1;		// maximum valid fuel injector pulse width in timer0 cycles
const uint8_t vButtonTimeoutIdx =			vInjectorValidMaxWidthIdx + 1;	// button press timeout value in timer0 ticks
const uint8_t vParkTimeoutIdx =				vButtonTimeoutIdx + 1;			// vehicle park (engine off and vehicle stopped) timeout value in timer0 ticks
const uint8_t vActivityTimeoutIdx =			vParkTimeoutIdx + 1;			// total activity (vehicle park and no button press) timeout value in timer0 ticks
const uint8_t vCyclesPerVolumeIdx =			vActivityTimeoutIdx + 1;		// injector conversion factor of unit volume per timer0 cycle
const uint8_t vTankSizeIdx =				vCyclesPerVolumeIdx + 1;		// tank quantity in timer0 cycles
const uint8_t vBingoTankSizeIdx =			vTankSizeIdx + 1;				// bingo fuel quantity in timer0 cycles
#define nextAllowedValue vBingoTankSizeIdx + 1
#ifdef usePartialRefuel
const uint8_t vPartialRefuelTankSize =		nextAllowedValue;				// partial refuel tank quantity in timer0 cycles
#define nextAllowedValue vPartialRefuelTankSize + 1
#endif // usePartialRefuel
#ifdef useDragRaceFunction
const uint8_t vDragInstantSpeedIdx =		nextAllowedValue;
const uint8_t vAccelHalfPeriodValueIdx =	vDragInstantSpeedIdx + 1;
const uint8_t vAccelFullPeriodValueIdx =	vAccelHalfPeriodValueIdx + 1;
const uint8_t vAccelDistanceValueIdx =		vAccelFullPeriodValueIdx + 1;
#define nextAllowedValue vAccelDistanceValueIdx + 1
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
const uint8_t vCoastdownPeriodIdx =			nextAllowedValue;
const uint8_t vCoastdownMeasurementIdx =	vCoastdownPeriodIdx + 1;
#define nextAllowedValue vCoastdownMeasurementIdx + 3
#endif // useCoastDownCalculator
#ifdef useChryslerMAPCorrection
const uint8_t vMAPpressureIdx =				nextAllowedValue;
const uint8_t vBaroPressureIdx =			vMAPpressureIdx + 1;
const uint8_t vFuelPressureIdx =			vBaroPressureIdx + 1;
const uint8_t vInjPressureIdx =				vFuelPressureIdx + 1;
const uint8_t vInjectorCorrectionIdx =		vInjPressureIdx + 1;
const uint8_t vAnalogFloor0Idx =			vInjectorCorrectionIdx + 1;
const uint8_t vAnalogFloor1Idx =			vAnalogFloor0Idx + 1;
const uint8_t vAnalogSlope0Idx =			vAnalogFloor1Idx + 1;
const uint8_t vAnalogSlope1Idx =			vAnalogSlope0Idx + 1;
const uint8_t vAnalogOffset0Idx =			vAnalogSlope1Idx + 1;
const uint8_t vAnalogOffset1Idx =			vAnalogOffset0Idx + 1;
#define nextAllowedValue vAnalogOffset1Idx + 1
#endif // useChryslerMAPCorrection
#ifdef useDebugValueReadout
const uint8_t vDebugValue1Idx =				nextAllowedValue;
const uint8_t vDebugValue2Idx =				vDebugValue1Idx + 1;
const uint8_t vDebugValue3Idx =				vDebugValue2Idx + 1;
const uint8_t vDebugValue4Idx =				vDebugValue3Idx + 1;
#define nextAllowedValue vDebugValue4Idx + 1
#endif // useDebugValueReadout

const uint8_t maxVolatileVariableIdx =		nextAllowedValue;

volatile unsigned long volatileVariables[(unsigned int)(maxVolatileVariableIdx)];

uint8_t metricFlag;

const uint8_t alternateFEmode =	0b00000010;
const uint8_t metricMode =		0b00000001;

char mBuff1[17]; // used by formatDecimal(), doFormat(), bigNumber::, bar graph routines
char pBuff[12]; // used by numberEdit::, function result output routines

static const char titleMPGuino[] PROGMEM = {
	"MPGuino v1.95tav"
};

static const char dateMPGuino[] PROGMEM = {
	"2022-DEC-02"
};

#ifdef useEEPROMtripStorage
const uint8_t tripSaveSlotCount = 10; // plan for saving 10 separate trip variables (could be changed by compiler directives later on)
const uint8_t tripListPtrLength = rvLength;
const uint8_t tripSlotPtrSize = tripListPtrLength + 2; // (uint8_t signature) plus (uint32_t timestamp)
const uint8_t tripSlotSignaturePtr = tripSlotPtrSize - 1;
const uint8_t tripSlotAdrSize = tripListPtrLength * 4 + 5; // trip list length plus (uint8_t signature) plus (uint32_t timestamp)

#ifdef useSavedTrips
const uint8_t savedTripsCount = 2 // number of trip save/show screens
#ifdef trackIdleEOCdata
	+ 2
#endif // trackIdleEOCdata
;

const uint8_t tripSaveMenuSize = 3
#ifdef showSavedTrips
	+ 1
#endif // showSavedTrips
; // size of trip save menu

#ifdef showSavedTrips
const uint8_t tripShowMenuSize = 7; // size of trip show menu
#endif // showSavedTrips

#endif // useSavedTrips
#endif // useEEPROMtripStorage
const uint8_t displayPageCount = 9						// count of base number of data screens
#ifdef useChryslerMAPCorrection
	+ 1													// count of Chrysler MAP-specific data screen
#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
	+ 1													// count of analog voltage data screen
#endif // useAnalogRead
#ifdef trackIdleEOCdata
	+ 3													// count of Idle/EOC tracking data screens
#endif // trackIdleEOCdata
;
const uint8_t mainScreenDisplayFormatSize = displayPageCount * 4;

const uint8_t guinosig =		0b10110111;

// start of remarkably long EEPROM stored settings section

const uint8_t pSizeSignature =				16;
#ifdef useLegacyLCD
const uint8_t pSizeContrast =				1;
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t pSizeLCDcolor =				3;
#endif // useAdafruitRGBLCDshield
const uint8_t pSizeMetricMode =				1;
const uint8_t pSizeAlternateFE =			1;
#ifdef useDragRaceFunction
const uint8_t pSizeDragSpeed =				24;
const uint8_t pSizeDragDistance =			16;
const uint8_t pSizeDragAutoFlag =			1;
#endif // useDragRaceFunction
const uint8_t pSizeInjEdgeTrigger =			1;
#ifdef useIsqrt
const uint8_t pSizeSysFuelPressure =		32;
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pSizeRefFuelPressure =		32;
const uint8_t pSizeInjectorCount =			8;
const uint8_t pSizeInjectorSize =			20;
#endif // useCalculatedFuelFactor
const uint8_t pSizeMicroSecondsPerGallon =	32;
const uint8_t pSizeInjectorOpeningTime =	16;
const uint8_t pSizeInjectorClosingTime =	16;
const uint8_t pSizePulsesPerDistance =		18;
const uint8_t pSizeVSSpause =				8;
const uint8_t pSizeCrankRevPerInj =			8;
const uint8_t pSizeMinGoodRPM =				16;
const uint8_t pSizeMinGoodSpeed =			16;
const uint8_t pSizeTankSize =				24;
const uint8_t pSizeTankBingoSize =			24;
#ifdef useChryslerMAPCorrection
const uint8_t pSizeMAPsensorFloor =			16;
const uint8_t pSizeBaroSensorFloor =		16;
const uint8_t pSizeMAPsensorCeiling =		16;
const uint8_t pSizeBaroSensorCeiling =		16;
const uint8_t pSizeMAPsensorRange =			32;
const uint8_t pSizeBaroSensorRange =		32;
const uint8_t pSizeMAPsensorOffset =		32;
const uint8_t pSizeBaroSensorOffset =		32;
#endif // useChryslerMAPCorrection
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
const uint8_t pSizeIdleTimeout =			8;
const uint8_t pSizeEOCtimeout =				8;
const uint8_t pSizeButtonTimeout =			8;
const uint8_t pSizeParkTimeout =			8;
const uint8_t pSizeActivityTimeout =		8;
const uint8_t pSizeWakeupResetCurrentOnEngine =	1;
const uint8_t pSizeWakeupResetCurrentOnButton =	1;
#ifdef useSavedTrips
const uint8_t pSizeAutoSaveActive =			1;
#endif // useSavedTrips
#ifdef useSerialDataLogging
const uint8_t pSizeSerialDataLogging =		1;
#endif // useSerialDataLogging
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
#ifdef useFuelCost
const uint8_t pSizeFuelUnitCost =			16;
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pSizeExpansionPin1Mode =		3;
const uint8_t pSizeExpansionPin2Mode =		3;
#endif // useExpansionPins
const uint8_t pSizeScratchpad =				32;
#ifdef usePartialRefuel
const uint8_t pSizeRefuelSize =				24;
#endif // usePartialRefuel

#define byteSize(bitLength) ((((bitLength & 0x07) != 0)? 1 : 0) + (bitLength / 8))

const uint8_t eeAdrSignature = 0;

#define nextAllowedValue eeAdrSignature
const unsigned int pAddressSignature =					nextAllowedValue;
#define nextAllowedValue pAddressSignature + byteSize(pSizeSignature)
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
#ifdef useDragRaceFunction
const unsigned int pAddressDragSpeed =					nextAllowedValue;
const unsigned int pAddressDragDistance =				pAddressDragSpeed + byteSize(pSizeDragSpeed);
const unsigned int pAddressDragAutoFlag =				pAddressDragDistance + byteSize(pSizeDragDistance);
#define nextAllowedValue pAddressDragAutoFlag + byteSize(pSizeDragAutoFlag)
#endif // useDragRaceFunction
const unsigned int pAddressInjEdgeTrigger =				nextAllowedValue;
#define nextAllowedValue pAddressInjEdgeTrigger + byteSize(pSizeInjEdgeTrigger)
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
const unsigned int pAddressInjectorOpeningTime =		pAddressMicroSecondsPerGallon + byteSize(pSizeMicroSecondsPerGallon);
const unsigned int pAddressInjectorClosingTime =		pAddressInjectorOpeningTime + byteSize(pSizeInjectorOpeningTime);
const unsigned int pAddressPulsesPerDistance =			pAddressInjectorClosingTime + byteSize(pSizeInjectorClosingTime);
const unsigned int pAddressVSSpause =					pAddressPulsesPerDistance + byteSize(pSizePulsesPerDistance);
const unsigned int pAddressCrankRevPerInj =				pAddressVSSpause + byteSize(pSizeVSSpause);
const unsigned int pAddressMinGoodRPM =					pAddressCrankRevPerInj + byteSize(pSizeCrankRevPerInj);
const unsigned int pAddressMinGoodSpeed =				pAddressMinGoodRPM + byteSize(pSizeMinGoodRPM);
const unsigned int pAddressTankSize =					pAddressMinGoodSpeed + byteSize(pSizeMinGoodSpeed);
const unsigned int pAddressTankBingoSize =				pAddressTankSize + byteSize(pSizeTankSize);
#define nextAllowedValue pAddressTankBingoSize + byteSize(pSizeTankBingoSize)
#ifdef useChryslerMAPCorrection
const unsigned int pAddressMAPsensorFloor =				nextAllowedValue;
const unsigned int pAddressBaroSensorFloor =			pAddressMAPsensorFloor + byteSize(pSizeMAPsensorFloor);
const unsigned int pAddressMAPsensorCeiling =			pAddressBaroSensorFloor + byteSize(pSizeBaroSensorFloor);
const unsigned int pAddressBaroSensorCeiling =			pAddressMAPsensorCeiling + byteSize(pSizeMAPsensorCeiling);
const unsigned int pAddressMAPsensorRange =				pAddressBaroSensorCeiling + byteSize(pSizeBaroSensorCeiling);
const unsigned int pAddressBaroSensorRange =			pAddressMAPsensorRange + byteSize(pSizeMAPsensorRange);
const unsigned int pAddressMAPsensorOffset =			pAddressBaroSensorRange + byteSize(pSizeBaroSensorRange);
const unsigned int pAddressBaroSensorOffset =			pAddressMAPsensorOffset + byteSize(pSizeMAPsensorOffset);
#define nextAllowedValue pAddressBaroSensorOffset + byteSize(pSizeBaroSensorOffset)
#endif // useChryslerMAPCorrection
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
const unsigned int pAddressIdleTimeout =				nextAllowedValue;
const unsigned int pAddressEOCtimeout =					pAddressIdleTimeout + byteSize(pSizeIdleTimeout);
const unsigned int pAddressButtonTimeout =				pAddressEOCtimeout + byteSize(pSizeEOCtimeout);
const unsigned int pAddressParkTimeout =				pAddressButtonTimeout + byteSize(pSizeButtonTimeout);
const unsigned int pAddressActivityTimeout =			pAddressParkTimeout + byteSize(pSizeParkTimeout);
const unsigned int pAddressWakeupResetCurrentOnEngine =	pAddressActivityTimeout + byteSize(pSizeActivityTimeout);
const unsigned int pAddressWakeupResetCurrentOnButton =	pAddressWakeupResetCurrentOnEngine + byteSize(pSizeWakeupResetCurrentOnEngine);
#define nextAllowedValue pAddressWakeupResetCurrentOnButton + byteSize(pSizeWakeupResetCurrentOnButton)
#ifdef useSavedTrips
const unsigned int pAddressAutoSaveActive =				nextAllowedValue;
#define nextAllowedValue pAddressAutoSaveActive + byteSize(pSizeAutoSaveActive)
#endif // useSavedTrips
#ifdef useSerialDataLogging
const unsigned int pAddressSerialDataLogging =			nextAllowedValue;
#define nextAllowedValue pAddressSerialDataLogging + byteSize(pSizeSerialDataLogging)
#endif // useSerialDataLogging
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
#ifdef useFuelCost
const unsigned int pAddressFuelUnitCost =				nextAllowedValue;
#define nextAllowedValue pAddressFuelUnitCost + byteSize(pSizeFuelUnitCost)
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pAddressExpansionPin1Mode =				nextAllowedValue;
const uint8_t pAddressExpansionPin2Mode =				pAddressExpansionPin1Mode + byteSize(pSizeExpansionPin1Mode);
#define nextAllowedValue pAddressExpansionPin2Mode + byteSize(pSizeExpansionPin2Mode)
#endif // useExpansionPins
const unsigned int pAddressScratchpad =					nextAllowedValue;
#define nextAllowedValue pAddressScratchpad + byteSize(pSizeScratchpad)
#ifdef usePartialRefuel
const unsigned int pAddressRefuelSize =					nextAllowedValue;
#define nextAllowedValue pAddressRefuelSize + byteSize(pSizeRefuelSize)
#endif // usePartialRefuel

const unsigned int eeAdrSettingsEnd =					nextAllowedValue;

#ifdef useScreenEditor
const unsigned int eeAdrScreensStart =					nextAllowedValue;
const unsigned int eeAdrScreensEnd =					eeAdrScreensStart + 2 * mainScreenDisplayFormatSize;
#define nextAllowedValue eeAdrScreensEnd

#endif // useScreenEditor
#ifdef useEEPROMtripStorage
const unsigned int eeAdrSavedTripsStart = 				nextAllowedValue;

// figure out how much space is available for saving trip variables
const unsigned int eeAdrRemainingEEPROM =			(unsigned int)(E2END) - eeAdrSavedTripsStart + 1;
// figure out how many trip variables can actually be saved in EEPROM
const uint8_t tripMaxSaveSlotCount =				(uint8_t)(eeAdrRemainingEEPROM / (unsigned int)(tripSlotAdrSize));
// figure out the maximum allowable number of trip variables that can be saved
const uint8_t tripAllowedSaveSlotCount =			((tripSaveSlotCount > tripMaxSaveSlotCount) ? tripMaxSaveSlotCount : tripSaveSlotCount);

const unsigned int eeAdrSavedTripsEnd =				eeAdrSavedTripsStart + ((unsigned int)(tripSlotAdrSize) * (unsigned int)(tripAllowedSaveSlotCount));
#define nextAllowedValue eeAdrSavedTripsEnd

#endif // useEEPROMtripStorage
#define nextAllowedValue 0
const uint8_t pSignatureIdx =			nextAllowedValue;
#define nextAllowedValue pSignatureIdx + 1
const uint8_t eePtrSettingsStart =		nextAllowedValue;
#ifdef useLegacyLCD
const uint8_t pContrastIdx =			nextAllowedValue;
#define nextAllowedValue pContrastIdx + 1
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t pLCDcolorIdx =			nextAllowedValue;
#define nextAllowedValue pLCDcolorIdx + 1
#endif // useAdafruitRGBLCDshield
const uint8_t pMetricModeIdx =			nextAllowedValue;
const uint8_t pAlternateFEidx =			pMetricModeIdx + 1;
#define nextAllowedValue pAlternateFEidx + 1
#ifdef useDragRaceFunction
const uint8_t pDragSpeedIdx =			nextAllowedValue;
const uint8_t pDragDistanceIdx =		pDragSpeedIdx + 1;
const uint8_t pDragAutoFlagIdx =		pDragDistanceIdx + 1;
#define nextAllowedValue pDragAutoFlagIdx + 1
#endif // useDragRaceFunction
const uint8_t pInjEdgeTriggerIdx =		nextAllowedValue;
#define nextAllowedValue pInjEdgeTriggerIdx + 1
#ifdef useIsqrt
const uint8_t pSysFuelPressureIdx =		nextAllowedValue;
#define nextAllowedValue pSysFuelPressureIdx + 1
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pRefFuelPressureIdx =		nextAllowedValue;
const uint8_t pInjectorCountIdx =		pRefFuelPressureIdx + 1;
const uint8_t pInjectorSizeIdx =		pInjectorCountIdx + 1;
#define nextAllowedValue pInjectorSizeIdx + 1
#endif // useCalculatedFuelFactor
const uint8_t pMicroSecondsPerGallonIdx =	nextAllowedValue;
const uint8_t pInjectorOpeningTimeIdx =	pMicroSecondsPerGallonIdx + 1;
const uint8_t pInjectorClosingTimeIdx =	pInjectorOpeningTimeIdx + 1;
const uint8_t pPulsesPerDistanceIdx =	pInjectorClosingTimeIdx + 1;
const uint8_t pVSSpauseIdx =			pPulsesPerDistanceIdx + 1;
const uint8_t pCrankRevPerInjIdx =		pVSSpauseIdx + 1;
const uint8_t pMinGoodRPMidx =			pCrankRevPerInjIdx + 1;
const uint8_t pMinGoodSpeedidx =		pMinGoodRPMidx + 1;
const uint8_t pTankSizeIdx =			pMinGoodSpeedidx + 1;
const uint8_t pTankBingoSizeIdx =		pTankSizeIdx + 1;
#define nextAllowedValue pTankBingoSizeIdx + 1
#ifdef useChryslerMAPCorrection
const uint8_t pMAPsensorFloorIdx =		nextAllowedValue;
const uint8_t pBaroSensorFloorIdx =		pMAPsensorFloorIdx + 1;
const uint8_t pMAPsensorCeilingIdx =	pBaroSensorFloorIdx + 1;
const uint8_t pBaroSensorCeilingIdx =	pMAPsensorCeilingIdx + 1;
const uint8_t pMAPsensorRangeIdx =		pBaroSensorCeilingIdx + 1;
const uint8_t pBaroSensorRangeIdx =		pMAPsensorRangeIdx + 1;
const uint8_t pMAPsensorOffsetIdx =		pBaroSensorRangeIdx + 1;
const uint8_t pBaroSensorOffsetIdx =	pMAPsensorOffsetIdx + 1;
#define nextAllowedValue pBaroSensorOffsetIdx + 1
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
const uint8_t pVehicleMassIdx =			nextAllowedValue;
#define nextAllowedValue pVehicleMassIdx + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pVehicleFrontalAreaIdx =	nextAllowedValue;
const uint8_t pLocustDensityIdx =		pVehicleFrontalAreaIdx + 1;
const uint8_t pCoefficientDidx =		pLocustDensityIdx + 1;
const uint8_t pCoefficientVidx =		pCoefficientDidx + 1;
const uint8_t pCoefficientRRidx =		pCoefficientVidx + 1;
const uint8_t pCoastdownSamplePeriodIdx =	pCoefficientRRidx + 1;
#define nextAllowedValue pCoastdownSamplePeriodIdx + 1
#endif // useCoastDownCalculator
const uint8_t pIdleTimeoutIdx =			nextAllowedValue;
const uint8_t pEOCtimeoutIdx =			pIdleTimeoutIdx + 1;
const uint8_t pButtonTimeoutIdx =		pEOCtimeoutIdx + 1;
const uint8_t pParkTimeoutIdx =			pButtonTimeoutIdx + 1;
const uint8_t pActivityTimeoutIdx =		pParkTimeoutIdx + 1;
const uint8_t pWakeupResetCurrentOnEngineIdx =	pActivityTimeoutIdx + 1;
const uint8_t pWakeupResetCurrentOnButtonIdx =	pWakeupResetCurrentOnEngineIdx + 1;
#define nextAllowedValue pWakeupResetCurrentOnButtonIdx + 1
#ifdef useSavedTrips
const uint8_t pAutoSaveActiveIdx =		nextAllowedValue;
#define nextAllowedValue pAutoSaveActiveIdx + 1
#endif // useSavedTrips
#ifdef useSerialDataLogging
const uint8_t pSerialDataLoggingIdx =	nextAllowedValue;
#define nextAllowedValue pSerialDataLoggingIdx + 1
#endif // useSerialDataLogging
#ifdef useWindowTripFilter
const uint8_t pWindowTripFilterIdx =	nextAllowedValue;
#define nextAllowedValue pWindowTripFilterIdx + 1
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
const uint8_t pFEvsTimeIdx =			nextAllowedValue;
#define nextAllowedValue pFEvsTimeIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pBarLowSpeedCutoffIdx =	nextAllowedValue;
const uint8_t pBarSpeedQuantumIdx =		pBarLowSpeedCutoffIdx + 1;
#define nextAllowedValue pBarSpeedQuantumIdx + 1
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
const uint8_t pCostPerQuantity =		nextAllowedValue;
#define nextAllowedValue pCostPerQuantity + 1
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pExpansionPin1Mode =		nextAllowedValue;
const uint8_t pExpansionPin2Mode =		pExpansionPin1Mode + 1;
#define nextAllowedValue pExpansionPin2Mode + 1
#endif // useExpansionPins
const uint8_t pScratchpadIdx =			nextAllowedValue;
#define nextAllowedValue pScratchpadIdx + 1
#ifdef usePartialRefuel
const uint8_t pRefuelSizeIdx =			nextAllowedValue;
#define nextAllowedValue pRefuelSizeIdx + 1
#endif // usePartialRefuel

const uint8_t eePtrSettingsEnd =		nextAllowedValue;

#ifdef useScreenEditor
const uint8_t eePtrScreensStart =		nextAllowedValue;
const uint8_t eePtrScreensEnd =			eePtrScreensStart + mainScreenDisplayFormatSize;
#define nextAllowedValue eePtrScreensEnd

#endif // useScreenEditor
#ifdef useEEPROMtripStorage
const uint8_t eePtrSavedTripsStart = 	nextAllowedValue;
const uint8_t eePtrSavedTripsEnd =		eePtrSavedTripsStart + (tripSlotPtrSize * tripAllowedSaveSlotCount);
#define nextAllowedValue eePtrSavedTripsEnd

#endif // useEEPROMtripStorage
const uint8_t eePtrEnd =				nextAllowedValue;

const char parmLabels[] PROGMEM = {
#ifdef useLegacyLCD
	"Contrast\0"
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	"LCD BG Color\0"
#endif // useAdafruitRGBLCDshield
	"Metric 1-Yes\0"
	"AltFEformat 1-Y\0"
#ifdef useDragRaceFunction
	"DragSpd \xfbMPH\xfckph\xfd*1000\0"
	"DragDist \xfbmi\xfckm\xfd*1000\0"
	"DragAutoTrigger\0"
#endif // useDragRaceFunction
	"InjTrg 0-Dn 1-Up\0"
#ifdef useIsqrt
	"P(Fuel) \xfbpsig*1000\xfcPa\xfd\0"
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	"P(Ref) \xfbpsig*1000\xfcPa\xfd\0"
	"Injector Count\0"
	"InjSiz cc/min*1K\0"
#endif // useCalculatedFuelFactor
	"Microsec/USgal\0"
	"Inj Delay (us)\0"
	"InjCloseDly (us)\0"
	"VSS Pulses/\xfbMile\xfckm\xfd\0"
	"VSS Delay (ms)\0"
	"Revs/Inj Pulse\0"
	"Min RPM\0"
	"Min Spd\xfbMPH\xfcKPH\xfd*1000 \0"
#ifdef useImperialGallon
	"Tank*1000 \xfbImpGal\xfcL\xfd\0"
	"Bingo*1000 \xfbImpGal\xfcL\xfd\0"
#else // useImperialGallon
	"Tank*1000 \xfbUSgal\xfcL\xfd\0"
	"Bingo*1000 \xfbUSgal\xfcL\xfd\0"
#endif // useImperialGallon
#ifdef useChryslerMAPCorrection
	"MAPfloor (mV)\0"
	"BaroFloor (mV)\0"
	"MAPceiling (mV)\0"
	"BaroCeiling (mV)\0"
	"MAPrng \xfbpsi*1000\xfcPa\xfd\0"
	"BaroRng \xfbpsi*1000\xfcPa\xfd\0"
	"MAPofst \xfbpsi*1000\xfcPa\xfd\0"
	"BaroOfst \xfbpsi*1000\xfcPa\xfd\0"
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
	"\xfbWeight\xfcMass\xfd (\xfblbs\xfckg\xfd)\0"
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	"FrArea*1000 \xfbft\xfcm\xfd^2\0"
	"rho*1000 \xfblb/yd\xfckg/m\xfd^3\0"
	"C(d) * 1000\0"
	"C(v) * 1000\0"
	"C(rr) * 1000\0"
	"Cd SamplePd (s)\0"
#endif // useCoastDownCalculator
	"Idle Timeout (s)\0"
	"EOC Timeout (s)\0"
	"BtnPress T/O (s)\0"
	"Park Timeout (s)\0"
	"OFF Timeout (s)\0"
	"WakeEngRst CURR\0"
	"WakeBtnRst CURR\0"
#ifdef useSavedTrips
	"AutoSaveTrip 1-Y\0"
#endif // useSavedTrips
#ifdef useSerialDataLogging
	"DLogSerial 1-Yes\0"
#endif // useSerialDataLogging
#ifdef useWindowTripFilter
	"FE Filter 1-Y\0"
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	"FE/Time Period s\0"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"bgLower*1000 \xfbMPH\xfckph\xfd\0"
	"bgSize*1000 \xfbMPH\xfckph\xfd\0"
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
#ifdef useImperialGallon
	"Price*1000/\xfbImpGa\xfcL\xfd\0"
#else  // useImperialGallon
	"Price*1000/\xfbUSgal\xfcL\xfd\0"
#endif // useImperialGallon
#endif // useFuelCost
#ifdef useExpansionPins
	"OutPtPin 1 Mode\0"
	"OutPtPin 2 Mode\0"
#endif // useExpansionPins
	"Scratchpad(odo?)\0"
#ifdef usePartialRefuel
#ifdef useImperialGallon
	"Refill*1K \xfbImpGal\xfcL\xfd\0"
#else // useImperialGallon
	"Refill*1K \xfbUSgal\xfcL\xfd\0"
#endif // useImperialGallon
#endif // usePartialRefuel
};

const uint8_t paramsLength[] PROGMEM = {
	pSizeSignature							// EEPROM MPGuino signature long word
#ifdef useLegacyLCD
	,pSizeContrast							// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,pSizeLCDcolor							// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,pSizeMetricMode						// Display Mode (0 - US Display, 1 - Metric Display)
	,pSizeAlternateFE						// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useDragRaceFunction
	,pSizeDragSpeed							// Drag speed cutoff (MPH or kph) * 1000
	,pSizeDragDistance						// Drag distance cutoff (miles or km) * 1000
	,pSizeDragAutoFlag						// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction
	,pSizeInjEdgeTrigger 					// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
#ifdef useIsqrt
	,pSizeSysFuelPressure					// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,pSizeRefFuelPressure					// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,pSizeInjectorCount						// Fuel Injector Count
	,pSizeInjectorSize						// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,pSizeMicroSecondsPerGallon				// Microseconds per US gallon
	,pSizeInjectorOpeningTime				// Fuel Injector Opening Delay Time (us)
	,pSizeInjectorClosingTime				// Fuel Injector Closing Delay Time (us)
	,pSizePulsesPerDistance					// VSS Pulses (per mile or per km)
	,pSizeVSSpause							// VSS Pause Debounce Count (ms)
	,pSizeCrankRevPerInj					// Crankshaft Revolutions per Fuel Injector Event
	,pSizeMinGoodRPM						// Minimum Engine Speed For Engine On (RPM)
	,pSizeMinGoodSpeed						// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	,pSizeTankSize							// Tank Capacity * 1000 (gal or L)
	,pSizeTankBingoSize						// Bingo Fuel (reserve tank) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection
	,pSizeMAPsensorFloor					// MAP Sensor Floor * 1000 (V)
	,pSizeBaroSensorFloor					// Barometric Sensor Floor * 1000 (V)
	,pSizeMAPsensorCeiling					// MAP Sensor Ceiling * 1000 (V)
	,pSizeBaroSensorCeiling					// Barometric Sensor Ceiling * 1000 (V)
	,pSizeMAPsensorRange					// MAP Sensor Range (Pa or * 1000 psig)
	,pSizeBaroSensorRange					// Barometric Sensor Range (Pa or * 1000 psig)
	,pSizeMAPsensorOffset					// MAP Sensor Offset (Pa or * 1000 psia)
	,pSizeBaroSensorOffset					// Barometric Sensor Offset (Pa or * 1000 psia)
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
	,pSizeVehicleMass						// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,pSizeVehicleFrontalArea				// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,pSizeLocustDensity						// Air density (lb/yd^3 or kg/m^3)
	,pSizeCoefficientD						// Vehicle C(d) * 1000
	,pSizeCoefficientV						// Vehicle C(v) * 1000
	,pSizeCoefficientRR						// Vehicle C(rr) * 1000
	,pSizeCoastdownSamplePeriod				// Sample Period in seconds
#endif // useCoastDownCalculator
	,pSizeIdleTimeout						// Engine Idle Timeout (s)
	,pSizeEOCtimeout						// Engine-Off Coasting Timeout (s)
	,pSizeButtonTimeout						// Button Press Activity Timeout (s)
	,pSizeParkTimeout						// Vehicle Parked (engine off, no movement) Timeout (s)
	,pSizeActivityTimeout					// Activity (engine off, no movement, no button press) Timeout (s)
	,pSizeWakeupResetCurrentOnEngine		// Enable current trip reset upon wakeup due to engine running
	,pSizeWakeupResetCurrentOnButton		// Enable current trip reset upon wakeup due to button press
#ifdef useSavedTrips
	,pSizeAutoSaveActive					// Autosave Active Trip Data Enable
#endif // useSavedTrips
#ifdef useSerialDataLogging
	,pSizeSerialDataLogging					// Serial Data Logging Enable
#endif // useSerialDataLogging
#ifdef useWindowTripFilter
	,pSizeWindowTripFilter					// Window Trip Filter Enable
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,pSizeFEvsTime							// Period Of FE over Time BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,pSizeBarLowSpeedCutoff					// FE vs Speed Bargraph lower speed
	,pSizeBarSpeedQuantumIdx				// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,pSizeFuelUnitCost						// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,pSizeExpansionPin1Mode					// Output Pin 1 mode
	,pSizeExpansionPin2Mode					// Output Pin 2 mode
#endif // useExpansionPins
	,pSizeScratchpad						// Scratchpad Memory
#ifdef usePartialRefuel
	,pSizeRefuelSize						// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
};

const uint8_t paramAddrs[] PROGMEM = {
	(uint8_t)(pAddressSignature)			// EEPROM signature long word for MPGuino
#ifdef useLegacyLCD
	,(uint8_t)(pAddressContrast)			// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,(uint8_t)(pAddressLCDcolor)			// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,(uint8_t)(pAddressMetricMode)			// Display Mode (0 - US Display, 1 - Metric Display)
	,(uint8_t)(pAddressAlternateFE)			// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useDragRaceFunction
	,(uint8_t)(pAddressDragSpeed)			// Drag speed cutoff (MPH or kph) * 1000
	,(uint8_t)(pAddressDragDistance)		// Drag distance cutoff (miles or km) * 1000
	,(uint8_t)(pAddressDragAutoFlag)		// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction
	,(uint8_t)(pAddressInjEdgeTrigger)		// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
#ifdef useIsqrt
	,(uint8_t)(pAddressSysFuelPressure)		// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,(uint8_t)(pAddressRefFuelPressure)		// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,(uint8_t)(pAddressInjectorCount)		// Fuel Injector Count
	,(uint8_t)(pAddressInjectorSize)		// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,(uint8_t)(pAddressMicroSecondsPerGallon)	// Microseconds per US gallon
	,(uint8_t)(pAddressInjectorOpeningTime)	// Fuel Injector Opening Delay Time (us)
	,(uint8_t)(pAddressInjectorClosingTime)	// Fuel Injector Closing Delay Time (us)
	,(uint8_t)(pAddressPulsesPerDistance)	// VSS Pulses (per mile or per km)
	,(uint8_t)(pAddressVSSpause)			// VSS Pause Debounce Count (ms)
	,(uint8_t)(pAddressCrankRevPerInj)		// Crankshaft Revolutions per Fuel Injector Event
	,(uint8_t)(pAddressMinGoodRPM)			// Minimum Engine Speed For Engine On (RPM)
	,(uint8_t)(pAddressMinGoodSpeed)		// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	,(uint8_t)(pAddressTankSize)			// Tank Capacity * 1000 (gal or L)
	,(uint8_t)(pAddressTankBingoSize)		// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection
	,(uint8_t)(pAddressMAPsensorFloor)		// MAP Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressBaroSensorFloor)		// Barometric Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressMAPsensorCeiling)	// MAP Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressBaroSensorCeiling)	// Barometric Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressMAPsensorRange)		// MAP Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressBaroSensorRange)		// Barometric Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressMAPsensorOffset)		// MAP Sensor Offset (Pa or * 1000 psia)
	,(uint8_t)(pAddressBaroSensorOffset)	// Barometric Sensor Offset (Pa or * 1000 psia)
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
	,(uint8_t)(pAddressVehicleMass)			// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,(uint8_t)(pAddressVehicleFrontalArea)	// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,(uint8_t)(pAddressLocustDensity)		// Air density (lb/yd^3 or kg/m^3)
	,(uint8_t)(pAddressCoefficientD)		// Vehicle C(d) * 1000
	,(uint8_t)(pAddressCoefficientV)		// Vehicle C(v) * 1000
	,(uint8_t)(pAddressCoefficientRR)		// Vehicle C(rr) * 1000
	,(uint8_t)(pAddressCoastdownSamplePeriod)	// Sample Period in seconds
#endif // useCoastDownCalculator
	,(uint8_t)(pAddressIdleTimeout)			// Engine Idle Timeout (s)
	,(uint8_t)(pAddressEOCtimeout)			// Engine-Off Coasting Timeout (s)
	,(uint8_t)(pAddressButtonTimeout)		// Button Press Activity Timeout (s)
	,(uint8_t)(pAddressParkTimeout)			// Vehicle Parked (engine off, no movement) Timeout (s)
	,(uint8_t)(pAddressActivityTimeout)		// Activity (engine off, no movement, no button press) Timeout (s)
	,(uint8_t)(pAddressWakeupResetCurrentOnEngine)	// Enable current trip reset upon wakeup due to engine running
	,(uint8_t)(pAddressWakeupResetCurrentOnButton)	// Enable current trip reset upon wakeup due to button press
#ifdef useSavedTrips
	,(uint8_t)(pAddressAutoSaveActive)		// Autosave Active Trip Data Enable
#endif // useSavedTrips
#ifdef useSerialDataLogging
	,(uint8_t)(pAddressSerialDataLogging)	// Serial Data Logging Enable
#endif // useSerialDataLogging
#ifdef useWindowTripFilter
	,(uint8_t)(pAddressWindowTripFilter)	// Window Trip Filter Enable
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,(uint8_t)(pAddressFEvsTime)			// Period Of FE over Time Bar Graph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,(uint8_t)(pAddressBarLowSpeedCutoff)	// FE vs Speed Bargraph lower speed
	,(uint8_t)(pAddressBarSpeedQuantumIdx)	// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,(uint8_t)(pAddressFuelUnitCost)		// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,(uint8_t)(pAddressExpansionPin1Mode)	// Output Pin 1 mode
	,(uint8_t)(pAddressExpansionPin2Mode)	// Output Pin 2 mode
#endif // useExpansionPins
	,(uint8_t)(pAddressScratchpad)			// Scratchpad Memory
	,(uint8_t)(eeAdrSettingsEnd)			// Start address of next EEPROM logical block
#ifdef usePartialRefuel
	,(uint8_t)(pAddressRefuelSize)			// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
};

const uint32_t newEEPROMsignature = ((unsigned long)(guinosig) << 8) + (unsigned long)(eePtrEnd);

//typedef struct
//{
//
//	const uint8_t bitSize;
//	const uint32_t value;
//
//} parameter;
//
//static const parameter parameters[] PROGMEM = {
//	{16, newEEPROMsignature}	// EEPROM MPGuino signature long word
//#ifdef useLegacyLCD
//	,{8, 55}			// LCD Contrast
//#endif // useLegacyLCD
//#ifdef useAdafruitRGBLCDshield
//	,{3, 3}				// LCD Backlight color
//#endif // useAdafruitRGBLCDshield
//	,{1, 0}				// Display Mode (0 - US Display, 1 - Metric Display)
//	,{1, 0}				// 0 - MPG or L/100km, 1 - G/100mile or km/L
//#ifdef useDragRaceFunction
//	,{17, 60000}		// Drag speed cutoff (MPH or kph) * 1000
//	,{11, 250}			// Drag distance cutoff (miles or km) * 1000
//	,{1, 0}				// Drag automatic retrigger on vehicle stop
//#endif // useDragRaceFunction
//	,{1, 0}				// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
//#ifdef useIsqrt
//	,{24, 43500}		// Fuel System Pressure (Pa or * 1000 psig)
//#endif // useIsqrt
//#ifdef useCalculatedFuelFactor
//	,{24, 43500}		// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
//	,{8, 6}				// Fuel Injector Count
//	,{20, 246000}		// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
//#endif // useCalculatedFuelFactor
//	,{32, 133262651}	// Microseconds per US gallon
//	,{16, 550}			// Fuel Injector Response Delay Time (us)
//	,{16, 2000}			// Fuel Injector Closing Delay Time (us)
//	,{14, 10000}		// VSS Pulses (per mile or per km)
//	,{4, 2}				// VSS Pause Debounce Count (ms)
//	,{2, 2}				// Crankshaft Revolutions per Fuel Injector Event
//	,{16, 40}			// Minimum Engine Speed For Engine On (RPM)
//	,{24, 300}			// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
//	,{24, 18000}		// Tank Capacity * 1000 (gal or L)
//	,{24, 2200}			// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
//#ifdef useChryslerMAPCorrection
//	,{16, 0}			// MAP Sensor Floor * 1000 (V)
//	,{16, 0}			// Barometric Sensor Floor * 1000 (V)
//	,{16, 4500}			// MAP Sensor Ceiling * 1000 (V)
//	,{16, 4500}			// Barometric Sensor Ceiling * 1000 (V)
//	,{24, 14270}		// MAP Sensor Range (Pa or * 1000 psig)
//	,{24, 0}			// Barometric Sensor Range (Pa or * 1000 psig)
//	,{24, 551}			// MAP Sensor Offset (Pa or * 1000 psia)
//	,{24, 14696}		// Barometric Sensor Offset (Pa or * 1000 psia)
//#endif // useChryslerMAPCorrection
//#ifdef useVehicleMass
//	,{16, 4000}			// Vehicle Weight/Mass (lbs or kg)
//#endif // useVehicleMass
//#ifdef useCoastDownCalculator
//	,{16, 25400}		// Vehicle Frontal Area (ft^2 or m^2)
//	,{16, 2065}			// Air density (lb/yd^3 or kg/m^3)
//	,{16, 346}			// C(d)
//	,{16, 1}			// C(v)
//	,{16, 8}			// C(rr)
//	,{8, 2}				// Sample Period in seconds
//#endif // useCoastDownCalculator
//	,{8, 15}			// Engine Idle Timeout (s)
//	,{8, 2}				// Engine-Off Coasting Timeout (s)
//	,{8, 5}				// Button Press Activity Timeout (s)
//	,{8, 5}				// Vehicle Parked (engine off, no movement) Timeout (s)
//	,{8, 120}			// Activity (engine off, no movement, no button press) Timeout (s)
//	,{1, 1}				// Enable current trip reset upon wakeup due to engine running
//	,{1, 0}				// Enable current trip reset upon wakeup due to button press
//#ifdef useSavedTrips
//	,{1, 1}				// Autosave Active Trip Data Enable
//#endif // useSavedTrips
//#ifdef useSerialDataLogging
//	,{1, 1}				// Serial Data Logging Enable
//#endif // useSerialDataLogging
//#ifdef useWindowTripFilter
//	,{1, 1}				// Window Trip Filter Enable
//#endif // useWindowTripFilter
//#ifdef useBarFuelEconVsTime
//	,{16, 5}			// Length Of BarGraph Bar (s)
//#endif // useBarFuelEconVsTime
//#ifdef useBarFuelEconVsSpeed
//	,{17, 25000}		// FE vs Speed Bargraph lower speed
//	,{17, 5000}			// FE vs Speed Bargraph speed bar size
//#endif // useBarFuelEconVsSpeed
//#ifdef useFuelCost
//	,{16, 3799}			// Price per unit volume of fuel
//#endif // useFuelCost
//#ifdef useExpansionPins
//	,{3, 0}				// Output pin 0 mode
//	,{3, 0}				// Output pin 1 mode
//#endif // useExpansionPins
//	,{32, 0}			// Scratchpad Memory
//#ifdef usePartialRefuel
//	,{24, 0}			// Partial Refuel amount * 1000 (gal or L)
//#endif // usePartialRefuel
//};

const uint32_t params[] PROGMEM = {
	newEEPROMsignature	// EEPROM MPGuino signature long word
#ifdef useLegacyLCD
	,55					// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,3					// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,0					// Display Mode (0 - US Display, 1 - Metric Display)
	,0					// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useDragRaceFunction
	,60000				// Drag speed cutoff (MPH or kph) * 1000
	,250				// Drag distance cutoff (miles or km) * 1000
	,0					// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction
	,0					// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
#ifdef useIsqrt
	,43500				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,43500				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,6					// Fuel Injector Count
	,246000				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,133262651			// Microseconds per US gallon
	,550				// Fuel Injector Response Delay Time (us)
	,2000				// Fuel Injector Closing Delay Time (us)
	,10000				// VSS Pulses (per mile or per km)
	,2					// VSS Pause Debounce Count (ms)
	,2					// Crankshaft Revolutions per Fuel Injector Event
	,40					// Minimum Engine Speed For Engine On (RPM)
	,300				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	,18000				// Tank Capacity * 1000 (gal or L)
	,2200				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection
	,0					// MAP Sensor Floor * 1000 (V)
	,0					// Barometric Sensor Floor * 1000 (V)
	,4500				// MAP Sensor Ceiling * 1000 (V)
	,4500				// Barometric Sensor Ceiling * 1000 (V)
	,14270				// MAP Sensor Range (Pa or * 1000 psig)
	,0					// Barometric Sensor Range (Pa or * 1000 psig)
	,551				// MAP Sensor Offset (Pa or * 1000 psia)
	,14696				// Barometric Sensor Offset (Pa or * 1000 psia)
#endif // useChryslerMAPCorrection
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
#ifdef useSerialDataLogging
	,1					// Serial Data Logging Enable
#endif // useSerialDataLogging
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
#ifdef useFuelCost
	,3799				// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,0					// Output pin 1 mode
	,0					// Output pin 2 mode
#endif // useExpansionPins
	,0					// Scratchpad Memory
#ifdef usePartialRefuel
	,0					// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
};

// end of remarkably long EEPROM stored settings section

#ifdef useSerial0Port
void serial0::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#ifdef useBufferedSerial0Port

	ringBuffer::init(serial0Buffer, serial0Data);
#endif // useBufferedSerial0Port

#ifdef useATmega2560
	PRR0 &= ~(1 << PRUSART0); // turn on USART transmitter
#endif // useATmega2560
#ifdef useATmega128
	PRR &= ~(1 << PRUSART0); // turn on USART transmitter
#endif // useATmega128
	UBRR0H = (uint8_t)(myubbr0 >> 8); // set serial uart baud rate
	UBRR0L = (uint8_t)(myubbr0);
	UCSR0A &= ~((1 << TXC0) | (1 << UDRE0) | (1 << U2X0));
	UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00)); // set for 8 data bits, no parity, and 1 stop bit
	UCSR0B |= (1 << TXEN0); // enable USART0 transmitter

	devSerial0.charOut = charOut;
	devSerial0.controlFlags |= (odvFlagCRLF);

	SREG = oldSREG; // restore interrupt flag status

}

void serial0::shutdown(void)
{

	UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disable USART0 transmitter and data register empty interrupt
#ifdef useATmega2560
	PRR0 |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // useATmega2560
#ifdef useATmega128
	PRR |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // useATmega128

}

void serial0::charOut(uint8_t chr)
{

#ifdef useBufferedSerial0Port
	ringBuffer::push(serial0Buffer, chr);
	UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
#else // useBufferedSerial0Port
	while ((UCSR0A & (1 << UDRE0)) == 0); // wait until USART0 data buffer is empty

	UCSR0A &= ~(1 << TXC0); // clear transmit complete flag
	UDR0 = chr; //send the data
#endif // useBufferedSerial0Port

}

#ifdef useBufferedSerial0Port
// triggered once serial hardware is ready to receive another character for transmission
#ifdef useATmega2560
ISR( USART0_UDRE_vect ) // called whenever USART data buffer empties
#endif // useATmega2560
#ifdef useATmega128
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // useATmega128
{

	if (ringBuffer::isBufferNotEmpty(serial0Buffer)) UDR0 = ringBuffer::pull(serial0Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR0B &= ~(1 << UDRIE0); // otherwise, disable data register empty interrupt

}

#endif // useBufferedSerial0Port
#endif // useSerial0Port
#ifdef useSerial1Port
void serial1::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#ifdef useBufferedSerial1Port

	ringBuffer::init(serial1Buffer, serial1Data);
#endif // useBufferedSerial1Port

	PRR1 &= ~(1 << PRUSART1); // turn on USART1
	UBRR1H = (uint8_t)(myubbr1 >> 8); // set serial uart baud rate
	UBRR1L = (uint8_t)(myubbr1);
	UCSR1A &= ~((1 << TXC1) | (1 << UDRE1) | (1 << U2X1));
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10)); // set for 8 data bits, no parity, and 1 stop bit
	UCSR1B |= (1 << TXEN1); // enable USART1 transmitter

	devSerial1.charOut = charOut;
	devSerial1.controlFlags |= (odvFlagCRLF);

	SREG = oldSREG; // restore interrupt flag status

}

void serial1::shutdown(void)
{

	UCSR1B &= ~((1 << TXEN1) | (1 << UDRIE1)); // disable USART1 transmitter and data register empty interrupt
	PRR1 |= (1 << PRUSART1); // shut off USART1 for power reduction

}

void serial1::charOut(uint8_t chr)
{

#ifdef useBufferedSerial1Port
	ringBuffer::push(serial1Buffer, chr);
#else // useBufferedSerial1Port
	while ((UCSR1A & (1 << UDRE1)) == 0); // wait until USART1 data buffer buffer is empty

	UCSR1A &= ~(1 << TXC1); // clear transmit complete flag
	UDR1 = chr; //send the data
#endif // useBufferedSerial1Port

}

#ifdef useBufferedSerial1Port
// triggered once serial hardware is ready to receive another character for transmission
#ifdef useATmega32U4
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // useATmega32U4
#ifdef useATmega2560
ISR( USART1_UDRE_vect ) // called whenever USART data buffer empties
#endif // useATmega2560
{

	if (ringBuffer::isBufferNotEmpty(serial1Buffer)) UDR1 = ringBuffer::pull(serial1Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR1B &= ~(1 << UDRIE1); // otherwise, disable data register empty interrupt

}

#endif // useBufferedSerial1Port
#endif // useSerial1Port
#ifdef useTWIsupport
const uint8_t TWI_READ =				0;
const uint8_t TWI_WRITE =				1;

const uint8_t TWI_REPEAT_START =		0;
const uint8_t TWI_STOP =				1;

const uint8_t TWI_MAIN_PROGRAM =		0;
const uint8_t TWI_INTERRUPT =			1;

const unsigned int twiFrequency = 100L;
const uint8_t twiMasterBufferSize = 16;
const uint8_t twiSlaveRxBufferSize = 16;
const uint8_t twiSlaveTxBufferSize = 16;

static uint8_t twiMasterBufferData[twiMasterBufferSize];
static uint8_t twiSlaveTxBufferData[twiSlaveTxBufferSize];
static uint8_t twiSlaveRxBufferData[twiSlaveRxBufferSize];

volatile uint8_t twiStatusFlags;

volatile uint8_t twiSlaveAddress;
volatile uint8_t twiErrorCode;

volatile uint8_t twiMasterBufferIdx;
volatile uint8_t twiMasterBufferLen;
volatile uint8_t twiSlaveTxBufferIdx;
volatile uint8_t twiSlaveTxBufferLen;
volatile uint8_t twiSlaveRxBufferIdx;

const uint8_t twiMasterTransmitter =	0b10000000;
const uint8_t twiMasterReceiver =		0b01000000;
const uint8_t twiSlaveTransmitter =		0b00100000;
const uint8_t twiSlaveReceiver =		0b00010000;
const uint8_t twiSendStop =				0b00001000;
const uint8_t twiRepeatStart =			0b00000100;
const uint8_t twiErrorFlag =			0b00000010;
const uint8_t twiBlockMainProgram =		0b00000001;

const uint8_t twiTxRxStatus =			twiMasterTransmitter | twiMasterReceiver | twiSlaveTransmitter | twiSlaveReceiver;
const uint8_t twiTxRxStatusMain =		twiTxRxStatus | twiBlockMainProgram;

ISR( TWI_vect )
{

	static uint8_t twiStatus;

	twiStatus = TW_STATUS;

	switch (twiStatus)
	{

		case TW_START:     // sent start condition
		case TW_REP_START: // sent repeated start condition
			TWDR = twiSlaveAddress; // copy device address and r/w bit to output register and send ACK
			TWI::reply(1);
			break;

		case TW_MT_SLA_ACK:  // slave receiver ACKed address
		case TW_MT_DATA_ACK: // slave receiver ACKed data
			if(twiMasterBufferIdx < twiMasterBufferLen) // if there is data to send, send it
			{

				TWDR = twiMasterBufferData[twiMasterBufferIdx++]; // copy data to output register
				TWI::reply(1); // ack

			}
			else TWI::stopOrRepeatStart();// otherwise, stop (or do repeated start)
			break;

		case TW_MR_DATA_NACK: // data received, nack sent
			twiMasterBufferData[twiMasterBufferIdx++] = TWDR; // put final byte into buffer
			TWI::stopOrRepeatStart();// stop (or do repeated start)
			break;

		case TW_MT_ARB_LOST: // lost bus arbitration (also handles TW_MR_ARB_LOST)
			twiStatusFlags |= twiErrorFlag; // set error condition
			twiErrorCode = TW_MT_ARB_LOST;
			TWI::releaseBus();
			break;

		case TW_MR_DATA_ACK: // data received, ACK sent
			twiMasterBufferData[(unsigned int)(twiMasterBufferIdx++)] = TWDR; // put byte into buffer
		case TW_MR_SLA_ACK:  // address sent, ACK received
			if(twiMasterBufferIdx < twiMasterBufferLen) TWI::reply(1); // ack if more bytes are expected
			else TWI::reply(0); // otherwise, send NEGATIVE ACK
			break;

		case TW_MR_SLA_NACK: // address sent, NEGATIVE ACK received
			TWI::hardStop();
			break;

		case TW_SR_SLA_ACK:   // addressed, returned ACK
		case TW_SR_GCALL_ACK: // addressed generally, returned ACK
		case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ACK
		case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ACK
			twiStatusFlags |= twiSlaveReceiver; // enter slave receiver mode
			twiSlaveRxBufferIdx = 0; // indicate that Rx buffer can be overwritten
			TWI::reply(1); // send ACK condition
			break;

		case TW_SR_DATA_ACK:       // data received, returned ack
		case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
			if(twiSlaveRxBufferIdx < twiSlaveRxBufferSize) // if there is still room in the rx buffer
			{

				twiSlaveRxBufferData[(unsigned int)(twiSlaveRxBufferIdx++)] = TWDR; // put byte in buffer
				TWI::reply(1); // send ACK condition

			}
			else TWI::reply(0); // otherwise nack
			break;

		case TW_SR_STOP: // stop or repeated start condition received
			if(twiSlaveRxBufferIdx < twiSlaveRxBufferSize) twiSlaveRxBufferData[(unsigned int)(twiSlaveRxBufferIdx)] = '\0'; // put a null char after data if there's room
			TWI::hardStop(); // send ACK, and stop interface for clock stretching
			if (TWI::onSlaveReceive) // if user-defined callback exists
			{

				TWI::onSlaveReceive(twiSlaveRxBufferData, twiSlaveRxBufferIdx); // callback to user defined callback
				twiSlaveRxBufferIdx = 0; // since Rx buffer submitted to user callback, it can be reset

			}
			TWI::releaseBus(); // ack future responses and leave slave receiver state
			break;

		case TW_SR_DATA_NACK:       // data received, returned nack
		case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
			TWI::reply(0); // send NEGATIVE ACK condition to master
			break;

		case TW_ST_SLA_ACK:          // addressed, returned ack
		case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
			twiStatusFlags |= twiSlaveTransmitter; // enter slave transmitter mode
			twiSlaveTxBufferIdx = 0; // ready the tx buffer index for iteration
			twiSlaveTxBufferLen = 0; // set tx buffer length to be zero, to verify if main program changes it

			// request for txBuffer to be filled and length to be set
			// note: main program must call TWI::transmit(bytes, length) to do this
			if (TWI::onSlaveTransmit) TWI::onSlaveTransmit();

			if(twiSlaveTxBufferLen == 0) // if main program didn't change buffer & length, initialize it
			{

				twiSlaveTxBufferLen = 1;
				twiSlaveTxBufferData[0] = 0x00;

			} // fall through to transmit first byte from buffer
		case TW_ST_DATA_ACK: // byte sent, ack returned
			TWDR = twiSlaveTxBufferData[(unsigned int)(twiSlaveTxBufferIdx++)]; // copy data to output register
			if(twiSlaveTxBufferIdx < twiSlaveTxBufferLen) TWI::reply(1); // if there is more to send, send ACK condition
			else TWI::reply(0); // otherwise send NEGATIVE ACK condition
			break;

		case TW_ST_DATA_NACK: // received NEGATIVE ACK, we are done
		case TW_ST_LAST_DATA: // received ACK, but we are already done
			TWI::reply(1); // send ACK condition
			twiStatusFlags &= ~(twiTxRxStatus); // set TWI state to ready
			break;

		case TW_MT_SLA_NACK:  // address sent, NEGATIVE ACK received
		case TW_MT_DATA_NACK: // data sent, NEGATIVE ACK received
		case TW_BUS_ERROR: // bus error, illegal stop/start
			twiStatusFlags |= twiErrorFlag; // set error condition
			twiErrorCode = twiStatus;
			TWI::hardStop();
			break;

		case TW_NO_INFO: // no state information
		default:
			break;

	}

}

#endif // useTWIsupport
volatile uint8_t lastPINxState;

volatile uint8_t VSScount; // for VSS debouncing
volatile uint8_t VSSpause; // for VSS debouncing

volatile uint8_t buttonState;
#ifdef useTestButtonValues
volatile uint8_t buttonIdx;
#endif // useTestButtonValues

volatile uint8_t mainLoopHeartBeat;

volatile unsigned long timer0_overflow_count;
volatile unsigned long systemCycles[2] = { 0, 0 };
#ifdef useSoftwareClock
volatile unsigned long clockCycles[2] = { 0, 0 };
#endif // useSoftwareClock

volatile unsigned int watchdogInjectorCount;
volatile unsigned int watchdogVSSCount;
volatile unsigned int buttonCount;
volatile unsigned int timer0DelayCount;
volatile unsigned int displayPauseCount;

// these flags specifically tell the system timer0 to do something
// main program sets flag, system timer0 acknowledges by clearing flag
volatile uint8_t timer0Command;

const uint8_t t0cResetTimer =			0b10000000;
const uint8_t t0cDoDelay =				0b01000000;
const uint8_t t0cDisplayDelay =			0b00100000;
#ifdef useAnalogButtons
const uint8_t t0cProcessAnalogButton =	0b00010000;
#endif // useAnalogButtons
#ifdef useTWIsupport
const uint8_t t0cEnableTWIsample =		0b00000001;
#endif // useTWIsupport

// these flags specifically tell the main program to do something
// system timer0 sets flag, main program acknowledges by clearing flag
volatile uint8_t timer0Status;

const uint8_t t0sActivityChange =		0b10000000;
const uint8_t t0sUpdateDisplay =		0b01000000;
const uint8_t t0sShowCursor =			0b00100000;
const uint8_t t0sReadButton =			0b00010000;
const uint8_t t0sTakeSample =			0b00001000;
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

const uint8_t aPerformFallAsleep =		0b00000100;
const uint8_t aPerformPark =			0b00000010;
const uint8_t aPerformWakeUp =			0b00000001;

const uint8_t aAwake =					(aAwakeOnInjector | aAwakeOnVSS | aAwakeOnButton);
const uint8_t aAwakeOnVehicle =			(aAwakeOnInjector | aAwakeOnVSS);

// these status flags inform the main program about MPGuino sensor activity
volatile uint8_t activityFlags;

const uint8_t afEngineOffFlag =			0b10000000;
const uint8_t afVehicleStoppedFlag =	0b01000000;
const uint8_t afButtonFlag =			0b00100000;
const uint8_t afParkFlag =				0b00010000;
const uint8_t afActivityTimeoutFlag =	0b00001000;
const uint8_t afSwapFEwithFCR =			0b00000100;
#ifdef useTWIsupport
const uint8_t afTWIsampleActive =		0b00000010;
#endif // useTWIsupport

const uint8_t afValidFlags =			(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag | afButtonFlag | afActivityTimeoutFlag);
const uint8_t afActivityCheckFlags =	(afEngineOffFlag | afVehicleStoppedFlag | afButtonFlag);
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
#ifdef useAnalogInterrupt
volatile uint8_t analogCommand;

const uint8_t acResetInterrupt =		0b10000000;
const uint8_t acSampleGround =			0b01000000;
#ifdef useAnalogRead
#ifdef useATmega32U4
const uint8_t acSampleChannel2 =		0b00000100;
const uint8_t acSampleChannel1 =		0b00000010;
const uint8_t acSampleChannel0 =		0b00000001;

#ifdef useChryslerMAPCorrection
const uint8_t acSampleBaroChannel =		0b00000100;
const uint8_t acSampleMAPchannel =		0b00000010;
#endif // useChryslerMAPCorrection
#ifdef useAnalogButtons
const uint8_t acSampleButtonChannel =	0b00000001;
#endif // useAnalogButtons

#endif // useATmega32U4
#ifdef useATmega2560
#ifdef useAnalogButtons
#ifndef useTWIsupport
const uint8_t acSampleChannel4 =		0b00010000;
const uint8_t acSampleChannel3 =		0b00001000;
#endif // useTWIsupport
const uint8_t acSampleChannel2 =		0b00000100;
#endif // useAnalogButtons
const uint8_t acSampleChannel1 =		0b00000010;
const uint8_t acSampleChannel0 =		0b00000001;

#ifdef useAnalogButtons
const uint8_t acSampleButtonChannel =	0b00000100;
#endif // useAnalogButtons
#ifdef useChryslerMAPCorrection
const uint8_t acSampleBaroChannel =		0b00000010;
const uint8_t acSampleMAPchannel =		0b00000001;
#endif // useChryslerMAPCorrection

#endif // useATmega2560
#ifdef useATmega128
#ifdef useAnalogButtons
#ifndef useTWIsupport
const uint8_t acSampleChannel4 =		0b00010000;
const uint8_t acSampleChannel3 =		0b00001000;
#endif // useTWIsupport
const uint8_t acSampleChannel2 =		0b00000100;
#endif // useAnalogButtons
const uint8_t acSampleChannel1 =		0b00000010;
const uint8_t acSampleChannel0 =		0b00000001;

#ifdef useAnalogButtons
const uint8_t acSampleButtonChannel =	0b00000100;
#endif // useAnalogButtons
#ifdef useChryslerMAPCorrection
const uint8_t acSampleBaroChannel =		0b00000010;
const uint8_t acSampleMAPchannel =		0b00000001;
#endif // useChryslerMAPCorrection

#endif // useATmega128
#endif // useAnalogRead

volatile uint8_t analogStatus;

const uint8_t asReadChannel =			0b10000000;
const uint8_t acSampleChannelActive =	0b00111111; // invalid bits, as previously defined above, will simply be ignored

#ifdef useAnalogButtons
volatile uint8_t analogButton;
#ifdef useTestButtonValues
volatile uint8_t analogButtonIdx;
#endif // useTestButtonValues

#endif // useAnalogButtons
#endif // useAnalogInterrupt
const uint8_t internalButtonRead =			0b00010000;
#ifdef useTWIsupport
const uint8_t internalSampleTWI =			0b00001000;
#endif // useTWIsupport
#ifdef useAnalogRead
const uint8_t internalSampleADC =			0b00000100;
#endif // useAnalogRead
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

	static uint8_t lastButtonPressed;
	static uint8_t thisButtonPressed;
	static uint8_t previousActivity;
#ifdef useTWIbuttons
	static uint8_t lastTWIbuttonPressed;
#endif // useTWIbuttons
	static uint8_t internalFlags;
#ifdef useTWIsupport
	static uint8_t TWIsampleCount;
	static uint8_t TWIsampleState;
#endif // useTWIsupport
	static unsigned int cursorCount;
	static unsigned int loopCount = 0;
#ifdef useJSONoutput
	static unsigned int JSONtimeoutCount = 0;
#endif // useJSONoutput
	static unsigned long lastTime;
	static unsigned long buttonTimeoutCount = 0;
	static unsigned long parkTimeoutCount = 0;
	static unsigned long activityTimeoutCount = 0;
	static unsigned long swapFEwithFCRcount = 0;
#ifdef useCoastDownCalculator
	static unsigned long coastdownCount = 0;
#endif // useCoastDownCalculator
#ifdef useAnalogRead
	static unsigned int analogSampleCount;
#ifdef useAnalogButtons
	static uint8_t thisAnalogButtonPressed;
#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useTWIbuttons
	static uint8_t thisTWIbuttonPressed;
#endif // useTWIbuttons
	unsigned long thisTime;
	unsigned long cycleLength;

	if (timer0Command & t0cResetTimer)
	{

		timer0Command &= ~(t0cResetTimer);
		timer0_overflow_count = 0; // initialize timer 0 overflow counter
		timer0Status = 0;
		loopCount = loopTickLength;
		lastButtonPressed = 0;
		awakeFlags = 0;
		internalFlags = 0;
		mainLoopHeartBeat = 1;
		watchdogInjectorCount = 0;
		dirty &= ~(dGoodVehicleDrive);
		buttonCount = 0;
		activityFlags = (afEngineOffFlag | afVehicleStoppedFlag | afSwapFEwithFCR);
		previousActivity = 0;
#ifdef useTWIsupport
		TWIsampleCount = 0;
		TWIsampleState = 0;
#endif // useTWIsupport
#ifdef useAnalogRead
		analogSampleCount = analogSampleTickLength;
#endif // useAnalogRead

	}
	else
	{

		timer0_overflow_count += 256; // update TOV count
		thisTime = timer0_overflow_count | TCNT0; // calculate current cycle count

		cycleLength = findCycleLength(lastTime, thisTime);

		systemCycles[0] += cycleLength;
		if (systemCycles[0] < cycleLength) systemCycles[1]++;

#ifdef useSoftwareClock
		clockCycles[0] += cycleLength;
		if (clockCycles[0] < cycleLength) clockCycles[1]++;

#endif // useSoftwareClock
	}

	lastTime = thisTime; // save cycle count

	if (VSScount) // if there is a VSS debounce countdown in progress
	{

		VSScount--; // bump down the VSS count
		if (VSScount == 0) updateVSS(thisTime); // if count has reached zero, go update VSS

	}

	if (awakeFlags & aAwakeOnInjector) // if MPGuino is awake on detected fuel injector event
	{

		if (watchdogInjectorCount) // if the fuel injector watchdog timer is running on minimum good engine speed
		{

			watchdogInjectorCount--; // cycle fuel injector watchdog timer down

#ifdef useChryslerMAPCorrection
			if (dirty & dSampleADC) // if injector monitor commanded an analog engine sensor read
			{

				dirty &= ~(dSampleADC); // acknowledge the command
				analogCommand |= (acSampleMAPchannel | acSampleBaroChannel);
				internalFlags |= internalSampleADC; // signal to ADC module to read engine sensors

			}

#endif // useChryslerMAPCorrection
		}
		else // fuel injector watchdog timer has timed out
		{

			awakeFlags &= ~(aAwakeOnInjector); // signal that MPGuino is not awake any more due to no detected injector event during injector watchdog period
			dirty &= ~(dGoodEngineRun); // reset all fuel injector measurement flags
			watchdogInjectorCount = volatileVariables[(unsigned int)(vEngineOffTimeoutIdx)]; // start the fuel injector watchdog for engine off mode

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
			watchdogVSSCount = volatileVariables[(unsigned int)(vVehicleStopTimeoutIdx)]; // start the VSS watchdog for vehicle stopped mode
			swapFEwithFCRcount = swapFEwithFCRDelay; // reset swap timer counter

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

	if ((activityFlags & afParkCheckFlags) == afNotParkedFlags) // if MPGuino has engine stop and vehicle stop flags set, but is not yet parked
	{

		if (parkTimeoutCount) parkTimeoutCount--; // run down park watchdog timer until it expires
		else activityFlags |= (afParkFlag); // set vehicle parked flag

	}

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
				coastdownCount = volatileVariables[(unsigned int)(vCoastdownPeriodIdx)]; // reset coastdown counter

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
						else coastdownCount = volatileVariables[(unsigned int)(vCoastdownPeriodIdx)]; // reset coastdown counter

					}
					else coastdownFlags |= cdTakeSample;

				}

			}

		}

	}

#endif // useCoastDownCalculator
#ifdef useTWIsupport
	if (internalFlags & internalSampleTWI)
	{

		switch (TWIsampleState)
		{

			case 0:
				if ((twiStatusFlags & twiTxRxStatus) == 0) // wait for TWI to become available
				{

#ifdef useAdafruitRGBLCDshield
					TWI::initWrite(buttonAddress, TWI_REPEAT_START, TWI_INTERRUPT); // open TWI as master transmitter, do repeated start to set up for read, flag as driven within interrupt
					TWI::writeByte(MCP23017_B1_GPIOA); // specify bank A GPIO pin register address
					TWI::commitWrite(false); // go write out read request
#endif // useAdafruitRGBLCDshield
					TWIsampleState++; // advance to waiting for TWI sample request to finish

				}

			case 1:
				if ((twiStatusFlags & twiTxRxStatus) == 0) // wait for TWI button pin register address write to complete
				{

#ifdef useAdafruitRGBLCDshield
					TWI::initMasterDevice(buttonAddress, TWI_READ, TWI_STOP, TWI_INTERRUPT); // initialize master receiver, flag as driven within interrupt
					twiMasterBufferLen = 0; // go look at comments for TWI::readDevice to see why this is zero
					TWI::commitMasterDevice(false); // go commit to read
#endif // useAdafruitRGBLCDshield

					TWIsampleState++; // advance to waiting for TWI sample to complete

				}
				break;

			case 2:
				if ((twiStatusFlags & twiTxRxStatus) == 0) // wait for TWI button read to complete
				{

					TWIsampleState++; // advance to debouncing initialization

					if (twiStatusFlags & twiErrorFlag) TWIsampleState = 255; // if an error occurred during read, exit
					else
					{

#ifdef useTWIbuttons
#ifdef useAdafruitRGBLCDshield
						thisTWIbuttonPressed = twiMasterBufferData[0]; // fetch button state that was just read in
						thisTWIbuttonPressed &= (buttonMask);
						if (thisTWIbuttonPressed ^ lastTWIbuttonPressed) buttonCount = buttonDelayTick;
						lastTWIbuttonPressed = thisTWIbuttonPressed;
#endif // useAdafruitRGBLCDshield
#endif // useTWIbuttons

					}

				}
			case 3:
			default:
				activityFlags &= ~(afTWIsampleActive);
				internalFlags &= ~(internalSampleTWI);
				twiStatusFlags &= ~(twiBlockMainProgram);
				break;

		}

	}
	else
	{

		if (timer0Command & t0cEnableTWIsample)
		{

			if (TWIsampleCount) TWIsampleCount--;
			else
			{

				TWIsampleCount = TWItickLength;
				activityFlags |= afTWIsampleActive;
				internalFlags |= internalSampleTWI; // signal that an internal TWI session is in progress
				twiStatusFlags |= twiBlockMainProgram; // block main program from making any TWI requests
				TWIsampleState = 0; // initialize TWI button read state machine

			}

		}
		else TWIsampleCount = 0;

	}

#endif // useTWIsupport
#ifdef useAnalogButtons
	if (timer0Command & t0cProcessAnalogButton)
	{

		timer0Command &= ~(t0cProcessAnalogButton);
		if (thisAnalogButtonPressed != analogButton) buttonCount = buttonDelayTick; //
		thisAnalogButtonPressed = analogButton;
#ifdef useTestButtonValues
		thisButtonIdx = analogButtonIdx;
#endif // useTestButtonValues

	}

#endif // useAnalogButtons
	if (buttonCount) // if there is a button press debounce countdown in progress
	{

		buttonCount--; // bump down the button press count by one

		if (buttonCount == 0) lastButtonPressed |= longButtonBit; // signal that a "long" button press has been detected

		if (buttonCount == buttonShortDelayTick) // if button debounce countdown reaches this point
		{

			// figure out what buttons are being pressed
#ifdef useLegacyButtons
			thisButtonPressed = lastPINxState & buttonMask; // strip out all but relevant button bits
#ifdef useTestButtonValues
			thisButtonIdx = thisButtonPressed;
#endif // useTestButtonValues
			thisButtonPressed ^= buttonMask; // reverse bit pattern so that "button down" = 1 and "button up" = 0
#endif // useLegacyButtons
#ifdef useAnalogButtons
			thisButtonPressed = thisAnalogButtonPressed;
#ifdef useTestButtonValues
			thisButtonIdx = thisAnalogButtonIdx;
#endif // useTestButtonValues
#endif // useAnalogButtons
#ifdef useTWIbuttons
			thisButtonPressed = thisTWIbuttonPressed;
#ifdef useTestButtonValues
			thisButtonIdx = thisButtonPressed;
#endif // useTestButtonValues
#endif // useTWIbuttons

			if (thisButtonPressed != buttonsUp) // if any buttons are pressed
			{

				lastButtonPressed = thisButtonPressed; // remember the button press status for later
#ifdef useTestButtonValues
				lastButtonIdx = thisButtonIdx;
#endif // useTestButtonValues
				internalFlags |= internalButtonRead; // signal that a button has been read in

			}
			else buttonCount = 0; // reset button press debounce countdown to zero

		}

		if (buttonCount == 0) // if a button has been read, go pass it on to the main program
		{

			awakeFlags |= (aAwakeOnButton); // set awake status on button pressed

			// if a valid button press was read in, and main program is awake
			if ((internalFlags & internalButtonRead) && (awakeFlags & aAwake))
			{

				internalFlags &= ~(internalButtonRead);
				buttonState = lastButtonPressed; // pass off the remembered button press status to the main program
#ifdef useTestButtonValues
				buttonIdx = lastButtonIdx;
#endif // useTestButtonValues
				timer0Command &= ~(t0cDisplayDelay); // shut off display change delay
				timer0Status |= (t0sReadButton | t0sShowCursor); // force cursor show bit, and signal that keypress was detected
				cursorCount = cursorDelay; // reset cursor count
				activityFlags &= ~(afButtonFlag | afActivityTimeoutFlag);
				buttonTimeoutCount = volatileVariables[(unsigned int)(vButtonTimeoutIdx)];

			}

		}

	}

#ifdef useAnalogRead
	if (analogSampleCount) analogSampleCount--;
	else
	{

		analogSampleCount = analogSampleTickLength;
		analogCommand |= (acSampleChannelActive); // go sample them all
		internalFlags |= internalSampleADC;

	}

#endif // useAnalogRead
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

		timer0Status |= (t0sUpdateDisplay | t0sTakeSample); // signal to main program that a sampling should occur, and to update display
		loopCount = loopTickLength; // restart loop count
		mainLoopHeartBeat <<= 1; // cycle the heartbeat bit
		if (mainLoopHeartBeat == 0) mainLoopHeartBeat = 1; // wrap around the heartbeat bit, if necessary

	}

	if (cursorCount) cursorCount--;
	else
	{

		cursorCount = cursorDelay; // reset cursor count
		timer0Status ^= t0sShowCursor; // toggle cursor show bit

	}

	if (timer0Command & t0cDisplayDelay) // if message pause is in effect
	{

		if (displayPauseCount) displayPauseCount--; // update pause counter
		else
		{

			timer0Command &= ~(t0cDisplayDelay); // otherwise, signal that message pause is over
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

	if ((activityFlags & afValidFlags) == afActivityCheckFlags) // if there is no activity but the activity watchdog hasn't timed out yet
	{

		if (activityTimeoutCount) activityTimeoutCount--; // cycle down the activity timeout watchdog
		else activityFlags |= (afActivityTimeoutFlag); // signal that MPGuino is in a period of inactivity

	}

	previousActivity ^= (activityFlags & afValidFlags); // detect any activity change since last timer0 tick

	if (previousActivity) timer0Status |= (t0sActivityChange | t0sUpdateDisplay); // if there was any activity change at all, signal that the display needs updating

	if (previousActivity & afActivityCheckFlags) // if any of the fuel injector, VSS pulse, button press, or park flags have changed
	{

		if (activityFlags & afActivityCheckFlags) activityTimeoutCount = volatileVariables[(unsigned int)(vActivityTimeoutIdx)]; // reset activity timeout watchdog if any activity was detected

	}

	if (previousActivity & afEngineOffFlag)
	{

		if (activityFlags & afEngineOffFlag) // if engine off flag was just set
		{

			parkTimeoutCount = volatileVariables[(unsigned int)(vParkTimeoutIdx)]; // reset park counter

		}

	}

	if (previousActivity & afVehicleStoppedFlag)
	{

		if (activityFlags & afVehicleStoppedFlag) // if vehicle stop flag was just set
		{

			parkTimeoutCount = volatileVariables[(unsigned int)(vParkTimeoutIdx)]; // reset park counter

		}

	}

	if (previousActivity & afParkFlag)
	{

		// if vehicle park flag was just set (engine off and vehicle stopped and park timeout occurred)
		if (activityFlags & afParkFlag) awakeFlags |= (aPerformPark);

	}

	if (previousActivity & afActivityTimeoutFlag) // if the activity timeout flag has changed
	{

		if (activityFlags & afActivityTimeoutFlag) awakeFlags |= (aPerformFallAsleep);
		else awakeFlags |= (aPerformWakeUp);

	}

	previousActivity = (activityFlags & afValidFlags); // save for next timer0 tick

#ifdef useAnalogRead
	if (internalFlags & internalSampleADC)
	{

		internalFlags &= ~(internalSampleADC);
		if ((ADCSRA & (1 << ADSC)) == 0) // if ADC read is not already in progress
		{

			analogCommand |= (acSampleGround); // signal to ADC interrupt that the last conversion was for internal ground
			analogStatus &= ~(asReadChannel);
#ifdef useATmega32U4
			ADMUX = (1 << REFS0) | (1 << MUX4) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif // useATmega32U4
#ifdef useATmega2560
			ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif // useATmega2560
#ifdef useATmega128
			ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif // useATmega128
			ADCSRB = 0; // set ADMUX[5] = 0
			ADCSRA |= (1 << ADSC); // start ADC read

		}

	}

#endif // useAnalogRead
}

#ifdef useTimer1
#ifdef use4BitLCD
outputDevice devLCD;

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

const unsigned long debugVSSresetLength = (unsigned long)(ceil)((2ul * systemProcessorSpeed * 1000000ul) / (2ul * 255ul)) - 1; // 2 sec
const unsigned long debugFIPresetLength = (unsigned long)(ceil)((4ul * systemProcessorSpeed * 1000000ul) / (3ul * 2ul * 255ul)) - 1; // 4/3 sec

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
#ifdef useATmega32U4
				PORTB ^= (1 << PINB7); // generate VSS pin interrupt
#endif // useATmega32U4
#ifdef useATmega2560
				PORTK ^= (1 << PINK0); // generate VSS pin interrupt
#endif // useATmega2560
#ifdef useATmega128
				PORTC ^= (1 << PINC0); // generate VSS pin interrupt
#endif // useATmega128

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

#ifdef useATmega32U4
					PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to generate injector closed interrupt
#endif // useATmega32U4
#ifdef useATmega2560
					PORTE |= ((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin high to generate injector closed interrupt
#endif // useATmega2560
#ifdef useATmega128
					PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to generate injector closed interrupt
#endif // useATmega128

				}

			}
			else
			{

				debugFIPcount = debugFIPtickLength;
				debugFIPWcount = debugFIPWtickLength;
				if (debugFIPWtickLength) // if DFCO is not commanded
				{

#ifdef useATmega32U4
					PORTD &= ~((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin low to generate injector open interrupt
#endif // useATmega32U4
#ifdef useATmega2560
					PORTE &= ~((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin low to generate injector open interrupt
#endif // useATmega2560
#ifdef useATmega128
					PORTD &= ~((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin low to generate injector open interrupt
#endif // useATmega128

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
				// if buffer is not empty and TWI hardware is ready and there's no interrupt-driven TWI button read in progress
				if (((twiStatusFlags & twiTxRxStatusMain) == 0) && ((activityFlags & afTWIsampleActive) == 0))
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

								TWI::initWrite(lcdAddress, TWI_STOP, TWI_INTERRUPT); // open TWI as master transmitter
								TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address
								timer1Status |= (t1sDoOutputTWI); // signal to complete TWI master transmission

							}

						}

						LCD::outputNybble(value); // output the nybble and set timing

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((value & lcdSendFlags) == lcdSendNybble) // if sending an ordinary data nybble, check if we can continue looping
							{

								if ((twiMasterBufferSize - twiMasterBufferLen) < 5) timer1Status &= ~(t1sLoopFlag); // if TWI send buffer is getting low, signal end of loop
								if (ringBuffer::isBufferNotEmpty(lcdBuffer) == 0) timer1Status &= ~(t1sLoopFlag); // if LCD send buffer is empty, signal end of loop

							}
							else timer1Status &= ~(t1sLoopFlag); // otherwise, this is a special (command or reset) nybble, so signal end of loop

						}
						else timer1Status &= ~(t1sLoopFlag); // otherwise, this is just a delay request, so signal end of loop

					}
					while (timer1Status & t1sLoopFlag);

					if (timer1Status & t1sDoOutputTWI) TWI::commitWrite(false); // commit LCD port expander write, if required

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
}

#endif // useTimer1
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
#ifdef useATmega32U4
ISR( INT2_vect )
#endif // useATmega32U4
#ifdef useATmega2560
ISR( INT4_vect )
#endif // useATmega2560
#ifdef useATmega128
ISR( INT0_vect )
#endif // useATmega128
{

	static unsigned long lastInjectorOpenStart;
	unsigned int a;

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

	watchdogInjectorCount = volatileVariables[(unsigned int)(vDetectEngineOffIdx)]; // reset minimum engine speed watchdog timer

}

// injector opening event handler
//
// this measures the end of the fuel injector pulse, and is used to calculate engine speed
//
// if a fuel injector pulse width measurement is in progress, this also performs the measurement and stores raw fuel consumption data
//
// it will either store one of the good existing engine period measurements or an average of both if both are good
//
#ifdef useATmega32U4
ISR( INT3_vect )
#endif // useATmega32U4
#ifdef useATmega2560
ISR( INT5_vect )
#endif // useATmega2560
#ifdef useATmega128
ISR( INT1_vect )
#endif // useATmega128
{

	static unsigned long lastInjectorCloseStart;

	uint8_t b;
	unsigned int a;
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
		thisInjectorPulseLength = findCycleLength(thisInjectorOpenStart, thisInjectorCloseStart) - volatileVariables[(unsigned int)(vInjectorOpenDelayIdx)]; // strip off injector open delay time

		// if this pulse is larger than the maximum good pulse that could happen at the minimum valid engine speed, reject it
		// 1 - pulse could be narrower than vInjectorOpenDelayIdx
		// 2 - pulse could be wider than the maximum allowable pulse width for minimum good engine speed
		if (thisInjectorPulseLength > volatileVariables[(unsigned int)(vInjectorValidMaxWidthIdx)]) dirty &= ~(dGoodInjectorWidth | dGoodInjectorRead);
		else dirty |= (dGoodInjectorWidth);

		if (b) // if we have an engine rotation period measurement
		{

			// calculate good maximum fuel injector open time for injector pulse width sanity check
			goodInjectorPulseLength = engineRotationPeriod - volatileVariables[(unsigned int)(vInjectorTotalDelayIdx)];

			if (thisInjectorPulseLength > goodInjectorPulseLength) dirty &= ~(dGoodInjectorRead); // if measured pulse is larger than largest good pulse, signal that last injector read may be bad
			else dirty |= (dGoodInjectorRead); // signal that last injector read is good

			// if measured engine speed is greater than the specified minimum good engine speed
			if (engineRotationPeriod < volatileVariables[(unsigned int)(vMaximumEnginePeriodIdx)])
			{

				activityFlags &= ~(afEngineOffFlag | afParkFlag | afActivityTimeoutFlag); // signal that engine is running, and vehicle is therefore no longer parked
				awakeFlags |= (aAwakeEngineRunning); // MPGuino is awake due to engine running

			}

#ifdef trackIdleEOCdata
			if (awakeFlags & aAwakeVehicleMoving) tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjCycleIdx, engineRotationPeriod); // add to fuel injector total cycle accumulator
			else tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvInjCycleIdx, engineRotationPeriod); // add to idle fuel injector total cycle accumulator

#else // trackIdleEOCdata
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjCycleIdx, engineRotationPeriod); // add to fuel injector total cycle accumulator

#endif // trackIdleEOCdata
#ifdef useDragRaceFunction
			// add to distance acceleration fuel injector total cycle accumulator
			if (accelerationFlags & accelTestActive)tripArray[(unsigned int)(dragRawDistanceIdx)].add64s(rvInjCycleIdx, engineRotationPeriod);

#endif // useDragRaceFunction
		}

		// if the injector pulse width is valid
		if (dirty & dGoodInjectorWidth)
		{

			awakeFlags |= (aAwakeOnInjector); // signal that MPGuino is awake due to detected injector

#ifdef useChryslerMAPCorrection
			thisInjectorPulseLength *= volatileVariables[(unsigned int)(vInjectorCorrectionIdx)]; // multiply by differential fuel pressure correction factor numerator
			thisInjectorPulseLength >>= 12; // divide by differential fuel pressure correction factor denominator

#endif // useChryslerMAPCorrection
#ifdef trackIdleEOCdata
			if (awakeFlags & aAwakeVehicleMoving) // if vehicle is moving, save injector measurements in active raw trip variable
			{

				tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // add to fuel injector pulse count
				tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjOpenCycleIdx, thisInjectorPulseLength); // add to fuel injector open cycle accumulator

			}
			else // if vehicle is not moving, save injector measurements in active idle/EOC raw trip variable
			{

				tripArray[(unsigned int)(rawEOCidleTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // add to idle fuel injector pulse count
				tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvInjOpenCycleIdx, thisInjectorPulseLength); // add to idle fuel injector open cycle accumulator

			}

#else // trackIdleEOCdata
			tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // add to fuel injector pulse count
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjOpenCycleIdx, thisInjectorPulseLength); // add to fuel injector open cycle accumulator

#endif // trackIdleEOCdata
#ifdef useDragRaceFunction
			if (accelerationFlags & accelTestActive)
			{

				tripArray[(unsigned int)(dragRawDistanceIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // update the distance acceleration injector pulse count
				tripArray[(unsigned int)(dragRawDistanceIdx)].add64s(rvInjOpenCycleIdx, thisInjectorPulseLength); // add to distance acceleration fuel injector open cycle accumulator

			}

#endif // useDragRaceFunction
		}

	}

	dirty |= (dGoodEngineRotationClose);
	lastInjectorCloseStart = thisInjectorCloseStart;

	watchdogInjectorCount = volatileVariables[(unsigned int)(vDetectEngineOffIdx)]; // reset minimum engine speed watchdog timer

}

#ifdef useATmega32U4
ISR( PCINT0_vect )
#endif // useATmega32U4
#ifdef useATmega2560
ISR( PCINT2_vect )
#endif // useATmega2560
#ifdef useATmega128
ISR( PCINT1_vect )
#endif // useATmega128
{

	uint8_t p;
	uint8_t q;

	unsigned int a;
	unsigned long thisTime;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisTime = timer0_overflow_count + (unsigned long)(a);

#ifdef useATmega32U4
	p = PINB; // read current input pin
#endif // useATmega32U4
#ifdef useATmega2560
	p = PINK; // read current input pin
#endif // useATmega2560
#ifdef useATmega128
	p = PINC; // read current input pin
#endif // useATmega128
	q = p ^ lastPINxState; // detect any changes from the last time this ISR is called

#ifdef useATmega32U4
	if (q & (1 << PINB7)) // if a VSS pulse is received
#endif // useATmega32U4
#ifdef useATmega2560
	if (q & (1 << PINK0)) // if a VSS pulse is received
#endif // useATmega2560
#ifdef useATmega128
	if (q & (1 << PINC0)) // if a VSS pulse is received
#endif // useATmega128
	{

		if (VSSpause) VSScount = VSSpause; // if there is a VSS debounce count defined, set VSS debounce count and let system timer handle the debouncing
		else updateVSS(thisTime); // otherwise, go process VSS pulse

	}

#ifdef useLegacyButtons
	if (q & buttonMask) buttonCount = buttonDelayTick; // if a button change was detected, set button press debounce count, and let system timer handle the debouncing

#endif // useLegacyButtons
	lastPINxState = p; // remember the current input pin state for the next time this ISR gets called

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

		if (cycleLength < volatileVariables[(unsigned int)(vMaximumVSSperiodIdx)]) // if VSS period is less than that for minimum good vehicle speed
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

			tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // add to VSS pulse count
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvVSScycleIdx, cycleLength); // add to VSS cycle accumulator

		}
		else // if the engine is not running, vehicle is in EOC mode
		{

			tripArray[(unsigned int)(rawEOCidleTripIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // add to EOC VSS pulse count
			tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvVSScycleIdx, cycleLength); // add to EOC VSS cycle accumulator

		}

#else // trackIdleEOCdata
		tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // add to VSS pulse count
		tripArray[(unsigned int)(rawTripIdx)].add64s(rvVSScycleIdx, cycleLength); // add to VSS cycle accumulator

#endif // trackIdleEOCdata
#ifdef useCoastDownCalculator
		if (coastdownFlags & (cdtActive | cdTakeSample)) // if coastdown test is active
		{

			coastdownFlags &= ~(cdTakeSample);
			coastdownFlags |= cdSampleTaken;
			volatileVariables[(unsigned int)(vCoastdownMeasurementIdx + coastdownState)] = cycleLength;

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

			if (volatileVariables[(unsigned int)(vDragInstantSpeedIdx)] > cycleLength) volatileVariables[(unsigned int)(vDragInstantSpeedIdx)] = cycleLength; // if this vehicle speed is larger than previous vehicle speed

			tripArray[(unsigned int)(dragRawDistanceIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // update the accel test distance measurement VSS pulse count
			tripArray[(unsigned int)(dragRawDistanceIdx)].add64s(rvVSScycleIdx, cycleLength); // update the accel test distance measurement VSS cycle accumulator

			if (accelerationFlags & accelTestDistance)
			{

				if (volatileVariables[(unsigned int)(vAccelDistanceValueIdx)]) volatileVariables[(unsigned int)(vAccelDistanceValueIdx)]--; // count down drag distance setpoint in VSS pulses
				else
				{

					accelerationFlags &= ~accelTestDistance; // otherwise, mark drag function distance measurement as complete
					timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

				}

			}

			if ((accelerationFlags & accelTestHalfSpeed) && (cycleLength < volatileVariables[(unsigned int)(vAccelHalfPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripArray[(unsigned int)(dragRawHalfSpeedIdx)].transfer(tripArray[(unsigned int)(dragRawDistanceIdx)]);
				accelerationFlags &= ~accelTestHalfSpeed; // mark drag function speed measurement as complete
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

			if ((accelerationFlags & accelTestFullSpeed) && (cycleLength < volatileVariables[(unsigned int)(vAccelFullPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripArray[(unsigned int)(dragRawFullSpeedIdx)].transfer(tripArray[(unsigned int)(dragRawDistanceIdx)]);
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
	watchdogVSSCount = volatileVariables[(unsigned int)(vDetectVehicleStopIdx)]; // reset minimum engine speed watchdog timer
	lastVSStime = thisVSStime;

}

unsigned long findCycleLength(unsigned long lastCycle, unsigned long thisCycle)
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

#ifdef useAnalogInterrupt
#ifdef useAnalogRead
#define nextAllowedValue 0
#ifdef useATmega32U4
const uint8_t analog0Idx =				nextAllowedValue;
const uint8_t analog1Idx =				analog0Idx + 1;
const uint8_t analog2Idx =				analog1Idx + 1;
#define nextAllowedValue analog2Idx + 1

#ifdef useAnalogButtons
const uint8_t analogButtonChannelIdx =	analog0Idx;
#endif // useAnalogButtons
#ifdef useChryslerMAPCorrection
const uint8_t analogMAPchannelIdx =		analog1Idx;
const uint8_t analogBaroChannelIdx =	analog2Idx;
#endif // useChryslerMAPCorrection
#endif useATmega32U4
#ifdef useATmega2560
const uint8_t analog0Idx =				nextAllowedValue;
const uint8_t analog1Idx =				analog0Idx + 1;
#define nextAllowedValue analog1Idx + 1
#ifdef useAnalogButtons
const uint8_t analog2Idx =				nextAllowedValue;
#define nextAllowedValue analog2Idx + 1
#ifndef useTWIsupport
const uint8_t analog3Idx =				nextAllowedValue;
const uint8_t analog4Idx =				analog3Idx + 1;
#define nextAllowedValue analog4Idx + 1
#endif // useTWIsupport
#endif // useAnalogButtons

#ifdef useAnalogButtons
const uint8_t analogButtonChannelIdx =	analog2Idx;
#endif // useAnalogButtons
#ifdef useChryslerMAPCorrection
const uint8_t analogMAPchannelIdx =		analog0Idx;
const uint8_t analogBaroChannelIdx =	analog1Idx;
#endif // useChryslerMAPCorrection
#endif useATmega2560
#ifdef useATmega128
const uint8_t analog0Idx =				nextAllowedValue;
const uint8_t analog1Idx =				analog0Idx + 1;
#define nextAllowedValue analog1Idx + 1
#ifdef useAnalogButtons
const uint8_t analog2Idx =				nextAllowedValue;
#define nextAllowedValue analog2Idx + 1
#ifndef useTWIsupport
const uint8_t analog3Idx =				nextAllowedValue;
const uint8_t analog4Idx =				analog3Idx + 1;
#define nextAllowedValue analog4Idx + 1
#endif // useTWIsupport
#endif // useAnalogButtons

#ifdef useAnalogButtons
const uint8_t analogButtonChannelIdx =	analog2Idx;
#endif // useAnalogButtons
#ifdef useChryslerMAPCorrection
const uint8_t analogMAPchannelIdx =		analog0Idx;
const uint8_t analogBaroChannelIdx =	analog1Idx;
#endif // useChryslerMAPCorrection
#endif // useATmega128
const uint8_t dfMaxValAnalogCount =		nextAllowedValue;
#endif // useAnalogRead

volatile uint8_t analogValueIdx;
volatile unsigned int analogValue[(unsigned int)(dfMaxValAnalogCount)];

const uint8_t analogChannelValue[(unsigned int)(dfMaxValAnalogCount)] PROGMEM = { // points to the next channel to be read
#ifdef useATmega32U4
	 (1 << REFS0)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// analog channel 1 PF7 A0 analog button input
	,(1 << REFS0)|	(1 << MUX2)|	(1 << MUX1)					// analog channel 2 PF6 A1 MAP sensor
	,(1 << REFS0)|	(1 << MUX2)|					(1 << MUX0)	// analog channel 3 PF5 A2 baro sensor / car voltage
#endif // useATmega32U4
#ifdef useATmega2560
	(1 << REFS0)|									(1 << MUX0)	// analog channel 1 PF1 A1 MAP sensor
	,(1 << REFS0)|					(1 << MUX1)					// analog channel 2 PF2 A2 baro sensor / car voltage
#ifdef useAnalogButtons
	,(1 << REFS0)|					(1 << MUX1)|	(1 << MUX0)	// analog channel 3 PF3 A3 analog button input
#ifndef useTWIsupport
	,(1 << REFS0)|	(1 << MUX2)									// analog channel 4 PF4 A4
	,(1 << REFS0)|	(1 << MUX2)|					(1 << MUX0)	// analog channel 5 PF5 A5
#endif // useTWIsupport
#endif // useAnalogButtons
#endif // useATmega2560
#ifdef useATmega128
	(1 << REFS0)|									(1 << MUX0)	// analog channel 1 PC1 A1 MAP sensor
	,(1 << REFS0)|					(1 << MUX1)					// analog channel 2 PC2 A2 baro sensor / car voltage
#ifdef useAnalogButtons
	,(1 << REFS0)|					(1 << MUX1)|	(1 << MUX0)	// analog channel 3 PC3 A3 analog button input
#ifndef useTWIsupport
	,(1 << REFS0)|	(1 << MUX2)									// analog channel 4 PC4 A4
	,(1 << REFS0)|	(1 << MUX2)|					(1 << MUX0)	// analog channel 5 PC5 A5
#endif // useTWIsupport
#endif // useAnalogButtons
#endif // useATmega128
};

// this interrupt is called upon completion of an analog to digital conversion
//
// this interrupt is normally initially called by timer0, and will continue as long as there are analog channel requests present
//
// based on a 20 MHz clock, and a prescaler of 128, a single A/D conversion should take about 179 microseconds
ISR( ADC_vect )
{

	unsigned int rawRead;
	union union_16 * rawValue = (union union_16 *)(&rawRead);
	uint8_t analogChannelIdx;
	uint8_t analogChannelMask;

	rawValue->u8[0] = ADCL; // (locks ADC sample result register from AtMega hardware)
	rawValue->u8[1] = ADCH; // (releases ADC sample result register to AtMega hardware)

	analogChannelIdx = 0;
	analogChannelMask = 0b000000001;

	while (((analogCommand & analogChannelMask) == 0) && (analogChannelIdx < dfMaxValAnalogCount))
	{

		analogChannelIdx++; // increment the analog channel index
		analogChannelMask <<= 1; // shift mask left one bit

	}

	if (analogChannelIdx >= dfMaxValAnalogCount) analogCommand &= ~(acSampleChannelActive); // an invalid channel was requested, so ignore it

	if (analogCommand & acSampleGround)
	{

		analogCommand &= ~(acSampleGround); // signal that internal ground was read
		analogStatus &= ~(asReadChannel);

		if (analogCommand & acSampleChannelActive) ADMUX = pgm_read_byte(&analogChannelValue[(unsigned int)(analogChannelIdx)]); // select next analog channel to read

	}
	else
	{

		analogCommand &= ~(analogChannelMask); // clear the relevant bit in analog command status
		analogStatus |= (asReadChannel); // signal to main program that an analog channel was read in
		analogValue[(unsigned int)(analogChannelIdx)] = rawRead; // save the value just read in
		analogValueIdx = analogChannelIdx; // save the analog index value

		if (analogCommand & acSampleChannelActive) // if there are other channels, set up for another read of internal ground
		{

			analogCommand |= (acSampleGround); // signal that next read is for internal ground
#ifdef useATmega32U4
			ADMUX = (1 << REFS0) | (1 << MUX4) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif // useATmega32U4
#ifdef useATmega2560
			ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif // useATmega2560
#ifdef useATmega128
			ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif // useATmega128

		}

	}

	if (analogCommand & acSampleChannelActive) ADCSRA |= (1 << ADSC); // start the next conversion

}

#endif // useAnalogInterrupt
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
void TWI::init(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useATmega32U4
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#endif // useATmega32U4
#ifdef useATmega2560
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#endif // useATmega2560
#ifdef useATmega128
	PRR &= ~(1 << PRTWI); // turn on TWI module
	PORTC |= ((1 << PORTC5) | (1 << PORTC4)); // enable port C TWI pin pullup resistors
#endif // useATmega128
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // set TWI prescaler to 1
	TWBR = (uint8_t)(((unsigned int)(systemProcessorSpeed * 1000 / twiFrequency) - 16) / 2); // set TWI frequency
	TWCR &= ~((1 << TWINT) | (1 << TWSTA) | (1 << TWSTO)); // clear TWI interrupt flag, start condition, stop condition
	TWCR |= ((1 << TWIE) | (1 << TWEA) | (1 << TWEN)); // enable TWI module, acks, and interrupt

	twiStatusFlags = twiSendStop; // initialize TWI state to ready, default send stop, and no repeated start in progress

	onSlaveTransmit = 0;
	onSlaveReceive = 0;

	SREG = oldSREG; // restore interrupt flag status

}

void TWI::shutdown(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	while (twiStatusFlags & twiTxRxStatusMain); // wait for all TWI transactions to complete

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	TWCR &= ~((1 << TWEN) | (1 << TWIE)); // disable TWI module and interrupt
#ifdef useATmega32U4
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#endif // useATmega32U4
#ifdef useATmega2560
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#endif // useATmega2560
#ifdef useATmega128
	PORTC &= ~((1 << PORTC5) | (1 << PORTC4)); // disable port C TWI pin pullup resistors
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D)); // disable digital input on port C TWI pins
	PRR |= (1 << PRTWI); // turn off TWI module
#endif // useATmega128

	twiStatusFlags = 0;

	SREG = oldSREG; // restore interrupt flag status

}

void TWI::setAddress(uint8_t address)
{

	TWAR = address << 1; // set twi slave address (skip over TWGCE bit)

}

uint8_t TWI::transmit(const uint8_t * data, uint8_t length) // this can be in either main program or interrupt context
{

	if(twiSlaveTxBufferSize < length) return 1; // ensure data will fit into buffer

	if((twiStatusFlags & twiSlaveTransmitter) == 0) return 2; // ensure we are currently a slave transmitter

	twiSlaveTxBufferLen = length; // set length and copy data into tx buffer

	for(uint8_t x = 0; x < length; ++x) twiSlaveTxBufferData[(unsigned int)(x)] = data[(unsigned int)(x)];

	return 0;

}

uint8_t TWI::readDevice(uint8_t address, uint8_t * data, uint8_t length, uint8_t sendStop) // this can be in either main program or interrupt context
{

	if(twiMasterBufferSize < length) return 1; // ensure data will fit into buffer

	initMasterDevice(address, TWI_READ, sendStop, TWI_MAIN_PROGRAM); // initialize master receiver, wait for TWI to become ready

	twiMasterBufferLen = length - 1;  // This is not intuitive, read on...
	// On receive, the previously configured ACK/NACK setting is transmitted in
	// response to the received byte before the interrupt is signalled.
	// Therefor we must actually set NACK when the _next_ to last byte is
	// received, causing that NACK to be sent in response to receiving the last
	// expected byte of data.

	commitMasterDevice(true);

	for(uint8_t x = 0; x < twiMasterBufferIdx; ++x) data[(unsigned int)(x)] = twiMasterBufferData[(unsigned int)(x)]; // copy twi buffer to data

	return twiMasterBufferIdx;

}

uint8_t TWI::writeDevice(uint8_t address, uint8_t * data, uint8_t length, uint8_t waitFlag, uint8_t sendStop)
{

	if(twiMasterBufferSize < length) return 1; // ensure data will fit into buffer

	initMasterDevice(address, TWI_WRITE, sendStop, TWI_MAIN_PROGRAM); // initialize master transmitter, wait for TWI to become ready

	twiMasterBufferLen = length;

	for(uint8_t x = 0; x < length; ++x) twiMasterBufferData[(unsigned int)(x)] = data[(unsigned int)(x)]; // copy data to twi buffer

	return commitMasterDevice(waitFlag);

}

void TWI::initWrite(uint8_t address, uint8_t sendStop, uint8_t interruptFlag) // interrupt-friendly master transmitter packet send
{

	initMasterDevice(address, TWI_WRITE, sendStop, interruptFlag); // initialize master transmitter
	twiMasterBufferLen = 0; // initialize buffer length

}

void TWI::initMasterDevice(uint8_t address, uint8_t writeFlag, uint8_t sendStop, uint8_t interruptFlag) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status

	switch (interruptFlag)
	{

		case TWI_MAIN_PROGRAM:
			while (twiStatusFlags & twiTxRxStatusMain) idleProcess(); // wait for TWI to become available

			cli(); // disable interrupts

		case TWI_INTERRUPT:
		default:
			twiSlaveAddress = (address << 1); // form initial slave address

			if (writeFlag)
			{

				twiStatusFlags |= twiMasterTransmitter; // shift to master transmitter state
				twiSlaveAddress |= TW_WRITE; // form slave address + write

			}
			else
			{

				twiStatusFlags |= twiMasterReceiver; // shift to master receiver state
				twiSlaveAddress |= TW_READ; // form slave address + read

			}

			if (sendStop) twiStatusFlags |= twiSendStop; // if stop was requested as end of transmission, remember it
			else twiStatusFlags &= ~(twiSendStop); // otherwise, remember to do a repeated start

			twiStatusFlags &= ~(twiErrorFlag); // clear error condition
			twiErrorCode = 0x00; // reset error state

			twiMasterBufferIdx = 0; // initialize buffer index
			break;

	}

	SREG = oldSREG; // restore interrupt flag status

}

uint8_t TWI::writeByte(uint8_t data) // this can be in either main program or interrupt context
{

	if(twiMasterBufferLen < twiMasterBufferSize)
	{

		twiMasterBufferData[(unsigned int)(twiMasterBufferLen++)] = data;
		return 0;

	}
	else
	{

		EEPROM::changeBitFlags(twiStatusFlags, twiTxRxStatus, 0); // free up TWI for main program use
		return 1; // signal buffer overflow

	}

}

void TWI::commitWrite(void) // this is called from the main program context
{

	commitWrite(false);

}

void TWI::commitWrite(uint8_t waitFlag) // this can be in either main program or interrupt context
{

	if ((twiStatusFlags & twiTxRxStatus) == twiMasterTransmitter) // if in master transmitter state
	{

		if (twiMasterBufferLen) commitMasterDevice(waitFlag); // if there is data in the master transmitter butter, go initiate TWI master transmitter job
		else EEPROM::changeBitFlags(twiStatusFlags, twiTxRxStatus, 0); // otherwise, free up TWI for main program use

	}

}

uint8_t TWI::commitMasterDevice(uint8_t waitFlag) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	// if we're in a repeated start, then we've already sent the START
	// in the ISR. Don't do it again.
	//
	// if we're in the repeated start state, then we've already sent the start,
	// (@@@ we hope), and the TWI statemachine is just waiting for the address byte.
	// We need to remove ourselves from the repeated start state before we enable interrupts,
	// since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
	// up. Also, don't enable the START interrupt. There may be one pending from the
	// repeated start that we sent outselves, and that would really confuse things.

	oldSREG = SREG; // save interrupt flag status

	cli(); // disable interrupts

	if (twiStatusFlags & twiRepeatStart) // if in repeated start state
	{

		twiStatusFlags &= ~(twiRepeatStart); // clear repeated start state
		TWDR = twiSlaveAddress; // set data address to slave address
		TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // enable INT but do not send start condition

	}
	else TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTA)); // enable INT and send start condition

	SREG = oldSREG; // restore interrupt flag status

	if (waitFlag)
	{

		while (twiStatusFlags & twiTxRxStatus) idleProcess(); // wait for master operation to complete

		switch (twiErrorCode)
		{

			case 0x00:
				return 0; // success

			case TW_MT_SLA_NACK:
				return 2; // error: address send, nack received

			case TW_MT_DATA_NACK:
				return 3; // error: data send, nack received

			default:
				return 4; // other twi error

		}

	}
	else return 0; // no point returning an error code if we're not waiting for the master transmitter to finish

}

void TWI::reply(uint8_t ack) // called only in the context of the TWI interrupt
{

	if (ack) TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA)); // send ACK condition
	else TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWINT)); // send NEGATIVE ACK condition

}

void TWI::stopOrRepeatStart(void) // called only in the context of the TWI interrupt
{

	if (twiStatusFlags & twiSendStop) TWI::hardStop();
	else
	{

		twiStatusFlags |= twiRepeatStart;	// we're gonna send the START
		// don't enable the interrupt. We'll generate the start, but we
		// avoid handling the interrupt until we're in the next transaction,
		// at the point where we would normally issue the start.
		TWCR = ((1 << TWINT) | (1 << TWSTA)| (1 << TWEN));
		twiStatusFlags &= ~(twiTxRxStatus); // set TWI state to ready

	}

}

void TWI::hardStop(void) // called only in the context of the TWI interrupt
{

	TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT) | (1 << TWSTO)); // send stop condition

	while (TWCR & (1 << TWSTO)); // wait for stop condition to be executed on bus

	twiStatusFlags &= ~(twiTxRxStatus); // set TWI state to ready

}

void TWI::releaseBus(void) // called only in the context of the TWI interrupt
{

	TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT)); // release two-wire bus
	twiStatusFlags &= ~(twiTxRxStatus); // set TWI state to ready

}

#endif // useTWIsupport
#ifdef useAdafruitRGBLCDshield
// initialize Adafruit RGB LGB shield to a known initial state
void adafruitRGBLCDsupport::init(void)
{

	uint16_t MCP23017registers;

	adafruitRGBLCDflags = 0; // initially set all control register bits to 0

	// we might have inadvertently entered into MCP23017 bank mode - if we didn't, we'll just end up disabling all of bank B's interrupt enable bits
	writeRegister8Bit(TWI_MAIN_PROGRAM, MCP23017_B1_IOCON, adafruitRGBLCDflags); // write initialization value to IO control register

	// if we did happen to enter bank mode prior to initialization, we are now known to not be in bank mode
	writeRegister8Bit(TWI_MAIN_PROGRAM, MCP23017_B0_IOCON, adafruitRGBLCDflags); // write initialization value to IO control register

	setTransferMode(TWI_MAIN_PROGRAM, adaTWItoggleMode);

	MCP23017registers = 0;

	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_GPINTENx, (union_16 *)(&MCP23017registers)); // write out interrupt enable register (the Adafruit RGB LCD shield wasn't wired to support this)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_IODIRx, (union_16 *)(&MCP23017registers)); // write out port direction (no inputs)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (input bits same as GPIO bits)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_GPINTENx, (union_16 *)(&MCP23017registers)); // write out GPIO bit interrupt assignment register (no GPIO bits associated with interrupts)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_DEFVALx, (union_16 *)(&MCP23017registers)); // write out interrupt default value register (all default bits 0)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_INTCONx, (union_16 *)(&MCP23017registers)); // write out interrupt control register (all interrupts trigger on GPIO input pin change)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (no pull-up resistors)
	writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_GPIOx, (union_16 *)(&MCP23017registers)); // write out GPIO state (all 0) - also writes out OLAT register

}

void adafruitRGBLCDsupport::writeRegister16Bit(uint8_t interruptFlag, uint8_t registerAddress, union union_16 * registerValue)
{

	writeRegister16Bit(interruptFlag, registerAddress, registerValue->u8[0], registerValue->u8[1]);

}

void adafruitRGBLCDsupport::writeRegister16Bit(uint8_t interruptFlag, uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte)
{

	if (adafruitRGBLCDflags & afRGBLCDbankMode) setTransferMode(interruptFlag, adaTWItoggleMode); // if address mode isn't set to access 16-bit registers, configure as such

	TWI::initWrite(adafruitRGBLCDaddress, TWI_STOP, interruptFlag); // open TWI as master transmitter
	TWI::writeByte(registerAddress | MCP23017_B0_PORTA); // specify bank A register address
	TWI::writeByte(portAbyte); // write desired value to register bank A byte
	TWI::writeByte(portBbyte); // write desired value to register bank B byte
	TWI::commitWrite(true); // go write out register contents

}

void adafruitRGBLCDsupport::writeRegister8Bit(uint8_t interruptFlag, uint8_t registerAddress, uint8_t portByte)
{

	TWI::initWrite(adafruitRGBLCDaddress, TWI_STOP, interruptFlag); // open TWI as master transmitter
	TWI::writeByte(registerAddress); // specify register address
	TWI::writeByte(portByte); // write desired value to register address
	TWI::commitWrite(true); // go write out register contents

}

void adafruitRGBLCDsupport::setTransferMode(uint8_t interruptFlag, uint8_t mode)
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

		writeRegister8Bit(interruptFlag, address, newFlags); // write new value to IO control register

		adafruitRGBLCDflags = newFlags; // changes take effect just as soon as IOCON is written

	}

}

#endif // useAdafruitRGBLCDshield
#ifdef useLCDoutput
static void LCD::init(void)
{

#ifdef use4BitLCD
	uint8_t oldSREG;

#endif // use4BitLCD
	devLCD.charOut = LCD::writeData;
	devLCD.gotoXY = LCD::gotoXY;
	devLCD.controlFlags |= odvFlagSupportsDelay;
	devLCD.xMax = 16;

#ifdef useParallaxSerialLCDmodule
	devLCDserial.controlFlags |= odvFlagSupportsDelay;
	devLCDserial.controlFlags &= ~(odvFlagCRLF);
	devLCDserial.xMax = 16;

	delay0(delay0005msTick); // wait for 5 ms
	LCDserialPort::charOut(12); // clear the LCD screen
#ifdef useBufferedLCD
	ringBuffer::flush(LCDserialBuffer); // clear the LCD buffer to force the LCD screen to clear
#endif // useBufferedLCD
	delay0(delay0005msTick); // wait for another 5 ms
	LCDserialPort::charOut(22); // turn on display, no cursor, no blink

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
#ifdef useBufferedLCD
	ringBuffer::init(lcdBuffer, LCDdata);

#endif // useBufferedLCD
#ifdef useTWILCD
	disableIntSample(); // disable TWI button sampling

	portLCD = 0; // reset LCD port byte
#ifdef useAdafruitRGBLCDshield
	portSwitches = 0; // reset button port byte (contains two of the three LCD backlight LED bits)

	adafruitRGBLCDsupport::setTransferMode(TWI_MAIN_PROGRAM, adaTWIbyteMode);
#endif // useAdafruitRGBLCDshield

#else // useTWILCD
#ifdef useATmega32U4
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
#endif // useATmega32U4
#ifdef useATmega2560
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRA |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);
	DDRB |= (lcdBrightness | lcdContrast);

#endif // useATmega2560
#ifdef useATmega128
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBrightness);
	DDRD |= (lcdBit0 | lcdContrast | lcdEnable | lcdRegisterSelect);

#endif // useATmega128
#endif // useTWILCD
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useATmega32U4
	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#endif // useATmega32U4
#ifdef useATmega2560
	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#endif // useATmega2560
#ifdef useATmega128
	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#endif // useATmega128

	lcdDelayCount = 0;
	timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

	SREG = oldSREG; // restore interrupt flag status

	// perform reset by instruction on LCD interface
	// the below 5 lines actually take up less space than using a PROGMEM table / loop to write these values
	writeNybble(lcdNullValue, lcdDelay0015ms); // wait for more than 15 msec
	writeNybble(lcdFunctionSet | lcdFSdataLength, lcdCommandByte | lcdSendNybble | lcdDelay4100us); // send (B0011) to DB7-4, then wait for more than 4.1 ms
	writeNybble(lcdFunctionSet | lcdFSdataLength, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(lcdFunctionSet | lcdFSdataLength, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us

	writeNybble(lcdFunctionSet, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0010) to DB7-4 for 4 bit mode, then wait for more than 40 us

#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // useBufferedLCD
	// ready to use normal writeCommand() function now!
	writeCommand(lcdFunctionSet | lcdFSnumberOfLines); // 4-bit interface, 2 display lines, 5x8 font
	writeCommand(lcdDisplayControl | lcdDCdisplayShow); // display control:
	writeCommand(lcdClearDisplay); // clear display, set cursor position to zero

	LCDgotoXYaddress = lcdSetDDRAMaddress;

#ifdef useTWILCD
	enableIntSample(); // enable TWI button sampling

#endif // useTWILCD
#endif // use4BitLCD
	setBrightness(brightnessIdx);
#ifdef useLegacyLCD

	setContrast(EEPROM::readVal(pContrastIdx));
#endif // useLegacyLCD

}

static void LCD::shutdown(void)
{

#ifdef use4BitLCD
	uint8_t oldSREG;

#endif // use4BitLCD
	setBrightness(0); // turn off LCD brightness
#ifdef useLegacyLCD
	setContrast(255); // turn off LCD contrast
#endif // useLegacyLCD

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::charOut(21); // turn off LCD display
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

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useATmega32U4
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // useATmega32U4
#ifdef useATmega2560
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // useATmega2560
#ifdef useATmega128
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // useATmega128

	SREG = oldSREG; // restore interrupt flag status

#ifndef useTWILCD
#ifdef useATmega32U4
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
#endif // useATmega32U4
#ifdef useATmega2560
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

#endif // useATmega2560
#ifdef useATmega128
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

#endif // useATmega128
#endif // useTWILCD
#endif // use4BitLCD
}

static void LCD::gotoXY(uint8_t x, uint8_t y) // x = 0..16, y = 0..1
{

#ifdef useParallaxSerialLCDmodule
	uint8_t dr = 128 + 20 * y + x;

	LCDserialPort::charOut(dr);

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
	LCDgotoXYaddress = lcdSetDDRAMaddress + 0x40 * y + x;

	writeCommand(LCDgotoXYaddress);

#endif // use4BitLCD
}

static void LCD::loadCGRAMcharacter(uint8_t chr, char * chrData)
{

	uint8_t b = chr & 0x07;

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::charOut(248 + b);

	for (uint8_t x = 0; x < 8; x++) LCDserialPort::charOut(*chrData++); //write the character data to the character generator ram

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
	writeCommand(lcdEntryModeSet | lcdEMSincrement); // entry mode set: increment automatically, no display shift
	writeCommand(lcdSetCGRAMaddress + (b << 3)); // set CGRAM

	for (uint8_t x = 0; x < 8; x++) writeData(*chrData++); //write the character data to the character generator ram

	writeCommand(LCDgotoXYaddress); // set DDRAM to whatever the screen position was

#endif // use4BitLCD
}

static void LCD::setBrightness(uint8_t idx)
{

#ifdef useParallaxSerialLCDmodule
	if (idx) LCDserialPort::charOut(17); // turn on LCD backlight
	else LCDserialPort::charOut(18); // turn off LCD backlight
#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDshield
	if (idx) setRGBcolor(EEPROM::readVal(pLCDcolorIdx)); // turn on LCD backlight
	else setRGBcolor(0); // turn off LCD backlight
#else // useAdafruitRGBLCDshield
	if (idx) portLCD |= lcdBrightness; // turn on LCD backlight
	else portLCD &= ~(lcdBrightness); // turn off LCD backlight
#endif // useAdafruitRGBLCDshield
#else // useTWILCD
#ifdef useATmega32U4
	OCR1B = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#endif // useATmega32U4
#ifdef useATmega2560
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#endif // useATmega2560
#ifdef useATmega128
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#endif // useATmega128
#endif // useTWILCD
#endif // use4BitLCD

}

#ifdef useLegacyLCD
static void LCD::setContrast(uint8_t idx)
{

#ifdef useATmega32U4
	OCR1A = idx;
#endif // useATmega32U4
#ifdef useATmega2560
	OCR0A = idx;
#endif // useATmega2560
#ifdef useATmega128
	OCR0A = idx;
#endif // useATmega128

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

	disableIntSample(); // disable TWI button sampling

	adafruitRGBLCDsupport::writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_OLATx, portSwitches, portLCD); // write out 16-bit register (which sets address mode to toggle)

	adafruitRGBLCDsupport::setTransferMode(TWI_MAIN_PROGRAM, adaTWIbyteMode); // set address mode to byte mode

	enableIntSample(); // enable TWI button sampling

}

#endif // useAdafruitRGBLCDshield
static void LCD::writeData(uint8_t value)
{

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::charOut(value);
#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
	writeByte(value, lcdDataByte, lcdDelay0040us);
#endif // use4BitLCD

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

		disableIntSample(); // disable TWI button sampling
		TWI::initWrite(lcdAddress, TWI_STOP, TWI_MAIN_PROGRAM); // open TWI as master transmitter
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

		TWI::commitWrite(); // commit LCD port expander write
		enableIntSample(); // enable TWI button sampling

	}
#endif // useTWILCD
#endif // useBufferedLCD

	EEPROM::changeBitFlags(timer1Command, 0, t1cDelayLCD); // enable LCD delay

}

//const unsigned long t1CyclesPerSecond = (unsigned long)(systemProcessorSpeed * 1000000ul); // (systemProcessorSpeed * 1000000 / (timer 1 prescaler))

//const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // initial LCD delay for 4-bit initialization
//const unsigned int delayLCD04100usTick = (unsigned int)(ceil)((double)(4100ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // secondary LCD delay for 4-bit initialization
//const unsigned int delayLCD00100usTick = (unsigned int)(ceil)((double)(100ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // final LCD delay for 4-bit initialization
//const unsigned int delayLCD00040usTick = (unsigned int)(ceil)((double)(40ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // normal LCD character transmission delay
const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
const unsigned int delayLCD04100usTick = (unsigned int)(ceil)((double)(4100ul * systemProcessorSpeed) / (double)(510ul)) - 1; // secondary LCD delay for 4-bit initialization
const unsigned int delayLCD00100usTick = (unsigned int)(ceil)((double)(100ul * systemProcessorSpeed) / (double)(510ul)) - 1; // final LCD delay for 4-bit initialization
const unsigned int delayLCD00040usTick = (unsigned int)(ceil)((double)(40ul * systemProcessorSpeed) / (double)(510ul)); // normal LCD character transmission delay

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
#ifdef useATmega32U4
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
#endif // useATmega32U4
#ifdef useATmega2560
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
#endif // useATmega2560
#ifdef useATmega128
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
#endif // useATmega128
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

#ifdef useTWILCD
static void LCD::disableIntSample(void)
{

	EEPROM::changeBitFlags(timer0Command, t0cEnableTWIsample, 0); // disable TWI button sampling as it interferes with TWI LCD output
	while (activityFlags & afTWIsampleActive) idleProcess(); // wait for any in-progress TWI button samples to finish

}

static void LCD::enableIntSample(void)
{

	EEPROM::changeBitFlags(timer0Command, 0, t0cEnableTWIsample); // re-enable TWI button sampling

}

#endif // useTWILCD
#endif // use4BitLCD
#endif // useLCDoutput
#ifdef useBuffering
const uint8_t bufferIsFull =	0b10000000;
const uint8_t bufferIsEmpty =	0b01000000;

void ringBuffer::init(ringBufferVariable &bfr, volatile uint8_t * storage)
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

uint8_t ringBuffer::isBufferNotEmpty(ringBufferVariable &bfr)
{

	return ((bfr.status & bufferIsEmpty) == 0);

}

void ringBuffer::push(ringBufferVariable &bfr, uint8_t value)
{

	uint8_t oldSREG;

	while (bfr.status & bufferIsFull) idleProcess(); // wait for calling routine's buffer to become not full

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	bfr.data[(unsigned int)(bfr.start++)] = value; // save a buffered character

	if (bfr.status & bufferIsEmpty) bfr.status &= ~(bufferIsEmpty); // mark buffer as no longer empty
	if (bfr.start == bfr.size) bfr.start = 0; // handle wrap-around
	if (bfr.start == bfr.end) bfr.status |= (bufferIsFull); // test if buffer is full

	SREG = oldSREG; // restore interrupt flag status

}

uint8_t ringBuffer::pull(ringBufferVariable &bfr)
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

void ringBuffer::flush(ringBufferVariable &bfr)
{

	while ((bfr.status & bufferIsEmpty) == 0) idleProcess(); // wait for calling routine's buffer to become empty

}

#endif // useBuffering
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
	,pBaroSensorRangeIdx
	,pBaroSensorOffsetIdx
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
	,idxNumerPressure						// pBaroSensorRangeIdx
	,idxNumerPressure						// pBaroSensorOffsetIdx
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	,idxNumerDistance						// pBarLowSpeedCutoffIdx
	,idxNumerDistance						// pBarSpeedQuantumIdx
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,idxDenomVolume							// pCostPerQuantity
#endif // useFuelCost
};

const uint8_t prgmEngineSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,		// load injector pulse count into register 2
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// set up for conversion of denominator injector cycle count to time in seconds
	instrMul2by1,
	instrLdRegByte, 0x01, 60,							// load seconds per minute into register 1
	instrMul2by1,										// set up for conversion of denominator injector time in seconds to time in minutes
	instrLdRegConst, 0x01, idxDecimalPoint,				// perform output decimal formatting
	instrMul2by1,
	instrLdRegEEPROM, 0x01, pCrankRevPerInjIdx,			// multiply by the number of crank revolutions per injector fire event
	instrMul2by1,
	instrLdRegTripVarIndexed, 0x01, rvInjCycleIdx,		// divide by the injector pulse time
	instrDiv2by1,										// store result in register 2
	instrDone											// exit to caller
};

const uint8_t prgmMotionTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load cycles per second value into register 1
	instrDiv2by1,										// calculate VSS time and store in register 2
	instrDone											// exit to caller
};

const uint8_t prgmEngineRunTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector pulse cycle value into register 2
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load cycles per second value into register 1
	instrDiv2by1,										// calculate engine run time and store in register 1
	instrDone											// exit to caller
};

const uint8_t prgmRangeTime[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

const uint8_t prgmReserveTime[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrSubYfromX, 0x21,								// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelTime						// go format it
};

const uint8_t prgmBingoTime[] PROGMEM = {
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

const uint8_t prgmTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

const uint8_t prgmReserveTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

const uint8_t prgmBingoTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

const uint8_t prgmCalculateFuelTime[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 20,								// if inputted fuel quantity is zero, exit to caller

	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,	// shift number upward to retain numeric precision
	instrMul2by1,
	instrLdRegTripVarIndexed, 0x01, rvInjOpenCycleIdx,	// divide by open injector time in cycles
	instrDiv2by1,
	instrLdRegTripVarIndexed, 0x01, rvInjCycleIdx,		// multiply by injector open count
	instrMul2by1,
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// convert output term from cycles remaining to seconds remaining
	instrDiv2by1,
	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,	// shift number downward - high numeric precision no longer needed
	instrDiv2by1,										// go finish conversion

	instrDone											// exit to caller
};

const uint8_t prgmDistance[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS cycle value into register 2
	instrLdRegConst, 0x01, idxDecimalPoint,				// perform output decimal formatting
	instrMul2by1,										// store result in register 2
	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// load pulse per distance value into register 1
	instrDiv2by1,										// calculate distance travelled and store in register 2
	instrDone											// exit to caller
};

const uint8_t prgmSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrTestReg, 0x02,									// test VSS cycle value
	instrBranchIfZero, 24,								// if zero, then speed is also zero

	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// set up to convert pulses per unit distance
	instrMul2by1,
	instrLdReg, 0x32,									// save denominator term for later

	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS pulse count
	instrLdRegConst, 0x01, idxDecimalPoint,				// adjust by decimal formatting term
	instrMul2by1,
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// set up to convert VSS cycle value to time in seconds
	instrMul2by1,
	instrLdRegConst, 0x01, idxSecondsPerHour,			// set up to convert VSS time in seconds to time in hours
	instrMul2by1,
	instrLdReg, 0x13,									// load denominator term
	instrDiv2by1,										// divide to obtain unit distance per hour

	instrDone											// exit to caller
};

const uint8_t prgmFuelUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,	// get amount of consumed fuel in cycles
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity value
};

const uint8_t prgmRemainingFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel quantity value
};

const uint8_t prgmReserveRemainingFuel[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the reserve fuel value
};

const uint8_t prgmBingoRemainingFuel[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the bingo fuel value
};

const uint8_t prgmCalculateFuelQuantity[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 7,								// if inputted fuel quantity is zero, exit to caller

	instrMul2by1,										// multiply the fuel used value by the formatting term
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrDiv2by1,										// convert fuel value in cycles to fuel value per unit quantity
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone											// exit to caller
};

const uint8_t prgmRangeDistance[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

const uint8_t prgmReserveDistance[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrSubYfromX, 0x21,								// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelDistance					// go format it
};

#ifdef useJSONoutput
const uint8_t prgmFindHalfReserveRange[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrSubYfromX, 0x21,								// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRegRight, 0x02,							// shift result right one bit
	instrJump, tCalculateFuelDistance					// go format it
};

#endif // useJSONoutput
const uint8_t prgmBingoDistance[] PROGMEM = {
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

const uint8_t prgmDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

const uint8_t prgmReserveDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

const uint8_t prgmBingoDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

const uint8_t prgmCalculateFuelDistance[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 26,								// if inputted fuel quantity is zero, exit to caller

	instrLdRegTripVarIndexed, 0x01, rvVSSpulseIdx,		// fetch the accumulated distance travelled pulses
	instrMul2by1,										// multiply input fuel quantity by accumulated distance travelled pulses to get the numerator for distance to empty
	instrLdReg, 0x32,									// move numerator to register 3 for now

	instrLdRegTripVarIndexed, 0x01, rvInjOpenCycleIdx,	// fetch the accumulated fuel injector open cycles
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch the pulses per unit distance factor
	instrMul2by1,										// multiply accumulated fuel injector open cycles by pulses per unit distance factor
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrDiv2by1,										// divide to get denominator / decimal factor
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdReg, 0x12,									// move modified denominator to register 1 for now
	instrLdReg, 0x23,									// fetch numerator from register 3

	instrDiv2by1,										// divide to get DTE = (numerator) / (denominator / decimal factor ) = (numerator * decimal factor) / denominator
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone											// exit to caller
};

#ifdef useFuelCost
const uint8_t prgmFuelCostUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,	// get amount of consumed fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

const uint8_t prgmFuelCostTank[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

const uint8_t prgmFuelCostReserve[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrSubYfromX, 0x21,								// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

const uint8_t prgmFuelCostBingo[] PROGMEM = {
	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

const uint8_t prgmFuelCostRemaining[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

const uint8_t prgmFuelCostReserveRemaining[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

const uint8_t prgmFuelCostBingoRemaining[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

const uint8_t prgmFuelCostPerDistance[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply the two numbers to get the denominator for fuel cost per distance
	instrLdReg, 0x32,									// save denominator term

	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,	// fetch the accumulated fuel injector open cycle measurement
	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// fetch the pulses per unit distance factor
	instrMul2by1,										// multiply the two numbers to get the numerator for fuel cost per distance
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// load fuel cost per unit quantity into register 1
	instrMul2by1,										// multiply the numerator by the formatting term

	instrLdReg, 0x13,									// move the denominator term into position
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

const uint8_t prgmDistancePerFuelCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,	// fetch the accumulated fuel injector open cycle measurement
	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// fetch the pulses per unit distance factor
	instrMul2by1,										// multiply the two numbers to get the denominator for distance per fuel cost
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// load cost per unit quantity of fuel into register 1
	instrMul2by1,										// multiply the denominator by the formatting term
	instrLdReg, 0x32,									// save the denominator term

	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply the two numbers to get the numerator for distance per fuel cost
	instrLdRegConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,										// multiply the numerator by the formatting term
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrMul2by1,										// multiply the numerator by the formatting term

	instrLdReg, 0x13,									// move the denominator term into position
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

#endif // useFuelCost
const uint8_t prgmFuelRate[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,
	instrTestReg, 0x02,									// test injector open cycle value
	instrBranchIfZero, 22,								// if zero, then fuel rate is also zero

	instrLdRegConst, 0x01, idxSecondsPerHour,
	instrMul2by1,
	instrLdRegConst, 0x01, idxDecimalPoint,
	instrMul2by1,
	instrLdRegTripVarIndexed, 0x01, rvInjCycleIdx,
	instrDiv2by1,
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// for conversion of quantity/cycles to quantity/sec
	instrMul2by1,
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrDiv2by1,										// multiply by quantity/cycle (divide by cycles/quantity)
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone
};

#ifdef useFuelCost
const uint8_t prgmFuelRateCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,
	instrTestReg, 0x02,									// test injector open cycle value
	instrBranchIfZero, 22,								// if zero, then fuel rate is also zero

	instrLdRegConst, 0x01, idxSecondsPerHour,
	instrMul2by1,
	instrLdRegEEPROM, 0x01, pCostPerQuantity,
	instrMul2by1,
	instrLdRegTripVarIndexed, 0x01, rvInjCycleIdx,
	instrDiv2by1,
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// for conversion of quantity/cycles to quantity/sec
	instrMul2by1,
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrDiv2by1,										// multiply by quantity/cycle (divide by cycles/quantity)
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone
};

#endif // useFuelCost
const uint8_t prgmFuelEcon[] PROGMEM = {
	instrTraceOn,

	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply the two numbers to get the denominator for fuel economy
	instrLdReg, 0x32,									// save denominator for later

	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,	// fetch the accumulated fuel injector open cycle measurement
	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// fetch the pulses per unit distance factor
	instrMul2by1,										// multiply the two numbers to get the numerator for fuel economy

	instrTestMetricFlag, metricMode,					// check if MPGuino is in metric mode
	instrBranchIfZset, 7,								// if metric mode set, skip ahead
	instrSwapReg, 0x23,									// swap the numerator and denominator terms around
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrSkip, 3,										// go skip ahead

	instrLdRegConst, 0x01, idxMetricFE,					// load the output formatting decimal point constant, multiplied by 100 (for 100km/L)

	instrTestReg, 0x02,									// test numerator term
	instrBranchIfZero, 6,								// if zero, then fuel economy is also zero

	instrMul2by1,										// multiply the numerator by the formatting term
	instrLdReg, 0x13,									// move the denominator term into position
	instrDiv2by1,										// divide the numerator by the denominator
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone											// exit to caller
};

const uint8_t prgmAlternateFuelEcon[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply the two numbers to get the denominator for fuel economy
	instrLdReg, 0x32,									// save denominator for later

	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,	// fetch the accumulated fuel injector open cycle measurement
	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// fetch the pulses per unit distance factor
	instrMul2by1,										// multiply the two numbers to get the numerator for fuel economy

	instrTestMetricFlag, metricMode,					// check if MPGuino is in metric mode
	instrBranchIfZset, 5,								// if metric mode set, skip ahead
	instrLdRegConst, 0x01, idxMetricFE,					// load the output formatting decimal point constant, multiplied by 100 (for 100mi/gal)
	instrSkip, 5,										// go skip ahead

	instrSwapReg, 0x23,									// swap the numerator and denominator terms around
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting

	instrTestReg, 0x02,									// test numerator term
	instrBranchIfZero, 6,								// if zero, then fuel economy is also zero

	instrMul2by1,										// multiply the numerator by the formatting term
	instrLdReg, 0x13,									// move the denominator term into position
	instrDiv2by1,										// divide the numerator by the denominator
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone											// exit to caller
};

const uint8_t prgmCalculateRemainingTank[] PROGMEM = {
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
#ifdef usePartialRefuel
	instrLdRegVolatile, 0x01, vPartialRefuelTankSize,	// fetch calculated partial refuel size in injector open cycles
	instrAddYtoX, 0x21,									// add the two quantities to get total tank size
#endif // usePartialRefuel
	instrLdRegTripVar, 0x01, rvInjOpenCycleIdx, tankIdx,	// fetch tank quantity in injector open cycles
	instrCmpReg, 0x21,									// compare tank quantity to calculated tank size
	instrBranchIfLTorE,	4,								// if tank quantity <= calculated tank size, skip ahead

	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

	instrSubYfromX, 0x21,								// subtract tank injector time in cycles from calculated tank size in cycles to get remaining fuel in cycles
	instrDone											// exit to caller
};

const uint8_t prgmCalculateRemainingReserve[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 10,								// if zero, then reserve fuel is also zero

	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrCmpReg, 0x12,
	instrBranchIfLTorE, 3,								// if tank injector time in cycles <= calculated bingo size in cycles, exit to caller

	instrSubYfromX, 0x21,								// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrDone,											// exit to caller

	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone											// exit to caller
};

const uint8_t prgmCalculateBingoFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in injector open cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 9,								// if zero, then bingo fuel is also zero

	instrLdRegVolatile, 0x01, vBingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrCmpReg, 0x12,
	instrBranchIfLTorE, 2,								// if tank quantity in injector open cycles <= calculated bingo size in injector open cycles, exit to caller

	instrLdReg, 0x21,									// shift calculated bingo size in injector open cycles to register 2

	instrDone											// exit to caller
};

const uint8_t prgmInjectorOpenTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjOpenCycleIdx,
	instrLdRegConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,										// multiply the numerator by the formatting term
	instrJump, tConvertToMicroSeconds
};

const uint8_t prgmInjectorTotalTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,
	instrLdRegConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,										// multiply the numerator by the formatting term
	instrJump, tConvertToMicroSeconds
};

const uint8_t prgmVSStotalTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdRegConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,										// multiply the numerator by the formatting term
	instrJump, tConvertToMicroSeconds
};

const uint8_t prgmVSSpulseCount[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,
	instrLdRegConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,										// multiply the numerator by the formatting term
	instrDone
};

const uint8_t prgmInjectorPulseCount[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,
	instrLdRegConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,										// multiply the numerator by the formatting term
	instrDone
};

#ifdef useAnalogRead
const uint8_t prgmAnalogChannel[] PROGMEM = {
	instrCmpIndex, dfMaxValAnalogCount,					// is trip index pointing to a valid analog channel?
	instrBranchIfLT, 4,									// if so, skip ahead

	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

	instrLdRegVoltage, 0x01,							// load analog channel ADC step value
	instrLdRegConst, 0x02, idxDenomVoltage,				// load denominator to convert from voltage to ADC steps
	instrMul2by1,										// perform multiply (we're converting from ADC steps to voltage)
	instrLdRegConst, 0x01, idxNumerVoltage,				// load numerator to convert from voltage to ADC steps
	instrDiv2by1,										// go finish conversion
	instrDone											// exit to caller
};

#endif // useAnalogRead
#ifdef useChryslerMAPCorrection
const uint8_t prgmPressureChannel[] PROGMEM = {
	instrCmpIndex, vInjectorCorrectionIdx - vMAPpressureIdx,	// is trip index pointing to a valid pressure element?
	instrBranchIfLT, 4,									// if so, skip ahead

	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

	instrLdRegVolatileIndexed, 0x02, vMAPpressureIdx,	// load pre-formatted pressure element
	instrDone											// exit to caller
};

#endif // useChryslerMAPCorrection

const uint8_t prgmConvertToMicroSeconds[] PROGMEM = {
	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,	// load conversion factor for microseconds per second into register 1
	instrMul2by1,										// multiply by system cycles value in register 2
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load conversion factor for cycles to seconds into register 1
	instrDiv2by1,										// convert register 2 from system cycles to microseconds
	instrDone											// exit to caller
};

const uint8_t prgmRoundOffNumber[] PROGMEM = {
	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrAddYtoX, 0x12,									// add register 2 results to register 1
	instrBranchIfZero, 25,								// if register 1 is zero, register 2 has overflow value
	instrCmpIndex, 1,									// check if 0 or 1 right-hand digits were specified
	instrBranchIfLT, 11,								// branch if 0 right-hand digits were specified
	instrBranchIfE, 14,									// branch if 1 right-hand digit was specified
	instrCmpIndex, 2,									// check if 2 right-hand digits were specified
	instrBranchIfGT, 15,								// skip if 3 or more right-hand digits were specified

	instrLdRegByte, 0x01, 5,							// round off to nearest 1/100th
	instrSkip, 8,										// skip to add
	instrLdRegConst, 0x01, idxNumber500,				// round off to nearest whole integer
	instrSkip, 3,										// skip to add
	instrLdRegByte, 0x01, 50,							// round off to nearest 1/10th

	instrAddYtoX, 0x21,									// perform round to nearest number

	instrJump, tFormatToNumber							// go call prgmFormatToNumber to perform actual formatting
};

const uint8_t prgmFormatToNumber[] PROGMEM = {
	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrAddYtoX, 0x12,									// add register 2 results to register 1
	instrBranchIfZero, 15,								// if register 1 is zero, register 2 has overflow value
	instrCmpIndex, 3,									// check if valid number of decimal points were requested
	instrBranchIfGT, 11,								// if too many decimal points were requested, error out with overflow
	instrLdRegConst, 0x01, idxBCDdivisor,				// load 10^8 into register 1
	instrDiv2by1,										// perform division - quotient remains in register 2, and remainder goes into register 1
	instrLdRegByte, 0x03, 99,
	instrCmpReg, 0x32,									// test if quotient is greater than 99 (remainder will be less than 10^8)
	instrBranchIfLTorE, 6,								// if quotient is 99 or less, continue with processing

	instrLxdI, 6,										// load index with offset into byte 6
	instrStRegByteToIndexedU8, 0x03, 255,				// signal overflow by storing total length of 255 into byte 6 of register 3
	instrDone,

	instrLxdI, 6,										// load index with offset into byte 6
	instrStRegByteToIndexedU8, 0x03, 5,					// store total BCD length into byte 6 of register 3
	instrLxdI, 7,
	instrStRegByteToIndexedU8, 0x03, 32,				// store leading zero character into byte 7 of register 3
	instrLxdI, 1,
	instrStRegByteToIndexedU8, 0x03, 100,				// store BCD divisor into byte 3 of register 3
	instrLxdI, 2,
	instrStRegByteToIndexedU8, 0x03, 100,				// store BCD divisor into byte 2 of register 3
	instrLxdI, 3,
	instrStRegByteToIndexedU8, 0x03, 100,				// store BCD divisor into byte 1 of register 3
	instrLxdI, 4,
	instrStRegByteToIndexedU8, 0x03, 100,				// store BCD divisor into byte 0 of register 3

	instrDoBCDadjust, 0x13,								// go turn register 1 into a BCD string and store it in register 3

	instrDone											// exit to caller
};

const uint8_t prgmFormatToTime[] PROGMEM = {
	instrLdReg, 0x12,									// move time in seconds into register 1

	instrLxdI, 6,										// load index with offset into byte 6
	instrStRegByteToIndexedU8, 0x03, 3,					// store total BCD length into byte 6 of register 3
	instrLxdI, 7,
	instrStRegByteToIndexedU8, 0x03, 48,				// store leading zero character into byte 7 of register 3
	instrLxdI, 2,
	instrStRegByteToIndexedU8, 0x03, 60,				// store hours divisor into byte 2 of register 3
	instrLxdI, 1,
	instrStRegByteToIndexedU8, 0x03, 60,				// store minutes divisor into byte 1 of register 3
	instrLxdI, 0,
	instrStRegByteToIndexedU8, 0x03, 24,				// store seconds divisor into byte 0 of register 3

	instrDoBCDadjust, 0x13,								// go turn register 1 into a BCD string and store it in register 3

	instrDone											// exit to caller
};

#ifdef useDebugValueReadout
const uint8_t prgmShowDebugValue[] PROGMEM = {
	instrCmpIndex, 4,									// is a valid debug channel being requested?
	instrBranchIfLT, 4,									// if so, skip ahead

	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone,											// return to caller

	instrLdRegVolatileIndexed, 0x02, vDebugValue1Idx,	// load indexed debug value into register 2
	instrDone											// exit to caller
};

#endif // useDebugValueReadout
#ifdef useWindowTripFilter
const uint8_t prgmUpdateWindowTripFilter[] PROGMEM = {
	instrLdRegByte, 0x02, 0,							// load a zero into register 2
	instrStRegTripVar, 0x02, rvInjOpenCycleIdx, instantIdx,	// store fuel quantity consumed in cycles in instant trip variable
	instrStRegTripVar, 0x02, rvInjCycleIdx, instantIdx,	// store fuel pulse period in cycles in instant trip variable
	instrStRegTripVar, 0x02, rvInjPulseIdx, instantIdx,	// store fuel pulse count in instant trip variable
	instrStRegTripVar, 0x02, rvVSSpulseIdx, instantIdx,	// store VSS pulse count in instant trip variable
	instrStRegTripVar, 0x02, rvVSScycleIdx, instantIdx,	// store VSS pulse period in cycles in instant trip variable

	instrLxdI, windowTripFilterIdx,

	instrLdRegTripVarIndexed, 0x01, rvInjOpenCycleIdx,	// load fuel quantity consumed in cycles from current window trip variable element
	instrLdRegTripVar, 0x02, rvInjOpenCycleIdx, instantIdx,	// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,									// add them together
	instrStRegTripVar, 0x02, rvInjOpenCycleIdx, instantIdx,	// store fuel quantity consumed in cycles in instant trip variable

	instrLdRegTripVarIndexed, 0x01, rvInjCycleIdx,		// load fuel quantity consumed in cycles from current window trip variable element
	instrLdRegTripVar, 0x02, rvInjCycleIdx, instantIdx,	// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,									// add them together
	instrStRegTripVar, 0x02, rvInjCycleIdx, instantIdx,	// store fuel quantity consumed in cycles in instant trip variable

	instrLdRegTripVarIndexed, 0x01, rvInjPulseIdx,		// load fuel quantity consumed in cycles from current window trip variable element
	instrLdRegTripVar, 0x02, rvInjPulseIdx, instantIdx,	// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,									// add them together
	instrStRegTripVar, 0x02, rvInjPulseIdx, instantIdx,	// store fuel quantity consumed in cycles in instant trip variable

	instrLdRegTripVarIndexed, 0x01, rvVSSpulseIdx,		// load fuel quantity consumed in cycles from current window trip variable element
	instrLdRegTripVar, 0x02, rvVSSpulseIdx, instantIdx,	// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,									// add them together
	instrStRegTripVar, 0x02, rvVSSpulseIdx, instantIdx,	// store fuel quantity consumed in cycles in instant trip variable

	instrLdRegTripVarIndexed, 0x01, rvVSScycleIdx,		// load fuel quantity consumed in cycles from current window trip variable element
	instrLdRegTripVar, 0x02, rvVSScycleIdx, instantIdx,	// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,									// add them together
	instrStRegTripVar, 0x02, rvVSScycleIdx, instantIdx,	// store fuel quantity consumed in cycles in instant trip variable

	instrAddIndex, 1,
	instrCmpIndex, windowTripFilterIdx + windowTripFilterSize,
	instrBranchIfLT, 185,

	instrDone											// exit to caller
};

#endif // useWindowTripFilter
#ifdef useDragRaceFunction
const uint8_t prgmDragSpeed[] PROGMEM = {
	instrLdRegVolatile, 0x02, vDragInstantSpeedIdx,		// load instantaneous drag speed measurement
	instrTestReg, 0x02,									// test speed measurement
	instrBranchIfZero, 20,								// if speed measurement is zero, exit to caller

	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// set up to convert pulses per unit distance
	instrMul2by1,
	instrLdReg, 0x32,									// save denominator term for later

	instrLdRegConst, 0x01, idxDecimalPoint,				// load decimal formatting term
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// set up to convert VSS cycle value to time in seconds
	instrMul2by1,
	instrLdRegConst, 0x01, idxSecondsPerHour,			// set up to convert VSS time in seconds to time in hours
	instrMul2by1,
	instrLdReg, 0x13,									// load denominator term
	instrDiv2by1,										// divide to obtain unit distance per hour

	instrDone											// exit to caller
};

const uint8_t prgmEstimateEnginePower[] PROGMEM = {
	instrCall, tDragSpeed,								// calculate vehicle speed (this will be the maximum vehicle speed obtained during the acceleration test)

	instrTestMetricFlag, metricMode,					// check if MPGuino is in metric mode
	instrBranchIfZclear, 10,							// if metric mode clear, skip to power calculation setup

	instrLdRegConst, 0x01, idxDenomDistance,			// multiply by numerator to convert km/hr to MPH
	instrMul2by1,
	instrLdRegConst, 0x01, idxNumerDistance,			// divide by denominator to convert km/hr to MPH
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdReg, 0x12,									// load register 1 with calculated vehicle speed
	instrMul2by1,										// perform cube function on maximum accel test vehicle speed value to obtain initial denominator term
	instrMul2by1,

	instrLdRegConst, 0x01, idxPowerFactor,				// divide by power estimation term to reduce the number of bits in the estimate
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdRegEEPROM, 0x01, pVehicleMassIdx,			// fetch vehicle weight
	instrMul2by1,										// multiply estimate by vehicle weight
	instrLdRegConst, 0x01, idxPowerFactor,				// divide by power estimation term to reduce the number of bits in the estimate
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrTestMetricFlag, metricMode,					// check if MPGuino is in metric mode
	instrBranchIfZclear, 10,							// if metric mode clear, skip to power calculation

	instrLdRegConst, 0x01, idxDenomMass,				// multiply by numerator to convert kg to lbf
	instrMul2by1,
	instrLdRegConst, 0x01, idxNumerMass,				// divide by denominator to convert kg to lbf
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdRegConst, 0x01, idxPowerFactor,				// divide by power estimation term to reduce the number of bits in the estimate
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone											// exit to caller
};

const uint8_t prgmAccelTestTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrMul2by1,
	instrLdRegConst, 0x01, idxCycles0PerSecond,
	instrDiv2by1,
	instrDone
};

#endif // useDragRaceFunction
const char overFlowStr[] PROGMEM = "----------";
const char overFlow9Str[] PROGMEM = "9999999999";

const uint8_t dfAdjustWindow =		0b00000001;
const uint8_t dfOverflow9s =		0b10000000;

const uint8_t tripUpdateSrcList[] PROGMEM = {
#ifdef trackIdleEOCdata
	 eocIdleInstantIdx					// update idle tank trip with idle instant trip
	,eocIdleInstantIdx					// update idle current trip with idle instant trip
	,eocIdleInstantIdx					// update instant trip with idle instant trip
	,instantIdx							// update tank trip with instant trip
	,instantIdx							// update current trip with instant trip
#else // trackIdleEOCdata
	 instantIdx							// update tank trip with instant trip
	,instantIdx							// update current trip with instant trip
#endif // trackIdleEOCdata
#ifdef useBarFuelEconVsTime
	,instantIdx							// update bargraph periodic trip with instant trip
#endif // useBarFuelEconVsTime
};

const uint8_t tripUpdateDestList[] PROGMEM = {
#ifdef trackIdleEOCdata
	 eocIdleTankIdx 					// update idle tank trip with idle instant trip
	,eocIdleCurrentIdx 					// update idle current trip with idle instant trip
	,instantIdx							// update instant trip with idle instant trip
	,tankIdx							// update tank trip with instant trip
	,currentIdx							// update current trip with instant trip
#else // trackIdleEOCdata
	 tankIdx							// update tank trip with instant trip
	,currentIdx							// update current trip with instant trip
#endif // trackIdleEOCdata
#ifdef useBarFuelEconVsTime
	,periodIdx 							// update bargraph periodic trip with instant trip
#endif // useBarFuelEconVsTime
	,raw0tripIdx						// reset raw 0 trip index
	,raw1tripIdx						// reset raw 1 trip index
	,instantIdx							// reset instant trip index
#ifdef trackIdleEOCdata
	,raw0eocIdleTripIdx					// reset raw 0 EOC/idle trip index
	,raw1eocIdleTripIdx					// reset raw 1 EOC/idle trip index
	,eocIdleInstantIdx					// reset EOC/idle instant trip index
#endif // trackIdleEOCdata
};

const uint8_t tUScount = (sizeof(tripUpdateSrcList) / sizeof(uint8_t));
const uint8_t tUDcount = (sizeof(tripUpdateDestList) / sizeof(uint8_t));

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
static const uint8_t clockDisplayShowScreenIdx =	nextAllowedValue;
#define nextAllowedValue clockDisplayShowScreenIdx + 1
#endif // useClockDisplay

static const uint8_t mainScreenSize =				nextAllowedValue;	// this variable is used to figure out how many menu levels the main screen has

// the following screen index defines are for the parameter setting screen, and any other extra features that have been added
static const uint8_t settingScreenIdx =				nextAllowedValue;
#define nextAllowedValue settingScreenIdx + 1
#ifdef useSavedTrips
static const uint8_t tripSaveCurScreenIdx =			nextAllowedValue;
static const uint8_t tripSaveTankScreenIdx =		tripSaveCurScreenIdx + 1;
#define nextAllowedValue tripSaveTankScreenIdx + 1
#ifdef showSavedTrips
static const uint8_t tripShowCurScreenIdx =			nextAllowedValue;
static const uint8_t tripShowTankScreenIdx =		tripShowCurScreenIdx + 1;
#define nextAllowedValue tripShowTankScreenIdx + 1
#ifdef trackIdleEOCdata
static const uint8_t tripShowIECscreenIdx =			nextAllowedValue;
static const uint8_t tripShowIETscreenIdx =			tripShowIECscreenIdx + 1;
#define nextAllowedValue tripShowIETscreenIdx + 1
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
static const uint8_t partialRefuelScreenIdx =		nextAllowedValue;
#define nextAllowedValue partialRefuelScreenIdx + 1
#endif // usePartialRefuel
#ifdef useEEPROMviewer
static const uint8_t eepromViewIdx =				nextAllowedValue;
#define nextAllowedValue eepromViewIdx + 1
#endif // useEEPROMviewer
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
#ifdef useDebugValueReadout
static const uint8_t debugValueIdx =				nextAllowedValue;
#define nextAllowedValue debugValueIdx + 1
#endif // useDebugValueReadout

static const char menuVisibleSize =					nextAllowedValue;	// this variable is used to figure out how many total menu levels are visible

// the following screen index defines do not show up in the top-down menu list
const uint8_t menuScreenIdx =						nextAllowedValue;
const uint8_t numberEditScreenIdx =					menuScreenIdx + 1;
#define nextAllowedValue numberEditScreenIdx + 1
#ifdef useClockDisplay
const uint8_t clockDisplayEditScreenIdx =			nextAllowedValue;
#define nextAllowedValue clockDisplayEditScreenIdx + 1
#endif // useClockDisplay
#ifdef useScreenEditor
const uint8_t screenEditIdx =						nextAllowedValue;
#define nextAllowedValue screenEditIdx + 1
#endif // useScreenEditor

static const char menuTotalSize =					nextAllowedValue;

static const char menuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
	"Main Display\0"
#ifdef useBigFE
	"Big FuelEcon\0"
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	"FE/Time\0"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"FE/Speed\0"
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	"Big DistToE\0"
#endif // useBigDTE
#ifdef useBigTTE
	"Big TimeToE\0"
#endif // useBigTTE
#ifdef useCPUreading
	"CPU Info\0"
#endif // useCPUreading
#ifdef useClockDisplay
	"Clock\0"
#endif // useClockDisplay
	"Settings\0"
#ifdef useSavedTrips
	"Load/Save CURR\0"
	"Load/Save TANK\0"
#ifdef showSavedTrips
	"See Saved CURR\0"
	"See Saved TANK\0"
#ifdef trackIdleEOCdata
	"See Saved cC/I\0"
	"See Saved tC/I\0"
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef useEEPROMviewer
	"EEPROM Viewer\0"
#endif // useEEPROMviewer
#ifdef usePartialRefuel
	"Partial Refuel\0"
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	"Accel Test\0"
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	"Coastdown\0"
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	"Sensor Sim\0"
#endif // useSimulatedFIandVSS
#ifdef useDebugValueReadout
	"Debug Values\0"
#endif // useDebugValueReadout
};

static const uint8_t menuIndexList[menuVisibleSize] PROGMEM = {
	 mainScreenIdx
#ifdef useBigFE
	,bigFEscreenIdx
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,barFEvTscreenIdx
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,barFEvSscreenIdx
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,bigDTEscreenIdx
#endif // useBigDTE
#ifdef useBigTTE
	,bigTTEscreenIdx
#endif // useBigTTE
#ifdef useCPUreading
	,CPUmonScreenIdx
#endif // useCPUreading
#ifdef useClockDisplay
	,clockDisplayShowScreenIdx
#endif // useClockDisplay
	,settingScreenIdx
#ifdef useSavedTrips
	,tripSaveCurScreenIdx
	,tripSaveTankScreenIdx
#ifdef showSavedTrips
	,tripShowCurScreenIdx
	,tripShowTankScreenIdx
#ifdef trackIdleEOCdata
	,tripShowIECscreenIdx
	,tripShowIETscreenIdx
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef useEEPROMviewer
	,eepromViewIdx
#endif // useEEPROMviewer
#ifdef usePartialRefuel
	,partialRefuelScreenIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,dragRaceIdx
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,coastdownIdx
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,debugReadingIdx
#endif // useSimulatedFIandVSS
#ifdef useDebugValueReadout
	,debugValueIdx
#endif // useDebugValueReadout
};

typedef struct
{

	uint8_t buttonCode;
	void (* buttonCommand)(void);

} buttonVariable;

typedef struct
{

	uint8_t modeIndex;
	uint8_t modeYcount;
	uint8_t modeXcount;
	uint8_t (* screenDisplayHandler)(uint8_t cmd, uint8_t cursorPos);
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
		,{btnShortPressC,	menu::doEnter}
		,{btnLongPressUR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressUL,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useSavedTrips
		,{btnShortPressDR,	doGoTripSaveCurrent}
		,{btnShortPressDL,	doGoTripSaveTank}
	#endif // useSavedTrips
	#ifdef useScreenEditor
		,{btnLongPressC,	doGoScreenEdit}
	#endif // useScreenEditor
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
		,{btnLongPressCR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressLC,	doTripResetTank}
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
	,{btnLongPressL,	menu::doEnter}
	,{btnShortPressLR,	numberEdit::goEditNumber}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::doEnter}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp, 		noSupport}
};

static const buttonVariable bpListNumberEdit[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	numberEdit::cancelEdit}
	,{btnLongPressLR,	numberEdit::cancelEdit}
	,{btnLongPressC,	numberEdit::saveEdit}
	,{btnShortPressLR,	numberEdit::saveEdit}
	,{btnLongPressR,	numberEdit::revert}
#ifdef useButtonCrossConfig
		,{btnShortPressUL,	numberEdit::findLeft}
		,{btnShortPressUR,	numberEdit::findRight}
		,{btnLongPressD,	numberEdit::readMinValue}
		,{btnLongPressU,	numberEdit::readMaxValue}
		,{btnShortPressU,	numberEdit::changeDigitUp}
		,{btnShortPressD,	numberEdit::changeDigitDown}
		,{btnShortPressC,	numberEdit::saveEdit}
#else // useButtonCrossConfig
		,{btnShortPressLC,	numberEdit::findLeft}
		,{btnShortPressCR,	numberEdit::findRight}
		,{btnLongPressCR,	numberEdit::readMinValue}
		,{btnLongPressLC,	numberEdit::readMaxValue}
		,{btnShortPressC,	numberEdit::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#ifdef useBigNumberDisplay
static const buttonVariable bpListBigNum[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::doEnter}
		,{btnLongPressUR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressUL,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
		,{btnLongPressCR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressLC,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useBigNumberDisplay
#ifdef useClockDisplay
static const buttonVariable bpListClockDisplayShow[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
	,{btnLongPressC,	clockDisplay::goSetClock}
	,{btnShortPressLR,	clockDisplay::goSetClock}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::doEnter}
		,{btnLongPressUR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressUL,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
		,{btnLongPressCR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressLC,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

static const buttonVariable bpListClockDisplayEdit[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	clockDisplay::cancelClockSet}
	,{btnLongPressLR,	clockDisplay::cancelClockSet}
	,{btnLongPressC,	clockDisplay::setClock}
	,{btnShortPressLR,	clockDisplay::setClock}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	clockDisplay::changeDigitUp}
		,{btnShortPressD,	clockDisplay::changeDigitDown}
		,{btnShortPressC,	clockDisplay::setClock}
#else // useButtonCrossConfig
		,{btnShortPressC,	clockDisplay::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useClockDisplay
#ifdef useSimulatedFIandVSS
static const buttonVariable bpListDebugReading[] PROGMEM = {
	{btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::doEnter}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::doEnter}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif //useSimulatedFIandVSS
#ifdef useDebugValueReadout
static const buttonVariable bpListDebugValue[] PROGMEM = {
	{btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::doEnter}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::doEnter}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useDebugValueReadout
#ifdef useCPUreading
static const buttonVariable bpListCPUmonitor[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::doEnter}
		,{btnLongPressUR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressUL,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useBenchMark
		,{btnLongPressU,	systemInfo::doBenchMark}
	#endif // useBenchMark
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
		,{btnShortPressLCR,	menu::doEnter}
		,{btnLongPressCR,	doTripResetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	doGoPartialRefuel}
	#else // usePartialRefuel
		,{btnLongPressLC,	doTripResetTank}
	#endif // usePartialRefuel
	#ifdef useBenchMark
		,{btnLongPressC,	systemInfo::doBenchMark}
	#endif // useBenchMark
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // useCPUreading
#ifdef usePartialRefuel
static const buttonVariable bpListPartialRefuel[] PROGMEM = {
	{btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::doEnter}	// this should go back to previous screen
	,{btnShortPressC,	menu::doEnter}	// this should perform selection
#ifdef useButtonCrossConfig
		,{btnShortPressU,	doNextBright}
#else // useButtonCrossConfig
		,{btnLongPressC,	doNextBright}
#endif // useButtonCrossConfig
	,{buttonsUp,		noSupport}
};

#endif // usePartialRefuel
#ifdef useButtonCrossConfig
#ifdef useBarFuelEconVsTime
static const buttonVariable bpListBFET[] PROGMEM = {
	{btnLongPressCR,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressLC,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressLC,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useCPUreading
	,{btnLongPressU,	systemInfo::showCPUloading}
#endif // useCPUreading
#ifdef useSavedTrips
	,{btnShortPressCR,	doGoTripSaveCurrent}
	,{btnShortPressLC,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const buttonVariable bpListBFES[] PROGMEM = {
	{btnLongPressCR,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressLC,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressLC,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useCPUreading
	,{btnLongPressU,	systemInfo::showCPUloading}
#endif // useCPUreading
	,{btnLongPressUC,	doResetBarFEvS}
#ifdef useSavedTrips
	,{btnShortPressCR,	doGoTripSaveCurrent}
	,{btnShortPressLC,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
static const buttonVariable bpListTripSave[] PROGMEM = {
	{btnShortPressU,	doTripSelect}
	,{btnLongPressCR,	doTripResetCurrent}
	,{btnLongPressLC,	doTripResetTank}
	,{btnLongPressU,	menu::select}
	,{btnShortPressD,	doTripLongSelect}
	,{buttonsUp,		noSupport}
};

#ifdef showSavedTrips
static const buttonVariable bpListTripView[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // showSavedTrips
#endif // useSavedTrips
#ifdef useScreenEditor
static const buttonVariable bpListScreenEdit[] PROGMEM = {
	{btnShortPressU,	doScreenEditBump}
	,{btnShortPressD,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressU,	doSaveScreen}
	,{buttonsUp,		noSupport}
};

#endif // useScreenEditor
#ifdef useEEPROMviewer
static const buttonVariable bpListEEPROMview[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // useEEPROMviewer
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
#ifdef useBarFuelEconVsTime
static const buttonVariable bpListBFET[] PROGMEM = {
	{btnLongPressCR,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressLC,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressLC,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useCPUreading
	,{btnShortPressLR,	systemInfo::showCPUloading}
#endif // useCPUreading
#ifdef useSavedTrips
	,{btnShortPressCR,	doGoTripSaveCurrent}
	,{btnShortPressLC,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const buttonVariable bpListBFES[] PROGMEM = {
	{btnLongPressCR,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressLC,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressLC,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useCPUreading
	,{btnShortPressLR,	systemInfo::showCPUloading}
#endif // useCPUreading
	,{btnLongPressLCR,	doResetBarFEvS}
#ifdef useSavedTrips
	,{btnShortPressCR,	doGoTripSaveCurrent}
	,{btnShortPressLC,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
static const buttonVariable bpListTripSave[] PROGMEM = {
	{btnShortPressC,	doTripSelect}
	,{btnLongPressCR,	doTripResetCurrent}
	,{btnLongPressLC,	doTripResetTank}
	,{btnLongPressLR,	menu::select}
	,{btnLongPressC,	doTripLongSelect}
	,{buttonsUp,		noSupport}
};

#ifdef showSavedTrips
static const buttonVariable bpListTripView[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // showSavedTrips
#endif // useSavedTrips
#ifdef useScreenEditor
static const buttonVariable bpListScreenEdit[] PROGMEM = {
	{btnShortPressC,	doScreenEditBump}
	,{btnLongPressLR,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressC,	doSaveScreen}
	,{buttonsUp,		noSupport}
};

#endif // useScreenEditor
#ifdef useEEPROMviewer
static const buttonVariable bpListEEPROMview[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // useEEPROMviewer
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
static const modeInformation screenParameters[(unsigned int)(menuTotalSize)] PROGMEM = {
	{mainScreenIdx,							mainScreenSize,		displayPageCount,					mainScreenDisplayHandler,			bpListMain}
#ifdef useBigFE
	,{mainScreenIdx,						mainScreenSize,		3,									bigNumber::displayHandler,			bpListBigNum}
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,{mainScreenIdx,						mainScreenSize,		2,									doBarFEvTdisplay,			doCursorUpdateBarFEvT,		bpListBFET}
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,{mainScreenIdx,						mainScreenSize,		4,									doBarFEvSdisplay,			doCursorUpdateBarFEvS,		bpListBFES}
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,{mainScreenIdx,						mainScreenSize,		3,									bigNumber::displayHandler,			bpListBigNum}
#endif // useBigDTE
#ifdef useBigTTE
	,{mainScreenIdx,						mainScreenSize,		3,									bigNumber::displayHandler,			bpListBigNum}
#endif // useBigTTE
#ifdef useCPUreading
	,{mainScreenIdx | 0x80,					mainScreenSize,		1,									systemInfo::displayHandler,			bpListCPUmonitor}
#endif // useCPUreading
#ifdef useClockDisplay
	,{mainScreenIdx | 0x80,					mainScreenSize,		1,									bigNumber::displayHandler,			bpListClockDisplayShow}
#endif // useClockDisplay
	,{settingScreenIdx | 0x80,				1,					eePtrSettingsEnd - 1,				settings::displayHandler,			bpListSetting}
#ifdef useSavedTrips
	,{tripSaveCurScreenIdx | 0x80,			2,					tripSaveMenuSize,					doTripSaveDisplay,			doNothing,					bpListTripSave}
	,{tripSaveCurScreenIdx | 0x80,			2,					tripSaveMenuSize,					doTripSaveDisplay,			doNothing,					bpListTripSave}
#ifdef showSavedTrips
	,{tripShowCurScreenIdx | 0x80,			savedTripsCount,	tripSlotPtrSize,					doTripShowDisplay,			doNothing,					bpListTripView}
	,{tripShowCurScreenIdx | 0x80,			savedTripsCount,	tripSlotPtrSize,					doTripShowDisplay,			doNothing,					bpListTripView}
#ifdef trackIdleEOCdata
	,{tripShowCurScreenIdx | 0x80,			savedTripsCount,	tripSlotPtrSize,					doTripShowDisplay,			doNothing,					bpListTripView}
	,{tripShowCurScreenIdx | 0x80,			savedTripsCount,	tripSlotPtrSize,					doTripShowDisplay,			doNothing,					bpListTripView}
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
	,{partialRefuelScreenIdx | 0x80,		1,					3,									doPartialRefuelDisplay,		doNothing,					bpListPartialRefuel}
#endif // usePartialRefuel
#ifdef useEEPROMviewer
	,{eepromViewIdx | 0x80,					1,					eePtrEnd,							EEPROM::display,			doNothing,					bpListEEPROMview}
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
	,{dragRaceIdx,							1,					4,									accelerationTest::goDisplay,	doNothing,				bpListDragRace}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,{coastdownIdx | 0x80,					1,					3,									coastdown::goDisplay,		doNothing,					bpListCoastdown}
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,{debugReadingIdx | 0x80,				1,					4,									debugReading::displayHandler,		bpListDebugReading}
#endif // useSimulatedFIandVSS
#ifdef useDebugValueReadout
	,{debugValueIdx | 0x80,					1,					1,									debugValueDisplay::displayHandler,	bpListDebugValue}
#endif // useDebugValueReadout
	,{menuScreenIdx | 0x80,					1,					menuVisibleSize,					menu::displayHandler,				bpListMenu}
	,{numberEditScreenIdx | 0x80,			1,					12,									numberEdit::displayHandler,			bpListNumberEdit}
#ifdef useClockDisplay
	,{clockDisplayEditScreenIdx | 0x80,		1,					4,									bigNumber::displayHandler,			bpListClockDisplayEdit}
#endif // useClockDisplay
#ifdef useScreenEditor
	,{screenEditIdx | 0x80,					1,					mainScreenDisplayFormatSize * 2,	doScreenEditDisplay,		doCursorUpdateScreenEdit,	bpListScreenEdit}
#endif // useScreenEditor
};

uint8_t screenCursor[(unsigned int)(menuTotalSize)] = {
	0
#ifdef useBigFE
	,0
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,0
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,0
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,0
#endif // useBigDTE
#ifdef useBigTTE
	,0
#endif // useBigTTE
#ifdef useCPUreading
	,0
#endif // useCPUreading
#ifdef useClockDisplay
	,0
#endif // useClockDisplay
	,0
#ifdef useSavedTrips
	,0
	,0
#ifdef showSavedTrips
	,0
	,0
#ifdef trackIdleEOCdata
	,0
	,0
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
	,0
#endif // usePartialRefuel
#ifdef useEEPROMviewer
	,0
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
	,0
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,0
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,0
#endif // useSimulatedFIandVSS
#ifdef useDebugValueReadout
	,0
#endif // useDebugValueReadout
	,0
	,0
#ifdef useClockDisplay
	,0
#endif // useClockDisplay
#ifdef useScreenEditor
	,0
#endif // useScreenEditor
};

static uint8_t cursorXdirection;

static uint8_t menu::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t i;
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
			i = cursorPos;
			if (cursorXdirection & 0x80)
			{

				i--;
				if (i > menuVisibleSize) i += menuVisibleSize;

			}

			for (uint8_t x = 0; x < 2; x++)
			{

				text::gotoXY(devLCD, 0, x);
				text::charOut(devLCD, ((i == cursorPos) ? '>' : ' ' ));
				text::stringOut(devLCD, menuTitles, i);
				text::clearEOL(devLCD);
				i++;
				if (i >= menuVisibleSize) i = 0;

			}
			break;

		default:
			break;

	}

	return retVal;

}

static void menu::doEnter(void)
{

	cursor::moveAbsolute(menuScreenIdx, 255); // go to the menu screen level

}

static void menu::select(void)
{

	uint8_t i;

	i = screenCursor[(unsigned int)(menuScreenIdx)];

	cursor::moveAbsolute(pgm_read_byte(&menuIndexList[(unsigned int)(i)]), 255);

}

/* LCD screen cursor manipulation section */

static void cursor::screenLevelEntry(const char * s, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	printStatusMessage(s);

}

static void cursor::moveAbsolute(uint8_t positionY, uint8_t positionX)
{

	uint8_t levelChangeFlag;

	if (menuLevel == positionY) levelChangeFlag = 0;
	else
	{

		menuLevel = positionY;
		levelChangeFlag = 1;

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

	((handlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(cf, cp);

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

	initStatusLine();
	text::stringOut(devLCD, s, strIdx);
	execStatusLine();

}

void doNothing(void)
{
}

void noSupport(void)
{

	initStatusLine();
	text::stringOut(devLCD, PSTR("Btn 0x"));
	text::hexByteOut(devLCD, buttonState);
#ifdef useTestButtonValues
	text::hexByteOut(devLCD, buttonIdx);
#ifdef useAnalogButtons
	text::hexWordOut(devLCD, (uint16_t)(analogValue[(unsigned int)(analogButtonIdx)]));
#else // useAnalogButtons
	text::stringOut(devLCD, PSTR(" Pressed"));
#endif // useAnalogButtons
#else // useTestButtonValues
	text::stringOut(devLCD, PSTR(" Pressed"));
#endif // useTestButtonValues
	execStatusLine();
}

void tripVar::reset(void)
{

	for (uint8_t x = 0; x < rvLength; x++) collectedData[(unsigned int)(x)] = 0;

}

void tripVar::transfer(tripVar t)
{

	for (uint8_t x = 0; x < rvLength; x++) collectedData[(unsigned int)(x)] = t.collectedData[(unsigned int)(x)];

}

void tripVar::update(tripVar src)
{

	add32(rvVSSpulseIdx, src.collectedData[(unsigned int)(rvVSSpulseIdx)]);
	add32(rvInjPulseIdx, src.collectedData[(unsigned int)(rvInjPulseIdx)]);

	for (uint8_t x = rvVSScycleIdx; x < rvLength; x += 2)
	{

#ifdef useAssemblyLanguage
		asm volatile(
		"	add	%A0, %A2	\n"
		"	adc	%B0, %B2	\n"
		"	adc	%C0, %C2	\n"
		"	adc	%D0, %D2	\n"
		"	adc	%A1, %A3	\n"
		"	adc	%B1, %B3	\n"
		"	adc	%C1, %C3	\n"
		"	adc	%D1, %D3	\n"
		: "+r" (collectedData[(unsigned int)(x)]), "+r" (collectedData[(unsigned int)(x + 1)])
		: "r" (src.collectedData[(unsigned int)(x)]), "r" (src.collectedData[(unsigned int)(x + 1)])
		);
#else // useAssemblyLanguage
		add64s(x, src.collectedData[(unsigned int)(x)]);
		add32(x + 1, src.collectedData[(unsigned int)(x + 1)]);
#endif // useAssemblyLanguage

	}

}

void tripVar::add64s(uint8_t calcIdx, unsigned long v)
{

#ifdef useAssemblyLanguage
	asm volatile(
	"	add	%A0, %A2	\n"
	"	adc	%B0, %B2	\n"
	"	adc	%C0, %C2	\n"
	"	adc	%D0, %D2	\n"
	"	adc	%A1, __zero_reg__	\n"
	"	adc	%B1, __zero_reg__	\n"
	"	adc	%C1, __zero_reg__	\n"
	"	adc	%D1, __zero_reg__	\n"
	: "+r" (collectedData[(unsigned int)(calcIdx)]), "+r" (collectedData[(unsigned int)(calcIdx + 1)])
	: "r" (v)
	);
#else // useAssemblyLanguage
	add32(calcIdx, v); // add to accumulator
	if (collectedData[(unsigned int)(calcIdx)] < v) collectedData[(unsigned int)(calcIdx + 1)]++; // handle any possible overflow
#endif // useAssemblyLanguage

}

void tripVar::add32(uint8_t calcIdx, unsigned long v)
{

#ifdef useAssemblyLanguage
	asm volatile(
	"	add	%A0, %A1	\n"
	"	adc	%B0, %B1	\n"
	"	adc	%C0, %C1	\n"
	"	adc	%D0, %D1	\n"
	: "+r" (collectedData[(unsigned int)(calcIdx)])
	: "r" (v)
	);
#else // useAssemblyLanguage
	collectedData[(unsigned int)(calcIdx)] += v;
#endif // useAssemblyLanguage

}

#ifdef useEEPROMtripStorage
uint8_t getBaseTripPointer(uint8_t tripSlot)
{

	return (tripSlot * tripSlotPtrSize) + eePtrSavedTripsStart;

}

uint8_t tripVar::load(uint8_t tripPos)
{

	uint8_t t = getBaseTripPointer(tripPos);
	uint8_t b = (uint8_t)(EEPROM::readVal(t + tripSlotSignaturePtr));

	reset();

	if (b == guinosig)
	{

		for (uint8_t x = 0; x < tripListPtrLength; x++) collectedData[(unsigned int)(x)] = EEPROM::readVal(++t);

		b = 1;

	}
	else b = 0;

	return b;

}

uint8_t tripVar::save(uint8_t tripPos)
{

	uint8_t t = getBaseTripPointer(tripPos);

#ifdef useSoftwareClock
	EEPROM::writeVal(t++, convertTime((uint32_t * )(&clockCycles)));
#else // useSoftwareClock
	EEPROM::writeVal(t++, convertTime((uint32_t *)(&systemCycles)));
#endif // useSoftwareClock

	for (uint8_t x = 0; x < tripListPtrLength; x++) EEPROM::writeVal(t++, collectedData[(unsigned int)(x)]);

	EEPROM::writeVal(t++, guinosig);

	return 1;

}

#endif // useEEPROMtripStorage
#ifdef useSWEET64mult
const uint8_t prgmMult64[] PROGMEM = {
	instrLdReg, 0x41,									// load multiplier into register 4
	instrLdReg, 0x52,									// load multiplicand into register 5
	instrLdRegByte, 0x02, 0,							// zero out result (register 2)
	instrSkip, 8,										// skip forward to multiplier test

	instrShiftRegRight, 0x04,							// shift multiplier right by one bit
	instrBranchIfCclear, 2,								// if multiplier bit was clear, don't add multiplicand to result
	instrAddYtoX, 0x25,									// add multiplicand to result
	instrShiftRegLeft, 0x05,							// shift multiplicand left by one bit
	instrTestReg, 0x04,									// test multiplier register
	instrBranchIfNotZero, 244,							// if multiplier is not zero, loop back

	instrTraceDone										// exit to caller
};

#endif // useSWEET64mult
#ifdef useSWEET64div
const uint8_t prgmDiv64[] PROGMEM = {
	instrLdReg, 0x41,									// initialize divisor register
	instrLdReg, 0x12,									// initialize remainder with dividend
	instrLdRegByte, 0x05, 1,							// load quotient bitmask with a 1
	instrTestReg, 0x04,									// test divisor register
	instrBranchIfNotZero, 11,							// if divisor is not zero, skip ahead

	instrLdRegByte, 0x02, 0,							// zero out quotient
	instrSubYfromX, 0x25,								// set overflow value in quotient
	instrLdReg, 0x12,									// set overflow value in remainder
	instrLdRegByte, 0x04, 0,							// zero out divisor
	instrTraceDone,										// exit to caller

	instrTestReg, 0x02,									// test dividend register
	instrBranchIfZero, 248,								// if dividend is zero, go exit
	instrLdRegByte, 0x02, 0,							// initialize quotient (register 2)

	instrTestReg, 0x04,									// test dividend register
	instrBranchIfMinus, 6,								// if dividend is already fully shifted left, skip
	instrShiftRegLeft, 0x05,							// shift quotient bitmask left one bit
	instrShiftRegLeft, 0x04,							// shift divisor left one bit
	instrBranchIfPlus, 250,								// if dividend is not fully shifted left, loop back

	instrCmpReg, 0x14,									// compare divisor to dividend
	instrBranchIfGT, 4,									// if divisor is greater than dividend, skip to bitmask adjustment
	instrSubYfromX, 0x14,								// subtract divisor from dividend
	instrAddYtoX, 0x25,	       							// add quotient bitmask to quotient
	instrShiftRegRight, 0x05,							// shift quotient bitmask right by one bit
	instrBranchIfCset, 4,								// if quotient bitmask empty, skip ahead
	instrShiftRegRight, 0x04,							// shift divisor right by one bit
	instrBranchIfNotZero, 240,							// go back to quotient bitmask test

	instrShiftRegLeft, 0x01,							// shift remainder left one bit to multiply by 2
	instrCmpReg, 0x14,									// compare divisor to remainder
	instrBranchIfLTorE, 5,								// if divisor is less than or equal to remainder, skip ahead
	instrLdRegByte, 0x04, 0,							// zero out divisor
	instrSkip, 3,										// skip ahead to restore remainder

	instrLdRegByte, 0x04, 1,							// load divisor with a 1
	instrShiftRegRight, 0x01,							// shift remainder right one bit to restore it
	instrTraceDone										// exit to caller
};

#endif // useSWEET64div
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
#ifdef useAnalogRead
	,prgmAnalogChannel									// tAnalogChannel - DC voltage
#endif // useAnalogRead
#ifdef useCarVoltageOutput
	,prgmAlternatorChannel								// tAlternatorChannel - DC voltage
	,prgmSpareVoltageChannel							// tSpareVoltageChannel - DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	,prgmAccelTestTime									// tAccelTestTime - acceleration test time (s.s)
#endif // useDragRaceFunction
#ifdef useDebugValueReadout
	,prgmShowDebugValue									// tShowDebugValue - internal volatile variable value
#endif // useDebugValueReadout
	,prgmFuelUsed										// tFuelUsed - fuel quantity used (SI/SAE)
	,prgmFuelRate										// tFuelRate - fuel consumption rate (SI/SAE)
	,prgmDistance										// tDistance - vehicle distance traveled (SI/SAE)
	,prgmSpeed											// tSpeed - vehicle speed (SI/SAE)
	,prgmFuelEcon										// tFuelEcon - fuel economy (SI/SAE)
	,prgmAlternateFuelEcon								// tAlternateFuelEcon - alternate format fuel economy (SI/SAE)
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

// this is the start of the internal SWEET64 index program address list
	,prgmCalculateRemainingTank							// tCalculateRemainingTank - calculate estimated remaining fuel quantity in injector open cycles
	,prgmCalculateRemainingReserve						// tCalculateRemainingReserve - calculate estimated remaining fuel reserve value in injector open cycles
	,prgmCalculateBingoFuel								// tCalculateBingoFuel - calculate estimated fuel bingo value in injector open cycles
	,prgmConvertToMicroSeconds
	,prgmCalculateFuelQuantity
	,prgmCalculateFuelDistance
	,prgmCalculateFuelTime
	,prgmFormatToNumber
};

uint8_t SWEET64_fetchbyte(const uint8_t * &prgmPtr)
{

	uint8_t byt;

	byt = pgm_read_byte(prgmPtr++);

#ifdef useSWEET64trace
	if (SWEET64processorFlags & SWEET64traceFlag)
	{

		text::hexByteOut(devDebugSerial, byt);
		text::charOut(devDebugSerial, 32);

	}

#endif // useSWEET64trace
	return byt;

}

unsigned long SWEET64(const uint8_t * sched, uint8_t tripIdx)
{

	uint8_t spnt = 0;
	const uint8_t * prgmStack[16];
	uint8_t instr;
	uint8_t operand;
	uint8_t loopFlag = true;
	uint8_t branchFlag;

	union union_64 * tu1 = (union union_64 *)(&s64reg[(uint16_t)(s64reg1)]);
	union union_64 * tu2 = (union union_64 *)(&s64reg[(uint16_t)(s64reg2)]);
#ifdef useSWEET64trace
	union union_64 * tus;
	uint8_t traceSave;
	uint8_t i;
#endif // useSWEET64trace

	SWEET64processorFlags = 0;

	while (loopFlag)
	{

#ifdef useSWEET64trace
		i = (SWEET64processorFlags & SWEET64traceFlagGroup);
		if (i == SWEET64traceFlag) SWEET64processorFlags &= ~(SWEET64traceFlag);
		if (i == SWEET64traceCommandFlag) SWEET64processorFlags |= (SWEET64traceFlag);

		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugSerial, 13);
			text::hexWordOut(devDebugSerial, (uint16_t)(sched));
			text::charOut(devDebugSerial, 32);
			text::hexByteOut(devDebugSerial, tripIdx);
			text::charOut(devDebugSerial, 32);
			text::hexByteOut(devDebugSerial, spnt);
			text::charOut(devDebugSerial, 32);

		}

#endif // useSWEET64trace
		instr = SWEET64_fetchbyte(sched);

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::stringOut(devDebugSerial, opcodeList, (instr & 0x3F));
			text::charOut(devDebugSerial, 13);
			text::charOut(devDebugSerial, 9);

		}

#endif // useSWEET64trace
		if (instr & 0x40)
		{

			operand = SWEET64_fetchbyte(sched);

			if (operand & 0x70) tu1 = (union union_64 *)(&s64reg[(unsigned int)(((operand & 0x70) >> 4) - 1)]);
			if (operand & 0x07) tu2 = (union union_64 *)(&s64reg[(unsigned int)((operand & 0x07) - 1)]);

		}

		if (instr & 0x80) operand = SWEET64_fetchbyte(sched);

		branchFlag = false;

		switch (instr)
		{

			case instrTraceDone: // this instruction is basically "instrDone" if #useSWEET64trace is not chosen
#ifdef useSWEET64trace
				if (traceSave & SWEET64traceFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
				else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
			case instrDone:
				if (spnt--) sched = prgmStack[(unsigned int)(spnt)];
				else loopFlag = false;
				break;

			case instrCall:
				prgmStack[(unsigned int)(spnt++)] = sched;
				if (spnt > 15) loopFlag = false;
			case instrJump:
				sched = (const uint8_t *)pgm_read_word(&S64programList[(unsigned int)(operand)]);
				break;

			case instrSkip:
				branchFlag = true;
				break;

			case instrBranchIfMclear:
				branchFlag = ((SWEET64processorFlags & SWEET64minusFlag) == 0);
				break;

			case instrBranchIfMset:
				branchFlag = (SWEET64processorFlags & SWEET64minusFlag);
				break;

			case instrBranchIfZclear:
				branchFlag = ((SWEET64processorFlags & SWEET64zeroFlag) == 0);
				break;

			case instrBranchIfZset:
				branchFlag = (SWEET64processorFlags & SWEET64zeroFlag);
				break;

			case instrBranchIfCclear:
				branchFlag = ((SWEET64processorFlags & SWEET64carryFlag) == 0);
				break;

			case instrBranchIfCset:
				branchFlag = (SWEET64processorFlags & SWEET64carryFlag);
				break;

			case instrBranchIfLT:
				branchFlag = (((SWEET64processorFlags & SWEET64zeroFlag) == 0) && ((SWEET64processorFlags & SWEET64carryFlag) == 0));
				break;

			case instrBranchIfGTorE:
				branchFlag = ((SWEET64processorFlags & SWEET64zeroFlag) || (SWEET64processorFlags & SWEET64carryFlag));
				break;

			case instrTestMetricFlag:
				if ((metricFlag & operand) == operand) SWEET64processorFlags |= (SWEET64zeroFlag);
				else SWEET64processorFlags &= ~(SWEET64zeroFlag);
				break;

			case instrTestReg: // this cannot affect the carry flag!
				registerTest64(tu2);
				break;

			case instrTestIndex: // this cannot affect the carry flag!
				if (tripIdx) SWEET64processorFlags &= ~(SWEET64zeroFlag);
				else SWEET64processorFlags |= (SWEET64zeroFlag);

				if (tripIdx & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
				else SWEET64processorFlags &= ~(SWEET64minusFlag);

				break;

			case instrCmpReg:
				cmp64(tu1, tu2);
				break;

			case instrCmpIndex:
				if (operand < tripIdx) SWEET64processorFlags |= (SWEET64carryFlag);
				else SWEET64processorFlags &= ~(SWEET64carryFlag);

				if (operand == tripIdx) SWEET64processorFlags |=(SWEET64zeroFlag);
				else SWEET64processorFlags &= ~(SWEET64zeroFlag);

				if ((operand - tripIdx) & 0x80) SWEET64processorFlags |=(SWEET64minusFlag);
				else SWEET64processorFlags &= ~(SWEET64minusFlag);

				break;

			case instrShiftRegLeft:
				SWEET64processorFlags &= ~(SWEET64carryFlag);
//			case instrRollRegLeft:
				shl64(tu2);
				break;

			case instrShiftRegRight:
				SWEET64processorFlags &= ~(SWEET64carryFlag);
//			case instrRollRegRight:
				shr64(tu2);
				break;

			case instrLxdI:
				tripIdx = operand;
				break;

			case instrLxdIEEPROMindexed:
				operand += tripIdx;
			case instrLxdIEEPROM:
				tripIdx = EEPROM::readVal(operand);
				break;

			case instrAddIndex:
				tripIdx += operand;
				break;

			case instrLdReg:
				copy64(tu1, tu2);
				break;

			case instrLdRegByteFromIndex:
				init64(tu2, tripIdx);
				break;

			case instrLdRegByteFromYindexed:
				operand = tu1->u8[(unsigned int)(tripIdx)];
			case instrLdRegByte:
				init64(tu2, operand);
				break;

			case instrLdRegTripVar:
				tripVarLoad64(tu2, SWEET64_fetchbyte(sched), operand);
				break;

			case instrLdRegTripVarIndexed:
				tripVarLoad64(tu2, tripIdx, operand);
				break;

			case instrLdRegNumer:
			case instrLdRegDenom:
				operand = pgm_read_byte(&convNumerIdx[(unsigned int)(tripIdx)]);
				if (metricFlag & metricMode) operand ^= 1;
				if (instr == instrLdRegDenom) operand ^= 1;
			case instrLdRegConst:
				init64(tu2, pgm_read_dword(&convNumbers[(unsigned int)(operand)]));
				break;

			case instrLdRegEEPROMindexed:
				operand += tripIdx;
				init64(tu2, EEPROM::readVal(operand));
				break;

			case instrLdRegEEPROMindirect:
				operand = pgm_read_byte(&convIdx[(unsigned int)(tripIdx)]);
			case instrLdRegEEPROM:
				init64(tu2, EEPROM::readVal(operand));
				break;

			case instrStRegEEPROMindirect:
				operand = pgm_read_byte(&convIdx[(unsigned int)(tripIdx)]);
			case instrStRegEEPROM:
				EEPROM::writeVal(operand, tu2->ul[0]);
				break;

			case instrLdRegVolatileIndexed:
				operand += tripIdx;
			case instrLdRegVolatile:
				init64(tu2, volatileVariables[(unsigned int)(operand)]);
				break;

			case instrStRegVolatileIndexed:
				operand += tripIdx;
			case instrStRegVolatile:
				volatileVariables[(unsigned int)(operand)] = tu2->ul[0];
				break;

			case instrStRegByteToIndexedU8:
				tu2->u8[(unsigned int)(tripIdx)] = operand;
				break;

			case instrDoBCDadjust:
				instr = tu2->u8[6]; // fetch length

				if (instr < 4) tripIdx = instr - 1;
				else
				{

					tripIdx = 3;
					tu2->u8[0] = ((union union_64 *)(&s64reg[s64reg2]))->u8[0]; // workaround to support 10 decimal digits / 5 BCD bytes

				}

				for (uint8_t x = tripIdx; x < 4; x--)
				{

					operand = tu2->u8[(uint16_t)(--instr)]; // get indexed divisor
					if (operand)
					{

						tu2->u8[(uint16_t)(instr)] = (uint8_t)(tu1->ul[0] % operand); // put result of (source register) mod divisor into indexed byte of (target register)
						tu1->ul[0] /= operand; // divide (source register) by divisor

					}
					else tu2->u8[(uint16_t)(instr)] = operand;

				}

				break;

			case instrSwapReg:
				swap64(tu1, tu2);
				break;

			case instrSubYfromX:
				SWEET64processorFlags &= ~(SWEET64carryFlag);
//			case instrSubYfromXwithCarry:
				sbc64(tu1, tu2);
				break;

			case instrAddYtoX:
				SWEET64processorFlags &= ~(SWEET64carryFlag);
//			case instrAddYtoXwithCarry:
				adc64(tu1, tu2);
				break;

			case instrTraceOn: // this instruction is basically "nop" if #useSWEET64trace is not chosen
#ifdef useSWEET64trace
				SWEET64processorFlags |= (SWEET64traceCommandFlag);
#endif // useSWEET64trace
				break;

			case instrTraceSave: // this instruction is basically "nop" if #useSWEET64trace is not chosen
#ifdef useSWEET64trace
				traceSave = SWEET64processorFlags;
#endif // useSWEET64trace
			case instrTraceOff: // this instruction is basically "nop" if #useSWEET64trace is not chosen
#ifdef useSWEET64trace
				SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
				break;

			case instrTraceRestore: // this instruction is basically "nop" if #useSWEET64trace is not chosen
#ifdef useSWEET64trace
				if (traceSave & SWEET64traceFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
				else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
				break;

#ifdef useWindowTripFilter
			case instrStRegTripVar:
				if (operand < rvVSScycleIdx) tripArray[(unsigned int)(SWEET64_fetchbyte(sched))].collectedData[(unsigned int)(operand)] = tu2->ul[0];
				else copy64((union union_64 *)&tripArray[(unsigned int)(SWEET64_fetchbyte(sched))].collectedData[(unsigned int)(operand)], tu2);
				break;

#endif // useWindowTripFilter
			case instrMul2by1:
#ifdef useSWEET64mult
				prgmStack[(unsigned int)(spnt++)] = sched;
				if (spnt > 15) loopFlag = false;
				else sched = prgmMult64;
#ifdef useSWEET64trace
				traceSave = SWEET64processorFlags;
				SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
#else // useSWEET64mult
				mult64();
#endif // useSWEET64mult
				break;

			case instrDiv2by1:
#ifdef useSWEET64div
				prgmStack[(unsigned int)(spnt++)] = sched;
				if (spnt > 15) loopFlag = false;
				else sched = prgmDiv64;
#ifdef useSWEET64trace
				traceSave = SWEET64processorFlags;
				SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
#else // useSWEET64div
				div64();
#endif // useSWEET64div
				break;

#ifdef useAnalogRead
			case instrLdRegVoltage:
				init64(tu2, analogValue[(unsigned int)(tripIdx)]);
				break;

#endif // useAnalogRead
#ifdef useIsqrt
			case instrIsqrt:
				tu2->ui[0] = iSqrt(tu2->ui[0]);
				break;

#endif // useIsqrt
			default:
				loopFlag = false; // just found an unsupported opcode
				break;

		}

		if (branchFlag)
		{

			if (operand < 128) sched += operand;
			else sched -= (256 - operand);

		}

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugSerial, 13);
			text::charOut(devDebugSerial, 9);
			text::charOut(devDebugSerial, 9);
			text::hexByteOut(devDebugSerial, SWEET64processorFlags);
			text::charOut(devDebugSerial, 32);
			text::hexByteOut(devDebugSerial, branchFlag);
			text::charOut(devDebugSerial, 13);

			for (uint8_t x = 0;x < 5; x++)
			{

				tus = (union union_64 *)(&s64reg[(uint16_t)(x)]);

				text::charOut(devDebugSerial, 9);
				text::charOut(devDebugSerial, 9);
				text::charOut(devDebugSerial, 9);
				text::hexByteOut(devDebugSerial, (x + 1));
				text::charOut(devDebugSerial, 32);
				text::hexLWordOut(devDebugSerial, tus->ull);
				text::charOut(devDebugSerial, 13);

			}

			text::charOut(devDebugSerial, 13);

		}

#endif // useSWEET64trace
	}

	return ((union union_64 *)(&s64reg[(uint16_t)(s64reg2)]))->ul[0];

}

void tripVarLoad64(union union_64 * an, uint8_t tripIdx, uint8_t dataIdx)
{

	switch (dataIdx)
	{

		case rvVSSpulseIdx:
		case rvInjPulseIdx:
			init64(an, tripArray[(unsigned int)(tripIdx)].collectedData[(unsigned int)(dataIdx)]);
			break;

		case rvVSScycleIdx:
		case rvInjCycleIdx:
		case rvInjOpenCycleIdx:
			copy64(an, (union union_64 *)&tripArray[(unsigned int)(tripIdx)].collectedData[(unsigned int)(dataIdx)]);
			break;

		default:
			break;

	}

}

void init64(union union_64 * an, unsigned long dWordL)
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
	an->ull = 0;
	an->ul[0] = dWordL;
#endif // useAssemblyLanguage

}

void copy64(union union_64 * an, union union_64 * ann)
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

void swap64(union union_64 * an, union union_64 * ann) // swap ann and an
{

	unsigned int aing;

#ifdef useAssemblyLanguage
	asm volatile(
		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		"	ld 		%A2, %a0	\n"
		"	ld 		%B2, %a1	\n"
		"	st 		%a1+, %A2	\n"
		"	st 		%a0+, %B2	\n"

		: "+e" (ann), "+e" (an), "+r" (aing)
	);
#else // useAssemblyLanguage
	for (uint8_t x = 0; x < 4; x++)
	{

		aing = ann->ui[(unsigned int)(x)];
		ann->ui[(unsigned int)(x)] = an->ui[(unsigned int)(x)];
		an->ui[(unsigned int)(x)] = aing;

	}
#endif // useAssemblyLanguage

}

void shr64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t n;
#ifdef useAssemblyLanguage

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);

	asm volatile(
		"	ror	%A1					\n"		// shift input carry bit into avr carry

		"	ldd	__tmp_reg__, %a0+7	\n"		// 7
		"	ror	__tmp_reg__			\n"
		"	std	%a0+7, __tmp_reg__	\n"
		"	mov	%A3, __tmp_reg__	\n"		// save negative flag
		"	mov	%A2, __tmp_reg__	\n"		// initialize zero flag

		"	ldd	__tmp_reg__, %a0+6	\n"		// 6
		"	ror	__tmp_reg__			\n"
		"	std	%a0+6, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+5	\n"		// 5
		"	ror	__tmp_reg__			\n"
		"	std	%a0+5, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+4	\n"		// 4
		"	ror	__tmp_reg__			\n"
		"	std	%a0+4, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+3	\n"		// 3
		"	ror	__tmp_reg__			\n"
		"	std	%a0+3, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+2	\n"		// 2
		"	ror	__tmp_reg__			\n"
		"	std	%a0+2, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+1	\n"		// 1
		"	ror	__tmp_reg__			\n"
		"	std	%a0+1, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 0
		"	ror	__tmp_reg__			\n"
		"	st	%a0, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag
		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (n)
	);
#else // useAssemblyLanguage
	uint8_t b = ((SWEET64processorFlags & SWEET64carryFlag) ? 0x80 : 0x00);
	z = 0;

	for (uint8_t x = 7; x < 8; x--)
	{

		c = b;
		b = ((an->u8[(unsigned int)(x)] & 0x01) ? 0x80 : 0x00);
		n = an->u8[(unsigned int)(x)];
		n >>= 1;
		n |= c;
		z |= n;
		an->u8[(unsigned int)(x)] = n;

	}

	n = an->u8[7];
	c = b;
#endif // useAssemblyLanguage

	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
	else SWEET64processorFlags &= ~(SWEET64carryFlag);

}

void shl64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t n;
#ifdef useAssemblyLanguage

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);

	asm volatile(
		"	ror	%A1					\n"		// shift input carry bit into avr carry

		"	ld	__tmp_reg__, %a0	\n"		// 0
		"	rol	__tmp_reg__			\n"
		"	st	%a0, __tmp_reg__	\n"
		"	mov	%A2, __tmp_reg__	\n"		// initialize zero flag

		"	ldd	__tmp_reg__, %a0+1	\n"		// 1
		"	rol	__tmp_reg__			\n"
		"	std	%a0+1, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+2	\n"		// 2
		"	rol	__tmp_reg__			\n"
		"	std	%a0+2, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+3	\n"		// 3
		"	rol	__tmp_reg__			\n"
		"	std	%a0+3, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+4	\n"		// 4
		"	rol	__tmp_reg__			\n"
		"	std	%a0+4, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+5	\n"		// 5
		"	rol	__tmp_reg__			\n"
		"	std	%a0+5, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+6	\n"		// 6
		"	rol	__tmp_reg__			\n"
		"	std	%a0+6, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ldd	__tmp_reg__, %a0+7	\n"		// 7
		"	rol	__tmp_reg__			\n"
		"	std	%a0+7, __tmp_reg__	\n"
		"	mov	%A3, __tmp_reg__	\n"		// save negative flag
		"	or	%A2, __tmp_reg__	\n"		// update zero flag
		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (n)
	);
#else // useAssemblyLanguage
	uint8_t b = ((SWEET64processorFlags & SWEET64carryFlag) ? 0x01 : 0x00);
	z = 0;

	for (uint8_t x = 0; x < 8; x++)
	{

		c = b;
		b = ((an->u8[(unsigned int)(x)] & 0x80) ? 0x01 : 0x00);
		n = an->u8[(unsigned int)(x)];
		n <<= 1;
		n |= c;
		z |= n;
		an->u8[(unsigned int)(x)] = n;

	}

	c = b;
#endif // useAssemblyLanguage

	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
	else SWEET64processorFlags &= ~(SWEET64carryFlag);

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
void adc64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	uint8_t c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);
	uint8_t z;
	uint8_t n;

	asm volatile (
		"	ror	%A1					\n"		// shift input carry bit into avr carry

		"	ld	__tmp_reg__, %a0	\n"		// 0
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	mov	%A2, __tmp_reg__	\n"		// initialize zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 1
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 2
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 3
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 4
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 5
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 6
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 7
		"	ld	%A1, %a4+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	mov	%A3, __tmp_reg__	\n"		// save negative flag
		"	or	%A2, __tmp_reg__	\n"		// update zero flag
		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (n)
		: "e" (ann)
	);

	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
	else SWEET64processorFlags &= ~(SWEET64carryFlag);
#else // useAssemblyLanguage
	adc64(an, ann, 0x00);
#endif // useAssemblyLanguage

}

// SWEET64 64-bit subtract instruction
//
// subtracts ann from an, and stores the result in an
//
// if an < ann, 		z=0, c=1
// if an == ann,		z=1, c=0
// if an > ann,			z=0, c=0
// if (an - ann) < 0,	m=1
// if (an - ann) >= 0,	m = 0
void sbc64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	uint8_t c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);
	uint8_t z;
	uint8_t n;

	asm volatile (
		"	ror	%A1					\n"		// shift input carry bit into avr carry

		"	ld	__tmp_reg__, %a0	\n"		// 0
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	mov	%A2, __tmp_reg__	\n"		// initialize zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 1
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 2
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 3
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 4
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 5
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 6
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a0	\n"		// 7
		"	ld	%A1, %a4+			\n"
		"	sbc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	mov	%A3, __tmp_reg__	\n"		// save negative flag
		"	or	%A2, __tmp_reg__	\n"		// update zero flag
		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (n)
		: "e" (ann)
	);

	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
	else SWEET64processorFlags &= ~(SWEET64carryFlag);
#else // useAssemblyLanguage
	adc64(an, ann, 0x01);
#endif // useAssemblyLanguage

}

// SWEET64 64-bit compare instruction
//
// compares an to ann
// basicically an - ann -> discard
//
// if an < ann, 		z=0, c=1
// if an == ann,		z=1, c=0
// if an > ann,			z=0, c=0
// if (an - ann) < 0,	m=1
// if (an - ann) >= 0,	m = 0
void cmp64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	uint8_t c;
	uint8_t z;
	uint8_t n;

	asm volatile(
		"	ld	__tmp_reg__, %a3+	\n"		// 0
		"	ld	%A0, %a4+			\n"
		"	sub	__tmp_reg__, %A0	\n"		// we do not do a "compare with carry" like we can with adc64 or sbc64
		"	mov	%A1, __tmp_reg__	\n"		// initialize zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 1
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"
		"	or	%A1, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 2
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"
		"	or	%A1, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 3
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"
		"	or	%A1, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 4
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"
		"	or	%A1, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 5
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"
		"	or	%A1, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 6
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"
		"	or	%A1, __tmp_reg__	\n"		// update zero flag

		"	ld	__tmp_reg__, %a3+	\n"		// 7
		"	ld	%A0, %a4+			\n"
		"	sbc	__tmp_reg__, %A0	\n"

		"	mov	%A2, __tmp_reg__	\n"		// save negative flag
		"	or	%A1, __tmp_reg__	\n"		// update zero flag
		"	mov	%A0, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A0, __zero_reg__	\n"		// save carry flag
		: "+r" (c), "+r" (z), "+r" (n)
		: "e" (an), "e" (ann)
	);

	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
	else SWEET64processorFlags &= ~(SWEET64carryFlag);
#else // useAssemblyLanguage
	adc64(an, ann, 0x81);
#endif // useAssemblyLanguage

}

void registerTest64(union union_64 * an)
{

	uint8_t z;
	uint8_t n;

#ifdef useAssemblyLanguage
	asm volatile(
	"	ld	%A0, %a2+			\n"		// load output byte with first byte of 64-bit number to be checked
	"	ld	%A1, %a2+			\n"		// load temp register with second byte of 64-bit number to be checked
	"	or	%A0, %A1			\n"		// perform logical OR of output byte and temp register
	"	ld	%A1, %a2+			\n"		// load temp register with third byte of 64-bit number to be checked
	"	or	%A0, %A1			\n"		// perform logical OR of output byte and temp register
	"	ld	%A1, %a2+			\n"		// load temp register with fourth byte of 64-bit number to be checked
	"	or	%A0, %A1			\n"		// perform logical OR of output byte and temp register
	"	ld	%A1, %a2+			\n"		// load temp register with fifth byte of 64-bit number to be checked
	"	or	%A0, %A1			\n"		// perform logical OR of output byte and temp register
	"	ld	%A1, %a2+			\n"		// load temp register with sixth byte of 64-bit number to be checked
	"	or	%A0, %A1			\n"		// perform logical OR of output byte and temp register
	"	ld	%A1, %a2+			\n"		// load temp register with seventh byte of 64-bit number to be checked
	"	or	%A0, %A1			\n"		// perform logical OR of output byte and temp register
	: "+r" (z), "+r" (n)
	: "e" (an)
	);
#else // useAssemblyLanguage
	z = an->u8[0];

	for (uint8_t x = 1; x < 8; x++) z |= an->u8[(unsigned int)(x)];
	n = an->u8[7];
#endif // useAssemblyLanguage

	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

}

#ifndef useAssemblyLanguage
void adc64(union union_64 * an, union union_64 * ann, uint8_t mode)	// target addend in an, source addend in ann, result to an
{

	uint8_t c = ((mode & 0x01) ? 0xFF : 0x00);		// if mode[0] == 0, then addition, otherwise subtraction
	uint8_t s = ((mode & 0x80) ? 0x00 : 0xFF);		// if mode[7] == 0, then store result, otherwise just do compare
	uint8_t d;										// intermediate source addend byte
	uint8_t z = 0;
	unsigned int enn;								// intermediate result storage
	union union_16 * n = (union union_16 *)(&enn);	// byte-wise access into intermediate result storage

	d = c & 0x01;									// generate carry flag based on whether this operation is add or subtract
	d ^= ((SWEET64processorFlags & SWEET64carryFlag) ? 0x01 : 0x00);
	n->u8[1] = d;
	for (uint8_t x = 0; x < 8; x++)
	{

		d = c ^ ann->u8[(unsigned int)(x)];			// fetch (and complement, if subtract op) this operation's source addend byte
		n->ui = (unsigned int)(n->u8[1]);			// move previous operation's carry byte into position, and clear this operation's carry byte
		n->ui += (unsigned int)(an->u8[(unsigned int)(x)]);	// add previous operation's carry byte to this operation's target addend byte
		n->ui += (unsigned int)(d);					// add source addend byte to this operation's target addend byte (auto-generates carry byte for next operation)
		z |= n->u8[0];
		if (s) an->u8[(unsigned int)(x)] = n->u8[0];// save this operation's target addend byte

	}

	d = (n->u8[1] ^ c) & 0x01;

	if (n->u8[0] & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= (SWEET64zeroFlag);

	if (d) SWEET64processorFlags |= (SWEET64carryFlag);
	else SWEET64processorFlags &= ~(SWEET64carryFlag);

}

#endif // useAssemblyLanguage
#ifndef useSWEET64mult
void mult64(void)
{

	union union_64 * an = (union union_64 *)(&s64reg[s64reg2]);	// multiplier in an, result to an
	union union_64 * ann = (union union_64 *)(&s64reg[s64reg1]);	// multiplicand in ann

	union union_64 * multiplier = (union union_64 *)(&s64reg[s64reg4]);	// define multiplier term as register 4
	union union_64 * multiplicand = (union union_64 *)(&s64reg[s64reg5]);	// define multiplicand term as register 5

	copy64(multiplier, an);						// load multiplier into register 4
	copy64(multiplicand, ann);					// load multiplicand into register 5
	init64(an, 0);								// zero out result

	registerTest64(multiplier);

	while (!(SWEET64processorFlags & SWEET64zeroFlag))		// while multiplier is non-zero
	{

		SWEET64processorFlags &= ~(SWEET64carryFlag);
		shr64(multiplier);					// shift multiplier right by one bit

		if (SWEET64processorFlags & SWEET64carryFlag)
		{

			SWEET64processorFlags &= ~(SWEET64carryFlag);
			adc64(an, multiplicand);			// if the low bit of multiplier is set, add multiplicand to result

		}

		SWEET64processorFlags &= ~(SWEET64carryFlag);
		shl64(multiplicand);					// shift multiplicand left by one bit

		registerTest64(multiplier);

	}

}

#endif // useSWEET64mult
#ifndef useSWEET64div
// uses algorithm for non-restoring hardware division
void div64(void)
{

	union union_64 * an = (union union_64 *)(&s64reg[s64reg2]);	// dividend in an
	union union_64 * ann = (union union_64 *)(&s64reg[s64reg1]);	// divisor in ann

	union union_64 * divisor = (union union_64 *)(&s64reg[s64reg4]);
	uint8_t x;
	uint8_t y;
	uint8_t z;

	copy64(divisor, ann);					// copy ann value to divisor
	init64(ann, 0);							// zero out remainder

	registerTest64(divisor);

	if (SWEET64processorFlags & SWEET64zeroFlag)			// if divisor is zero, mark as overflow, then exit
	{

		SWEET64processorFlags |= (SWEET64carryFlag);		// set carry bit
		sbc64(ann, ann);					// subtract 1 from zeroed-out result to generate overflow value
		copy64(an, ann);					// copy overflow value to remainder

	}
	else
	{

		x = 64;							// start off with a dividend size of 64 bits
		SWEET64processorFlags |= (SWEET64divisionFlag);		// mark

		y = 7;
		while ((x) && (an->u8[(unsigned int)(y)] == 0))		// examine dividend for leading zero bytes
		{

			y--;						// if this byte is zero, skip to look at next byte
			x -= 8;						// shrink dividend size by 8 bits

		}

		if ((x) && (y < 7))
		{

			for (z = 7; z < 8; z--)
			{

				if (y < 7) an->u8[(unsigned int)(z)] = an->u8[(unsigned int)(y)];
				else an->u8[(unsigned int)(z)] = 0;
				y--;

			}

		}

		while (x) // while there are still bits in dividend to process
		{

			SWEET64processorFlags &= ~(SWEET64carryFlag);	// perform shift left of dividend/quotient, through remainder (A)
			shl64(an);					// shift Q left one bit
			shl64(ann);					// shift A left one bit

			SWEET64processorFlags &= ~(SWEET64carryFlag);	// clear carry flag for impending operation
			if (SWEET64processorFlags & SWEET64divisionFlag) // if previous operation resulted in A >= 0
			{

				sbc64(ann, divisor); // subtract M from A if previous A was greater than or equal to zero
				if ((SWEET64processorFlags & SWEET64carryFlag) && ((SWEET64processorFlags & SWEET64zeroFlag) == 0))
				{

					SWEET64processorFlags &= ~(SWEET64divisionFlag); // if A < 0

				}
				else
				{

					an->u8[0] |= 0x01; // if A >= 0

				}

			}
			else // otherwise, previous operation resulted in A < 0
			{

				adc64(ann, divisor); // add M to A if previous A was less than zero
				if (SWEET64processorFlags & (SWEET64carryFlag | SWEET64zeroFlag))
				{

					SWEET64processorFlags |= (SWEET64divisionFlag); // if A >= 0
					an->u8[0] |= 0x01;

				}

			}

			x--; // reduce dividend bit count by one

		}

		if (!(SWEET64processorFlags & SWEET64divisionFlag)) // if most recent operation resulted in A < 0
		{

			SWEET64processorFlags &= ~(SWEET64carryFlag);
			adc64(ann, divisor); // make A (remainder) positive by adding M to it

		}

		SWEET64processorFlags &= ~(SWEET64carryFlag);
		shl64(ann);			// multiply remainder by 2 by shifting left one bit

		cmp64(ann, divisor);

		if (SWEET64processorFlags & SWEET64carryFlag) init64(divisor, 1); // if remainder * 2 is not less than divisor
		else init64(divisor, 0);	// otherwise, store a zero

		shr64(ann);			// restore remainder

	}

}

#endif // useSWEET64div
void initStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = 0; // end hold delay countdown
	timer0Command &= ~t0cDisplayDelay; // cancel display delay

	SREG = oldSREG; // restore interrupt flag status

	text::gotoXY(devLCD, 0, 0); // go to the first line

}

void execStatusLine(void)
{

	uint8_t oldSREG;

	text::clearEOL(devLCD); // clear the rest of the status message line
#ifdef blankScreenOnMessage
	text::gotoXY(devLCD, 0, 1); // clear the second line
	text::clearEOL(devLCD);
#endif // blankScreenOnMessage

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = holdDelay; // start hold delay countdown
	timer0Command |= t0cDisplayDelay; // make display delay active

	SREG = oldSREG; // restore interrupt flag status

}

const char * findStr(const char * str, uint8_t strIdx)
{

	while (strIdx)
	{

		while (pgm_read_byte(str++));
		strIdx--;

	}

	return str;

}

void text::gotoXY(outputDevice &dev, uint8_t xPos, uint8_t yPos)
{

	dev.xPos = xPos;
	dev.yPos = yPos;

	if (dev.gotoXY) dev.gotoXY(xPos, yPos);

}

uint8_t text::charOut(outputDevice &dev, uint8_t chr)
{

	switch (chr)
	{

		case 0x00:
			return 0;

		case 0xFB:
			if (metricFlag & metricMode) dev.controlFlags &= ~(odvFlagEnableOutput);
			else dev.controlFlags |= odvFlagEnableOutput;
			break;

		case 0xFC:
			dev.controlFlags ^= odvFlagEnableOutput;
			break;

		case 0xFD:
			dev.controlFlags |= odvFlagEnableOutput;
			break;

		default:
			if (dev.controlFlags & odvFlagEnableOutput)
			{

#ifdef blankScreenOnMessage
				if (((dev.controlFlags & odvFlagSupportsDelay) == 0) || ((timer0Command & t0cDisplayDelay) == 0))
#else // blankScreenOnMessage
				if (((dev.controlFlags & odvFlagSupportsDelay) == 0) || ((timer0Command & t0cDisplayDelay) == 0) || (dev.yPos > 0))
#endif // blankScreenOnMessage
				{

					if (dev.controlFlags & odvFlagShootBlanks) chr = ' ';
					else chr &= 0x7F;

					if (dev.charOut)
					{

						dev.charOut(chr);
						dev.xPos++;

						if ((chr == 0x0D) && (dev.controlFlags & odvFlagCRLF)) dev.charOut(0x0A);

					}

				}

			}

			break;

	}

	return 1;

}

void text::blinkMode(outputDevice &dev, uint8_t condition)
{

	if (condition) dev.controlFlags |= odvFlagShootBlanks; // if condition is true, output blanks instead of characters
	else dev.controlFlags &= ~(odvFlagShootBlanks); // otherwise, output characters normally

}

void text::stringOut(outputDevice &dev, char * str)
{

	dev.controlFlags |= odvFlagEnableOutput;
	while (charOut(dev, * str++));

}

void text::stringOut(outputDevice &dev, const char * str)
{

	stringOut(dev, str, 0);

}

void text::stringOut(outputDevice &dev, const char * str, uint8_t strIdx)
{

	str = findStr(str, strIdx);

	dev.controlFlags |= odvFlagEnableOutput; // enable text output, disable blanking output
	while (charOut(dev, pgm_read_byte(str++))) ;

}

void text::clearEOL(outputDevice &dev)
{

	while ((dev.xPos < dev.xMax) && (dev.controlFlags & odvFlagEnableOutput)) charOut(dev, ' ');

}

void text::hexNybbleOut(outputDevice &dev, uint8_t val)
{

	val &= 0x0F;
	val |= 0x30;
	if (val > 0x39) val += 0x07;
	charOut(dev, val);

}

void text::hexByteOut(outputDevice &dev, uint8_t val)
{

	hexNybbleOut(dev, val >> 4);
	hexNybbleOut(dev, val);

}

void text::hexWordOut(outputDevice &dev, unsigned int val)
{

	union union_16 * vee = (union union_16 *) &val;

	for (uint8_t i = 1; i < 2; i--) hexByteOut(dev, vee->u8[i]);

}

void text::hexDWordOut(outputDevice &dev, unsigned long val)
{

	union union_32 * vee = (union union_32 *) &val;

	for (uint8_t i = 3; i < 4; i--) hexByteOut(dev, vee->u8[i]);

}

void text::hexLWordOut(outputDevice &dev, unsigned long long val)
{

	union union_64 * vee = (union union_64 *) &val;

	for (uint8_t i = 7; i < 8; i--) hexByteOut(dev, vee->u8[i]);

}

#define nextAllowedValue 0 // S64programList
const uint8_t calcFormatTimeHHmmSSIdx =				nextAllowedValue;						// time in HHmmSS format
const uint8_t calcFormatTimeInMicrosecondsIdx =		calcFormatTimeHHmmSSIdx + 1;			// time in microseconds
const uint8_t calcFormatEngineSpeedIdx =			calcFormatTimeInMicrosecondsIdx + 1;	// engine speed
const uint8_t calcFormatPulseCountIdx =				calcFormatEngineSpeedIdx + 1;			// pulse count
#define nextAllowedValue calcFormatPulseCountIdx + 1
#ifdef useFuelCost
const uint8_t calcFormatFuelCostIdx =				nextAllowedValue;						// fuel cost
const uint8_t calcFormatFuelRateCostIdx =			calcFormatFuelCostIdx + 1;				// fuel rate cost
#define nextAllowedValue calcFormatFuelRateCostIdx + 1
#endif // useFuelCost
#ifdef useAnalogRead
const uint8_t calcFormatAnalogReadIdx =				nextAllowedValue;						// voltage
#define nextAllowedValue calcFormatAnalogReadIdx + 1
#endif // useAnalogRead
#ifdef useDragRaceFunction
const uint8_t calcFormatTimeInTenthsSecIdx =		nextAllowedValue;						// time in tenths of seconds
#define nextAllowedValue calcFormatTimeInTenthsSecIdx + 1
#endif // useDragRaceFunction
#ifdef useDebugValueReadout
const uint8_t calcFormatDebugValue =				nextAllowedValue;						// selected debug value
#define nextAllowedValue calcFormatDebugValue + 1
#endif // useDebugValueReadout
const uint8_t calcFormatFuelQuantityIdx =			nextAllowedValue;						// fuel quantity (SAE/SI)
const uint8_t calcFormatFuelRateIdx =				calcFormatFuelQuantityIdx + 2;			// fuel rate (SAE/SI)
const uint8_t calcFormatDistanceIdx =				calcFormatFuelRateIdx + 2;				// distance travelled (SAE/SI)
const uint8_t calcFormatSpeedIdx =					calcFormatDistanceIdx + 2;				// speed (SAE/SI)
const uint8_t calcFormatFuelEconomyIdx =			calcFormatSpeedIdx + 2;					// fuel economy (SAE/SI)
const uint8_t calcFormatAltFuelEconomyIdx =			calcFormatFuelEconomyIdx + 2;			// alternate fuel economy (SAE/SI)
#define nextAllowedValue calcFormatAltFuelEconomyIdx + 2
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

const uint8_t calcFormatIdxCount =					nextAllowedValue;

// 0x80 is trip label display flag - flag set means do not display trip label
const uint8_t calcFormatIdx[(unsigned int)(dfMaxValDisplayCount)] PROGMEM = { // S64programList
	 calcFormatTimeHHmmSSIdx					// tEngineRunTime - engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tRangeTime - estimated total runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tMotionTime - time vehicle in motion (hhmmss)

	,calcFormatTimeInMicrosecondsIdx			// tInjectorOpenTime - fuel used (microseconds)
	,calcFormatTimeInMicrosecondsIdx			// tInjectorTotalTime - engine run time (microseconds)
	,calcFormatTimeInMicrosecondsIdx			// tVSStotalTime - time vehicle in motion (microseconds)
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
#ifdef useAnalogRead
	,calcFormatAnalogReadIdx					// tAnalogChannel - DC voltage
#endif // useAnalogRead
#ifdef useCarVoltageOutput
	,calcFormatAnalogReadIdx					// tAlternatorChannel - DC voltage
	,calcFormatAnalogReadIdx					// tSpareVoltageChannel - DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	,calcFormatTimeInTenthsSecIdx				// tAccelTestTime - acceleration test time (s.s)
#endif // useDragRaceFunction
#ifdef useDebugValueReadout
	,calcFormatDebugValue						// tShowDebugValue - internal volatile variable value
#endif // useDebugValueReadout
	,calcFormatFuelQuantityIdx					// tFuelUsed - fuel quantity used (SI/SAE)
	,calcFormatFuelRateIdx						// tFuelRate - fuel consumption rate (SI/SAE)
	,calcFormatDistanceIdx						// tDistance - vehicle distance traveled (SI/SAE)
	,calcFormatSpeedIdx							// tSpeed - vehicle speed (SI/SAE)
	,calcFormatFuelEconomyIdx					// tFuelEcon - fuel economy (SI/SAE)
	,calcFormatAltFuelEconomyIdx				// tAlternateFuelEcon - alternate format fuel economy (SI/SAE)
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
};

const uint8_t calcFormatDecimalPoints[(unsigned int)(calcFormatIdxCount)] PROGMEM = { // S64programList
	 0	// time in HHmmSS format
	,0	// time in microseconds
	,0	// engine speed
	,0	// pulse count
#ifdef useFuelCost
	,2	// fuel cost
	,2	// fuel rate cost
#endif // useFuelCost
#ifdef useAnalogRead
	,3	// voltage
#endif // useAnalogRead
#ifdef useDragRaceFunction
	,1	// time in tenths of seconds
#endif // useDragRaceFunction
#ifdef useDebugValueReadout
	,3	// selected debug value
#endif // useDebugValueReadout
	,2	// SAE fuel quantity
	,2	// SI fuel quantity
	,2	// SAE fuel rate
	,2	// SI fuel rate
	,1	// SAE distance travelled
	,1	// SI distance travelled
	,1	// SAE speed
	,1	// SI speed
	,2	// SAE fuel economy
	,2	// SI fuel economy
	,2	// alternate SAE fuel economy
	,2	// alternate SI fuel economy
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
};

const uint8_t calcFormatLabelText[(unsigned int)(calcFormatIdxCount)] PROGMEM = { // S64programList
	 's'	// time in HHmmSS format
	,'u'	// time in microseconds
	,'t'	// engine speed
	,'p'	// pulse count
#ifdef useFuelCost
	,'$'	// fuel cost
	,'#'	// fuel rate cost
#endif // useFuelCost
#ifdef useAnalogRead
	,'V'	// voltage
#endif // useAnalogRead
#ifdef useDragRaceFunction
	,'s'	// time in tenths of seconds
#endif // useDragRaceFunction
#ifdef useDebugValueReadout
	,' '	// selected debug value
#endif // useDebugValueReadout
	,'G'	// SAE fuel used
	,'L'	// SI fuel used
	,'g'	// SAE fuel rate
	,'l'	// SI rate
	,'m'	// SAE distance travelled
	,'k'	// SI distance travelled
	,'S'	// SAE speed
	,'S'	// SI speed
	,'E'	// SAE fuel economy
	,'E'	// SI fuel economy
	,'E'	// alternate SAE fuel economy
	,'E'	// alternate SI fuel economy
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
};

const uint8_t calcFormatLabelCGRAM[(unsigned int)(calcFormatIdxCount)][16] PROGMEM = { // S64programList
	// time in HHmmSS format
	 {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}

	// time in microseconds
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
#ifdef useAnalogRead

	// voltage
	,{0b00010001, 0b00010001, 0b00010001, 0b00001010, 0b00000100, 0b00000000, 0b00000000, 0b00000000
	, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011010, 0b00011010, 0b00010001}
#endif // useAnalogRead
#ifdef useDragRaceFunction

	// time in tenths of seconds
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}
#endif // useDragRaceFunction
#ifdef useDebugValueReadout

	// selected debug value
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00001010, 0b00000100, 0b00000000
	, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}
#endif // useDebugValueReadout

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
};

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

static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx)
{

	calcFuncObj thisCalcFuncObj;
	uint8_t calcFmtIdx;

	thisCalcFuncObj.translateFlag = calcIdx & 0x80; // save trip index translation bit

	if (calcIdx & 0x80)
	{

		thisCalcFuncObj.localTripIdx = tripIdx;
		calcIdx &= 0x7F;

	}
	else
	{

		thisCalcFuncObj.localTripIdx = (tripIdx & dfTripMask) >> dfBitShift; // extract local trip index
		tripIdx = (tripIdx & dfFunctionMask);  // extract calculation trip index

	}

	if (activityFlags & afSwapFEwithFCR) // do fuel consumption rate swap with fuel economy here
	{

		if ((tripIdx == instantIdx) && ((calcIdx == tFuelEcon) || (calcIdx == tAlternateFuelEcon))) calcIdx = tFuelRate;

	}
	else if (metricFlag & alternateFEmode) // do alternate fuel economy format swap here
	{

		switch (calcIdx)
		{

			case tFuelEcon:
				calcIdx = tAlternateFuelEcon;
				break;

			case tAlternateFuelEcon:
				calcIdx = tFuelEcon;
				break;

			default:
				break;

		}

	}

	thisCalcFuncObj.tripIdx = tripIdx;
	thisCalcFuncObj.calcIdx = calcIdx;

	calcFmtIdx = pgm_read_byte(&calcFormatIdx[(unsigned int)(calcIdx)]);
	if ((calcFmtIdx >= dfMaxValNonConversion) && (metricFlag & metricMode)) calcFmtIdx++; // shift index up one if this is an SI/SAE format

	thisCalcFuncObj.calcFmtIdx = calcFmtIdx;
	thisCalcFuncObj.decimalPlaces = pgm_read_byte(&calcFormatDecimalPoints[(unsigned int)(calcFmtIdx)]);
	thisCalcFuncObj.calcChar = pgm_read_byte(&calcFormatLabelText[(unsigned int)(calcFmtIdx)]);

	return thisCalcFuncObj;

}

static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
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

		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, localTripBitmask);

	}

}

static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
{

	calcFuncObj thisCalcFuncObj;

	uint8_t tripPart;
	uint8_t calcPart;
	uint8_t tripBitmask;
	uint8_t calcBitmask;
	uint8_t x;
	uint8_t y;

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx);

	readingIdx &= 3;
	x = (readingIdx & 1) << 3;
	y = (readingIdx & 2) >> 1;
	readingIdx <<= 1;

	text::gotoXY(devLCD, x, y);
	text::stringOut(devLCD, doFormat(thisCalcFuncObj, 6, 0));
	text::charOut(devLCD, 0x80 + readingIdx);
	text::charOut(devLCD, 0x81 + readingIdx);

	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function label component should blink or not

	for (uint8_t x = 0; x < 16; x++)
	{

		calcPart = pgm_read_byte(&calcFormatLabelCGRAM[(unsigned int)(thisCalcFuncObj.calcFmtIdx)][(unsigned int)(x)]); // read a byte of function label bit pattern

		if (thisCalcFuncObj.translateFlag) tripPart = 0;
		else
		{

			tripPart = (calcPart >> 5) & 0x03; // fetch partial address of trip label component
			tripPart = pgm_read_byte(&localTripBitmask[(unsigned int)(thisCalcFuncObj.localTripIdx)][(unsigned int)(tripPart)]); // read a byte of trip label bit pattern
			tripPart &= tripBitmask; // provide for blinking trip label component

		}

		calcPart &= calcBitmask; // provide for blinking function label component

		mBuff1[(unsigned int)(x)] = calcPart | tripPart; // combine trip label and function label components

	}

	LCD::loadCGRAMcharacter(readingIdx, (char *)(mBuff1)); // write out generated CGRAM character
	LCD::loadCGRAMcharacter(readingIdx + 1, (char *)(mBuff1 + 8)); // write out generated CGRAM character

}

#ifdef useClockDisplay
 /* Big Clock Display support section */

static const uint8_t prgmChangeSoftwareClock[] PROGMEM = {
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// convert datetime value from cycles to seconds
	instrDiv2by1,

	instrLdRegConst, 0x01, idxSecondsPerDay,			// divide by number of seconds in a day, to remove the existing time portion from the datetime value
	instrDiv2by1,

	instrLdRegByte, 0x01, 24,							// multiply datetime value by 24 (hours per day)
	instrMul2by1,
	instrLxdI, 0,										// add user-defined hours value to datetime value
	instrLdRegByteFromYindexed, 0x31,
	instrAddYtoX, 0x21,

	instrLdRegByte, 0x01, 60,							// multply datetime value by 60 (minutes per hour)
	instrMul2by1,
	instrLxdI, 2,										// add user-defined minutes value to datetime value
	instrLdRegByteFromYindexed, 0x31,
	instrAddYtoX, 0x21,

	instrLdRegByte, 0x01, 60,							// multiply datetime value by 60 (seconds per minute)
	instrMul2by1,
	instrLxdI, 4,										// add user-defined seconds value to datetime value
	instrLdRegByteFromYindexed, 0x31,
	instrAddYtoX, 0x21,

	instrLdRegConst, 0x01, idxCycles0PerSecond,			// convert datetime value from seconds to cycles
	instrMul2by1,

	instrDone
};

static void clockDisplay::goSetClock(void)
{

#ifdef useSoftwareClock
	convertTime((uint32_t *)(&clockCycles)); // perform initialization for ull2str
#endif // useSoftwareClock
	ull2str(prgmFormatToTime, pBuff, 3); // convert system time from ticks into seconds, and format for output
	cursor::moveAbsolute(clockDisplayEditScreenIdx, 0);

}

static void clockDisplay::changeDigitUp(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(clockDisplayEditScreenIdx)])]++;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(clockDisplayEditScreenIdx)])] > '9') pBuff[(unsigned int)(screenCursor[(unsigned int)(clockDisplayEditScreenIdx)])] = '0';

	if (pBuff[2] > '5') pBuff[2] = '0'; // this will only happen if clockDisplayEditScreenIdx == 2
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '0'; // this will only happen if clockDisplayEditScreenIdx == 0 or 1
	if (pBuff[0] > '2') pBuff[0] = '0'; // this will only happen if clockDisplayEditScreenIdx == 0

}

static void clockDisplay::changeDigitDown(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(clockDisplayEditScreenIdx)])]--;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(clockDisplayEditScreenIdx)])] < '0') pBuff[(unsigned int)(screenCursor[(unsigned int)(clockDisplayEditScreenIdx)])] = '9';

	if (pBuff[0] > '2') pBuff[0] = '2'; // this will only happen if clockDisplayEditScreenIdx == 0
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '3'; // this will only happen if clockDisplayEditScreenIdx == 0 or 1
	if (pBuff[2] > '5') pBuff[2] = '5'; // this will only happen if clockDisplayEditScreenIdx == 2

}

static void clockDisplay::setClock(void)
{

#ifdef useSoftwareClock
	uint8_t b;
	uint8_t oldSREG;

	pBuff[4] = '0'; // set seconds to zero
	pBuff[5] = '0';

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	copy64((union union_64 *)(&s64reg[s64reg2]), (union union_64 *)(&clockCycles)); // transfer clock value to SWEET64 register 2

	SREG = oldSREG; // restore state of interrupt flag

	for (uint8_t x = 4; x < 6; x -= 2) // convert time string in pBuff into time value usable by prgmChangeSoftwareClock
	{

		b = pBuff[(unsigned int)(x)] - '0';
		b *= 10;
		b += pBuff[(unsigned int)(x + 1)] - '0';
		((union union_64 *)(&s64reg[s64reg3]))->u8[(unsigned int)(x)] = b;

	}

	SWEET64(prgmChangeSoftwareClock, 0); // convert time value into timer2 clock cycles

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	copy64((union union_64 *)(&clockCycles), (union union_64 *)(&s64reg[s64reg2])); // transfer SWEET64 register 2 value back to clock

	SREG = oldSREG; // restore state of interrupt flag

	cursor::screenLevelEntry(PSTR("Time Set"), clockDisplayShowScreenIdx);

#endif // useSoftwareClock
}

static void clockDisplay::cancelClockSet(void)
{

	cursor::screenLevelEntry(PSTR("Time NOT Set"), clockDisplayShowScreenIdx);

}

#endif // useClockDisplay
#ifdef useBigDTE
/* Big Distance-To-Empty display support section */

const uint8_t prgmRoundOffDTE[] PROGMEM = {
	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrAddYtoX, 0x12,									// add register 2 results to register 1
	instrBranchIfZero, 36,								// if register 1 is zero, register 2 has overflow value - just go output

	instrLdRegConst, 0x01, idxNumber4nines,
	instrCmpReg, 0x21,
	instrBranchIfLTorE, 29,								// if number is less then 10, just go output
	instrLdRegConst, 0x01, idxNumber5nines,
	instrCmpReg, 0x21,
	instrBranchIfLTorE, 17,								// if number is less then 100, go round off to nearest 1/100th
	instrLdRegConst, 0x01, idxNumber6nines,
	instrCmpReg, 0x21,
	instrBranchIfLTorE, 5,								// if number is less then 1000, go round off to nearest 1/10th

	instrLdRegConst, 0x01, idxNumber500,				// round off to nearest whole integer
	instrSkip, 8,										// skip to add
	instrLdRegByte, 0x01, 50,							// round off to nearest 1/10th
	instrSkip, 3,										// skip to add
	instrLdRegByte, 0x01, 5,							// round off to nearest 1/100th

	instrAddYtoX, 0x21,									// perform round to nearest number

	instrJump, tFormatToNumber							// go call prgmFormatToNumber to perform actual formatting
};

#endif // useBigDTE
#ifdef useBigFE
/* Big Fuel Economy display support section */

const uint8_t prgmRoundOffFE[] PROGMEM = {
	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrAddYtoX, 0x12,									// add register 2 results to register 1
	instrBranchIfZero, 29,								// if register 1 is zero, register 2 has overflow value

	instrLdRegConst, 0x01, idxNumber4nines,
	instrCmpReg, 0x21,
	instrBranchIfLTorE, 17,								// if number is less then 10, go round off to nearest 1/100th
	instrLdRegConst, 0x01, idxNumber5nines,
	instrCmpReg, 0x21,
	instrBranchIfLTorE, 5,								// if number is less then 100, go round off to nearest 1/10th

	instrLdRegConst, 0x01, idxNumber500,				// round off to nearest whole integer
	instrSkip, 8,										// skip to add
	instrLdRegByte, 0x01, 50,							// round off to nearest 1/10th
	instrSkip, 3,										// skip to add
	instrLdRegByte, 0x01, 5,							// round off to nearest 1/100th

	instrAddYtoX, 0x21,									// perform round to nearest number

	instrJump, tFormatToNumber							// go call prgmFormatToNumber to perform actual formatting
};

#endif // useBigFE
#ifdef useBigNumberDisplay
/* Big Number Output support section */

#ifdef useSpiffyBigChars
static const char bigNumChars1[] PROGMEM = {
	0x86, 0x80, 0x87, 0,
	0x80, 0x84, 0x20, 0,
	0x82, 0x82, 0x87, 0,
	0x80, 0x82, 0x87, 0,
	0x84, 0x81, 0x84, 0,
	0x84, 0x82, 0x82, 0,
	0x86, 0x82, 0x82, 0,
	0x80, 0x80, 0x85, 0,
	0x86, 0x82, 0x87, 0,
	0x86, 0x82, 0x87, 0,
	0x20, 0x20, 0x20, 0,
	0x81, 0x81, 0x81, 0
};

static const char bigNumChars2[] PROGMEM = {
	0x83, 0x81, 0x85, 0,
	0x20, 0x84, 0x20, 0,
	0x84, 0x81, 0x81, 0,
	0x81, 0x81, 0x85, 0,
	0x20, 0x20, 0x84, 0,
	0x81, 0x81, 0x85, 0,
	0x83, 0x81, 0x85, 0,
	0x20, 0x86, 0x20, 0,
	0x83, 0x81, 0x85, 0,
	0x81, 0x81, 0x85, 0,
	0x20, 0x20, 0x20, 0,
	0x20, 0x20, 0x20, 0
};

static const char bigNumFont[] PROGMEM = {
	8, // number of characters in font

	0b00011111, // char 0x80
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,

	0b00000000, // char 0x81
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0x82
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0x83
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00001111,
	0b00000111,

	0b00011111, // char 0x84
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,

	0b00011111, // char 0x85
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011110,
	0b00011100,

	0b00000111, // char 0x86
	0b00001111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,

	0b00011100, // char 0x87
	0b00011110,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111
};

#else // useSpiffyBigChars
static const char bigNumChars1[] PROGMEM = {
	0x83, 0x80, 0x83, 0,
	0x80, 0x83, 0x20, 0,
	0x82, 0x82, 0x83, 0,
	0x80, 0x82, 0x83, 0,
	0x83, 0x81, 0x83, 0,
	0x83, 0x82, 0x82, 0,
	0x83, 0x82, 0x82, 0,
	0x80, 0x80, 0x83, 0,
	0x83, 0x82, 0x83, 0,
	0x83, 0x82, 0x83, 0,
	0x20, 0x20, 0x20, 0,
	0x81, 0x81, 0x81, 0
};

static const char bigNumChars2[] PROGMEM = {
	0x83, 0x81, 0x83, 0,
	0x81, 0x83, 0x81, 0,
	0x83, 0x81, 0x81, 0,
	0x81, 0x81, 0x83, 0,
	0x20, 0x20, 0x83, 0,
	0x81, 0x81, 0x83, 0,
	0x83, 0x81, 0x83, 0,
	0x20, 0x83, 0x20, 0,
	0x83, 0x81, 0x83, 0,
	0x81, 0x81, 0x83, 0,
	0x20, 0x20, 0x20, 0,
	0x20, 0x20, 0x20, 0
};

static const char bigNumFont[] PROGMEM = {
	4, // number of characters in font

	0b00011111, // char 0x80
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,

	0b00000000, // char 0x81
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0x82
	0b00011111,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011111,
	0b00011111,

	0b00011111, // char 0x83
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
};

#endif // useSpiffyBigChars
static uint8_t bigNumber::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;
	uint8_t thisMenuLevel = menuLevel;
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
	uint8_t tripIdx = pgm_read_byte(&msTripList[(unsigned int)(cursorPos)]);
	char * str;
#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)

	switch (thisMenuLevel)
	{

#ifdef useBigDTE
		case bigDTEscreenIdx:
			str = PSTR(" DistToEmpty");
			break;

#endif // useBigDTE
#ifdef useBigFE
		case bigFEscreenIdx:
			str = PSTR(" Fuel Econ");
			break;

#endif // useBigFE
#ifdef useBigTTE
		case bigTTEscreenIdx:
			str = PSTR(" TimeToEmpty");
			break;

#endif // useBigTTE
		default:
#ifdef useClockDisplay
			if (cursorPos == 255) thisMenuLevel = clockDisplayShowScreenIdx;
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

#ifdef useBigDTE
				case bigDTEscreenIdx:
#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
#endif // useBigTTE
#ifdef useClockDisplay
				case clockDisplayShowScreenIdx:
				case clockDisplayEditScreenIdx:
#endif // useClockDisplay
					bigNumber::loadCGRAMnumberFont();
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
				case clockDisplayShowScreenIdx:
					printStatusMessage(PSTR("Clock"));
					break;

				case clockDisplayEditScreenIdx:
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
					outputNumber(prgmRoundOffDTE, tripIdx, tDistanceToEmpty, 4, 3);
					break;

#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
					outputNumber(prgmRoundOffFE, tripIdx, tFuelEcon, 3, 2);

					text::stringOut(devLCD, msTripNameString, cursorPos);
					text::gotoXY(devLCD, 12, 1);
					text::stringOut(devLCD, PSTR("\xfbMPG \xfcL100\xfd"));
					break;

#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
					SWEET64(prgmTimeToEmpty, tripIdx);
					outputTime(ull2str(prgmFormatToTime, mBuff1, 3), (mainLoopHeartBeat & 0b01010101), 4);
					break;

#endif // useBigTTE
#ifdef useClockDisplay
				case clockDisplayShowScreenIdx:
#ifdef useSoftwareClock
					convertTime((uint32_t *)(&clockCycles)); // perform initialization for ull2str
#endif // useSoftwareClock
					outputTime(ull2str(prgmFormatToTime, mBuff1, 3), (mainLoopHeartBeat & 0b01010101), 4);
					break;

				case clockDisplayEditScreenIdx:
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

static void bigNumber::loadCGRAMnumberFont(void)
{

	uint8_t numChars;
	const char * fontPtr;

	fontPtr = bigNumFont;
	numChars = pgm_read_byte(fontPtr++); // get the number of characters in the font

	for (uint8_t chr = 0; chr < numChars; chr++)
	{

		for (uint8_t x = 0; x < 8; x++) mBuff1[(unsigned int)(x)] = pgm_read_byte(fontPtr++); // copy the CGRAM character data into RAM

		LCD::loadCGRAMcharacter(chr, (char *)(mBuff1)); // go perform CGRAM character loading

	}

}

#ifdef useBigTimeDisplay
static void bigNumber::outputTime(char * val, uint8_t blinkFlag, uint8_t blinkPos)
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
static void bigNumber::outputNumber(const uint8_t * prgmPtr, uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalPlaces)
{

	doCalculate(tripIdx, calcIdx); // perform the actual calculation
	outputNumberString(formatDecimal(prgmPtr, mBuff1, windowLength, decimalPlaces, dfAdjustWindow)); // perform the required decimal formatting, and output the number

}

static void bigNumber::outputNumberString(char * str)
{

	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t x = devLCD.xPos;

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

		outputDigit(bigNumChars1, x, 0, c, e);
		outputDigit(bigNumChars2, x, 1, c, d);
		x += 4;

	}

	text::gotoXY(devLCD, x, 0);

}

static void bigNumber::outputDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar)
{

	text::gotoXY(devLCD, xPos, yPos);
	text::stringOut(devLCD, digitDefStr, strIdx);
	text::charOut(devLCD, endChar);

}

static void bigNumber::displayStatus(const char * str, uint8_t cursorPos)
{

	initStatusLine();
	text::stringOut(devLCD, msTripNameString, cursorPos);
	text::stringOut(devLCD, str); // briefly display screen name
	execStatusLine();

}

#endif // useBigNumberDisplay
#ifdef useBarGraph // Bar Graph Output support section
uint8_t bgPlotArea[16];
unsigned long barGraphData[(unsigned int)(bgDataSize)];

void clearBGplot(uint8_t yIdx)
{

	for (uint8_t x = 0; x < 16; x++) bgPlotArea[(unsigned int)(x)] = 0;
	if (yIdx < 16) bgPlotArea[(unsigned int)(15 - yIdx)] = 31;

}

uint8_t bgPlotConvert(uint8_t coord)
{

	if (coord == 254) coord = 15;
	else if (coord > 15) coord = 0;
	else coord = 15 - coord;

	return coord;

}

void bgPlot(uint8_t idx, uint8_t lowerPoint, uint8_t upperPoint, uint8_t mode)
{

	uint8_t i = idx + 3;
	uint8_t k = i % 5;

	lowerPoint = bgPlotConvert(lowerPoint);
	upperPoint = bgPlotConvert(upperPoint);

	if (lowerPoint < upperPoint)
	{

		lowerPoint ^= upperPoint;
		upperPoint ^= lowerPoint;
		lowerPoint ^= upperPoint;

	}

	uint8_t bitMask = (1 << (4 - k));

	while ((lowerPoint >= upperPoint) && (lowerPoint < 16))
	{

		if ((mode) && (mainLoopHeartBeat & 0b01010101)) bgPlotArea[(unsigned int)(lowerPoint)] ^= bitMask;
		else bgPlotArea[(unsigned int)(lowerPoint)] |= bitMask;
		lowerPoint--;

	}

}

void bgOutputPlot(uint8_t idx, uint8_t yIdx)
{

	uint8_t i = idx + 3;
	uint8_t j = i / 5;
	uint8_t k = i % 5;

	if ((i == 3) || (k == 0))
	{

		if (i == 3)
		{

			for (uint8_t x = 0; x < 16; x++) bgPlotArea[(unsigned int)(x)] |= 16;

			for (uint8_t x = ((yIdx < 16) ? ((15 - yIdx) & 0x03): 3); x < 16; x += 4) bgPlotArea[(unsigned int)(x)] |= 8;

		}

		LCD::loadCGRAMcharacter(j, (char *)(&bgPlotArea[0]));
		j |= 0x04;
		LCD::loadCGRAMcharacter(j, (char *)(&bgPlotArea[8]));

		clearBGplot(yIdx);

	}

}

uint8_t bgConvert(unsigned long v, unsigned long ll, unsigned long d)
{

	uint8_t b;

	v *= 15;
	if (v < ll) b = 254;
	else
	{

		v -= ll;
		if (d == 0) b = 7;
		else
		{

			v /= d;
			if (v > 15) b = 255;
			else b = (uint8_t)(v);

		}

	}

	return b;

}

void formatBarGraph(uint8_t bgSize, uint8_t slotIdx, unsigned long centerVal, unsigned long topLimit)
{

	uint8_t i;
	uint8_t k;
	uint8_t t;
	uint8_t y = 0;

	unsigned long v;
	unsigned long v1 = centerVal;
	unsigned long v2 = centerVal;
	unsigned long v3;
	unsigned long v4 = topLimit / 4;

	if (centerVal) v3 = topLimit - v4;
	else v3 = 0;
	v4 += topLimit;

	if (v1 < v3)
	{

		v = v1;
		v1 = v3;
		v3 = v;

	}

	i = 0;
	while (i < bgSize)
	{

		v = barGraphData[(unsigned int)(i)];
		if (v > v2)
		{

			if (v < v4) v2 = v;
			else v2 = v4;

		}
		if (v < v1)
		{

			if (v > v3) v1 = v;
			else v1 = v3;

		}
		i++;

	}

	if (v2 - v1)
	{

		v2 -= v1;

		v1 *= 15;

		y = bgConvert(centerVal, v1, v2);

		i = 0;
		while (i < bgSize)
		{

			mBuff1[(unsigned int)(i)] = bgConvert(barGraphData[(unsigned int)(i)], v1, v2);
			i++;

		}

	}
	else
	{

		y = 7;

		i = 0;
		while (i < bgSize) mBuff1[(unsigned int)(i++)] = y;

	}

	clearBGplot(y);
	k = bgDataSize;

	i = 0;

	while (i < bgSize)
	{

		k--;
		t = mBuff1[(unsigned int)(i)];

		if ((k == slotIdx) && (mainLoopHeartBeat & 0b01010101))
		{

			if (t > 253) bgPlot(k, y, t, 1);
			else bgPlot(k, t, t, 1);

		}
		else
		{

			if (t > 253) bgPlot(k, y, t, 1);
			else bgPlot(k, y, t, 0);

		}

		bgOutputPlot(k, y);

		i++;

	}

	while ((--k) < bgDataSize) bgOutputPlot(k, y);

}

const char bgSpaces[] PROGMEM = {
	' ', ' ', 0x80, 0x81, 0x82, 0x83, ' ', ' ', 0x00
	,' ', ' ', 0x84, 0x85, 0x86, 0x87, ' ', ' ', 0x00
};

void displayBarGraphLine(uint8_t lineNum, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripIdxChar)
{

	calcFuncObj thisCalcFuncObj;

	text::gotoXY(devLCD, 0, lineNum);
	text::stringOut(devLCD, bgSpaces, lineNum);

	if (tripIdx < dfMaxValDisplayCount)
	{

		thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx);

		text::charOut(devLCD, tripIdxChar);
		text::charOut(devLCD, thisCalcFuncObj.calcChar);

		text::stringOut(devLCD, doFormat(thisCalcFuncObj, 6, 0));

	}
	else
	{

		text::clearEOL(devLCD);

	}

}

#endif // useBarGraph
unsigned long doCalculate(uint8_t tripIdx, uint8_t calcIdx)
{

	return SWEET64((const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(calcIdx)])), tripIdx);

}

const uint8_t prgmMultiplyBy100[] PROGMEM = {
	instrLdRegByte, 0x01, 100,							// load d = 100 into register 1
	instrMul2by1,										// multiply result by 100

	instrLdRegByteFromIndex, 0x03,						// load register 3 with whatever's in the trip variable index
	instrAddYtoX, 0x23,									// add registers 2 and 3, store the result in 2

	instrDone											// exit to caller
};

const uint8_t prgmMultiplyBy10[] PROGMEM = {
	instrLdRegByte, 0x01, 10,							// load d = 10 into register 1
	instrMul2by1,										// multiply result by 10

	instrLdRegByteFromIndex, 0x03,						// load register 3 with whatever's in the trip variable index
	instrAddYtoX, 0x23,									// add registers 2 and 3, store the result in 2

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

	init64((union union_64 *)(&s64reg[s64reg2]), 0); // initialize 64-bit number to zero

	while ((loopFlag) && (x < 17))
	{

		if ((c = strBuffer[(unsigned int)(x++)])) // if a non-NULL character is read in
		{

			n *= 10; // shift accumulator left one digit
			if (c != ' ') n += (uint8_t)(c) - 48; // if character is not a leading space, go add it to accumulator
			f= 1 - f; // flip-flop the SWEET64 addition flag

			if (f)
			{

				SWEET64(prgmMultiplyBy100, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 100 + n
				n = 0;

			}

		}
		else loopFlag = 0; // otherwise, terminate loop upon receipt of a NULL character

	}

	if (f == 0) SWEET64(prgmMultiplyBy10, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 10 + n

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

char * ull2str(const uint8_t * prgmPtr, char * strBuffer, uint8_t decimalPlaces)
{

	union union_64 * tmpPtr2 = (union union_64 *)(&s64reg[s64reg3]);

	uint8_t b;
	char c;
	uint8_t l;
	uint8_t y;
	char z;

	SWEET64(prgmPtr, decimalPlaces); // call SWEET64 routine to perform decimal point rounding to next nearest decimal place

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

char * formatDecimal(const uint8_t * prgmPtr, char * strBuffer, uint8_t windowLength, uint8_t decimalPlaces, uint8_t decimalFlag) // format number for output
{

	uint8_t x;
	uint8_t y;
	uint8_t z; // pointer to end of integer digits in string
	uint8_t c; // temporary character storage
	uint8_t i; // integer digit count

	ull2str(prgmPtr, strBuffer, decimalPlaces); // perform rounding of number to nearest decimal place, then format for ASCII output

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

static char * doFormat(calcFuncObj &thisCalcFuncObj, uint8_t windowLength, uint8_t decimalFlag)
{

	if ((thisCalcFuncObj.calcIdx < dfMaxValDisplayCount) && (thisCalcFuncObj.tripIdx < tripSlotCount))
	{

		doCalculate(thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);

		if (thisCalcFuncObj.calcFmtIdx == calcFormatTimeHHmmSSIdx) ull2str(prgmFormatToTime, mBuff1, 0);
		else
		{

			formatDecimal(prgmRoundOffNumber, mBuff1, windowLength, thisCalcFuncObj.decimalPlaces, decimalFlag);

		}

	}
	else
	{

		strcpy_P(mBuff1, overFlowStr);
		mBuff1[(unsigned int)(windowLength)] = 0;

	}

	return mBuff1;

}

const uint8_t prgmConvertToTime[] PROGMEM = {
	instrLdRegConst, 0x01, idxCycles0PerSecond,
	instrDiv2by1,
	instrDone
};

unsigned long convertTime(unsigned long * an)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	copy64((union union_64 *)(&s64reg[s64reg2]), (union union_64 *)(an)); // perform atomic copy of time variable to SWEET64 register 2

	SREG = oldSREG; // restore state of interrupt flag

	return SWEET64(prgmConvertToTime, 0); // go convert time value from system ticks to seconds

}

#ifdef useSimulatedFIandVSS
const uint8_t debugScreenFormats[4][2] PROGMEM = {
	 {lblInstantIdx,	tInjectorTotalTime} 		// Debug
	,{lblInstantIdx,	tVSStotalTime}
	,{lblInstantIdx,	tInjectorPulseCount}
	,{lblInstantIdx,	tVSSpulseCount}
};

const char debugScreenFuncNames[] PROGMEM = {
	"FI ON   VSS ON  \0"
	"FI OFF  VSS ON  \0"
	"FI OFF  VSS OFF \0"
	"FI ON   VSS OFF \0"
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

static uint8_t debugReading::displayHandler(uint8_t cmd, uint8_t cursorPos)
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
					EEPROM::changeBitFlags(debugFlags, 0, (debugInjectorFlag | debugVSSflag));
					break;

				case 1:
					EEPROM::changeBitFlags(debugFlags, debugInjectorFlag, debugVSSflag);
					break;

				case 2:
					EEPROM::changeBitFlags(debugFlags, (debugInjectorFlag | debugVSSflag), 0);
					break;

				case 3:
					EEPROM::changeBitFlags(debugFlags, debugVSSflag, debugInjectorFlag);
					break;

				default:
					break;

			}
			if ((debugFlags & debugEnableFlags) ^ retVal) configurePorts();

		case menuOutputDisplayIdx:
			displayMainScreenFunctions(debugScreenFormats, 0, 136, 0, msTripBitPattern);
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

#ifdef useATmega32U4
		DDRB |= (1 << DDB7); // enable VSS sense pin interrupt
#endif // useATmega32U4
#ifdef useATmega2560
		DDRK |= (1 << DDK0); // enable VSS sense pin interrupt
#endif // useATmega2560
#ifdef useATmega128
		DDRC |= (1 << DDC0); // enable VSS sense pin interrupt
#endif // useATmega128

		debugVSStickLength = 0;

		debugVSScount = 0;

		debugVSSidx = 0;

		debugVSSstate = 0; // start out by ramping up from 0 MPH to 250 MPH

		debugFlags &= ~(debugVSSready); // reset VSS ready bit

	}
	else
	{

#ifdef useATmega32U4
		DDRB &= ~(1 << DDB7); // disable VSS sense pin interrupt
#endif // useATmega32U4
#ifdef useATmega2560
		DDRK &= ~(1 << DDK0); // disable VSS sense pin interrupt
#endif // useATmega2560
#ifdef useATmega128
		DDRC &= ~(1 << DDC0); // disable VSS sense pin interrupt
#endif // useATmega128

	}

	// configure fuel injector pins for either normal operation input or debug output
	if (debugFlags & debugInjectorFlag) // configure injector sense pins as outputs
	{

#ifdef useATmega32U4
		DDRD |= ((1 << DDD3) | (1 << DDD2)); // enable injector sense pin interrupts
		PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // useATmega32U4
#ifdef useATmega2560
		DDRE |= ((1 << DDE4) | (1 << DDE5)); // enable injector sense pin interrupts
		PORTE |= ((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // useATmega2560
#ifdef useATmega128
		DDRD |= ((1 << DDD3) | (1 << DDD2)); // enable injector sense pin interrupts
		PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // useATmega128

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

#ifdef useATmega32U4
		DDRD &= ~((1 << DDD3) | (1 << DDD2)); // disable injector sense pin interrupts
#endif // useATmega32U4
#ifdef useATmega2560
		DDRE &= ~((1 << DDE4) | (1 << DDE5)); // disable injector sense pin interrupts
#endif // useATmega2560
#ifdef useATmega128
		DDRD &= ~((1 << DDD3) | (1 << DDD2)); // disable injector sense pin interrupts
#endif // useATmega128

	}

	if (debugFlags & debugEnableFlags) timer1Command |= (t1cEnableDebug);
	else timer1Command &= ~(t1cEnableDebug);

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // useSimulatedFIandVSS
#ifdef useDebugValueReadout
const uint8_t debugValueScreenFormats[4][2] PROGMEM = {
	 {0,	0x80 | tShowDebugValue}				// Debug show values
	,{1,	0x80 | tShowDebugValue}
	,{2,	0x80 | tShowDebugValue}
	,{3,	0x80 | tShowDebugValue}
};

const char debugValueScreenFuncNames[] PROGMEM = {
	"Show Debug Vals\0"
};

static const uint8_t prgmShowSWEET64Flags[] PROGMEM = { // if operand < tripidx, 0x00 (0) (mzc). if operand==tripidx, 0x10 (16) (mZc). if operand > tripidx, 0x28 (40) (MzC)
//	instrLdRegByteFromIndex, 0x01,						// tripidx
//	instrLdRegByte, 0x02, 1,							// reg
//	instrCmpReg, 0x21,
	instrTestIndex,
	instrDone
};

static uint8_t debugValueDisplay::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(debugValueScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			displayMainScreenFunctions(debugValueScreenFormats, cursorPos, 136, 0, msTripBitPattern);
			break;

		default:
			break;

	}

	return retVal;

}

static uint8_t debugValueDisplay::getSWEET64flags(uint8_t testVal)
{

	SWEET64(prgmShowSWEET64Flags, testVal);

	return SWEET64processorFlags;

}

#endif // useDebugValueReadout
/* Main screen section */

const char mainScreenFuncNames[] PROGMEM = {
	"Instrument\0"
	"Custom\0"
#ifdef useChryslerMAPCorrection
	"Pressures\0"
#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
	"Voltages\0"
#endif // useAnalogRead
	"Instant/Current\0"
	"Instant/Tank\0"
	"Current\0"
	"Tank\0"
#ifdef trackIdleEOCdata
	"EOC/Idle\0"
#endif // trackIdleEOCdata
	"Current Data\0"
	"Tank Data\0"
#ifdef trackIdleEOCdata
	"Current EOC/Idle\0"
	"Tank EOC/Idle\0"
#endif // trackIdleEOCdata
	"Remaining\0"
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

#ifdef useChryslerMAPCorrection
	,{(vMAPpressureIdx - vMAPpressureIdx),	0x80 | tPressureChannel}		// Pressures
	,{(vBaroPressureIdx - vMAPpressureIdx),	0x80 | tPressureChannel}
	,{(vFuelPressureIdx - vMAPpressureIdx),	0x80 | tPressureChannel}
	,{lblInstantIdx,			tFuelRate}

#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
#ifdef useATmega32U4
	,{analog0Idx,				0x80 | tAnalogChannel}	// Voltages
	,{analog1Idx,				0x80 | tAnalogChannel}
	,{analog2Idx,				0x80 | tAnalogChannel}
	,{analog2Idx,				0x80 | tAnalogChannel}
#endif // useATmega32U4
#ifdef useATmega2560
	,{analog0Idx,				0x80 | tAnalogChannel}	// Voltages
	,{analog1Idx,				0x80 | tAnalogChannel}
#ifdef useAnalogButtons
	,{analog2Idx,				0x80 | tAnalogChannel}
#ifdef useTWIsupport
	,{analog0Idx,				0x80 | tAnalogChannel}
#else // useTWIsupport
	,{analog3Idx,				0x80 | tAnalogChannel}
#endif // useTWIsupport
#else // useAnalogButtons
	,{analog0Idx,				0x80 | tAnalogChannel}
	,{analog1Idx,				0x80 | tAnalogChannel}
#endif // useAnalogButtons
#endif // useATmega2560
#ifdef useATmega128
	,{analog0Idx,				0x80 | tAnalogChannel}	// Voltages
	,{analog1Idx,				0x80 | tAnalogChannel}
#ifdef useAnalogButtons
	,{analog2Idx,				0x80 | tAnalogChannel}
#ifdef useTWIsupport
	,{analog0Idx,				0x80 | tAnalogChannel}
#else // useTWIsupport
	,{analog3Idx,				0x80 | tAnalogChannel}
#endif // useTWIsupport
#else // useAnalogButtons
	,{analog0Idx,				0x80 | tAnalogChannel}
	,{analog1Idx,				0x80 | tAnalogChannel}
#endif // useAnalogButtons
#endif // useATmega128

#endif // useAnalogRead
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
uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos)
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
			topScreenLevel = menuLevel; // save current menu level for sub-function (param edit, trip load/save, etc) support (try to get rid of this)

		case menuCursorUpdateIdx:
			printStatusMessage(mainScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			retVal = (activityFlags & afActivityCheckFlags);
			switch (retVal)
			{

				case (afVehicleStoppedFlag | afButtonFlag):
#ifdef trackIdleEOCdata
					displayMainScreenFunctions(mainIdleScreenFormats, 0, 136, 0, msTripBitPattern);
					break;

#endif // trackIdleEOCdata
				case (afEngineOffFlag | afButtonFlag):
#ifdef trackIdleEOCdata
					displayMainScreenFunctions(mainEOCscreenFormats, 0, 136, 0, msTripBitPattern);
					break;

#endif // trackIdleEOCdata
				default:
#ifdef useScreenEditor
					i = cursorPos;
					i <<= 2;
					i += eePtrScreensStart;

					for (uint8_t x = 0; x < 4; x++) displayFunction(x, (uint8_t)(EEPROM::readVal(i++)), (uint8_t)(EEPROM::readVal(i++)), 136, 0, msTripBitPattern);

#else // useScreenEditor
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos, 136, 0, msTripBitPattern);
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

void doNextBright(void)
{

	brightnessIdx++;
	if (brightnessIdx < brightnessLength) LCD::setBrightness(brightnessIdx);
	else
	{

		brightnessIdx = 0;
		LCD::shutdown();
		LCD::init();

	}

	initStatusLine();
	text::stringOut(devLCD, PSTR("Backlight = "));
	text::stringOut(devLCD, brightString, brightnessIdx);
	execStatusLine();

}

void doTripResetTank(void)
{

	tripArray[(unsigned int)(tankIdx)].reset();
#ifdef trackIdleEOCdata
	tripArray[(unsigned int)(eocIdleTankIdx)].reset();
#endif // trackIdleEOCdata
#ifdef useBarFuelEconVsSpeed
	doResetBarFEvS();
#endif // useBarFuelEconVsSpeed
#ifdef usePartialRefuel
	EEPROM::writeVal(pRefuelSizeIdx, 0);
#endif // usePartialRefuel
	printStatusMessage(PSTR("Tank Reset"));

}

void doTripResetCurrent(void)
{

	tripArray[(unsigned int)(currentIdx)].reset();
#ifdef trackIdleEOCdata
	tripArray[(unsigned int)(eocIdleCurrentIdx)].reset();
#endif // trackIdleEOCdata
	printStatusMessage(PSTR("Current Reset"));

}

#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void)
{

	peripheral::shutdownMain();

#ifdef useTWIsupport
	TWI::shutdown();

#endif // useTWIsupport
#ifdef useAnalogRead
#ifdef useAnalogButtons
	while (buttonCount == 0) // keep sleeping until there is an analog button press event
	{

		performSleepMode(SLEEP_MODE_ADC); // go perform cheesy ADC noise reduction sleep mode (because we must stay awake enough to monitor buttons)

	}

#else // useAnalogButtons
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

#endif // useAnalogButtons
#else // useAnalogRead
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

#endif // useAnalogRead

	peripheral::initMain(); // restart all peripherals

}

#endif // useDeepSleep
#ifdef useBarFuelEconVsSpeed // (parameter) vs. Speed Bar Graph display section
const uint8_t barFEvSdisplayFuncs[] PROGMEM = {
	tFuelEcon
	,tFuelUsed
	,tMotionTime
	,tDistance
};

const char barFEvSfuncNames[] PROGMEM = {
	"FE / Speed\0"
	"Fuel Used/Speed\0"
	"RunTime / Speed\0"
	"Distance / Speed\0"
};

const uint8_t prgmFEvsSpeed[] PROGMEM = {
	instrCall, tSpeed,									// go generate vehicle speed

	instrLdRegEEPROM, 0x03, pBarLowSpeedCutoffIdx,		// obtain low-speed cutoff parameter in decimal formatting term * unit distance per hour
	instrSkipIfLTorE, 0x32, 4,							// if low-speed cutoff parameter <= speed, skip to next section

	instrLdRegByte, 0x02, 0xFF,							// load a 255 into register 2
	instrDone,											// exit to caller

	instrSubYfromX, 0x23,								// subtract low-speed cutoff parameter from speed

	instrLdRegEEPROM, 0x01, pBarSpeedQuantumIdx,		// load speed quantum parameter in decimal formatting term * unit distance per hour
	instrTestReg, 0x01,
	instrSkipIfFlagSet, SWEET64zeroFlag, 242,			// if (speed quantum parameter == 0), load an 0xFF into register 2 and exit to caller

	instrDiv2by1,										// divide to obtain quantum trip index value

	instrLdRegByte, 0x03, bgDataSize,					// compare to bargraph data size
	instrSkipIfLTorE, 0x32, 235,						// if (bargraph data size <= quantum trip index value), load an 0xFF into register 2 and exit to caller
	instrLdRegByte, 0x01, FEvsSpeedIdx,					// add quantum trip index value to base FE vs. speed trip index, to obtain relevant FE vs. speed trip index
	instrAddYtoX, 0x21,
	instrDone											// exit to caller
};

uint8_t FEvSpdTripIdx;

const uint8_t tripIDchars[(unsigned int)(tripSlotCount)] PROGMEM = {
	'0'
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
	,'C'	// *facepalm*
	,'D'
	,'E'
};

void doCursorUpdateBarFEvS(void)
{

	uint8_t calcIdx;

	calcIdx = pgm_read_byte(&barFEvSdisplayFuncs[(unsigned int)(screenCursor[(unsigned int)(barFEvSscreenIdx)])]);

	for (uint8_t x = 0; x < bgDataSize; x++) barGraphData[(unsigned int)(x)] = doCalculate((FEvsSpeedIdx + bgDataSize - x - 1), calcIdx);

	printStatusMessage(barFEvSfuncNames, screenCursor[(unsigned int)(barFEvSscreenIdx)]); // briefly display screen name

}

void doBarFEvSdisplay(void)
{

	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t tripIdxChar;

	tripIdx = FEvSpdTripIdx - FEvsSpeedIdx;
	tripIdxChar = pgm_read_byte(&tripIDchars[(unsigned int)(tripIdx)]);
	calcIdx = pgm_read_byte(&barFEvSdisplayFuncs[(unsigned int)(screenCursor[(unsigned int)(barFEvSscreenIdx)])]);

	if (FEvSpdTripIdx < 255) barGraphData[(unsigned int)(bgDataSize - tripIdx - 1)] = doCalculate(FEvSpdTripIdx, calcIdx);

	formatBarGraph(bgDataSize, tripIdx, 0, doCalculate(tankIdx, calcIdx));

	displayBarGraphLine(0, FEvSpdTripIdx, calcIdx, tripIdxChar);
	displayBarGraphLine(1, ((mainLoopHeartBeat & 0b00110011) ? tankIdx : instantIdx), ((mainLoopHeartBeat & 0b00110011) ? calcIdx : tSpeed), ((mainLoopHeartBeat & 0b00110011) ? 'T' : 'I'));

}

void doResetBarFEvS(void)
{

	for (uint8_t x = 0; x < bgDataSize; x++) tripArray[(unsigned int)(FEvsSpeedIdx + x)].reset();

}

#endif // useBarFuelEconVsSpeed
#ifdef useBarFuelEconVsTime // Differential/Absolute Fuel Economy vs. Time display section
const char barFEvTfuncNames[] PROGMEM = {
	"DiffFE / Time\0"
	"FE / Time\0"
};

unsigned long barFEvsTimeData[bgDataSize];

unsigned int bFEvTperiod;
unsigned int bFEvTcount;

uint8_t bFEvTstartIDx;
uint8_t bFEvTsize;

const uint8_t prgmFindFEvsTimePeriod[] PROGMEM = {
	instrLdRegByte, 0x01, loopsPerSecond,
	instrLdRegEEPROM, 0x02, pFEvsTimeIdx,
	instrMul2by1,
	instrDone
};

void doResetBarFEvT(void)
{

	bFEvTperiod = (unsigned int)(SWEET64(prgmFindFEvsTimePeriod, 0));
	tripArray[(unsigned int)(periodIdx)].reset();
	bFEvTcount = bFEvTperiod;
	bFEvTstartIDx = 0;
	bFEvTsize = 0;

}

void doCursorUpdateBarFEvT(void)
{

	printStatusMessage(barFEvTfuncNames, screenCursor[(unsigned int)(barFEvTscreenIdx)]); // briefly display screen name

}

void doBarFEvTdisplay(void)
{

	uint8_t i = 0;
	uint8_t j = bFEvTstartIDx;
	unsigned long v = doCalculate(currentIdx, tFuelEcon);

	while (i < bFEvTsize)
	{

		if (j == 0) j = bgDataSize;
		j--;

		barGraphData[(unsigned int)(i)] = barFEvsTimeData[(unsigned int)(j)];
		i++;

	}

	formatBarGraph(bFEvTsize, (bgDataSize - 1), ((screenCursor[(unsigned int)(barFEvTscreenIdx)]) ? 0 : v), v);

	displayBarGraphLine(0, currentIdx, tFuelEcon, 'C');
	displayBarGraphLine(1, periodIdx, tFuelEcon, 'P');

}

#endif // useBarFuelEconVsTime
#ifdef useCPUreading

/* CPU loading and RAM availability support section */

unsigned long timerLoopStart;
unsigned long thisTimerLoopLength;
unsigned long lastTimerLoopLength;

const uint8_t prgmFindCPUutilPercent[] PROGMEM = {
	instrLdRegConst, 0x01, idxNumerCPUutil,
	instrMul2by1,
	instrLdRegConst, 0x01, idxDenomCPUutil,
	instrDiv2by1,
	instrDone
};

const uint8_t prgmOutputAvailableRAM[] PROGMEM = {
	instrLdRegConst, 0x01, idxDecimalPoint,
	instrMul2by1,
	instrJump, tFormatToNumber
};

static uint8_t systemInfo::displayHandler (uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx: // display max cpu utilization and RAM
			showCPUload();
			text::stringOut(devLCD, PSTR(" T"));
			convertTime((uint32_t *)(&systemCycles));
			text::stringOut(devLCD, ull2str(prgmFormatToTime, mBuff1, 3)); // output system time (since MPGuino was powered up)

			text::gotoXY(devLCD, 0, 1);
			text::stringOut(devLCD, PSTR("FREE RAM: "));
			init64((union union_64 *)(&s64reg[s64reg2]), (unsigned long)(getAvailableRAMsize()));
			text::stringOut(devLCD, formatDecimal(prgmOutputAvailableRAM, mBuff1, 6, 0, 0));
			break;

		default:
			break;

	}

	return retVal;

}

static void systemInfo::showCPUload(void)
{

	text::stringOut(devLCD, PSTR("C%"));
	init64((union union_64 *)(&s64reg[s64reg2]), lastTimerLoopLength);
	SWEET64(prgmFindCPUutilPercent, 0);
	text::stringOut(devLCD, formatDecimal(prgmRoundOffNumber, mBuff1, 6, 2, 0));

}

static void systemInfo::showCPUloading(void)
{

	initStatusLine();
	showCPUload();
	execStatusLine();

}

static unsigned int systemInfo::getAvailableRAMsize(void)
{

	unsigned int availableRAMptr;

	if((unsigned int)__brkval == 0) availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)&__bss_end);
	else availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)__brkval);

	return availableRAMptr;

}

static unsigned long systemInfo::cycles0(void)
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

#ifdef useBenchMark
static void systemInfo::doBenchMark(void)
{

	unsigned long t = 0;
	unsigned long w;
	unsigned long s;
	unsigned long e;

	unsigned long c;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	while (!(TIFR0 & (1 << TOV0))); // wait for timer0 to overflow
	TIFR0 |= (1 << TOV0); // reset timer0 overflow flag

	s = TCNT0; // do a microSeconds() - like read to determine loop length in cycles

	for (unsigned int x = 0; x < 1000; x++)
	{

		if (TIFR0 & (1 << TOV0))
		{

			TIFR0 |= (1 << TOV0); // reset timer0 overflow flag
			t += 256;

		}

		c = (unsigned long)(iSqrt((unsigned int)(s)));

	}

	e = TCNT0; // do a microSeconds() - like read to determine loop length in cycles

	if (TIFR0 & (1 << TOV0))
	{

		e = TCNT0;
		TIFR0 |= (1 << TOV0); // reset timer0 overflow flag
		t += 256;

	}

	e += t;

	SREG = oldSREG; // restore interrupt flag status

	w = findCycleLength(s, e) - 156;

	init64((union union_64 *)(&s64reg[s64reg2]), w);
	SWEET64(prgmConvertToMicroSeconds, 0);

	initStatusLine();
	text::stringOut(devLCD, formatDecimal(prgmRoundOffNumber, mBuff1, 6, 3, 0));
	text::stringOut(devLCD, PSTR(" usec"));
	execStatusLine();

}

#endif // useBenchMark
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
	"\0"
	"Drag Ready\0"
	"Drag Test Start\0"
	"Drag Finished\0"
	"Drag Cancelled\0"
	"DIST          \0"
	"     HALF     \0"
	"DIST HALF     \0"
	"          FULL\0"
	"DIST      FULL\0"
	"     HALF FULL\0"
	"DIST HALF FULL\0"
	"Drag Test Fail\0"
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

// display variable trip labels
const uint8_t drTripBitPattern[][4] PROGMEM = {
	{0b00000000, 0b00000001, 0b00000010, 0b00000100} // /
	,{0b00000000, 0b00000010, 0b00000101, 0b00000010} // full circle
	,{0b00000000, 0b00000110, 0b00000101, 0b00000110} // D
};

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

const uint8_t prgmInitializeAccelTest[] PROGMEM = {
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// fetch cycles per second constant
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,		// store it in instantaneous accel test speed period variable

	instrLdRegEEPROM, 0x01, pDragSpeedIdx,				// fetch drag function speed parameter value in unit distance per hour
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// multiply by distance parameter value in VSS pulses
	instrMul2by1,
	instrLdReg, 0x32,									// save denominator term in VSS pulses per hour for later

	instrLdRegConst, 0x01, idxCycles0PerSecond,			// fetch cycles per second constant
	instrShiftRegLeft, 0x01,							// multiply it by two to obtain accel half speed period value
	instrLdRegConst, 0x02, idxSecondsPerHour,			// multiply it by seconds per hour constant
	instrMul2by1,
	instrLdRegConst, 0x01, idxDecimalPoint,				// fetch decimal formatting term
	instrMul2by1,										// multiply to factor out decimal formatting term from accel test speed parameter value

	instrLdReg, 0x13,									// fetch denominator term calculated above
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelFullPeriodValueIdx,	// save it to accel test full-speed period tripwire variable

	instrShiftRegLeft, 0x02,							// shift result right one bit
	instrStRegVolatile, 0x02, vAccelHalfPeriodValueIdx,	// save it to accel test half-speed period tripwire variable

	instrLdRegEEPROM, 0x01, pDragDistanceIdx,			// fetch drag function distance parameter value in unit distance
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// multiply to obtain drag function distance parameter value in VSS pulses
	instrMul2by1,
	instrLdRegConst, 0x01, idxDecimalPoint,				// get rid of decimal formatting factor
	instrDiv2by1,										// perform conversion
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelDistanceValueIdx,	// save it to accel test distanct tripwire variable

	instrDone											// exit to caller
};

const uint8_t prgmAccelTestCompareFullSpeeds[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrTestReg, 0x01,									// test accel test autotrigger flag
	instrBranchIfZero, 12,								// if zero, then output a 1

	instrLdRegTripVar, 0x01, rvVSScycleIdx, dragRawFullSpeedIdx,	// load raw full speed register elapsed time
	instrLdRegTripVar, 0x02, rvVSScycleIdx, dragFullSpeedIdx,		// load regular full speed register elapsed time

	instrCmpReg, 0x12,									// if raw full speed register has a smaller time than regular full speed register, output a 1
	instrBranchIfLTorE, 4,								// if raw full speed register has a smaller time than regular full speed register, output a 1

	instrLdRegByte, 0x02, 1,							// load a 1 into register 2
	instrDone,											// exit to caller

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

	displayMainScreenFunctions(dragRaceScreenFormats, screenCursor[(unsigned int)(dragRaceIdx)], 136, 0, drTripBitPattern);

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

				tripArray[(unsigned int)(dragRawHalfSpeedIdx)].reset(); // zero out acceleration 1/2 speed setpoint data
				tripArray[(unsigned int)(dragRawFullSpeedIdx)].reset(); // zero out acceleration full speed setpoint data
				tripArray[(unsigned int)(dragRawDistanceIdx)].reset(); // zero out acceleration distance setpoint data

				SWEET64(prgmInitializeAccelTest, 0); // initialize low-level values and tripwires

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
	"Coastdown Active\0"
	"CD Sample 1 read\0"
	"CD Sample 2 read\0"
	"CD Sample 3 read\0"
	"Coastdown Done\0"
	"Coastdown Canx\0"
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
		text::clearEOL(devLCD);
		displayFunction(2, lblInstantIdx, tSpeed, 136, 0, msTripBitPattern); // call main screen function display routine

		text::gotoXY(devLCD, 8, 1);
		text::stringOut(devLCD, PSTR(" ACTIVE"));
		text::charOut(devLCD, pgm_read_byte(&coastdownSymbol[(unsigned int)(coastdownCharIdx++)]));

	}
	else
	{

		i = screenCursor[(unsigned int)(coastdownIdx)] + pCoefficientDidx;

		EEPROM::readValToString(i, pBuff);

		text::stringOut(devLCD, parmLabels, i - 1); // print parameter name at top left
		text::clearEOL(devLCD);
		text::gotoXY(devLCD, 0, 1); // go to next line
		text::stringOut(devLCD, pBuff);

	}

	text::clearEOL(devLCD);

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
#ifdef useSavedTrips // Trip save/restore/raw data view support section
const uint8_t tripSelectList[(unsigned int)(savedTripsCount)] PROGMEM = {
	currentIdx
	,tankIdx
#ifdef trackIdleEOCdata
	,eocIdleCurrentIdx
	,eocIdleTankIdx
#endif // trackIdleEOCdata
};

const char tripSaveFuncNames[] PROGMEM = {
	"View Active \0"
#ifdef showSavedTrips
	"View Saved \0"
#endif // showSavedTrips
	"Save \0"
	"Load \0"
};

const uint8_t tripScreenIdxBase = 6
#ifdef useChryslerMAPCorrection
	+ 1
#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
	+ 1
#endif // useAnalogRead
#ifdef trackIdleEOCdata
	+ 1
#endif // trackIdleEOCdata
;

void doTripSaveDisplay(void)
{

	tripSaveDisplayOut(tripSaveFuncNames);

}

void tripSaveDisplayOut(const char * menuString)
{

	uint8_t tripSlot;
	uint8_t tripPtr;

	tripSlot = menuLevel - tripSaveCurScreenIdx;

	text::stringOut(devLCD, menuString, screenCursor[(unsigned int)(menuLevel)]); // print trip function name at top left
	text::stringOut(devLCD, msTripNameString, tripSlot + 1);
	text::clearEOL(devLCD);

	tripPtr = getBaseTripPointer(tripSlot);

	text::gotoXY(devLCD, 0, 1); // go to next line
	if (guinosig == (uint8_t)(EEPROM::readVal(tripPtr + tripSlotSignaturePtr)))
	{

		init64((union union_64 *)(&s64reg[s64reg2]), EEPROM::readVal(tripPtr));
		text::stringOut(devLCD, ull2str(prgmFormatToTime, mBuff1, 3));

	}
	else text::stringOut(devLCD, PSTR("Empty"));
	text::clearEOL(devLCD);

}

void doGoTripSaveTank(void)
{

	cursor::moveAbsolute(tripSaveTankScreenIdx, 2);

}

void doGoTripSaveCurrent(void)
{

	cursor::moveAbsolute(tripSaveCurScreenIdx, 2);

}

void doTripSelect(void)
{

	uint8_t tripSlot;

	tripSlot = menuLevel - tripSaveCurScreenIdx;

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 0: // view active
			cursor::moveAbsolute(topScreenLevel, tripScreenIdxBase + tripSlot);
			break;

#ifdef showSavedTrips
		case 1: // view saved
			cursor::moveAbsolute(tripShowCurScreenIdx + tripSlot, 0);
			break;

#endif // showSavedTrips
		default:
			noSupport();
			break;

	}

}

void doTripLongSelect(void)
{

	uint8_t tripSlot;
	uint8_t tripIdx;
#ifdef trackIdleEOCdata
	uint8_t tripIdleEOCidx;
#endif // trackIdleEOCdata

	tripSlot = menuLevel - tripSaveCurScreenIdx;
	tripIdx = pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot)]);
#ifdef trackIdleEOCdata
	tripIdleEOCidx = pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot + 2)]);
#endif // trackIdleEOCdata

#ifdef showSavedTrips
	switch (screenCursor[(unsigned int)(menuLevel)])
#else // showSavedTrips
	switch (screenCursor[(unsigned int)(menuLevel)] + 1)
#endif // showSavedTrips
	{

		case 2: // save
			tripArray[(unsigned int)(tripIdx)].save(tripSlot);
#ifdef trackIdleEOCdata
			tripArray[(unsigned int)(tripIdleEOCidx)].save(tripSlot + 2);
#endif // trackIdleEOCdata
			tripDisplayStatus(tripSlot, PSTR(" Save"));
			menuLevel = mainScreenIdx;
			break;

		case 3: // load
			tripArray[(unsigned int)(tripIdx)].load(tripSlot);
#ifdef trackIdleEOCdata
			tripArray[(unsigned int)(tripIdleEOCidx)].load(tripSlot + 2);
#endif // trackIdleEOCdata
			tripDisplayStatus(tripSlot, PSTR(" Load"));
			menuLevel = mainScreenIdx;
			break;

		default:
			doTripSelect();
			break;

	}

}

void tripDisplayStatus(uint8_t tripSlot, const char * str)
{

	initStatusLine();
	text::stringOut(devLCD, msTripNameString, tripSlot + 1);
	text::stringOut(devLCD, PSTR(" Trip "));
	text::charOut(devLCD, '0' + tripSlot);
	text::stringOut(devLCD, str);
	execStatusLine();

}

#ifdef showSavedTrips
const char tripShowFuncNames[] PROGMEM = {
	"Timestamp\0"
	"Inj Pulse\0"
	"VSS Pulse\0"
	"Inj CycL\0"
	"Inj CycH\0"
	"Inj OpenCycL\0"
	"Inj OpenCycH\0"
	"VSS CycL\0"
	"VSS CycH\0"
	"Ident\0"
};

void doTripShowDisplay(void)
{

	uint8_t tripSlot;
	uint8_t tripVariable;
	uint8_t tripPtr;

	tripSlot = menuLevel - tripShowCurScreenIdx;
	tripVariable = screenCursor[(unsigned int)(menuLevel)];

	EEPROM::readValToString((getBaseTripPointer(tripSlot) + tripVariable), pBuff); // fetch value from EEPROM, and format it for decimal output

	text::charOut(devLCD, '0' + tripSlot);
	text::charOut(devLCD, ':');
	text::charOut(devLCD, ' ');
	text::stringOut(devLCD, tripShowFuncNames, tripVariable);
	text::clearEOL(devLCD);

	text::gotoXY(devLCD, 0, 1); // go to next line
	text::stringOut(devLCD, pBuff); // print value
	text::clearEOL(devLCD);

}

#endif // showSavedTrips
uint8_t doTripAutoAction(uint8_t taaMode)
{

	uint8_t b;
	uint8_t c = 0;

	if (EEPROM::readVal(pAutoSaveActiveIdx))
	{

		for (uint8_t x = 0; x < savedTripsCount; x++)
		{

			b = pgm_read_byte(&tripSelectList[(unsigned int)(x)]);

			if (taaMode) c += tripArray[(unsigned int)(b)].load(x);
			else c += tripArray[(unsigned int)(b)].save(x);

		}

	}

	return c;

}

#endif // useSavedTrips
#ifdef usePartialRefuel // Partial Refuel support section
const char partialRefuelFuncNames[] PROGMEM = {
	"Add Partial\0"
	"Zero Partial\0"
	"Reset Tank Trip\0"
};

void doPartialRefuelDisplay(void)
{

	EEPROM::readValToString(pRefuelSizeIdx, pBuff); // fetch value from EEPROM, and format it for decimal output

	text::stringOut(devLCD, partialRefuelFuncNames, screenCursor[(unsigned int)(menuLevel)]); // print trip function name at top left
	text::clearEOL(devLCD);

	text::gotoXY(devLCD, 0, 1); // go to next line
	text::stringOut(devLCD, pBuff); // print value
	text::stringOut(devLCD, PSTR(" \xfbgal\xfcL\xfd*1K"));
	text::clearEOL(devLCD);

}

void doPartialFuelSelect(void)
{

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 0: // go edit partial refuel value
			cursor::moveAbsolute(settingScreenIdx, pRefuelSizeIdx - eePtrSettingsStart);
			break;

		default:
			noSupport();
			break;

	}

}

void doPartialFuelLongSelect(void)
{

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 1: // reset partial refuel quantity
			EEPROM::writeVal(pRefuelSizeIdx, 0);
			printStatusMessage(PSTR("PartialFuel RST"));
			menuLevel = mainScreenIdx;
			break;

		case 2: // tank trip reset
			doTripResetTank();
			menuLevel = mainScreenIdx;
			break;

		default:
			doPartialFuelSelect();
			break;

	}

}

void doGoPartialRefuel(void)
{

	cursor::moveAbsolute(partialRefuelScreenIdx, 0);

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

		screenTripValue = (uint8_t)(EEPROM::readVal(b + eePtrScreensStart));
		screenFunctionValue = (uint8_t)(EEPROM::readVal(b + eePtrScreensStart + 1));

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

			tripIdx = (uint8_t)(EEPROM::readVal(eePtrScreensStart + (i + x) * 2));
			calcIdx = (uint8_t)(EEPROM::readVal(eePtrScreensStart + (i + x) * 2 + 1));

		}

		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, msTripBitPattern); // call main screen function display routine

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

	const char * s;

	if (screenEditDirty & screenEditFlag_dirty) s = seFormatRevertedString;
	else s = seExitScreenEditString;

	screenCursor[(unsigned int)(mainScreenIdx)] = (screenCursor[(unsigned int)(screenEditIdx)] >> 3);
	cursor::screenLevelEntry(s, topScreenLevel);

}

#endif // useScreenEditor
/* EEPROM parameter I/O section prototype */

#ifdef useEEPROMviewer
static void EEPROM::display(void)
{

	init64((union union_64 *)(&s64reg[s64reg2]), (unsigned long)(screenCursor[(unsigned int)(eepromViewIdx)]));
	text::stringOut(devLCD, ull2str(prgmFormatToNumber, mBuff1, 3));
	text::clearEOL(devLCD);
	text::gotoXY(devLCD, 0, 1);
	init64((union union_64 *)(&s64reg[s64reg2]), EEPROM::readVal(screenCursor[(unsigned int)(eepromViewIdx)]));
	text::stringOut(devLCD, ull2str(prgmFormatToNumber, mBuff1, 3));
	text::clearEOL(devLCD);

}

#endif // useEEPROMviewer
static uint8_t EEPROM::powerUpCheck(void)
{

	uint8_t b = 0;
	uint8_t t;

#ifdef forceEEPROMsettingsInit
	if (true)
#else // forceEEPROMsettingsInit
	if (readVal(pSignatureIdx) != newEEPROMsignature)
#endif // forceEEPROMsettingsInit
	{

		b = 1;

		t = pSignatureIdx;
		for (uint8_t x = 0; x < eePtrSettingsEnd; x++) writeVal(t++, pgm_read_dword(&params[(unsigned int)(x)]));

#ifdef useScreenEditor
		t = eePtrScreensStart;
		for (uint8_t x = 0; x < mainScreenDisplayFormatSize; x++) writeVal(t++, (unsigned long)(pgm_read_byte(&mainScreenDisplayFormats[(unsigned int)(x)])));

#endif // useScreenEditor
#ifdef useEEPROMtripStorage
		for (uint8_t x = 0; x < tripAllowedSaveSlotCount; x++) writeVal(getBaseTripPointer(x) + tripSlotSignaturePtr, 0);

#endif // useEEPROMtripStorage
	}

	initGuino();
	return b;

}

const uint8_t prgmInitMPGuino[] PROGMEM = {
	instrLdRegEEPROM, 0x02, pIdleTimeoutIdx,			// load idle timeout value in seconds
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now timer0 cycles
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 ticks
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vVehicleStopTimeoutIdx,	// store idle timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pEOCtimeoutIdx,				// load EOC timeout value in seconds
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now timer0 cycles
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 ticks
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vEngineOffTimeoutIdx,		// store EOC timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pMinGoodSpeedidx,			// fetch minimum good vehicle speed value in (distance)(* 1000) / hour
	instrLdRegEEPROM, 0x01, pPulsesPerDistanceIdx,		// multiply by distance parameter value in (VSS pulses) / (distance)
	instrMul2by1,										// term is now (VSS pulses)(* 1000) / hour
	instrLdReg, 0x32,									// save denominator term for later
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// fetch timer0 cycles / second term
	instrLdRegConst, 0x01, idxSecondsPerHour,			// multiply it by seconds / hour constant
	instrMul2by1,										// term is now (seconds)(* 1000) / hour
	instrLdRegConst, 0x01, idxDecimalPoint,				// fetch decimal formatting term
	instrMul2by1,										// term is now (timer0 cycles)(* 1000) / (hour)
	instrLdReg, 0x13,									// fetch denominator term calculated above
	instrDiv2by1,										// perform conversion, term is now in timer0 cycles / VSS pulse
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumVSSperiodIdx,		// store maximum good VSS period of timer0 cycles / VSS pulse
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 tick / VSS pulse event
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vDetectVehicleStopIdx,	// store minimum good vehicle speed of timer0 ticks / VSS pulse

	instrLdRegEEPROM, 0x02, pCrankRevPerInjIdx,			// load number of crank revolutions / injector fire event
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now in (crank revolutions)(timer0 cycles) / (fire event)(second)
	instrLdRegByte, 0x01, 60,							// load seconds / minute into register 1
	instrMul2by1,										// term is now in (crank revolutions)(timer0 cycles) / (fire event)(minute)
	instrLdRegEEPROM, 0x01, pMinGoodRPMidx,				// load minimum good engine speed value in crank revolutions / minute
	instrDiv2by1,										// perform conversion, term is now in timer0 tick / fire event
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// store maximum good engine period value in timer0 cycles / fire event
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 tick / fire event
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vDetectEngineOffIdx,		// store minimum good engine speed value in timer0 ticks / fire event

	instrLdRegEEPROM, 0x02, pButtonTimeoutIdx,			// load button press timeout stored parameter
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now timer0 cycles
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 ticks
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vButtonTimeoutIdx,		// store button press timeout timer ticks value

	instrLdRegEEPROM, 0x02, pParkTimeoutIdx,			// load parking timeout stored parameter
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now timer0 cycles
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 ticks
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vParkTimeoutIdx,			// store parking timeout timer ticks value

	instrLdRegEEPROM, 0x02, pActivityTimeoutIdx,		// load activity timeout stored parameter
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now timer0 cycles
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 ticks
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vActivityTimeoutIdx,		// store activity timeout timer ticks value

	instrLdRegConst, 0x01, idxCycles0PerSecond,
	instrLdRegEEPROM, 0x02, pInjectorOpeningTimeIdx,	// fetch injector opening settle time in microseconds
	instrMul2by1,
	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,
	instrDiv2by1,
	instrStRegVolatile, 0x02, vInjectorOpenDelayIdx,	// store injector opening settle time value in cycles
	instrLdReg, 0x32,									// save injector opening settle time value in register 3

	instrLdRegConst, 0x01, idxCycles0PerSecond,
	instrLdRegEEPROM, 0x02, pInjectorClosingTimeIdx,	// fetch injector closing settle time in microseconds
	instrMul2by1,
	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,
	instrDiv2by1,
	instrStRegVolatile, 0x02, vInjectorCloseDelayIdx,	// store injector closing settle time value in cycles
	instrAddYtoX, 0x23,									// add injector opening settle time value to total injector settle time
	instrStRegVolatile, 0x02, vInjectorTotalDelayIdx,	// store injector total settle time value in cycles
	instrLdReg, 0x32,									// save injector total settle time value in register 3
	instrLdRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// load maximum good engine period value in timer0 cycles / fire event
	instrSubYfromX, 0x23,								// subtract total injector settle time from maximum good engine period
	instrStRegVolatile, 0x02, vInjectorValidMaxWidthIdx,	// store maximum valid fuel injector pulse width in timer0 cycles

	instrLdRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,	// fetch injector cycle time in microseconds per US gallon
#ifdef useImperialGallon
	instrTestMetricFlag, metricMode,					// check if MPGuino is in metric mode
	instrBranchIfZset, 10,								// if metric mode set, skip ahead to metric conversion
	instrLdRegConst, 0x01, idxNumerImperialGallon,		// perform conversion from microseconds per liter into microseconds per Imperial gallon
	instrMul2by1,
	instrLdRegConst, 0x01, idxDenomImperialGallon,
	instrDiv2by1,
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
#else // useImperialGallon
	instrTestMetricFlag, metricMode,					// check if MPGuino is in metric mode
	instrBranchIfZclear, 10,							// if metric mode not set, skip ahead to metric conversion
#endif // useImperialGallon
	instrLdRegConst, 0x01, idxDenomVolume,				// perform conversion from microseconds per US gallon into microseconds per liter
	instrMul2by1,
	instrLdRegConst, 0x01, idxNumerVolume,
	instrDiv2by1,
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// get cycles per second constant
	instrMul2by1,										// multiply to get cycles-microseconds per unit volume-second value
	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,	// get microseconds per second constant
	instrDiv2by1,										// convert to get cycles per unit volume value
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vCyclesPerVolumeIdx,		// save cycles per unit volume value

	instrLdRegEEPROM, 0x02, pTankSizeIdx,				// fetch tank size in unit volume * formatting factor
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply to get tank size in cycles * formatting factor
	instrLdRegConst, 0x01, idxDecimalPoint,				// fetch formatting factor
	instrDiv2by1,										// remove formatting factor to get tank size in cycles
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vTankSizeIdx,				// save tank size in cycles

	instrLdRegEEPROM, 0x02, pTankBingoSizeIdx,			// fetch bingo tank size in unit volume
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply to get bingo tank size in cycles * formatting factor
	instrLdRegConst, 0x01, idxDecimalPoint,				// fetch formatting factor
	instrDiv2by1,										// remove formatting factor to get bingo tank size in cycles
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vBingoTankSizeIdx,		// save bingo tank size in cycles

#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,				// fetch bingo tank size in unit volume
	instrLdRegVolatile, 0x01, vCyclesPerVolumeIdx,		// fetch cycles per unit volume value
	instrMul2by1,										// multiply to get bingo tank size in cycles * formatting factor
	instrLdRegConst, 0x01, idxDecimalPoint,				// fetch formatting factor
	instrDiv2by1,										// remove formatting factor to get bingo tank size in cycles
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vPartialRefuelTankSize,	// save bingo tank size in cycles

#endif // usePartialRefuel
#ifdef useDragRaceFunction
	instrLdRegByte, 0x02, 0,							// zero out accel test top speed and estimated engine power
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	instrLdRegEEPROM, 0x02, pCoastdownSamplePeriodIdx,	// coastdown timer ticks value
	instrLdRegConst, 0x01, idxCycles0PerSecond,			// load number of timer0 cycles / second into register 1
	instrMul2by1,										// term is now timer0 cycles
	instrLdRegConst, 0x01, idxCycles0PerTick,			// fetch timer0 cycles / timer0 tick value
	instrDiv2by1,										// perform conversion, term is now in timer0 ticks
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vCoastdownPeriodIdx,		// store coastdown timeout timer ticks value

#endif // useCoastDownCalculator
#ifdef useChryslerMAPCorrection
	instrLxdI, 0,										// process stored EEPROM fuel pressure parameters

	instrLdRegEEPROMindexed, 0x02, pMAPsensorFloorIdx,	// convert pressure sensor voltage floor to equivalent ADC floor value
	instrLdRegConst, 0x01, idxNumerVoltage,
	instrMul2by1,
	instrLdRegConst, 0x01, idxDenomVoltage,
	instrDiv2by1,
	instrStRegVolatileIndexed, 0x02, vAnalogFloor0Idx,	// store it in volatile variable storage

	instrLdRegEEPROMindexed, 0x02, pMAPsensorCeilingIdx,// calculate pressure sensor voltage slope
	instrLdRegEEPROMindexed, 0x01, pMAPsensorFloorIdx,
	instrSubYfromX, 0x21,
	instrLdReg, 0x32,									// save slope difference in register 3

	instrLdRegConst, 0x02, idxDenomVoltage,
	instrLdRegEEPROMindexed, 0x01, pMAPsensorRangeIdx,	// convert voltage slope to pressure value
	instrMul2by1,

	instrLdReg, 0x13,									// restore slope difference in register 3 as denominator
	instrDiv2by1,										// divide pressure value by slope difference
	instrStRegVolatileIndexed, 0x02, vAnalogSlope0Idx,	// store it in volatile variable storage

	instrLdRegEEPROMindexed, 0x02, pMAPsensorOffsetIdx,	// load pressure offset value from EEPROM
	instrStRegVolatileIndexed, 0x02, vAnalogOffset0Idx,	// store it in volatile variable storage

	instrAddIndex, 1,									// loop back to process both MAP and baro pressure sensor values
	instrCmpIndex, 2,									// test whether done processing pressure sensor values
	instrBranchIfLT, 207,								// if not done, loop back

	instrLdRegEEPROM, 0x02, pSysFuelPressureIdx,		// fetch base fuel pressure value in psig * 1000
	instrStRegVolatile, 0x02, vFuelPressureIdx,			// save base fuel pressure value in psig * 1000

	instrLdRegConst, 0x02, idxCorrectionFactor,
	instrStRegVolatile, 0x02, vInjectorCorrectionIdx,	// save initial injector correction index for pressure differential calculation

#endif // useChryslerMAPCorrection
	instrDone											// exit to caller
};

static void EEPROM::initGuino(void) // initialize MPGuino base hardware and basic system settings
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useATmega32U4
	EIMSK &= ~((1 << INT3) | (1 << INT2)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC31) | (1 << ISC30) | (1 << ISC21) | (1 << ISC20)); // set injector sense pin control
	EICRA &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC30 : ISC20));

	EIFR |= ((1 << INTF3) | (1 << INTF2)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT3) | (1 << INT2)); // enable fuel injector sense interrupts

	PCMSK0 |= (1 << PCINT7); // enable port B VSS pin interrupt

	PCICR |= (1 << PCIE0); // enable selected interrupts on port B

	lastPINxState = PINB;

#endif // useATmega32U4
#ifdef useATmega2560
	EIMSK &= ~((1 << INT5) | (1 << INT4)); // disable fuel injector sense interrupts

	EICRB |= ((1 << ISC51) | (1 << ISC50) | (1 << ISC41) | (1 << ISC40)); // set injector sense pin control
	EICRB &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC50 : ISC40));

	EIFR |= ((1 << INTF5) | (1 << INTF4)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT5) | (1 << INT4)); // enable fuel injector sense interrupts

	DIDR2 &= ~(1 << ADC8D); // enable digital input on VSS pin

	PCMSK2 |= (1 << PCINT16); // enable port K VSS pin interrupt

	PCICR |= (1 << PCIE2); // enable selected interrupts on port K

	lastPINxState = PINK;

#endif // useATmega2560
#ifdef useATmega128
	EIMSK &= ~((1 << INT1) | (1 << INT0)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00)); // set injector sense pin control
	EICRA &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC10 : ISC00));

	EIFR |= ((1 << INTF1) | (1 << INTF0)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT1) | (1 << INT0)); // enable fuel injector sense interrupts

	DIDR0 &= ~(1 << ADC0D); // enable digital input on VSS pin

	PCMSK1 |= (1 << PCINT8); // enable port C VSS pin interrupt

	PCICR |= (1 << PCIE1); // enable selected interrupts on port C

	lastPINxState = PINC;

#endif // useATmega128
#ifdef useAnalogInterrupt
#ifdef useATmega32U4
	PRR0 &= ~(1 << PRADC); // turn on ADC module
#endif // useATmega32U4
#ifdef useATmega2560
	PRR0 &= ~(1 << PRADC); // turn on ADC module
#endif // useATmega2560
#ifdef useATmega128
	PRR &= ~(1 << PRADC); // turn on ADC module
#endif // useATmega128

	// disable ADC free-running mode
	ADCSRA &= ~(1 << ADATE);

	// enable ADC
	ADCSRA |= (1 << ADEN);

	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	// clear any pending ADC interrupt
	ADCSRA |= (1 << ADIF);

	// set ADMUX[5] bit to 0
	ADCSRB = 0;

	// set ADC timer frequency to 1/128 of system timer
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

#endif // useAnalogInterrupt
	if (readVal(pPulsesPerDistanceIdx) < 8000) VSSpause = (uint8_t)(readVal(pVSSpauseIdx)); // if pulses per distance index < 8000, it's likely that a mechanical VSS is being used
	else VSSpause = 0; // otherwise, force VSS debounce pause to zero

	if (readVal(pMetricModeIdx)) metricFlag |= metricMode;
	else metricFlag &= ~(metricMode);

	if (readVal(pAlternateFEidx)) metricFlag |= alternateFEmode;
	else metricFlag &= ~(alternateFEmode);

	SWEET64(prgmInitMPGuino, 0); // calculate multiple MPGuino system values for use within code

	SREG = oldSREG; // restore interrupt flag status

#ifdef useWindowTripFilter
	windowTripFilterReset();

#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	doResetBarFEvT();

#endif // useBarFuelEconVsTime
}

static uint8_t EEPROM::writeVal(uint8_t eePtr, unsigned long val)
{

	union union_32 * vee = (union union_32 *) &val;

	unsigned int t;
	uint8_t l;
	uint8_t updateFlag = 0;
	uint8_t eByt;
	uint8_t rByt;

	t = getAddress(eePtr);
	l = (uint8_t)(t & 0x07);
	t >>= 3;

	for (uint8_t x = 0; x < l; x++)
	{

		eByt = eeprom_read_byte((uint8_t *)(t));
		rByt = vee->u8[(unsigned int)(x)];
		if (eByt != rByt)
		{

			updateFlag = 1;
			eeprom_write_byte((uint8_t *)(t), rByt);

		}
		t++;

	}

	return updateFlag;

}

static void EEPROM::readValToString(uint8_t parameterPtr, char * strBuffer)
{

	init64((union union_64 *)(&s64reg[s64reg2]), EEPROM::readVal(parameterPtr));
	ull2str(prgmFormatToNumber, strBuffer, 3);

}

static unsigned long EEPROM::readVal(uint8_t eePtr)
{

	unsigned long val;
	union union_32 * vee = (union union_32 *) &val;

	unsigned int t;
	uint8_t l;

	val = 0;
	t = getAddress(eePtr);
	l = (uint8_t)(t & 0x07);
	t >>= 3;

	for (uint8_t x = 0; x < l; x++)
	{

		vee->u8[(unsigned int)(x)] = eeprom_read_byte((uint8_t *)(t++));

	}

	return val;

}

static unsigned int EEPROM::getAddress(uint8_t eePtr)
{

	unsigned int t;
	uint8_t l;

	if (eePtr >= eePtrEnd) eePtr = pSignatureIdx;

	if (eePtr < eePtrSettingsEnd)
	{

		t = (unsigned int)(pgm_read_byte(&paramAddrs[eePtr]));
		l = pgm_read_byte(&paramAddrs[eePtr + 1]);
		l -= (uint8_t)(t);

#ifdef useScreenEditor
	}
	else if ((eePtr >= eePtrScreensStart) && (eePtr < eePtrScreensEnd))
	{

		eePtr -= eePtrScreensStart;
		l = 2;
		t = eePtr + eeAdrScreensStart;

#endif // useScreenEditor
#ifdef useEEPROMtripStorage
	}
	else if ((eePtr >= eePtrSavedTripsStart) && (eePtr < eePtrSavedTripsEnd))
	{

		eePtr -= eePtrSavedTripsStart;
		l = (uint8_t)(eePtr / tripSlotPtrSize);
		eePtr -= (unsigned int)(l) * (unsigned int)(tripSlotPtrSize);

		t = eeAdrSavedTripsStart + (unsigned int)(l) * (unsigned int)(tripSlotAdrSize);

		if ((eePtr > 0) && (eePtr < tripSlotSignaturePtr))
		{

			l = 4;
			t += 4 * (unsigned int)(eePtr - 1);
			t++;

		}
		else
		{

			if (eePtr > 0) t += (unsigned int)(tripSlotAdrSize - 1);
			l = 1;

		}
#endif // useEEPROMtripStorage

	}

	t <<= 3;
	t += (unsigned int)(l);

	return t;

}

// this function is needed since there is no way to perform an atomic bit change of an SRAM byte value
// most MPGuino variables that are shared between main program and interrupt handlers should not need to
//    be treated as atomic (!) because only one side or the other is supposed to change said variables
// however, status flag registers are obviously an exception, and status flag changes are common
//    enough to warrant an explicit function definition
static void EEPROM::changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	flagRegister = (flagRegister & ~(maskAND)) | (maskOR); // go perform atomic status flag change

	SREG = oldSREG; // restore interrupt flag status

}

#ifdef useWindowTripFilter
void windowTripFilterReset(void)
{

	wtpCurrentIdx = 0;

	for (uint8_t x = 0; x < windowTripFilterSize; x++) tripArray[(unsigned int)(windowTripFilterIdx + x)].reset();

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

const uint8_t prgmJSON23tankFE[] PROGMEM = {
	instrLdRegByte, 0x01, 2,							// load a 2 into register 1
	instrMul2by1,										// multiply results
	instrLdRegByte, 0x01, 3,							// load a 3 into register 1
	instrDiv2by1,										// divide results
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

const uint8_t prgmJSON53tankFE[] PROGMEM = {
	instrLdRegByte, 0x01, 5,							// load a 5 into register 1
	instrMul2by1,										// multiply results
	instrLdRegByte, 0x01, 3,							// load a 3 into register 1
	instrDiv2by1,										// divide results
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

const uint8_t prgmJSONminValue[] PROGMEM = {
	instrCmpReg, 0x12,
	instrBranchIfLTorE, 2,								// if register 2 contents are less than or equal to register 1 contents, skip
	instrLdReg, 0x21,									// load register 2 with contents of register 1
	instrDone											// exit to caller
};

void doOutputNumberJSON(const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64(sched, tripIdx);
	doOutputNumberJSON(decimalPlaces, labelStr);

}

void doOutputNumberJSON(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalPlaces, const char * labelStr)
{

	doCalculate(tripIdx, calcIdx);
	doOutputNumberJSON(decimalPlaces, labelStr);

}

void doOutputNumberJSON(unsigned long an, uint8_t decimalPlaces, const char * labelStr)
{

	init64((union union_64 *)(&s64reg[s64reg2]), an);
	doOutputNumberJSON(decimalPlaces, labelStr);

}

void doOutputNumberJSON(union union_64 * an, uint8_t decimalPlaces, const char * labelStr)
{

	copy64((union union_64 *)(&s64reg[s64reg2]), an);
	doOutputNumberJSON(decimalPlaces, labelStr);

}

void doOutputNumberJSON(uint8_t decimalPlaces, const char * labelStr)
{

	text::stringOut(devLogSerial, formatDecimal(prgmRoundOffNumber, mBuff1, 0, decimalPlaces, dfOverflow9s)); // output the number part
	text::stringOut(devLogSerial, labelStr); // output the label part

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

		EEPROM::changeBitFlags(timer0Status, t0sOutputJSON, 0); // clear JSON subtitle change timer command

		if !(--subtitleCount1) subtitleCount1 = 2;
#ifdef useDragRaceFunction
		if !(--subtitleCount2) subtitleCount2 = 3;
#endif // useDragRaceFunction

	}

	unsigned long reserveQuantity = SWEET64(prgmReserveFuelRemaining, 0);

	unsigned long tankSize = EEPROM::readVal(pTankSizeIdx);

	text::stringOut(devLogSerial, PSTR("\n\n[")); // begin JSON payload

	// first graph, fuel
	text::stringOut(devLogSerial, PSTR("{\"title\":\""));

	if (reserveQuantity == 0) text::stringOut(devLogSerial, PSTR("fumes remaining, ")); // fumes left
	else doOutputNumberJSON(prgmReserveFuelRemaining, 0, 3, PSTR(" \xfbgal\xfcL\xfd remaining, ")); // reserve remaining fuel left

	doOutputNumberJSON(prgmBingoFuelRemaining, 0, 3, PSTR(" \xfbgal\xfcL\xfd e-reserve\",\n")); // bingo remaining fuel left
	//  doOutputNumberJSON(instantIdx, tEngineSpeed, 0, PSTR(" RPM\",\n")); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(devLogSerial, PSTR("\"subtitle\":\"fuel used: "));

			doOutputNumberJSON(currentIdx, tFuelUsed, 3, PSTR(" trip \xfbgal\xfcL\xfd; ")); // current trip fuel used
			doOutputNumberJSON(tankIdx, tFuelUsed, 3, PSTR(" tank \xfbgal\xfcL\xfd; ")); // tank trip fuel used
			doOutputNumberJSON(instantIdx, tFuelRate, 3, PSTR(" using \xfbgal\xfcL\xfd/hr\",\n")); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(devLogSerial, PSTR("\"subtitle\":\"eco stats: "));
#ifdef trackIdleEOCdata
			doOutputNumberJSON(eocIdleTankIdx, tFuelUsed, 3, PSTR(" \xfbgal\xfcL\xfd used@idle, "));
			doOutputNumberJSON(eocIdleTankIdx, tDistance, 1, PSTR(" \xfbmi\xfckm\xfd fuel cut\",\n"));
#else // trackIdleEOCdata
			if (reserveQuantity == 0) text::stringOut(devLogSerial, PSTR("fumes remaining, ")); // fumes left
			else doOutputNumberJSON(prgmReserveFuelRemaining, 0, 3, PSTR(" \xfbgal\xfcL\xfd remaining, ")); // reserve remaining fuel left
			doOutputNumberJSON(prgmBingoFuelRemaining, 0, 3, PSTR(" \xfbgal\xfcL\xfd e-reserve\",\n")); // bingo remaining fuel left
#endif // trackIdleEOCdata

			break;

	}

	reserveQuantity = EEPROM::readVal(pTankBingoSizeIdx);

	text::stringOut(devLogSerial, PSTR("\"ranges\":[")); //ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	doOutputNumberJSON(tankSize, 3, PSTR(",")); // largest, full tank size (e.g, 13.8 g)
	doOutputNumberJSON(tankSize - reserveQuantity, 3, PSTR(",")); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	doOutputNumberJSON(reserveQuantity, 3, PSTR("],\n")); // reserve amount (e.g. 2.2g)

	text::stringOut(devLogSerial, PSTR("\"measures\":["));
	doOutputNumberJSON(prgmReserveFuelRemaining, 0, 3, PSTR(",")); // reserve remaining fuel left
	doOutputNumberJSON(prgmRemainingFuel, 0, 3, PSTR("],\n")); // total remaining fuel left

	text::stringOut(devLogSerial, PSTR("\"markers\":["));
	doOutputNumberJSON(instantIdx, tFuelRate, 3, PSTR("]},\n\n")); // current rate of fuel burn in units/time

	// second graph, distance
	text::stringOut(devLogSerial, PSTR("{\"title\":\""));
	doOutputNumberJSON(prgmReserveDistanceToEmpty, tankIdx, 2, PSTR("\xfbmi\xfckm\xfd to e, ")); // distance to bingo
	doOutputNumberJSON(prgmBingoDistanceToEmpty, tankIdx, 2, PSTR(" \xfbmi\xfckm\xfd e-reserve\",\n")); // distance to fully empty tank from bingo

	switch (subtitleCount1)
	{

		case 2:
			//   text::stringOut(devLogSerial, PSTR("\"subtitle\":\"trip/tank distance: "));
			//   doOutputNumberJSON(currentIdx, tDistance, 2, PSTR(" \xfbmi\xfckm\xfd/")); // current trip distance
			//   doOutputNumberJSON(tankIdx, tDistance, 2, PSTR(" \xfbmi\xfckm\xfd\",\n")); // current trip distance

			text::stringOut(devLogSerial, PSTR("\"subtitle\":\""));
			doOutputNumberJSON(currentIdx, tDistance, 2, PSTR(" \xfbmi\xfckm\xfd trip distance, ")); // current trip distance
			doOutputNumberJSON(tankIdx, tDistance, 2, PSTR(" \xfbmi\xfckm\xfd tank distance\",\n")); // current trip distance

			break;

		case 1:
			text::stringOut(devLogSerial, PSTR("\"subtitle\":\""));
			doOutputNumberJSON(prgmReserveDistance, tankIdx, 2, PSTR(" \xfbmi\xfckm\xfd safe range, ")); // reserve range
			doOutputNumberJSON(prgmRangeDistance, tankIdx, 2, PSTR(" \xfbmi\xfckm\xfd dry range\",\n")); // distance to fully empty tank

			break;

	}

	text::stringOut(devLogSerial, PSTR("\"ranges\":["));
	doOutputNumberJSON(prgmRangeDistance, tankIdx, 1, PSTR(",")); // maximum range
	doOutputNumberJSON(prgmReserveDistance, tankIdx, 1, PSTR(",")); // range 2, safe range
	doOutputNumberJSON(prgmFindHalfReserveRange, tankIdx, 1, PSTR("],\n")); // range 3, half of safe range

	text::stringOut(devLogSerial, PSTR("\"measures\":["));
	doOutputNumberJSON(prgmBingoDistance, tankIdx, 2, PSTR("],\n")); // shows miles of e-reserve in bar form

	text::stringOut(devLogSerial, PSTR("\"markers\":["));
	doOutputNumberJSON(tankIdx, tDistanceToEmpty, 2, PSTR("]},\n\n")); // line is distance to empty

	// third graph, econ
	text::stringOut(devLogSerial, PSTR("{\"title\":\"")); //\xfbgal\xfcL\xfd/

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

		text::stringOut(devLogSerial, findStr(JSONaccelTestStateMsgs, accelTestState));
		text::stringOut(devLogSerial, PSTR(" ...\",\n"));

	}
	else // else not racing or waiting, go to normal
	{

		text::stringOut(devLogSerial, PSTR("trip/tank/inst: ")); //\xfbgal\xfcL\xfd/
		doOutputNumberJSON(currentIdx, tFuelEcon, 3, PSTR("/ ")); // current fuel economy
		doOutputNumberJSON(tankIdx, tFuelEcon, 3, PSTR("/ ")); // tank fuel economy
		reserveQuantity = doCalculate(instantIdx, tFuelEcon);
		if (reserveQuantity > 9999999) text::stringOut(devLogSerial, PSTR("infinite\",\n"));
		else doOutputNumberJSON(instantIdx, tFuelEcon, 3, PSTR("\",\n")); // instantaneous fuel economy

	}

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(devLogSerial, PSTR("\"subtitle\":\"accel time: 0-"));
			doOutputNumberJSON(EEPROM::readVal(pDragSpeedIdx) / 2, 0, PSTR("/"));
			doOutputNumberJSON(EEPROM::readVal(pDragSpeedIdx), 0, PSTR(": "));
			doOutputNumberJSON(dragHalfSpeedIdx, tAccelTestTime, 2, PSTR("/")); // 0-30 time
			doOutputNumberJSON(dragFullSpeedIdx, tAccelTestTime, 2, PSTR(", ")); // 0-60 time
			doOutputNumberJSON(dragDistanceIdx, tDistance * 1, 2, PSTR("\xfbmi\xfckm\xfd in ")); // trap distance
			doOutputNumberJSON(dragDistanceIdx, tAccelTestTime, 2, PSTR(" @")); // trap time
			doOutputNumberJSON(dragDistanceIdx, tSpeed, 3, PSTR("\xfbmph\xfckph\xfd; ")); // trap speed
			doOutputNumberJSON(dragHalfSpeedIdx, tEstimatedEnginePower, 0, PSTR("\xfbhp\xfckW\xfd @")); // estimated engine power
			doOutputNumberJSON(dragHalfSpeedIdx, tDragSpeed, 0, PSTR("\xfbmph\xfckph\xfd\",\n")); // max speed

			break;

		case 2:
			text::stringOut(devLogSerial, PSTR("\"subtitle\":\"accel fuel: "));

			doOutputNumberJSON(dragHalfSpeedIdx, tFuelUsed, 3, PSTR("\xfbga\xfcL\xfd to ")); // 0-half fuel
			doOutputNumberJSON(EEPROM::readVal(pDragSpeedIdx) / 2, 0, PSTR(", ")); // 0-half speed
			doOutputNumberJSON(dragFullSpeedIdx, tFuelUsed, 3, PSTR("\xfbga\xfcL\xfd to ")); // 0-full fuel
			doOutputNumberJSON(EEPROM::readVal(pDragSpeedIdx), 0, PSTR(", ")); // 0-full speed
			doOutputNumberJSON(dragDistanceIdx, tFuelUsed, 3, PSTR("\xfbga\xfcL\xfd to ")); //trap fuel
			doOutputNumberJSON(dragDistanceIdx,  tDistance * 1, 2, PSTR("\xfbmi\xfckm\xfd\",\n")); // "to [trap distance]"  \xfbft\xfcm\xfd;

			break;

		case 1:
			text::stringOut(devLogSerial, PSTR("\"subtitle\":\"accel fuel: "));

			doOutputNumberJSON(dragHalfSpeedIdx, tFuelEcon, 3, PSTR("\xfbmpg\xfcL100\xfd to ")); // 0-30 mpg
			doOutputNumberJSON(EEPROM::readVal(pDragSpeedIdx) / 2, 0, PSTR(", ")); // 0-half speed
			doOutputNumberJSON(dragFullSpeedIdx, tFuelEcon, 3, PSTR("\xfbmpg\xfcL100\xfd to ")); // 0-60 mpg
			doOutputNumberJSON(EEPROM::readVal(pDragSpeedIdx), 0, PSTR(", ")); // 0-60 speed
			doOutputNumberJSON(dragDistanceIdx, tFuelEcon, 3, PSTR("\xfbmpg\xfcL100\xfd to ")); //trap mpg
			doOutputNumberJSON(dragDistanceIdx,  tDistance * 1, 2, PSTR("\xfbmi\xfckm\xfd\",\n")); // "to [trap distance]"  \xfbft\xfcm\xfd;

			break;

	}

#else // useDragRaceFunction
	text::stringOut(devLogSerial, PSTR("\"subtitle\":\"[this space intentionally left blank]\",\n"));

#endif // useDragRaceFunction
	text::stringOut(devLogSerial, PSTR("\"ranges\":[18,24,"));

	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	doOutputNumberJSON(min(max(40000,  doCalculate(instantIdx, tFuelEcon)), 999000), 3, PSTR("],\n"));

	text::stringOut(devLogSerial, PSTR("\"measures\":["));
	doOutputNumberJSON(currentIdx, tFuelEcon, 3, PSTR(",")); // current fuel economy
	doOutputNumberJSON(tankIdx, tFuelEcon, 3, PSTR("],\n")); // tank fuel economy

	text::stringOut(devLogSerial, PSTR("\"markers\":["));

	// instantaneous fuel economy, do not let scale exceed 999
	doOutputNumberJSON(min(999000, doCalculate(instantIdx, tFuelEcon)), 3, PSTR("]"));

	text::stringOut(devLogSerial, PSTR("\}]\n\r")); // end JSON payload, and go trigger read on python.script

} // end sendJson function

#endif // useJSONoutput
#ifdef useSerialDataLogging
const uint8_t dataLogInstr[][2] PROGMEM = {
	 {lblInstantIdx,	 	tFuelEcon}				// average fuel economy  for the past loop
	,{lblInstantIdx,	 	tSpeed}					// average vehicle speed for the past loop
	,{lblInstantIdx,	 	tInjectorOpenTime}		// fuel injector raw open time for the past loop
	,{lblInstantIdx,	 	tInjectorPulseCount}	// fuel injector pulse count for the past loop
	,{lblInstantIdx,	 	tVSSpulseCount}			// vss pulse count for the past loop
};

const uint8_t dLIcount = (sizeof(dataLogInstr) / sizeof(uint8_t));

void doOutputDataLog(void)
{

	calcFuncObj thisCalcFuncObj;
	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		tripIdx = pgm_read_byte(&dataLogInstr[(unsigned int)(x)][0]);
		calcIdx = pgm_read_byte(&dataLogInstr[(unsigned int)(x)][1]);

		thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx);

		doCalculate(thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);

		text::stringOut(devLogSerial, formatDecimal(prgmRoundOffNumber, mBuff1, 0, thisCalcFuncObj.decimalPlaces, dfOverflow9s));
		text::charOut(devLogSerial, c);

	}

}

#endif // useSerialDataLogging
/* Input number editor section */

static const char numberEditSave[] PROGMEM = {
	" OK"
};

static const char numberEditCancel[] PROGMEM = {
	" XX"
};

static uint8_t numberEdit::displayHandler(uint8_t cmd, uint8_t cursorPos)
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

				c = numberEditObj.neBuffer[(unsigned int)(x)]; // fetch a character from buffer

				if (x > cursorPos) // if current position is past cursor position, just make sure leading spaces get turned into zero digits
				{

					if (c == ' ') numberEditObj.neBuffer[(unsigned int)(x)] = '0';

				}
				else // if current position is before or at cursor position, ensure leading spaces up to either cursor position or to first non-zero digit
				{

					if ((c == ' ') || (c == '0')) numberEditObj.neBuffer[(unsigned int)(x)] = k; // if character is either a space or a zero digit, it may be leading so change it
					else k = '0'; // character is a non-zero digit, so change leading character value to a zero digit

				}

			}

			if (numberEditObj.neBuffer[9] == ' ') numberEditObj.neBuffer[9] = '0'; // ensure at least one digit

		case menuOutputDisplayIdx:
			blinkFlag = (timer0Status & t0sShowCursor);
			numberEditObj.numberEditMethod(nesOutputUpperScreen); // print initial screen information and position cursor

			uint8_t c = numberEditObj.neBuffer[(unsigned int)(cursorPos)]; // save existing character
			if ((cursorPos < 10) && (blinkFlag)) numberEditObj.neBuffer[(unsigned int)(cursorPos)] = '_'; // replace character with an underscore
			text::stringOut(devLCD, (char *)(numberEditObj.neBuffer)); // print number
			numberEditObj.neBuffer[(unsigned int)(cursorPos)] = c; // restore existing character

			text::blinkMode(devLCD, ((cursorPos == 10) && (blinkFlag)));
			text::stringOut(devLCD, numberEditSave);
			text::blinkMode(devLCD, ((cursorPos == 11) && (blinkFlag)));
			text::stringOut(devLCD, numberEditCancel);
			text::blinkMode(devLCD, 0);
			break;

		default:
			break;

	}

	return retVal;

}

static void numberEdit::goEditNumber(void)
{

	numberEditObj.callingScreenLevel = menuLevel;

	cursor::moveAbsolute(numberEditScreenIdx, 255);

}

static void numberEdit::findLeft(void)
{

	screenCursor[(unsigned int)(numberEditScreenIdx)] = 9;

	// do a nice thing and put the edit cursor at the first non zero number
	for (uint8_t x = 9; x < 10; x--) if (numberEditObj.neBuffer[(unsigned int)(x)] != ' ') screenCursor[(unsigned int)(numberEditScreenIdx)] = x;

}

static void numberEdit::findRight(void)
{

	screenCursor[(unsigned int)(numberEditScreenIdx)] = 9;

}

static void numberEdit::revert(void)
{

	numberEditObj.numberEditMethod(nesFetchPowerUpValue);
	numberEditObj.numberEditMethod(nesLoadValue);

}

static void numberEdit::readMaxValue(void)
{

	numberEditObj.numberEditMethod(nesFetchMaxValue);
	numberEditObj.numberEditMethod(nesLoadValue);

}

static void numberEdit::readMinValue(void)
{

	numberEditObj.numberEditMethod(nesFetchMinValue);
	numberEditObj.numberEditMethod(nesLoadValue);

}

static void numberEdit::changeDigitUp(void)
{

	changeDigit(1);

}

static void numberEdit::changeDigitDown(void)
{

	changeDigit(255);

}

static void numberEdit::changeDigit(uint8_t digitDir)
{

	uint8_t cp = screenCursor[(unsigned int)(numberEditScreenIdx)];
	uint8_t c = '0';
	uint8_t d = ' ';

	uint8_t w;

	switch (cp)
	{
		case 10:
			saveEdit();
			break;

		case 11:
			cancelEdit();
			break;

		default:
			w = numberEditObj.neBuffer[(unsigned int)(cp)]; // fetch digit from stored numeric string representing parameter to be changed

			if (w == ' ') w = '0'; // if this is a leading space, use 0 as working digit
			w += digitDir; // adjust working digit
			if (w > '9') w = '0'; // handle working digit rollover
			if (w < '0') w = '9'; // handle working digit rollover

			numberEditObj.neBuffer[(unsigned int)(cp)] = w;

			w = numberEditObj.numberEditMethod(nesCheckMaxValue);

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

			numberEditObj.numberEditMethod(nesOnChange);

			break;

	}

}

static void numberEdit::saveEdit(void)
{

	uint8_t cp = screenCursor[(unsigned int)(numberEditScreenIdx)];
	uint8_t retVal;
	const char * str;

	switch (cp)
	{

		case 10:
			retVal = numberEditObj.numberEditMethod(nesSaveNumber);

			str = findStr(numberEditObj.neStatusMessage, retVal);
			cursor::screenLevelEntry(str, numberEditObj.callingScreenLevel);
			break;

		case 11:
			cancelEdit();
			break;

		default:
			cp = 10;
			screenCursor[(unsigned int)(numberEditScreenIdx)] = cp;
			displayHandler(menuCursorUpdateIdx, cp);
			break;

	}

}

static void numberEdit::cancelEdit(void)
{

	uint8_t cp = screenCursor[(unsigned int)(numberEditScreenIdx)];
	const char * str;

	if (cp != 11)
	{
		cp = 11;
		screenCursor[(unsigned int)(numberEditScreenIdx)] = cp;
		displayHandler(menuCursorUpdateIdx, cp);
	}
	else
	{

		str = findStr(numberEditObj.neStatusMessage, 2);
		cursor::screenLevelEntry(str, numberEditObj.callingScreenLevel);

	}

}

/* MPGuino parameter settings edit section */

static const char pseStatusMessages[] PROGMEM = {
	"Param Unchanged\0"
	"Param Changed\0"
	"Param Reverted\0"
};

#ifdef useCalculatedFuelFactor
static const uint8_t prgmCalculateFuelFactor[] PROGMEM = {
	instrLdRegConst, 0x02, idxCorrectionFactor,			// obtain reference correction factor
	instrLdRegEEPROM, 0x01, pSysFuelPressureIdx,		// multiply by this vehicle's stored fuel system absolute pressure
	instrMul2by1,
	instrLdRegEEPROM, 0x01, pRefFuelPressureIdx,		// divide by the fuel injector reference absolute pressure
	instrDiv2by1,
	instrIsqrt, 0x02,									// perform square root on result
	instrLdRegEEPROM, 0x01, pInjectorCountIdx,			// get stored number of fuel injectors for this engine
	instrMul2by1,
	instrLdRegEEPROM, 0x01, pInjectorSizeIdx,			// get injector size in cc/minute * decimal formatting factor (L/min * decimal formatting factor * 1000)
	instrMul2by1,
	instrLdReg, 0x32,									// save denominator term for later

	instrLdRegByte, 0x02, 60,							// load seconds per minute into register 2
	instrLdRegConst, 0x01, idxMicroSecondsPerSecond,	// load microseconds per second into register 1
	instrMul2by1,
	instrLdRegConst, 0x01, idxOneThousand,				// load number of cc's per liter into register 1
	instrMul2by1,										// factor into numerator term
	instrLdRegConst, 0x01, idxDecimalPoint,				// set numerator up to cancel reference correction factor in denominator
	instrMul2by1,
	instrLdRegConst, 0x01, idxCorrectionFactor,			// set numerator up to cancel reference correction factor in denominator
	instrMul2by1,

	instrLdReg, 0x13,									// transfer denominator back to register 1
	instrDiv2by1,										// perform comversion for injector flow
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdRegConst, 0x01, idxNumerVolume,				// set up denominator to convert injector flow in liters to US gallons
	instrMul2by1,
	instrLdRegConst, 0x01, idxDenomVolume,				// set up numerator to convert injector flow in liters to US gallons
	instrDiv2by1,										// perform comversion for injector flow
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrStRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,
	instrDone
};

#endif // useCalculatedFuelFactor
static const uint8_t prgmDoEEPROMmetricConversion[] PROGMEM = {
	instrLxdI, 0,										// zero the trip index register

	instrLdRegEEPROMindirect, 0x02,						// load an EEPROM parameter, indexed into the convIdx array
	instrLdRegNumer, 0x01,								// load numerator into register 1
	instrMul2by1,										// multiply EEPROM parameter value by numerator
	instrLdRegDenom, 0x01,								// load denominator into register 1
	instrDiv2by1,										// divide EEPROM parameter value by denominator
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegEEPROMindirect, 0x02,						// store converted EEPROM parameter
	instrAddIndex, 1,									// bump conversion index stored in trip index register
	instrCmpIndex, convSize,							// finished processing list of EEPROM parameters?
	instrBranchIfLT, 238,								// if not, loop back

	instrDone											// return to caller
};

static const uint8_t prgmCompareWithMaximumParamValue[] PROGMEM = {
	instrLdReg, 0x32,									// save converted working number for comparison
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead

	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

	instrCmpReg, 0x32,									// compare working number to maximum parameter value
	instrBranchIfLTorE, 4,								// if maximum parameter value <= working number, signal "overflow"
	instrLdRegByte, 0x02, 0,							// signal that working number is good
	instrDone,											// return to caller

	instrLdRegByte, 0x02, 255,							// signal that working number is bad
	instrDone											// return to caller
};

static const uint8_t prgmFindMaximumParamValue[] PROGMEM = {
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead

	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrSubYfromX, 0x21,								// subtract 1 from register 2 to obtain final maximum parameter value

	instrJump, tFormatToNumber							// go call prgmFormatToNumber to perform actual formatting
};

static const uint8_t prgmFetchMaximumParamValue[] PROGMEM = {
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead

	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrSubYfromX, 0x21,								// subtract 1 from register 2 to obtain final maximum parameter value

	instrDone											// return to caller
};

static uint8_t settings::numberEditorFunc(uint8_t cmd)
{

	uint8_t retVal = 0;
	uint8_t parameterPtr = screenCursor[(unsigned int)(settingScreenIdx)] + eePtrSettingsStart;

	switch (cmd)
	{

		case nesLoadInitial:
			init64((union union_64 *)(&s64reg[s64reg2]), EEPROM::readVal(parameterPtr));

		case nesLoadValue:
			ull2str(prgmFormatToNumber, numberEditObj.neBuffer, 3);
			numberEdit::findLeft();

		case nesOnChange:
#ifdef useLegacyLCD
			if (parameterPtr == pContrastIdx) LCD::setContrast((uint8_t)(str2ull(numberEditObj.neBuffer))); // adjust contrast dynamically
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
			if (parameterPtr == pLCDcolorIdx) LCD::setRGBcolor((uint8_t)(str2ull(numberEditObj.neBuffer))); // adjust backlight color dynamically
#endif // useAdafruitRGBLCDshield
			break;

		case nesCheckMaxValue:
			str2ull(numberEditObj.neBuffer); // convert parameter buffer string into uint64_t
			retVal = (uint8_t)(SWEET64(prgmCompareWithMaximumParamValue, pgm_read_byte(&paramsLength[(unsigned int)(parameterPtr)])));
			break;

		case nesFetchMinValue:
			init64((union union_64 *)(&s64reg[s64reg2]), 0);
			break;

		case nesFetchMaxValue:
			SWEET64(prgmFetchMaximumParamValue, pgm_read_byte(&paramsLength[(unsigned int)(parameterPtr)]));
			break;

		case nesFetchPowerUpValue:
			init64((union union_64 *)(&s64reg[s64reg2]), pgm_read_dword(&params[(unsigned int)(parameterPtr)]));
			break;

		case nesSaveNumber:
			retVal = EEPROM::writeVal(parameterPtr, str2ull(numberEditObj.neBuffer));

			if (retVal) // if the setting has changed
			{

				if (parameterPtr == pMetricModeIdx) SWEET64(prgmDoEEPROMmetricConversion, 0); // if metric flag has changed

#ifdef useBarFuelEconVsSpeed
				if ((parameterPtr == pBarLowSpeedCutoffIdx) || (parameterPtr == pBarSpeedQuantumIdx)) doResetBarFEvS();

#endif // useBarFuelEconVsSpeed
#ifdef useCalculatedFuelFactor
				// if fuel pressure, reference pressure, injector count, or injector size changed
				if ((parameterPtr == pSysFuelPressureIdx) || (parameterPtr == pRefFuelPressureIdx) || (parameterPtr == pInjectorCountIdx) || (parameterPtr == pInjectorSizeIdx))
					SWEET64(prgmCalculateFuelFactor, 0); // calculate and store microseconds per US gallon factor

#endif // useCalculatedFuelFactor
				EEPROM::initGuino(); // reconfigure system based on changed settings

				retVal = 1;

			}

			break;

		case nesOutputUpperScreen:
			text::stringOut(devLCD, parmLabels, screenCursor[(unsigned int)(settingScreenIdx)]); // print parameter name at top left
			text::clearEOL(devLCD);
			text::gotoXY(devLCD, 0, 1); // go to next line
			break;

		default:
			break;

	}

	return retVal;

}

static uint8_t settings::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			numberEditObj.callingScreenLevel = menuLevel;
			numberEditObj.neBuffer = pBuff;
			numberEditObj.neStatusMessage = pseStatusMessages;
			numberEditObj.numberEditMethod = numberEditorFunc;

		case menuCursorUpdateIdx:
			numberEditObj.numberEditMethod(nesLoadInitial);

		case menuOutputDisplayIdx:
			numberEditObj.numberEditMethod(nesOutputUpperScreen); // print initial screen information and position cursor
			text::stringOut(devLCD, (char *)(numberEditObj.neBuffer));
			text::clearEOL(devLCD);
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
const uint8_t prgmCalculateOutputPinValue[] PROGMEM = {
	instrCmpIndex, 2,									// is a valid expansion output pin number being requested?
	instrBranchIfGTorE, 13,								// skip to output a zero if not
	instrLxdIEEPROMindexed, pExpansionPin1Mode,			// load the indexed stored parameter index for the expansion output pin setting
	instrTestIndex,										// test pin mode value for zero
	instrBranchIfZero, 4,								// exit out if pin mode is zero
	instrCmpIndex, 4,									// test if pin mode is "fuel economy comparison between instant and whatever"
	instrBranchIfLTorE,	48,								// if so, skip ahead
	instrCmpIndex, 6,									// test if pin mode is analog output tank quantity or quantity remaining
	instrBranchIfLTorE, 4,								// if so, skip ahead

	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone,											// exit to caller

	instrLdRegTripVar, 0x01, rvInjOpenCycleIdx, tankIdx,	// fetch tank consumed amount in injector open cycles
	instrLdRegVolatile, 0x02, vTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrCmpReg, 0x12,									// compare calculated tank size to tank consumed amount
	instrBranchIfGT, 8,									// if tank size > tank consumed amount, skip ahead

	instrCmpIndex, 6,									// calculating remaining value?
	instrBranchIfE, 237,								// go zero out result if so
	instrLdRegByte, 0x02, 255,							// max out result
	instrDone,											// return to caller

	instrCmpIndex, 5,									// calculating consumed value?
	instrBranchIfE, 4,									// skip ahead, if so
	instrSubYfromX, 0x21,								// subtract tank injector time in cycles from calculated tank size in cycles to get remaining fuel in cycles in register 2
	instrSkip, 2,										// go perform calculation
	instrLdReg, 0x21,									// move tank quantity in injector open cycles to register 2 for now
	instrLdRegByte, 0x01, 255,							// use 255 as maximum PWM output value
	instrMul2by1,										// do multiply to get PWM output value for fuel value in cycles
	instrLdRegVolatile, 0x01, vTankSizeIdx,				// fetch calculated tank size in injector open cycles as denominator
	instrDiv2by1,										// convert fuel value in cycles to fuel PWM outout value
	instrAddYtoX, 0x24,									// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone,											// exit to caller

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
	instrLdRegTripVar, 0x03, rvInjOpenCycleIdx, tankIdx,// fetch tank (accum quantity)
	instrLdRegTripVar, 0x02, rvVSSpulseIdx, tankIdx,	// fetch tank (accum distance)
	instrSkip, 8,										// skip ahead
	instrLdRegTripVar, 0x03, rvInjOpenCycleIdx, currentIdx,	// fetch current (accum quantity)
	instrLdRegTripVar, 0x02, rvVSSpulseIdx, currentIdx,	// fetch current (accum distance)
	instrLdRegTripVar, 0x01, rvInjOpenCycleIdx, instantIdx,	// fetch (inst quantity)
	instrMul2by1,										// calculate (inst quantity) * (accum distance) as right term
	instrSwapReg, 0x23,									// swap right term and (accum quantity) values
	instrLdRegTripVar, 0x01, rvVSSpulseIdx, instantIdx,	// fetch (inst distance)
	instrMul2by1,										// calculate (inst distance) * (accum quantity) as left term
	instrCmpIndex, 2,									// testing (inst distance) * (current quantity) <= (inst quantity) * (current distance)?
	instrBranchIfEqual, 6,								// if so, skip ahead
	instrCmpIndex, 4,									// testing (inst distance) * (tank quantity) <= (inst quantity) * (tank distance)?
	instrBranchIfEqual, 2,								// if so, skip ahead
	instrSwapReg, 0x23,									// swap left and right terms
	instrCmpReg, 0x32,									// compare left term to right term
	instrBranchIfLTorE, 4,								// if left term <= right term, go zero out and return

	instrLdRegByte, 0x02, 255,							// max out result
	instrDone,											// return to caller

	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone											// exit to caller
};

#endif // useExpansionPins
void peripheral::initMain(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer initialization section - multiple peripherals may use the same timer
#ifdef useTimer1
#ifdef useATmega32U4
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

	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // useATmega32U4
#ifdef useATmega2560
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

	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // useATmega2560
#ifdef useATmega128
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

	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // useATmega128
#endif // useTimer1
#ifdef useTimer2
#ifdef useATmega2560
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

#endif // useATmega2560
#ifdef useATmega128
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

#endif // useATmega128
#endif // useTimer2
#ifdef useTimer4
#ifdef useATmega32U4
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

#endif // useATmega32U4
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
	initButton();
	LCD::init();
#ifdef useStatusLEDpins
	initStatusLED();
#endif // useStatusLEDpins
#ifdef useExpansionPins
	initExpansionPin();
#endif // useExpansionPins

}

void peripheral::shutdownMain(void)
{

#ifdef useExpansionPins
	shutdownExpansionPin();
#endif // useExpansionPins
#ifdef useStatusLEDpins
	shutdownStatusLED();
#endif // useStatusLEDpins
	EEPROM::changeBitFlags(timer0Command, t0cDisplayDelay, 0); // cancel any display delays in progress
	LCD::shutdown(); // shut down the LCD display
	shutdownButton();
#ifdef useSerial1Port
	serial1::shutdown();
#endif // useSerial1Port
#ifdef useSerial0Port
	serial0::shutdown();
#endif // useSerial0Port

#ifdef useTimer4
#ifdef useATmega32U4
	PRR0 |= (1 << PRTIM4); // shut off timer4 module to reduce power consumption
#endif // useATmega32U4
#endif // useTimer4
#ifdef useTimer2
#ifdef useATmega2560
	PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // useATmega2560
#ifdef useATmega128
	PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // useATmega128
#endif // useTimer2
#ifdef useTimer1
#ifdef useATmega32U4
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // useATmega32U4
#ifdef useATmega2560
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // useATmega2560
#ifdef useATmega128
	PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // useATmega128
#endif // useTimer1

}

#ifdef useExpansionPins
void peripheral::initExpansionPin(void)
{

#ifdef useATmega32U4
	// set OC4A to clear-up/set-down PWM mode for EXP1 option pin
	TCCR4A &= ~(1 << COM4A0);
	TCCR4A |= ((1 << COM4A1) | (1 << PWM4A));

	// set OC4D to clear-up/set-down PWM mode for EXP2 option pin
	TCCR4C &= ~(1 << COM4D0);
	TCCR4C |= ((1 << COM4D1) | (1 << PWM4D));

	// enable EXP1 and EXP2 option pin outputs
	DDRC |= (1 << DDC6); // set OC4A as output
	DDRD |= (1 << DDD7); // set OC4D as output

#endif // useATmega32U4
#ifdef useATmega2560
	// set OC1B to clear-up/set-down for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << PB6) | (1 << PB4));

#endif // useATmega2560
#ifdef useATmega128
	// set OC1B to non-inverting mode for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << PB3) | (1 << PB2));

#endif // useATmega128
	outputExpansionPin1(0); // initially set EXP1 option pin to 0
	outputExpansionPin2(0); // initially set EXP2 option pin to 0

}

void peripheral::shutdownExpansionPin(void)
{

	outputExpansionPin1(0); // set EXP1 option pin to 0
	outputExpansionPin2(0); // set EXP2 option pin to 0

#ifdef useATmega32U4
	// disable expansion pin output
	DDRC &= ~(1 << DDC6);
	DDRD &= ~(1 << DDD7);

	// set OC4A to disabled for EXP1 option pin
	TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << PWM4A));

	// set OC4D to disabled for EXP2 option pin
	TCCR4C &= ~((1 << COM4D1) | (1 << COM4D0) | (1 << PWM4D));

#endif // useATmega32U4
#ifdef useATmega2560
	// disable expansion pin output
	DDRB &= ~((1 << PB6) | (1 << PB4));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

#endif // useATmega2560
#ifdef useATmega128
	// disable expansion pin output
	DDRB &= ~((1 << PB3) | (1 << PB2));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

#endif // useATmega128
}

// sets EXP1 option pin PWM output from 0-100%
void peripheral::outputExpansionPin1(uint8_t val)
{

#ifdef useATmega32U4
	OCR4A = val;
#endif // useATmega32U4
#ifdef useATmega2560
	OCR1B = val;
#endif // useATmega2560
#ifdef useATmega128
	OCR1B = val;
#endif // useATmega128

}

// sets EXP2 option pin PWM output from 0-100%
void peripheral::outputExpansionPin2(uint8_t val)
{

#ifdef useATmega32U4
	OCR4D = val;
#endif // useATmega32U4
#ifdef useATmega2560
	OCR2A = val;
#endif // useATmega2560
#ifdef useATmega128
	OCR2A = val;
#endif // useATmega128

}

#endif // useExpansionPins
void peripheral::initButton(void)
{

#ifdef useAdafruitRGBLCDshield
	uint16_t MCP23017registers;
#endif // useAdafruitRGBLCDshield
	uint8_t oldSREG;

#ifdef useAdafruitRGBLCDshield
	LCD::disableIntSample(); // disable TWI button sampling

	MCP23017registers = (uint16_t)(buttonMask);

	adafruitRGBLCDsupport::writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_IODIRx, (union_16 *)(&MCP23017registers)); // write out port direction (only buttons as input)
	adafruitRGBLCDsupport::writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (only buttons as input)
	adafruitRGBLCDsupport::writeRegister16Bit(TWI_MAIN_PROGRAM, MCP23017_B0_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (only buttons as input)

	adafruitRGBLCDsupport::setTransferMode(TWI_MAIN_PROGRAM, adaTWIbyteMode); // ensure address mode is in byte mode

	LCD::enableIntSample(); // enable TWI button sampling

#endif // useAdafruitRGBLCDshield
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useLegacyButtons
#ifdef useATmega2560
	DIDR2 &= ~((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // enable digital input on port K button pins

	PORTK |= ((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // enable port K button pullup resistors

	PCMSK2 |= ((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // enable port K button interrupts

	PCICR |= (1 << PCIE2); // enable selected interrupts on port K

	lastPINxState = PINK; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // useATmega2560
#ifdef useATmega128
	DIDR0 &= ~((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // enable digital input on port C button pins

	PORTC |= ((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // enable port C button pullup resistors

	PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // enable port C button interrupts

	PCICR |= (1 << PCIE1); // enable selected interrupts on port C

	lastPINxState = PINC; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // useATmega128
#endif // useLegacyButtons
	SREG = oldSREG; // restore interrupt flag status

}

void peripheral::shutdownButton(void)
{
}

#ifdef useStatusLEDpins
void peripheral::initStatusLED(void)
{

#ifdef useATmega32U4
#ifdef useTinkerkitLCDmodule
	DDRB |= (1 << DDB0); // turn on digital output for RX LED
	DDRC |= (1 << DDC7); // turn on digital output for LED L
	DDRD |= (1 << DDD5); // turn on digital output for TX LED
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for initializing status LEDs here
#endif // useTinkerkitLCDmodule
#endif // useATmega32U4
#ifdef useATmega2560
#ifdef useArduinoMega2560
	DDRB |= (1 << DDB7); // turn on digital output for LED L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for initializing status LEDs here
#endif // useArduinoMega2560
#endif // useATmega2560
#ifdef useATmega128
	DDRB |= (1 << DDB5); // turn on digital output for LED L
#endif // useATmega128

	outputStatusLED(0); // initially turn off status LED

}

void peripheral::shutdownStatusLED(void)
{

	outputStatusLED(0); // turn off status LED

#ifdef useTinkerkitLCDmodule
	DDRB &= ~(1 << DDB0); // turn off digital output for RX LED
	DDRC &= ~(1 << DDC7); // turn off digital output for LED L
	DDRD &= ~(1 << DDD5); // turn off digital output for TX LED
#endif // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
	DDRB &= ~(1 << DDB7); // turn off digital output for LED L
#endif // useArduinoMega2560
#ifdef useATmega128
	DDRB &= ~(1 << DDB5); // turn off digital output for LED L
#endif // useATmega128

}

void peripheral::outputStatusLED(uint8_t val)
{

	if (val)
	{

#ifdef useTinkerkitLCDmodule
		PORTB &= ~(1 << PORTB0); // active low RX
		PORTC |= (1 << PORTC7); // active high L
		PORTD &= ~(1 << PORTD5); // active low TX
#endif // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
		PORTB |= (1 << PORTB7); // active high L
#endif // useArduinoMega2560
#ifdef useATmega128
		PORTB |= (1 << PORTB5); // active high L
#endif // useATmega128

	}
	else
	{

#ifdef useTinkerkitLCDmodule
		PORTB |= (1 << PORTB0); // active low RX
		PORTC &= ~(1 << PORTC7); // active high L
		PORTD |= (1 << PORTD5); // active low TX
#endif // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
		PORTB &= ~(1 << PORTB7); // active high L
#endif // useArduinoMega2560
#ifdef useATmega128
		PORTB &= ~(1 << PORTB5); // active high L
#endif // useATmega128

	}

}

#endif // useStatusLEDpins
#ifdef useParallax5PositionSwitch
const unsigned int analogButtonThreshold[] PROGMEM = {
	0,		// 00
	559,	// 01
	580,	// 02
	586,	// 03
	618,	// 04
	651,	// 05
	664,	// 06
	693,	// 07
	717,	// 08
	728,	// 09
	748,	// 0A
	766,	// 0B
	786,	// 0C
	814,	// 0D
	834,	// 0E
	858,	// 0F
	897,	// 10
	927,	// 11
	980		// 12
};

const uint8_t analogButtonCount = (sizeof(analogButtonThreshold) / sizeof(unsigned int));

const uint8_t analogTranslate[(unsigned int)(analogButtonCount)] PROGMEM = {
	buttonsUp,			// 00
	btnShortPressULC,	// 01
	btnShortPressDLC,	// 02
	btnShortPressLC,	// 03
	btnShortPressUL,	// 04
	btnShortPressDL,	// 05
	btnShortPressL,		// 06
	btnShortPressUCR,	// 07
	btnShortPressDCR,	// 08
	btnShortPressCR,	// 09
	btnShortPressUC,	// 0A
	btnShortPressDC,	// 0B
	btnShortPressC,		// 0C
	btnShortPressUR,	// 0D
	btnShortPressDR,	// 0E
	btnShortPressR,		// 0F
	btnShortPressU,		// 10
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
#ifdef useStatusLEDpins
	uint8_t ledSleepVal = 0;
#endif // useStatusLEDpins

#ifdef useChryslerMAPCorrection
	unsigned long workingPressure;

#endif // useChryslerMAPCorrection
#ifdef useStatusLEDpins
	peripheral::outputStatusLED(ledSleepVal);

#endif // useStatusLEDpins
#ifdef useCPUreading
	thisTimerLoopLength += findCycleLength(timerLoopStart, systemInfo::cycles0());

#endif // useCPUreading
	performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#ifdef useCPUreading
	timerLoopStart = systemInfo::cycles0(); // record starting time

#endif // useCPUreading
#ifdef useStatusLEDpins
	peripheral::outputStatusLED(1 - ledSleepVal);

#endif // useStatusLEDpins
	// this is the part of the main loop that only executes twice a second (or what is defined by loopsPerSecond), to collect and process readings
	if (timer0Status & t0sTakeSample) // if main timer has commanded a sample be taken
	{

#ifdef useCPUreading
		lastTimerLoopLength = thisTimerLoopLength;
		thisTimerLoopLength = 0;

#endif // useCPUreading
		EEPROM::changeBitFlags(timer0Status, t0sTakeSample, 0); // acknowledge sample command

		uint8_t oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operations atomic

		i = rawTripIdx; // save old trip variable index
		rawTripIdx ^= (raw0tripIdx ^ raw1tripIdx); // set new raw trip variable index

#ifdef trackIdleEOCdata
		j = rawEOCidleTripIdx; // save old EOC/idle trip variable index
		rawEOCidleTripIdx ^= (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set new raw EOC/idle trip variable index

#endif // trackIdleEOCdata
#ifdef useDebugValueReadout
//		volatileVariables[(unsigned int)(vDebugValue1Idx)] = SWEET64(prgmCalculateOutputPinValue, 0);
//		volatileVariables[(unsigned int)(vDebugValue2Idx)] = SWEET64(prgmCalculateOutputPinValue, 1);
//		volatileVariables[(unsigned int)(vDebugValue3Idx)] = SWEET64(prgmFuelEcon, instantIdx);
//		volatileVariables[(unsigned int)(vDebugValue4Idx)] = SWEET64(prgmFuelEcon, currentIdx);
		volatileVariables[(unsigned int)(vDebugValue1Idx)] = debugValueDisplay::getSWEET64flags(0);
		volatileVariables[(unsigned int)(vDebugValue2Idx)] = debugValueDisplay::getSWEET64flags(1);
		volatileVariables[(unsigned int)(vDebugValue3Idx)] = debugValueDisplay::getSWEET64flags(2);
		volatileVariables[(unsigned int)(vDebugValue4Idx)] = debugValueDisplay::getSWEET64flags(128);

#endif // useDebugValueReadout
		SREG = oldSREG; // restore interrupt flag status

		tripArray[(unsigned int)(instantIdx)].transfer(tripArray[(unsigned int)(i)]); // transfer old raw trip variable to instant
#ifdef trackIdleEOCdata
		tripArray[(unsigned int)(eocIdleInstantIdx)].transfer(tripArray[(unsigned int)(j)]); // transfer old EOC/idle raw trip variable to EOC/idle instant
#endif // trackIdleEOCdata

		tripArray[(unsigned int)(i)].reset(); // reset old raw trip variable
#ifdef trackIdleEOCdata
		tripArray[(unsigned int)(j)].reset(); // reset old EOC/idle raw trip variable
#endif // trackIdleEOCdata

		for (uint8_t x = 0; x < tUScount; x++)
		{

			i = pgm_read_byte(&tripUpdateSrcList[(unsigned int)(x)]);
			j = pgm_read_byte(&tripUpdateDestList[(unsigned int)(x)]);

			tripArray[(unsigned int)(j)].update(tripArray[(unsigned int)(i)]);

		}

		if (awakeFlags & aAwakeOnVehicle)
		{

#ifdef useBarFuelEconVsTime
			if (bFEvTcount) bFEvTcount--;
			else
			{

				if (bFEvTsize < bgDataSize) bFEvTsize++;

				barFEvsTimeData[(unsigned int)(bFEvTstartIDx)] = SWEET64(prgmFuelEcon, periodIdx);

				bFEvTstartIDx++;
				if (bFEvTstartIDx == bgDataSize) bFEvTstartIDx = 0;

				tripArray[(unsigned int)(periodIdx)].reset();
				bFEvTcount = bFEvTperiod;

			}

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
			FEvSpdTripIdx = (uint8_t)(SWEET64(prgmFEvsSpeed, instantIdx));
			if (FEvSpdTripIdx < 255) tripArray[(unsigned int)(FEvSpdTripIdx)].update(tripArray[(unsigned int)(instantIdx)]);

#endif // useBarFuelEconVsSpeed
#ifdef useSerialDataLogging
			if (EEPROM::readVal(pSerialDataLoggingIdx)) doOutputDataLog();

#endif // useSerialDataLogging
#ifdef useWindowTripFilter
			if (EEPROM::readVal(pWindowTripFilterIdx))
			{

				tripArray[(unsigned int)(windowTripFilterIdx + wtpCurrentIdx)].transfer(tripArray[(unsigned int)(instantIdx)]);

				SWEET64(prgmUpdateWindowTripFilter, 0); // perform window trip filter magic here

				wtpCurrentIdx++;
				if (wtpCurrentIdx == windowTripFilterSize) wtpCurrentIdx = 0;

			}

#endif // useWindowTripFilter
		}

	}

#ifdef useAnalogRead
	if (analogStatus & asReadChannel)
	{

		EEPROM::changeBitFlags(analogStatus, asReadChannel, 0); // acknowledge ADC read completion

		i = analogValueIdx; // get a copy of the analog channel number just read in
		j = 0;

		switch (i)
		{

#ifdef useAnalogButtons
			case analogButtonChannelIdx:
				for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
				{

					if (analogValue[(unsigned int)(i)] >= pgm_read_word(&analogButtonThreshold[(unsigned int)(x)]))
					{

						analogButton = pgm_read_byte(&analogTranslate[(unsigned int)(x)]);
#ifdef useTestButtonValues
						analogButtonIdx = x;
#endif //  useTestButtonValues
						EEPROM::changeBitFlags(timer0Command, t0cProcessAnalogButton, 0); // send timer0 notification that an analog button was just read in
						break;

					}

				}
				break;

#endif // useAnalogButtons
#ifdef useChryslerMAPCorrection
			case analogBaroChannelIdx:
				j++;
			case analogMAPchannelIdx:
				if (analogValue[(unsigned int)(i)] < volatileVariables[(unsigned int)(vAnalogFloor0Idx + j)]) workingPressure = 0;
				else
				{

					workingPressure = (unsigned long)(analogValue[(unsigned int)(i)] - volatileVariables[(unsigned int)(vAnalogFloor0Idx + j)]); // shift raw MAP voltage down by floor
					workingPressure *= volatileVariables[(unsigned int)(vAnalogSlope0Idx + j)]; // convert raw MAP voltage to corresponding MAP units
					workingPressure >>= 10;
					workingPressure += volatileVariables[(unsigned int)(vAnalogOffset0Idx + j)]; // shift MAP reading up by MAP floor

				}

				volatileVariables[(unsigned int)(vMAPpressureIdx + j)] = workingPressure;

				// calculate differential pressure seen across the fuel injector
				workingPressure = volatileVariables[(unsigned int)(vFuelPressureIdx)] + volatileVariables[(unsigned int)(vBaroPressureIdx)] - volatileVariables[(unsigned int)(vMAPpressureIdx)];
				volatileVariables[(unsigned int)(vInjPressureIdx)] = workingPressure;

				// to get fuel pressure ratio, multiply differential pressure by denominator factor (1 << 12), then divide by fuel system pressure
				workingPressure <<= 12;
				workingPressure /= volatileVariables[(unsigned int)(vFuelPressureIdx)];

				// calculate square root of fuel pressure ratio
				volatileVariables[(unsigned int)(vInjectorCorrectionIdx)] = (unsigned long)(iSqrt((unsigned int)(workingPressure)));

				break;

#endif // useChryslerMAPCorrection

			default:
				break;

		}

	}

#endif // useAnalogRead
#ifdef useDragRaceFunction
	if (timer0Status & t0sAccelTestFlag)
	{

		EEPROM::changeBitFlags(timer0Status, t0sAccelTestFlag, 0); // acknowledge sample command

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
					if (SWEET64(prgmAccelTestCompareFullSpeeds, 0))
					{

						tripArray[(unsigned int)(dragHalfSpeedIdx)].transfer(tripArray[(unsigned int)(dragRawHalfSpeedIdx)]);
						tripArray[(unsigned int)(dragFullSpeedIdx)].transfer(tripArray[(unsigned int)(dragRawFullSpeedIdx)]);
						tripArray[(unsigned int)(dragDistanceIdx)].transfer(tripArray[(unsigned int)(dragRawDistanceIdx)]);

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

		EEPROM::changeBitFlags(timer1Status, t1sDebugUpdateFIP, 0); // acknowledge debug update request

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
				EEPROM::changeBitFlags(debugFlags, 0, debugFIPready);
				break;

			case 1:
				break;

			case 2:
				debugFIPtickLength = pgm_read_word(&debugFIPvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
				debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
				debugFIPWgoodTickLength = debugFIPtickLength - 63;
				EEPROM::changeBitFlags(debugFlags, 0, debugFIPready);
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

		EEPROM::changeBitFlags(timer1Status, t1sDebugUpdateVSS, 0); // acknowledge debug update request

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
				EEPROM::changeBitFlags(debugFlags, 0, debugVSSready);
				break;

			case 1:
				break;

			case 2:
				debugVSStickLength = pgm_read_word(&debugVSSvalues[(unsigned int)(debugVSSlength - debugVSSidx - 1)]);
				EEPROM::changeBitFlags(debugFlags, 0, debugVSSready);
				break;

			case 3:
				break;

			default:
				break;

		}

	}

#endif // useSimulatedFIandVSS
}

void performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

}

// primary MPGuino processing routine - overwrites Arduino sketch main if compiled in Arduino IDE
int main(void)
{

	uint8_t i;
	uint8_t j;

#ifndef useTestButtonValues
	const buttonVariable * bpPtr;

#endif // useTestButtonValues
	cli(); // disable interrupts while interrupts are being fiddled with

	// timer0 is the taskmaster driving MPGuino's measurement functionality
#ifdef useATmega32U4
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

#endif // useATmega32U4
#ifdef useATmega2560
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

#endif // useATmega2560
#ifdef useATmega128
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

#endif // useATmega128
	ACSR &= ~(1 << ACIE); // disable analog comparator interrupt
	ACSR |= (1 << ACD); // disable analog comparator module
	ADCSRB &= ~(1 << ACME); // disable analog comparator multiplexer

	timer0Command = t0cResetTimer;
#ifdef useTimer1
	timer1Command = t1cResetTimer;
#endif // useTimer1

#ifdef useSimulatedFIandVSS
	debugFlags = (debugVSSflag | debugInjectorFlag);
	debugReading::configurePorts();

#endif // useSimulatedFIandVSS
	rawTripIdx = raw0tripIdx;
#ifdef trackIdleEOCdata
	rawEOCidleTripIdx = raw0eocIdleTripIdx;
#endif // trackIdleEOCdata

	for (uint8_t x = 0; x < tUDcount; x++) tripArray[(unsigned int)(pgm_read_byte(&tripUpdateDestList[(unsigned int)(x)]))].reset();

#ifdef useDragRaceFunction
	accelerationFlags &= ~accelTestClearFlags;
	lastAccelTestStatus = accelerationFlags;
	accelTestStatus = accelerationFlags;

#endif // useDragRaceFunction
	menuLevel = 255;
	topScreenLevel = 0;
	brightnessIdx = 1;

	j = EEPROM::powerUpCheck();

	timer0DelayCount = delay1500msTick; // request a set number of timer tick delays equivalent to 1.5 seconds
	timer0Command |= (t0cDoDelay); // signal request to timer

#ifdef useBarFuelEconVsSpeed
	doResetBarFEvS();

#endif // useBarFuelEconVsSpeed
#ifdef useWindowTripFilter
	windowTripFilterReset();

#endif // useWindowTripFilter
	sei();

	peripheral::initMain(); // initialize all human interface peripherals

#ifdef useSavedTrips
	i = doTripAutoAction(1);

#endif // useSavedTrips
	text::gotoXY(devLCD, 0, 0);
	text::stringOut(devLCD, titleMPGuino);
	text::clearEOL(devLCD);
	text::gotoXY(devLCD, 0, 1);
	text::stringOut(devLCD, dateMPGuino);
	text::clearEOL(devLCD);

#ifdef useSerialDebugOutput
	text::stringOut(devDebugSerial, titleMPGuino);
	text::charOut(devDebugSerial, 0x0D);
	text::stringOut(devDebugSerial, dateMPGuino);
	text::charOut(devDebugSerial, 0x0D);

#endif // useSerialDebugOutput
#ifdef useSerialLoggingOutput
	text::stringOut(devLogSerial, titleMPGuino);
	text::charOut(devLogSerial, 0x0D);
	text::stringOut(devLogSerial, dateMPGuino);
	text::charOut(devLogSerial, 0x0D);

#endif // useSerialLoggingOutput
#ifdef useTerminalOutput
	text::stringOut(devLogTerminal, titleMPGuino);
	text::charOut(devLogTerminal, 0x0D);
	text::stringOut(devLogTerminal, dateMPGuino);
	text::charOut(devLogTerminal, 0x0D);

#endif // useTerminalOutput
	while (timer0Command & t0cDoDelay) idleProcess(); // show splash screen for 1.5 seconds

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

#ifdef useSavedTrips
	if (i) printStatusMessage(PSTR("AutoRestore Done"));

#endif // useSavedTrips
#ifdef useCPUreading
	timerLoopStart = systemInfo::cycles0();

#endif // useCPUreading
	while (true)
	{

		if (timer0Status & t0sActivityChange) // if any activity change has been detected
		{

			EEPROM::changeBitFlags(timer0Status, t0sActivityChange, 0); // clear activity detection flag

			if (awakeFlags & aPerformPark) // if MPGuino is commanded to go park
			{

				EEPROM::changeBitFlags(awakeFlags, aPerformPark, 0);

#ifdef useWindowTripFilter
				windowTripFilterReset(); // reset the window trip filter

#endif // useWindowTripFilter
#ifdef useSavedTrips
				if (doTripAutoAction(0)) printStatusMessage(PSTR("AutoSave Done"));

#endif // useSavedTrips
			}

			if (awakeFlags & aPerformFallAsleep) // if MPGuino is commanded to go asleep
			{

				EEPROM::changeBitFlags(awakeFlags, aPerformFallAsleep, 0);
#ifdef useDeepSleep

				doGoDeepSleep();
#else // useDeepSleep
#ifdef useClockDisplay

				bigNumber::displayHandler(menuEntryIdx, 255); // initialize the software clock
				LCD::setBrightness(0); // set backlight brightness to zero
#else // useClockDisplay

				LCD::shutdown(); // shut down the LCD display
#endif // useClockDisplay
#endif // useDeepSleep

			}

			if (awakeFlags & aPerformWakeUp) // if MPGuino is commanded to wake up
			{

				EEPROM::changeBitFlags(awakeFlags, aPerformWakeUp, 0);

				LCD::init(); // re-initialize LCD device
				((handlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(menuEntryIdx, screenCursor[(unsigned int)(menuLevel)]); // call indexed support section screen initialization function

				if ((EEPROM::readVal(pWakeupResetCurrentOnEngineIdx)) || (EEPROM::readVal(pWakeupResetCurrentOnButtonIdx))) doTripResetCurrent();

			}

		}

		if (timer0Status & t0sReadButton) // see if any buttons were pressed, process related button function if so
		{

			j = buttonState; // capture button state
			EEPROM::changeBitFlags(timer0Status, t0sReadButton, 0); // acknowledge sample command

#ifdef useTestButtonValues
			noSupport();
#else // useTestButtonValues
			bpPtr = (const buttonVariable *)(pgm_read_word(&(screenParameters[(unsigned int)(menuLevel)].modeButtonList)));

			while (true)
			{

				i = pgm_read_byte(&(bpPtr->buttonCode));

				if ((j == i) || (i == buttonsUp)) break;
				else bpPtr++;

			}

			text::gotoXY(devLCD, 0, 0);
			((pFunc)pgm_read_word(&(bpPtr->buttonCommand)))(); // go perform action
#endif // useTestButtonValues

		}

		// this part of the main loop handles screen output to the user
		// it can execute either after the samples are collected and processed above, or after a key has been pressed
		if (timer0Status & t0sUpdateDisplay)
		{

			EEPROM::changeBitFlags(timer0Status, t0sUpdateDisplay, 0); // acknowledge update display command

#ifdef useJSONoutput
			if (awakeFlags & aAwakeOnVehicle) doOutputJSON();

#endif // useJSONoutput
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
				case afVehicleStoppedFlag: // vehicle stopped
				case afEngineOffFlag: // engine stopped
				case (afVehicleStoppedFlag | afEngineOffFlag): // engine stopped, vehicle stopped
				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag): // engine stopped, vehicle stopped, button not pressed
				case 0: // engine running and vehicle in motion
				case afButtonFlag: // engine running, vehicle in motion, button not pressed
					((handlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(menuOutputDisplayIdx, screenCursor[(unsigned int)(menuLevel)]); // call indexed support section screen refresh function
					break;

				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag | afButtonFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, activity timeout reached
				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, park timeout reached, activity timeout reached
#ifndef useDeepSleep
#ifdef useClockDisplay
					bigNumber::displayHandler(menuOutputDisplayIdx, 255); // display the software clock

#endif // useClockDisplay
#endif // useDeepSleep
					break;

				default: // handle unexpected cases
					text::hexByteOut(devLCD, i);
					break;

			}

		}

		idleProcess(); // all functions are completed at this point, so wait for an interrupt to occur

	}

}
