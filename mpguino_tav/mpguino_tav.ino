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

  MPGuino Colour Touch by abbalooga
    injector sense open  PE4 (INT4), Digital 2
    injector sense close PE5 (INT5), Digital 3
    speed                PK0 (PCINT16), A8
    Valt                 PK1 (ADC9), A9

-------------------------------------
configuration for alternator voltage input to MPGuino (via meelis11)

                     o------------|<|----------------o V(alternator)
                     |            1N4004
                     |
                     |
                     |  R6      R5
                     o--VVV--o--VVV------------------o ground
                             |
                             |
                             |                         R6    (JBD)
                             o-----------------------o PC2 - legacy MPGuino hardware
                                                       PF2 - Arduino Mega 2560
R5             R6                                      PF6 - TinkerKit! LCD module
9.6K           27K - original meelis11                 PK9 - MPGuino Colour Touch by abbalooga
10K            51K - MPGuino Colour Touch

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

TFT Pins
  MPGuino Colour Touch by abbalooga
    CS         PL2, Digital 47
    RESET      PL5, Digital 44
    DC         PL1, Digital 48
    SDI (MOSI) PB2, Digital 51
    SCK        PB1, Digital 52
    LED        PB6, Digital 12, controlled by PWM on OC1B

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

Touchscreen Pins
  MPGuino Colour Touch by abbalooga
    T_CLK      PB5, Digital 11, software SPI
    T_CS       PB4, Digital 10
    T_DIN      PH6, Digital 9, software SPI
    T_DO       PH5, Digital 8, software SPI
    T_IRQ      PH4, Digital 7

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

Bluetooth I/O
  MPGuino Colour Touch by abbalooga
    RX      D3 (TXD1), Digital 18   (USART1)
    TX      D2 (RXD1), Digital 19

-------------------------------------

Logging Output / Debug Monitor I/O
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1     (USART)
    TX      D0 (RXD), Digital 0

  Arduino Mega 2560
    RX      E1 (TXD0), Digital 1    (USART0)
    TX      E0 (RXD0), Digital 0
    - or -
    RX      D3 (TXD1), Digital 18   (USART1)
    TX      D2 (RXD1), Digital 19

  TinkerKit! LCD module
    D-                              (USB)
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

#define tcEOS		"\0"
#define tcEOSCR		"\r"
#define tcLCDHI		"\x0F"
#define tcLCDMED	"\x10"
#define tcLCDLO		"\x11"
#define tcLCDOFF	"\x12"
#define tcOMOFF		"\xEB"
#define tcOTOG		"\xEC"
#define tcOON		"\xED"
#define tcOOFF		"\xEE"
#define tcCR		"\xEF"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/EEPROM.h>
#include <avr/sleep.h>

static const char titleMPGuino[] PROGMEM = {
	tcOON "MPGuino v1.95tav" tcEOSCR
};

static const char dateMPGuino[] PROGMEM = {
	"2023-JUN-29" tcEOSCR
};

#include "configs.h"

static const uint8_t mainDisplayPageCount = 9			// count of base number of data screens
#if defined(trackIdleEOCdata)
	+ 3													// count of Idle/EOC tracking data screens
#endif // defined(trackIdleEOCdata)
;
static const uint8_t mainDisplayFormatSize = mainDisplayPageCount * 4;

#include "heart.h"
#include "parameters.h"
#include "trip_measurement.h"
#include "sweet64.h"
#include "m_serial.h"
#include "m_analog.h"
#include "m_twi.h"
//#include "m_usb.h"
#include "m_lcd.h"
#include "m_tft.h"
#include "m_button.h"
#include "functions.h"
#include "text.h"
#include "feature_base.h"
#include "feature_settings.h"
#include "feature_datalogging.h"
#include "feature_bignum.h"
#include "feature_bargraph.h"
#include "feature_outputpin.h"
#include "feature_debug.h"
#include "feature_dragrace.h"
#include "feature_coastdown.h"

namespace menu /* Top-down menu selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void entry(void);
	static void select(void);

};

namespace cursor /* LCD screen cursor manipulation section prototype */
{

