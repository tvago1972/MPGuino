static void translateCalcIdx(uint16_t tripCalc, uint8_t windowLength, uint8_t decimalFlag);
static void translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalFlag);

typedef struct
{

	uint8_t isValid;
	uint8_t suppressTripLabel;
	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t calcFmtIdx;
	uint8_t tripChar;
	uint8_t calcChar;
	uint8_t decimalPlaces;
	uint32_t value;
#if defined(useSpiffyTripLabels)
	uint8_t labelIdx;
#endif // defined(useSpiffyTripLabels)
#if defined(useDebugTerminal) || defined(useJSONoutput)
	const char * calcFormatLabelPtr;
#endif // defined(useDebugTerminal) || defined(useJSONoutput)

} calcFuncObj;

calcFuncObj mainCalcFuncVar;

static char nBuff[17]; // used by bigDigit::, barGraphSupport::, systemInfo::, data logging, function result output routines

// calculation indexes into SWEET64 S64programList[] for display functions to either screen or logging output
#define nextAllowedValue 0
static const uint8_t tEngineRunTime =				nextAllowedValue;				// engine runtime (hhmmss)
static const uint8_t tRangeTime =					tEngineRunTime + 1;				// estimated total runtime from full tank (hhmmss)
static const uint8_t tReserveTime =					tRangeTime + 1;					// estimated reserve runtime from full tank (hhmmss)
static const uint8_t tBingoTime =					tReserveTime + 1;				// estimated bingo fuel runtime from full tank (hhmmss)
static const uint8_t tTimeToEmpty =					tBingoTime + 1;					// estimated remaining engine runtime (hhmmss)
static const uint8_t tReserveTimeToEmpty =			tTimeToEmpty + 1;				// estimated remaining reserve engine runtime (hhmmss)
static const uint8_t tBingoTimeToEmpty =			tReserveTimeToEmpty + 1;		// estimated bingo fuel quantity engine runtime (hhmmss)
static const uint8_t tMotionTime =					tBingoTimeToEmpty + 1;			// time vehicle in motion (hhmmss)
static const uint8_t tInjectorOpenTime =			tMotionTime + 1;				// fuel used (milliseconds)
static const uint8_t tInjectorTotalTime =			tInjectorOpenTime + 1;			// engine run time (milliseconds)
static const uint8_t tVSStotalTime =				tInjectorTotalTime + 1;			// time vehicle in motion (milliseconds)
static const uint8_t tEngineSpeed =					tVSStotalTime + 1;				// engine speed (1/m)
static const uint8_t tInjectorPulseCount =			tEngineSpeed + 1;				// fuel injector pulse count
static const uint8_t tVSSpulseCount =				tInjectorPulseCount + 1;		// VSS pulse count
static const uint8_t tDistance =					tVSSpulseCount + 1;				// vehicle distance traveled (SI/SAE)
static const uint8_t tSpeed =						tDistance + 1;					// vehicle speed (SI/SAE)
static const uint8_t tFuelUsed =					tSpeed + 1;						// fuel quantity used (SI/SAE)
static const uint8_t tFuelRate =					tFuelUsed + 1;					// fuel consumption rate (SI/SAE)
static const uint8_t tFuelEcon =					tFuelRate + 1;					// fuel economy (SI/SAE)
static const uint8_t tRangeDistance =				tFuelEcon + 1;					// estimated total distance on a full tank (SI/SAE)
static const uint8_t tReserveDistance =				tRangeDistance + 1;				// estimated reserve fuel tank distance (SI/SAE)
static const uint8_t tBingoDistance =				tReserveDistance + 1;			// estimated bingo fuel tank distance (SI/SAE)
static const uint8_t tDistanceToEmpty =				tBingoDistance + 1;				// estimated remaining distance (SI/SAE)
static const uint8_t tReserveDistanceToEmpty =		tDistanceToEmpty + 1;			// estimated reserve remaining distance (SI/SAE)
static const uint8_t tBingoDistanceToEmpty =		tReserveDistanceToEmpty + 1;	// estimated bingo remaining distance (SI/SAE)
#define nextAllowedValue tBingoDistanceToEmpty + 1
#if defined(useFuelCost)
static const uint8_t tFuelCostUsed =				nextAllowedValue;				// cost of fuel quantity used
static const uint8_t tFuelRateCost =				tFuelCostUsed + 1;				// fuel rate cost in currency units
static const uint8_t tFuelCostPerDistance =			tFuelRateCost + 1;				// fuel cost per unit distance (SI/SAE)
static const uint8_t tDistancePerFuelCost =			tFuelCostPerDistance + 1;		// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue tDistancePerFuelCost + 1
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
static const uint8_t tAccelTestTime =				nextAllowedValue;				// acceleration test time (sec)
#define nextAllowedValue tAccelTestTime + 1
#endif // defined(useDragRaceFunction)

static const uint8_t dfMaxValTripFunction =			nextAllowedValue;				// maximum index for function indexes that require a trip variable to function

static const uint8_t tFuelQuantity =				nextAllowedValue;				// tank total fuel quantity (SI/SAE)
static const uint8_t tReserveQuantity =				tFuelQuantity + 1;				// tank reserve fuel quantity (SI/SAE)
static const uint8_t tBingoQuantity =				tReserveQuantity + 1;			// tank bingo fuel quantity (SI/SAE)
static const uint8_t tRemainingFuel =				tBingoQuantity + 1;				// estimated remaining fuel quantity (SI/SAE)
static const uint8_t tReserveRemainingFuel =		tRemainingFuel + 1;				// estimated remaining reserve fuel quantity (SI/SAE)
static const uint8_t tBingoRemainingFuel =			tReserveRemainingFuel + 1;		// estimated bingo fuel quantity remaining (SI/SAE)
#define nextAllowedValue tBingoRemainingFuel + 1
#if defined(useDebugAnalog)
static const uint8_t tAnalogChannel =				nextAllowedValue;				// DC voltage
#define nextAllowedValue tAnalogChannel + 1
#endif // defined(useDebugAnalog)
#if defined(useCarVoltageOutput)
static const uint8_t tAlternatorChannel =			nextAllowedValue;				// DC voltage
#define nextAllowedValue tAlternatorChannel + 1
#endif // defined(useCarVoltageOutput)
#if defined(useChryslerMAPCorrection)
static const uint8_t tPressureChannel =				nextAllowedValue;				// absolute pressure (SI/SAE)
#define nextAllowedValue tPressureChannel + 1
#endif // defined(useChryslerMAPCorrection)
#if defined(useFuelCost)
static const uint8_t tFuelCostTank =				nextAllowedValue;				// full tank fuel cost in currency units
static const uint8_t tFuelCostReserve =				tFuelCostTank + 1;				// reserve fuel quantity fuel cost in currency units
static const uint8_t tFuelCostBingo =				tFuelCostReserve + 1;			// bingo fuel quantity cost in currency units
static const uint8_t tFuelCostRemaining =			tFuelCostBingo + 1;				// value of estimated remaining total fuel quantity in currency units
static const uint8_t tFuelCostReserveRemaining =	tFuelCostRemaining + 1;			// value of estimated remaining reserve fuel quantity in currency units
static const uint8_t tFuelCostBingoRemaining =		tFuelCostReserveRemaining + 1;	// value of estimated remaining bingo fuel quantity in currency units
#define nextAllowedValue tFuelCostBingoRemaining + 1
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
static const uint8_t tEstimatedEnginePower =		nextAllowedValue;				// estimated engine power (SI/SAE)
static const uint8_t tDragSpeed =					tEstimatedEnginePower + 1;		// acceleration test maximum vehicle speed (SI/SAE)
static const uint8_t tTrapSpeed =					tDragSpeed + 1;					// acceleration test vehicle speed at defined distance (SI/SAE)
#define nextAllowedValue tTrapSpeed + 1
#endif // defined(useDragRaceFunction)

static const uint8_t dfMaxValDisplayCount =			nextAllowedValue;				// maximum index for function indexes that return results for display

#if defined(useBluetooth)
static const uint8_t tGetBTparameterValue =			nextAllowedValue;
static const uint8_t tGetProgramVariableValue =		tGetBTparameterValue + 1;
#define nextAllowedValue tGetProgramVariableValue + 1
#endif // defined(useBluetooth)

static const uint8_t dfMaxValCalcCount =			nextAllowedValue;				// maximum index for function indexes that return results

