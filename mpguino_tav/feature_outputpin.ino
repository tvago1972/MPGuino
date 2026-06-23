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
	7	instant fuel economy analog, 0 to pOutputPinMaxFuelEconomy (0 (zero FE) to 255 (at or above max FE))
	8	instant fuel economy analog, 0 to 1.5 * current trip average FE (0 (zero) to 255 (at 1.5x average))
	9	instant fuel economy analog, 0 to 1.5 * tank trip average FE (0 (zero) to 255 (at 1.5x average))
*/
static const uint8_t prgmCalculateOutputPinValue[] PROGMEM = {
	instrCmpIndex, 2,									// is a valid expansion output pin number being requested?
	instrBranchIfGTorE, 17,								// skip to output a zero if not
	instrLxdIEEPROMoffset, pOutputPin1Mode,				// load the indexed stored parameter index for the expansion output pin setting
	instrTestIndex,										// test pin mode value for zero
	instrBranchIfZero, 12,								// exit out if pin mode is zero
	instrCmpIndex, 4,									// test if pin mode is "fuel economy comparison between instant and whatever"
	instrBranchIfLTorE, 30,								// if so, skip ahead
	instrCmpIndex, 6,									// test if pin mode is analog output tank quantity or quantity remaining
	instrBranchIfLTorE, 8,								// if so, skip ahead
	instrCmpIndex, 9,									// test if pin mode is analog FE output
	instrBranchIfLTorE, 78,								// if so, skip ahead

//zeroOutRet:
	instrLdRegByte, 0x02, 0,							// zero out result
	instrDone,											// exit to caller

//analogOut:
	instrCall, tCalculateRemainingTank,					// fetch remaining fuel value in injector open cycles
	instrCmpIndex, 6,									// outputting remaining value?
	instrBranchIfE, 7,									// go do conversion if so
	instrLdReg, 0x21,									// shift contents to register 1
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubYfromX, 0x12,								// subtract remaining fuel value from tank size

//analogOut2:
	instrMul2byByte, 255,								// multiply tank quantity by 255
	instrDiv2byVariable, m64TankSizeIdx,				// divide by calculated tank size
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
	instrDone,											// exit to caller

// for analog FE output (modes 7-9) the gauge always uses distance/fuel ratio regardless of metric mode setting,
// so that higher output always means better fuel economy in all unit configurations

//feAnalog:
// mode 7: instant FE analog output scaled to pOutputPinMaxFuelEconomy
// PWM = (inst_VSS * m32CyclesPerVolume * idxDecimalPoint * 255) / (inst_InjCycles * pPulseEdgePerDistance * pOutputPinMaxFuelEconomy)
// pOutputPinMaxFuelEconomy is stored as FE * idxDecimalPoint (e.g. 50000 = 50 MPG or 50 KPL after metric conversion)
	instrCmpIndex, 7,									// test if mode 7 (absolute max FE)
	instrBranchIfGT, 41,								// if mode > 7, skip to modes 8/9 handler
	instrLdRegTripVar, 0x02, instantIdx, rvInjCycleIdx,	// fetch instant injector open cycles (fuel denominator)
	instrTestReg, 0x02,									// test for zero (coasting / engine off)
	instrBranchIfNotZero, 4,							// if non-zero, skip to computation
	instrLdRegByte, 0x02, 0,							// zero output when coasting
	instrDone,											// exit to caller
	instrMul2byEEPROM, pPulseEdgePerDistanceIdx,		// multiply by pulse edges per distance
	instrMul2byEEPROM, pOutputPinMaxFuelEconomy,		// multiply by stored max FE (FE * idxDecimalPoint)
	instrLdReg, 0x21,									// save denominator in register 1
	instrLdRegTripVar, 0x02, instantIdx, rvVSSpulseIdx,	// fetch instant VSS pulses (distance numerator)
	instrMul2byVariable, m32CyclesPerVolumeIdx,			// multiply by cycles per unit volume
	instrMul2byRdOnly, idxDecimalPoint,					// scale by idxDecimalPoint (cancels stored max FE scaling)
	instrMul2byByte, 255,								// scale to 0-255 output range
	instrDiv2by1,										// compute PWM = numerator / denominator
	instrAdjustQuotient,								// round result
	instrLdRegByte, 0x01, 255,							// load 255 for clamp comparison
	instrCmpXtoY, 0x21,									// compare result to 255
	instrBranchIfLTorE, 3,								// if result <= 255, skip clamp
	instrLdRegByte, 0x02, 255,							// clamp output to 255
	instrDone,											// exit to caller

// modes 8/9: instant FE analog output scaled to 1.5 * average FE (current trip or tank)
// PWM = (inst_VSS * accum_InjCycles * 170) / (inst_InjCycles * accum_VSS)
// where 170 = 255 / 1.5, so full scale (255) corresponds to 1.5 * average FE
// this computation is unit-agnostic: distance and fuel conversion factors cancel in the ratio
	instrCmpIndex, 9,									// test if mode 9 (tank average)
	instrBranchIfLT, 10,								// if mode < 9 (i.e. mode 8), skip to current trip load
	instrLdRegTripVar, 0x03, tankIdx, rvInjCycleIdx,	// fetch tank accumulated injector open cycles
	instrLdRegTripVar, 0x02, tankIdx, rvVSSpulseIdx,	// fetch tank accumulated VSS pulses
	instrSkip, 8,										// skip current trip load
	instrLdRegTripVar, 0x03, currentIdx, rvInjCycleIdx,	// fetch current accumulated injector open cycles
	instrLdRegTripVar, 0x02, currentIdx, rvVSSpulseIdx,	// fetch current accumulated VSS pulses
	instrTestReg, 0x02,									// test accum_VSS for zero (no trip data yet)
	instrBranchIfNotZero, 4,							// if non-zero, continue
	instrLdRegByte, 0x02, 0,							// zero output when no trip data
	instrDone,											// exit to caller
	instrLdRegTripVar, 0x01, instantIdx, rvInjCycleIdx,	// fetch instant injector open cycles
	instrTestReg, 0x01,									// test for zero (coasting / engine off)
	instrBranchIfNotZero, 4,							// if non-zero, continue
	instrLdRegByte, 0x02, 0,							// zero output when coasting
	instrDone,											// exit to caller
	instrMul2by1,										// reg2 = accum_VSS * inst_InjCycles (denominator)
	instrLdReg, 0x21,									// save denominator in register 1
	instrSwapReg, 0x13,									// swap: reg1 = accum_InjCycles, reg3 = denominator
	instrLdRegTripVar, 0x02, instantIdx, rvVSSpulseIdx,	// fetch instant VSS pulses
	instrMul2by1,										// reg2 = inst_VSS * accum_InjCycles (numerator part)
	instrMul2byByte, 170,								// scale to 0-255 range at 1.5x average (255/1.5 = 170)
	instrSwapReg, 0x13,									// swap back: reg1 = denominator, reg3 = accum_InjCycles
	instrDiv2by1,										// compute PWM = numerator / denominator
	instrAdjustQuotient,								// round result
	instrLdRegByte, 0x01, 255,							// load 255 for clamp comparison
	instrCmpXtoY, 0x21,									// compare result to 255
	instrBranchIfLTorE, 3,								// if result <= 255, skip clamp
	instrLdRegByte, 0x02, 255,							// clamp output to 255
	instrDone											// exit to caller
};

