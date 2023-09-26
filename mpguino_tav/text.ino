/* text support section */

static void text::initDev(uint8_t devIdx, uint8_t devStatus, void (* charOut)(uint8_t), uint8_t (* charIn)(void))
{

	deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut = charOut;
	deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrIn = charIn;
	mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (devStatus);

}

static void text::initDev(uint8_t devIdx, uint8_t devStatus, void (* charOut)(uint8_t))
{

	deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut = charOut;
	mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (devStatus);

}

static uint8_t text::chrIn(uint8_t devIdx)
{

	uint8_t retVal;

	if (deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrIn) retVal = deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrIn();
	else retVal = 0;

	return retVal;

}

static void text::gotoXY(uint8_t devIdx, uint8_t xPos, uint8_t yPos)
{

	charOut(devIdx, 0x80 + yPos * 20 + xPos);

}

static uint8_t text::charOut(uint8_t devIdx, uint8_t chr, uint8_t chrCount)
{

	while (chrCount--) charOut(devIdx, chr);

}

static void text::newLine(uint8_t devIdx)
{

	charOut(devIdx, 0x0D);

}

static uint8_t text::charOut(uint8_t devIdx, uint8_t chr)
{

	uint8_t retVal;

	retVal = 1;

	if (deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut)
	{

		switch (chr)
		{

			case 0x00:	// tcEOS, end-of-string
				retVal = 0;
			case 0xED:	// tcOON, enable device output
				mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (odvFlagEnableOutput);
				break;

			case 0xEB:	// tcOMOFF, disable device output for metric mode
				if (mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayMetric) mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] &= ~(odvFlagEnableOutput);
				else mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (odvFlagEnableOutput);
				break;

			case 0xEC:	// tcOTOG, toggle device output enable
				mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] ^= (odvFlagEnableOutput);
				break;

			case 0xEE:	// tcOOFF, disable device output
				mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] &= ~(odvFlagEnableOutput);
				break;

			case 0x0D:	// tcEOSCR, output carriage return, defined as end of string
				retVal = 0;
			case 0xEF:	// tcCR, output carriage return not at end of string
				mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (odvFlagEnableOutput);
				deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut(0x0D);
				if (mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] & odvFlagCRLF) deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut(0x0A);
				break;

			case 0xF0 ... 0xF7: // print CGRAM character
				chr &= 0x07;
			case 0x20 ... 0x7F: // print normal character
				if (mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] & odvFlagEnableOutput) deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut(chr);
				break;

			default:
				deviceDefs[(uint16_t)(devIdx - m8DevStartIdx)].chrOut(chr);
				break;

		}

	}
	else retVal = 0;

	return retVal;

}

static void text::statusOut(uint8_t devIdx, const char * sList, uint8_t strIdx, const char * str)
{

	initStatus(devIdx);
	stringOut(devIdx, findStr(sList, strIdx));
	stringOut(devIdx, str);
	commitStatus(devIdx);

#if defined(useDebugTerminal)
	if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekStatusMessage)
	{

		stringOut(m8DevDebugTerminalIdx, findStr(sList, strIdx));
		stringOut(m8DevDebugTerminalIdx, str);
		newLine(m8DevDebugTerminalIdx);

	}

#endif // defined(useDebugTerminal)
}

static void text::statusOut(uint8_t devIdx, const char * str, const char * sList, uint8_t strIdx)
{

	initStatus(devIdx);
	stringOut(devIdx, str);
	stringOut(devIdx, findStr(sList, strIdx));
	commitStatus(devIdx);

#if defined(useDebugTerminal)
	if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekStatusMessage)
	{

		stringOut(m8DevDebugTerminalIdx, str);
		stringOut(m8DevDebugTerminalIdx, findStr(sList, strIdx));
		newLine(m8DevDebugTerminalIdx);

	}

#endif // defined(useDebugTerminal)
}

static void text::statusOut(uint8_t devIdx, const char * sList, uint8_t strIdx)
{

	statusOut(devIdx, findStr(sList, strIdx));

}

static void text::statusOut(uint8_t devIdx, const char * str)
{

	initStatus(devIdx);
	stringOut(devIdx, str);
	commitStatus(devIdx);

#if defined(useDebugTerminal)
	if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekStatusMessage)
	{

		stringOut(m8DevDebugTerminalIdx, str);
		newLine(m8DevDebugTerminalIdx);

	}

