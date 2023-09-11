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
#if defined(useTestButtonValues)
static const uint8_t buttonDisplayIdx =				nextAllowedValue;
#define nextAllowedValue buttonDisplayIdx + 1
#endif // defined(useTestButtonValues)

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

static const uint8_t pSizeSignature =					32;
static const uint8_t pSizeMetricMode =					1;
static const uint8_t pSizeAlternateFE =					1;
static const uint8_t pSizeMicroSecondsPerGallon =		32;
static const uint8_t pSizeInjEdgeTrigger =				1;
static const uint8_t pSizeInjectorSettleTime =			16;
static const uint8_t pSizeInjPer2CrankRev =				8;
static const uint8_t pSizeMinGoodRPM =					16;
static const uint8_t pSizePulsesPerDistance =			18;
static const uint8_t pSizeVSSpause =					8;
static const uint8_t pSizeMinGoodSpeed =				16;
static const uint8_t pSizeTankSize =					24;
static const uint8_t pSizeTankBingoSize =				pSizeTankSize;
static const uint8_t pSizeIdleTimeout =					8;
static const uint8_t pSizeEOCtimeout =					8;
static const uint8_t pSizeButtonTimeout =				8;
static const uint8_t pSizeParkTimeout =					8;
static const uint8_t pSizeActivityTimeout =				8;
static const uint8_t pSizeScratchpad =					32;

#if defined(useButtonInput)
static const uint8_t pSizeWakeupResetCurrentOnEngine =	1;
static const uint8_t pSizeWakeupResetCurrentOnMove =	1;
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
#if defined(useBinaryLCDbrightness)
static const uint8_t pSizeBrightness =					1;
#else // defined(useBinaryLCDbrightness)
static const uint8_t pSizeBrightness =					2;
#endif // defined(useBinaryLCDbrightness)
#if defined(useLCDcontrast)
static const uint8_t pSizeContrast =					1;
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
static const uint8_t pSizeLCDcolor =					3;
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
static const uint8_t pSizeFuelUnitCost =				24;
#endif // defined(useFuelCost)
#if defined(useOutputPins)
static const uint8_t pSizeOutputPin1Mode =				3;
static const uint8_t pSizeOutputPin2Mode =				3;
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
static const uint8_t pSizeVoltageOffset =				12;
#endif // defined(useCarVoltageOutput)
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
static const uint8_t pSizeSysFuelPressure =				32;
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
static const uint8_t pSizeRefFuelPressure =				32;
static const uint8_t pSizeInjectorCount =				8;
static const uint8_t pSizeInjectorSize =				20;
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
static const uint8_t pSizeMAPsensorFloor =				16;
static const uint8_t pSizeMAPsensorCeiling =			16;
static const uint8_t pSizeMAPsensorRange =				32;
static const uint8_t pSizeMAPsensorOffset =				32;
#if defined(useChryslerBaroSensor)
static const uint8_t pSizeBaroSensorFloor =				16;
static const uint8_t pSizeBaroSensorCeiling =			16;
static const uint8_t pSizeBaroSensorRange =				32;
static const uint8_t pSizeBaroSensorOffset =			32;
#else // defined(useChryslerBaroSensor)
static const uint8_t pSizeBarometricPressure =			32;
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint8_t pSizeVehicleMass =					16;
#if defined(useCoastDownCalculator)
static const uint8_t pSizeVehicleFrontalArea =			16;
static const uint8_t pSizeLocustDensity =				16;
static const uint8_t pSizeCoefficientD =				16;
static const uint8_t pSizeCoefficientV =				16;
static const uint8_t pSizeCoefficientRR =				16;
static const uint8_t pSizeCoastdownSamplePeriod =		8;
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
static const uint8_t pSizeDragSpeed =					24;
static const uint8_t pSizeDragDistance =				16;
static const uint8_t pSizeDragAutoFlag =				1;
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
static const uint8_t pSizeAutoSaveActive =				1;
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
static const uint8_t pSizeRefuelSize =					pSizeTankSize;
#endif // defined(usePartialRefuel)

#if defined(useButtonInput)
static const uint8_t pSizeDisplayIdx =					8;
#if LCDcharHeight == 4
static const uint8_t pSizeBottomDisplayIdx =			8;
static const uint8_t pSizeBottomCursorIdx =				8;
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
static const uint8_t pSizeRefuelSaveSizeIdx =			pSizeTankSize;
#endif // defined(usePartialRefuel)
static const uint8_t pSizeCurrTripSignatureIdx =		8;
static const uint8_t pSizeTankTripSignatureIdx =		8;
static const uint8_t pSizeCurrTripVSSpulseIdx =			32;
static const uint8_t pSizeCurrTripVSScycleIdx =			64;
static const uint8_t pSizeCurrTripInjPulseIdx =			32;
static const uint8_t pSizeCurrTripInjCycleIdx =			64;
static const uint8_t pSizeCurrTripEngCycleIdx =			64;
static const uint8_t pSizeTankTripVSSpulseIdx =			32;
static const uint8_t pSizeTankTripVSScycleIdx =			64;
static const uint8_t pSizeTankTripInjPulseIdx =			32;
static const uint8_t pSizeTankTripInjCycleIdx =			64;
static const uint8_t pSizeTankTripEngCycleIdx =			64;
#if defined(trackIdleEOCdata)
static const uint8_t pSizeCurrIEOCvssPulseIdx =			32;
static const uint8_t pSizeCurrIEOCvssCycleIdx =			64;
static const uint8_t pSizeCurrIEOCinjPulseIdx =			32;
static const uint8_t pSizeCurrIEOCinjCycleIdx =			64;
static const uint8_t pSizeCurrIEOCengCycleIdx =			64;
static const uint8_t pSizeTankIEOCvssPulseIdx =			32;
static const uint8_t pSizeTankIEOCvssCycleIdx =			64;
static const uint8_t pSizeTankIEOCinjPulseIdx =			32;
static const uint8_t pSizeTankIEOCinjCycleIdx =			64;
static const uint8_t pSizeTankIEOCengCycleIdx =			64;
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)

