#if defined(__AVR_ATmega32U4__)
#ifndef PRTIM4
#define PRTIM4 4
#endif // PRTIM4
#endif // defined(__AVR_ATmega32U4__)
namespace heart /* core MPGuino system support section prototype */
{

	static void initCore(void);
	static void initHardware(void);
	static void updateVSS(void);
	static void sleepModeIdle(uint8_t bmsk);
#ifdef useDeepSleep
	static void doGoDeepSleep(void);
#endif // useDeepSleep
#if defined(useCPUreading) || defined(useDebugCPUreading)
	static uint32_t getCycle0Length(uint8_t lastCycleIdx);
	static uint32_t cycles0(void);
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
	static void wait0(uint16_t ms);
	static void changeBitFlagBits(uint8_t bitFlagIdx, uint8_t maskAND, uint8_t maskOR);
	static void performSleepMode(uint8_t sleepMode);
#if defined(useTimer1Interrupt)
	static void enableTimer1Interrupt(void);
	static void disableTimer1Interrupt(void);
#endif // defined(useTimer1Interrupt)

};

#if defined(__AVR__) && defined(__AVR_3_BYTE_PC__)
typedef uint_farptr_t s64prgm_ptr_t;
#define S64_PRGM_PTR(prgm) ((s64prgm_ptr_t)pgm_get_far_address(prgm))
#else // defined(__AVR__) && defined(__AVR_3_BYTE_PC__)
typedef const uint8_t * s64prgm_ptr_t;
#define S64_PRGM_PTR(prgm) (prgm)
#endif // defined(__AVR__) && defined(__AVR_3_BYTE_PC__)

typedef struct
{

    s64prgm_ptr_t ptr;
#if defined(useSWEET64RAMprograms)
	uint8_t * ram_ptr;
    uint8_t source;
#endif // defined(useSWEET64RAMprograms)

} s64pc_t;

#if defined(useSWEET64RAMprograms)
static const uint8_t s64srcProgmem = 0;
static const uint8_t s64srcRAM = 1;

#endif // defined(useSWEET64RAMprograms)
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
	static uint8_t testBuffer(uint8_t ringBufferIdx, uint8_t testFlag);
	static uint8_t testBufferNot(uint8_t ringBufferIdx, uint8_t testFlag);
	static uint16_t lengthMain(uint8_t ringBufferIdx);
	static uint16_t freeMain(uint8_t ringBufferIdx);
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

#if defined(useBufferedSerial0Port)
static const uint16_t serial0DataSize = 64;
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
static const uint16_t serial0InputDataSize = 64;
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
static const uint16_t serial1DataSize = 64;
#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
static const uint16_t serial1InputDataSize = 64;
#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial2Port)
static const uint16_t serial2DataSize = 64;
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
static const uint16_t serial2InputDataSize = 64;
#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial3Port)
static const uint16_t serial3DataSize = 64;
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
static const uint16_t USBoutputDataSize = 128;
static const uint16_t USBinputDataSize = 128;
#endif // defined(__AVR_ATmega32U4__)

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

enum {
#if defined(useBufferedSerial0Port)
	rbIdxSerial0Out,			// serial0Buffer
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
	rbIdxSerial0In,				// serial0InputBuffer
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
	rbIdxSerial1Out,			// serial1Buffer
#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
	rbIdxSerial1In,				// serial1InputBuffer
#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial2Port)
	rbIdxSerial2Out,			// serial2Buffer
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
	rbIdxSerial2In,				// serial2InputBuffer
#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial3Port)
	rbIdxSerial3Out,			// serial3Buffer
#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
	rbIdxSerial3In,				// serial3InputBuffer
#endif // defined(useSerial3PortInput)
#if defined(useBluetoothAdaFruitSPI)
	rbIdxBLEfriendOut,			// btSPIoutputBuffer
	rbIdxBLEfriendIn,			// btSPIinputBuffer
	rbIdxBluetoothIn,			// btInputBuffer
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugTerminal)
	rbIdxTerminal,				// terminalBuffer
#endif // defined(useDebugTerminal)
#if defined(__AVR_ATmega32U4__)
	rbIdxUSBout,				// USBoutputBuffer
	rbIdxUSBin,					// USBinputBuffer
#endif // defined(__AVR_ATmega32U4__)
	rbIdxCount
};

static FIFO_t ringBufferDef[(uint16_t)(rbIdxCount)];

static const FIFO_storage_t ringBufferDefList[(uint16_t)(rbIdxCount)] PROGMEM = {
#if defined(useBufferedSerial0Port)
	{serial0Data,		serial0DataSize},
#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
	{serial0InputData,	serial0InputDataSize},
#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial1Port)
	{serial1Data,		serial1DataSize},
#endif // defined(useBufferedSerial1Port)
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
#if defined(useBarGraph) || defined(useFEvTdata) || defined(useBarFuelEconVsSpeed)
static const uint8_t bgDataSize = 15; // bar-graph dataset dimension; needed by the FEvT/FEvsSpeed trip data even when the bar-graph display is absent

#endif // defined(useBarGraph) || defined(useFEvTdata) || defined(useBarFuelEconVsSpeed)
typedef struct
{

  void (* chrOut)(uint8_t character);
  uint8_t (* chrIn)(void);

} device_t;

// bit flags for use with m8Dev*Idx
static const uint8_t odvFlagCRLF =				0b10000000;
static const uint8_t odvFlagDoubleHeight =		0b01000000;
static const uint8_t odvFlagEnableOutput =		0b00100000;

union union_16
{

  uint16_t u16;
  uint8_t u08[2];

};

union union_32
{

  uint32_t u32;
  uint16_t u16[2];
  uint8_t u08[4];

};

union union_64
{

  uint64_t u64;
  uint32_t u32[2];
  uint16_t u16[4];
  uint8_t u08[8];

};

static const uint8_t samplesPerSecond = 2; // how many times will we sample in a second (also for sampling non-critical ADC channels, if configured)
static const uint8_t updatesPerSecond = 2; // how many times will we update the primary user display in a second
static const uint8_t buttonSamplesPerSecond = 32; // how many times will we try to sample ADC/TWI button presses in a second

static const uint32_t t0CyclesPerSecond =	(uint32_t)(F_CPU / 64ul);
static const uint32_t t0TicksPerSecond =	(uint32_t)(t0CyclesPerSecond / 256ul);
static const uint32_t t0TicksPerDay =		(uint32_t)(86400ul * t0TicksPerSecond);

static const uint16_t delay0TickSampleLoop =	(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * samplesPerSecond)) - 1;
static const uint16_t delay0TickUpdateLoop =	(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * updatesPerSecond)) - 1;
static const uint16_t delay0TickButtonSample =	(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * buttonSamplesPerSecond)) - 1;

static const uint16_t delay0Tick3000ms =		(uint16_t)(ceil)((double)(  3ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 3 second delay
static const uint16_t delay0Tick2000ms =		(uint16_t)(ceil)((double)(  2ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 2 second delay
static const uint16_t delay0Tick1600ms =		(uint16_t)(ceil)((double)( 16ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.6 second delay
static const uint16_t delay0Tick1500ms =		(uint16_t)(ceil)((double)( 15ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.5 second delay
static const uint16_t delay0Tick1333ms =		(uint16_t)(ceil)((double)(  4ul * t0CyclesPerSecond) / (double)(256ul * 3ul)) - 1; // 1.3333 second delay
static const uint16_t delay0Tick1000ms =		(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul)); // 1 second delay
static const uint16_t delay0Tick500ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 2ul)) - 1; // display cursor toggle period is 500 milliseconds
static const uint16_t delay0Tick200ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 5ul)) - 1; // 200 millisecond delay
static const uint16_t delay0Tick100ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 100 millisecond delay
static const uint16_t delay0Tick50ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 20ul)) - 1; // 50 millisecond delay button debounce
static const uint16_t delay0Tick30ms =			(uint16_t)(ceil)((double)( 30ul * t0CyclesPerSecond) / (double)(256ul * 1000ul)) - 1; // 30 millisecond delay
static const uint16_t delay0Tick20ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 50ul)) - 1; // 20 millisecond delay
static const uint16_t delay0Tick5ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 200ul)) - 1; // 5 millisecond delay
static const uint16_t delay0Tick2ms =			(uint16_t)(ceil)((double)(        t0CyclesPerSecond) / (double)(256ul * 500ul)); // 2 millisecond delay

