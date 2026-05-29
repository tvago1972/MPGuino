namespace EEPROM /* EEPROM parameter I/O section prototype */
{

	static uint8_t powerUpCheck(void);
	static void initGuinoHardware(void);
	static void initGuinoSoftware(void);
	static void setMetricDisplayMode(void);
	static uint8_t onChange(const uint8_t * sched, uint8_t parameterIdx);
	static uint8_t readByte(uint8_t eePtr);
#if defined(useScreenEditor)
	static uint16_t readWord(uint8_t eePtr);
#endif // defined(useScreenEditor)
	static void writeByte(uint8_t eePtr, uint8_t value);
#if defined(useScreenEditor)
	static void writeVal(uint8_t eePtr, uint32_t value);
#endif // defined(useScreenEditor)
	static void read64(union union_64 * an, uint8_t parameterIdx);
	static void write64(union union_64 * an, uint8_t parameterIdx);
	static uint16_t getAddress(uint8_t eePtr);
	static uint8_t getParameterFlags(uint8_t eePtr);
	static uint8_t getLength(uint8_t eePtr);

};

#if defined(useButtonInput)
static const uint8_t displayCountMain = 9			// count of base number of data displays
#if defined(trackIdleEOCdata)
	+ 3												// count of Idle/EOC tracking data displays
#endif // defined(trackIdleEOCdata)
;
static const uint8_t mainDisplayFormatSize = displayCountMain * 4;

// the following display index defines are for the main MPGuino displays

#define nextAllowedValue 0
static const uint8_t baseMenuDisplayIdx =			nextAllowedValue;
#define nextAllowedValue baseMenuDisplayIdx + 1
#if defined(useExpandedMainDisplay)
static const uint8_t mainMenuDisplayIdx =			nextAllowedValue;
#define nextAllowedValue mainMenuDisplayIdx + 1
#endif // defined(useExpandedMainDisplay)
static const uint8_t settingsMenuDisplayIdx =		nextAllowedValue;
#define nextAllowedValue settingsMenuDisplayIdx + 1

// the following display index defines are for the various EEPROM parameter settings, grouped by function

static const uint8_t displaySettingsDisplayIdx =	nextAllowedValue;
static const uint8_t fuelSettingsDisplayIdx =		displaySettingsDisplayIdx + 1;
static const uint8_t VSSsettingsDisplayIdx =		fuelSettingsDisplayIdx + 1;
static const uint8_t tankSettingsDisplayIdx =		VSSsettingsDisplayIdx + 1;
#define nextAllowedValue tankSettingsDisplayIdx + 1
#if defined(useChryslerMAPCorrection)
static const uint8_t CRFICsettingsDisplayIdx =		nextAllowedValue;
#define nextAllowedValue CRFICsettingsDisplayIdx + 1
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint8_t acdSettingsDisplayIdx =		nextAllowedValue;
#define nextAllowedValue acdSettingsDisplayIdx + 1
#endif // defined(useVehicleParameters)
static const uint8_t timeoutSettingsDisplayIdx =	nextAllowedValue;
static const uint8_t miscSettingsDisplayIdx =		timeoutSettingsDisplayIdx + 1;
#define nextAllowedValue miscSettingsDisplayIdx + 1

static const uint8_t displayCountSettings =			nextAllowedValue - displaySettingsDisplayIdx;

// the following display index defines are for any otherwise unlinked menu groups

#if defined(useSavedTrips)
static const uint8_t tripSaveCurrentDisplayIdx =	nextAllowedValue;
#define nextAllowedValue tripSaveCurrentDisplayIdx + 1
#endif // defined(useSavedTrips)
#if defined(useEnhancedTripReset)
static const uint8_t tripSaveTankDisplayIdx =		nextAllowedValue;
#define nextAllowedValue tripSaveTankDisplayIdx + 1
#endif // defined(useEnhancedTripReset)

// the following display index defines are for options selected via configs.h

static const uint8_t optionalDisplayIdxStart =		nextAllowedValue;

#if defined(useDragRaceFunction)
static const uint8_t dragRaceMenuDisplayIdx =		nextAllowedValue;
#define nextAllowedValue dragRaceMenuDisplayIdx + 1
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
static const uint8_t coastdownMenuDisplayIdx =		nextAllowedValue;
#define nextAllowedValue coastdownMenuDisplayIdx + 1
#endif // defined(useCoastDownCalculator)

static const uint8_t displayCountMenu =				nextAllowedValue - baseMenuDisplayIdx;

#if defined(useSimulatedFIandVSS)
static const uint8_t signalSimDisplayIdx =			nextAllowedValue;
#define nextAllowedValue signalSimDisplayIdx + 1
#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
static const uint8_t pressureDisplayIdx =			nextAllowedValue;
#define nextAllowedValue pressureDisplayIdx + 1
#endif // defined(useChryslerMAPCorrection)
#if defined(useDebugAnalog)
static const uint8_t analogDisplayIdx =				nextAllowedValue;
#define nextAllowedValue analogDisplayIdx + 1
#endif // defined(useDebugAnalog)

static const uint8_t displayCountBase =				nextAllowedValue + 2 - optionalDisplayIdxStart;

// the following display index defines are for the various main displays

static const uint8_t mainDisplayIdx =				nextAllowedValue;
#define nextAllowedValue mainDisplayIdx + 1
#if defined(useStatusMeter)
static const uint8_t statusMeterDisplayIdx =		nextAllowedValue;
#define nextAllowedValue statusMeterDisplayIdx + 1
#endif // defined(useStatusMeter)
#if defined(useBigFE)
static const uint8_t bigFEdisplayIdx =				nextAllowedValue;
#define nextAllowedValue bigFEdisplayIdx + 1
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
static const uint8_t barFEvTdisplayIdx =			nextAllowedValue;
#define nextAllowedValue barFEvTdisplayIdx + 1
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t barFEvSdisplayIdx =			nextAllowedValue;
#define nextAllowedValue barFEvSdisplayIdx + 1
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
static const uint8_t bigDTEdisplayIdx =				nextAllowedValue;
#define nextAllowedValue bigDTEdisplayIdx + 1
#endif // defined(useBigDTE)
#if defined(useBigTTE)
static const uint8_t bigTTEdisplayIdx =				nextAllowedValue;
#define nextAllowedValue bigTTEdisplayIdx + 1
#endif // defined(useBigTTE)
#if defined(useCPUreading)
static const uint8_t CPUmonDisplayIdx =				nextAllowedValue;
#define nextAllowedValue CPUmonDisplayIdx + 1
#endif // defined(useCPUreading)
#if defined(useClockDisplay)
static const uint8_t clockShowDisplayIdx =			nextAllowedValue;
#define nextAllowedValue clockShowDisplayIdx + 1
#endif // defined(useClockDisplay)

static const uint8_t displayCountUser =				nextAllowedValue - mainDisplayIdx;	// this variable is used to figure out how many menu levels the user display section has

// the following display index defines are supplemental non-menu display index defines for options selected via configs.h

#if defined(useDragRaceFunction)
static const uint8_t dragRaceDisplayIdx =			nextAllowedValue;
#define nextAllowedValue dragRaceDisplayIdx + 1
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
static const uint8_t coastdownDisplayIdx =			nextAllowedValue;
#define nextAllowedValue coastdownDisplayIdx + 1
#endif // defined(useCoastDownCalculator)

static const uint8_t displayMaxSavableIdx =			nextAllowedValue;

static const uint8_t parameterEditDisplayIdx =		nextAllowedValue;
#define nextAllowedValue parameterEditDisplayIdx + 1
#if defined(useClockDisplay)
static const uint8_t clockSetDisplayIdx =			nextAllowedValue;
#define nextAllowedValue clockSetDisplayIdx + 1
#endif // defined(useClockDisplay)
#if defined(useScreenEditor)
static const uint8_t displayEditDisplayIdx =		nextAllowedValue;
#define nextAllowedValue displayEditDisplayIdx + 1
#endif // defined(useScreenEditor)

static const char displayCountTotal =				nextAllowedValue;

#endif // defined(useButtonInput)
static const uint8_t guinosig =		0b10110111;

// start of remarkably long EEPROM stored settings section

/* parameter sizes in bits */

static const uint8_t pSizeSignature =					32;					// pSignatureIdx
static const uint8_t pSizeMetricMode =					1;					// pMetricModeIdx
static const uint8_t pSizeAlternateFE =					1;					// pAlternateFEidx
static const uint8_t pSizeMicroSecondsPerGallon =		32;					// pMicroSecondsPerGallonIdx
static const uint8_t pSizeInjEdgeTrigger =				2;					// pInjEdgeTriggerIdx
static const uint8_t pSizeInjectorSettleTime =			16;					// pInjectorSettleTimeIdx
static const uint8_t pSizeInjPer2CrankRev =				8;					// pInjPer2CrankRevIdx
static const uint8_t pSizeMinGoodRPM =					16;					// pMinGoodRPMidx
static const uint8_t pSizePulseEdgePerDistance =		18;					// pPulseEdgePerDistanceIdx
static const uint8_t pSizeVSSpause =					8;					// pVSSpauseIdx
static const uint8_t pSizeMinGoodSpeed =				16;					// pMinGoodSpeedidx
static const uint8_t pSizeTankSize =					24;					// pTankSizeIdx
static const uint8_t pSizeTankBingoSize =				pSizeTankSize;		// pTankBingoSizeIdx
static const uint8_t pSizeIdleTimeout =					8;					// pIdleTimeoutIdx
static const uint8_t pSizeEOCtimeout =					8;					// pEOCtimeoutIdx
static const uint8_t pSizeButtonTimeout =				8;					// pButtonTimeoutIdx
static const uint8_t pSizeParkTimeout =					8;					// pParkTimeoutIdx
static const uint8_t pSizeActivityTimeout =				8;					// pActivityTimeoutIdx
static const uint8_t pSizeScratchpad =					32;					// pScratchpadIdx

