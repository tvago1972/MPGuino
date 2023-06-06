/* EEPROM parameter I/O section */

static const uint8_t prgmInitEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x02, pSignatureIdx,				// load the MPGuino signature long word currently in EEPROM
	instrLdRegEinit, 0x01, pSignatureIdx,				// load the MPGuino signature long word inside the program
	instrCmpXtoY, 0x21,									// are they the same?
	instrBranchIfNotE, 4,								// if not, go initialize the EEPROM
	instrLdRegByte, 0x02, 0,							// otherwise, signal nothing got initialized
	instrDone,											// exit to caller

//cont:
	instrLxdI, pSignatureIdx,

//loop:
	instrLdRegEinitIndexed, 0x02,						// loop through entire EEPROM space
	instrStRegEEPROMindexed, 0x02,						// store the program stored parameter defaults into EEPROM
	instrAddIndex, 1,
	instrCmpIndex, eePtrSettingsEnd,
	instrBranchIfLT, 246,
#ifdef useEEPROMtripStorage
	instrLdRegByte, 0x02, 0,							// zero out current and tank trip signature bytes
	instrStRegEEPROM, 0x02, pCurrTripSignatureIdx,
	instrStRegEEPROM, 0x02, pTankTripSignatureIdx,
#endif // useEEPROMtripStorage
	instrLdRegByte, 0x02, 1,
	instrDone											// exit to caller
};

