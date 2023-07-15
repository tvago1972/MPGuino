#if defined(useCPUreading)
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

static void systemInfo::idleProcess(void)
{

	uint8_t oldSREG;

	mainProgramVariables[(uint16_t)(mpMainLoopAccumulatorIdx)] = systemInfo::findCycleLength(mainStart, systemInfo::cycles0());
	mainProgramVariables[(uint16_t)(mpIdleAccumulatorIdx)] = idleTimerLength;

	mainStart = systemInfo::cycles0();
	idleTimerLength = 0;

#if defined(useDebugCPUreading)
	switch (monitorState)
	{

		case 1:
			idleProcessTimerLength = 0;
			displayTimerLength = 0;
			SWEET64timerLength = 0;

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] = 0;

			SREG = oldSREG; // restore interrupt flag status

			monitorState = 2;
			break;

		case 2:
			mainProgramVariables[(uint16_t)(mpDebugAccMainLoopIdx)] = mainProgramVariables[(uint16_t)(mpMainLoopAccumulatorIdx)];
			mainProgramVariables[(uint16_t)(mpDebugAccIdleIdx)] = mainProgramVariables[(uint16_t)(mpIdleAccumulatorIdx)];
			mainProgramVariables[(uint16_t)(mpDebugAccIdleProcessIdx)] = idleProcessTimerLength;
			mainProgramVariables[(uint16_t)(mpDebugAccDisplayIdx)] = displayTimerLength;
			mainProgramVariables[(uint16_t)(mpDebugAccSWEET64idx)] = SWEET64timerLength;

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			mainProgramVariables[(uint16_t)(mpDebugAccInterruptIdx)] = volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)];

			SREG = oldSREG; // restore interrupt flag status

			monitorState = 0;
			break;

		default:
			monitorState = 0;
			break;

	}

#endif // defined(useDebugCPUreading)
}

static void systemInfo::displayHandler (uint8_t cmd, uint8_t cursorPos)
{

	uint16_t availableRAMptr;

	if((unsigned int)__brkval == 0) availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)&__bss_end);
	else availableRAMptr = ((unsigned int)&availableRAMptr) - ((unsigned int)__brkval);

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
		case displayOutputIdx: // display max cpu utilization and RAM
			showCPUload();
#if LCDcharWidth == 20
			text::stringOut(devLCD, PSTR("     T"));
#else // LCDcharWidth == 20
			text::stringOut(devLCD, PSTR(" T"));
#endif // LCDcharWidth == 20
			text::stringOut(devLCD, ull2str(pBuff, vSystemCycleIdx, tReadTicksToSeconds)); // output system time (since MPGuino was powered up)

			text::gotoXY(devLCD, 0, 1);
#if LCDcharWidth == 20
			text::stringOut(devLCD, PSTR("  FREE RAM: "));
#else // LCDcharWidth == 20
			text::stringOut(devLCD, PSTR("FREE RAM: "));
#endif // LCDcharWidth == 20
			mainProgramVariables[(uint16_t)(mpAvailableRAMidx)] = availableRAMptr;
			SWEET64::runPrgm(prgmOutputAvailableRAM, 0);
			text::stringOut(devLCD, ull2str(pBuff, 0, (LCDcharWidth / 2) - 2, 0));
			break;

		default:
			break;

	}

}

static void systemInfo::showCPUload(void)
{

	text::stringOut(devLCD, PSTR("C%"));
	SWEET64::runPrgm(prgmFindCPUutilPercent, 0);
	text::stringOut(devLCD, ull2str(pBuff, 2, 6, 0));
#if defined(useDebugCPUreading)
	monitorState = 1;
#endif // defined(useDebugCPUreading)

}

