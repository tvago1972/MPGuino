// MPGuino - open source fuel consumption tracking system
// GPL Software, mass production use rights reserved by opengauge.org
// personal use is perfectly fine
// no warranties expressed or implied

// Special thanks to the good folks at ecomodder.com, ardunio.cc, avrfreaks.net, cadsoft.de, atmel.com,
// and all the folks who donate their time and resources and share their experiences freely

/* External connections:

 Vehicle interface pins
 injector open D2 (int0)
 injector closed D3 (int1)
 speed C0 (pcint8)

 LCD Pins
 DI  D4
 DB4 D7
 DB5 B0
 DB6 B4
 DB7 B5
 Enable C5
 Contrast D6, controlled by PWM on OC0A
 Brightness B1, controlled by PWM on OC1A

 Buttons
 left C3 (pcint11)
 middle  C4 (pcint12)
 right C5 (pcint13)

 */

/* Program overview (easier said than done)
 set up digital pins to drive the lcd
 set up pwm pins for lcd brightness and contrast
 set up interrupts for the buttons, the speed signal, and the injector high/low signals.
 set up tx pin for transmitting values over uart

 create accumulators for speed/injector data

 mainloop{
 incorporate the accumulators into longer storage trips, reset accumulators
 display computations, transmit accumulators
 scan for key presses and perform their function (change screen,reset a trip,goto setup)
 pause for remainder of 1/2 second
 }

 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

typedef void (* pFunc)(void); // type for display function pointers

// how many times will we try and loop in a second
#define loopsPerSecond 2

// use with 20mhz
#define speedfactor 2500ul

// use with 16mhz, not tested
//#define speedfactor 2000ul

#define tickfactor (speedfactor * 125ul)
#define loopfactor (tickfactor / (loopsPerSecond * 128ul))
// number of processor timer2 cycles (configured with 64x clock divider) per nanosecond
#define cycle2ms (tickfactor / 500)
#define myubbr (speedfactor * 8000 / 16 / 9600 - 1)

unsigned long elapsedMicroseconds(unsigned long startMicroSeconds, unsigned long currentMicroseconds);
unsigned long cycle2second[2];
unsigned long cycle2hour[2];

void doDisplayCustom(void);
void doDisplayEOCIdleData(void);
void doDisplayInstantCurrent(void);
void doDisplayInstantTank(void);
void doDisplayBigInstant(void);
void doDisplayBigCurrent(void);
void doDisplayBigTank(void);
void doDisplayCurrentTripData(void);
void doDisplayTankTripData(void);
void doDisplayCPUutil(void);
void doDisplaySystemInfo(void);
void doDisplaySystemTime(void);
void doDisplayRawInstFuelCount(void);
void doDisplayRawInstFuelTime(void);
void doDisplayRawInstTime(void);
void dispRawVal(unsigned long an[]);
int memoryTest(void);
void bigNum(char *value, char *txt1);
void init64(unsigned long an[], unsigned long ann[]);
void init64(unsigned long an[], unsigned long bigPart, unsigned long littlePart);
void shl64(unsigned long an[]);
void shr64(unsigned long an[]);
void add64(unsigned long an[], unsigned long ann[]);
void sub64(unsigned long an[], unsigned long ann[]);
void div64(unsigned long num[], unsigned long den[]);
void mul64(unsigned long an[], unsigned long ann[]);
unsigned long doFactor(unsigned long source, unsigned long numer, unsigned long denom);
char *format(unsigned long num);
char *uformat(unsigned long val);
unsigned long rformat(char *val);
char *formatTime(unsigned long cycles[]);
unsigned long editParm(uint8_t parmIdx, unsigned long value, uint8_t leftPos, unsigned long maxValue);
unsigned long editUL(uint8_t parmIdx, unsigned long value);
uint8_t editByte(uint8_t parmIdx, uint8_t value);
uint8_t editBit(uint8_t parmIdx, uint8_t value);
void editGuino(void);
void initGuino(void);
void delay2(unsigned long ms);
void simpletx(char * string);

unsigned long parms[] = { // default values (defaults are for a 2005 Dodge Magnum SXT with 3.5L V6)
	37158915ul,     // flags and contrast and VSS debounce delay and pulses per every 2 crankshaft revolutions
	10000ul,        // VSS pulses per mile
	125884406ul,    // microseconds per gallon
	120000000ul,    // activity timeout in microseconds (microseconds? really? Why not go full anal and use Planck units, instead?)
	18500ul,        // tank capacity in gallons * 1000ul
	850ul,          // fuel injector delay delta in microseconds
	4000ul,         // vehicle weight in lbs
	0ul             // scratchpad memory
};

prog_char pString_00[] PROGMEM = "Contrast";
prog_char pString_01[] PROGMEM = "VSS Pulses/Mile";
prog_char pString_02[] PROGMEM = "MicroSec/Gallon";
prog_char pString_03[] PROGMEM = "Timeout (uSec)";
prog_char pString_04[] PROGMEM = "Tank Gal * 1000";
prog_char pString_05[] PROGMEM = "Injector DelayuS";
prog_char pString_06[] PROGMEM = "Weight (lbs)";
prog_char pString_07[] PROGMEM = "Scratchpad(odo?)";
prog_char pString_08[] PROGMEM = "Pulses/2 revs";
prog_char pString_09[] PROGMEM = "VSS Delay (ms)";
prog_char pString_10[] PROGMEM = "InjTrg 0-Dn 1-Up";
prog_char pString_11[] PROGMEM = "Metric 1-Yes";
prog_char pString_12[] PROGMEM = "XMitSerial 1-Yes";

prog_char fString_00[] PROGMEM = "Custom  ";
prog_char fString_01[] PROGMEM = "Instant/Current ";
prog_char fString_02[] PROGMEM = "Instant/Tank ";
prog_char fString_03[] PROGMEM = "BIG Instant ";
prog_char fString_04[] PROGMEM = "BIG Current ";
prog_char fString_05[] PROGMEM = "BIG Tank ";
prog_char fString_06[] PROGMEM = "Current ";
prog_char fString_07[] PROGMEM = "Tank ";
prog_char fString_08[] PROGMEM = "EOC/Idle ";
prog_char fString_09[] PROGMEM = "CPU Monitor ";
prog_char fString_10[] PROGMEM = "System Time ";
prog_char fString_11[] PROGMEM = "Raw Inj Counts ";
prog_char fString_12[] PROGMEM = "Raw Inj Time ";
prog_char fString_13[] PROGMEM = "Raw Time ";

prog_char dString_00[] PROGMEM = " G"; // fuel quantity
prog_char dString_01[] PROGMEM = " L";
prog_char dString_02[] PROGMEM = " R"; // fuel consumption rate
prog_char dString_03[] PROGMEM = " r";
prog_char dString_04[] PROGMEM = " D"; // distance
prog_char dString_05[] PROGMEM = " d";
prog_char dString_06[] PROGMEM = " S"; // speed
prog_char dString_07[] PROGMEM = " s";
prog_char dString_08[] PROGMEM = " M"; // fuel economy
prog_char dString_09[] PROGMEM = " L";
prog_char dString_10[] PROGMEM = " T"; // time
prog_char dString_11[] PROGMEM = " T";
prog_char dString_12[] PROGMEM = "MPG ";
prog_char dString_13[] PROGMEM = "L/K ";

prog_char bignumchars1[] PROGMEM = {
	4, 1, 4, 0,
	1, 4, 32, 0,
	3, 3, 4, 0,
	1, 3, 4, 0,
	4, 2, 4, 0,
	4, 3, 3, 0,
	4, 3, 3, 0,
	1, 1, 4, 0,
	4, 3, 4, 0,
	4, 3, 4, 0
};

prog_char bignumchars2[] PROGMEM = {
	4, 2, 4, 0,
	2, 4, 2, 0,
	4, 2, 2, 0,
	2, 2, 4, 0,
	32, 32, 4, 0,
	2, 2, 4, 0,
	4, 2, 4, 0,
	32, 4, 32, 0,
	4, 2, 4, 0,
	2, 2, 4, 0
};

prog_char lcdNybble[] PROGMEM = { 0, 128, 1, 129, 16, 144, 17, 145, 32, 160, 33, 161, 48, 176, 49, 177 };

prog_char chars[] PROGMEM = {
	0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000,
	0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111,
	0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111,
	0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111,
	0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01110, 0b01110, 0b01110
};

char *displayFuncNames[] = {
	fString_00,
	fString_01,
	fString_02,
	fString_03,
	fString_04,
	fString_05,
	fString_06,
	fString_07,
	fString_08,
	fString_09,
	fString_10,
	fString_11,
	fString_12,
	fString_13
};

char *parmLabels[] = {
	pString_00,
	pString_01,
	pString_02,
	pString_03,
	pString_04,
	pString_05,
	pString_06,
	pString_07,
	pString_08,
	pString_09,
	pString_10,
	pString_11,
	pString_12
};

char *displayLabels[] = {
	dString_00,
	dString_01,
	dString_02,
	dString_03,
	dString_04,
	dString_05,
	dString_06,
	dString_07,
	dString_08,
	dString_09,
	dString_10,
	dString_11,
	dString_12,
	dString_13
};

uint8_t brightness[] = { 0, 41, 84, 128 }; //middle button cycles through these brightness settings
// size of brightness table
#define brightnessLength ( sizeof(brightness) / sizeof(uint8_t) )
uint8_t brightnessIdx = 1;

char lblBuff[2]; // used by doFormat()
char mBuff1[17]; // used by format()
char mBuff2[17]; // used by editParm()

//for display computing
unsigned long tmp1[2];
unsigned long tmp2[2];
unsigned long tmp3[2];

volatile unsigned long distancefactor;
volatile unsigned long fuelfactor;
volatile unsigned long sleepfactor;
volatile unsigned long injSettleTime;
volatile uint8_t lcdContrast;
volatile uint8_t vssPause;
volatile uint8_t rpmfactor;

volatile uint8_t injEdgeIdx;
volatile uint8_t metricIdx;
volatile uint8_t serialIdx;

#define injectorEdgeBit 0b10000000
#define metricBit 0b01000000
#define serialTransmitBit 0b00100000

// size of parameter table
#define parmsLength ( sizeof(parms) / sizeof(unsigned long) )

#define storage1Index 0
#define contrastIdx 0
#define vssPulsesPerMileIdx 1
#define microSecondsPerGallonIdx 2
#define currentTripResetTimeoutUSIdx 3
#define tankSizeIdx 4
#define injectorSettleTimeIdx 5
#define weightIdx 6
#define scratchpadIdx 7

#define injPulsesPer2Revolutions parmsLength
#define vssPausePos (parmsLength + 1)
#define injEdgePos (parmsLength + 2)
#define metricPos (parmsLength + 3)
#define SerialPos (parmsLength + 4)
#define maxParmPos (parmsLength + 5)

#define nil 3999999999ul

#define guinosig 0b11100111

#define vssBit ( 1 << 0 )
#define lbuttonBit ( 1 << 3 )
#define mbuttonBit ( 1 << 4 )
#define rbuttonBit ( 1 << 5 )
#define pressOnlyL (mbuttonBit | rbuttonBit)
#define pressOnlyC (lbuttonBit | rbuttonBit)
#define pressOnlyR (lbuttonBit | mbuttonBit)
#define pressBothLC (rbuttonBit)
#define pressBothLR (mbuttonBit)
#define pressBothRC (lbuttonBit)
#define longPressOnlyL (vssBit | mbuttonBit | rbuttonBit)
#define longPressOnlyC (vssBit | lbuttonBit | rbuttonBit)
#define longPressOnlyR (vssBit | lbuttonBit | mbuttonBit)
#define longPressBothLC (vssBit | rbuttonBit)
#define longPressBothLR (vssBit | mbuttonBit)
#define longPressBothRC (vssBit | lbuttonBit)
#define keyDelay 2441
#define keyShortDelay (keyDelay - 80)

#define buttonsUp (lbuttonBit | mbuttonBit | rbuttonBit)

uint8_t buttonState = buttonsUp; // start with the buttons in the right state

volatile unsigned long timer2_overflow_count;

pFunc displayFuncs[] = {
	doDisplayCustom,
	doDisplayInstantCurrent,
	doDisplayInstantTank,
	doDisplayBigInstant,
	doDisplayBigCurrent,
	doDisplayBigTank,
	doDisplayCurrentTripData,
	doDisplayTankTripData,
	doDisplayEOCIdleData,
	doDisplaySystemInfo,
	doDisplaySystemTime,
	doDisplayRawInstFuelCount,
	doDisplayRawInstFuelTime,
	doDisplayRawInstTime
};

// size of display function pointer array
#define displayFuncSize ( sizeof(displayFuncs) / sizeof(pFunc) )

#define dispLeft 0b00000000
#define dispRight 0b00010000
#define dispRaw 0b10000000
#define dispTime 0b01000000
#define lblQtyIdx 0
#define lblRateIdx 2
#define lblDistIdx 4
#define lblSpeedIdx 6
#define lblEconIdx 8
#define lblTimeIdx 10
#define lblBigFEIdx 12

//Trip prototype
class Trip {
public:
	unsigned long elapsedTime[2]; // time elapsed since this class was reset
	unsigned long injOpenTime[2]; // time that the fuel injector has been open
	unsigned long injPulses; // rpm
	unsigned long vssPulses; // from the speedo

	char id; // identification byte for display formatting
	unsigned long idleTime[2]; // time spent while idling
	unsigned long idleInjOpenTime[2]; // time that the fuel injector has been open while idling
	unsigned long EOCTime[2]; // time spent while at EOC
	unsigned long EOCPulses; // vss pulse count while at EOC

	//these functions actually return in thousandths,
	char *fuelUsed(uint8_t dispPos);
	char *idleFuelUsed(uint8_t dispPos); //how much fuel was used at 0 mph?
	char *fuelRate(uint8_t dispPos);
	char *idleFuelRate(uint8_t dispPos);
	char *distance(uint8_t dispPos);
	char *eocDistance(uint8_t dispPos); //how many "free" miles?
	char *speed(uint8_t dispPos);
	char *eocSpeed(uint8_t dispPos);
	char *fuelEcon(uint8_t dispPos);
	char *time(uint8_t dispPos); //mmm.ss

	// these functions perform basic tasks
	void update(Trip t);
	void reset(void);

private:
	char *doFormat(unsigned long an[], uint8_t dispPos);
	char *doFuelUsed(unsigned long inj[], uint8_t dispPos);
	char *doFuelRate(unsigned long inj[], unsigned long elapsed[], uint8_t dispPos);
	char *doDistance(unsigned long vss, uint8_t dispPos);
	char *doSpeed(unsigned long vss, unsigned long elapsed[], uint8_t dispPos);

};

//LCD prototype
namespace LCD {
void gotoXY(uint8_t x, uint8_t y);
void flashPrint(PGM_P s);
void print(char *string);
void init();
void LcdCommandWrite(uint8_t value);
void LcdDataWrite(uint8_t value);
void pushNibble(uint8_t value);
};

//main objects we will be working with:

Trip instant;
Trip current;
Trip tank;

//ms counters
uint8_t VSSCounts;

unsigned long elapsedMicroseconds(unsigned long startMicroSeconds, unsigned long currentMicroseconds) {
	if (currentMicroseconds >= startMicroSeconds) return currentMicroseconds - startMicroSeconds;
	return 4294967295 - (startMicroSeconds - currentMicroseconds);
}

unsigned long outputTime[2];
volatile unsigned long lastTime;
volatile unsigned long thisTime = nil;
volatile unsigned long timeSlice;
volatile unsigned long timeTemp;
volatile unsigned long systemTime[2] = { 0 ,0 };
volatile unsigned long tripTime[2] = { 0, 0 };
volatile unsigned long vssPulseCount = 0;
volatile unsigned long injOpenStart = nil;
volatile unsigned long injOpenStop;
volatile unsigned long injOpenTime[2] = { 0, 0 };
volatile unsigned long injPulseCount = 0;
volatile uint8_t dirty = 0;

#define dirtyFlag    0b10000000
#define dirtyInjOnly 0b01000000
#define dirtyVSSonly 0b00100000
#define dirtyInj     (dirtyFlag | dirtyInjOnly)
#define dirtyVSS     (dirtyFlag | dirtyVSSonly)

#define timerStartDelay 0b10000000
#define timerExecDelay  0b01000000
#define timerStartCycle 0b00100000
#define timerExecCycle  0b00010000
#define timerMarkCycle  0b00001000
#define timerKeyWait    0b00000100
#define timerAsleep     0b00000010
#define timerWakeUp     0b00000001
volatile uint8_t thisTCNT2;
volatile unsigned long thisTOV;
volatile uint8_t timerStatus = 0;
volatile unsigned long timerDelay = 0;
volatile unsigned long timerCycle = 0;
volatile unsigned long timerCycleLength = 0;
volatile unsigned long timerSleep = 0;
volatile uint8_t lastPINCstate = 0;
volatile uint8_t lastKeyPressed = 0;
volatile unsigned int keyCounts = 0;

// this ISR gets called every time timer 2 overflows.
// timer 2 prescaler is set at 32, and it's an 8 bit counter
// so this ISR gets called every 16384 / (system clock) seconds (for 20 MHz clock, that is every 0.8192 ms)

ISR(TIMER2_OVF_vect)
{

	timer2_overflow_count++;
	thisTCNT2 = TCNT2;
	lastTime = thisTime;
	thisTime = (timer2_overflow_count << 8) | thisTCNT2;
	if (lastTime != nil) {

		timeSlice = elapsedMicroseconds(lastTime, thisTime);

		timeTemp = systemTime[1] + timeSlice;
		if (systemTime[1] > timeTemp) systemTime[0]++;
		systemTime[1] = timeTemp;

		timeTemp = tripTime[1] + timeSlice;
		if (tripTime[1] > timeTemp) tripTime[0]++;
		tripTime[1] = timeTemp;

	}

	if (VSSCounts != 0) {

		VSSCounts--;
		if (VSSCounts == 0) {

			vssPulseCount++;
			dirty |= dirtyVSS;

		}

	}

	if (keyCounts) {

		keyCounts--;

		if (keyCounts == 0) {

			lastKeyPressed |= vssBit;
			buttonState = lastKeyPressed;
			timerStatus &= ~timerKeyWait;

		}

		if (keyCounts == keyShortDelay) {

			thisTCNT2 = buttonsUp & lastPINCstate;

			if ((thisTCNT2 != buttonsUp) || (lastKeyPressed & vssBit)) {

				lastKeyPressed = thisTCNT2;

			} else {

				buttonState = lastKeyPressed;
				timerStatus &= ~timerKeyWait;
				keyCounts = 0;

			}

		}

	}

	if (timerStatus & timerMarkCycle) {

		timerCycleLength = timerCycle; // copy cycle timer ticks for main program
		timerStatus &= ~timerMarkCycle; // signal to main program that cycle timer has acknowledged main program request

	}

	if (timerStatus & timerStartCycle) { // if main program has requested to start cycle

		timerStatus &= ~timerStartCycle; // signal to main program that cycle timer has acknowledged main program request
		timerStatus |= timerExecCycle; // signal to main program that cycle timer is in progress
		timerCycle = 0; // initialize cycle timer

		outputTime[0] = systemTime[0]; // perform atomic transfer of system time to main program
		outputTime[1] = systemTime[1];

		instant.injOpenTime[0] = injOpenTime[0]; // perform atomic transfer of raw measurements to main program
		instant.injOpenTime[1] = injOpenTime[1];
		instant.injPulses = injPulseCount;
		instant.vssPulses = vssPulseCount;

		if (dirty & dirtyFlag) { // if vehicle engine is running and/or vehicle is moving

			instant.elapsedTime[0] = tripTime[0];
			instant.elapsedTime[1] = tripTime[1];

			if (dirty & dirtyVSSonly) { // if vehicle movement is detected, then vehicle is not idling

				instant.idleTime[0] = 0;
				instant.idleTime[1] = 0;
				instant.idleInjOpenTime[0] = 0;
				instant.idleInjOpenTime[1] = 0;

			} else { // otherwise, vehicle is idling

				instant.idleTime[0] = tripTime[0];
				instant.idleTime[1] = tripTime[1];
				instant.idleInjOpenTime[0] = injOpenTime[0];
				instant.idleInjOpenTime[1] = injOpenTime[1];

			}

			if (dirty & dirtyInjOnly) { // if vehicle fuel injector is firing, then vehicle is not in EOC mode

				instant.EOCTime[0] = 0;
				instant.EOCTime[1] = 0;
				instant.EOCPulses = 0;

			} else { // otherwise, vehicle is in EOC mode

				instant.EOCTime[0] = tripTime[0];
				instant.EOCTime[1] = tripTime[1];
				instant.EOCPulses = vssPulseCount;

			}

		} else {

			instant.elapsedTime[0] = 0;
			instant.elapsedTime[1] = 0;
			instant.idleTime[0] = 0;
			instant.idleTime[1] = 0;
			instant.idleInjOpenTime[0] = 0;
			instant.idleInjOpenTime[1] = 0;
			instant.EOCTime[0] = 0;
			instant.EOCTime[1] = 0;
			instant.EOCPulses = 0;

		}

		tripTime[0] = 0;
		tripTime[1] = 0;
		injOpenTime[0] = 0;
		injOpenTime[1] = 0;
		injPulseCount = 0;
		vssPulseCount = 0;
		dirty = 0;

	}

	if (timerStatus & timerExecCycle) { // if a cycle request execution is in progress

		timerCycle++; // bump cycle timer up by one tick

		if (timerCycle > loopfactor) { // if the cycle period has been reached

			timerStatus &= ~timerExecCycle; // stop the cycle timer and signal finished to main program

		}

	}

	if (timerStatus & timerStartDelay) { // if main program has requested a delay

		timerStatus &= ~timerStartDelay; // signal to main program that delay timer has acknowledged main program request
		timerStatus |= timerExecDelay; // signal to main program that delay timer is in progress

	}

	if (timerStatus & timerExecDelay) { // if a delay request execution is in progress

		if (timerDelay == 0) {

			timerStatus &= ~timerExecDelay;  // signal to main program that delay timer has completed

		} else {

			timerDelay--; // bump timer delay value down by one tick

		}

	}

	if (!(timerStatus & timerAsleep)) {

		timerSleep++;
		if (timerSleep > sleepfactor) {

			timerStatus |= timerAsleep;
			OCR1A = brightness[0];

		}

	}

	if (timerStatus & timerWakeUp) {

		OCR1A = brightness[brightnessIdx];
		timerStatus &= ~(timerWakeUp | timerAsleep);
		timerSleep = 0;

	}

}

ISR(INT0_vect) {

	thisTCNT2 = TCNT2;
	thisTOV = timer2_overflow_count;
	if (TIFR2 & (1 << TOV2)) thisTOV++;

	injOpenStart = (thisTOV << 8) | thisTCNT2;

	timerStatus |= timerWakeUp;

}

ISR(INT1_vect) {

	thisTCNT2 = TCNT2;
	thisTOV = timer2_overflow_count;
	if (TIFR2 & (1 << TOV2)) thisTOV++;

	injOpenStop = (thisTOV << 8) | thisTCNT2;

	timerStatus |= timerWakeUp;

	if (injOpenStart != nil) {

		timeTemp = injOpenTime[1] + elapsedMicroseconds(injOpenStart, injOpenStop) - injSettleTime;
		if (injOpenTime[1] > timeTemp) injOpenTime[0]++;
		injOpenTime[1] = timeTemp;
		injPulseCount++;
		dirty |= dirtyInj;

	}

}

ISR( PCINT1_vect ) {

	static uint8_t p;
	static uint8_t q;

	p = PINC;
	q = p ^ lastPINCstate;

	timerStatus |= timerWakeUp;

	if (q & vssBit) {

		if (vssPause == 0) {

			vssPulseCount++;
			dirty |= dirtyVSS;

		} else VSSCounts = vssPause;

	}

	if (q & buttonsUp) {

		keyCounts = keyDelay;

	}

	lastPINCstate = p;

}

uint8_t newRun = 0;
uint8_t screen = 0;
uint8_t holdDisplay = 0;

// all doDisplay functions assume that screen cursor will be at top left corner

void doDisplayCustom(void) {

	LCD::print(instant.fuelEcon(dispLeft));
	LCD::print(instant.speed(dispRight));
	LCD::gotoXY(0, 1);
	LCD::print(instant.fuelRate(dispLeft));
	LCD::print(current.fuelEcon(dispRight));

}

void doDisplayEOCIdleData(void) {

	LCD::print(current.eocDistance(dispLeft));
	LCD::print(current.idleFuelUsed(dispRight));
	LCD::gotoXY(0, 1);
	LCD::print(tank.eocDistance(dispLeft));
	LCD::print(tank.idleFuelUsed(dispRight));

}

void doDisplayInstantCurrent(void) {

	LCD::print(instant.fuelEcon(dispLeft));
	LCD::print(instant.speed(dispRight));
	LCD::gotoXY(0, 1);
	LCD::print(current.fuelEcon(dispLeft));
	LCD::print(current.distance(dispRight));

}

void doDisplayInstantTank(void) {

	LCD::print(instant.fuelEcon(dispLeft));
	LCD::print(instant.speed(dispRight));
	LCD::gotoXY(0, 1);
	LCD::print(tank.fuelEcon(dispLeft));
	LCD::print(tank.distance(dispRight));

}

void doDisplayBigInstant(void) {

	bigNum(instant.fuelEcon(dispRaw), "INST");

}

void doDisplayBigCurrent(void) {

	bigNum(current.fuelEcon(dispRaw), "CURR");

}

void doDisplayBigTank(void) {

	bigNum(tank.fuelEcon(dispRaw), "TANK");

}

void doDisplayCurrentTripData(void) { // display current trip formatted data

	LCD::print(current.speed(dispLeft));
	LCD::print(current.fuelEcon(dispRight));
	LCD::gotoXY(0, 1);
	LCD::print(current.distance(dispLeft));
	LCD::print(current.fuelUsed(dispRight));

}

void doDisplayTankTripData(void) { // display tank trip formatted data.

	LCD::print(tank.speed(dispLeft));
	LCD::print(tank.fuelEcon(dispRight));
	LCD::gotoXY(0, 1);
	LCD::print(tank.distance(dispLeft));
	LCD::print(tank.fuelUsed(dispRight));

}

void doDisplayCPUutil(void) {

	LCD::print("C%");
	LCD::print(format(timerCycleLength * 100000ul / loopfactor));

}

void doDisplaySystemTime(void) { // display system time

	char *val = formatTime(outputTime);

	LCD::flashPrint(&bignumchars1[(val[0] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(&bignumchars1[(val[1] - '0') * 4]);
	LCD::LcdDataWrite(5);
	LCD::flashPrint(&bignumchars1[(val[2] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(&bignumchars1[(val[3] - '0') * 4]);
	LCD::LcdDataWrite(' ');

	LCD::gotoXY(0, 1);
	LCD::flashPrint(&bignumchars2[(val[0] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(&bignumchars2[(val[1] - '0') * 4]);
	LCD::LcdDataWrite(5);
	LCD::flashPrint(&bignumchars2[(val[2] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(&bignumchars2[(val[3] - '0') * 4]);
	LCD::LcdDataWrite(' ');

}

void doDisplaySystemInfo(void) { // display max cpu utilization and RAM

	doDisplayCPUutil();
	LCD::print(tank.time(dispRight));
	unsigned long mem = memoryTest();
	mem *= 1000;
	LCD::gotoXY(0, 1);
	LCD::print("FREE MEM:");
	LCD::print(format(mem));

}

void doDisplayRawInstFuelCount(void) {

	init64(tmp1, 0, instant.injPulses);
	dispRawVal(tmp1);

}

void doDisplayRawInstFuelTime(void) {

	dispRawVal(instant.injOpenTime);

}

void doDisplayRawInstTime(void) {

	init64(tmp1, tank.elapsedTime); // load elapsed time into tmp1
	init64(tmp2, cycle2second); // load conversion factor from clock cycles to seconds into tmp2
	div64(tmp1, tmp2); // perform the conversion

	dispRawVal(tmp1);

}

void dispRawVal(unsigned long an[]) {

	LCD::print("rawH: ");
	LCD::print(uformat(an[0], mBuff1));
	LCD::gotoXY(0, 1);
	LCD::print("rawL: ");
	LCD::print(uformat(an[1], mBuff1));

}

void bigNum(char *value, char *txt1) {

	char dp1;
	char dp2;
	uint8_t p;
	uint8_t q;
	uint8_t r;

	p = 0; // default is number between 100 and 999
	q = 1;
	r = 2;
	dp1 = ' ';
	dp2 = ' ';

	if (value[3] == '.') { // check that decimal place is in correct spot

		if (value[0] == '0') { // if number is less than 100

			if (value[1] == '0') { // if number is less than 10

				p = 2;
				q = 4;
				r = 5;
				dp1 = 5;

			} else { // number between 10 and 99

				p = 1;
				q = 2;
				r = 4;
				dp2 = 5;

			}

		}

	} else { // number greater than 999

		value = "999";
		dp1 = ' ';

	}

	LCD::flashPrint(&bignumchars1[(value[p] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(&bignumchars1[(value[q] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(&bignumchars1[(value[r] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::print(txt1);

	LCD::gotoXY(0, 1);
	LCD::flashPrint(&bignumchars2[(value[p] - '0') * 4]);
	LCD::LcdDataWrite(dp1);
	LCD::flashPrint(&bignumchars2[(value[q] - '0') * 4]);
	LCD::LcdDataWrite(dp2);
	LCD::flashPrint(&bignumchars2[(value[r] - '0') * 4]);
	LCD::LcdDataWrite(' ');
	LCD::flashPrint(displayLabels[lblBigFEIdx + metricIdx]);
}

void LCD::gotoXY(uint8_t x, uint8_t y) { // x=0..16, y= 0..1

	uint8_t dr = x + 0x80;
	if (y == 1) dr += 0x40;
	LCD::LcdCommandWrite(dr);

}

void LCD::flashPrint(PGM_P s) {

	uint8_t c;

	while (0 != (c = pgm_read_byte(s++))) LCD::LcdDataWrite(c);

}

void LCD::print(char *string) {

	while (*string) LCD::LcdDataWrite(*string++);

}

void LCD::init() {

	TCCR1A |= (1 << COM1A1); // brightness pwm enable
	OCR1A = brightness[brightnessIdx]; // Port B bit 1

	DDRB = 0b00110011; // set direction to output for data bits 3, 2, and 1; and brightness pin
	DDRD = 0b11110000; // set direction to output for data bit 0; and contrast pin, LCD enable, and LCD command/data pin

	delay2(500); // allow 1/2 second delay for stabilization

	TCCR0A |= (1 << COM0A1); // contrast pwm enable
	OCR0A = lcdContrast; // Port D bit 6

	delay2(16); // wait for more than 15 msec
	PORTD &= ~(1 << 4); // set direction low for command byte transfer
	pushNibble(0b0011); // send (B0011) to DB7-4
	delay2(5); // wait for more than 4.1 msec
	pushNibble(0b0011); // send (B0011) to DB7-4
	delay2(1); // wait for more than 100 usec
	pushNibble(0b0011); // send (B0011) to DB7-4
	delay2(1); // wait for more than 100 usec
	pushNibble(0b0010); // send (B0010) to DB7-4 for 4bit
	delay2(1); // wait for more than 100 usec
	// ready to use normal LcdCommandWrite() function now!
	LcdCommandWrite(0b00101000); // 4-bit interface, 2 display lines, 5x8 font
	LcdCommandWrite(0b00001100); // display control:
	LcdCommandWrite(0b00000110); // entry mode set: increment automatically, no display shift

	//create the custom fonts:
	LcdCommandWrite(0b01001000); // set cgram
	for (uint8_t x = 0; x < 40; x++) LcdDataWrite(pgm_read_byte(&chars[x])); //write the character data to the character generator ram

	LcdCommandWrite(0b00000001); // clear display, set cursor position to zero
	LcdCommandWrite(0b10000000); // set dram to zero

}

void LCD::pushNibble(uint8_t value) { //db7=PB5, db6=PB4, db5 = PB0, db4  = PD7

	uint8_t t = pgm_read_byte(&lcdNybble[value & 15]);

	PORTB = PORTB & 0b11001110 | (t & 0b00110001); // transmit bits 3, 2, 1
	PORTD = PORTD & 0b01011111 | (t & 0b10000000) | 0b00100000; // transmit final bit 0, set enable high
	PORTD &= 0b11011111; // set enable low

}

void LCD::LcdCommandWrite(uint8_t value) {

	PORTD &= ~(1 << 4); // set direction low for command byte transfer
	pushNibble(value >> 4);
	pushNibble(value);
	delay2(5);

}

void LCD::LcdDataWrite(uint8_t value) {

	PORTD |= (1 << 4); // set direction high for data byte transfer
	pushNibble(value >> 4);
	pushNibble(value);
	delay2(5);

}

// this function will return the number of bytes currently free in RAM
extern int __bss_end;
extern int *__brkval;
int memoryTest(void) {
	int free_memory;
	if ((int) __brkval == 0)
		free_memory = ((int) &free_memory) - ((int) &__bss_end);
	else
		free_memory = ((int) &free_memory) - ((int) __brkval);
	return free_memory;
}

char *Trip::doFormat(unsigned long an[], uint8_t dispPos) {

	if (dispPos & dispTime) formatTime(an);
	else format(an[1]); // format the value to be output

	if (dispPos & dispRaw) return mBuff1;
	else {

		lblBuff[0] = pgm_read_byte(displayLabels[dispPos & 15]);
		lblBuff[1] = pgm_read_byte(displayLabels[dispPos & 15] + 1);

		if (!(dispPos & dispRight)) {

			if (lblBuff[0] == 32) lblBuff[0] = id;

		}

		return lblBuff;

	}

}

char *Trip::doFuelUsed(unsigned long inj[], uint8_t dispPos) {

	init64(tmp1, inj); // load tmp1 with time the fuel injector has been open
	init64(tmp2, 0, 1000ul); // load tmp2 with decimal point display factor
	mul64(tmp1, tmp2); // go shift decimal point
	init64(tmp2, 0, fuelfactor); // load tmp2 with the clock cycles to liquid units factor
	div64(tmp1, tmp2); // convert clock cycles into liquid units
	return doFormat(tmp1, dispPos | lblQtyIdx); // return gallons consumed

}

char *Trip::doFuelRate(unsigned long inj[], unsigned long elapsed[], uint8_t dispPos) {

	init64(tmp1, inj); // load tmp1 with time the fuel injector has been open
	init64(tmp2, 0, 1000ul); // load tmp2 with decimal point display factor
	mul64(tmp1, tmp2); // go shift decimal point
	init64(tmp2, cycle2hour); // load tmp2 with clock cycles to hours factor
	mul64(tmp1, tmp2); // build up numerator factor

	init64(tmp3, elapsed); // load tmp3 with total elapsed time since last reset
	init64(tmp2, 0, fuelfactor); // load tmp2 with the clock cycles to liquid units factor
	mul64(tmp3, tmp2); // build up denominator factor

	div64(tmp1, tmp3); // divide liquid units by hours to obtain fuel consumption per hour
	return doFormat(tmp1, dispPos | lblRateIdx); // return fuel consumption per hour

}

char *Trip::doDistance(unsigned long vss, uint8_t dispPos) {

	init64(tmp1, 0, vss); // load tmp1 with the vss pulse count
	init64(tmp2, 0, 1000ul); // load tmp2 with decimal point display factor
	mul64(tmp1, tmp2); // go shift decimal point
	init64(tmp2, 0, distancefactor); // load tmp2 with the vss pulses per unit distance factor
	div64(tmp1, tmp2); // convert vss pulse count into distance traveled
	return doFormat(tmp1, dispPos | lblDistIdx); // return distance traveled

}

char *Trip::doSpeed(unsigned long vss, unsigned long elapsed[], uint8_t dispPos) {

	init64(tmp1, 0, vss); // load tmp1 with the vss pulse count
	init64(tmp2, 0, 1000ul); // load tmp2 with decimal point display factor
	mul64(tmp1, tmp2); // go shift decimal point
	init64(tmp2, cycle2hour); // load tmp2 with clock cycles to hours factor
	mul64(tmp1, tmp2); // build up numerator factor

	init64(tmp3, elapsed); // load tmp3 with total elapsed time since last reset
	init64(tmp2, 0, distancefactor); // load tmp2 with the vss pulses per unit distance factor
	mul64(tmp3, tmp2); // build up denominator factor

	div64(tmp1, tmp3); // divide distance traveled by elapsed time
	return doFormat(tmp1, dispPos | lblSpeedIdx); // return average distance traveled per hour

}

char *Trip::fuelUsed(uint8_t dispPos) {

	return doFuelUsed(injOpenTime, dispPos); // return gallons consumed

}

char *Trip::idleFuelUsed(uint8_t dispPos) {

	return doFuelUsed(idleInjOpenTime, dispPos); // return gallons consumed while idling

}

char *Trip::fuelRate(uint8_t dispPos) {

	return doFuelRate(injOpenTime, elapsedTime, dispPos); // return fuel consumption per hour

}

char *Trip::idleFuelRate(uint8_t dispPos) {

	return doFuelRate(idleInjOpenTime, idleTime, dispPos); // return fuel consumption per hour spent idling

}

char *Trip::distance(uint8_t dispPos) {

	return doDistance(vssPulses, dispPos); // return distance traveled

}

char *Trip::eocDistance(uint8_t dispPos) {

	return doDistance(EOCPulses, dispPos); // return distance traveled while performing EOC

}

char *Trip::speed(uint8_t dispPos) {

	return doSpeed(vssPulses, elapsedTime, dispPos); // return average distance traveled per hour

}

char *Trip::eocSpeed(uint8_t dispPos) {

	return doSpeed(EOCPulses, EOCTime, dispPos); // return average distance traveled per hour while performing EOC

}

char *Trip::fuelEcon(uint8_t dispPos) {

	init64(tmp1, 0, 999999000ul);
	init64(tmp2, 0, 0);
	if (metricIdx == 1) {

		if (vssPulses == 0) return doFormat(tmp1, dispPos | lblEconIdx);

		if ((injOpenTime[0] == 0) && (injOpenTime[1] == 0)) return doFormat(tmp2, dispPos | lblEconIdx); // who doesn't like to see 0?  :)

		init64(tmp1, injOpenTime); // load tmp1 with time the fuel injector has been open
		init64(tmp2, 0, distancefactor); // load tmp2 with the vss pulses per unit distance factor
		mul64(tmp1, tmp2); // build up numerator term

		init64(tmp3, 0, vssPulses); // load tmp3 with the vss pulse count
		init64(tmp2, 0, fuelfactor); // load tmp2 with the clock cycles to liquid units factor
		mul64(tmp3, tmp2); // build up denominator term

		init64(tmp2, 0, 100000ul); // multiply numerator by 1000 for formatting, and by 100 for L / >100< km

	} else {

		if ((injOpenTime[0] == 0) && (injOpenTime[1] == 0)) return doFormat(tmp1, dispPos | lblEconIdx); // who doesn't like to see 999999?  :)

		if (vssPulses == 0) return doFormat(tmp2, dispPos | lblEconIdx);

		init64(tmp1, 0, vssPulses); // load tmp1 with the vss pulse count
		init64(tmp2, 0, fuelfactor); // load tmp2 with the clock cycles to liquid units factor
		mul64(tmp1, tmp2); // build up numerator term

		init64(tmp3, injOpenTime); // load tmp1 with time the fuel injector has been open
		init64(tmp2, 0, distancefactor); // load tmp2 with the vss pulses per unit distance factor
		mul64(tmp3, tmp2); // build up denominator term

		init64(tmp2, 0, 1000ul); // multiply numerator by 1000 for formatting

	}

	mul64(tmp1, tmp2); // perform decimal point adjustment
	div64(tmp1, tmp3); // perform ratio calculation
	return doFormat(tmp1, dispPos | lblEconIdx); // return fuel economy

}

//return the seconds as a time mmm.ss, eventually hhh:mm too
char *Trip::time(uint8_t dispPos) {

	return doFormat(elapsedTime, dispPos | lblTimeIdx | dispTime); // return formatted time

}

void Trip::reset(void) {

	init64(elapsedTime, 0, 0);
	init64(injOpenTime, 0, 0);
	injPulses = 0;
	vssPulses = 0;

	init64(idleTime, 0, 0);
	init64(idleInjOpenTime, 0, 0);
	init64(EOCTime, 0, 0);
	EOCPulses = 0;

}

void Trip::update(Trip t) {

	add64(elapsedTime, t.elapsedTime);
	add64(injOpenTime, t.injOpenTime);
	injPulses += t.injPulses;
	vssPulses += t.vssPulses;

	add64(idleTime, t.idleTime);
	add64(idleInjOpenTime, t.idleInjOpenTime);
	add64(EOCTime, t.EOCTime);
	EOCPulses += t.EOCPulses;

}

//the standard 64 bit math brings in 5000+ bytes
//these bring in 1214 bytes, and everything is pass by reference

void init64(unsigned long an[], unsigned long ann[]) {

	an[0] = ann[0];
	an[1] = ann[1];

}

void init64(unsigned long an[], unsigned long bigPart, unsigned long littlePart) {

	an[0] = bigPart;
	an[1] = littlePart;

}

void shl64(unsigned long an[]) { // left shift 64 bit "number"

	an[0] <<= 1;
	if (an[1] & 0x80000000) an[0]++;
	an[1] <<= 1;

}

void shr64(unsigned long an[]) { // right shift 64 bit "number"

	an[1] >>= 1;
	if (an[0] & 0x1) an[1] += 0x80000000;
	an[0] >>= 1;

}

void add64(unsigned long an[], unsigned long ann[]) { // add ann to an

	an[0] += ann[0];
	if (an[1] + ann[1] < ann[1]) an[0]++;
	an[1] += ann[1];

}

void sub64(unsigned long an[], unsigned long ann[]) { // subtract ann from an

	an[0] -= ann[0];
	if (an[1] < ann[1]) an[0]--;
	an[1] -= ann[1];

}

void div64(unsigned long num[], unsigned long den[]) { // divide num by den

	unsigned long quot[2];
	unsigned long qbit[2];

	init64(quot, 0, 0);
	init64(qbit, 0, 1);

	if (den[0] == 0) {

		if (den[1] == 0) { // numerator not 0, but denominator 0, so quotient is undefined

			init64(num, 0xffffffff, 0xffffffff);
			return;

		}

		den[0] = den[1]; // shift denominator left by 32 bits
		den[1] = 0;
		init64(qbit, 1, 0); // shift quotient bit left by 32 bits

	}

	if ((num[0] == 0) && (num[1] == 0)) { // numerator 0, so quotient is 0

		init64(num, 0, 0);
		return;

	}

	while (!(den[0] & 0x80000000)) {

		shl64(den);
		shl64(qbit);

	}

	while ((qbit[0] != 0) || (qbit[1] != 0)) {

		if ((den[0] < num[0]) || ((den[0] == num[0]) && (den[1] <= num[1]))) { // if den <= num

			sub64(num, den); // subtract numerator from denominator
			quot[0] |= qbit[0]; // mark corresponding bit in quotient
			quot[1] |= qbit[1];

		}

		shr64(den);
		shr64(qbit);

	}

	init64(num, quot); // remainder now in num, but using it to return quotient for now

}

void mul64(unsigned long an[], unsigned long ann[]) { // multiply num by den

	unsigned long p[2];
	unsigned long y[2];

	init64(p, 0, 0);
	init64(y, ann);
	while ((y[0] != 0) || (y[1] != 0)) {

		if (y[1] & 1) add64(p, an);
		shl64(an);
		shr64(y);

	}

	init64(an, p);

}

char *format(unsigned long num) {
	uint8_t dp = 3;

	while (num > 999999) {
		num /= 10;
		dp++;
		if (dp == 5)
			break; // We'll lose the top numbers like an odometer
	}
	if (dp == 5)
		dp = 99; // We don't need a decimal point here.

	// Round off the non-printed value.
	if ((num % 10) > 4)
		num += 10;
	num /= 10;
	uint8_t x = 6;
	while (x > 0) {
		x--;
		if (x == dp) { //time to poke in the decimal point?{
			mBuff1[x] = '.';
		} else {
			mBuff1[x] = '0' + (num % 10);//poke the ascii character for the digit.
			num /= 10;
		}
	}
	mBuff1[6] = 0;
	return mBuff1;
}

char *uformat(unsigned long val, char *str) {

	unsigned long d = 1000000000ul;
	uint8_t b;
	uint8_t c;
	uint8_t q;

	c = 32;
	for (uint8_t p = 0; p < 10; p++) {

		q = val / d;
		b = q + '0';

		if ((b == '0') && (p < 9)) b = c; else c = '0';
		str[p] = b;
		val -= q * d;
		d /= 10;
	}

	str[10] = 0;
	return str;

}

unsigned long rformat(char *val) {

	unsigned long d = 1000000000ul;
	unsigned long v = 0ul;
	uint8_t c;

	for (uint8_t p = 0; p < 10; p++) {

		c = val[p];
		if (c == 32) c = 0; else c -= '0';
		v += d * c;
		d /= 10;

	}

	return v;

}

char *formatTime(unsigned long cycles[]) {

	char *str;

	init64(tmp1, cycles); // load elapsed time into tmp1
	init64(tmp2, cycle2second); // load conversion factor from clock cycles to seconds into tmp2
	div64(tmp1, tmp2); // perform the conversion

	tmp1[0] = tmp1[1] / 60; // perform second extraction
	tmp1[1] -= tmp1[0] * 60;
	tmp2[1] = tmp1[0] / 60; // perform minutes extraction
	tmp1[0] -= tmp2[1] * 60;
	tmp2[0] = tmp2[1] / 24; // perform hours extraction
	tmp2[1] -= tmp2[0] * 24;

	tmp1[1] += tmp1[0] * 100ul + tmp2[1] * 10000ul + 1000000ul; // end term is to force zeros

	str = uformat(tmp1[1], mBuff2);
	for (int x = 0; x < 7; x++) mBuff1[x] = str[x + 4];
	return mBuff1;

}

unsigned long editParm(uint8_t parmIdx, unsigned long value, uint8_t leftPos, unsigned long maxValue) {

	char *fmtv = uformat(value, mBuff1);
	char *maxv = uformat(maxValue, mBuff2);
	uint8_t n; // temporary digit storage
	uint8_t p; // edit cursor position
	uint8_t q; // screen cursor position
	uint8_t r; // cursor move flag
	uint8_t z; // editing flag

	LCD::LcdCommandWrite(0b00000001); // clear display screen
	LCD::flashPrint(parmLabels[parmIdx]); // print parameter name at top left
	LCD::gotoXY(10, 1); // go to next line
	LCD::print(" OK XX"); // print two command options, 'enter' and 'cancel'
	LCD::LcdCommandWrite(0b00001111); // turn on blinking cursor

	for (int x = 9; x >= 0; x--) if (fmtv[x] != ' ') p = x; // do a nice thing and put the edit cursor at the first non zero number

	z = (leftPos == 9) ? 1 : 0;
	while (true) {

		LCD::gotoXY(0, 1); // go to start of edit line
		LCD::print(fmtv); // print numeric-formatted line representing value to be edited
		// adjust screen cursor to match edit cursor
		if (p < 10) q = p; // if edit cursor is within number,
		else if (p == 10) q = 11; // if edit cursor is at 'enter'
		else if (p == 11) q = 14; // if edit cursor is at 'cancel'

		LCD::gotoXY(q, 1); // move screen cursor to commanded position

		while (timerStatus & timerKeyWait); // wait for a keypress
		timerStatus |= timerKeyWait; // reset keypress flag
		r = 0; // reset movement flag

		if (buttonState == pressBothLR) { // left and right = cancel

			p = 11; // move edit cursor to 'cancel' position
			r = 254; // signal exit

		} else if (buttonState == pressOnlyL) { // left = move edit cursor left

			r = 255;

		} else if (buttonState == pressOnlyR) { // right = move edit cursor right

			r = 1;

		} else if (buttonState == pressOnlyC) { // middle = select position at edit cursor

			if (p > 9) { // 'enter' or 'cancel' selected

				r = 254; // signal exit

			} else { // numeric digit selected

				n = fmtv[p]; // save numeric digit
				q = n; // copy over to working digit
				if (q == 32) q = 0; // if this is a leading space, then convert to zero
				else q -= '0'; // otherwise convert from ascii to actual digit
				q++; // adjust working digit
				if (q > 9) q = 0; // handle ordinary digit rollover
				if (q == 0) { // if working digit is zero
					if (p == 0) q = 32; // store a leading blank
					else if (p == 9) q = '0';
					else if (fmtv[p - 1] == 32) q = 32; // store a leading blank
					else q = '0'; // store a ascii zero
				} else q += '0'; // or turn digit back into ascii

				fmtv[p] = q; // store working digit for comparison

				for (q = 0; q < 10; q++) {

					if (fmtv[q] < maxv[q]) q = 10;
					else if (fmtv[q] > maxv[q]) {

						q = 10;
						fmtv[p] = n;
						z++;

					}

				}

				if (z == 2) fmtv = uformat(maxValue, mBuff1); // if user attempted to go past max value 2 times in row, max out value
				else if (z == 3) { // otherwise, alternate between max and min values

					fmtv = uformat(0, mBuff1);
					z = 1;

				}

				if (parmIdx == contrastIdx) OCR0A = rformat(fmtv); // adjust contrast dynamically

			}

		}

		if (r == 254) {

			LCD::LcdCommandWrite(0b00001100); // turn off blinking cursor
			if (p == 10) return rformat(fmtv); // return edited value
			else return value; // return original value

		}

		while (r)
		{

			z = (leftPos == 9) ? 1 : 0; // reset edit flag
			p += r; // adjust edit cursor
			if (p == 12) p = leftPos; // handle wrap-around
			else if ((p > 12) || (p < leftPos)) p = 11;

			if (p > 9) r = 0; // if edit cursor moved to 'enter' or 'cancel'
			else if (fmtv[p] != 32) r = 0; // if edit cursor moved to a numeric digit
			else if (fmtv[p + 1] != 32) r = 0; // if edit cursor is at a space, but there's a numeric digit immediately to the right

		}

	}

}

unsigned long editUL(uint8_t parmIdx, unsigned long value) {

	return editParm(parmIdx, value, 0, 4294967295ul);

}

uint8_t editByte(uint8_t parmIdx, uint8_t value) {

	return editParm(parmIdx, value, 7, 255ul);

}

uint8_t editBit(uint8_t parmIdx, uint8_t value) {

	return editParm(parmIdx, value, 9, 1ul);

}

void editGuino(void) {

	uint8_t parmIdx = 0;
	unsigned long v;

	while (true) {

		LCD::LcdCommandWrite(0b00000001); // clear display screen
		LCD::flashPrint(parmLabels[parmIdx]); // print parameter name at top left
		LCD::gotoXY(10, 1); // go to next line

		while (timerStatus & timerKeyWait); // wait for a keypress
		timerStatus |= timerKeyWait; // reset keypress flag

		if (buttonState == pressBothLR)	{ // left and right = exit

			parmIdx = injEdgeIdx * injectorEdgeBit + metricIdx * metricBit + serialIdx * serialTransmitBit;
			parms[storage1Index] = (vssPause << 24) | (lcdContrast << 16) | (parmIdx << 8) | rpmfactor;

			eeprom_write_byte((unsigned char *) 0, guinosig);
			eeprom_write_byte((unsigned char *) 1, parmsLength);

			parmIdx = 0;

			for (int x = 4; parmIdx < parmsLength; x += 4) {

				v = parms[parmIdx];
				eeprom_write_byte((unsigned char *) x, (v >> 24) & 255);
				eeprom_write_byte((unsigned char *) x + 1, (v >> 16) & 255);
				eeprom_write_byte((unsigned char *) x + 2, (v >> 8) & 255);
				eeprom_write_byte((unsigned char *) x + 3, (v) & 255);
				parmIdx++;

			}
			initGuino();

			return;

		} else if (buttonState == pressOnlyL) { // left = move up through parameters

			if (parmIdx == 0) parmIdx = maxParmPos;
			parmIdx--;

		} else if (buttonState == pressOnlyR) { // right = move down through parameters

			parmIdx++;
			if (parmIdx == maxParmPos) parmIdx = 0;

		} else if (buttonState == pressOnlyC) { // middle = select position at edit cursor

			if (parmIdx == contrastIdx) lcdContrast = editByte(contrastIdx, lcdContrast);
			else if (parmIdx == injPulsesPer2Revolutions) rpmfactor = editByte(injPulsesPer2Revolutions, rpmfactor);
			else if (parmIdx == vssPausePos) vssPause = editByte(vssPausePos, vssPause);
			else if (parmIdx == injEdgePos) injEdgeIdx = editBit(injEdgePos, injEdgeIdx);
			else if (parmIdx == metricPos) metricIdx = editBit(metricPos, metricIdx);
			else if (parmIdx == SerialPos) serialIdx = editBit(SerialPos, serialIdx);
			else parms[parmIdx] = editUL(parmIdx, parms[parmIdx]);

		}

	}

}

unsigned long doFactor(unsigned long source, unsigned long numer, unsigned long denom) {

	init64(tmp1, 0, source);
	init64(tmp2, 0, numer);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, denom);
	div64(tmp1, tmp2);
	return tmp1[1];

}

void initGuino(void) { //edit all the parameters

	uint8_t flags1;

	vssPause = (parms[storage1Index] >> 24) & 255;
	lcdContrast = (parms[storage1Index] >> 16) & 255;
	flags1 = (parms[storage1Index] >> 8) & 255;
	rpmfactor = parms[storage1Index] & 255;

	injEdgeIdx = ((flags1 & injectorEdgeBit) ? 1 : 0);
	metricIdx = ((flags1 & metricBit) ? 1 : 0);
	serialIdx = ((flags1 & serialTransmitBit) ? 1 : 0);

	cli(); // disable interrupts while messing with fuel injector sense
	EIMSK = EIMSK & 0b11111100; // disable fuel injector sense interrupts
	EICRA = EICRA & 0b11110000 | (injEdgeIdx ? 0b00001011 : 0b00001110);  // set injector sense pin directions (R/F or F/R)
	EIFR = 0b00000011; // manually clear fuel injector sense interrupts
	injOpenStart = nil; // reset fuel injector capture mechanism
	EIMSK = EIMSK | 0b00000011; // enable fuel injector sense interrupts
	sei(); // re-enable interrupts

	distancefactor = parms[vssPulsesPerMileIdx];

	fuelfactor = doFactor(parms[microSecondsPerGallonIdx], 1000ul, cycle2ms); // convert microseconds into timer2 clock cycles
	injSettleTime = doFactor(parms[injectorSettleTimeIdx], 1000ul, cycle2ms); // convert microseconds into timer2 clock cycles
	sleepfactor = doFactor(parms[currentTripResetTimeoutUSIdx], tickfactor, 128000000ul); // convert microseconds into timer2 ticks

	if (metricIdx == 1){

		distancefactor = doFactor(distancefactor, 1000000ul, 1609344ul); // convert miles into kilometers
		fuelfactor = doFactor(fuelfactor, 1000000000ul, 3785411784ul); // convert gallons into liters

	}

}

void delay2(unsigned long ms) {

	timerDelay = tickfactor * ms / 128000; // request a set number of timer tick delays per millisecond
	timerStatus |= timerStartDelay; // signal request to timer
	while (timerStatus & timerStartDelay); // wait for timer to acknowledge request
	while (timerStatus & timerExecDelay); // wait for timer to complete request

}

void simpletx(char * string) {
	if (UCSR0B != (1 << TXEN0)) { //do we need to init the uart?
		UBRR0H = (unsigned char) (myubbr >> 8);
		UBRR0L = (unsigned char) myubbr;
		UCSR0B = (1 << TXEN0);//Enable transmitter
		UCSR0C = (3 << UCSZ00);//N81
	}
	while (*string) {
		while (!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = *string++; //send the data
	}
}

int main(void) {

	uint8_t b;
	uint8_t c;
	uint8_t p;
	unsigned long v;

	TCCR0A = TCCR0A & 0b00001100 | 0b00000011; // configure timer 0 for fast PWM
	TCCR1A = TCCR1A & 0b00001100 | 0b00000001; // configure timer 1 for phase-correct PWM
	TCCR2A = TCCR2A & 0b00001100 | 0b00000011; // configure timer 2 for fast PWM

	TCCR0B = TCCR0B & 0b00110000 | 0b00000011; // set timer 0 divisor to 64
	TCCR1B = TCCR1B & 0b00110000 | 0b00000011; // set timer 1 divisor to 64
	TCCR2B = TCCR2B & 0b00110000 | 0b00000011; // set timer 2 divisor to 64

	TIMSK0 &= 0b11111000; // disable all timer 0 interrupts
	TIMSK1 &= 0b11111000; // disable all timer 1 interrupts
	TIMSK2 = TIMSK2 & 0b11111000 | 0b00000001; // enable timer 2 overflow interrupt only

	ADCSRA = ADCSRA & 0b01111000 |  0b10000111; // enable ADC, and set frequency to 1/128 of system timer

	UCSR0B = 0; // disable serial uart pins

	init64(tmp1, 0, cycle2ms);
	init64(tmp2, 0, 1000ul);
	mul64(tmp1, tmp2);
	init64(cycle2second, tmp1);
	init64(tmp2, 0, 3600ul);
	mul64(tmp1, tmp2);
	init64(cycle2hour, tmp1);

	timer2_overflow_count = 0;

	b = eeprom_read_byte((unsigned char *) 0);
	c = eeprom_read_byte((unsigned char *) 1);
	if (b == guinosig) {

		p = 0;

		for (int x = 4; p < c; x += 4) {

			v = eeprom_read_byte((unsigned char *) x);
			v = (v << 8) + eeprom_read_byte((unsigned char *) x + 1);
			v = (v << 8) + eeprom_read_byte((unsigned char *) x + 2);
			parms[p] = (v << 8) + eeprom_read_byte((unsigned char *) x + 3);
			p++;

		}
		newRun = 1;

	}

	initGuino();

	current.reset();
	tank.reset();

	instant.id = 'i';
	current.id = 'c';
	tank.id = 't';

	timerStatus = timerKeyWait;

	sei();

	LCD::init();
	LCD::LcdCommandWrite(0b00000001); // clear display, set cursor position to zero
	LCD::LcdCommandWrite(0b00010000); // set dram to zero
	LCD::gotoXY(0, 0);
	LCD::flashPrint(PSTR("OpenGauge       "));
	LCD::gotoXY(0, 1);
	LCD::flashPrint(PSTR("MPGuino v0.86tav"));

	PORTC |= (1 << 5) | (1 << 4) | (1 << 3); // enable button pullup resistors

	PCMSK1 |= (1 << PCINT8); // enable interrupt for vss pin
	PCMSK1 |= (1 << PCINT11); // enable interrupt for left button
	PCMSK1 |= (1 << PCINT12); // enable interrupt for middle button
	PCMSK1 |= (1 << PCINT13); // enable interrupt for right button
	PCICR |= (1 << PCIE1);

	delay2(1500); // show splash screen for 1.5 seconds

	if (newRun != 1) editGuino(); // go through the initialization screen

	while (true) {

		if (!(timerStatus & timerExecCycle)) { // if not currently executing a cycle

			if (timerStatus & timerAsleep) current.reset(); // if system is in activity timeout, then reset current trip data

			timerStatus |= timerStartCycle; // start a new cycle
			while (timerStatus & timerStartCycle); // wait for new cycle to start (also copy system readings to instant)

			current.update(instant); //use instant to update current
			tank.update(instant); //use instant to update tank

			if (serialIdx) {

				init64(tmp1, instant.injOpenTime); // load tmp1 with fuel injector raw open time in clock cycles
				init64(tmp2, 0, 1000ul);
				mul64(tmp1, tmp2);
				init64(tmp2, 0, cycle2ms); // load tmp2 with clock cycles to milliseconds factor
				div64(tmp1, tmp2); // convert clock cycles into milliseconds

				simpletx(instant.fuelEcon(dispRaw)); // transmit average fuel economy  for the past loop
				simpletx(",");
				simpletx(instant.speed(dispRaw)); // transmit average vehicle speed for the past loop
				simpletx(",");
				simpletx(format(tmp1[1])); // transmit fuel injector raw open time for the past loop
				simpletx(",");
				simpletx(format(instant.injPulses * 1000)); // transmit fuel injector pulse count for the past loop
				simpletx(",");
				simpletx(format(instant.vssPulses * 1000)); // transmit vss pulse count for the past loop
				simpletx("\n");

			}

		}

		if (holdDisplay == 0) {

			LCD::gotoXY(0, 0);
			displayFuncs[screen](); // call the appropriate display routine

		}

		timerStatus |= timerMarkCycle; // mark how many timer ticks it took to complete this iteration
		while ((timerStatus & timerExecCycle) && (timerStatus & timerKeyWait)); // wait for cycle to end, or for a keypress

		if (!(timerStatus & timerExecCycle) && (holdDisplay != 0)) holdDisplay--;
		if (!(timerStatus & timerKeyWait)) { // see if any buttons were pressed, display a brief message if so

			b = 0;
			timerStatus |= timerKeyWait; // reset keypress flag
			holdDisplay = 2;
			LCD::gotoXY(0, 0);
			if (buttonState == pressBothLR) { // left and right = enter setup menu

				editGuino();
				holdDisplay = 0;

			} else if (buttonState == pressOnlyL) { // left = rotate through screeens to the left

				b = 255;

			} else if (buttonState == pressOnlyC) { // middle = cycle through brightness settings

				brightnessIdx++;
				if (brightnessIdx >= brightnessLength) brightnessIdx = 0;
				OCR1A = brightness[brightnessIdx];

				LCD::flashPrint(PSTR("Brightness "));
				LCD::LcdDataWrite('0' + brightnessIdx);
				LCD::print(" ");

			} else if (buttonState == pressOnlyR) { // right = rotate through screeens to the right

				b = 1;

			} else if (buttonState == longPressBothLC) { // long (left and middle) = tank reset

				tank.reset();
				LCD::flashPrint(PSTR("Tank Reset "));

			} else if (buttonState == longPressBothRC) { // long (right and middle) = current reset

				current.reset();
				LCD::flashPrint(PSTR("Current Reset "));

			} else if (buttonState == longPressOnlyC) { // long middle = display CPU utilization for this screen

				doDisplayCPUutil();
				LCD::flashPrint(PSTR("        "));

			}

			if (b) { // perform rotation throughout screens

				screen += b;
				if (screen == displayFuncSize) screen = 0;
				if (screen > displayFuncSize) screen = displayFuncSize - 1;
				displayFuncs[screen](); // call the appropriate display routine
				LCD::gotoXY(0, 0);
				LCD::flashPrint(displayFuncNames[screen]); // briefly display screen name

			}

		}

	}

	return 0;
}