#define byteSize(bitLength) ((((bitLength & 0x07) != 0)? 1 : 0) + (bitLength / 8))

/* parameter addresses from sizes defined above */

static const uint16_t eeAdrSignature = 0;

#define nextAllowedValue eeAdrSignature
static const uint16_t pAddressSignature =					nextAllowedValue;
static const uint16_t pAddressMetricMode =					pAddressSignature + byteSize(pSizeSignature);
static const uint16_t pAddressAlternateFE =					pAddressMetricMode + byteSize(pSizeMetricMode);
static const uint16_t pAddressMicroSecondsPerGallon =		pAddressAlternateFE + byteSize(pSizeAlternateFE);;
static const uint16_t pAddressInjEdgeTrigger =				pAddressMicroSecondsPerGallon + byteSize(pSizeMicroSecondsPerGallon);
static const uint16_t pAddressInjectorSettleTime =			pAddressInjEdgeTrigger + byteSize(pSizeInjEdgeTrigger);
static const uint16_t pAddressInjPer2CrankRev =				pAddressInjectorSettleTime + byteSize(pSizeInjectorSettleTime);
static const uint16_t pAddressMinGoodRPM =					pAddressInjPer2CrankRev + byteSize(pSizeInjPer2CrankRev);
static const uint16_t pAddressPulsesPerDistance =			pAddressMinGoodRPM + byteSize(pSizeMinGoodRPM);
static const uint16_t pAddressVSSpause =					pAddressPulsesPerDistance + byteSize(pSizePulsesPerDistance);
static const uint16_t pAddressMinGoodSpeed =				pAddressVSSpause + byteSize(pSizeVSSpause);
static const uint16_t pAddressTankSize =					pAddressMinGoodSpeed + byteSize(pSizeMinGoodSpeed);
static const uint16_t pAddressTankBingoSize =				pAddressTankSize + byteSize(pSizeTankSize);
static const uint16_t pAddressIdleTimeout =					pAddressTankBingoSize + byteSize(pSizeTankBingoSize);
static const uint16_t pAddressEOCtimeout =					pAddressIdleTimeout + byteSize(pSizeIdleTimeout);
static const uint16_t pAddressButtonTimeout =				pAddressEOCtimeout + byteSize(pSizeEOCtimeout);
static const uint16_t pAddressParkTimeout =					pAddressButtonTimeout + byteSize(pSizeButtonTimeout);
static const uint16_t pAddressActivityTimeout =				pAddressParkTimeout + byteSize(pSizeParkTimeout);
static const uint16_t pAddressScratchpad =					pAddressActivityTimeout + byteSize(pSizeActivityTimeout);
#define nextAllowedValue pAddressScratchpad + byteSize(pSizeScratchpad)

