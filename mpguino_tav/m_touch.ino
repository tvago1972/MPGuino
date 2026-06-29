#if defined(useTouchScreenInput)
/* XPT2046 resistive touch screen support section (bit-banged software SPI) */

// send one command byte to the XPT2046, MSB first; the chip samples DIN on the
// rising edge of CLK (SPI mode 0). matches the URTouch reference timing.
static void touchWriteByte(uint8_t data)
{

	uint8_t i;

	TOUCH_CLK_LO();

	for (i = 0; i < 8; i++)
	{

		if (data & 0x80) TOUCH_DIN_HI();
		else TOUCH_DIN_LO();

		data <<= 1;
		TOUCH_CLK_LO();
		TOUCH_CLK_HI();		// chip latches DIN on the rising edge

	}

}

// read a 12-bit conversion result, MSB first; DOUT is valid after the CLK falling edge
static uint16_t touchReadBits(void)
{

	uint8_t i;
	uint16_t data = 0;

	for (i = 0; i < 12; i++)
	{

		data <<= 1;
		TOUCH_CLK_HI();
		TOUCH_CLK_LO();
		if (TOUCH_DOUT_SET) data++;

	}

	return data;

}

static void touch::init(void)
{

#if defined(useMPGuinoColourTouch)
	DDRB |= ((1 << DDB5) | (1 << DDB4));		// CLK, CS as outputs
	DDRH |= (1 << DDH6);						// DIN as output
	DDRH &= ~((1 << DDH5) | (1 << DDH4));		// DOUT, IRQ as inputs

	PORTH |= (1 << PORTH4);						// enable pull-up on PENIRQ

	TOUCH_CS_HI();								// deselect
	TOUCH_CLK_HI();
	TOUCH_DIN_HI();
#endif // defined(useMPGuinoColourTouch)

	touchRawX = 0;
	touchRawY = 0;

	touch::loadCalibration();

}

// read one 16-bit EEPROM parameter as a word
static uint16_t touchReadWordParam(uint8_t parameterIdx)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *)(&val);

	EEPROM::read64(vee, parameterIdx);

	return vee->u16[0];

}

// store one 16-bit value into an EEPROM parameter
static void touchWriteWordParam(uint8_t parameterIdx, uint16_t value)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *)(&val);

	SWEET64::init64(vee, (uint32_t)(value));

	EEPROM::write64(vee, parameterIdx);

}

// refresh the RAM calibration cache from the pTouchRaw* EEPROM parameters
static void touch::loadCalibration(void)
{

	touchRawXlo = touchReadWordParam(pTouchRawXloIdx);
	touchRawXhi = touchReadWordParam(pTouchRawXhiIdx);
	touchRawYlo = touchReadWordParam(pTouchRawYloIdx);
	touchRawYhi = touchReadWordParam(pTouchRawYhiIdx);

}

// returns non-zero while the screen is being touched (PENIRQ low)
static uint8_t touch::pressed(void)
{

	return (TOUCH_IRQ_DOWN);

}

// take an averaged raw reading. returns 1 and fills rawX/rawY (0..4095) if a
// stable touch was sampled, 0 otherwise. outlier rejection is left for the
// caller's debounce; here we just average the valid in-range samples.
static uint8_t touch::sample(uint16_t * rawX, uint16_t * rawY)
{

	uint8_t i;
	uint8_t count = 0;
	uint32_t sumX = 0;
	uint32_t sumY = 0;
	uint16_t tx, ty;

	if (!(TOUCH_IRQ_DOWN)) return 0;			// not touched

	TOUCH_CS_LO();								// select the XPT2046

	for (i = 0; i < touchSamples; i++)
	{

		if (!(TOUCH_IRQ_DOWN)) break;			// released mid-read

		touchWriteByte(touchCmdX);
		TOUCH_CLK_HI();							// extra clock past the busy bit
		TOUCH_CLK_LO();
		tx = touchReadBits();

		touchWriteByte(touchCmdY);
		TOUCH_CLK_HI();
		TOUCH_CLK_LO();
		ty = touchReadBits();

		if ((tx > 0) && (tx < 4096) && (ty > 0) && (ty < 4096))
		{

			sumX += tx;
			sumY += ty;
			count++;

		}

	}

	TOUCH_CS_HI();								// deselect

	if (count < (touchSamples / 2)) return 0;	// too few good samples to trust

	touchRawX = (uint16_t)(sumX / count);
	touchRawY = (uint16_t)(sumY / count);
	*rawX = touchRawX;
	*rawY = touchRawY;

	return 1;

}

