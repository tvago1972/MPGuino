typedef struct
{

  uint8_t isValid;
  uint8_t tripIdx;
  uint8_t tripChar;
  uint8_t calcIdx;
  uint8_t calcFmtIdx;
  uint8_t decimalPlaces;
  uint8_t calcChar;
  char * strBuffer;

} calcFuncObj;

static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag);
#ifdef useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
#else // useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos);
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx);
#endif // useSpiffyTripLabels

// calculation indexes into SWEET64 S64programList[] for display functions to either screen or logging output
//
// functions are grouped into three categories, in order
//
//    functions that return results that do not require conversion between SI and SAE formats
//    functions that return results in either SI or SAE format, depending on MPGuino settings
//    functions that return results in either SI or SAE format, and have an alternate SI/SAE format, depending on MPGuino settings
//
#define nextAllowedValue 0
static const uint8_t tEngineRunTime =				nextAllowedValue;				// engine runtime (hhmmss)
static const uint8_t tRangeTime =					tEngineRunTime + 1;				// estimated total runtime from full tank (hhmmss)
static const uint8_t tReserveTime =					tRangeTime + 1;					// estimated reserve runtime from full tank (hhmmss)
static const uint8_t tBingoTime =					tReserveTime + 1;				// estimated bingo fuel runtime from full tank (hhmmss)
static const uint8_t tTimeToEmpty =					tBingoTime + 1;					// estimated remaining total tank engine runtime (hhmmss)
static const uint8_t tReserveTimeToEmpty =			tTimeToEmpty + 1;				// estimated remaining reserve engine runtime (hhmmss)
static const uint8_t tBingoTimeToEmpty =			tReserveTimeToEmpty + 1;		// estimated bingo quantity engine runtime (hhmmss)
static const uint8_t tMotionTime =					tBingoTimeToEmpty + 1;			// time vehicle in motion (hhmmss)
#define nextAllowedValue tMotionTime + 1

static const uint8_t dfMaxValHHMMSS =				nextAllowedValue;				// maximum index for function indexes that return results that are not affected by metric conversions

static const uint8_t tInjectorOpenTime =			nextAllowedValue;				// fuel used (microseconds)
static const uint8_t tInjectorTotalTime =			tInjectorOpenTime + 1;			// engine run time (microseconds)
static const uint8_t tVSStotalTime =				tInjectorTotalTime + 1;			// time vehicle in motion (microseconds)
static const uint8_t tEngineSpeed =					tVSStotalTime + 1;				// engine speed (1/m)
static const uint8_t tInjectorPulseCount =			tEngineSpeed + 1;				// fuel injector pulse count
static const uint8_t tVSSpulseCount =				tInjectorPulseCount + 1;		// VSS pulse count
#define nextAllowedValue tVSSpulseCount + 1

#ifdef useFuelCost
static const uint8_t tFuelCostUsed =				nextAllowedValue;				// cost of fuel quantity used
static const uint8_t tFuelCostTank =				tFuelCostUsed + 1;				// full tank fuel cost in currency units
static const uint8_t tFuelCostReserve =				tFuelCostTank + 1;				// reserve fuel quantity fuel cost in currency units
static const uint8_t tFuelCostBingo =				tFuelCostReserve + 1;			// bingo fuel quantity cost in currency units
static const uint8_t tFuelCostRemaining =			tFuelCostBingo + 1;				// value of estimated remaining fuel quantity in currency units
static const uint8_t tFuelCostReserveRemaining =	tFuelCostRemaining + 1;			// value of estimated remaining reserve fuel quantity in currency units
static const uint8_t tFuelCostBingoRemaining =		tFuelCostReserveRemaining + 1;	// value of estimated remaining bingo fuel quantity in currency units
static const uint8_t tFuelRateCost =				tFuelCostBingoRemaining + 1;	// fuel rate cost in currency units
#define nextAllowedValue tFuelRateCost + 1
#endif // useFuelCost

#ifdef useDebugAnalog
static const uint8_t tAnalogChannel =				nextAllowedValue;				// DC voltage
#define nextAllowedValue tAnalogChannel + 1
#endif // useDebugAnalog

#ifdef useCarVoltageOutput
static const uint8_t tAlternatorChannel =			nextAllowedValue;				// DC voltage
#define nextAllowedValue tAlternatorChannel + 1
#endif // useCarVoltageOutput

#ifdef useDragRaceFunction
static const uint8_t tAccelTestTime =				nextAllowedValue;				// acceleration test time (s.s)
#define nextAllowedValue tAccelTestTime + 1
#endif // useDragRaceFunction

static const uint8_t dfMaxValNonConversion =		nextAllowedValue;				// maximum index for function indexes that return results that are not affected by metric conversions

static const uint8_t tFuelUsed =					nextAllowedValue;				// fuel quantity used (SI/SAE)
static const uint8_t tFuelRate =					tFuelUsed + 1;					// fuel consumption rate (SI/SAE)
static const uint8_t tDistance =					tFuelRate + 1;					// vehicle distance traveled (SI/SAE)
static const uint8_t tSpeed =						tDistance + 1;					// vehicle speed (SI/SAE)
static const uint8_t tRemainingFuel =				tSpeed + 1;						// estimated total remaining fuel quantity (SI/SAE)
static const uint8_t tReserveRemainingFuel =		tRemainingFuel + 1;				// estimated reserve remaining fuel quantity (SI/SAE)
static const uint8_t tBingoRemainingFuel =			tReserveRemainingFuel + 1;		// estimated bingo remaining fuel quantity (SI/SAE)
static const uint8_t tRangeDistance =				tBingoRemainingFuel + 1;		// estimated total fuel tank distance (SI/SAE)
static const uint8_t tReserveDistance =				tRangeDistance + 1;				// estimated reserve fuel tank distance (SI/SAE)
static const uint8_t tBingoDistance =				tReserveDistance + 1;			// estimated bingo fuel tank distance (SI/SAE)
static const uint8_t tDistanceToEmpty =				tBingoDistance + 1;				// estimated remaining distance (SI/SAE)
static const uint8_t tReserveDistanceToEmpty =		tDistanceToEmpty + 1;			// estimated reserve remaining distance (SI/SAE)
static const uint8_t tBingoDistanceToEmpty =		tReserveDistanceToEmpty + 1;	// estimated bingo remaining distance (SI/SAE)
#define nextAllowedValue tBingoDistanceToEmpty + 1
#ifdef useFuelCost
static const uint8_t tFuelCostPerDistance =			nextAllowedValue;				// fuel cost per unit distance (SI/SAE)
static const uint8_t tDistancePerFuelCost =			tFuelCostPerDistance + 1;		// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue tDistancePerFuelCost + 1
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
static const uint8_t tPressureChannel =				nextAllowedValue;				// absolute pressure (SI/SAE)
#define nextAllowedValue tPressureChannel + 1
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
static const uint8_t tEstimatedEnginePower =		nextAllowedValue;				// estimated engine power (SI/SAE)
static const uint8_t tDragSpeed =					tEstimatedEnginePower + 1;		// acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#define nextAllowedValue tDragSpeed + 1
#endif // useDragRaceFunction

static const uint8_t dfMaxValSingleFormat =			nextAllowedValue;				// maximum index for function indexes that return results that only have single metric formats

static const uint8_t tFuelEcon =					nextAllowedValue;				// fuel economy (SI/SAE)
#define nextAllowedValue tFuelEcon + 1

static const uint8_t dfMaxValDisplayCount =			nextAllowedValue;				// maximum index for function indexes that return results for display

