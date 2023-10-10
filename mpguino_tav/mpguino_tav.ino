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
#include <avr/EEPROM.h>
#include <avr/sleep.h>

static const char titleMPGuino[] PROGMEM = {
	tcOON "MPGuino v1.95tav" tcEOSCR
};

static const char dateMPGuino[] PROGMEM = {
	"2023-AUG-14" tcEOSCR
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
#include "functions.h"
#include "text.h"
#include "feature_bluetooth.h"
#include "feature_outputpin.h"
#include "feature_debug.h"
#include "feature_settings.h"
#include "feature_datalogging.h"
#include "feature_bignum.h"
#include "feature_bargraph.h"
#include "feature_dragrace.h"
#include "feature_coastdown.h"
#include "feature_base.h"
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

	j = heart::delay0(delay0Tick1500ms, 0); // show splash screen for 1.5 seconds

	heart::initHardware(); // initialize all human interface peripherals

#if defined(useSavedTrips)
	i = tripSave::doAutoAction(taaModeRead);

#endif // defined(useSavedTrips)
#if defined(useLCDoutput)
	text::gotoXY(m8DevLCDidx, 0, 0);
	text::stringOut(m8DevLCDidx, titleMPGuino);
	text::stringOut(m8DevLCDidx, dateMPGuino);

#endif // defined(useLCDoutput)
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
	while (volatile8Variables[(uint16_t)(v8Timer0DelayIdx - v8VariableStartIdx)] & j) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#if defined(useButtonInput)
#if defined(useSimulatedFIandVSS)
	signalSim::configurePorts(debugVSSflag | debugInjectorFlag);

#endif // defined(useSimulatedFIandVSS)
	// restore cursor positions from EEPROM
	for (uint8_t x = 0; x < displayCountTotal; x++) displayCursor[(uint16_t)(x)] = EEPROM::readByte(x + eePtrDisplayCursorStart);

	// restore menu heights from EEPROM
	for (uint8_t x = 0; x < displayCountMenu; x++) menuHeight[(uint16_t)(x)] = EEPROM::readByte(x + eePtrMenuHeightStart);

	// restore working display index from EEPROM
	workingDisplayIdx = EEPROM::readByte(pDisplayIdx);

	// call working display index initialization function
	cursor::updateDisplay(workingDisplayIdx, displayInitialEntryIdx);

#endif // defined(useButtonInput)
#if defined(useSavedTrips)
#if defined(useLCDoutput)
	if (i) text::statusOut(m8DevLCDidx, PSTR("AutoRestore Done"));

#endif // defined(useLCDoutput)
#endif // defined(useSavedTrips)
	j = 0;

	while (true)
	{

#if defined(useActivityRecord)
#if defined(useDebugCPUreading)
		activity::record((arMainProcess | arMainDevices), j);

#else // defined(useDebugCPUreading)
		activity::record((arMainProcess), j);

#endif // defined(useDebugCPUreading)
#endif // defined(useActivityRecord)
#if defined(useSimulatedFIandVSS)
		if (volatile8Variables[(uint16_t)(v8SignalSimModeIdx - v8VariableStartIdx)] & debugEnableFlags)
		{

			if (volatile8Variables[(uint16_t)(v8SignalSimModeIdx - v8VariableStartIdx)] & debugInjectorFlag) // if injector simulator is enabled
			{

				// if timer0 delay for fuel injector simulator is timed out
				if (volatile8Variables[(uint16_t)(v8Timer0DelaySignalIdx - v8VariableStartIdx)] & mainProgram8Variables[(uint16_t)(m8SignalSimFIPdelayFlagIdx - m8VariableStartIdx)])
				{

					// reset timer0 delay for fuel injector simulator
					heart::changeBitFlagBits(v8Timer0DelaySignalIdx - v8VariableStartIdx, mainProgram8Variables[(uint16_t)(m8SignalSimFIPdelayFlagIdx - m8VariableStartIdx)], 0);

					if (mainProgram8Variables[(uint16_t)(m8SignalSimFIPidx - m8VariableStartIdx)]) mainProgram8Variables[(uint16_t)(m8SignalSimFIPidx - m8VariableStartIdx)]--;
					else
					{

						mainProgram8Variables[(uint16_t)(m8SignalSimFIPidx - m8VariableStartIdx)] = debugFIPlength - 1;
						mainProgram8Variables[(uint16_t)(m8SignalSimFIPstate - m8VariableStartIdx)] += 0x40;

					}

					if (mainProgram8Variables[(uint16_t)(m8SignalSimFIPstate - m8VariableStartIdx)] & 0x40)
					{

						if (mainProgram8Variables[(uint16_t)(m8SignalSimFIPstate - m8VariableStartIdx)] & 0x80) i = debugFIPlength - mainProgram8Variables[(uint16_t)(m8SignalSimFIPidx - m8VariableStartIdx)] - 1;
						else i = mainProgram8Variables[(uint16_t)(m8SignalSimFIPidx - m8VariableStartIdx)];

						signalSimFIPtickLength = pgm_read_word(&debugFIPvalues[(uint16_t)(i)]); // read stored engine period countdown value
						signalSimFIPWopenTickLength = pgm_read_word(&debugFIPWvalues[(uint16_t)(i)]); // read stored fuel injector open period value

						// if fuel injector open period value is greater than stored engine period countdown value, use the stored engine period countdown value instead
						if (signalSimFIPWopenTickLength > (signalSimFIPtickLength - 63)) signalSimFIPWopenTickLength = signalSimFIPtickLength - 63;

						oldSREG = SREG; // save interrupt flag status
						cli(); // disable interrupts to make the next operation atomic

						volatile16Variables[(uint16_t)(v16SignalSimFIPtickLength - v16VariableStartIdx)] = signalSimFIPtickLength;
						volatile16Variables[(uint16_t)(v16SignalSimFIPWtickLength - v16VariableStartIdx)] = signalSimFIPWopenTickLength;

						volatile8Variables[(uint16_t)(v8SignalSimModeIdx - v8VariableStartIdx)] |= (debugFIPready);

						SREG = oldSREG; // restore interrupt flag status

					}

				}

			}

			if (volatile8Variables[(uint16_t)(v8SignalSimModeIdx - v8VariableStartIdx)] & debugVSSflag) // if VSS simulator is enabled
			{

				// if timer0 delay for VSS simulator is timed out
				if (volatile8Variables[(uint16_t)(v8Timer0DelaySignalIdx - v8VariableStartIdx)] & mainProgram8Variables[(uint16_t)(m8SignalSimVSSdelayFlagIdx - m8VariableStartIdx)])
				{

					// reset timer0 delay for VSS simulator
					heart::changeBitFlagBits(v8Timer0DelaySignalIdx - v8VariableStartIdx, mainProgram8Variables[(uint16_t)(m8SignalSimVSSdelayFlagIdx - m8VariableStartIdx)], 0);

					if (mainProgram8Variables[(uint16_t)(m8SignalSimVSSidx - m8VariableStartIdx)]) mainProgram8Variables[(uint16_t)(m8SignalSimVSSidx - m8VariableStartIdx)]--;
					else
					{

						mainProgram8Variables[(uint16_t)(m8SignalSimVSSidx - m8VariableStartIdx)] = debugVSSlength - 1;
						mainProgram8Variables[(uint16_t)(m8SignalSimVSSstate - m8VariableStartIdx)] += 0x40;

					}

					if (mainProgram8Variables[(uint16_t)(m8SignalSimVSSstate - m8VariableStartIdx)] & 0x40)
					{

						if (mainProgram8Variables[(uint16_t)(m8SignalSimVSSstate - m8VariableStartIdx)] & 0x80) i = mainProgram8Variables[(uint16_t)(m8SignalSimVSSidx - m8VariableStartIdx)];
						else i = debugVSSlength - mainProgram8Variables[(uint16_t)(m8SignalSimVSSidx - m8VariableStartIdx)] - 1;

						oldSREG = SREG; // save interrupt flag status
						cli(); // disable interrupts to make the next operations atomic

						volatile16Variables[(uint16_t)(v16SignalSimVSStickLength - v16VariableStartIdx)] = pgm_read_word(&debugVSSvalues[(uint16_t)(i)]);

						volatile8Variables[(uint16_t)(v8SignalSimModeIdx - v8VariableStartIdx)] |= (debugVSSready);

						SREG = oldSREG; // restore interrupt flag status

					}

				}

			}

		}

#endif // defined(useSimulatedFIandVSS)
#if defined(useChryslerMAPCorrection)
		if (volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] & asReadMAPchannel)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] &= ~(asReadMAPchannel);

			SREG = oldSREG; // restore interrupt flag status

			SWEET64::runPrgm(prgmCalculateMAPpressure, 0);

		}

