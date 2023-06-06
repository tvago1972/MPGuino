//
// MPGuino - open source fuel consumption tracking system
// GPL Software, mass production use rights reserved by opengauge.org
// personal use is perfectly fine
// no warranties expressed or implied
//
// This version of MPGuino is only configured for use with saturation-type fuel injectors. It does not
// support peak and hold fuel injectors, due to the relative complexity of having to add many more
// electrical components to properly detect the peak and hold currents for two separate types of
// peak and hold drivers (analog current and PWM). Look up "jaycar peak hold adapter" on the intertubes
// if you want more information

// Special thanks to the good folks at ecomodder.com, ardunio.cc, avrfreaks.net, cadsoft.de, atmel.com,
// and all the folks who donate their time and resources and share their experiences freely

/* External connections:

Legacy MPGuino hardware is defined as anything that MPGuino was originally designed to run on. This includes
   Arduino Duemilanove, Arduino Uno, JellyBeanDriver board, meelis11 board, iDuino, and any other board based off the
   original MPGuino schematic, that can be found at http://ecomodder.com/wiki/index.php/MPGuino

Vehicle interface pins
  legacy MPGuino hardware
    injector sense open  PD2 (INT0), Digital 0
    injector sense close PD3 (INT1), Digital 1
    speed                PC0 (PCINT8), A0
    (if configured) MAP  PC1 (ADC1), A1
    (if configured) Baro PC2 (ADC2), A2
    (if configured) Valt PC2 (ADC2), A2

  TinkerKit! LCD module
    injector sense open  PD2 (INT2), Digital 0 (note this makes it impossible for the TinkerKit! LCD module to use Serial 1)
    injector sense close PD3 (INT3), Digital 1
    speed                PB7 (PCINT7), Digital 11
    (if configured) MAP  PF5 (ADC5), A2
    (if configured) Baro PF6 (ADC6), A1
    (if configured) Valt PF6 (ADC2), A2

  Arduino Mega 2560
    injector sense open  PE4 (INT4), Digital 2
    injector sense close PE5 (INT5), Digital 3
    speed                PK0 (PCINT16), A8
    (if configured) MAP  PF1 (ADC1), A2
    (if configured) Baro PF2 (ADC2), A3
    (if configured) Valt PF2 (ADC2), A2

-------------------------------------
configuration for alternator voltage input to MPGuino (via meelis11)

                     o------------|<|----------------o V(alternator)
                     |            1N4004
                     |
                     |
                     |  R6      R5
                     o--VVV--o--VVV------------------o ground
                        27k  |  9.6k
                             |
                             |                         R6 (JBD)
                             o-----------------------o PC2 - legacy MPGuino hardware
                                                       PF2 - Arduino Mega 2560
                                                       PF6 - TinkerKit! LCD module

-------------------------------------
sensor configuration for Chrysler MAP/baro sensor

          --------------------------------------------
         |                 MAP SENSOR                 |
         |                                            |
         |   +5V             SIGNAL          MAP      |
         |   SUPPLY          GROUND          SIGNAL   |
         |     3               2               1      |
          --------------------------------------------
               o               o               o
               |               |               |                       R7 (JBD)
               |               |               o----------------vvv--o PC1 - legacy MPGuino hardware
               |               |               |                2.2k   PF1 - Arduino Mega 2560
               |               |               |                       PF5 - TinkerKit! LCD module
               o               o               o
          --------------------------------------------
         |   C1-27           C2-27           C2-23    |
         |   +5V             SIGNAL          MAP      |
         |   SUPPLY          GROUND          SIGNAL   |
         |                                            |
         |                  CHRYSLER                  |
         |       NGC POWERTRAIN CONTROL MODULE        |
          --------------------------------------------
                 (older Chrysler PCMs similar)



          --------------------------------------------
         |          Baro SENSOR (optional)            |
         |         (use a spare MAP sensor)           |
         |                                            |
         |   +5V             SIGNAL          Baro     |
         |   SUPPLY          GROUND          SIGNAL   |
         |     3               2               1      |
          --------------------------------------------
               o               o               o
               |               |               |                       R6 (JBD)
               |               |               o----------------vvv--o PC2 - legacy MPGuino hardware
               |               |                                2.2k   PF2 - Arduino Mega 2560
               |               |                                       PF6 - TinkerKit! LCD module
               o               o
          --------------------------------------------
         |   C1-27           C2-27                    |
         |   +5V             SIGNAL                   |
         |   SUPPLY          GROUND                   |
         |                                            |
         |                  CHRYSLER                  |
         |       NGC POWERTRAIN CONTROL MODULE        |
          --------------------------------------------
                 (older Chrysler PCMs similar)

-------------------------------------

LCD Pins - Legacy
  legacy MPGuino hardware
    DIR        PD4, Digital 4
    DB4        PD7, Digital 7
    DB5        PB0, Digital 8
    DB6        PB4, Digital 12
    DB7        PB5, Digital 13
    Enable     PD5, Digital 5
    Contrast   PD6, Digital 6, controlled by PWM on OC0A
    Brightness PB1, Digital 9, controlled by PWM on OC1A

  TinkerKit! LCD module
    RW         PF0, A5
    DIR        PF1, A4
    DB4        PF4, A3
    DB5        PD4, Digital 4
    DB6        PD6, Digital 12
    DB7        PB4, Digital 8
    Enable     PE6, Digital 7
    Contrast   PB5, Digital 9, controlled by PWM on OC1A
    Brightness PB6, Digital 10, controlled by PWM on OC1B

  Arduino Mega 2560
    DIR        PA4, Digital 26
    DB4        PA3, Digital 25
    DB5        PA2, Digital 24
    DB6        PA1, Digital 23
    DB7        PA0, Digital 22
    Enable     PA5, Digital 27
    Contrast   PB7, Digital 13, controlled by PWM on OC0A
    Brightness PB5, Digital 11, controlled by PWM on OC1A

LCD Pins - Parallax Serial Interface
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1

  Arduino Mega 2560
    RX      E1 (TXD0), Digital 1
    - or -
    RX      D3 (TXD1), Digital 18

LCD Pins - Adafruit RGB LCD Shield
  legacy MPGuino hardware
    SDA     PC4 (SDA), A4
    SCL     PC5 (SCL), A5

  TinkerKit! LCD module
    SDA     PD1 (SDA), Digital 2
    SCL     PD0 (SCL), Digital 3

  Arduino Mega 2560
    SCL     PD0 (SCL), Digital 21
    SDA     PD1 (SDA), Digital 20

-------------------------------------

Buttons - Legacy
* Physical layout

    left   middle   right

  legacy MPGuino hardware
    left    PC3 (PCINT11), A3
    middle  PC4 (PCINT12), A4
    right   PC5 (PCINT13), A5

  Arduino Mega 2560
    left    PK3 (PCINT19), A11
    middle  PK4 (PCINT20), A12
    right   PK5 (PCINT21), A13

-------------------------------------

Buttons - Multiplexed Analog (diagram courtesy of josemapiro)
* physical layout

            extra#1

    left	middle	right

            extra#2

  legacy MPGuino hardware
    left, middle, right, extra#1, extra#2 PC3 (ADC3), A3

  TinkerKit! LCD module
    left, middle, right, extra#1, extra#2 PF7 (ADC7), A0

  Arduino Mega 2560
    left, middle, right, extra#1, extra#2 PF3 (ADC3), A3


             o---------------o---------------o---------------o---------------o--o GND
        R2   |          R3   |          R4   |          R5   |          R6   |
     o--vvv--o       o--VVV--o       o--vvv--o       o--vvv--o       o--vvv--o
     |  2.2k         |  4.7k         |  10k          |  22k          |  47k
     o               o               o               o               o
      /               /               /               /               /
     o left          o middle        o right         o Extra#1       o Extra#2
     |               |               |               |               |
     o---------------o---------------o---------------o---------------o--vvv--o--o 5V
                                                                     | R1 1k
                                                                     o----------o PC3 - legacy MPGuino hardware
                                                                                  PF3 - Arduino Mega 2560
                                                                                  PF7 - TinkerKit! LCD module

Buttons - Parallax 5-position switch (diagram based on josemapiro efforts)
          (or any 5-position switch module with 10k pullup resistors on their switches)
  legacy MPGuino hardware
    left, middle, right, extra#1, extra#2 PC3 (ADC3), A3

  TinkerKit! LCD module
    left, middle, right, extra#1, extra#2 PF7 (ADC7), A0

  Arduino Mega 2560
    left, middle, right, extra#1, extra#2 PF3 (ADC3), A3

                                                                                  PF3 - Arduino Mega 2560
                                                                                  PF7 - TinkerKit! LCD module
                                                                     o----------o PC3 - legacy MPGuino hardware
                                                                     | R1 1k
     o---------------o---------------o---------------o---------------o--vvv--o--o 5V
     |               |               |               |               |
     |  left         | middle        |  right        | Extra#1       | Extra#2
     |               |               |               |               |
     |  R2           |  R3           |  R4           |  R5           |  R6
     o--vvv--o       o--VVV--o       o--vvv--o       o--vvv--o       o--vvv--o
        2.2k |          4.7k |          10k  |          22k  |          47k  |
             |               |               |               |               |
             |               |               |               |               |
             o               o               o               o               o
          -----------------------------------------------------------------------
         |   4               6               2               7               3   |
         |   LT              CTR             RT              UP              DN  |
         |                                                                       |
         |                  VCC             GND                                  |
         |                   5               8                                   |
          -----------------------------------------------------------------------
                             o               o
                             |               |
                             |               |
                             o               o----------------------------------O GND
                            N/C

Buttons - Adafruit RGB LCD Shield
  legacy MPGuino hardware
    SCL     PC5 (SCL), A5
    SDA     PC4 (SDA), A4

  TinkerKit! LCD module
    SDA     PD1 (SDA), Digital 2
    SCL     PD0 (SCL), Digital 3

  Arduino Mega 2560
    SCL     PD0 (SCL), Digital 21
    SDA     PD1 (SDA), Digital 20

-------------------------------------

Expansion outputs
  legacy MPGuino hardware
    EXP1       PB2, Digital 10, controlled by PWM on OC1B
    EXP2       PB3, Digital 11, controlled by PWM on OC2A

  TinkerKit! LCD module
    EXP1       PD7, Digital 6, controlled by PWM on OC4D
    EXP2       PC6, Digital 5, controlled by PWM on OC4A

  Arduino Mega 2560
    EXP1       PB6, Digital 12, controlled by PWM on OC1B
    EXP2       PB4, Digital 10, controlled by PWM on OC2A

-------------------------------------

Logging outputs
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1		(USART)

  Arduino Mega 2560
    RX      E1 (TXD0), Digital 1	(USART0)
    - or -
    RX      D3 (TXD1), Digital 18	(USART1)

  TinkerKit! LCD module
    D-								(USB)
    D+

 Program overview
 set up timer hardware
 set up interrupts
 set up system constants
 load system settings from EEPROM
 set up LCD hardware
 (if configured) set up serial UART output

 create accumulators for raw speed/injector data

 mainloop{
 update instantaneous trip, current trip, tank trip, any other setup trip accumulators with raw data accumulators
 reset raw data accumulators
 (if configured) transmit instantaneous trip accumulators
 display computations
 (if configured) transmit outgoing bluetooth computations
 scan for key presses and perform their function (change screen, reset a trip, goto setup, edit screen, restore trips, etc)
 (if configured) scan for incoming bluetooth commands and perform their function
 pause for remainder of 1/2 second
 }

*/