static void systemInfo::showCPUloading(void)
{

	text::initStatus(devLCD);
	showCPUload();
	text::commitStatus(devLCD);

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

#endif // defined(useCPUreading)
#if defined(useActivityLED)
/* Activity status LED support section */

static void activityLED::init(void)
{

#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
//	DDRB |= (1 << DDB0); // turn on digital output for RX LED
	DDRC |= (1 << DDC7); // turn on digital output for LED L
//	DDRD |= (1 << DDD5); // turn on digital output for TX LED
#else // defined(useTinkerkitLCDmodule)
// insert any other ATmega32U4 port information for initializing status LEDs here
#endif // defined(useTinkerkitLCDmodule)
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
	DDRB |= (1 << DDB7); // turn on digital output for LED L
#else // defined(useArduinoMega2560)
// insert any other ATmega2560 port information for initializing status LEDs here
#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB |= (1 << DDB5); // turn on digital output for LED L
#endif // defined(__AVR_ATmega328P__)

	output(1); // initially turn on status LED

}

static void activityLED::shutdown(void)
{

	output(0); // turn off status LED

#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
//	DDRB &= ~(1 << DDB0); // turn off digital output for RX LED
	DDRC &= ~(1 << DDC7); // turn off digital output for LED L
//	DDRD &= ~(1 << DDD5); // turn off digital output for TX LED
#else // defined(useTinkerkitLCDmodule)
// insert any other ATmega32U4 port information for turning off status LEDs here
#endif // defined(useTinkerkitLCDmodule)
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
	DDRB &= ~(1 << DDB7); // turn off digital output for LED L
#else // defined(useArduinoMega2560)
// insert any other ATmega2560 port information for turning off status LEDs here
#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB &= ~(1 << DDB5); // turn off digital output for LED L
#endif // defined(__AVR_ATmega328P__)

}

static void activityLED::output(uint8_t val)
{

	if (val)
	{

#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
//		PORTB &= ~(1 << PORTB0); // active low RX
		PORTC |= (1 << PORTC7); // active high L
//		PORTD &= ~(1 << PORTD5); // active low TX
#else // defined(useTinkerkitLCDmodule)
// insert any other ATmega32U4 port information for turning on status LEDs here
#endif // defined(useTinkerkitLCDmodule)
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
		PORTB |= (1 << PORTB7); // active high L
#else // defined(useArduinoMega2560)
// insert any other ATmega2560 port information for turning on status LEDs here
#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB |= (1 << PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
//		PORTB |= (1 << PORTB0); // active low RX
		PORTC &= ~(1 << PORTC7); // active high L
//		PORTD |= (1 << PORTD5); // active low TX
#else // defined(useTinkerkitLCDmodule)
// insert any other ATmega32U4 port information for turning off status LEDs here
#endif // defined(useTinkerkitLCDmodule)
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
		PORTB &= ~(1 << PORTB7); // active high L
#else // defined(useArduinoMega2560)
// insert any other ATmega2560 port information for turning off status LEDs here
#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB &= ~(1 << PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}

}

#endif // defined(useActivityLED)
#if defined(useTestButtonValues)
/* Button input value viewer section */

static void buttonView::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
		case displayOutputIdx:
#if defined(useAnalogButtons)
			text::hexWordOut(devLCD, (uint16_t)(analogValue[(unsigned int)(analogButtonChannelIdx)]));
#endif // defined(useAnalogButtons)
			text::newLine(devLCD);
			text::hexByteOut(devLCD, thisButtonState);
			text::hexByteOut(devLCD, lastButtonState);
#if defined(useTestAnalogButtonIdx)
			text::stringOut(devLCD, PSTR(" pins, "));
			text::hexByteOut(devLCD, thisButtonIdx);
			text::stringOut(devLCD, PSTR(" idx"));
#else // defined(useTestAnalogButtonIdx)
			text::stringOut(devLCD, PSTR(" pins"));
			text::newLine(devLCD);
#endif // defined(useTestAnalogButtonIdx)
			break;

		default:
			break;

	}

}

#endif // defined(useTestButtonValues)
#if defined(useDebugTerminal)
/* debug terminal section */

static const uint8_t prgmParseDecimalDigit[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 10,								// multiply by 10
	instrJump, tParseCharacterToReg						// go perform number parse
};

static const uint8_t prgmParseHexDigit[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 16,								// multiply by 16
	instrJump, tParseCharacterToReg						// go perform number parse
};

static const uint8_t prgmFetchMainProgramValue[] PROGMEM = {
	instrLdRegMainIndexed, 0x02,
	instrDone											// exit to caller
};

static const uint8_t prgmFetchConstantValue[] PROGMEM = {
	instrLdRegConstIndexed, 0x02,
	instrDone											// exit to caller
};

static const uint8_t prgmFetchVolatileValue[] PROGMEM = {
	instrLdRegVolatileIndexed, 0x02,
	instrDone											// exit to caller
};

static const uint8_t prgmUpdateDecimalValue[] PROGMEM = {
	instrTestIndex,										// test line number
	instrBranchIfNotE, 3,								// skip if line number is not zero
	instrLdRegByte, 0x06, 0,							// initialize terminal register

//cont:
	instrLdReg, 0x62,									// load terminal register into main register
	instrMul2byByte, 10,								// multiply by 10
	instrAddByteToX, 0x02, 5,							// add 5
	instrLdReg, 0x26,									// load main register into terminal register
	instrDone											// exit to caller
};

