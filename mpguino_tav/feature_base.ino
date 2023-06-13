/* Main screen section */

static uint8_t mainDisplay::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

#if defined(trackIdleEOCdata)
	pageFunc pageFormatFunc;

#endif // defined(trackIdleEOCdata)
	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			topScreenLevel = menuLevel; // save current menu level for sub-function (param edit, trip load/save, etc) support

		case menuCursorUpdateIdx:
			text::statusOut(devLCD, mainDisplayFuncNames, cursorPos); // briefly display screen name
#if defined(useScreenEditor)
			basePageIdx = cursorPos * 4;
#endif // defined(useScreenEditor)

		case menuOutputDisplayIdx:
#if defined(trackIdleEOCdata)
			retVal = (activityFlags & afActivityCheckFlags);
			switch (retVal)
			{

				case (afVehicleStoppedFlag | afButtonFlag):
					pageFormatFunc = getMainIdlePageFormats;
					break;

				case (afEngineOffFlag | afButtonFlag):
					pageFormatFunc = getMainEOCpageFormats;
					break;

				default:
					pageFormatFunc = getMainDisplayPageFormat;
					break;

			}

#if defined(useSpiffyTripLabels)
			outputPage(pageFormatFunc, 0, 136, 0, msTripBitPattern);
#else // defined(useSpiffyTripLabels)
			outputPage(pageFormatFunc, 0, 136, 0);
#endif // defined(useSpiffyTripLabels)
#else // defined(trackIdleEOCdata)
#if defined(useSpiffyTripLabels)
			outputPage(getMainDisplayPageFormat, 0, 136, 0, msTripBitPattern);
#else // defined(useSpiffyTripLabels)
			outputPage(getMainDisplayPageFormat, 0, 136, 0);
#endif // defined(useSpiffyTripLabels)
#endif // defined(trackIdleEOCdata)

			retVal = 0;
			break;

		default:
			break;

	}

	return retVal;

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

		localTripIdx = (tripIdx & dfTripMask) >> dfBitShift; // extract local trip index
		tripIdx = (tripIdx & dfFunctionMask);  // extract calculation trip index

	}

#else // defined(useSpiffyTripLabels)
	if (calcIdx & 0x80) calcIdx &= 0x7F; // if trip index translation bit is set, trip index is immediately usable
	else tripIdx &= (dfFunctionMask);  // otherwise, extract calculation trip index

#endif // defined(useSpiffyTripLabels)
	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function component should blink or not

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, 6, 0);

	readingIdx &= 3;
	x = (readingIdx & 1) << 3; // figure out horizontal component (0 or 8)
	y = (readingIdx & 2) >> 1; // figure out vertical component (0 or 1)

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

static void mainDisplay::returnToMain(void)
{

	cursor::moveAbsolute(topScreenLevel, 255);

}

#if defined(useScreenEditor)
/* Programmable main display screen edit support section */

static uint8_t displayEdit::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t tripBlink;
	uint8_t calcBlink;

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			basePageIdx = displayCursor[(uint16_t)(mainDisplayIdx)] * 4;
			for (uint8_t x = 0; x < 4; x++) displayEditPageFormats[(uint16_t)(x)] = mainDisplay::getMainDisplayPageFormat(basePageIdx + x);

		case menuCursorUpdateIdx:
			
		case menuOutputDisplayIdx:
			retVal = cursorPos / 2;
			for (uint8_t x = 0; x < 4; x++)
			{

				tripBlink = 0;
				calcBlink = 0;

				if (x == retVal)
				{

					if (cursorPos & 1) calcBlink = 170;
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
			retVal = 0;
			break;

		default:
			break;

	}

	return retVal;

}

static void displayEdit::entry(void)
{

	cursor::moveAbsolute(displayEditDisplayIdx, 0);

}

static void displayEdit::cancel(void)
{

	cursor::screenLevelEntry(seFormatRevertedString, mainDisplayIdx);

}

static void displayEdit::set(void)
{



}

static void displayEdit::readInitial(void)
{



}

static void displayEdit::changeItemUp(void)
{



}

#if defined(useButtonCrossConfig)
static void displayEdit::changeItemDown(void)
{



}

#endif // useButtonCrossConfig
void doCursorUpdateScreenEdit(void)
{

	uint8_t b;

	b = displayCursor[(unsigned int)(displayEditDisplayIdx)];
	b &= ~screenEditFlag_dirty;

	if ((screenEditDirty & ~screenEditFlag_dirty) ^ b) // if cursor moved to a different screen function altogether
	{

		screenTripValue = EEPROM::readByte(b + eePtrDisplayPagesStart);
		screenFunctionValue = EEPROM::readByte(b + eePtrDisplayPagesStart + 1);

		if (screenEditDirty & screenEditFlag_dirty) // if previous screen function was changed but not saved
		{

			text::statusOut(devLCD, seFormatRevertedString);

		}

		screenEditDirty = b; // save current cursor position of screen function being edited
		screenEditDirty &= ~screenEditFlag_dirty; // mark screen function as not modified

	}

}

void doScreenEditBump(void)
{

	uint8_t b;

	b = (displayCursor[(unsigned int)(displayEditDisplayIdx)] & 0x01); // figure out whether trip variable or trip function is being modified
	screenEditDirty |= screenEditFlag_dirty; // mark current screen function as modified

	if (b) // if trip function is being modified
	{

		screenFunctionValue++;
		if (screenFunctionValue == dfMaxValDisplayCount) screenFunctionValue = 0;

	}
	else // otherwise, trip variable is being modified
	{

		screenTripValue++;
		if (screenTripValue == msMaxTripCount) screenTripValue = 0;

	}

}

void doSaveScreen(void)
{

	uint8_t b;

	b = displayCursor[(unsigned int)(displayEditDisplayIdx)];

	// generate and save new screen function from trip variable and trip function
	EEPROM::writeVal(eePtrDisplayPagesStart + b, (unsigned long)(screenTripValue));
	EEPROM::writeVal(eePtrDisplayPagesStart + b + 1, (unsigned long)(screenFunctionValue));
	screenEditDirty &= ~screenEditFlag_dirty;

	text::statusOut(devLCD, PSTR("Format saved"));

}

void doScreenReturnToMain(void)
{

	const char * str;

	if (screenEditDirty & screenEditFlag_dirty) str = seFormatRevertedString;
	else str = seExitScreenEditString;

	displayCursor[(unsigned int)(mainDisplayIdx)] = (displayCursor[(unsigned int)(displayEditDisplayIdx)] >> 3);
	cursor::screenLevelEntry(str, topScreenLevel);

}

#endif // defined(useScreenEditor)
