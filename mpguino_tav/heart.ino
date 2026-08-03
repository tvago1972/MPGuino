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
// this ISR also contains the fuel injector monitor, which performs a few sanity checks
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
//
ISR( TIMER0_OVF_vect ) // system timer interrupt handler
{

#if defined(useCoastDownCalculator)
	static uint32_t coastdownCount;
#endif // defined(useCoastDownCalculator)

	static uint16_t activityTimeoutCount;
	static uint16_t inputTimeoutCount;
	static uint16_t parkTimeoutCount;
	static uint16_t sampleCount;
	static uint16_t updateCount;
	static uint16_t cursorCount;
#if defined(useButtonInput)
	static uint16_t buttonLongPressCount;
#if defined(useTWIbuttons) || defined(useAnalogButtons)
	static uint16_t buttonSampleCount;
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
#endif // defined(useButtonInput)
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

	uint8_t i;

#if defined(useDebugCPUreading)
	v32(v32WorkingTimer0Idx) -= TCNT0;

#endif // defined(useDebugCPUreading)
	if (v08(v8Timer0CommandIdx) & t0cResetTimer)
	{

		v08(v8Timer0CommandIdx) &= ~(t0cResetTimer); // acknowledge reset request

		internalFlags = (internalResetJSONtiming | internalResetCursorTimer); // reset cursor timer

		sampleCount = delay0TickSampleLoop;
		updateCount = delay0TickUpdateLoop;

		parkTimeoutCount = 0;

		activityTimeoutCount = v16(v16ActivityTimeoutIdx);
		previousActivity = (afActivityCheckFlags);

#if defined(useButtonInput)
#if defined(useTWIbuttons) || defined(useAnalogButtons)
		buttonSampleCount = delay0TickButtonSample;
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)

#endif // defined(useButtonInput)
#if defined(useInterruptBasedTWI)
		TWIsampleState = 0;

#endif // defined(useInterruptBasedTWI)
#if defined(useBluetoothAdaFruitSPI)
		BLEtimeoutCount = 0;

#endif // defined(useBluetoothAdaFruitSPI)
	}
	else
	{

		v32(v32Timer0OverflowCountIdx) += 256; // update TOV count

#if defined(useCPUreading)
		v32(v32SystemCycleIdx)++; // update systemcycles

#endif // defined(useCPUreading)
#if defined(useSoftwareClock)
		// update clockcycles - if clockcycles goes past day length in timer0 ticks, roll back to 0
		if ((++v32(v32ClockCycleIdx)) >= t0TicksPerDay)
			v32(v32ClockCycleIdx) = 0;

#endif // defined(useSoftwareClock)
#if defined(useRealTimeClockModule)
		// update clockcycles - if clockcycles goes past two times day length in timer0 ticks, roll back to day length in timer0 ticks
		if ((++v32(v32ClockCycleIdx)) >= (2 * t0TicksPerDay))
		{

			v32(v32ClockCycleIdx) = t0TicksPerDay;
#if defined(useTWIrtcModule)
			internalFlags |= (internalReadTWIrtc);
#endif // defined(useTWIrtcModule)

		}

#endif // defined(useRealTimeClockModule)
	}

	if (v08(v8AwakeIdx) & aAwakeOnInjector) // if MPGuino is awake on detected fuel injector event
	{

		// if there is an fuel injector pulse width read in progress
		if (v08(v8DirtyInjectorIdx) & dInjectorReadInProgress)
		{

			// if the fuel injector sample countdown is in progress
			if (v08(v8InjectorSampleCountIdx)) v08(v8InjectorSampleCountIdx)--;
			else // otherwise, the fuel injector pulse width is ready
			{

				v08(v8DirtyInjectorIdx) &= ~(dInjectorReadInProgress);

				if (v08(v8AwakeIdx) & aAwakeEngineRunning) // if the engine is running
				{

#if defined(trackIdleEOCdata)
					// if vehicle is in idle mode, add to raw idle fuel injector total cycle accumulator
					if (v08(v8ActivityIdx) & afVehicleIdleFlag) tripVar::update64(collectedEngCycleCount, v32EnginePeriodIdx, curRawEOCidleTripIdx);
					// otherwise, add to raw fuel injector total cycle accumulator
					else tripVar::update64(collectedEngCycleCount, v32EnginePeriodIdx, curRawTripIdx);

#else // defined(trackIdleEOCdata)
					// add to raw fuel injector total cycle accumulator
					tripVar::update64(collectedEngCycleCount, v32EnginePeriodIdx, curRawTripIdx);

#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
					if (v08(v8AccelerationFlagsIdx) & accelTestActive)
					{

						// add to raw accel test distance fuel injector total cycle accumulator
						if (v08(v8AccelerationFlagsIdx) & accelTestDistance) tripVar::update64(collectedEngCycleCount, v32EnginePeriodIdx, dragRawDistanceIdx);

						// add to raw accel test full speed fuel injector total cycle accumulator
						if (v08(v8AccelerationFlagsIdx) & accelTestFullSpeed) tripVar::update64(collectedEngCycleCount, v32EnginePeriodIdx, dragRawFullSpeedIdx);

						// add to raw accel test half speed fuel injector total cycle accumulator
						if (v08(v8AccelerationFlagsIdx) & accelTestHalfSpeed) tripVar::update64(collectedEngCycleCount, v32EnginePeriodIdx, dragRawHalfSpeedIdx);

					}

#endif // defined(useDragRaceFunction)
				}

				v32(v32InjectorAccumulatorIdx) -= v32(v32InjectorOpenDelayIdx); // strip off injector open delay time

				// if this pulse is larger than the maximum good pulse that could happen at the minimum valid engine speed, reject it
				// 1 - pulse could be narrower than v32InjectorOpenDelayIdx
				// 2 - pulse could be wider than the maximum allowable pulse width for minimum good engine speed
				if (v32(v32InjectorAccumulatorIdx) > v32(v32InjectorValidMaxWidthIdx)) v08(v8DirtyInjectorIdx) &= ~(dGoodInjectorRead);
				else // otherwise, the measured pulse width is valid
				{

					// calculate good maximum fuel injector open time for injector pulse width sanity check
					v32(v32EnginePeriodIdx) -= v32(v32InjectorOpenDelayIdx);

					// if measured pulse is larger than largest good pulse, signal that last injector read may be bad
					if (v32(v32InjectorAccumulatorIdx) > v32(v32EnginePeriodIdx)) v08(v8DirtyInjectorIdx) &= ~(dGoodInjectorRead);
					else
					{

						v08(v8DirtyInjectorIdx) |= (dGoodInjectorRead); // signal that last injector read is good

#if defined(useChryslerMAPCorrection)
						v32(v32InjectorAccumulatorIdx) *= v32(v32InjectorCorrectionIdx); // multiply by differential fuel pressure correction factor numerator
						v32(v32InjectorAccumulatorIdx) >>= 12; // divide by differential fuel pressure correction factor denominator

#endif // defined(useChryslerMAPCorrection)
#if defined(trackIdleEOCdata)
						if (v08(v8ActivityIdx) & afVehicleIdleFlag) // if vehicle is in idle mode
						{

							tripVar::update64(collectedInjCycleCount, v32InjectorAccumulatorIdx, curRawEOCidleTripIdx); // update raw idle injector open cycle accumulator
							collectedInjPulseCount[(uint16_t)(curRawEOCidleTripIdx)]++; // update raw idle injector pulse count

						}
						else // otherwise, vehicle is moving
						{

							tripVar::update64(collectedInjCycleCount, v32InjectorAccumulatorIdx, curRawTripIdx); // update raw injector open cycle accumulator
							collectedInjPulseCount[(uint16_t)(curRawTripIdx)]++; // update raw injector pulse count

						}

#else // defined(trackIdleEOCdata)
						tripVar::update64(collectedInjCycleCount, v32InjectorAccumulatorIdx, curRawTripIdx); // update raw injector open cycle accumulator
						collectedInjPulseCount[(uint16_t)(curRawTripIdx)]++; // update raw injector pulse count

#endif // defined(trackIdleEOCdata)
#if defined(useDragRaceFunction)
						if (v08(v8AccelerationFlagsIdx) & accelTestActive)
						{

							if (v08(v8AccelerationFlagsIdx) & accelTestDistance) // if accel test distance portion is active
							{

								tripVar::update64(collectedInjCycleCount, v32InjectorAccumulatorIdx, dragRawDistanceIdx); // update raw injector open cycle accumulator
								collectedInjPulseCount[(uint16_t)(dragRawDistanceIdx)]++; // update raw injector pulse count

							}

							if (v08(v8AccelerationFlagsIdx) & accelTestFullSpeed) // if accel test full speed portion is active
							{

								tripVar::update64(collectedInjCycleCount, v32InjectorAccumulatorIdx, dragRawFullSpeedIdx); // update raw injector open cycle accumulator
								collectedInjPulseCount[(uint16_t)(dragRawFullSpeedIdx)]++; // update raw injector pulse count

							}

							if (v08(v8AccelerationFlagsIdx) & accelTestHalfSpeed) // if accel test half speed portion is active
							{

								tripVar::update64(collectedInjCycleCount, v32InjectorAccumulatorIdx, dragRawHalfSpeedIdx); // update raw injector open cycle accumulator
								collectedInjPulseCount[(uint16_t)(dragRawHalfSpeedIdx)]++; // update raw injector pulse count

							}

						}

#endif // defined(useDragRaceFunction)
					}

				}

			}

		}

		if (v16(v16WatchdogInjectorCountIdx)) v16(v16WatchdogInjectorCountIdx)--; // cycle down engine activity timeout timer
		else // if engine activity timeout timer has timed out
		{

			if (v08(v8ActivityIdx) & afVehicleEOCflag) // if vehicle is in EOC mode
			{

				v08(v8AwakeIdx) &= ~(aAwakeOnInjector);  // MPGuino is no longer awake due to no detected fuel injector event
				v08(v8ActivityIdx) |= (afEngineOffFlag); // flag engine as being off
				v08(v8DirtyInjectorIdx) &= ~(dGoodEngineRun); // reset all fuel injector measurement flags

			}
			else // vehicle engine is running
			{

				v08(v8AwakeIdx) &= ~(aAwakeEngineRunning); // engine is no longer considered to be running
				v08(v8ActivityIdx) |= (afVehicleEOCflag); // set vehicle EOC mode flag
				v16(v16WatchdogInjectorCountIdx) = v16(v16EngineOffTimeoutIdx); // reset engine activity timeout timer

			}

		}

	}

	if (v08(v8AwakeIdx) & aAwakeOnVSS) // if MPGuino is awake on detected VSS pulse event
	{

		if (v08(v8DirtyVSSIdx) & dVSSreadInProgress) // if a VSS signal was read in
		{

			if (v08(v8VSSsampleCountIdx)) v08(v8VSSsampleCountIdx)--; // if there is a VSS debounce countdown in progress, do it
			else
			{

				v08(v8DirtyVSSIdx) &= ~(dVSSreadInProgress); // reset VSS signal read flag

				heart::updateVSS(); // go update VSS measurements

			}

		}

		if (v16(v16WatchdogVSScountIdx)) v16(v16WatchdogVSScountIdx)--; // cycle down vehicle movement activity timeout timer
		else // if vehicle movement activity timeout timer has timed out
		{

			if (v08(v8ActivityIdx) & afVehicleIdleFlag) // if vehicle is in idle mode
			{

				v08(v8AwakeIdx) &= ~(aAwakeOnVSS); // MPGuino is no longer awake due to no detected VSS pulse event during vehicle stopped watchdog timeout
				v08(v8ActivityIdx) |= (afVehicleStoppedFlag); // flag vehicle as stopped
				v08(v8DirtyVSSIdx) &= ~(dGoodVehicleMotion); // reset all VSS measurement flags

			}
			else // vehicle is moving
			{

				v08(v8AwakeIdx) &= ~(aAwakeVehicleMoving); // vehicle is no longer considered to be moving
				v08(v8ActivityIdx) |= (afVehicleIdleFlag); // set vehicle idle mode flag
				v16(v16WatchdogVSScountIdx) = v16(v16VehicleStopTimeoutIdx); // reset vehicle movement activity timeout timer

#if defined(useDragRaceFunction)
				if (v08(v8AccelerationFlagsIdx) & accelTestActive) // if accel test function is active
				{

					v08(v8AccelerationFlagsIdx) &= ~(accelTestClearFlags); // reset accel test capture flags
					v08(v8AccelerationFlagsIdx) |= (accelTestCompleteFlags); // signal that accel test is cancelled
					v08(v8Timer0Status1Idx) |= (t0sbAccelTestFlag);

				}

#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
				if (v08(v8CoastdownStatusIdx) & cdTestInProgress) // if coastdown test has started
				{

					v08(v8CoastdownStatusIdx) &= ~(cdTestClearFlags); // signal that coastdown test is no longer active
					v08(v8CoastdownStatusIdx) |= (cdTestCompleteFlags); // signal that coastdown test is cancelled
					v08(v8Timer0Status1Idx) |= (t0sbCoastdownTestFlag);

				}

#endif // defined(useCoastDownCalculator)
			}

		}

	}

