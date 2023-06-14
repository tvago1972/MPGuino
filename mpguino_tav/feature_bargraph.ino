#ifdef useBarGraph
/* Bar Graph support section */

static uint8_t barGraphSupport::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	const char * labelList;

	uint8_t graphCursorPos;
	uint8_t graphCalcIdx;
	uint8_t differentialFlag;

	uint8_t line0CalcIdx;
	uint8_t line1CalcIdx;
	uint8_t line0TripIdx;
	uint8_t line1TripIdx;

	uint8_t i;

	switch (menuLevel)
	{

#ifdef useBarFuelEconVsTime
		case barFEvTdisplayIdx:
			labelList = barFEvTfuncNames;

			graphCursorPos = bgDataSize - 1;
			graphCalcIdx = pgm_read_byte(&barFEvTgraphFuncs[(uint16_t)(cursorPos)]);
			line1CalcIdx = pgm_read_byte(&barFEvTdisplayFuncs[(uint16_t)(cursorPos)]);
			differentialFlag = pgm_read_byte(&barFEvTdiffFuncs[(uint16_t)(cursorPos)]);

			line0CalcIdx = line1CalcIdx;
			line0TripIdx = currentIdx;

			line1TripIdx = FEvTperiodIdx;
			break;

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
		case barFEvSdisplayIdx:
			labelList = barFEvSfuncNames;

			if (FEvSpdTripIdx < tripSlotCount) i = FEvSpdTripIdx - FEvsSpeedIdx + 1;
			else i = 0;

			graphCursorPos = i - 1;
			graphCalcIdx = pgm_read_byte(&barFEvSgraphFuncs[(uint16_t)(cursorPos)]);
			line0CalcIdx = pgm_read_byte(&barFEvSdisplayFuncs[(uint16_t)(cursorPos)]);
			differentialFlag = 0; // no differential graphs for fuel econ vs speed

			if (i)
			{

				line0TripIdx = FEvSpdTripIdx;

				if (mainLoopHeartBeat & 0b11110001)
				{

					line1CalcIdx = tSpeed;
					line1TripIdx = instantIdx;

				}
				else
				{

					line1TripIdx = tankIdx;
					line1CalcIdx = line0CalcIdx;

				}

			}
			else
			{

				line0TripIdx = tankIdx;
				line1CalcIdx = tSpeed;
				line1TripIdx = instantIdx;

			}

			break;

#endif // useBarFuelEconVsSpeed
	}

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			text::statusOut(devLCD, labelList, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			graphData(graphCursorPos, graphCalcIdx, differentialFlag);
			displayBarGraphLine(0, line0TripIdx, line0CalcIdx);
			displayBarGraphLine(1, line1TripIdx, line1CalcIdx);

			LCD::flushCGRAM();

			break;

		default:
			break;

	}

	return retVal;

}

static void barGraphSupport::displayBarGraphLine(uint8_t lineNumber, uint8_t tripIdx, uint8_t calcIdx)
{

	calcFuncObj thisCalcFuncObj;

	text::stringOut(devLCD, bgSpaces, lineNumber);

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, mBuff1, 6, 0);

	if (thisCalcFuncObj.isValid & isValidFlag)
	{

		text::charOut(devLCD, thisCalcFuncObj.tripChar);
		text::charOut(devLCD, thisCalcFuncObj.calcChar);

		text::stringOut(devLCD, thisCalcFuncObj.strBuffer);

	}
	else text::newLine(devLCD);

}

static void barGraphSupport::clearPlot(void)
{

	for (uint8_t x = 0; x < 64; x++) LCD::writeCGRAMbyte(x, 0); // clear the drawing area

}

static uint8_t barGraphSupport::rescaleData(uint8_t byt, uint8_t uBound)
{

	uint16_t worksp;
	uint8_t out;

	if (byt > uBound) out = 15; // this may occur with the cursor position data point
	else
	{

		worksp = 13 * byt / uBound + 1;
		out = worksp;

	}

	return out;

}

/*
static int16_t barGraphSupport::sqrrt(uint16_t num)
{

	int16_t x;
	int16_t c;
	int16_t d;

	x = num;
	c = 0;
	d = (1 << 14);

	while (d > num) d >>= 2;

	while (d)
	{

		if (x >= (c + d))
		{

			x -= (c + d);
			c = (c >> 1) + d;

		}
		else c >>= 1;

		d >>= 2;

	}

	return c;

}
*/

