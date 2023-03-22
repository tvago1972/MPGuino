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
interfaceDevice devLCD;

#ifdef useBufferedLCD
ringBufferVariable lcdBuffer;

volatile uint8_t LCDdata[32];

#endif // useBufferedLCD

volatile unsigned int lcdDelayCount;

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
