uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
void doReturnToMainScreen(void);
#ifdef useScreenEditor // Programmable main display screen edit support section
void doCursorUpdateScreenEdit(void);
void doScreenEditDisplay(void);
void doGoScreenEdit(void);
void doScreenEditBump(void);
void doSaveScreen(void);
void doScreenReturnToMain(void);
#endif // useScreenEditor

uint8_t topScreenLevel;

const uint8_t displayPageCount = 9						// count of base number of data screens
#ifdef trackIdleEOCdata
	+ 3													// count of Idle/EOC tracking data screens
#endif // trackIdleEOCdata
;
const uint8_t mainScreenDisplayFormatSize = displayPageCount * 4;

const char mainScreenFuncNames[] PROGMEM = {
	"Instrument\r"
	"Custom\r"
	"Instant/Current\r"
	"Instant/Tank\r"
	"Current\r"
	"Tank\r"
#ifdef trackIdleEOCdata
	"EOC/Idle\r"
#endif // trackIdleEOCdata
	"Current Data\r"
	"Tank Data\r"
#ifdef trackIdleEOCdata
	"Current EOC/Idle\r"
	"Tank EOC/Idle\r"
#endif // trackIdleEOCdata
	"Remaining\r"
};

const uint8_t mainDisplayScreenFormats[(unsigned int)(mainScreenDisplayFormatSize)][2] PROGMEM = {
	 {lblInstantIdx,			tSpeed}					// Instrument
	,{lblInstantIdx,			tEngineSpeed}
	,{lblInstantIdx,			tFuelRate}
	,{lblInstantIdx,			tFuelEcon}

	,{lblInstantIdx,			tFuelEcon}				// Custom
	,{lblInstantIdx,			tSpeed}
	,{lblInstantIdx,			tFuelRate}
	,{lblCurrentIdx,			tFuelEcon}

	,{lblInstantIdx,			tFuelEcon}				// Instant / Current
	,{lblInstantIdx,			tSpeed}
	,{lblCurrentIdx,			tFuelEcon}
	,{lblCurrentIdx,			tDistance}

	,{lblInstantIdx,			tFuelEcon}				// Instant / Tank
	,{lblInstantIdx,			tSpeed}
	,{lblTankIdx,				tFuelEcon}
	,{lblTankIdx,				tDistance}

	,{lblCurrentIdx,			tSpeed}					// Current
	,{lblCurrentIdx,			tFuelEcon}
	,{lblCurrentIdx,			tDistance}
	,{lblCurrentIdx,			tFuelUsed}

	,{lblTankIdx,				tSpeed}					// Tank
	,{lblTankIdx,				tFuelEcon}
	,{lblTankIdx,				tDistance}
	,{lblTankIdx,				tFuelUsed}

#ifdef trackIdleEOCdata
	,{lblEOCidleCurrentIdx,		tDistance}				// EOC / Idle
	,{lblEOCidleCurrentIdx,		tFuelUsed}
	,{lblEOCidleTankIdx,		tDistance}
	,{lblEOCidleTankIdx,		tFuelUsed}

#endif // trackIdleEOCdata
	,{lblCurrentIdx,			tEngineRunTime}			// Current data
	,{lblCurrentIdx,			tFuelUsed}
	,{lblCurrentIdx,			tMotionTime}
	,{lblCurrentIdx,			tDistance}

	,{lblTankIdx,				tEngineRunTime}			// Tank data
	,{lblTankIdx,				tFuelUsed}
	,{lblTankIdx,				tMotionTime}
	,{lblTankIdx,				tDistance}

#ifdef trackIdleEOCdata
	,{lblEOCidleCurrentIdx,		tEngineRunTime}			// Current EOC / Idle
	,{lblEOCidleCurrentIdx,		tFuelUsed}
	,{lblEOCidleCurrentIdx,		tMotionTime}
	,{lblEOCidleCurrentIdx,		tDistance}

	,{lblEOCidleTankIdx,		tEngineRunTime}			// Tank EOC / Idle
	,{lblEOCidleTankIdx,		tFuelUsed}
	,{lblEOCidleTankIdx,		tMotionTime}
	,{lblEOCidleTankIdx,		tDistance}

#endif // trackIdleEOCdata
	,{lblTankIdx,				tFuelUsed}				// Remaining
	,{lblTankIdx,				tRemainingFuel}
	,{lblTankIdx,				tTimeToEmpty}
	,{lblTankIdx,				tDistanceToEmpty}
};

#ifdef trackIdleEOCdata
const uint8_t mainEOCscreenFormats[(unsigned int)(4)][2] PROGMEM = {
	 {lblInstantIdx,			tSpeed}
	,{lblCurrentIdx,			tFuelEcon}
	,{lblEOCidleCurrentIdx,		tDistance}
	,{lblEOCidleTankIdx,		tDistance}
};

const uint8_t mainIdleScreenFormats[(unsigned int)(4)][2] PROGMEM = {
	 {lblInstantIdx,			tFuelRate}
	,{lblCurrentIdx,			tFuelEcon}
	,{lblEOCidleCurrentIdx,		tFuelUsed}
	,{lblEOCidleTankIdx,		tFuelUsed}
};

#endif // trackIdleEOCdata
#if defined(useScreenEditor)
/* Programmable main display screen edit support section */

uint8_t screenTripValue = 0;
uint8_t screenFunctionValue = 0;
uint8_t screenEditDirty = 0;

const uint8_t screenEditFlag_dirty =	0x80;

const char seFormatRevertedString[] PROGMEM = "Format reverted";
const char seExitScreenEditString[] PROGMEM = "Screen Display";

#endif // defined(useScreenEditor)