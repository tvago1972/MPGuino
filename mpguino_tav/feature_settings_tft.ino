#if defined(useTFToutput) && defined(useTouchScreenInput)
/* on-screen (touch) EEPROM settings editor (driven non-blocking by the main loop:
   the coordinator detects taps and calls tftSettings::tap/dropdownTap) */

// list row height for the current orientation (taller in portrait for easier taps)
static uint16_t tftSettingsRowHeight(void)
{

	return (tftHeight > tftWidth) ? tftSettingsRowHportrait : tftSettingsRowHlandscape;

}

// option-label list for a boolean/enum parameter (value i -> i-th substring), or
// 0 if the parameter is edited numerically with the keypad
static const char * tftSettingsChoices(uint8_t parameterIdx)
{

	switch (parameterIdx)
	{

		case pMetricModeIdx:		return tftChoiceYesNo;
		case pAlternateFEidx:		return tftChoiceYesNo;
		case pInjEdgeTriggerIdx:	return tftChoiceInjTrigger;
		default:					return 0;

	}

}


// map a touch y to a list row: 0..count-1 for a row, 0xFF for the footer button,
// 0xFE for the title bar / empty space (ignored)
static uint8_t tftSettingsRowAt(uint16_t py, uint8_t count)
{

	uint8_t r;

	if (py >= tftHeight - tftSettingsFooterH - tftSettingsBottomReserve) return 0xFF;	// footer (back / exit)
	if (py < tftSettingsTitleH) return 0xFE;				// title bar

	r = (uint8_t)((py - tftSettingsTitleH) / tftSettingsRowHeight());

	return (r < count) ? r : 0xFE;

}

// title bar + footer button + cleared list area
static void tftSettingsFrame(const char * title, const char * footer)
{

	uint16_t fy = tftHeight - tftSettingsFooterH - tftSettingsBottomReserve;

	ILI9341::fillRect(0, 0, tftWidth, tftSettingsTitleH, tftSettingsTitleBG);
	TFT::setTextColour(tftSettingsRowFG, tftSettingsTitleBG);
	TFT::setCursorPixel(6, (tftSettingsTitleH - 16) / 2);
	text::stringOut(m8DevTFTidx, title);

	ILI9341::fillRect(0, tftSettingsTitleH, tftWidth, fy - tftSettingsTitleH, ILI9341_BLACK); // list area

	uint8_t flen = 0;
	uint16_t fcw = (uint16_t)(TFT_CELL_W) * tftScale;		// footer text cell width
	uint16_t fx;

	while (pgm_read_byte(&footer[(uint16_t)(flen)])) flen++;	// length of the PROGMEM footer label
	fx = (tftWidth > (uint16_t)(flen) * fcw) ? ((tftWidth - (uint16_t)(flen) * fcw) / 2) : 0;

	ILI9341::fillRect(0, fy, tftWidth, tftSettingsFooterH, tftSettingsDivider);
	TFT::setTextColour(tftSettingsRowFG, tftSettingsDivider);
	TFT::setCursorPixel(fx, fy + (tftSettingsFooterH - 16) / 2);
	text::stringOut(m8DevTFTidx, footer);

#if defined(useTFTsleepBarEverywhere)
	TFT::drawActivityBar();	// activity/sleep bar in the reserved bottom strip
#endif // defined(useTFTsleepBarEverywhere)

}

// draw a list row: a white label, an optional right-aligned yellow value, divider
// (label is a PROGMEM string; value is a RAM string or 0 for none)
static void tftSettingsRow(uint8_t r, const char * label, char * value)
{

	uint16_t rowH = tftSettingsRowHeight();
	uint16_t y = tftSettingsTitleH + (uint16_t)(r) * rowH;
	uint16_t ty = y + (rowH - 16) / 2;

	TFT::setTextColour(tftSettingsRowFG, ILI9341_BLACK);
	TFT::setCursorPixel(6, ty);
	text::stringOut(m8DevTFTidx, label);

	if (value)
	{

		uint8_t vlen = 0;

		while (value[(uint16_t)(vlen)]) vlen++;					// length of the RAM value string

		uint16_t vw = (uint16_t)(vlen) * (uint16_t)(TFT_CELL_W) * 2;	// scale-2 cell width
		uint16_t vx = (tftWidth > vw + 6) ? (tftWidth - 6 - vw) : 6;

		TFT::setTextColour(tftSettingsValFG, ILI9341_BLACK);
		TFT::setCursorPixel(vx, ty);
		text::stringOut(m8DevTFTidx, value);

	}

	ILI9341::drawLine(0, y + rowH - 1, tftWidth - 1, y + rowH - 1, tftSettingsDivider);

}

