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

#define nextAllowedValue 0
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
#define nextAllowedValue 0

// 8-bit volatile variable array index values - these may be referenced inside an interrupt service routine

static const uint8_t v8VariableStartIdx =				nextAllowedValue;						// start of 8-bit volatile variable storage

static const uint8_t v8Timer0CommandIdx =				nextAllowedValue;						// timer0 command flags
static const uint8_t v8Timer0Status0Idx =				v8Timer0CommandIdx + 1;					// timer0 status flags
static const uint8_t v8Timer0Status1Idx =				v8Timer0Status0Idx + 1;					// more timer0 status flags
static const uint8_t v8DirtyInjectorIdx =				v8Timer0Status1Idx + 1;					// MPGuino FI dirty (processing) flags
static const uint8_t v8DirtyVSSIdx =					v8DirtyInjectorIdx + 1;					// MPGuino VSS dirty (processing) flags
static const uint8_t v8AwakeIdx =						v8DirtyVSSIdx + 1;						// MPGuino awake flags
static const uint8_t v8ActivityIdx =					v8AwakeIdx + 1;							// MPGuino activity flags
static const uint8_t v8ActivityChangeIdx =				v8ActivityIdx + 1;						// MPGuino activity change detection flags
static const uint8_t v8HeartbeatBitmaskIdx =			v8ActivityChangeIdx + 1;				// Sample loop-driven heartbeat bitmask
static const uint8_t v8VSSdebounceTickIdx =				v8HeartbeatBitmaskIdx + 1;				// MPGuino VSS sample timeout initial setting
static const uint8_t v8VSSsampleCountIdx =				v8VSSdebounceTickIdx + 1;				// MPGuino VSS sample timeout counter
static const uint8_t v8InjectorSampleCountIdx =			v8VSSsampleCountIdx + 1;				// MPGuino fuel injector sample timeout counter
#define nextAllowedValue v8InjectorSampleCountIdx + 1
#if defined(useTWI4BitLCD)
static const uint8_t v8LCDportByteIdx =					nextAllowedValue;						// LCD port register expander byte
#define nextAllowedValue v8LCDportByteIdx + 1
#if defined(useAdafruitRGBLCDdisplay)
static const uint8_t v8SwitchPortByteIdx =				nextAllowedValue;						// contains two out of the three LCD backlighting LED pins
#define nextAllowedValue v8SwitchPortByteIdx + 1
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useTWI4BitLCD)
#if defined(useButtonInput)
static const uint8_t v8ButtonStatusIdx =				nextAllowedValue;
static const uint8_t v8ThisButtonStateIdx =				v8ButtonStatusIdx + 1;
static const uint8_t v8LastButtonStateIdx =				v8ThisButtonStateIdx + 1;
static const uint8_t v8ButtonPressIdx =					v8LastButtonStateIdx + 1;
#define nextAllowedValue v8ButtonPressIdx + 1
#endif // defined(useButtonInput)
#if defined(useAnalogRead)
static const uint8_t v8AnalogCommandIdx =				nextAllowedValue;						// analog command flags
static const uint8_t v8AnalogStatusIdx =				v8AnalogCommandIdx + 1;					// analog status flags
#define nextAllowedValue v8AnalogStatusIdx + 1
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
static const uint8_t v8TWIstatusIdx =					nextAllowedValue;						// TWI status flags
static const uint8_t v8TWIerrorIdx =					v8TWIstatusIdx + 1;						// TWI error flags
#define nextAllowedValue v8TWIerrorIdx + 1
#endif // defined(useHardwareTWI)
#if defined(useTimer1Interrupt)
static const uint8_t v8Timer1CommandIdx =				nextAllowedValue;						// timer1 command flags
#define nextAllowedValue v8Timer1CommandIdx + 1
#if defined(useSimulatedFIandVSS)
static const uint8_t v8SignalSimModeIdx =				nextAllowedValue;						// simulated FI/VSS mode flags
static const uint8_t v8SignalSimVSSidx =				v8SignalSimModeIdx + 1;					// simulated VSS value index
static const uint8_t v8SignalSimFIPidx =				v8SignalSimVSSidx + 1;					// simulated fuel injector value index
static const uint8_t v8SignalSimVSSstate =				v8SignalSimFIPidx + 1;					// simulated VSS state
static const uint8_t v8SignalSimFIPstate =				v8SignalSimVSSstate + 1;				// simulated fuel injector state
#define nextAllowedValue v8SignalSimFIPstate + 1
#endif // defined(useSimulatedFIandVSS)
#endif // defined(useTimer1Interrupt)
// the bluetooth status variables are not timer1-specific, so they are defined
// outside the useTimer1Interrupt block (bluetooth can be enabled without it).
// kept in their original order so the debug-terminal label list stays aligned.
#if defined(useBluetooth)
static const uint8_t v8btOutputStatusIdx =				nextAllowedValue;						// bluetooth output status flag
#define nextAllowedValue v8btOutputStatusIdx + 1
#endif // defined(useBluetooth)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t v8BLEstatusIdx =					nextAllowedValue;						// AdaFruit BLEfriend condition flags
#define nextAllowedValue v8BLEstatusIdx + 1
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDragRaceFunction)
static const uint8_t v8AccelerationFlagsIdx =			nextAllowedValue;						// acceleration test mode flags
#define nextAllowedValue v8AccelerationFlagsIdx + 1
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
static const uint8_t v8CoastdownStatusIdx =				nextAllowedValue;						// coastdown status flags
#define nextAllowedValue v8CoastdownStatusIdx + 1
#endif // defined(useCoastDownCalculator)
#if defined(useSerial0Port)
static const uint8_t v8Serial0StatusIdx =				nextAllowedValue;
#define nextAllowedValue v8Serial0StatusIdx + 1
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
static const uint8_t v8Serial1StatusIdx =				nextAllowedValue;
#define nextAllowedValue v8Serial1StatusIdx + 1
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
static const uint8_t v8Serial2StatusIdx =				nextAllowedValue;
#define nextAllowedValue v8Serial2StatusIdx + 1
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
static const uint8_t v8Serial3StatusIdx =				nextAllowedValue;
#define nextAllowedValue v8Serial3StatusIdx + 1
#endif // defined(useSerial3Port)
#if defined(useFEvTdata)
static const uint8_t v8FEvTimeTripIdx =					nextAllowedValue;
#define nextAllowedValue v8FEvTimeTripIdx + 1
#endif // defined(useFEvTdata)
#if defined(useJSONoutput)
static const uint8_t v8Subtitle1Idx =					nextAllowedValue;
#define nextAllowedValue v8Subtitle1Idx + 1
#if defined(useDragRaceFunction)
static const uint8_t v8Subtitle2Idx =					nextAllowedValue;
#define nextAllowedValue v8Subtitle2Idx + 1
#endif // defined(useDragRaceFunction)
#endif // defined(useJSONoutput)
#if defined(useDS1307clock)
static const uint8_t v8RTCsecondIdx =					nextAllowedValue;
static const uint8_t v8RTCminuteIdx =					v8RTCsecondIdx + 1;
static const uint8_t v8RTChourIdx =						v8RTCminuteIdx + 1;
static const uint8_t v8RTCdayOfWeekIdx =				v8RTChourIdx + 1;
static const uint8_t v8RTCdayIdx =						v8RTCdayOfWeekIdx + 1;
static const uint8_t v8RTCmonthIdx =					v8RTCdayIdx + 1;
static const uint8_t v8RTCyearIdx =						v8RTCmonthIdx + 1;
static const uint8_t v8RTCcontrolIdx =					v8RTCyearIdx + 1;
#define nextAllowedValue v8RTCcontrolIdx + 1
#endif // defined(useDS1307clock)