// this section marks the start of the program

// If AVR command-line tools are used to compile and upload this code, use these fuse settings:
// lfuse:0xf7 hfuse:0xd9 efuse:0x07
// (via Ettore_M)
//

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "configs.h"
#include "heart.h"
#include "parameters.h"
#include "trip_measurement.h"
#include "sweet64.h"
#include "m_serial.h"
#include "m_analog.h"
#include "m_twi.h"
#include "m_usb.h"
#include "m_input.h"
#include "m_lcd.h"
#include "functions.h"
#include "feature_base.h"
#include "feature_settings.h"
#include "feature_datalogging.h"
#include "feature_bignum.h"
#include "feature_bargraph.h"
#include "feature_outputpin.h"
#include "feature_debug.h"
#include "feature_dragrace.h"
#include "feature_coastdown.h"

static const char titleMPGuino[] PROGMEM = {
	"\xED" "MPGuino v1.95tav\r"
};

static const char dateMPGuino[] PROGMEM = {
	"2023-MAR-18\r"
};

typedef uint8_t (* displayHandlerFunc)(uint8_t, uint8_t, uint8_t); // type for various handler functions

typedef struct
{

	uint8_t buttonCode;
	void (* buttonCommand)(void);

} buttonVariable;

typedef struct
{

	uint8_t menuIndex;
	uint8_t modeIndex;
	uint8_t modeYcount;
	uint8_t modeXcount;
	displayHandlerFunc screenDisplayHandler;
	const buttonVariable (* modeButtonList);

} modeInformation;

const char * findStr(const char * str, uint8_t strIdx);
static void printStatusMessage(const char * s);
static void printStatusMessage(const char * s, uint8_t strIdx);
void initStatusLine(void);
void execStatusLine(void);
static void idleProcess(void); // place all time critical main program internal functionality here - no I/O!
int main(void);

namespace menu /* Top-down menu selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void select(void);
	static void doNextBright(void);
	static void noSupport(void);
	static void doNothing(void);

};

namespace cursor /* LCD screen cursor manipulation section prototype */
{

	static void screenLevelEntry(const char * str, uint8_t newScreenLevel);
	static void screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel);
	static void moveAbsolute(uint8_t positionY, uint8_t positionX);
	static void moveRelative(uint8_t moveY, uint8_t moveX);
	static void updateAfterMove(uint8_t levelChangeFlag);
	static void shortLeft(void);
	static void shortRight(void);
	static void longLeft(void);
	static void longRight(void);

};

uint8_t menuLevel;
uint8_t brightnessIdx;

volatile uint8_t cursorXdirection;
volatile uint8_t menuTop;

const uint8_t menuExitIdx =				0;								// this call may not even be necessary
const uint8_t menuEntryIdx =			menuExitIdx + 1;				// typically, this call will fall through
const uint8_t menuCursorUpdateIdx =		menuEntryIdx + 1;				// ...to this call, then will fall through
const uint8_t menuOutputDisplayIdx =	menuCursorUpdateIdx + 1;		// ...to this call

namespace text /* text string output section prototype */
{

	static void gotoXY(interfaceDevice &dev, uint8_t x, uint8_t y);
	static uint8_t charOut(interfaceDevice &dev, uint8_t chr);