#endif // defined(useDebugTerminal)
}

static void text::initStatus(uint8_t devIdx)
{

	heart::delayS(0);

#if defined(blankScreenOnMessage)
	charOut(devIdx, 0x0C); // clear the entire screen
#else // defined(blankScreenOnMessage)
	gotoXY(devIdx, 0, 0); // go to the first line
#endif // defined(blankScreenOnMessage)

}

static void text::commitStatus(uint8_t devIdx)
{

	newLine(devIdx);
	heart::delayS(delay0Tick2000ms);

}

static void text::stringOut(uint8_t devIdx, const char * str, uint8_t strIdx)
{

	stringOut(devIdx, findStr(str, strIdx));

}

static void text::stringOut(uint8_t devIdx, const char * str)
{

	while (charOut(devIdx, pgm_read_byte(str++))) ;

}

static void text::stringOut(uint8_t devIdx, char * str)
{

	while (charOut(devIdx, * str++));

}

static void text::stringOutIf(uint8_t devIdx, uint8_t condition, const char * str, uint8_t strIdx)
{

	if (condition) mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (odvFlagEnableOutput);
	else mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] &= ~(odvFlagEnableOutput);

	stringOut(devIdx, str, strIdx);

}

static void text::stringOutIf(uint8_t devIdx, uint8_t condition, const char * str)
{

	if (condition) mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] |= (odvFlagEnableOutput);
	else mainProgram8Variables[(uint16_t)(devIdx - m8VariableStartIdx)] &= ~(odvFlagEnableOutput);

	stringOut(devIdx, str);

}

static void text::hexNybbleOut(uint8_t devIdx, uint8_t val)
{

	charOut(devIdx, nybble(val));

}

static uint8_t text::nybble(uint8_t val)
{

	val &= 0x0F;
	val |= 0x30;
	if (val > 0x39) val += 0x07;
	return val;

}

static void text::hexByteOut(uint8_t devIdx, uint8_t val)
{

	hexNybbleOut(devIdx, val >> 4);
	hexNybbleOut(devIdx, val);

}

static void text::hexWordOut(uint8_t devIdx, uint16_t val)
{

	union union_16 * vee = (union union_16 *)(&val);

	for (uint8_t i = 1; i < 2; i--) hexByteOut(devIdx, vee->u8[i]);

}

static void text::hexDWordOut(uint8_t devIdx, uint32_t val)
{

	union union_32 * vee = (union union_32 *)(&val);

	hexWordOut(devIdx, vee->ui[1]);
	hexWordOut(devIdx, vee->ui[0]);

}

static void text::hexLWordOut(uint8_t devIdx, uint64_t * val)
{

	union union_64 * vee = (union union_64 *)(val);

	for (uint8_t i = 7; i < 8; i--) hexByteOut(devIdx, vee->u8[i]);

}

static void text::tripFunctionOut(uint8_t devIdx, uint16_t tripCalc, uint8_t windowLength, uint8_t decimalFlag)
{

	union union_16 * tC = (union union_16 *)(&tripCalc);

	tripFunctionOut(devIdx, tC->u8[1], tC->u8[0], windowLength, decimalFlag);

}