static const uint8_t prgmPerformMathOperation[] PROGMEM = {
#if defined(useIsqrt)
	instrCmpIndex, '_',									// if math operation is '_', do square root
	instrBranchIfE, 47,
#endif // defined(useIsqrt)
	instrTestIndex,										// if math operation is null, do assignment
	instrBranchIfE, 27,
	instrCmpIndex, '=',									// if math operation is '=', do assignment
	instrBranchIfE, 23,
	instrCmpIndex, '+',									// if math operation is '+', do addition
	instrBranchIfE, 22,
	instrCmpIndex, '-',									// if math operation is '-', do subtraction
	instrBranchIfE, 21,
	instrLdReg, 0x61,									// load terminal register into register 1
	instrLdReg, 0x72,									// load math register into register 2
	instrCmpIndex, '*',									// if math operation is '*', do multiplication
	instrBranchIfE, 16,
	instrCmpIndex, '/',									// if math operation is '/', do division
	instrBranchIfE, 16,
	instrLdReg, 0x67,									// assign decimal value to result register
	instrDone,											// exit to caller

//assignment:
	instrLdReg, 0x67,									// assign decimal value to result register
	instrDone,											// exit to caller

//addition:
	instrAddYtoX, 0x67,									// perform addition
	instrDone,											// exit to caller

//subtraction:
	instrSubYfromX, 0x67,								// perform subtraction
	instrDone,											// exit to caller

//multiplication:
	instrMul2by1,										// perform multiplication
	instrLdReg, 0x27,									// save math result
	instrDone,											// exit to caller

//division:
	instrDiv2by1,										// perform division
	instrLdReg, 0x27,									// save math result
#if defined(useIsqrt)
	instrDone,											// exit to caller

//sqrt:
	instrLdReg, 0x72,									// load terminal register into register 2
	instrIsqrt,	0x02,									// perform square root function
	instrLdReg, 0x27,									// save math result
#endif // defined(useIsqrt)
	instrDone											// exit to caller
};

static const uint8_t prgmFetchResultValue[] PROGMEM = {
	instrLdReg, 0x72,									// load terminal register into math register
	instrDone											// exit to caller
};

static const uint8_t prgmFetchDecimalValue[] PROGMEM = {
	instrLdReg, 0x62,									// load terminal register into main register
	instrDone											// exit to caller
};

static const uint8_t prgmFetchTripVarValue[] PROGMEM = {
	instrLdRegTripVarIndexedRV, 0x02, terminalIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmWriteMainProgramValue[] PROGMEM = {
	instrStRegMainIndexed, 0x06,
	instrDone											// exit to caller
};

static const uint8_t prgmWriteVolatileValue[] PROGMEM = {
	instrStRegVolatileIndexed, 0x06,
	instrDone											// exit to caller
};

static const uint8_t prgmWriteTripMeasurementValue[] PROGMEM = {
	instrStRegTripVarIndexedRV, 0x06, terminalIdx,
	instrDone											// exit to caller
};

#if defined(useDebugButtonInjection)
#if defined(useLegacyButtons)
static const uint8_t terminalButtonCount = 6;

static const char terminalButtonChars[] PROGMEM = {
	"lcrLCR" tcEOS
};

static const uint8_t terminalButtonValues[] PROGMEM = {
	 btnShortPressL
	,btnShortPressC
	,btnShortPressR
	,btnLongPressL
	,btnLongPressC
	,btnLongPressR
};

#else // defined(useLegacyButtons)
static const uint8_t terminalButtonCount = 10;

static const char terminalButtonChars[] PROGMEM = {
	"lcrudLCRUD" tcEOS
};

static const uint8_t terminalButtonValues[] PROGMEM = {
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

#endif // defined(useLegacyButtons)
#endif // defined(useDebugButtonInjection)
static void terminal::outputFlags(uint8_t flagRegister, const char * flagStr)
{

	uint8_t bitMask;
	uint8_t c;

	bitMask = 0x80;

	text::stringOut(devDebugTerminal, flagStr);

	for (uint8_t x = 1; x < 9; x++)
	{

		text::stringOutIf(devDebugTerminal, (flagRegister & bitMask), flagStr, x);

		if (x == 8) c = 0x0D;
		else c = 0x20;
		text::charOut(devDebugTerminal, c);

		bitMask >>= 1;

	}

}

static void terminal::outputTripFunctionValue(uint8_t lineNumber)
{

	calcFuncObj thisCalcFuncObj;

	// perform the required decimal formatting
	thisCalcFuncObj = translateCalcIdx(terminalIdx, lineNumber, termNumberBuff, decWindow, 0);

	text::stringOut(devDebugTerminal, thisCalcFuncObj.strBuffer); // output the number
	text::charOut(devDebugTerminal, ' ');
	text::stringOut(devDebugTerminal, terminalFormats, thisCalcFuncObj.calcFmtIdx);

}

static void terminal::outputConstantValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchConstantValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(termNumberBuff, 0, tFormatToNumber));

}

static void terminal::outputParameterValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(termNumberBuff, 0, tFormatToNumber));

}

static void terminal::outputParameterExtra(uint8_t lineNumber)
{

	uint8_t i;

	i = EEPROM::getLength(lineNumber);

	text::hexByteOut(devDebugTerminal, EEPROM::getParameterFlags(lineNumber));
	text::charOut(devDebugTerminal, '-');
	text::hexByteOut(devDebugTerminal, i);
	text::charOut(devDebugTerminal, '-');
	text::hexWordOut(devDebugTerminal, EEPROM::getAddress(lineNumber));
	if (lineNumber < eePtrSettingsEnd)
	{

		text::stringOut(devDebugTerminal, PSTR(" (orig "));
		SWEET64::runPrgm(prgmFetchInitialParamValue, lineNumber);
		text::stringOut(devDebugTerminal, ull2str(termNumberBuff, 0, tFormatToNumber));
		text::stringOut(devDebugTerminal, PSTR(")"));

	}
	else if (lineNumber < eePtrEnd)
	{

		if (i & 0x07) i += 0x08;
		i >>= 3;

		text::charOut(devDebugTerminal, ' ');
		SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);

		for (uint8_t x = 7; x < 8; x--)
			if (x < i) text::hexByteOut(devDebugTerminal, ((union union_64 *)(&s64reg[(uint16_t)(s64reg2)]))->u8[(uint16_t)(x)]);
			else text::stringOut(devDebugTerminal, PSTR("  "));

		text::charOut(devDebugTerminal, ' ');

	}

}

