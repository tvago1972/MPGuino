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
	instrLdRegEinitIndexed, 0x02,						// loop through parameter EEPROM space
	instrStRegEEPROMindexed, 0x02,						// store the program stored parameter defaults into EEPROM
	instrAddIndex, 1,
	instrCmpIndex, eePtrSettingsEnd,
	instrBranchIfLT, 246,
	instrLdRegByte, 0x02, 0,							// load a zero into register 2
//loop2:
	instrStRegEEPROMindexed, 0x02,						// zero the corresponding position in EEPROM
	instrAddIndex, 1,
	instrCmpIndex, eePtrEnd,
	instrBranchIfLT, 248,
	instrLdRegByte, 0x02, 1,
	instrDone											// exit to caller
};

static const uint8_t prgmInitMPGuino[] PROGMEM = {
#if defined(useSoftwareClock)
	instrLdRegConst, 0x02, idxSecondsPerDay,			// load the number of seconds per day
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vClockCycleDayLengthIdx,	// store the day length in timer0 ticks

#endif // defined(useSoftwareClock)
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

	instrLdRegEEPROM, 0x02, pMinGoodRPMidx,				// load minimum good engine speed value in (crank revolutions) / (minute)
	instrMul2byEEPROM, pInjPer2CrankRevIdx,				// multiply by the number of (injector fire event) / (2)(crank revolutions)
	instrLdReg, 0x21,									// move denominator (injector fire event) / (2)(minute) to register 1

	instrLdRegConst, 0x02, idxCycles0PerSecond,			// load (timer0 cycles) / (second) term
	instrMul2byByte, 120,								// term is now in (2)(timer0 cycles) / (minute)
	instrDiv2by1,										// divide by minimum good engine speed value in (injector fire event) / (2)(minute)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// store maximum good engine period value in (timer0 cycles) / (injector fire event)
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

	instrLdRegEEPROM, 0x02, pInjectorSettleTimeIdx,		// fetch injector settle time in microseconds
	instrMul2byConst, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrDiv2byConst, idxMicroSecondsPerSecond,			// divide by microseconds per seconds value
	instrStRegVolatile, 0x02, vInjectorOpenDelayIdx,	// store injector settle time value in cycles
	instrLdReg, 0x23,									// save injector settle time value in register 3
	instrLdRegVolatile, 0x02, vMaximumEnginePeriodIdx,	// load maximum good engine period value in timer0 cycles / fire event
	instrSubYfromX, 0x32,								// subtract injector settle time from maximum good engine period
	instrStRegVolatile, 0x02, vInjectorValidMaxWidthIdx,	// store maximum valid fuel injector pulse width in timer0 cycles

	instrLdRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,	// fetch injector cycle time in microseconds per US gallon
#ifdef useImperialGallon
	instrBranchIfMetricMode, 5,							// if metric mode set, skip ahead to cycle0s conversion
	instrMul2byConst, idxNumerImperialGallon,			// perform conversion from microseconds per liter into microseconds per Imperial gallon
	instrDiv2byConst, idxDenomImperialGallon,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont0:
#else	// useImperialGallon
	instrBranchIfSAEmode, 5,							// if metric mode not set, skip ahead to cycle0s conversion
#endif	// useImperialGallon
	instrMul2byConst, idxDenomVolume,					// perform conversion from microseconds per US gallon into microseconds per liter
	instrDiv2byConst, idxNumerVolume,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont:
	instrMul2byConst, idxCycles0PerSecond,				// multiply to get cycle0s-microseconds per unit volume-second value
	instrDiv2byConst, idxMicroSecondsPerSecond,			// convert to get cycle0s per unit volume value
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpCyclesPerVolumeIdx,			// save cycle0s per unit volume value

	instrLdRegEEPROM, 0x02, pTankSizeIdx,				// fetch tank size in unit volume * formatting factor
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycle0s per unit volume value to get tank size in cycle0s * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get tank size in cycle0s
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpTankSizeIdx,				// save tank size in cycle0s

	instrLdRegEEPROM, 0x02, pTankBingoSizeIdx,			// fetch bingo tank size in unit volume
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycle0s per unit volume value to get tank size in cycle0s * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get bingo tank size in cycle0s
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpBingoTankSizeIdx,			// save bingo tank size in cycle0s

	instrLdRegConst, 0x02, idxCycles0PerHour,			// load timer0 clock cycles per hour
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term
	instrStRegMain, 0x02, mpLargeSpeedFactorIdx,		// save large speed factor

#if defined(usePartialRefuel)
	instrLdRegEEPROM, 0x02, pRefuelSizeIdx,				// fetch partial refuel tank size in unit volume
	instrMul2byMain, mpCyclesPerVolumeIdx,				// multiply by cycles per unit volume value to get tank size in cycles * formatting factor
	instrDiv2byConst, idxDecimalPoint,					// remove formatting factor to get partial refuel tank size in cycles
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegMain, 0x02, mpPartialRefuelTankSize,		// save partial refuel tank size in cycles

#endif // defined(usePartialRefuel)
#if defined(useCoastDownCalculator)
	instrLdRegEEPROM, 0x02, pCoastdownSamplePeriodIdx,	// coastdown timer ticks value
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vCoastdownPeriodIdx,		// store coastdown timeout timer ticks value

#endif	// defined(useCoastDownCalculator)
#if defined(useChryslerMAPCorrection)
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

#if defined(useChryslerBaroSensor)
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
#else // defined(useChryslerBaroSensor)

	instrLdRegEEPROM, 0x02, pBarometricPressureIdx,		// fetch reference barometric pressure value in psig * 1000
	instrStRegMain, 0x02, mpBaroPressureIdx,			// store it in volatile variable storage
#endif // defined(useChryslerBaroSensor)
	instrLdRegEEPROM, 0x02, pSysFuelPressureIdx,		// fetch base fuel pressure value in psig * 1000
	instrStRegMain, 0x02, mpFuelPressureIdx,			// save base fuel pressure value in psig * 1000

	instrLdRegConst, 0x02, idxCorrectionFactor,
	instrStRegVolatile, 0x02, vInjectorCorrectionIdx,	// save initial injector correction index for pressure differential calculation

#endif	// defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsTime)
	instrLdRegEEPROM, 0x02, pFEvsTimeIdx,				// load fuel econ vs time period stored parameter
	instrMul2byConst, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVolatile, 0x02, vFEvsTimePeriodTimeoutIdx,	// store fuel econ vs time period timer ticks value

#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
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

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useCPUreading) || defined(useDebugCPUreading)
	instrLdRegByte, 0x02, 0,
	instrStRegMain, 0x02, mpCPUsampledMainLoopIdx,		// initialize the cpu utilization stopwatch timer values
	instrStRegMain, 0x02, mpCPUworkingMainWorkingIdx,
	instrStRegMain, 0x02, mpCPUsampledMainWorkingIdx,
