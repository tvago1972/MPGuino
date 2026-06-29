#if defined(useActivityLED)
/* Activity status LED support section */

static void activityLED::init(void)
{

#if defined(__AVR_ATmega32U4__)
//	DDRB |= _BV(DDB0); // turn on digital output for RX LED
	DDRC |= _BV(DDC7); // turn on digital output for LED L
//	DDRD |= _BV(DDD5); // turn on digital output for TX LED
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	DDRB |= _BV(DDB7); // turn on digital output for LED L
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB |= _BV(DDB5); // turn on digital output for LED L
#endif // defined(__AVR_ATmega328P__)

	m08(m8ActivityStatusIdx) = 0;
	m08(m8ActivityOutputIdx) = (arMainProcess);

	output(); // initially turn off status LED

}

static void activityLED::shutdown(void)
{

	m08(m8ActivityStatusIdx) = 0;
	m08(m8ActivityOutputIdx) = 0;

	output(); // turn off status LED

#if defined(__AVR_ATmega32U4__)
//	DDRB &= ~_BV(DDB0); // turn off digital output for RX LED
	DDRC &= ~_BV(DDC7); // turn off digital output for LED L
//	DDRD &= ~_BV(DDD5); // turn off digital output for TX LED
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	DDRB &= ~_BV(DDB7); // turn off digital output for LED L
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB &= ~_BV(DDB5); // turn off digital output for LED L
#endif // defined(__AVR_ATmega328P__)

}

static void activityLED::assert(uint8_t flag)
{

	m08(m8ActivityStatusIdx) |= (flag);
	output();

}

static void activityLED::release(uint8_t flag)
{

	m08(m8ActivityStatusIdx) &= ~(flag);
	output();

}

static void activityLED::toggle(uint8_t flag)
{

	m08(m8ActivityStatusIdx) ^= (flag);
	output();

}

static void activityLED::output(void)
{

	if (m08(m8ActivityStatusIdx) & m08(m8ActivityOutputIdx))
	{

#if defined(__AVR_ATmega32U4__)
//		PORTB &= ~_BV(PORTB0); // active low RX
		PORTC |= _BV(PORTC7); // active high L
//		PORTD &= ~_BV(PORTD5); // active low TX
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		PORTB |= _BV(PORTB7); // active high L
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB |= _BV(PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
//		PORTB |= _BV(PORTB0); // active low RX
		PORTC &= ~_BV(PORTC7); // active high L
//		PORTD |= _BV(PORTD5); // active low TX
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		PORTB &= ~_BV(PORTB7); // active high L
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB &= ~_BV(PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}

}

#endif // defined(useActivityLED)
#if defined(useCPUreading) || defined(useDebugCPUreading)
/* CPU loading and RAM availability support section */

#if defined(useCPUreading) && defined(useButtonInput)
static const uint8_t prgmFindCPUutilPercent[] PROGMEM = {
	instrLdRegVariable, 0x02, m32CPUsampledMainProcessIdx,
	instrMul2byByte, 100,
	instrMul2byRdOnly, idxDecimalPoint,
	instrDiv2byVariable, m32CPUsampledMainLoopIdx,
	instrDone											// exit to caller
};

#endif // defined(useCPUreading) && defined(useButtonInput)
static const uint8_t prgmOutputOperatingTime[] PROGMEM = {
	instrLdRegVariable, 0x02, v32SystemCycleIdx,
	instrDiv2byRdOnly, idxTicks0PerSecond,
	instrLdReg, 0x21,									// move time in seconds into register 1
	instrDoBCDadjust, 0x12, bcdFormatH9MMSS,			// process register 1 as hhmmss BCD string and store it in register 2
	instrDone											// exit to caller
};

static const uint8_t prgmOutputAvailableRAM[] PROGMEM = {
	instrLdRegVariable, 0x02, m32AvailableRAMidx,
	instrMul2byRdOnly, idxDecimalPoint,
	instrDone											// exit to caller
};

#if defined(useCPUreading) && defined(useButtonInput)
static uint8_t systemInfo::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
		case displayOutputIdx: // display max cpu utilization and RAM
			showCPUload();
#if LCDcharWidth == 20
			text::stringOut(m8DevLCDidx, PSTR("     T"));
#else // LCDcharWidth == 20
			text::stringOut(m8DevLCDidx, PSTR(" T"));
#endif // LCDcharWidth == 20
			text::stringOut(m8DevLCDidx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmOutputOperatingTime))); // output system time (since MPGuino was powered up)

			text::gotoXY(m8DevLCDidx, 0, 1);
#if LCDcharWidth == 20
			text::stringOut(m8DevLCDidx, PSTR("  FREE RAM: "));
#else // LCDcharWidth == 20
			text::stringOut(m8DevLCDidx, PSTR("FREE RAM: "));
#endif // LCDcharWidth == 20
			SWEET64::runPrgm(S64_PRGM_PTR(prgmOutputAvailableRAM), 0);
			text::stringOut(m8DevLCDidx, ull2str(nBuff, 0, (LCDcharWidth / 2) - 2, 0));
			break;

		default:
			break;

	}

}

static void systemInfo::showCPUload(void)
{

	text::stringOut(m8DevLCDidx, PSTR("C%"));
	SWEET64::runPrgm(S64_PRGM_PTR(prgmFindCPUutilPercent), 0);
	text::stringOut(m8DevLCDidx, ull2str(nBuff, 2, 6, 0));

}

static void systemInfo::showCPUloading(void)
{

	text::initStatus(m8DevLCDidx);
	showCPUload();
	text::commitStatus(m8DevLCDidx);

}

#endif // defined(useCPUreading) && defined(useButtonInput)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
static uint8_t signalSim::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t i;

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(m8DevLCDidx, debugScreenFuncNames, cursorPos); // briefly display screen name

			switch (cursorPos)
			{

				case 0:
					i = (debugFIsaturatedFlag | debugInjectorFlag | debugVSSflag);
					break;

				case 1:
					i = (debugVSSflag);
					break;

				case 6:
				case 2:
					i = 0;
					break;

				case 3:
					i = (debugFIsaturatedFlag | debugInjectorFlag);
					break;

				case 4:
					i = (debugInjectorFlag | debugVSSflag);
					break;

				case 5:
					i = (debugVSSflag);
					break;

				case 7:
					i = (debugInjectorFlag);
					break;

				default:
					break;

			}

			configurePorts(i);

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
static void signalSim::configurePorts(uint8_t newMode)
{

	uint8_t oldSREG;

	newMode &= (debugEnableFlags);

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	v08(v8SignalSimModeIdx) &= ~(debugEnableFlags); // disable signal sim normal operation for VSS and fuel injector signals

	// configure VSS pin for either normal operation input or debug output
	if (newMode & debugVSSflag)
	{

#if defined(__AVR_ATmega32U4__)
		DDRB |= _BV(DDB7); // configure VSS sense pin as output
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRK |= _BV(DDK0); // configure VSS sense pin as output
		DDRA |= _BV(DDA2); // configure VSS sense pin repeater as output
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRC |= _BV(DDC0); // configure VSS sense pin as output
#endif // defined(__AVR_ATmega328P__)

		v08(v8SignalSimModeIdx) |= (debugVSSready); // tell timer0 to reset the VSS signal simulator

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
		DDRB &= ~_BV(DDB7); // configure VSS sense pin as input
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRK &= ~_BV(DDK0); // configure VSS sense pin as input
#if !defined(useArduinoMega2560)
		DDRA &= ~_BV(DDA2); // configure VSS sense pin repeater as input
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRC &= ~_BV(DDC0); // configure VSS sense pin as input
#endif // defined(__AVR_ATmega328P__)

		v08(v8SignalSimModeIdx) &= ~(debugVSSready); // tell timer0 to turn off VSS signal simulator

	}

	// configure fuel injector pins for either normal operation input or debug output
	if (newMode & debugInjectorFlag) // configure injector sense pins as outputs
	{

#if defined(__AVR_ATmega32U4__)
		DDRD |= (_BV(DDD3) | _BV(DDD2)); // configure injector sense pins as output
		PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRE |= (_BV(DDE5) | _BV(DDE4)); // configure injector sense pins as output
		PORTE |= (_BV(PORTE5) | _BV(PORTE4)); // drive injector sense pins high to simulate vehicle being initially turned on
#if !defined(useArduinoMega2560)
		DDRA |= (_BV(DDA1) | _BV(DDA0)); // configure injector sense pin repeaters as output
		PORTA |= (_BV(PORTA0)); // drive injector sense pin repeater high to simulate vehicle being initially turned on
		PORTA &= ~(_BV(PORTA1)); // drive injector sense pin repeater reference low
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRD |= (_BV(DDD3) | _BV(DDD2)); // configure injector sense pins as output
		PORTD |= (_BV(PORTD3) | _BV(PORTD2)); // drive injector sense pin high to simulate vehicle being initially turned on
#endif // defined(__AVR_ATmega328P__)

		v08(v8SignalSimModeIdx) |= (debugFIPready); // tell timer0 to reset injector signal simulator

		if (newMode & debugFIsaturatedFlag) v08(v8SignalSimModeIdx) |= (debugFIsaturatedFlag);

	}
	else // configure injector sense pins as inputs
	{

#if defined(__AVR_ATmega32U4__)
		DDRD &= ~(_BV(DDD3) | _BV(DDD2)); // configure injector sense pins as input
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
		DDRE &= ~(_BV(DDE5) | _BV(DDE4)); // configure injector sense pins as input
#if !defined(useArduinoMega2560)
		DDRA &= ~(_BV(DDA1) | _BV(DDA0)); // configure injector sense pin repeaters as input
#endif // !defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		DDRD &= ~(_BV(DDD3) | _BV(DDD2)); // configure injector sense pins as input
#endif // defined(__AVR_ATmega328P__)

		v08(v8SignalSimModeIdx) &= ~(debugFIPready); // tell timer0 to turn off injector signal simulator

	}

	if (newMode & debugOutputFlags)
	{

		v08(v8Timer1CommandIdx) |= (t1cEnableDebug);

		heart::enableTimer1Interrupt();

	}
	else v08(v8Timer1CommandIdx) &= ~(t1cEnableDebug);

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // defined(useSimulatedFIandVSS)
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
	instrLdRegRdOnlyIndexed, 0x02,
	instrDone											// exit to caller
};

