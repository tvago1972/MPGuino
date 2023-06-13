namespace EEPROM /* EEPROM parameter I/O section prototype */
{

	static uint8_t powerUpCheck(void);
	static void initGuino(void);
	static void initGuinoHardware(void);
	static void initGuinoSoftware(void);
	static uint8_t readByte(uint8_t eePtr);
#if defined(useScreenEditor)
	static uint16_t readWord(uint8_t eePtr);
#endif // defined(useScreenEditor)
	static void writeVal(uint8_t eePtr, uint32_t value);
	static void read64(union union_64 * an, uint8_t parameterIdx);
	static void write64(union union_64 * an, uint8_t parameterIdx);
	static uint16_t getAddress(uint8_t eePtr);
	static uint8_t getParameterFlags(uint8_t eePtr);
	static uint8_t getLength(uint8_t eePtr);

};

const uint8_t guinosig =		0b10110111;

// start of remarkably long EEPROM stored settings section

/* parameter sizes in bits */

const uint8_t pSizeSignature =				24;

// display settings

#ifdef useLegacyLCD
const uint8_t pSizeContrast =				1;
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t pSizeLCDcolor =				3;
#endif // useAdafruitRGBLCDshield
const uint8_t pSizeMetricMode =				1;
const uint8_t pSizeAlternateFE =			1;
#ifdef useFuelCost
const uint8_t pSizeFuelUnitCost =			24;
#endif // useFuelCost
#if defined(useOutputPins)
const uint8_t pSizeOutputPin1Mode =			3;
const uint8_t pSizeOutputPin2Mode =			3;
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
const uint8_t pSizeVoltageOffset =			12;
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
const uint8_t pSizeSerialDataLogging =		1;
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
const uint8_t pSizeFEvsTime =				16;
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pSizeBarLowSpeedCutoff =		24;
const uint8_t pSizeBarSpeedQuantumIdx =		24;
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
const uint8_t pSizeSysFuelPressure =		32;
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pSizeRefFuelPressure =		32;
const uint8_t pSizeInjectorCount =			8;
const uint8_t pSizeInjectorSize =			20;
#endif // useCalculatedFuelFactor
const uint8_t pSizeMicroSecondsPerGallon =	32;
const uint8_t pSizeInjEdgeTrigger =			1;
const uint8_t pSizeInjectorOpeningTime =	16;
const uint8_t pSizeInjectorClosingTime =	16;
const uint8_t pSizeCrankRevPerInj =			8;
const uint8_t pSizeMinGoodRPM =				16;

// vehicle speed sensor settings

const uint8_t pSizePulsesPerDistance =		18;
const uint8_t pSizeVSSpause =				8;
const uint8_t pSizeMinGoodSpeed =			16;
const uint8_t pSizeTankSize =				24;
const uint8_t pSizeTankBingoSize =			pSizeTankSize;
#ifdef useChryslerMAPCorrection

// Chrysler settings

const uint8_t pSizeMAPsensorFloor =			16;
const uint8_t pSizeMAPsensorCeiling =		16;
const uint8_t pSizeMAPsensorRange =			32;
const uint8_t pSizeMAPsensorOffset =		32;
#ifdef useChryslerBaroSensor
const uint8_t pSizeBaroSensorFloor =		16;
const uint8_t pSizeBaroSensorCeiling =		16;
const uint8_t pSizeBaroSensorRange =		32;
const uint8_t pSizeBaroSensorOffset =		32;
#else // useChryslerBaroSensor
const uint8_t pSizeBarometricPressure =		32;
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
const uint8_t pSizeVehicleMass =			16;
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pSizeVehicleFrontalArea =		16;
const uint8_t pSizeLocustDensity =			16;
const uint8_t pSizeCoefficientD =			16;
const uint8_t pSizeCoefficientV =			16;
const uint8_t pSizeCoefficientRR =			16;
const uint8_t pSizeCoastdownSamplePeriod =	8;
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const uint8_t pSizeDragSpeed =				24;
const uint8_t pSizeDragDistance =			16;
const uint8_t pSizeDragAutoFlag =			1;
#endif // useDragRaceFunction

// timeout settings

const uint8_t pSizeIdleTimeout =			8;
const uint8_t pSizeEOCtimeout =				8;
const uint8_t pSizeButtonTimeout =			8;
const uint8_t pSizeParkTimeout =			8;
const uint8_t pSizeActivityTimeout =		8;
const uint8_t pSizeWakeupResetCurrentOnEngine =	1;
const uint8_t pSizeWakeupResetCurrentOnMove =	1;
#ifdef useSavedTrips
const uint8_t pSizeAutoSaveActive =			1;
#endif // useSavedTrips

// miscellaneous settings

const uint8_t pSizeScratchpad =				32;

// settings inaccessible from the menu

#ifdef usePartialRefuel
const uint8_t pSizeRefuelSize =				pSizeTankSize;
#ifdef useEEPROMtripStorage
const uint8_t pSizeRefuelSaveSizeIdx =		pSizeTankSize;
#endif // useEEPROMtripStorage
#endif // usePartialRefuel

#ifdef useEEPROMtripStorage
const uint8_t pSizeCurrTripSignatureIdx =	8;
const uint8_t pSizeTankTripSignatureIdx =	8;
const uint8_t pSizeCurrTripVSSpulseIdx =	32;
const uint8_t pSizeCurrTripVSScycleIdx =	64;
const uint8_t pSizeCurrTripInjPulseIdx =	32;
const uint8_t pSizeCurrTripInjCycleIdx =	64;
const uint8_t pSizeCurrTripEngCycleIdx =	64;
const uint8_t pSizeTankTripVSSpulseIdx =	32;
const uint8_t pSizeTankTripVSScycleIdx =	64;
const uint8_t pSizeTankTripInjPulseIdx =	32;
const uint8_t pSizeTankTripInjCycleIdx =	64;
const uint8_t pSizeTankTripEngCycleIdx =	64;
#if defined(trackIdleEOCdata)
const uint8_t pSizeCurrIEOCvssPulseIdx =	32;
const uint8_t pSizeCurrIEOCvssCycleIdx =	64;
const uint8_t pSizeCurrIEOCinjPulseIdx =	32;
const uint8_t pSizeCurrIEOCinjCycleIdx =	64;
const uint8_t pSizeCurrIEOCengCycleIdx =	64;
const uint8_t pSizeTankIEOCvssPulseIdx =	32;
const uint8_t pSizeTankIEOCvssCycleIdx =	64;
const uint8_t pSizeTankIEOCinjPulseIdx =	32;
const uint8_t pSizeTankIEOCinjCycleIdx =	64;
const uint8_t pSizeTankIEOCengCycleIdx =	64;
#endif // defined(trackIdleEOCdata)
#endif // useEEPROMtripStorage

#define byteSize(bitLength) ((((bitLength & 0x07) != 0)? 1 : 0) + (bitLength / 8))

/* parameter addresses from sizes defined above */

const uint8_t eeAdrSignature = 0;

#define nextAllowedValue eeAdrSignature
const unsigned int pAddressSignature =					nextAllowedValue;
#define nextAllowedValue pAddressSignature + byteSize(pSizeSignature)

// display settings