static const uint8_t prgmInitMPGuino[] PROGMEM = {
	instrLdRegEEPROM, 0x02, pIdleTimeoutIdx,			// load idle timeout value in seconds
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vVehicleStopTimeoutIdx,	// store idle timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pEOCtimeoutIdx,				// load EOC timeout value in seconds
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vEngineOffTimeoutIdx,		// store EOC timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pMinGoodSpeedidx,			// fetch minimum good vehicle speed value in (distance)(* 1000) / hour
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// multiply by distance parameter value in (VSS pulses) / (distance)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegConst, 0x02, idxSecondsPerHour,			// fetch seconds / hour constant
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrMul2byConst, idxDecimalPoint,					// term is now (timer0 cycles)(* 1000) / (hour)
	instrDiv2by1,										// perform conversion, term is now in timer0 cycles / VSS pulse
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumVSSperiodIdx,		// store maximum good VSS period of timer0 cycles / VSS pulse
	instrDiv2byConst, idxCycles0PerTick,				// perform conversion, term is now in timer0 ticks
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vDetectVehicleStopIdx,	// store minimum good vehicle speed of timer0 ticks / VSS pulse

	instrLdRegEEPROM, 0x02, pCrankRevPerInjIdx,			// load number of crank revolutions / injector fire event
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrMul2byByte, 60,								// term is now in (crank revolutions)(timer0 cycles) / (fire event)(minute)
	instrDiv2byEEPROM, pMinGoodRPMidx,					// divide by minimum good engine speed value in crank revolutions / minute
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// store maximum good engine period value in timer0 cycles / fire event
	instrDiv2byConst, idxCycles0PerTick,				// perform conversion, term is now in timer0 ticks
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vDetectEngineOffIdx,		// store minimum good engine speed value in timer0 ticks / fire event

	instrLdRegEEPROM, 0x02, pButtonTimeoutIdx,			// load button press timeout stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vButtonTimeoutIdx,		// store button press timeout timer ticks value

	instrLdRegEEPROM, 0x02, pParkTimeoutIdx,			// load parking timeout stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vParkTimeoutIdx,			// store parking timeout timer ticks value

	instrLdRegEEPROM, 0x02, pActivityTimeoutIdx,		// load activity timeout stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vActivityTimeoutIdx,		// store activity timeout timer ticks value

	instrLdRegEEPROM, 0x02, pInjectorOpeningTimeIdx,	// fetch injector opening settle time in microseconds
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrDiv2byConst, idxMicroSecondsPerSecond,			// divide by microseconds per seconds value
	instrStRegVolatile, 0x02, vInjectorOpenDelayIdx,	// store injector opening settle time value in cycles
	instrLdReg, 0x23,									// save for later injector opening settle time value in register 3

	instrLdRegEEPROM, 0x02, pInjectorClosingTimeIdx,	// fetch injector closing settle time in microseconds
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrDiv2byConst, idxMicroSecondsPerSecond,			// divide by microseconds per seconds value
	instrStRegVolatile, 0x02, vInjectorCloseDelayIdx,	// store injector closing settle time value in cycles
	instrAddYtoX, 0x32,									// add injector opening settle time value to total injector settle time
	instrStRegVolatile, 0x02, vInjectorTotalDelayIdx,	// store injector total settle time value in cycles
	instrLdReg, 0x23,									// save injector total settle time value in register 3
	instrLdRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// load maximum good engine period value in timer0 cycles / fire event
	instrSubYfromX, 0x32,								// subtract total injector settle time from maximum good engine period
	instrStRegVolatile, 0x02, vInjectorValidMaxWidthIdx,	// store maximum valid fuel injector pulse width in timer0 cycles

	instrLdRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,	// fetch injector cycle time in microseconds per US gallon
#ifdef useImperialGallon
	instrBranchIfMetricMode, 5,							// if metric mode set, skip ahead to cycles conversion
	instrMul2byConst, idxNumerImperialGallon,			// perform conversion from microseconds per liter into microseconds per Imperial gallon
	instrDiv2byConst, idxDenomImperialGallon,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont0:
#else	// useImperialGallon
	instrBranchIfSAEmode, 5,							// if metric mode not set, skip ahead to cycles conversion
#endif	// useImperialGallon
	instrMul2byConst, idxDenomVolume,					// perform conversion from microseconds per US gallon into microseconds per liter
	instrDiv2byConst, idxNumerVolume,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont:
	instrMul2byConst, idxCycles0PerSecond,				// multiply to get cycles-microseconds per unit volume-second value
	instrDiv2byConst, idxMicroSecondsPerSecond,			// convert to get cycles per unit volume value
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpCyclesPerVolumeIdx,			// save cycles per unit volume value

	instrLdRegEEPROM, 0x02, pTankSizeIdx,				// fetch tank size in unit volume * formatting factor
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpTankSizeIdx,				// save tank size in cycles

	instrLdRegEEPROM, 0x02, pTankBingoSizeIdx,			// fetch bingo tank size in unit volume
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get bingo tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpBingoTankSizeIdx,			// save bingo tank size in cycles

#ifdef usePartialRefuel
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,				// fetch partial refuel tank size in unit volume
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get partial refuel tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpPartialRefuelTankSize,		// save partial refuel tank size in cycles

#endif	// usePartialRefuel
#ifdef useDragRaceFunction
	instrLdRegByte, 0x02, 0,							// zero out accel test top speed and estimated engine power
	instrStRegVolatile, 0x02, vDragInstantSpeedIdx,

#endif	// useDragRaceFunction
#ifdef useCoastDownCalculator
	instrLdRegEEPROM, 0x02, pCoastdownSamplePeriodIdx,	// coastdown timer ticks value
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vCoastdownPeriodIdx,		// store coastdown timeout timer ticks value

#endif	// useCoastDownCalculator
#ifdef useChryslerMAPCorrection
	instrLdRegEEPROM, 0x02, pMAPsensorFloorIdx,			// convert pressure sensor voltage floor to equivalent ADC floor value
	instrMul2byConst, idxNumerVoltage,
	instrDiv2byConst, idxDenomVoltage,
	instrStRegMain, 0x02, mpAnalogMAPfloorIdx,			// store it in volatile variable storage

	instrLdRegEEPROM, 0x02, pMAPsensorCeilingIdx,		// calculate pressure sensor ADC step slope denominator
	instrLdRegEEPROM, 0x01, pMAPsensorFloorIdx,
	instrSubYfromX, 0x12,
	instrMul2byConst, idxNumerVoltage,
	instrStRegMain, 0x02, mpAnalogMAPdenomIdx,			// store it in volatile variable storage

	instrLdRegEEPROM, 0x02, pMAPsensorRangeIdx,
	instrMul2byConst, idxDenomVoltage,					// calculate pressure sensor ADC step slope numerator
	instrStRegMain, 0x02, mpAnalogMAPnumerIdx,			// store it in volatile variable storage

#ifdef useChryslerBaroSensor
	instrLdRegEEPROM, 0x02, pBaroSensorFloorIdx,		// convert pressure sensor voltage floor to equivalent ADC floor value
	instrMul2byConst, idxNumerVoltage,
	instrDiv2byConst, idxDenomVoltage,
	instrStRegMain, 0x02, mpAnalogBaroFloorIdx,			// store it in volatile variable storage

	instrLdRegEEPROM, 0x02, pBaroSensorCeilingIdx,		// calculate pressure sensor ADC step slope numerator
	instrLdRegEEPROM, 0x01, pBaroSensorFloorIdx,
	instrSubYfromX, 0x12,
	instrMul2byConst, idxNumerVoltage,
	instrStRegMain, 0x02, mpAnalogBaroDenomIdx,			// store it in volatile variable storage

	instrLdRegConst, 0x02, idxDenomVoltage,				// calculate pressure sensor ADC step slope numerator
	instrMul2byEEPROM, pBaroSensorRangeIdx,
	instrStRegMain, 0x02, mpAnalogBaroNumerIdx,			// store it in volatile variable storage
#else	// useChryslerBaroSensor

	instrLdRegEEPROM, 0x02, pBarometricPressureIdx,		// fetch reference barometric pressure value in psig * 1000
	instrStRegMain, 0x02, mpBaroPressureIdx,			// store it in volatile variable storage
#endif	// useChryslerBaroSensor
	instrLdRegEEPROM, 0x02, pSysFuelPressureIdx,		// fetch base fuel pressure value in psig * 1000
	instrStRegMain, 0x02, mpFuelPressureIdx,			// save base fuel pressure value in psig * 1000

	instrLdRegConst, 0x02, idxCorrectionFactor,
	instrStRegVolatile, 0x02, vInjectorCorrectionIdx,	// save initial injector correction index for pressure differential calculation

#endif	// useChryslerMAPCorrection
#ifdef useBarFuelEconVsTime
	instrLdRegEEPROM, 0x02, pFEvsTimeIdx,				// load fuel econ vs time period stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vFEvsTimePeriodTimeoutIdx,	// store fuel econ vs time period timer ticks value

#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	instrLdRegEEPROM, 0x02, pBarLowSpeedCutoffIdx,		// obtain low-speed cutoff parameter in (distance)(* 1000) / (hour)
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulses)(* 1000) / (hour)
	instrDiv2byConst, idxSecondsPerHour,				// term is now (VSS pulses)(* 1000) / (second)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpFEvsSpeedMinThresholdIdx,	// store minimum threshold speed in (VSS pulses)(* 1000) / (second)

	instrLdRegEEPROM, 0x02, pBarSpeedQuantumIdx,		// fetch speed quantum parameter in (distance)(* 1000) / hour
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulses)(* 1000) / (hour)
	instrDiv2byConst, idxSecondsPerHour,				// term is now (VSS pulses)(* 1000) / (second)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpFEvsSpeedQuantumIdx,		// store speed quantum in (VSS pulses)(* 1000) / (second)