static const uint8_t prgmFetchVariableValue[] PROGMEM = {
	instrLdRegVariableIndexed, 0x02,
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

static const uint8_t prgmWriteVariableValue[] PROGMEM = {
	instrStRegVariableIndexed, 0x06,
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
static uint8_t terminal::removeTerminalInputByte(void)
{

	uint8_t chr;

	if (ringBuffer::testBuffer(rbIdxTerminal, bufferIsEmpty)) return 0;

	if (ringBufferDef[(uint16_t)(rbIdxTerminal)].start) ringBufferDef[(uint16_t)(rbIdxTerminal)].start--;
	else ringBufferDef[(uint16_t)(rbIdxTerminal)].start = ringBufferDef[(uint16_t)(rbIdxTerminal)].size - 1;

	chr = ringBufferDef[(uint16_t)(rbIdxTerminal)].data[ringBufferDef[(uint16_t)(rbIdxTerminal)].start];

	ringBufferDef[(uint16_t)(rbIdxTerminal)].status &= ~(bufferIsFull);
	if (ringBufferDef[(uint16_t)(rbIdxTerminal)].start == ringBufferDef[(uint16_t)(rbIdxTerminal)].end) ringBufferDef[(uint16_t)(rbIdxTerminal)].status |= bufferIsEmpty;

	return chr;

}

static void terminal::backspaceTerminalInput(void)
{

	uint8_t chr = removeTerminalInputByte();

	if (chr)
	{

		for (uint8_t x = 0; x < ((chr > 31) ? 1 : 2); x++)
		{

			text::charOut(m8DevDebugTerminalIdx, 0x08);
			text::charOut(m8DevDebugTerminalIdx, ' ');
			text::charOut(m8DevDebugTerminalIdx, 0x08);

		}

	}

}

static void terminal::outputTripFunctionValue(uint8_t lineNumber)
{

	text::tripFunctionOut(m8DevDebugTerminalIdx, terminalIdx, lineNumber, decWindow, dfOutputLabel);
	text::charOut(m8DevDebugTerminalIdx, mainCalcFuncVar.tripChar);

}

static void terminal::outputConstantValue(uint8_t lineNumber)
{

	text::hexDWordOut(m8DevDebugTerminalIdx, SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchConstantValue), lineNumber));

#if defined(useDebugTerminalLabels)
	switch (lineNumber)
	{

		case (idxConstantStart) ... (idxConstantEnd - 1):
			labelList = terminalConstIdxNames;
			labelListOffset = idxConstantStart;
			break;

		case (pSettingsIdxStart) ... (pSettingsIdxEnd - 1):
			labelList = terminalParameterNames;
			labelListOffset = pSettingsIdxStart;
			break;

	}

#endif // defined(useDebugTerminalLabels)
}

static void terminal::outputConstantExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchConstantValue), lineNumber);
	text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmFormatToNumber)));

}

static void terminal::outputParameterValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchParameterValue), lineNumber);
	text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmFormatToNumber)));

#if defined(useDebugTerminalLabels)
	switch (lineNumber)
	{

		case (pSettingsIdxStart) ... (pSettingsIdxEnd - 1):
			labelList = terminalParameterNames;
			labelListOffset = pSettingsIdxStart;
			break;

		case (pExpandedSettingsIdxStart) ... (pExpandedSettingsIdxEnd - 1):
			labelList = terminalExpandedParameterNames;
			labelListOffset = pExpandedSettingsIdxStart;
			break;

	}

#endif // defined(useDebugTerminalLabels)
}

static void terminal::outputParameterExtra(uint8_t lineNumber)
{

	uint8_t i;

	i = EEPROM::getLength(lineNumber);

	text::hexByteOut(m8DevDebugTerminalIdx, EEPROM::getParameterFlags(lineNumber));
	text::charOut(m8DevDebugTerminalIdx, '-');
	text::hexByteOut(m8DevDebugTerminalIdx, i);
	text::charOut(m8DevDebugTerminalIdx, '-');
	text::hexWordOut(m8DevDebugTerminalIdx, EEPROM::getAddress(lineNumber));
	if (lineNumber < pSettingsIdxEnd)
	{

		text::stringOut(m8DevDebugTerminalIdx, PSTR(" (orig "));
		SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchInitialParamValue), lineNumber);
		text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmFormatToNumber)));
		text::stringOut(m8DevDebugTerminalIdx, PSTR(")"));

	}
	else if (lineNumber < eePtrEnd)
	{

		if (i & 0x07) i += 0x08;
		i >>= 3;

		text::charOut(m8DevDebugTerminalIdx, ' ');
		SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchParameterValue), lineNumber);

		for (uint8_t x = 7; x < 8; x--)
			if (x < i) text::hexByteOut(m8DevDebugTerminalIdx, ((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]))->u08[(uint16_t)(x)]);
			else text::stringOut(m8DevDebugTerminalIdx, PSTR("  "));

		text::charOut(m8DevDebugTerminalIdx, ' ');

	}

}

static void terminal::outputVariableValue(uint8_t lineNumber)
{

	switch (lineNumber)
	{

		case (v8VariableStartIdx) ... (v8VariableEndIdx - 1):
		case (m8VariableStartIdx) ... (m8VariableEndIdx - 1):
			text::charOut(m8DevDebugTerminalIdx, ' ', 14);
			text::hexByteOut(m8DevDebugTerminalIdx, SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchVariableValue), lineNumber));
			break;

		case (v16VariableStartIdx) ... (v16VariableEndIdx - 1):
			text::charOut(m8DevDebugTerminalIdx, ' ', 12);
			text::hexWordOut(m8DevDebugTerminalIdx, SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchVariableValue), lineNumber));
			break;

		case (v32VariableStartIdx) ... (v32VariableEndIdx - 1):
		case (m32VariableStartIdx) ... (m32VariableEndIdx - 1):
			text::charOut(m8DevDebugTerminalIdx, ' ', 8);
			text::hexDWordOut(m8DevDebugTerminalIdx, SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchVariableValue), lineNumber));
			break;

		case (m64VariableStartIdx) ... (m64VariableEndIdx - 1):
			SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchVariableValue), lineNumber);
			text::hexLWordOut(m8DevDebugTerminalIdx, &s64reg[(uint16_t)(s64reg64_2)]);
			break;

		default:
			text::charOut(m8DevDebugTerminalIdx, ' ', 16);
			break;

	}


}

static void terminal::outputVariableExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchVariableValue), lineNumber);
	text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmFormatToNumber)));
}

static void terminal::outputTripVarMeasuredValue(uint8_t lineNumber)
{

	if ((lineNumber == rvVSSpulseIdx) || (lineNumber == rvInjPulseIdx))
	{

		text::charOut(m8DevDebugTerminalIdx, ' ', 8);
		text::hexDWordOut(m8DevDebugTerminalIdx, SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchTripVarValue), lineNumber));

	}
	else
	{

		SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchTripVarValue), lineNumber);
		text::hexLWordOut(m8DevDebugTerminalIdx, &s64reg[(uint16_t)(s64reg64_2)]);

	}

}

static void terminal::outputTripVarMeasuredExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchTripVarValue), lineNumber);
	text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmFormatToNumber)));

}

static void terminal::outputDecimalValue(uint8_t lineNumber)
{

	SWEET64::runPrgm(S64_PRGM_PTR(prgmUpdateDecimalValue), lineNumber);
	text::hexLWordOut(m8DevDebugTerminalIdx, &s64reg[(uint16_t)(s64reg64_2)]);

}

static void terminal::outputDecimalExtra(uint8_t lineNumber)
{

	SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchDecimalValue), lineNumber);
	text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, decPlace, decWindow, decMode));

}

static void terminal::processMath(uint8_t cmd)
{

	if (terminalMode & tmTargetReadIn) decWindow = terminalTarget; // if decimal window specified, save it
	if (terminalMode & tmSourceReadIn) decPlace = terminalSource; // if decimal count specified, save it
	// save terminal register contents for later
	if ((terminalMode & tmByteReadIn) || (cmd == '_')) SWEET64::runPrgm(S64_PRGM_PTR(prgmPerformMathOperation), cmd);

}

static void terminal::outputDecimalSettings(void)
{
	text::stringOut(m8DevDebugTerminalIdx, PSTR("decimalFlags="));
	text::hexByteOut(m8DevDebugTerminalIdx, decMode);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(", windowLen="));
	text::hexByteOut(m8DevDebugTerminalIdx, decWindow);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(", places="));
	text::hexByteOut(m8DevDebugTerminalIdx, decPlace);
	text::newLine(m8DevDebugTerminalIdx);

}

static void terminal::outputFlagStatusGroup(const char * flagGroup, uint8_t flags, uint8_t labelFlag)
{

	uint8_t foundFlag = 0;

	if (labelFlag) text::stringOut(m8DevDebugTerminalIdx, flagGroup);
	text::charOut(m8DevDebugTerminalIdx, '=');

	for (uint8_t x = 0; x < 8; x++)
	{

		if (flags & 0x80)
		{

			if (foundFlag) text::charOut(m8DevDebugTerminalIdx, ',');
			text::stringOut(m8DevDebugTerminalIdx, flagGroup, x + 1);

			foundFlag++;

		}

		flags <<= 1;
		
	}

	if (foundFlag == 0) text::stringOut(m8DevDebugTerminalIdx, PSTR("none"));

}

static void terminal::outputSystemStatusFlags(void)
{

	outputFlagStatusGroup(awakeFlagMarkers, v08(v8AwakeIdx), 1);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" ACT"));
	outputFlagStatusGroup(activityFlagMarkers, v08(v8ActivityIdx), 0);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" CHG"));
	outputFlagStatusGroup(activityFlagMarkers, v08(v8ActivityChangeIdx), 0);
	text::newLine(m8DevDebugTerminalIdx);

	outputFlagStatusGroup(timer0CommandFlagMarkers, v08(v8Timer0CommandIdx), 1);
	outputFlagStatusGroup(timer0Status0FlagMarkers, v08(v8Timer0Status0Idx), 1);
	outputFlagStatusGroup(timer0Status1FlagMarkers, v08(v8Timer0Status1Idx), 1);
	text::newLine(m8DevDebugTerminalIdx);

	outputFlagStatusGroup(dirtyInjectorFlagMarkers, v08(v8DirtyInjectorIdx), 1);
	outputFlagStatusGroup(dirtyVSSflagMarkers, v08(v8DirtyVSSIdx), 1);
	text::newLine(m8DevDebugTerminalIdx);

}

static void terminal::outputSystemStatusBytes(void)
{

	text::stringOut(m8DevDebugTerminalIdx, PSTR("SYS AW="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8AwakeIdx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" ACT="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8ActivityIdx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" CHG="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8ActivityChangeIdx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" T0C="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8Timer0CommandIdx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" T0S0="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8Timer0Status0Idx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" T0S1="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8Timer0Status1Idx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" INJ="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8DirtyInjectorIdx));
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" VSS="));
	text::hexByteOut(m8DevDebugTerminalIdx, v08(v8DirtyVSSIdx));
	text::newLine(m8DevDebugTerminalIdx);

}

#if defined(useSimulatedFIandVSS)
static void terminal::outputSignalSimSetting(uint8_t lineNumber)
{

	if ((debugEnableFlags & v08(v8SignalSimModeIdx)) == terminalLine) text::charOut(m8DevDebugTerminalIdx, '*');
	else text::charOut(m8DevDebugTerminalIdx, ' ');

}

#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugTerminalSWEET64)
static void terminal::outputSWEET64programCounter(s64pc_t prgmPtr)
{

#if defined(useSWEET64RAMprograms)
	if (prgmPtr.source == s64srcRAM) text::hexByteOut(m8DevDebugTerminalIdx, SWEET64::getProgramRAMoffset(prgmPtr.ram_ptr));
    else
#endif
#if defined(__AVR__) && defined(__AVR_3_BYTE_PC__)
	text::hexDWordOut(m8DevDebugTerminalIdx, (uint32_t)prgmPtr.ptr);
#else // defined(__AVR__) && defined(__AVR_3_BYTE_PC__)
	text::hexWordOut(m8DevDebugTerminalIdx, (uint16_t)(prgmPtr.ptr));
#endif // defined(__AVR__) && defined(__AVR_3_BYTE_PC__)

#if defined(useSWEET64RAMprograms)
    text::charOut(m8DevDebugTerminalIdx, (prgmPtr.source == s64srcRAM) ? 'R' : 'P');

#endif // defined(useSWEET64RAMprograms)
}

static void terminal::outputSWEET64error(uint8_t errorCode)
{

	text::stringOut(m8DevDebugTerminalIdx, PSTR("S64 ERR "));
	text::hexByteOut(m8DevDebugTerminalIdx, errorCode);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(": "));
	if (errorCode <= s64errBadOperand) text::stringOut(m8DevDebugTerminalIdx, terminalSWEET64errorList, errorCode);
	else text::stringOut(m8DevDebugTerminalIdx, PSTR("unknown"));
	text::newLine(m8DevDebugTerminalIdx);

}

