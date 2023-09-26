// this ISR gets called every time timer 0 overflows.
//
// f(fast PWM) = f(system clock) / (N * 256)
//
// N - prescaler, which is 64
//
// so this ISR gets called every 256 * 64 / (system clock) seconds
//   - 20 MHz clock -> once every 0.8192 ms
//   - 16 MHz clock -> once every 1.024 ms
//
ISR( TIMER0_OVF_vect ) // system timer interrupt handler
{

#if defined(useCoastDownCalculator)
	static uint32_t coastdownCount;
#endif // defined(useCoastDownCalculator)
#if defined(useBarFuelEconVsTime)
	static uint32_t FEvTimeCount;
#endif // defined(useBarFuelEconVsTime)

	static uint16_t activityTimeoutCount;
	static uint16_t inputTimeoutCount;
	static uint16_t parkTimeoutCount;
	static uint16_t swapFEwithFCRcount;
	static uint16_t sampleCount;
	static uint16_t updateCount;
	static uint16_t cursorCount;
#if defined(useButtonInput)
	static uint16_t buttonLongPressCount;
#if defined(useTWIbuttons) || defined(useAnalogButtons)
	static uint16_t buttonSampleCount;
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
#endif // defined(useButtonInput)
#if defined(useJSONoutput)
	static uint16_t JSONtimeoutCount;
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	static uint16_t bluetoothPeriodCount;
#endif // defined(useBluetooth)
#if defined(useBluetoothAdaFruitSPI)
	static uint16_t BLEtimeoutCount;
#endif // defined(useBluetoothAdaFruitSPI)

	static uint8_t previousActivity;
	static uint8_t internalFlags;
#if defined(useInterruptBasedTWI)
	static uint8_t TWIsampleState;
	static uint8_t nextTWIsampleState;
	static uint8_t TWIsampleRegister;
	static uint8_t TWIsampleAddress;
	static uint8_t TWIsampleLength;
#endif // defined(useInterruptBasedTWI)

	uint32_t thisTime;

	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cResetTimer)
	{

		timer0_overflow_count = 0; // initialize timer 0 overflow counter
		thisTime = TCNT0;
		internalFlags = (internalResetCursorTimer);

		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] &= ~(t0cResetTimer); // acknowledge reset request
		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] |= (t0cResetInputTimer); // reset display cursor

		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] = 0; // initialize status flags
		volatile8Variables[(uint16_t)(v8Timer0StatusB - v8VariableStartIdx)] = 0;
		volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dGoodVehicleDrive);

		volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] = 0; // initialize dedicated delay flags
		volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] = 0;

		sampleCount = delay0TickSampleLoop;
		updateCount = delay0TickUpdateLoop;
		mainLoopHeartBeat = 0x01;

		volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] = 0;
		activityTimeoutCount = volatile16Variables[(uint16_t)(v16ActivityTimeoutIdx - v16VariableStartIdx)];
		parkTimeoutCount = 0;
		swapFEwithFCRcount = 0;

		volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] = (afActivityCheckFlags | afSwapFEwithFCR);
		previousActivity = (afActivityCheckFlags);

#if defined(useButtonInput)
		buttonLongPressCount = 0;
#if defined(useTWIbuttons) || defined(useAnalogButtons)
		buttonSampleCount = delay0TickButtonSample;
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)

#endif // defined(useButtonInput)
#if defined(useAnalogRead)
		volatile8Variables[(uint16_t)(v8AnalogStatus - v8VariableStartIdx)] = asHardwareReady;

#endif // defined(useAnalogRead)
#if defined(useInterruptBasedTWI)
		TWIsampleState = 0;

#endif // defined(useInterruptBasedTWI)
#if defined(useBluetooth)
		bluetoothPeriodCount = delay0TickSampleLoop;

#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] |= (t0cResetFEvTime);

#endif // defined(useBarFuelEconVsTime)
#if defined(useBluetoothAdaFruitSPI)
		BLEtimeoutCount = 0;

#endif // defined(useBluetoothAdaFruitSPI)
	}
	else
	{

		timer0_overflow_count += 256; // update TOV count
		thisTime = timer0_overflow_count | TCNT0; // calculate current cycle count

#if defined(useCPUreading)
		volatile32Variables[(uint16_t)(v32SystemCycleIdx - v32VariableStartIdx)]++; // update systemcycles

#endif // defined(useCPUreading)
#if defined(useClockDisplay)
		// update clockcycles - if clockcycles goes past day length in timer0 ticks, roll back to 0
#if defined(useSoftwareClock)
		if ((++volatile32Variables[(uint16_t)(v32ClockCycleIdx - v32VariableStartIdx)]) >= t0TicksPerDay)
			volatile32Variables[(uint16_t)(v32ClockCycleIdx - v32VariableStartIdx)] = 0;
#endif // defined(useSoftwareClock)
#if defined(useDS1307clock)
		if ((++volatile32Variables[(uint16_t)(v32ClockCycleIdx - v32VariableStartIdx)]) >= (2 * t0TicksPerDay))
		{

			volatile32Variables[(uint16_t)(v32ClockCycleIdx - v32VariableStartIdx)] = t0TicksPerDay;
			internalFlags |= (internalReadTWIrtc);

		}
#endif // defined(useDS1307clock)

#endif // defined(useClockDisplay)
	}

	if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeOnInjector) // if MPGuino is awake on detected fuel injector event
	{

		if (watchdogInjectorCount) // if the fuel injector watchdog timer is running on minimum good engine speed
		{

			watchdogInjectorCount--; // cycle fuel injector watchdog timer down

#if defined(useChryslerMAPCorrection)
			if (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dSampleADC) // if injector monitor commanded an analog engine sensor read
			{

				volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dSampleADC); // acknowledge the command
				volatile8Variables[(uint16_t)(v8AnalogCommand - v8VariableStartIdx)] |= (acSampleChrysler);

			}

#endif // defined(useChryslerMAPCorrection)
		}
		else // fuel injector watchdog timer has timed out
		{

			volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] &= ~(aAwakeOnInjector); // signal that MPGuino is not awake any more due to no detected injector event during injector watchdog period
			volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dGoodEngineRun); // reset all fuel injector measurement flags
			watchdogInjectorCount = volatile16Variables[(uint16_t)(v16EngineOffTimeoutIdx - v16VariableStartIdx)]; // start the fuel injector watchdog for engine off mode

		}

	}
	else // MPGuino is no longer awake due to no detected fuel injector events
	{

		if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeEngineRunning) // if MPGuino is still awake due to running engine
		{

			if (watchdogInjectorCount) watchdogInjectorCount--; // cycle fuel injector watchdog timer down for engine off flag mode
			else
			{

				volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] |= (afEngineOffFlag); // flag engine as being off
				volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] &= ~(aAwakeEngineRunning); // MPGuino is no longer awake due to engine running

			}

		}

	}

	if (VSScount) // if there is a VSS debounce countdown in progress
	{

		VSScount--; // bump down the VSS count
		if (VSScount == 0) heart::updateVSS(thisTime); // if count has reached zero, go update VSS

	}

	if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeOnVSS) // if MPGuino is awake on detected VSS pulse event
	{

		if (watchdogVSSCount) // if the VSS watchdog timer is running on minimum good vehicle speed
		{

			watchdogVSSCount--;

		}
		else // VSS watchdog timer has timed out on minimum good vehicle speed
		{

			volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] &= ~(aAwakeOnVSS); // signal that MPGuino is no longer awake due to no detected VSS pulse event during VSS watchdog period
			volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dGoodVehicleMotion); // reset all VSS measurement flags
			watchdogVSSCount = volatile16Variables[(uint16_t)(v16VehicleStopTimeoutIdx - v16VariableStartIdx)]; // start the VSS watchdog for vehicle stopped mode
			swapFEwithFCRcount = delay0Tick3000ms; // reset swap timer counter

		}

	}
	else // MPGuino is no longer awake due to no detected VSS pulse events
	{

		if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeVehicleMoving) // if MPGuino is awake due to detected vehicle movement
		{

			if (watchdogVSSCount) watchdogVSSCount--;// cycle VSS watchdog timer down for vehicle stopped flag mode
			else
			{

				volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] |= (afVehicleStoppedFlag); // flag vehicle as stopped
				volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] &= ~(aAwakeVehicleMoving); // vehicle is no longer awake on detected vehicle movement

#if defined(useDragRaceFunction)
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestActive) // if accel test function is active
				{

					volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] &= ~(accelTestClearFlags); // reset accel test capture flags
					volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] |= (accelTestCompleteFlags); // signal that accel test is cancelled
					volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saAccelTestFlag);

				}

#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
				if (volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] & cdTestInProgress) // if coastdown test has started
				{

					volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] &= ~(cdTestClearFlags); // signal that coastdown test is no longer active
					volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] |= (cdTestCompleteFlags); // signal that coastdown test is cancelled
					volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saCoastdownTestFlag);

				}

#endif // defined(useCoastDownCalculator)
			}

		}

		if ((volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afSwapFEwithFCR) == 0) // if not yet showing fuel consumption rate instead of fuel economy
		{

			if (swapFEwithFCRcount) swapFEwithFCRcount--; // cycle down fuel display watchdog until it zeroes out
			else volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] |= (afSwapFEwithFCR); // output fuel consumption rate function instead of fuel economy

		}

	}

