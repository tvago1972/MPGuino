//mpguino, open source fuel consumption system
//GPL Software, mass production use rights reserved by opengauge.org, personal use is perfectly fine , no warranties expressed or implied

//Special thanks to the good folks at ecomodder.com, ardunio.cc, avrfreaks.net, cadsoft.de, atmel.com,
//and all the folks who donate their time and resources and share their experiences freely

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
typedef uint8_t boolean;
typedef uint8_t byte;
#define RISING 3
#define FALLING 2
#define loopsPerSecond 2 // how many times will we try and loop in a second
//use with 20mhz
#define cyclesperhour 4500
#define dispadj 800
#define dispadj2 1250
#define looptime 1250000ul/loopsPerSecond //1/2 second
#define myubbr (20000000/16/9600-1)
#define injhold (parms[injectorSettleTimeIdx]*5)/4

#define outhi(port,pin) PORT##port |= ( 1 << P##port##pin )
#define outlo(port,pin) PORT##port &= ~( 1 << P##port##pin )
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

//use with 16mhz, not tested
//#define cyclesperhour 3600
//#define dispadj 1000
//#define dispadj2 1250
//#define looptime 1000000ul/loopsPerSecond //1/2 second
//#define myubbr (16000000/16/9600-1)
//#define injhold parms[injectorSettleTimeIdx]

//#define usedefaults true
void toggleVSS(void);
unsigned long microSeconds(void);
unsigned long elapsedMicroseconds(unsigned long startMicroSeconds,
		unsigned long currentMicroseconds);
unsigned long elapsedMicroseconds(unsigned long startMicroSeconds);
void setup(void);
void mainloop(void);
void dispv(byte lidx, unsigned long num);
char* format(unsigned long num);
void doDisplayCustom();
void doDisplayEOCIdleData();
void doDisplayInstantCurrent();
void doDisplayInstantTank();
void doDisplayBigInstant();
void doDisplayBigCurrent();
void doDisplayBigTank();
void doDisplayCurrentTripData(void);
void doDisplayTankTripData(void);
void doDisplaySystemInfo(void);
void displayTripCombo(byte lidx1, unsigned long v1, byte lidx2, unsigned long v2, byte lidx3, unsigned long v3, byte lidx4, unsigned long v4);
void tDisplay(void * r);
int memoryTest();
unsigned long instantmph();
unsigned long instantmpg();
unsigned long instantgph();
void bigNum(unsigned long t, PGM_P txt1);
void init64(unsigned long an[], unsigned long bigPart,
				unsigned long littlePart);
void shl64(unsigned long an[]);
void shr64(unsigned long an[]);
void add64(unsigned long an[], unsigned long ann[]);
void sub64(unsigned long an[], unsigned long ann[]);
boolean eq64(unsigned long an[], unsigned long ann[]);
boolean lt64(unsigned long an[], unsigned long ann[]);
void div64(unsigned long num[], unsigned long den[]);
void mul64(unsigned long an[], unsigned long ann[]);
void save();
byte load();
char * uformat(unsigned long val);
unsigned long rformat(char * val);
void editParm(byte parmIdx);
void editGuino(void);
void initGuino(void);
unsigned long millis2();
void delay2(unsigned long ms);

void simpletx(char * string);
unsigned long parms[] = { // default values
  55ul,
  8208ul,
  500000000ul,
  3ul,
  420000000ul,
  10300ul,
  500ul,
  2400ul,
  0ul,
  2ul,
  0ul,
  0ul
};

PROGMEM prog_char fontBigNum[]={
  B11111,B11111,B11111,B00000,B00000,B00000,B00000,B00000,
  B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111,
  B11111,B11111,B11111,B00000,B00000,B11111,B11111,B11111,
  B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111,
  B00000,B00000,B00000,B00000,B00000,B01110,B01110,B01110};

#define fontBigNumLength (sizeof(fontBigNum)/sizeof(prog_char)) // custom character array size

PROGMEM prog_char fString_00[] = "Contrast";
PROGMEM prog_char fString_01[] = "VSS Pulses/Mile";
PROGMEM prog_char fString_02[] = "MicroSec/Gallon";
PROGMEM prog_char fString_03[] = "Pulses/2 revs";
PROGMEM prog_char fString_04[] = "Timout(microSec)";
PROGMEM prog_char fString_05[] = "Tank Gal * 1000";
PROGMEM prog_char fString_06[] = "Injector DelayuS";
PROGMEM prog_char fString_07[] = "Weight (lbs)";
PROGMEM prog_char fString_08[] = "Scratchpad(odo?)";
PROGMEM prog_char fString_09[] = "VSS Delay ms";
PROGMEM prog_char fString_10[] = "InjTrg 0-Dn 1-Up";
PROGMEM prog_char fString_11[] = "Metric (1=yes)";
PROGMEM prog_char fString_12[] = "Custom  ";
PROGMEM prog_char fString_13[] = "Instant/Current ";
PROGMEM prog_char fString_14[] = "Instant/Tank ";
PROGMEM prog_char fString_15[] = "BIG Instant ";
PROGMEM prog_char fString_16[] = "BIG Current ";
PROGMEM prog_char fString_17[] = "BIG Tank ";
PROGMEM prog_char fString_18[] = "Current ";
PROGMEM prog_char fString_19[] = "Tank ";
PROGMEM prog_char fString_20[] = "EOC/Idle ";
PROGMEM prog_char fString_21[] = "CPU Monitor ";

PROGMEM prog_char fString_22[] = {4, 1, 4, 32, 0};
PROGMEM prog_char fString_23[] = {1, 4, 32, 32, 0};
PROGMEM prog_char fString_24[] = {3, 3, 4, 32, 0};
PROGMEM prog_char fString_25[] = {1, 3, 4, 32, 0};
PROGMEM prog_char fString_26[] = {4, 2, 4, 32, 0};
PROGMEM prog_char fString_27[] = {4, 3, 3, 32, 0};
PROGMEM prog_char fString_28[] = {4, 3, 3, 32, 0};
PROGMEM prog_char fString_29[] = {1, 1, 4, 32, 0};
PROGMEM prog_char fString_30[] = {4, 3, 4, 32, 0};
PROGMEM prog_char fString_31[] = {4, 3, 4, 32, 0};

