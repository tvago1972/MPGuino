#ifdef useDragRaceFunction
namespace accelerationTest /* Acceleration Test support section prototype */
{

	static void displayHandler(uint8_t cmd, uint8_t cursorPos);

	void goTrigger(void);
	uint8_t triggerTest(void);
	void idleProcess(void);

}

volatile uint8_t accelerationFlags;

const uint8_t accelTestActive =				0b10000000;
const uint8_t accelTestTriggered =			0b01000000;
const uint8_t accelTestFullSpeed =			0b00100000;
const uint8_t accelTestHalfSpeed =			0b00010000;
const uint8_t accelTestDistance =			0b00001000;
const uint8_t accelTestCancelled =			0b00000100;
const uint8_t accelTestFinished =			0b00000010;

const uint8_t accelTestMeasurementFlags =	accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance;
const uint8_t accelTestInProgress =			accelTestActive | accelTestTriggered;
const uint8_t accelTestCompleteFlags =		accelTestCancelled | accelTestFinished;
const uint8_t accelTestClearFlags =			accelTestInProgress | accelTestMeasurementFlags | accelTestCompleteFlags;

uint8_t accelTestStatus;
uint8_t lastAccelTestStatus;

/*

meaning of value contained in accel test state variable accelTestState is as follows:

0  - no status change
1  - accel test ready
2  - accel test armed
3  - accel test finished
4  - accel test cancelled
5  - accel test distance checkpoint reached
6  - accel test half speed checkpoint reached
7  - accel test half speed and distance checkpoints reached
8  - accel test full speed checkpoint reached
9  - accel test full speed and distance checkpoints reached
10 - accel test full speed and half speed checkpoints reached
11 - accel test full speed, half speed, and distance checkpoints reached
12 - invalid accel test state encountered

*/
uint8_t accelTestState;

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

#endif // useDragRaceFunction