	static void setModeOnCondition(interfaceDevice &dev, uint8_t condition, uint8_t odvFlag);
	static void stringOut(interfaceDevice &dev, char * str);
	static void stringOut(interfaceDevice &dev, const char * str);
	static void stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx);
	static void newLine(interfaceDevice &dev);
	static void hexNybbleOut(interfaceDevice &dev, uint8_t val);
	static void hexByteOut(interfaceDevice &dev, uint8_t val);
	static void hexWordOut(interfaceDevice &dev, uint16_t val);
	static void hexDWordOut(interfaceDevice &dev, uint32_t val);
	static void hexLWordOut(interfaceDevice &dev, uint64_t * val);
	static uint8_t numberOut(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag);

};

// Menu display / screen cursor support section

// the following screen index defines are for the legacy MPGuino screen displays
#define nextAllowedValue 0
static const uint8_t mainScreenIdx =				nextAllowedValue;
#define nextAllowedValue mainScreenIdx + 1
#ifdef useBigFE
static const uint8_t bigFEscreenIdx =				nextAllowedValue;
#define nextAllowedValue bigFEscreenIdx + 1
#endif // useBigFE
#ifdef useBarFuelEconVsTime
static const uint8_t barFEvTscreenIdx =				nextAllowedValue;
#define nextAllowedValue barFEvTscreenIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t barFEvSscreenIdx =				nextAllowedValue;
#define nextAllowedValue barFEvSscreenIdx + 1
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
static const uint8_t bigDTEscreenIdx =				nextAllowedValue;
#define nextAllowedValue bigDTEscreenIdx + 1
#endif // useBigDTE
#ifdef useBigTTE
static const uint8_t bigTTEscreenIdx =				nextAllowedValue;
#define nextAllowedValue bigTTEscreenIdx + 1
#endif // useBigTTE
#ifdef useCPUreading
static const uint8_t CPUmonScreenIdx =				nextAllowedValue;
#define nextAllowedValue CPUmonScreenIdx + 1
#endif // useCPUreading
#ifdef useClockDisplay
static const uint8_t clockShowScreenIdx =			nextAllowedValue;
#define nextAllowedValue clockShowScreenIdx + 1
#endif // useClockDisplay

static const uint8_t mainScreenSize =				nextAllowedValue;	// this variable is used to figure out how many menu levels the main screen has

// the following screen index defines are for the parameter setting screen, and any other extra features that have been added
static const uint8_t settingScreenIdx =				nextAllowedValue;
static const uint8_t fuelSettingsScreenIdx =		settingScreenIdx + 1;
static const uint8_t VSSsettingsScreenIdx =			fuelSettingsScreenIdx + 1;
static const uint8_t tankSettingsScreenIdx =		VSSsettingsScreenIdx + 1;
#define nextAllowedValue tankSettingsScreenIdx + 1
#ifdef useChryslerMAPCorrection
static const uint8_t CRFICsettingsScreenIdx =		nextAllowedValue;
#define nextAllowedValue CRFICsettingsScreenIdx + 1
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
static const uint8_t acdSettingsScreenIdx =			nextAllowedValue;
#define nextAllowedValue acdSettingsScreenIdx + 1
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
static const uint8_t timeoutSettingsScreenIdx =		nextAllowedValue;
static const uint8_t miscSettingsScreenIdx =		timeoutSettingsScreenIdx + 1;
#define nextAllowedValue miscSettingsScreenIdx + 1
#ifdef useDragRaceFunction
static const uint8_t dragRaceIdx =					nextAllowedValue;
#define nextAllowedValue dragRaceIdx + 1
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const uint8_t coastdownIdx =					nextAllowedValue;
#define nextAllowedValue coastdownIdx + 1
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
static const uint8_t debugReadingIdx =				nextAllowedValue;
#define nextAllowedValue debugReadingIdx + 1
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
static const uint8_t pressureCorrectionIdx =		nextAllowedValue;
#define nextAllowedValue pressureCorrectionIdx + 1
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
static const uint8_t analogReadIdx =				nextAllowedValue;
#define nextAllowedValue analogReadIdx + 1
#endif // useDebugAnalog
#ifdef useTestButtonValues
static const uint8_t buttonViewIdx =				nextAllowedValue;
#define nextAllowedValue buttonViewIdx + 1
#endif // useTestButtonValues

static const char menuVisibleSize =					nextAllowedValue;	// this variable is used to figure out how many total menu levels are visible

// the following screen index defines do not show up in the top-down menu list
const uint8_t menuScreenIdx =						nextAllowedValue;
const uint8_t parameterEditScreenIdx =				menuScreenIdx + 1;
#define nextAllowedValue parameterEditScreenIdx + 1
#ifdef useClockDisplay
const uint8_t clockSetScreenIdx =					nextAllowedValue;
#define nextAllowedValue clockSetScreenIdx + 1
#endif // useClockDisplay
#ifdef useSavedTrips
static const uint8_t tripSaveScreenIdx =			nextAllowedValue;
#define nextAllowedValue tripSaveScreenIdx + 1
#endif // useSavedTrips
#ifdef usePartialRefuel
static const uint8_t partialRefuelScreenIdx =		nextAllowedValue;
#define nextAllowedValue partialRefuelScreenIdx + 1
#endif // usePartialRefuel
#ifdef useScreenEditor
const uint8_t screenEditIdx =						nextAllowedValue;
#define nextAllowedValue screenEditIdx + 1
#endif // useScreenEditor

static const char menuTotalSize =					nextAllowedValue;

static const buttonVariable bpListMenu[] PROGMEM = {
	 {btnShortPressC,	menu::select}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::shortRight}
		,{btnShortPressU,	cursor::shortLeft}
		,{btnLongPressU,	menu::doNextBright}
		,{btnShortPressL,	menu::doNothing}
		,{btnShortPressR,	menu::select}
#else // useButtonCrossConfig
		,{btnShortPressR,	cursor::shortRight}
		,{btnShortPressL,	cursor::shortLeft}
		,{btnLongPressC,	menu::doNextBright}
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

static const buttonVariable bpListMain[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useSavedTrips
		,{btnShortPressDR,	tripSupport::goSaveCurrent}
		,{btnShortPressDL,	tripSupport::goSaveTank}
	#endif // useSavedTrips
	#ifdef useScreenEditor
		,{btnLongPressC,	doGoScreenEdit}
	#endif // useScreenEditor
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useScreenEditor
		,{btnLongPressC,	doGoScreenEdit}
	#endif // useScreenEditor
	#ifdef useCPUreading
		,{btnLongPressLCR,	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

static const buttonVariable bpListSetting[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::entry}
	,{btnShortPressLR,	parameterEdit::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnShortPressD,	cursor::shortRight}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp, 		menu::noSupport}
};

static const buttonVariable bpListParameterEdit[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	parameterEdit::cancel}
	,{btnLongPressLR,	parameterEdit::cancel}
	,{btnLongPressC,	parameterEdit::save}
	,{btnShortPressLR,	parameterEdit::save}
	,{btnLongPressR,	parameterEdit::readInitial}
#ifdef useButtonCrossConfig
		,{btnShortPressUL,	parameterEdit::findLeft}
		,{btnShortPressUR,	parameterEdit::findRight}
		,{btnLongPressD,	parameterEdit::readMinValue}
		,{btnLongPressU,	parameterEdit::readMaxValue}
		,{btnShortPressU,	parameterEdit::changeDigitUp}
		,{btnShortPressD,	parameterEdit::changeDigitDown}
		,{btnShortPressC,	parameterEdit::save}