static void terminal::outputVolatileValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchVolatileValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(termNumberBuff, 0, tFormatToNumber));

}

static void terminal::outputMainProgramValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchMainProgramValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(termNumberBuff, 0, tFormatToNumber));

}

static void terminal::outputTripVarMeasuredValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(termNumberBuff, 0, tFormatToNumber));

}

static void terminal::outputTripVarMeasuredExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::hexLWordOut(devDebugTerminal, &s64reg[s64reg2]);

}

static void terminal::outputDecimalValue(uint8_t lineNumber)
{

	if (lineNumber == 0)
	{

		text::hexByteOut(devDebugTerminal, decMode);
		text::charOut(devDebugTerminal, ' ');
		text::hexByteOut(devDebugTerminal, decWindow);
		text::charOut(devDebugTerminal, ' ');
		text::hexByteOut(devDebugTerminal, decPlace);
		text::stringOut(devDebugTerminal, PSTR(tcCR "    "));

	}

	SWEET64::runPrgm(prgmUpdateDecimalValue, lineNumber);
	text::hexLWordOut(devDebugTerminal, &s64reg[s64reg2]);

}

static void terminal::outputDecimalExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchDecimalValue, lineNumber);
	text::stringOut(devDebugTerminal, ull2str(termNumberBuff, decPlace, decWindow, decMode));

}

static void terminal::processMath(uint8_t cmd)
{

	if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it
	if (terminalMode & tmSourceReadIn) decPlace = terminalSource; // if decimal count specified, save it
	// save terminal register contents for later
	if ((terminalMode & tmByteReadIn) || (cmd == '_')) SWEET64::runPrgm(prgmPerformMathOperation, cmd);

}

