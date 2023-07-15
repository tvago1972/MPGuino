#if defined(useDragRaceFunction)
namespace accelerationTest /* Acceleration Test support section prototype */
{

	static void displayHandler(uint8_t cmd, uint8_t cursorPos);
	void goTrigger(void);
	uint8_t triggerTest(void);
	static void idleProcess(void);

}

static volatile uint8_t accelerationFlags;

static const uint8_t accelTestActive =				0b10000000;
static const uint8_t accelTestTriggered =			0b01000000;
static const uint8_t accelTestFullSpeed =			0b00100000;
static const uint8_t accelTestHalfSpeed =			0b00010000;
static const uint8_t accelTestDistance =			0b00001000;
static const uint8_t accelTestCancelled =			0b00000100;
static const uint8_t accelTestFinished =			0b00000010;

static const uint8_t accelTestMeasurementFlags =	accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance;
static const uint8_t accelTestInProgress =			accelTestActive | accelTestTriggered;
static const uint8_t accelTestCompleteFlags =		accelTestCancelled | accelTestFinished;
static const uint8_t accelTestClearFlags =			accelTestInProgress | accelTestMeasurementFlags | accelTestCompleteFlags;

static uint8_t accelTestStatus;
static uint8_t lastAccelTestStatus;
static uint8_t accelTestState;

static const uint8_t atsNoStatusChange =			0;
static const uint8_t atsReady =						atsNoStatusChange + 1;
static const uint8_t atsActive =					atsReady + 1;
static const uint8_t atsFinished =					atsActive + 1;
static const uint8_t atsCancelled =					atsFinished + 1;
static const uint8_t atsCheckPointDist =			atsCancelled + 1;
static const uint8_t atsCheckPointHalf =			atsCheckPointDist + 1;
static const uint8_t atsCheckPointDistHalf =		atsCheckPointHalf + 1;
static const uint8_t atsCheckPointFull =			atsCheckPointDistHalf + 1;
static const uint8_t atsCheckPointDistFull =		atsCheckPointFull + 1;
static const uint8_t atsCheckPointHalfFull =		atsCheckPointDistFull + 1;
static const uint8_t atsCheckPointDistHalfFull =	atsCheckPointHalfFull + 1;
static const uint8_t atsInvalidState =				atsCheckPointDistHalfFull + 1;

static const char accelTestStateMsgs[] PROGMEM = {
	tcEOS
	"Drag Ready" tcEOS
	"Drag Test Start" tcEOS
	"Drag Finished" tcEOS
	"Drag Cancelled" tcEOS
	"DIST" tcEOS
	"     HALF" tcEOS
	"DIST HALF" tcEOS
	"          FULL" tcEOS
	"DIST      FULL" tcEOS
	"     HALF FULL" tcEOS
	"DIST HALF FULL" tcEOS
	"Drag Test Fail" tcEOS
};

#if defined(useJSONoutput)
static const char JSONaccelTestStateMsgs[] PROGMEM = {
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

#endif // defined(useJSONoutput)
static const uint16_t accelTestPageFormats[] PROGMEM = {
	 (dragDistanceIdx << 8 ) |			(tFuelEcon)
	,(dragDistanceIdx << 8 ) |			(tDragSpeed)				// for calculations, it really doesn't matter what trip index is used here
	,(dragDistanceIdx << 8 ) |			(tAccelTestTime)
	,(dragDistanceIdx << 8 ) |			(tEstimatedEnginePower)		// for calculations, it really doesn't matter what trip index is used here

	,(dragHalfSpeedIdx << 8 ) |			(tAccelTestTime)
	,(dragHalfSpeedIdx << 8 ) |			(tFuelUsed)
	,(dragHalfSpeedIdx << 8 ) |			(tDistance)
	,(dragHalfSpeedIdx << 8 ) |			(tFuelEcon)

	,(dragFullSpeedIdx << 8 ) |			(tAccelTestTime)
	,(dragFullSpeedIdx << 8 ) |			(tFuelUsed)
	,(dragFullSpeedIdx << 8 ) |			(tDistance)
	,(dragFullSpeedIdx << 8 ) |			(tFuelEcon)

	,(dragDistanceIdx << 8 ) |			(tAccelTestTime)
	,(dragDistanceIdx << 8 ) |			(tFuelUsed)
	,(dragDistanceIdx << 8 ) |			(tDistance)
	,(dragDistanceIdx << 8 ) |			(tFuelEcon)
};

#endif // defined(useDragRaceFunction)
