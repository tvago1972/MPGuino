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

volatile uint8_t serial0Data[32];

#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
ringBufferVariable serial0InputBuffer;

volatile uint8_t serial0InputData[16];

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

volatile uint8_t serial1Data[32];

#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
ringBufferVariable serial1InputBuffer;

volatile uint8_t serial1InputData[16];

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

volatile uint8_t serial2Data[32];

#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
ringBufferVariable serial2InputBuffer;

volatile uint8_t serial2InputData[16];

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

volatile uint8_t serial3Data[32];

#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
ringBufferVariable serial3InputBuffer;

volatile uint8_t serial3InputData[16];

#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)