#if defined(useButtonInput)
static const uint8_t pSizeWakeupResetCurrentOnEngine =	1;					// pWakeupResetCurrentOnEngineIdx
static const uint8_t pSizeWakeupResetCurrentOnMove =	1;					// pWakeupResetCurrentOnMoveIdx
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
#if defined(useBinaryLCDbrightness)
static const uint8_t pSizeBrightness =					1;					// pBrightnessIdx
#else // defined(useBinaryLCDbrightness)
static const uint8_t pSizeBrightness =					2;					// pBrightnessIdx
#endif // defined(useBinaryLCDbrightness)
#if defined(useLCDcontrast)
static const uint8_t pSizeContrast =					1;					// pContrastIdx
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
static const uint8_t pSizeLCDcolor =					3;					// pLCDcolorIdx
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
static const uint8_t pSizeFuelUnitCost =				24;
#endif // defined(useFuelCost)
#if defined(useOutputPins)
static const uint8_t pSizeOutputPin1Mode =				3;
static const uint8_t pSizeOutputPin2Mode =				3;
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
static const uint8_t pSizeVoltageOffset =				12;
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
static const uint8_t pSizeSerialDataLogging =			1;
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
static const uint8_t pSizeJSONoutput =					1;
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
static const uint8_t pSizeBluetoothOutput =				1;
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
static const uint8_t pSizeFEvsTime =					16;
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t pSizeBarLowSpeedCutoff =			24;
static const uint8_t pSizeBarSpeedQuantumIdx =			24;
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
static const uint8_t pSizeSysFuelPressure =				32;					// pSysFuelPressureIdx
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
static const uint8_t pSizeRefFuelPressure =				32;					// pRefFuelPressureIdx
static const uint8_t pSizeInjectorCount =				8; 					// pInjectorCountIdx
static const uint8_t pSizeInjectorSize =				20;					// pInjectorSizeIdx
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
static const uint8_t pSizeMAPsensorFloor =				16;					// pMAPsensorFloorIdx
static const uint8_t pSizeMAPsensorCeiling =			16;					// pMAPsensorCeilingIdx
static const uint8_t pSizeMAPsensorRange =				32;					// pMAPsensorRangeIdx
static const uint8_t pSizeMAPsensorOffset =				32;					// pMAPsensorOffsetIdx
#if defined(useChryslerBaroSensor)
static const uint8_t pSizeBaroSensorFloor =				16;					// pBaroSensorFloorIdx
static const uint8_t pSizeBaroSensorCeiling =			16;					// pBaroSensorCeilingIdx
static const uint8_t pSizeBaroSensorRange =				32;					// pBaroSensorRangeIdx
static const uint8_t pSizeBaroSensorOffset =			32;					// pBaroSensorOffsetIdx
#else // defined(useChryslerBaroSensor)
static const uint8_t pSizeBarometricPressure =			32;					// pBarometricPressureIdx
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint8_t pSizeVehicleMass =					16;					// pVehicleMassIdx
#if defined(useCoastDownCalculator)
static const uint8_t pSizeVehicleFrontalArea =			16;					// pVehicleFrontalAreaIdx
static const uint8_t pSizeLocustDensity =				16;					// pLocustDensityIdx
static const uint8_t pSizeCoefficientD =				16;					// pCoefficientDidx
static const uint8_t pSizeCoefficientV =				16;					// pCoefficientVidx
static const uint8_t pSizeCoefficientRR =				16;					// pCoefficientRRidx
static const uint8_t pSizeCoastdownSamplePeriod =		8; 					// pCoastdownSamplePeriodIdx
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
static const uint8_t pSizeDragSpeed =					24;					// pDragSpeedIdx
static const uint8_t pSizeDragDistance =				16;					// pDragDistanceIdx
static const uint8_t pSizeDragAutoFlag =				1; 					// pDragAutoFlagIdx
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
static const uint8_t pSizeAutoSaveActive =				1;					// pAutoSaveActiveIdx
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
static const uint8_t pSizeRefuelSize =					pSizeTankSize;		// pRefuelSizeIdx
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
static const uint8_t pSizePeakHoldSimPeriod =			16;					// pPeakHoldSimPeriodIdx
static const uint8_t pSizePeakHoldSimActive =			16;					// pPeakHoldSimActiveIdx
#endif // defined(useSimulatedFIandVSS)

#if defined(useButtonInput)
static const uint8_t pSizeDisplayIdx =					8;					// pDisplayIdx
#if LCDcharHeight == 4
static const uint8_t pSizeBottomDisplayIdx =			8;					// pBottomDisplayIdx
static const uint8_t pSizeBottomCursorIdx =				8;					// pBottomCursorIdx
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
static const uint8_t pSizeRefuelSaveSizeIdx =			pSizeTankSize;		// pRefuelSaveSizeIdx
#endif // defined(usePartialRefuel)
static const uint8_t pSizeCurrTripSignatureIdx =		8;					// pCurrTripSignatureIdx
static const uint8_t pSizeTankTripSignatureIdx =		8;					// pTankTripSignatureIdx
static const uint8_t pSizeCurrTripVSSpulseIdx =			32;					// pCurrTripVSSpulseIdx
static const uint8_t pSizeCurrTripVSScycleIdx =			64;					// pCurrTripVSScycleIdx
static const uint8_t pSizeCurrTripInjPulseIdx =			32;					// pCurrTripInjPulseIdx
static const uint8_t pSizeCurrTripInjCycleIdx =			64;					// pCurrTripInjCycleIdx
static const uint8_t pSizeCurrTripEngCycleIdx =			64;					// pCurrTripEngCycleIdx
static const uint8_t pSizeTankTripVSSpulseIdx =			32;					// pTankTripVSSpulseIdx
static const uint8_t pSizeTankTripVSScycleIdx =			64;					// pTankTripVSScycleIdx
static const uint8_t pSizeTankTripInjPulseIdx =			32;					// pTankTripInjPulseIdx
static const uint8_t pSizeTankTripInjCycleIdx =			64;					// pTankTripInjCycleIdx
static const uint8_t pSizeTankTripEngCycleIdx =			64;					// pTankTripEngCycleIdx
#if defined(trackIdleEOCdata)
static const uint8_t pSizeCurrIEOCvssPulseIdx =			32;					// pCurrIEOCvssPulseIdx
static const uint8_t pSizeCurrIEOCvssCycleIdx =			64;					// pCurrIEOCvssCycleIdx
static const uint8_t pSizeCurrIEOCinjPulseIdx =			32;					// pCurrIEOCinjPulseIdx
static const uint8_t pSizeCurrIEOCinjCycleIdx =			64;					// pCurrIEOCinjCycleIdx
static const uint8_t pSizeCurrIEOCengCycleIdx =			64;					// pCurrIEOCengCycleIdx
static const uint8_t pSizeTankIEOCvssPulseIdx =			32;					// pTankIEOCvssPulseIdx
static const uint8_t pSizeTankIEOCvssCycleIdx =			64;					// pTankIEOCvssCycleIdx
static const uint8_t pSizeTankIEOCinjPulseIdx =			32;					// pTankIEOCinjPulseIdx
static const uint8_t pSizeTankIEOCinjCycleIdx =			64;					// pTankIEOCinjCycleIdx
static const uint8_t pSizeTankIEOCengCycleIdx =			64;					// pTankIEOCengCycleIdx
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)

#define byteSize(bitLength) ((((bitLength & 0x07) != 0)? 1 : 0) + (bitLength / 8))

/* parameter addresses from sizes defined above */

static const uint16_t eeAdrSignature = 0;

#define nextAllowedValue eeAdrSignature
static const uint16_t pAddressSignature =					nextAllowedValue;	// pSignatureIdx
static const uint16_t pAddressMetricMode =					pAddressSignature + byteSize(pSizeSignature);	// pMetricModeIdx
static const uint16_t pAddressAlternateFE =					pAddressMetricMode + byteSize(pSizeMetricMode);	// pAlternateFEidx
static const uint16_t pAddressMicroSecondsPerGallon =		pAddressAlternateFE + byteSize(pSizeAlternateFE);	// pMicroSecondsPerGallonIdx
static const uint16_t pAddressInjEdgeTrigger =				pAddressMicroSecondsPerGallon + byteSize(pSizeMicroSecondsPerGallon);	// pInjEdgeTriggerIdx
static const uint16_t pAddressInjectorSettleTime =			pAddressInjEdgeTrigger + byteSize(pSizeInjEdgeTrigger);	// pInjectorSettleTimeIdx
static const uint16_t pAddressInjPer2CrankRev =				pAddressInjectorSettleTime + byteSize(pSizeInjectorSettleTime);	// pInjPer2CrankRevIdx
static const uint16_t pAddressMinGoodRPM =					pAddressInjPer2CrankRev + byteSize(pSizeInjPer2CrankRev);	// pMinGoodRPMidx
static const uint16_t pAddressPulseEdgePerDistance =		pAddressMinGoodRPM + byteSize(pSizeMinGoodRPM);	// pPulseEdgePerDistanceIdx
static const uint16_t pAddressVSSpause =					pAddressPulseEdgePerDistance + byteSize(pSizePulseEdgePerDistance);	// pVSSpauseIdx
static const uint16_t pAddressMinGoodSpeed =				pAddressVSSpause + byteSize(pSizeVSSpause);	// pMinGoodSpeedidx
static const uint16_t pAddressTankSize =					pAddressMinGoodSpeed + byteSize(pSizeMinGoodSpeed);	// pTankSizeIdx
static const uint16_t pAddressTankBingoSize =				pAddressTankSize + byteSize(pSizeTankSize);	// pTankBingoSizeIdx
static const uint16_t pAddressIdleTimeout =					pAddressTankBingoSize + byteSize(pSizeTankBingoSize);	// pIdleTimeoutIdx
static const uint16_t pAddressEOCtimeout =					pAddressIdleTimeout + byteSize(pSizeIdleTimeout);	// pEOCtimeoutIdx
static const uint16_t pAddressButtonTimeout =				pAddressEOCtimeout + byteSize(pSizeEOCtimeout);	// pButtonTimeoutIdx
static const uint16_t pAddressParkTimeout =					pAddressButtonTimeout + byteSize(pSizeButtonTimeout);	// pParkTimeoutIdx
static const uint16_t pAddressActivityTimeout =				pAddressParkTimeout + byteSize(pSizeParkTimeout);	// pActivityTimeoutIdx
static const uint16_t pAddressScratchpad =					pAddressActivityTimeout + byteSize(pSizeActivityTimeout);	// pScratchpadIdx
#define nextAllowedValue pAddressScratchpad + byteSize(pSizeScratchpad)

