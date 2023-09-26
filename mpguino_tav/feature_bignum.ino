#if defined(useClockDisplay)
 /* Big Clock Display support section */

static const uint8_t prgmSetClock[] PROGMEM = {
	instrLdRegVariable, 0x02, m8HourIdx,				// load user-defined hours value
	instrMul2byByte, 60,								// multply hours value by 60 (minutes per hour)
	instrAddVariableToX, 0x02, m8MinuteIdx,				// add user-defined minutes value to time value
	instrMul2byByte, 60,								// multply time value by 60 (seconds per minute)
	instrAddVariableToX, 0x02, m8SecondIdx,				// add user-defined seconds value to time value
	instrMul2byRdOnly, idxTicksPerSecond,				// convert time value from seconds to cycles
	instrStRegVariable, 0x02, v32ClockCycleIdx,			// write software clock
	instrDone
};

static uint8_t clockDisplay::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
			text::charOut(m8DevLCDidx, 0x0C);

			LCD::loadCGRAMfont(bigDigitFont);
			LCD::flushCGRAM();

		case displayCursorUpdateIdx:
			text::statusOut(m8DevLCDidx, PSTR("Clock"));
		case displayOutputIdx:
			bigDigit::outputTime(((LCDcharWidth - 16) >> 1), ull2str(nBuff, v32ClockCycleIdx, tReadTicksToSeconds), (mainLoopHeartBeat & 0b01010101), 4, 0, 0);
			break;

		default:
			break;

	}

}

static uint8_t clockSet::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
			ull2str(csBuff, v32ClockCycleIdx, tReadTicksToSeconds);
		case displayCursorUpdateIdx:
		case displayOutputIdx:
			bigDigit::outputTime(((LCDcharWidth - 16) >> 1), csBuff, (volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] & t0saShowCursor), cursorPos, 0, 0);

		default:
			break;

	}

}

static void clockSet::entry(void)
{

	cursor::moveAbsolute(clockSetDisplayIdx, 0);

}

static void clockSet::changeDigitUp(void)
{

	csBuff[(uint16_t)(displayCursor[(uint16_t)(clockSetDisplayIdx)])]++;
	if (csBuff[(uint16_t)(displayCursor[(uint16_t)(clockSetDisplayIdx)])] > '9') csBuff[(uint16_t)(displayCursor[(uint16_t)(clockSetDisplayIdx)])] = '0';

	if (csBuff[2] > '5') csBuff[2] = '0'; // this will only happen if clockSetDisplayIdx == 2
	if ((csBuff[0] == '2') && (csBuff[1] > '3')) csBuff[1] = '0'; // this will only happen if clockSetDisplayIdx == 0 or 1
	if (csBuff[0] > '2') csBuff[0] = '0'; // this will only happen if clockSetDisplayIdx == 0

}

static void clockSet::changeDigitDown(void)
{

	csBuff[(uint16_t)(displayCursor[(uint16_t)(clockSetDisplayIdx)])]--;
	if (csBuff[(uint16_t)(displayCursor[(uint16_t)(clockSetDisplayIdx)])] < '0') csBuff[(uint16_t)(displayCursor[(uint16_t)(clockSetDisplayIdx)])] = '9';

	if (csBuff[2] > '5') csBuff[2] = '5'; // this will only happen if clockSetDisplayIdx == 2
	if ((csBuff[0] == '2') && (csBuff[1] > '3')) csBuff[1] = '3'; // this will only happen if clockSetDisplayIdx == 0 or 1
	if (csBuff[0] > '2') csBuff[0] = '2'; // this will only happen if clockSetDisplayIdx == 0

}