static uint8_t terminal::isSWEET64errorLatched(uint8_t errorCode, s64pc_t prgmPtr)
{

	if (terminalS64errorLatched == 0) return 0;
	if (terminalS64lastErrorCode != errorCode) return 0;
#if defined(useSWEET64RAMprograms)
	if (terminalS64lastErrorPC.source != prgmPtr.source) return 0;
	if (prgmPtr.source == s64srcRAM) return (terminalS64lastErrorPC.ram_ptr == prgmPtr.ram_ptr);
#endif // defined(useSWEET64RAMprograms)

	return (terminalS64lastErrorPC.ptr == prgmPtr.ptr);

}

static void terminal::clearSWEET64errorLatch(void)
{

	terminalS64errorLatched = 0;
	terminalS64lastErrorCode = s64errNone;
	terminalS64lastErrorCount = 0;
	terminalS64lastErrorPC = SWEET64::makeProgmemProgram(0);

}

static void terminal::outputSWEET64errorLatch(void)
{

	text::stringOut(m8DevDebugTerminalIdx, PSTR("S64 ERR "));
	if (terminalS64errorMuted) text::stringOut(m8DevDebugTerminalIdx, PSTR("[muted] "));

	if (terminalS64errorLatched)
	{

		text::stringOut(m8DevDebugTerminalIdx, PSTR("count="));
		text::hexWordOut(m8DevDebugTerminalIdx, terminalS64lastErrorCount);
		text::stringOut(m8DevDebugTerminalIdx, PSTR(" @ "));
		outputSWEET64programCounter(terminalS64lastErrorPC);
		text::stringOut(m8DevDebugTerminalIdx, PSTR(" last="));
		text::hexByteOut(m8DevDebugTerminalIdx, terminalS64lastErrorCode);
		text::charOut(m8DevDebugTerminalIdx, ' ');
		if (terminalS64lastErrorCode <= s64errBadOperand) text::stringOut(m8DevDebugTerminalIdx, terminalSWEET64errorList, terminalS64lastErrorCode);
		else text::stringOut(m8DevDebugTerminalIdx, PSTR("unknown"));

	}
	else text::stringOut(m8DevDebugTerminalIdx, PSTR("last=none"));

	text::newLine(m8DevDebugTerminalIdx);

}

static void terminal::reportSWEET64error(union union_32 * instrLWord, s64pc_t &prgmPtr, s64pc_t prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8)
{

	uint8_t errorCode = prgmReg8[(uint16_t)(si64reg8error)];

	if (errorCode == s64errNone) return;

	if (isSWEET64errorLatched(errorCode, prgmPtr))
	{

		if (terminalS64lastErrorCount < 0xFFFF) terminalS64lastErrorCount++;

	}
	else
	{
		terminalS64lastErrorCode = errorCode;
		terminalS64lastErrorCount = 1;
		terminalS64lastErrorPC = prgmPtr;
	}

	terminalS64errorLatched = 1;

	if (terminalS64errorMuted) return;

	outputSWEET64error(errorCode);
	dumpSWEET64information(instrLWord, prgmPtr, prgmStack, prgmReg64, prgmReg8);

}

static void terminal::dumpSWEET64information(union union_32 * instrLWord, s64pc_t &prgmPtr, s64pc_t prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8)
{

	outputSWEET64programCounter(prgmPtr);
	text::charOut(m8DevDebugTerminalIdx, ' ');
	text::hexDWordOut(m8DevDebugTerminalIdx, instrLWord->u32);
	outputFlagStatusGroup(SWEET64processorFlagMarkers, SWEET64processorFlags, 1);
	text::newLine(m8DevDebugTerminalIdx);

	for (uint8_t x = 0; x < s64stackSize; x++)
	{

		hexByteOut(m8DevDebugTerminalIdx, x);

		if ((x < prgmReg8[(uint16_t)(si64reg8spnt)]) && (prgmReg8[(uint16_t)(si64reg8spnt)] < s64stackSize))
		{

			outputSWEET64programCounter(prgmStack[(uint16_t)(x)]);
			hexByteOut(m8DevDebugTerminalIdx, s64callIndexStack[(uint16_t)(x)]);
#if defined(useDebugTerminalLabels)
			text::stringOut(m8DevDebugTerminalIdx, terminalTripFuncNames, s64callIndexStack[(uint16_t)(x)]);
#endif // defined(useDebugTerminalLabels)

		}

		text::newLine(m8DevDebugTerminalIdx);

	}

	text::newLine(m8DevDebugTerminalIdx);

	for (uint8_t x = 0; x < s64reg64count; x++)
	{

		hexByteOut(m8DevDebugTerminalIdx, x);
		text::hexLWordOut(m8DevDebugTerminalIdx, &prgmReg64[(uint16_t)(x)]);

#if defined(useDebugTerminalLabels)
		text::charOut(m8DevDebugTerminalIdx, ' ');
		text::stringOut(m8DevDebugTerminalIdx, terminalSWEET64registerLabels, x);

#endif // defined(useDebugTerminalLabels)
		text::newLine(m8DevDebugTerminalIdx);

	}

	text::newLine(m8DevDebugTerminalIdx);

	for (uint8_t x = 0; x < si64reg8count; x++)
	{

		hexByteOut(m8DevDebugTerminalIdx, x);
		text::hexByteOut(m8DevDebugTerminalIdx, prgmReg8[(uint16_t)(x)]);

#if defined(useDebugTerminalLabels)
		text::charOut(m8DevDebugTerminalIdx, ' ');
		text::stringOut(m8DevDebugTerminalIdx, terminalSWEET64registerLabels, x + s64reg64count);

#endif // defined(useDebugTerminalLabels)
		text::newLine(m8DevDebugTerminalIdx);

	}

	text::newLine(m8DevDebugTerminalIdx);

}

static void terminal::hexByteOut(uint8_t devIdx, uint8_t val)
{

	text::charOut(devIdx, 9);
	text::hexByteOut(devIdx, val);
	text::charOut(devIdx, ' ');

}

#if defined(useDebugTerminalLabels)
static void terminal::outputSWEET64functionLength(uint8_t lineNumber)
{

	text::hexWordOut(m8DevDebugTerminalIdx, SWEET64::getProgramLength(lineNumber));

}

#endif // defined(useDebugTerminalLabels)
static void terminal::outputSWEET64registerContents(uint8_t lineNumber)
{

	if (lineNumber < s64reg64count) text::hexLWordOut(m8DevDebugTerminalIdx, &terminalS64reg64[(uint16_t)(lineNumber)]);
	else
	{

		text::charOut(m8DevDebugTerminalIdx, ' ', 14);
		text::hexByteOut(m8DevDebugTerminalIdx, terminalS64reg8[(uint16_t)(lineNumber - s64reg64count)]);

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

		SWEET64::runPrgm(S64_PRGM_PTR(prgmLoadByteValue), terminalS64reg8[(uint16_t)(lineNumber - s64reg64count)]);
		i = 0;

	}

	text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, i, decWindow, decMode));

}

static void terminal::outputSWEET64byte(uint8_t byt)
{

	text::charOut(m8DevDebugTerminalIdx, ' ');
	text::hexByteOut(m8DevDebugTerminalIdx, byt);

}

#if defined(useSWEET64RAMprograms)
static void terminal::outputSWEET64programRAMline(uint8_t lineNumber)
{

	text::hexByteOut(m8DevDebugTerminalIdx, lineNumber);
	text::charOut(m8DevDebugTerminalIdx, 'R');
	text::charOut(m8DevDebugTerminalIdx, ':');

	uint16_t byteCount = ((uint8_t)(s64programRAMdumpEnd - lineNumber)) + 1;
	if (byteCount > 8) byteCount = 8;

	for (uint8_t x = 0; x < byteCount; x++)
	{

		text::charOut(m8DevDebugTerminalIdx, ' ');
		text::hexByteOut(m8DevDebugTerminalIdx, SWEET64::readProgramRAM((uint8_t)(lineNumber + x)));

	}

	text::newLine(m8DevDebugTerminalIdx);

}

static void terminal::outputSWEET64programRAMoverride(void)
{

	text::stringOut(m8DevDebugTerminalIdx, PSTR("S64 RAM override="));

	if (SWEET64::isProgramRAMoverrideEnabled())
	{

		text::hexByteOut(m8DevDebugTerminalIdx, SWEET64::getProgramRAMoverrideIndex());
		text::stringOut(m8DevDebugTerminalIdx, PSTR("<-"));
		text::hexByteOut(m8DevDebugTerminalIdx, SWEET64::getProgramRAMoverrideAddress());

	}
	else text::stringOut(m8DevDebugTerminalIdx, PSTR("off"));

	text::newLine(m8DevDebugTerminalIdx);

}

static uint8_t terminal::getSWEET64instructionByteCount(uint8_t opCode, uint8_t format)
{

	uint8_t instrByteCount = 1;

	if (opCode >= maxValidSWEET64instr) return instrByteCount;

	switch (format & rxxMask)
	{

		case r01:
		case r04:
		case r06:
		case r07:
			instrByteCount++;
			break;

		default:
			break;

	}

	switch (format & pxxMask)
	{

		case p01:
		case p03:
			instrByteCount++;
			break;

		default:
			break;

	}

	if ((format & sxxMask) == s01) instrByteCount++;

	return instrByteCount;

}

static void terminal::outputSWEET64sourceByte(uint8_t byt)
{

	text::stringOut(m8DevDebugTerminalIdx, PSTR("0x"));
	text::hexByteOut(m8DevDebugTerminalIdx, byt);

}

static uint8_t terminal::outputSWEET64sourceLabelByte(uint8_t byt, uint8_t labelIdx)
{

#if defined(useDebugTerminalLabels)
	uint8_t flg;
	uint8_t typ;
	uint8_t len;
	const char * labelList;

	if (labelIdx)
	{

		typ = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType);

		do
		{

			flg = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType);
			if (flg != typ) return 0;

			len = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelLength);

			if (byt < len)
			{

				labelList = (const char *)(pgm_read_word(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelStringPointer));
				text::stringOut(m8DevDebugTerminalIdx, labelList, byt);
				return 1;

			}

			byt -= len;
			labelIdx++;

		}
		while (labelIdx < dslIdxEnd);

	}

#endif // defined(useDebugTerminalLabels)
	return 0;

}

