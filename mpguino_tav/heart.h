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
	static uint32_t getCycle0Length(uint8_t lastCycleIdx, uint32_t thisCycle);
	static uint32_t cycles0(void);
	static uint8_t delay0(uint16_t ms, uint8_t repeatFlag);
	static uint8_t delayInt0(uint16_t ms, uint8_t repeatFlag);
	static void wait0(uint16_t ms);
	static void changeBitFlagBits(uint8_t bitFlagIdx, uint8_t maskAND, uint8_t maskOR);
	static void performSleepMode(uint8_t sleepMode);
#if defined(useTimer1Interrupt)
	static void enableTimer1Interrupt(void);
	static void disableTimer1Interrupt(void);
#endif // defined(useTimer1Interrupt)

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

#endif // defined(useBarGraph)
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

static const uint8_t samplesPerSecond = 2; // how many times will we sample in a second (also for sampling non-critical ADC channels, if configured)
static const uint8_t updatesPerSecond = 2; // how many times will we update the primary user display in a second
static const uint8_t buttonSamplesPerSecond = 32; // how many times will we try to sample ADC/TWI button presses in a second

static const uint8_t systemProcessorSpeed = (F_CPU / 1000000ul);

static const uint32_t t0CyclesPerSecond =	(uint32_t)(F_CPU / 64ul);
static const uint32_t t0TicksPerSecond =	(uint32_t)(t0CyclesPerSecond / 256ul);
static const uint32_t t0TicksPerDay =		(uint32_t)(86400ul * t0TicksPerSecond);

static const uint16_t delay0TickSampleLoop =	(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(samplesPerSecond * 256ul)) - 1;
static const uint16_t delay0TickUpdateLoop =	(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(updatesPerSecond * 256ul)) - 1;
static const uint16_t delay0TickButtonSample =	(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(buttonSamplesPerSecond * 256ul)) - 1;

static const uint16_t delay0Tick3000ms =		(uint16_t)(ceil)((double)(3ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 3 second delay
static const uint16_t delay0Tick2000ms =		(uint16_t)(ceil)((double)(2ul * t0CyclesPerSecond) / (double)(256ul)) - 1; // 2 second delay
static const uint16_t delay0Tick1600ms =		(uint16_t)(ceil)((double)(16ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.6 second delay
static const uint16_t delay0Tick1500ms =		(uint16_t)(ceil)((double)(15ul * t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 1.5 second delay
static const uint16_t delay0Tick1333ms =		(uint16_t)(ceil)((double)(4ul * t0CyclesPerSecond) / (double)(256ul * 3ul)) - 1; // 1.3333 second delay
static const uint16_t delay0Tick1000ms =		(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul)); // 1 second delay
static const uint16_t delay0Tick500ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 2ul)) - 1; // display cursor toggle period is 500 milliseconds
static const uint16_t delay0Tick200ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 5ul)) - 1; // 200 millisecond delay
static const uint16_t delay0Tick100ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 10ul)) - 1; // 100 millisecond delay
static const uint16_t delay0Tick50ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 20ul)) - 1; // 50 millisecond delay button debounce
static const uint16_t delay0Tick30ms =			(uint16_t)(ceil)((double)(30ul * t0CyclesPerSecond) / (double)(256ul * 1000ul)) - 1; // 30 millisecond delay
static const uint16_t delay0Tick20ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 50ul)) - 1; // 20 millisecond delay
static const uint16_t delay0Tick5ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 200ul)) - 1; // 5 millisecond delay
static const uint16_t delay0Tick2ms =			(uint16_t)(ceil)((double)(t0CyclesPerSecond) / (double)(256ul * 500ul)) - 1; // 2 millisecond delay

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
#define nextAllowedValue 0

// 8-bit volatile variable array index values - these may be referenced inside an interrupt service routine

static const uint8_t v8VariableStartIdx =				nextAllowedValue;						// start of 8-bit volatile variable storage