static const uint8_t v8VariableEndIdx =					nextAllowedValue;						// end of 8-bit volatile variable storage
static const uint8_t v8VariableLength =					v8VariableEndIdx - v8VariableStartIdx;

// 8-bit main program variable array index values - these should NEVER be referenced inside an interrupt service routine

static const uint8_t m8VariableStartIdx =				nextAllowedValue;						// start of 8-bit main program variable storage

static const uint8_t m8MetricModeFlags =				nextAllowedValue;						// metric mode flags
static const uint8_t m8EEPROMchangeStatus =				m8MetricModeFlags + 1;					// EEPROM change status flags
#define nextAllowedValue m8EEPROMchangeStatus + 1

static const uint8_t m8DevStartIdx =					nextAllowedValue;

#if defined(useLCDoutput)
static const uint8_t m8DevLCDidx =						nextAllowedValue;
#define nextAllowedValue m8DevLCDidx + 1
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
static const uint8_t m8DevTFTidx =						nextAllowedValue;
#define nextAllowedValue m8DevTFTidx + 1
#endif // defined(useTFToutput)
#if defined(useSerial0Port)
static const uint8_t m8DevSerial0idx =					nextAllowedValue;
#define nextAllowedValue m8DevSerial0idx + 1
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
static const uint8_t m8DevSerial1idx =					nextAllowedValue;
#define nextAllowedValue m8DevSerial1idx + 1
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
static const uint8_t m8DevSerial2idx =					nextAllowedValue;
#define nextAllowedValue m8DevSerial2idx + 1
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
static const uint8_t m8DevSerial3idx =					nextAllowedValue;
#define nextAllowedValue m8DevSerial3idx + 1
#endif // defined(useSerial3Port)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t m8DevBLEfriendIdx =				nextAllowedValue;
static const uint8_t m8DevBluetoothIdx =				m8DevBLEfriendIdx + 1;
#define nextAllowedValue m8DevBluetoothIdx + 1
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(__AVR_ATmega32U4__)
static const uint8_t m8DevUSBidx =						nextAllowedValue;
#define nextAllowedValue m8DevUSBidx + 1
#endif // defined(__AVR_ATmega32U4__)