#else // useButtonCrossConfig
		,{btnShortPressLC,	parameterEdit::findLeft}
		,{btnShortPressCR,	parameterEdit::findRight}
		,{btnLongPressCR,	parameterEdit::readMinValue}
		,{btnLongPressLC,	parameterEdit::readMaxValue}
		,{btnShortPressC,	parameterEdit::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#ifdef useBigDigitDisplay
static const buttonVariable bpListBigNum[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // useBigDigitDisplay
#ifdef useClockDisplay
static const buttonVariable bpListClockShow[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
	,{btnLongPressC,	clockSet::entry}
	,{btnShortPressLR,	clockSet::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

static const buttonVariable bpListClockSet[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	clockSet::cancel}
	,{btnLongPressLR,	clockSet::cancel}
	,{btnLongPressC,	clockSet::set}
	,{btnShortPressLR,	clockSet::set}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	clockSet::changeDigitUp}
		,{btnShortPressD,	clockSet::changeDigitDown}
		,{btnShortPressC,	clockSet::set}
#else // useButtonCrossConfig
		,{btnShortPressC,	clockSet::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // useClockDisplay
#ifdef useCPUreading
static const buttonVariable bpListCPUmonitor[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // useCPUreading
#ifdef usePartialRefuel
static const buttonVariable bpListPartialRefuel[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	doReturnToMainScreen}
	,{btnLongPressLR,	doReturnToMainScreen}
	,{btnLongPressC,	partialRefuel::longSelect}
	,{btnShortPressLR,	partialRefuel::select}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	partialRefuel::select}
		,{btnLongPressU,	partialRefuel::longSelect}
		,{btnShortPressD,	cursor::shortRight}
		,{btnShortPressC,	doReturnToMainScreen}
#else // useButtonCrossConfig
		,{btnShortPressC,	partialRefuel::select}
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // usePartialRefuel
#if defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog)
static const buttonVariable bpListMiscViewer[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	menu::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog)
#ifdef useTestButtonValues
static const buttonVariable bpListButtonView[] PROGMEM = {
	 {buttonsUp,		menu::doNothing}
};

#endif // useTestButtonValues
#ifdef useSavedTrips
static const buttonVariable bpListTripSave[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressL,	doReturnToMainScreen}
	,{btnLongPressLR,	doReturnToMainScreen}
	,{btnShortPressC,	doReturnToMainScreen}
#ifdef useButtonCrossConfig
		,{btnLongPressU,	tripSupport::select}
		,{btnShortPressD,	cursor::shortRight}
#else // useButtonCrossConfig
		,{btnLongPressC,	tripSupport::select}
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // useSavedTrips
#ifdef useBarGraph
static const buttonVariable bpListBarGraph[] PROGMEM = {
	 {btnShortPressR,	cursor::shortRight}
	,{btnShortPressL,	cursor::shortLeft}
	,{btnLongPressR,	cursor::longRight}
	,{btnLongPressL,	cursor::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::longRight}
		,{btnShortPressU,	menu::doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useBarFuelEconVsSpeed
		,{btnLongPressUC,	bgFEvsSsupport::reset}
	#endif // useBarFuelEconVsSpeed
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	menu::doNextBright}
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useBarFuelEconVsSpeed
		,{btnLongPressLCR,	bgFEvsSsupport::reset}
	#endif // useBarFuelEconVsSpeed
	#ifdef useCPUreading
		,{btnLongPressLR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		menu::noSupport}
};

#endif // useBarGraph
#ifdef useButtonCrossConfig
#ifdef useScreenEditor
static const buttonVariable bpListScreenEdit[] PROGMEM = {
	{btnShortPressU,	doScreenEditBump}
	,{btnShortPressD,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressU,	doSaveScreen}
	,{buttonsUp,		menu::noSupport}
};

#endif // useScreenEditor
#ifdef useDragRaceFunction
static const buttonVariable bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
	,{buttonsUp,		menu::noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonVariable bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
	,{buttonsUp,		menu::noSupport}
};

#endif // useCoastDownCalculator
#else // useButtonCrossConfig
#ifdef useScreenEditor
static const buttonVariable bpListScreenEdit[] PROGMEM = {
	{btnShortPressC,	doScreenEditBump}
	,{btnLongPressLR,	doCursorUpdateScreenEdit}	// revert screen format value
	,{btnLongPressC,	doSaveScreen}
	,{buttonsUp,		menu::noSupport}
};

#endif // useScreenEditor
#ifdef useDragRaceFunction
static const buttonVariable bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
	,{buttonsUp,		menu::noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonVariable bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
	,{buttonsUp,		menu::noSupport}
};

#endif // useCoastDownCalculator
#endif //useButtonCrossConfig
static const char menuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
	"Main Display\r"
#ifdef useBigFE
	"Big FuelEcon\r"
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	"FE/Time\r"
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"FE/Speed\r"
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	"Big DistToE\r"
#endif // useBigDTE
#ifdef useBigTTE
	"Big TimeToE\r"
#endif // useBigTTE
#ifdef useCPUreading
	"CPU Info\r"
#endif // useCPUreading
#ifdef useClockDisplay
	"Clock\r"
#endif // useClockDisplay
	"Settings Disp\r"
	"Settings Inj\r"
	"Settings VSS\r"
	"SettingsTankQty\r"
#ifdef useChryslerMAPCorrection
	"SettingsChryslr\r"
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	"SettingsVehTest\r"
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	"SettingsTimeout\r"
	"Settings Misc\r"
#ifdef useDragRaceFunction
	"Accel Test\r"
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	"Coastdown\r"
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	"Sensor Sim\r"
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	"Pressures\r"
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	"ADC readings\r"
#endif // useDebugAnalog
#ifdef useTestButtonValues
	"Button Values\r"
#endif // useTestButtonValues
};

uint8_t screenCursor[(unsigned int)(menuTotalSize)];

static const modeInformation screenParameters[(unsigned int)(menuTotalSize)] PROGMEM = {

// the following screen entries are in the top-down menu list

	{mainScreenIdx,					mainScreenIdx,						mainScreenSize,		displayPageCount,			mainScreenDisplayHandler,			bpListMain}
#ifdef useBigFE
	,{bigFEscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							bigDigit::displayHandler,			bpListBigNum}
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,{barFEvTscreenIdx,				mainScreenIdx,						mainScreenSize,		4,							barGraphSupport::displayHandler,	bpListBarGraph}
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,{barFEvSscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							barGraphSupport::displayHandler,	bpListBarGraph}
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,{bigDTEscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							bigDigit::displayHandler,			bpListBigNum}
#endif // useBigDTE
#ifdef useBigTTE
	,{bigTTEscreenIdx,				mainScreenIdx,						mainScreenSize,		3,							bigDigit::displayHandler,			bpListBigNum}
#endif // useBigTTE
#ifdef useCPUreading
	,{CPUmonScreenIdx,				mainScreenIdx | 0x80,				mainScreenSize,		1,							systemInfo::displayHandler,			bpListCPUmonitor}
#endif // useCPUreading
#ifdef useClockDisplay
	,{clockShowScreenIdx,			mainScreenIdx | 0x80,				mainScreenSize,		1,							bigDigit::displayHandler,			bpListClockShow}
#endif // useClockDisplay
	,{settingScreenIdx,				settingScreenIdx | 0x80,			1,					eePtrSettingsDispLen,		settings::displayHandler,			bpListSetting}
	,{fuelSettingsScreenIdx,		fuelSettingsScreenIdx | 0x80,		1,					eePtrSettingsInjLen,		settings::displayHandler,			bpListSetting}
	,{VSSsettingsScreenIdx,			VSSsettingsScreenIdx | 0x80,		1,					eePtrSettingsVSSlen,		settings::displayHandler,			bpListSetting}
	,{tankSettingsScreenIdx,		tankSettingsScreenIdx | 0x80,		1,					eePtrSettingsTankLen,		settings::displayHandler,			bpListSetting}
#ifdef useChryslerMAPCorrection
	,{CRFICsettingsScreenIdx,		CRFICsettingsScreenIdx | 0x80,		1,					eePtrSettingsCRFIClen,		settings::displayHandler,			bpListSetting}
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,{acdSettingsScreenIdx,			acdSettingsScreenIdx | 0x80,		1,					eePtrSettingsACDlen,		settings::displayHandler,			bpListSetting}
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,{timeoutSettingsScreenIdx,		timeoutSettingsScreenIdx | 0x80,	1,					eePtrSettingsTimeoutLen,	settings::displayHandler,			bpListSetting}
	,{miscSettingsScreenIdx,		miscSettingsScreenIdx | 0x80,		1,					eePtrSettingsMiscLen,		settings::displayHandler,			bpListSetting}
#ifdef useDragRaceFunction
	,{dragRaceIdx,	1,	4,	accelerationTest::goDisplay,	menu::doNothing,	bpListDragRace}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,{coastdownIdx | 0x80,	1,	3,	coastdown::goDisplay,	menu::doNothing,	bpListCoastdown}
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,{debugReadingIdx,				debugReadingIdx | 0x80,				1,					4,							debugReading::displayHandler,		bpListMiscViewer}
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	,{pressureCorrectionIdx,		pressureCorrectionIdx | 0x80,		1,					1,							pressureCorrect::displayHandler,	bpListMiscViewer}
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	,{analogReadIdx,				analogReadIdx | 0x80,				1,					1,							analogReadViewer::displayHandler,	bpListMiscViewer}
#endif // useDebugAnalog
#ifdef useTestButtonValues
	,{buttonViewIdx,				buttonViewIdx | 0x80,				1,					1,							buttonView::displayHandler,			bpListButtonView}
#endif // useTestButtonValues

// the following screen entries do not show up in the top-down menu list

	,{0,							menuScreenIdx | 0x80,				1,					menuVisibleSize,			menu::displayHandler,				bpListMenu}
	,{0,							parameterEditScreenIdx | 0x80,		1,					12,							parameterEdit::displayHandler,		bpListParameterEdit}
#ifdef useClockDisplay
	,{0,							clockSetScreenIdx | 0x80,			1,					4,							bigDigit::displayHandler,			bpListClockSet}
#endif // useClockDisplay
#ifdef useSavedTrips
	,{0,							tripSaveScreenIdx,					1,					3,							tripSupport::displayHandler,		bpListTripSave}
#endif // useSavedTrips
#ifdef usePartialRefuel
	,{0,							partialRefuelScreenIdx | 0x80,		1,					3,							partialRefuel::displayHandler,		bpListPartialRefuel}
#endif // usePartialRefuel
#ifdef useScreenEditor
	,{screenEditIdx | 0x80,	1,	mainScreenDisplayFormatSize * 2,	doScreenEditDisplay,	doCursorUpdateScreenEdit,	bpListScreenEdit}
#endif // useScreenEditor
};

static uint8_t menu::displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

	uint8_t i;
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
		case menuCursorUpdateIdx:
			menuTop = cursorPos;
			if ((cursorXdirection & 0x80) == 0)
			{

				menuTop--;
				if (menuTop > menuVisibleSize) menuTop += menuVisibleSize;

			}

		case menuOutputDisplayIdx:
			i = menuTop;

			for (uint8_t x = 0; x < 2; x++)
			{

				text::gotoXY(devLCD, 0, x);
				text::charOut(devLCD, ((i == cursorPos) ? '>' : ' ' ));
				text::stringOut(devLCD, menuTitles, i);
				i++;
				if (i >= menuVisibleSize) i = 0;

			}
			break;

		default:
			break;

	}

	return retVal;

}

static void menu::entry(void)
{

	cursor::moveAbsolute(menuScreenIdx, 255); // go to the menu screen level

}

static void menu::select(void)
{

	uint8_t i;

	i = screenCursor[(unsigned int)(menuScreenIdx)];

	cursor::moveAbsolute(pgm_read_byte(&screenParameters[(unsigned int)(i)].menuIndex), 255);

}

static void menu::doNextBright(void)
{

	initStatusLine();
	text::stringOut(devLCD, PSTR("Backlight = "));
	text::stringOut(devLCD, brightString, brightnessIdx);
	execStatusLine();

}

static void menu::doNothing(void)
{
}

static void menu::noSupport(void)
{

	initStatusLine();
	text::stringOut(devLCD, PSTR("Btn 0x"));
	text::hexByteOut(devLCD, buttonPress);
	text::stringOut(devLCD, PSTR(" Pressed"));
	execStatusLine();

}

/* LCD screen cursor manipulation section */

static void cursor::screenLevelEntry(const char * str, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	printStatusMessage(str);

}

static void cursor::screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	printStatusMessage(str, strIdx);

}

