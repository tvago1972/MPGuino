namespace EEPROM /* EEPROM parameter I/O section prototype */
{

	static uint8_t powerUpCheck(void);
	static void initGuinoHardware(void);
	static void initGuinoSoftware(void);
	static void setMetricDisplayMode(void);
	static uint8_t onChange(s64prgm_ptr_t sched, uint8_t parameterIdx);
	static uint8_t readByte(uint8_t eePtr);
#if defined(useScreenEditor)
	static uint16_t readWord(uint8_t eePtr);
#endif // defined(useScreenEditor)
	static uint32_t readVal(uint8_t eePtr);
	static void writeByte(uint8_t eePtr, uint8_t value);
	static void writeVal(uint8_t eePtr, uint32_t value);
	static uint32_t getDefault(uint8_t eePtr);
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

enum {
	baseMenuDisplayIdx = 0,
#if defined(useExpandedMainDisplay)
	mainMenuDisplayIdx,
	displayMainMenuEndIdx,
#else // defined(useExpandedMainDisplay)
	displayMainMenuEndIdx = baseMenuDisplayIdx + 1,
#endif // defined(useExpandedMainDisplay)
	settingsMenuDisplayIdx = displayMainMenuEndIdx,
	displaySettingsStartIdx,

// the following display index defines are for the various EEPROM parameter settings, grouped by function

	displaySettingsDisplayIdx = displaySettingsStartIdx,
	fuelSettingsDisplayIdx,
	VSSsettingsDisplayIdx,
	tankSettingsDisplayIdx,
	displaySettingsBaseEndIdx,
#if defined(useChryslerMAPCorrection)
	CRFICsettingsDisplayIdx = displaySettingsBaseEndIdx,
	displaySettingsCRFICEndIdx,
#else // defined(useChryslerMAPCorrection)
	displaySettingsCRFICEndIdx = displaySettingsBaseEndIdx,
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	acdSettingsDisplayIdx = displaySettingsCRFICEndIdx,
	displaySettingsVehicleEndIdx,
#else // defined(useVehicleParameters)
	displaySettingsVehicleEndIdx = displaySettingsCRFICEndIdx,
#endif // defined(useVehicleParameters)
	timeoutSettingsDisplayIdx = displaySettingsVehicleEndIdx,
	miscSettingsDisplayIdx,
	displaySettingsEndIdx,

	displayCountSettings = displaySettingsEndIdx - displaySettingsDisplayIdx,

// the following display index defines are for any otherwise unlinked menu groups

#if defined(useSavedTrips)
	tripSaveCurrentDisplayIdx = displaySettingsEndIdx,
	displayTripSaveCurrentEndIdx,
#else // defined(useSavedTrips)
	displayTripSaveCurrentEndIdx = displaySettingsEndIdx,
#endif // defined(useSavedTrips)
#if defined(useEnhancedTripReset)
	tripSaveTankDisplayIdx = displayTripSaveCurrentEndIdx,
	displayTripSaveTankEndIdx,
#else // defined(useEnhancedTripReset)
	displayTripSaveTankEndIdx = displayTripSaveCurrentEndIdx,
#endif // defined(useEnhancedTripReset)

// the following display index defines are for options selected via configs.h

	optionalDisplayIdxStart = displayTripSaveTankEndIdx,

#if defined(useDragRaceFunction)
	dragRaceMenuDisplayIdx = optionalDisplayIdxStart,
	displayDragRaceMenuEndIdx,
#else // defined(useDragRaceFunction)
	displayDragRaceMenuEndIdx = optionalDisplayIdxStart,
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	coastdownMenuDisplayIdx = displayDragRaceMenuEndIdx,
	displayCoastdownMenuEndIdx,
#else // defined(useCoastDownCalculator)
	displayCoastdownMenuEndIdx = displayDragRaceMenuEndIdx,
#endif // defined(useCoastDownCalculator)

	displayCountMenu = displayCoastdownMenuEndIdx - baseMenuDisplayIdx,

#if defined(useSimulatedFIandVSS)
	signalSimDisplayIdx = displayCoastdownMenuEndIdx,
	displaySignalSimEndIdx,
#else // defined(useSimulatedFIandVSS)
	displaySignalSimEndIdx = displayCoastdownMenuEndIdx,
#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
	pressureDisplayIdx = displaySignalSimEndIdx,
	displayPressureEndIdx,
#else // defined(useChryslerMAPCorrection)
	displayPressureEndIdx = displaySignalSimEndIdx,
#endif // defined(useChryslerMAPCorrection)
#if defined(useDebugAnalog)
	analogDisplayIdx = displayPressureEndIdx,
	displayAnalogEndIdx,
#else // defined(useDebugAnalog)
	displayAnalogEndIdx = displayPressureEndIdx,
#endif // defined(useDebugAnalog)

	displayCountBase = displayAnalogEndIdx + 2 - optionalDisplayIdxStart,

// the following display index defines are for the various main displays

	mainDisplayIdx = displayAnalogEndIdx,
#if defined(useStatusMeter)
	statusMeterDisplayIdx,
	displayStatusMeterEndIdx,
#else // defined(useStatusMeter)
	displayStatusMeterEndIdx = mainDisplayIdx + 1,
#endif // defined(useStatusMeter)
#if defined(useBigFE)
	bigFEdisplayIdx = displayStatusMeterEndIdx,
	displayBigFEEndIdx,
#else // defined(useBigFE)
	displayBigFEEndIdx = displayStatusMeterEndIdx,
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
	barFEvTdisplayIdx = displayBigFEEndIdx,
	displayBarFEvTEndIdx,
#else // defined(useBarFuelEconVsTime)
	displayBarFEvTEndIdx = displayBigFEEndIdx,
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	barFEvSdisplayIdx = displayBarFEvTEndIdx,
	displayBarFEvSEndIdx,
#else // defined(useBarFuelEconVsSpeed)
	displayBarFEvSEndIdx = displayBarFEvTEndIdx,
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
	bigDTEdisplayIdx = displayBarFEvSEndIdx,
	displayBigDTEEndIdx,
#else // defined(useBigDTE)
	displayBigDTEEndIdx = displayBarFEvSEndIdx,
#endif // defined(useBigDTE)
#if defined(useBigTTE)
	bigTTEdisplayIdx = displayBigDTEEndIdx,
	displayBigTTEEndIdx,
#else // defined(useBigTTE)
	displayBigTTEEndIdx = displayBigDTEEndIdx,
#endif // defined(useBigTTE)
#if defined(useCPUreading)
	CPUmonDisplayIdx = displayBigTTEEndIdx,
	displayCPUmonEndIdx,
#else // defined(useCPUreading)
	displayCPUmonEndIdx = displayBigTTEEndIdx,
#endif // defined(useCPUreading)
#if defined(useClockDisplay)
	clockShowDisplayIdx = displayCPUmonEndIdx,
	displayClockShowEndIdx,
#else // defined(useClockDisplay)
	displayClockShowEndIdx = displayCPUmonEndIdx,
#endif // defined(useClockDisplay)

	displayCountUser = displayClockShowEndIdx - mainDisplayIdx,	// this variable is used to figure out how many menu levels the user display section has

// the following display index defines are supplemental non-menu display index defines for options selected via configs.h

#if defined(useDragRaceFunction)
	dragRaceDisplayIdx = displayClockShowEndIdx,
	displayDragRaceEndIdx,
#else // defined(useDragRaceFunction)
	displayDragRaceEndIdx = displayClockShowEndIdx,
#endif // defined(useDragRaceFunction)
#if defined(useCoastDownCalculator)
	coastdownDisplayIdx = displayDragRaceEndIdx,
	displayCoastdownEndIdx,
#else // defined(useCoastDownCalculator)
	displayCoastdownEndIdx = displayDragRaceEndIdx,
#endif // defined(useCoastDownCalculator)

	displayMaxSavableIdx = displayCoastdownEndIdx,

	parameterEditDisplayIdx = displayMaxSavableIdx,
	displayParameterEditEndIdx,
#if defined(useClockDisplay)
	clockSetDisplayIdx = displayParameterEditEndIdx,
	displayClockSetEndIdx,
#else // defined(useClockDisplay)
	displayClockSetEndIdx = displayParameterEditEndIdx,
#endif // defined(useClockDisplay)
#if defined(useScreenEditor)
	displayEditDisplayIdx = displayClockSetEndIdx,
	displayEditEndIdx,
#else // defined(useScreenEditor)
	displayEditEndIdx = displayClockSetEndIdx,
#endif // defined(useScreenEditor)

	displayCountTotal = displayEditEndIdx
};

#endif // defined(useButtonInput)
static const uint8_t guinosig =		0b10110111;
static const uint8_t eepromLayoutVersion = 7;

// start of remarkably long EEPROM stored settings section

/* parameter indexes grouped by stored parameter size */

enum {

	pSettingsIdxStart = 0,

	pSettingsByteIdxStart =						pSettingsIdxStart,
	pEEPROMlayoutVersionIdx =					pSettingsByteIdxStart,

	pSettings1BitIdxStart,
	pMetricModeIdx =								pSettings1BitIdxStart,
	pAlternateFEidx,
#if defined(useButtonInput)
	pWakeupResetCurrentOnEngineIdx,
	pWakeupResetCurrentOnMoveIdx,
#endif // defined(useButtonInput)
#if defined(useLCDoutput) && defined(useBinaryLCDbrightness)
	pBrightnessIdx,
#endif // defined(useLCDoutput) && defined(useBinaryLCDbrightness)
#if defined(useLCDoutput) && defined(useLCDcontrast)
	pContrastIdx,
#endif // defined(useLCDoutput) && defined(useLCDcontrast)
#if defined(useDataLoggingOutput)
	pSerialDataLoggingIdx,
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	pJSONoutputIdx,
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	pBluetoothOutputIdx,
#endif // defined(useBluetooth)
#if defined(useVehicleParameters) && defined(useDragRaceFunction)
	pDragAutoFlagIdx,
#endif // defined(useVehicleParameters) && defined(useDragRaceFunction)
#if defined(useSavedTrips)
	pAutoSaveActiveIdx,
#endif // defined(useSavedTrips)
	pSettings1BitIdxEnd,

	pSettings2BitIdxStart =						pSettings1BitIdxEnd,
	pInjEdgeTriggerIdx =						pSettings2BitIdxStart,
#if defined(useLCDoutput) && !defined(useBinaryLCDbrightness)
	pBrightnessIdx,
#endif // defined(useLCDoutput) && !defined(useBinaryLCDbrightness)
	pSettings2BitIdxEnd,

	pSettings3BitIdxStart =						pSettings2BitIdxEnd,
#if defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
	pLCDcolorIdx =								pSettings3BitIdxStart,
#endif // defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
	pSettings3BitIdxEnd
#if defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
		= pLCDcolorIdx + 1
#else // defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
		= pSettings3BitIdxStart
#endif // defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
	,

	pSettings4BitIdxStart =						pSettings3BitIdxEnd,
#if defined(useOutputPins)
	pOutputPin1Mode =							pSettings4BitIdxStart,
	pOutputPin2Mode,
#endif // defined(useOutputPins)
	pSettings4BitIdxEnd
#if defined(useOutputPins)
		= pOutputPin2Mode + 1
#else // defined(useOutputPins)
		= pSettings4BitIdxStart
#endif // defined(useOutputPins)
	,

	pSettings8BitIdxStart =						pSettings4BitIdxEnd,
	pInjPer2CrankRevIdx =						pSettings8BitIdxStart,
	pVSSpauseIdx,
	pIdleTimeoutIdx,
	pEOCtimeoutIdx,
	pButtonTimeoutIdx,
	pParkTimeoutIdx,
	pActivityTimeoutIdx,
#ifdef useCalculatedFuelFactor
	pInjectorCountIdx,
#endif // useCalculatedFuelFactor
#if defined(useVehicleParameters) && defined(useCoastDownCalculator)
	pCoastdownSamplePeriodIdx,
#endif // defined(useVehicleParameters) && defined(useCoastDownCalculator)
#if defined(useButtonInput)
	pDisplayIdx,
#if LCDcharHeight == 4
	pBottomDisplayIdx,
	pBottomCursorIdx,
#endif // LCDcharHeight == 4
	eePtrDisplayCursorStart,
	eePtrDisplayCursorEnd =						eePtrDisplayCursorStart + displayCountTotal,
	eePtrMenuHeightStart =						eePtrDisplayCursorEnd,
	eePtrMenuHeightEnd =						eePtrMenuHeightStart + displayCountMenu,
#else // defined(useButtonInput)
	eePtrMenuHeightEnd =						pSettings8BitIdxStart,
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
	pCurrTripSignatureIdx =						eePtrMenuHeightEnd,
	pTankTripSignatureIdx,
	pSavedTrips8BitIdxEnd,
#else // defined(useEEPROMtripStorage)
	pSavedTrips8BitIdxEnd =						eePtrMenuHeightEnd,
#endif // defined(useEEPROMtripStorage)
	pSettings8BitIdxEnd =						pSavedTrips8BitIdxEnd,
	pSettingsByteIdxEnd =						pSettings8BitIdxEnd,

	pSettings12BitIdxStart =					pSettingsByteIdxEnd,
#if defined(useAlternatorVoltage)
	pVoltageOffset =							pSettings12BitIdxStart,
#endif // defined(useAlternatorVoltage)
	pSettings12BitIdxEnd
#if defined(useAlternatorVoltage)
		= pVoltageOffset + 1
#else // defined(useAlternatorVoltage)
		= pSettings12BitIdxStart
#endif // defined(useAlternatorVoltage)
	,

	pSettings16BitIdxStart =					pSettings12BitIdxEnd,
	pInjectorSettleTimeIdx =					pSettings16BitIdxStart,
	pMinGoodRPMidx,
	pMinGoodSpeedidx,
#if defined(useTouchScreenInput)
	pTouchRawXloIdx,
	pTouchRawXhiIdx,
	pTouchRawYloIdx,
	pTouchRawYhiIdx,
#endif // defined(useTouchScreenInput)
#if defined(useFEvTdata)
	pFEvsTimeIdx,
#endif // defined(useFEvTdata)
#if defined(useChryslerMAPCorrection)
	pMAPsensorFloorIdx,
	pMAPsensorCeilingIdx,
#if defined(useChryslerBaroSensor)
	pBaroSensorFloorIdx,
	pBaroSensorCeilingIdx,
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	pVehicleMassIdx,
#if defined(useCoastDownCalculator)
	pVehicleFrontalAreaIdx,
	pLocustDensityIdx,
	pCoefficientDidx,
	pCoefficientVidx,
	pCoefficientRRidx,
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	pDragDistanceIdx,
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSimulatedFIandVSS)
	pPeakHoldSimPeriodIdx,
	pPeakHoldSimActiveIdx,
#endif // defined(useSimulatedFIandVSS)
	pSettings16BitCoreEnd,
#if defined(useScreenEditor)
	eePtrDisplayPagesStart =					pSettings16BitCoreEnd,
	eePtrDisplayPagesEnd =						eePtrDisplayPagesStart + mainDisplayFormatSize,
#else // defined(useScreenEditor)
	eePtrDisplayPagesEnd =						pSettings16BitCoreEnd,
#endif // defined(useScreenEditor)
	pSettings16BitIdxEnd =						eePtrDisplayPagesEnd,

	pSettings18BitIdxStart =					pSettings16BitIdxEnd,
	pPulseEdgePerDistanceIdx =					pSettings18BitIdxStart,
	pSettings18BitIdxEnd,

	pSettings20BitIdxStart =					pSettings18BitIdxEnd,
#ifdef useCalculatedFuelFactor
	pInjectorSizeIdx =							pSettings20BitIdxStart,
#endif // useCalculatedFuelFactor
	pSettings20BitIdxEnd
#ifdef useCalculatedFuelFactor
		= pInjectorSizeIdx + 1
#else // useCalculatedFuelFactor
		= pSettings20BitIdxStart
#endif // useCalculatedFuelFactor
	,

	pSettings24BitIdxStart =					pSettings20BitIdxEnd,
	pTankSizeIdx =								pSettings24BitIdxStart,
	pTankBingoSizeIdx,
#if defined(useFuelCost)
	pCostPerQuantity,
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	pOutputPinMaxFuelEconomy,
#endif // defined(useOutputPins)
#if defined(useBarFuelEconVsSpeed)
	pBarLowSpeedCutoffIdx,
	pBarSpeedQuantumIdx,
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useVehicleParameters) && defined(useDragRaceFunction)
	pDragSpeedIdx,
