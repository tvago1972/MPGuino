typedef uint16_t (* pageFunc)(uint8_t); // type for display page format function pointers

namespace baseMenu /* basic top-down menu section prototype */
{

	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);

};

static const char baseMenuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
#if defined(useExpandedMainDisplay)
	"Displays" tcEOSCR
#else // defined(useExpandedMainDisplay)
	"Main Display" tcEOSCR
#endif // defined(useExpandedMainDisplay)
	"Settings" tcEOSCR
#if defined(useDragRaceFunction)
	"Accel Test" tcEOSCR
#endif // defined(useDragRaceFunction)
#ifdef useCoastDownCalculator
	"Coastdown" tcEOSCR
#endif // useCoastDownCalculator
#if defined(useSimulatedFIandVSS)
	"Sensor Sim" tcEOSCR
#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
	"Pressures" tcEOSCR
#endif // defined(useChryslerMAPCorrection)
#if defined(useDebugAnalog)
	"ADC readings" tcEOSCR
#endif // defined(useDebugAnalog)
#if defined(useTestButtonValues)
	"Button Values" tcEOSCR
#endif // defined(useTestButtonValues)
};

namespace mainDisplay /* main display section prototype */
{

	static void displayHandler(uint8_t cmd, uint8_t cursorPos);
#if defined(useExpandedMainDisplay)
	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);
#endif // defined(useExpandedMainDisplay)
	static uint16_t getMainDisplayPageFormat(uint8_t formatIdx);
#if defined(trackIdleEOCdata)
	static uint16_t getMainEOCpageFormats(uint8_t formatIdx);
	static uint16_t getMainIdlePageFormats(uint8_t formatIdx);
#endif // defined(trackIdleEOCdata)
	static void outputPage(pageFunc pageFormatFunc, uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink);
	static void outputFunction(uint8_t readingIdx, uint16_t pageFormat, uint8_t tripBlink, uint8_t calcBlink);
	static uint8_t findTripIdx(uint8_t tripIdx);

};

static const char mainMenuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
	"Main Display" tcEOSCR
#if defined(useStatusBar)
	"INSTvs(trip) FE" tcEOSCR
#endif // defined(useStatusBar)
#if defined(useBigFE)
	"Big FuelEcon" tcEOSCR
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
	"FE/Time" tcEOSCR
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	"FE/Speed" tcEOSCR
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
	"Big DistToE" tcEOSCR
#endif // defined(useBigDTE)
#if defined(useBigTTE)
	"Big TimeToE" tcEOSCR
#endif // defined(useBigTTE)
#if defined(useCPUreading)
	"CPU Info" tcEOSCR
#endif // defined(useCPUreading)
#if defined(useClockDisplay)
	"Clock" tcEOSCR
#endif // defined(useClockDisplay)
};

static const char mainDisplayPageTitles[] PROGMEM = {
	"Instrument" tcEOS
	"Custom" tcEOS
	"Instant/Current" tcEOS
	"Instant/Tank" tcEOS
	"Current" tcEOS
	"Tank" tcEOS
#if defined(trackIdleEOCdata)
	"EOC/Idle" tcEOS
#endif // defined(trackIdleEOCdata)
	"Current Data" tcEOS
	"Tank Data" tcEOS
#if defined(trackIdleEOCdata)
	"Current EOC/Idle" tcEOS
	"Tank EOC/Idle" tcEOS
#endif // defined(trackIdleEOCdata)
	"Remaining" tcEOS
};

