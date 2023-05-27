const char * findStr(const char * str, uint8_t strIdx);
static void printStatusMessage(const char * s);
static void printStatusMessage(const char * s, uint8_t strIdx);
void initStatusLine(void);
void execStatusLine(void);
char findDigit(uint8_t value, char &zeroChar);
unsigned long str2ull(char * strBuffer);
char * ull2str(uint8_t prgmIdx, char * strBuffer, uint8_t decimalPlaces);
char * formatDecimal(char * strBuffer, uint8_t windowLength, uint8_t decimalPlaces, uint8_t decimalFlag);

namespace text /* text string output section prototype */
{

	static void gotoXY(interfaceDevice &dev, uint8_t x, uint8_t y);
	static uint8_t charOut(interfaceDevice &dev, uint8_t chr);

	static void setModeOnCondition(interfaceDevice &dev, uint8_t condition, uint8_t odvFlag);
	static void stringOut(interfaceDevice &dev, char * str);
	static void stringOut(interfaceDevice &dev, const char * str);
	static void stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx);
	static void newLine(interfaceDevice &dev);
	static void hexNybbleOut(interfaceDevice &dev, uint8_t val);
	static void hexByteOut(interfaceDevice &dev, uint8_t val);
	static void hexWordOut(interfaceDevice &dev, uint16_t val);
	static void hexDWordOut(interfaceDevice &dev, uint32_t val);
	static void hexLWordOut(interfaceDevice &dev, uint64_t * val);
	static uint8_t numberOut(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag);

};

static const uint8_t odvFlagCRLF =				0b00000100;
static const uint8_t odvFlagShootBlanks =		0b00000010;
static const uint8_t odvFlagEnableOutput =		0b00000001;

const char overFlowStr[] PROGMEM = "----------";
const char overFlow9Str[] PROGMEM = "9999999999";

const uint8_t dfAdjustWindow =		0b00000001;
const uint8_t dfOverflow9s =		0b10000000;