static const uint8_t v8Timer0CommandIdx =				nextAllowedValue;						// timer0 command flags
static const uint8_t v8Timer0Status0Idx =				v8Timer0CommandIdx + 1;					// timer0 status flags
static const uint8_t v8Timer0Status1Idx =				v8Timer0Status0Idx + 1;					// more timer0 status flags
static const uint8_t v8DirtyIdx =						v8Timer0Status1Idx + 1;					// MPGuino FI / VSS dirty (processing) flags
static const uint8_t v8AwakeIdx =						v8DirtyIdx + 1;							// MPGuino awake flags
static const uint8_t v8Timer0DelayIdx =					v8AwakeIdx + 1;							// timer0 delay function flags
static const uint8_t v8Timer0DelayRepeatIdx =			v8Timer0DelayIdx + 1;					// timer0 delay function repeat flags
static const uint8_t v8Timer0DelaySignalIdx =			v8Timer0DelayRepeatIdx + 1;				// timer0 delay function signal flags
static const uint8_t v8ActivityIdx =					v8Timer0DelaySignalIdx + 1;				// MPGuino activity flags
static const uint8_t v8ActivityIdxChangeIdx =			v8ActivityIdx + 1;						// MPGuino activity change detection flags
static const uint8_t v8HeartbeatBitmaskIdx =			v8ActivityIdxChangeIdx + 1;				// Sample loop-driven heartbeat bitmask
static const uint8_t v8VSSdebounceTickIdx =				v8HeartbeatBitmaskIdx + 1;				// for VSS debouncing
static const uint8_t v8VSSdebounceCountIdx =			v8VSSdebounceTickIdx + 1;				// for VSS debouncing
#define nextAllowedValue v8VSSdebounceCountIdx + 1
#if defined(useButtonInput)
static const uint8_t v8ThisButtonStateIdx =				nextAllowedValue;
static const uint8_t v8LastButtonStateIdx =				v8ThisButtonStateIdx + 1;
static const uint8_t v8ButtonPressIdx =					v8LastButtonStateIdx + 1;
#define nextAllowedValue v8ButtonPressIdx + 1
#endif // defined(useButtonInput)
#if defined(useAnalogRead)
static const uint8_t v8AnalogCommandIdx =				nextAllowedValue;						// analog command flags
static const uint8_t v8AnalogStatusIdx =				v8AnalogCommandIdx + 1;					// analog status flags
#define nextAllowedValue v8AnalogStatusIdx + 1
#endif // defined(useAnalogRead)
#if defined(useTWIsupport)
static const uint8_t v8TWIstatusIdx =					nextAllowedValue;						// TWI status flags
static const uint8_t v8TWIerrorIdx =					v8TWIstatusIdx + 1;						// TWI error flags
#define nextAllowedValue v8TWIerrorIdx + 1
#endif // defined(useTWIsupport)
#if defined(useTimer1Interrupt)
static const uint8_t v8Timer1CommandIdx =				nextAllowedValue;						// timer1 command flags
static const uint8_t v8Timer1StatusIdx =				v8Timer1CommandIdx + 1;					// timer1 status flags
#define nextAllowedValue v8Timer1StatusIdx + 1
#if defined(useSimulatedFIandVSS)
static const uint8_t v8SignalSimModeIdx =				nextAllowedValue;						// simulated FI/VSS mode flags
#define nextAllowedValue v8SignalSimModeIdx + 1
#endif // defined(useSimulatedFIandVSS)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t v8BLEstatusIdx =					nextAllowedValue;						// AdaFruit BLEfriend condition flags
#define nextAllowedValue v8BLEstatusIdx + 1
#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useTimer1Interrupt)
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
#if defined(useBarFuelEconVsTime)
static const uint8_t v8FEvTimeTripIdx =					nextAllowedValue;
#define nextAllowedValue v8FEvTimeTripIdx + 1
#endif // defined(useBarFuelEconVsTime)
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

static const uint8_t m8Delay0FlagStartIdx =				nextAllowedValue;

