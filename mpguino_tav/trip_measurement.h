namespace tripVar /* trip data collection basic function prototype */
{

	static void reset(uint8_t tripIdx);
	static void transfer(uint8_t srcTripIdx, uint8_t destTripIdx);
	static void update(uint8_t srcTripIdx, uint8_t destTripIdx);
	static void update64(uint64_t collectedCycleArray[], uint32_t value, uint8_t destTripIdx);
	static void update64(uint64_t collectedCycleArray[], uint32_t collectedPulseArray[], uint32_t value, uint8_t destTripIdx);
	static void add32(uint32_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx);
	static void add64(uint64_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx);
#if defined(useEEPROMtripStorage)
	static uint8_t getBaseEEPROMaddress(uint8_t tripIdx, uint8_t dataIdx);
#endif // defined(useEEPROMtripStorage)

};

const uint8_t rvVSSpulseIdx = 0; 			// distance pulse count
const uint8_t rvVSScycleIdx = 1; 			// time that the vehicle has spent moving
const uint8_t rvInjPulseIdx = 2; 			// engine revolution count
const uint8_t rvInjCycleIdx = 3; 			// time that the fuel injector has been open
const uint8_t rvEngCycleIdx = 4; 			// engine run time

const uint8_t rvMeasuredCount = 5;

#if defined(useDebugTerminalLabels)
static const char terminalTripVarLabels[] PROGMEM = {
	"rvVSSpulseIdx" tcEOS
	"rvVSScycleIdx" tcEOS
	"rvInjPulseIdx" tcEOS
	"rvInjCycleIdx" tcEOS
	"rvEngCycleIdx" tcEOS
};

#endif // defined(useDebugTerminalLabels)
// trip index definitions for all of the trip variables used within MPGuino

static volatile uint8_t curRawTripIdx;
static uint8_t oldRawTripIdx;
#if defined(trackIdleEOCdata)
static volatile uint8_t curRawEOCidleTripIdx;
static uint8_t oldRawEOCidleTripIdx;
#endif // defined(trackIdleEOCdata)

#if defined(useWindowTripFilter)
const uint8_t windowTripFilterSize = 4;
uint8_t wtpCurrentIdx;

#endif // defined(useWindowTripFilter)
#define nextAllowedValue 0
const uint8_t raw0tripIdx =				nextAllowedValue;
const uint8_t raw1tripIdx =				raw0tripIdx + 1;
#define nextAllowedValue raw1tripIdx + 1
#if defined(trackIdleEOCdata)
const uint8_t raw0eocIdleTripIdx =		nextAllowedValue;
const uint8_t raw1eocIdleTripIdx =		raw0eocIdleTripIdx + 1;
#define nextAllowedValue raw1eocIdleTripIdx + 1
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
const uint8_t dragRawHalfSpeedIdx =		nextAllowedValue;
const uint8_t dragRawFullSpeedIdx =		dragRawHalfSpeedIdx + 1;
const uint8_t dragRawDistanceIdx =		dragRawFullSpeedIdx + 1;
#define nextAllowedValue dragRawDistanceIdx + 1
#endif // defined(useDragRaceFunction)

const uint8_t instantIdx =				nextAllowedValue;
const uint8_t currentIdx =				instantIdx + 1;
const uint8_t tankIdx =					currentIdx + 1;
#define nextAllowedValue tankIdx + 1
#if defined(trackIdleEOCdata)
const uint8_t eocIdleInstantIdx =		nextAllowedValue;
const uint8_t eocIdleCurrentIdx =		eocIdleInstantIdx + 1;
const uint8_t eocIdleTankIdx =			eocIdleCurrentIdx + 1;
#define nextAllowedValue eocIdleTankIdx + 1
#endif // defined(trackIdleEOCdata)

#if defined(useDebugTerminal)
const uint8_t terminalIdx =				nextAllowedValue;
#define nextAllowedValue terminalIdx + 1
#endif // defined(useDebugTerminal)

