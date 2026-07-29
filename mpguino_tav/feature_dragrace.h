#if defined(useDragRaceFunction)
namespace accelerationTest /* Acceleration Test support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);
	static uint16_t getAccelTestDisplayPageFormat(uint8_t formatIdx);
	static void triggerTest(void);

}

static const uint8_t prgmTransferAccelTestTrips[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 16,								// if zero, then perform copy
	instrLdRegTripVar, 0x01, dragRawHalfSpeedIdx, rvVSScycleIdx,	// load raw half speed register elapsed time
	instrLdRegTripVar, 0x02, dragHalfSpeedIdx, rvVSScycleIdx,		// load regular half speed register elapsed time
	instrTestReg, 0x02,									// test regular half speed register elapsed time
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw half speed elapsed time to regular half speed elapsed time
	instrBranchIfGT, 8,									// if regular half speed elapsed time is greater than raw half speed elapsed time, skip

//copyHalfSpeed:
	instrLxdI, dragRawHalfSpeedIdx,						// transfer accel test raw half speed trip to accel test half speed trip
	instrCall, tLoadTrip,
	instrLxdI, dragHalfSpeedIdx,
	instrCall, tSaveTrip,

//cont1:
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 16,								// if zero, then perform copy
	instrLdRegTripVar, 0x01, dragRawFullSpeedIdx, rvVSScycleIdx,	// load raw full speed register elapsed time
	instrLdRegTripVar, 0x02, dragFullSpeedIdx, rvVSScycleIdx,		// load regular full speed register elapsed time
	instrTestReg, 0x02,									// test regular full speed register elapsed time
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw full speed elapsed time to regular full speed elapsed time
	instrBranchIfGT, 8,									// if raw full speed elapsed time is smaller than regular full speed elapsed time, skip

//copyFullSpeed:
	instrLxdI, dragRawFullSpeedIdx,						// transfer accel test raw full speed trip to accel test full speed trip
	instrCall, tLoadTrip,
	instrLxdI, dragFullSpeedIdx,
	instrCall, tSaveTrip,

//cont2:
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 16,								// if zero, then perform copy
	instrLdRegTripVar, 0x01, dragRawDistanceIdx, rvVSScycleIdx,	// load raw distance register elapsed time
	instrLdRegTripVar, 0x02, dragDistanceIdx, rvVSScycleIdx,	// load regular distance register elapsed time
	instrTestReg, 0x02,									// test regular distance register elapsed time
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw distance elapsed time to regular distance elapsed time
	instrBranchIfGT, 8,									// if raw distance elapsed time is smaller than regular distance elapsed time, skip

//copyDistance:
	instrLxdI, dragRawDistanceIdx,						// transfer accel test raw distance trip to accel test distance trip
	instrCall, tLoadTrip,
	instrLxdI, dragDistanceIdx,
	instrCall, tSaveTrip,

//cont3:
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 14,								// if zero, then perform copy
	instrLdRegVariable, 0x02, v32DragRawTopSpeedIdx,	// load raw top speed
	instrLdRegVariable, 0x01, m32DragTopSpeedIdx,		// load regular top speed
	instrTestReg, 0x01,									// test regular top speed
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw top speed to regular top speed
	instrBranchIfGT, 3,									// if raw top speed is smaller than regular top speed, skip

//copyTopSpeed:
	instrStRegVariable, 0x02, m32DragTopSpeedIdx,		// store raw top speed value to regular top speed

//cont4:
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 14,								// if zero, then perform copy
	instrLdRegVariable, 0x02, v32DragRawTrapSpeedIdx,	// load raw trap speed
	instrLdRegVariable, 0x01, m32DragTrapSpeedIdx,		// load regular trap speed
	instrTestReg, 0x01,									// test regular trap speed
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw trap speed to regular trap speed
	instrBranchIfGT, 3,									// if raw trap speed is smaller than regular trap speed, skip

//copyTopSpeed:
	instrStRegVariable, 0x02, m32DragTrapSpeedIdx,		// store raw trap speed value to regular trap speed

//cont5:
	instrDone											// exit to caller
};

static const char accelTestMenuTitles[] PROGMEM = {
	"Stats" tcEOSCR
	"Arm Accel Test" tcEOSCR
	"Test AutoArm 1-Y" tcEOSCR
	"TstDist " tcOMOFF "mi" tcOTOG "km" tcOON "*1000" tcEOSCR
	"TstSpd " tcOMOFF "MPH" tcOTOG "kph" tcOON "*1000" tcEOSCR
	"Veh" tcOMOFF "Weight" tcOTOG "Mass" tcOON " (" tcOMOFF "lbs" tcOTOG "kg" tcOON ")" tcEOSCR
};

static const uint8_t accelTestParamList[] PROGMEM = {
	pDragAutoFlagIdx,
	pDragDistanceIdx,
	pDragSpeedIdx,
	pVehicleMassIdx,
};

// bit flags for use with v8AccelerationFlagsIdx
static const uint8_t accelTestActive =				0b10000000;
static const uint8_t accelTestTriggered =			0b01000000;
static const uint8_t accelTestCancelled =			0b00100000;
static const uint8_t accelTestFinished =			0b00010000;
static const uint8_t accelTestFullSpeed =			0b00001000;
static const uint8_t accelTestHalfSpeed =			0b00000100;
static const uint8_t accelTestDistance =			0b00000010;

static const uint8_t accelTestMeasurementFlags =	accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance;
static const uint8_t accelTestInProgress =			accelTestActive | accelTestTriggered;
static const uint8_t accelTestCompleteFlags =		accelTestCancelled | accelTestFinished;
static const uint8_t accelTestClearFlags =			accelTestInProgress | accelTestMeasurementFlags | accelTestCompleteFlags;

static uint8_t accelTestStatus;
static uint8_t lastAccelTestStatus;
static uint8_t accelTestState;

enum {
	atsNoStatusChange,
	atsReady,
	atsActive,
	atsFinished,
	atsCancelled,
	atsCheckPointDist,
	atsCheckPointHalf,
	atsCheckPointDistHalf,
	atsCheckPointFull,
	atsCheckPointDistFull,
	atsCheckPointHalfFull,
	atsCheckPointDistHalfFull,
	atsInvalidState
};

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
enum {
	attTriggerNormal,
	attVehicleStillMoving,
	attTriggerForced
};

static const char accelTestTriggerMsgs[] PROGMEM = {
	"DragTest Armed" tcEOS
	"Stop Vehicle" tcEOS
	"DragTest Manual" tcEOS
};

static const uint16_t accelTestPageFormats[] PROGMEM = {
	(dragDistanceIdx << 8 ) |			(tFuelEcon),
	(dragDistanceIdx << 8 ) |			(tDragSpeed),				// for calculations, it really doesn't matter what trip index is used here
	(dragDistanceIdx << 8 ) |			(tAccelTestTime),
	(dragDistanceIdx << 8 ) |			(tEstimatedEnginePower),	// for calculations, it really doesn't matter what trip index is used here

	(dragHalfSpeedIdx << 8 ) |			(tAccelTestTime),
	(dragHalfSpeedIdx << 8 ) |			(tFuelUsed),
	(dragHalfSpeedIdx << 8 ) |			(tDistance),
	(dragHalfSpeedIdx << 8 ) |			(tFuelEcon),

	(dragFullSpeedIdx << 8 ) |			(tAccelTestTime),
	(dragFullSpeedIdx << 8 ) |			(tFuelUsed),
	(dragFullSpeedIdx << 8 ) |			(tDistance),
	(dragFullSpeedIdx << 8 ) |			(tFuelEcon),

	(dragDistanceIdx << 8 ) |			(tAccelTestTime),
	(dragDistanceIdx << 8 ) |			(tFuelUsed),
	(dragDistanceIdx << 8 ) |			(tDistance),
	(dragDistanceIdx << 8 ) |			(tFuelEcon),
};

#endif // defined(useDragRaceFunction)