// settings sub-state (the top-level Main/Settings screen is owned by feature_tftmain)
static uint8_t tftSettingsScreen;	// 0 = group menu, 1 = parameter list
static uint8_t tftSettingsGroup;	// selected group, for the parameter list

// draw the group menu
static void tftSettingsDrawGroups(void)
{

	uint8_t g;

	tftSettingsFrame(PSTR("Settings"), PSTR("Exit"));

	for (g = 0; g < tftSettingsGroupCount; g++) tftSettingsRow(g, findStr(tftSettingsGroupNames, g), 0);

}

// draw the parameter list for the selected group (label + current value per row)
static void tftSettingsDrawParams(void)
{

	uint8_t start = pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(tftSettingsGroup)]);
	uint8_t count = (uint8_t)(pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(tftSettingsGroup + 1)]) - start);
	uint8_t r;

	tftSettingsFrame(findStr(tftSettingsGroupNames, tftSettingsGroup), PSTR("Back"));

	for (r = 0; r < count; r++)
	{

		uint8_t p = pgm_read_byte(&tftSettingsParams[(uint16_t)(start + r)]);
		const char * choices;
		char * vp;

		numberEditObj.parameterIdx = p;
		parameterEdit::sharedFunctionCall(nesLoadInitial);	// pBuff = current value (space-padded digits)

		choices = tftSettingsChoices(p);

		if (choices) // show the option label for the current value, not the raw number
		{

			strcpy_P(pBuff, findStr(choices, (uint8_t)(str2ull(pBuff))));
			vp = pBuff;

		}
		else
		{

			vp = pBuff;
			while (*vp == ' ') vp++;						// skip leading spaces

		}

		tftSettingsRow(r, findStr(tftSettingsLabels, start + r), vp);

	}

}

// dropdown (choice parameter) edit context, set when a choice param is tapped
static uint8_t tftDropParam;			// the parameter being chosen
static const char * tftDropOptions;		// its option-label list
static uint8_t tftDropCount;			// number of options (= value range)
static uint8_t tftDropLabelIdx;			// label index (for the dropdown title)

// draw the option dropdown: the options with `current` highlighted, Cancel footer
static void tftSettingsDrawDropdown(uint8_t current)
{

	uint16_t rowH = tftSettingsRowHeight();
	uint8_t i;

	tftSettingsFrame(findStr(tftSettingsLabels, tftDropLabelIdx), PSTR("Cancel"));

	for (i = 0; i < tftDropCount; i++)
	{

		uint16_t y = tftSettingsTitleH + (uint16_t)(i) * rowH;
		uint16_t bg = (i == current) ? tftSettingsSelBG : ILI9341_BLACK;	// highlight the current value

		ILI9341::fillRect(0, y, tftWidth, rowH - 1, bg);
		TFT::setTextColour(tftSettingsRowFG, bg);
		TFT::setCursorPixel(6, y + (rowH - 16) / 2);
		text::stringOut(m8DevTFTidx, findStr(tftDropOptions, i));
		ILI9341::drawLine(0, y + rowH - 1, tftWidth - 1, y + rowH - 1, tftSettingsDivider);

	}

}

// set up and draw the dropdown for a choice parameter (current value highlighted)
static void tftSettingsOpenDropdown(uint8_t parameterIdx, const char * choices, uint8_t labelIdx)
{

	uint32_t cur, maxValue;

	numberEditObj.parameterIdx = parameterIdx;
	parameterEdit::sharedFunctionCall(nesLoadInitial);		// pBuff = current value; reg 2 = value
	cur = str2ull(pBuff);
	maxValue = SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchMaximumParamValue), parameterIdx);	// 2^bits - 1

	tftDropParam = parameterIdx;
	tftDropOptions = choices;
	tftDropCount = (uint8_t)(maxValue + 1);
	tftDropLabelIdx = labelIdx;

	tftSettingsDrawDropdown((uint8_t)(cur));

}

