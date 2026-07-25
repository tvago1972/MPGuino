#if defined(useTFToutput) && !defined(useButtonInput)
/* TFT primary-display main screen (reuses the first LCD page's functions) */

#if defined(useTouchScreenInput)
// non-blocking settings-gear state (persists across pollTouch() calls); declared
// here so update() can draw the gear in its current (idle/held) colour
static uint8_t tftGearTracking;		// a press is being followed
static uint8_t tftGearHeld;			// the followed press is on the gear (main screen)
static uint8_t tftGearResolved;		// gearHeld has been determined for the current press (a read succeeded)
static uint32_t tftGearHoldStart;	// cycles0() when the gear press began
static uint16_t tftGearBarW;		// last progress-bar width drawn (avoid redundant fills)
static uint16_t tftTapX, tftTapY;	// press-edge coords, dispatched as a tap on release (non-main screens)
static uint8_t tftLongFired;		// a long-press has already been dispatched for the current press
static uint8_t tftTouchSuppress;	// while blanked (and until the wake touch releases) ignore touch input
static uint32_t tftGearLastTapCycles;	// cycles0() of the last un-paired gear tap release (0 = none pending)
static uint8_t tftGearReleasePending;	// a not-pressed reading is being debounced before it counts as a release
static uint32_t tftGearReleaseStart;	// cycles0() when the pending release began

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

	tftScreen = tftScreenMain;	// boot / exit-settings: go to the dashboard
	TFT::clearScreen();
	tftMain::update();	// update() draws the gear too, so it survives any screen clear

}

