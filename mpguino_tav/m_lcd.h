#if defined(useLCDoutput)
/*

Currently, this module is capable of high speed output to any HD44780 based LCD display. Provisions are made for the
following methods of display driving:

* Directly via 4-bit interface using the assigned ATmega processor/board combo specified in the Arduino IDE
* 4-bit interface over TWI-connected MCP23017 chip
* Character interface via Serial UART
* Character interface via TWI

The 4-bit interface configurations include an optional LCD character buffer which helps speed up processing.
The Serual UART character interface can optionally include a serial UART buffer.
The TWI character interface is already buffered within the TWI module.

*/
namespace LCD /* LCD hardware support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static void writeData(uint8_t value);
	static void setBrightness(uint8_t idx);
#if defined(useLCDcontrast)
	static void setContrast(uint8_t idx);
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	static void setRGBcolor(uint8_t idx);
#endif // defined(useAdafruitRGBLCDshield)
#if defined(useLCDfonts)
	static void loadCGRAMfont(const char * fontPtr);
#endif // defined(useLCDfonts)
#if defined(useLCDgraphics)
	static void loadCGRAMcharacter(uint8_t chr, char * chrData);
	static void writeCGRAMbyte(uint8_t cgrAddress, uint8_t chr);
	static uint8_t peekCGRAMbyte(uint8_t cgrAddress);
	static void flushCGRAM(void);
#endif // defined(useLCDgraphics)
#if defined(use4BitLCD)
	static void writeCommand(uint8_t value);
	static void writeByte(uint8_t value, uint8_t flags, uint8_t delay);
	static void writeNybble(uint8_t value, uint8_t flags);
	static void outputNybble(uint8_t s);
#endif // defined(use4BitLCD)

};

// these flags provide flow control for the LCD::writeData character output routine
static const uint8_t lcdCharGotoXY =	0b00000010;
static const uint8_t lcdCharOutput =	0b00000001;

static uint8_t lcdAddr3ssX;
static uint8_t lcdAddr3ssY;

static volatile unsigned int lcdDelayCount;

#if defined(useLCDgraphics)
static const uint8_t cgramFlagDirty =		0b10000000;

static uint8_t CGRAMbuffer[64]; // used by LCD output routine

#endif // defined(useLCDgraphics)
#if defined(use4BitLCD)
#if defined(usePort4BitLCD)
#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
static const uint8_t lcdDirection =			(1 << PORTF0); // on PORTF, A5, Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
static const uint8_t lcdRegisterSelect =	(1 << PORTF1); // on PORTF, A4
static const uint8_t lcdEnable =			(1 << PORTE6); // on PORTE, 7
static const uint8_t lcdBit3 =				(1 << PORTB4); // on PORTB, 8
static const uint8_t lcdBit2 =				(1 << PORTD6); // on PORTD, 12
static const uint8_t lcdBit1 =				(1 << PORTD4); // on PORTD, 4
static const uint8_t lcdBit0 =				(1 << PORTF4); // on PORTF, A3
static const uint8_t lcdBrightness =		(1 << DDB6); // on PORTB, 10
static const uint8_t lcdContrast =			(1 << DDB5); // on PORTB, 9

#endif // defined(useTinkerkitLCDmodule)
// any pin definitions for any other ATmega32U4 board goes here

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
static const uint8_t lcdRegisterSelect =	(1 << PORTA4); // on PORTA, 26
static const uint8_t lcdEnable =			(1 << PORTA5); // on PORTA, 27
static const uint8_t lcdBit3 =				(1 << PORTA0); // on PORTA, 22
static const uint8_t lcdBit2 =				(1 << PORTA1); // on PORTA, 23
static const uint8_t lcdBit1 =				(1 << PORTA2); // on PORTA, 24
static const uint8_t lcdBit0 =				(1 << PORTA3); // on PORTA, 25
static const uint8_t lcdBrightness =		(1 << DDB5); // on PORTB, 11 (OC1A)
static const uint8_t lcdContrast =			(1 << DDB7); // on PORTB, 13 (OC0A)

#endif // defined(useArduinoMega2560)
// any pin definitions for any other ATmega2560 board goes here

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#if defined(useLegacyLCD)
static const uint8_t lcdRegisterSelect =	(1 << PORTD4); // on PORTD, 4
static const uint8_t lcdEnable =			(1 << PORTD5); // on PORTD, 5
static const uint8_t lcdBit3 =				(1 << PORTB5); // on PORTB, 13
static const uint8_t lcdBit2 =				(1 << PORTB4); // on PORTB, 12
static const uint8_t lcdBit1 =				(1 << PORTB0); // on PORTB, 8
static const uint8_t lcdBit0 =				(1 << PORTD7); // on PORTD, 7
static const uint8_t lcdBrightness =		(1 << DDB1); // on PORTB, 9 (OC1A)
static const uint8_t lcdContrast =			(1 << DDD6); // on PORTD, 6 (OC0A)

#endif // defined(useLegacyLCD)
#if defined(useDFR0009LCD)
static const uint8_t lcdRegisterSelect =	(1 << PORTB0); // on PORTB, 8
static const uint8_t lcdEnable =			(1 << PORTB1); // on PORTB, 9
static const uint8_t lcdBit3 =				(1 << PORTD4); // on PORTD, 4
static const uint8_t lcdBit2 =				(1 << PORTD5); // on PORTD, 5
static const uint8_t lcdBit1 =				(1 << PORTD6); // on PORTD, 6
static const uint8_t lcdBit0 =				(1 << PORTD7); // on PORTD, 7
static const uint8_t lcdBrightness =		(1 << DDB2); // on PORTB, 10 (OC1B)
static const uint8_t lcdContrast =			0; // not used

#endif // defined(useDFR0009LCD)
// any pin definitions for any other ATmega168/328/328P board goes here

#endif // defined(__AVR_ATmega328P__)
#endif // defined(usePort4BitLCD)
#if defined(useTWI4BitLCD)
static volatile uint8_t portLCD; // LCD port register expander byte
#if defined(useAdafruitRGBLCDshield)
static volatile uint8_t portSwitches; // contains two out of the three LCD backlighting LED pins

static const uint8_t lcdDirection =			0b01000000; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
static const uint8_t lcdRegisterSelect =	0b10000000; // GPIO B
static const uint8_t lcdEnable =			0b00100000; // GPIO B
static const uint8_t lcdBit3 =				0b00000010; // GPIO B
static const uint8_t lcdBit2 =				0b00000100; // GPIO B
static const uint8_t lcdBit1 =				0b00001000; // GPIO B
static const uint8_t lcdBit0 =				0b00010000; // GPIO B
static const uint8_t lcdBrightnessRed =		0b01000000; // GPIO A
static const uint8_t lcdBrightnessGreen =	0b10000000; // GPIO A
static const uint8_t lcdBrightnessBlue =	0b00000001; // GPIO B

static const uint8_t RGBcolors[8] PROGMEM =
{
	0b11000001,	// off
	0b01000001,	// green
	0b10000001,	// red
	0b00000001,	// yeller
	0b11000000,	// blue
	0b01000000,	// cyan
	0b10000000,	// magenta
	0b00000000,	// white
};

#endif // defined(useAdafruitRGBLCDshield)
#if defined(useSainSmart2004LCD) || defined(useGenericTWILCD)

// these definitions are for any TWI LCD module using a PCF8574 port expander

static const uint8_t lcdDirection =			0b00000010; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
static const uint8_t lcdRegisterSelect =	0b00000001;
static const uint8_t lcdEnable =			0b00000100;
static const uint8_t lcdBit3 =				0b10000000;
static const uint8_t lcdBit2 =				0b01000000;
static const uint8_t lcdBit1 =				0b00100000;
static const uint8_t lcdBit0 =				0b00010000;
static const uint8_t lcdBrightness =		0b00001000;

#endif // defined(useSainSmart2004LCD) || defined(useGenericTWILCD)
#endif // defined(useTWI4BitLCD)
static const uint8_t lcdNullValue =					0b00000000;

static const uint8_t lcdClearDisplay =				0b00000001;

static const uint8_t lcdReturnHome =				0b00000010;

static const uint8_t lcdEntryModeSet =				0b00000100;
static const uint8_t lcdEMSincrement =				0b00000010;		// 1 = increment, 0 = decrement
static const uint8_t lcdEMSsetDisplayShift =		0b00000001;		// 1 = display shift, 0 = no display shift

static const uint8_t lcdDisplayControl =			0b00001000;
static const uint8_t lcdDCdisplayShow =				0b00000100;		// 1 = enable display, 0 = disable display
static const uint8_t lcdDCcursorControl =			0b00000010;		// 1 = cursor on, 0 = cursor off
static const uint8_t lcdDCcursorBlinkControl =		0b00000001;		// 1 = cursor blink, 0 = cursor steady

static const uint8_t lcdShift =						0b00010000;
static const uint8_t lcdSdisplayShift =				0b00001000;		// 1 = shift display, 0 = cursor move
static const uint8_t lcdSdirection =				0b00000100;		// 1 = move right, 0 = move left

static const uint8_t lcdFunctionSet =				0b00100000;
static const uint8_t lcdFSdataLength =				0b00010000;		// 1 = 8 bit data, 0 = 4 bit data
static const uint8_t lcdFSnumberOfLines =			0b00001000;		// 1 = 2 lines, 0 = 1 line
static const uint8_t lcdFScharacterFont =			0b00000100;		// 1 = 5x10 dot character font, 0 = 5x8 dot character font

static const uint8_t lcdSetCGRAMaddress =			0b01000000;

static const uint8_t lcdSetDDRAMaddress =			0b10000000;

// these flags tell LCD::writeData whether to output the passed in character, and how to handle the character if it is output
static const uint8_t lcdSendNybble =				0b00001000;
static const uint8_t lcdDataByte =					0b00000100;
static const uint8_t lcdCommandByte =				0b00000000;

// these flags tell LCD::writeData what kind of delay is associated with the character
static const uint8_t lcdDelay0015ms =				0x03;
static const uint8_t lcdDelay4100us =				0x02;
static const uint8_t lcdDelay0100us =				0x01;
static const uint8_t lcdDelay0040us =				0x00;
static const uint8_t lcdDelayFlags =				lcdDataByte | 0x03;
static const uint8_t lcdSendFlags =					lcdSendNybble | 0x03;

static uint8_t LCDgotoXYaddress;

static const uint8_t lcdBaseYposition[] PROGMEM = {
	lcdSetDDRAMaddress,
	lcdSetDDRAMaddress | 0x40,
	lcdSetDDRAMaddress | 0x14,
	lcdSetDDRAMaddress | 0x54,
};

static const uint8_t lcdDisplayModes[] PROGMEM = {
	lcdDisplayControl,																		// turn off display
	lcdDisplayControl | lcdDCdisplayShow,													// turn on display, no cursor, no character blink (default)
	lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorBlinkControl,							// turn on display, no cursor, with character blink
	lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorControl,								// turn on display, cursor, no character blink
	lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorControl | lcdDCcursorBlinkControl,	// turn on display, cursor, with character blink
};

#endif // defined(use4BitLCD)
#if defined(useBinaryLCDbrightness)
static const uint8_t brightnessLength = 2;

#else // defined(useBinaryLCDbrightness)
#if defined(useInvertedLegacyLCDbrightness)
static const uint8_t brightness[] PROGMEM = { 255, 214, 171, 127 }; // middle button cycles through these brightness settings
#else // defined(useInvertedLegacyLCDbrightness)
static const uint8_t brightness[] PROGMEM = { 0, 41, 84, 128 }; // middle button cycles through these brightness settings
#endif // defined(useInvertedLegacyLCDbrightness)

static const uint8_t brightnessLength = ( sizeof(brightness) / sizeof(uint8_t) ); // size of brightness table

#endif // defined(useBinaryLCDbrightness)
#endif // defined(useLCDoutput)