#if defined(useBarFuelEconVsTime)
	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cResetFEvTime) FEvTperiodIdx = FEvsTimeIdx; // initialize fuel econ vs time trip index variable
	else
	{

		if (FEvTimeCount) FEvTimeCount--;
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] |= (t0cResetFEvTime);
			FEvTperiodIdx++;
			if (FEvTperiodIdx > FEvsTimeEndIdx) FEvTperiodIdx -= bgDataSize;

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cResetFEvTime)
	{

		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] &= ~(t0cResetFEvTime);
		tripVar::reset(FEvTperiodIdx); // reset source trip variable
		FEvTimeCount = volatile32Variables[(uint16_t)(v32FEvsTimePeriodTimeoutIdx - v32VariableStartIdx)];

	}

#endif // defined(useBarFuelEconVsTime)
#if defined(useCoastDownCalculator)
	if (volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] & cdTestTriggered) // if coastdown test has been requested
	{

		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saCoastdownTestFlag); // signal to main program that coastdown flags have changed
		volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] &= ~(cdTestTriggered | cdTestSampleTaken); // clear coastdown test state
		volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] |= (cdTestActive); // mark coastdown test as active
		coastdownCount = volatile32Variables[(uint16_t)(v32CoastdownPeriodIdx - v32VariableStartIdx)]; // reset coastdown timer
		coastdownState = v32CoastdownMeasurement1Idx; // reset coastdown state

	}

	if (volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] & cdTestSampleTaken)
	{

		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saCoastdownTestFlag); // signal to main program that coastdown flags have changed
		volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] &= ~(cdTestSampleTaken);
		coastdownState++;

		if (coastdownState < v32CoastdownPeriodIdx - v32VariableStartIdx) // if coastdown state is still valid
		{

			coastdownCount = volatile32Variables[(uint16_t)(v32CoastdownPeriodIdx - v32VariableStartIdx)]; // reset coastdown timer

		}
		else // otherwise, signal that coastdown test ended normally
		{

			volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] &= ~(cdTestActive); // make coastdown test no longer active
			volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] |= cdTestFinished; // signal that coastdown test finished normally

		}

	}

	if (volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] & cdTestActive) // if coastdown test is active
	{

		if (coastdownCount) coastdownCount--; // if coastdown timer hasn't elapsed
		else if ((volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] & cdTestSampleTaken) == 0) volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] |= (cdTestTakeSample); // otherwise, signal VSS handler to take a coastdown sample

	}

#endif // defined(useCoastDownCalculator)
#if defined(useDS1307clock)
	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cReadRTC)
	{

		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] &= ~(t0cReadRTC); // acknowledge RTC read request
		internalFlags |= (internalReadTWIrtc); // generate TWI RTC read request

	}

#endif // defined(useDS1307clock)
#if defined(useTWIbuttons) || defined(useAnalogButtons)
	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cEnableButtonSampling)
	{

		if (buttonSampleCount) buttonSampleCount--;
		else
		{

			buttonSampleCount = delay0TickButtonSample;
#if defined(useTWIbuttons)
			internalFlags |= (internalReadTWIbutton); // generate TWI button read request
#endif // defined(useTWIbuttons)
#if defined(useAnalogButtons)
			volatile8Variables[(uint16_t)(v8AnalogCommand - v8VariableStartIdx)] |= (acSampleButtonChannel); // go sample analog button channel
#endif // defined(useAnalogButtons)

		}

	}

#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
#if defined(useInterruptBasedTWI)
	if (volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] & twiBlockMainProgram)
	{

		if ((volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] & twiOpenMain) == twiBlockMainProgram) // if TWI section is finished processing
			switch (TWIsampleState)
			{

				case 1: // TWI sample request has finished, now set up for TWI read sample
					TWI::openChannel(TWIsampleAddress, TW_READ); // open TWI as master receiver
					twiDataBufferLen = TWIsampleLength - 1; // set number of bytes to read before stop is sent
					TWI::transmitChannel(TWI_STOP); // go commit to read, send stop when read is finished

					TWIsampleState++; // advance to waiting for TWI sample to complete

					break;

				case 2: // TWI read sample has finished, check if error occurred
					if ((volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] & twiErrorFlag) == 0)
					{

						TWIsampleState = nextTWIsampleState;
						break;

					}
				default:
					volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] &= ~(twiBlockMainProgram);
					break;

#if defined(useDS1307clock)
				case 10:
					for (uint8_t x = 0; x < TWIsampleLength; x++) volatile8Variables[(uint16_t)(v8RTCsecondIdx + x - v8VariableStartIdx)] = twiDataBuffer[(uint16_t)(x)];

					// if the RTC clock is not halted, notify main program that RTC time has been read in
					if ((volatile8Variables[(uint16_t)(v8RTCsecondIdx - v8VariableStartIdx)] & 0x80) == 0) volatile8Variables[(uint16_t)(v8Timer0StatusB - v8VariableStartIdx)] |= (t0sbReadRTC);

					volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] &= ~(twiBlockMainProgram);
					break;

#endif // defined(useDS1307clock)
#if defined(useTWIbuttons)
				case 20:
					if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cEnableButtonSampling)
					{

						thisButtonState = (twiDataBuffer[0] & buttonMask); // fetch button state that was just read in
						volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] |= (t0cProcessButton); // send timer0 notification that a button was just read in

					}

					volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] &= ~(twiBlockMainProgram);
					break;

#endif // defined(useTWIbuttons)
			}

	}
	else
	{

		if ((volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] & twiOpenInterrupt) == twiAllowISRactivity) // wait for TWI to be closed and allow ISR activity
		{

			TWIsampleState = 1;

			if (internalFlags & internalReadTWIrtc)
			{

				internalFlags &= ~(internalReadTWIrtc); // acknowledge TWI RTC read request

#if defined(useDS1307clock)
				TWIsampleAddress = TWIaddressRTC; // specify TWI RTC clock device address
				TWIsampleLength = 8; // set up to read in all date and time bytes, and RTC status byte

				TWIsampleRegister = 0; // specify RTC seconds register
				internalFlags |= (internalWriteTWIbyte);

				nextTWIsampleState = 10;

#else // defined(useDS1307clock)
				TWIsampleState = 0;

#endif // defined(useDS1307clock)
			}
			else if (internalFlags & internalReadTWIbutton)
			{

				internalFlags &= ~(internalReadTWIbutton); // acknowledge TWI button read request

#if defined(useTWIbuttons)
				TWIsampleAddress = TWIaddressButton; // specify TWI button device address
				TWIsampleLength = 1; // set up to read in 1 byte

#if defined(useAdafruitRGBLCDshield)
				TWIsampleRegister = MCP23017_B1_GPIOA; // specify bank A GPIO pin register
				internalFlags |= (internalWriteTWIbyte);

#else // defined(useAdafruitRGBLCDshield)
				internalFlags &= ~(internalWriteTWIbyte); // do not write out supplemental TWI register byte

#endif // defined(useAdafruitRGBLCDshield)
				nextTWIsampleState = 20;

#else // defined(useTWIbuttons)
				TWIsampleState = 0;

#endif // defined(useTWIbuttons)
			}
			else TWIsampleState = 0;

			// if a valid TWI sample state exists, block main program from making any TWI requests, and initiate TWI sample request
			if (TWIsampleState)
			{

				volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] |= (twiBlockMainProgram);

				TWI::openChannel(TWIsampleAddress, TW_WRITE); // open TWI as master transmitter

				if (internalFlags & internalWriteTWIbyte) TWI::writeByte(TWIsampleRegister); // if TWI register byte is to be output, go output it

				TWI::transmitChannel(TWI_REPEAT_START); // go write out read request, with repeated start to set up for read

			}

		}

	}

#endif // defined(useInterruptBasedTWI)
#if defined(useButtonInput)
	if (internalFlags & internalProcessButton) // handle button debounce and long-press detection
	{

		if (buttonLongPressCount) // if button debounce / long-press countdown is in progress
		{

			buttonLongPressCount--; // bump down the button long-press count by one

			if ((internalFlags & internalButtonValid) == 0) // if button debounce timeout period has not yet elapsed
			{

				if (buttonLongPressCount < (delay0Tick1000ms - delay0Tick50ms))
				{

					internalFlags |= (internalButtonValid); // signal that button state is valid
					buttonPress = thisButtonState; // transfer button state to button press output variable

				}

			}

		}
		else // otherwise, long-press countdown has finished
		{

			buttonPress |= (longButtonBit); // signal that a "long" button press has been detected
			internalFlags |= (internalOutputButton);

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cProcessButton) // if button hardware reports reading in a button
	{

		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] &= ~(t0cProcessButton); // acknowledge report

		if (thisButtonState != lastButtonState) // if there was a button state change since the last button was read in
		{

			if (thisButtonState == buttonsUp) // if it's all buttons being released
			{

				if (internalFlags & internalButtonValid) internalFlags |= (internalOutputButton);

			}
			else
			{

				internalFlags &= ~(internalButtonValid); // reset button valid bit
				internalFlags |= (internalProcessButton);
				buttonLongPressCount = delay0Tick1000ms; // start the button debounce and long-press timer

			}

		}

		lastButtonState = thisButtonState;

	}

	if (internalFlags & internalOutputButton)
	{

		internalFlags &= ~(internalOutputButton | internalProcessButton | internalButtonValid);
		internalFlags |= (internalResetCursorTimer); // reset cursor timer

		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] |= (t0cResetInputTimer); // reset input activity timer

		// reset all display delays in progress
		volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)]);
		volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] = 0;

		// only pass button press if MPGuino was already awake
		if ((volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afActivityTimeoutFlag) == 0) volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saReadButton);

		buttonLongPressCount = 0; // reset button long-press timer

	}

