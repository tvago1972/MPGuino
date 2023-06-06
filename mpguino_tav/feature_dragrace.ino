#ifdef useDragRaceFunction
// drag race function
//
// upon successful arming, the drag race function will measure times to reach the distance specified by the EEPROM parameter pDragDistanceIdx (preset to 1/4 mile),
// and to reach the speed specified by the EEPROM parameter pDragSpeedIdx (preset to 60 MPH).

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

const char accelTestStateMsgs[] PROGMEM = {
	"\r"
	"Drag Ready\r"
	"Drag Test Start\r"
	"Drag Finished\r"
	"Drag Cancelled\r"
	"DIST\r"
	"     HALF\r"
	"DIST HALF\r"
	"          FULL\r"
	"DIST      FULL\r"
	"     HALF FULL\r"
	"DIST HALF FULL\r"
	"Drag Test Fail\r"
};

// local trip indexes for drag calculation trip function display variables
#define nextAllowedValue 0
const uint8_t drDragHalfSpeedIdx =	nextAllowedValue;
const uint8_t drDragFullSpeedIdx =	drDragHalfSpeedIdx + 1;
const uint8_t drDragDistanceIdx =	drDragFullSpeedIdx + 1;
#define nextAllowedValue drDragDistanceIdx + 1

const uint8_t lblDragHalfSpeedIdx =	(drDragHalfSpeedIdx << dfBitShift)	| dragHalfSpeedIdx;
const uint8_t lblDragFullSpeedIdx =	(drDragFullSpeedIdx << dfBitShift)	| dragFullSpeedIdx;
const uint8_t lblDragDistanceIdx =	(drDragDistanceIdx << dfBitShift)	| dragDistanceIdx;

#ifdef useSpiffyTripLabels
// display variable trip labels
const uint8_t drTripBitPattern[][4] PROGMEM = {
	{0b00000000, 0b00000001, 0b00000010, 0b00000100} // /
	,{0b00000000, 0b00000010, 0b00000101, 0b00000010} // full circle
	,{0b00000000, 0b00000110, 0b00000101, 0b00000110} // D
};

#endif // useSpiffyTripLabels
const uint8_t dragRaceScreenFormats[] PROGMEM = {
	 {lblDragDistanceIdx,	tFuelEcon}
	,{lblDragDistanceIdx,	tDragSpeed}						// for calculations, it really doesn't matter what trip index is used here
	,{lblDragDistanceIdx,	tAccelTestTime}
	,{lblDragDistanceIdx,	0x80 | tEstimatedEnginePower}	// for calculations, it really doesn't matter what trip index is used here

	,{lblDragHalfSpeedIdx,	tAccelTestTime}
	,{lblDragHalfSpeedIdx,	tFuelUsed}
	,{lblDragHalfSpeedIdx,	tDistance}
	,{lblDragHalfSpeedIdx,	tFuelEcon}

	,{lblDragFullSpeedIdx,	tAccelTestTime}
	,{lblDragFullSpeedIdx,	tFuelUsed}
	,{lblDragFullSpeedIdx,	tDistance}
	,{lblDragFullSpeedIdx,	tFuelEcon}

	,{lblDragDistanceIdx,	tAccelTestTime}
	,{lblDragDistanceIdx,	tFuelUsed}
	,{lblDragDistanceIdx,	tDistance}
	,{lblDragDistanceIdx,	tFuelEcon}
};

static const uint8_t prgmInitializeAccelTest[] PROGMEM = {
	instrLdRegConst, 0x02, idxCycles0PerSecond,			// fetch cycles per second constant
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,		// store it in instantaneous accel test speed period variable
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch distance parameter value in VSS pulses
	instrMul2byEEPROM, pDragSpeedIdx,					// multiply by drag function speed parameter value in unit distance per hour
	instrLdReg, 0x21,									// save denominator term in VSS pulses per hour for later
	instrLdRegConst, 0x02, idxSecondsPerHour,			// fetch seconds per hour constant
	instrMul2byConst, idxCycles0PerSecond,				// multply by cycles per second constant
	instrShiftRegLeft, 0x02,							// multiply it by two to obtain accel half speed period value
	instrMul2byConst, idxDecimalPoint,					// multiply to factor out decimal formatting term from accel test speed parameter value
	instrDiv2by1,										// perform conversion
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelFullPeriodValueIdx,	// save it to accel test full-speed period tripwire variable
	instrShiftRegLeft, 0x02,							// shift result right one bit
	instrStRegVolatile, 0x02, vAccelHalfPeriodValueIdx,	// save it to accel test half-speed period tripwire variable
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch drag function distance parameter value in VSS pulses
	instrMul2byConst, pDragDistanceIdx,					// multiply by drag function distance parameter value in unit distance
	instrDiv2byConst, idxDecimalPoint,					// get rid of decimal formatting factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelDistanceValueIdx,	// save it to accel test distanct tripwire variable
	instrDone											// exit to caller
};

static const uint8_t prgmAccelTestCompareFullSpeeds[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrTestReg, 0x01,									// test accel test autotrigger flag
	instrBranchIfZero, 12,								// if zero, then output a 1
	instrLdRegTripVar, 0x01, dragRawFullSpeedIdx, rvVSScycleIdx,	// load raw full speed register elapsed time
	instrLdRegTripVar, 0x02, dragFullSpeedIdx, rvVSScycleIdx,		// load regular full speed register elapsed time
	instrCmpXtoY, 0x21,									// if raw full speed register has a smaller time than regular full speed register, output a 1
	instrBranchIfLTorE, 4,								// if raw full speed register has a smaller time than regular full speed register, output a 1

//oneret:
	instrLdRegByte, 0x02, 1,							// load a 1 into register 2
	instrDone,											// exit to caller

//zeroret:
	instrLdRegByte, 0x02, 0,							// load a 0 into register 2
	instrDone											// exit to caller
};