#if defined(useSimulatedFIandVSS)
	if (v08(v8SignalSimModeIdx) & debugFIreadyFlags)
	{

		if (v08(v8SignalSimModeIdx) & debugInjectorFlag) // if injector simulator is enabled
		{

			if (v16(v16SignalSimFIPcycleCountIdx)) v16(v16SignalSimFIPcycleCountIdx)--;
			else
			{

				v16(v16SignalSimFIPcycleCountIdx) = delay0Tick1333ms;

				if (v08(v8SignalSimFIPidx)) v08(v8SignalSimFIPidx)--;
				else
				{

					v08(v8SignalSimFIPidx) = debugFIPlength - 1;
					v08(v8SignalSimFIPstate) += 0x40;

				}

				if (v08(v8SignalSimFIPstate) & 0x40)
				{

					if (v08(v8SignalSimFIPstate) & 0x80) i = v08(v8SignalSimFIPidx);
					else i = debugFIPlength - v08(v8SignalSimFIPidx) - 1;

					v16(v16SignalSimFIPtickIdx) = pgm_read_word(&debugFIPvalues[(uint16_t)(i)]); // read stored engine period countdown value;
					v16(v16SignalSimFIPWtickIdx) = pgm_read_word(&debugFIPWvalues[(uint16_t)(i)]); // read stored fuel injector open period value;

					v08(v8SignalSimModeIdx) |= (debugFIPready);

				}

			}

		}
		else
		{

			v08(v8SignalSimModeIdx) &= ~(debugFIPready);
			v08(v8SignalSimModeIdx) |= (debugInjectorFlag);

			v08(v8SignalSimFIPidx) = 0;
			v08(v8SignalSimFIPstate) = 0x40; // start out by waiting before ramping up from 0 RPM to 12000 RPM
			v16(v16SignalSimFIPcycleCountIdx) = 0;
			v16(v16SignalSimFIPcountIdx) = 0;
			v16(v16SignalSimFIPWcountIdx) = 0;

		}

	}

	if (v08(v8SignalSimModeIdx) & debugVSreadyFlags)
	{

		if (v08(v8SignalSimModeIdx) & debugVSSflag) // if VSS simulator is enabled
		{

			if (v16(v16SignalSimVSScycleCountIdx)) v16(v16SignalSimVSScycleCountIdx)--;
			else
			{

				v16(v16SignalSimVSScycleCountIdx) = delay0Tick2000ms;

				if (v08(v8SignalSimVSSidx)) v08(v8SignalSimVSSidx)--;
				else
				{

					v08(v8SignalSimVSSidx) = debugVSSlength - 1;
					v08(v8SignalSimVSSstate) += 0x40;

				}

				if (v08(v8SignalSimVSSstate) & 0x40)
				{

					if (v08(v8SignalSimVSSstate) & 0x80) i = v08(v8SignalSimVSSidx);
					else i = debugVSSlength - v08(v8SignalSimVSSidx) - 1;

					v16(v16SignalSimVSStickIdx) = pgm_read_word(&debugVSSvalues[(uint16_t)(i)]);

					v08(v8SignalSimModeIdx) |= (debugVSSready);

				}

			}

		}
		else
		{

			v08(v8SignalSimModeIdx) &= ~(debugVSSready);
			v08(v8SignalSimModeIdx) |= (debugVSSflag);

			v08(v8SignalSimVSSidx) = 0;
			v08(v8SignalSimVSSstate) = 0x00; // start out by ramping up from 0 MPH to 250 MPH
			v16(v16SignalSimVSScycleCountIdx) = 0;
			v16(v16SignalSimVSScountIdx) = 0;

		}

	}

#endif // defined(useSimulatedFIandVSS)
#if defined(useFEvTdata)
	if (v32(v32FEvsTimePeriodCountIdx)) v32(v32FEvsTimePeriodCountIdx)--;
	else
	{

		v32(v32FEvsTimePeriodCountIdx) = v32(v32FEvsTimePeriodTickIdx);
		v08(v8FEvTimeTripIdx)++;

		if (v08(v8FEvTimeTripIdx) >= bgDataSize) v08(v8FEvTimeTripIdx) = 0;

	}

#endif // defined(useFEvTdata)
#if defined(useCoastDownCalculator)
	if (v08(v8CoastdownStatusIdx) & cdTestTriggered) // if coastdown test has been requested
	{

		v08(v8Timer0Status1Idx) |= (t0sbCoastdownTestFlag); // signal to main program that coastdown flags have changed
		v08(v8CoastdownStatusIdx) &= ~(cdTestTriggered | cdTestSampleTaken); // clear coastdown test state
		v08(v8CoastdownStatusIdx) |= (cdTestActive); // mark coastdown test as active
		coastdownCount = v32(v32CoastdownPeriodIdx); // reset coastdown timer
		coastdownState = v32CoastdownMeasurement1Idx; // reset coastdown state

	}

	if (v08(v8CoastdownStatusIdx) & cdTestSampleTaken)
	{

		v08(v8Timer0Status1Idx) |= (t0sbCoastdownTestFlag); // signal to main program that coastdown flags have changed
		v08(v8CoastdownStatusIdx) &= ~(cdTestSampleTaken);
		coastdownState++;

		if (coastdownState < v32CoastdownPeriodIdx) // if coastdown state is still valid
		{

			coastdownCount = v32(v32CoastdownPeriodIdx); // reset coastdown timer

		}
		else // otherwise, signal that coastdown test ended normally
		{

			v08(v8CoastdownStatusIdx) &= ~(cdTestActive); // make coastdown test no longer active
			v08(v8CoastdownStatusIdx) |= cdTestFinished; // signal that coastdown test finished normally

		}

	}

	if (v08(v8CoastdownStatusIdx) & cdTestActive) // if coastdown test is active
	{

		if (coastdownCount) coastdownCount--; // if coastdown timer hasn't elapsed
		else if ((v08(v8CoastdownStatusIdx) & cdTestSampleTaken) == 0) v08(v8CoastdownStatusIdx) |= (cdTestTakeSample); // otherwise, signal VSS handler to take a coastdown sample

	}

#endif // defined(useCoastDownCalculator)
#if defined(useRealTimeClockModule)
	if (v08(v8Timer0CommandIdx) & t0cReadRTC)
	{

		v08(v8Timer0CommandIdx) &= ~(t0cReadRTC); // acknowledge RTC read request
#if defined(useTWIrtcModule)
		internalFlags |= (internalReadTWIrtc); // generate TWI RTC read request
#endif // defined(useTWIrtcModule)

	}

