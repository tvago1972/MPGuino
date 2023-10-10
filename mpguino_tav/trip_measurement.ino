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

static void tripVar::update64(uint64_t collectedCycleArray[], uint32_t value, uint8_t destTripIdx)
{

#if defined(useAssemblyLanguage)
	union union_64 * an;

	an = (union union_64 *)(&collectedCycleArray[(uint16_t)(destTripIdx)]);

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
#else // defined(useAssemblyLanguage)
	collectedCycleArray[(uint16_t)(destTripIdx)] += value;
#endif // defined(useAssemblyLanguage)

}

static void tripVar::update64(uint64_t collectedCycleArray[], uint32_t collectedPulseArray[], uint32_t value, uint8_t destTripIdx)
{

#if defined(useAssemblyLanguage)
	union union_64 * an;

	an = (union union_64 *)(&collectedCycleArray[(uint16_t)(destTripIdx)]);

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
#else // defined(useAssemblyLanguage)
	update64(collectedCycleArray, value, destTripIdx);
#endif // defined(useAssemblyLanguage)

	collectedPulseArray[(uint16_t)(destTripIdx)]++;

}

static void tripVar::add32(uint32_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx)
{

#if defined(useAssemblyLanguage)
	asm volatile(
		"   add %A0, %A1            \n"		// 0
		"   adc %B0, %B1            \n"		// 1
		"   adc %C0, %C1            \n"		// 2
		"   adc %D0, %D1            \n"		// 3
		: "+r" (collectedArray[(uint16_t)(destTripIdx)])
		: "r" (collectedArray[(uint16_t)(srcTripIdx)])
	);
#else // defined(useAssemblyLanguage)
	collectedArray[(uint16_t)(destTripIdx)] += collectedArray[(uint16_t)(srcTripIdx)];
#endif // defined(useAssemblyLanguage)

}

static void tripVar::add64(uint64_t collectedArray[], uint8_t srcTripIdx, uint8_t destTripIdx)
{

	union union_64 * an = (union union_64 *)(&collectedArray[(uint16_t)(destTripIdx)]);
	union union_64 * ann = (union union_64 *)(&collectedArray[(uint16_t)(srcTripIdx)]);

	uint8_t x;
	uint8_t c;
#if defined(useAssemblyLanguage)

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
#else // defined(useAssemblyLanguage)
	uint16_t enn;
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
#endif // defined(useAssemblyLanguage)

}

#if defined(useEEPROMtripStorage)
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

#endif // defined(useEEPROMtripStorage)
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

	SREG = oldSREG; // restore interrupt flag status

	for (uint8_t x = 0; x < tripSlotCount; x++) tripVar::reset(x);

#if defined(useWindowTripFilter)
	mainProgram8Variables[(uint16_t)(m8CurrentWindowTripIdx - m8VariableStartIdx)] = windowTripFilterSize;

#endif // defined(useWindowTripFilter)
}

static uint8_t tripSupport::translateTripIndex(uint8_t tripTransferIdx, uint8_t tripDirIndex)
{

	uint8_t i;
	uint8_t j;

	j = pgm_read_byte(&tripUpdateList[(uint16_t)(tripTransferIdx)][(uint16_t)(tripDirIndex)]);
	i = j & 0x7F; // strip off transfer/update bit for now, to look at the trip index in question

	switch (i)
	{

		case 0x7F:		// replace generic raw trip index with old raw trip index
			i = oldRawTripIdx;
			break;

#if defined(trackIdleEOCdata)
		case 0x7E:		// replace generic idle/eoc raw trip index with old idle/eoc raw trip index
			i = oldRawEOCidleTripIdx;
			break;

#endif // defined(trackIdleEOCdata)
#if defined(useWindowTripFilter)
		case 0x7D:		// replace generic window trip index with current window trip index
			i = mainProgram8Variables[(uint16_t)(m8CurrentWindowTripIdx - m8VariableStartIdx)] + windowTripFilterIdx - 1;

			if ((--mainProgram8Variables[(uint16_t)(m8CurrentWindowTripIdx - m8VariableStartIdx)]) == 0)
				mainProgram8Variables[(uint16_t)(m8CurrentWindowTripIdx - m8VariableStartIdx)] = windowTripFilterSize;

			break;

#endif // defined(useWindowTripFilter)
#if defined(useBarFuelEconVsTime)
		case 0x7C:		// replace generic fuel econ vs time trip index with current fuel econ vs time trip index
			i = bgFEvsTsupport::getFEvTimeIdx();
			break;

#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
		case 0x7B:	// replace generic fuel econ vs speed trip index with current fuel econ vs speed trip index
			SWEET64::runPrgm(prgmFEvsSpeed, instantIdx);
			i = mainProgram8Variables[(uint16_t)(m8FEvSpeedTripIdx - m8VariableStartIdx)];
			break;

#endif // defined(useBarFuelEconVsSpeed)
		default:
			break;

	}

	if (tripDirIndex) i |= (j & 0x80); // restore transfer/update bit if this is the destination trip index

	return i;

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
		EEPROM::writeByte(pRefuelSizeIdx, 0); // since we're zeroing out pRefuelSizeIdx, we can use writeByte instead of writeVal

#endif // defined(usePartialRefuel)
	}

