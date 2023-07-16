// the following options sets preconfigured board settings, which determine hardware port usage, hardware support checking, and processor speed
// if all of the below preconfigured board "#define"s are commented out, code will compile for an AVR microprocessor specified in either the Arduino IDE or the avr toolchain
//
//#define useLegacyBoard true					// sets LCD and button configuration for the original MPGuino circuit
//#define useJellyBeanDriverBoard true		// sets LCD and button configuration for the JBD version of MPGuino
//#define useArduinoMega2560 true			// sets specific LCD configuration for Arduino Mega2560 board
//#define useTinkerkitLCDmodule true		// sets specific LCD configuration for TinkerKit! LCD module 
//#define useMPGuinoColourTouch true			// sets hardware configuration for MPGuino Colour Touch

// the below line sets the serial output speed
//
const unsigned long myBaudRate0 = 19200ul;	// (most Arduino boards, excluding TinkerKit! LCD module))
const unsigned long myBaudRate1 = 19200ul;	// (ATmega2560 board)
const unsigned long myBaudRate2 = 19200ul;	// (ATmega2560 board)
const unsigned long myBaudRate3 = 19200ul;	// (ATmega2560 board)

// only one of the below LCD options may be chosen - choosing more than one will cause a compilation error to occur
//
// if either useTinkerkitLCDmodule or useMPGuinoColourTouch is used, the below options will be ignored
//
//#define useLegacyLCD true					// select Legacy 16x2 4-bit LCD
//#define useDFR0009LCD true						// (inw) select DFRobot DFR0009 LCD Keypad Shield
//#define useAdafruitRGBLCDshield true		// select Adafruit RGB 16x2 4-bit LCD module over TWI
//#define useParallaxSerialLCDmodule true		// select Parallax 16x2 Serial LCD module
//#define useSainSmart2004LCD true			// select SainSmart 2004 20x4 4-bit LCD module over TWI
#define useGenericTWILCD true				// select any 4-bit LCD module over TWI using a PCF8574 port expander

// the below defines determine the LCD character screen width and screen height. These settings are overridden by the above
// LCD device options, if specified.
//
#define LCDcharWidth 20
#define LCDcharHeight 2

// the below options only work if useParallaxSerialLCDmodule is selected. If useParallaxSerialLCDmodule is not selected, the below options will not be inserted at all
//
//#define useLCDserialPort0 true				// select serial LCD module output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLCDserialPort1 true				// select serial LCD module output on serial port channel 1 (ATmega2560 board)
//#define useLCDserialPort2 true				// select serial LCD module output on serial port channel 2 (ATmega2560 board)
//#define useLCDserialPort3 true				// select serial LCD module output on serial port channel 3 (ATmega2560 board)

// the below option only works if useLegacyLCD is selected. If useLegacyLCD is not selected, the below option will not be inserted at all
//
//#define useInvertedLegacyLCDbrightness true	// For alternate LCD backlight connections

// only one of the below hardware button options may be chosen - choosing more than one will cause a compilation error to occur
//
// note: if useAdafruitRGBLCDshield is selected, useLegacyButtons will be ignored
//       if useMPGuinoColourTouch is selected, all hardware button choices will be ignored
//
//#define useLegacyButtons true
//#define useAnalogMuxButtons true
//#define useParallax5PositionSwitch true