static const uint8_t m8DevEndIdx =						nextAllowedValue;
static const uint8_t m8DevLength =						m8DevEndIdx - m8DevStartIdx;

#if defined(useClockSupport)
static const uint8_t m8SecondIdx =						nextAllowedValue;
static const uint8_t m8MinuteIdx =						m8SecondIdx + 1;
static const uint8_t m8HourIdx =						m8MinuteIdx + 1;
#define nextAllowedValue m8HourIdx + 1

#endif // defined(useClockSupport)
#if defined(useBluetooth)
static const uint8_t m8btInputStateIdx =				nextAllowedValue;						// bluetooth input status flag
#define nextAllowedValue m8btInputStateIdx + 1

#endif // defined(useBluetooth)
#if defined(useActivityLED)
static const uint8_t m8ActivityStatusIdx =				nextAllowedValue;
static const uint8_t m8ActivityOutputIdx =				m8ActivityStatusIdx + 1;
#define nextAllowedValue m8ActivityOutputIdx + 1

#endif // defined(useActivityLED)
#if defined(useWindowTripFilter)
static const uint8_t m8CurrentWindowTripIdx =			nextAllowedValue;
#define nextAllowedValue m8CurrentWindowTripIdx + 1

#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t m8FEvSpeedTripIdx =				nextAllowedValue;
#define nextAllowedValue m8FEvSpeedTripIdx + 1

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBarGraph)
static const uint8_t m8BarGraphIdx =					nextAllowedValue;
#define nextAllowedValue m8BarGraphIdx + bgDataSize

#endif // defined(useBarGraph)
#if defined(useMCP23017portExpander)
static const uint8_t m8MCP23017statusFlags =			nextAllowedValue;						// MCP23017 status flags
#define nextAllowedValue m8MCP23017statusFlags + 1

#endif // defined(useMCP23017portExpander)
#if defined(useDebugTerminal)
static const uint8_t m8ButtonFlags =					nextAllowedValue;						// debug terminal button injection flags
static const uint8_t m8PeekFlags =						m8ButtonFlags + 1;						// debug terminal peek (echo to terminal) flags
#define nextAllowedValue m8PeekFlags + 1

#endif // defined(useDebugTerminal)
static const uint8_t m8VariableEndIdx =					nextAllowedValue;						// end of 8-bit main program variable storage
static const uint8_t m8VariableLength =					m8VariableEndIdx - m8VariableStartIdx;

// 16-bit volatile variable array index values - these may be referenced inside an interrupt service routine

static const uint8_t v16VariableStartIdx =				nextAllowedValue;						// start of 16-bit volatile variable storage

static const uint8_t v16VehicleStopTimeoutIdx =			nextAllowedValue;						// engine idle timeout value in timer0 ticks
static const uint8_t v16EngineOffTimeoutIdx =			v16VehicleStopTimeoutIdx + 1;			// engine off coasting timeout value in timer0 ticks
static const uint8_t v16InputTimeoutIdx =				v16EngineOffTimeoutIdx + 1;				// button press timeout value in timer0 ticks
static const uint8_t v16ParkTimeoutIdx =				v16InputTimeoutIdx + 1;					// vehicle park (engine off and vehicle stopped) timeout value in timer0 ticks
static const uint8_t v16ActivityTimeoutIdx =			v16ParkTimeoutIdx + 1;					// total activity (vehicle park and no button press) timeout value in timer0 ticks
static const uint8_t v16DetectVehicleStopIdx =			v16ActivityTimeoutIdx + 1;				// minimum good vehicle speed in timer0 ticks
static const uint8_t v16DetectEngineOffIdx =			v16DetectVehicleStopIdx + 1;			// minimum good engine speed in timer0 ticks
static const uint8_t v16DisplayDelayCountIdx =			v16DetectEngineOffIdx + 1;				// dedicated display delay timer countdown in timer0 ticks
static const uint8_t v16WatchdogInjectorCountIdx =		v16DisplayDelayCountIdx + 1;			// watchdog counter for minimum good engine speed / engine off activity timeout
static const uint8_t v16WatchdogVSScountIdx =			v16WatchdogInjectorCountIdx + 1;		// watchdog counter for minimum good vehicle speed / vehicle stopped activity timeout
#define nextAllowedValue v16WatchdogVSScountIdx + 1

