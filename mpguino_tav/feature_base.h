#if defined(useButtonInput)

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
#if defined(useCoastDownCalculator)
	"Coastdown" tcEOSCR
#endif // defined(useCoastDownCalculator)
#if defined(useSimulatedFIandVSS)
	"Sensor Sim" tcEOSCR
#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
	"Pressures" tcEOSCR
#endif // defined(useChryslerMAPCorrection)
#if defined(useDebugAnalog)
	"ADC readings" tcEOSCR
#endif // defined(useDebugAnalog)
};

#endif // defined(useButtonInput)