static void text::tripFunctionOut(uint8_t devIdx, uint8_t tripIdx, uint8_t calcIdx, uint8_t windowLength, uint8_t decimalFlag)
{

	uint8_t i;

	// perform the required decimal formatting
	translateCalcIdx(tripIdx, calcIdx, windowLength, decimalFlag);

	if (mainCalcFuncVar.isValid & isValidFlag)
	{

		if (decimalFlag & dfBlinkCalc) mainCalcFuncVar.calcChar = ' ';
		if (decimalFlag & dfBlinkTrip) mainCalcFuncVar.tripChar = ' ';

		i = 0;

		do
		{

			if ((decimalFlag & dfOutputTagFirst) ^ i)
			{

				if (decimalFlag & dfOutputTag)
				{

#if defined(useSpiffyTripLabels)
					if (decimalFlag & dfOutputSpiffyTag)
					{

						charOut(devIdx, 0xF0 + mainCalcFuncVar.labelIdx);
						charOut(devIdx, 0xF1 + mainCalcFuncVar.labelIdx);

					}
					else
					{

						charOut(devIdx, mainCalcFuncVar.tripChar);
						charOut(devIdx, mainCalcFuncVar.calcChar);

					}

#else // defined(useSpiffyTripLabels)
					charOut(devIdx, mainCalcFuncVar.tripChar);
					charOut(devIdx, mainCalcFuncVar.calcChar);

#endif // defined(useSpiffyTripLabels)
				}

			}
			else
			{

				if (decimalFlag & dfBlinkCalc) charOut(devIdx, ' ', windowLength); // output blanks corresponding to number
				else numberOut(devIdx, decimalFlag);

			}

		}
		while ((i++) < 2);

	}
	else
	{

		if (windowLength)
		{

			if (decimalFlag & dfOutputTag) windowLength += 2;

			charOut(devIdx, ' ', windowLength);

		}

	}

}

static void text::numberOut(uint8_t devIdx, uint8_t decimalFlag)
{

	uint8_t c;
	uint8_t f;
	char * strBuffer;

	if ((decimalFlag & dfOutputBluetooth) == dfOutputBluetooth)
	{

		f = 0;
		strBuffer = nBuff;

		do
		{

			c = * strBuffer++;

			if (((c >= '1') && (c <= '9')) || ((* strBuffer) == 0)) f = 1;

			if ((c != '.') && (f)) c = charOut(devIdx, c);

		}
		while (c);

	}
	else stringOut(devIdx, nBuff); // output the number

#if defined(useDebugTerminal) || defined(useJSONoutput)
	if ((decimalFlag & dfOutputLabelCheck) == dfOutputLabel)
	{

		charOut(devIdx, ' ');
		stringOut(devIdx, mainCalcFuncVar.calcFormatLabelPtr);

	}

#endif // defined(useDebugTerminal) || defined(useJSONoutput)
}

static const char * findStr(const char * str, uint8_t strIdx)
{

	uint8_t c;

	while (strIdx)
	{

		do c = pgm_read_byte(str++);
		while ((c) && (c != 0x0D));

		strIdx--;

	}

	return str;

}