#if defined(useDragRaceFunction)
const uint8_t dragHalfSpeedIdx =		nextAllowedValue;
const uint8_t dragFullSpeedIdx =		dragHalfSpeedIdx + 1;
const uint8_t dragDistanceIdx =			dragFullSpeedIdx + 1;
#define nextAllowedValue dragDistanceIdx + 1
#endif // defined(useDragRaceFunction)

#if defined(useWindowTripFilter)
const uint8_t windowTripFilterIdx =		nextAllowedValue;
#define nextAllowedValue windowTripFilterIdx + windowTripFilterSize
#endif // defined(useWindowTripFilter)

const uint8_t tripSlotFullCount =		nextAllowedValue;

#if defined(useBarFuelEconVsTime)
const uint8_t FEvsTimeIdx =				nextAllowedValue;
const uint8_t FEvsTimeEndIdx =			FEvsTimeIdx + bgDataSize - 1;
#define nextAllowedValue FEvsTimeEndIdx + 1
#endif // defined(useBarFuelEconVsTime)

#if defined(useBarFuelEconVsSpeed)
const uint8_t FEvsSpeedIdx =			nextAllowedValue;
#define nextAllowedValue FEvsSpeedIdx + bgDataSize
#endif // defined(useBarFuelEconVsSpeed)

const uint8_t tripSlotCount =			nextAllowedValue;

#if defined(useEEPROMtripStorage)
const uint8_t EEPROMcurrentIdx =		nextAllowedValue;
const uint8_t EEPROMtankIdx =			EEPROMcurrentIdx + 1;
#define nextAllowedValue EEPROMtankIdx + 1
#if defined(trackIdleEOCdata)
const uint8_t EEPROMeocIdleCurrentIdx =	nextAllowedValue;
const uint8_t EEPROMeocIdleTankIdx =	EEPROMeocIdleCurrentIdx + 1;
#define nextAllowedValue EEPROMeocIdleTankIdx + 1
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)

const uint8_t tripSlotTotalCount =		nextAllowedValue;

static const char tripFormatLabelText[(uint16_t)(tripSlotTotalCount)] PROGMEM = {
	'0',
	'1',
#if defined(trackIdleEOCdata)
	'\\',
	'/',
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
	'h',
	'f',
	'd',
#endif // defined(useDragRaceFunction)
	'I',
	'C',
	'T',
#if defined(trackIdleEOCdata)
	'i',
	'c',
	't',
#endif // defined(trackIdleEOCdata)
#if defined(useDebugTerminal)
	']',
#endif // defined(useDebugTerminal)
#if defined(useDragRaceFunction)
	'H',
	'F',
	'D',
#endif // defined(useDragRaceFunction)
#if defined(useWindowTripFilter)
	'W',					// ensure there are as many of these as is specified in windowTripFilterSize
	'W',
	'W',
	'W',
#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsTime)
	'P',					// ensure there are as many of these as is specified in bgDataSize
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
	'P',
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	'0',					// ensure there are as many of these as is specified in bgDataSize
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'A',
	'B',
	'C',
	'D',
	'E',
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useEEPROMtripStorage)
	'<',
	'(',
#if defined(trackIdleEOCdata)
	'[',
	'{',
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

