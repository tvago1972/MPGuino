#ifdef useClockDisplay
 /* Big Clock Display support section */

static void clockSet::entry(void)
{

#ifdef useSoftwareClock
	ull2str(tReadTicksToSeconds, pBuff, vClockCycleIdx);
#endif // useSoftwareClock

	cursor::moveAbsolute(clockSetScreenIdx, 0);

}

static void clockSet::changeDigitUp(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])]++;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] > '9') pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] = '0';

	if (pBuff[2] > '5') pBuff[2] = '0'; // this will only happen if clockSetScreenIdx == 2
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '0'; // this will only happen if clockSetScreenIdx == 0 or 1
	if (pBuff[0] > '2') pBuff[0] = '0'; // this will only happen if clockSetScreenIdx == 0

}

static void clockSet::changeDigitDown(void)
{

	pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])]--;
	if (pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] < '0') pBuff[(unsigned int)(screenCursor[(unsigned int)(clockSetScreenIdx)])] = '9';

	if (pBuff[0] > '2') pBuff[0] = '2'; // this will only happen if clockSetScreenIdx == 0
	if ((pBuff[0] == '2') && (pBuff[1] > '3')) pBuff[1] = '3'; // this will only happen if clockSetScreenIdx == 0 or 1
	if (pBuff[2] > '5') pBuff[2] = '5'; // this will only happen if clockSetScreenIdx == 2

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

	cursor::screenLevelEntry(PSTR("Time Set"), clockShowScreenIdx);

#endif // useSoftwareClock
}

static void clockSet::cancel(void)
{

	cursor::screenLevelEntry(PSTR("Time NOT Set"), clockShowScreenIdx);

}

#endif // useClockDisplay
#ifdef useBigDigitDisplay
/* Big Digit Output support section */

static uint8_t bigDigit::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;
	uint8_t thisMenuLevel = menuLevel;
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
	uint8_t tripIdx = pgm_read_byte(&msTripList[(unsigned int)(cursorPos)]);
	char * str;
#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
#ifdef useBigFE
	uint8_t i;
#endif // useBigFE

	switch (thisMenuLevel)
	{

#ifdef useBigDTE
		case bigDTEscreenIdx:
			str = PSTR(" DistToEmpty\r");
			break;

#endif // useBigDTE
#ifdef useBigFE
		case bigFEscreenIdx:
			str = PSTR(" Fuel Econ\r");
			break;

#endif // useBigFE
#ifdef useBigTTE
		case bigTTEscreenIdx:
			str = PSTR(" TimeToEmpty\r");
			break;

#endif // useBigTTE
		default:
#ifdef useClockDisplay
			if (cursorPos == 255) thisMenuLevel = clockShowScreenIdx;
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

#ifdef useClockDisplay
				case clockShowScreenIdx:
				case clockSetScreenIdx:
#endif // useClockDisplay
#ifdef useBigDTE
				case bigDTEscreenIdx:
#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
#endif // useBigTTE
					bigDigit::loadCGRAMnumberFont();
					break;

				default:
					break;

			}

		case menuCursorUpdateIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigDTE
				case bigDTEscreenIdx:
#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
#endif // useBigTTE
#if defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
					displayStatus(str, cursorPos);
					break;

#endif // defined(useBigDTE) || defined(useBigFE) || defined(useBigTTE)
#ifdef useClockDisplay
				case clockShowScreenIdx:
					printStatusMessage(PSTR("Clock\r"));
					break;

				case clockSetScreenIdx:
					break;

#endif // useClockDisplay
				default:
					break;

			}

		case menuOutputDisplayIdx:
			switch (thisMenuLevel)
			{

#ifdef useBigDTE
				case bigDTEscreenIdx:
					outputNumber(tripIdx, tDistanceToEmpty, 4);
					break;

#endif // useBigDTE
#ifdef useBigFE
				case bigFEscreenIdx:
					i = outputNumber(tripIdx, tFuelEcon, 3) - calcFormatFuelEconomyIdx;

					text::stringOut(devLCD, msTripNameString, cursorPos);
					text::gotoXY(devLCD, 12, 1);
					text::stringOut(devLCD, bigFElabels, i);
					break;

#endif // useBigFE
#ifdef useBigTTE
				case bigTTEscreenIdx:
					outputTime(ull2str(tTimeToEmpty, mBuff1, tripIdx), (mainLoopHeartBeat & 0b01010101), 4);
					break;

#endif // useBigTTE
#ifdef useClockDisplay
				case clockShowScreenIdx:
					outputTime(ull2str(tReadTicksToSeconds, mBuff1, vClockCycleIdx), (mainLoopHeartBeat & 0b01010101), 4);
					break;

				case clockSetScreenIdx:
					outputTime(pBuff, (timer0Status & t0sShowCursor), cursorPos);
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

static void bigDigit::loadCGRAMnumberFont(void)
{

	uint8_t numChars;
	const char * fontPtr;

	fontPtr = bigDigitFont;
	numChars = pgm_read_byte(fontPtr++); // get the number of characters in the font

	for (uint8_t chr = 0; chr < numChars * 8; chr++) LCD::writeCGRAMbyte(chr, pgm_read_byte(fontPtr++)); // copy the CGRAM character data into RAM

	LCD::flushCGRAM();

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

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, windowLength, dfAdjustWindow); // perform the required decimal formatting
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

static void bigDigit::displayStatus(const char * str, uint8_t cursorPos)
{

	initStatusLine();
	text::stringOut(devLCD, msTripNameString, cursorPos);
	text::stringOut(devLCD, str); // briefly display screen name
	execStatusLine();

}

#endif // useBigDigitDisplay