#endif // defined(useBarFuelEconVsSpeed) || defined(usePartialRefuel)
}

#if defined(useEnhancedTripReset)
#if defined(useButtonInput)
static uint8_t tripSave::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;
	uint8_t thisCursorPos;
	uint8_t i;

	thisCursorPos = cursorPos + menuTitlesOffset;

	switch (cmd)
	{

		case menuInitialEntryIdx:
#if defined(usePartialRefuel)
			numberEditObj.neStatusMessage = prStatusMessages;
#endif //  defined(usePartialRefuel)
			retVal = 1;
			break;

		case menuFirstLineOutIdx:
			text::stringOut(m8DevLCDidx, tripSaveMenuTitles, cursorPos + menuTitlesOffset);
			break;

		case menuSecondLineInitIdx:
#if defined(usePartialRefuel)
			if (thisCursorPos == tsfAddPartialIdx)
			{

				numberEditObj.parameterIdx = pRefuelSizeIdx;
				parameterEdit::sharedFunctionCall(nesLoadInitial);

			}

#endif //  defined(usePartialRefuel)
		case menuSecondLineFlagIdx: // if the menu cursor line has supplemental information, retVal will be set to 1
#if defined(usePartialRefuel)
			if (thisCursorPos == tsfAddPartialIdx) retVal = 1;

#endif //  defined(usePartialRefuel)
#if defined(useSavedTrips)
			if ((thisCursorPos == tsfCurrentLoadIdx) || (thisCursorPos == tsfTankLoadIdx)) retVal = 1;

#endif //  defined(useSavedTrips)
			break;

		case menuSecondLineOutIdx:
#if defined(usePartialRefuel)
			if (thisCursorPos == tsfAddPartialIdx)
			{

				text::stringOut(m8DevLCDidx, pBuff); // output supplementary information
				text::newLine(m8DevLCDidx); // clear to the end of the line

			}

#endif //  defined(usePartialRefuel)
#if defined(useSavedTrips)
			if ((thisCursorPos == tsfCurrentLoadIdx) || (thisCursorPos == tsfTankLoadIdx))
				text::stringOutIf(m8DevLCDidx, (EEPROM::readByte(pgm_read_byte(&tripSignatureList[(uint16_t)(thisTripSlot)])) == guinosig), tripSlotStatus);

#endif //  defined(useSavedTrips)
			break;

		case menuDoSelectionIdx:
			retVal = 255; // default case - don't change display index

			switch (thisCursorPos)
			{

#if defined(usePartialRefuel)
				case tsfAddPartialIdx:
					numberEditObj.callingDisplayIdx = workingDisplayIdx;
					retVal = parameterEditDisplayIdx; // go to parameter edit display index
					break;

				case tsfZeroPartialIdx:
					SWEET64::init64byt((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), 0); // initialize 64-bit number to zero
					EEPROM::onChange(prgmWriteParameterValue, numberEditObj.parameterIdx);
					text::statusOut(m8DevLCDidx, PSTR("PartialFuel RST"));
					break;

#endif //  defined(usePartialRefuel)
#if defined(useSavedTrips)
				case tsfCurrentSaveIdx:
				case tsfTankSaveIdx:
					doWriteTrip(thisTripSlot);
					text::statusOut(m8DevLCDidx, tripFormatReverseNames, thisTripSlot + 1, PSTR(" Trip Saved"));
					break;

				case tsfCurrentLoadIdx:
				case tsfTankLoadIdx:
					i = doReadTrip(thisTripSlot);
					if (i) text::statusOut(m8DevLCDidx, tripFormatReverseNames, thisTripSlot + 1, PSTR(" Trip Loaded"));
					else text::statusOut(m8DevLCDidx, PSTR("Nothing to load"));
					break;

				case tsfCurrentResetIdx: // current trip reset
#endif // defined(useSavedTrips)
				case tsfTankResetIdx: // tank trip reset
					tripSupport::doResetTrip(thisTripSlot);
					tripSupport::outputResetStatus(thisTripSlot);
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
	topScreenLevel = workingDisplayIdx; // save calling display index
	cursor::moveAbsolute(tripSaveTankDisplayIdx, 0);

}

#if defined(useSavedTrips)
static void tripSave::goSaveCurrent(void)
{

	thisTripSlot = 0;
	topScreenLevel = workingDisplayIdx; // save calling display index
	cursor::moveAbsolute(tripSaveCurrentDisplayIdx, 0);

}

#endif // defined(useSavedTrips)
#endif // defined(useButtonInput)
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

	mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] &= ~(ecsEEPROMchangeDetected);

	if (tripSlot) SWEET64::runPrgm(prgmSaveTankToEEPROM, 0);
	else SWEET64::runPrgm(prgmSaveCurrentToEEPROM, 0);

	return (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsEEPROMchangeDetected);

}