#ifdef useLegacyLCD
const unsigned int pAddressContrast =					nextAllowedValue;
#define nextAllowedValue pAddressContrast + byteSize(pSizeContrast)
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const unsigned int pAddressLCDcolor =					nextAllowedValue;
#define nextAllowedValue pAddressLCDcolor + byteSize(pSizeLCDcolor)
#endif // useAdafruitRGBLCDshield
const unsigned int pAddressMetricMode =					nextAllowedValue;
const unsigned int pAddressAlternateFE =				pAddressMetricMode + byteSize(pSizeMetricMode);
#define nextAllowedValue pAddressAlternateFE + byteSize(pSizeAlternateFE)
#ifdef useFuelCost
const unsigned int pAddressFuelUnitCost =				nextAllowedValue;
#define nextAllowedValue pAddressFuelUnitCost + byteSize(pSizeFuelUnitCost)
#endif // useFuelCost
#if defined(useOutputPins)
const uint8_t pAddressOutputPin1Mode =					nextAllowedValue;
const uint8_t pAddressOutputPin2Mode =					pAddressOutputPin1Mode + byteSize(pSizeOutputPin1Mode);
#define nextAllowedValue pAddressOutputPin2Mode + byteSize(pSizeOutputPin2Mode)
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
const unsigned int pAddressVoltageOffset =				nextAllowedValue;
#define nextAllowedValue pAddressVoltageOffset + byteSize(pSizeVoltageOffset)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
const unsigned int pAddressSerialDataLogging =			nextAllowedValue;
#define nextAllowedValue pAddressSerialDataLogging + byteSize(pSizeSerialDataLogging)
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
const unsigned int pAddressFEvsTime =					nextAllowedValue;
#define nextAllowedValue pAddressFEvsTime + byteSize(pSizeFEvsTime)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const unsigned int pAddressBarLowSpeedCutoff =			nextAllowedValue;
const unsigned int pAddressBarSpeedQuantumIdx =			pAddressBarLowSpeedCutoff + byteSize(pSizeBarLowSpeedCutoff);
#define nextAllowedValue pAddressBarSpeedQuantumIdx + byteSize(pSizeBarSpeedQuantumIdx)
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
const unsigned int pAddressSysFuelPressure =			nextAllowedValue;
#define nextAllowedValue pAddressSysFuelPressure + byteSize(pSizeSysFuelPressure)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const unsigned int pAddressRefFuelPressure =			nextAllowedValue;
const unsigned int pAddressInjectorCount =				pAddressRefFuelPressure + byteSize(pSizeRefFuelPressure);
const unsigned int pAddressInjectorSize =				pAddressInjectorCount + byteSize(pSizeInjectorCount);
#define nextAllowedValue pAddressInjectorSize + byteSize(pSizeInjectorSize)
#endif // useCalculatedFuelFactor
const unsigned int pAddressMicroSecondsPerGallon =		nextAllowedValue;
const unsigned int pAddressInjEdgeTrigger =				pAddressMicroSecondsPerGallon + byteSize(pSizeMicroSecondsPerGallon);
const unsigned int pAddressInjectorOpeningTime =		pAddressInjEdgeTrigger + byteSize(pSizeInjEdgeTrigger);
const unsigned int pAddressInjectorClosingTime =		pAddressInjectorOpeningTime + byteSize(pSizeInjectorOpeningTime);
const unsigned int pAddressCrankRevPerInj =				pAddressInjectorClosingTime + byteSize(pSizeInjectorClosingTime);
const unsigned int pAddressMinGoodRPM =					pAddressCrankRevPerInj + byteSize(pSizeCrankRevPerInj);
#define nextAllowedValue pAddressMinGoodRPM + byteSize(pSizeMinGoodRPM)

// vehicle speed sensor settings

const unsigned int pAddressPulsesPerDistance =			nextAllowedValue;
const unsigned int pAddressVSSpause =					pAddressPulsesPerDistance + byteSize(pSizePulsesPerDistance);
const unsigned int pAddressMinGoodSpeed =				pAddressVSSpause + byteSize(pSizeVSSpause);
#define nextAllowedValue pAddressMinGoodSpeed + byteSize(pSizeMinGoodSpeed)

// fuel tank size settings

const unsigned int pAddressTankSize =					nextAllowedValue;
const unsigned int pAddressTankBingoSize =				pAddressTankSize + byteSize(pSizeTankSize);
#define nextAllowedValue pAddressTankBingoSize + byteSize(pSizeTankBingoSize)
#ifdef useChryslerMAPCorrection

// Chrysler settings

const unsigned int pAddressMAPsensorFloor =				nextAllowedValue;
const unsigned int pAddressMAPsensorCeiling =			pAddressMAPsensorFloor + byteSize(pSizeMAPsensorFloor);
const unsigned int pAddressMAPsensorRange =				pAddressMAPsensorCeiling + byteSize(pSizeMAPsensorCeiling);
const unsigned int pAddressMAPsensorOffset =			pAddressMAPsensorRange + byteSize(pSizeMAPsensorRange);
#define nextAllowedValue pAddressMAPsensorOffset + byteSize(pSizeMAPsensorOffset)
#ifdef useChryslerBaroSensor
const unsigned int pAddressBaroSensorFloor =			nextAllowedValue;
const unsigned int pAddressBaroSensorCeiling =			pAddressBaroSensorFloor + byteSize(pSizeBaroSensorFloor);
const unsigned int pAddressBaroSensorRange =			pAddressBaroSensorCeiling + byteSize(pSizeBaroSensorCeiling);
const unsigned int pAddressBaroSensorOffset =			pAddressBaroSensorRange + byteSize(pSizeBaroSensorRange);
#define nextAllowedValue pAddressBaroSensorOffset + byteSize(pSizeBaroSensorOffset)
#else // useChryslerBaroSensor
const unsigned int pAddressBarometricPressure =			nextAllowedValue;
#define nextAllowedValue pAddressBarometricPressure + byteSize(pSizeBarometricPressure)
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
const unsigned int pAddressVehicleMass =				nextAllowedValue;
#define nextAllowedValue pAddressVehicleMass + byteSize(pSizeVehicleMass)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const unsigned int pAddressVehicleFrontalArea =			nextAllowedValue;
const unsigned int pAddressLocustDensity =				pAddressVehicleFrontalArea + byteSize(pSizeVehicleFrontalArea);
const unsigned int pAddressCoefficientD =				pAddressLocustDensity + byteSize(pSizeLocustDensity);
const unsigned int pAddressCoefficientV =				pAddressCoefficientD + byteSize(pSizeCoefficientD);
const unsigned int pAddressCoefficientRR =				pAddressCoefficientV + byteSize(pSizeCoefficientV);
const unsigned int pAddressCoastdownSamplePeriod =		pAddressCoefficientRR + byteSize(pSizeCoefficientRR);
#define nextAllowedValue pAddressCoastdownSamplePeriod + byteSize(pSizeCoastdownSamplePeriod)
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const unsigned int pAddressDragSpeed =					nextAllowedValue;
const unsigned int pAddressDragDistance =				pAddressDragSpeed + byteSize(pSizeDragSpeed);
const unsigned int pAddressDragAutoFlag =				pAddressDragDistance + byteSize(pSizeDragDistance);
#define nextAllowedValue pAddressDragAutoFlag + byteSize(pSizeDragAutoFlag)
#endif // useDragRaceFunction

// timeout settings

const unsigned int pAddressIdleTimeout =				nextAllowedValue;
const unsigned int pAddressEOCtimeout =					pAddressIdleTimeout + byteSize(pSizeIdleTimeout);
const unsigned int pAddressButtonTimeout =				pAddressEOCtimeout + byteSize(pSizeEOCtimeout);
const unsigned int pAddressParkTimeout =				pAddressButtonTimeout + byteSize(pSizeButtonTimeout);
const unsigned int pAddressActivityTimeout =			pAddressParkTimeout + byteSize(pSizeParkTimeout);
const unsigned int pAddressWakeupResetCurrentOnEngine =	pAddressActivityTimeout + byteSize(pSizeActivityTimeout);
const unsigned int pAddressWakeupResetCurrentOnMove =	pAddressWakeupResetCurrentOnEngine + byteSize(pSizeWakeupResetCurrentOnEngine);
#define nextAllowedValue pAddressWakeupResetCurrentOnMove + byteSize(pSizeWakeupResetCurrentOnMove)
#ifdef useSavedTrips
const unsigned int pAddressAutoSaveActive =				nextAllowedValue;
#define nextAllowedValue pAddressAutoSaveActive + byteSize(pSizeAutoSaveActive)
#endif // useSavedTrips

// miscellaneous settings

const unsigned int pAddressScratchpad =					nextAllowedValue;
#define nextAllowedValue pAddressScratchpad + byteSize(pSizeScratchpad)

// settings inaccessible from the menu

#ifdef usePartialRefuel
const unsigned int pAddressRefuelSize =					nextAllowedValue;
#define nextAllowedValue pAddressRefuelSize + byteSize(pSizeRefuelSize)
#ifdef useEEPROMtripStorage
const uint8_t pAddressRefuelSaveSizeIdx =				nextAllowedValue;
#define nextAllowedValue pAddressRefuelSaveSizeIdx + byteSize(pSizeRefuelSaveSizeIdx)
#endif // useEEPROMtripStorage
#endif // usePartialRefuel