#if defined(useButtonInput)
static const uint16_t pAddressWakeupResetCurrentOnEngine =	nextAllowedValue;	// pWakeupResetCurrentOnEngineIdx
static const uint16_t pAddressWakeupResetCurrentOnMove =	pAddressWakeupResetCurrentOnEngine + byteSize(pSizeWakeupResetCurrentOnEngine);	// pWakeupResetCurrentOnMoveIdx
#define nextAllowedValue pAddressWakeupResetCurrentOnMove + byteSize(pSizeWakeupResetCurrentOnMove)
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
static const uint16_t pAddressBrightness =					nextAllowedValue;	// pBrightnessIdx
#define nextAllowedValue pAddressBrightness + byteSize(pSizeBrightness)
#if defined(useLCDcontrast)
static const uint16_t pAddressContrast =					nextAllowedValue;	// pContrastIdx
#define nextAllowedValue pAddressContrast + byteSize(pSizeContrast)
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
static const uint16_t pAddressLCDcolor =					nextAllowedValue;	// pLCDcolorIdx
#define nextAllowedValue pAddressLCDcolor + byteSize(pSizeLCDcolor)
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
static const uint16_t pAddressFuelUnitCost =				nextAllowedValue;
#define nextAllowedValue pAddressFuelUnitCost + byteSize(pSizeFuelUnitCost)
#endif // defined(useFuelCost)
#if defined(useOutputPins)
static const uint16_t pAddressOutputPin1Mode =				nextAllowedValue;
static const uint16_t pAddressOutputPin2Mode =				pAddressOutputPin1Mode + byteSize(pSizeOutputPin1Mode);
#define nextAllowedValue pAddressOutputPin2Mode + byteSize(pSizeOutputPin2Mode)
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
static const uint16_t pAddressVoltageOffset =				nextAllowedValue;
#define nextAllowedValue pAddressVoltageOffset + byteSize(pSizeVoltageOffset)
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
static const uint16_t pAddressSerialDataLogging =			nextAllowedValue;
#define nextAllowedValue pAddressSerialDataLogging + byteSize(pSizeSerialDataLogging)
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
static const uint16_t pAddressJSONoutput =					nextAllowedValue;
#define nextAllowedValue pAddressJSONoutput + byteSize(pSizeJSONoutput)
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
static const uint16_t pAddressBluetoothOutput =				nextAllowedValue;
#define nextAllowedValue pAddressBluetoothOutput + byteSize(pSizeBluetoothOutput)
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
static const uint16_t pAddressFEvsTime =					nextAllowedValue;
#define nextAllowedValue pAddressFEvsTime + byteSize(pSizeFEvsTime)
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
static const uint16_t pAddressBarLowSpeedCutoff =			nextAllowedValue;
static const uint16_t pAddressBarSpeedQuantumIdx =			pAddressBarLowSpeedCutoff + byteSize(pSizeBarLowSpeedCutoff);
#define nextAllowedValue pAddressBarSpeedQuantumIdx + byteSize(pSizeBarSpeedQuantumIdx)
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
static const uint16_t pAddressSysFuelPressure =				nextAllowedValue;	// pSysFuelPressureIdx
#define nextAllowedValue pAddressSysFuelPressure + byteSize(pSizeSysFuelPressure)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
static const uint16_t pAddressRefFuelPressure =				nextAllowedValue;	// pRefFuelPressureIdx
static const uint16_t pAddressInjectorCount =				pAddressRefFuelPressure + byteSize(pSizeRefFuelPressure);	// pInjectorCountIdx
static const uint16_t pAddressInjectorSize =				pAddressInjectorCount + byteSize(pSizeInjectorCount);	// pInjectorSizeIdx
#define nextAllowedValue pAddressInjectorSize + byteSize(pSizeInjectorSize)
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
static const uint16_t pAddressMAPsensorFloor =				nextAllowedValue;	// pMAPsensorFloorIdx
static const uint16_t pAddressMAPsensorCeiling =			pAddressMAPsensorFloor + byteSize(pSizeMAPsensorFloor);	// pMAPsensorCeilingIdx
static const uint16_t pAddressMAPsensorRange =				pAddressMAPsensorCeiling + byteSize(pSizeMAPsensorCeiling);	// pMAPsensorRangeIdx
static const uint16_t pAddressMAPsensorOffset =				pAddressMAPsensorRange + byteSize(pSizeMAPsensorRange);	// pMAPsensorOffsetIdx
#define nextAllowedValue pAddressMAPsensorOffset + byteSize(pSizeMAPsensorOffset)
#if defined(useChryslerBaroSensor)
static const uint16_t pAddressBaroSensorFloor =				nextAllowedValue;	// pBaroSensorFloorIdx
static const uint16_t pAddressBaroSensorCeiling =			pAddressBaroSensorFloor + byteSize(pSizeBaroSensorFloor);	// pBaroSensorCeilingIdx
static const uint16_t pAddressBaroSensorRange =				pAddressBaroSensorCeiling + byteSize(pSizeBaroSensorCeiling);	// pBaroSensorRangeIdx
static const uint16_t pAddressBaroSensorOffset =			pAddressBaroSensorRange + byteSize(pSizeBaroSensorRange);	// pBaroSensorOffsetIdx
#define nextAllowedValue pAddressBaroSensorOffset + byteSize(pSizeBaroSensorOffset)
#else // defined(useChryslerBaroSensor)
static const uint16_t pAddressBarometricPressure =			nextAllowedValue;	// pBarometricPressureIdx
#define nextAllowedValue pAddressBarometricPressure + byteSize(pSizeBarometricPressure)
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint16_t pAddressVehicleMass =					nextAllowedValue;	// pVehicleMassIdx
#define nextAllowedValue pAddressVehicleMass + byteSize(pSizeVehicleMass)
#if defined(useCoastDownCalculator)
static const uint16_t pAddressVehicleFrontalArea =			nextAllowedValue;	// pVehicleFrontalAreaIdx
static const uint16_t pAddressLocustDensity =				pAddressVehicleFrontalArea + byteSize(pSizeVehicleFrontalArea);	// pLocustDensityIdx
static const uint16_t pAddressCoefficientD =				pAddressLocustDensity + byteSize(pSizeLocustDensity);	// pCoefficientDidx
static const uint16_t pAddressCoefficientV =				pAddressCoefficientD + byteSize(pSizeCoefficientD);	// pCoefficientVidx
static const uint16_t pAddressCoefficientRR =				pAddressCoefficientV + byteSize(pSizeCoefficientV);	// pCoefficientRRidx
static const uint16_t pAddressCoastdownSamplePeriod =		pAddressCoefficientRR + byteSize(pSizeCoefficientRR);	// pCoastdownSamplePeriodIdx
#define nextAllowedValue pAddressCoastdownSamplePeriod + byteSize(pSizeCoastdownSamplePeriod)
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
static const uint16_t pAddressDragSpeed =					nextAllowedValue;	// pDragSpeedIdx
static const uint16_t pAddressDragDistance =				pAddressDragSpeed + byteSize(pSizeDragSpeed);	// pDragDistanceIdx
static const uint16_t pAddressDragAutoFlag =				pAddressDragDistance + byteSize(pSizeDragDistance);	// pDragAutoFlagIdx
#define nextAllowedValue pAddressDragAutoFlag + byteSize(pSizeDragAutoFlag)
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
static const uint16_t pAddressAutoSaveActive =				nextAllowedValue;	// pAutoSaveActiveIdx
#define nextAllowedValue pAddressAutoSaveActive + byteSize(pSizeAutoSaveActive)
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
static const uint16_t pAddressRefuelSize =					nextAllowedValue;	// pRefuelSizeIdx
#define nextAllowedValue pAddressRefuelSize + byteSize(pSizeRefuelSize)
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
static const uint8_t pAddressPeakHoldSimPeriod =			nextAllowedValue;	// pPeakHoldSimPeriodIdx
static const uint8_t pAddressPeakHoldSimActive =			pAddressPeakHoldSimPeriod + byteSize(pSizePeakHoldSimPeriod);	// pPeakHoldSimActiveIdx
#define nextAllowedValue pAddressPeakHoldSimActive + byteSize(pSizePeakHoldSimActive)
#endif // defined(useSimulatedFIandVSS)

#if defined(useButtonInput)
static const uint16_t pAddressDisplayIdx =					nextAllowedValue;	// pDisplayIdx
#define nextAllowedValue pAddressDisplayIdx + byteSize(pSizeDisplayIdx)
#if LCDcharHeight == 4
static const uint16_t pAddressBottomDisplayIdx =			nextAllowedValue;	// pBottomDisplayIdx
static const uint16_t pAddressBottomCursorIdx =				pAddressBottomDisplayIdx + byteSize(pSizeBottomDisplayIdx);	// pBottomCursorIdx
#define nextAllowedValue pAddressBottomCursorIdx + byteSize(pSizeBottomCursorIdx)
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
static const uint16_t pAddressRefuelSaveSizeIdx =			nextAllowedValue;	// pRefuelSaveSizeIdx
#define nextAllowedValue pAddressRefuelSaveSizeIdx + byteSize(pSizeRefuelSaveSizeIdx)
#endif // defined(usePartialRefuel)
static const uint16_t pAddressCurrTripSignatureIdx =		nextAllowedValue;	// pCurrTripSignatureIdx
static const uint16_t pAddressTankTripSignatureIdx =		pAddressCurrTripSignatureIdx + byteSize(pSizeCurrTripSignatureIdx);	// pTankTripSignatureIdx
static const uint16_t pAddressCurrTripVSSpulseIdx =			pAddressTankTripSignatureIdx + byteSize(pSizeTankTripSignatureIdx);	// pCurrTripVSSpulseIdx
static const uint16_t pAddressCurrTripVSScycleIdx =			pAddressCurrTripVSSpulseIdx + byteSize(pSizeCurrTripVSSpulseIdx);	// pCurrTripVSScycleIdx
static const uint16_t pAddressCurrTripInjPulseIdx =			pAddressCurrTripVSScycleIdx + byteSize(pSizeCurrTripVSScycleIdx);	// pCurrTripInjPulseIdx
static const uint16_t pAddressCurrTripInjCycleIdx =			pAddressCurrTripInjPulseIdx + byteSize(pSizeCurrTripInjPulseIdx);	// pCurrTripInjCycleIdx
static const uint16_t pAddressCurrTripEngCycleIdx =			pAddressCurrTripInjCycleIdx + byteSize(pSizeCurrTripInjCycleIdx);	// pCurrTripEngCycleIdx
static const uint16_t pAddressTankTripVSSpulseIdx =			pAddressCurrTripEngCycleIdx + byteSize(pSizeCurrTripEngCycleIdx);	// pTankTripVSSpulseIdx
static const uint16_t pAddressTankTripVSScycleIdx =			pAddressTankTripVSSpulseIdx + byteSize(pSizeTankTripVSSpulseIdx);	// pTankTripVSScycleIdx
static const uint16_t pAddressTankTripInjPulseIdx =			pAddressTankTripVSScycleIdx + byteSize(pSizeTankTripVSScycleIdx);	// pTankTripInjPulseIdx
static const uint16_t pAddressTankTripInjCycleIdx =			pAddressTankTripInjPulseIdx + byteSize(pSizeTankTripInjPulseIdx);	// pTankTripInjCycleIdx
static const uint16_t pAddressTankTripEngCycleIdx =			pAddressTankTripInjCycleIdx + byteSize(pSizeTankTripInjCycleIdx);	// pTankTripEngCycleIdx
#define nextAllowedValue pAddressTankTripEngCycleIdx + byteSize(pSizeTankTripEngCycleIdx)
#if defined(trackIdleEOCdata)
static const uint16_t pAddressCurrIEOCvssPulseIdx =			nextAllowedValue;	// pCurrIEOCvssPulseIdx
static const uint16_t pAddressCurrIEOCvssCycleIdx =			pAddressCurrIEOCvssPulseIdx + byteSize(pSizeCurrIEOCvssPulseIdx);	// pCurrIEOCvssCycleIdx
static const uint16_t pAddressCurrIEOCinjPulseIdx =			pAddressCurrIEOCvssCycleIdx + byteSize(pSizeCurrIEOCvssCycleIdx);	// pCurrIEOCinjPulseIdx
static const uint16_t pAddressCurrIEOCinjCycleIdx =			pAddressCurrIEOCinjPulseIdx + byteSize(pSizeCurrIEOCinjPulseIdx);	// pCurrIEOCinjCycleIdx
static const uint16_t pAddressCurrIEOCengCycleIdx =			pAddressCurrIEOCinjCycleIdx + byteSize(pSizeCurrIEOCinjCycleIdx);	// pCurrIEOCengCycleIdx
static const uint16_t pAddressTankIEOCvssPulseIdx =			pAddressCurrIEOCengCycleIdx + byteSize(pSizeCurrIEOCengCycleIdx);	// pTankIEOCvssPulseIdx
static const uint16_t pAddressTankIEOCvssCycleIdx =			pAddressTankIEOCvssPulseIdx + byteSize(pSizeTankIEOCvssPulseIdx);	// pTankIEOCvssCycleIdx
static const uint16_t pAddressTankIEOCinjPulseIdx =			pAddressTankIEOCvssCycleIdx + byteSize(pSizeTankIEOCvssCycleIdx);	// pTankIEOCinjPulseIdx
static const uint16_t pAddressTankIEOCinjCycleIdx =			pAddressTankIEOCinjPulseIdx + byteSize(pSizeTankIEOCinjPulseIdx);	// pTankIEOCinjCycleIdx
static const uint16_t pAddressTankIEOCengCycleIdx =			pAddressTankIEOCinjCycleIdx + byteSize(pSizeTankIEOCinjCycleIdx);	// pTankIEOCengCycleIdx
#define nextAllowedValue pAddressTankIEOCengCycleIdx + byteSize(pSizeTankIEOCengCycleIdx)
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)

