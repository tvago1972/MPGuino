// the following options sets preconfigured board settings, which determine hardware port usage, hardware support checking, and processor speed
// if all of the below preconfigured board "#define"s are commented out, code will compile for an AVR microprocessor specified in either the Arduino IDE or the avr toolchain
//
//#define useLegacyBoard true					// sets LCD and button configuration for the original MPGuino circuit
//#define useJellyBeanDriverBoard true		// sets LCD and button configuration for the JBD version of MPGuino
//#define useArduinoMega2560 true			// sets specific LCD configuration for Arduino Mega2560 board
//#define useTinkerkitLCDmodule true		// sets specific LCD configuration for TinkerKit! LCD module 
//#define useMPGuinoColourTouch true			// sets hardware configuration for MPGuino Colour Touch

// the below output options may be chosen independently of any other MPGuino hardware
//   - none of the below options require an LCD or TFT display
//   - none of the below options require buttons or a touch screen
//   - useBluetooth provides for a basic I/O capability with an Android SMARTphone
//   - useDataLoggingOutput outputs 5 basic trip functions to a data logger or SD card
//   - useJSONoutput outputs selected trip functions as a JSON array of JSON objects
//   - useDebugTerminal provides for a PC-MPGuino debugging environment to test various basic MPGuino functions
//
//#define useDataLoggingOutput true			// output 5 basic trip functions to a data logger or SD card, once every refresh period (0.5 second)
#define useJSONoutput true					// skybolt added to enable and call JSON out routine
//#define useDebugTerminal true				// debugging terminal interface between PC and MPGuino
//#define useBluetooth true					// bluetooth interface with Android phone

// logging output port options
//   - if useDataLoggingOutput is not selected, the below output port options will be ignored
//   - if useDataLoggingOutput is selected, choose only one of the below output port options, or an error will result
//
//#define useLoggingSerialPort0 true			// select logging output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLoggingSerialPort1 true			// select logging output on serial port channel 1 (ATmega2560 board, Atmega32U4 board excluding TinkerKit! LCD module)
//#define useLoggingSerialPort2 true			// select logging output on serial port channel 2 (ATmega2560 board)
//#define useLoggingSerialPort3 true			// select logging output on serial port channel 3 (ATmega2560 board)
//#define useLoggingSerialUSB true			// select logging output on USB CDC output (TinkerKit! LCD module)

// JSON output port options
//   - if useJSONoutput is not selected, the below output port options will be ignored
//   - if useJSONoutput is selected, choose only one of the below output port options, or an error will result
//
#define useJSONserialPort0 true			// select JSON output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useJSONserialPort1 true			// select JSON output on serial port channel 1 (ATmega2560 board, Atmega32U4 board excluding TinkerKit! LCD module)
//#define useJSONserialPort2 true			// select JSON output on serial port channel 2 (ATmega2560 board)
//#define useJSONserialPort3 true			// select JSON output on serial port channel 3 (ATmega2560 board)
//#define useJSONserialUSB true			// select JSON output on USB CDC output (TinkerKit! LCD module)

// debug terminal I/O port options
//   - if useDebugTerminal is not selected, the below output port options will be ignored
//   - if useDebugTerminal is selected, choose only one of the below output port options, or an error will result
//
#define useDebugTerminalSerialPort0 true	// select serial port channel 0 for PC-MPGuino terminal interface (most Arduino boards, excluding TinkerKit! LCD module)
//#define useDebugTerminalSerialPort1 true	// select serial port channel 1 for PC-MPGuino terminal interface (ATmega2560 board, Atmega32U4 board excluding TinkerKit! LCD module)
//#define useDebugTerminalSerialPort2 true	// select serial port channel 2 for PC-MPGuino terminal interface (ATmega2560 board)
//#define useDebugTerminalSerialPort3 true	// select serial port channel 3 for PC-MPGuino terminal interface (ATmega2560 board)
//#define useDebugTerminalSerialUSB true		// select USB CDC output for PC-MPGuino terminal interface (TinkerKit! LCD module)

// Bluetooth output port options
//   - if useBluetooth is not selected, the below output port options will be ignored
//   - if useBluetooth is selected, choose only one of the below output port options, or an error will result
//
//#define useBluetoothSerialPort0 true		// select Bluetooth output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useBluetoothSerialPort1 true		// select Bluetooth output on serial port channel 1 (ATmega2560 board, Atmega32U4 board excluding TinkerKit! LCD module)
//#define useBluetoothSerialPort2 true		// select Bluetooth output on serial port channel 2 (ATmega2560 board)
//#define useBluetoothSerialPort3 true		// select Bluetooth output on serial port channel 3 (ATmega2560 board)
#define useBluetoothAdaFruitSPI true		// select Bluetooth output on SPI using AdaFruit Bluefruit LE Shield

