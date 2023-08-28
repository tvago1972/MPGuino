#if defined(useSerial0Port)
namespace serial0
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#if defined(useSerial0PortInput)
	static uint8_t chrIn(void);
#endif // defined(useSerial0PortInput)

};

interfaceDevice devSerial0;

#if defined(useBufferedSerial0Port)
ringBufferVariable serial0Buffer;

static const uint16_t serial0DataSize = 32;
static uint8_t serial0Data[serial0DataSize];

#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
ringBufferVariable serial0InputBuffer;

static const uint16_t serial0InputDataSize = 64;
static uint8_t serial0InputData[serial0InputDataSize];

#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)

#if defined(useSerial1Port)
namespace serial1
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#if defined(useSerial1PortInput)
	static uint8_t chrIn(void);
#endif // defined(useSerial1PortInput)

};

interfaceDevice devSerial1;

#if defined(useBufferedSerial1Port)
ringBufferVariable serial1Buffer;

static const uint16_t serial1DataSize = 32;
static uint8_t serial1Data[serial1DataSize];

#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
ringBufferVariable serial1InputBuffer;

static const uint16_t serial1InputDataSize = 64;
static uint8_t serial1InputData[serial1InputDataSize];

#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
namespace serial2
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#if defined(useSerial2PortInput)
	static uint8_t chrIn(void);
#endif // defined(useSerial2PortInput)

};

interfaceDevice devSerial2;

#if defined(useBufferedSerial2Port)
ringBufferVariable serial2Buffer;

static const uint16_t serial2DataSize = 32;
static uint8_t serial2Data[serial2DataSize];

#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
ringBufferVariable serial2InputBuffer;

static const uint16_t serial2InputDataSize = 64;
static uint8_t serial2InputData[serial2InputDataSize];

#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
namespace serial3
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#if defined(useSerial3PortInput)
	static uint8_t chrIn(void);
#endif // defined(useSerial3PortInput)

};

interfaceDevice devSerial3;

#if defined(useBufferedSerial3Port)
ringBufferVariable serial3Buffer;

static const uint16_t serial3DataSize = 32;
static uint8_t serial3Data[serial3DataSize];

#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
ringBufferVariable serial3InputBuffer;

static const uint16_t serial3InputDataSize = 64;
static uint8_t serial3InputData[serial3InputDataSize];

#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)