PROGMEM prog_char fString_32[] = {4, 2, 4, 0};
PROGMEM prog_char fString_33[] = {2, 4, 2, 0};
PROGMEM prog_char fString_34[] = {4, 2, 2, 0};
PROGMEM prog_char fString_35[] = {2, 2, 4, 0};
PROGMEM prog_char fString_36[] = {32, 32, 4, 0};
PROGMEM prog_char fString_37[] = {2, 2, 4, 0};
PROGMEM prog_char fString_38[] = {4, 2, 4, 0};
PROGMEM prog_char fString_39[] = {32, 4, 32, 0};
PROGMEM prog_char fString_40[] = {4, 2, 4, 0};
PROGMEM prog_char fString_41[] = {2, 2, 4, 0};

PROGMEM prog_char fString_42[] = " MPG";
PROGMEM prog_char fString_43[] = " L/K";
PROGMEM prog_char fString_44[] = "INST";
PROGMEM prog_char fString_45[] = "CURR";
PROGMEM prog_char fString_46[] = "TANK";

PROGMEM prog_char fString_47[] = "MG"; // fuel economy
PROGMEM prog_char fString_48[] = "LK";
PROGMEM prog_char fString_49[] = "MH"; // speed
PROGMEM prog_char fString_50[] = "KH";
PROGMEM prog_char fString_51[] = "GH"; // fuel consumption
PROGMEM prog_char fString_52[] = "LH";
PROGMEM prog_char fString_53[] = " G"; // fuel quantity
PROGMEM prog_char fString_54[] = " L";
PROGMEM prog_char fString_55[] = " M"; // distance
PROGMEM prog_char fString_56[] = " K";
PROGMEM prog_char fString_57[] = "EM"; // EOC distance
PROGMEM prog_char fString_58[] = "EK"; // EOC distance

PROGMEM PGM_P displayLabels[] = {
  fString_47,
  fString_48,
  fString_49,
  fString_50,
  fString_51,
  fString_52,
  fString_53,
  fString_54,
  fString_55,
  fString_56,
  fString_57,
  fString_58
};

PROGMEM PGM_P fuelEconLabel[] = {
  fString_42,
  fString_43,
  fString_44,
  fString_45,
  fString_46
};

PROGMEM PGM_P bigNumChars1[] = {
  fString_22,
  fString_23,
  fString_24,
  fString_25,
  fString_26,
  fString_27,
  fString_28,
  fString_29,
  fString_30,
  fString_31
};

PROGMEM PGM_P bigNumChars2[] = {
  fString_32,
  fString_33,
  fString_34,
  fString_35,
  fString_36,
  fString_37,
  fString_38,
  fString_39,
  fString_40,
  fString_41
};

PROGMEM PGM_P parmLabels[] = {
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
  fString_11
};

PROGMEM PGM_P displayFuncNames[] = {
  fString_12,
  fString_13,
  fString_14,
  fString_15,
  fString_16,
  fString_17,
  fString_18,
  fString_19,
  fString_20,
  fString_21
};

#define displayFuelEconIdx 0
#define displaySpeedIdx 2
#define displayFuelConsIdx 4
#define displayFuelQuanIdx 6
#define displayDistIdx 8
#define displayEOCIdx 10

byte brightness[] = { 0, 41, 84, 128 }; //middle button cycles through these brightness settings
#define brightnessLength (sizeof(brightness)/sizeof(byte)) //array size
byte brightnessIdx = 1;

#define contrastIdx 0  // do contrast first to get display dialed in
#define vssPulsesPerMileIdx 1
volatile unsigned long distancefactor;

#define microSecondsPerGallonIdx 2
volatile unsigned long fuelfactor;

#define injPulsesPer2Revolutions 3
#define currentTripResetTimeoutUSIdx 4
#define tankSizeIdx 5
#define injectorSettleTimeIdx 6
#define weightIdx 7
#define scratchpadIdx 8
#define vsspauseIdx 9
#define injEdgeIdx 10
#define metricIdx 11
#define parmsLength (sizeof(parms)/sizeof(unsigned long)) /*  //array size      */

unsigned long injectorSettleTime;
unsigned long vssPauseCount;

#define nil 3999999999ul

#define guinosigold 0b10100101
#define guinosig 0b11100111

#define vssBit ( 1 << 0 )
#define lbuttonBit ( 1 << 3 )
#define mbuttonBit ( 1 << 4 )
#define rbuttonBit ( 1 << 5 )

typedef void (* pFunc)(void);//type for display function pointers

volatile unsigned long timer2_overflow_count=0;
volatile int countsVSS=0;

// VSS debounce and button debounce interrupt
//   button pin states are polled every 1.024 milliseconds
//   if button pin states change, then a 10.24 ms wait is started
//   if button pin states change during the 10.24 ms wait, the wait gets restarted
//   if the 10.24 ms wait finishes, the program button state is updated with the new button pin states

ISR( TIMER2_OVF_vect )             // this ISR gets called every 1.024 milliseconds (we will call that a millisecond for our purposes)
{
  static uint8_t pinBtnCurrState=0;
  static uint8_t pinBtnPrevState=0;
  static int countsButtons=0;

  pinBtnCurrState=PINC&buttonsUp; // read relevant pins (bypassing digitalRead for interrupt performance)
  timer2_overflow_count++;         // update timer overflow countdown
  if(countsVSS!=0){                // if VSS event countdown is in progress,
    if(!(countsVSS--)) toggleVSS();  // if VSS event countdown reaches zero, toggle VSS state
  }

  if(pinBtnCurrState^pinBtnPrevState) countsButton=10; // if button pin states changed during the last ISR period, (re)start a button countdown
  else if(countsButton!=0){        // if button countdown in progress and button pin state has not changed,
    if(!(countsButton--)) buttonState &= pinBtnCurrState; // if button countdown reaches zero, update button state with current pin state
  }

  pinBtnPrevState = pinBtnCurrState;      // save current pin state for the next time this ISR gets called
}

