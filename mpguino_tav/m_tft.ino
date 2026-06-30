#if defined(useTFToutput)

static void TFT::init(void)
{

	ILI9341::init();

	// text-device defaults: 2x scale, white on black. setRotation establishes the
	// MADCTL + active width/height and clears the screen (default portrait).
	tftScale = 2;
	tftFGcolour = ILI9341_WHITE;
	tftBGcolour = ILI9341_BLACK;
	tftTextPadding = 0;
	tftFieldStartX = 0;
	TFT::setRotation(0);

	// register the TFT as a text output device so text::stringOut/charOut/numberOut
	// (and the rest of the text layer) can render to the screen. the startup splash
	// (title + date) is drawn by main() so it can be held for the normal display delay.
	text::initDev(m8DevTFTidx, (odvFlagCRLF | odvFlagEnableOutput), TFT::chrOut);

}

static void TFT::shutdown(void)
{

	ILI9341::shutdown();

}

// light wake from TFT::shutdown(): the controller kept VCC, so no reset/re-init is
// needed - just undo shutdown(). (the boot and deep-sleep paths still use init().)
static void TFT::resume(void)
{

	ILI9341::resume();

}

// text output device callback: advance a pixel cursor, handle CR/LF and wrap.
static void TFT::chrOut(uint8_t chr)
{

	uint16_t cellW = (uint16_t)(TFT_CELL_W) * tftScale;
	uint16_t cellH = (uint16_t)(TFT_CELL_H) * tftScale;

	switch (chr)
	{

		case 0x0D: // carriage return - back to left margin (pad the field being left)
			TFT::applyPadding();
			tftCursorX = 0;
			tftFieldStartX = 0;
			break;

		case 0x0A: // line feed - down one row, wrap to top at the bottom
			tftCursorY += cellH;
			if (tftCursorY + cellH > tftHeight) tftCursorY = 0;
			break;

		case 0x80 ... 0xCF: // text::gotoXY position char: 0x80 + row*20 + col (LCD-shaped: <=4 rows x 20 cols)
			TFT::applyPadding();
			tftCursorX = (uint16_t)((chr - 0x80) % 20) * cellW;
			tftCursorY = (uint16_t)((chr - 0x80) / 20) * cellH;
			tftFieldStartX = tftCursorX;
			break;

		default:
			if ((chr >= 0x20) && (chr <= 0x7F))
			{

				if (tftCursorX + cellW > tftWidth) // wrap at the right edge
				{

					tftCursorX = 0;
					tftCursorY += cellH;
					if (tftCursorY + cellH > tftHeight) tftCursorY = 0;

				}

				ILI9341::drawChar(tftCursorX, tftCursorY, chr, tftFGcolour, tftBGcolour, tftScale);
				tftCursorX += cellW;

			}
			break;

	}

}

static void TFT::clearScreen(void)
{

	ILI9341::fillScreen(tftBGcolour);
	tftCursorX = 0;
	tftCursorY = 0;

}

static void TFT::setTextColour(uint16_t fg, uint16_t bg)
{

	tftFGcolour = fg;
	tftBGcolour = bg;

}

static void TFT::gotoXY(uint8_t col, uint8_t row)
{

	TFT::applyPadding();
	tftCursorX = (uint16_t)(col) * (uint16_t)(TFT_CELL_W) * tftScale;
	tftCursorY = (uint16_t)(row) * (uint16_t)(TFT_CELL_H) * tftScale;
	tftFieldStartX = tftCursorX;

}

// pixel-precise cursor for absolute (abbalooga-style) layout
static void TFT::setCursorPixel(uint16_t x, uint16_t y)
{

	TFT::applyPadding();
	tftCursorX = x;
	tftCursorY = y;
	tftFieldStartX = x;

}