#if defined(useLCDoutput)
static const uint8_t m8Delay0FlagLCDidx =				nextAllowedValue;
#define nextAllowedValue m8Delay0FlagLCDidx + 1
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
static const uint8_t m8Delay0FlagTFTidx =				nextAllowedValue;
#define nextAllowedValue m8Delay0FlagTFTidx + 1
#endif // defined(useTFToutput)
#if defined(useSerial0Port)
static const uint8_t m8Delay0FlagSerial0idx =			nextAllowedValue;
#define nextAllowedValue m8Delay0FlagSerial0idx + 1
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
static const uint8_t m8Delay0FlagSerial1idx =			nextAllowedValue;
#define nextAllowedValue m8Delay0FlagSerial1idx + 1
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
static const uint8_t m8Delay0FlagSerial2idx =			nextAllowedValue;
#define nextAllowedValue m8Delay0FlagSerial2idx + 1
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
static const uint8_t m8Delay0FlagSerial3idx =			nextAllowedValue;
#define nextAllowedValue m8Delay0FlagSerial3idx + 1
#endif // defined(useSerial3Port)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t m8Delay0FlagBLEfriendIdx =			nextAllowedValue;
static const uint8_t m8Delay0FlagBluetoothIdx =			m8Delay0FlagBLEfriendIdx + 1;
#define nextAllowedValue m8Delay0FlagBluetoothIdx + 1
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(__AVR_ATmega32U4__)
static const uint8_t m8Delay0FlagUSBidx =				nextAllowedValue;
#define nextAllowedValue m8Delay0FlagUSBidx + 1
#endif // defined(__AVR_ATmega32U4__)

static const uint8_t m8Delay0FlagEndIdx =				nextAllowedValue;
static const uint8_t m8Delay0FlagLength =				m8Delay0FlagEndIdx - m8Delay0FlagStartIdx;

#if defined(useClockSupport)
static const uint8_t m8SecondIdx =						nextAllowedValue;
static const uint8_t m8MinuteIdx =						m8SecondIdx + 1;
static const uint8_t m8HourIdx =						m8MinuteIdx + 1;
#define nextAllowedValue m8HourIdx + 1

#endif // defined(useClockSupport)
#if defined(useActivityRecord)
static const uint8_t m8ActivityStatusIdx =				nextAllowedValue;
#define nextAllowedValue m8ActivityStatusIdx + 1
#if defined(useActivityLED)
static const uint8_t m8ActivityOutputIdx =				nextAllowedValue;
#define nextAllowedValue m8ActivityOutputIdx + 1
#endif // defined(useActivityLED)

#endif // defined(useActivityRecord)
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
#if defined(useSimulatedFIandVSS)
static const uint8_t m8SignalSimVSSdelayFlagIdx =		nextAllowedValue;
static const uint8_t m8SignalSimFIPdelayFlagIdx =		m8SignalSimVSSdelayFlagIdx + 1;
static const uint8_t m8SignalSimVSSidx =				m8SignalSimFIPdelayFlagIdx + 1;			// simulated VSS value index
static const uint8_t m8SignalSimFIPidx =				m8SignalSimVSSidx + 1;					// simulated fuel injector value index
static const uint8_t m8SignalSimVSSstate =				m8SignalSimFIPidx + 1;					// simulated VSS state
static const uint8_t m8SignalSimFIPstate =				m8SignalSimVSSstate + 1;				// simulated fuel injector state
#define nextAllowedValue m8SignalSimFIPstate + 1

#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugTerminal)
static const uint8_t m8PeekFlags =						nextAllowedValue;						// debug terminal peek (echo to terminal) flags
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
static const uint8_t v16Timer0DelayTickIdx =			v16DetectEngineOffIdx + 1;				// timer0 delay repeater start values in timer0 ticks
static const uint8_t v16Timer0DelayCountIdx =			v16Timer0DelayTickIdx + 8;				// timer0 delay repeater countdown values in timer0 ticks
#define nextAllowedValue v16Timer0DelayCountIdx + 8

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
#if defined(useSimulatedFIandVSS)
static const uint8_t v16SignalSimVSStickLength =		nextAllowedValue;
static const uint8_t v16SignalSimFIPtickLength =		v16SignalSimVSStickLength + 1;
static const uint8_t v16SignalSimFIPWtickLength =		v16SignalSimFIPtickLength + 1;
#define nextAllowedValue v16SignalSimFIPWtickLength + 1