#endif // defined(useSavedTrips)
#endif // defined(useEnhancedTripReset)
#if defined(useButtonInput)
static void tripSupport::resetCurrent(void)
{

	doResetTrip(0);
	outputResetStatus(0);

}

static void tripSupport::resetTank(void)
{

	doResetTrip(1);
	outputResetStatus(1);

}

static void tripSupport::outputResetStatus(uint8_t tripSlot)
{

	text::statusOut(m8DevLCDidx, tripFormatReverseNames, tripSlot + 1, PSTR(" Trip Reset"));

}

#endif // defined(useButtonInput)
#if defined(useWindowTripFilter)
static void tripSupport::resetWindowFilter(void)
{

	mainProgram8Variables[(uint16_t)(m8CurrentWindowTripIdx - m8VariableStartIdx)] = windowTripFilterSize;

	for (uint8_t x = 0; x < windowTripFilterSize; x++) tripVar::reset(windowTripFilterIdx + x);

}

#endif // defined(useWindowTripFilter)
#if defined(useChryslerMAPCorrection)
/* Chrysler returnless fuel pressure correction display section */

static const uint8_t prgmCalculateMAPpressure[] PROGMEM = {
	instrLdRegVariable, 0x02, v16AnalogMAPchannelIdx,	// load analog channel ADC step value
	instrSubVariableFromX, 0x02, m32AnalogMAPfloorIdx,	// is reading below MAP sensor voltage floor?
	instrBranchIfLT, 3,									// if not, continue
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//cont1:
	instrMul2byVariable, m32AnalogMAPnumerIdx,			// perform conversion to get pressure units per volts value
	instrDiv2byVariable, m32AnalogMAPdenomIdx,			// divide by pressure units per volts value
	instrAddEEPROMtoX, 0x02, pMAPsensorOffsetIdx,		// add pressure offset value from EEPROM
	instrStRegVariable, 0x02, m32MAPpressureIdx,		// store resulting MAP sensor reading
#if defined(useChryslerBaroSensor)
	instrDone											// exit to caller
};

static const uint8_t prgmCalculateBaroPressure[] PROGMEM = {
	instrLdRegVariable, 0x02, v16AnalogBaroChannelIdx,	// load analog channel ADC step value
	instrSubVariableFromX, 0x02, m32AnalogBaroFloorIdx,	// is reading below barometric sensor voltage floor?
	instrBranchIfLT, 3,									// if not, continue
	instrLdRegByte, 0x02, 0,							// zero out result in register 2

//cont1:
	instrMul2byVariable, m32AnalogBaroNumerIdx,			// convert to obtain pressure units per volts value
	instrDiv2byVariable, m32AnalogBaroDenomIdx,			// divide by pressure units per volts value
	instrAddEEPROMtoX, 0x02, pBaroSensorOffsetIdx,		// add pressure offset value from EEPROM
	instrStRegVariable, 0x02, m32BaroPressureIdx,		// store resulting barometric sensor reading
#endif // defined(useChryslerBaroSensor)
	instrLdRegVariable, 0x02, m32FuelPressureIdx,		// get fuel system differential pressure
	instrAddVariableToX, 0x02, m32BaroPressureIdx,		// add to reference barometric pressure to get fuel system absolute pressure
	instrSubVariableFromX, 0x02, m32MAPpressureIdx,		// subtract MAP to get differential pressure across the fuel injector
	instrStRegVariable, 0x02, m32InjPressureIdx,		// store differential pressure across the fuel injector
	instrMul2byRdOnly, idxCorrectionFactor2,			// set up for iSqrt
	instrDiv2byVariable, m32FuelPressureIdx,			// divide by the fuel system differential pressure
	instrTestReg, 0x02,									// test whether overflow occurred
	instrBranchIfOverflow, 6,							// if overflow occurred, go handle it
	instrIsqrt, 0x02,									// perform square root on result
	instrStRegVariable, 0x02, v32InjectorCorrectionIdx,	// save square root of presssure differential ratio as fuel injector correction factor
	instrDone,											// return to caller

//cont3:
	instrLdRegRdOnly, 0x02, idxCorrectionFactor,
	instrStRegVariable, 0x02, v32InjectorCorrectionIdx,	// save initial injector correction index for pressure differential calculation
	instrDone											// return to caller
};

static uint8_t pressureCorrect::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	switch (cmd)
	{

		case displayInitialEntryIdx:
		case displayCursorUpdateIdx:
			text::statusOut(m8DevLCDidx, pressureCorrectDisplayTitles, cursorPos); // briefly display display name

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

#endif // defined(useChryslerMAPCorrection)