// set a fixed field width (pixels) starting at the current cursor. when the
// cursor next leaves the field (reposition or CR), any gap from the end of the
// drawn text out to the field width is cleared to the background, so a shorter
// new value cleanly overwrites a longer old one. 0 disables padding.
static void TFT::setTextPadding(uint16_t pixels)
{

	tftTextPadding = pixels;
	tftFieldStartX = tftCursorX;

}

// fill the remainder of the current padded field with background (no-op if
// padding is off or the text already filled/overran the field)
static void TFT::applyPadding(void)
{

	uint16_t endX;

	if (tftTextPadding)
	{

		endX = tftFieldStartX + tftTextPadding;
		if (tftCursorX < endX) ILI9341::fillRect(tftCursorX, tftCursorY, endX - tftCursorX, (uint16_t)(TFT_CELL_H) * tftScale, tftBGcolour);

	}

}

// change display orientation on the fly: 0/2 = portrait (240x320), 1/3 =
// landscape (320x240). writes MADCTL, updates the active width/height, and
// clears the screen in the new geometry.
// NOTE: rotation 0 keeps the known-good portrait MADCTL (0x08). The MX/MY on the
// landscape/flipped entries are the conventional choice; if a rotation comes out
// mirrored on this panel, swap MX<->MY on that entry.
static void TFT::setRotation(uint8_t rotation)
{

	uint8_t madctl;

	tftRotation = (rotation & 0x03);

	switch (tftRotation)
	{

		case 1: // landscape
			madctl = (ILI9341_MAD_MV | ILI9341_MAD_MX | ILI9341_MAD_BGR);
			tftWidth = ILI9341_TFTHEIGHT;
			tftHeight = ILI9341_TFTWIDTH;
			break;

		case 2: // portrait, rotated 180
			madctl = (ILI9341_MAD_MX | ILI9341_MAD_MY | ILI9341_MAD_BGR);
			tftWidth = ILI9341_TFTWIDTH;
			tftHeight = ILI9341_TFTHEIGHT;
			break;

		case 3: // landscape, rotated 180
			madctl = (ILI9341_MAD_MV | ILI9341_MAD_MY | ILI9341_MAD_BGR);
			tftWidth = ILI9341_TFTHEIGHT;
			tftHeight = ILI9341_TFTWIDTH;
			break;

		default: // case 0: portrait (known-good orientation)
			madctl = (ILI9341_MAD_BGR);
			tftWidth = ILI9341_TFTWIDTH;
			tftHeight = ILI9341_TFTHEIGHT;
			break;

	}

	spi::set(SPIconfigTFT);
	ILI9341::writeCommandByte(ILI9341_MADCTL);
	ILI9341::writeDataByte(madctl);

	TFT::clearScreen(); // reset cursor and repaint the background in the new geometry

}

