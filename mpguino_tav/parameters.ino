/* EEPROM parameter I/O section */

static const uint8_t prgmInitEEPROM[] PROGMEM = {
	instrLdRegEEPROM, 0x02, pSignatureIdx,				// load the MPGuino signature long word currently in EEPROM
	instrLdRegRdOnly, 0x01, pSignatureIdx,				// load the MPGuino signature long word inside the program
	instrCmpXtoY, 0x21,									// are they the same?
	instrBranchIfNotE, 4,								// if not, go initialize the EEPROM
	instrLdRegByte, 0x02, 0,							// otherwise, signal nothing got initialized
	instrDone,											// exit to caller

//cont:
	instrLxdI, pSettingsIdxStart,						// initialize parameter index

//loop:
	instrLdRegRdOnlyIndexed, 0x02,						// loop through parameter EEPROM space
	instrStRegEEPROMindexed, 0x02,						// store the program stored parameter defaults into EEPROM
	instrAddIndex, 1,
	instrCmpIndex, pSettingsIdxLen,
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

static const uint8_t prgmInitMPGuinoHardware[] PROGMEM = {
	instrLdRegRdOnly, 0x02, idxCycles0PerSecond,		// load timer0 cycles per second term
	instrMul2byRdOnly, idxSecondsPerHour,				// term is now timer0 cycles per hour
	instrMul2byRdOnly, idxDecimalPoint,					// term is now (timer0 cycles * decimal format) per hour
	instrDiv2byEEPROM, pPulsesPerDistanceIdx,			// term is now (timer0 cycles * unit distance * decimal format) / (pulse * hour)
	instrStRegVariable, 0x02, m32SpeedFactorIdx,		// save speed factor

	instrDiv2byEEPROM, pMinGoodSpeedidx,				// divide speed factor by (unit distance * decimal format) / (hour)
	instrStRegVariable, 0x02, v32MaximumVSSperiodIdx,	// store maximum good VSS period of timer0 cycles / VSS pulse
	instrDiv2byRdOnly, idxCycles0PerTick,				// perform conversion, term is now in timer0 ticks
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, v16DetectVehicleStopIdx,	// store minimum good vehicle speed of timer0 ticks / VSS pulse

	instrLdRegEEPROM, 0x02, pMinGoodRPMidx,				// load minimum good engine speed value in (crank revolutions) / (minute)
	instrMul2byEEPROM, pInjPer2CrankRevIdx,				// multiply by the number of (injector fire event) / (2)(crank revolutions)
	instrLdReg, 0x21,									// move denominator (injector fire event) / (2)(minute) to register 1
	instrLdRegRdOnly, 0x02, idxCycles0PerSecond,		// load (timer0 cycles) / (second) term
	instrMul2byByte, 120,								// term is now in (2)(timer0 cycles) / (minute)
	instrDiv2by1,										// divide by minimum good engine speed value in (injector fire event) / (2)(minute)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, v32MaximumEnginePeriodIdx,	// store maximum good engine period value in (timer0 cycles) / (injector fire event)
	instrDiv2byRdOnly, idxCycles0PerTick,				// perform conversion, term is now in timer0 ticks
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, v16DetectEngineOffIdx,	// store minimum good engine speed value in timer0 ticks / fire event

	instrLdRegEEPROM, 0x02, pInjectorSettleTimeIdx,		// fetch injector settle time in microseconds
	instrMul2byRdOnly, idxCycles0PerSecond,				// multiply by timer0 cycles / second term
	instrDiv2byRdOnly, idxMicroSecondsPerSecond,		// divide by microseconds per seconds value
	instrStRegVariable, 0x02, v32InjectorOpenDelayIdx,	// store injector settle time value in cycles
	instrLdReg, 0x23,									// save injector settle time value in register 3
	instrLdRegVariable, 0x02, v32MaximumEnginePeriodIdx,	// load maximum good engine period value in timer0 cycles / fire event
	instrSubYfromX, 0x32,								// subtract injector settle time from maximum good engine period
	instrStRegVariable, 0x02, v32InjectorValidMaxWidthIdx,	// store maximum valid fuel injector pulse width in timer0 cycles

	instrLdRegEEPROM, 0x02, pIdleTimeoutIdx,			// load idle timeout value in seconds
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v16VehicleStopTimeoutIdx,	// store idle timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pEOCtimeoutIdx,				// load EOC timeout value in seconds
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v16EngineOffTimeoutIdx,	// store EOC timeout value in timer0 ticks

	instrLdRegEEPROM, 0x02, pButtonTimeoutIdx,			// load button press timeout stored parameter
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v16InputTimeoutIdx,		// store button press timeout timer ticks value

	instrLdRegEEPROM, 0x02, pParkTimeoutIdx,			// load parking timeout stored parameter
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v16ParkTimeoutIdx,		// store parking timeout timer ticks value

	instrLdRegEEPROM, 0x02, pActivityTimeoutIdx,		// load activity timeout stored parameter
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v16ActivityTimeoutIdx,	// store activity timeout timer ticks value

	instrLdRegEEPROM, 0x02, pVSSpauseIdx,				// load stored parameter VSS debounce timeout value
	instrStRegVariable, 0x02, v8VSSdebounceTickIdx,		// save as variable

#if defined(useChryslerMAPCorrection)
	instrLdRegEEPROM, 0x02, pMAPsensorFloorIdx,			// convert pressure sensor voltage floor to equivalent ADC floor value
	instrMul2byRdOnly, idxNumerVoltage,
	instrDiv2byRdOnly, idxDenomVoltage,
	instrStRegVariable, 0x02, m32AnalogMAPfloorIdx,		// store it in variable storage

	instrLdRegEEPROM, 0x02, pMAPsensorCeilingIdx,		// calculate pressure sensor ADC step slope denominator
	instrLdRegEEPROM, 0x01, pMAPsensorFloorIdx,
	instrSubYfromX, 0x12,
	instrMul2byRdOnly, idxNumerVoltage,
	instrStRegVariable, 0x02, m32AnalogMAPdenomIdx,		// store it in variable storage

	instrLdRegEEPROM, 0x02, pMAPsensorRangeIdx,
	instrMul2byRdOnly, idxDenomVoltage,					// calculate pressure sensor ADC step slope numerator
	instrStRegVariable, 0x02, m32AnalogMAPnumerIdx,		// store it in variable storage

#if defined(useChryslerBaroSensor)
	instrLdRegEEPROM, 0x02, pBaroSensorFloorIdx,		// convert pressure sensor voltage floor to equivalent ADC floor value
	instrMul2byRdOnly, idxNumerVoltage,
	instrDiv2byRdOnly, idxDenomVoltage,
	instrStRegVariable, 0x02, m32AnalogBaroFloorIdx,	// store it in variable storage

	instrLdRegEEPROM, 0x02, pBaroSensorCeilingIdx,		// calculate pressure sensor ADC step slope numerator
	instrLdRegEEPROM, 0x01, pBaroSensorFloorIdx,
	instrSubYfromX, 0x12,
	instrMul2byRdOnly, idxNumerVoltage,
	instrStRegVariable, 0x02, m32AnalogBaroDenomIdx,	// store it in variable storage

	instrLdRegRdOnly, 0x02, idxDenomVoltage,			// calculate pressure sensor ADC step slope numerator
	instrMul2byEEPROM, pBaroSensorRangeIdx,
	instrStRegVariable, 0x02, m32AnalogBaroNumerIdx,	// store it in variable storage
#else // defined(useChryslerBaroSensor)

	instrLdRegEEPROM, 0x02, pBarometricPressureIdx,		// fetch reference barometric pressure value in psig * 1000
	instrStRegVariable, 0x02, m32BaroPressureIdx,		// store it in variable storage
#endif // defined(useChryslerBaroSensor)
	instrLdRegEEPROM, 0x02, pSysFuelPressureIdx,		// fetch base fuel pressure value in psig * 1000
	instrStRegVariable, 0x02, m32FuelPressureIdx,		// save base fuel pressure value in psig * 1000

	instrLdRegRdOnly, 0x02, idxCorrectionFactor,
	instrStRegVariable, 0x02, v32InjectorCorrectionIdx,	// save initial injector correction index for pressure differential calculation

#endif	// defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsTime)
	instrLdRegEEPROM, 0x02, pFEvsTimeIdx,				// load fuel econ vs time period stored parameter
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v32FEvsTimePeriodTimeoutIdx,	// store fuel econ vs time period timer ticks value

#endif // defined(useBarFuelEconVsTime)
#if defined(useDragRaceFunction)
	instrLdRegVariable, 0x02, m32SpeedFactorIdx,		// load speed factor in (timer0 cycles * unit distance * decimal format) / (pulse * hour)
	instrMul2byByte, 2,									// term is now (2 * timer0 cycles * unit distance * decimal format) / (pulse * hour)
	instrDiv2byEEPROM, pDragSpeedIdx,					// divide speed factor by (unit distance * decimal format) / (hour)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, v32AccelHalfPeriodIdx,	// term is now (2 * timer0 cycles) / (pulse)
	instrDiv2byByte, 2,									// term is now (timer0 cycles) / (pulse)
	instrStRegVariable, 0x02, v32AccelFullPeriodIdx,	// save result to accel test full-speed period tripwire variable

	instrLdRegEEPROM, 0x02, pPulsesPerDistanceIdx,		// fetch drag function distance parameter value in VSS pulses
	instrMul2byEEPROM, pDragDistanceIdx,				// multiply by drag function distance parameter value in unit distance
	instrDiv2byRdOnly, idxDecimalPoint,					// get rid of decimal formatting factor
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, v32AccelDistanceValueIdx,	// save it to accel test distanct tripwire variable

#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	instrLdRegEEPROM, 0x02, pCoastdownSamplePeriodIdx,	// coastdown timer ticks value
	instrMul2byRdOnly, idxTicksPerSecond,				// multiply by timer0 ticks / second term
	instrStRegVariable, 0x02, v32CoastdownPeriodIdx,	// store coastdown timeout timer ticks value

#endif	// defined(useCoastDownCalculator)
	instrDone											// exit to caller
};

static const uint8_t prgmInitMPGuinoSoftware[] PROGMEM = {
#if defined(useBarFuelEconVsSpeed)
	instrLdRegEEPROM, 0x02, pBarLowSpeedCutoffIdx,		// obtain low-speed cutoff parameter in (distance)(* 1000) / (hour)
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulses)(* 1000) / (hour)
	instrDiv2byRdOnly, idxSecondsPerHour,				// term is now (VSS pulses)(* 1000) / (second)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, m32FEvsSpeedMinThresholdIdx,	// store minimum threshold speed in (VSS pulses)(* 1000) / (second)

	instrLdRegEEPROM, 0x02, pBarSpeedQuantumIdx,		// fetch speed quantum parameter in (distance)(* 1000) / hour
	instrMul2byEEPROM, pPulsesPerDistanceIdx,			// term is now (VSS pulses)(* 1000) / (hour)
	instrDiv2byRdOnly, idxSecondsPerHour,				// term is now (VSS pulses)(* 1000) / (second)
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, m32FEvsSpeedQuantumIdx,	// store speed quantum in (VSS pulses)(* 1000) / (second)

#endif // defined(useBarFuelEconVsSpeed)
	instrLdRegEEPROM, 0x02, pMicroSecondsPerGallonIdx,	// fetch injector cycle time in microseconds per US gallon
#ifdef useImperialGallon
	instrBranchIfMetricMode, 5,							// if metric mode set, skip ahead to cycle0s conversion
	instrMul2byRdOnly, idxNumerImperialGallon,			// perform conversion from microseconds per liter into microseconds per Imperial gallon
	instrDiv2byRdOnly, idxDenomImperialGallon,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont0:
#else	// useImperialGallon
	instrBranchIfSAEmode, 5,							// if metric mode not set, skip ahead to cycle0s conversion
#endif	// useImperialGallon
	instrMul2byRdOnly, idxDenomVolume,					// perform conversion from microseconds per US gallon into microseconds per liter
	instrDiv2byRdOnly, idxNumerVolume,
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)