static const uint8_t tCalculateRemainingTank =		nextAllowedValue;				// calculate estimated remaining fuel quantity in injector open cycles
static const uint8_t tCalculateRemainingReserve =	tCalculateRemainingTank + 1;	// calculate estimated remaining fuel reserve value in injector open cycles
static const uint8_t tCalculateBingoFuel =			tCalculateRemainingReserve + 1;	// calculate estimated fuel bingo value in injector open cycles
static const uint8_t tConvertToMilliSeconds =		tCalculateBingoFuel + 1;		// convert value in timer0 cycles to decimal formatted milliseconds
static const uint8_t tCalculateFuelQuantity =		tConvertToMilliSeconds + 1;		// convert fuel quantity in timer0 cycles to unit liquid quantity
static const uint8_t tCalculateFuelDistance =		tCalculateFuelQuantity + 1;
static const uint8_t tCalculateFuelTime =			tCalculateFuelDistance + 1;
static const uint8_t tCalculateSpeed =				tCalculateFuelTime + 1;
static const uint8_t tFormatToHHMMSStime =			tCalculateSpeed + 1;
static const uint8_t tFormatToH9MMSStime =			tFormatToHHMMSStime + 1;
static const uint8_t tFormatToNumber =				tFormatToH9MMSStime + 1;
static const uint8_t tRoundOffNumber =				tFormatToNumber + 1;
static const uint8_t tLoadTrip =					tRoundOffNumber + 1;
static const uint8_t tSaveTrip =					tLoadTrip + 1;
static const uint8_t tReadTicksToSeconds =			tSaveTrip + 1;
#define nextAllowedValue tReadTicksToSeconds + 1
#if defined(useBarFuelEconVsTime)
static const uint8_t tFEvTgetDistance =				nextAllowedValue;
static const uint8_t tFEvTgetConsumedFuel =			tFEvTgetDistance + 1;
static const uint8_t tFEvTgetFuelEconomy =			tFEvTgetConsumedFuel + 1;
#define nextAllowedValue tFEvTgetFuelEconomy + 1
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t tFEvSgetDistance =				nextAllowedValue;
static const uint8_t tFEvSgetConsumedFuel =			tFEvSgetDistance + 1;
static const uint8_t tFEvSgetFuelEconomy =			tFEvSgetConsumedFuel + 1;
#define nextAllowedValue tFEvSgetFuelEconomy + 1
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useDebugTerminal)
static const uint8_t tParseCharacterToReg =			nextAllowedValue;
#define nextAllowedValue tParseCharacterToReg + 1
#endif //defined(useDebugTerminal)

static const uint8_t dfMaxValTotalCount =			nextAllowedValue;				// maximum index for all indexed programs

#if defined(useDebugTerminalLabels)
static const char terminalTripFuncNames[] PROGMEM = {
	"tEngineRunTime" tcEOS				// engine runtime (hhmmss)
	"tRangeTime" tcEOS					// estimated total runtime from full tank (hhmmss)
	"tReserveTime" tcEOS				// estimated reserve runtime from full tank (hhmmss)
	"tBingoTime" tcEOS					// estimated bingo fuel runtime from full tank (hhmmss)
	"tTimeToEmpty" tcEOS				// estimated remaining engine runtime (hhmmss)
	"tReserveTimeToEmpty" tcEOS			// estimated remaining reserve engine runtime (hhmmss)
	"tBingoTimeToEmpty" tcEOS			// estimated bingo fuel quantity engine runtime (hhmmss)
	"tMotionTime" tcEOS					// time vehicle in motion (hhmmss)
	"tInjectorOpenTime" tcEOS			// fuel used (milliseconds)
	"tInjectorTotalTime" tcEOS			// engine run time (milliseconds)
	"tVSStotalTime" tcEOS				// time vehicle in motion (milliseconds)
	"tEngineSpeed" tcEOS				// engine speed (1/m)
	"tInjectorPulseCount" tcEOS			// fuel injector pulse count
	"tVSSpulseCount" tcEOS				// VSS pulse count
	"tDistance" tcEOS					// vehicle distance traveled (SI/SAE)
	"tSpeed" tcEOS						// vehicle speed (SI/SAE)
	"tFuelUsed" tcEOS					// fuel quantity used (SI/SAE)
	"tFuelRate" tcEOS					// fuel consumption rate (SI/SAE)
	"tFuelEcon" tcEOS					// fuel economy (SI/SAE)
	"tRangeDistance" tcEOS				// estimated total distance on a full tank (SI/SAE)
	"tReserveDistance" tcEOS			// estimated reserve fuel tank distance (SI/SAE)
	"tBingoDistance" tcEOS				// estimated bingo fuel tank distance (SI/SAE)
	"tDistanceToEmpty" tcEOS			// estimated remaining distance (SI/SAE)
	"tReserveDistanceToEmpty" tcEOS		// estimated reserve remaining distance (SI/SAE)
	"tBingoDistanceToEmpty" tcEOS		// estimated bingo remaining distance (SI/SAE)
#if defined(useFuelCost)
	"tFuelCostUsed" tcEOS				// cost of fuel quantity used
	"tFuelRateCost" tcEOS				// fuel rate cost in currency units
	"tFuelCostPerDistance" tcEOS		// fuel cost per unit distance (SI/SAE)
	"tDistancePerFuelCost" tcEOS		// distance per unit fuel cost (SI/SAE)
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
	"tAccelTestTime" tcEOS				// acceleration test time (sec)
#endif // defined(useDragRaceFunction)
	"tFuelQuantity" tcEOS				// tank total fuel quantity (SI/SAE)
	"tReserveQuantity" tcEOS			// tank reserve fuel quantity (SI/SAE)
	"tBingoQuantity" tcEOS				// tank bingo fuel quantity (SI/SAE)
	"tRemainingFuel" tcEOS				// estimated remaining fuel quantity (SI/SAE)
	"tReserveRemainingFuel" tcEOS		// estimated remaining reserve fuel quantity (SI/SAE)
	"tBingoRemainingFuel" tcEOS			// estimated bingo fuel quantity remaining (SI/SAE)
#if defined(useDebugAnalog)
	"tAnalogChannel" tcEOS				// DC voltage
#endif // defined(useDebugAnalog)
#if defined(useCarVoltageOutput)
	"tAlternatorChannel" tcEOS			// DC voltage
#endif // defined(useCarVoltageOutput)
#if defined(useChryslerMAPCorrection)
	"tPressureChannel" tcEOS			// absolute pressure (SI/SAE)
#endif // defined(useChryslerMAPCorrection)
#if defined(useFuelCost)
	"tFuelCostTank" tcEOS				// full tank fuel cost in currency units
	"tFuelCostReserve" tcEOS			// reserve fuel quantity fuel cost in currency units
	"tFuelCostBingo" tcEOS				// bingo fuel quantity cost in currency units
	"tFuelCostRemaining" tcEOS			// value of estimated remaining total fuel quantity in currency units
	"tFuelCostReserveRemaining" tcEOS	// value of estimated remaining reserve fuel quantity in currency units
	"tFuelCostBingoRemaining" tcEOS		// value of estimated remaining bingo fuel quantity in currency units
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
	"tEstimatedEnginePower" tcEOS		// estimated engine power (SI/SAE)
	"tDragSpeed" tcEOS					// acceleration test maximum vehicle speed (SI/SAE)
	"tTrapSpeed" tcEOS					// acceleration test vehicle speed at defined distance (SI/SAE)
#endif // defined(useDragRaceFunction)
#if defined(useBluetooth)
	"tGetBTparameterValue" tcEOS
	"tGetProgramVariableValue" tcEOS
#endif // defined(useBluetooth)
	"tCalculateRemainingTank" tcEOS		// calculate estimated remaining fuel quantity in injector open cycles
	"tCalculateRemainingReserve" tcEOS	// calculate estimated remaining fuel reserve value in injector open cycles
	"tCalculateBingoFuel" tcEOS			// calculate estimated fuel bingo value in injector open cycles
	"tConvertToMilliSeconds" tcEOS		// convert value in timer0 cycles to decimal formatted milliseconds
	"tCalculateFuelQuantity" tcEOS		// convert fuel quantity in timer0 cycles to unit liquid quantity
	"tCalculateFuelDistance" tcEOS
	"tCalculateFuelTime" tcEOS
	"tCalculateSpeed" tcEOS
	"tFormatToHHMMSStime" tcEOS
	"tFormatToH9MMSStime" tcEOS
	"tFormatToNumber" tcEOS
	"tRoundOffNumber" tcEOS
	"tLoadTrip" tcEOS
	"tSaveTrip" tcEOS
	"tReadTicksToSeconds" tcEOS
#if defined(useBarFuelEconVsTime)
	"tFEvTgetDistance" tcEOS
	"tFEvTgetConsumedFuel" tcEOS
	"tFEvTgetFuelEconomy" tcEOS
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	"tFEvSgetDistance" tcEOS
	"tFEvSgetConsumedFuel" tcEOS
	"tFEvSgetFuelEconomy" tcEOS
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useDebugTerminal)
	"tParseCharacterToReg" tcEOS
#endif // defined(useDebugTerminal)
};

#endif // defined(useDebugTerminalLabels)
static const uint8_t prgmEngineRunTime[] PROGMEM = {	// tEngineRunTime - engine runtime (hhmmss)
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,		// load injector pulse cycle value into register 2
	instrDiv2byRdOnly, idxCycles0PerSecond,				// divide by cycles per second value
	instrJump, tFormatToH9MMSStime						// go format the number to hhmmss time
};