#endif // defined(useSimulatedFIandVSS)
static const uint8_t v16VariableEndIdx =				nextAllowedValue;						// end of 16-bit volatile variable storage
static const uint8_t v16VariableLength =				v16VariableEndIdx - v16VariableStartIdx;

// 32-bit volatile variable array index values - these may be referenced inside an interrupt service routine

static const uint8_t v32VariableStartIdx =				nextAllowedValue;						// start of 32-bit volatile variable storage

static const uint8_t v32Timer0OverflowCountIdx =		nextAllowedValue;
static const uint8_t v32InjectorOpenTickStartIdx =		v32Timer0OverflowCountIdx + 1;
static const uint8_t v32EnginePeriodTickStartIdx =		v32InjectorOpenTickStartIdx + 1;
static const uint8_t v32EnginePeriodTickCloseIdx =		v32EnginePeriodTickStartIdx + 1;
static const uint8_t v32MaximumVSSperiodIdx =			v32EnginePeriodTickCloseIdx + 1;		// maximum good VSS period in timer0 cycles
static const uint8_t v32MaximumEnginePeriodIdx =		v32MaximumVSSperiodIdx + 1;				// maximum good engine period in timer0 cycles
static const uint8_t v32InjectorOpenDelayIdx =			v32MaximumEnginePeriodIdx + 1;			// injector settle time in timer0 cycles
static const uint8_t v32InjectorValidMaxWidthIdx =		v32InjectorOpenDelayIdx + 1;			// maximum valid fuel injector pulse width in timer0 cycles
#define nextAllowedValue v32InjectorValidMaxWidthIdx + 1
#if defined(useCPUreading)
static const uint8_t v32SystemCycleIdx =				nextAllowedValue;						// system timer tick count
#define nextAllowedValue v32SystemCycleIdx + 1
#endif // defined(useCPUreading)
#if defined(useClockSupport)
static const uint8_t v32ClockCycleIdx =					nextAllowedValue;						// software clock tick count
#define nextAllowedValue v32ClockCycleIdx + 1
#endif // defined(useClockSupport)
#if defined(useChryslerMAPCorrection)
static const uint8_t v32InjectorCorrectionIdx =			nextAllowedValue;						// Chrysler fuel injector correction value
#define nextAllowedValue v32InjectorCorrectionIdx + 1
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsTime)
static const uint8_t v32FEvsTimePeriodTimeoutIdx =		nextAllowedValue;						// time period for fuel economy vs time bargraph
#define nextAllowedValue v32FEvsTimePeriodTimeoutIdx + 1
#endif // defined(useBarFuelEconVsTime)
#if defined(useDebugCPUreading)
static const uint8_t v32WorkingInterruptProcessIdx =	nextAllowedValue;						// interrupt handler stopwatch direct measurement
#define nextAllowedValue v32WorkingInterruptProcessIdx + 1
#endif // defined(useDebugCPUreading)
#if defined(useDragRaceFunction)
static const uint8_t v32DragRawInstantSpeedIdx =		nextAllowedValue;
static const uint8_t v32DragRawTrapSpeedIdx =			v32DragRawInstantSpeedIdx + 1;
static const uint8_t v32AccelHalfPeriodIdx =			v32DragRawTrapSpeedIdx + 1;
static const uint8_t v32AccelFullPeriodIdx =			v32AccelHalfPeriodIdx + 1;
static const uint8_t v32AccelDistanceValueIdx =			v32AccelFullPeriodIdx + 1;
#define nextAllowedValue v32AccelDistanceValueIdx + 1
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
static const uint8_t m32DragInstantSpeedIdx =			nextAllowedValue;
static const uint8_t m32DragTrapSpeedIdx =				m32DragInstantSpeedIdx + 1;
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
#if defined(useCPUreading)
static const uint8_t m32AvailableRAMidx =				nextAllowedValue;						// amount of remaining free RAM
#define nextAllowedValue m32AvailableRAMidx + 1