	static void screenLevelEntry(const char * str, uint8_t newScreenLevel);
	static void screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel);
	static void moveAbsolute(uint8_t positionY, uint8_t positionX);
	static void moveRelative(uint8_t moveY, uint8_t moveX);
	static void updateAfterMove(uint8_t levelChangeFlag);

};

static void idleProcess(void); // place all time critical main program internal functionality here - no I/O!
int main(void);

// Menu display / screen cursor support section

typedef uint8_t (* displayHandlerFunc)(uint8_t, uint8_t); // type for various handler functions

typedef struct
{

	uint8_t menuIndex;
	uint8_t baseDisplayIdx;
	uint8_t displayGroupCount;
	uint8_t modeXcount;
	displayHandlerFunc displayHandler;

} displayData;

uint8_t menuLevel;
uint8_t menuTop;

const uint8_t menuExitIdx =				0;								// this call may not even be necessary
const uint8_t menuEntryIdx =			menuExitIdx + 1;				// typically, this call will fall through
const uint8_t menuCursorUpdateIdx =		menuEntryIdx + 1;				// ...to this call, then will fall through
const uint8_t menuOutputDisplayIdx =	menuCursorUpdateIdx + 1;		// ...to this call

// the following display index defines are for the legacy MPGuino displays

#define nextAllowedValue 0
static const uint8_t mainDisplayIdx =				nextAllowedValue;
#define nextAllowedValue mainDisplayIdx + 1
#if defined(useStatusBar)
static const uint8_t statusBarIdx =					nextAllowedValue;
#define nextAllowedValue statusBarIdx + 1
#endif // defined(useStatusBar)
#ifdef useBigFE
static const uint8_t bigFEdisplayIdx =				nextAllowedValue;
#define nextAllowedValue bigFEdisplayIdx + 1
#endif // useBigFE
#ifdef useBarFuelEconVsTime
static const uint8_t barFEvTdisplayIdx =			nextAllowedValue;
#define nextAllowedValue barFEvTdisplayIdx + 1
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
static const uint8_t barFEvSdisplayIdx =			nextAllowedValue;
#define nextAllowedValue barFEvSdisplayIdx + 1
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
static const uint8_t bigDTEdisplayIdx =				nextAllowedValue;
#define nextAllowedValue bigDTEdisplayIdx + 1
#endif // useBigDTE
#ifdef useBigTTE
static const uint8_t bigTTEdisplayIdx =				nextAllowedValue;
#define nextAllowedValue bigTTEdisplayIdx + 1
#endif // useBigTTE
#ifdef useCPUreading
static const uint8_t CPUmonDisplayIdx =				nextAllowedValue;
#define nextAllowedValue CPUmonDisplayIdx + 1
#endif // useCPUreading
#ifdef useClockDisplay
static const uint8_t clockShowDisplayIdx =			nextAllowedValue;
#define nextAllowedValue clockShowDisplayIdx + 1
#endif // useClockDisplay

static const uint8_t displayCountUser =				nextAllowedValue;	// this variable is used to figure out how many menu levels the user display section has

// the following display index defines are for the parameter setting display, and any other extra features that have been added
static const uint8_t displaySettingsDisplayIdx =	nextAllowedValue;
static const uint8_t fuelSettingsDisplayIdx =		displaySettingsDisplayIdx + 1;
static const uint8_t VSSsettingsDisplayIdx =		fuelSettingsDisplayIdx + 1;
static const uint8_t tankSettingsDisplayIdx =		VSSsettingsDisplayIdx + 1;
#define nextAllowedValue tankSettingsDisplayIdx + 1
#ifdef useChryslerMAPCorrection
static const uint8_t CRFICsettingsDisplayIdx =		nextAllowedValue;
#define nextAllowedValue CRFICsettingsDisplayIdx + 1
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
static const uint8_t acdSettingsDisplayIdx =		nextAllowedValue;
#define nextAllowedValue acdSettingsDisplayIdx + 1
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
static const uint8_t timeoutSettingsDisplayIdx =	nextAllowedValue;
static const uint8_t miscSettingsDisplayIdx =		timeoutSettingsDisplayIdx + 1;
#define nextAllowedValue miscSettingsDisplayIdx + 1

