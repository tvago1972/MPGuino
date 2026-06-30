#if defined(useTFToutput) && defined(useTouchScreenInput)
namespace tftSettings /* on-screen (touch) EEPROM settings editor prototype */
{

	static void enter(void);					// draw the group menu (called when the gear opens settings)
	static uint8_t tap(uint16_t px, uint16_t py);	// handle one tap; returns 1 to stay, 0 to exit to the main screen

};

// Grouped, touch-friendly view of this build's editable parameters. The parameter
// definitions in parameters.h remain the single source of truth; this is only a
// grouping/label table over the always-present (non button/LCD) parameters, edited
// through the shared value core (parameterEdit::sharedFunctionCall + the SWEET64
// fetch/max/store programs + EEPROM::onChange). Booleans/enums are edited with the
// keypad for now (their bit-width max bounds the input); a dropdown widget is TODO.

static const char tftSettingsGroupNames[] PROGMEM = {
	"Display" tcEOS
	"Fuel Injector" tcEOS
	"VSS" tcEOS
	"Tank" tcEOS
	"Timeouts" tcEOS
	"Misc" tcEOS
};

static const uint8_t tftSettingsParams[] PROGMEM = {
	pMetricModeIdx, pAlternateFEidx,																	// Display
	pMicroSecondsPerGallonIdx, pInjEdgeTriggerIdx, pInjectorSettleTimeIdx, pInjPer2CrankRevIdx, pMinGoodRPMidx,	// Fuel Injector
	pPulseEdgePerDistanceIdx, pVSSpauseIdx, pMinGoodSpeedidx,											// VSS
	pTankSizeIdx, pTankBingoSizeIdx,																	// Tank
	pIdleTimeoutIdx, pEOCtimeoutIdx, pButtonTimeoutIdx, pParkTimeoutIdx, pActivityTimeoutIdx,			// Timeouts
	pScratchpadIdx,																					// Misc
};

static const char tftSettingsLabels[] PROGMEM = {
	"Metric mode" tcEOS "Alt FE format" tcEOS
	"us per gallon" tcEOS "Inj edge" tcEOS "Inj delay us" tcEOS "Rev per inj" tcEOS "Min good RPM" tcEOS
	"Pulses/dist" tcEOS "VSS pause ms" tcEOS "Min speed *1k" tcEOS
	"Tank *1000" tcEOS "Bingo *1000" tcEOS
	"Idle T/O s" tcEOS "EOC T/O s" tcEOS "Button T/O s" tcEOS "Park T/O s" tcEOS "Off T/O s" tcEOS
	"Scratchpad" tcEOS
};

// group g spans tftSettingsParams[ start[g] .. start[g+1] )
static const uint8_t tftSettingsGroupCount = 6;
static const uint8_t tftSettingsGroupStart[tftSettingsGroupCount + 1] PROGMEM = { 0, 2, 7, 10, 12, 17, 18 };

// option-label lists for boolean/enum parameters (value i -> i-th tcEOS substring).
// tftSettingsChoices() maps a parameter to its list; a 0 return means "numeric".
static const char tftChoiceYesNo[] PROGMEM = { "No" tcEOS "Yes" tcEOS };
static const char tftChoiceInjTrigger[] PROGMEM = { "Sat Fall" tcEOS "Sat Rise" tcEOS "P&H Fall" tcEOS "P&H Rise" tcEOS };

// layout (px); list rows sit between the title bar and the footer button. the row
// height is orientation-dependent (taller, easier-to-tap rows in portrait, which
// has the spare vertical room) - see tftSettingsRowHeight().
static const uint16_t tftSettingsTitleH =	22;
static const uint16_t tftSettingsFooterH =	26;
#if defined(useTFTsleepBarEverywhere)
static const uint16_t tftSettingsBottomReserve = tftSleepBarH;	// bottom strip kept for the activity bar
#else // defined(useTFTsleepBarEverywhere)
static const uint16_t tftSettingsBottomReserve = 0;
#endif // defined(useTFTsleepBarEverywhere)
static const uint16_t tftSettingsRowHportrait =		44;
static const uint16_t tftSettingsRowHlandscape =	28;

// RGB565 palette
static const uint16_t tftSettingsTitleBG =	0x001F;		// blue
static const uint16_t tftSettingsRowFG =	0xFFFF;		// white label
static const uint16_t tftSettingsValFG =	0xFFE0;		// yellow value
static const uint16_t tftSettingsDivider =	0x4208;		// grey row divider / footer face
static const uint16_t tftSettingsSelBG =	0x001F;		// highlighted (current) dropdown option

#endif // defined(useTFToutput) && defined(useTouchScreenInput)
