#if defined(useCPUreading) || defined(useDebugCPUreading)
/* CPU loading and RAM availability support section */

static void systemInfo::idleProcess(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	mainProgramVariables[(uint16_t)(mpCPUsampledMainLoopIdx)] = heart::findCycle0Length(mainProgramVariables[(uint16_t)(mpCPUworkingLoopStartIdx)]);
	mainProgramVariables[(uint16_t)(mpCPUworkingLoopStartIdx)] = heart::cycles0(); // record starting time

#if defined(useDebugCPUreading)
	if (bitFlags[(uint16_t)(bfPeek)] & peekEnableCPUread) mainProgramVariables[(uint16_t)(mpDbgSampledInterruptProcessIdx)] = volatileVariables[(uint16_t)(vWorkingInterruptProcessIdx)];
	volatileVariables[(uint16_t)(vWorkingInterruptProcessIdx)] = 0;

#endif // defined(useDebugCPUreading)
	SREG = oldSREG; // restore interrupt flag status

	mainProgramVariables[(uint16_t)(mpCPUsampledMainWorkingIdx)] = mainProgramVariables[(uint16_t)(mpCPUworkingMainWorkingIdx)];
	mainProgramVariables[(uint16_t)(mpCPUworkingMainWorkingIdx)] = 0;

	mainProgramVariables[(uint16_t)(mpCPUsampledIdleProcessIdx)] = mainProgramVariables[(uint16_t)(mpCPUworkingIdleProcessIdx)];
	mainProgramVariables[(uint16_t)(mpCPUworkingIdleProcessIdx)] = 0;

#if defined(useDebugCPUreading)
	transferResetCycle0Length(mpDbgSampledSleepModeIdx, mpDbgWorkingSleepModeIdx);
	transferResetCycle0Length(mpDbgSampledIdleSampleIdx, mpDbgWorkingIdleSampleIdx);
#if defined(useChryslerMAPCorrection) || defined(useChryslerBaroSensor) || defined(useAnalogButtons)
	transferResetCycle0Length(mpDbgSampledIdleAnalogIdx, mpDbgWorkingIdleAnalogIdx);
#endif // defined(useChryslerMAPCorrection) || defined(useChryslerBaroSensor) || defined(useAnalogButtons)
#if defined(useDragRaceFunction)
	transferResetCycle0Length(mpDbgSampledAccelTestIdx, mpDbgWorkingAccelTestIdx);
#endif // defined(useDragRaceFunction)
#if defined(useSimulatedFIandVSS)
	transferResetCycle0Length(mpDbgSampledSignalSimIdx, mpDbgWorkingSignalSimIdx);
#endif // defined(useSimulatedFIandVSS)
	transferResetCycle0Length(mpDbgSampledMainProcessIdx, mpDbgWorkingMainProcessIdx);
	transferResetCycle0Length(mpDbgSampledPeriodicProcessIdx, mpDbgWorkingPeriodicProcessIdx);
	transferResetCycle0Length(mpDbgSampledDisplayProcessIdx, mpDbgWorkingDisplayProcessIdx);
	transferResetCycle0Length(mpDbgSampledS64processIdx, mpDbgWorkingS64processIdx);
#endif // defined(useDebugCPUreading)

}

static void systemInfo::transferCycle0Length(uint8_t workingIdx, uint8_t startIdx)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	mainProgramVariables[(uint16_t)(workingIdx)] += heart::findCycle0Length(mainProgramVariables[(uint16_t)(startIdx)]);
	mainProgramVariables[(uint16_t)(startIdx)] = heart::cycles0(); // record starting time

	SREG = oldSREG; // restore interrupt flag status

}

#if defined(useDebugCPUreading)
static void systemInfo::transferResetCycle0Length(uint8_t sampledIdx, uint8_t workingIdx)
{

	if (bitFlags[(uint16_t)(bfPeek)] & peekEnableCPUread) mainProgramVariables[(uint16_t)(sampledIdx)] = mainProgramVariables[(uint16_t)(workingIdx)];
	mainProgramVariables[(uint16_t)(workingIdx)] = 0;

}

#endif // defined(useDebugCPUreading)
#if defined(useCPUreading)
static const uint8_t prgmFindCPUutilPercent[] PROGMEM = {
	instrLdRegMain, 0x02, mpCPUsampledMainWorkingIdx,
	instrAddMainToX, 0x02, mpCPUsampledIdleProcessIdx,
	instrMul2byByte, 100,
	instrMul2byConst, idxDecimalPoint,
	instrDiv2byMain, mpCPUsampledMainLoopIdx,
	instrDone											// exit to caller
};

static const uint8_t prgmOutputAvailableRAM[] PROGMEM = {
	instrLdRegMain, 0x02, mpAvailableRAMidx,
	instrMul2byConst, idxDecimalPoint,
	instrDone											// exit to caller
};

static uint8_t systemInfo::displayHandler(uint8_t cmd, uint8_t cursorPos)
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
			text::stringOut(devIdxLCD, PSTR("     T"));
#else // LCDcharWidth == 20
			text::stringOut(devIdxLCD, PSTR(" T"));
#endif // LCDcharWidth == 20
			text::stringOut(devIdxLCD, ull2str(nBuff, vSystemCycleIdx, tReadTicksToSeconds)); // output system time (since MPGuino was powered up)

			text::gotoXY(devIdxLCD, 0, 1);
#if LCDcharWidth == 20
			text::stringOut(devIdxLCD, PSTR("  FREE RAM: "));
#else // LCDcharWidth == 20
			text::stringOut(devIdxLCD, PSTR("FREE RAM: "));
#endif // LCDcharWidth == 20
			mainProgramVariables[(uint16_t)(mpAvailableRAMidx)] = availableRAMptr;
			SWEET64::runPrgm(prgmOutputAvailableRAM, 0);
			text::stringOut(devIdxLCD, ull2str(nBuff, 0, (LCDcharWidth / 2) - 2, 0));
			break;

		default:
			break;

	}

}

static void systemInfo::showCPUload(void)
{

	text::stringOut(devIdxLCD, PSTR("C%"));
	SWEET64::runPrgm(prgmFindCPUutilPercent, 0);
	text::stringOut(devIdxLCD, ull2str(nBuff, 2, 6, 0));

}

static void systemInfo::showCPUloading(void)
{

	text::initStatus(devIdxLCD);
	showCPUload();
	text::commitStatus(devIdxLCD);

}

#endif // defined(useCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
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

static uint8_t buttonView::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
		case displayOutputIdx:
#if defined(useAnalogButtons)
			text::hexWordOut(devIdxLCD, (uint16_t)(analogValue[(uint16_t)(analogButtonChannelIdx)]));
#endif // defined(useAnalogButtons)
			text::newLine(devIdxLCD);
			text::hexByteOut(devIdxLCD, thisButtonState);
			text::hexByteOut(devIdxLCD, lastButtonState);
#if defined(useTestAnalogButtonIdx)
			text::stringOut(devIdxLCD, PSTR(" pins, "));
			text::hexByteOut(devIdxLCD, thisButtonIdx);
			text::stringOut(devIdxLCD, PSTR(" idx"));
#else // defined(useTestAnalogButtonIdx)
			text::stringOut(devIdxLCD, PSTR(" pins"));
			text::newLine(devIdxLCD);
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

static const uint8_t prgmTerminalWriteMainProgramValue[] PROGMEM = {
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
	btnShortPressL,
	btnShortPressC,
	btnShortPressR,
	btnLongPressL,
	btnLongPressC,
	btnLongPressR,
};

#else // defined(useLegacyButtons)
static const uint8_t terminalButtonCount = 10;

static const char terminalButtonChars[] PROGMEM = {
	"lcrudLCRUD" tcEOS
};