#endif // defined(useVehicleParameters) && defined(useDragRaceFunction)
#if defined(usePartialRefuel)
	pRefuelSizeIdx,
#endif // defined(usePartialRefuel)
#if defined(useEEPROMtripStorage) && defined(usePartialRefuel)
	pRefuelSaveSizeIdx,
#endif // defined(useEEPROMtripStorage) && defined(usePartialRefuel)
	pSettings24BitIdxEnd,

	pSettings32BitIdxStart =					pSettings24BitIdxEnd,
	pSignatureIdx =								pSettings32BitIdxStart,
	pMicroSecondsPerGallonIdx,
	pScratchpadIdx,
#if defined(useFuelPressure)
	pSysFuelPressureIdx,
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	pRefFuelPressureIdx,
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	pMAPsensorRangeIdx,
	pMAPsensorOffsetIdx,
#if defined(useChryslerBaroSensor)
	pBaroSensorRangeIdx,
	pBaroSensorOffsetIdx,
#else // defined(useChryslerBaroSensor)
	pBarometricPressureIdx,
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
	pDefaultSettingsIdxEnd,
#if defined(useEEPROMtripStorage)
	pCurrTripVSSpulseIdx =						pDefaultSettingsIdxEnd,
	pCurrTripInjPulseIdx,
	pTankTripVSSpulseIdx,
	pTankTripInjPulseIdx,
