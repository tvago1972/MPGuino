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

	static unsigned long lastTime;
	static unsigned long inputTimeoutCount;
	static unsigned long parkTimeoutCount;
	static unsigned long activityTimeoutCount;
	static unsigned long swapFEwithFCRcount;
#ifdef useCoastDownCalculator
	static unsigned long coastdownCount;
#endif // useCoastDownCalculator
#ifdef useBarFuelEconVsTime
	static unsigned long FEvTimeCount;
#endif // useBarFuelEconVsTime
#if defined(useButtonInput)
	static unsigned int buttonLongPressCount;
#endif // defined(useButtonInput)
	static unsigned int cursorCount;
	static unsigned int loopCount;
#ifdef useJSONoutput
	static unsigned int JSONtimeoutCount;
#endif // useJSONoutput
	static uint8_t previousActivity;
	static uint8_t internalFlags;
#if defined(useAnalogButtons)
	static uint16_t analogSampleCount;
#endif // defined(useAnalogButtons)
#if defined(useTWIbuttons)
	static uint8_t TWIsampleCount;
	static uint8_t TWIsampleState;
#endif // defined(useTWIbuttons)
	unsigned long thisTime;

	if (timer0Command & t0cResetTimer)
	{

		timer0Command &= ~(t0cResetTimer);
		timer0_overflow_count = 0; // initialize timer 0 overflow counter
		thisTime = TCNT0;
		timer0Status = 0;
		loopCount = loopTickLength;
		awakeFlags = 0;
		internalFlags = 0;
		mainLoopHeartBeat = 1;
		dirty &= ~(dGoodVehicleDrive);
		activityTimeoutCount = volatileVariables[(uint16_t)(vActivityTimeoutIdx)];
		activityFlags = (afActivityCheckFlags | afSwapFEwithFCR);
		previousActivity = (afActivityCheckFlags);
#if defined(useTWIbuttons)
		TWIsampleCount = TWItickLength;
		TWIsampleState = 0;
#endif // defined(useTWIbuttons)
#if defined(useAnalogRead)
		analogStatus = asHardwareReady;
#if defined(useAnalogButtons)
		analogSampleCount = analogSampleTickLength;
#endif // defined(useAnalogButtons)
#endif // defined(useAnalogRead)
#ifdef useLegacyButtons
		buttonDebounceCount = 0;
#endif // useLegacyButtons
#ifdef useBarFuelEconVsTime
		timer0Command |= (t0cResetFEvTime);
#endif // useBarFuelEconVsTime
#if defined(useButtonInput)
		buttonLongPressCount = 0;
#endif // defined(useButtonInput)
		inputTimeoutCount = 0;
		parkTimeoutCount = 0;
		swapFEwithFCRcount = 0;

	}
	else
	{

		timer0_overflow_count += 256; // update TOV count
		thisTime = timer0_overflow_count | TCNT0; // calculate current cycle count

#ifdef useCPUreading
		volatileVariables[(uint16_t)(vSystemCycleIdx)]++; // systemcycles

#endif // useCPUreading
#ifdef useSoftwareClock
		volatileVariables[(uint16_t)(vClockCycleIdx)]++; // clockcycles

#endif // useSoftwareClock
	}

	lastTime = thisTime; // save cycle count

	if (awakeFlags & aAwakeOnInjector) // if MPGuino is awake on detected fuel injector event
	{

		if (watchdogInjectorCount) // if the fuel injector watchdog timer is running on minimum good engine speed
		{

			watchdogInjectorCount--; // cycle fuel injector watchdog timer down

#ifdef useChryslerMAPCorrection
			if (dirty & dSampleADC) // if injector monitor commanded an analog engine sensor read
			{

				dirty &= ~(dSampleADC); // acknowledge the command
				analogCommand |= (acSampleChrysler);

			}

#endif // useChryslerMAPCorrection
		}
		else // fuel injector watchdog timer has timed out
		{

			awakeFlags &= ~(aAwakeOnInjector); // signal that MPGuino is not awake any more due to no detected injector event during injector watchdog period
			dirty &= ~(dGoodEngineRun); // reset all fuel injector measurement flags
			watchdogInjectorCount = volatileVariables[(uint16_t)(vEngineOffTimeoutIdx)]; // start the fuel injector watchdog for engine off mode

		}

	}
	else // MPGuino is no longer awake due to no detected fuel injector events
	{

		if (awakeFlags & aAwakeEngineRunning) // if MPGuino is still awake due to running engine
		{

			if (watchdogInjectorCount) watchdogInjectorCount--; // cycle fuel injector watchdog timer down for engine off flag mode
			else
			{

				activityFlags |= (afEngineOffFlag); // flag engine as being off
				awakeFlags &= ~(aAwakeEngineRunning); // MPGuino is no longer awake due to engine running

			}

		}

	}

	if (VSScount) // if there is a VSS debounce countdown in progress
	{

		VSScount--; // bump down the VSS count
		if (VSScount == 0) updateVSS(thisTime); // if count has reached zero, go update VSS

	}

	if (awakeFlags & aAwakeOnVSS) // if MPGuino is awake on detected VSS pulse event
	{

		if (watchdogVSSCount) // if the VSS watchdog timer is running on minimum good vehicle speed
		{

			watchdogVSSCount--;

		}
		else // VSS watchdog timer has timed out on minimum good vehicle speed
		{

			awakeFlags &= ~(aAwakeOnVSS); // signal that MPGuino is no longer awake due to no detected VSS pulse event during VSS watchdog period
			dirty &= ~(dGoodVehicleMotion); // reset all VSS measurement flags
			watchdogVSSCount = volatileVariables[(uint16_t)(vVehicleStopTimeoutIdx)]; // start the VSS watchdog for vehicle stopped mode
			swapFEwithFCRcount = swapFEwithFCRdelay; // reset swap timer counter

		}

	}
	else // MPGuino is no longer awake due to no detected VSS pulse events
	{

		if (awakeFlags & aAwakeVehicleMoving) // if MPGuino is awake due to detected vehicle movement
		{

			if (watchdogVSSCount) watchdogVSSCount--;// cycle VSS watchdog timer down for vehicle stopped flag mode
			else
			{

				activityFlags |= (afVehicleStoppedFlag); // flag vehicle as stopped
				awakeFlags &= ~(aAwakeVehicleMoving); // vehicle is no longer awake on detected vehicle movement

			}

		}

		if ((activityFlags & afSwapFEwithFCR) == 0) // if not yet showing fuel consumption rate instead of fuel economy
		{

			if (swapFEwithFCRcount) swapFEwithFCRcount--; // cycle down fuel display watchdog until it zeroes out
			else activityFlags |= (afSwapFEwithFCR); // output fuel consumption rate function instead of fuel economy

		}

#ifdef useDragRaceFunction
		if (accelerationFlags & accelTestInProgress) // if acceleration test has started
		{

			timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);
			accelerationFlags &= ~(accelTestClearFlags); // reset drag race capture flags
			accelerationFlags |= (accelTestCompleteFlags); // signal that drag function is cancelled

		}

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
		internalFlags |= internalCancelCDT; // coastdown test will cancel if vehicle is idling