#endif // defined(useCPUreading)
#if defined(useCPUreading) || defined(useDebugCPUreading)
static const uint8_t m32CPUworkingLoopStartIdx =		nextAllowedValue;
static const uint8_t m32CPUworkingMainStartIdx =		m32CPUworkingLoopStartIdx + 1;
static const uint8_t m32CPUworkingIdleStartIdx =		m32CPUworkingMainStartIdx + 1;
#define nextAllowedValue m32CPUworkingIdleStartIdx + 1
#if defined(useDebugCPUreading)
static const uint8_t m32DbgWorkingMainStartIdx =		nextAllowedValue;
static const uint8_t m32DbgWorkingS64StartIdx =			m32DbgWorkingMainStartIdx + 1;
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

static const uint8_t m32DebugAccS64multIdx =			m32DbgSampledInterruptProcessIdx + 1;
static const uint8_t m32DebugCountS64multIdx =			m32DebugAccS64multIdx + 1;
static const uint8_t m32DebugAccS64divIdx =				m32DebugCountS64multIdx + 1;
static const uint8_t m32DebugCountS64divIdx =			m32DebugAccS64divIdx + 1;
#define nextAllowedValue m32DebugCountS64divIdx + 1
#if defined(useIsqrt)
static const uint8_t m32DebugAccS64sqrtIdx =			nextAllowedValue;
static const uint8_t m32DebugCountS64sqrtIdx =			m32DebugAccS64sqrtIdx + 1;
#define nextAllowedValue m32DebugCountS64sqrtIdx + 1
#endif // defined(useIsqrt)
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

