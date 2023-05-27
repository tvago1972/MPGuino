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
#include "m_output.h"
#include "m_lcd.h"
#include "functions.h"
#include "feature_settings.h"
#include "feature_datalogging.h"
#include "feature_bignum.h"
#include "feature_bargraph.h"
#include "feature_debug.h"
#include "feature_dragrace.h"
#include "feature_coastdown.h"

#if defined(__AVR_ATmega32U4__)
#ifndef PRTIM4
#define PRTIM4 4

#endif // PRTIM4
#endif // defined(__AVR_ATmega32U4__)
uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged); /* Main screen section */
void doReturnToMainScreen(void);
void doNextBright(void);
#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void);
#endif // useDeepSleep
#ifdef useScreenEditor // Programmable main display screen edit support section
void doCursorUpdateScreenEdit(void);
void doScreenEditDisplay(void);
void doGoScreenEdit(void);
void doScreenEditBump(void);
void doSaveScreen(void);
void doScreenReturnToMain(void);
#endif // useScreenEditor
static void idleProcess(void); // place all time critical main program internal functionality here - no I/O!
static void performSleepMode(uint8_t sleepMode);
int main(void);

namespace peripheral /* MPGuino human interface I/O peripheral device prototype */
{

	static void initMain(void);
	static void shutdownMain(void);
	static void changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR);
#ifdef useExpansionPins
	static void initExpansionPin(void);
	static void shutdownExpansionPin(void);
	static void outputExpansionPin1(uint8_t pin);
	static void outputExpansionPin2(uint8_t pin);
#endif // useExpansionPins
#ifdef useStatusLEDpins
	static void initStatusLED(void);
	static void shutdownStatusLED(void);
	static void outputStatusLED(uint8_t val);
#endif // useStatusLEDpins

};

uint8_t menuLevel;
uint8_t topScreenLevel;
uint8_t brightnessIdx;

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

const uint8_t menuExitIdx =				0;								// this call may not even be necessary
const uint8_t menuEntryIdx =			menuExitIdx + 1;				// typically, this call will fall through
const uint8_t menuCursorUpdateIdx =		menuEntryIdx + 1;				// ...to this call, then will fall through
const uint8_t menuOutputDisplayIdx =	menuCursorUpdateIdx + 1;		// ...to this call

typedef uint8_t (* displayHandlerFunc)(uint8_t, uint8_t, uint8_t); // type for various handler functions

namespace menu /* Top-down menu selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void select(void);
	static void doNothing(void);
	static void noSupport(void);

};

static const char titleMPGuino[] PROGMEM = {
	"\xED" "MPGuino v1.95tav\r"
};

static const char dateMPGuino[] PROGMEM = {
	"2023-MAR-18\r"
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

const uint8_t displayPageCount = 9						// count of base number of data screens
#ifdef trackIdleEOCdata
	+ 3													// count of Idle/EOC tracking data screens
#endif // trackIdleEOCdata
;
const uint8_t mainScreenDisplayFormatSize = displayPageCount * 4;

typedef struct
{

	uint8_t menuIndex;
	uint8_t modeIndex;
	uint8_t modeYcount;
	uint8_t modeXcount;
	displayHandlerFunc screenDisplayHandler;
	const buttonVariable (* modeButtonList);

} modeInformation;

static const buttonVariable bpListMenu[] PROGMEM = {
	 {btnShortPressC,	menu::select}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	cursor::shortRight}
		,{btnShortPressU,	cursor::shortLeft}
		,{btnLongPressU,	doNextBright}
		,{btnShortPressL,	menu::doNothing}
		,{btnShortPressR,	menu::select}
#else // useButtonCrossConfig
		,{btnShortPressR,	cursor::shortRight}
		,{btnShortPressL,	cursor::shortLeft}
		,{btnLongPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
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
		,{btnShortPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnShortPressD,	cursor::shortRight}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
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
		,{btnShortPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
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
		,{btnShortPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
		,{btnShortPressC,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
		,{btnShortPressC,	doNextBright}
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
		,{btnShortPressU,	doNextBright}
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
		,{btnShortPressC,	doNextBright}
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

volatile uint8_t cursorXdirection;
volatile uint8_t menuTop;

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

/* Main screen section */

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
uint8_t mainScreenDisplayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged)
{

#ifdef useScreenEditor
	uint8_t i;
#endif // useScreenEditor
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			topScreenLevel = menuLevel; // save current menu level for sub-function (param edit, trip load/save, etc) support

		case menuCursorUpdateIdx:
			printStatusMessage(mainScreenFuncNames, cursorPos); // briefly display screen name

		case menuOutputDisplayIdx:
			retVal = (activityFlags & afActivityCheckFlags);
			switch (retVal)
			{

				case (afVehicleStoppedFlag | afButtonFlag):
#ifdef trackIdleEOCdata
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainIdleScreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainIdleScreenFormats, 0);
#endif // useSpiffyTripLabels
					break;

#endif // trackIdleEOCdata
				case (afEngineOffFlag | afButtonFlag):
#ifdef trackIdleEOCdata
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainEOCscreenFormats, 0, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainEOCscreenFormats, 0);
#endif // useSpiffyTripLabels
					break;

#endif // trackIdleEOCdata
				default:
#ifdef useScreenEditor
					i = cursorPos;
					i <<= 2;
					i += eePtrScreensStart;