#endif // useCoastDownCalculator
	}

#ifdef useBarFuelEconVsTime
	if (timer0Command & t0cResetFEvTime) FEvTperiodIdx = FEvsTimeIdx; // initialize fuel econ vs time trip index variable
	else
	{

		if (FEvTimeCount) FEvTimeCount--;
		else
		{

			timer0Command |= (t0cResetFEvTime);
			FEvTperiodIdx++;
			if (FEvTperiodIdx > FEvsTimeEndIdx) FEvTperiodIdx -= bgDataSize;

		}

	}

	if (timer0Command & t0cResetFEvTime)
	{

		timer0Command &= ~(t0cResetFEvTime);
		tripVar::reset(FEvTperiodIdx); // reset source trip variable
		FEvTimeCount = volatileVariables[(uint16_t)(vFEvsTimePeriodTimeoutIdx)];

	}

#endif // useBarFuelEconVsTime
#ifdef useCoastDownCalculator
	if (internalFlags & internalCancelCDT)
	{

		internalFlags &= ~(internalCancelCDT);
		timer0Status |= (t0sCoastdownTestFlag);
		if (coastdownFlags & cdtTestInProgress) // if coastdown test has started
		{

			coastdownFlags &= ~(cdtTestClearFlags); // signal that coastdown test is no longer active
			coastdownFlags |= cdtCancelled | cdtFinished | cdSignalStateChange; // signal that coastdown test is cancelled

		}

	}
	else
	{

		if (coastdownFlags & cdtTestInProgress) // if coastdown test has been requested or is active
		{

			if (coastdownFlags & cdtTriggered) // if coastdown test has been requested
			{

				timer0Status |= (t0sCoastdownTestFlag);
				coastdownFlags &= ~(cdtTriggered); // clear coastdown test state
				coastdownFlags |= cdtActive | cdSignalStateChange; // mark coastdown test as active
				coastdownCount = volatileVariables[(uint16_t)(vCoastdownPeriodIdx)]; // reset coastdown counter

			}
			else
			{

				if (coastdownCount) coastdownCount--; // if coastdown clock hasn't elapsed
				else // perform state action
				{

					timer0Status |= (t0sCoastdownTestFlag);
					if (coastdownFlags & cdSampleTaken)
					{

						coastdownFlags &= ~(cdSampleTaken);
						coastdownFlags |= cdSignalStateChange; // signal coastdown test state change
						coastdownState++; // bump up to next state, for VSS read routine
						if (coastdownState > 2)
						{

							coastdownFlags &= ~(cdtActive); // make coastdown test no longer active
							coastdownFlags |= cdtFinished; // signal that coastdown test finished normally

						}
						else coastdownCount = volatileVariables[(uint16_t)(vCoastdownPeriodIdx)]; // reset coastdown counter

					}
					else coastdownFlags |= cdTakeSample;

				}

			}

		}

	}

