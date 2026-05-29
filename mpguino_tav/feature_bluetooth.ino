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

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	v08(v8btOutputStatusIdx) = btoFlagContinuousOutput;
	btOutputListIdx = btolTripFunctionIdx;

	SREG = oldSREG; // restore interrupt flag status

#if defined(useBluetoothAdaFruitSPI)
	m08(m8btInputStateIdx) = (btiResetFlag);

	text::initDev(m8DevBluetoothIdx, (odvFlagEnableOutput), blefriend::chrOut, chrIn);

#else // defined(useBluetoothAdaFruitSPI)
	m08(m8DevBluetoothIdx) &= ~(odvFlagCRLF);
	m08(m8btInputStateIdx) = 0;

#endif // defined(useBluetoothAdaFruitSPI)
}

static void bluetooth::shutdown(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	v08(v8btOutputStatusIdx) = 0;

	SREG = oldSREG; // restore interrupt flag status

	m08(m8btInputStateIdx) = 0;

}

#if defined(useBluetoothAdaFruitSPI)
static uint8_t bluetooth::chrIn(void)
{

	if (ringBuffer::testBuffer(rbIdxBluetoothIn, bufferIsEmpty)) return 0;
	else return ringBuffer::pull(rbIdxBluetoothIn);

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
	uint8_t c;
	uint8_t k;
	union union_16 * btF = (union union_16 *)(&btFormat);

#if defined(useBluetoothAdaFruitSPI)
	if (m08(m8btInputStateIdx) & btiResetFlag) // if performing a /RST cycle
	{

		if ((v08(v8BLEstatusIdx) & bleResetFlags) == 0) // if /RST cycle is completed
		{

			m08(m8btInputStateIdx) &= ~(btiResetFlag); // mark /RST cycle as completed

			for (uint8_t x = 0; x < 4; x++)
			{

				text::stringOut(m8DevBluetoothIdx, btResetString, x); // send a string of reset message

				blefriend::outputBufferWithResponse(); // go output via AT wrapper, and pop response from input

			}

			ringBuffer::empty(rbIdxBLEfriendIn);

			 // set for a 1/2 sec delay so that smartphone app can have time to process variable just transmitted
			heart::changeBitFlagBits(v8btOutputStatusIdx, 0, (btoFlagDelay | btoFlagDelayInit500ms));

		}

	}

#else // defined(useBluetoothAdaFruitSPI)
	m08(m8btInputStateIdx) &= ~(btiResetFlag);

#endif // defined(useBluetoothAdaFruitSPI)
	if ((m08(m8btInputStateIdx) & btiResetFlag) == 0)
	{

#if defined(useBluetoothAdaFruitSPI)
		if (m08(m8btInputStateIdx) & btiAllowPolling)
		{

			if (v08(v8Timer0Status1Idx) & t0sbSampleBLEfriend)
			{

				heart::changeBitFlagBits(v8Timer0Status1Idx, t0sbSampleBLEfriend, 0);

				text::stringOut(m8DevBluetoothIdx, btInputString);
				blefriend::outputBufferWithResponse(); // output GATT input request via 0xFFE1 characteristic

				k = 1;

				while (ringBuffer::testBufferNot(rbIdxBLEfriendIn, bufferIsEmpty)) // read GATT response from 0xFFE1 characteristic
				{

					c = ringBuffer::pull(rbIdxBLEfriendIn);

					if (c < 0x20) k = 0;

					if ((k) && (c != ' ')) ringBuffer::push(rbIdxBluetoothIn, c);

				}


			}

		}

#endif // defined(useBluetoothAdaFruitSPI)
		do
		{

			btChar = text::chrIn(m8DevBluetoothIdx); // read in a character from the input buffer

			if (btChar) // if a valid character was read in
			{

#if defined(useDebugTerminal)
				if (m08(m8PeekFlags) & peekBluetoothInput) text::charOut(m8DevDebugTerminalIdx, btChar);

#endif // defined(useDebugTerminal)
				switch (m08(m8btInputStateIdx))
				{

					case 'M':	// processing an expanded command
						switch (btChar)
						{

							case 'M':	// output selected EEPROM parameter list
								heart::changeBitFlagBits(v8btOutputStatusIdx, btoOutputFlags, btoFlagSingleShotOutput); // clear all output flags, set up for one-shot output

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

						m08(m8btInputStateIdx) = 0;
						break;

					case '!':
						switch (btChar)
						{

							case '!':
								heart::changeBitFlagBits(v8btOutputStatusIdx, btoOutputFlags, btoFlagContinuousOutput); // clear all output flags, set up for continuous output
								btOutputListIdx = btolTripFunctionIdx;
								m08(m8btInputStateIdx) = 1; // treat this also as a variable write, with special character substituting for '!'
								btInpBuffIdx = 0; // reset input buffer
								break;

							default:	// unsupported command
								m08(m8btInputStateIdx) = 0;
								break;

						}

						break;

					case 0:		// normal command processing
						switch (btChar)
						{

							case '!':	// initialize and output selected trip functions
							case 'M':	// process expanded command
								m08(m8btInputStateIdx) = btChar; // save input state
								break;

							default:	// unrecognized command - could be a variable write
								btFormat = findFormat(btChar); // go find the corresponding output format

								if (btFormat) // if format was found, this is a variable write
								{

									m08(m8btInputStateIdx) = btChar; // save variable character
									btInpBuffIdx = 0; // reset input buffer

								}

								break;

						}
						break;

					default:	// K-delimited string processing
						switch (btChar)
						{

							default:	// unrecognized character - reset number input
								m08(m8btInputStateIdx) = 0;
								break;

							case '0' ... '9':	// digits
								if (m08(m8btInputStateIdx) & 0x80) // if in check digit mode, and read in digit does not equal stored digit, abort
								{

									if (btChar != btInpBuff[(uint16_t)(btInpBuffIdx++)]) m08(m8btInputStateIdx) = 0;

								}
								else // if not, we are in digit storage mode
									btInpBuff[(uint16_t)(btInpBuffIdx++)] = btChar; // store digit

								break;

							case 'K':	// number string terminator
								m08(m8btInputStateIdx) ^= 0x80; // toggle digit mode

								if (m08(m8btInputStateIdx) & 0x80) btInpBuff[(uint16_t)(btInpBuffIdx)] = 0; // if in check digit mode, mark end of string
								else // if back in digit storage mode, time to convert number string
								{

									// if end of digit string is correct, then the two input digit strings are identical
									if (btInpBuff[(uint16_t)(btInpBuffIdx)] == 0)
									{

										if (m08(m8btInputStateIdx) == 1) btChar = '!'; // re-translate back to '!' for output
										else btChar = m08(m8btInputStateIdx);

										btFormat = findFormat(btChar); // go find the corresponding output format

										if (btFormat)
										{

											switch (btF->u08[0])
											{

												case tGetBTparameterValue:
													str2ull(btInpBuff); // convert digit string into a number
#if defined(usePartialRefuel)
													if (btF->u08[1] == pRefuelSizeIdx) SWEET64::runPrgm(prgmAddToPartialRefuel, 0);
#endif // defined(usePartialRefuel)
													EEPROM::onChange(prgmWriteBTparameterValue, btF->u08[1]);
													heart::changeBitFlagBits(v8Timer0CommandIdx, 0, t0cResetInputActivityTimer);
													break;

												case tGetProgramVariableValue:
													str2ull(btInpBuff); // convert digit string into a number
													SWEET64::runPrgm(prgmWriteProgramVariableValue, btF->u08[1]);
													heart::changeBitFlagBits(v8Timer0CommandIdx, 0, t0cResetInputActivityTimer);
													break;

												default:
													break;

											}

										}

									}

									m08(m8btInputStateIdx) = 0; // reset input state

								}

								btInpBuffIdx = 0;

								break;

						}
						break;

				}

			}

		}
		while (btChar);

#if defined(rbIdxBluetoothSerial)
		if (v08(v8btOutputStatusIdx) & btoFlagFlushBuffer) // flush the output ring buffer
		{

			if (ringBuffer::testBuffer(rbIdxBluetoothSerial, bufferIsEmpty)) // if the output ring buffer is flushed
			{

				// set up for a 20 ms delay to allow smartphone app to process variable just transmitted
				heart::changeBitFlagBits(v8btOutputStatusIdx, btoFlagFlushBuffer, (btoFlagDelayInit20ms | btoFlagDelay));

			}

		}

#endif // defined(rbIdxBluetoothSerial)
		if ((v08(v8btOutputStatusIdx) & btoOutputActiveFlags) == btoFlagActiveOutput)
		{

			do
			{

				btChar = pgm_read_byte(btOutputStringPtr++); // read in a character of output list

				if (btChar)  // if this is a valid character
				{

					btFormat = findFormat(btChar); // go find the corresponding output format

					if (btFormat) // if this is a valid format
					{

#if defined(useBluetoothAdaFruitSPI)
						text::stringOut(m8DevBluetoothIdx, btOutputString);
#endif // defined(useBluetoothAdaFruitSPI)
						text::charOut(m8DevBluetoothIdx, btChar); // output character corresponding to output format
#if defined(useDebugTerminal)
						if (m08(m8PeekFlags) & peekBluetoothOutput) text::charOut(m8DevDebugTerminalIdx, btChar);
#endif // defined(useDebugTerminal)

						if ((btF->u08[1] == instantIdx) && (btF->u08[0] == tFuelEcon)) // check if swap with fuel consumption rate is needed
						{

							if (SWEET64::runPrgm(prgmCheckInstantSpeed, 0) == 0) btF->u08[0] = tFuelRate;

						}

						btChar = ((btF->u08[0] < dfMaxValDisplayCount) ? 7 : 10);

						text::tripFunctionOut(m8DevBluetoothIdx, btFormat, btChar, (dfOutputBluetooth));
#if defined(useDebugTerminal)
						if (m08(m8PeekFlags) & peekBluetoothOutput) text::tripFunctionOut(m8DevDebugTerminalIdx, btFormat, btChar, (dfOutputBluetooth));
#endif // defined(useDebugTerminal)
#if defined(useBluetoothAdaFruitSPI)
						blefriend::outputBufferWithResponse(); // send out via 0xFFE1 characteristic
						ringBuffer::empty(rbIdxBLEfriendIn); // pop response from input buffer

						// set up for a 20 ms delay to allow smartphone app to process variable just transmitted
						heart::changeBitFlagBits(v8btOutputStatusIdx, 0, (btoFlagDelayInit20ms | btoFlagDelay));

#else // defined(useBluetoothAdaFruitSPI)
#if defined(rbIdxBluetoothSerial)
						heart::changeBitFlagBits(v8btOutputStatusIdx, 0, btoFlagFlushBuffer);

#else // defined(rbIdxBluetoothSerial)
						// set up for a 20 ms delay to allow smartphone app to process variable just transmitted
						heart::changeBitFlagBits(v8btOutputStatusIdx, 0, (btoFlagDelayInit20ms | btoFlagDelay));

#endif // defined(rbIdxBluetoothSerial)
#endif // defined(useBluetoothAdaFruitSPI)
					}

				}
				else heart::changeBitFlagBits(v8btOutputStatusIdx, btoFlagActiveOutput, 0); // finished outputting list

			}
			while ((btChar) && (btFormat == 0)); // loop back if we found a valid character but an invalid format for that character

		}

	}

}

static void bluetooth::mainOutput(void)
{

	if ((v08(v8btOutputStatusIdx) & btoOutputEnabledFlags) && ((v08(v8btOutputStatusIdx) & btoFlagActiveOutput) == 0))
	{

		// clear single-shot flag, and enable bluetooth::mainProcess output
		heart::changeBitFlagBits(v8btOutputStatusIdx, btoFlagSingleShotOutput, btoFlagActiveOutput);

		btOutputStringPtr = findStr(btOutputList, btOutputListIdx); // get selected bluetooth output list string pointer

	}

}

#endif // defined(useBluetooth)
