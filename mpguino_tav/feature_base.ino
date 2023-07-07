/* Main screen section */

static void mainDisplay::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

#if defined(trackIdleEOCdata)
	uint8_t i;

	pageFunc pageFormatFunc;

#endif // defined(trackIdleEOCdata)
	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(devLCD, mainDisplayFuncNames, cursorPos); // briefly display screen name
#if defined(useScreenEditor)
			basePageIdx = cursorPos * 4;
#endif // defined(useScreenEditor)

		case displayOutputIdx:
#if defined(trackIdleEOCdata)
			i = (activityFlags & afActivityCheckFlags);
			switch (i)
			{

				case (afVehicleStoppedFlag | afUserInputFlag):
					pageFormatFunc = getMainIdlePageFormats;
					i = 0;
					break;

				case (afEngineOffFlag | afUserInputFlag):
					pageFormatFunc = getMainEOCpageFormats;
					i = 0;
					break;

				default:
					pageFormatFunc = getMainDisplayPageFormat;
					i = cursorPos;
					break;

			}

#if defined(useSpiffyTripLabels)
			outputPage(pageFormatFunc, i, 136, 0, msTripBitPattern);
#else // defined(useSpiffyTripLabels)
			outputPage(pageFormatFunc, i, 136, 0);
#endif // defined(useSpiffyTripLabels)
#else // defined(trackIdleEOCdata)
#if defined(useSpiffyTripLabels)
			outputPage(getMainDisplayPageFormat, cursorPos, 136, 0, msTripBitPattern);
#else // defined(useSpiffyTripLabels)
			outputPage(getMainDisplayPageFormat, cursorPos, 136, 0);
#endif // defined(useSpiffyTripLabels)
#endif // defined(trackIdleEOCdata)

			break;

		default:
			break;

	}

}

static uint16_t mainDisplay::getMainDisplayPageFormat(uint8_t formatIdx)
{

#if defined(useScreenEditor)
	return EEPROM::readWord(formatIdx + eePtrDisplayPagesStart);
#else // defined(useScreenEditor)
	return pgm_read_word(&mainDisplayPageFormats[(uint16_t)(formatIdx)]);
#endif // defined(useScreenEditor)

}

#if defined(trackIdleEOCdata)
static uint16_t mainDisplay::getMainEOCpageFormats(uint8_t formatIdx)
{

	return pgm_read_word(&mainEOCpageFormats[(uint16_t)(formatIdx)]);

}

static uint16_t mainDisplay::getMainIdlePageFormats(uint8_t formatIdx)
{

	return pgm_read_word(&mainIdlePageFormats[(uint16_t)(formatIdx)]);

}