#if defined(useDebugCPUreading)
	instrStRegVolatile, 0x02, vWorkingInterruptProcessIdx,
	instrStRegMain, 0x02, mpDbgSampledInterruptProcessIdx,
	instrStRegMain, 0x02, mpDbgWorkingSleepModeIdx,
	instrStRegMain, 0x02, mpDbgSampledSleepModeIdx,
	instrStRegMain, 0x02, mpCPUworkingIdleProcessIdx,
	instrStRegMain, 0x02, mpCPUsampledIdleProcessIdx,
	instrStRegMain, 0x02, mpDbgWorkingMainProcessIdx,
	instrStRegMain, 0x02, mpDbgSampledMainProcessIdx,
	instrStRegMain, 0x02, mpDbgWorkingPeriodicProcessIdx,
	instrStRegMain, 0x02, mpDbgSampledPeriodicProcessIdx,
	instrStRegMain, 0x02, mpDbgWorkingDisplayProcessIdx,
	instrStRegMain, 0x02, mpDbgSampledDisplayProcessIdx,
	instrStRegMain, 0x02, mpDbgWorkingS64processIdx,
	instrStRegMain, 0x02, mpDbgSampledS64processIdx,
	instrStRegMain, 0x02, mpDebugAccS64multIdx,
	instrStRegMain, 0x02, mpDebugCountS64multIdx,
	instrStRegMain, 0x02, mpDebugAccS64divIdx,
	instrStRegMain, 0x02, mpDebugCountS64divIdx,