//tankcont:
	instrMul2byRdOnly, idxCycles0PerSecond,				// multiply to get cycle0s-microseconds per unit volume-second value
	instrDiv2byRdOnly, idxMicroSecondsPerSecond,		// convert to get cycle0s per unit volume value
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, m32CyclesPerVolumeIdx,	// save cycle0s per unit volume value

	instrLdRegEEPROM, 0x02, pTankSizeIdx,				// fetch tank size in unit volume * formatting factor
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycle0s per unit volume value to get tank size in cycle0s * formatting factor
	instrDiv2byRdOnly, idxDecimalPoint,					// remove formatting factor to get tank size in cycle0s
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, m64TankSizeIdx,			// save tank size in cycle0s

	instrLdRegEEPROM, 0x02, pTankBingoSizeIdx,			// fetch bingo tank size in unit volume
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycle0s per unit volume value to get tank size in cycle0s * formatting factor
	instrDiv2byRdOnly, idxDecimalPoint,					// remove formatting factor to get bingo tank size in cycle0s
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrStRegVariable, 0x02, m64BingoTankSizeIdx,		// save bingo tank size in cycle0s

#if defined(useCPUreading) || defined(useDebugCPUreading)
	instrLdRegByte, 0x02, 0,
	instrStRegVariable, 0x02, m32CPUworkingMainLoopIdx,	// initialize the cpu utilization stopwatch timer values
	instrStRegVariable, 0x02, m32CPUsampledMainLoopIdx,

	instrStRegVariable, 0x02, m32CPUworkingMainProcessIdx,
	instrStRegVariable, 0x02, m32CPUsampledMainProcessIdx,

	instrStRegVariable, 0x02, m32CPUworkingIdleProcessIdx,
	instrStRegVariable, 0x02, m32CPUsampledIdleProcessIdx,

