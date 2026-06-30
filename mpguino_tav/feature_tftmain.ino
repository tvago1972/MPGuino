#if defined(useTFToutput) && !defined(useButtonInput)
/* TFT primary-display main screen (reuses the first LCD page's functions) */

#if defined(useTouchScreenInput)
// draw the settings gear (a small square cog with four teeth and a centre hole) in
// the top-right corner, in the given colour. drawn in place every update() (no
// leading footprint-clear, so it doesn't flicker) - the corner is already cleared
// to background by the last full screen clear, and nothing else draws there.
static void tftMainBlitGear(uint16_t fg)
{

	uint16_t gx = tftWidth - 2 * tftGearR - tftGearMargin;
	uint16_t gy = tftGearMargin;
	uint16_t cx = gx + tftGearR;
	uint16_t cy = gy + tftGearR;

	ILI9341::fillRect(cx - 2, gy, 4, 3, fg);								// top tooth
	ILI9341::fillRect(cx - 2, gy + 2 * tftGearR - 3, 4, 3, fg);				// bottom tooth
	ILI9341::fillRect(gx, cy - 2, 3, 4, fg);								// left tooth
	ILI9341::fillRect(gx + 2 * tftGearR - 3, cy - 2, 3, 4, fg);				// right tooth
	ILI9341::fillRect(cx - 6, cy - 6, 12, 12, fg);							// body
	ILI9341::fillRect(cx - 3, cy - 3, 6, 6, tftMainBG);						// centre hole

}
#endif // defined(useTouchScreenInput)

static void tftMain::init(void)
{

	TFT::clearScreen();
	tftMain::update();	// update() draws the gear too, so it survives any screen clear

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

	// activity/sleep countdown bar along the very bottom (clear of the quadrant text)
	TFT::drawActivityBar();

#if defined(useTouchScreenInput)
	tftMainBlitGear(tftGearFG);	// settings gear (redrawn every frame so it survives screen clears)
#endif // defined(useTouchScreenInput)

}

#if defined(useTouchScreenInput)
// is a touch (tx,ty) within the gear's generous top-right-corner hit target?
static uint8_t tftMainOnGear(uint16_t tx, uint16_t ty)
{

	return ((tx >= tftWidth - tftGearHitSize) && (ty < tftGearHitSize));

}

// poll the settings gear: a touch on it must be HELD (not tapped) to open the
// settings editor, so it can't be triggered by accident. while held, a green bar
// under the gear fills; releasing or sliding off before the threshold aborts.
static void tftMain::pollTouch(void)
{

	uint16_t tx, ty, held;
	uint16_t gx = tftWidth - 2 * tftGearR - tftGearMargin;
	uint16_t barY = tftGearMargin + 2 * tftGearR + 2;

	if (!touch::read(&tx, &ty)) return;				// not touched (cheap when idle), or no stable sample
	if (!tftMainOnGear(tx, ty)) return;				// touch elsewhere: ignored on the main screen

	tftMainBlitGear(tftGearActiveFG);				// acknowledge the press

	for (held = 0; held < tftGearHoldTicks; held++)
	{

		if (!touch::read(&tx, &ty)) break;			// released / unstable -> abort
		if (!tftMainOnGear(tx, ty)) break;			// slid off the gear -> abort

		ILI9341::fillRect(gx, barY, (uint16_t)((uint32_t)(held + 1) * 2 * tftGearR / tftGearHoldTicks), 3, tftGearProgressFG);

		heart::wait0(8);

	}

	if (held >= tftGearHoldTicks)					// confirmed: open the settings editor
	{

		tftSettings::run();
		tftMain::init();							// settings cleared the screen: repaint everything (incl. gear)

	}
	else											// aborted: erase the progress bar and restore the idle gear
	{

		ILI9341::fillRect(gx, barY, 2 * tftGearR, 3, tftMainBG);
		tftMainBlitGear(tftGearFG);

	}

}
#endif // defined(useTouchScreenInput)

#endif // defined(useTFToutput) && !defined(useButtonInput)
