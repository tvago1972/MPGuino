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

#endif // defined(useSerial3Port)
