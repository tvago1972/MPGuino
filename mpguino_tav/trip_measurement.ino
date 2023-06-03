#ifdef useEEPROMtripStorage
static uint8_t tripVar::getBaseEEPROMaddress(uint8_t tripIdx, uint8_t dataIdx)
{

	uint8_t retVal;

	if (dataIdx < rvMeasuredCount) switch (tripIdx)
		{

			case EEPROMcurrentIdx:
				retVal = pCurrTripVSSpulseIdx + dataIdx;
				break;

			case EEPROMtankIdx:
				retVal = pTankTripVSSpulseIdx + dataIdx;
				break;

#ifdef trackIdleEOCdata
			case EEPROMeocIdleCurrentIdx:
				retVal = pCurrIEOCvssPulseIdx + dataIdx;
				break;

			case EEPROMeocIdleTankIdx:
				retVal = pTankIEOCvssPulseIdx + dataIdx;
				break;

#endif // trackIdleEOCdata
			default:
				retVal = 0;
				break;

		}
	else retVal = 0;

	return retVal;

}

#endif // useEEPROMtripStorage
static void tripVar::reset(uint8_t tripIdx)
{

	if (tripIdx < tripSlotCount)
	{

		collectedVSSpulseCount[(uint16_t)(tripIdx)] = 0;
		SWEET64::init64byt((union union_64 *)&collectedInjCycleCount[(uint16_t)(tripIdx)], 0);

		if (tripIdx < tripSlotFullCount)
		{

			collectedInjPulseCount[(uint16_t)(tripIdx)] = 0;
			SWEET64::init64byt((union union_64 *)&collectedVSScycleCount[(uint16_t)(tripIdx)], 0);
			SWEET64::init64byt((union union_64 *)&collectedEngCycleCount[(uint16_t)(tripIdx)], 0);

		}

	}

}

static void tripVar::transfer(uint8_t srcTripIdx, uint8_t destTripIdx)
{

	if (destTripIdx < tripSlotCount)
	{

		if (srcTripIdx < tripSlotCount)
		{

			collectedVSSpulseCount[(uint16_t)(destTripIdx)] = collectedVSSpulseCount[(uint16_t)(srcTripIdx)];
			SWEET64::copy64((union union_64 *)&collectedInjCycleCount[(uint16_t)(destTripIdx)], (union union_64 *)&collectedInjCycleCount[(uint16_t)(srcTripIdx)]);

			if (destTripIdx < tripSlotFullCount)
			{

				if (srcTripIdx < tripSlotFullCount)
				{

					collectedInjPulseCount[(uint16_t)(destTripIdx)] = collectedInjPulseCount[(uint16_t)(srcTripIdx)];
					SWEET64::copy64((union union_64 *)&collectedVSScycleCount[(uint16_t)(destTripIdx)], (union union_64 *)&collectedVSScycleCount[(uint16_t)(srcTripIdx)]);
					SWEET64::copy64((union union_64 *)&collectedEngCycleCount[(uint16_t)(destTripIdx)], (union union_64 *)&collectedEngCycleCount[(uint16_t)(srcTripIdx)]);

				}
				else
				{

					collectedInjPulseCount[(uint16_t)(destTripIdx)] = 0;
					SWEET64::init64byt((union union_64 *)&collectedVSScycleCount[(uint16_t)(destTripIdx)], 0);
					SWEET64::init64byt((union union_64 *)&collectedEngCycleCount[(uint16_t)(destTripIdx)], 0);

				}

			}

		}

	}

}

static void tripVar::update(uint8_t srcTripIdx, uint8_t destTripIdx)
{

	if (destTripIdx < tripSlotCount)
	{

		if (srcTripIdx < tripSlotCount)
		{

			tripVar::add32(collectedVSSpulseCount, srcTripIdx, destTripIdx);
			tripVar::add64(collectedInjCycleCount, srcTripIdx, destTripIdx);

			if (destTripIdx < tripSlotFullCount)
			{

				if (srcTripIdx < tripSlotFullCount)
				{

					tripVar::add32(collectedInjPulseCount, srcTripIdx, destTripIdx);
					tripVar::add64(collectedVSScycleCount, srcTripIdx, destTripIdx);
					tripVar::add64(collectedEngCycleCount, srcTripIdx, destTripIdx);

				}

			}

		}

	}

}

static void tripVar::add32(uint32_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"   add %A0, %A1            \n"		// 0
		"   adc %B0, %B1            \n"		// 1
		"   adc %C0, %C1            \n"		// 2
		"   adc %D0, %D1            \n"		// 3
		: "+r" (collectedArray[(uint16_t)(destTripIdx)])
		: "r" (collectedArray[(uint16_t)(srcTripIdx)])
	);
#else // useAssemblyLanguage
	collectedArray[(uint16_t)(destTripIdx)] += collectedArray[(uint16_t)(srcTripIdx)];
#endif // useAssemblyLanguage

}