#if defined(trackIdleEOCdata)
	pCurrIEOCvssPulseIdx,
	pCurrIEOCinjPulseIdx,
	pTankIEOCvssPulseIdx,
	pTankIEOCinjPulseIdx,
#endif // defined(trackIdleEOCdata)
	pSavedTrips32BitIdxEnd,
#else // defined(useEEPROMtripStorage)
	pSavedTrips32BitIdxEnd =					pDefaultSettingsIdxEnd,
#endif // defined(useEEPROMtripStorage)

	pSettings32BitIdxEnd =						pSavedTrips32BitIdxEnd,

	pSettings64BitIdxStart =					pSettings32BitIdxEnd,
#if defined(useEEPROMtripStorage)
	pSavedTrips64BitIdxStart =					pSettings64BitIdxStart,
	pCurrTripVSScycleIdx =						pSavedTrips64BitIdxStart,
	pCurrTripInjCycleIdx,
	pCurrTripEngCycleIdx,
	pTankTripVSScycleIdx,
	pTankTripInjCycleIdx,
	pTankTripEngCycleIdx,
#if defined(trackIdleEOCdata)
	pCurrIEOCvssCycleIdx,
	pCurrIEOCinjCycleIdx,
	pCurrIEOCengCycleIdx,
	pTankIEOCvssCycleIdx,
	pTankIEOCinjCycleIdx,
	pTankIEOCengCycleIdx,
