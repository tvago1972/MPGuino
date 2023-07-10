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

	if (tripIdx < tripSlotTotalCount)
	{

		thisCalcFuncObj.isValid ^= (isValidTripIdx);
		thisCalcFuncObj.tripChar = pgm_read_byte(&tripVarChars[(uint16_t)(tripIdx)]);

	}

	if (calcIdx < dfMaxValDisplayCount)
	{

		thisCalcFuncObj.isValid ^= (isValidCalcIdx);

		if (activityFlags & afSwapFEwithFCR) // do fuel consumption rate swap with fuel economy here
		{

			if ((tripIdx == instantIdx) && (calcIdx == tFuelEcon)) calcIdx = tFuelRate;

		}

		thisCalcFuncObj.tripIdx = tripIdx;
		thisCalcFuncObj.calcIdx = calcIdx;

		calcFmtIdx = pgm_read_byte(&calcFormatIdx[(unsigned int)(calcIdx)]);
		if ((calcIdx >= dfMaxValNonConversion) && (metricFlag & metricMode)) calcFmtIdx++; // shift index up one if this is an SI/SAE format
		if ((calcIdx >= dfMaxValSingleFormat) && (metricFlag & alternateFEmode)) calcFmtIdx += 2; // shift index up one if this has two separate formats

		thisCalcFuncObj.calcFmtIdx = calcFmtIdx;
		i = pgm_read_byte(&calcFormatDecimalPlaces[(unsigned int)(calcFmtIdx)]);
		thisCalcFuncObj.decimalPlaces = i & 0x7F;
		thisCalcFuncObj.suppressTripLabel = i & 0x80;
		thisCalcFuncObj.calcChar = pgm_read_byte(&calcFormatLabelText[(unsigned int)(calcFmtIdx)]);

	}

	if (thisCalcFuncObj.isValid & isValidCalcObj) thisCalcFuncObj.isValid ^= (isValidFlag);

	thisCalcFuncObj.strBuffer = strBuff;

	if (thisCalcFuncObj.isValid)
	{


		if (thisCalcFuncObj.calcFmtIdx == calcFormatTimeHHmmSSIdx) ull2str(strBuff, thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);
		else
		{

			SWEET64::doCalculate(thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);
			ull2str(strBuff, thisCalcFuncObj.decimalPlaces, windowLength, decimalFlag);

		}

	}
	else
	{

		strcpy_P(strBuff, overFlowStr);
		thisCalcFuncObj.strBuffer[(unsigned int)(windowLength)] = 0;

	}

	return thisCalcFuncObj;

}

