#ifdef useJSONoutput
void doOutputJSONnumber(const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces, const char * labelStr); // SWEET64::runPrgm
void doOutputJSONnumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalPlaces, const char * labelStr); // SWEET64::doCalculate
void doOutputJSONnumber(unsigned long an, uint8_t decimalPlaces, const char * labelStr);
void doOutputJSONnumber(union union_64 * an, uint8_t decimalPlaces, const char * labelStr);
void doOutputJSONnumber(uint8_t decimalPlaces, const char * labelStr); // base
void doOutputJSON(void);
#endif // useJSONoutput
#if defined(useDataLoggingOutput)
static void doOutputDataLog(void);

static const uint16_t dataLogTripCalcFormats[] PROGMEM = {
	 (instantIdx << 8) |	 	tFuelEcon)				// average fuel economy  for the past loop
	,(instantIdx << 8) |	 	tSpeed)					// average vehicle speed for the past loop
	,(instantIdx << 8) |	 	tInjectorOpenTime)		// fuel injector raw open time for the past loop
	,(instantIdx << 8) |	 	tInjectorPulseCount)	// fuel injector pulse count for the past loop
	,(instantIdx << 8) |	 	tVSSpulseCount)			// vss pulse count for the past loop
};

static const uint8_t dLIcount = (sizeof(dataLogTripCalcFormats) / sizeof(uint16_t));
#endif // defined(useDataLoggingOutput)
#ifdef useJSONoutput
const char JSONaccelTestStateMsgs[] PROGMEM = {
	tcEOS
	"Drag Ready" tcEOS
	"Testing" tcEOS
	"Drag Finished" tcEOS
	"Drag Cancelled" tcEOS
	"Distance reached" tcEOS
	"Half-speed reached" tcEOS
	"Half-speed and distance reached" tcEOS
	"Full speed reached" tcEOS
	"Full speed and distance reached" tcEOS
	"Full speed and half-speed reached" tcEOS
	"Full speed, half-speed, and distance reached" tcEOS
	"Drag Test Failure" tcEOS
};

#endif // useJSONoutput