#if defined(useTimer1Interrupt)
static const uint32_t t1CyclesPerSecond =		(uint32_t)(F_CPU);
static const uint32_t t1TicksPerSecond =		(uint32_t)(t1CyclesPerSecond / 510ul);

static const uint16_t delay1Tick1s =			(uint16_t)(ceil)((double)(        t1CyclesPerSecond) / (double)(510ul)) - 1; // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick250ms =			(uint16_t)(ceil)((double)(        t1CyclesPerSecond) / (double)(510ul * 4ul)) - 1; // BLEfriend packet receive timeout
static const uint16_t delay1Tick15200us =		(uint16_t)(ceil)((double)(152ul * t1CyclesPerSecond) / (double)(510ul * 10000ul)); // initial LCD delay for 4-bit initialization
static const uint16_t delay1Tick10ms =			(uint16_t)(ceil)((double)(        t1CyclesPerSecond) / (double)(510ul * 100ul)) - 1; // BLEfriend reset delay
static const uint16_t delay1Tick4100us =		(uint16_t)(ceil)((double)( 41ul * t1CyclesPerSecond) / (double)(510ul * 10000ul)); // secondary LCD delay for 4-bit initialization
static const uint16_t delay1Tick100us =			(uint16_t)(ceil)((double)(        t1CyclesPerSecond) / (double)(510ul * 10000ul)); // final LCD delay for 4-bit initialization
static const uint16_t delay1Tick75us =			(uint16_t)(ceil)((double)( 75ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)) - 1; // BLEfriend /CS assert delay
static const uint16_t delay1Tick40us =			(uint16_t)(ceil)((double)( 40ul * t1CyclesPerSecond) / (double)(510ul * 1000000ul)); // normal LCD character transmission delay

#endif // defined(useTimer1Interrupt)
enum {
	// 8-bit volatile variable array index values - these may be referenced inside an interrupt service routine
	v8VariableStartIdx = 0,					// start of 8-bit volatile variable storage
	v8Timer0CommandIdx = v8VariableStartIdx,	// timer0 command flags
	v8Timer0Status0Idx,						// timer0 status flags
	v8Timer0Status1Idx,						// more timer0 status flags
	v8DirtyInjectorIdx,						// MPGuino FI dirty (processing) flags
	v8DirtyVSSIdx,							// MPGuino VSS dirty (processing) flags
	v8AwakeIdx,								// MPGuino awake flags
	v8ActivityIdx,							// MPGuino activity flags
	v8ActivityChangeIdx,					// MPGuino activity change detection flags
	v8HeartbeatBitmaskIdx,					// Sample loop-driven heartbeat bitmask
	v8VSSdebounceTickIdx,					// MPGuino VSS sample timeout initial setting
	v8VSSsampleCountIdx,					// MPGuino VSS sample timeout counter
	v8InjectorSampleCountIdx,				// MPGuino fuel injector sample timeout counter
#if defined(useTWI4BitLCD)
	v8LCDportByteIdx,						// LCD port register expander byte
#if defined(useAdafruitRGBLCDdisplay)
	v8SwitchPortByteIdx,					// contains two out of the three LCD backlighting LED pins
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useTWI4BitLCD)
#if defined(useButtonInput)
	v8ButtonStatusIdx,
	v8ThisButtonStateIdx,
	v8LastButtonStateIdx,
	v8ButtonPressIdx,
#endif // defined(useButtonInput)
#if defined(useAnalogRead)
	v8AnalogCommandIdx,						// analog command flags
	v8AnalogStatusIdx,						// analog status flags
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
	v8TWIstatusIdx,							// TWI status flags
	v8TWIerrorIdx,							// TWI error flags
#endif // defined(useHardwareTWI)
#if defined(useTimer1Interrupt)
	v8Timer1CommandIdx,						// timer1 command flags
#if defined(useSimulatedFIandVSS)
	v8SignalSimModeIdx,						// simulated FI/VSS mode flags
	v8SignalSimVSSidx,						// simulated VSS value index
	v8SignalSimFIPidx,						// simulated fuel injector value index
	v8SignalSimVSSstate,					// simulated VSS state
	v8SignalSimFIPstate,					// simulated fuel injector state
#endif // defined(useSimulatedFIandVSS)
#endif // defined(useTimer1Interrupt)
	// Bluetooth status variables are kept in their original order so the debug-terminal label list stays aligned.
#if defined(useBluetooth)
	v8btOutputStatusIdx,					// bluetooth output status flag
#endif // defined(useBluetooth)
#if defined(useBluetoothAdaFruitSPI)
	v8BLEstatusIdx,							// AdaFruit BLEfriend condition flags
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDragRaceFunction)
	v8AccelerationFlagsIdx,					// acceleration test mode flags
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	v8CoastdownStatusIdx,					// coastdown status flags
#endif // defined(useCoastDownCalculator)
#if defined(useSerial0Port)
	v8Serial0StatusIdx,
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	v8Serial1StatusIdx,
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	v8Serial2StatusIdx,
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	v8Serial3StatusIdx,
#endif // defined(useSerial3Port)
#if defined(useFEvTdata)
	v8FEvTimeTripIdx,
#endif // defined(useFEvTdata)
#if defined(useJSONoutput)
	v8Subtitle1Idx,
#if defined(useDragRaceFunction)
	v8Subtitle2Idx,
#endif // defined(useDragRaceFunction)
#endif // defined(useJSONoutput)
#if defined(useDS1307clock)
	v8RTCsecondIdx,
	v8RTCminuteIdx,
	v8RTChourIdx,
	v8RTCdayOfWeekIdx,
	v8RTCdayIdx,
	v8RTCmonthIdx,
	v8RTCyearIdx,
	v8RTCcontrolIdx,
#endif // defined(useDS1307clock)
	v8VariableEndIdx,						// end of 8-bit volatile variable storage
	v8VariableLength = v8VariableEndIdx - v8VariableStartIdx,

	// 8-bit main program variable array index values - these should NEVER be referenced inside an interrupt service routine
	m8VariableStartIdx = v8VariableEndIdx,	// start of 8-bit main program variable storage
	m8MetricModeFlags = m8VariableStartIdx,	// metric mode flags
	m8EEPROMchangeStatus,					// EEPROM change status flags
	m8DevStartIdx,
#if defined(useLCDoutput)
	m8DevLCDidx = m8DevStartIdx,
	m8AfterDevLCDIdx,
#else // defined(useLCDoutput)
	m8AfterDevLCDIdx = m8DevStartIdx,
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
	m8DevTFTidx = m8AfterDevLCDIdx,
	m8AfterDevTFTIdx,
#else // defined(useTFToutput)
	m8AfterDevTFTIdx = m8AfterDevLCDIdx,
#endif // defined(useTFToutput)
#if defined(useSerial0Port)
	m8DevSerial0idx = m8AfterDevTFTIdx,
	m8AfterDevSerial0Idx,
#else // defined(useSerial0Port)
	m8AfterDevSerial0Idx = m8AfterDevTFTIdx,
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	m8DevSerial1idx = m8AfterDevSerial0Idx,
	m8AfterDevSerial1Idx,
#else // defined(useSerial1Port)
	m8AfterDevSerial1Idx = m8AfterDevSerial0Idx,
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	m8DevSerial2idx = m8AfterDevSerial1Idx,
	m8AfterDevSerial2Idx,
#else // defined(useSerial2Port)
	m8AfterDevSerial2Idx = m8AfterDevSerial1Idx,
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	m8DevSerial3idx = m8AfterDevSerial2Idx,
	m8AfterDevSerial3Idx,
#else // defined(useSerial3Port)
	m8AfterDevSerial3Idx = m8AfterDevSerial2Idx,
