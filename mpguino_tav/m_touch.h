#if defined(useTouchScreenInput)
namespace touch /* XPT2046 resistive touch (software SPI) support section prototype */
{

	static void init(void);
	static uint8_t pressed(void);
	static uint8_t sample(uint16_t * rawX, uint16_t * rawY);
	static uint8_t read(uint16_t * x, uint16_t * y);
#if defined(useDebugTerminal) && defined(useTFToutput)
	static void testLoop(void);
#endif // defined(useDebugTerminal) && defined(useTFToutput)

};

#if defined(useMPGuinoColourTouch)
// XPT2046 wired to bit-banged GPIO on the MPGuino Colour Touch (ATmega2560):
//   T_CLK = PB5, T_CS = PB4, T_DIN (MOSI) = PH6, T_DOUT (MISO) = PH5, T_IRQ = PH4
// (the board does not route these to the hardware SPI bus, so they are bit-banged;
//  only touchWriteByte/touchReadBits below would change if a revision shared the bus)
#define TOUCH_CLK_HI()	(PORTB |= (1 << PORTB5))
#define TOUCH_CLK_LO()	(PORTB &= ~(1 << PORTB5))
#define TOUCH_CS_HI()	(PORTB |= (1 << PORTB4))
#define TOUCH_CS_LO()	(PORTB &= ~(1 << PORTB4))
#define TOUCH_DIN_HI()	(PORTH |= (1 << PORTH6))
#define TOUCH_DIN_LO()	(PORTH &= ~(1 << PORTH6))
#define TOUCH_DOUT_SET	(PINH & (1 << PINH5))			// data bit read from the XPT2046
#define TOUCH_IRQ_DOWN	(!(PINH & (1 << PINH4)))		// pen-down: PENIRQ pulled low
#endif // defined(useMPGuinoColourTouch)

static const uint8_t touchCmdX =		0x90;			// XPT2046 control byte: read X channel
static const uint8_t touchCmdY =		0xD0;			// XPT2046 control byte: read Y channel
static const uint8_t touchSamples =		8;				// averaged samples per read

// raw ADC calibration (panel-fixed), measured at the screen corners in landscape
// orientation 3: rawX runs along the long (320px) axis, rawY along the short (240px)
// axis. tune these if a corner crosshair does not land under the finger.
static const uint16_t touchRawXlo =		907;
static const uint16_t touchRawXhi =		3453;
static const uint16_t touchRawYlo =		743;
static const uint16_t touchRawYhi =		2884;

static uint16_t touchRawX;								// last averaged raw X (0..4095)
static uint16_t touchRawY;								// last averaged raw Y (0..4095)

#endif // defined(useTouchScreenInput)