#endif // defined(trackIdleEOCdata)
#if defined(useSpiffyTripLabels)
static void mainDisplay::outputPage(pageFunc pageFormatFunc, uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
#else // defined(useSpiffyTripLabels)
static void mainDisplay::outputPage(pageFunc pageFormatFunc, uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink)
#endif // defined(useSpiffyTripLabels)
{

	uint8_t sfIdx;
	uint16_t displayPageFormat;

	sfIdx = cursorPos * 4;

	for (uint8_t x = 0; x < 4; x++)
	{

		displayPageFormat = pageFormatFunc(sfIdx + x);

#if defined(useSpiffyTripLabels)
		outputFunction(x, displayPageFormat, tripBlink, calcBlink, localTripBitmask);
#else // defined(useSpiffyTripLabels)
		outputFunction(x, displayPageFormat, tripBlink, calcBlink);
#endif // defined(useSpiffyTripLabels)

	}

#if defined(useSpiffyTripLabels)
	LCD::flushCGRAM();

#endif // defined(useSpiffyTripLabels)
}

#if defined(useSpiffyTripLabels)
static void mainDisplay::outputFunction(uint8_t readingIdx, uint16_t pageFormat, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
#else // defined(useSpiffyTripLabels)
static void mainDisplay::outputFunction(uint8_t readingIdx, uint16_t pageFormat, uint8_t tripBlink, uint8_t calcBlink)
#endif // defined(useSpiffyTripLabels)
{

	calcFuncObj thisCalcFuncObj;

	union union_16 * pF = (union union_16 *)(&pageFormat);
	uint8_t x;
	uint8_t y;
	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t tripBitmask;
	uint8_t calcBitmask;
#if defined(useSpiffyTripLabels)
	uint8_t tripPart;
	uint8_t calcPart;
	uint8_t translateFlag;
	uint8_t localTripIdx;
#endif // defined(useSpiffyTripLabels)

	calcIdx = pF->u8[0];
	tripIdx = pF->u8[1];

#if defined(useSpiffyTripLabels)
	translateFlag = calcIdx & 0x80; // save trip index translation bit

	if (calcIdx & 0x80)
	{

		localTripIdx = tripIdx;
		calcIdx &= 0x7F; // strip off trip index translation bit

	}
	else
	{

		localTripIdx = (tripIdx & dfFunctionMask); // extract local trip index
		tripIdx = (tripIdx & dfTripMask) >> dfBitShift; // extract calculation trip index

	}

#else // defined(useSpiffyTripLabels)
	if (calcIdx & 0x80) calcIdx &= 0x7F; // if trip index translation bit is set, trip index is immediately usable
	else tripIdx = (tripIdx & dfTripMask) >> dfBitShift; // otherwise, extract calculation trip index

#endif // defined(useSpiffyTripLabels)
	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function component should blink or not

	readingIdx &= 3;
	x = (readingIdx & 1) * (LCDcharWidth / 2); // figure out horizontal component (0 or 8)
	y = (readingIdx & 2) >> 1; // figure out vertical component (0 or 1)

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, pBuff, (LCDcharWidth / 2) - 2, 0);

	text::gotoXY(devLCD, x, y);
	if (calcBitmask) text::stringOut(devLCD, thisCalcFuncObj.strBuffer);
	else
	{

		thisCalcFuncObj.calcChar = ' ';
		text::charOut(devLCD, ' ', 6);

	}

#if defined(useSpiffyTripLabels)
	readingIdx <<= 1;

	text::charOut(devLCD, 0xF0 + readingIdx);
	text::charOut(devLCD, 0xF1 + readingIdx);

	readingIdx <<= 3;

	for (uint8_t x = 0; x < 16; x++)
	{

		calcPart = pgm_read_byte(&calcFormatLabelCGRAM[(uint16_t)(thisCalcFuncObj.calcFmtIdx)][(uint16_t)(x)]); // read a byte of function label bit pattern

		if (translateFlag) tripPart = 0;
		else
		{

			tripPart = (calcPart >> 5) & 0x03; // fetch partial address of trip label component
			tripPart = pgm_read_byte(&localTripBitmask[(uint16_t)(localTripIdx)][(uint16_t)(tripPart)]); // read a byte of trip label bit pattern
			tripPart &= tripBitmask; // provide for blinking trip label component

		}

		calcPart &= calcBitmask; // provide for blinking function label component

		LCD::writeCGRAMbyte(readingIdx + x, calcPart | tripPart); // combine trip label and function label components

	}

#else // defined(useSpiffyTripLabels)
	if (tripBitmask == 0) thisCalcFuncObj.tripChar = ' ';

	text::charOut(devLCD, thisCalcFuncObj.tripChar);
	text::charOut(devLCD, thisCalcFuncObj.calcChar);

#endif // defined(useSpiffyTripLabels)
}

#if defined(useScreenEditor)
/* Programmable main display screen edit support section */

static void displayEdit::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t tripBlink;
	uint8_t calcBlink;

	switch (cmd)
	{

		case displayInitialEntryIdx:
			basePageIdx = displayCursor[(uint16_t)(mainDisplayIdx)] * 4;
			for (uint8_t x = 0; x < 4; x++) displayEditPageFormats[(uint16_t)(x)] = mainDisplay::getMainDisplayPageFormat(basePageIdx + x);

		case displayCursorUpdateIdx:
			formatEditIdx = cursorPos / 2;
			formatFunctionFlag = (cursorPos & 1);

		case displayOutputIdx:
			for (uint8_t x = 0; x < 4; x++)
			{

				tripBlink = 0;
				calcBlink = 0;

				if (x == formatEditIdx)
				{

					if (formatFunctionFlag) calcBlink = 170;
					else tripBlink = 170;

				}

#if defined(useSpiffyTripLabels)
				mainDisplay::outputFunction(x, displayEditPageFormats[(uint16_t)(x)], tripBlink, calcBlink, msTripBitPattern);
#else // defined(useSpiffyTripLabels)
				mainDisplay::outputFunction(x, displayEditPageFormats[(uint16_t)(x)], tripBlink, calcBlink);
#endif // defined(useSpiffyTripLabels)

			}

#if defined(useSpiffyTripLabels)
			LCD::flushCGRAM();

#endif // defined(useSpiffyTripLabels)
			break;

		default:
			break;

	}

}

static void displayEdit::entry(void)
{

	cursor::moveAbsolute(displayEditDisplayIdx, 0);

}

static void displayEdit::cancel(void)
{

	cursor::screenLevelEntry(deFormatReverted, mainDisplayIdx);

}

static void displayEdit::set(void)
{

	const char * str;

	metricFlag &= ~(EEPROMbulkChangeFlag);

	for (uint8_t x = 0; x < 4; x++) EEPROM::writeVal(basePageIdx + x + eePtrDisplayPagesStart, (uint32_t)(displayEditPageFormats[(uint16_t)(x)]));

	if (metricFlag & EEPROMbulkChangeFlag) str = deFormatSaved;
	else str = deFormatNoChange;

	cursor::screenLevelEntry(str, mainDisplayIdx);

}

static void displayEdit::readInitial(void)
{

	displayEditPageFormats[(uint16_t)(formatEditIdx)] = pgm_read_word(&mainDisplayPageFormats[(uint16_t)(basePageIdx + formatEditIdx)]);

}

static void displayEdit::changeItemUp(void)
{

	changeItem(1);

}

#if defined(useButtonCrossConfig)
static void displayEdit::changeItemDown(void)
{

	changeItem(255);

}

#endif // useButtonCrossConfig
static void displayEdit::changeItem(uint8_t changeDir)
{

	union union_16 * dEPF = (union union_16 *)(&displayEditPageFormats[(uint16_t)(formatEditIdx)]);

	if (formatFunctionFlag) // modify the function portion of the format value
	{

		dEPF->u8[0] += changeDir; // adjust trip function index

		if (dEPF->u8[0] >= dfMaxValDisplayCount) dEPF->u8[0] = ( changeDir == 1 ? 0 : dfMaxValDisplayCount - 1); // boundary check

	}
	else // modify the trip portion of the format value
	{

		dEPF->u8[1] &= dfFunctionMask; // strip off trip variable index
		dEPF->u8[1] += changeDir; // adjust trip label index

		if (dEPF->u8[1] >= msMaxTripCount) dEPF->u8[1] = ( changeDir == 1 ? 0 : msMaxTripCount - 1); // boundary check

		dEPF->u8[1] |= (pgm_read_byte(&msTripList[(uint16_t)(dEPF->u8[1])]) << dfBitShift); // combine with new corresponding trip variable index

	}

}

#endif // defined(useScreenEditor)