// options for debugging terminal interface between PC and MPGuino
//   - mainly used to conserve space on ATmega328/128 and ATmega32U4 boards
//   - an LCD display option must be selected with useDebugButtonInjection, or a configuration error will result
//
//#define useDebugTerminalHelp true			// entering '?' in debug terminal displays brief help
//#define useDebugTerminalLabels true			// nice labels for various terminal interface output lists
//#define useDebugButtonInjection true		// ability to inject button presses into MPGuino
//#define useDebugCPUreading true				// Show enhanced CPU loading
//#define useSWEET64trace true				// (inw) Ability to view real-time 64-bit calculations from SWEET64 kernel - requires useDebugTerminal

// only one of the below LCD options may be chosen - choosing more than one will cause a compilation error to occur
//
// if either useTinkerkitLCDmodule or useMPGuinoColourTouch is used, the below options will be ignored
//
//#define useLegacyLCD true					// select Legacy 16x2 4-bit LCD
//#define useDFR0009LCD true						// (inw) select DFRobot DFR0009 LCD Keypad Shield
#define useAdafruitRGBLCDshield true		// select Adafruit RGB 16x2 4-bit LCD module over TWI
//#define useParallaxSerialLCDmodule true		// select Parallax 16x2 Serial LCD module
//#define useSainSmart2004LCD true			// select SainSmart 2004 20x4 4-bit LCD module over TWI
//#define useGenericTWILCD true				// select any 4-bit LCD module over TWI using a PCF8574 port expander

// the below defines determine the LCD character screen width and screen height. These settings are overridden by the above
// LCD device options, if specified.
//
#define LCDcharWidth 20
#define LCDcharHeight 2

// the below options only work if useParallaxSerialLCDmodule is selected. If useParallaxSerialLCDmodule is not selected, the below options will not be inserted at all
//
//#define useLCDserialPort0 true				// select serial LCD module output on serial port channel 0 (most Arduino boards, excluding TinkerKit! LCD module)
//#define useLCDserialPort1 true				// select serial LCD module output on serial port channel 1 (ATmega2560 board, Atmega32U4 board excluding TinkerKit! LCD module)
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
//#define useFuelCost true					// Show fuel cost
//#define useDragRaceFunction true			// Performs "drag race" 0-60 MPH, 1/4 mile time, estimated horsepower functionality
//#define useBigFE true						// Show big fuel economy displays
//#define useBigDTE true						// Show big distance-to-empty displays
//#define useBigTTE true						// Show big time-to-empty displays
//#define useBarFuelEconVsTime true			// Show Fuel Economy over Time bar graph
//#define useBarFuelEconVsSpeed true			// Show Fuel Economy vs Speed, Fuel Used vs Speed bar graphs
//#define useStatusMeter true					// displays a graphical meter for use with MPG display
//#define useSpiffyTripLabels true			// Ability to use enhanced trip labels on main display screens
//#define useSpiffyBigChars true				// Provides better number font with use with big number displays above
//#define useScreenEditor true				// Ability to change any of (9 or 12, depending on configuration) existing trip data screens, with 4 configurable figures on each screen
//#define useSoftwareClock true				// Shows 24 hour clock driven off of timer0, and provides a means to set it
//#define useCPUreading true					// Show CPU loading and available RAM usage
//#define useChryslerMAPCorrection true		// Ability to perform on-the-fly fuel injector data correction for late-model Chrysler vehicles
//#define useChryslerBaroSensor true			// allows use of a separate MAP sensor wired to MPGuino to read barometric pressure, for even more accurate correction
//#define useOutputPins true					// Generate analog 0-5VDC output voltage on expansion pins to drive LEDs or feed signal to external gauges
//#define blankScreenOnMessage true			// Completely blank display screen upon display of status message
//#define useImperialGallon true				// when selected, uses Imperial gallons instead of default US gallons
#define useCarVoltageOutput true			// Ability to display alternator voltage and optional secondary sensor (via meelis11)

//#define useDeepSleep true					// (inw) places MPGuino into deep sleep after activity timeout
//#define useCalculatedFuelFactor true		// (inw) Ability to calculate that pesky us/gal (or L) factor from easily available published fuel injector data