#endif // defined(useRealTimeClockModule)
#if defined(useTWIbuttons) || defined(useAnalogButtons)
	if (v08(v8ButtonStatusIdx) & btnCmdEnableSampling)
	{

		if (buttonSampleCount) buttonSampleCount--;
		else
		{

			buttonSampleCount = delay0TickButtonSample;
#if defined(useTWIbuttons)
			internalFlags |= (internalReadTWIbutton); // generate TWI button read request
#endif // defined(useTWIbuttons)
#if defined(useAnalogButtons)
			v08(v8AnalogCommandIdx) |= (acSampleButtonChannel); // go sample analog button channel
#endif // defined(useAnalogButtons)

		}

	}

#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
#if defined(useTouchScreenInput)
	// the touch panel's pen-down line (PENIRQ) is just a pin read, so sample it every
	// ISR tick rather than at the decimated button cadence - that catches brief taps
	// that would otherwise fall in the gap between samples. a touch counts as user
	// input: reset the activity timer so MPGuino stays awake (and wakes from idle
	// sleep) while the screen is being touched. only the cheap pen-detect happens
	// here - coordinate reads stay in the main-loop touch driver.
	// (NOTE: cannot wake from full power-down - PENIRQ is on PH4, which has no
	//  external/pin-change interrupt on the ATmega2560, and timer0 is stopped then.)
	if (touch::pressed()) v08(v8Timer0CommandIdx) |= (t0cResetInputActivityTimer);

#endif // defined(useTouchScreenInput)
#if defined(useInterruptBasedTWI)
	if (v08(v8TWIstatusIdx) & twiInterruptInUse)
	{

		if ((v08(v8TWIstatusIdx) & twiOpen) == 0) // if TWI section is finished processing
			switch (TWIsampleState)
			{

				case 1: // write sample request to TWI device
					TWI::open(TWIsampleAddress, TW_WRITE); // open TWI as master transmitter
					TWI::writeByte(TWIsampleRegister); // output device-specific address byte
					TWI::transmit(TWI_REPEAT_START); // go write out read request, with repeated start to set up for read

					TWIsampleState++; // advance to waiting for TWI sample to complete

					break;

				case 2: // TWI sample request has finished, now set up for TWI read sample
					TWI::open(TWIsampleAddress, TW_READ); // open TWI as master receiver
					twiDataBufferLen = TWIsampleLength - 1; // set number of bytes to read before stop is sent
					TWI::transmit(TWI_STOP); // go commit to read, send stop when read is finished

					TWIsampleState++; // advance to waiting for TWI sample to complete

					break;

				case 3: // TWI read sample has finished, check if error occurred
					if ((v08(v8TWIstatusIdx) & twiErrorFlag) == 0)
					{

						TWIsampleState = nextTWIsampleState;
						break;

					}
				default:
					v08(v8TWIstatusIdx) &= ~(twiInterruptInUse);
					break;

#if defined(useTWIrtcModule)
				case 10:
					v08(v8Timer0Status1Idx) &= ~(t0sbReadRTC | t0sbErrorRTC);

#if defined(useDS1307clock)
					for (uint8_t x = 0; x < TWIsampleLength; x++) v08(v8RTCsecondIdx + x) = twiDataBuffer[(uint16_t)(x)];

					// if the RTC clock is not halted, notify main program that RTC time has been read in
					if (v08(v8RTCsecondIdx) & 0x80) v08(v8Timer0Status1Idx) |= (t0sbErrorRTC);
					else v08(v8Timer0Status1Idx) |= (t0sbReadRTC);

#endif // defined(useDS1307clock)
					v08(v8TWIstatusIdx) &= ~(twiInterruptInUse);
					break;

#endif // defined(useTWIrtcModule)
#if defined(useTWIbuttons)
				case 20:
					if (v08(v8ButtonStatusIdx) & btnCmdEnableSampling)
					{

						v08(v8ThisButtonStateIdx) = (twiDataBuffer[0] & buttonMask); // fetch button state that was just read in
						v08(v8ButtonStatusIdx) |= (btnCmdInjectButton); // send notification that a button was just read in

					}

					v08(v8TWIstatusIdx) &= ~(twiInterruptInUse);
					break;

#endif // defined(useTWIbuttons)
			}

	}
	else
	{

		if ((v08(v8TWIstatusIdx) & twiInUse) == 0) // wait for TWI to be closed and allow ISR activity
		{

#if defined(useTWIrtcModule)
			if (internalFlags & internalReadTWIrtc)
			{

				internalFlags &= ~(internalReadTWIrtc); // acknowledge TWI RTC read request

				TWIsampleAddress = TWIaddressRTC; // specify TWI RTC clock device address
#if defined(useDS1307clock)
				TWIsampleLength = 8; // set up to read in all date and time bytes, and RTC status byte
				TWIsampleRegister = 0; // specify RTC seconds register
#endif // defined(useDS1307clock)
				TWIsampleState = 1; // next state is to write out RTC register address request
				nextTWIsampleState = 10; // final state is to process the TWI port input as RTC input data

			}
			else
#endif // defined(useTWIrtcModule)
			if (internalFlags & internalReadTWIbutton)
			{

				internalFlags &= ~(internalReadTWIbutton); // acknowledge TWI button read request

#if defined(useTWIbuttons)
				TWIsampleAddress = TWIaddressButton; // specify TWI button device address
				TWIsampleLength = 1; // set up to read in 1 byte
#if defined(useAdafruitRGBLCDbuttons)
				TWIsampleRegister = MCP23017_B1_GPIOA; // specify bank A GPIO pin register
				TWIsampleState = 1; // next state is to write out bank A GPIO pin register address request
#else // defined(useAdafruitRGBLCDbuttons)
				TWIsampleState = 2;	// next state is to read the TWI device port
#endif // defined(useAdafruitRGBLCDbuttons)
				nextTWIsampleState = 20; // final state is to process the TWI port input as a button press

#else // defined(useTWIbuttons)
				TWIsampleState = 0;

#endif // defined(useTWIbuttons)
			}
			else TWIsampleState = 0;

			// if a valid TWI sample state exists, block main program from making any TWI requests, and initiate TWI sample request
			if (TWIsampleState) v08(v8TWIstatusIdx) |= (twiInterruptInUse);

		}

	}

#endif // defined(useInterruptBasedTWI)
#if defined(useButtonInput)
	if (v08(v8ButtonStatusIdx) & btnCmdProcessButton) // if button hardware reports reading in a button
	{

		v08(v8ButtonStatusIdx) &= ~(btnCmdProcessButton); // acknowledge report

		// if there was a button state change since the last button was read in
		if (v08(v8ThisButtonStateIdx) != v08(v8LastButtonStateIdx))
		{

			internalFlags &= ~(internalButtonValid); // reset button valid bit

			v08(v8LastButtonStateIdx) = v08(v8ThisButtonStateIdx); // remember this as previously read button state

			internalFlags |= (internalProcessButton);
			buttonLongPressCount = delay0Tick1000ms; // start the button debounce and long-press timer

		}

	}

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
					v08(v8ButtonStatusIdx) &= ~(btnStatusDetectShortPress); // clear short-press status bit

					// if this is 'buttons-up' being debounced, just send out the button combo just previously read
					if (v08(v8ThisButtonStateIdx) == buttonsUp)
					{

						// if long button press was already sent, just reset processing
						if (v08(v8ButtonPressIdx) & longButtonBit) internalFlags &= ~(internalOutputButton | internalProcessButton | internalButtonValid);
						else internalFlags |= (internalOutputButton); // otherwise, send out short button press

					}
					else v08(v8ButtonPressIdx) = v08(v8ThisButtonStateIdx); // otherwise, save button combo for later

				}

			}

		}
		else // otherwise, long-press countdown has finished
		{

			v08(v8ButtonPressIdx) |= (longButtonBit); // signal that a "long" button press has been detected
			v08(v8ButtonStatusIdx) &= ~(btnStatusDetectLongPress); // clear long-press status bit
			internalFlags |= (internalOutputButton); // send out long button press

		}

	}

	if (internalFlags & internalOutputButton)
	{

		internalFlags &= ~(internalOutputButton | internalProcessButton | internalButtonValid); // reset button processing
		internalFlags |= (internalResetCursorTimer); // reset cursor timer

		v08(v8Timer0CommandIdx) |= (t0cResetInputActivityTimer); // reset user input activity timer

		// only pass button press if MPGuino was already awake
		if ((v08(v8ActivityIdx) & afActivityTimeoutFlag) == 0) v08(v8ButtonStatusIdx) |= (btnStatusButtonRead);

	}

#endif // defined(useButtonInput)
#if defined(useBluetooth)
	if (v08(v8btOutputStatusIdx) & btoDelayFlags)
	{

		if (v08(v8btOutputStatusIdx) & btoDelayInitFlags)
		{

			if (v08(v8btOutputStatusIdx) & btoFlagDelayInit500ms) v16(v16BluetoothDelayCount) = delay0Tick500ms;
			else if (v08(v8btOutputStatusIdx) & btoFlagDelayInit20ms) v16(v16BluetoothDelayCount) = delay0Tick20ms;
			else v16(v16BluetoothDelayCount) = 0;

			v08(v8btOutputStatusIdx) &= ~(btoDelayInitFlags);

		}
		else
		{

			if (v16(v16BluetoothDelayCount)) v16(v16BluetoothDelayCount)--;
			else v08(v8btOutputStatusIdx) &= ~(btoFlagDelay);

		}

	}

