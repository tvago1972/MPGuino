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
	btInputState = btiResetFlag;

	text::initDev(m8DevBluetoothIdx, (odvFlagEnableOutput), blefriend::chrOut, chrIn);

#else // defined(useBluetoothAdaFruitSPI)
	mainProgram8Variables[(uint16_t)(m8DevBluetoothIdx - m8VariableStartIdx)] &= ~(odvFlagCRLF);
	btInputState = 0;

#endif // defined(useBluetoothAdaFruitSPI)
	btOutputState = btoFlagContinuousOutput;
	btOutputListIdx = btolTripFunctionIdx;

}

static void bluetooth::shutdown(void)
{

	btInputState = 0;
	btOutputState = 0;

}

#if defined(useBluetoothAdaFruitSPI)
static uint8_t bluetooth::chrIn(void)
{

	if (ringBuffer::isBufferEmpty(rbIdxBluetoothIn)) return 0;
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
	if (btInputState & btiResetFlag) // if performing a /RST cycle
	{

		if ((volatile8Variables[(uint16_t)(v8BLEstatus - v8VariableStartIdx)] & bleResetFlags) == 0) // if /RST cycle is completed
		{

			btInputState &= ~(btiResetFlag); // mark /RST cycle as completed

			for (uint8_t x = 0; x < 4; x++)
			{

				text::stringOut(m8DevBluetoothIdx, btResetString, x); // send a string of reset message

				blefriend::outputBufferWithResponse(); // go output via AT wrapper, and pop response from input

			}

			ringBuffer::empty(rbIdxBLEfriendIn);

			btDelayFlag = heart::delay0(delay0Tick500ms); // set for a 1/2 sec delay
			btOutputState |= (btoFlagDelay); // allows smartphone app time to process variable just transmitted

		}

	}

#else // defined(useBluetoothAdaFruitSPI)
	btInputState &= ~(btiResetFlag);

#endif // defined(useBluetoothAdaFruitSPI)
	if ((btInputState & btiResetFlag) == 0)
	{

#if defined(useBluetoothAdaFruitSPI)
		if (btInputState & btiAllowPolling)
		{

			if (volatile8Variables[(uint16_t)(v8Timer0StatusB - v8VariableStartIdx)] & t0sbSampleBLEfriend)
			{

				heart::changeBitFlagBits(v8Timer0StatusB - v8VariableStartIdx, t0sbSampleBLEfriend, 0);

				text::stringOut(m8DevBluetoothIdx, btInputString);
				blefriend::outputBufferWithResponse(); // output GATT input request via 0xFFE1 characteristic

				k = 1;

				while (ringBuffer::isBufferNotEmpty(rbIdxBLEfriendIn)) // read GATT response from 0xFFE1 characteristic
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
				if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekBluetoothInput) text::charOut(m8DevDebugTerminalIdx, btChar);

#endif // defined(useDebugTerminal)
				switch (btInputState)
				{

					case 'M':	// processing an expanded command
						switch (btChar)
						{

							case 'M':	// output selected EEPROM parameter list
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
													EEPROM::onChange(prgmWriteBTparameterValue, btF->u8[1]);
													heart::changeBitFlagBits(v8Timer0Command - v8VariableStartIdx, 0, t0cResetInputTimer);
													break;

												case tGetProgramVariableValue:
													str2ull(btInpBuff); // convert digit string into a number
													SWEET64::runPrgm(prgmWriteProgramVariableValue, btF->u8[1]);
													heart::changeBitFlagBits(v8Timer0Command - v8VariableStartIdx, 0, t0cResetInputTimer);
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

#if defined(rbIdxBluetoothSerial)
		if (btOutputState & btoFlagFlushBuffer) // flush the output ring buffer
		{

			if (ringBuffer::isBufferEmpty(rbIdxBluetoothSerial)) // if the output ring buffer is flushed
			{

				btOutputState &= ~(btoFlagFlushBuffer);
				btDelayFlag = heart::delay0(delay0Tick20ms); // set for a 20 ms delay
				btOutputState |= (btoFlagDelay); // allows smartphone app time to process variable just transmitted

			}

		}

#endif // defined(rbIdxBluetoothSerial)
		if (btOutputState & btoFlagDelay) // check if output delay is finished
		{

			if ((volatile8Variables[(uint16_t)(v8Timer0Delay - v8VariableStartIdx)] & btDelayFlag) == 0)
			{

				btOutputState &= ~(btoFlagDelay); // if delay is finished, allow output to continue

#if defined(useBluetoothAdaFruitSPI)
				if ((btInputState & btiAllowPolling) == 0)
				{

					btInputState |= (btiAllowPolling);
					heart::changeBitFlagBits(v8Timer0Command - v8VariableStartIdx, 0, t0cEnableBLEsample);

				}

#endif // defined(useBluetoothAdaFruitSPI)
			}

		}

		if ((btOutputState & btoOutputActiveFlags) == btoFlagActiveOutput)
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
						if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekBluetoothOutput) text::charOut(m8DevDebugTerminalIdx, btChar);
#endif // defined(useDebugTerminal)

						if ((btF->u8[1] == instantIdx) && (btF->u8[0] == tFuelEcon)) // check if swap with fuel consumption rate is needed
						{

							if (SWEET64::runPrgm(prgmCheckInstantSpeed, 0) == 0) btF->u8[0] = tFuelRate;

						}

						btChar = ((btF->u8[0] < dfMaxValDisplayCount) ? 7 : 10);

						text::tripFunctionOut(m8DevBluetoothIdx, btFormat, btChar, (dfOutputBluetooth));
#if defined(useDebugTerminal)
						if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekBluetoothOutput) text::tripFunctionOut(m8DevDebugTerminalIdx, btFormat, btChar, (dfOutputBluetooth));
#endif // defined(useDebugTerminal)
#if defined(useBluetoothAdaFruitSPI)
						blefriend::outputBufferWithResponse(); // send out via 0xFFE1 characteristic
						ringBuffer::empty(rbIdxBLEfriendIn); // pop response from input buffer
						btDelayFlag = heart::delay0(delay0Tick20ms); // set for a 20 ms delay
						btOutputState |= (btoFlagDelay); // allows smartphone app time to process variable just transmitted

#else // defined(useBluetoothAdaFruitSPI)
#if defined(rbIdxBluetoothSerial)
						btOutputState |= (btoFlagFlushBuffer);

#else // defined(rbIdxBluetoothSerial)
						btDelayFlag = heart::delay0(delay0Tick20ms); // set for a 20 ms delay
						btOutputState |= (btoFlagDelay); // allows smartphone app time to process variable just transmitted

#endif // defined(rbIdxBluetoothSerial)
#endif // defined(useBluetoothAdaFruitSPI)
					}

				}
				else btOutputState &= ~(btoFlagActiveOutput); // finished outputting list

			}
			while ((btChar) && (btFormat == 0)); // loop back if we found a valid character but an invalid format for that character

		}

	}

}

static void bluetooth::mainOutput(void)
{

	if ((btOutputState & btoOutputEnabledFlags) && ((btOutputState & btoFlagActiveOutput) == 0))
	{

		btOutputState &= ~(btoFlagSingleShotOutput); // clear single-shot flag
		btOutputState |= (btoFlagActiveOutput); // enable bluetooth::mainProcess output

		btOutputStringPtr = findStr(btOutputList, btOutputListIdx); // get selected bluetooth output list string pointer

	}

}

#endif // defined(useBluetooth)