#if defined(useDebugCPUreading)
	instrStRegVariable, 0x02, m32DbgWorkingMainDevicesIdx,
	instrStRegVariable, 0x02, m32DbgSampledMainDevicesIdx,

	instrStRegVariable, 0x02, m32DbgWorkingMainActivityIdx,
	instrStRegVariable, 0x02, m32DbgSampledMainActivityIdx,

	instrStRegVariable, 0x02, m32DbgWorkingMainSampleIdx,
	instrStRegVariable, 0x02, m32DbgSampledMainSampleIdx,

	instrStRegVariable, 0x02, m32DbgWorkingMainOutputIdx,
	instrStRegVariable, 0x02, m32DbgSampledMainOutputIdx,

	instrStRegVariable, 0x02, m32DbgWorkingMainOtherIdx,
	instrStRegVariable, 0x02, m32DbgSampledMainOtherIdx,

	instrStRegVariable, 0x02, m32DbgWorkingS64processIdx,
	instrStRegVariable, 0x02, m32DbgSampledS64processIdx,

	instrStRegVariable, 0x02, v32WorkingInterruptProcessIdx,
	instrStRegVariable, 0x02, m32DbgSampledInterruptProcessIdx,

	instrStRegVariable, 0x02, m32DebugAccS64multIdx,
	instrStRegVariable, 0x02, m32DebugCountS64multIdx,
	instrStRegVariable, 0x02, m32DebugAccS64divIdx,
	instrStRegVariable, 0x02, m32DebugCountS64divIdx,