static const uint8_t prgmRangeTime[] PROGMEM = {		// tRangeTime - estimated total runtime from full tank (hhmmss)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmReserveTime[] PROGMEM = {		// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubVariableFromX, 0x02, m64BingoTankSizeIdx,	// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmBingoTime[] PROGMEM = {		// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	instrLdRegVariable, 0x02, m64BingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmTimeToEmpty[] PROGMEM = {		// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmReserveTimeToEmpty[] PROGMEM = {	// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmBingoTimeToEmpty[] PROGMEM = {	// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelTime						// go format it
};

static const uint8_t prgmCalculateFuelTime[] PROGMEM = {	// tCalculateFuelTime
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 10,								// if inputted fuel quantity is zero, exit to caller
	instrMul2byRdOnly, idxMicroSecondsPerSecond,		// shift number upward to retain numeric precision
	instrDiv2byTripVarIndexed, rvInjCycleIdx,			// divide by open injector time in cycles
	instrMul2byTripVarIndexed, rvEngCycleIdx,			// multiply by injector open count
	instrDiv2byRdOnly, idxCycles0PerSecond,				// convert output term from cycles remaining to seconds remaining
	instrDiv2byRdOnly, idxMicroSecondsPerSecond,		// shift number downward - high numeric precision no longer needed

//cont:
	instrJump, tFormatToH9MMSStime						// go format the number to hhmmss time
};

static const uint8_t prgmMotionTime[] PROGMEM = {		// tMotionTime - time vehicle in motion (hhmmss)
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrDiv2byRdOnly, idxCycles0PerSecond,				// divide by cycles per second value
	instrJump, tFormatToH9MMSStime						// go format the number to hhmmss time
};

static const uint8_t prgmFormatToH9MMSStime[] PROGMEM = {	// tFormatToH9MMSStime
	instrLdReg, 0x21,									// move time in seconds into register 1
	instrDoBCDadjust, 0x12, bcdFormatH9MMSS,			// process register 1 as hhmmss BCD string and store it in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmInjectorOpenTime[] PROGMEM = {	// tInjectorOpenTime - fuel used (milliseconds)
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load trip injector open time in timer0 cycles
	instrJump, tConvertToMilliSeconds					// go convert timer0 cycles to milliseconds
};

static const uint8_t prgmInjectorTotalTime[] PROGMEM = {	// tInjectorTotalTime - engine run time (milliseconds)
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,		// load engine run time in timer0 cycles
	instrJump, tConvertToMilliSeconds					// go convert timer0 cycles to milliseconds
};

static const uint8_t prgmVSStotalTime[] PROGMEM = {		// tVSStotalTime - time vehicle in motion (milliseconds)
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,		// load time vehicle spent in motion in timer0 cycles
	instrJump, tConvertToMilliSeconds					// go convert timer0 cycles to milliseconds
};

static const uint8_t prgmConvertToMicroSeconds[] PROGMEM = {	// tConvertToMilliSeconds - convert value in timer0 cycles to decimal formatted milliseconds
	instrMul2byRdOnly, idxMicroSecondsPerSecond,		// multiply by conversion factor for microseconds per second (also milliseconds per second times decimal formatting factor)
	instrDiv2byRdOnly, idxCycles0PerSecond,				// divide by conversion factor for cycles to seconds
	instrDone											// exit to caller
};

static const uint8_t prgmEngineSpeed[] PROGMEM = {		// tEngineSpeed - engine speed (1/m)
	instrLdRegTripVarIndexed, 0x02, rvEngCycleIdx,		// load injector pulse time into register 2
	instrMul2byEEPROM, pInjPer2CrankRevIdx,				// multiply by the number of injector fire events per 2 crank revolutions
	instrLdReg, 0x21,									// move denominator to register 1

	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,		// load injector pulse count into register 2
	instrMul2byRdOnly, idxCycles0PerSecond,				// set up for conversion of denominator injector cycle count to time in seconds
	instrMul2byByte, 120,								// set up for conversion of denominator injector time in seconds to time in minutes
	instrMul2byRdOnly, idxDecimalPoint,					// perform output decimal formatting
	instrDiv2by1,										// perform divide
	instrDone											// exit to caller
};

static const uint8_t prgmInjectorPulseCount[] PROGMEM = {	// tInjectorPulseCount - fuel injector pulse count
	instrLdRegTripVarIndexed, 0x02, rvInjPulseIdx,		// load trip fuel injector pulse count
	instrMul2byRdOnly, idxDecimalPoint,					// multiply by decimal point factor for output as an integer
	instrDone											// exit to caller
};

static const uint8_t prgmVSSpulseCount[] PROGMEM = {	// tInjectorPulseCount - fuel injector pulse count
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load trip vehicle speed sensor pulse count
	instrMul2byRdOnly, idxDecimalPoint,					// multiply by decimal point factor for output as an integer
	instrDone											// exit to caller
};

static const uint8_t prgmDistance[] PROGMEM = {			// tDistance - vehicle distance traveled (SI/SAE)
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS cycle value into register 2
	instrMul2byRdOnly, idxDecimalPoint,					// perform output decimal formatting
	instrDiv2byEEPROM, pPulsesPerDistanceIdx,			// divide by pulse per distance value and store in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmSpeed[] PROGMEM = {			// tSpeed - vehicle speed (SI/SAE)
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// load VSS pulse count
	instrLdRegTripVarIndexed, 0x01, rvVSScycleIdx,		// load VSS cycle value into register 2
	instrJump, tCalculateSpeed							// go calculate speed
};

static const uint8_t prgmFuelUsed[] PROGMEM = {			// tFuelUsed - fuel quantity used (SI/SAE)
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// get amount of consumed fuel in cycles
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity value
};

static const uint8_t prgmFuelRate[] PROGMEM = {			// tFuelRate - fuel consumption rate (SI/SAE)
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector open cycle value
	instrBranchIfZero, 11,								// if zero, then fuel rate is also zero
	instrMul2byRdOnly, idxSecondsPerHour,
	instrMul2byRdOnly, idxDecimalPoint,
	instrDiv2byTripVarIndexed, rvEngCycleIdx,
	instrMul2byRdOnly, idxCycles0PerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrDiv2byVariable, m32CyclesPerVolumeIdx,			// multiply by quantity/cycle (divide by cycles/quantity)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone
};

static const uint8_t prgmFuelEcon[] PROGMEM = {			// tFuelEcon - fuel economy (SI/SAE)
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch the accumulated number of VSS pulses counted
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator for later
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycle measurement
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino is in metric mode, skip ahead
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byRdOnly, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or km/L)
	instrSkip, 2,										// go skip ahead

//fecont1:
	instrMul2byRdOnly, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for L/100km or gal/100mi)

//fecont2:
	instrDiv2by1,										// divide the numerator by the denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmRangeDistance[] PROGMEM = {	// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmReserveDistance[] PROGMEM = {	// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubVariableFromX, 0x02, m64BingoTankSizeIdx,	// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmBingoDistance[] PROGMEM = {	// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	instrLdRegVariable, 0x02, m64BingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmDistanceToEmpty[] PROGMEM = {	// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmReserveDistanceToEmpty[] PROGMEM = {	// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	instrCall, tCalculateRemainingReserve,				// fetch reserve fuel quantity in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmBingoDistanceToEmpty[] PROGMEM = {	// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
	instrCall, tCalculateBingoFuel,						// fetch bingo fuel value in injector open cycles
	instrJump, tCalculateFuelDistance					// go format it
};

static const uint8_t prgmCalculateFuelDistance[] PROGMEM = {	// tCalculateFuelDistance
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 18,								// if inputted fuel quantity is zero, exit to caller
	instrMul2byTripVarIndexed, rvVSSpulseIdx,			// multiply input fuel quantity by accumulated distance travelled pulses to get the numerator for distance to empty
	instrLdReg, 0x23,									// move numerator to register 3 for now
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch the accumulated fuel injector open cycles
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply accumulated fuel injector open cycles by pulses per unit distance factor
	instrDiv2byRdOnly, idxDecimalPoint,					// divide by decimal point constant to get denominator / decimal factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrLdReg, 0x21,									// move modified denominator to register 1 for now
	instrLdReg, 0x32,									// fetch numerator from register 3
	instrDiv2by1,										// divide to get DTE = (numerator) / (denominator / decimal factor ) = (numerator * decimal factor) / denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmFuelQuantity[] PROGMEM = {		// tFuelQuantity - tank total fuel quantity (SI/SAE)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity
};

static const uint8_t prgmReserveQuantity[] PROGMEM = {	// tReserveQuantity - tank reserve fuel quantity (SI/SAE)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubVariableFromX, 0x02, m64BingoTankSizeIdx,	// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity
};