static void cursor::moveAbsolute(uint8_t positionY, uint8_t positionX)
{

	uint8_t levelChangeFlag;

	if (menuLevel == positionY) levelChangeFlag = 0; // same level
	else
	{

		levelChangeFlag = 1;
		menuLevel = positionY;

	}

	cursorXdirection = 0;
	positionY = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount);

	switch (positionX)
	{

		case 255:
			break;

		case 254:
			screenCursor[(unsigned int)(menuLevel)] = positionX - 1;
			break;

		default:
			if (positionX < positionY) screenCursor[(unsigned int)(menuLevel)] = positionX;
			break;

	}

	updateAfterMove(levelChangeFlag);

}

static void cursor::moveRelative(uint8_t moveY, uint8_t moveX)
{

	uint8_t wrapAroundFlag;
	uint8_t levelChangeFlag;
	uint8_t v;
	uint8_t maxVal;
	uint8_t x; // base menu level

	x = (pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeIndex) & 0x7F); // base menu level
	levelChangeFlag = 0;

	wrapAroundFlag = 0; // initially, signal that no wrap-around occurred

	if (moveX) // if movement within the screen level is called for
	{

		if (moveX & 0x80) cursorXdirection = 0xFF;
		else cursorXdirection = 0x01;

		maxVal = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount); // "horizontal" size

		v = screenCursor[(unsigned int)(menuLevel)] + cursorXdirection; // perform move

		if (v == maxVal) // if screen cursor bumped up to screen level size
		{

			v = 0; // wrap around
			wrapAroundFlag = 1; // signal wrap-around occurred

		}

		if (v > maxVal) // if screen cursor bumped down past lower limit
		{

			v = maxVal - 1; // wrap around
			wrapAroundFlag = 1;

		}

		if (wrapAroundFlag) moveY = cursorXdirection; // if wrap-around occurred, signal direction to next screen level

		screenCursor[(unsigned int)(menuLevel)] = v;

	}
	else cursorXdirection = 0;

	if (moveY)
	{

		if (moveY & 0x80) moveY = 0xFF;
		else moveY = 0x01;

		maxVal = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeYcount); // "vertical" size

		v = menuLevel - x + moveY;

		if (v == maxVal) v = 0;
		if (v > maxVal) v = maxVal - 1;

		v += x;
		if (menuLevel != v)
		{

			menuLevel = v;
			levelChangeFlag = 1;

		}

		if (wrapAroundFlag) // if wrap-around occurred, look at direction
		{

			if (moveY == 1) v = 0; // if direction was forward, set current cursor to zero
			else v = pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeXcount) - 1; // else, set current cursor to end of new menu level

			screenCursor[(unsigned int)(menuLevel)] = v;

		}
		else cursorXdirection = 0;

	}

	updateAfterMove(levelChangeFlag);

}