static void tripVar::add64(uint64_t collectedArray[], uint32_t value, uint8_t destTripIdx)
{

#ifdef useAssemblyLanguage
	union union_64 * an;

	an = (union union_64 *)(&collectedArray[(uint16_t)(destTripIdx)]);

	uint8_t x;

	asm volatile(
		"	ld	__tmp_reg__, %a0	\n"		// 0
		"   add __tmp_reg__, %A2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a0	\n"		// 1
		"   adc __tmp_reg__, %B2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a0	\n"		// 2
		"   adc __tmp_reg__, %C2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a0	\n"		// 3
		"   adc __tmp_reg__, %D2    \n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ldi	%A1, 4				\n"		// initialize counter

		"l_add64a%=:				\n"
		"	ld	__tmp_reg__, %a0	\n"		// 4
		"   adc __tmp_reg__, __zero_reg__    \n"
		"	st	%a0+, __tmp_reg__	\n"
		"	dec	%A1					\n"
		"	brne l_add64a%=			\n"

		: "+e" (an), "+r" (x)
		: "r" (value)

	);
#else // useAssemblyLanguage
	collectedArray[(uint16_t)(destTripIdx)] += value;
#endif // useAssemblyLanguage

}

static void tripVar::add64(uint64_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx)
{

	union union_64 * an = (union union_64 *)(&collectedArray[(uint16_t)(destTripIdx)]);
	union union_64 * ann = (union union_64 *)(&collectedArray[(uint16_t)(srcTripIdx)]);

	uint8_t x;
	uint8_t c;
#ifdef useAssemblyLanguage

	asm volatile(
		"	clc						\n"
		"	ldi	%A2, 8				\n"		// initialize counter

		"l_add64b%=:				\n"
		"	ld	__tmp_reg__, %a0	\n"
		"	ld	%A1, %a3+			\n"
		"	adc	__tmp_reg__, %A1	\n"
		"	st	%a0+, __tmp_reg__	\n"
		"	dec	%A2					\n"
		"	brne l_add64b%=			\n"
		: "+e" (an), "+r" (c), "+r" (x)
		: "e" (ann)
	);
#else // useAssemblyLanguage
	unsigned int enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[0] = c;
		n->u8[1] = 0;
		enn += an->u8[(uint16_t)(x)];
		enn += ann->u8[(uint16_t)(x)];
		an->u8[(uint16_t)(x)] = n->u8[0];
		c = n->u8[1];

	}
#endif // useAssemblyLanguage

}

#ifdef useWindowTripFilter
void windowTripFilterReset(void)
{

	wtpCurrentIdx = windowTripFilterIdx;

	for (uint8_t x = 0; x < windowTripFilterSize; x++) tripVar::reset(windowTripFilterIdx + x);

}

#endif // useWindowTripFilter
static uint8_t translateTripIndex(const uint8_t tripTranslateList[], uint8_t tripListPos)
{

#ifdef useBarFuelEconVsTime
	uint8_t oldSREG;
#endif // useBarFuelEconVsTime
	uint8_t i;
	uint8_t j;

	j = pgm_read_byte(&tripTranslateList[(unsigned int)(tripListPos)]);
	i = j & 0x7F; // strip off upper bit for now, to look at the trip index in question

	switch (i)
	{

		case 0x7F:		// replace generic raw trip index with old raw trip index
			i = rawTripIdx ^ (raw0tripIdx ^ raw1tripIdx);
			break;

#ifdef trackIdleEOCdata
		case 0x7E:		// replace generic idle/eoc raw trip index with old idle/eoc raw trip index
			i = rawEOCidleTripIdx ^ (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx);
			break;

#endif // trackIdleEOCdata
#ifdef useWindowTripFilter
		case 0x7D:		// replace generic window trip index with current window trip index
			i = wtpCurrentIdx;
			break;

#endif // useWindowTripFilter
#ifdef useBarFuelEconVsTime
		case 0x7C:		// replace generic fuel econ vs time trip index with current fuel econ vs time trip index
			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			j = FEvTperiodIdx; // current exchange could either be an update or a transfer
			i = j & 0x7F; // work with below trip index validation mechanism

			FEvTperiodIdx = i; // ensure that future trip variable exchanges are updates
			calcFEvTperiodIdx = i;

			SREG = oldSREG; // restore interrupt flag status
			break;

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
		case 0x7B:	// replace generic fuel econ vs speed trip index with current fuel econ vs speed trip index
			i = FEvSpdTripIdx;
			break;

#endif // useBarFuelEconVsSpeed
		default:
			break;

	}

	if (i < tripSlotCount) i |= (j & 0x80); // restore high bit as it tells whether to update or transfer
	else i = 0; // invalid values get remapped to 0

	return i;

}

/* Trip save/restore/reset support section */