#ifdef useEEPROMtripStorage
const uint8_t pAddressCurrTripSignatureIdx =			nextAllowedValue;
const uint8_t pAddressTankTripSignatureIdx =			pAddressCurrTripSignatureIdx + byteSize(pSizeCurrTripSignatureIdx);
const uint8_t pAddressCurrTripVSSpulseIdx =				pAddressTankTripSignatureIdx + byteSize(pSizeTankTripSignatureIdx);
const uint8_t pAddressCurrTripVSScycleIdx =				pAddressCurrTripVSSpulseIdx + byteSize(pSizeCurrTripVSSpulseIdx);
const uint8_t pAddressCurrTripInjPulseIdx =				pAddressCurrTripVSScycleIdx + byteSize(pSizeCurrTripVSScycleIdx);
const uint8_t pAddressCurrTripInjCycleIdx =				pAddressCurrTripInjPulseIdx + byteSize(pSizeCurrTripInjPulseIdx);
const uint8_t pAddressCurrTripEngCycleIdx =				pAddressCurrTripInjCycleIdx + byteSize(pSizeCurrTripInjCycleIdx);
const uint8_t pAddressTankTripVSSpulseIdx =				pAddressCurrTripEngCycleIdx + byteSize(pSizeCurrTripEngCycleIdx);
const uint8_t pAddressTankTripVSScycleIdx =				pAddressTankTripVSSpulseIdx + byteSize(pSizeTankTripVSSpulseIdx);
const uint8_t pAddressTankTripInjPulseIdx =				pAddressTankTripVSScycleIdx + byteSize(pSizeTankTripVSScycleIdx);
const uint8_t pAddressTankTripInjCycleIdx =				pAddressTankTripInjPulseIdx + byteSize(pSizeTankTripInjPulseIdx);
const uint8_t pAddressTankTripEngCycleIdx =				pAddressTankTripInjCycleIdx + byteSize(pSizeTankTripInjCycleIdx);
#define nextAllowedValue pAddressTankTripEngCycleIdx + byteSize(pSizeTankTripEngCycleIdx)
#if defined(trackIdleEOCdata)
const uint8_t pAddressCurrIEOCvssPulseIdx =				nextAllowedValue;
const uint8_t pAddressCurrIEOCvssCycleIdx =				pAddressCurrIEOCvssPulseIdx + byteSize(pSizeCurrIEOCvssPulseIdx);
const uint8_t pAddressCurrIEOCinjPulseIdx =				pAddressCurrIEOCvssCycleIdx + byteSize(pSizeCurrIEOCvssCycleIdx);
const uint8_t pAddressCurrIEOCinjCycleIdx =				pAddressCurrIEOCinjPulseIdx + byteSize(pSizeCurrIEOCinjPulseIdx);
const uint8_t pAddressCurrIEOCengCycleIdx =				pAddressCurrIEOCinjCycleIdx + byteSize(pSizeCurrIEOCinjCycleIdx);
const uint8_t pAddressTankIEOCvssPulseIdx =				pAddressCurrIEOCengCycleIdx + byteSize(pSizeCurrIEOCengCycleIdx);
const uint8_t pAddressTankIEOCvssCycleIdx =				pAddressTankIEOCvssPulseIdx + byteSize(pSizeTankIEOCvssPulseIdx);
const uint8_t pAddressTankIEOCinjPulseIdx =				pAddressTankIEOCvssCycleIdx + byteSize(pSizeTankIEOCvssCycleIdx);
const uint8_t pAddressTankIEOCinjCycleIdx =				pAddressTankIEOCinjPulseIdx + byteSize(pSizeTankIEOCinjPulseIdx);
const uint8_t pAddressTankIEOCengCycleIdx =				pAddressTankIEOCinjCycleIdx + byteSize(pSizeTankIEOCinjCycleIdx);
#define nextAllowedValue pAddressTankIEOCengCycleIdx + byteSize(pSizeTankIEOCengCycleIdx)
#endif // defined(trackIdleEOCdata)
#endif // useEEPROMtripStorage

const unsigned int eeAdrSettingsEnd =					nextAllowedValue;

#if defined(useScreenEditor)
const unsigned int eeAdrScreensStart =					nextAllowedValue;
const unsigned int eeAdrScreensEnd =					eeAdrScreensStart + 2 * mainDisplayFormatSize;
#define nextAllowedValue eeAdrScreensEnd

#endif // defined(useScreenEditor)
const unsigned int eeAdrStorageEnd =					nextAllowedValue;

#define nextAllowedValue 0
const uint8_t pSignatureIdx =			nextAllowedValue;
#define nextAllowedValue pSignatureIdx + 1

/* parameter indexes corresponding to sizes and addresses defined above */

// this is the start of all EEPROM parameters that show up in the general settings screen

const uint8_t eePtrSettingsStart =			nextAllowedValue;

// display settings

const uint8_t eePtrSettingsDispStart =		nextAllowedValue;

#ifdef useLegacyLCD
const uint8_t pContrastIdx =				nextAllowedValue;
#define nextAllowedValue pContrastIdx + 1
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t pLCDcolorIdx =				nextAllowedValue;
#define nextAllowedValue pLCDcolorIdx + 1
#endif // useAdafruitRGBLCDshield
const uint8_t pMetricModeIdx =				nextAllowedValue;
const uint8_t pAlternateFEidx =				pMetricModeIdx + 1;
#define nextAllowedValue pAlternateFEidx + 1
#ifdef useFuelCost
const uint8_t pCostPerQuantity =			nextAllowedValue;
#define nextAllowedValue pCostPerQuantity + 1
#endif // useFuelCost
#if defined(useOutputPins)
const uint8_t pOutputPin1Mode =				nextAllowedValue;
const uint8_t pOutputPin2Mode =				pOutputPin1Mode + 1;
#define nextAllowedValue pOutputPin2Mode + 1
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
const uint8_t pVoltageOffset =				nextAllowedValue;
#define nextAllowedValue pVoltageOffset + 1
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
const uint8_t pSerialDataLoggingIdx =		nextAllowedValue;
#define nextAllowedValue pSerialDataLoggingIdx + 1
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
const uint8_t pFEvsTimeIdx =				nextAllowedValue;
#define nextAllowedValue pFEvsTimeIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
const uint8_t pBarLowSpeedCutoffIdx =		nextAllowedValue;
const uint8_t pBarSpeedQuantumIdx =			pBarLowSpeedCutoffIdx + 1;
#define nextAllowedValue pBarSpeedQuantumIdx + 1
#endif // useBarFuelEconVsSpeed

const uint8_t eePtrSettingsDispEnd =		nextAllowedValue;
const uint8_t eePtrSettingsDispLen =		eePtrSettingsDispEnd - eePtrSettingsDispStart;

// fuel injection settings

const uint8_t eePtrSettingsInjStart =		nextAllowedValue;

#ifdef useIsqrt
const uint8_t pSysFuelPressureIdx =			nextAllowedValue;
#define nextAllowedValue pSysFuelPressureIdx + 1
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
const uint8_t pRefFuelPressureIdx =			nextAllowedValue;
const uint8_t pInjectorCountIdx =			pRefFuelPressureIdx + 1;
const uint8_t pInjectorSizeIdx =			pInjectorCountIdx + 1;
#define nextAllowedValue pInjectorSizeIdx + 1
#endif // useCalculatedFuelFactor
const uint8_t pMicroSecondsPerGallonIdx =	nextAllowedValue;
const uint8_t pInjEdgeTriggerIdx =			pMicroSecondsPerGallonIdx + 1;
const uint8_t pInjectorOpeningTimeIdx =		pInjEdgeTriggerIdx + 1;
const uint8_t pInjectorClosingTimeIdx =		pInjectorOpeningTimeIdx + 1;
const uint8_t pCrankRevPerInjIdx =			pInjectorClosingTimeIdx + 1;
const uint8_t pMinGoodRPMidx =				pCrankRevPerInjIdx + 1;
#define nextAllowedValue pMinGoodRPMidx + 1

const uint8_t eePtrSettingsInjEnd =			nextAllowedValue;
const uint8_t eePtrSettingsInjLen =			eePtrSettingsInjEnd - eePtrSettingsInjStart;

// vehicle speed sensor settings

const uint8_t eePtrSettingsVSSstart =		nextAllowedValue;

const uint8_t pPulsesPerDistanceIdx =		nextAllowedValue;
const uint8_t pVSSpauseIdx =				pPulsesPerDistanceIdx + 1;
const uint8_t pMinGoodSpeedidx =			pVSSpauseIdx + 1;
#define nextAllowedValue pMinGoodSpeedidx + 1