#if defined(useButtonInput)
static const uint16_t pAddressWakeupResetCurrentOnEngine =	nextAllowedValue;
static const uint16_t pAddressWakeupResetCurrentOnMove =	pAddressWakeupResetCurrentOnEngine + byteSize(pSizeWakeupResetCurrentOnEngine);
#define nextAllowedValue pAddressWakeupResetCurrentOnMove + byteSize(pSizeWakeupResetCurrentOnMove)
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
static const uint16_t pAddressBrightness =					nextAllowedValue;
#define nextAllowedValue pAddressBrightness + byteSize(pSizeBrightness)
#if defined(useLCDcontrast)
static const uint16_t pAddressContrast =					nextAllowedValue;
#define nextAllowedValue pAddressContrast + byteSize(pSizeContrast)
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
static const uint16_t pAddressLCDcolor =					nextAllowedValue;
#define nextAllowedValue pAddressLCDcolor + byteSize(pSizeLCDcolor)
#endif // defined(useAdafruitRGBLCDshield)
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
#if defined(useCarVoltageOutput)
static const uint16_t pAddressVoltageOffset =				nextAllowedValue;
#define nextAllowedValue pAddressVoltageOffset + byteSize(pSizeVoltageOffset)
#endif // defined(useCarVoltageOutput)
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
static const uint16_t pAddressSysFuelPressure =				nextAllowedValue;
#define nextAllowedValue pAddressSysFuelPressure + byteSize(pSizeSysFuelPressure)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
static const uint16_t pAddressRefFuelPressure =				nextAllowedValue;
static const uint16_t pAddressInjectorCount =				pAddressRefFuelPressure + byteSize(pSizeRefFuelPressure);
static const uint16_t pAddressInjectorSize =				pAddressInjectorCount + byteSize(pSizeInjectorCount);
#define nextAllowedValue pAddressInjectorSize + byteSize(pSizeInjectorSize)
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
static const uint16_t pAddressMAPsensorFloor =				nextAllowedValue;
static const uint16_t pAddressMAPsensorCeiling =			pAddressMAPsensorFloor + byteSize(pSizeMAPsensorFloor);
static const uint16_t pAddressMAPsensorRange =				pAddressMAPsensorCeiling + byteSize(pSizeMAPsensorCeiling);
static const uint16_t pAddressMAPsensorOffset =				pAddressMAPsensorRange + byteSize(pSizeMAPsensorRange);
#define nextAllowedValue pAddressMAPsensorOffset + byteSize(pSizeMAPsensorOffset)
#if defined(useChryslerBaroSensor)
static const uint16_t pAddressBaroSensorFloor =				nextAllowedValue;
static const uint16_t pAddressBaroSensorCeiling =			pAddressBaroSensorFloor + byteSize(pSizeBaroSensorFloor);
static const uint16_t pAddressBaroSensorRange =				pAddressBaroSensorCeiling + byteSize(pSizeBaroSensorCeiling);
static const uint16_t pAddressBaroSensorOffset =			pAddressBaroSensorRange + byteSize(pSizeBaroSensorRange);
#define nextAllowedValue pAddressBaroSensorOffset + byteSize(pSizeBaroSensorOffset)
#else // defined(useChryslerBaroSensor)
static const uint16_t pAddressBarometricPressure =			nextAllowedValue;
#define nextAllowedValue pAddressBarometricPressure + byteSize(pSizeBarometricPressure)
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint16_t pAddressVehicleMass =					nextAllowedValue;
#define nextAllowedValue pAddressVehicleMass + byteSize(pSizeVehicleMass)
#if defined(useCoastDownCalculator)
static const uint16_t pAddressVehicleFrontalArea =			nextAllowedValue;
static const uint16_t pAddressLocustDensity =				pAddressVehicleFrontalArea + byteSize(pSizeVehicleFrontalArea);
static const uint16_t pAddressCoefficientD =				pAddressLocustDensity + byteSize(pSizeLocustDensity);
static const uint16_t pAddressCoefficientV =				pAddressCoefficientD + byteSize(pSizeCoefficientD);
static const uint16_t pAddressCoefficientRR =				pAddressCoefficientV + byteSize(pSizeCoefficientV);
static const uint16_t pAddressCoastdownSamplePeriod =		pAddressCoefficientRR + byteSize(pSizeCoefficientRR);
#define nextAllowedValue pAddressCoastdownSamplePeriod + byteSize(pSizeCoastdownSamplePeriod)
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
static const uint16_t pAddressDragSpeed =					nextAllowedValue;
static const uint16_t pAddressDragDistance =				pAddressDragSpeed + byteSize(pSizeDragSpeed);
static const uint16_t pAddressDragAutoFlag =				pAddressDragDistance + byteSize(pSizeDragDistance);
#define nextAllowedValue pAddressDragAutoFlag + byteSize(pSizeDragAutoFlag)
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
static const uint16_t pAddressAutoSaveActive =				nextAllowedValue;
#define nextAllowedValue pAddressAutoSaveActive + byteSize(pSizeAutoSaveActive)
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
static const uint16_t pAddressRefuelSize =					nextAllowedValue;
#define nextAllowedValue pAddressRefuelSize + byteSize(pSizeRefuelSize)
#endif // defined(usePartialRefuel)

#if defined(useButtonInput)
static const uint16_t pAddressDisplayIdx =					nextAllowedValue;
#define nextAllowedValue pAddressDisplayIdx + byteSize(pSizeDisplayIdx)
#if LCDcharHeight == 4
static const uint16_t pAddressBottomDisplayIdx =			nextAllowedValue;
static const uint16_t pAddressBottomCursorIdx =				pAddressBottomDisplayIdx + byteSize(pSizeBottomDisplayIdx);
#define nextAllowedValue pAddressBottomCursorIdx + byteSize(pSizeBottomCursorIdx)
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
static const uint16_t pAddressRefuelSaveSizeIdx =			nextAllowedValue;
#define nextAllowedValue pAddressRefuelSaveSizeIdx + byteSize(pSizeRefuelSaveSizeIdx)
#endif // defined(usePartialRefuel)
static const uint16_t pAddressCurrTripSignatureIdx =		nextAllowedValue;
static const uint16_t pAddressTankTripSignatureIdx =		pAddressCurrTripSignatureIdx + byteSize(pSizeCurrTripSignatureIdx);
static const uint16_t pAddressCurrTripVSSpulseIdx =			pAddressTankTripSignatureIdx + byteSize(pSizeTankTripSignatureIdx);
static const uint16_t pAddressCurrTripVSScycleIdx =			pAddressCurrTripVSSpulseIdx + byteSize(pSizeCurrTripVSSpulseIdx);
static const uint16_t pAddressCurrTripInjPulseIdx =			pAddressCurrTripVSScycleIdx + byteSize(pSizeCurrTripVSScycleIdx);
static const uint16_t pAddressCurrTripInjCycleIdx =			pAddressCurrTripInjPulseIdx + byteSize(pSizeCurrTripInjPulseIdx);
static const uint16_t pAddressCurrTripEngCycleIdx =			pAddressCurrTripInjCycleIdx + byteSize(pSizeCurrTripInjCycleIdx);
static const uint16_t pAddressTankTripVSSpulseIdx =			pAddressCurrTripEngCycleIdx + byteSize(pSizeCurrTripEngCycleIdx);
static const uint16_t pAddressTankTripVSScycleIdx =			pAddressTankTripVSSpulseIdx + byteSize(pSizeTankTripVSSpulseIdx);
static const uint16_t pAddressTankTripInjPulseIdx =			pAddressTankTripVSScycleIdx + byteSize(pSizeTankTripVSScycleIdx);
static const uint16_t pAddressTankTripInjCycleIdx =			pAddressTankTripInjPulseIdx + byteSize(pSizeTankTripInjPulseIdx);
static const uint16_t pAddressTankTripEngCycleIdx =			pAddressTankTripInjCycleIdx + byteSize(pSizeTankTripInjCycleIdx);
#define nextAllowedValue pAddressTankTripEngCycleIdx + byteSize(pSizeTankTripEngCycleIdx)
#if defined(trackIdleEOCdata)
static const uint16_t pAddressCurrIEOCvssPulseIdx =			nextAllowedValue;
static const uint16_t pAddressCurrIEOCvssCycleIdx =			pAddressCurrIEOCvssPulseIdx + byteSize(pSizeCurrIEOCvssPulseIdx);
static const uint16_t pAddressCurrIEOCinjPulseIdx =			pAddressCurrIEOCvssCycleIdx + byteSize(pSizeCurrIEOCvssCycleIdx);
static const uint16_t pAddressCurrIEOCinjCycleIdx =			pAddressCurrIEOCinjPulseIdx + byteSize(pSizeCurrIEOCinjPulseIdx);
static const uint16_t pAddressCurrIEOCengCycleIdx =			pAddressCurrIEOCinjCycleIdx + byteSize(pSizeCurrIEOCinjCycleIdx);
static const uint16_t pAddressTankIEOCvssPulseIdx =			pAddressCurrIEOCengCycleIdx + byteSize(pSizeCurrIEOCengCycleIdx);
static const uint16_t pAddressTankIEOCvssCycleIdx =			pAddressTankIEOCvssPulseIdx + byteSize(pSizeTankIEOCvssPulseIdx);
static const uint16_t pAddressTankIEOCinjPulseIdx =			pAddressTankIEOCvssCycleIdx + byteSize(pSizeTankIEOCvssCycleIdx);
static const uint16_t pAddressTankIEOCinjCycleIdx =			pAddressTankIEOCinjPulseIdx + byteSize(pSizeTankIEOCinjPulseIdx);
static const uint16_t pAddressTankIEOCengCycleIdx =			pAddressTankIEOCinjCycleIdx + byteSize(pSizeTankIEOCinjCycleIdx);
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