#if defined(useIsqrt)
	instrStRegMain, 0x02, mpDebugAccS64sqrtIdx,
	instrStRegMain, 0x02, mpDebugCountS64sqrtIdx,
#endif // defined(useIsqrt)
#endif // defined(useDebugCPUreading)
#endif	// defined(useCPUreading) || defined(useDebugCPUreading)


#if defined(useDragRaceFunction)
	instrLdRegEEPROM, 0x02, pDragSpeedIdx,				// load acceleration test full speed parameter in (distance)(* 1000) / (hour)
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulse)(* 1000) / (hour)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegConst, 0x02, idxSecondsPerHour,			// fetch (second) / (hour) constant
	instrMul2byConst, idxCycles0PerSecond,				// term is now (timer0 cycles) / (hour) term
	instrMul2byConst, idxDecimalPoint,					// term is now (timer0 cycles)(* 1000) / (hour)
	instrMul2byByte, 2,									// term is now (timer0 cycles)(* 2000) / (hour)
	instrDiv2by1,										// perform conversion, term is now in (timer0 cycles)(* 2) / (VSS pulse)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelHalfPeriodValueIdx,	// save it to accel test half-speed period tripwire variable
	instrDiv2byByte, 2,									// term is now (timer0 cycles) / (VSS pulse)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelFullPeriodValueIdx,	// save it to accel test full-speed period tripwire variable
	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch drag function distance parameter value in VSS pulses
	instrMul2byEEPROM, pDragDistanceIdx,				// multiply by drag function distance parameter value in unit distance
	instrDiv2byConst, idxDecimalPoint,					// get rid of decimal formatting factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVolatile, 0x02, vAccelDistanceValueIdx,	// save it to accel test distanct tripwire variable

#endif // defined(useDragRaceFunction)
	instrDone											// exit to caller
};

static const uint8_t prgmDoEEPROMmetricConversion[] PROGMEM = {
	instrLdRegConstMetric, 0x12, idxNumerDistance,		// convert minimum good speed value in (distance) / (time)
	instrLdRegConstMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pMinGoodSpeedidx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pMinGoodSpeedidx,

	instrLdRegConstMetric, 0x12, idxDenomDistance,		// convert pulses per distance value in (count) / (distance)
	instrLdRegConstMetric, 0x21, idxNumerDistance,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pPulsesPerDistanceIdx,

	instrLdRegConstMetric, 0x12, idxNumerVolume,		// convert total tank size value in (volume)
	instrLdRegConstMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pTankSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pTankSizeIdx,

	instrLdRegConstMetric, 0x12, idxNumerVolume,		// convert bingo tank size value in (volume)
	instrLdRegConstMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pTankBingoSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pTankBingoSizeIdx,

#if defined(usePartialRefuel)
	instrLdRegConstMetric, 0x12, idxNumerVolume,		// convert refuel quantity value in (volume)
	instrLdRegConstMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pRefuelSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pRefuelSizeIdx,

#if defined(useEEPROMtripStorage)
	instrLdRegConstMetric, 0x12, idxNumerVolume,		// convert saved refuel quantity value in (volume)
	instrLdRegConstMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pRefuelSaveSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pRefuelSaveSizeIdx,

#endif // defined(useEEPROMtripStorage)
#endif // defined(usePartialRefuel)
#if defined(useFuelCost)
	instrLdRegConstMetric, 0x12, idxDenomVolume,		// convert fuel cost value in (cost) / (volume)
	instrLdRegConstMetric, 0x21, idxNumerVolume,
	instrMul2byEEPROM, pCostPerQuantity,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pCostPerQuantity,

#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert MAP sensor pressure range value in (force) / (area * volt)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pMAPsensorRangeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pMAPsensorRangeIdx,

	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert MAP sensor pressure offset value in (force) / (area * volt)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pMAPsensorOffsetIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pMAPsensorOffsetIdx,

#if defined(useChryslerBaroSensor)
	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert barometric sensor pressure range value in (force) / (area * volt)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pBaroSensorRangeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBaroSensorRangeIdx,

	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert barometric sensor pressure offset value in (force) / (area * volt)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pBaroSensorOffsetIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBaroSensorOffsetIdx,

#else // defined(useChryslerBaroSensor)
	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert reference barometric pressure value in (force) / (area)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pBarometricPressureIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBarometricPressureIdx,

#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
	instrLdRegConstMetric, 0x12, idxNumerDistance,		// convert FEvSpeed histograph minimum speed value in (distance) / (time)
	instrLdRegConstMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pBarLowSpeedCutoffIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBarLowSpeedCutoffIdx,

	instrLdRegConstMetric, 0x12, idxNumerDistance,		// convert FEvSpeed histograph bar width value in (distance) / (time)
	instrLdRegConstMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pBarSpeedQuantumIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBarSpeedQuantumIdx,

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useVehicleParameters)
	instrLdRegConstMetric, 0x12, idxNumerMass,			// convert vehicle mass (weight) value in (mass)
	instrLdRegConstMetric, 0x21, idxDenomMass,
	instrMul2byEEPROM, pVehicleMassIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pVehicleMassIdx,