#endif // useCoastDownCalculator
#if defined(useTWIbuttons)
	if (TWIsampleCount)
	{

		TWIsampleCount--;

		if ((twiStatusFlags & twiOpenMain) == twiBlockMainProgram) // if TWI section is finished processing
			switch (TWIsampleState)
			{

				case 0:
					TWI::openChannel(buttonAddress, TW_WRITE); // open TWI as master transmitter
#if defined(useAdafruitRGBLCDshield)
					TWI::writeByte(MCP23017_B1_GPIOA); // specify bank A GPIO pin register address
#endif // defined(useAdafruitRGBLCDshield)
					TWI::transmitChannel(TWI_REPEAT_START); // go write out read request, with repeated start to set up for read

					TWIsampleState++; // advance to waiting for TWI sample request to finish

					break;

				case 1:
					TWI::openChannel(buttonAddress, TW_READ); // open TWI as master receiver
					TWI::transmitChannel(TWI_STOP); // go commit to read, send stop when read is finished

					TWIsampleState++; // advance to waiting for TWI sample to complete

					break;

				case 2:
					if ((twiStatusFlags & twiErrorFlag) == 0)
					{

						thisButtonState = (twiDataBuffer[0] & buttonMask); // fetch button state that was just read in
						timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

					}

				default:
					twiStatusFlags &= ~(twiBlockMainProgram); // allow main program to utilize TWI
					break;

		}

	}
	else
	{

		TWIsampleCount = TWItickLength;

		if (twiStatusFlags & twiAllowISRactivity)
		{

			twiStatusFlags |= (twiBlockMainProgram); // block main program from making any TWI requests
			TWIsampleState = 0; // initialize TWI button read state machine

		}
		else twiStatusFlags &= ~(twiBlockMainProgram);

	}

#endif // defined(useTWIbuttons)
#if defined(useAnalogButtons)
	if (analogSampleCount) analogSampleCount--;
	else
	{

		analogSampleCount = analogSampleTickLength;
		if (timer0Command & t0cEnableAnalogButtons) analogCommand |= (acSampleButtonChannel); // go sample analog button channel

	}

#endif // defined(useAnalogButtons)
#ifdef useLegacyButtons
	if (buttonDebounceCount) // if there is a button press debounce countdown in progress
	{

		buttonDebounceCount--;

		if (buttonDebounceCount == 0)
		{

			thisButtonState = (lastPINxState & buttonMask) ^ buttonMask; // strip out all but relevant button bits
			timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

		}

	}

#endif // useLegacyButtons
#if defined(useButtonInput)
	if (buttonLongPressCount)
	{

		buttonLongPressCount--; // bump down the button long-press count by one

		if (buttonLongPressCount == 0)
		{

			buttonPress |= longButtonBit; // signal that a "long" button press has been detected
			internalFlags |= internalOutputButton;

		}

	}

	if (timer0Command & t0cProcessButton) // if button hardware reports reading in a button
	{

		timer0Command &= ~(t0cProcessButton); // ack
		if (thisButtonState != lastButtonState) // if there was a button state change since the last button was read in
		{

			if (thisButtonState == buttonsUp) // if it's all buttons being released
			{

				if (internalFlags & internalProcessButtonsUp) internalFlags |= internalOutputButton;

			}
			else
			{

				buttonPress = thisButtonState;
				internalFlags |= (internalProcessButtonsUp);
				buttonLongPressCount = buttonLongPressTick; // start the button long-press timer

			}

		}

		lastButtonState = thisButtonState;

	}

	if (internalFlags & internalOutputButton)
	{

		internalFlags &= ~(internalOutputButton);
		internalFlags &= ~(internalProcessButtonsUp);
		awakeFlags |= (aAwakeOnInput); // set awake status on button pressed
		timer0Command &= ~(t0cDisplayDelay); // shut off display change delay
		if (activityFlags & afActivityTimeoutFlag) timer0Status |= (t0sUpdateDisplay); // simply update the display if MPGuino was asleep
		else timer0Status |= (t0sReadButton | t0sShowCursor | t0sUpdateDisplay); // otherwise, force cursor show bit, and signal that keypress was detected
		buttonLongPressCount = 0; // reset button long-press timer
		cursorCount = cursorDelayTick; // reset cursor count
		activityFlags &= ~(afUserInputFlag | afActivityTimeoutFlag);
		inputTimeoutCount = volatileVariables[(uint16_t)(vButtonTimeoutIdx)];

	}

#endif // defined(useButtonInput)
#ifdef useJSONoutput
	if (JSONtimeoutCount) JSONtimeoutCount--;
	else
	{

		timer0Status |= t0sOutputJSON; // signal to JSON output routine to display next round of subtitles
		JSONtimeoutCount = JSONtickLength; // restart JSON output timeout count

	}

