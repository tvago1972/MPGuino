namespace tripVar /* trip data collection basic function prototype */
{

	static void reset(uint8_t tripIdx);
	static void transfer(uint8_t srcTripIdx, uint8_t destTripIdx);
	static void update(uint8_t srcTripIdx, uint8_t destTripIdx);
	static void add64(uint64_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx);
	static void add64(uint64_t collectedArray[], uint32_t value, uint8_t destTripIdx);
	static void add32(uint32_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx);
#ifdef useEEPROMtripStorage
	static uint8_t getBaseEEPROMaddress(uint8_t tripIdx, uint8_t dataIdx);
#endif // useEEPROMtripStorage

};

namespace tripSupport /* Trip save/restore/reset support section prototype */
{

	static void init(void);
	static void idleProcess(void);
	static uint8_t translateTripIndex(const uint8_t tripTranslateList[], uint8_t tripListPos);
#ifdef useSavedTrips
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void goSaveCurrent(void);
	static void goSaveTank(void);
	static void select(void);
	static uint8_t doReadTrip(uint8_t tripSlot);
	static uint8_t doWriteTrip(uint8_t tripSlot);
	static uint8_t doAutoAction(uint8_t taaMode);
#endif // useSavedTrips
	static void displayStatus(uint8_t tripSlot, const char * str);
	static void doResetTrip(uint8_t tripSlot);
	static void resetCurrent(void);
	static void resetTank(void);
#ifdef useWindowTripFilter
	static void resetWindowFilter(void);
#endif // useWindowTripFilter

};