static void terminal::mainProcess(void)
{

	uint8_t i;
	uint8_t j;
	uint8_t oldSREG;

/*
the debug terminal is based off of the Apple II system monitor, which is command-line based. One or more commands are
entered at the prompt, separated by space characters. Pressing <Enter> will cause the monitor to execute these commands

	terminal commands:
    [y].[x]P - list stored parameters, optionally between [y] and [x]
    [y].[x]V - list volatile variables, optionally between [y] and [x]
    [y].[x]M - list main program variables, optionally between [y] and [x]
    [y].[x]T - list terminal trip variable values, optionally between [y] and [x]
    [y].[x]O - list program constants, optionally between [y] and [x]
[z]<[y].[x]L - list terminal trip variable function outputs, optionally between [y] and [x]
                [z] - decimal window length (optional)
[z]<[y].[x]U - list decimal number sample for output
                [z] - decimal window length (optional)
                [y] - decimal digit count (optional)
                [x] - decimal processing flag (optional)
         x^L - list SWEET64 source code for trip function
   [z]<[y].x - enters a number x into the 64-bit math accumulator
                [z] - decimal window length (optional)
                [y] - decimal digit count (optional)
          +x - adds x to math accumulator
          -x - subtracts x from math accumulator
          *x - multiplies math accumulator by x
          /x - divides math accumulator by x
          |  - finds square root of math accumulator
          =x - enters a number x into the 64-bit math accumulator

  x:Py [y] [y]... - store one or more y values, starting at stored parameter x
  x:Vy [y] [y]... - store one or more y values, starting at volatile variable x
  x:My [y] [y]... - store one or more y values, starting at main program variable x
  x:Ty [y] [y]... - store one or more y values, starting at terminal trip variable x

    [y]<[x]R - read trip variable x into trip variable y
                default for x and y is terminal trip variable
                if no x or y specified, lists available trip variables
           I - inject button press
                short (l, c, r, u, d)
                 long (L, C, R, U, D)
           ? - displays this help

	numbers or button presses are separated by spaces

	ex:
	]1r           << reads instant trip variable into terminal trip variable

	]1.3l         << outputs functions 1, 2, and 3 associated with read-in trip variable

	]0.3p         << outputs stored parameters 0 through 3

	]1p2 66 3000  << stores 2 at stored parameter 1, 66 at stored parameter 2, and 3000 at stored parameter 3

	]l            << outputs all available functions associated with terminal trip variable

	]ild          << injects short button press l+d into MPGuino

	]ir r r r     << injects 4 successive short press r into MPGuino

	]irc R LC r   << injects short press r+c, long press r, long press l+c, short press r into MPGuino

	]300          << stores 768 into the 64-bit math accumulator

	]*f           << multiplies 64-bit math accumulator contents by 15

*/
	switch (terminalState)
	{

		case 0:		// initialize terminal state machine and output the prompt
			text::charOut(devDebugTerminal, 0x0D);
			text::charOut(devDebugTerminal, ']');
			terminalState++;
			inpIdx = 0;
			readIdx = 0;
			terminalCmd = 0;
			terminalMode = tmInitHex;
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

#if defined(useSWEET64trace)
					case 0x0C:
#endif // defined(useSWEET64trace)
					case 0x20 ... 0x7E:
						if (inpIdx == tBuffLength)
						{

							text::stringOut(devDebugTerminal, PSTR("\\" tcEOSCR));
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

			j = chr; // save raw input character for button press processing

			if (chr > 0x5F) chr &= 0x5F; // force input character to uppercase

			if (terminalMode & tmInitInput)
			{

				terminalMode &= ~(tmInitInput | tmByteReadIn);

				terminalByte = 0;
				SWEET64::init64byt((union union_64 *)(&s64reg[s64reg6]), 0);

			}

			switch (terminalMode & tmInputMask) // process a possible digit, hexit, or button press character
			{

#if defined(useDebugButtonInjection)
				case (tmButtonInput):					// parse a button press character
				case (tmButtonInput | tmByteReadIn):	// parse a button press character
					for (uint8_t x = 0; x < terminalButtonCount; x++)
						if (j == pgm_read_byte(&terminalButtonChars[(uint16_t)(x)]))
						{

							i = 0; // signal that a valid character was read in
							terminalByte |= (pgm_read_byte(&terminalButtonValues[(uint16_t)(x)]));
							terminalMode |= (tmByteReadIn);

						}
					break;

#endif // defined(useDebugButtonInjection)
				case (tmHexInput):						// parse a generic hexadecimal digit or switch to decimal mode
					if (chr == '\\')
					{

						terminalMode &= ~(tmHexInput); // clear hexadecimal input mode
						terminalMode |= (tmDecimalInput); // set decimal input mode
						i = 0; // signal that a valid character was read in
						break;

					}

				case (tmHexInput | tmByteReadIn):		// parse a generic hexadecimal digit
					switch (chr)
					{

						case 'A' ... 'F':
							chr -= 7;
						case '0' ... '9':
							chr -= 48;
							i = 0; // signal that a valid character was read in
							terminalMode |= (tmByteReadIn);
							terminalByte = SWEET64::runPrgm(prgmParseHexDigit, chr);
							break;

						default:
							break;

					}
					break;

				case (tmDecimalInput):					// parse a generic decimal digit or switch to hex mode
					if ((chr == 'X') || (chr == '$'))
					{

						terminalMode &= ~(tmDecimalInput); // clear decimal input mode
						terminalMode |= (tmHexInput); // set hexadecimal input mode
						i = 0; // signal that a valid character was read in
						break;

					}

				case (tmDecimalInput | tmByteReadIn):	// parse a generic decimal digit
					switch (chr)
					{

						case '0' ... '9':
							chr -= 48;
							i = 0; // signal that a valid character was read in
							terminalMode |= (tmByteReadIn);
							terminalByte = SWEET64::runPrgm(prgmParseDecimalDigit, chr);
							break;

						default:
							break;

					}

				default:								// nothing to parse - could be a possible command
					break;

			}

			if (i) // parsing wasn't valid, so check for a command
			{

				nextTerminalState = 2; // get another character from input buffer
				switch (chr)
				{

#if defined(useDebugTerminalHelp)
					case '?':   // display help
						terminalCmd = chr;
						terminalLine = 0; // initialize terminal output line
						terminalState = 12; // this command WILL print a lot of different lines, so handle this command one iteration at a time
						break;

#endif // defined(useDebugTerminalHelp)
					case '.':	// specify source address
						if (terminalMode & tmButtonInput) chr = '\\'; // if in button injection mode, reset input mode and pending command
						else
						{

							terminalSource = terminalByte; // save source start byte value
							terminalMode &= ~(tmInputMask); // clear input mode processing bits
							terminalMode |= (tmInitHex | tmSourceReadIn); // shift to hex input

						}

						break;

					case '<':	// specify target address
						if (terminalMode & tmButtonInput) chr = '\\'; // if in button injection mode, reset input mode and pending command
						else
						{

							terminalTarget = terminalByte; // save source start byte value
							terminalMode &= ~(tmInputMask); // clear input mode processing bits
							terminalMode |= (tmInitHex | tmTargetReadIn); // shift to hex input

						}

						break;

					case ':':   // specify storage address
						if (terminalMode & tmButtonInput) chr = '\\'; // if in button injection mode, reset input mode and pending command
						else
						{

							terminalCmd = chr; // save command for later
							terminalAddress = terminalByte; // save address byte value
							terminalMode &= ~(tmInputMask); // clear input mode processing bits
							terminalMode |= (tmInitHex | tmAddressReadIn); // shift to hex input

						}

						break;

					case 'I':   // inject button press
						terminalCmd = chr; // save command for later
						terminalMode = tmInitButton; // shift to reading button press words
						break;

					case '+':	// add
					case '-':	// subtract
					case '*':	// multiply
					case '/':	// divide
#if defined(useIsqrt)
					case '_':	// square root
#endif // defined(useIsqrt)
					case '=':	// output last result
						if (terminalMode & tmButtonInput) chr = '\\'; // if in button injection mode, reset input mode and pending command
						else
						{

							processMath(terminalCmd);

							terminalCmd = chr; // save command for later
							terminalMode |= (tmInitInput); // shift to reading a new numeric value

						}

						break;

#if defined(useSWEET64trace)
					case 0x0C:
						chr = '\\'; // reset input mode and pending command
						break;

#endif // defined(useSWEET64trace)
					case 'L':   // list available trip functions from terminalIdx
						if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it
					case 'O':	// list available program constants
						terminalState = 32; // this command could print a lot of different lines, so handle this command one iteration at a time
						terminalCmd = chr; // save command for later
						chr = '$'; // do unified list output preparation
						break;

					case 'U':	// output a sample list of decimal numbers
						if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it
						if (terminalMode & tmSourceReadIn) decPlace = terminalSource; // if decimal count specified, save it
						if (terminalMode & tmByteReadIn) decMode = terminalByte; // if decimal mode specified, save it

						terminalState = 32; // this command could print a lot of different lines, so handle this command one iteration at a time
						terminalCmd = chr; // save command for later
						chr = '$'; // do unified list output preparation
						break;

					case 'M':   // list available main program variables
					case 'P':   // list available stored parameters
					case 'T':   // list available trip variable measurements
					case 'V':   // list available volatile variables
						if (terminalCmd != ':') terminalState = 32; // this command could print a lot of different lines, so handle this command one iteration at a time

						terminalCmd = chr; // save command for later
						chr = '$'; // do unified list output preparation
						break;

					case 'R':   // read trip variable into terminal
						if (terminalMode & tmButtonInput) chr = '\\'; // if in button injection mode, reset input mode and pending command
						else
						{

							// if no target was read in, assume terminal trip index
							if ((terminalMode & tmTargetReadIn) == 0) terminalTarget = terminalIdx;

							// if no source was read in, assume terminal trip index
							if ((terminalMode & tmByteReadIn) == 0) terminalByte = terminalIdx;

							if (terminalTarget != terminalByte) // if target and source are not the same
							{

								if (terminalTarget >= tripSlotTotalCount)
								{

									text::stringOut(devDebugTerminal, PSTR("Invalid target trip variable specified" tcEOSCR));
									terminalState = 0;

								}
								else if (terminalByte >= tripSlotTotalCount)
								{

									text::stringOut(devDebugTerminal, PSTR("Invalid source trip variable specified" tcEOSCR));
									terminalState = 0;

								}
								else
								{

									oldSREG = SREG; // save interrupt flag status
									cli(); // disable interrupts to make the next operations atomic

									SWEET64::runPrgm(prgmLoadTrip, terminalByte); // this allows direct loading/saving of trips to EEPROM
									SWEET64::runPrgm(prgmSaveTrip, terminalTarget);

									SREG = oldSREG; // restore interrupt flag status

									text::stringOut(devDebugTerminal, PSTR("transferred "));
									text::stringOut(devDebugTerminal, terminalTripVarNames, terminalByte);
									text::stringOut(devDebugTerminal, PSTR(" to "));
									text::stringOut(devDebugTerminal, terminalTripVarNames, terminalTarget);

									chr = '\\'; // reset input mode and pending command

								}

							}
							else // source and target trip variables are the same, output a list of available trip variables
							{

								terminalCmd = chr; // save command for later
								terminalState = 32; // this command could print a lot of different lines, so handle this command one iteration at a time
								chr = '$'; // do unified list output preparation

							}

						}

						break;

					case 0x0D:    // carriage return - treat as a special space
						nextTerminalState = 0; // when finished processing, go back to terminal state 0 - initialize input and print prompt character
					case ' ':   // space character
						i = 1;

						switch (terminalCmd)
						{

#if defined(useDebugButtonInjection)
							case 'I':   // inject button press
								if (terminalMode & tmByteReadIn)
								{

									button::inject(terminalByte); // inject the parsed button press value into timer0
									terminalState = 14;

									i = 0;

								}

								break;

#endif // defined(useDebugButtonInjection)
							case 'P':   // enter a stored parameter value
								if ((terminalMode & tmAddressReadIn) && (terminalMode & tmByteReadIn) && ((terminalMode & tmSourceReadIn) == 0) && (prgmPtr))
								{

									parameterEdit::onEEPROMchange(prgmPtr, terminalAddress++);
									terminalMode &= ~(tmInputMask); // clear input mode processing bits
									terminalMode |= (tmDecimalInput); // shift to reading a new decimal value

									i = 0;

								}

								break;

							case 'M':   // enter a main program variable value
							case 'T':   // enter a trip variable measurement value
							case 'V':   // enter a volatile variable value
								if ((terminalMode & tmAddressReadIn) && (terminalMode & tmByteReadIn) && ((terminalMode & tmSourceReadIn) == 0) && (prgmPtr))
								{

									SWEET64::runPrgm(prgmPtr, terminalAddress++);
									terminalMode &= ~(tmInputMask); // clear input mode processing bits
									terminalMode |= (tmDecimalInput); // shift to reading a new decimal value

									i = 0;

								}

								break;

							case 0:
								outputFlags(activityFlags, terminalActivityFlagStr);
#if defined(useBarFuelEconVsSpeed)

								text::stringOut(devDebugTerminal, PSTR("FEvSpdTripIdx = " tcEOS));
								text::hexByteOut(devDebugTerminal, FEvSpdTripIdx);
								text::newLine(devDebugTerminal);
#endif // defined(useBarFuelEconVsSpeed)
							case '+':	// add
							case '-':	// subtract
							case '*':	// multiply
							case '/':	// divide
#if defined(useIsqrt)
							case '_':	// square root
#endif // defined(useIsqrt)
							case '=':	// output last result
								processMath(terminalCmd);

								SWEET64::runPrgm(prgmFetchResultValue, 0);
								text::charOut(devDebugTerminal, '=');
								text::stringOut(devDebugTerminal, ull2str(termNumberBuff, decPlace, decWindow, decMode));
								text::stringOut(devDebugTerminal, PSTR(" (0x"));
								text::hexLWordOut(devDebugTerminal, &s64reg[s64reg7]);
								text::stringOut(devDebugTerminal, PSTR(")" tcCR));

								break;

							default:
								chr = '\\';
								break;

						}

						if (i) chr = '\\';
						else terminalMode |= (tmInitInput); // go parse another input value

						break;

					case '$': // modeshift from decimal input mode to hexadecimal input mode
					case 'X': // modeshift from decimal input mode to hexadecimal input mode
					case '\\': // modeshift from decimal input mode to hexadecimal input mode
					default:    // unsupported command
						terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character
						chr = ' ';
						break;

				}

				if (chr == '$') // unified list output preparation
				{

					terminalMode &= ~(tmInputMask); // clear input mode processing bits
					terminalMode |= (tmInitDecimal); // shift to decimal input

					primaryFunc = 0;
					extraFunc = 0;
					maxLine = 0;
					prgmPtr = 0;
					labelList = 0;
					terminalLine = 0;
					chr = 'X';

					switch (terminalCmd)
					{

						case 'L':   // list available trip functions from terminalIdx
							maxLine = dfMaxValDisplayCount;
#if defined(useDebugTerminalLabels) || defined(useSWEET64trace)
							labelList = terminalTripFuncNames;
#endif // defined(useDebugTerminalLabels) || defined(useSWEET64trace)
							primaryFunc = terminal::outputTripFunctionValue;
							break;

						case 'M':   // list available main program variables
							maxLine = mpVariableMaxIdx;
#if defined(useDebugTerminalLabels)
							labelList = terminalMainProgramVarLabels;
#endif // defined(useDebugTerminalLabels)
							primaryFunc = terminal::outputMainProgramValue;
							prgmPtr = prgmWriteMainProgramValue;
							break;

						case 'O':   // list available program constants
							maxLine = idxMaxConstant;
#if defined(useDebugTerminalLabels)
							labelList = terminalConstIdxNames;
#endif // defined(useDebugTerminalLabels)
							primaryFunc = terminal::outputConstantValue;
							break;

						case 'P':   // list available stored parameters
							maxLine = eePtrEnd;
#if defined(useDebugTerminalLabels)
							labelList = parmLabels;
#endif // defined(useDebugTerminalLabels)
							primaryFunc = terminal::outputParameterValue;
							extraFunc = terminal::outputParameterExtra;
							prgmPtr = prgmTerminalWriteParameterValue;
							break;

						case 'R':	// list available trip variables
							maxLine = tripSlotTotalCount;
#if defined(useDebugTerminalLabels)
							labelList = terminalTripVarNames;
#endif // defined(useDebugTerminalLabels)
							break;

						case 'T':   // list available trip variable measurements
							maxLine = rvMeasuredCount;
#if defined(useDebugTerminalLabels)
							labelList = terminalTripVarLabels;
#endif // defined(useDebugTerminalLabels)
							primaryFunc = terminal::outputTripVarMeasuredValue;
							extraFunc = terminal::outputTripVarMeasuredExtra;
							prgmPtr = prgmWriteTripMeasurementValue;
							break;

						case 'U':   // output a sample list of decimal numbers
							maxLine = 14;
							primaryFunc = terminal::outputDecimalValue;
							extraFunc = terminal::outputDecimalExtra;
							chr = ' ';
							break;

						case 'V':   // list available volatile variables
							maxLine = vVariableMaxIdx;
#if defined(useDebugTerminalLabels)
							labelList = terminalVolatileVarLabels;
#endif // defined(useDebugTerminalLabels)
							primaryFunc = terminal::outputVolatileValue;
							prgmPtr = prgmWriteVolatileValue;
							break;

						default:	// unrecognized listing command
							chr = '\\'; // reset input mode and pending command
							break;

					}

				}

				if (chr == 'X') // handle second part of list preparation
				{

					if (terminalMode & tmSourceReadIn)
					{

						if (terminalSource >= maxLine)
						{

							text::stringOut(devDebugTerminal, PSTR("index start value too large" tcEOSCR));
							terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

						}
						else
						{

							terminalLine = terminalSource;

							if (terminalMode & tmByteReadIn)
							{

								if (terminalByte >= maxLine)
								{

									text::stringOut(devDebugTerminal, PSTR("index end value too large" tcEOSCR));
									terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

								}
								else
								{

									maxLine = terminalByte + 1;

									if (terminalLine >= maxLine) maxLine = terminalLine + 1;

								}

							}

						}

					}
					else
					{

						if (terminalMode & tmByteReadIn)
						{

							if (terminalByte >= maxLine)
							{

								text::stringOut(devDebugTerminal, PSTR("index value too large" tcEOSCR));
								terminalState = 0; // go back to terminal state 0 - initialize input and print prompt character

							}
							else
							{

								terminalLine = terminalByte;
								maxLine = terminalByte + 1;

							}

						}
						else terminalLine = 0;

					}

				}

				if (chr == '\\') // reset pending commands, reset number input mode
				{

					terminalState = nextTerminalState; // go fetch next command
					terminalMode = tmInitHex; // clear all read-in byte addressing values, and shift to parsing a generic input value

				}

			}
			break;

#if defined(useDebugTerminalHelp)
		case 12:	// print a line of help
			text::stringOut(devDebugTerminal, terminalHelp, terminalLine++);
			if (pgm_read_byte(findStr(terminalHelp, terminalLine)) == 0) terminalState = nextTerminalState;
			break;

#endif // defined(useDebugTerminalHelp)
#if defined(useDebugButtonInjection)
		case 14:	// wait for injected buttonpress to be accepted into timer0
			if (timer0Command & t0cProcessButton) break;
			terminalState++;
			button::inject(buttonsUp); // inject a buttons-up press into timer0
			break;

		case 15:	// wait for injected buttonpress to be accepted into timer0
			if (timer0Command & t0cProcessButton) break;
			terminalState = nextTerminalState;
			break;

#endif // defined(useDebugButtonInjection)
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

#endif // defined(useDebugTerminal)
#if defined(useSimulatedFIandVSS)
static void signalSim::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t i;

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(devLCD, debugScreenFuncNames, cursorPos); // briefly display screen name
			i = (debugFlags & debugEnableFlags);
			switch (cursorPos)
			{

				case 0:
					changeBitFlags(debugFlags, 0, (debugInjectorFlag | debugVSSflag));
					break;

				case 1:
					changeBitFlags(debugFlags, debugInjectorFlag, debugVSSflag);
					break;

				case 2:
					changeBitFlags(debugFlags, (debugInjectorFlag | debugVSSflag), 0);
					break;

				case 3:
					changeBitFlags(debugFlags, debugVSSflag, debugInjectorFlag);
					break;

				default:
					break;

			}
			if ((debugFlags & debugEnableFlags) ^ i) configurePorts();

		case displayOutputIdx:
			mainDisplay::outputPage(getSignalSimPageFormats, 0, 136, 0);
			break;

		default:
			break;

	}

}

static uint16_t signalSim::getSignalSimPageFormats(uint8_t formatIdx)
{

	return pgm_read_word(&signalSimPageFormats[(uint16_t)(formatIdx)]);

}

static void signalSim::configurePorts(void)
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

static void signalSim::idleProcessFuel(void)
{

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
			changeBitFlags(debugFlags, 0, debugFIPready);
			break;

		case 1:
			break;

		case 2:
			debugFIPtickLength = pgm_read_word(&debugFIPvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
			debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(unsigned int)(debugFIPlength - debugFIPidx - 1)]);
			debugFIPWgoodTickLength = debugFIPtickLength - 63;
			changeBitFlags(debugFlags, 0, debugFIPready);
			break;

		case 3:
			break;

		default:
			break;

	}

	if (debugFIPWreadTickLength > debugFIPWgoodTickLength) debugFIPWreadTickLength = debugFIPWgoodTickLength;
	else debugFIPWtickLength = debugFIPWreadTickLength;

}

static void signalSim::idleProcessVSS(void)
{

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
			changeBitFlags(debugFlags, 0, debugVSSready);
			break;

		case 1:
			break;

		case 2:
			debugVSStickLength = pgm_read_word(&debugVSSvalues[(unsigned int)(debugVSSlength - debugVSSidx - 1)]);
			changeBitFlags(debugFlags, 0, debugVSSready);
			break;

		case 3:
			break;

		default:
			break;

	}

}

#endif // defined(useSimulatedFIandVSS)
