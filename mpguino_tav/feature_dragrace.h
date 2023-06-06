#ifdef useDragRaceFunction
namespace accelerationTest /* Acceleration Test support section prototype */
{

	void goDisplay(void);
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

#endif // useDragRaceFunction