static const uint16_t mainDisplayPageFormats[(uint16_t)(mainDisplayFormatSize)] PROGMEM = {
	 (instantIdx << 8 ) |			(tSpeed)				// Instrument
	,(instantIdx << 8 ) |			(tEngineSpeed)
	,(instantIdx << 8 ) |			(tFuelRate)
	,(instantIdx << 8 ) |			(tFuelEcon)

	,(instantIdx << 8 ) |			(tFuelEcon)				// Custom
	,(instantIdx << 8 ) |			(tSpeed)
	,(instantIdx << 8 ) |			(tFuelRate)
	,(currentIdx << 8 ) |			(tFuelEcon)

	,(instantIdx << 8 ) |			(tFuelEcon)				// Instant / Current
	,(instantIdx << 8 ) |			(tSpeed)
	,(currentIdx << 8 ) |			(tFuelEcon)
	,(currentIdx << 8 ) |			(tDistance)

	,(instantIdx << 8 ) |			(tFuelEcon)				// Instant / Tank
	,(instantIdx << 8 ) |			(tSpeed)
	,(tankIdx << 8 ) |				(tFuelEcon)
	,(tankIdx << 8 ) |				(tDistance)

	,(currentIdx << 8 ) |			(tSpeed)				// Current
	,(currentIdx << 8 ) |			(tFuelEcon)
	,(currentIdx << 8 ) |			(tDistance)
	,(currentIdx << 8 ) |			(tFuelUsed)

	,(tankIdx << 8 ) |				(tSpeed)				// Tank
	,(tankIdx << 8 ) |				(tFuelEcon)
	,(tankIdx << 8 ) |				(tDistance)
	,(tankIdx << 8 ) |				(tFuelUsed)

#if defined(trackIdleEOCdata)
	,(eocIdleCurrentIdx << 8 ) |	(tDistance)				// EOC / Idle
	,(eocIdleCurrentIdx << 8 ) |	(tFuelUsed)
	,(eocIdleTankIdx << 8 ) |		(tDistance)
	,(eocIdleTankIdx << 8 ) |		(tFuelUsed)

#endif // defined(trackIdleEOCdata)
	,(currentIdx << 8 ) |			(tEngineRunTime)		// Current data
	,(currentIdx << 8 ) |			(tFuelUsed)
	,(currentIdx << 8 ) |			(tMotionTime)
	,(currentIdx << 8 ) |			(tDistance)

	,(tankIdx << 8 ) |				(tEngineRunTime)		// Tank data
	,(tankIdx << 8 ) |				(tFuelUsed)
	,(tankIdx << 8 ) |				(tMotionTime)
	,(tankIdx << 8 ) |				(tDistance)

#if defined(trackIdleEOCdata)
	,(eocIdleCurrentIdx << 8 ) |	(tEngineRunTime)		// Current EOC / Idle
	,(eocIdleCurrentIdx << 8 ) |	(tFuelUsed)
	,(eocIdleCurrentIdx << 8 ) |	(tMotionTime)
	,(eocIdleCurrentIdx << 8 ) |	(tDistance)

	,(eocIdleTankIdx << 8 ) |		(tEngineRunTime)		// Tank EOC / Idle
	,(eocIdleTankIdx << 8 ) |		(tFuelUsed)
	,(eocIdleTankIdx << 8 ) |		(tMotionTime)
	,(eocIdleTankIdx << 8 ) |		(tDistance)

#endif // defined(trackIdleEOCdata)
	,(tankIdx << 8 ) |				(tFuelUsed)				// Remaining
	,(tankIdx << 8 ) |				(tRemainingFuel)
	,(tankIdx << 8 ) |				(tTimeToEmpty)
	,(tankIdx << 8 ) |				(tDistanceToEmpty)
};

#if defined(trackIdleEOCdata)
static const uint16_t mainEOCpageFormats[4] PROGMEM = {
	 (instantIdx << 8 ) |			(tSpeed)
	,(currentIdx << 8 ) |			(tFuelEcon)
	,(eocIdleCurrentIdx << 8 ) |	(tDistance)
	,(eocIdleTankIdx << 8 ) |		(tDistance)
};

static const uint16_t mainIdlePageFormats[4] PROGMEM = {
	 (instantIdx << 8 ) |			(tFuelRate)
	,(currentIdx << 8 ) |			(tFuelEcon)
	,(eocIdleCurrentIdx << 8 ) |	(tFuelUsed)
	,(eocIdleTankIdx << 8 ) |		(tFuelUsed)
};

#endif // defined(trackIdleEOCdata)
#if defined(useScreenEditor)
namespace displayEdit /* Programmable main display page edit support section prototype */
{

	static void displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void entry(void);
	static void cancel(void);
	static void set(void);
	static void readInitial(void);
	static void changeItemUp(void);
#if defined(useButtonCrossConfig)
	static void changeItemDown(void);
#endif // defined(useButtonCrossConfig)
	static void changeItem(uint8_t changeDir);

};

static uint8_t basePageIdx;
static uint8_t formatEditIdx;
static uint8_t formatFunctionFlag;
static uint16_t displayEditPageFormats[(uint16_t)(4)];

static const char deFormatReverted[] PROGMEM = "DisplayEdit Canx";
static const char deFormatNoChange[] PROGMEM = "Disp Unchanged";
static const char deFormatSaved[] PROGMEM = "Disp Changed";

#endif // defined(useScreenEditor)