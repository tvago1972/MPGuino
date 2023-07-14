/* EEPROM parameter settings menu support section */

static uint8_t settings::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

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

/* EEPROM parameter number editor section */

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

static const uint8_t prgmDoEEPROMmetricConversion[] PROGMEM = {
	instrLxdI, 0,										// zero the trip index register

//loop:
	instrLdRegEEPROMindirect, 0x02,						// load an EEPROM parameter, indexed into the convIdx array
	instrLdRegNumer, 0x01,								// load numerator into register 1
	instrMul2by1,										// multiply EEPROM parameter value by numerator
	instrLdRegDenom, 0x01,								// load denominator into register 1
	instrDiv2by1,										// divide EEPROM parameter value by denominator
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegEEPROMindirect, 0x02,						// store converted EEPROM parameter
	instrAddIndex, 1,									// bump conversion index stored in trip index register
	instrCmpIndex, convSize,							// finished processing list of EEPROM parameters?
	instrBranchIfLT, 239,								// if not, loop back
	instrDone											// return to caller
};

#ifdef useCalculatedFuelFactor
static const uint8_t prgmCalculateFuelFactor[] PROGMEM = {
	instrLdRegConst, 0x02, idxCorrectionFactor2,		// obtain reference correction factor
	instrMul2byEEPROM, pSysFuelPressureIdx,				// multiply by this vehicle's stored fuel system absolute pressure
	instrDiv2byEEPROM, pRefFuelPressureIdx,				// divide by the fuel injector reference absolute pressure
	instrIsqrt, 0x02,									// perform square root on result
	instrMul2byEEPROM, pInjectorCountIdx,				// multiply by stored number of fuel injectors for this engine
	instrMul2byEEPROM, pInjectorSizeIdx,				// multiply by injector size in cc/minute * decimal formatting factor (L/min * decimal formatting factor * 1000)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegByte, 0x02, 60,							// load seconds per minute into register 2
	instrMul2byConst, idxMicroSecondsPerSecond,			// multiply by microseconds per second into register 1
	instrMul2byConst, idxOneThousand,					// multiply by number of cc's per liter into register 1
	instrMul2byConst, idxDecimalPoint,					// set numerator up to cancel reference correction factor in denominator
	instrMul2byConst, idxCorrectionFactor,				// set numerator up to cancel reference correction factor in denominator
	instrDiv2by1,										// perform comversion for injector flow
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byConst, idxNumerVolume,					// set up denominator to convert injector flow in liters to US gallons
	instrDiv2byConst, idxDenomVolume,					// perform conversion of injector flow in liters to US gallons
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,
	instrDone											// return to caller
};

#endif // useCalculatedFuelFactor
#if defined(usePartialRefuel)
static const uint8_t prgmAddToPartialRefuel[] PROGMEM = {
	instrAddEEPROMtoX, 0x02, pRefuelSizeIdx,			// add existing partial refuel size parameter to what's in the result register
	instrDone											// return to caller
};

#endif // defined(usePartialRefuel)

static uint8_t parameterEdit::sharedFunctionCall(uint8_t cmd)
{

	uint8_t retVal = 0;
	uint8_t parameterPtr = numberEditObj.parameterIdx;
	uint8_t cp = parameterPtr - eePtrSettingsStart;

	switch (cmd)
	{

		case nesLoadInitial:
			SWEET64::runPrgm(prgmFetchParameterValue, parameterPtr);

		case nesLoadValue:
			ull2str(pBuff, 3, tFormatToNumber);
			parameterEdit::findLeft();

		case nesOnChange:
#if defined(useLCDcontrast)
			if (parameterPtr == pContrastIdx) LCD::setContrast((uint8_t)(str2ull(pBuff))); // adjust contrast dynamically
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
			if (parameterPtr == pLCDcolorIdx) LCD::setRGBcolor((uint8_t)(str2ull(pBuff))); // adjust backlight color dynamically
#endif // defined(useAdafruitRGBLCDshield)
			break;

		case nesSaveParameter:
			retVal = onEEPROMchange(prgmWriteParameterValue, parameterPtr);
			break;

		default:
			break;

	}

	return retVal;

}

static uint8_t parameterEdit::onEEPROMchange(const uint8_t * sched, uint8_t parameterIdx)
{

	uint8_t retVal;

	retVal = 0;

	metricFlag &= ~(detectEEPROMchangeFlag);

	SWEET64::runPrgm(sched, parameterIdx);

#ifdef useCalculatedFuelFactor
	if (metricFlag & calculateFuelParamFlag) SWEET64::runPrgm(prgmCalculateFuelFactor, 0); // calculate and store microseconds per US gallon factor

#endif // useCalculatedFuelFactor
	if (metricFlag & metricConversionFlag) SWEET64::runPrgm(prgmDoEEPROMmetricConversion, 0); // if metric flag has changed

	if (metricFlag & changeDisplayFlag)
	{

#if defined(useLCDcontrast)
		if (parameterIdx == pContrastIdx) LCD::setContrast(EEPROM::readByte(pContrastIdx)); // adjust contrast
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
		if ((parameterIdx == pLCDcolorIdx) && (brightnessIdx)) LCD::setRGBcolor(EEPROM::readByte(pLCDcolorIdx)); // adjust backlight color
#endif // defined(useAdafruitRGBLCDshield)

	}

	if (metricFlag & hardInitGuinoFlag) EEPROM::initGuinoHardware();

	if (metricFlag & softInitGuinoFlag) EEPROM::initGuinoSoftware();

	if (metricFlag & detectEEPROMchangeFlag) retVal = 1; // if the setting has changed

	return retVal;

}

