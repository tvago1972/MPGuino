#if defined(__AVR_ATmega32U4__)
#ifndef PRTIM4
#define PRTIM4 4
#endif // PRTIM4
#endif // defined(__AVR_ATmega32U4__)
namespace heart /* core MPGuino system support section prototype */
{

	static void updateVSS(uint32_t thisVSStime);
	static void initCore(void);
	static void initHardware(void);
#ifdef useDeepSleep
	static void doGoDeepSleep(void);
#endif // useDeepSleep
	static uint32_t findCycle0Length(uint32_t lastCycle, uint32_t thisCycle);
	static uint32_t findCycle0Length(uint32_t lastCycle);
	static uint32_t cycles0(void);
	static void doDelay0(uint8_t delay0Channel);
	static uint8_t delay0(uint16_t ms);
	static void wait0(uint16_t ms);
	static void delayS(uint16_t ms);
	static void changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR);
	static void changeBitFlagBits(uint8_t bitFlagIdx, uint8_t maskAND, uint8_t maskOR);
	static void performSleepMode(uint8_t sleepMode);

};

#if defined(useBuffering)
typedef struct
{

	uint8_t * data;
	uint8_t status;
	uint16_t size;
	uint16_t start;
	uint16_t end;

} FIFO_t;

typedef struct
{

	uint8_t * data;
	uint16_t size;

} FIFO_storage_t;

namespace ringBuffer // ringBuffer prototype
{

	static void init(void);
	static uint8_t isBufferEmpty(uint8_t ringBufferIdx);
	static uint8_t isBufferNotEmpty(uint8_t ringBufferIdx);
	static uint8_t isBufferFull(uint8_t ringBufferIdx);
	static uint16_t lengthMain(uint8_t ringBufferIdx);
	static uint16_t freeMain(uint8_t ringBufferIdx);
	static void pushMain(uint8_t ringBufferIdx, uint8_t value);
	static uint8_t pullMain(uint8_t ringBufferIdx);
	static void flush(uint8_t ringBufferIdx);
	static void empty(uint8_t ringBufferIdx);
	static void push(uint8_t ringBufferIdx, uint8_t value);
	static uint8_t pull(uint8_t ringBufferIdx);
	static uint16_t length(uint8_t ringBufferIdx);
	static uint16_t free(uint8_t ringBufferIdx);

};

static const uint8_t bufferIsFull =		0b10000000;
static const uint8_t bufferIsEmpty =	0b01000000;

#if defined(useLCDbufferedOutput)
static const uint16_t LCDdataSize = 32;
#endif // defined(useLCDbufferedOutput)
#if defined(useBufferedSerial0Port)
static const uint16_t serial0DataSize = 32;
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
static const uint16_t serial0InputDataSize = 64;
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
static const uint16_t serial1DataSize = 32;
#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
static const uint16_t serial1InputDataSize = 64;
#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial2Port)
static const uint16_t serial2DataSize = 32;
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
static const uint16_t serial2InputDataSize = 64;
#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial3Port)
static const uint16_t serial3DataSize = 32;
#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
static const uint16_t serial3InputDataSize = 64;
#endif // defined(useSerial3PortInput)
#if defined(useBluetoothAdaFruitSPI)
static const uint16_t outputSPIdataSize = 128;
static const uint16_t inputSPIdataSize = 32;
static const uint16_t inputBTdataSize = 32;
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugTerminal)
static const uint16_t tBuffLength = 120;
#endif // defined(useDebugTerminal)
#if defined(__AVR_ATmega32U4__)
static const uint16_t USBoutputDataSize = 250;
static const uint16_t USBinputDataSize = 250;
#endif // defined(__AVR_ATmega32U4__)

#if defined(useLCDbufferedOutput)
static uint8_t LCDdata[LCDdataSize];
#endif // defined(useLCDbufferedOutput)
#if defined(useBufferedSerial0Port)
static uint8_t serial0Data[serial0DataSize];
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
static uint8_t serial0InputData[serial0InputDataSize];
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
static uint8_t serial1Data[serial1DataSize];
#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
static uint8_t serial1InputData[serial1InputDataSize];
#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial2Port)
static uint8_t serial2Data[serial2DataSize];
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
static uint8_t serial2InputData[serial2InputDataSize];
#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial3Port)
static uint8_t serial3Data[serial3DataSize];
#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
static uint8_t serial3InputData[serial3InputDataSize];
#endif // defined(useSerial3PortInput)
#if defined(useBluetoothAdaFruitSPI)
static uint8_t outputSPIdata[outputSPIdataSize];
static uint8_t inputSPIdata[inputSPIdataSize];
static uint8_t inputBTdata[inputBTdataSize];
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugTerminal)
static uint8_t terminalBuff[tBuffLength];
#endif // defined(useDebugTerminal)
#if defined(__AVR_ATmega32U4__)
static uint8_t USBoutputData[USBoutputDataSize];
static uint8_t USBinputData[USBinputDataSize];
#endif // defined(__AVR_ATmega32U4__)