// performance enhancement options
//   - all may be chosen independently of one another
//   - if the performance enhancement option depends on a base option that is not selected, the performance enhancement is ignored
//
#define useLCDbufferedOutput true			// Speed up LCD output
#define useBluetoothBufferedOutput true		// speed up Bluetooth output on serial port
#define useLoggingBufferedOutput true		// speed up logging output on serial port
#define useJSONbufferedOutput true			// speed up JSON output on serial port
#define useDebugTerminalBufferedOutput true	// speed up debug terminal output on serial port
#define useWindowTripFilter true			// Smooths out "jumpy" instant FE figures that are caused by modern OBDII engine computers
#define useAssemblyLanguage true			// Speeds up many low-level MPGuino functions

// serial speed options
//   - if the corresponding serial port is not selected, serial speed option will be ignored
//
#define useLCDserialBaudRate 19200
#define useBluetoothSerialBaudRate 9600
#define useLoggingSerialBaudRate 9600
#define useJSONserialBaudRate 19200
#define useDebugTerminalSerialBaudRate 38400

// options that are in progress at the moment - don't enable them unless you want to test them
//
//#define useABresultViewer true				// (inw) Ability to graphically show current (B) versus stored (A) fuel consumption rates
//#define useCoastDownCalculator true			// (inw) Ability to calculate C(rr) and C(d) from coastdown
//#define useFuelParamCalculator true			// (inw) Ability to calculate microseconds per gallon and fuel injector delay stored parameters
//#define useRealTimeClockModule true			// (inw) Ability to read and display Realtime Clock data from an SPI RTC module

// other program measurement and debugging tools
//
//#define useTestButtonValues true			// Allows observation of button mapping
#define useSimulatedFIandVSS true			// forces simulation of VSS and fuel injector events
//#define useActivityLED true					// indicates when MPGuino is awake vs idle/asleep
//#define useDebugAnalog true					// forces ADC support to be compiled in, along with a dedicated analog screen
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
#define useBluetoothSerialPort1 true
#define useBluetoothSerialBaudRate 9600
#define useBluetoothBufferedOutput true
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
#undef useBluetoothSerialPort0
#undef useBluetoothSerialPort2
#undef useBluetoothSerialPort3
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

// major feature define configurations

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

// serial / USB / SPI initial define configurations

#if defined(useSerialLCD)
#if ( defined(useLCDserialPort0) + defined(useLCDserialPort1) + defined(useLCDserialPort2) + defined(useLCDserialPort3) ) != 1
#error *** Serial LCD Output requires exactly one serial output port!!! ***
#endif // ( defined(useLCDserialPort0) + defined(useLCDserialPort1) + defined(useLCDserialPort2) + defined(useLCDserialPort3) ) != 1
#define useBinaryLCDbrightness true
#else // defined(useSerialLCD)
#undef useLCDserialPort0
#undef useLCDserialPort1
#undef useLCDserialPort2
#undef useLCDserialPort3
#undef useLCDserialBaudRate
#endif // defined(useSerialLCD)

#if defined(useBluetooth)
#if ( defined(useBluetoothSerialPort0) + defined(useBluetoothSerialPort1) + defined(useBluetoothSerialPort2) + defined(useBluetoothSerialPort3) + defined(useBluetoothAdaFruitSPI) ) != 1
#error *** Bluetooth interface requires exactly one output port!!! ***
#endif // ( defined(useBluetoothSerialPort0) + defined(useBluetoothSerialPort1) + defined(useBluetoothSerialPort2) + defined(useBluetoothSerialPort3) + defined(useBluetoothAdaFruitSPI) ) != 1
#else // defined(useBluetooth)
#undef useBluetoothSerialPort0
#undef useBluetoothSerialPort1
#undef useBluetoothSerialPort2
#undef useBluetoothSerialPort3
#undef useBluetoothSerialBaudRate
#undef useBluetoothAdaFruitSPI
#undef useBluetoothBufferedOutput
#endif // defined(useBluetooth)

#if defined(useDataLoggingOutput)
#if ( defined(useLoggingSerialPort0) + defined(useLoggingSerialPort1) + defined(useLoggingSerialPort2) + defined(useLoggingSerialPort3) + defined(useLoggingSerialUSB) ) != 1
#error *** Logging Output requires exactly one I/O port!!! ***
#endif // ( defined(useLoggingSerialPort0) + defined(useLoggingSerialPort1) + defined(useLoggingSerialPort2) + defined(useLoggingSerialPort3) + defined(useLoggingSerialUSB) ) != 1
#else // defined(useDataLoggingOutput)
#undef useLoggingSerialPort0
#undef useLoggingSerialPort1
#undef useLoggingSerialPort2
#undef useLoggingSerialPort3
#undef useLoggingSerialBaudRate
#undef useLoggingSerialUSB
#undef useLoggingBufferedOutput
#endif // defined(useDataLoggingOutput)