static const uint16_t eeAdrSettingsEnd =					nextAllowedValue;

#if defined(useScreenEditor)
static const uint16_t eeAdrScreensStart =					nextAllowedValue;
static const uint16_t eeAdrScreensEnd =						eeAdrScreensStart + 2 * mainDisplayFormatSize;
#define nextAllowedValue eeAdrScreensEnd

#endif // defined(useScreenEditor)
#if defined(useButtonInput)
static const uint16_t eeAdrDisplayCursorStart =				nextAllowedValue;
static const uint16_t eeAdrDisplayCursorEnd =				eeAdrDisplayCursorStart + displayCountTotal;
static const uint16_t eeAdrMenuCursorStart =				eeAdrDisplayCursorEnd;
static const uint16_t eeAdrMenuCursorEnd =					eeAdrMenuCursorStart + displayCountMenu;
#define nextAllowedValue eeAdrMenuCursorEnd
#endif // defined(useButtonInput)

static const uint16_t eeAdrStorageEnd =						nextAllowedValue;

#define nextAllowedValue 0

/* parameter indexes */

static const uint8_t pSettingsIdxStart =				nextAllowedValue;

static const uint8_t pSignatureIdx =					nextAllowedValue;
static const uint8_t pMetricModeIdx =					pSignatureIdx + 1;
static const uint8_t pAlternateFEidx =					pMetricModeIdx + 1;
static const uint8_t pMicroSecondsPerGallonIdx =		pAlternateFEidx + 1;
static const uint8_t pInjEdgeTriggerIdx =				pMicroSecondsPerGallonIdx + 1;
static const uint8_t pInjectorSettleTimeIdx =			pInjEdgeTriggerIdx + 1;
static const uint8_t pInjPer2CrankRevIdx =				pInjectorSettleTimeIdx + 1;
static const uint8_t pMinGoodRPMidx =					pInjPer2CrankRevIdx + 1;
static const uint8_t pPulseEdgePerDistanceIdx =			pMinGoodRPMidx + 1;
static const uint8_t pVSSpauseIdx =						pPulseEdgePerDistanceIdx + 1;
static const uint8_t pMinGoodSpeedidx =					pVSSpauseIdx + 1;
static const uint8_t pTankSizeIdx =						pMinGoodSpeedidx + 1;
static const uint8_t pTankBingoSizeIdx =				pTankSizeIdx + 1;
static const uint8_t pIdleTimeoutIdx =					pTankBingoSizeIdx + 1;
static const uint8_t pEOCtimeoutIdx =					pIdleTimeoutIdx + 1;
static const uint8_t pButtonTimeoutIdx =				pEOCtimeoutIdx + 1;
static const uint8_t pParkTimeoutIdx =					pButtonTimeoutIdx + 1;
static const uint8_t pActivityTimeoutIdx =				pParkTimeoutIdx + 1;
static const uint8_t pScratchpadIdx =					pActivityTimeoutIdx + 1;
#define nextAllowedValue pScratchpadIdx + 1

#if defined(useButtonInput)
static const uint8_t pWakeupResetCurrentOnEngineIdx =	nextAllowedValue;
static const uint8_t pWakeupResetCurrentOnMoveIdx =		pWakeupResetCurrentOnEngineIdx + 1;
#define nextAllowedValue pWakeupResetCurrentOnMoveIdx + 1
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
static const uint8_t pBrightnessIdx =					nextAllowedValue;
#define nextAllowedValue pBrightnessIdx + 1
#if defined(useLCDcontrast)
static const uint8_t pContrastIdx =						nextAllowedValue;
#define nextAllowedValue pContrastIdx + 1
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
static const uint8_t pLCDcolorIdx =						nextAllowedValue;
#define nextAllowedValue pLCDcolorIdx + 1
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
static const uint8_t pCostPerQuantity =					nextAllowedValue;
#define nextAllowedValue pCostPerQuantity + 1
#endif // defined(useFuelCost)
#if defined(useOutputPins)
static const uint8_t pOutputPin1Mode =					nextAllowedValue;
static const uint8_t pOutputPin2Mode =					pOutputPin1Mode + 1;
#define nextAllowedValue pOutputPin2Mode + 1
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
static const uint8_t pVoltageOffset =					nextAllowedValue;
#define nextAllowedValue pVoltageOffset + 1
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
static const uint8_t pSerialDataLoggingIdx =			nextAllowedValue;
#define nextAllowedValue pSerialDataLoggingIdx + 1
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
static const uint8_t pJSONoutputIdx =					nextAllowedValue;
#define nextAllowedValue pJSONoutputIdx + 1
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
static const uint8_t pBluetoothOutputIdx =				nextAllowedValue;
#define nextAllowedValue pBluetoothOutputIdx + 1
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
static const uint8_t pFEvsTimeIdx =						nextAllowedValue;
#define nextAllowedValue pFEvsTimeIdx + 1
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
static const uint8_t pBarLowSpeedCutoffIdx =			nextAllowedValue;
static const uint8_t pBarSpeedQuantumIdx =				pBarLowSpeedCutoffIdx + 1;
#define nextAllowedValue pBarSpeedQuantumIdx + 1
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
static const uint8_t pSysFuelPressureIdx =				nextAllowedValue;
#define nextAllowedValue pSysFuelPressureIdx + 1
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
static const uint8_t pRefFuelPressureIdx =				nextAllowedValue;
static const uint8_t pInjectorCountIdx =				pRefFuelPressureIdx + 1;
static const uint8_t pInjectorSizeIdx =					pInjectorCountIdx + 1;
#define nextAllowedValue pInjectorSizeIdx + 1
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
static const uint8_t pMAPsensorFloorIdx =				nextAllowedValue;
static const uint8_t pMAPsensorCeilingIdx =				pMAPsensorFloorIdx + 1;
static const uint8_t pMAPsensorRangeIdx =				pMAPsensorCeilingIdx + 1;
static const uint8_t pMAPsensorOffsetIdx =				pMAPsensorRangeIdx + 1;
#define nextAllowedValue pMAPsensorOffsetIdx + 1
#if defined(useChryslerBaroSensor)
static const uint8_t pBaroSensorFloorIdx =				nextAllowedValue;
static const uint8_t pBaroSensorCeilingIdx =			pBaroSensorFloorIdx + 1;
static const uint8_t pBaroSensorRangeIdx =				pBaroSensorCeilingIdx + 1;
static const uint8_t pBaroSensorOffsetIdx =				pBaroSensorRangeIdx + 1;
#define nextAllowedValue pBaroSensorOffsetIdx + 1
#else // defined(useChryslerBaroSensor)
static const uint8_t pBarometricPressureIdx =			nextAllowedValue;
#define nextAllowedValue pBarometricPressureIdx + 1
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint8_t pVehicleMassIdx =					nextAllowedValue;
#define nextAllowedValue pVehicleMassIdx + 1
#if defined(useCoastDownCalculator)
static const uint8_t pVehicleFrontalAreaIdx =			nextAllowedValue;
static const uint8_t pLocustDensityIdx =				pVehicleFrontalAreaIdx + 1;
static const uint8_t pCoefficientDidx =					pLocustDensityIdx + 1;
static const uint8_t pCoefficientVidx =					pCoefficientDidx + 1;
static const uint8_t pCoefficientRRidx =				pCoefficientVidx + 1;
static const uint8_t pCoastdownSamplePeriodIdx =		pCoefficientRRidx + 1;
#define nextAllowedValue pCoastdownSamplePeriodIdx + 1
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
static const uint8_t pDragSpeedIdx =					nextAllowedValue;
static const uint8_t pDragDistanceIdx =					pDragSpeedIdx + 1;
static const uint8_t pDragAutoFlagIdx =					pDragDistanceIdx + 1;
#define nextAllowedValue pDragAutoFlagIdx + 1
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
static const uint8_t pAutoSaveActiveIdx =				nextAllowedValue;
#define nextAllowedValue pAutoSaveActiveIdx + 1
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
static const uint8_t pRefuelSizeIdx =					nextAllowedValue;
#define nextAllowedValue pRefuelSizeIdx + 1
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
static const uint8_t pPeakHoldSimPeriodIdx =			nextAllowedValue;
static const uint8_t pPeakHoldSimActiveIdx =			pPeakHoldSimPeriodIdx + 1;
#define nextAllowedValue pPeakHoldSimActiveIdx + 1
#endif // defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
static const uint8_t pDisplayIdx =						nextAllowedValue;
#define nextAllowedValue pDisplayIdx + 1
#if LCDcharHeight == 4
static const uint8_t pBottomDisplayIdx =				nextAllowedValue;
#define nextAllowedValue pBottomDisplayIdx + 1
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)

