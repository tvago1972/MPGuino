#if defined(useButtonInput)
/* basic top-down menu section */

static uint8_t baseMenu::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuFirstLineOutIdx:
			text::stringOut(m8DevLCDidx, baseMenuTitles, cursorPos);
			break;

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
#if defined(useExpandedMainDisplay)
			retVal = displayCursor[(uint16_t)(mainMenuDisplayIdx)] + mainDisplayIdx;
#else // defined(useExpandedMainDisplay)
			retVal = mainDisplayIdx;
#endif // defined(useExpandedMainDisplay)
			break;

		default:
			break;

	}

	return retVal;

}

/* main display section */

static uint8_t mainDisplay::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

#if defined(trackIdleEOCdata)
	uint8_t i;

	pageFunc pageFormatFunc;

#endif // defined(trackIdleEOCdata)
	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(m8DevLCDidx, mainDisplayPageTitles, cursorPos); // briefly display screen name
#if defined(useScreenEditor)
			basePageIdx = cursorPos * 4;
#endif // defined(useScreenEditor)

		case displayOutputIdx:
#if defined(trackIdleEOCdata)
			i = (volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afActivityCheckFlags);
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

		case menuFirstLineOutIdx:
			text::stringOut(m8DevLCDidx, mainMenuTitles, cursorPos);
			break;

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

static void mainDisplay::goToMenu(void)
{

	displayCursor[(uint16_t)(mainMenuDisplayIdx)] = workingDisplayIdx - mainDisplayIdx;
	menu::entry();

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

	uint8_t x;
	uint8_t y;
	uint8_t tripBitmask;
	uint8_t calcBitmask;
	uint8_t windowLength;
#if defined(useSpiffyTripLabels)
	uint8_t tripPart;
	uint8_t calcPart;
	uint8_t localTripIdx;
#endif // defined(useSpiffyTripLabels)

	tripBitmask = ((volatile8Variables[(uint16_t)(v8HeartbeatBitmaskIdx - v8VariableStartIdx)] & tripBlink) ? dfBlinkTrip : 0); // determine if trip label component should blink or not
	calcBitmask = ((volatile8Variables[(uint16_t)(v8HeartbeatBitmaskIdx - v8VariableStartIdx)] & calcBlink) ? dfBlinkCalc : 0); // determine if function component should blink or not

	readingIdx &= 3;
	windowLength = LCDcharWidth / 2;
	x = (readingIdx & 1) * windowLength; // figure out horizontal component (0 or 8)
	y = (readingIdx & 2) >> 1; // figure out vertical component (0 or 1)
	windowLength -= 2; // account for trip/function tag characters for tripFunctionOut

#if defined(useSpiffyTripLabels)
	readingIdx <<= 1;

	mainCalcFuncVar.labelIdx = readingIdx;

	readingIdx <<= 3;

#endif // defined(useSpiffyTripLabels)
	text::gotoXY(m8DevLCDidx, x, y);
	text::tripFunctionOut(m8DevLCDidx, tripFunction, windowLength, (tripBitmask | calcBitmask | dfOutputTag | dfOutputSpiffyTag));

#if defined(useSpiffyTripLabels)
	localTripIdx = findTripIdx(mainCalcFuncVar.tripIdx);

	for (uint8_t x = 0; x < 16; x++)
	{

		// provide for blinking function label component
		if (calcBitmask) calcPart = 0;
		else calcPart = pgm_read_byte(&calcFormatLabelCGRAM[(uint16_t)(mainCalcFuncVar.calcFmtIdx)][(uint16_t)(x)]); // read a byte of function label bit pattern

		if ((tripBitmask) || (mainCalcFuncVar.suppressTripLabel)) tripPart = 0; // provide for blinking or suppressed trip label component
		else
		{

			tripPart = (calcPart >> 5) & 0x03; // fetch partial address of trip label component
			tripPart = pgm_read_byte(&tripFormatLabelCGRAM[(uint16_t)(localTripIdx)][(uint16_t)(tripPart)]); // read a byte of trip label bit pattern

		}

		LCD::writeCGRAMbyte(readingIdx + x, calcPart | tripPart); // combine trip label and function label components

	}

#endif // defined(useSpiffyTripLabels)
}

static uint8_t mainDisplay::findTripIdx(uint8_t tripIdx)
{

	uint8_t i;

	i = 0;

	for (uint8_t x = 0; x < tripFormatIdxCount; x++) if (tripIdx == pgm_read_byte(&tripFormatReverseList[(uint16_t)(x)])) i = x;

	return i;

}

#if defined(useScreenEditor)
/* Programmable main display screen edit support section */

static uint8_t displayEdit::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t calcBlink;

	switch (cmd)
	{

		case displayInitialEntryIdx:
			basePageIdx = displayCursor[(uint16_t)(mainDisplayIdx)] * 4;
			for (uint8_t x = 0; x < 4; x++) displayEditPageFormats[(uint16_t)(x)] = mainDisplay::getMainDisplayPageFormat(basePageIdx + x);

		case displayCursorUpdateIdx:
			functionPos = cursorPos;
		case displayOutputIdx:
			for (uint8_t x = 0; x < 4; x++)
			{

				if (x == functionPos) calcBlink = 170;
				else calcBlink = 0;

				mainDisplay::outputFunction(x, displayEditPageFormats[(uint16_t)(x)], 0, calcBlink);

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

	mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] &= ~(ecsEEPROMchangeDetected);

	for (uint8_t x = 0; x < 4; x++) EEPROM::writeVal(basePageIdx + x + eePtrDisplayPagesStart, (uint32_t)(displayEditPageFormats[(uint16_t)(x)]));

	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsEEPROMchangeDetected) str = deFormatSaved;
	else str = deFormatNoChange;

	cursor::screenLevelEntry(str, mainDisplayIdx);

}

static void displayEdit::readInitial(void)
{

	displayEditPageFormats[(uint16_t)(functionPos)] = pgm_read_word(&mainDisplayPageFormats[(uint16_t)(basePageIdx + functionPos)]);

}

static void displayEdit::changeFunctionUp(void)
{

	changeFunction(1);

}

static void displayEdit::changeTripUp(void)
{

	changeTrip(1);

}

#if defined(useButtonCrossConfig)
static void displayEdit::changeFunctionDown(void)
{

	changeFunction(255);

}

static void displayEdit::changeTripDown(void)
{

	changeTrip(255);

}

#endif // defined(useButtonCrossConfig)
static void displayEdit::changeFunction(uint8_t changeDir)
{

	union union_16 * dEPF = (union union_16 *)(&displayEditPageFormats[(uint16_t)(functionPos)]);

	dEPF->u8[0] += changeDir; // adjust trip function index

	if (dEPF->u8[0] >= dfMaxValDisplayCount) dEPF->u8[0] = ( changeDir == 1 ? 0 : dfMaxValDisplayCount - 1); // boundary check

}

static void displayEdit::changeTrip(uint8_t changeDir)
{

	uint8_t i;
	union union_16 * dEPF = (union union_16 *)(&displayEditPageFormats[(uint16_t)(functionPos)]);

	i = mainDisplay::findTripIdx(dEPF->u8[1]) + changeDir; // adjust trip label index

	if (i >= tripFormatIdxCount) i = ( changeDir == 1 ? 0 : tripFormatIdxCount - 1); // boundary check

	dEPF->u8[1] = pgm_read_byte(&tripFormatReverseList[(uint16_t)(i)]);

}

#endif // defined(useScreenEditor)
#endif // defined(useButtonInput)