#endif // defined(useChryslerMAPCorrection)
#if defined(useChryslerBaroSensor)
		if (volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] & asReadBaroChannel)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] &= ~(asReadBaroChannel);

			SREG = oldSREG; // restore interrupt flag status

			SWEET64::runPrgm(prgmCalculateBaroPressure, 0);

		}

#endif // defined(useChryslerBaroSensor)
#if defined(useAnalogButtons)
		if (volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] & asReadButtonChannel)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			volatile8Variables[(uint16_t)(v8AnalogStatusIdx - v8VariableStartIdx)] &= ~(asReadButtonChannel);

			SREG = oldSREG; // restore interrupt flag status

			for (uint8_t x = analogButtonCount - 1; x < analogButtonCount; x--)
			{

				if (volatile16Variables[(uint16_t)(v16AnalogButtonChannelIdx - v16VariableStartIdx)] >= pgm_read_word(&analogButtonThreshold[(uint16_t)(x)]))
				{

					if (volatile8Variables[(uint16_t)(v8Timer0CommandIdx - v8VariableStartIdx)] & t0cEnableButtonSampling) button::inject(pgm_read_byte(&analogTranslate[(uint16_t)(x)]));

					break;

				}

			}

		}

#endif // defined(useAnalogButtons)
#if defined(useDebugCPUreading)
		activity::record(arMainActivity, arMainDevices);