static const uint8_t prgmBingoQuantity[] PROGMEM = {	// tBingoQuantity - tank bingo fuel quantity (SI/SAE)
	instrLdRegVariable, 0x02, m64BingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the fuel quantity
};

static const uint8_t prgmRemainingFuel[] PROGMEM = {	// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	instrCall, tCalculateRemainingTank,					// go get remaining fuel quantity in cycles
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the remaining fuel quantity value
};

static const uint8_t prgmReserveRemainingFuel[] PROGMEM = {	// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	instrCall, tCalculateRemainingReserve,				// go calculate how much reserve fuel remains
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the reserve fuel value
};

static const uint8_t prgmBingoRemainingFuel[] PROGMEM = {	// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
	instrCall, tCalculateBingoFuel,						// go calculate bingo fuel quantity
	instrLdRegRdOnly, 0x01, idxDecimalPoint,			// load the decimal point constant used for output formatting
	instrJump, tCalculateFuelQuantity					// go format the bingo fuel value
};

static const uint8_t prgmCalculateFuelQuantity[] PROGMEM = {	// tCalculateFuelQuantity - convert fuel quantity in timer0 cycles to unit liquid quantity
	instrTestReg, 0x02,									// test inputted fuel quantity
	instrBranchIfZero, 4,								// if inputted fuel quantity is zero, exit to caller
	instrMul2by1,										// multiply the fuel used value by the formatting term
	instrDiv2byVariable, m32CyclesPerVolumeIdx,			// convert fuel value in cycles to fuel value per unit quantity
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateSpeed[] PROGMEM = {	// tCalculateSpeed -
	instrBranchIfZero, 4,								// if speed measurement is zero, exit to caller
	instrMul2byVariable, m32SpeedFactorIdx,				// multiply pulse count by speed factor in (cycle0 * distance) / (pulse * hour)
	instrDiv2by1,										// divide by time vehicle spent moving in cycle0s
	instrDone,											// exit to caller

//cont0:
	instrLdRegByte, 0x02, 0,							// zero out result register
	instrDone											// exit to caller
};

#if defined(useFuelCost)
static const uint8_t prgmFuelCostUsed[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// get amount of consumed fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostTank[] PROGMEM = {
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostReserve[] PROGMEM = {
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubVariableFromX, 0x02, m64BingoTankSizeIdx,	// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrLdRegEEPROM, 0x01, pCostPerQuantity,			// fetch fuel monetary value per unit volume term
	instrJump, tCalculateFuelQuantity					// go format the fuel monetary value
};

static const uint8_t prgmFuelCostBingo[] PROGMEM = {
	instrLdRegVariable, 0x02, m64BingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
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
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume value to get the denominator for fuel cost per distance
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
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume to get the numerator for distance per fuel cost
	instrMul2byRdOnly, idxDecimalPoint,					// cancel out the decimal point constant for the cost per unit quantity of fuel
	instrMul2byRdOnly, idxDecimalPoint,					// load the decimal point constant used for output formatting
	instrDiv2by1,										// divide the numerator by the denominator
	instrDone
};

static const uint8_t prgmFuelRateCost[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// load injector open cycle value
	instrBranchIfZero, 11,								// if zero, then fuel rate is also zero
	instrMul2byRdOnly, idxSecondsPerHour,
	instrMul2byEEPROM, pCostPerQuantity,
	instrDiv2byTripVarIndexed, rvEngCycleIdx,
	instrMul2byRdOnly, idxCycles0PerSecond,				// for conversion of quantity/cycles to quantity/sec
	instrDiv2byVariable, m32CyclesPerVolumeIdx,			// multiply by quantity/cycle (divide by cycles/quantity)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont:
	instrDone
};

#endif // defined(useFuelCost)
static const uint8_t prgmCalculateRemainingReserve[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 8,								// if zero, then reserve fuel is also zero
	instrSubVariableFromX, 0x02, m64BingoTankSizeIdx,	// subtract bingo tank size in injector open cycles
	instrBranchIfLTorE, 3,								// if remaining fuel >= bingo tank size, exit to caller
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateBingoFuel[] PROGMEM = {
	instrCall, tCalculateRemainingTank,					// get calculated remaining fuel in injector open cycles
	instrTestReg, 0x02,									// test calculated remaining fuel left
	instrBranchIfZero, 9,								// if zero, then bingo fuel is also zero
	instrLdRegVariable, 0x01, m64BingoTankSizeIdx,		// fetch bingo tank size in injector open cycles
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if tank quantity in injector open cycles <= calculated bingo size in injector open cycles, exit to caller
	instrLdReg, 0x12,									// shift calculated bingo size in injector open cycles to register 2

//ret:
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateRemainingTank[] PROGMEM = {	// tCalculateRemainingTank - calculate estimated remaining fuel quantity in injector open cycles
#if defined(usePartialRefuel)
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,				// fetch partial refuel tank size in unit volume * decimal formatting factor
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byRdOnly, idxDecimalPoint,					// remove formatting factor to get partial refuel tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrLdRegTripVar, 0x01, tankIdx, rvInjCycleIdx,	// fetch tank quantity in injector open cycles
	instrSubYfromX, 0x21,								// subtract calculated partial refuel size in injector open cycles
	instrBranchIfGT, 7,									// if calculated partial refuel size > tank quantity, exit with calculated tank size in result
#else // defined(usePartialRefuel)
	instrLdRegTripVar, 0x01, tankIdx, rvInjCycleIdx,	// fetch tank quantity in injector open cycles
#endif // defined(usePartialRefuel)
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubYfromX, 0x12,								// subtract tank injector time in cycles from calculated tank size in cycles to get remaining fuel in cycles
	instrBranchIfLTorE,	3,								// if tank quantity <= calculated tank size, exit
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//ret:
	instrDone											// exit to caller
};

#if defined(useDebugAnalog)
static const uint8_t prgmAnalogChannel[] PROGMEM = {	// tAnalogChannel - DC voltage
	instrCmpIndex, v16AnalogStartIdx,					// is trip index pointing to a valid analog channel?
	instrBranchIfLT, 4,									// if not, zero out result and return
	instrCmpIndex, v16AnalogEndIdx,
	instrBranchIfLT, 4,									// if so, skip ahead
	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

//cont1:
	instrLdRegVariableIndexed, 0x02, 					// load analog channel ADC step value
	instrMul2byRdOnly, idxDenomVoltage,					// perform multiply (we're converting from ADC steps to voltage)
	instrDiv2byRdOnly, idxNumerVoltage,					// divide by numerator to convert from voltage to ADC steps
	instrDone											// exit to caller
};

#endif // defined(useDebugAnalog)
#if defined(useCarVoltageOutput)
static const uint8_t prgmAlternatorChannel[] PROGMEM = {
	instrLdRegVariable, 0x02, v16AnalogAlternatorChannelIdx,	// load analog channel ADC step value
	instrMul2byRdOnly, idxDenomVoltage,					// perform multiply (we're converting from ADC steps to voltage)
	instrDiv2byRdOnly, idxNumerVoltage,					// divide by numerator to get voltage drop V(R5) between R5 and ground
	instrLdRegRdOnly, 0x01, idxResistanceR5,			// figure out V(R5+R6) from V(R5)
	instrAddConstToX, 0x01, idxResistanceR6,
	instrMul2by1,
	instrDiv2byRdOnly, idxResistanceR5,
	instrAddEEPROMtoX, 0x02, pVoltageOffset,			// add diode offset
	instrDone											// exit to caller
};

#endif // defined(useCarVoltageOutput)
static const uint8_t prgmRoundOffNumber[] PROGMEM = {
	instrTestReg, 0x02,									// test register 2
	instrBranchIfOverflow, 23,							// if register 2 has overflow value, exit
	instrCmpIndex, 2,									// check if 3 or more right hand digits were specified
	instrBranchIfGT, 17,								// if so, just exit
	instrBranchIfE, 12,									// if 2 right hand digits were specified, round to nearest 100th
	instrCmpIndex, 1,									// check if 0 or 1 right-hand digits were specified
	instrBranchIfE, 4,									// if 1 right hand digit was specified, round to nearest 10th
	instrDiv2byRdOnly, idxDecimalPoint,					// shift number right 3 digits to round to nearest whole digit
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

static const uint8_t prgmFormatToNumber[] PROGMEM = {	// tFormatToNumber
	instrTestReg, 0x02,									// test register 2
	instrBranchIfOverflow, 13,							// if register 2 has overflow value, exit
	instrCmpIndex, 3,									// check if valid number of decimal points were requested
	instrBranchIfGT, 9,									// if too many decimal points were requested, error out with overflow
	instrDiv2byRdOnly, idxBCDdivisor,					// divide by 10^8 - quotient remains in register 2, and remainder goes into register 1
	instrLdRegByte, 0x03, 99,
	instrCmpXtoY, 0x23,									// test if quotient is greater than 99 (remainder will be less than 10^8)
	instrBranchIfLTorE, 4,								// if quotient is 99 or less, continue with processing

//overflow:
	instrDoBCDadjust, 0x12, bcdFormatOverflow,			// store overflow BCD string in register 2
	instrDone,

//cont:
	instrDoBCDadjust, 0x12, bcdFormat10digit,			// process register 1 as 10 digit BCD string and store it in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmFetchParameterValue[] PROGMEM = {
	instrLdRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

#if defined(useBluetooth)
static const uint8_t prgmGetBTparameterValue[] PROGMEM = {	// tGetBTparameterValue
	instrLdRegEEPROMindexed, 0x02,						// load EEPROM parameter for output
	instrCmpIndex, pMetricModeIdx,						// is this pMetricModeIdx being output
	instrBranchIfNotE, 3,								// if not, exit
	instrAddByteToX, 0x02, 1,							// adjust pMetricModeIdx so it is compatible with MPGuinoBlue
	instrDone											// return to caller
};

static const uint8_t prgmWriteProgramVariableValue[] PROGMEM = {
	instrStRegVariableIndexed, 0x02,
	instrDone											// exit to caller
};

#endif // defined(useBluetooth)
static const uint8_t prgmGetProgramVariableValue[] PROGMEM = { // tGetProgramVariableValue
	instrLdRegVariableIndexed, 0x02,
	instrDone											// exit to caller
};

static const uint8_t prgmFetchInitialParamValue[] PROGMEM = {
	instrLdRegRdOnlyIndexed, 0x02,
	instrDone											// return to caller
};

#if defined(useEEPROMtripStorage)
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
#if defined(trackIdleEOCdata)
	instrCall, tSaveTrip,
	instrLxdI, EEPROMeocIdleCurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, eocIdleCurrentIdx,
#endif // defined(trackIdleEOCdata)
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
#if defined(trackIdleEOCdata)
	instrCall, tSaveTrip,
	instrLxdI, eocIdleCurrentIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMeocIdleCurrentIdx,
#endif // defined(trackIdleEOCdata)
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
#if defined(usePartialRefuel)
	instrLdRegEEPROM, 0x02, pRefuelSaveSizeIdx,
	instrStRegEEPROM, 0x02, pRefuelSizeIdx,
#endif // defined(usePartialRefuel)
	instrLxdI, EEPROMtankIdx,
	instrCall, tLoadTrip,
	instrLxdI, tankIdx,
#if defined(trackIdleEOCdata)
	instrCall, tSaveTrip,
	instrLxdI, EEPROMeocIdleTankIdx,
	instrCall, tLoadTrip,
	instrLxdI, eocIdleTankIdx,
#endif // defined(trackIdleEOCdata)
	instrCall, tSaveTrip,
	instrLdRegByte, 0x02, 1,
	instrDone
};

static const uint8_t prgmSaveTankToEEPROM[] PROGMEM = {
	instrLdRegByte, 0x02, guinosig,
	instrStRegEEPROM, 0x02, pTankTripSignatureIdx,
#if defined(usePartialRefuel)
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,
	instrStRegEEPROM, 0x02, pRefuelSaveSizeIdx,
#endif // defined(usePartialRefuel)
	instrLxdI, tankIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMtankIdx,
#if defined(trackIdleEOCdata)
	instrCall, tSaveTrip,
	instrLxdI, eocIdleTankIdx,
	instrCall, tLoadTrip,
	instrLxdI, EEPROMeocIdleTankIdx,
#endif // defined(trackIdleEOCdata)
	instrJump, tSaveTrip,
};

#endif // defined(useEEPROMtripStorage)
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
	instrLdRegVariableIndexed, 0x02,
	instrDiv2byRdOnly, idxTicksPerSecond,
	instrJump, tFormatToHHMMSStime						// go format the number to hhmmss time
};

#if defined(useBarFuelEconVsTime)
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
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator (for G/100mi or L/100km) for later
	instrLdRegTripFEvTindexed, 0x02, rvInjCycleIdx,		// load indexed injector cycles from fuel econ vs time trip variable bank
	instrBranchIfE, 22,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino currently outputting fuel econ as fuel over distance, skip
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byRdOnly, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or k/L)
	instrSkip, 2,										// skip ahead

//cont0:
	instrMul2byRdOnly, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for G/100mi or L/100km)
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

#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
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
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume to get the denominator for fuel economy
	instrLdReg, 0x21,									// save denominator (for G/100mi or L/100km) for later
	instrLdRegTripVarOffset, 0x02, FEvsSpeedIdx, rvInjCycleIdx,	// load indexed injector cycles from fuel econ vs speed trip variable bank
	instrBranchIfE, 22,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by the pulses per unit distance factor to get the numerator for fuel economy
	instrBranchIfFuelOverDist, 6,						// if MPGuino currently outputting fuel econ as fuel over distance, skip
	instrSwapReg, 0x21,									// swap the numerator and denominator terms around
	instrMul2byRdOnly, idxDecimalPoint,					// put in the decimal point constant used for output formatting (for mi/gal or k/L)
	instrSkip, 2,										// skip ahead

//cont0:
	instrMul2byRdOnly, idxMetricFE,						// put in the output formatting decimal point constant, multiplied by 100 (for G/100mi or L/100km)
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

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useDebugTerminal)
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

#endif // defined(useDebugTerminal)
static const uint8_t prgmFormatToHHMMSStime[] PROGMEM = {
	instrLdReg, 0x21,									// move time in seconds into register 1
	instrDoBCDadjust, 0x12, bcdFormatHHMMSS,			// process register 1 as hhmmss BCD string and store it in register 2
	instrDone											// exit to caller
};

#if defined(useChryslerMAPCorrection)
static const uint8_t prgmPressureChannel[] PROGMEM = {
	instrCmpIndex, m32AnalogMAPfloorIdx - m32MAPpressureIdx,	// is trip index pointing to a valid pressure element?
	instrBranchIfLT, 4,									// if so, skip ahead
	instrLdRegByte, 0x02, 0,							// zero out result in register 2
	instrDone,											// exit to caller

//cont1:
	instrLdRegVariableOffset, 0x02, m32MAPpressureIdx,	// load pre-formatted pressure element
	instrDone											// exit to caller
};

#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)
static const uint8_t prgmDragSpeed[] PROGMEM = {		// tDragSpeed - acceleration test maximum vehicle speed (SI/SAE)
	instrLdRegByte, 0x02, 1,							// load 1 pulse into numerator term
	instrLdRegVariable, 0x01, m32DragInstantSpeedIdx,	// load instantaneous drag speed measurement into denominator term
	instrTestReg, 0x01,									// test speed measurement
	instrJump, tCalculateSpeed							// go calculate speed
};

static const uint8_t prgmTrapSpeed[] PROGMEM = {		// tTrapSpeed - acceleration test vehicle speed at defined distance (SI/SAE)
	instrLdRegByte, 0x02, 1,							// load 1 pulse into numerator term
	instrLdRegVariable, 0x01, m32DragTrapSpeedIdx,		// load instantaneous trap speed measurement into denominator term
	instrTestReg, 0x01,									// test speed measurement
	instrJump, tCalculateSpeed							// go calculate speed
};

static const uint8_t prgmEstimatedEnginePower[] PROGMEM = {	// tEstimatedEnginePower - estimated engine power (SI/SAE)
	instrCall, tDragSpeed,								// calculate vehicle speed (this will be the maximum vehicle speed obtained during the acceleration test)
	instrBranchIfSAEmode, 5,							// if MPGuino is in SAE mode, skip to power calculation setup

//cont1:
	instrMul2byRdOnly, idxDenomDistance,				// multiply by numerator to convert km/hr to MPH
	instrDiv2byRdOnly, idxNumerDistance,				// divide by denominator to convert km/hr to MPH
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont2:
	instrLdReg, 0x21,									// load register 1 with calculated vehicle speed
	instrMul2by1,										// perform cube function on maximum accel test vehicle speed value to obtain initial denominator term
	instrMul2by1,
	instrDiv2byRdOnly, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byEEPROM, pVehicleMassIdx,					// multiply estimate by vehicle weight
	instrDiv2byRdOnly, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrBranchIfSAEmode, 5,							// if MPGuino is in SAE mode, skip to power calculation setup

//cont3:
	instrMul2byRdOnly, idxDenomMass,					// multiply by numerator to convert kg to lbf
	instrDiv2byRdOnly, idxNumerMass,					// divide by denominator to convert kg to lbf
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//cont4:
	instrDiv2byRdOnly, idxPowerFactor,					// divide by power estimation term to reduce the number of bits in the estimate
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmAccelTestTime[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSScycleIdx,
	instrMul2byRdOnly, idxDecimalPoint,					// load the decimal point constant used for output formatting
	instrDiv2byRdOnly, idxCycles0PerSecond,
	instrDone
};

#endif // defined(useDragRaceFunction)
static const uint8_t * const S64programList[] PROGMEM = {
// this is the start of the SWEET64 display function list
	prgmEngineRunTime,							// tEngineRunTime - engine runtime (hhmmss)
	prgmRangeTime,								// tRangeTime - estimated total runtime from full tank (hhmmss)
	prgmReserveTime,							// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	prgmBingoTime,								// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	prgmTimeToEmpty,							// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	prgmReserveTimeToEmpty,						// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	prgmBingoTimeToEmpty,						// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	prgmMotionTime,								// tMotionTime - time vehicle in motion (hhmmss)
	prgmInjectorOpenTime,						// tInjectorOpenTime - fuel used (milliseconds)
	prgmInjectorTotalTime,						// tInjectorTotalTime - engine run time (milliseconds)
	prgmVSStotalTime,							// tVSStotalTime - time vehicle in motion (milliseconds)
	prgmEngineSpeed,							// tEngineSpeed - engine speed (1/m)
	prgmInjectorPulseCount,						// tInjectorPulseCount - fuel injector pulse count
	prgmVSSpulseCount,							// tVSSpulseCount - VSS pulse count
	prgmDistance,								// tDistance - vehicle distance traveled (SI/SAE)
	prgmSpeed,									// tSpeed - vehicle speed (SI/SAE)
	prgmFuelUsed,								// tFuelUsed - fuel quantity used (SI/SAE)
	prgmFuelRate,								// tFuelRate - fuel consumption rate (SI/SAE)
	prgmFuelEcon,								// tFuelEcon - fuel economy (SI/SAE)
	prgmRangeDistance,							// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	prgmReserveDistance,						// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	prgmBingoDistance,							// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	prgmDistanceToEmpty,						// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	prgmReserveDistanceToEmpty,					// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	prgmBingoDistanceToEmpty,					// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
#if defined(useFuelCost)
	prgmFuelCostUsed,							// tFuelCostUsed - cost of fuel quantity used
	prgmFuelRateCost,							// tFuelRateCost - fuel rate cost in currency units
	prgmFuelCostPerDistance,					// tFuelCostPerDistance - fuel cost per unit distance (SI/SAE)
	prgmDistancePerFuelCost,					// tDistancePerFuelCost - distance per unit fuel cost (SI/SAE)
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
	prgmAccelTestTime,							// tAccelTestTime - acceleration test time (sec)
#endif // defined(useDragRaceFunction)

// this is the start of the SWEET64 display function list for those programs which do not require a trip index
	prgmFuelQuantity,							// tFuelQuantity - tank total fuel quantity (SI/SAE)
	prgmReserveQuantity,						// tReserveQuantity - tank reserve fuel quantity (SI/SAE)
	prgmBingoQuantity,							// tBingoQuantity - tank bingo fuel quantity (SI/SAE)
	prgmRemainingFuel,							// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	prgmReserveRemainingFuel,					// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	prgmBingoRemainingFuel,						// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
#if defined(useDebugAnalog)
	prgmAnalogChannel,							// tAnalogChannel - DC voltage
#endif // defined(useDebugAnalog)
#if defined(useCarVoltageOutput)
	prgmAlternatorChannel,						// tAlternatorChannel - DC voltage
#endif // defined(useCarVoltageOutput)
#if defined(useChryslerMAPCorrection)
	prgmPressureChannel,						// tPressureChannel - absolute pressure (SI/SAE)
#endif // defined(useChryslerMAPCorrection)
#if defined(useFuelCost)
	prgmFuelCostTank,							// tFuelCostTank - full tank fuel cost in currency units
	prgmFuelCostReserve,						// tFuelCostReserve - reserve fuel quantity fuel cost in currency units
	prgmFuelCostBingo,							// tFuelCostBingo - bingo fuel quantity cost in currency units
	prgmFuelCostRemaining,						// tFuelCostRemaining - value of estimated remaining total fuel quantity in currency units
	prgmFuelCostReserveRemaining,				// tFuelCostReserveRemaining - value of estimated remaining reserve fuel quantity in currency units
	prgmFuelCostBingoRemaining,					// tFuelCostBingoRemaining - value of estimated remaining bingo fuel quantity in currency units
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
	prgmEstimatedEnginePower,					// tEstimatedEnginePower - estimated engine power (SI/SAE)
	prgmDragSpeed,								// tDragSpeed - acceleration test maximum vehicle speed (SI/SAE)
	prgmTrapSpeed,								// tTrapSpeed - acceleration test vehicle speed at defined distance (SI/SAE)
#endif // defined(useDragRaceFunction)

// this is the start of the SWEET64 function list for useful functions that do not get displayed
#if defined(useBluetooth)
	prgmGetBTparameterValue,					// tGetBTparameterValue
	prgmGetProgramVariableValue,				// tGetProgramVariableValue
#endif // defined(useBluetooth)

// this is the start of the internal SWEET64 index program address list
	prgmCalculateRemainingTank,					// tCalculateRemainingTank - calculate estimated remaining fuel quantity in injector open cycles
	prgmCalculateRemainingReserve,				// tCalculateRemainingReserve - calculate estimated remaining fuel reserve value in injector open cycles
	prgmCalculateBingoFuel,						// tCalculateBingoFuel - calculate estimated fuel bingo value in injector open cycles
	prgmConvertToMicroSeconds,					// tConvertToMilliSeconds - convert value in timer0 cycles to decimal formatted milliseconds
	prgmCalculateFuelQuantity,					// tCalculateFuelQuantity - convert fuel quantity in timer0 cycles to unit liquid quantity
	prgmCalculateFuelDistance,					// tCalculateFuelDistance
	prgmCalculateFuelTime,						// tCalculateFuelTime
	prgmCalculateSpeed,							// tCalculateSpeed
	prgmFormatToHHMMSStime,						// tFormatToHHMMSStime
	prgmFormatToH9MMSStime,						// tFormatToH9MMSStime
	prgmFormatToNumber,							// tFormatToNumber
	prgmRoundOffNumber,							// tRoundOffNumber
	prgmLoadTrip,								// tLoadTrip
	prgmSaveTrip,								// tSaveTrip
	prgmReadTicksToSeconds,						// tReadTicksToSeconds
#if defined(useBarFuelEconVsTime)
	prgmFEvTgetDistance,						// tFEvTgetDistance
	prgmFEvTgetConsumedFuel,					// tFEvTgetConsumedFuel
	prgmFEvTgetFuelEconomy,						// tFEvTgetFuelEconomy
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	prgmFEvSgetDistance,						// tFEvSgetDistance
	prgmFEvSgetConsumedFuel,					// tFEvSgetConsumedFuel
	prgmFEvSgetFuelEconomy,						// tFEvSgetFuelEconomy
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useDebugTerminal)
	prgmParseCharacterToReg,					// tParseCharacterToReg
#endif //defined(useDebugTerminal)
};

// trip functions are grouped into three categories, in order
//
//    functions that return results that do not require conversion between SI and SAE formats
//    functions that return results in either SI or SAE format, depending on MPGuino settings
//    functions that return results in either SI or SAE format, and have an alternate SI/SAE format, depending on MPGuino settings
//
// the below calculation formats are arranged with this in mind
//
#define nextAllowedValue 0 // S64programList
static const uint8_t calcFormatTimeHHmmSSIdx =			nextAllowedValue;						// time in HHmmSS format
static const uint8_t calcFormatTimeInMillisecondsIdx =	calcFormatTimeHHmmSSIdx + 1;			// time in milliseconds
static const uint8_t calcFormatEngineSpeedIdx =			calcFormatTimeInMillisecondsIdx + 1;	// engine speed
static const uint8_t calcFormatPulseCountIdx =			calcFormatEngineSpeedIdx + 1;			// pulse count
#define nextAllowedValue calcFormatPulseCountIdx + 1
#if defined(useDragRaceFunction)
static const uint8_t calcFormatTimeInSecondsIdx =		nextAllowedValue;						// time in seconds
#define nextAllowedValue calcFormatTimeInSecondsIdx + 1
#endif // defined(useDragRaceFunction)
#if defined(useAnalogRead)
static const uint8_t calcFormatAnalogDisplayIdx =		nextAllowedValue;						// voltage
#define nextAllowedValue calcFormatAnalogDisplayIdx + 1
#endif // useAnalogRead
#if defined(useFuelCost)
static const uint8_t calcFormatFuelCostIdx =			nextAllowedValue;						// fuel cost
static const uint8_t calcFormatFuelRateCostIdx =		calcFormatFuelCostIdx + 1;				// fuel rate cost
#define nextAllowedValue calcFormatFuelRateCostIdx + 1
#endif // defined(useFuelCost)

static const uint8_t calcFormatMaxValNonConversion =	nextAllowedValue;

// these calc format indices are separated by 2, to account for both SAE and metric units

static const uint8_t calcFormatFuelQuantityIdx =		nextAllowedValue;						// fuel quantity (SAE/SI)
static const uint8_t calcFormatFuelRateIdx =			calcFormatFuelQuantityIdx + 2;			// fuel rate (SAE/SI)
static const uint8_t calcFormatDistanceIdx =			calcFormatFuelRateIdx + 2;				// distance travelled (SAE/SI)
static const uint8_t calcFormatSpeedIdx =				calcFormatDistanceIdx + 2;				// speed (SAE/SI)
#define nextAllowedValue calcFormatSpeedIdx + 2
#if defined(useFuelCost)
static const uint8_t calcFormatFuelCostPerDistanceIdx =	nextAllowedValue;						// fuel cost per unit distance (SI/SAE)
static const uint8_t calcFormatDistancePerFuelCostIdx =	calcFormatFuelCostPerDistanceIdx + 2;	// distance per unit fuel cost (SI/SAE)
#define nextAllowedValue calcFormatDistancePerFuelCostIdx + 2
#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
static const uint8_t calcFormatPressureIdx =			nextAllowedValue;						// absolute pressure (SI/SAE)
#define nextAllowedValue calcFormatPressureIdx + 2
#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)
static const uint8_t calcFormatEstimatedPowerIdx =		nextAllowedValue;						// estimated engine power (SI/SAE)
#define nextAllowedValue calcFormatEstimatedPowerIdx + 2
#endif // defined(useDragRaceFunction)

static const uint8_t calcFormatMaxValSingleFormat =		nextAllowedValue;

// these calc format indices are separated by 4, to account for both SAE and metric units, and normal and alternate formats

static const uint8_t calcFormatFuelEconomyIdx =			nextAllowedValue;						// fuel economy (SAE/SI)
#define nextAllowedValue calcFormatFuelEconomyIdx + 4

static const uint8_t calcFormatListCount =				nextAllowedValue;

#if defined(useDebugTerminal) || defined(useJSONoutput)
static const char calcFormatLabels[] PROGMEM = {
	"hhmmss" tcEOS					// time in HHmmSS format
	"ms" tcEOS						// time in milliseconds
	"rev/min" tcEOS					// engine speed
	"pulses" tcEOS					// pulse count
#if defined(useDragRaceFunction)
	"sec" tcEOS						// time in seconds
#endif // defined(useDragRaceFunction)
#if defined(useAnalogRead)
	"V(dc)" tcEOS					// voltage
#endif // useAnalogRead
#if defined(useFuelCost)
	"cost" tcEOS					// fuel cost
	"cost/hour" tcEOS				// fuel rate cost
#endif // defined(useFuelCost)

	"gallon" tcEOS					// SAE fuel quantity
	"liter" tcEOS					// SI fuel quantity
	"gallon/hour" tcEOS				// SAE fuel rate
	"liter/hour" tcEOS				// SI fuel rate
	"mile" tcEOS					// SAE distance travelled
	"km" tcEOS						// SI distance travelled
	"MPH" tcEOS						// SAE speed
	"kPH" tcEOS						// SI speed
#if defined(useFuelCost)
	"cost/mile" tcEOS				// SAE fuel cost per unit distance
	"cost/km" tcEOS					// SI fuel cost per unit distance
	"mile/cost" tcEOS				// SAE distance per unit fuel cost
	"km/cost" tcEOS					// SI distance per unit fuel cost
#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
	"psia" tcEOS					// SAE pressure
	"kPa" tcEOS						// SI pressure
#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)
	"WHP" tcEOS						// SAE horsepower
	"kW" tcEOS						// SI horsepower
#endif // defined(useDragRaceFunction)

	"MPG" tcEOS						// SAE fuel economy
	"KPL" tcEOS						// alternate SI fuel economy
	"gal/100miles" tcEOS			// alternate SAE fuel economy
	"L/100km" tcEOS					// SI fuel economy
};

#endif // defined(useDebugTerminal) || defined(useJSONoutput)
static const uint8_t calcFormatDecimalPlaces[(uint16_t)(calcFormatListCount)] PROGMEM = { // S64programList
	0,				// time in HHmmSS format
	3,				// time in milliseconds
	0,				// engine speed
	0,				// pulse count
#if defined(useDragRaceFunction)
	1,				// time in seconds
#endif // defined(useDragRaceFunction)
#if defined(useAnalogRead)
	3,				// voltage
#endif // defined(useAnalogRead)
#if defined(useFuelCost)
	2,				// fuel cost
	2,				// fuel rate cost
#endif // defined(useFuelCost)

	2,				// SAE fuel quantity
	2,				// SI fuel quantity
	2,				// SAE fuel rate
	2,				// SI fuel rate
	(2 << 4) |	1,	// SAE distance travelled
	(2 << 4) |	1,	// SI distance travelled
	(2 << 4) |	1,	// SAE speed
	(2 << 4) |	1,	// SI speed
#if defined(useFuelCost)
	2,				// SAE fuel cost per unit distance
	2,				// SI fuel cost per unit distance
	1,				// SAE distance per unit fuel cost
	1,				// SI distance per unit fuel cost
#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
	2,				// SAE pressure
	2,				// SI pressure
#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)
	1,				// SAE horsepower
	1,				// SI horsepower
#endif // defined(useDragRaceFunction)

	(2 << 4) |	1,	// SAE fuel economy
	(2 << 4) |	1,	// alternate SI fuel economy
	(2 << 4) |	1,	// alternate SAE fuel economy
	(2 << 4) |	1,	// SI fuel economy
};

static const uint8_t calcFormatLabelText[(uint16_t)(calcFormatListCount)] PROGMEM = { // S64programList
	's',	// time in HHmmSS format
	'm',	// time in milliseconds
	't',	// engine speed
	'p',	// pulse count
#if defined(useDragRaceFunction)
	's',	// time in seconds
#endif // defined(useDragRaceFunction)
#if defined(useAnalogRead)
	'V',	// voltage
#endif // useAnalogRead
#if defined(useFuelCost)
	'$',	// fuel cost
	'#',	// fuel rate cost
#endif // defined(useFuelCost)

	'G',	// SAE fuel quantity
	'L',	// SI fuel quantity
	'g',	// SAE fuel rate
	'l',	// SI fuel rate
	'm',	// SAE distance travelled
	'k',	// SI distance travelled
	'S',	// SAE speed
	'S',	// SI speed
#if defined(useFuelCost)
	'C',	// SAE fuel cost per distance
	'C',	// SI fuel cost per distance
	'D',	// SAE distance per fuel cost
	'D',	// SI distance per fuel cost
#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
	'P',	// SAE pressure
	'P',	// SI pressure
#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)
	'H',	// SAE horsepower
	'W',	// SI horsepower
#endif // defined(useDragRaceFunction)

	'E',	// SAE fuel economy
	'E',	// alternate SI fuel economy
	'E',	// alternate SAE fuel economy
	'E',	// SI fuel economy
};

static const uint8_t isValidTripIdx =	0b10100000;
static const uint8_t isValidCalcIdx =	0b01100000;
static const uint8_t isValidFlag =		0b00100000;

static const uint8_t isValidCalcObj =	0b11000000;

#if defined(useSpiffyTripLabels)
static const uint8_t calcFormatLabelCGRAM[(uint16_t)(calcFormatListCount)][16] PROGMEM = { // S64programList
	// time in HHmmSS format
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101},

	// time in milliseconds
	{0b00010010, 0b00010010, 0b00010010, 0b00011110, 0b00010000, 0b00110000, 0b01000000, 0b01100000
	,0b00001110, 0b00010000, 0b00001100, 0b00000010, 0b00011100, 0b00000000, 0b00000000, 0b00000000},

	// engine speed
	{0b00001100, 0b00010000, 0b00010001, 0b00010010, 0b00000100, 0b00001000, 0b00000000, 0b00000000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011011, 0b00010101, 0b00010101},

	// pulse count
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001000, 0b00011100, 0b00001000, 0b00001000},
#if defined(useDragRaceFunction)

	// time in seconds
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000100, 0b00000011
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00010101, 0b00010110, 0b00010101},
#endif // defined(useDragRaceFunction)
#if defined(useAnalogRead)

	// voltage
	{0b00010001, 0b00010001, 0b00010001, 0b00001010, 0b00000100, 0b00000000, 0b00000000, 0b00000000
	,0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011010, 0b00011010, 0b00010001},