#ifdef useDragRaceFunction
static const uint8_t dragRaceIdx =					nextAllowedValue;
#define nextAllowedValue dragRaceIdx + 1
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const uint8_t coastdownIdx =					nextAllowedValue;
#define nextAllowedValue coastdownIdx + 1
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
static const uint8_t signalSimDisplayIdx =			nextAllowedValue;
#define nextAllowedValue signalSimDisplayIdx + 1
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
static const uint8_t pressureDisplayIdx =			nextAllowedValue;
#define nextAllowedValue pressureDisplayIdx + 1
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
static const uint8_t analogDisplayIdx =				nextAllowedValue;
#define nextAllowedValue analogDisplayIdx + 1
#endif // useDebugAnalog
#ifdef useTestButtonValues
static const uint8_t buttonDisplayIdx =				nextAllowedValue;
#define nextAllowedValue buttonDisplayIdx + 1
#endif // useTestButtonValues

static const char displayCountVisible =				nextAllowedValue;	// this variable is used to figure out how many total menu levels are visible

// the following screen index defines do not show up in the top-down menu list
const uint8_t menuDisplayIdx =						nextAllowedValue;
const uint8_t parameterEditDisplayIdx =				menuDisplayIdx + 1;
#define nextAllowedValue parameterEditDisplayIdx + 1
#ifdef useClockDisplay
const uint8_t clockSetDisplayIdx =					nextAllowedValue;
#define nextAllowedValue clockSetDisplayIdx + 1
#endif // useClockDisplay
#ifdef useSavedTrips
static const uint8_t tripSaveDisplayIdx =			nextAllowedValue;
#define nextAllowedValue tripSaveDisplayIdx + 1
#endif // useSavedTrips
#ifdef usePartialRefuel
static const uint8_t partialRefuelDisplayIdx =		nextAllowedValue;
#define nextAllowedValue partialRefuelDisplayIdx + 1
#endif // usePartialRefuel
#if defined(useScreenEditor)
const uint8_t displayEditDisplayIdx =				nextAllowedValue;
#define nextAllowedValue displayEditDisplayIdx + 1
#endif // defined(useScreenEditor)

static const char displayCountTotal =				nextAllowedValue;

static const char menuTitles[] PROGMEM = {	// each title must be no longer than 15 characters
	"Main Display" tcEOSCR
#if defined(useStatusBar)
	"(trip)vsINST FE" tcEOSCR
#endif // defined(useStatusBar)
#ifdef useBigFE
	"Big FuelEcon" tcEOSCR
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	"FE/Time" tcEOSCR
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	"FE/Speed" tcEOSCR
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	"Big DistToE" tcEOSCR
#endif // useBigDTE
#ifdef useBigTTE
	"Big TimeToE" tcEOSCR
#endif // useBigTTE
#ifdef useCPUreading
	"CPU Info" tcEOSCR
#endif // useCPUreading
#ifdef useClockDisplay
	"Clock" tcEOSCR
#endif // useClockDisplay

	"Settings Disp" tcEOSCR
	"Settings Inj" tcEOSCR
	"Settings VSS" tcEOSCR
	"SettingsTankQty" tcEOSCR
#ifdef useChryslerMAPCorrection
	"SettingsChryslr" tcEOSCR
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	"SettingsVehTest" tcEOSCR
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	"SettingsTimeout" tcEOSCR
	"Settings Misc" tcEOSCR
#ifdef useDragRaceFunction
	"Accel Test" tcEOSCR
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	"Coastdown" tcEOSCR
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	"Sensor Sim" tcEOSCR
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	"Pressures" tcEOSCR
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	"ADC readings" tcEOSCR
#endif // useDebugAnalog
#ifdef useTestButtonValues
	"Button Values" tcEOSCR
#endif // useTestButtonValues
};

static uint8_t displayCursor[(uint16_t)(displayCountTotal)];