#endif // defined(useSerial3Port)
#if defined(useBluetoothAdaFruitSPI)
	m8DevBLEfriendIdx = m8AfterDevSerial3Idx,
	m8DevBluetoothIdx,
	m8AfterDevBluetoothIdx,
#else // defined(useBluetoothAdaFruitSPI)
	m8AfterDevBluetoothIdx = m8AfterDevSerial3Idx,
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(__AVR_ATmega32U4__)
	m8DevUSBidx = m8AfterDevBluetoothIdx,
	m8AfterDevUSBIdx,
#else // defined(__AVR_ATmega32U4__)
	m8AfterDevUSBIdx = m8AfterDevBluetoothIdx,
#endif // defined(__AVR_ATmega32U4__)
	m8DevEndIdx = m8AfterDevUSBIdx,
	m8DevLength = m8DevEndIdx - m8DevStartIdx,
#if defined(useClockSupport)
	m8SecondIdx = m8DevEndIdx,
	m8MinuteIdx,
	m8HourIdx,
	m8AfterClockIdx,
#else // defined(useClockSupport)
	m8AfterClockIdx = m8DevEndIdx,
#endif // defined(useClockSupport)
#if defined(useBluetooth)
	m8btInputStateIdx = m8AfterClockIdx,		// bluetooth input status flag
	m8AfterBluetoothIdx,
#else // defined(useBluetooth)
	m8AfterBluetoothIdx = m8AfterClockIdx,
#endif // defined(useBluetooth)
#if defined(useActivityLED)
	m8ActivityStatusIdx = m8AfterBluetoothIdx,
	m8ActivityOutputIdx,
	m8AfterActivityLEDIdx,
#else // defined(useActivityLED)
	m8AfterActivityLEDIdx = m8AfterBluetoothIdx,
#endif // defined(useActivityLED)
#if defined(useWindowTripFilter)
	m8CurrentWindowTripIdx = m8AfterActivityLEDIdx,
	m8AfterWindowTripFilterIdx,
#else // defined(useWindowTripFilter)
	m8AfterWindowTripFilterIdx = m8AfterActivityLEDIdx,
#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsSpeed)
	m8FEvSpeedTripIdx = m8AfterWindowTripFilterIdx,
	m8AfterFEvSpeedIdx,
#else // defined(useBarFuelEconVsSpeed)
	m8AfterFEvSpeedIdx = m8AfterWindowTripFilterIdx,
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBarGraph)
	m8BarGraphIdx = m8AfterFEvSpeedIdx,
	m8BarGraphLastIdx = m8BarGraphIdx + bgDataSize - 1,
	m8AfterBarGraphIdx = m8BarGraphIdx + bgDataSize,
#else // defined(useBarGraph)
	m8AfterBarGraphIdx = m8AfterFEvSpeedIdx,
#endif // defined(useBarGraph)
#if defined(useMCP23017portExpander)
	m8MCP23017statusFlags = m8AfterBarGraphIdx,
	m8AfterMCP23017Idx,
#else // defined(useMCP23017portExpander)
	m8AfterMCP23017Idx = m8AfterBarGraphIdx,
#endif // defined(useMCP23017portExpander)
#if defined(useDebugTerminal)
	m8ButtonFlags = m8AfterMCP23017Idx,		// debug terminal button injection flags
	m8PeekFlags,							// debug terminal peek (echo to terminal) flags
	m8AfterDebugTerminalIdx,
#else // defined(useDebugTerminal)
	m8AfterDebugTerminalIdx = m8AfterMCP23017Idx,
#endif // defined(useDebugTerminal)
	m8VariableEndIdx = m8AfterDebugTerminalIdx,	// end of 8-bit main program variable storage
	m8VariableLength = m8VariableEndIdx - m8VariableStartIdx,

	// 16-bit volatile variable array index values - these may be referenced inside an interrupt service routine
	v16VariableStartIdx = m8VariableEndIdx,	// start of 16-bit volatile variable storage
	v16VehicleStopTimeoutIdx = v16VariableStartIdx,	// engine idle timeout value in timer0 ticks
	v16EngineOffTimeoutIdx,					// engine off coasting timeout value in timer0 ticks
	v16InputTimeoutIdx,						// button press timeout value in timer0 ticks
	v16ParkTimeoutIdx,						// vehicle park (engine off and vehicle stopped) timeout value in timer0 ticks
	v16ActivityTimeoutIdx,					// total activity (vehicle park and no button press) timeout value in timer0 ticks
	v16DetectVehicleStopIdx,				// minimum good vehicle speed in timer0 ticks
	v16DetectEngineOffIdx,					// minimum good engine speed in timer0 ticks
	v16DisplayDelayCountIdx,				// dedicated display delay timer countdown in timer0 ticks
	v16WatchdogInjectorCountIdx,			// watchdog counter for minimum good engine speed / engine off activity timeout
	v16WatchdogVSScountIdx,					// watchdog counter for minimum good vehicle speed / vehicle stopped activity timeout
#if defined(useTFToutput) && !defined(useButtonInput)
	v16ActivityRemainingIdx,				// activity timeout countdown remaining (timer0 ticks), for the TFT sleep bar
	v16AfterActivityRemainingIdx,
#else // defined(useTFToutput) && !defined(useButtonInput)
	v16AfterActivityRemainingIdx = v16WatchdogVSScountIdx + 1,
#endif // defined(useTFToutput) && !defined(useButtonInput)
#if defined(useAnalogRead)
	v16AnalogStartIdx = v16AfterActivityRemainingIdx,
	v16Analog0Idx = v16AnalogStartIdx,		// highest priority analog channel
	v16Analog1Idx,
	v16Analog2Idx,
	v16Analog3Idx,
	v16Analog4Idx,
	v16Analog5Idx,							// lowest priority analog channel
	v16AnalogGroundIdx,						// analog ground
	v16AnalogEndIdx,
	v16AnalogLength = v16AnalogEndIdx - v16AnalogStartIdx,
	v16AfterAnalogIdx = v16AnalogEndIdx,
#else // defined(useAnalogRead)
	v16AfterAnalogIdx = v16AfterActivityRemainingIdx,
#endif // defined(useAnalogRead)
#if defined(useJSONoutput)
	v16JSONsubtitleCount = v16AfterAnalogIdx,
	v16AfterJSONIdx,
#else // defined(useJSONoutput)
	v16AfterJSONIdx = v16AfterAnalogIdx,
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	v16BluetoothDelayCount = v16AfterJSONIdx,
	v16AfterBluetoothIdx,
#else // defined(useBluetooth)
	v16AfterBluetoothIdx = v16AfterJSONIdx,
#endif // defined(useBluetooth)
#if defined(useSimulatedFIandVSS)
	v16SignalSimVSStickIdx = v16AfterBluetoothIdx,
	v16SignalSimFIPtickIdx,
	v16SignalSimFIPWtickIdx,
	v16SignalSimVSScountIdx,
	v16SignalSimFIPcountIdx,
	v16SignalSimFIPWcountIdx,
	v16SignalSimVSScycleCountIdx,
	v16SignalSimFIPcycleCountIdx,
	v16SignalSimPeakPeriodIdx,
	v16SignalSimPeakActiveIdx,
	v16SignalSimPeakPeriodCountIdx,
	v16SignalSimPeakActiveCountIdx,
	v16AfterSignalSimIdx,
#else // defined(useSimulatedFIandVSS)
	v16AfterSignalSimIdx = v16AfterBluetoothIdx,
#endif // defined(useSimulatedFIandVSS)
	v16VariableEndIdx = v16AfterSignalSimIdx,	// end of 16-bit volatile variable storage
	v16VariableLength = v16VariableEndIdx - v16VariableStartIdx,

	// 32-bit volatile variable array index values - these may be referenced inside an interrupt service routine
	v32VariableStartIdx = v16VariableEndIdx,	// start of 32-bit volatile variable storage
	v32Timer0OverflowCountIdx = v32VariableStartIdx,
	v32ThisInjectorOpenCycleIdx,
	v32LastInjectorOpenCycleIdx,
	v32ThisInjectorCloseCycleIdx,
	v32LastInjectorCloseCycleIdx,
	v32EngineCycleAccumulatorIdx,
	v32EnginePeriodIdx,
	v32InjectorAccumulatorIdx,
	v32ThisVSSpulseCycleIdx,
	v32LastVSSpulseCycleIdx,
	v32VSSpulseWidth0Idx,
	v32VSSpulseWidth1Idx,
	v32VSSperiodIdx,
	v32MaximumVSSperiodIdx,					// maximum good VSS period in timer0 cycles
	v32MaximumEnginePeriodIdx,				// maximum good engine period in timer0 cycles
	v32InjectorOpenDelayIdx,					// injector settle time in timer0 cycles
	v32InjectorValidMaxWidthIdx,				// maximum valid fuel injector pulse width in timer0 cycles