#endif // defined(trackIdleEOCdata)
	pSavedTrips64BitIdxEnd,
#else // defined(useEEPROMtripStorage)
	pSavedTrips64BitIdxEnd =					pSettings64BitIdxStart,
#endif // defined(useEEPROMtripStorage)
	pSettings64BitIdxEnd =						pSavedTrips64BitIdxEnd,

	pSettingsIdxEnd =							pSettings64BitIdxEnd,
	pSettingsIdxLen =							pSettingsIdxEnd - pSettingsIdxStart,

	eePtrStorageEnd =							pSettingsIdxEnd,

	eePtrEnd =									pSettingsIdxEnd

};

/* EEPROM storage addresses derived from parameter index ranges */

static const uint16_t eeAdrSettingsByteStart = 0;
static const uint16_t eeAdrSettingsByteEnd = eeAdrSettingsByteStart + (pSettingsByteIdxEnd - pSettingsByteIdxStart);
static const uint16_t eeAdrSettings12BitStart = eeAdrSettingsByteEnd;
static const uint16_t eeAdrSettings12BitEnd = eeAdrSettings12BitStart + 2 * (pSettings12BitIdxEnd - pSettings12BitIdxStart);
static const uint16_t eeAdrSettings16BitStart = eeAdrSettings12BitEnd;
static const uint16_t eeAdrSettings16BitEnd = eeAdrSettings16BitStart + 2 * (pSettings16BitIdxEnd - pSettings16BitIdxStart);
static const uint16_t eeAdrSettings18BitStart = eeAdrSettings16BitEnd;
static const uint16_t eeAdrSettings18BitEnd = eeAdrSettings18BitStart + 3 * (pSettings18BitIdxEnd - pSettings18BitIdxStart);
static const uint16_t eeAdrSettings20BitStart = eeAdrSettings18BitEnd;
static const uint16_t eeAdrSettings20BitEnd = eeAdrSettings20BitStart + 3 * (pSettings20BitIdxEnd - pSettings20BitIdxStart);
static const uint16_t eeAdrSettings24BitStart = eeAdrSettings20BitEnd;
static const uint16_t eeAdrSettings24BitEnd = eeAdrSettings24BitStart + 3 * (pSettings24BitIdxEnd - pSettings24BitIdxStart);
static const uint16_t eeAdrSettings32BitStart = eeAdrSettings24BitEnd;
static const uint16_t eeAdrSettings32BitEnd = eeAdrSettings32BitStart + 4 * (pSettings32BitIdxEnd - pSettings32BitIdxStart);
static const uint16_t eeAdrSettings64BitStart = eeAdrSettings32BitEnd;
static const uint16_t eeAdrSettings64BitEnd = eeAdrSettings64BitStart + 8 * (pSettings64BitIdxEnd - pSettings64BitIdxStart);
static const uint16_t eeAdrSettingsEnd = eeAdrSettings64BitEnd;