static void cursor::updateAfterMove(uint8_t levelChangeFlag)
{

	uint8_t cp;
	uint8_t cf;

	cp = screenCursor[(unsigned int)(menuLevel)];

	// call indexed support section cursor update function to update any section-specific data
	if (levelChangeFlag) cf = menuEntryIdx;
	else cf = menuCursorUpdateIdx;

	((displayHandlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(cf, cp, 0);

}

static void cursor::shortLeft(void)
{

	cursor::moveRelative(0, 255); // go to previous option in current level

}

static void cursor::shortRight(void)
{

	cursor::moveRelative(0, 1); // go to next option in current level

}

static void cursor::longLeft(void)
{

	cursor::moveRelative(255, 0); // go to last option in previous level

}

static void cursor::longRight(void)
{

	cursor::moveRelative(1, 0); // go to next option in next level

}

static void printStatusMessage(const char * s)
{

	initStatusLine();
	text::stringOut(devLCD, s);
	execStatusLine();

}

static void printStatusMessage(const char * s, uint8_t strIdx)
{

	printStatusMessage(findStr(s, strIdx));

}

void initStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = 0; // end hold delay countdown
	timer0Command &= ~(t0cDisplayDelay); // cancel display delay

	SREG = oldSREG; // restore interrupt flag status

#ifdef blankScreenOnMessage
	text::charOut(devLCD, 0x0C); // clear the entire screen
#else // blankScreenOnMessage
	text::gotoXY(devLCD, 0, 0); // go to the first line
#endif // blankScreenOnMessage

}

void execStatusLine(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	displayPauseCount = holdDelay; // start hold delay countdown
	timer0Command |= (t0cDisplayDelay); // make display delay active

	SREG = oldSREG; // restore interrupt flag status

}

const char * findStr(const char * str, uint8_t strIdx)
{

	uint8_t c;

	while (strIdx)
	{

		do c = pgm_read_byte(str++);
		while ((c) && (c != 0x0D));

		strIdx--;

	}

	return str;

}

/* text string output section */

static const uint8_t odvFlagCRLF =				0b00000100;
static const uint8_t odvFlagShootBlanks =		0b00000010;
static const uint8_t odvFlagEnableOutput =		0b00000001;

static void text::newLine(interfaceDevice &dev)
{

	charOut(dev, 0x0D);

}

static void text::gotoXY(interfaceDevice &dev, uint8_t xPos, uint8_t yPos)
{

	charOut(dev, 0x80 + yPos * 20 + xPos);

}

static uint8_t text::charOut(interfaceDevice &dev, uint8_t chr)
{

	uint8_t retVal;

	retVal = 1;

	if (dev.chrOut)
	{

		switch (chr)
		{

			case 0x00:	// defined as end-of-string
				dev.controlFlags |= (odvFlagEnableOutput);
				retVal = 0;
				break;

			case 0xEB:	// disable device output for metric mode
				if (metricFlag & metricMode) dev.controlFlags &= ~(odvFlagEnableOutput);
				else dev.controlFlags |= (odvFlagEnableOutput);
				break;

			case 0xEC: // toggle device output enable
				dev.controlFlags ^= (odvFlagEnableOutput);
				break;

			case 0xED: // enable device output
				dev.controlFlags |= (odvFlagEnableOutput);
				break;

			case 0xEE:	// disable device output
				dev.controlFlags &= ~(odvFlagEnableOutput);
				break;

			case 0x0D:	// also defined as end of string
				retVal = 0;
				dev.controlFlags |= (odvFlagEnableOutput);
				dev.chrOut(0x0D);
				if (dev.controlFlags & odvFlagCRLF) dev.chrOut(0x0A);
				break;

			case 0xF0 ... 0xF7: // print CGRAM character
				chr &= 0x07;
			case 0x20 ... 0x7F: // print normal character
				if (dev.controlFlags & odvFlagEnableOutput)
				{

					if (dev.controlFlags & odvFlagShootBlanks) chr = ' ';
					dev.chrOut(chr);

				}
				break;

			default:
				dev.chrOut(chr);
				break;

		}
	}
	else retVal = 0;

	return retVal;

}

static void text::setModeOnCondition(interfaceDevice &dev, uint8_t condition, uint8_t odvFlag)
{

	if (condition) dev.controlFlags |= (odvFlag);
	else dev.controlFlags &= ~(odvFlag);

}

static void text::stringOut(interfaceDevice &dev, char * str)
{

	while (charOut(dev, * str++));

}

static void text::stringOut(interfaceDevice &dev, const char * str)
{

	while (charOut(dev, pgm_read_byte(str++))) ;

}

static void text::stringOut(interfaceDevice &dev, const char * str, uint8_t strIdx)
{

	stringOut(dev, findStr(str, strIdx));

}

static void text::hexNybbleOut(interfaceDevice &dev, uint8_t val)
{

	val &= 0x0F;
	val |= 0x30;
	if (val > 0x39) val += 0x07;
	charOut(dev, val);

}

static void text::hexByteOut(interfaceDevice &dev, uint8_t val)
{

	hexNybbleOut(dev, val >> 4);
	hexNybbleOut(dev, val);

}

static void text::hexWordOut(interfaceDevice &dev, uint16_t val)
{

	union union_16 * vee = (union union_16 *) &val;

	for (uint8_t i = 1; i < 2; i--) hexByteOut(dev, vee->u8[i]);

}

static void text::hexDWordOut(interfaceDevice &dev, uint32_t val)
{

	union union_32 * vee = (union union_32 *) &val;

	for (uint8_t i = 3; i < 4; i--) hexByteOut(dev, vee->u8[i]);

}

static void text::hexLWordOut(interfaceDevice &dev, uint64_t * val)
{

	union union_64 * vee = (union union_64 *) val;

	for (uint8_t i = 7; i < 8; i--) hexByteOut(dev, vee->u8[i]);

}

static uint8_t text::numberOut(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx, char * strBuff, uint8_t windowLength, uint8_t decimalFlag)
{

	calcFuncObj thisCalcFuncObj;

	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, strBuff, windowLength, decimalFlag); // perform the required decimal formatting
	stringOut(dev, thisCalcFuncObj.strBuffer); // output the number

	return thisCalcFuncObj.calcFmtIdx;

}

