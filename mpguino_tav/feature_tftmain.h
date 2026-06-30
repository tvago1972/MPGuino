#if defined(useTFToutput) && !defined(useButtonInput)
namespace tftMain /* TFT primary-display main screen prototype */
{

	static void init(void);
	static void update(void);
#if defined(useTouchScreenInput)
	static void pollTouch(void);
#endif // defined(useTouchScreenInput)

};

// top-level TFT screen state - the current screen is data (like the LCD's
// workingDisplayIdx), so the main loop never blocks regardless of what's shown
static const uint8_t tftScreenMain =		0;	// live instrument dashboard
static const uint8_t tftScreenSettings =	1;	// settings editor (group menu / parameter list)
static const uint8_t tftScreenDropdown =	2;	// option dropdown (editing a boolean/enum parameter)
static uint8_t tftScreen;

#if defined(useTouchScreenInput)
// settings gear in the top-right corner: a sustained hold (not a tap) opens the
// settings editor, so it can't be triggered accidentally. a progress bar under the
// gear fills as the hold counts up.
static const uint16_t tftGearR =			10;			// gear half-size (footprint 2R x 2R px)
static const uint16_t tftGearMargin =		4;			// inset from the top-right corner
static const uint16_t tftGearHitSize =		28;			// touch target (top-right corner square); kept clear of the value field
static const uint32_t tftGearHoldCycles = ((uint32_t)(t0CyclesPerSecond) * 3) / 4;	// ~750ms hold (timer0 cycles) to confirm entry
static const uint16_t tftGearFG =			0x8410;		// idle gear (grey)
static const uint16_t tftGearActiveFG =		0x07FF;		// gear while held (cyan)
static const uint16_t tftGearProgressFG =	0x07E0;		// hold progress bar (green)
#endif // defined(useTouchScreenInput)

// the four functions of the first ("Instrument") LCD display page, reused here:
// instant speed, engine RPM (or battery voltage), fuel rate, fuel economy. these
// mirror mainDisplayPageFormats[0..3] in feature_base.h (which is button-gated).
static const uint16_t tftMainFormats[4] PROGMEM = {
	(instantIdx << 8) | (tSpeed),
#if defined(useAlternatorVoltage)
	(instantIdx << 8) | (tAlternatorChannel),
#else // defined(useAlternatorVoltage)
	(instantIdx << 8) | (tEngineSpeed),
#endif // defined(useAlternatorVoltage)
	(instantIdx << 8) | (tFuelRate),
	(instantIdx << 8) | (tFuelEcon),
};

static const char tftMainLabels[] PROGMEM = {
	"Speed" tcEOS
#if defined(useAlternatorVoltage)
	"Battery" tcEOS
#else // defined(useAlternatorVoltage)
	"RPM" tcEOS
#endif // defined(useAlternatorVoltage)
	"Fuel rate" tcEOS
	"Economy" tcEOS
};

// short measurement-unit labels for the TFT, indexed by mainCalcFuncVar.calcFmtIdx
// (so metric/SAE/alternate formats are tracked automatically). this MUST mirror the
// order and #if guards of calcFormatLabels in functions.h - the terminal labels there
// (e.g. "gallon/hour") are sized for an 80-column terminal, too wide for a quadrant.
static const char tftMainUnits[] PROGMEM = {
	"h:m:s" tcEOS					// time in H9mmSS format
	"ms" tcEOS						// time in milliseconds
	"RPM" tcEOS						// engine speed
	"pulse" tcEOS					// pulse count
	"edges" tcEOS					// pulse edge count
#if defined(useDragRaceFunction)
	"sec" tcEOS						// time in seconds
#endif // defined(useDragRaceFunction)
#if defined(useAnalogRead)
	"V" tcEOS						// voltage
#endif // defined(useAnalogRead)
#if defined(useFuelCost)
	"cost" tcEOS					// fuel cost
	"$/hr" tcEOS					// fuel rate cost
#endif // defined(useFuelCost)
	"gal" tcEOS						// gallons
	"L" tcEOS						// liters
	"gal/hr" tcEOS					// gallons per hour
	"L/hr" tcEOS					// liters per hour
	"mi" tcEOS						// miles traveled
	"km" tcEOS						// kilometers traveled
	"MPH" tcEOS						// miles per hour
	"kPH" tcEOS						// kilometers per hour
#if defined(useFuelCost)
	"$/mi" tcEOS					// fuel cost per mile
	"$/km" tcEOS					// fuel cost per kilometer
	"mi/$" tcEOS					// mile per unit fuel cost
	"km/$" tcEOS					// kilometer per unit fuel cost
#endif // defined(useFuelCost)
#if defined(useChryslerMAPCorrection)
	"psi" tcEOS						// psia
	"kPa" tcEOS						// kPa
#endif // defined(useChryslerMAPCorrection)
#if defined(useDragRaceFunction)
	"WHP" tcEOS						// horsepower
	"kW" tcEOS						// kilowatts
#endif // defined(useDragRaceFunction)
	"MPG" tcEOS						// miles per gallon
	"KPL" tcEOS						// km per liter
	"g/100mi" tcEOS					// gallons per 100 miles
	"L/100km" tcEOS					// liters per 100 km
};

static const uint8_t tftMainWindow =	7;			// value field width (chars) per quadrant

// RGB565 palette
static const uint16_t tftMainLabelFG =	0x07FF;		// cyan label
static const uint16_t tftMainValueFG =	0xFFFF;		// white value
static const uint16_t tftMainBG =		0x0000;		// black

#endif // defined(useTFToutput) && !defined(useButtonInput)