#if defined(useCPUreading) || defined(useDebugCPUreading)
	v32SystemCycleIdx,						// system timer tick count
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useClockSupport)
	v32ClockCycleIdx,						// software clock tick count
#endif // defined(useClockSupport)
#if defined(useChryslerMAPCorrection)
	v32InjectorCorrectionIdx,				// Chrysler fuel injector correction value
#endif // defined(useChryslerMAPCorrection)
#if defined(useFEvTdata)
	v32FEvsTimePeriodTickIdx,				// time period for fuel economy vs time bargraph
	v32FEvsTimePeriodCountIdx,				// timer0 countdown timer for fuel economy vs time bargraph
#endif // defined(useFEvTdata)
#if defined(useDebugCPUreading)
	v32WorkingTimer0Idx,					// timer0 overflow interrupt handler stopwatch direct measurement
#if defined(useTimer1Interrupt)
	v32WorkingTimer1Idx,					// timer1 overflow interrupt handler stopwatch direct measurement
#endif // defined(useTimer1Interrupt)
	v32WorkingInjectorOpenIdx,				// injector open interrupt handler stopwatch direct measurement
	v32WorkingInjectorCloseIdx,				// injector close interrupt handler stopwatch direct measurement
	v32WorkingVSSpulseIdx,					// VSS pulse interrupt handler stopwatch direct measurement
#if defined(useAnalogRead)
	v32WorkingAnalogIdx,					// analog read interrupt handler stopwatch direct measurement
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
	v32WorkingTwoWireIdx,					// two-wire interface interrupt handler stopwatch direct measurement
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
	v32WorkingSerial0Idx,					// UART0 output interrupt handler stopwatch direct measurement
#if defined(useSerial0PortInput)
	v32WorkingSerial0InputIdx,				// UART0 input interrupt handler stopwatch direct measurement
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	v32WorkingSerial1Idx,					// UART1 output interrupt handler stopwatch direct measurement
#if defined(useSerial1PortInput)
	v32WorkingSerial1InputIdx,				// UART1 input interrupt handler stopwatch direct measurement
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	v32WorkingSerial2Idx,					// UART2 output interrupt handler stopwatch direct measurement
#if defined(useSerial2PortInput)
	v32WorkingSerial2InputIdx,				// UART2 input interrupt handler stopwatch direct measurement
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	v32WorkingSerial3Idx,					// UART3 output interrupt handler stopwatch direct measurement
#if defined(useSerial3PortInput)
	v32WorkingSerial3InputIdx,				// UART3 input interrupt handler stopwatch direct measurement
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)
#endif // defined(useDebugCPUreading)
#if defined(useDragRaceFunction)
	v32DragRawTopSpeedIdx,
	v32DragRawTrapSpeedIdx,
	v32AccelHalfPeriodIdx,
	v32AccelFullPeriodIdx,
	v32AccelDistanceValueIdx,
	v32AccelDistanceCountIdx,
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	v32CoastdownMeasurement1Idx,
	v32CoastdownMeasurement2Idx,
	v32CoastdownMeasurement3Idx,
	v32CoastdownMeasurement4Idx,
	v32CoastdownPeriodIdx,
#endif // defined(useCoastDownCalculator)
	v32VariableEndIdx,						// end of 32-bit volatile variable storage
	v32VariableLength = v32VariableEndIdx - v32VariableStartIdx,

	// 32-bit main program variable array index values - these should NEVER be referenced inside an interrupt service routine
	m32VariableStartIdx = v32VariableEndIdx,	// start of 32-bit main program variable storage
	m32CyclesPerVolumeIdx = m32VariableStartIdx,	// injector conversion factor of unit volume per timer0 cycle
	m32SpeedFactorIdx,						// (timer0 cycles * unit distance * decimal format)/(pulses * hour)
#if defined(useDragRaceFunction)
	m32DragTopSpeedIdx,
	m32DragTrapSpeedIdx,
#endif // defined(useDragRaceFunction)
#if defined(useChryslerMAPCorrection)
	m32MAPpressureIdx,
	m32BaroPressureIdx,
	m32FuelPressureIdx,
	m32InjPressureIdx,
	m32AnalogMAPfloorIdx,
	m32AnalogMAPnumerIdx,
	m32AnalogMAPdenomIdx,
#if defined(useChryslerBaroSensor)
	m32AnalogBaroFloorIdx,
	m32AnalogBaroNumerIdx,
	m32AnalogBaroDenomIdx,
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
	m32FEvsSpeedMinThresholdIdx,				// minimum speed for fuel econ vs speed bargraph
	m32FEvsSpeedQuantumIdx,					// speed quantum for each bar in fuel econ vs speed bargraph
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useCPUreading) || defined(useDebugCPUreading)
	m32AvailableRAMidx,						// amount of remaining free RAM
	m32CPUworkingLoopStartIdx,
	m32CPUworkingMainStartIdx,
	m32CPUworkingIdleStartIdx,
	m32DbgWorkingMainStartIdx,
#if defined(useDebugCPUreading)
	m32DbgWorkingS64StartIdx,
	m32DbgWorkingMathStartIdx,
#endif // defined(useDebugCPUreading)
	m32CPUworkingMainLoopIdx,
	m32CPUworkingMainProcessIdx,
	m32CPUworkingIdleProcessIdx,
#if defined(useDebugCPUreading)
	m32DbgWorkingMainDevicesIdx,
	m32DbgWorkingMainActivityIdx,
	m32DbgWorkingMainSampleIdx,
	m32DbgWorkingMainOutputIdx,
	m32DbgWorkingMainOtherIdx,
	m32DbgWorkingS64processIdx,
#endif // defined(useDebugCPUreading)
	m32CPUsampledMainLoopIdx,
	m32CPUsampledMainProcessIdx,
	m32CPUsampledIdleProcessIdx,
#if defined(useDebugCPUreading)
	m32DbgSampledMainDevicesIdx,
	m32DbgSampledMainActivityIdx,
	m32DbgSampledMainSampleIdx,
	m32DbgSampledMainOutputIdx,
	m32DbgSampledMainOtherIdx,
	m32DbgSampledS64processIdx,
	m32DbgSampledInterruptProcessIdx,
	m32DbgSampledTimer0Idx,
#if defined(useTimer1Interrupt)
	m32DbgSampledTimer1Idx,
#endif // defined(useTimer1Interrupt)
	m32DbgSampledInjectorOpenIdx,
	m32DbgSampledInjectorCloseIdx,
	m32DbgSampledVSSpulseIdx,
#if defined(useAnalogRead)
	m32DbgSampledAnalogIdx,
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
	m32DbgSampledTwoWireIdx,
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
	m32DbgSampledSerial0Idx,
#if defined(useSerial0PortInput)
	m32DbgSampledSerial0InputIdx,
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	m32DbgSampledSerial1Idx,
#if defined(useSerial1PortInput)
	m32DbgSampledSerial1InputIdx,
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	m32DbgSampledSerial2Idx,
#if defined(useSerial2PortInput)
	m32DbgSampledSerial2InputIdx,
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	m32DbgSampledSerial3Idx,
#if defined(useSerial3PortInput)
	m32DbgSampledSerial3InputIdx,
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)
	m32DebugAccS64multIdx,
	m32DebugCountS64multIdx,
	m32DebugAccS64divIdx,
	m32DebugCountS64divIdx,
#if defined(useIsqrt)
	m32DebugAccS64sqrtIdx,
	m32DebugCountS64sqrtIdx,