// this function is called whenever the main program has to wait for some external condition to occur
// when the main program performs some I/O activity, the target peripheral may take some time to acknowledge the activity
//    and let the main program know that the peripheral is ready for more activity
// oftentimes, the main program has to wait for the peripheral in question, so instead of spinning its wheels, it will simply
//    cause the processor to go to sleep by calling this function
// any interrupt will wake up the processor - this also means the primary timer0
// this function is therefore ideal for placing all purely internal operations that do not require direct interaction with the outside world
//
// - fuel/vehicle speed data transfers
// - acceleration test state changes
// - debug sensor simulation counter updates
// - analog button read translations
// - Chrysler fuel correction factor calculations
// - any other operation not requiring interaction with peripherals that require waiting periods to finish
//
// said internal operations would hopelessly bog down interrupt handlers, were they loaded with these internal operations
//
static void idleProcess(void)
{

#ifdef useCPUreading
	uint32_t idleStart;
#endif // useCPUreading

#if defined(useActivityLED)
	activityLED::output(0);

#endif // defined(useActivityLED)
#ifdef useCPUreading
	idleStart = systemInfo::cycles0(); // record starting time

#endif // useCPUreading
	performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#ifdef useCPUreading
	idleTimerLength += systemInfo::findCycleLength(idleStart, systemInfo::cycles0());
#ifdef useDebugCPUreading
	idleStart = systemInfo::cycles0(); // record starting time
#endif // useDebugCPUreading

#endif // useCPUreading
#if defined(useActivityLED)
	activityLED::output(1);

#endif // defined(useActivityLED)
	// this is the part of the main loop that only executes twice a second (or what is defined by loopsPerSecond), to collect and process readings
	if (timer0Status & t0sTakeSample) // if main timer has commanded a sample be taken
	{

		changeBitFlags(timer0Status, t0sTakeSample, 0); // acknowledge sample command

#ifdef useCPUreading
		systemInfo::idleProcess();

#endif // useCPUreading
		tripSupport::idleProcess();

	}

#if defined(useAnalogRead)
#ifdef useChryslerMAPCorrection
	if (analogStatus & asReadMAPchannel)
	{

		changeBitFlags(analogStatus, asReadMAPchannel, 0); // acknowledge ADC read completion

		SWEET64::runPrgm(prgmCalculateMAPpressure, 0);

	}

#endif // useChryslerMAPCorrection
#ifdef useChryslerBaroSensor
	if (analogStatus & asReadBaroChannel)
	{

		changeBitFlags(analogStatus, asReadBaroChannel, 0); // acknowledge ADC read completion

		SWEET64::runPrgm(prgmCalculateBaroPressure, 0);

	}

#endif // useChryslerBaroSensor
#ifdef useAnalogButtons
	if (analogStatus & asReadButtonChannel)
	{

		changeBitFlags(analogStatus, asReadButtonChannel, 0); // acknowledge ADC read completion

		for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
		{

			if (analogValue[(unsigned int)(analogButtonChannelIdx)] >= pgm_read_word(&analogButtonThreshold[(unsigned int)(x)]))
			{

#ifdef useTestAnalogButtonIdx
				thisButtonIdx = x;
#endif //  useTestAnalogButtonIdx
				button::inject(pgm_read_byte(&analogTranslate[(unsigned int)(x)]));
				break;

			}

		}

	}

#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useDragRaceFunction
	if (timer0Status & t0sAccelTestFlag)
	{

		changeBitFlags(timer0Status, t0sAccelTestFlag, 0); // acknowledge sample command

		accelerationTest::idleProcess();

	}

#endif // useDragRaceFunction
#ifdef useSimulatedFIandVSS
	if (timer1Status & t1sDebugUpdateFIP) // if debug fuel injector pulse period needs adjustment
	{

		changeBitFlags(timer1Status, t1sDebugUpdateFIP, 0); // acknowledge debug update request

		debugReading::idleProcessFuel();

	}

	if (timer1Status & t1sDebugUpdateVSS) // if VSS pulse period needs adjustment
	{

		changeBitFlags(timer1Status, t1sDebugUpdateVSS, 0); // acknowledge debug update request

		debugReading::idleProcessVSS();

	}

#endif // useSimulatedFIandVSS
#ifdef useDebugCPUreading
	idleProcessTimerLength += systemInfo::findCycleLength(idleStart, systemInfo::cycles0());

#endif // useDebugCPUreading
}

// primary MPGuino processing routine - overwrites Arduino sketch main if compiled in Arduino IDE