#endif // useBarFuelEconVsSpeed
#ifdef useCPUreading
	instrLdRegByte, 0x02, 0,
	instrStRegMain, 0x02, mpMainLoopAccumulatorIdx,		// initialize the cpu utilization stopwatch timer values
	instrStRegMain, 0x02, mpIdleAccumulatorIdx,
#ifdef useDebugCPUreading
	instrStRegVolatile, 0x02, vInterruptAccumulatorIdx,
	instrStRegMain, 0x02, mpDebugAccMainLoopIdx,
	instrStRegMain, 0x02, mpDebugAccIdleIdx,
	instrStRegMain, 0x02, mpDebugAccIdleProcessIdx,
	instrStRegMain, 0x02, mpDebugAccInterruptIdx,
	instrStRegMain, 0x02, mpDebugAccDisplayIdx,
	instrStRegMain, 0x02, mpDebugAccSWEET64idx,
	instrStRegMain, 0x02, mpDebugAccS64multIdx,
	instrStRegMain, 0x02, mpDebugCountS64multIdx,
	instrStRegMain, 0x02, mpDebugAccS64divIdx,
	instrStRegMain, 0x02, mpDebugCountS64divIdx,
#endif // useDebugCPUreading

#endif	// useCPUreading
	instrDone											// exit to caller
};