#endif // defined(useButtonInput)
#if defined(useJSONoutput)
	if (JSONtimeoutCount) JSONtimeoutCount--;
	else
	{

		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saJSONchangeSubtitle); // signal to JSON output routine to display next round of subtitles
		JSONtimeoutCount = delay0Tick1600ms; // restart JSON output timeout count

	}

#endif // defined(useJSONoutput)
#if defined(useBluetoothAdaFruitSPI)
	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cEnableBLEsample)
	{

		if (BLEtimeoutCount) BLEtimeoutCount--;
		else
		{

			BLEtimeoutCount = delay0Tick100ms;
			volatile8Variables[(uint16_t)(v8Timer0StatusB - v8VariableStartIdx)] |= (t0sbSampleBLEfriend);

		}

	}

#endif // defined(useBluetoothAdaFruitSPI)
	if (sampleCount) sampleCount--;
	else
	{

		sampleCount = delay0TickSampleLoop; // restart sample loop count

		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saTakeSample); // signal to main program that a sampling should occur

		if (mainLoopHeartBeat == 0x80) mainLoopHeartBeat = 0x01; // wrap around the heartbeat bit, if necessary
		else mainLoopHeartBeat <<= 1; // cycle the heartbeat bit

#if defined(useAnalogRead)
		volatile8Variables[(uint16_t)(v8AnalogCommand - v8VariableStartIdx)] |= (acSampleChannelInit); // go sample all non-critical channels

#endif // useAnalogRead
	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x01)
	{

		if (timer0DelayCount[0]) timer0DelayCount[0]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x01); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x01) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x01); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x02)
	{

		if (timer0DelayCount[1]) timer0DelayCount[1]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x02); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x02) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x02); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x04)
	{

		if (timer0DelayCount[2]) timer0DelayCount[2]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x04); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x04) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x04); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x08)
	{

		if (timer0DelayCount[3]) timer0DelayCount[3]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x08); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x08) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x08); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x10)
	{

		if (timer0DelayCount[4]) timer0DelayCount[4]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x10); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x10) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x10); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x20)
	{

		if (timer0DelayCount[5]) timer0DelayCount[5]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x20); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x20) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x20); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x40)
	{

		if (timer0DelayCount[6]) timer0DelayCount[6]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x40); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x40) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x40); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & 0x80)
	{

		if (timer0DelayCount[7]) timer0DelayCount[7]--; // bump timer delay value down by one tick
		else
		{

			volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(0x80); // signal to main program that delay timer has completed main program request
			if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] & 0x80) // if this was a display delay
			{

				volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] &= ~(0x80); // clear display delay flag
				if (volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] == 0) internalFlags |= (internalResetUpdateTimer); // reset update loop

			}

		}

	}

	if (volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] & t0cResetInputTimer) // if user input was received
	{

		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] &= ~(t0cResetInputTimer); // acknowledge request
		volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] |= (aAwakeOnInput); // set awake status on input received
		volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] &= ~(afUserInputFlag | afActivityTimeoutFlag);

		inputTimeoutCount = volatile16Variables[(uint16_t)(v16InputTimeoutIdx - v16VariableStartIdx)];

	}

	if (internalFlags & internalResetCursorTimer)
	{

		internalFlags &= ~(internalResetCursorTimer);
		internalFlags |= (internalResetUpdateTimer);
		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saShowCursor);
		cursorCount = delay0Tick100ms; // reset cursor count

	}
	else
	{

		if (cursorCount) cursorCount--;
		else
		{

			cursorCount = delay0Tick500ms; // reset cursor count
			volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] ^= (t0saShowCursor); // toggle cursor show bit

		}

	}

	if (updateCount) updateCount--;
	else internalFlags |= (internalResetUpdateTimer);

	if (internalFlags & internalResetUpdateTimer)
	{

		internalFlags &= ~(internalResetUpdateTimer);

		volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saUpdateDisplay); // signal to main program to update the user display
		updateCount = delay0TickUpdateLoop; // restart display update loop count

	}

	if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeOnInput)
	{

		if (inputTimeoutCount) inputTimeoutCount--;
		else
		{

			volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] &= ~(aAwakeOnInput);
			volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] |= (afUserInputFlag);

		}

	}

	if ((volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afParkCheckFlags) == afNotParkedFlags) // if MPGuino has engine stop and vehicle stop flags set, but is not yet parked
	{

		if (parkTimeoutCount) parkTimeoutCount--; // run down park watchdog timer until it expires
		else volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] |= (afParkFlag); // set vehicle parked flag

	}

	if ((volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afValidFlags) == afActivityCheckFlags) // if there is no activity but the activity watchdog hasn't timed out yet
	{

		if (activityTimeoutCount) activityTimeoutCount--; // cycle down the activity timeout watchdog
		else volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] |= (afActivityTimeoutFlag); // signal that MPGuino is in a period of inactivity

	}

	previousActivity ^= (volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afValidFlags); // detect any activity change since last timer0 tick

	if (previousActivity) volatile8Variables[(uint16_t)(v8ActivityChange - v8VariableStartIdx)] |= (previousActivity); // if there was any activity change at all, signal that the display needs updating

	// reset activity timeout watchdog if any of the fuel injector, VSS pulse, button press, or park flags have changed
	if (previousActivity & afActivityCheckFlags) activityTimeoutCount = volatile16Variables[(uint16_t)(v16ActivityTimeoutIdx - v16VariableStartIdx)];

	// reset park timeout watchdog if any of the fuel injector or VSS pulse flags have changed
	if (previousActivity & afNotParkedFlags) parkTimeoutCount = volatile16Variables[(uint16_t)(v16ParkTimeoutIdx - v16VariableStartIdx)];

	previousActivity = (volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afValidFlags); // save for next timer0 tick

#if defined(useAnalogRead)
	if (volatile8Variables[(uint16_t)(v8AnalogCommand - v8VariableStartIdx)] & acSampleChannelActive)
	{

		if (volatile8Variables[(uint16_t)(v8AnalogStatus - v8VariableStartIdx)] & asHardwareReady)
		{

			volatile8Variables[(uint16_t)(v8AnalogCommand - v8VariableStartIdx)] |= (acSampleGround); // signal to ADC interrupt that the last requested conversion was for internal ground
			volatile8Variables[(uint16_t)(v8AnalogStatus - v8VariableStartIdx)] &= ~(asHardwareReady);

			ADMUX = pgm_read_byte(&analogChannelValue[(uint16_t)(v16AnalogGroundIdx - v16AnalogStartIdx)]);
			ADCSRA |= ((1 << ADSC) | (1 << ADIF) | (1 << ADIE)); // start ADC read, enable interrupt, and clear interrupt flag, because this crappy hardware allows the ADC interrupt to alway do free running mode

		}

	}

#endif // useAnalogRead
#if defined(useDebugCPUreading)
	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += TCNT0;

#endif // defined(useDebugCPUreading)
}

#if defined(useTimer1Interrupt)
// this ISR gets called every time timer 1 overflows.
//
// f(phase correct PWM) = f(system clock) / (N * 510)
//
// N - prescaler, which is 1
//
// so this ISR gets called every 510 * 1 / (system clock) seconds (for 20 MHz clock, that is every 25.5 us)
ISR( TIMER1_OVF_vect ) // LCD delay interrupt handler
{

#if defined(useBluetoothAdaFruitSPI)
	static uint16_t responseDelay;
	static uint16_t chipSelectDelay;
	static uint16_t resetDelay;
	static uint8_t chipSelectState;
	static uint8_t resetState;
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(use4BitLCD)
	static uint8_t value;
#endif // defined(use4BitLCD)
#if defined(useSimulatedFIandVSS)
	static uint32_t debugVSSresetCount;
	static uint32_t debugFIPresetCount;
#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	if (volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] & t1cResetTimer)
	{

		volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] &= ~(t1cResetTimer);
		volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] = 0;
#if defined(useBluetoothAdaFruitSPI)
		chipSelectState = 0;
		resetState = 0;
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useSimulatedFIandVSS)
		debugVSScount = 0;
		debugFIPcount = 0;
		debugFIPWcount = 0;
		debugVSSresetCount = 0;
		debugFIPresetCount = 0;
#endif // defined(useSimulatedFIandVSS)

	}