ISR( PCINT1_vect )                 // VSS event handler interrupt
{
  static uint8_t pinVSSCurrState=0;
  static uint8_t pinVSSPrevState=0;

  pinVSSCurrState=PINC&vssBit;    // read relevant pins (bypassing digitalRead for interrupt performance)
  if (pinVSSCurrState^pinVSSPrevState){   // if VSS pin actually changed state
    if(vssPauseCount==0) toggleVSS(); // if VSS debounce pause defined to be zero, immediately toggle VSS state
    else if(countsVSS==0) countsVSS=vssPauseCount; // otherwise, if no VSS event is scheduled, schedule a delayed VSS state flip
  }

  pinVSSPrevState = pinVSSCurrState;      // save current pin state for the next time this ISR gets called
}

void toggleVSS(void){
  static unsigned long lastVSSTime=0;
  static unsigned long currVSSTime=0;
  static unsigned long VSSTimeLength=0;

  lastVSSTime = currVSSTime;       // save previous VSS pulse tick
  currVSSTime = microSeconds();    // get current VSS pulse tick
  t = elapsedMicroseconds(lastVSSTime,currVSSTime); // calculate current VSS pulse length
  tmpTrip.vssPulses++;             // bump VSS pulse count by 1
  tmpTrip.vssPulseLength += t;     // add measured VSS pulse length to VSS pulse length accumulator
}

volatile unsigned long lastInjStartTime=nil;
volatile unsigned long currInjStartTime=nil;
volatile unsigned long currInjEndTime=nil;

ISR( INT0_vect ){
  lastInjStartTime = curInjStartTime;
  currInjStartTime = microSeconds();
}

ISR( INT1_vect ){
  static unsigned long injPulseLength=0;
  static unsigned long temp=0;

  currInjEndtime=microSeconds();

  if (currInjStartTime != nil) {
    tmpTrip.injHiLength = currInjStartTime - lastInjStartTime;
    injPulseLength = elapsedMicroseconds(currInjStartTime,currInjEndtime) - injectorSettleTime;
    temp = tmpTrip.injHius;
    tmpTrip.injHius += injPulseLength;
    if (tmpTrip.injHius < temp) tmpTrip.injHiSec++;
    tmpTrip.injPulses++;
  }
}

unsigned long maxLoopLength = 0; //see if we are overutilizing the CPU

#define buttonsUp   lbuttonBit + mbuttonBit + rbuttonBit  // start with the buttons in the right state
byte buttonState = buttonsUp;

//overflow counter used by millis2()

unsigned long lastMicroSeconds = millis2() * 1000;

unsigned long microSeconds(void) {
	unsigned long tmp_timer2_overflow_count;
	unsigned long tmp;
	byte tmp_tcnt2;
	cli();
	//disable interrupts
	tmp_timer2_overflow_count = timer2_overflow_count;
	tmp_tcnt2 = TCNT2;
	sei();
	// enable interrupts
	tmp = ((tmp_timer2_overflow_count << 8) + tmp_tcnt2) * 4;
	if ((tmp <= lastMicroSeconds) && (lastMicroSeconds < 4290560000ul))
		return microSeconds();
	lastMicroSeconds = tmp;
	return tmp;
}

unsigned long elapsedMicroseconds(unsigned long startMicroSeconds,
		unsigned long currentMicroseconds) {
	if (currentMicroseconds >= startMicroSeconds)
		return currentMicroseconds - startMicroSeconds;
	return 4294967295 - (startMicroSeconds - currentMicroseconds);
}

unsigned long elapsedMicroseconds(unsigned long startMicroSeconds) {
	return elapsedMicroseconds(startMicroSeconds, microSeconds());
}

//Trip prototype
class Trip {
public:
	unsigned long loopCount; //how long has this trip been running
	unsigned long injPulses; //rpm
	unsigned long injHiLength; // actual rpm, only used by instant
	unsigned long injHiSec; // seconds the injector has been open
	unsigned long injHius; // microseconds, fractional part of the injectors open
	unsigned long injIdleHiSec; // seconds the injector has been open
	unsigned long injIdleHius; // microseconds, fractional part of the injectors open
	unsigned long vssPulses;//from the speedo
	unsigned long vssEOCPulses;//from the speedo
	unsigned long vssPulseLength; // only used by instant
	//these functions actually return in thousandths,
	unsigned long miles();
	unsigned long gallons();
	unsigned long lkm();
	unsigned long mpg();
	unsigned long mph();
	unsigned long time(); //mmm.ss
	unsigned long eocMiles(); //how many "free" miles?
	unsigned long idleGallons(); //how many gallons spent at 0 mph?
	void update(Trip t);
	void reset();
	Trip();
};

//LCD prototype
namespace LCD{
  void init();
  void gotoXY(byte x, byte y);
  void printProgMem(PGM_P s);
  void print(char * string);
  void WriteByte(uint8_t value, uint8_t rs);
  void writeNybble(uint8_t value, uint8_t mask, uint8_t rs);
};

#define LCDCommand B0
#define LCDData B1

//main objects we will be working with:
Trip tmpTrip;
Trip instant;
Trip current;
Trip tank;

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
  doDisplaySystemInfo
};

#define displayFuncSize (sizeof(displayFuncs)/sizeof(pFunc)) //array size

