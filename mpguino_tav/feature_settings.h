namespace settings /* EEPROM parameter settings menu support section prototype */
{

	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);

};

static const char settingsMenuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
	"Display&Format" tcEOSCR
	"Fuel Injector" tcEOSCR
	"VSS" tcEOSCR
	"Tank Quantity" tcEOSCR
#if defined(useChryslerMAPCorrection)
	"Chrysler MAP" tcEOSCR
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
	"VehicleParam" tcEOSCR
#endif // defined(useVehicleParameters)
	"Timeout" tcEOSCR
	"Miscellaneous" tcEOSCR
};

static const uint8_t displayCountSettingsDisplay = 2
#if defined(useLCDoutput)
#if defined(useLCDcontrast)
	+ 1
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	+ 1
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	+ 1
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	+ 2
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	+ 1
#endif // defined(useCarVoltageOutput)
#if defined(useDataLoggingOutput)
	+ 1
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	+ 1
#endif // defined(useJSONoutput)
#if defined(useBarFuelEconVsTime)
	+ 1
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	+ 2
#endif // defined(useBarFuelEconVsSpeed)
;

static const uint8_t displayCountSettingsFuel = 6
#if defined(useFuelPressure)
	+ 1
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	+ 3
#endif // useCalculatedFuelFactor
;

#if defined(useChryslerMAPCorrection)
static const uint8_t displayCountSettingsCRFIC = 4
#if defined(useChryslerBaroSensor)
	+ 4
#else // defined(useChryslerBaroSensor)
	+ 1
#endif // defined(useChryslerBaroSensor)
;

#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint8_t displayCountSettingsVehicle = 1
#if defined(useCoastDownCalculator)
	+ 6
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	+ 3
#endif // defined(useDragRaceFunction)
;

#endif // defined(useVehicleParameters)
static const uint8_t displayCountSettingsTimeout = 7
#if defined(useSavedTrips)
	+ 1
#endif // defined(useSavedTrips)
;

static const uint8_t displayCountSettingsVSS = 3;
static const uint8_t displayCountSettingsTank = 2;
static const uint8_t displayCountSettingsMisc = 1;

#define nextAllowedValue 0
static const uint8_t displayStartSettingsDisplay = nextAllowedValue;
static const uint8_t displayStartSettingsFuel = displayStartSettingsDisplay + displayCountSettingsDisplay;
static const uint8_t displayStartSettingsVSS = displayStartSettingsFuel + displayCountSettingsFuel;
static const uint8_t displayStartSettingsTank = displayStartSettingsVSS + displayCountSettingsVSS;
#define nextAllowedValue displayStartSettingsTank + displayCountSettingsTank
#if defined(useChryslerMAPCorrection)
static const uint8_t displayStartSettingsCRFIC = nextAllowedValue;
#define nextAllowedValue displayStartSettingsCRFIC + displayCountSettingsCRFIC
#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
static const uint8_t displayStartSettingsVehicle = nextAllowedValue;
#define nextAllowedValue displayStartSettingsVehicle + displayCountSettingsVehicle
#endif // defined(useVehicleParameters)
static const uint8_t displayStartSettingsTimeout = nextAllowedValue;
static const uint8_t displayStartSettingsMisc = displayStartSettingsTimeout + displayCountSettingsTimeout;
#define nextAllowedValue displayStartSettingsMisc + displayCountSettingsMisc