static const displayData displayParameters[(uint16_t)(displayCountTotal)] PROGMEM = {

// the following screen entries are in the top-down menu list

	 {mainDisplayIdx,				mainDisplayIdx,				displayCountUser,	mainDisplayPageCount,		mainDisplay::displayHandler}
#if defined(useStatusBar)
	,{statusBarIdx,					mainDisplayIdx,				displayCountUser,	2,							statusBar::displayHandler}
#endif // defined(useStatusBar)
#ifdef useBigFE
	,{bigFEdisplayIdx,				mainDisplayIdx,				displayCountUser,	3,							bigDigit::displayHandler}
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,{barFEvTdisplayIdx,			mainDisplayIdx,				displayCountUser,	4,							barGraphSupport::displayHandler}
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,{barFEvSdisplayIdx,			mainDisplayIdx,				displayCountUser,	3,							barGraphSupport::displayHandler}
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,{bigDTEdisplayIdx,				mainDisplayIdx,				displayCountUser,	3,							bigDigit::displayHandler}
#endif // useBigDTE
#ifdef useBigTTE
	,{bigTTEdisplayIdx,				mainDisplayIdx,				displayCountUser,	3,							bigDigit::displayHandler}
#endif // useBigTTE
#ifdef useCPUreading
	,{CPUmonDisplayIdx,				mainDisplayIdx,				displayCountUser,	1,							systemInfo::displayHandler}
#endif // useCPUreading
#ifdef useClockDisplay
	,{clockShowDisplayIdx,			mainDisplayIdx,				displayCountUser,	1,							bigDigit::displayHandler}
#endif // useClockDisplay
	,{displaySettingsDisplayIdx,	displaySettingsDisplayIdx,	1,					eePtrSettingsDispLen,		settings::displayHandler}
	,{fuelSettingsDisplayIdx,		fuelSettingsDisplayIdx,		1,					eePtrSettingsInjLen,		settings::displayHandler}
	,{VSSsettingsDisplayIdx,		VSSsettingsDisplayIdx,		1,					eePtrSettingsVSSlen,		settings::displayHandler}
	,{tankSettingsDisplayIdx,		tankSettingsDisplayIdx,		1,					eePtrSettingsTankLen,		settings::displayHandler}
#ifdef useChryslerMAPCorrection
	,{CRFICsettingsDisplayIdx,		CRFICsettingsDisplayIdx,	1,					eePtrSettingsCRFIClen,		settings::displayHandler}
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,{acdSettingsDisplayIdx,		acdSettingsDisplayIdx,		1,					eePtrSettingsACDlen,		settings::displayHandler}
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,{timeoutSettingsDisplayIdx,	timeoutSettingsDisplayIdx,	1,					eePtrSettingsTimeoutLen,	settings::displayHandler}
	,{miscSettingsDisplayIdx,		miscSettingsDisplayIdx,		1,					eePtrSettingsMiscLen,		settings::displayHandler}
#ifdef useDragRaceFunction
	,{dragRaceIdx,	1,	4,	accelerationTest::goDisplay,	button::doNothing}
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,{coastdownIdx,	1,	3,	coastdown::goDisplay,	button::doNothing}
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,{signalSimDisplayIdx,			signalSimDisplayIdx,		1,					4,							signalSim::displayHandler}
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	,{pressureDisplayIdx,			pressureDisplayIdx,			1,					1,							pressureCorrect::displayHandler}
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	,{analogDisplayIdx,				analogDisplayIdx,			1,					1,							analogReadViewer::displayHandler}
#endif // useDebugAnalog
#ifdef useTestButtonValues
	,{buttonDisplayIdx,				buttonDisplayIdx,			1,					1,							buttonView::displayHandler}
#endif // useTestButtonValues

// the following screen entries do not show up in the top-down menu list

	,{0,							menuDisplayIdx,				1,					displayCountVisible,		menu::displayHandler}
	,{0,							parameterEditDisplayIdx,	1,					12,							parameterEdit::displayHandler}
#ifdef useClockDisplay
	,{0,							clockSetDisplayIdx,			1,					4,							clockSet::displayHandler}
#endif // useClockDisplay
#ifdef useSavedTrips
	,{0,							tripSaveDisplayIdx,			1,					3,							tripSupport::displayHandler}
#endif // useSavedTrips
#ifdef usePartialRefuel
	,{0,							partialRefuelDisplayIdx,	1,					3,							partialRefuel::displayHandler}
#endif // usePartialRefuel
#if defined(useScreenEditor)
	,{0,							displayEditDisplayIdx,		1,					8,							displayEdit::displayHandler}
#endif // defined(useScreenEditor)
};