byte newRun = 0;
void setup(void) {

	newRun = load(); // load the default parameters
	sbi(TCCR1A, COM1A1); // brightness pwm enable

	OCR1A = brightness[brightnessIdx];

	DDRB = (1 << DDB5) | (1 << DDB4) | (1 << DDB1) | (1 << DDB0);
	DDRD = (1 << DDD7) | (1 << DDD6) | (1 << DDD5) | (1 << DDD4);

	delay2(500);

	sbi(TCCR0A, COM0A1); // contrast pwm enable
	OCR0A = parms[contrastIdx];

	LCD::init();
	LCD::WriteByte(0b00000001, LCDCommand); // clear display, set cursor position to zero
	LCD::WriteByte(0b10000, LCDCommand); // set dram to zero
	LCD::gotoXY(0, 0);
	LCD::printProgMem(PSTR("OpenGauge       "));
	LCD::gotoXY(0, 1);
	LCD::printProgMem(PSTR("MPGuino  v0.86tav"));

	initGuino(void);

	PORTC |= (1 << 5) | (1 << 4) | (1 << 3); //button pullup resistors
	PCMSK1 |= (1 << PCINT8); // enable vss input pin interrupt
	PCICR |= (1 << PCIE1);

	delay2(1500);
}

byte screen = 0;
byte holdDisplay = 0;

void mainloop(void) {
	if (newRun != 1)
		editGuino(void); // go through the initialization screen
		initGuino(void); // load stored parameters from EEPROM
	unsigned long lastActivity = microSeconds();
	unsigned long tankHold; //state at point of last activity
	while (true) {
		unsigned long loopStart = microSeconds();
		instant.reset(); //clear instant

		cli();
		instant.update(tmpTrip); //"copy" of tmpTrip in instant now
		tmpTrip.reset(); //reset tmpTrip first so we don't lose too many interrupts
		sei();

		//send out instantmpg * 1000, instantmph * 1000, the injector/vss raw data
		simpletx(format(instantmpg()));
		simpletx(",");
		simpletx(format(instantmph()));
		simpletx(",");
		simpletx(format(instant.injHius * 1000));
		simpletx(",");
		simpletx(format(instant.injPulses * 1000));
		simpletx(",");
		simpletx(format(instant.vssPulses * 1000));
		simpletx("\n");

		current.update(instant); //use instant to update current
		tank.update(instant); //use instant to update tank

		//currentTripResetTimeoutUS
		if (instant.vssPulses == 0 && instant.injPulses == 0 && holdDisplay == 0) {
			if (elapsedMicroseconds(lastActivity) > parms[currentTripResetTimeoutUSIdx] && lastActivity != nil) {
				OCR1A = brightness[0];
				lastActivity = nil;
			}
		} else {
			if (lastActivity == nil) {//wake up!!!
				OCR1A = brightness[brightnessIdx];
				//        analogWrite(BrightnessPin,brightness[brightnessIdx]);
				lastActivity = loopStart;
				current.reset();
				tank.loopCount = tankHold;
				current.update(instant);
				tank.update(instant);
			} else {
				lastActivity = loopStart;
				tankHold = tank.loopCount;
			}
		}

		if (holdDisplay == 0) {
			displayFuncs[screen](); //call the appropriate display routine
			LCD::gotoXY(0, 0);

			//see if any buttons were pressed, display a brief message if so
			if (!(buttonState & lbuttonBit) && !(buttonState & rbuttonBit)) {// left and right = initialize
				LCD::printProgMem(PSTR("Setup "));
				editGuino();
				initGuino();
			} else if (!(buttonState & lbuttonBit) && !(buttonState & mbuttonBit)) {// left and middle = tank reset
				tank.reset();
				LCD::printProgMem(PSTR("Tank Reset "));
			} else if (!(buttonState & mbuttonBit) && !(buttonState & rbuttonBit)) {// right and middle = current reset
				current.reset();
				LCD::printProgMem(PSTR("Current Reset "));
			} else if (!(buttonState & lbuttonBit)) { //left is rotate through screeens to the left
				if (screen == 0) screen = displayFuncSize;
				screen--;
				LCD::printProgMem(displayFuncNames[screen]);
			} else if (!(buttonState & mbuttonBit)) { //middle is cycle through brightness settings
				brightnessIdx++;
				if (brightnessIdx == brightnessLength) brightnessIdx = 0;
				OCR1A = brightness[brightnessIdx];
				LCD::printProgMem(PSTR("Brightness "));
				LCD::WriteByte('0' + brightnessIdx, LCDData);
				LCD::print(" ");
			} else if (!(buttonState & rbuttonBit)) {//right is rotate through screeens to the right
				screen++;
				if (screen == displayFuncSize) screen = 0;
				LCD::printProgMem(displayFuncNames[screen]);
			}
			if (buttonState != buttonsUp)
				holdDisplay = 1;
		} else {
			holdDisplay = 0;
		}
		buttonState = buttonsUp;//reset the buttons

		//keep track of how long the loops take before we go int waiting.
		unsigned long loopX = elapsedMicroseconds(loopStart);
		if (loopX > maxLoopLength) maxLoopLength = loopX;

		while (elapsedMicroseconds(loopStart) < (looptime))
			;//wait for the end of a second to arrive
	}
}

char fBuff[7];//used by format

void dispv(byte lidx, unsigned long num) {
	LCD::printProgMem(displayLabels[parms[metricIdx] + lidx]);
	LCD::print(format(num));
}


char* format(unsigned long num) {
	byte dp = 3;

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
	byte x = 6;
	while (x > 0) {
		x--;
		if (x == dp) { //time to poke in the decimal point?{
			fBuff[x] = '.';
		} else {
			fBuff[x] = '0' + (num % 10);//poke the ascii character for the digit.
			num /= 10;
		}
	}
	fBuff[6] = 0;
	return fBuff;
}

void doDisplayCustom() {
	displayTripCombo(displayFuelEconIdx, instantmpg(), displaySpeedIdx, instantmph(), displayFuelConsIdx, instantgph(), displayFuelEconIdx, current.mpg());
}

