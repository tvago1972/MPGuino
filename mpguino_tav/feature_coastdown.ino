#ifdef useCoastDownCalculator
/* Coastdown Calculator support section */

static uint8_t coastdown::displayHandler(uint8_t cmd, uint8_t cursorPos)
{



}

static uint8_t coastdown::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuInitialEntryIdx:
			numberEditObj.neStatusMessage = pseStatusMessages;
			retVal = 1;
			break;

		case menuFirstLineOutIdx:
			text::stringOut(devIdxLCD, coastdownTestMenuTitles, cursorPos);
			break;

		case menuSecondLineInitIdx:
			if (cursorPos)
			{

				numberEditObj.parameterIdx = pgm_read_byte(&coastdownTestParamList[(uint16_t)(cursorPos - 1)]);
				parameterEdit::sharedFunctionCall(nesLoadInitial);

			}

		case menuSecondLineFlagIdx: // if the menu cursor line has supplemental information, retVal will be set to 1
			if (cursorPos) retVal = 1;
			break;

		case menuSecondLineOutIdx:
			if (cursorPos)
			{

				text::stringOut(devIdxLCD, pBuff); // output supplementary information
				text::newLine(devIdxLCD); // clear to the end of the line

			}

			break;

		case menuDoSelectionIdx:
			if (cursorPos)
			{

					numberEditObj.callingDisplayIdx = workingDisplayIdx;
					retVal = parameterEditDisplayIdx; // go to parameter edit display index

			}
			else
			{

				retVal = coastdownDisplayIdx;

			}

			break;

		case menuExitIdx:
			retVal = baseMenuDisplayIdx;
			break;

		default:
			break;

	}

	return retVal;

}

void coastdown::goDisplay(void)
{

	const char * msgPtr = 0;

	uint8_t oldSREG;
	uint8_t i;

	if (bitFlags[(uint16_t)(bfCoastdownStatus)] & cdTestFinished) // coastdown test has finished - let's find out why
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		i = bitFlags[(uint16_t)(bfCoastdownStatus)]; // save coastdown flag state
		bitFlags[(uint16_t)(bfCoastdownStatus)] &= ~(cdTestClearFlags);

		SREG = oldSREG; // restore state of interrupt flag

		if (i & cdTestCanceled) // coastdown test has cancelled
		{

			msgPtr = findStr(coastdownMsgs, 5); // signal that test was cancelled

		}
		else // coastdown test finished normally
		{

			msgPtr = findStr(coastdownMsgs, 4); // signal that test finished normally
			// insert coastdown determiniation call here

		}

	}
	else
	{

		if (bitFlags[(uint16_t)(bfCoastdownStatus)] & cdSignalStateChange)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts

			bitFlags[(uint16_t)(bfCoastdownStatus)] &= ~(cdSignalStateChange);
			i = coastdownState; // fetch current coastdown state

			SREG = oldSREG; // restore state of interrupt flag

			msgPtr = findStr(coastdownMsgs, i); // get appropriate coastdown test message pointer

		}

	}

	if (msgPtr) text::statusOut(devIdxLCD, msgPtr);

	if (bitFlags[(uint16_t)(bfCoastdownStatus)] & cdTestActive) // coastdown test is in progress - display changes accordingly
	{

		coastdownCharIdx &= 0x07;
		text::newLine(devIdxLCD);

		mainDisplay::outputFunction(2, (instantIdx << 8) | (tSpeed), 136, 0); // call main screen function display routine

		text::gotoXY(devIdxLCD, 8, 1);
		text::stringOut(devIdxLCD, PSTR(" ACTIVE"));
		text::charOut(devIdxLCD, pgm_read_byte(&coastdownSymbol[(uint16_t)(coastdownCharIdx++)]));

	}
	else
	{

		i = displayCursor[(uint16_t)(coastdownIdx)] + pCoefficientDidx;

		SWEET64::runPrgm(prgmFetchParameterValue, i);
		ull2str(nBuff, 0, tFormatToNumber);

		text::stringOut(devIdxLCD, parmLabels, i); // print parameter name at top left
		text::numberOut(devIdxLCD, 0);

	}

	text::newLine(devIdxLCD);

}

void coastdown::goTrigger(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (bitFlags[(uint16_t)(bfCoastdownStatus)] & cdTestInProgress) // signal that coastdown test is cancelled
	{

		bitFlags[(uint16_t)(bfCoastdownStatus)] &= ~(cdTestClearFlags); // signal that coastdown test is no longer active
		bitFlags[(uint16_t)(bfCoastdownStatus)] |= cdTestCanceled | cdTestFinished | cdSignalStateChange; // signal that coastdown test is cancelled

	}
	else
	{

		bitFlags[(uint16_t)(bfCoastdownStatus)] &= ~(cdTestClearFlags); // signal that coastdown test is no longer active
		bitFlags[(uint16_t)(bfCoastdownStatus)] |= cdTestTriggered; // set coastdown test flags in bfCoastdownStatus register

	}

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // useCoastDownCalculator