// selectable options - any conflicts will be reported at compile time
//
#define trackIdleEOCdata true				// Ability to track engine idling and EOC modes
#define useSavedTrips true					// Ability to save current or tank trips to EEPROM
#define usePartialRefuel true				// Provide means to enter partial refuel amount into MPGuino
#define useFuelCost true					// Show fuel cost
#define useDragRaceFunction true			// Performs "drag race" 0-60 MPH, 1/4 mile time, estimated horsepower functionality
#define useBigFE true						// Show big fuel economy displays
#define useBigDTE true						// Show big distance-to-empty displays
#define useBigTTE true						// Show big time-to-empty displays
#define useBarFuelEconVsTime true			// Show Fuel Economy over Time bar graph
#define useBarFuelEconVsSpeed true			// Show Fuel Economy vs Speed, Fuel Used vs Speed bar graphs
#define useStatusMeter true					// displays a graphical meter for use with MPG display
#define useSpiffyTripLabels true			// Ability to use enhanced trip labels on main display screens
#define useSpiffyBigChars true				// Provides better number font with use with big number displays above
#define useScreenEditor true				// Ability to change any of (9 or 12, depending on configuration) existing trip data screens, with 4 configurable figures on each screen
#define useSoftwareClock true				// Shows 24 hour clock driven off of timer0, and provides a means to set it
#define useCPUreading true					// Show CPU loading and available RAM usage
//#define useChryslerMAPCorrection true		// Ability to perform on-the-fly fuel injector data correction for late-model Chrysler vehicles
//#define useChryslerBaroSensor true			// allows use of a separate MAP sensor wired to MPGuino to read barometric pressure, for even more accurate correction
//#define useOutputPins true					// Generate analog 0-5VDC output voltage on expansion pins to drive LEDs or feed signal to external gauges
//#define blankScreenOnMessage true			// Completely blank display screen upon display of status message
//#define useImperialGallon true				// when selected, uses Imperial gallons instead of default US gallons
#define useCarVoltageOutput true			// Ability to display alternator voltage and optional secondary sensor (via meelis11)
//#define useDataLoggingOutput true			// Ability to output 5 basic parameters to a data logger or SD card, once every refresh period (0.5 second)

//#define useDeepSleep true					// (inw) places MPGuino into deep sleep after activity timeout
//#define useCalculatedFuelFactor true		// (inw) Ability to calculate that pesky us/gal (or L) factor from easily available published fuel injector data
//#define useJSONoutput true					// (inw) skybolt added to enable and call JSON out routine

// performance enhancement options - all may be chosen independently of one another
//
#define useBufferedLCD true					// Speed up LCD output
//#define useBufferedSerial0Port true			// Speed up serial port 0 output
#define useBufferedSerial1Port true			// Speed up serial port 1 output
//#define useBufferedSerial2Port true			// Speed up serial port 2 output
//#define useBufferedSerial3Port true			// Speed up serial port 3 output
#define useWindowTripFilter true			// Smooths out "jumpy" instant FE figures that are caused by modern OBDII engine computers
#define useAssemblyLanguage true			// Speeds up many low-level MPGuino functions

// logging output port options - choose one or the other with the above logging output options, or an error will result
//
//#define useLoggingSerialPort0 true			// select logging output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLoggingSerialPort1 true		// select logging output on serial port channel 1 (ATmega2560 board)
//#define useLoggingSerialPort2 true		// select logging output on serial port channel 2 (ATmega2560 board)
//#define useLoggingSerialPort3 true		// select logging output on serial port channel 3 (ATmega2560 board)
//#define useLoggingUSBserial true			// select logging output on USB CDC output (TinkerKit! LCD module)

// debug terminal I/O port options - choose one or the other with useDebugTerminal, or an error will result
//
//#define useDebugTerminalSerialPort0 true	// select serial port channel 0 for PC-MPGuino terminal interface (most Arduino boards, excluding TinkerKit! LCD module)
#define useDebugTerminalSerialPort1 true	// select serial port channel 1 for PC-MPGuino terminal interface (ATmega2560 board)
//#define useDebugTerminalSerialPort2 true	// select serial port channel 2 for PC-MPGuino terminal interface (ATmega2560 board)
//#define useDebugTerminalSerialPort3 true	// select serial port channel 3 for PC-MPGuino terminal interface (ATmega2560 board)
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
#define useDebugTerminal true				// terminal interface between PC and MPGuino
#define useDebugTerminalHelp true			// entering '?' in debug terminal displays brief help - requires useDebugTerminal
#define useDebugTerminalLabels true			// nice labels for various terminal interface output lists - requires useDebugTerminal
#define useDebugButtonInjection true		// ability to inject button presses into MPGuino - requires useDebugTerminal
//#define useDebugCPUreading true				// Show enhanced CPU loading - requires useDebugTerminal
//#define useSWEET64trace true				// (inw) Ability to view real-time 64-bit calculations from SWEET64 kernel - requires useDebugTerminal
//#define useTestButtonValues true			// Allows observation of button mapping
#define useSimulatedFIandVSS true			// forces simulation of VSS and fuel injector events
#define useActivityLED true					// indicates when MPGuino is awake vs idle/asleep
#define useDebugAnalog true					// forces ADC support to be compiled in, along with a dedicated analog screen
//#define useSWEET64mult true					// shift mult64 from native C++ to SWEET64 bytecode (saves 36 bytes)
//#define useSWEET64div true					// shift div64 from native C++ to SWEET64 bytecode (saves 220 bytes)