#endif // defined(useBluetooth)
	if (sampleCount) sampleCount--;
	else
	{

		sampleCount = delay0TickSampleLoop; // restart sample loop count

		v08(v8Timer0Status0Idx) |= (t0saTakeSample); // signal to main program that a sampling should occur

		if (v08(v8HeartbeatBitmaskIdx) == 0x80) // wrap around the heartbeat bit, if necessary
			v08(v8HeartbeatBitmaskIdx) = 0x01;
		else v08(v8HeartbeatBitmaskIdx) <<= 1; // cycle the heartbeat bit

#if defined(useAnalogRead)
		v08(v8AnalogCommandIdx) |= (acSampleChannelInit); // go sample all non-critical channels

#endif // defined(useAnalogRead)
	}

#if defined(useJSONoutput)
	if (v08(v8Timer0CommandIdx) & t0cEnableJSONoutput)
	{

		// replaced timerChecker with this because it's a more accurate method to change once every 1.6 seconds
		if (v08(v8AwakeIdx) & aAwakeOnVehicle)
		{

			if ((v08(v8Timer0Status0Idx) & t0saOutputJSON) == 0) v08(v8Timer0Status0Idx) |= (t0saOutputJSON);

			if (v16(v16JSONsubtitleCount)) v16(v16JSONsubtitleCount)--;
			else
			{

				v16(v16JSONsubtitleCount) = delay0Tick1600ms;

				// signal to JSON output routine to display next round of subtitles
				if (!(--v08(v8Subtitle1Idx))) v08(v8Subtitle1Idx) = 2;
#if defined(useDragRaceFunction)
				if (!(--v08(v8Subtitle2Idx))) v08(v8Subtitle2Idx) = 3;
#endif // defined(useDragRaceFunction)

			}

		}
		else
		{

			if (v08(v8Timer0Status0Idx) & t0saOutputJSON) internalFlags |= (internalResetJSONtiming);

		}

	}
	else
	{

		if (v08(v8Timer0Status0Idx) & t0saOutputJSON) internalFlags |= (internalResetJSONtiming);

	}

	if (internalFlags & internalResetJSONtiming)
	{

		internalFlags &= ~(internalResetJSONtiming);

		v08(v8Timer0Status0Idx) &= ~(t0saOutputJSON);

		v16(v16JSONsubtitleCount) = delay0Tick1600ms;

		v08(v8Subtitle1Idx) = 2;
#if defined(useDragRaceFunction)
		v08(v8Subtitle2Idx) = 3;
#endif // defined(useDragRaceFunction)

	}

#endif // defined(useJSONoutput)
	if (v08(v8Timer0CommandIdx) & t0cResetInputActivityTimer) // if user input was received
	{

		v08(v8Timer0CommandIdx) &= ~(t0cResetInputActivityTimer); // acknowledge request
		v08(v8AwakeIdx) |= (aAwakeOnInput); // set awake status on input received
		v08(v8ActivityIdx) &= ~(afUserInputFlag | afActivityTimeoutFlag);

		inputTimeoutCount = v16(v16InputTimeoutIdx);

	}

	if (internalFlags & internalResetCursorTimer)
	{

		internalFlags &= ~(internalResetCursorTimer);
		v08(v8Timer0CommandIdx) |= (t0cResetOutputTimer);
		v08(v8Timer0Status0Idx) |= (t0saShowCursor);
		cursorCount = delay0Tick100ms; // reset cursor count

	}
	else
	{

		if (cursorCount) cursorCount--;
		else
		{

			cursorCount = delay0Tick500ms; // reset cursor count
			v08(v8Timer0Status0Idx) ^= (t0saShowCursor); // toggle cursor show bit

		}

	}
	
	if (v08(v8Timer0Status0Idx) & t0saDisplayDelayFlags)
	{

		if (v08(v8Timer0Status0Idx) & t0saDisplayDelayInit)
		{

			v08(v8Timer0Status0Idx) &= ~(t0saDisplayDelayInit);
			v08(v8Timer0CommandIdx) |= (t0cResetOutputTimer);
			v16(v16DisplayDelayCountIdx) = delay0Tick2000ms;

		}
		else
		{

			if (v16(v16DisplayDelayCountIdx)) v16(v16DisplayDelayCountIdx)--;
			else
			{

				v08(v8Timer0Status0Idx) &= ~(t0saDisplayDelayActive);
				v08(v8Timer0CommandIdx) |= (t0cResetOutputTimer);

			}

		}

	}

	if (updateCount) updateCount--;
	else v08(v8Timer0CommandIdx) |= (t0cResetOutputTimer);

	if (v08(v8Timer0CommandIdx) & t0cResetOutputTimer)
	{

		v08(v8Timer0CommandIdx) &= ~(t0cResetOutputTimer);

		v08(v8Timer0Status0Idx) |= (t0saUpdateDisplay); // signal to main program to update the user display
		updateCount = delay0TickUpdateLoop; // restart display update loop count

	}

	if (v08(v8AwakeIdx) & aAwakeOnInput)
	{

		if (inputTimeoutCount) inputTimeoutCount--;
		else
		{

			v08(v8AwakeIdx) &= ~(aAwakeOnInput);
			v08(v8ActivityIdx) |= (afUserInputFlag);

		}

	}

	if ((v08(v8ActivityIdx) & afParkCheckFlags) == afNotParkedFlags) // if MPGuino has engine stop and vehicle stop flags set, but is not yet parked
	{

		if (parkTimeoutCount) parkTimeoutCount--; // run down park watchdog timer until it expires
		else v08(v8ActivityIdx) |= (afParkFlag); // set vehicle parked flag

	}

	if ((v08(v8ActivityIdx) & afValidFlags) == afActivityCheckFlags) // if there is no activity but the activity watchdog hasn't timed out yet
	{

		if (activityTimeoutCount) activityTimeoutCount--; // cycle down the activity timeout watchdog
		else v08(v8ActivityIdx) |= (afActivityTimeoutFlag); // signal that MPGuino is in a period of inactivity

	}

	previousActivity ^= (v08(v8ActivityIdx) & afValidFlags); // detect any activity change since last timer0 tick

	if (previousActivity) v08(v8ActivityChangeIdx) |= (previousActivity); // if there was any activity change at all, signal that the display needs updating

	// reset activity timeout watchdog if any of the fuel injector, VSS pulse, button press, or park flags have changed
	if (previousActivity & afActivityCheckFlags) activityTimeoutCount = v16(v16ActivityTimeoutIdx);

	// reset park timeout watchdog if any of the fuel injector or VSS pulse flags have changed
	if (previousActivity & afNotParkedFlags) parkTimeoutCount = v16(v16ParkTimeoutIdx);

#if defined(useTFToutput) && !defined(useButtonInput)
	v16(v16ActivityRemainingIdx) = activityTimeoutCount; // expose the countdown for the TFT sleep bar
#endif // defined(useTFToutput) && !defined(useButtonInput)

	previousActivity = (v08(v8ActivityIdx) & afValidFlags); // save for next timer0 tick

#if defined(useAnalogRead)
	if (v08(v8AnalogCommandIdx) & acSampleChannelActive)
	{

		if (v08(v8AnalogStatusIdx) & asHardwareReady)
		{

			v08(v8AnalogCommandIdx) |= (acSampleGround); // signal to ADC interrupt that the last requested conversion was for internal ground
			v08(v8AnalogStatusIdx) &= ~(asHardwareReady);

			ADMUX = pgm_read_byte(&analogChannelValue[(uint16_t)(v16AnalogGroundIdx - v16AnalogStartIdx)]);
			ADCSRA |= (_BV(ADSC) | _BV(ADIF) | _BV(ADIE)); // start ADC read, enable interrupt, and clear interrupt flag, because this crappy hardware allows the ADC interrupt to alway do free running mode

		}

	}

