static calcFuncObj translateCalcIdx(uint16_t tripCalc, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	union union_16 * tC = (union union_16 *)(&tripCalc);
	uint8_t tripIdx;
	uint8_t calcIdx;

	calcIdx = tC->u8[0];
	tripIdx = tC->u8[1];

	return translateCalcIdx(tripIdx, calcIdx, strBuff, windowLength, decimalFlag);

}

static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	calcFuncObj thisCalcFuncObj;

	uint8_t calcFmtIdx;
	uint8_t i;

	thisCalcFuncObj.isValid = 0;

	if (tripIdx < tripSlotTotalCount) thisCalcFuncObj.isValid ^= (isValidTripIdx);

	if (calcIdx < dfMaxValDisplayCount)
	{

		thisCalcFuncObj.isValid ^= (isValidCalcIdx);

		if (activityFlags & afSwapFEwithFCR) // do fuel consumption rate swap with fuel economy here
		{

			if ((tripIdx == instantIdx) && (calcIdx == tFuelEcon)) calcIdx = tFuelRate;

		}

		thisCalcFuncObj.tripIdx = tripIdx;
		thisCalcFuncObj.calcIdx = calcIdx;

		thisCalcFuncObj.suppressTripLabel = ((calcIdx < dfMaxValTripFunction) ? 0 : 0x80);

		calcFmtIdx = getCalcFormatIdx(calcIdx);

		thisCalcFuncObj.calcFmtIdx = calcFmtIdx;
		thisCalcFuncObj.decimalPlaces = pgm_read_byte(&calcFormatDecimalPlaces[(uint16_t)(calcFmtIdx)]) & 0x0F; // strip off useBluetooth supplemental data
		if (thisCalcFuncObj.suppressTripLabel) thisCalcFuncObj.tripChar = ' ';
		else thisCalcFuncObj.tripChar = pgm_read_byte(&tripFormatLabelText[(uint16_t)(tripIdx)]);
		thisCalcFuncObj.calcChar = pgm_read_byte(&calcFormatLabelText[(uint16_t)(calcFmtIdx)]);

	}

	if (thisCalcFuncObj.isValid & isValidCalcObj) thisCalcFuncObj.isValid ^= (isValidFlag);

	thisCalcFuncObj.strBuffer = strBuff;

	if (thisCalcFuncObj.isValid)
	{

#if defined(useDebugTerminal) || defined(useJSONoutput)
		thisCalcFuncObj.calcFormatLabelPtr = findStr(calcFormatLabels, thisCalcFuncObj.calcFmtIdx);
#endif // defined(useDebugTerminal) || defined(useJSONoutput)

		if (thisCalcFuncObj.calcFmtIdx == calcFormatTimeHHmmSSIdx)
		{

			ull2str(strBuff, thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);

			if (windowLength > 6)
			{

				i = 6;

				for (uint8_t x = windowLength; x <= windowLength; x--) strBuff[(uint16_t)(x)] = ((i < 7) ? strBuff[(uint16_t)(i--)] : ' ');

			}

		}
		else
		{

			thisCalcFuncObj.value = SWEET64::doCalculate(thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx); // perform calculation
			ull2str(strBuff, thisCalcFuncObj.decimalPlaces, windowLength, decimalFlag); // format output for window length and number of decimal places

		}

	}
	else
	{

		strcpy_P(strBuff, overFlowStr);
		thisCalcFuncObj.strBuffer[(uint16_t)(windowLength)] = 0;

	}

	return thisCalcFuncObj;

}

static uint8_t getCalcFormatIdx(uint8_t calcIdx)
{

	uint8_t calcFmtIdx;

	calcFmtIdx = pgm_read_byte(&calcFormatList[(uint16_t)(calcIdx)]); // read calculation format index
	if ((calcFmtIdx >= calcFormatMaxValNonConversion) && (metricFlag & metricMode)) calcFmtIdx++; // shift index up one if this is an SI/SAE format
	if ((calcFmtIdx >= calcFormatMaxValSingleFormat) && (metricFlag & alternateFEmode)) calcFmtIdx += 2; // shift index up one if this has two separate formats

	return calcFmtIdx;

}

#if defined(useDebugTerminal) || defined(useJSONoutput)
static void outputTripFunctionValue(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	uint8_t c;

	calcFuncObj thisCalcFuncObj;

	// perform the required decimal formatting
	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, strBuff, windowLength, decimalFlag);

	text::stringOut(dev, thisCalcFuncObj.strBuffer); // output the number

	if ((decimalFlag & dfSuppressLabel) == 0)
	{

		text::charOut(dev, ' ');
		text::stringOut(dev, thisCalcFuncObj.calcFormatLabelPtr);

		if (decimalFlag & dfOutputTripChar) text::charOut(dev, thisCalcFuncObj.tripChar);

	}

}

#endif // defined(useDebugTerminal) || defined(useJSONoutput)