// map one raw ADC axis (lo..hi) onto 0..dim-1, clamped
static uint16_t touchMap(uint16_t raw, uint16_t lo, uint16_t hi, uint16_t dim)
{

	if (raw <= lo) return 0;
	if (raw >= hi) return (dim ? (dim - 1) : 0);
	return (uint16_t)((uint32_t)(raw - lo) * dim / (hi - lo));

}

// read a touch and map it to screen pixels for the current rotation. returns 1 and
// fills x/y on a valid touch. orientation 3 is the measured/exact case; the other
// three are derived from it (landscape uses rawX->X / rawY->Y, portrait swaps the
// axes) and should be spot-checked with 'G' + 'H'. touchRawX/Y hold the raw values.
static uint8_t touch::read(uint16_t * x, uint16_t * y)
{

	uint16_t rx, ry;

	if (!touch::sample(&rx, &ry)) return 0;

	switch (tftRotation)
	{

		case 1: // landscape, 180 of orientation 3
			*x = tftWidth - 1 - touchMap(rx, touchRawXlo, touchRawXhi, tftWidth);
			*y = tftHeight - 1 - touchMap(ry, touchRawYlo, touchRawYhi, tftHeight);
			break;

		case 0: // portrait (raw axes swap relative to landscape; Y inverted)
			*x = touchMap(ry, touchRawYlo, touchRawYhi, tftWidth);
			*y = tftHeight - 1 - touchMap(rx, touchRawXlo, touchRawXhi, tftHeight);
			break;

		case 2: // portrait, 180
			*x = tftWidth - 1 - touchMap(ry, touchRawYlo, touchRawYhi, tftWidth);
			*y = touchMap(rx, touchRawXlo, touchRawXhi, tftHeight);
			break;

		default: // case 3: landscape (calibration reference, exact)
			*x = touchMap(rx, touchRawXlo, touchRawXhi, tftWidth);
			*y = touchMap(ry, touchRawYlo, touchRawYhi, tftHeight);
			break;

	}

	return 1;

}

#if defined(useDebugTerminal) && defined(useTFToutput)
// dev aid: poll the touch panel for a few seconds, drawing a crosshair where the
// finger lands (crude linear map, pre-calibration) and printing the raw ADC
// coordinates to the terminal on each new press so the panel can be calibrated.
static void touch::testLoop(void)
{

	uint16_t sx, sy, i;
	uint8_t now, was = 0;

	TFT::clearScreen();
	text::stringOut(m8DevTFTidx, PSTR("touch test" tcCR "tap the screen" tcCR));
	text::stringOut(m8DevDebugTerminalIdx, PSTR("touch test - tap corners; raw ADC coords follow" tcCR));

	for (i = 0; i < 1500; i++)
	{

		now = touch::read(&sx, &sy); // calibrated screen coords; touchRawX/Y hold the raw

		if (now)
		{

			ILI9341::drawLine((sx > 6) ? (sx - 6) : 0, sy, (sx + 6 < tftWidth) ? (sx + 6) : (tftWidth - 1), sy, ILI9341_GREEN);
			ILI9341::drawLine(sx, (sy > 6) ? (sy - 6) : 0, sx, (sy + 6 < tftHeight) ? (sy + 6) : (tftHeight - 1), ILI9341_GREEN);

			if (!was) // print once per new press
			{

				text::stringOut(m8DevDebugTerminalIdx, PSTR("raw x=0x"));
				text::hexWordOut(m8DevDebugTerminalIdx, touchRawX);
				text::stringOut(m8DevDebugTerminalIdx, PSTR(" y=0x"));
				text::hexWordOut(m8DevDebugTerminalIdx, touchRawY);
				text::stringOut(m8DevDebugTerminalIdx, PSTR("  ->  px="));
				text::hexWordOut(m8DevDebugTerminalIdx, sx);
				text::stringOut(m8DevDebugTerminalIdx, PSTR(" py="));
				text::hexWordOut(m8DevDebugTerminalIdx, sy);
				text::newLine(m8DevDebugTerminalIdx);

			}

		}

		was = now;
		heart::wait0(8);

	}

	text::stringOut(m8DevDebugTerminalIdx, PSTR("touch test done" tcCR));

}