static uint8_t terminal::outputSWEET64programRAMsourceLine(uint8_t lineNumber)
{

	uint8_t opCode;
	uint8_t format;
	uint8_t instr;
	uint16_t opcodeWord;
	uint8_t instrByteCount;
	uint8_t operandCount = 0;
	uint8_t byt;
	uint8_t x;
	uint8_t validSourceInstruction = 0;
#if defined(useDebugTerminalLabels)
	uint8_t operandLabelIdx;
	uint8_t extraLabelIdx;
#endif // defined(useDebugTerminalLabels)

	opCode = SWEET64::readProgramRAM(lineNumber);

	if (opCode < maxValidSWEET64instr)
	{

		opcodeWord = pgm_read_word(&opcodeFetchWord[(uint16_t)(opCode)]);
		instr = opcodeWord;
		format = opcodeWord >> 8;
		instrByteCount = getSWEET64instructionByteCount(opCode, format);

		if ((uint8_t)(s64programRAMdumpEnd - lineNumber) >= (uint8_t)(instrByteCount - 1)) validSourceInstruction = 1;
		else instrByteCount = 1;

	}
	else instrByteCount = 1;

	text::charOut(m8DevDebugTerminalIdx, 0x09);

	if (validSourceInstruction)
	{

		text::stringOut(m8DevDebugTerminalIdx, PSTR("instr"));
		text::stringOut(m8DevDebugTerminalIdx, opCodeList, opCode);

#if defined(useDebugTerminalLabels)
		getSWEET64operandLabelIndexes(instr, format, operandLabelIdx, extraLabelIdx);
#endif // defined(useDebugTerminalLabels)

		switch (format & rxxMask)
		{

			case r01:
			case r04:
			case r06:
			case r07:
				text::stringOut(m8DevDebugTerminalIdx, PSTR(", "));
				outputSWEET64sourceByte(SWEET64::readProgramRAM((uint8_t)(lineNumber + (++operandCount))));
				break;

			default:
				break;

		}

		switch (format & pxxMask)
		{

			case p01:
			case p03:
				text::stringOut(m8DevDebugTerminalIdx, PSTR(", "));
				byt = SWEET64::readProgramRAM((uint8_t)(lineNumber + (++operandCount)));
#if defined(useDebugTerminalLabels)
				if (outputSWEET64sourceLabelByte(byt, operandLabelIdx) == 0)
#endif // defined(useDebugTerminalLabels)
					outputSWEET64sourceByte(byt);
				break;

			default:
				break;

		}

		if ((format & sxxMask) == s01)
		{

			text::stringOut(m8DevDebugTerminalIdx, PSTR(", "));
			byt = SWEET64::readProgramRAM((uint8_t)(lineNumber + (++operandCount)));
#if defined(useDebugTerminalLabels)
			if ((instr < eMaxBranchInstrIdx) || (outputSWEET64sourceLabelByte(byt, extraLabelIdx) == 0))
#endif // defined(useDebugTerminalLabels)
				outputSWEET64sourceByte(byt);

		}

	}
	else
	{

		for (x = 0; x < instrByteCount; x++)
		{

			if (x) text::stringOut(m8DevDebugTerminalIdx, PSTR(", "));
			outputSWEET64sourceByte(SWEET64::readProgramRAM((uint8_t)(lineNumber + x)));

		}

	}

	text::charOut(m8DevDebugTerminalIdx, ',');
	text::newLine(m8DevDebugTerminalIdx);
	return instrByteCount;

}

static uint8_t terminal::chrEqualIgnoreCase(uint8_t inputChar, uint8_t storedChar)
{

	uint8_t diff = inputChar ^ storedChar;

	if (diff == 0) return 1;
	if (diff != 0x20) return 0;

	inputChar |= 0x20;
	return ((inputChar >= 'a') && (inputChar <= 'z'));

}

static uint8_t terminal::matchSWEET64opcodeAlias(char * token, const char * opCodePtr)
{

	uint8_t tokenIdx = 0;
	uint8_t storedChar;
	uint8_t inputChar;
	uint8_t sawAliasChar = 0;

	for (;;)
	{

		storedChar = pgm_read_byte(opCodePtr++);
		inputChar = token[(uint16_t)(tokenIdx)];

		if ((storedChar == '/') || (storedChar == 0))
		{

			if ((inputChar == 0) && (sawAliasChar)) return 1;
			if (storedChar == 0) return 0;

			tokenIdx = 0;
			sawAliasChar = 0;
			continue;

		}

		sawAliasChar = 1;

		if ((inputChar == 0) || (chrEqualIgnoreCase(inputChar, storedChar) == 0))
		{

			while ((storedChar != '/') && (storedChar != 0)) storedChar = pgm_read_byte(opCodePtr++);
			if (storedChar == 0) return 0;

			tokenIdx = 0;
			sawAliasChar = 0;
			continue;

		}

		tokenIdx++;

	}

}

static uint8_t terminal::findSWEET64opcode(char * token)
{

	for (uint8_t x = 0; x < maxValidSWEET64instr; x++)
		if (matchSWEET64opcodeAlias(token, findStr(opCodeList, x))) return x;

	return maxValidSWEET64instr;

}

static uint8_t terminal::pullSWEET64assemblerToken(void)
{

	uint8_t tokenIdx = 0;
	uint8_t chr;

	while (ringBuffer::testBufferNot(rbIdxTerminal, bufferIsEmpty))
	{

		chr = ringBuffer::pull(rbIdxTerminal);

		if ((chr == ' ') || (chr == ','))
		{

			if (tokenIdx) break;
			else continue;

		}

		if (tokenIdx >= (sizeof(s64programRAMassemblerToken) - 1))
		{

			while (ringBuffer::testBufferNot(rbIdxTerminal, bufferIsEmpty)) ringBuffer::pull(rbIdxTerminal);
			return 2;

		}

		s64programRAMassemblerToken[(uint16_t)(tokenIdx++)] = chr;

	}

	if (tokenIdx == 0) return 0;
	s64programRAMassemblerToken[(uint16_t)(tokenIdx)] = 0;
	return 1;

}

static uint8_t terminal::parseSWEET64assemblerByte(char * token, uint8_t &byt)
{

	uint8_t chr;
	uint8_t digit;
	uint8_t digitCount = 0;

	byt = 0;

	if ((token[0] == '0') && (chrEqualIgnoreCase(token[1], 'x'))) token += 2;

	while (*token)
	{

		chr = *token++;

		if ((chr >= '0') && (chr <= '9')) digit = chr - '0';
		else
		{

			chr |= 0x20;
			if ((chr >= 'a') && (chr <= 'f')) digit = (chr - 'a') + 10;
			else return 0;

		}

		if (digitCount >= 2) return 0;

		byt <<= 4;
		byt |= digit;
		digitCount++;

	}

	return digitCount;

}

#if defined(useDebugTerminalLabels)
static uint8_t terminal::findSWEET64labelByte(char * token, uint8_t labelIdx, uint8_t &byt)
{

	uint8_t typ;
	uint8_t flg;
	uint8_t len;
	const char * labelList;

	if (labelIdx == 0) return 0;

	byt = 0;
	typ = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType);

	do
	{

		flg = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType);
		if (flg != typ) return 0;

		labelList = (const char *)(pgm_read_word(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelStringPointer));
		len = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelLength);

		for (uint8_t x = 0; x < len; x++)
			if (matchSWEET64opcodeAlias(token, findStr(labelList, x)))
			{

				byt += x;
				return 1;

			}

		byt += len;
		labelIdx++;

	}
	while (labelIdx < dslIdxEnd);

	return 0;

}

#endif // defined(useDebugTerminalLabels)

static uint8_t terminal::pullSWEET64assemblerOperand(uint8_t &byt, uint8_t labelIdx)
{

	if (pullSWEET64assemblerToken() != 1) return 0;
	if (parseSWEET64assemblerByte(s64programRAMassemblerToken, byt)) return 1;
#if defined(useDebugTerminalLabels)
	if (findSWEET64labelByte(s64programRAMassemblerToken, labelIdx, byt)) return 1;
#endif // defined(useDebugTerminalLabels)

	return 0;

}

static uint8_t terminal::assembleSWEET64programRAMline(void)
{

	uint8_t opCode;
	uint8_t format;
	uint8_t instr;
	uint16_t opcodeWord;
	uint8_t operandCount = 0;
	uint8_t operands[3];
	int16_t relativeOffset;
#if defined(useDebugTerminalLabels)
	uint8_t operandLabelIdx;
	uint8_t extraLabelIdx;
#endif // defined(useDebugTerminalLabels)

	if (pullSWEET64assemblerToken() != 1) return 0;

	opCode = findSWEET64opcode(s64programRAMassemblerToken);
	if (opCode >= maxValidSWEET64instr) return 0;

	opcodeWord = pgm_read_word(&opcodeFetchWord[(uint16_t)(opCode)]);
	instr = opcodeWord;
	format = opcodeWord >> 8;
#if defined(useDebugTerminalLabels)
	getSWEET64operandLabelIndexes(instr, format, operandLabelIdx, extraLabelIdx);
#endif // defined(useDebugTerminalLabels)

	switch (format & rxxMask)
	{

		case r01:
		case r04:
			if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount)], 0) == 0) return 0;
			if (((operands[(uint16_t)(operandCount)] & 0x70) || (operands[(uint16_t)(operandCount)] & 0x07)) && ((operands[(uint16_t)(operandCount)] & 0x88) == 0)) operandCount++;
			else return 0;
			break;

		case r06:
			if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount)], 0) == 0) return 0;
			if ((operands[(uint16_t)(operandCount)] & 0x07) && ((operands[(uint16_t)(operandCount)] & 0xF8) == 0)) operandCount++;
			else return 0;
			break;

		case r07:
			if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount)], 0) == 0) return 0;
			if ((operands[(uint16_t)(operandCount)] & 0x77) && ((operands[(uint16_t)(operandCount)] & 0x88) == 0)) operandCount++;
			else return 0;
			break;

		default:
			break;

	}

	switch (format & pxxMask)
	{

		case p01:
		case p03:
#if defined(useDebugTerminalLabels)
			if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount++)], operandLabelIdx) == 0) return 0;
#else // defined(useDebugTerminalLabels)
			if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount++)], 0) == 0) return 0;
#endif // defined(useDebugTerminalLabels)
			break;

		default:
			break;

	}

	if ((format & sxxMask) == s01)
	{

#if defined(useDebugTerminalLabels)
		if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount)], extraLabelIdx) == 0) return 0;
#else // defined(useDebugTerminalLabels)
		if (pullSWEET64assemblerOperand(operands[(uint16_t)(operandCount)], 0) == 0) return 0;
#endif // defined(useDebugTerminalLabels)

		if (instr < eMaxBranchInstrIdx)
		{

			relativeOffset = (int16_t)(operands[(uint16_t)(operandCount)]) - (int16_t)(s64programRAMassemblerAddr + 1 + operandCount + 1);
			if ((relativeOffset < -128) || (relativeOffset > 127)) return 0;
			operands[(uint16_t)(operandCount)] = (uint8_t)(relativeOffset);

		}

		operandCount++;

	}

	if (pullSWEET64assemblerToken() != 0) return 0;

	if ((uint16_t)(s64programRAMassemblerAddr) + 1 + operandCount > SWEET64::getProgramRAMsize()) return 0; // reject the line if it would overflow program RAM, instead of silently wrapping and overwriting earlier instructions

	clearSWEET64errorLatch();
	SWEET64::writeProgramRAM(s64programRAMassemblerAddr++, opCode);
	for (uint8_t x = 0; x < operandCount; x++) SWEET64::writeProgramRAM(s64programRAMassemblerAddr++, operands[(uint16_t)(x)]);

	return 1;

}

#endif // defined(useSWEET64RAMprograms)
#if defined(useDebugTerminalLabels)
static void terminal::getSWEET64operandLabelIndexes(uint8_t instr, uint8_t format, uint8_t &operandLabelIdx, uint8_t &extraLabelIdx)
{

	operandLabelIdx = 0;
	extraLabelIdx = 0;

	if ((format & rxxMask) != r00) // instruction does something with the 64 bit registers
	{

		switch (instr & ixxMask) // perform load or store operation, according to ixx
		{

			case i14:	// load rX with const
				operandLabelIdx = dslIdxConst;
				break;

			case i03:	// load rX with EEPROM
			case i04:	// store EEPROM rX
				operandLabelIdx = dslIdxEEPROM;
				break;

			case i07:	// load rX with volatile
			case i08:	// store volatile rX
				operandLabelIdx = dslIdxProgramVariable;
				break;

#if defined(useFEvTdata)
			case i17:	// load rX with FEvT trip variable
#endif // defined(useFEvTdata)
			case i18:	// load rX with trip variable
			case i19:	// store trip variable rX
				operandLabelIdx = dslIdxTripVariable;
				extraLabelIdx = dslIdxTripMeasurement;
				break;

			case i31:	// BCD adjust
				operandLabelIdx = dslIdxBCDformat;
				break;

			default:
				break;

		}

	}
	else
	{

		if (instr >= eMaxBranchInstrIdx)
		{

			switch (instr)
			{

				case e29:	// load jump register
				case e27:	// call
				case e28:	// jump
					extraLabelIdx = dslIdxFunction;
					break;

				case e24:	// load index EEPROM
				case e26:	// load index EEPROM parameter length
					extraLabelIdx = dslIdxEEPROM;
					break;

				default:
					break;

			}

		}

	}

}