// these #defines are used to select various features to support the above choices
// do not mess with them, or compilation errors will occur
//

#if ( defined(useLegacyBoard) + defined(useJellyBeanDriverBoard) + defined(useArduinoMega2560) + defined(useTinkerkitLCDmodule) + defined(useMPGuinoColourTouch) ) > 1
#error *** Pre-defined MPGuino board conflict exists!!! ***
#endif // ( defined(useLegacyBoard) + defined(useJellyBeanDriverBoard) + defined(useArduinoMega2560) + defined(useTinkerkitLCDmodule) + defined(useMPGuinoColourTouch) ) > 1

#if not(__AVR_ATmega32U4__) && not(__AVR_ATmega2560__) && not(__AVR_ATmega328P__)
#warning *** Unknown board - hardware features may not work! ***
#endif // not(__AVR_ATmega32U4__) && not(__AVR_ATmega2560__) && not(__AVR_ATmega328P__)

#if defined(useTinkerkitLCDmodule) && defined(useLegacyButtons)
#error *** CANNOT configure for TinkerkitLCDmodule and Legacy Buttons!!! ***
#endif // defined(useTinkerkitLCDmodule) && defined(useLegacyButtons)

#if defined(useMPGuinoColourTouch)
#define useTFToutput true
#define useTouchScreenInput true
#define useBluetooth true
#define useCarVoltageOutput true
#undef useLegacyButtons
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useDebugButtonInjection
#undef useLegacyLCD
#undef useDFR0009LCD
#undef useAdafruitRGBLCDshield
#undef useParallaxSerialLCDmodule
#undef useSainSmart2004LCD
#undef useGenericTWILCD
#endif // defined(useMPGuinoColourTouch)

#if defined(useArduinoMega2560)
#if !defined(__AVR_ATmega2560__)
#error *** useArduinoMega2560 requires compilation for ATmega2560 board!!! ***
#endif // !defined(__AVR_ATmega2560__)
#endif // defined(useArduinoMega2560)

#if defined(useTinkerkitLCDmodule)
#if !defined(__AVR_ATmega32U4__)
#error *** useTinkerkitLCDmodule requires compilation for ATmega32U4 board!!! ***
#endif // !defined(__AVR_ATmega32U4__)
#define usePort4BitLCD true
#define useLCDcontrast true
#define LCDcharWidth 16
#define LCDcharHeight 2
#undef useDFR0009LCD
#undef useAdafruitRGBLCDshield
#undef useParallaxSerialLCDmodule
#undef useSainSmart2004LCD
#undef useGenericTWILCD
#undef useLegacyLCD
#endif // defined(useTinkerkitLCDmodule)

#if defined(useJellyBeanDriverBoard) || defined(useLegacyBoard)
#if !defined(__AVR_ATmega328P__)
#error *** Legacy/JBD boards require compilation for ATmega328 board!!! ***
#endif // !defined(__AVR_ATmega328P__)
#define useLegacyLCD true
#define useLegacyButtons true
#if defined(useJellyBeanDriverBoard)
#define F_CPU = 20000000UL; // redefine F_CPU
#endif // defined(useJellyBeanDriverBoard)
#undef useDFR0009LCD
#undef useAdafruitRGBLCDshield
#undef useParallaxSerialLCDmodule
#undef useSainSmart2004LCD
#undef useGenericTWILCD
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useActivityLED
#endif // defined(useJellyBeanDriverBoard) || defined(useLegacyBoard)

// LCD defines and sanity checks

