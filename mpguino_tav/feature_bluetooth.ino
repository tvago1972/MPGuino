#if defined(useBluetooth)
/* Bluetooth interface terminal section */

static const uint8_t prgmWriteBTparameterValue[] PROGMEM = {
	instrCmpIndex, pMetricModeIdx,						// is this pMetricModeIdx being stored
	instrBranchIfNotE, 3,								// if not, go store value
	instrSubByteFromX, 0x02, 1,							// adjust pMetricModeIdx so it is compatible with MPGuinoBlue
	instrStRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

static const uint8_t prgmCheckInstantSpeed[] PROGMEM = {
	instrLdRegTripVar, 0x02, instantIdx, rvVSSpulseIdx,	// load instant trip VSS pulse count
	instrBranchIfZero, 3,								// if speed measurement is zero, exit to caller
	instrLdRegByte, 0x02, 1,							// load a non-zero number
	instrDone											// return to caller
};

static void bluetooth::init(void)
{

#if defined(useBluetoothAdaFruitSPI)
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	devBluetooth.chrOut = chrOut;
	devBluetooth.chrIn = chrIn;

	PORTB &= ~(1 << PORTB0); // disable CS when this pin becomes an output
	PORTD &= ~(1 << PORTD4); // force /RST low when this pin becomes an output

	DDRB |= (1 << DDB0); // turn CS to an output pin
	DDRD &= ~(1 << DDD7); // turn IRQ to an input pin
	DDRD |= (1 << DDD4); // turn /RST to an output pin

	SREG = oldSREG; // restore interrupt flag status

	heart::doDelay0(delay0(delay0005msTick)); // hold /RST low for 5 ms

	PORTD |= (1 << PORTD4); // pull /RST high
	PORTB |= (1 << PORTB0); // enable CS

	heart::doDelay0(delay0(delay0002msTick)); // wait for 2 ms to allow Bluefruit module to become ready

#else // defined(useBluetoothAdaFruitSPI)
	devBluetooth.controlFlags &= ~(odvFlagCRLF);

#endif // defined(useBluetoothAdaFruitSPI)
	btInputState = 0;
	btOutputState = btoFlagContinuousOutput;
	btOutputListIdx = btolTripFunctionIdx;

}

static void bluetooth::shutdown(void)
{

#if defined(useBluetoothAdaFruitSPI)
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	PORTB &= ~(1 << PORTB0);
	PORTD &= ~((1 << PORTD7) | (1 << PORTD4));

	DDRB |= (1 << DDB0); // turn all PORTB pins to input
	DDRD &= ~((1 << DDD7) | (1 << DDD4)); // turn all PORTD pins to input

	SREG = oldSREG; // restore interrupt flag status

#endif // defined(useBluetoothAdaFruitSPI)
	btInputState = 0;
	btOutputState = 0;

}

#if defined(useBluetoothAdaFruitSPI)
static void bluetooth::chrOut(uint8_t chr)
{
}

static uint8_t bluetooth::chrIn(void)
{
}

#endif // defined(useBluetoothAdaFruitSPI)
static uint16_t bluetooth::findFormat(uint8_t inpChar)
{

	for (uint8_t x = 0; x < btFunctionListLength; x++)
	{

		if (pgm_read_byte(&btFunctionList[(uint16_t)(x)].btFuncChar) == inpChar) // if output format char was found
			return pgm_read_word(&btFunctionList[(uint16_t)(x)].btFuncFormat); // return the corresponding output format

	}

	return 0; // otherwise, return 0 for 'format not found'

}

static void bluetooth::mainProcess(void)
{

	uint8_t btChar;
	uint16_t btFormat;
	union union_16 * btF = (union union_16 *)(&btFormat);

	do
	{

		btChar = text::charIn(devBluetooth); // read in a character from the input buffer

		if (btChar) // if a valid character was read in
		{

#if defined(useDebugTerminal)
			if (peek & peekBluetoothInput) text::charOut(devDebugTerminal, btChar);

#endif // defined(useDebugTerminal)
			switch (btInputState)
			{

				case 'M':	// processing an expanded command
					switch (btChar)
					{

						case 'M':	// output selected EEPROM parameter list
							heart::changeBitFlags(timer0Command, 0, t0cResetBluetoothOutput); // reset bluetooth output

							while (timer0Command & t0cResetBluetoothOutput); // wait for timer0 to acknowledge reset

							btOutputState &= ~(btoOutputFlags); // clear all output flags
							btOutputState |= (btoFlagSingleShotOutput);
							btOutputListIdx = btolParameterIdx;
							break;

						case 'R':	// reset current trip
							tripSupport::doResetTrip(0);
#if defined(useSavedTrips)
							tripSave::doWriteTrip(0);
#endif // defined(useSavedTrips)
							break;

						case 'T':	// reset tank trip
							tripSupport::doResetTrip(1);
#if defined(useSavedTrips)
							tripSave::doWriteTrip(1);
#endif // defined(useSavedTrips)
							break;

						default:	// unsupported command
							break;

					}

					btInputState = 0;
					break;

				case '!':
					switch (btChar)
					{

						case '!':
							heart::changeBitFlags(timer0Command, 0, t0cResetBluetoothOutput); // reset bluetooth output

							while (timer0Command & t0cResetBluetoothOutput); // wait for timer0 to acknowledge reset

							btOutputState &= ~(btoOutputFlags); // clear all output flags
							btOutputState |= (btoFlagContinuousOutput);
							btOutputListIdx = btolTripFunctionIdx;
							btInputState = 1; // treat this also as a variable write, with special character substituting for '!'
							btInpBuffIdx = 0; // reset input buffer
							break;

						default:	// unsupported command
							btInputState = 0;
							break;

					}

					break;

				case 0:		// normal command processing
					switch (btChar)
					{

						case '!':	// initialize and output selected trip functions
						case 'M':	// process expanded command
							btInputState = btChar; // save input state
							break;

						default:	// unrecognized command - could be a variable write
							btFormat = findFormat(btChar); // go find the corresponding output format

							if (btFormat) // if format was found, this is a variable write
							{

								btInputState = btChar; // save variable character
								btInpBuffIdx = 0; // reset input buffer

							}

							break;

					}
					break;

				default:	// K-delimited string processing
					switch (btChar)
					{

						default:	// unrecognized character - reset number input
							btInputState = 0;
							break;

						case '0' ... '9':	// digits
							if (btInputState & 0x80) // if in check digit mode, and read in digit does not equal stored digit, abort
							{

								if (btChar != btInpBuff[(uint16_t)(btInpBuffIdx++)]) btInputState = 0;

							}
							else // if not, we are in digit storage mode
								btInpBuff[(uint16_t)(btInpBuffIdx++)] = btChar; // store digit

							break;

						case 'K':	// number string terminator
							btInputState ^= 0x80; // toggle digit mode

							if (btInputState & 0x80) btInpBuff[(uint16_t)(btInpBuffIdx)] = 0; // if in check digit mode, mark end of string
							else // if back in digit storage mode, time to convert number string
							{

								// if end of digit string is correct, then the two input digit strings are identical
								if (btInpBuff[(uint16_t)(btInpBuffIdx)] == 0)
								{

									if (btInputState == 1) btChar = '!'; // re-translate back to '!' for output
									else btChar = btInputState;

									btFormat = findFormat(btChar); // go find the corresponding output format

									if (btFormat)
									{

										switch (btF->u8[0])
										{

											case tGetBTparameterValue:
												str2ull(btInpBuff); // convert digit string into a number
#if defined(usePartialRefuel)
												if (btF->u8[1] == pRefuelSizeIdx) SWEET64::runPrgm(prgmAddToPartialRefuel, 0);
#endif // defined(usePartialRefuel)
												parameterEdit::onEEPROMchange(prgmWriteBTparameterValue, btF->u8[1]);
												heart::changeBitFlags(timer0Command, 0, t0cInputReceived);
												break;

											case tFetchMainProgramValue:
												str2ull(btInpBuff); // convert digit string into a number
												SWEET64::runPrgm(prgmWriteMainProgramValue, btF->u8[1]);
												heart::changeBitFlags(timer0Command, 0, t0cInputReceived);
												break;

											default:
												break;

										}

									}

								}

								btInputState = 0; // reset input state

							}

							btInpBuffIdx = 0;

							break;

					}
					break;

			}

		}

	}
	while (btChar);

#if defined(bluetoothSerialBuffer)
	if (btOutputState & btoFlagFlushBuffer) // flush the output ring buffer
	{

		if (bluetoothSerialBuffer.status & bufferIsEmpty) // if the output ring buffer is flushed
		{

			btOutputState &= ~(btoFlagFlushBuffer);
			btDelayFlag = heart::delay0(delay0020msTick); // set for a 20 ms delay
			btOutputState |= (btoFlagDelay); // allows smartphone app time to process variable just transmitted

		}

	}

#endif // defined(bluetoothSerialBuffer)
	if (btOutputState & btoFlagDelay) // check if output delay is finished
	{

		if ((timer0DelayFlags & btDelayFlag) == 0) btOutputState &= ~(btoFlagDelay); // if delay is finished, allow output to continue

	}

	if ((btOutputState & btoOutputActiveFlags) == btoFlagActiveOutput)
	{

		do
		{

			btChar = pgm_read_byte(btOutputString++); // read in a character of output list

			if (btChar)  // if this is a valid character
			{

				btFormat = findFormat(btChar); // go find the corresponding output format

				if (btFormat) // if this is a valid format
				{

					text::charOut(devBluetooth, btChar); // output character corresponding to output format

					if ((btF->u8[1] == instantIdx) && (btF->u8[0] == tFuelEcon)) // check if swap with fuel consumption rate is needed
					{

						if (SWEET64::runPrgm(prgmCheckInstantSpeed, 0) == 0) btF->u8[0] = tFuelRate;

					}

					btChar = ((btF->u8[0] < dfMaxValDisplayCount) ? 7 : 10);

					text::tripFunctionOut(devBluetooth, btFormat, btChar, (dfOutputBluetooth));

#if defined(bluetoothSerialBuffer)
					btOutputState |= (btoFlagFlushBuffer);

#else // defined(bluetoothSerialBuffer)
					btDelayFlag = heart::delay0(delay0020msTick); // set for a 20 ms delay
					btOutputState |= (btoFlagDelay); // allows smartphone app time to process variable just transmitted

#endif // defined(bluetoothSerialBuffer)
				}

			}
			else btOutputState &= ~(btoFlagActiveOutput); // finished outputting list

		}
		while ((btChar) && (btFormat == 0)); // loop back if we found a valid character but an invalid format for that character

	}

}

static void bluetooth::mainOutput(void)
{

	if (activityFlags & afBluetoothOutput)
	{

		heart::changeBitFlags(activityFlags, afBluetoothOutput, 0); // acknowledge update bluetooth output command

		if ((btOutputState & btoOutputEnabledFlags) && ((btOutputState & btoFlagActiveOutput) == 0))
		{

			btOutputState &= ~(btoFlagSingleShotOutput); // clear single-shot flag
			btOutputState |= (btoFlagActiveOutput); // enable bluetooth::mainProcess output

			btOutputString = findStr(btOutputList, btOutputListIdx); // get selected bluetooth output list string pointer

		}

	}

}

#endif // defined(useBluetooth)