static const uint8_t pSettingsIdxEnd =					nextAllowedValue;
static const uint8_t pSettingsIdxLen =					pSettingsIdxEnd - pSettingsIdxStart;

static const uint8_t pExpandedSettingsIdxStart =		nextAllowedValue;

#if defined(useButtonInput)
#if LCDcharHeight == 4
static const uint8_t pBottomCursorIdx =					nextAllowedValue;
#define nextAllowedValue pBottomCursorIdx + 1
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)

#if defined(useEEPROMtripStorage)
static const uint8_t eePtrSavedTripsStart = 			nextAllowedValue;

#if defined(usePartialRefuel)
static const uint8_t pRefuelSaveSizeIdx =				nextAllowedValue;
#define nextAllowedValue pRefuelSaveSizeIdx + 1
#endif // defined(usePartialRefuel)
static const uint8_t pCurrTripSignatureIdx =			nextAllowedValue;
static const uint8_t pTankTripSignatureIdx =			pCurrTripSignatureIdx + 1;
static const uint8_t pCurrTripVSSpulseIdx =				pTankTripSignatureIdx + 1;
static const uint8_t pCurrTripVSScycleIdx =				pCurrTripVSSpulseIdx + 1;
static const uint8_t pCurrTripInjPulseIdx =				pCurrTripVSScycleIdx + 1;
static const uint8_t pCurrTripInjCycleIdx =				pCurrTripInjPulseIdx + 1;
static const uint8_t pCurrTripEngCycleIdx =				pCurrTripInjCycleIdx + 1;
static const uint8_t pTankTripVSSpulseIdx =				pCurrTripEngCycleIdx + 1;
static const uint8_t pTankTripVSScycleIdx =				pTankTripVSSpulseIdx + 1;
static const uint8_t pTankTripInjPulseIdx =				pTankTripVSScycleIdx + 1;
static const uint8_t pTankTripInjCycleIdx =				pTankTripInjPulseIdx + 1;
static const uint8_t pTankTripEngCycleIdx =				pTankTripInjCycleIdx + 1;
#define nextAllowedValue pTankTripEngCycleIdx + 1
#if defined(trackIdleEOCdata)
static const uint8_t pCurrIEOCvssPulseIdx =				nextAllowedValue;
static const uint8_t pCurrIEOCvssCycleIdx =				pCurrIEOCvssPulseIdx + 1;
static const uint8_t pCurrIEOCinjPulseIdx =				pCurrIEOCvssCycleIdx + 1;
static const uint8_t pCurrIEOCinjCycleIdx =				pCurrIEOCinjPulseIdx + 1;
static const uint8_t pCurrIEOCengCycleIdx =				pCurrIEOCinjCycleIdx + 1;
static const uint8_t pTankIEOCvssPulseIdx =				pCurrIEOCengCycleIdx + 1;
static const uint8_t pTankIEOCvssCycleIdx =				pTankIEOCvssPulseIdx + 1;
static const uint8_t pTankIEOCinjPulseIdx =				pTankIEOCvssCycleIdx + 1;
static const uint8_t pTankIEOCinjCycleIdx =				pTankIEOCinjPulseIdx + 1;
static const uint8_t pTankIEOCengCycleIdx =				pTankIEOCinjCycleIdx + 1;
#define nextAllowedValue pTankIEOCengCycleIdx + 1
#endif // defined(trackIdleEOCdata)

static const uint8_t eePtrSavedTripsEnd = 				nextAllowedValue;
static const uint8_t eePtrSavedTripsLen = 				eePtrSavedTripsEnd - eePtrSavedTripsStart;

#endif // defined(useEEPROMtripStorage)
static const uint8_t eePtrStorageEnd =					nextAllowedValue;

#if defined(useScreenEditor)
static const uint8_t eePtrDisplayPagesStart =			nextAllowedValue;
static const uint8_t eePtrDisplayPagesEnd =				eePtrDisplayPagesStart + mainDisplayFormatSize;
#define nextAllowedValue eePtrDisplayPagesEnd

#endif // defined(useScreenEditor)
#if defined(useButtonInput)
static const uint16_t eePtrDisplayCursorStart =			nextAllowedValue;
static const uint16_t eePtrDisplayCursorEnd =			eePtrDisplayCursorStart + displayCountTotal;
static const uint16_t eePtrMenuHeightStart =			eePtrDisplayCursorEnd;
static const uint16_t eePtrMenuHeightEnd =				eePtrMenuHeightStart + displayCountMenu;
#define nextAllowedValue eePtrMenuHeightEnd
#endif // defined(useButtonInput)

static const uint8_t pExpandedSettingsIdxEnd =			nextAllowedValue;
static const uint8_t pExpandedSettingsIdxLen =			pExpandedSettingsIdxEnd - pExpandedSettingsIdxStart;

static const uint8_t eePtrEnd =							nextAllowedValue;

#if defined(useDebugTerminalLabels)
/* labels for parameters definitions above */

static const char terminalParameterNames[] PROGMEM = {
	"pSignatureIdx" tcEOS
	"pMetricModeIdx" tcEOS
	"pAlternateFEidx" tcEOS
	"pMicroSecondsPerGallonIdx" tcEOS
	"pInjEdgeTriggerIdx" tcEOS
	"pInjectorSettleTimeIdx" tcEOS
	"pInjPer2CrankRevIdx" tcEOS
	"pMinGoodRPMidx" tcEOS
	"pPulseEdgePerDistanceIdx" tcEOS
	"pVSSpauseIdx" tcEOS
	"pMinGoodSpeedidx" tcEOS
	"pTankSizeIdx" tcEOS
	"pTankBingoSizeIdx" tcEOS
	"pIdleTimeoutIdx" tcEOS
	"pEOCtimeoutIdx" tcEOS
	"pButtonTimeoutIdx" tcEOS
	"pParkTimeoutIdx" tcEOS
	"pActivityTimeoutIdx" tcEOS
	"pScratchpadIdx" tcEOS
#if defined(useButtonInput)
	"pWakeupResetCurrentOnEngineIdx" tcEOS
	"pWakeupResetCurrentOnMoveIdx" tcEOS
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
	"pBrightnessIdx" tcEOS
#if defined(useLCDcontrast)
	"pContrastIdx" tcEOS
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
	"pLCDcolorIdx" tcEOS
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	"pCostPerQuantity" tcEOS
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	"pOutputPin1Mode" tcEOS
	"pOutputPin2Mode" tcEOS
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
	"pVoltageOffset" tcEOS
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
	"pSerialDataLoggingIdx" tcEOS
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	"pJSONoutputIdx" tcEOS
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	"pBluetoothOutputIdx" tcEOS
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
	"pFEvsTimeIdx" tcEOS
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	"pBarLowSpeedCutoffIdx" tcEOS
	"pBarSpeedQuantumIdx" tcEOS
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
	"pSysFuelPressureIdx" tcEOS
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	"pRefFuelPressureIdx" tcEOS
	"pInjectorCountIdx" tcEOS
	"pInjectorSizeIdx" tcEOS
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	"pMAPsensorFloorIdx" tcEOS
	"pMAPsensorCeilingIdx" tcEOS
	"pMAPsensorRangeIdx" tcEOS
	"pMAPsensorOffsetIdx" tcEOS
#if defined(useChryslerBaroSensor)
	"pBaroSensorFloorIdx" tcEOS
	"pBaroSensorCeilingIdx" tcEOS
	"pBaroSensorRangeIdx" tcEOS
	"pBaroSensorOffsetIdx" tcEOS
#else // defined(useChryslerBaroSensor)
	"pBarometricPressureIdx" tcEOS
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	"pVehicleMassIdx" tcEOS
#if defined(useCoastDownCalculator)
	"pVehicleFrontalAreaIdx" tcEOS
	"pLocustDensityIdx" tcEOS
	"pCoefficientDidx" tcEOS
	"pCoefficientVidx" tcEOS
	"pCoefficientRRidx" tcEOS
	"pCoastdownSamplePeriodIdx" tcEOS
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	"pDragSpeedIdx" tcEOS
	"pDragDistanceIdx" tcEOS
	"pDragAutoFlagIdx" tcEOS
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	"pAutoSaveActiveIdx" tcEOS
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	"pRefuelSizeIdx" tcEOS
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
	"pPeakHoldSimPeriodIdx" tcEOS
	"pPeakHoldSimActiveIdx" tcEOS
#endif // defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
	"pDisplayIdx" tcEOS
#if LCDcharHeight == 4
	"pBottomDisplayIdx" tcEOS
	"pBottomCursorIdx" tcEOS
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
};