#if ( defined(useLegacyLCD) + defined(useDFR0009LCD) + defined(useAdafruitRGBLCDshield) + defined(useParallaxSerialLCDmodule) + defined(useSainSmart2004LCD) + defined(useGenericTWILCD) ) > 1
#error *** MPGuino can only use exactly one LCD device!!! ***
#endif // ( defined(useLegacyLCD) + defined(useDFR0009LCD) + defined(useAdafruitRGBLCDshield) + defined(useParallaxSerialLCDmodule) + defined(useSainSmart2004LCD) + defined(useGenericTWILCD) ) > 1

#if defined(useLegacyLCD)
#define LCDcharWidth 16
#define LCDcharHeight 2
#define usePort4BitLCD true
#undef useSerialLCD
#undef useTWI4BitLCD
#define useLCDcontrast true
#endif // defined(useLegacyLCD)

#if defined(useDFR0009LCD)
#define LCDcharWidth 16
#define LCDcharHeight 2
#define usePort4BitLCD true
#undef useSerialLCD
#undef useTWI4BitLCD
#endif // defined(useDFR0009LCD)

#if defined(useParallaxSerialLCDmodule)
#define LCDcharWidth 16
#define LCDcharHeight 2
#define useSerialLCD true
#undef usePort4BitLCD
#undef useTWI4BitLCD
#endif // defined(useParallaxSerialLCDmodule)

#if defined(useGenericTWILCD)
#define useTWI4BitLCD true
#undef usePort4BitLCD
#undef useSerialLCD
#endif // defined(useGenericTWILCD)

#if defined(useSainSmart2004LCD)
#define LCDcharWidth 20
#define LCDcharHeight 4
#define useTWI4BitLCD true
#undef usePort4BitLCD
#undef useSerialLCD
#endif // defined(useSainSmart2004LCD)

#if defined(useAdafruitRGBLCDshield)
#define LCDcharWidth 16
#define LCDcharHeight 2
#define useTWI4BitLCD true
#define useMCP23017portExpander true
#undef usePort4BitLCD
#undef useSerialLCD
#if !defined(useAnalogButtons)
#undef useLegacyButtons
#define useTWIbuttons true
#endif // !defined(useAnalogButtons)
#endif // defined(useAdafruitRGBLCDshield)

#if defined(useSpiffyTripLabels) && LCDcharHeight == 4
#error *** CANNOT use useSpiffyTripLabels with an LCD display height of 4 lines!!! ***
#endif // defined(useSpiffyTripLabels) && LCDcharHeight == 4

#if defined(usePartialRefuel) || defined(useSavedTrips)
#define useEnhancedTripReset true
#endif // defined(usePartialRefuel) || defined(useSavedTrips)

#if defined(useSoftwareClock)
#define useClockDisplay true
#define useBigTimeDisplay true
#endif // defined(useSoftwareClock)

#if defined(useStatusMeter)
#define useExpandedMainDisplay true
#endif // defined(useStatusMeter)

#if defined(useBigFE)
#define useExpandedMainDisplay true
#define useBigNumberDisplay true
#endif // defined(useBigFE)

#if defined(useBarFuelEconVsTime)
#define useExpandedMainDisplay true
#define useBarGraph true
#endif // defined(useBarFuelEconVsTime)

#if defined(useBarFuelEconVsSpeed)
#define useExpandedMainDisplay true
#define useBarGraph true
#endif // defined(useBarFuelEconVsSpeed)

#if defined(useBigTTE)
#define useExpandedMainDisplay true
#define useBigTimeDisplay true
#endif // defined(useBigTTE)

#if defined(useBigDTE)
#define useExpandedMainDisplay true
#define useBigNumberDisplay true
#endif // defined(useBigDTE)

#if defined(useCPUreading)
#define useExpandedMainDisplay true
#else // defined(useCPUreading)
#undef useDebugCPUreading
#endif // defined(useCPUreading)

#if defined(useClockDisplay)
#define useExpandedMainDisplay true
#endif // defined(useClockDisplay)

#if defined(useBigTimeDisplay) || defined(useBigNumberDisplay)
#define useBigDigitDisplay true
#endif // defined(useBigTimeDisplay) || defined(useBigNumberDisplay)