// trip format indexes for displayed trip function display variables
#define nextAllowedValue 0
static const uint8_t tripFormatInstantIdx =			nextAllowedValue;
static const uint8_t tripFormatCurrentIdx =			tripFormatInstantIdx + 1;
static const uint8_t tripFormatTankIdx =			tripFormatCurrentIdx + 1;
#define nextAllowedValue tripFormatTankIdx + 1
#if defined(trackIdleEOCdata)
static const uint8_t tripFormatEOCidleCurrentIdx =	nextAllowedValue;
static const uint8_t tripFormatEOCidleTankIdx =		tripFormatEOCidleCurrentIdx + 1;
static const uint8_t tripFormatEOCidleInstantIdx =	tripFormatEOCidleTankIdx + 1;
#define nextAllowedValue tripFormatEOCidleInstantIdx + 1
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
static const uint8_t tripFormatDragHalfSpeedIdx =	nextAllowedValue;
static const uint8_t tripFormatDragFullSpeedIdx =	tripFormatDragHalfSpeedIdx + 1;
static const uint8_t tripFormatDragDistanceIdx =	tripFormatDragFullSpeedIdx + 1;
#define nextAllowedValue tripFormatDragDistanceIdx + 1
#endif // defined(useDragRaceFunction)

static const uint8_t tripFormatIdxCount =			nextAllowedValue;

static const uint8_t tripFormatReverseList[] PROGMEM = {
	255,						// there should hopefully never be a trip index with this value
	instantIdx,
	currentIdx,
	tankIdx,
#if defined(trackIdleEOCdata)
	eocIdleCurrentIdx,
	eocIdleTankIdx,
	eocIdleInstantIdx,
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
	dragHalfSpeedIdx,
	dragFullSpeedIdx,
	dragDistanceIdx,
#endif // defined(useDragRaceFunction)
};

static const char tripFormatReverseNames[] PROGMEM = {
	"INST" tcEOS
	"CURR" tcEOS
	"TANK" tcEOS
#if defined(trackIdleEOCdata)
	"cC/I" tcEOS
	"tC/I" tcEOS
	"iC/I" tcEOS
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
	"D1/2" tcEOS
	"DF/S" tcEOS
	"DDST" tcEOS
#endif // defined(useDragRaceFunction)
};

#if defined(useSpiffyTripLabels)
// display variable trip labels
static const uint8_t tripFormatLabelCGRAM[][4] PROGMEM = {
	{0b00000000, 0b00000000, 0b00000000, 0b00000000}, // blank

	{0b00000000, 0b00000111, 0b00000010, 0b00000111}, // I
 	{0b00000000, 0b00000011, 0b00000100, 0b00000011}, // C
	{0b00000000, 0b00000111, 0b00000010, 0b00000010}, // T
#if defined(trackIdleEOCdata)
	{0b00000000, 0b00000011, 0b00000100, 0b00000110}, // italic C
	{0b00000000, 0b00000111, 0b00000010, 0b00000100}, // italic T
	{0b00000000, 0b00000011, 0b00000010, 0b00000110}, // italic I
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
	{0b00000000, 0b00000001, 0b00000010, 0b00000100}, // /
	{0b00000000, 0b00000010, 0b00000101, 0b00000010}, // full circle
	{0b00000000, 0b00000110, 0b00000101, 0b00000110}, // D
#endif // defined(useDragRaceFunction)
};

#endif // defined(useSpiffyTripLabels)
volatile uint32_t collectedVSSpulseCount[(uint16_t)(tripSlotCount)];
volatile uint64_t collectedInjCycleCount[(uint16_t)(tripSlotCount)];

volatile uint32_t collectedInjPulseCount[(uint16_t)(tripSlotFullCount)];
volatile uint64_t collectedVSScycleCount[(uint16_t)(tripSlotFullCount)];
volatile uint64_t collectedEngCycleCount[(uint16_t)(tripSlotFullCount)];

