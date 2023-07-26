#if defined(useDragRaceFunction)
 /* Acceleration Test support section */
 
// upon successful arming, the drag race function will measure times to reach the distance specified by the EEPROM parameter pDragDistanceIdx (preset to 1/4 mile),
// and to reach the speed specified by the EEPROM parameter pDragSpeedIdx (preset to 60 MPH).

static const uint8_t prgmTransferAccelTestTrips[] PROGMEM = {
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 16,								// if zero, then perform copy
	instrLdRegTripVar, 0x01, dragRawHalfSpeedIdx, rvVSScycleIdx,	// load raw half speed register elapsed time
	instrLdRegTripVar, 0x02, dragHalfSpeedIdx, rvVSScycleIdx,		// load regular half speed register elapsed time
	instrTestReg, 0x02,									// test regular half speed register elapsed time
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw half speed elapsed time to regular half speed elapsed time
	instrBranchIfGT, 8,									// if raw half speed elapsed time is smaller than regular half speed elapsed time, skip

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
	instrLdRegTripVar, 0x02, dragDistanceIdx, rvVSScycleIdx,		// load regular distance register elapsed time
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
	instrLdRegVolatile, 0x02, vDragRawInstantSpeedIdx,	// load raw top speed
	instrLdRegVolatile, 0x01, vDragInstantSpeedIdx,		// load regular top speed
	instrTestReg, 0x01,									// test regular top speed
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw top speed to regular top speed
	instrBranchIfGT, 3,									// if raw top speed is smaller than regular top speed, skip

//copyTopSpeed:
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,		// store raw top speed value to regular top speed

//cont4:
	instrLdRegEEPROM, 0x01, pDragAutoFlagIdx,			// fetch accel test autotrigger flag
	instrBranchIfZero, 14,								// if zero, then perform copy
	instrLdRegVolatile, 0x02, vDragRawTrapSpeedIdx,		// load raw trap speed
	instrLdRegVolatile, 0x01, vDragTrapSpeedIdx,		// load regular trap speed
	instrTestReg, 0x01,									// test regular trap speed
	instrBranchIfZero, 4,								// if zero, then perform copy
	instrCmpXtoY, 0x21,									// compare raw trap speed to regular trap speed
	instrBranchIfGT, 3,									// if raw trap speed is smaller than regular trap speed, skip

//copyTopSpeed:
	instrStRegVolatile, 0x02, vDragTrapSpeedIdx,		// store raw trap speed value to regular trap speed

//cont5:
	instrDone											// exit to caller
};

static void accelerationTest::init(void)
{

	changeBitFlags(accelerationFlags, accelTestClearFlags, 0);

	lastAccelTestStatus = 0;
	accelTestStatus = 0;

}

static uint8_t accelerationTest::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
		case displayOutputIdx:
			if (accelTestState) // if acceleration test state has changed
			{

				// display status message for new state
				text::statusOut(devLCD, accelTestStateMsgs, accelTestState);
				accelTestState = 0;

			}

			mainDisplay::outputPage(accelerationTest::getAccelTestDisplayPageFormat, cursorPos, 136, 0);

			break;

		default:
			break;

	}

}

static uint16_t accelerationTest::getAccelTestDisplayPageFormat(uint8_t formatIdx)
{

	return pgm_read_word(&accelTestPageFormats[(uint16_t)(formatIdx)]);

}

