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
	injector sense open  PD2 (INT0), Digital 2
	injector sense close PD3 (INT1), Digital 3
	speed                PC0 (PCINT8), A0
	(if configured) MAP  PC1 (ADC1), A1
	(if configured) Baro PC2 (ADC2), A2

  TinkerKit! LCD module
	injector sense open  PD0 (INT0), Digital 3 (note this makes it impossible for the TinkerKit! LCD module to use TWI)
	injector sense close PD1 (INT1), Digital 2
	speed                PB7 (PCINT7), Digital 11
	(if configured) MAP  PF7 (ADC7), A0
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
               |               |               |              R7 (JBD)
               |               |               o----------------vvv--o PC1 - legacy MPGuino hardware
               |               |               |                2.2k   PF1 - Arduino Mega 2560
               |               |               |                       PF7 - TinkerKit! LCD module
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
               |               |               |              R6 (JBD)
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

  Arduino Mega 2560
	SCL     PD0 (SCL), Digital 21
	SDA     PD1 (SDA), Digital 20

-------------------------------------

Buttons - Legacy
* Physical layout

	left	middle	right

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
	left, middle, right, extra#1, extra#2 PF7 (ADC7)

  Arduino Mega 2560
	left, middle, right, extra#1, extra#2 PF3 (ADC3), A3


             o---------------o---------------o---------------o---------------o--o GND
        R2   |          R3   |          R4   |          R5   |          R6   |
     o--vvv--o       o--VVV--o       o--vvv--o       o--vvv--o       o--vvv--o
     |  2.2k         |  4.7k         |  10k          |  22k          |  47k
     o               o               o               o               o
      /               /               /               /               /
     o left          o middle        o right         o Extra#1       o Extra#2

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
	left, middle, right, extra#1, extra#2 PF7 (ADC7)

  Arduino Mega 2560
	left, middle, right, extra#1, extra#2 PF3 (ADC3), A3

                                                                                  PF3 - Arduino Mega 2560
                                                                                  PF7 - TinkerKit! LCD module
                                                                     o----------o PC3 - legacy MPGuino hardware
                                                                     | R1 1k
     o---------------o---------------o---------------o---------------o--vvv--o--o 5V

     |  left         | middle        |  right        | Extra#1       | Extra#2

     |  R2           |  R3           |  R4           |  R5           |  R6
     o--vvv--o       o--VVV--o       o--vvv--o       o--vvv--o       o--vvv--o
        2.2k |          4.7k |          10k  |          22k  |          47k  |


             o               o               o               o               o
          -----------------------------------------------------------------------
         |   4               6               2               7               3   |
         |   LT              CTR             RT              UP              DN  |


         |                  VCC             GND                                  |
         |                   5               8                                   |
          -----------------------------------------------------------------------
                             o               o

                             o               o----------------------------------O GND
                            N/C

Buttons - Adafruit RGB LCD Shield
  legacy MPGuino hardware
	SCL     PC5 (SCL), A5
	SDA     PC4 (SDA), A4

  Arduino Mega 2560
	SCL     PD0 (SCL), Digital 21
	SDA     PD1 (SDA), Digital 20

Expansion outputs
  legacy MPGuino hardware
	EXP1       PB2, Digital 10, controlled by PWM on OC1B
	EXP2       PB3, Digital 11, controlled by PWM on OC2A

  TinkerKit! LCD module
	EXP1       PD7, Digital 6, controlled by PWM on OC4D
	EXP2       PC6, Digital 5, controlled by PWM on OC3A

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
// if all of the below "#define"s are commented out, code will compile for an AtMega328-series processor
//
//#define useJellyBeanDriverBoard true		// used to preset LCD, processor speed, and switch settings, ignoring below related #defines
//#define useLegacyBoard true			// used to preset LCD, processor speed, and switch settings, ignoring below related #defines
//#define useArduinoMega2560 true
//#define useTinkerkitLCDmodule true

// the below line sets the serial output speed
//
const unsigned long myBaudRate0 = 57600ul;	// (most Arduino boards, excluding TinkerKit! LCD module)
const unsigned long myBaudRate1 = 19200ul;	// (Atmega2560 board and TinkerKit! LCD module)

// the below line sets the processor speed for the MPGuino hardware to be used. Common values are 16 (legacy) or 20 (JellyBeanDriver)
//
const uint8_t processorSpeed = 16;

// only one of the below LCD options may be chosen - choosing more than one will cause a compilation error to occur
// if useTinkerkitLCDmodule is used, useLegacyLCD will automatically be used, and the below options will be ignored
//
#define useLegacyLCD true			// select Legacy 4-bit parallel LCD output
//#define useParallaxSerialLCDmodule true		// select Serial Interface LCD module output
//#define useAdafruitRGBLCDmodule true		// select Adafruit 4-bit RGB LCD module output
//#define useTWILCD true				// select generic LCD 16x2 module output over TWI
#define useBufferedLCD true			// Speed up LCD output. Works with either useLegacyLCD or useParallaxSerialLCDmodule or useAdafruitRGBLCDmodule or useTWILCD

// only one of the below button options may be chosen - choosing more than one will cause a compilation error to occur
//
// note: if useAdafruitRGBLCDmodule is selected, useLegacyButtons will be ignored
//
#define useLegacyButtons true
//#define useAnalogMuxButtons true
//#define useParallax5PositionSwitch true

// the below options only work if useParallaxSerialLCDmodule is selected. If useParallaxSerialLCDmodule is not selected, the below options will not be inserted at all
//
//#define useLCDserialPort0 true			// select serial LCD module output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLCDserialPort1 true			// select serial LCD module output on serial port channel 1 (Atmega2560 board and TinkerKit! LCD module)

// the below option only works if useLegacyLCD is selected. If useLegacyLCD is not selected, the below option will not be inserted at all
//
//#define useInvertedLegacyLCDbrightness true	// For alternate LCD backlight connections

// serial logging options - mutually exclusive - either choose serial data logging or JSON output, but not both
// selecting both will result in a compilation error
//
//#define useJSONoutput true			// skybolt added to enable and call JSON out routine
//#define useSerialDataLogging true		// Ability to output 5 basic parameters to a data logger or SD card

// common serial logging options
//
//#define useLoggingSerialPort0 true		// select serial logging output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLoggingSerialPort1 true		// select serial logging output on serial port channel 1 (Atmega2560 board and TinkerKit! LCD module)
//#define useBufferedLoggingSerialPort true	// Speed up serial logging output

// mutually exclusive options - you may choose one or the other, but not both
//
#define useDeepSleep true			// places MPGuino into deep sleep after activity timeout
//#define useSoftwareClock true			// Show system clock, and provide means to set it

// selectable options - all may be chosen independently of one another
//
//#define blankScreenOnMessage true		// Completely blank display screen upon display of message
#define trackIdleEOCdata true			// Ability to track engine idling and EOC modes
#define useCalculatedFuelFactor true		// Ability to calculate that pesky us/gal (or L) factor from easily available published fuel injector data
#define useBigFE true				// Show big fuel economy displays
#define useBigDTE true				// Show big distance-to-empty displays
#define useBigTTE true				// Show big time-to-empty displays
#define useSavedTrips true			// Ability to save current or tank trips to any one of 10 different trip slots in EEPROM
//#define showSavedTrips true			// Ability to display trips saved in EEPROM, in conjunction with useSavedTrips above
#define useScreenEditor true			// Ability to change any of (8 to 14, depending on configuration) existing trip data screens, with 4 configurable figures on each screen
#define useBarFuelEconVsTime true		// Show Fuel Economy over Time bar graph
#define useBarFuelEconVsSpeed true		// Show Fuel Economy vs Speed, Fuel Used vs Speed bar graphs
#define useSpiffyBigChars true
//#define useFuelCost true			// Show fuel cost
#define useChryslerMAPCorrection true		// Ability to perform on-the-fly fuel injector data correction for late-model Chrysler vehicles
//#define useImperialGallon true			// when selected, uses Imperial gallons instead of default US gallons
#define useWindowTripFilter true		// Smooths out "jumpy" instant FE figures that are caused by modern OBDII engine computers
#define useAssemblyLanguage true		// Speeds up many low-level MPGuino functions
#define useDragRaceFunction true		// Performs "drag race" 0-60 MPH, 1/4 mile time, estimated horsepower functionality
#define usePartialRefuel true			// Provide means to enter partial refuel amount into MPGuino

// options that are in progress at the moment - don't enable them unless you want to test them
//
//#define useBluetooth true				// Use bluetooth interface with Android phone in addition to or instead of LCD/buttons
//#define useExpansionPins true				// Generate analog 0-5VDC output voltage on expansion pins to drive LEDs or feed signal to external gauges
//#define useABresultViewer true			// Ability to graphically show current (B) versus stored (A) fuel consumption rates
//#define useCoastDownCalculator true		// Ability to calculate C(rr) and C(d) from coastdown
//#define useFuelParamCalculator true		// Ability to calculate microseconds per gallon and fuel injector delay stored parameters
//#define useRealTimeClockModule true		// Ability to read and display Realtime Clock data from a TWI RTC module

// program measurement and debugging tools
//
//#define useCPUreading true			// Show CPU loading and available RAM usage
//#define useBenchMark true				// this is possibly broken - last time I used it was in August 2013
//#define useDebugReadings true			// forces inputs to simulate going 73.7 MPH while getting 26.42 MPG.
//#define forceEEPROMsettingsInit true		// forces EEPROM reset every time MPGuino is powered up
//#define useEEPROMviewer true			// Ability to directly examine EEPROM
//#define useLEDpins true				// allows usage of LEDs to show status of certain bits

// SWEET64 configuration/debugging
//
//#define useSWEET64trace true			// Ability to view real-time 64-bit calculations from SWEET64 kernel
//#define useSWEET64multDiv true			// shift mul64 and div64 from native C++ to SWEET64 bytecode
//#define useSerialDebugOutput true
//#define useDebuggingSerialPort0 true		// select serial debug output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useDebuggingSerialPort1 true		// select serial debug output on serial port channel 1 (Atmega2560 board and TinkerKit! LCD module)
//#define useBufferedDebugSerialPort true		// Speed up serial debug output

// these #defines are used to select various features to support the above choices
// do not mess with them, or compilation errors will occur
//
#ifdef useTinkerkitLCDmodule
#define useLegacyLCD true
#ifdef useLegacyButtons
#define useAnalogMuxButtons true
#undef useLegacyButtons
#endif // useLegacyButtons
#undef useParallaxSerialLCDmodule
#undef useLCDserialPort0
#undef useLCDserialPort1
const uint8_t systemProcessorSpeed = 16;
#else // useTinkerkitLCDmodule
#ifdef useJellyBeanDriverBoard
const uint8_t systemProcessorSpeed = 20;
#define useLegacyLCD true
#define useLegacyButtons true
#undef useParallaxSerialLCDmodule
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useLEDpins
#undef useAdafruitRGBLCDmodule
#else // useJellyBeanDriverBoard
#ifdef useLegacyBoard
const uint8_t systemProcessorSpeed = 16;
#define useLegacyLCD true
#define useLegacyButtons true
#undef useParallaxSerialLCDmodule
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useAdafruitRGBLCDmodule
#undef useLEDpins
#else // useLegacyBoard
const uint8_t systemProcessorSpeed = processorSpeed;
#endif // useLegacyBoard
#endif // useJellyBeanDriverBoard
#endif // useTinkerkitLCDmodule

#ifdef useSoftwareClock
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

#ifndef useSWEET64trace
#ifndef useSerialDebugOutput
#undef useDebuggingSerialPort0
#undef useDebuggingSerialPort1
#undef useBufferedDebugSerialPort
#endif // useSerialDebugOutput
#endif // useSWEET64trace

#ifndef useJSONoutput
#ifndef useSerialDataLogging
#undef useLoggingSerialPort0
#undef useLoggingSerialPort1
#undef useBufferedLoggingSerialPort
#endif // useSerialDataLogging
#endif // useJSONoutput

#ifndef useParallaxSerialLCDmodule
#undef useLCDserialPort0
#undef useLCDserialPort1
#endif

#ifdef useLoggingSerialPort0
#ifdef useBufferedLoggingSerialPort
#define useBuffering true
#define useBufferedSerial0Port true
#define loggingSerialBuffer serial0Buffer
#define loggingSerialData serial0Data
#endif
#define useSerial0Port true
#define useSerialLoggingOutput true
#define loggingSerialPort serial0
#endif

#ifdef useLoggingSerialPort1
#ifdef useBufferedLoggingSerialPort
#define useBuffering true
#define useBufferedSerial1Port true
#define loggingSerialBuffer serial1Buffer
#define loggingSerialData serial1Data
#endif // useBufferedLoggingSerialPort
#define useSerial1Port true
#define useSerialLoggingOutput true
#define loggingSerialPort serial1
#endif // useLoggingSerialPort1

#ifdef useLCDserialPort0
#ifdef useBufferedLCD
#define useBuffering true
#define useBufferedSerial0Port true
#define LCDserialBuffer serial0Buffer
#define LCDserialData serial0Data
#endif // useBufferedLCD
#define useSerial0Port true
#define LCDserialPort serial0
#endif // useLCDserialPort0

#ifdef useLCDserialPort1
#ifdef useBufferedLCD
#define useBuffering true
#define useBufferedSerial1Port true
#define LCDserialBuffer serial1Buffer
#define LCDserialData serial1Data
#endif // useBufferedLCD
#define useSerial1Port true
#define LCDserialPort serial1
#endif // useLCDserialPort1

#ifdef useDebuggingSerialPort0
#ifdef useBufferedDebugSerialPort
#define useBuffering true
#define useBufferedSerial0Port true
#define debugSerialBuffer serial0Buffer
#define debugSerialData serial0Data
#endif // useBufferedDebugSerialPort
#define useSerial0Port true
#define debugSerialPort serial0
#endif // useDebuggingSerialPort0

#ifdef useDebuggingSerialPort1
#ifdef useBufferedDebugSerialPort
#define useBuffering true
#define useBufferedSerial1Port true
#define debugSerialBuffer serial1Buffer
#define debugSerialData serial1Data
#endif // useBufferedDebugSerialPort
#define useSerial1Port true
#define debugSerialPort serial1
#endif // useDebuggingSerialPort1

#ifdef useBenchMark
#define useCPUreading true
#endif // useBenchMark

#ifdef useLegacyLCD
#define use4BitLCD true
#define useAnalogLCDdelay true
#endif // useLegacyLCD

#ifdef useParallaxSerialLCDmodule
#define use4BitLCD true
#define useBinaryLCDbrightness true
#endif // useParallaxSerialLCDmodule

#ifdef useAnalogMuxButtons
#define useAnalogButtons true
#endif // useAnalogMuxButtons

#ifdef useParallax5PositionSwitch
#define useAnalogButtons true
#endif // useParallax5PositionSwitch

#ifdef useAdafruitRGBLCDmodule
#define useTWIsupport true
#define useTWILCD true
#ifndef useAnalogButtons
#undef useLegacyButtons
#define useTWIbuttons true
#endif // useAnalogButtons
#endif // useAdafruitRGBLCDmodule

#ifdef useTWILCD
#define useTWIsupport true
#define use4BitLCD true
#define useAnalogLCDdelay true
#define useBinaryLCDbrightness true
#endif // useTWILCD

#ifdef useAnalogButtons
#define useAnalogRead true
#endif // useAnalogButtons

#ifdef useAnalogLCDdelay
#define useAnalogInterrupt true
#endif // useAnalogLCDdelay

#ifdef useAnalogRead
#define useAnalogInterrupt true
#endif // useAnalogRead

#ifdef useDebugReadings
#define useAnalogInterrupt true
#endif // useDebugReadings

#ifdef useLegacyLCD
#define useTimer1 true
#endif // useLegacyLCD

#ifdef useExpansionPins
#ifndef useTinkerkitLCDmodule
#ifndef useArduinoMega2560
#define useTimer1 true
#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
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

#ifdef useBufferedLCD
#define useBuffering true
#endif // useBufferedLCD

#ifdef useSavedTrips
#define useEEPROMtripStorage true
#endif // useSavedTrips

#ifdef useTWIbuttons
#define useButtonCrossConfig true
#endif //useTWIbuttons

#ifdef useAnalogButtons
#define useButtonCrossConfig true
#endif //useAnalogButtons

// this section catches configuration errors

#ifdef useTWIsupport
#ifdef useLegacyButtons
#ifndef useArduinoMega2560
#error *** CANNOT configure for Legacy pushbuttons and TwoWire support!!! ***
#endif // useArduinoMega2560
#endif // useLegacyButtons
#ifdef useTinkerkitLCDmodule
#error *** CANNOT configure for TinkerKit! LCD Module and TwoWire support!!! ***
#endif // useTinkerkitLCDmodule
#endif // useTWIsupport

#ifdef useArduinoMega2560
#ifdef useTinkerkitLCDmodule
#error *** CANNOT configure for Arduino AtMega2560 and TinkerKit! LCD Module!!! ***
#endif // useTinkerkitLCDmodule
#endif // useArduinoMega2560

#ifdef useLegacyBoard
#ifdef useTinkerkitLCDmodule
#error *** CANNOT configure for Legacy Board and TinkerKit! LCD Module!!! ***
#endif // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
#error *** CANNOT configure for Legacy Board and Arduino AtMega2560!!! ***
#endif // useArduinoMega2560
#endif // useLegacyBoard

#ifdef useJellyBeanDriverBoard
#ifdef useArduinoMega2560
#error *** CANNOT configure for JellyBeanDriver Board and Arduino AtMega2560!!! ***
#endif // useArduinoMega2560
#ifdef useTinkerkitLCDmodule
#error *** CANNOT configure for JellyBeanDriver Board and TinkerKit! LCD Module!!! ***
#endif // useTinkerkitLCDmodule
#ifdef useLegacyBoard
#error *** CANNOT configure for JellyBeanDriver Board and Legacy Board!!! ***
#endif // useLegacyBoard
#endif // useJellyBeanDriverBoard

#ifdef useJSONoutput
#ifdef useSerialDataLogging
#error *** CANNOT use Serial Logging Output for both JSON output and Serial Data Logging!!! ***
#endif // useSerialDataLogging
#endif // useJSONoutput

#ifdef useParallaxSerialLCDmodule
#ifdef useLCDserialPort0
#ifdef useLCDserialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial LCD Output!!! ***
#endif // useLCDserialPort1
#endif // useLCDserialPort0
#ifndef useLCDserialPort0
#ifndef useLCDserialPort1
#error *** Serial LCD Output needs a serial port assigned to it!!! ***
#endif // useLCDserialPort1
#endif // useLCDserialPort0
#endif // useParallaxSerialLCDmodule

#ifdef useJSONoutput
#ifdef useLoggingSerialPort0
#ifdef useLoggingSerialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for JSON Output!!! ***
#endif // useLoggingSerialPort1
#endif // useLoggingSerialPort0
#ifndef useLoggingSerialPort0
#ifndef useLoggingSerialPort1
#error *** JSON Output needs a serial port assigned to it!!! ***
#endif // useLoggingSerialPort1
#endif // useLoggingSerialPort0
#endif // useJSONoutput

#ifdef useSerialDataLogging
#ifdef useLoggingSerialPort0
#ifdef useLoggingSerialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial Logging Output!!! ***
#endif // useLoggingSerialPort1
#endif // useLoggingSerialPort0
#ifndef useLoggingSerialPort0
#ifndef useLoggingSerialPort1
#error *** Serial Logging Output needs a serial port assigned to it!!! ***
#endif // useLoggingSerialPort1
#endif // useLoggingSerialPort0
#endif // useSerialDataLogging

#ifdef useSerialDebugOutput
#ifdef useDebuggingSerialPort0
#ifdef useDebuggingSerialPort1
#error *** CANNOT use Serial Port 0 and Serial Port 1 for Serial Debugging Output!!! ***
#endif // useDebuggingSerialPort1
#endif // useDebuggingSerialPort0
#ifndef useDebuggingSerialPort0
#ifndef useDebuggingSerialPort1
#error *** Serial Debugging Output needs a serial port assigned to it!!! ***
#endif // useDebuggingSerialPort1
#endif // useDebuggingSerialPort0
#endif // useSerialDebugOutput

#ifdef useSerial1Port
#ifndef useArduinoMega2560
#ifndef useTinkerkitLCDmodule
#error *** Must use Atmega2560 or Tinkerkit! LCD module for Serial Port 1!!! ***
#endif // useTinkerkitLCDmodule
#endif // useArduinoMega2560
#endif // useSerial1Port

#ifdef useSerial0Port
#ifdef useTinkerkitLCDmodule
#error *** Cannot use Serial Port 0 on Tinkerkit! LCD module!!! ***
#endif // useTinkerkitLCDmodule
#endif // useSerial0Port

#ifdef useLegacyLCD
#ifndef useArduinoMega2560
#ifndef useTinkerkitLCDmodule
#ifdef useLEDpins
#error *** CANNOT use built-in LED pins and Legacy LCD configuration!!! ***
#endif // useLEDpins
#endif // useTinkerkitLCDmodule
#endif // useArduinoMega2560
#endif // useLegacyLCD

#ifdef useLCDserialPort0
#ifdef useLoggingSerialPort0
#ifdef useJSONoutput
#error *** CANNOT use serial port 0 for both JSON output and Serial LCD!!! ***
#endif // useJSONoutput
#ifdef useSerialDataLogging
#error *** CANNOT use serial port 0 for both Serial Data Logging! and Serial LCD!!! ***
#endif // useSerialDataLogging
#error *** CONFLICT serial port 0!!! ***
#endif // useLoggingSerialPort0
#endif // useLCDserialPort0

#ifdef useLCDserialPort1
#ifdef useLoggingSerialPort1
#ifdef useJSONoutput
#error *** CANNOT use serial port 1 for both JSON output and Serial LCD!!! ***
#endif // useJSONoutput
#ifdef useSerialDataLogging
#error *** CANNOT use serial port 1 for both Serial Data Logging! and Serial LCD!!! ***
#endif // useSerialDataLogging
#error *** CONFLICT serial port 1!!! ***
#endif // useLoggingSerialPort1
#endif // useLCDserialPort1

#ifdef useLCDserialPort0
#ifdef useDebuggingSerialPort0
#error *** CANNOT use serial port 0 for both Serial Debugging Output and Serial LCD!!! ***
#endif // useDebuggingSerialPort0
#endif // useLCDserialPort0

#ifdef useLCDserialPort1
#ifdef useDebuggingSerialPort1
#error *** CANNOT use serial port 1 for both Serial Debugging Output and Serial LCD!!! ***
#endif // useDebuggingSerialPort1
#endif // useLCDserialPort1

#ifdef useDebuggingSerialPort0
#ifdef useLoggingSerialPort0
#ifdef useJSONoutput
#error *** CANNOT use serial port 0 for both JSON output and Serial Debugging Output!!! ***
#endif // useJSONoutput
#ifdef useSerialDataLogging
#error *** CANNOT use serial port 0 for both Serial Data Logging and Serial Debugging Output!!! ***
#endif // useSerialDataLogging
#error *** CONFLICT serial port 0!!! ***
#endif // useLoggingSerialPort0
#endif // useDebuggingSerialPort0

#ifdef useDebuggingSerialPort1
#ifdef useLoggingSerialPort1
#ifdef useJSONoutput
#error *** CANNOT use serial port 1 for both JSON output and Serial Debugging Output!!! ***
#endif // useJSONoutput
#ifdef useSerialDataLogging
#error *** CANNOT use serial port 1 for both Serial Data Logging and Serial Debugging Output!!! ***
#endif // useSerialDataLogging
#error *** CONFLICT serial port 1!!! ***
#endif // useLoggingSerialPort1
#endif // useDebuggingSerialPort1

#ifdef useLegacyButtons
#ifdef useAnalogMuxButtons
#error *** CANNOT use both Legacy pushbuttons and Analog Mux buttons!!! ***
#endif // useAnalogMuxButtons
#endif // useLegacyButtons

#ifdef useLegacyButtons
#ifdef useParallax5PositionSwitch
#error *** CANNOT use both Legacy pushbuttons and Parallax 5-position switch!!! ***
#endif // useParallax5PositionSwitch
#endif // useLegacyButtons

#ifdef useAnalogMuxButtons
#ifdef useParallax5PositionSwitch
#error *** CANNOT use both Analog Mux buttons and Parallax 5-position switch!!! ***
#endif // useParallax5PositionSwitch
#endif // useAnalogMuxButtons

#ifdef useSoftwareClock
#ifdef useDeepSleep
#error *** CANNOT use both Software Clock and Deep Sleep functionality!!! ***
#endif // useDeepSleep
#endif // useSoftwareClock

// this is the start of the actual program code - device-specific #defines and equates should go here

#ifdef useAdafruitRGBLCDmodule
const uint8_t MCP23017_IODIRx =		0x00;
const uint8_t MCP23017_IPOLx =		0x02;
const uint8_t MCP23017_GPINTENx =	0x04;
const uint8_t MCP23017_DEFVALx =	0x06;
const uint8_t MCP23017_INTCONx =	0x08;
const uint8_t MCP23017_IOCON =		0x0A;
const uint8_t MCP23017_GPPUx =		0x0C;
const uint8_t MCP23017_INTFx =		0x0E;
const uint8_t MCP23017_INTCAPx =	0x10;
const uint8_t MCP23017_GPIOx =		0x12;
const uint8_t MCP23017_OLATx =		0x14;

const uint8_t MCP23017_PORTA =		0x00;
const uint8_t MCP23017_PORTB =		0x01;

#endif // useAdafruitRGBLCDmodule
// this is where LCD low-level definitions should go

#ifdef use4BitLCD
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDmodule
const uint8_t lcdDirection =		0b01000000; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	0b10000000; // GPIO B
const uint8_t lcdEnable =		0b00100000; // GPIO B
const uint8_t lcdBit3 =			0b00000010; // GPIO B
const uint8_t lcdBit2 =			0b00000100; // GPIO B
const uint8_t lcdBit1 =			0b00001000; // GPIO B
const uint8_t lcdBit0 =			0b00010000; // GPIO B
const uint8_t lcdBrightnessRed =	0b01000000; // GPIO A
const uint8_t lcdBrightnessGreen =	0b10000000; // GPIO A
const uint8_t lcdBrightnessBlue =	0b00000001; // GPIO B

const uint8_t lcdAddress =		0x20;

volatile uint8_t portSwitches; // switch (and LCD port) register expander byte
volatile uint8_t portLCD;  // LCD port register expander byte

#else // useAdafruitRGBLCDmodule
const uint8_t lcdDirection =		0b00000010; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	0b00000001;
const uint8_t lcdEnable =		0b00000100;
const uint8_t lcdBit3 =			0b10000000;
const uint8_t lcdBit2 =			0b01000000;
const uint8_t lcdBit1 =			0b00100000;
const uint8_t lcdBit0 =			0b00010000;
const uint8_t lcdBrightness =		0b00001000;

const uint8_t lcdAddress =		0x27;

volatile uint8_t portLCD; // LCD port register expander byte

#endif // useAdafruitRGBLCDmodule
const uint8_t lcdDataBitMask =		(lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);

#else // useTWILCD
#ifdef useTinkerkitLCDmodule
const uint8_t lcdDirection =		(1 << PORTF0); // on PORTF, A5, Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
const uint8_t lcdRegisterSelect =	(1 << PORTF1); // on PORTF, A4
const uint8_t lcdEnable =		(1 << PORTE6); // on PORTE, 7
const uint8_t lcdBit3 =			(1 << PORTB4); // on PORTB, 8
const uint8_t lcdBit2 =			(1 << PORTD6); // on PORTD, 12
const uint8_t lcdBit1 =			(1 << PORTD4); // on PORTD, 4
const uint8_t lcdBit0 =			(1 << PORTF4); // on PORTF, A3
const uint8_t lcdBrightness =		(1 << DDB6); // on PORTB, 10
const uint8_t lcdContrast =		(1 << DDB5); // on PORTB, 9

#else // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
const uint8_t lcdRegisterSelect =	(1 << PORTA4); // on PORTA
const uint8_t lcdEnable =		(1 << PORTA5); // on PORTA
const uint8_t lcdBit3 =			(1 << PORTA0); // on PORTA
const uint8_t lcdBit2 =			(1 << PORTA1); // on PORTA
const uint8_t lcdBit1 =			(1 << PORTA2); // on PORTA
const uint8_t lcdBit0 =			(1 << PORTA3); // on PORTA
const uint8_t lcdBrightness =		(1 << DDB5); // on PORTB
const uint8_t lcdContrast =		(1 << DDB7); // on PORTB

#else // useArduinoMega2560
const uint8_t lcdRegisterSelect =	(1 << PORTD4); // on PORTD, 4
const uint8_t lcdEnable =		(1 << PORTD5); // on PORTD, 5
const uint8_t lcdBit3 =			(1 << PORTB5); // on PORTB, 13
const uint8_t lcdBit2 =			(1 << PORTB4); // on PORTB, 12
const uint8_t lcdBit1 =			(1 << PORTB0); // on PORTB, 8
const uint8_t lcdBit0 =			(1 << PORTD7); // on PORTD, 7
const uint8_t lcdBrightness =		(1 << DDB1); // on PORTB, 9
const uint8_t lcdContrast =		(1 << DDD6); // on PORTD, 6

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useTWILCD
#endif // use4BitLCD
// This is where direct-ported (not analog) pushbutton definitions should go

#ifdef useTWIbuttons
#ifdef useAdafruitRGBLCDmodule
const uint8_t lbuttonBit = 		0b00010000; // GPIO A bit 4, left button
const uint8_t mbuttonBit = 		0b00000001; // GPIO A bit 0, select button
const uint8_t rbuttonBit = 		0b00000010; // GPIO A bit 1, right button
const uint8_t abuttonBit = 		0b00001000; // GPIO A bit 3, up button
const uint8_t bbuttonBit = 		0b00000100; // GPIO A bit 2, down button

const uint8_t longButtonBit =	 	0b00100000; // GPIO A bit 5 isn't being used for anything right now

#else // useAdafruitRGBLCDmodule
#endif // useAdafruitRGBLCDmodule
#else // useTWIbuttons
#ifdef useLegacyButtons
#ifndef useTinkerkitLCDmodule
#ifdef useArduinoMega2560
const uint8_t lbuttonBit = 		(1 << PINK3);
const uint8_t mbuttonBit = 		(1 << PINK4);
const uint8_t rbuttonBit = 		(1 << PINK5);

const uint8_t longButtonBit =	 	(1 << PINK6); // PINK6 isn't being used for anything right now

#else // useArduinoMega2560
const uint8_t lbuttonBit = 		(1 << PINC3);
const uint8_t mbuttonBit = 		(1 << PINC4);
const uint8_t rbuttonBit = 		(1 << PINC5);

const uint8_t longButtonBit =		(1 << PINC6); // PINC6 is used as the RESET pin, so this value is safe to use for long-press signalling

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useLegacyButtons
#endif // useTWIbuttons
// this is where device-specific fuel injector and VSS pin definitions should go

#ifdef useTinkerkitLCDmodule
#ifndef PRTIM4
#define PRTIM4 4
#endif // PRTIM4
#define FI_EN_INT_O INT1
#define FI_EN_INT_C INT0
#define EICRx EICRA
#define FI_ISC_OH ISC11
#define FI_ISC_OL ISC10
#define FI_ISC_CH ISC01
#define FI_ISC_CL ISC00
#define FI_FL_INT_O INTF1
#define FI_FL_INT_C INTF0
#define INJ_OPEN_vect INT0_vect
#define INJ_CLOSE_vect INT1_vect
#define PCINTx_vect PCINT0_vect
#define PINx PINB

const uint8_t vssBit =		 	(1 << PINB7);

#else // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
#define FI_EN_INT_O INT5
#define FI_EN_INT_C INT4
#define EICRx EICRB
#define FI_ISC_OH ISC51
#define FI_ISC_OL ISC50
#define FI_ISC_CH ISC41
#define FI_ISC_CL ISC40
#define FI_FL_INT_O INTF5
#define FI_FL_INT_C INTF4
#define INJ_OPEN_vect INT4_vect
#define INJ_CLOSE_vect INT5_vect
#define PCINTx_vect PCINT2_vect
#define PINx PINK

const uint8_t vssBit =			(1 << PINK0);

#else // useArduinoMega2560
#define FI_EN_INT_O INT1
#define FI_EN_INT_C INT0
#define EICRx EICRA
#define FI_ISC_OH ISC11
#define FI_ISC_OL ISC10
#define FI_ISC_CH ISC01
#define FI_ISC_CL ISC00
#define FI_FL_INT_O INTF1
#define FI_FL_INT_C INTF0
#define INJ_OPEN_vect INT0_vect
#define INJ_CLOSE_vect INT1_vect
#define PCINTx_vect PCINT1_vect
#define PINx PINC

const uint8_t vssBit =		 	(1 << PINC0);

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
// this is where device-specific serial port 0 definitions should go

#ifdef useBufferedSerial0Port
#ifndef useTinkerkitLCDmodule
#ifdef useArduinoMega2560
#define USART0_tx_vect USART0_UDRE_vect
#define USART0_sd_vect USART0_TX_vect

#else // useArduinoMega2560
#define USART0_tx_vect USART_UDRE_vect
#define USART0_sd_vect USART_TX_vect

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useBufferedSerial0Port
// this is where device-specific serial port 1 definitions should go

#ifdef useBufferedSerial1Port
#ifdef useTinkerkitLCDmodule
#define USART1_tx_vect USART_UDRE_vect
#define USART1_sd_vect USART_TXC_vect

#else // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
#define USART1_tx_vect USART1_UDRE_vect
#define USART1_sd_vect USART1_TX_vect

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useBufferedSerial1Port
// This is where device-specific LED definitions should go

#ifdef useLEDpins
#ifdef useTinkerkitLCDmodule
const uint8_t LEDpinLinit =		(1 << DDC7);
const uint8_t LEDpinTXinit =		(1 << DDD5);
const uint8_t LEDpinRXinit =		(1 << DDB0);
const uint8_t LEDpinL =			(1 << PORTC7); // active high ?
const uint8_t LEDpinTX =		(1 << PORTD5); // active low
const uint8_t LEDpinRX =		(1 << PORTB0); // active low
#define LEDportL PORTC
#define LEDportTX PORTD
#define LEDportRX PORTB
#define LEDportLinit DDRC
#define LEDportTXinit DDRD
#define LEDportRXinit DDRB

#else // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
const uint8_t LEDpinLinit =		(1 << DDB7);
const uint8_t LEDpinL =			(1 << PORTB7); // active high
#define LEDportL PORTB
#define LEDportLinit DDRB

#else // useArduinoMega2560
const uint8_t LEDpinLinit =		(1 << DDB5);
const uint8_t LEDpinL =			(1 << PORTB5); // Really, dcb? Really? You couldn't have used something else for LCD data bit 3?
#define LEDportL PORTB
#define LEDportLinit DDRB

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useLEDpins
typedef void (* pFunc)(void); // type for display function pointers
typedef uint8_t (* pSaveFunc)(void);
typedef uint8_t (* deviceCallFunc)(uint8_t cmd, uint8_t data); // type for device event handler function pointers

const uint8_t loopsPerSecond = 2; // how many times will we try and loop in a second
const uint8_t samplesPerSecond = 2; // how many times will we try to sample the ADC output in a second

const unsigned long t2CyclesPerSecond = (unsigned long)(systemProcessorSpeed * 15625ul); // (systemProcessorSpeed * 1000000 / (timer 0 prescaler))
const unsigned long loopSystemLength = (t2CyclesPerSecond / (loopsPerSecond));
const unsigned int loopTickLength = (unsigned int)(t2CyclesPerSecond / (loopsPerSecond * 256ul));
const unsigned int sampleTickLength  = (unsigned int)(t2CyclesPerSecond / (samplesPerSecond * 256ul));
const unsigned int myubbr0 = (unsigned int)(systemProcessorSpeed * 62500ul / myBaudRate0 - 1);
const unsigned int myubbr1 = (unsigned int)(systemProcessorSpeed * 62500ul / myBaudRate1 - 1);
const unsigned int buttonDelay = (unsigned int)(t2CyclesPerSecond / 256ul);
const unsigned int buttonShortDelay = buttonDelay - (5 * buttonDelay / 100); // wait 5/100 of a second before accepting button presses
const unsigned int vssResetDelay = (unsigned int)(500ul * t2CyclesPerSecond / 256000ul); // VSS pulse timeout is 500 milliseconds
const unsigned int cursorDelay = (unsigned int)(500ul * t2CyclesPerSecond / 256000ul); // display cursor toggle period is 500 milliseconds

const unsigned int holdDelay = (unsigned int)(2000ul * t2CyclesPerSecond / 256000ul); // 2 second delay
const unsigned int delay1500ms = (unsigned int)(1500ul * t2CyclesPerSecond / 256000ul); // 1.5 second delay
const unsigned int delay0005ms = (unsigned int)(5ul * t2CyclesPerSecond / 256000ul); // 5 millisecond delay

#ifdef useJSONoutput
const unsigned int JSONtickLength = (unsigned int)(16ul * t2CyclesPerSecond / 2560ul); // 1.6 second delay

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

uint8_t odvFlagForceOutput =		0b00000100;
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

#ifdef useBuffering
const uint8_t bufferIsFull =	0b10000000;
const uint8_t bufferIsEmpty =	0b01000000;

const uint8_t rbvEmptyEvent =		0;
const uint8_t rbvNoLongerEmptyEvent =	rbvEmptyEvent + 1;
const uint8_t rbvNoLongerFullEvent =	rbvNoLongerEmptyEvent + 1;
const uint8_t rbvFullEvent =		rbvNoLongerFullEvent + 1;
const uint8_t rbvReturnOutputLength =	rbvFullEvent + 1;
const uint8_t rbvOpenOutput =		rbvReturnOutputLength + 1;
const uint8_t rbvProcessCharacter =	rbvOpenOutput + 1;
const uint8_t rbvCloseOutput =		rbvProcessCharacter + 1;
const uint8_t rbvTestIfDone =		rbvCloseOutput + 1;

typedef struct
{

	volatile uint8_t * data;
	volatile uint8_t size;
	volatile uint8_t start;
	volatile uint8_t end;
	volatile uint8_t status;

	deviceCallFunc ringBufferDeviceCall;

} ringBufferVariable;

#endif // useBuffering
#ifdef useSerialLoggingOutput
outputDevice devLogSerial;
#endif // useSerialLoggingOutput
#ifdef useSerialDebugOutput
outputDevice devDebugSerial;
#endif // useSerialDebugOutput

#ifdef useAnalogInterrupt
void initADC(uint8_t adcModeFlag);
void shutdownADC(void);
#endif // useAnalogInterrupt
#ifdef useChryslerMAPCorrection
void calculatePressure(unsigned int analogReading, uint8_t pressureIdx);
void calculateMAPcorrection(void);
#endif // useChryslerMAPCorrection
#ifdef useIsqrt
unsigned int iSqrt(unsigned int n);
#endif // useIsqrt
void updateVSS(unsigned long thisVSStime);
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
void add64(union union_64 * an, union union_64 * ann);
void sub64(union union_64 * an, union union_64 * ann);
#ifndef useAssemblyLanguage
void add64(union union_64 * an, union union_64 * ann, uint8_t mode);
#endif // useAssemblyLanguage
#ifndef useSWEET64multDiv
void mul64(void);
void div64(void);
#endif // useSWEET64multDiv
void registerTest64(union union_64 * an);
uint8_t ltOrEtest64(union union_64 * an, union union_64 * ann);
uint8_t getScreenLabelIndex(uint8_t calcIdx);
void displayMainScreenFunction(uint32_t screenFormatValue, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripList[], const uint8_t localTripBitmask[][4]);
void displayMainScreenFunction(uint8_t readingIdx, uint8_t screenFormatValue, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripList[], const uint8_t localTripBitmask[][4]);
#ifdef useBigTimeDisplay
void displayBigTime(char * val, uint8_t blinkFlag, uint8_t blinkPos);
#endif // useBigTimeDisplay
#ifdef useBigNumberDisplay
uint8_t fedSelect(uint8_t bigScreenIdx);
void displayBigNumber(const uint8_t * prgmPtr, uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalPlaces);
void displayBigNumber(char * str);
void displayBigDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar);
void displayBigStatus(uint8_t bigScreenIdx, const char * str);
#endif // useBigNumberDisplay
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
char * doFormat(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalFlag);
unsigned long convertTime(unsigned long * an);
void majorScreenLevelEntry(const char * s, uint8_t newScreenLevel);
void doCursorMoveAbsolute(uint8_t positionY, uint8_t positionX);
void doCursorMoveRelative(uint8_t moveY, uint8_t moveX);
void doCursorUpdateScreen(void);
void printStatusMessage(const char * s);
void doNothing(void);
void noSupport(void);
void doCursorUpdateMain(void); /* Main screen section */
void doMainScreenDisplay(void);
void doNextBright(void);
void doLongGoLeft(void);
void doLongGoRight(void);
void doTripResetTank(void);
void doTripResetCurrent(void);
#ifdef useBigFE // Large Fuel Economy display support section
void doCursorUpdateBigFEscreen(void);
void doBigFEdisplay(void);
#endif // useBigFE
#ifdef useBigDTE // Large Distance-To-Empty display support section
void doCursorUpdateBigDTEscreen(void);
void doBigDTEdisplay(void);
#endif // useBigDTE
#ifdef useBigTTE // Large Time-To-Empty display support section
void doCursorUpdateBigTTEscreen(void);
void doBigTTEdisplay(void);
#endif // useBigTTE
#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void);
#else // useDeepSleep
#ifdef useSoftwareClock // Software Clock support section
void doShowSoftwareClock(void);
void doGoEditSoftwareClock(void);
void doEditSoftwareClock(void);
void doEditSoftwareClockChangeDigit(void);
void doEditSoftwareClockSave(void);
void doEditSoftwareClockCancel(void);
#endif // useSoftwareClock
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
#ifdef useCPUreading // CPU loading, remaining available RAM, and function benchmarking support section
void doDisplaySystemInfo(void);
void displayCPUutil(void);
void doShowCPU(void);
unsigned int getAvailableRAMsize(void);
unsigned long cycles0(void);
#ifdef useBenchMark
void doBenchMark(void);
#endif // useBenchMark
#endif // useCPUreading
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
uint8_t generateDisplayFormat(uint8_t tripValue, uint8_t functionValue);
void doCursorUpdateScreenEdit(void);
void doScreenEditDisplay(void);
void doGoScreenEdit(void);
void doScreenEditBump(void);
void doSaveScreen(void);
void doScreenReturnToMain(void);
#endif // useScreenEditor
#ifdef useDebugReadings
void goStepDebug(void);
#endif // useDebugReadings
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
void idleProcess(void);
void performSleepMode(uint8_t sleepMode);
#ifdef useAdafruitRGBLCDmodule
void writeAdafruitRGBLCDportDirection(uint8_t interruptFlag, uint8_t readLCDflag);
void writeAdafruitRGBLCDregister(uint8_t interruptFlag);
#endif // useAdafruitRGBLCDmodule
void peripheralInit(void);
int main(void);

namespace menu /* Top-down menu selector section prototype */
{

	void display(void);
	void select(void);

};

namespace settings /* Parameter setting selector section prototype */
{

	void cursorUpdate(void);
	void display(void);
	void editParameter(void);
	void fetchParameter(void);
	void parameterChanged(void);
	uint8_t saveParameter(void);
	uint8_t EEPROMpowerUpCheck(void);
	void initGuino(void);

};

typedef struct
{

	void (* initializeParam)(void);
	uint8_t (* saveParam)(void);
	void (* paramChanged)(void);

} parameterVector;

static const parameterVector settingsParamVect PROGMEM = {
	settings::fetchParameter
	,settings::saveParameter
	,settings::parameterChanged
};

namespace parameter /* Individual parameter editor section prototype */
{

	void doCursorUpdate(void);
	void display(void);
	void entry(uint8_t parameterLength, const parameterVector * callingParameterVector);
	void saveEdit(void);
	void cancelEdit(void);
	void findLeft(void);
	void findRight(void);
	void readMaxValue(void);
	void readMinValue(void);
	void revert(void);
	void readEEPROM(uint8_t parameterPtr);
	void formatInputNumber(unsigned long v);
	void changeDigit(void);

}

typedef struct
{

	uint8_t callingScreenLevel;
	uint8_t parameterLength;
	const parameterVector * paramVect;

} parameterVariable;

parameterVariable paramVar;

namespace EEPROM /* EEPROM parameter I/O section prototype */
{

#ifdef useEEPROMviewer
	void display(void);
#endif
	uint8_t writeVal(uint8_t eePtr, unsigned long val);
	unsigned long readVal(uint8_t eePtr);
	unsigned int getAddress(uint8_t eePtr);

};

#ifdef useLEDpins
namespace LED /* built-in LED support section prototype */
{

	void init(void);
	void shutdown(void);
	void turnOn(void);
	void turnOff(void);

};

#endif // useLEDpins
#ifdef useCoastDownCalculator
const uint8_t cdtActive =			0b10000000;
const uint8_t cdtTriggered =			0b01000000;
const uint8_t cdtCancelled =			0b00100000;
const uint8_t cdtFinished =			0b00010000;
const uint8_t cdSignalStateChange =		0b00001000;
const uint8_t cdTakeSample =			0b00000100;
const uint8_t cdSampleTaken =			0b00000010;

const uint8_t cdtTestInProgress =		cdtActive | cdtTriggered;
const uint8_t cdtTestClearFlags =		cdtTestInProgress | cdtCancelled | cdtFinished | cdSignalStateChange | cdTakeSample | cdSampleTaken;

volatile uint8_t coastdownFlags;
volatile uint8_t coastdownState;

namespace coastdown /* Coastdown Calculator support section prototype */
{

	void goDisplay(void);
	void goTrigger(void);

}

#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const uint8_t accelTestActive =			0b10000000;
const uint8_t accelTestTriggered =		0b01000000;
const uint8_t accelTestFullSpeed =		0b00100000;
const uint8_t accelTestHalfSpeed =		0b00010000;
const uint8_t accelTestDistance =		0b00001000;
const uint8_t accelTestCancelled =		0b00000100;
const uint8_t accelTestFinished =		0b00000010;

const uint8_t accelTestMeasurementFlags =	accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance;
const uint8_t accelTestInProgress =		accelTestActive | accelTestTriggered;
const uint8_t accelTestCompleteFlags =		accelTestCancelled | accelTestFinished;
const uint8_t accelTestClearFlags =		accelTestInProgress | accelTestMeasurementFlags | accelTestCompleteFlags;

volatile uint8_t accelerationFlags;

namespace accelerationTest /* Acceleration Test support section prototype */
{

	void goDisplay(void);
	void goTrigger(void);
	uint8_t triggerTest(void);

}

#endif // useDragRaceFunction
#ifdef useSerial0Port
namespace serial0
{

	void init(void);
	void shutdown(void);
	void pushCharacter(uint8_t chr);
#ifdef useBufferedSerial0Port
	uint8_t bufferDevice(uint8_t cmd, uint8_t data);
#endif // useBufferedSerial0Port

};

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
	void pushCharacter(uint8_t chr);
#ifdef useBufferedSerial1Port
	uint8_t bufferDevice(uint8_t cmd, uint8_t data);
#endif // useBufferedSerial1Port

};

#ifdef useBufferedSerial1Port
ringBufferVariable serial1Buffer;

volatile uint8_t serial1Data[32];

#endif // useBufferedSerial1Port
#endif // useSerial1Port
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

const uint8_t TWI_READ =		0;
const uint8_t TWI_WRITE =		1;

const uint8_t TWI_REPEAT_START =	0;
const uint8_t TWI_STOP =		1;

const uint8_t TWI_MAIN_PROGRAM =	0;
const uint8_t TWI_INTERRUPT =		1;

const uint8_t twiMasterTransmitter =	0b10000000;
const uint8_t twiMasterReceiver =	0b01000000;
const uint8_t twiSlaveTransmitter =	0b00100000;
const uint8_t twiSlaveReceiver =	0b00010000;
const uint8_t twiSendStop =		0b00001000;
const uint8_t twiRepeatStart =		0b00000100;
const uint8_t twiErrorFlag =		0b00000010;
const uint8_t twiBlockMainProgram =	0b00000001;

const uint8_t twiTxRxStatus =		twiMasterTransmitter | twiMasterReceiver | twiSlaveTransmitter | twiSlaveReceiver;
const uint8_t twiTxRxStatusMain =	twiTxRxStatus | twiBlockMainProgram;

volatile uint8_t twiStatusFlags;

const unsigned int twiFrequency = 100L;
const uint8_t twiMasterBufferSize = 16;
const uint8_t twiSlaveRxBufferSize = 16;
const uint8_t twiSlaveTxBufferSize = 16;

static uint8_t twiMasterBufferData[twiMasterBufferSize];
static uint8_t twiSlaveTxBufferData[twiSlaveTxBufferSize];
static uint8_t twiSlaveRxBufferData[twiSlaveRxBufferSize];

volatile uint8_t twiSlaveAddress;
volatile uint8_t twiErrorCode;

volatile uint8_t twiMasterBufferIdx;
volatile uint8_t twiMasterBufferLen;
volatile uint8_t twiSlaveTxBufferIdx;
volatile uint8_t twiSlaveTxBufferLen;
volatile uint8_t twiSlaveRxBufferIdx;

#endif // useTWIsupport
const char titleMPGuino[] PROGMEM = {
	"MPGuino v1.95tav"
};

const char dateMPGuino[] PROGMEM = {
	"2022-OCT-22"
};

const uint8_t rvLength = 8;			// total length of trip variable in bytes

const uint8_t rvVSSpulseIdx = 0; 		// from the speedo
const uint8_t rvInjPulseIdx = 1; 		// rpm
const uint8_t rvVSScycleIdx = 2; 		// time that the vehicle has spent moving
const uint8_t rvInjCycleIdx = 4; 		// engine run time since this class was reset
const uint8_t rvInjOpenCycleIdx = 6; 		// time that the fuel injector has been open

#define nextAllowedValue 0
const uint8_t tFuelUsed =			nextAllowedValue;
const uint8_t tFuelRate =			tFuelUsed + 1;
const uint8_t tEngineRunTime =			tFuelRate + 1;
const uint8_t tTimeToEmpty =			tEngineRunTime + 1;
const uint8_t tDistance =			tTimeToEmpty + 1;
const uint8_t tSpeed =				tDistance + 1;
const uint8_t tMotionTime =			tSpeed + 1;
const uint8_t tFuelEcon =			tMotionTime + 1;
const uint8_t tRemainingFuel =			tFuelEcon + 1;
const uint8_t tDistanceToEmpty =		tRemainingFuel + 1;
const uint8_t tEngineSpeed =			tDistanceToEmpty + 1;
const uint8_t tInjectorOpenTime =		tEngineSpeed + 1;
const uint8_t tInjectorTotalTime =		tInjectorOpenTime + 1;
const uint8_t tVSStotalTime =			tInjectorTotalTime + 1;
const uint8_t tInjectorPulseCount =		tVSStotalTime + 1;
const uint8_t tVSSpulseCount =			tInjectorPulseCount + 1;
#define nextAllowedValue tVSSpulseCount + 1
#ifdef useFuelCost
const uint8_t tFuelCost =			nextAllowedValue;
const uint8_t tFuelRateCost =			tFuelCost + 1;
const uint8_t tFuelCostPerDistance =		tFuelRateCost + 1;
const uint8_t tDistancePerFuelCost =		tFuelCostPerDistance + 1;
const uint8_t tFuelCostRemaining =		tDistancePerFuelCost + 1;
#define nextAllowedValue tFuelCostRemaining + 1
#endif // useFuelCost
#ifdef useAnalogRead
const uint8_t tAnalogChannel =			nextAllowedValue;
#define nextAllowedValue tAnalogChannel + 1
#endif
#ifdef useChryslerMAPCorrection
const uint8_t tPressureChannel =		nextAllowedValue;
const uint8_t tCorrectionFactor =		tPressureChannel + 1;
#define nextAllowedValue tCorrectionFactor + 1
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
const uint8_t tEstimatedEnginePower =		nextAllowedValue;
const uint8_t tAccelTestTime =			tEstimatedEnginePower + 1;
const uint8_t tDragSpeed =			tAccelTestTime + 1;
#define nextAllowedValue tDragSpeed + 1
#endif

const uint8_t dfMaxValDisplayCount =		nextAllowedValue;

const uint8_t tFindRemainingFuel =		nextAllowedValue;
#define nextAllowedValue tFindRemainingFuel + 1
#ifdef useJSONoutput
const uint8_t tFindBingoFuel =			nextAllowedValue;
const uint8_t tFindReserveFuel =		tFindBingoFuel + 1;
#define nextAllowedValue tFindReserveFuel + 1
#endif // useJSONoutput
#ifdef useExpansionPins
const uint8_t tFindExpPinSetting =		nextAllowedValue;
#define nextAllowedValue tFindExpPinSetting + 1
#endif // useExpansionPins
const uint8_t tConvertToMicroSeconds =		nextAllowedValue;
const uint8_t tCalculateFuelQuantity =		tConvertToMicroSeconds + 1;
const uint8_t tCalculateDistanceToEmpty =	tCalculateFuelQuantity + 1;
const uint8_t tCalculateTimeToEmpty =		tCalculateDistanceToEmpty + 1;
const uint8_t tConvertToFiveBCDbyteNumber =	tCalculateTimeToEmpty + 1;
const uint8_t tConvertToTimerTicks =		tConvertToFiveBCDbyteNumber + 1;
#define nextAllowedValue tConvertToTimerTicks + 1

const uint8_t dfBitShift = 5;
const uint8_t dfFunctionMask =	0b00011111;
const uint8_t dfTripMask =	0b11100000;
#ifdef useScreenEditor
#ifdef trackIdleEOCdata
const uint8_t dfMaxTripCount = 6;
#else
const uint8_t dfMaxTripCount = 3;
#endif
#endif

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
#endif
;

const uint8_t tripSaveMenuSize = 3
#ifdef showSavedTrips
	+ 1
#endif
; // size of trip save menu

#ifdef showSavedTrips
const uint8_t tripShowMenuSize = 7; // size of trip show menu
#endif

#endif
#endif
#ifdef useBarGraph
const uint8_t bgDataSize = 15;

#endif
#ifdef useWindowTripFilter
const uint8_t windowTripFilterSize = 4;
uint8_t wtpCurrentIdx;

#endif
volatile uint8_t lastPINxState;

// this  is the start of the index definitions for all of the trip variables

volatile uint8_t rawTripIdx;
#ifdef trackIdleEOCdata
volatile uint8_t rawEOCidleTripIdx;
#endif

#define nextAllowedValue 0
const uint8_t raw0tripIdx =			nextAllowedValue;
const uint8_t instantIdx =			raw0tripIdx + 1;
const uint8_t currentIdx =			instantIdx + 1;
const uint8_t tankIdx =				currentIdx + 1;
#define nextAllowedValue tankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t raw0eocIdleTripIdx =		nextAllowedValue;
const uint8_t eocIdleInstantIdx =		raw0eocIdleTripIdx + 1;
const uint8_t eocIdleCurrentIdx =		eocIdleInstantIdx + 1;
const uint8_t eocIdleTankIdx =			eocIdleCurrentIdx + 1;
const uint8_t raw1eocIdleTripIdx =		eocIdleTankIdx + 1;
#define nextAllowedValue raw1eocIdleTripIdx + 1
#endif

// these trip indexes are not meant to be displayed on any main MPGuino screen, so should not be selected for mainScreenDisplayFormats[]

const uint8_t raw1tripIdx =			nextAllowedValue;
#define nextAllowedValue raw1tripIdx + 1
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
const uint8_t periodIdx =			nextAllowedValue;
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

#ifdef useAnalogRead
#define nextAllowedValue 0
#ifdef useTinkerkitLCDmodule
const uint8_t analog0Idx =		nextAllowedValue;
const uint8_t analog1Idx =		analog0Idx + 1;
const uint8_t analog2Idx =		analog1Idx + 1;
#define nextAllowedValue analog2Idx + 1
#else // useTinkerkitLCDmodule
const uint8_t analog0Idx =		nextAllowedValue;
const uint8_t analog1Idx =		analog0Idx + 1;
#define nextAllowedValue analog1Idx + 1
#ifdef useAnalogButtons
const uint8_t analog2Idx =		nextAllowedValue;
#define nextAllowedValue analog2Idx + 1
#ifndef useTWIsupport
const uint8_t analog3Idx =		nextAllowedValue;
const uint8_t analog4Idx =		analog3Idx + 1;
#define nextAllowedValue analog4Idx + 1
#endif // useTWIsupport
#endif // useAnalogButtons
#endif // useTinkerkitLCDmodule
const uint8_t dfMaxValAnalogCount =	nextAllowedValue;

#endif // useAnalogRead
#define nextAllowedValue 0
const uint8_t msInstantIdx =		nextAllowedValue;
const uint8_t msCurrentIdx =		msInstantIdx + 1;
const uint8_t msTankIdx =		msCurrentIdx + 1;
#define nextAllowedValue msTankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t msEOCidleInstantIdx =	nextAllowedValue;
const uint8_t msEOCidleCurrentIdx =	msEOCidleInstantIdx + 1;
const uint8_t msEOCidleTankIdx =	msEOCidleCurrentIdx + 1;
#define nextAllowedValue msEOCidleTankIdx + 1
#endif

const uint8_t msTripList[] PROGMEM = {
	instantIdx
	,currentIdx
	,tankIdx
#ifdef trackIdleEOCdata
	,eocIdleInstantIdx
	,eocIdleCurrentIdx
	,eocIdleTankIdx
#endif
};

const uint8_t msTripBitPattern[][4] PROGMEM = {
	{0b00000000, 0b00000111, 0b00000010, 0b00000111} // I
 	,{0b00000000, 0b00000011, 0b00000100, 0b00000011} // C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000010} // T
#ifdef trackIdleEOCdata
	,{0b00000000, 0b00000011, 0b00000010, 0b00000110} // italic I
	,{0b00000000, 0b00000011, 0b00000100, 0b00000110} // italic C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000100} // italic T
#endif
};

const uint8_t displayPageCount = 9						// count of base number of data screens
#ifdef useChryslerMAPCorrection
	+ 1									// count of Chrysler MAP-specific data screen
#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
	+ 1									// count of analog voltage data screen
#endif
#ifdef trackIdleEOCdata
	+ 3									// count of Idle/EOC tracking data screens
#endif
#ifdef useDebugReadings
	+ 1
#endif
;
const uint8_t mainScreenDisplayFormatSize = displayPageCount * 4;

#ifdef useLegacyButtons
const uint8_t buttonsUp =		rbuttonBit | mbuttonBit | lbuttonBit;

const uint8_t btnShortPressL =		rbuttonBit | mbuttonBit;
const uint8_t btnShortPressC =		rbuttonBit | lbuttonBit;
const uint8_t btnShortPressCL =		rbuttonBit;
const uint8_t btnShortPressR =		mbuttonBit | lbuttonBit;
const uint8_t btnShortPressRL =		mbuttonBit;
const uint8_t btnShortPressRC =		lbuttonBit;
const uint8_t btnShortPressRCL =	0;

#endif // useLegacyButtons
#ifdef useAnalogButtons
const uint8_t lbuttonBit = 		0b00000001;
const uint8_t mbuttonBit = 		0b00000010;
const uint8_t rbuttonBit = 		0b00000100;
const uint8_t abuttonBit = 		0b00001000;
const uint8_t bbuttonBit = 		0b00010000;

const uint8_t longButtonBit =		0b10000000;

#endif // useAnalogButtons
#ifdef useButtonCrossConfig
const uint8_t buttonsUp =		0;
const uint8_t btnShortPressL =		lbuttonBit;
const uint8_t btnShortPressC =		mbuttonBit;
const uint8_t btnShortPressCL =		mbuttonBit | lbuttonBit;
const uint8_t btnShortPressR =		rbuttonBit;
const uint8_t btnShortPressRL =		rbuttonBit | lbuttonBit;
const uint8_t btnShortPressRC =		rbuttonBit | mbuttonBit;
const uint8_t btnShortPressRCL =	rbuttonBit | btnShortPressCL;
const uint8_t btnShortPress1 =		abuttonBit;
const uint8_t btnShortPress1L =		abuttonBit | btnShortPressL;
const uint8_t btnShortPress1C =		abuttonBit | btnShortPressC;
const uint8_t btnShortPress1CL =	abuttonBit | btnShortPressCL;
const uint8_t btnShortPress1R =		abuttonBit | btnShortPressR;
const uint8_t btnShortPress1RL =	abuttonBit | btnShortPressRL;
const uint8_t btnShortPress1RC =	abuttonBit | btnShortPressRC;
const uint8_t btnShortPress1RCL =	abuttonBit | btnShortPressRCL;
const uint8_t btnShortPress2 =		bbuttonBit;
const uint8_t btnShortPress2L =		bbuttonBit | btnShortPressL;
const uint8_t btnShortPress2C =		bbuttonBit | btnShortPressC;
const uint8_t btnShortPress2CL =	bbuttonBit | btnShortPressCL;
const uint8_t btnShortPress2R =		bbuttonBit | btnShortPressR;
const uint8_t btnShortPress2RL =	bbuttonBit | btnShortPressRL;
const uint8_t btnShortPress2RC =	bbuttonBit | btnShortPressRC;
const uint8_t btnShortPress2RCL =	bbuttonBit | btnShortPressRCL;
const uint8_t btnShortPress21 =		bbuttonBit | btnShortPress1;
const uint8_t btnShortPress21L =	bbuttonBit | btnShortPress1L;
const uint8_t btnShortPress21C =	bbuttonBit | btnShortPress1C;
const uint8_t btnShortPress21CL =	bbuttonBit | btnShortPress1CL;
const uint8_t btnShortPress21R =	bbuttonBit | btnShortPress1R;
const uint8_t btnShortPress21RL =	bbuttonBit | btnShortPress1RL;
const uint8_t btnShortPress21RC =	bbuttonBit | btnShortPress1RC;
const uint8_t btnShortPress21RCL =	bbuttonBit | btnShortPress1RCL;

#endif // useButtonCrossConfig
const uint8_t btnLongPressL =		longButtonBit | btnShortPressL;
const uint8_t btnLongPressC =		longButtonBit | btnShortPressC;
const uint8_t btnLongPressCL =		longButtonBit | btnShortPressCL;
const uint8_t btnLongPressR =		longButtonBit | btnShortPressR;
const uint8_t btnLongPressRL =		longButtonBit | btnShortPressRL;
const uint8_t btnLongPressRC =		longButtonBit | btnShortPressRC;
const uint8_t btnLongPressRCL =		longButtonBit | btnShortPressRCL;

#ifdef useButtonCrossConfig
const uint8_t btnLongPress1 =		longButtonBit | btnShortPress1;
const uint8_t btnLongPress1L =		longButtonBit | btnShortPress1L;
const uint8_t btnLongPress1C =		longButtonBit | btnShortPress1C;
const uint8_t btnLongPress1CL =		longButtonBit | btnShortPress1CL;
const uint8_t btnLongPress1R =		longButtonBit | btnShortPress1R;
const uint8_t btnLongPress1RL =		longButtonBit | btnShortPress1RL;
const uint8_t btnLongPress1RC =		longButtonBit | btnShortPress1RC;
const uint8_t btnLongPress1RCL =	longButtonBit | btnShortPress1RCL;
const uint8_t btnLongPress2 =		longButtonBit | btnShortPress2;
const uint8_t btnLongPress2L =		longButtonBit | btnShortPress2L;
const uint8_t btnLongPress2C =		longButtonBit | btnShortPress2C;
const uint8_t btnLongPress2CL =		longButtonBit | btnShortPress2CL;
const uint8_t btnLongPress2R =		longButtonBit | btnShortPress2R;
const uint8_t btnLongPress2RL =		longButtonBit | btnShortPress2RL;
const uint8_t btnLongPress2RC =		longButtonBit | btnShortPress2RC;
const uint8_t btnLongPress2RCL =	longButtonBit | btnShortPress2RCL;
const uint8_t btnLongPress21 =		longButtonBit | btnShortPress21;
const uint8_t btnLongPress21L =		longButtonBit | btnShortPress21L;
const uint8_t btnLongPress21C =		longButtonBit | btnShortPress21C;
const uint8_t btnLongPress21CL =	longButtonBit | btnShortPress21CL;
const uint8_t btnLongPress21R =		longButtonBit | btnShortPress21R;
const uint8_t btnLongPress21RL =	longButtonBit | btnShortPress21RL;
const uint8_t btnLongPress21RC =	longButtonBit | btnShortPress21RC;
const uint8_t btnLongPress21RCL =	longButtonBit | btnShortPress21RCL;

#endif // useButtonCrossConfig
const uint8_t guinosig =		0b10110111;

const uint8_t dirtyInjectorMonitor =	0b10000000;
const uint8_t dirtyVSSmonitor =		0b01000000;
const uint8_t dirtyWatchInjector =	0b00100000;
const uint8_t dirtyWatchVSS =		0b00010000;
const uint8_t dirtyInjOpenRead =	0b00001000;
const uint8_t dirtyGoodInj =		0b00000100 | dirtyWatchInjector;
const uint8_t dirtyGoodVSS =		0b00000010 | dirtyWatchVSS;
const uint8_t dirtySysTick =		0b00000001;

const uint8_t afIdleFlag =		0b10000000;
const uint8_t afEOCflag =		0b01000000;
const uint8_t afButtonFlag =		0b00100000;
const uint8_t afParkFlag =		0b00010000;
const uint8_t afActivityTimeoutFlag =	0b00001000;
const uint8_t afResetCurrentTrip =	0b00000100;
const uint8_t afDoResetOnVehicle =	0b00000010;
const uint8_t afDoResetOnButton =	0b00000001;
const uint8_t afDoResetCheckFlags =	(afDoResetOnVehicle | afDoResetOnButton);
const uint8_t afValidFlags =		(afIdleFlag | afEOCflag | afParkFlag | afButtonFlag | afActivityTimeoutFlag);

const uint8_t tcDisplayDelay =		0b10000000;
const uint8_t tcDoDelay =		0b01000000;
const uint8_t tcShowCursor =		0b00100000;
const uint8_t tcUpdateDisplay =		0b00010000;
const uint8_t tcTakeSample =		0b00001000;
#ifdef useTWIsupport
const uint8_t tcBlockTWIsample =	0b00000100;
#endif // useTWIsupport
#ifdef useJSONoutput
const uint8_t tcOutputJSON =		0b00000100;
#endif // useJSONoutput

const uint8_t tsFellAsleep =		0b10000000;
const uint8_t tsAwakeOnInjector =	0b01000000;
const uint8_t tsAwakeOnVSS =		0b00100000;
const uint8_t tsAwakeOnButton =		0b00010000;
const uint8_t tsButtonRead =		0b00001000;
#ifdef useTWIsupport
const uint8_t tsTWIsampleActive =	0b00000100;
#endif // useTWIsupport
const uint8_t tsAwake =			tsAwakeOnInjector | tsAwakeOnVSS | tsAwakeOnButton;
const uint8_t tsAwakeOnVehicle =	tsAwakeOnInjector | tsAwakeOnVSS;

// start of remarkably long EEPROM stored settings section

const uint8_t pSizeSignature =			16;
#ifdef useLegacyLCD
const uint8_t pSizeContrast =			8;
#endif
#ifdef useAdafruitRGBLCDmodule
const uint8_t pSizeLCDcolor =			8;
#endif
const uint8_t pSizeMetricMode =			1;
#ifdef useDragRaceFunction
const uint8_t pSizeDragSpeed =			24;
const uint8_t pSizeDragDistance =		16;
const uint8_t pSizeDragAutoFlag =		1;
#endif // useDragRaceFunction
const uint8_t pSizeInjEdgeTrigger =		1;
#ifdef useIsqrt
const uint8_t pSizeSysFuelPressure =		32;
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pSizeRefFuelPressure =		32;
const uint8_t pSizeInjectorCount =		8;
const uint8_t pSizeInjectorSize =		20;
#endif // useCalculatedFuelFactor
const uint8_t pSizeMicroSecondsPerGallon =	32;
const uint8_t pSizeInjectorSettleTime =		16;
const uint8_t pSizePulsesPerDistance =		18;
const uint8_t pSizeVSSpause =			8;
const uint8_t pSizeCrankRevPerInj =		8;
const uint8_t pSizeMinGoodRPM =			16;
const uint8_t pSizeTankSize =			24;
#ifdef usePartialRefuel
const uint8_t pSizeRefuelSize =			24;
#endif // usePartialRefuel
#ifdef useJSONoutput
const uint8_t pSizeBingoSize =			24;
#endif // useJSONoutput
#ifdef useChryslerMAPCorrection
const uint8_t pSizeMAPsensorFloor =		16;
const uint8_t pSizeBaroSensorFloor =		16;
const uint8_t pSizeMAPsensorCeiling =		16;
const uint8_t pSizeBaroSensorCeiling =		16;
const uint8_t pSizeMAPsensorRange =		32;
const uint8_t pSizeBaroSensorRange =		32;
const uint8_t pSizeMAPsensorOffset =		32;
const uint8_t pSizeBaroSensorOffset =		32;
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
const uint8_t pSizeVehicleMass =		16;
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pSizeVehicleFrontalArea =		16;
const uint8_t pSizeLocustDensity =		16;
const uint8_t pSizeCoefficientD =		16;
const uint8_t pSizeCoefficientV =		16;
const uint8_t pSizeCoefficientRR =		16;
const uint8_t pSizeCoastdownSamplePeriod =	8;
#endif // useCoastDownCalculator
const uint8_t pSizeIdleTimeout =		8;
const uint8_t pSizeEOCtimeout =			8;
const uint8_t pSizeButtonTimeout =		8;
const uint8_t pSizeParkTimeout =		8;
const uint8_t pSizeActivityTimeout =		8;
const uint8_t pSizeWakeupResetCurrentOnEngine =	1;
const uint8_t pSizeWakeupResetCurrentOnButton =	1;
#ifdef useSerialDataLogging
const uint8_t pSizeSerialDataLogging =		1;
#endif
#ifdef useWindowTripFilter
const uint8_t pSizeWindowTripFilter =		1;
#endif
#ifdef useBarFuelEconVsTime
const uint8_t pSizeFEvsTime =			16;
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pSizeBarLowSpeedCutoff =		24;
const uint8_t pSizeBarSpeedQuantumIdx =		24;
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
const uint8_t pSizeAutoSaveActive =		1;
#endif
#ifdef useFuelCost
const uint8_t pSizeFuelUnitCost =		16;
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pSizeExpansionPin1Mode =		3;
const uint8_t pSizeExpansionPin2Mode =		3;
#endif // useExpansionPins
const uint8_t pSizeScratchpad =			32;

#define byteSize(bitLength) ((((bitLength & 0x07) != 0)? 1 : 0) + (bitLength / 8))

const uint8_t eeAdrSignature = 0;

#define nextAllowedValue eeAdrSignature
const unsigned int pAddressSignature =				nextAllowedValue;
#define nextAllowedValue pAddressSignature + byteSize(pSizeSignature)
#ifdef useLegacyLCD
const unsigned int pAddressContrast =				nextAllowedValue;
#define nextAllowedValue pAddressContrast + byteSize(pSizeContrast)
#endif
#ifdef useAdafruitRGBLCDmodule
const unsigned int pAddressLCDcolor =				nextAllowedValue;
#define nextAllowedValue pAddressLCDcolor + byteSize(pSizeLCDcolor)
#endif
const unsigned int pAddressMetricMode =				nextAllowedValue;
#define nextAllowedValue pAddressMetricMode + byteSize(pSizeMetricMode)
#ifdef useDragRaceFunction
const unsigned int pAddressDragSpeed =				nextAllowedValue;
const unsigned int pAddressDragDistance =			pAddressDragSpeed + byteSize(pSizeDragSpeed);
const unsigned int pAddressDragAutoFlag =			pAddressDragDistance + byteSize(pSizeDragDistance);
#define nextAllowedValue pAddressDragAutoFlag + byteSize(pSizeDragAutoFlag)
#endif // useDragRaceFunction
const unsigned int pAddressInjEdgeTrigger =			nextAllowedValue;
#define nextAllowedValue pAddressInjEdgeTrigger + byteSize(pSizeInjEdgeTrigger)
#ifdef useIsqrt
const unsigned int pAddressSysFuelPressure =			nextAllowedValue;
#define nextAllowedValue pAddressSysFuelPressure + byteSize(pSizeSysFuelPressure)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const unsigned int pAddressRefFuelPressure =			nextAllowedValue;
const unsigned int pAddressInjectorCount =			pAddressRefFuelPressure + byteSize(pSizeRefFuelPressure);
const unsigned int pAddressInjectorSize =			pAddressInjectorCount + byteSize(pSizeInjectorCount);
#define nextAllowedValue pAddressInjectorSize + byteSize(pSizeInjectorSize)
#endif // useCalculatedFuelFactor
const unsigned int pAddressMicroSecondsPerGallon =		nextAllowedValue;
const unsigned int pAddressInjectorSettleTime =			pAddressMicroSecondsPerGallon + byteSize(pSizeMicroSecondsPerGallon);
const unsigned int pAddressPulsesPerDistance =			pAddressInjectorSettleTime + byteSize(pSizeInjectorSettleTime);
const unsigned int pAddressVSSpause =				pAddressPulsesPerDistance + byteSize(pSizePulsesPerDistance);
const unsigned int pAddressCrankRevPerInj =			pAddressVSSpause + byteSize(pSizeVSSpause);
const unsigned int pAddressMinGoodRPM =				pAddressCrankRevPerInj + byteSize(pSizeCrankRevPerInj);
const unsigned int pAddressTankSize =				pAddressMinGoodRPM + byteSize(pSizeMinGoodRPM);
#define nextAllowedValue pAddressTankSize + byteSize(pSizeTankSize)
#ifdef usePartialRefuel
const unsigned int pAddressRefuelSize =				nextAllowedValue;
#define nextAllowedValue pAddressRefuelSize + byteSize(pSizeRefuelSize)
#endif // usePartialRefuel
#ifdef useJSONoutput
const unsigned int pAddressBingoSize =				nextAllowedValue;
#define nextAllowedValue pAddressBingoSize + byteSize(pSizeBingoSize)
#endif // useJSONoutput
#ifdef useChryslerMAPCorrection
const unsigned int pAddressMAPsensorFloor =			nextAllowedValue;
const unsigned int pAddressBaroSensorFloor =			pAddressMAPsensorFloor + byteSize(pSizeMAPsensorFloor);
const unsigned int pAddressMAPsensorCeiling =			pAddressBaroSensorFloor + byteSize(pSizeBaroSensorFloor);
const unsigned int pAddressBaroSensorCeiling =			pAddressMAPsensorCeiling + byteSize(pSizeMAPsensorCeiling);
const unsigned int pAddressMAPsensorRange =			pAddressBaroSensorCeiling + byteSize(pSizeBaroSensorCeiling);
const unsigned int pAddressBaroSensorRange =			pAddressMAPsensorRange + byteSize(pSizeMAPsensorRange);
const unsigned int pAddressMAPsensorOffset =			pAddressBaroSensorRange + byteSize(pSizeBaroSensorRange);
const unsigned int pAddressBaroSensorOffset =			pAddressMAPsensorOffset + byteSize(pSizeMAPsensorOffset);
#define nextAllowedValue pAddressBaroSensorOffset + byteSize(pSizeBaroSensorOffset)
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
const unsigned int pAddressVehicleMass =			nextAllowedValue;
#define nextAllowedValue pAddressVehicleMass + byteSize(pSizeVehicleMass)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const unsigned int pAddressVehicleFrontalArea =			nextAllowedValue;
const unsigned int pAddressLocustDensity =			pAddressVehicleFrontalArea + byteSize(pSizeVehicleFrontalArea);
const unsigned int pAddressCoefficientD =			pAddressLocustDensity + byteSize(pSizeLocustDensity);
const unsigned int pAddressCoefficientV =			pAddressCoefficientD + byteSize(pSizeCoefficientD);
const unsigned int pAddressCoefficientRR =			pAddressCoefficientV + byteSize(pSizeCoefficientV);
const unsigned int pAddressCoastdownSamplePeriod =		pAddressCoefficientRR + byteSize(pSizeCoefficientRR);
#define nextAllowedValue pAddressCoastdownSamplePeriod + byteSize(pSizeCoastdownSamplePeriod)
#endif // useCoastDownCalculator
const unsigned int pAddressIdleTimeout =			nextAllowedValue;
const unsigned int pAddressEOCtimeout =				pAddressIdleTimeout + byteSize(pSizeIdleTimeout);
const unsigned int pAddressButtonTimeout =			pAddressEOCtimeout + byteSize(pSizeEOCtimeout);
const unsigned int pAddressParkTimeout =			pAddressButtonTimeout + byteSize(pSizeButtonTimeout);
const unsigned int pAddressActivityTimeout =			pAddressParkTimeout + byteSize(pSizeParkTimeout);
const unsigned int pAddressWakeupResetCurrentOnEngine =		pAddressActivityTimeout + byteSize(pSizeActivityTimeout);
const unsigned int pAddressWakeupResetCurrentOnButton =		pAddressWakeupResetCurrentOnEngine + byteSize(pSizeWakeupResetCurrentOnEngine);
#define nextAllowedValue pAddressWakeupResetCurrentOnButton + byteSize(pSizeWakeupResetCurrentOnButton)
#ifdef useSerialDataLogging
const unsigned int pAddressSerialDataLogging =			nextAllowedValue;
#define nextAllowedValue pAddressSerialDataLogging + byteSize(pSizeSerialDataLogging)
#endif
#ifdef useWindowTripFilter
const unsigned int pAddressWindowTripFilter =			nextAllowedValue;
#define nextAllowedValue pAddressWindowTripFilter + byteSize(pSizeWindowTripFilter)
#endif
#ifdef useBarFuelEconVsTime
const unsigned int pAddressFEvsTime =				nextAllowedValue;
#define nextAllowedValue pAddressFEvsTime + byteSize(pSizeFEvsTime)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const unsigned int pAddressBarLowSpeedCutoff =			nextAllowedValue;
const unsigned int pAddressBarSpeedQuantumIdx =			pAddressBarLowSpeedCutoff + byteSize(pSizeBarLowSpeedCutoff);
#define nextAllowedValue pAddressBarSpeedQuantumIdx + byteSize(pSizeBarSpeedQuantumIdx)
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
const unsigned int pAddressAutoSaveActive =			nextAllowedValue;
#define nextAllowedValue pAddressAutoSaveActive + byteSize(pSizeAutoSaveActive)
#endif
#ifdef useFuelCost
const unsigned int pAddressFuelUnitCost =			nextAllowedValue;
#define nextAllowedValue pAddressFuelUnitCost + byteSize(pSizeFuelUnitCost)
#endif // useFuelCost
#ifdef useExpansionPins
const uint8_t pAddressExpansionPin1Mode =			nextAllowedValue;
const uint8_t pAddressExpansionPin2Mode =			pAddressExpansionPin1Mode + byteSize(pSizeExpansionPin1Mode);
#define nextAllowedValue pAddressExpansionPin2Mode + byteSize(pSizeExpansionPin2Mode)
#endif // useExpansionPins
const unsigned int pAddressScratchpad =				nextAllowedValue;
#define nextAllowedValue pAddressScratchpad + byteSize(pSizeScratchpad)

const unsigned int eeAdrSettingsEnd =				nextAllowedValue;

#ifdef useScreenEditor
const unsigned int eeAdrScreensStart =				nextAllowedValue;
const unsigned int eeAdrScreensEnd =				eeAdrScreensStart + mainScreenDisplayFormatSize;
#define nextAllowedValue eeAdrScreensEnd

#endif
#ifdef useEEPROMtripStorage
const unsigned int eeAdrSavedTripsStart = 			nextAllowedValue;

// figure out how much space is available for saving trip variables
const unsigned int eeAdrRemainingEEPROM =			(unsigned int)(E2END) - eeAdrSavedTripsStart + 1;
// figure out how many trip variables can actually be saved in EEPROM
const uint8_t tripMaxSaveSlotCount =				(uint8_t)(eeAdrRemainingEEPROM / (unsigned int)(tripSlotAdrSize));
// figure out the maximum allowable number of trip variables that can be saved
const uint8_t tripAllowedSaveSlotCount =			((tripSaveSlotCount > tripMaxSaveSlotCount) ? tripMaxSaveSlotCount : tripSaveSlotCount);

const unsigned int eeAdrSavedTripsEnd =				eeAdrSavedTripsStart + ((unsigned int)(tripSlotAdrSize) * (unsigned int)(tripAllowedSaveSlotCount));
#define nextAllowedValue eeAdrSavedTripsEnd

#endif
#define nextAllowedValue 0
const uint8_t pSignatureIdx =			nextAllowedValue;
#define nextAllowedValue pSignatureIdx + 1
const uint8_t eePtrSettingsStart =		nextAllowedValue;
#ifdef useLegacyLCD
const uint8_t pContrastIdx =			nextAllowedValue;
#define nextAllowedValue pContrastIdx + 1
#endif
#ifdef useAdafruitRGBLCDmodule
const uint8_t pLCDcolorIdx =			nextAllowedValue;
#define nextAllowedValue pLCDcolorIdx + 1
#endif
const uint8_t pMetricModeIdx =			nextAllowedValue;
#define nextAllowedValue pMetricModeIdx + 1
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
const uint8_t pInjectorSettleTimeIdx =		pMicroSecondsPerGallonIdx + 1;
const uint8_t pPulsesPerDistanceIdx =		pInjectorSettleTimeIdx + 1;
const uint8_t pVSSpauseIdx =			pPulsesPerDistanceIdx + 1;
const uint8_t pCrankRevPerInjIdx =		pVSSpauseIdx + 1;
const uint8_t pMinGoodRPMidx =			pCrankRevPerInjIdx + 1;
const uint8_t pTankSizeIdx =			pMinGoodRPMidx + 1;
#define nextAllowedValue pTankSizeIdx + 1
#ifdef usePartialRefuel
const uint8_t pRefuelSizeIdx =			nextAllowedValue;
#define nextAllowedValue pRefuelSizeIdx + 1
#endif // usePartialRefuel
#ifdef useJSONoutput
const uint8_t pTankBingoIdx =			nextAllowedValue;
#define nextAllowedValue pTankBingoIdx + 1
#endif // useJSONoutput
#ifdef useChryslerMAPCorrection
const uint8_t pMAPsensorFloorIdx =		nextAllowedValue;
const uint8_t pBaroSensorFloorIdx =		pMAPsensorFloorIdx + 1;
const uint8_t pMAPsensorCeilingIdx =		pBaroSensorFloorIdx + 1;
const uint8_t pBaroSensorCeilingIdx =		pMAPsensorCeilingIdx + 1;
const uint8_t pMAPsensorRangeIdx =		pBaroSensorCeilingIdx + 1;
const uint8_t pBaroSensorRangeIdx =		pMAPsensorRangeIdx + 1;
const uint8_t pMAPsensorOffsetIdx =		pBaroSensorRangeIdx + 1;
const uint8_t pBaroSensorOffsetIdx =		pMAPsensorOffsetIdx + 1;
#define nextAllowedValue pBaroSensorOffsetIdx + 1
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
const uint8_t pVehicleMassIdx =			nextAllowedValue;
#define nextAllowedValue pVehicleMassIdx + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pVehicleFrontalAreaIdx =		nextAllowedValue;
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
#ifdef useSerialDataLogging
const uint8_t pSerialDataLoggingIdx =		nextAllowedValue;
#define nextAllowedValue pSerialDataLoggingIdx + 1
#endif
#ifdef useWindowTripFilter
const uint8_t pWindowTripFilterIdx =		nextAllowedValue;
#define nextAllowedValue pWindowTripFilterIdx + 1
#endif
#ifdef useBarFuelEconVsTime
const uint8_t pFEvsTimeIdx =			nextAllowedValue;
#define nextAllowedValue pFEvsTimeIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pBarLowSpeedCutoffIdx =		nextAllowedValue;
const uint8_t pBarSpeedQuantumIdx =		pBarLowSpeedCutoffIdx + 1;
#define nextAllowedValue pBarSpeedQuantumIdx + 1
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
const uint8_t pAutoSaveActiveIdx =		nextAllowedValue;
#define nextAllowedValue pAutoSaveActiveIdx + 1
#endif
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

const uint8_t eePtrSettingsEnd =		nextAllowedValue;

#ifdef useScreenEditor
const uint8_t eePtrScreensStart =		nextAllowedValue;
const uint8_t eePtrScreensEnd =			eePtrScreensStart + mainScreenDisplayFormatSize;
#define nextAllowedValue eePtrScreensEnd

#endif
#ifdef useEEPROMtripStorage
const uint8_t eePtrSavedTripsStart = 		nextAllowedValue;
const uint8_t eePtrSavedTripsEnd =		eePtrSavedTripsStart + (tripSlotPtrSize * tripAllowedSaveSlotCount);
#define nextAllowedValue eePtrSavedTripsEnd

#endif
const uint8_t eePtrEnd =			nextAllowedValue;

const char parmLabels[] PROGMEM = {
#ifdef useLegacyLCD
	"Contrast\0"
#endif
#ifdef useAdafruitRGBLCDmodule
	"LCD BG Color\0"
#endif
	"Metric 1-Yes\0"
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
	"Inj Delay (uS)\0"
	"VSS Pulses/\xfbMile\xfckm\xfd\0"
	"VSS Delay (ms)\0"
	"Revs/Inj Pulse\0"
	"Min Good RPM\0"
#ifdef useImperialGallon
	"Tank*1000 \xfbImpGal\xfcL\xfd\0"
#else // useImperialGallon
	"Tank*1000 \xfbUSgal\xfcL\xfd\0"
#endif // useImperialGallon
#ifdef usePartialRefuel
#ifdef useImperialGallon
	"Refill*1K \xfbImpGal\xfcL\xfd\0"
#else // useImperialGallon
	"Refill*1K \xfbUSgal\xfcL\xfd\0"
#endif // useImperialGallon
#endif // usePartialRefuel
#ifdef useJSONoutput
#ifdef useImperialGallon
	"Bingo*1000 \xfbImpGal\xfcL\xfd\0"
#else // useImperialGallon
	"Bingo*1000 \xfbUSgal\xfcL\xfd\0"
#endif // useImperialGallon
#endif // useJSONoutput
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
#ifdef useSerialDataLogging
	"DLogSerial 1-Yes\0"
#endif
#ifdef useWindowTripFilter
	"FE Filter 1-Y\0"
#endif
#ifdef useBarFuelEconVsTime
	"FE/Time Period s\0"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"bgLower*1000 \xfbMPH\xfckph\xfd\0"
	"bgSize*1000 \xfbMPH\xfckph\xfd\0"
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
	"AutoSaveTrip 1-Y\0"
#endif
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
};

const uint8_t paramsLength[] PROGMEM = {
	pSizeSignature					// EEPROM MPGuino signature long word
#ifdef useLegacyLCD
	,pSizeContrast					// LCD Contrast
#endif
#ifdef useAdafruitRGBLCDmodule
	,pSizeLCDcolor					// LCD Backlight color
#endif
	,pSizeMetricMode				// Display Mode (0 - US Display, 1 - Metric Display)
#ifdef useDragRaceFunction
	,pSizeDragSpeed					// Drag speed cutoff (MPH or kph) * 1000
	,pSizeDragDistance				// Drag distance cutoff (miles or km) * 1000
	,pSizeDragAutoFlag				// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction
	,pSizeInjEdgeTrigger 				// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
#ifdef useIsqrt
	,pSizeSysFuelPressure				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,pSizeRefFuelPressure				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,pSizeInjectorCount				// Fuel Injector Count
	,pSizeInjectorSize				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,pSizeMicroSecondsPerGallon			// Microseconds per US gallon
	,pSizeInjectorSettleTime			// Fuel Injector Response Delay Time (us)
	,pSizePulsesPerDistance				// VSS Pulses (per mile or per km)
	,pSizeVSSpause					// VSS Pause Debounce Count (ms)
	,pSizeCrankRevPerInj				// Crankshaft Revolutions per Fuel Injector Event
	,pSizeMinGoodRPM				// Minimum Engine Speed For Engine On (RPM)
	,pSizeTankSize					// Tank Capacity * 1000 (gal or L)
#ifdef usePartialRefuel
	,pSizeRefuelSize				// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
#ifdef useJSONoutput
	,pSizeBingoSize					// Bingo Fuel (reserve tank) Capacity * 1000 (gal or L)
#endif // useJSONoutput
#ifdef useChryslerMAPCorrection
	,pSizeMAPsensorFloor				// MAP Sensor Floor * 1000 (V)
	,pSizeBaroSensorFloor				// Barometric Sensor Floor * 1000 (V)
	,pSizeMAPsensorCeiling				// MAP Sensor Ceiling * 1000 (V)
	,pSizeBaroSensorCeiling				// Barometric Sensor Ceiling * 1000 (V)
	,pSizeMAPsensorRange				// MAP Sensor Range (Pa or * 1000 psig)
	,pSizeBaroSensorRange				// Barometric Sensor Range (Pa or * 1000 psig)
	,pSizeMAPsensorOffset				// MAP Sensor Offset (Pa or * 1000 psia)
	,pSizeBaroSensorOffset				// Barometric Sensor Offset (Pa or * 1000 psia)
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
	,pSizeVehicleMass				// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,pSizeVehicleFrontalArea			// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,pSizeLocustDensity				// Air density (lb/yd^3 or kg/m^3)
	,pSizeCoefficientD				// Vehicle C(d) * 1000
	,pSizeCoefficientV				// Vehicle C(v) * 1000
	,pSizeCoefficientRR				// Vehicle C(rr) * 1000
	,pSizeCoastdownSamplePeriod			// Sample Period in seconds
#endif // useCoastDownCalculator
	,pSizeIdleTimeout				// Engine Idle Timeout (s)
	,pSizeEOCtimeout				// Engine-Off Coasting Timeout (s)
	,pSizeButtonTimeout				// Button Press Activity Timeout (s)
	,pSizeParkTimeout				// Vehicle Parked (engine off, no movement) Timeout (s)
	,pSizeActivityTimeout				// Activity Timeout (s)
	,pSizeWakeupResetCurrentOnEngine		// Enable current trip reset upon wakeup due to engine running
	,pSizeWakeupResetCurrentOnButton		// Enable current trip reset upon wakeup due to button press
#ifdef useSerialDataLogging
	,pSizeSerialDataLogging				// Serial Data Logging Enable
#endif
#ifdef useWindowTripFilter
	,pSizeWindowTripFilter				// Window Trip Filter Enable
#endif
#ifdef useBarFuelEconVsTime
	,pSizeFEvsTime					// Period Of FE over Time BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,pSizeBarLowSpeedCutoff				// FE vs Speed Bargraph lower speed
	,pSizeBarSpeedQuantumIdx			// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
	,pSizeAutoSaveActive				// Autosave Active Trip Data Enable
#endif
#ifdef useFuelCost
	,pSizeFuelUnitCost				// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	pSizeExpansionPin1Mode,				// Output Pin 1 mode
	pSizeExpansionPin2Mode,				// Output Pin 2 mode
#endif // useExpansionPins
	,pSizeScratchpad				// Scratchpad Memory
};

const uint8_t paramAddrs[] PROGMEM = {
	(uint8_t)(pAddressSignature)			// EEPROM signature long word for MPGuino
#ifdef useLegacyLCD
	,(uint8_t)(pAddressContrast)			// LCD Contrast
#endif
#ifdef useAdafruitRGBLCDmodule
	,(uint8_t)(pAddressLCDcolor)			// LCD Backlight color
#endif
	,(uint8_t)(pAddressMetricMode)			// Display Mode (0 - US Display, 1 - Metric Display)
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
	,(uint8_t)(pAddressInjectorSettleTime)		// Fuel Injector Response Delay Time (us)
	,(uint8_t)(pAddressPulsesPerDistance)		// VSS Pulses (per mile or per km)
	,(uint8_t)(pAddressVSSpause)			// VSS Pause Debounce Count (ms)
	,(uint8_t)(pAddressCrankRevPerInj)		// Crankshaft Revolutions per Fuel Injector Event
	,(uint8_t)(pAddressMinGoodRPM)			// Minimum Engine Speed For Engine On (RPM)
	,(uint8_t)(pAddressTankSize)			// Tank Capacity * 1000 (gal or L)
#ifdef usePartialRefuel
	,(uint8_t)(pAddressRefuelSize)			// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
#ifdef useJSONoutput
	,(uint8_t)(pAddressBingoSize)			// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#endif // useJSONoutput
#ifdef useChryslerMAPCorrection
	,(uint8_t)(pAddressMAPsensorFloor)		// MAP Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressBaroSensorFloor)		// Barometric Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressMAPsensorCeiling)		// MAP Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressBaroSensorCeiling)		// Barometric Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressMAPsensorRange)		// MAP Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressBaroSensorRange)		// Barometric Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressMAPsensorOffset)		// MAP Sensor Offset (Pa or * 1000 psia)
	,(uint8_t)(pAddressBaroSensorOffset)		// Barometric Sensor Offset (Pa or * 1000 psia)
#endif // useChryslerMAPCorrection
#ifdef useVehicleMass
	,(uint8_t)(pAddressVehicleMass)			// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,(uint8_t)(pAddressVehicleFrontalArea)		// Vehicle Frontal Area * 1000 (ft^2 or m^2)
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
	,(uint8_t)(pAddressActivityTimeout)		// Activity Timeout (s)
	,(uint8_t)(pAddressWakeupResetCurrentOnEngine)	// Enable current trip reset upon wakeup due to engine running
	,(uint8_t)(pAddressWakeupResetCurrentOnButton)	// Enable current trip reset upon wakeup due to button press
#ifdef useSerialDataLogging
	,(uint8_t)(pAddressSerialDataLogging)		// Serial Data Logging Enable
#endif
#ifdef useWindowTripFilter
	,(uint8_t)(pAddressWindowTripFilter)		// Window Trip Filter Enable
#endif
#ifdef useBarFuelEconVsTime
	,(uint8_t)(pAddressFEvsTime)			// Period Of FE over Time Bar Graph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,(uint8_t)(pAddressBarLowSpeedCutoff)		// FE vs Speed Bargraph lower speed
	,(uint8_t)(pAddressBarSpeedQuantumIdx)		// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
	,(uint8_t)(pAddressAutoSaveActive)		// Autosave Active Trip Data Enable
#endif
#ifdef useFuelCost
	,(uint8_t)(pAddressFuelUnitCost)		// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,(uint8_t)(pAddressExpansionPin1Mode)		// Output Pin 1 mode
	,(uint8_t)(pAddressExpansionPin2Mode)		// Output Pin 2 mode
#endif // useExpansionPins
	,(uint8_t)(pAddressScratchpad)			// Scratchpad Memory
	,(uint8_t)(eeAdrSettingsEnd)			// Start address of next EEPROM logical block
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
//	{16	,newEEPROMsignature}	// EEPROM MPGuino signature long word
//#ifdef useLegacyLCD
//	,{8	,55}			// LCD Contrast
//#endif
//#ifdef useAdafruitRGBLCDmodule
//	,{3	,3}			// LCD Backlight color
//#endif
//	,{1	,0}			// Display Mode (0 - US Display, 1 - Metric Display)
//#ifdef useDragRaceFunction
//	,{17	,60000}			// Drag speed cutoff (MPH or kph) * 1000
//	,{11	,250}			// Drag distance cutoff (miles or km) * 1000
//	,{1	,0}			// Drag automatic retrigger on vehicle stop
//#endif // useDragRaceFunction
//	,{1	,0}			// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
//#ifdef useIsqrt
//	,{24	,43500}			// Fuel System Pressure (Pa or * 1000 psig)
//#endif // useIsqrt
//#ifdef useCalculatedFuelFactor
//	,{24	,43500}			// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
//	,{8	,6}			// Fuel Injector Count
//	,{20	,246000}		// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
//#endif // useCalculatedFuelFactor
//	,{32	,133262651}		// Microseconds per US gallon
//	,{11	,550}			// Fuel Injector Response Delay Time (us)
//	,{14	,10000}			// VSS Pulses (per mile or per km)
//	,{4	,2}			// VSS Pause Debounce Count (ms)
//	,{2	,2}			// Crankshaft Revolutions per Fuel Injector Event
//	,{14	,100}			// Minimum Engine Speed For Engine On (RPM)
//	,{24	,18000}			// Tank Capacity * 1000 (gal or L)
//#ifdef usePartialRefuel
//	,{24	,0}			// Partial Refuel amount * 1000 (gal or L)
//#endif // usePartialRefuel
//#ifdef useJSONoutput
//	,{24	,2200}			// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
//#endif // useJSONoutput
//#ifdef useChryslerMAPCorrection
//	,{16	,0}			// MAP Sensor Floor * 1000 (V)
//	,{16	,0}			// Barometric Sensor Floor * 1000 (V)
//	,{16	,4500}			// MAP Sensor Ceiling * 1000 (V)
//	,{16	,4500}			// Barometric Sensor Ceiling * 1000 (V)
//	,{24	,14270}			// MAP Sensor Range (Pa or * 1000 psig)
//	,{24	,0}			// Barometric Sensor Range (Pa or * 1000 psig)
//	,{24	,551}			// MAP Sensor Offset (Pa or * 1000 psia)
//	,{24	,14696}			// Barometric Sensor Offset (Pa or * 1000 psia)
//#endif // useChryslerMAPCorrection
//#ifdef useVehicleMass
//	,{16	,4000}			// Vehicle Weight/Mass (lbs or kg)
//#endif // useVehicleMass
//#ifdef useCoastDownCalculator
//	,{16	,25400}			// Vehicle Frontal Area (ft^2 or m^2)
//	,{16	,2065}			// Air density (lb/yd^3 or kg/m^3)
//	,{16	,346}			// C(d)
//	,{16	,1}			// C(v)
//	,{16	,8}			// C(rr)
//	,{8	,2}			// Sample Period in seconds
//#endif // useCoastDownCalculator
//	,{8	,15}			// Engine Idle Timeout (s)
//	,{8	,2}			// Engine-Off Coasting Timeout (s)
//	,{8	,5}			// Button Press Activity Timeout (s)
//	,{8	,5}			// Vehicle Parked (engine off, no movement) Timeout (s)
//	,{8	,120}			// Activity Timeout (s)
//	,{1	,1}			// Enable current trip reset upon wakeup due to engine running
//	,{1	,0}			// Enable current trip reset upon wakeup due to button press
//#ifdef useSerialDataLogging
//	,{1	,1}			// Serial Data Logging Enable
//#endif
//#ifdef useWindowTripFilter
//	,{1	,1}			// Window Trip Filter Enable
//#ifdef useBarFuelEconVsTime
//	,{16	,5}			// Length Of BarGraph Bar (s)
//#endif // useBarFuelEconVsTime
//#ifdef useBarFuelEconVsSpeed
//	,{17	,25000}			// FE vs Speed Bargraph lower speed
//	,{17	,5000}			// FE vs Speed Bargraph speed bar size
//#endif // useBarFuelEconVsSpeed
//#ifdef useSavedTrips
//	,{1	,1}			// Autosave Active Trip Data Enable
//#endif
//#ifdef useFuelCost
//	,{16	,3799}			// Price per unit volume of fuel
//#endif // useFuelCost
//#ifdef useExpansionPins
//	,{3	,0}			// Output pin 0 mode
//	,{3	,0}			// Output pin 1 mode
//#endif // useExpansionPins
//	,{32	,0}			// Scratchpad Memory
//};

const uint32_t params[] PROGMEM = {
	newEEPROMsignature		// EEPROM MPGuino signature long word
#ifdef useLegacyLCD
	,55				// LCD Contrast
#endif
#ifdef useAdafruitRGBLCDmodule
	,3				// LCD Backlight color
#endif
	,0				// Display Mode (0 - US Display, 1 - Metric Display)
#ifdef useDragRaceFunction
	,60000				// Drag speed cutoff (MPH or kph) * 1000
	,250				// Drag distance cutoff (miles or km) * 1000
	,0				// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction
	,0				// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
#ifdef useIsqrt
	,43500				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,43500				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,6				// Fuel Injector Count
	,246000				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,133262651			// Microseconds per US gallon
	,550				// Fuel Injector Response Delay Time (us)
	,10000				// VSS Pulses (per mile or per km)
	,2				// VSS Pause Debounce Count (ms)
	,2				// Crankshaft Revolutions per Fuel Injector Event
	,100				// Minimum Engine Speed For Engine On (RPM)
	,18000				// Tank Capacity * 1000 (gal or L)
#ifdef usePartialRefuel
	,0				// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
#ifdef useJSONoutput
	,2200				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#endif // useJSONoutput
#ifdef useChryslerMAPCorrection
	,0				// MAP Sensor Floor * 1000 (V)
	,0				// Barometric Sensor Floor * 1000 (V)
	,4500				// MAP Sensor Ceiling * 1000 (V)
	,4500				// Barometric Sensor Ceiling * 1000 (V)
	,14270				// MAP Sensor Range (Pa or * 1000 psig)
	,0				// Barometric Sensor Range (Pa or * 1000 psig)
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
	,1				// C(v)
	,8				// C(rr)
	,3				// Sample Period in seconds
#endif // useCoastDownCalculator
	,15				// Engine Idle Timeout (s)
	,2				// Engine-Off Coasting Timeout (s)
	,5				// Button Press Activity Timeout (s)
	,5				// Vehicle Parked (engine off, no movement) Timeout (s)
	,120				// Activity Timeout (s)
	,1				// Enable current trip reset upon wakeup due to engine running
	,0				// Enable current trip reset upon wakeup due to button press
#ifdef useSerialDataLogging
	,1				// Serial Data Logging Enable
#endif
#ifdef useWindowTripFilter
	,1				// Window Trip Filter Enable
#endif
#ifdef useBarFuelEconVsTime
	,5				// Length Of BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,25000				// FE vs Speed Bargraph lower speed
	,5000				// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed
#ifdef useSavedTrips
	,1				// Autosave Active Trip Data Enable
#endif
#ifdef useFuelCost
	,3799				// Price per unit volume of fuel
#endif // useFuelCost
#ifdef useExpansionPins
	,0				// Output pin 1 mode
	,0				// Output pin 2 mode
#endif // useExpansionPins
	,0				// Scratchpad Memory
};

// end of remarkably long EEPROM stored settings section

const char overFlowStr[] PROGMEM = "----------";
const char overFlow9Str[] PROGMEM = "9999999999";

const uint8_t dfAdjustWindow =		0b00000001;
const uint8_t dfOverflow9s =		0b10000000;

const char bigFEDispChars[] PROGMEM = {
	"INST\0"
	"CURR\0"
	"TANK\0"
#ifdef trackIdleEOCdata
	"cC/I\0"
	"tC/I\0"
#endif
};

const char paramButtonChars[] PROGMEM = {
	" OK\0"
	" XX\0"
};

#ifdef useBigNumberDisplay
uint8_t cgramMode;

const uint8_t cgramBigNum = 1;

#ifdef useSpiffyBigChars
const char bigNumChars1[] PROGMEM = {
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

const char bigNumChars2[] PROGMEM = {
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

const char bigNumFont[] PROGMEM = {
	cgramBigNum, // font code
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
const char bigNumChars1[] PROGMEM = {
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

const char bigNumChars2[] PROGMEM = {
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

const char bigNumFont[] PROGMEM = {
	cgramBigNum, // font code
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
#endif // useBigNumberDisplay

const uint8_t tripUpdateSrcList[] PROGMEM = {
#ifdef trackIdleEOCdata
	eocIdleInstantIdx					// update idle tank trip with idle instant
	,eocIdleInstantIdx					// update idle current trip with idle instant
	,eocIdleInstantIdx,					// update instant trip with idle instant
#endif // trackIdleEOCdata
	instantIdx						// update tank trip with instant trip
	,instantIdx						// update current trip with instant trip
#ifdef useBarFuelEconVsTime
	,instantIdx						// update bargraph periodic trip with instant trip
#endif // useBarFuelEconVsTime
};

const uint8_t tripUpdateDestList[] PROGMEM = {
#ifdef trackIdleEOCdata
	eocIdleTankIdx 						// update idle tank trip with idle instant
	,eocIdleCurrentIdx 					// update idle current trip with idle instant
	,instantIdx,						// update instant trip with idle instant
#endif // trackIdleEOCdata
	tankIdx							// update tank trip with instant trip
	,currentIdx						// update current trip with instant trip
#ifdef useBarFuelEconVsTime
	,periodIdx 						// update bargraph periodic trip with instant trip
#endif // useBarFuelEconVsTime
	,raw0tripIdx						// reset raw 0 trip index
	,raw1tripIdx						// reset raw 1 trip index
#ifdef trackIdleEOCdata
	,raw0eocIdleTripIdx					// reset raw 0 EOC/idle trip index
	,raw1eocIdleTripIdx					// reset raw 1 EOC/idle trip index
#endif // trackIdleEOCdata
	,instantIdx						// reset instant trip index
};

const uint8_t tUScount = (sizeof(tripUpdateSrcList) / sizeof(uint8_t));
const uint8_t tUDcount = (sizeof(tripUpdateDestList) / sizeof(uint8_t));

#define nextAllowedValue 0
const uint8_t idxNumerDistance =		nextAllowedValue;
const uint8_t idxDenomDistance =		idxNumerDistance + 1;
const uint8_t idxNumerVolume =			idxDenomDistance + 1;
const uint8_t idxDenomVolume =			idxNumerVolume + 1;
#define nextAllowedValue idxDenomVolume + 1
#ifdef useCPUreading
const uint8_t idxNumerCPUutil =			nextAllowedValue;
const uint8_t idxDenomCPUutil =			idxNumerCPUutil + 1;
#define nextAllowedValue idxDenomCPUutil + 1
#endif // useCPUreading
#ifdef useIsqrt
const uint8_t idxNumerPressure =		nextAllowedValue;
const uint8_t idxDenomPressure =		idxNumerPressure + 1;
#define nextAllowedValue idxDenomPressure + 1
#endif // useIsqrt
#ifdef useAnalogRead
const uint8_t idxNumerVoltage =			nextAllowedValue;
const uint8_t idxDenomVoltage =			idxNumerVoltage + 1;
#define nextAllowedValue idxDenomVoltage + 1
#endif // useAnalogRead
#ifdef useVehicleMass
const uint8_t idxNumerMass =			nextAllowedValue;
const uint8_t idxDenomMass =			idxNumerMass + 1;
#define nextAllowedValue idxDenomMass + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t idxNumerArea =			nextAllowedValue;
const uint8_t idxDenomArea =			idxNumerArea + 1;
const uint8_t idxNumerDensity =			idxDenomArea + 1;
const uint8_t idxDenomDensity =			idxNumerDensity + 1;
#define nextAllowedValue idxDenomDensity + 1
#endif // useCoastDownCalculator
#ifdef useSoftwareClock
const uint8_t idxSecondsPerDay =		nextAllowedValue;
#define nextAllowedValue idxSecondsPerDay + 1
#endif // useSoftwareClock
#ifdef useIsqrt
const uint8_t idxCorrFactor =			nextAllowedValue;
#define nextAllowedValue idxCorrFactor + 1
#endif // useIsqrt
#ifdef useImperialGallon
const uint8_t idxNumerImperialGallon =		nextAllowedValue;
const uint8_t idxDenomImperialGallon =		idxNumerImperialGallon + 1;
#define nextAllowedValue idxDenomImperialGallon + 1
#endif // useImperialGallon
const uint8_t idxCyclesPerSecond =		nextAllowedValue;
const uint8_t idxMicroSecondsPerSecond =	idxCyclesPerSecond + 1;
const uint8_t idxDecimalPoint =			idxMicroSecondsPerSecond + 1;
const uint8_t idxOneThousand =			idxDecimalPoint;
const uint8_t idxMetricFE =			idxDecimalPoint + 1;
const uint8_t idxSecondsPerHour =		idxMetricFE + 1;
const uint8_t idxNumber500 =			idxSecondsPerHour + 1;
const uint8_t idxBCDdivisor =			idxNumber500 + 1;
#define nextAllowedValue idxBCDdivisor + 1
#ifdef useBigNumberDisplay
const uint8_t idxNumber6nines =			nextAllowedValue;
const uint8_t idxNumber5nines =			idxNumber6nines + 1;
const uint8_t idxNumber4nines =			idxNumber5nines + 1;
#define nextAllowedValue idxNumber4nines + 1
#endif // useBigNumberDisplay
#ifdef useDragRaceFunction
const uint8_t idxPowerFactor =			nextAllowedValue;
#define nextAllowedValue idxPowerFactor + 1
#endif // useDragRaceFunction


const uint32_t convNumbers[] PROGMEM = {
	1609344ul					// numerator to convert miles to kilometers
	,1000000ul					// denominator to convert miles to kilometers
	,3785411784ul					// numerator to convert US gallons to liters
	,1000000000ul					// denominator to convert US gallons to liters
#ifdef useCPUreading
	,100000ul					// numerator to convert loop count to CPU utilization percentage
	,loopSystemLength				// denominator to convert loop count to CPU utilization percentage
#endif // useCPUreading
#ifdef useIsqrt
	,68947573ul					// numerator to convert psig to kPa
	,10000000ul					// denominator to convert psig to kPa
#endif // useIsqrt
#ifdef useAnalogRead
	,1024ul						// numerator to convert volts DC to ADC steps
	,5000ul						// denominator to convert volts DC to ADC steps
#endif // useAnalogRead
#ifdef useVehicleMass
	,1000000000ul					// numerator to convert pounds to kilograms
	,2204622621ul					// denominator to convert pounds to kilograms
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,9290304ul					// numerator to convert square feet to square meters
	,100000000ul					// denominator to convert square feet to square meters
	,100000ul					// numerator to convert SAE density to metric density
	,168555ul					// denominator to convert SAE density to metric density
#endif // useCoastDownCalculator
#ifdef useSoftwareClock
	,86400ul					// number of seconds in a day
#endif // useSoftwareClock
#ifdef useIsqrt
	,4096ul						// correction factor seed for square root function
#endif // useIsqrt
#ifdef useImperialGallon
	,454609ul					// numerator to convert Imperial gallons to liters
	,100000ul					// denominator to convert Imperial gallons to liters
#endif // useImperialGallon
	,t2CyclesPerSecond				// clock cycles per second
	,1000000ul					// microseconds per second
	,1000ul						// decimal point format (the basis for all of those '* 1000' parameters)
	,100000ul					// decimal point format * 100 for metric FE (L / 100km)
	,3600ul						// seconds per hour
	,500ul						// round to nearest whole number, for numeric formatting
	,100000000ul					// divisor to separate lower 4 BCD bytes from 5th byte
#ifdef useBigNumberDisplay
	,1000000ul					// 999.999 + 1  - numeric formatting
	,100000ul					// 99.999 + 1   - numeric formatting
	,10000ul					// 9.999 + 1    - numeric formatting
#endif // useBigNumberDisplay
#ifdef useDragRaceFunction
	,22840ul					// 22.84, or vehicle speed division factor for accel test power estimation function ( 228.4/10 for internal calculations)
#endif // useDragRaceFunction
};

const uint8_t convIdx[] PROGMEM = {
	pPulsesPerDistanceIdx
	,pTankSizeIdx
#ifdef usePartialRefuel
	,pRefuelSizeIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,pDragSpeedIdx
	,pDragDistanceIdx
#endif // useDragRaceFunction
#ifdef useJSONoutput
	,pTankBingoIdx
#endif // useJSONoutput
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
	,idxNumerVolume							// pTankSizeIdx
#ifdef usePartialRefuel
	,idxNumerVolume							// pRefuelSizeIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,idxNumerDistance						// pDragSpeedIdx
	,idxNumerDistance						// pDragDistanceIdx
#endif // useDragRaceFunction
#ifdef useJSONoutput
	,idxNumerVolume							// pTankBingoIdx
#endif // useJSONoutput
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

#define nextAllowedValue 0
const uint8_t vIdleTimeoutIdx =			nextAllowedValue;
const uint8_t vEOCtimeoutIdx =			vIdleTimeoutIdx + 1;
const uint8_t vButtonTimeoutIdx =		vEOCtimeoutIdx + 1;
const uint8_t vParkTimeoutIdx =			vButtonTimeoutIdx + 1;
const uint8_t vActivityTimeoutIdx =		vParkTimeoutIdx + 1;
const uint8_t vInjSettleCyclesIdx =		vActivityTimeoutIdx + 1;
const uint8_t vMinGoodRPMcyclesIdx =		vInjSettleCyclesIdx + 1;
const uint8_t vMaxGoodInjCyclesIdx =		vMinGoodRPMcyclesIdx + 1;
const uint8_t vCyclesPerVolumeIdx =		vMaxGoodInjCyclesIdx + 1;
const uint8_t vTankCapacityIdx =		vCyclesPerVolumeIdx + 1;
#define nextAllowedValue vTankCapacityIdx + 1
#ifdef useDragRaceFunction
const uint8_t vDragInstantSpeedIdx =		nextAllowedValue;
const uint8_t vAccelHalfPeriodValueIdx =	vDragInstantSpeedIdx + 1;
const uint8_t vAccelFullPeriodValueIdx =	vAccelHalfPeriodValueIdx + 1;
const uint8_t vAccelDistanceValueIdx =		vAccelFullPeriodValueIdx + 1;
#define nextAllowedValue vAccelDistanceValueIdx + 1
#endif // useDragRaceFunction
#ifdef useJSONoutput
const uint8_t vBingoTankCapacityIdx =		nextAllowedValue;
#define nextAllowedValue vBingoTankCapacityIdx + 1
#endif // useJSONoutput
#ifdef useCoastDownCalculator
const uint8_t vCoastdownPeriodIdx =		nextAllowedValue;
const uint8_t vCoastdownMeasurementIdx =	vCoastdownPeriodIdx + 1;
#define nextAllowedValue vCoastdownMeasurementIdx + 3
#endif // useCoastDownCalculator
#ifdef useChryslerMAPCorrection
const uint8_t vMAPpressureIdx =			nextAllowedValue;
const uint8_t vBaroPressureIdx =		vMAPpressureIdx + 1;
const uint8_t vFuelPressureIdx =		vBaroPressureIdx + 1;
const uint8_t vInjPressureIdx =			vFuelPressureIdx + 1;
const uint8_t vInjCorrectionIdx =		vInjPressureIdx + 1;
#define nextAllowedValue vInjCorrectionIdx + 1
#endif // useChryslerMAPCorrection

const uint8_t maxVolatileVariableIdx =	nextAllowedValue;

volatile unsigned long volatileVariables[(unsigned int)(maxVolatileVariableIdx)];

uint8_t SWEET64processorFlags;
uint64_t s64reg[5];

const uint8_t s64reg0 = 0;	// general purpose
const uint8_t s64reg1 = 1;	// output value / general purpose
const uint8_t s64reg2 = 2;	// general purpose / temporary storage
const uint8_t s64reg3 = 3;	// used in multiply, divide operations
const uint8_t s64reg4 = 4;	// used in multiply, divide operations

#define nextAllowedValue 0
const uint8_t DNUISinstrDone =				nextAllowedValue;
const uint8_t DNUISinstrSetFlag =			DNUISinstrDone + 1;
const uint8_t DNUISinstrClearFlag =			DNUISinstrSetFlag + 1;
const uint8_t DNUISinstrSkipIfMetricMode =		DNUISinstrClearFlag + 1;
const uint8_t DNUISinstrTestRegister =			DNUISinstrSkipIfMetricMode + 1;
const uint8_t DNUISinstrSkipIfLTorE =			DNUISinstrTestRegister + 1;
const uint8_t DNUISinstrSkipIfIndexBelow =		DNUISinstrSkipIfLTorE + 1;
const uint8_t DNUISinstrSkip =				DNUISinstrSkipIfIndexBelow + 1;
const uint8_t DNUISinstrSkipIfFlagSet =			DNUISinstrSkip + 1;
const uint8_t DNUISinstrSkipIfFlagClear =		DNUISinstrSkipIfFlagSet + 1;
const uint8_t DNUISinstrLd =				DNUISinstrSkipIfFlagClear + 1;
const uint8_t DNUISinstrLdByteFromIndex =		DNUISinstrLd + 1;
const uint8_t DNUISinstrLdByte =			DNUISinstrLdByteFromIndex + 1;
const uint8_t DNUISinstrLdByteFromYindexed =		DNUISinstrLdByte + 1;
const uint8_t DNUISinstrLdTripVar =			DNUISinstrLdByteFromYindexed + 1;
const uint8_t DNUISinstrLdTripVarIndexed =		DNUISinstrLdTripVar + 1;
const uint8_t DNUISinstrLdConst =			DNUISinstrLdTripVarIndexed + 1;
const uint8_t DNUISinstrStByteToByteIndex =		DNUISinstrLdConst + 1;
const uint8_t DNUISinstrDoBCDadjust =			DNUISinstrStByteToByteIndex + 1;
const uint8_t DNUISinstrLdEEPROM =			DNUISinstrDoBCDadjust + 1;
const uint8_t DNUISinstrLdEEPROMindexed =		DNUISinstrLdEEPROM + 1;
const uint8_t DNUISinstrLdEEPROMindirect =		DNUISinstrLdEEPROMindexed + 1;
const uint8_t DNUISinstrStEEPROM =			DNUISinstrLdEEPROMindirect + 1;
const uint8_t DNUISinstrStEEPROMindirect =		DNUISinstrStEEPROM + 1;
const uint8_t DNUISinstrLdVolatile =			DNUISinstrStEEPROMindirect + 1;
const uint8_t DNUISinstrLdVolatileIndexed =		DNUISinstrLdVolatile + 1;
const uint8_t DNUISinstrStVolatile =			DNUISinstrLdVolatileIndexed + 1;
const uint8_t DNUISinstrLdIndex =			DNUISinstrStVolatile + 1;
const uint8_t DNUISinstrLdNumer =			DNUISinstrLdIndex + 1;
const uint8_t DNUISinstrLdDenom =			DNUISinstrLdNumer + 1;
const uint8_t DNUISinstrCall =				DNUISinstrLdDenom + 1;
const uint8_t DNUISinstrJump =				DNUISinstrCall + 1;
const uint8_t DNUISinstrSwap =				DNUISinstrJump + 1;
const uint8_t DNUISinstrSubYfromX =			DNUISinstrSwap + 1;
const uint8_t DNUISinstrAddYtoX =			DNUISinstrSubYfromX + 1;
const uint8_t DNUISinstrSubYfromXwithCarry =		DNUISinstrAddYtoX + 1;
const uint8_t DNUISinstrAddYtoXwithCarry =		DNUISinstrSubYfromXwithCarry + 1;
const uint8_t DNUISinstrMul2by1 =			DNUISinstrAddYtoXwithCarry + 1;
const uint8_t DNUISinstrDiv2by1 =			DNUISinstrMul2by1 + 1;
const uint8_t DNUISinstrShiftLeft =			DNUISinstrDiv2by1 + 1;
const uint8_t DNUISinstrRollLeft =			DNUISinstrShiftLeft + 1;
const uint8_t DNUISinstrShiftRight =			DNUISinstrRollLeft + 1;
const uint8_t DNUISinstrRollRight =			DNUISinstrShiftRight + 1;
const uint8_t DNUISinstrAddToIndex =			DNUISinstrRollRight + 1;
#define nextAllowedValue DNUISinstrAddToIndex + 1
#ifdef useWindowTripFilter
const uint8_t DNUISinstrStTripVar =			nextAllowedValue;
#define nextAllowedValue DNUISinstrStTripVar + 1
#endif // useWindowTripFilter
#ifdef useIsqrt
const uint8_t DNUISinstrIsqrt =				nextAllowedValue;
#define nextAllowedValue DNUISinstrIsqrt + 1
#endif // useIsqrt
#ifdef useAnalogRead
const uint8_t DNUISinstrLdVoltage =			nextAllowedValue;
#define nextAllowedValue DNUISinstrLdVoltage + 1
#endif // useAnalogRead

#define instrDone			DNUISinstrDone
#define instrSetFlag			(DNUISinstrSetFlag | 0x80)
#define instrClearFlag			(DNUISinstrClearFlag | 0x80)
#define instrSkipIfMetricMode		(DNUISinstrSkipIfMetricMode | 0x80)
#define instrSkipIfLTorE		(DNUISinstrSkipIfLTorE | 0x80 | 0x40)
#define instrSkipIfIndexBelow		(DNUISinstrSkipIfIndexBelow | 0x80)
#define instrSkip			(DNUISinstrSkip | 0x80)
#define instrTestRegister		(DNUISinstrTestRegister | 0x40)
#define instrSkipIfFlagSet		(DNUISinstrSkipIfFlagSet | 0x80)
#define instrSkipIfFlagClear		(DNUISinstrSkipIfFlagClear | 0x80)
#define instrLd				(DNUISinstrLd | 0x40)
#define instrLdByteFromIndex		(DNUISinstrLdByteFromIndex | 0x40)
#define instrLdByte			(DNUISinstrLdByte | 0x80 | 0x40)
#define instrLdByteFromYindexed		(DNUISinstrLdByteFromYindexed | 0x40)
#define instrLdTripVar			(DNUISinstrLdTripVar | 0x80 | 0x40)
#define instrLdTripVarIndexed		(DNUISinstrLdTripVarIndexed | 0x80 | 0x40)
#define instrLdConst			(DNUISinstrLdConst | 0x80 | 0x40)
#define instrStByteToByteIndex		(DNUISinstrStByteToByteIndex | 0x80 | 0x40)
#define instrDoBCDadjust		(DNUISinstrDoBCDadjust | 0x40)
#define instrLdEEPROM			(DNUISinstrLdEEPROM | 0x80 | 0x40)
#define instrLdEEPROMindexed		(DNUISinstrLdEEPROMindexed | 0x80 | 0x40)
#define instrLdEEPROMindirect		(DNUISinstrLdEEPROMindirect | 0x40)
#define instrStEEPROM			(DNUISinstrStEEPROM | 0x80 | 0x40)
#define instrStEEPROMindirect		(DNUISinstrStEEPROMindirect | 0x40)
#define instrLdVolatile			(DNUISinstrLdVolatile | 0x80 | 0x40)
#define instrLdVolatileIndexed		(DNUISinstrLdVolatileIndexed | 0x40)
#define instrStVolatile			(DNUISinstrStVolatile | 0x80 | 0x40)
#define instrLdIndex			(DNUISinstrLdIndex | 0x80)
#define instrLdNumer			(DNUISinstrLdNumer | 0x40)
#define instrLdDenom			(DNUISinstrLdDenom | 0x40)
#define instrCall			(DNUISinstrCall | 0x80)
#define instrJump			(DNUISinstrJump | 0x80)
#define instrSwap			(DNUISinstrSwap | 0x40)
#define instrSubYfromX			(DNUISinstrSubYfromX | 0x40)
#define instrAddYtoX			(DNUISinstrAddYtoX | 0x40)
#define instrSubYfromXwithCarry		(DNUISinstrSubYfromXwithCarry | 0x40)
#define instrAddYtoXwithCarry		(DNUISinstrAddYtoXwithCarry | 0x40)
#define instrMul2by1			DNUISinstrMul2by1
#define instrDiv2by1			DNUISinstrDiv2by1
#define instrShiftLeft			(DNUISinstrShiftLeft | 0x40)
#define instrRollLeft			(DNUISinstrRollLeft | 0x40)
#define instrShiftRight			(DNUISinstrShiftRight | 0x40)
#define instrRollRight			(DNUISinstrRollRight | 0x40)
#define instrAddToIndex			(DNUISinstrAddToIndex | 0x80)
#ifdef useAnalogRead
#define instrLdVoltage			(DNUISinstrLdVoltage | 0x40)
#endif // useAnalogRead
#ifdef useIsqrt
#define instrIsqrt			(DNUISinstrIsqrt | 0x40)
#endif // useIsqrt
#ifdef useWindowTripFilter
#define instrStTripVar			(DNUISinstrStTripVar | 0x80 | 0x40)
#endif // useWindowTripFilter

const uint8_t SWEET64carryFlag =	0b00000001;
const uint8_t SWEET64zeroFlag =		0b00000010;
const uint8_t SWEET64minusFlag =	0b00000100;
const uint8_t SWEET64divisionFlag =	0b00001000;
const uint8_t SWEET64traceFlag =	0b10000000;

const uint8_t prgmEngineSpeed[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjPulseIdx,			// load injector pulse count into register 2
	instrLdConst, 0x01, idxCyclesPerSecond,				// set up for conversion of denominator injector cycle count to time in seconds
	instrMul2by1,
	instrLdByte, 0x01, 60,						// load seconds per minute into register 1
	instrMul2by1,							// set up for conversion of denominator injector time in seconds to time in minutes
	instrLdConst, 0x01, idxDecimalPoint,				// perform output decimal formatting
	instrMul2by1,
	instrLdEEPROM, 0x01, pCrankRevPerInjIdx,			// multiply by the number of crank revolutions per injector fire event
	instrMul2by1,
	instrLdTripVarIndexed, 0x01, rvInjCycleIdx,			// divide by the injector pulse time
	instrDiv2by1,
	instrDone							// exit to caller
};

const uint8_t prgmMotionTime[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdConst, 0x01, idxCyclesPerSecond,
	instrDiv2by1,
	instrDone
};

const uint8_t prgmEngineRunTime[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjCycleIdx,
	instrLdConst, 0x01, idxCyclesPerSecond,
	instrDiv2by1,
	instrDone
};

const uint8_t prgmDistance[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSSpulseIdx,
	instrLdConst, 0x01, idxDecimalPoint,
	instrMul2by1,
	instrLdEEPROM, 0x01, pPulsesPerDistanceIdx,
	instrDiv2by1,
	instrDone
};

const uint8_t prgmSpeed[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSScycleIdx,			// load VSS cycle value into register 2
	instrTestRegister, 0x02,					// test VSS cycle value
	instrSkipIfFlagSet, SWEET64zeroFlag, 24,			// if zero, then speed is also zero

	instrLdEEPROM, 0x01, pPulsesPerDistanceIdx,			// set up to convert pulses per unit distance
	instrMul2by1,
	instrLd, 0x32,							// save denominator term for later

	instrLdTripVarIndexed, 0x02, rvVSSpulseIdx,			// load VSS pulse count
	instrLdConst, 0x01, idxDecimalPoint,				// adjust by decimal formatting term
	instrMul2by1,
	instrLdConst, 0x01, idxCyclesPerSecond,				// set up to convert VSS cycle value to time in seconds
	instrMul2by1,
	instrLdConst, 0x01, idxSecondsPerHour,				// set up to convert VSS time in seconds to time in hours
	instrMul2by1,
	instrLd, 0x13,							// load denominator term
	instrDiv2by1,							// divide to obtain unit distance per hour

	instrDone							// exit to caller
};

const uint8_t prgmFuelUsed[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,			// get amount of consumed fuel in cycles
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity				// go format the fuel quantity value
};

const uint8_t prgmRemainingFuel[] PROGMEM = {
	instrCall, tFindRemainingFuel,					// go get remaining fuel quantity in cycles
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity				// go format the remaining fuel quantity value
};

#ifdef useJSONoutput
const uint8_t prgmReserveFuelRemaining[] PROGMEM = {
	instrCall, tFindReserveFuel,					// go calculate how much reserve fuel remains
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity				// go format the reserve fuel value
};

const uint8_t prgmBingoFuelRemaining[] PROGMEM = {
	instrCall, tFindBingoFuel,					// go calculate bingo fuel quantity
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity				// go format the bingo fuel value
};

#endif // useJSONoutput
#ifdef useFuelCost
const uint8_t prgmFuelCost[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,			// get amount of consumed fuel in cycles
	instrLdEEPROM, 0x01, pCostPerQuantity,				// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity				// go format the fuel monetary value
};

const uint8_t prgmRemainingFuelCost[] PROGMEM = {
	instrCall, tFindRemainingFuel,					// go get remaining fuel quantity in cycles
	instrLdEEPROM, 0x01, pCostPerQuantity,				// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity				// go format the remaining fuel monetary value
};

const uint8_t prgmFuelCostPerDistance[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSSpulseIdx,			// fetch the accumulated number of VSS pulses counted
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrMul2by1,							// multiply the two numbers to get the denominator for fuel cost per distance
	instrLd, 0x32,							// save denominator term

	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,			// fetch the accumulated fuel injector open cycle measurement
	instrLdEEPROM, 0x01, pPulsesPerDistanceIdx,			// fetch the pulses per unit distance factor
	instrMul2by1,							// multiply the two numbers to get the numerator for fuel cost per distance
	instrLdEEPROM, 0x01, pCostPerQuantity,				// load fuel cost per unit quantity into register 1
	instrMul2by1,							// multiply the numerator by the formatting term

	instrLd, 0x13,							// move the denominator term into position
	instrDiv2by1,							// divide the numerator by the denominator
	instrDone
};

const uint8_t prgmDistancePerFuelCost[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,			// fetch the accumulated fuel injector open cycle measurement
	instrLdEEPROM, 0x01, pPulsesPerDistanceIdx,			// fetch the pulses per unit distance factor
	instrMul2by1,							// multiply the two numbers to get the denominator for distance per fuel cost
	instrLdEEPROM, 0x01, pCostPerQuantity,				// load cost per unit quantity of fuel into register 1
	instrMul2by1,							// multiply the denominator by the formatting term
	instrLd, 0x32,							// save the denominator term

	instrLdTripVarIndexed, 0x02, rvVSSpulseIdx,			// fetch the accumulated number of VSS pulses counted
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrMul2by1,							// multiply the two numbers to get the numerator for distance per fuel cost
	instrLdConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,							// multiply the numerator by the formatting term
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrMul2by1,							// multiply the numerator by the formatting term

	instrLd, 0x13,							// move the denominator term into position
	instrDiv2by1,							// divide the numerator by the denominator
	instrDone
};

#endif // useFuelCost
const uint8_t prgmFuelRate[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,
	instrTestRegister, 0x02,					// test injector open cycle value
	instrSkipIfFlagSet, SWEET64zeroFlag, 22,			// if zero, then fuel rate is also zero

	instrLdConst, 0x01, idxSecondsPerHour,
	instrMul2by1,
	instrLdConst, 0x01, idxDecimalPoint,
	instrMul2by1,
	instrLdTripVarIndexed, 0x01, rvInjCycleIdx,
	instrDiv2by1,
	instrLdConst, 0x01, idxCyclesPerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrMul2by1,
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrDiv2by1,							// multiply by quantity/cycle (divide by cycles/quantity)
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone
};

#ifdef useFuelCost
const uint8_t prgmFuelRateCost[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,
	instrTestRegister, 0x02,					// test injector open cycle value
	instrSkipIfFlagSet, SWEET64zeroFlag, 22,			// if zero, then fuel rate is also zero

	instrLdConst, 0x01, idxSecondsPerHour,
	instrMul2by1,
	instrLdEEPROM, 0x01, pCostPerQuantity,
	instrMul2by1,
	instrLdTripVarIndexed, 0x01, rvInjCycleIdx,
	instrDiv2by1,
	instrLdConst, 0x01, idxCyclesPerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrMul2by1,
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrDiv2by1,							// multiply by quantity/cycle (divide by cycles/quantity)
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone
};

#endif // useFuelCost
const uint8_t prgmFuelEcon[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSSpulseIdx,			// fetch the accumulated number of VSS pulses counted
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrMul2by1,							// multiply the two numbers to get the denominator for fuel economy
	instrLd, 0x32,							// save denominator for later

	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,			// fetch the accumulated fuel injector open cycle measurement
	instrLdEEPROM, 0x01, pPulsesPerDistanceIdx,			// fetch the pulses per unit distance factor
	instrMul2by1,							// multiply the two numbers to get the numerator for fuel economy

	instrSkipIfMetricMode, 7,					// if metric mode set, skip ahead
	instrSwap, 0x23,						// swap the numerator and denominator terms around
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrSkip, 3,							// go skip ahead

	instrLdConst, 0x01, idxMetricFE,				// load the output formatting decimal point constant, multiplied by 100 (for 100km/L)

	instrTestRegister, 0x02,					// test numerator term
	instrSkipIfFlagSet, SWEET64zeroFlag, 6,				// if zero, then fuel economy is also zero

	instrMul2by1,							// multiply the numerator by the formatting term
	instrLd, 0x13,							// move the denominator term into position
	instrDiv2by1,							// divide the numerator by the denominator
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone							// exit to caller

};

const uint8_t prgmDistanceToEmpty[] PROGMEM = {
	instrCall, tFindRemainingFuel,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateDistanceToEmpty				// go format it
};

#ifdef useJSONoutput
const uint8_t prgmFindRange[] PROGMEM = {
	instrLdVolatile, 0x02, vTankCapacityIdx,			// fetch calculated tank size in injector open cycles
	instrJump, tCalculateDistanceToEmpty				// go format it
};

const uint8_t prgmFindReserveRange[] PROGMEM = {
	instrLdVolatile, 0x02, vTankCapacityIdx,			// fetch calculated tank size in injector open cycles
	instrLdVolatile, 0x01, vBingoTankCapacityIdx,			// fetch bingo tank size in injector open cycles
	instrSubYfromX, 0x21,						// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateDistanceToEmpty				// go format it
};

const uint8_t prgmFindHalfReserveRange[] PROGMEM = {
	instrLdVolatile, 0x02, vTankCapacityIdx,			// fetch calculated tank size in injector open cycles
	instrLdVolatile, 0x01, vBingoTankCapacityIdx,			// fetch bingo tank size in injector open cycles
	instrSubYfromX, 0x21,						// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRight, 0x02,						// shift result right one bit
	instrJump, tCalculateDistanceToEmpty				// go format it
};

const uint8_t prgmFindBingoRange[] PROGMEM = {
	instrLdVolatile, 0x02, vBingoTankCapacityIdx,			// fetch bingo tank size in injector open cycles
	instrJump, tCalculateDistanceToEmpty				// go format it
};

const uint8_t prgmReserveDTE[] PROGMEM = {
	instrCall, tFindReserveFuel,					// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateDistanceToEmpty				// go format it
};

const uint8_t prgmBingoDTE[] PROGMEM = {
	instrCall, tFindBingoFuel,					// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateDistanceToEmpty				// go format it
};

#endif // useJSONoutput
const uint8_t prgmTimeToEmpty[] PROGMEM = {
	instrCall, tFindRemainingFuel,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateTimeToEmpty				// go format it
};

#ifdef useJSONoutput
const uint8_t prgmReserveTTE[] PROGMEM = {
	instrCall, tFindReserveFuel,					// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateTimeToEmpty				// go format it
};

const uint8_t prgmBingoTTE[] PROGMEM = {
	instrCall, tFindBingoFuel,					// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateTimeToEmpty				// go format it
};

#endif // useJSONoutput
const uint8_t prgmFindRemainingFuel[] PROGMEM = {
	instrLdTripVar, 0x01, rvInjOpenCycleIdx, tankIdx,		// fetch tank quantity in injector open cycles
	instrLdVolatile, 0x02, vTankCapacityIdx,			// fetch calculated tank size in injector open cycles
	instrSkipIfLTorE, 0x12, 4,					// if tank injector time in cycles <= calculated tank size in cycles, skip ahead

	instrLdByte, 0x02, 0,						// zero out result in register 2
	instrDone,							// exit to caller

	instrSubYfromX, 0x21,						// subtract tank injector time in cycles from calculated tank size in cycles to get remaining fuel in cycles
	instrDone							// exit to caller
};

#ifdef useJSONoutput
const uint8_t prgmFindBingoFuel[] PROGMEM = {
	instrCall, tFindRemainingFuel,					// get calculated remaining fuel in injector open cycles
	instrTestRegister, 0x02,					// test calculated remaining fuel left
	instrSkipIfFlagSet, SWEET64zeroFlag, 7,				// if zero, then bingo fuel is also zero

	instrLdVolatile, 0x01, vBingoTankCapacityIdx,			// fetch bingo tank size in injector open cycles
	instrSkipIfLTorE, 0x21, 2,					// if tank quantity in injector open cycles <= calculated bingo size in injector open cycles, exit to caller

	instrLd, 0x21,							// shift calculated bingo size in injector open cycles to register 2

	instrDone							// exit to caller
};

const uint8_t prgmFindReserveFuel[] PROGMEM = {
	instrCall, tFindRemainingFuel,					// get calculated remaining fuel in cycles
	instrTestRegister, 0x02,					// test calculated remaining fuel left
	instrSkipIfFlagSet, SWEET64zeroFlag, 8,				// if zero, then reserve fuel is also zero

	instrLdVolatile, 0x01, vBingoTankCapacityIdx,			// fetch bingo tank size in injector open cycles
	instrSkipIfLTorE, 0x21, 3,					// if tank injector time in cycles <= calculated bingo size in cycles, exit to caller

	instrSubYfromX, 0x21,						// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrDone,							// exit to caller

	instrLdByte, 0x02, 0,						// zero out result in register 2
	instrDone							// exit to caller
};

#endif // useJSONoutput
const uint8_t prgmInjectorOpenTime[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjOpenCycleIdx,
	instrLdConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,							// multiply the numerator by the formatting term
	instrJump, tConvertToMicroSeconds
};

const uint8_t prgmInjectorTotalTime[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjCycleIdx,
	instrLdConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,							// multiply the numerator by the formatting term
	instrJump, tConvertToMicroSeconds
};

const uint8_t prgmVSStotalTime[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,							// multiply the numerator by the formatting term
	instrJump, tConvertToMicroSeconds
};

const uint8_t prgmVSSpulseCount[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSSpulseIdx,
	instrLdConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,							// multiply the numerator by the formatting term
	instrDone
};

const uint8_t prgmInjectorPulseCount[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvInjPulseIdx,
	instrLdConst, 0x01, idxDecimalPoint,				// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2by1,							// multiply the numerator by the formatting term
	instrDone
};

#ifdef useAnalogRead
const uint8_t prgmVoltage[] PROGMEM = {
	instrSkipIfIndexBelow, 4, dfMaxValAnalogCount,			// skip if trip index is pointing to a valid analog channel

	instrLdByte, 0x02, 0,						// zero out result in register 2
	instrDone,							// exit to caller

	instrLdVoltage, 0x01,						// load analog channel ADC step value
	instrLdConst, 0x02, idxDenomVoltage,				// load denominator to convert from voltage to ADC steps
	instrMul2by1,							// perform multiply (we're converting from ADC steps to voltage)
	instrLdConst, 0x01, idxNumerVoltage,				// load numerator to convert from voltage to ADC steps
	instrDiv2by1,							// go finish conversion
	instrDone							// exit to caller
};

#endif // useAnalogRead
#ifdef useChryslerMAPCorrection
const uint8_t prgmPressure[] PROGMEM = {
	instrSkipIfIndexBelow, 4, vInjCorrectionIdx - vMAPpressureIdx,	// skip if trip index is pointing to a valid pressure element in the pressures array

	instrLdByte, 0x02, 0,						// zero out result in register 2
	instrDone,							// exit to caller

	instrAddToIndex, vMAPpressureIdx,
	instrLdVolatileIndexed, 0x02,					// load pre-formatted pressure element
	instrDone							// exit to caller
};

const uint8_t prgmCorrF[] PROGMEM = {
	instrLdConst, 0x02, idxDecimalPoint,				// load register 2 with the decimal point constant used for output formatting
	instrLdVolatile, 0x01, vInjCorrectionIdx,			// load the raw correction factor into register 1
	instrMul2by1,							// multiply by the decimal point constant used for output formatting
	instrLdConst, 0x01, idxCorrFactor,				// load the base correction factor into register 1
	instrDiv2by1,							// go finish conversion
	instrDone							// exit to caller
};

#endif // useChryslerMAPCorrection
const uint8_t prgmTimeToEmptyCalc[] PROGMEM = {
	instrTestRegister, 0x02,					// test inputted fuel quantity
	instrSkipIfFlagSet, SWEET64zeroFlag, 20,			// if inputted fuel quantity is zero, exit to caller

	instrLdConst, 0x01, idxMicroSecondsPerSecond,			// shift number upward to retain numeric precision
	instrMul2by1,
	instrLdTripVarIndexed, 0x01, rvInjOpenCycleIdx,			// divide by open injector time in cycles
	instrDiv2by1,
	instrLdTripVarIndexed, 0x01, rvInjCycleIdx,			// multiply by injector open count
	instrMul2by1,
	instrLdConst, 0x01, idxCyclesPerSecond,				// convert output term from cycles remaining to seconds remaining
	instrDiv2by1,
	instrLdConst, 0x01, idxMicroSecondsPerSecond,			// shift number downward - high numeric precision no longer needed
	instrDiv2by1,							// go finish conversion

	instrDone							// exit to caller
};

const uint8_t prgmDistanceToEmptyCalc[] PROGMEM = {
	instrTestRegister, 0x02,					// test inputted fuel quantity
	instrSkipIfFlagSet, SWEET64zeroFlag, 26,			// if inputted fuel quantity is zero, exit to caller

	instrLdTripVarIndexed, 0x01, rvVSSpulseIdx,			// fetch the accumulated distance travelled pulses
	instrMul2by1,							// multiply input fuel quantity by accumulated distance travelled pulses to get the numerator for distance to empty
	instrLd, 0x32,							// move numerator to register 3 for now

	instrLdTripVarIndexed, 0x01, rvInjOpenCycleIdx,			// fetch the accumulated fuel injector open cycles
	instrLdEEPROM, 0x02, pPulsesPerDistanceIdx,			// fetch the pulses per unit distance factor
	instrMul2by1,							// multiply accumulated fuel injector open cycles by pulses per unit distance factor
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrDiv2by1,							// divide to get denominator / decimal factor
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLd, 0x12,							// move modified denominator to register 1 for now
	instrLd, 0x23,							// fetch numerator from register 3

	instrDiv2by1,							// divide to get DTE = (numerator) / (denominator / decimal factor ) = (numerator * decimal factor) / denominator
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone							// exit to caller
};

const uint8_t prgmFuelQuantityCalc[] PROGMEM = {
	instrTestRegister, 0x02,					// test inputted fuel quantity
	instrSkipIfFlagSet, SWEET64zeroFlag, 7,				// if inputted fuel quantity is zero, exit to caller

	instrMul2by1,							// multiply the fuel used value by the formatting term
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrDiv2by1,							// convert fuel value in cycles to fuel value per unit quantity
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone							// exit to caller
};

const uint8_t prgmConvertToMicroSeconds[] PROGMEM = {
	instrLdConst, 0x01, idxMicroSecondsPerSecond,			// load conversion factor for microseconds per second into register 1
	instrMul2by1,							// multiply by system cycles value in register 2
	instrLdConst, 0x01, idxCyclesPerSecond,				// load conversion factor for cycles to seconds into register 1
	instrDiv2by1,							// convert register 2 from system cycles to microseconds
	instrDone							// exit to caller
};

const uint8_t prgmRoundOffNumber[] PROGMEM = {
	instrLdByte, 0x01, 1,						// load a 1 into register 1
	instrAddYtoX, 0x12,						// add register 2 results to register 1
	instrSkipIfFlagSet, SWEET64zeroFlag, 26,			// if register 1 is zero, register 2 has overflow value

	instrSkipIfIndexBelow, 8, 1,					// skip if 0 right-hand digits were specified
	instrSkipIfIndexBelow, 10, 2,					// skip if 1 right-hand digit was specified
	instrSkipIfIndexBelow, 12, 3,					// skip if 2 right-hand digits were specified
	instrSkip, 15,							// skip if 3 or more right-hand digits were specified

	instrLdConst, 0x01, idxNumber500,				// perform round to nearest whole integer
	instrSkip, 8,							// skip to add
	instrLdByte, 0x01, 50,						// perform round to nearest 1/10th
	instrSkip, 3,							// skip to add
	instrLdByte, 0x01, 5,						// perform round to nearest 1/100th

	instrAddYtoX, 0x21,						// perform round to nearest number

	instrJump, tConvertToFiveBCDbyteNumber				// go call prgmFormatToNumber to perform actual formatting
};

const uint8_t prgmFormatToNumber[] PROGMEM = {
	instrLdByte, 0x01, 1,						// load a 1 into register 1
	instrAddYtoX, 0x12,						// add register 2 results to register 1
	instrSkipIfFlagSet, SWEET64zeroFlag, 3,				// if register 1 is zero, register 2 has overflow value

	instrSkipIfIndexBelow, 4, 4,					// skip if valid number of decimal points were requested
	instrLdIndex, 6,						// load index with offset into byte 6
	instrSkip, 12,							// skip to exit with overflow error

	instrLdConst, 0x01, idxBCDdivisor,				// load 10^8 into register 1
	instrDiv2by1,							// perform division - quotient remains in register 2, and remainder goes into register 1

	instrLdIndex, 6,						// load index with offset into byte 6
	instrLdByte, 0x03, 99,						// test if quotient is greater than 99 (remainder will be less than 10^8)
	instrSkipIfLTorE, 0x23, 4,					// if quotient is 99 or less, continue with processing

	instrStByteToByteIndex, 0x03, 255,				// signal overflow by storing total length of 255 into byte 6 of register 3
	instrDone,

	instrStByteToByteIndex, 0x03, 5,				// store total BCD length into byte 6 of register 3
	instrLdIndex, 7,
	instrStByteToByteIndex, 0x03, 32,				// store leading zero character into byte 7 of register 3
	instrLdIndex, 1,
	instrStByteToByteIndex, 0x03, 100,				// store BCD divisor into byte 3 of register 3
	instrLdIndex, 2,
	instrStByteToByteIndex, 0x03, 100,				// store BCD divisor into byte 2 of register 3
	instrLdIndex, 3,
	instrStByteToByteIndex, 0x03, 100,				// store BCD divisor into byte 1 of register 3
	instrLdIndex, 4,
	instrStByteToByteIndex, 0x03, 100,				// store BCD divisor into byte 0 of register 3

	instrDoBCDadjust, 0x13,						// go turn register 1 into a BCD string and store it in register 3

	instrDone							// exit to caller
};

const uint8_t prgmFormatToTime[] PROGMEM = {
	instrLd, 0x12,							// move time in seconds into register 1

	instrLdIndex, 6,						// load index with offset into byte 6
	instrStByteToByteIndex, 0x03, 3,				// store total BCD length into byte 6 of register 3
	instrLdIndex, 7,
	instrStByteToByteIndex, 0x03, 48,				// store leading zero character into byte 7 of register 3
	instrLdIndex, 2,
	instrStByteToByteIndex, 0x03, 60,				// store hours divisor into byte 2 of register 3
	instrLdIndex, 1,
	instrStByteToByteIndex, 0x03, 60,				// store minutes divisor into byte 1 of register 3
	instrLdIndex, 0,
	instrStByteToByteIndex, 0x03, 24,				// store seconds divisor into byte 0 of register 3

	instrDoBCDadjust, 0x13,						// go turn register 1 into a BCD string and store it in register 3

	instrDone							// exit to caller
};

const uint8_t prgmMultiplyBy100[] PROGMEM = {
	instrLdByte, 0x01, 100,						// load d = 100 into register 1
	instrMul2by1,							// multiply result by 100

	instrLdByteFromIndex, 0x03,					// load register 3 with whatever's in trip variable index
	instrAddYtoX, 0x32,						// add registers 2 and 3, store the result in 3

	instrDone							// exit to caller
};

const uint8_t prgmMultiplyBy10[] PROGMEM = {
	instrLdByte, 0x01, 10,						// load d = 10 into register 1
	instrMul2by1,							// multiply result by 10

	instrLdByteFromIndex, 0x03,					// load register 3 with whatever's in trip variable index
	instrAddYtoX, 0x32,						// add registers 2 and 3, store the result in 3

	instrDone							// exit to caller
};

const uint8_t prgmFindTimerTicks[] PROGMEM = {
	instrLdConst, 0x01, idxCyclesPerSecond,				// load register 1 with cycles per second value
	instrMul2by1,							// do multiply to get number of processor cycles for timeout
	instrLdByte, 0x01, 128,
	instrAddYtoXwithCarry, 0x21,					// perform round to nearest multiple of 256
	instrLdIndex, 0,						// prepare for divide by 256, as timer0 overflow will handle timeout counter

	instrShiftRight, 0x02,						// shift result right one bit
	instrAddToIndex, 1,						// update counter
	instrSkipIfIndexBelow, 249, 8,					// if counter is less than 8, go back

	instrDone							// exit to caller
};

#ifdef useWindowTripFilter
const uint8_t prgmUpdateWindowTripFilter[] PROGMEM = {
	instrLdByte, 0x02, 0,						// load a zero into register 2
	instrStTripVar, 0x02, rvInjOpenCycleIdx, instantIdx,		// store fuel quantity consumed in cycles in instant trip variable
	instrStTripVar, 0x02, rvInjCycleIdx, instantIdx,		// store fuel pulse period in cycles in instant trip variable
	instrStTripVar, 0x02, rvInjPulseIdx, instantIdx,		// store fuel pulse count in instant trip variable
	instrStTripVar, 0x02, rvVSSpulseIdx, instantIdx,		// store VSS pulse count in instant trip variable
	instrStTripVar, 0x02, rvVSScycleIdx, instantIdx,		// store VSS pulse period in cycles in instant trip variable

	instrLdIndex, windowTripFilterIdx,

	instrLdTripVarIndexed, 0x01, rvInjOpenCycleIdx,			// load fuel quantity consumed in cycles from current window trip variable element
	instrLdTripVar, 0x02, rvInjOpenCycleIdx, instantIdx,		// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,						// add them together
	instrStTripVar, 0x02, rvInjOpenCycleIdx, instantIdx,		// store fuel quantity consumed in cycles in instant trip variable

	instrLdTripVarIndexed, 0x01, rvInjCycleIdx,			// load fuel quantity consumed in cycles from current window trip variable element
	instrLdTripVar, 0x02, rvInjCycleIdx, instantIdx,		// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,						// add them together
	instrStTripVar, 0x02, rvInjCycleIdx, instantIdx,		// store fuel quantity consumed in cycles in instant trip variable

	instrLdTripVarIndexed, 0x01, rvInjPulseIdx,			// load fuel quantity consumed in cycles from current window trip variable element
	instrLdTripVar, 0x02, rvInjPulseIdx, instantIdx,		// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,						// add them together
	instrStTripVar, 0x02, rvInjPulseIdx, instantIdx,		// store fuel quantity consumed in cycles in instant trip variable

	instrLdTripVarIndexed, 0x01, rvVSSpulseIdx,			// load fuel quantity consumed in cycles from current window trip variable element
	instrLdTripVar, 0x02, rvVSSpulseIdx, instantIdx,		// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,						// add them together
	instrStTripVar, 0x02, rvVSSpulseIdx, instantIdx,		// store fuel quantity consumed in cycles in instant trip variable

	instrLdTripVarIndexed, 0x01, rvVSScycleIdx,			// load fuel quantity consumed in cycles from current window trip variable element
	instrLdTripVar, 0x02, rvVSScycleIdx, instantIdx,		// load fuel quantity consumed in cycles from instant trip variable
	instrAddYtoX, 0x21,						// add them together
	instrStTripVar, 0x02, rvVSScycleIdx, instantIdx,		// store fuel quantity consumed in cycles in instant trip variable

	instrAddToIndex, 1,
	instrSkipIfIndexBelow, 186, windowTripFilterIdx + windowTripFilterSize,

	instrDone							// exit to caller
};

#endif // useWindowTripFilter
#ifdef useExpansionPins
const uint8_t prgmFindExpansionPin1Value[] PROGMEM = {
	instrLdEEPROM, 0x02, pExpansionPin1Mode,			// load register 2 with expansion output pin 1 mode
	instrJump, tFindExpPinSetting					// go find output I/O value
};

const uint8_t prgmFindExpansionPin2Value[] PROGMEM = {
	instrLdEEPROM, 0x02, pExpansionPin2Mode,			// load register 2 with expansion output pin 2 mode
	instrJump, tFindExpPinSetting					// go find output I/O value
};

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

const uint8_t prgmFindExpansionPinSetting[] PROGMEM = {
	instrSkipIfIndexBelow, 6, 1,					// skip to return if index register is zero
	instrSkipIfIndexBelow, 10, 3,					// go handle instant <> current comparison
	instrSkipIfIndexBelow, 42, 5,					// go handle instant <> tank comparison
	instrSkipIfIndexBelow, 72, 7,					// go handle fuel tank analog output

	instrLdByte, 0x02, 0,						// zero out result
	instrDone,							// return to caller

	instrLdTripVar, 0x01, rvInjOpenCycleIdx, instantIdx,		// fetch instant total fuel used
	instrLdTripVar, 0x02, rvVSSpulseIdx, currentIdx,		// fetch current distance travelled
	instrMul2by1,							// calculate numerator for comparison
	instrLd, 0x032,							// save numerator for later
	instrLdTripVar, 0x01, rvInjOpenCycleIdx, currentIdx,		// fetch current total fuel used
	instrLdTripVar, 0x02, rvVSSpulseIdx, instantIdx,		// fetch instant distance travelled
	instrMul2by1,							// calculate denominator for comparison
	instrSkipIfIndexBelow, 5, 2,					// skip ahead if index register is 1
	instrSkipIfLTorE, 0x32, 226,					// if denominator is less than or equal to numerator, go zero out and return
	instrSkip, 3,							// go max out result
	instrSkipIfLTorE, 0x23, 221,					// if numerator is less than or equal to denominator, go zero out and return
	instrLdByte, 0x02, 255,						// max out result
	instrDone,							// return to caller

	instrLdTripVar, 0x01, rvInjOpenCycleIdx, instantIdx,		// fetch instant total fuel used
	instrLdTripVar, 0x02, rvVSSpulseIdx, tankIdx,			// fetch tank distance travelled
	instrMul2by1,							// calculate numerator for comparison
	instrLd, 0x032,							// save numerator for later
	instrLdTripVar, 0x01, rvInjOpenCycleIdx, tankIdx,		// fetch tank total fuel used
	instrLdTripVar, 0x02, rvVSSpulseIdx, instantIdx,		// fetch instant distance travelled
	instrMul2by1,							// calculate denominator for comparison
	instrSkipIfIndexBelow, 5, 4,					// skip ahead if index register is 3
	instrSkipIfLTorE, 0x32, 191,					// if denominator is less than or equal to numerator, go zero out and return
	instrSkip, 224,							// go max out result
	instrSkipIfLTorE, 0x23, 186,					// if numerator is less than or equal to denominator, go zero out and return
	instrSkip, 219,							// go max out result

//	this part still needs written
	instrLdByte, 0x02, 0,						// zero out result
	instrDone							// exit to caller
};

#endif // useExpansionPins
#ifdef useDragRaceFunction
const uint8_t prgmDragSpeed[] PROGMEM = {
	instrLdVolatile, 0x02, vDragInstantSpeedIdx,			// load instantaneous drag speed measurement
	instrTestRegister, 0x02,					// test speed measurement
	instrSkipIfFlagSet, SWEET64zeroFlag, 20,			// if speed measurement is zero, exit to caller

	instrLdEEPROM, 0x01, pPulsesPerDistanceIdx,			// set up to convert pulses per unit distance
	instrMul2by1,
	instrLd, 0x32,							// save denominator term for later

	instrLdConst, 0x01, idxDecimalPoint,				// load decimal formatting term
	instrLdConst, 0x02, idxCyclesPerSecond,				// set up to convert VSS cycle value to time in seconds
	instrMul2by1,
	instrLdConst, 0x01, idxSecondsPerHour,				// set up to convert VSS time in seconds to time in hours
	instrMul2by1,
	instrLd, 0x13,							// load denominator term
	instrDiv2by1,							// divide to obtain unit distance per hour

	instrDone							// exit to caller
};

const uint8_t prgmEstimateEnginePower[] PROGMEM = {
	instrCall, tDragSpeed,						// calculate vehicle speed (this will be the maximum vehicle speed obtained during the acceleration test)

	instrSkipIfMetricMode, 2,					// if speed is in km/hr, skip ahead to convert to MPH
	instrSkip, 10,							// skip to power calculation

	instrLdConst, 0x01, idxDenomDistance,				// multiply by numerator to convert km/hr to MPH
	instrMul2by1,
	instrLdConst, 0x01, idxNumerDistance,				// divide by denominator to convert km/hr to MPH
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLd, 0x12,							// load register 1 with calculated vehicle speed
	instrMul2by1,							// perform cube function on maximum accel test vehicle speed value to obtain initial denominator term
	instrMul2by1,

	instrLdConst, 0x01, idxPowerFactor,				// divide by power estimation term to reduce the number of bits in the estimate
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdEEPROM, 0x01, pVehicleMassIdx,				// fetch vehicle weight
	instrMul2by1,							// multiply estimate by vehicle weight
	instrLdConst, 0x01, idxPowerFactor,				// divide by power estimation term to reduce the number of bits in the estimate
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrSkipIfMetricMode, 2,					// if vehicle weight is in kg (yes, that's actually a mass term in SI), skip ahead to convert to lbf
	instrSkip, 10,							// skip to power calculation

	instrLdConst, 0x01, idxDenomMass,				// multiply by numerator to convert kg to lbf
	instrMul2by1,
	instrLdConst, 0x01, idxNumerMass,				// divide by denominator to convert kg to lbf
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdConst, 0x01, idxPowerFactor,				// divide by power estimation term to reduce the number of bits in the estimate
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrDone							// exit to caller
};

const uint8_t prgmAccelTestTime[] PROGMEM = {
	instrLdTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrMul2by1,
	instrLdConst, 0x01, idxCyclesPerSecond,
	instrDiv2by1,
	instrDone
};

#endif // useDragRaceFunction
// Button Press variable section

typedef struct
{

	uint8_t buttonCode;
	void (* buttonCommand)(void);

} buttonList;

#ifdef useButtonCrossConfig
static const buttonList bpListMain[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnLongPress1,	doShowCPU}
#endif // useCPUreading
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif
#ifdef useScreenEditor
	,{btnShortPress2,	doGoScreenEdit}
#endif // useScreenEditor
#ifdef useDebugReadings
	,{btnLongPress2,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

static const buttonList bpListMenu[] PROGMEM = {
	{btnShortPressC,	menu::select}
#ifdef useCPUreading
	,{btnLongPress1,	doShowCPU}
#endif // useCPUreading
	,{buttonsUp,		noSupport}
};

static const buttonList bpListSetting[] PROGMEM = {
	{btnShortPressC,	settings::editParameter}
#ifdef useCPUreading
	,{btnLongPress1,	doShowCPU}
#endif // useCPUreading
	,{buttonsUp, 		noSupport}
};

static const buttonList bpListParam[] PROGMEM = {
	{btnShortPressRC,	parameter::findRight}
	,{btnLongPress1,	parameter::cancelEdit}
	,{btnShortPressCL,	parameter::findLeft}
	,{btnShortPress1,	parameter::changeDigit}
	,{btnLongPressRC,	parameter::readMinValue}
	,{btnLongPress2,	parameter::revert}
	,{btnLongPressCL,	parameter::readMaxValue}
	,{btnLongPressC,	parameter::saveEdit}
	,{buttonsUp,		noSupport}
};

#ifdef useCPUreading
static const buttonList bpListCPUmonitor[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
#ifdef useBenchMark
	,{btnLongPress1,	doBenchMark}
#endif // useBenchMark
	,{buttonsUp,		noSupport}
};

#endif // useCPUreading
#ifdef useBigNumberDisplay
static const buttonList bpListBigNum[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnLongPress1,	doShowCPU}
#endif // useCPUreading
#ifdef useDebugReadings
	,{btnLongPress2,	goStepDebug}
#endif // useDebugReadings
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBigNumberDisplay
#ifdef useBarFuelEconVsTime
static const buttonList bpListBFET[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnLongPress1,	doShowCPU}
#endif // useCPUreading
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
#ifdef useDebugReadings
	,{btnLongPress2,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const buttonList bpListBFES[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnLongPress1,	doShowCPU}
#endif // useCPUreading
	,{btnLongPress1C,	doResetBarFEvS}
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
#ifdef useDebugReadings
	,{btnLongPress2,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsSpeed
#ifdef useSoftwareClock
static const buttonList bpListSoftwareClockShow[] PROGMEM = {
	{btnShortPress1,	doGoEditSoftwareClock}
	,{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

static const buttonList bpListSoftwareClockEdit[] PROGMEM = {
	{btnShortPress2,	doEditSoftwareClockCancel}
	,{btnShortPress1,	doEditSoftwareClockChangeDigit}
	,{btnLongPress2,	doEditSoftwareClockCancel}
	,{btnLongPressC,	doEditSoftwareClockSave}
	,{buttonsUp,		noSupport}
};

#endif // useSoftwareClock
#ifdef useSavedTrips
static const buttonList bpListTripSave[] PROGMEM = {
	{btnShortPress1,	doTripSelect}
	,{btnLongPressRC,	doTripResetCurrent}
	,{btnLongPressCL,	doTripResetTank}
	,{btnLongPress1,	menu::select}
	,{btnShortPress2,	doTripLongSelect}
	,{buttonsUp,		noSupport}
};

#ifdef showSavedTrips
static const buttonList bpListTripView[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
static const buttonList bpListPartialRefuel[] PROGMEM = {
	{btnShortPress1,	doPartialFuelSelect}
	,{btnLongPressRC,	doTripResetCurrent}
	,{btnLongPressCL,	doTripResetTank}
	,{btnLongPress1,	doPartialFuelLongSelect}
	,{buttonsUp,		noSupport}
};

#endif // usePartialRefuel
#ifdef useScreenEditor
static const buttonList bpListScreenEdit[] PROGMEM = {
	{btnShortPress1,	doScreenEditBump}
	,{btnShortPress2,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPress1,	doSaveScreen}
	,{buttonsUp,		noSupport}
};

#endif // useScreenEditor
#ifdef useEEPROMviewer
static const buttonList bpListEEPROMview[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // useEEPROMviewer
#ifdef useDragRaceFunction
static const buttonList bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
#ifdef useDebugReadings
	,{btnLongPress2,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonList bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
#ifdef useDebugReadings
	,{btnLongPress2,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useCoastDownCalculator
#else // useButtonCrossConfig
static const buttonList bpListMain[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnShortPressRL,	doShowCPU}
#endif // useCPUreading
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif
#ifdef useScreenEditor
	,{btnLongPressRL,	doGoScreenEdit}
#endif // useScreenEditor
#ifdef useDebugReadings
	,{btnLongPressRCL,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

static const buttonList bpListMenu[] PROGMEM = {
	{btnShortPressC,	menu::select}
#ifdef useCPUreading
	,{btnShortPressRL,	doShowCPU}
#endif // useCPUreading
	,{buttonsUp,		noSupport}
};

static const buttonList bpListSetting[] PROGMEM = {
	{btnShortPressC,	settings::editParameter}
#ifdef useCPUreading
	,{btnShortPressRL, 	doShowCPU}
#endif // useCPUreading
	,{buttonsUp, 		noSupport}
};

static const buttonList bpListParam[] PROGMEM = {
	{btnShortPressRC,	parameter::findRight}
	,{btnShortPressRL,	parameter::cancelEdit}
	,{btnShortPressCL,	parameter::findLeft}
	,{btnShortPressC,	parameter::changeDigit}
	,{btnLongPressRC,	parameter::readMinValue}
	,{btnLongPressRL,	parameter::revert}
	,{btnLongPressCL,	parameter::readMaxValue}
	,{btnLongPressC,	parameter::saveEdit}
	,{buttonsUp,		noSupport}
};

#ifdef useCPUreading
static const buttonList bpListCPUmonitor[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
#ifdef useBenchMark
	,{btnLongPressRCL,	doBenchMark}
#endif // useBenchMark
	,{buttonsUp,		noSupport}
};

#endif // useCPUreading
#ifdef useBigNumberDisplay
static const buttonList bpListBigNum[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnShortPressRL,	doShowCPU}
#endif // useCPUreading
#ifdef useDebugReadings
	,{btnLongPressRCL,	goStepDebug}
#endif // useDebugReadings
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBigNumberDisplay
#ifdef useBarFuelEconVsTime
static const buttonList bpListBFET[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnShortPressRL,	doShowCPU}
#endif // useCPUreading
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
#ifdef useDebugReadings
	,{btnLongPressRCL,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const buttonList bpListBFES[] PROGMEM = {
	{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useCPUreading
	,{btnShortPressRL,	doShowCPU}
#endif // useCPUreading
	,{btnLongPressRCL,	doResetBarFEvS}
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

#endif // useBarFuelEconVsSpeed
#ifdef useSoftwareClock
static const buttonList bpListSoftwareClockShow[] PROGMEM = {
	{btnShortPressRL,	doGoEditSoftwareClock}
	,{btnLongPressRC,	doTripResetCurrent}
#ifdef usePartialRefuel
	,{btnLongPressCL,	doGoPartialRefuel}
#else // usePartialRefuel
	,{btnLongPressCL,	doTripResetTank}
#endif // usePartialRefuel
	,{btnLongPressR,	doLongGoRight}
	,{btnLongPressL,	doLongGoLeft}
#ifdef useSavedTrips
	,{btnShortPressRC,	doGoTripSaveCurrent}
	,{btnShortPressCL,	doGoTripSaveTank}
#endif // useSavedTrips
	,{buttonsUp,		noSupport}
};

static const buttonList bpListSoftwareClockEdit[] PROGMEM = {
	{btnShortPressRL,	doEditSoftwareClockCancel}
	,{btnShortPressC,	doEditSoftwareClockChangeDigit}
	,{btnLongPressRL,	doEditSoftwareClockCancel}
	,{btnLongPressC,	doEditSoftwareClockSave}
	,{buttonsUp,		noSupport}
};

#endif // useSoftwareClock
#ifdef useSavedTrips
static const buttonList bpListTripSave[] PROGMEM = {
	{btnShortPressC,	doTripSelect}
	,{btnLongPressRC,	doTripResetCurrent}
	,{btnLongPressCL,	doTripResetTank}
	,{btnLongPressRL,	menu::select}
	,{btnLongPressC,	doTripLongSelect}
	,{buttonsUp,		noSupport}
};

#ifdef showSavedTrips
static const buttonList bpListTripView[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
static const buttonList bpListPartialRefuel[] PROGMEM = {
	{btnShortPressC,	doPartialFuelSelect}
	,{btnLongPressRC,	doTripResetCurrent}
	,{btnLongPressCL,	doTripResetTank}
	,{btnLongPressRL,	menu::select}
	,{btnLongPressC,	doPartialFuelLongSelect}
	,{buttonsUp,		noSupport}
};

#endif // usePartialRefuel
#ifdef useScreenEditor
static const buttonList bpListScreenEdit[] PROGMEM = {
	{btnShortPressC,	doScreenEditBump}
	,{btnLongPressRL,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressC,	doSaveScreen}
	,{buttonsUp,		noSupport}
};

#endif // useScreenEditor
#ifdef useEEPROMviewer
static const buttonList bpListEEPROMview[] PROGMEM = {
	{buttonsUp,		noSupport}
};

#endif // useEEPROMviewer
#ifdef useDragRaceFunction
static const buttonList bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
#ifdef useDebugReadings
	,{btnLongPressRCL,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonList bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
#ifdef useDebugReadings
	,{btnLongPressRCL,	goStepDebug}
#endif // useDebugReadings
	,{buttonsUp,		noSupport}
};

#endif // useCoastDownCalculator
#endif //useButtonCrossConfig
// Display screen variable section

const uint8_t mainScreenSize = 1 // this variable is used to figure out how many menu levels the main screen has
#ifdef useCPUreading
	+ 1
#endif // useCPUreading
#ifdef useBarFuelEconVsTime
	+ 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	+ 1
#endif // useBarFuelEconVsSpeed
#ifdef useBigFE
	+ 1
#endif // useBigFE
#ifdef useBigDTE
	+ 1
#endif // useBigDTE
#ifdef useBigTTE
	+ 1
#endif // useBigTTE
#ifdef useSoftwareClock
	+ 1
#endif // useSoftwareClock
;

const uint8_t totalMenuSize = mainScreenSize + 3 // this variable is used to figure out how many total menu levels exist
#ifdef useSoftwareClock
	+ 1
#endif // useSoftwareClock
#ifdef useSavedTrips
	+ 2
#ifdef showSavedTrips
	+ 2
#ifdef trackIdleEOCdata
	+ 2
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
	+ 1
#endif // usePartialRefuel
#ifdef useScreenEditor
	+ 1
#endif // useScreenEditor
#ifdef useEEPROMviewer
	+ 1
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
	+ 1
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	+ 1
#endif // useCoastDownCalculator
;

#define nextAllowedValue 0
const uint8_t mainScreenIdx =			nextAllowedValue;
#define nextAllowedValue mainScreenIdx + 1
#ifdef useBigFE
const uint8_t bigFEscreenIdx =			nextAllowedValue;
#define nextAllowedValue bigFEscreenIdx + 1
#endif // useBigFE
#ifdef useBarFuelEconVsTime
const uint8_t barFEvTscreenIdx =		nextAllowedValue;
#define nextAllowedValue barFEvTscreenIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t barFEvSscreenIdx =		nextAllowedValue;
#define nextAllowedValue barFEvSscreenIdx + 1
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
const uint8_t bigDTEscreenIdx =			nextAllowedValue;
#define nextAllowedValue bigDTEscreenIdx + 1
#endif // useBigDTE
#ifdef useBigTTE
const uint8_t bigTTEscreenIdx =			nextAllowedValue;
#define nextAllowedValue bigTTEscreenIdx + 1
#endif // useBigTTE
#ifdef useCPUreading
const uint8_t CPUmonScreenIdx =			nextAllowedValue;
#define nextAllowedValue CPUmonScreenIdx + 1
#endif // useCPUreading
#ifdef useSoftwareClock
const uint8_t softwareClockShowScreenIdx =	nextAllowedValue;
const uint8_t softwareClockEditScreenIdx =	softwareClockShowScreenIdx + 1;
#define nextAllowedValue softwareClockEditScreenIdx + 1
#endif // useSoftwareClock
const uint8_t menuScreenIdx =			nextAllowedValue;
const uint8_t settingScreenIdx =		menuScreenIdx + 1;
const uint8_t paramScreenIdx =			settingScreenIdx + 1;
#define nextAllowedValue paramScreenIdx + 1
#ifdef useSavedTrips
const uint8_t tripSaveCurScreenIdx =		nextAllowedValue;
const uint8_t tripSaveTankScreenIdx =		tripSaveCurScreenIdx + 1;
#define nextAllowedValue tripSaveTankScreenIdx + 1
#ifdef showSavedTrips
const uint8_t tripShowCurScreenIdx =		nextAllowedValue;
const uint8_t tripShowTankScreenIdx =		tripShowCurScreenIdx + 1;
#define nextAllowedValue tripShowTankScreenIdx + 1
#ifdef trackIdleEOCdata
const uint8_t tripShowIECscreenIdx =		nextAllowedValue;
const uint8_t tripShowIETscreenIdx =		tripShowIECscreenIdx + 1;
#define nextAllowedValue tripShowIETscreenIdx + 1
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
const uint8_t partialRefuelScreenIdx =		nextAllowedValue;
#define nextAllowedValue partialRefuelScreenIdx + 1
#endif // usePartialRefuel
#ifdef useScreenEditor
const uint8_t screenEditIdx =			nextAllowedValue;
#define nextAllowedValue screenEditIdx + 1
#endif // useScreenEditor
#ifdef useEEPROMviewer
const uint8_t eepromViewIdx =			nextAllowedValue;
#define nextAllowedValue eepromViewIdx + 1
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
const uint8_t dragRaceIdx =			nextAllowedValue;
#define nextAllowedValue dragRaceIdx + 1
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
const uint8_t coastdownIdx =			nextAllowedValue;
#define nextAllowedValue coastdownIdx + 1
#endif // useCoastDownCalculator

const char stmMain[] PROGMEM =			"Main Display";
#ifdef useBigFE
const char stmBigFE[] PROGMEM =			"Big FuelEcon";
#endif // useBigFE
#ifdef useBarFuelEconVsTime
const char stmFEvT[] PROGMEM =			"FE/Time";
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const char stmFEvS[] PROGMEM =			"FE/Speed";
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
const char stmBigDTE[] PROGMEM =		"Big DistToE";
#endif // useBigDTE
#ifdef useBigTTE
const char stmBigTTE[] PROGMEM =		"Big TimeToE";
#endif // useBigTTE
#ifdef useSoftwareClock
const char stmShowSoftwareClock[] PROGMEM =	"Clock";
#endif // useSoftwareClock
#ifdef usePartialRefuel
const char stmPartialRefuel[] PROGMEM =		"Partial Refuel";
#endif // usePartialRefuel
const char stmSettings[] PROGMEM =		"Settings";
#ifdef useSavedTrips
const char stmCurrSave[] PROGMEM =		"Load/Save CURR";
const char stmTankSave[] PROGMEM =		"Load/Save TANK";
#ifdef showSavedTrips
const char stmCurrView[] PROGMEM =		"See Saved CURR";
const char stmTankView[] PROGMEM =		"See Saved TANK";
#ifdef trackIdleEOCdata
const char stmCCIview[] PROGMEM =		"See Saved cC/I";
const char stmTCIview[] PROGMEM =		"See Saved tC/I";
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef useEEPROMviewer
const char stmEEPROMviewer[] PROGMEM =		"EEPROM Viewer";
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
const char stmDragRace[] PROGMEM =		"Accel Test";
#endif // useDragRaceFunction
#ifdef useCPUreading
const char stmCPUinfo[] PROGMEM =		"CPU Info";
#endif // useCPUreading
#ifdef useCoastDownCalculator
const char stmCoastdown[] PROGMEM =		"Coastdown";
#endif // useCoastDownCalculator

typedef struct
{

	uint8_t menuIndex;
	const char * menuString;

} menuInformation;

static const menuInformation menuItems[] PROGMEM = {
	{settingScreenIdx, stmSettings}
#ifdef useDragRaceFunction
	,{dragRaceIdx, stmDragRace}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,{coastdownIdx, stmCoastdown}
#endif // useCoastDownCalculator
#ifdef usePartialRefuel
	,{partialRefuelScreenIdx, stmPartialRefuel}
#endif // usePartialRefuel
#ifdef useSavedTrips
	,{tripSaveCurScreenIdx, stmCurrSave}
	,{tripSaveTankScreenIdx, stmTankSave}
#ifdef showSavedTrips
	,{tripShowCurScreenIdx, stmCurrView}
	,{tripShowTankScreenIdx, stmTankView}
#ifdef trackIdleEOCdata
	,{tripShowIECscreenIdx, stmCCIview}
	,{tripShowIETscreenIdx, stmTCIview}
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef useBigFE
	,{bigFEscreenIdx, stmBigFE}
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,{barFEvTscreenIdx, stmFEvT}
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,{barFEvSscreenIdx, stmFEvS}
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,{bigDTEscreenIdx, stmBigDTE}
#endif // useBigDTE
#ifdef useBigTTE
	,{bigTTEscreenIdx, stmBigTTE}
#endif // useBigTTE
#ifdef useSoftwareClock
	,{softwareClockShowScreenIdx, stmShowSoftwareClock}
#endif // useSoftwareClock
#ifdef useEEPROMviewer
	,{eepromViewIdx, stmEEPROMviewer}
#endif // useEEPROMviewer
#ifdef useCPUreading
	,{CPUmonScreenIdx, stmCPUinfo}
#endif // useCPUreading
	,{mainScreenIdx, stmMain}
};

const uint8_t menuSize = (sizeof(menuItems) / sizeof(menuInformation));

typedef struct
{

	uint8_t modeIndex;
	uint8_t modeYcount;
	uint8_t modeXcount;
	void (* modeDisplay)(void);
	void (* modeCursorUpdate)(void);
	const buttonList (* modeButtonList);

} modeInformation;

static const modeInformation screenParameters[(unsigned int)(totalMenuSize)] PROGMEM = {
	{mainScreenIdx,				mainScreenSize,		displayPageCount, 			doMainScreenDisplay, 		doCursorUpdateMain,	 	bpListMain}
#ifdef useBigFE
	,{mainScreenIdx,			mainScreenSize,		3,					doBigFEdisplay,			doCursorUpdateBigFEscreen,	bpListBigNum}
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,{mainScreenIdx,			mainScreenSize,		2,					doBarFEvTdisplay,		doCursorUpdateBarFEvT,		bpListBFET}
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,{mainScreenIdx,			mainScreenSize,		4,					doBarFEvSdisplay,		doCursorUpdateBarFEvS,		bpListBFES}
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,{mainScreenIdx,			mainScreenSize,		3,					doBigDTEdisplay,		doCursorUpdateBigDTEscreen,	bpListBigNum}
#endif // useBigDTE
#ifdef useBigTTE
	,{mainScreenIdx,			mainScreenSize,		3,					doBigTTEdisplay,		doCursorUpdateBigTTEscreen,	bpListBigNum}
#endif // useBigTTE
#ifdef useCPUreading
	,{mainScreenIdx | 0x80,			mainScreenSize,		1,					doDisplaySystemInfo,		doNothing,			bpListCPUmonitor}
#endif // useCPUreading
#ifdef useSoftwareClock
	,{mainScreenIdx | 0x80,			mainScreenSize,		1,					doShowSoftwareClock,		doNothing,			bpListSoftwareClockShow}
	,{softwareClockEditScreenIdx | 0x80,	1,			4,					doEditSoftwareClock,		doNothing,			bpListSoftwareClockEdit}
#endif // useSoftwareClock
	,{menuScreenIdx | 0x80,			1,			menuSize,				menu::display,			doNothing,			bpListMenu}
	,{settingScreenIdx | 0x80,		1,			eePtrSettingsEnd - 1,			settings::display,		settings::cursorUpdate,		bpListSetting}
	,{paramScreenIdx | 0x80,		1,			12,					parameter::display,		parameter::doCursorUpdate,	bpListParam}
#ifdef useSavedTrips
	,{tripSaveCurScreenIdx | 0x80,		2,			tripSaveMenuSize,			doTripSaveDisplay,		doNothing,			bpListTripSave}
	,{tripSaveCurScreenIdx | 0x80,		2,			tripSaveMenuSize,			doTripSaveDisplay,		doNothing,			bpListTripSave}
#ifdef showSavedTrips
	,{tripShowCurScreenIdx | 0x80,		savedTripsCount,	tripSlotPtrSize,			doTripShowDisplay,		doNothing,			bpListTripView}
	,{tripShowCurScreenIdx | 0x80,		savedTripsCount,	tripSlotPtrSize,			doTripShowDisplay,		doNothing,			bpListTripView}
#ifdef trackIdleEOCdata
	,{tripShowCurScreenIdx | 0x80,		savedTripsCount,	tripSlotPtrSize,			doTripShowDisplay,		doNothing,			bpListTripView}
	,{tripShowCurScreenIdx | 0x80,		savedTripsCount,	tripSlotPtrSize,			doTripShowDisplay,		doNothing,			bpListTripView}
#endif // trackIdleEOCdata
#endif // showSavedTrips
#endif // useSavedTrips
#ifdef usePartialRefuel
	,{partialRefuelScreenIdx | 0x80,	1,			3,					doPartialRefuelDisplay,		doNothing,			bpListPartialRefuel}
#endif // usePartialRefuel
#ifdef useScreenEditor
	,{screenEditIdx | 0x80,			1,			mainScreenDisplayFormatSize * 2,	doScreenEditDisplay,		doCursorUpdateScreenEdit,	bpListScreenEdit}
#endif // useScreenEditor
#ifdef useEEPROMviewer
	,{eepromViewIdx | 0x80,			1,			eePtrEnd,				EEPROM::display,		doNothing,			bpListEEPROMview}
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
	,{dragRaceIdx,				1,			4,					accelerationTest::goDisplay,	doNothing,			bpListDragRace}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,{coastdownIdx | 0x80,			1,			3,					coastdown::goDisplay,		doNothing,			bpListCoastdown}
#endif // useCoastDownCalculator
};

uint8_t screenCursor[(unsigned int)(totalMenuSize)] = {
	0
	,0
	,0
#ifdef useCPUreading
	,0
#endif // useCPUreading
#ifdef useBigFE
	,0
#endif // useBigFE
#ifdef useBigDTE
	,0
#endif // useBigDTE
#ifdef useBigTTE
	,0
#endif // useBigTTE
#ifdef useBarFuelEconVsTime
	,0
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,0
#endif // useBarFuelEconVsSpeed
#ifdef useSoftwareClock
	,0
	,0
#endif // useSoftwareClock
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
#ifdef useScreenEditor
	,0
#endif // useScreenEditor
#ifdef useEEPROMviewer
	,0
#endif // useEEPROMviewer
#ifdef useDragRaceFunction
	,0
#endif // useDragRaceFunction
};

#ifdef useCoastDownCalculator
uint64_t matrix_x[3][3];	// coastdown sample storage
uint64_t matrix_r[3][3];	// inverse matrix
uint64_t matrix_e[3];		// coastdown energy value storage
uint64_t matrix_c[3];		// coastdown coefficient matrix (what we are trying to solve for

#endif // useCoastDownCalculator
#ifdef useBuffering
namespace ringBuffer // ringBuffer prototype
{

	void init(ringBufferVariable &bfr, volatile uint8_t * storage, deviceCallFunc devCallPtr);
	void push(ringBufferVariable &bfr, uint8_t value);
	void pull(ringBufferVariable &bfr);
	void updateStatus(ringBufferVariable &bfr, uint8_t clearFlag, uint8_t setFlag);
	void flush(ringBufferVariable &bfr);

};

#endif
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
#endif

};

namespace text // text string output prototype
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

#ifdef use4BitLCD
outputDevice devLCD;

namespace LCD // LCD prototype
{

	void init(void);
	void hardwareInit(void);
	void shutdown(void);
	void gotoXY(uint8_t x, uint8_t y);
#ifdef useBigNumberDisplay
	void loadCGRAMfont(const char * fontPtr);
#endif // useBigNumberDisplay
	void loadCGRAMcharacter(uint8_t chr, char * chrData);
	void setBrightness(uint8_t idx);
#ifdef useAnalogLCDdelay
#ifdef useLegacyLCD
	void setContrast(uint8_t idx);
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDmodule
	void setRGBcolor(uint8_t idx);
#endif // useAdafruitRGBLCDmodule
	void writeCommand(uint8_t value);
#endif // useAnalogLCDdelay
	void writeData(uint8_t value);
#ifdef useAnalogLCDdelay
#ifdef useTWILCD
	void initTWItransmit(uint8_t interruptFlag);
	void writeTWIportLCD(void);
	void portLCDdirectTWIout(void);
#endif // useTWILCD
	void writeByte(uint8_t value, uint8_t flags);
	void writeNybble(uint8_t value, uint8_t flags);
	void outputNybble(uint8_t s);
#ifdef useBufferedLCD
	uint8_t bufferDevice(uint8_t cmd, uint8_t data);
#endif // useBufferedLCD
#endif // useAnalogLCDdelay

};

#ifdef useAnalogLCDdelay
#ifdef useBufferedLCD
ringBufferVariable lcdBuffer;

volatile uint8_t LCDdata[32];

#endif // useBufferedLCD
volatile unsigned int lcdDelayCount;

#endif // useAnalogLCDdelay
#endif // use4BitLCD
#ifdef useAnalogInterrupt
volatile uint8_t analogState;

#ifdef useDebugReadings
const uint8_t asDebugActive =		0b00100000;
#endif // useDebugReadings
#ifdef useAnalogLCDdelay
const uint8_t asLCDdelayTimer =		0b00010000;
#endif // useAnalogLCDdelay
#ifdef useAnalogRead
#ifdef useAnalogButtons
const uint8_t asReadButton =		0b00001000;
#endif // useAnalogButtons
const uint8_t asInitializeRead =	0b00000100;
const uint8_t asReadChannel =		0b00000010;
#endif // useAnalogRead
const uint8_t asReadInternalGround =	0b00000001;
const uint8_t asKeepInterrupt =		0
#ifdef useAnalogRead
	| asReadChannel
#ifdef useAnalogButtons
	| asReadButton
#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useDebugReadings
	| asDebugActive
#endif // useDebugReadings
#ifdef useAnalogLCDdelay
	| asLCDdelayTimer
#endif // useAnalogLCDdelay
;
#ifdef useAnalogRead
const uint8_t asPerformRead =		asReadChannel
#ifdef useAnalogButtons
	| asReadButton
#endif
;

volatile unsigned int sampleCount;
volatile unsigned int analogValue[(unsigned int)(dfMaxValAnalogCount)];

volatile uint8_t analogChannelValue[(unsigned int)(dfMaxValAnalogCount)] = { // points to the next channel to be read
#ifdef useTinkerkitLCDmodule
	(1 << REFS0)|	(1 << MUX2)|			(1 << MUX0)	// analog channel 1
	,(1 << REFS0)|	(1 << MUX2)|	(1 << MUX1)			// analog channel 2
	,(1 << REFS0)|	(1 << MUX2)|	(1 << MUX1)|	(1 << MUX0)	// analog channel 3
#else
	(1 << REFS0)|					(1 << MUX0)	// analog channel 1
	,(1 << REFS0)|			(1 << MUX1)			// analog channel 2
#ifdef useAnalogButtons
	,(1 << REFS0)|			(1 << MUX1)|	(1 << MUX0)	// analog channel 3
#ifndef useTWIsupport
	,(1 << REFS0)|	(1 << MUX2)					// analog channel 4
	,(1 << REFS0)|	(1 << MUX2)|			(1 << MUX0)	// analog channel 5
#endif
#endif
#endif
};

#ifdef useAnalogButtons
volatile uint8_t analogButtonChannel = 2;
volatile uint8_t thisAnalogButtonPressed = buttonsUp;

#endif
#endif
#endif
volatile unsigned long timer0_overflow_count;
volatile unsigned long systemCycles[2] = { 0, 0 };
#ifdef useSoftwareClock
volatile unsigned long clockCycles[2] = { 0, 0 };
#endif
volatile unsigned long lastVSStime;

volatile unsigned int injResetCount;
volatile unsigned int VSSresetCount;
volatile unsigned int buttonCount;
volatile unsigned int timerDelayCount;
volatile unsigned int displayPauseCount;
volatile unsigned int injResetDelay;

volatile uint8_t VSScount;
volatile uint8_t VSSpause;
volatile uint8_t buttonState;
volatile uint8_t mainLoopHeartBeat;
volatile uint8_t dirty;
volatile uint8_t activityFlags;
volatile uint8_t timerCommand;
volatile uint8_t timerStatus;

tripVar tripArray[(unsigned int)(tripSlotCount)]; // main object we will be working with

const uint8_t metricMode =	0b00000001;

uint8_t metricFlag;
uint8_t menuLevel;
uint8_t topScreenLevel;
uint8_t brightnessIdx;

char mBuff1[17]; // used by formatDecimal(), doFormat(), bar graph routines
char pBuff[12]; // used by editParm(), editClock()

const uint8_t internalAwakeOnInjector =	0b10000000;
const uint8_t internalAwakeOnVSS =	0b01000000;
const uint8_t internalAwakeOnButton =	0b00100000;
const uint8_t internalButtonRead =	0b00010000;
#ifdef useTWIsupport
const uint8_t internalSampleTWI =	0b00001000;
#endif // useTWIsupport
#ifdef useAnalogRead
const uint8_t internalSampleADC =	0b00000100;
#endif // useAnalogRead
#ifdef useCoastDownCalculator
const uint8_t internalCancelCDT =	0b00000010;
#endif // useCoastDownCalculator
const uint8_t internalAwake =		internalAwakeOnInjector | internalAwakeOnVSS | internalAwakeOnButton;

// this ISR gets called every time timer 0 overflows.
// timer 2 prescaler is set at 64, and it's an 8 bit counter
// so this ISR gets called every 256 * 64 / (system clock) seconds (for 20 MHz clock, that is every 0.8192 ms)

ISR( TIMER0_OVF_vect ) // system timer interrupt handler
{

	static uint8_t lastButtonPressed = 0;
	static uint8_t thisButtonPressed;
#ifdef useTWIbuttons
	static uint8_t thisTWIbuttonPressed;
	static uint8_t lastTWIbuttonPressed;
#endif // useTWIbuttons
	static uint8_t internalFlags = 0;
#ifdef useTWIsupport
	static uint8_t TWIsampleCount = 0;
	static uint8_t TWIsampleState = 0;
#endif // useTWIsupport
	static unsigned int cursorCount;
	static unsigned int loopCount = 0;
#ifdef useJSONoutput
	static unsigned int JSONtimeoutCount = 0;
#endif // useJSONoutput
	static unsigned long lastTime;
	static unsigned long idleTimeoutCount = 0;
	static unsigned long EOCtimeoutCount = 0;
	static unsigned long buttonTimeoutCount = 0;
	static unsigned long parkTimeoutCount = 0;
	static unsigned long activityTimeoutCount = 0;
#ifdef useCoastDownCalculator
	static unsigned long coastdownCount = 0;
#endif // useCoastDownCalculator

	unsigned long thisTime;
	unsigned long cycleLength;

	timer0_overflow_count += 256; // update TOV count
	thisTime = timer0_overflow_count | TCNT0; // calculate current cycle count

	if (dirty & dirtySysTick)
	{

		cycleLength = findCycleLength(lastTime, thisTime);

		systemCycles[0] += cycleLength;
		if (systemCycles[0] < cycleLength) systemCycles[1]++;

#ifdef useSoftwareClock
		clockCycles[0] += cycleLength;
		if (clockCycles[0] < cycleLength) clockCycles[1]++;

#endif // useSoftwareClock
		if (VSScount) // if there is a VSS debounce countdown in progress
		{

			VSScount--; // bump down the VSS count
			if (VSScount == 0) updateVSS(thisTime); // if count has reached zero, go update VSS

		}

	}
	else
	{

		dirty |= dirtySysTick;
		internalFlags = internalAwakeOnButton;
		mainLoopHeartBeat = 1;
		injResetCount = 0;
		VSSresetCount = 0;
		buttonCount = 0;

	}

	lastTime = thisTime; // save cycle count

	if (dirty & dirtyWatchInjector)
	{

		internalFlags |= internalAwakeOnInjector; // say that engine is keeping main program awake
#ifdef useAnalogRead
		internalFlags |= internalSampleADC;
#endif // useAnalogRead
		dirty &= ~(dirtyWatchInjector);
		dirty |= dirtyInjectorMonitor;
		injResetCount = injResetDelay;
		EOCtimeoutCount = volatileVariables[(unsigned int)(vEOCtimeoutIdx)];
		activityFlags &= ~(afEOCflag);
		if (activityFlags & afDoResetOnVehicle)
		{

			activityFlags &= ~(afDoResetCheckFlags);
			activityFlags |= afResetCurrentTrip;

		}

	}
	else
	{

		if (dirty & dirtyInjectorMonitor) // if a fuel injector was recently read
		{

			if (injResetCount) injResetCount--; // execute fuel injection monitor timeout
			else // if timeout is complete, cancel any pending injector pulse read and signal that no injector pulse has been read in a while
			{

				dirty &= ~(dirtyGoodInj | dirtyInjOpenRead | dirtyInjectorMonitor); // reset fuel injection state

			}

		}

	}

	if (dirty & dirtyWatchVSS)
	{

		internalFlags |= internalAwakeOnVSS; // say that VSS is keeping main program awake
		dirty &= ~(dirtyWatchVSS);
		dirty |= dirtyVSSmonitor;
		VSSresetCount = vssResetDelay;
		idleTimeoutCount = volatileVariables[(unsigned int)(vIdleTimeoutIdx)];
		activityFlags &= ~(afIdleFlag);
		if (activityFlags & afDoResetOnVehicle)
		{

			activityFlags &= ~(afDoResetCheckFlags);
			activityFlags |= afResetCurrentTrip;

		}

	}
	else
	{

		if (dirty & dirtyVSSmonitor)
		{

			if (VSSresetCount) VSSresetCount--; // if vehicle speed monitor is in progress
			else // if we detect that the vehicle is stopped
			{

				dirty &= ~(dirtyGoodVSS | dirtyVSSmonitor); // reset vehicle speed state
#ifdef useDragRaceFunction

				if (accelerationFlags & accelTestInProgress) // if acceleration test has started
				{

					accelerationFlags &= ~(accelTestClearFlags); // reset drag race capture flags
					accelerationFlags |= (accelTestCompleteFlags); // signal that drag function is cancelled

				}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator

				internalFlags |= internalCancelCDT; // coastdown test will cancel if vehicle is idling
#endif // useCoastDownCalculator

			}

		}

	}

#ifdef useCoastDownCalculator
	if (internalFlags & internalCancelCDT)
	{

		internalFlags &= ~(internalCancelCDT);
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

				coastdownFlags &= ~(cdtTriggered); // clear coastdown test state
				coastdownFlags |= cdtActive | cdSignalStateChange; // mark coastdown test as active
				coastdownCount = volatileVariables[(unsigned int)(vCoastdownPeriodIdx)]; // reset coastdown counter

			}
			else
			{

				if (coastdownCount) coastdownCount--; // if coastdown clock hasn't elapsed
				else // perform state action
				{

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

#ifdef useAdafruitRGBLCDmodule
					TWI::initWrite(lcdAddress, TWI_REPEAT_START, TWI_INTERRUPT); // open TWI as master transmitter, do repeated start to set up for read, flag as driven within interrupt
					TWI::writeByte(MCP23017_GPIOx | MCP23017_PORTA); // specify bank A GPIO pin register address
					TWI::commitWrite(); // go write out read request
#endif // useAdafruitRGBLCDmodule
					TWIsampleState++; // advance to waiting for TWI sample request to finish

				}

			case 1:
				if ((twiStatusFlags & twiTxRxStatus) == 0) // wait for TWI button pin register address write to complete
				{

#ifdef useAdafruitRGBLCDmodule
					TWI::initMasterDevice(lcdAddress, TWI_READ, TWI_STOP, TWI_INTERRUPT); // initialize master receiver, flag as driven within interrupt
					twiMasterBufferLen = 0; // go look at comments for TWI::readDevice to see why this is zero
					TWI::commitMasterDevice(false); // go commit to read
#endif // useAdafruitRGBLCDmodule

					TWIsampleState++; // advance to waiting for TWI sample to complete

				}
				break;

			case 2:
				if ((twiStatusFlags & twiTxRxStatus) == 0) // wait for TWI button read to complete
				{

					timerStatus &= ~(tsTWIsampleActive);
					TWIsampleState++; // advance to debouncing initialization

					if (twiStatusFlags & twiErrorFlag) TWIsampleState = 255; // if an error occurred during read, exit
					else
					{

#ifdef useTWIbuttons
#ifdef useAdafruitRGBLCDmodule
						thisTWIbuttonPressed = twiMasterBufferData[0]; // fetch button state that was just read in
						thisTWIbuttonPressed &= (btnShortPress21RCL);
						if (thisTWIbuttonPressed ^ lastTWIbuttonPressed) buttonCount = buttonDelay;
						lastTWIbuttonPressed = thisTWIbuttonPressed;
#endif // useAdafruitRGBLCDmodule
#endif // useTWIbuttons

					}

				}
				break;

			case 3:
			default:
				internalFlags &= ~(internalSampleTWI);
				twiStatusFlags &= ~(twiBlockMainProgram);
				break;

		}

	}
	else
	{

		if (TWIsampleCount) TWIsampleCount--;
		else
		{

			TWIsampleCount = 30;

			if (!(timerCommand & tcBlockTWIsample))
			{

				timerStatus |= tsTWIsampleActive;
				internalFlags |= internalSampleTWI; // signal that an internal TWI session is in progress
				twiStatusFlags |= twiBlockMainProgram; // block main program from making any TWI requests
				TWIsampleState = 0; // initialize TWI button read state machine

			}

		}

	}

#endif // useTWIsupport
	if (buttonCount) // if there is a button press debounce countdown in progress
	{

		buttonCount--; // bump down the button press count by one

		if (buttonCount == 0) lastButtonPressed |= longButtonBit; // signal that a "long" button press has been detected

		if (buttonCount == buttonShortDelay) // if button debounce countdown reaches this point
		{

			// figure out what buttons are being pressed
#ifdef useLegacyButtons
			thisButtonPressed = buttonsUp & lastPINxState;
#endif // useLegacyButtons
#ifdef useAnalogButtons
			thisButtonPressed = thisAnalogButtonPressed;
#endif // useAnalogButtons
#ifdef useTWIbuttons
			thisButtonPressed = thisTWIbuttonPressed;
#endif // useTWIbuttons

			if (thisButtonPressed != buttonsUp) // if any buttons are pressed
			{

				lastButtonPressed = thisButtonPressed; // remember the button press status for later
				internalFlags |= internalButtonRead; // signal that a button has been read in

			}
			else buttonCount = 0; // reset button press debounce countdown to zero

		}

		if (buttonCount == 0) // if a button has been read, go pass it on to the main program
		{

			internalFlags |= internalAwakeOnButton; // tell system timer to wake up the main program

			// if a valid button press was read in, and main program is awake
			if ((internalFlags & internalButtonRead) && (!(timerStatus & tsFellAsleep)))
			{

				internalFlags &= ~internalButtonRead;
				buttonState = lastButtonPressed; // pass off the remembered button press status to the main program
				timerStatus |= tsButtonRead; // signal main program that a key press was detected
				timerCommand &= ~tcDisplayDelay;
				timerCommand |= (tcShowCursor | tcUpdateDisplay); // force cursor show bit, and signal that display needs updating
				cursorCount = cursorDelay; // reset cursor count
				activityFlags &= ~(afButtonFlag);
				if (activityFlags & afDoResetOnButton)
				{

					activityFlags &= ~(afDoResetCheckFlags);
					activityFlags |= afResetCurrentTrip;

				}
				buttonTimeoutCount = volatileVariables[(unsigned int)(vButtonTimeoutIdx)];

			}

		}

	}

#ifdef useAnalogRead
	if (sampleCount) sampleCount--;
	else internalFlags |= internalSampleADC;

#endif // useAnalogRead
#ifdef useJSONoutput
	if (JSONtimeoutCount) JSONtimeoutCount--;
	else
	{

		timerCommand |= tcOutputJSON; // signal to JSON output routine to display next round of subtitles
		JSONtimeoutCount = JSONtickLength - 1; // restart JSON output timeout count

	}

#endif // useJSONoutput
	if (loopCount) loopCount--;
	else
	{

		timerCommand |= (tcTakeSample | tcUpdateDisplay); // signal to main program that a sampling should occur, and to update display
		loopCount = loopTickLength - 1; // restart loop count
		mainLoopHeartBeat <<= 1; // cycle the heartbeat bit
		if (mainLoopHeartBeat == 0) mainLoopHeartBeat = 1; // wrap around the heartbeat bit, if necessary

	}

	if (cursorCount) cursorCount--;
	else
	{

		cursorCount = cursorDelay; // reset cursor count
		timerCommand ^= tcShowCursor; // toggle cursor show bit

	}

	if (timerCommand & tcDisplayDelay) // if message pause is in effect
	{

		if (displayPauseCount) displayPauseCount--; // update pause counter
		else
		{

			timerCommand &= ~tcDisplayDelay; // otherwise, signal that message pause is over
			timerCommand |= tcUpdateDisplay; // tell main program to update the display

		}

	}

	if (timerCommand & tcDoDelay) // if main program has requested a delay
	{

		if (timerDelayCount) timerDelayCount--; // bump timer delay value down by one tick
		else timerCommand &= ~tcDoDelay; // signal to main program that delay timer has completed main program request

	}

	if (internalFlags & internalAwake) // if buttonpress or fuel injector or VSS modules have commanded a wake-up
	{

		if (internalFlags & internalAwakeOnInjector) timerStatus |= tsAwakeOnInjector; // set awake status on engine running
		if (internalFlags & internalAwakeOnVSS) timerStatus |= tsAwakeOnVSS; // set awake status on moving vehicle
		if (internalFlags & internalAwakeOnButton) timerStatus |= tsAwakeOnButton; // set awake status on button pressed

		internalFlags &= ~internalAwake; // clear wakeup command
		activityFlags &= ~(afParkFlag | afActivityTimeoutFlag);
		parkTimeoutCount =  volatileVariables[(unsigned int)(vParkTimeoutIdx)]; // reset park counter
		activityTimeoutCount = volatileVariables[(unsigned int)(vActivityTimeoutIdx)]; // reset sleep counter

	}

	if (idleTimeoutCount) idleTimeoutCount--;
	else
	{

		if ((activityFlags & afIdleFlag) == 0)
		{

			activityFlags |= afIdleFlag;
			timerCommand |= tcUpdateDisplay;

		}

	}

	if (EOCtimeoutCount) EOCtimeoutCount--;
	else
	{

		if ((activityFlags & afEOCflag) == 0)
		{

			activityFlags |= afEOCflag;
			timerCommand |= tcUpdateDisplay;

		}

	}

	if (buttonTimeoutCount) buttonTimeoutCount--;
	else
	{

		if ((activityFlags & afButtonFlag) == 0)
		{

			activityFlags |= afButtonFlag;
			timerCommand |= tcUpdateDisplay;

		}

	}

	if (parkTimeoutCount) parkTimeoutCount--;
	else
	{

		if ((activityFlags & afParkFlag) == 0)
		{

			activityFlags |= (afParkFlag | afEOCflag | afIdleFlag);
			timerCommand |= tcUpdateDisplay;

		}

		if (activityTimeoutCount) activityTimeoutCount--;
		else
		{

			if ((activityFlags & afActivityTimeoutFlag) == 0)
			{

				activityFlags |= afActivityTimeoutFlag;
				timerCommand |= tcUpdateDisplay;
				timerStatus &= ~tsAwake; // clear awake status

			}

		}

	}

#ifdef useAnalogRead
	if (internalFlags & internalSampleADC)
	{

		internalFlags &= ~(internalSampleADC);
		if ((analogState & asInitializeRead) == 0) initADC(asInitializeRead); // initiate a round of ADC readings

	}

#endif // useAnalogRead
}

volatile unsigned long lastInjOpenStart;
volatile unsigned long thisInjOpenStart;
volatile unsigned long totalInjCycleLength;
volatile unsigned long maximumInjOpenCycleLength;

ISR( INJ_OPEN_vect ) // injector opening event handler
{

	unsigned int a;

	lastInjOpenStart = thisInjOpenStart;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjOpenStart = timer0_overflow_count + (unsigned long)(a);

	if (dirty & dirtyGoodInj)
	{

		// calculate fuel injector length between pulse starts
		totalInjCycleLength = findCycleLength(lastInjOpenStart, thisInjOpenStart);

		if (totalInjCycleLength < volatileVariables[(unsigned int)(vMinGoodRPMcyclesIdx)])
		{

			maximumInjOpenCycleLength = 205 * totalInjCycleLength; // to determine instantaneous maximum injector on-time
			maximumInjOpenCycleLength >>= 8; // and multiply it by 0.8 (or something reasonably close) for injector duty cycle

		}
		else
		{

			totalInjCycleLength = 0;
			dirty &= ~dirtyGoodInj; // signal that no injector pulse has been read for a while

		}

	}

	if (!(dirty & dirtyGoodInj)) maximumInjOpenCycleLength = volatileVariables[(unsigned int)(vMaxGoodInjCyclesIdx)]; // seed working maxGoodInjCycles with default value

	dirty |= dirtyInjOpenRead; // signal that injector pulse read is in progress

}

ISR( INJ_CLOSE_vect ) // injector closing event handler
{

	unsigned long thisTime;
	unsigned long injOpenCycleLength = 0;
	unsigned int a;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisTime = timer0_overflow_count + (unsigned long)(a);

	if (dirty & dirtyInjOpenRead)
	{

		// calculate fuel injector pulse length
		injOpenCycleLength = findCycleLength(thisInjOpenStart, thisTime) - volatileVariables[(unsigned int)(vInjSettleCyclesIdx)];

		if (injOpenCycleLength < maximumInjOpenCycleLength) // perform rationality test on injector open cycle pulse length
		{

#ifdef useChryslerMAPCorrection
			injOpenCycleLength *= volatileVariables[(unsigned int)(vInjCorrectionIdx)]; // multiply by differential fuel pressure correction factor
			injOpenCycleLength >>= 12; // divide by denominator factor

#endif // useChryslerMAPCorrection
			dirty |= dirtyGoodInj; // signal that a valid fuel injector pulse has just been read

		}
		else
		{

			injOpenCycleLength = 0;
			dirty &= ~dirtyGoodInj; // signal that no injector pulse has been read
			injResetCount = 0; // stop injector validity monitor

		}

		dirty &= ~dirtyInjOpenRead; // signal that the injector pulse has been read

	}

	if (injOpenCycleLength)
	{

#ifdef trackIdleEOCdata
		if (dirty & dirtyGoodVSS)
		{

			tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // update the injector pulse count
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjOpenCycleIdx, injOpenCycleLength); // add to fuel injector open cycle accumulator

		}
		else
		{

			tripArray[(unsigned int)(rawEOCidleTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // update the idle injector pulse count
			tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvInjOpenCycleIdx, injOpenCycleLength); // add to idle fuel injector open cycle accumulator

		}
#else // trackIdleEOCdata
		tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // update the injector pulse count
		tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjOpenCycleIdx, injOpenCycleLength); // add to fuel injector open cycle accumulator
#endif // trackIdleEOCdata
#ifdef useDragRaceFunction
		if (accelerationFlags & accelTestActive)
		{

			tripArray[(unsigned int)(dragRawDistanceIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++; // update the distance acceleration injector pulse count
			tripArray[(unsigned int)(dragRawDistanceIdx)].add64s(rvInjOpenCycleIdx, injOpenCycleLength); // add to distance acceleration fuel injector open cycle accumulator

		}

#endif // useDragRaceFunction
	}

#ifdef trackIdleEOCdata
	if (dirty & dirtyGoodVSS) tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjCycleIdx, totalInjCycleLength); // add to fuel injector total cycle accumulator
	else tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvInjCycleIdx, totalInjCycleLength); // add to idle fuel injector total cycle accumulator
#else // trackIdleEOCdata
	tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjCycleIdx, totalInjCycleLength); // add to fuel injector total cycle accumulator
#endif // trackIdleEOCdata

#ifdef useDragRaceFunction
	if (accelerationFlags & accelTestActive) tripArray[(unsigned int)(dragRawDistanceIdx)].add64s(rvInjCycleIdx, totalInjCycleLength); // add to distance acceleration fuel injector total cycle accumulator

#endif // useDragRaceFunction
	totalInjCycleLength = 0;

}

ISR( PCINTx_vect )
{

	static uint8_t p;
	static uint8_t q;

	unsigned long thisTime;
	unsigned int a;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisTime = timer0_overflow_count + (unsigned long)(a);

	p = PINx; // read current input pin
	q = p ^ lastPINxState; // detect any changes from the last time this ISR is called

	if (q & vssBit) // if a VSS pulse is received
	{

		if (VSSpause) VSScount = VSSpause; // if there is a VSS debounce count defined, set VSS debounce count and let system timer handle the debouncing
		else updateVSS(thisTime); // otherwise, go process VSS pulse

	}

#ifdef useLegacyButtons
	if (q & buttonsUp) buttonCount = buttonDelay; // set button press debounce count, and let system timer handle the debouncing

#endif // useLegacyButtons
	lastPINxState = p; // remember the current input pin state for the next time this ISR gets called

}

#ifdef useParallax5PositionSwitch
const unsigned int analogButtonThreshold[] PROGMEM = {
	0,
	559,
	580,
	586,
	618,
	651,
	664,
	693,
	717,
	728,
	748,
	766,
	786,
	814,
	834,
	858,
	897,
	927,
	980
};

const uint8_t analogButtonCount = (sizeof(analogButtonThreshold) / sizeof(unsigned int));

const uint8_t analogTranslate[(unsigned int)(analogButtonCount)] PROGMEM = {
	buttonsUp,
	btnShortPress1CL,
	btnShortPress2CL,
	btnShortPressCL,
	btnShortPress1L,
	btnShortPress2L,
	btnShortPressL,
	btnShortPress1RC,
	btnShortPress2RC,
	btnShortPressRC,
	btnShortPress1C,
	btnShortPress2C,
	btnShortPressC,
	btnShortPress1R,
	btnShortPress2R,
	btnShortPressR,
	btnShortPress1,
	btnShortPress2,
	buttonsUp
};

#endif
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
	btnShortPress21RCL,
	btnShortPress1RCL,
	btnShortPress2RCL,
	btnShortPressRCL,
	btnShortPress21CL,
	btnShortPress1CL,
	btnShortPress2CL,
	btnShortPressCL,
	btnShortPress21RL,
	btnShortPress1RL,
	btnShortPress2RL,
	btnShortPressRL,
	btnShortPress21L,
	btnShortPress1L,
	btnShortPress2L,
	btnShortPressL,
	btnShortPress21RC,
	btnShortPress1RC,
	btnShortPress2RC,
	btnShortPressRC,
	btnShortPress21C,
	btnShortPress1C,
	btnShortPress2C,
	btnShortPressC,
	btnShortPress21R,
	btnShortPress1R,
	btnShortPress2R,
	btnShortPressR,
	btnShortPress21,
	btnShortPress1,
	btnShortPress2,
	buttonsUp
};

#endif
#ifdef useAnalogInterrupt
#ifdef useDebugReadings
volatile uint8_t debugFlags;
volatile uint8_t VSSdebugCount;
volatile unsigned int injectorDebugCount;

const uint8_t debugVSSflag =			0b00000001;
const uint8_t debugInjectorFlag =		0b00000010;

#endif
ISR( ADC_vect )
{

#ifdef useDebugReadings
	static unsigned long lastDebugCycleCount;
	static unsigned long thisDebugCycleCount;
	unsigned int a;
#endif // useDebugReadings
#ifdef useAnalogRead
#ifdef useAnalogButtons
	static uint8_t lastAnalogButtonPressed = buttonsUp;
#endif // useAnalogButtons
	unsigned int rawRead;
	union union_16 * rawValue = (union union_16 *)(&rawRead);
	static uint8_t analogChannelIdx = dfMaxValAnalogCount - 1;

	rawValue->u8[0] = ADCL; // (locks ADC sample result register from AtMega hardware)
	rawValue->u8[1] = ADCH; // (releases ADC sample result register to AtMega hardware)

#endif // useAnalogRead
#ifdef useAnalogLCDdelay
	if (analogState & asLCDdelayTimer) // if main program has requested a delay
	{

		if (lcdDelayCount) lcdDelayCount--; // bump timer delay value down by one tick
		else
		{

#ifdef useBufferedLCD
#ifdef useTWILCD
			if ((twiStatusFlags & twiTxRxStatusMain) == 0) ringBuffer::pull(lcdBuffer); // pull a buffered LCD byte and output it
#else // useTWILCD
			ringBuffer::pull(lcdBuffer); // pull a buffered LCD byte and output it
#endif // useTWILCD
#else // useBufferedLCD
			analogState &= ~(asLCDdelayTimer); // signal to main program that delay timer has completed main program request
#endif // useBufferedLCD

		}

	}

#endif // useAnalogLCDdelay
#ifdef useAnalogRead
	if (analogState & asInitializeRead) // if analog read set requested, go initialize analog read mechanism
	{

		analogState &= ~(asInitializeRead); // clear analog read request
		analogChannelIdx = dfMaxValAnalogCount; // start with end channel
		analogState |= asReadChannel; // start by signalling to keep analog interrupt alive
		sampleCount = sampleTickLength;

	}

	if (analogState & asReadInternalGround) // ADC module just got done reading internal ground
	{

#ifdef useAnalogButtons
		if (analogState & asReadChannel)
		{
#endif
			analogChannelIdx--;

			if (analogChannelIdx >= dfMaxValAnalogCount) // if analog channel index rolled around, there are no more channels to be read
			{

				analogState &= ~(asReadChannel); // shut down analog read set

#ifdef useChryslerMAPCorrection
				calculateMAPcorrection(); // calculate correction factor for differential pressure across the fuel injector

#endif // useChryslerMAPCorrection
#ifdef useAnalogButtons
				if (analogState & asReadButton) analogChannelIdx = analogButtonChannel; // select button analog channel

#endif
			}

#ifdef useAnalogButtons
		}
		else analogChannelIdx = analogButtonChannel; // select button analog channel
#endif

		if (analogChannelIdx < dfMaxValAnalogCount) ADMUX = analogChannelValue[(unsigned int)(analogChannelIdx)]; // select next analog channel to read

	}
	else
	{

#ifdef useTinkerkitLCDmodule
		ADMUX = (1 << REFS0) | (1 << MUX4) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#else
		ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ground ADC sample/hold capacitor to reset it
#endif

		if ((analogChannelIdx < dfMaxValAnalogCount) && (analogState & asPerformRead)) analogValue[(unsigned int)(analogChannelIdx)] = rawRead;

#ifdef useAnalogButtons
		if (analogChannelIdx == analogButtonChannel) // if analog button channel was just read
		{

			for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
			{

				if (rawRead >= pgm_read_word(&analogButtonThreshold[(unsigned int)(x)]))
				{

					thisAnalogButtonPressed = pgm_read_byte(&analogTranslate[(unsigned int)(x)]);
					break;

				}

			}

			if (lastAnalogButtonPressed != thisAnalogButtonPressed)
			{

				buttonCount = buttonDelay;
				lastAnalogButtonPressed = thisAnalogButtonPressed;

			}

		}

#endif
#ifdef useChryslerMAPCorrection
		if (analogChannelIdx == 0) calculatePressure(rawRead, vBaroPressureIdx);
		if (analogChannelIdx == 1) calculatePressure(rawRead, vMAPpressureIdx);

#endif // useChryslerMAPCorrection
	}

	analogState ^= asReadInternalGround; // flip-flop between reading internal ground and reading analog channel

#else
	ADMUX = (1 << REFS0); // set ADC voltage reference to AVCC, and right-adjust the ADC reading

#endif
#ifdef useDebugReadings
	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisDebugCycleCount = timer0_overflow_count + (unsigned long)(a);

	if (injectorDebugCount) injectorDebugCount--;
	else
	{

		if (debugFlags & debugInjectorFlag)
		{

#ifdef trackIdleEOCdata
			if (dirty & dirtyGoodVSS)
			{

				tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjCycleIdx, findCycleLength(lastDebugCycleCount, thisDebugCycleCount));
				tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjOpenCycleIdx, 1300ul);
				tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++;

			}
			else
			{

				tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvInjCycleIdx, findCycleLength(lastDebugCycleCount, thisDebugCycleCount));
				tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvInjOpenCycleIdx, 1300ul);
				tripArray[(unsigned int)(rawEOCidleTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++;

			}
#else
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjCycleIdx, findCycleLength(lastDebugCycleCount, thisDebugCycleCount));
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvInjOpenCycleIdx, 1300ul);
			tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvInjPulseIdx)]++;
#endif

			dirty &= ~dirtyInjOpenRead; // signal that the injector pulse has been read
			dirty |= dirtyGoodInj;

		}

		lastDebugCycleCount = thisDebugCycleCount;
		injectorDebugCount = 480;

	}

	if (VSSdebugCount) VSSdebugCount--;
	else
	{

		if (debugFlags & debugVSSflag) VSScount = 1;
		VSSdebugCount = 44;

	}

#endif
	if (analogState & asKeepInterrupt) ADCSRA |= (1 << ADSC); // if one or more program modes need the ADC interrupt to keep going, start another ADC conversion
	else shutdownADC(); // otherwise, shut down ADC interrupt and ADC module

}

// this is meant to be executed only when interrupts are already disabled
// main program routines must disable interrupts prior to calling
//
void initADC(uint8_t adcModeFlag)
{

#ifdef useTinkerkitLCDmodule
	if (PRR0 & (1 << PRADC))
	{

		PRR0 &= ~(1 << PRADC); // turn on ADC module
#else
#ifdef useArduinoMega2560
	if (PRR0 & (1 << PRADC))
	{

		PRR0 &= ~(1 << PRADC); // turn on ADC module
#else
	if (PRR & (1 << PRADC))
	{

		PRR &= ~(1 << PRADC); // turn on ADC module
#endif
#endif
		ADCSRA &= ~(1 << ADATE); // disable ADC free-running mode

		 // enable ADC, enable ADC interrupt, clear any pending ADC interrupt, and set frequency to 1/128 of system timer
		ADCSRA |= ((1 << ADEN) | (1 << ADSC) | (1 << ADIF) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

	}

	analogState |= adcModeFlag;

}

void shutdownADC(void)
{

	analogState &= ~(asKeepInterrupt); // disable all analog program modes

	ADCSRA &= ~((1 << ADEN) | (1 << ADIE)); // disable ADC, disable ADC interrupt
#ifdef useTinkerkitLCDmodule
	PRR0 |= (1 << PRADC); // turn off ADC module
#else
	#ifdef useArduinoMega2560
		PRR0 |= (1 << PRADC); // turn off ADC module
	#else
		PRR |= (1 << PRADC); // turn off ADC module
	#endif
#endif

}

#endif
#ifdef useChryslerMAPCorrection
unsigned long analogFloor[2];
unsigned long analogSlope[2];
unsigned long analogOffset[2];

void calculatePressure(unsigned int analogReading, uint8_t pressureIdx)
{

//	static unsigned int MAPfilter[2] = { 0, 0 };
	unsigned long wp;

//	// perform 2nd stage IIR filter operation
//	MAPfilter[(unsigned int)(pressureIdx)] = MAPfilter[(unsigned int)(pressureIdx)] + 7 * analogReading; // first order IIR filter - filt = filt + 7/8 * (reading - filt)
//	MAPfilter[(unsigned int)(pressureIdx)] >>= 3;

	if (analogReading < analogFloor[(unsigned int)(pressureIdx)]) volatileVariables[(unsigned int)(pressureIdx)] = 0;
	else
	{

		wp = (unsigned long)(analogReading - analogFloor[(unsigned int)(pressureIdx - vMAPpressureIdx)]);
		wp *= analogSlope[(unsigned int)(pressureIdx - vMAPpressureIdx)];
		wp >>= 10;
		volatileVariables[(unsigned int)(pressureIdx)] = wp + analogOffset[(unsigned int)(pressureIdx - vMAPpressureIdx)];

	}

}

void calculateMAPcorrection(void)
{

	static unsigned long wp;

	// calculate differential pressure seen across the fuel injector
	wp = volatileVariables[(unsigned int)(vFuelPressureIdx)] + volatileVariables[(unsigned int)(vBaroPressureIdx)] - volatileVariables[(unsigned int)(vMAPpressureIdx)];
	volatileVariables[(unsigned int)(vInjPressureIdx)] = wp;

	// to get fuel pressure ratio, multiply differential pressure by denominator factor (1 << 12), then divide by fuel system pressure
	wp <<= 12;
	wp /= volatileVariables[(unsigned int)(vFuelPressureIdx)];

	// calculate square root of fuel pressure ratio
	volatileVariables[(unsigned int)(vInjCorrectionIdx)] = (unsigned long)(iSqrt((unsigned int)(wp)));

}

#endif // useChryslerMAPCorrection
#ifdef useIsqrt
unsigned int iSqrt(unsigned int n)
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
void updateVSS(unsigned long thisVSStime)
{

	unsigned long cycleLength;

	if (dirty & dirtyGoodVSS)
	{

		cycleLength = findCycleLength(lastVSStime, thisVSStime);

#ifdef trackIdleEOCdata
		if (dirty & dirtyGoodInj) // if a valid fuel injector event has been read, vehicle is in normal cruise mode
		{

			tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // update the VSS pulse count
			tripArray[(unsigned int)(rawTripIdx)].add64s(rvVSScycleIdx, cycleLength); // add to VSS cycle accumulator

		}
		else // vehicle is in EOC mode
		{

			tripArray[(unsigned int)(rawEOCidleTripIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // update the VSS pulse count
			tripArray[(unsigned int)(rawEOCidleTripIdx)].add64s(rvVSScycleIdx, cycleLength); // add to VSS cycle accumulator

		}
#else // trackIdleEOCdata
		tripArray[(unsigned int)(rawTripIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // update the VSS pulse count
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

		}

		if (accelerationFlags & accelTestActive) // if accel test function is active
		{

			if (volatileVariables[(unsigned int)(vDragInstantSpeedIdx)] > cycleLength) volatileVariables[(unsigned int)(vDragInstantSpeedIdx)] = cycleLength; // if this vehicle speed is larger than previous vehicle speed

			tripArray[(unsigned int)(dragRawDistanceIdx)].collectedData[(unsigned int)(rvVSSpulseIdx)]++; // update the accel test distance measurement VSS pulse count
			tripArray[(unsigned int)(dragRawDistanceIdx)].add64s(rvVSScycleIdx, cycleLength); // update the accel test distance measurement VSS cycle accumulator

			if (accelerationFlags & accelTestDistance)
			{

				if (volatileVariables[(unsigned int)(vAccelDistanceValueIdx)]) volatileVariables[(unsigned int)(vAccelDistanceValueIdx)]--; // count down drag distance setpoint in VSS pulses
				else accelerationFlags &= ~accelTestDistance; // otherwise, mark drag function distance measurement as complete

			}

			if ((accelerationFlags & accelTestHalfSpeed) && (cycleLength < volatileVariables[(unsigned int)(vAccelHalfPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripArray[(unsigned int)(dragRawHalfSpeedIdx)].transfer(tripArray[(unsigned int)(dragRawDistanceIdx)]);
				accelerationFlags &= ~accelTestHalfSpeed; // mark drag function speed measurement as complete

			}

			if ((accelerationFlags & accelTestFullSpeed) && (cycleLength < volatileVariables[(unsigned int)(vAccelFullPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripArray[(unsigned int)(dragRawFullSpeedIdx)].transfer(tripArray[(unsigned int)(dragRawDistanceIdx)]);
				accelerationFlags &= ~accelTestFullSpeed; // mark drag function speed measurement as complete

			}

			if ((accelerationFlags & accelTestMeasurementFlags) == 0) // if all drag measurements have completed, mark drag function as complete
			{

				accelerationFlags &= ~accelTestActive; // switch status from 'active' to 'finished'
				accelerationFlags |= accelTestFinished;

			}

		}

#endif // useDragRaceFunction
	}

	dirty |= dirtyGoodVSS; // annotate that a valid VSS pulse has been read
	lastVSStime = thisVSStime;

}

unsigned long findCycleLength(unsigned long lastCycle, unsigned long thisCycle)
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

void delay0(unsigned int ms)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	timerDelayCount = ms; // request a set number of timer tick delays per millisecond
	timerCommand |= tcDoDelay; // signal request to timer

	SREG = oldSREG; // restore interrupt flag status

	while (timerCommand & tcDoDelay) idleProcess(); // wait for delay timeout

}

#ifdef useBuffering
void ringBuffer::init(ringBufferVariable &bfr, volatile uint8_t * storage, deviceCallFunc devCallPtr)
{

	bfr.data = storage;
	bfr.size = sizeof(storage) / sizeof(storage[0]);
	bfr.start = 0;
	bfr.end = 0;
	bfr.status = bufferIsEmpty;
	bfr.ringBufferDeviceCall = devCallPtr;

}

void ringBuffer::updateStatus(ringBufferVariable &bfr, uint8_t clearFlag, uint8_t setFlag)
{

	if (bfr.start == bfr.size) bfr.start = 0;
	if (bfr.end == bfr.size) bfr.end = 0;
	bfr.status &= ~clearFlag;
	if (bfr.start == bfr.end) bfr.status |= setFlag;

}

void ringBuffer::push(ringBufferVariable &bfr, uint8_t value)
{

	uint8_t oldSREG;

	while (bfr.status & bufferIsFull) idleProcess(); // wait for calling routine's buffer to become not full

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (bfr.status & bufferIsFull) bfr.ringBufferDeviceCall(rbvFullEvent, 0);
	else
	{

		if (bfr.status & bufferIsEmpty) bfr.ringBufferDeviceCall(rbvNoLongerEmptyEvent, 0);
		bfr.data[(unsigned int)(bfr.start++)] = value; // save a buffered character

		updateStatus(bfr,bufferIsEmpty ,bufferIsFull);

	}

	SREG = oldSREG; // restore interrupt flag status

}

void ringBuffer::pull(ringBufferVariable &bfr)
{

	uint8_t outputLength;
	uint8_t x;

	if (bfr.status & bufferIsEmpty) bfr.ringBufferDeviceCall(rbvEmptyEvent, 0);
	else
	{

		if (bfr.status & bufferIsFull) bfr.ringBufferDeviceCall(rbvNoLongerFullEvent, 0);

		outputLength = bfr.ringBufferDeviceCall(rbvReturnOutputLength, 0);

		bfr.ringBufferDeviceCall(rbvOpenOutput, 0);

		while (outputLength)
		{

			x = bfr.ringBufferDeviceCall(rbvProcessCharacter, bfr.data[(unsigned int)(bfr.end++)]); // get a buffered character, update end pointer as required

			if (x) outputLength--;
			else outputLength = 0;

			updateStatus(bfr, bufferIsFull, bufferIsEmpty);
			if (bfr.status & bufferIsEmpty) outputLength = 0;

		}

		bfr.ringBufferDeviceCall(rbvCloseOutput, 0);

	}

}

void ringBuffer::flush(ringBufferVariable &bfr)
{

	while ((bfr.status & bufferIsEmpty) == 0) idleProcess(); // wait for calling routine's buffer to become empty

	while (bfr.ringBufferDeviceCall(rbvTestIfDone, 0)) idleProcess(); // wait for underlying hardware to empty out

}

#endif // useBuffering
#ifdef useTWIsupport
ISR( TWI_vect )
{

	uint8_t twiStatus;

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

void TWI::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useArduinoMega2560
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#else // useArduinoMega2560
	PRR &= ~(1 << PRTWI); // turn on TWI module
	PORTC |= ((1 << PORTC5) | (1 << PORTC4)); // enable port C TWI pin pullup resistors
#endif // useArduinoMega2560
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // set TWI prescaler to 1
	TWBR = (uint8_t)(((unsigned int)(systemProcessorSpeed * 1000 / twiFrequency) - 16) / 2); // set TWI frequency
	TWCR &= ~((1 << TWINT) | (1 << TWSTA) | (1 << TWSTO)); // clear TWI interrupt flag, start condition, stop condition
	TWCR |= ((1 << TWIE) | (1 << TWEA) | (1 << TWEN)); // enable TWI module, acks, and interrupt

	twiStatusFlags = twiSendStop; // initialize TWI state to ready, default send stop, and no repeated start in progress

	onSlaveTransmit = 0;
	onSlaveReceive = 0;

	SREG = oldSREG; // restore interrupt flag status

}

void TWI::shutdown(void)
{

	while (twiStatusFlags & twiTxRxStatusMain); // wait for all TWI transactions to complete

	TWCR &= ~((1 << TWEN) | (1 << TWIE)); // disable TWI module and interrupt
#ifdef useArduinoMega2560
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#else // useArduinoMega2560
	PORTC &= ~((1 << PORTC5) | (1 << PORTC4)); // disable port C TWI pin pullup resistors
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D)); // disable digital input on port C TWI pins
	PRR |= (1 << PRTWI); // turn off TWI module
#endif // useArduinoMega2560

	twiStatusFlags = 0;

}

void TWI::setAddress(uint8_t address)
{

	TWAR = address << 1; // set twi slave address (skip over TWGCE bit)

}

uint8_t TWI::transmit(const uint8_t * data, uint8_t length)
{

	if(twiSlaveTxBufferSize < length) return 1; // ensure data will fit into buffer

	if((twiStatusFlags & twiSlaveTransmitter) == 0) return 2; // ensure we are currently a slave transmitter

	twiSlaveTxBufferLen = length; // set length and copy data into tx buffer

	for(uint8_t x = 0; x < length; ++x) twiSlaveTxBufferData[(unsigned int)(x)] = data[(unsigned int)(x)];

	return 0;

}

uint8_t TWI::readDevice(uint8_t address, uint8_t * data, uint8_t length, uint8_t sendStop)
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

uint8_t TWI::writeByte(uint8_t data)
{

	if(twiMasterBufferLen < twiMasterBufferSize)
	{

		twiMasterBufferData[(unsigned int)(twiMasterBufferLen++)] = data;
		return 0;

	}
	else
	{

		twiStatusFlags &= ~(twiTxRxStatus); // free up TWI for main program use
		return 1; // signal buffer overflow

	}

}

void TWI::commitWrite(void)
{

	commitWrite(false);

}

void TWI::commitWrite(uint8_t waitFlag)
{

	if ((twiStatusFlags & twiTxRxStatus) == twiMasterTransmitter) // if in master transmitter state
	{

		if (twiMasterBufferLen) commitMasterDevice(waitFlag); // if there is data in the master transmitter butter, go initiate TWI master transmitter job
		else twiStatusFlags &= ~(twiTxRxStatus); // otherwise, free up TWI for main program use

	}

}

void TWI::initMasterDevice(uint8_t address, uint8_t writeFlag, uint8_t sendStop, uint8_t interruptFlag)
{

	switch (interruptFlag)
	{

		case 0:
			while (twiStatusFlags & twiTxRxStatusMain) idleProcess(); // wait for TWI to become available
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

}

uint8_t TWI::commitMasterDevice(uint8_t waitFlag)
{

	// if we're in a repeated start, then we've already sent the START
	// in the ISR. Don't do it again.
	//
	// if we're in the repeated start state, then we've already sent the start,
	// (@@@ we hope), and the TWI statemachine is just waiting for the address byte.
	// We need to remove ourselves from the repeated start state before we enable interrupts,
	// since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
	// up. Also, don't enable the START interrupt. There may be one pending from the
	// repeated start that we sent outselves, and that would really confuse things.

	if (twiStatusFlags & twiRepeatStart) // if in repeated start state
	{

		twiStatusFlags &= ~(twiRepeatStart); // clear repeated start state
		TWDR = twiSlaveAddress; // set data address to slave address
		TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // enable INT but do not send start condition

	}
	else TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTA)); // enable INT and send start condition

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

void TWI::reply(uint8_t ack)
{

	if (ack) TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA)); // send ACK condition
	else TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWINT)); // send NEGATIVE ACK condition

}

void TWI::stopOrRepeatStart(void)
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

void TWI::hardStop(void)
{

	TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT) | (1 << TWSTO)); // send stop condition

	while (TWCR & (1 << TWSTO)); // wait for stop condition to be exectued on bus

	twiStatusFlags &= ~(twiTxRxStatus); // set TWI state to ready

}

void TWI::releaseBus(void)
{

	TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT)); // release two-wire bus
	twiStatusFlags &= ~(twiTxRxStatus); // set TWI state to ready

}

#endif // useTWIsupport
#ifdef useSerial0Port
void serial0::init(void)
{

#ifdef useArduinoMega2560
	if (PRR0 & (1 << PRUSART0))
	{

		PRR0 &= ~(1 << PRUSART0); // turn on USART transmitter
#else // useArduinoMega2560
	if (PRR & (1 << PRUSART0))
	{

		PRR &= ~(1 << PRUSART0); // turn on USART transmitter
#endif // useArduinoMega2560
		UBRR0H = (uint8_t)(myubbr0 >> 8); // set serial uart baud rate
		UBRR0L = (uint8_t)(myubbr0);
		UCSR0A &= ~((1 << TXC0) | (1 << UDRE0) | (1 << U2X0));
		UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00)); // set for 8 data bits, no parity, and 1 stop bit
		UCSR0B |= (1 << TXEN0); // enable USART1 transmitter

	}

}

void serial0::shutdown(void)
{

#ifdef useArduinoMega2560
	PRR0 |= (1 << PRUSART0); // shut off USART0 for power reduction
#else
	PRR |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif

}

void serial0::pushCharacter(uint8_t chr)
{

#ifdef useBufferedSerial0Port
	ringBuffer::push(serial0Buffer, chr);
#else // useBufferedSerial0Port
	init(); // if USART0 transmitter is turned off, turn it on

	while ((UCSR0A & (1 << UDRE0)) == 0); // wait until USART0 data buffer is empty

	UCSR0A &= ~(1 << TXC0);
	UDR0 = chr; //send the data
#endif // useBufferedSerial0Port

}

#ifdef useBufferedSerial0Port
ISR( USART0_tx_vect )
{

	ringBuffer::pull(serial0Buffer); // send a buffered character to the serial hardware

}

ISR( USART0_sd_vect )
{

	UCSR0B &= ~((1 << TXEN0) | (1 << TXCIE0)); // Disable transmitter and interrupts

}

uint8_t serial0::bufferDevice(uint8_t cmd, uint8_t data)
{

	uint8_t returnCode = 0;

	switch (cmd)
	{

		case (rbvEmptyEvent):
			UCSR0B &= ~(1 << UDRIE0); // Disable data register empty interrupt
			break;

		case (rbvNoLongerEmptyEvent):
			init(); // if USART0 transmitter is turned off, turn it on
			UCSR0B |= ((1 << TXEN0) | (1 << UDRIE0) | (1 << TXCIE0)); // Enable transmitter and interrupts
			break;

		case (rbvNoLongerFullEvent):
		case (rbvFullEvent):
		case (rbvOpenOutput):
		case (rbvCloseOutput):
			break;

		case (rbvReturnOutputLength):
			returnCode = 1; // return an output buffer length of 1 byte
			break;

		case (rbvProcessCharacter):
			UDR0 = data; // Transmit a byte
			returnCode = 1; // signal to process the next byte
			break;

		case (rbvTestIfDone):
			returnCode = ((UCSR0B & (1 << TXEN0)) != 0);
			break;

		default:
			returnCode = 255;
			break;

	}

	return returnCode;

}

#endif // useBufferedSerial0Port
#endif // useSerial0Port
#ifdef useSerial1Port
void serial1::init(void)
{

	if (PRR1 & (1 << PRUSART1))
	{

		PRR1 &= ~(1 << PRUSART1); // turn on USART1
		UBRR1H = (uint8_t)(myubbr1 >> 8); // set serial uart baud rate
		UBRR1L = (uint8_t)(myubbr1);
		UCSR1A &= ~((1 << TXC1) | (1 << UDRE1) | (1 << U2X1));
		UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10)); // set for 8 data bits, no parity, and 1 stop bit
		UCSR1B |= (1 << TXEN1); // enable USART1 transmitter

	}

}

void serial1::shutdown(void)
{

	PRR1 |= (1 << PRUSART1); // shut off USART1 for power reduction

}

void serial1::pushCharacter(uint8_t chr)
{

#ifdef useBufferedSerial1Port
	ringBuffer::push(serial1Buffer, chr);
#else // useBufferedSerial1Port
	init(); // if USART1 transmitter is turned off, turn it on

	while ((UCSR1A & (1 << UDRE1)) == 0); // wait until USART1 data buffer buffer is empty

	UCSR1A &= ~(1 << TXC1);
	UDR1 = chr; //send the data
#endif // useBufferedSerial1Port

}

#ifdef useBufferedSerial1Port
ISR( USART1_tx_vect ) // called whenever USART data buffer empties
{

	ringBuffer::pull(serial1Buffer); // send a buffered character to the serial hardware

}

ISR( USART1_sd_vect ) // only called when USART data buffer is empty and transmission of USART serial frame is complete
{

	UCSR1B = ~((1 << TXEN1) | (1 << TXCIE1)); // Disable transmitter and interrupts

}

uint8_t serial1::bufferDevice(uint8_t cmd, uint8_t data)
{

	uint8_t returnCode = 0;

	switch (cmd)
	{

		case (rbvEmptyEvent):
			UCSR1B &= ~(1 << UDRIE1); // Disable data register empty interrupt
			break;

		case (rbvNoLongerEmptyEvent):
			init(); // if USART1 transmitter is turned off, turn it on
			UCSR1B |= ((1 << TXEN1) | (1 << UDRIE1) | (1 << TXCIE1)); // Enable transmitter and interrupts
			break;

		case (rbvNoLongerFullEvent):
		case (rbvFullEvent):
		case (rbvOpenOutput):
		case (rbvCloseOutput):
			break;

		case (rbvReturnOutputLength):
			returnCode = 1; // return an output buffer length of 1 byte
			break;

		case (rbvProcessCharacter):
			UDR0 = data; // Transmit a byte
			returnCode = 1; // signal to process the next byte
			break;

		case (rbvTestIfDone):
			returnCode = ((UCSR1B & (1 << TXEN1)) != 0);
			break;

		default:
			returnCode = 255;
			break;

	}

	return returnCode;

}

#endif // useBufferedSerial1Port
#endif // useSerial1Port
#ifdef use4BitLCD
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
#ifdef useAnalogLCDdelay

const uint8_t lcdNullValue =				0b00000000;

const uint8_t lcdClearDisplay =				0b00000001;

const uint8_t lcdReturnHome =				0b00000010;

const uint8_t lcdEntryModeSet =				0b00000100;
const uint8_t lcdEMSincrement =				0b00000010;		// 1 = increment, 0 = decrement
const uint8_t lcdEMSsetDisplayShift =			0b00000001;		// 1 = display shift, 0 = no display shift

const uint8_t lcdDisplayControl =			0b00001000;
const uint8_t lcdDCdisplayShow =			0b00000100;		// 1 = enable display, 0 = disable display
const uint8_t lcdDCcursorControl =			0b00000010;		// 1 = cursor on, 0 = cursor off
const uint8_t lcdDCcursorBlinkControl =			0b00000001;		// 1 = cursor blink, 0 = cursor steady

const uint8_t lcdShift =				0b00010000;
const uint8_t lcdSdisplayShift =			0b00001000;		// 1 = shift display, 0 = cursor move
const uint8_t lcdSdirection =				0b00000100;		// 1 = move right, 0 = move left

const uint8_t lcdFunctionSet =				0b00100000;
const uint8_t lcdFSdataLength =				0b00010000;		// 1 = 8 bit data, 0 = 4 bit data
const uint8_t lcdFSnumberOfLines =			0b00001000;		// 1 = 2 lines, 0 = 1 line
const uint8_t lcdFScharacterFont =			0b00000100;		// 1 = 5x10 dot character font, 0 = 5x8 dot character font

const uint8_t lcdSetCGRAMaddress =			0b01000000;

const uint8_t lcdSetDDRAMaddress =			0b10000000;

const uint8_t lcdDataByte =		0b00001000;
const uint8_t lcdCommandByte =		0b00000000;
const uint8_t lcdSendByte =		0b00000100;
const uint8_t lcdDelay0015ms =		0x03;
const uint8_t lcdDelay4100us =		0x02;
const uint8_t lcdDelay0100us =		0x01;
const uint8_t lcdDelay0080us =		0x00;

uint8_t LCDgotoXYaddress;

#endif // useAnalogLCDdelay
void LCD::init(void)
{

	devLCD.charOut = LCD::writeData;
	devLCD.gotoXY = LCD::gotoXY;
	devLCD.xMax = 16;

#ifdef useParallaxSerialLCDmodule
#ifdef useBufferedLCD
	ringBuffer::init(LCDserialBuffer, LCDserialData, LCDserialPort::bufferDevice);

#else // useBufferedLCD
	LCDserialPort::init();			// initialize LCD serial device

#endif // useBufferedLCD
#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
#ifdef useBufferedLCD
	ringBuffer::init(lcdBuffer, LCDdata, LCD::bufferDevice);

#endif // useBufferedLCD
#endif // useAnalogLCDdelay
	hardwareInit();

#ifdef useBigNumberDisplay
	cgramMode = 0; // clear CGRAM font status for big number displays

#endif // useBigNumberDisplay
}

void LCD::hardwareInit(void)
{

#ifdef useLegacyLCD
#ifdef useTinkerkitLCDmodule
	// set OC1A to non-inverting mode for LCD contrast, OC1B to non-inverting mode for LCD brightness
	TCCR1A &= ~((1 << COM1A0) | (1 << COM1B0));
	TCCR1A |= ((1 << COM1A1) | (1 << COM1B1));

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdContrast | lcdBrightness);
	DDRD |= (lcdBit2 | lcdBit1);
	DDRE |= lcdEnable;
	DDRF |= (lcdRegisterSelect | lcdBit0 | lcdDirection);
	PORTF &= ~lcdDirection; // write a zero to this pin - MPGuino has no need to read anything from the LCD module

#else // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
	// set OC0A to non-inverting mode, for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode, for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// set direction to output LCD data on selected port A pins
	DDRA |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);

	// set direction to output LCD data on selected port B pins
	DDRB |= (lcdBrightness | lcdContrast);

#else // useArduinoMega2560
	// set OC0A to non-inverting mode, for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode, for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// set direction to output LCD data on selected port B pins
	DDRB |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBrightness);

	// set direction to output LCD data on selected port D pins
	DDRD |= (lcdBit0 | lcdContrast | lcdEnable | lcdRegisterSelect);

#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useLegacyLCD
#ifdef useParallaxSerialLCDmodule
	delay0(delay0005ms);			// wait for 5 ms
	LCDserialPort::pushCharacter(12);	// clear the LCD screen
#ifdef useBufferedLCD
	ringBuffer::flush(LCDserialBuffer);	// clear the LCD buffer to force the LCD screen to clear
#endif // useBufferedLCD
	delay0(delay0005ms);			// wait for another 5 ms
	LCDserialPort::pushCharacter(22);	// turn on display, no cursor, no blink

#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
#ifdef useTWILCD
	timerCommand |= tcBlockTWIsample; // disable TWI sampling as it interferes with TWI LCD output
	while (timerStatus & tsTWIsampleActive) idleProcess(); // wait for any in-progress TWI samples to finish

#endif // useTWILCD
	// the below 5 lines actually take up less space than using a PROGMEM table / loop to write these values
	writeNybble(lcdNullValue, lcdDelay0015ms); // wait for more than 15 msec
	writeNybble(lcdFunctionSet | lcdFSdataLength, lcdCommandByte | lcdSendByte | lcdDelay4100us); // send (B0011) to DB7-4, then wait for more than 4.1 ms
	writeNybble(lcdFunctionSet | lcdFSdataLength, lcdCommandByte | lcdSendByte | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(lcdFunctionSet | lcdFSdataLength, lcdCommandByte | lcdSendByte | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(lcdFunctionSet, lcdCommandByte | lcdSendByte | lcdDelay0100us); // send (B0010) to DB7-4 for 4 bit mode, then wait for more than 100 us

#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // useBufferedLCD
	// ready to use normal writeCommand() function now!
	writeCommand(lcdFunctionSet | lcdFSnumberOfLines); // 4-bit interface, 2 display lines, 5x8 font
	writeCommand(lcdDisplayControl | lcdDCdisplayShow); // display control:

	writeCommand(lcdClearDisplay); // clear display, set cursor position to zero
	writeNybble(lcdNullValue, lcdDelay0015ms); // wait for more than 15 msec for display clear

	LCDgotoXYaddress = lcdSetDDRAMaddress;

#ifdef useTWILCD
	timerCommand &= ~(tcBlockTWIsample); // re-enable TWI sampling

#endif // useTWILCD
#endif // useAnalogLCDdelay
	setBrightness(brightnessIdx);
#ifdef useLegacyLCD
	setContrast(EEPROM::readVal(pContrastIdx));
#endif // useLegacyLCD

}

void LCD::shutdown(void)
{

	setBrightness(0); // turn off LCD brightness
#ifdef useLegacyLCD
	setContrast(255); // turn off LCD contrast
#endif // useLegacyLCD
#ifdef useParallaxSerialLCDmodule
	LCDserialPort::pushCharacter(21); // turn off LCD display
#ifdef useBufferedLCD
	ringBuffer::flush(LCDserialBuffer); // flush LCD output buffer
#endif // useBufferedLCD

#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
	writeCommand(lcdDisplayControl); // turn off LCD display
#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer
#endif // useBufferedLCD

#endif // useAnalogLCDdelay

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::shutdown(); // shut down LCD serial port

#endif // useParallaxSerialLCDmodule
#ifdef useLegacyLCD
#ifdef useTinkerkitLCDmodule
	// set OC1A to disabled, OC1B to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0));

	// disable LCD pins
	DDRB &= ~(lcdBit3);
	DDRD &= ~(lcdBit2 | lcdBit1);
	DDRE &= ~(lcdEnable);
	DDRF &= ~(lcdRegisterSelect | lcdBit0 | lcdDirection);

	PORTB |= lcdContrast; // ensure LCD contrast is turned off
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#else // useTinkerkitLCDmodule
	#ifdef useArduinoMega2560
		// set OC0A to disabled
		TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

		// set OC1A to disabled
		TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

		// disable LCD pins
		DDRA &= ~(lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);

		PORTB |= lcdContrast; // ensure LCD contrast is turned off
		#ifdef useInvertedLegacyLCDbrightness
			PORTB |= lcdBrightness; // ensure LCD brightness is turned off
		#else // useInvertedLegacyLCDbrightness
			PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
		#endif // useInvertedLegacyLCDbrightness
	#else // useArduinoMega2560
		// set OC0A to disabled
		TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

		// set OC1A to disabled
		TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

		// disable LCD pins
		DDRB &= ~(lcdBit3 | lcdBit2 | lcdBit1);
		DDRD &= ~(lcdBit0 | lcdEnable | lcdRegisterSelect);

		PORTD |= lcdContrast;
		#ifdef useInvertedLegacyLCDbrightness
			PORTB |= lcdBrightness; // ensure LCD brightness is turned off
		#else // useInvertedLegacyLCDbrightness
			PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
		#endif // useInvertedLegacyLCDbrightness
	#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule

#endif // useLegacyLCD
}

void LCD::gotoXY(uint8_t x, uint8_t y) // x = 0..16, y = 0..1
{

#ifdef useParallaxSerialLCDmodule
	uint8_t dr = 128 + 20 * y + x;

	LCDserialPort::pushCharacter(dr);

#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
	LCDgotoXYaddress = lcdSetDDRAMaddress + 0x40 * y + x;

	writeCommand(LCDgotoXYaddress);

#endif // useAnalogLCDdelay
}

#ifdef useBigNumberDisplay
void LCD::loadCGRAMfont(const char * fontPtr)
{

	uint8_t s = pgm_read_byte(fontPtr++);

	if (cgramMode != s)
	{

#ifdef useAnalogLCDdelay
		writeCommand(lcdEntryModeSet | lcdEMSincrement); // entry mode set: increment automatically, no display shift

#endif // useAnalogLCDdelay
		cgramMode = s;
		s = pgm_read_byte(fontPtr++);

		for (uint8_t chr = 0; chr < s; chr++)
		{

#ifdef useParallaxSerialLCDmodule
			LCDserialPort::pushCharacter(248 + chr);

#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
			writeCommand(lcdSetCGRAMaddress + (chr << 3)); // set CGRAM

#endif // useAnalogLCDdelay
			for (uint8_t x = 0; x < 8; x++) writeData(pgm_read_byte(fontPtr++)); //write the character data to the character generator ram

		}

#ifdef useAnalogLCDdelay
		writeCommand(LCDgotoXYaddress); // set DDRAM to whatever the screen position was

#endif // useAnalogLCDdelay
	}

}

#endif // useBigNumberDisplay
void LCD::loadCGRAMcharacter(uint8_t chr, char * chrData)
{

	uint8_t b = chr & 0x07;

#ifdef useBigNumberDisplay
	cgramMode = 0; // reset CGRAM mode for big number displays

#endif
#ifdef useParallaxSerialLCDmodule
	LCDserialPort::pushCharacter(248 + b);

	for (uint8_t x = 0; x < 8; x++) LCDserialPort::pushCharacter(*chrData++); //write the character data to the character generator ram

#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
	writeCommand(lcdEntryModeSet | lcdEMSincrement); // entry mode set: increment automatically, no display shift
	writeCommand(lcdSetCGRAMaddress + (b << 3)); // set CGRAM

	for (uint8_t x = 0; x < 8; x++) writeData(*chrData++); //write the character data to the character generator ram

	writeCommand(LCDgotoXYaddress); // set DDRAM to whatever the screen position was

#endif // useAnalogLCDdelay
}

void LCD::setBrightness(uint8_t idx)
{

#ifdef useLegacyLCD
#ifdef useTinkerkitLCDmodule
	OCR1B = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#else // useTinkerkitLCDmodule
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#endif // useTinkerkitLCDmodule
#endif // useLegacyLCD
#ifdef useParallaxSerialLCDmodule
	if (idx) LCDserialPort::pushCharacter(17); // turn on LCD backlight
	else LCDserialPort::pushCharacter(18); // turn off LCD backlight
#endif // useParallaxSerialLCDmodule
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDmodule
	if (idx) setRGBcolor(EEPROM::readVal(pLCDcolorIdx)); // turn on LCD backlight
	else setRGBcolor(0); // turn off LCD backlight
#else // useAdafruitRGBLCDmodule
	if (idx) portLCD |= lcdBrightness; // turn on LCD backlight
	else portLCD &= ~(lcdBrightness); // turn off LCD backlight
#endif // useAdafruitRGBLCDmodule
#endif // useTWILCD

}

#ifdef useAnalogLCDdelay
#ifdef useLegacyLCD
void LCD::setContrast(uint8_t idx)
{

#ifdef useTinkerkitLCDmodule
	OCR1A = idx;
#else // useTinkerkitLCDmodule
	OCR0A = idx;
#endif // useTinkerkitLCDmodule

}

#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDmodule
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

void LCD::setRGBcolor(uint8_t idx)
{

	uint8_t RGBbitMask;

	RGBbitMask = pgm_read_byte(&RGBcolors[(unsigned int)(idx & 0x07)]); // read the color bitmask pattern to turn on LCD backlight

	portSwitches ^= RGBbitMask;
	portSwitches &= ~(lcdBrightnessRed | lcdBrightnessGreen);
	portSwitches ^= RGBbitMask;

	portLCD ^= RGBbitMask;
	portLCD &= ~(lcdBrightnessBlue);
	portLCD ^= RGBbitMask;

}

#endif // useAdafruitRGBLCDmodule
void LCD::writeCommand(uint8_t value)
{

	writeByte(value, lcdCommandByte | lcdDelay0100us);

}

#endif // useAnalogLCDdelay
void LCD::writeData(uint8_t value)
{

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::pushCharacter(value);
#endif // useParallaxSerialLCDmodule
#ifdef useAnalogLCDdelay
	writeByte(value, lcdDataByte | lcdDelay0080us);
#endif // useAnalogLCDdelay

}

#ifdef useAnalogLCDdelay
void LCD::writeByte(uint8_t value, uint8_t flags)
{

	flags |= lcdSendByte;

	writeNybble(value, flags);
	writeNybble(value << 4, flags);

}

const unsigned long delayLCD15000us = (unsigned long)(1ul + ((15000ul * (unsigned long)(systemProcessorSpeed)) / 16384ul)); // initial LCD delay for 4-bit initialization
const unsigned long delayLCD04100us = (unsigned long)(1ul + ((4100ul * (unsigned long)(systemProcessorSpeed)) / 16384ul)); // secondary LCD delay for 4-bit initialization
#ifdef useTWILCD
const unsigned long delayLCD00100us = 0; // final LCD delay for 4-bit initialization - at 100 kHz, TWI subsystem will take longer than 100 us just to pass an address and a data byte
const unsigned long delayLCD00080us = 0; // normal LCD character transmission delay - at 100 kHz, TWI subsystem will take longer than 80 us just to pass an address and a data byte
#else // useTWILCD
const unsigned long delayLCD00100us = (unsigned long)(1ul + ((100ul * (unsigned long)(systemProcessorSpeed)) / 16384ul)); // final LCD delay for 4-bit initialization
const unsigned long delayLCD00080us = (unsigned long)(1ul + ((80ul * (unsigned long)(systemProcessorSpeed)) / 16384ul)); // normal LCD character transmission delay
#endif // useTWILCD

const unsigned int lcdDelayTable[4] PROGMEM = { (unsigned int)(delayLCD00080us), (unsigned int)(delayLCD00100us), (unsigned int)(delayLCD04100us), (unsigned int)(delayLCD15000us) }; // LCD delay values, using ADC freewheeling and a divider of 128

#ifdef useTWILCD
void LCD::initTWItransmit(uint8_t interruptFlag)
{

	TWI::initWrite(lcdAddress, TWI_STOP, interruptFlag ); // open TWI as master transmitter
#ifdef useAdafruitRGBLCDmodule
	TWI::writeByte(MCP23017_GPIOx | MCP23017_PORTA); // specify bank A output latch register address
#endif // useAdafruitRGBLCDmodule

}

void LCD::writeTWIportLCD(void)
{

#ifdef useAdafruitRGBLCDmodule
	TWI::writeByte(portSwitches); // write switch port expander register
#endif // useAdafruitRGBLCDmodule
	TWI::writeByte(portLCD); // write LCD port expander register

}

void LCD::portLCDdirectTWIout(void)
{

	initTWItransmit(TWI_MAIN_PROGRAM);
	writeTWIportLCD();
	TWI::commitWrite();

}

#endif // useTWILCD
void LCD::writeNybble(uint8_t value, uint8_t flags)
{

#ifdef useBufferedLCD
	ringBuffer::push(lcdBuffer, (value & 0xF0) | (flags & 0x0F));
#else // useBufferedLCD
	uint8_t oldSREG;

#ifdef useTWILCD
	if (flags & lcdSendByte) initTWItransmit(TWI_MAIN_PROGRAM);

#endif // useTWILCD
	outputNybble((value & 0xF0) | (flags & 0x0F));

#ifdef useTWILCD
	if (flags & lcdSendByte) TWI::commitWrite(); // commit LCD port expander write

#endif // useTWILCD
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	initADC(asLCDdelayTimer);

	SREG = oldSREG; // restore interrupt flag status

	while (analogState & asLCDdelayTimer) idleProcess(); // wait for LCD timer delay to complete
#endif // useBufferedLCD

}

void LCD::outputNybble(uint8_t LCDchar)
{

	if (LCDchar & lcdSendByte)
	{

#ifdef useLegacyLCD
#ifdef useTinkerkitLCDmodule
		PORTF &= ~(lcdRegisterSelect | lcdDirection | lcdBit0);
		if (LCDchar & lcdDataByte) PORTF |= lcdRegisterSelect; // set nybble type
		if (LCDchar & 0b00010000) PORTF |= lcdBit0; // set bit 0

		PORTB &= ~(lcdBit3);
		if (LCDchar & 0b10000000) PORTB |= lcdBit3; // set bit 3

		PORTD &= ~(lcdBit2 | lcdBit1);
		if (LCDchar & 0b01000000) PORTD |= lcdBit2; // set bit 2
		if (LCDchar & 0b00100000) PORTD |= lcdBit1; // set bit 1

		PORTE |= lcdEnable; // set enable high
		PORTE &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#else // useTinkerkitLCDmodule
#ifdef useArduinoMega2560
		PORTA &= ~(lcdRegisterSelect | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) PORTA |= lcdRegisterSelect; // set nybble type
		if (LCDchar & 0b10000000) PORTA |= lcdBit3; // set bit 3
		if (LCDchar & 0b01000000) PORTA |= lcdBit2; // set bit 2
		if (LCDchar & 0b00100000) PORTA |= lcdBit1; // set bit 1
		if (LCDchar & 0b00010000) PORTA |= lcdBit0; // set bit 0

		PORTA |= lcdEnable; // set enable high
		PORTA &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#else // useArduinoMega2560
		PORTD &= ~(lcdRegisterSelect | lcdBit0);
		if (LCDchar & lcdDataByte) PORTD |= lcdRegisterSelect; // set nybble type
		if (LCDchar & 0b00010000) PORTD |= lcdBit0; // set bit 0

		PORTB &= ~(lcdBit3 | lcdBit2 | lcdBit1);
		if (LCDchar & 0b10000000) PORTB |= lcdBit3; // set bit 3
		if (LCDchar & 0b01000000) PORTB |= lcdBit2; // set bit 2
		if (LCDchar & 0b00100000) PORTB |= lcdBit1; // set bit 1

		PORTD |= lcdEnable; // set enable high
		PORTD &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#endif // useLegacyLCD
#ifdef useTWILCD
		portLCD &= ~(lcdRegisterSelect | lcdDirection | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) portLCD |= lcdRegisterSelect; // set nybble type
		if (LCDchar & 0b10000000) portLCD |= lcdBit3; // set bit 3
		if (LCDchar & 0b01000000) portLCD |= lcdBit2; // set bit 2
		if (LCDchar & 0b00100000) portLCD |= lcdBit1; // set bit 1
		if (LCDchar & 0b00010000) portLCD |= lcdBit0; // set bit 0

		portLCD |= lcdEnable; // set enable high

		writeTWIportLCD();

		portLCD &= ~(lcdEnable); // set enable low to 'tickle' enable bit (it will take TWI subsystem at least 22.5 us to write the update, which is larger than 50 ns required for enable 'tickling'

		writeTWIportLCD();

#endif // useTWILCD
	}

	lcdDelayCount = pgm_read_word(&lcdDelayTable[(unsigned int)(LCDchar & 0x03)]);

}

#ifdef useBufferedLCD
uint8_t LCD::bufferDevice(uint8_t cmd, uint8_t data)
{

	uint8_t returnCode = 0;

	switch (cmd)
	{

		case (rbvEmptyEvent):
			analogState &= ~(asLCDdelayTimer); // turn off LCD delay checking
			break;

		case (rbvNoLongerEmptyEvent):
			lcdDelayCount = 0;
			initADC(asLCDdelayTimer);
			break;

		case (rbvNoLongerFullEvent):
		case (rbvFullEvent):
			break;

		case (rbvOpenOutput):
#ifdef useTWILCD
			initTWItransmit(TWI_INTERRUPT);
#endif // useTWILCD
			break;

		case (rbvCloseOutput):
#ifdef useTWILCD
			TWI::commitWrite(); // commit LCD port expander write
#endif // useTWILCD
			break;

		case (rbvReturnOutputLength):
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDmodule
			returnCode = (twiMasterBufferSize / 2) - 1; // return output logical buffer length adjusted for GPIO register size and accounting for GPIO address byte transmission
#else // useAdafruitRGBLCDmodule
			returnCode = twiMasterBufferSize; // return an output logical buffer length equivalent to the master buffer byte length
#endif // useAdafruitRGBLCDmodule
#else // useTWILCD
			returnCode = 1; // return an output buffer length of 1 byte
#endif // useTWILCD
			break;

		case (rbvProcessCharacter):
			outputNybble(data); // if there is data to send out, go configure hardware ports
#ifdef useTWILCD
			if (data & lcdSendByte)
			{

				if (lcdDelayCount) returnCode = 0;
				else returnCode = 1; // signal to process the next byte

			}
			else returnCode = 0; // signal to end processing
#else // useTWILCD
			returnCode = 1; // signal to process the next byte
#endif // useTWILCD
			break;

		case (rbvTestIfDone):
#ifdef useTWILCD
			returnCode = ((twiStatusFlags & twiTxRxStatusMain) != 0);
#else // useTWILCD
			returnCode = ((analogState & asLCDdelayTimer) == 0);
#endif // useTWILCD
			break;

		default:
			returnCode = 255;
			break;

	}

	return returnCode;

}

#endif // useBufferedLCD
#endif // useAnalogLCDdelay
#endif // use4BitLCD
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
#else
		add64s(x, src.collectedData[(unsigned int)(x)]);
		add32(x + 1, src.collectedData[(unsigned int)(x + 1)]);
#endif

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
#else
	add32(calcIdx, v); // add to accumulator
	if (collectedData[(unsigned int)(calcIdx)] < v) collectedData[(unsigned int)(calcIdx + 1)]++; // handle any possible overflow
#endif

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
#else
	collectedData[(unsigned int)(calcIdx)] += v;
#endif

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
#else
	EEPROM::writeVal(t++, convertTime((uint32_t *)(&systemCycles)));
#endif

	for (uint8_t x = 0; x < tripListPtrLength; x++) EEPROM::writeVal(t++, collectedData[(unsigned int)(x)]);

	EEPROM::writeVal(t++, guinosig);

	return 1;

}

#endif
#ifdef useSWEET64multDiv
const uint8_t prgmDoMultiply[] PROGMEM = {
	instrLd, 0x41,							// load multiplier into register 4
	instrLd, 0x52,							// load multiplicand into register 5
	instrLdByte, 0x02, 0,						// zero out result (register 2)
	instrSkip, 9,							// skip forward to multiplier test

	instrShiftRight, 0x04,						// shift multiplier right by one bit
	instrSkipIfFlagClear, SWEET64carryFlag, 2,			// if multiplier bit was clear, don't add multiplicand to result
	instrAddYtoX, 0x25,						// add multiplicand to result
	instrShiftLeft, 0x05,						// shift multiplicand left by one bit
	instrTestRegister, 0x04,					// test multiplier register
	instrSkipIfFlagClear, SWEET64zeroFlag, 242,			// if multiplier is not zero, loop back

	instrDone							// exit to caller
};

const uint8_t prgmDoDivide[] PROGMEM = {
	instrLd, 0x41,							// initialize divisor register
	instrLd, 0x12,							// initialize remainder with dividend
	instrLdByte, 0x05, 1,						// load quotient bitmask with a 1
	instrTestRegister, 0x04,					// test divisor register
	instrSkipIfFlagClear, SWEET64zeroFlag, 11,			// if divisor is not zero, skip ahead

	instrLdByte, 0x02, 0,						// zero out quotient
	instrSubYfromX, 0x25,						// set overflow value in quotient
	instrLd, 0x12,							// set overflow value in remainder
	instrLdByte, 0x04, 0,						// zero out divisor
	instrDone,							// exit to caller

	instrTestRegister, 0x02,					// test dividend register
	instrSkipIfFlagSet, SWEET64zeroFlag, 247,			// if dividend is zero, go exit
	instrLdByte, 0x02, 0,						// initialize quotient (register 2)

	instrTestRegister, 0x04,					// test dividend register
	instrSkipIfFlagSet, SWEET64minusFlag, 7,			// if dividend is already fully shifted left, skip
	instrShiftLeft, 0x05,						// shift quotient bitmask left one bit
	instrShiftLeft, 0x04,						// shift divisor left one bit
	instrSkipIfFlagClear, SWEET64minusFlag, 249,			// if dividend is not fully shifted left, loop back

	instrSkipIfLTorE, 0x41, 2,					// if divisor is less than or equal to dividend, skip ahead
	instrSkip, 4,							// skip to divisor/quotient bitmask adjustment routine
	instrSubYfromX, 0x14,						// subtract divisor from dividend
	instrAddYtoX, 0x25,	       					// add quotient bitmask to quotient
	instrShiftRight, 0x05,						// shift quotient bitmask right by one bit
	instrSkipIfFlagSet, SWEET64carryFlag, 5,			// if quotient bitmask empty, skip ahead
	instrShiftRight, 0x04,						// shift divisor right by one bit
	instrSkipIfFlagClear, SWEET64zeroFlag, 237,			// go back to quotient bitmask test

	instrShiftLeft, 0x01,						// shift remainder left one bit to multiply by 2
	instrSkipIfLTorE, 0x41, 5,					// if divisor is less than or equal to remainder, skip ahead
	instrLdByte, 0x04, 0,						// zero out divisor
	instrSkip, 3,							// skip ahead to restore remainder

	instrLdByte, 0x04, 1,						// load divisor with a 1
	instrShiftRight, 0x01,						// shift remainder right one bit to restore it
	instrDone							// exit to caller
};

#endif
const uint8_t * const S64programList[] PROGMEM = {
	prgmFuelUsed
	,prgmFuelRate
	,prgmEngineRunTime
	,prgmTimeToEmpty
	,prgmDistance
	,prgmSpeed
	,prgmMotionTime
	,prgmFuelEcon
	,prgmRemainingFuel
	,prgmDistanceToEmpty
	,prgmEngineSpeed
	,prgmInjectorOpenTime
	,prgmInjectorTotalTime
	,prgmVSStotalTime
	,prgmInjectorPulseCount
	,prgmVSSpulseCount
#ifdef useFuelCost
	,prgmFuelCost
	,prgmFuelRateCost
	,prgmFuelCostPerDistance
	,prgmDistancePerFuelCost
	,prgmRemainingFuelCost
#endif // useFuelCost
#ifdef useAnalogRead
	,prgmVoltage
#endif
#ifdef useChryslerMAPCorrection
	,prgmPressure
	,prgmCorrF
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,prgmEstimateEnginePower
	,prgmAccelTestTime
	,prgmDragSpeed
#endif // useDragRaceFunction
	,prgmFindRemainingFuel
#ifdef useJSONoutput
	,prgmFindBingoFuel
	,prgmFindReserveFuel
#endif // useJSONoutput
#ifdef useExpansionPins
	,prgmFindExpansionPinSetting
#endif // useExpansionPins
	,prgmConvertToMicroSeconds
	,prgmFuelQuantityCalc
	,prgmDistanceToEmptyCalc
	,prgmTimeToEmptyCalc
	,prgmFormatToNumber
	,prgmFindTimerTicks
};

uint8_t SWEET64_fetchbyte(const uint8_t * &prgmPtr)
{

	uint8_t byt;

	byt = pgm_read_byte(prgmPtr++);

#ifdef useSWEET64trace
	if (SWEET64processorFlags & SWEET64traceFlag)
	{

		text::charOut(devDebugSerial, 32);
		text::hexByteOut(devDebugSerial, byt);

	}

#endif
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

	union union_64 * tu1 = (union union_64 *)(&s64reg[(uint16_t)(s64reg0)]);
	union union_64 * tu2 = (union union_64 *)(&s64reg[(uint16_t)(s64reg1)]);

	SWEET64processorFlags = 0;

#ifdef useSWEET64trace
	text::charOut(devDebugSerial, 13);
	text::charOut(devDebugSerial, 10);
	text::hexWordOut(devDebugSerial, (uint16_t)(sched));

#endif
	while (loopFlag)
	{

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugSerial, 13);
			text::hexWordOut(devDebugSerial, (unsigned int)(sched));
			text::charOut(devDebugSerial, 32);
			text::hexByteOut(devDebugSerial, tripIdx);
			text::charOut(devDebugSerial, 32);
			text::hexByteOut(devDebugSerial, spnt);

		}

#endif
		instr = SWEET64_fetchbyte(sched);

		if (instr & 0x40)
		{

			operand = SWEET64_fetchbyte(sched) - 0x11;

			tu1 = (union union_64 *)(&s64reg[(unsigned int)((operand >> 4) & 0x07)]);
			tu2 = (union union_64 *)(&s64reg[(unsigned int)(operand & 0x07)]);

		}

		if (instr & 0x80) operand = SWEET64_fetchbyte(sched);

		branchFlag = false;

		switch (instr)
		{

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

			case instrSkipIfMetricMode:
				branchFlag = ((metricFlag & metricMode) == metricMode);
				break;

			case instrSkipIfLTorE:
				branchFlag = ltOrEtest64(tu1, tu2);
				break;

			case instrSkipIfIndexBelow:
				branchFlag = (tripIdx < SWEET64_fetchbyte(sched));
				break;

			case instrSkip:
				branchFlag = true;
				break;

			case instrSkipIfFlagSet:
				branchFlag = ((SWEET64processorFlags & operand) == operand);
				operand = SWEET64_fetchbyte(sched);
				break;

			case instrSkipIfFlagClear:
				branchFlag = ((SWEET64processorFlags & operand) == 0);
				operand = SWEET64_fetchbyte(sched);
				break;

			case instrTestRegister:
				registerTest64(tu2);
				break;

			case instrSetFlag:
				SWEET64processorFlags |= operand;
				break;

			case instrClearFlag:
				SWEET64processorFlags &= ~operand;
				break;

			case instrShiftLeft:
				SWEET64processorFlags &= ~SWEET64carryFlag;
			case instrRollLeft:
				shl64(tu2);
				break;

			case instrShiftRight:
				SWEET64processorFlags &= ~SWEET64carryFlag;
			case instrRollRight:
				shr64(tu2);
				break;

			case instrAddToIndex:
				tripIdx += operand;
				break;

			case instrLd:
				copy64(tu1, tu2);
				break;

			case instrLdByteFromIndex:
				init64(tu2, tripIdx);
				break;

			case instrLdByteFromYindexed:
				operand = tu1->u8[(unsigned int)(tripIdx)];
			case instrLdByte:
				init64(tu2, operand);
				break;

			case instrLdTripVar:
				tripVarLoad64(tu2, SWEET64_fetchbyte(sched), operand);
				break;

			case instrLdTripVarIndexed:
				tripVarLoad64(tu2, tripIdx, operand);
				break;

			case instrLdNumer:
			case instrLdDenom:
				operand = pgm_read_byte(&convNumerIdx[(unsigned int)(tripIdx)]);
				if (metricFlag & metricMode) operand ^= 1;
				if (instr == instrLdDenom) operand ^= 1;
			case instrLdConst:
				init64(tu2, pgm_read_dword(&convNumbers[(unsigned int)(operand)]));
				break;

			case instrLdEEPROMindexed:
				operand += tripIdx;
				init64(tu2, EEPROM::readVal(operand));
				break;

			case instrLdEEPROMindirect:
				operand = pgm_read_byte(&convIdx[(unsigned int)(tripIdx)]);
			case instrLdEEPROM:
				init64(tu2, EEPROM::readVal(operand));
				break;

			case instrStEEPROMindirect:
				operand = pgm_read_byte(&convIdx[(unsigned int)(tripIdx)]);
			case instrStEEPROM:
				EEPROM::writeVal(operand, tu2->ul[0]);
				break;

			case instrLdVolatileIndexed:
				operand = tripIdx;
			case instrLdVolatile:
				init64(tu2, volatileVariables[(unsigned int)(operand)]);
				break;

			case instrStVolatile:
				volatileVariables[(unsigned int)(operand)] = tu2->ul[0];
				break;

			case instrStByteToByteIndex:
				tu2->u8[(unsigned int)(tripIdx)] = operand;
				break;

			case instrDoBCDadjust:
				instr = tu2->u8[6]; // fetch length

				if (instr < 4) tripIdx = instr - 1;
				else
				{

					tripIdx = 3;
					tu2->u8[0] = ((union union_64 *)(&s64reg[s64reg1]))->u8[0]; // workaround to support 10 decimal digits / 5 BCD bytes

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

			case instrLdIndex:
				tripIdx = operand;
				break;

			case instrSwap:
				swap64(tu1, tu2);
				break;

			case instrSubYfromX:
				SWEET64processorFlags &= ~SWEET64carryFlag;
			case instrSubYfromXwithCarry:
				sub64(tu1, tu2);
				break;

			case instrAddYtoX:
				SWEET64processorFlags &= ~SWEET64carryFlag;
			case instrAddYtoXwithCarry:
				add64(tu1, tu2);
				break;

#ifdef useWindowTripFilter
			case instrStTripVar:
				if (operand < rvVSScycleIdx) tripArray[(unsigned int)(SWEET64_fetchbyte(sched))].collectedData[(unsigned int)(operand)] = tu2->ul[0];
				else copy64((union union_64 *)&tripArray[(unsigned int)(SWEET64_fetchbyte(sched))].collectedData[(unsigned int)(operand)], tu2);
				break;

#endif
			case instrMul2by1:
#ifdef useSWEET64multDiv
				prgmStack[(unsigned int)(spnt++)] = sched;
				if (spnt > 15) loopFlag = false;
				else sched = prgmDoMultiply;
#else
				mul64();
#endif
				break;

			case instrDiv2by1:
#ifdef useSWEET64multDiv
				prgmStack[(unsigned int)(spnt++)] = sched;
				if (spnt > 15) loopFlag = false;
				else sched = prgmDoDivide;
#else
				div64();
#endif
				break;

#ifdef useAnalogRead
			case instrLdVoltage:
				init64(tu2, analogValue[(unsigned int)(tripIdx)]);
				break;

#endif
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

			text::charOut(devDebugSerial, 32);
			text::hexByteOut(devDebugSerial, branchFlag);
			text::charOut(devDebugSerial, 13);

			for (uint8_t x = 0;x < 6; x++)
			{

				tu2 = (union union_64 *)(&s64reg[(uint16_t)(x)])

				text::charOut(devDebugSerial, 9);
				text::hexLWordOut(devDebugSerial, tu2->ull[1]);
				text::charOut(devDebugSerial, 13);
				text::charOut(devDebugSerial, 13);

			}

		}

#endif
	}

	return ((union union_64 *)(&s64reg[(uint16_t)(s64reg1)]))->ul[0];

}

void tripVarLoad64(union union_64 * an, uint8_t tripIdx, uint8_t dataIdx)
{

#ifdef useAssemblyLanguage
	if (dataIdx < rvVSScycleIdx)
	{

		asm volatile(
		"	st	%a0, %A1	\n"
		"	std	%a0+1, %B1	\n"
		"	std	%a0+2, %C1	\n"
		"	std	%a0+3, %D1	\n"
		"	std	%a0+4, __zero_reg__	\n"
		"	std	%a0+5, __zero_reg__	\n"
		"	std	%a0+6, __zero_reg__	\n"
		"	std	%a0+7, __zero_reg__	\n"
		: "+e" (an)
		: "r" (tripArray[(unsigned int)(tripIdx)].collectedData[(unsigned int)(dataIdx)])
		);

	}
	else
	{

		asm volatile(
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		"	ld 	__tmp_reg__, X+	\n"
		"	st 	Z+, __tmp_reg__	\n"
		:
		: "x" ((union union_64 *)&tripArray[(unsigned int)(tripIdx)].collectedData[(unsigned int)(dataIdx)]), "z" (an)
		);

	}
#else
	if (dataIdx < rvVSScycleIdx) init64(an, tripArray[(unsigned int)(tripIdx)].collectedData[(unsigned int)(dataIdx)]);
	else copy64(an, (union union_64 *)&tripArray[(unsigned int)(tripIdx)].collectedData[(unsigned int)(dataIdx)]);
#endif

}

void init64(union union_64 * an, unsigned long dWordL)
{

#ifdef useAssemblyLanguage
	asm volatile(
	"	st	%a0, %A1	\n"
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
#else
	an->ull = 0;
	an->ul[0] = dWordL;
#endif

}

void copy64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	asm volatile(
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	"	ld 	__tmp_reg__, X+	\n"
	"	st 	Z+, __tmp_reg__	\n"
	:
	: "x" (ann), "z" (an)
	);
#else
	for (uint8_t x = 0; x < 4; x++) an->ui[(unsigned int)(x)] = ann->ui[(unsigned int)(x)];
#endif

}

void swap64(union union_64 * an, union union_64 * ann) // swap ann and an
{

#ifdef useAssemblyLanguage
	unsigned int aing;

	asm volatile(
	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"

	"	ld 	%A2, X	\n"
	"	ld 	%B2, Z	\n"
	"	st 	Z+, %A2	\n"
	"	st 	X+, %B2	\n"
	: "+x" (ann), "+z" (an), "+r" (aing)
	:
	);
#else
	uint8_t b = 0;

	for (uint8_t x = 0; x < 8; x++)
	{

		b = ann->u8[(unsigned int)(x)];
		ann->u8[(unsigned int)(x)] = an->u8[(unsigned int)(x)];
		an->u8[(unsigned int)(x)] = b;

	}
#endif

}

void shr64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t n;
#ifdef useAssemblyLanguage

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);

	asm volatile(
	"	ror	%A1	\n"
	"	ldd	__tmp_reg__, %a0+7	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+7, __tmp_reg__	\n"
	"	mov	%A3, __tmp_reg__	\n"
	"	mov	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+6	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+6, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+5	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+5, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+4	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+4, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+3	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+2	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+2, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+1	\n"
	"	ror	__tmp_reg__	\n"
	"	std	%a0+1, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ld	__tmp_reg__, %a0	\n"
	"	ror	__tmp_reg__	\n"
	"	st	%a0, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	mov	%A1, __zero_reg__	\n"
	"	adc	%A1, __zero_reg__	\n"
	: "+e" (an), "+r" (c), "+r" (z), "+r" (n)
	:
	);
#else
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
#endif

	if (n & 0x80) SWEET64processorFlags |= SWEET64minusFlag;
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= SWEET64zeroFlag;

	if (c) SWEET64processorFlags |= SWEET64carryFlag;
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
	"	ror	%A1	\n"
	"	ld	__tmp_reg__, %a0	\n"
	"	rol	__tmp_reg__	\n"
	"	st	%a0, __tmp_reg__	\n"
	"	mov	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+1	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+1, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+2	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+2, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+3	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+4	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+4, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+5	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+5, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+6	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+6, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, %a0+7	\n"
	"	rol	__tmp_reg__	\n"
	"	std	%a0+7, __tmp_reg__	\n"
	"	mov	%A3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	mov	%A1, __zero_reg__	\n"
	"	adc	%A1, __zero_reg__	\n"
	: "+e" (an), "+r" (c), "+r" (z), "+r" (n)
	:
	);
#else
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
#endif

	if (n & 0x80) SWEET64processorFlags |= SWEET64minusFlag;
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= SWEET64zeroFlag;

	if (c) SWEET64processorFlags |= SWEET64carryFlag;
	else SWEET64processorFlags &= ~(SWEET64carryFlag);

}

void add64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	uint8_t c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);
	uint8_t z;
	uint8_t n;

	asm volatile (
	"	ror	%A1	\n"
	"	ld	__tmp_reg__, Y	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	st	Y, __tmp_reg__	\n"
	"	mov	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+1	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+1, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+2	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+2, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+3	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+4	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+4, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+5	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+5, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+6	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+6, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+7	\n"
	"	ld	%A1, X+	\n"
	"	adc	__tmp_reg__, %A1	\n"
	"	std	Y+7, __tmp_reg__	\n"
	"	mov	%A3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	adc	%A1, __zero_reg__	\n"
	: "+y" (an), "+r" (c), "+r" (z), "+r" (n)
	: "x" (ann)
	);

	if (n & 0x80) SWEET64processorFlags |= SWEET64minusFlag;
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= SWEET64zeroFlag;

	if (c) SWEET64processorFlags |= SWEET64carryFlag;
	else SWEET64processorFlags &= ~(SWEET64carryFlag);
#else
	add64(an, ann, 0);
#endif

}

void sub64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	uint8_t c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);
	uint8_t z;
	uint8_t n;

	asm volatile (
	"	ror	%A1	\n"
	"	ld	__tmp_reg__, Y	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	st	Y, __tmp_reg__	\n"
	"	mov	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+1	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+1, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+2	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+2, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+3	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+4	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+4, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+5	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+5, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+6	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+6, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	ldd	__tmp_reg__, Y+7	\n"
	"	ld	%A1, X+	\n"
	"	sbc	__tmp_reg__, %A1	\n"
	"	std	Y+7, __tmp_reg__	\n"
	"	mov	%A3, __tmp_reg__	\n"
	"	or	%A2, __tmp_reg__	\n"
	"	adc	%A1, __zero_reg__	\n"
	: "+y" (an), "+r" (c), "+r" (z), "+r" (n)
	: "x" (ann)
	);

	if (n & 0x80) SWEET64processorFlags |= SWEET64minusFlag;
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= SWEET64zeroFlag;

	if (c) SWEET64processorFlags |= SWEET64carryFlag;
	else SWEET64processorFlags &= ~(SWEET64carryFlag);
#else
	add64(an, ann, 1);
#endif

}

#ifndef useAssemblyLanguage
void add64(union union_64 * an, union union_64 * ann, uint8_t mode)	// target addend in an, source addend in ann, result to an
{

	uint8_t c = ((mode) ? 0xFF : 0x00);				// if mode == 0, then addition, otherwise subtraction
	uint8_t d;							// intermediate source addend byte
	uint8_t z = 0;
	unsigned int enn;						// intermediate result storage
	union union_16 * n = (union union_16 *)(&enn);			// byte-wise access into intermediate result storage

	d = c & 0x01;							// generate carry flag based on whether this operation is add or subtract
	d ^= ((SWEET64processorFlags & SWEET64carryFlag) ? 0x01 : 0x00);
	n->u8[1] = d;
	for (uint8_t x = 0; x < 8; x++)
	{

		d = c ^ ann->u8[(unsigned int)(x)];			// fetch (and complement, if subtract op) this operation's source addend byte
		n->ui = (unsigned int)(n->u8[1]);			// move previous operation's carry byte into position, and clear this operation's carry byte
		n->ui += (unsigned int)(an->u8[(unsigned int)(x)]);	// add previous operation's carry byte to this operation's target addend byte
		n->ui += (unsigned int)(d);				// add source addend byte to this operation's target addend byte (auto-generates carry byte for next operation)
		z |= n->u8[0];
		an->u8[(unsigned int)(x)] = n->u8[0];			// save this operation's target addend byte

	}

	d = (n->u8[1] ^ c) & 0x01;

	if (an->u8[7] & 0x80) SWEET64processorFlags |= SWEET64minusFlag;
	else SWEET64processorFlags &= ~SWEET64minusFlag;

	if (z) SWEET64processorFlags &= ~SWEET64zeroFlag;
	else SWEET64processorFlags |= SWEET64zeroFlag;

	if (d) SWEET64processorFlags |= SWEET64carryFlag;
	else SWEET64processorFlags &= ~SWEET64carryFlag;

}

#endif
#ifndef useSWEET64multDiv
void mul64(void)
{

	union union_64 * an = (union union_64 *)(&s64reg[s64reg1]);	// multiplier in an, result to an
	union union_64 * ann = (union union_64 *)(&s64reg[s64reg0]);	// multiplicand in ann

	union union_64 * multiplier = (union union_64 *)(&s64reg[3]);	// define multiplier term as register 4
	union union_64 * multiplicand = (union union_64 *)(&s64reg[4]);	// define multiplicand term as register 5

	copy64(multiplier, an);						// load multiplier into register 4
	copy64(multiplicand, ann);					// load multiplicand into register 5
	init64(an, 0);							// zero out result

	registerTest64(multiplier);

	while (!(SWEET64processorFlags & SWEET64zeroFlag))		// while multiplier is non-zero
	{

		SWEET64processorFlags &= ~SWEET64carryFlag;
		shr64(multiplier);					// shift multiplier right by one bit

		if (SWEET64processorFlags & SWEET64carryFlag)
		{

			SWEET64processorFlags &= ~SWEET64carryFlag;
			add64(an, multiplicand);			// if the low bit of multiplier is set, add multiplicand to result

		}

		SWEET64processorFlags &= ~SWEET64carryFlag;
		shl64(multiplicand);					// shift multiplicand left by one bit

		registerTest64(multiplier);

	}

}

// uses algorithm for non-restoring hardware division
void div64(void)
{

	union union_64 * an = (union union_64 *)(&s64reg[s64reg1]);	// dividend in an
	union union_64 * ann = (union union_64 *)(&s64reg[s64reg0]);	// divisor in ann

	union union_64 * divisor = (union union_64 *)(&s64reg[s64reg3]);
	uint8_t x;
	uint8_t y;
	uint8_t z;

	copy64(divisor, ann);						// copy ann value to divisor
	init64(ann, 0);							// zero out remainder

	registerTest64(divisor);

	if (SWEET64processorFlags & SWEET64zeroFlag)			// if divisor is zero, mark as overflow, then exit
	{

		SWEET64processorFlags |= SWEET64carryFlag;		// set carry bit
		sub64(ann, ann);					// subtract 1 from zeroed-out result to generate overflow value
		copy64(an, ann);					// copy overflow value to remainder

	}
	else
	{

		x = 64;							// start off with a dividend size of 64 bits
		SWEET64processorFlags |= SWEET64divisionFlag;		// mark

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

			SWEET64processorFlags &= ~SWEET64carryFlag;	// perform shift left of dividend/quotient, through remainder (A)
			shl64(an);					// shift Q left one bit
			shl64(ann);					// shift A left one bit

			SWEET64processorFlags &= ~SWEET64carryFlag;	// clear carry flag for impending operation
			if (SWEET64processorFlags & SWEET64divisionFlag) // if previous operation resulted in A >= 0
			{

				sub64(ann, divisor); // subtract M from A if previous A was greater than or equal to zero
				if ((SWEET64processorFlags & SWEET64carryFlag) && !(SWEET64processorFlags & SWEET64zeroFlag))
				{

					SWEET64processorFlags &= ~SWEET64divisionFlag; // if A < 0

				}
				else
				{

					an->u8[0] |= 0x01; // if A >= 0

				}

			}
			else // otherwise, previous operation resulted in A < 0
			{

				add64(ann, divisor); // add M to A if previous A was less than zero
				if ((SWEET64processorFlags & SWEET64carryFlag) || (SWEET64processorFlags & SWEET64zeroFlag))
				{

					SWEET64processorFlags |= SWEET64divisionFlag; // if A >= 0
					an->u8[0] |= 0x01;

				}

			}

			x--; // reduce dividend bit count by one

		}

		if (!(SWEET64processorFlags & SWEET64divisionFlag)) // if most recent operation resulted in A < 0
		{

			SWEET64processorFlags &= ~SWEET64carryFlag;
			add64(ann, divisor); // make A (remainder) positive by adding M to it

		}

		SWEET64processorFlags &= ~SWEET64carryFlag;
		shl64(ann);			// multiply remainder by 2 by shifting left one bit

		if (ltOrEtest64(divisor, ann)) init64(divisor, 1); // if remainder * 2 is not less than divisor
		else init64(divisor, 0);	// otherwise, store a zero

		shr64(ann);			// restore remainder

	}

}

#endif
void registerTest64(union union_64 * an)
{

	uint8_t z;
	uint8_t n;

#ifdef useAssemblyLanguage
	asm volatile(
	"	ld	%A0, Z	\n"					// load output byte with first byte of 64-bit number to be checked
	"	ldd	__tmp_reg__, Z+1	\n"			// load temp register with second byte of 64-bit number to be checked
	"	or	%A0, __tmp_reg__	\n"			// perform logical OR of output byte and temp register
	"	ldd	__tmp_reg__, Z+2	\n"			// load temp register with third byte of 64-bit number to be checked
	"	or	%A0, __tmp_reg__	\n"			// perform logical OR of output byte and temp register
	"	ldd	__tmp_reg__, Z+3	\n"			// load temp register with fourth byte of 64-bit number to be checked
	"	or	%A0, __tmp_reg__	\n"			// perform logical OR of output byte and temp register
	"	ldd	__tmp_reg__, Z+4	\n"			// load temp register with fifth byte of 64-bit number to be checked
	"	or	%A0, __tmp_reg__	\n"			// perform logical OR of output byte and temp register
	"	ldd	__tmp_reg__, Z+5	\n"			// load temp register with sixth byte of 64-bit number to be checked
	"	or	%A0, __tmp_reg__	\n"			// perform logical OR of output byte and temp register
	"	ldd	__tmp_reg__, Z+6	\n"			// load temp register with seventh byte of 64-bit number to be checked
	"	or	%A0, __tmp_reg__	\n"			// perform logical OR of output byte and temp register
	"	ldd	%A1, Z+7	\n"				// load temp register with eighth byte of 64-bit number to be checked
	"	or	%A0, %A1	\n"				// perform logical OR of output byte and temp register
	: "+r" (z), "+r" (n)
	: "z" (an)
	);
#else
	z = an->u8[0];

	for (uint8_t x = 1; x < 8; x++) z |= an->u8[(unsigned int)(x)];
	n = an->u8[7];
#endif

	if (n & 0x80) SWEET64processorFlags |= SWEET64minusFlag;
	else SWEET64processorFlags &= ~(SWEET64minusFlag);

	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
	else SWEET64processorFlags |= SWEET64zeroFlag;

}

uint8_t ltOrEtest64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	uint8_t b = 0x01;

	asm volatile (
	"	ld	__tmp_reg__, Y	\n"
	"	ld	%A0, Z	\n"
	"	sub	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+1	\n"
	"	ldd	%A0, Z+1	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+2	\n"
	"	ldd	%A0, Z+2	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+3	\n"
	"	ldd	%A0, Z+3	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+4	\n"
	"	ldd	%A0, Z+4	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+5	\n"
	"	ldd	%A0, Z+5	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+6	\n"
	"	ldd	%A0, Z+6	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	ldd	__tmp_reg__, Y+7	\n"
	"	ldd	%A0, Z+7	\n"
	"	sbc	__tmp_reg__, %A0 	\n"
	"	mov	%A0, __zero_reg__	\n"
	"	adc	%A0, __zero_reg__	\n"
	: "+r" (b)
	: "z" (an), "y" (ann)
	);

	return (b == 0);
#else
	uint8_t b = 1;

	for (uint8_t x = 7; x < 8; x--)
	{

		if (an->u8[(unsigned int)(x)] < ann->u8[(unsigned int)(x)]) break;
		else if (an->u8[(unsigned int)(x)] > ann->u8[(unsigned int)(x)])
		{

			b = 0;
			break;

		}

	}

	return (b == 1);
#endif

}

void initStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = 0; // end hold delay countdown
	timerCommand &= ~tcDisplayDelay; // cancel display delay

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
#endif

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = holdDelay; // start hold delay countdown
	timerCommand |= tcDisplayDelay; // make display delay active

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
				if ((dev.controlFlags & odvFlagForceOutput) || ((timerCommand & tcDisplayDelay) == 0))
#else // blankScreenOnMessage
				if ((dev.controlFlags & odvFlagForceOutput) || ((timerCommand & tcDisplayDelay) == 0) || (dev.yPos > 0))
#endif // blankScreenOnMessage
				{

					if (dev.controlFlags & odvFlagShootBlanks) chr = ' ';
					else chr &= 0x7F;

					if (dev.charOut) dev.charOut(chr);

				}

			}

			dev.xPos++;

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

	while (dev.xPos < dev.xMax) charOut(dev, ' ');

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

#define nextAllowedValue 0
const uint8_t lblCGRAMtimeInSecondsIdx =	nextAllowedValue;
const uint8_t lblCGRAMtimeInMicrosecondsIdx =	lblCGRAMtimeInSecondsIdx + 1;
const uint8_t lblCGRAMinjectorPulseCountIdx =	lblCGRAMtimeInMicrosecondsIdx + 1;
const uint8_t lblCGRAMengineSpeedIdx =		lblCGRAMinjectorPulseCountIdx + 1;
const uint8_t lblCGRAMfuelEconomyIdx =		lblCGRAMengineSpeedIdx + 1;
const uint8_t lblCGRAMdistanceIdx =		lblCGRAMfuelEconomyIdx + 2;
const uint8_t lblCGRAMspeedIdx =		lblCGRAMdistanceIdx + 2;
const uint8_t lblCGRAMfuelUsedIdx =		lblCGRAMspeedIdx + 2;
const uint8_t lblCGRAMfuelRateIdx =		lblCGRAMfuelUsedIdx + 2;
#define nextAllowedValue lblCGRAMfuelRateIdx + 2;
#ifdef useFuelCost
const uint8_t lblCGRAMfuelCostIdx =		nextAllowedValue;
const uint8_t lblCGRAMfuelRateCostIdx =		lblCGRAMfuelCostIdx + 1;
const uint8_t lblCGRAMfuelCostPerDistanceIdx =	lblCGRAMfuelRateCostIdx + 1;
const uint8_t lblCGRAMdistancePerFuelCostIdx =	lblCGRAMfuelCostPerDistanceIdx + 2;
#define nextAllowedValue lblCGRAMdistancePerFuelCostIdx + 2;
#endif // useFuelCost
#ifdef useAnalogRead
const uint8_t lblCGRAManalogReadIdx =	nextAllowedValue;
#define nextAllowedValue lblCGRAManalogReadIdx + 1;
#endif
#ifdef useChryslerMAPCorrection
const uint8_t lblCGRAMpressureIdx =		nextAllowedValue;
const uint8_t lblCGRAMcorrFactorIdx =		lblCGRAMpressureIdx + 2;
#define nextAllowedValue lblCGRAMcorrFactorIdx + 1;
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
const uint8_t lblCGRAMestimatedPowerIdx =	nextAllowedValue;
const uint8_t lblCGRAMtimeInTenthsSecIdx =	lblCGRAMestimatedPowerIdx + 2;
#define nextAllowedValue lblCGRAMtimeInTenthsSecIdx + 1;
#endif // useDragRaceFunction

// 0x80 is metric flag - metric label has offset of 1 from index
// 0x40 is relative/absolute trip flag - flag set means do not perform trip index translation
const uint8_t calcLabelIdx[(unsigned int)(dfMaxValDisplayCount)] PROGMEM = {
	lblCGRAMfuelUsedIdx | 0x80			// fuel used
	,lblCGRAMfuelRateIdx | 0x80			// fuel rate
	,lblCGRAMtimeInSecondsIdx			// engine run time
	,lblCGRAMtimeInSecondsIdx			// time to empty
	,lblCGRAMdistanceIdx | 0x80			// distance travelled
	,lblCGRAMspeedIdx | 0x80			// speed
	,lblCGRAMtimeInSecondsIdx			// time in motion
	,lblCGRAMfuelEconomyIdx | 0x80			// fuel economy
	,lblCGRAMfuelUsedIdx | 0x80			// remaining fuel
	,lblCGRAMdistanceIdx | 0x80			// remaining distance
	,lblCGRAMengineSpeedIdx				// engine speed
	,lblCGRAMtimeInMicrosecondsIdx			// fuel used, in microseconds
	,lblCGRAMtimeInMicrosecondsIdx			// engine run time, in microseconds
	,lblCGRAMtimeInMicrosecondsIdx			// time in motion, in microseconds
	,lblCGRAMinjectorPulseCountIdx			// injector pulses
	,lblCGRAMinjectorPulseCountIdx			// injector pulses
#ifdef useFuelCost
	,lblCGRAMfuelCostIdx				// fuel cost
	,lblCGRAMfuelRateCostIdx			// fuel rate cost
	,lblCGRAMfuelCostPerDistanceIdx | 0x80		// fuel cost per unit distance
	,lblCGRAMdistancePerFuelCostIdx | 0x80		// distance per unit fuel cost
	,lblCGRAMfuelCostIdx				// fuel cost remaining
#endif // useFuelCost
#ifdef useAnalogRead
	,lblCGRAManalogReadIdx | 0x40			// voltage
#endif
#ifdef useChryslerMAPCorrection
	,lblCGRAMpressureIdx | 0x80 | 0x40		// pressure
	,lblCGRAMcorrFactorIdx | 0x40			// correction factor
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,lblCGRAMestimatedPowerIdx | 0x80 | 0x40	// estimated engine power
	,lblCGRAMtimeInTenthsSecIdx			// time in tenths of second
#endif // useDragRaceFunction
};

const uint8_t calcDecimalPoints[] PROGMEM = {
	0	// time in seconds
	,0	// time in microseconds
	,0	// pulse count
	,0	// engine speed
	,2	// SAE fuel economy
	,2	// SI fuel economy
	,1	// SAE distance travelled
	,1	// SI distance travelled
	,1	// SAE speed
	,1	// SI speed
	,2	// SAE fuel quantity
	,2	// SI fuel quantity
	,2	// SAE fuel rate
	,2	// SI fuel rate
#ifdef useFuelCost
	,2	// fuel cost
	,2	// fuel cost rate
	,2	// SAE fuel cost per unit distance
	,2	// SI fuel cost per unit distance
	,1	// SAE distance per unit fuel cost
	,1	// SI distance per unit fuel cost
#endif // useFuelCost
#ifdef useAnalogRead
	,3	// voltage
#endif
#ifdef useChryslerMAPCorrection
	,2	// SAE pressure
	,2	// SI pressure
	,3	// correction factor
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,1	// SAE horsepower
	,1	// SI horsepower
	,1	// time in tenths of seconds
#endif // useDragRaceFunction
};

const uint8_t calcLabelText[] PROGMEM = {
	's'	// time in seconds
	,'u'	// time in microseconds
	,'p'	// pulse count
	,'t'	// engine speed
	,'E'	// SAE fuel economy
	,'E'	// SI fuel economy
	,'m'	// SAE distance travelled
	,'k'	// SI distance travelled
	,'S'	// SAE speed
	,'S'	// SI speed
	,'G'	// SAE fuel used
	,'L'	// SI fuel used
	,'g'	// SAE fuel rate
	,'l'	// SI rate
#ifdef useFuelCost
	,'$'	// fuel cost
	,'#'	// fuel rate cost
	,'C'	// SAE fuel cost per distance
	,'C'	// SAE fuel cost per distance
	,'D'	// SAE distance per fuel cost
	,'D'	// SI distance per fuel cost
#endif // useFuelCost
#ifdef useAnalogRead
	,'V'	// voltage
#endif
#ifdef useChryslerMAPCorrection
	,'P'	// SAE pressure
	,'P'	// SI pressure
	,'F'	// correction factor
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,'H'	// SAE horsepower
	,'W'	// SI horsepower
	,'s'	// time in tenths of seconds
#endif // useDragRaceFunction
};

const uint8_t calcLabelCGRAM[][16] PROGMEM = {
	// clock
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}

	// microseconds
	,{0b00010010, 0b00010010, 0b00010010, 0b00011110, 0b00010000, 0b00110000, 0b01000000, 0b01100000, 0b00001110, 0b00010000, 0b00001100, 0b00000010, 0b00011100, 0b00000000, 0b00000000, 0b00000000}

	// pulse count
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001000, 0b00011100, 0b00001000, 0b00001000}

	// revolutions per minute
	,{0b00001100, 0b00010000, 0b00010001, 0b00010010, 0b00000100, 0b00001000, 0b00000000, 0b00000000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011011, 0b00010101, 0b00010101}

	// miles per gallon
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010100, 0b00001100}

	// liters per 100 km
	,{0b00010000, 0b00010000, 0b00011001, 0b00000010, 0b00000100, 0b00001001, 0b00000001, 0b00000001, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111}

	// miles
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00000000, 0b00010000, 0b00010000}

	// kilometers
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000101, 0b00000110, 0b00000101, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00001010, 0b00010101, 0b00010101}

	// miles per hour
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00011100, 0b00010100, 0b00010100}

	// kilometers per hour
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// gallons
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000101, 0b00000011, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000001, 0b00000001, 0b00011001, 0b00011101}

	// liters
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000100, 0b00000100, 0b00000111, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}

	// gallons per hour
	,{0b00001100, 0b00010000, 0b00010100, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// liters per hour
	,{0b00010000, 0b00010000, 0b00010000, 0b00011100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

#ifdef useFuelCost
	// fuel cost
	,{0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000010, 0b00000101, 0b00000100, 0b00001110, 0b00000100, 0b00000100, 0b00000100, 0b00000100}

	// fuel cost rate
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// fuel cost per mile
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101}

	// fuel cost per kilometer
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010100, 0b00011000, 0b00010100}

	// mile per unit fuel cost
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100}

	// kilometer per unit fuel cost
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100}

#endif // useFuelCost
#ifdef useAnalogRead
	// voltage
	,{0b00010001, 0b00010001, 0b00010001, 0b00001010, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011010, 0b00011010, 0b00010001}

#endif
#ifdef useChryslerMAPCorrection
	// psi
	,{0b00001110, 0b00001001, 0b00001001, 0b00001110, 0b00001000, 0b00001000, 0b00001000, 0b00000000, 0b00001001, 0b00010101, 0b00010001, 0b00001001, 0b00000101, 0b00010101, 0b00001001, 0b00000000}

	// kPa
	,{0b00001000, 0b00001000, 0b00001001, 0b00001010, 0b00001100, 0b00001010, 0b00001001, 0b00000000, 0b00011000, 0b00010100, 0b00010100, 0b00011000, 0b00010010, 0b00010101, 0b00010011, 0b00000000}

	// correction factor
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}

#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	// HP
	,{0b00001001, 0b00001001, 0b00001001, 0b00001111, 0b00001001, 0b00001001, 0b00001001, 0b00000000, 0b00100000, 0b01000000, 0b01100000, 0b00011000, 0b00010100, 0b00011000, 0b00010000, 0b00010000}

	// kW
	,{0b00100000, 0b01000000, 0b01110000, 0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000000, 0b00010001, 0b00010001, 0b00010101, 0b00010101, 0b00010101, 0b00001010, 0b00001010, 0b00000000}

	// tenths-second clock
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}

#endif // useDragRaceFunction
};

uint8_t getScreenLabelIndex(uint8_t calcIdx)
{

	calcIdx = pgm_read_byte(&calcLabelIdx[(unsigned int)(calcIdx)]);
	if ((calcIdx & 0x80) && (metricFlag & metricMode)) calcIdx++;
	return calcIdx &= 0x7F; // calculate initial position into function label bit pattern table

}

void displayMainScreenFunction(uint32_t screenFormatValue, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripList[], const uint8_t localTripBitmask[][4])
{

	union union_32 * sfvp = (union union_32 *) &screenFormatValue;

	for (uint8_t x = 0; x < 4; x++) displayMainScreenFunction(x, sfvp->u8[(unsigned int)(x)], tripBlink, calcBlink, localTripList, localTripBitmask);

}

void displayMainScreenFunction(uint8_t readingIdx, uint8_t screenFormatValue, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripList[], const uint8_t localTripBitmask[][4])
{

	uint8_t localTripIdx;
	uint8_t calcIdx;
	uint8_t calcBitmask;
	uint8_t tripBitmask;
	uint8_t calcPart;
	uint8_t tripPart;
	uint8_t x;
	uint8_t y;

	readingIdx &= 3;
	x = (readingIdx & 1) << 3;
	y = (readingIdx & 2) >> 1;
	readingIdx <<= 1;

	localTripIdx = (screenFormatValue & dfTripMask) >> dfBitShift; // extract local trip index
	calcIdx = (screenFormatValue & dfFunctionMask); // extract display function

	calcPart = getScreenLabelIndex(calcIdx);
	tripPart = ((calcPart & 0x40) ? localTripIdx : pgm_read_byte(&localTripList[(unsigned int)(localTripIdx)]));

	text::gotoXY(devLCD, x, y);
	text::stringOut(devLCD, doFormat(tripPart, calcIdx, 6, 0));
	text::charOut(devLCD, 0x80 + readingIdx);
	text::charOut(devLCD, 0x81 + readingIdx);

	calcIdx = calcPart;

	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function label component should blink or not

	for (uint8_t x = 0; x < 16; x++)
	{

		calcPart = pgm_read_byte(&calcLabelCGRAM[(unsigned int)(calcIdx & 0x3F)][(unsigned int)(x)]); // read a byte of function label bit pattern

		if (calcIdx & 0x40) tripPart = 0;
		else
		{

			tripPart = (calcPart >> 5) & 0x03; // fetch partial address of trip label component
			tripPart = pgm_read_byte(&localTripBitmask[(unsigned int)(localTripIdx)][(unsigned int)(tripPart)]); // read a byte of trip label bit pattern
			tripPart &= tripBitmask; // provide for blinking trip label component

		}

		calcPart &= calcBitmask; // provide for blinking function label component

		mBuff1[(unsigned int)(x)] = calcPart | tripPart; // combine trip label and function label components

	}

	LCD::loadCGRAMcharacter(readingIdx, (char *)(mBuff1)); // write out generated CGRAM character
	LCD::loadCGRAMcharacter(readingIdx + 1, (char *)(mBuff1 + 8)); // write out generated CGRAM character

}

#ifdef useBigTimeDisplay // Big time output section
void displayBigTime(char * val, uint8_t blinkFlag, uint8_t blinkPos)
{

	val[4] = val[0];
	val[5] = val[1];
	val[7] = val[2];
	val[8] = val[3];
	val[9] = 0;
	val[6] = ':';

	if (blinkFlag) // if it's time to blink something
	{

		if (blinkPos== 4) val[6] = ';'; // if hh:mm separator is selected, blink it
		else if (blinkPos < 2) val[(unsigned int)(blinkPos + 4)] = ' '; // if digit 0 or 1 is selected, blink it
		else if (blinkPos < 4) val[(unsigned int)(blinkPos + 5)] = ' '; // otherwise, if digit 2 or 3 is selected, blink it

	}

	displayBigNumber(&val[4]);

}

#endif
#ifdef useBigNumberDisplay // Big number output section
const uint8_t fedSelectList[] PROGMEM = {
	instantIdx
	,currentIdx
	,tankIdx
};

uint8_t fedSelect(uint8_t bigScreenIdx)
{

	return pgm_read_byte(&fedSelectList[(unsigned int)(screenCursor[(unsigned int)(bigScreenIdx)])]);

}

void displayBigNumber(const uint8_t * prgmPtr, uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalPlaces)
{

	doCalculate(tripIdx, calcIdx); // perform the actual calculation
	displayBigNumber(formatDecimal(prgmPtr, mBuff1, windowLength, decimalPlaces, dfAdjustWindow)); // perform the required decimal formatting, and output the number

}

void displayBigNumber(char * str)
{

	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t x = devLCD.xPos;

	LCD::loadCGRAMfont(bigNumFont);

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

		displayBigDigit(bigNumChars1, x, 0, c, e);
		displayBigDigit(bigNumChars2, x, 1, c, d);
		x += 4;

	}

	text::gotoXY(devLCD, x, 0);

}

void displayBigDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar)
{

	text::gotoXY(devLCD, xPos, yPos);
	text::stringOut(devLCD, digitDefStr, strIdx);
	text::charOut(devLCD, endChar);

}

void displayBigStatus(uint8_t bigScreenIdx, const char * str)
{

	initStatusLine();
	text::stringOut(devLCD, bigFEDispChars, screenCursor[(unsigned int)(bigScreenIdx)]);
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

	text::gotoXY(devLCD, 0, lineNum);
	text::stringOut(devLCD, bgSpaces, lineNum);

	if (tripIdx < 255)
	{

		text::charOut(devLCD, tripIdxChar);
		text::charOut(devLCD, pgm_read_byte(&calcLabelText[(unsigned int)(getScreenLabelIndex(calcIdx) & 0x3F)]));

		text::stringOut(devLCD, doFormat(tripIdx, calcIdx, 6, 0));

	}
	else
	{

		text::clearEOL(devLCD);

	}

}

#endif
unsigned long doCalculate(uint8_t tripIdx, uint8_t calcIdx)
{

	return SWEET64((const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(calcIdx)])), tripIdx);

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

unsigned long str2ull(char * strBuffer)
{

	const uint8_t * prgmPtr;

	uint8_t c;
	uint8_t n;

	uint8_t x;
	uint8_t loopFlag;

	x = 0;
	loopFlag = 1;

	init64((union union_64 *)(&s64reg[1]), 0); // initialize 64-bit number to zero

	while ((loopFlag) && (x < 17))
	{

		if ((c = strBuffer[(unsigned int)(x++)]))
		{

			if (c == ' ') n = 0;
			else n = (uint8_t)(c) - 48;

			prgmPtr = prgmMultiplyBy10; // define d = 10;

			if (x < 17)
			{

				if ((c = strBuffer[(unsigned int)(x++)]))
				{

					prgmPtr = prgmMultiplyBy100; // define d = 100

					n *= 10;

					if (c != ' ') n += (uint8_t)(c) - 48;

				}
				else loopFlag = 0;

			}
			else loopFlag = 0;

			SWEET64(prgmPtr, n); // call SWEET64 routine to perform (accumulated 64-bit number) * d + n

		}
		else loopFlag = 0; //

	}

	return ((union union_64 *)(&s64reg[2]))->ul[0];

}

char * ull2str(const uint8_t * prgmPtr, char * strBuffer, uint8_t decimalPlaces)
{

	union union_64 * tmpPtr2 = (union union_64 *)(&s64reg[2]);

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

		if (c == ' ') strBuffer[(unsigned int)(y - 1)] = '0';
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

char * doFormat(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalFlag)
{

	uint8_t decimalPlaces;
	uint8_t calcWord;

	if ((calcIdx < dfMaxValDisplayCount) && (tripIdx < tripSlotCount))
	{

		doCalculate(tripIdx, calcIdx);

		calcWord = (getScreenLabelIndex(calcIdx) & 0x3F);

		if (calcWord == lblCGRAMtimeInSecondsIdx) ull2str(prgmFormatToTime, mBuff1, 0);
		else
		{

			decimalPlaces = pgm_read_byte(&calcDecimalPoints[(unsigned int)(calcWord)]);

			formatDecimal(prgmRoundOffNumber, mBuff1, windowLength, decimalPlaces, decimalFlag);

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
	instrLdConst, 0x01, idxCyclesPerSecond,
	instrDiv2by1,
	instrDone
};

unsigned long convertTime(unsigned long * an)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	copy64((union union_64 *)(&s64reg[s64reg1]), (union union_64 *)(an)); // perform atomic copy of time variable to SWEET64 register 2

	SREG = oldSREG; // restore state of interrupt flag

	return SWEET64(prgmConvertToTime, 0); // go convert time value from system ticks to seconds

}

#ifdef useChryslerMAPCorrection
const uint8_t prgmGenerateVoltageSlope[] PROGMEM = {
	instrLdEEPROMindexed, 0x02, pMAPsensorCeilingIdx,
	instrLdEEPROMindexed, 0x01, pMAPsensorFloorIdx,
	instrSubYfromX, 0x21,
	instrLd, 0x32,

	instrLdConst, 0x02, idxDenomVoltage,
	instrLdEEPROMindexed, 0x01, pMAPsensorRangeIdx,
	instrMul2by1,

	instrLd, 0x13,
	instrDiv2by1,
	instrDone
};

const uint8_t prgmConvertVolts[] PROGMEM = {
	instrLdEEPROMindexed, 0x02, pMAPsensorFloorIdx,
	instrLdConst, 0x01, idxNumerVoltage,
	instrMul2by1,
	instrLdConst, 0x01, idxDenomVoltage,
	instrDiv2by1,
	instrDone
};

#endif // useChryslerMAPCorrection
const uint8_t prgmInitMPGuino[] PROGMEM = {
	instrLdEEPROM, 0x02, pIdleTimeoutIdx,				// load idle timeout stored parameter
	instrCall, tConvertToTimerTicks,
	instrStVolatile, 0x02, vIdleTimeoutIdx,				// store idle timeout timer ticks value

	instrLdEEPROM, 0x02, pEOCtimeoutIdx,				// load EOC timeout stored parameter
	instrCall, tConvertToTimerTicks,
	instrStVolatile, 0x02, vEOCtimeoutIdx,				// store EOC timeout timer ticks value

	instrLdEEPROM, 0x02, pButtonTimeoutIdx,				// load button press timeout stored parameter
	instrCall, tConvertToTimerTicks,
	instrStVolatile, 0x02, vButtonTimeoutIdx,			// store button press timeout timer ticks value

	instrLdEEPROM, 0x02, pParkTimeoutIdx,				// load parking timeout stored parameter
	instrCall, tConvertToTimerTicks,
	instrStVolatile, 0x02, vParkTimeoutIdx,				// store parking timeout timer ticks value

	instrLdEEPROM, 0x02, pActivityTimeoutIdx,			// load activity timeout stored parameter
	instrCall, tConvertToTimerTicks,
	instrStVolatile, 0x02, vActivityTimeoutIdx,			// store activity timeout timer ticks value

#ifdef useCoastDownCalculator
	instrLdEEPROM, 0x02, pCoastdownSamplePeriodIdx,			// coastdown timer ticks value
	instrCall, tConvertToTimerTicks,
	instrStVolatile, 0x02, vCoastdownPeriodIdx,			// store coastdown timeout timer ticks value

#endif // useCoastDownCalculator
	instrLdConst, 0x01, idxCyclesPerSecond,
	instrLdEEPROM, 0x02, pInjectorSettleTimeIdx,
	instrMul2by1,
	instrLdConst, 0x01, idxMicroSecondsPerSecond,
	instrDiv2by1,
	instrStVolatile, 0x02, vInjSettleCyclesIdx,			// store injector settle time value in cycles

	instrLdByte, 0x01, 60,						// load seconds per minute into register 1
	instrLdEEPROM, 0x02, pCrankRevPerInjIdx,			// load crank revolutions per injector event into register 2
	instrMul2by1,							// perform multiply
	instrLdConst, 0x01, idxCyclesPerSecond,				// load cycles per second into register 1
	instrMul2by1,							// perform conversion
	instrLdEEPROM, 0x01, pMinGoodRPMidx,				// get minimum good RPM figure from EEPROM
	instrDiv2by1,							// convert figure into cycles
	instrStVolatile, 0x02, vMinGoodRPMcyclesIdx,			// store injector settle time value in cycles

	instrLdByte, 0x01, 8,						// multiply minGoodRPMcycles figure by 0.8
	instrMul2by1,
	instrLdByte, 0x01, 10,
	instrDiv2by1,							// (maxGoodInjCycles)
	instrStVolatile, 0x02, vMaxGoodInjCyclesIdx,			// store injector settle time value in cycles

	instrLdEEPROM, 0x02, pMicroSecondsPerGallonIdx,			// fetch injector cycle time in microseconds per US gallon
#ifdef useImperialGallon
	instrSkipIfMetricMode, 10,					// if metric mode set, skip ahead to metric conversion
	instrLdConst, 0x01, idxNumerImperialGallon,			// perform conversion from microseconds per liter into microseconds per Imperial gallon
	instrMul2by1,
	instrLdConst, 0x01, idxDenomImperialGallon,
	instrDiv2by1,
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
#else // useImperialGallon
	instrSkipIfMetricMode, 2,					// if metric mode set, skip ahead to metric conversion
	instrSkip, 10,							// skip past US gallons to liters conversion
#endif // useImperialGallon
	instrLdConst, 0x01, idxDenomVolume,				// perform conversion from microseconds per US gallon into microseconds per liter
	instrMul2by1,
	instrLdConst, 0x01, idxNumerVolume,
	instrDiv2by1,
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrLdConst, 0x01, idxCyclesPerSecond,				// get cycles per second constant
	instrMul2by1,							// multiply to get cycles-microseconds per unit volume-second value
	instrLdConst, 0x01, idxMicroSecondsPerSecond,			// get microseconds per second constant
	instrDiv2by1,							// convert to get cycles per unit volume value
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStVolatile, 0x02, vCyclesPerVolumeIdx,			// save cycles per unit volume value

	instrLdEEPROM, 0x02, pTankSizeIdx,				// fetch tank size in unit volume * formatting factor
#ifdef usePartialRefuel
	instrLdEEPROM, 0x01, pRefuelSizeIdx,				// partial refill quantity in unit volume * formatting factor
	instrAddYtoX, 0x21,						// add partial refill quantity to tank size to artificially inflate tank size
#endif // usePartialRefuel
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrMul2by1,							// multiply to get tank size in cycles * formatting factor
	instrLdConst, 0x01, idxDecimalPoint,				// fetch formatting factor
	instrDiv2by1,							// remove formatting factor to get tank size in cycles
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStVolatile, 0x02, vTankCapacityIdx,			// save tank size in cycles

#ifdef useDragRaceFunction
	instrLdByte, 0x02, 0,						// zero out accel test top speed and estimated engine power
	instrStVolatile, 0x02, vDragInstantSpeedIdx,

#endif // useDragRaceFunction
#ifdef useJSONoutput
	instrLdEEPROM, 0x02, pTankBingoIdx,				// fetch bingo tank size in unit volume
	instrLdVolatile, 0x01, vCyclesPerVolumeIdx,			// fetch cycles per unit volume value
	instrMul2by1,							// multiply to get bingo tank size in cycles * formatting factor
	instrLdConst, 0x01, idxDecimalPoint,				// fetch formatting factor
	instrDiv2by1,							// remove formatting factor to get bingo tank size in cycles
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStVolatile, 0x02, vBingoTankCapacityIdx,			// save bingo tank size in cycles

#endif // useJSONoutput
	instrDone							// exit to caller
};

const uint8_t prgmFindInjResetDelay[] PROGMEM = {
	instrLdVolatile, 0x02, vMinGoodRPMcyclesIdx,			// fetch injector settle time value in cycles
	instrLdIndex, 0,						// divide by 256 to generate timer2 cycles
	instrShiftRight, 0x02,
	instrAddToIndex, 1,
	instrSkipIfIndexBelow, 249, 8,
	instrLdByte, 0x01, 2,						// add 2 to result
	instrAddYtoX, 0x21,						// register 2 (injResetDelay)
	instrDone
};

/* Display cursor update section */

uint8_t cursorXdirection;

void majorScreenLevelEntry(const char * s, uint8_t newScreenLevel)
{

	doCursorMoveAbsolute(newScreenLevel, 255);
	printStatusMessage(s);

}

void doCursorMoveAbsolute(uint8_t positionY, uint8_t positionX)
{

	menuLevel = positionY;
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
			if (positionX < positionY)
				screenCursor[(unsigned int)(menuLevel)] = positionX;
			break;

	}

	doCursorUpdateScreen();

}

void doCursorMoveRelative(uint8_t moveY, uint8_t moveX)
{

	uint8_t wrapAroundFlag;
	uint8_t v;
	uint8_t maxVal;
	uint8_t x; // base menu level

	x = (pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeIndex) & 0x7F); // base menu level

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

		menuLevel = v + x;

		if (wrapAroundFlag) // if wrap-around occurred, look at direction
		{

			if (moveY == 1) v = 0; // if direction was forward, set current cursor to zero
			else v = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount) - 1; // else, set current cursor to end of new menu level

			screenCursor[(unsigned int)(menuLevel)] = v;

		}
		else cursorXdirection = 0;

	}

	doCursorUpdateScreen();

}

void doCursorUpdateScreen(void)
{

	// call indexed support section cursor update function to update any section-specific data
	((pFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].modeCursorUpdate))();

}

void printStatusMessage(const char * s)
{

	initStatusLine();
	text::stringOut(devLCD, s);
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
	text::stringOut(devLCD, PSTR(" Pressed"));
	execStatusLine();

}

/* Top-down menu selector section */

void menu::display(void)
{

	uint8_t i;

	i = screenCursor[(unsigned int)(menuScreenIdx)];
	if (cursorXdirection & 0x80)
	{

		i--;
		if (i > menuSize) i += menuSize;

	}

	for (uint8_t x = 0; x < 2; x++)
	{

		text::gotoXY(devLCD, 0, x);
		text::charOut(devLCD, ((i == screenCursor[(unsigned int)(menuScreenIdx)]) ? '>' : ' ' ));
		text::stringOut(devLCD, (const char *)(pgm_read_word(&menuItems[(unsigned int)(i)].menuString)));
		text::clearEOL(devLCD);
		i++;
		if (i == menuSize) i = 0;

	}

}

void menu::select(void)
{

	uint8_t i;

	i = screenCursor[(unsigned int)(menuScreenIdx)];

	doCursorMoveAbsolute(pgm_read_byte(&menuItems[(unsigned int)(i)].menuIndex), 255);

}

/* Main screen section */

const char mainScreenFuncNames[] PROGMEM = {
#ifdef useDebugReadings
	"Debug\0"
#endif
	"Instrument\0"
	"Custom\0"
#ifdef useChryslerMAPCorrection
	"Pressures\0"
#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
	"Voltages\0"
#endif
	"Instant/Current\0"
	"Instant/Tank\0"
	"Current\0"
	"Tank\0"
#ifdef trackIdleEOCdata
	"EOC/Idle\0"
#endif
	"Current Data\0"
	"Tank Data\0"
#ifdef trackIdleEOCdata
	"Current EOC/Idle\0"
	"Tank EOC/Idle\0"
#endif
	"Remaining\0"
};

const uint8_t mainScreenDisplayFormats[(unsigned int)(mainScreenDisplayFormatSize)] PROGMEM = {
#ifdef useDebugReadings
	(msInstantIdx << dfBitShift) | tInjectorOpenTime		// Debug
	,(msInstantIdx << dfBitShift) | tInjectorTotalTime
	,(msInstantIdx << dfBitShift) | tInjectorPulseCount
	,(msInstantIdx << dfBitShift) | tVSSpulseCount

	,(msInstantIdx << dfBitShift) | tSpeed				// Instrument
	,(msInstantIdx << dfBitShift) | tEngineSpeed
	,(msInstantIdx << dfBitShift) | tFuelRate
	,(msInstantIdx << dfBitShift) | tFuelEcon

#else
	(msInstantIdx << dfBitShift) | tSpeed				// Instrument
	,(msInstantIdx << dfBitShift) | tEngineSpeed
	,(msInstantIdx << dfBitShift) | tFuelRate
	,(msInstantIdx << dfBitShift) | tFuelEcon

#endif
	,(msInstantIdx << dfBitShift) | tFuelEcon			// Custom
	,(msInstantIdx << dfBitShift) | tSpeed
	,(msInstantIdx << dfBitShift) | tFuelRate
	,(msCurrentIdx << dfBitShift) | tFuelEcon

#ifdef useChryslerMAPCorrection
	,((vMAPpressureIdx - vMAPpressureIdx) << dfBitShift) | tPressureChannel		// Pressures
	,((vBaroPressureIdx - vMAPpressureIdx) << dfBitShift) | tPressureChannel
	,((vFuelPressureIdx - vMAPpressureIdx) << dfBitShift) | tPressureChannel
	,(msInstantIdx << dfBitShift) | tCorrectionFactor

#endif // useChryslerMAPCorrection
#ifdef useAnalogRead
#ifdef useTinkerkitLCDmodule
	,(analog0Idx << dfBitShift) | tAnalogChannel			// Voltages
	,(analog1Idx << dfBitShift) | tAnalogChannel
	,(analog2Idx << dfBitShift) | tAnalogChannel
	,(analog2Idx << dfBitShift) | tAnalogChannel
#else
	,(analog0Idx << dfBitShift) | tAnalogChannel			// Voltages
	,(analog1Idx << dfBitShift) | tAnalogChannel
#ifdef useAnalogButtons
	,(analog2Idx << dfBitShift) | tAnalogChannel
#ifdef useTWIsupport
	,(analog0Idx << dfBitShift) | tAnalogChannel
#else // useTWIsupport
	,(analog3Idx << dfBitShift) | tAnalogChannel
#endif // useTWIsupport
#else
	,(analog0Idx << dfBitShift) | tAnalogChannel
	,(analog1Idx << dfBitShift) | tAnalogChannel
#endif
#endif

#endif
	,(msInstantIdx << dfBitShift) | tFuelEcon			// Instant / Current
	,(msInstantIdx << dfBitShift) | tSpeed
	,(msCurrentIdx << dfBitShift) | tFuelEcon
	,(msCurrentIdx << dfBitShift) | tDistance

	,(msInstantIdx << dfBitShift) | tFuelEcon			// Instant / Tank
	,(msInstantIdx << dfBitShift) | tSpeed
	,(msTankIdx << dfBitShift) | tFuelEcon
	,(msTankIdx << dfBitShift) | tDistance

	,(msCurrentIdx << dfBitShift) | tSpeed				// Current
	,(msCurrentIdx << dfBitShift) | tFuelEcon
	,(msCurrentIdx << dfBitShift) | tDistance
	,(msCurrentIdx << dfBitShift) | tFuelUsed

	,(msTankIdx << dfBitShift) | tSpeed				// Tank
	,(msTankIdx << dfBitShift) | tFuelEcon
	,(msTankIdx << dfBitShift) | tDistance
	,(msTankIdx << dfBitShift) | tFuelUsed

#ifdef trackIdleEOCdata
	,(msEOCidleCurrentIdx << dfBitShift) | tDistance		// EOC / Idle
	,(msEOCidleCurrentIdx << dfBitShift) | tFuelUsed
	,(msEOCidleTankIdx << dfBitShift) | tDistance
	,(msEOCidleTankIdx << dfBitShift) | tFuelUsed

#endif
	,(msCurrentIdx << dfBitShift) | tEngineRunTime			// Current data
	,(msCurrentIdx << dfBitShift) | tFuelUsed
	,(msCurrentIdx << dfBitShift) | tMotionTime
	,(msCurrentIdx << dfBitShift) | tDistance

	,(msTankIdx << dfBitShift) | tEngineRunTime			// Tank data
	,(msTankIdx << dfBitShift) | tFuelUsed
	,(msTankIdx << dfBitShift) | tMotionTime
	,(msTankIdx << dfBitShift) | tDistance

#ifdef trackIdleEOCdata
	,(msEOCidleCurrentIdx << dfBitShift) | tEngineRunTime		// Current EOC / Idle
	,(msEOCidleCurrentIdx << dfBitShift) | tFuelUsed
	,(msEOCidleCurrentIdx << dfBitShift) | tMotionTime
	,(msEOCidleCurrentIdx << dfBitShift) | tDistance

	,(msEOCidleTankIdx << dfBitShift) | tEngineRunTime		// Tank EOC / Idle
	,(msEOCidleTankIdx << dfBitShift) | tFuelUsed
	,(msEOCidleTankIdx << dfBitShift) | tMotionTime
	,(msEOCidleTankIdx << dfBitShift) | tDistance

#endif
	,(msTankIdx << dfBitShift) | tFuelUsed				// Remaining
	,(msTankIdx << dfBitShift) | tRemainingFuel
	,(msTankIdx << dfBitShift) | tTimeToEmpty
	,(msTankIdx << dfBitShift) | tDistanceToEmpty
};

#ifdef trackIdleEOCdata
const uint8_t mainScreenEOCformats[(unsigned int)(4)] PROGMEM = {
	(msInstantIdx << dfBitShift) | tSpeed
	,(msCurrentIdx << dfBitShift) | tFuelEcon
	,(msEOCidleCurrentIdx << dfBitShift) | tDistance
	,(msEOCidleTankIdx << dfBitShift) | tDistance
};

const uint8_t mainScreenIdleFormats[(unsigned int)(4)] PROGMEM = {
	(msInstantIdx << dfBitShift) | tFuelRate
	,(msCurrentIdx << dfBitShift) | tFuelEcon
	,(msEOCidleCurrentIdx << dfBitShift) | tFuelUsed
	,(msEOCidleTankIdx << dfBitShift) | tFuelUsed
};

#endif // trackIdleEOCdata
void doCursorUpdateMain(void)
{

	topScreenLevel = menuLevel; // save current menu level for sub-function (param edit, trip load/save, etc) support
	printStatusMessage(findStr(mainScreenFuncNames, screenCursor[(unsigned int)(mainScreenIdx)])); // briefly display screen name

}

void doMainScreenDisplay(void)
{

	uint8_t i;
	uint32_t screenFormatValue;
#ifdef useScreenEditor
	union union_32 * sfvp = (union union_32 *)(&screenFormatValue);
#endif // useScreenEditor

	i = screenCursor[(unsigned int)(mainScreenIdx)];
	i <<= 2;
#ifdef useScreenEditor
	i += eePtrScreensStart;

	for (uint8_t x = 0; x < 4; x++) sfvp->u8[(unsigned int)(x)] = (uint8_t)(EEPROM::readVal(i++));
#else // useScreenEditor
	screenFormatValue = pgm_read_dword(&mainScreenDisplayFormats[(unsigned int)(i)]);
#endif // useScreenEditor

	displayMainScreenFunction(screenFormatValue, 136, 0, msTripList, msTripBitPattern);

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

void doLongGoLeft(void)
{

	doCursorMoveRelative(255, 0);

}

void doLongGoRight(void)
{

	doCursorMoveRelative(1, 0);

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
#endif
	printStatusMessage(PSTR("Current Reset"));

}

#ifdef useBigFE // large Fuel Economy display support section
void doCursorUpdateBigFEscreen(void)
{

	displayBigStatus(bigFEscreenIdx, PSTR(" Fuel Econ"));

}

const uint8_t prgmRoundOffFE[] PROGMEM = {
	instrLdByte, 0x01, 1,						// load a 1 into register 1
	instrAddYtoX, 0x12,						// add register 2 results to register 1
	instrSkipIfFlagSet, SWEET64zeroFlag, 27,			// if register 1 is zero, register 2 has overflow value

	instrLdConst, 0x01, idxNumber5nines,				// if number is greater than 99, round off to nearest integer
	instrSkipIfLTorE, 0x12, 16,
	instrLdConst, 0x01, idxNumber4nines,				// if number is greater than 9, round off to nearest 1/10th
	instrSkipIfLTorE, 0x12, 5,

	instrLdByte, 0x01, 5,						// perform round to nearest 1/100th
	instrSkip, 8,							// skip to add
	instrLdByte, 0x01, 50,						// perform round to nearest 1/10th
	instrSkip, 3,							// skip to add
	instrLdConst, 0x01, idxNumber500,				// perform round to nearest whole integer

	instrAddYtoX, 0x21,						// perform round to nearest number

	instrJump, tConvertToFiveBCDbyteNumber				// go call prgmFormatToNumber to perform actual formatting
};

void doBigFEdisplay(void)
{

	uint8_t tripIdx = fedSelect(bigFEscreenIdx);

	displayBigNumber(prgmRoundOffFE, tripIdx, tFuelEcon, 3, 2);

	text::stringOut(devLCD, bigFEDispChars, screenCursor[(unsigned int)(bigFEscreenIdx)]);
	text::gotoXY(devLCD, 12, 1);
	text::stringOut(devLCD, PSTR("\xfbMPG \xfcL100\xfd"));

}

#endif
#ifdef useBigDTE // large Distance-To-Empty display support section
void doCursorUpdateBigDTEscreen(void)
{

	displayBigStatus(bigDTEscreenIdx, PSTR(" DistToEmpty"));

}

const uint8_t prgmRoundOffDTE[] PROGMEM = {
	instrLdByte, 0x01, 1,						// load a 1 into register 1
	instrAddYtoX, 0x12,						// add register 2 results to register 1
	instrSkipIfFlagSet, SWEET64zeroFlag, 18,			// if register 1 is zero, register 2 has overflow value

	instrLdConst, 0x01, idxNumber6nines,				// if number is greater than 999, round off to nearest integer
	instrSkipIfLTorE, 0x12, 14,
	instrLdConst, 0x01, idxNumber5nines,				// if number is greater than 99, round off to nearest 1/10th
	instrSkipIfLTorE, 0x12, 13,
	instrLdConst, 0x01, idxNumber4nines,				// if number is greater than 9, round off to nearest 1/100th
	instrSkipIfLTorE, 0x12, 12,

	instrJump, tConvertToFiveBCDbyteNumber,				// go call prgmFormatToNumber to perform actual formatting

	instrLdConst, 0x01, idxNumber500,				// perform round to nearest whole integer
	instrSkip, 8,							// skip to add
	instrLdByte, 0x01, 50,						// perform round to nearest 1/10th
	instrSkip, 3,							// skip to add
	instrLdByte, 0x01, 5,						// perform round to nearest 1/100th

	instrAddYtoX, 0x21,						// perform round to nearest number

	instrJump, tConvertToFiveBCDbyteNumber				// go call prgmFormatToNumber to perform actual formatting
};

void doBigDTEdisplay(void)
{

	displayBigNumber(prgmRoundOffDTE, fedSelect(bigDTEscreenIdx), tDistanceToEmpty, 4, 3);

}

#endif
#ifdef useBigTTE // large Time-To-Empty display support section
void doCursorUpdateBigTTEscreen(void)
{

	displayBigStatus(bigTTEscreenIdx, PSTR(" TimeToEmpty"));

}

void doBigTTEdisplay(void)
{

	SWEET64(prgmTimeToEmpty, fedSelect(bigTTEscreenIdx));
	displayBigTime(ull2str(prgmFormatToTime, mBuff1, 3), (mainLoopHeartBeat & 0b01010101), 4);

}

#endif // useBigTTE
#ifdef useLEDpins
void LED::init(void)
{

	LEDportLinit |= LEDpinLinit; // turn on digital output for LED L
#ifdef useTinkerkitLCDmodule
	LEDportTXinit |= LEDpinTXinit; // turn on digital output for TX LED
	LEDportRXinit |= LEDpinRXinit; // turn on digital output for RX LED
#endif // useTinkerkitLCDmodule

}

void LED::shutdown(void)
{

	LEDportLinit &= ~(LEDpinLinit); // turn off digital output for LED L
#ifdef useTinkerkitLCDmodule
	LEDportTXinit &= ~(LEDpinTXinit); // turn off digital output for TX LED
	LEDportRXinit &= ~(LEDpinRXinit); // turn off digital output for RX LED
#endif // useTinkerkitLCDmodule

}

void LED::turnOn(void)
{

	LEDportL |= LEDpinL;
#ifdef useTinkerkitLCDmodule
	LEDportTX &= ~(LEDpinTX);
	LEDportRX &= ~(LEDpinRX);
#endif // useTinkerkitLCDmodule

}

void LED::turnOff(void)
{

	LEDportL &= ~(LEDpinL);
#ifdef useTinkerkitLCDmodule
	LEDportTX |= LEDpinTX;
	LEDportRX |= LEDpinRX;
#endif // useTinkerkitLCDmodule

}

#endif // useLEDpins
#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void)
{

#ifdef useLEDpins
	LED::turnOff();
	LED::shutdown();

#endif // useLEDpins
	timerCommand &= ~(tcDisplayDelay); // cancel any display delays in progress
	LCD::shutdown(); // shut down LCD module

#ifdef useTWIsupport
	TWI::shutdown();

#endif // useTWIsupport
#ifdef useSerial0Port
#ifdef useArduinoMega2560
	PRR0 |= (1 << PRUSART0); // shut off USART0 for power reduction
#else // useArduinoMega2560
	PRR |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // useArduinoMega2560
#endif // useSerial0Port
#ifdef useSerial1Port
	PRR1 |= (1 << PRUSART1); // shut off USART1 for power reduction
#endif // useSerial1Port
#ifdef useSerialLoggingOutput
	loggingSerialPort::shutdown();

#endif // useSerialLoggingOutput
#ifdef useSerialDebugOutput
	debugSerialPort::shutdown();

#endif // useSerialDebugOutput
#ifdef useAnalogRead
#ifdef useAnalogButtons
#ifdef useTinkerkitLCDmodule
	PRR0 |= (1 << PRTIM0); // shut off timer0 module to reduce power consumption
	#ifdef useTimer1
		PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
	#endif // useTimer1
	#ifdef useExpansionPins
		PRR1 |= ((1 << PRTIM3)); // shut off timer3 and timer4 modules to reduce power consumption
	#endif // useExpansionPins
#else // useTinkerkitLCDmodule
	#ifdef useArduinoMega2560
		PRR0 |= (1 << PRTIM0); // shut off timer0 module to reduce power consumption
		#ifdef useTimer1
			PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
		#endif
		#ifdef useExpansionPins
			PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
		#endif // useExpansionPins
	#else // useArduinoMega2560
		PRR |= (1 << PRTIM0); // shut off timer0 module to reduce power consumption
		#ifdef useTimer1
			PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
		#endif // useTimer1
		#ifdef useExpansionPins
			PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
		#endif // useExpansionPins
	#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule

	while (buttonCount == 0) // keep sleeping until there is an analog button press event
	{

		performSleepMode(SLEEP_MODE_ADC); // go perform cheesy ADC noise reduction sleep mode (because we must stay awake enough to monitor buttons)

	}

#ifdef useTinkerkitLCDmodule
	PRR0 &= ~(1 << PRTIM0); // turn on timer0 module
	#ifdef useTimer1
		PRR0 &= ~(1 << PRTIM1); // turn on timer1 module
	#endif // useTimer1
	#ifdef useExpansionPins
		PRR1 &= ~((1 << PRTIM3)); // turn on timer3 and timer4 modules
	#endif // useExpansionPins
#else // useTinkerkitLCDmodule
	#ifdef useArduinoMega2560
		PRR0 &= ~(1 << PRTIM0); // turn on timer0 module
		#ifdef useTimer1
			PRR0 &= ~(1 << PRTIM1); // turn on timer1 module
		#endif
		#ifdef useExpansionPins
			PRR0 &= ~(1 << PRTIM2); // turn on timer2 module
		#endif // useExpansionPins
	#else // useArduinoMega2560
		PRR &= ~(1 << PRTIM0); // turn on timer0 module
		#ifdef useTimer1
			PRR &= ~(1 << PRTIM1); // turn on timer1 module
		#endif
		#ifdef useExpansionPins
			PRR &= ~(1 << PRTIM2); // turn on timer2 module
		#endif // useExpansionPins
	#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
#else // useAnalogButtons
	shutdownADC(); // if analog buttons are not being used, shut down analog converter

	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode
#endif // useAnalogButtons
#else // useAnalogRead
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode
#endif // useAnalogRead

	peripheralInit(); // restart all peripherals

}

#else // useDeepSleep
#ifdef useSoftwareClock // Software Clock support section
const uint8_t prgmChangeSoftwareClock[] PROGMEM = {
	instrLdConst, 0x01, idxCyclesPerSecond,				// convert datetime value from cycles to seconds
	instrDiv2by1,

	instrLdConst, 0x01, idxSecondsPerDay,				// divide by number of seconds in a day, to remove the existing time portion from the datetime value
	instrDiv2by1,

	instrLdByte, 0x01, 24,						// multiply datetime value by 24 (hours per day)
	instrMul2by1,
	instrLdIndex, 0,						// add user-defined hours value to datetime value
	instrLdByteFromYindexed, 0x31,
	instrAddYtoX, 0x21,

	instrLdByte, 0x01, 60,						// multply datetime value by 60 (minutes per hour)
	instrMul2by1,
	instrLdIndex, 2,						// add user-defined minutes value to datetime value
	instrLdByteFromYindexed, 0x31,
	instrAddYtoX, 0x21,

	instrLdByte, 0x01, 60,						// multiply datetime value by 60 (seconds per minute)
	instrMul2by1,
	instrLdIndex, 4,						// add user-defined seconds value to datetime value
	instrLdByteFromYindexed, 0x31,
	instrAddYtoX, 0x21,

	instrLdConst, 0x01, idxCyclesPerSecond,				// convert datetime value from seconds to cycles
	instrMul2by1,

	instrDone
};

void doShowSoftwareClock(void) // display software clock
{

	convertTime((uint32_t *)(&clockCycles)); // perform initialization for ull2str
	displayBigTime(ull2str(prgmFormatToTime, mBuff1, 3), (mainLoopHeartBeat & 0b01010101), 4);

}

void doGoEditSoftwareClock(void)
{

	convertTime((uint32_t *)(&clockCycles)); // perform initialization for ull2str
	ull2str(prgmFormatToTime, pBuff, 3); // convert system time from ticks into seconds, and format for output
	doCursorMoveAbsolute(softwareClockEditScreenIdx, 0);

}

void doEditSoftwareClock(void)
{

	displayBigTime(pBuff, (timerCommand & tcShowCursor), screenCursor[(unsigned int)(softwareClockEditScreenIdx)]);

}

void doEditSoftwareClockChangeDigit(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(softwareClockEditScreenIdx)])]++;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(softwareClockEditScreenIdx)])] > '9') pBuff[(unsigned int)(screenCursor[(unsigned int)(softwareClockEditScreenIdx)])] = '0';

	if (pBuff[2] > '5') pBuff[2] = '0'; // this will only happen if softwareClockEditScreenIdx == 2
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '0'; // this will only happen if softwareClockEditScreenIdx == 0 or 1
	if (pBuff[0] > '2') pBuff[0] = '0'; // this will only happen if softwareClockEditScreenIdx == 0

}

void doEditSoftwareClockSave(void)
{

	uint8_t b;
	uint8_t oldSREG;

	pBuff[4] = '0'; // set seconds to zero
	pBuff[5] = '0';

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	copy64((union union_64 *)(&s64reg[s64reg1]), (union union_64 *)(&clockCycles)); // transfer clock value to SWEET64 register 2

	SREG = oldSREG; // restore state of interrupt flag

	for (uint8_t x = 4; x < 6; x -= 2) // convert time string in pBuff into time value usable by prgmChangeSoftwareClock
	{

		b = pBuff[(unsigned int)(x)] - '0';
		b *= 10;
		b += pBuff[(unsigned int)(x + 1)] - '0';
		((union union_64 *)(&s64reg[s64reg2]))->u8[(unsigned int)(x)] = b;

	}

	SWEET64(prgmChangeSoftwareClock, 0); // convert time value into timer2 clock cycles

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	copy64((union union_64 *)(&clockCycles), (union union_64 *)(&s64reg[s64reg1])); // transfer SWEET64 register 2 value back to clock

	SREG = oldSREG; // restore state of interrupt flag

	majorScreenLevelEntry(PSTR("Time Set"), softwareClockShowScreenIdx);

}

void doEditSoftwareClockCancel(void)
{

	majorScreenLevelEntry(PSTR("Time NOT Set"), softwareClockShowScreenIdx);

}

#endif // useSoftwareClock
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
	instrCall, tSpeed,						// go generate vehicle speed

	instrLdEEPROM, 0x03, pBarLowSpeedCutoffIdx,			// obtain low-speed cutoff parameter in decimal formatting term * unit distance per hour
	instrSkipIfLTorE, 0x32, 4,					// if low-speed cutoff parameter <= speed, skip to next section

	instrLdByte, 0x02, 0xFF,					// load a 255 into register 2
	instrDone,							// exit to caller

	instrSubYfromX, 0x23,						// subtract low-speed cutoff parameter from speed

	instrLdEEPROM, 0x01, pBarSpeedQuantumIdx,			// load speed quantum parameter in decimal formatting term * unit distance per hour
	instrTestRegister, 0x01,
	instrSkipIfFlagSet, SWEET64zeroFlag, 242,			// if (speed quantum parameter == 0), load an 0xFF into register 2 and exit to caller

	instrDiv2by1,							// divide to obtain quantum trip index value

	instrLdByte, 0x03, bgDataSize,					// compare to bargraph data size
	instrSkipIfLTorE, 0x32, 235,					// if (bargraph data size <= quantum trip index value), load an 0xFF into register 2 and exit to caller
	instrLdByte, 0x01, FEvsSpeedIdx,				// add quantum trip index value to base FE vs. speed trip index, to obtain relevant FE vs. speed trip index
	instrAddYtoX, 0x21,
	instrDone							// exit to caller
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

	printStatusMessage(findStr(barFEvSfuncNames, screenCursor[(unsigned int)(barFEvSscreenIdx)])); // briefly display screen name

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
	instrLdByte, 0x01, loopsPerSecond,
	instrLdEEPROM, 0x02, pFEvsTimeIdx,
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

	printStatusMessage(findStr(barFEvTfuncNames, screenCursor[(unsigned int)(barFEvTscreenIdx)])); // briefly display screen name

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
#ifdef useCPUreading // System utilization display section
unsigned long timerLoopStart;
unsigned long thisTimerLoopLength;
unsigned long lastTimerLoopLength;

const uint8_t prgmFindCPUutilPercent[] PROGMEM = {
	instrLdConst, 0x01, idxNumerCPUutil,
	instrMul2by1,
	instrLdConst, 0x01, idxDenomCPUutil,
	instrDiv2by1,
	instrDone
};

const uint8_t prgmOutputAvailableRAM[] PROGMEM = {
	instrLdConst, 0x01, idxDecimalPoint,
	instrMul2by1,
	instrJump, tConvertToFiveBCDbyteNumber
};

void doDisplaySystemInfo(void) // display max cpu utilization and RAM
{

	displayCPUutil();
	text::stringOut(devLCD, PSTR(" T"));
	convertTime((uint32_t *)(&systemCycles));
	text::stringOut(devLCD, ull2str(prgmFormatToTime, mBuff1, 3)); // output system time (since MPGuino was powered up)

	text::gotoXY(devLCD, 0, 1);
	text::stringOut(devLCD, PSTR("FREE RAM: "));
	init64((union union_64 *)(&s64reg[s64reg1]), (unsigned long)(getAvailableRAMsize()));
	text::stringOut(devLCD, formatDecimal(prgmOutputAvailableRAM, mBuff1, 6, 0, 0));

}

void displayCPUutil(void)
{

	text::stringOut(devLCD, PSTR("C%"));
	init64((union union_64 *)(&s64reg[s64reg1]), lastTimerLoopLength);
	SWEET64(prgmFindCPUutilPercent, 0);
	text::stringOut(devLCD, formatDecimal(prgmRoundOffNumber, mBuff1, 6, 2, 0));

}

void doShowCPU(void)
{

	initStatusLine();
	displayCPUutil();
	execStatusLine();

}

unsigned int getAvailableRAMsize(void)
{

	extern char __bss_end;
	extern char *__brkval;

	unsigned int availableRAMptr;

	if((unsigned int)__brkval == 0) availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)&__bss_end);
	else availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)__brkval);

	return availableRAMptr;

}

unsigned long cycles0(void)
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
void doBenchMark(void)
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

		c = (unsigned long)iSqrt((unsigned int)(s));
//		calculateMAPcorrection();
//		c = s * volatileVariables[(unsigned int)(vInjCorrectionIdx)];
//		c >>= 12;

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

	init64((union union_64 *)(&s64reg[s64reg1]), w);
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

#define nextAllowedValue 0
const uint8_t drDragHalfSpeedIdx =	nextAllowedValue;
const uint8_t drDragFullSpeedIdx =	drDragHalfSpeedIdx + 1;
const uint8_t drDragDistanceIdx =	drDragFullSpeedIdx + 1;
#define nextAllowedValue drDragDistanceIdx + 1

const uint8_t drTripList[] PROGMEM = {
	dragHalfSpeedIdx
	,dragFullSpeedIdx
	,dragDistanceIdx
};

const uint8_t drTripBitPattern[][4] PROGMEM = {
	{0b00000000, 0b00000001, 0b00000010, 0b00000100} // /
	,{0b00000000, 0b00000010, 0b00000101, 0b00000010} // full circle
	,{0b00000000, 0b00000110, 0b00000101, 0b00000110} // D
};

const uint8_t dragRaceFormats[] PROGMEM = {
	(drDragDistanceIdx << dfBitShift) | tFuelEcon
	,(drDragDistanceIdx << dfBitShift) | tDragSpeed			// for calculations, it really doesn't matter what trip index is used here
	,(drDragDistanceIdx << dfBitShift) | tAccelTestTime
	,(drDragDistanceIdx << dfBitShift) | tEstimatedEnginePower	// for calculations, it really doesn't matter what trip index is used here

	,(drDragHalfSpeedIdx << dfBitShift) | tAccelTestTime
	,(drDragHalfSpeedIdx << dfBitShift) | tFuelUsed
	,(drDragHalfSpeedIdx << dfBitShift) | tDistance
	,(drDragHalfSpeedIdx << dfBitShift) | tFuelEcon

	,(drDragFullSpeedIdx << dfBitShift) | tAccelTestTime
	,(drDragFullSpeedIdx << dfBitShift) | tFuelUsed
	,(drDragFullSpeedIdx << dfBitShift) | tDistance
	,(drDragFullSpeedIdx << dfBitShift) | tFuelEcon

	,(drDragDistanceIdx << dfBitShift) | tAccelTestTime
	,(drDragDistanceIdx << dfBitShift) | tFuelUsed
	,(drDragDistanceIdx << dfBitShift) | tDistance
	,(drDragDistanceIdx << dfBitShift) | tFuelEcon
};

const uint8_t prgmInitializeAccelTest[] PROGMEM = {
	instrLdConst, 0x02, idxCyclesPerSecond,				// fetch cycles per second constant
	instrStVolatile, 0x02, vDragInstantSpeedIdx,			// store it in instantaneous accel test speed period variable

	instrLdEEPROM, 0x01, pDragSpeedIdx,				// fetch drag function speed parameter value in unit distance per hour
	instrLdEEPROM, 0x02, pPulsesPerDistanceIdx,			// multiply to obtain drag function distance parameter value in VSS pulses
	instrMul2by1,
	instrLd, 0x32,							// save denominator term for later

	instrLdConst, 0x01, idxCyclesPerSecond,				// fetch cycles per second constant
	instrShiftLeft, 0x01,						// multiply it by two to obtain accel half speed period value
	instrLdConst, 0x02, idxSecondsPerHour,				// multiply it by seconds per hour constant
	instrMul2by1,
	instrLdConst, 0x01, idxDecimalPoint,				// fetch decimal formatting term
	instrMul2by1,							// multiply to factor out decimal formatting term from accel test speed parameter value

	instrLd, 0x13,							// fetch denominator term calculated above
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStVolatile, 0x02, vAccelFullPeriodValueIdx,		// save it to accel test full-speed period tripwire variable

	instrShiftLeft, 0x02,						// shift result right one bit
	instrStVolatile, 0x02, vAccelHalfPeriodValueIdx,		// save it to accel test half-speed period tripwire variable

	instrLdEEPROM, 0x01, pDragDistanceIdx,				// fetch drag function distance parameter value in unit distance
	instrLdEEPROM, 0x02, pPulsesPerDistanceIdx,			// multiply to obtain drag function distance parameter value in VSS pulses
	instrMul2by1,
	instrLdConst, 0x01, idxDecimalPoint,				// get rid of decimal formatting factor
	instrDiv2by1,							// perform conversion
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStVolatile, 0x02, vAccelDistanceValueIdx,			// save it to accel test distanct tripwire variable

	instrDone							// exit to caller
};

const uint8_t prgmAccelTestCompareFullSpeeds[] PROGMEM = {
	instrLdEEPROM, 0x01, pDragAutoFlagIdx,				// fetch accel test autotrigger flag
	instrTestRegister, 0x01,					// test accel test autotrigger flag
	instrSkipIfFlagSet, SWEET64zeroFlag, 11,			// if zero, then output a 1

	instrLdTripVar, 0x01, rvVSScycleIdx, dragRawFullSpeedIdx,	// load raw full speed register elapsed time
	instrLdTripVar, 0x02, rvVSScycleIdx, dragFullSpeedIdx,		// load regular full speed register elapsed time

	instrSkipIfLTorE, 0x21, 4,					// if raw full speed register has a smaller time than regular full speed register, output a 1

	instrLdByte, 0x02, 1,						// load a 1 into register 2
	instrDone,							// exit to caller

	instrLdByte, 0x02, 0,						// load a 0 into register 2
	instrDone							// exit to caller
};

void accelerationTest::goDisplay(void)
{

	uint8_t dragScreenPtr;

	if (accelTestState) // if acceleration test state has changed
	{

		// display status message for new state
		printStatusMessage(findStr(accelTestStateMsgs, accelTestState));

	}

	dragScreenPtr = screenCursor[(unsigned int)(dragRaceIdx)] * 4;

	displayMainScreenFunction(pgm_read_dword(&dragRaceFormats[(unsigned int)(dragScreenPtr)]), 136, 0, drTripList, drTripBitPattern);

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

		if (dirty & dirtyGoodVSS) // if vehicle is still moving, then alert driver
		{

			return 1; // vehicle is still moving - can't trigger drag mode

		}
		else // otherwise, set drag trigger
		{

			if (dirty & dirtyGoodInj) // if vehicle engine is running, go trigger drag mode
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
		displayMainScreenFunction(2, ((msInstantIdx << dfBitShift) | tSpeed), 136, 0, msTripList, msTripBitPattern); // call main screen function display routine

		text::gotoXY(devLCD, 8, 1);
		text::stringOut(devLCD, PSTR(" ACTIVE"));
		text::charOut(devLCD, pgm_read_byte(&coastdownSymbol[(unsigned int)(coastdownCharIdx++)]));

	}
	else
	{

		i = screenCursor[(unsigned int)(coastdownIdx)] + pCoefficientDidx;

		parameter::readEEPROM(i);

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
	text::stringOut(devLCD, bigFEDispChars, tripSlot + 1);
	text::clearEOL(devLCD);

	tripPtr = getBaseTripPointer(tripSlot);

	text::gotoXY(devLCD, 0, 1); // go to next line
	if (guinosig == (uint8_t)(EEPROM::readVal(tripPtr + tripSlotSignaturePtr)))
	{

		init64((union union_64 *)(&s64reg[s64reg1]), EEPROM::readVal(tripPtr));
		text::stringOut(devLCD, ull2str(prgmFormatToTime, mBuff1, 3));

	}
	else text::stringOut(devLCD, PSTR("Empty"));
	text::clearEOL(devLCD);

}

void doGoTripSaveTank(void)
{

	doCursorMoveAbsolute(tripSaveTankScreenIdx, 2);

}

void doGoTripSaveCurrent(void)
{

	doCursorMoveAbsolute(tripSaveCurScreenIdx, 2);

}

void doTripSelect(void)
{

	uint8_t tripSlot;

	tripSlot = menuLevel - tripSaveCurScreenIdx;

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 0: // view active
			doCursorMoveAbsolute(topScreenLevel, tripScreenIdxBase + tripSlot);
			break;

#ifdef showSavedTrips
		case 1: // view saved
			doCursorMoveAbsolute(tripShowCurScreenIdx + tripSlot, 0);
			break;

#endif
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
#endif

	tripSlot = menuLevel - tripSaveCurScreenIdx;
	tripIdx = pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot)]);
#ifdef trackIdleEOCdata
	tripIdleEOCidx = pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot + 2)]);
#endif

#ifdef showSavedTrips
	switch (screenCursor[(unsigned int)(menuLevel)])
#else
	switch (screenCursor[(unsigned int)(menuLevel)] + 1)
#endif
	{

		case 2: // save
			tripArray[(unsigned int)(tripIdx)].save(tripSlot);
#ifdef trackIdleEOCdata
			tripArray[(unsigned int)(tripIdleEOCidx)].save(tripSlot + 2);
#endif
			tripDisplayStatus(tripSlot, PSTR(" Save"));
			menuLevel = mainScreenIdx;
			break;

		case 3: // load
			tripArray[(unsigned int)(tripIdx)].load(tripSlot);
#ifdef trackIdleEOCdata
			tripArray[(unsigned int)(tripIdleEOCidx)].load(tripSlot + 2);
#endif
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
	text::stringOut(devLCD, bigFEDispChars, tripSlot + 1);
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

	parameter::readEEPROM(getBaseTripPointer(tripSlot) + tripVariable); // fetch value from EEPROM, and format it for decimal output

	text::charOut(devLCD, '0' + tripSlot);
	text::charOut(devLCD, ':');
	text::charOut(devLCD, ' ');
	text::stringOut(devLCD, tripShowFuncNames, tripVariable);
	text::clearEOL(devLCD);

	text::gotoXY(devLCD, 0, 1); // go to next line
	text::stringOut(devLCD, pBuff); // print value
	text::clearEOL(devLCD);

}

#endif
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

#endif
#ifdef usePartialRefuel // Partial Refuel support section
const char partialRefuelFuncNames[] PROGMEM = {
	"Add Partial\0"
	"Zero Partial\0"
	"Reset Tank Trip\0"
};

void doPartialRefuelDisplay(void)
{

	parameter::readEEPROM(pRefuelSizeIdx); // fetch value from EEPROM, and format it for decimal output

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
			doCursorMoveAbsolute(settingScreenIdx, pRefuelSizeIdx - eePtrSettingsStart);
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

	doCursorMoveAbsolute(partialRefuelScreenIdx, 0);

}

#endif // usePartialRefuel
#ifdef useScreenEditor // Programmable main display screen edit support section
uint8_t screenTripValue = 0;
uint8_t screenFunctionValue = 0;
uint8_t screenEditDirty = 0;

const uint8_t screenEditFlag_dirty =	0x80;

const char seFormatRevertedString[] PROGMEM = "Format reverted";
const char seExitScreenEditString[] PROGMEM = "Screen Display";

uint8_t generateDisplayFormat(uint8_t tripValue, uint8_t functionValue)
{

	return (((tripValue << dfBitShift) & dfTripMask) | (functionValue & dfFunctionMask));

}

void doCursorUpdateScreenEdit(void)
{

	uint8_t b;
	uint8_t c;

	b = screenCursor[(unsigned int)(screenEditIdx)];
	b >>= 1;
	b &= ~screenEditFlag_dirty;

	if ((screenEditDirty & ~screenEditFlag_dirty) ^ b) // if cursor moved to a different screen function altogether
	{

		c = (uint8_t)(EEPROM::readVal(b + eePtrScreensStart));
		screenFunctionValue = (c & dfFunctionMask); // extract display function
		screenTripValue = (c & dfTripMask) >> dfBitShift; // extract trip index

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

	uint8_t screenFormatValue;
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

			screenFormatValue = generateDisplayFormat(screenTripValue, screenFunctionValue);
			if (j == 1) calcBlink = 170;
			else tripBlink = 170;

		}
		else screenFormatValue = (uint8_t)(EEPROM::readVal(eePtrScreensStart + i + x));

		displayMainScreenFunction(x, screenFormatValue, tripBlink, calcBlink, msTripList, msTripBitPattern); // call main screen function display routine

	}

}

void doGoScreenEdit(void)
{

	screenEditDirty = screenCursor[(unsigned int)(screenEditIdx)];
	screenEditDirty >>= 1;
	screenEditDirty ^= 0x01; // force update of screen function
	screenEditDirty &= ~screenEditFlag_dirty;

	doCursorMoveAbsolute(screenEditIdx, screenCursor[(unsigned int)(mainScreenIdx)] << 3);

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

	b = screenCursor[(unsigned int)(screenEditIdx)] >> 1;

	// generate and save new screen function from trip variable and trip function
	EEPROM::writeVal(eePtrScreensStart + b, (unsigned long)(generateDisplayFormat(screenTripValue, screenFunctionValue)));
	screenEditDirty &= ~screenEditFlag_dirty;

	printStatusMessage(PSTR("Format saved"));

}

void doScreenReturnToMain(void)
{

	const char * s;

	if (screenEditDirty & screenEditFlag_dirty) s = seFormatRevertedString;
	else s = seExitScreenEditString;

	screenCursor[(unsigned int)(mainScreenIdx)] = (screenCursor[(unsigned int)(screenEditIdx)] >> 3);
	majorScreenLevelEntry(s, topScreenLevel);

}

#endif // useScreenEditor
#ifdef useDebugReadings
void goStepDebug(void)
{

	const char * strPtr;

	if (debugFlags & debugVSSflag)
	{

		debugFlags &= ~debugVSSflag;

		if (debugFlags & debugInjectorFlag)
		{

			debugFlags &= ~debugInjectorFlag;
			strPtr = PSTR("DebugInj Off");

		}
		else
		{

			debugFlags |= debugInjectorFlag;
			strPtr = PSTR("DebugInj On");

		}

	}
	else
	{

		debugFlags |= debugVSSflag;
		strPtr = PSTR("DebugVSS On");

	}

	printStatusMessage(strPtr);

}

#endif // useDebugReadings
/* EEPROM parameter I/O section prototype */

#ifdef useEEPROMviewer
void EEPROM::display(void)
{

	init64((union union_64 *)(&s64reg[s64reg1]), (unsigned long)(screenCursor[(unsigned int)(eepromViewIdx)]));
	text::stringOut(devLCD, ull2str(prgmFormatToNumber, mBuff1, 3));
	text::clearEOL(devLCD);
	text::gotoXY(devLCD, 0, 1);
	init64((union union_64 *)(&s64reg[s64reg1]), EEPROM::readVal(screenCursor[(unsigned int)(eepromViewIdx)]));
	text::stringOut(devLCD, ull2str(prgmFormatToNumber, mBuff1, 3));
	text::clearEOL(devLCD);

}

#endif // useEEPROMviewer
uint8_t EEPROM::writeVal(uint8_t eePtr, unsigned long val)
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

unsigned long EEPROM::readVal(uint8_t eePtr)
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

unsigned int EEPROM::getAddress(uint8_t eePtr)
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
		l = 1;
		t = eePtr + eeAdrScreensStart;

#endif
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
#endif

	}

	t <<= 3;
	t += (unsigned int)(l);

	return t;

}

#ifdef useWindowTripFilter
void windowTripFilterReset(void)
{

	wtpCurrentIdx = 0;

	for (uint8_t x = 0; x < windowTripFilterSize; x++) tripArray[(unsigned int)(windowTripFilterIdx + x)].reset();

}

#endif
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
	instrLdByte, 0x01, 2,						// load a 2 into register 1
	instrMul2by1,							// multiply results
	instrLdByte, 0x01, 3,						// load a 5 into register 1
	instrDiv2by1,							// divide results
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone							// exit to caller
};

const uint8_t prgmJSON53tankFE[] PROGMEM = {
	instrLdByte, 0x01, 5,						// load a 5 into register 1
	instrMul2by1,							// multiply results
	instrLdByte, 0x01, 3,						// load a 5 into register 1
	instrDiv2by1,							// divide results
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone							// exit to caller
};

const uint8_t prgmJSONminValue[] PROGMEM = {
	instrSkipIfLTorE, 0x21, 2,					// if register 2 contents are less than or equal to register 1 contents, skip
	instrLd, 0x21,							// load register 2 with contents of register 1
	instrDone							// exit to caller
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

	init64((union union_64 *)(&s64reg[s64reg1]), an);
	doOutputNumberJSON(decimalPlaces, labelStr);

}

void doOutputNumberJSON(union union_64 * an, uint8_t decimalPlaces, const char * labelStr)
{

	copy64((union union_64 *)(&s64reg[s64reg1]), an);
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

	digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

	uint64_t instFuelEcon;
	uint64_t tankFuelEcon;
	uint64_t tankFuelEcon53;

	if (timerCommand & tcOutputJSON) // replaced timerChecker with this because cycles0() is a compile-time option that could break JSON output if #doShowCPU is not set
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		timerCommand &= ~(tcOutputJSON); // clear JSON subtitle change timer command

		SREG = oldSREG; // restore interrupt flag status

		if !(--subtitleCount1) subtitleCount1 = 2;
#ifdef useDragRaceFunction
		if !(--subtitleCount2) subtitleCount2 = 3;
#endif // useDragRaceFunction

	}

	unsigned long reserveQuantity = SWEET64(prgmReserveFuelRemaining, 0);

#ifdef usePartialRefuel
	unsigned long tankSize = EEPROM::readVal(pTankSizeIdx) + EEPROM::readVal(pRefuelSizeIdx);
#else // usePartialRefuel
	unsigned long tankSize = EEPROM::readVal(pTankSizeIdx);
#endif // usePartialRefuel

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
#else
			if (reserveQuantity == 0) text::stringOut(devLogSerial, PSTR("fumes remaining, ")); // fumes left
			else doOutputNumberJSON(prgmReserveFuelRemaining, 0, 3, PSTR(" \xfbgal\xfcL\xfd remaining, ")); // reserve remaining fuel left
			doOutputNumberJSON(prgmBingoFuelRemaining, 0, 3, PSTR(" \xfbgal\xfcL\xfd e-reserve\",\n")); // bingo remaining fuel left
#endif // trackIdleEOCdata

			break;

	}

	reserveQuantity = EEPROM::readVal(pTankBingoIdx);

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
	doOutputNumberJSON(prgmReserveDTE, tankIdx, 2, PSTR("\xfbmi\xfckm\xfd to e, ")); // distance to bingo
	doOutputNumberJSON(prgmBingoDTE, tankIdx, 2, PSTR(" \xfbmi\xfckm\xfd e-reserve\",\n")); // distance to fully empty tank from bingo

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
			doOutputNumberJSON(prgmFindReserveRange, tankIdx, 2, PSTR(" \xfbmi\xfckm\xfd safe range, ")); // reserve range
			doOutputNumberJSON(prgmFindRange, tankIdx, 2, PSTR(" \xfbmi\xfckm\xfd dry range\",\n")); // distance to fully empty tank

			break;

	}

	text::stringOut(devLogSerial, PSTR("\"ranges\":["));
	doOutputNumberJSON(prgmFindRange, tankIdx, 1, PSTR(",")); // maximum range
	doOutputNumberJSON(prgmFindReserveRange, tankIdx, 1, PSTR(",")); // range 2, safe range
	doOutputNumberJSON(prgmFindHalfReserveRange, tankIdx, 1, PSTR("],\n")); // range 3, half of safe range

	text::stringOut(devLogSerial, PSTR("\"measures\":["));
	doOutputNumberJSON(prgmFindBingoRange, tankIdx, 2, PSTR("],\n")); // shows miles of e-reserve in bar form

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
const uint8_t dataLogInstr[] PROGMEM = {
	(msInstantIdx << dfBitShift) | tFuelEcon		// average fuel economy  for the past loop
	,(msInstantIdx << dfBitShift) | tSpeed			// average vehicle speed for the past loop
	,(msInstantIdx << dfBitShift) | tInjectorOpenTime	// fuel injector raw open time for the past loop
	,(msInstantIdx << dfBitShift) | tInjectorPulseCount	// fuel injector pulse count for the past loop
	,(msInstantIdx << dfBitShift) | tVSSpulseCount		// vss pulse count for the past loop
};

const uint8_t dLIcount = (sizeof(dataLogInstr) / sizeof(uint8_t));

void doOutputDataLog(void)
{

	uint8_t c = ',';
	uint8_t screenFormatValue;
	uint8_t localTripIdx;
	uint8_t calcIdx;
	uint8_t decimalPlaces;

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		screenFormatValue = pgm_read_byte(&dataLogInstr[(unsigned int)(x)]);
		localTripIdx = (screenFormatValue & dfTripMask) >> dfBitShift;
		calcIdx = getScreenLabelIndex(screenFormatValue & dfFunctionMask);
		localTripIdx = ((calcIdx & 0x40) ? localTripIdx : pgm_read_byte(&msTripList[(unsigned int)(localTripIdx)]));
		calcIdx &= 0x3F;

		decimalPlaces = pgm_read_byte(&calcDecimalPoints[(unsigned int)(calcIdx)]);
		doCalculate(localTripIdx, calcIdx);
		text::stringOut(devLogSerial, formatDecimal(prgmRoundOffNumber, mBuff1, 0, decimalPlaces, dfOverflow9s));
		text::charOut(devLogSerial, c);

	}

}

#endif // useSerialDataLogging
void idleProcess(void)
{

//#ifdef useLEDpins
//	LED::turnOff();
//
//#endif // useLEDpins
#ifdef useSerial0Port
#ifdef useBufferedSerial0Port
#ifdef useArduinoMega2560
	if ((UCSR0B & (1 << TXEN0)) == 0) PRR0 |= (1 << PRUSART0); // if USART0 transmitter is disabled, shut off USART0 for power reduction
#else
	if ((UCSR0B & (1 << TXEN0)) == 0) PRR |= (1 << PRUSART0); // if USART0 transmitter is disabled, shut off USART0 for power reduction
#endif
#endif

#endif
#ifdef useSerial1Port
#ifdef useBufferedSerial1Port
	if ((UCSR1B & (1 << TXEN1)) == 0) PRR1 |= (1 << PRUSART1); // if USART1 transmitter is disabled, shut off USART1 for power reduction
#endif

#endif
	performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

//#ifdef useLEDpins
//	LED::turnOn();
//
//#endif // useLEDpins
}

void performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

}

#ifdef useAdafruitRGBLCDmodule
void writeAdafruitRGBLCDportDirection(uint8_t interruptFlag, uint8_t readLCDflag)
{

	uint16_t MCP23017registers;
	union union_16 * MCP23017reg = (union_16 *)(&MCP23017registers);

	MCP23017reg->u8[0] = btnShortPress21RCL;

	if (readLCDflag) MCP23017reg->u8[1] = lcdDataBitMask;
	else MCP23017reg->u8[1] = 0;

	writeAdafruitRGBLCDregister(interruptFlag, MCP23017_IODIRx, MCP23017reg);

}

void writeAdafruitRGBLCDregister(uint8_t interruptFlag, uint8_t registerAddress, union union_16 * registerValue)
{

	TWI::initWrite(lcdAddress, TWI_STOP, interruptFlag); // open TWI as master transmitter
	TWI::writeByte(registerAddress | MCP23017_PORTA); // specify bank A register address
	TWI::writeByte(registerValue->u8[0]);
	TWI::writeByte(registerValue->u8[1]);
	TWI::commitWrite(); // go write out register contents

}

#endif // useAdafruitRGBLCDmodule
void peripheralInit(void)
{

#ifdef useAdafruitRGBLCDmodule
	uint16_t MCP23017registers;

#endif // useAdafruitRGBLCDmodule
#ifdef useLEDpins
	LED::init();

#endif // useLEDpins
#ifdef useTWIsupport
	TWI::init();

#ifdef useAdafruitRGBLCDmodule
	TWI::initWrite(lcdAddress, TWI_STOP, TWI_MAIN_PROGRAM); // open TWI as master transmitter
	TWI::writeByte(MCP23017_IOCON); // specify IO control register address
	TWI::writeByte(0b00100000); // write initialization value to IO control register
	TWI::commitWrite(); // go write out initialization data

	writeAdafruitRGBLCDportDirection(TWI_MAIN_PROGRAM, 0); // write out port direction (only buttons as input)

	MCP23017registers = (uint16_t)(btnShortPress21RCL);

	writeAdafruitRGBLCDregister(TWI_MAIN_PROGRAM, MCP23017_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (only buttons as input)
	writeAdafruitRGBLCDregister(TWI_MAIN_PROGRAM, MCP23017_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (only buttons as input)

	MCP23017registers = 0;

	writeAdafruitRGBLCDregister(TWI_MAIN_PROGRAM, MCP23017_GPINTENx, (union_16 *)(&MCP23017registers)); // write out interrupt enable register (kind'a useless on the Adafruit RGB LCD module)

	portSwitches = 0;

#endif // useAdafruitRGBLCDmodule
#endif // useTWIsupport
#ifdef useSerialDebugOutput
#ifdef useBufferedDebugSerialPort
	ringBuffer::init(debugSerialBuffer, debugSerialData, debugSerialPort::bufferDevice);
#else // useBufferedDebugSerialPort
	debugSerialPort::init();
#endif // useBufferedDebugSerialPort
	devDebugSerial.charOut = debugSerialPort::pushCharacter;
	devDebugSerial.controlFlags |= odvFlagForceOutput;

#endif // useSerialDebugOutput
#ifdef useSerialLoggingOutput
#ifdef useBufferedLoggingSerialPort
	ringBuffer::init(loggingSerialBuffer, loggingSerialData, loggingSerialPort::bufferDevice);
#else // useBufferedLoggingSerialPort
	loggingSerialPort::init();
#endif // useBufferedLoggingSerialPort
	devLogSerial.charOut = loggingSerialPort::pushCharacter;
	devLogSerial.controlFlags |= odvFlagForceOutput;

#endif // useSerialLoggingOutput
	LCD::init();

}

/* Parameter setting selector section */

#ifdef useCalculatedFuelFactor
const uint8_t prgmCalculateFuelFactor[] PROGMEM = {
	instrLdConst, 0x02, idxCorrFactor,				// obtain reference correction factor
	instrLdEEPROM, 0x01, pSysFuelPressureIdx,			// multiply by this vehicle's stored fuel system absolute pressure
	instrMul2by1,
	instrLdEEPROM, 0x01, pRefFuelPressureIdx,			// divide by the fuel injector reference absolute pressure
	instrDiv2by1,
	instrIsqrt, 0x02,						// perform square root on result
	instrLdEEPROM, 0x01, pInjectorCountIdx,				// get stored number of fuel injectors for this engine
	instrMul2by1,
	instrLdEEPROM, 0x01, pInjectorSizeIdx,				// get injector size in cc/minute * decimal formatting factor (L/min * decimal formatting factor * 1000)
	instrMul2by1,
	instrLd, 0x32,							// save denominator term for later

	instrLdByte, 0x02, 60,						// load seconds per minute into register 2
	instrLdConst, 0x01, idxMicroSecondsPerSecond,			// load microseconds per second into register 1
	instrMul2by1,
	instrLdConst, 0x01, idxOneThousand,				// load number of cc's per liter into register 1
	instrMul2by1,							// factor into numerator term
	instrLdConst, 0x01, idxDecimalPoint,				// set numerator up to cancel reference correction factor in denominator
	instrMul2by1,
	instrLdConst, 0x01, idxCorrFactor,				// set numerator up to cancel reference correction factor in denominator
	instrMul2by1,

	instrLd, 0x13,							// transfer denominator back to register 1
	instrDiv2by1,							// perform comversion for injector flow
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrLdConst, 0x01, idxNumerVolume,				// set up denominator to convert injector flow in liters to US gallons
	instrMul2by1,
	instrLdConst, 0x01, idxDenomVolume,				// set up numerator to convert injector flow in liters to US gallons
	instrDiv2by1,							// perform comversion for injector flow
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

	instrStEEPROM, 0x02, pMicroSecondsPerGallonIdx,
	instrDone
};

#endif // useCalculatedFuelFactor
const uint8_t prgmDoEEPROMmetricConversion[] PROGMEM = {
	instrLdIndex, 0,						// zero the trip index register

	instrLdEEPROMindirect, 0x02,					// load an EEPROM parameter, indexed into the convIdx array
	instrLdNumer, 0x01,						// load numerator into register 1
	instrMul2by1,							// multiply EEPROM parameter value by numerator
	instrLdDenom, 0x01,						// load denominator into register 1
	instrDiv2by1,							// divide EEPROM parameter value by denominator
	instrAddYtoX, 0x24,						// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStEEPROMindirect, 0x02,					// store converted EEPROM parameter
	instrAddToIndex, 1,						// bump conversion index stored in trip index register
	instrSkipIfIndexBelow, 239, convSize,				// if not at end of list of EEPROM paramaters to convert, loop back

	instrDone							// return to caller
};

void settings::cursorUpdate(void)
{

	fetchParameter();

}

void settings::display(void)
{

	text::stringOut(devLCD, parmLabels, screenCursor[(unsigned int)(settingScreenIdx)]); // print parameter name at top left
	text::clearEOL(devLCD);
	text::gotoXY(devLCD, 0, 1); // go to next line
	text::stringOut(devLCD, pBuff);
	text::clearEOL(devLCD);

}

void settings::editParameter(void)
{

	uint8_t parameterPtr = screenCursor[(unsigned int)(settingScreenIdx)] + eePtrSettingsStart;
	uint8_t paramLength = pgm_read_byte(&paramsLength[(unsigned int)(parameterPtr)]);;

	parameter::entry(paramLength, &settingsParamVect);

}

void settings::fetchParameter(void)
{

	uint8_t parameterPtr = screenCursor[(unsigned int)(settingScreenIdx)] + eePtrSettingsStart;

	parameter::readEEPROM(parameterPtr);

	parameterChanged();

}

void settings::parameterChanged(void)
{

	uint8_t parameterPtr = screenCursor[(unsigned int)(settingScreenIdx)] + eePtrSettingsStart;

#ifdef useLegacyLCD
	if (parameterPtr == pContrastIdx) LCD::setContrast((uint8_t)(str2ull(pBuff))); // adjust contrast dynamically

#endif
#ifdef useAdafruitRGBLCDmodule
	if (parameterPtr == pLCDcolorIdx) LCD::setRGBcolor((uint8_t)(str2ull(pBuff))); // adjust backlight color dynamically

#endif
}

uint8_t settings::saveParameter(void)
{

	uint8_t parameterPtr = screenCursor[(unsigned int)(settingScreenIdx)] + eePtrSettingsStart;
	uint8_t paramChangedFlag;

#ifdef usePartialRefuel
	if (parameterPtr == pRefuelSizeIdx) paramChangedFlag = EEPROM::writeVal(pRefuelSizeIdx, str2ull(pBuff) + EEPROM::readVal(pRefuelSizeIdx));
	else paramChangedFlag = EEPROM::writeVal(parameterPtr, str2ull(pBuff));
#else // usePartialRefuel
	paramChangedFlag = EEPROM::writeVal(parameterPtr, str2ull(pBuff));
#endif // usePartialRefuel

	if (paramChangedFlag) // if the setting has changed
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
		initGuino(); // reconfigure system based on changed settings

	}

	return paramChangedFlag;

}

uint8_t settings::EEPROMpowerUpCheck(void)
{

	uint8_t b = 1;
	uint8_t t;

#ifdef forceEEPROMsettingsInit
	if (true)
#else // forceEEPROMsettingsInit
	if (EEPROM::readVal(pSignatureIdx) != newEEPROMsignature)
#endif // forceEEPROMsettingsInit
	{

		b = 0;

		t = pSignatureIdx;
		for (uint8_t x = 0; x < eePtrSettingsEnd; x++) EEPROM::writeVal(t++, pgm_read_dword(&params[(unsigned int)(x)]));

#ifdef useScreenEditor
		t = eePtrScreensStart;
		for (uint8_t x = 0; x < mainScreenDisplayFormatSize; x++) EEPROM::writeVal(t++, (unsigned long)(pgm_read_byte(&mainScreenDisplayFormats[(unsigned int)(x)])));

#endif // useScreenEditor
#ifdef useEEPROMtripStorage
		for (uint8_t x = 0; x < tripAllowedSaveSlotCount; x++) EEPROM::writeVal(getBaseTripPointer(x) + tripSlotSignaturePtr, 0);

#endif // useEEPROMtripStorage
	}

	initGuino();
	return b;

}

void settings::initGuino(void) // initialize hardware and basic system settings
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (EEPROM::readVal(pPulsesPerDistanceIdx) < 8000) VSSpause = (uint8_t)(EEPROM::readVal(pVSSpauseIdx)); // if pulses per distance index < 8000, it's likely that a mechanical VSS is being used
	else VSSpause = 0; // otherwise, force VSS debounce pause to zero

	dirty &= ~(dirtyGoodInj | dirtyInjOpenRead); // reset fuel injector capture mechanism

	EIMSK &= ~((1 << FI_EN_INT_O) | (1 << FI_EN_INT_C)); // disable fuel injector sense interrupts

	EICRx |= ((1 << FI_ISC_OH) | (1 << FI_ISC_OL) | (1 << FI_ISC_CH) | (1 << FI_ISC_CL)); // set injector sense pin control
	EICRx &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? FI_ISC_OL : FI_ISC_CL));

	EIFR |= ((1 << FI_FL_INT_O) | (1 << FI_FL_INT_C)); // clear fuel injector sense flag
	EIMSK |= ((1 << FI_EN_INT_O) | (1 << FI_EN_INT_C)); // enable fuel injector sense interrupts

#ifdef useChryslerMAPCorrection
	for (uint8_t x = 0; x < 2; x++)
	{

		analogFloor[(unsigned int)(x)] = SWEET64(prgmConvertVolts, 0);
		analogSlope[(unsigned int)(x)] = SWEET64(prgmGenerateVoltageSlope, x);
		analogOffset[(unsigned int)(x)] = EEPROM::readVal(pMAPsensorOffsetIdx + x);

	}

	volatileVariables[(unsigned int)(vFuelPressureIdx)] = EEPROM::readVal(pSysFuelPressureIdx); // this is in psig * 1000
	volatileVariables[(unsigned int)(vInjCorrectionIdx)] = 4096;

#endif // useChryslerMAPCorrection
	metricFlag = ((uint8_t)(EEPROM::readVal(pMetricModeIdx)) ? metricMode : 0);

	SWEET64(prgmInitMPGuino, 0); // calculate multiple MPGuino system values for use within code

	injResetDelay = SWEET64(prgmFindInjResetDelay, 0); // used by main timer to timeout any long pending injector reads

	SREG = oldSREG; // restore interrupt flag status

#ifdef useWindowTripFilter
	windowTripFilterReset();

#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	doResetBarFEvT();

#endif // useBarFuelEconVsTime
}

/* Individual parameter editor section */

const uint8_t prgmCompareWithMaximumParamValue[] PROGMEM = {
	instrLdByte, 0x02, 1,						// load MSB of maximum parameter value into register 2
	instrSkipIfIndexBelow, 6, 1,					// if power of 2 has gone to zero, go exit

	instrShiftLeft, 0x02,						// otherwise, shift maximum parameter value left by one bit
	instrAddToIndex, 255,						// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,							// loop back

	instrSkipIfLTorE, 0x23, 4,					// if maximum parameter value <= working number, signal "overflow"
	instrLdByte, 0x02, 0,						// signal that working number is good
	instrDone,							// return to caller

	instrLdByte, 0x02, 255,						// signal that working number is bad
	instrDone							// return to caller
};

const uint8_t prgmFindMaximumParamValue[] PROGMEM = {
	instrLdByte, 0x02, 1,						// load MSB of maximum parameter value into register 2
	instrSkipIfIndexBelow, 6, 1,					// if power of 2 has gone to zero, go exit

	instrShiftLeft, 0x02,						// otherwise, shift maximum parameter value left by one bit
	instrAddToIndex, 255,						// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,							// loop back

	instrLdByte, 0x01, 1,						// load a 1 into register 1
	instrSubYfromX, 0x21,						// subtract 1 from register 2 to obtain final maximum parameter value

	instrJump, tConvertToFiveBCDbyteNumber				// go call prgmFormatToNumber to perform actual formatting
};

void parameter::doCursorUpdate(void)
{

	uint8_t i = screenCursor[(unsigned int)(paramScreenIdx)];

	if ((pBuff[(unsigned int)(i)] == ' ') && (i < 9) && (pBuff[(unsigned int)(i + 1)] == ' ')) screenCursor[(unsigned int)(paramScreenIdx)] = 9;

}

void parameter::display(void)
{

	uint8_t i = screenCursor[(unsigned int)(paramScreenIdx)];
	uint8_t blinkFlag = (timerCommand & tcShowCursor);

	text::stringOut(devLCD, parmLabels, screenCursor[(unsigned int)(settingScreenIdx)]); // print parameter name at top left
	text::clearEOL(devLCD);
	text::gotoXY(devLCD, 0, 1); // go to next line

	uint8_t c = pBuff[(unsigned int)(i)]; // save existing character
	if ((i < 10) && (blinkFlag)) pBuff[(unsigned int)(i)] = '_'; // replace character with an underscore
	text::stringOut(devLCD, pBuff); // print number
	pBuff[(unsigned int)(i)] = c;

	text::blinkMode(devLCD, ((i == 10) && (blinkFlag)));
	text::stringOut(devLCD, &paramButtonChars[0]);
	text::blinkMode(devLCD, ((i == 11) && (blinkFlag)));
	text::stringOut(devLCD, &paramButtonChars[4]);
	text::blinkMode(devLCD, 0);

}

void parameter::entry(uint8_t parameterLength, const parameterVector * callingParameterVector)
{

	paramVar.callingScreenLevel = menuLevel;
	paramVar.parameterLength = parameterLength;
	paramVar.paramVect = callingParameterVector;

	menuLevel = paramScreenIdx;

	findLeft();

}

void parameter::saveEdit(void)
{

	const char * str;
	pSaveFunc saveParam;

	saveParam = (pSaveFunc)pgm_read_word(&(paramVar.paramVect->saveParam));

	if (saveParam()) // if the setting has changed
	{

		str = PSTR("Param Changed");

	}
	else str = PSTR("Param Unchanged");

	majorScreenLevelEntry(str, paramVar.callingScreenLevel);

}

void parameter::cancelEdit(void)
{

	majorScreenLevelEntry(PSTR("Param Reverted"), paramVar.callingScreenLevel);

}

void parameter::findLeft(void)
{

	screenCursor[(unsigned int)(paramScreenIdx)] = 9;

	// do a nice thing and put the edit cursor at the first non zero number
	for (uint8_t x = 9; x < 10; x--) if (pBuff[(unsigned int)(x)] != ' ') screenCursor[(unsigned int)(paramScreenIdx)] = x;

}

void parameter::findRight(void)
{

	screenCursor[(unsigned int)(paramScreenIdx)] = 9;

}

void parameter::readMaxValue(void)
{

	ull2str(prgmFindMaximumParamValue, pBuff, paramVar.parameterLength);
	findLeft();

}

void parameter::readMinValue(void)
{

	formatInputNumber(0);

}

void parameter::revert(void)
{

	((pFunc)pgm_read_word(&(paramVar.paramVect->initializeParam)))();

}

void parameter::readEEPROM(uint8_t parameterPtr)
{

	formatInputNumber(EEPROM::readVal(parameterPtr));

}

void parameter::formatInputNumber(unsigned long v)
{

	init64((union union_64 *)(&s64reg[s64reg1]), v);
	ull2str(prgmFormatToNumber, pBuff, 3);
	findLeft();

}

void parameter::changeDigit(void)
{

	uint8_t c = '0';
	uint8_t d = ' ';

	uint8_t w;

	switch (screenCursor[(unsigned int)(paramScreenIdx)])
	{
		case 10:
			saveEdit();
			break;

		case 11:
			cancelEdit();
			break;

		default:
			w = pBuff[(unsigned int)(screenCursor[(unsigned int)(paramScreenIdx)])]; // fetch digit from stored numeric string representing parameter to be changed
			if (w == ' ') w = '0'; // if this is a leading space, use 0 as working digit
			w++; // adjust working digit
			if (w > '9') w = '0'; // handle working digit rollover

			pBuff[(unsigned int)(screenCursor[(unsigned int)(paramScreenIdx)])] = w;

			for (uint8_t x = 0; x < 9; x++)
			{

				if (pBuff[(unsigned int)(x)] == c) pBuff[(unsigned int)(x)] = d;
				else if ((c == '0') && (pBuff[(unsigned int)(x)] != ' '))
				{

					c = ' ';
					d = '0';

				}

			}

			if (pBuff[9] == ' ') pBuff[9] = '0';

			str2ull(pBuff); // convert parameter buffer string into uint64_t, then shift to register 3
			w = (uint8_t)(SWEET64(prgmCompareWithMaximumParamValue, paramVar.parameterLength));

			if (w) readMinValue();

			((pSaveFunc)pgm_read_word(&(paramVar.paramVect->paramChanged)))();

			break;

	}

}

int main(void)
{

	uint8_t i;
	uint8_t j;

	const buttonList * bpPtr;

	cli(); // disable interrupts while interrupts are being fiddled with

#ifdef useTinkerkitLCDmodule
	// set timer 0 to 8-bit fast pwm mode
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set OC0A to disabled, OC0B to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0));

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	#ifdef useTimer1
		// set timer 1 to 8-bit phase correct pwm mode
		TCCR1A &= ~(1 << WGM11);
		TCCR1A |= (1 << WGM10);
		TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

		// set OC1A to disabled, OC1B to disabled, OC1C to disabled
		TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << COM1C1) | (1 << COM1C0));

		// set timer 1 prescale factor to 64
		TCCR1B &= ~(1 << CS12);
		TCCR1B |= ((1 << CS11) | (1 << CS10));

		// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
		TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

		// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
		TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

		// disable timer 1 interrupts
		TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));

		// clear timer 1 interrupt flags
		TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

	#else // useTimer1
		PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption

	#endif // useTimer1
	#ifdef useExpansionPins
		// set timer 3 to 8-bit phase correct pwm mode
		TCCR3A &= ~(1 << WGM31);
		TCCR3A |= (1 << WGM30);
		TCCR3B &= ~((1 << WGM33) | (1 << WGM32));

		// set OC3A to clear-up/set-down, OC3B to disabled, OC3C to disabled
		TCCR3A &= ~((1 << COM3A0) | (1 << COM3B1) | (1 << COM3B0) | (1 << COM3C1) | (1 << COM3C0));
		TCCR3A |= (1 << COM3A1);

		// set timer 3 prescale factor to 64
		TCCR3B &= ~(1 << CS32);
		TCCR3B |= ((1 << CS31) | (1 << CS30));

		// disable timer 3 input capture noise canceler, select timer 3 falling edge for input capture
		TCCR3B &= ~((1 << ICNC3) | (1 << ICES3));

		// clear timer 3 output compare force bit for OC3A
		TCCR3C &= ~(1 << FOC3A);

		// disable timer 3 interrupts
		TIMSK3 &= ~((1 << ICIE3) | (1 << OCIE3C) | (1 << OCIE3B) | (1 << OCIE3A) | (1 << TOIE3));

		// clear timer 3 interrupt flags
		TIFR3 |= ((1 << ICF3) | (1 << OCF3C) | (1 << OCF3B) | (1 << OCF3A) | (1 << TOV3));

		// set timer 4 to phase correct pwm mode
		TCCR4D |= (1 << WGM40);
		TCCR4D &= ~(1 << WGM41);

		// set OC4A to disabled, OC4B to disabled, OC4D to clear-up/set-down
		TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << COM4B1) | (1 << COM4B0) | (1 << PWM4A) | (1 << PWM4B));
		TCCR4C &= ~(1 << COM4D0);
		TCCR4C |= ((1 << COM4D1) | (1 << PWM4D));

		// set timer 4 prescale factor to 64
		TCCR4B &= ~(1 << CS43);
		TCCR4B |= ((1 << CS42) | (1 << CS41) | (1 << CS40));

		// clear timer 4 output compare force bits for OC4A, OC4B, and OC4D
		TCCR4A &= ~((1 << FOC4A) | (1 << FOC4B));
		TCCR4C &= ~(1 << FOC4D);

		// clear timer 4 PWM inversion mode
		TCCR4B &= ~(1 << PWM4X);

		// set timer 4 dead timer prescaler to 0
		TCCR4B &= ~((1 << DTPS41) | (1 << DTPS40));

		// disable fault protection
		TCCR4D &= ~((1 << FPIE4) | (1 << FPEN4) | (1 << FPNC4) | (1 << FPES4) | (1 << FPAC4) | (1 << FPF4));

		// clear timer 4 update lock, disable timer 4 enhanced compare mode
		TCCR4E &= ~((1 << TLOCK4) | (1 << ENHC4));

		// disable timer 4 interrupts
		TIMSK4 &= ~((1 < OCIE4D) | (1 < OCIE4A) | (1 < OCIE4B) | (1 < TOIE4));

		// clear timer 4 interrupt flags
		TIFR4 |= ((1 << OCF4D) | (1 << OCF4A) | (1 << OCF4B) | (1 << TOV4));

		// set timer 4 dead time to 0
		DT4 = 0;

	#else // useExpansionPins
		PRR1 |= ((1 << PRTIM4) | (1 << PRTIM3)); // disable timer3 and timer4 modules to reduce power consumption

	#endif // useExpansionPins

	DIDR0 |= ((ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC1D) | (1 << ADC0D)); // disable digital input on port F pins
	DIDR1 |= (1 << AIN0D); // disable digital input for analog comparator
	DIDR2 |= ((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D)); // disable digital input on certain port B, port D pins

	PCMSK0 |= (1 << PCINT7); // enable port B VSS pin interrupt

	PCICR |= (1 << PCIE0); // enable selected interrupts on port B

	PRR0 |= ((1 << PRTWI) | (1 << PRSPI) | (1 << PRADC)); // shut off TWI, SPI, ADC modules for power reduction
	PRR1 |= ((1 << PRUSB) | (1 << PRUSART1)); // shut off USB, USART1 modules for power reduction

#else // useTinkerkitLCDmodule
	#ifdef useArduinoMega2560
		// set timer 0 to 8-bit fast pwm mode
		TCCR0A |= ((1 << WGM01) | (1 << WGM00));
		TCCR0B &= ~(1 << WGM02);

		// set OC0A to disabled, OC0B to disabled
		TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0));

		// set timer 0 prescale factor to 64
		TCCR0B &= ~(1 << CS02);
		TCCR0B |= ((1 << CS01) | (1 << CS00));

		// clear timer 0 output compare force bits for OC0A and OC0B
		TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

		// disable timer 0 output compare interrupts
		TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

		// enable timer 0 overflow interrupt
		TIMSK0 |= (1 << TOIE0);

		// clear timer 0 interrupt flags
		TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

		#ifdef useTimer1
			// set timer 1 to 8-bit phase correct pwm mode
			TCCR1A &= ~(1 << WGM11);
			TCCR1A |= (1 << WGM10);
			TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

			#ifdef useExpansionPins
				// set OC1A to disabled, OC1B to non-inverting mode
				TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B0));
				TCCR1A |= (1 << COM1B1);
			#else // useExpansionPins
				// set OC1A to disabled, OC1B to disabled
				TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0));
			#endif // useExpansionPins

			// set timer 1 prescale factor to 64
			TCCR1B &= ~(1 << CS12);
			TCCR1B |= ((1 << CS11) | (1 << CS10));

			// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
			TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

			// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
			TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

			// disable timer 1 interrupts
			TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));

			// clear timer 1 interrupt flags
			TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

		#else // useTimer1
			PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption

		#endif // useTimer1
		#ifdef useExpansionPins

			// set timer 2 to 8-bit phase correct pwm mode
			TCCR2A &= ~(1 << WGM21);
			TCCR2A |= (1 << WGM20);
			TCCR2B &= ~(1 << WGM22);

			// set timer 2 output compare A to clear-up/set-down, output compare B to disabled
			TCCR2A &= ~((1 << COM2A0) | (1 << COM2B1) | (1 << COM2B0));
			TCCR2A |= (1 << COM2A1);

			// set timer 2 prescale factor to 64
			TCCR2B &= ~((1 << CS22));
			TCCR2B |= ((1 << CS21) | (1 << CS20));

			// clear timer 2 output compare force bits for OC2A and OC2B
			TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

			// disable timer 2 interrupts
			TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

			// clear timer 2 interrupt flags
			TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

		#else // useExpansionPins
			PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption

		#endif // useExpansionPins
		DIDR0 = ~(1 << ADC0D); // enable digital input on VSS pin, disable for all other port F pins
		DIDR1 |= ((1 << AIN1D) | (1 << AIN0D)); // disable digital inputs on analog comparator input
		DIDR2 = 0xFF; // disable digital inputs on port K pins

		PCMSK2 |= (1 << PCINT16); // enable VSS pin interrupt

		#ifdef useLegacyButtons
			DIDR2 &= ~((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // enable digital input on port K button pins

			PORTK |= ((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // enable port K button pullup resistors

			PCMSK2 |= ((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // enable port K button interrupts

		#endif
		PCICR |= (1 << PCIE2); // enable selected interrupts on port K

		PRR0 |= ((1 << PRTWI) | (1 << PRSPI) | (1 << PRADC) | (1 << PRUSART0)); // disable TWI, SPI, ADC, USART0 modules to reduce power consumption
		PRR1 |= ((1 << PRTIM5) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1)); // disable timer5, timer4, timer3, USART3, USART2, USART1 modules to reduce power consumption

	#else // useArduinoMega2560
		// set timer 0 to 8-bit fast pwm mode
		TCCR0A |= ((1 << WGM01) | (1 << WGM00));
		TCCR0B &= ~(1 << WGM02);

		// set OC0A to disabled, OC0B to disabled
		TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0));

		// set timer 0 prescale factor to 64
		TCCR0B &= ~(1 << CS02);
		TCCR0B |= ((1 << CS01) | (1 << CS00));

		// clear timer 0 output compare force bits for OC0A and OC0B
		TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

		// disable timer 0 output compare interrupts
		TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

		// enable timer 0 overflow interrupt
		TIMSK0 |= (1 << TOIE0);

		// clear timer 0 interrupt flags
		TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

		#ifdef useTimer1
			// set timer 1 to 8-bit phase correct pwm mode
			TCCR1A &= ~(1 << WGM11);
			TCCR1A |= (1 << WGM10);
			TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

			#ifdef useExpansionPins
				// set OC1A to disabled, OC1B to non-inverting mode
				TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B0));
				TCCR1A |= (1 << COM1B1);
			#else // useExpansionPins
				// set OC1A to disabled, OC1B to disabled
				TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0));
			#endif // useExpansionPins

			// set timer 1 prescale factor to 64
			TCCR1B &= ~(1 << CS12);
			TCCR1B |= ((1 << CS11) | (1 << CS10));

			// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
			TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

			// clear timer 1 output compare force bits for OC1A and OC1B
			TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B));

			// disable timer 1 interrupts
			TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));

			// clear timer 1 interrupt flags
			TIFR1 |= ((1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

		#else // useTimer1
			PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption

		#endif // useTimer1
		#ifdef useExpansionPins
			// set timer 2 to 8-bit phase correct pwm mode
			TCCR2A &= ~(1 << WGM21);
			TCCR2A |= (1 << WGM20);
			TCCR2B &= ~(1 << WGM22);

			// set timer 2 output compare A to clear-up/set-down, output compare B to disabled
			TCCR2A &= ~((1 << COM2A0) | (1 << COM2B1) | (1 << COM2B0));
			TCCR2A |= (1 << COM2A1);

			// set timer 2 prescale factor to 64
			TCCR2B &= ~((1 << CS22));
			TCCR2B |= ((1 << CS21) | (1 << CS20));

			// clear force bits for OC2A and OC2B
			TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

			// disable timer 2 interrupts
			TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

			// clear timer 2 interrupt flags
			TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

		#else // useExpansionPins
			PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption

		#endif // useExpansionPins
		DIDR0 |= ((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D)); // enable digital input on VSS pin, disable on all other port C pins
		DIDR1 |= ((1 << AIN1D) | (1 << AIN0D)); // disable digital input on analog comparator input pins

		PCMSK1 |= (1 << PCINT8); // enable port C VSS pin interrupt

		#ifdef useLegacyButtons
			DIDR0 &= ~((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // enable digital input on port C button pins

			PORTC |= ((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // enable port C button pullup resistors

			PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // enable port C button interrupts

		#endif // useLegacyButtons
		PCICR |= (1 << PCIE1); // enable selected interrupts on port C

		PRR |= ((1 << PRTWI) | (1 << PRSPI) | (1 << PRADC) | (1 << PRUSART0)); // shut off TWI, USART0, SPI, ADC modules for power reduction

	#endif // useArduinoMega2560
#endif // useTinkerkitLCDmodule
	ACSR &= ~(1 << ACIE); // disable analog comparator interrupt
	ACSR |= (1 << ACD); // disable analog comparator module
	ADCSRB &= ~(1 << ACME); // disable analog comparator multiplexer

	for (uint8_t x = 0; x < tUDcount; x++) tripArray[(unsigned int)(pgm_read_byte(&tripUpdateDestList[(unsigned int)(x)]))].reset();

#ifdef useBarFuelEconVsSpeed
	doResetBarFEvS();

#endif // useBarFuelEconVsSpeed
#ifdef useWindowTripFilter
	windowTripFilterReset();

#endif // useWindowTripFilter
	rawTripIdx = raw0tripIdx;
#ifdef trackIdleEOCdata
	rawEOCidleTripIdx = raw0eocIdleTripIdx;
#endif // trackIdleEOCdata
	lastPINxState = PINx; // initialize last input pin state value so as to not erroneously detect a button press on start
	timer0_overflow_count = 0; // initialize timer 0 overflow counter
	timerCommand = 0;
	timerStatus = 0;
	dirty = 0;
	menuLevel = 0;
	topScreenLevel = 0;
	brightnessIdx = 1;
#ifdef useAnalogInterrupt
	analogState = 0;
#ifdef useAnalogRead
	sampleCount = 0;
#ifdef useAnalogButtons
	initADC(asReadButton);
#endif // useAnalogButtons
#endif // useAnalogRead
#endif // useAnalogInterrupt
#ifdef useDragRaceFunction
	accelerationFlags &= ~accelTestClearFlags;
	lastAccelTestStatus = accelerationFlags;
	accelTestStatus = accelerationFlags;
#endif // useDragRaceFunction

#ifdef useDebugReadings
	initADC(asDebugActive);
	debugFlags = 0;
	VSSdebugCount = 46;
	injectorDebugCount = 481;

#endif // useDebugReadings
	if (settings::EEPROMpowerUpCheck() != 1) doCursorMoveAbsolute(settingScreenIdx, 0);; // go through the initialization screen

	timerDelayCount = delay1500ms; // request a set number of timer tick delays equivalent to 1.5 seconds
	timerCommand |= tcDoDelay; // signal request to timer

	sei();

	peripheralInit();

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

#endif // useSerialDebugOutput
#ifdef useSerialLoggingOutput
	text::stringOut(devLogSerial, titleMPGuino);
	text::charOut(devLogSerial, 0x0D);

#endif // useSerialLoggingOutput
	while (timerCommand & tcDoDelay) idleProcess(); // show splash screen for 1.5 seconds

#ifdef useSavedTrips
	if (i) printStatusMessage(PSTR("AutoRestore Done"));

#endif // useSavedTrips
	while (true)
	{

#ifdef useCPUreading
		lastTimerLoopLength = thisTimerLoopLength;
		timerLoopStart = cycles0(); // record starting time

#endif // useCPUreading
		if (timerStatus & tsAwake)
		{

			if (timerStatus & tsFellAsleep)
			{

				LCD::init(); // re-initialize LCD device
				timerStatus &= ~tsFellAsleep;

			}

			if (activityFlags & afResetCurrentTrip)
			{

				activityFlags &= ~(afResetCurrentTrip);
				doTripResetCurrent();

			}

		}

		// this is the part of the main loop that only executes twice a second (or what is defined by loopsPerSecond), to collect and process readings
		if (timerCommand & tcTakeSample) // if main timer has commanded a sample be taken
		{

#ifdef useCPUreading
			lastTimerLoopLength = thisTimerLoopLength;
			thisTimerLoopLength = 0;

#endif // useCPUreading
			timerCommand &= ~tcTakeSample; // acknowledge sample command

			if (timerStatus & tsAwakeOnVehicle)
			{

				cli(); // perform atomic transfer of raw measurement trip variable

				i = rawTripIdx; // get current raw trip variable index
				rawTripIdx = i ^ (raw0tripIdx ^ raw1tripIdx); // set new raw trip variable index

#ifdef trackIdleEOCdata
				j = rawEOCidleTripIdx; // get current raw EOC/idle trip variable index
				rawEOCidleTripIdx = j ^ (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set new raw EOC/idle trip variable index

#endif // trackIdleEOCdata
				sei(); // re-enable interrupts

				tripArray[(unsigned int)(instantIdx)].transfer(tripArray[(unsigned int)(i)]); // transfer old raw trip variable to instant
				tripArray[(unsigned int)(i)].reset(); // reset old raw trip variable so it will be zero the next time rawTripIdx is toggled
#ifdef trackIdleEOCdata
				tripArray[(unsigned int)(eocIdleInstantIdx)].transfer(tripArray[(unsigned int)(j)]); // transfer old raw trip variable to instant
				tripArray[(unsigned int)(j)].reset(); // reset old raw trip variable so it will be zero the next time rawEOCidleTripIdx is toggled
#endif // trackIdleEOCdata

				for (uint8_t x = 0; x < tUScount; x++)
				{

					i = pgm_read_byte(&tripUpdateSrcList[(unsigned int)(x)]);
					j = pgm_read_byte(&tripUpdateDestList[(unsigned int)(x)]);

					tripArray[(unsigned int)(j)].update(tripArray[(unsigned int)(i)]);

				}

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

					if ((dirty & dirtyGoodInj) || (dirty & dirtyGoodVSS)) // if engine is running or vehicle is moving
					{ // update the window trip filter

						tripArray[(unsigned int)(windowTripFilterIdx + wtpCurrentIdx)].transfer(tripArray[(unsigned int)(instantIdx)]);

						SWEET64(prgmUpdateWindowTripFilter, 0); // perform window trip filter magic here

						wtpCurrentIdx++;
						if (wtpCurrentIdx == windowTripFilterSize) wtpCurrentIdx = 0;

					}
					else windowTripFilterReset(); // otherwise, vehicle is still with engine off

				}

#endif // useWindowTripFilter
			}

		}

#ifdef useCPUreading
		thisTimerLoopLength += findCycleLength(timerLoopStart, cycles0());

#endif // useCPUreading
		if (timerStatus & tsButtonRead) // see if any buttons were pressed, process related button function if so
		{

			LCD::hardwareInit();

			j = buttonState; // capture button state
			timerStatus &= ~tsButtonRead; // reset button press flag

			bpPtr = (const buttonList *)(pgm_read_word(&(screenParameters[(unsigned int)(menuLevel)].modeButtonList)));

			while (true)
			{

				i = pgm_read_byte(&(bpPtr->buttonCode));

				if ((j == i) || (i == buttonsUp)) break;
				else bpPtr++;

			}

			text::gotoXY(devLCD, 0, 0);
			if (i == buttonsUp) // if buttonpress was not in menu button list
			{

				switch (j)
				{

					case btnShortPressR:
						doCursorMoveRelative(0, 1); // go to next option in current level
						break;

					case btnShortPressL:
						doCursorMoveRelative(0, 255); // go to previous option in current level
						break;

					case btnShortPressC:
						doCursorMoveAbsolute(menuScreenIdx, 255); // go to the menu screen level
						break;

					case btnLongPressC:
						doNextBright();
						break;

					default: // go perform action meant for 'button not recognized'
						((pFunc)pgm_read_word(&(bpPtr->buttonCommand)))();
						break;

				}

			}
			else ((pFunc)pgm_read_word(&(bpPtr->buttonCommand)))(); // go perform action

		}

#ifdef useCPUreading
		timerLoopStart = cycles0(); // record next starting time

#endif // useCPUreading
		// this part of the main loop handles screen output to the user
		// it can execute either after the samples are collected and processed above, or after a key has been pressed
		if (timerCommand & tcUpdateDisplay)
		{

#ifdef useDragRaceFunction
			cli(); // perform atomic transfer of acceleration status flags

			accelTestStatus = lastAccelTestStatus; // copy last loop's accel test flag status to this loop
			lastAccelTestStatus = accelerationFlags; // copy current accel test flag status for next loop

			sei();

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

#endif // useDragRaceFunction
			timerCommand &= ~tcUpdateDisplay;
			i = activityFlags & afValidFlags;

			if (pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeIndex) & 0x80)
			{

				if ((i == (afIdleFlag | afButtonFlag)) || (i == (afEOCflag | afButtonFlag))) i &= ~(afButtonFlag);

			}

			switch (i)
			{

#ifdef trackIdleEOCdata
				case (afIdleFlag | afButtonFlag): // vehicle stopped, button not pressed
					displayMainScreenFunction(pgm_read_dword(&mainScreenIdleFormats[0]), 136, 0, msTripList, msTripBitPattern);
#ifdef useJSONoutput
					doOutputJSON();
#endif // useJSONoutput
					break;

				case (afEOCflag | afButtonFlag): // engine stopped, button not pressed
					displayMainScreenFunction(pgm_read_dword(&mainScreenEOCformats[0]), 136, 0, msTripList, msTripBitPattern);
#ifdef useJSONoutput
					doOutputJSON();
#endif // useJSONoutput
					break;

#endif // trackIdleEOCdata
				case (afIdleFlag | afEOCflag | afButtonFlag | afParkFlag): // engine stopped, vehicle stopped, no buttons pressed for a while
				case (afIdleFlag | afEOCflag | afParkFlag): // engine stopped, vehicle stopped
					if ((activityFlags & afDoResetCheckFlags) == 0) // only run one time, when MPGuino becomes drowsy
					{

#ifdef useSavedTrips
						if (doTripAutoAction(0)) printStatusMessage(PSTR("AutoSave Done"));

#endif // useSavedTrips
						if (EEPROM::readVal(pWakeupResetCurrentOnEngineIdx)) activityFlags |= afDoResetOnVehicle;
						if (EEPROM::readVal(pWakeupResetCurrentOnButtonIdx)) activityFlags |= afDoResetOnButton;

					}
#ifndef trackIdleEOCdata
				case (afIdleFlag | afButtonFlag): // vehicle stopped, button not pressed
				case (afEOCflag | afButtonFlag): // engine stopped, button not pressed
#endif // trackIdleEOCdata
				case afIdleFlag: // vehicle stopped
				case afEOCflag: // engine stopped
				case (afIdleFlag | afEOCflag): // engine stopped, vehicle stopped
				case (afIdleFlag | afEOCflag | afButtonFlag): // engine stopped, vehicle stopped, button not pressed
				case 0: // engine running and vehicle in motion
				case afButtonFlag: // engine running, vehicle in motion, button not pressed
#ifdef useJSONoutput
					doOutputJSON();
#endif // useJSONoutput
					text::gotoXY(devLCD, 0, 0);
					((pFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].modeDisplay))(); // call indexed support section screen refresh function
					break;

				case afValidFlags: // engine stopped, vehicle stopped, no buttons pressed, park timeout reached, activity timeout reached
					if ((timerStatus & tsFellAsleep) == 0) // only run one time, when MPGuino falls asleep
					{

						timerStatus |= tsFellAsleep;
#ifdef useDeepSleep
						doGoDeepSleep();
#else // useDeepSleep
#ifdef useSoftwareClock
						LCD::setBrightness(0); // set backlight brightness to zero
#endif // useSoftwareClock
#endif // useDeepSleep
					}
#ifndef useDeepSleep
#ifdef useSoftwareClock
					text::gotoXY(devLCD, 0, 0);
					doShowSoftwareClock();
#endif // useSoftwareClock
#endif // useDeepSleep
					break;

				default:
					text::gotoXY(devLCD, 0, 0);
					text::hexByteOut(devLCD, i);
					break;

			}

		}

#ifdef useCPUreading
		thisTimerLoopLength += findCycleLength(timerLoopStart, cycles0());

#endif // useCPUreading
		idleProcess(); // all functions are completed at this point, so wait for an interrupt to occur

	}

}