static void clockSet::set(void)
{

	uint8_t b;
	uint8_t v;
#if defined(useDS1307clock)
	uint8_t oldSREG;
#endif // defined(useDS1307clock)

	csBuff[4] = '0'; // set seconds to zero
	csBuff[5] = '0';

#if defined(useSoftwareClock)
	v = m8SecondIdx - m8VariableStartIdx;

	for (uint8_t x = 4; x < 6; x -= 2) // convert time string in csBuff into time value usable by prgmSetClock
	{

		b = csBuff[(uint16_t)(x)] - '0';
		b *= 10;
		b += csBuff[(uint16_t)(x + 1)] - '0';

		mainProgram8Variables[(uint16_t)(v++)] = b;

	}

	SWEET64::runPrgm(prgmSetClock, 0); // convert time value into timer0 clock cycles

#endif // defined(useSoftwareClock)
#if defined(useDS1307clock)
	TWI::disableISRactivity(); // disable ISR-based TWI activity for RTC write

	v = v8RTCsecondIdx - v8VariableStartIdx;

	for (uint8_t x = 4; x < 6; x -= 2) // convert time string in csBuff into time value usable by prgmSetClock
	{

		b = csBuff[(uint16_t)(x)] - '0';
		b *= 16;
		b += csBuff[(uint16_t)(x + 1)] - '0';

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		volatile8Variables[(uint16_t)(v++)] = b;

		SREG = oldSREG; // restore interrupt flag status

	}

	TWI::openChannelMain(TWIaddressRTC, TW_WRITE); // open TWI as master transmitter
	TWI::writeByte(0); // write out RTC seconds address
	TWI::writeByte(volatile8Variables[(uint16_t)(v8RTCsecondIdx - v8VariableStartIdx)]); // write out RTC seconds value
	TWI::writeByte(volatile8Variables[(uint16_t)(v8RTCminuteIdx - v8VariableStartIdx)]); // write out RTC minutes value
	TWI::writeByte(volatile8Variables[(uint16_t)(v8RTChourIdx - v8VariableStartIdx)]); // write out RTC hours value
	TWI::transmitChannelMain(TWI_STOP); // go write out register contents

	TWI::enableISRactivity(); // re-enable ISR-based TWI activity

#endif // defined(useDS1307clock)
	cursor::screenLevelEntry(PSTR("Time Set"), clockShowDisplayIdx);

}

static void clockSet::cancel(void)
{

	cursor::screenLevelEntry(PSTR("Time NOT Set"), clockShowDisplayIdx);

}

#if defined(useDS1307clock)
static void clockSet::setFromRTC(void)
{

	uint8_t b;
	uint8_t c;
	uint8_t oldSREG;

	for (uint8_t x = 0; x < 3; x++)
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		b = volatile8Variables[(uint16_t)(x + v8RTCsecondIdx - v8VariableStartIdx)]; // fetch a BCD value of time

		SREG = oldSREG; // restore interrupt flag status

		c = b & 0x0F; // save low digit for now
		b &= 0xF0; // strip off low digit
		b >>= 1; // divide by 2
		b *= 5; // multiply by 5
		b >>= 2; // divide again by 4 (to convert from BCD to hex, accomplishes (high digit) * 10 / 16)

		mainProgram8Variables[(uint16_t)(x + m8SecondIdx - m8VariableStartIdx)] = b + c; // store natural value of time

	}

	SWEET64::runPrgm(prgmSetClock, 0); // convert time value into timer0 clock cycles

}

#endif // defined(useDS1307clock)
#endif // defined(useClockDisplay)
#if defined(useStatusMeter)
/* Status Meter Output support section */

// normalizes instant fuel economy relative to either current or tank trip fuel economy, then converts to the range 0-255
//
static const uint8_t prgmCalculateRelativeInstVsTripFE[] PROGMEM = {
	instrLdRegTripVarIndexed, 0x02, rvVSSpulseIdx,		// fetch (trip distance)
	instrLdRegTripVar, 0x01, instantIdx, rvInjCycleIdx,	// fetch (inst quantity)
	instrMul2by1,										// calculate (inst distance) * (trip quantity) as left term
	instrSwapReg, 0x23,									// swap right term and (trip quantity) values
	instrLdRegTripVarIndexed, 0x02, rvInjCycleIdx,		// fetch (trip quantity)
	instrLdRegTripVar, 0x01, instantIdx, rvVSSpulseIdx,	// fetch (inst distance)
	instrMul2by1,										// calculate (inst quantity) * (trip distance) as right term
	instrBranchIfFuelOverDist, 2,						// if MPGuino is in fuel/dist mode, skip ahead
	instrSwapReg, 0x23,									// swap the numerator and denominator terms around

//cont:
	instrMul2byByte, 3,									// set such that if inst FE == trip FE, output 2/3
	instrLdReg, 0x21,									// move into denominator position
	instrLdReg, 0x32,									// get numerator term
	instrMul2byByte, 2,									// set such that if inst FE == trip FE, output 2/3
	instrMul2byByte, 0xFF,								// convert to range 0-255
	instrDiv2by1,										// perform fuel economy division
	instrBranchIfOverflow, 9,							// if division by zero attempted, skip ahead
	instrLdRegByte, 0x01, 0xFF,							// see if result is greater than 0xFF
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if register 2 contents are less than or equal to register 1 contents, skip
	instrLdReg, 0x12,									// load register 2 with contents of register 1
//ret:
	instrDone											// exit to caller
};