#endif // defined(useDebugTerminal) && defined(useTFToutput)

#if defined(useTFToutput)
// draw a crosshair (two arms) centred at (x,y) in the given colour
static void touchDrawCrosshair(uint16_t x, uint16_t y, uint16_t colour)
{

	ILI9341::drawLine((x > 10) ? (x - 10) : 0, y, (x + 10 < tftWidth) ? (x + 10) : (tftWidth - 1), y, colour);
	ILI9341::drawLine(x, (y > 10) ? (y - 10) : 0, x, (y + 10 < tftHeight) ? (y + 10) : (tftHeight - 1), colour);

}

// interactive 4-corner touch calibration. forces landscape orientation 3 (the
// raw->pixel maps for every rotation derive from it), draws a crosshair at each
// corner, samples the raw ADC where the user taps, then derives and stores the
// pTouchRaw* edge parameters. the panel is sampled raw here, so the existing
// (possibly bad) calibration does not affect the result.
static void touch::calibrate(void)
{

	static const uint8_t inset = 8;					// corner target offset from the edge (px)
	uint16_t rawXatCorner[4];						// 0=TL, 1=TR, 2=BL, 3=BR
	uint16_t rawYatCorner[4];
	uint16_t cornerX, cornerY, rx, ry;
	uint8_t c, prevRotation, got;

	prevRotation = tftRotation;
	TFT::setRotation(3);							// calibration reference orientation

	text::stringOut(m8DevDebugTerminalIdx, PSTR("touch calibration: tap each crosshair (orientation 3)" tcCR));

	for (c = 0; c < 4; c++)
	{

		cornerX = (c & 1) ? (tftWidth - 1 - inset) : inset;		// TR/BR on the right
		cornerY = (c & 2) ? (tftHeight - 1 - inset) : inset;	// BL/BR on the bottom

		TFT::clearScreen();
		text::stringOut(m8DevTFTidx, PSTR("Touch calibration" tcCR "tap the crosshair"));
		touchDrawCrosshair(cornerX, cornerY, ILI9341_GREEN);

		while (touch::pressed()) heart::wait0(8);	// wait for any prior touch to release

		got = 0;
		while (!got) { if (touch::sample(&rx, &ry)) got = 1; else heart::wait0(8); }

		rawXatCorner[c] = rx;
		rawYatCorner[c] = ry;

		touchDrawCrosshair(cornerX, cornerY, ILI9341_RED);	// acknowledge the capture

		text::stringOut(m8DevDebugTerminalIdx, PSTR("corner "));
		text::charOut(m8DevDebugTerminalIdx, (uint8_t)('1' + c));
		text::stringOut(m8DevDebugTerminalIdx, PSTR(": raw x=0x"));
		text::hexWordOut(m8DevDebugTerminalIdx, rx);
		text::stringOut(m8DevDebugTerminalIdx, PSTR(" y=0x"));
		text::hexWordOut(m8DevDebugTerminalIdx, ry);
		text::newLine(m8DevDebugTerminalIdx);

		while (touch::pressed()) heart::wait0(8);	// wait for release before the next corner

	}

	// left edge = TL & BL, right edge = TR & BR; top = TL & TR, bottom = BL & BR
	touchRawXlo = (uint16_t)(((uint32_t)(rawXatCorner[0]) + rawXatCorner[2]) / 2);
	touchRawXhi = (uint16_t)(((uint32_t)(rawXatCorner[1]) + rawXatCorner[3]) / 2);
	touchRawYlo = (uint16_t)(((uint32_t)(rawYatCorner[0]) + rawYatCorner[1]) / 2);
	touchRawYhi = (uint16_t)(((uint32_t)(rawYatCorner[2]) + rawYatCorner[3]) / 2);

	touchWriteWordParam(pTouchRawXloIdx, touchRawXlo);
	touchWriteWordParam(pTouchRawXhiIdx, touchRawXhi);
	touchWriteWordParam(pTouchRawYloIdx, touchRawYlo);
	touchWriteWordParam(pTouchRawYhiIdx, touchRawYhi);

	text::stringOut(m8DevDebugTerminalIdx, PSTR("stored Xlo=0x"));
	text::hexWordOut(m8DevDebugTerminalIdx, touchRawXlo);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" Xhi=0x"));
	text::hexWordOut(m8DevDebugTerminalIdx, touchRawXhi);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" Ylo=0x"));
	text::hexWordOut(m8DevDebugTerminalIdx, touchRawYlo);
	text::stringOut(m8DevDebugTerminalIdx, PSTR(" Yhi=0x"));
	text::hexWordOut(m8DevDebugTerminalIdx, touchRawYhi);
	text::newLine(m8DevDebugTerminalIdx);

	TFT::setRotation(prevRotation);

}

