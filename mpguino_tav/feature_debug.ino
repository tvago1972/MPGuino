#ifdef useCPUreading
/* CPU loading and RAM availability support section */

static const uint8_t prgmFindCPUutilPercent[] PROGMEM = {
	instrLdRegMain, 0x02, mpMainLoopAccumulatorIdx,
	instrSubMainFromX, 0x02, mpIdleAccumulatorIdx,
	instrMul2byByte, 100,
	instrMul2byConst, idxDecimalPoint,
	instrDiv2byMain, mpMainLoopAccumulatorIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmOutputAvailableRAM[] PROGMEM = {
	instrLdRegMain, 0x02, mpAvailableRAMidx,
	instrMul2byConst, idxDecimalPoint,
	instrDone											// exit to caller
};

static uint8_t systemInfo::displayHandler (uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint16_t availableRAMptr;
	uint8_t retVal = 0;

	if((unsigned int)__brkval == 0) availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)&__bss_end);
	else availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)__brkval);

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx: // display max cpu utilization and RAM
			showCPUload();
			text::stringOut(devLCD, PSTR(" T"));
			text::stringOut(devLCD, ull2str(tReadTicksToSeconds, mBuff1, vSystemCycleIdx)); // output system time (since MPGuino was powered up)

			text::gotoXY(devLCD, 0, 1);
			text::stringOut(devLCD, PSTR("FREE RAM: "));
			mainProgramVariables[(uint16_t)(mpAvailableRAMidx)] = availableRAMptr;
			SWEET64::runPrgm(prgmOutputAvailableRAM, 0);
			text::stringOut(devLCD, formatDecimal(mBuff1, 6, 2, 0));
			break;

		default:
			break;

	}

	return retVal;

}

static void systemInfo::showCPUload(void)
{

	text::stringOut(devLCD, PSTR("C%"));
	SWEET64::runPrgm(prgmFindCPUutilPercent, 0);
	text::stringOut(devLCD, formatDecimal(mBuff1, 6, 2, 0));
#ifdef useDebugCPUreading
	monitorState = 1;
#endif // useDebugCPUreading

}

static void systemInfo::showCPUloading(void)
{

	initStatusLine();
	showCPUload();
	text::charOut(devLCD, 0x0D);
	execStatusLine();

}

static uint16_t systemInfo::getAvailableRAMsize(void)
{

}

static uint32_t systemInfo::findCycleLength(unsigned long lastCycle, unsigned long thisCycle)
{

	if (thisCycle < lastCycle) thisCycle = 4294967295ul - lastCycle + thisCycle + 1;
	else thisCycle = thisCycle - lastCycle;

	return thisCycle;

}

static uint32_t systemInfo::cycles0(void)
{

	uint8_t oldSREG;
	unsigned long t;
	unsigned int a;

	oldSREG = SREG; // save state of interrupt flag
	cli(); // disable interrupts

	a = (unsigned int)(TCNT0); // do a microSeconds() - like read to determine loop length in cycles
	if (TIFR0 & (1 << TOV0)) a = (unsigned int)(TCNT0) + 256; // if overflow occurred, re-read with overflow flag taken into account

	t = timer0_overflow_count + (unsigned long)(a);

	SREG = oldSREG; // restore state of interrupt flag

	return t;

}

#endif // useCPUreading
#ifdef useTestButtonValues
/* Button input value viewer section */

static uint8_t buttonView::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
#ifdef useAnalogButtons
			text::hexWordOut(devLCD, (uint16_t)(analogValue[(unsigned int)(analogButtonChannelIdx)]));
#endif // useAnalogButtons
			text::newLine(devLCD);
			text::hexByteOut(devLCD, thisButtonState);
			text::hexByteOut(devLCD, lastButtonState);
#ifdef useTestAnalogButtonIdx
			text::stringOut(devLCD, PSTR(" pins, "));
			text::hexByteOut(devLCD, thisButtonIdx);
			text::stringOut(devLCD, PSTR(" idx"));
#else // useTestAnalogButtonIdx
			text::stringOut(devLCD, PSTR(" pins"));
			text::newLine(devLCD);
#endif // useTestAnalogButtonIdx
			break;

		default:
			break;

	}

	return retVal;

}

#endif // useTestButtonValues
#ifdef useDebugTerminal
/* debug terminal section */

