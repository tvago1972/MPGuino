#if defined(useTFToutput) && !defined(useButtonInput)
/* TFT primary-display main screen (reuses the first LCD page's functions) */

static void tftMain::init(void)
{

	TFT::clearScreen();
	tftMain::update();

}

// render the four instrument functions in a 2x2 grid. nothing is cleared first -
// each glyph cell is drawn with its own background (drawChar streams fg+bg in one
// pass, so there is no blank flash), and the value field is padded so a shorter new
// value clears the stale tail of a longer old one. labels are static and overwrite
// themselves. this in-place redraw is flicker-free; a one-time clear happens in
// init() (and whenever a modal/wake repaints the screen before update() resumes).
static void tftMain::update(void)
{

	uint16_t qW = tftWidth / 2;
	uint16_t qH = tftHeight / 2;
	uint16_t cellW = (uint16_t)(TFT_CELL_W) * tftScale;
	uint16_t lineH = (uint16_t)(TFT_CELL_H) * tftScale + 4;
	uint8_t x;

	for (x = 0; x < 4; x++)
	{

		uint16_t qx = (uint16_t)(x & 1) * qW;
		uint16_t qy = (uint16_t)(x >> 1) * qH;

		// each field sets its own padding; the next setCursorPixel flushes the
		// previous field (clearing a shrunken value/units tail). the number and its
		// units go on separate lines so a wide value can't bleed into the neighbour.

		TFT::setTextColour(tftMainLabelFG, tftMainBG);
		TFT::setCursorPixel(qx + 4, qy + 4);						// flushes the previous quadrant's last field
		TFT::setTextPadding(tftMainWindow * cellW);
		text::stringOut(m8DevTFTidx, findStr(tftMainLabels, x));

		TFT::setTextColour(tftMainValueFG, tftMainBG);
		TFT::setCursorPixel(qx + 4, qy + 4 + lineH);
		TFT::setTextPadding(tftMainWindow * cellW);
		text::tripFunctionOut(m8DevTFTidx, pgm_read_word(&tftMainFormats[(uint16_t)(x)]), tftMainWindow, 0);	// number only (no inline units)

		TFT::setTextColour(tftMainLabelFG, tftMainBG);
		TFT::setCursorPixel(qx + 4, qy + 4 + 2 * lineH);
		TFT::setTextPadding(tftMainWindow * cellW);
		text::stringOut(m8DevTFTidx, findStr(tftMainUnits, mainCalcFuncVar.calcFmtIdx));	// short TFT units for the format tripFunctionOut just resolved

	}

	TFT::applyPadding();	// flush the final field's padding
	TFT::setTextPadding(0);

}

#endif // defined(useTFToutput) && !defined(useButtonInput)