#if defined(useBigDigitDisplay) || defined(useStatusMeter)
#define useLCDfonts true
#endif // defined(useBigDigitDisplay) || defined(useStatusMeter)

#if defined(useSpiffyTripLabels) || defined(useBigDigitDisplay) || defined(useBarGraph) || defined(useLCDfonts)
#define useLCDgraphics true
#endif // defined(useSpiffyTripLabels) || defined(useBigDigitDisplay) || defined(useBarGraph) || defined(useLCDfonts)

#if defined(useDebugTerminal)
#define useIsqrt true
#define usePressure true
#endif // defined(useDebugTerminal)

#ifdef useCalculatedFuelFactor
#define useIsqrt true
#define usePressure true
#define useFuelPressure true
#endif // useCalculatedFuelFactor

#if defined(useDebugAnalog)
#define useAnalogRead true
#endif // defined(useDebugAnalog)

#if defined(useChryslerMAPCorrection)
#define useIsqrt true
#define usePressure true
#define useFuelPressure true
#define useAnalogRead true
#else // defined(useChryslerMAPCorrection)
#undef useChryslerBaroSensor
#endif // defined(useChryslerMAPCorrection)

#if defined(useCarVoltageOutput)
#define useAnalogRead true
#endif // defined(useCarVoltageOutput)

#if defined(useCoastDownCalculator)
#define useVehicleParameters true
#define useMatrixMath true
#endif // defined(useCoastDownCalculator)

#ifdef useFuelParamCalculator
#define useMatrixMath true
#endif // useFuelParamCalculator

#if defined(useSerialLCD)
#define useBinaryLCDbrightness true
#else // defined(useSerialLCD)
#undef useLCDserialPort0
#undef useLCDserialPort1
#undef useLCDserialPort2
#undef useLCDserialPort3
#endif // defined(useSerialLCD)

#if !defined(useJSONoutput) && !defined(useDataLoggingOutput)
#undef useLoggingSerialPort0
#undef useLoggingSerialPort1
#undef useLoggingSerialPort2
#undef useLoggingSerialPort3
#undef useLoggingUSBserial
#endif // !defined(useJSONoutput) && !defined(useDataLoggingOutput)

#if !defined(useDebugTerminal)
#undef useDebugTerminalSerialPort0
#undef useDebugTerminalSerialPort1
#undef useDebugTerminalSerialPort2
#undef useDebugTerminalSerialPort3
#undef useDebugTerminalUSBserial
#undef useDebugButtonInjection
#undef useDebugTerminalLabels
#undef useDebugTerminalHelp
#undef useDebugCPUreading
#undef useSWEET64trace
#endif // !defined(useDebugTerminal)

#if defined(useLCDserialPort0)
#define useSerial0Port true
#define devLCDserial devSerial0
#define LCDserialPort serial0
#if defined(useBufferedLCD)
#undef useBufferedLCD
#define useBufferedSerial0Port true
#endif // defined(useBufferedLCD)
#if defined(useBufferedSerial0Port)
#define LCDserialBuffer serial0Buffer
#endif // defined(useBufferedSerial0Port)
#endif // defined(useLCDserialPort0)

#if defined(useLCDserialPort1)
#define useSerial1Port true
#define devLCDserial devSerial1
#define LCDserialPort serial1
#if defined(useBufferedLCD)
#undef useBufferedLCD
#define useBufferedSerial1Port true
#endif // defined(useBufferedLCD)
#if defined(useBufferedSerial1Port)
#define LCDserialBuffer serial1Buffer
#endif // defined(useBufferedSerial1Port)
#endif // defined(useLCDserialPort1)

#if defined(useLCDserialPort2)
#define useSerial2Port true
#define devLCDserial devSerial2
#define LCDserialPort serial2
#if defined(useBufferedLCD)
#undef useBufferedLCD
#define useBufferedSerial2Port true
#endif // defined(useBufferedLCD)
#if defined(useBufferedSerial2Port)
#define LCDserialBuffer serial2Buffer
#endif // defined(useBufferedSerial2Port)
#endif // defined(useLCDserialPort2)