#if defined(useBluetoothAdaFruitSPI)
	if (volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleReset) // if main program requests bluetooth hardware reset
	{

		volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] &= ~(bleReset | bleAssertFlags | blePacketWaitFlags); // clear any in-progress lesser waiting tasks
		resetState = 1; // initialize reset state machine
		chipSelectState = 0; // halt CS state machine

	}

	if (volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleAssert) // if main program requests to assert /CS
	{

		if ((volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleResetting) == 0) // wait until reset is complete
		{

			volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] &= ~(bleAssert); // acknowledge main program command
			chipSelectState = 1; // initialize CS state machine

		}

	}

	if (volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & blePacketWait) // if main program requests waiting for a SDEP packet wait delay
	{

		if ((volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleResetting) == 0) // wait until reset is complete
		{

			volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] &= ~(blePacketWait); // acknowledge main program command
			responseDelay = delay1Tick250ms; // initialize response delay wait counter

		}

	}

	if (volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleResetting) // if hardware reset is in progress
	{

		switch (resetState)
		{

			case 1: // release /CS pin and assert /RST pin
				blefriend::releaseCS();
#if defined(__AVR_ATmega32U4__)
				PORTD &= ~(1 << PORTD4);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTG &= ~(1 << PORTG5);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTD &= ~(1 << PORTD4);
#endif // defined(__AVR_ATmega328P__)
				resetDelay = delay1Tick10ms; // cause MPGuino to assert /RST for 10 ms
				resetState++;
				break;

			case 3: // release /RST pin
#if defined(__AVR_ATmega32U4__)
				PORTD |= (1 << PORTD4);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTG |= (1 << PORTG5);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTD |= (1 << PORTD4);
#endif // defined(__AVR_ATmega328P__)
				resetDelay = delay1Tick1s; // cause MPGuino to wait on just-reset BLE hardware for 1 sec
				resetState++;
				break;

			case 4:
			case 2: // perform /RST delay
				if (resetDelay) resetDelay--;
				else resetState++;
				break;

			default: // catch invalid reset states
				volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] &= ~(bleResetting); // mark hardware reset as completed
				break;

		}

	}

	if (volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleAsserting) // if /CS assertion is in progress
	{

		switch (chipSelectState)
		{

			case 1: // release /CS pin if it's asserted
				if (blefriend::isCSreleased()) chipSelectState += 2; // if /CS is not asserted, skip ahead
				else // otherwise, release /CS and wait
				{

					blefriend::releaseCS();
					chipSelectDelay = delay1Tick50us; // reset /CS delay timer
					chipSelectState++;
					break;

				}

			case 3: // assert /CS pin
				blefriend::assertCS();
				chipSelectDelay = delay1Tick100us; // reset CS delay timer
				chipSelectState++;
				break;

			case 4:
			case 2: // perform /CS delay
				if (chipSelectDelay) chipSelectDelay--;
				else chipSelectState++;
				break;

			default: // catch invalid chip select states
				volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] &= ~(bleAsserting); // mark chip select assert as completed
				break;

		}

	}

	if (volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & blePacketWaiting) // if response delay is in progress
	{

		if (responseDelay) responseDelay--; // if response delay counter still valid, bump down by one
		else
		{

			volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] &= ~(blePacketWaiting); // otherwise, signal that response delay timed out
			blefriend::releaseCS();

		}

	}

#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useSimulatedFIandVSS)
	if (volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] & t1cEnableDebug)
	{

		if ((volatile8Variables[(uint16_t)(v8SignalSimModeFlags - v8VariableStartIdx)] & debugVSreadyFlags) == debugVSreadyFlags) // if VSS simulator is ready to output
		{

			if (debugVSScount) debugVSScount--;
			else
			{

				debugVSScount = debugVSStickLength;
#if defined(__AVR_ATmega32U4__)
				PORTB ^= (1 << PORTB7); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTK ^= (1 << PORTK0); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTC ^= (1 << PORTC0); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega328P__)

			}

		}

		if (volatile8Variables[(uint16_t)(v8SignalSimModeFlags - v8VariableStartIdx)] & debugVSSflag) // if VSS simulator is enabled
		{

			if (debugVSSresetCount) debugVSSresetCount--;
			else
			{

				debugVSSresetCount = debugVSSresetLength;
				volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] |= (t1sDebugUpdateVSS);

			}

		}

		if ((volatile8Variables[(uint16_t)(v8SignalSimModeFlags - v8VariableStartIdx)] & debugFIreadyFlags) == debugFIreadyFlags) // if fuel injector simulator is ready to output
		{

			if (debugFIPcount)
			{

				debugFIPcount--;

				if (debugFIPWcount) debugFIPWcount--;
				else
				{

#if defined(__AVR_ATmega32U4__)
					PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to generate injector closed interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE |= ((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin high to generate injector closed interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to generate injector closed interrupt
#endif // defined(__AVR_ATmega328P__)

				}

			}
			else
			{

				debugFIPcount = debugFIPtickLength;
				debugFIPWcount = debugFIPWtickLength;
				if (debugFIPWtickLength) // if DFCO is not commanded
				{

#if defined(__AVR_ATmega32U4__)
					PORTD &= ~((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin low to generate injector open interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE &= ~((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin low to generate injector open interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD &= ~((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin low to generate injector open interrupt
#endif // defined(__AVR_ATmega328P__)

				}

			}

		}

		if (volatile8Variables[(uint16_t)(v8SignalSimModeFlags - v8VariableStartIdx)] & debugInjectorFlag) // if injector simulator is enabled
		{

			if (debugFIPresetCount) debugFIPresetCount--;
			else
			{

				debugFIPresetCount = debugFIPresetLength;
				volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] |= (t1sDebugUpdateFIP);

			}

		}

	}

#endif // defined(useSimulatedFIandVSS)
#if defined(useLCDoutput)
	if (volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] & t1cDelayLCD)
	{

		if (lcdDelayCount) lcdDelayCount--;
#if defined(useLCDbufferedOutput)
		else
		{

			if (ringBuffer::isBufferNotEmpty(rbIdxLCD)) // if there's at least one nybble in the LCD send buffer
			{

#if defined(useTWI4BitLCD)
				// if buffer is not empty and TWI hardware is ready
				if ((volatile8Variables[(uint16_t)(v8TWIstatus - v8VariableStartIdx)] & twiOpenMain) == 0)
				{

					volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] &= ~(t1sDoOutputTWI); // reset TWI master transmission in progress flag
					volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] |= (t1sLoopFlag); // set loop flag

					do
					{

						value = ringBuffer::pull(rbIdxLCD); // pull a buffered LCD nybble

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] & t1sDoOutputTWI) == 0) // if this is the first nybble to be output
							{

								TWI::openChannel(TWIaddressLCD, TW_WRITE); // open TWI as master transmitter
#if defined(useAdafruitRGBLCDshield)
								TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address
#endif // defined(useAdafruitRGBLCDshield)
								volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] |= (t1sDoOutputTWI); // signal to complete TWI master transmission

							}

						}

						LCD::outputNybble(value); // output the nybble and set timing

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((value & lcdSendFlags) == lcdSendNybble) // if sending an ordinary data nybble, check if we can continue looping
							{

								if ((twiDataBufferSize - twiDataBufferLen) < 5) volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] &= ~(t1sLoopFlag); // if TWI send buffer is getting low, signal end of loop
								if (ringBuffer::isBufferEmpty(rbIdxLCD)) volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] &= ~(t1sLoopFlag); // if LCD send buffer is empty, signal end of loop

							}
							else volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] &= ~(t1sLoopFlag); // otherwise, this is a special (command or reset) nybble, so signal end of loop

						}
						else volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] &= ~(t1sLoopFlag); // otherwise, this is just a delay request, so signal end of loop

					}
					while (volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] & t1sLoopFlag);

					if (volatile8Variables[(uint16_t)(v8Timer1Status - v8VariableStartIdx)] & t1sDoOutputTWI) TWI::transmitChannel(TWI_STOP); // commit LCD port expander write, if required

				}

#endif // defined(useTWI4BitLCD)
#if defined(usePort4BitLCD)
				value = ringBuffer::pull(rbIdxLCD); // pull a buffered LCD byte

				LCD::outputNybble(value); // output byte

#endif // defined(usePort4BitLCD)
			}
			else volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] &= ~(t1cDelayLCD); // turn off LCD delay

		}

#else // defined(useLCDbufferedOutput)
		else volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] &= ~(t1cDelayLCD); // turn off LCD delay

#endif // defined(useLCDbufferedOutput)
	}

#endif // defined(useLCDoutput)
#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useTimer1Interrupt)
volatile unsigned long thisInjectorOpenStart;
volatile unsigned long thisEnginePeriodOpen; // engine speed measurement based on fuel injector open event
volatile unsigned long thisEnginePeriodClose; // engine speed measurement based on fuel injector close event

// fuel injector monitor interrupt pair
//
// this pair is responsible to measure fuel injector open pulse width, and engine speed
//
// the fuel injector monitor also performs a few sanity checks
//
// sanity check 1 - the engine revolution period measured must be less than the calculated period corresponding to the minimum acceptable engine RPM speed
//                  if this measured period is greater, then the fuel injector is assumed to be de-energized
//                   - the fuel injector pulse measurement is abandoned
//                   - the engine is also assumed to be turned off (for EOC mode)
//
// sanity check 2 - if a successful pulse measurement is made, the (measured pulse + injector open delay + injector close delay) must be less than the measured engine revolution period
//                  if this is not the case, the fuel injector is operating past its design duty cycle (typically 85% at 7000 RPM or something)
//                   - MPGuino may no longer be able to reliably measure fuel consumption
//                   - the main program is informed