static const uint8_t tCalculateRemainingTank =		nextAllowedValue;				// calculate estimated remaining fuel quantity in injector open cycles
static const uint8_t tCalculateRemainingReserve =	tCalculateRemainingTank + 1;	// calculate estimated remaining fuel reserve value in injector open cycles
static const uint8_t tCalculateBingoFuel =			tCalculateRemainingReserve + 1;	// calculate estimated fuel bingo value in injector open cycles
static const uint8_t tConvertToMicroSeconds =		tCalculateBingoFuel + 1;
static const uint8_t tCalculateFuelQuantity =		tConvertToMicroSeconds + 1;
static const uint8_t tCalculateFuelDistance =		tCalculateFuelQuantity + 1;
static const uint8_t tCalculateFuelTime =			tCalculateFuelDistance + 1;
static const uint8_t tFormatToTime =				tCalculateFuelTime + 1;
static const uint8_t tFormatToNumber =				tFormatToTime + 1;
static const uint8_t tRoundOffNumber =				tFormatToNumber + 1;
static const uint8_t tLoadTrip =					tRoundOffNumber + 1;
static const uint8_t tSaveTrip =					tLoadTrip + 1;
static const uint8_t tReadTicksToSeconds =			tSaveTrip + 1;
#define nextAllowedValue tReadTicksToSeconds + 1
#ifdef useBarFuelEconVsTime
static const uint8_t tFEvTgetDistance =				nextAllowedValue;
static const uint8_t tFEvTgetConsumedFuel =			tFEvTgetDistance + 1;
static const uint8_t tFEvTgetFuelEconomy =			tFEvTgetConsumedFuel + 1;
#define nextAllowedValue tFEvTgetFuelEconomy + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t tFEvSgetDistance =				nextAllowedValue;
static const uint8_t tFEvSgetConsumedFuel =			tFEvSgetDistance + 1;
static const uint8_t tFEvSgetFuelEconomy =			tFEvSgetConsumedFuel + 1;
#define nextAllowedValue tFEvSgetFuelEconomy + 1
#endif // useBarFuelEconVsSpeed
#ifdef useDebugTerminal
static const uint8_t tParseCharacterToReg =			nextAllowedValue;
#define nextAllowedValue tParseCharacterToReg + 1
#endif //useDebugTerminal

#if (useDebugTerminalLabels) || (useSWEET64trace)
static const char terminalTripFuncNames[] PROGMEM = {
	"tEngineRunTime\r"					// engine runtime (hhmmss)
	"tRangeTime\r"						// estimated total runtime from full tank (hhmmss)
	"tReserveTime\r"					// estimated reserve runtime from full tank (hhmmss)
	"tBingoTime\r"						// estimated bingo fuel runtime from full tank (hhmmss)
	"tTimeToEmpty\r"					// estimated remaining total tank engine runtime (hhmmss)
	"tReserveTimeToEmpty\r"				// estimated remaining reserve engine runtime (hhmmss)
	"tBingoTimeToEmpty\r"				// estimated bingo quantity engine runtime (hhmmss)
	"tMotionTime\r"						// time vehicle in motion (hhmmss)
	"tInjectorOpenTime\r"				// fuel used (microseconds)
	"tInjectorTotalTime\r"				// engine run time (microseconds)
	"tVSStotalTime\r"					// time vehicle in motion (microseconds)
	"tEngineSpeed\r"					// engine speed (1/m)
	"tInjectorPulseCount\r"				// fuel injector pulse count
	"tVSSpulseCount\r"					// VSS pulse count
#ifdef useFuelCost
	"tFuelCostUsed\r"					// cost of fuel quantity used
	"tFuelCostTank\r"					// full tank fuel cost in currency units
	"tFuelCostReserve\r"				// reserve fuel quantity fuel cost in currency units
	"tFuelCostBingo\r"					// bingo fuel quantity cost in currency units
	"tFuelCostRemaining\r"				// value of estimated remaining fuel quantity in currency units
	"tFuelCostReserveRemaining\r"		// value of estimated remaining reserve fuel quantity in currency units
	"tFuelCostBingoRemaining\r"			// value of estimated remaining bingo fuel quantity in currency units
	"tFuelRateCost\r"					// fuel rate cost in currency units
#endif // useFuelCost
#ifdef useDebugAnalog
	"tAnalogChannel\r"					// DC voltage
#endif // useDebugAnalog
#ifdef useCarVoltageOutput
	"tAlternatorChannel\r"				// DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	"tAccelTestTime\r"					// acceleration test time (s.s)
#endif // useDragRaceFunction
	"tFuelUsed\r"						// fuel quantity used (SI/SAE)
	"tFuelRate\r"						// fuel consumption rate (SI/SAE)
	"tDistance\r"						// vehicle distance traveled (SI/SAE)
	"tSpeed\r"							// vehicle speed (SI/SAE)
	"tRemainingFuel\r"					// estimated total remaining fuel quantity (SI/SAE)
	"tReserveRemainingFuel\r"			// estimated reserve remaining fuel quantity (SI/SAE)
	"tBingoRemainingFuel\r"				// estimated bingo remaining fuel quantity (SI/SAE)
	"tRangeDistance\r"					// estimated total fuel tank distance (SI/SAE)
	"tReserveDistance\r"				// estimated reserve fuel tank distance (SI/SAE)
	"tBingoDistance\r"					// estimated bingo fuel tank distance (SI/SAE)
	"tDistanceToEmpty\r"				// estimated remaining distance (SI/SAE)
	"tReserveDistanceToEmpty\r"			// estimated reserve remaining distance (SI/SAE)
	"tBingoDistanceToEmpty\r"			// estimated bingo remaining distance (SI/SAE)
#ifdef useFuelCost
	"tFuelCostPerDistance\r"			// fuel cost per unit distance (SI/SAE)
	"tDistancePerFuelCost\r"			// distance per unit fuel cost (SI/SAE)
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	"tPressureChannel\r"				// absolute pressure (SI/SAE)
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	"tEstimatedEnginePower\r"			// estimated engine power (SI/SAE)
	"tDragSpeed\r"						// acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#endif // useDragRaceFunction
	"tFuelEcon\r"						// fuel economy (SI/SAE)
};

#endif // (useDebugTerminalLabels) || (useSWEET64trace)
static const uint8_t prgmEngineSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,		// load injector pulse count into register 2
	instrMul2byConst, idxCycles0PerSecond,				// set up for conversion of denominator injector cycle count to time in seconds
	instrMul2byByte, 60,								// set up for conversion of denominator injector time in seconds to time in minutes
	instrMul2byConst, idxDecimalPoint,					// perform output decimal formatting
	instrMul2byEEPROM, pCrankRevPerInjIdx,				// multiply by the number of crank revolutions per injector fire event
	instrDiv2byTripVarIndexed, rvEngCycleIdx,			// divide by the injector pulse time
	instrDone											// exit to caller
};

static const uint8_t prgmMotionTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrDiv2byConst, idxCycles0PerSecond,				// divide by cycles per second value
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

static const uint8_t prgmEngineRunTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,		// load injector pulse cycle value into register 2
	instrDiv2byConst, idxCycles0PerSecond,				// divide by cycles per second value
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