// redraw whichever screen is current, without changing tftScreen. used on wake (after
// the panel's full re-init) so a blank-from-inactivity returns to where the user was -
// the dashboard, a settings list, the dropdown, or the keypad (entry preserved).
static void tftMain::repaint(void)
{

	switch (tftScreen)
	{

#if defined(useTouchScreenInput)
		case tftScreenSettings:	tftSettings::redraw();			break;
		case tftScreenDropdown:	tftSettings::redrawDropdown();	break;
		case tftScreenKeypad:	keypad::draw();					break;
		// TFT::init() (full wake re-init) always resets rotation to 0, wiping the
		// orientation-3 reference calibration was using mid-capture - resuming would
		// silently corrupt the result, so abort back to the dashboard instead.
		case tftScreenCalibrate:	tftMain::init();				break;
#endif // defined(useTouchScreenInput)
		default:				tftMain::update();				break;	// dashboard (TFT::init already cleared the screen)

	}

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

	// while the screen is blanked from inactivity, a touch must only WAKE it (handled
	// by the ISR activity hook + the wake repaint), not act as UI input - and once
	// woken, ignore that same touch until it releases so it can't trigger an action.
	if (v08(v8ActivityIdx) & afActivityTimeoutFlag) { tftTouchSuppress = 1; tftGearTracking = 0; return; }
	if (tftTouchSuppress) { if (touch::pressed()) return; tftTouchSuppress = 0; }

	if (tftScreen == tftScreenMain)	// dashboard: double-tap the gear -> settings; HOLD the gear -> calibration
	{

		if (touch::pressed())
		{

			tftGearReleasePending = 0;	// still (or again) down: cancel any pending release (PENIRQ can glitch high mid-press)

			if (!tftGearTracking)	// press edge: not yet resolved whether it landed on the gear
			{

				tftGearTracking = 1;
				tftGearResolved = 0;
				tftGearHeld = 0;						// stale from a prior press otherwise, since it's only set once resolved
				tftGearHoldStart = heart::cycles0();	// start the hold clock at the press edge, not once resolved

			}

			if (!tftGearResolved)	// keep sampling each pass until a read succeeds (the first sample right at
			{						// contact is often unstable) or the finger lifts before one does

				if (touch::read(&tx, &ty))
				{

					tftGearResolved = 1;
					tftGearHeld = tftMainOnGear(tx, ty);
					tftGearBarW = 0;

					if (tftGearHeld) tftMainBlitGear(tftGearActiveFG);	// acknowledge the touch immediately (cyan)

				}

			}
			else if (tftGearHeld)	// on-gear press: once it outlasts a tap, grow the hold bar; confirm calibration at the threshold
			{

				uint32_t elapsed = heart::cycles0() - tftGearHoldStart;

				if (elapsed >= tftGearTapMaxCycles)	// past tap territory: this is a deliberate hold, show progress
				{

					uint16_t span = tftGearHoldCycles - tftGearTapMaxCycles;
					uint16_t w = (elapsed >= tftGearHoldCycles) ? (2 * tftGearR) : (uint16_t)((uint32_t)(elapsed - tftGearTapMaxCycles) * 2 * tftGearR / span);

					if (w != tftGearBarW) { ILI9341::fillRect(gx, barY, w, 3, tftGearProgressFG); tftGearBarW = w; }

				}

				if (elapsed >= tftGearHoldCycles)	// confirmed hold: switch to touch calibration (finger still down)
				{

					tftGearTracking = 0;
					tftGearHeld = 0;
					tftGearResolved = 0;
					tftGearLastTapCycles = 0;		// this gesture is consumed; don't let it also complete as a double-tap
					ILI9341::fillRect(gx, barY, 2 * tftGearR, 3, tftMainBG);	// clear the hold bar
					tftScreen = tftScreenCalibrate;
					touch::calibEnter();			// draw the first crosshair (non-blocking from here on)

				}

			}

		}
		else if (tftGearTracking)	// finger reads up - but debounce it: PENIRQ glitches high mid-press (esp. after a read)
		{

			if (!tftGearReleasePending) { tftGearReleasePending = 1; tftGearReleaseStart = heart::cycles0(); }
			else if (heart::cycles0() - tftGearReleaseStart >= tftGearReleaseDebounce)	// release confirmed (stayed up)
			{

				// timings use the finger-lift instant (tftGearReleaseStart), not now, so the
				// debounce wait doesn't inflate the measured press/gap durations.
				// a short press on the gear is a tap (counts toward the double-tap); a press that
				// outlasted a tap but never reached the hold threshold is an aborted hold - ignored,
				// so a partial hold can't be mistaken for a settings tap.
				if (tftGearHeld && (tftGearReleaseStart - tftGearHoldStart <= tftGearTapMaxCycles))
				{

					if (tftGearLastTapCycles && (tftGearReleaseStart - tftGearLastTapCycles <= tftGearDoubleTapCycles))
					{

						tftGearLastTapCycles = 0;
						tftScreen = tftScreenSettings;
						tftSettings::enter();		// draw the group menu (non-blocking from here on)

					}
					else tftGearLastTapCycles = tftGearReleaseStart;	// arm: a second tap within the window completes the double-tap

				}

				ILI9341::fillRect(gx, barY, 2 * tftGearR, 3, tftMainBG);	// clear any hold bar
				if (tftGearHeld) tftMainBlitGear(tftGearFG);			// restore the idle gear (cyan -> grey)
				tftGearTracking = 0;
				tftGearHeld = 0;
				tftGearResolved = 0;
				tftGearReleasePending = 0;

			}

		}

	}
	else if (tftScreen == tftScreenCalibrate)	// touch calibration: fed one pass at a time, owns its own press/release logic
	{

		if (touch::calibPoll()) tftMain::init();	// all 4 corners captured + stored -> back to the dashboard

	}
	else	// settings / dropdown / keypad: a tap on release, plus a long-press for the keypad's DEL-cancel
	{

		if (touch::pressed())
		{

			if (!tftGearTracking)	// press edge: sample coords (retry until stable), start the hold timer
			{

				if (touch::read(&tftTapX, &tftTapY))
				{

					tftGearTracking = 1;
					tftLongFired = 0;
					tftGearHoldStart = heart::cycles0();

				}

			}
			else if ((!tftLongFired) && (tftScreen == tftScreenKeypad) && (heart::cycles0() - tftGearHoldStart >= tftGearHoldCycles))
			{

				tftLongFired = 1;	// keypad DEL long-press -> cancel the edit

				if (keypad::longPress(tftTapX, tftTapY)) { tftSettings::cancelEdited(); tftScreen = tftScreenSettings; }

			}

		}
		else if (tftGearTracking)
		{

			tftGearTracking = 0;

			if (tftLongFired) { } // a long-press already handled this press
			else if (tftScreen == tftScreenSettings)
			{

				uint8_t r = tftSettings::tap(tftTapX, tftTapY);

				if (r == tftSettingsExit) tftMain::init();						// Exit -> dashboard (init resets tftScreen)
				else if (r == tftSettingsDropdown) tftScreen = tftScreenDropdown;	// a choice param opened the dropdown
				else if (r == tftSettingsKeypad) tftScreen = tftScreenKeypad;		// a numeric param opened the keypad

			}
			else if (tftScreen == tftScreenDropdown)
			{

				if (!tftSettings::dropdownTap(tftTapX, tftTapY)) tftScreen = tftScreenSettings;	// done -> back to the parameter list

			}
			else if (tftScreen == tftScreenKeypad)
			{

				if (keypad::tap(tftTapX, tftTapY)) { tftSettings::storeEdited(keypad::value()); tftScreen = tftScreenSettings; }	// OK -> store + back to params

			}

		}

	}

}
#endif // defined(useTouchScreenInput)

#endif // defined(useTFToutput) && !defined(useButtonInput)