#endif // useJSONoutput
	if (loopCount) loopCount--;
	else
	{

#if useDataLoggingOutput || useJSONoutput
		timer0Status |= (t0sUpdateDisplay | t0sTakeSample | t0sOutputLogging); // signal to main program that a sampling should occur, and to update display
#else // useDataLoggingOutput || useJSONoutput
		timer0Status |= (t0sUpdateDisplay | t0sTakeSample); // signal to main program that a sampling should occur, and to update display
#endif // useDataLoggingOutput || useJSONoutput
		loopCount = loopTickLength; // restart loop count
		mainLoopHeartBeat <<= 1; // cycle the heartbeat bit
		if (mainLoopHeartBeat == 0) mainLoopHeartBeat = 1; // wrap around the heartbeat bit, if necessary
#if defined(useAnalogRead)
		analogCommand |= (acSampleChannelInit); // go sample all non-critical channels
#endif // useAnalogRead

	}

	if (cursorCount) cursorCount--;
	else
	{

		cursorCount = cursorDelayTick; // reset cursor count
		timer0Status ^= t0sShowCursor; // toggle cursor show bit

	}

	if (timer0Command & t0cDisplayDelay) // if display change delay is in effect
	{

		if (displayPauseCount) displayPauseCount--; // update pause counter
		else
		{

			timer0Command &= ~(t0cDisplayDelay); // otherwise, signal that display change delay is over
			timer0Status |= (t0sUpdateDisplay); // tell main program to update the display

		}

	}

	if (timer0Command & t0cDoDelay) // if main program has requested a delay
	{

		if (timer0DelayCount) timer0DelayCount--; // bump timer delay value down by one tick
		else timer0Command &= ~(t0cDoDelay); // signal to main program that delay timer has completed main program request

	}

	if (awakeFlags & aAwakeOnInput)
	{

		if (inputTimeoutCount) inputTimeoutCount--;
		else
		{

			awakeFlags &= ~(aAwakeOnInput);
			activityFlags |= (afUserInputFlag);

		}

	}

	if ((activityFlags & afParkCheckFlags) == afNotParkedFlags) // if MPGuino has engine stop and vehicle stop flags set, but is not yet parked
	{

		if (parkTimeoutCount) parkTimeoutCount--; // run down park watchdog timer until it expires
		else activityFlags |= (afParkFlag); // set vehicle parked flag

	}

	if ((activityFlags & afValidFlags) == afActivityCheckFlags) // if there is no activity but the activity watchdog hasn't timed out yet
	{

		if (activityTimeoutCount) activityTimeoutCount--; // cycle down the activity timeout watchdog
		else activityFlags |= (afActivityTimeoutFlag); // signal that MPGuino is in a period of inactivity

	}

	previousActivity ^= (activityFlags & afValidFlags); // detect any activity change since last timer0 tick

	if (previousActivity) activityChangeFlags |= (previousActivity); // if there was any activity change at all, signal that the display needs updating

	// reset activity timeout watchdog if any of the fuel injector, VSS pulse, button press, or park flags have changed
	if (previousActivity & afActivityCheckFlags) activityTimeoutCount = volatileVariables[(uint16_t)(vActivityTimeoutIdx)];

	// reset park timeout watchdog if any of the fuel injector or VSS pulse flags have changed
	if (previousActivity & afNotParkedFlags) parkTimeoutCount = volatileVariables[(uint16_t)(vParkTimeoutIdx)];

	previousActivity = (activityFlags & afValidFlags); // save for next timer0 tick

#if defined(useAnalogRead)
	if (analogCommand & acSampleChannelActive)
	{

		if (analogStatus & asHardwareReady)
		{

			analogCommand |= (acSampleGround); // signal to ADC interrupt that the last requested conversion was for internal ground
			analogStatus &= ~(asHardwareReady);

			ADMUX = pgm_read_byte(&analogChannelValue[(unsigned int)(analogGroundIdx)]);
			ADCSRA |= ((1 << ADSC) | (1 << ADIF) | (1 << ADIE)); // start ADC read, enable interrupt, and clear interrupt flag, because this crappy hardware allows the ADC interrupt to alway do free running mode

		}

	}

#endif // useAnalogRead
#ifdef useDebugCPUreading
	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += TCNT0;

#endif // useDebugCPUreading
}