/* on-screen numeric keypad (TFT draw + XPT2046 touch input) */

// pixel origin of key `index` (0..keypadKeys-1) for the current geometry
static uint16_t keypadKeyX(uint8_t index)
{

	return (uint16_t)(keypadGap) + (uint16_t)(index % keypadCols) * (keypadKeyW + keypadGap);

}

static uint16_t keypadKeyY(uint8_t index)
{

	uint8_t row = index / keypadCols;
	uint16_t y = keypadGridTop + (uint16_t)(row) * (keypadKeyH + keypadGap);

	if (row == (keypadRows - 1)) y += keypadActionGap;	// push the C/0/E row down off the digits

	return y;

}

// draw one key: filled face (highlighted when pressed), white border, centred label
static void keypad::drawKey(uint8_t index, uint8_t highlight)
{

	uint16_t x = keypadKeyX(index);
	uint16_t y = keypadKeyY(index);
	uint16_t face = (highlight ? keypadKeyDownColour : keypadKeyColour);
	uint16_t cellW = (uint16_t)(TFT_CELL_W) * keypadLabelScale;
	uint16_t cellH = (uint16_t)(TFT_CELL_H) * keypadLabelScale;
	uint8_t label = pgm_read_byte(&keypadLabels[index]);

	ILI9341::fillRect(x, y, keypadKeyW, keypadKeyH, face);

	// border (four sides)
	ILI9341::drawLine(x, y, x + keypadKeyW - 1, y, keypadBorderColour);
	ILI9341::drawLine(x, y + keypadKeyH - 1, x + keypadKeyW - 1, y + keypadKeyH - 1, keypadBorderColour);
	ILI9341::drawLine(x, y, x, y + keypadKeyH - 1, keypadBorderColour);
	ILI9341::drawLine(x + keypadKeyW - 1, y, x + keypadKeyW - 1, y + keypadKeyH - 1, keypadBorderColour);

	// label drawn with bg = face so the glyph cell blends into the key
	ILI9341::drawChar(x + (keypadKeyW - cellW) / 2, y + (keypadKeyH - cellH) / 2, label, keypadBorderColour, face, keypadLabelScale);

}

// repaint the entry box and the current digit string (right-aligned, yellow on black)
static void keypad::drawEntry(void)
{

	uint16_t cellW = (uint16_t)(TFT_CELL_W) * keypadLabelScale;
	uint16_t cellH = (uint16_t)(TFT_CELL_H) * keypadLabelScale;
	uint16_t innerW = tftWidth - 2 * keypadGap;
	uint16_t x, y;
	uint8_t i;

	// box background + border
	ILI9341::fillRect(keypadGap, keypadGap, innerW, keypadEntryH, ILI9341_BLACK);
	ILI9341::drawLine(keypadGap, keypadGap, keypadGap + innerW - 1, keypadGap, keypadBorderColour);
	ILI9341::drawLine(keypadGap, keypadGap + keypadEntryH - 1, keypadGap + innerW - 1, keypadGap + keypadEntryH - 1, keypadBorderColour);
	ILI9341::drawLine(keypadGap, keypadGap, keypadGap, keypadGap + keypadEntryH - 1, keypadBorderColour);
	ILI9341::drawLine(keypadGap + innerW - 1, keypadGap, keypadGap + innerW - 1, keypadGap + keypadEntryH - 1, keypadBorderColour);

	// right-align the digits inside the box
	y = keypadGap + (keypadEntryH - cellH) / 2;
	x = keypadGap + innerW - 4 - (uint16_t)(keypadEntryLen ? keypadEntryLen : 1) * cellW;

	if (keypadEntryLen == 0) ILI9341::drawChar(x, y, '0', keypadEntryColour, ILI9341_BLACK, keypadLabelScale);
	else for (i = 0; i < keypadEntryLen; i++) ILI9341::drawChar(x + (uint16_t)(i) * cellW, y, keypadEntry[i], keypadEntryColour, ILI9341_BLACK, keypadLabelScale);

}

