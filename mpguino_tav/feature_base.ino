/* Main screen section */

uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

#ifdef useScreenEditor
	uint8_t i;
#endif // useScreenEditor
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			topScreenLevel = menuLevel; // save current menu level for sub-function (param edit, trip load/save, etc) support

		case menuCursorUpdateIdx:
			printStatusMessage(mainScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			retVal = (activityFlags & afActivityCheckFlags);
			switch (retVal)
			{

				case (afVehicleStoppedFlag | afButtonFlag):
#ifdef trackIdleEOCdata
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainIdleScreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainIdleScreenFormats, 0);
#endif // useSpiffyTripLabels
					break;

#endif // trackIdleEOCdata
				case (afEngineOffFlag | afButtonFlag):
#ifdef trackIdleEOCdata
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainEOCscreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainEOCscreenFormats, 0);
#endif // useSpiffyTripLabels
					break;

#endif // trackIdleEOCdata
				default:
#ifdef useScreenEditor
					i = cursorPos;
					i <<= 2;
					i += eePtrScreensStart;

#ifdef useSpiffyTripLabels
					for (uint8_t x = 0; x < 4; x++) displayFunction(x, EEPROM::readVal(i++), EEPROM::readVal(i++), 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					for (uint8_t x = 0; x < 4; x++) displayFunction(x, EEPROM::readVal(i++), EEPROM::readVal(i++));
#endif // useSpiffyTripLabels

#else // useScreenEditor
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos);
#endif // useSpiffyTripLabels
#endif // useScreenEditor
					break;

			}
			retVal = 0;
			break;

		default:
			break;

	}

	return retVal;

}

void doReturnToMainScreen(void)
{

	cursor::moveAbsolute(topScreenLevel, 255);

}

#if defined(useScreenEditor)
/* Programmable main display screen edit support section */

void doCursorUpdateScreenEdit(void)
{

	uint8_t b;

	b = screenCursor[(unsigned int)(screenEditIdx)];
	b &= ~screenEditFlag_dirty;

	if ((screenEditDirty & ~screenEditFlag_dirty) ^ b) // if cursor moved to a different screen function altogether
	{

		screenTripValue = EEPROM::readVal(b + eePtrScreensStart);
		screenFunctionValue = EEPROM::readVal(b + eePtrScreensStart + 1);

		if (screenEditDirty & screenEditFlag_dirty) // if previous screen function was changed but not saved
		{

			printStatusMessage(seFormatRevertedString);

		}

		screenEditDirty = b; // save current cursor position of screen function being edited
		screenEditDirty &= ~screenEditFlag_dirty; // mark screen function as not modified

	}

}

void doScreenEditDisplay(void)
{

	uint8_t i;
	uint8_t j;
	uint8_t k;

	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t calcBlink;
	uint8_t tripBlink;

	i = screenCursor[(unsigned int)(screenEditIdx)];

	j = i; // extract whether trip index or trip function is to be edited
	j &= 0x01;

	i >>= 1; // figure out edited screen function placement on screen
	k = i;

	i &= 0xFC; // figure out start of screen page
	k &= 0x03;

	for (uint8_t x = 0; x < 4; x++)
	{

		calcBlink = 0;
		tripBlink = 0;

		if (x == k) // if at
		{

			tripIdx = screenTripValue;
			calcIdx = screenFunctionValue;
			if (j == 1) calcBlink = 170;
			else tripBlink = 170;

		}
		else
		{

			tripIdx = EEPROM::readVal(eePtrScreensStart + (i + x) * 2);
			calcIdx = EEPROM::readVal(eePtrScreensStart + (i + x) * 2 + 1);

		}

#ifdef useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, msTripBitPattern); // call main screen function display routine
#else // useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx); // call main screen function display routine
#endif // useSpiffyTripLabels

	}

}

void doGoScreenEdit(void)
{

	screenEditDirty = screenCursor[(unsigned int)(screenEditIdx)];
	screenEditDirty >>= 1;
	screenEditDirty ^= 0x01; // force update of screen function
	screenEditDirty &= ~screenEditFlag_dirty;

	cursor::moveAbsolute(screenEditIdx, screenCursor[(unsigned int)(mainScreenIdx)] << 3);

	printStatusMessage(PSTR("Screen Editor"));

}

void doScreenEditBump(void)
{

	uint8_t b;

	b = (screenCursor[(unsigned int)(screenEditIdx)] & 0x01); // figure out whether trip variable or trip function is being modified
	screenEditDirty |= screenEditFlag_dirty; // mark current screen function as modified

	if (b) // if trip function is being modified
	{

		screenFunctionValue++;
		if (screenFunctionValue == dfMaxValDisplayCount) screenFunctionValue = 0;

	}
	else // otherwise, trip variable is being modified
	{

		screenTripValue++;
		if (screenTripValue == dfMaxTripCount) screenTripValue = 0;

	}

}

void doSaveScreen(void)
{

	uint8_t b;

	b = screenCursor[(unsigned int)(screenEditIdx)];

	// generate and save new screen function from trip variable and trip function
	EEPROM::writeVal(eePtrScreensStart + b, (unsigned long)(screenTripValue));
	EEPROM::writeVal(eePtrScreensStart + b + 1, (unsigned long)(screenFunctionValue));
	screenEditDirty &= ~screenEditFlag_dirty;

	printStatusMessage(PSTR("Format saved"));

}

void doScreenReturnToMain(void)
{

	const char * str;

	if (screenEditDirty & screenEditFlag_dirty) str = seFormatRevertedString;
	else s = seExitScreenEditString;

	screenCursor[(unsigned int)(mainScreenIdx)] = (screenCursor[(unsigned int)(screenEditIdx)] >> 3);
	cursor::screenLevelEntry(str, topScreenLevel);

}

#endif // defined(useScreenEditor)