#if defined(useJSONoutput)
#if ( defined(useJSONserialPort0) + defined(useJSONserialPort1) + defined(useJSONserialPort2) + defined(useJSONserialPort3) + defined(useJSONserialUSB) ) != 1
#error *** JSON Output requires exactly one I/O port!!! ***
#endif // ( defined(useJSONserialPort0) + defined(useJSONserialPort1) + defined(useJSONserialPort2) + defined(useJSONserialPort3) + defined(useJSONserialUSB) ) != 1
#else // defined(useJSONoutput)
#undef useJSONserialPort0
#undef useJSONserialPort1
#undef useJSONserialPort2
#undef useJSONserialPort3
#undef useJSONserialBaudRate
#undef useJSONserialUSB
#undef useJSONbufferedOutput
#endif // defined(useJSONoutput)

#if defined(useDebugTerminal)
#if ( defined(useDebugTerminalSerialPort0) + defined(useDebugTerminalSerialPort1) + defined(useDebugTerminalSerialPort2) + defined(useDebugTerminalSerialPort3) + defined(useDebugTerminalSerialUSB) ) != 1
#error *** Debug Terminal output requires exactly one I/O port!!! ***
#endif // ( defined(useDebugTerminalSerialPort0) + defined(useDebugTerminalSerialPort1) + defined(useDebugTerminalSerialPort2) + defined(useDebugTerminalSerialPort3) + defined(useDebugTerminalSerialUSB) ) != 1
#else // defined(useDebugTerminal)
#undef useDebugTerminalSerialPort0
#undef useDebugTerminalSerialPort1
#undef useDebugTerminalSerialPort2
#undef useDebugTerminalSerialPort3
#undef useDebugTerminalSerialBaudRate
#undef useDebugTerminalSerialUSB
#undef useDebugTerminalBufferedOutput
#undef useDebugButtonInjection
#undef useDebugTerminalLabels
#undef useDebugTerminalHelp
#undef useDebugCPUreading
#undef useSWEET64trace
#endif // defined(useDebugTerminal)

#if defined(useLCDserialPort0)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useLCDserialPort0!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if !defined(useLCDserialBaudRate)
#error *** useLCDserialPort0 requires useLCDserialBaudRate!!! ***
#endif // !defined(useLCDserialBaudRate)
#define useSerial0Port true
#define serial0BaudRate useLCDserialBaudRate
#define devLCDserial devSerial0
#define LCDserialPort serial0
#if defined(useLCDbufferedOutput)
#undef useLCDbufferedOutput
#define useBufferedSerial0Port true
#define LCDserialBuffer serial0Buffer
#endif // defined(useLCDbufferedOutput)
#endif // defined(useLCDserialPort0)

#if defined(useLCDserialPort1)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useLCDserialPort1!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(useTinkerkitLCDmodule)
#error *** useLCDserialPort1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#if !defined(useLCDserialBaudRate)
#error *** useLCDserialPort1 requires useLCDserialBaudRate!!! ***
#endif // !defined(useLCDserialBaudRate)
#define useSerial1Port true
#define serial1BaudRate useLCDserialBaudRate
#define devLCDserial devSerial1
#define LCDserialPort serial1
#if defined(useLCDbufferedOutput)
#undef useLCDbufferedOutput
#define useBufferedSerial1Port true
#define LCDserialBuffer serial1Buffer
#endif // defined(useLCDbufferedOutput)
#endif // defined(useLCDserialPort1)

#if defined(useLCDserialPort2)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useLCDserialPort2!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useLCDserialPort2!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useLCDserialBaudRate)
#error *** useLCDserialPort2 requires useLCDserialBaudRate!!! ***
#endif // !defined(useLCDserialBaudRate)
#define useSerial2Port true
#define serial2BaudRate useLCDserialBaudRate
#define devLCDserial devSerial2
#define LCDserialPort serial2
#if defined(useLCDbufferedOutput)
#undef useLCDbufferedOutput
#define useBufferedSerial2Port true
#define LCDserialBuffer serial2Buffer
#endif // defined(useLCDbufferedOutput)
#endif // defined(useLCDserialPort2)

#if defined(useLCDserialPort3)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useLCDserialPort3!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useLCDserialPort3!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useLCDserialBaudRate)
#error *** useLCDserialPort3 requires useLCDserialBaudRate!!! ***
#endif // !defined(useLCDserialBaudRate)
#define useSerial3Port true
#define serial3BaudRate useLCDserialBaudRate
#define devLCDserial devSerial3
#define LCDserialPort serial3
#if defined(useLCDbufferedOutput)
#undef useLCDbufferedOutput
#define useBufferedSerial3Port true
#define LCDserialBuffer serial3Buffer
#endif // defined(useLCDbufferedOutput)
#endif // defined(useLCDserialPort3)