#if defined(useAnalogRead)
static const uint8_t v16AnalogStartIdx =				nextAllowedValue;						// start of analog value storage

static const uint8_t v16Analog0Idx =					nextAllowedValue;						// highest priority analog channel
static const uint8_t v16Analog1Idx =					v16Analog0Idx + 1;
static const uint8_t v16Analog2Idx =					v16Analog1Idx + 1;
static const uint8_t v16Analog3Idx =					v16Analog2Idx + 1;
static const uint8_t v16Analog4Idx =					v16Analog3Idx + 1;
static const uint8_t v16Analog5Idx =					v16Analog4Idx + 1;						// lowest priority analog channel
static const uint8_t v16AnalogGroundIdx =				v16Analog5Idx + 1;						// analog ground
#define nextAllowedValue v16AnalogGroundIdx + 1

static const uint8_t v16AnalogEndIdx =					nextAllowedValue;						// end of analog value storage
static const uint8_t v16AnalogLength =					v16AnalogEndIdx - v16AnalogStartIdx;

#endif // defined(useAnalogRead)
#if defined(useJSONoutput)
static const uint8_t v16JSONsubtitleCount =				nextAllowedValue;
#define nextAllowedValue v16JSONsubtitleCount + 1

#endif // defined(useJSONoutput)
#if defined(useBluetooth)
static const uint8_t v16BluetoothDelayCount =			nextAllowedValue;
#define nextAllowedValue v16BluetoothDelayCount + 1

#endif // defined(useBluetooth)
#if defined(useSimulatedFIandVSS)
static const uint8_t v16SignalSimVSStickIdx =			nextAllowedValue;
static const uint8_t v16SignalSimFIPtickIdx =			v16SignalSimVSStickIdx + 1;
static const uint8_t v16SignalSimFIPWtickIdx =			v16SignalSimFIPtickIdx + 1;
static const uint8_t v16SignalSimVSScountIdx =			v16SignalSimFIPWtickIdx + 1;
static const uint8_t v16SignalSimFIPcountIdx =			v16SignalSimVSScountIdx + 1;
static const uint8_t v16SignalSimFIPWcountIdx =			v16SignalSimFIPcountIdx + 1;
static const uint8_t v16SignalSimVSScycleCountIdx =		v16SignalSimFIPWcountIdx + 1;
static const uint8_t v16SignalSimFIPcycleCountIdx =		v16SignalSimVSScycleCountIdx + 1;
static const uint8_t v16SignalSimPeakPeriodIdx =		v16SignalSimFIPcycleCountIdx + 1;
static const uint8_t v16SignalSimPeakActiveIdx =		v16SignalSimPeakPeriodIdx + 1;
static const uint8_t v16SignalSimPeakPeriodCountIdx =	v16SignalSimPeakActiveIdx + 1;
static const uint8_t v16SignalSimPeakActiveCountIdx =	v16SignalSimPeakPeriodCountIdx + 1;
#define nextAllowedValue v16SignalSimPeakActiveCountIdx + 1

#endif // defined(useSimulatedFIandVSS)
static const uint8_t v16VariableEndIdx =				nextAllowedValue;						// end of 16-bit volatile variable storage
static const uint8_t v16VariableLength =				v16VariableEndIdx - v16VariableStartIdx;

// 32-bit volatile variable array index values - these may be referenced inside an interrupt service routine

static const uint8_t v32VariableStartIdx =				nextAllowedValue;						// start of 32-bit volatile variable storage