#ifdef useSpiffyTripLabels
					for (uint8_t x = 0; x < 4; x++) displayFunction(x, EEPROM::readVal(i++), EEPROM::readVal(i++), 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					for (uint8_t x = 0; x < 4; x++) displayFunction(x, EEPROM::readVal(i++), EEPROM::readVal(i++));
#endif // useSpiffyTripLabels

#else // useScreenEditor
#ifdef useSpiffyTripLabels
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos, 136, 0, msTripBitPattern);
#else // useSpiffyTripLabels
					displayMainScreenFunctions(mainDisplayScreenFormats, cursorPos);
#endif // useSpiffyTripLabels
#endif // useScreenEditor
					break;

			}
			retVal = 0;
			break;

		default:
			break;

	}

	return retVal;

}

void doReturnToMainScreen(void)
{

	cursor::moveAbsolute(topScreenLevel, 255);

}

void doNextBright(void)
{

	initStatusLine();
	text::stringOut(devLCD, PSTR("Backlight = "));
	text::stringOut(devLCD, brightString, brightnessIdx);
	execStatusLine();

}

#ifdef useDeepSleep // Deep Sleep support section
void doGoDeepSleep(void)
{

	peripheral::shutdownMain();

#ifdef useTWIsupport
	TWI::shutdown();

#endif // useTWIsupport
	performSleepMode(SLEEP_MODE_PWR_DOWN); // go perform power-down sleep mode

	peripheral::initMain(); // restart all peripherals

}

#endif // useDeepSleep
#ifdef useScreenEditor // Programmable main display screen edit support section
uint8_t screenTripValue = 0;
uint8_t screenFunctionValue = 0;
uint8_t screenEditDirty = 0;

const uint8_t screenEditFlag_dirty =	0x80;

const char seFormatRevertedString[] PROGMEM = "Format reverted";
const char seExitScreenEditString[] PROGMEM = "Screen Display";

void doCursorUpdateScreenEdit(void)
{

	uint8_t b;

	b = screenCursor[(unsigned int)(screenEditIdx)];
	b &= ~screenEditFlag_dirty;

	if ((screenEditDirty & ~screenEditFlag_dirty) ^ b) // if cursor moved to a different screen function altogether
	{

		screenTripValue = EEPROM::readVal(b + eePtrScreensStart);
		screenFunctionValue = EEPROM::readVal(b + eePtrScreensStart + 1);

		if (screenEditDirty & screenEditFlag_dirty) // if previous screen function was changed but not saved
		{

			printStatusMessage(seFormatRevertedString);

		}

		screenEditDirty = b; // save current cursor position of screen function being edited
		screenEditDirty &= ~screenEditFlag_dirty; // mark screen function as not modified

	}

}

void doScreenEditDisplay(void)
{

	uint8_t i;
	uint8_t j;
	uint8_t k;

	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t calcBlink;
	uint8_t tripBlink;

	i = screenCursor[(unsigned int)(screenEditIdx)];

	j = i; // extract whether trip index or trip function is to be edited
	j &= 0x01;

	i >>= 1; // figure out edited screen function placement on screen
	k = i;

	i &= 0xFC; // figure out start of screen page
	k &= 0x03;

	for (uint8_t x = 0; x < 4; x++)
	{

		calcBlink = 0;
		tripBlink = 0;

		if (x == k) // if at
		{

			tripIdx = screenTripValue;
			calcIdx = screenFunctionValue;
			if (j == 1) calcBlink = 170;
			else tripBlink = 170;

		}
		else
		{

			tripIdx = EEPROM::readVal(eePtrScreensStart + (i + x) * 2);
			calcIdx = EEPROM::readVal(eePtrScreensStart + (i + x) * 2 + 1);

		}

#ifdef useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx, tripBlink, calcBlink, msTripBitPattern); // call main screen function display routine
#else // useSpiffyTripLabels
		displayFunction(x, tripIdx, calcIdx); // call main screen function display routine
#endif // useSpiffyTripLabels

	}

}

void doGoScreenEdit(void)
{

	screenEditDirty = screenCursor[(unsigned int)(screenEditIdx)];
	screenEditDirty >>= 1;
	screenEditDirty ^= 0x01; // force update of screen function
	screenEditDirty &= ~screenEditFlag_dirty;

	cursor::moveAbsolute(screenEditIdx, screenCursor[(unsigned int)(mainScreenIdx)] << 3);

	printStatusMessage(PSTR("Screen Editor"));

}

void doScreenEditBump(void)
{

	uint8_t b;

	b = (screenCursor[(unsigned int)(screenEditIdx)] & 0x01); // figure out whether trip variable or trip function is being modified
	screenEditDirty |= screenEditFlag_dirty; // mark current screen function as modified

	if (b) // if trip function is being modified
	{

		screenFunctionValue++;
		if (screenFunctionValue == dfMaxValDisplayCount) screenFunctionValue = 0;

	}
	else // otherwise, trip variable is being modified
	{

		screenTripValue++;
		if (screenTripValue == dfMaxTripCount) screenTripValue = 0;

	}

}

void doSaveScreen(void)
{

	uint8_t b;

	b = screenCursor[(unsigned int)(screenEditIdx)];

	// generate and save new screen function from trip variable and trip function
	EEPROM::writeVal(eePtrScreensStart + b, (unsigned long)(screenTripValue));
	EEPROM::writeVal(eePtrScreensStart + b + 1, (unsigned long)(screenFunctionValue));
	screenEditDirty &= ~screenEditFlag_dirty;

	printStatusMessage(PSTR("Format saved"));

}

