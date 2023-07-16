#if defined(useButtonInput)
typedef void (* bdFunc)(void); // type for display function pointers

typedef struct
{

	uint8_t buttonCode;
	bdFunc buttonCommand;

} buttonVariable;

typedef struct
{

	const buttonVariable (* buttonVariableList);

} buttonVariablePointer;

namespace button /* button input support section prototype */
{

	static void init(void);
	static void shutdown(void);
#if defined(useAnalogButtons) || defined(useDebugButtonInjection)
	static void inject(uint8_t buttonValue);
#endif // defined(useAnalogButtons) || defined(useDebugButtonInjection)
	static void doCommand(void);
	static void noSupport(void);
	static void doNothing(void);
#if defined(useLCDoutput)
	static void doNextBright(void);
#endif // defined(useLCDoutput)
	static void shortLeft(void);
	static void shortRight(void);
	static void longLeft(void);
	static void longRight(void);

};

#if defined(useTWIbuttons)
#if defined(useAdafruitRGBLCDshield)
#define useButtonCrossConfig true
const uint8_t buttonAddress =		0x20;

const uint8_t buttonLbit = 			0b00010000; // GPIO A bit 4, left button
const uint8_t buttonCbit = 			0b00000001; // GPIO A bit 0, select button
const uint8_t buttonRbit = 			0b00000010; // GPIO A bit 1, right button
const uint8_t buttonUbit = 			0b00001000; // GPIO A bit 3, up button
const uint8_t buttonDbit = 			0b00000100; // GPIO A bit 2, down button

const uint8_t longButtonBit =		0b00100000; // GPIO A bit 5 isn't being used for anything right now

const uint8_t buttonMask =			buttonUbit | buttonDbit | buttonLbit | buttonCbit | buttonRbit;

#endif // defined(useAdafruitRGBLCDshield)
	// place any other TWI button definitions here

#endif // defined(useTWIbuttons)
#if defined(useDebugButtons)
#define useButtonCrossConfig true
const uint8_t buttonLbit = 			0b00000001;
const uint8_t buttonCbit = 			0b00000010;
const uint8_t buttonRbit = 			0b00000100;
const uint8_t buttonUbit = 			0b00001000;
const uint8_t buttonDbit = 			0b00010000;

const uint8_t longButtonBit =		0b10000000;

const uint8_t buttonMask =			buttonUbit | buttonDbit | buttonLbit | buttonCbit | buttonRbit;

#endif // defined(useDebugButtons)
#if defined(useAnalogButtons)
#define useButtonCrossConfig true
const uint8_t buttonLbit = 			0b00000001;
const uint8_t buttonCbit = 			0b00000010;
const uint8_t buttonRbit = 			0b00000100;
const uint8_t buttonUbit = 			0b00001000;
const uint8_t buttonDbit = 			0b00010000;

const uint8_t longButtonBit =		0b10000000;

const uint8_t buttonMask =			buttonUbit | buttonDbit | buttonLbit | buttonCbit | buttonRbit;

#endif // defined(useAnalogButtons)
#if defined(useLegacyButtons)
#if defined(__AVR_ATmega2560__)
const uint8_t buttonLbit = 			(1 << PINK3);
const uint8_t buttonCbit = 			(1 << PINK4);
const uint8_t buttonRbit = 			(1 << PINK5);

const uint8_t longButtonBit =		(1 << PINK6); // PINK6 isn't being used for anything right now

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
const uint8_t buttonLbit = 			(1 << PINC3);
const uint8_t buttonCbit = 			(1 << PINC4);
const uint8_t buttonRbit = 			(1 << PINC5);

const uint8_t longButtonBit =		(1 << PINC6); // PINC6 is used as the RESET pin, so this value is safe to use for long-press signalling

#endif // defined(__AVR_ATmega328P__)
const uint8_t buttonMask =			buttonLbit | buttonCbit | buttonRbit;

#endif // defined(useLegacyButtons)
const uint8_t buttonsUp =			0;
const uint8_t btnShortPressL =		buttonLbit;
const uint8_t btnShortPressC =		buttonCbit;
const uint8_t btnShortPressR =		buttonRbit;
const uint8_t btnShortPressLC =		buttonCbit | buttonLbit;
const uint8_t btnShortPressLR =		buttonRbit | buttonLbit;
const uint8_t btnShortPressCR =		buttonRbit | buttonCbit;
const uint8_t btnShortPressLCR =	buttonRbit | btnShortPressLC;