static unsigned long str2ull(char * strBuffer)
{

	uint8_t c;
	uint8_t n;

	uint8_t x;
	uint8_t f;
	uint8_t loopFlag;

	x = 0;
	n = 0;
	f = 1;
	loopFlag = 1;

	SWEET64::init64byt((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), 0); // initialize 64-bit number to zero

	while ((loopFlag) && (x < 17))
	{

		if ((c = strBuffer[(uint16_t)(x++)])) // if a non-NULL character is read in
		{

			n *= 10; // shift accumulator left one digit
			if (c != ' ') n += (uint8_t)(c) - 48; // if character is not a leading space, go add it to accumulator
			f= 1 - f; // flip-flop the SWEET64 addition flag

			if (f)
			{

				SWEET64::runPrgm(prgmMultiplyBy100, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 100 + n
				n = 0;

			}

		}
		else loopFlag = 0; // otherwise, terminate loop upon receipt of a NULL character

	}

	if (f == 0) SWEET64::runPrgm(prgmMultiplyBy10, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 10 + n

	return ((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]))->ul[0];

}

static void storeDigit(uint8_t value, char * strBuffer, uint8_t &strPos, uint8_t &decPos, char &zeroChar, uint8_t &digCnt, uint8_t flg)
{

	decPos--; // bump down decimal position

	if ((value) || (decPos == 1)) // if either this digit is not zero, or at 1s position
	{

		value += '0'; // shift value into character 0-9 range
		zeroChar = '0'; // turn leading zero character into a 0

	}
	else value = zeroChar; // otherwise, use previously defined leading zero character

	if (value != ' ') digCnt++;

	if ((decPos == 0) && (flg)) strBuffer[(uint16_t)(strPos++)] = '.'; // if at 1/10ths position, store a decimal point
	strBuffer[(uint16_t)(strPos++)] = value; // store converted character value

}

// converts the 64-digit number stored in SWEET64 register 2 into an up-to 10-digit decimal number string
// a SWEET64 indexed program is called to do the initial processing of the number from 64-bit to an initial BCD string
// the SWEET64 program must specify the leading zero character and the number of BCD bytes
//
// the returned string will always have at least one digit at the rightmost string position
// the string is terminated with a NULL character
//
// this routine can handle numbers up to 9999999999
//
// if called with prgmIdx = tRoundOffNumber, also inserts the decimal point in the string specified by the value in decimalPlaces
//
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t prgmIdx)
{

	union union_64 * tmpPtr2 = (union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]);

	uint8_t l;
	uint8_t value;
	uint8_t strPos;
	uint8_t decPos;
	uint8_t digCnt;
	uint8_t flg;
	char zeroChar;

	SWEET64::doCalculate(decimalPlaces, prgmIdx); // call SWEET64 routine to perform decimal point rounding to next nearest decimal place

	l = tmpPtr2->u8[6];	// load total length of binary-coded decimal bytes of converted number

	if (l == 255) strcpy_P(strBuffer, overFlowStr); // if length is 255, this number overflowed
	else
	{

		if (prgmIdx == tRoundOffNumber) flg = 1; // if using tRoundOffNumber to process number, do decimal conversion
		else flg = 0;

		if (flg) decPos = 11 - decimalPlaces; // if using tRoundOffNumber, compute decimal position
		else decPos = 11;

		zeroChar = (char)(tmpPtr2->u8[7]);	// load leading zero character
		strPos = 0; // set initial string buffer position
		digCnt = 0; // set initial digit count

		for (uint8_t x = 0; x < l; x++) // go through all of the binary-coded decimal bytes of converted number
		{

			value = tmpPtr2->u8[(uint16_t)(x)];	// load a binary-coded decimal byte of number

			storeDigit(value / 10, strBuffer, strPos, decPos, zeroChar, digCnt, flg); // store 10's place digit in string buffer
			storeDigit(value % 10, strBuffer, strPos, decPos, zeroChar, digCnt, flg); // store 1's place digit in string buffer

		}

		strBuffer[(uint16_t)(strPos++)] = 0; // mark end of string buffer with a NULL character
		strBuffer[(uint16_t)(strPos++)] = digCnt; // store digit count at 1 past the string buffer end

	}

	return strBuffer; // return pointer to string buffer containing the number

}