#define nextAllowedValue 0
#if defined(useLCDbufferedOutput)
static const uint8_t rbIdxLCD =				nextAllowedValue;		// lcdBuffer
#define nextAllowedValue rbIdxLCD + 1;
#endif // defined(useLCDbufferedOutput)
#if defined(useBufferedSerial0Port)
static const uint8_t rbIdxSerial0Out =		nextAllowedValue;		// serial0Buffer
#define nextAllowedValue rbIdxSerial0Out + 1;
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
static const uint8_t rbIdxSerial0In =		nextAllowedValue;		// serial0InputBuffer
#define nextAllowedValue rbIdxSerial0In + 1;
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
static const uint8_t rbIdxSerial1Out =		nextAllowedValue;		// serial1Buffer
#define nextAllowedValue rbIdxSerial1Out + 1;
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial1PortInput)
static const uint8_t rbIdxSerial1In =		nextAllowedValue;		// serial1InputBuffer
#define nextAllowedValue rbIdxSerial1In + 1;
#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial2Port)
static const uint8_t rbIdxSerial2Out =		nextAllowedValue;		// serial2Buffer
#define nextAllowedValue rbIdxSerial2Out + 1;
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
static const uint8_t rbIdxSerial2In =		nextAllowedValue;		// serial2InputBuffer
#define nextAllowedValue rbIdxSerial2In + 1;
#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial3Port)
static const uint8_t rbIdxSerial3Out =		nextAllowedValue;		// serial3Buffer
#define nextAllowedValue rbIdxSerial3Out + 1;
#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
static const uint8_t rbIdxSerial3In =		nextAllowedValue;		// serial3InputBuffer
#define nextAllowedValue rbIdxSerial3In + 1;
#endif // defined(useSerial3PortInput)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t rbIdxBLEfriendOut =	nextAllowedValue;		// btSPIoutputBuffer
static const uint8_t rbIdxBLEfriendIn =		rbIdxBLEfriendOut + 1;	// btSPIinputBuffer
static const uint8_t rbIdxBluetoothIn =		rbIdxBLEfriendIn + 1;	// btInputBuffer
#define nextAllowedValue rbIdxBluetoothIn + 1;
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugTerminal)
static const uint8_t rbIdxTerminal =		nextAllowedValue;		// terminalBuffer
#define nextAllowedValue rbIdxTerminal + 1;
#endif // defined(useDebugTerminal)
#if defined(__AVR_ATmega32U4__)
static const uint8_t rbIdxUSBout =			nextAllowedValue;		// USBoutputBuffer
static const uint8_t rbIdxUSBin =			rbIdxUSBout + 1;		// USBinputBuffer
#define nextAllowedValue rbIdxUSBin + 1;
#endif defined(__AVR_ATmega32U4__)

static const uint8_t rbIdxCount =			nextAllowedValue;

static FIFO_t ringBufferDef[(uint16_t)(rbIdxCount)];

static const FIFO_storage_t ringBufferDefList[(uint16_t)(rbIdxCount)] PROGMEM = {
#if defined(useLCDbufferedOutput)
	{LCDdata,			LCDdataSize},
#endif // defined(useLCDbufferedOutput)
#if defined(useBufferedSerial0Port)
	{serial0Data,		serial0DataSize},
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
	{serial0InputData,	serial0InputDataSize},
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
	{serial1Data,		serial1DataSize},
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial1PortInput)
	{serial1InputData,	serial1InputDataSize},
#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial2Port)
	{serial2Data,		serial2DataSize},
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
	{serial2InputData,	serial2InputDataSize},
#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial3Port)
	{serial3Data,		serial3DataSize},
#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
	{serial3InputData,	serial3InputDataSize},
