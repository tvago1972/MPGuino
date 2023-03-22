#ifdef useSerial0Port
namespace serial0
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#ifdef useSerial0PortInput
	static uint8_t chrIn(void);
#endif // useSerial0PortInput

};

interfaceDevice devSerial0;

#ifdef useBufferedSerial0Port
ringBufferVariable serial0Buffer;

volatile uint8_t serial0Data[32];

#endif // useBufferedSerial0Port
#ifdef useSerial0PortInput
ringBufferVariable serial0InputBuffer;

volatile uint8_t serial0InputData[16];

#endif // useSerial0PortInput
#endif // useSerial0Port
#ifdef useSerial1Port
namespace serial1
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
#ifdef useSerial1PortInput
	static uint8_t chrIn(void);
#endif // useSerial1PortInput

};

interfaceDevice devSerial1;

#ifdef useBufferedSerial1Port
ringBufferVariable serial1Buffer;

volatile uint8_t serial1Data[32];

#endif // useBufferedSerial1Port
#ifdef useSerial1PortInput
ringBufferVariable serial1InputBuffer;

volatile uint8_t serial1InputData[16];

#endif // useSerial1PortInput
#endif // useSerial1Port
