#if defined(useCoastDownCalculator)
namespace coastdown /* Coastdown Calculator support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);

	void goDisplay(void);
	void goTrigger(void);

}

static const char coastdownTestMenuTitles[] PROGMEM = {
	"Perform Test" tcEOSCR
	"Cd SamplePd (s)" tcEOSCR
	"Veh" tcOMOFF "Weight" tcOTOG "Mass" tcOON " (" tcOMOFF "lbs" tcOTOG "kg" tcOON ")" tcEOSCR
	"FrArea*1000 " tcOMOFF "ft" tcOTOG "m" tcOON "^2" tcEOSCR
	"rho*1000 " tcOMOFF "lb/yd" tcOTOG "kg/m" tcOON "^3" tcEOSCR
	"C(d) * 1000" tcEOSCR
	"C(v) * 1000" tcEOSCR
	"C(rr) * 1000" tcEOSCR
};

static const uint8_t coastdownTestParamList[] PROGMEM = {
	pCoastdownSamplePeriodIdx,
	pVehicleMassIdx,
	pVehicleFrontalAreaIdx,
	pLocustDensityIdx,
	pCoefficientDidx,
	pCoefficientVidx,
	pCoefficientRRidx,
};

static volatile uint8_t coastdownState;

// bit flags for use with v8CoastdownStatus
static const uint8_t cdTestActive =				0b10000000;
static const uint8_t cdTestTriggered =			0b01000000;
static const uint8_t cdTestCanceled =			0b00100000;
static const uint8_t cdTestFinished =			0b00010000;
static const uint8_t cdTestTakeSample =			0b00001000;
static const uint8_t cdTestSampleTaken =		0b00000100;

static const uint8_t cdTestMeasurementFlags =	cdTestTakeSample | cdTestSampleTaken;
static const uint8_t cdTestInProgress =			cdTestActive | cdTestTriggered;
static const uint8_t cdTestCompleteFlags =		cdTestCanceled | cdTestFinished;
static const uint8_t cdTestClearFlags =			cdTestInProgress | cdTestMeasurementFlags | cdTestCompleteFlags;

static uint8_t coastdownCharIdx;

static const char coastdownMsgs[] PROGMEM = {
	"Coastdown Active\r"
	"CD Sample 1 read\r"
	"CD Sample 2 read\r"
	"CD Sample 3 read\r"
	"Coastdown Done\r"
	"Coastdown Canx\r"
};

static const char coastdownSymbol[] PROGMEM = "|}>}|{<{";

#endif // defined(useCoastDownCalculator)