static const uint8_t prgmRangeTime[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmReserveTime[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmBingoTime[] PROGMEM = {
	instrLdRegMain, 0x02, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmReserveTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmBingoTimeToEmpty[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmCalculateFuelTime[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 10,								// if inputted fuel quantity is zero, exit to caller
	instrMul2byConst, idxMicroSecondsPerSecond,			// shift number upward to retain numeric precision
	instrDiv2byTripVarIndexed, rvInjCycleIdx,			// divide by open injector time in cycles
	instrMul2byTripVarIndexed, rvEngCycleIdx,			// multiply by injector open count
	instrDiv2byConst, idxCycles0PerSecond,				// convert output term from cycles remaining to seconds remaining
	instrDiv2byConst, idxMicroSecondsPerSecond,			// shift number downward - high numeric precision no longer needed

//cont:
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

static const uint8_t prgmDistance[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS cycle value into register 2
	instrMul2byConst, idxDecimalPoint,					// perform output decimal formatting
	instrDiv2byEEPROM, pPulsesPerDistanceIdx,			// divide by pulse per distance value and store in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmSpeed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrBranchIfZero, 14,								// if zero, then speed is also zero
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// set up to convert pulses per unit distance
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS pulse count
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term
	instrMul2byConst, idxCycles0PerSecond,				// set up to convert VSS cycle value to time in seconds
	instrMul2byConst, idxSecondsPerHour,				// set up to convert VSS time in seconds to time in hours
	instrDiv2by1,										// divide to obtain unit distance per hour

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmFuelUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// get amount of consumed fuel in cycles
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity value
};

static const uint8_t prgmRemainingFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel quantity value
};

static const uint8_t prgmReserveRemainingFuel[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the reserve fuel value
};

static const uint8_t prgmBingoRemainingFuel[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegConst, 0x01, idxDecimalPoint,				// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the bingo fuel value
};

static const uint8_t prgmCalculateFuelQuantity[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 4,								// if inputted fuel quantity is zero, exit to caller
	instrMul2by1,										// multiply the fuel used value by the formatting term
	instrDiv2byMain, mpCyclesPerVolumeIdx,				// convert fuel value in cycles to fuel value per unit quantity
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmRangeDistance[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmReserveDistance[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelDistance					// go format it
};

#ifdef useJSONoutput
static const uint8_t prgmFindHalfReserveRange[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,			// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRegRight, 0x02,							// shift result right one bit
	instrJump, tCalculateFuelDistance					// go format it
};

#endif // useJSONoutput
static const uint8_t prgmBingoDistance[] PROGMEM = {
	instrLdRegMain, 0x02, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmReserveDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmBingoDistanceToEmpty[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmCalculateFuelDistance[] PROGMEM = {
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 18,								// if inputted fuel quantity is zero, exit to caller
	instrMul2byTripVarIndexed, rvVSSpulseIdx,			// multiply input fuel quantity by accumulated distance travelled pulses to get the numerator for distance to empty
	instrLdReg, 0x23,									// move numerator to register 3 for now
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycles
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply accumulated fuel injector open cycles by pulses per unit distance factor
	instrDiv2byConst, idxDecimalPoint,					// divide by decimal point constant to get denominator / decimal factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrLdReg, 0x21,									// move modified denominator to register 1 for now
	instrLdReg, 0x32,									// fetch numerator from register 3
	instrDiv2by1,										// divide to get DTE = (numerator) / (denominator / decimal factor ) = (numerator * decimal factor) / denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone											// exit to caller
};

#ifdef useFuelCost
static const uint8_t prgmFuelCostUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// get amount of consumed fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostTank[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostReserve[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostBingo[] PROGMEM = {
	instrLdRegMain, 0x02, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostRemaining[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

static const uint8_t prgmFuelCostReserveRemaining[] PROGMEM = {
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

static const uint8_t prgmFuelCostBingoRemaining[] PROGMEM = {
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel monetary value
};

static const uint8_t prgmFuelCostPerDistance[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get the denominator for fuel cost per distance
	instrLdReg, 0x21,									// save denominator term
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel cost per distance
	instrMul2byEEPROM, pCostPerQuantity,				// multiply the numerator by the fuel cost per unit quantity
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

static const uint8_t prgmDistancePerFuelCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the denominator for distance per fuel cost
	instrMul2byEEPROM, pCostPerQuantity,				// multiply the denominator by the cost per unit quantity of fuel
	instrLdReg, 0x21,									// save the denominator term
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the numerator for distance per fuel cost
	instrMul2byConst, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2byConst, idxDecimalPoint,					// load the decimal point constant used for output formatting
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

#endif // useFuelCost
static const uint8_t prgmFuelRate[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector open cycle value
	instrBranchIfZero, 11,								// if zero, then fuel rate is also zero
	instrMul2byConst, idxSecondsPerHour,
	instrMul2byConst, idxDecimalPoint,
	instrDiv2byTripVarIndexed, rvEngCycleIdx,
	instrMul2byConst, idxCycles0PerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrDiv2byMain, mpCyclesPerVolumeIdx,				// multiply by quantity/cycle (divide by cycles/quantity)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone
};

#ifdef useFuelCost
static const uint8_t prgmFuelRateCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector open cycle value
	instrBranchIfZero, 11,								// if zero, then fuel rate is also zero
	instrMul2byConst, idxSecondsPerHour,
	instrMul2byEEPROM, pCostPerQuantity,
	instrDiv2byTripVarIndexed, rvEngCycleIdx,
	instrMul2byConst, idxCycles0PerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrDiv2byMain, mpCyclesPerVolumeIdx,				// multiply by quantity/cycle (divide by cycles/quantity)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone
};

#endif // useFuelCost
static const uint8_t prgmFuelEcon[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator for later
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino is in metric mode, skip ahead
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byConst, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or km/L)
	instrSkip, 2,										// go skip ahead

//fecont1:
	instrMul2byConst, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for L/100km or gal/100mi)

//fecont2:
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateRemainingTank[] PROGMEM = {
	instrLdRegTripVar, 0x01, tankIdx, rvInjCycleIdx,	// fetch tank quantity in injector open cycles
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
#ifdef usePartialRefuel
	instrSubMainFromX, 0x01, mpPartialRefuelTankSize,	// subtract calculated partial refuel size in injector open cycles
	instrBranchIfGT, 7,									// if calculated partial refuel size > tank quantity, exit with calculated tank size in result
#endif // usePartialRefuel
	instrSubYfromX, 0x12,								// subtract tank injector time in cycles from calculated tank size in cycles to get remaining fuel in cycles
	instrBranchIfLTorE,	3,								// if tank quantity <= calculated tank size, exit
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateRemainingReserve[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 8,								// if zero, then reserve fuel is also zero
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo tank size in injector open cycles
	instrBranchIfLTorE, 3,								// if remaining fuel >= bingo tank size, exit to caller
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateBingoFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in injector open cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 9,								// if zero, then bingo fuel is also zero
	instrLdRegMain, 0x01, mpBingoTankSizeIdx,			// fetch bingo tank size in injector open cycles
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if tank quantity in injector open cycles <= calculated bingo size in injector open cycles, exit to caller
	instrLdReg, 0x12,									// shift calculated bingo size in injector open cycles to register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmInjectorOpenTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,
	instrJump, tConvertToMicroSeconds
};

static const uint8_t prgmInjectorTotalTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,
	instrJump, tConvertToMicroSeconds
};

static const uint8_t prgmVSStotalTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrJump, tConvertToMicroSeconds
};

static const uint8_t prgmConvertToMicroSeconds[] PROGMEM = {
	instrMul2byConst, idxMicroSecondsPerSecond,			// multiply by conversion factor for microseconds per second
	instrDiv2byConst, idxCycles0PerSecond,				// divide by conversion factor for cycles to seconds
	instrDone											// exit to caller
};

static const uint8_t prgmVSSpulseCount[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,
	instrMul2byConst, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrDone
};

static const uint8_t prgmInjectorPulseCount[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,
	instrMul2byConst, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrDone
};

#ifdef useDebugAnalog
static const uint8_t prgmAnalogChannel[] PROGMEM = {
	instrCmpIndex, dfMaxValAnalogCount,					// is trip index pointing to a valid analog channel?
	instrBranchIfLT, 4,									// if so, skip ahead
	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

//cont1:
	instrLdRegVoltageIndexed, 0x02,						// load analog channel ADC step value
	instrMul2byConst, idxDenomVoltage,					// perform multiply (we're converting from ADC steps to voltage)
	instrDiv2byConst, idxNumerVoltage,					// divide by numerator to convert from voltage to ADC steps
	instrDone											// exit to caller
};

#endif // useDebugAnalog
#ifdef useCarVoltageOutput
static const uint8_t prgmAlternatorChannel[] PROGMEM = {
	instrLdRegVoltage, 0x02, analogAlternatorChannelIdx,	// load analog channel ADC step value
	instrMul2byConst, idxDenomVoltage,					// perform multiply (we're converting from ADC steps to voltage)
	instrDiv2byConst, idxNumerVoltage,					// divide by numerator to get voltage drop V(R5) between R5 and ground
	instrLdRegConst, 0x01, idxResistanceR5,				// figure out V(R5+R6) from V(R5)
	instrAddConstToX, 0x01, idxResistanceR6,
	instrMul2by1,
	instrDiv2byConst, idxResistanceR5,
	instrAddEEPROMtoX, 0x02, pVoltageOffset,			// add diode offset
	instrDone											// exit to caller
};

#endif // useCarVoltageOutput
static const uint8_t prgmRoundOffNumber[] PROGMEM = {
	instrTestReg, 0x02,									// test register 2
	instrBranchIfOverflow, 23,							// if register 2 has overflow value, exit
	instrCmpIndex, 2,									// check if 3 or more right hand digits were specified
	instrBranchIfGT, 17,								// if so, just exit
	instrBranchIfE, 12,									// if 2 right hand digits were specified, round to nearest 100th
	instrCmpIndex, 1,									// check if 0 or 1 right-hand digits were specified
	instrBranchIfE, 4,									// if 1 right hand digit was specified, round to nearest 10th
	instrDiv2byConst, idxDecimalPoint,					// shift number right 3 digits to round to nearest whole digit
	instrSkip, 6,										// skip to adjustment

//to10ths:
	instrDiv2byByte, 100,								// shift number right 2 digits
	instrSkip, 2,										// skip to adjustment

//to100ths:
	instrDiv2byByte, 10,								// shift number right 1 digit

//adjust:
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//exit:
	instrJump, tFormatToNumber							// go call prgmFormatToNumber to perform actual formatting
};

static const uint8_t prgmFormatToNumber[] PROGMEM = {
	instrTestReg, 0x02,									// test register 2
	instrBranchIfOverflow, 13,							// if register 2 has overflow value, exit
	instrCmpIndex, 3,									// check if valid number of decimal points were requested
	instrBranchIfGT, 9,									// if too many decimal points were requested, error out with overflow
	instrDiv2byConst, idxBCDdivisor,					// divide by 10^8 - quotient remains in register 2, and remainder goes into register 1
	instrLdRegByte, 0x03, 99,
	instrCmpXtoY, 0x23,									// test if quotient is greater than 99 (remainder will be less than 10^8)
	instrBranchIfLTorE, 4,								// if quotient is 99 or less, continue with processing

//overflow:
	instrDoBCDadjust, 0x12, 0x02,						// store overflow BCD string in register 2
	instrDone,

//cont:
	instrDoBCDadjust, 0x12, 0x00,						// process register 1 as 10 digit BCD string and store it in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmFetchParameterValue[] PROGMEM = {
	instrLdRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

static const uint8_t prgmWriteParameterValue[] PROGMEM = {
	instrStRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

static const uint8_t prgmFetchInitialParamValue[] PROGMEM = {
	instrLdRegEinitIndexed, 0x02,
	instrDone											// return to caller
};

#ifdef useEEPROMtripStorage
static const uint8_t prgmLoadCurrentFromEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pCurrTripSignatureIdx,
	instrLdRegByte, 0x02, guinosig,
	instrCmpXtoY, 0x21,
	instrBranchIfE, 4,
	instrLdRegByte, 0x02, 0,
	instrDone,

//cont:
	instrLxdI, EEPROMcurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, currentIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, EEPROMeocIdleCurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, eocIdleCurrentIdx,
#endif // trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLdRegByte, 0x02, 1,
	instrDone
};

static const uint8_t prgmSaveCurrentToEEPROM[] PROGMEM = {
	instrLdRegByte, 0x02, guinosig,
	instrStRegEEPROM, 0x02, pCurrTripSignatureIdx,
	instrLxdI, currentIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMcurrentIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, eocIdleCurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMeocIdleCurrentIdx,
#endif // trackIdleEOCdata
	instrJump, tSaveTrip,
};

static const uint8_t prgmLoadTankFromEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pTankTripSignatureIdx,
	instrLdRegByte, 0x02, guinosig,
	instrCmpXtoY, 0x21,
	instrBranchIfE, 4,
	instrLdRegByte, 0x02, 0,
	instrDone,

//cont:
#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSaveSizeIdx,
	instrStRegEEPROM, 0x02, pRefuelSizeIdx,
#endif // usePartialRefuel
	instrLxdI, EEPROMtankIdx,
	instrCall, tLoadTrip,
	instrLxdI, tankIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, EEPROMeocIdleTankIdx,
	instrCall, tLoadTrip,
	instrLxdI, eocIdleTankIdx,
#endif // trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLdRegByte, 0x02, 1,
	instrDone
};

static const uint8_t prgmSaveTankToEEPROM[] PROGMEM = {
	instrLdRegByte, 0x02, guinosig,
	instrStRegEEPROM, 0x02, pTankTripSignatureIdx,
#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,
	instrStRegEEPROM, 0x02, pRefuelSaveSizeIdx,
#endif // usePartialRefuel
	instrLxdI, tankIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMtankIdx,
#ifdef trackIdleEOCdata
	instrCall, tSaveTrip,
	instrLxdI, eocIdleTankIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMeocIdleTankIdx,
#endif // trackIdleEOCdata
	instrJump, tSaveTrip,
};

#endif // useEEPROMtripStorage
static const uint8_t prgmLoadTrip[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x01, rvVSSpulseIdx,
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrLdRegTripVarIndexed, 0x03, rvInjPulseIdx,
	instrLdRegTripVarIndexed, 0x04, rvInjCycleIdx,
	instrLdRegTripVarIndexed, 0x05, rvEngCycleIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmSaveTrip[] PROGMEM = {
	instrStRegTripVarIndexed, 0x01, rvVSSpulseIdx,
	instrStRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrStRegTripVarIndexed, 0x03, rvInjPulseIdx,
	instrStRegTripVarIndexed, 0x04, rvInjCycleIdx,
	instrStRegTripVarIndexed, 0x05, rvEngCycleIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmReadTicksToSeconds[] PROGMEM = {
	instrLdRegVolatileIndexed, 0x02,
	instrDiv2byConst, idxTicksPerSecond,
	instrJump, tFormatToTime							// go format the number to hhmmss time
};

#ifdef useBarFuelEconVsTime
static const uint8_t prgmFEvTgetDistance[] PROGMEM = {
	instrLdRegTripFEvTindexed, 0x02, rvVSSpulseIdx,		// load indexed VSS pulses from fuel econ vs time trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvTgetConsumedFuel[] PROGMEM = {
	instrLdRegTripFEvTindexed, 0x02, rvInjCycleIdx,		// load indexed injector cycles from fuel econ vs time trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvTgetFuelEconomy[] PROGMEM = {
	instrLdRegTripFEvTindexed, 0x02, rvVSSpulseIdx,		// load indexed VSS pulses from fuel econ vs time trip variable bank
	instrBranchIfE, 26,
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator (for G/100mi or L/100km) for later
	instrLdRegTripFEvTindexed, 0x02, rvInjCycleIdx,		// load indexed injector cycles from fuel econ vs time trip variable bank
	instrBranchIfE, 22,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino currently outputting fuel econ as fuel over distance, skip
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byConst, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or k/L)
	instrSkip, 2,										// skip ahead

//cont0:
	instrMul2byConst, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for G/100mi or L/100km)
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrClearFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags clear, to signal good data
	instrDone,											// return to caller

//zero0:
	instrBranchIfFuelOverDist, 5,						// if MPGuino currently outputting fuel econ as fuel over distance, denominator is zero
//zero:
	instrClearFlag, SWEET64overflowFlag,				// clear V flag since this would have divided just fine otherwise
	instrDone,											// return to caller

//ovfl0:
	instrBranchIfFuelOverDist, 249,						// if MPGuino currently outputting fuel econ as fuel over distance, numerator is zero
//ovfl:
	instrSetFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags set, to signal unusable data (division by 0 would have resulted)
	instrDone											// return to caller
};

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t prgmFEvSgetDistance[] PROGMEM = {
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvVSSpulseIdx,	// load indexed VSS pulses from fuel econ vs speed trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvSgetConsumedFuel[] PROGMEM = {
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvInjCycleIdx,	// load indexed injector cycles from fuel econ vs speed trip variable bank
	instrClearFlag, SWEET64overflowFlag,				// Z flag is set on data, and V flag is clear
	instrDone											// return to caller
};

static const uint8_t prgmFEvSgetFuelEconomy[] PROGMEM = {
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvVSSpulseIdx,	// load indexed VSS pulses from fuel econ vs speed trip variable bank
	instrBranchIfE, 27,
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator (for G/100mi or L/100km) for later
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvInjCycleIdx,	// load indexed injector cycles from fuel econ vs speed trip variable bank
	instrBranchIfE, 22,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino currently outputting fuel econ as fuel over distance, skip
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byConst, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or k/L)
	instrSkip, 2,										// skip ahead

//cont0:
	instrMul2byConst, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for G/100mi or L/100km)
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrClearFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags clear, to signal good data
	instrDone,											// return to caller

//zero0:
	instrBranchIfFuelOverDist, 5,						// if MPGuino currently outputting fuel econ as fuel over distance, denominator is zero
//zero:
	instrClearFlag, SWEET64overflowFlag,				// clear V flag since this would have divided just fine otherwise
	instrDone,											// return to caller

//ovfl0:
	instrBranchIfFuelOverDist, 249,						// if MPGuino currently outputting fuel econ as fuel over distance, numerator is zero
//ovfl:
	instrSetFlag, SWEET64zeroFlag | SWEET64overflowFlag,	// ensure Z and V flags set, to signal unusable data (division by 0 would have resulted)
	instrDone											// return to caller
};

#endif // useBarFuelEconVsSpeed
#ifdef useDebugTerminal
static const uint8_t prgmTerminalWriteParameterValue[] PROGMEM = {
	instrStRegEEPROMindexed, 0x06,
	instrDone											// return to caller
};

static const uint8_t prgmParseCharacterToReg[] PROGMEM = {
	instrAddIndexToX, 0x02,								// add whatever's in the trip variable index to main register
	instrLdReg, 0x26,									// load main register into terminal register
	instrLdRegByte, 0x01, 0xFF,							// is parsed number less than 256?
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,
	instrLdReg, 0x12,
	instrDone											// exit to caller
};

#endif // useDebugTerminal
static const uint8_t prgmFormatToTime[] PROGMEM = {
	instrLdReg, 0x21,									// move time in seconds into register 1
	instrDoBCDadjust, 0x12, 0x01,						// process register 1 as hhmmss BCD string and store it in register 2
	instrDone											// exit to caller
};

#ifdef useChryslerMAPCorrection
static const uint8_t prgmPressureChannel[] PROGMEM = {
	instrCmpIndex, mpAnalogMAPfloorIdx - mpMAPpressureIdx,	// is trip index pointing to a valid pressure element?
	instrBranchIfLT, 4,									// if so, skip ahead
	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

//cont1:
	instrLdRegMainOffset, 0x02, mpMAPpressureIdx,		// load pre-formatted pressure element
	instrDone											// exit to caller
};

#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
static const uint8_t prgmDragSpeed[] PROGMEM = {
	instrLdRegVolatile, 0x02, vDragInstantSpeedIdx,		// load instantaneous drag speed measurement
	instrTestReg, 0x02,									// test speed measurement
	instrBranchIfZero, 12,								// if speed measurement is zero, exit to caller
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// set up to convert pulses per unit distance
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// set up to convert VSS cycle value to time in seconds
	instrMul2byConst, idxDecimalPoint,					// load decimal formatting term
	instrMul2byConst, idxSecondsPerHour,				// set up to convert VSS time in seconds to time in hours
	instrDiv2by1,										// divide to obtain unit distance per hour

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmEstimateEnginePower[] PROGMEM = {
	instrCall, tDragSpeed,								// calculate vehicle speed (this will be the maximum vehicle speed obtained during the acceleration test)
	instrBranchifSAEmode, 5,							// if MPGuino is in SAE mode, skip to power calculation setup

//cont1:
	instrMul2byConst, idxDenomDistance,					// multiply by numerator to convert km/hr to MPH
	instrDiv2byConst, idxNumerDistance					// divide by denominator to convert km/hr to MPH
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont2:
	instrLdReg, 0x21,									// load register 1 with calculated vehicle speed
	instrMul2by1,										// perform cube function on maximum accel test vehicle speed value to obtain initial denominator term
	instrMul2by1,
	instrDiv2byConst, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byEEPROM, pVehicleMassIdx,					// multiply estimate by vehicle weight
	instrDiv2byConst, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrBranchifSAEmode, 5,							// if MPGuino is in SAE mode, skip to power calculation setup

//cont3:
	instrMul2byConst, idxDenomMass,						// multiply by numerator to convert kg to lbf
	instrDiv2byConst, idxNumerMass,						// divide by denominator to convert kg to lbf
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont4:
	instrDiv2byConst, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmAccelTestTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrMul2byConst, idxDecimalPoint,					// load the decimal point constant used for output formatting
	instrDiv2byConst, idxCycles0PerSecond,
	instrDone
};

#endif // useDragRaceFunction
const uint8_t * const S64programList[] PROGMEM = {
// these SWEET64 program addresses correspond to the display functions to either screen or logging output
	 prgmEngineRunTime									// tEngineRunTime - engine runtime (hhmmss)
	,prgmRangeTime										// tRangeTime - estimated total runtime from full tank (hhmmss)
	,prgmReserveTime									// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	,prgmBingoTime										// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	,prgmTimeToEmpty									// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	,prgmReserveTimeToEmpty								// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	,prgmBingoTimeToEmpty								// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	,prgmMotionTime										// tMotionTime - time vehicle in motion (hhmmss)
	,prgmInjectorOpenTime								// tInjectorOpenTime - fuel used (microseconds)
	,prgmInjectorTotalTime								// tInjectorTotalTime - engine run time (microseconds)
	,prgmVSStotalTime									// tVSStotalTime - time vehicle in motion (microseconds)
	,prgmEngineSpeed									// tEngineSpeed - engine speed (1/m)
	,prgmInjectorPulseCount								// tInjectorPulseCount - fuel injector pulse count
	,prgmVSSpulseCount									// tVSSpulseCount - VSS pulse count
#ifdef useFuelCost
	,prgmFuelCostUsed									// tFuelCostUsed - cost of fuel quantity used
	,prgmFuelCostTank									// tFuelCostTank - full tank fuel cost in currency units
	,prgmFuelCostReserve								// tFuelCostReserve - reserve fuel quantity fuel cost in currency units
	,prgmFuelCostBingo									// tFuelCostBingo - bingo fuel quantity cost in currency units
	,prgmFuelCostRemaining								// tFuelCostRemaining - value of estimated remaining total fuel quantity in currency units
	,prgmFuelCostReserveRemaining						// tFuelCostReserveRemaining - value of estimated remaining reserve fuel quantity in currency units
	,prgmFuelCostBingoRemaining							// tFuelCostBingoRemaining - value of estimated remaining bingo fuel quantity in currency units
	,prgmFuelRateCost									// tFuelRateCost - fuel rate cost in currency units
#endif // useFuelCost
#ifdef useDebugAnalog
	,prgmAnalogChannel									// tAnalogChannel - DC voltage
#endif // useDebugAnalog
#ifdef useCarVoltageOutput
	,prgmAlternatorChannel								// tAlternatorChannel - DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	,prgmAccelTestTime									// tAccelTestTime - acceleration test time (s.s)
#endif // useDragRaceFunction
	,prgmFuelUsed										// tFuelUsed - fuel quantity used (SI/SAE)
	,prgmFuelRate										// tFuelRate - fuel consumption rate (SI/SAE)
	,prgmDistance										// tDistance - vehicle distance traveled (SI/SAE)
	,prgmSpeed											// tSpeed - vehicle speed (SI/SAE)
	,prgmRemainingFuel									// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	,prgmReserveRemainingFuel							// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	,prgmBingoRemainingFuel								// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
	,prgmRangeDistance									// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	,prgmReserveDistance								// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	,prgmBingoDistance									// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	,prgmDistanceToEmpty								// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	,prgmReserveDistanceToEmpty							// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	,prgmBingoDistanceToEmpty							// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
#ifdef useFuelCost
	,prgmFuelCostPerDistance							// tFuelCostPerDistance - fuel cost per unit distance (SI/SAE)
	,prgmDistancePerFuelCost							// tDistancePerFuelCost - distance per unit fuel cost (SI/SAE)
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,prgmPressureChannel								// tPressureChannel - absolute pressure (SI/SAE)
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,prgmEstimateEnginePower							// tEstimatedEnginePower - estimated engine power (SI/SAE)
	,prgmDragSpeed										// tDragSpeed - acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#endif // useDragRaceFunction
	,prgmFuelEcon										// tFuelEcon - fuel economy (SI/SAE)

// this is the start of the internal SWEET64 index program address list
	,prgmCalculateRemainingTank							// tCalculateRemainingTank - calculate estimated remaining fuel quantity in injector open cycles
	,prgmCalculateRemainingReserve						// tCalculateRemainingReserve - calculate estimated remaining fuel reserve value in injector open cycles
	,prgmCalculateBingoFuel								// tCalculateBingoFuel - calculate estimated fuel bingo value in injector open cycles
	,prgmConvertToMicroSeconds							// tCalculateFuelQuantity
	,prgmCalculateFuelQuantity							// tCalculateFuelQuantity
	,prgmCalculateFuelDistance							// tCalculateFuelDistance
	,prgmCalculateFuelTime								// tCalculateFuelTime
	,prgmFormatToTime									// tFormatToTime
	,prgmFormatToNumber									// tFormatToNumber
	,prgmRoundOffNumber									// tRoundOffNumber
	,prgmLoadTrip
	,prgmSaveTrip
	,prgmReadTicksToSeconds								// tReadTicksToSeconds
#ifdef useBarFuelEconVsTime
	,prgmFEvTgetDistance								// tFEvTgetDistance
	,prgmFEvTgetConsumedFuel							// tFEvTgetConsumedFuel
	,prgmFEvTgetFuelEconomy								// tFEvTgetFuelEconomy
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,prgmFEvSgetDistance								// tFEvSgetDistance
	,prgmFEvSgetConsumedFuel							// tFEvSgetConsumedFuel
	,prgmFEvSgetFuelEconomy								// tFEvSgetFuelEconomy
#endif // useBarFuelEconVsSpeed
#ifdef useDebugTerminal
	,prgmParseCharacterToReg
#endif //useDebugTerminal
};

#define nextAllowedValue 0 // S64programList
const uint8_t calcFormatTimeHHmmSSIdx =				nextAllowedValue;						// time in HHmmSS format
const uint8_t calcFormatTimeInMillisecondsIdx =		calcFormatTimeHHmmSSIdx + 1;			// time in milliseconds
const uint8_t calcFormatEngineSpeedIdx =			calcFormatTimeInMillisecondsIdx + 1;	// engine speed
const uint8_t calcFormatPulseCountIdx =				calcFormatEngineSpeedIdx + 1;			// pulse count
#define nextAllowedValue calcFormatPulseCountIdx + 1
#ifdef useFuelCost
const uint8_t calcFormatFuelCostIdx =				nextAllowedValue;						// fuel cost
const uint8_t calcFormatFuelRateCostIdx =			calcFormatFuelCostIdx + 1;				// fuel rate cost
#define nextAllowedValue calcFormatFuelRateCostIdx + 1
#endif // useFuelCost
#if defined(useAnalogRead)
const uint8_t calcFormatAnalogReadIdx =				nextAllowedValue;						// voltage
#define nextAllowedValue calcFormatAnalogReadIdx + 1
#endif // useAnalogRead
#ifdef useDragRaceFunction
const uint8_t calcFormatTimeInTenthsSecIdx =		nextAllowedValue;						// time in tenths of seconds
#define nextAllowedValue calcFormatTimeInTenthsSecIdx + 1
#endif // useDragRaceFunction
const uint8_t calcFormatFuelQuantityIdx =			nextAllowedValue;						// fuel quantity (SAE/SI)
const uint8_t calcFormatFuelRateIdx =				calcFormatFuelQuantityIdx + 2;			// fuel rate (SAE/SI)
const uint8_t calcFormatDistanceIdx =				calcFormatFuelRateIdx + 2;				// distance travelled (SAE/SI)
const uint8_t calcFormatSpeedIdx =					calcFormatDistanceIdx + 2;				// speed (SAE/SI)
#define nextAllowedValue calcFormatSpeedIdx + 2
#ifdef useFuelCost
const uint8_t calcFormatFuelCostPerDistanceIdx =	nextAllowedValue;						// fuel cost per unit distance (SI/SAE)
const uint8_t calcFormatDistancePerFuelCostIdx =	calcFormatFuelCostPerDistanceIdx + 2;	// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue calcFormatDistancePerFuelCostIdx + 2
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
const uint8_t calcFormatPressureIdx =				nextAllowedValue;						// absolute pressure (SI/SAE)
#define nextAllowedValue calcFormatPressureIdx + 2
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
const uint8_t calcFormatEstimatedPowerIdx =			nextAllowedValue;						// estimated engine power (SI/SAE)
#define nextAllowedValue calcFormatEstimatedPowerIdx + 2
#endif // useDragRaceFunction
const uint8_t calcFormatFuelEconomyIdx =			nextAllowedValue;						// fuel economy (SAE/SI)
#define nextAllowedValue calcFormatFuelEconomyIdx + 4

const uint8_t calcFormatIdxCount =					nextAllowedValue;

#ifdef useDebugTerminal
static const char terminalFormats[] PROGMEM = {
	"hhmmss\0"
	"ms\0"
	"rev/min\0"
	"pulses\0"
#ifdef useFuelCost
	"cost\0"
	"cost/hour\0"
#endif // useFuelCost
#if defined(useAnalogRead)
	"V(dc)\0"
#endif // useAnalogRead
#ifdef useDragRaceFunction
	"tenths\0"
#endif // useDragRaceFunction
	"gallon\0"
	"liter\0"
	"gallon/hour\0"
	"liter/hour\0"
	"mile\0"
	"km\0"
	"MPH\0"
	"kPH\0"
#ifdef useFuelCost
	"cost/mile\0"
	"cost/km\0"
	"mile/cost\0"
	"km/cost\0"
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	"psia\0"
	"kPa\0"
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	"WHP\0"
	"kW\0"
#endif // useDragRaceFunction
	"MPG\0"
	"L/100km\0"
	"gal/100miles\0"
	"LPK\0"
};

#endif // useDebugTerminal
const uint8_t calcFormatIdx[(unsigned int)(dfMaxValDisplayCount)] PROGMEM = { // S64programList
	 calcFormatTimeHHmmSSIdx					// tEngineRunTime - engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tRangeTime - estimated total runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	,calcFormatTimeHHmmSSIdx					// tMotionTime - time vehicle in motion (hhmmss)

	,calcFormatTimeInMillisecondsIdx			// tInjectorOpenTime - fuel used (milliseconds)
	,calcFormatTimeInMillisecondsIdx			// tInjectorTotalTime - engine run time (milliseconds)
	,calcFormatTimeInMillisecondsIdx			// tVSStotalTime - time vehicle in motion (milliseconds)
	,calcFormatEngineSpeedIdx					// tEngineSpeed - engine speed (1/m)
	,calcFormatPulseCountIdx					// tInjectorPulseCount - fuel injector pulse count
	,calcFormatPulseCountIdx					// tVSSpulseCount - VSS pulse count
#ifdef useFuelCost
	,calcFormatFuelCostIdx						// tFuelCostUsed - cost of fuel quantity used
	,calcFormatFuelCostIdx						// tFuelCostTank - full tank fuel cost in currency units
	,calcFormatFuelCostIdx						// tFuelCostReserve - reserve fuel quantity fuel cost in currency units
	,calcFormatFuelCostIdx						// tFuelCostBingo - bingo fuel quantity cost in currency units
	,calcFormatFuelCostIdx						// tFuelCostRemaining - value of estimated remaining total fuel quantity in currency units
	,calcFormatFuelCostIdx						// tFuelCostReserveRemaining - value of estimated remaining reserve fuel quantity in currency units
	,calcFormatFuelCostIdx						// tFuelCostBingoRemaining - value of estimated remaining bingo fuel quantity in currency units
	,calcFormatFuelRateCostIdx					// tFuelRateCost - fuel rate cost in currency units
#endif // useFuelCost
#ifdef useDebugAnalog
	,calcFormatAnalogReadIdx					// tAnalogChannel - DC voltage
#endif // useDebugAnalog
#ifdef useCarVoltageOutput
	,calcFormatAnalogReadIdx					// tAlternatorChannel - DC voltage
#endif // useCarVoltageOutput
#ifdef useDragRaceFunction
	,calcFormatTimeInTenthsSecIdx				// tAccelTestTime - acceleration test time (s.s)
#endif // useDragRaceFunction
	,calcFormatFuelQuantityIdx					// tFuelUsed - fuel quantity used (SI/SAE)
	,calcFormatFuelRateIdx						// tFuelRate - fuel consumption rate (SI/SAE)
	,calcFormatDistanceIdx						// tDistance - vehicle distance traveled (SI/SAE)
	,calcFormatSpeedIdx							// tSpeed - vehicle speed (SI/SAE)
	,calcFormatFuelQuantityIdx					// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	,calcFormatFuelQuantityIdx					// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	,calcFormatFuelQuantityIdx					// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
	,calcFormatDistanceIdx						// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	,calcFormatDistanceIdx						// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	,calcFormatDistanceIdx						// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	,calcFormatDistanceIdx						// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	,calcFormatDistanceIdx						// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	,calcFormatDistanceIdx						// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
#ifdef useFuelCost
	,calcFormatFuelCostPerDistanceIdx			// tFuelCostPerDistance - fuel cost per unit distance (SI/SAE)
	,calcFormatDistancePerFuelCostIdx			// tDistancePerFuelCost - distance per unit fuel cost (SI/SAE)
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,calcFormatPressureIdx						// tPressureChannel - absolute pressure (SI/SAE)
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,calcFormatEstimatedPowerIdx				// tEstimatedEnginePower - estimated engine power (SI/SAE)
	,calcFormatSpeedIdx							// tDragSpeed - acceleration test maximum vehicle speed at 1/4 mile (SI/SAE)
#endif // useDragRaceFunction
	,calcFormatFuelEconomyIdx					// tFuelEcon - fuel economy (SI/SAE)
};

const uint8_t calcFormatDecimalPlaces[(unsigned int)(calcFormatIdxCount)] PROGMEM = { // S64programList
	 0	// time in HHmmSS format
	,3	// time in milliseconds
	,0	// engine speed
	,0	// pulse count
#ifdef useFuelCost
	,2	// fuel cost
	,2	// fuel rate cost
#endif // useFuelCost
#if defined(useAnalogRead)
	,3	// voltage
#endif // useAnalogRead
#ifdef useDragRaceFunction
	,1	// time in tenths of seconds
#endif // useDragRaceFunction
	,2	// SAE fuel quantity
	,2	// SI fuel quantity
	,2	// SAE fuel rate
	,2	// SI fuel rate
	,3	// SAE distance travelled
	,3	// SI distance travelled
	,1	// SAE speed
	,1	// SI speed
#ifdef useFuelCost
	,2	// SAE fuel cost per unit distance
	,2	// SI fuel cost per unit distance
	,1	// SAE distance per unit fuel cost
	,1	// SI distance per unit fuel cost
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,2	// SAE pressure
	,2	// SI pressure
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,1	// SAE horsepower
	,1	// SI horsepower
#endif // useDragRaceFunction
	,2	// SAE fuel economy
	,2	// SI fuel economy
	,2	// alternate SAE fuel economy
	,2	// alternate SI fuel economy
};

const uint8_t calcFormatLabelText[(unsigned int)(calcFormatIdxCount)] PROGMEM = { // S64programList
	 's'	// time in HHmmSS format
	,'m'	// time in milliseconds
	,'t'	// engine speed
	,'p'	// pulse count
#ifdef useFuelCost
	,'$'	// fuel cost
	,'#'	// fuel rate cost
#endif // useFuelCost
#if defined(useAnalogRead)
	,'V'	// voltage
#endif // useAnalogRead
#ifdef useDragRaceFunction
	,'s'	// time in tenths of seconds
#endif // useDragRaceFunction
	,'G'	// SAE fuel used
	,'L'	// SI fuel used
	,'g'	// SAE fuel rate
	,'l'	// SI rate
	,'m'	// SAE distance travelled
	,'k'	// SI distance travelled
	,'S'	// SAE speed
	,'S'	// SI speed
#ifdef useFuelCost
	,'C'	// SAE fuel cost per distance
	,'C'	// SI fuel cost per distance
	,'D'	// SAE distance per fuel cost
	,'D'	// SI distance per fuel cost
#endif // useFuelCost
#ifdef useChryslerMAPCorrection
	,'P'	// SAE pressure
	,'P'	// SI pressure
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction
	,'H'	// SAE horsepower
	,'W'	// SI horsepower
#endif // useDragRaceFunction
	,'E'	// SAE fuel economy
	,'E'	// SI fuel economy
	,'E'	// alternate SAE fuel economy
	,'E'	// alternate SI fuel economy
};

static const uint8_t isValidTripIdx =	0b10100000;
static const uint8_t isValidCalcIdx =	0b01100000;
static const uint8_t isValidFlag =		0b00100000;

static const uint8_t isValidCalcObj =	0b11000000;

#ifdef useSpiffyTripLabels
const uint8_t calcFormatLabelCGRAM[(unsigned int)(calcFormatIdxCount)][16] PROGMEM = { // S64programList
	// time in HHmmSS format
	 {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}

	// time in milliseconds
	,{0b00010010, 0b00010010, 0b00010010, 0b00011110, 0b00010000, 0b00110000, 0b01000000, 0b01100000
	, 0b00001110, 0b00010000, 0b00001100, 0b00000010, 0b00011100, 0b00000000, 0b00000000, 0b00000000}

	// engine speed
	,{0b00001100, 0b00010000, 0b00010001, 0b00010010, 0b00000100, 0b00001000, 0b00000000, 0b00000000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011011, 0b00010101, 0b00010101}

	// pulse count
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001000, 0b00011100, 0b00001000, 0b00001000}
#ifdef useFuelCost

	// fuel cost
	,{0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100
	, 0b00000010, 0b00000101, 0b00000100, 0b00001110, 0b00000100, 0b00000100, 0b00000100, 0b00000100}

	// fuel rate cost
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}
#endif // useFuelCost
#if defined(useAnalogRead)

	// voltage
	,{0b00010001, 0b00010001, 0b00010001, 0b00001010, 0b00000100, 0b00000000, 0b00000000, 0b00000000
	, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011010, 0b00011010, 0b00010001}
#endif // useAnalogRead
#ifdef useDragRaceFunction

	// time in tenths of seconds
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101}
#endif // useDragRaceFunction

	// gallons
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000101, 0b00000011
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000001, 0b00000001, 0b00011001, 0b00011101}

	// liters
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000100, 0b00000100, 0b00000111
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}

	// gallons per hour
	,{0b00001100, 0b00010000, 0b00010100, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// liters per hour
	,{0b00010000, 0b00010000, 0b00010000, 0b00011100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}

	// miles
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00000000, 0b00010000, 0b00010000}

	// kilometers
	,{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000101, 0b00000110, 0b00000101
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00001010, 0b00010101, 0b00010101}

	// miles per hour
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00011100, 0b00010100, 0b00010100}

	// kilometers per hour
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100}
#ifdef useFuelCost

	// fuel cost per mile
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101}

	// fuel cost per kilometer
	,{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010100, 0b00011000, 0b00010100}

	// mile per unit fuel cost
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100}

	// kilometer per unit fuel cost
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100}
#endif // useFuelCost
#ifdef useChryslerMAPCorrection

	// psi
	,{0b00001110, 0b00001001, 0b00001001, 0b00001110, 0b00001000, 0b00001000, 0b00001000, 0b00000000
	, 0b00001001, 0b00010101, 0b00010001, 0b00001001, 0b00000101, 0b00010101, 0b00001001, 0b00000000}

	// kPa
	,{0b00001000, 0b00001000, 0b00001001, 0b00001010, 0b00001100, 0b00001010, 0b00001001, 0b00000000
	, 0b00011000, 0b00010100, 0b00010100, 0b00011000, 0b00010010, 0b00010101, 0b00010011, 0b00000000}
