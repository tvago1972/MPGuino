#if defined(useOutputPins)
namespace outputPin
{

	static void init(void);
	static void shutdown(void);
	static void setOutputPin1(uint8_t pin);
	static void setOutputPin2(uint8_t pin);

};

#endif // defined(useOutputPins)
