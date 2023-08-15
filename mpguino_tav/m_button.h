#if defined(useButtonInput)
typedef void (* bdFunc)(void); // type for display function pointers

typedef struct
{

	uint8_t buttonCode;
	bdFunc buttonCommand;

} buttonVariable;

typedef uint8_t (* displayHandlerFunc)(uint8_t, uint8_t); // type for display handler functions

typedef struct
{

	uint8_t baseDisplayIdx;
	uint8_t displayGroupCount;
	uint8_t displayPageCount;
	uint8_t displayFlags;
	displayHandlerFunc displayHandlerPtr;
	const buttonVariable (* buttonList);

} displayData;

namespace menu /* Top-down menu selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void entry(void);
	static void select(void);
	static void exit(void);

};

namespace button /* button input support section prototype */
{

	static void init(void);
	static void shutdown(void);
#if defined(useAnalogButtons) || defined(useDebugButtonInjection)
	static void inject(uint8_t buttonValue);
#endif // defined(useAnalogButtons) || defined(useDebugButtonInjection)

};

namespace cursor /* LCD screen cursor manipulation section prototype */
{

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
#if LCDcharHeight == 4
	static void transferDisplay(void);
#endif // LCDcharHeight == 4
	static void screenLevelEntry(const char * str, uint8_t newScreenLevel);
	static void screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel);
	static void moveAbsolute(uint8_t positionY, uint8_t positionX);
	static void moveRelative(uint8_t moveY, uint8_t moveX);
	static void updateAfterMove(uint8_t displayIdxChange);
	static void updateDisplay(uint8_t thisDisplayIdx, uint8_t cmd);

};

static uint8_t workingDisplayIdx;
static uint8_t menuTitlesOffset;
static displayHandlerFunc menuHandlerPtr;
static uint8_t menuIdx;
static uint8_t thisMenuHeight;
static uint8_t menuTop;
static uint8_t menuLength;
static uint8_t displayHeight;

static const uint8_t dfFullScreen =			0b10000000;		// tells whether display uses entire screen or not
static const uint8_t dfSplitScreen =		0b01000000;		// allows display to go to either top or bottom screen
static const uint8_t dfUsesCGRAM =			0b00100000;		// tells whether display uses LCD CGRAM or not
static const uint8_t dfUsesCGRAMfont =		0b00010000;		// tells whether display CGRAM usage is dynamically generated or a stored font
static const uint8_t dfCGRAMfontMask =		0b00001111;		// if stored CGRAM font, tells font number

static uint8_t callingDisplayIdx;
static const uint8_t displayInitialEntryIdx =	0;								// typically, this call will fall through
static const uint8_t displayCursorUpdateIdx =	displayInitialEntryIdx + 1;		// ...to this call, which will then will fall through
static const uint8_t displayOutputIdx =			displayCursorUpdateIdx + 1;		// ...to this call

static const uint8_t menuInitialEntryIdx =		displayOutputIdx + 1;			// if the associated menu cursor is reset upon entry
static const uint8_t menuFirstLineOutIdx =		menuInitialEntryIdx + 1;		// displays first line for menu
static const uint8_t menuSecondLineInitIdx =	menuFirstLineOutIdx + 1;		// if the associated menu entry has additional info, initializes info
static const uint8_t menuSecondLineFlagIdx =	menuSecondLineInitIdx + 1;		// if the associated menu entry has additional info, returns a 1
static const uint8_t menuSecondLineOutIdx =		menuSecondLineFlagIdx + 1;		// if the associated menu entry has additional info, displays info line
static const uint8_t menuDoSelectionIdx =		menuSecondLineOutIdx + 1;		// performs optional defined associated action, and returns next display index
static const uint8_t menuExitIdx =				menuDoSelectionIdx + 1;			// performs optional defined exit action, and returns next display index

