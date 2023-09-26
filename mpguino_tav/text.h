static const char * findStr(const char * str, uint8_t strIdx);
static unsigned long str2ull(char * strBuffer);
static void storeDigit(uint8_t value, char * strBuffer, uint8_t &strPos, uint8_t &decPos, char &zeroChar, uint8_t &digCnt, uint8_t flg);
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t prgmIdx);
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t windowLength, uint8_t decimalFlag);

namespace text /* text support section prototype */
{

	static void initDev(uint8_t devIdx, uint8_t devStatus, void (* charOut)(uint8_t), uint8_t (* charIn)(void));
	static void initDev(uint8_t devIdx, uint8_t devStatus, void (* charOut)(uint8_t));
	static uint8_t chrIn(uint8_t devIdx);

	static void gotoXY(uint8_t devIdx, uint8_t x, uint8_t y);
	static void newLine(uint8_t devIdx);
	static uint8_t charOut(uint8_t devIdx, uint8_t chr, uint8_t chrCount);
	static uint8_t charOut(uint8_t devIdx, uint8_t chr);

	static void statusOut(uint8_t devIdx, const char * sList, uint8_t strIdx, const char * str);
	static void statusOut(uint8_t devIdx, const char * str, const char * sList, uint8_t strIdx);
	static void statusOut(uint8_t devIdx, const char * sList, uint8_t strIdx);
	static void statusOut(uint8_t devIdx, const char * str);
	static void initStatus(uint8_t devIdx);
	static void commitStatus(uint8_t devIdx);
	static void stringOut(uint8_t devIdx, const char * str, uint8_t strIdx);
	static void stringOut(uint8_t devIdx, const char * str);
	static void stringOut(uint8_t devIdx, char * str);
	static void stringOutIf(uint8_t devIdx, uint8_t condition, const char * str, uint8_t strIdx);
	static void stringOutIf(uint8_t devIdx, uint8_t condition, const char * str);
	static void hexNybbleOut(uint8_t devIdx, uint8_t val);
	static uint8_t nybble(uint8_t val);
	static void hexByteOut(uint8_t devIdx, uint8_t val);
	static void hexWordOut(uint8_t devIdx, uint16_t val);
	static void hexDWordOut(uint8_t devIdx, uint32_t val);
	static void hexLWordOut(uint8_t devIdx, uint64_t * val);
	static void tripFunctionOut(uint8_t devIdx, uint16_t tripCalc, uint8_t windowLength, uint8_t decimalFlag);
	static void tripFunctionOut(uint8_t devIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalFlag);
	static void numberOut(uint8_t devIdx, uint8_t decimalFlag);

};

static const char overFlowStr[] PROGMEM =	"----------";
static const char overFlow9Str[] PROGMEM =	"9999999999";

static const uint8_t dfOverflow9s =			0b10000000;
static const uint8_t dfIgnoreDecimalPoint =	0b01000000;
static const uint8_t dfSuppressAutoRange =	0b00100000;

static const uint8_t dfOutputLabel =		0b00010000;

static const uint8_t dfOutputSpiffyTag =	0b00010000;
static const uint8_t dfOutputTag =			0b00001000;
static const uint8_t dfBlinkCalc =			0b00000100;
static const uint8_t dfBlinkTrip =			0b00000010;
static const uint8_t dfOutputTagFirst =		0b00000001;
#define dfOutputNumberLast dfOutputTagFirst

static const uint8_t dfOutputBluetooth =	(dfOverflow9s | dfIgnoreDecimalPoint | dfSuppressAutoRange);
static const uint8_t dfOutputLabelCheck =	(dfOutputSpiffyTag | dfOutputTag);

static const uint8_t prgmMultiplyBy100[] PROGMEM = {
	instrMul2byByte, 100,								// multiply result by 100
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static const uint8_t prgmMultiplyBy10[] PROGMEM = {
	instrMul2byByte, 10,								// multiply result by 10
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static const uint8_t prgmAutoRangeNumber[] PROGMEM = {
	instrLdReg, 0x23,									// save contents of register 2
	instrAddIndex, 0xFE,								// bump window length down by 2
	instrCmpIndex, 0xFF,								// was window length 1 digit?
	instrBranchIfE, 8,									// if so, go process corner case
	instrCmpIndex, 9,									// check window length for validity
	instrBranchIfLT, 9,									// if valid, skip ahead
	instrLxdI, 8,										// assume window length of 8 digits
	instrSkip, 5,										// skip ahead
	instrLdRegRdOnly, 0x02, idxDecimalPoint,			// window length is 1 digit, load equivalent of decimal formatting term
	instrSkip, 5,										// skip ahead

//cont:
	instrLdRegRdOnlyOffset, 0x02, idxTen,				// load power of 10 corresponding to window into register 2
	instrMul2byRdOnly, idxDecimalPoint,					// adjust by decimal formatting term

//cont2:
	instrLxdI, 0,										// initialize decimal count with 0
	instrCmpXtoY, 0x32,									// compare register 2 contents to initial window value
	instrBranchIfGT, 18,								// if larger or equal, there are no decimal digits to output
	instrAddIndex, 1,									// bump decimal count up by 1
	instrDiv2byByte, 10,								// shift window right 1 digit (should be 0.1 equivalent)
	instrCmpXtoY, 0x32,									// compare register 2 contents to initial window value
	instrBranchIfGT, 10,								// if register 2 contents are not less than window, go save decimal count
	instrAddIndex, 1,									// bump decimal count up by 1
	instrDiv2byByte, 10,								// shift window right 1 digit (should be 0.01 equivalent)
	instrCmpXtoY, 0x32,									// compare register 2 contents to initial window value
	instrBranchIfGT, 2,									// if register 2 contents are not less than window, go save decimal count
	instrAddIndex, 1,									// bump decimal count up by 1

//storeDecCnt:
	instrLdRegByteFromIndex, 0x02,						// save obtained decimal count
	instrSwapReg, 0x32,									// swap register 2 contents with obtained decimal count
	instrDone											// exit to caller
};