#if defined(useLCDserialPort3)
#define useSerial3Port true
#define devLCDserial devSerial3
#define LCDserialPort serial3
#if defined(useBufferedLCD)
#undef useBufferedLCD
#define useBufferedSerial3Port true
#endif // defined(useBufferedLCD)
#if defined(useBufferedSerial3Port)
#define LCDserialBuffer serial3Buffer
#endif // defined(useBufferedSerial3Port)
#endif // defined(useLCDserialPort3)

#if defined(useLoggingSerialPort0)
#define useSerial0Port true
#define devLogOutput devSerial0
#define outputLoggingSplash true
#endif // defined(useLoggingSerialPort0)

#if defined(useLoggingSerialPort1)
#define useSerial1Port true
#define devLogOutput devSerial1
#define outputLoggingSplash true
#endif // defined(useLoggingSerialPort1)

#if defined(useLoggingSerialPort2)
#define useSerial2Port true
#define devLogOutput devSerial2
#define outputLoggingSplash true
#endif // defined(useLoggingSerialPort2)

#if defined(useLoggingSerialPort3)
#define useSerial3Port true
#define devLogOutput devSerial3
#define outputLoggingSplash true
#endif // defined(useLoggingSerialPort3)

#if defined(useLoggingUSBserial)
#define useUSBserial true
#define devLogOutput devUSB
#define outputLoggingSplash true
#endif // defined(useLoggingUSBserial)

#if defined(useDebugTerminalSerialPort0)
#define useSerial0Port true
#define useSerial0PortInput true
#define devDebugTerminal devSerial0
#define outputDebugTerminalSplash true
#endif // defined(useDebugTerminalSerialPort0)

#if defined(useDebugTerminalSerialPort1)
#define useSerial1Port true
#define useSerial1PortInput true
#define devDebugTerminal devSerial1
#define outputDebugTerminalSplash true
#endif // defined(useDebugTerminalSerialPort1)

#if defined(useDebugTerminalSerialPort2)
#define useSerial2Port true
#define useSerial2PortInput true
#define devDebugTerminal devSerial2
#define outputDebugTerminalSplash true
#endif // defined(useDebugTerminalSerialPort2)

#if defined(useDebugTerminalSerialPort3)
#define useSerial3Port true
#define useSerial3PortInput true
#define devDebugTerminal devSerial3
#define outputDebugTerminalSplash true
#endif // defined(useDebugTerminalSerialPort3)

#if defined(useDebugTerminalUSBserial)
#define useUSBserial true
#define devDebugTerminal devUSB
#define outputDebugTerminalSplash true
#endif // defined(useDebugTerminalUSBserial)

#if defined(useBufferedSerial0Port) || defined(useBufferedSerial1Port) || defined(useBufferedSerial2Port) || defined(useBufferedSerial3Port) \
	|| defined(useSerial0PortInput) || defined(useSerial1PortInput) || defined(useSerial2PortInput) || defined(useSerial3PortInput) \
	|| defined(useBufferedLCD) \
	|| defined(useUSBserial)
#define useBuffering true
#endif

#if defined(useTouchScreenInput)
#undef useLegacyButtons
#undef useAnalogMuxButtons
#undef useParallax5PositionSwitch
#undef useAnalogButtons
#undef useTWIbuttons
#undef useDebugButtonInjection
#endif // defined(useTouchScreenInput)

#if defined(useAnalogMuxButtons) || defined(useParallax5PositionSwitch)
#define useAnalogButtons true
#endif // defined(useAnalogMuxButtons) || defined(useParallax5PositionSwitch)

#if defined(useTWI4BitLCD)
#define useTWIsupport true
#define useBinaryLCDbrightness true
#endif // defined(useTWI4BitLCD)

#if defined(usePort4BitLCD) || defined(useTWI4BitLCD)
#define use4BitLCD true
#endif // defined(usePort4BitLCD) || defined(useTWI4BitLCD)

#if defined(usePort4BitLCD) || defined (useSerialLCD) || defined(useTWI4BitLCD)
#define useLCDoutput true
#define useTimer1Interrupt true
#endif // defined(usePort4BitLCD) || defined (useSerialLCD) || defined(useTWI4BitLCD)