// edit a numeric parameter with the keypad (seeded with the current value, bounded
// by the bit-width max). still blocking - converted to a screen state in Stage 3b.
static void tftSettingsEdit(uint8_t parameterIdx, const char * label)
{

	uint32_t cur, maxValue, newValue;

	numberEditObj.parameterIdx = parameterIdx;
	parameterEdit::sharedFunctionCall(nesLoadInitial);		// pBuff = current value; reg 2 = value
	cur = str2ull(pBuff);									// current value (also reloads reg 2)
	maxValue = SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchMaximumParamValue), parameterIdx);	// 2^bits - 1

	if (!keypad::getNumber(&newValue, maxValue, cur, label)) return;	// cancelled

	SWEET64::init64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), newValue);	// reg 2 = new value
	EEPROM::onChange(S64_PRGM_PTR(prgmWriteParameterValue), parameterIdx);			// store + housekeeping

}

// open the settings editor at the group menu (the coordinator switches to the
// Settings screen and calls this; the main loop keeps running throughout)
static void tftSettings::enter(void)
{

	tftSettingsScreen = 0;
	tftSettingsDrawGroups();

}

// handle one tap on the group menu / parameter list. returns a tftSettings* code
// the coordinator acts on. a choice parameter opens the dropdown (returns Dropdown);
// a numeric parameter still uses the blocking keypad for now (Stage 3b).
static uint8_t tftSettings::tap(uint16_t px, uint16_t py)
{

	if (tftSettingsScreen == 0)	// group menu
	{

		uint8_t hit = tftSettingsRowAt(py, tftSettingsGroupCount);

		if (hit == 0xFF) return tftSettingsExit;			// Exit -> main screen
		if (hit < tftSettingsGroupCount)					// pick a group -> parameter list
		{

			tftSettingsGroup = hit;
			tftSettingsScreen = 1;
			tftSettingsDrawParams();

		}

	}
	else						// parameter list
	{

		uint8_t start = pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(tftSettingsGroup)]);
		uint8_t count = (uint8_t)(pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(tftSettingsGroup + 1)]) - start);
		uint8_t hit = tftSettingsRowAt(py, count);

		if (hit == 0xFF) { tftSettingsScreen = 0; tftSettingsDrawGroups(); }	// Back -> group menu
		else if (hit < count)								// pick a parameter -> edit it
		{

			uint8_t j = start + hit;
			uint8_t p = pgm_read_byte(&tftSettingsParams[(uint16_t)(j)]);
			const char * choices = tftSettingsChoices(p);

			if (choices)									// boolean/enum -> open the dropdown (non-blocking)
			{

				tftSettingsOpenDropdown(p, choices, j);
				return tftSettingsDropdown;

			}

			tftSettingsEdit(p, findStr(tftSettingsLabels, j));	// numeric -> keypad (TEMP blocking, Stage 3b)
			tftSettingsDrawParams();						// redraw with the (possibly) new value

		}

	}

	return tftSettingsStay;

}

// handle one tap on the option dropdown: a tap on an option stores it; the Cancel
// footer discards. either way return to the parameter list (redrawn). returns
// 0 = done (back to the parameter list), 1 = stay (tap missed an option/footer).
static uint8_t tftSettings::dropdownTap(uint16_t px, uint16_t py)
{

	uint8_t hit = tftSettingsRowAt(py, tftDropCount);

	if (hit < tftDropCount)	// selected an option -> store it with housekeeping
	{

		SWEET64::init64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), (uint32_t)(hit));
		EEPROM::onChange(S64_PRGM_PTR(prgmWriteParameterValue), tftDropParam);

	}
	else if (hit != 0xFF) return 1;	// tap landed on the title/gap -> ignore, stay in the dropdown

	tftSettingsDrawParams();		// option chosen or Cancel: back to the parameter list
	return 0;

}

#endif // defined(useTFToutput) && defined(useTouchScreenInput)