static const unsigned int eeAdrStorageEnd =					nextAllowedValue;

/* parameter indexes */

#define nextAllowedValue 0
static const uint8_t pSignatureIdx =					nextAllowedValue;
static const uint8_t pMetricModeIdx =					pSignatureIdx + 1;
static const uint8_t pAlternateFEidx =					pMetricModeIdx + 1;
static const uint8_t pMicroSecondsPerGallonIdx =		pAlternateFEidx + 1;
static const uint8_t pInjEdgeTriggerIdx =				pMicroSecondsPerGallonIdx + 1;
static const uint8_t pInjectorSettleTimeIdx =			pInjEdgeTriggerIdx + 1;
static const uint8_t pInjPer2CrankRevIdx =				pInjectorSettleTimeIdx + 1;
static const uint8_t pMinGoodRPMidx =					pInjPer2CrankRevIdx + 1;
static const uint8_t pPulsesPerDistanceIdx =			pMinGoodRPMidx + 1;
static const uint8_t pVSSpauseIdx =						pPulsesPerDistanceIdx + 1;
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
#if defined(useAdafruitRGBLCDshield)
static const uint8_t pLCDcolorIdx =						nextAllowedValue;
#define nextAllowedValue pLCDcolorIdx + 1
#endif // defined(useAdafruitRGBLCDshield)
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
#if defined(useCarVoltageOutput)
static const uint8_t pVoltageOffset =					nextAllowedValue;
#define nextAllowedValue pVoltageOffset + 1
#endif // defined(useCarVoltageOutput)
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
#if defined(useButtonInput)
static const uint8_t pDisplayIdx =						nextAllowedValue;
#define nextAllowedValue pDisplayIdx + 1
#if LCDcharHeight == 4
static const uint8_t pBottomDisplayIdx =				nextAllowedValue;
#define nextAllowedValue pBottomDisplayIdx + 1
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)

static const uint8_t eePtrSettingsEnd =					nextAllowedValue;

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
	"pPulsesPerDistanceIdx" tcEOS
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
#if defined(useAdafruitRGBLCDshield)
	"pLCDcolorIdx" tcEOS
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	"pCostPerQuantity" tcEOS
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	"pOutputPin1Mode" tcEOS
	"pOutputPin2Mode" tcEOS
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	"pVoltageOffset" tcEOS
#endif // defined(useCarVoltageOutput)
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

#if defined(useButtonInput)
	"pDisplayIdx" tcEOS
#if LCDcharHeight == 4
	"pBottomDisplayIdx" tcEOS
	"pBottomCursorIdx" tcEOS
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
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
#if defined(useTestButtonValues)
	"buttonDisplayIdx" tcEOS
#endif // defined(useTestButtonValues)

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

static const uint8_t pfDoNothing =				0;
static const uint8_t pfSoftwareInitMPGuino =	pfDoNothing + 32;
static const uint8_t pfHardwareInitMPGuino =	pfSoftwareInitMPGuino + 32;
static const uint8_t pfDoMetricModeConversion =	pfHardwareInitMPGuino + 32;
static const uint8_t pfChangeDisplay =			pfDoMetricModeConversion + 32;
static const uint8_t pfCalculateFuelParams =	pfChangeDisplay + 32;

