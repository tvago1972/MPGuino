#ifdef useClockDisplay
 /* Big Clock Display support section */

static uint8_t clockSet::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
			bigDigit::outputTime(pBuff, (timer0Status & t0sShowCursor), cursorPos);

		default:
			break;

	}

	return retVal;

}

static void clockSet::entry(void)
{

#ifdef useSoftwareClock
	ull2str(pBuff, vClockCycleIdx, tReadTicksToSeconds);
#endif // useSoftwareClock

	cursor::moveAbsolute(clockSetDisplayIdx, 0);

}

static void clockSet::changeDigitUp(void)
{

	pBuff[(unsigned int)(displayCursor[(unsigned int)(clockSetDisplayIdx)])]++;
	if (pBuff[(unsigned int)(displayCursor[(unsigned int)(clockSetDisplayIdx)])] > '9') pBuff[(unsigned int)(displayCursor[(unsigned int)(clockSetDisplayIdx)])] = '0';

	if (pBuff[2] > '5') pBuff[2] = '0'; // this will only happen if clockSetDisplayIdx == 2
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '0'; // this will only happen if clockSetDisplayIdx == 0 or 1
	if (pBuff[0] > '2') pBuff[0] = '0'; // this will only happen if clockSetDisplayIdx == 0

}

static void clockSet::changeDigitDown(void)
{

	pBuff[(unsigned int)(displayCursor[(unsigned int)(clockSetDisplayIdx)])]--;
	if (pBuff[(unsigned int)(displayCursor[(unsigned int)(clockSetDisplayIdx)])] < '0') pBuff[(unsigned int)(displayCursor[(unsigned int)(clockSetDisplayIdx)])] = '9';

	if (pBuff[2] > '5') pBuff[2] = '5'; // this will only happen if clockSetDisplayIdx == 2
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '3'; // this will only happen if clockSetDisplayIdx == 0 or 1
	if (pBuff[0] > '2') pBuff[0] = '2'; // this will only happen if clockSetDisplayIdx == 0

}

static void clockSet::set(void)
{

#ifdef useSoftwareClock
	uint8_t b;

	pBuff[4] = '0'; // set seconds to zero
	pBuff[5] = '0';

	for (uint8_t x = 4; x < 6; x -= 2) // convert time string in pBuff into time value usable by prgmChangeSoftwareClock
	{

		b = pBuff[(unsigned int)(x)] - '0';
		b *= 10;
		b += pBuff[(unsigned int)(x + 1)] - '0';
		((union union_64 *)(&s64reg[s64reg3]))->u8[(unsigned int)(x)] = b;

	}

	SWEET64::runPrgm(prgmChangeSoftwareClock, 0); // convert time value into timer0 clock cycles

	cursor::screenLevelEntry(PSTR("Time Set"), clockShowDisplayIdx);

#endif // useSoftwareClock
}

static void clockSet::cancel(void)
{

	cursor::screenLevelEntry(PSTR("Time NOT Set"), clockShowDisplayIdx);

}

#endif // useClockDisplay
#if defined(useStatusBar)
/* Status Bar Output support section */

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
	instrBranchIfOverflow, 8,							// if division by zero attempted, skip ahead
	instrLdRegByte, 0x01, 0xFF,							// see if result is greater than 0xFF
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if register 2 contents are less than or equal to register 1 contents, skip
	instrLdReg, 0x12,									// load register 2 with contents of register 1
//ret:
	instrDone											// exit to caller
};

static uint8_t statusBar::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;
	uint8_t tripIdx = pgm_read_byte(&msTripList[(uint16_t)(cursorPos + 1)]);

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			text::statusOut(devLCD, msTripNameString, cursorPos + 1, PSTR(" vs INST" tcEOSCR));
		case menuOutputDisplayIdx:
			outputStatusBar(SWEET64::runPrgm(prgmCalculateRelativeInstVsTripFE, tripIdx));
			text::charOut(devLCD, ' ', 8);
#if defined(useSpiffyTripLabels)
			mainDisplay::outputFunction(3, (lblInstantIdx << 8 ) | (tFuelEcon), 136, 0, msTripBitPattern);
#else // defined(useSpiffyTripLabels)
			mainDisplay::outputFunction(3, (lblInstantIdx << 8 ) | (tFuelEcon), 136, 0);
#endif // defined(useSpiffyTripLabels)
			break;

		default:
			break;

	}

	return retVal;

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

			text::charOut(devLCD, oc);

		}

	}
	else
	{

		LCD::loadCGRAMfont(statusBarOverflowFont); // load initial status bar overflow custom characters

		text::charOut(devLCD, 0xF0);
		text::charOut(devLCD, 0xF2, 14);
		text::charOut(devLCD, 0xF1);
	}

	LCD::flushCGRAM(); // go output status bar custom characters

	text::newLine(devLCD);

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

#endif // defined(useStatusBar)
#ifdef useBigDigitDisplay
/* Big Digit Output support section */

static uint8_t bigDigit::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;
	uint8_t thisMenuLevel = menuLevel;
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
	uint8_t tripIdx = pgm_read_byte(&msTripList[(uint16_t)(cursorPos)]);
	char * str;