#endif // defined(useDebugTerminalLabels)
static void terminal::outputSWEET64operand(uint8_t flag, uint8_t &byt)
{

	switch (flag)
	{

		case (s64vReadInOperandByte):
		case (s64vReadInExtraByte):
		case (s64vReadInOperandByte | s64vOperandIndexed):
		case (s64vReadInExtraByte | s64vExtraIndexed):
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (0x"));
			text::hexByteOut(m8DevDebugTerminalIdx, (byt++) * 17);
			if (flag & (s64vOperandIndexed | s64vExtraIndexed)) text::stringOut(m8DevDebugTerminalIdx, PSTR(" + tripIdx"));
			text::charOut(m8DevDebugTerminalIdx, ')');
			break;

		case (s64vOperandIndexed):
		case (s64vExtraIndexed):
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (tripIdx)"));
			break;

		case (s64vReadInExtraByte | s64vRelativeOperand):
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (+/-0x"));
			text::hexByteOut(m8DevDebugTerminalIdx, (byt++) * 17);
			text::charOut(m8DevDebugTerminalIdx, ')');
			break;

		case (s64vExtraJump):
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (jumpReg)"));
			break;

		default:
			break;

	}

}

static void terminal::outputSWEET64opcode(uint8_t lineNumber)
{

	uint32_t instrLWord;
	union union_32 * iLW = (union union_32 *)(&instrLWord);
	uint8_t formatBytes[4];
	s64pc_t prgmPtr;
	uint8_t isValid;
	uint8_t i;

	formatBytes[0] = lineNumber; // opcode to be decoded
	formatBytes[1] = formatBytes[2] = formatBytes[3] = 0x01; // placeholder bytes that satisfy every register-operand validity check in fetchInstruction, so the opcode's format is decoded without tripping a bogus program error
	prgmPtr = SWEET64::makeRAMprogram(formatBytes);

	SWEET64::fetchInstruction(iLW, prgmPtr, terminalS64reg8); // decode instruction
	isValid = terminalS64reg8[(uint16_t)(si64reg8valid)];

	if (isValid)
	{

		i = 1;

		text::charOut(m8DevDebugTerminalIdx, '(');
		text::hexWordOut(m8DevDebugTerminalIdx, iLW->u16[0]);
		text::charOut(m8DevDebugTerminalIdx, ')');

		outputSWEET64byte(lineNumber); // output opcode byte

		if ((isValid & (s64vRegisterOperation | s64vReadInRegisterByte)) == (s64vRegisterOperation | s64vReadInRegisterByte)) outputSWEET64byte((i++) * 17); // output nominal 64-bit register operand byte

		if (isValid & s64vReadInOperandByte) outputSWEET64byte((i++) * 17); // output nominal primary operand byte

		if (isValid & s64vReadInExtraByte) outputSWEET64byte((i++) * 17); // output nominal extra opcode byte

		text::charOut(m8DevDebugTerminalIdx, ' ', 3 * (5 - i)); // tab over to instruction column of output

		text::stringOut(m8DevDebugTerminalIdx, opCodeList, lineNumber); // output opcode mnemonic

		i = 1;

		if ((isValid & (s64vRegisterOperation | s64vReadInRegisterByte)) == (s64vRegisterOperation | s64vReadInRegisterByte)) // output nominal 64-bit register operand
		{

			text::stringOut(m8DevDebugTerminalIdx, PSTR(" 0x"));
			text::hexByteOut(m8DevDebugTerminalIdx, (i++) * 17);

		}

		outputSWEET64operand(isValid & (s64vReadInOperandByte | s64vOperandIndexed), i); // output nominal primary operand

		outputSWEET64operand(isValid & (s64vReadInExtraByte | s64vExtraIndexed | s64vRelativeOperand | s64vExtraJump), i); // output nominal extra operand

	}
	else text::stringOut(m8DevDebugTerminalIdx, PSTR("INVALID INSTRUCTION"));

}

#if defined(useDebugTerminalLabels)
static void terminal::outputSWEET64prgmOperand(s64pc_t prgmPtr, uint8_t flag, uint8_t operandIdx, uint8_t labelIdx)
#else // defined(useDebugTerminalLabels)
static void terminal::outputSWEET64prgmOperand(s64pc_t prgmPtr, uint8_t flag, uint8_t operandIdx)
#endif // defined(useDebugTerminalLabels)
{

	uint16_t tgt;
	uint8_t byt;
	uint8_t flg;
	uint8_t typ;

	byt = s64operands[(uint16_t)(operandIdx)];

	switch (flag)
	{

		case (s64vReadInOperandByte):
		case (s64vReadInExtraByte):
		case (s64vReadInOperandByte | s64vOperandIndexed):
		case (s64vReadInExtraByte | s64vExtraIndexed):
#if defined(useDebugTerminalLabels)
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" ("));
			if (labelIdx)
			{

				typ = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType);

				do
				{

					flg = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType);
					if (flg != typ) break; // crossed into a different label group - byt does not belong to any entry here

					flg = pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelLength);
					if (byt < flg) break; // found the entry byt belongs to

					byt -= flg;
					labelIdx++;

				}
				while (labelIdx < dslIdxEnd);

				if ((pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelType) == typ) && (byt < pgm_read_byte(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelLength)))
				{

					text::stringOut(m8DevDebugTerminalIdx, (const char *)(pgm_read_word(&debugSWEET64labelList[(uint16_t)(labelIdx - 1)].labelStringPointer)), byt);

				}
				else text::stringOut(m8DevDebugTerminalIdx, PSTR("* INVALID *"));

			}
			else
			{

				text::stringOut(m8DevDebugTerminalIdx, PSTR("0x"));
				text::hexByteOut(m8DevDebugTerminalIdx, byt);

			}

#else // defined(useDebugTerminalLabels)
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (0x"));
			text::hexByteOut(m8DevDebugTerminalIdx, byt);

#endif // defined(useDebugTerminalLabels)
			if (flag & (s64vOperandIndexed | s64vExtraIndexed)) text::stringOut(m8DevDebugTerminalIdx, PSTR(" + tripIdx"));
			text::charOut(m8DevDebugTerminalIdx, ')');
			break;

		case (s64vOperandIndexed):
		case (s64vExtraIndexed):
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (tripIdx)"));
			break;

		case (s64vReadInExtraByte | s64vRelativeOperand):
			SWEET64::addProgramOffset(prgmPtr, byt);
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" $"));
			outputSWEET64programCounter(prgmPtr);
			break;

		case (s64vExtraJump):
			text::stringOut(m8DevDebugTerminalIdx, PSTR(" (jumpReg)")); // output jumpReg implied operand
			break;

		default:
			break;

	}

}

static void terminal::outputSWEET64prgmLine(union union_32 * instrLWord, s64pc_t &prgmPtr, uint8_t traceFlag)
{

	s64pc_t oldSched;
	uint8_t instrByteCount = 1;
	uint8_t isValid;
	uint8_t opCode;
	uint8_t reg;
	uint8_t i;
#if defined(useDebugTerminalLabels)
	uint8_t operandLabelIdx;
	uint8_t extraLabelIdx;
#endif // defined(useDebugTerminalLabels)

	oldSched = prgmPtr;
	opCode = SWEET64::readProgramByte(oldSched);
	oldSched = prgmPtr;

	SWEET64::fetchInstruction(instrLWord, prgmPtr, terminalS64reg8); // decode instruction
	isValid = terminalS64reg8[(uint16_t)(si64reg8valid)];

	if (opCode < maxValidSWEET64instr)
	{

		switch (instrLWord->u08[1] & rxxMask)
		{

			case r01:
			case r04:
			case r06:
			case r07:
				instrByteCount++;
				break;

			default:
				break;

		}

		switch (instrLWord->u08[1] & pxxMask)
		{

			case p01:
			case p03:
				instrByteCount++;
				break;

			default:
				break;

		}

		if ((instrLWord->u08[1] & sxxMask) == s01) instrByteCount++;

	}

	if ((isValid == 0) && (opCode < maxValidSWEET64instr))
	{

		prgmPtr = oldSched;
		SWEET64::addProgramOffset(prgmPtr, instrByteCount);

	}

	if (traceFlag)
	{

		text::charOut(m8DevDebugTerminalIdx, ' ');
		outputSWEET64programCounter(oldSched);
		text::charOut(m8DevDebugTerminalIdx, '-');

		for (uint8_t x = 0; x < 5; x++)
		{

			i = SWEET64::readProgramByte(oldSched);
			if (x == 0) opCode = i;
			if (x == 1) reg = i;

			if (x < instrByteCount) outputSWEET64byte(i); // output opcode byte
			else text::charOut(m8DevDebugTerminalIdx, ' ', 3);

		}

		if (terminalS64reg8[(uint16_t)(si64reg8valid)])
		{

#if defined(useDebugTerminalLabels)
			getSWEET64operandLabelIndexes(instrLWord->u08[0], instrLWord->u08[1], operandLabelIdx, extraLabelIdx);
#endif // defined(useDebugTerminalLabels)
			text::stringOut(m8DevDebugTerminalIdx, opCodeList, opCode); // output opcode mnemonic

			if ((isValid & (s64vRegisterOperation | s64vReadInRegisterByte)) == (s64vRegisterOperation | s64vReadInRegisterByte)) // output nominal 64-bit register operand
			{

				text::stringOut(m8DevDebugTerminalIdx, PSTR(" 0x"));
				text::hexByteOut(m8DevDebugTerminalIdx, reg);

			}

#if defined(useDebugTerminalLabels)
			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInOperandByte | s64vOperandIndexed), s64oprPrimary, operandLabelIdx); // output primary operand

			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInExtraByte | s64vExtraIndexed | s64vRelativeOperand | s64vExtraJump), s64oprExtra, extraLabelIdx); // output extra operand

#else // defined(useDebugTerminalLabels)
			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInOperandByte | s64vOperandIndexed), s64oprPrimary); // output primary operand

			outputSWEET64prgmOperand(prgmPtr, isValid & (s64vReadInExtraByte | s64vExtraIndexed | s64vRelativeOperand | s64vExtraJump), s64oprExtra); // output extra operand

#endif // defined(useDebugTerminalLabels)
		}
		else
		{

			if (opCode < maxValidSWEET64instr)
			{

				text::stringOut(m8DevDebugTerminalIdx, opCodeList, opCode);
				text::stringOut(m8DevDebugTerminalIdx, PSTR(" * INVALID OPERAND *"));

			}
			else text::stringOut(m8DevDebugTerminalIdx, PSTR("?????"));

		}

		text::newLine(m8DevDebugTerminalIdx);

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

		f = ringBuffer::testBufferNot(rbIdxBLEfriendIn, bufferIsEmpty);

		if (f)
		{

			c = ringBuffer::pull(rbIdxBLEfriendIn);

			if (c >= 0x20) i = 1; // if this is a printable character, signal to end this function with a newline
			else i = 0; // otherwise, suppress the newline at the end of the function

			if (c == 0x0D) text::newLine(m8DevDebugTerminalIdx);
			else if (c == 0x0A) c = 0;
			else text::charOut(m8DevDebugTerminalIdx, c);

			if (ringBuffer::free(rbIdxBLEfriendIn) > 15) // is there enough room in the input buffer for another packet?
			{

				// if IRQ is still pulled high, there's another packet to be read in
				if (blefriend::inputCheck()) blefriend::readPacket();

			}

		}

	}
	while (f);

	if (i) text::newLine(m8DevDebugTerminalIdx);

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

       [y].[x]V - list program variables, optionally between [y] and [x]