#endif // defined(useSerial3PortInput)
#if defined(useBluetoothAdaFruitSPI)
	{outputSPIdata,		outputSPIdataSize},
	{inputSPIdata,		inputSPIdataSize},
	{inputBTdata,		inputBTdataSize},
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugTerminal)
	{terminalBuff,		tBuffLength},
#endif // defined(useDebugTerminal)
#if defined(__AVR_ATmega32U4__)
	{USBoutputData,		USBoutputDataSize},
	{USBinputData,		USBinputDataSize},
#endif // defined(__AVR_ATmega32U4__)
};

#endif // defined(useBuffering)
#if defined(useBarGraph)
static const uint8_t bgDataSize = 15;

uint8_t bargraphData[(uint16_t)(bgDataSize)];

#endif // defined(useBarGraph)
typedef struct
{

  uint8_t controlFlags;
  void (* chrOut)(uint8_t character);
  uint8_t (* chrIn)(void);

} device_t;

// for use with controlFlags above
static const uint8_t odvFlagCRLF =				0b00100000;
static const uint8_t odvFlagFrameError =		0b00010000;
static const uint8_t odvFlagDataOverrun =		0b00001000;
static const uint8_t odvFlagParityError =		0b00000100;
static const uint8_t odvFlagDoubleHeight =		0b00000010;
static const uint8_t odvFlagEnableOutput =		0b00000001;

static const uint8_t odvErrorFlags =			odvFlagFrameError | odvFlagDataOverrun | odvFlagParityError;

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
#if defined(useAnalogButtons)
static const uint8_t analogSamplesPerSecond = 32; // how many times will we try to sample ADC button presses in a second
#endif // defined(useAnalogButtons)
#if defined(useTWIbuttons)
static const uint8_t TWIsamplesPerSecond = 32; // how many times will we try to sample TWI button presses in a second
#endif // defined(useTWIbuttons)

static const uint8_t systemProcessorSpeed = (F_CPU / 1000000ul);

static const unsigned long t0CyclesPerSecond = (unsigned long)(F_CPU / 64ul);
static const unsigned long t0TicksPerSecond = t0CyclesPerSecond / 256ul;

static const uint16_t delay0TickSampleLoop = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(loopsPerSecond * 256ul)) - 1;
#if defined(useAnalogButtons)
static const uint16_t delay0TickAnalogSample  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(analogSamplesPerSecond * 256ul)) - 1;
#endif // defined(useAnalogButtons)
#if defined(useTWIbuttons)
static const uint16_t delay0TickTWIsample  = (unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(TWIsamplesPerSecond * 256ul)) - 1;
#endif // defined(useTWIbuttons)