#if defined(useDebugTerminalLabels)
static const char terminalTripVarNames[] PROGMEM = {
	"raw0tripIdx" tcEOS
	"raw1tripIdx" tcEOS
#if defined(trackIdleEOCdata)
	"raw0eocIdleTripIdx" tcEOS
	"raw1eocIdleTripIdx" tcEOS
#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
	"dragRawHalfSpeedIdx" tcEOS
	"dragRawFullSpeedIdx" tcEOS
	"dragRawDistanceIdx" tcEOS
#endif // defined(useDragRaceFunction)
	"instantIdx" tcEOS
	"currentIdx" tcEOS
	"tankIdx" tcEOS
#if defined(trackIdleEOCdata)
	"eocIdleInstantIdx" tcEOS
	"eocIdleCurrentIdx" tcEOS
	"eocIdleTankIdx" tcEOS
#endif // defined(trackIdleEOCdata)
	"terminalIdx" tcEOS
#if defined(useDragRaceFunction)
	"dragHalfSpeedIdx" tcEOS
	"dragFullSpeedIdx" tcEOS
	"dragDistanceIdx" tcEOS
#endif // defined(useDragRaceFunction)
#if defined(useWindowTripFilter)
	"windowTripFilterIdx[00]" tcEOS
	"windowTripFilterIdx[01]" tcEOS
	"windowTripFilterIdx[02]" tcEOS
	"windowTripFilterIdx[03]" tcEOS
#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsTime)
	"FEvsTimeIdx[00]" tcEOS
	"FEvsTimeIdx[01]" tcEOS
	"FEvsTimeIdx[02]" tcEOS
	"FEvsTimeIdx[03]" tcEOS
	"FEvsTimeIdx[04]" tcEOS
	"FEvsTimeIdx[05]" tcEOS
	"FEvsTimeIdx[06]" tcEOS
	"FEvsTimeIdx[07]" tcEOS
	"FEvsTimeIdx[08]" tcEOS
	"FEvsTimeIdx[09]" tcEOS
	"FEvsTimeIdx[10]" tcEOS
	"FEvsTimeIdx[11]" tcEOS
	"FEvsTimeIdx[12]" tcEOS
	"FEvsTimeIdx[13]" tcEOS
	"FEvsTimeIdx[14]" tcEOS
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	"FEvsSpeedIdx[00]" tcEOS
	"FEvsSpeedIdx[01]" tcEOS
	"FEvsSpeedIdx[02]" tcEOS
	"FEvsSpeedIdx[03]" tcEOS
	"FEvsSpeedIdx[04]" tcEOS
	"FEvsSpeedIdx[05]" tcEOS
	"FEvsSpeedIdx[06]" tcEOS
	"FEvsSpeedIdx[07]" tcEOS
	"FEvsSpeedIdx[08]" tcEOS
	"FEvsSpeedIdx[09]" tcEOS
	"FEvsSpeedIdx[10]" tcEOS
	"FEvsSpeedIdx[11]" tcEOS
	"FEvsSpeedIdx[12]" tcEOS
	"FEvsSpeedIdx[13]" tcEOS
	"FEvsSpeedIdx[14]" tcEOS
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useEEPROMtripStorage)
	"EEPROMcurrentIdx" tcEOS
	"EEPROMtankIdx" tcEOS
#if defined(trackIdleEOCdata)
	"EEPROMeocIdleCurrentIdx" tcEOS
	"EEPROMeocIdleTankIdx" tcEOS
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

#endif // defined(useDebugTerminalLabels)
namespace tripSupport /* Trip save/restore/reset support section prototype */
{

	static void init(void);
	static void mainProcess(void);
	static uint8_t translateTripIndex(uint8_t tripTransferIdx, uint8_t tripDirIndex);
	static void doResetTrip(uint8_t tripSlot);
#if defined(useButtonInput)
	static void resetCurrent(void);
	static void resetTank(void);
	static void outputResetStatus(uint8_t tripSlot);
#endif // defined(useButtonInput)
#if defined(useWindowTripFilter)
	static void resetWindowFilter(void);
#endif // defined(useWindowTripFilter)

};

static const uint8_t tripUpdateListSize = 2				// base trip update count
#if defined(trackIdleEOCdata)
	+ 4													// count of idle/EOC trips to be updated
#endif // defined(trackIdleEOCdata)
#if defined(useWindowTripFilter)
	+ 5													// count of window filter trips to be updated