// dev test screen exercising the drawing primitives + padding. assumes the
// screen was just cleared (the 'G' command calls this right after setRotation).
static void TFT::drawTestScreen(void)
{

	uint16_t cellW = (uint16_t)(TFT_CELL_W) * tftScale;
	uint16_t cellH = (uint16_t)(TFT_CELL_H) * tftScale;
	uint16_t y;

	// header bar (fillRect) with a title drawn white-on-blue
	ILI9341::fillRect(0, 0, tftWidth, cellH + 4, ILI9341_BLUE);
	TFT::setTextColour(ILI9341_WHITE, ILI9341_BLUE);
	TFT::setCursorPixel(4, 2);
	text::stringOut(m8DevTFTidx, PSTR("TFT test  rot "));
	text::charOut(m8DevTFTidx, (uint8_t)('0' + tftRotation));
	TFT::setTextColour(ILI9341_WHITE, ILI9341_BLACK);

	// a horizontal divider and a diagonal (drawLine: fast H/V path + Bresenham)
	y = cellH + 10;
	ILI9341::drawLine(0, y, tftWidth - 1, y, ILI9341_GREEN);
	ILI9341::drawLine(0, y + 6, 48, y + 54, ILI9341_RED);

	// fillRect colour swatches in the top-right
	ILI9341::fillRect(tftWidth - 58, y + 8, 16, 16, ILI9341_RED);
	ILI9341::fillRect(tftWidth - 38, y + 8, 16, 16, ILI9341_GREEN);
	ILI9341::fillRect(tftWidth - 18, y + 8, 16, 16, ILI9341_BLUE);

	// padding demo: fill an 8-char field with X's, then overwrite with a short
	// value. padding clears the stale tail, so the X's vanish without a clear.
	y = y + 70;
	TFT::setCursorPixel(4, y);
	text::stringOut(m8DevTFTidx, PSTR("pad:"));
	TFT::setTextPadding(8 * cellW);					// 8-char field starting after "pad:"
	text::stringOut(m8DevTFTidx, PSTR("XXXXXXXX"));	// fill the field
	heart::wait0(600);								// hold so the wide fill is visible
	TFT::setCursorPixel(4 + 4 * cellW, y);			// back to the field start
	TFT::setTextPadding(8 * cellW);					// re-arm padding for this field
	text::stringOut(m8DevTFTidx, PSTR("ok"));		// short value
	text::charOut(m8DevTFTidx, 0x0D);				// leaving the field clears its tail
	TFT::setTextPadding(0);							// padding off

}

#if defined(useILI9341)
/* ILI9341 TFT hardware support section */

