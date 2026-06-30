#if defined(useTFToutput) && !defined(useButtonInput)
/* TFT primary-display main screen (reuses the first LCD page's functions) */

#if defined(useTouchScreenInput)
// non-blocking settings-gear state (persists across pollTouch() calls); declared
// here so update() can draw the gear in its current (idle/held) colour
static uint8_t tftGearTracking;		// a press is being followed
static uint8_t tftGearHeld;			// the followed press is on the gear (main screen)
static uint32_t tftGearHoldStart;	// cycles0() when the gear press began
static uint16_t tftGearBarW;		// last progress-bar width drawn (avoid redundant fills)
static uint16_t tftTapX, tftTapY;	// press-edge coords, dispatched as a tap on release (non-main screens)

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

	tftScreen = tftScreenMain;	// boot/wake/exit-settings always returns to the dashboard
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
	tftMainBlitGear(tftGearHeld ? tftGearActiveFG : tftGearFG);	// gear, in its current state (redrawn each frame so it survives screen clears)
#endif // defined(useTouchScreenInput)

}

#if defined(useTouchScreenInput)
// is a touch (tx,ty) within the gear's generous top-right-corner hit target?
static uint8_t tftMainOnGear(uint16_t tx, uint16_t ty)
{

	return ((tx >= tftWidth - tftGearHitSize) && (ty < tftGearHitSize));

}

// non-blocking settings-gear poll, called every main-loop pass (so the loop keeps
// running - trip processing, datalogging, etc. - while the user interacts). a touch
// on the gear must be HELD (not tapped) to open the settings editor, so it can't be
// triggered by accident; a green bar under the gear fills as the hold accrues, and
// releasing/sliding off before the threshold aborts. state persists across calls;
// the hold is timed with cycles0() so it doesn't depend on the loop rate.
static void tftMain::pollTouch(void)
{

	uint16_t tx, ty;
	uint16_t gx = tftWidth - 2 * tftGearR - tftGearMargin;
	uint16_t barY = tftGearMargin + 2 * tftGearR + 2;

	if (tftScreen == tftScreenMain)	// dashboard: the gear must be HELD to open settings
	{

		if (touch::pressed())
		{

			if (!tftGearTracking)	// press edge: sample once, decide if it landed on the gear
			{

				tftGearTracking = 1;
				tftGearHeld = (touch::read(&tx, &ty) && tftMainOnGear(tx, ty));

				if (tftGearHeld)
				{

					tftGearHoldStart = heart::cycles0();
					tftGearBarW = 0;
					tftMainBlitGear(tftGearActiveFG);

				}

			}
			else if (tftGearHeld)	// holding on the gear: grow the progress bar, confirm at threshold
			{

				uint32_t elapsed = heart::cycles0() - tftGearHoldStart;
				uint16_t w = (elapsed >= tftGearHoldCycles) ? (2 * tftGearR) : (uint16_t)((uint32_t)(elapsed) * 2 * tftGearR / tftGearHoldCycles);

				if (w != tftGearBarW) { ILI9341::fillRect(gx, barY, w, 3, tftGearProgressFG); tftGearBarW = w; }

				if (elapsed >= tftGearHoldCycles)	// confirmed: switch to the settings screen
				{

					tftGearTracking = 0;
					tftGearHeld = 0;
					tftScreen = tftScreenSettings;
					tftSettings::enter();			// draw the group menu (non-blocking from here on)

				}

			}

		}
		else	// finger up
		{

			if (tftGearTracking && tftGearHeld)	// released before confirm: abort, restore the idle gear
			{

				ILI9341::fillRect(gx, barY, 2 * tftGearR, 3, tftMainBG);
				tftMainBlitGear(tftGearFG);

			}

			tftGearTracking = 0;
			tftGearHeld = 0;

		}

	}
	else	// settings (and later keypad/dropdown): dispatch a tap on release
	{

		if (touch::pressed())
		{

			if (!tftGearTracking) { tftGearTracking = 1; touch::read(&tftTapX, &tftTapY); }	// sample press-edge coords

		}
		else
		{

			if (tftGearTracking)
			{

				tftGearTracking = 0;
				if (!tftSettings::tap(tftTapX, tftTapY)) tftMain::init();	// tap returned "exit" -> back to the dashboard (init resets tftScreen)

			}

		}

	}

}
#endif // defined(useTouchScreenInput)

#endif // defined(useTFToutput) && !defined(useButtonInput)
