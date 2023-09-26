static void translateCalcIdx(uint16_t tripCalc, uint8_t windowLength, uint8_t decimalFlag)
{

	union union_16 * tC = (union union_16 *)(&tripCalc);

	translateCalcIdx(tC->u8[1], tC->u8[0], windowLength, decimalFlag);

}

static void translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalFlag)
{

	uint8_t calcFmtIdx;
	uint8_t i;

	mainCalcFuncVar.isValid = 0;

	if (tripIdx < tripSlotTotalCount) mainCalcFuncVar.isValid ^= (isValidTripIdx);

	if (calcIdx < dfMaxValCalcCount)
	{

		mainCalcFuncVar.isValid ^= (isValidCalcIdx);

		if (volatile8Variables[(uint16_t)(v8Activity - v8VariableStartIdx)] & afSwapFEwithFCR) // do fuel consumption rate swap with fuel economy here
		{

			if ((tripIdx == instantIdx) && (calcIdx == tFuelEcon)) calcIdx = tFuelRate;

		}

		mainCalcFuncVar.tripIdx = tripIdx;
		mainCalcFuncVar.calcIdx = calcIdx;

		mainCalcFuncVar.suppressTripLabel = ((calcIdx < dfMaxValTripFunction) ? 0 : 0x80);

		if (calcIdx < dfMaxValDisplayCount)
		{

			calcFmtIdx = pgm_read_byte(&calcFormatList[(uint16_t)(calcIdx)]); // read calculation format index

			// shift index up one if this is an SI/SAE format
			if ((calcFmtIdx >= calcFormatMaxValNonConversion) && (mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayMetric)) calcFmtIdx++;

			// shift index up two if this has two separate formats
			if ((calcFmtIdx >= calcFormatMaxValSingleFormat) && (mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayAlternateFE)) calcFmtIdx += 2;

		}
		else calcFmtIdx = calcFormatTimeInMillisecondsIdx;

		mainCalcFuncVar.calcFmtIdx = calcFmtIdx;

		i = pgm_read_byte(&calcFormatDecimalPlaces[(uint16_t)(calcFmtIdx)]);

		if (((decimalFlag & dfOutputBluetooth) == dfOutputBluetooth) && (i & 0xF0)) mainCalcFuncVar.decimalPlaces = (i >> 4); // fetch useBluetooth supplemental data, if present
		else mainCalcFuncVar.decimalPlaces = (i & 0x0F); // strip off useBluetooth supplemental data

		if (mainCalcFuncVar.suppressTripLabel) mainCalcFuncVar.tripChar = ' ';
		else mainCalcFuncVar.tripChar = pgm_read_byte(&tripFormatLabelText[(uint16_t)(tripIdx)]);

		mainCalcFuncVar.calcChar = pgm_read_byte(&calcFormatLabelText[(uint16_t)(calcFmtIdx)]);

	}

	if (mainCalcFuncVar.isValid & isValidCalcObj) mainCalcFuncVar.isValid ^= (isValidFlag);

	if (mainCalcFuncVar.isValid)
	{

#if defined(useDebugTerminal) || defined(useJSONoutput)
		mainCalcFuncVar.calcFormatLabelPtr = findStr(calcFormatLabels, mainCalcFuncVar.calcFmtIdx);
#endif // defined(useDebugTerminal) || defined(useJSONoutput)

		if (mainCalcFuncVar.calcFmtIdx == calcFormatTimeHHmmSSIdx)
		{

			ull2str(nBuff, mainCalcFuncVar.tripIdx, mainCalcFuncVar.calcIdx);

			if (windowLength > 6)
			{

				i = 6;

				for (uint8_t x = windowLength; x <= windowLength; x--) nBuff[(uint16_t)(x)] = ((i < 7) ? nBuff[(uint16_t)(i--)] : ' ');

			}

		}
		else
		{

			mainCalcFuncVar.value = SWEET64::doCalculate(mainCalcFuncVar.tripIdx, mainCalcFuncVar.calcIdx); // perform calculation
			ull2str(nBuff, mainCalcFuncVar.decimalPlaces, windowLength, decimalFlag); // format output for window length and number of decimal places

		}

	}
	else
	{

		strcpy_P(nBuff, overFlowStr);
		nBuff[(uint16_t)(windowLength)] = 0;

	}

}