xV:y [y] [y]... - store one or more y values, starting at program variable x

       [y].[x]T - list terminal trip variable values, optionally between [y] and [x]
xT:y [y] [y]... - store one or more y values, starting at terminal trip variable x

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
           S - lists available signal simulator modes
          yS - sets signal simulator mode to y
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
			text::stringOut(m8DevDebugTerminalIdx, tseErrorStringList, errIdx);
		case tsBell:		// ring terminal bell
			text::charOut(m8DevDebugTerminalIdx, 0x07);
		case tsInitInput:	// initialize user line input, output prompt
			text::newLine(m8DevDebugTerminalIdx);
			text::charOut(m8DevDebugTerminalIdx, ']');
			ringBuffer::empty(rbIdxTerminal);
			terminalState = tsUserInput;
			errIdx = tseIdxLineCancel;
#if defined(useDebugCPUreading)
			m08(m8PeekFlags) |= (peekEnableCPUread); // enable supplemental CPU time measurements
#endif // defined(useDebugCPUreading)

		case tsUserInput:	// get line
			do
			{

				i = text::chrIn(m8DevDebugTerminalIdx); // read in a possible input character

				if (i) // if an actual input character is read in
				{

					switch (i)
					{

						case 0x0D:	// enter
							text::charOut(m8DevDebugTerminalIdx, 0x0D);
							heart::changeBitFlagBits(v8Timer0CommandIdx, 0, t0cResetInputActivityTimer); // tell timer0 that some user input was received
							terminalState = tsInitTerminalCmd;
							break;

						case 0x03:	// cancel line input
						case 0x18:	// cancel line input
							terminalState = tsError;
							break;

						case 0x08:	// backspace
						case 0x7F:	// delete
							backspaceTerminalInput();
							break;

#if defined(useDebugTerminalSWEET64)
						case 0x05:	// display SWEET64 register contents
#if defined(useDebugTerminalLabels)
						case 0x06:	// display complete list of available SWEET64 functions
#endif // defined(useDebugTerminalLabels)
						case 0x09:	// list SWEET64 opcodes and their operands
						case 0x0C:	// list 20 lines of SWEET64 pseudo-code
#if defined(useSWEET64RAMprograms)
						case 0x0F:	// enable/disable SWEET64 program RAM override
						case 0x17:	// export SWEET64 program RAM as source
#endif // defined(useSWEET64RAMprograms)
						case 0x14:	// trace 1 or more lines of SWEET64 pseudo-code
#endif // defined(useDebugTerminalSWEET64)
						case 0x13:	// display supplemental system information
						case 0x20 ... 0x7E:	// unspecified input character
							if (ringBuffer::testBuffer(rbIdxTerminal, bufferIsFull)) terminalState = tsError;
							else
							{

								ringBuffer::push(rbIdxTerminal, i);

								if (i > 31)	text::charOut(m8DevDebugTerminalIdx, i);
								else
								{

									text::charOut(m8DevDebugTerminalIdx, '^');
									text::charOut(m8DevDebugTerminalIdx, i + 64);

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
			prgmPtr = SWEET64::makeProgmemProgram(0);
#if defined(useDebugTerminalLabels)
			labelList = 0;
			labelListOffset = 0;
#endif // defined(useDebugTerminalLabels)
			errIdx = 0;
			terminalState = tsProcessCommand;
			nextTerminalState = tsProcessCommand;

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

					if (ringBuffer::testBuffer(rbIdxTerminal, bufferIsEmpty)) chr = 0x0D;
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
									terminalByte = SWEET64::runPrgm(S64_PRGM_PTR(prgmParseHexDigit), chr);
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
									terminalByte = SWEET64::runPrgm(S64_PRGM_PTR(prgmParseDecimalDigit), chr);
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

									if (terminalByte & longButtonBit)
									{

										terminalByte &= ~(longButtonBit);
										m08(m8ButtonFlags) = (btnCmdProcessButton | btnStatusDetectLongPress);

									}
									else m08(m8ButtonFlags) = (btnCmdProcessButton | btnStatusDetectShortPress);

									button::inject(terminalByte & buttonMask); // inject the parsed button press value into timer0

									terminalState = tsInjectButtonPress;
									terminalMode |= (tmInitInput); // signal to go parse another input value

								}
								else
								{

#if defined(useTWIbuttons) || defined(useAnalogButtons)
									heart::changeBitFlagBits(v8ButtonStatusIdx, 0, btnCmdEnableSampling); // re-enable sensor-based button sampling
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
									terminalState = i; // no button group was read in, so cancel button injection mode

								}
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
#if defined(useSWEET64RAMprograms)
							case 'm':	// enter a SWEET64 program RAM byte
								if (terminalMode & tmByteReadIn) // if a byte value was read in
								{

									clearSWEET64errorLatch();
									SWEET64::writeProgramRAM(terminalAddress++, terminalByte);

									if (terminalAddress)
									{

										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmHexInput | tmInitInput); // shift to reading a new hexadecimal byte

									}
									else terminalState = i; // if byte address wrapped, reset command

								}
								else terminalState = i; // no byte value was read in, so cancel program RAM byte entry mode
								break;

#endif // defined(useSWEET64RAMprograms)
							case 'p':   // enter a stored parameter value
								if (terminalMode & tmByteReadIn) // if a parameter value was read in
								{

									EEPROM::onChange(S64_PRGM_PTR(prgmTerminalWriteParameterValue), terminalAddress++);

									if (terminalAddress < maxLine)
									{

										terminalMode &= ~(tmInputMask); // clear input mode processing bits
										terminalMode |= (tmDecimalInput | tmInitInput); // shift to reading a new decimal value

									}
									else terminalState = i; // if at end of storage range, reset command

								}
								else terminalState = i; // no parameter value was read in, so cancel parameter value entry mode
								break;

							case 't':   // enter a trip variable measurement value
							case 'v':   // enter a program variable value
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

							case 0:		// no input received
							case '+':	// add
							case '-':	// subtract
							case '*':	// multiply
							case '/':	// divide
#if defined(useIsqrt)
							case '_':	// square root
#endif // defined(useIsqrt)
							case '=':	// output last result
								processMath(terminalCmd);

								SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchResultValue), 0);
								outputDecimalSettings();
								text::charOut(m8DevDebugTerminalIdx, '=');
								text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, decPlace, decWindow, decMode));
								text::stringOut(m8DevDebugTerminalIdx, PSTR(" (0x"));
								text::hexLWordOut(m8DevDebugTerminalIdx, &s64reg[(uint16_t)(s64reg64_7)]);
								text::stringOut(m8DevDebugTerminalIdx, PSTR(")" tcCR));

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

#if defined(useSWEET64RAMprograms)
							case 'M':	// list SWEET64 program RAM bytes
								if ((terminalMode & tmTargetReadIn) && (terminalMode & tmByteReadIn))
								{

									uint16_t prgmLength = SWEET64::getProgramLength(terminalByte);

									if (terminalMode & tmSourceReadIn) errIdx = tseIdxSyntax;
									else if ((prgmLength == 0) || (prgmLength > SWEET64::getProgramRAMsize())) errIdx = tseIdxBadSWEET64addr;
									else if (prgmLength > (SWEET64::getProgramRAMsize() - terminalTarget)) errIdx = tseIdxTargetVal;
									else
									{

										s64pc_t src = SWEET64::makeProgmemProgram(SWEET64::getProgramPointer(terminalByte));
										uint8_t dst = terminalTarget;

										clearSWEET64errorLatch();
										while (prgmLength--) SWEET64::writeProgramRAM(dst++, SWEET64::readProgramByte(src));

										terminalState = i;

									}

									break;

								}
								else if (terminalMode & tmTargetReadIn)
								{

									if (terminalMode & tmSourceReadIn) errIdx = tseIdxSyntax;
									else
									{

										clearSWEET64errorLatch();
										SWEET64::fillProgramRAM(terminalTarget);

										terminalState = i;

									}

									break;

								}
								else if (terminalMode & tmSourceReadIn)
								{

									terminalLine = terminalSource;
									if (terminalMode & tmByteReadIn) s64programRAMdumpEnd = terminalByte;
									else s64programRAMdumpEnd = 255;

									maxLine = (((uint8_t)(s64programRAMdumpEnd - terminalSource)) >> 3) + 1;

								}
								else
								{

									if (terminalMode & tmByteReadIn)
									{

										terminalLine = terminalByte;
										s64programRAMdumpEnd = terminalByte;
										maxLine = 1;

									}
									else
									{

										terminalLine = 0;
										s64programRAMdumpEnd = 255;
										maxLine = 32;

									}

								}

								terminalState = tsOutputSWEET64RAMline;
								nextTerminalState = i;
								break;

#endif // defined(useSWEET64RAMprograms)
#endif // defined(useDebugTerminalSWEET64)
							case 'P':   // list available stored parameters
								primaryFunc = terminal::outputParameterValue;
								extraFunc = terminal::outputParameterExtra;
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

							case 'V':   // list available program variables
#if defined(useDebugCPUreading)
								m08(m8PeekFlags) &= ~(peekEnableCPUread);

#endif // defined(useDebugCPUreading)
#if defined(useDebugTerminalLabels)
								labelList = terminalVariableLabels;
#endif // defined(useDebugTerminalLabels)
								primaryFunc = terminal::outputVariableValue;
								extraFunc = terminal::outputVariableExtra;
								terminalState = tsInitList;
								nextTerminalState = i;
								break;

							default:	// unrecognized command - do nothing
								terminalState = nextTerminalState;
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
#if defined(useTWIbuttons) || defined(useAnalogButtons)
									heart::changeBitFlagBits(v8ButtonStatusIdx, btnCmdEnableSampling, 0); // disable sensor-based button sampling
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
									chr = 'i';
									terminalMode = (tmInitButton); // shift to reading button press words
									break;

#endif // defined(useDebugButtonInjection)
								case 0x13:	// display supplemental system information
#if defined(useCPUreading) || defined(useDebugCPUreading)
									text::stringOut(m8DevDebugTerminalIdx, PSTR("UP "));
									text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmOutputOperatingTime)));
									text::newLine(m8DevDebugTerminalIdx);
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useClockDisplay)
									text::stringOut(m8DevDebugTerminalIdx, PSTR("CLK "));
									text::stringOut(m8DevDebugTerminalIdx, ull2str(nBuff, 0, S64_PRGM_PTR(prgmOutputClockTime)));
									text::newLine(m8DevDebugTerminalIdx);
#endif // defined(useClockDisplay)
									outputDecimalSettings();
									outputSystemStatusFlags();
									outputSystemStatusBytes();

#if defined(useSWEET64RAMprograms)
									outputSWEET64programRAMoverride();

#endif // defined(useSWEET64RAMprograms)
#if defined(useDebugTerminalSWEET64)
									outputSWEET64errorLatch();

#endif // defined(useDebugTerminalSWEET64)
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

											case 'p':   // enter a stored parameter value
											case 't':   // enter a trip variable measurement value
											case 'v':   // enter a program variable value
												terminalMode |= (tmInitDecimal); // shift to decimal input
												if (terminalAddress >= maxLine) errIdx = tseIdxAddressVal;
												break;