// injector opening event handler
//
// this measures the start of the fuel injector pulse, and is used to calculate engine speed
//
#if defined(__AVR_ATmega32U4__)
ISR( INT2_vect )
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( INT4_vect )
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( INT0_vect )
#endif // defined(__AVR_ATmega328P__)
{

	static unsigned long lastInjectorOpenStart;
	unsigned int a;
#if defined(useDebugCPUreading)
	unsigned int b;
#endif // defined(useDebugCPUreading)

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjectorOpenStart = timer0_overflow_count + (unsigned long)(a);

	if (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dGoodEngineRotationOpen) thisEnginePeriodOpen = heart::findCycle0Length(lastInjectorOpenStart, thisInjectorOpenStart); // calculate length between fuel injector pulse starts
	else thisEnginePeriodOpen = 0;

#if defined(useChryslerMAPCorrection)
	volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= (dGoodEngineRotationOpen | dInjectorReadInProgress | dSampleADC);
#else // defined(useChryslerMAPCorrection)
	volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= (dGoodEngineRotationOpen | dInjectorReadInProgress);
#endif // defined(useChryslerMAPCorrection)

	lastInjectorOpenStart = thisInjectorOpenStart;

	watchdogInjectorCount = volatile16Variables[(uint16_t)(v16DetectEngineOffIdx - v16VariableStartIdx)]; // reset minimum engine speed watchdog timer

#if defined(useDebugCPUreading)
	b = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) b = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += b - a;

#endif // defined(useDebugCPUreading)
}

// injector opening event handler
//
// this measures the end of the fuel injector pulse, and is used to calculate engine speed
//
// if a fuel injector pulse width measurement is in progress, this also performs the measurement and stores raw fuel consumption data
//
// it will either store one of the good existing engine period measurements or an average of both if both are good
//
#if defined(__AVR_ATmega32U4__)
ISR( INT3_vect )
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( INT5_vect )
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( INT1_vect )
#endif // defined(__AVR_ATmega328P__)
{

	static unsigned long lastInjectorCloseStart;

	uint8_t b;
	unsigned int a;
#if defined(useDebugCPUreading)
	unsigned int c;
#endif // defined(useDebugCPUreading)
	unsigned long thisInjectorCloseStart;
	unsigned long engineRotationPeriod;
	unsigned long thisInjectorPulseLength;
	unsigned long goodInjectorPulseLength;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjectorCloseStart = timer0_overflow_count + (unsigned long)(a);

	if (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dGoodEngineRotationClose) thisEnginePeriodClose = heart::findCycle0Length(lastInjectorCloseStart, thisInjectorCloseStart); // calculate length between fuel injector pulse starts
	else thisEnginePeriodClose = 0;

	if (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dInjectorReadInProgress) // if there was a fuel injector open pulse detected, there's now a fuel injector pulse width to be measured
	{

		volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dInjectorReadInProgress);

		b = (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dGoodEngineRotation);

		switch (b)
		{

			case (dGoodEngineRotationClose):
				engineRotationPeriod = thisEnginePeriodClose;
				b = dGoodInjectorRead;
				break;

			case (dGoodEngineRotationOpen):
				engineRotationPeriod = thisEnginePeriodOpen;
				b = dGoodInjectorRead;
				break;

			case (dGoodEngineRotation):
				engineRotationPeriod = thisEnginePeriodClose + thisEnginePeriodOpen;
				engineRotationPeriod++; // perform pre-emptive rounding up from averaging operation
				engineRotationPeriod >>= 1; // perform average of two measurements
				b = dGoodInjectorRead;
				break;

			default:
				b = 0;
				break;

		}

		// calculate fuel injector pulse length
		thisInjectorPulseLength = heart::findCycle0Length(thisInjectorOpenStart, thisInjectorCloseStart) - volatile32Variables[(uint16_t)(v32InjectorOpenDelayIdx - v32VariableStartIdx)]; // strip off injector open delay time

		// if this pulse is larger than the maximum good pulse that could happen at the minimum valid engine speed, reject it
		// 1 - pulse could be narrower than v32InjectorOpenDelayIdx
		// 2 - pulse could be wider than the maximum allowable pulse width for minimum good engine speed
		if (thisInjectorPulseLength > volatile32Variables[(uint16_t)(v32InjectorValidMaxWidthIdx - v32VariableStartIdx)]) volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dGoodInjectorWidth | dGoodInjectorRead);
		else volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= (dGoodInjectorWidth);

		if (b) // if we have an engine rotation period measurement
		{

			// calculate good maximum fuel injector open time for injector pulse width sanity check
			goodInjectorPulseLength = engineRotationPeriod - volatile32Variables[(uint16_t)(v32InjectorOpenDelayIdx - v32VariableStartIdx)];

			if (thisInjectorPulseLength > goodInjectorPulseLength) volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] &= ~(dGoodInjectorRead); // if measured pulse is larger than largest good pulse, signal that last injector read may be bad
			else volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= (dGoodInjectorRead); // signal that last injector read is good

			// if measured engine speed is greater than the specified minimum good engine speed
			if (engineRotationPeriod < volatile32Variables[(uint16_t)(v32MaximumEnginePeriodIdx - v32VariableStartIdx)])
			{

				volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] &= ~(afEngineOffFlag | afParkFlag | afActivityTimeoutFlag); // signal that engine is running, and vehicle is therefore no longer parked
				volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] |= (aAwakeEngineRunning); // MPGuino is awake due to engine running

			}

#if defined(trackIdleEOCdata)
			if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeVehicleMoving) // if vehicle is moving
				// add to raw fuel injector total cycle accumulator
				tripVar::update64(collectedEngCycleCount, engineRotationPeriod, curRawTripIdx);
			else // if vehicle is not moving
				// add to raw idle fuel injector total cycle accumulator
				tripVar::update64(collectedEngCycleCount, engineRotationPeriod, curRawEOCidleTripIdx);

#else // defined(trackIdleEOCdata)
			// add to raw fuel injector total cycle accumulator
			tripVar::update64(collectedEngCycleCount, engineRotationPeriod, curRawTripIdx);

#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
			if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestActive)
			{

				// add to raw accel test distance fuel injector total cycle accumulator
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestDistance) tripVar::update64(collectedEngCycleCount, engineRotationPeriod, dragRawDistanceIdx);

				// add to raw accel test full speed fuel injector total cycle accumulator
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestFullSpeed) tripVar::update64(collectedEngCycleCount, engineRotationPeriod, dragRawFullSpeedIdx);

				// add to raw accel test half speed fuel injector total cycle accumulator
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestHalfSpeed) tripVar::update64(collectedEngCycleCount, engineRotationPeriod, dragRawHalfSpeedIdx);

			}

#endif // defined(useDragRaceFunction)
		}

		// if the injector pulse width is valid
		if (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dGoodInjectorWidth)
		{

			volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] |= (aAwakeOnInjector); // signal that MPGuino is awake due to detected injector

#if defined(useChryslerMAPCorrection)
			thisInjectorPulseLength *= volatile32Variables[(uint16_t)(v32InjectorCorrectionIdx - v32VariableStartIdx)]; // multiply by differential fuel pressure correction factor numerator
			thisInjectorPulseLength >>= 12; // divide by differential fuel pressure correction factor denominator

#endif // defined(useChryslerMAPCorrection)
#if defined(trackIdleEOCdata)
			if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeVehicleMoving) // if vehicle is moving
				// update fuel injector open cycle accumulator, and fuel injector pulse count
				tripVar::update64(collectedInjCycleCount, collectedInjPulseCount, thisInjectorPulseLength, curRawTripIdx);
			else // if vehicle is not moving
				// update idle fuel injector open cycle accumulator, and idle fuel injector pulse count
				tripVar::update64(collectedInjCycleCount, collectedInjPulseCount, thisInjectorPulseLength, curRawEOCidleTripIdx);

#else // defined(trackIdleEOCdata)
			// update fuel injector open cycle accumulator, and fuel injector pulse count
			tripVar::update64(collectedInjCycleCount, collectedInjPulseCount, thisInjectorPulseLength, curRawTripIdx);

#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
			if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestActive)
			{

				// update raw accel test distance fuel injector open cycle accumulator, and raw accel test distance fuel injector pulse count
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestDistance)
					tripVar::update64(collectedInjCycleCount, collectedInjPulseCount, thisInjectorPulseLength, dragRawDistanceIdx);

				// update raw accel test full speed fuel injector open cycle accumulator, and raw accel test full speed fuel injector pulse count
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestFullSpeed)
					tripVar::update64(collectedInjCycleCount, collectedInjPulseCount, thisInjectorPulseLength, dragRawFullSpeedIdx);

				// update raw accel test half speed fuel injector open cycle accumulator, and raw accel test half speed fuel injector pulse count
				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestHalfSpeed)
					tripVar::update64(collectedInjCycleCount, collectedInjPulseCount, thisInjectorPulseLength, dragRawHalfSpeedIdx);

			}

#endif // defined(useDragRaceFunction)
		}

	}

	volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= (dGoodEngineRotationClose);
	lastInjectorCloseStart = thisInjectorCloseStart;

	watchdogInjectorCount = volatile32Variables[(uint16_t)(v16DetectEngineOffIdx - v32VariableStartIdx)]; // reset minimum engine speed watchdog timer