void doDisplayEOCIdleData() {
	displayTripCombo(displayEOCIdx, current.eocMiles(), displayFuelQuanIdx, current.idleGallons(), displayEOCIdx, tank.eocMiles(), displayFuelQuanIdx, tank.idleGallons());
}
void doDisplayInstantCurrent() {
	displayTripCombo(displayFuelEconIdx, instantmpg(), displaySpeedIdx, instantmph(), displayFuelEconIdx, current.mpg(), displayDistIdx, current.miles());
}

void doDisplayInstantTank() {
	displayTripCombo(displayFuelEconIdx, instantmpg(), displaySpeedIdx, instantmph(), displayFuelEconIdx, tank.mpg(), displayDistIdx, tank.miles());
}

void doDisplayBigInstant() {
	bigNum(instantmpg(), fuelEconLabel[2]);
}

void doDisplayBigCurrent() {
	bigNum(current.mpg(), fuelEconLabel[3]);
}

void doDisplayBigTank() {
	bigNum(tank.mpg(), fuelEconLabel[4]);
}


void doDisplayCurrentTripData(void) {
	tDisplay(&current);
} //display current trip formatted data.

void doDisplayTankTripData(void) {
	tDisplay(&tank);
} //display tank trip formatted data.

void doDisplaySystemInfo(void) {
	LCD::gotoXY(0, 0);
	LCD::print("C%");
	LCD::print(format(maxLoopLength * 1000 / (looptime / 100)));
	LCD::print(" T");
	LCD::print(format(tank.time()));
	unsigned long mem = memoryTest();
	mem *= 1000;
	LCD::gotoXY(0, 1);
	LCD::print("FREE MEM:");
	LCD::print(format(mem));
//	LCD::print(" D");
//	LCD::print(format(readTemp()));
} //display max cpu utilization and ram.

void displayTripCombo(byte lidx1, unsigned long v1, byte lidx2, unsigned long v2, byte lidx3, unsigned long v3, byte lidx4, unsigned long v4){
	LCD::gotoXY(0, 0);
	dispv(lidx1, v1);
	dispv(lidx2, v2);
	LCD::gotoXY(0, 1);
	dispv(lidx3, v3);
	dispv(lidx4, v4);
}

//arduino doesn't do well with types defined in a script as parameters, so have to pass as void * and use -> notation.
void tDisplay(void * r) { //display trip functions.
	Trip *t = (Trip *) r;
	LCD::gotoXY(0, 0);
	dispv(displaySpeedIdx, t->mph());
	dispv(displayFuelEconIdx, t->mpg());
	LCD::gotoXY(0, 1);
	dispv(displayDistIdx ,t->miles());
	dispv(displayFuelQuanIdx, t->gallons());
}

//x=0..16, y= 0..1
void LCD::gotoXY(byte x, byte y) {
	byte dr = x + 0x80;
	if (y == 1)
		dr += 0x40;
	if (y == 2)
		dr += 0x14;
	if (y == 3)
		dr += 0x54;
	LCD::WriteByte(dr, LCDCommand);
}

void LCD::printProgMem(PGM_P s){
  while((c=pgm_read_byte(s++))!=0) LCD::WriteByte(c, LCDData);
}

void LCD::print(char * string){
  while(*string) LCD::WriteByte(*string++, LCDData);
}

void LCD::init(){

  delay2(16);                    // wait for more than 15 msec
  writeNybble(0b00110000, HIGH, LCDCommand); // send (B0011) to DB7-4
  delay2(5);                     // wait for more than 4.1 msec
  writeNybble(0b00110000, HIGH, LCDCommand); // send (B0011) to DB7-4
  delay2(1);                     // wait for more than 100 usec
  writeNybble(0b00110000, HIGH, LCDCommand); // send (B0011) to DB7-4
  delay2(1);                     // wait for more than 100 usec
  writeNybble(0b00100000, HIGH, LCDCommand); // send (B0010) to DB7-4 for 4bit
  delay2(1);                     // wait for more than 100 usec
  // ready to use normal WriteByte() function now!
  WriteByte(0b00101000, LCDCommand); // 4-bit interface, 2 display lines, 5x8 font
  WriteByte(0b00001100, LCDCommand); // display control:
  WriteByte(0b00000110, LCDCommand); // entry mode set: increment automatically, no display shift

  WriteByte(0b01001000, LCDCommand);  // set cgram to define the custom fonts
  for(byte x=0;x<fontBigNumLength;x++)
    WriteByte(pgm_read_byte(&fontBigNum[x]), LCDData); //write the character font data to the character generator ram
  WriteByte(0b00000001, LCDCommand);  // clear display, set cursor position to zero
  WriteByte(0b10000000, LCDCommand);  // set dram to zero

}

void LCD::WriteByte(uint8_t value, uint8_t rs){

  writeNybble(value, HIGH, rs); // write out high nybble of output byte
  writeNybble(value, LOW, rs); // write out low nybble of output byte

}

void LCD::writeNybble(uint8_t value, uint8_t mask, uint8_t rs){ // db7=PB5, db6=PB4, db5 = PB0, db4  = PD7

  uint8_t v;

  if(mask==LOW) v=value&B00001111;
  else v=value&B11110000;

  rs & LCDData ? PORTD |= (1 << 4) : PORTD &= ~(1 << 4); // tell LCD module whether output nybble is a command nybble or a data nybble
  v & B10001000 ? PORTB |= (1 << 5) : PORTB &= ~(1 << 5); // write out bit 7 (bit 3) of nybble
  v & B01000100 ? PORTB |= (1 << 4) : PORTB &= ~(1 << 4); // write out bit 6 (bit 2) of nybble
  v & B00100010 ? PORTB |= (1 << 0) : PORTB &= ~(1 << 0); // write out bit 5 (bit 1 of nybble
  v & B00010001 ? PORTD |= (1 << 7) : PORTD &= ~(1 << 7); // write out bit 4 (bit 0) of nybble
  PORTD |= (1 << 5);               // tell LCD module to pay attention to us
  PORTD &= ~(1 << 5);              // tell LCD module to process the nybble

}