#ifdef useTimer1Interrupt
// this ISR gets called every time timer 1 overflows.
//
// f(phase correct PWM) = f(system clock) / (N * 510)
//
// N - prescaler, which is 1
//
// so this ISR gets called every 510 * 1 / (system clock) seconds (for 20 MHz clock, that is every 25.5 us)
ISR( TIMER1_OVF_vect ) // LCD delay interrupt handler
{

#if defined(use4BitLCD)
	static uint8_t value;
#endif // defined(use4BitLCD)
#if defined(useSimulatedFIandVSS)
	static unsigned long debugVSSresetCount;
	static unsigned long debugFIPresetCount;
#endif // defined(useSimulatedFIandVSS)
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	if (timer1Command & t1cResetTimer)
	{

		timer1Command &= ~(t1cResetTimer);
		timer1Status = 0;
#ifdef useSimulatedFIandVSS
		debugVSScount = 0;
		debugFIPcount = 0;
		debugFIPWcount = 0;
		debugVSSresetCount = 0;
		debugFIPresetCount = 0;
#endif // useSimulatedFIandVSS

	}

#ifdef useSimulatedFIandVSS
	if (timer1Command & t1cEnableDebug)
	{

		if ((debugFlags & debugVSreadyFlags) == debugVSreadyFlags) // if VSS simulator is ready to output
		{

			if (debugVSScount) debugVSScount--;
			else
			{

				debugVSScount = debugVSStickLength;
#if defined(__AVR_ATmega32U4__)
				PORTB ^= (1 << PINB7); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
				PORTK ^= (1 << PINK0); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
				PORTC ^= (1 << PINC0); // generate VSS pin interrupt
#endif // defined(__AVR_ATmega328P__)

			}

		}

		if (debugFlags & debugVSSflag) // if VSS simulator is enabled
		{

			if (debugVSSresetCount) debugVSSresetCount--;
			else
			{

				debugVSSresetCount = debugVSSresetLength;
				timer1Status |= (t1sDebugUpdateVSS);

			}

		}

		if ((debugFlags & debugFIreadyFlags) == debugFIreadyFlags) // if fuel injector simulator is ready to output
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

		if (debugFlags & debugInjectorFlag) // if injector simulator is enabled
		{

			if (debugFIPresetCount) debugFIPresetCount--;
			else
			{

				debugFIPresetCount = debugFIPresetLength;
				timer1Status |= (t1sDebugUpdateFIP);

			}

		}

	}

#endif // useSimulatedFIandVSS
#if defined(useLCDoutput)
	if (timer1Command & t1cDelayLCD)
	{

		if (lcdDelayCount) lcdDelayCount--;
#if defined(useBufferedLCD)
		else
		{

			if (ringBuffer::isBufferNotEmpty(lcdBuffer)) // if there's at least one nybble in the LCD send buffer
			{

#if defined(useTWI4BitLCD)
				// if buffer is not empty and TWI hardware is ready
				if ((twiStatusFlags & twiOpenMain) == 0)
				{

					timer1Status &= ~(t1sDoOutputTWI); // reset TWI master transmission in progress flag
					timer1Status |= (t1sLoopFlag); // set loop flag

					do
					{

						value = ringBuffer::pull(lcdBuffer); // pull a buffered LCD nybble

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((timer1Status & t1sDoOutputTWI) == 0) // if this is the first nybble to be output
							{

								TWI::openChannel(lcdAddress, TW_WRITE); // open TWI as master transmitter
#if defined(useAdafruitRGBLCDshield)
								TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address
#endif // defined(useAdafruitRGBLCDshield)
								timer1Status |= (t1sDoOutputTWI); // signal to complete TWI master transmission

							}

						}

						LCD::outputNybble(value); // output the nybble and set timing

						if (value & lcdSendNybble) // if this nybble is to be sent out
						{

							if ((value & lcdSendFlags) == lcdSendNybble) // if sending an ordinary data nybble, check if we can continue looping
							{

								if ((twiDataBufferSize - twiDataBufferLen) < 5) timer1Status &= ~(t1sLoopFlag); // if TWI send buffer is getting low, signal end of loop
								if (ringBuffer::isBufferNotEmpty(lcdBuffer) == 0) timer1Status &= ~(t1sLoopFlag); // if LCD send buffer is empty, signal end of loop

							}
							else timer1Status &= ~(t1sLoopFlag); // otherwise, this is a special (command or reset) nybble, so signal end of loop

						}
						else timer1Status &= ~(t1sLoopFlag); // otherwise, this is just a delay request, so signal end of loop

					}
					while (timer1Status & t1sLoopFlag);

					if (timer1Status & t1sDoOutputTWI) TWI::transmitChannel(TWI_STOP); // commit LCD port expander write, if required

				}

#endif // defined(useTWI4BitLCD)
#if defined(usePort4BitLCD)
				value = ringBuffer::pull(lcdBuffer); // pull a buffered LCD byte

				LCD::outputNybble(value); // output byte

#endif // defined(usePort4BitLCD)
			}
			else timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

		}

#else // defined(useBufferedLCD)
		else timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

#endif // defined(useBufferedLCD)
	}

#endif // defined(useLCDoutput)
#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useTimer1Interrupt
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
#ifdef useDebugCPUreading
	unsigned int b;
#endif // useDebugCPUreading

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjectorOpenStart = timer0_overflow_count + (unsigned long)(a);

	if (dirty & dGoodEngineRotationOpen) thisEnginePeriodOpen = findCycleLength(lastInjectorOpenStart, thisInjectorOpenStart); // calculate length between fuel injector pulse starts
	else thisEnginePeriodOpen = 0;

#ifdef useChryslerMAPCorrection
	dirty |= (dGoodEngineRotationOpen | dInjectorReadInProgress | dSampleADC);
#else // useChryslerMAPCorrection
	dirty |= (dGoodEngineRotationOpen | dInjectorReadInProgress);
#endif // useChryslerMAPCorrection

	lastInjectorOpenStart = thisInjectorOpenStart;

	watchdogInjectorCount = volatileVariables[(uint16_t)(vDetectEngineOffIdx)]; // reset minimum engine speed watchdog timer

#ifdef useDebugCPUreading
	b = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) b = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += b - a;

#endif // useDebugCPUreading
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
#ifdef useDebugCPUreading
	unsigned int c;