#endif // defined(useIsqrt)
#if defined(useDebugTerminalSWEET64)
	m32S64programCyclesIdx,					// timer0 cycles executed in the last ^T trace run
	m32S64programInstrIdx,					// instructions executed in the last ^T trace run
#endif // defined(useDebugTerminalSWEET64)
#endif // defined(useDebugCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useBluetooth)
	m32BluetoothMainValue,					// default string value after '!' read-in character
#endif // defined(useBluetooth)
	m32VariableEndIdx,						// end of 32-bit main program variable storage
	m32VariableLength = m32VariableEndIdx - m32VariableStartIdx,

	// 64-bit main program variable array index values - these should NEVER be referenced inside an interrupt service routine
	m64VariableStartIdx = m32VariableEndIdx,	// start of 64-bit main program variable storage
	m64TankSizeIdx = m64VariableStartIdx,	// tank quantity in timer0 cycles
	m64BingoTankSizeIdx,					// bingo fuel quantity in timer0 cycles
	m64VariableEndIdx,						// end of 64-bit main program variable storage
	m64VariableLength = m64VariableEndIdx - m64VariableStartIdx,
	programVariableMaxIdx = m64VariableEndIdx
};

static device_t deviceDefs[(uint16_t)(m8DevLength)];

static volatile uint8_t volatile8Variables[(uint16_t)(v8VariableLength)];
static volatile uint16_t volatile16Variables[(uint16_t)(v16VariableLength)];
static volatile uint32_t volatile32Variables[(uint16_t)(v32VariableLength)];

static uint8_t mainProgram8Variables[(uint16_t)(m8VariableLength)];
static uint32_t mainProgram32Variables[(uint16_t)(m32VariableLength)];
static uint64_t mainProgram64Variables[(uint16_t)(m64VariableLength)];

#define v08(v08Index) volatile8Variables[(uint16_t)(v08Index - v8VariableStartIdx)]
#define v16(v16Index) volatile16Variables[(uint16_t)(v16Index - v16VariableStartIdx)]
#define v32(v32Index) volatile32Variables[(uint16_t)(v32Index - v32VariableStartIdx)]

#define m08(m08Index) mainProgram8Variables[(uint16_t)(m08Index - m8VariableStartIdx)]
#define m32(m32Index) mainProgram32Variables[(uint16_t)(m32Index - m32VariableStartIdx)]

#if defined(useDebugTerminalLabels)
static const char terminalVariableLabels[] PROGMEM = {
	"v8Timer0CommandIdx" tcEOS
	"v8Timer0Status0Idx" tcEOS
	"v8Timer0Status1Idx" tcEOS
	"v8DirtyInjectorIdx" tcEOS
	"v8DirtyVSSIdx" tcEOS
	"v8AwakeIdx" tcEOS
	"v8ActivityIdx" tcEOS
	"v8ActivityChangeIdx" tcEOS
	"v8HeartbeatBitmaskIdx" tcEOS
	"v8VSSdebounceTickIdx" tcEOS
	"v8VSSsampleCountIdx" tcEOS
	"v8InjectorSampleCountIdx" tcEOS
#if defined(useTWI4BitLCD)
	"v8LCDportByteIdx" tcEOS
#if defined(useAdafruitRGBLCDdisplay)
	"v8SwitchPortByteIdx" tcEOS
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useTWI4BitLCD)
#if defined(useButtonInput)
	"v8ButtonStatusIdx" tcEOS
	"v8ThisButtonStateIdx" tcEOS
	"v8LastButtonStateIdx" tcEOS
	"v8ButtonPressIdx" tcEOS
#endif // defined(useButtonInput)
#if defined(useAnalogRead)
	"v8AnalogCommandIdx" tcEOS
	"v8AnalogStatusIdx" tcEOS
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
	"v8TWIstatusIdx" tcEOS
	"v8TWIerrorIdx" tcEOS
#endif // defined(useHardwareTWI)
#if defined(useTimer1Interrupt)
	"v8Timer1CommandIdx" tcEOS
#if defined(useSimulatedFIandVSS)
	"v8SignalSimModeIdx" tcEOS
	"v8SignalSimVSSidx" tcEOS
	"v8SignalSimFIPidx" tcEOS
	"v8SignalSimVSSstate" tcEOS
	"v8SignalSimFIPstate" tcEOS
#endif // defined(useSimulatedFIandVSS)
#if defined(useBluetooth)
	"v8btOutputStatusIdx" tcEOS
#endif // defined(useBluetooth)
#if defined(useBluetoothAdaFruitSPI)
	"v8BLEstatusIdx" tcEOS
#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useTimer1Interrupt)
#if defined(useDragRaceFunction)
	"v8AccelerationFlagsIdx" tcEOS
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"v8CoastdownStatusIdx" tcEOS
#endif // defined(useCoastDownCalculator)
#if defined(useSerial0Port)
	"v8Serial0StatusIdx" tcEOS
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	"v8Serial1StatusIdx" tcEOS
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	"v8Serial2StatusIdx" tcEOS
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	"v8Serial3StatusIdx" tcEOS
#endif // defined(useSerial3Port)
#if defined(useFEvTdata)
	"v8FEvTimeTripIdx" tcEOS
#endif // defined(useFEvTdata)
#if defined(useJSONoutput)
	"v8Subtitle1Idx" tcEOS
#if defined(useDragRaceFunction)
	"v8Subtitle2Idx" tcEOS
#endif // defined(useDragRaceFunction)
#endif // defined(useJSONoutput)
#if defined(useDS1307clock)
	"v8RTCsecondIdx" tcEOS
	"v8RTCminuteIdx" tcEOS
	"v8RTChourIdx" tcEOS
	"v8RTCdayOfWeekIdx" tcEOS
	"v8RTCdayIdx" tcEOS
	"v8RTCmonthIdx" tcEOS
	"v8RTCyearIdx" tcEOS
	"v8RTCcontrolIdx" tcEOS
#endif // defined(useDS1307clock)

	"m8MetricModeFlags" tcEOS
	"m8EEPROMchangeStatus" tcEOS
#if defined(useLCDoutput)
	"m8DevLCDidx" tcEOS
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
	"m8DevTFTidx" tcEOS
#endif // defined(useTFToutput)
#if defined(useSerial0Port)
	"m8DevSerial0idx" tcEOS
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	"m8DevSerial1idx" tcEOS
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	"m8DevSerial2idx" tcEOS
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	"m8DevSerial3idx" tcEOS
#endif // defined(useSerial3Port)
#if defined(useBluetoothAdaFruitSPI)
	"m8DevBLEfriendIdx" tcEOS
	"m8DevBluetoothIdx" tcEOS
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(__AVR_ATmega32U4__)
	"m8DevUSBidx" tcEOS
#endif // defined(__AVR_ATmega32U4__)
#if defined(useClockSupport)
	"m8SecondIdx" tcEOS
	"m8MinuteIdx" tcEOS
	"m8HourIdx" tcEOS
#endif // defined(useClockSupport)
#if defined(useBluetooth)
	"m8btInputStateIdx" tcEOS
#endif // defined(useBluetooth)
#if defined(useActivityLED)
	"m8ActivityStatusIdx" tcEOS
	"m8ActivityOutputIdx" tcEOS
#endif // defined(useActivityLED)
#if defined(useWindowTripFilter)
	"m8CurrentWindowTripIdx" tcEOS
#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsSpeed)
	"m8FEvSpeedTripIdx" tcEOS
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBarGraph)
	"m8BarGraphIdx[0x00]" tcEOS
	"m8BarGraphIdx[0x01]" tcEOS
	"m8BarGraphIdx[0x02]" tcEOS
	"m8BarGraphIdx[0x03]" tcEOS
	"m8BarGraphIdx[0x04]" tcEOS
	"m8BarGraphIdx[0x05]" tcEOS
	"m8BarGraphIdx[0x06]" tcEOS
	"m8BarGraphIdx[0x07]" tcEOS
	"m8BarGraphIdx[0x08]" tcEOS
	"m8BarGraphIdx[0x09]" tcEOS
	"m8BarGraphIdx[0x0A]" tcEOS
	"m8BarGraphIdx[0x0B]" tcEOS
	"m8BarGraphIdx[0x0C]" tcEOS
	"m8BarGraphIdx[0x0D]" tcEOS
	"m8BarGraphIdx[0x0E]" tcEOS