#if defined(useDebugTerminalLabels)
static const char terminalVariableLabels[] PROGMEM = {
	"v8Timer0CommandIdx" tcEOS
	"v8Timer0Status0Idx" tcEOS
	"v8Timer0Status1Idx" tcEOS
	"v8DirtyIdx" tcEOS
	"v8AwakeIdx" tcEOS
	"v8Timer0DelayIdx" tcEOS
	"v8Timer0DelayRepeatIdx" tcEOS
	"v8Timer0DelaySignalIdx" tcEOS
	"v8ActivityIdx" tcEOS
	"v8ActivityIdxChangeIdx" tcEOS
	"v8HeartbeatBitmaskIdx" tcEOS
	"v8VSSdebounceTickIdx" tcEOS
	"v8VSSdebounceCountIdx" tcEOS
#if defined(useButtonInput)
	"v8ThisButtonStateIdx" tcEOS
	"v8LastButtonStateIdx" tcEOS
	"v8ButtonPressIdx" tcEOS
#endif // defined(useButtonInput)
#if defined(useAnalogRead)
	"v8AnalogCommandIdx" tcEOS
	"v8AnalogStatusIdx" tcEOS
#endif // defined(useAnalogRead)
#if defined(useTWIsupport)
	"v8TWIstatusIdx" tcEOS
	"v8TWIerrorIdx" tcEOS
#endif // defined(useTWIsupport)
#if defined(useTimer1Interrupt)
	"v8Timer1CommandIdx" tcEOS
	"v8Timer1StatusIdx" tcEOS
#if defined(useSimulatedFIandVSS)
	"v8SignalSimModeIdx" tcEOS
#endif // defined(useSimulatedFIandVSS)
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
#if defined(useBarFuelEconVsTime)
	"v8FEvTimeTripIdx" tcEOS
#endif // defined(useBarFuelEconVsTime)
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
#if defined(useLCDoutput)
	"m8Delay0FlagLCDidx" tcEOS
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
	"m8Delay0FlagTFTidx" tcEOS
#endif // defined(useTFToutput)
#if defined(useSerial0Port)
	"m8Delay0FlagSerial0idx" tcEOS
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	"m8Delay0FlagSerial1idx" tcEOS
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	"m8Delay0FlagSerial2idx" tcEOS
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	"m8Delay0FlagSerial3idx" tcEOS
#endif // defined(useSerial3Port)
#if defined(useBluetoothAdaFruitSPI)
	"m8Delay0FlagBLEfriendIdx" tcEOS
	"m8Delay0FlagBluetoothIdx" tcEOS
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(__AVR_ATmega32U4__)
	"m8Delay0FlagUSBidx" tcEOS
#endif // defined(__AVR_ATmega32U4__)
#if defined(useClockSupport)
	"m8SecondIdx" tcEOS
	"m8MinuteIdx" tcEOS
	"m8HourIdx" tcEOS
#endif // defined(useClockSupport)
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
#if defined(useSimulatedFIandVSS)
	"m8SignalSimVSSdelayFlagIdx" tcEOS
	"m8SignalSimFIPdelayFlagIdx" tcEOS
	"m8SignalSimVSSidx" tcEOS
	"m8SignalSimFIPidx" tcEOS
	"m8SignalSimVSSstate" tcEOS
	"m8SignalSimFIPstate" tcEOS
#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugTerminal)
	"m8PeekFlags" tcEOS
#endif // defined(useDebugTerminal)

	"v16VehicleStopTimeoutIdx" tcEOS			// timer0
	"v16EngineOffTimeoutIdx" tcEOS				// timer0
	"v16InputTimeoutIdx" tcEOS					// timer0
	"v16ParkTimeoutIdx" tcEOS					// timer0
	"v16ActivityTimeoutIdx" tcEOS				// timer0
	"v16DetectVehicleStopIdx" tcEOS				// vss
	"v16DetectEngineOffIdx" tcEOS				// fi open
	"v16Timer0DelayTickIdx[0]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[1]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[2]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[3]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[4]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[5]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[6]" tcEOS			// timer0
	"v16Timer0DelayTickIdx[7]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[0]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[1]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[2]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[3]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[4]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[5]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[6]" tcEOS			// timer0
	"v16Timer0DelayCountIdx[7]" tcEOS			// timer0 
#if defined(useAnalogRead)
	"v16Analog0Idx" tcEOS						// analog
	"v16Analog1Idx" tcEOS						// analog
	"v16Analog2Idx" tcEOS						// analog
	"v16Analog3Idx" tcEOS						// analog
	"v16Analog4Idx" tcEOS						// analog
	"v16Analog5Idx" tcEOS						// analog
	"v16AnalogGroundIdx" tcEOS					// analog
#endif // defined(useAnalogRead)
#if defined(useSimulatedFIandVSS)
	"v16SignalSimVSStickLength" tcEOS			// timer1
	"v16SignalSimFIPtickLength" tcEOS			// timer1
	"v16SignalSimFIPWtickLength" tcEOS			// timer1
#endif // defined(useSimulatedFIandVSS)

	"v32Timer0OverflowCountIdx" tcEOS			// timer0, fi, vss, timing
	"v32InjectorOpenTickStartIdx" tcEOS			// fi
	"v32EnginePeriodTickStartIdx" tcEOS			// fi
	"v32EnginePeriodTickCloseIdx" tcEOS			// fi
	"v32MaximumVSSperiodIdx" tcEOS				// vss
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
#if defined(useBarFuelEconVsTime)
	"v32FEvsTimePeriodTimeoutIdx" tcEOS			// timer0
#endif // defined(useBarFuelEconVsTime)
#if defined(useDebugCPUreading)
	"v32WorkingInterruptProcessIdx" tcEOS		// all interrupts
#endif // defined(useDebugCPUreading)
#if defined(useDragRaceFunction)
	"v32DragRawInstantSpeedIdx" tcEOS			// vss
	"v32DragRawTrapSpeedIdx" tcEOS				// vss
	"v32AccelHalfPeriodIdx" tcEOS				// vss
	"v32AccelFullPeriodIdx" tcEOS				// vss
	"v32AccelDistanceValueIdx" tcEOS			// vss
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"v32CoastdownMeasurement1Idx" tcEOS			// vss
	"v32CoastdownMeasurement2Idx" tcEOS			// vss
	"v32CoastdownMeasurement3Idx" tcEOS			// vss
	"v32CoastdownMeasurement4Idx" tcEOS			// vss
	"v32CoastdownPeriodIdx" tcEOS				// timer0, vss
#endif // defined(useCoastDownCalculator)

	"m32CyclesPerVolumeIdx" tcEOS				// main program only
	"m32SpeedFactorIdx" tcEOS					// main program only
#if defined(useDragRaceFunction)
	"m32DragInstantSpeedIdx" tcEOS				// main program only
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
#if defined(useDebugCPUreading)
	"m32DbgWorkingMainStartIdx" tcEOS
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
	"m32DebugAccS64multIdx" tcEOS
	"m32DebugCountS64multIdx" tcEOS
	"m32DebugAccS64divIdx" tcEOS
	"m32DebugCountS64divIdx" tcEOS
#if defined(useIsqrt)
	"m32DebugAccS64sqrtIdx" tcEOS
	"m32DebugCountS64sqrtIdx" tcEOS
#endif // defined(useIsqrt)
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
static const uint8_t ecsDoMPGuinoInitSoftware =	0b10000000;
static const uint8_t ecsDoMPGuinoInitHardware =	0b01000000;
static const uint8_t ecsDoMetricConversion =	0b00100000;
static const uint8_t ecsChangeDisplay =			0b00010000;
static const uint8_t ecsCalculateFuelParam =	0b00001000;
static const uint8_t ecsResetBarFEvsSpeed =		0b00000100;
static const uint8_t ecsEEPROMchangeDetected =	0b00000010;

static const uint8_t ecsEEPROMchangeFlags =		(ecsDoMPGuinoInitSoftware | ecsDoMPGuinoInitHardware | ecsDoMetricConversion | ecsChangeDisplay | ecsCalculateFuelParam | ecsResetBarFEvsSpeed | ecsEEPROMchangeDetected);

// bit flags for use with m8MetricModeFlags
static const uint8_t mmDisplayAlternateFE =		0b00000010;
static const uint8_t mmDisplayMetric =			0b00000001;

static const uint8_t mmFuelEconOutputFlags =	(mmDisplayAlternateFE | mmDisplayMetric);

static volatile uint8_t lastPINxState;

static volatile uint16_t watchdogInjectorCount;
static volatile uint16_t watchdogVSSCount;

// these flags specifically tell the system timer0 to do something (v8Timer0CommandIdx)
// main program sets flag, system timer0 usually acknowledges by clearing flag
static const uint8_t t0cResetTimer =			0b10000000;
static const uint8_t t0cResetInputTimer =		0b01000000;
static const uint8_t t0cResetOutputTimer =		0b00100000;
static const uint8_t t0cProcessButton =			0b00010000;
static const uint8_t t0cEnableButtonSampling =	0b00001000;
static const uint8_t t0cResetFEvTime =			0b00000100;
static const uint8_t t0cEnableBLEsample =		0b00000010;
static const uint8_t t0cReadRTC =				0b00000001;

// these flags specifically tell the main program to do something (v8Timer0Status0Idx)
// system timer0 sets flag, main program acknowledges by clearing flag
static const uint8_t t0saTakeSample =			0b10000000;	// tells the main program to perform trip variable sampling
static const uint8_t t0saUpdateDisplay =		0b01000000;
static const uint8_t t0saShowCursor =			0b00100000;
#if defined(useButtonInput)
static const uint8_t t0saReadButton =			0b00010000;
#endif // defined(useButtonInput)
#if defined(useDragRaceFunction)
static const uint8_t t0saAccelTestFlag =		0b00000100;
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
static const uint8_t t0saCoastdownTestFlag =	0b00000010;
#endif // defined(useCoastDownCalculator)
#if defined(useJSONoutput)
static const uint8_t t0saJSONchangeSubtitle =	0b00000001;
#endif // defined(useJSONoutput)

// these flags specifically tell the main program to do something (v8Timer0Status1Idx)
// system timer0 sets flag, main program acknowledges by clearing flag
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t t0sbSampleBLEfriend =		0b10000000;
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDS1307clock)
static const uint8_t t0sbReadRTC =				0b01000000;
#endif // defined(useDS1307clock)
#if defined(useBarFuelEconVsTime)
static const uint8_t t0sbResetFEvsTimeTrip =	0b00100000;
#endif // defined(useBarFuelEconVsTime)

