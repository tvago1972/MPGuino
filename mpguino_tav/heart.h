#if defined(__AVR_ATmega32U4__)
#ifndef PRTIM4
#define PRTIM4 4

#endif // PRTIM4
#endif // defined(__AVR_ATmega32U4__)

static void updateVSS(unsigned long thisVSStime);
static void initCore(void);
static void initHardware(void);
#ifdef useDeepSleep
static void doGoDeepSleep(void);
#endif // useDeepSleep
static uint32_t findCycleLength(unsigned long lastCycle, unsigned long thisCycle);
static void delay0(uint16_t ms);
static void delayS(uint16_t ms);
static void changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR);
static void performSleepMode(uint8_t sleepMode);

#ifdef useBuffering
typedef struct
{

	volatile uint8_t * data;
	volatile uint8_t size;
	volatile uint8_t start;
	volatile uint8_t end;
	volatile uint8_t status;

} ringBufferVariable;

namespace ringBuffer // ringBuffer prototype
{

	static void init(ringBufferVariable &bfr, volatile uint8_t * storage);
	static uint8_t isBufferNotEmpty(ringBufferVariable &bfr);
	static void push(ringBufferVariable &bfr, uint8_t value);
	static void pushInterrupt(ringBufferVariable &bfr, uint8_t value);
	static uint8_t pull(ringBufferVariable &bfr);
	static void flush(ringBufferVariable &bfr);

};

const uint8_t bufferIsFull =	0b10000000;
const uint8_t bufferIsEmpty =	0b01000000;

#endif // useBuffering
#ifdef useBarGraph
static const uint8_t bgDataSize = 15;

uint8_t bargraphData[(uint16_t)(bgDataSize)];

#endif // useBarGraph
typedef void (* pFunc)(void); // type for display function pointers

typedef struct
{

  uint8_t controlFlags;
  void (* chrOut)(uint8_t character);
  uint8_t (* chrIn)(void);

} interfaceDevice;

union union_16
{

  uint16_t ui;
  uint8_t u8[2];

};

union union_32
{

  uint32_t ul;
  uint16_t ui[2];
  uint8_t u8[4];

};

union union_64
{

  uint64_t ull;
  uint32_t ul[2];
  uint16_t ui[4];
  uint8_t u8[8];

};

static const uint8_t loopsPerSecond = 2; // how many times will we try and loop in a second (also for sampling non-critical ADC channels, if configured)
#ifdef useAnalogButtons
static const uint8_t analogSamplesPerSecond = 32; // how many times will we try to sample ADC button presses in a second
#endif // useAnalogButtons
#ifdef useTWIbuttons
static const uint8_t TWIsamplesPerSecond = 32; // how many times will we try to sample TWI button presses in a second
#endif // useTWIbuttons

static const unsigned long t0CyclesPerSecond = (unsigned long)(F_CPU / 64ul); // (systemProcessorSpeed * 1000000 / (timer 0 prescaler))
static const unsigned long t0TicksPerSecond = t0CyclesPerSecond / 256ul;