static uint8_t parameterEdit::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuInitialEntryIdx:
			numberEditObj.neStatusMessage = pseStatusMessages;
			break;

		case menuCursorUpdateIdx:
			numberEditObj.parameterIdx = cursorPos + thisMenuData.menuTitlesOffset;
			parameterEdit::sharedFunctionCall(nesLoadInitial);
		case menuDisplayStatusIdx:
			retVal = 1;
			break;

		case menuDisplayOutputIdx:
			text::stringOut(devLCD, pBuff); // output supplementary information
			text::newLine(devLCD); // clear to the end of the line
			break;

		case menuDoSelectionIdx:
			numberEditObj.callingDisplayIdx = thisMenuData.displayIdx;
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

static void parameterEdit::displayHandler(uint8_t cmd, uint8_t cursorPos)
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

				c = pBuff[(unsigned int)(x)]; // fetch a character from buffer

				if (x > cursorPos) // if current position is past cursor position, just make sure leading spaces get turned into zero digits
				{

					if (c == ' ') pBuff[(unsigned int)(x)] = '0';

				}
				else // if current position is before or at cursor position, ensure leading spaces up to either cursor position or to first non-zero digit
				{

					if ((c == ' ') || (c == '0')) pBuff[(unsigned int)(x)] = k; // if character is either a space or a zero digit, it may be leading so change it
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

			if (timer0Status & t0sShowCursor)
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

			callDisplayHandler(numberEditObj.callingDisplayIdx, displayOutputIdx);

			pBuff[(uint16_t)(cursorPos)] = c; // restore existing character
			pBuff[10] = 0; // restore end of string

			break;

		default:
			break;

	}

}

static void parameterEdit::entry(void)
{

	numberEditObj.callingDisplayIdx = thisMenuData.displayIdx;

	cursor::moveAbsolute(parameterEditDisplayIdx, 255);

}

static void parameterEdit::findLeft(void)
{

	displayCursor[(unsigned int)(parameterEditDisplayIdx)] = 9;

	// do a nice thing and put the edit cursor at the first non zero number
	for (uint8_t x = 9; x < 10; x--) if (pBuff[(unsigned int)(x)] != ' ') displayCursor[(unsigned int)(parameterEditDisplayIdx)] = x;

}

static void parameterEdit::findRight(void)
{

	displayCursor[(unsigned int)(parameterEditDisplayIdx)] = 9;

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

	SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0);
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

	uint8_t cp = displayCursor[(unsigned int)(parameterEditDisplayIdx)];
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
			w = pBuff[(unsigned int)(cp)]; // fetch digit from stored numeric string representing parameter to be changed

			if (w == ' ') w = '0'; // if this is a leading space, use 0 as working digit
			w += digitDir; // adjust working digit
			if (w > '9') w = '0'; // handle working digit rollover
			if (w < '0') w = '9'; // handle working digit rollover

			pBuff[(unsigned int)(cp)] = w;

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

	uint8_t cp = displayCursor[(unsigned int)(parameterEditDisplayIdx)];
	uint8_t retVal;

	switch (cp)
	{

		case 10:
			str2ull(pBuff); // convert parameter buffer string into uint64_t
#if defined(usePartialRefuel)
			if (numberEditObj.parameterIdx == pRefuelSizeIdx) SWEET64::runPrgm(prgmAddToPartialRefuel, 0);
#endif // defined(usePartialRefuel)
			retVal = sharedFunctionCall(nesSaveParameter); // go save parameter and do any required housekeeping

			cursor::screenLevelEntry(numberEditObj.neStatusMessage, retVal, numberEditObj.callingDisplayIdx);
			break;

		case 11:
			cancel();
			break;

		default:
			cp = 10;
			displayCursor[(unsigned int)(parameterEditDisplayIdx)] = cp;
			displayHandler(displayCursorUpdateIdx, cp);
			break;

	}

}

static void parameterEdit::cancel(void)
{

	uint8_t cp = displayCursor[(unsigned int)(parameterEditDisplayIdx)];
	const char * str;

	if (cp != 11)
	{

		cp = 11;
		displayCursor[(unsigned int)(parameterEditDisplayIdx)] = cp;
		displayHandler(displayCursorUpdateIdx, cp);

	}
	else
	{

		cursor::screenLevelEntry(numberEditObj.neStatusMessage, 2, numberEditObj.callingDisplayIdx);

	}

}