const uint8_t eePtrSettingsVSSend =			nextAllowedValue;
const uint8_t eePtrSettingsVSSlen =			eePtrSettingsVSSend - eePtrSettingsVSSstart;

// fuel tank size settings

const uint8_t eePtrSettingsTankStart =		nextAllowedValue;

const uint8_t pTankSizeIdx =				nextAllowedValue;
const uint8_t pTankBingoSizeIdx =			pTankSizeIdx + 1;
#define nextAllowedValue pTankBingoSizeIdx + 1

const uint8_t eePtrSettingsTankEnd =		nextAllowedValue;
const uint8_t eePtrSettingsTankLen =		eePtrSettingsTankEnd - eePtrSettingsTankStart;
#ifdef useChryslerMAPCorrection

// chrysler settings

const uint8_t eePtrSettingsCRFICstart =		nextAllowedValue;

const uint8_t pMAPsensorFloorIdx =			nextAllowedValue;
const uint8_t pMAPsensorCeilingIdx =		pMAPsensorFloorIdx + 1;
const uint8_t pMAPsensorRangeIdx =			pMAPsensorCeilingIdx + 1;
const uint8_t pMAPsensorOffsetIdx =			pMAPsensorRangeIdx + 1;
#define nextAllowedValue pMAPsensorOffsetIdx + 1
#ifdef useChryslerBaroSensor
const uint8_t pBaroSensorFloorIdx =			nextAllowedValue;
const uint8_t pBaroSensorCeilingIdx =		pBaroSensorFloorIdx + 1;
const uint8_t pBaroSensorRangeIdx =			pBaroSensorCeilingIdx + 1;
const uint8_t pBaroSensorOffsetIdx =		pBaroSensorRangeIdx + 1;
#define nextAllowedValue pBaroSensorOffsetIdx + 1
#else // useChryslerBaroSensor
const uint8_t pBarometricPressureIdx =		nextAllowedValue;
#define nextAllowedValue pBarometricPressureIdx + 1
#endif // useChryslerBaroSensor

const uint8_t eePtrSettingsCRFICend =		nextAllowedValue;
const uint8_t eePtrSettingsCRFIClen =		eePtrSettingsCRFICend - eePtrSettingsCRFICstart;
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

const uint8_t eePtrSettingsACDstart =		nextAllowedValue;

#ifdef useVehicleMass
const uint8_t pVehicleMassIdx =				nextAllowedValue;
#define nextAllowedValue pVehicleMassIdx + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t pVehicleFrontalAreaIdx =		nextAllowedValue;
const uint8_t pLocustDensityIdx =			pVehicleFrontalAreaIdx + 1;
const uint8_t pCoefficientDidx =			pLocustDensityIdx + 1;
const uint8_t pCoefficientVidx =			pCoefficientDidx + 1;
const uint8_t pCoefficientRRidx =			pCoefficientVidx + 1;
const uint8_t pCoastdownSamplePeriodIdx =	pCoefficientRRidx + 1;
#define nextAllowedValue pCoastdownSamplePeriodIdx + 1
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
const uint8_t pDragSpeedIdx =				nextAllowedValue;
const uint8_t pDragDistanceIdx =			pDragSpeedIdx + 1;
const uint8_t pDragAutoFlagIdx =			pDragDistanceIdx + 1;
#define nextAllowedValue pDragAutoFlagIdx + 1
#endif // useDragRaceFunction

const uint8_t eePtrSettingsACDend =			nextAllowedValue;
const uint8_t eePtrSettingsACDlen =			eePtrSettingsACDend - eePtrSettingsACDstart;

// timeout settings

const uint8_t eePtrSettingsTimeoutStart =	nextAllowedValue;

const uint8_t pIdleTimeoutIdx =				nextAllowedValue;
const uint8_t pEOCtimeoutIdx =				pIdleTimeoutIdx + 1;
const uint8_t pButtonTimeoutIdx =			pEOCtimeoutIdx + 1;
const uint8_t pParkTimeoutIdx =				pButtonTimeoutIdx + 1;
const uint8_t pActivityTimeoutIdx =			pParkTimeoutIdx + 1;
const uint8_t pWakeupResetCurrentOnEngineIdx =	pActivityTimeoutIdx + 1;
const uint8_t pWakeupResetCurrentOnMoveIdx =	pWakeupResetCurrentOnEngineIdx + 1;
#define nextAllowedValue pWakeupResetCurrentOnMoveIdx + 1
#ifdef useSavedTrips
const uint8_t pAutoSaveActiveIdx =			nextAllowedValue;
#define nextAllowedValue pAutoSaveActiveIdx + 1
#endif // useSavedTrips

const uint8_t eePtrSettingsTimeoutEnd =		nextAllowedValue;
const uint8_t eePtrSettingsTimeoutLen =		eePtrSettingsTimeoutEnd - eePtrSettingsTimeoutStart;

// miscellaneous settings

const uint8_t eePtrSettingsMiscStart =		nextAllowedValue;

const uint8_t pScratchpadIdx =				nextAllowedValue;
#define nextAllowedValue pScratchpadIdx + 1

const uint8_t eePtrSettingsMiscEnd =		nextAllowedValue;
const uint8_t eePtrSettingsMiscLen =		eePtrSettingsMiscEnd - eePtrSettingsMiscStart;

const uint8_t eePtrSettingsVisibleEnd =	nextAllowedValue;

// settings inaccessible from the menu

#ifdef usePartialRefuel
const uint8_t pRefuelSizeIdx =				nextAllowedValue;
#define nextAllowedValue pRefuelSizeIdx + 1
#endif // usePartialRefuel

const uint8_t eePtrSettingsEnd =			nextAllowedValue;

#ifdef useEEPROMtripStorage
const uint8_t eePtrSavedTripsStart = 		nextAllowedValue;

#ifdef usePartialRefuel
const uint8_t pRefuelSaveSizeIdx =			nextAllowedValue;
#define nextAllowedValue pRefuelSaveSizeIdx + 1
#endif // usePartialRefuel
const uint8_t pCurrTripSignatureIdx =		nextAllowedValue;
const uint8_t pTankTripSignatureIdx =		pCurrTripSignatureIdx + 1;
const uint8_t pCurrTripVSSpulseIdx =		pTankTripSignatureIdx + 1;
const uint8_t pCurrTripVSScycleIdx =		pCurrTripVSSpulseIdx + 1;
const uint8_t pCurrTripInjPulseIdx =		pCurrTripVSScycleIdx + 1;
const uint8_t pCurrTripInjCycleIdx =		pCurrTripInjPulseIdx + 1;
const uint8_t pCurrTripEngCycleIdx =		pCurrTripInjCycleIdx + 1;
const uint8_t pTankTripVSSpulseIdx =		pCurrTripEngCycleIdx + 1;
const uint8_t pTankTripVSScycleIdx =		pTankTripVSSpulseIdx + 1;
const uint8_t pTankTripInjPulseIdx =		pTankTripVSScycleIdx + 1;
const uint8_t pTankTripInjCycleIdx =		pTankTripInjPulseIdx + 1;
const uint8_t pTankTripEngCycleIdx =		pTankTripInjCycleIdx + 1;
#define nextAllowedValue pTankTripEngCycleIdx + 1
#if defined(trackIdleEOCdata)
const uint8_t pCurrIEOCvssPulseIdx =		nextAllowedValue;
const uint8_t pCurrIEOCvssCycleIdx =		pCurrIEOCvssPulseIdx + 1;
const uint8_t pCurrIEOCinjPulseIdx =		pCurrIEOCvssCycleIdx + 1;
const uint8_t pCurrIEOCinjCycleIdx =		pCurrIEOCinjPulseIdx + 1;
const uint8_t pCurrIEOCengCycleIdx =		pCurrIEOCinjCycleIdx + 1;
const uint8_t pTankIEOCvssPulseIdx =		pCurrIEOCengCycleIdx + 1;
const uint8_t pTankIEOCvssCycleIdx =		pTankIEOCvssPulseIdx + 1;
const uint8_t pTankIEOCinjPulseIdx =		pTankIEOCvssCycleIdx + 1;
const uint8_t pTankIEOCinjCycleIdx =		pTankIEOCinjPulseIdx + 1;
const uint8_t pTankIEOCengCycleIdx =		pTankIEOCinjCycleIdx + 1;
#define nextAllowedValue pTankIEOCengCycleIdx + 1
#endif // defined(trackIdleEOCdata)