#endif // useDebugCPUreading
	unsigned long thisInjectorCloseStart;
	unsigned long engineRotationPeriod;
	unsigned long thisInjectorPulseLength;
	unsigned long goodInjectorPulseLength;

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	thisInjectorCloseStart = timer0_overflow_count + (unsigned long)(a);

	if (dirty & dGoodEngineRotationClose) thisEnginePeriodClose = findCycleLength(lastInjectorCloseStart, thisInjectorCloseStart); // calculate length between fuel injector pulse starts
	else thisEnginePeriodClose = 0;

	if (dirty & dInjectorReadInProgress) // if there was a fuel injector open pulse detected, there's now a fuel injector pulse width to be measured
	{

		dirty &= ~(dInjectorReadInProgress);

		b = (dirty & dGoodEngineRotation);

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
		thisInjectorPulseLength = findCycleLength(thisInjectorOpenStart, thisInjectorCloseStart) - volatileVariables[(uint16_t)(vInjectorOpenDelayIdx)]; // strip off injector open delay time

		// if this pulse is larger than the maximum good pulse that could happen at the minimum valid engine speed, reject it
		// 1 - pulse could be narrower than vInjectorOpenDelayIdx
		// 2 - pulse could be wider than the maximum allowable pulse width for minimum good engine speed
		if (thisInjectorPulseLength > volatileVariables[(uint16_t)(vInjectorValidMaxWidthIdx)]) dirty &= ~(dGoodInjectorWidth | dGoodInjectorRead);
		else dirty |= (dGoodInjectorWidth);

		if (b) // if we have an engine rotation period measurement
		{

			// calculate good maximum fuel injector open time for injector pulse width sanity check
			goodInjectorPulseLength = engineRotationPeriod - volatileVariables[(uint16_t)(vInjectorTotalDelayIdx)];

			if (thisInjectorPulseLength > goodInjectorPulseLength) dirty &= ~(dGoodInjectorRead); // if measured pulse is larger than largest good pulse, signal that last injector read may be bad
			else dirty |= (dGoodInjectorRead); // signal that last injector read is good

			// if measured engine speed is greater than the specified minimum good engine speed
			if (engineRotationPeriod < volatileVariables[(uint16_t)(vMaximumEnginePeriodIdx)])
			{

				activityFlags &= ~(afEngineOffFlag | afParkFlag | afActivityTimeoutFlag); // signal that engine is running, and vehicle is therefore no longer parked
				awakeFlags |= (aAwakeEngineRunning); // MPGuino is awake due to engine running

			}

#if defined(trackIdleEOCdata)
			if (awakeFlags & aAwakeVehicleMoving) tripVar::add64(collectedEngCycleCount, engineRotationPeriod, curRawTripIdx); // add to fuel injector total cycle accumulator
			else tripVar::add64(collectedEngCycleCount, engineRotationPeriod, curRawEOCidleTripIdx); // add to idle fuel injector total cycle accumulator

#else // defined(trackIdleEOCdata)
			tripVar::add64(collectedEngCycleCount, engineRotationPeriod, curRawTripIdx); // add to fuel injector total cycle accumulator

#endif // defined(trackIdleEOCdata)
#ifdef useDragRaceFunction
			// add to distance acceleration fuel injector total cycle accumulator
			if (accelerationFlags & accelTestActive) tripVar::add64(collectedEngCycleCount, engineRotationPeriod, dragRawDistanceIdx);

#endif // useDragRaceFunction
		}

		// if the injector pulse width is valid
		if (dirty & dGoodInjectorWidth)
		{

			awakeFlags |= (aAwakeOnInjector); // signal that MPGuino is awake due to detected injector

#ifdef useChryslerMAPCorrection
			thisInjectorPulseLength *= volatileVariables[(uint16_t)(vInjectorCorrectionIdx)]; // multiply by differential fuel pressure correction factor numerator
			thisInjectorPulseLength >>= 12; // divide by differential fuel pressure correction factor denominator

#endif // useChryslerMAPCorrection
#if defined(trackIdleEOCdata)
			if (awakeFlags & aAwakeVehicleMoving) // if vehicle is moving, save injector measurements in active raw trip variable
			{

				collectedInjPulseCount[(uint16_t)(curRawTripIdx)]++; // add to fuel injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, curRawTripIdx); // add to fuel injector open cycle accumulator

			}
			else // if vehicle is not moving, save injector measurements in active idle/EOC raw trip variable
			{

				collectedInjPulseCount[(uint16_t)(curRawEOCidleTripIdx)]++; // add to idle fuel injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, curRawEOCidleTripIdx); // add to idle fuel injector open cycle accumulator

			}

#else // defined(trackIdleEOCdata)
			collectedInjPulseCount[(uint16_t)(curRawTripIdx)]++; // add to fuel injector pulse count
			tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, curRawTripIdx); // add to fuel injector open cycle accumulator

#endif // defined(trackIdleEOCdata)
#ifdef useDragRaceFunction
			if (accelerationFlags & accelTestActive)
			{

				collectedInjPulseCount[(uint16_t)(dragRawDistanceIdx)]++; // update the distance acceleration injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, dragRawDistanceIdx); // add to distance acceleration fuel injector open cycle accumulator

			}

#endif // useDragRaceFunction
		}

	}

	dirty |= (dGoodEngineRotationClose);
	lastInjectorCloseStart = thisInjectorCloseStart;

	watchdogInjectorCount = volatileVariables[(uint16_t)(vDetectEngineOffIdx)]; // reset minimum engine speed watchdog timer

#ifdef useDebugCPUreading
	c = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) c = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c - a;

#endif // useDebugCPUreading
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
#ifdef useDebugCPUreading
	unsigned int c;
#endif // useDebugCPUreading
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
		else updateVSS(thisTime); // otherwise, go process VSS pulse

	}

#ifdef useLegacyButtons
	if (q & buttonMask) buttonDebounceCount = buttonDebounceTick; // if a button change was detected, set button press debounce count, and let system timer handle the debouncing

#endif // useLegacyButtons
	lastPINxState = p; // remember the current input pin state for the next time this ISR gets called

#ifdef useDebugCPUreading
	c = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) c = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c - a;

#endif // useDebugCPUreading
}

#ifdef useBuffering
static void ringBuffer::init(ringBufferVariable &bfr, volatile uint8_t * storage)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	bfr.data = storage;
	bfr.size = sizeof(storage) / sizeof(storage[0]);
	bfr.start = 0;
	bfr.end = 0;
	bfr.status = bufferIsEmpty;

	SREG = oldSREG; // restore interrupt flag status

}