// this function will return the number of bytes currently free in RAM
extern int __bss_end;
extern int *__brkval;
int memoryTest() {
	int free_memory;
	if ((int) __brkval == 0)
		free_memory = ((int) &free_memory) - ((int) &__bss_end);
	else
		free_memory = ((int) &free_memory) - ((int) __brkval);
	return free_memory;
}

Trip::Trip() {
}

//for display computing
unsigned long tmp1[2];
unsigned long tmp2[2];
unsigned long tmp3[2];

unsigned long instantmph() {
	unsigned long vssPulseTimeuS = instant.vssPulseLength / instant.vssPulses;

	init64(tmp1, 0, 1000000000ul);
	init64(tmp2, 0, distancefactor);
	div64(tmp1, tmp2);
	init64(tmp2, 0, cyclesperhour);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, vssPulseTimeuS);
	div64(tmp1, tmp2);
	return tmp1[1];
}

unsigned long instantlkm() {
	unsigned long imph = instantmph();
	unsigned long igph = instantgph();
	if (igph == 0)
		return 0;
	if (imph == 0)
		return 999999000;
	init64(tmp1, 0, 100000ul);
	init64(tmp2, 0, igph);
	mul64(tmp2, tmp1);
	init64(tmp1, 0, imph);
        div64(tmp2,tmp1);
	return tmp2[1];
}

unsigned long instantmpg() {
	if(parms[metricIdx]==1)
		return instantlkm();
	unsigned long imph = instantmph();
	unsigned long igph = instantgph();
	if (imph == 0)
		return 0;
	if (igph == 0)
		return 999999000;
	init64(tmp1, 0, 1000ul);
	init64(tmp2, 0, imph);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, igph);
    div64(tmp1,tmp2);
	return tmp1[1];

}

unsigned long instantgph() {
  if (lastInjStartTime == nil) return 0;
	init64(tmp1, injHiSec, injHius);
	init64(tmp2, 0, 3600000000ul);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, 1000ul);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, fuelfactor);
	div64(tmp1, tmp2);
	init64(tmp2, 0, injHiLength);
	div64(tmp1, tmp2);
	return tmp1[1];
}

unsigned long Trip::miles() {
	init64(tmp1, 0, vssPulses);
	init64(tmp2, 0, 1000);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, distancefactor);
	div64(tmp1, tmp2);
	return tmp1[1];
}

unsigned long Trip::eocMiles() {
	init64(tmp1, 0, vssEOCPulses);
	init64(tmp2, 0, 1000);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, distancefactor);
	div64(tmp1, tmp2);
	return tmp1[1];
}

unsigned long Trip::mph() {
	if (loopCount == 0)
		return 0;
	init64(tmp1, 0, loopsPerSecond);
	init64(tmp2, 0, vssPulses);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, 3600000ul);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, distancefactor);
	div64(tmp1, tmp2);
	init64(tmp2, 0, loopCount);
	div64(tmp1, tmp2);
	return tmp1[1];
}

unsigned long Trip::gallons() {
	init64(tmp1, injHiSec, injHius);
	init64(tmp2, 0, dispadj);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, fuelfactor);
	div64(tmp1, tmp2);
	return tmp1[1];
}

unsigned long Trip::idleGallons() {
	init64(tmp1, 0, injIdleHiSec);
	init64(tmp2, 0, 1000000);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, injIdleHius);
	add64(tmp1, tmp2);
	init64(tmp2, 0, dispadj);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, fuelfactor);
	div64(tmp1, tmp2);
	return tmp1[1];
}

//eocMiles
//idleGallons

unsigned long Trip::lkm() {
	if (injPulses == 0)
		return 0;
	if (vssPulses == 0)
		return 999999000; //who doesn't like to see 999999?  :)


	init64(tmp3, injHiSec, injHius);
	init64(tmp1, 0, distancefactor);
	mul64(tmp3, tmp1);
	init64(tmp1, 0, 80000ul);
//	init64(tmp1, 0, 100000000ul);
	mul64(tmp3, tmp1);

	init64(tmp1, 0, fuelfactor);
//	init64(tmp2, 0, dispadj2);
//	mul64(tmp1, tmp2);
	init64(tmp2, 0, vssPulses);
	mul64(tmp1, tmp2);

	div64(tmp3, tmp1);
	return tmp3[1];
}

unsigned long Trip::mpg() {
	if(parms[metricIdx]==1)
		return lkm();
	if (vssPulses == 0)
		return 0;
	if (injPulses == 0)
		return 999999000; //who doesn't like to see 999999?  :)

	init64(tmp3, injHiSec, injHius);
	init64(tmp1, 0, distancefactor);
	mul64(tmp3, tmp1);

	init64(tmp1, 0, fuelfactor);
	init64(tmp2, 0, dispadj2);
	mul64(tmp1, tmp2);
	init64(tmp2, 0, vssPulses);
	mul64(tmp1, tmp2);

	div64(tmp1, tmp3);
	return tmp1[1];
}



//return the seconds as a time mmm.ss, eventually hhh:mm too
unsigned long Trip::time() {
	//  return seconds*1000;
	byte d = 60;
	unsigned long seconds = loopCount / loopsPerSecond;
	//  if(seconds/60 > 999) d = 3600; //scale up to hours.minutes if we get past 999 minutes
	return ((seconds / d) * 1000) + ((seconds % d) * 10);
}

void Trip::reset() {
	loopCount = 0;
	injPulses = 0;
	injHiLength = 0;
	injHius = 0;
	injHiSec = 0;
	vssPulses = 0;
	vssPulseLength = 0;
	injIdleHiSec = 0;
	injIdleHius = 0;
	vssEOCPulses = 0;
}

