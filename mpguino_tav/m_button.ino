#if defined(useButtonInput)
/* button input support section */

static void button::init(void)
{

#if defined(useLegacyButtons)
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega2560__)
	DIDR2 &= ~((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // enable digital input on port K button pins

	PORTK |= ((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // enable port K button pullup resistors

	PCMSK2 |= ((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // enable port K button interrupts

	lastPINxState = PINK; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DIDR0 &= ~((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // enable digital input on port C button pins

	PORTC |= ((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // enable port C button pullup resistors

	PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // enable port C button interrupts

	lastPINxState = PINC; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

#endif // defined(useLegacyButtons)
#if defined(useTWIbuttons)
#if defined(useInterruptBasedTWI)
	TWI::disableISRactivity(); // disable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)
#if defined(useAdafruitRGBLCDshield)
	MCP23017portExpanderSupport::configureOutputPort((uint16_t)(buttonMask));
#endif // defined(useAdafruitRGBLCDshield)
#if defined(useInterruptBasedTWI)
	TWI::enableISRactivity(); // enable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)

#endif // defined(useTWIbuttons)
#if defined(useAnalogButtons)
	changeBitFlags(timer0Command, 0, t0cEnableAnalogButtons); // enable analog button sampling

#endif // defined(useAnalogButtons)
}

static void button::shutdown(void)
{

#if defined(useLegacyButtons)
	uint8_t oldSREG;

#endif // defined(useLegacyButtons)
#if defined(useAnalogButtons)
	changeBitFlags(timer0Command, t0cEnableAnalogButtons, 0); // disable analog button sampling

#endif // defined(useAnalogButtons)
#if defined(useTWIbuttons)
#if defined(useInterruptBasedTWI)
	TWI::disableISRactivity(); // disable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)
#if defined(useAdafruitRGBLCDshield)
	MCP23017portExpanderSupport::configureOutputPort(0);
#endif // defined(useAdafruitRGBLCDshield)
#if defined(useInterruptBasedTWI)
	TWI::enableISRactivity(); // enable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)

#endif // defined(useTWIbuttons)
#if defined(useLegacyButtons)
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega2560__)
	DIDR2 |= ((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // disable digital input on port K button pins

	PORTK &= ~((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // disable port K button pullup resistors

	PCMSK2 &= ~((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // disable port K button interrupts

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // disable digital input on port C button pins

	PORTC &= ~((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // disable port C button pullup resistors

	PCMSK1 &= ~((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // disable port C button interrupts

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

#endif // defined(useLegacyButtons)
}

#if defined(useAnalogButtons) || defined(useDebugButtonInjection)
static void button::inject(uint8_t buttonValue)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	thisButtonState = buttonValue;
	timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

	SREG = oldSREG; // restore interrupt flag status

}

#endif // defined(useAnalogButtons) || defined(useDebugButtonInjection)
/* LCD screen cursor manipulation section */

static void cursor::screenLevelEntry(const char * str, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	text::statusOut(devLCD, str);

}

static void cursor::screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	text::statusOut(devLCD, str, strIdx);

}

static void cursor::moveAbsolute(uint8_t positionY, uint8_t positionX)
{

	uint8_t displayIdxChange;

	if ((workingDisplayIdx == positionY) || (positionY == 255)) displayIdxChange = 0; // same level
	else
	{

		displayIdxChange = 1;
		workingDisplayIdx = positionY;

	}

	positionY = pgm_read_byte(&displayParameters[(uint16_t)(workingDisplayIdx)].displayPageCount);

	switch (positionX)
	{

		case 255:
			break;

		case 254:
			displayCursor[(uint16_t)(workingDisplayIdx)] = positionX - 1;
			break;

		default:
			if (positionX < positionY) displayCursor[(uint16_t)(workingDisplayIdx)] = positionX;
			break;

	}

	updateAfterMove(displayIdxChange);

}

static void cursor::moveRelative(uint8_t moveY, uint8_t moveX)
{

	uint8_t wrapAroundFlag;
	uint8_t cursorXdirection;
	uint8_t displayIdxChange;
	uint8_t v;
	uint8_t maxVal;
	uint8_t x; // base menu level

	x = (pgm_read_byte(&displayParameters[(uint16_t)(workingDisplayIdx)].baseDisplayIdx)); // base menu level
	displayIdxChange = 0;

	wrapAroundFlag = 0; // initially, signal that no wrap-around occurred

	if (moveX) // if movement within the screen level is called for
	{

		if (moveX & 0x80) cursorXdirection = 0xFF;
		else cursorXdirection = 0x01;

		maxVal = pgm_read_byte(&displayParameters[(uint16_t)(workingDisplayIdx)].displayPageCount); // "horizontal" size

		v = displayCursor[(uint16_t)(workingDisplayIdx)] + cursorXdirection; // perform move
		thisMenuHeight += cursorXdirection;

		if (v == maxVal) // if screen cursor bumped up to screen level size
		{

			v = 0; // wrap around
			wrapAroundFlag = 1; // signal wrap-around occurred

		}

		if (v > maxVal) // if screen cursor bumped down past lower limit
		{

			v = maxVal - 1; // wrap around
			wrapAroundFlag = 1;

		}

		if (wrapAroundFlag) moveY = cursorXdirection; // if wrap-around occurred, signal direction to next screen level

		displayCursor[(uint16_t)(workingDisplayIdx)] = v;

	}

	if (moveY)
	{

		if (moveY & 0x80) moveY = 0xFF;
		else moveY = 0x01;

		maxVal = pgm_read_byte(&displayParameters[(uint16_t)(workingDisplayIdx)].displayGroupCount); // "vertical" size

		v = workingDisplayIdx - x + moveY;

		if (v == maxVal) v = 0;
		if (v > maxVal) v = maxVal - 1;

		v += x;
		if (workingDisplayIdx != v)
		{

			workingDisplayIdx = v;
			displayIdxChange = 1;

		}

		if (wrapAroundFlag) // if wrap-around occurred, look at direction
		{

			if (moveY == 1) v = 0; // if direction was forward, set current cursor to zero
			else v = pgm_read_byte(&displayParameters[(uint16_t)(workingDisplayIdx)].displayPageCount) - 1; // else, set current cursor to end of new menu level

			displayCursor[(uint16_t)(workingDisplayIdx)] = v;

		}

	}

	updateAfterMove(displayIdxChange);

}

static void cursor::updateAfterMove(uint8_t displayIdxChange)
{

	uint8_t cf;

	// call indexed support section cursor update function to update any section-specific data
	if (displayIdxChange) cf = displayInitialEntryIdx;
	else cf = displayCursorUpdateIdx;

	cursor::updateDisplay(workingDisplayIdx, cf);

}

static void cursor::doCommand(void)
{

	const buttonVariable * bpPtr;
	uint8_t bp;
	uint8_t i;

	bp = buttonPress; // capture button state
	bpPtr = (const buttonVariable *)(pgm_read_word(&(displayParameters[(uint16_t)(workingDisplayIdx)].buttonList)));

	while (true)
	{

		i = pgm_read_byte(&(bpPtr->buttonCode));

		if ((bp == i) || (i == buttonsUp)) break;
		else bpPtr++;

	}

	text::gotoXY(devLCD, 0, 0);
	((bdFunc)pgm_read_word(&(bpPtr->buttonCommand)))(); // go perform action

}

static void cursor::noSupport(void)
{

	text::initStatus(devLCD);
	text::stringOut(devLCD, PSTR("Btn 0x"));
	text::hexByteOut(devLCD, buttonPress);
	text::stringOut(devLCD, PSTR(" Pressed"));
	text::commitStatus(devLCD);

}

static void cursor::doNothing(void)
{
}

#if defined(useLCDoutput)
static void cursor::doNextBright(void)
{

	text::statusOut(devLCD, brightMsg, brightString, EEPROM::readByte(pBrightnessIdx));

}

#endif // defined(useLCDoutput)
static void cursor::shortLeft(void)
{

	moveRelative(0, 255); // go to previous option in current level

}

static void cursor::shortRight(void)
{

	moveRelative(0, 1); // go to next option in current level

}

static void cursor::longLeft(void)
{

	moveRelative(255, 0); // go to last option in previous level

}

static void cursor::longRight(void)
{

	moveRelative(1, 0); // go to next option in next level

}

#if LCDcharHeight == 4
static void cursor::transferDisplay(void)
{

	if (pgm_read_byte(&displayParameters[(uint16_t)(workingDisplayIdx)].displayFlags) & dfSplitScreen) // if the current display supports double displays
	{

		EEPROM::writeByte(pBottomDisplayIdx, workingDisplayIdx);
		EEPROM::writeByte(pBottomCursorIdx, displayCursor[(uint16_t)(workingDisplayIdx)]);

	}

}

#endif // LCDcharHeight == 4
static void cursor::updateDisplay(uint8_t thisDisplayIdx, uint8_t cmd)
{

	uint8_t cursorPos;
#if LCDcharHeight == 4
	uint8_t bottomCursorPos;
	uint8_t topFlg;
	uint8_t btmFlg;
	uint8_t outFlg;
	uint8_t lineCount;
#endif // LCDcharHeight == 4

	cursorPos = displayCursor[(uint16_t)(thisDisplayIdx)];

	text::gotoXY(devLCD, 0, 0);

	// call indexed support section screen refresh function
	callingDisplayIdx = thisDisplayIdx;
	if (thisDisplayIdx < displayCountMenu) menu::displayHandler(cmd, cursorPos);
	else ((displayHandlerFunc)pgm_read_word(&displayParameters[(uint16_t)(thisDisplayIdx)].displayHandlerPtr))(cmd, cursorPos);

#if LCDcharHeight == 4
	if (thisDisplayIdx < displayCountMenu) topFlg = dfFullScreen;
	else topFlg = pgm_read_byte(&displayParameters[(uint16_t)(thisDisplayIdx)].displayFlags); // fetch double height bit for top display

	if (topFlg & dfFullScreen) lineCount = 4;
	else
	{

		callingDisplayIdx = EEPROM::readByte(pBottomDisplayIdx);
		bottomCursorPos = EEPROM::readByte(pBottomCursorIdx);
		btmFlg = pgm_read_byte(&displayParameters[(uint16_t)(callingDisplayIdx)].displayFlags); // fetch double height bit for botton display
		lineCount = 2;
		outFlg = 0;

		if (topFlg & dfSplitScreen) outFlg = 1; // if top display supports split screen, then continue checking bottom display

		if (outFlg)
		{

			if ((topFlg & dfUsesCGRAM) && (btmFlg & dfUsesCGRAM)) // check if CRGRAM usage is compatible
			{

				if ((topFlg & dfUsesCGRAMfont) && (btmFlg & dfUsesCGRAMfont)) // if both displays use a predefined CGRAM font
				{

					topFlg &= (dfCGRAMfontMask); // extract the CGRAM font number for top display
					btmFlg &= (dfCGRAMfontMask); // extract the CGRAM font number for bottom display

					if (topFlg != btmFlg) outFlg = 0; // if these CGRAM font numbers are different, do not show bottom display

				}
				else // otherwise, both displays use dynamic CGRAM - check if the top display equals the bottom display
					if ((callingDisplayIdx != thisDisplayIdx) || (cursorPos != bottomCursorPos)) outFlg = 0;

			}

		}

		if (outFlg)
		{

			devLCD.controlFlags |= (odvFlagDoubleHeight);

			text::gotoXY(devLCD, 0, 0);

			// call indexed support section screen refresh function
			((displayHandlerFunc)pgm_read_word(&displayParameters[(uint16_t)(callingDisplayIdx)].displayHandlerPtr))(cmd, bottomCursorPos);

			devLCD.controlFlags &= ~(odvFlagDoubleHeight);
			lineCount += 2;

		}

	}

	while (lineCount < LCDcharHeight)
	{

		text::gotoXY(devLCD, 0, lineCount++);
		text::newLine(devLCD);

	}

#endif // LCDcharHeight == 4
}

// Menu display / screen cursor support section

static uint8_t menu::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t thisMenuTop;
	uint8_t displayLine;
	uint8_t menuLine;
	uint8_t allowOutput;
	uint8_t i;
	uint8_t flg;

	switch (cmd)
	{

		case displayInitialEntryIdx:
			if (workingDisplayIdx < displayCountMenu) menuIdx = workingDisplayIdx; // if a valid menu index was found for the current display index, use it
			else menuIdx = baseMenuDisplayIdx; // instead, use top menu index

			// load pre-defined parameters for the given menu index
			menuLength = pgm_read_byte(&displayParameters[(uint16_t)(menuIdx)].displayPageCount);
			menuTitlesOffset = pgm_read_byte(&displayParameters[(uint16_t)(menuIdx)].displayFlags);
			menuHandlerPtr = (displayHandlerFunc)(pgm_read_word(&displayParameters[(uint16_t)(menuIdx)].displayHandlerPtr));

			if (menuHandlerPtr(menuInitialEntryIdx, cursorPos)) // if the menu cursor is to be reset to 1 upon entry, retVal will be set to 1
			{

				displayCursor[(uint16_t)(workingDisplayIdx)] = 0;
				cursorPos = 0;
				thisMenuHeight = 0;

			}
			else thisMenuHeight = menuHeight[(uint16_t)(menuIdx)];

		case displayCursorUpdateIdx: // menu display window is (menuTop + LCDcharHeight - 1) with wraparound
			displayHeight = LCDcharHeight;

			displayHeight -= menuHandlerPtr(menuSecondLineInitIdx, cursorPos);

			if (displayHeight > menuLength) displayHeight = menuLength;

			if (thisMenuHeight == 255) thisMenuHeight = 0;
			else if (thisMenuHeight >= displayHeight) thisMenuHeight = displayHeight - 1;

			menuHeight[(uint16_t)(menuIdx)] = thisMenuHeight;

			menuTop = cursorPos - thisMenuHeight;

			if (menuTop >= menuLength) menuTop += menuLength;

		case displayOutputIdx:
			displayLine = 0;
			allowOutput = 0;
			i = 0;

			while (displayLine < LCDcharHeight)
			{

				text::gotoXY(devLCD, 0, displayLine);
				menuLine = i + menuTop;

				if (menuLine >= menuLength) menuLine -= menuLength;

				if (menuLine == menuTop) allowOutput++;

				if (allowOutput < 2)
				{

					if (displayHeight > 1) text::charOut(devLCD, ((menuLine == cursorPos) ? '>' : ' ' )); // output caret if more than one element is being displayed

					menuHandlerPtr(menuFirstLineOutIdx, menuLine); // output menu element

					// if supplementary information is to be shown at the cursor position
					if (menuLine == cursorPos)
					{

						flg = menuHandlerPtr(menuSecondLineFlagIdx, menuLine);

						if (flg)
						{

							text::gotoXY(devLCD, 0, ++displayLine);
							menuHandlerPtr(menuSecondLineOutIdx, menuLine);

						}

					}

				}
				else text::newLine(devLCD); // clear the line

				displayLine++;
				i++;

			}

			break;

		default:
			break;

	}

}

static void menu::entry(void)
{

	cursor::moveAbsolute(baseMenuDisplayIdx, 255); // go to the menu display level

}

static void menu::select(void)
{

	uint8_t i;

	i = displayCursor[(uint16_t)(workingDisplayIdx)];

	i = menuHandlerPtr(menuDoSelectionIdx, i); // go perform any supplemental defined command

	cursor::moveAbsolute(i, 255); // go to the menu screen level

}

static void menu::exit(void)
{

	uint8_t i;

	i = menuHandlerPtr(menuExitIdx, 0);

	cursor::moveAbsolute(i, 255); // go to the menu screen level

}

#endif // defined(useButtonInput)
