#if defined(useDataLoggingOutput)
static void doOutputDataLog(void);

static const uint16_t dataLogTripCalcFormats[] PROGMEM = {
	 ((instantIdx << 8) |	 	tFuelEcon)				// average fuel economy  for the past loop
	,((instantIdx << 8) |	 	tSpeed)					// average vehicle speed for the past loop
	,((instantIdx << 8) |	 	tInjectorOpenTime)		// fuel injector raw open time for the past loop
	,((instantIdx << 8) |	 	tInjectorPulseCount)	// fuel injector pulse count for the past loop
	,((instantIdx << 8) |	 	tVSSpulseCount)			// vss pulse count for the past loop
};

static const uint8_t dLIcount = (sizeof(dataLogTripCalcFormats) / sizeof(uint16_t));

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
namespace JSON /* JSON formatting support section prototype */
{

	static void init(interfaceDevice &dev);
	static void openElement(interfaceDevice &dev, uint8_t elementType);
	static void openKey(interfaceDevice &dev, const char * str, uint8_t elementType);
	static void closeElement(interfaceDevice &dev);
	static void closeElementInternal(interfaceDevice &dev);
	static void outputElementStart(interfaceDevice &dev, uint8_t elementType);
	static void outputElementNext(interfaceDevice &dev, uint8_t elementType);
	static void outputElementEnd(interfaceDevice &dev);
	static void outputNumber(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx);
	static void outputNumber(interfaceDevice &dev, const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces);
	static void outputNumber(interfaceDevice &dev, union union_64 * an, uint8_t decimalPlaces);
	static void outputNumber(interfaceDevice &dev, uint32_t an, uint8_t decimalPlaces);

};

static uint8_t JSONlevel;
static uint8_t JSONflag[16];

static const uint8_t JSONflagArray =		0b10000000;
static const uint8_t JSONflagObject =		0b01000000;
static const uint8_t JSONflagString =		0b00100000;
static const uint8_t JSONflagKey =			0b00010000;
static const uint8_t JSONflagMany =			0b00001000;

/* skybolt JSON packet output support section prototype */

void doOutputJSONnumber(unsigned long an, uint8_t decimalPlaces, const char * labelStr);
void doOutputJSON(void);

static const char JSONtitleStr[] PROGMEM = {
	"title" tcEOS
};

static const char JSONsubtitleStr[] PROGMEM = {
	"subtitle" tcEOS
};

static const char JSONrangesStr[] PROGMEM = {
	"ranges" tcEOS
};

static const char JSONmeasuresStr[] PROGMEM = {
	"measures" tcEOS
};

static const char JSONmarkersStr[] PROGMEM = {
	"markers" tcEOS
};

#endif // defined(useJSONoutput)