static const uint8_t v32Timer0OverflowCountIdx =		nextAllowedValue;
static const uint8_t v32ThisInjectorOpenCycleIdx =		v32Timer0OverflowCountIdx + 1;
static const uint8_t v32LastInjectorOpenCycleIdx =		v32ThisInjectorOpenCycleIdx + 1;
static const uint8_t v32ThisInjectorCloseCycleIdx =		v32LastInjectorOpenCycleIdx + 1;
static const uint8_t v32LastInjectorCloseCycleIdx =		v32ThisInjectorCloseCycleIdx + 1;
static const uint8_t v32EngineCycleAccumulatorIdx =		v32LastInjectorCloseCycleIdx + 1;
static const uint8_t v32EnginePeriodIdx =				v32EngineCycleAccumulatorIdx + 1;
static const uint8_t v32InjectorAccumulatorIdx =		v32EnginePeriodIdx + 1;
static const uint8_t v32ThisVSSpulseCycleIdx =			v32InjectorAccumulatorIdx + 1;
static const uint8_t v32LastVSSpulseCycleIdx =			v32ThisVSSpulseCycleIdx + 1;
static const uint8_t v32VSSpulseWidth0Idx =				v32LastVSSpulseCycleIdx + 1;
static const uint8_t v32VSSpulseWidth1Idx =				v32VSSpulseWidth0Idx + 1;
static const uint8_t v32VSSperiodIdx =					v32VSSpulseWidth1Idx + 1;
static const uint8_t v32MaximumVSSperiodIdx =			v32VSSperiodIdx + 1;				// maximum good VSS period in timer0 cycles
static const uint8_t v32MaximumEnginePeriodIdx =		v32MaximumVSSperiodIdx + 1;				// maximum good engine period in timer0 cycles
static const uint8_t v32InjectorOpenDelayIdx =			v32MaximumEnginePeriodIdx + 1;			// injector settle time in timer0 cycles
static const uint8_t v32InjectorValidMaxWidthIdx =		v32InjectorOpenDelayIdx + 1;			// maximum valid fuel injector pulse width in timer0 cycles
#define nextAllowedValue v32InjectorValidMaxWidthIdx + 1
#if defined(useCPUreading) || defined(useDebugCPUreading)
static const uint8_t v32SystemCycleIdx =				nextAllowedValue;						// system timer tick count
#define nextAllowedValue v32SystemCycleIdx + 1
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useClockSupport)
static const uint8_t v32ClockCycleIdx =					nextAllowedValue;						// software clock tick count
#define nextAllowedValue v32ClockCycleIdx + 1
#endif // defined(useClockSupport)
#if defined(useChryslerMAPCorrection)
static const uint8_t v32InjectorCorrectionIdx =			nextAllowedValue;						// Chrysler fuel injector correction value
#define nextAllowedValue v32InjectorCorrectionIdx + 1
#endif // defined(useChryslerMAPCorrection)
#if defined(useFEvTdata)
static const uint8_t v32FEvsTimePeriodTickIdx =			nextAllowedValue;						// time period for fuel economy vs time bargraph
static const uint8_t v32FEvsTimePeriodCountIdx =		v32FEvsTimePeriodTickIdx + 1;			// timer0 countdown timer for fuel economy vs time bargraph
#define nextAllowedValue v32FEvsTimePeriodCountIdx + 1
#endif // defined(useFEvTdata)
#if defined(useDebugCPUreading)
static const uint8_t v32WorkingTimer0Idx =				nextAllowedValue;						// timer0 overflow interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingTimer0Idx + 1
#if defined(useTimer1Interrupt)
static const uint8_t v32WorkingTimer1Idx =				nextAllowedValue;						// timer1 overflow interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingTimer1Idx + 1
#endif // defined(useTimer1Interrupt)
static const uint8_t v32WorkingInjectorOpenIdx =		nextAllowedValue;						// injector open interrupt handler stopwatch direct measurement
static const uint8_t v32WorkingInjectorCloseIdx =		v32WorkingInjectorOpenIdx + 1;			// injector close interrupt handler stopwatch direct measurement
static const uint8_t v32WorkingVSSpulseIdx =			v32WorkingInjectorCloseIdx + 1;			// VSS pulse interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingVSSpulseIdx + 1
#if defined(useAnalogRead)
static const uint8_t v32WorkingAnalogIdx =				nextAllowedValue;						// analog read interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingAnalogIdx + 1
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
static const uint8_t v32WorkingTwoWireIdx =				nextAllowedValue;						// two-wire interface interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingTwoWireIdx + 1
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
static const uint8_t v32WorkingSerial0Idx =				nextAllowedValue;						// UART0 output interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial0Idx + 1
#if defined(useSerial0PortInput)
static const uint8_t v32WorkingSerial0InputIdx =		nextAllowedValue;						// UART0 input interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial0InputIdx + 1
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
static const uint8_t v32WorkingSerial1Idx =				nextAllowedValue;						// UART1 output interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial1Idx + 1
#if defined(useSerial1PortInput)
static const uint8_t v32WorkingSerial1InputIdx =		nextAllowedValue;						// UART1 input interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial1InputIdx + 1
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
static const uint8_t v32WorkingSerial2Idx =				nextAllowedValue;						// UART2 output interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial2Idx + 1
#if defined(useSerial2PortInput)
static const uint8_t v32WorkingSerial2InputIdx =		nextAllowedValue;						// UART2 input interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial2InputIdx + 1
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
static const uint8_t v32WorkingSerial3Idx =				nextAllowedValue;						// UART3 output interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial3Idx + 1
#if defined(useSerial3PortInput)
static const uint8_t v32WorkingSerial3InputIdx =		nextAllowedValue;						// UART3 input interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingSerial3InputIdx + 1
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)