static const uint8_t prgmParseDecimalDigit[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 10,								// multiply by 10
	instrJump, tParseCharacterToReg
};

static const uint8_t prgmParseHexDigit[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 16,								// multiply by 16
	instrJump, tParseCharacterToReg
};

static const uint8_t prgmFetchMainProgramValue[] PROGMEM = {
	instrLdRegMainIndexed, 0x02,
	instrDone
};

static const uint8_t prgmFetchConstantValue[] PROGMEM = {
	instrLdRegConstIndexed, 0x02,
	instrDone
};

static const uint8_t prgmFetchVolatileValue[] PROGMEM = {
	instrLdRegVolatileIndexed, 0x02,
	instrDone
};

static const uint8_t prgmFetchTripVarValue[] PROGMEM = {
	instrLdRegTripVarIndexedRV, 0x02, terminalIdx,
	instrDone
};

static const uint8_t prgmWriteMainProgramValue[] PROGMEM = {
	instrStRegMainIndexed, 0x06,
	instrDone
};

static const uint8_t prgmWriteVolatileValue[] PROGMEM = {
	instrStRegVolatileIndexed, 0x06,
	instrDone
};

static const uint8_t prgmWriteTripMeasurementValue[] PROGMEM = {
	instrStRegTripVarIndexedRV, 0x06, terminalIdx,
	instrDone
};

#ifdef useLegacyButtons
static const uint8_t terminalButtonCount = 6;

static const char terminalButtonChars[] PROGMEM = {
	"lcrLCR\0"
};

static const uint8_t terminalButtonValues[] PROGMEM = {
	 btnShortPressL
	,btnShortPressC
	,btnShortPressR
	,btnLongPressL
	,btnLongPressC
	,btnLongPressR
};

#else // useLegacyButtons
static const uint8_t terminalButtonCount = 10;

static const char terminalButtonChars[] PROGMEM = {
	"lcrudLCRUD\0"
};

static const char terminalButtonValues[] PROGMEM = {
	 btnShortPressL
	,btnShortPressC
	,btnShortPressR
	,btnShortPressU
	,btnShortPressD
	,btnLongPressL
	,btnLongPressC
	,btnLongPressR
	,btnLongPressU
	,btnLongPressD
};

#endif // useLegacyButtons
const char terminalActivityFlagStr[] PROGMEM = {
	"activityFlags: \0"
	"EngOff" "\xEC" "running\0"
	"VehStop" "\xEC" "moving\0"
	"NoButtons" "\xEC" "pressed\0"
	"Parked" "\xEC" "notparked\0"
	"Inactive" "\xEC" "active\0"
	"FuelRate" "\xEC" "fuelecon\0"
#ifdef useTWIbuttons
	"TWIsample" "\xEC" "twi\0"
#else // useTWIbuttons
	"1" "\xEC" "0\0"
#endif // useTWIbuttons
	"1" "\xEC" "0\0"
};

static void terminal::outputFlags(uint8_t flagRegister, const char * flagStr)
{

	uint8_t bitMask;
	uint8_t c;

	bitMask = 0x80;

	text::stringOut(devDebugTerminal, flagStr);

	for (uint8_t x = 1; x < 9; x++)
	{

		text::setModeOnCondition(devDebugTerminal, (flagRegister & bitMask), odvFlagEnableOutput);
		text::stringOut(devDebugTerminal, flagStr, x);
		text::setModeOnCondition(devDebugTerminal, 1, odvFlagEnableOutput);

		if (x == 8) c = 0x0D;
		else c = 0x20;
		text::charOut(devDebugTerminal, c);

		bitMask >>= 1;

	}

}

static void terminal::outputTripFunctionValue(uint8_t lineNumber)
{

	uint8_t i;

	i = text::numberOut(devDebugTerminal, terminalIdx, lineNumber, termNumberBuff, 0, dfOverflow9s);
	text::charOut(devDebugTerminal, ' ');
	text::stringOut(devDebugTerminal, terminalFormats, i);

}