#endif // useChryslerMAPCorrection
#ifdef useDragRaceFunction

	// HP
	,{0b00001001, 0b00001001, 0b00001001, 0b00001111, 0b00001001, 0b00001001, 0b00001001, 0b00000000
	, 0b00100000, 0b01000000, 0b01100000, 0b00011000, 0b00010100, 0b00011000, 0b00010000, 0b00010000}

	// kW
	,{0b00100000, 0b01000000, 0b01110000, 0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000000
	, 0b00010001, 0b00010001, 0b00010101, 0b00010101, 0b00010101, 0b00001010, 0b00001010, 0b00000000}
#endif // useDragRaceFunction

	// miles per gallon
	,{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010100, 0b00001100}

	// liters per 100 km
	,{0b00010000, 0b00010000, 0b00011001, 0b00000010, 0b00000100, 0b00001001, 0b00000001, 0b00000001
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111}

	// gallons per 100 miles
	,{0b00001100, 0b00010000, 0b00010100, 0b00001101, 0b00000010, 0b00000101, 0b00001001, 0b00000001
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111}

	// km per liter
	,{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	, 0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00011100}
};

#endif // useSpiffyTripLabels
const uint8_t dfBitShift = 5;
const uint8_t dfFunctionMask =		0b00011111;
const uint8_t dfTripMask =			0b11100000;