#if defined(useAnalogButtons)
#define useAnalogRead true
#if defined(useTestButtonValues)
#define useTestAnalogButtonIdx true
#endif // defined(useTestButtonValues)
#endif // defined(useAnalogButtons)

#if defined(useTWIbuttons)
#define useTWIsupport true
#define useInterruptBasedTWI true
#endif // defined(useTWIbuttons)

#if defined(useSimulatedFIandVSS)
#define useTimer1Interrupt true
#endif // defined(useSimulatedFIandVSS)

#if defined(useTimer1Interrupt)
#define useTimer1 true
#endif // defined(useTimer1Interrupt)

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

#if defined(useDragRaceFunction)
#define useVehicleParameters true
#endif // defined(useDragRaceFunction)

#if defined(useSavedTrips)
#define useEEPROMtripStorage true
#endif // defined(useSavedTrips)

#if defined(useLegacyButtons) || defined(useAnalogButtons) || defined(useTWIbuttons) || defined(useDebugButtonInjection)
#define useButtonInput true
#endif // defined(useLegacyButtons) || defined(useAnalogButtons) || defined(useTWIbuttons) || defined(useDebugButtonInjection)

#if !defined(useLegacyButtons) && !defined(useAnalogButtons) && !defined(useTWIbuttons) && defined(useButtonInput)
#define useDebugButtons true
#endif // !defined(useLegacyButtons) && !defined(useAnalogButtons) && !defined(useTWIbuttons) && defined(useButtonInput)

// this section catches configuration errors

#if !defined(useTouchScreenInput) && !defined(useLegacyButtons) && !defined(useAnalogButtons) && !defined(useTWIbuttons) && !defined(useDebugButtonInjection)
#error *** MPGuino input configuration not specified!!! ***
#endif // !defined(useTouchScreenInput) && !defined(useLegacyButtons) && !defined(useAnalogButtons) && !defined(useTWIbuttons) && !defined(useDebugButtonInjection)

#if defined(useTWIsupport) && defined(useLegacyButtons) && !defined(__AVR_ATmega2560__)
#error *** CANNOT configure for Legacy pushbuttons and TwoWire support!!! ***
#endif // defined(useTWIsupport) && defined(useLegacyButtons) && !defined(__AVR_ATmega2560__)

#if ( defined(useLegacyButtons) + defined(useAnalogButtons) + defined(useTWIbuttons) ) > 1
#error *** Button hardware configuration error detected!!! ***
#endif // ( defined(useLegacyButtons) + defined(useAnalogButtons) + defined(useTWIbuttons) ) > 1

#if ( defined(useLCDserialPort0) + defined(useLoggingSerialPort0) + defined(useDebugTerminalSerialPort0) ) > 1
#error *** Serial Port 0 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort0) + defined(useLoggingSerialPort0) + defined(useDebugTerminalSerialPort0) ) > 1

#if ( defined(useLCDserialPort1) + defined(useLoggingSerialPort1) + defined(useDebugTerminalSerialPort1) ) > 1
#error *** Serial Port 1 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort1) + defined(useLoggingSerialPort1) + defined(useDebugTerminalSerialPort1) ) > 1

#if ( defined(useLCDserialPort2) + defined(useLoggingSerialPort2) + defined(useDebugTerminalSerialPort2) ) > 1
#error *** Serial Port 2 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort2) + defined(useLoggingSerialPort2) + defined(useDebugTerminalSerialPort2) ) > 1

#if ( defined(useLCDserialPort3) + defined(useLoggingSerialPort3) + defined(useDebugTerminalSerialPort3) ) > 1
#error *** Serial Port 3 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort3) + defined(useLoggingSerialPort3) + defined(useDebugTerminalSerialPort3) ) > 1

#if defined(useDebugTerminalUSBserial) && defined(useLoggingUSBserial)
#error *** USB port conflict exists between Logging Output and something else!!! ***
#endif // defined(useDebugTerminalUSBserial) && defined(useLoggingUSBserial)