static void terminal::outputConstantValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchConstantValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputParameterValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputParameterExtra(uint8_t lineNumber)
{

	text::hexByteOut(devDebugTerminal, EEPROM::getParameterFlags(lineNumber));
	text::charOut(devDebugTerminal, '-');
	text::hexByteOut(devDebugTerminal, EEPROM::getLength(lineNumber));
	text::charOut(devDebugTerminal, '-');
	text::hexByteOut(devDebugTerminal, EEPROM::getAddress(lineNumber));
	if (lineNumber < eePtrSettingsEnd)
	{

		text::stringOut(devDebugTerminal, PSTR(" (orig "));
		SWEET64::runPrgm(prgmFetchInitialParamValue, lineNumber);
		text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));
		text::stringOut(devDebugTerminal, PSTR(")"));

	}
	else
	{

		text::charOut(devDebugTerminal, ' ');
		SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);
		text::hexLWordOut(devDebugTerminal, &s64reg[s64reg2]);
		text::charOut(devDebugTerminal, ' ');

	}

}

static void terminal::outputVolatileValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchVolatileValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputMainProgramValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchMainProgramValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputTripVarMeasuredValue(uint8_t lineNumber)
{


	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(tFormatToNumber, termNumberBuff, 3));

}

static void terminal::outputTripVarMeasuredExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::hexLWordOut(devDebugTerminal, &s64reg[s64reg2]);

}