#endif // defined(useDebugCPUreading)
		if (volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] & (afActivityTimeoutFlag | afEngineOffFlag | afVehicleStoppedFlag | afParkFlag))
		{

			if (volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] & afActivityTimeoutFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] &= ~(afActivityTimeoutFlag);

				SREG = oldSREG; // restore interrupt flag status

				if (volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afActivityTimeoutFlag) // if MPGuino is commanded to go asleep
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
					TFT::init(); // re-initialize TFT device
#endif // defined(useTFToutput)
#if defined(useButtonInput)
					cursor::updateDisplay(workingDisplayIdx, displayInitialEntryIdx); // call indexed support section screen initialization function
#endif // defined(useButtonInput)

				}

			}

			if (volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] & afEngineOffFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] &= ~(afEngineOffFlag);

				SREG = oldSREG; // restore interrupt flag status

#if defined(useButtonInput)
				// if engine start is detected
				if (((volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afEngineOffFlag) == 0) && (EEPROM::readByte(pWakeupResetCurrentOnEngineIdx))) tripSupport::resetCurrent();

#endif // defined(useButtonInput)
			}

			if (volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] & afVehicleStoppedFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] &= ~(afVehicleStoppedFlag);

				SREG = oldSREG; // restore interrupt flag status

#if defined(useButtonInput)
				// if vehicle movement is detected
				if (((volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afVehicleStoppedFlag) == 0) && (EEPROM::readByte(pWakeupResetCurrentOnMoveIdx))) tripSupport::resetCurrent();

#endif // defined(useButtonInput)
#if defined(useDragRaceFunction)
				// if vehicle is stopped
				if ((volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afVehicleStoppedFlag) && (EEPROM::readByte(pDragAutoFlagIdx))) accelerationTest::triggerTest();

#endif // defined(useDragRaceFunction)
			}

			if (volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] & afParkFlag)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				volatile8Variables[(uint16_t)(v8ActivityIdxChangeIdx - v8VariableStartIdx)] &= ~(afParkFlag);

				SREG = oldSREG; // restore interrupt flag status

#if defined(useWindowTripFilter) || defined(useSavedTrips)
				if (volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afParkFlag) // if MPGuino is commanded to go park
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

#if defined(useDebugCPUreading)
		activity::record(arMainSample, arMainActivity);

#endif // defined(useDebugCPUreading)
		// this is the part of the main loop that only executes twice a second (or what is defined by samplesPerSecond), to collect and process readings
		if (volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] & t0saTakeSample)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] &= ~(t0saTakeSample);

			oldRawTripIdx = curRawTripIdx; // save old raw trip variable index
			curRawTripIdx ^= (raw0tripIdx ^ raw1tripIdx); // set current raw trip variable index