#else // defined(useWindowTripFilter)
	+ 1													// count of instant trips to be updated
#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsTime)
	+ 1													// count of fuel econ vs time bargraph trips to be updated
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	+ 1													// count of fuel econ vs speed bargraph trips to be updated
#endif // defined(useBarFuelEconVsSpeed)
;

// trip variable update list - tells MPGuino in what order and how to update the various tracked trip variables
//
// entries can be either actual trip variable indices, or dummy trip indices that are then translated to actual trip indices
//
// currently defined dummy trip indices are defined below:
//
// 0x7F - non-active raw trip
// 0x7E - non-active raw idle/EOC trip
// 0x7D - currently active window trip
// 0x7C - currently active fuel econ vs. time trip
// 0x7B - currently active fuel econ vs. speed trip
//
// in addition, the destination trip has a definable bit 7
//  0b1xxx xxxx - transfer source trip values to destination trip
//  0b0xxx xxxx - add source trip valaues to destination trip, and then store results in destination trip
//
static const uint8_t tripUpdateList[(uint16_t)(tripUpdateListSize)][2] PROGMEM = {
#if defined(trackIdleEOCdata)
	{0x7E						,eocIdleInstantIdx | 0x80},	// transfer old raw idle trip to idle instant trip
	{0x7E						,eocIdleCurrentIdx},		// update idle current trip with old raw idle trip
	{0x7E						,eocIdleTankIdx},			// update idle tank trip with old raw idle trip
	{0x7E						,0x7F},						// update old raw trip with old raw idle trip
#endif // defined(trackIdleEOCdata)
#if defined(useWindowTripFilter)
	{0x7F						,0x7D | 0x80},				// transfer old raw trip to current window trip
	{windowTripFilterIdx		,instantIdx | 0x80},		// transfer window trip 1 to instant trip
	{windowTripFilterIdx + 1	,instantIdx},				// update instant trip with window trip 2
	{windowTripFilterIdx + 2	,instantIdx},				// update instant trip with window trip 3
	{windowTripFilterIdx + 3	,instantIdx},				// update instant trip with window trip 4
#else // defined(useWindowTripFilter)
	{0x7F						,instantIdx | 0x80},		// transfer old raw trip to instant trip
#endif // defined(useWindowTripFilter)
	{0x7F						,currentIdx},				// update current trip with old raw trip
	{0x7F						,tankIdx},					// update tank trip with old raw trip
#if defined(useBarFuelEconVsTime)
	{0x7F						,0x7C},						// update fuel econ vs time bargraph trip with old raw trip
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	{0x7F						,0x7B},						// update fuel econ vs speed bargraph trip with old raw trip
#endif // defined(useBarFuelEconVsSpeed)
};

const uint8_t tripSelectList[] PROGMEM = {
	currentIdx,
	tankIdx,
#if defined(trackIdleEOCdata)
	eocIdleCurrentIdx,
	eocIdleTankIdx,
#endif // defined(trackIdleEOCdata)
};

#if defined(useChryslerMAPCorrection)
namespace pressureCorrect /* Chrysler returnless fuel pressure correction display section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static uint16_t getPressureCorrectPageFormats(uint8_t formatIdx);

}

static const char pressureCorrectDisplayTitles[] PROGMEM = {
	"Pressures" tcEOS
};

static const uint16_t pressureCorrectPageFormats[4] PROGMEM = {
	((m32MAPpressureIdx - m32MAPpressureIdx) << 8 ) |	(tPressureChannel),		// Pressures
	((m32BaroPressureIdx - m32MAPpressureIdx) << 8 ) |	(tPressureChannel),
	((m32FuelPressureIdx - m32MAPpressureIdx) << 8 ) |	(tPressureChannel),
	((m32InjPressureIdx - m32MAPpressureIdx) << 8 ) |	(tPressureChannel),
};

#endif // defined(useChryslerMAPCorrection)
#if defined(useEnhancedTripReset)
namespace tripSave /* Trip save/restore/reset display support section prototype */
{

#if defined(useButtonInput)
	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);
	static void goSaveCurrent(void);
	static void goSaveTank(void);