static const char settingsSubMenuTitles[] PROGMEM = {	// each title must be no longer than 15 characters

// display settings

	"Metric 1-Yes" tcEOSCR
	"AltFEformat 1-Y" tcEOSCR
#if defined(useLCDoutput)
#if defined(useLCDcontrast)
	"Contrast" tcEOSCR
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	"LCD BG Color" tcEOSCR
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
#ifdef useImperialGallon
	"Price*1000/" tcOMOFF "ImpGa" tcOTOG "L" tcOON tcEOSCR
#else  // useImperialGallon
	"Price*1000/" tcOMOFF "USgal" tcOTOG tcOON tcEOSCR
#endif // useImperialGallon
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	"OutPtPin 1 Mode" tcEOSCR
	"OutPtPin 2 Mode" tcEOSCR
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	"V(diode)*1000" tcEOSCR
#endif // defined(useCarVoltageOutput)
#if defined(useDataLoggingOutput)
	"DLogSerial 1-Yes" tcEOSCR
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	"JSONoutput 1-Yes" tcEOSCR
#endif // defined(useJSONoutput)
#if defined(useBarFuelEconVsTime)
	"FE/Time Period s" tcEOSCR
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	"bgLower*1000 " tcOMOFF "MPH" tcOTOG "kph" tcOON tcEOSCR
	"bgSize*1000 " tcOMOFF "MPH" tcOTOG "kph" tcOON tcEOSCR
#endif // defined(useBarFuelEconVsSpeed)

// fuel injection settings

#if defined(useFuelPressure)
	"P(Fuel) " tcOMOFF "psig*1K" tcOTOG "Pa" tcOON tcEOSCR
#endif // defined(useFuelPressure)
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

#if defined(useChryslerMAPCorrection)
// Chrysler settings

	"MAPfloor (mV)" tcEOSCR
	"MAPceiling (mV)" tcEOSCR
	"MAPrng " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
	"MAPofst " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
#if defined(useChryslerBaroSensor)
	"BaroFloor (mV)" tcEOSCR
	"BaroCeiling (mV)" tcEOSCR
	"BaroRng " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
	"BaroOfst " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
#else // defined(useChryslerBaroSensor)
	"BaroPrs " tcOMOFF "psi*1000" tcOTOG "Pa" tcOON tcEOSCR
#endif // defined(useChryslerBaroSensor)

#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
// drag race / coastdown calc settings

	"Veh" tcOMOFF "Weight" tcOTOG "Mass" tcOON " (" tcOMOFF "lbs" tcOTOG "kg" tcOON ")" tcEOSCR
#if defined(useCoastDownCalculator)
	"FrArea*1000 " tcOMOFF "ft" tcOTOG "m" tcOON "^2" tcEOSCR
	"rho*1000 " tcOMOFF "lb/yd" tcOTOG "kg/m" tcOON "^3" tcEOSCR
	"C(d) * 1000" tcEOSCR
	"C(v) * 1000" tcEOSCR
	"C(rr) * 1000" tcEOSCR
	"Cd SamplePd (s)" tcEOSCR
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	"DragSpd " tcOMOFF "MPH" tcOTOG "kph" tcOON "*1000" tcEOSCR
	"DragDist " tcOMOFF "mi" tcOTOG "km" tcOON "*1000" tcEOSCR
	"DragAutoTrigger" tcEOSCR
#endif // defined(useDragRaceFunction)

#endif // defined(useVehicleParameters)
// timeout settings

	"Idle Timeout (s)" tcEOSCR
	"EOC Timeout (s)" tcEOSCR
	"BtnPress T/O (s)" tcEOSCR
	"Park Timeout (s)" tcEOSCR
	"OFF Timeout (s)" tcEOSCR
	"WakeEngRst CURR" tcEOSCR
	"WakeVSSRst CURR" tcEOSCR
#if defined(useSavedTrips)
	"AutoSaveTrip 1-Y" tcEOSCR
#endif // defined(useSavedTrips)

// miscellaneous settings

	"Scratchpad(odo?)" tcEOSCR
};

static const char settingsParameterList[] PROGMEM = {

// display settings

	 pMetricModeIdx
	,pAlternateFEidx
#if defined(useLCDoutput)
#if defined(useLCDcontrast)
	,pContrastIdx
#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	,pLCDcolorIdx
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDoutput)
#if defined(useFuelCost)
	,pCostPerQuantity
#endif // defined(useFuelCost)
#if defined(useOutputPins)
	,pOutputPin1Mode
	,pOutputPin2Mode
#endif // defined(useOutputPins)
#if defined(useCarVoltageOutput)
	,pVoltageOffset
#endif // defined(useCarVoltageOutput)
#if defined(useDataLoggingOutput)
	,pSerialDataLoggingIdx
#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
	,pJSONoutputIdx