#if defined(useDebugTerminalSWEET64)
											case 0x25:	// enter a SWEET64 register value
#endif // defined(useDebugTerminalSWEET64)
#if defined(useSWEET64RAMprograms)
											case 'm':	// enter a SWEET64 program RAM byte
#endif // defined(useSWEET64RAMprograms)
											case 's':	// enter a bitflag register value
												terminalMode |= (tmInitHex); // shift to hexadecimal input
#if defined(useSWEET64RAMprograms)
												if ((chr != 'm') && (terminalAddress >= maxLine)) errIdx = tseIdxAddressVal;
#else // defined(useSWEET64RAMprograms)
												if (terminalAddress >= maxLine) errIdx = tseIdxAddressVal;
#endif // defined(useSWEET64RAMprograms)
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

#if defined(useDebugTerminalSWEET64)
								case 'N':	// toggle SWEET64 error output mute
									terminalS64errorMuted ^= 1;
									terminalState = tsInitProcessing;
									break;

#endif // defined(useDebugTerminalSWEET64)
#if defined(useTFToutput)
								case 'G':	// G: cycle TFT rotation; <0..3> G: set a specific rotation
									if (terminalMode & tmByteReadIn) TFT::setRotation(terminalByte); // jump to rotation 0..3
									else TFT::setRotation(tftRotation + 1); // no arg: advance to the next rotation
									// setRotation cleared the screen; draw the primitives + padding test
									TFT::drawTestScreen();
									terminalState = tsInitProcessing;
									break;
#endif // defined(useTFToutput)

#if defined(useTouchScreenInput) && defined(useTFToutput)
								case 'H':	// H: touch-screen test (crosshair on the TFT, raw coords to terminal)
									touch::testLoop();
									terminalState = tsInitProcessing;
									break;

								case 'J':	// J: 4-corner touch calibration (writes pTouchRaw* EEPROM params)
									touch::calibrate();
									TFT::clearScreen();
									terminalState = tsInitProcessing;
									break;

								case 'K':	// K: on-screen numeric keypad test (entered value to terminal)
									{

										uint32_t kpValue;

										if (keypad::getNumber(&kpValue, 0xFFFFFFFFul))
										{

											text::stringOut(m8DevDebugTerminalIdx, PSTR("keypad entered 0x"));
											text::hexDWordOut(m8DevDebugTerminalIdx, kpValue);
											text::newLine(m8DevDebugTerminalIdx);

										}
										else text::stringOut(m8DevDebugTerminalIdx, PSTR("keypad cancelled" tcCR));

									}
									TFT::clearScreen();
									terminalState = tsInitProcessing;
									break;
#endif // defined(useTouchScreenInput) && defined(useTFToutput)

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
									if (terminalMode & tmTargetReadIn) // if target byte read in, select one of 2 pre-defined ranges
									{

										if (terminalMode & (tmSourceReadIn | tmByteReadIn)) errIdx = tseIdxSyntax;
										else
										{

											terminalMode |= (tmSourceReadIn | tmByteReadIn);

											switch (terminalTarget)
											{

												case 0:	// select all initial stored parameter settings
													terminalSource = pSettingsIdxStart;
													terminalByte = pSettingsIdxEnd - 1;
													break;

												case 1:	// select all program constants
													terminalSource = idxConstantStart;
													terminalByte = idxConstantEnd - 1;
													break;

												default:
													errIdx = tseIdxSyntax;
													break;

											}

										}

									}

									maxLine = idxMaxConstant;
									primaryFunc = terminal::outputConstantValue;
									extraFunc = terminal::outputConstantExtra;
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

											SWEET64::runPrgm(S64_PRGM_PTR(prgmLoadTrip), terminalByte); // this allows direct loading/saving of trips to EEPROM
											SWEET64::runPrgm(S64_PRGM_PTR(prgmSaveTrip), terminalTarget);

											terminalState = tsInitProcessing;

#if defined(useDebugTerminalLabels)
											text::stringOut(m8DevDebugTerminalIdx, terminalTripVarNames, terminalByte);
											text::stringOut(m8DevDebugTerminalIdx, PSTR(" -> "));
											text::stringOut(m8DevDebugTerminalIdx, terminalTripVarNames, terminalTarget);
											text::newLine(m8DevDebugTerminalIdx);

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
									primaryFunc = terminal::outputSWEET64functionLength;
									terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

#endif // defined(useDebugTerminalLabels)
								case 0x09:	// list SWEET64 opcodes and their operands
									maxLine = maxValidSWEET64instr;
									primaryFunc = terminal::outputSWEET64opcode;
									terminalState = tsInitListReadOnly; // this command could print a lot of different lines, so handle this command one iteration at a time
									break;

#if defined(useSWEET64RAMprograms)
								case '!':	// enter SWEET64 program RAM assembler
									if (terminalMode & (tmSourceReadIn | tmTargetReadIn)) errIdx = tseIdxSyntax;
									else
									{

										if (terminalMode & tmByteReadIn) s64programRAMassemblerAddr = terminalByte;
										else s64programRAMassemblerAddr = 0;

										terminalState = tsInitSWEET64assembler;

									}

									break;

								case 0x0F:	// enable/disable SWEET64 program RAM override
									if ((terminalMode & tmTargetReadIn) && (terminalMode & tmByteReadIn))
									{

										uint16_t prgmLength = SWEET64::getProgramLength(terminalByte);

										if (terminalMode & tmSourceReadIn) errIdx = tseIdxSyntax;
										else if ((prgmLength == 0) || (prgmLength > SWEET64::getProgramRAMsize())) errIdx = tseIdxBadSWEET64addr;
										else if (prgmLength > (SWEET64::getProgramRAMsize() - terminalTarget)) errIdx = tseIdxTargetVal;
										else
										{

											clearSWEET64errorLatch();
											SWEET64::enableProgramRAMoverride(terminalByte, terminalTarget);
											terminalState = tsInitProcessing;

										}

									}
									else if (terminalMode & tmReadInMask) errIdx = tseIdxSyntax;
									else
									{

										SWEET64::disableProgramRAMoverride();
										terminalState = tsInitProcessing;

									}

									break;

								case 0x17:	// export SWEET64 program RAM as source
									if ((terminalMode & tmTargetReadIn) || ((terminalMode & (tmSourceReadIn | tmByteReadIn)) != (tmSourceReadIn | tmByteReadIn))) errIdx = tseIdxSyntax;
									else if (terminalSource > terminalByte) errIdx = tseIdxSourceVal;
									else
									{

										terminalLine = terminalSource;
										s64programRAMdumpEnd = terminalByte;
										text::stringOut(m8DevDebugTerminalIdx, PSTR("static const uint8_t prgmRAMexport[] PROGMEM = {" tcCR));
										terminalState = tsOutputSWEET64RAMsourceLine;

									}

									break;

#endif // defined(useSWEET64RAMprograms)
								case 0x14:	// trace 1 or more lines of SWEET64 pseudo-code
									if (terminalMode & tmTargetReadIn)
									{

										if (terminalMode & tmSourceReadIn) errIdx = tseIdxSyntax;
										else
										{

#if defined(useSWEET64RAMprograms)
											terminalExecSched = SWEET64::makeRAMprogram(terminalTarget);
#else // defined(useSWEET64RAMprograms)
											terminalExecSched = SWEET64::getProgramPC(terminalTarget);
#endif // defined(useSWEET64RAMprograms)

											if (!SWEET64::isProgramValid(terminalListSched)) terminalListSched = terminalExecSched;

											terminalS64reg8[(uint16_t)(si64reg8flags)] = SWEET64traceFlagGroup; // initialize terminal SWEET64 flags
											terminalS64reg8[(uint16_t)(si64reg8spnt)] = 0; // initialize terminal SWEET64 stack pointer
											terminalS64reg8[(uint16_t)(si64reg8jump)] = 0; // initialize terminal SWEET64 jump register
											terminalS64reg8[(uint16_t)(si64reg8error)] = s64errNone; // initialize terminal SWEET64 error code
											clearSWEET64errorLatch(); // this is a new trace run, so do not let it be suppressed by a stale latched error from a previous run

										}

									}
									else if (terminalMode & tmSourceReadIn)
									{

										terminalExecSched = SWEET64::getProgramPC(terminalSource);

										if (!SWEET64::isProgramValid(terminalListSched)) terminalListSched = terminalExecSched;

										terminalS64reg8[(uint16_t)(si64reg8flags)] = SWEET64traceFlagGroup; // initialize terminal SWEET64 flags
										terminalS64reg8[(uint16_t)(si64reg8spnt)] = 0; // initialize terminal SWEET64 stack pointer
										terminalS64reg8[(uint16_t)(si64reg8jump)] = 0; // initialize terminal SWEET64 jump register
										terminalS64reg8[(uint16_t)(si64reg8error)] = s64errNone; // initialize terminal SWEET64 error code
										clearSWEET64errorLatch(); // this is a new trace run, so do not let it be suppressed by a stale latched error from a previous run

									}
									if (terminalMode & tmByteReadIn) maxLine = terminalByte;
									else maxLine = 1;

#if defined(useDebugCPUreading)
									m32(m32S64programCyclesIdx) = 0; // reset per-run execution counters
									m32(m32S64programInstrIdx) = 0;
#endif // defined(useDebugCPUreading)

									if (SWEET64::isProgramValid(terminalExecSched)) terminalState = tsTraceSWEET64line;
									else errIdx = tseIdxBadSWEET64addr;

									break;

								case 0x0C:	// list 20 lines of SWEET64 pseudo-code
#if defined(useSWEET64RAMprograms)
									if (terminalMode & tmTargetReadIn) terminalListSched = SWEET64::makeRAMprogram(terminalTarget);
									else
#endif // defined(useSWEET64RAMprograms)
									if (terminalMode & tmByteReadIn) terminalListSched = SWEET64::getProgramPC(terminalByte);

									maxLine = 20;

									if (SWEET64::isProgramValid(terminalListSched)) terminalState = tsOutputSWEET64line;
									else errIdx = tseIdxBadSWEET64addr;

									break;

								case 0x05:	// list SWEET64 register contents, with optional register value storage
									maxLine = s64reg64count + si64reg8count;
									break;

#if defined(useSWEET64RAMprograms)
								case 'M':	// access SWEET64 program RAM
									break;

#endif // defined(useSWEET64RAMprograms)
#endif // defined(useDebugTerminalSWEET64)
#if defined(useSimulatedFIandVSS)
								case 'S':   // list available signal simulator mode values, with optional mode setting
									if (terminalMode & tmByteReadIn)
									{

										signalSim::configurePorts(terminalByte & debugEnableFlags);

#if defined(useDebugTerminalLabels)
										text::stringOut(m8DevDebugTerminalIdx, terminalSignalSimHelp, terminalByte & debugEnableFlags);
										text::newLine(m8DevDebugTerminalIdx);

#endif // defined(useDebugTerminalLabels)
										terminalState = tsInitProcessing;

									}
									else
									{

										primaryFunc = terminal::outputSignalSimSetting;
										maxLine = 8;
#if defined(useDebugTerminalLabels)
										labelList = terminalSignalSimHelp;
#endif // defined(useDebugTerminalLabels)
										terminalState = tsProcessList;
										nextTerminalState = tsInitProcessing;

									}

									break;