#if defined(useIsqrt)
	instrStRegVariable, 0x02, m32DebugAccS64sqrtIdx,
	instrStRegVariable, 0x02, m32DebugCountS64sqrtIdx,
#endif // defined(useIsqrt)
#endif // defined(useDebugCPUreading)

#endif	// defined(useCPUreading) || defined(useDebugCPUreading)
	instrDone											// exit to caller
};

static const uint8_t prgmDoEEPROMmetricConversion[] PROGMEM = {
	instrLdRegRdOnlyMetric, 0x12, idxNumerDistance,		// convert minimum good speed value in (distance) / (time)
	instrLdRegRdOnlyMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pMinGoodSpeedidx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pMinGoodSpeedidx,

	instrLdRegRdOnlyMetric, 0x12, idxDenomDistance,		// convert pulses per distance value in (count) / (distance)
	instrLdRegRdOnlyMetric, 0x21, idxNumerDistance,
	instrMul2byEEPROM, pPulsesPerDistanceIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pPulsesPerDistanceIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerVolume,		// convert total tank size value in (volume)
	instrLdRegRdOnlyMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pTankSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pTankSizeIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerVolume,		// convert bingo tank size value in (volume)
	instrLdRegRdOnlyMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pTankBingoSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pTankBingoSizeIdx,

#if defined(usePartialRefuel)
	instrLdRegRdOnlyMetric, 0x12, idxNumerVolume,		// convert refuel quantity value in (volume)
	instrLdRegRdOnlyMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pRefuelSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pRefuelSizeIdx,

#if defined(useEEPROMtripStorage)
	instrLdRegRdOnlyMetric, 0x12, idxNumerVolume,		// convert saved refuel quantity value in (volume)
	instrLdRegRdOnlyMetric, 0x21, idxDenomVolume,
	instrMul2byEEPROM, pRefuelSaveSizeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pRefuelSaveSizeIdx,

#endif // defined(useEEPROMtripStorage)
#endif // defined(usePartialRefuel)
#if defined(useFuelCost)
	instrLdRegRdOnlyMetric, 0x12, idxDenomVolume,		// convert fuel cost value in (cost) / (volume)
	instrLdRegRdOnlyMetric, 0x21, idxNumerVolume,
	instrMul2byEEPROM, pCostPerQuantity,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pCostPerQuantity,

#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert MAP sensor pressure range value in (force) / (area * volt)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pMAPsensorRangeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pMAPsensorRangeIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert MAP sensor pressure offset value in (force) / (area * volt)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pMAPsensorOffsetIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pMAPsensorOffsetIdx,

#if defined(useChryslerBaroSensor)
	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert barometric sensor pressure range value in (force) / (area * volt)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pBaroSensorRangeIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBaroSensorRangeIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert barometric sensor pressure offset value in (force) / (area * volt)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pBaroSensorOffsetIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBaroSensorOffsetIdx,

#else // defined(useChryslerBaroSensor)
	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert reference barometric pressure value in (force) / (area)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pBarometricPressureIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBarometricPressureIdx,

#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useBarFuelEconVsSpeed)
	instrLdRegRdOnlyMetric, 0x12, idxNumerDistance,		// convert FEvSpeed histograph minimum speed value in (distance) / (time)
	instrLdRegRdOnlyMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pBarLowSpeedCutoffIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBarLowSpeedCutoffIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerDistance,		// convert FEvSpeed histograph bar width value in (distance) / (time)
	instrLdRegRdOnlyMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pBarSpeedQuantumIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pBarSpeedQuantumIdx,

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useVehicleParameters)
	instrLdRegRdOnlyMetric, 0x12, idxNumerMass,			// convert vehicle mass (weight) value in (mass)
	instrLdRegRdOnlyMetric, 0x21, idxDenomMass,
	instrMul2byEEPROM, pVehicleMassIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pVehicleMassIdx,

