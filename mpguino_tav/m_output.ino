static const uint8_t prgmMultiplyBy100[] PROGMEM = {
	instrMul2byByte, 100,								// multiply result by 100
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static const uint8_t prgmMultiplyBy10[] PROGMEM = {
	instrMul2byByte, 10,								// multiply result by 10
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static void printStatusMessage(const char * s)
{

	initStatusLine();
	text::stringOut(devLCD, s);
	execStatusLine();

}

static void printStatusMessage(const char * s, uint8_t strIdx)
{

	printStatusMessage(findStr(s, strIdx));

}

void initStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = 0; // end hold delay countdown
	timer0Command &= ~(t0cDisplayDelay); // cancel display delay

	SREG = oldSREG; // restore interrupt flag status

#ifdef blankScreenOnMessage
	text::charOut(devLCD, 0x0C); // clear the entire screen
#else // blankScreenOnMessage
	text::gotoXY(devLCD, 0, 0); // go to the first line
#endif // blankScreenOnMessage

}

void execStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = holdDelay; // start hold delay countdown
	timer0Command |= (t0cDisplayDelay); // make display delay active

	SREG = oldSREG; // restore interrupt flag status

}

const char * findStr(const char * str, uint8_t strIdx)
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

unsigned long str2ull(char * strBuffer)
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

	SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0); // initialize 64-bit number to zero

	while ((loopFlag) && (x < 17))
	{

		if ((c = strBuffer[(unsigned int)(x++)])) // if a non-NULL character is read in
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

	return ((union union_64 *)(&s64reg[s64reg2]))->ul[0];

}

char findDigit(uint8_t value, char &zeroChar)
{

	if (value)
	{
		value += '0';
		zeroChar = '0';
	}
	else value = zeroChar;

	return value;

}

char * ull2str(uint8_t prgmIdx, char * strBuffer, uint8_t decimalPlaces)
{

	union union_64 * tmpPtr2 = (union union_64 *)(&s64reg[s64reg2]);

	uint8_t b;
	char c;
	uint8_t l;
	uint8_t y;
	char z;

	SWEET64::doCalculate(decimalPlaces, prgmIdx); // call SWEET64 routine to perform decimal point rounding to next nearest decimal place

	l = tmpPtr2->u8[6];	// load total length of binary-coded decimal bytes of converted number

	if (l == 255) strcpy_P(strBuffer, overFlowStr); // if length is 255, this number overflowed
	else
	{

		z = (char)(tmpPtr2->u8[7]);	// load leading zero character
		y = 0;

		for (uint8_t x = 0; x < l; x++) // go through all of the binary-coded decimal bytes of converted number
		{

			b = tmpPtr2->u8[(unsigned int)(x)];	// load a binary-coded decimal byte of number

			c = findDigit(b / 10, z);		// perform leading zero conversion on 10's place digit
			strBuffer[(unsigned int)(y++)] = c;	// store 10's place digit in string buffer

			c = findDigit(b % 10, z);		// perform leading zero conversion on 1's place digit
			strBuffer[(unsigned int)(y++)] = c;	// store 1's place digit in string buffer

		}

		if (c == ' ') strBuffer[(unsigned int)(y - 1)] = '0'; // ensure that at least one numeric digit exists
		strBuffer[(unsigned int)(y)] = 0; // mark end of string buffer with a NULL character

	}

	return strBuffer; // return pointer to string buffer containing the number

}

char * formatDecimal(char * strBuffer, uint8_t windowLength, uint8_t decimalPlaces, uint8_t decimalFlag) // format number for output
{

	uint8_t x;
	uint8_t y;
	uint8_t z; // pointer to end of integer digits in string
	uint8_t c; // temporary character storage
	uint8_t i; // integer digit count

	ull2str(tRoundOffNumber, strBuffer, decimalPlaces); // perform rounding of number to nearest decimal place, then format for ASCII output

	if (windowLength > 11) windowLength = 0; // if window length is larger than the contents of the output string buffer, zero out window length

	if (strBuffer[2] != '-') // if number did not overflow
	{

		x = 6 + decimalPlaces; // source string index
		y = 10; // destination string index
		i = 0;

		while (y < 12) // loop through entire string buffer, insert padding and a decimal point as required, and preserve ending NULL
		{

			if (x < 10)
			{

				c = strBuffer[(unsigned int)(x)]; // read in a digit of formatted number from string buffer, bump down source string index by one

				if ((x > 5) && (c == ' ')) c = '0'; // change 1's place and lower places leading spaces into leading zeros
				if ((x == 6) && (decimalPlaces)) strBuffer[(unsigned int)(y--)] = '.'; // if a decimal position was specified, and at decimal point, insert it
				if ((x < 7) && (c != ' ')) i++; // count the number of non-space integer characters

				x--;

			}
			else c = ' '; // otherwise, pad front of number with extra spaces

			strBuffer[(unsigned int)(y--)] = c; // store processed character back into string buffer, bump down destination string index by one

		}

		if (decimalPlaces) z = 10 - decimalPlaces; // point to either decimal point or to end of string
		else z = 11; // results in pointing to first character past the 1's digit in both cases

		if (windowLength)
		{

			if (i > windowLength) // if window length is smaller than the number of integer digits, overflow
			{

				x = 0; // signal 'do not perform string buffer move'
				strcpy_P(strBuffer, overFlowStr); // copy overflow string to string buffer

			}
			else
			{

				if ((decimalFlag & dfAdjustWindow) && (i < windowLength)) windowLength++; // if variable window length, and window length > the # of integer digits, expand window to include decimal point

				if ((i == windowLength) || ((i + 1) == windowLength)) x = z - windowLength;
				else
				{

					x = z - i;
					if ((x + windowLength) > 10) x = 11 - windowLength;

				}

			}

		}
		else // no window length specified, just eliminate leading spaces
		{

			x = z - i; // point to first integer character in string buffer
			strBuffer[11] = 0; // mark new end of string buffer

		}

		if (x) for (y = 0; y < (12 - x); y++) strBuffer[(unsigned int)(y)] = strBuffer[(unsigned int)(y + x)]; // shift number left to remove unneeded leading spaces

	}
	else
	{

		if (decimalFlag & dfOverflow9s) strcpy_P(strBuffer, overFlow9Str);

	}

	if (windowLength) strBuffer[(unsigned int)(windowLength)] = 0; // mark new end of string buffer

	return strBuffer;

}

