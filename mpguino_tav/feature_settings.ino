#if defined(useButtonInput)
/* EEPROM parameter settings menu support section */

static uint8_t settings::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuFirstLineOutIdx:
			text::stringOut(m8DevLCDidx, settingsMenuTitles, cursorPos);
			break;

		case menuDoSelectionIdx:
			retVal = displaySettingsDisplayIdx + cursorPos;
			break;

		case menuExitIdx:
			retVal = baseMenuDisplayIdx;
			break;

		default:
			break;

	}

	return retVal;

}

#endif // defined(useButtonInput)
/* EEPROM parameter number editor section */

static const uint8_t prgmWriteParameterValue[] PROGMEM = {
	instrStRegEEPROMindexed, 0x02,
	instrDone											// return to caller
};

static const uint8_t prgmFetchMaximumParamValue[] PROGMEM = {
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrLxdIParamLengthIndexed,						// load parameter length into index

//loop:
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead
	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

//cont:
	instrLdRegByte, 0x01, 1,							// load a 1 into register 1
	instrSubYfromX, 0x12,								// subtract 1 from register 2 to obtain final maximum parameter value
	instrDone											// return to caller
};

static const uint8_t prgmCompareWithMaximumParamValue[] PROGMEM = {
	instrLdReg, 0x23,									// save converted working number for comparison
	instrLdRegByte, 0x02, 1,							// load MSB of maximum parameter value into register 2
	instrLxdIParamLengthIndexed,						// load parameter length into index

//loop:
	instrTestIndex,										// test parameter power of 2 size passed in
	instrBranchIfZero,	6,								// if power of 2 has gone to zero, continue ahead
	instrShiftRegLeft, 0x02,							// otherwise, shift maximum parameter value left by one bit
	instrAddIndex, 255,									// shift left MSB of maximum parameter value by power of 2 stored in trip index register
	instrSkip, 247,										// loop back

//cont:
	instrCmpXtoY, 0x23,									// compare working number to maximum parameter value
	instrBranchIfLTorE, 4,								// if maximum parameter value <= working number, signal "overflow"
	instrLdRegByte, 0x02, 0,							// signal that working number is good
	instrDone,											// return to caller

//badret:
	instrLdRegByte, 0x02, 255,							// signal that working number is bad
	instrDone											// return to caller
};

static uint8_t parameterEdit::sharedFunctionCall(uint8_t cmd)
{

	uint8_t retVal = 0;
	uint8_t parameterPtr = numberEditObj.parameterIdx;

	switch (cmd)
	{

		case nesLoadInitial:
			SWEET64::runPrgm(prgmFetchParameterValue, parameterPtr);

		case nesLoadValue:
			ull2str(pBuff, 3, tFormatToNumber);
#if defined(useButtonInput)
			parameterEdit::findLeft();
#endif // defined(useButtonInput)

		case nesOnChange:
#if defined(useLCDoutput)
			if (parameterPtr == pBrightnessIdx) LCD::setBrightness((uint8_t)(str2ull(pBuff))); // adjust brightness
#if defined(useLCDcontrast)
			if (parameterPtr == pContrastIdx) LCD::setContrast((uint8_t)(str2ull(pBuff))); // adjust contrast dynamically
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
			if (parameterPtr == pLCDcolorIdx) LCD::setRGBcolor((uint8_t)(str2ull(pBuff))); // adjust backlight color dynamically
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
			break;

		default:
			break;

	}

	return retVal;

}

#if defined(useButtonInput)
static uint8_t parameterEdit::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuFirstLineOutIdx:
			text::stringOut(m8DevLCDidx, settingsSubMenuTitles, cursorPos + menuTitlesOffset);
			break;

		case menuInitialEntryIdx:
			numberEditObj.neStatusMessage = pseStatusMessages;
			break;

		case menuSecondLineInitIdx:
			numberEditObj.parameterIdx = pgm_read_byte(&settingsParameterList[(uint16_t)(cursorPos + menuTitlesOffset)]);
			parameterEdit::sharedFunctionCall(nesLoadInitial);
		case menuSecondLineFlagIdx:
			retVal = 1;
			break;

		case menuSecondLineOutIdx:
			text::stringOut(m8DevLCDidx, pBuff); // output supplementary information
			text::newLine(m8DevLCDidx); // clear to the end of the line
			break;

		case menuDoSelectionIdx:
			numberEditObj.callingDisplayIdx = workingDisplayIdx;
			retVal = parameterEditDisplayIdx;
			break;

		case menuExitIdx:
			retVal = settingsMenuDisplayIdx;
			break;

		default:
			break;

	}

	return retVal;

}