static uint8_t menu::displayHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t i;
	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuExitIdx:
			break;

		case menuEntryIdx:
			if (menuTop >= displayCountVisible) menuTop = cursorPos;
		case menuCursorUpdateIdx: // menu display window is (menuTop + LCDcharHeight - 1) with wraparound
			if (cursorPos < menuTop)
			{

				i = menuTop - cursorPos;
				if (i == 1) menuTop = cursorPos; // if cursor is just above menuTop, align menuTop to cursor
				else cursorPos += displayCountVisible; // pretend that cursor wraparound did not occur

			}

			if (cursorPos > menuTop)
			{

				i = cursorPos - menuTop; // determine relative cursor position inside display window
				if (i >= LCDcharHeight) menuTop = cursorPos - LCDcharHeight + 1; // if cursor position is outside the window, move the window top

				if (cursorPos >= displayCountVisible) cursorPos -= displayCountVisible; // stop pretending that cursor wraparound did not occur

			}

		case menuOutputDisplayIdx:
			for (uint8_t x = 0; x < LCDcharHeight; x++)
			{

				i = menuTop + x;

				if (i >= displayCountVisible) i -= displayCountVisible;

				text::gotoXY(devLCD, 0, x);
				text::charOut(devLCD, ((i == cursorPos) ? '>' : ' ' ));
				text::stringOut(devLCD, menuTitles, i);

			}
			break;

		default:
			break;

	}

	return retVal;

}

static void menu::entry(void)
{

	cursor::moveAbsolute(menuDisplayIdx, 255); // go to the menu screen level

}

static void menu::select(void)
{

	uint8_t i;

	i = displayCursor[(unsigned int)(menuDisplayIdx)];

	cursor::moveAbsolute(pgm_read_byte(&displayParameters[(unsigned int)(i)].menuIndex), 255);

}

/* LCD screen cursor manipulation section */

static void cursor::screenLevelEntry(const char * str, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	text::statusOut(devLCD, str);

}

static void cursor::screenLevelEntry(const char * str, uint8_t strIdx, uint8_t newScreenLevel)
{

	moveAbsolute(newScreenLevel, 255);
	text::statusOut(devLCD, str, strIdx);

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

	positionY = pgm_read_byte(&displayParameters[(unsigned int)(menuLevel)].modeXcount);

	switch (positionX)
	{

		case 255:
			break;

		case 254:
			displayCursor[(unsigned int)(menuLevel)] = positionX - 1;
			break;

		default:
			if (positionX < positionY) displayCursor[(unsigned int)(menuLevel)] = positionX;
			break;

	}

	updateAfterMove(levelChangeFlag);

}

static void cursor::moveRelative(uint8_t moveY, uint8_t moveX)
{

	uint8_t wrapAroundFlag;
	uint8_t cursorXdirection;
	uint8_t levelChangeFlag;
	uint8_t v;
	uint8_t maxVal;
	uint8_t x; // base menu level

	x = (pgm_read_byte(&displayParameters[(unsigned int)(menuLevel)].baseDisplayIdx)); // base menu level
	levelChangeFlag = 0;

	wrapAroundFlag = 0; // initially, signal that no wrap-around occurred

	if (moveX) // if movement within the screen level is called for
	{

		if (moveX & 0x80) cursorXdirection = 0xFF;
		else cursorXdirection = 0x01;

		maxVal = pgm_read_byte(&displayParameters[(unsigned int)(menuLevel)].modeXcount); // "horizontal" size

		v = displayCursor[(unsigned int)(menuLevel)] + cursorXdirection; // perform move

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

		displayCursor[(unsigned int)(menuLevel)] = v;

	}

	if (moveY)
	{

		if (moveY & 0x80) moveY = 0xFF;
		else moveY = 0x01;

		maxVal = pgm_read_byte(&displayParameters[(unsigned int)(menuLevel)].displayGroupCount); // "vertical" size

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
			else v = pgm_read_byte(&displayParameters[(unsigned int)(menuLevel)].modeXcount) - 1; // else, set current cursor to end of new menu level

			displayCursor[(unsigned int)(menuLevel)] = v;

		}

	}

	updateAfterMove(levelChangeFlag);

}