static void ILI9341::init(void)
{

	const uint8_t * str;
	uint8_t strLen;
	uint8_t cmd;
	uint8_t dly;

	SPIconfigTFT = spi::configuration(4000000ul, SPI_MSBFIRST, SPI_MODE0);

	// enable TFT brightness
#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	DDRB |= (1 << DDB6); // backlight enable (PB6) as output
	DDRL |= (1 << DDL1); // DC (PL1 / Arduino pin 48) as output
	DDRC |= (1 << DDC7); // hardware /RESET (PC7 / Arduino pin 30) as output

	// pulse the ILI9341 hardware reset. the controller ignores all SPI traffic
	// until it is brought out of hardware reset, so this must happen before any
	// commands are sent. RESET is on PC7 (Arduino pin 30) per the original
	// abbalooga TFT_ILI9341 User_Setup.h; timing follows that library (5/20/150ms,
	// rounded up here).
	PORTC |= (1 << PORTC7); // /RESET high (inactive)
	heart::wait0(5);
	PORTC &= ~(1 << PORTC7); // assert /RESET (active low)
	heart::wait0(20);
	PORTC |= (1 << PORTC7); // release /RESET
	heart::wait0(200); // wait for the controller to become ready
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	DDRH |= ((1 << DDH4) | (1 << DDH3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
	setBrightness(1);

	spi::set(SPIconfigTFT);

	str = ILI9341initCmdTable;

	do
	{

		strLen = pgm_read_byte(str++);

		if (strLen)
		{

			cmd = pgm_read_byte(str++);

			if (cmd == ILI9341_SWRESET) heart::wait0(500);

			writeCommandByte(cmd);

			writeDataByteString(str, strLen - 1);

			if (cmd == ILI9341_SWRESET) heart::wait0(200);
			else if (cmd == ILI9341_SLPOUT) heart::wait0(200);

		}

	}
	while (strLen);

}

static void ILI9341::shutdown(void)
{

	// blank the panel and put the controller to sleep for lower power draw (GRAM is
	// retained, so resume() can light it back up without a full re-init). this is done
	// while the DC pin is still an output, before it is released below.
	spi::set(SPIconfigTFT);
	writeCommandByte(ILI9341_DISPOFF);
	writeCommandByte(ILI9341_SLPIN);

	setBrightness(0);

	// disable TFT brightness
#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	DDRB &= ~(1 << DDB6);
	DDRL &= ~(1 << DDL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	DDRH &= ~((1 << DDH4) | (1 << DDH3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

// reverse ILI9341::shutdown(): restore the released pins, wake the controller out of
// sleep, and re-enable the backlight. the controller kept VCC and its configuration
// (and GRAM), so the last image returns - no reset or command-table re-init needed.
static void ILI9341::resume(void)
{

	// restore the pins shutdown() released (DC must be an output before any command)
#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	DDRB |= (1 << DDB6);
	DDRL |= (1 << DDL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	DDRH |= ((1 << DDH4) | (1 << DDH3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)

	spi::set(SPIconfigTFT);
	writeCommandByte(ILI9341_SLPOUT);
	heart::wait0(150);						// datasheet: wait >= 120ms after sleep-out
	writeCommandByte(ILI9341_DISPON);

	setBrightness(1);

}

static void ILI9341::writeCommandByte(uint8_t byt)
{

	commandMode();
	assertCS();

	spi::transfer(byt);

	releaseCS();

}

static void ILI9341::writeDataByte(uint8_t byt)
{

	dataMode();
	assertCS();

	spi::transfer(byt);

	releaseCS();

}

static void ILI9341::writeDataWord(uint16_t wrd)
{

	union union_16 * W = (union union_16 *)(&wrd);

	dataMode();
	assertCS();

	spi::transfer(W->u08[1]);
	spi::transfer(W->u08[0]);

	releaseCS();

}

static void ILI9341::writeDataByteString(const uint8_t * &str, uint8_t strLen)
{

	if (strLen)
	{

		dataMode();
		assertCS();

		while (strLen--) spi::transfer(pgm_read_byte(str++));

		releaseCS();

	}

}

// set the active drawing rectangle, then issue RAMWR so pixel data can stream
static void ILI9341::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

	writeCommandByte(ILI9341_CASET); // column address set
	writeDataByte(x0 >> 8);
	writeDataByte(x0);
	writeDataByte(x1 >> 8);
	writeDataByte(x1);

	writeCommandByte(ILI9341_PASET); // page (row) address set
	writeDataByte(y0 >> 8);
	writeDataByte(y0);
	writeDataByte(y1 >> 8);
	writeDataByte(y1);

	writeCommandByte(ILI9341_RAMWR); // memory write

}

// fill the whole panel with one RGB565 color - a visible go/no-go bring-up test
static void ILI9341::fillScreen(uint16_t color)
{

	union union_16 * c = (union union_16 *)(&color);
	uint32_t pixels = (uint32_t)(tftWidth) * (uint32_t)(tftHeight);

	setAddrWindow(0, 0, tftWidth - 1, tftHeight - 1);

	// stream every pixel with CS held the whole time (MSB first)
	dataMode();
	assertCS();

	while (pixels--)
	{

		spi::transfer(c->u08[1]);
		spi::transfer(c->u08[0]);

	}

	releaseCS();

}

// render one 5x7 glyph into a (6*scale x 8*scale) cell at pixel (x,y), streaming
// foreground/background RGB565 per font bit. one address window, MSB-first.
static void ILI9341::drawChar(uint16_t x, uint16_t y, uint8_t c, uint16_t fg, uint16_t bg, uint8_t scale)
{

	uint8_t glyph[TFT_GLYPH_W];
	uint8_t fgHi = (uint8_t)(fg >> 8), fgLo = (uint8_t)(fg);
	uint8_t bgHi = (uint8_t)(bg >> 8), bgLo = (uint8_t)(bg);
	uint8_t row, col, sx, sy, colBits, on;

	if ((c < 0x20) || (c > 0x7F)) c = 0x20; // out-of-range glyphs render as a space

	for (col = 0; col < TFT_GLYPH_W; col++) glyph[(uint16_t)(col)] = pgm_read_byte(&ILI9341font5x7[(uint16_t)(c - 0x20) * TFT_GLYPH_W + col]);

	setAddrWindow(x, y, x + (uint16_t)(TFT_CELL_W) * scale - 1, y + (uint16_t)(TFT_CELL_H) * scale - 1);

	dataMode();
	assertCS();

	for (row = 0; row < TFT_CELL_H; row++) // rows top to bottom
		for (sy = 0; sy < scale; sy++) // vertical scaling
			for (col = 0; col < TFT_CELL_W; col++) // columns left to right (col 5 = spacing)
			{

				on = ((col < TFT_GLYPH_W) && (row < 7)) ? ((glyph[(uint16_t)(col)] >> row) & 0x01) : 0;

				for (sx = 0; sx < scale; sx++) // horizontal scaling
				{

					if (on) { spi::transfer(fgHi); spi::transfer(fgLo); }
					else { spi::transfer(bgHi); spi::transfer(bgLo); }

				}

			}

	releaseCS();

}

// fill a rectangle with a solid RGB565 colour (one address window, MSB-first)
static void ILI9341::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{

	uint32_t pixels;
	uint8_t hi = (uint8_t)(color >> 8), lo = (uint8_t)(color);

	if ((w == 0) || (h == 0)) return;

	pixels = (uint32_t)(w) * (uint32_t)(h);

	setAddrWindow(x, y, x + w - 1, y + h - 1);
	dataMode();
	assertCS();

	while (pixels--) { spi::transfer(hi); spi::transfer(lo); }

	releaseCS();

}

// set a single pixel
static void ILI9341::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{

	setAddrWindow(x, y, x, y);
	dataMode();
	assertCS();

	spi::transfer((uint8_t)(color >> 8));
	spi::transfer((uint8_t)(color));

	releaseCS();

}

// draw a line. horizontal/vertical lines stream as a 1px rect; diagonals use Bresenham.
static void ILI9341::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{

	int16_t dx, dy, sx, sy, err, e2, cx, cy;

	if (y0 == y1) { if (x1 >= x0) fillRect(x0, y0, x1 - x0 + 1, 1, color); else fillRect(x1, y0, x0 - x1 + 1, 1, color); return; }
	if (x0 == x1) { if (y1 >= y0) fillRect(x0, y0, 1, y1 - y0 + 1, color); else fillRect(x0, y1, 1, y0 - y1 + 1, color); return; }

	dx = (int16_t)(x1) - (int16_t)(x0);
	dy = (int16_t)(y1) - (int16_t)(y0);
	sx = (dx >= 0) ? 1 : -1;
	sy = (dy >= 0) ? 1 : -1;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	err = ((dx > dy) ? dx : -dy) / 2;
	cx = (int16_t)(x0);
	cy = (int16_t)(y0);

	while (1)
	{

		drawPixel((uint16_t)(cx), (uint16_t)(cy), color);
		if ((cx == (int16_t)(x1)) && (cy == (int16_t)(y1))) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; cx += sx; }
		if (e2 < dy) { err += dx; cy += sy; }

	}

}

__attribute__((always_inline))
static void ILI9341::setBrightness(uint8_t brightIdx)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	if (brightIdx) PORTB |= (1 << PORTB6);
	else PORTB &= ~(1 << PORTB6);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	if (brightIdx) PORTH |= (1 << PORTH4);
	else PORTH &= ~(1 << PORTH4);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static uint8_t ILI9341::isCSreleased(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	return (PORTL & (1 << PORTL2));
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	return (PORTE & (1 << PORTE3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::releaseCS(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL |= (1 << PORTL2);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTE |= (1 << PORTE3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::assertCS(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL &= ~(1 << PORTL2);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTE &= ~(1 << PORTE3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::dataMode(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL |= (1 << PORTL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTH |= (1 << PORTH3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::commandMode(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL &= ~(1 << PORTL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTH &= ~(1 << PORTH3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

#endif // defined(useILI9341)
#endif // defined(useTFToutput)
