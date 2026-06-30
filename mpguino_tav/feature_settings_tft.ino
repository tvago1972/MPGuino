#if defined(useTFToutput) && defined(useTouchScreenInput)
/* on-screen (touch) EEPROM settings editor */

// wait for one debounced tap and return its press coords. there is no idle timeout:
// the settings screens stay open until the user taps a row or the Back/Exit footer
// (like the LCD settings menu), so pausing to recall a value never reverts the screen.
static void tftSettingsTap(uint16_t * px, uint16_t * py)
{

	uint16_t x, y;

	while (1)
	{

		if (touch::read(&x, &y))
		{

			*px = x;
			*py = y;
			while (touch::pressed()) heart::wait0(8);	// wait for release
			return;

		}

		heart::wait0(8);

	}

}

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

// group selector: draw the group list, return the chosen group, or 0xFF to exit
static uint8_t tftSettingsGroupScreen(void)
{

	uint16_t px, py;
	uint8_t g, hit;

	tftSettingsFrame(PSTR("Settings"), PSTR("Exit"));

	for (g = 0; g < tftSettingsGroupCount; g++) tftSettingsRow(g, findStr(tftSettingsGroupNames, g), 0);

	while (1)
	{

		tftSettingsTap(&px, &py);							// wait for a tap (no timeout)

		hit = tftSettingsRowAt(py, tftSettingsGroupCount);

		if (hit == 0xFF) return 0xFF;						// footer -> exit
		if (hit < tftSettingsGroupCount) return hit;

	}

}

// parameter list for a group: draw label + current value per row, return the
// chosen row (0-based within the group), or 0xFF to go back
static uint8_t tftSettingsParamScreen(uint8_t group)
{

	uint16_t px, py;
	uint8_t start = pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(group)]);
	uint8_t count = (uint8_t)(pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(group + 1)]) - start);
	uint8_t r, hit;

	tftSettingsFrame(findStr(tftSettingsGroupNames, group), PSTR("Back"));

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

	while (1)
	{

		tftSettingsTap(&px, &py);							// wait for a tap (no timeout)

		hit = tftSettingsRowAt(py, count);

		if (hit == 0xFF) return 0xFF;						// footer -> back
		if (hit < count) return hit;

	}

}

// modal dropdown: list the options with the current one highlighted; return the
// chosen index, or 0xFF on Cancel / idle timeout
static uint8_t tftSettingsChoose(const char * title, const char * options, uint8_t count, uint8_t current)
{

	uint16_t px, py, rowH = tftSettingsRowHeight();
	uint8_t i, hit;

	tftSettingsFrame(title, PSTR("Cancel"));

	for (i = 0; i < count; i++)
	{

		uint16_t y = tftSettingsTitleH + (uint16_t)(i) * rowH;
		uint16_t bg = (i == current) ? tftSettingsSelBG : ILI9341_BLACK;	// highlight the current value

		ILI9341::fillRect(0, y, tftWidth, rowH - 1, bg);
		TFT::setTextColour(tftSettingsRowFG, bg);
		TFT::setCursorPixel(6, y + (rowH - 16) / 2);
		text::stringOut(m8DevTFTidx, findStr(options, i));
		ILI9341::drawLine(0, y + rowH - 1, tftWidth - 1, y + rowH - 1, tftSettingsDivider);

	}

	while (1)
	{

		tftSettingsTap(&px, &py);							// wait for a tap (no timeout)

		hit = tftSettingsRowAt(py, count);

		if (hit == 0xFF) return 0xFF;						// footer -> cancel
		if (hit < count) return hit;

	}

}

// edit one parameter: a boolean/enum parameter uses the dropdown of named options;
// everything else uses the keypad (seeded with the current value, bounded by the
// parameter's bit-width max). both are titled with the parameter's label. on a
// confirmed change the new value is stored with the usual housekeeping.
static void tftSettingsEdit(uint8_t parameterIdx, const char * label)
{

	uint32_t cur, maxValue, newValue;
	const char * choices = tftSettingsChoices(parameterIdx);

	numberEditObj.parameterIdx = parameterIdx;
	parameterEdit::sharedFunctionCall(nesLoadInitial);		// pBuff = current value; reg 2 = value
	cur = str2ull(pBuff);									// current value (also reloads reg 2)
	maxValue = SWEET64::runPrgm(S64_PRGM_PTR(prgmFetchMaximumParamValue), parameterIdx);	// 2^bits - 1

	if (choices) // boolean / enum -> dropdown (option count is the value range, maxValue + 1)
	{

		uint8_t sel = tftSettingsChoose(label, choices, (uint8_t)(maxValue + 1), (uint8_t)(cur));

		if (sel == 0xFF) return;							// cancelled
		newValue = sel;

	}
	else // numeric -> keypad
	{

		if (!keypad::getNumber(&newValue, maxValue, cur, label)) return;	// cancelled

	}

	SWEET64::init64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), newValue);	// reg 2 = new value
	EEPROM::onChange(S64_PRGM_PTR(prgmWriteParameterValue), parameterIdx);			// store + housekeeping

}

// modal settings editor: group menu -> parameter list -> edit, until Exit/timeout
static void tftSettings::run(void)
{

	uint8_t group, row;

	while (1)
	{

		group = tftSettingsGroupScreen();
		if (group == 0xFF) break;

		while (1)
		{

			uint8_t j;

			row = tftSettingsParamScreen(group);
			if (row == 0xFF) break;

			j = pgm_read_byte(&tftSettingsGroupStart[(uint16_t)(group)]) + row;	// flat index of the chosen param
			tftSettingsEdit(pgm_read_byte(&tftSettingsParams[(uint16_t)(j)]), findStr(tftSettingsLabels, j));

		}

	}

	TFT::clearScreen();

}

#endif // defined(useTFToutput) && defined(useTouchScreenInput)