#endif // defined(useDebugCPUreading)
#if defined(useDragRaceFunction)
static const uint8_t v32DragRawTopSpeedIdx =			nextAllowedValue;
static const uint8_t v32DragRawTrapSpeedIdx =			v32DragRawTopSpeedIdx + 1;
static const uint8_t v32AccelHalfPeriodIdx =			v32DragRawTrapSpeedIdx + 1;
static const uint8_t v32AccelFullPeriodIdx =			v32AccelHalfPeriodIdx + 1;
static const uint8_t v32AccelDistanceValueIdx =			v32AccelFullPeriodIdx + 1;
static const uint8_t v32AccelDistanceCountIdx =			v32AccelDistanceValueIdx + 1;
#define nextAllowedValue v32AccelDistanceCountIdx + 1
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
static const uint8_t v32CoastdownMeasurement1Idx =		nextAllowedValue;
static const uint8_t v32CoastdownMeasurement2Idx =		v32CoastdownMeasurement1Idx + 1;
static const uint8_t v32CoastdownMeasurement3Idx =		v32CoastdownMeasurement2Idx + 1;
static const uint8_t v32CoastdownMeasurement4Idx =		v32CoastdownMeasurement3Idx + 1;
static const uint8_t v32CoastdownPeriodIdx =			v32CoastdownMeasurement4Idx + 1;
#define nextAllowedValue v32CoastdownPeriodIdx + 1
#endif // defined(useCoastDownCalculator)

static const uint8_t v32VariableEndIdx =				nextAllowedValue;						// end of 32-bit volatile variable storage
static const uint8_t v32VariableLength =				v32VariableEndIdx - v32VariableStartIdx;

// 32-bit main program variable array index values - these should NEVER be referenced inside an interrupt service routine

static const uint8_t m32VariableStartIdx =				nextAllowedValue;						// start of 32-bit main program variable storage

static const uint8_t m32CyclesPerVolumeIdx =			nextAllowedValue;						// injector conversion factor of unit volume per timer0 cycle
static const uint8_t m32SpeedFactorIdx =				m32CyclesPerVolumeIdx + 1;				// (timer0 cycles * unit distance * decimal format)/(pulses * hour)
#define nextAllowedValue m32SpeedFactorIdx + 1

#if defined(useDragRaceFunction)
static const uint8_t m32DragTopSpeedIdx =				nextAllowedValue;
static const uint8_t m32DragTrapSpeedIdx =				m32DragTopSpeedIdx + 1;
#define nextAllowedValue m32DragTrapSpeedIdx + 1

#endif // defined(useDragRaceFunction)
#if defined(useChryslerMAPCorrection)
static const uint8_t m32MAPpressureIdx =				nextAllowedValue;
static const uint8_t m32BaroPressureIdx =				m32MAPpressureIdx + 1;
static const uint8_t m32FuelPressureIdx =				m32BaroPressureIdx + 1;
static const uint8_t m32InjPressureIdx =				m32FuelPressureIdx + 1;
static const uint8_t m32AnalogMAPfloorIdx =				m32InjPressureIdx + 1;
static const uint8_t m32AnalogMAPnumerIdx =				m32AnalogMAPfloorIdx + 1;
static const uint8_t m32AnalogMAPdenomIdx =				m32AnalogMAPnumerIdx + 1;
#define nextAllowedValue m32AnalogMAPdenomIdx + 1
#if defined(useChryslerBaroSensor)
static const uint8_t m32AnalogBaroFloorIdx =			nextAllowedValue;
static const uint8_t m32AnalogBaroNumerIdx =			m32AnalogBaroFloorIdx + 1;
static const uint8_t m32AnalogBaroDenomIdx =			m32AnalogBaroNumerIdx + 1;
#define nextAllowedValue m32AnalogBaroDenomIdx + 1
#endif // defined(useChryslerBaroSensor)