#if defined(useCoastDownCalculator)
	instrLdRegConstMetric, 0x12, idxNumerArea,			// convert frontal area value in (area)
	instrLdRegConstMetric, 0x21, idxDenomArea,
	instrMul2byEEPROM, pVehicleFrontalAreaIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pVehicleFrontalAreaIdx,

	instrLdRegConstMetric, 0x12, idxNumerDensity,		// convert vehicle mass (weight) value in (mass) / (volume)
	instrLdRegConstMetric, 0x21, idxDenomDensity,
	instrMul2byEEPROM, pLocustDensityIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pLocustDensityIdx,

#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	instrLdRegConstMetric, 0x12, idxNumerDistance,		// convert accel test defined speed value in (distance) / (time)
	instrLdRegConstMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pDragSpeedIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pDragSpeedIdx,

	instrLdRegConstMetric, 0x12, idxNumerDistance,		// convert accel test defined distance value value in (distance)
	instrLdRegConstMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pDragDistanceIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pDragDistanceIdx,

#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#ifdef useCalculatedFuelFactor
	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert system fuel pressure value in (force) / (area)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pSysFuelPressureIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pSysFuelPressureIdx,

	instrLdRegConstMetric, 0x12, idxNumerPressure,		// convert reference fuel injector pressure value in (force) / (area)
	instrLdRegConstMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pRefFuelPressureIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pRefFuelPressureIdx,

#endif // useCalculatedFuelFactor
	instrDone											// return to caller
};

#ifdef useCalculatedFuelFactor
static const uint8_t prgmCalculateFuelFactor[] PROGMEM = {
	instrLdRegConst, 0x02, idxCorrectionFactor2,		// obtain reference correction factor
	instrMul2byEEPROM, pSysFuelPressureIdx,				// multiply by this vehicle's stored fuel system absolute pressure
	instrDiv2byEEPROM, pRefFuelPressureIdx,				// divide by the fuel injector reference absolute pressure
	instrIsqrt, 0x02,									// perform square root on result
	instrMul2byEEPROM, pInjectorCountIdx,				// multiply by stored number of fuel injectors for this engine
	instrMul2byEEPROM, pInjectorSizeIdx,				// multiply by injector size in cc/minute * decimal formatting factor (L/min * decimal formatting factor * 1000)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegByte, 0x02, 60,							// load seconds per minute into register 2
	instrMul2byConst, idxMicroSecondsPerSecond,			// multiply by microseconds per second into register 1
	instrMul2byConst, idxOneThousand,					// multiply by number of cc's per liter into register 1
	instrMul2byConst, idxDecimalPoint,					// set numerator up to cancel reference correction factor in denominator
	instrMul2byConst, idxCorrectionFactor,				// set numerator up to cancel reference correction factor in denominator
	instrDiv2by1,										// perform comversion for injector flow
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byConst, idxNumerVolume,					// set up denominator to convert injector flow in liters to US gallons
	instrDiv2byConst, idxDenomVolume,					// perform conversion of injector flow in liters to US gallons
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,
	instrDone											// return to caller
};