#endif // defined(useBarGraph)
#if defined(useMCP23017portExpander)
	"m8MCP23017statusFlags" tcEOS
#endif // defined(useMCP23017portExpander)
#if defined(useDebugTerminal)
	"m8ButtonFlags" tcEOS
	"m8PeekFlags" tcEOS
#endif // defined(useDebugTerminal)

	"v16VehicleStopTimeoutIdx" tcEOS			// timer0
	"v16EngineOffTimeoutIdx" tcEOS				// timer0
	"v16InputTimeoutIdx" tcEOS					// timer0
	"v16ParkTimeoutIdx" tcEOS					// timer0
	"v16ActivityTimeoutIdx" tcEOS				// timer0
	"v16DetectVehicleStopIdx" tcEOS				// vss
	"v16DetectEngineOffIdx" tcEOS				// fi open
	"v16DisplayDelayCountIdx" tcEOS				// timer0
	"v16WatchdogInjectorCountIdx" tcEOS			// fi / timer0
	"v16WatchdogVSScountIdx" tcEOS				// vss / timer0
#if defined(useTFToutput) && !defined(useButtonInput)
	"v16ActivityRemainingIdx" tcEOS				// timer0 / TFT sleep bar
#endif // defined(useTFToutput) && !defined(useButtonInput)
#if defined(useAnalogRead)
	"v16Analog0Idx" tcEOS						// analog
	"v16Analog1Idx" tcEOS						// analog
	"v16Analog2Idx" tcEOS						// analog
	"v16Analog3Idx" tcEOS						// analog
	"v16Analog4Idx" tcEOS						// analog
	"v16Analog5Idx" tcEOS						// analog
	"v16AnalogGroundIdx" tcEOS					// analog
#endif // defined(useAnalogRead)
#if defined(useJSONoutput)
	"v16JSONsubtitleCount" tcEOS				// timer0
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	"v16BluetoothDelayCount" tcEOS
#endif // defined(useBluetooth)
#if defined(useSimulatedFIandVSS)
	"v16SignalSimVSStickIdx" tcEOS				// timer0, timer1
	"v16SignalSimFIPtickIdx" tcEOS				// timer0, timer1
	"v16SignalSimFIPWtickIdx" tcEOS				// timer0, timer1
	"v16SignalSimVSScountIdx" tcEOS				// timer0, timer1
	"v16SignalSimFIPcountIdx" tcEOS				// timer0, timer1
	"v16SignalSimFIPWcountIdx" tcEOS			// timer0, timer1
	"v16SignalSimVSScycleCountIdx" tcEOS		// timer0
	"v16SignalSimFIPcycleCountIdx" tcEOS		// timer0
	"v16SignalSimPeakPeriodIdx" tcEOS			// timer1
	"v16SignalSimPeakActiveIdx" tcEOS			// timer1
	"v16SignalSimPeakPeriodCountIdx" tcEOS		// timer1
	"v16SignalSimPeakActiveCountIdx" tcEOS		// timer1
#endif // defined(useSimulatedFIandVSS)

	"v32Timer0OverflowCountIdx" tcEOS			// timer0, fi, vss, timing
	"v32ThisInjectorOpenCycleIdx" tcEOS			// fi
	"v32LastInjectorOpenCycleIdx" tcEOS			// fi
	"v32ThisInjectorCloseCycleIdx" tcEOS		// fi
	"v32LastInjectorCloseCycleIdx" tcEOS		// fi
	"v32EngineCycleAccumulatorIdx" tcEOS		// fi
	"v32EnginePeriodIdx" tcEOS					// timer0, fi
	"v32InjectorAccumulatorIdx" tcEOS			// timer0, fi
	"v32ThisVSSpulseCycleIdx" tcEOS				// vss
	"v32LastVSSpulseCycleIdx" tcEOS				// vss
	"v32VSSpulseWidth0Idx" tcEOS				// vss
	"v32VSSpulseWidth1Idx" tcEOS				// vss
	"v32VSSperiodIdx" tcEOS						// vss
	"v32MaximumVSSperiodIdx" tcEOS				// timer0
	"v32MaximumEnginePeriodIdx" tcEOS			// fi close
	"v32InjectorOpenDelayIdx" tcEOS				// fi close
	"v32InjectorValidMaxWidthIdx" tcEOS			// fi close
#if defined(useCPUreading)
	"v32SystemCycleIdx" tcEOS					// timer0
#endif // defined(useCPUreading)
#if defined(useClockSupport)
	"v32ClockCycleIdx" tcEOS					// timer0
#endif // defined(useClockSupport)
#if defined(useChryslerMAPCorrection)
	"v32InjectorCorrectionIdx" tcEOS			// fi close
#endif // defined(useChryslerMAPCorrection)
#if defined(useFEvTdata)
	"v32FEvsTimePeriodTickIdx" tcEOS			// timer0
	"v32FEvsTimePeriodCountIdx" tcEOS			// timer0
#endif // defined(useFEvTdata)
#if defined(useDebugCPUreading)
	"v32WorkingTimer0Idx" tcEOS					// timer0 overflow interrupt handler
#if defined(useTimer1Interrupt)
	"v32WorkingTimer1Idx" tcEOS					// timer1 overflow interrupt handler
#endif // defined(useTimer1Interrupt)
	"v32WorkingInjectorOpenIdx" tcEOS			// injector open interrupt handler
	"v32WorkingInjectorCloseIdx" tcEOS			// injector close interrupt handler
	"v32WorkingVSSpulseIdx" tcEOS				// VSS pulse interrupt handler
#if defined(useAnalogRead)
	"v32WorkingAnalogIdx" tcEOS					// analog read interrupt handler
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
	"v32WorkingTwoWireIdx" tcEOS				// two-wire interface interrupt handler
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
	"v32WorkingSerial0Idx" tcEOS				// UART0 output interrupt handler
#if defined(useSerial0PortInput)
	"v32WorkingSerial0InputIdx" tcEOS			// UART0 input interrupt handler
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	"v32WorkingSerial1Idx" tcEOS				// UART1 output interrupt handler
#if defined(useSerial1PortInput)
	"v32WorkingSerial1InputIdx" tcEOS			// UART1 input interrupt handler
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	"v32WorkingSerial2Idx" tcEOS				// UART2 output interrupt handler
#if defined(useSerial2PortInput)
	"v32WorkingSerial2InputIdx" tcEOS			// UART2 input interrupt handler
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	"v32WorkingSerial3Idx" tcEOS				// UART3 output interrupt handler
#if defined(useSerial3PortInput)
	"v32WorkingSerial3InputIdx" tcEOS			// UART3 input interrupt handler
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)

#endif // defined(useDebugCPUreading)
#if defined(useFEvTdata)
#endif // defined(useFEvTdata)
#if defined(useDragRaceFunction)
	"v32DragRawTopSpeedIdx" tcEOS				// timer0
	"v32DragRawTrapSpeedIdx" tcEOS				// timer0
	"v32AccelHalfPeriodIdx" tcEOS				// timer0
	"v32AccelFullPeriodIdx" tcEOS				// timer0
	"v32AccelDistanceValueIdx" tcEOS			// timer0
	"v32AccelDistanceCountIdx" tcEOS			// timer0
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"v32CoastdownMeasurement1Idx" tcEOS			// timer0
	"v32CoastdownMeasurement2Idx" tcEOS			// timer0
	"v32CoastdownMeasurement3Idx" tcEOS			// timer0
	"v32CoastdownMeasurement4Idx" tcEOS			// timer0
	"v32CoastdownPeriodIdx" tcEOS				// timer0
#endif // defined(useCoastDownCalculator)

	"m32CyclesPerVolumeIdx" tcEOS				// main program only
	"m32SpeedFactorIdx" tcEOS					// main program only
