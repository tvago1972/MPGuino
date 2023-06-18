// the following options sets preconfigured board settings, which determine hardware port usage, hardware support checking, and processor speed
// if all of the below preconfigured board "#define"s are commented out, code will compile for an AVR microprocessor specified in either the Arduino IDE or the avr toolchain
//
//#define useJellyBeanDriverBoard true		// also sets LCD and button settings
//#define useLegacyBoard true					// also sets LCD and button settings
//#define useArduinoMega2560 true			// (inw)
//#define useTinkerkitLCDmodule true

// the below line sets the serial output speed
//
const unsigned long myBaudRate0 = 19200ul;	// (most Arduino boards)
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

// selectable options - any conflicts will be reported at compile time
//
//#define trackIdleEOCdata true				// Ability to track engine idling and EOC modes
//#define useSpiffyTripLabels true			// Ability to use enhanced trip labels on main display screens
//#define useScreenEditor true				// Ability to change any of (9 or 12, depending on configuration) existing trip data screens, with 4 configurable figures on each screen
//#define useSoftwareClock true				// Shows 24 hour clock driven off of timer0, and provides a means to set it
//#define useBigFE true						// Show big fuel economy displays
//#define useBigDTE true						// Show big distance-to-empty displays
//#define useBigTTE true						// Show big time-to-empty displays
//#define useSpiffyBigChars true			// Provides better number font with use with big number displays above
//#define useBarFuelEconVsTime true			// Show Fuel Economy over Time bar graph
//#define useBarFuelEconVsSpeed true			// Show Fuel Economy vs Speed, Fuel Used vs Speed bar graphs
//#define usePartialRefuel true				// Provide means to enter partial refuel amount into MPGuino
//#define useSavedTrips true					// Ability to save current or tank trips to EEPROM
//#define useFuelCost true					// Show fuel cost
#define useCPUreading true					// Show CPU loading and available RAM usage
//#define useChryslerMAPCorrection true		// Ability to perform on-the-fly fuel injector data correction for late-model Chrysler vehicles
//#define useChryslerBaroSensor true			// allows use of a separate MAP sensor wired to MPGuino to read barometric pressure, for even more accurate correction
//#define useOutputPins true					// Generate analog 0-5VDC output voltage on expansion pins to drive LEDs or feed signal to external gauges
//#define blankScreenOnMessage true			// Completely blank display screen upon display of message
//#define useImperialGallon true				// when selected, uses Imperial gallons instead of default US gallons
//#define useCarVoltageOutput true			// Ability to display alternator voltage and optional secondary sensor (via meelis11)
//#define useDataLoggingOutput true			// Ability to output 5 basic parameters to a data logger or SD card, once every refresh period (0.5 second)

//#define useDeepSleep true					// (inw) places MPGuino into deep sleep after activity timeout
//#define useDragRaceFunction true			// (inw) Performs "drag race" 0-60 MPH, 1/4 mile time, estimated horsepower functionality
//#define useCalculatedFuelFactor true		// (inw) Ability to calculate that pesky us/gal (or L) factor from easily available published fuel injector data
//#define useJSONoutput true					// (inw) skybolt added to enable and call JSON out routine

// performance enhancement options - all may be chosen independently of one another
//
//#define useBufferedLCD true					// Speed up non-serial LCD output
//#define useBufferedSerial0Port true			// Speed up serial port 0 output (including serial LCD)
//#define useBufferedSerial1Port true			// Speed up serial port 1 output (including serial LCD)
//#define useWindowTripFilter true			// Smooths out "jumpy" instant FE figures that are caused by modern OBDII engine computers
#define useAssemblyLanguage true			// Speeds up many low-level MPGuino functions

// logging output port options - choose one or the other with the above logging output options, or an error will result
//
//#define useLoggingSerialPort0 true			// select logging output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLoggingSerialPort1 true		// select logging output on serial port channel 1 (Atmega2560 board)
//#define useLoggingUSBserial true			// select logging output on USB CDC output (TinkerKit! LCD module)

// debug terminal I/O port options - choose one or the other with useDebugTerminal, or an error will result
//
#define useDebugTerminalSerialPort0 true	// select serial port channel 0 for PC-MPGuino terminal interface (most Arduino boards, excluding TinkerKit! LCD module)
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
#define useDebugTerminal true					// terminal interface between PC and MPGuino
#define useDebugTerminalHelp true				// entering '?' in debug terminal displays brief help
#define useDebugTerminalLabels true				// nice labels for various terminal interface output lists - requires useDebugTerminal
#define useDebugCPUreading true					// Show enhanced CPU loading and available RAM usage - requires useDebugTerminal
//#define useSWEET64trace true					// (inw) Ability to view real-time 64-bit calculations from SWEET64 kernel - requires useDebugTerminal
//#define useTestButtonValues true				// Allows observation of button mapping
#define useSimulatedFIandVSS true				// forces simulation of VSS and fuel injector events
//#define useActivityLED true						// allows usage of LEDs to show status of certain bits - currently used to indicate when MPGuino is awake
//#define useDebugAnalog true						// forces ADC support to be compiled in, along with a dedicated analog screen
//#define useSWEET64mult true						// shift mult64 from native C++ to SWEET64 bytecode (saves 36 bytes)
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
#undef useActivityLED
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

#if defined(useSpiffyTripLabels) || useBigDigitDisplay || useBarGraph
#define useLCDgraphics true
#endif // defined(useSpiffyTripLabels) || useBigDigitDisplay || useBarGraph

#if defined(useDebugTerminal)
#define useIsqrt true
#define usePressure true
#endif // defined(useDebugTerminal)

#ifdef useCalculatedFuelFactor
#define useIsqrt true
#define usePressure true
#define useFuelPressure true
#endif // useCalculatedFuelFactor

#ifdef useDebugAnalog
#define useAnalogRead true
#endif // useDebugAnalog

#ifndef useCPUreading
#undef useDebugCPUreading
#endif // useCPUreading

#ifdef useChryslerMAPCorrection
#define useIsqrt true
#define usePressure true
#define useFuelPressure true
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

#if defined(useOutputPins)
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
#endif // defined(useOutputPins)

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
#if useLegacyLCD && __AVR_ATmega328P__ && useActivityLED
#error *** CANNOT use useLegacyLCD and useActivityLED!!! ***
#endif // useLegacyLCD && __AVR_ATmega328P__ && useActivityLED

#if useSoftwareClock && useDeepSleep
#error *** CANNOT use both useSoftwareClock and useDeepSleep!!! ***
#endif // useSoftwareClock && useDeepSleep

#if useCarVoltageOutput && useChryslerBaroSensor
#error *** CANNOT use both useCarVoltageOutput and useChryslerBaroSensor!!! ***
#endif // useCarVoltageOutput && useChryslerBaroSensor