void doScreenReturnToMain(void)
{

	const char * str;

	if (screenEditDirty & screenEditFlag_dirty) str = seFormatRevertedString;
	else s = seExitScreenEditString;

	screenCursor[(unsigned int)(mainScreenIdx)] = (screenCursor[(unsigned int)(screenEditIdx)] >> 3);
	cursor::screenLevelEntry(str, topScreenLevel);

}

#endif // useScreenEditor
/* human interface I/O peripheral device support section */

#ifdef useExpansionPins
/*
	Modes
	0	Off
	1	instant fuel economy < current fuel economy (0 - false, 255 - true)
	2	instant fuel economy > current fuel economy (0 - false, 255 - true)
	3	instant fuel economy < tank fuel economy (0 - false, 255 - true)
	4	instant fuel economy > tank fuel economy (0 - false, 255 - true)
	5	estimated tank fuel consumed (0 (none) to 255 (all of the tank), based on tank size)
	6	estimated tank fuel remaining (0 (empty) to 255 (full), based on tank size)
*/
static const uint8_t prgmCalculateOutputPinValue[] PROGMEM = {
	instrCmpIndex, 2,									// is a valid expansion output pin number being requested?
	instrBranchIfGTorE, 13,								// skip to output a zero if not
	instrLxdIEEPROMoffset, pExpansionPin1Mode,			// load the indexed stored parameter index for the expansion output pin setting
	instrTestIndex,										// test pin mode value for zero
	instrBranchIfZero, 8,								// exit out if pin mode is zero
	instrCmpIndex, 4,									// test if pin mode is "fuel economy comparison between instant and whatever"
	instrBranchIfLTorE,	26,								// if so, skip ahead
	instrCmpIndex, 6,									// test if pin mode is analog output tank quantity or quantity remaining
	instrBranchIfLTorE, 4,								// if so, skip ahead

//zeroOutRet:
	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone,											// exit to caller

//analogOut:
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrCmpIndex, 6,									// outputting remaining value?
	instrBranchIfE, 7,									// go do conversion if so
	instrLdReg, 0x21,									// shift contents to register 1
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubYfromX, 0x12,								// subtract remaining fuel value from tank size

//analogOut2:
	instrMul2byByte, 255,								// multiply tank quantity by 255
	instrDiv2byMain, mpTankSizeIdx,						// divide by calculated tank size
	instrDone,											// return to caller

// US (inst FE > accumulated FE) = /(inst FE <= accumulated FE)
//
//  (inst distance)      (accum distance)
//  ---------------  <=  ----------------  >>>  (inst distance) * (accum quantity)  <=  (inst quantity) * (accum distance)
//  (inst quantity)      (accum quantity)
//
//
//
// metric (inst FE > accumulated FE) = /(inst FE <= accumulated FE)
//
//  (inst quantity)      (accum quantity)
//  ---------------  =>  ----------------  >>>  (inst quantity) * (accum distance)  =>  (inst distance) * (accum quantity)
//  (inst distance)      (accum distance)                                           or
//                                              (inst distance) * (accum quantity)  <=  (inst quantity) * (accum distance)
//
// therefore, for (inst FE > accumulated FE) comparison, there is no need to check whether MPGuino is in metric mode or not
// (inst FE < accumulated FE) is treated in a similar way, but the left and right terms are swapped around
//
	instrCmpIndex, 2,									// comparing instant to current?
	instrBranchIfLTorE, 10,								// if so, skip ahead
	instrLdRegTripVar, 0x03, tankIdx, rvInjCycleIdx,	// fetch tank (accum quantity)
	instrLdRegTripVar, 0x02, tankIdx, rvVSSpulseIdx,	// fetch tank (accum distance)
	instrSkip, 8,										// skip ahead
	instrLdRegTripVar, 0x03, currentIdx, rvInjCycleIdx,	// fetch current (accum quantity)
	instrLdRegTripVar, 0x02, currentIdx, rvVSSpulseIdx,	// fetch current (accum distance)
	instrLdRegTripVar, 0x01, instantIdx, rvInjCycleIdx,	// fetch (inst quantity)
	instrMul2by1,										// calculate (inst quantity) * (accum distance) as right term
	instrSwapReg, 0x23,									// swap right term and (accum quantity) values
	instrLdRegTripVar, 0x01, instantIdx, rvVSSpulseIdx,	// fetch (inst distance)
	instrMul2by1,										// calculate (inst distance) * (accum quantity) as left term
	instrCmpIndex, 2,									// testing (inst distance) * (current quantity) <= (inst quantity) * (current distance)?
	instrBranchIfE, 6,									// if so, skip ahead
	instrCmpIndex, 4,									// testing (inst distance) * (tank quantity) <= (inst quantity) * (tank distance)?
	instrBranchIfE, 2,									// if so, skip ahead
	instrSwapReg, 0x23,									// swap left and right terms
	instrCmpXtoY, 0x32,									// compare left term to right term
	instrBranchIfLTorE, 4,								// if left term <= right term, go zero out and return

	instrLdRegByte, 0x02, 255,							// max out result
	instrDone,											// return to caller

	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone											// exit to caller
};

