static calcFuncObj translateCalcIdx(uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	calcFuncObj thisCalcFuncObj;

	uint8_t calcFmtIdx;

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
		thisCalcFuncObj.decimalPlaces = pgm_read_byte(&calcFormatDecimalPlaces[(unsigned int)(calcFmtIdx)]);
		thisCalcFuncObj.calcChar = pgm_read_byte(&calcFormatLabelText[(unsigned int)(calcFmtIdx)]);

	}

	if (thisCalcFuncObj.isValid & isValidCalcObj) thisCalcFuncObj.isValid ^= (isValidFlag);

	thisCalcFuncObj.strBuffer = strBuff;

	if (thisCalcFuncObj.isValid)
	{


		if (thisCalcFuncObj.calcFmtIdx == calcFormatTimeHHmmSSIdx) ull2str(thisCalcFuncObj.calcIdx, strBuff, thisCalcFuncObj.tripIdx);
		else
		{

			SWEET64::doCalculate(thisCalcFuncObj.tripIdx, thisCalcFuncObj.calcIdx);
			formatDecimal(strBuff, windowLength, thisCalcFuncObj.decimalPlaces, decimalFlag);

		}

	}
	else
	{

		strcpy_P(strBuff, overFlowStr);
		thisCalcFuncObj.strBuffer[(unsigned int)(windowLength)] = 0;

	}

	return thisCalcFuncObj;

}

#ifdef useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
#else // useSpiffyTripLabels
static void displayMainScreenFunctions(const uint8_t localScreenFormatList[][2], uint8_t cursorPos)
#endif // useSpiffyTripLabels
{

	uint32_t screenFormatValue;
	union union_32 * sfvp = (union union_32 *) &screenFormatValue;
	uint16_t sfIdx;
	uint8_t tripIdx;
	uint8_t calcIdx;

	sfIdx = cursorPos * 4;

	for (uint8_t x = 0; x < 4; x++)
	{

		tripIdx = pgm_read_byte(&localScreenFormatList[sfIdx + x][0]);
		calcIdx = pgm_read_byte(&localScreenFormatList[sfIdx + x][1]);

#ifdef useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, localTripBitmask);
#else // useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx);
#endif // useSpiffyTripLabels

	}

#ifdef useSpiffyTripLabels
	LCD::flushCGRAM();

#endif // useSpiffyTripLabels
}

#ifdef useSpiffyTripLabels
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t tripBlink, uint8_t calcBlink, const uint8_t localTripBitmask[][4])
#else // useSpiffyTripLabels
static void displayFunction(uint8_t readingIdx, uint8_t tripIdx, uint8_t calcIdx)
#endif // useSpiffyTripLabels
{

	calcFuncObj thisCalcFuncObj;

	uint8_t x;
	uint8_t y;
#ifdef useSpiffyTripLabels
	uint8_t tripBitmask;
	uint8_t calcBitmask;
	uint8_t tripPart;
	uint8_t calcPart;
	uint8_t translateFlag;
	uint8_t localTripIdx;

	translateFlag = calcIdx & 0x80; // save trip index translation bit

	if (calcIdx & 0x80)
	{

		localTripIdx = tripIdx;
		calcIdx &= 0x7F;

	}
	else
	{

		localTripIdx = (tripIdx & dfTripMask) >> dfBitShift; // extract local trip index
		tripIdx = (tripIdx & dfFunctionMask);  // extract calculation trip index

	}
#endif // useSpiffyTripLabels

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, 6, 0);

	readingIdx &= 3;
	x = (readingIdx & 1) << 3;
	y = (readingIdx & 2) >> 1;

	text::gotoXY(devLCD, x, y);
	text::stringOut(devLCD, thisCalcFuncObj.strBuffer);

#ifdef useSpiffyTripLabels
	readingIdx <<= 1;

	text::charOut(devLCD, 0xF0 + readingIdx);
	text::charOut(devLCD, 0xF1 + readingIdx);

	tripBitmask = ((mainLoopHeartBeat & tripBlink) ? 0 : 0x1F); // determine if trip label component should blink or not
	calcBitmask = ((mainLoopHeartBeat & calcBlink) ? 0 : 0x1F); // determine if function label component should blink or not

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

#else // useSpiffyTripLabels
	text::charOut(devLCD, thisCalcFuncObj.tripChar);
	text::charOut(devLCD, thisCalcFuncObj.calcChar);

#endif // useSpiffyTripLabels
}