static uint8_t statusBar::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t tripIdx = pgm_read_byte(&tripFormatReverseList[(uint16_t)(cursorPos + 1)]);

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(m8DevLCDidx, PSTR("INST vs "), tripFormatReverseNames, cursorPos + 1);
		case displayOutputIdx:
			outputStatusBar(SWEET64::runPrgm(prgmCalculateRelativeInstVsTripFE, tripIdx));
			text::charOut(m8DevLCDidx, ' ', (LCDcharWidth / 2));
			mainDisplay::outputFunction(3, (instantIdx << 8 ) | (tFuelEcon), 136, 0);
			break;

		default:
			break;

	}

}

static void statusBar::outputStatusBar(uint16_t val) // takes an input number between 0 and 255, anything outside is out of range
{

	uint8_t flg;
	uint8_t ai;
	uint8_t ei;
	uint8_t oc;

	if (val < 256) // determine translated range and left endcap character
	{

		LCD::loadCGRAMfont(statusBarFont); // load initial status bar custom characters

		val *= (uint16_t)(statusBarLength);
		val /= 255;

		ei = pgm_read_byte(&statusBarPos[val]);
		ai = ei & 0x1F; // mask for getting string index
		ei >>= 5; // shift element index into position

		for (uint8_t x = 0; x < 16; x++)
		{

			flg = 0;

			if (x == 0)
			{

				oc = 0xF0;
				if (ai == 0) flg = 1;
				else oc = 0xF5;

			}
			else if (x == 15)
			{

				if (ai == 31) flg = 1;
				oc = 0xF1;

			}
			else
			{

				if (x == ai)
				{

					flg = 1;
					oc = 0xF2;

				}
				else if (x < ai) oc = 0xF4;
				else oc = 0xF3;

			}

			if (flg) writeStatusBarElement(oc, ei);

			text::charOut(m8DevLCDidx, oc);

		}

	}
	else
	{

		LCD::loadCGRAMfont(statusBarOverflowFont); // load initial status bar overflow custom characters

		text::charOut(m8DevLCDidx, 0xF0);
		text::charOut(m8DevLCDidx, 0xF2, 14);
		text::charOut(m8DevLCDidx, 0xF1);
	}

	LCD::flushCGRAM(); // go output status bar custom characters

	text::newLine(m8DevLCDidx);

}

static void statusBar::writeStatusBarElement(uint8_t chr, uint8_t val)
{

	uint8_t cgrAddress;
	uint8_t bmsk;
	uint8_t i;

	cgrAddress = ((chr & 0x07) << 3);

	bmsk = pgm_read_byte(&statusBarElement[(uint16_t)(val)]);

	for (uint8_t x = 1; x < 6; x++)
	{

		i = LCD::peekCGRAMbyte(cgrAddress + x);
		i |= (bmsk);
		LCD::writeCGRAMbyte(cgrAddress + x, i);

	}

}

#endif // defined(useStatusMeter)
#if defined(useBigDigitDisplay)
/* Big Digit Output support section */

