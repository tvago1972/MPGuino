#if defined(useClockSupport)
namespace clockSet /* Clock support section prototype */
{

#if defined(useClockDisplay)
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void entry(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void set(void);
	static void cancel(void);
#endif // defined(useClockDisplay)
#if defined(useDS1307clock)
	static void setFromRTC(void);
#endif // defined(useDS1307clock)

};

#endif // defined(useClockSupport)
#if defined(useClockDisplay)
namespace clockDisplay /* Big Clock Display support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);

};

static char csBuff[17];

#endif // defined(useClockDisplay)
#if defined(useStatusMeter)
namespace statusBar /* Status Meter Output support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void outputStatusBar(uint16_t val);
	static void writeStatusBarElement(uint8_t chr, uint8_t val);

};

// format for below entries is as follows:
//
// 0bxxxy yyyy
//
// xxx   - number of elements to fill in
// yyyyy - position of element on page
//         00000 - left endcap
//         11111 - right endcap
//                 everything else is a normal segment
//
// the below is configured for a 16x2 display, endcaps with 3 elements each, and normal segments with 4 elements
//
static const uint8_t statusBarPos[63] PROGMEM = {
	0b00000000,		// zero value

	0b01000000,		// left endcap
	0b01100000,
	0b10000000,

	0b00100001,		// normal segment 1
	0b01000001,
	0b01100001,
	0b10000001,

	0b00100010,		// normal segment 2
	0b01000010,
	0b01100010,
	0b10000010,

	0b00100011,		// normal segment 3
	0b01000011,
	0b01100011,
	0b10000011,

	0b00100100,		// normal segment 4
	0b01000100,
	0b01100100,
	0b10000100,

	0b00100101,		// normal segment 5
	0b01000101,
	0b01100101,
	0b10000101,

	0b00100110,		// normal segment 6
	0b01000110,
	0b01100110,
	0b10000110,

	0b00100111,		// normal segment 7
	0b01000111,
	0b01100111,
	0b10000111,

	0b00101000,		// normal segment 8
	0b01001000,
	0b01101000,
	0b10001000,

	0b00101001,		// normal segment 9
	0b01001001,
	0b01101001,
	0b10001001,

	0b00101010,		// normal segment 10
	0b01001010,
	0b01101010,
	0b10001010,

	0b00101011,		// normal segment 11
	0b01001011,
	0b01101011,
	0b10001011,

	0b00101100,		// normal segment 12
	0b01001100,
	0b01101100,
	0b10001100,

	0b00101101,		// normal segment 13
	0b01001101,
	0b01101101,
	0b10001101,

	0b00101110,		// normal segment 14
	0b01001110,
	0b01101110,
	0b10001110,

	0b00111111,		// right endcap
	0b01011111,
	0b01111111,
};

static const uint8_t statusBarElement[] PROGMEM = {
	0b00000000,
	0b00010000,
	0b00011000,
	0b00011010,
	0b00011011,
};

static const uint8_t statusBarLength = sizeof(statusBarPos) / sizeof(uint8_t);

static const uint8_t statusBarFont[] PROGMEM = {
	6, // number of characters in font

	0b00001011, // char 0xF0 - left endcap (variable fill, empty)
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00001011,
	0b00000000,

	0b00011010, // char 0xF1 - right endcap (variable fill, empty, full)
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00000001,
	0b00011010,
	0b00000000,

	0b00011011, // char 0xF2 - middle (variable fill)
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011011,
	0b00000000,

	0b00011011, // char 0xF3 - middle (empty)
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011011,
	0b00000000,

	0b00011011, // char 0xF4 - middle (full)
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00000000,

	0b00001011, // char 0xF5 - left endcap (full)
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00001011,
	0b00000000,
};

static const uint8_t statusBarOverflowFont[] PROGMEM = {
	3, // number of characters in font

	0b00001011, // char 0xF0 - left endcap (out of range)
	0b00011001,
	0b00010010,
	0b00011001,
	0b00010010,
	0b00011001,
	0b00001011,
	0b00000000,

	0b00011010, // char 0xF1 - right endcap (out of range)
	0b00001001,
	0b00010011,
	0b00001001,
	0b00010011,
	0b00001001,
	0b00011010,
	0b00000000,

	0b00011011, // char 0xF2 - middle (out of range)
	0b00001001,
	0b00010010,
	0b00001001,
	0b00010010,
	0b00001001,
	0b00011011,
	0b00000000,
};

#endif // defined(useStatusMeter)
#if defined(useBigDigitDisplay)
namespace bigDigit /* Big Digit output support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
#if defined(useBigNumberDisplay)
	static void outputNumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalFlag, uint8_t cursorPos, const char * str);
#endif // defined(useBigNumberDisplay)
#if defined(useBigTimeDisplay)
	static void outputTime(uint8_t hPos, char * val, uint8_t blinkFlag, uint8_t blinkPos, uint8_t cursorPos, const char * str);
#endif // defined(useBigTimeDisplay)
	static void outputNumberString(uint8_t hPos, char * str, uint8_t cursorPos, const char * titleStr);

};

#if defined(useBigFE)
static const char bigFElabels[] PROGMEM = {
	"MPG" tcEOSCR
	"L100" tcEOSCR
	"G100" tcEOSCR
	"KPL" tcEOSCR
};

#endif // defined(useBigFE)
#if defined(useSpiffyBigChars)
static const char bigDigitChars1[] PROGMEM = {
	tcCG6	tcCG0	tcCG7	tcEOS
	tcCG0	tcCG4	" "		tcEOS
	tcCG2	tcCG2	tcCG7	tcEOS
	tcCG0	tcCG2	tcCG7	tcEOS
	tcCG4	tcCG1	tcCG4	tcEOS
	tcCG4	tcCG2	tcCG2	tcEOS
	tcCG6	tcCG2	tcCG2	tcEOS
	tcCG0	tcCG0	tcCG5	tcEOS
	tcCG6	tcCG2	tcCG7	tcEOS
	tcCG6	tcCG2	tcCG7	tcEOS
	"   "					tcEOS
	tcCG1	tcCG1	tcCG1	tcEOS
};

static const char bigDigitChars2[] PROGMEM = {
	tcCG3	tcCG1	tcCG5	tcEOS
	" "		tcCG4	" "		tcEOS
	tcCG4	tcCG1	tcCG1	tcEOS
	tcCG1	tcCG1	tcCG5	tcEOS
	"  "			tcCG4	tcEOS
	tcCG1	tcCG1	tcCG5	tcEOS
	tcCG3	tcCG1	tcCG5	tcEOS
	" "		tcCG6	" "		tcEOS
	tcCG3	tcCG1	tcCG5	tcEOS
	tcCG1	tcCG1	tcCG5	tcEOS
	"   "					tcEOS
	"   "					tcEOS
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

#else // defined(useSpiffyBigChars)
static const char bigDigitChars1[] PROGMEM = {
	tcCG3	tcCG0	tcCG3	tcEOS
	tcCG0	tcCG3	" "		tcEOS
	tcCG2	tcCG2	tcCG3	tcEOS
	tcCG0	tcCG2	tcCG3	tcEOS
	tcCG3	tcCG1	tcCG3	tcEOS
	tcCG3	tcCG2	tcCG2	tcEOS
	tcCG3	tcCG2	tcCG2	tcEOS
	tcCG0	tcCG0	tcCG3	tcEOS
	tcCG3	tcCG2	tcCG3	tcEOS
	tcCG3	tcCG2	tcCG3	tcEOS
	"   "					tcEOS
	tcCG1	tcCG1	tcCG1	tcEOS
};

static const char bigDigitChars2[] PROGMEM = {
	tcCG3	tcCG1	tcCG3	tcEOS
	tcCG1	tcCG3	tcCG1	tcEOS
	tcCG3	tcCG1	tcCG1	tcEOS
	tcCG1	tcCG1	tcCG3	tcEOS
	"  "			tcCG3	tcEOS
	tcCG1	tcCG1	tcCG3	tcEOS
	tcCG3	tcCG1	tcCG3	tcEOS
	" "		tcCG3	" "		tcEOS
	tcCG3	tcCG1	tcCG3	tcEOS
	tcCG1	tcCG1	tcCG3	tcEOS
	"   "					tcEOS
	"   "					tcEOS
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

#endif // defined(useSpiffyBigChars)
#endif // defined(useBigDigitDisplay)
