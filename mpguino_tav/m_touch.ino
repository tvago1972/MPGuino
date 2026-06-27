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
#endif // defined(useTouchScreenInput)