static uint8_t accelerationTest::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuInitialEntryIdx:
			numberEditObj.neStatusMessage = pseStatusMessages;
			retVal = 1;
			break;

		case menuFirstLineOutIdx:
			text::stringOut(devLCD, accelTestMenuTitles, cursorPos);
			break;

		case menuSecondLineInitIdx:
			if (cursorPos > 1)
			{

				numberEditObj.parameterIdx = pgm_read_byte(&accelTestParamList[(uint16_t)(cursorPos - 2)]);
				parameterEdit::sharedFunctionCall(nesLoadInitial);

			}

		case menuSecondLineFlagIdx: // if the menu cursor line has supplemental information, retVal will be set to 1
			if (cursorPos > 1) retVal = 1;
			break;

		case menuSecondLineOutIdx:
			if (cursorPos > 1)
			{

				text::stringOut(devLCD, pBuff); // output supplementary information
				text::newLine(devLCD); // clear to the end of the line

			}

			break;

		case menuDoSelectionIdx:
			if (cursorPos > 1)
			{

					numberEditObj.callingDisplayIdx = workingDisplayIdx;
					retVal = parameterEditDisplayIdx; // go to parameter edit display index

			}
			else
			{

				retVal = dragRaceDisplayIdx;

				if (cursorPos == 1) triggerTest();

			}

			break;

		case menuExitIdx:
			retVal = baseMenuDisplayIdx;
			break;

		default:
			break;

	}

	return retVal;

}

static void accelerationTest::triggerTest(void)
{

	uint8_t retVal;

	if (accelerationFlags & accelTestInProgress)
	{

		// reset accel test capture flags, and signal that accel test is cancelled
		changeBitFlags(accelerationFlags, accelTestClearFlags, accelTestCompleteFlags);

		// force manual accel test triggering
		EEPROM::writeByte(pDragAutoFlagIdx, 0);

		retVal = attTriggerForced;

	}
	else
	{

		if (activityFlags & afVehicleStoppedFlag) // if vehicle is stopped, set drag trigger
		{

			changeBitFlags(accelerationFlags, accelTestClearFlags, 0); // turn off all acceleration test functionality in interrupt-land

			tripVar::reset(dragRawHalfSpeedIdx); // zero out acceleration 1/2 speed setpoint data
			tripVar::reset(dragRawFullSpeedIdx); // zero out acceleration full speed setpoint data
			tripVar::reset(dragRawDistanceIdx); // zero out acceleration distance setpoint data

			changeBitFlags(accelerationFlags, 0, (accelTestTriggered | accelTestMeasurementFlags)); // set drag flags in accelerationFlags register

			retVal = attTriggerNormal;

		}
		else // otherwise, alert driver that vehicle must be stopped
		{

			retVal = attVehicleStillMoving; // vehicle is still moving - can't trigger drag mode

		}

	}

	text::statusOut(devLCD, accelTestTriggerMsgs, retVal);

}

static void accelerationTest::idleProcess(void)
{

	uint8_t oldSREG;
	uint8_t i;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	accelTestStatus = accelerationFlags; // copy accel test flag status to this loop

	SREG = oldSREG; // restore interrupt flag status

	i = (lastAccelTestStatus ^ accelTestStatus) & accelTestClearFlags; // detect any drag race flag changes

	lastAccelTestStatus = accelTestStatus; // copy current accel test flag status for next loop

	if (i)
	{

		i = accelTestStatus & accelTestClearFlags;

		switch (i)
		{

			case (accelTestTriggered | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
				accelTestState = atsReady;
				break;

			case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
				accelTestState = atsActive;
				break;

			case (accelTestFinished):
				SWEET64::runPrgm(prgmTransferAccelTestTrips, 0);
				accelTestState = atsFinished;
				break;

			case (accelTestFinished | accelTestCancelled):
				accelTestState = atsCancelled;
				break;

			case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed):
				accelTestState = atsCheckPointDist;
				break;

			case (accelTestActive | accelTestFullSpeed | accelTestDistance):
				accelTestState = atsCheckPointHalf;
				break;

			case (accelTestActive | accelTestFullSpeed):
				accelTestState = atsCheckPointDistHalf;
				break;

			case (accelTestActive | accelTestHalfSpeed | accelTestDistance):
				accelTestState = atsCheckPointFull;
				break;

			case (accelTestActive | accelTestHalfSpeed):
				accelTestState = atsCheckPointDistFull;
				break;

			case (accelTestActive | accelTestDistance):
				accelTestState = atsCheckPointHalfFull;
				break;

			case (accelTestActive):
				accelTestState = atsCheckPointDistHalfFull;
				break;

			default:
				accelTestState = atsInvalidState;
				break;

		}

	}
	else accelTestState = atsNoStatusChange;

}

#endif // defined(useDragRaceFunction)