static uint8_t EEPROM::powerUpCheck(void)
{

	uint8_t b;

	b = SWEET64::runPrgm(prgmInitEEPROM, 0); // perform EEPROM initialization if required, and cause MPGuino initialization when done

#ifdef useScreenEditor
	if (b)
	{

		uint8_t t = eePtrScreensStart;
		for (uint8_t x = 0; x < mainScreenDisplayFormatSize; x++) writeVal(t++, (unsigned long)(pgm_read_byte(&mainScreenDisplayFormats[(unsigned int)(x)])));

	}

#endif // useScreenEditor
	initGuino();

	return b;

}

static void EEPROM::initGuinoHardware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega32U4__)
	EIMSK &= ~((1 << INT3) | (1 << INT2)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC31) | (1 << ISC30) | (1 << ISC21) | (1 << ISC20)); // set injector sense pin control
	EICRA &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC30 : ISC20));

	EIFR |= ((1 << INTF3) | (1 << INTF2)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT3) | (1 << INT2)); // enable fuel injector sense interrupts

	PCMSK0 |= (1 << PCINT7); // enable port B VSS pin interrupt

	PCICR |= (1 << PCIE0); // enable selected interrupts on port B

	lastPINxState = PINB;

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	EIMSK &= ~((1 << INT5) | (1 << INT4)); // disable fuel injector sense interrupts

	EICRB |= ((1 << ISC51) | (1 << ISC50) | (1 << ISC41) | (1 << ISC40)); // set injector sense pin control
	EICRB &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC50 : ISC40));

	EIFR |= ((1 << INTF5) | (1 << INTF4)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT5) | (1 << INT4)); // enable fuel injector sense interrupts

	DIDR2 &= ~(1 << ADC8D); // enable digital input on VSS pin

	PCMSK2 |= (1 << PCINT16); // enable port K VSS pin interrupt

	PCICR |= (1 << PCIE2); // enable selected interrupts on port K

	lastPINxState = PINK;

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	EIMSK &= ~((1 << INT1) | (1 << INT0)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00)); // set injector sense pin control
	EICRA &= ~(1 << (EEPROM::readVal(pInjEdgeTriggerIdx) ? ISC10 : ISC00));

	EIFR |= ((1 << INTF1) | (1 << INTF0)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT1) | (1 << INT0)); // enable fuel injector sense interrupts

	DIDR0 &= ~(1 << ADC0D); // enable digital input on VSS pin

	PCMSK1 |= (1 << PCINT8); // enable port C VSS pin interrupt

	PCICR |= (1 << PCIE1); // enable selected interrupts on port C

	lastPINxState = PINC;

#endif // defined(__AVR_ATmega328P__)
#if defined(useAnalogRead)
#if defined(__AVR_ATmega32U4__)
	PRR0 &= ~(1 << PRADC); // turn on ADC module
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRADC); // turn on ADC module
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRADC); // turn on ADC module
#endif // defined(__AVR_ATmega328P__)

	// disable ADC free-running mode
	ADCSRA &= ~(1 << ADATE); // the AVR hardware just ignores this bit, and always goes to free running mode

	// enable ADC
	ADCSRA |= (1 << ADEN);

	// set ADMUX[5] bit to 0
	ADCSRB = 0;

	// set ADC timer frequency to 1/128 of system timer
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

#endif // useAnalogRead
	VSSpause = readVal(pVSSpauseIdx);

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::initGuinoSoftware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if (readVal(pMetricModeIdx)) metricFlag |= metricMode;
	else metricFlag &= ~(metricMode);

	if (readVal(pAlternateFEidx)) metricFlag |= alternateFEmode;
	else metricFlag &= ~(alternateFEmode);

	SWEET64::runPrgm(prgmInitMPGuino, 0); // calculate multiple MPGuino system values for use within code