static const char terminalExpandedParameterNames[] PROGMEM = {
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
	"pRefuelSaveSizeIdx" tcEOS
#endif // defined(usePartialRefuel)
	"pCurrTripSignatureIdx" tcEOS
	"pTankTripSignatureIdx" tcEOS
	"pCurrTripVSSpulseIdx" tcEOS
	"pCurrTripVSScycleIdx" tcEOS
	"pCurrTripInjPulseIdx" tcEOS
	"pCurrTripInjCycleIdx" tcEOS
	"pCurrTripEngCycleIdx" tcEOS
	"pTankTripVSSpulseIdx" tcEOS
	"pTankTripVSScycleIdx" tcEOS
	"pTankTripInjPulseIdx" tcEOS
	"pTankTripInjCycleIdx" tcEOS
	"pTankTripEngCycleIdx" tcEOS
#if defined(trackIdleEOCdata)
	"pCurrIEOCvssPulseIdx" tcEOS
	"pCurrIEOCvssCycleIdx" tcEOS
	"pCurrIEOCinjPulseIdx" tcEOS
	"pCurrIEOCinjCycleIdx" tcEOS
	"pCurrIEOCengCycleIdx" tcEOS
	"pTankIEOCvssPulseIdx" tcEOS
	"pTankIEOCvssCycleIdx" tcEOS
	"pTankIEOCinjPulseIdx" tcEOS
	"pTankIEOCinjCycleIdx" tcEOS
	"pTankIEOCengCycleIdx" tcEOS
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
#if defined(useScreenEditor)
	"P00F00" tcEOS
	"P00F01" tcEOS
	"P00F02" tcEOS
	"P00F03" tcEOS

	"P01F00" tcEOS
	"P01F01" tcEOS
	"P01F02" tcEOS
	"P01F03" tcEOS

	"P02F00" tcEOS
	"P02F01" tcEOS
	"P02F02" tcEOS
	"P02F03" tcEOS

	"P03F00" tcEOS
	"P03F01" tcEOS
	"P03F02" tcEOS
	"P03F03" tcEOS

	"P04F00" tcEOS
	"P04F01" tcEOS
	"P04F02" tcEOS
	"P04F03" tcEOS

	"P05F00" tcEOS
	"P05F01" tcEOS
	"P05F02" tcEOS
	"P05F03" tcEOS

#if defined(trackIdleEOCdata)
	"P06F00" tcEOS
	"P06F01" tcEOS
	"P06F02" tcEOS
	"P06F03" tcEOS

#endif // defined(trackIdleEOCdata)
	"P07F00" tcEOS
	"P07F01" tcEOS
	"P07F02" tcEOS
	"P07F03" tcEOS

	"P08F00" tcEOS
	"P08F01" tcEOS
	"P08F02" tcEOS
	"P08F03" tcEOS

#if defined(trackIdleEOCdata)
	"P09F00" tcEOS
	"P09F01" tcEOS
	"P09F02" tcEOS
	"P09F03" tcEOS

	"P10F00" tcEOS
	"P10F01" tcEOS
	"P10F02" tcEOS
	"P10F03" tcEOS

#endif // defined(trackIdleEOCdata)
	"P11F00" tcEOS
	"P11F01" tcEOS
	"P11F02" tcEOS
	"P11F03" tcEOS

#endif // defined(useScreenEditor)

#if defined(useButtonInput)
	"baseMenuDisplayIdx" tcEOS
#if defined(useExpandedMainDisplay)
	"mainMenuDisplayIdx" tcEOS
#endif // defined(useExpandedMainDisplay)
	"settingsMenuDisplayIdx" tcEOS

	"displaySettingsDisplayIdx" tcEOS
	"fuelSettingsDisplayIdx" tcEOS
	"VSSsettingsDisplayIdx" tcEOS
	"tankSettingsDisplayIdx" tcEOS
#if defined(useChryslerMAPCorrection)
	"CRFICsettingsDisplayIdx" tcEOS
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	"acdSettingsDisplayIdx" tcEOS
#endif // defined(useVehicleParameters)
	"timeoutSettingsDisplayIdx" tcEOS
	"miscSettingsDisplayIdx" tcEOS

#if defined(useSavedTrips)
	"tripSaveCurrentDisplayIdx" tcEOS
#endif // defined(useSavedTrips)
#if defined(useEnhancedTripReset)
	"tripSaveTankDisplayIdx" tcEOS
#endif // defined(useEnhancedTripReset)

#if defined(useDragRaceFunction)
	"dragRaceMenuDisplayIdx" tcEOS
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"coastdownMenuDisplayIdx" tcEOS
#endif // defined(useCoastDownCalculator)
#if defined(useSimulatedFIandVSS)
	"signalSimDisplayIdx" tcEOS
#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
	"pressureDisplayIdx" tcEOS
#endif // defined(useChryslerMAPCorrection)
#if defined(useDebugAnalog)
	"analogDisplayIdx" tcEOS
#endif // defined(useDebugAnalog)

	"mainDisplayIdx" tcEOS
#if defined(useStatusMeter)
	"statusMeterDisplayIdx" tcEOS
#endif // defined(useStatusMeter)
#if defined(useBigFE)
	"bigFEdisplayIdx" tcEOS
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
	"barFEvTdisplayIdx" tcEOS
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	"barFEvSdisplayIdx" tcEOS
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
	"bigDTEdisplayIdx" tcEOS
#endif // defined(useBigDTE)
#if defined(useBigTTE)
	"bigTTEdisplayIdx" tcEOS
#endif // defined(useBigTTE)
#if defined(useCPUreading)
	"CPUmonDisplayIdx" tcEOS
#endif // defined(useCPUreading)
#if defined(useClockDisplay)
	"clockShowDisplayIdx" tcEOS
#endif // defined(useClockDisplay)

	"parameterEditDisplayIdx" tcEOS
#if defined(useClockDisplay)
	"clockSetDisplayIdx" tcEOS
#endif // defined(useClockDisplay)
#if defined(useScreenEditor)
	"displayEditDisplayIdx" tcEOS
#endif // defined(useScreenEditor)
#if defined(useDragRaceFunction)
	"dragRaceDisplayIdx" tcEOS
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"coastdownDisplayIdx" tcEOS
#endif // defined(useCoastDownCalculator)

	"topMenuIdx" tcEOS

#if defined(useExpandedMainDisplay)
	"mainMenuIdx" tcEOS

#endif // defined(useExpandedMainDisplay)
	"settingsMenuIdx" tcEOS

	"displaySettingMenuIdx" tcEOS
	"fuelSettingMenuIdx" tcEOS
	"VSSsettingMenuIdx" tcEOS
	"tankSettingMenuIdx" tcEOS
#if defined(useChryslerMAPCorrection)
	"CRFICsettingMenuIdx" tcEOS
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	"acdSettingMenuIdx" tcEOS
#endif // defined(useVehicleParameters)
	"timeoutSettingMenuIdx" tcEOS
	"miscSettingMenuIdx" tcEOS

#if defined(useSavedTrips)
	"tripSaveCurrentMenuIdx" tcEOS
#endif // defined(useSavedTrips)
#if defined(useEnhancedTripReset)
	"tripSaveTankMenuIdx" tcEOS
#endif // defined(useEnhancedTripReset)
#if defined(useDragRaceFunction)
	"accelTestMenuIdx" tcEOS
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	"coastdownTestMenuIdx" tcEOS
#endif // defined(useCoastDownCalculator)
#endif // defined(useButtonInput)
};

#endif // defined(useDebugTerminalLabels)
// 0byyy0 0000 - MPGuino parameter action needed that is critical to measurements of fuel injection, VSS, timeouts, and the like
// 0b0000 0xxx - fractional part of parameter size in bits

static const uint8_t pfDoNothing =					0;
static const uint8_t pfSoftwareInitMPGuino =		pfDoNothing + 8;
static const uint8_t pfHardwareInitMPGuino =		pfSoftwareInitMPGuino + 8;
static const uint8_t pfDoMetricModeConversion =		pfHardwareInitMPGuino + 8;
static const uint8_t pfChangeDisplay =				pfDoMetricModeConversion + 8;
static const uint8_t pfCalculateFuelParams =		pfChangeDisplay + 8;
#define nextAllowedValue pfCalculateFuelParams + 8
#if defined(useBarFuelEconVsSpeed)
static const uint8_t pfHWresetAndBFEvSreset =		nextAllowedValue;
static const uint8_t pfSWresetAndBFEvSreset =		pfHWresetAndBFEvSreset + 8;
#define nextAllowedValue pfSWresetAndBFEvSreset + 8
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useChryslerMAPCorrection)
static const uint8_t pfHWresetAndFuelParamCalc =	nextAllowedValue;
#define nextAllowedValue pfHWresetAndFuelParamCalc + 8
#endif // defined(useChryslerMAPCorrection)

static const uint8_t pfBitmask =					0xF8;