static const uint8_t paramsLength[(uint16_t)(eePtrStorageEnd)] PROGMEM = {
	(pSizeSignature & 0x07) | pfHardwareInitMPGuino,							// EEPROM MPGuino signature long word
	(pSizeMetricMode & 0x07) | pfDoMetricModeConversion,						// Display Mode (0 - US Display, 1 - Metric Display)
	(pSizeAlternateFE & 0x07) | pfChangeDisplay,								// 0 - MPG or L/100km, 1 - G/100mile or km/L
	(pSizeMicroSecondsPerGallon & 0x07) | pfSoftwareInitMPGuino,				// Microseconds per US gallon
	(pSizeInjEdgeTrigger & 0x07) | pfHardwareInitMPGuino,						// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	(pSizeInjectorSettleTime & 0x07) | pfSoftwareInitMPGuino,					// Fuel Injector Opening Delay Time (us)
	(pSizeInjPer2CrankRev & 0x07) | pfSoftwareInitMPGuino,						// Crankshaft Revolutions per Fuel Injector Event
	(pSizeMinGoodRPM & 0x07) | pfSoftwareInitMPGuino,							// Minimum Engine Speed For Engine On (RPM)
	(pSizePulsesPerDistance & 0x07) | pfSoftwareInitMPGuino,					// VSS Pulses (per mile or per km)
	(pSizeVSSpause & 0x07) | pfHardwareInitMPGuino,								// VSS Pause Debounce Count (ms)
	(pSizeMinGoodSpeed & 0x07) | pfSoftwareInitMPGuino,							// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	(pSizeTankSize & 0x07) | pfSoftwareInitMPGuino,								// Tank Capacity * 1000 (gal or L)
	(pSizeTankBingoSize & 0x07) | pfSoftwareInitMPGuino,						// Bingo Fuel (reserve tank) Capacity * 1000 (gal or L)
	(pSizeIdleTimeout & 0x07) | pfSoftwareInitMPGuino,							// Engine Idle Timeout (s)
	(pSizeEOCtimeout & 0x07) | pfSoftwareInitMPGuino,							// Engine-Off Coasting Timeout (s)
	(pSizeButtonTimeout & 0x07) | pfSoftwareInitMPGuino,						// Button Press Activity Timeout (s)
	(pSizeParkTimeout & 0x07) | pfSoftwareInitMPGuino,							// Vehicle Parked (engine off, no movement) Timeout (s)
	(pSizeActivityTimeout & 0x07) | pfSoftwareInitMPGuino,						// Activity (engine off, no movement, no button press) Timeout (s)
	(pSizeScratchpad & 0x07),													// Scratchpad Memory
#if defined(useButtonInput)
	(pSizeWakeupResetCurrentOnEngine & 0x07),									// Enable current trip reset upon wakeup due to engine running
	(pSizeWakeupResetCurrentOnMove & 0x07),										// Enable current trip reset upon wakeup due to vehicle movement
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
	(pSizeBrightness & 0x07) | pfChangeDisplay,									// LCD Brightness
#if defined(useLCDcontrast)
	(pSizeContrast & 0x07) | pfChangeDisplay,									// LCD Contrast
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	(pSizeLCDcolor & 0x07) | pfChangeDisplay,									// LCD Backlight color
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	(pSizeFuelUnitCost & 0x07),													// Price per unit volume of fuel
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	(pSizeOutputPin1Mode & 0x07),												// Output Pin 1 mode
	(pSizeOutputPin2Mode & 0x07),												// Output Pin 2 mode
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	(pSizeVoltageOffset & 0x07),												// diode offset from V(alternator)
#endif // defined(useCarVoltageOutput)
#if defined(useDataLoggingOutput)
	(pSizeSerialDataLogging & 0x07),											// Serial Data Logging Enable
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	(pSizeJSONoutput & 0x07),													// JSON output Enable
#endif // defined(useJSONoutput)
#if defined(useBluetooth)
	(pSizeBluetoothOutput & 0x07),												// Bluetooth output Enable
#endif // defined(useBluetooth)
#if defined(useBarFuelEconVsTime)
	(pSizeFEvsTime & 0x07) | pfSoftwareInitMPGuino,								// Period Of FE over Time BarGraph Bar (s)
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	(pSizeBarLowSpeedCutoff & 0x07) | pfSoftwareInitMPGuino,					// FE vs Speed Bargraph lower speed
	(pSizeBarSpeedQuantumIdx & 0x07) | pfSoftwareInitMPGuino,					// FE vs Speed Bargraph speed bar size
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useFuelPressure)
	(pSizeSysFuelPressure & 0x07) | pfSoftwareInitMPGuino,						// Fuel System Pressure (Pa or * 1000 psig)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	(pSizeRefFuelPressure & 0x07) | pfCalculateFuelParams,						// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	(pSizeInjectorCount & 0x07) | pfCalculateFuelParams,						// Fuel Injector Count
	(pSizeInjectorSize & 0x07) | pfCalculateFuelParams,							// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	(pSizeMAPsensorFloor & 0x07) | pfSoftwareInitMPGuino,						// MAP Sensor Floor * 1000 (V)
	(pSizeMAPsensorCeiling & 0x07) | pfSoftwareInitMPGuino,						// MAP Sensor Ceiling * 1000 (V)
	(pSizeMAPsensorRange & 0x07) | pfSoftwareInitMPGuino,						// MAP Sensor Range (Pa or * 1000 psig)
	(pSizeMAPsensorOffset & 0x07) | pfSoftwareInitMPGuino,						// MAP Sensor Offset (Pa or * 1000 psia)
#if defined(useChryslerBaroSensor)
	(pSizeBaroSensorFloor & 0x07) | pfSoftwareInitMPGuino,						// Barometric Sensor Floor * 1000 (V)
	(pSizeBaroSensorCeiling & 0x07) | pfSoftwareInitMPGuino,					// Barometric Sensor Ceiling * 1000 (V)
	(pSizeBaroSensorRange & 0x07) | pfSoftwareInitMPGuino,						// Barometric Sensor Range (Pa or * 1000 psig)
	(pSizeBaroSensorOffset & 0x07) | pfSoftwareInitMPGuino,						// Barometric Sensor Offset (Pa or * 1000 psia)