static const uint16_t eeAdrStorageEnd =
	eeAdrSettingsEnd;


#if defined(useDebugTerminalLabels)
/* labels for parameters definitions above */

static const char terminalParameterNames[] PROGMEM = {
	"pEEPROMlayoutVersionIdx" tcEOS
	"pMetricModeIdx" tcEOS
	"pAlternateFEidx" tcEOS
#if defined(useButtonInput)
	"pWakeupResetCurrentOnEngineIdx" tcEOS
	"pWakeupResetCurrentOnMoveIdx" tcEOS
#endif // defined(useButtonInput)
#if defined(useLCDoutput) && defined(useBinaryLCDbrightness)
	"pBrightnessIdx" tcEOS
#endif // defined(useLCDoutput) && defined(useBinaryLCDbrightness)
#if defined(useLCDoutput) && defined(useLCDcontrast)
	"pContrastIdx" tcEOS
#endif // defined(useLCDoutput) && defined(useLCDcontrast)
#if defined(useDataLoggingOutput)
	"pSerialDataLoggingIdx" tcEOS
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	"pJSONoutputIdx" tcEOS
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	"pBluetoothOutputIdx" tcEOS
#endif // defined(useBluetooth)
#if defined(useVehicleParameters) && defined(useDragRaceFunction)
	"pDragAutoFlagIdx" tcEOS
#endif // defined(useVehicleParameters) && defined(useDragRaceFunction)
#if defined(useSavedTrips)
	"pAutoSaveActiveIdx" tcEOS
#endif // defined(useSavedTrips)
	"pInjEdgeTriggerIdx" tcEOS
#if defined(useLCDoutput) && !defined(useBinaryLCDbrightness)
	"pBrightnessIdx" tcEOS
#endif // defined(useLCDoutput) && !defined(useBinaryLCDbrightness)
#if defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
	"pLCDcolorIdx" tcEOS
#endif // defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
#if defined(useOutputPins)
	"pOutputPin1Mode" tcEOS
	"pOutputPin2Mode" tcEOS
#endif // defined(useOutputPins)
	"pInjPer2CrankRevIdx" tcEOS
	"pVSSpauseIdx" tcEOS
	"pIdleTimeoutIdx" tcEOS
	"pEOCtimeoutIdx" tcEOS
	"pButtonTimeoutIdx" tcEOS
	"pParkTimeoutIdx" tcEOS
	"pActivityTimeoutIdx" tcEOS
#ifdef useCalculatedFuelFactor
	"pInjectorCountIdx" tcEOS
#endif // useCalculatedFuelFactor
#if defined(useVehicleParameters) && defined(useCoastDownCalculator)
	"pCoastdownSamplePeriodIdx" tcEOS
#endif // defined(useVehicleParameters) && defined(useCoastDownCalculator)
#if defined(useButtonInput)
	"pDisplayIdx" tcEOS
#if LCDcharHeight == 4
	"pBottomDisplayIdx" tcEOS
	"pBottomCursorIdx" tcEOS
#endif // LCDcharHeight == 4
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
#if defined(useEEPROMtripStorage)
	"pCurrTripSignatureIdx" tcEOS
	"pTankTripSignatureIdx" tcEOS
#endif // defined(useEEPROMtripStorage)
#if defined(useAlternatorVoltage)
	"pVoltageOffset" tcEOS
#endif // defined(useAlternatorVoltage)
	"pInjectorSettleTimeIdx" tcEOS
	"pMinGoodRPMidx" tcEOS
	"pMinGoodSpeedidx" tcEOS
#if defined(useTouchScreenInput)
	"pTouchRawXloIdx" tcEOS
	"pTouchRawXhiIdx" tcEOS
	"pTouchRawYloIdx" tcEOS
	"pTouchRawYhiIdx" tcEOS
#endif // defined(useTouchScreenInput)
#if defined(useFEvTdata)
	"pFEvsTimeIdx" tcEOS
#endif // defined(useFEvTdata)
#if defined(useChryslerMAPCorrection)
	"pMAPsensorFloorIdx" tcEOS
	"pMAPsensorCeilingIdx" tcEOS
#if defined(useChryslerBaroSensor)
	"pBaroSensorFloorIdx" tcEOS
	"pBaroSensorCeilingIdx" tcEOS
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
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	"pDragDistanceIdx" tcEOS
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSimulatedFIandVSS)
	"pPeakHoldSimPeriodIdx" tcEOS
	"pPeakHoldSimActiveIdx" tcEOS
