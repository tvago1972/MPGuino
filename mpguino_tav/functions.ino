static const uint8_t prgmFormatToH9MMSStime[] PROGMEM = {
	instrLdReg, 0x21,									// move time in seconds into register 1
	instrDoBCDadjust, 0x12, bcdFormatH9MMSS,			// process register 1 as hhmmss BCD string and store it in register 2
	instrDone											// exit to caller
};


static void translateCalcIdx(uint16_t tripCalc, uint8_t windowLength, uint8_t decimalFlag)
{

	union union_16 * tC = (union union_16 *)(&tripCalc);

	translateCalcIdx(tC->u08[1], tC->u08[0], windowLength, decimalFlag);

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

		if (v08(v8ActivityIdx) & afVehicleIdleFlag) // do fuel consumption rate swap with fuel economy here
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
			if ((calcFmtIdx >= calcFormatMaxValNonConversion) && (m08(m8MetricModeFlags) & mmDisplayMetric)) calcFmtIdx++;

			// shift index up two if this has two separate formats
			if ((calcFmtIdx >= calcFormatMaxValSingleFormat) && (m08(m8MetricModeFlags) & mmDisplayAlternateFE)) calcFmtIdx += 2;

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

		// perform calculation
#if defined(useSWEET64RAMprograms)
		mainCalcFuncVar.value = SWEET64::runPrgm(SWEET64::getProgramPC(mainCalcFuncVar.calcIdx), mainCalcFuncVar.tripIdx);
#else // defined(useSWEET64RAMprograms)
		mainCalcFuncVar.value = SWEET64::runPrgm(SWEET64::getProgramPointer(mainCalcFuncVar.calcIdx), mainCalcFuncVar.tripIdx);
#endif // defined(useSWEET64RAMprograms)

		if (mainCalcFuncVar.calcFmtIdx == calcFormatTimeH9mmSSIdx)
		{

			ull2str(nBuff, 0, S64_PRGM_PTR(prgmFormatToH9MMSStime));

			if (windowLength > 6)
			{

				i = 6;

				for (uint8_t x = windowLength; x <= windowLength; x--) nBuff[(uint16_t)(x)] = ((i < 7) ? nBuff[(uint16_t)(i--)] : ' ');

			}

		}
		else
		{

			ull2str(nBuff, mainCalcFuncVar.decimalPlaces, windowLength, decimalFlag); // format output for window length and number of decimal places

		}

	}
	else
	{

		strcpy_P(nBuff, overFlowStr);
		nBuff[(uint16_t)(windowLength)] = 0;

	}

}