#else // defined(useChryslerBaroSensor)
	(pSizeBarometricPressure & 0x07) | pfSoftwareInitMPGuino,					// Reference Barometric Pressure
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	(pSizeVehicleMass & 0x07),													// Vehicle Weight/Mass (lbs or kg)
#if defined(useCoastDownCalculator)
	(pSizeVehicleFrontalArea & 0x07),											// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	(pSizeLocustDensity & 0x07),												// Air density (lb/yd^3 or kg/m^3)
	(pSizeCoefficientD & 0x07),													// Vehicle C(d) * 1000
	(pSizeCoefficientV & 0x07),													// Vehicle C(v) * 1000
	(pSizeCoefficientRR & 0x07),												// Vehicle C(rr) * 1000
	(pSizeCoastdownSamplePeriod & 0x07),										// Sample Period in seconds
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	(pSizeDragSpeed & 0x07),													// Drag speed cutoff (MPH or kph) * 1000
	(pSizeDragDistance & 0x07),													// Drag distance cutoff (miles or km) * 1000
	(pSizeDragAutoFlag & 0x07),													// Drag automatic retrigger on vehicle stop
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	(pSizeAutoSaveActive & 0x07),												// Autosave Active Trip Data Enable
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	(pSizeRefuelSize & 0x07) | pfSoftwareInitMPGuino,							// Partial Refuel amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)

#if defined(useButtonInput)
	(pSizeDisplayIdx & 0x07) | pfChangeDisplay,									// current display index
#if LCDcharHeight == 4
	(pSizeBottomDisplayIdx & 0x07) | pfChangeDisplay,							// bottom display index
	(pSizeBottomCursorIdx & 0x07),												// bottom display cursor position
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
	(pSizeRefuelSaveSizeIdx & 0x07),											// Partial Refuel save amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)
	(pSizeCurrTripSignatureIdx & 0x07),											// Current Trip signature byte
	(pSizeTankTripSignatureIdx & 0x07),											// Tank Trip signature byte
	(pSizeCurrTripVSSpulseIdx & 0x07),											// Current Trip VSS pulse count storage
	(pSizeCurrTripVSScycleIdx & 0x07),											// Current Trip VSS cycle accumulator storage
	(pSizeCurrTripInjPulseIdx & 0x07),											// Current Trip injector pulse count storage
	(pSizeCurrTripInjCycleIdx & 0x07),											// Current Trip injector open cycle accumulator storage
	(pSizeCurrTripEngCycleIdx & 0x07),											// Current Trip engine revolution cycle accumulator storage
	(pSizeTankTripVSSpulseIdx & 0x07),											// Tank Trip VSS pulse count storage
	(pSizeTankTripVSScycleIdx & 0x07),											// Tank Trip VSS cycle accumulator storage
	(pSizeTankTripInjPulseIdx & 0x07),											// Tank Trip injector pulse count storage
	(pSizeTankTripInjCycleIdx & 0x07),											// Tank Trip injector open cycle accumulator storage
	(pSizeTankTripEngCycleIdx & 0x07),											// Tank Trip engine revolution cycle accumulator storage
#if defined(trackIdleEOCdata)
	(pSizeCurrIEOCvssPulseIdx & 0x07),											// Current Idle/EOC Trip VSS pulse count storage
	(pSizeCurrIEOCvssCycleIdx & 0x07),											// Current Idle/EOC Trip VSS cycle accumulator storage
	(pSizeCurrIEOCinjPulseIdx & 0x07),											// Current Idle/EOC Trip injector pulse count storage
	(pSizeCurrIEOCinjCycleIdx & 0x07),											// Current Idle/EOC Trip injector open cycle accumulator storage
	(pSizeCurrIEOCengCycleIdx & 0x07),											// Current Idle/EOC Trip engine revolution cycle accumulator storage
	(pSizeTankIEOCvssPulseIdx & 0x07),											// Tank Idle/EOC Trip VSS pulse count storage
	(pSizeTankIEOCvssCycleIdx & 0x07),											// Tank Idle/EOC Trip VSS cycle accumulator storage
	(pSizeTankIEOCinjPulseIdx & 0x07),											// Tank Idle/EOC Trip injector pulse count storage
	(pSizeTankIEOCinjCycleIdx & 0x07),											// Tank Idle/EOC Trip injector open cycle accumulator storage
	(pSizeTankIEOCengCycleIdx & 0x07),											// Tank Idle/EOC Trip engine revolution cycle accumulator storage
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

/* parameter EEPROM addresses for parameter definitions above */