#ifdef useChryslerMAPCorrection
namespace pressureCorrect /* Chrysler returnless fuel pressure correction display section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

}

const char pressureCorrectScreenFuncNames[] PROGMEM = {
	"Pressures\r"
};

#endif // useChryslerMAPCorrection
const uint8_t rvVSSpulseIdx = 0; 			// distance pulse count
const uint8_t rvVSScycleIdx = 1; 			// time that the vehicle has spent moving
const uint8_t rvInjPulseIdx = 2; 			// engine revolution count
const uint8_t rvInjCycleIdx = 3; 			// time that the fuel injector has been open
const uint8_t rvEngCycleIdx = 4; 			// engine run time

const uint8_t rvMeasuredCount = 5;

#ifdef useDebugTerminalLabels
static const char terminalTripVarLabels[] PROGMEM = {
	"rvVSSpulseIdx\r"
	"rvVSScycleIdx\r"
	"rvInjPulseIdx\r"
	"rvInjCycleIdx\r"
	"rvEngCycleIdx\r"
};

#endif // useDebugTerminalLabels
// trip index definitions for all of the trip variables used within MPGuino

volatile uint8_t rawTripIdx;
#ifdef trackIdleEOCdata
volatile uint8_t rawEOCidleTripIdx;
#endif // trackIdleEOCdata

#ifdef useWindowTripFilter
const uint8_t windowTripFilterSize = 4;
uint8_t wtpCurrentIdx;

#endif // useWindowTripFilter
#define nextAllowedValue 0
const uint8_t raw0tripIdx =				nextAllowedValue;
const uint8_t raw1tripIdx =				raw0tripIdx + 1;
const uint8_t instantIdx =				raw1tripIdx + 1;
const uint8_t currentIdx =				instantIdx + 1;
const uint8_t tankIdx =					currentIdx + 1;
#define nextAllowedValue tankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t raw0eocIdleTripIdx =		nextAllowedValue;
const uint8_t raw1eocIdleTripIdx =		raw0eocIdleTripIdx + 1;
const uint8_t eocIdleInstantIdx =		raw1eocIdleTripIdx + 1;
const uint8_t eocIdleCurrentIdx =		eocIdleInstantIdx + 1;
const uint8_t eocIdleTankIdx =			eocIdleCurrentIdx + 1;
#define nextAllowedValue eocIdleTankIdx + 1
#endif // trackIdleEOCdata

#ifdef useDebugTerminal
const uint8_t terminalIdx =				nextAllowedValue;
#define nextAllowedValue terminalIdx + 1
#endif // useDebugTerminal

#ifdef useDragRaceFunction
const uint8_t dragRawHalfSpeedIdx =		nextAllowedValue;
const uint8_t dragRawFullSpeedIdx =		dragRawHalfSpeedIdx + 1;
const uint8_t dragRawDistanceIdx =		dragRawFullSpeedIdx + 1;
const uint8_t dragHalfSpeedIdx =		dragRawDistanceIdx + 1;
const uint8_t dragFullSpeedIdx =		dragHalfSpeedIdx + 1;
const uint8_t dragDistanceIdx =			dragFullSpeedIdx + 1;
#define nextAllowedValue dragDistanceIdx + 1
#endif // useDragRaceFunction

#ifdef useWindowTripFilter
const uint8_t windowTripFilterIdx =		nextAllowedValue;
#define nextAllowedValue windowTripFilterIdx + windowTripFilterSize
#endif // useWindowTripFilter

const uint8_t tripSlotFullCount =		nextAllowedValue;

#ifdef useBarFuelEconVsTime
const uint8_t FEvsTimeIdx =				nextAllowedValue;
const uint8_t FEvsTimeEndIdx =			FEvsTimeIdx + bgDataSize - 1;
#define nextAllowedValue FEvsTimeEndIdx + 1
#endif // useBarFuelEconVsTime

#ifdef useBarFuelEconVsSpeed
const uint8_t FEvsSpeedIdx =			nextAllowedValue;
#define nextAllowedValue FEvsSpeedIdx + bgDataSize
#endif // useBarFuelEconVsSpeed

const uint8_t tripSlotCount =			nextAllowedValue;

#ifdef useEEPROMtripStorage
const uint8_t EEPROMcurrentIdx =		nextAllowedValue;
const uint8_t EEPROMtankIdx =			EEPROMcurrentIdx + 1;
#define nextAllowedValue EEPROMtankIdx + 1
#ifdef trackIdleEOCdata
const uint8_t EEPROMeocIdleCurrentIdx =	nextAllowedValue;
const uint8_t EEPROMeocIdleTankIdx =	EEPROMeocIdleCurrentIdx + 1;
#define nextAllowedValue EEPROMeocIdleTankIdx + 1
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage

const uint8_t tripSlotTotalCount =		nextAllowedValue;

static const char tripVarChars[(uint16_t)(tripSlotTotalCount)] PROGMEM = {
	 '0'
	,'1'
	,'I'
	,'C'
	,'T'
#ifdef trackIdleEOCdata
	,'\\'
	,'/'
	,'i'
	,'c'
	,'t'
#endif // trackIdleEOCdata
#ifdef useDebugTerminal
	,']'
#endif // useDebugTerminal
#ifdef useDragRaceFunction
	,'h'
	,'f'
	,'d'
	,'H'
	,'F'
	,'D'
#endif // useDragRaceFunction
#ifdef useWindowTripFilter
	,'W'						// ensure there are as many of these as is specified in windowTripFilterSize
	,'W'
	,'W'
	,'W'
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,'P'						// ensure there are as many of these as is specified in bgDataSize
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
	,'P'
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,'0'						// ensure there are as many of these as is specified in bgDataSize
	,'1'
	,'2'
	,'3'
	,'4'
	,'5'
	,'6'
	,'7'
	,'8'
	,'9'
	,'A'
	,'B'
	,'C'
	,'D'
	,'E'
#endif // useBarFuelEconVsSpeed
#ifdef useEEPROMtripStorage
	,'<'
	,'('
#ifdef trackIdleEOCdata
	,'['
	,'{'
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
};

volatile uint32_t collectedVSSpulseCount[(uint16_t)(tripSlotCount)];
volatile uint64_t collectedInjCycleCount[(uint16_t)(tripSlotCount)];

volatile uint32_t collectedInjPulseCount[(uint16_t)(tripSlotFullCount)];
volatile uint64_t collectedVSScycleCount[(uint16_t)(tripSlotFullCount)];
volatile uint64_t collectedEngCycleCount[(uint16_t)(tripSlotFullCount)];

#ifdef useDebugTerminal
static const char terminalTripVarNames[] PROGMEM = {
	"raw0tripIdx\r"
	"raw1tripIdx\r"
	"instantIdx\r"
	"currentIdx\r"
	"tankIdx\r"
#ifdef trackIdleEOCdata
	"raw0eocIdleTripIdx\r"
	"raw1eocIdleTripIdx\r"
	"eocIdleInstantIdx\r"
	"eocIdleCurrentIdx\r"
	"eocIdleTankIdx\r"
#endif // trackIdleEOCdata
	"terminalIdx\r"
#ifdef useDragRaceFunction
	"dragRawHalfSpeedIdx\r"
	"dragRawFullSpeedIdx\r"
	"dragRawDistanceIdx\r"
	"dragHalfSpeedIdx\r"
	"dragFullSpeedIdx\r"
	"dragDistanceIdx\r"
#endif // useDragRaceFunction
#ifdef useWindowTripFilter
	"windowTripFilterIdx[00]\r"
	"windowTripFilterIdx[01]\r"
	"windowTripFilterIdx[02]\r"
	"windowTripFilterIdx[03]\r"
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	"FEvsTimeIdx[00]\r"
	"FEvsTimeIdx[01]\r"
	"FEvsTimeIdx[02]\r"
	"FEvsTimeIdx[03]\r"
	"FEvsTimeIdx[04]\r"
	"FEvsTimeIdx[05]\r"
	"FEvsTimeIdx[06]\r"
	"FEvsTimeIdx[07]\r"
	"FEvsTimeIdx[08]\r"
	"FEvsTimeIdx[09]\r"
	"FEvsTimeIdx[10]\r"
	"FEvsTimeIdx[11]\r"
	"FEvsTimeIdx[12]\r"
	"FEvsTimeIdx[13]\r"
	"FEvsTimeIdx[14]\r"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"FEvsSpeedIdx[00]\r"
	"FEvsSpeedIdx[01]\r"
	"FEvsSpeedIdx[02]\r"
	"FEvsSpeedIdx[03]\r"
	"FEvsSpeedIdx[04]\r"
	"FEvsSpeedIdx[05]\r"
	"FEvsSpeedIdx[06]\r"
	"FEvsSpeedIdx[07]\r"
	"FEvsSpeedIdx[08]\r"
	"FEvsSpeedIdx[09]\r"
	"FEvsSpeedIdx[10]\r"
	"FEvsSpeedIdx[11]\r"
	"FEvsSpeedIdx[12]\r"
	"FEvsSpeedIdx[13]\r"
	"FEvsSpeedIdx[14]\r"
#endif // useBarFuelEconVsSpeed
#ifdef useEEPROMtripStorage
	"EEPROMcurrentIdx\r"
	"EEPROMtankIdx\r"
#ifdef trackIdleEOCdata
	"EEPROMeocIdleCurrentIdx\r"
	"EEPROMeocIdleTankIdx\r"
#endif // trackIdleEOCdata
#endif // useEEPROMtripStorage
};

#endif // useDebugTerminal
const uint8_t tripUpdateSrcList[] PROGMEM = {
#ifdef trackIdleEOCdata
	 0x7E								// transfer old raw idle trip to idle instant trip
	,0x7E								// update idle tank trip with old raw idle trip
	,0x7E								// update idle current trip with old raw idle trip
	,0x7E								// update old raw trip with old raw idle trip
	,0x7F								// update tank trip with old raw trip
#else // trackIdleEOCdata
	 0x7F								// update tank trip with old raw trip
#endif // trackIdleEOCdata
	,0x7F								// update current trip with old raw trip
#ifdef useWindowTripFilter
	,0x7F								// transfer old raw trip to current window trip
	,windowTripFilterIdx				// transfer window trip 1 to instant trip
	,windowTripFilterIdx + 1			// update instant trip with window trip 2
	,windowTripFilterIdx + 2			// update instant trip with window trip 3
	,windowTripFilterIdx + 3			// update instant trip with window trip 4
#else // useWindowTripFilter
	,0x7F								// transfer old raw trip to instant trip
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,0x7F								// update (or transfer) fuel econ vs time bargraph trip with old raw trip
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,0x7F								// update fuel econ vs speed bargraph trip with old raw trip
#endif // useBarFuelEconVsSpeed
};

const uint8_t tripUpdateDestList[] PROGMEM = {
#ifdef trackIdleEOCdata
	 eocIdleInstantIdx | 0x80			// transfer old raw idle trip to idle instant trip
	,eocIdleTankIdx 					// update idle tank trip with old raw idle trip
	,eocIdleCurrentIdx 					// update idle current trip with old raw idle trip
	,0x7F								// update old raw trip with old raw idle trip
	,tankIdx							// update tank trip with old raw trip
#else // trackIdleEOCdata
	 tankIdx							// update tank trip with old raw trip
#endif // trackIdleEOCdata
	,currentIdx							// update current trip with old raw trip
#ifdef useWindowTripFilter
	,0x7D | 0x80						// transfer old raw trip to current window trip
	,instantIdx | 0x80					// transfer window trip 1 to instant trip
	,instantIdx							// update instant trip with window trip 2
	,instantIdx							// update instant trip with window trip 3
	,instantIdx							// update instant trip with window trip 4
#else // useWindowTripFilter
	,instantIdx | 0x80					// transfer old raw trip to instant trip
#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
	,0x7C	 							// update (or transfer) fuel econ vs time bargraph trip with old raw trip
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,0x7B								// update fuel econ vs speed bargraph trip with old raw trip
#endif // useBarFuelEconVsSpeed
};

const uint8_t tUScount = (sizeof(tripUpdateSrcList) / sizeof(uint8_t));

const uint8_t tripSelectList[] PROGMEM = {
	 currentIdx
	,tankIdx
#ifdef trackIdleEOCdata
	,eocIdleCurrentIdx
	,eocIdleTankIdx
#endif // trackIdleEOCdata
};

#ifdef useSavedTrips
uint8_t tripSlot;

const uint8_t tripSignatureList[] PROGMEM = {
	 pCurrTripSignatureIdx
	,pTankTripSignatureIdx
};

const char tripSaveFuncNames[] PROGMEM = {
	"Save \0"
	"Load \0"
	"Reset \0"
};

#endif // useSavedTrips
