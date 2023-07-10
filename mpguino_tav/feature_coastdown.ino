#ifdef useCoastDownCalculator // Coastdown Calculator support section
uint8_t coastdownCharIdx;

const char coastdownMsgs[] PROGMEM = {
	"Coastdown Active\r"
	"CD Sample 1 read\r"
	"CD Sample 2 read\r"
	"CD Sample 3 read\r"
	"Coastdown Done\r"
	"Coastdown Canx\r"
};

const char coastdownSymbol[] PROGMEM = "|}>}|{<{";

void coastdown::goDisplay(void)
{

	const char * msgPtr = 0;

	uint8_t oldSREG;
	uint8_t i;

	if (coastdownFlags & cdtFinished) // coastdown test has finished - let's find out why
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts

		i = coastdownFlags; // save coastdown flag state
		coastdownFlags &= ~(cdtTestClearFlags);

		SREG = oldSREG; // restore state of interrupt flag

		if (i & cdtCancelled) // coastdown test has cancelled
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

		if (coastdownFlags & cdSignalStateChange)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts

			coastdownFlags &= ~(cdSignalStateChange);
			i = coastdownState; // fetch current coastdown state

			SREG = oldSREG; // restore state of interrupt flag

			msgPtr = findStr(coastdownMsgs, i); // get appropriate coastdown test message pointer

		}

	}

	if (msgPtr) text::statusOut(devLCD, msgPtr);

	if (coastdownFlags & cdtActive) // coastdown test is in progress - display changes accordingly
	{

		coastdownCharIdx &= 0x07;
		text::newLine(devLCD);

		mainDisplay::outputFunction(2, (instantIdx << 8) | (tSpeed), 136, 0); // call main screen function display routine

		text::gotoXY(devLCD, 8, 1);
		text::stringOut(devLCD, PSTR(" ACTIVE"));
		text::charOut(devLCD, pgm_read_byte(&coastdownSymbol[(unsigned int)(coastdownCharIdx++)]));

	}
	else
	{

		i = displayCursor[(unsigned int)(coastdownIdx)] + pCoefficientDidx;

		SWEET64::runPrgm(prgmFetchParameterValue, i);
		ull2str(pBuff, 0, tFormatToNumber);

		text::stringOut(devLCD, parmLabels, i); // print parameter name at top left
		text::stringOut(devLCD, pBuff);

	}

	text::newLine(devLCD);

}

void coastdown::goTrigger(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (coastdownFlags & cdtTestInProgress) // signal that coastdown test is cancelled
	{

		coastdownFlags &= ~(cdtTestClearFlags); // signal that coastdown test is no longer active
		coastdownFlags |= cdtCancelled | cdtFinished | cdSignalStateChange; // signal that coastdown test is cancelled

	}
	else
	{

		coastdownFlags &= ~(cdtTestClearFlags); // signal that coastdown test is no longer active
		coastdownFlags |= cdtTriggered; // set coastdown test flags in coastdownFlags register

	}

	coastdownState = 0; // reset coastdown state

	SREG = oldSREG; // restore state of interrupt flag

}

#endif // useCoastDownCalculator