#if defined(useDebugCPUreading)
	c = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) c = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c - a;

#endif // defined(useDebugCPUreading)
}

#if defined(__AVR_ATmega32U4__)
ISR( PCINT0_vect )
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( PCINT2_vect )
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( PCINT1_vect )
#endif // defined(__AVR_ATmega328P__)
{

	uint8_t p;
	uint8_t q;

	unsigned int a;
#if defined(useDebugCPUreading)
	unsigned int c;
#endif // defined(useDebugCPUreading)
	unsigned long thisTime;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisTime = timer0_overflow_count + (unsigned long)(a);

#if defined(__AVR_ATmega32U4__)
	p = PINB; // read current input pin
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	p = PINK; // read current input pin
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	p = PINC; // read current input pin
#endif // defined(__AVR_ATmega328P__)
	q = p ^ lastPINxState; // detect any changes from the last time this ISR is called

#if defined(__AVR_ATmega32U4__)
	if (q & (1 << PINB7)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	if (q & (1 << PINK0)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	if (q & (1 << PINC0)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega328P__)
	{

		if (VSSpause) VSScount = VSSpause; // if there is a VSS debounce count defined, set VSS debounce count and let system timer handle the debouncing
		else heart::updateVSS(thisTime); // otherwise, go process VSS pulse

	}

#if defined(useLegacyButtons)
	if (q & buttonMask)
	{

		thisButtonState = (p & buttonMask) ^ buttonMask; // strip out all but relevant button bits
		volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] |= (t0cProcessButton); // send timer0 notification that a button was just read in

	}

#endif // defined(useLegacyButtons)
	lastPINxState = p; // remember the current input pin state for the next time this ISR gets called

#if defined(useDebugCPUreading)
	c = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) c = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c - a;

#endif // defined(useDebugCPUreading)
}

#if defined(useBuffering)
static void ringBuffer::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	for (uint8_t x = 0; x < rbIdxCount; x++)
	{

		ringBufferDef[(uint16_t)(x)].data = (uint8_t *)(pgm_read_word(&ringBufferDefList[(uint16_t)(x)].data));
		ringBufferDef[(uint16_t)(x)].size = pgm_read_word(&ringBufferDefList[(uint16_t)(x)].size);
		ringBufferDef[(uint16_t)(x)].start = 0;
		ringBufferDef[(uint16_t)(x)].end = 0;
		ringBufferDef[(uint16_t)(x)].status = (bufferIsEmpty);

	}

	SREG = oldSREG; // restore interrupt flag status

}

static uint8_t ringBuffer::isBufferEmpty(uint8_t ringBufferIdx)
{

	return (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty);

}

static uint8_t ringBuffer::isBufferNotEmpty(uint8_t ringBufferIdx)
{

	return ((ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) == 0);

}

static uint8_t ringBuffer::isBufferFull(uint8_t ringBufferIdx)
{

	return (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsFull);

}

static void ringBuffer::pushMain(uint8_t ringBufferIdx, uint8_t value)
{

	uint8_t oldSREG;

	while (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsFull) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	push(ringBufferIdx, value);

	SREG = oldSREG; // restore interrupt flag status

}

static uint8_t ringBuffer::pullMain(uint8_t ringBufferIdx)
{

	uint8_t value;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	value = pull(ringBufferIdx);

	SREG = oldSREG; // restore interrupt flag status

	return value;

}

static uint16_t ringBuffer::lengthMain(uint8_t ringBufferIdx)
{

	uint16_t i;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	i = length(ringBufferIdx);

	SREG = oldSREG; // restore interrupt flag status

	return i;

}

static uint16_t ringBuffer::freeMain(uint8_t ringBufferIdx)
{

	uint16_t i;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	i = free(ringBufferIdx);

	SREG = oldSREG; // restore interrupt flag status

	return i;

}

static void ringBuffer::flush(uint8_t ringBufferIdx)
{

	while ((ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) == 0) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

}

static void ringBuffer::empty(uint8_t ringBufferIdx)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	ringBufferDef[(uint16_t)(ringBufferIdx)].end = ringBufferDef[(uint16_t)(ringBufferIdx)].start;
	ringBufferDef[(uint16_t)(ringBufferIdx)].status = bufferIsEmpty;

	SREG = oldSREG; // restore interrupt flag status

}

static void ringBuffer::push(uint8_t ringBufferIdx, uint8_t value)
{

	ringBufferDef[(uint16_t)(ringBufferIdx)].data[ringBufferDef[(uint16_t)(ringBufferIdx)].start++] = value; // save a buffered character

	if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) ringBufferDef[(uint16_t)(ringBufferIdx)].status &= ~(bufferIsEmpty); // mark buffer as no longer empty
	if (ringBufferDef[(uint16_t)(ringBufferIdx)].start == ringBufferDef[(uint16_t)(ringBufferIdx)].size) ringBufferDef[(uint16_t)(ringBufferIdx)].start = 0; // handle wrap-around
	if (ringBufferDef[(uint16_t)(ringBufferIdx)].start == ringBufferDef[(uint16_t)(ringBufferIdx)].end) ringBufferDef[(uint16_t)(ringBufferIdx)].status |= (bufferIsFull); // test if buffer is full

}

static uint8_t ringBuffer::pull(uint8_t ringBufferIdx)
{

	uint8_t value;

	if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) value = 0; // if buffer is empty, return a NULL
	else
	{

		value = ringBufferDef[(uint16_t)(ringBufferIdx)].data[ringBufferDef[(uint16_t)(ringBufferIdx)].end++]; // pull a buffered character

		if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsFull) ringBufferDef[(uint16_t)(ringBufferIdx)].status &= ~(bufferIsFull); // mark buffer as no longer full
		if (ringBufferDef[(uint16_t)(ringBufferIdx)].end == ringBufferDef[(uint16_t)(ringBufferIdx)].size) ringBufferDef[(uint16_t)(ringBufferIdx)].end = 0; // handle wrap-around
		if (ringBufferDef[(uint16_t)(ringBufferIdx)].end == ringBufferDef[(uint16_t)(ringBufferIdx)].start) ringBufferDef[(uint16_t)(ringBufferIdx)].status |= (bufferIsEmpty); // test if buffer is empty

	}

	return value;

}

static uint16_t ringBuffer::length(uint8_t ringBufferIdx)
{

	uint16_t i;

	if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsFull) i = ringBufferDef[(uint16_t)(ringBufferIdx)].size;
	else if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) i = 0;
	else if (ringBufferDef[(uint16_t)(ringBufferIdx)].end < ringBufferDef[(uint16_t)(ringBufferIdx)].start) i = (ringBufferDef[(uint16_t)(ringBufferIdx)].start - ringBufferDef[(uint16_t)(ringBufferIdx)].end);
	else
	{

		i = ringBufferDef[(uint16_t)(ringBufferIdx)].size - ringBufferDef[(uint16_t)(ringBufferIdx)].end;
		i += ringBufferDef[(uint16_t)(ringBufferIdx)].start;

	}

	return i;

}

static uint16_t ringBuffer::free(uint8_t ringBufferIdx)
{

	uint16_t i;

	if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsFull) i = 0;
	else if (ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) i = ringBufferDef[(uint16_t)(ringBufferIdx)].size;
	else if (ringBufferDef[(uint16_t)(ringBufferIdx)].end > ringBufferDef[(uint16_t)(ringBufferIdx)].start) i = (ringBufferDef[(uint16_t)(ringBufferIdx)].end - ringBufferDef[(uint16_t)(ringBufferIdx)].start);
	else
	{

		i = ringBufferDef[(uint16_t)(ringBufferIdx)].size - ringBufferDef[(uint16_t)(ringBufferIdx)].start;
		i += ringBufferDef[(uint16_t)(ringBufferIdx)].end;

	}

	return i;

}

#endif // defined(useBuffering)
/* core MPGuino system support section */

static void heart::updateVSS(uint32_t thisVSStime)
{

	static uint32_t lastVSStime;
#if defined(useDragRaceFunction)
	static uint32_t accelTestDistanceCount;
	static uint32_t accelTestVSStime;

#endif // defined(useDragRaceFunction)
	static uint32_t cycleLength;

	if (volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] & dGoodVSSsignal) // if a valid VSS signal had previously been read in
	{

		volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= (dGoodVSSRead); // mark valid VSS pulse measurement
		volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] |= (aAwakeOnVSS); // MPGuino is awake on valid VSS pulse measurement

		cycleLength = heart::findCycle0Length(lastVSStime, thisVSStime); // calculate VSS pulse length

		if (cycleLength < volatile32Variables[(uint16_t)(v32MaximumVSSperiodIdx - v32VariableStartIdx)]) // if VSS period is less than that for minimum good vehicle speed
		{

			if (volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afVehicleStoppedFlag) // if vehicle has been previously flagged as not moving
			{

				volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] &= ~(afVehicleStoppedFlag | afSwapFEwithFCR | afParkFlag | afActivityTimeoutFlag); // signal that vehicle is moving, and vehicle is therefore no longer parked
				volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] |= (aAwakeVehicleMoving); // MPGuino is awake on vehicle movement

			}

		}