#endif // defined(useSimulatedFIandVSS)
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

	"pPulseEdgePerDistanceIdx" tcEOS
#ifdef useCalculatedFuelFactor
	"pInjectorSizeIdx" tcEOS
#endif // useCalculatedFuelFactor
	"pTankSizeIdx" tcEOS
	"pTankBingoSizeIdx" tcEOS
#if defined(useFuelCost)
	"pCostPerQuantity" tcEOS
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	"pOutputPinMaxFuelEconomy" tcEOS
#endif // defined(useOutputPins)
#if defined(useBarFuelEconVsSpeed)
	"pBarLowSpeedCutoffIdx" tcEOS
	"pBarSpeedQuantumIdx" tcEOS
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useVehicleParameters) && defined(useDragRaceFunction)
	"pDragSpeedIdx" tcEOS
#endif // defined(useVehicleParameters) && defined(useDragRaceFunction)
#if defined(usePartialRefuel)
	"pRefuelSizeIdx" tcEOS
#endif // defined(usePartialRefuel)
#if defined(useEEPROMtripStorage) && defined(usePartialRefuel)
	"pRefuelSaveSizeIdx" tcEOS
#endif // defined(useEEPROMtripStorage) && defined(usePartialRefuel)
	"pSignatureIdx" tcEOS
	"pMicroSecondsPerGallonIdx" tcEOS
	"pScratchpadIdx" tcEOS
#if defined(useFuelPressure)
	"pSysFuelPressureIdx" tcEOS
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	"pRefFuelPressureIdx" tcEOS
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	"pMAPsensorRangeIdx" tcEOS
	"pMAPsensorOffsetIdx" tcEOS
#if defined(useChryslerBaroSensor)
	"pBaroSensorRangeIdx" tcEOS
	"pBaroSensorOffsetIdx" tcEOS
#else // defined(useChryslerBaroSensor)
	"pBarometricPressureIdx" tcEOS
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useEEPROMtripStorage)
	"pCurrTripVSSpulseIdx" tcEOS
	"pCurrTripInjPulseIdx" tcEOS
	"pTankTripVSSpulseIdx" tcEOS
	"pTankTripInjPulseIdx" tcEOS
#if defined(trackIdleEOCdata)
	"pCurrIEOCvssPulseIdx" tcEOS
	"pCurrIEOCinjPulseIdx" tcEOS
	"pTankIEOCvssPulseIdx" tcEOS
	"pTankIEOCinjPulseIdx" tcEOS
#endif // defined(trackIdleEOCdata)
	"pCurrTripVSScycleIdx" tcEOS
	"pCurrTripInjCycleIdx" tcEOS
	"pCurrTripEngCycleIdx" tcEOS
	"pTankTripVSScycleIdx" tcEOS
	"pTankTripInjCycleIdx" tcEOS
	"pTankTripEngCycleIdx" tcEOS
#if defined(trackIdleEOCdata)
	"pCurrIEOCvssCycleIdx" tcEOS
	"pCurrIEOCinjCycleIdx" tcEOS
	"pCurrIEOCengCycleIdx" tcEOS
	"pTankIEOCvssCycleIdx" tcEOS
	"pTankIEOCinjCycleIdx" tcEOS
	"pTankIEOCengCycleIdx" tcEOS
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

#endif // defined(useDebugTerminalLabels)
/* parameter values for parameter definitions above */

static const uint32_t newEEPROMsignature = ((uint32_t)(guinosig) << 24) + ((uint32_t)(eepromLayoutVersion) << 16) + (uint32_t)(eeAdrSettingsEnd);

struct parameterDefault
{

	uint8_t parameterIdx;
	uint32_t value;

};