#endif // defined(useButtonInput)
#if defined(useSavedTrips)
	static uint8_t doReadTrip(uint8_t tripSlot);
	static uint8_t doWriteTrip(uint8_t tripSlot);
	static uint8_t doAutoAction(uint8_t taaMode);
#endif // defined(useSavedTrips)

};

#define nextAllowedValue 0
#if defined(useSavedTrips)
static const uint8_t displayStartTripSaveCurrent =		nextAllowedValue;

static const uint8_t tsfCurrentSaveIdx =				nextAllowedValue;
static const uint8_t tsfCurrentLoadIdx =				tsfCurrentSaveIdx + 1;
static const uint8_t tsfCurrentResetIdx =				tsfCurrentLoadIdx + 1;
#define nextAllowedValue tsfCurrentResetIdx + 1

static const uint8_t displayCountTripSaveCurrent =		nextAllowedValue - displayStartTripSaveCurrent;

#endif // defined(useSavedTrips)
static const uint8_t displayStartTripSaveTank =			nextAllowedValue;

#if defined(usePartialRefuel)
static const uint8_t tsfAddPartialIdx =					nextAllowedValue;
static const uint8_t tsfZeroPartialIdx =				tsfAddPartialIdx + 1;
#define nextAllowedValue tsfZeroPartialIdx + 1
#endif // defined(usePartialRefuel)
#if defined(useSavedTrips)
static const uint8_t tsfTankSaveIdx =					nextAllowedValue;
static const uint8_t tsfTankLoadIdx =					tsfTankSaveIdx + 1;
#define nextAllowedValue tsfTankLoadIdx + 1
#endif // defined(useSavedTrips)
static const uint8_t tsfTankResetIdx =					nextAllowedValue;
#define nextAllowedValue tsfTankResetIdx + 1

static const uint8_t displayCountTripSaveTank =			nextAllowedValue - displayStartTripSaveTank;

static const char tripSaveMenuTitles[] PROGMEM = {
#if defined(useSavedTrips)
	"Save CURR Trip" tcEOSCR
	"Load CURR Trip" tcEOSCR
	"Reset CURR Trip" tcEOSCR
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
#if defined(useImperialGallon)
	"Add " tcOMOFF "ImpGal" tcOTOG "Prtl L" tcOON "*1K" tcEOSCR
#else // defined(useImperialGallon)
	"Add Prtl " tcOMOFF "gal" tcOTOG "L" tcOON "*1K" tcEOSCR
#endif // defined(useImperialGallon)
	"Zero Partial" tcEOSCR
#endif // defined(usePartialRefuel)
#if defined(useSavedTrips)
	"Save TANK Trip" tcEOSCR
	"Load TANK Trip" tcEOSCR
#endif // defined(useSavedTrips)
	"Reset TANK Trip" tcEOSCR
};

#if defined(useSavedTrips)
static uint8_t thisTripSlot;
static uint8_t topScreenLevel;

static const uint8_t taaModeWrite =			0;
static const uint8_t taaModeRead =			taaModeWrite + 1;

const uint8_t tripSignatureList[] PROGMEM = {
	pCurrTripSignatureIdx,
	pTankTripSignatureIdx,
};

const uint8_t tripSlotStatus[] PROGMEM = {
	"  Present" tcOTOG "  Empty" tcEOSCR
};

#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
static const char prStatusMessages[] PROGMEM = {
	"No change" tcEOSCR
	"Added Partial" tcEOSCR
	"Canx Partial Add" tcEOSCR
};

#endif // defined(usePartialRefuel)
#endif // defined(useEnhancedTripReset)
