#if defined(useBarGraph)
namespace barGraphSupport /* Bar Graph support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void displayBarGraphLine(uint8_t lineNumber, uint8_t tripIdx, uint8_t calcIdx);
	static void clearPlot(void);
	static uint8_t rescaleData(uint8_t byt, uint8_t uBound);
	static void graphData(uint8_t cursorPos, uint8_t calcIdx, uint8_t diffFlag);
	static void drawLine(uint8_t color, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
	static void draw(uint8_t color, uint8_t xPos, uint8_t yPos);

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