#if defined(useDragRaceFunction)
	"m32DragTopSpeedIdx" tcEOS					// main program only
	"m32DragTrapSpeedIdx" tcEOS					// main program only
#endif // defined(useDragRaceFunction)
#if defined(useChryslerMAPCorrection)
	"m32MAPpressureIdx" tcEOS					// main program only
	"m32BaroPressureIdx" tcEOS					// main program only
	"m32FuelPressureIdx" tcEOS					// main program only
	"m32InjPressureIdx" tcEOS					// main program only
	"m32AnalogMAPfloorIdx" tcEOS				// main program only
	"m32AnalogMAPnumerIdx" tcEOS				// main program only
	"m32AnalogMAPdenomIdx" tcEOS				// main program only
#if defined(useChryslerBaroSensor)
	"m32AnalogBaroFloorIdx" tcEOS				// main program only
	"m32AnalogBaroNumerIdx" tcEOS				// main program only
	"m32AnalogBaroDenomIdx" tcEOS				// main program only
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
	"m32FEvsSpeedMinThresholdIdx" tcEOS			// main program only
	"m32FEvsSpeedQuantumIdx" tcEOS				// main program only
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useCPUreading)
	"m32AvailableRAMidx" tcEOS					// main program only
#endif // defined(useCPUreading)
#if defined(useCPUreading) || defined(useDebugCPUreading)
	"m32CPUworkingLoopStartIdx" tcEOS
	"m32CPUworkingMainStartIdx" tcEOS
	"m32CPUworkingIdleStartIdx" tcEOS
	"m32DbgWorkingMainStartIdx" tcEOS
#if defined(useDebugCPUreading)
	"m32DbgWorkingS64StartIdx" tcEOS
	"m32DbgWorkingMathStartIdx" tcEOS
#endif // defined(useDebugCPUreading)
	"m32CPUworkingMainLoopIdx" tcEOS
	"m32CPUworkingMainProcessIdx" tcEOS
	"m32CPUworkingIdleProcessIdx" tcEOS
#if defined(useDebugCPUreading)
	"m32DbgWorkingMainDevicesIdx" tcEOS
	"m32DbgWorkingMainActivityIdx" tcEOS
	"m32DbgWorkingMainSampleIdx" tcEOS
	"m32DbgWorkingMainOutputIdx" tcEOS
	"m32DbgWorkingMainOtherIdx" tcEOS
	"m32DbgWorkingS64processIdx" tcEOS
#endif // defined(useDebugCPUreading)
	"m32CPUsampledMainLoopIdx" tcEOS
	"m32CPUsampledMainProcessIdx" tcEOS
	"m32CPUsampledIdleProcessIdx" tcEOS
#if defined(useDebugCPUreading)
	"m32DbgSampledMainDevicesIdx" tcEOS
	"m32DbgSampledMainActivityIdx" tcEOS
	"m32DbgSampledMainSampleIdx" tcEOS
	"m32DbgSampledMainOutputIdx" tcEOS
	"m32DbgSampledMainOtherIdx" tcEOS
	"m32DbgSampledS64processIdx" tcEOS
	"m32DbgSampledInterruptProcessIdx" tcEOS
	"m32DbgSampledTimer0Idx" tcEOS
#if defined(useTimer1Interrupt)
	"m32DbgSampledTimer1Idx" tcEOS
#endif // defined(useTimer1Interrupt)
	"m32DbgSampledInjectorOpenIdx" tcEOS
	"m32DbgSampledInjectorCloseIdx" tcEOS
	"m32DbgSampledVSSpulseIdx" tcEOS
#if defined(useAnalogRead)
	"m32DbgSampledAnalogIdx" tcEOS
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
	"m32DbgSampledTwoWireIdx" tcEOS
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
	"m32DbgSampledSerial0Idx" tcEOS
#if defined(useSerial0PortInput)
	"m32DbgSampledSerial0InputIdx" tcEOS
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	"m32DbgSampledSerial1Idx" tcEOS
#if defined(useSerial1PortInput)
	"m32DbgSampledSerial1InputIdx" tcEOS
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	"m32DbgSampledSerial2Idx" tcEOS
#if defined(useSerial2PortInput)
	"m32DbgSampledSerial2InputIdx" tcEOS
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	"m32DbgSampledSerial3Idx" tcEOS
#if defined(useSerial3PortInput)
	"m32DbgSampledSerial3InputIdx" tcEOS
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)
	"m32DebugAccS64multIdx" tcEOS
	"m32DebugCountS64multIdx" tcEOS
	"m32DebugAccS64divIdx" tcEOS
	"m32DebugCountS64divIdx" tcEOS
#if defined(useIsqrt)
	"m32DebugAccS64sqrtIdx" tcEOS
	"m32DebugCountS64sqrtIdx" tcEOS
#endif // defined(useIsqrt)
#if defined(useDebugTerminalSWEET64)
	"m32S64programCyclesIdx" tcEOS
	"m32S64programInstrIdx" tcEOS
#endif // defined(useDebugTerminalSWEET64)
#endif // defined(useDebugCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useBluetooth)
	"m32BluetoothMainValue" tcEOS				// main program only
#endif // defined(useBluetooth)

	"m64TankSizeIdx" tcEOS						// main program only
	"m64BingoTankSizeIdx" tcEOS					// main program only
};

#endif // defined(useDebugTerminalLabels)
// bit flags for use with m8EEPROMchangeStatus
static const uint8_t ecsDoMPGuinoInitSoftware =		0b10000000;
static const uint8_t ecsDoMPGuinoInitHardware =		0b01000000;
static const uint8_t ecsDoMetricConversion =		0b00100000;
static const uint8_t ecsChangeDisplay =				0b00010000;
static const uint8_t ecsCalculateFuelParam =		0b00001000;
static const uint8_t ecsResetBarFEvsSpeed =			0b00000100;
static const uint8_t ecsEEPROMchangeDetected =		0b00000010;

static const uint8_t ecsEEPROMchangeFlags =			(ecsDoMPGuinoInitSoftware | ecsDoMPGuinoInitHardware | ecsDoMetricConversion | ecsChangeDisplay | ecsCalculateFuelParam | ecsResetBarFEvsSpeed | ecsEEPROMchangeDetected);

// bit flags for use with m8MetricModeFlags
static const uint8_t mmDisplayAlternateFE =			0b00000010;
static const uint8_t mmDisplayMetric =				0b00000001;

static const uint8_t mmFuelEconOutputFlags =		(mmDisplayAlternateFE | mmDisplayMetric);

static volatile uint8_t lastPINxState;

// these flags specifically tell the system timer0 to do something (v8Timer0CommandIdx)
// main program sets flag, system timer0 usually acknowledges by clearing flag
static const uint8_t t0cResetTimer =				0b10000000;
static const uint8_t t0cResetInputActivityTimer =	0b01000000;
static const uint8_t t0cResetOutputTimer =			0b00100000;
static const uint8_t t0cReadRTC =					0b00010000;		// useRealTimeClockModule
static const uint8_t t0cEnableJSONoutput =			0b00001000;		// useJSONoutput