static const uint8_t paramsLength[(uint16_t)(eePtrStorageEnd)] PROGMEM = {
	(pSizeSignature & 0x07) | pfHardwareInitMPGuino,							// pSignatureIdx
	(pSizeMetricMode & 0x07) | pfDoMetricModeConversion,						// pMetricModeIdx
	(pSizeAlternateFE & 0x07) | pfChangeDisplay,								// pAlternateFEidx
	(pSizeMicroSecondsPerGallon & 0x07) | pfSoftwareInitMPGuino,				// pMicroSecondsPerGallonIdx
	(pSizeInjEdgeTrigger & 0x07) | pfHardwareInitMPGuino,						// pInjEdgeTriggerIdx
	(pSizeInjectorSettleTime & 0x07) | pfHardwareInitMPGuino,					// pInjectorSettleTimeIdx
	(pSizeInjPer2CrankRev & 0x07) | pfHardwareInitMPGuino,						// pInjPer2CrankRevIdx
	(pSizeMinGoodRPM & 0x07) | pfHardwareInitMPGuino,							// pMinGoodRPMidx
#if defined(useBarFuelEconVsSpeed)
	(pSizePulseEdgePerDistance & 0x07) | pfHWresetAndBFEvSreset,				// pPulseEdgePerDistanceIdx
#else // defined(useBarFuelEconVsSpeed)
	(pSizePulseEdgePerDistance & 0x07) | pfHardwareInitMPGuino,					// pPulseEdgePerDistanceIdx
#endif // defined(useBarFuelEconVsSpeed)
	(pSizeVSSpause & 0x07) | pfHardwareInitMPGuino,								// pVSSpauseIdx
	(pSizeMinGoodSpeed & 0x07) | pfHardwareInitMPGuino,							// pMinGoodSpeedidx
	(pSizeTankSize & 0x07) | pfSoftwareInitMPGuino,								// pTankSizeIdx
	(pSizeTankBingoSize & 0x07) | pfSoftwareInitMPGuino,						// pTankBingoSizeIdx
	(pSizeIdleTimeout & 0x07) | pfHardwareInitMPGuino,							// pIdleTimeoutIdx
	(pSizeEOCtimeout & 0x07) | pfHardwareInitMPGuino,							// pEOCtimeoutIdx
	(pSizeButtonTimeout & 0x07) | pfHardwareInitMPGuino,						// pButtonTimeoutIdx
	(pSizeParkTimeout & 0x07) | pfHardwareInitMPGuino,							// pParkTimeoutIdx
	(pSizeActivityTimeout & 0x07) | pfHardwareInitMPGuino,						// pActivityTimeoutIdx
	(pSizeScratchpad & 0x07),													// pScratchpadIdx
#if defined(useButtonInput)
	(pSizeWakeupResetCurrentOnEngine & 0x07),									// pWakeupResetCurrentOnEngineIdx
	(pSizeWakeupResetCurrentOnMove & 0x07),										// pWakeupResetCurrentOnMoveIdx
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
	(pSizeBrightness & 0x07) | pfChangeDisplay,									// pBrightnessIdx
#if defined(useLCDcontrast)
	(pSizeContrast & 0x07) | pfChangeDisplay,									// pContrastIdx
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
	(pSizeLCDcolor & 0x07) | pfChangeDisplay,									// pLCDcolorIdx
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	(pSizeFuelUnitCost & 0x07),													// Price per unit volume of fuel
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	(pSizeOutputPin1Mode & 0x07),												// Output Pin 1 mode
	(pSizeOutputPin2Mode & 0x07),												// Output Pin 2 mode
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
	(pSizeVoltageOffset & 0x07),												// diode offset from V(alternator)
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
	(pSizeSerialDataLogging & 0x07),											// Serial Data Logging Enable
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	(pSizeJSONoutput & 0x07) | pfSoftwareInitMPGuino,							// JSON output Enable
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	(pSizeBluetoothOutput & 0x07),												// Bluetooth output Enable
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
	(pSizeFEvsTime & 0x07) | pfHardwareInitMPGuino,								// Period Of FE over Time BarGraph Bar (s)
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	(pSizeBarLowSpeedCutoff & 0x07) | pfSWresetAndBFEvSreset,					// FE vs Speed Bargraph lower speed
	(pSizeBarSpeedQuantumIdx & 0x07) | pfSWresetAndBFEvSreset,					// FE vs Speed Bargraph speed bar size
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
#if defined(useChryslerMAPCorrection)
	(pSizeSysFuelPressure & 0x07) | pfHWresetAndFuelParamCalc,					// pSysFuelPressureIdx
#else // defined(useChryslerMAPCorrection)
	(pSizeSysFuelPressure & 0x07) | pfCalculateFuelParams,						// pSysFuelPressureIdx
#endif // defined(useChryslerMAPCorrection)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	(pSizeRefFuelPressure & 0x07) | pfCalculateFuelParams,						// pRefFuelPressureIdx
	(pSizeInjectorCount & 0x07) | pfCalculateFuelParams,						// pInjectorCountIdx
	(pSizeInjectorSize & 0x07) | pfCalculateFuelParams,							// pInjectorSizeIdx
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	(pSizeMAPsensorFloor & 0x07) | pfHardwareInitMPGuino,						// pMAPsensorFloorIdx
	(pSizeMAPsensorCeiling & 0x07) | pfHardwareInitMPGuino,						// pMAPsensorCeilingIdx
	(pSizeMAPsensorRange & 0x07) | pfHardwareInitMPGuino,						// pMAPsensorRangeIdx
	(pSizeMAPsensorOffset & 0x07),												// pMAPsensorOffsetIdx
#if defined(useChryslerBaroSensor)
	(pSizeBaroSensorFloor & 0x07) | pfHardwareInitMPGuino,						// pBaroSensorFloorIdx
	(pSizeBaroSensorCeiling & 0x07) | pfHardwareInitMPGuino,					// pBaroSensorCeilingIdx
	(pSizeBaroSensorRange & 0x07) | pfHardwareInitMPGuino,						// pBaroSensorRangeIdx
	(pSizeBaroSensorOffset & 0x07),												// pBaroSensorOffsetIdx
#else // defined(useChryslerBaroSensor)
	(pSizeBarometricPressure & 0x07) | pfHardwareInitMPGuino,					// pBarometricPressureIdx
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	(pSizeVehicleMass & 0x07),													// pVehicleMassIdx
#if defined(useCoastDownCalculator)
	(pSizeVehicleFrontalArea & 0x07),											// pVehicleFrontalAreaIdx
	(pSizeLocustDensity & 0x07),												// pLocustDensityIdx
	(pSizeCoefficientD & 0x07),													// pCoefficientDidx
	(pSizeCoefficientV & 0x07),													// pCoefficientVidx
	(pSizeCoefficientRR & 0x07),												// pCoefficientRRidx
	(pSizeCoastdownSamplePeriod & 0x07) | pfHardwareInitMPGuino,				// pCoastdownSamplePeriodIdx
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	(pSizeDragSpeed & 0x07) | pfHardwareInitMPGuino,							// pDragSpeedIdx
	(pSizeDragDistance & 0x07) | pfHardwareInitMPGuino,							// pDragDistanceIdx
	(pSizeDragAutoFlag & 0x07),													// pDragAutoFlagIdx
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	(pSizeAutoSaveActive & 0x07),												// pAutoSaveActiveIdx
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	(pSizeRefuelSize & 0x07),													// pRefuelSizeIdx
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
	(pSizePeakHoldSimPeriod & 0x07) | pfSoftwareInitMPGuino,					// pPeakHoldSimPeriodIdx
	(pSizePeakHoldSimActive & 0x07) | pfSoftwareInitMPGuino,					// pPeakHoldSimActiveIdx
#endif // defined(useSimulatedFIandVSS)

#if defined(useButtonInput)
	(pSizeDisplayIdx & 0x07) | pfChangeDisplay,									// pDisplayIdx
#if LCDcharHeight == 4
	(pSizeBottomDisplayIdx & 0x07) | pfChangeDisplay,							// pBottomDisplayIdx
	(pSizeBottomCursorIdx & 0x07) | pfChangeDisplay,							// pBottomCursorIdx
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
	(pSizeRefuelSaveSizeIdx & 0x07),											// pRefuelSaveSizeIdx
#endif // defined(usePartialRefuel)
	(pSizeCurrTripSignatureIdx & 0x07),											// pCurrTripSignatureIdx
	(pSizeTankTripSignatureIdx & 0x07),											// pTankTripSignatureIdx
	(pSizeCurrTripVSSpulseIdx & 0x07),											// pCurrTripVSSpulseIdx
	(pSizeCurrTripVSScycleIdx & 0x07),											// pCurrTripVSScycleIdx
	(pSizeCurrTripInjPulseIdx & 0x07),											// pCurrTripInjPulseIdx
	(pSizeCurrTripInjCycleIdx & 0x07),											// pCurrTripInjCycleIdx
	(pSizeCurrTripEngCycleIdx & 0x07),											// pCurrTripEngCycleIdx
	(pSizeTankTripVSSpulseIdx & 0x07),											// pTankTripVSSpulseIdx
	(pSizeTankTripVSScycleIdx & 0x07),											// pTankTripVSScycleIdx
	(pSizeTankTripInjPulseIdx & 0x07),											// pTankTripInjPulseIdx
	(pSizeTankTripInjCycleIdx & 0x07),											// pTankTripInjCycleIdx
	(pSizeTankTripEngCycleIdx & 0x07),											// pTankTripEngCycleIdx
#if defined(trackIdleEOCdata)
	(pSizeCurrIEOCvssPulseIdx & 0x07),											// pCurrIEOCvssPulseIdx
	(pSizeCurrIEOCvssCycleIdx & 0x07),											// pCurrIEOCvssCycleIdx
	(pSizeCurrIEOCinjPulseIdx & 0x07),											// pCurrIEOCinjPulseIdx
	(pSizeCurrIEOCinjCycleIdx & 0x07),											// pCurrIEOCinjCycleIdx
	(pSizeCurrIEOCengCycleIdx & 0x07),											// pCurrIEOCengCycleIdx
	(pSizeTankIEOCvssPulseIdx & 0x07),											// pTankIEOCvssPulseIdx
	(pSizeTankIEOCvssCycleIdx & 0x07),											// pTankIEOCvssCycleIdx
	(pSizeTankIEOCinjPulseIdx & 0x07),											// pTankIEOCinjPulseIdx
	(pSizeTankIEOCinjCycleIdx & 0x07),											// pTankIEOCinjCycleIdx
	(pSizeTankIEOCengCycleIdx & 0x07),											// pTankIEOCengCycleIdx
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

/* parameter EEPROM addresses for parameter definitions above */

static const uint16_t paramAddrs[(uint16_t)(eePtrStorageEnd)] PROGMEM = {
	pAddressSignature,					// pSignatureIdx
	pAddressMetricMode,					// pMetricModeIdx
	pAddressAlternateFE,				// pAlternateFEidx
	pAddressMicroSecondsPerGallon,		// pMicroSecondsPerGallonIdx
	pAddressInjEdgeTrigger,				// pInjEdgeTriggerIdx
	pAddressInjectorSettleTime,			// pInjectorSettleTimeIdx
	pAddressInjPer2CrankRev,			// pInjPer2CrankRevIdx
	pAddressMinGoodRPM,					// pMinGoodRPMidx
	pAddressPulseEdgePerDistance,		// pPulseEdgePerDistanceIdx
	pAddressVSSpause,					// pVSSpauseIdx
	pAddressMinGoodSpeed,				// pMinGoodSpeedidx
	pAddressTankSize,					// pTankSizeIdx
	pAddressTankBingoSize,				// pTankBingoSizeIdx
	pAddressIdleTimeout,				// pIdleTimeoutIdx
	pAddressEOCtimeout,					// pEOCtimeoutIdx
	pAddressButtonTimeout,				// pButtonTimeoutIdx
	pAddressParkTimeout,				// pParkTimeoutIdx
	pAddressActivityTimeout,			// pActivityTimeoutIdx
	pAddressScratchpad,					// pScratchpadIdx
#if defined(useButtonInput)
	pAddressWakeupResetCurrentOnEngine,	// pWakeupResetCurrentOnEngineIdx
	pAddressWakeupResetCurrentOnMove,	// pWakeupResetCurrentOnMoveIdx
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
	pAddressBrightness,					// pBrightnessIdx
#if defined(useLCDcontrast)
	pAddressContrast,					// pContrastIdx
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
	pAddressLCDcolor,					// pLCDcolorIdx
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	pAddressFuelUnitCost,				// Price per unit volume of fuel
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	pAddressOutputPin1Mode,				// Output Pin 1 mode
	pAddressOutputPin2Mode,				// Output Pin 2 mode
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
	pAddressVoltageOffset,				// diode offset from V(alternator)
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
	pAddressSerialDataLogging,			// Serial Data Logging Enable
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	pAddressJSONoutput,					// JSON output Enable
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	pAddressBluetoothOutput,			// Bluetooth output Enable
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
	pAddressFEvsTime,					// Period Of FE over Time Bar Graph Bar (s)
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	pAddressBarLowSpeedCutoff,			// FE vs Speed Bargraph lower speed
	pAddressBarSpeedQuantumIdx,			// FE vs Speed Bargraph speed bar size
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
	pAddressSysFuelPressure,			// pSysFuelPressureIdx
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	pAddressRefFuelPressure,			// pRefFuelPressureIdx
	pAddressInjectorCount,				// pInjectorCountIdx
	pAddressInjectorSize,				// pInjectorSizeIdx
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	pAddressMAPsensorFloor,				// pMAPsensorFloorIdx
	pAddressMAPsensorCeiling,			// pMAPsensorCeilingIdx
	pAddressMAPsensorRange,				// pMAPsensorRangeIdx
	pAddressMAPsensorOffset,			// pMAPsensorOffsetIdx
#if defined(useChryslerBaroSensor)
	pAddressBaroSensorFloor,		 	// pBaroSensorFloorIdx
	pAddressBaroSensorCeiling,			// pBaroSensorCeilingIdx
	pAddressBaroSensorRange,			// pBaroSensorRangeIdx
	pAddressBaroSensorOffset,			// pBaroSensorOffsetIdx
#else // defined(useChryslerBaroSensor)
	pAddressBarometricPressure,			// pBarometricPressureIdx
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	pAddressVehicleMass,				// pVehicleMassIdx
#if defined(useCoastDownCalculator)
	pAddressVehicleFrontalArea,			// pVehicleFrontalAreaIdx
	pAddressLocustDensity,				// pLocustDensityIdx
	pAddressCoefficientD,				// pCoefficientDidx
	pAddressCoefficientV,				// pCoefficientVidx
	pAddressCoefficientRR,				// pCoefficientRRidx
	pAddressCoastdownSamplePeriod,		// pCoastdownSamplePeriodIdx
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	pAddressDragSpeed,					// pDragSpeedIdx
	pAddressDragDistance,				// pDragDistanceIdx
	pAddressDragAutoFlag,				// pDragAutoFlagIdx
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	pAddressAutoSaveActive,				// pAutoSaveActiveIdx
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	pAddressRefuelSize,					// pRefuelSizeIdx
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
	pAddressPeakHoldSimPeriod,			// pPeakHoldSimPeriodIdx
	pAddressPeakHoldSimActive,			// pPeakHoldSimActiveIdx
#endif // defined(useSimulatedFIandVSS)

#if defined(useButtonInput)
	pAddressDisplayIdx,					// pDisplayIdx
#if LCDcharHeight == 4
	pAddressBottomDisplayIdx,			// pBottomDisplayIdx
	pAddressBottomCursorIdx,			// pBottomCursorIdx
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
	pAddressRefuelSaveSizeIdx,			// pRefuelSaveSizeIdx
#endif // defined(usePartialRefuel)
	pAddressCurrTripSignatureIdx,		// pCurrTripSignatureIdx
	pAddressTankTripSignatureIdx,		// pTankTripSignatureIdx
	pAddressCurrTripVSSpulseIdx,		// pCurrTripVSSpulseIdx
	pAddressCurrTripVSScycleIdx,		// pCurrTripVSScycleIdx
	pAddressCurrTripInjPulseIdx,		// pCurrTripInjPulseIdx
	pAddressCurrTripInjCycleIdx,		// pCurrTripInjCycleIdx
	pAddressCurrTripEngCycleIdx,		// pCurrTripEngCycleIdx
	pAddressTankTripVSSpulseIdx,		// pTankTripVSSpulseIdx
	pAddressTankTripVSScycleIdx,		// pTankTripVSScycleIdx
	pAddressTankTripInjPulseIdx,		// pTankTripInjPulseIdx
	pAddressTankTripInjCycleIdx,		// pTankTripInjCycleIdx
	pAddressTankTripEngCycleIdx,		// pTankTripEngCycleIdx
#if defined(trackIdleEOCdata)
	pAddressCurrIEOCvssPulseIdx,		// pCurrIEOCvssPulseIdx
	pAddressCurrIEOCvssCycleIdx,		// pCurrIEOCvssCycleIdx
	pAddressCurrIEOCinjPulseIdx,		// pCurrIEOCinjPulseIdx
	pAddressCurrIEOCinjCycleIdx,		// pCurrIEOCinjCycleIdx
	pAddressCurrIEOCengCycleIdx,		// pCurrIEOCengCycleIdx
	pAddressTankIEOCvssPulseIdx,		// pTankIEOCvssPulseIdx
	pAddressTankIEOCvssCycleIdx,		// pTankIEOCvssCycleIdx
	pAddressTankIEOCinjPulseIdx,		// pTankIEOCinjPulseIdx
	pAddressTankIEOCinjCycleIdx,		// pTankIEOCinjCycleIdx
	pAddressTankIEOCengCycleIdx,		// pTankIEOCengCycleIdx
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

/* parameter values for parameter definitions above */

static const uint32_t newEEPROMsignature = ((uint32_t)(guinosig) << 24) + ((uint32_t)(eePtrEnd) << 16) + (uint32_t)(eeAdrSettingsEnd);

static const uint32_t params[(uint16_t)(pSettingsIdxLen)] PROGMEM = {
	newEEPROMsignature,	// pSignatureIdx					EEPROM MPGuino signature long word
	0,					// pMetricModeIdx					Display Mode (0 - US Display, 1 - Metric Display)
	0,					// pAlternateFEidx					0 - MPG or L/100km, 1 - G/100mile or km/L
	133262651,			// pMicroSecondsPerGallonIdx		Microseconds per US gallon
	0,					// pInjEdgeTriggerIdx				Fuel Injector Edge Trigger (0 - Sat Fall, 1 - Sat Rise, 2 - P/H Fall, 3- P/H Rise)
	550,				// pInjectorSettleTimeIdx			Fuel Injector Response Delay Time (us)
	1,					// pInjPer2CrankRevIdx				Fuel Injector Event Count for every 2 Crankshaft Revolutions
	40,					// pMinGoodRPMidx					Minimum Engine Speed For Engine On (RPM)
	10000,				// pPulseEdgePerDistanceIdx			VSS Pulse Edges (per mile or per km)
	0,					// pVSSpauseIdx						VSS Pause Debounce Count (ms)
	300,				// pMinGoodSpeedidx					Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	18000,				// pTankSizeIdx						Tank Capacity * 1000 (gal or L)
	2200,				// pTankBingoSizeIdx				Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
	15,					// pIdleTimeoutIdx					Engine Idle Timeout (s)
	2,					// pEOCtimeoutIdx					Engine-Off Coasting Timeout (s)
	5,					// pButtonTimeoutIdx				Button Press Activity Timeout (s)
	5,					// pParkTimeoutIdx					Vehicle Parked (engine off, no movement) Timeout (s)
	120,				// pActivityTimeoutIdx				Activity (engine off, no movement, no button press) Timeout (s)
	0,					// pScratchpadIdx					Scratchpad Memory

#if defined(useButtonInput)
	1,					// pWakeupResetCurrentOnEngineIdx	Enable current trip reset upon wakeup due to engine running
	0,					// pWakeupResetCurrentOnMoveIdx		Enable current trip reset upon wakeup due to button press
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
#if defined(useBinaryLCDbrightness)
	1,					// pBrightnessIdx					LCD Brightness
#else // defined(useBinaryLCDbrightness)
	2,					// pBrightnessIdx					LCD Brightness
#endif // defined(useBinaryLCDbrightness)
#if defined(useLCDcontrast)
	55,					// pContrastIdx						LCD Contrast
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDdisplay)
	3,					// pLCDcolorIdx						LCD Backlight color
#endif // defined(useAdafruitRGBLCDdisplay)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	2710259,			// Price per unit volume of fuel (this price is for a liter of diesel at 717 HUF / liter)
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	0,					// Output pin 1 mode
	0,					// Output pin 2 mode
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
	700,				// diode offset from V(alternator) (via meelis11)
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
	1,					// Serial Data Logging Enable
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	1,					// JSON output Enable
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	1,					// Bluetooth output Enable
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
	60,					// Length Of BarGraph Bar (s)
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	25000,				// FE vs Speed Bargraph lower speed
	5000,				// FE vs Speed Bargraph speed bar size
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
	43500,				// pSysFuelPressureIdx				Fuel System Pressure (Pa or * 1000 psig)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	43500,				// pRefFuelPressureIdx				Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	6,					// pInjectorCountIdx				Fuel Injector Count
	246000,				// pInjectorSizeIdx					Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	500,				// pMAPsensorFloorIdx				MAP Sensor Floor * 1000 (V)
	4000,				// pMAPsensorCeilingIdx				MAP Sensor Ceiling * 1000 (V)
	14145,				// pMAPsensorRangeIdx				Barometric Sensor Range (Pa or * 1000 psig)
	551,				// pMAPsensorOffsetIdx				MAP Sensor Offset (Pa or * 1000 psia)
#if defined(useChryslerBaroSensor)
	500,				// pBaroSensorFloorIdx				Barometric Sensor Floor * 1000 (V)
	4000,				// pBaroSensorCeilingIdx			Barometric Sensor Ceiling * 1000 (V)
	14145,				// pBaroSensorRangeIdx				Barometric Sensor Range (Pa or * 1000 psig)
	551,				// pBaroSensorOffsetIdx				Barometric Sensor Offset (Pa or * 1000 psia)
#else // defined(useChryslerBaroSensor)
	14696,				// pBarometricPressureIdx			Reference Barometric Pressure
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	4000,				// pVehicleMassIdx					Vehicle Weight/Mass (lbs or kg)
#if defined(useCoastDownCalculator)
	25400,				// pVehicleFrontalAreaIdx			Vehicle Frontal Area (ft^2 or m^2)
	2065,				// pLocustDensityIdx				Air density (lb/yd^3 or kg/m^3)
	346,				// pCoefficientDidx					C(d)
	1,					// pCoefficientVidx					C(v)
	8,					// pCoefficientRRidx				C(rr)
	3,					// pCoastdownSamplePeriodIdx		Sample Period in seconds
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	60000,				// pDragSpeedIdx					Drag speed cutoff (MPH or kph) * 1000
	250,				// pDragDistanceIdx					Drag distance cutoff (miles or km) * 1000
	0,					// pDragAutoFlagIdx					Drag automatic retrigger on vehicle stop
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	1,					// pAutoSaveActiveIdx				Autosave Active Trip Data Enable
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	0,					// pRefuelSizeIdx					Partial Refuel amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
	1000,				// pPeakHoldSimPeriodIdx			Simulated Peak/Hold Injector initial period in us
	600,				// pPeakHoldSimActiveIdx			Simulated Peak/Hold Injector initial on-time in us
#endif // defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
	mainDisplayIdx,		// pDisplayIdx
#if LCDcharHeight == 4
	mainDisplayIdx,		// pBottomDisplayIdx
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
};

// end of remarkably long EEPROM stored settings section
