//
// MPGuino - open source fuel consumption tracking system
// GPL Software, mass production use rights reserved by opengauge.org
// personal use is perfectly fine
// no warranties expressed or implied
//
// This version of MPGuino can automatically process fuel injection signals for both saturated and
// frequency-modulated peak/hold fuel injectors. There is no need to change any settings in order to
// switch between saturated or frequency-modulated peak/hold fuel injection drivers.
//
// This version of MPGuino does not support analog-style peak/hold fuel injectors, due to the relative
// complexity of having to add many more electrical components to properly detect the peak and hold
// currents for the analog waveform. Look up "jaycar peak hold adapter" on the intertubes, if you want
// more information.

// Special thanks to the good folks at ecomodder.com, ardunio.cc, avrfreaks.net, cadsoft.de, atmel.com,
// and all the folks who donate their time and resources and share their experiences freely

/* External connections:

Legacy MPGuino hardware is defined as anything that MPGuino was originally designed to run on. This includes
   Arduino Duemilanove, Arduino Uno, JellyBeanDriver board, meelis11 board, and any other board based off the
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

  Arduino Mega2560
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
                                                       PF2 - Arduino Mega2560
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
               |               |               |                2.2k   PF1 - Arduino Mega2560
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
               |               |                                2.2k   PF2 - Arduino Mega2560
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

TFT Pins - MPGuino Colour Touch by abbalooga
  Arduino Mega2560
    CS         PL2, Digital 47
    DC         PL1, Digital 48
    LED        PB6, Digital 12
    RESET      PL5, Digital 44
    SDI (MOSI) PB2, Digital 51
    SCK        PB1, Digital 52

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

  Arduino Mega2560
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

  Arduino Mega2560
    RX      E1 (TXD0), Digital 1
    - or -
    RX      D3 (TXD1), Digital 18
    - or -
    RX      H1 (TXD2), Digital 16
    - or -
    RX      J1 (TXD3), Digital 14

LCD Pins - Adafruit RGB LCD Shield
  legacy MPGuino hardware
    SDA     PC4 (SDA), A4
    SCL     PC5 (SCL), A5

  TinkerKit! LCD module
    SDA     PD1 (SDA), Digital 2
    SCL     PD0 (SCL), Digital 3

  Arduino Mega2560
    SCL     PD0 (SCL), Digital 21
    SDA     PD1 (SDA), Digital 20

-------------------------------------

Touchscreen Pins - MPGuino Colour Touch by abbalooga
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

  Arduino Mega2560
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

  Arduino Mega2560
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
                                                                                  PF3 - Arduino Mega2560
                                                                                  PF7 - TinkerKit! LCD module

Buttons - Parallax 5-position switch (diagram based on josemapiro efforts)
          (or any 5-position switch module with 10k pullup resistors on their switches)
  legacy MPGuino hardware
    left, middle, right, extra#1, extra#2 PC3 (ADC3), A3

  TinkerKit! LCD module
    left, middle, right, extra#1, extra#2 PF7 (ADC7), A0

  Arduino Mega2560
    left, middle, right, extra#1, extra#2 PF3 (ADC3), A3

                                                                                  PF3 - Arduino Mega2560
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

  Arduino Mega2560
    SCL     PD0 (SCL), Digital 21
    SDA     PD1 (SDA), Digital 20

-------------------------------------

Realtime Clock - DS1307-based hardware
  legacy MPGuino hardware
    SCL     PC5 (SCL), A5
    SDA     PC4 (SDA), A4

  TinkerKit! LCD module
    SDA     PD1 (SDA), Digital 2
    SCL     PD0 (SCL), Digital 3

  Arduino Mega2560
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

  Arduino Mega2560
    EXP1       PB6, Digital 12, controlled by PWM on OC1B
    EXP2       PB4, Digital 10, controlled by PWM on OC2A

-------------------------------------

Bluetooth I/O
 HM-10
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1     (USART)
    TX      D0 (RXD), Digital 0

  Arduino Mega2560
    RX      E1 (TXD0), Digital 1    (USART0)
    TX      E0 (RXD0), Digital 0
    - or -
    RX      D3 (TXD1), Digital 18   (USART1, MPGuino Colour Touch by abbalooga)
    TX      D2 (RXD1), Digital 19
    - or -
    RX      H1 (TXD2), Digital 16   (USART2)
    TX      H0 (RXD2), Digital 17
    - or -
    RX      J1 (TXD3), Digital 14   (USART3)
    TX      J0 (RXD3), Digital 15

 Adafruit Bluefruit LE Shield
  legacy MPGuino hardware
   SCK      PB5 (SCK), Digital 13
  MISO      PB4 (MISO), Digital 12
  MOSI      PB3 (MOSI), Digital 11
    CS      PB0, Digital 8
   IRQ      PD7, Digital 7
   RST      PD4, Digital 4

  Arduino Mega2560
   SCK      PB1 (SCK), Digital 52
  MISO      PB3 (MISO), Digital 50
  MOSI      PB2 (MOSI), Digital 51
    CS      PH5, Digital 8
   IRQ      PH4, Digital 7
   RST      PG5, Digital 4

  Arduino Leonardo
   SCK      PB1 (SCK)
  MISO      PB3 (MISO)
  MOSI      PB2 (MOSI)
    CS      PB4, Digital 8
   IRQ      PE6, Digital 7
   RST      PD4, Digital 4

-------------------------------------

Logging Output / Debug Monitor I/O
  legacy MPGuino hardware
    RX      D1 (TXD), Digital 1     (USART)
    TX      D0 (RXD), Digital 0

  Arduino Mega2560
    RX      E1 (TXD0), Digital 1    (USART0)
    TX      E0 (RXD0), Digital 0
    - or -
    RX      D3 (TXD1), Digital 18   (USART1)
    TX      D2 (RXD1), Digital 19
    - or -
    RX      H1 (TXD2), Digital 16   (USART2)
    TX      H0 (RXD2), Digital 17
    - or -
    RX      J1 (TXD3), Digital 14   (USART3)
    TX      J0 (RXD3), Digital 15

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
#define tcSPC		"\x01"			// space-run sentinel: followed by count byte, expands to that many spaces
#define tcSP3		tcSPC "\x03"
#define tcSP4		tcSPC "\x04"
#define tcSP6		tcSPC "\x06"
#define tcSP7		tcSPC "\x07"
#define tcSP10		tcSPC "\x0a"
#define tcSP11		tcSPC "\x0b"
#define tcSP15		tcSPC "\x0f"
#define tcSP16		tcSPC "\x10"
#define tcSP17		tcSPC "\x11"
#define tcSP18		tcSPC "\x12"
#define tcSP19		tcSPC "\x13"
#define tcOMOFF		"\xEB"
#define tcOTOG		"\xEC"
#define tcOON		"\xED"
#define tcOOFF		"\xEE"
#define tcCR		"\xEF"

#define tcCG0		"\xF0"
#define tcCG1		"\xF1"
#define tcCG2		"\xF2"
#define tcCG3		"\xF3"
#define tcCG4		"\xF4"
#define tcCG5		"\xF5"
#define tcCG6		"\xF6"
#define tcCG7		"\xF7"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

static const char titleMPGuino[] PROGMEM = {
	tcOON "MPGuino v1.95tav" tcEOSCR
};

static const char dateMPGuino[] PROGMEM = {
	"2026-06-02" tcEOSCR
};

int main(void);

#include "configs.h"
#include "heart.h"
#include "parameters.h"
#include "trip_measurement.h"
#include "sweet64.h"
#include "m_serial.h"
#include "m_spi.h"
#include "m_analog.h"
#include "m_twi.h"
//#include "m_usb.h"
#include "m_lcd.h"
#include "m_tft.h"
#include "m_touch.h"
#include "functions.h"
#include "text.h"
#include "feature_bluetooth.h"
#include "feature_outputpin.h"
#include "feature_debug.h"
#include "feature_settings.h"
#include "feature_settings_tft.h"
#include "feature_datalogging.h"
#include "feature_bignum.h"
#include "feature_bargraph.h"
#include "feature_dragrace.h"
#include "feature_coastdown.h"
#include "feature_base.h"
#include "feature_lcdmain.h"
#include "feature_tftmain.h"
#include "m_button.h"

// primary MPGuino processing routine - overwrites Arduino sketch main if compiled in Arduino IDE

int main(void)
{

#if defined(useSimulatedFIandVSS)
	uint16_t signalSimFIPtickLength; // to force debug injector pulse width to a maximum good engine speed-dependent value
	uint16_t signalSimFIPWopenTickLength;
#endif // defined(useSimulatedFIandVSS)
#if defined(useCPUreading) || defined(useDebugCPUreading)
	uint16_t availableRAMptr;
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
	uint8_t i;
	uint8_t j;
	uint8_t oldSREG;

	cli(); // disable interrupts while interrupts are being fiddled with

	heart::initCore(); // go initialize core MPGuino timers

	EEPROM::powerUpCheck(); // go check, and initialize EEPROM parameter storage if required - also initialize basic MPGuino functionality

	tripSupport::init(); // go initialize trip variable storage

	sei();

	heart::initHardware(); // initialize all human interface peripherals

#if defined(useLCDoutput)
	text::gotoXY(m8DevLCDidx, 0, 0);
	text::stringOut(m8DevLCDidx, titleMPGuino);
	text::stringOut(m8DevLCDidx, dateMPGuino);

	heart::changeBitFlagBits(v8Timer0Status0Idx, 0, t0saDisplayDelayFlags);

#endif // defined(useLCDoutput)
#if defined(useTFToutput) && !defined(useButtonInput)
	// startup splash: standard MPGuino title + date, held for the normal display
	// delay (counted down by the timer ISR), then the main screen takes over below
	text::gotoXY(m8DevTFTidx, 0, 0);
	text::stringOut(m8DevTFTidx, titleMPGuino);
	text::stringOut(m8DevTFTidx, dateMPGuino);

	heart::changeBitFlagBits(v8Timer0Status0Idx, 0, t0saDisplayDelayFlags);

#endif // defined(useTFToutput) && !defined(useButtonInput)
#if defined(outputLoggingSplash)
	text::stringOut(m8DevLogOutputIdx, titleMPGuino);
	text::stringOut(m8DevLogOutputIdx, dateMPGuino);

#endif // defined(outputLoggingSplash)
#if defined(outputDebugTerminalSplash)
	text::stringOut(m8DevDebugTerminalIdx, titleMPGuino);
	text::stringOut(m8DevDebugTerminalIdx, dateMPGuino);

	terminalState = tsInitInput;
	decWindow = 10;

#endif // defined(outputDebugTerminalSplash)
#if defined(useSimulatedFIandVSS)
	signalSim::configurePorts(debugFIsaturatedFlag | debugVSSflag | debugInjectorFlag);

#endif // defined(useSimulatedFIandVSS)
#if defined(useButtonInput)
	// restore cursor positions from EEPROM
	for (uint8_t x = 0; x < displayCountTotal; x++) displayCursor[(uint16_t)(x)] = EEPROM::readByte(x + eePtrDisplayCursorStart);

	// restore menu heights from EEPROM
	for (uint8_t x = 0; x < displayCountMenu; x++) menuHeight[(uint16_t)(x)] = EEPROM::readByte(x + eePtrMenuHeightStart);

	// restore working display index from EEPROM
	workingDisplayIdx = EEPROM::readByte(pDisplayIdx);

#endif // defined(useButtonInput)
#if defined(useSavedTrips)
	i = tripSave::doAutoAction(taaModeRead);

#endif // defined(useSavedTrips)
#if defined(useLCDoutput) || (defined(useTFToutput) && !defined(useButtonInput))
	while (v08(v8Timer0Status0Idx) & t0saDisplayDelayFlags) heart::performSleepMode(SLEEP_MODE_IDLE); // hold the splash for the display delay

#endif // defined(useLCDoutput) || (defined(useTFToutput) && !defined(useButtonInput))

#if defined(useButtonInput)
	// call working display index initialization function
	cursor::updateDisplay(workingDisplayIdx, displayInitialEntryIdx);

#endif // defined(useButtonInput)
#if defined(useTFToutput) && !defined(useButtonInput)
	tftMain::init(); // splash delay elapsed: bring up the main screen
#endif // defined(useTFToutput) && !defined(useButtonInput)
#if defined(useSavedTrips)
#if defined(useLCDoutput)
	if (i) text::statusOut(m8DevLCDidx, PSTR("AutoRestore Done"));

#endif // defined(useLCDoutput)
#endif // defined(useSavedTrips)

#if defined(useCPUreading) || defined(useDebugCPUreading)
	m32(m32DbgWorkingMainStartIdx) = heart::cycles0();

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
	while (true)
	{

#if defined(useCPUreading) || defined(useDebugCPUreading)
		m32(m32CPUworkingLoopStartIdx) = m32(m32DbgWorkingMainStartIdx);
		m32(m32CPUworkingMainStartIdx) = m32(m32DbgWorkingMainStartIdx);

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useActivityLED)
		activityLED::assert(arMainProcess | arMainDevices);

#endif // defined(useActivityLED)
#if defined(useChryslerMAPCorrection)
		if (v08(v8AnalogStatusIdx) & asReadMAPchannel)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			v08(v8AnalogStatusIdx) &= ~(asReadMAPchannel);

			SREG = oldSREG; // restore interrupt flag status

			SWEET64::runPrgm(S64_PRGM_PTR(prgmCalculateMAPpressure), 0);

		}

#endif // defined(useChryslerMAPCorrection)
#if defined(useChryslerBaroSensor)
		if (v08(v8AnalogStatusIdx) & asReadBaroChannel)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			v08(v8AnalogStatusIdx) &= ~(asReadBaroChannel);

			SREG = oldSREG; // restore interrupt flag status

			SWEET64::runPrgm(S64_PRGM_PTR(prgmCalculateBaroPressure), 0);

		}

#endif // defined(useChryslerBaroSensor)
#if defined(useAnalogButtons)
		if (v08(v8AnalogStatusIdx) & asReadButtonChannel)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			v08(v8AnalogStatusIdx) &= ~(asReadButtonChannel);

			SREG = oldSREG; // restore interrupt flag status

			for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
			{

				if (v16(v16AnalogButtonChannelIdx) >= pgm_read_word(&analogButtonThreshold[(uint16_t)(x)]))
				{

					if (v08(v8ButtonStatusIdx) & btnCmdEnableSampling) button::inject(pgm_read_byte(&analogTranslate[(uint16_t)(x)]));

					break;

				}

			}

		}

#endif // defined(useAnalogButtons)
#if defined(useActivityLED)
		activityLED::release(arMainDevices);

#endif // defined(useActivityLED)
#if defined(useDebugCPUreading)
		m32(m32DbgWorkingMainDevicesIdx) += heart::getCycle0Length(m32DbgWorkingMainStartIdx);

#endif // defined(useDebugCPUreading)
#if defined(useActivityLED)
		activityLED::assert(arMainActivity);

#endif // defined(useActivityLED)
		if (v08(v8ActivityChangeIdx) & (afActivityTimeoutFlag | afEngineOffFlag | afVehicleStoppedFlag | afParkFlag))
		{

			if (v08(v8ActivityChangeIdx) & afActivityTimeoutFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				v08(v8ActivityChangeIdx) &= ~(afActivityTimeoutFlag);

				SREG = oldSREG; // restore interrupt flag status

				if (v08(v8ActivityIdx) & afActivityTimeoutFlag) // if MPGuino is commanded to go asleep
				{

#ifdef useDeepSleep
					heart::doGoDeepSleep();

#else // useDeepSleep
#if defined(useClockDisplay)
					clockDisplay::displayHandler(displayInitialEntryIdx, 0); // initialize the software clock

					LCD::setBrightness(0); // set backlight brightness to zero

#else // defined(useClockDisplay)
#if defined(useLCDoutput)
					LCD::shutdown(); // shut down the LCD display

#endif // defined(useLCDoutput)
#if defined(useTFToutput)
					TFT::shutdown(); // shut down the TFT display

#endif // defined(useTFToutput)
#endif // defined(useClockDisplay)
#endif // useDeepSleep
				}
				else // if MPGuino is commanded to wake up
				{

#if defined(useLCDoutput)
					LCD::init(); // re-initialize LCD device
#endif // defined(useLCDoutput)
#if defined(useTFToutput)
					TFT::init(); // full re-init on wake (this panel does not reliably retain GRAM/config through SLPIN sleep)
#endif // defined(useTFToutput)
#if defined(useButtonInput)
					cursor::updateDisplay(workingDisplayIdx, displayInitialEntryIdx); // call indexed support section screen initialization function
#endif // defined(useButtonInput)
#if defined(useTFToutput) && !defined(useButtonInput)
					tftMain::repaint(); // repaint the screen the user was on (dashboard/settings/keypad), not forced back to the dashboard
#endif // defined(useTFToutput) && !defined(useButtonInput)

				}

			}

			if (v08(v8ActivityChangeIdx) & afEngineOffFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				v08(v8ActivityChangeIdx) &= ~(afEngineOffFlag);

				SREG = oldSREG; // restore interrupt flag status

#if defined(useButtonInput)
				// if engine start is detected
				if (((v08(v8ActivityIdx) & afEngineOffFlag) == 0) && (EEPROM::readByte(pWakeupResetCurrentOnEngineIdx))) tripSupport::resetCurrent();

#endif // defined(useButtonInput)
			}

			if (v08(v8ActivityChangeIdx) & afVehicleStoppedFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				v08(v8ActivityChangeIdx) &= ~(afVehicleStoppedFlag);

				SREG = oldSREG; // restore interrupt flag status

#if defined(useButtonInput)
				// if vehicle movement is detected
				if (((v08(v8ActivityIdx) & afVehicleStoppedFlag) == 0) && (EEPROM::readByte(pWakeupResetCurrentOnMoveIdx))) tripSupport::resetCurrent();

#endif // defined(useButtonInput)
#if defined(useDragRaceFunction)
				// if vehicle is stopped
				if ((v08(v8ActivityIdx) & afVehicleStoppedFlag) && (EEPROM::readByte(pDragAutoFlagIdx))) accelerationTest::triggerTest();

#endif // defined(useDragRaceFunction)
			}

			if (v08(v8ActivityChangeIdx) & afParkFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				v08(v8ActivityChangeIdx) &= ~(afParkFlag);

				SREG = oldSREG; // restore interrupt flag status

#if defined(useWindowTripFilter) || defined(useSavedTrips)
				if (v08(v8ActivityIdx) & afParkFlag) // if MPGuino is commanded to go park
				{

#if defined(useButtonInput)
					// save working display index to EEPROM
					if (workingDisplayIdx < displayMaxSavableIdx) EEPROM::writeByte(pDisplayIdx, workingDisplayIdx);

					// save cursor positions to EEPROM
					for (uint8_t x = 0; x < displayCountTotal; x++)
						EEPROM::writeByte(x + eePtrDisplayCursorStart, displayCursor[(uint16_t)(x)]);

					// save menu heights to EEPROM
					for (uint8_t x = 0; x < displayCountMenu; x++)
						EEPROM::writeByte(x + eePtrMenuHeightStart, menuHeight[(uint16_t)(x)]);

#endif // defined(useButtonInput)
#if defined(useWindowTripFilter)
					tripSupport::resetWindowFilter(); // reset the window trip filter

#endif // defined(useWindowTripFilter)
#if defined(useSavedTrips)
					i = tripSave::doAutoAction(taaModeWrite);
#if defined(useLCDoutput)
					if (i) text::statusOut(m8DevLCDidx, PSTR("AutoSave Done"));
#endif // defined(useLCDoutput)

#endif // defined(useSavedTrips)
				}

#endif // defined(useWindowTripFilter) || defined(useSavedTrips)
			}

		}

#if defined(useActivityLED)
		activityLED::release(arMainActivity);

#endif // defined(useActivityLED)
#if defined(useDebugCPUreading)
		m32(m32DbgWorkingMainActivityIdx) += heart::getCycle0Length(m32DbgWorkingMainStartIdx);

#endif // defined(useDebugCPUreading)
#if defined(useActivityLED)
		activityLED::assert(arMainSample);

#endif // defined(useActivityLED)
		// this is the part of the main loop that only executes twice a second (or what is defined by samplesPerSecond), to collect and process readings
		if (v08(v8Timer0Status0Idx) & t0saTakeSample)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			v08(v8Timer0Status0Idx) &= ~(t0saTakeSample);

			oldRawTripIdx = curRawTripIdx; // save old raw trip variable index
			curRawTripIdx ^= (raw0tripIdx ^ raw1tripIdx); // set current raw trip variable index
#if defined(trackIdleEOCdata)
			oldRawEOCidleTripIdx = curRawEOCidleTripIdx; // save old raw EOC/idle trip variable index
			curRawEOCidleTripIdx ^= (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set current raw EOC/idle trip variable index
#endif // defined(trackIdleEOCdata)

			SREG = oldSREG; // restore interrupt flag status

#if defined(useFEvTdata)
			if (v08(v8Timer0Status1Idx) & t0sbResetFEvsTimeTrip) 
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operations atomic

				v08(v8Timer0Status1Idx) &= ~(t0sbResetFEvsTimeTrip);
				i = v08(v8FEvTimeTripIdx);

				SREG = oldSREG; // restore interrupt flag status

				tripVar::reset(i + FEvsTimePeriodIdx); // reset source trip variable

			}

#endif // defined(useFEvTdata)
			for (uint8_t x = 0; x < tripUpdateListSize; x++)
			{

				i = tripSupport::translateTripIndex(x, 0);
				j = tripSupport::translateTripIndex(x, 1);

				if (j & 0x80) tripVar::transfer(i, j & 0x7F); // if transfer bit set, do trip transfer
				else tripVar::update(i, j); // otherwise, just do trip update

			}

			tripVar::reset(oldRawTripIdx); // reset old raw trip variable
#if defined(trackIdleEOCdata)
			tripVar::reset(oldRawEOCidleTripIdx); // reset old raw EOC/idle trip variable
#endif // defined(trackIdleEOCdata)

#if defined(useCPUreading) || defined(useDebugCPUreading)
			if((uint16_t)__brkval == 0) availableRAMptr = ((uint16_t)&availableRAMptr) - ((uint16_t)&__bss_end);
			else availableRAMptr = ((uint16_t)&availableRAMptr) - ((uint16_t)__brkval);

			m32(m32AvailableRAMidx) = availableRAMptr;

#if defined(useDebugCPUreading)
			if (m08(m8PeekFlags) & peekEnableCPUread)
			{

				m32(m32CPUsampledMainLoopIdx) = m32(m32CPUworkingMainLoopIdx);
				m32(m32CPUsampledMainProcessIdx) = m32(m32CPUworkingMainProcessIdx);
				m32(m32CPUsampledIdleProcessIdx) = m32(m32CPUworkingIdleProcessIdx);
				m32(m32DbgSampledMainDevicesIdx) = m32(m32DbgWorkingMainDevicesIdx);
				m32(m32DbgSampledMainActivityIdx) = m32(m32DbgWorkingMainActivityIdx);
				m32(m32DbgSampledMainSampleIdx) = m32(m32DbgWorkingMainSampleIdx);
				m32(m32DbgSampledMainOutputIdx) = m32(m32DbgWorkingMainOutputIdx);
				m32(m32DbgSampledMainOtherIdx) = m32(m32DbgWorkingMainOtherIdx);
				m32(m32DbgSampledS64processIdx) = m32(m32DbgWorkingS64processIdx);

			}
#else // defined(useDebugCPUreading)
			m32(m32CPUsampledMainLoopIdx) = m32(m32CPUworkingMainLoopIdx);
			m32(m32CPUsampledMainProcessIdx) = m32(m32CPUworkingMainProcessIdx);
			m32(m32CPUsampledIdleProcessIdx) = m32(m32CPUworkingIdleProcessIdx);
#endif // defined(useDebugCPUreading)

			m32(m32CPUworkingMainLoopIdx) = 0;
			m32(m32CPUworkingMainProcessIdx) = 0;
			m32(m32CPUworkingIdleProcessIdx) = 0;
#if defined(useDebugCPUreading)
			m32(m32DbgWorkingMainDevicesIdx) = 0;
			m32(m32DbgWorkingMainActivityIdx) = 0;
			m32(m32DbgWorkingMainSampleIdx) = 0;
			m32(m32DbgWorkingMainOutputIdx) = 0;
			m32(m32DbgWorkingMainOtherIdx) = 0;
			m32(m32DbgWorkingS64processIdx) = 0;
#endif // defined(useDebugCPUreading)

#if defined(useDebugCPUreading)
			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			if (m08(m8PeekFlags) & peekEnableCPUread)
			{

				m32(m32DbgSampledInterruptProcessIdx) = v32(v32WorkingTimer0Idx);
#if defined(useTimer1Interrupt)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingTimer1Idx);
#endif // defined(useTimer1Interrupt)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingInjectorOpenIdx);
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingInjectorCloseIdx);
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingVSSpulseIdx);
#if defined(useAnalogRead)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingAnalogIdx);
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingTwoWireIdx);
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial0Idx);
#if defined(useSerial0PortInput)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial0InputIdx);
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial1Idx);
#if defined(useSerial1PortInput)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial1InputIdx);
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial2Idx);
#if defined(useSerial2PortInput)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial2InputIdx);
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial3Idx);
#if defined(useSerial3PortInput)
				m32(m32DbgSampledInterruptProcessIdx) += v32(v32WorkingSerial3InputIdx);
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)

				m32(m32DbgSampledTimer0Idx) = v32(v32WorkingTimer0Idx);
#if defined(useTimer1Interrupt)
				m32(m32DbgSampledTimer1Idx) = v32(v32WorkingTimer1Idx);
#endif // defined(useTimer1Interrupt)
				m32(m32DbgSampledInjectorOpenIdx) = v32(v32WorkingInjectorOpenIdx);;
				m32(m32DbgSampledInjectorCloseIdx) = v32(v32WorkingInjectorCloseIdx);;
				m32(m32DbgSampledVSSpulseIdx) = v32(v32WorkingVSSpulseIdx);;
#if defined(useAnalogRead)
				m32(m32DbgSampledAnalogIdx) = v32(v32WorkingAnalogIdx);
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
				m32(m32DbgSampledTwoWireIdx) = v32(v32WorkingTwoWireIdx);
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
				m32(m32DbgSampledSerial0Idx) = v32(v32WorkingSerial0Idx);
#if defined(useSerial0PortInput)
				m32(m32DbgSampledSerial0InputIdx) = v32(v32WorkingSerial0InputIdx);
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
				m32(m32DbgSampledSerial1Idx) = v32(v32WorkingSerial1Idx);
#if defined(useSerial1PortInput)
				m32(m32DbgSampledSerial1InputIdx) = v32(v32WorkingSerial1InputIdx);
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
				m32(m32DbgSampledSerial2Idx) = v32(v32WorkingSerial2Idx);
#if defined(useSerial2PortInput)
				m32(m32DbgSampledSerial2InputIdx) = v32(v32WorkingSerial2InputIdx);
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
				m32(m32DbgSampledSerial3Idx) = v32(v32WorkingSerial3Idx);
#if defined(useSerial3PortInput)
				m32(m32DbgSampledSerial3InputIdx) = v32(v32WorkingSerial3InputIdx);
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)

			}

			v32(v32WorkingTimer0Idx) = 0;
#if defined(useTimer1Interrupt)
			v32(v32WorkingTimer1Idx) = 0;
#endif // defined(useTimer1Interrupt)
			v32(v32WorkingInjectorOpenIdx) = 0;
			v32(v32WorkingInjectorCloseIdx) = 0;
			v32(v32WorkingVSSpulseIdx) = 0;
#if defined(useAnalogRead)
			v32(v32WorkingAnalogIdx) = 0;
#endif // defined(useAnalogRead)
#if defined(useHardwareTWI)
			v32(v32WorkingTwoWireIdx) = 0;
#endif // defined(useHardwareTWI)
#if defined(useSerial0Port)
			v32(v32WorkingSerial0Idx) = 0;
#if defined(useSerial0PortInput)
			v32(v32WorkingSerial0InputIdx) = 0;
#endif // defined(useSerial0PortInput)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
			v32(v32WorkingSerial1Idx) = 0;
#if defined(useSerial1PortInput)
			v32(v32WorkingSerial1InputIdx) = 0;
#endif // defined(useSerial1PortInput)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
			v32(v32WorkingSerial2Idx) = 0;
#if defined(useSerial2PortInput)
			v32(v32WorkingSerial2InputIdx) = 0;
#endif // defined(useSerial2PortInput)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
			v32(v32WorkingSerial3Idx) = 0;
#if defined(useSerial3PortInput)
			v32(v32WorkingSerial3InputIdx) = 0;
#endif // defined(useSerial3PortInput)
#endif // defined(useSerial3Port)

			SREG = oldSREG; // restore interrupt flag status

#endif // defined(useDebugCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useDataLoggingOutput)
			if (EEPROM::readByte(pSerialDataLoggingIdx)) doOutputDataLog();

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
			if (v08(v8Timer0Status0Idx) & t0saOutputJSON) doOutputJSON();

#endif // defined(useJSONoutput)
#if defined(useBluetooth)
			if (EEPROM::readByte(pBluetoothOutputIdx)) bluetooth::mainOutput();

#endif // defined(useBluetooth)
#if defined(useRealTimeClockModule)
			if (v08(v8Timer0Status1Idx) & t0sbReadRTC)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				v08(v8Timer0Status1Idx) &= ~(t0sbReadRTC);

				SREG = oldSREG; // restore interrupt flag status

				clockSet::setFromRTC();

			}

#endif // defined(useRealTimeClockModule)
		}

#if defined(useActivityLED)
		activityLED::release(arMainSample);

#endif // defined(useActivityLED)
#if defined(useDebugCPUreading)
		m32(m32DbgWorkingMainSampleIdx) += heart::getCycle0Length(m32DbgWorkingMainStartIdx);

#endif // defined(useDebugCPUreading)
#if defined(useActivityLED)
		activityLED::assert(arMainOther);

#endif // defined(useActivityLED)
#if defined(useBluetooth)
		bluetooth::mainProcess();

#endif // defined(useBluetooth)
#if defined(useDebugTerminal)
		terminal::mainProcess();

#endif // defined(useDebugTerminal)
#if defined(useActivityLED)
		activityLED::release(arMainOther);

#endif // defined(useActivityLED)
#if defined(useDebugCPUreading)
		m32(m32DbgWorkingMainOtherIdx) += heart::getCycle0Length(m32DbgWorkingMainStartIdx);

#endif // defined(useDebugCPUreading)
#if defined(useActivityLED)
		activityLED::assert(arMainOutput);

#endif // defined(useActivityLED)
#if defined(useButtonInput)
		if (v08(v8ButtonStatusIdx) & btnStatusButtonRead) cursor::doCommand(); // if any buttons were pressed, go perform button action

#endif // defined(useButtonInput)
#if defined(useTFToutput) && !defined(useButtonInput) && defined(useTouchScreenInput)
		tftMain::pollTouch(); // non-blocking touch input (settings gear); runs every pass so the loop never stalls
#endif // defined(useTFToutput) && !defined(useButtonInput) && defined(useTouchScreenInput)
#if defined(useDragRaceFunction)
		if (v08(v8Timer0Status1Idx) & t0sbAccelTestFlag)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			v08(v8Timer0Status1Idx) &= ~(t0sbAccelTestFlag);
			accelTestStatus = v08(v8AccelerationFlagsIdx); // copy accel test flag status to this loop

			SREG = oldSREG; // restore interrupt flag status

			i = (lastAccelTestStatus ^ accelTestStatus) & accelTestClearFlags; // detect any drag race flag changes

			lastAccelTestStatus = accelTestStatus; // copy current accel test flag status for next loop

			if (i)
			{

				i = accelTestStatus & accelTestClearFlags;

				switch (i)
				{

					case (accelTestTriggered | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
						accelTestState = atsReady;
						break;

					case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed | accelTestDistance):
						accelTestState = atsActive;
						break;

					case (accelTestFinished):
						SWEET64::runPrgm(S64_PRGM_PTR(prgmTransferAccelTestTrips), 0);
						accelTestState = atsFinished;
						break;

					case (accelTestFinished | accelTestCancelled):
						accelTestState = atsCancelled;
						break;

					case (accelTestActive | accelTestFullSpeed | accelTestHalfSpeed):
						accelTestState = atsCheckPointDist;
						break;

					case (accelTestActive | accelTestFullSpeed | accelTestDistance):
						accelTestState = atsCheckPointHalf;
						break;

					case (accelTestActive | accelTestFullSpeed):
						accelTestState = atsCheckPointDistHalf;
						break;

					case (accelTestActive | accelTestHalfSpeed | accelTestDistance):
						accelTestState = atsCheckPointFull;
						break;

					case (accelTestActive | accelTestHalfSpeed):
						accelTestState = atsCheckPointDistFull;
						break;

					case (accelTestActive | accelTestDistance):
						accelTestState = atsCheckPointHalfFull;
						break;

					case (accelTestActive):
						accelTestState = atsCheckPointDistHalfFull;
						break;

					default:
						accelTestState = atsInvalidState;
						break;

				}

			}
			else accelTestState = atsNoStatusChange;

		}

#endif // defined(useDragRaceFunction)
		// this part of the main loop handles screen output to the user
		// it can execute either after the samples are collected and processed above, or after a key has been pressed
		if (v08(v8Timer0Status0Idx) & t0saUpdateDisplay)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			v08(v8Timer0Status0Idx) &= ~(t0saUpdateDisplay);

			SREG = oldSREG; // restore interrupt flag status

#if defined(useButtonInput)
#if defined(useDebugTerminal)
			if ((m08(m8PeekFlags) & peekStatusMessage) && (v08(v8Timer0Status0Idx) & t0saDisplayDelayActive))
			{

				text::charOut(m8DevDebugTerminalIdx, '*');

			}

			if (m08(m8PeekFlags) & peekOutputFlags)
			{

				text::charOut(m8DevDebugTerminalIdx, '*');
				text::hexByteOut(m8DevDebugTerminalIdx, v08(v8Timer0CommandIdx));
				text::hexByteOut(m8DevDebugTerminalIdx, v08(v8Timer0Status0Idx));
				text::hexByteOut(m8DevDebugTerminalIdx, v08(v8Timer0Status1Idx));
#if defined(useHardwareTWI)
				text::hexByteOut(m8DevDebugTerminalIdx, v08(v8TWIstatusIdx));
#endif // defined(useHardwareTWI)
				text::newLine(m8DevDebugTerminalIdx);

			}

#endif // defined(useDebugTerminal)
#if defined(useClockDisplay) && !defined(useDeepSleep)
			if (v08(v8ActivityIdx) & afActivityTimeoutFlag) clockDisplay::displayHandler(displayOutputIdx, 0);
			else cursor::updateDisplay(workingDisplayIdx, displayOutputIdx); // call indexed support section screen refresh function

#else // defined(useClockDisplay) && !defined(useDeepSleep)
			cursor::updateDisplay(workingDisplayIdx, displayOutputIdx); // call indexed support section screen refresh function

#endif // defined(useClockDisplay) && !defined(useDeepSleep)
#endif // defined(useButtonInput)
#if defined(useTFToutput) && !defined(useButtonInput)
			if (tftScreen == tftScreenMain) tftMain::update(); // refresh the dashboard; other screens draw on transitions (input is polled per-pass)
#if defined(useTFTsleepBarEverywhere)
			else TFT::drawActivityBar(); // keep the sleep bar live on the settings/dropdown/keypad screens (they reserve the bottom strip)
#endif // defined(useTFTsleepBarEverywhere)
#endif // defined(useTFToutput) && !defined(useButtonInput)
		}

#if defined(useActivityLED)
		activityLED::release(arMainProcess | arMainOutput);

#endif // defined(useActivityLED)
#if defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useDebugCPUreading)
		m32(m32DbgWorkingMainOutputIdx) -= m32(m32DbgWorkingMainStartIdx);
#endif // defined(useDebugCPUreading)
		m32(m32CPUworkingMainProcessIdx) += heart::getCycle0Length(m32CPUworkingMainStartIdx);
#if defined(useDebugCPUreading)
		m32(m32DbgWorkingMainOutputIdx) += m32(m32DbgWorkingMainStartIdx);
#endif // defined(useDebugCPUreading)
		m32(m32CPUworkingIdleStartIdx) = m32(m32DbgWorkingMainStartIdx);

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useActivityLED)
		activityLED::assert(arIdleProcess);

#endif // defined(useActivityLED)
		heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#if defined(useActivityLED)
		activityLED::release(arIdleProcess);

#endif // defined(useActivityLED)
#if defined(useCPUreading) || defined(useDebugCPUreading)
		m32(m32CPUworkingIdleProcessIdx) += heart::getCycle0Length(m32CPUworkingIdleStartIdx);
		m32(m32CPUworkingMainLoopIdx) += heart::getCycle0Length(m32CPUworkingLoopStartIdx);

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
	}

}