void Trip::update(Trip t) {
	static unsigned long temp=0;

	loopCount++; //we call update once per loop
	vssPulses += t.vssPulses;
	vssPulseLength += t.vssPulseLength;
	injHiLength = t.injHiLength;
	if (t.injPulses == 0) vssEOCPulses += t.vssPulses; // track distance traveled with engine off
	injPulses += t.injPulses;
	temp = injHius;
	injHius += t.injHius;
	if (injHius < temp) injHiSec++; // rollover into the injHiSec counter
	if (t.vssPulses == 0) { // track gallons spent sitting still
		temp = injIdleHius;
		injIdleHius += t.injHius;
		if (injIdleHius < temp) injIdleHiSec++; // rollover
	}
}

void bigNum(unsigned long t, PGM_P txt1) {
	//  unsigned long t = 98550ul;//number in thousandths
	//  unsigned long t = 9855ul;//number in thousandths
	//  char * txt1="INST";
	//  char * txt2="MPG ";
	char dp1 = 32;
	char dp2 = 32;

    char txt2 = fuelEconLabel[parms[metricIdx]];

//	return format2(num,4,9999);


	char * r = "009.99"; //default to 999
	if (t <= 9950) {
		r = format(t); //009.86
		dp1 = 5;
	} else if (t <= 99500) {
		r = format(t / 10); //009.86
		dp2 = 5;
	} else if (t <= 999500) {
		r = format(t / 100); //009.86
	}

	LCD::gotoXY(0, 0);
	LCD::printProgMem(bigNumChars1[r[2] - '0']);
	LCD::printProgMem(bigNumChars1[r[4] - '0']);
	LCD::printProgMem(bigNumChars1[r[5] - '0']);
	LCD::printProgMem(txt1);

	LCD::gotoXY(0, 1);
	LCD::printProgMem(bigNumChars2[r[2] - '0']);
	LCD::WriteByte(dp1, LCDData);
	LCD::printProgMem(bigNumChars2[r[4] - '0']);
	LCD::WriteByte(dp2, LCDData);
	LCD::printProgMem(bigNumChars2[r[5] - '0']);
	LCD::printProgMem(fuelEconLabel[parms[metricIdx]]);
}

//the standard 64 bit math brings in  5000+ bytes
//these bring in 1214 bytes, and everything is pass by reference
unsigned long zero64[] = { 0, 0 };

void init64(unsigned long an[], unsigned long bigPart, unsigned long littlePart) {
	an[0] = bigPart;
	an[1] = littlePart;
}

//left shift 64 bit "number"
void shl64(unsigned long an[]) {
	an[0] <<= 1;
	if (an[1] & 0x80000000)
		an[0]++;
	an[1] <<= 1;
}

//right shift 64 bit "number"
void shr64(unsigned long an[]) {
	an[1] >>= 1;
	if (an[0] & 0x1)
		an[1] += 0x80000000;
	an[0] >>= 1;
}

//add ann to an
void add64(unsigned long an[], unsigned long ann[]) {
	an[0] += ann[0];
	if (an[1] + ann[1] < ann[1])
		an[0]++;
	an[1] += ann[1];
}

//subtract ann from an
void sub64(unsigned long an[], unsigned long ann[]) {
	an[0] -= ann[0];
	if (an[1] < ann[1]) {
		an[0]--;
	}
	an[1] -= ann[1];
}

//true if an == ann
boolean eq64(unsigned long an[], unsigned long ann[]) {
	return (an[0] == ann[0]) && (an[1] == ann[1]);
}

//true if an < ann
boolean lt64(unsigned long an[], unsigned long ann[]) {
	if (an[0] > ann[0])
		return false;
	return (an[0] < ann[0]) || (an[1] < ann[1]);
}

//divide num by den
void div64(unsigned long num[], unsigned long den[]) {
	unsigned long quot[2];
	unsigned long qbit[2];
	unsigned long tmp[2];
	init64(quot, 0, 0);
	init64(qbit, 0, 1);

	if (eq64(num, zero64)) { //numerator 0, call it 0
		init64(num, 0, 0);
		return;
	}

	if (eq64(den, zero64)) { //numerator not zero, denominator 0, infinity in my book.
		init64(num, 0xffffffff, 0xffffffff);
		return;
	}

	init64(tmp, 0x80000000, 0);
	while (lt64(den, tmp)) {
		shl64(den);
		shl64(qbit);
	}

	while (!eq64(qbit, zero64)) {
		if (lt64(den, num) || eq64(den, num)) {
			sub64(num, den);
			add64(quot, qbit);
		}
		shr64(den);
		shr64(qbit);
	}

	//remainder now in num, but using it to return quotient for now
	init64(num, quot[0], quot[1]);
}

//multiply num by den
void mul64(unsigned long an[], unsigned long ann[]) {
	unsigned long p[2] = { 0, 0 };
	unsigned long y[2] = { ann[0], ann[1] };
	while (!eq64(y, zero64)) {
		if (y[1] & 1)
			add64(p, an);
		shl64(an);
		shr64(y);
	}
	init64(an, p[0], p[1]);
}

void save() {
	eeprom_write_byte((unsigned char *) 0, guinosig);
	eeprom_write_byte((unsigned char *) 1, parmsLength);
	byte p = 0;
	for (int x = 4; p < parmsLength; x += 4) {
		unsigned long v = parms[p];
		eeprom_write_byte((unsigned char *) x, (v >> 24) & 255);
		eeprom_write_byte((unsigned char *) x + 1, (v >> 16) & 255);
		eeprom_write_byte((unsigned char *) x + 2, (v >> 8) & 255);
		eeprom_write_byte((unsigned char *) x + 3, (v) & 255);
		p++;
	}
}

