#ifdef useBarGraph
namespace barGraphSupport /* Bar Graph support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void displayBarGraphLine(uint8_t lineNumber, uint8_t tripIdx, uint8_t calcIdx);
	static void clearPlot(void);
	static uint8_t rescaleData(uint8_t byt, uint8_t uBound);
	static void graphData(uint8_t cursorPos, uint8_t calcIdx, uint8_t diffFlag);
	static void drawLine(uint8_t color, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
	static void draw(uint8_t color, uint8_t xPos, uint8_t yPos);
//	static int16_t sqrrt(uint16_t num);

};

static const uint8_t prgmGenerateHistographData[] PROGMEM = {
	instrLdJumpReg,										// save jump register value of input function
	instrLxdI, bgDataSize,
	instrLdRegByte, 0x03, 0,

//loop:
	instrAddIndex, 255,									// decrement index
	instrCallImplied,									// go call input function
	instrBranchIfE, 10,									// if function result is zero, go skip
	instrTestReg, 0x03,									// test currently saved high value
	instrBranchIfE, 4,									// if high value is zero, go save what the input function returned
	instrCmpXtoY, 0x23,									// otherwise, compare current value to high value
	instrBranchIfLTorE,	2,								// if not higher, skip

//cont0:
	instrLdReg, 0x23,									// save high value

//cont1:
	instrTestIndex,
	instrBranchIfNotE, 238,
	instrLdReg, 0x32,									// load high value
	instrDiv2byByte, 100,								// divide by normalization value
	instrLdReg, 0x23,									// save high value

//loop2:
	instrTestReg, 0x03,									// is high value zero
	instrBranchIfE, 42,									// if so, just store a zero - can't divide by zero
	instrCallImplied,									// go call input function
	instrBranchIfNotE, 12,								// if function result is not zero, go skip
	instrBranchIfOverflow, 5,
	instrLdRegByte, 0x02, 0,							// load zero value into main register
	instrSkip, 9,

//ovfl:
	instrLdRegByte, 0x02, 255,							// load overflow value into main register
	instrSkip, 4,

//cont2:
	instrLdReg, 0x31,									// recall high value
	instrDiv2by1,										// go normalize value
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont3:
	instrStRegBGdataIndexed, 0x02,						// save normalized value
	instrAddIndex, 1,									// bump index up
	instrCmpIndex, bgDataSize,							// processed through all of fuel econ vs time trip variable bank?
	instrBranchIfLT, 225,								// if not, loop back
	instrDone											// return to caller
};

static const char bgSpaces[] PROGMEM = {
	 0x80, ' ', ' ', 0xF0, 0xF2, 0xF4, 0xF6, ' ', ' ', 0
	,0x94, ' ', ' ', 0xF1, 0xF3, 0xF5, 0xF7, ' ', ' ', 0
};

#endif // useBarGraph
#ifdef useBarFuelEconVsTime
volatile uint8_t FEvTperiodIdx;

uint8_t calcFEvTperiodIdx; // used for calculations, graphing

static const char barFEvTfuncNames[] PROGMEM = {
	"FuelUsed / Time\r"
	"Distance / Time\r"
	"FE / Time\r"
	"DiffFE / Time\r"
};

static const uint8_t barFEvTdisplayFuncs[] PROGMEM = {
	 tFuelUsed
	,tDistance
	,tFuelEcon
	,tFuelEcon
};

static const uint8_t barFEvTgraphFuncs[] PROGMEM = {
	 tFEvTgetConsumedFuel
	,tFEvTgetDistance
	,tFEvTgetFuelEconomy
	,tFEvTgetFuelEconomy
};

static const uint8_t barFEvTdiffFuncs[] PROGMEM = {
	 0
	,0
	,0
	,1
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
uint8_t FEvSpdTripIdx;

namespace bgFEvsSsupport /* fuel economy over speed histograph support section prototype */
{

	static void reset(void);

};

static const char barFEvSfuncNames[] PROGMEM = {
	"FE / Speed\r"
	"Fuel Used/Speed\r"
	"Distance / Speed\r"
};

static const uint8_t barFEvSdisplayFuncs[] PROGMEM = {
	 tFuelEcon
	,tFuelUsed
	,tDistance
};

static const uint8_t barFEvSgraphFuncs[] PROGMEM = {
	 tFEvSgetFuelEconomy
	,tFEvSgetConsumedFuel
	,tFEvSgetDistance
};

static void bgFEvsSsupport::reset(void)
{

	for (uint8_t x = 0; x < bgDataSize; x++) tripVar::reset(FEvsSpeedIdx + x);

	FEvSpdTripIdx = 255;

}

static const uint8_t prgmFEvsSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrTestReg, 0x02,									// test VSS cycle value
	instrBranchIfZero, 15,								// if zero, then speed is also zero
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS pulse count
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term
	instrMul2byConst, idxCycles0PerSecond,				// set up to convert VSS cycle value to time in seconds
	instrDiv2by1,										// divide to obtain vehicle speed

//cont:
	instrSubMainFromX, 0x02, mpFEvsSpeedMinThresholdIdx,	// compare vehicle speed to minimum threshold
	instrBranchIfLTorE, 4,								// if vehicle speed is above minimum threshold, skip ahead

//badRet:
	instrLdRegByte, 0x02, 0xFF,							// load a 255 into register 2
	instrDone,											// exit to caller

//cont2:
	instrDiv2byMain, mpFEvsSpeedQuantumIdx,				// find trip index offset
	instrLdRegByte, 0x01, bgDataSize - 1,				// is offset greater than the number of available trip slots
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if not, skip ahead
	instrLdReg, 0x12,									// load the last trip slot index

//cont3:
	instrAddByteToX, 0x02, FEvsSpeedIdx,				// obtain working fuel econ vs speed trip index value
	instrDone											// exit to caller
};

#endif // useBarFuelEconVsSpeed