static const uint8_t terminalButtonValues[] PROGMEM = {
	btnShortPressL,
	btnShortPressC,
	btnShortPressR,
	btnShortPressU,
	btnShortPressD,
	btnLongPressL,
	btnLongPressC,
	btnLongPressR,
	btnLongPressU,
	btnLongPressD,
};

#endif // defined(useLegacyButtons)
#endif // defined(useDebugButtonInjection)
static void terminal::outputBitFlagRegisterValue(uint8_t lineNumber)
{

	uint8_t oldSREG;
	uint8_t val;

	oldSREG = SREG; // save state of interrupt flag
	cli(); // disable interrupts

	val = bitFlags[(uint16_t)(lineNumber)]; // fetch instantaneous state of bit flag

	SREG = oldSREG; // restore state of interrupt flag

	text::hexByteOut(devIdxDebugTerminal, val);

}

static void terminal::outputTripFunctionValue(uint8_t lineNumber)
{

	text::tripFunctionOut(devIdxDebugTerminal, terminalIdx, lineNumber, decWindow, dfOutputLabel);
	text::charOut(devIdxDebugTerminal, mainCalcFuncVar.tripChar);

}

static void terminal::outputConstantValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchConstantValue, lineNumber);
	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, 0, tFormatToNumber));

}

static void terminal::outputParameterValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);
	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, 0, tFormatToNumber));

}

static void terminal::outputParameterExtra(uint8_t lineNumber)
{

	uint8_t i;

	i = EEPROM::getLength(lineNumber);

	text::hexByteOut(devIdxDebugTerminal, EEPROM::getParameterFlags(lineNumber));
	text::charOut(devIdxDebugTerminal, '-');
	text::hexByteOut(devIdxDebugTerminal, i);
	text::charOut(devIdxDebugTerminal, '-');
	text::hexWordOut(devIdxDebugTerminal, EEPROM::getAddress(lineNumber));
	if (lineNumber < eePtrSettingsEnd)
	{

		text::stringOut(devIdxDebugTerminal, PSTR(" (orig "));
		SWEET64::runPrgm(prgmFetchInitialParamValue, lineNumber);
		text::stringOut(devIdxDebugTerminal, ull2str(nBuff, 0, tFormatToNumber));
		text::stringOut(devIdxDebugTerminal, PSTR(")"));

	}
	else if (lineNumber < eePtrEnd)
	{

		if (i & 0x07) i += 0x08;
		i >>= 3;

		text::charOut(devIdxDebugTerminal, ' ');
		SWEET64::runPrgm(prgmFetchParameterValue, lineNumber);

		for (uint8_t x = 7; x < 8; x--)
			if (x < i) text::hexByteOut(devIdxDebugTerminal, ((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]))->u8[(uint16_t)(x)]);
			else text::stringOut(devIdxDebugTerminal, PSTR("  "));

		text::charOut(devIdxDebugTerminal, ' ');

	}

}

static void terminal::outputVolatileValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchVolatileValue, lineNumber);
	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, 0, tFormatToNumber));

}

static void terminal::outputMainProgramValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchMainProgramValue, lineNumber);
	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, 0, tFormatToNumber));

}

static void terminal::outputTripVarMeasuredValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, 0, tFormatToNumber));

}

static void terminal::outputTripVarMeasuredExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchTripVarValue, lineNumber);
	text::hexLWordOut(devIdxDebugTerminal, &s64reg[(uint16_t)(s64reg64_2)]);

}

static void terminal::outputDecimalValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmUpdateDecimalValue, lineNumber);
	text::hexLWordOut(devIdxDebugTerminal, &s64reg[(uint16_t)(s64reg64_2)]);

}

static void terminal::outputDecimalExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(prgmFetchDecimalValue, lineNumber);
	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, decPlace, decWindow, decMode));

}

static void terminal::processMath(uint8_t cmd)
{

	if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it
	if (terminalMode & tmSourceReadIn) decPlace = terminalSource; // if decimal count specified, save it
	// save terminal register contents for later
	if ((terminalMode & tmByteReadIn) || (cmd == '_')) SWEET64::runPrgm(prgmPerformMathOperation, cmd);

}

static void terminal::outputDecimalSettings(void)
{
	text::stringOut(devIdxDebugTerminal, PSTR("decimalFlags="));
	text::hexByteOut(devIdxDebugTerminal, decMode);
	text::stringOut(devIdxDebugTerminal, PSTR(", windowLen="));
	text::hexByteOut(devIdxDebugTerminal, decWindow);
	text::stringOut(devIdxDebugTerminal, PSTR(", places="));
	text::hexByteOut(devIdxDebugTerminal, decPlace);
	text::newLine(devIdxDebugTerminal);

}

static void terminal::dumpSWEET64information(union union_32 * instrLWord, const uint8_t * &prgmPtr, const uint8_t * prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8)
{

	text::hexWordOut(devIdxDebugTerminal, (uint16_t)(prgmPtr));
	text::charOut(devIdxDebugTerminal, ' ');
	text::hexWordOut(devIdxDebugTerminal, instrLWord->ui[1]);
	text::hexWordOut(devIdxDebugTerminal, instrLWord->ui[0]);
	text::charOut(devIdxDebugTerminal, ' ');
	text::hexByteOut(devIdxDebugTerminal, SWEET64processorFlags);
	text::newLine(devIdxDebugTerminal);

	for (uint8_t x = 0; x < 16; x++)
	{

		text::charOut(devIdxDebugTerminal, 9);
		text::hexByteOut(devIdxDebugTerminal, x);
		text::charOut(devIdxDebugTerminal, ' ');
		text::hexWordOut(devIdxDebugTerminal, (uint16_t)(prgmStack[(uint16_t)(x)]));

		if (x < s64reg64count)
		{

			text::charOut(devIdxDebugTerminal, 9, 2);
			text::hexByteOut(devIdxDebugTerminal, (x + 1));
			text::charOut(devIdxDebugTerminal, ' ');
			text::hexLWordOut(devIdxDebugTerminal, &prgmReg64[(uint16_t)(x)]);

		}

		if (x < si64reg8count)
		{

			text::charOut(devIdxDebugTerminal, 9);
			text::hexByteOut(devIdxDebugTerminal, x);
			text::charOut(devIdxDebugTerminal, ' ');
			text::hexByteOut(devIdxDebugTerminal, prgmReg8[(uint16_t)(x)]);

		}

		text::newLine(devIdxDebugTerminal);

	}

	text::newLine(devIdxDebugTerminal);

}

#if defined(useDebugTerminalSWEET64)
static void terminal::outputSWEET64registerContents(uint8_t lineNumber)
{

	if (lineNumber < s64reg64count) text::hexLWordOut(devIdxDebugTerminal, &terminalS64reg64[(uint16_t)(lineNumber)]);
	else
	{

		text::charOut(devIdxDebugTerminal, ' ', 14);
		text::hexByteOut(devIdxDebugTerminal, terminalS64reg8[(uint16_t)(lineNumber - s64reg64count)]);

	}

}

static void terminal::outputSWEET64registerExtra(uint8_t lineNumber)
{

	uint8_t i;

	if (lineNumber < s64reg64count)
	{

		SWEET64::copy64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), (union union_64 *)(&terminalS64reg64[(uint16_t)(lineNumber)]));
		i = decPlace;

	}
	else
	{

		SWEET64::runPrgm(prgmLoadByteValue, terminalS64reg8[(uint16_t)(lineNumber - s64reg64count)]);
		i = 0;

	}

	text::stringOut(devIdxDebugTerminal, ull2str(nBuff, i, decWindow, decMode));

}

static void terminal::outputSWEET64byte(uint8_t byt)
{

	text::charOut(devIdxDebugTerminal, ' ');
	text::hexByteOut(devIdxDebugTerminal, byt);

}