#if defined(useJSONoutput) && defined(useDataLoggingOutput)
#error *** CANNOT use useJSONoutput and useDataLoggingOutput!!! ***
#endif // defined(useJSONoutput) && defined(useDataLoggingOutput)

#if defined(useSerialLCD)
#if ( defined(useLCDserialPort0) + defined(useLCDserialPort1) + defined(useLCDserialPort2) + defined(useLCDserialPort3) ) != 1
#error *** Serial LCD Output requires exactly one serial output port!!! ***
#endif // ( defined(useLCDserialPort0) + defined(useLCDserialPort1) + defined(useLCDserialPort2) + defined(useLCDserialPort3) ) != 1
#endif // defined(useSerialLCD)

#if defined(useDebugTerminal)
#if ( defined(useDebugTerminalSerialPort0) + defined(useDebugTerminalSerialPort1) + defined(useDebugTerminalSerialPort2) + defined(useDebugTerminalSerialPort3) + defined(useDebugTerminalUSBserial) ) != 1
#error *** Terminal Output requires exactly one I/O port!!! ***
#endif // ( defined(useDebugTerminalSerialPort0) + defined(useDebugTerminalSerialPort1) + defined(useDebugTerminalSerialPort2) + defined(useDebugTerminalSerialPort3) + defined(useDebugTerminalUSBserial) ) != 1
#endif // defined(useDebugTerminal)

#if ( defined(useLoggingSerialPort0) + defined(useLoggingSerialPort1) + defined(useLoggingSerialPort2) + defined(useLoggingSerialPort3) + defined(useLoggingUSBserial) ) != 1
#if defined(useJSONoutput)
#error *** JSON Output requires exactly one I/O port!!! ***
#endif // defined(useJSONoutput)
#if defined(useDataLoggingOutput)
#error *** Logging Output requires exactly one I/O port!!! ***
#endif // defined(useDataLoggingOutput)
#endif // ( defined(useLoggingSerialPort0) + defined(useLoggingSerialPort1) + defined(useLoggingSerialPort2) + defined(useLoggingSerialPort3) + defined(useLoggingUSBserial) ) != 1

#if defined(useUSBserial)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useUSBserial!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(__AVR_ATmega2560__)
#error *** Cannot use ATmega2560 hardware and useUSBserial!!! ***
#endif // defined(__AVR_ATmega2560__)
#endif // defined(useUSBserial)

#if defined(useSerial1Port)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useSerial1Port!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(useTinkerkitLCDmodule)
#error *** Serial Port 1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#endif // defined(useSerial1Port)

#if defined(__AVR_ATmega328P__)
#if defined(useSerial1Port) || defined(useSerial2Port) || defined(useSerial3Port)
#error *** Cannot use ATmega328/128 and configured serial port!!! ***
#endif // defined(useSerial1Port) || defined(useSerial2Port) || defined(useSerial3Port)
#endif // defined(__AVR_ATmega328P__)

#if defined(__AVR_ATmega32U4__)
#if defined(useSerial0Port) || defined(useSerial2Port) || defined(useSerial3Port)
#error *** Cannot use ATmega32U4 and configured serial port!!! ***
#endif // defined(useSerial0Port) || defined(useSerial2Port) || defined(useSerial3Port)
#endif // defined(__AVR_ATmega32U4__)

// Really, dcb? Really? You couldn't have used something else for LCD data bit 3?
#if defined(useLegacyLCD) && defined(__AVR_ATmega328P__) && defined(useActivityLED)
#error *** CANNOT use useLegacyLCD and useActivityLED!!! ***
#endif // defined(useLegacyLCD) && defined(__AVR_ATmega328P__) && defined(useActivityLED)

#if defined(useSoftwareClock) && defined(useDeepSleep)
#error *** CANNOT use both useSoftwareClock and useDeepSleep!!! ***
#endif // defined(useSoftwareClock) && defined(useDeepSleep)

#if defined(useCarVoltageOutput) && defined(useChryslerBaroSensor)
#error *** CANNOT use both useCarVoltageOutput and useChryslerBaroSensor!!! ***
#endif // defined(useCarVoltageOutput) && defined(useChryslerBaroSensor)

