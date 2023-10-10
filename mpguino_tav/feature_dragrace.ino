#if defined(useDragRaceFunction)
 /* Acceleration Test support section */

// upon successful arming, the drag race function will measure times to reach the distance specified by the EEPROM parameter pDragDistanceIdx (preset to 1/4 mile),
// and to reach the speed specified by the EEPROM parameter pDragSpeedIdx (preset to 60 MPH).

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
				text::statusOut(m8DevLCDidx, accelTestStateMsgs, accelTestState);
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
			text::stringOut(m8DevLCDidx, accelTestMenuTitles, cursorPos);
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

				text::stringOut(m8DevLCDidx, pBuff); // output supplementary information
				text::newLine(m8DevLCDidx); // clear to the end of the line

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

	if (volatile8Variables[(uint16_t)(v8AccelerationFlagsIdx - v8VariableStartIdx)] & accelTestInProgress)
	{

		// reset accel test capture flags, and signal that accel test is cancelled
		heart::changeBitFlagBits(v8AccelerationFlagsIdx - v8VariableStartIdx, accelTestClearFlags, accelTestCompleteFlags);

		// force manual accel test triggering
		EEPROM::writeByte(pDragAutoFlagIdx, 0);

		retVal = attTriggerForced;

	}
	else
	{

		if (volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afVehicleStoppedFlag) // if vehicle is stopped, set drag trigger
		{

			heart::changeBitFlagBits(v8AccelerationFlagsIdx - v8VariableStartIdx, accelTestClearFlags, 0); // turn off all acceleration test functionality in interrupt-land

			tripVar::reset(dragRawHalfSpeedIdx); // zero out acceleration 1/2 speed setpoint data
			tripVar::reset(dragRawFullSpeedIdx); // zero out acceleration full speed setpoint data
			tripVar::reset(dragRawDistanceIdx); // zero out acceleration distance setpoint data

			heart::changeBitFlagBits(v8AccelerationFlagsIdx - v8VariableStartIdx, 0, (accelTestTriggered | accelTestMeasurementFlags)); // set drag flags in v8AccelerationFlagsIdx register

			retVal = attTriggerNormal;

		}
		else // otherwise, alert driver that vehicle must be stopped
		{

			retVal = attVehicleStillMoving; // vehicle is still moving - can't trigger drag mode

		}

	}

	text::statusOut(m8DevLCDidx, accelTestTriggerMsgs, retVal);

}

#endif // defined(useDragRaceFunction)