// converts the 64-digit number stored in SWEET64 register 2 into an up-to 10-digit decimal number string
//
// this routine is called to perform initial conversion into a 10 character long string containing
//    the up-to 10 digit number with inserted decimal point as appropriate, and leading spaces
//
// this routine will autorange the number being converted to automatically support the number of decimal digits being output
//    unless disabled
//
// if no window length is specified, this routine just removes all leading spaces
//
// decimalFlag currently has three bits defined within ull2str below:
// 1xxx xxxx - fill overflow string from all 9s instead of all '-' characters
// x1xx xxxx - ignore decimal point character in window length consideration (used with useBigNumberDisplay)
// xx1x xxxx - suppress autoranging (used with useBluetooth)
//
// the following bits are also defined in decimalFlag, but are used in other functions instead of ull2str below
// xxx1 xxxx - output trip label (used in tripFunctionOut, with useJSONoutput and useDebugTerminal)
// xxxx 1xxx - suppress leading spaces/zeros/decimal point (used in tripFunctionOut, with useBluetooth)
//
// sample debug monitor outputs:
//
// (overflow='-', do not ignore decimal point)                (overflow='9', ignore decimal point)
//
// ]6<2.0u                                                    ]6<2.c0u
// decimalFlags=00, windowLen=06, places=02                   decimalFlags=C0, windowLen=06, places=02
// 00: 0000000000000005 -   0.01                              00: 0000000000000005 -    0.01
// 01: 0000000000000037 -   0.06                              01: 0000000000000037 -    0.06
// 02: 000000000000022B -   0.56                              02: 000000000000022B -    0.56
// 03: 00000000000015B3 -   5.56                              03: 00000000000015B3 -    5.56
// 04: 000000000000D903 -  55.56                              04: 000000000000D903 -   55.56
// 05: 0000000000087A23 - 555.56                              05: 0000000000087A23 -  555.56
// 06: 000000000054C563 - 5555.6                              06: 000000000054C563 - 5555.56
// 07: 00000000034FB5E3 -  55556                              07: 00000000034FB5E3 - 55555.6
// 08: 00000000211D1AE3 - 555556                              08: 00000000211D1AE3 - 555556
// 09: 000000014B230CE3 - ------                              09: 000000014B230CE3 - 999999
// 0A: 0000000CEF5E80E3 - ------                              0A: 0000000CEF5E80E3 - 999999
// 0B: 0000008159B108E3 - ------                              0B: 0000008159B108E3 - 999999
// 0C: 0000050D80EA58E3 - ------                              0C: 0000050D80EA58E3 - 999999
// 0D: 00003287092778E3 - ------                              0D: 00003287092778E3 - 999999
//
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t windowLength, uint8_t decimalFlag) // format number for output
{

	union union_64 * tmpPtr3 = (union union_64 *)(&s64reg[(uint16_t)(s64reg64_3)]);

	uint8_t d;
	uint8_t e;
	uint8_t f;

	if (decimalPlaces > 3) decimalPlaces = 0; // if too many decimal digits specified, reset to 0

	if ((windowLength > 10) || (windowLength < 2)) windowLength = 0; // validity check for window length
	else if (decimalPlaces > (windowLength - 1)) decimalPlaces = windowLength - 1; // ensure complete number fits window

	f = ((decimalFlag & dfIgnoreDecimalPoint) ? 0 : 1); // shrink window if decimal point is considered

	SWEET64::runPrgm(prgmAutoRangeNumber, windowLength - f); // fetch supportable decimal digit count for window
	d = tmpPtr3->u8[0];
	f = 0; // initially signal no overflow occurred

	if (decimalPlaces > d)
	{

		if (decimalFlag & dfSuppressAutoRange) f = 1;
		else decimalPlaces = d; // if supportable digit count is less than specified, use supportable instead

	}

	if (f == 0) ull2str(strBuffer, decimalPlaces, tRoundOffNumber); // perform rounding of number to nearest decimal place, then format for ASCII output and insert a decimal point

	if ((strBuffer[2] == '-') || (f)) f = 1; // if number overflowed
	else
	{

		d = 0;

		if (windowLength) // if there is a valid windowLength
		{

			e = ((decimalPlaces) ? strBuffer[12] - decimalPlaces: strBuffer[11]); // get whole digit count
			d = 9 - decimalPlaces; // compute position of decimal 1s position

			if (windowLength == e) // if window length == whole digit count
			{

				strBuffer[(uint16_t)(++d)] = 0; // replace the decimal point with a null
				d -= windowLength; // set up for left adjust string copy

			}
			else if (windowLength < e) // if window length < whole digit count
			{

				f = 1; // signal that number overflowed
				d = 0; // signal 'do not perform string copy'

			}
			else // window length > whole digit count
			{

				d -= (e - 1); // point to largest whole digit stored

				// adjust window length if instructed to ignore the decimal point for length consideration
				f = windowLength;
				if ((decimalFlag & dfIgnoreDecimalPoint) && (decimalPlaces)) f++;

				// find the string end position minus adjusted window length
				e = ((decimalPlaces) ? 11 : 10) - f;

				// if string end position minus window length is less than the position for the largest whole digit, use it instead
				if (e < d) d = e;

				// catch the special case where the string has decimal point but no decimals
				if (strBuffer[(uint16_t)(d + f - 1)] == '.') d--;

				strBuffer[(uint16_t)(d + f)] = 0; // mark end of string

				f = 0;

			}

		}
		else // no windowLength specified, just eliminate leading spaces
		{

			d = 0;

			while (strBuffer[(uint16_t)(d)] == ' ' && (d < 11)) d++; // find position of first non-blank character

		}

		if (d) // move string buffer left to remove extra spaces, and to move number into window if required
		{

			e = 0;

			while (d < 12) strBuffer[(uint16_t)(e++)] = strBuffer[(uint16_t)(d++)];

		}

	}

	if (f) // if an overflow occurred
	{

		strcpy_P(strBuffer, ((decimalFlag & dfOverflow9s) ? overFlow9Str : overFlowStr)); // copy overflow string into buffer

		if (windowLength) strBuffer[(uint16_t)(windowLength)] = 0; // mark new end of string buffer

	}

	return strBuffer;

}