static void cursor::updateAfterMove(uint8_t levelChangeFlag)
{

	uint8_t cp;
	uint8_t cf;

	cp = displayCursor[(unsigned int)(menuLevel)];

	// call indexed support section cursor update function to update any section-specific data
	if (levelChangeFlag) cf = menuEntryIdx;
	else cf = menuCursorUpdateIdx;

	((displayHandlerFunc)pgm_read_word(&displayParameters[(unsigned int)(menuLevel)].displayHandler))(cf, cp);

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

		signalSim::idleProcessFuel();

	}

	if (timer1Status & t1sDebugUpdateVSS) // if VSS pulse period needs adjustment
	{

		changeBitFlags(timer1Status, t1sDebugUpdateVSS, 0); // acknowledge debug update request

		signalSim::idleProcessVSS();

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

	cli(); // disable interrupts while interrupts are being fiddled with

	initCore(); // go initialize core MPGuino functionality

	j = EEPROM::powerUpCheck();

	tripSupport::init();

#ifdef useSimulatedFIandVSS
	debugFlags = (debugVSSflag | debugInjectorFlag);
	signalSim::configurePorts();

#endif // useSimulatedFIandVSS
#ifdef useDragRaceFunction
	accelerationFlags &= ~accelTestClearFlags;
	lastAccelTestStatus = accelerationFlags;
	accelTestStatus = accelerationFlags;

#endif // useDragRaceFunction
	menuLevel = mainDisplayIdx;
	menuTop = 255;
	topScreenLevel = 0;

	sei();

	timer0DelayCount = delay1500msTick; // request a set number of timer tick delays equivalent to 1.5 seconds
	timer0Command |= (t0cDoDelay); // signal request to timer

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
	cursor::moveAbsolute(buttonDisplayIdx, 0);
#else // useTestButtonValues
#if defined(useLCDcontrast)
	if (j) cursor::moveAbsolute(displaySettingsDisplayIdx, (pContrastIdx - eePtrSettingsStart)); // go through the initialization screen
#else // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
	if (j) cursor::moveAbsolute(displaySettingsDisplayIdx, (pLCDcolorIdx - eePtrSettingsStart)); // go through the initialization screen
#else // defined(useAdafruitRGBLCDshield)
	if (j) cursor::moveAbsolute(displaySettingsDisplayIdx, (pMetricModeIdx - eePtrSettingsStart)); // go through the initialization screen
#endif // defined(useAdafruitRGBLCDshield)
#endif // defined(useLCDcontrast)
	else cursor::moveAbsolute(mainDisplayIdx, 0);
#endif // useTestButtonValues

#ifdef useSavedTrips
	if (i) text::statusOut(devLCD, PSTR("AutoRestore Done"));

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
				bigDigit::displayHandler(menuEntryIdx, 255); // initialize the software clock
				text::charOut(devLCD, 0x12); // set backlight brightness to zero

#else // useClockDisplay
#if defined(useLCDoutput)
				LCD::shutdown(); // shut down the LCD display

#endif // defined(useLCDoutput)
#if defined(useTFToutput)
				TFT::shutdown(); // shut down the TFT display

#endif // defined(useTFToutput)
#endif // useClockDisplay
#endif // useDeepSleep
			}
			else // if MPGuino is commanded to wake up
			{

#if defined(useLCDoutput)
				LCD::init(); // re-initialize LCD device
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
				TFT::init(); // re-initialize TFT device
#endif // defined(useTFToutput)
				((displayHandlerFunc)pgm_read_word(&displayParameters[(unsigned int)(menuLevel)].displayHandler))(menuEntryIdx, displayCursor[(unsigned int)(menuLevel)]); // call indexed support section screen initialization function

			}

		}

		if (activityChangeFlags & afEngineOffFlag)
		{

			changeBitFlags(activityChangeFlags, afEngineOffFlag, 0); // clear activity change engine off flag

			if (((activityFlags & afEngineOffFlag) == 0) && (EEPROM::readByte(pWakeupResetCurrentOnEngineIdx))) tripSupport::resetCurrent(); // if engine start is detected

		}

		if (activityChangeFlags & afVehicleStoppedFlag)
		{

			changeBitFlags(activityChangeFlags, afVehicleStoppedFlag, 0); // clear activity change vehicle stopped flag

			if (((activityFlags & afVehicleStoppedFlag) == 0) && (EEPROM::readByte(pWakeupResetCurrentOnMoveIdx))) tripSupport::resetCurrent(); // if vehicle movement is detected

		}