static void terminal::mainProcess(void)
{

	uint8_t i;
	uint8_t j;
	uint8_t oldSREG;

/*

	terminal commands:
      P - list stored parameters
      V - list volatile variables
      M - list main program variables
      O - list program constants
      T - list read-in trip variable values
      L - list functions for read-in trip variable
     ^L - list SWEET64 source code for function
   [x]R - read trip variable x into terminal trip variable
   [x]W - write terminal trip variable into trip variable x
          if no x specified, lists available trip variables
      S - output system status
      I - inject button press
            short (l, c, r, u, d)
             long (L, C, R, U, D)
      ? - displays this help

	numbers or button presses are separated by spaces

	ex:
	]1r           << reads instant trip variable into terminal

	]1l 2l 3l     << prints functions 1, 2, and 3 associated with read-in trip variable

	]l            << prints all available functions associated with read-in  trip variable

	]ild          << injects short button press l+d into MPGuino

	]ir r r r     << injects 4 successive short press r into MPGuino

	]irc R LC r   << injects short press r+c, long press r, long press l+c, short press r into MPGuino

*/
	switch (terminalState)
	{

		case 0:		// initialize terminal state machine and output the prompt
			text::charOut(devDebugTerminal, 0x0D);
			text::charOut(devDebugTerminal, ']');
			terminalState++;
			inpIdx = 0;
			readIdx = 0;
			terminalMode = tmHexInput;
			terminalByte = 0;
			terminalAddress = 0;
		case 1:		// get line
			i = devDebugTerminal.chrIn();

			if (i)
			{

				switch (i)
				{

					case 0x7F:
					case 0x08:
						if (inpIdx) inpIdx--;
						else
						{

							text::charOut(devDebugTerminal, 0x0D);
							terminalState = 0;

						}
						break;

					case 0x0D:
						text::charOut(devDebugTerminal, 0x0D);
						terminalState++;
						break;

					case 0x18:
						text::charOut(devDebugTerminal, '\\');
						terminalState = 0;
						break;

#ifdef useSWEET64trace
					case 0x0C:
#endif // useSWEET64trace
					case 0x20 ... 0x7E:
						if (inpIdx == tBuffLength)
						{

							text::stringOut(devDebugTerminal, PSTR("\\\r"));
							terminalState = 0;

						}
						else
						{

							if (i > 31)	text::charOut(devDebugTerminal, i);
							else
							{

								text::charOut(devDebugTerminal, '^');
								text::charOut(devDebugTerminal, i + 64);

							}
							terminalBuff[(uint16_t)(inpIdx++)] = i;

						}
						break;

					default:
						break;

				}

			}
			break;

		case 2:   // process input line
			if (readIdx >= inpIdx) chr = 0x0D;
			else chr = terminalBuff[(uint16_t)(readIdx++)];

			i = 1;
			if ((terminalMode & tmSomethingReadIn) == 0)
			{

				terminalByte = 0;
				SWEET64::init64byt((union union_64 *)(&s64reg[s64reg6]), 0);

			}
			if (chr > 0x5F) j = chr - 0x20;
			else j = chr;
			switch (terminalMode)
			{

				case 'I':										// parse a button press character
				case ('I' | tmSomethingReadIn):					// parse a button press character
					for (uint8_t x = 0; x < terminalButtonCount; x++)
						if (chr == pgm_read_byte(&terminalButtonChars[(uint16_t)(x)]))
						{

							i = 0; // signal that a valid character was already read in
							terminalByte |= (pgm_read_byte(&terminalButtonValues[(uint16_t)(x)]));
							terminalMode |= (tmSomethingReadIn);

						}
					break;

				case ('.' | tmHexInput):						// parse an address range upper limit
#ifdef useSWEET64trace
				case (0x1B | tmHexInput):						// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
				case (0x0D | tmHexInput):						// parse a specific main program variable hexadecimal value input digit
				case (0x10 | tmHexInput):						// parse a specific parameter value input hexadecimal digit
				case (0x14 | tmHexInput):						// parse a specific trip variable measurement hexadecimal value input digit
				case (0x16 | tmHexInput):						// parse a specific volatile variable value input hexadecimal digit
				case (tmHexInput):								// parse a generic hexadecimal digit
				case ('.' | tmSomethingReadIn | tmHexInput):	// parse an address range upper limit
#ifdef useSWEET64trace
				case (0x1B | tmSomethingReadIn | tmHexInput):	// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
				case (0x0D | tmSomethingReadIn | tmHexInput):	// parse a specific main program variable value input hexadecimal digit
				case (0x10 | tmSomethingReadIn | tmHexInput):	// parse a specific parameter value input hexadecimal digit
				case (0x14 | tmSomethingReadIn | tmHexInput):	// parse a specific trip variable measurement value input hexadecimal digit
				case (0x16 | tmSomethingReadIn | tmHexInput):	// parse a specific volatile variable value input hexadecimal digit
				case (tmSomethingReadIn | tmHexInput):			// parse a generic hexadecimal digit
					switch (j)
					{

						case 'A' ... 'F':
							j -= 7;
						case '0' ... '9':
							j -= 48;
							i = 0; // signal that a valid character was already read in
							terminalMode |= (tmSomethingReadIn);
							terminalByte = SWEET64::runPrgm(prgmParseHexDigit, j);
							break;

						default:
							break;

					}
					break;

				case '.':										// parse an address range upper limit
#ifdef useSWEET64trace
				case 0x1B:										// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
				case 0x0D:										// parse a specific main program decimal digit variable input decimal digit or switch to hex mode
				case 0x10:										// parse a specific parameter decimal digit input decimal digit or switch to hex mode
				case 0x14:										// parse a specific trip variable measurement decimal digit input decimal digit or switch to hex mode
				case 0x16:										// parse a specific volatile variable decimal digit input decimal digit or switch to hex mode
				case 0x00:										// parse a generic decimal digit or switch to hex mode
					if ((j == 'X') || (j == '$'))
					{

						terminalMode |= (tmHexInput);
						i = 0; // signal that a valid character was already read in
						break;

					}

				case ('.' | tmSomethingReadIn):					// parse an address range upper limit
#ifdef useSWEET64trace
				case (0x1B | tmSomethingReadIn):				// parse a specific SWEET64 argument hexadecimal value input digit
#endif // useSWEET64trace
				case (0x0D | tmSomethingReadIn):				// parse a specific main program variable decimal digit
				case (0x10 | tmSomethingReadIn):				// parse a specific parameter decimal digit
				case (0x14 | tmSomethingReadIn):				// parse a specific trip variable measurement decimal digit
				case (0x16 | tmSomethingReadIn):				// parse a specific volatile variable decimal digit
				case (tmSomethingReadIn):						// parse a generic decimal digit
					switch (j)
					{

						case '0' ... '9':
							j -= 48;
							i = 0; // signal that a valid character was already read in
							terminalMode |= (tmSomethingReadIn);
							terminalByte = SWEET64::runPrgm(prgmParseDecimalDigit, j);
							break;

						default:
							break;

					}
				default:										// nothing to parse - could be a possible command
					break;

			}

			if (i) // parsing wasn't valid, so check for a command
			{

				if (chr > 0x5F) chr &= 0x5F;
				nextTerminalState = 2; // get another character from input buffer
				switch (chr)
				{

					case '?':   // display help
						terminalLine = 0; // initialize terminal output line
						terminalMode = tmHexInput; // shift to parsing a generic input value
						terminalState = 12; // this command WILL print a lot of different lines, so handle this command one iteration at a time
						break;

#ifdef useSWEET64trace
					case 0x0C:
#endif // useSWEET64trace
					case 'L':   // list available trip functions from terminalIdx
					case 'M':   // list available main program variables
					case 'O':	// list available program constants
					case 'P':   // list available stored parameters
					case 'T':   // list available trip variable measurements
					case 'V':   // list available volatile variables
						i = terminalMode; // save old terminal mode
						terminalMode = chr; // prepare to shift to parsing an array-specific input value
						chr = '>'; // do unified list output preparation
						break;

					case '.':	// specify address range
						terminalAddress = terminalByte;
						terminalMode = (chr | tmHexInput); // shift to reading button press words
						break;

					case 'I':   // inject button press
						terminalMode = chr; // shift to reading button press words
						break;

					case 'W':	// write trip variable
						terminalLine = terminalByte;
						terminalByte = terminalIdx;
						terminalState = 20;
						break;

					case 'R':   // read trip variable into terminal
						terminalLine = terminalIdx;
						terminalState = 20;
						break;

					case 'S':   // print system status
						terminal::outputFlags(activityFlags, terminalActivityFlagStr);
						terminalMode = tmHexInput; // shift to parsing a generic input value
						break;

					case 0x0D:    // carriage return - treat as a special space
						nextTerminalState = 0; // when finished processing, go back to terminal state 0 - initialize input and print prompt character
					case ' ':   // space character
						switch (terminalMode)
						{

							case ('I' | tmSomethingReadIn):
								input::inject(terminalByte); // inject the parsed button press value into timer0
								terminalState = 14;
								terminalMode &= ~(tmSomethingReadIn); // go parse another button press value
								break;

#ifdef useSWEET64trace
							case 0x1B:	// if no address was specified
								primaryFunc = 0;
#endif // useSWEET64trace
							case 'L':	// if no address was specified
							case 'M':	// if no address was specified
							case 'O':	// if no address was specified
							case 'P':	// if no address was specified
							case 'T':	// if no address was specified
							case 'V':	// if no address was specified
							case (0x0D):	// if no values were input
							case (0x10):	// if no values were input
							case (0x14):	// if no values were input
							case (0x16):	// if no values were input
								terminalState = 32; // this command could print a lot of different lines, so handle this command one iteration at a time
								break;

							case (0x10 | tmSomethingReadIn | tmHexInput):	// process a specific parameter hexadecimal value input
							case (0x10 | tmSomethingReadIn):				// process a specific parameter decimal value
								if (prgmPtr) parameterEdit::onEEPROMchange(prgmPtr, terminalAddress);
								terminalAddress = 0;
								terminalMode = tmHexInput; // go parse a generic hexadecimal digit
								terminalState = nextTerminalState;
								break;

							case (0x0D | tmSomethingReadIn | tmHexInput):	// process a specific main program variable hexadecimal value input
							case (0x14 | tmSomethingReadIn | tmHexInput):	// process a specific trip variable measurement hexadecimal value input
							case (0x16 | tmSomethingReadIn | tmHexInput):	// process a specific volatile variable hexadecimal value input
							case (0x0D | tmSomethingReadIn):				// process a specific main program variable decimal value
							case (0x14 | tmSomethingReadIn):				// process a specific trip variable measurement decimal value
							case (0x16 | tmSomethingReadIn):				// process a specific volatile variable decimal value
								if (prgmPtr) SWEET64::runPrgm(prgmPtr, terminalAddress);
							default:
								terminalAddress = 0;
								terminalMode = tmHexInput; // go parse a generic hexadecimal digit
								terminalState = nextTerminalState;
								break;

						}
						break;

					default:    // unsupported command
						chr = ' ';
						terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character
						break;

				}

				if (chr == '>')
				{

					primaryFunc = 0;
					extraFunc = 0;
					maxLine = 0;
					prgmPtr = 0;
					labelList = 0;
					switch (terminalMode)
					{

#ifdef useSWEET64trace
						case 0x0C:	// generate SWEET64 source code for a given function
							terminalMode = 0x1B;
#endif // useSWEET64trace
						case 'L':   // list available trip functions from terminalIdx
							maxLine = dfMaxValDisplayCount;
#if (useDebugTerminalLabels) || (useSWEET64trace)
							labelList = terminalTripFuncNames;
#endif // useDebugTerminalLabels
							primaryFunc = terminal::outputTripFunctionValue;
							break;

						case 'M':   // list available main program variables
							maxLine = mpVariableMaxIdx;
#ifdef useDebugTerminalLabels
							labelList = terminalMainProgramVarLabels;
#endif // useDebugTerminalLabels
							primaryFunc = terminal::outputMainProgramValue;
							prgmPtr = prgmWriteMainProgramValue;
							break;

						case 'O':   // list available program constants
							maxLine = idxMaxConstant;
#ifdef useDebugTerminalLabels
							labelList = terminalConstIdxNames;
#endif // useDebugTerminalLabels
							primaryFunc = terminal::outputConstantValue;
							break;

						case 'P':   // list available stored parameters
							maxLine = eePtrStorageEnd;
#ifdef useDebugTerminalLabels
							labelList = parmLabels;
#endif // useDebugTerminalLabels
							primaryFunc = terminal::outputParameterValue;
							extraFunc = terminal::outputParameterExtra;
							prgmPtr = prgmTerminalWriteParameterValue;
							break;

						case 'T':   // list available trip variable measurements
							maxLine = rvMeasuredCount;
#ifdef useDebugTerminalLabels
							labelList = terminalTripVarLabels;
#endif // useDebugTerminalLabels
							primaryFunc = terminal::outputTripVarMeasuredValue;
							extraFunc = terminal::outputTripVarMeasuredExtra;
							prgmPtr = prgmWriteTripMeasurementValue;
							break;

						case 'V':   // list available volatile variables
							maxLine = vVariableMaxIdx;
#ifdef useDebugTerminalLabels
							labelList = terminalVolatileVarLabels;
#endif // useDebugTerminalLabels
							primaryFunc = terminal::outputVolatileValue;
							prgmPtr = prgmWriteVolatileValue;
							break;

						default:
							break;

					}

					if (i & tmSomethingReadIn) // examine old terminal mode to see if a hex byte was read in
					{

						if (terminalByte < maxLine)
						{

							switch (i)
							{

								case (tmSomethingReadIn | tmHexInput):
								case (tmSomethingReadIn):
									terminalLine = terminalByte; // if a hex byte was read in and is less than maxLine, use it
									maxLine = terminalByte; // only output one line

									if (prgmPtr) // if this command could result in storing a value instead
									{

										terminalMode &= 0x1F; // go parse a decimal or hex value for possible saving
										terminalAddress = terminalByte; // save for possible saving

									}
									break;

								case ('.' | tmSomethingReadIn | tmHexInput):
								case ('.' | tmSomethingReadIn):
									if (terminalAddress < terminalByte) terminalLine = terminalAddress;
									else terminalLine = terminalByte;

									maxLine = terminalByte + 1;
									prgmPtr = 0;
									break;

								default:
									break;

							}

						}
						else // otherwise
						{

							text::stringOut(devDebugTerminal, PSTR("input value too large\r"));
							terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

						}

					}
					else
					{

						prgmPtr = 0; // no address was specified, so do not store anything
						switch (i)
						{

							case ('.' | tmHexInput):
							case '.':
								if (terminalAddress < maxLine) terminalLine = terminalAddress;
								else
								{

									text::stringOut(devDebugTerminal, PSTR("input value too large\r"));
									terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

								}
								break;

							default:
								terminalLine = 0;
								break;

						}

					}

				}

			}
			break;

		case 12:	// print a line of help
			text::stringOut(devDebugTerminal, terminalHelp, terminalLine++);
			if (pgm_read_byte(findStr(terminalHelp, terminalLine)) == 0) terminalState = nextTerminalState;
			break;

		case 14:	// wait for injected buttonpress to be accepted into timer0
			if (timer0Command & t0cProcessButton) break;
			terminalState++;
			input::inject(buttonsUp); // inject a buttons-up press into timer0
			break;

		case 15:	// wait for injected buttonpress to be accepted into timer0
			if (timer0Command & t0cProcessButton) break;
			terminalState = nextTerminalState;
			break;

		case 20:
			if (terminalMode & 0x80) // if a hex byte was read in
			{

				if (terminalByte >= tripSlotTotalCount)
				{

					text::stringOut(devDebugTerminal, PSTR("Invalid trip variable specified\r"));
					terminalState = 0;
					break;

				}
				else
				{

					oldSREG = SREG; // save interrupt flag status
					cli(); // disable interrupts to make the next operation atomic

					termRawIdx = terminalByte;
					SWEET64::runPrgm(prgmLoadTrip, terminalByte); // this allows direct loading/saving of trips to EEPROM
					SWEET64::runPrgm(prgmSaveTrip, terminalLine);

					SREG = oldSREG; // restore interrupt flag status

					text::stringOut(devDebugTerminal, PSTR("transferred "));
					text::stringOut(devDebugTerminal, terminalTripVarNames, terminalByte);
					text::stringOut(devDebugTerminal, PSTR(" to "));
					text::stringOut(devDebugTerminal, terminalTripVarNames, terminalLine);

					terminalState = nextTerminalState; // go fetch next command
					terminalMode = tmHexInput; // shift to reading hex words
					break;

				}

			}
			terminalState = 32;
			terminalMode = tmHexInput; // shift to reading hex words
			terminalLine = 0;
			maxLine = tripSlotTotalCount;
#ifdef useDebugTerminalLabels
			labelList = terminalTripVarNames;
#else // useDebugTerminalLabels
			labelList = 0;
#endif // useDebugTerminalLabels
			primaryFunc = 0;
			extraFunc = 0;

		case 32:	// output list of selected items
			text::hexByteOut(devDebugTerminal, terminalLine);
			text::stringOut(devDebugTerminal, PSTR(": "));
			if (primaryFunc)
			{

				primaryFunc(terminalLine);
				text::stringOut(devDebugTerminal, PSTR(" - "));

			}
			if (extraFunc)
			{

				extraFunc(terminalLine);
				text::stringOut(devDebugTerminal, PSTR(" - "));

			}
			if (labelList) text::stringOut(devDebugTerminal, labelList, terminalLine);
			else text::newLine(devDebugTerminal);

			terminalLine++;

			if (terminalLine >= maxLine) terminalState = nextTerminalState; // go back to read another command
			break;

		default:
			terminalState = 0;
			break;

	}

}

