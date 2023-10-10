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
			text::stringOut(m8DevLCDidx, coastdownTestMenuTitles, cursorPos);
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

				text::stringOut(m8DevLCDidx, pBuff); // output supplementary information
				text::newLine(m8DevLCDidx); // clear to the end of the line

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

	if (volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] & cdTestFinished) // coastdown test has finished - let's find out why
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		i = volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)]; // save coastdown flag state
		volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] &= ~(cdTestClearFlags);

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

		if (volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] & cdSignalStateChange)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts

			volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] &= ~(cdSignalStateChange);
			i = coastdownState; // fetch current coastdown state

			SREG = oldSREG; // restore state of interrupt flag

			msgPtr = findStr(coastdownMsgs, i); // get appropriate coastdown test message pointer

		}

	}

	if (msgPtr) text::statusOut(m8DevLCDidx, msgPtr);

	if (volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] & cdTestActive) // coastdown test is in progress - display changes accordingly
	{

		coastdownCharIdx &= 0x07;
		text::newLine(m8DevLCDidx);

		mainDisplay::outputFunction(2, (instantIdx << 8) | (tSpeed), 136, 0); // call main screen function display routine

		text::gotoXY(m8DevLCDidx, 8, 1);
		text::stringOut(m8DevLCDidx, PSTR(" ACTIVE"));
		text::charOut(m8DevLCDidx, pgm_read_byte(&coastdownSymbol[(uint16_t)(coastdownCharIdx++)]));

	}
	else
	{

		i = displayCursor[(uint16_t)(coastdownIdx)] + pCoefficientDidx;

		SWEET64::runPrgm(prgmFetchParameterValue, i);
		ull2str(nBuff, 0, tFormatToNumber);

		text::stringOut(m8DevLCDidx, parmLabels, i); // print parameter name at top left
		text::numberOut(m8DevLCDidx, 0);

	}

	text::newLine(m8DevLCDidx);

}

void coastdown::goTrigger(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] & cdTestInProgress) // signal that coastdown test is cancelled
	{

		volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] &= ~(cdTestClearFlags); // signal that coastdown test is no longer active
		volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] |= cdTestCanceled | cdTestFinished | cdSignalStateChange; // signal that coastdown test is cancelled

	}
	else
	{

		volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] &= ~(cdTestClearFlags); // signal that coastdown test is no longer active
		volatile8Variables[(uint16_t)(v8CoastdownStatusIdx - v8VariableStartIdx)] |= cdTestTriggered; // set coastdown test flags in v8CoastdownStatusIdx register

	}

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // useCoastDownCalculator