const uint8_t eePtrSavedTripsEnd = 			nextAllowedValue;
const uint8_t eePtrSavedTripsLen = 			eePtrSavedTripsEnd - eePtrSavedTripsStart;

#endif // useEEPROMtripStorage
const uint8_t eePtrStorageEnd =				nextAllowedValue;

#if defined(useScreenEditor)
const uint8_t eePtrDisplayPagesStart =		nextAllowedValue;
const uint8_t eePtrDisplayPagesEnd =		eePtrDisplayPagesStart + mainDisplayFormatSize;
#define nextAllowedValue eePtrDisplayPagesEnd

#endif // defined(useScreenEditor)
const uint8_t eePtrEnd =					nextAllowedValue;

/* labels for parameters definitions above */

const char parmLabels[] PROGMEM = {
	"MPGuinoSig" tcEOSCR

// display settings

#ifdef useLegacyLCD
	"Contrast" tcEOSCR
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	"LCD BG Color" tcEOSCR
#endif // useAdafruitRGBLCDshield
	"Metric 1-Yes" tcEOSCR
	"AltFEformat 1-Y" tcEOSCR
#ifdef useFuelCost
#ifdef useImperialGallon
	"Price*1000/" tcOMOFF "ImpGa" tcOTOG "L" tcOON tcEOSCR
#else  // useImperialGallon
	"Price*1000/" tcOMOFF "USgal" tcOTOG tcOON tcEOSCR
#endif // useImperialGallon
#endif // useFuelCost
#if defined(useOutputPins)
	"OutPtPin 1 Mode" tcEOSCR
	"OutPtPin 2 Mode" tcEOSCR
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
	"V(diode)*1000" tcEOSCR
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	"DLogSerial 1-Yes" tcEOSCR
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
	"FE/Time Period s" tcEOSCR
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"bgLower*1000 " tcOMOFF "MPH" tcOTOG "kph" tcOON tcEOSCR
	"bgSize*1000 " tcOMOFF "MPH" tcOTOG "kph" tcOON tcEOSCR
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	"P(Fuel) " tcOMOFF "psig*1K" tcOTOG "Pa" tcOON tcEOSCR
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	"P(Ref) " tcOMOFF "psig*1K" tcOTOG "Pa" tcOON tcEOSCR
	"Injector Count" tcEOSCR
	"InjSiz cc/min*1K" tcEOSCR
#endif // useCalculatedFuelFactor
	"Microsec/USgal" tcEOSCR
	"InjTrg 0-Dn 1-Up" tcEOSCR
	"Inj Delay (us)" tcEOSCR
	"InjCloseDly (us)" tcEOSCR
	"Revs/Inj Pulse" tcEOSCR
	"Min good RPM" tcEOSCR

// vehicle speed sensor settings

	"VSS Pulses/" tcOMOFF "Mile" tcOTOG "km" tcOON tcEOSCR
	"VSS Delay (ms)" tcEOSCR
	"Min Speed " tcOMOFF "MPH" tcOTOG "KPH" tcOON "*1K " tcEOSCR

// fuel tank size settings

#ifdef useImperialGallon
	"Tank*1000 " tcOMOFF "ImpGal" tcOTOG "L" tcOON tcEOSCR
	"Bingo*1000 " tcOMOFF "ImpGal" tcOTOG "L" tcOON tcEOSCR
#else // useImperialGallon
	"Tank*1000 " tcOMOFF "USgal" tcOTOG "L" tcOON tcEOSCR
	"Bingo*1000 " tcOMOFF "USgal" tcOTOG "L" tcOON tcEOSCR
#endif // useImperialGallon
#ifdef useChryslerMAPCorrection

// Chrysler settings

	"MAPfloor (mV)" tcEOSCR
	"MAPceiling (mV)" tcEOSCR
	"MAPrng " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
	"MAPofst " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
#ifdef useChryslerBaroSensor
	"BaroFloor (mV)" tcEOSCR
	"BaroCeiling (mV)" tcEOSCR
	"BaroRng " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
	"BaroOfst " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
#else // useChryslerBaroSensor
	"BaroPrs " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	tcOMOFF "Weight" tcOTOG "Mass" tcOON " (" tcOMOFF "lbs" tcOTOG "kg" tcOON ")" tcEOSCR
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	"FrArea*1000 " tcOMOFF "ft" tcOTOG "m" tcOON "^2" tcEOSCR
	"rho*1000 " tcOMOFF "lb/yd" tcOTOG "kg/m" tcOON "^3" tcEOSCR
	"C(d) * 1000" tcEOSCR
	"C(v) * 1000" tcEOSCR
	"C(rr) * 1000" tcEOSCR
	"Cd SamplePd (s)" tcEOSCR
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	"DragSpd " tcOMOFF "MPH" tcOTOG "kph" tcOON "*1000" tcEOSCR
	"DragDist " tcOMOFF "mi" tcOTOG "km" tcOON "*1000" tcEOSCR
	"DragAutoTrigger" tcEOSCR
#endif // useDragRaceFunction

// timeout settings

	"Idle Timeout (s)" tcEOSCR
	"EOC Timeout (s)" tcEOSCR
	"BtnPress T/O (s)" tcEOSCR
	"Park Timeout (s)" tcEOSCR
	"OFF Timeout (s)" tcEOSCR
	"WakeEngRst CURR" tcEOSCR
	"WakeVSSRst CURR" tcEOSCR
#ifdef useSavedTrips
	"AutoSaveTrip 1-Y" tcEOSCR
#endif // useSavedTrips

// miscellaneous settings

	"Scratchpad(odo?)" tcEOSCR

// settings inaccessible from the menu

#ifdef usePartialRefuel
#ifdef useImperialGallon
	"Refill*1K " tcOMOFF "ImpGal" tcOTOG "L" tcOON tcEOSCR
#ifdef useEEPROMtripStorage
	"RefillSave*1K " tcOMOFF "ImpGal" tcOTOG "L" tcOON tcEOSCR
#endif // useEEPROMtripStorage
#else // useImperialGallon
	"Refill*1K " tcOMOFF "USgal" tcOTOG "L" tcOON tcEOSCR
#ifdef useEEPROMtripStorage
	"RefillSave*1K " tcOMOFF "USgal" tcOTOG "L" tcOON tcEOSCR
#endif // useEEPROMtripStorage
#endif // useImperialGallon
#endif // usePartialRefuel
#ifdef useDebugTerminalLabels
#ifdef useEEPROMtripStorage
	"CurrTripSig" tcEOSCR
	"TankTripSig" tcEOSCR
	"CurrVSSpulse" tcEOSCR
	"CurrVSScycle" tcEOSCR
	"CurrInjPulse" tcEOSCR
	"CurrInjCycle" tcEOSCR
	"CurrEngCycle" tcEOSCR
	"TankVSSpulse" tcEOSCR
	"TankVSScycle" tcEOSCR
	"TankInjPulse" tcEOSCR
	"TankInjCycle" tcEOSCR
	"TankEngCycle" tcEOSCR
#if defined(trackIdleEOCdata)
	"CurrIEOCvssPulse" tcEOSCR
	"CurrIEOCvssCycle" tcEOSCR
	"CurrIEOCinjPulse" tcEOSCR
	"CurrIEOCinjCycle" tcEOSCR
	"CurrIEOCengCycle" tcEOSCR
	"TankIEOCvssPulse" tcEOSCR
	"TankIEOCvssCycle" tcEOSCR
	"TankIEOCinjPulse" tcEOSCR
	"TankIEOCinjCycle" tcEOSCR
	"TankIEOCengCycle" tcEOSCR
#endif // defined(trackIdleEOCdata)
#endif // useEEPROMtripStorage
#if defined(useScreenEditor)
	"P00F00" tcEOSCR
	"P00F01" tcEOSCR
	"P00F02" tcEOSCR
	"P00F03" tcEOSCR

	"P01F00" tcEOSCR
	"P01F01" tcEOSCR
	"P01F02" tcEOSCR
	"P01F03" tcEOSCR

	"P02F00" tcEOSCR
	"P02F01" tcEOSCR
	"P02F02" tcEOSCR
	"P02F03" tcEOSCR

	"P03F00" tcEOSCR
	"P03F01" tcEOSCR
	"P03F02" tcEOSCR
	"P03F03" tcEOSCR

	"P04F00" tcEOSCR
	"P04F01" tcEOSCR
	"P04F02" tcEOSCR
	"P04F03" tcEOSCR

	"P05F00" tcEOSCR
	"P05F01" tcEOSCR
	"P05F02" tcEOSCR
	"P05F03" tcEOSCR

#if defined(trackIdleEOCdata)
	"P06F00" tcEOSCR
	"P06F01" tcEOSCR
	"P06F02" tcEOSCR
	"P06F03" tcEOSCR

#endif // defined(trackIdleEOCdata)
	"P07F00" tcEOSCR
	"P07F01" tcEOSCR
	"P07F02" tcEOSCR
	"P07F03" tcEOSCR

	"P08F00" tcEOSCR
	"P08F01" tcEOSCR
	"P08F02" tcEOSCR
	"P08F03" tcEOSCR

#if defined(trackIdleEOCdata)
	"P09F00" tcEOSCR
	"P09F01" tcEOSCR
	"P09F02" tcEOSCR
	"P09F03" tcEOSCR

	"P10F00" tcEOSCR
	"P10F01" tcEOSCR
	"P10F02" tcEOSCR
	"P10F03" tcEOSCR

#endif // defined(trackIdleEOCdata)
	"P11F00" tcEOSCR
	"P11F01" tcEOSCR
	"P11F02" tcEOSCR
	"P11F03" tcEOSCR

#endif // defined(useScreenEditor)
#endif // useDebugTerminalLabels
};