static uint8_t parameterEdit::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t j;
	uint8_t k;
	uint8_t c;
	const char * str;

	switch (cmd)
	{

		case displayInitialEntryIdx:
			findLeft();

		case displayCursorUpdateIdx:
			k = ' '; // initially a leading space
			for (uint8_t x = 0; x < 10; x++) // scan all of numeric buffer from left to right
			{

				c = pBuff[(uint16_t)(x)]; // fetch a character from buffer

				if (x > cursorPos) // if current position is past cursor position, just make sure leading spaces get turned into zero digits
				{

					if (c == ' ') pBuff[(uint16_t)(x)] = '0';

				}
				else // if current position is before or at cursor position, ensure leading spaces up to either cursor position or to first non-zero digit
				{

					if ((c == ' ') || (c == '0')) pBuff[(uint16_t)(x)] = k; // if character is either a space or a zero digit, it may be leading so change it
					else k = '0'; // character is a non-zero digit, so change leading character value to a zero digit

				}

			}

			if (pBuff[9] == ' ') pBuff[9] = '0'; // ensure at least one digit

		case displayOutputIdx:
			j = 10;
			str = numberEditSaveCanx;
			do // append save/canx string to end
			{

				c = pgm_read_byte(str++);
				pBuff[(uint16_t)(j++)] = c;

			}
			while (c);

			c = pBuff[(uint16_t)(cursorPos)]; // save existing character

			if (volatile8Variables[(uint16_t)(v8Timer0StatusA - v8VariableStartIdx)] & t0saShowCursor)
			{

				if (cursorPos < 10) pBuff[(uint16_t)(cursorPos)] = '_'; // replace character with an underscore
				else
				{

					if (cursorPos == 10) j = 11; // either select string position for OK
					else j = 14; // ...or string position for XX (cancel)

					pBuff[(uint16_t)(j++)] = ' '; // blank out selected string position
					pBuff[(uint16_t)(j++)] = ' ';

				}

			}

			cursor::updateDisplay(numberEditObj.callingDisplayIdx, displayOutputIdx);

			pBuff[(uint16_t)(cursorPos)] = c; // restore existing character
			pBuff[10] = 0; // restore end of string

			break;

		default:
			break;

	}

}

static void parameterEdit::findLeft(void)
{

	displayCursor[(uint16_t)(parameterEditDisplayIdx)] = 9;

	// do a nice thing and put the edit cursor at the first non zero number
	for (uint8_t x = 9; x < 10; x--) if (pBuff[(uint16_t)(x)] != ' ') displayCursor[(uint16_t)(parameterEditDisplayIdx)] = x;

}

static void parameterEdit::findRight(void)
{

	displayCursor[(uint16_t)(parameterEditDisplayIdx)] = 9;

}

static void parameterEdit::readInitial(void)
{

	SWEET64::runPrgm(prgmFetchInitialParamValue, numberEditObj.parameterIdx);
	sharedFunctionCall(nesLoadValue);

}

static void parameterEdit::readMaxValue(void)
{

	SWEET64::runPrgm(prgmFetchMaximumParamValue, numberEditObj.parameterIdx);
	sharedFunctionCall(nesLoadValue);

}

static void parameterEdit::readMinValue(void)
{

	SWEET64::init64byt((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), 0);
	sharedFunctionCall(nesLoadValue);

}

static void parameterEdit::changeDigitUp(void)
{

	changeDigit(1);

}

static void parameterEdit::changeDigitDown(void)
{

	changeDigit(255);

}

static void parameterEdit::changeDigit(uint8_t digitDir)
{

	uint8_t cp = displayCursor[(uint16_t)(parameterEditDisplayIdx)];
	uint8_t c = '0';
	uint8_t d = ' ';

	uint8_t w;

	switch (cp)
	{
		case 10:
			save();
			break;

		case 11:
			cancel();
			break;

		default:
			w = pBuff[(uint16_t)(cp)]; // fetch digit from stored numeric string representing parameter to be changed

			if (w == ' ') w = '0'; // if this is a leading space, use 0 as working digit
			w += digitDir; // adjust working digit
			if (w > '9') w = '0'; // handle working digit rollover
			if (w < '0') w = '9'; // handle working digit rollover

			pBuff[(uint16_t)(cp)] = w;

			str2ull(pBuff); // convert parameter buffer string into uint64_t
			w = (uint8_t)(SWEET64::runPrgm(prgmCompareWithMaximumParamValue, numberEditObj.parameterIdx));

			if (w) switch (digitDir)
			{

				case 1:
					readMinValue();
					break;

				case 255:
					readMaxValue();
					break;

				default:
					break;

			}

			sharedFunctionCall(nesOnChange);

			break;

	}

}

static void parameterEdit::save(void)
{

	uint8_t cp = displayCursor[(uint16_t)(parameterEditDisplayIdx)];
	uint8_t retVal;

	switch (cp)
	{

		case 10:
			str2ull(pBuff); // convert parameter buffer string into uint64_t
#if defined(usePartialRefuel)
			if (numberEditObj.parameterIdx == pRefuelSizeIdx) SWEET64::runPrgm(prgmAddToPartialRefuel, 0);
#endif // defined(usePartialRefuel)
			retVal = EEPROM::onChange(prgmWriteParameterValue, numberEditObj.parameterIdx); // go save parameter and do any required housekeeping

			cursor::screenLevelEntry(numberEditObj.neStatusMessage, retVal, numberEditObj.callingDisplayIdx);
			break;

		case 11:
			cancel();
			break;

		default:
			cp = 10;
			displayCursor[(uint16_t)(parameterEditDisplayIdx)] = cp;
			displayHandler(displayCursorUpdateIdx, cp);
			break;

	}

}

static void parameterEdit::cancel(void)
{

	uint8_t cp = displayCursor[(uint16_t)(parameterEditDisplayIdx)];
	const char * str;

	if (cp != 11)
	{

		cp = 11;
		displayCursor[(uint16_t)(parameterEditDisplayIdx)] = cp;
		displayHandler(displayCursorUpdateIdx, cp);

	}
	else
	{

		cursor::screenLevelEntry(numberEditObj.neStatusMessage, 2, numberEditObj.callingDisplayIdx);

	}

}

#endif // defined(useButtonInput)