#if defined(useWindowTripFilter) || defined(useSavedTrips)
		if (activityChangeFlags & afParkFlag)
		{

			changeBitFlags(activityChangeFlags, afParkFlag, 0); // clear activity change park flag

			if (activityFlags & afParkFlag) // if MPGuino is commanded to go park
			{

#if defined(useWindowTripFilter)
				tripSupport::resetWindowFilter(); // reset the window trip filter

#endif // defined(useWindowTripFilter)
#if defined(useSavedTrips)
				if (tripSupport::doAutoAction(0)) text::statusOut(devLCD, PSTR("AutoSave Done"));

#endif // defined(useSavedTrips)
			}

		}

#endif // defined(useWindowTripFilter) || defined(useSavedTrips)
#ifdef useDebugTerminal
		terminal::mainProcess();

#endif // useDebugTerminal
#if defined(useButtonInput)
		if (timer0Status & t0sReadButton) // see if any buttons were pressed, process related button function if so
		{

			changeBitFlags(timer0Status, t0sReadButton, 0); // acknowledge sample command

			button::doCommand(); // go perform button action

		}

#endif // defined(useButtonInput)
#if useDataLoggingOutput || useJSONoutput
		// this part of the main loop handles logging data output
		if (timer0Status & t0sOutputLogging)
		{

			changeBitFlags(timer0Status, t0sOutputLogging, 0); // acknowledge output serial command

#ifdef useDataLoggingOutput
			if (EEPROM::readByte(pSerialDataLoggingIdx)) doOutputDataLog();

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

			text::gotoXY(devLCD, 0, 0);
			switch (i)
			{

				case (afVehicleStoppedFlag | afEngineOffFlag | afUserInputFlag | afParkFlag): // engine stopped, vehicle stopped, no buttons pressed for a while
				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag): // engine stopped, vehicle stopped for a while
				case (afVehicleStoppedFlag | afUserInputFlag): // vehicle stopped, button not pressed
				case (afEngineOffFlag | afUserInputFlag): // engine stopped, button not pressed
				case (afVehicleStoppedFlag): // vehicle stopped
				case (afEngineOffFlag): // engine stopped
				case (afVehicleStoppedFlag | afEngineOffFlag): // engine stopped, vehicle stopped
				case (afVehicleStoppedFlag | afEngineOffFlag | afUserInputFlag): // engine stopped, vehicle stopped, button not pressed
				case 0: // engine running and vehicle in motion
				case (afUserInputFlag): // engine running, vehicle in motion, button not pressed
					((displayHandlerFunc)pgm_read_word(&displayParameters[(unsigned int)(menuLevel)].displayHandler))(menuOutputDisplayIdx, displayCursor[(unsigned int)(menuLevel)]); // call indexed support section screen refresh function
					break;

				case (afVehicleStoppedFlag | afEngineOffFlag | afParkFlag | afUserInputFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, activity timeout reached
				case (afVehicleStoppedFlag | afEngineOffFlag | afUserInputFlag | afActivityTimeoutFlag): // engine stopped, vehicle stopped, no buttons pressed, park timeout reached, activity timeout reached
#ifndef useDeepSleep
#ifdef useClockDisplay
					bigDigit::displayHandler(menuOutputDisplayIdx, 255); // display the software clock

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