#if defined(trackIdleEOCdata)
			oldRawEOCidleTripIdx = curRawEOCidleTripIdx; // save old raw EOC/idle trip variable index
			curRawEOCidleTripIdx ^= (raw0eocIdleTripIdx ^ raw1eocIdleTripIdx); // set current raw EOC/idle trip variable index
#endif // defined(trackIdleEOCdata)

			SREG = oldSREG; // restore interrupt flag status

#if defined(useBarFuelEconVsTime)
			if (volatile8Variables[(uint16_t)(v8Timer0Status1Idx - v8VariableStartIdx)] & t0sbResetFEvsTimeTrip) 
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operations atomic

				volatile8Variables[(uint16_t)(v8Timer0Status1Idx - v8VariableStartIdx)] &= ~(t0sbResetFEvsTimeTrip);
				i = volatile8Variables[(uint16_t)(v8FEvTimeTripIdx - v8VariableStartIdx)];

				SREG = oldSREG; // restore interrupt flag status

				tripVar::reset(i + FEvsTimePeriodIdx); // reset source trip variable

			}

#endif // defined(useBarFuelEconVsTime)
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

			mainProgram32Variables[(uint16_t)(m32AvailableRAMidx - m32VariableStartIdx)] = availableRAMptr;

#if defined(useDebugCPUreading)
			if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekEnableCPUread)
			{

				mainProgram32Variables[(uint16_t)(m32CPUsampledMainLoopIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32CPUworkingMainLoopIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32CPUsampledMainProcessIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32CPUworkingMainProcessIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32CPUsampledIdleProcessIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32CPUworkingIdleProcessIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32DbgSampledMainDevicesIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32DbgWorkingMainDevicesIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32DbgSampledMainActivityIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32DbgWorkingMainActivityIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32DbgSampledMainSampleIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32DbgWorkingMainSampleIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32DbgSampledMainOutputIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32DbgWorkingMainOutputIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32DbgSampledMainOtherIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32DbgWorkingMainOtherIdx - m32VariableStartIdx)];
				mainProgram32Variables[(uint16_t)(m32DbgSampledS64processIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32DbgWorkingS64processIdx - m32VariableStartIdx)];

			}
#else // defined(useDebugCPUreading)
			mainProgram32Variables[(uint16_t)(m32CPUsampledMainLoopIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32CPUworkingMainLoopIdx - m32VariableStartIdx)];
			mainProgram32Variables[(uint16_t)(m32CPUsampledMainProcessIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32CPUworkingMainProcessIdx - m32VariableStartIdx)];
			mainProgram32Variables[(uint16_t)(m32CPUsampledIdleProcessIdx - m32VariableStartIdx)] = mainProgram32Variables[(uint16_t)(m32CPUworkingIdleProcessIdx - m32VariableStartIdx)];
#endif // defined(useDebugCPUreading)

			mainProgram32Variables[(uint16_t)(m32CPUworkingMainLoopIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32CPUworkingMainProcessIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32CPUworkingIdleProcessIdx - m32VariableStartIdx)] = 0;
#if defined(useDebugCPUreading)
			mainProgram32Variables[(uint16_t)(m32DbgWorkingMainDevicesIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32DbgWorkingMainActivityIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32DbgWorkingMainSampleIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32DbgWorkingMainOutputIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32DbgWorkingMainOtherIdx - m32VariableStartIdx)] = 0;
			mainProgram32Variables[(uint16_t)(m32DbgWorkingS64processIdx - m32VariableStartIdx)] = 0;
#endif // defined(useDebugCPUreading)

#if defined(useDebugCPUreading)
			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekEnableCPUread)
				mainProgram32Variables[(uint16_t)(m32DbgSampledInterruptProcessIdx - m32VariableStartIdx)] = volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)];

			volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] = 0;

			SREG = oldSREG; // restore interrupt flag status

#endif // defined(useDebugCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useOutputPins)
			outputPin::setOutputPin1(0);
			outputPin::setOutputPin2(1);

#endif // defined(useOutputPins)
#if defined(useDataLoggingOutput)
			if (EEPROM::readByte(pSerialDataLoggingIdx)) doOutputDataLog();

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
			if ((volatile8Variables[(uint16_t)(v8AwakeIdx - v8VariableStartIdx)] & aAwakeOnVehicle) && (EEPROM::readByte(pJSONoutputIdx))) doOutputJSON();