static void terminal::outputSWEET64operand(uint8_t flag, uint8_t &byt)
{

	switch (flag)
	{

		case (s64vReadInOperandByte):
		case (s64vReadInExtraByte):
		case (s64vReadInOperandByte | s64vOperandIndexed):
		case (s64vReadInExtraByte | s64vExtraIndexed):
			text::stringOut(devIdxDebugTerminal, PSTR(" (0x"));
			text::hexByteOut(devIdxDebugTerminal, (byt++) * 17);
			if (flag & (s64vOperandIndexed | s64vExtraIndexed)) text::stringOut(devIdxDebugTerminal, PSTR(" + tripIdx"));
			text::charOut(devIdxDebugTerminal, ')');
			break;

		case (s64vOperandIndexed):
		case (s64vExtraIndexed):
			text::stringOut(devIdxDebugTerminal, PSTR(" (tripIdx)"));
			break;

		case (s64vReadInExtraByte | s64vRelativeOperand):
			text::stringOut(devIdxDebugTerminal, PSTR(" (+/-0x"));
			text::hexByteOut(devIdxDebugTerminal, (byt++) * 17);
			text::charOut(devIdxDebugTerminal, ')');
			break;

		case (s64vExtraJump):
			text::stringOut(devIdxDebugTerminal, PSTR(" (jumpReg)"));
			break;

		default:
			break;

	}

}

static void terminal::outputSWEET64opcode(uint8_t lineNumber)
{

	uint32_t instrLWord;
	union union_32 * iLW = (union union_32 *)(&instrLWord);
	const uint8_t * prgmPtr;
	uint8_t isValid;
	uint8_t i;

	iLW->u8[0] = lineNumber; // store instruction to be decoded here
	prgmPtr = 0;

	SWEET64::fetchInstruction(iLW, prgmPtr, terminalS64reg8); // decode instruction
	isValid = terminalS64reg8[(uint16_t)(si64reg8valid)];

	if (isValid)
	{

		i = 1;

		text::charOut(devIdxDebugTerminal, '(');
		text::hexWordOut(devIdxDebugTerminal, iLW->ui[0]);
		text::charOut(devIdxDebugTerminal, ')');

		outputSWEET64byte(lineNumber); // output opcode byte

		if ((isValid & (s64vRegisterOperation | s64vReadInRegisterByte)) == (s64vRegisterOperation | s64vReadInRegisterByte)) outputSWEET64byte((i++) * 17); // output nominal 64-bit register operand byte

		if (isValid & s64vReadInOperandByte) outputSWEET64byte((i++) * 17); // output nominal primary operand byte

		if (isValid & s64vReadInExtraByte) outputSWEET64byte((i++) * 17); // output nominal extra opcode byte

		text::charOut(devIdxDebugTerminal, ' ', 3 * (5 - i)); // tab over to instruction column of output

		text::stringOut(devIdxDebugTerminal, opCodeList, lineNumber); // output opcode mnemonic

		i = 1;

		if ((isValid & (s64vRegisterOperation | s64vReadInRegisterByte)) == (s64vRegisterOperation | s64vReadInRegisterByte)) // output nominal 64-bit register operand
		{

			text::stringOut(devIdxDebugTerminal, PSTR(" 0x"));
			text::hexByteOut(devIdxDebugTerminal, (i++) * 17);

		}

		outputSWEET64operand(isValid & (s64vReadInOperandByte | s64vOperandIndexed), i); // output nominal primary operand

		outputSWEET64operand(isValid & (s64vReadInExtraByte | s64vExtraIndexed | s64vRelativeOperand | s64vExtraJump), i); // output nominal extra operand

	}
	else text::stringOut(devIdxDebugTerminal, PSTR("INVALID INSTRUCTION"));

}

#if defined(useDebugTerminalLabels)
static void terminal::outputSWEET64prgmOperand(const uint8_t * prgmPtr, uint8_t flag, uint8_t operandIdx, uint8_t labelIdx)
#else // defined(useDebugTerminalLabels)
static void terminal::outputSWEET64prgmOperand(const uint8_t * prgmPtr, uint8_t flag, uint8_t operandIdx)
#endif // defined(useDebugTerminalLabels)
{

	uint16_t tgt;
	uint8_t byt;

	byt = s64operands[(uint16_t)(operandIdx)];

	switch (flag)
	{

		case (s64vReadInOperandByte):
		case (s64vReadInExtraByte):
		case (s64vReadInOperandByte | s64vOperandIndexed):
		case (s64vReadInExtraByte | s64vExtraIndexed):
#if defined(useDebugTerminalLabels)
			text::stringOut(devIdxDebugTerminal, PSTR(" ("));
			if (labelIdx)
			{

				if (byt < pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelLength))
				{

					text::stringOut(devIdxDebugTerminal, (const char *)(pgm_read_word(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelStringPointer)), byt);

				}
				else text::stringOut(devIdxDebugTerminal, PSTR("* INVALID *"));

			}
			else
			{

				text::stringOut(devIdxDebugTerminal, PSTR("0x"));
				text::hexByteOut(devIdxDebugTerminal, byt);

			}

#else // defined(useDebugTerminalLabels)
			text::stringOut(devIdxDebugTerminal, PSTR(" (0x"));
			text::hexByteOut(devIdxDebugTerminal, byt);

#endif // defined(useDebugTerminalLabels)
			if (flag & (s64vOperandIndexed | s64vExtraIndexed)) text::stringOut(devIdxDebugTerminal, PSTR(" + tripIdx"));
			text::charOut(devIdxDebugTerminal, ')');
			break;

		case (s64vOperandIndexed):
		case (s64vExtraIndexed):
			text::stringOut(devIdxDebugTerminal, PSTR(" (tripIdx)"));
			break;

		case (s64vReadInExtraByte | s64vRelativeOperand):
			if (byt < 127) prgmPtr += byt;
			else prgmPtr -= (256 - byt);
			text::stringOut(devIdxDebugTerminal, PSTR(" $"));
			text::hexWordOut(devIdxDebugTerminal, prgmPtr);
			break;

		case (s64vExtraJump):
			text::stringOut(devIdxDebugTerminal, PSTR(" (jumpReg)")); // output jumpReg implied operand
			break;

		default:
			break;

	}

}

