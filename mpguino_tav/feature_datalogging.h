#if defined(useDataLoggingOutput)
static void doOutputDataLog(void);

static const uint16_t dataLogTripCalcFormats[] PROGMEM = {
	((instantIdx << 8) |		tFuelEcon),				// average fuel economy  for the past loop
	((instantIdx << 8) |		tSpeed),				// average vehicle speed for the past loop
	((instantIdx << 8) |		tInjectorOpenTime),		// fuel injector raw open time for the past loop
	((instantIdx << 8) |		tInjectorPulseCount),	// fuel injector pulse count for the past loop
	((instantIdx << 8) |		tVSSpulseCount),		// vss pulse count for the past loop
};

static const uint8_t dLIcount = (sizeof(dataLogTripCalcFormats) / sizeof(uint16_t));

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
namespace JSONsupport /* JSON formatting support section prototype */
{

	static void init(device_t &dev);
	static void openElement(device_t &dev, uint8_t elementType);
	static void openKey(device_t &dev, const char * str, uint8_t elementType);
	static void closeElement(device_t &dev);
	static void closeElementInternal(device_t &dev);
	static void outputElementStart(device_t &dev, uint8_t elementType);
	static void outputElementNext(device_t &dev, uint8_t elementType);
	static void outputElementEnd(device_t &dev);
	static void outputNumber(device_t &dev, uint8_t tripIdx, uint8_t calcIdx);
	static void outputNumber(device_t &dev, const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces);
	static void outputNumber(device_t &dev, uint32_t an, uint8_t decimalPlaces);

};

static uint8_t JSONlevel;
static uint8_t JSONflag[16];

static const uint8_t JSONflagArray =		0b10000000;
static const uint8_t JSONflagObject =		0b01000000;
static const uint8_t JSONflagString =		0b00100000;
static const uint8_t JSONflagKey =			0b00010000;
static const uint8_t JSONflagMany =			0b00001000;

/* skybolt JSON packet output support section prototype */

#if defined(useDragRaceFunction)
static void doOutputJSONnumber(uint32_t an, uint8_t decimalPlaces, const char * labelStr);
#endif // defined(useDragRaceFunction)
static void doOutputJSONfuelEcon(const char * prefixStr, uint8_t tripIdx);
static void doOutputJSONremainingFuel(void);
static void doOutputJSON(void);

static const char tripStr[] PROGMEM = {
	"trip " tcEOS
};

static const char semicolonTankStr[] PROGMEM = {
	"; tank " tcEOS
};

static const char semicolonUsingStr[] PROGMEM = {
	"; using " tcEOS
};

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