byte load() { //return 1 if loaded ok
#ifdef usedefaults
	return 1;
#endif
	byte b = eeprom_read_byte((unsigned char *) 0);
	byte c = eeprom_read_byte((unsigned char *) 1);
	if (b == guinosigold)
		c = 9; //before fancy parameter counter

	if (b == guinosig || b == guinosigold) {
		byte p = 0;

		for (int x = 4; p < c; x += 4) {
			unsigned long v = eeprom_read_byte((unsigned char *) x);
			v = (v << 8) + eeprom_read_byte((unsigned char *) x + 1);
			v = (v << 8) + eeprom_read_byte((unsigned char *) x + 2);
			v = (v << 8) + eeprom_read_byte((unsigned char *) x + 3);
			parms[p] = v;
			p++;
		}
		return 1;
	}
	return 0;
}

char * uformat(unsigned long val) {
	unsigned long d = 1000000000ul;
	for (byte p = 0; p < 10; p++) {
		mBuff[p] = '0' + (val / d);
		val = val - (val / d * d);
		d /= 10;
	}
	mBuff[10] = 0;
	return mBuff;
}

unsigned long rformat(char * val) {
	unsigned long d = 1000000000ul;
	unsigned long v = 0ul;
	for (byte p = 0; p < 10; p++) {
		v = v + (d * (val[p] - '0'));
		d /= 10;
	}
	return v;
}

void editParm(byte parmIdx) {
	unsigned long v = parms[parmIdx];
	byte p = 9; //right end of 10 digit number
	//display label on top line
	//set cursor visible
	//set pos = 0
	//display v

	LCD::gotoXY(8, 0);
	LCD::print("        ");
	LCD::gotoXY(0, 0);
	LCD::printProgMem(parmLabels[parmIdx]);
	LCD::gotoXY(0, 1);
	char * fmtv = uformat(v);
	LCD::print(fmtv);
	LCD::print(" OK XX");
	LCD::WriteByte(0b00001110, LCDCommand);

	for (int x = 9; x >= 0; x--) { //do a nice thing and put the cursor at the first non zero number
		if (fmtv[x] != '0')
			p = x;
	}
	byte keyLock = 1;
	while (true) {

		if (p < 10)
			LCD::gotoXY(p, 1);
		if (p == 10)
			LCD::gotoXY(11, 1);
		if (p == 11)
			LCD::gotoXY(14, 1);

		if (keyLock == 0) {
			if (!(buttonState & lbuttonBit) && !(buttonState & rbuttonBit)) {// left & right
				LCD::WriteByte(0b00001100, LCDCommand);
				return;
			} else if (!(buttonState & lbuttonBit)) {// left
				p = p - 1;
				if (p == 255)
					p = 11;
			} else if (!(buttonState & rbuttonBit)) {// right
				p = p + 1;
				if (p == 12)
					p = 0;
			} else if (!(buttonState & mbuttonBit)) {// middle
				if (p == 11) { //cancel selected
					LCD::WriteByte(0b00001100, LCDCommand);
					return;
				}
				if (p == 10) { //ok selected
					LCD::WriteByte(0b00001100, LCDCommand);
					parms[parmIdx] = rformat(fmtv);
					return;
				}

				byte n = fmtv[p] - '0';
				n++;
				if (n > 9)
					n = 0;
				if (p == 0 && n > 3)
					n = 0;
				fmtv[p] = '0' + n;
				LCD::gotoXY(0, 1);
				LCD::print(fmtv);
				LCD::gotoXY(p, 1);
				if (parmIdx == contrastIdx) OCR0A = rformat(fmtv); //adjust contrast dynamically
			}

			if (buttonState != buttonsUp)
				keyLock = 1;
		} else {
			keyLock = 0;
		}
		buttonState = buttonsUp;
		delay2(125);
	}

}

void editGuino(void) { // edit all the parameters
	for (int x = 0; x < parmsLength; x++)
		editParm(x);
	save();
}

void initGuino(void) { // initialize all the parameters

	injectorSettleTime = injhold;
	distancefactor = parms[vssPulsesPerMileIdx];
	fuelfactor = parms[microSecondsPerGallonIdx];
	vssPauseCount = parms[vsspause];

	EIMSK &= ~((1 << INT0) | (1 << INT1));

	EICRA &= ~((1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11));
	EICRA |= (parms[injEdgeIdx] == 1 ? ((RISING << ISC00) | (FALLING << ISC10)) : ((FALLING << ISC00) | (RISING << ISC10)));

	EIMSK |= (1 << INT1) | (1 << INT0);


	if (parms[metricIdx] == 1){
		distancefactor /= 1.609;
		fuelfactor /= 3.785;
	}

	holdDisplay = 1;
}

unsigned long millis2() {
	return timer2_overflow_count * 64UL * 2 / (16000000UL / 128000UL);
}

void delay2(unsigned long ms) {
	unsigned long start = millis2();
	while (millis2() - start < ms)
		;
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
	sei();
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
	sbi(TIMSK0, TOIE0);

	// set timer 1 prescale factor to 64
	sbi(TCCR1B, CS11);
	sbi(TCCR1B, CS10);
	// put timer 1 in 8-bit phase correct pwm mode
	sbi(TCCR1A, WGM10);
	// set timer 2 prescale factor to 64
	sbi(TCCR2B, CS22);
	// configure timer 2 for phase correct pwm (8-bit)
	sbi(TCCR2A, WGM20);

	// set a2d prescale factor to 128
	sbi(ADCSRA, ADPS2);
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);

	// enable a2d conversions
	sbi(ADCSRA, ADEN);

	UCSR0B = 0;

	sei();

	timer2_overflow_count = 0;

	TCCR2A = 1 << WGM20 | 1 << WGM21;
	// set timer 2 prescale factor to 64
	TCCR2B = 1 << CS22;
	TIMSK2 |= 1 << TOIE2;
	TIMSK0 &= !(1 << TOIE0);

	setup();
	mainloop();

	return 0;
}