static const unsigned int loopTickLength = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(loopsPerSecond * 256ul)) - 1;
static const unsigned int buttonLongPressTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul)); // 1 second delay
static const unsigned int cursorDelayTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 2ul)) - 1; // display cursor toggle period is 500 milliseconds
static const unsigned int swapFEwithFCRdelay = (unsigned int)(ceil)((double)(3ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 3 second delay
static const unsigned int holdDelay = (unsigned int)(ceil)((double)(2ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 2 second delay
static const unsigned int delay1500msTick = (unsigned int)(ceil)((double)(15ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.5 second delay
static const unsigned int delay0005msTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 200ul)) - 1; // 5 millisecond delay

#ifdef useLegacyButtons
static const unsigned int buttonDebounceTick = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 20ul)) - 1; // 50 millisecond delay button debounce
#endif // useLegacyButtons
#ifdef useAnalogButtons
static const unsigned int analogSampleTickLength  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(analogSamplesPerSecond * 256ul)) - 1;
#endif // useAnalogButtons
#ifdef useTWIbuttons
static const unsigned int TWItickLength  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(TWIsamplesPerSecond * 256ul)) - 1;
#endif // useTWIbuttons
#ifdef useJSONoutput
static const unsigned int JSONtickLength = (unsigned int)(ceil)((double)(16ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.6 second delay
#endif // useJSONoutput

// volatile variable array index values - these may be referenced inside an interrupt service routine
#define nextAllowedValue 0
#ifdef useCPUreading
static const uint8_t vSystemCycleIdx =				nextAllowedValue;				// system timer tick count
#define nextAllowedValue vSystemCycleIdx + 1
#endif // useCPUreading

#ifdef useSoftwareClock
static const uint8_t vClockCycleIdx =				nextAllowedValue;				// software clock tick count
#define nextAllowedValue vClockCycleIdx + 1
#endif // useSoftwareClock

static const uint8_t vVehicleStopTimeoutIdx =		nextAllowedValue;				// engine idle timeout value in timer0 ticks
static const uint8_t vEngineOffTimeoutIdx =			vVehicleStopTimeoutIdx + 1;		// engine off coasting timeout value in timer0 ticks
static const uint8_t vButtonTimeoutIdx =			vEngineOffTimeoutIdx + 1;		// button press timeout value in timer0 ticks
static const uint8_t vParkTimeoutIdx =				vButtonTimeoutIdx + 1;			// vehicle park (engine off and vehicle stopped) timeout value in timer0 ticks
static const uint8_t vActivityTimeoutIdx =			vParkTimeoutIdx + 1;			// total activity (vehicle park and no button press) timeout value in timer0 ticks
static const uint8_t vDetectVehicleStopIdx =		vActivityTimeoutIdx + 1;		// minimum good vehicle speed in timer0 ticks
static const uint8_t vDetectEngineOffIdx =			vDetectVehicleStopIdx + 1;		// minimum good engine speed in timer0 ticks
static const uint8_t vMaximumVSSperiodIdx =			vDetectEngineOffIdx + 1;		// maximum good VSS period in timer0 cycles
static const uint8_t vMaximumEnginePeriodIdx =		vMaximumVSSperiodIdx + 1;		// maximum good engine period in timer0 cycles
static const uint8_t vInjectorOpenDelayIdx =		vMaximumEnginePeriodIdx + 1;	// injector settle time in timer0 cycles
static const uint8_t vInjectorCloseDelayIdx =		vInjectorOpenDelayIdx + 1;		// injector closing settle time in timer0 cycles
static const uint8_t vInjectorTotalDelayIdx =		vInjectorCloseDelayIdx + 1;		// injector total settle time in timer0 cycles, for injector validity check
static const uint8_t vInjectorValidMaxWidthIdx =	vInjectorTotalDelayIdx + 1;		// maximum valid fuel injector pulse width in timer0 cycles
#define nextAllowedValue vInjectorValidMaxWidthIdx + 1
#ifdef useChryslerMAPCorrection
static const uint8_t vInjectorCorrectionIdx =		nextAllowedValue;				// Chrysler fuel injector correction value
#define nextAllowedValue vInjectorCorrectionIdx + 1
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsTime
static const uint8_t vFEvsTimePeriodTimeoutIdx =	nextAllowedValue;				// time period for fuel economy vs time bargraph
#define nextAllowedValue vFEvsTimePeriodTimeoutIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useDebugCPUreading
static const uint8_t vInterruptAccumulatorIdx =		nextAllowedValue;				// interrupt handler stopwatch direct measurement
#define nextAllowedValue vInterruptAccumulatorIdx + 1
#endif // useDebugCPUreading

#ifdef useDragRaceFunction
static const uint8_t vDragInstantSpeedIdx =			nextAllowedValue;
static const uint8_t vAccelHalfPeriodValueIdx =		vDragInstantSpeedIdx + 1;
static const uint8_t vAccelFullPeriodValueIdx =		vAccelHalfPeriodValueIdx + 1;
static const uint8_t vAccelDistanceValueIdx =		vAccelFullPeriodValueIdx + 1;
#define nextAllowedValue vAccelDistanceValueIdx + 1
#endif // useDragRaceFunction

#ifdef useCoastDownCalculator
static const uint8_t vCoastdownPeriodIdx =			nextAllowedValue;
static const uint8_t vCoastdownMeasurement1Idx =	vCoastdownPeriodIdx + 1;
static const uint8_t vCoastdownMeasurement2Idx =	vCoastdownMeasurement1Idx + 1;
static const uint8_t vCoastdownMeasurement3Idx =	vCoastdownMeasurement2Idx + 1;
static const uint8_t vCoastdownMeasurement4Idx =	vCoastdownMeasurement3Idx + 1;
#define nextAllowedValue vCoastdownMeasurement4Idx + 1
#endif // useCoastDownCalculator

#ifdef useDebugTerminal
static const uint8_t vDebugValue1Idx =				nextAllowedValue;
static const uint8_t vDebugValue2Idx =				vDebugValue1Idx + 1;
static const uint8_t vDebugValue3Idx =				vDebugValue2Idx + 1;
static const uint8_t vDebugValue4Idx =				vDebugValue3Idx + 1;
#define nextAllowedValue vDebugValue4Idx + 1
#endif // useDebugTerminal

static const uint8_t vVariableMaxIdx =				nextAllowedValue;

// main program variable array index values - these should NEVER be referenced inside an interrupt service routine
#define nextAllowedValue 0
static const uint8_t mpCyclesPerVolumeIdx =			nextAllowedValue;					// injector conversion factor of unit volume per timer0 cycle
static const uint8_t mpTankSizeIdx =				mpCyclesPerVolumeIdx + 1;			// tank quantity in timer0 cycles
static const uint8_t mpBingoTankSizeIdx =			mpTankSizeIdx + 1;					// bingo fuel quantity in timer0 cycles
#define nextAllowedValue mpBingoTankSizeIdx + 1

#ifdef usePartialRefuel
static const uint8_t mpPartialRefuelTankSize =		nextAllowedValue;					// partial refuel tank quantity in timer0 cycles
#define nextAllowedValue mpPartialRefuelTankSize + 1
#endif // usePartialRefuel

#ifdef useChryslerMAPCorrection
static const uint8_t mpMAPpressureIdx =				nextAllowedValue;
static const uint8_t mpBaroPressureIdx =			mpMAPpressureIdx + 1;
static const uint8_t mpFuelPressureIdx =			mpBaroPressureIdx + 1;
static const uint8_t mpInjPressureIdx =				mpFuelPressureIdx + 1;
static const uint8_t mpAnalogMAPfloorIdx =			mpInjPressureIdx + 1;
static const uint8_t mpAnalogMAPnumerIdx =			mpAnalogMAPfloorIdx + 1;
static const uint8_t mpAnalogMAPdenomIdx =			mpAnalogMAPnumerIdx + 1;
#define nextAllowedValue mpAnalogMAPdenomIdx + 1
#ifdef useChryslerBaroSensor
static const uint8_t mpAnalogBaroFloorIdx =			nextAllowedValue;
static const uint8_t mpAnalogBaroNumerIdx =			mpAnalogBaroFloorIdx + 1;
static const uint8_t mpAnalogBaroDenomIdx =			mpAnalogBaroNumerIdx + 1;
#define nextAllowedValue mpAnalogBaroDenomIdx + 1
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

#ifdef useBarFuelEconVsSpeed
static const uint8_t mpFEvsSpeedMinThresholdIdx =	nextAllowedValue;					// minimum speed for fuel econ vs speed bargraph
static const uint8_t mpFEvsSpeedQuantumIdx =		mpFEvsSpeedMinThresholdIdx + 1;		// speed quantum for each bar in fuel econ vs speed bargraph
#define nextAllowedValue mpFEvsSpeedQuantumIdx + 1
#endif // useBarFuelEconVsSpeed

#ifdef useCPUreading
static const uint8_t mpMainLoopAccumulatorIdx =		nextAllowedValue;					// main loop stopwatch direct measurement
static const uint8_t mpIdleAccumulatorIdx =			mpMainLoopAccumulatorIdx + 1;		// stopwatch direct measurement of time that processor actually did jack and shit
static const uint8_t mpAvailableRAMidx =			mpIdleAccumulatorIdx + 1;			// amount of remaining free RAM
#define nextAllowedValue mpAvailableRAMidx + 1

#ifdef useDebugCPUreading
static const uint8_t mpDebugAccMainLoopIdx =		nextAllowedValue;					// copy of main loop stopwatch direct measurement
static const uint8_t mpDebugAccIdleIdx =			mpDebugAccMainLoopIdx + 1;			// copy of stopwatch direct measurement of time that processor actually did jack and shit
static const uint8_t mpDebugAccIdleProcessIdx =		mpDebugAccIdleIdx + 1;				// idle process stopwatch direct measurement
static const uint8_t mpDebugAccInterruptIdx =		mpDebugAccIdleProcessIdx + 1;		// copy of interrupt handler stopwatch direct measurement
static const uint8_t mpDebugAccDisplayIdx =			mpDebugAccInterruptIdx + 1;			// main display output stopwatch direct measurement
static const uint8_t mpDebugAccSWEET64idx =			mpDebugAccDisplayIdx + 1;			// SWEET64 engine stopwatch direct measurement
static const uint8_t mpDebugAccS64multIdx =			mpDebugAccSWEET64idx + 1;			// mult64 stopwatch direct measurement
static const uint8_t mpDebugCountS64multIdx =		mpDebugAccS64multIdx + 1;			// mult64 direct measurement counter
static const uint8_t mpDebugAccS64divIdx =			mpDebugCountS64multIdx + 1;			// div64 stopwatch direct measurement
static const uint8_t mpDebugCountS64divIdx =		mpDebugAccS64divIdx + 1;			// div64 direct measurement counter
#define nextAllowedValue mpDebugCountS64divIdx + 1
#if defined(useIsqrt)
static const uint8_t mpDebugAccS64sqrtIdx =			nextAllowedValue;					// iSqrt stopwatch direct measurement
static const uint8_t mpDebugCountS64sqrtIdx =		mpDebugAccS64sqrtIdx + 1;			// iSqrt direct measurement counter
#define nextAllowedValue mpDebugCountS64sqrtIdx + 1
#endif // defined(useIsqrt)
#endif // useDebugCPUreading
#endif // useCPUreading

static const uint8_t mpVariableMaxIdx =				nextAllowedValue;

#ifdef useDebugTerminalLabels
static const char terminalVolatileVarLabels[] PROGMEM = {
#ifdef useCPUreading
	"vSystemCycleIdx" tcEOSCR					// timer0
#endif // useCPUreading
#ifdef useSoftwareClock
	"vClockCycleIdx" tcEOSCR					// timer0
#endif // useSoftwareClock
	"vVehicleStopTimeoutIdx" tcEOSCR			// timer0
	"vEngineOffTimeoutIdx" tcEOSCR				// timer0
	"vButtonTimeoutIdx" tcEOSCR					// timer0
	"vParkTimeoutIdx" tcEOSCR					// timer0
	"vActivityTimeoutIdx" tcEOSCR				// timer0
	"vDetectVehicleStopIdx" tcEOSCR				// vss
	"vDetectEngineOffIdx" tcEOSCR				// fi open
	"vMaximumVSSperiodIdx" tcEOSCR				// vss
	"vMaximumEnginePeriodIdx" tcEOSCR			// fi close
	"vInjectorOpenDelayIdx" tcEOSCR				// fi close
	"vInjectorCloseDelayIdx" tcEOSCR			// fi close
	"vInjectorTotalDelayIdx" tcEOSCR			// fi close
	"vInjectorValidMaxWidthIdx" tcEOSCR			// fi close
#ifdef useChryslerMAPCorrection
	"vInjectorCorrectionIdx" tcEOSCR			// fi close
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsTime
	"vFEvsTimePeriodTimeoutIdx" tcEOSCR			// timer0
#endif // useBarFuelEconVsTime
#ifdef useDebugCPUreading
	"vInterruptAccumulatorIdx" tcEOSCR			// all interrupts
#endif // useDebugCPUreading
#ifdef useDragRaceFunction
	"vDragInstantSpeedIdx" tcEOSCR				// vss
	"vAccelHalfPeriodValueIdx" tcEOSCR			// vss
	"vAccelFullPeriodValueIdx" tcEOSCR			// vss
	"vAccelDistanceValueIdx" tcEOSCR			// vss
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	"vCoastdownPeriodIdx" tcEOSCR				// timer0, vss
	"vCoastdownMeasurement1Idx" tcEOSCR			// vss
	"vCoastdownMeasurement2Idx" tcEOSCR			// vss
	"vCoastdownMeasurement3Idx" tcEOSCR			// vss
	"vCoastdownMeasurement4Idx" tcEOSCR			// vss
#endif // useCoastDownCalculator
//#ifdef useDebugTerminal
	"vDebugValue1Idx" tcEOSCR
	"vDebugValue2Idx" tcEOSCR
	"vDebugValue3Idx" tcEOSCR
	"vDebugValue4Idx" tcEOSCR
//#endif // useDebugTerminal
};

static const char terminalMainProgramVarLabels[] PROGMEM = {
	"mpCyclesPerVolumeIdx" tcEOSCR				// main program only
	"mpTankSizeIdx" tcEOSCR						// main program only
	"mpBingoTankSizeIdx" tcEOSCR				// main program only
#ifdef usePartialRefuel
	"mpPartialRefuelTankSize" tcEOSCR			// main program only
#endif // usePartialRefuel
#ifdef useChryslerMAPCorrection
	"mpMAPpressureIdx" tcEOSCR					// main program only
	"mpBaroPressureIdx" tcEOSCR					// main program only
	"mpFuelPressureIdx" tcEOSCR					// main program only
	"mpInjPressureIdx" tcEOSCR					// main program only
	"mpAnalogMAPfloorIdx" tcEOSCR				// main program only
	"mpAnalogMAPnumerIdx" tcEOSCR				// main program only
	"mpAnalogMAPdenomIdx" tcEOSCR				// main program only
#ifdef useChryslerBaroSensor
	"mpAnalogBaroFloorIdx" tcEOSCR				// main program only
	"mpAnalogBaroNumerIdx" tcEOSCR				// main program only
	"mpAnalogBaroDenomIdx" tcEOSCR				// main program only
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	"mpFEvsSpeedMinThresholdIdx" tcEOSCR		// main program only
	"mpFEvsSpeedQuantumIdx" tcEOSCR				// main program only
#endif // useBarFuelEconVsSpeed
#ifdef useCPUreading
	"mpMainLoopAccumulatorIdx" tcEOSCR			// main program only
	"mpIdleAccumulatorIdx" tcEOSCR				// main program only
	"mpAvailableRAMidx" tcEOSCR					// main program only
#ifdef useDebugCPUreading
	"mpDebugAccMainLoopIdx" tcEOSCR				// main program only
	"mpDebugAccIdleIdx" tcEOSCR					// main program only
	"mpDebugAccIdleProcessIdx" tcEOSCR			// main program only
	"mpDebugAccInterruptIdx" tcEOSCR			// main program only
	"mpDebugAccDisplayIdx" tcEOSCR				// main program only
	"mpDebugAccSWEET64idx" tcEOSCR				// main program only
	"mpDebugAccS64multIdx" tcEOSCR				// main program only
	"mpDebugCountS64multIdx" tcEOSCR			// main program only
	"mpDebugAccS64divIdx" tcEOSCR				// main program only
	"mpDebugCountS64divIdx" tcEOSCR				// main program only
#if defined(useIsqrt)
	"mpDebugAccS64sqrtIdx" tcEOSCR				// main program only
	"mpDebugCountS64sqrtIdx" tcEOSCR			// main program only
#endif // defined(useIsqrt)
#endif // useDebugCPUreading
#endif // useCPUreading
};

#endif // useDebugTerminalLabels
volatile uint32_t volatileVariables[(uint16_t)(vVariableMaxIdx)];
uint32_t mainProgramVariables[(uint16_t)(mpVariableMaxIdx)];

static uint8_t metricFlag;

static const uint8_t softInitGuinoFlag =		0b10000000;
static const uint8_t hardInitGuinoFlag =		0b01000000;
static const uint8_t metricConversionFlag =		0b00100000;
static const uint8_t changeDisplayFlag =		0b00010000;
static const uint8_t calculateFuelParamFlag =	0b00001000;
static const uint8_t EEPROMbulkChangeFlag =		0b00000100;
static const uint8_t alternateFEmode =			0b00000010;
static const uint8_t metricMode =				0b00000001;

static const uint8_t detectEEPROMchangeFlag =	0b11111100;
static const uint8_t fuelEconOutputFlags =		0b00000011;

static char mBuff1[17]; // used by ull2str(), translateCalcIdx(), bigDigit::, bar graph routines
static char pBuff[14]; // used by parameterEdit::, clockSet::, function result output routines

volatile uint8_t lastPINxState;

volatile uint8_t VSScount; // for VSS debouncing
volatile uint8_t VSSpause; // for VSS debouncing

volatile uint8_t thisButtonState;
volatile uint8_t lastButtonState;
#ifdef useTestAnalogButtonIdx
volatile uint8_t thisButtonIdx;
#endif // useTestAnalogButtonIdx
volatile uint8_t buttonPress;

volatile uint8_t mainLoopHeartBeat;

volatile unsigned long timer0_overflow_count;

volatile unsigned int watchdogInjectorCount;
volatile unsigned int watchdogVSSCount;
volatile unsigned int timer0DelayCount;
volatile unsigned int displayPauseCount;
#ifdef useLegacyButtons
volatile unsigned int buttonDebounceCount;
#endif // useLegacyButtons

// these flags specifically tell the system timer0 to do something
// main program sets flag, system timer0 acknowledges by clearing flag
volatile uint8_t timer0Command;

const uint8_t t0cResetTimer =			0b10000000;
const uint8_t t0cDoDelay =				0b01000000;
const uint8_t t0cDisplayDelay =			0b00100000;
const uint8_t t0cProcessButton =		0b00010000;
#ifdef useBarFuelEconVsTime
const uint8_t t0cResetFEvTime =			0b00001000;
#endif // useBarFuelEconVsTime
#ifdef useTWIbuttons
const uint8_t t0cEnableTWIsample =		0b00000001;
#endif // useTWIbuttons

// these flags specifically tell the main program to do something
// system timer0 sets flag, main program acknowledges by clearing flag
volatile uint8_t timer0Status;

const uint8_t t0sUpdateDisplay =		0b10000000;
const uint8_t t0sShowCursor =			0b01000000;
const uint8_t t0sReadButton =			0b00100000;
const uint8_t t0sTakeSample =			0b00010000;
#if useDataLoggingOutput || useJSONoutput
const uint8_t t0sOutputLogging =		0b00001000;
#endif // useDataLoggingOutput || useJSONoutput
#ifdef useDragRaceFunction
const uint8_t t0sAccelTestFlag =		0b00000100;
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
const uint8_t t0sCoastdownTestFlag =	0b00000010;
#endif // useCoastDownCalculator
#ifdef useJSONoutput
const uint8_t t0sOutputJSON =			0b00000001;
#endif // useJSONoutput

// these status flags inform the main program about MPGuino awake state
volatile uint8_t awakeFlags;

const uint8_t aAwakeOnInjector =		0b10000000;
const uint8_t aAwakeOnVSS =				0b01000000;
const uint8_t aAwakeOnButton =			0b00100000;
const uint8_t aAwakeEngineRunning =		0b00010000;
const uint8_t aAwakeVehicleMoving =		0b00001000;

const uint8_t aAwake =					(aAwakeOnInjector | aAwakeOnVSS | aAwakeOnButton);
const uint8_t aAwakeOnVehicle =			(aAwakeOnInjector | aAwakeOnVSS);

// these status flags inform the main program about MPGuino sensor activity
volatile uint8_t activityFlags;
volatile uint8_t activityChangeFlags;

const uint8_t afEngineOffFlag =			0b10000000;
const uint8_t afVehicleStoppedFlag =	0b01000000;
const uint8_t afButtonFlag =			0b00100000;
const uint8_t afParkFlag =				0b00010000;
const uint8_t afActivityTimeoutFlag =	0b00001000;
const uint8_t afSwapFEwithFCR =			0b00000100;

const uint8_t afValidFlags =			(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag | afButtonFlag | afActivityTimeoutFlag);
const uint8_t afInputCheckFlags =		(afEngineOffFlag | afVehicleStoppedFlag | afButtonFlag);
const uint8_t afActivityCheckFlags =	(afEngineOffFlag | afVehicleStoppedFlag | afButtonFlag | afParkFlag);
const uint8_t afParkCheckFlags =		(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag);
const uint8_t afNotParkedFlags =		(afEngineOffFlag | afVehicleStoppedFlag);

// these status flags communicate instantaneous vehicle status between the sensor interrupts and the system timer0 interrupt
volatile uint8_t dirty;

const uint8_t dGoodEngineRotationOpen =		0b10000000;
const uint8_t dGoodEngineRotationClose =	0b01000000;
const uint8_t dInjectorReadInProgress =		0b00100000;
const uint8_t dGoodInjectorWidth =			0b00010000;
const uint8_t dGoodInjectorRead =			0b00001000;
const uint8_t dGoodVSSsignal =				0b00000100;
const uint8_t dGoodVSSRead =				0b00000010;
#ifdef useChryslerMAPCorrection
const uint8_t dSampleADC =					0b00000001;
#endif // useChryslerMAPCorrection

const uint8_t dGoodEngineRotation =		(dGoodEngineRotationOpen | dGoodEngineRotationClose);
const uint8_t dGoodEngineRun =			(dGoodEngineRotationOpen | dGoodEngineRotationClose | dInjectorReadInProgress | dGoodInjectorWidth | dGoodInjectorRead);
const uint8_t dGoodVehicleMotion =		(dGoodVSSsignal | dGoodVSSRead);
const uint8_t dGoodVehicleDrive =		(dGoodEngineRun | dGoodVehicleMotion);

const uint8_t internalOutputButton =		0b10000000;
const uint8_t internalProcessButtonsUp =	0b01000000;
#ifdef useCoastDownCalculator
const uint8_t internalCancelCDT =			0b00000010;
#endif // useCoastDownCalculator

#ifdef useTimer1Interrupt
// these flags specifically tell the system timer1 to do something
// main program sets flag, system timer1 acknowledges by clearing flag
volatile uint8_t timer1Command;

const uint8_t t1cResetTimer =			0b10000000;
#ifdef use4BitLCD
const uint8_t t1cDelayLCD =				0b01000000;
#endif // use4BitLCD
#ifdef useSimulatedFIandVSS
const uint8_t t1cEnableDebug =			0b00100000;
#endif // useSimulatedFIandVSS

// these flags specifically tell the main program to do something
// system timer1 sets flag, main program acknowledges by clearing flag
// these flags also have some internal use
volatile uint8_t timer1Status;

#ifdef useTWILCD
const uint8_t t1sLoopFlag =				0b10000000;
const uint8_t t1sDoOutputTWI =			0b01000000;
#endif // useTWILCD
#ifdef useSimulatedFIandVSS
const uint8_t t1sDebugUpdateFIP =		0b00100000;
const uint8_t t1sDebugUpdateVSS =		0b00010000;
#endif // useSimulatedFIandVSS

#endif // useTimer1Interrupt