#endif // useCalculatedFuelFactor
static uint8_t EEPROM::powerUpCheck(void)
{

	uint8_t b;

	b = SWEET64::runPrgm(prgmInitEEPROM, 0); // perform EEPROM initialization if required, and cause MPGuino initialization when done

#if defined(useScreenEditor)
	if (b)
	{

		uint8_t t = eePtrDisplayPagesStart;
		for (uint8_t x = 0; x < mainDisplayFormatSize; x++) writeVal(t++, (uint32_t)(pgm_read_word(&mainDisplayPageFormats[(uint16_t)(x)])));

	}

#endif // defined(useScreenEditor)
	initGuinoHardware();
	initGuinoSoftware();

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
	EICRA &= ~(1 << (readByte(pInjEdgeTriggerIdx) ? ISC30 : ISC20));

	EIFR |= ((1 << INTF3) | (1 << INTF2)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT3) | (1 << INT2)); // enable fuel injector sense interrupts

	PCMSK0 |= (1 << PCINT7); // enable port B VSS pin interrupt

	PCICR |= (1 << PCIE0); // enable selected interrupts on port B

	lastPINxState = PINB;

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	EIMSK &= ~((1 << INT5) | (1 << INT4)); // disable fuel injector sense interrupts

	EICRB |= ((1 << ISC51) | (1 << ISC50) | (1 << ISC41) | (1 << ISC40)); // set injector sense pin control
	EICRB &= ~(1 << (readByte(pInjEdgeTriggerIdx) ? ISC50 : ISC40));

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
	EICRA &= ~(1 << (readByte(pInjEdgeTriggerIdx) ? ISC10 : ISC00));

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

#endif // defined(useAnalogRead)
	VSSpause = readByte(pVSSpauseIdx);

	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::initGuinoSoftware(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	setMetricDisplayMode();

	SWEET64::runPrgm(prgmInitMPGuino, 0); // calculate multiple MPGuino system values for use within code

#if defined(useBarFuelEconVsTime)
	bitFlags[(uint16_t)(bfTimer0Command)] |= (t0cResetFEvTime); // reset fuel economy vs time bargraph mechanism

#endif // defined(useBarFuelEconVsTime)
	SREG = oldSREG; // restore interrupt flag status

#if defined(useBarFuelEconVsSpeed)
	bgFEvsSsupport::reset();

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useWindowTripFilter)
	tripSupport::resetWindowFilter();

#endif // defined(useWindowTripFilter)
}

static void EEPROM::setMetricDisplayMode(void)
{

	if (readByte(pMetricModeIdx)) bitFlags[(uint16_t)(bfMetricModeFlags)] |= (mmFuelEconOutputFlags);
	else bitFlags[(uint16_t)(bfMetricModeFlags)] &= ~(mmFuelEconOutputFlags);

	if (readByte(pAlternateFEidx)) bitFlags[(uint16_t)(bfMetricModeFlags)] ^= (mmDisplayAlternateFE);

}

static uint8_t EEPROM::onChange(const uint8_t * sched, uint8_t parameterIdx)
{

	uint8_t retVal;

	retVal = 0;

	bitFlags[(uint16_t)(bfEEPROMchangeStatus)] &= ~(ecsEEPROMchangeFlags); // clear all bit flags

	SWEET64::runPrgm(sched, parameterIdx); // perform initial SWEET64 call to store EEPROM parameter

#ifdef useCalculatedFuelFactor
	// calculate and store microseconds per US gallon factor (this will trigger ecsDoMPGuinoInitSoftware)
	if (bitFlags[(uint16_t)(bfEEPROMchangeStatus)] & ecsCalculateFuelParam) SWEET64::runPrgm(prgmCalculateFuelFactor, 0);

#endif // useCalculatedFuelFactor
	// perform conversion between metric mode and SAE mode (this will trigger ecsDoMPGuinoInitSoftware)
	if (bitFlags[(uint16_t)(bfEEPROMchangeStatus)] & ecsDoMetricConversion) SWEET64::runPrgm(prgmDoEEPROMmetricConversion, 0);

	if (bitFlags[(uint16_t)(bfEEPROMchangeStatus)] & ecsChangeDisplay)
	{

		if (parameterIdx == pAlternateFEidx) setMetricDisplayMode(); // ensure alternate FE mode is correctly handled

#if defined(useButtonInput)
#if LCDcharHeight == 4
		if ((parameterIdx == pSizeDisplayIdx) || (parameterIdx == pSizeBottomDisplayIdx))
#else // LCDcharHeight == 4
		if (parameterIdx == pSizeDisplayIdx)
#endif // LCDcharHeight == 4
		{

			workingDisplayIdx = readByte(pDisplayIdx);
			cursor::updateDisplay(workingDisplayIdx, displayInitialEntryIdx); // call indexed support section screen initialization function

		}

#endif // defined(useButtonInput)
#if defined(useLCDoutput)
		if (parameterIdx == pBrightnessIdx) LCD::setBrightness(readByte(pBrightnessIdx)); // adjust brightness

#if defined(useLCDcontrast)
		if (parameterIdx == pContrastIdx) LCD::setContrast(readByte(pContrastIdx)); // adjust contrast

#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
		if (parameterIdx == pLCDcolorIdx) LCD::setRGBcolor(readByte(pLCDcolorIdx)); // adjust backlight color

#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
	}

	if (bitFlags[(uint16_t)(bfEEPROMchangeStatus)] & ecsDoMPGuinoInitHardware) initGuinoHardware();

	if (bitFlags[(uint16_t)(bfEEPROMchangeStatus)] & ecsDoMPGuinoInitSoftware) initGuinoSoftware();

	if (bitFlags[(uint16_t)(bfEEPROMchangeStatus)] & ecsEEPROMchangeDetected) retVal = 1; // if the setting has changed

	return retVal;

}