// these status flags inform the main program about MPGuino awake state (v8AwakeIdx)
static const uint8_t aAwakeOnInjector =			0b10000000;
static const uint8_t aAwakeOnVSS =				0b01000000;
static const uint8_t aAwakeOnInput =			0b00100000;
static const uint8_t aAwakeEngineRunning =		0b00010000;
static const uint8_t aAwakeVehicleMoving =		0b00001000;

static const uint8_t aAwake =					(aAwakeOnInjector | aAwakeOnVSS | aAwakeOnInput);
static const uint8_t aAwakeOnVehicle =			(aAwakeOnInjector | aAwakeOnVSS);

// these status flags inform the main program about MPGuino sensor activity (v8ActivityIdx, v8ActivityIdxChangeIdx)
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

// these status flags communicate instantaneous vehicle status between the sensor interrupts and the system timer0 interrupt (v8DirtyIdx)
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

static const uint8_t internalOutputButton =			0b10000000;
static const uint8_t internalProcessButton =		0b01000000;
static const uint8_t internalButtonValid =			0b00100000;
static const uint8_t internalReadTWIrtc =			0b00010000;
static const uint8_t internalReadTWIbutton =		0b00001000;
static const uint8_t internalWriteTWIbyte =			0b00000100;
static const uint8_t internalResetCursorTimer =		0b00000001;

