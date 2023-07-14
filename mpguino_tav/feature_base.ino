/* basic top-down menu section */

static uint8_t baseMenu::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuDoSelectionIdx:
			switch (cursorPos)
			{

				case 0:
#if defined(useExpandedMainDisplay)
					retVal = mainMenuDisplayIdx;
#else // defined(useExpandedMainDisplay)
					retVal = mainDisplayIdx;
#endif // defined(useExpandedMainDisplay)

					break;

				case 1:
					retVal = settingsMenuDisplayIdx;
					break;

				default:
					retVal = optionalDisplayIdxStart + cursorPos - 2;
					break;

			}
			break;

		case menuExitIdx:
			retVal = mainDisplayIdx;
			break;

		default:
			break;

	}

	return retVal;

}

/* main display section */

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
			text::statusOut(devLCD, mainDisplayPageTitles, cursorPos); // briefly display screen name
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

			outputPage(pageFormatFunc, i, 136, 0);
#else // defined(trackIdleEOCdata)
			outputPage(getMainDisplayPageFormat, cursorPos, 136, 0);
#endif // defined(trackIdleEOCdata)

			break;

		default:
			break;

	}

}

#if defined(useExpandedMainDisplay)
static uint8_t mainDisplay::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuDoSelectionIdx:
			retVal = mainDisplayIdx + cursorPos;
			break;

		case menuExitIdx:
			retVal = baseMenuDisplayIdx;
			break;

		default:
			break;

	}

	return retVal;

}

#endif // defined(useExpandedMainDisplay)
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
static void mainDisplay::outputPage(pageFunc pageFormatFunc, uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink)
{

	uint8_t sfIdx;
	uint16_t displayPageFormat;

	sfIdx = cursorPos * 4;

	for (uint8_t x = 0; x < 4; x++)
	{

		displayPageFormat = pageFormatFunc(sfIdx + x);

		outputFunction(x, displayPageFormat, tripBlink, calcBlink);

	}

#if defined(useSpiffyTripLabels)
	LCD::flushCGRAM();

#endif // defined(useSpiffyTripLabels)
}

static void mainDisplay::outputFunction(uint8_t readingIdx, uint16_t tripFunction, uint8_t tripBlink, uint8_t calcBlink)
{

	calcFuncObj thisCalcFuncObj;

	uint8_t x;
	uint8_t y;
	uint8_t tripBitmask;
	uint8_t calcBitmask;
#if defined(useSpiffyTripLabels)
	uint8_t tripPart;
	uint8_t calcPart;
	uint8_t localTripIdx;
#endif // defined(useSpiffyTripLabels)

	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function component should blink or not

	readingIdx &= 3;
	x = (readingIdx & 1) * (LCDcharWidth / 2); // figure out horizontal component (0 or 8)
	y = (readingIdx & 2) >> 1; // figure out vertical component (0 or 1)

	thisCalcFuncObj = translateCalcIdx(tripFunction, pBuff, (LCDcharWidth / 2) - 2, 0);

	text::gotoXY(devLCD, x, y);
	if (calcBitmask) text::stringOut(devLCD, thisCalcFuncObj.strBuffer);
	else
	{

		thisCalcFuncObj.calcChar = ' ';
		text::charOut(devLCD, ' ', (LCDcharWidth / 2) - 2);

	}

#if defined(useSpiffyTripLabels)
	readingIdx <<= 1;

	text::charOut(devLCD, 0xF0 + readingIdx);
	text::charOut(devLCD, 0xF1 + readingIdx);

	readingIdx <<= 3;

	if (thisCalcFuncObj.suppressTripLabel == 0) localTripIdx = findTripIdx(thisCalcFuncObj.tripIdx);

	for (uint8_t x = 0; x < 16; x++)
	{

		calcPart = pgm_read_byte(&calcFormatLabelCGRAM[(uint16_t)(thisCalcFuncObj.calcFmtIdx)][(uint16_t)(x)]); // read a byte of function label bit pattern

		if (thisCalcFuncObj.suppressTripLabel) tripPart = 0;
		else
		{

			tripPart = (calcPart >> 5) & 0x03; // fetch partial address of trip label component
			tripPart = pgm_read_byte(&tripFormatLabelCGRAM[(uint16_t)(localTripIdx)][(uint16_t)(tripPart)]); // read a byte of trip label bit pattern
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

static uint8_t mainDisplay::findTripIdx(uint8_t tripIdx)
{

	uint8_t i;

	for (uint8_t x = 0; x < tripFormatIdxCount; x++) if (tripIdx == pgm_read_byte(&tripFormatReverseList[(uint16_t)(x)])) i = x;

	return i;

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

				mainDisplay::outputFunction(x, displayEditPageFormats[(uint16_t)(x)], tripBlink, calcBlink);

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

#endif // defined(useButtonCrossConfig)
static void displayEdit::changeItem(uint8_t changeDir)
{

	uint8_t i;
	union union_16 * dEPF = (union union_16 *)(&displayEditPageFormats[(uint16_t)(formatEditIdx)]);

	if (formatFunctionFlag) // modify the function portion of the format value
	{

		dEPF->u8[0] += changeDir; // adjust trip function index

		if (dEPF->u8[0] >= dfMaxValDisplayCount) dEPF->u8[0] = ( changeDir == 1 ? 0 : dfMaxValDisplayCount - 1); // boundary check

	}
	else // modify the trip portion of the format value
	{

		i = mainDisplay::findTripIdx(dEPF->u8[1]) + changeDir; // adjust trip label index

		if (i >= tripFormatIdxCount) i = ( changeDir == 1 ? 0 : tripFormatIdxCount - 1); // boundary check

		dEPF->u8[1] = pgm_read_byte(&tripFormatReverseList[(uint16_t)(i)]);

	}

}

#endif // defined(useScreenEditor)