static const parameterDefault parameterDefaults[] PROGMEM = {

	{ pEEPROMlayoutVersionIdx, eepromLayoutVersion },	// pEEPROMlayoutVersionIdx		Hidden EEPROM storage layout version
	{ pSignatureIdx, newEEPROMsignature },	// pSignatureIdx					EEPROM MPGuino signature long word
	{ pMetricModeIdx, 0 },	// pMetricModeIdx					Display Mode (0 - US Display, 1 - Metric Display)
	{ pAlternateFEidx, 0 },	// pAlternateFEidx					0 - MPG or L/100km, 1 - G/100mile or km/L
	{ pMicroSecondsPerGallonIdx, 133262651 },	// pMicroSecondsPerGallonIdx		Microseconds per US gallon
	{ pInjEdgeTriggerIdx, 0 },	// pInjEdgeTriggerIdx				Fuel Injector Edge Trigger (0 - Sat Fall, 1 - Sat Rise, 2 - P/H Fall, 3- P/H Rise)
	{ pInjectorSettleTimeIdx, 550 },	// pInjectorSettleTimeIdx			Fuel Injector Response Delay Time (us)
	{ pInjPer2CrankRevIdx, 1 },	// pInjPer2CrankRevIdx				Fuel Injector Event Count for every 2 Crankshaft Revolutions
	{ pMinGoodRPMidx, 40 },	// pMinGoodRPMidx					Minimum Engine Speed For Engine On (RPM)
	{ pPulseEdgePerDistanceIdx, 10000 },	// pPulseEdgePerDistanceIdx			VSS Pulse Edges (per mile or per km)
	{ pVSSpauseIdx, 0 },	// pVSSpauseIdx						VSS Pause Debounce Count (ms)
	{ pMinGoodSpeedidx, 300 },	// pMinGoodSpeedidx					Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	{ pTankSizeIdx, 18000 },	// pTankSizeIdx						Tank Capacity * 1000 (gal or L)
	{ pTankBingoSizeIdx, 2200 },	// pTankBingoSizeIdx				Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
	{ pIdleTimeoutIdx, 15 },	// pIdleTimeoutIdx					Engine Idle Timeout (s)
	{ pEOCtimeoutIdx, 2 },	// pEOCtimeoutIdx					Engine-Off Coasting Timeout (s)
	{ pButtonTimeoutIdx, 5 },	// pButtonTimeoutIdx				Button Press Activity Timeout (s)
	{ pParkTimeoutIdx, 5 },	// pParkTimeoutIdx					Vehicle Parked (engine off, no movement) Timeout (s)
	{ pActivityTimeoutIdx, 120 },	// pActivityTimeoutIdx				Activity (engine off, no movement, no button press) Timeout (s)
	{ pScratchpadIdx, 0 },	// pScratchpadIdx					Scratchpad Memory
#if defined(useTouchScreenInput)
	{ pTouchRawXloIdx, 907 },	// pTouchRawXloIdx					Touch panel raw ADC X at left edge (orientation 3)
	{ pTouchRawXhiIdx, 3453 },	// pTouchRawXhiIdx					Touch panel raw ADC X at right edge (orientation 3)
	{ pTouchRawYloIdx, 620 },	// pTouchRawYloIdx					Touch panel raw ADC Y at top edge (orientation 3)
	{ pTouchRawYhiIdx, 3150 },	// pTouchRawYhiIdx					Touch panel raw ADC Y at bottom edge (orientation 3)
#endif // defined(useTouchScreenInput)
#if defined(useButtonInput)
	{ pWakeupResetCurrentOnEngineIdx, 1 },	// pWakeupResetCurrentOnEngineIdx	Enable current trip reset upon wakeup due to engine running
	{ pWakeupResetCurrentOnMoveIdx, 0 },	// pWakeupResetCurrentOnMoveIdx		Enable current trip reset upon wakeup due to button press
#endif // defined(useButtonInput)
#if defined(useLCDoutput) && defined(useBinaryLCDbrightness)
	{ pBrightnessIdx, 2 },	// pBrightnessIdx					LCD Brightness
#endif // defined(useLCDoutput) && defined(useBinaryLCDbrightness)
#if defined(useLCDoutput) && !defined(useBinaryLCDbrightness)
	{ pBrightnessIdx, 2 },	// pBrightnessIdx					LCD Brightness
#endif // defined(useLCDoutput) && !defined(useBinaryLCDbrightness)
#if defined(useLCDoutput) && defined(useLCDcontrast)
	{ pContrastIdx, 55 },	// pContrastIdx						LCD Contrast
#endif // defined(useLCDoutput) && defined(useLCDcontrast)
#if defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
	{ pLCDcolorIdx, 3 },	// pLCDcolorIdx						LCD Backlight color
#endif // defined(useLCDoutput) && defined(useAdafruitRGBLCDdisplay)
#if defined(useFuelCost)
	{ pCostPerQuantity, 2710259 },	// pCostPerQuantity					Price per unit volume of fuel (this price is for a liter of diesel at 717 HUF / liter)
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	{ pOutputPin1Mode, 0 },	// pOutputPin1Mode					Output pin 1 mode
	{ pOutputPin2Mode, 0 },	// pOutputPin2Mode					Output pin 2 mode
	{ pOutputPinMaxFuelEconomy, 50000 },	// pOutputPinMaxFuelEconomy		Output pin max fuel economy (50 MPG * 1000 SAE, or equivalent KPL * 1000 in metric)
#endif // defined(useOutputPins)
#if defined(useAlternatorVoltage)
	{ pVoltageOffset, 700 },	// pVoltageOffset					diode offset from V(alternator) (via meelis11)
#endif // defined(useAlternatorVoltage)
#if defined(useDataLoggingOutput)
	{ pSerialDataLoggingIdx, 1 },	// pSerialDataLoggingIdx			Serial Data Logging Enable
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	{ pJSONoutputIdx, 1 },	// pJSONoutputIdx					JSON output Enable
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	{ pBluetoothOutputIdx, 1 },	// pBluetoothOutputIdx				Bluetooth output Enable
#endif // defined(useBluetooth)
#if defined(useFEvTdata)
	{ pFEvsTimeIdx, 60 },	// pFEvsTimeIdx					Length Of BarGraph Bar (s)
#endif // defined(useFEvTdata)
#if defined(useBarFuelEconVsSpeed)
	{ pBarLowSpeedCutoffIdx, 25000 },	// pBarLowSpeedCutoffIdx			FE vs Speed Bargraph lower speed
	{ pBarSpeedQuantumIdx, 5000 },	// pBarSpeedQuantumIdx			FE vs Speed Bargraph speed bar size
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
	{ pSysFuelPressureIdx, 43500 },	// pSysFuelPressureIdx				Fuel System Pressure (Pa or * 1000 psig)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	{ pRefFuelPressureIdx, 43500 },	// pRefFuelPressureIdx				Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	{ pInjectorCountIdx, 6 },	// pInjectorCountIdx				Fuel Injector Count
	{ pInjectorSizeIdx, 246000 },	// pInjectorSizeIdx					Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	{ pMAPsensorFloorIdx, 500 },	// pMAPsensorFloorIdx				MAP Sensor Floor * 1000 (V)
	{ pMAPsensorCeilingIdx, 4000 },	// pMAPsensorCeilingIdx				MAP Sensor Ceiling * 1000 (V)
	{ pMAPsensorRangeIdx, 14145 },	// pMAPsensorRangeIdx				Barometric Sensor Range (Pa or * 1000 psig)
	{ pMAPsensorOffsetIdx, 551 },	// pMAPsensorOffsetIdx				MAP Sensor Offset (Pa or * 1000 psia)
#if defined(useChryslerBaroSensor)
	{ pBaroSensorFloorIdx, 500 },	// pBaroSensorFloorIdx				Barometric Sensor Floor * 1000 (V)
	{ pBaroSensorCeilingIdx, 4000 },	// pBaroSensorCeilingIdx			Barometric Sensor Ceiling * 1000 (V)
	{ pBaroSensorRangeIdx, 14145 },	// pBaroSensorRangeIdx				Barometric Sensor Range (Pa or * 1000 psig)
	{ pBaroSensorOffsetIdx, 551 },	// pBaroSensorOffsetIdx				Barometric Sensor Offset (Pa or * 1000 psia)
#else // defined(useChryslerBaroSensor)
	{ pBarometricPressureIdx, 14696 },	// pBarometricPressureIdx			Reference Barometric Pressure
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	{ pVehicleMassIdx, 4000 },	// pVehicleMassIdx					Vehicle Weight/Mass (lbs or kg)
#if defined(useCoastDownCalculator)
	{ pVehicleFrontalAreaIdx, 25400 },	// pVehicleFrontalAreaIdx			Vehicle Frontal Area (ft^2 or m^2)
	{ pLocustDensityIdx, 2065 },	// pLocustDensityIdx				Air density (lb/yd^3 or kg/m^3)
	{ pCoefficientDidx, 346 },	// pCoefficientDidx					C(d)
	{ pCoefficientVidx, 1 },	// pCoefficientVidx					C(v)
	{ pCoefficientRRidx, 8 },	// pCoefficientRRidx				C(rr)
	{ pCoastdownSamplePeriodIdx, 3 },	// pCoastdownSamplePeriodIdx		Sample Period in seconds
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	{ pDragSpeedIdx, 60000 },	// pDragSpeedIdx					Drag speed cutoff (MPH or kph) * 1000
	{ pDragDistanceIdx, 250 },	// pDragDistanceIdx					Drag distance cutoff (miles or km) * 1000
	{ pDragAutoFlagIdx, 0 },	// pDragAutoFlagIdx					Drag automatic retrigger on vehicle stop
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	{ pAutoSaveActiveIdx, 1 },	// pAutoSaveActiveIdx				Autosave Active Trip Data Enable
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	{ pRefuelSizeIdx, 0 },	// pRefuelSizeIdx					Partial Refuel amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)
#if defined(useSimulatedFIandVSS)
	{ pPeakHoldSimPeriodIdx, 1000 },	// pPeakHoldSimPeriodIdx			Simulated Peak/Hold Injector initial period in us
	{ pPeakHoldSimActiveIdx, 600 },	// pPeakHoldSimActiveIdx			Simulated Peak/Hold Injector initial on-time in us
#endif // defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
	{ pDisplayIdx, mainDisplayIdx },	// pDisplayIdx
#if LCDcharHeight == 4
	{ pBottomDisplayIdx, mainDisplayIdx },	// pBottomDisplayIdx
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
};

static const uint8_t parameterDefaultsCount = (sizeof(parameterDefaults) / sizeof(parameterDefaults[0]));
static const uint8_t parameterDefaultInitCheckCount = 2;
typedef char parameterDefaultsCountCheck[(parameterDefaultsCount <= pSettingsIdxLen) ? 1 : -1];

// end of remarkably long EEPROM stored settings section