int main(void)
{

	uint8_t i;
	uint8_t j;
#ifdef useDebugCPUreading
	uint32_t displayStart;
#endif // useDebugCPUreading

	const buttonVariable * bpPtr;

	cli(); // disable interrupts while interrupts are being fiddled with

	initCore(); // go initialize core MPGuino functionality

	j = EEPROM::powerUpCheck();

	tripSupport::init();

#ifdef useSimulatedFIandVSS
	debugFlags = (debugVSSflag | debugInjectorFlag);
	debugReading::configurePorts();

#endif // useSimulatedFIandVSS
#ifdef useDragRaceFunction
	accelerationFlags &= ~accelTestClearFlags;
	lastAccelTestStatus = accelerationFlags;
	accelTestStatus = accelerationFlags;

#endif // useDragRaceFunction
	menuLevel = 255;
	topScreenLevel = 0;

	timer0DelayCount = delay1500msTick; // request a set number of timer tick delays equivalent to 1.5 seconds
	timer0Command |= (t0cDoDelay); // signal request to timer

	sei();

	initHardware(); // initialize all human interface peripherals

#ifdef useSavedTrips
	i = tripSupport::doAutoAction(1);

#endif // useSavedTrips
	text::gotoXY(devLCD, 0, 0);
	text::stringOut(devLCD, titleMPGuino);
	text::stringOut(devLCD, dateMPGuino);

#ifdef outputLoggingSplash
	text::stringOut(devLogOutput, titleMPGuino);
	text::stringOut(devLogOutput, dateMPGuino);

#endif // outputLoggingSplash
#ifdef outputDebugTerminalSplash
	text::stringOut(devDebugTerminal, titleMPGuino);
	text::stringOut(devDebugTerminal, dateMPGuino);

	terminalState = 0;

#endif // outputDebugTerminalSplash
	delay0(0); // show splash screen for 1.5 seconds

#ifdef useTestButtonValues
	cursor::moveAbsolute(buttonViewIdx, 0);
#else // useTestButtonValues
#ifdef useLegacyLCD
	if (j) cursor::moveAbsolute(settingScreenIdx, (pContrastIdx - eePtrSettingsStart)); // go through the initialization screen
#else // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
	if (j) cursor::moveAbsolute(settingScreenIdx, (pLCDcolorIdx - eePtrSettingsStart)); // go through the initialization screen
#else // useAdafruitRGBLCDshield
	if (j) cursor::moveAbsolute(settingScreenIdx, (pMetricModeIdx - eePtrSettingsStart)); // go through the initialization screen
#endif // useAdafruitRGBLCDshield
#endif // useLegacyLCD
	else cursor::moveAbsolute(mainScreenIdx, 0);
#endif // useTestButtonValues

#ifdef useSavedTrips
	if (i) printStatusMessage(PSTR("AutoRestore Done"));

#endif // useSavedTrips
#ifdef useCPUreading
	mainStart = systemInfo::cycles0();
	idleTimerLength = 0;
#ifdef useDebugCPUreading
	idleProcessTimerLength = 0;
	monitorState = 0;
	displayTimerLength = 0;
	SWEET64timerLength = 0;
#endif // useDebugCPUreading

#endif // useCPUreading
	while (true)
	{

#ifdef useDebugCPUreading
		displayStart = systemInfo::cycles0();

#endif // useDebugCPUreading
		if (activityChangeFlags & afActivityTimeoutFlag)
		{

			changeBitFlags(activityChangeFlags, afActivityTimeoutFlag, 0); // clear activity change activity timeout flag

			if (activityFlags & afActivityTimeoutFlag) // if MPGuino is commanded to go asleep
			{

#ifdef useDeepSleep
				doGoDeepSleep();

#else // useDeepSleep
#ifdef useClockDisplay
				bigDigit::displayHandler(menuEntryIdx, 255, 1); // initialize the software clock
				text::charOut(devLCD, 0x12); // set backlight brightness to zero

#else // useClockDisplay
				LCD::shutdown(); // shut down the LCD display

#endif // useClockDisplay
#endif // useDeepSleep
			}
			else // if MPGuino is commanded to wake up
			{

				LCD::init(); // re-initialize LCD device
				((displayHandlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(menuEntryIdx, screenCursor[(unsigned int)(menuLevel)], 1); // call indexed support section screen initialization function

			}

		}

		if (activityChangeFlags & afEngineOffFlag)
		{

			changeBitFlags(activityChangeFlags, afEngineOffFlag, 0); // clear activity change engine off flag

			if (((activityFlags & afEngineOffFlag) == 0) && (EEPROM::readVal(pWakeupResetCurrentOnEngineIdx))) tripSupport::resetCurrent(); // if engine start is detected

		}

		if (activityChangeFlags & afVehicleStoppedFlag)
		{

			changeBitFlags(activityChangeFlags, afVehicleStoppedFlag, 0); // clear activity change vehicle stopped flag

			if (((activityFlags & afVehicleStoppedFlag) == 0) && (EEPROM::readVal(pWakeupResetCurrentOnMoveIdx))) tripSupport::resetCurrent(); // if vehicle movement is detected

		}

#if useWindowTripFilter || useSavedTrips
		if (activityChangeFlags & afParkFlag)
		{

			changeBitFlags(activityChangeFlags, afParkFlag, 0); // clear activity change park flag

			if (activityFlags & afParkFlag) // if MPGuino is commanded to go park
			{

#ifdef useWindowTripFilter
				tripSupport::resetWindowFilter(); // reset the window trip filter

#endif // useWindowTripFilter
#ifdef useSavedTrips
				if (tripSupport::doAutoAction(0)) printStatusMessage(PSTR("AutoSave Done"));

#endif // useSavedTrips
			}

		}

#endif // useWindowTripFilter || useSavedTrips
#ifdef useDebugTerminal
		terminal::mainProcess();

#endif // useDebugTerminal
		if (timer0Status & t0sReadButton) // see if any buttons were pressed, process related button function if so
		{

			j = buttonPress; // capture button state
			changeBitFlags(timer0Status, t0sReadButton, 0); // acknowledge sample command

			bpPtr = (const buttonVariable *)(pgm_read_word(&(screenParameters[(unsigned int)(menuLevel)].modeButtonList)));

			while (true)
			{

				i = pgm_read_byte(&(bpPtr->buttonCode));

				if ((j == i) || (i == buttonsUp)) break;
				else bpPtr++;

			}

			text::gotoXY(devLCD, 0, 0);
			((pFunc)pgm_read_word(&(bpPtr->buttonCommand)))(); // go perform action

		}

#if useDataLoggingOutput || useJSONoutput
		// this part of the main loop handles logging data output
		if (timer0Status & t0sOutputLogging)
		{

			changeBitFlags(timer0Status, t0sOutputLogging, 0); // acknowledge output serial command

#ifdef useDataLoggingOutput
			if (EEPROM::readVal(pSerialDataLoggingIdx)) doOutputDataLog();

#endif // useDataLoggingOutput
#ifdef useJSONoutput
			if (awakeFlags & aAwakeOnVehicle) doOutputJSON();

#endif // useJSONoutput
		}

#endif // useDataLoggingOutput || useJSONoutput
		// this part of the main loop handles screen output to the user
		// it can execute either after the samples are collected and processed above, or after a key has been pressed
		if (timer0Status & t0sUpdateDisplay)
		{

			changeBitFlags(timer0Status, t0sUpdateDisplay, 0); // acknowledge update display command

#if defined(useOutputPins)
			outputPin::setOutputPin1(0);
			outputPin::setOutputPin2(1);

#endif // defined(useOutputPins)
#ifdef useBarFuelEconVsSpeed
			FEvSpdTripIdx = (uint8_t)(SWEET64::runPrgm(prgmFEvsSpeed, instantIdx));

#endif // useBarFuelEconVsSpeed
			// this section handles all MPGuino activity modes
			// engine running and vehicle in motion and a button being pressed
			// engine not running and vehicle in motion (Engine Off Coasting) and a button being pressed
			// engine running and vehicle stopped and a button being pressed
			// engine stopped and vehicle stopped and a button being pressed
			// engine running and vehicle in motion
			// engine not running and vehicle in motion (Engine Off Coasting)
			// engine running and vehicle stopped
			// engine stopped and vehicle stopped
			//
			// in addition, two independent timers are dedicated to these activity modes
			//
			// the park timer counts down from a stored parameter value once MPGuino detects engine not running and vehicle stopped
			// if the park timer goes to zero, the vehicle is said to be parked, and MPGuino goes to parked mode
			// parked mode is exited once MPGuino detects that the engine runs OR the vehicle is in motion
			//
			// the activity timer counts down from a stored parameter value once MPGuino detects engine not running, vehicle stopped, and no button press occurs
			// if the activity timer goes to zero, MPGuino goes to sleep mode
			// sleep mode is exited once MPGuino detects that the engine runs OR the vehicle is in motion OR a button press occurs
			//
			// if the park timer setting in stored parameters is larger than the activity timer, MPGuino will skip parked mode
			i = activityFlags & afValidFlags; // fetch a local copy of activity flags

			if (pgm_read_byte(&screenParameters[(unsigned int)(menuLevel)].modeIndex) & 0x80) // if the currently displayed screen is marked to ignore button presses
			{

				if ((i == (afVehicleStoppedFlag | afButtonFlag)) || (i == (afEngineOffFlag | afButtonFlag))) i &= ~(afButtonFlag); // ignore button presses for this screen

			}

			text::gotoXY(devLCD, 0, 0);
			switch (i)
			{

				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag | afParkFlag): // engine stopped, vehicle stopped, no buttons pressed for a while
				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag): // engine stopped, vehicle stopped for a while
				case (afVehicleStoppedFlag | afButtonFlag): // vehicle stopped, button not pressed
				case (afEngineOffFlag | afButtonFlag): // engine stopped, button not pressed
				case (afVehicleStoppedFlag): // vehicle stopped
				case (afEngineOffFlag): // engine stopped
				case (afVehicleStoppedFlag | afEngineOffFlag): // engine stopped, vehicle stopped
				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag): // engine stopped, vehicle stopped, button not pressed
				case 0: // engine running and vehicle in motion
				case (afButtonFlag): // engine running, vehicle in motion, button not pressed
					((displayHandlerFunc)pgm_read_word(&screenParameters[(unsigned int)(menuLevel)].screenDisplayHandler))(menuOutputDisplayIdx, screenCursor[(unsigned int)(menuLevel)], 0); // call indexed support section screen refresh function
					break;

				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag | afButtonFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, activity timeout reached
				case (afVehicleStoppedFlag | afEngineOffFlag | afButtonFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, park timeout reached, activity timeout reached
#ifndef useDeepSleep
#ifdef useClockDisplay
					bigDigit::displayHandler(menuOutputDisplayIdx, 255, 0); // display the software clock

#endif // useClockDisplay
#endif // useDeepSleep
					break;

				default: // handle unexpected cases
					text::hexByteOut(devLCD, i);
					break;

			}

		}

#ifdef useDebugCPUreading
		displayTimerLength =+ systemInfo::findCycleLength(displayStart, systemInfo::cycles0());

#endif // useDebugCPUreading
		idleProcess(); // all functions are completed at this point, so wait for an interrupt to occur

	}

}