#endif // defined(useAnalogRead)
#if defined(useFuelCost)

	// fuel cost
	{0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100
	,0b00000010, 0b00000101, 0b00000100, 0b00001110, 0b00000100, 0b00000100, 0b00000100, 0b00000100},

	// fuel rate cost
	{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100},
#endif // defined(useFuelCost)

	// gallons
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000101, 0b00000011
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000001, 0b00000001, 0b00011001, 0b00011101},

	// liters
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000100, 0b00000100, 0b00000111
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},

	// gallons per hour
	{0b00001100, 0b00010000, 0b00010100, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100},

	// liters per hour
	{0b00010000, 0b00010000, 0b00010000, 0b00011100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100},

	// miles
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00000000, 0b00010000, 0b00010000},

	// kilometers
	{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000101, 0b00000110, 0b00000101
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00001010, 0b00010101, 0b00010101},

	// miles per hour
	{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010100, 0b00011100, 0b00010100, 0b00010100},

	// kilometers per hour
	{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00011100, 0b00010100},
#if defined(useFuelCost)

	// fuel cost per mile
	{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010001, 0b00011011, 0b00010101, 0b00010101},

	// fuel cost per kilometer
	{0b00001100, 0b00010000, 0b00010000, 0b00001100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010100, 0b00011000, 0b00010100},

	// mile per unit fuel cost
	{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100},

	// kilometer per unit fuel cost
	{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010000, 0b00001100},
