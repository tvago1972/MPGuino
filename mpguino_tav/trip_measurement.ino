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

#if defined(trackIdleEOCdata)
			case EEPROMeocIdleCurrentIdx:
				retVal = pCurrIEOCvssPulseIdx + dataIdx;
				break;

			case EEPROMeocIdleTankIdx:
				retVal = pTankIEOCvssPulseIdx + dataIdx;
				break;

#endif // defined(trackIdleEOCdata)
			default:
				retVal = 0;
				break;

		}
	else retVal = 0;

	return retVal;

}

#endif // useEEPROMtripStorage
/* Trip save/restore/reset support section */

static void tripSupport::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	curRawTripIdx = raw0tripIdx;
#if defined(trackIdleEOCdata)
	curRawEOCidleTripIdx = raw0eocIdleTripIdx;
#endif // defined(trackIdleEOCdata)

	for (uint8_t x = 0; x < tripSlotCount; x++) tripVar::reset(x);

	SREG = oldSREG; // restore interrupt flag status

}

static void tripSupport::idleProcess(void)
{

	uint8_t oldSREG;
	uint8_t k;
	uint8_t m;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	curRawTripIdx ^= (raw0tripIdx ^ raw1tripIdx); // set new raw trip variable index
#if defined(trackIdleEOCdata)
	curRawEOCidleTripIdx ^= (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set new raw EOC/idle trip variable index
#endif // defined(trackIdleEOCdata)

	SREG = oldSREG; // restore interrupt flag status

	for (uint8_t x = 0; x < tripUpdateListSize; x++)
	{

		k = translateTripIndex(x, 0);
		m = translateTripIndex(x, 1);

		if (m & 0x80) // if transfer bit set
		{

			tripVar::transfer(k, m & 0x7F); // if transfer bit set, do trip transfer
			tripVar::reset(k); // reset source trip variable

		}
		else tripVar::update(k, m); // otherwise, just do trip update

	}

#if defined(useWindowTripFilter)
	if (awakeFlags & aAwakeOnVehicle)
	{

		wtpCurrentIdx++;
		if (wtpCurrentIdx == windowTripFilterIdx + windowTripFilterSize) wtpCurrentIdx = windowTripFilterIdx;

	}

#endif // defined(useWindowTripFilter)
}

static uint8_t tripSupport::translateTripIndex(uint8_t tripTransferIdx, uint8_t tripDirIndex)
{

#ifdef useBarFuelEconVsTime
	uint8_t oldSREG;
#endif // useBarFuelEconVsTime
	uint8_t i;
	uint8_t j;

	j = pgm_read_byte(&tripUpdateList[(unsigned int)(tripTransferIdx)][(unsigned int)(tripDirIndex)]);
	i = j & 0x7F; // strip off upper bit for now, to look at the trip index in question

	switch (i)
	{

		case 0x7F:		// replace generic raw trip index with old raw trip index
			i = curRawTripIdx ^ (raw0tripIdx ^ raw1tripIdx);
			break;

#if defined(trackIdleEOCdata)
		case 0x7E:		// replace generic idle/eoc raw trip index with old idle/eoc raw trip index
			i = curRawEOCidleTripIdx ^ (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx);
			break;

#endif // defined(trackIdleEOCdata)
#if defined(useWindowTripFilter)
		case 0x7D:		// replace generic window trip index with current window trip index
			i = wtpCurrentIdx;
			break;

#endif // defined(useWindowTripFilter)
#ifdef useBarFuelEconVsTime
		case 0x7C:		// replace generic fuel econ vs time trip index with current fuel econ vs time trip index
			i = FEvTperiodIdx;
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

#if defined(useEnhancedTripReset)
static uint8_t tripSave::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;
	uint8_t thisCursorPos;
	uint8_t i;

	thisCursorPos = cursorPos + thisMenuData.menuTitlesOffset;

	switch (cmd)
	{

		case menuInitialEntryIdx:
#if defined(usePartialRefuel)
			numberEditObj.neStatusMessage = prStatusMessages;
#endif //  defined(usePartialRefuel)
			retVal = 1;
			break;

		case menuCursorUpdateIdx:
#if defined(usePartialRefuel)
			if (thisCursorPos == tsfAddPartialIdx)
			{

				numberEditObj.parameterIdx = pRefuelSizeIdx;
				parameterEdit::sharedFunctionCall(nesLoadInitial);

			}

#endif //  defined(usePartialRefuel)
		case menuDisplayStatusIdx: // if the menu cursor line has supplemental information, retVal will be set to 1
#if defined(usePartialRefuel)
			if (thisCursorPos == tsfAddPartialIdx) retVal = 1;

#endif //  defined(usePartialRefuel)
#if defined(useSavedTrips)
			if ((thisCursorPos == tsfCurrentLoadIdx) || (thisCursorPos == tsfTankLoadIdx)) retVal = 1;

#endif //  defined(useSavedTrips)
			break;

		case menuDisplayOutputIdx:
#if defined(usePartialRefuel)
			if (thisCursorPos == tsfAddPartialIdx)
			{

				text::stringOut(devLCD, pBuff); // output supplementary information
				text::newLine(devLCD); // clear to the end of the line

			}

#endif //  defined(usePartialRefuel)
#if defined(useSavedTrips)
			if ((thisCursorPos == tsfCurrentLoadIdx) || (thisCursorPos == tsfTankLoadIdx))
				text::stringOutIf(devLCD, (EEPROM::readByte(pgm_read_byte(&tripSignatureList[(uint16_t)(thisTripSlot)])) == guinosig), tripSlotStatus);

#endif //  defined(useSavedTrips)
			break;

		case menuDoSelectionIdx:
			retVal = 255; // default case - don't change display index

			switch (thisCursorPos)
			{

#if defined(usePartialRefuel)
				case tsfAddPartialIdx:
					numberEditObj.callingDisplayIdx = thisMenuData.displayIdx;
					retVal = parameterEditDisplayIdx; // go to parameter edit display index
					break;

				case tsfZeroPartialIdx:
					SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0); // initialize 64-bit number to zero
					parameterEdit::sharedFunctionCall(nesSaveParameter);
					text::statusOut(devLCD, PSTR("PartialFuel RST"));
					break;

#endif //  defined(usePartialRefuel)
#if defined(useSavedTrips)
				case tsfCurrentSaveIdx:
				case tsfTankSaveIdx:
					doWriteTrip(thisTripSlot);
					text::statusOut(devLCD, tripFormatReverseNames, thisTripSlot + 1, PSTR(" Trip Saved"));
					break;

				case tsfCurrentLoadIdx:
				case tsfTankLoadIdx:
					i = doReadTrip(thisTripSlot);
					if (i) text::statusOut(devLCD, tripFormatReverseNames, thisTripSlot + 1, PSTR(" Trip Loaded"));
					else text::statusOut(devLCD, PSTR("Nothing to load"));
					break;

				case tsfCurrentResetIdx: // current trip reset
#endif // defined(useSavedTrips)
				case tsfTankResetIdx: // tank trip reset
					tripSupport::doResetTrip(thisTripSlot);
					break;

				default:
					break;

			}

			break;

		case menuExitIdx:
			retVal = topScreenLevel;
			break;

		default:
			break;

	}

	return retVal;

}

static void tripSave::goSaveTank(void)
{

	thisTripSlot = 1;
	topScreenLevel = thisMenuData.displayIdx; // save calling display index
	cursor::moveAbsolute(tripSaveTankDisplayIdx, 0);

}

static void tripSave::goSaveCurrent(void)
{

	thisTripSlot = 0;
	topScreenLevel = thisMenuData.displayIdx; // save calling display index
	cursor::moveAbsolute(tripSaveCurrentDisplayIdx, 0);

}

#if defined(useSavedTrips)
static uint8_t tripSave::doAutoAction(uint8_t taaMode)
{

	uint8_t retVal = 0;

	if (EEPROM::readByte(pAutoSaveActiveIdx))
	{

		for (uint8_t x = 0; x < 2; x++)
			if (taaMode) retVal |= doReadTrip(x);
			else retVal |= doWriteTrip(x);

	}

	return retVal;

}

static uint8_t tripSave::doReadTrip(uint8_t tripSlot)
{
	uint8_t retVal;

	retVal = 0;

	if (tripSlot) retVal += SWEET64::runPrgm(prgmLoadTankFromEEPROM, 0);
	else retVal += SWEET64::runPrgm(prgmLoadCurrentFromEEPROM, 0);

	return retVal;

}

static uint8_t tripSave::doWriteTrip(uint8_t tripSlot)
{

	metricFlag &= ~(EEPROMbulkChangeFlag);

	if (tripSlot) SWEET64::runPrgm(prgmSaveTankToEEPROM, 0);
	else SWEET64::runPrgm(prgmSaveCurrentToEEPROM, 0);

	return (metricFlag & EEPROMbulkChangeFlag);

}

#endif // defined(useSavedTrips)
#endif // defined(useEnhancedTripReset)
static void tripSupport::resetCurrent(void)
{

	doResetTrip(0);

}

static void tripSupport::resetTank(void)
{

	doResetTrip(1);

}

static void tripSupport::doResetTrip(uint8_t tripSlot)
{

	tripVar::reset(pgm_read_byte(&tripSelectList[(uint16_t)(tripSlot)]));
#if defined(trackIdleEOCdata)
	tripVar::reset(pgm_read_byte(&tripSelectList[(uint16_t)(tripSlot + 2)]));
#endif // defined(trackIdleEOCdata)

#if defined(useBarFuelEconVsSpeed) || defined(usePartialRefuel)
	if (tripSlot)
	{

#if defined(useBarFuelEconVsSpeed)
		bgFEvsSsupport::reset();

#endif // defined(useBarFuelEconVsSpeed)
#if defined(usePartialRefuel)
		EEPROM::writeVal(pRefuelSizeIdx, 0);

#endif // defined(usePartialRefuel)
	}

#endif // defined(useBarFuelEconVsSpeed) || defined(usePartialRefuel)
	text::statusOut(devLCD, tripFormatReverseNames, tripSlot + 1, PSTR(" Trip Reset"));

}

#if defined(useWindowTripFilter)
static void tripSupport::resetWindowFilter(void)
{

	wtpCurrentIdx = windowTripFilterIdx;

	for (uint8_t x = 0; x < windowTripFilterSize; x++) tripVar::reset(windowTripFilterIdx + x);

}

#endif // defined(useWindowTripFilter)
#ifdef useChryslerMAPCorrection
/* Chrysler returnless fuel pressure correction display section */

static const uint8_t prgmCalculateMAPpressure[] PROGMEM = {
	instrLdRegVoltage, 0x02, analogMAPchannelIdx,		// load analog channel ADC step value
	instrSubMainFromX, 0x02, mpAnalogMAPfloorIdx,		// is reading below MAP sensor voltage floor?
	instrBranchIfLT, 3,									// if not, continue
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//cont1:
	instrMul2byMain, mpAnalogMAPnumerIdx,				// perform conversion to get pressure units per volts value
	instrDiv2byMain, mpAnalogMAPdenomIdx,				// divide by pressure units per volts value
	instrAddEEPROMtoX, 0x02, pMAPsensorOffsetIdx,		// add pressure offset value from EEPROM
	instrStRegMain, 0x02, mpMAPpressureIdx,				// store resulting MAP sensor reading
#ifdef useChryslerBaroSensor
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateBaroPressure[] PROGMEM = {
	instrLdRegVoltage, 0x02, analogBaroChannelIdx,		// load analog channel ADC step value
	instrSubMainFromX, 0x02, mpAnalogBaroFloorIdx,		// is reading below barometric sensor voltage floor?
	instrBranchIfLT, 3,									// if not, continue
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//cont1:
	instrMul2byMain, mpAnalogBaroNumerIdx,				// convert to obtain pressure units per volts value
	instrDiv2byMain, mpAnalogBaroDenomIdx,				// divide by pressure units per volts value
	instrAddEEPROMtoX, 0x02, pBaroSensorOffsetIdx,		// add pressure offset value from EEPROM
	instrStRegMain, 0x02, mpBaroPressureIdx,			// store resulting barometric sensor reading
#endif // useChryslerBaroSensor
	instrLdRegMain, 0x02, mpFuelPressureIdx,			// get fuel system differential pressure
	instrAddMainToX, 0x02, mpBaroPressureIdx,			// add to reference barometric pressure to get fuel system absolute pressure
	instrSubMainFromX, 0x02, mpMAPpressureIdx,			// subtract MAP to get differential pressure across the fuel injector
	instrStRegMain, 0x02, mpInjPressureIdx,				// store differential pressure across the fuel injector
	instrMul2byConst, idxCorrectionFactor2,				// set up for iSqrt
	instrDiv2byMain, mpFuelPressureIdx,					// divide by the fuel system differential pressure
	instrTestReg, 0x02,									// test whether overflow occurred
	instrBranchIfOverflow, 6,							// if overflow occurred, go handle it
	instrIsqrt, 0x02,									// perform square root on result
	instrStRegVolatile, 0x02, vInjectorCorrectionIdx,	// save square root of presssure differential ratio as fuel injector correction factor
	instrDone,											// return to caller

//cont3:
	instrLdRegConst, 0x02, idxCorrectionFactor,
	instrStRegVolatile, 0x02, vInjectorCorrectionIdx,	// save initial injector correction index for pressure differential calculation
	instrDone											// return to caller
};

static void pressureCorrect::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(devLCD, pressureCorrectDisplayTitles, cursorPos); // briefly display display name

		case displayOutputIdx:
			mainDisplay::outputPage(getPressureCorrectPageFormats, cursorPos, 136, 0);
			break;

		default:
			break;

	}

}

static uint16_t pressureCorrect::getPressureCorrectPageFormats(uint8_t formatIdx)
{

	return pgm_read_word(&pressureCorrectPageFormats[(uint16_t)(formatIdx)]);

}

#endif // useChryslerMAPCorrection