static void terminal::outputSWEET64prgmLine(union union_32 * instrLWord, const uint8_t * &prgmPtr, uint8_t traceFlag)
{

	const uint8_t * oldSched;
	uint8_t isValid;
	uint8_t opCode;
	uint8_t reg;
	uint8_t i;
#if defined(useDebugTerminalLabels)
	uint8_t operandLabelIdx;
	uint8_t extraLabelIdx;
#endif // defined(useDebugTerminalLabels)

	oldSched = prgmPtr;

	SWEET64::fetchInstruction(instrLWord, prgmPtr, terminalS64reg8); // decode instruction
	isValid = terminalS64reg8[(uint16_t)(si64reg8valid)];

	if (traceFlag)
	{

		text::charOut(devIdxDebugTerminal, ' ');
		text::hexWordOut(devIdxDebugTerminal, (uint16_t)(oldSched));
		text::charOut(devIdxDebugTerminal, '-');

		for (uint8_t x = 0; x < 5; x++)
		{

			i = pgm_read_byte(oldSched++);
			if (x == 0) opCode = i;
			if (x == 1) reg = i;

			if (oldSched > prgmPtr) text::charOut(devIdxDebugTerminal, ' ', 3);
			else outputSWEET64byte(i); // output opcode byte

		}

		if (terminalS64reg8[(uint16_t)(si64reg8valid)])
		{

#if defined(useDebugTerminalLabels)
			operandLabelIdx = 0;
			extraLabelIdx = 0;

			if (isValid & s64vRegisterOperation) // instruction does something with the 64 bit registers
			{

				switch (instrLWord->u8[2]) // perform load or store operation, according to ixx
				{

					case i14:	// load rX with const
						operandLabelIdx = 1;
						break;

					case i15:	// load rX with EEPROM init
					case i03:	// load rX with EEPROM
					case i04:	// store EEPROM rX
						operandLabelIdx = 2;
						break;

					case i05:	// load rX with main program
					case i06:	// store main program rX
						operandLabelIdx = 3;
						break;

					case i07:	// load rX with volatile
					case i08:	// store volatile rX
						operandLabelIdx = 4;
						break;

#if defined(useBarFuelEconVsTime)
					case i17:	// load rX with FEvT trip variable
#endif // defined(useBarFuelEconVsTime)
					case i18:	// load rX with trip variable
					case i19:	// store trip variable rX
						operandLabelIdx = 5;
						extraLabelIdx = 6;
						break;

					default:
						break;

				}

			}
			else
			{

				if ((isValid & s64vRelativeOperand) == 0)
				{

					switch (instrLWord->u8[0])
					{

						case e29:	// load jump register
						case e27:	// call
						case e28:	// jump
							extraLabelIdx = 7;
							break;

						case e24:	// load index EEPROM
						case e26:	// load index EEPROM parameter length
							extraLabelIdx = 2;
							break;

						default:
							break;

					}

				}

			}

#endif // defined(useDebugTerminalLabels)
			text::stringOut(devIdxDebugTerminal, opCodeList, opCode); // output opcode mnemonic

			if ((isValid & (s64vRegisterOperation | s64vReadInRegisterByte)) == (s64vRegisterOperation | s64vReadInRegisterByte)) // output nominal 64-bit register operand
			{

				text::stringOut(devIdxDebugTerminal, PSTR(" 0x"));
				text::hexByteOut(devIdxDebugTerminal, reg);

			}

#if defined(useDebugTerminalLabels)
			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInOperandByte | s64vOperandIndexed), s64oprPrimary, operandLabelIdx); // output primary operand

			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInExtraByte | s64vExtraIndexed | s64vRelativeOperand | s64vExtraJump), s64oprExtra, extraLabelIdx); // output extra operand

#else // defined(useDebugTerminalLabels)
			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInOperandByte | s64vOperandIndexed), s64oprPrimary); // output primary operand

			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInExtraByte | s64vExtraIndexed | s64vRelativeOperand | s64vExtraJump), s64oprExtra); // output extra operand

#endif // defined(useDebugTerminalLabels)
		}
		else text::stringOut(devIdxDebugTerminal, PSTR("?????"));

		text::newLine(devIdxDebugTerminal);

	}

}

#endif // defined(useDebugTerminalSWEET64)
#if defined(useBluetoothAdaFruitSPI)
static void terminal::outputBluetoothResponse(void)
{

	uint8_t c;
	uint8_t f;
	uint8_t i;

	i = 0;

	do
	{

		f = ringBuffer::isBufferNotEmpty(rbIdxBLEfriendIn);

		if (f)
		{

			c = ringBuffer::pull(rbIdxBLEfriendIn);

			if (c >= 0x20) i = 1; // if this is a printable character, signal to end this function with a newline
			else i = 0; // otherwise, suppress the newline at the end of the function

			if (c == 0x0D) text::newLine(devIdxDebugTerminal);
			else if (c == 0x0A) c = 0;
			else text::charOut(devIdxDebugTerminal, c);

			if (ringBuffer::free(rbIdxBLEfriendIn) > 15) // is there enough room in the input buffer for another packet?
			{

				// if IRQ is still pulled high, there's another packet to be read in
				if (blefriend::inputCheck()) blefriend::readPacket();

			}

		}

	}
	while (f);

	if (i) text::newLine(devIdxDebugTerminal);

}