#endif // defined(useJSONoutput)
#if defined(useBluetooth)
			if (EEPROM::readByte(pBluetoothOutputIdx)) bluetooth::mainOutput();

#endif // defined(useBluetooth)
#if defined(useDS1307clock)
			if (volatile8Variables[(uint16_t)(v8Timer0Status1Idx - v8VariableStartIdx)] & t0sbReadRTC)
			{

				oldSREG = SREG; // save interrupt flag status
				cli(); // disable interrupts to make the next operation atomic

				volatile8Variables[(uint16_t)(v8Timer0Status1Idx - v8VariableStartIdx)] &= ~(t0sbReadRTC);

				SREG = oldSREG; // restore interrupt flag status

				clockSet::setFromRTC();

			}

#endif // defined(useDS1307clock)
		}

#if defined(useDebugCPUreading)
		activity::record(arMainOther, arMainSample);

#endif // defined(useDebugCPUreading)
#if defined(useBluetooth)
		bluetooth::mainProcess();

#endif // defined(useBluetooth)
#if defined(useDebugTerminal)
		terminal::mainProcess();

#endif // defined(useDebugTerminal)
#if defined(useDebugCPUreading)
		activity::record(arMainOutput, arMainOther);

#endif // defined(useDebugCPUreading)
#if defined(useButtonInput)
		if (volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] & t0saReadButton) cursor::doCommand(); // if any buttons were pressed, go perform button action

#endif // defined(useButtonInput)
#if defined(useDragRaceFunction)
		if (volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] & t0saAccelTestFlag)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operations atomic

			volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] &= ~(t0saAccelTestFlag);
			accelTestStatus = volatile8Variables[(uint16_t)(v8AccelerationFlagsIdx - v8VariableStartIdx)]; // copy accel test flag status to this loop

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
						SWEET64::runPrgm(prgmTransferAccelTestTrips, 0);
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
		if (volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] & t0saUpdateDisplay)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts to make the next operation atomic

			volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] &= ~(t0saUpdateDisplay);

			SREG = oldSREG; // restore interrupt flag status

#if defined(useButtonInput)
#if defined(useDebugTerminal)
			if ((mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekStatusMessage) && (mainProgram8Variables[(uint16_t)(m8Delay0FlagLCDidx - m8VariableStartIdx)]))
			{

				text::charOut(m8DevDebugTerminalIdx, '*');

			}

			if (mainProgram8Variables[(uint16_t)(m8PeekFlags - m8VariableStartIdx)] & peekOutputFlags)
			{

				text::charOut(m8DevDebugTerminalIdx, '*');
				text::hexByteOut(m8DevDebugTerminalIdx, volatile8Variables[(uint16_t)(v8Timer0CommandIdx - v8VariableStartIdx)]);
				text::hexByteOut(m8DevDebugTerminalIdx, volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)]);
				text::hexByteOut(m8DevDebugTerminalIdx, volatile8Variables[(uint16_t)(v8Timer0Status1Idx - v8VariableStartIdx)]);
#if defined(useTWIsupport)
				text::hexByteOut(m8DevDebugTerminalIdx, volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)]);
#endif // defined(useTWIsupport)
				text::newLine(m8DevDebugTerminalIdx);

			}

#endif // defined(useDebugTerminal)
#if defined(useClockDisplay) && !defined(useDeepSleep)
			if (volatile8Variables[(uint16_t)(v8ActivityIdx - v8VariableStartIdx)] & afActivityTimeoutFlag) clockDisplay::displayHandler(displayOutputIdx, 0);
			else cursor::updateDisplay(workingDisplayIdx, displayOutputIdx); // call indexed support section screen refresh function

#else // defined(useClockDisplay) && !defined(useDeepSleep)
			cursor::updateDisplay(workingDisplayIdx, displayOutputIdx); // call indexed support section screen refresh function

#endif // defined(useClockDisplay) && !defined(useDeepSleep)
#endif // defined(useButtonInput)
		}

#if defined(useActivityRecord)
#if defined(useDebugCPUreading)
		activity::record(arIdleProcess, (arMainProcess | arMainOutput));

#else // defined(useDebugCPUreading)
		activity::record(arIdleProcess, arMainProcess);

#endif // defined(useDebugCPUreading)
#endif // defined(useActivityRecord)
		heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

#if defined(useActivityRecord)
		j = arIdleProcess;

#endif // defined(useActivityRecord)
	}

}