#if defined(useCoastDownCalculator)
	instrLdRegRdOnlyMetric, 0x12, idxNumerArea,			// convert frontal area value in (area)
	instrLdRegRdOnlyMetric, 0x21, idxDenomArea,
	instrMul2byEEPROM, pVehicleFrontalAreaIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pVehicleFrontalAreaIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerDensity,		// convert vehicle mass (weight) value in (mass) / (volume)
	instrLdRegRdOnlyMetric, 0x21, idxDenomDensity,
	instrMul2byEEPROM, pLocustDensityIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pLocustDensityIdx,

#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	instrLdRegRdOnlyMetric, 0x12, idxNumerDistance,		// convert accel test defined speed value in (distance) / (time)
	instrLdRegRdOnlyMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pDragSpeedIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pDragSpeedIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerDistance,		// convert accel test defined distance value value in (distance)
	instrLdRegRdOnlyMetric, 0x21, idxDenomDistance,
	instrMul2byEEPROM, pDragDistanceIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pDragDistanceIdx,

#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#ifdef useCalculatedFuelFactor
	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert system fuel pressure value in (force) / (area)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pSysFuelPressureIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pSysFuelPressureIdx,

	instrLdRegRdOnlyMetric, 0x12, idxNumerPressure,		// convert reference fuel injector pressure value in (force) / (area)
	instrLdRegRdOnlyMetric, 0x21, idxDenomPressure,
	instrMul2byEEPROM, pRefFuelPressureIdx,
	instrDiv2by1,
	instrAdjustQuotient,
	instrStRegEEPROM, 0x02, pRefFuelPressureIdx,

#endif // useCalculatedFuelFactor
	instrDone											// return to caller
};