static uint8_t ringBuffer::isBufferNotEmpty(ringBufferVariable &bfr)
{

	return ((bfr.status & bufferIsEmpty) == 0);

}

static void ringBuffer::push(ringBufferVariable &bfr, uint8_t value)
{

	uint8_t oldSREG;

	while (bfr.status & bufferIsFull) idleProcess(); // wait for calling routine's buffer to become not full

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	pushInterrupt(bfr, value);

	SREG = oldSREG; // restore interrupt flag status

}

static void ringBuffer::pushInterrupt(ringBufferVariable &bfr, uint8_t value)
{

	bfr.data[(unsigned int)(bfr.start++)] = value; // save a buffered character

	if (bfr.status & bufferIsEmpty) bfr.status &= ~(bufferIsEmpty); // mark buffer as no longer empty
	if (bfr.start == bfr.size) bfr.start = 0; // handle wrap-around
	if (bfr.start == bfr.end) bfr.status |= (bufferIsFull); // test if buffer is full

}

static uint8_t ringBuffer::pull(ringBufferVariable &bfr)
{

	uint8_t value;

	if (bfr.status & bufferIsEmpty) value = 0; // if buffer is empty, return a NULL
	else
	{

		value = bfr.data[(unsigned int)(bfr.end++)]; // pull a buffered character

		if (bfr.status & bufferIsFull) bfr.status &= ~(bufferIsFull); // mark buffer as no longer full
		if (bfr.end == bfr.size) bfr.end = 0; // handle wrap-around
		if (bfr.end == bfr.start) bfr.status |= (bufferIsEmpty); // test if buffer is empty

	}

	return value;

}

static void ringBuffer::flush(ringBufferVariable &bfr)
{

	while ((bfr.status & bufferIsEmpty) == 0) idleProcess(); // wait for calling routine's buffer to become empty

}