// compute geometry for the current rotation and paint the whole keypad
static void keypad::draw(void)
{

	uint8_t i;

	TFT::clearScreen();

	keypadEntryH = (uint16_t)(TFT_CELL_H) * keypadLabelScale + 8;
	keypadGridTop = keypadGap + keypadEntryH + keypadGap;
	keypadKeyW = (tftWidth - (uint16_t)(keypadCols + 1) * keypadGap) / keypadCols;
	keypadKeyH = (tftHeight - keypadGridTop - (uint16_t)(keypadRows) * keypadGap - keypadActionGap) / keypadRows;

	keypad::drawEntry();
	for (i = 0; i < keypadKeys; i++) keypad::drawKey(i, 0);

}

// modal numeric entry: draw the keypad, then collect digits until Enter is
// pressed. returns 1 and stores the entered value (Enter), or 0 on idle timeout
// (or Enter with no digits). digits are accumulated as a string and parsed once.
static uint8_t keypad::getNumber(uint32_t * value)
{

	uint16_t px, py, idle;
	uint8_t i, hitRow, hitCol, hit, label, result = 0, done = 0;

	keypadEntryLen = 0;
	keypadEntry[0] = 0;
	keypad::draw();

	idle = 0;

	while ((!done) && (idle < keypadIdleTimeout))
	{

		if (!touch::read(&px, &py)) { idle++; heart::wait0(8); continue; }

		idle = 0;

		// hit-test the touch against the key grid by explicit per-key bounds, so
		// the non-uniform action-row gap (and inter-key gaps) are rejected cleanly
		hit = keypadKeys; // sentinel: no key
		for (hitRow = 0; (hit == keypadKeys) && (hitRow < keypadRows); hitRow++)
			for (hitCol = 0; hitCol < keypadCols; hitCol++)
			{

				uint8_t idx = (uint8_t)(hitRow * keypadCols + hitCol);
				uint16_t kx = keypadKeyX(idx);
				uint16_t ky = keypadKeyY(idx);

				if ((px >= kx) && (px < kx + keypadKeyW) && (py >= ky) && (py < ky + keypadKeyH))
				{

					hit = idx;
					break;

				}

			}

		if (hit < keypadKeys)
		{

			keypad::drawKey(hit, 1); // visual feedback while held

			// wait for release so each press registers once (debounce)
			while (touch::pressed()) heart::wait0(8);

			keypad::drawKey(hit, 0);

			label = pgm_read_byte(&keypadLabels[hit]);

			switch (label)
			{

				case 'C': // clear the entry
					keypadEntryLen = 0;
					keypadEntry[0] = 0;
					keypad::drawEntry();
					break;

				case 'E': // accept (only if at least one digit was entered)
					if (keypadEntryLen) done = result = 1;
					break;

				default: // a digit 0..9
					if (keypadEntryLen < keypadMaxDigits)
					{

						keypadEntry[keypadEntryLen++] = label;
						keypadEntry[keypadEntryLen] = 0;
						keypad::drawEntry();

					}
					break;

			}

		}
		else heart::wait0(8); // touched between keys; ignore

	}

	if (result)
	{

		uint32_t v = 0;

		for (i = 0; i < keypadEntryLen; i++) v = v * 10 + (uint32_t)(keypadEntry[i] - '0');
		*value = v;

	}

	return result;

}

#endif // defined(useTFToutput)
#endif // defined(useTouchScreenInput)