#endif // defined(useSimulatedFIandVSS)
								case 'P':   // list available stored parameters, with optional stored parameter value storage
									if (terminalMode & tmTargetReadIn) // if target byte read in, select a pre-defined range
									{

										if (terminalMode & (tmSourceReadIn | tmByteReadIn)) errIdx = tseIdxSyntax;
										else
										{

											terminalMode |= (tmSourceReadIn | tmByteReadIn);

											switch (terminalTarget)
											{

												case 0:
													terminalSource = pSettingsIdxStart;
													terminalByte = pSettingsIdxEnd - 1;
													break;

#if defined(useEEPROMtripStorage)
												case 1:	// select all saved trip information parameters
													terminalSource = eePtrSavedTripsStart;
													terminalByte = eePtrSavedTripsEnd - 1;
													break;

#endif // defined(useEEPROMtripStorage)
#if defined(useScreenEditor)
												case 2:	// select all user-editable main display format parameters
													terminalSource = eePtrDisplayPagesStart;
													terminalByte = eePtrDisplayPagesEnd - 1;
													break;

#endif // defined(useScreenEditor)
#if defined(useButtonInput)
												case 3:	// select all screen cursor parameters
													terminalSource = eePtrDisplayCursorStart;
													terminalByte = eePtrDisplayCursorEnd - 1;
													break;

												case 4:	// select all menu height position parameters
													terminalSource = eePtrMenuHeightStart;
													terminalByte = eePtrMenuHeightEnd - 1;
													break;

#endif // defined(useButtonInput)
												default:
													errIdx = tseIdxSyntax;
													break;

											}

										}

									}

									maxLine = eePtrEnd;
									break;

								case 'T':   // list available trip variable measurements, with optional trip variable value storage
									maxLine = rvMeasuredCount;
									prgmPtr = SWEET64::makeProgmemProgram(S64_PRGM_PTR(prgmWriteTripMeasurementValue));
									break;

								case 'V':   // list available program variables, with optional program variable value storage
									if (terminalMode & tmTargetReadIn) // if target byte read in, select one of 6 pre-defined ranges
									{

										if (terminalMode & (tmSourceReadIn | tmByteReadIn)) errIdx = tseIdxSyntax;
										else
										{

											terminalMode |= (tmSourceReadIn | tmByteReadIn);

											switch (terminalTarget)
											{

												case 0:	// select all 8-bit volatile variables
													terminalSource = v8VariableStartIdx;
													terminalByte = v8VariableEndIdx - 1;
													break;

												case 1:	// select all 8-bit main program variables
													terminalSource = m8VariableStartIdx;
													terminalByte = m8VariableEndIdx - 1;
													break;

												case 2:	// select all 16-bit volatile variables
													terminalSource = v16VariableStartIdx;
													terminalByte = v16VariableEndIdx - 1;
													break;

												case 4:	// select all 32-bit volatile variables
													terminalSource = v32VariableStartIdx;
													terminalByte = v32VariableEndIdx - 1;
													break;

												case 5:	// select all 32-bit main program variables
													terminalSource = m32VariableStartIdx;
													terminalByte = m32VariableEndIdx - 1;
													break;

												case 7:	// select all 64-bit main program variables
													terminalSource = m64VariableStartIdx;
													terminalByte = m64VariableEndIdx - 1;
													break;

												default:
													errIdx = tseIdxSyntax;
													break;

											}

										}

									}

									maxLine = programVariableMaxIdx;
									prgmPtr = SWEET64::makeProgmemProgram(S64_PRGM_PTR(prgmWriteVariableValue));
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
			text::stringOut(m8DevDebugTerminalIdx, terminalHelp, terminalLine++);
			if (pgm_read_byte(findStr(terminalHelp, terminalLine)) == 0) terminalState = tsInitProcessing;
			break;

#endif // defined(useDebugTerminalHelp)
#if defined(useBluetoothAdaFruitSPI)
		case tsOutputBLEfriend:
			i = 0;

			while (ringBuffer::testBufferNot(rbIdxTerminal, bufferIsEmpty))
			{

				i = ringBuffer::pull(rbIdxTerminal);
				if (m08(m8PeekFlags) & peekBLEfriendEcho) text::charOut(m8DevDebugTerminalIdx, i);
				text::charOut(m8DevBLEfriendIdx, i);
				i = 1;

			}

			if (i)
			{

				if (m08(m8PeekFlags) & peekBLEfriendEcho) text::newLine(m8DevDebugTerminalIdx);

				blefriend::outputBufferWithResponse();

				outputBluetoothResponse();

			}

			terminalState = tsInitInput;
			break;

#endif // defined(useBluetoothAdaFruitSPI)
#if defined(useDebugButtonInjection)
		case tsInjectButtonPress:	// wait for injected buttonpress to be accepted into timer0
			if (v08(v8ButtonStatusIdx) & m08(m8ButtonFlags)) break;

			button::inject(buttonsUp); // inject a buttons-up press into timer0
			terminalState = tsInjectButtonsUp;

			break;

		case tsInjectButtonsUp:	// wait for injected buttons-up status to be accepted into timer0
			if (v08(v8ButtonStatusIdx) & (btnCmdProcessButton | btnStatusDetectShortPress)) break;

#if defined(useTWIbuttons) || defined(useAnalogButtons)
			if (nextTerminalState != tsProcessCommand) heart::changeBitFlagBits(v8ButtonStatusIdx, 0, btnCmdEnableSampling);
#endif // defined(useTWIbuttons) || defined(useAnalogButtons)
			terminalState = nextTerminalState;

			break;

#endif // defined(useDebugButtonInjection)
#if defined(useDebugTerminalSWEET64)
		case tsTraceSWEET64line:	// trace one or more lines of SWEET64 program
			if ((SWEET64::isProgramValid(terminalExecSched)) && (terminalState == tsTraceSWEET64line))
			{

				// decode instruction, and output if trace flag is enabled
				outputSWEET64prgmLine(iLW, terminalExecSched, (terminalS64reg8[(uint16_t)(si64reg8flags)] & SWEET64traceFlag));

				if (terminalS64reg8[(uint16_t)(si64reg8valid)])
				{

#if defined(useDebugCPUreading)
					uint32_t s64execStartCycle = heart::cycles0(); // measure execution cycles only, excluding trace output
#endif // defined(useDebugCPUreading)
					SWEET64::executeInstruction(iLW, terminalExecSched, terminalStack, terminalS64reg64, terminalS64reg8); // execute instruction
#if defined(useDebugCPUreading)
					m32(m32S64programCyclesIdx) += (heart::cycles0() - s64execStartCycle);
					m32(m32S64programInstrIdx)++;
#endif // defined(useDebugCPUreading)

					if (terminalS64reg8[(uint16_t)(si64reg8valid)] == 0)
					{

						reportSWEET64error(iLW, terminalExecSched, terminalStack, terminalS64reg64, terminalS64reg8);
						terminalExecSched = SWEET64::makeProgmemProgram(0);

					}
					else if (terminalS64reg8[(uint16_t)(si64reg8flags)] & SWEET64traceFlag) // if trace flag is still enabled, output register values
						dumpSWEET64information(iLW, terminalExecSched, terminalStack, terminalS64reg64, terminalS64reg8);

				}
				else
				{

					reportSWEET64error(iLW, terminalExecSched, terminalStack, terminalS64reg64, terminalS64reg8);
					terminalExecSched = SWEET64::makeProgmemProgram(0);

				}

				if (maxLine)
				{

					maxLine--;
					if (maxLine == 0) terminalState = tsInitProcessing;

				}

			}

			if (!SWEET64::isProgramValid(terminalExecSched)) terminalState = tsInitProcessing;

			break;

		case tsOutputSWEET64line:	// output program listing
			outputSWEET64prgmLine(iLW, terminalListSched, 1);

			if ((--maxLine) == 0) terminalState = tsInitProcessing;

			break;

#if defined(useSWEET64RAMprograms)
		case tsOutputSWEET64RAMline:	// output SWEET64 program RAM bytes
			outputSWEET64programRAMline(terminalLine);
			terminalLine += 8;

			if ((--maxLine) == 0) terminalState = nextTerminalState;

			break;

		case tsOutputSWEET64RAMsourceLine:	// output SWEET64 program RAM as source
			i = outputSWEET64programRAMsourceLine(terminalLine);

			if ((terminalLine >= s64programRAMdumpEnd) || (i > (uint8_t)(s64programRAMdumpEnd - terminalLine)))
			{

				text::stringOut(m8DevDebugTerminalIdx, PSTR("};" tcCR));
				terminalState = tsInitProcessing;

			}
			else terminalLine += i;

			break;

		case tsInitSWEET64assembler:	// initialize SWEET64 program RAM assembler input
			text::newLine(m8DevDebugTerminalIdx);
			text::hexByteOut(m8DevDebugTerminalIdx, s64programRAMassemblerAddr);
			text::stringOut(m8DevDebugTerminalIdx, PSTR("!:"));
			ringBuffer::empty(rbIdxTerminal);
			terminalState = tsSWEET64assemblerInput;
			errIdx = tseIdxLineCancel;
			break;

		case tsSWEET64assemblerInput:	// get SWEET64 program RAM assembler line
			do
			{

				i = text::chrIn(m8DevDebugTerminalIdx);

				if (i)
				{

					switch (i)
					{

						case 0x0D:	// enter
							text::charOut(m8DevDebugTerminalIdx, 0x0D);
							heart::changeBitFlagBits(v8Timer0CommandIdx, 0, t0cResetInputActivityTimer);

							if (ringBuffer::testBuffer(rbIdxTerminal, bufferIsEmpty)) terminalState = tsInitInput;
							else
							{

								uint8_t assembledAddr = s64programRAMassemblerAddr;

								if (assembleSWEET64programRAMline())
								{

									s64pc_t assembledLine = SWEET64::makeRAMprogram(assembledAddr);

									outputSWEET64prgmLine(iLW, assembledLine, 1);
									terminalState = tsInitSWEET64assembler;

								}
								else
								{

									ringBuffer::empty(rbIdxTerminal);
									errIdx = tseIdxSyntax;
									terminalState = tsError;

								}

							}

							break;

						case 0x03:	// cancel line input
						case 0x18:	// cancel line input
							terminalState = tsInitInput;
							break;

						case 0x08:	// backspace
						case 0x7F:	// delete
							backspaceTerminalInput();
							break;

						case 0x20 ... 0x7E:
							if (ringBuffer::testBuffer(rbIdxTerminal, bufferIsFull)) terminalState = tsError;
							else
							{

								ringBuffer::push(rbIdxTerminal, i);
								text::charOut(m8DevDebugTerminalIdx, i);

							}
							break;

						default:
							break;

					}

				}

			}
			while ((i) && (terminalState == tsSWEET64assemblerInput));

			break;

#endif // defined(useSWEET64RAMprograms)
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
			text::hexByteOut(m8DevDebugTerminalIdx, terminalLine);

			if (primaryFunc)
			{

				text::stringOut(m8DevDebugTerminalIdx, separatorPtr);
				separatorPtr = terminalSecondarySeparator;
				primaryFunc(terminalLine);

			}
			if (extraFunc)
			{

				text::stringOut(m8DevDebugTerminalIdx, separatorPtr);
				separatorPtr = terminalSecondarySeparator;
				extraFunc(terminalLine);

			}
#if defined(useDebugTerminalLabels)
			if (labelList)
			{

				text::stringOut(m8DevDebugTerminalIdx, separatorPtr);
				separatorPtr = terminalSecondarySeparator;
				text::stringOut(m8DevDebugTerminalIdx, labelList, terminalLine - labelListOffset);

			}
#endif // defined(useDebugTerminalLabels)
			text::newLine(m8DevDebugTerminalIdx);

			terminalLine++;

			if (terminalLine >= maxLine) terminalState = nextTerminalState; // go back to read another command

			break;

	}

}

#endif // defined(useDebugTerminal)