#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)

	// psi
	{0b00001110, 0b00001001, 0b00001001, 0b00001110, 0b00001000, 0b00001000, 0b00001000, 0b00000000
	,0b00001001, 0b00010101, 0b00010001, 0b00001001, 0b00000101, 0b00010101, 0b00001001, 0b00000000},

	// kPa
	{0b00001000, 0b00001000, 0b00001001, 0b00001010, 0b00001100, 0b00001010, 0b00001001, 0b00000000
	,0b00011000, 0b00010100, 0b00010100, 0b00011000, 0b00010010, 0b00010101, 0b00010011, 0b00000000},
#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)

	// HP
	{0b00001001, 0b00001001, 0b00001001, 0b00001111, 0b00001001, 0b00001001, 0b00001001, 0b00000000
	,0b00100000, 0b01000000, 0b01100000, 0b00011000, 0b00010100, 0b00011000, 0b00010000, 0b00010000},

	// kW
	{0b00100000, 0b01000000, 0b01110000, 0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000000
	,0b00010001, 0b00010001, 0b00010101, 0b00010101, 0b00010101, 0b00001010, 0b00001010, 0b00000000},
#endif // defined(useDragRaceFunction)

	// miles per gallon
	{0b00011011, 0b00010101, 0b00010101, 0b00000000, 0b00000110, 0b00000101, 0b00000110, 0b00000100
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00001100, 0b00010000, 0b00010100, 0b00001100},

	// km per liter
	{0b00010000, 0b00010100, 0b00011000, 0b00010100, 0b00000001, 0b00000010, 0b00000100, 0b00001000
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00010000, 0b00010000, 0b00010000, 0b00011100},

	// gallons per 100 miles
	{0b00001100, 0b00010000, 0b00010100, 0b00001101, 0b00000010, 0b00000101, 0b00001001, 0b00000001
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111},

	// liters per 100 km
	{0b00010000, 0b00010000, 0b00011001, 0b00000010, 0b00000100, 0b00001001, 0b00000001, 0b00000001
	,0b00100000, 0b01000000, 0b01100000, 0b00000000, 0b00000000, 0b00011111, 0b00010101, 0b00011111},
};