#ifdef useSavedTrips
static uint8_t tripSupport::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;
	uint8_t tripIdx;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
		case menuOutputDisplayIdx:
			tripIdx = pgm_read_byte(&msTripList[(uint16_t)(tripSlot + 1)]);
			text::stringOut(devLCD, tripSaveFuncNames, cursorPos); // print trip function name at top left
			text::stringOut(devLCD, msTripNameString, tripSlot + 1);
			text::newLine(devLCD);

			text::gotoXY(devLCD, 0, 1); // go to next line

			if (EEPROM::readVal(pgm_read_byte(&tripSignatureList[(uint16_t)(tripSlot)])) == guinosig) text::stringOut(devLCD, PSTR("Present"));
			else text::stringOut(devLCD, PSTR("Empty"));

			text::newLine(devLCD);
			break;

		default:
			break;

	}

	return retVal;

}

static void tripSupport::goSaveTank(void)
{

	tripSlot = 1;
	cursor::moveAbsolute(tripSaveScreenIdx, 0);

}

static void tripSupport::goSaveCurrent(void)
{

	tripSlot = 0;
	cursor::moveAbsolute(tripSaveScreenIdx, 0);

}

static void tripSupport::select(void)
{

	switch (screenCursor[(unsigned int)(menuLevel)])
	{

		case 0: // save
			doWriteTrip(tripSlot);
			displayStatus(tripSlot, PSTR("Save"));
			doReturnToMainScreen();
			break;

		case 1: // load
			doReadTrip(tripSlot);
			displayStatus(tripSlot, PSTR("Load"));
			doReturnToMainScreen();
			break;

		case 2: // reset
			doResetTrip(tripSlot);
			displayStatus(tripSlot, PSTR("Reset"));
			doReturnToMainScreen();
			break;

		default:
			break;

	}

}

static uint8_t tripSupport::doAutoAction(uint8_t taaMode)
{

	uint8_t retVal = 0;

	if (EEPROM::readVal(pAutoSaveActiveIdx))
	{

		for (uint8_t x = 0; x < 2; x++)
			if (taaMode) retVal |= doReadTrip(x);
			else retVal |= doWriteTrip(x);

	}

	return retVal;

}

static uint8_t tripSupport::doReadTrip(uint8_t tripSlot)
{
	uint8_t retVal;

	retVal = 0;

	if (tripSlot) retVal += SWEET64::runPrgm(prgmLoadTankFromEEPROM, 0);
	else retVal += SWEET64::runPrgm(prgmLoadCurrentFromEEPROM, 0);

	return retVal;

}

static uint8_t tripSupport::doWriteTrip(uint8_t tripSlot)
{

	metricFlag &= ~(EEPROMbulkChangeFlag);

	if (tripSlot) SWEET64::runPrgm(prgmSaveTankToEEPROM, 0);
	else SWEET64::runPrgm(prgmSaveCurrentToEEPROM, 0);

	return (metricFlag & EEPROMbulkChangeFlag);

}

#endif // useSavedTrips
static void tripSupport::displayStatus(uint8_t tripSlot, const char * str)
{

	initStatusLine();
	text::stringOut(devLCD, msTripNameString, tripSlot + 1);
	text::stringOut(devLCD, PSTR(" Trip "));
	text::stringOut(devLCD, str);
	execStatusLine();

}

static void tripSupport::doResetTrip(uint8_t tripSlot)
{

#ifdef useSavedTrips
	doWriteTrip(tripSlot);

#endif // useSavedTrips
	tripVar::reset(pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot)]));
#ifdef trackIdleEOCdata
	tripVar::reset(pgm_read_byte(&tripSelectList[(unsigned int)(tripSlot + 2)]));
#endif // trackIdleEOCdata

#if useBarFuelEconVsSpeed || usePartialRefuel
	if (tripSlot)
	{

#ifdef useBarFuelEconVsSpeed
		bgFEvsSsupport::reset();

#endif // useBarFuelEconVsSpeed
#ifdef usePartialRefuel
		EEPROM::writeVal(pRefuelSizeIdx, 0);

#endif // usePartialRefuel
	}

#endif // useBarFuelEconVsSpeed || usePartialRefuel
}

static void tripSupport::resetTank(void)
{

	doResetTrip(1);
	displayStatus(1, PSTR("Reset"));

}

static void tripSupport::resetCurrent(void)
{

	doResetTrip(0);
	displayStatus(0, PSTR("Reset"));

}

#ifdef useChryslerMAPCorrection
/* Chrysler returnless fuel pressure correction display section */

const uint8_t pressureCorrectScreenFormats[4][2] PROGMEM = {
	 {(mpMAPpressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}		// Pressures
	,{(mpBaroPressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}
	,{(mpFuelPressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}
	,{(mpInjPressureIdx - mpMAPpressureIdx),	0x80 | tPressureChannel}
};

static uint8_t pressureCorrect::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			printStatusMessage(pressureCorrectScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
#ifdef useSpiffyTripLabels
			displayMainScreenFunctions(pressureCorrectScreenFormats, cursorPos, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
			displayMainScreenFunctions(pressureCorrectScreenFormats, cursorPos);
#endif // useSpiffyTripLabels
			break;

		default:
			break;

	}

	return retVal;

}

#endif // useChryslerMAPCorrection