#if defined(useBluetoothSerialPort0)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useBluetoothSerialPort0!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if !defined(useBluetoothSerialBaudRate)
#error *** useBluetoothSerialPort0 requires useBluetoothSerialBaudRate!!! ***
#endif // !defined(useBluetoothSerialBaudRate)
#define useSerial0Port true
#define useSerial0PortInput true
#define serial0BaudRate useBluetoothSerialBaudRate
#define devBluetooth devSerial0
#if defined(useBluetoothBufferedOutput)
#define useBufferedSerial0Port true
#define bluetoothSerialBuffer serial0Buffer
#endif // defined(useBluetoothBufferedOutput)
#endif // defined(useBluetoothSerialPort0)

#if defined(useBluetoothSerialPort1)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useBluetoothSerialPort1!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(useTinkerkitLCDmodule)
#error *** useBluetoothSerialPort1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#if !defined(useBluetoothSerialBaudRate)
#error *** useBluetoothSerialPort1 requires useBluetoothSerialBaudRate!!! ***
#endif // !defined(useBluetoothSerialBaudRate)
#define useSerial1Port true
#define useSerial1PortInput true
#define serial1BaudRate useBluetoothSerialBaudRate
#define devBluetooth devSerial1
#if defined(useBluetoothBufferedOutput)
#define useBufferedSerial1Port true
#define bluetoothSerialBuffer serial1Buffer
#endif // defined(useBluetoothBufferedOutput)
#endif // defined(useBluetoothSerialPort1)

#if defined(useBluetoothSerialPort2)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useBluetoothSerialPort2!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useBluetoothSerialPort2!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useBluetoothSerialBaudRate)
#error *** useBluetoothSerialPort2 requires useBluetoothSerialBaudRate!!! ***
#endif // !defined(useBluetoothSerialBaudRate)
#define useSerial2Port true
#define useSerial2PortInput true
#define serial2BaudRate useBluetoothSerialBaudRate
#define devBluetooth devSerial2
#if defined(useBluetoothBufferedOutput)
#define useBufferedSerial2Port true
#define bluetoothSerialBuffer serial2Buffer
#endif // defined(useBluetoothBufferedOutput)
#endif // defined(useBluetoothSerialPort2)

#if defined(useBluetoothSerialPort3)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useBluetoothSerialPort3!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useBluetoothSerialPort3!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useBluetoothSerialBaudRate)
#error *** useBluetoothSerialPort3 requires useBluetoothSerialBaudRate!!! ***
#endif // !defined(useBluetoothSerialBaudRate)
#define useSerial3Port true
#define useSerial3PortInput true
#define serial3BaudRate useBluetoothSerialBaudRate
#define devBluetooth devSerial3
#if defined(useBluetoothBufferedOutput)
#define useBufferedSerial3Port true
#define bluetoothSerialBuffer serial3Buffer
#endif // defined(useBluetoothBufferedOutput)
#endif // defined(useBluetoothSerialPort3)

#if defined(useBluetoothAdaFruitSPI)
#define useHardwareSPI true
#endif // defined(useBluetoothAdaFruitSPI)

#if defined(useLoggingSerialPort0)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useLoggingSerialPort0!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if !defined(useLoggingSerialBaudRate)
#error *** useLoggingSerialPort0 requires useLoggingSerialBaudRate!!! ***
#endif // !defined(useLoggingSerialBaudRate)
#define useSerial0Port true
#define serial0BaudRate useLoggingSerialBaudRate
#define devLogOutput devSerial0
#define outputLoggingSplash true
#if defined(useLoggingBufferedOutput)
#define useBufferedSerial0Port true
#endif // defined(useLoggingBufferedOutput)
#endif // defined(useLoggingSerialPort0)

#if defined(useLoggingSerialPort1)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useLoggingSerialPort1!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(useTinkerkitLCDmodule)
#error *** useLoggingSerialPort1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#if !defined(useLoggingSerialBaudRate)
#error *** useLoggingSerialPort1 requires useLoggingSerialBaudRate!!! ***
#endif // !defined(useLoggingSerialBaudRate)
#define useSerial1Port true
#define serial1BaudRate useLoggingSerialBaudRate
#define devLogOutput devSerial1
#define outputLoggingSplash true
#if defined(useLoggingBufferedOutput)
#define useBufferedSerial1Port true
#endif // defined(useLoggingBufferedOutput)
#endif // defined(useLoggingSerialPort1)