#endif // useExpansionPins
static void peripheral::initMain(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// timer initialization section - multiple peripherals may use the same timer
#ifdef useTimer1
#if defined(__AVR_ATmega32U4__)
	// turn on timer1 module
	PRR0 &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC1C to disabled
	TCCR1A &= ~((1 << COM1C1) | (1 << COM1C0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer1 module
	PRR0 &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// clear timer 1 output compare force bits for OC1A, OC1B, and OC1C
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B) | (1 << FOC1C));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1C) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1C) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer1 module
	PRR &= ~(1 << PRTIM1);

	// set timer 1 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR1A &= ~(1 << WGM11);
	TCCR1A |= (1 << WGM10);
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));

	// set timer 1 prescale factor to 1
	TCCR1B &= ~((1 << CS12) | (1 << CS11));
	TCCR1B |= (1 << CS10);

	// disable timer 1 input capture noise canceler, select timer 1 falling edge for input capture
	TCCR1B &= ~((1 << ICNC1) | (1 << ICES1));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// clear timer 1 output compare force bits for OC1A and OC1B
	TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B));

#ifdef useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A));

	// enable timer1 overflow interrupt
	TIMSK1 |= (1 << TOIE1);
#else // useTimer1Interrupt
	// disable timer 1 interrupts
	TIMSK1 &= ~((1 << ICIE1) | (1 << OCIE1B) | (1 << OCIE1A) | (1 << TOIE1));
#endif // useTimer1Interrupt

	// clear timer 1 interrupt flags
	TIFR1 |= ((1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1));

#endif // defined(__AVR_ATmega328P__)
#endif // useTimer1
#ifdef useTimer2
#if defined(__AVR_ATmega2560__)
	// turn on timer2 module
	PRR0 &= ~(1 << PRTIM2);

	// set timer 2 to 8-bit phase correct PWM mode, TOP = 0xFF
	TCCR2A &= ~(1 << WGM21);
	TCCR2A |= (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// set timer 2 prescale factor to 64
	TCCR2B &= ~((1 << CS22));
	TCCR2B |= ((1 << CS21) | (1 << CS20));

	// set OC2A to disabled
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

	// set OC2B to disabled
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

	// clear timer 2 output compare force bits for OC2A and OC2B
	TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

	// disable timer 2 interrupts
	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

	// clear timer 2 interrupt flags
	TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer2 module
	PRR &= ~(1 << PRTIM2);

	// set timer 2 to phase correct PWM mode, TOP = 0xFF
	TCCR2A &= ~(1 << WGM21);
	TCCR2A |= (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// set timer 2 prescale factor to 64
	TCCR2B &= ~((1 << CS22));
	TCCR2B |= ((1 << CS21) | (1 << CS20));

	// set OC2A to disabled
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

	// set OC2B to disabled
	TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

	// clear force bits for OC2A and OC2B
	TCCR2B &= ~((1 << FOC2A) | (1 << FOC2B));

	// disable timer 2 interrupts
	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));

	// clear timer 2 interrupt flags
	TIFR2 |= ((1 << OCF2B) | (1 << OCF2A) | (1 << TOV2));

#endif // defined(__AVR_ATmega328P__)
#endif // useTimer2
#ifdef useTimer4
#if defined(__AVR_ATmega32U4__)
	// turn on timer4 module
	PRR1 &= ~(1 << PRTIM4);

	// set timer 4 to phase and frequency correct mode
	TCCR4D &= ~(1 << WGM41);
	TCCR4D |= (1 << WGM40);

	// set timer 4 prescale factor to 64
	TCCR4B &= ~(1 << CS43);
	TCCR4B |= ((1 << CS42) | (1 << CS41) | (1 << CS40));

	// clear timer 4 fault protection
	TCCR4D &= ~((1 << FPIE4) | (1 << FPEN4) | (1 << FPNC4) | (1 << FPES4)  | (1 << FPAC4) | (1 << FPF4));

	// set OC4A to disabled
	TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << PWM4A));

	// set OC4B to disabled
	TCCR4A &= ~((1 << COM4B1) | (1 << COM4B0) | (1 << PWM4B));

	// set OC4D to disabled
	TCCR4C &= ~((1 << COM4D1) | (1 << COM4D0) | (1 << PWM4D));

	// clear timer 4 PWM inversion mode
	TCCR4B &= ~(1 << PWM4X);

	// set timer 4 dead time prescaler to 1
	TCCR4B &= ~((1 << DTPS41) | (1 << DTPS40));

	// clear timer 4 output compare force bits for OC4A and OC4B
	TCCR4A &= ~((1 << FOC4A) | (1 << FOC4B));

	// clear timer 4 output compare force bits for OC4D
	TCCR4C &= ~(1 << FOC4D);

	// clear timer 4 update lock, disable timer 4 enhanced compare mode
	TCCR4E &= ~((1 << TLOCK4) | (1 << ENHC4));

	// disable timer 4 interrupts
	TIMSK4 &= ~((1 < OCIE4D) | (1 < OCIE4A) | (1 < OCIE4B) | (1 < TOIE4));

	// clear timer 4 interrupt flags
	TIFR4 |= ((1 << OCF4D) | (1 << OCF4A) | (1 << OCF4B) | (1 << TOV4));

	// set timer 4 dead time to 0
	DT4 = 0;

	// set timer 4 TOP value to 0x00FF, setting 8 bit mode
	TC4H = 0;
	OCR4C = 255;

#endif // defined(__AVR_ATmega32U4__)
#endif // useTimer4
	SREG = oldSREG; // restore interrupt flag status

