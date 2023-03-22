// this ISR gets called every time timer 0 overflows.
//
// f(fast PWM) = f(system clock) / (N * 256)
//
// N - prescaler, which is 64
//
// so this ISR gets called every 256 * 64 / (system clock) seconds (for 20 MHz clock, that is every 0.8192 ms)
//
ISR( TIMER0_OVF_vect ) // system timer interrupt handler
{

	static unsigned long lastTime;
	static unsigned long buttonTimeoutCount;
	static unsigned long parkTimeoutCount;
	static unsigned long activityTimeoutCount;
	static unsigned long swapFEwithFCRcount;
#ifdef useCoastDownCalculator
	static unsigned long coastdownCount;
#endif // useCoastDownCalculator
#ifdef useBarFuelEconVsTime
	static unsigned long FEvTimeCount;
#endif // useBarFuelEconVsTime
	static unsigned int buttonLongPressCount;
	static unsigned int cursorCount;
	static unsigned int loopCount;
#ifdef useAnalogButtons
	static unsigned int analogSampleCount;
#endif // useAnalogButtons
#ifdef useJSONoutput
	static unsigned int JSONtimeoutCount;
#endif // useJSONoutput
	static uint8_t previousActivity;
	static uint8_t internalFlags;
#ifdef useTWIbuttons
	static uint8_t TWIsampleCount;
	static uint8_t TWIsampleState;
#endif // useTWIbuttons
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
#ifdef useTWIbuttons
		TWIsampleCount = TWItickLength;
		TWIsampleState = 0;
#endif // useTWIbuttons
#if defined(useAnalogRead)
		analogStatus = asHardwareReady;
#ifdef useAnalogButtons
		analogSampleCount = analogSampleTickLength;
#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useLegacyButtons
		buttonDebounceCount = 0;
#endif // useLegacyButtons
#ifdef useBarFuelEconVsTime
		timer0Command |= (t0cResetFEvTime);
#endif // useBarFuelEconVsTime
		buttonLongPressCount = 0;
		buttonTimeoutCount = 0;
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
	if (timer0Command & t0cResetFEvTime) FEvTperiodIdx = FEvsTimeIdx | 0x80; // initialize fuel econ vs time trip index variable
	else
	{

		if (FEvTimeCount) FEvTimeCount--;
		else
		{

			timer0Command |= (t0cResetFEvTime);
			FEvTperiodIdx &= 0x7F; // strip off transfer bit, if present
			FEvTperiodIdx++;
			if (FEvTperiodIdx > FEvsTimeEndIdx) FEvTperiodIdx -= bgDataSize;
			FEvTperiodIdx |= 0x80; // next trip variable interaction will be a transfer

		}

	}

	if (timer0Command & t0cResetFEvTime)
	{

		timer0Command &= ~(t0cResetFEvTime);
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
#ifdef useTWIbuttons
	if (TWIsampleCount)
	{

		TWIsampleCount--;

		if ((twiStatusFlags & twiOpenMain) == twiBlockMainProgram) // if TWI section is finished processing, and if timer0 TWI sampling is enabled
			switch (TWIsampleState)
			{

				case 0:
#ifdef useAdafruitRGBLCDshield
					TWI::openChannel(buttonAddress, TW_WRITE); // open TWI as master transmitter
					TWI::writeByte(MCP23017_B1_GPIOA); // specify bank A GPIO pin register address
					TWI::transmitChannel(TWI_REPEAT_START); // go write out read request, with repeated start to set up for read
#endif // useAdafruitRGBLCDshield

					TWIsampleState++; // advance to waiting for TWI sample request to finish

					break;

				case 1:
#ifdef useAdafruitRGBLCDshield
					TWI::openChannel(buttonAddress, TW_READ); // open TWI as master receiver
					TWI::transmitChannel(TWI_STOP); // go commit to read, send stop when read is finished
#endif // useAdafruitRGBLCDshield

					TWIsampleState++; // advance to waiting for TWI sample to complete

					break;

				case 2:
					if ((twiStatusFlags & twiErrorFlag) == 0)
					{

#ifdef useAdafruitRGBLCDshield
						thisButtonState = (twiDataBuffer[0] & buttonMask); // fetch button state that was just read in
						timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in
#endif // useAdafruitRGBLCDshield

					}

				default:
					twiStatusFlags &= ~(twiBlockMainProgram);
					break;

		}

	}
	else
	{

		TWIsampleCount = TWItickLength;

		if (timer0Command & t0cEnableTWIsample)
		{

			twiStatusFlags |= (twiBlockMainProgram); // block main program from making any TWI requests
			TWIsampleState = 0; // initialize TWI button read state machine

		}
		else twiStatusFlags &= ~(twiBlockMainProgram);

	}

#endif // useTWIbuttons
#ifdef useAnalogButtons
	if (analogSampleCount) analogSampleCount--;
	else
	{

		analogSampleCount = analogSampleTickLength;
		analogCommand |= (acSampleButtonChannel); // go sample analog button channel

	}

#endif // useAnalogButtons
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
		awakeFlags |= (aAwakeOnButton); // set awake status on button pressed
		timer0Command &= ~(t0cDisplayDelay); // shut off display change delay
		if (activityFlags & afActivityTimeoutFlag) timer0Status |= (t0sUpdateDisplay); // simply update the display if MPGuino was asleep
		else timer0Status |= (t0sReadButton | t0sShowCursor | t0sUpdateDisplay); // otherwise, force cursor show bit, and signal that keypress was detected
		buttonLongPressCount = 0; // reset button long-press timer
		cursorCount = cursorDelayTick; // reset cursor count
		activityFlags &= ~(afButtonFlag | afActivityTimeoutFlag);
		buttonTimeoutCount = volatileVariables[(uint16_t)(vButtonTimeoutIdx)];

	}

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

	if (awakeFlags & aAwakeOnButton)
	{

		if (buttonTimeoutCount) buttonTimeoutCount--;
		else
		{

			awakeFlags &= ~(aAwakeOnButton);
			activityFlags |= (afButtonFlag);

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

#ifdef use4BitLCD
	static uint8_t value;
#endif // use4BitLCD
#ifdef useSimulatedFIandVSS
	static unsigned long debugVSSresetCount;
	static unsigned long debugFIPresetCount;
#endif // useSimulatedFIandVSS
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
#ifdef use4BitLCD
	if (timer1Command & t1cDelayLCD)
	{

		if (lcdDelayCount) lcdDelayCount--;
#ifdef useBufferedLCD
		else
		{

			if (ringBuffer::isBufferNotEmpty(lcdBuffer)) // if there's at least one nybble in the LCD send buffer
			{

#ifdef useTWILCD
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
								TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address
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
#else // useTWILCD
				value = ringBuffer::pull(lcdBuffer); // pull a buffered LCD byte

				LCD::outputNybble(value); // output byte
#endif // useTWILCD

			}
			else timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

		}
#else // useBufferedLCD
		else timer1Command &= ~(t1cDelayLCD); // turn off LCD delay
#endif // useBufferedLCD

	}

#endif // use4BitLCD
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

#ifdef trackIdleEOCdata
			if (awakeFlags & aAwakeVehicleMoving) tripVar::add64(collectedEngCycleCount, engineRotationPeriod, rawTripIdx); // add to fuel injector total cycle accumulator
			else tripVar::add64(collectedEngCycleCount, engineRotationPeriod, rawEOCidleTripIdx); // add to idle fuel injector total cycle accumulator

#else // trackIdleEOCdata
			tripVar::add64(collectedEngCycleCount, engineRotationPeriod, rawTripIdx); // add to fuel injector total cycle accumulator

#endif // trackIdleEOCdata
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
#ifdef trackIdleEOCdata
			if (awakeFlags & aAwakeVehicleMoving) // if vehicle is moving, save injector measurements in active raw trip variable
			{

				collectedInjPulseCount[(uint16_t)(rawTripIdx)]++; // add to fuel injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, rawTripIdx); // add to fuel injector open cycle accumulator

			}
			else // if vehicle is not moving, save injector measurements in active idle/EOC raw trip variable
			{

				collectedInjPulseCount[(uint16_t)(rawEOCidleTripIdx)]++; // add to idle fuel injector pulse count
				tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, rawEOCidleTripIdx); // add to idle fuel injector open cycle accumulator

			}

#else // trackIdleEOCdata
			collectedInjPulseCount[(uint16_t)(rawTripIdx)]++; // add to fuel injector pulse count
			tripVar::add64(collectedInjCycleCount, thisInjectorPulseLength, rawTripIdx); // add to fuel injector open cycle accumulator

#endif // trackIdleEOCdata
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

#ifdef trackIdleEOCdata
		if (awakeFlags & aAwakeEngineRunning)
		{

			collectedVSSpulseCount[(unsigned int)(rawTripIdx)]++;
			tripVar::add64(collectedVSScycleCount, cycleLength, rawTripIdx); // add to VSS cycle accumulator

		}
		else // if the engine is not running, vehicle is in EOC mode
		{

			collectedVSSpulseCount[(unsigned int)(rawEOCidleTripIdx)]++;
			tripVar::add64(collectedVSScycleCount, cycleLength, rawEOCidleTripIdx); // add to EOC VSS cycle accumulator

		}

#else // trackIdleEOCdata
		collectedVSSpulseCount[(unsigned int)(rawTripIdx)]++;
		tripVar::add64(collectedVSScycleCount, cycleLength, rawTripIdx); // add to VSS cycle accumulator

#endif // trackIdleEOCdata
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

unsigned long findCycleLength(unsigned long lastCycle, unsigned long thisCycle) // this is only to be meant to be used with interrupt handlers
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

void delay0(unsigned int ms)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	timer0DelayCount = ms; // request a set number of timer tick delays per millisecond
	timer0Command |= t0cDoDelay; // signal request to timer

	SREG = oldSREG; // restore interrupt flag status

	while (timer0Command & t0cDoDelay) idleProcess(); // wait for delay timeout

}