#endif // defined(useSpiffyTripLabels)

// high bit set means the function does not depend on a trip variable, therefore does not require a trip label
//
static const uint8_t calcFormatList[(uint16_t)(dfMaxValDisplayCount)] PROGMEM = { // S64programList
	calcFormatTimeHHmmSSIdx,					// tEngineRunTime - engine runtime (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tRangeTime - estimated total runtime from full tank (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tReserveTime - estimated reserve runtime from full tank (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tBingoTime - estimated bingo fuel runtime from full tank (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tTimeToEmpty - estimated remaining engine runtime (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tReserveTimeToEmpty - estimated remaining reserve engine runtime (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tBingoTimeToEmpty - estimated bingo fuel quantity engine runtime (hhmmss)
	calcFormatTimeHHmmSSIdx,					// tMotionTime - time vehicle in motion (hhmmss)
	calcFormatTimeInMillisecondsIdx,			// tInjectorOpenTime - fuel used (milliseconds)
	calcFormatTimeInMillisecondsIdx,			// tInjectorTotalTime - engine run time (milliseconds)
	calcFormatTimeInMillisecondsIdx,			// tVSStotalTime - time vehicle in motion (milliseconds)
	calcFormatEngineSpeedIdx,					// tEngineSpeed - engine speed (1/m)
	calcFormatPulseCountIdx,					// tInjectorPulseCount - fuel injector pulse count
	calcFormatPulseCountIdx,					// tVSSpulseCount - VSS pulse count
	calcFormatDistanceIdx,						// tDistance - vehicle distance traveled (SI/SAE)
	calcFormatSpeedIdx,							// tSpeed - vehicle speed (SI/SAE)
	calcFormatFuelQuantityIdx,					// tFuelUsed - fuel quantity used (SI/SAE)
	calcFormatFuelRateIdx,						// tFuelRate - fuel consumption rate (SI/SAE)
	calcFormatFuelEconomyIdx,					// tFuelEcon - fuel economy (SI/SAE)
	calcFormatDistanceIdx,						// tRangeDistance - estimated total distance on a full tank (SI/SAE)
	calcFormatDistanceIdx,						// tReserveDistance - estimated reserve fuel tank distance (SI/SAE)
	calcFormatDistanceIdx,						// tBingoDistance - estimated bingo fuel tank distance (SI/SAE)
	calcFormatDistanceIdx,						// tDistanceToEmpty - estimated remaining distance (SI/SAE)
	calcFormatDistanceIdx,						// tReserveDistanceToEmpty - estimated reserve remaining distance (SI/SAE)
	calcFormatDistanceIdx,						// tBingoDistanceToEmpty - estimated bingo remaining distance (SI/SAE)
#if defined(useFuelCost)
	calcFormatFuelCostIdx,						// tFuelCostUsed - cost of fuel quantity used
	calcFormatFuelRateCostIdx,					// tFuelRateCost - fuel rate cost in currency units
	calcFormatFuelCostPerDistanceIdx,			// tFuelCostPerDistance - fuel cost per unit distance (SI/SAE)
	calcFormatDistancePerFuelCostIdx,			// tDistancePerFuelCost - distance per unit fuel cost (SI/SAE)
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
	calcFormatTimeInSecondsIdx,					// tAccelTestTime - acceleration test time (sec)
#endif // defined(useDragRaceFunction)
	calcFormatFuelQuantityIdx,					// tFuelQuantity - tank total fuel quantity (SI/SAE)
	calcFormatFuelQuantityIdx,					// tReserveQuantity - tank reserve fuel quantity (SI/SAE)
	calcFormatFuelQuantityIdx,					// tBingoQuantity - tank bingo fuel quantity (SI/SAE)
	calcFormatFuelQuantityIdx,					// tRemainingFuel - estimated remaining fuel quantity (SI/SAE)
	calcFormatFuelQuantityIdx,					// tReserveRemainingFuel - estimated remaining reserve fuel quantity (SI/SAE)
	calcFormatFuelQuantityIdx,					// tBingoRemainingFuel - estimated bingo fuel quantity remaining (SI/SAE)
#if defined(useDebugAnalog)
	calcFormatAnalogDisplayIdx,					// tAnalogChannel - DC voltage
#endif // defined(useDebugAnalog)
#if defined(useCarVoltageOutput)
	calcFormatAnalogDisplayIdx,					// tAlternatorChannel - DC voltage
#endif // defined(useCarVoltageOutput)
#if defined(useChryslerMAPCorrection)
	calcFormatPressureIdx,						// tPressureChannel - absolute pressure (SI/SAE)
#endif // defined(useChryslerMAPCorrection)
#if defined(useFuelCost)
	calcFormatFuelCostIdx,						// tFuelCostTank - full tank fuel cost in currency units
	calcFormatFuelCostIdx,						// tFuelCostReserve - reserve fuel quantity fuel cost in currency units
	calcFormatFuelCostIdx,						// tFuelCostBingo - bingo fuel quantity cost in currency units
	calcFormatFuelCostIdx,						// tFuelCostRemaining - value of estimated remaining total fuel quantity in currency units
	calcFormatFuelCostIdx,						// tFuelCostReserveRemaining - value of estimated remaining reserve fuel quantity in currency units
	calcFormatFuelCostIdx,						// tFuelCostBingoRemaining - value of estimated remaining bingo fuel quantity in currency units
#endif // defined(useFuelCost)
#if defined(useDragRaceFunction)
	calcFormatEstimatedPowerIdx,				// tEstimatedEnginePower - estimated engine power (SI/SAE)
	calcFormatSpeedIdx,							// tDragSpeed - acceleration test maximum vehicle speed (SI/SAE)
	calcFormatSpeedIdx,							// tTrapSpeed - acceleration test vehicle speed at defined distance (SI/SAE)
#endif // defined(useDragRaceFunction)
};