#ifdef useTWIsupport
	TWI::init();
#ifdef useAdafruitRGBLCDshield
	adafruitRGBLCDsupport::init(); // go init Adafruit RGB LCD shield
#endif // useAdafruitRGBLCDshield
#endif // useTWIsupport
#ifdef useSerial0Port
	serial0::init();
#endif // useSerial0Port
#ifdef useSerial1Port
	serial1::init();
#endif // useSerial1Port
#if defined(__AVR_ATmega32U4__)
	USB::init();
#endif // defined(__AVR_ATmega32U4__)
	input::init();
	LCD::init();
#ifdef useStatusLEDpins
	initStatusLED();
#endif // useStatusLEDpins
#ifdef useExpansionPins
	initExpansionPin();
#endif // useExpansionPins

}

static void peripheral::shutdownMain(void)
{

#ifdef useExpansionPins
	shutdownExpansionPin();
#endif // useExpansionPins
#ifdef useStatusLEDpins
	shutdownStatusLED();
#endif // useStatusLEDpins
	changeBitFlags(timer0Command, t0cDisplayDelay, 0); // cancel any display delays in progress
	LCD::shutdown(); // shut down the LCD display
	input::shutdown();
#if defined(__AVR_ATmega32U4__)
	USB::shutdown();
#endif // defined(__AVR_ATmega32U4__)
#ifdef useSerial1Port
	serial1::shutdown();
#endif // useSerial1Port
#ifdef useSerial0Port
	serial0::shutdown();
#endif // useSerial0Port

#ifdef useTimer4
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM4); // shut off timer4 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)

#endif // useTimer4
#ifdef useTimer2
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM2); // shut off timer2 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer2
#ifdef useTimer1Interrupt
#if defined(__AVR_ATmega32U4__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable timer1 overflow interrupt
	TIMSK1 &= ~(1 << TOIE1);
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer1Interrupt
#ifdef useTimer1
#if defined(__AVR_ATmega32U4__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRTIM1); // shut off timer1 module to reduce power consumption
#endif // defined(__AVR_ATmega328P__)

#endif // useTimer1
}

// this function is needed since there is no way to perform an atomic bit change of an SRAM byte value
// most MPGuino variables that are shared between main program and interrupt handlers should not need to
//    be treated as atomic (!) because only one side or the other is supposed to change said variables
// however, status flag registers are obviously an exception, and status flag changes are common
//    enough to warrant an explicit function definition
static void peripheral::changeBitFlags(volatile uint8_t &flagRegister, uint8_t maskAND, uint8_t maskOR)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	flagRegister = (flagRegister & ~(maskAND)) | (maskOR); // go perform atomic status flag change

	SREG = oldSREG; // restore interrupt flag status

}

#ifdef useExpansionPins
static void peripheral::initExpansionPin(void)
{

#if defined(__AVR_ATmega32U4__)
	// set OC4A to clear-up/set-down PWM mode for EXP1 option pin
	TCCR4A &= ~(1 << COM4A0);
	TCCR4A |= ((1 << COM4A1) | (1 << PWM4A));

	// set OC4D to clear-up/set-down PWM mode for EXP2 option pin
	TCCR4C &= ~(1 << COM4D0);
	TCCR4C |= ((1 << COM4D1) | (1 << PWM4D));

	// enable EXP1 and EXP2 option pin outputs
	DDRC |= (1 << DDC6); // set OC4A as output
	DDRD |= (1 << DDD7); // set OC4D as output

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// set OC1B to clear-up/set-down for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << PB6) | (1 << PB4));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// set OC1B to non-inverting mode for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << PB3) | (1 << PB2));

#endif // defined(__AVR_ATmega328P__)
	outputExpansionPin1(2); // initially set EXP1 option pin to 0
	outputExpansionPin2(2); // initially set EXP2 option pin to 0

}

static void peripheral::shutdownExpansionPin(void)
{

	outputExpansionPin1(2); // set EXP1 option pin to 0
	outputExpansionPin2(2); // set EXP2 option pin to 0

#if defined(__AVR_ATmega32U4__)
	// disable expansion pin output
	DDRC &= ~(1 << DDC6);
	DDRD &= ~(1 << DDD7);

	// set OC4A to disabled for EXP1 option pin
	TCCR4A &= ~((1 << COM4A1) | (1 << COM4A0) | (1 << PWM4A));

	// set OC4D to disabled for EXP2 option pin
	TCCR4C &= ~((1 << COM4D1) | (1 << COM4D0) | (1 << PWM4D));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// disable expansion pin output
	DDRB &= ~((1 << PB6) | (1 << PB4));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable expansion pin output
	DDRB &= ~((1 << PB3) | (1 << PB2));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

#endif // defined(__AVR_ATmega328P__)
}

// sets EXP1 option pin PWM output from 0-100%
static void peripheral::outputExpansionPin1(uint8_t pin)
{

	uint8_t val = SWEET64::runPrgm(prgmCalculateOutputPinValue, pin);

#if defined(__AVR_ATmega32U4__)
	OCR4A = val;
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	OCR1B = val;
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR1B = val;
#endif // defined(__AVR_ATmega328P__)

}

// sets EXP2 option pin PWM output from 0-100%
static void peripheral::outputExpansionPin2(uint8_t pin)
{

	uint8_t val = SWEET64::runPrgm(prgmCalculateOutputPinValue, pin);

#if defined(__AVR_ATmega32U4__)
	OCR4D = val;
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	OCR2A = val;
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR2A = val;
#endif // defined(__AVR_ATmega328P__)

}