#if defined(useDebugTerminal)
static const char timer0CommandFlagMarkers[] PROGMEM = {
	" T0C" tcEOS
	"reset" tcEOS
	"input" tcEOS
	"display" tcEOS
	"rtc" tcEOS
	"json" tcEOS
	"0" tcEOS
	"0" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
// these flags specifically tell the main program to do something (v8Timer0Status0Idx)
// system timer0 sets flag, main program acknowledges by clearing flag
static const uint8_t t0saTakeSample =				0b10000000;		// tells the main program to perform trip variable sampling
static const uint8_t t0saUpdateDisplay =			0b01000000;
static const uint8_t t0saShowCursor =				0b00100000;
static const uint8_t t0saDisplayDelayInit =			0b00010000;		// this is an exception in that main program actually sets this flag
static const uint8_t t0saDisplayDelayActive =		0b00001000;
static const uint8_t t0saOutputJSON =				0b00000100;		// useJSONoutput

static const uint8_t t0saDisplayDelayFlags =		(t0saDisplayDelayInit | t0saDisplayDelayActive);

#if defined(useDebugTerminal)
static const char timer0Status0FlagMarkers[] PROGMEM = {
	" T0S0" tcEOS
	"sample" tcEOS
	"display" tcEOS
	"cursor" tcEOS
	"delayInit" tcEOS
	"delay" tcEOS
	"json" tcEOS
	"0" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
// these flags specifically tell the main program to do something (v8Timer0Status1Idx)
// system timer0 sets flag, main program acknowledges by clearing flag
static const uint8_t t0sbSampleBLEfriend =			0b10000000;		// useBluetoothAdaFruitSPI
static const uint8_t t0sbReadRTC =					0b01000000;		// useRealTimeClockModule
static const uint8_t t0sbErrorRTC =					0b00100000;		// useRealTimeClockModule
static const uint8_t t0sbResetFEvsTimeTrip =		0b00010000;		// useFEvTdata
static const uint8_t t0sbAccelTestFlag =			0b00001000;		// useDragRaceFunction
static const uint8_t t0sbCoastdownTestFlag =		0b00000100;		// useCoastDownCalculator

#if defined(useDebugTerminal)
static const char timer0Status1FlagMarkers[] PROGMEM = {
	" T0S1" tcEOS
	"ble" tcEOS
	"rtc" tcEOS
	"rtcErr" tcEOS
	"fevt" tcEOS
	"accel" tcEOS
	"coast" tcEOS
	"0" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
// these status flags inform the main program about MPGuino awake state (v8AwakeIdx)
static const uint8_t aAwakeOnInjector =				0b10000000;
static const uint8_t aAwakeOnVSS =					0b01000000;
static const uint8_t aAwakeOnInput =				0b00100000;
static const uint8_t aAwakeEngineRunning =			0b00010000;
static const uint8_t aAwakeVehicleMoving =			0b00001000;

static const uint8_t aAwake =						(aAwakeOnInjector | aAwakeOnVSS | aAwakeOnInput);
static const uint8_t aAwakeOnVehicle =				(aAwakeOnInjector | aAwakeOnVSS | aAwakeEngineRunning | aAwakeVehicleMoving);

#if defined(useDebugTerminal)
static const char awakeFlagMarkers[] PROGMEM = {
	" AW" tcEOS
	"inj" tcEOS
	"vss" tcEOS
	"input" tcEOS
	"eng" tcEOS
	"move" tcEOS
	"0" tcEOS
	"0" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
// these status flags inform the main program about MPGuino sensor activity (v8ActivityIdx, v8ActivityChangeIdx)
static const uint8_t afEngineOffFlag =				0b10000000;
static const uint8_t afVehicleStoppedFlag =			0b01000000;
static const uint8_t afUserInputFlag =				0b00100000;
static const uint8_t afParkFlag =					0b00010000;
static const uint8_t afActivityTimeoutFlag =		0b00001000;
static const uint8_t afVehicleIdleFlag =			0b00000100;
static const uint8_t afVehicleEOCflag =				0b00000010;

#if defined(useDebugTerminal)
static const char activityFlagMarkers[] PROGMEM = {
	"" tcEOS
	"engOff" tcEOS
	"stopped" tcEOS
	"input" tcEOS
	"park" tcEOS
	"timeout" tcEOS
	"idle" tcEOS
	"eoc" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
static const uint8_t afValidFlags =					(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag | afUserInputFlag | afActivityTimeoutFlag);
static const uint8_t afInputCheckFlags =			(afEngineOffFlag | afVehicleStoppedFlag | afUserInputFlag);
static const uint8_t afActivityCheckFlags =			(afEngineOffFlag | afVehicleStoppedFlag | afUserInputFlag | afParkFlag);
static const uint8_t afParkCheckFlags =				(afEngineOffFlag | afVehicleStoppedFlag | afParkFlag);
static const uint8_t afNotParkedFlags =				(afEngineOffFlag | afVehicleStoppedFlag);

// these status flags communicate instantaneous vehicle status between the sensor interrupts and the system timer0 interrupt (v8DirtyInjectorIdx)
static const uint8_t dGoodInjectorOpen =			0b10000000;
static const uint8_t dGoodInjectorClose =			0b01000000;
static const uint8_t dGoodInjectorOpenPeriod =		0b00100000;
static const uint8_t dGoodInjectorRead =			0b00010000;
static const uint8_t dInjectorReadInProgress =		0b00001000;

static const uint8_t dGoodEngineRun =				(dGoodInjectorOpen | dGoodInjectorClose | dGoodInjectorOpenPeriod | dInjectorReadInProgress | dGoodInjectorRead);

#if defined(useDebugTerminal)
static const char dirtyInjectorFlagMarkers[] PROGMEM = {
	" INJ" tcEOS
	"open" tcEOS
	"close" tcEOS
	"period" tcEOS
	"read" tcEOS
	"busy" tcEOS
	"0" tcEOS
	"0" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
// these status flags communicate instantaneous vehicle status between the sensor interrupts and the system timer0 interrupt (v8DirtyVSSIdx)
static const uint8_t dGoodVSSsignal =				0b10000000;
static const uint8_t dVSSreadInProgress =			0b01000000;
static const uint8_t dGoodVSSpulse =				0b00100000;

static const uint8_t dGoodVehicleMotion =			(dGoodVSSsignal | dVSSreadInProgress | dGoodVSSpulse);

#if defined(useDebugTerminal)
static const char dirtyVSSflagMarkers[] PROGMEM = {
	" VSS" tcEOS
	"signal" tcEOS
	"busy" tcEOS
	"pulse" tcEOS
	"0" tcEOS
	"0" tcEOS
	"0" tcEOS
	"0" tcEOS
	"0" tcEOS
};

#endif // defined(useDebugTerminal)
static const uint8_t internalOutputButton =			0b10000000;
static const uint8_t internalProcessButton =		0b01000000;
static const uint8_t internalButtonValid =			0b00100000;
static const uint8_t internalReadTWIrtc =			0b00010000;
static const uint8_t internalReadTWIbutton =		0b00001000;
static const uint8_t internalResetJSONtiming =		0b00000100;
static const uint8_t internalResetCursorTimer =		0b00000001;

// these are for use with useDebugLEDactivity
static const uint8_t LEDdebugLCD =					0b10000000;
static const uint8_t LEDdebugTWI =					0b01000000;
static const uint8_t LEDdebugSPI =					0b00100000;
static const uint8_t LEDdebugRingBuffer =			0b00010000;
static const uint8_t LEDdebugUARTin =				0b00001000;
static const uint8_t LEDdebugUARTout =				0b00000100;
static const uint8_t LEDdebugUART0 =				0b00000000;
static const uint8_t LEDdebugUART1 =				0b00000001;
static const uint8_t LEDdebugUART2 =				0b00000010;
static const uint8_t LEDdebugUART3 =				0b00000011;

#if defined(useTimer1Interrupt)
// these flags specifically tell the system timer1 to do something (v8Timer1CommandIdx)
// main program sets flag, system timer1 acknowledges by clearing flag
static const uint8_t t1cResetTimer =				0b10000000;
static const uint8_t t1cDelayLCD =					0b01000000;
static const uint8_t t1cEnableDebug =				0b00100000;

static const uint8_t t1cEnableInterrupt =			(t1cDelayLCD | t1cEnableDebug);

#if defined(useBluetoothAdaFruitSPI)
// bit flags for use with v8BLEstatusIdx
static const uint8_t bleReset =						0b10000000;
static const uint8_t bleResetting =					0b01000000;
static const uint8_t bleAssert =					0b00100000;
static const uint8_t bleAsserting =					0b00010000;
static const uint8_t blePacketWait =				0b00001000;
static const uint8_t blePacketWaiting =				0b00000100;

static const uint8_t bleResetFlags =				(bleReset | bleResetting);
static const uint8_t bleAssertFlags =				(bleAssert | bleAsserting);
static const uint8_t blePacketWaitFlags =			(blePacketWait | blePacketWaiting);
static const uint8_t bleEnableInterrupt =			(bleResetFlags | bleAssertFlags | blePacketWaitFlags);

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useTimer1Interrupt)
