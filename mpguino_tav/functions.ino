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

		calcFmtIdx = pgm_read_byte(&calcFormatList[(uint16_t)(calcIdx)]);
		if ((calcIdx >= dfMaxValNonConversion) && (metricFlag & metricMode)) calcFmtIdx++; // shift index up one if this is an SI/SAE format
		if ((calcIdx >= dfMaxValSingleFormat) && (metricFlag & alternateFEmode)) calcFmtIdx += 2; // shift index up one if this has two separate formats

		thisCalcFuncObj.calcFmtIdx = calcFmtIdx;
		i = pgm_read_byte(&calcFormatDecimalPlaces[(uint16_t)(calcFmtIdx)]);
		thisCalcFuncObj.decimalPlaces = i & 0x7F;
		thisCalcFuncObj.suppressTripLabel = i & 0x80;
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

#if defined(useDebugTerminal) || defined(useJSONoutput)
static void outputTripFunctionValue(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

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
