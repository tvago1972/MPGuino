static const char * findStr(const char * str, uint8_t strIdx);
static unsigned long str2ull(char * strBuffer);
static void storeDigit(uint8_t value, char * strBuffer, uint8_t &strPos, uint8_t &decPos, char &zeroChar, uint8_t &digCnt, uint8_t flg);
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t prgmIdx);
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t windowLength, uint8_t decimalFlag);

namespace text /* text string output section prototype */
{

	static void gotoXY(interfaceDevice &dev, uint8_t x, uint8_t y);
	static void newLine(interfaceDevice &dev);
	static uint8_t charOut(interfaceDevice &dev, uint8_t chr, uint8_t chrCount);
	static uint8_t charOut(interfaceDevice &dev, uint8_t chr);

	static void statusOut(interfaceDevice &dev, const char * sList, uint8_t strIdx, const char * str);
	static void statusOut(interfaceDevice &dev, const char * sList, uint8_t strIdx);
	static void statusOut(interfaceDevice &dev, const char * str);
	static void initStatus(interfaceDevice &dev);
	static void stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx);
	static void stringOut(interfaceDevice &dev, const char * str);
	static void stringOut(interfaceDevice &dev, char * str);
	static void stringOutIf(interfaceDevice &dev, uint8_t condition, const char * str, uint8_t strIdx);
	static void stringOutIf(interfaceDevice &dev, uint8_t condition, const char * str);
	static void hexNybbleOut(interfaceDevice &dev, uint8_t val);
	static void hexByteOut(interfaceDevice &dev, uint8_t val);
	static void hexWordOut(interfaceDevice &dev, uint16_t val);
	static void hexDWordOut(interfaceDevice &dev, uint32_t val);
	static void hexLWordOut(interfaceDevice &dev, uint64_t * val);

};

static const uint8_t odvFlagCRLF =				0b00000100;
static const uint8_t odvFlagDoubleHeight =		0b00000010;
static const uint8_t odvFlagEnableOutput =		0b00000001;

static const char overFlowStr[] PROGMEM = "----------";
static const char overFlow9Str[] PROGMEM = "9999999999";

static const uint8_t dfOverflow9s =			0b10000000;
static const uint8_t dfIgnoreDecimalPoint =	0b01000000;

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
	instrLdRegConst, 0x02, idxDecimalPoint,				// window length is 1 digit, load equivalent of decimal formatting term
	instrSkip, 4,										// skip ahead

//cont:
	instrLdRegConstIndexed, 0x02,						// load power of 10 corresponding to window into register 2
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term

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