#if defined(useLoggingSerialPort2)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useLoggingSerialPort2!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useLoggingSerialPort2!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useLoggingSerialBaudRate)
#error *** useLoggingSerialPort2 requires useLoggingSerialBaudRate!!! ***
#endif // !defined(useLoggingSerialBaudRate)
#define useSerial2Port true
#define serial2BaudRate useLoggingSerialBaudRate
#define devLogOutput devSerial2
#define outputLoggingSplash true
#if defined(useLoggingBufferedOutput)
#define useBufferedSerial2Port true
#endif // defined(useLoggingBufferedOutput)
#endif // defined(useLoggingSerialPort2)

#if defined(useLoggingSerialPort3)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useLoggingSerialPort3!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useLoggingSerialPort3!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useLoggingSerialBaudRate)
#error *** useLoggingSerialPort3 requires useLoggingSerialBaudRate!!! ***
#endif // !defined(useLoggingSerialBaudRate)
#define useSerial3Port true
#define serial3BaudRate useLoggingSerialBaudRate
#define devLogOutput devSerial3
#define outputLoggingSplash true
#if defined(useLoggingBufferedOutput)
#define useBufferedSerial3Port true
#endif // defined(useLoggingBufferedOutput)
#endif // defined(useLoggingSerialPort3)

#if defined(useLoggingSerialUSB)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useLoggingSerialUSB!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(__AVR_ATmega2560__)
#error *** Cannot use ATmega2560 hardware and useLoggingSerialUSB!!! ***
#endif // defined(__AVR_ATmega2560__)
#define useUSBserial true
#define devLogOutput devUSB
#define outputLoggingSplash true
#endif // defined(useLoggingSerialUSB)

#if defined(useJSONserialPort0)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useJSONserialPort0!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if !defined(useJSONserialBaudRate)
#error *** useJSONserialPort0 requires useJSONserialBaudRate!!! ***
#endif // !defined(useJSONserialBaudRate)
#define useSerial0Port true
#define serial0BaudRate useJSONserialBaudRate
#define devJSONoutput devSerial0
#if defined(useJSONbufferedOutput)
#define useBufferedSerial0Port true
#endif // defined(useJSONbufferedOutput)
#endif // defined(useJSONserialPort0)

#if defined(useJSONserialPort1)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useJSONserialPort1!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(useTinkerkitLCDmodule)
#error *** useJSONserialPort1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#if !defined(useJSONserialBaudRate)
#error *** useJSONserialPort1 requires useJSONserialBaudRate!!! ***
#endif // !defined(useJSONserialBaudRate)
#define useSerial1Port true
#define serial1BaudRate useJSONserialBaudRate
#define devJSONoutput devSerial1
#if defined(useJSONbufferedOutput)
#define useBufferedSerial1Port true
#endif // defined(useJSONbufferedOutput)
#endif // defined(useJSONserialPort1)

#if defined(useJSONserialPort2)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useJSONserialPort2!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useJSONserialPort2!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useJSONserialBaudRate)
#error *** useJSONserialPort2 requires useJSONserialBaudRate!!! ***
#endif // !defined(useJSONserialBaudRate)
#define useSerial2Port true
#define serial2BaudRate useJSONserialBaudRate
#define devJSONoutput devSerial2
#if defined(useJSONbufferedOutput)
#define useBufferedSerial2Port true
#endif // defined(useJSONbufferedOutput)
#endif // defined(useJSONserialPort2)

#if defined(useJSONserialPort3)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useJSONserialPort3!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useJSONserialPort3!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useJSONserialBaudRate)
#error *** useJSONserialPort3 requires useJSONserialBaudRate!!! ***
#endif // !defined(useJSONserialBaudRate)
#define useSerial3Port true
#define serial3BaudRate useJSONserialBaudRate
#define devJSONoutput devSerial3
#if defined(useJSONbufferedOutput)
#define useBufferedSerial3Port true
#endif // defined(useJSONbufferedOutput)
#endif // defined(useJSONserialPort3)

#if defined(useJSONserialUSB)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useJSONserialUSB!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(__AVR_ATmega2560__)
#error *** Cannot use ATmega2560 hardware and useJSONserialUSB!!! ***
#endif // defined(__AVR_ATmega2560__)
#define useUSBserial true
#define devJSONoutput devUSB
#endif // defined(useJSONserialUSB)