#if defined(trackIdleEOCdata)
		if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeEngineRunning) // if the engine is running
			// update raw VSS cycle accumulator, and raw VSS pulse count
			tripVar::update64(collectedVSScycleCount, collectedVSSpulseCount, cycleLength, curRawTripIdx);
		else // if the engine is not running
			// update raw EOC VSS cycle accumulator, and raw EOC VSS pulse count
			tripVar::update64(collectedVSScycleCount, collectedVSSpulseCount, cycleLength, curRawEOCidleTripIdx);

#else // defined(trackIdleEOCdata)
		// update raw VSS cycle accumulator, and raw VSS pulse count
		tripVar::update64(collectedVSScycleCount, collectedVSSpulseCount, cycleLength, curRawTripIdx);

#endif // defined(trackIdleEOCdata)
#if defined(useCoastDownCalculator)
		if (volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] & cdTestTakeSample) // if coastdown test is active, and a sample is requested
		{

			volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] &= ~(cdTestTakeSample); // acknowledge sample request
			volatile8Variables[(uint16_t)(v8CoastdownStatus - v8VariableStartIdx)] |= (cdTestSampleTaken); // signal that a sample has been taken
			volatile32Variables[(uint16_t)(coastdownState)] = cycleLength; // take sample

		}

#endif // defined(useCoastDownCalculator)
#if defined(useVehicleParameters)
		if (volatile8Variables[(uint16_t)(v8Awake - v8VariableStartIdx)] & aAwakeVehicleMoving) // if vehicle is considered to be moving
		{

#if defined(useDragRaceFunction)
			if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestTriggered) // if accel test function is triggered
			{

				volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] &= ~(accelTestTriggered); // switch status from 'triggered' to 'active'
				volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] |= (accelTestActive);
				volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saAccelTestFlag);

				// initialize trap distance variables
				accelTestDistanceCount = volatile32Variables[(uint16_t)(v32AccelDistanceValueIdx - v32VariableStartIdx)];
				accelTestVSStime = 0;

			}

			if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestActive) // if accel test function is active
			{

				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestDistance)
				{

					if (accelTestDistanceCount)
					{

						accelTestDistanceCount--; // count down drag distance setpoint in VSS pulses

						// update raw accel test distance VSS cycle accumulator, and raw accel test distance VSS pulse count
						tripVar::update64(collectedVSScycleCount, collectedVSSpulseCount, cycleLength, dragRawDistanceIdx);

						if (accelTestVSStime) // fetch largest instantaneous speed
						{

							if (cycleLength < accelTestVSStime) accelTestVSStime = cycleLength;

						}
						else accelTestVSStime = cycleLength;

					}
					else
					{

						volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] &= ~(accelTestDistance); // otherwise, mark drag function distance measurement as complete
						volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saAccelTestFlag);
						volatile32Variables[(uint16_t)(v32DragRawInstantSpeedIdx - v32VariableStartIdx)] = accelTestVSStime; // store maximum recorded speed
						volatile32Variables[(uint16_t)(v32DragRawTrapSpeedIdx - v32VariableStartIdx)] = cycleLength; // store trap speed

					}

				}

				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestHalfSpeed)
				{

					if (cycleLength < volatile32Variables[(uint16_t)(v32AccelHalfPeriodIdx - v32VariableStartIdx)])
					{

						volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] &= ~(accelTestHalfSpeed); // mark drag function half speed measurement as complete
						volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saAccelTestFlag);

					}
					else
						// update raw accel test half speed VSS cycle accumulator, and raw accel test half speed VSS pulse count
						tripVar::update64(collectedVSScycleCount, collectedVSSpulseCount, cycleLength, dragRawHalfSpeedIdx);

				}

				if (volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestFullSpeed)
				{

					if (cycleLength < volatile32Variables[(uint16_t)(v32AccelFullPeriodIdx - v32VariableStartIdx)])
					{

						volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] &= ~(accelTestFullSpeed); // mark drag function full speed measurement as complete
						volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saAccelTestFlag);

					}
					else
						// update raw accel test full speed VSS cycle accumulator, and raw accel test full speed VSS pulse count
						tripVar::update64(collectedVSScycleCount, collectedVSSpulseCount, cycleLength, dragRawFullSpeedIdx);

				}

				if ((volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] & accelTestMeasurementFlags) == 0) // if all drag measurements have completed, mark drag function as complete
				{

					volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] &= ~(accelTestActive); // switch status from 'active' to 'finished'
					volatile8Variables[(uint16_t)(v8AccelerationFlags - v8VariableStartIdx)] |= (accelTestFinished);
					volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] |= (t0saAccelTestFlag);

				}

			}

#endif // defined(useDragRaceFunction)
		}

#endif // defined(useVehicleParameters)
	}

	volatile8Variables[(uint16_t)(v8Dirty - v8VariableStartIdx)] |= dGoodVSSsignal; // annotate that a valid VSS pulse has been read
	watchdogVSSCount = volatile16Variables[(uint16_t)(v16DetectVehicleStopIdx - v16VariableStartIdx)]; // reset minimum engine speed watchdog timer
	lastVSStime = thisVSStime;

}

