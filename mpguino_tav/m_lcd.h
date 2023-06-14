#ifdef useLCDoutput
namespace LCD /* LCD hardware support section prototype */
{

	static void init(void);
	static void shutdown(void);
#if defined(useLCDgraphics)
	static void loadCGRAMcharacter(uint8_t chr, char * chrData);
	static void writeCGRAMbyte(uint8_t cgrAddress, uint8_t chr);
	static uint8_t peekCGRAMbyte(uint8_t cgrAddress);
	static void flushCGRAM(void);
#endif // defined(useLCDgraphics)
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

#if defined(useLCDgraphics)
static const uint8_t cgramFlagDirty =		0b10000000;

static uint8_t CGRAMbuffer[64]; // used by LCD output routine

#endif // defined(useLCDgraphics)
#ifdef use4BitLCD
interfaceDevice devLCD;

#ifdef useBufferedLCD
ringBufferVariable lcdBuffer;

static volatile uint8_t LCDdata[32];

#endif // useBufferedLCD

static volatile unsigned int lcdDelayCount;

#ifdef useTWILCD
#ifdef useAdafruitRGBLCDshield
static const uint8_t lcdDirection =		0b01000000; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
static const uint8_t lcdRegisterSelect =	0b10000000; // GPIO B
static const uint8_t lcdEnable =			0b00100000; // GPIO B
static const uint8_t lcdBit3 =				0b00000010; // GPIO B
static const uint8_t lcdBit2 =				0b00000100; // GPIO B
static const uint8_t lcdBit1 =				0b00001000; // GPIO B
static const uint8_t lcdBit0 =				0b00010000; // GPIO B
static const uint8_t lcdBrightnessRed =	0b01000000; // GPIO A
static const uint8_t lcdBrightnessGreen =	0b10000000; // GPIO A
static const uint8_t lcdBrightnessBlue =	0b00000001; // GPIO B

static const uint8_t lcdAddress =		0x20;

static volatile uint8_t portSwitches; // contains two out of the three LCD backlighting LED pins
#else // useAdafruitRGBLCDshield
static const uint8_t lcdDirection =		0b00000010; // Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
static const uint8_t lcdRegisterSelect =	0b00000001;
static const uint8_t lcdEnable =			0b00000100;
static const uint8_t lcdBit3 =				0b10000000;
static const uint8_t lcdBit2 =				0b01000000;
static const uint8_t lcdBit1 =				0b00100000;
static const uint8_t lcdBit0 =				0b00010000;
static const uint8_t lcdBrightness =		0b00001000;

static const uint8_t lcdAddress =		0x27;

#endif // useAdafruitRGBLCDshield
static volatile uint8_t portLCD; // LCD port register expander byte

#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
static const uint8_t lcdDirection =		(1 << PORTF0); // on PORTF, A5, Legacy and Mega2560 Arduino LCDs have their pin R/W (5) tied directly to ground, so they don't need this assignment
static const uint8_t lcdRegisterSelect =	(1 << PORTF1); // on PORTF, A4
static const uint8_t lcdEnable =			(1 << PORTE6); // on PORTE, 7
static const uint8_t lcdBit3 =				(1 << PORTB4); // on PORTB, 8
static const uint8_t lcdBit2 =				(1 << PORTD6); // on PORTD, 12
static const uint8_t lcdBit1 =				(1 << PORTD4); // on PORTD, 4
static const uint8_t lcdBit0 =				(1 << PORTF4); // on PORTF, A3
static const uint8_t lcdBrightness =		(1 << DDB6); // on PORTB, 10
static const uint8_t lcdContrast =			(1 << DDB5); // on PORTB, 9

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
static const uint8_t lcdRegisterSelect =	(1 << PORTA4); // on PORTA
static const uint8_t lcdEnable =			(1 << PORTA5); // on PORTA
static const uint8_t lcdBit3 =				(1 << PORTA0); // on PORTA
static const uint8_t lcdBit2 =				(1 << PORTA1); // on PORTA
static const uint8_t lcdBit1 =				(1 << PORTA2); // on PORTA
static const uint8_t lcdBit0 =				(1 << PORTA3); // on PORTA
static const uint8_t lcdBrightness =		(1 << DDB5); // on PORTB
static const uint8_t lcdContrast =			(1 << DDB7); // on PORTB

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
static const uint8_t lcdRegisterSelect =	(1 << PORTD4); // on PORTD, 4
static const uint8_t lcdEnable =			(1 << PORTD5); // on PORTD, 5
static const uint8_t lcdBit3 =				(1 << PORTB5); // on PORTB, 13
static const uint8_t lcdBit2 =				(1 << PORTB4); // on PORTB, 12
static const uint8_t lcdBit1 =				(1 << PORTB0); // on PORTB, 8
static const uint8_t lcdBit0 =				(1 << PORTD7); // on PORTD, 7
static const uint8_t lcdBrightness =		(1 << DDB1); // on PORTB, 9
static const uint8_t lcdContrast =			(1 << DDD6); // on PORTD, 6

#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD
static const uint8_t lcdNullValue =				0b00000000;

static const uint8_t lcdClearDisplay =				0b00000001;

static const uint8_t lcdReturnHome =				0b00000010;

static const uint8_t lcdEntryModeSet =				0b00000100;
static const uint8_t lcdEMSincrement =				0b00000010;		// 1 = increment, 0 = decrement
static const uint8_t lcdEMSsetDisplayShift =		0b00000001;		// 1 = display shift, 0 = no display shift

static const uint8_t lcdDisplayControl =			0b00001000;
static const uint8_t lcdDCdisplayShow =			0b00000100;		// 1 = enable display, 0 = disable display
static const uint8_t lcdDCcursorControl =			0b00000010;		// 1 = cursor on, 0 = cursor off
static const uint8_t lcdDCcursorBlinkControl =		0b00000001;		// 1 = cursor blink, 0 = cursor steady

static const uint8_t lcdShift =					0b00010000;
static const uint8_t lcdSdisplayShift =			0b00001000;		// 1 = shift display, 0 = cursor move
static const uint8_t lcdSdirection =				0b00000100;		// 1 = move right, 0 = move left

static const uint8_t lcdFunctionSet =				0b00100000;
static const uint8_t lcdFSdataLength =				0b00010000;		// 1 = 8 bit data, 0 = 4 bit data
static const uint8_t lcdFSnumberOfLines =			0b00001000;		// 1 = 2 lines, 0 = 1 line
static const uint8_t lcdFScharacterFont =			0b00000100;		// 1 = 5x10 dot character font, 0 = 5x8 dot character font

static const uint8_t lcdSetCGRAMaddress =			0b01000000;

static const uint8_t lcdSetDDRAMaddress =			0b10000000;

static const uint8_t lcdSendNybble =				0b00001000;
static const uint8_t lcdDataByte =					0b00000100;
static const uint8_t lcdCommandByte =				0b00000000;

static const uint8_t lcdDelay0015ms =				0x03;
static const uint8_t lcdDelay4100us =				0x02;
static const uint8_t lcdDelay0100us =				0x01;
static const uint8_t lcdDelay0040us =				0x00;
static const uint8_t lcdDelayFlags =				lcdDataByte | 0x03;
static const uint8_t lcdSendFlags =				lcdSendNybble | 0x03;

static const uint8_t systemProcessorSpeed = (F_CPU / 1000000ul);

//static const unsigned long t1CyclesPerSecond = (unsigned long)(systemProcessorSpeed * 1000000ul); // (systemProcessorSpeed * 1000000 / (timer 1 prescaler))

//static const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // initial LCD delay for 4-bit initialization
//static const unsigned int delayLCD04100usTick = (unsigned int)(ceil)((double)(4100ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // secondary LCD delay for 4-bit initialization
//static const unsigned int delayLCD00100usTick = (unsigned int)(ceil)((double)(100ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // final LCD delay for 4-bit initialization
//static const unsigned int delayLCD00040usTick = (unsigned int)(ceil)((double)(40ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // normal LCD character transmission delay
//static const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * F_CPU) / (double)(510ul * 1000000ul)) - 1; // initial LCD delay for 4-bit initialization
static const unsigned int delayLCD15000usTick = (unsigned int)(ceil)((double)(15200ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const unsigned int delayLCD04100usTick = (unsigned int)(ceil)((double)(4100ul * systemProcessorSpeed) / (double)(510ul)) - 1; // secondary LCD delay for 4-bit initialization
static const unsigned int delayLCD00100usTick = (unsigned int)(ceil)((double)(100ul * systemProcessorSpeed) / (double)(510ul)) - 1; // final LCD delay for 4-bit initialization
static const unsigned int delayLCD00040usTick = (unsigned int)(ceil)((double)(40ul * systemProcessorSpeed) / (double)(510ul)); // normal LCD character transmission delay

static uint8_t LCDgotoXYaddress;
static uint8_t LCDaddressX;
static uint8_t LCDaddressY;

#endif // use4BitLCD
static uint8_t brightnessIdx;

#ifdef useLegacyLCD
#ifdef useInvertedLegacyLCDbrightness
static const uint8_t brightness[] PROGMEM = { 255, 214, 171, 127 }; //middle button cycles through these brightness settings
#else // useInvertedLegacyLCDbrightness
static const uint8_t brightness[] PROGMEM = { 0, 41, 84, 128 }; //middle button cycles through these brightness settings
#endif // useInvertedLegacyLCDbrightness

static const uint8_t brightnessLength = ( sizeof(brightness) / sizeof(uint8_t) ); // size of brightness table

static const char brightString[] PROGMEM = {
	"LOW" tcLCDLO tcEOSCR
	"MED" tcLCDMED tcEOSCR
	"HIGH" tcLCDHI tcEOSCR
	"OFF" tcLCDOFF tcEOSCR
};

#endif // useLegacyLCD
#ifdef useBinaryLCDbrightness
static const uint8_t brightnessLength = 2;

static const char brightString[] PROGMEM = {
	"ON" tcLCDLO tcEOSCR
	"OFF" tcLCDOFF tcEOSCR
};

#endif // useBinaryLCDbrightness
#endif // useLCDoutput