#endif // defined(useJSONoutput)
#if defined(useBarFuelEconVsTime)
	,pFEvsTimeIdx
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	,pBarLowSpeedCutoffIdx
	,pBarSpeedQuantumIdx
#endif // defined(useBarFuelEconVsSpeed)

// fuel injection settings

#if defined(useFuelPressure)
	,pSysFuelPressureIdx
#endif // defined(useFuelPressure)
#ifdef useCalculatedFuelFactor
	,pRefFuelPressureIdx
	,pInjectorCountIdx
	,pInjectorSizeIdx
#endif // useCalculatedFuelFactor
	,pMicroSecondsPerGallonIdx
	,pInjEdgeTriggerIdx
	,pInjectorOpeningTimeIdx
	,pInjectorClosingTimeIdx
	,pCrankRevPerInjIdx
	,pMinGoodRPMidx

// vehicle speed sensor settings

	,pPulsesPerDistanceIdx
	,pVSSpauseIdx
	,pMinGoodSpeedidx

// fuel tank size settings

	,pTankSizeIdx
	,pTankBingoSizeIdx

#if defined(useChryslerMAPCorrection)
// Chrysler MAP sensor settings

	,pMAPsensorFloorIdx
	,pMAPsensorCeilingIdx
	,pMAPsensorRangeIdx
	,pMAPsensorOffsetIdx
#if defined(useChryslerBaroSensor)
	,pBaroSensorFloorIdx
	,pBaroSensorCeilingIdx
	,pBaroSensorRangeIdx
	,pBaroSensorOffsetIdx
#else // defined(useChryslerBaroSensor)
	,pBarometricPressureIdx
#endif // defined(useChryslerBaroSensor)

#endif // defined(useChryslerMAPCorrection)
#if defined(useVehicleParameters)
// drag race / coastdown calc settings

	,pVehicleMassIdx
#if defined(useCoastDownCalculator)
	,pVehicleFrontalAreaIdx
	,pLocustDensityIdx
	,pCoefficientDidx
	,pCoefficientVidx
	,pCoefficientRRidx
	,pCoastdownSamplePeriodIdx
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	,pDragSpeedIdx
	,pDragDistanceIdx
	,pDragAutoFlagIdx
#endif // defined(useDragRaceFunction)

#endif // defined(useVehicleParameters)
// timeout settings

	,pIdleTimeoutIdx
	,pEOCtimeoutIdx
	,pButtonTimeoutIdx
	,pParkTimeoutIdx
	,pActivityTimeoutIdx
	,pWakeupResetCurrentOnEngineIdx
	,pWakeupResetCurrentOnMoveIdx
#if defined(useSavedTrips)
	,pAutoSaveActiveIdx
#endif // defined(useSavedTrips)

// miscellaneous settings

	,pScratchpadIdx
};

const uint8_t nesLoadInitial =			0;
const uint8_t nesLoadValue =			nesLoadInitial + 1;
const uint8_t nesOnChange =				nesLoadValue + 1;
const uint8_t nesSaveParameter =		nesOnChange + 1;

typedef struct
{

	uint8_t callingDisplayIdx; // remember what module called number editor
	uint8_t parameterIdx;
	const char * neStatusMessage;

} numberEditType;

static numberEditType numberEditObj;

namespace parameterEdit /* parameter editor/entry section prototype */
{

	static uint8_t sharedFunctionCall(uint8_t cmd);
	static uint8_t menuHandler(uint8_t cmd, uint8_t cursorPos);
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void findLeft(void);
	static void findRight(void);
	static void readInitial(void);
	static void readMinValue(void);
	static void readMaxValue(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void changeDigit(uint8_t dir);
	static void save(void);
	static void cancel(void);
	static uint8_t onEEPROMchange(const uint8_t * sched, uint8_t parameterIdx);

};

static const char numberEditSaveCanx[] PROGMEM = {
	" OK XX"
};

static const char pseStatusMessages[] PROGMEM = {
	"Param Unchanged" tcEOS
	"Param Changed" tcEOS
	"Param Reverted" tcEOS
};

static char pBuff[17]; // used by parameterEdit:: routines