#endif // useBuffering
static void updateVSS(unsigned long thisVSStime)
{

	static unsigned long lastVSStime;

	static unsigned long cycleLength;

	if (dirty & dGoodVSSsignal) // if a valid VSS signal had previously been read in
	{

		dirty |= (dGoodVSSRead); // mark valid VSS pulse measurement
		awakeFlags |= (aAwakeOnVSS); // MPGuino is awake on valid VSS pulse measurement

		cycleLength = findCycleLength(lastVSStime, thisVSStime); // calculate VSS pulse length

		if (cycleLength < volatileVariables[(uint16_t)(vMaximumVSSperiodIdx)]) // if VSS period is less than that for minimum good vehicle speed
		{

			if (activityFlags & afVehicleStoppedFlag) // if vehicle has been previously flagged as not moving
			{

				activityFlags &= ~(afVehicleStoppedFlag | afSwapFEwithFCR | afParkFlag | afActivityTimeoutFlag); // signal that vehicle is moving, and vehicle is therefore no longer parked
				awakeFlags |= (aAwakeVehicleMoving); // MPGuino is awake on vehicle movement

			}

		}

#if defined(trackIdleEOCdata)
		if (awakeFlags & aAwakeEngineRunning)
		{

			collectedVSSpulseCount[(unsigned int)(curRawTripIdx)]++;
			tripVar::add64(collectedVSScycleCount, cycleLength, curRawTripIdx); // add to VSS cycle accumulator

		}
		else // if the engine is not running, vehicle is in EOC mode
		{

			collectedVSSpulseCount[(unsigned int)(curRawEOCidleTripIdx)]++;
			tripVar::add64(collectedVSScycleCount, cycleLength, curRawEOCidleTripIdx); // add to EOC VSS cycle accumulator

		}

#else // defined(trackIdleEOCdata)
		collectedVSSpulseCount[(unsigned int)(curRawTripIdx)]++;
		tripVar::add64(collectedVSScycleCount, cycleLength, curRawTripIdx); // add to VSS cycle accumulator

#endif // defined(trackIdleEOCdata)
#ifdef useCoastDownCalculator
		if (coastdownFlags & (cdtActive | cdTakeSample)) // if coastdown test is active
		{

			coastdownFlags &= ~(cdTakeSample);
			coastdownFlags |= cdSampleTaken;
			volatileVariables[(uint16_t)(vCoastdownMeasurement1Idx + coastdownState)] = cycleLength;

		}

#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
		if (accelerationFlags & accelTestTriggered) // if accel test function is triggered
		{

				accelerationFlags &= ~accelTestTriggered; // switch status from 'triggered' to 'active'
				accelerationFlags |= accelTestActive;
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

		}

		if (accelerationFlags & accelTestActive) // if accel test function is active
		{

			if (volatileVariables[(uint16_t)(vDragInstantSpeedIdx)] > cycleLength) volatileVariables[(uint16_t)(vDragInstantSpeedIdx)] = cycleLength; // if this vehicle speed is larger than previous vehicle speed

			collectedVSSpulseCount[(unsigned int)(dragRawDistanceIdx)]++; // update the accel test distance measurement VSS pulse count
			tripVar::add64(collectedVSScycleCount, cycleLength, dragRawDistanceIdx); // update the accel test distance measurement VSS cycle accumulator

			if (accelerationFlags & accelTestDistance)
			{

				if (volatileVariables[(uint16_t)(vAccelDistanceValueIdx)]) volatileVariables[(uint16_t)(vAccelDistanceValueIdx)]--; // count down drag distance setpoint in VSS pulses
				else
				{

					accelerationFlags &= ~accelTestDistance; // otherwise, mark drag function distance measurement as complete
					timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

				}

			}

			if ((accelerationFlags & accelTestHalfSpeed) && (cycleLength < volatileVariables[(uint16_t)(vAccelHalfPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripVar::transfer(dragRawDistanceIdx, dragRawHalfSpeedIdx);
				accelerationFlags &= ~accelTestHalfSpeed; // mark drag function speed measurement as complete
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

			if ((accelerationFlags & accelTestFullSpeed) && (cycleLength < volatileVariables[(uint16_t)(vAccelFullPeriodValueIdx)])) // if instantaneous speed is greater than drag function speed
			{

				// copy drag raw trip variable to drag full speed trip variable
				tripVar::transfer(dragRawDistanceIdx, dragRawFullSpeedIdx);
				accelerationFlags &= ~accelTestFullSpeed; // mark drag function speed measurement as complete
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

			if ((accelerationFlags & accelTestMeasurementFlags) == 0) // if all drag measurements have completed, mark drag function as complete
			{

				accelerationFlags &= ~accelTestActive; // switch status from 'active' to 'finished'
				accelerationFlags |= accelTestFinished;
				timer0Status |= (t0sAccelTestFlag | t0sUpdateDisplay);

			}

		}

#endif // useDragRaceFunction
	}

	dirty |= dGoodVSSsignal; // annotate that a valid VSS pulse has been read
	watchdogVSSCount = volatileVariables[(uint16_t)(vDetectVehicleStopIdx)]; // reset minimum engine speed watchdog timer
	lastVSStime = thisVSStime;

}

static void initCore(void)
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

	timer0Command = t0cResetTimer;
#if defined(useTimer1Interrupt)
	timer1Command = t1cResetTimer;
#endif // defined(useTimer1Interrupt)

	SREG = oldSREG; // restore interrupt flag status

}

static void initHardware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer initialization section - multiple peripherals may use the same timer
#ifdef useTimer1
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

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

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

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

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

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega328P__)
#endif // useTimer1
#ifdef useTimer2
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
#endif // useTimer2
#ifdef useTimer4
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
#endif // useTimer4
	SREG = oldSREG; // restore interrupt flag status

#ifdef useTWIsupport
	TWI::init();
#if defined(useMCP23017portExpander)
	MCP23017portExpanderSupport::init(); // go init MCP23017 port expander
#endif // defined(useMCP23017portExpander)
#endif // useTWIsupport
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
#if defined(__AVR_ATmega32U4__)
//	usbSupport::init();
#endif // defined(__AVR_ATmega32U4__)
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

#ifdef useDeepSleep // Deep Sleep support section
static void doGoDeepSleep(void)
{

#if defined(useOutputPins)
	outputPin::shutdown();
#endif // defined(useOutputPins)
#if defined(useActivityLED)
	activityLED::shutdown();
#endif // defined(useActivityLED)
	changeBitFlags(timer0Command, t0cDisplayDelay, 0); // cancel any display delays in progress
#if defined(useTFToutput)
	TFT::shutdown(); // shut down the TFT display
#endif // defined(useTFToutput)
#if defined(useLCDoutput)
	LCD::shutdown(); // shut down the LCD display
#endif // defined(useLCDoutput)
#if defined(useButtonInput)
	button::shutdown();
#endif // defined(useButtonInput)
#if defined(__AVR_ATmega32U4__)
//	usbSupport::shutdown();
#endif // defined(__AVR_ATmega32U4__)
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
#ifdef useTWIsupport
	TWI::shutdown();
#endif // useTWIsupport

#ifdef useTimer4
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM4); // shut off timer4 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)

#endif // useTimer4
#ifdef useTimer2
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer2
#ifdef useTimer1Interrupt
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

#endif // useTimer1Interrupt
#ifdef useTimer1
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer1
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

	initHardware(); // restart all peripherals

}

#endif // useDeepSleep
static uint32_t findCycleLength(unsigned long lastCycle, unsigned long thisCycle) // this is only to be meant to be used with interrupt handlers
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

static void delay0(uint16_t ms)
{

	uint8_t oldSREG;

	if (ms)
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		timer0DelayCount = ms; // request a set number of timer tick delays per millisecond
		timer0Command |= (t0cDoDelay); // signal request to timer

		SREG = oldSREG; // restore interrupt flag status

	}

	while (timer0Command & t0cDoDelay) idleProcess(); // wait for delay timeout

}

static void delayS(uint16_t ms)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = ms; // request a set number of timer tick delays per millisecond

	if (ms) timer0Command |= (t0cDisplayDelay); // if display delay requested, make it active
	else timer0Command &= ~(t0cDisplayDelay); // otherwise, cancel display delay

	SREG = oldSREG; // restore interrupt flag status

}

// this function is needed since there is no way to perform an atomic bit change of an SRAM byte value
// most MPGuino variables that are shared between main program and interrupt handlers should not need to
//    be treated as atomic (!) because only one side or the other is supposed to change said variables
// however, status flag registers are obviously an exception, and status flag changes are common
//    enough to warrant an explicit function definition
static void changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	flagRegister = (flagRegister & ~(maskAND)) | (maskOR); // go perform atomic status flag change

	SREG = oldSREG; // restore interrupt flag status

}

static void performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

}