#endif // useDebugTerminal
#ifdef useSimulatedFIandVSS
static uint8_t debugReading::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(debugScreenFuncNames, cursorPos); // briefly display screen name
			retVal = (debugFlags & debugEnableFlags);
			switch (cursorPos)
			{

				case 0:
					peripheral::changeBitFlags(debugFlags, 0, (debugInjectorFlag | debugVSSflag));
					break;

				case 1:
					peripheral::changeBitFlags(debugFlags, debugInjectorFlag, debugVSSflag);
					break;

				case 2:
					peripheral::changeBitFlags(debugFlags, (debugInjectorFlag | debugVSSflag), 0);
					break;

				case 3:
					peripheral::changeBitFlags(debugFlags, debugVSSflag, debugInjectorFlag);
					break;

				default:
					break;

			}
			if ((debugFlags & debugEnableFlags) ^ retVal) configurePorts();

		case menuOutputDisplayIdx:
#ifdef useSpiffyTripLabels
			displayMainScreenFunctions(debugScreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
			displayMainScreenFunctions(debugScreenFormats, 0);
#endif // useSpiffyTripLabels
			break;

		default:
			break;

	}

	return retVal;

}

static void debugReading::configurePorts(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// configure VSS pin for either normal operation input or debug output
	if (debugFlags & debugVSSflag)
	{

#if defined(__AVR_ATmega32U4__)
		DDRB |= (1 << DDB7); // enable VSS sense pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRK |= (1 << DDK0); // enable VSS sense pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRC |= (1 << DDC0); // enable VSS sense pin interrupt
#endif // defined(__AVR_ATmega328P__)

		debugVSStickLength = 0;

		debugVSScount = 0;

		debugVSSidx = 0;

		debugVSSstate = 0; // start out by ramping up from 0 MPH to 250 MPH

		debugFlags &= ~(debugVSSready); // reset VSS ready bit

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
		DDRB &= ~(1 << DDB7); // disable VSS sense pin interrupt
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRK &= ~(1 << DDK0); // disable VSS sense pin interrupt
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRC &= ~(1 << DDC0); // disable VSS sense pin interrupt
#endif // defined(__AVR_ATmega328P__)

	}

	// configure fuel injector pins for either normal operation input or debug output
	if (debugFlags & debugInjectorFlag) // configure injector sense pins as outputs
	{

#if defined(__AVR_ATmega32U4__)
		DDRD |= ((1 << DDD3) | (1 << DDD2)); // enable injector sense pin interrupts
		PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRE |= ((1 << DDE4) | (1 << DDE5)); // enable injector sense pin interrupts
		PORTE |= ((1 << PORTE4) | (1 << PORTE5)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRD |= ((1 << DDD3) | (1 << DDD2)); // enable injector sense pin interrupts
		PORTD |= ((1 << PORTD3) | (1 << PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega328P__)

		debugFIPtickLength = 0;
		debugFIPWtickLength = 0;
		debugFIPWgoodTickLength = 0;
		debugFIPWreadTickLength = 0;

		debugFIPcount = 0;
		debugFIPWcount = 0;

		debugFIPidx = 0;

		debugFIPstate = 1; // start out by waiting before ramping up from 0 RPM to 12000 RPM

		debugFlags &= ~(debugFIPready); // reset pulse and pulse width ready bits

	}
	else // configure injector sense pins as inputs
	{

#if defined(__AVR_ATmega32U4__)
		DDRD &= ~((1 << DDD3) | (1 << DDD2)); // disable injector sense pin interrupts
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRE &= ~((1 << DDE4) | (1 << DDE5)); // disable injector sense pin interrupts
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRD &= ~((1 << DDD3) | (1 << DDD2)); // disable injector sense pin interrupts
#endif // defined(__AVR_ATmega328P__)

	}

	if (debugFlags & debugEnableFlags) timer1Command |= (t1cEnableDebug);
	else timer1Command &= ~(t1cEnableDebug);

	SREG = oldSREG; // restore state of interrupt flag

}

static void debugReading::idleProcess(void)
{

	if (timer1Status & t1sDebugUpdateFIP) // if debug fuel injector pulse period needs adjustment
	{

		peripheral::changeBitFlags(timer1Status, t1sDebugUpdateFIP, 0); // acknowledge debug update request

		debugFIPidx++;
		if (debugFIPidx >= debugFIPlength)
		{

			debugFIPidx = 0;
			debugFIPstate++;
			debugFIPstate &= 3;

		}

		switch (debugFIPstate)
		{

			case 0:
				debugFIPtickLength = pgm_read_word(&debugFIPvalues[(unsigned int)(debugFIPidx)]);
				debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(unsigned int)(debugFIPidx)]);
				debugFIPWgoodTickLength = debugFIPtickLength - 63;
				peripheral::changeBitFlags(debugFlags, 0, debugFIPready);
				break;

			case 1:
				break;

			case 2:
				debugFIPtickLength = pgm_read_word(&debugFIPvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
				debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
				debugFIPWgoodTickLength = debugFIPtickLength - 63;
				peripheral::changeBitFlags(debugFlags, 0, debugFIPready);
				break;

			case 3:
				break;

			default:
				break;

		}

		if (debugFIPWreadTickLength > debugFIPWgoodTickLength) debugFIPWreadTickLength = debugFIPWgoodTickLength;
		else debugFIPWtickLength = debugFIPWreadTickLength;

	}

	if (timer1Status & t1sDebugUpdateVSS) // if VSS pulse period needs adjustment
	{

		peripheral::changeBitFlags(timer1Status, t1sDebugUpdateVSS, 0); // acknowledge debug update request

		debugVSSidx++;
		if (debugVSSidx >= debugVSSlength)
		{

			debugVSSidx = 0;
			debugVSSstate++;
			debugVSSstate &= 3;

		}

		switch (debugVSSstate)
		{

			case 0:
				debugVSStickLength = pgm_read_word(&debugVSSvalues[(unsigned int)(debugVSSidx)]);
				peripheral::changeBitFlags(debugFlags, 0, debugVSSready);
				break;

			case 1:
				break;

			case 2:
				debugVSStickLength = pgm_read_word(&debugVSSvalues[(unsigned int)(debugVSSlength - debugVSSidx - 1)]);
				peripheral::changeBitFlags(debugFlags, 0, debugVSSready);
				break;

			case 3:
				break;

			default:
				break;

		}

	}

}

#endif // useSimulatedFIandVSS