static uint8_t bigDigit::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t tripIdx = pgm_read_byte(&tripFormatReverseList[(uint16_t)(cursorPos + 1)]);
	char * str;

	switch (cmd)
	{

		case displayInitialEntryIdx:
			LCD::loadCGRAMfont(bigDigitFont);
			LCD::flushCGRAM();

		case displayCursorUpdateIdx:
			switch (callingDisplayIdx)
			{

#if defined(useBigFE)
				case bigFEdisplayIdx:
					str = PSTR(" Fuel Econ");
					break;

#endif // defined(useBigFE)
#if defined(useBigDTE)
				case bigDTEdisplayIdx:
					str = PSTR(" DistToEmpty");
					break;

#endif // defined(useBigDTE)
#if defined(useBigTTE)
				case bigTTEdisplayIdx:
					str = PSTR(" TimeToEmpty");
					break;

#endif // defined(useBigTTE)
				default:
					break;

			}

			text::statusOut(m8DevLCDidx, tripFormatReverseNames, cursorPos, str);

		case displayOutputIdx:
			switch (callingDisplayIdx)
			{

#if defined(useBigFE)
				case bigFEdisplayIdx:
					outputNumber(tripIdx, tFuelEcon, 0, cursorPos, findStr(bigFElabels, mainCalcFuncVar.calcFmtIdx - calcFormatFuelEconomyIdx));
					break;

#endif // defined(useBigFE)
#if defined(useBigDTE)
				case bigDTEdisplayIdx:
					outputNumber(tripIdx, tDistanceToEmpty, dfOverflow9s, cursorPos, PSTR("DTE "));
					break;

#endif // defined(useBigDTE)
#if defined(useBigTTE)
				case bigTTEdisplayIdx:
					outputTime(0, ull2str(nBuff, tripIdx, tTimeToEmpty), (mainLoopHeartBeat & 0b10001000), 4, cursorPos, PSTR("TTE "));
					break;

#endif // defined(useBigTTE)
				default:
					break;

			}
			break;

		default:
			break;

	}

}

#if defined(useBigNumberDisplay)
static void bigDigit::outputNumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalFlag, uint8_t cursorPos, const char * str)
{

	uint8_t windowLength;

	windowLength = (LCDcharWidth / 4);

	translateCalcIdx(tripIdx, calcIdx, windowLength - 1, (dfIgnoreDecimalPoint | decimalFlag)); // perform the required decimal formatting

	outputNumberString(0, nBuff, cursorPos, str); // output the number

}

#endif // defined(useBigNumberDisplay)
#if defined(useBigTimeDisplay)
static void bigDigit::outputTime(uint8_t hPos, char * val, uint8_t blinkFlag, uint8_t blinkPos, uint8_t cursorPos, const char * str)
{

	val[4] = val[0];
	val[5] = val[1];
	val[6] = ':';
	val[7] = val[2];
	val[8] = val[3];
	val[9] = 0;

	if (blinkFlag) // if it's time to blink something
	{

		if (blinkPos== 4) val[6] = ';'; // if hh:mm separator is selected, blink it
		else if (blinkPos < 2) val[(uint16_t)(blinkPos + 4)] = ' '; // if digit 0 or 1 is selected, blink it
		else if (blinkPos < 4) val[(uint16_t)(blinkPos + 5)] = ' '; // otherwise, if digit 2 or 3 is selected, blink it

	}

	outputNumberString(hPos, &val[4], cursorPos, str);

}

#endif // defined(useBigTimeDisplay)
static void bigDigit::outputNumberString(uint8_t hPos, char * str, uint8_t cursorPos, const char * titleStr)
{

	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t x;

	x = hPos;
	while (*str)
	{

		c = *str++;
		d = *str;
		e = ' ';

		if ((d == '.') || (d == ':') || (d == ';'))
		{

			if (d == ':') e = '.';

			if (d == ';') d = ' ';
			else d = '.';

			str++;

		}
		else d = ' ';

		c -= '0';

		if (c == 240) c = 10;
		else if (c > 9) c = 11;

		text::gotoXY(m8DevLCDidx, x, 1);
		text::stringOut(m8DevLCDidx, bigDigitChars2, c);
		text::charOut(m8DevLCDidx, d);
		text::gotoXY(m8DevLCDidx, x, 0);
		text::stringOut(m8DevLCDidx, bigDigitChars1, c);
		text::charOut(m8DevLCDidx, e);

		x += 4;

	}

	if (((x + 4) <= LCDcharWidth) && (str))
	{

		text::gotoXY(m8DevLCDidx, x, 1);
		text::stringOut(m8DevLCDidx, titleStr);
		text::gotoXY(m8DevLCDidx, x, 0);
		text::stringOut(m8DevLCDidx, tripFormatReverseNames, cursorPos);

	}

}

#endif // defined(useBigDigitDisplay)