#if defined(useDebugTerminalSerialPort0)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useDebugTerminalSerialPort0!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if !defined(useDebugTerminalSerialBaudRate)
#error *** useDebugTerminalSerialPort0 requires useDebugTerminalSerialBaudRate!!! ***
#endif // !defined(useDebugTerminalSerialBaudRate)
#define useSerial0Port true
#define useSerial0PortInput true
#define serial0BaudRate useDebugTerminalSerialBaudRate
#define devDebugTerminal devSerial0
#define outputDebugTerminalSplash true
#if defined(useDebugTerminalBufferedOutput)
#define useBufferedSerial0Port true
#endif // defined(useDebugTerminalBufferedOutput)
#endif // defined(useDebugTerminalSerialPort0)

#if defined(useDebugTerminalSerialPort1)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useDebugTerminalSerialPort1!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(useTinkerkitLCDmodule)
#error *** useDebugTerminalSerialPort1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#if !defined(useDebugTerminalSerialBaudRate)
#error *** useDebugTerminalSerialPort1 requires useDebugTerminalSerialBaudRate!!! ***
#endif // !defined(useDebugTerminalSerialBaudRate)
#define useSerial1Port true
#define useSerial1PortInput true
#define serial1BaudRate useDebugTerminalSerialBaudRate
#define devDebugTerminal devSerial1
#define outputDebugTerminalSplash true
#if defined(useDebugTerminalBufferedOutput)
#define useBufferedSerial1Port true
#endif // defined(useDebugTerminalBufferedOutput)
#endif // defined(useDebugTerminalSerialPort1)

#if defined(useDebugTerminalSerialPort2)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useDebugTerminalSerialPort2!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useDebugTerminalSerialPort2!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useDebugTerminalSerialBaudRate)
#error *** useDebugTerminalSerialPort2 requires useDebugTerminalSerialBaudRate!!! ***
#endif // !defined(useDebugTerminalSerialBaudRate)
#define useSerial2Port true
#define useSerial2PortInput true
#define serial2BaudRate useDebugTerminalSerialBaudRate
#define devDebugTerminal devSerial2
#define outputDebugTerminalSplash true
#if defined(useDebugTerminalBufferedOutput)
#define useBufferedSerial2Port true
#endif // defined(useDebugTerminalBufferedOutput)
#endif // defined(useDebugTerminalSerialPort2)

#if defined(useDebugTerminalSerialPort3)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and useDebugTerminalSerialPort3!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and useDebugTerminalSerialPort3!!! ***
#endif // defined(__AVR_ATmega328P__)
#if !defined(useDebugTerminalSerialBaudRate)
#error *** useDebugTerminalSerialPort3 requires useDebugTerminalSerialBaudRate!!! ***
#endif // !defined(useDebugTerminalSerialBaudRate)
#define useSerial3Port true
#define useSerial3PortInput true
#define serial3BaudRate useDebugTerminalSerialBaudRate
#define devDebugTerminal devSerial3
#define outputDebugTerminalSplash true
#if defined(useDebugTerminalBufferedOutput)
#define useBufferedSerial3Port true
#endif // defined(useDebugTerminalBufferedOutput)
#endif // defined(useDebugTerminalSerialPort3)

#if defined(useDebugTerminalSerialUSB)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useDebugTerminalSerialUSB!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(__AVR_ATmega2560__)
#error *** Cannot use ATmega2560 hardware and useDebugTerminalSerialUSB!!! ***
#endif // defined(__AVR_ATmega2560__)
#define useUSBserial true
#define devDebugTerminal devUSB
#define outputDebugTerminalSplash true
#endif // defined(useDebugTerminalSerialUSB)

#if ( defined(useLCDserialPort0) + defined(useLoggingSerialPort0) + defined(useJSONserialPort0) + defined(useDebugTerminalSerialPort0) + defined(useBluetoothSerialPort0) ) > 1
#error *** Serial Port 0 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort0) + defined(useLoggingSerialPort0) + defined(useJSONserialPort0) + defined(useDebugTerminalSerialPort0) + defined(useBluetoothSerialPort0) ) > 1

#if ( defined(useLCDserialPort1) + defined(useLoggingSerialPort1) + defined(useJSONserialPort1) + defined(useDebugTerminalSerialPort1) + defined(useBluetoothSerialPort1) ) > 1
#error *** Serial Port 1 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort1) + defined(useLoggingSerialPort1) + defined(useJSONserialPort1) + defined(useDebugTerminalSerialPort1) + defined(useBluetoothSerialPort1) ) > 1

#if ( defined(useLCDserialPort2) + defined(useLoggingSerialPort2) + defined(useJSONserialPort2) + defined(useDebugTerminalSerialPort2) + defined(useBluetoothSerialPort2) ) > 1
#error *** Serial Port 2 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort2) + defined(useLoggingSerialPort2) + defined(useJSONserialPort2) + defined(useDebugTerminalSerialPort2) + defined(useBluetoothSerialPort2) ) > 1

