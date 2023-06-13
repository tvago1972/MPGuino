typedef uint16_t (* pageFunc)(uint8_t); // type for display page format function pointers

namespace mainDisplay /* main display section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static uint16_t getMainDisplayPageFormat(uint8_t formatIdx);
#if defined(trackIdleEOCdata)
	static uint16_t getMainEOCpageFormats(uint8_t formatIdx);
	static uint16_t getMainIdlePageFormats(uint8_t formatIdx);
#endif // defined(trackIdleEOCdata)
#if defined(useSpiffyTripLabels)
	static void outputPage(pageFunc pageFormatFunc, uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
	static void outputFunction(uint8_t readingIdx, uint16_t pageFormat, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4]);
#else // defined(useSpiffyTripLabels)
	static void outputPage(pageFunc pageFormatFunc, uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink);
	static void outputFunction(uint8_t readingIdx, uint16_t pageFormat, uint8_t tripBlink, uint8_t calcBlink);
#endif // defined(useSpiffyTripLabels)
	static void returnToMain(void);

};

static const uint8_t dfBitShift = 5;
static const uint8_t dfFunctionMask =		0b00011111;
static const uint8_t dfTripMask =			0b11100000;

// local trip indexes for main screen trip function display variables
#define nextAllowedValue 0
static const uint8_t msInstantIdx =			nextAllowedValue;
static const uint8_t msCurrentIdx =			msInstantIdx + 1;
static const uint8_t msTankIdx =			msCurrentIdx + 1;
#define nextAllowedValue msTankIdx + 1
#if defined(trackIdleEOCdata)
static const uint8_t msEOCidleCurrentIdx =	nextAllowedValue;
static const uint8_t msEOCidleTankIdx =		msEOCidleCurrentIdx + 1;
static const uint8_t msEOCidleInstantIdx =	msEOCidleTankIdx + 1;
#define nextAllowedValue msEOCidleInstantIdx + 1
#endif // defined(trackIdleEOCdata)

static const uint8_t msMaxTripCount =		nextAllowedValue;

static const uint8_t lblInstantIdx =		(msInstantIdx << dfBitShift)	| instantIdx;
static const uint8_t lblCurrentIdx =		(msCurrentIdx << dfBitShift)	| currentIdx;
static const uint8_t lblTankIdx =			(msTankIdx << dfBitShift)		| tankIdx;
#if defined(trackIdleEOCdata)
static const uint8_t lblEOCidleCurrentIdx =	(msEOCidleCurrentIdx << dfBitShift)	| eocIdleCurrentIdx;
static const uint8_t lblEOCidleTankIdx =	(msEOCidleTankIdx << dfBitShift)	| eocIdleTankIdx;
static const uint8_t lblEOCidleInstantIdx =	(msEOCidleInstantIdx << dfBitShift)	| eocIdleInstantIdx;
#endif // defined(trackIdleEOCdata)

// trip index translation list
static const uint8_t msTripList[] PROGMEM = {
	 instantIdx
	,currentIdx
	,tankIdx
#if defined(trackIdleEOCdata)
	,eocIdleCurrentIdx
	,eocIdleTankIdx
	,eocIdleInstantIdx
#endif // defined(trackIdleEOCdata)
};

#if defined(useSpiffyTripLabels)
// display variable trip labels
static const uint8_t msTripBitPattern[][4] PROGMEM = {
	 {0b00000000, 0b00000111, 0b00000010, 0b00000111} // I
 	,{0b00000000, 0b00000011, 0b00000100, 0b00000011} // C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000010} // T
#if defined(trackIdleEOCdata)
	,{0b00000000, 0b00000011, 0b00000100, 0b00000110} // italic C
	,{0b00000000, 0b00000111, 0b00000010, 0b00000100} // italic T
	,{0b00000000, 0b00000011, 0b00000010, 0b00000110} // italic I
#endif // defined(trackIdleEOCdata)
};

#endif // defined(useSpiffyTripLabels)
static const char msTripNameString[] PROGMEM = {
	"INST" tcEOS
	"CURR" tcEOS
	"TANK" tcEOS
#if defined(trackIdleEOCdata)
	"cC/I" tcEOS
	"tC/I" tcEOS
	"iC/I" tcEOS
#endif // defined(trackIdleEOCdata)
};

static const char mainDisplayFuncNames[] PROGMEM = {
	"Instrument" tcEOSCR
	"Custom" tcEOSCR
	"Instant/Current" tcEOSCR
	"Instant/Tank" tcEOSCR
	"Current" tcEOSCR
	"Tank" tcEOSCR
#if defined(trackIdleEOCdata)
	"EOC/Idle" tcEOSCR
#endif // defined(trackIdleEOCdata)
	"Current Data" tcEOSCR
	"Tank Data" tcEOSCR
#if defined(trackIdleEOCdata)
	"Current EOC/Idle" tcEOSCR
	"Tank EOC/Idle" tcEOSCR
#endif // defined(trackIdleEOCdata)
	"Remaining" tcEOSCR
};

static const uint16_t mainDisplayPageFormats[(uint16_t)(mainDisplayFormatSize)] PROGMEM = {
	 (lblInstantIdx << 8 ) |		(tSpeed)				// Instrument
	,(lblInstantIdx << 8 ) |		(tEngineSpeed)
	,(lblInstantIdx << 8 ) |		(tFuelRate)
	,(lblInstantIdx << 8 ) |		(tFuelEcon)

	,(lblInstantIdx << 8 ) |		(tFuelEcon)				// Custom
	,(lblInstantIdx << 8 ) |		(tSpeed)
	,(lblInstantIdx << 8 ) |		(tFuelRate)
	,(lblCurrentIdx << 8 ) |		(tFuelEcon)

	,(lblInstantIdx << 8 ) |		(tFuelEcon)				// Instant / Current
	,(lblInstantIdx << 8 ) |		(tSpeed)
	,(lblCurrentIdx << 8 ) |		(tFuelEcon)
	,(lblCurrentIdx << 8 ) |		(tDistance)

	,(lblInstantIdx << 8 ) |		(tFuelEcon)				// Instant / Tank
	,(lblInstantIdx << 8 ) |		(tSpeed)
	,(lblTankIdx << 8 ) |			(tFuelEcon)
	,(lblTankIdx << 8 ) |			(tDistance)

	,(lblCurrentIdx << 8 ) |		(tSpeed)				// Current
	,(lblCurrentIdx << 8 ) |		(tFuelEcon)
	,(lblCurrentIdx << 8 ) |		(tDistance)
	,(lblCurrentIdx << 8 ) |		(tFuelUsed)

	,(lblTankIdx << 8 ) |			(tSpeed)				// Tank
	,(lblTankIdx << 8 ) |			(tFuelEcon)
	,(lblTankIdx << 8 ) |			(tDistance)
	,(lblTankIdx << 8 ) |			(tFuelUsed)

#if defined(trackIdleEOCdata)
	,(lblEOCidleCurrentIdx << 8 ) |	(tDistance)				// EOC / Idle
	,(lblEOCidleCurrentIdx << 8 ) |	(tFuelUsed)
	,(lblEOCidleTankIdx << 8 ) |	(tDistance)
	,(lblEOCidleTankIdx << 8 ) |	(tFuelUsed)

#endif // defined(trackIdleEOCdata)
	,(lblCurrentIdx << 8 ) |		(tEngineRunTime)		// Current data
	,(lblCurrentIdx << 8 ) |		(tFuelUsed)
	,(lblCurrentIdx << 8 ) |		(tMotionTime)
	,(lblCurrentIdx << 8 ) |		(tDistance)

	,(lblTankIdx << 8 ) |			(tEngineRunTime)		// Tank data
	,(lblTankIdx << 8 ) |			(tFuelUsed)
	,(lblTankIdx << 8 ) |			(tMotionTime)
	,(lblTankIdx << 8 ) |			(tDistance)

#if defined(trackIdleEOCdata)
	,(lblEOCidleCurrentIdx << 8 ) |	(tEngineRunTime)		// Current EOC / Idle
	,(lblEOCidleCurrentIdx << 8 ) |	(tFuelUsed)
	,(lblEOCidleCurrentIdx << 8 ) |	(tMotionTime)
	,(lblEOCidleCurrentIdx << 8 ) |	(tDistance)

	,(lblEOCidleTankIdx << 8 ) |	(tEngineRunTime)		// Tank EOC / Idle
	,(lblEOCidleTankIdx << 8 ) |	(tFuelUsed)
	,(lblEOCidleTankIdx << 8 ) |	(tMotionTime)
	,(lblEOCidleTankIdx << 8 ) |	(tDistance)

#endif // defined(trackIdleEOCdata)
	,(lblTankIdx << 8 ) |			(tFuelUsed)				// Remaining
	,(lblTankIdx << 8 ) |			(tRemainingFuel)
	,(lblTankIdx << 8 ) |			(tTimeToEmpty)
	,(lblTankIdx << 8 ) |			(tDistanceToEmpty)
};

#if defined(trackIdleEOCdata)
static const uint16_t mainEOCpageFormats[4] PROGMEM = {
	 (lblInstantIdx << 8 ) |		(tSpeed)
	,(lblCurrentIdx << 8 ) |		(tFuelEcon)
	,(lblEOCidleCurrentIdx << 8 ) |	(tDistance)
	,(lblEOCidleTankIdx << 8 ) |	(tDistance)
};

static const uint16_t mainIdlePageFormats[4] PROGMEM = {
	 (lblInstantIdx << 8 ) |		(tFuelRate)
	,(lblCurrentIdx << 8 ) |		(tFuelEcon)
	,(lblEOCidleCurrentIdx << 8 ) |	(tFuelUsed)
	,(lblEOCidleTankIdx << 8 ) |	(tFuelUsed)
};

#endif // defined(trackIdleEOCdata)
static uint8_t topScreenLevel;

#if defined(useScreenEditor)
namespace displayEdit /* Programmable main display page edit support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void cancel(void);
	static void set(void);
	static void readInitial(void);
	static void changeItemUp(void);
#if defined(useButtonCrossConfig)
	static void changeItemDown(void);
#endif // useButtonCrossConfig

};

void doCursorUpdateScreenEdit(void);
void doScreenEditBump(void);
void doSaveScreen(void);
void doScreenReturnToMain(void);

static uint8_t basePageIdx;
static uint16_t displayEditPageFormats[(uint16_t)(4)];

uint8_t screenTripValue = 0;
uint8_t screenFunctionValue = 0;
uint8_t screenEditDirty = 0;

const uint8_t screenEditFlag_dirty =	0x80;

const char seFormatRevertedString[] PROGMEM = "DisplayEdit Canx";
const char seExitScreenEditString[] PROGMEM = "Screen Display";

#endif // defined(useScreenEditor)