#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t m32FEvsSpeedMinThresholdIdx =		nextAllowedValue;						// minimum speed for fuel econ vs speed bargraph
static const uint8_t m32FEvsSpeedQuantumIdx =			m32FEvsSpeedMinThresholdIdx + 1;		// speed quantum for each bar in fuel econ vs speed bargraph
#define nextAllowedValue m32FEvsSpeedQuantumIdx + 1

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useCPUreading) || defined(useDebugCPUreading)
static const uint8_t m32AvailableRAMidx =				nextAllowedValue;						// amount of remaining free RAM
#define nextAllowedValue m32AvailableRAMidx + 1

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useCPUreading) || defined(useDebugCPUreading)
static const uint8_t m32CPUworkingLoopStartIdx =		nextAllowedValue;
static const uint8_t m32CPUworkingMainStartIdx =		m32CPUworkingLoopStartIdx + 1;
static const uint8_t m32CPUworkingIdleStartIdx =		m32CPUworkingMainStartIdx + 1;
static const uint8_t m32DbgWorkingMainStartIdx =		m32CPUworkingIdleStartIdx + 1;
#define nextAllowedValue m32DbgWorkingMainStartIdx + 1
#if defined(useDebugCPUreading)
static const uint8_t m32DbgWorkingS64StartIdx =			nextAllowedValue;
static const uint8_t m32DbgWorkingMathStartIdx =		m32DbgWorkingS64StartIdx + 1;
#define nextAllowedValue m32DbgWorkingMathStartIdx + 1
#endif // defined(useDebugCPUreading)

static const uint8_t m32CPUworkingMainLoopIdx =			nextAllowedValue;
static const uint8_t m32CPUworkingMainProcessIdx =		m32CPUworkingMainLoopIdx + 1;
static const uint8_t m32CPUworkingIdleProcessIdx =		m32CPUworkingMainProcessIdx + 1;
#define nextAllowedValue m32CPUworkingIdleProcessIdx + 1
#if defined(useDebugCPUreading)
static const uint8_t m32DbgWorkingMainDevicesIdx =		nextAllowedValue;
static const uint8_t m32DbgWorkingMainActivityIdx =		m32DbgWorkingMainDevicesIdx + 1;
static const uint8_t m32DbgWorkingMainSampleIdx =		m32DbgWorkingMainActivityIdx + 1;
static const uint8_t m32DbgWorkingMainOutputIdx =		m32DbgWorkingMainSampleIdx + 1;
static const uint8_t m32DbgWorkingMainOtherIdx =		m32DbgWorkingMainOutputIdx + 1;
static const uint8_t m32DbgWorkingS64processIdx =		m32DbgWorkingMainOtherIdx + 1;
#define nextAllowedValue m32DbgWorkingS64processIdx + 1
#endif // defined(useDebugCPUreading)

static const uint8_t m32CPUsampledMainLoopIdx =			nextAllowedValue;
static const uint8_t m32CPUsampledMainProcessIdx =		m32CPUsampledMainLoopIdx + 1;
static const uint8_t m32CPUsampledIdleProcessIdx =		m32CPUsampledMainProcessIdx + 1;
#define nextAllowedValue m32CPUsampledIdleProcessIdx + 1
#if defined(useDebugCPUreading)