#endif // defined(useAnalogRead)
#if defined(useDebugCPUreading)
	v32(v32WorkingTimer0Idx) += TCNT0;

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
//
ISR( TIMER1_OVF_vect ) // secondary timer interrupt handler
{

#if defined(useBluetoothAdaFruitSPI)
	static uint16_t responseDelay;
	static uint16_t chipSelectDelay;
	static uint16_t resetDelay;
	static uint8_t chipSelectState;
	static uint8_t resetState;
#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	if (v08(v8Timer1CommandIdx) & t1cResetTimer)
	{

		v08(v8Timer1CommandIdx) &= ~(t1cResetTimer);

#if defined(useBluetoothAdaFruitSPI)
		chipSelectState = 0;
		resetState = 0;

#endif // defined(useBluetoothAdaFruitSPI)
	}

#if defined(useBluetoothAdaFruitSPI)
	if (v08(v8BLEstatusIdx) & bleReset) // if main program requests bluetooth hardware reset
	{

		v08(v8BLEstatusIdx) &= ~(bleReset | bleAssertFlags | blePacketWaitFlags); // clear any in-progress lesser waiting tasks
		resetState = 1; // initialize reset state machine
		chipSelectState = 0; // halt CS state machine

	}

	if (v08(v8BLEstatusIdx) & bleAssert) // if main program requests to assert /CS
	{

		if ((v08(v8BLEstatusIdx) & bleResetting) == 0) // wait until reset is complete
		{

			v08(v8BLEstatusIdx) &= ~(bleAssert); // acknowledge main program command
			chipSelectState = 1; // initialize CS state machine

		}

	}

	if (v08(v8BLEstatusIdx) & blePacketWait) // if main program requests waiting for a SDEP packet wait delay
	{

		if ((v08(v8BLEstatusIdx) & bleResetting) == 0) // wait until reset is complete
		{

			v08(v8BLEstatusIdx) &= ~(blePacketWait); // acknowledge main program command
			responseDelay = delay1Tick250ms; // initialize response delay wait counter

		}

	}

	if (v08(v8BLEstatusIdx) & bleResetting) // if hardware reset is in progress
	{

		switch (resetState)
		{

			case 1: // release /CS pin and assert /RST pin
				blefriend::releaseCS();
#if defined(__AVR_ATmega32U4__)
				PORTD &= ~_BV(PORTD4);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTG &= ~_BV(PORTG5);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTD &= ~_BV(PORTD4);
#endif // defined(__AVR_ATmega328P__)
				resetDelay = delay1Tick10ms; // cause MPGuino to assert /RST for 10 ms
				resetState++;
				break;

			case 3: // release /RST pin
#if defined(__AVR_ATmega32U4__)
				PORTD |= _BV(PORTD4);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTG |= _BV(PORTG5);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTD |= _BV(PORTD4);
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
				v08(v8BLEstatusIdx) &= ~(bleResetting); // mark hardware reset as completed
				break;

		}

	}

	if (v08(v8BLEstatusIdx) & bleAsserting) // if /CS assertion is in progress
	{

		switch (chipSelectState)
		{

			case 1: // release /CS pin if it's asserted
				if (blefriend::isCSreleased()) chipSelectState += 2; // if /CS is not asserted, skip ahead
				else // otherwise, release /CS and wait
				{

					blefriend::releaseCS();
					chipSelectDelay = delay1Tick75us; // reset /CS delay timer
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
				v08(v8BLEstatusIdx) &= ~(bleAsserting); // mark chip select assert as completed
				break;

		}

	}

	if (v08(v8BLEstatusIdx) & blePacketWaiting) // if response delay is in progress
	{

		if (responseDelay) responseDelay--; // if response delay counter still valid, bump down by one
		else
		{

			v08(v8BLEstatusIdx) &= ~(blePacketWaiting); // otherwise, signal that response delay timed out
			blefriend::releaseCS();

		}

	}

#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useSimulatedFIandVSS)
	if (v08(v8Timer1CommandIdx) & t1cEnableDebug)
	{

		if ((v08(v8SignalSimModeIdx) & debugVSreadyFlags) == debugVSreadyFlags) // if VSS simulator is ready to output
		{

			if (v16(v16SignalSimVSScountIdx)) v16(v16SignalSimVSScountIdx)--;
			else
			{

#if defined(__AVR_ATmega32U4__)
				PORTB ^= _BV(PORTB7); // generate VSS pin signal
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTK ^= _BV(PORTK0); // generate VSS pin signal
#if !defined(useArduinoMega2560)
				PORTA ^= _BV(PORTA2); // generate VSS pin repeater signal
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTC ^= _BV(PORTC0); // generate VSS pin signal
#endif // defined(__AVR_ATmega328P__)

				v16(v16SignalSimVSScountIdx) = v16(v16SignalSimVSStickIdx);

			}

		}

		if ((v08(v8SignalSimModeIdx) & debugFIreadyFlags) == debugFIreadyFlags) // if fuel injector simulator is ready to output
		{

			if (v16(v16SignalSimFIPcountIdx)) v16(v16SignalSimFIPcountIdx)--;
			else
			{

				if (v16(v16SignalSimFIPWtickIdx)) // if DFCO is not commanded
				{

#if defined(__AVR_ATmega32U4__)
					PORTD &= ~(_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins low to generate injector open interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE &= ~(_BV(PORTE5) | _BV(PORTE4)); // drive injector sense pins low to generate injector open interrupt
#if !defined(useArduinoMega2560)
					PORTA &= ~(_BV(PORTA0)); // drive injector sense repeater pin low
					PORTA |= (_BV(PORTA1)); // drive injector sense repeater reference pin high
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD &= ~(_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins low to generate injector open interrupt
#endif // defined(__AVR_ATmega328P__)

					v08(v8SignalSimModeIdx) &= ~(debugPeakHoldFlags);
					v08(v8SignalSimModeIdx) |= (debugFIPfiring);
					v16(v16SignalSimFIPWcountIdx) = v16(v16SignalSimFIPWtickIdx);

					if ((v08(v8SignalSimModeIdx) & debugFIsaturatedFlag) == 0)
					{

						v08(v8SignalSimModeIdx) |= (debugFIPpeak);
						v16(v16SignalSimPeakPeriodCountIdx) = v16(v16SignalSimPeakPeriodIdx);
						v16(v16SignalSimPeakActiveCountIdx) = v16(v16SignalSimPeakActiveIdx);

					}

				}
				else // DFCO is commanded, so simulate it
				{

#if defined(__AVR_ATmega32U4__)
					PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins high to generate injector closed interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE |= (_BV(PORTE5) | _BV(PORTE4)); // drive injector sense pins high to generate injector closed interrupt
#if !defined(useArduinoMega2560)
					PORTA |= (_BV(PORTA0)); // drive injector sense repeater pin high
					PORTA &= ~(_BV(PORTA1)); // drive injector sense repeater reference pin low
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins high to generate injector closed interrupt
#endif // defined(__AVR_ATmega328P__)

					v08(v8SignalSimModeIdx) &= ~(debugFIPfiring | debugPeakHoldFlags);

				}

				v16(v16SignalSimFIPcountIdx) = v16(v16SignalSimFIPtickIdx);

			}
			
			if (v08(v8SignalSimModeIdx) & debugFIPfiring)
			{

				if (v16(v16SignalSimFIPWcountIdx)) v16(v16SignalSimFIPWcountIdx)--;
				else
				{

					v08(v8SignalSimModeIdx) &= ~(debugFIPfiring | debugPeakHoldFlags);

#if defined(__AVR_ATmega32U4__)
					PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins high to generate injector closed interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
					PORTE |= (_BV(PORTE5) | _BV(PORTE4)); // drive injector sense pins high to generate injector closed interrupt
#if !defined(useArduinoMega2560)
					PORTA |= (_BV(PORTA0)); // drive injector sense repeater pin high
					PORTA &= ~(_BV(PORTA1)); // drive injector sense repeater reference pin low
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins high to generate injector closed interrupt
#endif // defined(__AVR_ATmega328P__)

				}

				if (v08(v8SignalSimModeIdx) & debugFIPpeak)
				{

					if (v16(v16SignalSimPeakPeriodCountIdx))
					{

						v16(v16SignalSimPeakPeriodCountIdx)--;

						if ((v08(v8SignalSimModeIdx) & debugFIPhold) == 0)
						{

							if (v16(v16SignalSimPeakPeriodCountIdx) < v16(v16SignalSimPeakActiveIdx))
							{

								v08(v8SignalSimModeIdx) |= (debugFIPhold);

#if defined(__AVR_ATmega32U4__)
								PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins high to generate injector closed interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
								PORTE |= (_BV(PORTE5) | _BV(PORTE4)); // drive injector sense pins high to generate injector closed interrupt
#else // defined(useArduinoMega2560)
								PORTA |= (_BV(PORTA0)); // drive injector sense repeater pin high
#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
								PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pins high to generate injector closed interrupt
#endif // defined(__AVR_ATmega328P__)

							}

						}

					}
					else
					{

						v08(v8SignalSimModeIdx) &= ~(debugFIPpeak);
						v08(v8SignalSimModeIdx) |= (debugFIPhold);

					}

				}

				if ((v08(v8SignalSimModeIdx) & debugPeakHoldFlags) == debugFIPhold)
				{

#if defined(__AVR_ATmega32U4__)
					PORTD ^= (_BV(PORTD3) | _BV(PORTD2)); // toggle injector sense pins to simulate hold waveform
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
					PORTE ^= (_BV(PORTE5) | _BV(PORTE4)); // toggle injector sense pins to simulate hold waveform
#else // defined(useArduinoMega2560)
					PORTA ^= (_BV(PORTA0)); // toggle injector sense repeater pin
#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
					PORTD ^= (_BV(PORTD3) | _BV(PORTD2)); // toggle injector sense pins to simulate hold waveform
#endif // defined(__AVR_ATmega328P__)

				}

			}

		}

	}

#endif // defined(useSimulatedFIandVSS)
#if defined(useLCDoutput)
	if (v08(v8Timer1CommandIdx) & t1cDelayLCD)
	{

		if (lcdDelayCount) lcdDelayCount--;
		else v08(v8Timer1CommandIdx) &= ~(t1cDelayLCD); // turn off LCD delay

	}

#endif // defined(useLCDoutput)
#if defined(useBluetoothAdaFruitSPI)
	if (((v08(v8Timer1CommandIdx) & t1cEnableInterrupt) == 0) && ((v08(v8BLEstatusIdx) & bleEnableInterrupt) == 0))
#else // defined(useBluetoothAdaFruitSPI)
	if ((v08(v8Timer1CommandIdx) & t1cEnableInterrupt) == 0)
#endif // defined(useBluetoothAdaFruitSPI)
	{

		heart::disableTimer1Interrupt();

	}

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	v32(v32WorkingTimer1Idx) += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useTimer1Interrupt)
// fuel injector interrupt pair
//
// this pair is responsible to measure fuel injector open pulse width, and engine speed

// injector opening event handler
//
// this measures the start of the fuel injector pulse, accumulates engine rotation cycle measurements, tells MPGuino that a fuel injector event happened,
//    and resets the engine off watchdog timer
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

	uint16_t a;

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in timer0 cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	v32(v32ThisInjectorOpenCycleIdx) = v32(v32Timer0OverflowCountIdx) + (uint32_t)(a);

	// calculate length between fuel injector pulse starts in timer0 cycles
	if (v08(v8DirtyInjectorIdx) & dGoodInjectorOpen)
	{

		v08(v8DirtyInjectorIdx) |= (dGoodInjectorOpenPeriod);

		v32(v32EngineCycleAccumulatorIdx) -= v32(v32LastInjectorOpenCycleIdx);
		v32(v32EngineCycleAccumulatorIdx) += v32(v32ThisInjectorOpenCycleIdx);

	}
	else v32(v32EngineCycleAccumulatorIdx) = 0; // reset engine speed accumulator

	v08(v8DirtyInjectorIdx) |= (dGoodInjectorOpen);
	v08(v8AwakeIdx) |= (aAwakeOnInjector); // MPGuino is awake due to detected fuel injector event

	v32(v32LastInjectorOpenCycleIdx) = v32(v32ThisInjectorOpenCycleIdx);

	if (v08(v8ActivityIdx) & afVehicleEOCflag) v16(v16WatchdogInjectorCountIdx) = v16(v16EngineOffTimeoutIdx); // reset engine activity timeout timer
	else v16(v16WatchdogInjectorCountIdx) = v16(v16DetectEngineOffIdx); // reset minimum good engine speed watchdog timer

#if defined(useDebugCPUreading)
	v32(v32WorkingInjectorOpenIdx) -= (uint32_t)(a);

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in timer0 cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	v32(v32WorkingInjectorOpenIdx) += (uint32_t)(a);

#endif // defined(useDebugCPUreading)
}

// injector closing event handler
//
// this measures the end of the fuel injector pulse, accumulates fuel injector pulse width cycle measurements, and starts the fuel injector sample timer
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

	uint16_t a;

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in timer0 cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	v32(v32ThisInjectorCloseCycleIdx) = v32(v32Timer0OverflowCountIdx) + (uint32_t)(a);

	if (v08(v8DirtyInjectorIdx) & dInjectorReadInProgress) // if fuel injector pulse width read is already in progress
	{

		// update fuel injector pulse width accumulator with period measurement between this and previous injector close event
		v32(v32InjectorAccumulatorIdx) -= v32(v32LastInjectorCloseCycleIdx);
		v32(v32InjectorAccumulatorIdx) += v32(v32ThisInjectorCloseCycleIdx);

	}
	else // start fuel injector pulse width read
	{

		if (v08(v8DirtyInjectorIdx) & dGoodInjectorOpen) // if a valid fuel injector open pulse was measured
		{

			v08(v8DirtyInjectorIdx) |= (dInjectorReadInProgress); // mark fuel injector read in progress
#if defined(useChryslerMAPCorrection)
			v08(v8AnalogCommandIdx) |= (acSampleChrysler); // command a MAP sensor reading
#endif // defined(useChryslerMAPCorrection)

			// initialize fuel injector pulse width accumulator to initial fuel injector open pulse width
			v32(v32InjectorAccumulatorIdx) = v32(v32ThisInjectorCloseCycleIdx);
			v32(v32InjectorAccumulatorIdx) -= v32(v32ThisInjectorOpenCycleIdx);

			if (v08(v8DirtyInjectorIdx) & dGoodInjectorOpenPeriod) // if at least one valid open period was read in
			{

				if (v32(v32EngineCycleAccumulatorIdx) < v32(v32MaximumEnginePeriodIdx)) // if engine speed is greater than the specified minimum good engine speed
				{

					v08(v8AwakeIdx) |= (aAwakeEngineRunning); // the engine is considered to be running
					v16(v16WatchdogInjectorCountIdx) = v16(v16DetectEngineOffIdx); // reset minimum good engine speed watchdog timer

					// vehicle engine is running, and vehicle is therefore no longer parked or in EOC
					v08(v8ActivityIdx) &= ~(afEngineOffFlag | afVehicleEOCflag | afParkFlag | afActivityTimeoutFlag);

					v32(v32EnginePeriodIdx) = v32(v32EngineCycleAccumulatorIdx); // copy engine speed accumulator value to engine period variable

				}

			}

			v32(v32EngineCycleAccumulatorIdx) = 0; // reset engine speed accumulator

		}

	}

	if (v08(v8DirtyInjectorIdx) & dInjectorReadInProgress) v08(v8InjectorSampleCountIdx) = delay0Tick2ms; // if injector read is in progress, start the fuel injector sample timer

	v08(v8DirtyInjectorIdx) |= (dGoodInjectorClose);

	v32(v32LastInjectorCloseCycleIdx) = v32(v32ThisInjectorCloseCycleIdx);

#if defined(useDebugCPUreading)
	v32(v32WorkingInjectorCloseIdx) -= (uint32_t)(a);

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in timer0 cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	v32(v32WorkingInjectorCloseIdx) += (uint32_t)(a);

#endif // defined(useDebugCPUreading)
}

// VSS pulse detect interrupt handler
//
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

	uint32_t thisPulseCycle;

	uint16_t a;

	uint8_t p;
	uint8_t q;

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in timer0 cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisPulseCycle = v32(v32Timer0OverflowCountIdx) + (uint32_t)(a);

#if defined(useDebugCPUreading)
	v32(v32WorkingVSSpulseIdx) -= (uint32_t)(a);

#endif // defined(useDebugCPUreading)
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
	if (q & _BV(PINB7)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	if (q & _BV(PINK0)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	if (q & _BV(PINC0)) // if a VSS pulse is received
#endif // defined(__AVR_ATmega328P__)
	{

		v32(v32ThisVSSpulseCycleIdx) = thisPulseCycle;

		if (v08(v8VSSdebounceTickIdx)) // if VSS debounce is in effect
		{

			v08(v8DirtyVSSIdx) |= (dVSSreadInProgress); // signal to timer0 that a VSS pulse was read in
			v08(v8VSSsampleCountIdx) = v08(v8VSSdebounceTickIdx); // start VSS debounce countdown

		}
		else heart::updateVSS(); // otherwise, just go update VSS measurements

	}

#if defined(useLegacyButtons)
	if (q & buttonMask)
	{

		v08(v8ThisButtonStateIdx) = (p & buttonMask) ^ buttonMask; // strip out all but relevant button bits
		v08(v8ButtonStatusIdx) |= (btnCmdInjectButton); // send notification that a button was just read in

	}

#endif // defined(useLegacyButtons)
	lastPINxState = p; // remember the current input pin state for the next time this ISR gets called

#if defined(useDebugCPUreading)
	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in timer0 cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	v32(v32WorkingVSSpulseIdx) += (uint32_t)(a);

#endif // defined(useDebugCPUreading)
}

#if defined(useOutputPins)
// Output pin completion interrupt
//
// this interrupt, along with TIMER0_OVF, are capable of driving up to 16 separate pins, in either normal PWM mode or RC servomotor driver mode
// in practice, to save on memory, support is programmed for up to 8 separate pins
//
// timer0 is set to run at fast PWM mode, with a clock divider of 64
// each of the 8 channels are driven at an effective frequency of about 61 Hz (16 MHz clock) or 76 Hz (20 MHz clock)
//
ISR( TIMER0_COMPB_vect )
{

}

#endif // defined(useOutputPins)
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

static uint8_t ringBuffer::testBuffer(uint8_t ringBufferIdx, uint8_t testFlag)
{

	return (ringBufferDef[(uint16_t)(ringBufferIdx)].status & testFlag);

}

static uint8_t ringBuffer::testBufferNot(uint8_t ringBufferIdx, uint8_t testFlag)
{

	return ((ringBufferDef[(uint16_t)(ringBufferIdx)].status & testFlag) == 0);

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

#if defined(useDebugLEDactivity)
	PORTC |= (LEDdebugRingBuffer);

#endif // defined(useDebugLEDactivity)
	while ((ringBufferDef[(uint16_t)(ringBufferIdx)].status & bufferIsEmpty) == 0) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#if defined(useDebugLEDactivity)
	PORTC &= ~(LEDdebugRingBuffer);

#endif // defined(useDebugLEDactivity)
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

static void heart::initCore(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer0 is the taskmaster driving MPGuino's measurement functionality
#if defined(__AVR_ATmega32U4__)
	// turn on timer0 module
	PRR0 &= ~_BV(PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= (_BV(WGM01) | _BV(WGM00));
	TCCR0B &= ~_BV(WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~_BV(CS02);
	TCCR0B |= (_BV(CS01) | _BV(CS00));

	// set OC0A to disabled
	TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~(_BV(FOC0A) | _BV(FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~(_BV(OCIE0B) | _BV(OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= _BV(TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= (_BV(OCF0B) | _BV(OCF0A) | _BV(TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= (_BV(ADC7D) | _BV(ADC6D) | _BV(ADC5D) | _BV(ADC4D) | _BV(ADC1D) | _BV(ADC0D));
	DIDR1 |= _BV(AIN0D);
	DIDR2 |= (_BV(ADC13D) | _BV(ADC12D) | _BV(ADC11D) | _BV(ADC10D) | _BV(ADC9D) | _BV(ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= (_BV(PRTWI) | _BV(PRTIM1) | _BV(PRSPI) | _BV(PRADC));
	PRR1 |= (_BV(PRUSB) | _BV(PRTIM4) | _BV(PRTIM3) | _BV(PRUSART1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer0 module
	PRR0 &= ~_BV(PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= (_BV(WGM01) | _BV(WGM00));
	TCCR0B &= ~_BV(WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~_BV(CS02);
	TCCR0B |= (_BV(CS01) | _BV(CS00));

	// set OC0A to disabled
	TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~(_BV(FOC0A) | _BV(FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~(_BV(OCIE0B) | _BV(OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= _BV(TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= (_BV(OCF0B) | _BV(OCF0A) | _BV(TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= (_BV(ADC7D) | _BV(ADC6D) | _BV(ADC5D) | _BV(ADC4D) | _BV(ADC3D) | _BV(ADC2D) | _BV(ADC1D) | _BV(ADC0D));
	DIDR1 |= (_BV(AIN1D) | _BV(AIN0D));
	DIDR2 |= (_BV(ADC15D) | _BV(ADC14D) | _BV(ADC13D) | _BV(ADC12D) | _BV(ADC11D) | _BV(ADC10D) | _BV(ADC9D) | _BV(ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= (_BV(PRTWI) | _BV(PRTIM2) | _BV(PRTIM1) | _BV(PRSPI) | _BV(PRUSART0) | _BV(PRADC));
	PRR1 |= (_BV(PRTIM5) | _BV(PRTIM4) | _BV(PRTIM3) | _BV(PRUSART3) | _BV(PRUSART2) | _BV(PRUSART1));

#if defined(useDebugLEDactivity)
	PORTC = 0;	// turn off all port C pin outputs
	DDRC = 0xFF; // configure all port C pins as output

#endif // defined(useDebugLEDactivity)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer0 module
	PRR &= ~_BV(PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= (_BV(WGM01) | _BV(WGM00));
	TCCR0B &= ~_BV(WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~_BV(CS02);
	TCCR0B |= (_BV(CS01) | _BV(CS00));

	// set OC0A to disabled
	TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~(_BV(FOC0A) | _BV(FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~(_BV(OCIE0B) | _BV(OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= _BV(TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= (_BV(OCF0B) | _BV(OCF0A) | _BV(TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= (_BV(ADC5D) | _BV(ADC4D) | _BV(ADC3D) | _BV(ADC2D) | _BV(ADC1D) | _BV(ADC0D));
	DIDR1 |= (_BV(AIN1D) | _BV(AIN0D));

	// shut off on-board peripherals to reduce power consumption
	PRR |= (_BV(PRTWI) | _BV(PRTIM2) | _BV(PRTIM1) | _BV(PRSPI) | _BV(PRUSART0) | _BV(PRADC));

#endif // defined(__AVR_ATmega328P__)
#if defined(useAnalogRead)
#if defined(__AVR_ATmega32U4__)
	PRR0 &= ~(1 << PRADC); // turn on ADC module

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRADC); // turn on ADC module

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRADC); // turn on ADC module

#endif // defined(__AVR_ATmega328P__)
	// disable ADC free-running mode
	ADCSRA &= ~(1 << ADATE); // the AVR hardware just ignores this bit, and always goes to free running mode

	// enable ADC
	ADCSRA |= (1 << ADEN);

	// set ADMUX[5] bit to 0
	ADCSRB = 0;

	// set ADC timer frequency to 1/128 of system timer
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

	v08(v8AnalogStatusIdx) = asHardwareReady;

#endif // defined(useAnalogRead)
	ACSR &= ~_BV(ACIE); // disable analog comparator interrupt
	ACSR |= _BV(ACD); // disable analog comparator module

	v32(v32Timer0OverflowCountIdx) = 0; // initialize timer 0 overflow counter

	v08(v8Timer0Status0Idx) = 0; // initialize status flags
	v08(v8Timer0Status1Idx) = 0;
	v08(v8DirtyInjectorIdx) &= ~(dGoodEngineRun);
	v08(v8DirtyVSSIdx) &= ~(dGoodVehicleMotion);
	v08(v8AwakeIdx) = 0;
	v08(v8ActivityIdx) = (afActivityCheckFlags | afVehicleIdleFlag | afVehicleEOCflag);

	v08(v8HeartbeatBitmaskIdx) = 0x01;

	v08(v8Timer0CommandIdx) = (t0cResetTimer | t0cResetInputActivityTimer | t0cResetOutputTimer);
#if defined(useTimer1Interrupt)
	v08(v8Timer1CommandIdx) = (t1cResetTimer);
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
	PRR0 &= ~_BV(PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~_BV(WGM11);
	TCCR1A |= _BV(WGM10);
	TCCR1B &= ~(_BV(WGM13) | _BV(WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~(_BV(CS12) | _BV(CS11));
	TCCR1B |= _BV(CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~(_BV(ICNC1) | _BV(ICES1));

	// set OC1A to disabled
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~(_BV(COM1B1) | _BV(COM1B0));

	// set OC1C to disabled
	TCCR1A &= ~(_BV(COM1C1) | _BV(COM1C0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~(_BV(FOC1A) | _BV(FOC1B) | _BV(FOC1C));

	// disable timer 1 interrupts
	TIMSK1 &= ~(_BV(ICIE1) | _BV(OCIE1C) | _BV(OCIE1B) | _BV(OCIE1A) | _BV(TOIE1));

	// clear timer 1 interrupt flags
	TIFR1 |= (_BV(ICF1) | _BV(OCF1C) | _BV(OCF1B) | _BV(OCF1A) | _BV(TOV1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer1 module
	PRR0 &= ~_BV(PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~_BV(WGM11);
	TCCR1A |= _BV(WGM10);
	TCCR1B &= ~(_BV(WGM13) | _BV(WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~(_BV(CS12) | _BV(CS11));
	TCCR1B |= _BV(CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~(_BV(ICNC1) | _BV(ICES1));

	// set OC1A to disabled
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~(_BV(COM1B1) | _BV(COM1B0));

	// set OC1C to disabled
	TCCR1A &= ~(_BV(COM1C1) | _BV(COM1C0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~(_BV(FOC1A) | _BV(FOC1B) | _BV(FOC1C));

	// disable timer 1 interrupts
	TIMSK1 &= ~(_BV(ICIE1) | _BV(OCIE1C) | _BV(OCIE1B) | _BV(OCIE1A) | _BV(TOIE1));

	// clear timer 1 interrupt flags
	TIFR1 |= (_BV(ICF1) | _BV(OCF1C) | _BV(OCF1B) | _BV(OCF1A) | _BV(TOV1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer1 module
	PRR &= ~_BV(PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~_BV(WGM11);
	TCCR1A |= _BV(WGM10);
	TCCR1B &= ~(_BV(WGM13) | _BV(WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~(_BV(CS12) | _BV(CS11));
	TCCR1B |= _BV(CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~(_BV(ICNC1) | _BV(ICES1));

	// set OC1A to disabled
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~(_BV(COM1B1) | _BV(COM1B0));

	// clear timer 1 output compare force bits for OC1A and OC1B
	TCCR1C &= ~(_BV(FOC1A) | _BV(FOC1B));

	// disable timer 1 interrupts
	TIMSK1 &= ~(_BV(ICIE1) | _BV(OCIE1B) | _BV(OCIE1A) | _BV(TOIE1));

	// clear timer 1 interrupt flags
	TIFR1 |= (_BV(ICF1) | _BV(OCF1B) | _BV(OCF1A) | _BV(TOV1));

#endif // defined(__AVR_ATmega328P__)
#if defined(useTimer1Interrupt)
	enableTimer1Interrupt();

#endif // defined(useTimer1Interrupt)
#endif // defined(useTimer1)
	SREG = oldSREG; // restore interrupt flag status

#if defined(useBuffering)
	ringBuffer::init();

#endif // defined(useBuffering)
#if defined(useHardwareTWI)
	TWI::init();
#if defined(useMCP23017portExpander)
	MCP23017portExpanderSupport::init(); // go init MCP23017 port expander
#endif // defined(useMCP23017portExpander)

#endif // defined(useHardwareTWI)
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
#if defined(useTouchScreenInput)
	touch::init();
#endif // defined(useTouchScreenInput)
#if defined(useActivityLED)
	activityLED::init();
#endif // defined(useActivityLED)
#if defined(useOutputPins)
	outputPin::init();
#endif // defined(useOutputPins)
#if defined(useRealTimeClockModule)

	changeBitFlagBits(v8Timer0CommandIdx, 0, t0cReadRTC); // tell timer0 to read RTC
#endif // defined(useRealTimeClockModule)

}

static void heart::updateVSS(void)
{

	v08(v8AwakeIdx) |= (aAwakeOnVSS); // MPGuino is awake due to read in VSS pulse

	if (v08(v8ActivityIdx) & afVehicleIdleFlag) v16(v16WatchdogVSScountIdx) = v16(v16VehicleStopTimeoutIdx); // reset vehicle movement activity timeout timer
	else v16(v16WatchdogVSScountIdx) = v16(v16DetectVehicleStopIdx); // reset minimum good vehicle speed watchdog timer

	if (v08(v8DirtyVSSIdx) & dGoodVSSsignal) // if a previous valid VSS signal was read in
	{

		v32(v32VSSpulseWidth1Idx) = v32(v32VSSpulseWidth0Idx); // save last pulse width measurement
		v32(v32VSSpulseWidth0Idx) = v32(v32ThisVSSpulseCycleIdx); // calculate this VSS pulse width
		v32(v32VSSpulseWidth0Idx) -= v32(v32LastVSSpulseCycleIdx);

		if (v08(v8DirtyVSSIdx) & dGoodVSSpulse) // if a previous valid VSS pulse was read in
		{

			v32(v32VSSperiodIdx) = v32(v32VSSpulseWidth0Idx); // add VSS pulse lengths to form VSS period
			v32(v32VSSperiodIdx) += v32(v32VSSpulseWidth1Idx);

			if (v08(v8ActivityIdx) & (afVehicleIdleFlag)) // if vehicle is currently in idle mode
			{

				if (v32(v32VSSperiodIdx) < v32(v32MaximumVSSperiodIdx)) // if VSS period is less than that for minimum good vehicle speed
				{

					v08(v8AwakeIdx) |= (aAwakeVehicleMoving); // vehicle is considered to be moving
					v16(v16WatchdogVSScountIdx) = v16(v16DetectVehicleStopIdx); // reset minimum good vehicle speed watchdog timer

					// vehicle is moving, and vehicle is therefore no longer parked or in idle mode
					v08(v8ActivityIdx) &= ~(afVehicleStoppedFlag | afVehicleIdleFlag | afParkFlag | afActivityTimeoutFlag);

				}

			}

#if defined(useDragRaceFunction)
			if (v08(v8AccelerationFlagsIdx) & accelTestTriggered) // if accel test function is triggered
			{

				v08(v8AccelerationFlagsIdx) &= ~(accelTestTriggered); // switch status from 'triggered' to 'active'
				v08(v8AccelerationFlagsIdx) |= (accelTestActive);
				v08(v8Timer0Status1Idx) |= (t0sbAccelTestFlag);

				// initialize trap distance variables
				v32(v32AccelDistanceCountIdx) = v32(v32AccelDistanceValueIdx);
				v32(v32DragRawTopSpeedIdx) = 0;

			}

			if (v08(v8AccelerationFlagsIdx) & accelTestActive) // if accel test function is active
			{

				if (v32(v32DragRawTopSpeedIdx)) // fetch largest instantaneous speed
				{

					if (v32(v32VSSperiodIdx) < v32(v32DragRawTopSpeedIdx)) v32(v32DragRawTopSpeedIdx) = v32(v32VSSperiodIdx);

				}
				else v32(v32DragRawTopSpeedIdx) = v32(v32VSSperiodIdx);

				if (v08(v8AccelerationFlagsIdx) & accelTestDistance)
				{

					if (v32(v32AccelDistanceCountIdx))
					{

						v32(v32AccelDistanceCountIdx)--; // count down drag distance setpoint in VSS pulses

						tripVar::update64(collectedVSScycleCount, v32VSSpulseWidth0Idx, dragRawDistanceIdx); // update raw accel test distance VSS cycle accumulator
						collectedVSSpulseCount[(uint16_t)(dragRawDistanceIdx)]++; // update raw accel test distance VSS pulse count

					}
					else
					{

						v08(v8AccelerationFlagsIdx) &= ~(accelTestDistance); // otherwise, mark drag function distance measurement as complete
						v08(v8Timer0Status1Idx) |= (t0sbAccelTestFlag);
						v32(v32DragRawTrapSpeedIdx) = v32(v32VSSperiodIdx); // store trap speed

					}

				}

				if (v08(v8AccelerationFlagsIdx) & accelTestFullSpeed)
				{

					if (v32(v32VSSperiodIdx) < v32(v32AccelFullPeriodIdx)) // if accel test full speed has been reached
					{

						v08(v8AccelerationFlagsIdx) &= ~(accelTestFullSpeed); // mark drag function full speed measurement as complete
						v08(v8Timer0Status1Idx) |= (t0sbAccelTestFlag);

					}
					else // otherwise, record raw accel test full speed VSS data
					{

						tripVar::update64(collectedVSScycleCount, v32VSSpulseWidth0Idx, dragRawFullSpeedIdx); // update raw VSS cycle accumulator
						collectedVSSpulseCount[(uint16_t)(dragRawFullSpeedIdx)]++; // update raw VSS pulse count

					}

				}

				if (v08(v8AccelerationFlagsIdx) & accelTestHalfSpeed)
				{

					if (v32(v32VSSperiodIdx) < v32(v32AccelHalfPeriodIdx)) // if accel test half speed has been reached
					{

						v08(v8AccelerationFlagsIdx) &= ~(accelTestHalfSpeed); // mark drag function half speed measurement as complete
						v08(v8Timer0Status1Idx) |= (t0sbAccelTestFlag);

					}
					else // otherwise, record raw accel test half speed VSS data
					{

						tripVar::update64(collectedVSScycleCount, v32VSSpulseWidth0Idx, dragRawHalfSpeedIdx); // update raw VSS cycle accumulator
						collectedVSSpulseCount[(uint16_t)(dragRawHalfSpeedIdx)]++; // update raw VSS pulse count

					}

				}

				if ((v08(v8AccelerationFlagsIdx) & accelTestMeasurementFlags) == 0) // if all drag measurements have completed, mark drag function as complete
				{

					v08(v8AccelerationFlagsIdx) &= ~(accelTestActive); // switch status from 'active' to 'finished'
					v08(v8AccelerationFlagsIdx) |= (accelTestFinished);
					v08(v8Timer0Status1Idx) |= (t0sbAccelTestFlag);

				}

			}

#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
			if (v08(v8CoastdownStatusIdx) & cdTestTakeSample) // if coastdown test is active, and a sample is requested
			{

				v08(v8CoastdownStatusIdx) &= ~(cdTestTakeSample); // acknowledge sample request
				v08(v8CoastdownStatusIdx) |= (cdTestSampleTaken); // signal that a sample has been taken
				v32(coastdownState) = v32(v32VSSperiodIdx); // take sample

			}

#endif // defined(useCoastDownCalculator)
		}

		v08(v8DirtyVSSIdx) |= (dGoodVSSpulse); // if first VSS pulse was read in, mark second VSS pulse as read in

#if defined(trackIdleEOCdata)
		if (v08(v8ActivityIdx) & afVehicleEOCflag) // if vehicle is in EOC mode
		{

			tripVar::update64(collectedVSScycleCount, v32VSSpulseWidth0Idx, curRawEOCidleTripIdx); // update raw idle VSS cycle accumulator
			collectedVSSpulseCount[(uint16_t)(curRawEOCidleTripIdx)]++; // update raw idle VSS pulse count

		}
		else // otherwise, vehicle engine is running
		{

			tripVar::update64(collectedVSScycleCount, v32VSSpulseWidth0Idx, curRawTripIdx); // update raw VSS cycle accumulator
			collectedVSSpulseCount[(uint16_t)(curRawTripIdx)]++; // update raw VSS pulse count

		}

#else // defined(trackIdleEOCdata)
		tripVar::update64(collectedVSScycleCount, v32VSSpulseWidth0Idx, curRawTripIdx); // update raw VSS cycle accumulator
		collectedVSSpulseCount[(uint16_t)(curRawTripIdx)]++; // update raw VSS pulse count

#endif // defined(trackIdleEOCdata)
	}

	v08(v8DirtyVSSIdx) |= (dGoodVSSsignal); // annotate that a valid VSS pulse has been read
	v32(v32LastVSSpulseCycleIdx) = v32(v32ThisVSSpulseCycleIdx); // save this VSS pulse measurement

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
#if defined(useHardwareTWI)
	TWI::shutdown();
#endif // defined(useHardwareTWI)

#if defined(useTimer1)
#if defined(useTimer1Interrupt)
	disableTimer1Interrupt();

#endif // defined(useTimer1Interrupt)
#if defined(__AVR_ATmega32U4__)
	PRR0 |= _BV(PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 |= _BV(PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= _BV(PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // defined(useTimer1)
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

	initHardware(); // restart all peripherals

}

#endif // useDeepSleep
#if defined(useCPUreading) || defined(useDebugCPUreading)
static uint32_t heart::getCycle0Length(uint8_t lastCycleIdx) // this is only to be meant to be used with the main program
{


	uint8_t oldSREG;
	uint32_t cycle0;
	uint32_t thisCycle;
	uint16_t a;

	oldSREG = SREG; // save state of interrupt flag
	cli(); // disable interrupts

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	cycle0 = v32(v32Timer0OverflowCountIdx) + (uint32_t)(a);

	SREG = oldSREG; // restore state of interrupt flag

	if (cycle0 < m32(lastCycleIdx)) thisCycle = 4294967295ul - m32(lastCycleIdx) + cycle0 + 1;
	else thisCycle = cycle0 - m32(lastCycleIdx);

	m32(m32DbgWorkingMainStartIdx) = cycle0;

	return thisCycle;

}

static uint32_t heart::cycles0(void)
{

	uint8_t oldSREG;
	uint32_t thisCycle;
	uint16_t a;

	oldSREG = SREG; // save state of interrupt flag
	cli(); // disable interrupts

	a = (uint16_t)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & _BV(TOV0)) a = (uint16_t)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisCycle = v32(v32Timer0OverflowCountIdx) + (uint32_t)(a);

	SREG = oldSREG; // restore state of interrupt flag

	return thisCycle;

}

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
static void heart::sleepModeIdle(uint8_t bmsk)
{

#if defined(useDebugLEDactivity)
	PORTC |= (bmsk);

#endif // defined(useDebugLEDactivity)
	performSleepMode(SLEEP_MODE_IDLE);

#if defined(useDebugLEDactivity)
	PORTC &= ~(bmsk);

#endif // defined(useDebugLEDactivity)
}

#include <util/delay.h>
// blocking millisecond wait used during one-time hardware bring-up (e.g. the
// TFT hardware reset). the old cooperative timer0 delay-channel mechanism was
// removed, so this is a simple busy-wait, which is acceptable here because it
// only runs at start-up before the cooperative main loop begins. _delay_ms()
// needs a compile-time-constant argument, so spin one millisecond at a time.
static void heart::wait0(uint16_t ms)
{

	while (ms--) _delay_ms(1);

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

	v08(bitFlagIdx) = ((v08(bitFlagIdx) & ~(maskAND)) | (maskOR)); // go perform atomic status flag change

#if defined(useTimer1Interrupt)
#if defined(useBluetoothAdaFruitSPI)
	if ((bitFlagIdx == (v8BLEstatusIdx)) && (maskOR & bleEnableInterrupt)) enableTimer1Interrupt();

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useTimer1Interrupt)
	SREG = oldSREG; // restore interrupt flag status

}

static void heart::performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

}

#if defined(useTimer1Interrupt)
__attribute__((always_inline))
static void heart::enableTimer1Interrupt(void)
{

#if defined(__AVR_ATmega32U4__)
	// enable timer 1 overflow interrupt
	TIMSK1 |= _BV(TOIE1);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// enable timer 1 overflow interrupt
	TIMSK1 |= _BV(TOIE1);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// enable timer 1 overflow interrupt
	TIMSK1 |= _BV(TOIE1);
#endif // defined(__AVR_ATmega328P__)

}

__attribute__((always_inline))
static void heart::disableTimer1Interrupt(void)
{

#if defined(__AVR_ATmega32U4__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~_BV(TOIE1);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~_BV(TOIE1);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~_BV(TOIE1);
#endif // defined(__AVR_ATmega328P__)

}

#endif // defined(useTimer1Interrupt)
