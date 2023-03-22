#ifdef useClockDisplay
namespace clockSet /* Big Clock Display support section prototype */
{

	static void entry(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void set(void);
	static void cancel(void);

};

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
#endif // useBigDigitDisplay