#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
#ifdef useBigFE
	uint8_t i;
#endif // useBigFE

	switch (thisMenuLevel)
	{

#ifdef useBigFE
		case bigFEdisplayIdx:
			str = PSTR(" Fuel Econ" tcEOSCR);
			break;

#endif // useBigFE
#ifdef useBigDTE
		case bigDTEdisplayIdx:
			str = PSTR(" DistToEmpty" tcEOSCR);
			break;

#endif // useBigDTE
#ifdef useBigTTE
		case bigTTEdisplayIdx:
			str = PSTR(" TimeToEmpty" tcEOSCR);
			break;

#endif // useBigTTE
		default:
#ifdef useClockDisplay
			if (cursorPos == 255) thisMenuLevel = clockShowDisplayIdx;
#endif // useClockDisplay
			break;

	}

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigFE
				case bigFEdisplayIdx:
#endif // useBigFE
#ifdef useBigDTE
				case bigDTEdisplayIdx:
#endif // useBigDTE
#ifdef useBigTTE
				case bigTTEdisplayIdx:
#endif // useBigTTE
#ifdef useClockDisplay
				case clockShowDisplayIdx:
#endif // useClockDisplay
					LCD::loadCGRAMfont(bigDigitFont);

					LCD::flushCGRAM();

					break;

				default:
					break;

			}

		case menuCursorUpdateIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigFE
				case bigFEdisplayIdx:
#endif // useBigFE
#ifdef useBigDTE
				case bigDTEdisplayIdx:
#endif // useBigDTE
#ifdef useBigTTE
				case bigTTEdisplayIdx:
#endif // useBigTTE
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
					text::statusOut(devLCD, msTripNameString, cursorPos, str);
					break;

#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
#ifdef useClockDisplay
				case clockShowDisplayIdx:
					text::statusOut(devLCD, PSTR("Clock" tcEOSCR));
					break;

#endif // useClockDisplay
				default:
					break;

			}

		case menuOutputDisplayIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigFE
				case bigFEdisplayIdx:
					i = outputNumber(tripIdx, tFuelEcon, 3) - calcFormatFuelEconomyIdx;

					text::gotoXY(devLCD, 12, 0);
					text::stringOut(devLCD, msTripNameString, cursorPos);
					text::gotoXY(devLCD, 12, 1);
					text::stringOut(devLCD, bigFElabels, i);
					break;

#endif // useBigFE
#ifdef useBigDTE
				case bigDTEdisplayIdx:
					outputNumber(tripIdx, tDistanceToEmpty, 4);
					break;

#endif // useBigDTE
#ifdef useBigTTE
				case bigTTEdisplayIdx:
					outputTime(ull2str(mBuff1, tripIdx, tTimeToEmpty), (mainLoopHeartBeat & 0b01010101), 4);
					break;

#endif // useBigTTE
#ifdef useClockDisplay
				case clockShowDisplayIdx:
					outputTime(ull2str(mBuff1, vClockCycleIdx, tReadTicksToSeconds), (mainLoopHeartBeat & 0b01010101), 4);
					break;

#endif // useClockDisplay
				default:
					break;

			}
			break;

		default:
			break;

	}

	return retVal;

}

#ifdef useBigTimeDisplay
static void bigDigit::outputTime(char * val, uint8_t blinkFlag, uint8_t blinkPos)
{

	val[4] = val[0];
	val[5] = val[1];
	val[6] = ':';
	val[7] = val[2];
	val[8] = val[3];
	val[9] = 0;

	if ((blinkFlag) == 0) // if it's time to blink something
	{

		if (blinkPos== 4) val[6] = ';'; // if hh:mm separator is selected, blink it
		else if (blinkPos < 2) val[(unsigned int)(blinkPos + 4)] = ' '; // if digit 0 or 1 is selected, blink it
		else if (blinkPos < 4) val[(unsigned int)(blinkPos + 5)] = ' '; // otherwise, if digit 2 or 3 is selected, blink it

	}

	outputNumberString(&val[4]);

}

#endif // useBigTimeDisplay
#ifdef useBigNumberDisplay
static uint8_t bigDigit::outputNumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength)
{

	calcFuncObj thisCalcFuncObj;

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, windowLength, dfIgnoreDecimalPoint); // perform the required decimal formatting
	outputNumberString(thisCalcFuncObj.strBuffer); // output the number

	return thisCalcFuncObj.calcFmtIdx; // for big MPG label

}

#endif // useBigNumberDisplay
static void bigDigit::outputNumberString(char * str)
{

	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t x;

	x = 0;
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

		outputDigit(bigDigitChars2, x, 1, c, d);
		outputDigit(bigDigitChars1, x, 0, c, e);
		x += 4;

	}

}

static void bigDigit::outputDigit(const char * digitDefStr, uint8_t xPos, uint8_t yPos, uint8_t strIdx, uint8_t endChar)
{

	text::gotoXY(devLCD, xPos, yPos);
	text::stringOut(devLCD, digitDefStr, strIdx);
	text::charOut(devLCD, endChar);

}

#endif // useBigDigitDisplay