// local trip indexes for main screen trip function display variables
#define nextAllowedValue 0
const uint8_t msInstantIdx =			nextAllowedValue;
const uint8_t msCurrentIdx =			msInstantIdx + 1;
const uint8_t msTankIdx =				msCurrentIdx + 1;
#define nextAllowedValue msTankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t msEOCidleCurrentIdx =		nextAllowedValue;
const uint8_t msEOCidleTankIdx =		msEOCidleCurrentIdx + 1;
const uint8_t msEOCidleInstantIdx =		msEOCidleTankIdx + 1;
#define nextAllowedValue msEOCidleInstantIdx + 1
#endif // trackIdleEOCdata

#ifdef useScreenEditor
const uint8_t dfMaxTripCount = nextAllowedValue;

#endif // useScreenEditor
const uint8_t lblInstantIdx =			(msInstantIdx << dfBitShift)	| instantIdx;
const uint8_t lblCurrentIdx =			(msCurrentIdx << dfBitShift)	| currentIdx;
const uint8_t lblTankIdx =				(msTankIdx << dfBitShift)		| tankIdx;
#ifdef trackIdleEOCdata
const uint8_t lblEOCidleCurrentIdx =	(msEOCidleCurrentIdx << dfBitShift)	| eocIdleCurrentIdx;
const uint8_t lblEOCidleTankIdx =		(msEOCidleTankIdx << dfBitShift)	| eocIdleTankIdx;
const uint8_t lblEOCidleInstantIdx =	(msEOCidleInstantIdx << dfBitShift)	| eocIdleInstantIdx;
#endif // trackIdleEOCdata

// trip index translation list
const uint8_t msTripList[] PROGMEM = {
	instantIdx
	,currentIdx
	,tankIdx
#ifdef trackIdleEOCdata
	,eocIdleCurrentIdx
	,eocIdleTankIdx
	,eocIdleInstantIdx
#endif // trackIdleEOCdata
};

#ifdef useSpiffyTripLabels
// display variable trip labels
const uint8_t msTripBitPattern[][4] PROGMEM = {
	 {0b00000000, 0b00000111, 0b00000010, 0b00000111} // I
 	,{0b00000000, 0b00000011, 0b00000100, 0b00000011} // C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000010} // T
#ifdef trackIdleEOCdata
	,{0b00000000, 0b00000011, 0b00000100, 0b00000110} // italic C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000100} // italic T
	,{0b00000000, 0b00000011, 0b00000010, 0b00000110} // italic I
#endif // trackIdleEOCdata
};

#endif // useSpiffyTripLabels
const char msTripNameString[] PROGMEM = {
	"INST\0"
	"CURR\0"
	"TANK\0"
#ifdef trackIdleEOCdata
	"cC/I\0"
	"tC/I\0"
	"iC/I\0"
#endif // trackIdleEOCdata
};