const uint8_t btnLongPressL =		longButtonBit | btnShortPressL;
const uint8_t btnLongPressC =		longButtonBit | btnShortPressC;
const uint8_t btnLongPressLC =		longButtonBit | btnShortPressLC;
const uint8_t btnLongPressR =		longButtonBit | btnShortPressR;
const uint8_t btnLongPressLR =		longButtonBit | btnShortPressLR;
const uint8_t btnLongPressCR =		longButtonBit | btnShortPressCR;
const uint8_t btnLongPressLCR =		longButtonBit | btnShortPressLCR;

#if defined(useButtonCrossConfig)
const uint8_t btnShortPressU =		buttonUbit;
const uint8_t btnShortPressUL =		buttonUbit | btnShortPressL;
const uint8_t btnShortPressUC =		buttonUbit | btnShortPressC;
const uint8_t btnShortPressULC =	buttonUbit | btnShortPressLC;
const uint8_t btnShortPressUR =		buttonUbit | btnShortPressR;
const uint8_t btnShortPressULR =	buttonUbit | btnShortPressLR;
const uint8_t btnShortPressUCR =	buttonUbit | btnShortPressCR;
const uint8_t btnShortPressULCR =	buttonUbit | btnShortPressLCR;

const uint8_t btnShortPressD =		buttonDbit;
const uint8_t btnShortPressDL =		buttonDbit | btnShortPressL;
const uint8_t btnShortPressDC =		buttonDbit | btnShortPressC;
const uint8_t btnShortPressDLC =	buttonDbit | btnShortPressLC;
const uint8_t btnShortPressDR =		buttonDbit | btnShortPressR;
const uint8_t btnShortPressDLR =	buttonDbit | btnShortPressLR;
const uint8_t btnShortPressDCR =	buttonDbit | btnShortPressCR;
const uint8_t btnShortPressDLCR =	buttonDbit | btnShortPressLCR;

const uint8_t btnShortPressUD =		buttonDbit | btnShortPressU;
const uint8_t btnShortPressUDL =	buttonDbit | btnShortPressUL;
const uint8_t btnShortPressUDC =	buttonDbit | btnShortPressUC;
const uint8_t btnShortPressUDLC =	buttonDbit | btnShortPressULC;
const uint8_t btnShortPressUDR =	buttonDbit | btnShortPressUR;
const uint8_t btnShortPressUDLR =	buttonDbit | btnShortPressULR;
const uint8_t btnShortPressUDCR =	buttonDbit | btnShortPressUCR;
const uint8_t btnShortPressUDCRL =	buttonDbit | btnShortPressULCR;

const uint8_t btnLongPressU =		longButtonBit | btnShortPressU;
const uint8_t btnLongPressUL =		longButtonBit | btnShortPressUL;
const uint8_t btnLongPressUC =		longButtonBit | btnShortPressUC;
const uint8_t btnLongPressULC =		longButtonBit | btnShortPressULC;
const uint8_t btnLongPressUR =		longButtonBit | btnShortPressUR;
const uint8_t btnLongPressULR =		longButtonBit | btnShortPressULR;
const uint8_t btnLongPressUCR =		longButtonBit | btnShortPressUCR;
const uint8_t btnLongPressULCR =	longButtonBit | btnShortPressULCR;

const uint8_t btnLongPressD =		longButtonBit | btnShortPressD;
const uint8_t btnLongPressDL =		longButtonBit | btnShortPressDL;
const uint8_t btnLongPressDC =		longButtonBit | btnShortPressDC;
const uint8_t btnLongPressDLC =		longButtonBit | btnShortPressDLC;
const uint8_t btnLongPressDR =		longButtonBit | btnShortPressDR;
const uint8_t btnLongPressDLR =		longButtonBit | btnShortPressDLR;
const uint8_t btnLongPressDCR =		longButtonBit | btnShortPressDCR;
const uint8_t btnLongPressDLCR =	longButtonBit | btnShortPressDLCR;

const uint8_t btnLongPressUD =		longButtonBit | btnShortPressUD;
const uint8_t btnLongPressUDL =		longButtonBit | btnShortPressUDL;
const uint8_t btnLongPressUDC =		longButtonBit | btnShortPressUDC;
const uint8_t btnLongPressUDLC =	longButtonBit | btnShortPressUDLC;
const uint8_t btnLongPressUDR =		longButtonBit | btnShortPressUDR;
const uint8_t btnLongPressUDLR =	longButtonBit | btnShortPressUDLR;
const uint8_t btnLongPressUDCR =	longButtonBit | btnShortPressUDCR;
const uint8_t btnLongPressUDCRL =	longButtonBit | btnShortPressUDCRL;