/* text string output section */

static void text::newLine(interfaceDevice &dev)
{

	charOut(dev, 0x0D);

}

static void text::gotoXY(interfaceDevice &dev, uint8_t xPos, uint8_t yPos)
{

	charOut(dev, 0x80 + yPos * 20 + xPos);

}

static uint8_t text::charOut(interfaceDevice &dev, uint8_t chr)
{

	uint8_t retVal;

	retVal = 1;

	if (dev.chrOut)
	{

		switch (chr)
		{

			case 0x00:	// defined as end-of-string
				dev.controlFlags |= (odvFlagEnableOutput);
				retVal = 0;
				break;

			case 0xEB:	// disable device output for metric mode
				if (metricFlag & metricMode) dev.controlFlags &= ~(odvFlagEnableOutput);
				else dev.controlFlags |= (odvFlagEnableOutput);
				break;

			case 0xEC: // toggle device output enable
				dev.controlFlags ^= (odvFlagEnableOutput);
				break;

			case 0xED: // enable device output
				dev.controlFlags |= (odvFlagEnableOutput);
				break;

			case 0xEE:	// disable device output
				dev.controlFlags &= ~(odvFlagEnableOutput);
				break;

			case 0x0D:	// also defined as end of string
				retVal = 0;
				dev.controlFlags |= (odvFlagEnableOutput);
				dev.chrOut(0x0D);
				if (dev.controlFlags & odvFlagCRLF) dev.chrOut(0x0A);
				break;

			case 0xF0 ... 0xF7: // print CGRAM character
				chr &= 0x07;
			case 0x20 ... 0x7F: // print normal character
				if (dev.controlFlags & odvFlagEnableOutput)
				{

					if (dev.controlFlags & odvFlagShootBlanks) chr = ' ';
					dev.chrOut(chr);

				}
				break;

			default:
				dev.chrOut(chr);
				break;

		}
	}
	else retVal = 0;

	return retVal;

}

static void text::setModeOnCondition(interfaceDevice &dev, uint8_t condition, uint8_t odvFlag)
{

	if (condition) dev.controlFlags |= (odvFlag);
	else dev.controlFlags &= ~(odvFlag);

}

static void text::stringOut(interfaceDevice &dev, char * str)
{

	while (charOut(dev, * str++));

}

static void text::stringOut(interfaceDevice &dev, const char * str)
{

	while (charOut(dev, pgm_read_byte(str++))) ;

}

static void text::stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx)
{

	stringOut(dev, findStr(str, strIdx));

}

static void text::hexNybbleOut(interfaceDevice &dev, uint8_t val)
{

	val &= 0x0F;
	val |= 0x30;
	if (val > 0x39) val += 0x07;
	charOut(dev, val);

}

static void text::hexByteOut(interfaceDevice &dev, uint8_t val)
{

	hexNybbleOut(dev, val >> 4);
	hexNybbleOut(dev, val);

}

static void text::hexWordOut(interfaceDevice &dev, uint16_t val)
{

	union union_16 * vee = (union union_16 *) &val;

	for (uint8_t i = 1; i < 2; i--) hexByteOut(dev, vee->u8[i]);

}

static void text::hexDWordOut(interfaceDevice &dev, uint32_t val)
{

	union union_32 * vee = (union union_32 *) &val;

	for (uint8_t i = 3; i < 4; i--) hexByteOut(dev, vee->u8[i]);

}

static void text::hexLWordOut(interfaceDevice &dev, uint64_t * val)
{

	union union_64 * vee = (union union_64 *) val;

	for (uint8_t i = 7; i < 8; i--) hexByteOut(dev, vee->u8[i]);

}

static uint8_t text::numberOut(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	calcFuncObj thisCalcFuncObj;

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, strBuff, windowLength, decimalFlag); // perform the required decimal formatting
	stringOut(dev, thisCalcFuncObj.strBuffer); // output the number

	return thisCalcFuncObj.calcFmtIdx;

}