#endif // defined(useBluetoothAdaFruitSPI)
static void terminal::mainProcess(void)
{

#if defined(useDebugTerminalSWEET64)
	uint32_t instrLWord;
	union union_32 * iLW = (union union_32 *)(&instrLWord);
	uint8_t loopFlag;
#endif // defined(useDebugTerminalSWEET64)
	uint8_t i;
	uint8_t j;
	uint8_t k;
	const char * separatorPtr;
	uint8_t chr;
	uint8_t oldSREG;

/*
the debug terminal is based off of the Apple II system monitor, which is command-line based. One or more commands are
entered at the prompt, separated by space characters. Pressing <Enter> will cause the monitor to execute these commands

	terminal commands:
       [y].[x]P - list stored parameters, optionally between [y] and [x]
xP:y [y] [y]... - store one or more y values, starting at stored parameter x

       [y].[x]V - list volatile variables, optionally between [y] and [x]
xV:y [y] [y]... - store one or more y values, starting at volatile variable x

       [y].[x]M - list main program variables, optionally between [y] and [x]
xM:y [y] [y]... - store one or more y values, starting at main program variable x

       [y].[x]T - list terminal trip variable values, optionally between [y] and [x]
xT:y [y] [y]... - store one or more y values, starting at terminal trip variable x

       [y].[x]S - list status and command bitflags, optionally between [y] and [x]
xS:y            - store new bitflag value y into bitflag register x

      [y].[x]^E - list SWEET64 register contents, optionally between [y] and [x]
x^E:y           - store one or more y values, starting at SWEET64 register x

    [y].[x]O - list program constants, optionally between [y] and [x]
[z]<[y].[x]L - list terminal trip variable function outputs, optionally between [y] and [x]
                [z] - decimal window length (optional)

[z]<[y].[x]U - list decimal number sample for output
                [z] - decimal window length (optional)
                [y] - decimal digit count (optional)
                [x] - decimal processing flag (optional)

   [y].[x]^I - list SWEET64 instructions, along with their operands, optionally between [y] and [x]
       [x]^L - list 20 lines of SWEET64 program code, optionally beginning at trip function [x]
   [y]<[x]^T - trace execution of [x] lines of SWEET64 program code, optionally beginning at trip function [y]
               if [x] is omitted, traces 1 line
               if [x] is explicitly set to 0, traces until program completes

    [y]<[x]R - read trip variable x into trip variable y
                default for x and y is terminal trip variable
                if no x or y specified, lists available trip variables

   [z]<[y].x - enters a number x into the 64-bit math accumulator
                [z] - decimal window length (optional)
                [y] - decimal digit count (optional)
          +x - adds x to math accumulator
          -x - subtracts x from math accumulator
          *x - multiplies math accumulator by x
          /x - divides math accumulator by x
          |  - finds square root of math accumulator
          =x - enters a number x into the 64-bit math accumulator

           I - inject button press
                short (l, c, r, u, d)
                 long (L, C, R, U, D)
           Y - sends the rest of the input string to BLEfriend shield
          ^S - displays supplemental system information
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

		default:			// handle undefined terminal states
			errIdx = tseIdxState;
		case tsError:		// output error message
			text::stringOut(devIdxDebugTerminal, tseErrorStringList, errIdx);
		case tsBell:		// ring terminal bell
			text::charOut(devIdxDebugTerminal, 0x07);
		case tsInitInput:	// initialize user line input, output prompt
			text::newLine(devIdxDebugTerminal);
			text::charOut(devIdxDebugTerminal, ']');
			ringBuffer::empty(rbIdxTerminal);
			terminalState = tsUserInput;
			errIdx = tseIdxLineCancel;
#if defined(useDebugCPUreading)
			bitFlags[(uint16_t)(bfPeek)] |= (peekEnableCPUread); // enable supplemental CPU time measurements
#endif // defined(useDebugCPUreading)

		case tsUserInput:	// get line
			do
			{

				i = text::chrIn(devIdxDebugTerminal); // read in a possible input character

				if (i) // if an actual input character is read in
				{

					switch (i)
					{

						case 0x0D:	// enter
							text::charOut(devIdxDebugTerminal, 0x0D);
							heart::changeBitFlagBits(bfTimer0Command, 0, t0cInputReceived); // tell timer0 that some user input was received
							terminalState = tsInitTerminalCmd;
							break;

						case 0x03:	// cancel line input
						case 0x18:	// cancel line input
							terminalState = tsError;
							break;

#if defined(useDebugTerminalSWEET64)
						case 0x05:	// display SWEET64 register contents
#if defined(useDebugTerminalLabels)
						case 0x06:	// display complete list of available SWEET64 functions
#endif // defined(useDebugTerminalLabels)
						case 0x09:	// list SWEET64 opcodes and their operands
						case 0x0C:	// list 20 lines of SWEET64 pseudo-code
						case 0x14:	// trace 1 or more lines of SWEET64 pseudo-code
#endif // defined(useDebugTerminalSWEET64)
						case 0x13:	// display supplemental system information
						case 0x20 ... 0x7E:	// unspecified input character
							if (ringBuffer::isBufferFull(rbIdxTerminal)) terminalState = tsError;
							else
							{

								ringBuffer::push(rbIdxTerminal, i);

								if (i > 31)	text::charOut(devIdxDebugTerminal, i);
								else
								{

									text::charOut(devIdxDebugTerminal, '^');
									text::charOut(devIdxDebugTerminal, i + 64);

								}

							}
							break;

						default:	// ignore input character as it is not supported
							break;

					}

				}

			}
			while ((i) && (terminalState == tsUserInput)); // loop while an input character is actually received, and terminal state is still in user input mode
			break;

		case tsInitTerminalCmd:	// initialize command processing and terminal command
			terminalCmd = 0; // initialize command executor
		case tsInitProcessing:	// initialize command processing
			terminalLine = 0; // initialize terminal output line
			terminalMode = (tmInitHex); // set up to initialize read-in of hexadecimal number
			terminalSource = 0;
			terminalTarget = 0;
			terminalAddress = 0;
			primaryFunc = 0;
			extraFunc = 0;
			maxLine = 0;
			prgmPtr = 0;
#if defined(useDebugTerminalLabels)
			labelList = 0;
#endif // defined(useDebugTerminalLabels)
			errIdx = 0;
			terminalState = tsProcessCommand;
			nextTerminalState = tsProcessCommand;
#if defined(useDebugCPUreading)
			bitFlags[(uint16_t)(bfPeek)] |= (peekEnableCPUread); // enable supplemental CPU time measurements
#endif // defined(useDebugCPUreading)

		case tsProcessCommand:   // process input line
			do // loop until the terminal state changes
			{

				if (terminalMode & tmInitInput) // if a terminal byte init command is received
				{

					terminalMode &= ~(tmInitInput | tmByteReadIn); // acknowledge

					terminalByte = 0; // reset terminal byte to 0
					SWEET64::init64byt((union union_64 *)(&s64reg[(uint16_t)(s64reg64_6)]), 0);

				}

				i = 1; // initialize numeric/keypress group processing loop flag

				while (i) // either pull a command character, or read in an entire hex or decimal number, or read in a keypress group
				{

					if (ringBuffer::isBufferEmpty(rbIdxTerminal)) chr = 0x0D;
					else chr = ringBuffer::pull(rbIdxTerminal);

					j = chr; // save raw input character for button press processing

					if (chr > 0x5F) chr &= 0x5F; // force input character to uppercase

					switch (terminalMode & tmInputMask) // process a possible digit, hexit, or button press character
					{

#if defined(useDebugButtonInjection)
						case (tmButtonInput):					// parse a button press character
						case (tmButtonInput | tmByteReadIn):	// parse a button press character
							i = 0; // assume this is an invalid character

							for (uint8_t x = 0; x < terminalButtonCount; x++)
							{

								if (j == pgm_read_byte(&terminalButtonChars[(uint16_t)(x)]))
								{

									terminalByte |= (pgm_read_byte(&terminalButtonValues[(uint16_t)(x)]));
									terminalMode |= (tmByteReadIn);
									i = 1; // signal that this is a valid character

								}

							}
							break;

#endif // defined(useDebugButtonInjection)
						case (tmHexInput):						// parse a generic hexadecimal digit or switch to decimal mode
							if (chr == '\\')
							{

								terminalMode &= ~(tmHexInput); // clear hexadecimal input mode
								terminalMode |= (tmDecimalInput | tmByteReadIn); // set decimal input mode
								break;

							}
						case (tmHexInput | tmByteReadIn):		// parse a generic hexadecimal digit
							switch (chr)
							{

								case 'A' ... 'F':
									chr -= 7;
								case '0' ... '9':
									chr -= 48;
									terminalMode |= (tmByteReadIn);
									terminalByte = SWEET64::runPrgm(prgmParseHexDigit, chr);
									break;

								default:
									i = 0; // signal that a non-numeric character was read in
									break;

							}
							break;

						case (tmDecimalInput):					// parse a generic decimal digit or switch to hex mode
							if ((chr == 'X') || (chr == '$'))
							{

								terminalMode &= ~(tmDecimalInput); // clear decimal input mode
								terminalMode |= (tmHexInput | tmByteReadIn); // set hexadecimal input mode
								break;

							}
						case (tmDecimalInput | tmByteReadIn):	// parse a generic decimal digit
							switch (chr)
							{

								case '0' ... '9':
									chr -= 48;
									terminalMode |= (tmByteReadIn);
									terminalByte = SWEET64::runPrgm(prgmParseDecimalDigit, chr);
									break;

								default:
									i = 0; // signal that a non-numeric character was read in
									break;

							}
							break;

						default:								// nothing to parse - could be a possible command
							i = 0; // signal that a non-numeric character was read in
							break;

					}

				}

				switch (chr) // perform overall command processing
				{

					case 0x0D:	// treat as a special space character
						nextTerminalState = tsInitInput;
					case ' ':	// space character - process a compound command
						if (chr == ' ') i = tsInitTerminalCmd;
						else i = tsInitInput;

						if (terminalCmd) chr = terminalCmd;

						switch (terminalCmd)
						{

#if defined(useDebugButtonInjection)
							case 'i':   // inject button press
								if (terminalMode & tmByteReadIn) // if a button group was read in
								{

									button::inject(terminalByte); // inject the parsed button press value into timer0
									terminalState = tsInjectButtonPress;
									terminalMode |= (tmInitInput); // signal to go parse another input value

								}
								else terminalState = i; // no button group was read in, so cancel button injection mode
								break;

#endif // defined(useDebugButtonInjection)
#if defined(useDebugTerminalSWEET64)
							case 0x25:	// enter a SWEET64 register value
								if (terminalMode & tmByteReadIn) // if a parameter value was read in
								{

									if (terminalAddress < s64reg64count) SWEET64::copy64((union union_64 *)(&terminalS64reg64[(uint16_t)(terminalAddress)]), (union union_64 *)(&s64reg[(uint16_t)(s64reg64_6)]));
									else terminalS64reg8[(uint16_t)(terminalAddress - s64reg64count)] = terminalByte;

									if ((++terminalAddress) < maxLine)
									{

										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmHexInput | tmInitInput); // shift to reading a new hexadecimal value

									}
									else terminalState = i; // if at end of storage range, reset command

								}
								else terminalState = i; // no parameter value was read in, so cancel parameter value entry mode
								break;

#endif // defined(useDebugTerminalSWEET64)
							case 'p':   // enter a stored parameter value
								if (terminalMode & tmByteReadIn) // if a parameter value was read in
								{

									EEPROM::onChange(prgmTerminalWriteParameterValue, terminalAddress++);

									if (terminalAddress < maxLine)
									{

										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmDecimalInput | tmInitInput); // shift to reading a new decimal value

									}
									else terminalState = i; // if at end of storage range, reset command

								}
								else terminalState = i; // no parameter value was read in, so cancel parameter value entry mode
								break;

							case 'm':   // enter a main program variable value
							case 't':   // enter a trip variable measurement value
							case 'v':   // enter a volatile variable value
								if (terminalMode & tmByteReadIn) // if a value was read in
								{

									SWEET64::runPrgm(prgmPtr, terminalAddress++);

									if (terminalAddress < maxLine)
									{

										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmDecimalInput | tmInitInput); // shift to reading a new decimal value

									}
									else terminalState = i; // if at end of storage range, reset command

								}
								else terminalState = i; // no value was read in, so cancel value entry mode

								break;

							case 's':	// enter a bitflag register value
								if (terminalMode & tmByteReadIn) // if a bitflag register value was read in
								{

									oldSREG = SREG; // save interrupt flag status
									cli(); // disable interrupts

									bitFlags[(uint16_t)(terminalAddress)] = terminalByte; // save bitflag register value

									SREG = oldSREG; // restore interrupt flag status

								}

								terminalState = i; // cancel bitflag register value entry mode
								break;

							case 0:		// no input received
								chr = 0x03;
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
								outputDecimalSettings();
								text::charOut(devIdxDebugTerminal, '=');
								text::stringOut(devIdxDebugTerminal, ull2str(nBuff, decPlace, decWindow, decMode));
								text::stringOut(devIdxDebugTerminal, PSTR(" (0x"));
								text::hexLWordOut(devIdxDebugTerminal, &s64reg[(uint16_t)(s64reg64_7)]);
								text::stringOut(devIdxDebugTerminal, PSTR(")" tcCR));

								terminalState = nextTerminalState;
								break;

#if defined(useDebugTerminalSWEET64)
							case 0x05:	// list SWEET64 register contents
								primaryFunc = terminal::outputSWEET64registerContents;
								extraFunc = terminal::outputSWEET64registerExtra;
#if defined(useDebugTerminalLabels)
								labelList = terminalSWEET64registerLabels;
#endif // defined(useDebugTerminalLabels)
								terminalState = tsInitListDecimal;
								nextTerminalState = i;
								break;

#endif // defined(useDebugTerminalSWEET64)
							case 'M':   // list available main program variables
#if defined(useDebugCPUreading)
								bitFlags[(uint16_t)(bfPeek)] &= ~(peekEnableCPUread);

#endif // defined(useDebugCPUreading)
#if defined(useDebugTerminalLabels)
								labelList = terminalMainProgramVarLabels;
#endif // defined(useDebugTerminalLabels)
								primaryFunc = terminal::outputMainProgramValue;
								terminalState = tsInitList;
								nextTerminalState = i;
								break;

							case 'P':   // list available stored parameters
#if defined(useDebugTerminalLabels)
								labelList = terminalParameterNames;
#endif // defined(useDebugTerminalLabels)
								primaryFunc = terminal::outputParameterValue;
								extraFunc = terminal::outputParameterExtra;
								terminalState = tsInitList;
								nextTerminalState = i;
								break;

							case 'S':	// list available bitflag registers
#if defined(useDebugTerminalLabels)
								labelList = terminalBitFlagRegisterNames;
#endif // defined(useDebugTerminalLabels)
								primaryFunc = terminal::outputBitFlagRegisterValue;
								terminalState = tsInitList;
								nextTerminalState = i;
								break;

							case 'T':   // list available trip variable measurements
#if defined(useDebugTerminalLabels)
								labelList = terminalTripVarLabels;
#endif // defined(useDebugTerminalLabels)
								primaryFunc = terminal::outputTripVarMeasuredValue;
								extraFunc = terminal::outputTripVarMeasuredExtra;
								terminalState = tsInitList;
								nextTerminalState = i;
								break;

							case 'V':   // list available volatile variables
#if defined(useDebugTerminalLabels)
								labelList = terminalVolatileVarLabels;
#endif // defined(useDebugTerminalLabels)
								primaryFunc = terminal::outputVolatileValue;
								terminalState = tsInitList;
								nextTerminalState = i;
								break;

							case 0x03:	// do nothing
								break;

							default:	// unrecognized command - do nothing
								terminalState = nextTerminalState;
								chr = 0x03; // cancel unrecognized command
								break;

						}
						break;

					default:
						if (terminalCmd > 0x5F) errIdx = tseIdxSyntax; // if any sort of repeating value processing mode
						else
						{

							switch (chr)
							{

#if defined(useDebugTerminalHelp)
								case '?':   // display help
									terminalState = tsOutputHelpLine; // this command WILL print a lot of different lines, so handle this command one iteration at a time
									break;

#endif // defined(useDebugTerminalHelp)
#if defined(useBluetoothAdaFruitSPI)
								case 'Y':   // copy the rest of the input buffer to the BLEfriend module
									terminalState = tsOutputBLEfriend;
									break;

#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugButtonInjection)
								case 'I':   // inject button press
									chr = 'i';
									terminalMode = (tmInitButton); // shift to reading button press words
									break;

#endif // defined(useDebugButtonInjection)
								case 0x13:	// display supplemental system information
									outputDecimalSettings();

#if defined(useBluetooth)
									text::stringOut(devIdxDebugTerminal, PSTR("btInputState = " tcEOS));
									text::hexByteOut(devIdxDebugTerminal, btInputState);
									text::newLine(devIdxDebugTerminal);

#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsSpeed)
									text::stringOut(devIdxDebugTerminal, PSTR("FEvSpdTripIdx = " tcEOS));
									text::hexByteOut(devIdxDebugTerminal, FEvSpdTripIdx);
									text::newLine(devIdxDebugTerminal);

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBluetoothAdaFruitSPI)
									outputBluetoothResponse();

#endif // defined(useBluetoothAdaFruitSPI)
									break;

								case '.':	// specify source address
									if (terminalMode & (tmButtonInput | tmSourceReadIn)) errIdx = tseIdxSourceVal; // if source was already read in
									else // allows for entering blank source address
									{

										terminalSource = terminalByte; // save source start byte value
										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmInitHex | tmSourceReadIn); // mark that source address was read in, and shift to hex input

									}
									break;

								case '<':	// specify target address
									if (terminalMode & (tmButtonInput | tmTargetReadIn)) errIdx = tseIdxTargetVal; // if target was already read in
									else
									{

										terminalTarget = terminalByte; // save target start byte value
										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmInitHex | tmTargetReadIn); // mark that target address was read in, and shift to hex input

									}
									break;

								case ':':   // specify storage address
									if (terminalMode & (tmButtonInput | tmAddressReadIn)) errIdx = tseIdxAddressVal; // if address was already read in
									else if (terminalMode & tmSourceReadIn) errIdx = tseIdxSourceVal; // if source was already read in
									else if (terminalMode & tmTargetReadIn) errIdx = tseIdxTargetVal; // if target was already read in
									else if (terminalMode & tmByteReadIn) // if a byte was read in
									{

										chr = (terminalCmd | 0x20); // shift existing mode into lower-case to signal that a value is to be stored
										terminalAddress = terminalByte; // save address byte value
										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmAddressReadIn); // signal that address was read in

										switch (chr)
										{

											case 'm':   // enter a main program variable value
											case 'p':   // enter a stored parameter value
											case 't':   // enter a trip variable measurement value
											case 'v':   // enter a volatile variable value
												terminalMode |= (tmInitDecimal); // shift to decimal input
												if (terminalAddress >= maxLine) errIdx = tseIdxAddressVal;
												break;

#if defined(useDebugTerminalSWEET64)
											case 0x25:	// enter a SWEET64 register value
#endif // defined(useDebugTerminalSWEET64)
											case 's':	// enter a bitflag register value
												terminalMode |= (tmInitHex); // shift to hexadecimal input
												if (terminalAddress >= maxLine) errIdx = tseIdxAddressVal;
												break;

											default:	// unsupported storage command
												errIdx = tseIdxSyntax;
												break;

										}

									}
									else errIdx = tseIdxNoAddress; // otherwise, no number was read in
									break;

								case '+':	// add
								case '-':	// subtract
								case '*':	// multiply
								case '/':	// divide
#if defined(useIsqrt)
								case '_':	// square root
#endif // defined(useIsqrt)
								case '=':	// output last result
									processMath(terminalCmd);
									terminalMode |= (tmInitInput); // shift to reading a new numeric value
									break;

								case 'L':   // list available trip functions
									if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it

									maxLine = dfMaxValDisplayCount;
#if defined(useDebugTerminalLabels)
									labelList = terminalTripFuncNames;
#endif // defined(useDebugTerminalLabels)
									primaryFunc = terminal::outputTripFunctionValue;
									terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

								case 'O':	// list available program constants
									maxLine = idxMaxConstant;
#if defined(useDebugTerminalLabels)
									labelList = terminalConstIdxNames;
#endif // defined(useDebugTerminalLabels)
									primaryFunc = terminal::outputConstantValue;
									terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

								case 'R':   // list available trip variables
									if (terminalMode & tmSourceReadIn) errIdx = tseIdxBadIndex; // if source was already read in
									else if (terminalMode & (tmTargetReadIn | tmByteReadIn)) // if either target or byte was read in
									{

										// if no target was read in, assume terminal trip index
										if ((terminalMode & tmTargetReadIn) == 0) terminalTarget = terminalIdx;

										// if no byte was read in, assume terminal trip index
										if ((terminalMode & tmByteReadIn) == 0) terminalByte = terminalIdx;

										if (terminalTarget >= tripSlotTotalCount) errIdx = tseIdxTargetVal;
										else if (terminalByte >= tripSlotTotalCount) errIdx = tseIdxSourceVal;
										else
										{

											SWEET64::runPrgm(prgmLoadTrip, terminalByte); // this allows direct loading/saving of trips to EEPROM
											SWEET64::runPrgm(prgmSaveTrip, terminalTarget);

											terminalState = tsInitProcessing;

#if defined(useDebugTerminalLabels)
											text::stringOut(devIdxDebugTerminal, terminalTripVarNames, terminalByte);
											text::stringOut(devIdxDebugTerminal, PSTR(" -> "));
											text::stringOut(devIdxDebugTerminal, terminalTripVarNames, terminalTarget);
											text::newLine(devIdxDebugTerminal);

#endif // defined(useDebugTerminalLabels)
										}

									}
#if defined(useDebugTerminalLabels)
									else
									{

										maxLine = tripSlotTotalCount;
										labelList = terminalTripVarNames;
										terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time

									}

#else // defined(useDebugTerminalLabels)
									else errIdx = tseIdxSyntax;

#endif // defined(useDebugTerminalLabels)
									break;

								case 'U':	// output a sample list of decimal numbers
									maxLine = 14;
									primaryFunc = terminal::outputDecimalValue;
									extraFunc = terminal::outputDecimalExtra;
									terminalState = tsInitListDecimal;
									nextTerminalState = tsInitProcessing;
									break;

#if defined(useDebugTerminalSWEET64)
#if defined(useDebugTerminalLabels)
								case 0x06:	// display complete list of available SWEET64 functions
									maxLine = dfMaxValTotalCount;
									labelList = terminalTripFuncNames;
									terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

#endif // defined(useDebugTerminalLabels)
								case 0x09:	// list SWEET64 opcodes and their operands
									maxLine = maxValidSWEET64instr;
									primaryFunc = terminal::outputSWEET64opcode;
									terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

								case 0x14:	// trace 1 or more lines of SWEET64 pseudo-code
									if (terminalMode & tmTargetReadIn)
									{

										terminalExecSched = (const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(terminalTarget)]));

										if (terminalListSched == 0) terminalListSched = terminalExecSched;
										if (terminalMode & tmSourceReadIn) terminalS64reg8[(uint16_t)(si64reg8trip)] = terminalSource;

										terminalS64reg8[(uint16_t)(si64reg8flags)] = SWEET64traceFlagGroup; // initialize terminal SWEET64 flags
										terminalS64reg8[(uint16_t)(si64reg8spnt)] = 0; // initialize terminal SWEET64 stack pointer
										terminalS64reg8[(uint16_t)(si64reg8jump)] = 0; // initialize terminal SWEET64 jump register

									}
									if (terminalMode & tmByteReadIn) maxLine = terminalByte;
									else maxLine = 1;

									if (terminalExecSched) terminalState = tsTraceSWEET64line;
									else errIdx = tseIdxBadSWEET64addr;

									break;

								case 0x0C:	// list 20 lines of SWEET64 pseudo-code
									if (terminalMode & tmByteReadIn) terminalListSched = (const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(terminalByte)]));

									maxLine = 20;

									if (terminalListSched) terminalState = tsOutputSWEET64line;
									else errIdx = tseIdxBadSWEET64addr;

									break;

								case 0x05:	// list SWEET64 register contents, with optional register value storage
									maxLine = s64reg64count + si64reg8count;
									break;

#endif // defined(useDebugTerminalSWEET64)
								case 'M':   // list available main program variables, with optional main program value storage
									maxLine = mpVariableMaxIdx;
									prgmPtr = prgmTerminalWriteMainProgramValue;
									break;

								case 'P':   // list available stored parameters, with optional stored parameter value storage
									maxLine = eePtrEnd;
									break;

								case 'S':	// list available bitflag registers, with optional bitflag register value storage
									maxLine = bitFlagMaxIdx;
									break;

								case 'T':   // list available trip variable measurements, with optional trip variable value storage
									maxLine = rvMeasuredCount;
									prgmPtr = prgmWriteTripMeasurementValue;
									break;

								case 'V':   // list available volatile variables, with optional volatile variable value storage
									maxLine = vVariableMaxIdx;
									prgmPtr = prgmWriteVolatileValue;
									break;

								case 0x03:	// "do-nothing" command to support repeating value processing mode
									break;

								case 'X':	// enter hexadecimal entry mode (if not caught by number parser above, it's a syntax error)
								case '$':	// enter hexadecimal entry mode (if not caught by number parser above, it's a syntax error)
								case '\\':	// enter decimal entry mode (if not caught by number parser above, it's a syntax error)
								default:
									errIdx = tseIdxSyntax;
									break;

							}

						}
						break;

				}

				if (errIdx) terminalState = tsError; // if an error occurred, shift to error handling terminal state
				else terminalCmd = chr; // save command for later

			}
			while (terminalState == tsProcessCommand);

			break;

#if defined(useDebugTerminalHelp)
		case tsOutputHelpLine:	// print a line of help
			text::stringOut(devIdxDebugTerminal, terminalHelp, terminalLine++);
			if (pgm_read_byte(findStr(terminalHelp, terminalLine)) == 0) terminalState = tsInitProcessing;
			break;

#endif // defined(useDebugTerminalHelp)
#if defined(useBluetoothAdaFruitSPI)
		case tsOutputBLEfriend:
			i = 0;

			while (ringBuffer::isBufferNotEmpty(rbIdxTerminal))
			{

				i = ringBuffer::pull(rbIdxTerminal);
				if (bitFlags[(uint16_t)(bfPeek)] & peekBLEfriendEcho) text::charOut(devIdxDebugTerminal, i);
				text::charOut(devIdxBLEfriend, i);
				i = 1;

			}

			if (i)
			{

				if (bitFlags[(uint16_t)(bfPeek)] & peekBLEfriendEcho) text::newLine(devIdxDebugTerminal);

				blefriend::outputBufferWithResponse();

				outputBluetoothResponse();

			}

			terminalState = tsInitInput;
			break;

#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugButtonInjection)
		case tsInjectButtonPress:	// wait for injected buttonpress to be accepted into timer0
			if (bitFlags[(uint16_t)(bfTimer0Command)] & t0cProcessButton) break;
			terminalState = tsInjectButtonsUp;
			button::inject(buttonsUp); // inject a buttons-up press into timer0
			break;

		case tsInjectButtonsUp:	// wait for injected buttonpress to be accepted into timer0
			if (bitFlags[(uint16_t)(bfTimer0Command)] & t0cProcessButton) break;
			terminalState = nextTerminalState;
			break;

#endif // defined(useDebugButtonInjection)
#if defined(useDebugTerminalSWEET64)
		case tsTraceSWEET64line:	// trace one or more lines of SWEET64 program
			if ((terminalExecSched) && (terminalState == tsTraceSWEET64line))
			{

				// decode instruction, and output if trace flag is enabled
				outputSWEET64prgmLine(iLW, terminalExecSched, (terminalS64reg8[(uint16_t)(si64reg8flags)] & SWEET64traceFlag));

				if (terminalS64reg8[(uint16_t)(si64reg8valid)])
				{

					SWEET64::executeInstruction(iLW, terminalExecSched, terminalStack, terminalS64reg64, terminalS64reg8); // execute instruction

					if (terminalS64reg8[(uint16_t)(si64reg8valid)] == 0) terminalExecSched = 0;

					if (terminalS64reg8[(uint16_t)(si64reg8flags)] & SWEET64traceFlag) // if trace flag is still enabled, output register values
						dumpSWEET64information(iLW, terminalExecSched, terminalStack, terminalS64reg64, terminalS64reg8);

				}
				else terminalExecSched = 0;

				if (maxLine)
				{

					maxLine--;
					if (maxLine == 0) terminalState = tsInitProcessing;

				}

			}

			if (terminalExecSched == 0) terminalState = tsInitProcessing;

			break;

		case tsOutputSWEET64line:	// output program listing
			outputSWEET64prgmLine(iLW, terminalListSched, 1);

			if ((--maxLine) == 0) terminalState = tsInitProcessing;

			break;

#endif // defined(useDebugTerminalSWEET64)
		case tsInitListDecimal:
			if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it
			if (terminalMode & tmSourceReadIn) decPlace = terminalSource; // if decimal count specified, save it
			if (terminalMode & tmByteReadIn) decMode = terminalByte; // if decimal mode specified, save it

			outputDecimalSettings();

			terminalState = tsProcessList;
			break;

		case tsInitListReadOnly:	// list output with command re-initialization and processing of read-in source, target, and end bytes
			nextTerminalState = tsInitProcessing;
		case tsInitList:			// list output with processing of read-in source, target, and end bytes
			if (terminalMode & tmSourceReadIn)
			{

				if (terminalSource >= maxLine) errIdx = tseIdxSourceVal;
				else
				{

					terminalLine = terminalSource;

					if (terminalMode & tmByteReadIn)
					{

						if (terminalByte >= maxLine) errIdx = tseIdxTargetVal;
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

					if (terminalByte >= maxLine) errIdx = tseIdxBadIndex;
					else
					{

						terminalLine = terminalByte;
						maxLine = terminalByte + 1;

					}

				}

			}

			if (errIdx)
			{

				terminalState = tsError; // error back to input state
				break;

			}
			else terminalState = tsProcessList;

		case tsProcessList:		// output list of selected items
			separatorPtr = terminalPrimarySeparator;
			text::hexByteOut(devIdxDebugTerminal, terminalLine);

			if (primaryFunc)
			{

				text::stringOut(devIdxDebugTerminal, separatorPtr);
				separatorPtr = terminalSecondarySeparator;
				primaryFunc(terminalLine);

			}
			if (extraFunc)
			{

				text::stringOut(devIdxDebugTerminal, separatorPtr);
				separatorPtr = terminalSecondarySeparator;
				extraFunc(terminalLine);

			}
#if defined(useDebugTerminalLabels)
			if (labelList)
			{

				text::stringOut(devIdxDebugTerminal, separatorPtr);
				separatorPtr = terminalSecondarySeparator;
				text::stringOut(devIdxDebugTerminal, labelList, terminalLine);

			}
#endif // defined(useDebugTerminalLabels)
			text::newLine(devIdxDebugTerminal);

			terminalLine++;

			if (terminalLine >= maxLine) terminalState = nextTerminalState; // go back to read another command

			break;

	}

}

#endif // defined(useDebugTerminal)
#if defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
static uint8_t signalSim::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t i;

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(devIdxLCD, debugScreenFuncNames, cursorPos); // briefly display screen name
			i = (bitFlags[(uint16_t)(bfSignalSimModeFlags)] & debugEnableFlags);
			switch (cursorPos)
			{

				case 0:
					heart::changeBitFlagBits(bfSignalSimModeFlags, 0, (debugInjectorFlag | debugVSSflag));
					break;

				case 1:
					heart::changeBitFlagBits(bfSignalSimModeFlags, debugInjectorFlag, debugVSSflag);
					break;

				case 2:
					heart::changeBitFlagBits(bfSignalSimModeFlags, (debugInjectorFlag | debugVSSflag), 0);
					break;

				case 3:
					heart::changeBitFlagBits(bfSignalSimModeFlags, debugVSSflag, debugInjectorFlag);
					break;

				default:
					break;

			}
			if ((bitFlags[(uint16_t)(bfSignalSimModeFlags)] & debugEnableFlags) ^ i) configurePorts();

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

#endif // defined(useButtonInput)
static void signalSim::configurePorts(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// configure VSS pin for either normal operation input or debug output
	if (bitFlags[(uint16_t)(bfSignalSimModeFlags)] & debugVSSflag)
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

		bitFlags[(uint16_t)(bfSignalSimModeFlags)] &= ~(debugVSSready); // reset VSS ready bit

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
	if (bitFlags[(uint16_t)(bfSignalSimModeFlags)] & debugInjectorFlag) // configure injector sense pins as outputs
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

		bitFlags[(uint16_t)(bfSignalSimModeFlags)] &= ~(debugFIPready); // reset pulse and pulse width ready bits

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

	if (bitFlags[(uint16_t)(bfSignalSimModeFlags)] & debugEnableFlags) bitFlags[(uint16_t)(bfTimer1Command)] |= (t1cEnableDebug);
	else bitFlags[(uint16_t)(bfTimer1Command)] &= ~(t1cEnableDebug);

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
			debugFIPtickLength = pgm_read_word(&debugFIPvalues[(uint16_t)(debugFIPidx)]);
			debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(uint16_t)(debugFIPidx)]);
			debugFIPWgoodTickLength = debugFIPtickLength - 63;
			heart::changeBitFlagBits(bfSignalSimModeFlags, 0, debugFIPready);
			break;

		case 1:
			break;

		case 2:
			debugFIPtickLength = pgm_read_word(&debugFIPvalues[(uint16_t)(debugFIPlength - debugFIPidx - 1)]);
			debugFIPWreadTickLength = pgm_read_word(&debugFIPWvalues[(uint16_t)(debugFIPlength - debugFIPidx - 1)]);
			debugFIPWgoodTickLength = debugFIPtickLength - 63;
			heart::changeBitFlagBits(bfSignalSimModeFlags, 0, debugFIPready);
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
			debugVSStickLength = pgm_read_word(&debugVSSvalues[(uint16_t)(debugVSSidx)]);
			heart::changeBitFlagBits(bfSignalSimModeFlags, 0, debugVSSready);
			break;

		case 1:
			break;

		case 2:
			debugVSStickLength = pgm_read_word(&debugVSSvalues[(uint16_t)(debugVSSlength - debugVSSidx - 1)]);
			heart::changeBitFlagBits(bfSignalSimModeFlags, 0, debugVSSready);
			break;

		case 3:
			break;

		default:
			break;

	}

}

#endif // defined(useSimulatedFIandVSS)
