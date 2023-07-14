#if defined(useBarGraph)
namespace barGraphSupport /* Bar Graph support section prototype */
{

	static void displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void displayBarGraphLine(uint8_t lineNumber, uint8_t tripIdx, uint8_t calcIdx);
	static void clearPlot(void);
	static uint8_t rescaleData(uint8_t byt, uint8_t uBound);
	static void graphData(uint8_t cursorPos, uint8_t calcIdx, uint8_t diffFlag);
	static void drawLine(uint8_t color, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
	static void draw(uint8_t color, uint8_t xPos, uint8_t yPos);

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
	"\x80" "  " tcCG0 tcCG2 tcCG4 tcCG6 "  " tcEOS
	"\x94" "  " tcCG1 tcCG3 tcCG5 tcCG7 "  " tcEOS
};

#if defined(useBarFuelEconVsTime)
namespace bgFEvsTsupport /* fuel economy over time histograph support section prototype */
{

	static uint8_t getFEvTperiodIdx(void);

};

volatile uint8_t FEvTperiodIdx;

static const char barFEvTfuncNames[] PROGMEM = {
	"FuelUsed / Time" tcEOS
	"Distance / Time" tcEOS
	"FE / Time" tcEOS
	"DiffFE / Time" tcEOS
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

#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
namespace bgFEvsSsupport /* fuel economy over speed histograph support section prototype */
{

	static void reset(void);

};

uint8_t FEvSpdTripIdx;

static const char barFEvSfuncNames[] PROGMEM = {
	"FE / Speed" tcEOS
	"Fuel Used/Speed" tcEOS
	"Distance / Speed" tcEOS
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

#endif // defined(useBarFuelEconVsSpeed)
#endif // defined(useBarGraph)
