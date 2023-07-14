#ifdef useDragRaceFunction
 /* Acceleration Test support section */
 
// upon successful arming, the drag race function will measure times to reach the distance specified by the EEPROM parameter pDragDistanceIdx (preset to 1/4 mile),
// and to reach the speed specified by the EEPROM parameter pDragSpeedIdx (preset to 60 MPH).


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

static void accelerationTest::displayHandler(uint8_t cmd, uint8_t cursorPos)
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

			}

			mainDisplay::outputPage(accelTestPageFormats, cursorPos, 136, 0);

			break;

		default:
			break;

	}

}

void accelerationTest::goTrigger(void)
{

	switch (triggerTest())
	{

		case 1: // vehicle is still moving - can't trigger drag mode
			text::statusOut(devLCD, PSTR("Stop vehicle"));
			break;

		case 2: // vehicle engine has stopped - can't trigger drag mode
			text::statusOut(devLCD, PSTR("Start Engine"));
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

			if (EEPROM::readByte(pDragAutoFlagIdx))
			{

				if (accelerationTest::triggerTest() == 0) accelTestState = 1;

			}

		}

	}

}

#endif // useDragRaceFunction