static void EEPROM::writeByte(uint8_t eePtr, uint8_t value)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	SWEET64::init64byt(vee, value);

	write64(vee, eePtr);

}

#if defined(useScreenEditor)
static void EEPROM::writeVal(uint8_t eePtr, uint32_t value)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	SWEET64::init64(vee, value);

	write64(vee, eePtr);

}

#endif // defined(useScreenEditor)
static uint8_t EEPROM::readByte(uint8_t eePtr)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	read64(vee, eePtr);

	return vee->u8[0];

}

#if defined(useScreenEditor)
static uint16_t EEPROM::readWord(uint8_t eePtr)
{

	uint64_t val;
	union union_64 * vee = (union union_64 *) &val;

	read64(vee, eePtr);

	return vee->ui[0];

}

#endif // defined(useScreenEditor)
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

			bitFlags[(uint16_t)(bfEEPROMchangeStatus)] |= (ecsEEPROMchangeDetected);
			b = 1;
			eeprom_write_byte((uint8_t *)(x), rByt);

		}

	}

	if (b)
	{

		switch (l & 0xE0)
		{

			case pfSoftwareInitMPGuino:
				bitFlags[(uint16_t)(bfEEPROMchangeStatus)] |= (ecsDoMPGuinoInitSoftware);
				break;

			case pfHardwareInitMPGuino:
				bitFlags[(uint16_t)(bfEEPROMchangeStatus)] |= (ecsDoMPGuinoInitHardware);
				break;

			case pfDoMetricModeConversion:
				bitFlags[(uint16_t)(bfEEPROMchangeStatus)] |= (ecsDoMetricConversion);
				break;

			case pfChangeDisplay:
				bitFlags[(uint16_t)(bfEEPROMchangeStatus)] |= (ecsChangeDisplay);
				break;

			case pfCalculateFuelParams:
				bitFlags[(uint16_t)(bfEEPROMchangeStatus)] |= (ecsCalculateFuelParam);
				break;

		}

	}

	SREG = oldSREG; // restore interrupt flag status

}

static uint16_t EEPROM::getAddress(uint8_t eePtr)
{

	uint16_t t;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) t = pgm_read_word(&paramAddrs[eePtr]);
#if defined(useScreenEditor)
	else if ((eePtr >= eePtrDisplayPagesStart) && (eePtr < eePtrDisplayPagesEnd)) t = eeAdrScreensStart + 2 * (eePtr - eePtrDisplayPagesStart);
#endif // defined(useScreenEditor)
#if defined(useButtonInput)
	else if ((eePtr >= eePtrDisplayCursorStart) && (eePtr < eePtrDisplayCursorEnd)) t = eeAdrDisplayCursorStart + (eePtr - eePtrDisplayCursorStart);
	else if ((eePtr >= eePtrMenuHeightStart) && (eePtr < eePtrMenuHeightEnd)) t = eeAdrMenuCursorStart + (eePtr - eePtrMenuHeightStart);
#endif // defined(useButtonInput)
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