#ifdef useBarFuelEconVsTime
	timer0Command |= (t0cResetFEvTime); // reset fuel economy vs time bargraph mechanism

#endif // useBarFuelEconVsTime
	SREG = oldSREG; // restore interrupt flag status

#ifdef useBarFuelEconVsSpeed
	bgFEvsSsupport::reset();

#endif // useBarFuelEconVsSpeed
#ifdef useWindowTripFilter
	tripSupport::resetWindowFilter();

#endif // useWindowTripFilter
}

static void EEPROM::initGuino(void) // initialize MPGuino base hardware and basic system settings
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	initGuinoHardware();
	initGuinoSoftware();

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::writeVal(uint8_t eePtr, uint32_t value)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	SWEET64::init64(vee, value);

	write64(vee, eePtr);

}

static uint8_t EEPROM::readVal(uint8_t eePtr)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	read64(vee, eePtr);

	return vee->u8[0];

}

static void EEPROM::read64(union union_64 * an, uint8_t parameterIdx)
{

	uint8_t oldSREG;
	uint16_t t;
	uint16_t u;

	t = getAddress(parameterIdx);
	u = getAddress(++parameterIdx);

	SWEET64::init64byt(an, 0);

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	for (uint16_t x = t; x < u; x++) an->u8[x - t] = eeprom_read_byte((uint8_t *)(x));

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::write64(union union_64 * an, uint8_t parameterIdx)
{

	uint8_t oldSREG;
	uint16_t t;
	uint16_t u;
	uint8_t l;
	uint8_t b;
	uint8_t eByt;
	uint8_t rByt;

	l = getParameterFlags(parameterIdx);
	t = getAddress(parameterIdx);
	u = getAddress(++parameterIdx);

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	b = 0;

	for (uint16_t x = t; x < u; x++)
	{

		eByt = eeprom_read_byte((uint8_t *)(x));
		rByt = an->u8[x - t];
		if (eByt != rByt)
		{

			metricFlag |= (EEPROMbulkChangeFlag);
			b = 1;
			eeprom_write_byte((uint8_t *)(x), rByt);

		}

	}

	if (b)
	{

		switch (l & 0xE0)
		{

			case pfSoftwareInitMPGuino:
				metricFlag |= (softInitGuinoFlag);
				break;

			case pfHardwareInitMPGuino:
				metricFlag |= (hardInitGuinoFlag);
				break;

			case pfDoMetricModeConversion:
				metricFlag |= (metricConversionFlag);
				break;

			case pfChangeDisplay:
				metricFlag |= (changeDisplayFlag);
				break;

			case pfCalculateFuelParams:
				metricFlag |= (calculateFuelParamFlag);
				break;

		}

	}

	SREG = oldSREG; // restore interrupt flag status

}

static uint16_t EEPROM::getAddress(uint8_t eePtr)
{

	uint16_t t;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) t = (uint16_t)(pgm_read_byte(&paramAddrs[eePtr]));
#ifdef useScreenEditor
	else if ((eePtr >= eePtrScreensStart) && (eePtr < eePtrScreensEnd)) t = 2 * (eePtr - eePtrScreensStart) + eeAdrScreensStart;
#endif // useScreenEditor
	else t = eeAdrStorageEnd;

	return t;

}

static uint8_t EEPROM::getParameterFlags(uint8_t eePtr)
{

	uint8_t t;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) t = pgm_read_byte(&paramsLength[(uint16_t)(eePtr)]) & 0xF8;
	else t = 0;

	return t;

}

static uint8_t EEPROM::getLength(uint8_t eePtr)
{

	uint8_t t;
	uint8_t u;
	uint8_t l;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) l = pgm_read_byte(&paramsLength[(uint16_t)(eePtr)]) & 0x07;
	else l = 0;

	t = getAddress(eePtr);
	u = getAddress(++eePtr);
	u -= t;
	if (l) u--;
	l += (u << 3);

	return l;

}