static void barGraphSupport::graphData(uint8_t cursorPos, uint8_t calcIdx, uint8_t diffFlag)
{

	int16_t rollSum;
	uint8_t byt;
	uint8_t mean;
	uint8_t uBound;
	uint8_t yStart;
	uint8_t zCount;
	uint8_t blinkFlag;
	uint8_t fl;

	SWEET64::runPrgm(prgmGenerateHistographData, calcIdx);

	rollSum = 0;
	yStart = 0;
	zCount = 0;
	fl = 1;

	for (uint8_t x = 0; x < bgDataSize; x++) // this is for calculating the mean of the dataset
	{

		byt = bargraphData[(uint16_t)(x)];

		switch (byt)
		{

			case 255: // ignore overflow values
				break;

			case 0: // treat zero values with special consideration
				if (fl) break; // if scan has not yet found a non-zero value, skip without updating data count
				else zCount++; // increment zero count
			default: // determine mean
				yStart++;
				fl = 0;
				zCount = 0; // reset zero count
				rollSum += byt;
				break;

		}

	}

	yStart -= zCount; // take off any trailing zeros from data count

	switch (yStart)
	{

		case 0: // no mean to calculate, use default lBound and uBound
			uBound = 2;
			break;

		default:
			rollSum /= yStart; // calculate mean
			mean = rollSum;
			if (mean) uBound = 2 * mean; // if there is a non-zero mean, use that for uBound calculation
			else uBound = 2; // otherwise, assume an arbitrary uBound
			break;

	}

	clearPlot();

	drawLine(1, 0, 0, 0, 15); // draw y-axis on left of graph

	if (diffFlag)
	{

		yStart = 7; // start in the middle of the graph
		draw(1, 1, yStart);

	}
	else // absolute graph selected
	{

		yStart = 0; // start at bottom of the graph
		draw(1, 1, 14);
		draw(1, 1, 1);

	}

	for (uint8_t x = 0; x < bgDataSize; x++)
	{

		byt = bargraphData[(uint16_t)(x)];
		blinkFlag = ((x == cursorPos) && (mainLoopHeartBeat & 136));

		switch (byt)
		{

			case 0: // handle zero values
				byt = 0;
				break;

			case 255: // handle overflow values
				byt = 15;
				break;

			default: // handle values between lBound and uBound
				byt = rescaleData(byt, uBound);
				break;

		}

		if (byt == yStart)
		{

			if (blinkFlag == 0) draw(1, x + 3, byt);

		}
		else
		{

			if (blinkFlag) draw(1, x + 3, byt);
			else drawLine(1, x + 3, yStart, x + 3, byt);

		}

	}

	byt = rescaleData(mean, uBound);
	draw(1, 1, byt);

}

static void barGraphSupport::drawLine(uint8_t color, uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY)
{

	uint8_t f;
	int dx;
	int dy;
	int sx;
	int sy;
	int err;
	int e2;

	dx = abs(endX - startX); // delta X between start and end
	dy = -abs(endY - startY); // delta Y between start and end
	sx = (endX > startX) - (endX < startX);	// sign(dx)
	sy = (endY > startY) - (endY < startY); // sign(dy)
	err = dx + dy; // figure out initial error

	f = 1;

	while (f)
	{

		draw(color, startX, startY); // draw a point of the line

		if ((startX == endX) && (startY == endY)) f = 0; // if start and endpoints are the same, then the line is finished
		else
		{

			e2 = 2 * err;
			if (e2 >= dy) // if error >= delta Y
			{

				if (startX == endX) f = 0; // if x is at endpoint, then line is finished
				else
				{

					err += dy; // update error
					startX += sx; // move x toward endpoint

				}

			}

			if (e2 <= dx) // if error <= delta X
			{

				if (startY == endY) f = 0; // if y is at endpoint, then line is finished
				else
				{

					err += dx; // update error
					startY += sy; // move y toward endpoint

				}

			}

		}

	}

}

static void barGraphSupport::draw(uint8_t color, uint8_t xPos, uint8_t yPos)
{

	uint8_t cgrAddress;
	uint8_t x;
	uint8_t y;
	uint8_t i;
	uint8_t msk;
	uint8_t c;
	uint8_t byt;

	x = xPos / 5;
	y = 15 - yPos;

	cgrAddress = y / 8; // results in 0 (upper) or 1 (lower)
	cgrAddress += 2 * x; // 0, 2, 4, 6 - horizontal component
	cgrAddress <<= 3; // multiply result by 8
	cgrAddress += (y & 0x07); // get byte offset into destination character
	i = xPos % 5; // figure out bitmask
	msk = (16 >> i);

	c = ((color) ? 31 : 0); // translate color to either on or off

	byt = LCD::peekCGRAMbyte(cgrAddress); // get the target byte from CGRAM storage

	c ^= byt; // xor color with target byte
	c &= msk; // strip out all but relevant bit
	c ^= byt; // xor relevant bit with target byte

	LCD::writeCGRAMbyte(cgrAddress, c); // go save target byte with changed bit

}

#endif // useBarGraph