void accelerationTest::goDisplay(void)
{

	if (accelTestState) // if acceleration test state has changed
	{

		// display status message for new state
		printStatusMessage(accelTestStateMsgs, accelTestState);

	}

#ifdef useSpiffyTripLabels
	displayMainScreenFunctions(dragRaceScreenFormats, screenCursor[(unsigned int)(dragRaceIdx)], 136, 0, drTripBitPattern);
#else // useSpiffyTripLabels
	displayMainScreenFunctions(dragRaceScreenFormats, screenCursor[(unsigned int)(dragRaceIdx)]);
#endif // useSpiffyTripLabels

}

void accelerationTest::goTrigger(void)
{

	switch (triggerTest())
	{

		case 1: // vehicle is still moving - can't trigger drag mode
			printStatusMessage(PSTR("Stop vehicle"));
			break;

		case 2: // vehicle engine has stopped - can't trigger drag mode
			printStatusMessage(PSTR("Start Engine"));
			break;

		case 3: // acceleration test manually cancelled, so force manual trigger mode
			EEPROM::writeVal(pDragAutoFlagIdx, 0);
			break;

		default:
			break;

	}

}

uint8_t accelerationTest::triggerTest(void)
{

	uint8_t oldSREG;

	if (accelerationFlags & accelTestInProgress)
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		accelerationFlags &= ~accelTestClearFlags; // reset drag race capture flags
		accelerationFlags |= (accelTestCompleteFlags); // signal that drag function is cancelled

		SREG = oldSREG; // restore state of interrupt flag

		return 3;

	}
	else
	{

		if (dirty & dGoodVehicleMotion) // if vehicle is still moving, then alert driver
		{

			return 1; // vehicle is still moving - can't trigger drag mode

		}
		else // otherwise, set drag trigger
		{

			if (dirty & dGoodEngineRotationOpen) // if vehicle engine is running, go trigger drag mode
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts

				accelerationFlags &= ~accelTestClearFlags; // turn off all acceleration test functionality in interrupt-land

				SREG = oldSREG; // restore state of interrupt flag

				tripVar::reset(dragRawHalfSpeedIdx); // zero out acceleration 1/2 speed setpoint data
				tripVar::reset(dragRawFullSpeedIdx); // zero out acceleration full speed setpoint data
				tripVar::reset(dragRawDistanceIdx); // zero out acceleration distance setpoint data

				SWEET64::runPrgm(prgmInitializeAccelTest, 0); // initialize low-level values and tripwires

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts

				accelerationFlags |= (accelTestTriggered | accelTestMeasurementFlags); // set drag flags in accelerationFlags register

				SREG = oldSREG; // restore state of interrupt flag

				return 0;

			}
			else
			{

				return 2; // vehicle engine has stopped - can't trigger drag mode

			}

		}

	}

}

void accelerationTest::idleProcess(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	accelTestStatus = lastAccelTestStatus; // copy last loop's accel test flag status to this loop
	lastAccelTestStatus = accelerationFlags; // copy current accel test flag status for next loop

	SREG = oldSREG; // restore interrupt flag status

	accelTestStatus = (lastAccelTestStatus ^ accelTestStatus) & accelTestClearFlags; // detect any drag race flag changes

	if (accelTestStatus)
	{

		accelTestState = accelerationFlags & accelTestClearFlags;

		switch (accelTestState)
		{

			case (accelTestTriggered | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
				accelTestState = 1;
				break;

			case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
				accelTestState = 2;
				break;

			case (accelTestFinished):
				if (SWEET64::runPrgm(prgmAccelTestCompareFullSpeeds, 0))
				{

					tripVar::transfer(dragRawHalfSpeedIdx, dragHalfSpeedIdx);
					tripVar::transfer(dragRawFullSpeedIdx, dragFullSpeedIdx);
					tripVar::transfer(dragRawDistanceIdx, dragDistanceIdx);

				}

				accelTestState = 3;
				break;

			case (accelTestFinished | accelTestCancelled):
				accelTestState = 4;
				break;

			case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed):
				accelTestState = 5;
				break;

			case (accelTestActive | accelTestFullSpeed | accelTestDistance):
				accelTestState = 6;
				break;

			case (accelTestActive | accelTestFullSpeed):
				accelTestState = 7;
				break;

			case (accelTestActive | accelTestHalfSpeed | accelTestDistance):
				accelTestState = 8;
				break;

			case (accelTestActive | accelTestHalfSpeed):
				accelTestState = 9;
				break;

			case (accelTestActive | accelTestDistance):
				accelTestState = 10;
				break;

			case (accelTestActive):
				accelTestState = 11;
				break;

			default:
				accelTestState = 12;
				break;

		}

	}
	else
	{

		accelTestState = 0;

		if (accelerationFlags & accelTestFinished)
		{

			if (EEPROM::readVal(pDragAutoFlagIdx))
			{

				if (accelerationTest::triggerTest() == 0) accelTestState = 1;

			}

		}

	}

}

#endif // useDragRaceFunction