#endif // useExpansionPins
#ifdef useStatusLEDpins
static void peripheral::initStatusLED(void)
{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//	DDRB |= (1 << DDB0); // turn on digital output for RX LED
	DDRC |= (1 << DDC7); // turn on digital output for LED L
//	DDRD |= (1 << DDD5); // turn on digital output for TX LED
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for initializing status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	DDRB |= (1 << DDB7); // turn on digital output for LED L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for initializing status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB |= (1 << DDB5); // turn on digital output for LED L
#endif // defined(__AVR_ATmega328P__)

	outputStatusLED(0); // initially turn off status LED

}

static void peripheral::shutdownStatusLED(void)
{

	outputStatusLED(0); // turn off status LED

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//	DDRB &= ~(1 << DDB0); // turn off digital output for RX LED
	DDRC &= ~(1 << DDC7); // turn off digital output for LED L
//	DDRD &= ~(1 << DDD5); // turn off digital output for TX LED
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for turning off status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	DDRB &= ~(1 << DDB7); // turn off digital output for LED L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for turning off status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DDRB &= ~(1 << DDB5); // turn off digital output for LED L
#endif // defined(__AVR_ATmega328P__)

}

static void peripheral::outputStatusLED(uint8_t val)
{

	if (val)
	{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//		PORTB &= ~(1 << PORTB0); // active low RX
		PORTC |= (1 << PORTC7); // active high L
//		PORTD &= ~(1 << PORTD5); // active low TX
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for turning on status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
		PORTB |= (1 << PORTB7); // active high L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for turning on status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB |= (1 << PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}
	else
	{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
//		PORTB |= (1 << PORTB0); // active low RX
		PORTC &= ~(1 << PORTC7); // active high L
//		PORTD |= (1 << PORTD5); // active low TX
#else // useTinkerkitLCDmodule
// insert any other ATmega32U4 port information for turning off status LEDs here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
		PORTB &= ~(1 << PORTB7); // active high L
#else // useArduinoMega2560
// insert any other ATmega2560 port information for turning off status LEDs here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTB &= ~(1 << PORTB5); // active high L
#endif // defined(__AVR_ATmega328P__)

	}

}

#endif // useStatusLEDpins
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

	uint8_t oldSREG;
	uint8_t i;
	uint8_t j;
	uint8_t k;
	uint8_t m;
#ifdef useStatusLEDpins
	uint8_t ledSleepVal = 0;
#endif // useStatusLEDpins
#ifdef useCPUreading
	uint32_t idleStart;
#endif // useCPUreading

#ifdef useStatusLEDpins
	peripheral::outputStatusLED(ledSleepVal);

#endif // useStatusLEDpins
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
#ifdef useStatusLEDpins
	peripheral::outputStatusLED(1 - ledSleepVal);

#endif // useStatusLEDpins
	// this is the part of the main loop that only executes twice a second (or what is defined by loopsPerSecond), to collect and process readings
	if (timer0Status & t0sTakeSample) // if main timer has commanded a sample be taken
	{

		peripheral::changeBitFlags(timer0Status, t0sTakeSample, 0); // acknowledge sample command

#ifdef useCPUreading
		mainProgramVariables[(uint16_t)(mpMainLoopAccumulatorIdx)] = systemInfo::findCycleLength(mainStart, systemInfo::cycles0());
		mainProgramVariables[(uint16_t)(mpIdleAccumulatorIdx)] = idleTimerLength;

		mainStart = systemInfo::cycles0();
		idleTimerLength = 0;

#ifdef useDebugCPUreading
		switch (monitorState)
		{

			case 1:
				idleProcessTimerLength = 0;
				displayTimerLength = 0;
				SWEET64timerLength = 0;

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operations atomic

				volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] = 0;

				SREG = oldSREG; // restore interrupt flag status

				monitorState = 2;
				break;

			case 2:
				mainProgramVariables[(uint16_t)(mpDebugAccMainLoopIdx)] = mainProgramVariables[(uint16_t)(mpMainLoopAccumulatorIdx)];
				mainProgramVariables[(uint16_t)(mpDebugAccIdleIdx)] = mainProgramVariables[(uint16_t)(mpIdleAccumulatorIdx)];
				mainProgramVariables[(uint16_t)(mpDebugAccIdleProcessIdx)] = idleProcessTimerLength;
				mainProgramVariables[(uint16_t)(mpDebugAccDisplayIdx)] = displayTimerLength;
				mainProgramVariables[(uint16_t)(mpDebugAccSWEET64idx)] = SWEET64timerLength;

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operations atomic

				mainProgramVariables[(uint16_t)(mpDebugAccInterruptIdx)] = volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)];

				SREG = oldSREG; // restore interrupt flag status

				monitorState = 0;
				break;

			default:
				monitorState = 0;
				break;

		}

#endif // useDebugCPUreading
#endif // useCPUreading
		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operations atomic

		i = rawTripIdx; // save old trip variable index
		rawTripIdx ^= (raw0tripIdx ^ raw1tripIdx); // set new raw trip variable index