static void outputPin::init(void)
{

// PWM frequencies for RC DAC filter design (PWM -> RC low-pass -> 0-5V analog output):
//
// ATmega32U4  Timer4  EXP1 (OC4A) + EXP2 (OC4D): high-speed timer with PLL; WGM/CS set by Arduino core (board-specific)
// ATmega2560  Timer5  EXP1 (OC5A) + EXP2 (OC5B): Arduino core sets phase-correct 8-bit PWM, prescaler 64
//                     f_PWM = 16 MHz / (2 * 64 * 255) = 490 Hz
// ATmega328P  Timer1  EXP1 (OC1B): heart.ino sets phase-correct 8-bit PWM, prescaler 1
//                     f_PWM = 16 MHz / (2 * 1 * 255) = 31,373 Hz
// ATmega328P  Timer2  EXP2 (OC2A): overridden below to phase-correct 8-bit PWM, prescaler 1
//                     f_PWM = 16 MHz / (2 * 1 * 255) = 31,373 Hz  (matches EXP1)

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
	// set OC5A to clear-up/set-down for EXP1 option pin
	TCCR5A &= ~(1 << COM5A0);
	TCCR5A |= (1 << COM5A1);

	// set OC5B to clear-up/set-down for EXP2 option pin
	TCCR5A &= ~(1 << COM5B0);
	TCCR5A |= (1 << COM5B1);

	// enable EXP1 and EXP2 option pin outputs
	DDRL |= ((1 << DDL4) | (1 << DDL3));

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// set OC1B to non-inverting mode for EXP1 option pin
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// set OC2A to clear-up/set-down for EXP2 option pin
	TCCR2A &= ~(1 << COM2A0);
	TCCR2A |= (1 << COM2A1);

	// set Timer2 to phase-correct 8-bit PWM, prescaler 1 (matches Timer1/EXP1 at 31,373 Hz)
	TCCR2A &= ~_BV(WGM21);
	TCCR2A |= _BV(WGM20);
	TCCR2B &= ~(_BV(CS22) | _BV(CS21));
	TCCR2B |= _BV(CS20);

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
	DDRL &= ~((1 << DDL4) | (1 << DDL3));

	// set OC5A to disabled for EXP1 option pin
	TCCR5A &= ~(_BV(COM5A1) | _BV(COM5A0));

	// set OC5B to disabled for EXP2 option pin
	TCCR5A &= ~(_BV(COM5B1) | _BV(COM5B0));

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

	uint8_t val = SWEET64::runPrgm(S64_PRGM_PTR(prgmCalculateOutputPinValue), pin);

#if defined(__AVR_ATmega32U4__)
	OCR4A = val;
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	OCR5A = val;
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR1B = val;
#endif // defined(__AVR_ATmega328P__)

}

// sets EXP2 option pin PWM output from 0-100%
static void outputPin::setOutputPin2(uint8_t pin)
{

	uint8_t val = SWEET64::runPrgm(S64_PRGM_PTR(prgmCalculateOutputPinValue), pin);

#if defined(__AVR_ATmega32U4__)
	OCR4D = val;
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	OCR5B = val;
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR2A = val;
#endif // defined(__AVR_ATmega328P__)

}

#endif // defined(useOutputPins)