#if defined(useTimer1Interrupt)
// these flags specifically tell the system timer1 to do something (v8Timer1CommandIdx)
// main program sets flag, system timer1 acknowledges by clearing flag
static const uint8_t t1cResetTimer =			0b10000000;
static const uint8_t t1cDelayLCD =				0b01000000;
static const uint8_t t1cEnableDebug =			0b00100000;

static const uint8_t t1cEnableInterrupt =		(t1cDelayLCD | t1cEnableDebug);

// these flags specifically tell the main program to do something (v8Timer1StatusIdx)
// system timer1 sets flag, main program acknowledges by clearing flag
// these flags also have some internal use
#if defined(useTWI4BitLCD)
static const uint8_t t1sLoopFlag =				0b10000000;
static const uint8_t t1sDoOutputTWI =			0b01000000;
#endif // defined(useTWI4BitLCD)

#if defined(useBluetoothAdaFruitSPI)
// bit flags for use with v8BLEstatusIdx
static const uint8_t bleReset =					0b10000000;
static const uint8_t bleResetting =				0b01000000;
static const uint8_t bleAssert =				0b00100000;
static const uint8_t bleAsserting =				0b00010000;
static const uint8_t blePacketWait =			0b00001000;
static const uint8_t blePacketWaiting =			0b00000100;

static const uint8_t bleResetFlags =			(bleReset | bleResetting);
static const uint8_t bleAssertFlags =			(bleAssert | bleAsserting);
static const uint8_t blePacketWaitFlags =		(blePacketWait | blePacketWaiting);
static const uint8_t bleEnableInterrupt =		(bleResetFlags | bleAssertFlags | blePacketWaitFlags);

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useTimer1Interrupt)