#ifdef trackIdleEOCdata
		j = rawEOCidleTripIdx; // save old EOC/idle trip variable index
		rawEOCidleTripIdx ^= (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set new raw EOC/idle trip variable index

#endif // trackIdleEOCdata
		SREG = oldSREG; // restore interrupt flag status

		for (uint8_t x = 0; x < tUScount; x++)
		{

			k = translateTripIndex(tripUpdateSrcList, x) & 0x7F;
			m = translateTripIndex(tripUpdateDestList, x);

			if (m)
			{

				if (m & 0x80) tripVar::transfer(k, m & 0x7F);
				else tripVar::update(k, m);

			}

		}

		tripVar::reset(i); // reset old raw trip variable
#ifdef trackIdleEOCdata
		tripVar::reset(j); // reset old EOC/idle raw trip variable
#endif // trackIdleEOCdata

		if (awakeFlags & aAwakeOnVehicle)
		{

#ifdef useWindowTripFilter
			if (EEPROM::readVal(pWindowTripFilterIdx))
			{

				wtpCurrentIdx++;
				if (wtpCurrentIdx == windowTripFilterIdx + windowTripFilterSize) wtpCurrentIdx = windowTripFilterIdx;

			}

#endif // useWindowTripFilter
		}

	}

#if defined(useAnalogRead)
#ifdef useChryslerMAPCorrection
	if (analogStatus & asReadMAPchannel)
	{

		peripheral::changeBitFlags(analogStatus, asReadMAPchannel, 0); // acknowledge ADC read completion

		SWEET64::runPrgm(prgmCalculateMAPpressure, 0);

	}

#endif // useChryslerMAPCorrection
#ifdef useChryslerBaroSensor
	if (analogStatus & asReadBaroChannel)
	{

		peripheral::changeBitFlags(analogStatus, asReadBaroChannel, 0); // acknowledge ADC read completion

		SWEET64::runPrgm(prgmCalculateBaroPressure, 0);

	}

#endif // useChryslerBaroSensor
#ifdef useAnalogButtons
	if (analogStatus & asReadButtonChannel)
	{

		peripheral::changeBitFlags(analogStatus, asReadButtonChannel, 0); // acknowledge ADC read completion

		for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
		{

			if (analogValue[(unsigned int)(analogButtonChannelIdx)] >= pgm_read_word(&analogButtonThreshold[(unsigned int)(x)]))
			{

#ifdef useTestAnalogButtonIdx
				thisButtonIdx = x;
#endif //  useTestAnalogButtonIdx
				input::inject(pgm_read_byte(&analogTranslate[(unsigned int)(x)]));
				break;

			}

		}

	}

#endif // useAnalogButtons
#endif // useAnalogRead
#ifdef useDragRaceFunction
	if (timer0Status & t0sAccelTestFlag)
	{

		peripheral::changeBitFlags(timer0Status, t0sAccelTestFlag, 0); // acknowledge sample command

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operations atomic

		accelTestStatus = lastAccelTestStatus; // copy last loop's accel test flag status to this loop
		lastAccelTestStatus = accelerationFlags; // copy current accel test flag status for next loop

		SREG = oldSREG; // restore interrupt flag status

		accelTestStatus = (lastAccelTestStatus ^ accelTestStatus) & accelTestClearFlags; // detect any drag race flag changes

		if (accelTestStatus)
		{

			accelTestState = accelerationFlags & accelTestClearFlags;

			switch (accelTestState)
			{

				case (accelTestTriggered | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
					accelTestState = 1;
					break;

				case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
					accelTestState = 2;
					break;

				case (accelTestFinished):
					if (SWEET64::runPrgm(prgmAccelTestCompareFullSpeeds, 0))
					{

						tripVar::transfer(dragRawHalfSpeedIdx, dragHalfSpeedIdx);
						tripVar::transfer(dragRawFullSpeedIdx, dragFullSpeedIdx);
						tripVar::transfer(dragRawDistanceIdx, dragDistanceIdx);

					}

					accelTestState = 3;
					break;

				case (accelTestFinished | accelTestCancelled):
					accelTestState = 4;
					break;

				case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed):
					accelTestState = 5;
					break;

				case (accelTestActive | accelTestFullSpeed | accelTestDistance):
					accelTestState = 6;
					break;

				case (accelTestActive | accelTestFullSpeed):
					accelTestState = 7;
					break;

				case (accelTestActive | accelTestHalfSpeed | accelTestDistance):
					accelTestState = 8;
					break;

				case (accelTestActive | accelTestHalfSpeed):
					accelTestState = 9;
					break;

				case (accelTestActive | accelTestDistance):
					accelTestState = 10;
					break;

				case (accelTestActive):
					accelTestState = 11;
					break;

				default:
					accelTestState = 12;
					break;

			}

		}
		else
		{

			accelTestState = 0;

			if (accelerationFlags & accelTestFinished)
			{

				if (EEPROM::readVal(pDragAutoFlagIdx))
				{

					if (accelerationTest::triggerTest() == 0) accelTestState = 1;

				}

			}

		}

	}

#endif // useDragRaceFunction
#ifdef useSimulatedFIandVSS
	debugReading::idleProcess();

#endif // useSimulatedFIandVSS
#ifdef useDebugCPUreading
	idleProcessTimerLength += systemInfo::findCycleLength(idleStart, systemInfo::cycles0());

#endif // useDebugCPUreading
}

