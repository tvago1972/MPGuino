#if defined(useOutputPins)
/* Analog output pin driver section */

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
	instrLxdIEEPROMoffset, pOutputPin1Mode,				// load the indexed stored parameter index for the expansion output pin setting
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

static void outputPin::init(void)
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
	DDRB |= ((1 << DDB6) | (1 << DDB4));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// set OC1B to non-inverting mode for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// enable EXP1 and EXP2 option pin outputs
	DDRB |= ((1 << DDB3) | (1 << DDB2));

#endif // defined(__AVR_ATmega328P__)
	setOutputPin1(2); // initially set EXP1 option pin to 0
	setOutputPin2(2); // initially set EXP2 option pin to 0

}

static void outputPin::shutdown(void)
{

	setOutputPin1(2); // set EXP1 option pin to 0
	setOutputPin2(2); // set EXP2 option pin to 0

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
	DDRB &= ~((1 << DDB6) | (1 << DDB4));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable expansion pin output
	DDRB &= ~((1 << DDB3) | (1 << DDB2));

	// set OC1B to disabled for EXP1 option pin
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

	// set OC2A to disabled for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

#endif // defined(__AVR_ATmega328P__)
}

// sets EXP1 option pin PWM output from 0-100%
static void outputPin::setOutputPin1(uint8_t pin)
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
static void outputPin::setOutputPin2(uint8_t pin)
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

#endif // defined(useOutputPins)