#ifdef useCalculatedFuelFactor
static const uint8_t prgmCalculateFuelFactor[] PROGMEM = {
	instrLdRegRdOnly, 0x02, idxCorrectionFactor2,		// obtain reference correction factor
	instrMul2byEEPROM, pSysFuelPressureIdx,				// multiply by this vehicle's stored fuel system absolute pressure
	instrDiv2byEEPROM, pRefFuelPressureIdx,				// divide by the fuel injector reference absolute pressure
	instrIsqrt, 0x02,									// perform square root on result
	instrMul2byEEPROM, pInjectorCountIdx,				// multiply by stored number of fuel injectors for this engine
	instrMul2byEEPROM, pInjectorSizeIdx,				// multiply by injector size in cc/minute * decimal formatting factor (L/min * decimal formatting factor * 1000)
	instrLdReg, 0x21,									// save denominator term for later
	instrLdRegByte, 0x02, 60,							// load seconds per minute into register 2
	instrMul2byRdOnly, idxMicroSecondsPerSecond,		// multiply by microseconds per second into register 1
	instrMul2byRdOnly, idxOneThousand,					// multiply by number of cc's per liter into register 1
	instrMul2byRdOnly, idxDecimalPoint,					// set numerator up to cancel reference correction factor in denominator
	instrMul2byRdOnly, idxCorrectionFactor,				// set numerator up to cancel reference correction factor in denominator
	instrDiv2by1,										// perform comversion for injector flow
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrMul2byRdOnly, idxNumerVolume,					// set up denominator to convert injector flow in liters to US gallons
	instrDiv2byRdOnly, idxDenomVolume,					// perform conversion of injector flow in liters to US gallons
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
	uint8_t i;

	i = readByte(pInjEdgeTriggerIdx) & 0x01;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

#if defined(__AVR_ATmega32U4__)
	EIMSK &= ~((1 << INT3) | (1 << INT2)); // disable fuel injector sense interrupts

	EICRA |= ((1 << ISC31) | (1 << ISC30) | (1 << ISC21) | (1 << ISC20)); // set injector sense pin control
	EICRA &= ~(1 << (i ? ISC30 : ISC20));

	EIFR |= ((1 << INTF3) | (1 << INTF2)); // clear fuel injector sense flag
	EIMSK |= ((1 << INT3) | (1 << INT2)); // enable fuel injector sense interrupts

	PCMSK0 |= (1 << PCINT7); // enable port B VSS pin interrupt

	PCICR |= (1 << PCIE0); // enable selected interrupts on port B

	lastPINxState = PINB;

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	EIMSK &= ~((1 << INT5) | (1 << INT4)); // disable fuel injector sense interrupts

	EICRB |= ((1 << ISC51) | (1 << ISC50) | (1 << ISC41) | (1 << ISC40)); // set injector sense pin control
	EICRB &= ~(1 << (i ? ISC50 : ISC40));

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
	EICRA &= ~(1 << (i ? ISC10 : ISC00));

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

	volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] = asHardwareReady;

#endif // defined(useAnalogRead)
	SWEET64::runPrgm(prgmInitMPGuinoHardware, 0); // calculate multiple MPGuino system values for use within timer0, fuel injector, and VSS interrupts

#if defined(useDragRaceFunction)
	volatile8Variables[(uint16_t)(accelTestClearFlags - v8VariableStartIdx)] &= ~(accelTestClearFlags);

	lastAccelTestStatus = 0;
	accelTestStatus = 0;

#endif // defined(useDragRaceFunction)
#if defined(useBarFuelEconVsTime)
	volatile8Variables[(uint16_t)(v8Timer0CommandIdx - v8VariableStartIdx)] |= (t0cResetFEvTime); // reset fuel economy vs time bargraph mechanism

#endif // defined(useBarFuelEconVsTime)
	SREG = oldSREG; // restore interrupt flag status

}

static void EEPROM::initGuinoSoftware(void)
{

	setMetricDisplayMode();

	SWEET64::runPrgm(prgmInitMPGuinoSoftware, 0); // calculate multiple MPGuino system values for use within code

#if defined(useWindowTripFilter)
	tripSupport::resetWindowFilter();

#endif // defined(useWindowTripFilter)
}