static void performSleepMode(uint8_t sleepMode)
{

	set_sleep_mode(sleepMode); // set for specified sleep mode
	sleep_enable(); // enable sleep mode
	sleep_mode(); // go sleep for a bit
	sleep_disable(); // disable sleep mode

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

	// timer0 is the taskmaster driving MPGuino's measurement functionality
#if defined(__AVR_ATmega32U4__)
	// turn on timer0 module
	PRR0 &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= (1 << AIN0D);
	DIDR2 |= ((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= ((1 << PRTWI) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRADC));
	PRR1 |= ((1 << PRUSB) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART1));

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	// turn on timer0 module
	PRR0 &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((1 << ADC7D) | (1 << ADC6D) | (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= ((1 << AIN1D) | (1 << AIN0D));
	DIDR2 |= ((1 << ADC15D) | (1 << ADC14D) | (1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D) | (1 << ADC10D) | (1 << ADC9D) | (1 << ADC8D));

	// shut off on-board peripherals to reduce power consumption
	PRR0 |= ((1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC));
	PRR1 |= ((1 << PRTIM5) | (1 << PRTIM4) | (1 << PRTIM3) | (1 << PRUSART3) | (1 << PRUSART2) | (1 << PRUSART1));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// turn on timer0 module
	PRR &= ~(1 << PRTIM0);

	// set timer 0 to fast PWM mode, TOP = 0xFF
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	// set timer 0 prescale factor to 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC0B to disabled
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));

	// clear timer 0 output compare force bits for OC0A and OC0B
	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	// disable timer 0 output compare interrupts
	TIMSK0 &= ~((1 << OCIE0B) | (1 << OCIE0A));

	// enable timer 0 overflow interrupt to generate ~1 ms tick
	TIMSK0 |= (1 << TOIE0);

	// clear timer 0 interrupt flags
	TIFR0 |= ((1 << OCF0B) | (1 << OCF0A) | (1 << TOV0));

	// disable digital inputs for all ADC capable pins to reduce power consumption
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D));
	DIDR1 |= ((1 << AIN1D) | (1 << AIN0D));

	// shut off on-board peripherals to reduce power consumption
	PRR |= ((1 << PRTWI) | (1 << PRTIM2) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) | (1 << PRADC));

#endif // defined(__AVR_ATmega328P__)
	ACSR &= ~(1 << ACIE); // disable analog comparator interrupt
	ACSR |= (1 << ACD); // disable analog comparator module
	ADCSRB &= ~(1 << ACME); // disable analog comparator multiplexer

	timer0Command = t0cResetTimer;
#ifdef useTimer1Interrupt
	timer1Command = t1cResetTimer;
#endif // useTimer1Interrupt

#ifdef useSimulatedFIandVSS
	debugFlags = (debugVSSflag | debugInjectorFlag);
	debugReading::configurePorts();

#endif // useSimulatedFIandVSS
	rawTripIdx = raw0tripIdx;
#ifdef trackIdleEOCdata
	rawEOCidleTripIdx = raw0eocIdleTripIdx;
#endif // trackIdleEOCdata

	tripVar::init();

#ifdef useDragRaceFunction
	accelerationFlags &= ~accelTestClearFlags;
	lastAccelTestStatus = accelerationFlags;
	accelTestStatus = accelerationFlags;

#endif // useDragRaceFunction
	menuLevel = 255;
	topScreenLevel = 0;

	j = EEPROM::powerUpCheck();

	peripheral::initMain(); // initialize all human interface peripherals

	timer0DelayCount = delay1500msTick; // request a set number of timer tick delays equivalent to 1.5 seconds
	timer0Command |= (t0cDoDelay); // signal request to timer

	sei();

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
	while (timer0Command & t0cDoDelay) idleProcess(); // show splash screen for 1.5 seconds

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

			peripheral::changeBitFlags(activityChangeFlags, afActivityTimeoutFlag, 0); // clear activity change activity timeout flag

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

			peripheral::changeBitFlags(activityChangeFlags, afEngineOffFlag, 0); // clear activity change engine off flag

			if (((activityFlags & afEngineOffFlag) == 0) && (EEPROM::readVal(pWakeupResetCurrentOnEngineIdx))) tripSupport::resetCurrent(); // if engine start is detected

		}

		if (activityChangeFlags & afVehicleStoppedFlag)
		{

			peripheral::changeBitFlags(activityChangeFlags, afVehicleStoppedFlag, 0); // clear activity change vehicle stopped flag

			if (((activityFlags & afVehicleStoppedFlag) == 0) && (EEPROM::readVal(pWakeupResetCurrentOnMoveIdx))) tripSupport::resetCurrent(); // if vehicle movement is detected

		}

#if useWindowTripFilter || useSavedTrips
		if (activityChangeFlags & afParkFlag)
		{

			peripheral::changeBitFlags(activityChangeFlags, afParkFlag, 0); // clear activity change park flag

			if (activityFlags & afParkFlag) // if MPGuino is commanded to go park
			{

#ifdef useWindowTripFilter
				tripVar::resetWindowFilter(); // reset the window trip filter

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
			peripheral::changeBitFlags(timer0Status, t0sReadButton, 0); // acknowledge sample command

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

			peripheral::changeBitFlags(timer0Status, t0sOutputLogging, 0); // acknowledge output serial command

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

			peripheral::changeBitFlags(timer0Status, t0sUpdateDisplay, 0); // acknowledge update display command

#ifdef useExpansionPins
			peripheral::outputExpansionPin1(0);
			peripheral::outputExpansionPin2(1);

#endif // useExpansionPins
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