// 0byyy0 0000 - MPGuino parameter action needed that is critical to measurements of fuel injection, VSS, timeouts, and the like
// 0b0000 0xxx - fractional part of parameter size in bits

static const uint8_t pfDoNothing =				0;
static const uint8_t pfSoftwareInitMPGuino =	pfDoNothing + 32;
static const uint8_t pfHardwareInitMPGuino =	pfSoftwareInitMPGuino + 32;
static const uint8_t pfDoMetricModeConversion =	pfHardwareInitMPGuino + 32;
static const uint8_t pfChangeDisplay =			pfDoMetricModeConversion + 32;
static const uint8_t pfCalculateFuelParams =	pfChangeDisplay + 32;

const uint8_t paramsLength[] PROGMEM = {
	 (pSizeSignature & 0x07) | pfHardwareInitMPGuino							// EEPROM MPGuino signature long word

// display settings

#ifdef useLegacyLCD
	,(pSizeContrast & 0x07) | pfChangeDisplay									// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,(pSizeLCDcolor & 0x07) | pfChangeDisplay									// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,(pSizeMetricMode & 0x07) | pfDoMetricModeConversion						// Display Mode (0 - US Display, 1 - Metric Display)
	,(pSizeAlternateFE & 0x07) | pfSoftwareInitMPGuino							// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useFuelCost
	,(pSizeFuelUnitCost & 0x07)													// Price per unit volume of fuel
#endif // useFuelCost
#if defined(useOutputPins)
	,(pSizeOutputPin1Mode & 0x07)												// Output Pin 1 mode
	,(pSizeOutputPin2Mode & 0x07)												// Output Pin 2 mode
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
	,(pSizeVoltageOffset & 0x07)												// diode offset from V(alternator)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	,(pSizeSerialDataLogging & 0x07)											// Serial Data Logging Enable
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
	,(pSizeFEvsTime & 0x07) | pfSoftwareInitMPGuino								// Period Of FE over Time BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,(pSizeBarLowSpeedCutoff & 0x07) | pfSoftwareInitMPGuino					// FE vs Speed Bargraph lower speed
	,(pSizeBarSpeedQuantumIdx & 0x07) | pfSoftwareInitMPGuino					// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	,(pSizeSysFuelPressure & 0x07) | pfSoftwareInitMPGuino						// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,(pSizeRefFuelPressure & 0x07) | pfCalculateFuelParams						// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,(pSizeInjectorCount & 0x07) | pfCalculateFuelParams						// Fuel Injector Count
	,(pSizeInjectorSize & 0x07) | pfCalculateFuelParams							// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,(pSizeMicroSecondsPerGallon & 0x07) | pfSoftwareInitMPGuino				// Microseconds per US gallon
	,(pSizeInjEdgeTrigger & 0x07) | pfHardwareInitMPGuino						// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	,(pSizeInjectorOpeningTime & 0x07) | pfSoftwareInitMPGuino					// Fuel Injector Opening Delay Time (us)
	,(pSizeInjectorClosingTime & 0x07) | pfSoftwareInitMPGuino					// Fuel Injector Closing Delay Time (us)
	,(pSizeCrankRevPerInj & 0x07) | pfSoftwareInitMPGuino						// Crankshaft Revolutions per Fuel Injector Event
	,(pSizeMinGoodRPM & 0x07) | pfSoftwareInitMPGuino							// Minimum Engine Speed For Engine On (RPM)

// vehicle speed sensor settings

	,(pSizePulsesPerDistance & 0x07) | pfSoftwareInitMPGuino					// VSS Pulses (per mile or per km)
	,(pSizeVSSpause & 0x07) | pfHardwareInitMPGuino								// VSS Pause Debounce Count (ms)
	,(pSizeMinGoodSpeed & 0x07) | pfSoftwareInitMPGuino							// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000

// fuel tank size settings

	,(pSizeTankSize & 0x07) | pfSoftwareInitMPGuino								// Tank Capacity * 1000 (gal or L)
	,(pSizeTankBingoSize & 0x07) | pfSoftwareInitMPGuino						// Bingo Fuel (reserve tank) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection

// Chrysler settings

	,(pSizeMAPsensorFloor & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Floor * 1000 (V)
	,(pSizeMAPsensorCeiling & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Ceiling * 1000 (V)
	,(pSizeMAPsensorRange & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Range (Pa or * 1000 psig)
	,(pSizeMAPsensorOffset & 0x07) | pfSoftwareInitMPGuino						// MAP Sensor Offset (Pa or * 1000 psia)
#ifdef useChryslerBaroSensor
	,(pSizeBaroSensorFloor & 0x07) | pfSoftwareInitMPGuino						// Barometric Sensor Floor * 1000 (V)
	,(pSizeBaroSensorCeiling & 0x07) | pfSoftwareInitMPGuino					// Barometric Sensor Ceiling * 1000 (V)
	,(pSizeBaroSensorRange & 0x07) | pfSoftwareInitMPGuino						// Barometric Sensor Range (Pa or * 1000 psig)
	,(pSizeBaroSensorOffset & 0x07) | pfSoftwareInitMPGuino						// Barometric Sensor Offset (Pa or * 1000 psia)
#else // useChryslerBaroSensor
	,(pSizeBarometricPressure & 0x07) | pfSoftwareInitMPGuino					// Reference Barometric Pressure
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	,(pSizeVehicleMass & 0x07)													// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,(pSizeVehicleFrontalArea & 0x07)											// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,(pSizeLocustDensity & 0x07)												// Air density (lb/yd^3 or kg/m^3)
	,(pSizeCoefficientD & 0x07)													// Vehicle C(d) * 1000
	,(pSizeCoefficientV & 0x07)													// Vehicle C(v) * 1000
	,(pSizeCoefficientRR & 0x07)												// Vehicle C(rr) * 1000
	,(pSizeCoastdownSamplePeriod & 0x07)										// Sample Period in seconds
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	,(pSizeDragSpeed & 0x07)													// Drag speed cutoff (MPH or kph) * 1000
	,(pSizeDragDistance & 0x07)													// Drag distance cutoff (miles or km) * 1000
	,(pSizeDragAutoFlag & 0x07)													// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction

// timeout settings

	,(pSizeIdleTimeout & 0x07) | pfSoftwareInitMPGuino							// Engine Idle Timeout (s)
	,(pSizeEOCtimeout & 0x07) | pfSoftwareInitMPGuino							// Engine-Off Coasting Timeout (s)
	,(pSizeButtonTimeout & 0x07) | pfSoftwareInitMPGuino						// Button Press Activity Timeout (s)
	,(pSizeParkTimeout & 0x07) | pfSoftwareInitMPGuino							// Vehicle Parked (engine off, no movement) Timeout (s)
	,(pSizeActivityTimeout & 0x07) | pfSoftwareInitMPGuino						// Activity (engine off, no movement, no button press) Timeout (s)
	,(pSizeWakeupResetCurrentOnEngine & 0x07)									// Enable current trip reset upon wakeup due to engine running
	,(pSizeWakeupResetCurrentOnMove & 0x07)										// Enable current trip reset upon wakeup due to vehicle movement
#ifdef useSavedTrips
	,(pSizeAutoSaveActive & 0x07)												// Autosave Active Trip Data Enable
#endif // useSavedTrips

// miscellaneous settings

	,(pSizeScratchpad & 0x07)													// Scratchpad Memory

// settings inaccessible from the menu

#ifdef usePartialRefuel
	,(pSizeRefuelSize & 0x07) | pfSoftwareInitMPGuino							// Partial Refuel amount * 1000 (gal or L)
#ifdef useEEPROMtripStorage
	,(pSizeRefuelSaveSizeIdx & 0x07)											// Partial Refuel save amount * 1000 (gal or L)
#endif // useEEPROMtripStorage
#endif // usePartialRefuel

#ifdef useEEPROMtripStorage
	,(pSizeCurrTripSignatureIdx & 0x07)											// Current Trip signature byte
	,(pSizeTankTripSignatureIdx & 0x07)											// Tank Trip signature byte

	,(pSizeCurrTripVSSpulseIdx & 0x07)											// Current Trip VSS pulse count storage
	,(pSizeCurrTripVSScycleIdx & 0x07)											// Current Trip VSS cycle accumulator storage
	,(pSizeCurrTripInjPulseIdx & 0x07)											// Current Trip injector pulse count storage
	,(pSizeCurrTripInjCycleIdx & 0x07)											// Current Trip injector open cycle accumulator storage
	,(pSizeCurrTripEngCycleIdx & 0x07)											// Current Trip engine revolution cycle accumulator storage
	,(pSizeTankTripVSSpulseIdx & 0x07)											// Tank Trip VSS pulse count storage
	,(pSizeTankTripVSScycleIdx & 0x07)											// Tank Trip VSS cycle accumulator storage
	,(pSizeTankTripInjPulseIdx & 0x07)											// Tank Trip injector pulse count storage
	,(pSizeTankTripInjCycleIdx & 0x07)											// Tank Trip injector open cycle accumulator storage
	,(pSizeTankTripEngCycleIdx & 0x07)											// Tank Trip engine revolution cycle accumulator storage
#if defined(trackIdleEOCdata)
	,(pSizeCurrIEOCvssPulseIdx & 0x07)											// Current Idle/EOC Trip VSS pulse count storage
	,(pSizeCurrIEOCvssCycleIdx & 0x07)											// Current Idle/EOC Trip VSS cycle accumulator storage
	,(pSizeCurrIEOCinjPulseIdx & 0x07)											// Current Idle/EOC Trip injector pulse count storage
	,(pSizeCurrIEOCinjCycleIdx & 0x07)											// Current Idle/EOC Trip injector open cycle accumulator storage
	,(pSizeCurrIEOCengCycleIdx & 0x07)											// Current Idle/EOC Trip engine revolution cycle accumulator storage
	,(pSizeTankIEOCvssPulseIdx & 0x07)											// Tank Idle/EOC Trip VSS pulse count storage
	,(pSizeTankIEOCvssCycleIdx & 0x07)											// Tank Idle/EOC Trip VSS cycle accumulator storage
	,(pSizeTankIEOCinjPulseIdx & 0x07)											// Tank Idle/EOC Trip injector pulse count storage
	,(pSizeTankIEOCinjCycleIdx & 0x07)											// Tank Idle/EOC Trip injector open cycle accumulator storage
	,(pSizeTankIEOCengCycleIdx & 0x07)											// Tank Idle/EOC Trip engine revolution cycle accumulator storage
#endif // defined(trackIdleEOCdata)
#endif // useEEPROMtripStorage
};

/* parameter EEPROM addresses for parameter definitions above */

const uint8_t paramAddrs[] PROGMEM = {
	(uint8_t)(pAddressSignature)					// EEPROM signature long word for MPGuino

// display settings

#ifdef useLegacyLCD
	,(uint8_t)(pAddressContrast)					// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,(uint8_t)(pAddressLCDcolor)					// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,(uint8_t)(pAddressMetricMode)					// Display Mode (0 - US Display, 1 - Metric Display)
	,(uint8_t)(pAddressAlternateFE)					// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useFuelCost
	,(uint8_t)(pAddressFuelUnitCost)				// Price per unit volume of fuel
#endif // useFuelCost
#if defined(useOutputPins)
	,(uint8_t)(pAddressOutputPin1Mode)				// Output Pin 1 mode
	,(uint8_t)(pAddressOutputPin2Mode)				// Output Pin 2 mode
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
	,(uint8_t)(pAddressVoltageOffset)				// diode offset from V(alternator)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	,(uint8_t)(pAddressSerialDataLogging)			// Serial Data Logging Enable
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
	,(uint8_t)(pAddressFEvsTime)					// Period Of FE over Time Bar Graph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,(uint8_t)(pAddressBarLowSpeedCutoff)			// FE vs Speed Bargraph lower speed
	,(uint8_t)(pAddressBarSpeedQuantumIdx)			// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	,(uint8_t)(pAddressSysFuelPressure)				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,(uint8_t)(pAddressRefFuelPressure)				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,(uint8_t)(pAddressInjectorCount)				// Fuel Injector Count
	,(uint8_t)(pAddressInjectorSize)				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,(uint8_t)(pAddressMicroSecondsPerGallon)		// Microseconds per US gallon
	,(uint8_t)(pAddressInjEdgeTrigger)				// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	,(uint8_t)(pAddressInjectorOpeningTime)			// Fuel Injector Opening Delay Time (us)
	,(uint8_t)(pAddressInjectorClosingTime)			// Fuel Injector Closing Delay Time (us)
	,(uint8_t)(pAddressCrankRevPerInj)				// Crankshaft Revolutions per Fuel Injector Event
	,(uint8_t)(pAddressMinGoodRPM)					// Minimum Engine Speed For Engine On (RPM)

// vehicle speed sensor settings

	,(uint8_t)(pAddressPulsesPerDistance)			// VSS Pulses (per mile or per km)
	,(uint8_t)(pAddressVSSpause)					// VSS Pause Debounce Count (ms)
	,(uint8_t)(pAddressMinGoodSpeed)				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000

// fuel tank size settings

	,(uint8_t)(pAddressTankSize)					// Tank Capacity * 1000 (gal or L)
	,(uint8_t)(pAddressTankBingoSize)				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection

// Chrysler settings

	,(uint8_t)(pAddressMAPsensorFloor)				// MAP Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressMAPsensorCeiling)			// MAP Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressMAPsensorRange)				// MAP Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressMAPsensorOffset)				// MAP Sensor Offset (Pa or * 1000 psia)
#ifdef useChryslerBaroSensor
	,(uint8_t)(pAddressBaroSensorFloor)				// Barometric Sensor Floor * 1000 (V)
	,(uint8_t)(pAddressBaroSensorCeiling)			// Barometric Sensor Ceiling * 1000 (V)
	,(uint8_t)(pAddressBaroSensorRange)				// Barometric Sensor Range (Pa or * 1000 psig)
	,(uint8_t)(pAddressBaroSensorOffset)			// Barometric Sensor Offset (Pa or * 1000 psia)
#else // useChryslerBaroSensor
	,(uint8_t)(pAddressBarometricPressure)			// Reference Barometric Pressure
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	,(uint8_t)(pAddressVehicleMass)					// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,(uint8_t)(pAddressVehicleFrontalArea)			// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	,(uint8_t)(pAddressLocustDensity)				// Air density (lb/yd^3 or kg/m^3)
	,(uint8_t)(pAddressCoefficientD)				// Vehicle C(d) * 1000
	,(uint8_t)(pAddressCoefficientV)				// Vehicle C(v) * 1000
	,(uint8_t)(pAddressCoefficientRR)				// Vehicle C(rr) * 1000
	,(uint8_t)(pAddressCoastdownSamplePeriod)		// Sample Period in seconds
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	,(uint8_t)(pAddressDragSpeed)					// Drag speed cutoff (MPH or kph) * 1000
	,(uint8_t)(pAddressDragDistance)				// Drag distance cutoff (miles or km) * 1000
	,(uint8_t)(pAddressDragAutoFlag)				// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction

// timeout settings

	,(uint8_t)(pAddressIdleTimeout)					// Engine Idle Timeout (s)
	,(uint8_t)(pAddressEOCtimeout)					// Engine-Off Coasting Timeout (s)
	,(uint8_t)(pAddressButtonTimeout)				// Button Press Activity Timeout (s)
	,(uint8_t)(pAddressParkTimeout)					// Vehicle Parked (engine off, no movement) Timeout (s)
	,(uint8_t)(pAddressActivityTimeout)				// Activity (engine off, no movement, no button press) Timeout (s)
	,(uint8_t)(pAddressWakeupResetCurrentOnEngine)	// Enable current trip reset upon wakeup due to engine running
	,(uint8_t)(pAddressWakeupResetCurrentOnMove)	// Enable current trip reset upon wakeup due to vehicle movement
#ifdef useSavedTrips
	,(uint8_t)(pAddressAutoSaveActive)				// Autosave Active Trip Data Enable
#endif // useSavedTrips

// miscellaneous settings

	,(uint8_t)(pAddressScratchpad)					// Scratchpad Memory

// settings inaccessible from the menu

#ifdef usePartialRefuel
	,(uint8_t)(pAddressRefuelSize)					// Partial Refuel amount * 1000 (gal or L)
#ifdef useEEPROMtripStorage
	,(uint8_t)(pAddressRefuelSaveSizeIdx)			// Partial Refuel save amount * 1000 (gal or L)
#endif // useEEPROMtripStorage
#endif // usePartialRefuel
#ifdef useEEPROMtripStorage
	,(uint8_t)(pAddressCurrTripSignatureIdx)		// Current Trip signature byte
	,(uint8_t)(pAddressTankTripSignatureIdx)		// Tank Trip signature byte

	,(uint8_t)(pAddressCurrTripVSSpulseIdx)			// Current Trip VSS pulse count storage
	,(uint8_t)(pAddressCurrTripVSScycleIdx)			// Current Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressCurrTripInjPulseIdx)			// Current Trip injector pulse count storage
	,(uint8_t)(pAddressCurrTripInjCycleIdx)			// Current Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressCurrTripEngCycleIdx)			// Current Trip engine revolution cycle accumulator storage
	,(uint8_t)(pAddressTankTripVSSpulseIdx)			// Tank Trip VSS pulse count storage
	,(uint8_t)(pAddressTankTripVSScycleIdx)			// Tank Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressTankTripInjPulseIdx)			// Tank Trip injector pulse count storage
	,(uint8_t)(pAddressTankTripInjCycleIdx)			// Tank Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressTankTripEngCycleIdx)			// Tank Trip engine revolution cycle accumulator storage
#if defined(trackIdleEOCdata)
	,(uint8_t)(pAddressCurrIEOCvssPulseIdx)			// Current Idle/EOC Trip VSS pulse count storage
	,(uint8_t)(pAddressCurrIEOCvssCycleIdx)			// Current Idle/EOC Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressCurrIEOCinjPulseIdx)			// Current Idle/EOC Trip injector pulse count storage
	,(uint8_t)(pAddressCurrIEOCinjCycleIdx)			// Current Idle/EOC Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressCurrIEOCengCycleIdx)			// Current Idle/EOC Trip engine revolution cycle accumulator storage
	,(uint8_t)(pAddressTankIEOCvssPulseIdx)			// Tank Idle/EOC Trip VSS pulse count storage
	,(uint8_t)(pAddressTankIEOCvssCycleIdx)			// Tank Idle/EOC Trip VSS cycle accumulator storage
	,(uint8_t)(pAddressTankIEOCinjPulseIdx)			// Tank Idle/EOC Trip injector pulse count storage
	,(uint8_t)(pAddressTankIEOCinjCycleIdx)			// Tank Idle/EOC Trip injector open cycle accumulator storage
	,(uint8_t)(pAddressTankIEOCengCycleIdx)			// Tank Idle/EOC Trip engine revolution cycle accumulator storage
#endif // defined(trackIdleEOCdata)
#endif // useEEPROMtripStorage
};

/* parameter values for parameter definitions above */

const uint32_t newEEPROMsignature = ((uint32_t)(guinosig) << 16) + ((uint32_t)(eePtrEnd) << 8) + (uint32_t)(eeAdrSettingsEnd);

const uint32_t params[] PROGMEM = {
	 newEEPROMsignature	// EEPROM MPGuino signature long word

// display settings

#ifdef useLegacyLCD
	,55					// LCD Contrast
#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	,3					// LCD Backlight color
#endif // useAdafruitRGBLCDshield
	,0					// Display Mode (0 - US Display, 1 - Metric Display)
	,0					// 0 - MPG or L/100km, 1 - G/100mile or km/L
#ifdef useFuelCost
	,2710259			// Price per unit volume of fuel (this prices is for a liter of diesel at 717 HUF / liter)
#endif // useFuelCost
#if defined(useOutputPins)
	,0					// Output pin 1 mode
	,0					// Output pin 2 mode
#endif // defined(useOutputPins)
#ifdef useCarVoltageOutput
	,700				// diode offset from V(alternator) (via meelis11)
#endif // useCarVoltageOutput
#ifdef useDataLoggingOutput
	,1					// Serial Data Logging Enable
#endif // useDataLoggingOutput
#ifdef useBarFuelEconVsTime
	,5					// Length Of BarGraph Bar (s)
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,25000				// FE vs Speed Bargraph lower speed
	,5000				// FE vs Speed Bargraph speed bar size
#endif // useBarFuelEconVsSpeed

// fuel injection settings

#ifdef useIsqrt
	,43500				// Fuel System Pressure (Pa or * 1000 psig)
#endif // useIsqrt
#ifdef useCalculatedFuelFactor
	,43500				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	,6					// Fuel Injector Count
	,246000				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
	,133262651			// Microseconds per US gallon
	,0					// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	,550				// Fuel Injector Response Delay Time (us)
	,2000				// Fuel Injector Closing Delay Time (us)
	,2					// Crankshaft Revolutions per Fuel Injector Event
	,40					// Minimum Engine Speed For Engine On (RPM)

// vehicle speed sensor settings

	,10000				// VSS Pulses (per mile or per km)
	,0					// VSS Pause Debounce Count (ms)
	,300				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000

// fuel tank size settings

	,18000				// Tank Capacity * 1000 (gal or L)
	,2200				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
#ifdef useChryslerMAPCorrection

// Chrysler settings

	,500				// MAP Sensor Floor * 1000 (V)
	,4000				// MAP Sensor Ceiling * 1000 (V)
	,14145				// Barometric Sensor Range (Pa or * 1000 psig)
	,551				// MAP Sensor Offset (Pa or * 1000 psia)
#ifdef useChryslerBaroSensor
	,500				// Barometric Sensor Floor * 1000 (V)
	,4000				// Barometric Sensor Ceiling * 1000 (V)
	,14145				// Barometric Sensor Range (Pa or * 1000 psig)
	,551				// Barometric Sensor Offset (Pa or * 1000 psia)
#else // useChryslerBaroSensor
	,14696				// Reference Barometric Pressure
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection

// drag race / coastdown calc settings

#ifdef useVehicleMass
	,4000				// Vehicle Weight/Mass (lbs or kg)
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,25400				// Vehicle Frontal Area (ft^2 or m^2)
	,2065				// Air density (lb/yd^3 or kg/m^3)
	,346				// C(d)
	,1					// C(v)
	,8					// C(rr)
	,3					// Sample Period in seconds
#endif // useCoastDownCalculator
#ifdef useDragRaceFunction
	,60000				// Drag speed cutoff (MPH or kph) * 1000
	,250				// Drag distance cutoff (miles or km) * 1000
	,0					// Drag automatic retrigger on vehicle stop
#endif // useDragRaceFunction

// timeout settings

	,15					// Engine Idle Timeout (s)
	,2					// Engine-Off Coasting Timeout (s)
	,5					// Button Press Activity Timeout (s)
	,5					// Vehicle Parked (engine off, no movement) Timeout (s)
	,120				// Activity (engine off, no movement, no button press) Timeout (s)
	,1					// Enable current trip reset upon wakeup due to engine running
	,0					// Enable current trip reset upon wakeup due to button press
#ifdef useSavedTrips
	,1					// Autosave Active Trip Data Enable
#endif // useSavedTrips

// miscellaneous settings

	,0					// Scratchpad Memory

// settings inaccessible from the menu

#ifdef usePartialRefuel
	,0					// Partial Refuel amount * 1000 (gal or L)
#endif // usePartialRefuel
};

// end of remarkably long EEPROM stored settings section