static void EEPROM::setMetricDisplayMode(void)
{

	if (readByte(pMetricModeIdx)) mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] |= (mmFuelEconOutputFlags);
	else mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] &= ~(mmFuelEconOutputFlags);

	if (readByte(pAlternateFEidx)) mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] ^= (mmDisplayAlternateFE);

}

static uint8_t EEPROM::onChange(const uint8_t * sched, uint8_t parameterIdx)
{

	uint8_t retVal;

	retVal = 0;

	mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] &= ~(ecsEEPROMchangeFlags); // clear all bit flags

	SWEET64::runPrgm(sched, parameterIdx); // perform initial SWEET64 call to store EEPROM parameter

#ifdef useCalculatedFuelFactor
	// calculate and store microseconds per US gallon factor (this will trigger ecsDoMPGuinoInitSoftware)
	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsCalculateFuelParam) SWEET64::runPrgm(prgmCalculateFuelFactor, 0);

#endif // useCalculatedFuelFactor
	// perform conversion between metric mode and SAE mode (this will trigger ecsDoMPGuinoInitSoftware)
	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsDoMetricConversion) SWEET64::runPrgm(prgmDoEEPROMmetricConversion, 0);

	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsChangeDisplay)
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
#if defined(useAdafruitRGBLCDdisplay)
		if (parameterIdx == pLCDcolorIdx) LCD::setRGBcolor(readByte(pLCDcolorIdx)); // adjust backlight color

#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
	}

	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsDoMPGuinoInitHardware) initGuinoHardware();

	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsDoMPGuinoInitSoftware) initGuinoSoftware();

#if defined(useBarFuelEconVsSpeed)
	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsResetBarFEvsSpeed) bgFEvsSsupport::reset();

#endif // defined(useBarFuelEconVsSpeed)
	if (mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] & ecsEEPROMchangeDetected) retVal = 1; // if the setting has changed

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
	cli(); // disable interrupts to make the next operations atomic

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
	cli(); // disable interrupts to make the next operations atomic

	b = 0;

	for (uint16_t x = t; x < u; x++)
	{

		eByt = eeprom_read_byte((uint8_t *)(x));
		rByt = an->u8[x - t];
		if (eByt != rByt)
		{

			mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsEEPROMchangeDetected);
			b = 1;
			eeprom_write_byte((uint8_t *)(x), rByt);

		}

	}

	SREG = oldSREG; // restore interrupt flag status

	if (b)
	{

		switch (l & pfBitmask)
		{

			case pfSoftwareInitMPGuino:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsDoMPGuinoInitSoftware);
				break;

			case pfHardwareInitMPGuino:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsDoMPGuinoInitHardware);
				break;

			case pfDoMetricModeConversion:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsDoMetricConversion);
				break;

			case pfChangeDisplay:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsChangeDisplay);
				break;

			case pfCalculateFuelParams:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsCalculateFuelParam);
				break;

#if defined(useBarFuelEconVsSpeed)
			case pfHWresetAndBFEvSreset:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsDoMPGuinoInitHardware);
			case pfSWresetAndBFEvSreset:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsDoMPGuinoInitSoftware | ecsResetBarFEvsSpeed);
				break;

#endif // defined(useBarFuelEconVsSpeed)
#if defined(useChryslerMAPCorrection)
			case pfHWresetAndFuelParamCalc:
				mainProgram8Variables[(uint16_t)(m8EEPROMchangeStatus - m8VariableStartIdx)] |= (ecsDoMPGuinoInitHardware | ecsCalculateFuelParam);
				break;

#endif // defined(useChryslerMAPCorrection)
		}

	}

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

	if (eePtr < eePtrStorageEnd) t = pgm_read_byte(&paramsLength[(uint16_t)(eePtr)]) & pfBitmask;
	else t = 0;

	return t;

}

static uint8_t EEPROM::getLength(uint8_t eePtr)
{

	uint8_t t;
	uint8_t u;
	uint8_t l;

	if (eePtr >= eePtrEnd) eePtr = eePtrEnd;

	if (eePtr < eePtrStorageEnd) l = pgm_read_byte(&paramsLength[(uint16_t)(eePtr)]) & ~(pfBitmask);
	else l = 0;

	t = getAddress(eePtr);
	u = getAddress(++eePtr);
	u -= t;
	if (l) u--;
	l += (u << 3);

	return l;

}