static const uint16_t paramAddrs[(uint16_t)(eePtrStorageEnd)] PROGMEM = {
	pAddressSignature,					// EEPROM signature long word for MPGuino
	pAddressMetricMode,					// Display Mode (0 - US Display, 1 - Metric Display)
	pAddressAlternateFE,				// 0 - MPG or L/100km, 1 - G/100mile or km/L
	pAddressMicroSecondsPerGallon,		// Microseconds per US gallon
	pAddressInjEdgeTrigger,				// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	pAddressInjectorSettleTime,			// Fuel Injector Opening Delay Time (us)
	pAddressInjPer2CrankRev,			// Fuel Injector Event Count for every 2 Crankshaft Revolutions
	pAddressMinGoodRPM,					// Minimum Engine Speed For Engine On (RPM)
	pAddressPulsesPerDistance,			// VSS Pulses (per mile or per km)
	pAddressVSSpause,					// VSS Pause Debounce Count (ms)
	pAddressMinGoodSpeed,				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	pAddressTankSize,					// Tank Capacity * 1000 (gal or L)
	pAddressTankBingoSize,				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
	pAddressIdleTimeout,				// Engine Idle Timeout (s)
	pAddressEOCtimeout,					// Engine-Off Coasting Timeout (s)
	pAddressButtonTimeout,				// Button Press Activity Timeout (s)
	pAddressParkTimeout,				// Vehicle Parked (engine off, no movement) Timeout (s)
	pAddressActivityTimeout,			// Activity (engine off, no movement, no button press) Timeout (s)
	pAddressScratchpad,					// Scratchpad Memory
#if defined(useButtonInput)
	pAddressWakeupResetCurrentOnEngine,	// Enable current trip reset upon wakeup due to engine running
	pAddressWakeupResetCurrentOnMove,	// Enable current trip reset upon wakeup due to vehicle movement
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
	pAddressBrightness,					// LCD Brightness
#if defined(useLCDcontrast)
	pAddressContrast,					// LCD Contrast
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	pAddressLCDcolor,					// LCD Backlight color
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	pAddressFuelUnitCost,				// Price per unit volume of fuel
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	pAddressOutputPin1Mode,				// Output Pin 1 mode
	pAddressOutputPin2Mode,				// Output Pin 2 mode
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	pAddressVoltageOffset,				// diode offset from V(alternator)
#endif // defined(useCarVoltageOutput)
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
	pAddressSysFuelPressure,			// Fuel System Pressure (Pa or * 1000 psig)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	pAddressRefFuelPressure,			// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	pAddressInjectorCount,				// Fuel Injector Count
	pAddressInjectorSize,				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	pAddressMAPsensorFloor,				// MAP Sensor Floor * 1000 (V)
	pAddressMAPsensorCeiling,			// MAP Sensor Ceiling * 1000 (V)
	pAddressMAPsensorRange,				// MAP Sensor Range (Pa or * 1000 psig)
	pAddressMAPsensorOffset,			// MAP Sensor Offset (Pa or * 1000 psia)
#if defined(useChryslerBaroSensor)
	pAddressBaroSensorFloor,			// Barometric Sensor Floor * 1000 (V)
	pAddressBaroSensorCeiling,			// Barometric Sensor Ceiling * 1000 (V)
	pAddressBaroSensorRange,			// Barometric Sensor Range (Pa or * 1000 psig)
	pAddressBaroSensorOffset,			// Barometric Sensor Offset (Pa or * 1000 psia)
#else // defined(useChryslerBaroSensor)
	pAddressBarometricPressure,			// Reference Barometric Pressure
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	pAddressVehicleMass,				// Vehicle Weight/Mass (lbs or kg)
#if defined(useCoastDownCalculator)
	pAddressVehicleFrontalArea,			// Vehicle Frontal Area * 1000 (ft^2 or m^2)
	pAddressLocustDensity,				// Air density (lb/yd^3 or kg/m^3)
	pAddressCoefficientD,				// Vehicle C(d) * 1000
	pAddressCoefficientV,				// Vehicle C(v) * 1000
	pAddressCoefficientRR,				// Vehicle C(rr) * 1000
	pAddressCoastdownSamplePeriod,		// Sample Period in seconds
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	pAddressDragSpeed,					// Drag speed cutoff (MPH or kph) * 1000
	pAddressDragDistance,				// Drag distance cutoff (miles or km) * 1000
	pAddressDragAutoFlag,				// Drag automatic retrigger on vehicle stop
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	pAddressAutoSaveActive,				// Autosave Active Trip Data Enable
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	pAddressRefuelSize,					// Partial Refuel amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)

#if defined(useButtonInput)
	pAddressDisplayIdx,					// current display index
#if LCDcharHeight == 4
	pAddressBottomDisplayIdx,			// bottom display index
	pAddressBottomCursorIdx,			// bottom display cursor position
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
#if defined(useEEPROMtripStorage)
#if defined(usePartialRefuel)
	pAddressRefuelSaveSizeIdx,			// Partial Refuel save amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)
	pAddressCurrTripSignatureIdx,		// Current Trip signature byte
	pAddressTankTripSignatureIdx,		// Tank Trip signature byte
	pAddressCurrTripVSSpulseIdx,		// Current Trip VSS pulse count storage
	pAddressCurrTripVSScycleIdx,		// Current Trip VSS cycle accumulator storage
	pAddressCurrTripInjPulseIdx,		// Current Trip injector pulse count storage
	pAddressCurrTripInjCycleIdx,		// Current Trip injector open cycle accumulator storage
	pAddressCurrTripEngCycleIdx,		// Current Trip engine revolution cycle accumulator storage
	pAddressTankTripVSSpulseIdx,		// Tank Trip VSS pulse count storage
	pAddressTankTripVSScycleIdx,		// Tank Trip VSS cycle accumulator storage
	pAddressTankTripInjPulseIdx,		// Tank Trip injector pulse count storage
	pAddressTankTripInjCycleIdx,		// Tank Trip injector open cycle accumulator storage
	pAddressTankTripEngCycleIdx,		// Tank Trip engine revolution cycle accumulator storage
#if defined(trackIdleEOCdata)
	pAddressCurrIEOCvssPulseIdx,		// Current Idle/EOC Trip VSS pulse count storage
	pAddressCurrIEOCvssCycleIdx,		// Current Idle/EOC Trip VSS cycle accumulator storage
	pAddressCurrIEOCinjPulseIdx,		// Current Idle/EOC Trip injector pulse count storage
	pAddressCurrIEOCinjCycleIdx,		// Current Idle/EOC Trip injector open cycle accumulator storage
	pAddressCurrIEOCengCycleIdx,		// Current Idle/EOC Trip engine revolution cycle accumulator storage
	pAddressTankIEOCvssPulseIdx,		// Tank Idle/EOC Trip VSS pulse count storage
	pAddressTankIEOCvssCycleIdx,		// Tank Idle/EOC Trip VSS cycle accumulator storage
	pAddressTankIEOCinjPulseIdx,		// Tank Idle/EOC Trip injector pulse count storage
	pAddressTankIEOCinjCycleIdx,		// Tank Idle/EOC Trip injector open cycle accumulator storage
	pAddressTankIEOCengCycleIdx,		// Tank Idle/EOC Trip engine revolution cycle accumulator storage
#endif // defined(trackIdleEOCdata)
#endif // defined(useEEPROMtripStorage)
};