#endif // defined(useButtonCrossConfig)
#if defined(useParallax5PositionSwitch)
const unsigned int analogButtonThreshold[] PROGMEM = {
	0,		// 00
	559,	// 01
	588,	// 02
	618,	// 04
	645,	// 03
	653,	// 05
	664,	// 06
	700,	// 0A
	725,	// 0B
	750,	// 07
	786,	// 0C
	805,	// 0D
	820,	// 09
	834,	// 0E
	858,	// 0F
	897,	// 10
	927,	// 11
	980		// 12
};

const uint8_t analogButtonCount = (sizeof(analogButtonThreshold) / sizeof(unsigned int));

//	btnShortPressDC,	// 08

const uint8_t analogTranslate[(uint16_t)(analogButtonCount)] PROGMEM = {
	buttonsUp,			// 00
	btnShortPressULC,	// 01
	btnShortPressUL,	// 02
	btnShortPressDLC,	// 04
	btnShortPressLC,	// 03
	btnShortPressDL,	// 05
	btnShortPressL,		// 06
	btnShortPressUCR,	// 0A
	btnShortPressUR,	// 0B
	btnShortPressUC,	// 07
	btnShortPressU,		// 0C
	btnShortPressDCR,	// 0D
	btnShortPressCR,	// 09
	btnShortPressDR,	// 0E
	btnShortPressR,		// 0F
	btnShortPressC,		// 10
	btnShortPressD,		// 11
	buttonsUp			// 12
};

#endif // defined(useParallax5PositionSwitch)
#if defined(useAnalogMuxButtons)
const unsigned int analogButtonThreshold[] PROGMEM = {
	0,		// 00
	556,	// 01
	560,	// 02
	567,	// 03
	574,	// 04
	583,	// 05
	593,	// 06
	601,	// 07
	609,	// 08
	621,	// 09
	634,	// 0A
	644,	// 0B
	653,	// 0C
	665,	// 0D
	677,	// 0E
	687,	// 0F
	698,	// 10
	722,	// 11
	747,	// 12
	759,	// 13
	772,	// 14
	789,	// 15
	806,	// 16
	820,	// 17
	835,	// 18
	859,	// 19
	884,	// 1A
	902,	// 1B
	921,	// 1C
	944,	// 1D
	968,	// 1E
	989,	// 1F
	1012	// 20
};

const uint8_t analogButtonCount = (sizeof(analogButtonThreshold) / sizeof(unsigned int));

const uint8_t analogTranslate[(uint16_t)(analogButtonCount)] PROGMEM = {
	buttonsUp,
	btnShortPressUDCRL,
	btnShortPressULCR,
	btnShortPressDLCR,
	btnShortPressLCR,
	btnShortPressUDLC,
	btnShortPressULC,
	btnShortPressDLC,
	btnShortPressLC,
	btnShortPressUDLR,
	btnShortPressULR,
	btnShortPressDLR,
	btnShortPressLR,
	btnShortPressUDL,
	btnShortPressUL,
	btnShortPressDL,
	btnShortPressL,
	btnShortPressUDCR,
	btnShortPressUCR,
	btnShortPressDCR,
	btnShortPressCR,
	btnShortPressUDC,
	btnShortPressUC,
	btnShortPressDC,
	btnShortPressC,
	btnShortPressUDR,
	btnShortPressUR,
	btnShortPressDR,
	btnShortPressR,
	btnShortPressUD,
	btnShortPressU,
	btnShortPressD,
	buttonsUp
};

#endif // defined(useAnalogMuxButtons)
#if defined(useLCDoutput)
static const char brightMsg[] PROGMEM = {
	"Backlight = " tcEOS
};

#if defined(useBinaryLCDbrightness)
static const char brightString[] PROGMEM = {
	"ON" tcLCDLO tcEOS
	"OFF" tcLCDOFF tcEOS
};

#else // defined(useBinaryLCDbrightness)
static const char brightString[] PROGMEM = {
	"LOW" tcLCDLO tcEOS
	"MED" tcLCDMED tcEOS
	"HIGH" tcLCDHI tcEOS
	"OFF" tcLCDOFF tcEOS
};

#endif // defined(useBinaryLCDbrightness)
#endif // defined(useLCDoutput)
#endif // defined(useButtonInput)