#if defined(useSpiffyTripLabels)
static const uint8_t dfMainDisplay =			dfSplitScreen | dfUsesCGRAM;
#else // defined(useSpiffyTripLabels)
static const uint8_t dfMainDisplay =			dfSplitScreen;
#endif // defined(useSpiffyTripLabels)
#if defined(useStatusMeter)
static const uint8_t dfStatusMeterDisplay =		dfSplitScreen | dfUsesCGRAM;
#endif // defined(useStatusMeter)
#if defined(useBigFE)
static const uint8_t dfBigFEdisplay =			dfSplitScreen | dfUsesCGRAM | dfUsesCGRAMfont;
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
static const uint8_t dfBarFEvTdisplay =			dfSplitScreen | dfUsesCGRAM;
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t dfBarFEvSdisplay =			dfSplitScreen | dfUsesCGRAM;
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
static const uint8_t dfBigDTEdisplay =			dfSplitScreen | dfUsesCGRAM | dfUsesCGRAMfont;
#endif // defined(useBigDTE)
#if defined(useBigTTE)
static const uint8_t dfBigTTEdisplay =			dfSplitScreen | dfUsesCGRAM | dfUsesCGRAMfont;
#endif // defined(useBigTTE)
#if defined(useCPUreading)
static const uint8_t dfCPUmonDisplay =			dfSplitScreen;
#endif // defined(useCPUreading)
#if defined(useClockDisplay)
static const uint8_t dfClockShowDisplay =		dfSplitScreen | dfUsesCGRAM | dfUsesCGRAMfont;
#endif // defined(useClockDisplay)

static uint8_t displayCursor[(uint16_t)(displayCountTotal)];
static uint8_t menuHeight[(uint16_t)(displayCountMenu)];

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
/* button input support section */