static void heart::initCore(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer0 is the taskmaster driving MPGuino's measurement functionality
#if defined(__AVR_ATmega32U4__)
	// turn on timer0 module
	PRR0 &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= (1 << AIN0D);
	DIDR2 |= ((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= ((1 << PRTWI) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRADC));
	PRR1 |= ((1 << PRUSB) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer0 module
	PRR0 &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((1 << ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= ((1 << AIN1D) | (1 << AIN0D));
	DIDR2 |= ((1 << ADC15D) | (1 << ADC14D) | (1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= ((1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC));
	PRR1 |= ((1 << PRTIM5) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer0 module
	PRR &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= ((1 << AIN1D) | (1 << AIN0D));

	// shut off on-board peripherals to reduce power consumption
	PRR |= ((1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC));

#endif // defined(__AVR_ATmega328P__)
	ACSR &= ~(1 << ACIE); // disable analog comparator interrupt
	ACSR |= (1 << ACD); // disable analog comparator module
	ADCSRB &= ~(1 << ACME); // disable analog comparator multiplexer

#if defined(useDS1307clock)
	volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] = (t0cResetTimer | t0cReadRTC);
	volatile32Variables[(uint16_t)(v32ClockCycleIdx - v32VariableStartIdx)] = t0TicksPerDay;
#else // defined(useDS1307clock)
	volatile8Variables[(uint16_t)(v8Timer0Command - v8VariableStartIdx)] = t0cResetTimer;
#endif // defined(useDS1307clock)
#if defined(useTimer1Interrupt)
	volatile8Variables[(uint16_t)(v8Timer1Command - v8VariableStartIdx)] = t1cResetTimer;
#endif // defined(useTimer1Interrupt)

	SREG = oldSREG; // restore interrupt flag status

}

static void heart::initHardware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer initialization section - multiple peripherals may use the same timer
#if defined(useTimer1)
#if defined(__AVR_ATmega32U4__)
	// turn on timer1 module
	PRR0 &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC1C to disabled
	TCCR1A &= ~((1 << COM1C1) | (1 << COM1C0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#if defined(useTimer1Interrupt)
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // defined(useTimer1Interrupt)
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // defined(useTimer1Interrupt)

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer1 module
	PRR0 &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC1C to disabled
	TCCR1A &= ~((1 << COM1C1) | (1 << COM1C0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#if defined(useTimer1Interrupt)
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // defined(useTimer1Interrupt)
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // defined(useTimer1Interrupt)

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer1 module
	PRR &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// clear timer 1 output compare force bits for OC1A and OC1B
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B));

#if defined(useTimer1Interrupt)
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // defined(useTimer1Interrupt)
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // defined(useTimer1Interrupt)

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega328P__)
#endif // defined(useTimer1)
#if defined(useTimer2)
#if defined(__AVR_ATmega2560__)
	// turn on timer2 module
	PRR0 &= ~(1 << PRTIM2);

	// set timer 2 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR2A &= ~(1 << WGM21);
	TCCR2A |= (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// set timer 2 prescale factor to 64
	TCCR2B &= ~((1 << CS22));
	TCCR2B |= ((1 << CS21) | (1 << CS20));

	// set OC2A to disabled
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

	// set OC2B to disabled
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

	// clear timer 2 output compare force bits for OC2A and OC2B
	TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

	// disable timer 2 interrupts
	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

	// clear timer 2 interrupt flags
	TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer2 module
	PRR &= ~(1 << PRTIM2);

	// set timer 2 to phase correct PWM mode, TOP = 0xFF
	TCCR2A &= ~(1 << WGM21);
	TCCR2A |= (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// set timer 2 prescale factor to 64
	TCCR2B &= ~((1 << CS22));
	TCCR2B |= ((1 << CS21) | (1 << CS20));

	// set OC2A to disabled
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

	// set OC2B to disabled
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

	// clear force bits for OC2A and OC2B
	TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

	// disable timer 2 interrupts
	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

	// clear timer 2 interrupt flags
	TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

#endif // defined(__AVR_ATmega328P__)
#endif // defined(useTimer2)
#if defined(useTimer4)
#if defined(__AVR_ATmega32U4__)
	// turn on timer4 module
	PRR1 &= ~(1 << PRTIM4);

	// set timer 4 to phase and frequency correct mode
	TCCR4D &= ~(1 << WGM41);
	TCCR4D |= (1 << WGM40);

	// set timer 4 prescale factor to 64
	TCCR4B &= ~(1 << CS43);
	TCCR4B |= ((1 << CS42) | (1 << CS41) | (1 << CS40));

	// clear timer 4 fault protection
	TCCR4D &= ~((1 << FPIE4) | (1 << FPEN4) | (1 << FPNC4) | (1 << FPES4)  | (1 << FPAC4) | (1 << FPF4));

	// set OC4A to disabled
	TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << PWM4A));

	// set OC4B to disabled
	TCCR4A &= ~((1 << COM4B1) | (1 << COM4B0) | (1 << PWM4B));

	// set OC4D to disabled
	TCCR4C &= ~((1 << COM4D1) | (1 << COM4D0) | (1 << PWM4D));

	// clear timer 4 PWM inversion mode
	TCCR4B &= ~(1 << PWM4X);

	// set timer 4 dead time prescaler to 1
	TCCR4B &= ~((1 << DTPS41) | (1 << DTPS40));

	// clear timer 4 output compare force bits for OC4A and OC4B
	TCCR4A &= ~((1 << FOC4A) | (1 << FOC4B));

	// clear timer 4 output compare force bits for OC4D
	TCCR4C &= ~(1 << FOC4D);

	// clear timer 4 update lock, disable timer 4 enhanced compare mode
	TCCR4E &= ~((1 << TLOCK4) | (1 << ENHC4));

	// disable timer 4 interrupts
	TIMSK4 &= ~((1 < OCIE4D) | (1 < OCIE4A) | (1 < OCIE4B) | (1 < TOIE4));

	// clear timer 4 interrupt flags
	TIFR4 |= ((1 << OCF4D) | (1 << OCF4A) | (1 << OCF4B) | (1 << TOV4));

	// set timer 4 dead time to 0
	DT4 = 0;

	// set timer 4 TOP value to 0x00FF, setting 8 bit mode
	TC4H = 0;
	OCR4C = 255;

#endif // defined(__AVR_ATmega32U4__)
#endif // defined(useTimer4)
	SREG = oldSREG; // restore interrupt flag status

#if defined(useBuffering)
	ringBuffer::init();
#endif // defined(useBuffering)
#if defined(useTWIsupport)
	TWI::init();
#if defined(useMCP23017portExpander)
	MCP23017portExpanderSupport::init(); // go init MCP23017 port expander
#endif // defined(useMCP23017portExpander)
#endif // defined(useTWIsupport)
#if defined(useSerial0Port)
	serial0::init();
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
	serial1::init();
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
	serial2::init();
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
	serial3::init();
#endif // defined(useSerial3Port)
#if defined(useHardwareSPI)
	spi::init();
#endif // defined(useHardwareSPI)
#if defined(__AVR_ATmega32U4__)
//	usbSupport::init();
#endif // defined(__AVR_ATmega32U4__)
#if defined(useBluetoothAdaFruitSPI)
	blefriend::init();
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useBluetooth)
	bluetooth::init();
#endif // defined(useBluetooth)
#if defined(useButtonInput)
	button::init();
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
	LCD::init();
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
	TFT::init();
#endif // defined(useTFToutput)
#if defined(useActivityLED)
	activityLED::init();
#endif // defined(useActivityLED)
#if defined(useOutputPins)
	outputPin::init();
#endif // defined(useOutputPins)

}

#ifdef useDeepSleep
static void heart::doGoDeepSleep(void)
{

#if defined(useOutputPins)
	outputPin::shutdown();
#endif // defined(useOutputPins)
#if defined(useActivityLED)
	activityLED::shutdown();
#endif // defined(useActivityLED)
	heart::changeBitFlagBits(v8Timer0Delay - v8VariableStartIdx, 0xFF, 0); // cancel any timer0 delays in progress
#if defined(useTFToutput)
	TFT::shutdown(); // shut down the TFT display
#endif // defined(useTFToutput)
#if defined(useLCDoutput)
	LCD::shutdown(); // shut down the LCD display
#endif // defined(useLCDoutput)
#if defined(useButtonInput)
	button::shutdown();
#endif // defined(useButtonInput)
#if defined(useBluetooth)
	bluetooth::shutdown();
#endif // defined(useBluetooth)
#if defined(useBluetoothAdaFruitSPI)
	blefriend::shutdown();
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(__AVR_ATmega32U4__)
//	usbSupport::shutdown();
#endif // defined(__AVR_ATmega32U4__)
#if defined(useHardwareSPI)
	spi::shutdown();
#endif // defined(useHardwareSPI)
#if defined(useSerial3Port)
	serial3::shutdown();
#endif // defined(useSerial3Port)
#if defined(useSerial2Port)
	serial2::shutdown();
#endif // defined(useSerial2Port)
#if defined(useSerial1Port)
	serial1::shutdown();
#endif // defined(useSerial1Port)
#if defined(useSerial0Port)
	serial0::shutdown();
#endif // defined(useSerial0Port)
#if defined(useTWIsupport)
	TWI::shutdown();
#endif // defined(useTWIsupport)

#if defined(useTimer4)
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM4); // shut off timer4 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)

#endif // defined(useTimer4)
#if defined(useTimer2)
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // defined(useTimer2)
#if defined(useTimer1Interrupt)
#if defined(__AVR_ATmega32U4__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega328P__)

#endif // defined(useTimer1Interrupt)
#if defined(useTimer1)
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // defined(useTimer1)
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

	initHardware(); // restart all peripherals

}

#endif // useDeepSleep
static uint32_t heart::findCycle0Length(uint32_t lastCycle, uint32_t thisCycle) // this is only to be meant to be used with interrupt handlers
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

static uint32_t heart::findCycle0Length(uint8_t lastCycleIdx) // this is only to be meant to be used with the main program
{

	uint32_t thisCycle;
	uint32_t lastCycle;

	lastCycle = mainProgram32Variables[(uint16_t)(lastCycleIdx)];
	thisCycle = cycles0();

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

static uint32_t heart::getCycle0Length(uint8_t lastCycleIdx, uint32_t thisCycle) // this is only to be meant to be used with the main program
{

	uint32_t lastCycle;

	lastCycle = mainProgram32Variables[(uint16_t)(lastCycleIdx)];

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

static uint32_t heart::cycles0(void)
{

	uint8_t oldSREG;
	uint32_t t;
	uint16_t a;

	oldSREG = SREG; // save state of interrupt flag
	cli(); // disable interrupts

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	t = timer0_overflow_count + (uint32_t)(a);

	SREG = oldSREG; // restore state of interrupt flag

	return t;

}

static void heart::wait0(uint16_t ms)
{

	uint8_t delay0Channel;

	delay0Channel = delay0(ms);
	doDelay0(delay0Channel);

}

static void heart::doDelay0(uint8_t delay0Channel)
{

	while (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & delay0Channel) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

}

static uint8_t heart::delay0(uint16_t ms)
{

	uint8_t oldSREG;
	uint8_t delay0Channel;
	uint8_t i;

	while (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] == 0xFF) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

	delay0Channel = 0x01;
	i = 0;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	while (volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & delay0Channel)
	{

		i++;
		delay0Channel <<= 1;

	}

	timer0DelayCount[(uint16_t)(i)] = ms; // request a set number of timer tick delays per millisecond

	if (ms) volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] |= (delay0Channel); // signal request to timer
	else volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(delay0Channel);

	SREG = oldSREG; // restore interrupt flag status

	return delay0Channel;

}

static void heart::delayS(uint16_t ms)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// turn off all active display delays in progress
	volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] &= ~(volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)]);
	volatile8Variables[(uint16_t)(v8Timer0DisplayDelay - v8VariableStartIdx)] = 0;

	SREG = oldSREG; // restore interrupt flag status

	if (ms) heart::changeBitFlagBits(v8Timer0DisplayDelay - v8VariableStartIdx, 0, delay0(ms));

}

static uint8_t heart::testAndResetBitFlagBit(uint8_t bitFlagIdx, uint8_t maskAND)
{

	uint8_t oldSREG;
	uint8_t retVal;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	retVal = (volatile8Variables[(uint16_t)(bitFlagIdx)] & maskAND);
	if (retVal) volatile8Variables[(uint16_t)(bitFlagIdx)] &= ~(maskAND);

	SREG = oldSREG; // restore interrupt flag status

	return retVal;

}

// this function is needed since there is no way to perform an atomic bit change of an SRAM byte value
// most MPGuino variables that are shared between main program and interrupt handlers should not need to
//    be treated as atomic (!) because only one side or the other is supposed to change said variables
// however, status flag registers are obviously an exception, and status flag changes are common
//    enough to warrant an explicit function definition
static void heart::changeBitFlagBits(uint8_t bitFlagIdx, uint8_t maskAND, uint8_t maskOR)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	volatile8Variables[(uint16_t)(bitFlagIdx)] = ((volatile8Variables[(uint16_t)(bitFlagIdx)] & ~(maskAND)) | (maskOR)); // go perform atomic status flag change

	SREG = oldSREG; // restore interrupt flag status

}

static void heart::performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

}