#if ( defined(useLCDserialPort3) + defined(useLoggingSerialPort3) + defined(useJSONserialPort3) + defined(useDebugTerminalSerialPort3) + defined(useBluetoothSerialPort3) ) > 1
#error *** Serial Port 3 assignment conflict exists!!! ***
#endif // ( defined(useLCDserialPort3) + defined(useLoggingSerialPort3) + defined(useJSONserialPort3) + defined(useDebugTerminalSerialPort3) + defined(useBluetoothSerialPort3) ) > 1

#if defined(useBufferedSerial0Port) || defined(useBufferedSerial1Port) || defined(useBufferedSerial2Port) || defined(useBufferedSerial3Port) \
	|| defined(useSerial0PortInput) || defined(useSerial1PortInput) || defined(useSerial2PortInput) || defined(useSerial3PortInput) \
	|| defined(useLCDbufferedOutput) \
	|| defined(useUSBserial)
#define useBuffering true
#endif

// hardware input define configurations

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

#if defined(useTWIsupport) && defined(useLegacyButtons) && !defined(__AVR_ATmega2560__)
#error *** CANNOT configure for Legacy pushbuttons and TwoWire support!!! ***
#endif // defined(useTWIsupport) && defined(useLegacyButtons) && !defined(__AVR_ATmega2560__)

#if ( defined(useLegacyButtons) + defined(useAnalogButtons) + defined(useTWIbuttons) ) > 1
#error *** Button hardware configuration error detected!!! ***
#endif // ( defined(useLegacyButtons) + defined(useAnalogButtons) + defined(useTWIbuttons) ) > 1

#if defined(useUSBserial)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 hardware and useUSBserial!!! ***
#endif // defined(__AVR_ATmega328P__)
#if defined(__AVR_ATmega2560__)
#error *** Cannot use ATmega2560 hardware and useUSBserial!!! ***
#endif // defined(__AVR_ATmega2560__)
#endif // defined(useUSBserial)

#if defined(useSerial0Port)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and Serial Port 0!!! ***
#endif // defined(__AVR_ATmega32U4__)
#endif // defined(useSerial0Port)

#if defined(useSerial1Port)
#if defined(useTinkerkitLCDmodule)
#error *** Serial Port 1 on TinkerKit! LCD module conflicts with fuel injector sense ports!!! ***
#endif // defined(useTinkerkitLCDmodule)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and Serial Port 1!!! ***
#endif // defined(__AVR_ATmega328P__)
#endif // defined(useSerial1Port)

#if defined(useSerial2Port)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and Serial Port 2!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and Serial Port 2!!! ***
#endif // defined(__AVR_ATmega328P__)
#endif // defined(useSerial2Port)

#if defined(useSerial3Port)
#if defined(__AVR_ATmega32U4__)
#error *** Cannot use ATmega32U4 and Serial Port 3!!! ***
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega328P__)
#error *** Cannot use ATmega328/128 and Serial Port 3!!! ***
#endif // defined(__AVR_ATmega328P__)
#endif // defined(useSerial3Port)

// Really, dcb? Really? You couldn't have used something else for LCD data bit 3?
#if defined(useLegacyLCD) && defined(__AVR_ATmega328P__) && ( defined(useActivityLED) || defined(useHardwareSPI) )
#error *** CANNOT use useLegacyLCD and useActivityLED!!! ***
#endif // defined(useLegacyLCD) && defined(__AVR_ATmega328P__) && ( defined(useActivityLED) || defined(useHardwareSPI) )

#if defined(useSoftwareClock) && defined(useDeepSleep)
#error *** CANNOT use both useSoftwareClock and useDeepSleep!!! ***
#endif // defined(useSoftwareClock) && defined(useDeepSleep)

#if defined(useCarVoltageOutput) && defined(useChryslerBaroSensor)
#error *** CANNOT use both useCarVoltageOutput and useChryslerBaroSensor!!! ***
#endif // defined(useCarVoltageOutput) && defined(useChryslerBaroSensor)

#if defined(useHardwareSPI)
#if defined(__AVR_ATmega328P__) && ( defined(useOutputPins) || defined(useActivityLED) )
#error *** Conflict exists between useHardwareSPI and useOutputPins / useActivityLED!!! ***
#endif // defined(__AVR_ATmega328P__) && ( defined(useOutputPins) || defined(useActivityLED) )
#endif // defined(useHardwareSPI)