static const uint8_t m32DbgSampledMainDevicesIdx =		nextAllowedValue;
static const uint8_t m32DbgSampledMainActivityIdx =		m32DbgSampledMainDevicesIdx + 1;
static const uint8_t m32DbgSampledMainSampleIdx =		m32DbgSampledMainActivityIdx + 1;
static const uint8_t m32DbgSampledMainOutputIdx =		m32DbgSampledMainSampleIdx + 1;
static const uint8_t m32DbgSampledMainOtherIdx =		m32DbgSampledMainOutputIdx + 1;
static const uint8_t m32DbgSampledS64processIdx =		m32DbgSampledMainOtherIdx + 1;
static const uint8_t m32DbgSampledInterruptProcessIdx =	m32DbgSampledS64processIdx + 1;
static const uint8_t m32DbgSampledTimer0Idx =			m32DbgSampledInterruptProcessIdx + 1;
#define nextAllowedValue m32DbgSampledTimer0Idx + 1
#if defined(useTimer1Interrupt)
static const uint8_t m32DbgSampledTimer1Idx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledTimer1Idx + 1
#endif // defined(useTimer1Interrupt)
static const uint8_t m32DbgSampledInjectorOpenIdx =		nextAllowedValue;
static const uint8_t m32DbgSampledInjectorCloseIdx =	m32DbgSampledInjectorOpenIdx + 1;
static const uint8_t m32DbgSampledVSSpulseIdx =			m32DbgSampledInjectorCloseIdx + 1;
#define nextAllowedValue m32DbgSampledVSSpulseIdx + 1
#if defined(useAnalogRead)
static const uint8_t m32DbgSampledAnalogIdx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledAnalogIdx + 1
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
static const uint8_t m32DbgSampledTwoWireIdx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledTwoWireIdx + 1
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
static const uint8_t m32DbgSampledSerial0Idx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial0Idx + 1
#if defined(useSerial0PortInput)
static const uint8_t m32DbgSampledSerial0InputIdx =		nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial0InputIdx + 1
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
static const uint8_t m32DbgSampledSerial1Idx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial1Idx + 1
#if defined(useSerial1PortInput)
static const uint8_t m32DbgSampledSerial1InputIdx =		nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial1InputIdx + 1
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
static const uint8_t m32DbgSampledSerial2Idx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial2Idx + 1
#if defined(useSerial2PortInput)
static const uint8_t m32DbgSampledSerial2InputIdx =		nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial2InputIdx + 1
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
static const uint8_t m32DbgSampledSerial3Idx =			nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial3Idx + 1
#if defined(useSerial3PortInput)
static const uint8_t m32DbgSampledSerial3InputIdx =		nextAllowedValue;
#define nextAllowedValue m32DbgSampledSerial3InputIdx + 1
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)

static const uint8_t m32DebugAccS64multIdx =			nextAllowedValue;
static const uint8_t m32DebugCountS64multIdx =			m32DebugAccS64multIdx + 1;
static const uint8_t m32DebugAccS64divIdx =				m32DebugCountS64multIdx + 1;
static const uint8_t m32DebugCountS64divIdx =			m32DebugAccS64divIdx + 1;
#define nextAllowedValue m32DebugCountS64divIdx + 1
#if defined(useIsqrt)
static const uint8_t m32DebugAccS64sqrtIdx =			nextAllowedValue;
static const uint8_t m32DebugCountS64sqrtIdx =			m32DebugAccS64sqrtIdx + 1;
#define nextAllowedValue m32DebugCountS64sqrtIdx + 1
#endif // defined(useIsqrt)
#if defined(useDebugTerminalSWEET64)
static const uint8_t m32S64programCyclesIdx =			nextAllowedValue;						// timer0 cycles executed in the last ^T trace run
static const uint8_t m32S64programInstrIdx =			m32S64programCyclesIdx + 1;				// instructions executed in the last ^T trace run
#define nextAllowedValue m32S64programInstrIdx + 1
#endif // defined(useDebugTerminalSWEET64)
#endif // defined(useDebugCPUreading)

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useBluetooth)
static const uint8_t m32BluetoothMainValue =			nextAllowedValue;						// default string value after '!' read-in character
#define nextAllowedValue m32BluetoothMainValue + 1

#endif // defined(useBluetooth)
static const uint8_t m32VariableEndIdx =				nextAllowedValue;						// end of 32-bit main program variable storage
static const uint8_t m32VariableLength =				m32VariableEndIdx - m32VariableStartIdx;

// 64-bit main program variable array index values - these should NEVER be referenced inside an interrupt service routine

static const uint8_t m64VariableStartIdx =				nextAllowedValue;						// start of 64-bit main program variable storage

static const uint8_t m64TankSizeIdx =					nextAllowedValue;						// tank quantity in timer0 cycles
static const uint8_t m64BingoTankSizeIdx =				m64TankSizeIdx + 1;						// bingo fuel quantity in timer0 cycles
#define nextAllowedValue m64BingoTankSizeIdx + 1

static const uint8_t m64VariableEndIdx =				nextAllowedValue;						// end of 64-bit main program variable storage
static const uint8_t m64VariableLength =				m64VariableEndIdx - m64VariableStartIdx;

static const uint8_t programVariableMaxIdx =			nextAllowedValue;

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