/* parameter values for parameter definitions above */

static const uint32_t newEEPROMsignature = ((uint32_t)(guinosig) << 24) + ((uint32_t)(eePtrEnd) << 16) + (uint32_t)(eeAdrSettingsEnd);

static const uint32_t params[(uint16_t)(eePtrSettingsEnd)] PROGMEM = {
	newEEPROMsignature,	// EEPROM MPGuino signature long word
	0,					// Display Mode (0 - US Display, 1 - Metric Display)
	0,					// 0 - MPG or L/100km, 1 - G/100mile or km/L
	133262651,			// Microseconds per US gallon
	0,					// Fuel Injector Edge Trigger (0 - Falling Edge, 1 - Rising Edge)
	550,				// Fuel Injector Response Delay Time (us)
	1,					// Fuel Injector Event Count for every 2 Crankshaft Revolutions
	40,					// Minimum Engine Speed For Engine On (RPM)
	10000,				// VSS Pulses (per mile or per km)
	0,					// VSS Pause Debounce Count (ms)
	300,				// Minimum Vehicle Speed for Vehicle in Motion (MPH or kph) * 1000
	18000,				// Tank Capacity * 1000 (gal or L)
	2200,				// Bingo Fuel (Reserve Fuel) Capacity * 1000 (gal or L)
	15,					// Engine Idle Timeout (s)
	2,					// Engine-Off Coasting Timeout (s)
	5,					// Button Press Activity Timeout (s)
	5,					// Vehicle Parked (engine off, no movement) Timeout (s)
	120,				// Activity (engine off, no movement, no button press) Timeout (s)
	0,					// Scratchpad Memory

#if defined(useButtonInput)
	1,					// Enable current trip reset upon wakeup due to engine running
	0,					// Enable current trip reset upon wakeup due to button press
#endif // defined(useButtonInput)
#if defined(useLCDoutput)
#if defined(useBinaryLCDbrightness)
	1,					// LCD Brightness
#else // defined(useBinaryLCDbrightness)
	2,					// LCD Brightness
#endif // defined(useBinaryLCDbrightness)
#if defined(useLCDcontrast)
	55,					// LCD Contrast
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	3,					// LCD Backlight color
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	2710259,			// Price per unit volume of fuel (this prices is for a liter of diesel at 717 HUF / liter)
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	0,					// Output pin 1 mode
	0,					// Output pin 2 mode
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	700,				// diode offset from V(alternator) (via meelis11)
#endif // defined(useCarVoltageOutput)
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
	43500,				// Fuel System Pressure (Pa or * 1000 psig)
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	43500,				// Reference Fuel Injector Rated Pressure (Pa or * 1000 psig)
	6,					// Fuel Injector Count
	246000,				// Fuel Injector Rated Capacity in cc/min * 1000 at Reference Fuel Injector Rated Pressure
#endif // useCalculatedFuelFactor
#if defined(useChryslerMAPCorrection)
	500,				// MAP Sensor Floor * 1000 (V)
	4000,				// MAP Sensor Ceiling * 1000 (V)
	14145,				// Barometric Sensor Range (Pa or * 1000 psig)
	551,				// MAP Sensor Offset (Pa or * 1000 psia)
#if defined(useChryslerBaroSensor)
	500,				// Barometric Sensor Floor * 1000 (V)
	4000,				// Barometric Sensor Ceiling * 1000 (V)
	14145,				// Barometric Sensor Range (Pa or * 1000 psig)
	551,				// Barometric Sensor Offset (Pa or * 1000 psia)
#else // defined(useChryslerBaroSensor)
	14696,				// Reference Barometric Pressure
#endif // defined(useChryslerBaroSensor)
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	4000,				// Vehicle Weight/Mass (lbs or kg)
#if defined(useCoastDownCalculator)
	25400,				// Vehicle Frontal Area (ft^2 or m^2)
	2065,				// Air density (lb/yd^3 or kg/m^3)
	346,				// C(d)
	1,					// C(v)
	8,					// C(rr)
	3,					// Sample Period in seconds
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	60000,				// Drag speed cutoff (MPH or kph) * 1000
	250,				// Drag distance cutoff (miles or km) * 1000
	0,					// Drag automatic retrigger on vehicle stop
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#if defined(useSavedTrips)
	1,					// Autosave Active Trip Data Enable
#endif // defined(useSavedTrips)
#if defined(usePartialRefuel)
	0,					// Partial Refuel amount * 1000 (gal or L)
#endif // defined(usePartialRefuel)
#if defined(useButtonInput)
	mainDisplayIdx,
#if LCDcharHeight == 4
	mainDisplayIdx,
#endif // LCDcharHeight == 4
#endif // defined(useButtonInput)
};

// end of remarkably long EEPROM stored settings section
