#if defined(useTouchScreenInput)
namespace touch /* XPT2046 resistive touch (software SPI) support section prototype */
{

	static void init(void);
	static uint8_t pressed(void);
	static uint8_t sample(uint16_t * rawX, uint16_t * rawY);
	static uint8_t read(uint16_t * x, uint16_t * y);
	static void loadCalibration(void);
#if defined(useTFToutput)
	static void calibEnter(void);	// start (non-blocking) 4-corner calibration: set orientation, draw the first crosshair
	static uint8_t calibPoll(void);	// call every pass while calibrating; returns 1 once all 4 corners are captured + stored
#endif // defined(useTFToutput)
#if defined(useDebugTerminal) && defined(useTFToutput)
	static void testLoop(void);
#endif // defined(useDebugTerminal) && defined(useTFToutput)

};

#if defined(useTFToutput)
namespace keypad /* on-screen numeric keypad (non-blocking TFT screen state) prototype */
{

	static void open(uint32_t maxValue, uint32_t initialValue, const char * title);	// set up + draw
	static uint8_t tap(uint16_t px, uint16_t py);	// handle one tap; returns 1 when OK was tapped (value() ready)
	static uint8_t longPress(uint16_t px, uint16_t py);	// handle a long-press; returns 1 if DEL-cancel
	static uint32_t value(void);					// the entered value (after tap() returns 1)
	static void draw(void);
	static void drawKey(uint8_t index, uint8_t highlight);
	static void drawEntry(void);

};

// 4-row x 3-col layout. labels double as key codes: '0'..'9' are digits, 'C'
// short-press deletes the digit left of the caret / long-press cancels, 'E'
// accepts. the caret is moved by tapping inside the entry box.
static const uint8_t keypadCols =		3;
static const uint8_t keypadRows =		4;
static const uint8_t keypadKeys =		(keypadCols * keypadRows);
static const uint8_t keypadMaxDigits =	10;				// full uint32_t width (4294967295); per-call maxValue clamps the range
static const uint8_t keypadLabelScale =	3;				// glyph scale for key labels / entry
static const uint8_t keypadGap =		4;				// px between keys and around the grid
static const uint8_t keypadActionGap =	8;				// extra px separating the DEL/0/OK row from the digits

// row-major key codes: 1-9, then Delete / 0 / Enter
static const char keypadLabels[] PROGMEM = "123456789C0E";

// RGB565 keypad palette
static const uint16_t keypadKeyColour =		0x4208;		// key face (dark grey)
static const uint16_t keypadKeyDownColour =	0x05BF;		// pressed key face (cyan-ish)
static const uint16_t keypadBorderColour =	0xFFFF;		// key border / entry box border
static const uint16_t keypadEntryColour =	0xFFE0;		// entry text (yellow) on black

static uint16_t keypadKeyW;								// computed key width  (px)
static uint16_t keypadKeyH;								// computed key height (px)
static uint16_t keypadGridTop;							// y of the first key row
static uint16_t keypadEntryTop;							// y of the entry box (below the optional title)
static uint16_t keypadEntryH;							// height of the entry box
static const char * keypadTitle;						// optional PROGMEM title shown above the entry box (0 = none)

static char keypadEntry[keypadMaxDigits + 1];			// edited digit string (NUL terminated)
static uint8_t keypadEntryLen;							// digits currently entered
static uint8_t keypadCursor;							// caret position (0..keypadEntryLen) for insert/backspace
static uint32_t keypadMaxValue;							// upper bound; a digit that would exceed it is ignored
#endif // defined(useTFToutput)

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
// axis. these are cached in RAM from the pTouchRaw* EEPROM parameters at touch::init
// and rewritten by the 'J' corner-calibration routine; defaults live in params[].
static uint16_t touchRawXlo;
static uint16_t touchRawXhi;
static uint16_t touchRawYlo;
static uint16_t touchRawYhi;

static uint16_t touchRawX;								// last averaged raw X (0..4095)
static uint16_t touchRawY;								// last averaged raw Y (0..4095)

#endif // defined(useTouchScreenInput)