static const buttonVariable bpListMenu[] PROGMEM = {
	 {btnShortPressC,	menu::select}
	,{btnLongPressL,	menu::exit}
#if defined(useLCDoutput)
	,{btnLongPressC,	cursor::doNextBright}
#endif // defined(useLCDoutput)
#if defined(useButtonCrossConfig)
		,{btnShortPressU,	cursor::shortLeft}
		,{btnShortPressD,	cursor::shortRight}
		,{btnShortPressL,	menu::exit}
		,{btnShortPressR,	menu::select}
#else // defined(useButtonCrossConfig)
		,{btnShortPressL,	cursor::shortLeft}
		,{btnShortPressR,	cursor::shortRight}
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

static const buttonVariable bpListMainDisplay[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#if LCDcharHeight == 4
	,{btnLongPressC,	cursor::transferDisplay}
#endif // LCDcharHeight == 4
#if defined(useScreenEditor)
	,{btnShortPressLR,	displayEdit::entry}
#endif // defined(useScreenEditor)
#if defined(useButtonCrossConfig)
		,{btnShortPressD,	cursor::longRight}
	#if defined(useLCDoutput)
		,{btnShortPressU,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
	#if defined(useExpandedMainDisplay)
		,{btnShortPressC,	mainDisplay::goToMenu}
	#else // defined(useExpandedMainDisplay)
		,{btnShortPressC,	menu::entry}
	#endif // defined(useExpandedMainDisplay)
	#if defined(useEnhancedTripReset)
		,{btnShortPressUL,	tripSave::goSaveTank}
		,{btnLongPressUL,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressUR,	tripSave::goSaveCurrent}
		,{btnLongPressUR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#if defined(useCPUreading)
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#else // defined(useButtonCrossConfig)
	#if defined(useLCDoutput)
		,{btnShortPressC,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
	#if defined(useExpandedMainDisplay)
		,{btnShortPressLCR,	mainDisplay::goToMenu}
	#else // defined(useExpandedMainDisplay)
		,{btnShortPressLCR,	menu::entry}
	#endif // defined(useExpandedMainDisplay)
	#if defined(useEnhancedTripReset)
		,{btnShortPressLC,	tripSave::goSaveTank}
		,{btnLongPressLC,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressCR,	tripSave::goSaveCurrent}
		,{btnLongPressCR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#if defined(useCPUreading)
		,{btnLongPressLCR,	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

#if defined(useScreenEditor)
static const buttonVariable bpListMainDisplayEdit[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	displayEdit::cancel}
	,{btnLongPressLR,	displayEdit::cancel}
	,{btnLongPressC,	displayEdit::set}
	,{btnShortPressLR,	displayEdit::set}
	,{btnLongPressR,	displayEdit::readInitial}
#if defined(useButtonCrossConfig)
		,{btnShortPressU,	displayEdit::changeItemUp}
		,{btnShortPressD,	displayEdit::changeItemDown}
		,{btnShortPressC,	displayEdit::set}
#else // defined(useButtonCrossConfig)
		,{btnShortPressC,	displayEdit::changeItemUp}
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

#endif // defined(useScreenEditor)
static const buttonVariable bpListParameterEdit[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	parameterEdit::cancel}
	,{btnLongPressLR,	parameterEdit::cancel}
	,{btnLongPressC,	parameterEdit::save}
	,{btnShortPressLR,	parameterEdit::save}
	,{btnLongPressR,	parameterEdit::readInitial}
#if defined(useButtonCrossConfig)
		,{btnShortPressUL,	parameterEdit::findLeft}
		,{btnShortPressUR,	parameterEdit::findRight}
		,{btnLongPressD,	parameterEdit::readMinValue}
		,{btnLongPressU,	parameterEdit::readMaxValue}
		,{btnShortPressU,	parameterEdit::changeDigitUp}
		,{btnShortPressD,	parameterEdit::changeDigitDown}
		,{btnShortPressC,	parameterEdit::save}
#else // defined(useButtonCrossConfig)
		,{btnShortPressLC,	parameterEdit::findLeft}
		,{btnShortPressCR,	parameterEdit::findRight}
		,{btnLongPressCR,	parameterEdit::readMinValue}
		,{btnLongPressLC,	parameterEdit::readMaxValue}
		,{btnShortPressC,	parameterEdit::changeDigitUp}
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

#if defined(useBigDigitDisplay) || defined(useStatusMeter) || defined(useCPUreading) || defined(useBarGraph)
static const buttonVariable bpListSecondaryDisplay[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#if LCDcharHeight == 4
	,{btnLongPressC,	cursor::transferDisplay}
#endif // LCDcharHeight == 4
#if defined(useButtonCrossConfig)
		,{btnShortPressD,	cursor::longRight}
	#if defined(useLCDoutput)
		,{btnShortPressU,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
	#if defined(useExpandedMainDisplay)
		,{btnShortPressC,	mainDisplay::goToMenu}
	#else // defined(useExpandedMainDisplay)
		,{btnShortPressC,	menu::entry}
	#endif // defined(useExpandedMainDisplay)
	#if defined(useEnhancedTripReset)
		,{btnShortPressUL,	tripSave::goSaveTank}
		,{btnLongPressUL,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressUR,	tripSave::goSaveCurrent}
		,{btnLongPressUR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#if defined(useCPUreading)
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#else // defined(useButtonCrossConfig)
	#if defined(useLCDoutput)
		,{btnShortPressC,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
	#if defined(useExpandedMainDisplay)
		,{btnShortPressLCR,	mainDisplay::goToMenu}
	#else // defined(useExpandedMainDisplay)
		,{btnShortPressLCR,	menu::entry}
	#endif // defined(useExpandedMainDisplay)
	#if defined(useEnhancedTripReset)
		,{btnShortPressLC,	tripSave::goSaveTank}
		,{btnLongPressLC,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressCR,	tripSave::goSaveCurrent}
		,{btnLongPressCR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#if defined(useCPUreading)
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

#endif // defined(useBigDigitDisplay) || defined(useStatusMeter) || defined(useCPUreading) || defined(useBarGraph)
#if defined(useClockDisplay)
static const buttonVariable bpListClockDisplay[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
	,{btnLongPressC,	clockSet::entry}
	,{btnShortPressLR,	clockSet::entry}
#if defined(useButtonCrossConfig)
		,{btnShortPressD,	cursor::longRight}
	#if defined(useLCDoutput)
		,{btnShortPressU,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
	#if defined(useExpandedMainDisplay)
		,{btnShortPressC,	mainDisplay::goToMenu}
	#else // defined(useExpandedMainDisplay)
		,{btnShortPressC,	menu::entry}
	#endif // defined(useExpandedMainDisplay)
	#if defined(useEnhancedTripReset)
		,{btnShortPressUL,	tripSave::goSaveTank}
		,{btnLongPressUL,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressUR,	tripSave::goSaveCurrent}
		,{btnLongPressUR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#if defined(useCPUreading)
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#else // defined(useButtonCrossConfig)
	#if defined(useLCDoutput)
		,{btnShortPressC,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
	#if defined(useExpandedMainDisplay)
		,{btnShortPressLCR,	mainDisplay::goToMenu}
	#else // defined(useExpandedMainDisplay)
		,{btnShortPressLCR,	menu::entry}
	#endif // defined(useExpandedMainDisplay)
	#if defined(useEnhancedTripReset)
		,{btnShortPressLC,	tripSave::goSaveTank}
		,{btnLongPressLC,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressCR,	tripSave::goSaveCurrent}
		,{btnLongPressCR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#if defined(useCPUreading)
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

static const buttonVariable bpListClockSet[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	clockSet::cancel}
	,{btnLongPressLR,	clockSet::cancel}
	,{btnLongPressC,	clockSet::set}
	,{btnShortPressLR,	clockSet::set}
#if defined(useButtonCrossConfig)
		,{btnShortPressU,	clockSet::changeDigitUp}
		,{btnShortPressD,	clockSet::changeDigitDown}
		,{btnShortPressC,	clockSet::set}
#else // defined(useButtonCrossConfig)
		,{btnShortPressC,	clockSet::changeDigitUp}
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

#endif // defined(useClockDisplay)
#if defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog) || defined(useDragRaceFunction) || defined(useCoastDownCalculator)
static const buttonVariable bpListMiscViewer[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::entry}
#if defined(useButtonCrossConfig)
	#if defined(useLCDoutput)
		,{btnShortPressU,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
	#if defined(useCPUreading)
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#else // defined(useButtonCrossConfig)
	#if defined(useLCDoutput)
		,{btnShortPressC,	cursor::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
	#if defined(useCPUreading)
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // defined(useCPUreading)
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		cursor::noSupport}
};

#endif // defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog) || defined(useDragRaceFunction) || defined(useCoastDownCalculator)
#if defined(useTestButtonValues)
static const buttonVariable bpListButtonView[] PROGMEM = {
	 {buttonsUp,		cursor::doNothing}
};

#endif // defined(useTestButtonValues)
static const displayData displayParameters[(uint16_t)(displayCountTotal)] PROGMEM = {

	 {baseMenuDisplayIdx,			1,					displayCountBase,				0,								baseMenu::menuHandler,				bpListMenu}
#if defined(useExpandedMainDisplay)
	,{mainMenuDisplayIdx,			1,					displayCountUser,				0,								mainDisplay::menuHandler,			bpListMenu}
#endif // defined(useExpandedMainDisplay)
	,{settingsMenuDisplayIdx,		1,					displayCountSettings,			0,								settings::menuHandler,				bpListMenu}

// the following display entries are for the various EEPROM parameter settings, grouped by function

	,{displaySettingsDisplayIdx,	1,					displayCountSettingsDisplay,	displayStartSettingsDisplay,	parameterEdit::menuHandler,			bpListMenu}
	,{fuelSettingsDisplayIdx,		1,					displayCountSettingsFuel,		displayStartSettingsFuel,		parameterEdit::menuHandler,			bpListMenu}
	,{VSSsettingsDisplayIdx,		1,					displayCountSettingsVSS,		displayStartSettingsVSS,		parameterEdit::menuHandler,			bpListMenu}
	,{tankSettingsDisplayIdx,		1,					displayCountSettingsTank,		displayStartSettingsTank,		parameterEdit::menuHandler,			bpListMenu}
#if defined(useChryslerMAPCorrection)
	,{CRFICsettingsDisplayIdx,		1,					displayCountSettingsCRFIC,		displayStartSettingsCRFIC,		parameterEdit::menuHandler,			bpListMenu}
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	,{acdSettingsDisplayIdx,		1,					displayCountSettingsVehicle,	displayStartSettingsVehicle,	parameterEdit::menuHandler,			bpListMenu}
#endif // defined(useVehicleParameters)
	,{timeoutSettingsDisplayIdx,	1,					displayCountSettingsTimeout,	displayStartSettingsTimeout,	parameterEdit::menuHandler,			bpListMenu}
	,{miscSettingsDisplayIdx,		1,					displayCountSettingsMisc,		displayStartSettingsMisc,		parameterEdit::menuHandler,			bpListMenu}

// the following display entries are for any otherwise unlinked menu groups

#if defined(useSavedTrips)
	,{tripSaveCurrentDisplayIdx,	1,					displayCountTripSaveCurrent,	displayStartTripSaveCurrent,	tripSave::menuHandler,				bpListMenu}
#endif // defined(useSavedTrips)
#if defined(useEnhancedTripReset)
	,{tripSaveTankDisplayIdx,		1,					displayCountTripSaveTank,		displayStartTripSaveTank,		tripSave::menuHandler,				bpListMenu}
#endif // defined(useEnhancedTripReset)

// the following display entries are for options selected via configs.h

#if defined(useDragRaceFunction)
	,{dragRaceMenuDisplayIdx,		1,					6,								0,								accelerationTest::menuHandler,		bpListMenu}
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	,{coastdownMenuDisplayIdx,		1,					8,								0,								coastdown::menuHandler,				bpListMenu}
#endif // defined(useCoastDownCalculator)

// this is the end of the display entry group handled as menu

#if defined(useSimulatedFIandVSS)
	,{signalSimDisplayIdx,			1,					4,								0,								signalSim::displayHandler,			bpListMiscViewer}
#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
	,{pressureDisplayIdx,			1,					1,								0,								pressureCorrect::displayHandler,	bpListMiscViewer}
#endif // defined(useChryslerMAPCorrection)
#if defined(useDebugAnalog)
	,{analogDisplayIdx,				1,					1,								0,								analogReadViewer::displayHandler,	bpListMiscViewer}
#endif // defined(useDebugAnalog)
#if defined(useTestButtonValues)
	,{buttonDisplayIdx,				1,					1,								0,								buttonView::displayHandler,			bpListMiscViewer}
#endif // defined(useTestButtonValues)

// the following display entries are for the various main displays

	,{mainDisplayIdx,				displayCountUser,	displayCountMain,				dfMainDisplay,					mainDisplay::displayHandler,		bpListMainDisplay}
#if defined(useStatusMeter)
	,{mainDisplayIdx,				displayCountUser,	2,								dfStatusMeterDisplay,			statusBar::displayHandler,			bpListSecondaryDisplay}
#endif // defined(useStatusMeter)
#if defined(useBigFE)
	,{mainDisplayIdx,				displayCountUser,	3,								dfBigFEdisplay,					bigDigit::displayHandler,			bpListSecondaryDisplay}
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
	,{mainDisplayIdx,				displayCountUser,	4,								dfBarFEvTdisplay,				barGraphSupport::displayHandler,	bpListSecondaryDisplay}
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	,{mainDisplayIdx,				displayCountUser,	3,								dfBarFEvSdisplay,				barGraphSupport::displayHandler,	bpListSecondaryDisplay}
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
	,{mainDisplayIdx,				displayCountUser,	3,								dfBigDTEdisplay,				bigDigit::displayHandler,			bpListSecondaryDisplay}
#endif // defined(useBigDTE)
#if defined(useBigTTE)
	,{mainDisplayIdx,				displayCountUser,	3,								dfBigTTEdisplay,				bigDigit::displayHandler,			bpListSecondaryDisplay}
#endif // defined(useBigTTE)
#if defined(useCPUreading)
	,{mainDisplayIdx,				displayCountUser,	1,								dfCPUmonDisplay,				systemInfo::displayHandler,			bpListSecondaryDisplay}
#endif // defined(useCPUreading)
#if defined(useClockDisplay)
	,{mainDisplayIdx,				displayCountUser,	1,								dfClockShowDisplay,				clockDisplay::displayHandler,		bpListClockDisplay}
#endif // defined(useClockDisplay)

// the following display entries are supplemental non-menu display entries for options selected via configs.h 

#if defined(useDragRaceFunction)
	,{dragRaceDisplayIdx,			1,					4,								0,								accelerationTest::displayHandler,	bpListMiscViewer}
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	,{coastdownDisplayIdx,			1,					4,								0,								coastdown::displayHandler,			bpListMiscViewer}
#endif // defined(useCoastDownCalculator)

// the following display entries are display entries whose cursor positions do not get saved into EEPROM

	,{parameterEditDisplayIdx,		1,					12,								dfFullScreen,					parameterEdit::displayHandler,		bpListParameterEdit}
#if defined(useClockDisplay)
	,{clockSetDisplayIdx,			1,					4,								dfFullScreen,					clockSet::displayHandler,			bpListClockSet}
#endif // defined(useClockDisplay)
#if defined(useScreenEditor)
	,{displayEditDisplayIdx,		1,					8,								dfSplitScreen,					displayEdit::displayHandler,		bpListMainDisplayEdit}
#endif // defined(useScreenEditor)
};

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