static const uint16_t delay0Tick3000ms =	(unsigned int)(ceil)((double)(3ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 3 second delay
static const uint16_t delay0Tick2000ms =	(unsigned int)(ceil)((double)(2ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 2 second delay
static const uint16_t delay0Tick1600ms =	(unsigned int)(ceil)((double)(16ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.6 second delay
static const uint16_t delay0Tick1500ms =	(unsigned int)(ceil)((double)(15ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.5 second delay
static const uint16_t delay0Tick1000ms =	(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul)); // 1 second delay
static const uint16_t delay0Tick500ms =		(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 2ul)) - 1; // display cursor toggle period is 500 milliseconds
static const uint16_t delay0Tick100ms =		(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 100 millisecond delay
static const uint16_t delay0Tick50ms =		(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 20ul)) - 1; // 50 millisecond delay button debounce
static const uint16_t delay0Tick30ms =		(unsigned int)(ceil)((double)(30ul * t0CyclesPerSecond) / (double)(256ul * 1000ul)) - 1; // 30 millisecond delay
static const uint16_t delay0Tick20ms =		(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 50ul)) - 1; // 20 millisecond delay
static const uint16_t delay0Tick5ms =		(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 200ul)) - 1; // 5 millisecond delay
static const uint16_t delay0Tick2ms =		(unsigned int)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 500ul)) - 1; // 2 millisecond delay

#if defined(useTimer1Interrupt)
static const uint16_t delay1Tick1s =		(uint16_t)(ceil)((double)(1000000ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick500ms =		(uint16_t)(ceil)((double)(500000ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick250ms =		(uint16_t)(ceil)((double)(250000ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick15200us =	(uint16_t)(ceil)((double)(15200ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick10ms =		(uint16_t)(ceil)((double)(10000ul * systemProcessorSpeed) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick4100us =	(uint16_t)(ceil)((double)(4100ul * systemProcessorSpeed) / (double)(510ul)) - 1; // secondary LCD delay for 4-bit initialization
static const uint16_t delay1Tick838us =		(uint16_t)(ceil)((double)(838ul * systemProcessorSpeed) / (double)(510ul)) - 1; // final LCD delay for 4-bit initialization
static const uint16_t delay1Tick100us =		(uint16_t)(ceil)((double)(100ul * systemProcessorSpeed) / (double)(510ul)) - 1; // final LCD delay for 4-bit initialization
static const uint16_t delay1Tick50us =		(uint16_t)(ceil)((double)(75ul * systemProcessorSpeed) / (double)(510ul)) - 1; // final LCD delay for 4-bit initialization
static const uint16_t delay1Tick40us =		(uint16_t)(ceil)((double)(40ul * systemProcessorSpeed) / (double)(510ul)); // normal LCD character transmission delay

#endif // defined(useTimer1Interrupt)
// volatile bit flag array index values
#define nextAllowedValue 0
static const uint8_t bfTimer0Cmd =					nextAllowedValue;				// timer0 command flags
static const uint8_t bfTimer0Sts =					bfTimer0Cmd + 1;				// timer0 status flags
#define nextAllowedValue bfTimer0Sts + 1

static const uint8_t bitFlagMaxIdx =				nextAllowedValue;

// volatile variable array index values - these may be referenced inside an interrupt service routine
#define nextAllowedValue 0
#if defined(useCPUreading)
static const uint8_t vSystemCycleIdx =				nextAllowedValue;				// system timer tick count
#define nextAllowedValue vSystemCycleIdx + 1
#endif // defined(useCPUreading)

#if defined(useSoftwareClock)
static const uint8_t vClockCycleIdx =				nextAllowedValue;				// software clock tick count
static const uint8_t vClockCycleDayLengthIdx =		vClockCycleIdx + 1;				// software clock maximum tick count for 24 hours
#define nextAllowedValue vClockCycleDayLengthIdx + 1
#endif // defined(useSoftwareClock)

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
static const uint8_t vInjectorValidMaxWidthIdx =	vInjectorOpenDelayIdx + 1;		// maximum valid fuel injector pulse width in timer0 cycles
#define nextAllowedValue vInjectorValidMaxWidthIdx + 1
#if defined(useChryslerMAPCorrection)
static const uint8_t vInjectorCorrectionIdx =		nextAllowedValue;				// Chrysler fuel injector correction value
#define nextAllowedValue vInjectorCorrectionIdx + 1
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsTime)
static const uint8_t vFEvsTimePeriodTimeoutIdx =	nextAllowedValue;				// time period for fuel economy vs time bargraph
#define nextAllowedValue vFEvsTimePeriodTimeoutIdx + 1
#endif // defined(useBarFuelEconVsTime)
#if defined(useDebugCPUreading)
static const uint8_t vInterruptAccumulatorIdx =		nextAllowedValue;				// interrupt handler stopwatch direct measurement
#define nextAllowedValue vInterruptAccumulatorIdx + 1
#endif // defined(useDebugCPUreading)

#if defined(useDragRaceFunction)
static const uint8_t vDragRawInstantSpeedIdx =		nextAllowedValue;
static const uint8_t vDragInstantSpeedIdx =			vDragRawInstantSpeedIdx + 1;
static const uint8_t vDragRawTrapSpeedIdx =			vDragInstantSpeedIdx + 1;
static const uint8_t vDragTrapSpeedIdx =			vDragRawTrapSpeedIdx + 1;
static const uint8_t vAccelHalfPeriodValueIdx =		vDragTrapSpeedIdx + 1;
static const uint8_t vAccelFullPeriodValueIdx =		vAccelHalfPeriodValueIdx + 1;
static const uint8_t vAccelDistanceValueIdx =		vAccelFullPeriodValueIdx + 1;
#define nextAllowedValue vAccelDistanceValueIdx + 1
#endif // defined(useDragRaceFunction)

#if defined(useCoastDownCalculator)
static const uint8_t vCoastdownMeasurement1Idx =	nextAllowedValue;
static const uint8_t vCoastdownMeasurement2Idx =	vCoastdownMeasurement1Idx + 1;
static const uint8_t vCoastdownMeasurement3Idx =	vCoastdownMeasurement2Idx + 1;
static const uint8_t vCoastdownMeasurement4Idx =	vCoastdownMeasurement3Idx + 1;
static const uint8_t vCoastdownPeriodIdx =			vCoastdownMeasurement4Idx + 1;
#define nextAllowedValue vCoastdownPeriodIdx + 1
#endif // defined(useCoastDownCalculator)

static const uint8_t vVariableMaxIdx =				nextAllowedValue;

// main program variable array index values - these should NEVER be referenced inside an interrupt service routine
#define nextAllowedValue 0
static const uint8_t mpCyclesPerVolumeIdx =			nextAllowedValue;					// injector conversion factor of unit volume per timer0 cycle
static const uint8_t mpTankSizeIdx =				mpCyclesPerVolumeIdx + 1;			// tank quantity in timer0 cycles
static const uint8_t mpBingoTankSizeIdx =			mpTankSizeIdx + 1;					// bingo fuel quantity in timer0 cycles
#define nextAllowedValue mpBingoTankSizeIdx + 1

#if defined(usePartialRefuel)
static const uint8_t mpPartialRefuelTankSize =		nextAllowedValue;					// partial refuel tank quantity in timer0 cycles
#define nextAllowedValue mpPartialRefuelTankSize + 1

#endif // defined(usePartialRefuel)
#if defined(useChryslerMAPCorrection)
static const uint8_t mpMAPpressureIdx =				nextAllowedValue;
static const uint8_t mpBaroPressureIdx =			mpMAPpressureIdx + 1;
static const uint8_t mpFuelPressureIdx =			mpBaroPressureIdx + 1;
static const uint8_t mpInjPressureIdx =				mpFuelPressureIdx + 1;
static const uint8_t mpAnalogMAPfloorIdx =			mpInjPressureIdx + 1;
static const uint8_t mpAnalogMAPnumerIdx =			mpAnalogMAPfloorIdx + 1;
static const uint8_t mpAnalogMAPdenomIdx =			mpAnalogMAPnumerIdx + 1;
#define nextAllowedValue mpAnalogMAPdenomIdx + 1
#if defined(useChryslerBaroSensor)
static const uint8_t mpAnalogBaroFloorIdx =			nextAllowedValue;
static const uint8_t mpAnalogBaroNumerIdx =			mpAnalogBaroFloorIdx + 1;
static const uint8_t mpAnalogBaroDenomIdx =			mpAnalogBaroNumerIdx + 1;
#define nextAllowedValue mpAnalogBaroDenomIdx + 1
#endif // defined(useChryslerBaroSensor)

#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t mpFEvsSpeedMinThresholdIdx =	nextAllowedValue;					// minimum speed for fuel econ vs speed bargraph
static const uint8_t mpFEvsSpeedQuantumIdx =		mpFEvsSpeedMinThresholdIdx + 1;		// speed quantum for each bar in fuel econ vs speed bargraph
#define nextAllowedValue mpFEvsSpeedQuantumIdx + 1

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useCPUreading)
static const uint8_t mpAvailableRAMidx =			nextAllowedValue;					// amount of remaining free RAM
#define nextAllowedValue mpAvailableRAMidx + 1

#endif // defined(useCPUreading)
#if defined(useCPUreading) || defined(useDebugCPUreading)
static const uint8_t mpMainLoopAccumulatorIdx =		nextAllowedValue;					// main loop stopwatch direct measurement
static const uint8_t mpIdleAccumulatorIdx =			mpMainLoopAccumulatorIdx + 1;		// stopwatch direct measurement of time that processor actually did jack and shit
#define nextAllowedValue mpIdleAccumulatorIdx + 1

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useDebugCPUreading)
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

#endif // defined(useDebugCPUreading)
#if defined(useBluetooth)
static const uint8_t mpBluetoothMainValue =			nextAllowedValue;					// default string value after '!' read-in character
#define nextAllowedValue mpBluetoothMainValue + 1

#endif // defined(useBluetooth)
static const uint8_t mpVariableMaxIdx =				nextAllowedValue;

#if defined(useDebugTerminalLabels)
static const char terminalVolatileVarLabels[] PROGMEM = {
#if defined(useCPUreading)
	"vSystemCycleIdx" tcEOS						// timer0
#endif // defined(useCPUreading)
#if defined(useSoftwareClock)
	"vClockCycleIdx" tcEOS						// timer0
	"vClockCycleDayLengthIdx" tcEOS				// timer0
#endif // defined(useSoftwareClock)
	"vVehicleStopTimeoutIdx" tcEOS				// timer0
	"vEngineOffTimeoutIdx" tcEOS				// timer0
	"vButtonTimeoutIdx" tcEOS					// timer0
	"vParkTimeoutIdx" tcEOS						// timer0
	"vActivityTimeoutIdx" tcEOS					// timer0
	"vDetectVehicleStopIdx" tcEOS				// vss
	"vDetectEngineOffIdx" tcEOS					// fi open
	"vMaximumVSSperiodIdx" tcEOS				// vss
	"vMaximumEnginePeriodIdx" tcEOS				// fi close
	"vInjectorOpenDelayIdx" tcEOS				// fi close
	"vInjectorValidMaxWidthIdx" tcEOS			// fi close
#if defined(useChryslerMAPCorrection)
	"vInjectorCorrectionIdx" tcEOS				// fi close
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsTime)
	"vFEvsTimePeriodTimeoutIdx" tcEOS			// timer0
#endif // defined(useBarFuelEconVsTime)
#if defined(useDebugCPUreading)
	"vInterruptAccumulatorIdx" tcEOS			// all interrupts
#endif // defined(useDebugCPUreading)
#if defined(useDragRaceFunction)
	"vDragRawInstantSpeedIdx" tcEOS				// vss
	"vDragInstantSpeedIdx" tcEOS				// vss
	"vDragRawTrapSpeedIdx" tcEOS				// vss
	"vDragTrapSpeedIdx" tcEOS					// vss
	"vAccelHalfPeriodValueIdx" tcEOS			// vss
	"vAccelFullPeriodValueIdx" tcEOS			// vss
	"vAccelDistanceValueIdx" tcEOS				// vss
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"vCoastdownPeriodIdx" tcEOS					// timer0, vss
	"vCoastdownMeasurement1Idx" tcEOS			// vss
	"vCoastdownMeasurement2Idx" tcEOS			// vss
	"vCoastdownMeasurement3Idx" tcEOS			// vss
	"vCoastdownMeasurement4Idx" tcEOS			// vss
#endif // defined(useCoastDownCalculator)
};

static const char terminalMainProgramVarLabels[] PROGMEM = {
	"mpCyclesPerVolumeIdx" tcEOS				// main program only
	"mpTankSizeIdx" tcEOS						// main program only
	"mpBingoTankSizeIdx" tcEOS					// main program only
#if defined(usePartialRefuel)
	"mpPartialRefuelTankSize" tcEOS				// main program only
#endif // defined(usePartialRefuel)
#if defined(useChryslerMAPCorrection)
	"mpMAPpressureIdx" tcEOS					// main program only
	"mpBaroPressureIdx" tcEOS					// main program only
	"mpFuelPressureIdx" tcEOS					// main program only
	"mpInjPressureIdx" tcEOS					// main program only
	"mpAnalogMAPfloorIdx" tcEOS					// main program only
	"mpAnalogMAPnumerIdx" tcEOS					// main program only
	"mpAnalogMAPdenomIdx" tcEOS					// main program only
#if defined(useChryslerBaroSensor)
	"mpAnalogBaroFloorIdx" tcEOS				// main program only
	"mpAnalogBaroNumerIdx" tcEOS				// main program only
	"mpAnalogBaroDenomIdx" tcEOS				// main program only
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
	"mpFEvsSpeedMinThresholdIdx" tcEOS			// main program only
	"mpFEvsSpeedQuantumIdx" tcEOS				// main program only
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useCPUreading)
	"mpAvailableRAMidx" tcEOS					// main program only
#endif // defined(useCPUreading)
#if defined(useCPUreading) || defined(useDebugCPUreading)
	"mpMainLoopAccumulatorIdx" tcEOS			// main program only
	"mpIdleAccumulatorIdx" tcEOS				// main program only
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useDebugCPUreading)
	"mpDebugAccMainLoopIdx" tcEOS				// main program only
	"mpDebugAccIdleIdx" tcEOS					// main program only
	"mpDebugAccIdleProcessIdx" tcEOS			// main program only
	"mpDebugAccInterruptIdx" tcEOS				// main program only
	"mpDebugAccDisplayIdx" tcEOS				// main program only
	"mpDebugAccSWEET64idx" tcEOS				// main program only
	"mpDebugAccS64multIdx" tcEOS				// main program only
	"mpDebugCountS64multIdx" tcEOS				// main program only
	"mpDebugAccS64divIdx" tcEOS					// main program only
	"mpDebugCountS64divIdx" tcEOS				// main program only
#if defined(useIsqrt)
	"mpDebugAccS64sqrtIdx" tcEOS				// main program only
	"mpDebugCountS64sqrtIdx" tcEOS				// main program only
#endif // defined(useIsqrt)
#endif // defined(useDebugCPUreading)
#if defined(useBluetooth)
	"mpBluetoothMainValue" tcEOS				// main program only
#endif // defined(useBluetooth)
};

#endif // defined(useDebugTerminalLabels)
static volatile uint8_t bitFlags[(uint16_t)(bitFlagMaxIdx)];
static volatile uint32_t volatileVariables[(uint16_t)(vVariableMaxIdx)];
static uint32_t mainProgramVariables[(uint16_t)(mpVariableMaxIdx)];

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

volatile uint8_t lastPINxState;

volatile uint8_t VSScount; // for VSS debouncing
volatile uint8_t VSSpause; // for VSS debouncing

#if defined(useButtonInput)
volatile uint8_t thisButtonState;
volatile uint8_t lastButtonState;
volatile uint8_t buttonPress;
#endif // defined(useButtonInput)
#if defined(useTestAnalogButtonIdx)
volatile uint8_t thisButtonIdx;
#endif // defined(useTestAnalogButtonIdx)

volatile uint8_t mainLoopHeartBeat;

volatile unsigned long timer0_overflow_count;

volatile uint16_t watchdogInjectorCount;
volatile uint16_t watchdogVSSCount;
#if defined(useLegacyButtons)
volatile uint16_t buttonDebounceCount;
#endif // defined(useLegacyButtons)

// these definitions provide for 8 separate delay counters based on timer0
volatile uint8_t timer0DelayFlags;
volatile uint8_t timer0DisplayDelayFlags;
volatile uint16_t timer0DelayCount[8];

// these flags specifically tell the system timer0 to do something
// main program sets flag, system timer0 acknowledges by clearing flag
volatile uint8_t timer0Command;

static const uint8_t t0cResetTimer =			0b10000000;
static const uint8_t t0cInputReceived =			0b01000000;
#if defined(useButtonInput)
static const uint8_t t0cProcessButton =			0b00100000;
#endif // defined(useButtonInput)
#if defined(useBarFuelEconVsTime)
static const uint8_t t0cResetFEvTime =			0b00010000;
#endif // defined(useBarFuelEconVsTime)
#if defined(useAnalogButtons)
static const uint8_t t0cEnableAnalogButtons =	0b00001000;
#endif // defined(useAnalogButtons)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t t0cEnableBLEsample =		0b00000100;
#endif // defined(useBluetoothAdaFruitSPI)

// these flags specifically tell the main program to do something
// system timer0 sets flag, main program acknowledges by clearing flag
volatile uint8_t timer0Status;

static const uint8_t t0sTakeSample =			0b10000000;	// tells the main program to perform trip variable sampling
static const uint8_t t0sUpdateDisplay =			0b01000000;
static const uint8_t t0sShowCursor =			0b00100000;
#if defined(useButtonInput)
static const uint8_t t0sReadButton =			0b00010000;
#endif // defined(useButtonInput)
#if defined(useDataLoggingOutput) || defined(useJSONoutput) || defined(useBluetooth)
static const uint8_t t0sOutputLogging =			0b00001000;
#endif // defined(useDataLoggingOutput) || defined(useJSONoutput) || defined(useBluetooth)
#if defined(useDragRaceFunction)
static const uint8_t t0sAccelTestFlag =			0b00000100;
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
static const uint8_t t0sCoastdownTestFlag =		0b00000010;
#endif // defined(useCoastDownCalculator)
#if defined(useJSONoutput)
static const uint8_t t0sJSONchangeSubtitle =	0b00000001;
#endif // defined(useJSONoutput)

// these status flags inform the main program about MPGuino awake state
volatile uint8_t awakeFlags;

static const uint8_t aAwakeOnInjector =			0b10000000;
static const uint8_t aAwakeOnVSS =				0b01000000;
static const uint8_t aAwakeOnInput =			0b00100000;
static const uint8_t aAwakeEngineRunning =		0b00010000;
static const uint8_t aAwakeVehicleMoving =		0b00001000;
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t aAwakeSampleBLEfriend =	0b00000100;
#endif // defined(useBluetoothAdaFruitSPI)

static const uint8_t aAwake =					(aAwakeOnInjector | aAwakeOnVSS | aAwakeOnInput);
static const uint8_t aAwakeOnVehicle =			(aAwakeOnInjector | aAwakeOnVSS);

// these status flags inform the main program about MPGuino sensor activity
volatile uint8_t activityFlags;
volatile uint8_t activityChangeFlags;

static const uint8_t afEngineOffFlag =			0b10000000;
static const uint8_t afVehicleStoppedFlag =		0b01000000;
static const uint8_t afUserInputFlag =			0b00100000;
static const uint8_t afParkFlag =				0b00010000;
static const uint8_t afActivityTimeoutFlag =	0b00001000;
static const uint8_t afSwapFEwithFCR =			0b00000100;

static const uint8_t afValidFlags =				(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag | afUserInputFlag | afActivityTimeoutFlag);
static const uint8_t afInputCheckFlags =		(afEngineOffFlag | afVehicleStoppedFlag | afUserInputFlag);
static const uint8_t afActivityCheckFlags =		(afEngineOffFlag | afVehicleStoppedFlag | afUserInputFlag | afParkFlag);
static const uint8_t afParkCheckFlags =			(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag);
static const uint8_t afNotParkedFlags =			(afEngineOffFlag | afVehicleStoppedFlag);

// these status flags communicate instantaneous vehicle status between the sensor interrupts and the system timer0 interrupt
volatile uint8_t dirty;

static const uint8_t dGoodEngineRotationOpen =		0b10000000;
static const uint8_t dGoodEngineRotationClose =		0b01000000;
static const uint8_t dInjectorReadInProgress =		0b00100000;
static const uint8_t dGoodInjectorWidth =			0b00010000;
static const uint8_t dGoodInjectorRead =			0b00001000;
static const uint8_t dGoodVSSsignal =				0b00000100;
static const uint8_t dGoodVSSRead =					0b00000010;
#if defined(useChryslerMAPCorrection)
static const uint8_t dSampleADC =					0b00000001;
#endif // defined(useChryslerMAPCorrection)

static const uint8_t dGoodEngineRotation =			(dGoodEngineRotationOpen | dGoodEngineRotationClose);
static const uint8_t dGoodEngineRun =				(dGoodEngineRotationOpen | dGoodEngineRotationClose | dInjectorReadInProgress | dGoodInjectorWidth | dGoodInjectorRead);
static const uint8_t dGoodVehicleMotion =			(dGoodVSSsignal | dGoodVSSRead);
static const uint8_t dGoodVehicleDrive =			(dGoodEngineRun | dGoodVehicleMotion);

#if defined(useButtonInput)
static const uint8_t internalOutputButton =			0b10000000;
static const uint8_t internalProcessButtonsUp =		0b01000000;

#endif // defined(useButtonInput)
#if defined(useTimer1Interrupt)
// these flags specifically tell the system timer1 to do something
// main program sets flag, system timer1 acknowledges by clearing flag
volatile uint8_t timer1Command;

static const uint8_t t1cResetTimer =			0b10000000;
#if defined(useLCDoutput)
static const uint8_t t1cDelayLCD =				0b01000000;
#endif // defined(useLCDoutput)
#if defined(useSimulatedFIandVSS)
static const uint8_t t1cEnableDebug =			0b00100000;
#endif // defined(useSimulatedFIandVSS)

// these flags specifically tell the main program to do something
// system timer1 sets flag, main program acknowledges by clearing flag
// these flags also have some internal use
volatile uint8_t timer1Status;

#if defined(useTWI4BitLCD)
static const uint8_t t1sLoopFlag =				0b10000000;
static const uint8_t t1sDoOutputTWI =			0b01000000;
#endif // defined(useTWI4BitLCD)
#if defined(useSimulatedFIandVSS)
static const uint8_t t1sDebugUpdateFIP =		0b00100000;
static const uint8_t t1sDebugUpdateVSS =		0b00010000;
#endif // defined(useSimulatedFIandVSS)

#if defined(useBluetoothAdaFruitSPI)
volatile uint8_t bleStatus;

static const uint8_t bleReset =					0b10000000;
static const uint8_t bleResetting =				0b01000000;
static const uint8_t bleAssert =				0b00100000;
static const uint8_t bleAsserting =				0b00010000;
static const uint8_t blePacketWait =			0b00001000;
static const uint8_t blePacketWaiting =			0b00000100;

static const uint8_t bleResetFlags =			(bleReset | bleResetting);
static const uint8_t bleAssertFlags =			(bleAssert | bleAsserting);
static const uint8_t blePacketWaitFlags =		(blePacketWait | blePacketWaiting);

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useTimer1Interrupt)
