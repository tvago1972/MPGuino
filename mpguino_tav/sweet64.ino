/*

 SWEET64 is a low-level pseudocode interpreter, meant to save a lot of space in program memory for 64-bit calculations

 the opcode definitions below provide means of manipulating MPGuino trip variables, interacting with calculated and EEPROM parameters, and providing basic
 number formatting for interaction with string representations of numbers

 SWEET64 is inspired by an early pseudocode processor named SWEET16, which was written by Steve Wozniak of Apple Computer back in 1977. SWEET16 was written to
    perform 16-bit machine language instructions while running on an 8-bit microprocessor

*/

#ifdef useSWEET64mult
static const uint8_t prgmMult64[] PROGMEM = {
	instrLdReg, 0x24,									// load multiplier into register 4
	instrLdRegByte, 0x02, 0,							// zero out result (register 2)
	instrCmpXtoY, 0x54,									// is multiplicand greater than multiplier?
	instrBranchIfGT, 12,								// if so, go to multipler test
	instrSwapReg, 0x54,									// otherwise, swap the two so that multiplier is smaller (eg it's faster to add 3 10s than it is to add 10 3s)
	instrSkip, 8,										// skip forward to multiplier test

//multloop:
	instrShiftRegRight, 0x04,							// shift multiplier right by one bit
	instrBranchIfCclear, 2,								// if multiplier bit was clear, don't add multiplicand to result
	instrAddYtoX, 0x52,									// add multiplicand to result

//shiftmultiplicand:
	instrShiftRegLeft, 0x05,							// shift multiplicand left by one bit

//testmultiplier:
	instrTestReg, 0x04,									// test multiplier register
	instrBranchIfNotZero, 244,							// if multiplier is not zero, loop back
	instrTraceDone										// exit to caller
};

#endif // useSWEET64mult
#ifdef useSWEET64div
static const uint8_t prgmDiv64[] PROGMEM = {
	instrLdReg, 0x21,									// initialize remainder with dividend
	instrLdRegByte, 0x04, 1,							// load quotient bitmask with a 1
	instrTestReg, 0x05,									// test divisor register
	instrBranchIfNotZero, 11,							// if divisor is not zero, skip ahead

//divoverflow:
	instrLdRegByte, 0x02, 0,							// zero out quotient
	instrSubYfromX, 0x42,								// set overflow value in quotient
	instrLdReg, 0x21,									// set overflow value in remainder

//divzero:
	instrLdRegByte, 0x05, 0,							// zero out divisor
	instrTraceDone,										// exit to caller

//testdividend:
	instrTestReg, 0x02,									// test dividend register
	instrBranchIfZero, 248,								// if dividend is zero, go exit
	instrLdRegByte, 0x02, 0,							// initialize quotient (register 2)
	instrTestReg, 0x05,									// test dividend register
	instrBranchIfMinus, 6,								// if dividend is already fully shifted left, skip

//divloop0:
	instrShiftRegLeft, 0x04,							// shift quotient bitmask left one bit
	instrShiftRegLeft, 0x05,							// shift divisor left one bit
	instrBranchIfPlus, 250,								// if dividend is not fully shifted left, loop back

//divcomp:
	instrCmpXtoY, 0x51,									// compare divisor to dividend
	instrBranchIfGT, 4,									// if divisor is greater than dividend, skip to bitmask adjustment
	instrSubYfromX, 0x51,								// subtract divisor from dividend
	instrAddYtoX, 0x42,	       							// add quotient bitmask to quotient

//divnext:
	instrShiftRegRight, 0x04,							// shift quotient bitmask right by one bit
	instrBranchIfCset, 4,								// if quotient bitmask empty, skip ahead
	instrShiftRegRight, 0x05,							// shift divisor right by one bit
	instrBranchIfNotZero, 240,							// go back to quotient bitmask test

//divexit:
	instrShiftRegLeft, 0x01,							// shift remainder left one bit to multiply by 2
	instrCmpXtoY, 0x51,									// compare divisor to 2*remainder
	instrBranchIfLTorE, 5,								// if divisor is less than or equal to 2*remainder, skip ahead
	instrLdRegByte, 0x05, 0,							// zero out divisor
	instrSkip, 3,										// skip ahead to restore remainder

//divadjust1:
	instrLdRegByte, 0x05, 1,							// load divisor with a 1

//divexit2:
	instrShiftRegRight, 0x01,							// shift remainder right one bit to restore it
	instrTraceDone										// exit to caller
};

#endif // useSWEET64div
#ifdef useSWEET64trace
static void SWEET64::listProgram(uint8_t calcIdx)
{
}

#endif // useSWEET64trace
static uint32_t SWEET64::doCalculate(uint8_t tripIdx, uint8_t calcIdx)
{

	return runPrgm((const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(calcIdx)])), tripIdx);

}

static uint32_t SWEET64::runPrgm(const uint8_t * sched, uint8_t tripIdx)
{

	uint8_t oldSREG;
	uint8_t spnt = 0;
	const uint8_t * prgmStack[16];
	const uint8_t * s64BCDptr;
	uint8_t opcodePrefix;
	uint8_t opcodeSuffix;
	uint8_t instr;
	uint8_t operand;
	uint8_t extra;
	uint8_t loopFlag;
	uint8_t branchFlag;
	uint8_t jumpVal;

	union union_64 * regX;
	union union_64 * regY;
	union union_64 * regP;
	union union_64 * regS;
#ifdef useSWEET64trace
	uint8_t traceSave;
	uint8_t i;
#endif // useSWEET64trace
#ifdef useDebugCPUreading
	uint32_t s64Start;
	uint32_t mathStart;
#endif // useDebugCPUreading

#ifdef useDebugCPUreading
	s64Start = systemInfo::cycles0();

#endif // useDebugCPUreading
	SWEET64processorFlags = 0;
	loopFlag = 1;

	do
	{

#ifdef useSWEET64trace
		i = (SWEET64processorFlags & SWEET64traceFlagGroup);
		if (i == SWEET64traceFlag) SWEET64processorFlags &= ~(SWEET64traceFlag);
		if (i == SWEET64traceCommandFlag) SWEET64processorFlags |= (SWEET64traceFlag);

		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugTerminal, 13);
			text::hexWordOut(devDebugTerminal, (uint16_t)(sched));
			text::charOut(devDebugTerminal, 32);
			text::hexByteOut(devDebugTerminal, tripIdx);
			text::charOut(devDebugTerminal, 32);
			text::hexByteOut(devDebugTerminal, spnt);
			text::charOut(devDebugTerminal, 32);

		}

#endif // useSWEET64trace
		instr = fetchByte(sched);

		if (instr < maxValidSWEET64instr)
		{

			opcodePrefix = pgm_read_byte(&opcodeFetchPrefix[(uint16_t)(instr)]);
			opcodeSuffix = pgm_read_byte(&opcodeFetchSuffix[(uint16_t)(instr)]);

		}
		else loopFlag = 0;

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::stringOut(devDebugTerminal, opcodeList, instr);
			text::charOut(devDebugTerminal, 13);
			text::charOut(devDebugTerminal, 9);

		}

#endif // useSWEET64trace
		branchFlag = 1;
		operand = 0;
		extra = 0;

		switch (opcodePrefix & rxxMask) // categorize opcode according to how it accesses the 64 bit registers
		{

			case r00:	// do not fetch register operand
				opcodePrefix &= ~(rxxMask); // slice off rxx portion of opcode prefix
				branchFlag = 0;
				break;

			case r01:	// fetch rX and rY from program
				operand = fetchByte(sched);
				break;

			case r02:	// set rX = r5
				operand = 0x05;	// signal to move external contents to arithmetic register 5
				break;

			case r03:	// set rX = r5, rY = r1
				operand = 0x15;	// signal to move register 1 contents to arithmetic register 5
				break;

			case r04:	// fetch rP and rS from program
				extra = fetchByte(sched);
				operand = extra;
				break;

			case r05:	// set rP = r5, rS = r2
				extra = 0x52;	// signal to add half remainder flag to quotient
				break;

			case r06:	// fetch rP from program, set rS = r5, and rX to r5
				extra = fetchByte(sched) & 0x07; // ignore the rS nybble
				extra |= 0x50; // set rS to r5
				operand = 0x05;
				break;

			default:	// invalid rxx code detected, exit program
				loopFlag = 0;
				break;

		}

		if (operand & 0x07) regX = (union union_64 *)(&s64reg[(unsigned int)((operand & 0x07) - 1)]);
		if (operand & 0x70) regY = (union union_64 *)(&s64reg[(unsigned int)(((operand & 0x70) >> 4) - 1)]);
		if (extra & 0x07) regP = (union union_64 *)(&s64reg[(unsigned int)((extra & 0x07) - 1)]);
		if (extra & 0x70) regS = (union union_64 *)(&s64reg[(unsigned int)(((extra & 0x70) >> 4) - 1)]);

		operand = 0;

		switch (opcodePrefix & pxxMask) // pull general operand to work with
		{

			case p00:	// do not fetch primary operand
				break;

			case p03:	// load primary operand with program + index
				operand += tripIdx;
			case p01:	// load primary operand from program
				operand += fetchByte(sched);
				break;

			case p02:	// load primary operand from index
				operand += tripIdx;
				break;

			case p04:	// load primary operand with EEPROM indirect[index]
				operand += pgm_read_byte(&convIdx[(unsigned int)(tripIdx)]);
				break;

			default:	// invalid pxx code detected, exit program
				loopFlag = 0;
				break;

		}

		extra = 0;

		switch (opcodePrefix & sxxMask) // pull extra operand to work with
		{

			case s00:	// do not fetch secondary operand
				break;

			case s01:	// fetch secondary operand from program
				extra += fetchByte(sched);
				break;

			case s02:	// fetch secondary operand from index
				extra += tripIdx;
				break;

			case s03:	// fetch secondary operand from jump register
				extra += jumpVal;
				break;

			default:	// invalid sxx code detected, exit program
				loopFlag = 0;
				break;

		}

		if (branchFlag) // opcode does something with the 64 bit registers
		{

			switch (opcodeSuffix & ixxMask) // disable interrupts, according to ixx
			{

				case i07:	// load rX with volatile
				case i08:	// store volatile rX
				case i18:	// load rX with trip variable
				case i19:	// store trip variable rX
					oldSREG = SREG; // save interrupt flag status
					cli(); // disable interrupts
				default:
					break;

			}

			switch (opcodeSuffix & ixxMask) // perform load or store operation, according to ixx
			{

				case i00:	// do nothing
					break;

				case i01:	// load rX with rY
					copy64(regX, regY);
					break;

				case i02:	// swap rX rY
					swap64(regX, regY);
					break;

				case i03:	// load rX with eeprom
					EEPROM::read64(regX, operand);
					break;

				case i04:	// store eeprom rX
					EEPROM::write64(regX, operand);
					break;

				case i05:	// load rX with main program
					init64(regX, mainProgramVariables[(uint16_t)(operand)]);
					break;

				case i06:	// store main program rX
					mainProgramVariables[(uint16_t)(operand)] = regX->ul[0];
					break;

				case i07:	// load rX with volatile
					init64(regX, volatileVariables[(uint16_t)(operand)]);
					break;

				case i08:	// store volatile rX
					volatileVariables[(uint16_t)(operand)] = regX->ul[0];
					break;

				case i09:	// load rX with byte[operand]
					operand = regY->u8[(unsigned int)(operand)];
				case i10:	// load rX with byte
					init64byt(regX, operand);
					break;

#ifdef useBarGraph
				case i11:	// store rX byte to bargraph data array
					bargraphData[(unsigned int)(operand)] = regX->u8[0];
					break;

#endif // useBarGraph
				case i12:	// load rX with denominator for constant
					operand ^= 1;
				case i13:	// load rX with numerator for constant
					if (metricFlag & metricMode) operand ^= 1;
					operand ^= pgm_read_byte(&convNumerIdx[(unsigned int)(operand)]);
				case i14:	// load rX with const
					init64(regX, pgm_read_dword(&convNumbers[(uint16_t)(operand)]));
					break;

				case i15:	// load rX with eeprom init
					init64(regX, pgm_read_dword(&params[(uint16_t)(operand)]));
					break;

#if defined(useAnalogRead)
				case i16:	// load rX with voltage
					init64(regX, analogValue[(unsigned int)(operand)]);
					break;

#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
				case i17:	// load rX with FEvT trip variable
					instr = calcFEvTperiodIdx; // get current fuel econ vs time trip variable
					instr -= FEvsTimeIdx; // translate out of trip index space

					if (operand >= bgDataSize) operand = 0; // shift index
					else operand++;

					operand += instr; // add to trip variable value
					if (operand >= bgDataSize) operand -= bgDataSize; // perform wrap-around if required

					operand += FEvsTimeIdx; // shift back into trip index space
#endif // useBarFuelEconVsTime
				case i18:	// load rX with trip variable
					if (operand < tripSlotCount)
					{

						switch (extra)
						{

							case rvInjPulseIdx:
								if (operand < tripSlotFullCount) init64(regX, collectedInjPulseCount[(unsigned int)(operand)]);
								else init64byt(regX, 0);
								break;

							case rvVSScycleIdx:
								if (operand < tripSlotFullCount) copy64(regX, (union union_64 *)&collectedVSScycleCount[(uint16_t)(operand)]);
								else init64byt(regX, 0);
								break;

							case rvEngCycleIdx:
								if (operand < tripSlotFullCount) copy64(regX, (union union_64 *)&collectedEngCycleCount[(uint16_t)(operand)]);
								else init64byt(regX, 0);
								break;

							case rvVSSpulseIdx:
								init64(regX, collectedVSSpulseCount[(unsigned int)(operand)]);
								break;

							case rvInjCycleIdx:
								copy64(regX, (union union_64 *)&collectedInjCycleCount[(uint16_t)(operand)]);
								break;

							default:
								init64byt(regX, 0);
								break;

						}

					}
#if defined(useEEPROMtripStorage)
					else
					{

						branchFlag = tripVar::getBaseEEPROMaddress(operand, extra);
						if (branchFlag) EEPROM::read64(regX, branchFlag);
						else init64byt(regX, 0);

					}
#else // defined(useEEPROMtripStorage)
					else init64byt(regX, 0);
#endif // defined(useEEPROMtripStorage)
					break;

				case i19:	// store trip variable rX
					if (operand < tripSlotCount)
					{

						switch (extra)
						{

							case rvInjPulseIdx:
								if (operand < tripSlotFullCount) collectedInjPulseCount[(unsigned int)(operand)] = regX->ul[0];
								break;

							case rvVSScycleIdx:
								if (operand < tripSlotFullCount) copy64((union union_64 *)&collectedVSScycleCount[(uint16_t)(operand)], regX);
								break;

							case rvEngCycleIdx:
								if (operand < tripSlotFullCount) copy64((union union_64 *)&collectedEngCycleCount[(uint16_t)(operand)], regX);
								break;

							case rvVSSpulseIdx:
								collectedVSSpulseCount[(unsigned int)(operand)] = regX->ul[0];
								break;

							case rvInjCycleIdx:
								copy64((union union_64 *)&collectedInjCycleCount[(uint16_t)(operand)], regX);
								break;

							default:
								break;

						}

					}
#if defined(useEEPROMtripStorage)
					else
					{

						branchFlag = tripVar::getBaseEEPROMaddress(operand, extra);
						if (branchFlag) EEPROM::write64(regX, branchFlag);

					}
#endif // defined(useEEPROMtripStorage)
					break;

#if defined(useMatrixMath)
				case i20:	// load rX with element of Matrix X
					copy64(regX, (union union_64 *)&matrix_x[(uint16_t)(operand)][(uint16_t)(extra)]);
					break;

				case i21:	// store Matrix X rX
					copy64((union union_64 *)&matrix_x[(uint16_t)(operand)][(uint16_t)(extra)], regX);
					break;

				case i22:	// load rX with element of Inverse Matrix
					copy64(regX, (union union_64 *)&matrix_r[(uint16_t)(operand)][(uint16_t)(extra)]);
					break;

				case i23:	// store Inverse Matrix rX
					copy64((union union_64 *)&matrix_r[(uint16_t)(operand)][(uint16_t)(extra)], regX);
					break;

				case i24:	// load rX with element of ExpData Matrix
					copy64(regX, (union union_64 *)&matrix_e[(uint16_t)(extra)]);
					break;

				case i25:	// store ExpData Matrix rX
					copy64((union union_64 *)&matrix_e[(uint16_t)(extra)], regX);
					break;

				case i26:	// load rX with element of Coefficient Matrix
					copy64(regX, (union union_64 *)&matrix_c[(uint16_t)(extra)]);
					break;

				case i27:	// store Coefficient Matrix rX
					copy64((union union_64 *)&matrix_c[(uint16_t)(extra)], regX);
					break;

#endif // defined(useMatrixMath)
#ifdef useIsqrt
				case i28:	// integer square root
					regX->ui[0] = iSqrt(regX->ui[0]);
					break;

#endif // useIsqrt
				case i29:	// shift rX left
					SWEET64processorFlags &= ~(SWEET64carryFlag);
					shl64(regX);
					break;

				case i30:	// shift rX right
					shr64(regX);
					break;

				case i31:	// BCD adjust
					s64BCDptr = s64BCDformatList;

					while (operand)
					{

						s64BCDptr += pgm_read_byte(s64BCDptr);
						operand--;

					}

					operand = pgm_read_byte(++s64BCDptr); // fetch leading zero character
					instr = pgm_read_byte(++s64BCDptr); // fetch total BCD byte length
					extra = pgm_read_byte(++s64BCDptr); // fetch divisor string length

					regX->u8[7] = operand; // store leading zero character
					regX->u8[6] = instr; // store total BCD byte length

					if (operand) // if this is a non-zero leading zero character
					{

						while (extra--)
						{

							operand = pgm_read_byte(++s64BCDptr); // get indexed divisor

							regX->u8[(uint16_t)(--instr)] = (uint8_t)(regY->ul[0] % operand); // put result of (source register) mod divisor into indexed byte of (target register)
							regY->ul[0] /= operand; // divide (source register) by divisor

						}

					}

					break;

				default:	// invalid ixx code detected, exit program
					loopFlag = 0;
					break;

			}

			switch (opcodeSuffix & ixxMask) // enable interrupts, according to ixx
			{

				case i07:	// load rX with volatile
				case i08:	// store volatile rX
				case i18:	// load rX with trip variable
				case i19:	// store trip variable rX
					SREG = oldSREG; // restore interrupt flag status
				default:
					break;

			}

			switch (opcodeSuffix & mxxMask) // perform basic arithmetic operation, given mxx
			{

				case m00:	// non-arithmetic
					break;

				case m01:	// add			rP = rP + rS
					adc64(regP, regS);
					break;

				case m02:	// sub			rP = rP - rS
					sbc64(regP, regS, 1);
					break;

				case m03:	// compare		na = rP - rS
					sbc64(regP, regS, 0);
					break;

				case m04:	// test			r5
					registerTest64(regP);
					break;

				case m05:	// multiply		r2 = r2 * r5
#ifdef useDebugCPUreading
					mathStart = systemInfo::cycles0(); // record starting time
#endif // useDebugCPUreading
#ifdef useSWEET64mult
					prgmStack[(unsigned int)(spnt++)] = sched;
					if (spnt > 15) loopFlag = 0;
					else sched = prgmMult64;
#ifdef useSWEET64trace
					traceSave = SWEET64processorFlags;
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
#else // useSWEET64mult
					mult64();
#endif // useSWEET64mult
#ifdef useDebugCPUreading
					mainProgramVariables[(uint16_t)(mpDebugAccS64multIdx)] += systemInfo::findCycleLength(mathStart, systemInfo::cycles0());
					mainProgramVariables[(uint16_t)(mpDebugCountS64multIdx)]++;
#endif // useDebugCPUreading
					break;

				case m06:	// divide		r2 = r2 / r5 rmdr r1 and qadj r5
#ifdef useDebugCPUreading
					mathStart = systemInfo::cycles0(); // record starting time
#endif // useDebugCPUreading
#ifdef useSWEET64div
					prgmStack[(unsigned int)(spnt++)] = sched;
					if (spnt > 15) loopFlag = 0;
					else sched = prgmDiv64;
#ifdef useSWEET64trace
					traceSave = SWEET64processorFlags;
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
#else // useSWEET64div
					div64();
#endif // useSWEET64div
#ifdef useDebugCPUreading
					mainProgramVariables[(uint16_t)(mpDebugAccS64divIdx)] += systemInfo::findCycleLength(mathStart, systemInfo::cycles0());
					mainProgramVariables[(uint16_t)(mpDebugCountS64divIdx)]++;
#endif // useDebugCPUreading
					break;

				default:	// invalid mxx opcode detected, exit program
					loopFlag = 0;
					break;

			}

		}
		else // opcode does nothing at all with the 64-bit registers
		{

			switch (opcodeSuffix)
			{

				case e00:	// do nothing
					break;

				case e01:	// instrBranchIfGTorE
					branchFlag = ((SWEET64processorFlags & SWEET64zeroFlag) || (SWEET64processorFlags & SWEET64carryFlag));
					break;

				case e02:	// instrBranchIfCset
					branchFlag = (SWEET64processorFlags & SWEET64carryFlag);
					break;

				case e03:	// instrBranchIfCclear
					branchFlag = ((SWEET64processorFlags & SWEET64carryFlag) == 0);
					break;

				case e04:	// instrBranchIfZclear
					branchFlag = ((SWEET64processorFlags & SWEET64zeroFlag) == 0);
					break;

				case e05:	// instrBranchIfZset
					branchFlag = (SWEET64processorFlags & SWEET64zeroFlag);
					break;

				case e06:	// instrBranchIfMclear
					branchFlag = ((SWEET64processorFlags & SWEET64minusFlag) == 0);
					break;

				case e07:	// instrBranchIfMset
					branchFlag = (SWEET64processorFlags & SWEET64minusFlag);
					break;

				case e08:	// instrBranchIfVclear
					branchFlag = ((SWEET64processorFlags & SWEET64overflowFlag) == 0);
					break;

				case e09:	// instrBranchIfVset
					branchFlag = (SWEET64processorFlags & SWEET64overflowFlag);
					break;

				case e10:	// instrBranchIfLT
					branchFlag = (((SWEET64processorFlags & SWEET64zeroFlag) == 0) && ((SWEET64processorFlags & SWEET64carryFlag) == 0));
					break;

				case e11:	// instrBranchIfMetricMode
					branchFlag = (metricFlag & metricMode);
					break;

				case e12:	// instrBranchIfSAEmode
					branchFlag = ((metricFlag & metricMode) == 0);
					break;

				case e13:	// instrBranchIfFuelOverDist (L/100km or G/100mi)
					branchFlag = (((metricFlag & fuelEconOutputFlags) == metricMode) || ((metricFlag & fuelEconOutputFlags) == alternateFEmode));
					break;

				case e14:	// instrBranchIfDistOverFuel (MPG or KPL)
					branchFlag = (((metricFlag & fuelEconOutputFlags) == 0) || ((metricFlag & fuelEconOutputFlags) == fuelEconOutputFlags));
					break;

				case e15:	// instrSkip
					branchFlag = true;
					break;

				case e17:	// instrTraceDone
#ifdef useSWEET64trace
					if (traceSave & SWEET64traceFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
					else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
				case e16:	// instrDone
					if (spnt--) sched = prgmStack[(unsigned int)(spnt)];
					else loopFlag = 0;
					break;

				case e18:	// instrTestIndex
					if (tripIdx) SWEET64processorFlags &= ~(SWEET64zeroFlag);
					else SWEET64processorFlags |= (SWEET64zeroFlag);

					if (tripIdx & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
					else SWEET64processorFlags &= ~(SWEET64minusFlag);

					break;

				case e19:	// instrTraceRestore
#ifdef useSWEET64trace
					if (traceSave & SWEET64traceFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
					else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
					break;

				case e20:	// instrTraceOn
#ifdef useSWEET64trace
					SWEET64processorFlags |= (SWEET64traceCommandFlag);
#endif // useSWEET64trace
					break;

				case e21:	// instrTraceSave
#ifdef useSWEET64trace
					traceSave = SWEET64processorFlags;
#endif // useSWEET64trace
				case e22:	// instrTraceOff
#ifdef useSWEET64trace
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // useSWEET64trace
					break;

				case e23:	// load index
					tripIdx = operand;
					break;

				case e24:	// load index eeprom
					tripIdx = EEPROM::readVal(operand);
					break;

				case e25:	// compare index
					if (operand < tripIdx) SWEET64processorFlags |= (SWEET64carryFlag);
					else SWEET64processorFlags &= ~(SWEET64carryFlag);

					if (operand == tripIdx) SWEET64processorFlags |=(SWEET64zeroFlag);
					else SWEET64processorFlags &= ~(SWEET64zeroFlag);

					if ((operand - tripIdx) & 0x80) SWEET64processorFlags |=(SWEET64minusFlag);
					else SWEET64processorFlags &= ~(SWEET64minusFlag);

					break;

				case e26:	// load index eeprom parameter length
					tripIdx = EEPROM::getLength(operand);
					break;

				case e27:	// call
					prgmStack[(unsigned int)(spnt++)] = sched;
					if (spnt > 15) loopFlag = 0;
				case e28:	// jump
					sched = (const uint8_t *)pgm_read_word(&S64programList[(unsigned int)(extra)]);
					break;

				case e29:	// load jump register
					jumpVal = operand;
					break;

				case e30:	// clear register flag
				case e31:	// set register flag
					flagSet((opcodeSuffix == e31), operand);
					break;

				default:	// invalid sxx code detected, exit program
					loopFlag = 0;
					break;

			}

			if (branchFlag)
			{

				if (extra < 128) sched += extra;
				else sched -= (256 - extra);

			}

		}

#ifdef useSWEET64trace
		if (SWEET64processorFlags & SWEET64traceFlag)
		{

			text::charOut(devDebugTerminal, 13);
			text::charOut(devDebugTerminal, 9);
			text::charOut(devDebugTerminal, 9);
			text::hexByteOut(devDebugTerminal, SWEET64processorFlags);
			text::charOut(devDebugTerminal, 32);
			text::hexByteOut(devDebugTerminal, branchFlag);
			text::charOut(devDebugTerminal, 13);

			for (uint8_t x = 0; x < 5; x++)
			{

				text::charOut(devDebugTerminal, 9);
				text::charOut(devDebugTerminal, 9);
				text::charOut(devDebugTerminal, 9);
				text::hexByteOut(devDebugTerminal, (x + 1));
				text::charOut(devDebugTerminal, 32);
				text::hexLWordOut(devDebugTerminal, &s64reg[(uint16_t)(x)]);
				text::charOut(devDebugTerminal, 13);

			}

			text::charOut(devDebugTerminal, 13);

		}

#endif // useSWEET64trace
	}
	while (loopFlag);

#ifdef useDebugCPUreading
	SWEET64timerLength += systemInfo::findCycleLength(s64Start, systemInfo::cycles0());

#endif // useDebugCPUreading
	return ((union union_64 *)(&s64reg[(uint16_t)(s64reg2)]))->ul[0];

}

static uint8_t SWEET64::fetchByte(const uint8_t * &prgmPtr)
{

	uint8_t byt;

	byt = pgm_read_byte(prgmPtr++);

#ifdef useSWEET64trace
	if (SWEET64processorFlags & SWEET64traceFlag)
	{

		text::hexByteOut(devDebugTerminal, byt);
		text::charOut(devDebugTerminal, 32);

	}

#endif // useSWEET64trace
	return byt;

}

static void SWEET64::copy64(union union_64 * an, union union_64 * ann)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		"	ld	__tmp_reg__, %a1+	\n"
		"	st	%a0+, __tmp_reg__	\n"

		: "+e" (an)
		: "e" (ann)
	);
#else // useAssemblyLanguage
	for (uint8_t x = 0; x < 4; x++) an->ui[(unsigned int)(x)] = ann->ui[(unsigned int)(x)];
#endif // useAssemblyLanguage

}

static void SWEET64::swap64(union union_64 * an, union union_64 * ann) // swap ann and an
{

	uint16_t aing;
	uint8_t x;

#ifdef useAssemblyLanguage
	asm volatile(
		"	ldi	%A3, 8				\n"		// initialize counter

		"l_swap64%=:				\n"
		"	ld	%A2, %a0			\n"
		"	ld	%B2, %a1			\n"
		"	st	%a1+, %A2			\n"
		"	st	%a0+, %B2			\n"
		"	dec	%A3					\n"
		"	brne l_swap64%=			\n"
		: "+e" (ann), "+e" (an), "+r" (aing), "+r" (x)
	);
#else // useAssemblyLanguage
	for (x = 0; x < 4; x++)
	{

		aing = ann->ui[(unsigned int)(x)];
		ann->ui[(unsigned int)(x)] = an->ui[(unsigned int)(x)];
		an->ui[(unsigned int)(x)] = aing;

	}
#endif // useAssemblyLanguage

}

static void SWEET64::shr64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	asm volatile(
		"	clc						\n"
		"	adiw	%a0, 8			\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_shr64%=:					\n"
		"	ld	__tmp_reg__, -%a0	\n"
		"	ror	__tmp_reg__			\n"
		"	st	%a0, __tmp_reg__	\n"
		"	or	%A2, __tmp_reg__	\n"		// update zero flag
		"	dec	%A4					\n"
		"	brne l_shr64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;
	z = 0;

	for (x = 7; x < 8; x--)
	{

		n->u8[1] = an->u8[(uint16_t)(x)];
		n->u8[0] = 0;
		enn >>= 1;
		m = c;
		m |= n->u8[1];
		c = n->u8[0];
		an->u8[(uint16_t)(x)] = m;
		z |= m;

	}

#endif // useAssemblyLanguage
	m = an->u8[7];

	flagSet64(m, z, c);

}

static void SWEET64::shl64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 1 : 0);

	asm volatile(
		"	ror	%A1					\n"		// shift input carry bit into avr carry
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_shl64%=:					\n"
		"	ld	%A3, %a0			\n"
		"	rol	%A3					\n"
		"	st	%a0+, %A3			\n"
		"	or	%A2, %A3			\n"		// update zero flag
		"	dec	%A4					\n"
		"	brne l_shl64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = ((SWEET64processorFlags & SWEET64carryFlag) ? 0x01 : 0x00);
	z = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[0] = an->u8[(uint16_t)(x)];
		n->u8[1] = 0;
		enn <<= 1;
		m = c;
		m |= n->u8[0];
		c = n->u8[1];
		an->u8[(uint16_t)(x)] = m;
		z |= m;

	}
#endif // useAssemblyLanguage

	flagSet64(m, z, c);

}
// SWEET64 64-bit addition instruction
//
// adds ann to an, and stores the result in an
//
// if an + ann results in overflow,		c=1
//						otherwise,		c=0
//
// if an + ann results in zero,			z=1
//						otherwise,		z=0
//
// if an + ann sum has high bit set,	m=1
//						otherwise,		m=0
//
static void SWEET64::adc64(union union_64 * an, union union_64 * ann)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	asm volatile (
		"	clc						\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_adc64%=:					\n"
		"	ld	%A3, %a0			\n"
		"	ld	%A1, %a5+			\n"
		"	adc	%A3, %A1			\n"
		"	st	%a0+, %A3			\n"
		"	or	%A2, %A3			\n"		// update zero flag
		"	dec	%A4					\n"
		"	brne l_adc64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
		: "e" (ann)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;
	z = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[0] = c;
		n->u8[1] = 0;
		enn += an->u8[(uint16_t)(x)];
		enn += ann->u8[(uint16_t)(x)];
		m = n->u8[0];
		c = n->u8[1];
		an->u8[(uint16_t)(x)] = m;
		z |= m;

	}
#endif // useAssemblyLanguage

	flagSet64(m, z, c);

}

// SWEET64 64-bit subtract instruction
//
// subtracts ann from an, and stores the result in an
// doubles as 64-bit compare operation (results are not stored)
//
// if an < ann, 		z=0, c=1
// if an == ann,		z=1, c=0
// if an > ann,			z=0, c=0
// if (an - ann) < 0,	m=1
// if (an - ann) >= 0,	m = 0
static void SWEET64::sbc64(union union_64 * an, union union_64 * ann, uint8_t sbcFlag)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#ifdef useAssemblyLanguage

	asm volatile (
		"	clc						\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A4, 8				\n"		// initialize counter

		"l_sub64%=:					\n"
		"	ld	%A3, %a0			\n"
		"	ld	%A1, %a5+			\n"
		"	sbc	%A3, %A1			\n"
		"	tst	%A6					\n"		// is this a subtract operation
		"	breq	c_sub64%=		\n"		// if not, just a compare, so throw away result
		"	st	%a0, %A3			\n"		// store result

		"c_sub64%=:					\n"
		"	or	%A2, %A3			\n"		// update zero flag
//		"	adiw	%a0, 1			\n"		// this does not work for some reason
		"	ld	__tmp_reg__, %a0+	\n"		// there is apparently no way to directly increment %a0
		"	dec	%A4					\n"
		"	brne l_sub64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+r" (c), "+r" (z), "+r" (m), "+r" (x)
		: "e" (ann), "r" (sbcFlag)
	);
#else // useAssemblyLanguage
	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	c = 0;
	z = 0;

	for (x = 0; x < 8; x++)
	{

		n->u8[1] = c;
		n->u8[0] = c;
		enn += an->u8[(uint16_t)(x)];
		enn -= ann->u8[(uint16_t)(x)];
		m = n->u8[0];
		c = n->u8[1];
		if (sbcFlag) an->u8[(uint16_t)(x)] = m;
		z |= m;

	}
#endif // useAssemblyLanguage

	flagSet64(m, z, c);

}

static void SWEET64::registerTest64(union union_64 * an)
{

	uint8_t z;
	uint8_t m;
	uint8_t v;
	uint8_t x;

#ifdef useAssemblyLanguage
	asm volatile(
		"	mov	%A0, __zero_reg__	\n"
		"	mov	%A2, __zero_reg__	\n"		// initialize zero flag
		"	ldi	%A3, 8				\n"		// initialize counter

		"l_tst64%=:					\n"
		"	ld	%A1, %a4+			\n"		// load temp register with byte of an
		"	or	%A0, %A1			\n"
		"	com	%A1					\n"		// invert temp register
		"	or	%A2, %A1			\n"
		"	dec	%A3					\n"
		"	brne l_tst64%=			\n"

		"	com	%A1					\n"		// restore temp register to original value
		: "+r" (z), "+r" (m), "+r" (v), "+r" (x)
		: "e" (an)
	);
#else // useAssemblyLanguage
	z = 0;
	v = 0;

	for (x = 0; x < 8; x++)
	{

		m = an->u8[(uint16_t)(x)];
		z |= m;
		if (m != 0xFF) v = 1;

	}
#endif // useAssemblyLanguage

	flagSet((m & 0x80), SWEET64minusFlag);
	flagSet((z == 0), SWEET64zeroFlag);
	flagSet((v == 0), SWEET64overflowFlag);

//	if (m & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
//	else SWEET64processorFlags &= ~(SWEET64minusFlag);

//	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
//	else SWEET64processorFlags |= (SWEET64zeroFlag);

//	if (v) SWEET64processorFlags &= ~(SWEET64overflowFlag);
//	else SWEET64processorFlags |= (SWEET64overflowFlag);

}

static void SWEET64::mult64(void)
{

	union union_64 * an = (union union_64 *)(&s64reg[s64reg2]);	// multiplier in an, result to an
	union union_64 * multiplicand = (union union_64 *)(&s64reg[s64reg5]);	// define multiplicand term as register 5
#ifdef useObsoleteMult64
	union union_64 * multiplier = (union union_64 *)(&s64reg[s64reg4]);	// define multiplier term as register 4

	copy64(multiplier, an); // load multiplier into register 4
	init64byt(an, 0); // zero out result

	sbc64(multiplicand, multiplier, 0); // is multiplier less than multiplicand?

	if (SWEET64processorFlags & SWEET64carryFlag) swap64(multiplicand, multiplier); // if not, swap them around

	registerTest64(multiplier);

	while (!(SWEET64processorFlags & SWEET64zeroFlag)) // while multiplier is non-zero
	{

		shr64(multiplier); // shift multiplier right by one bit

		if (SWEET64processorFlags & SWEET64carryFlag) adc64(an, multiplicand); // if the low bit of multiplier is set, add multiplicand to result

		SWEET64processorFlags &= ~(SWEET64carryFlag);
		shl64(multiplicand); // shift multiplicand left by one bit

	}

#else // useObsoleteMult64
#ifdef useAssemblyLanguage

	asm volatile(
		"	push	r0				\n"		// save this register which 'mul' will trash

		"	push	r1				\n"		// save this register which 'mul' will trash

		"	push	r12				\n"		// save result register original contents
		"	push	r13				\n"
		"	push	r14				\n"
		"	push	r15				\n"
		"	push	r16				\n"
		"	push	r17				\n"
		"	push	r18				\n"
		"	push	r19				\n"
		"	push	r20				\n"		// (result byte 9 is thrown away at end)
		"	push	r21				\n"		// (result byte 10 is thrown away at end)

		"	push	r22				\n"		// save multiplication counter original contents

		"	push	r23				\n"		// save secondary counter original contents

		"	push	r24				\n"		// save multiplication workspace

		"	push	r28				\n"		// save Y reg original contents
		"	push	r29				\n"

		"	ldi	r22, 0				\n"		// initialize counter

		"	ldi	r21, 0				\n"		// initialize result register
		"	mov	r20, r21			\n"
		"	mov	r19, r21			\n"
		"	mov	r18, r21			\n"
		"	mov	r17, r21			\n"
		"	mov	r16, r21			\n"
		"	mov	r15, r21			\n"
		"	mov	r14, r21			\n"
		"	mov	r13, r21			\n"
		"	mov	r12, r21			\n"

		"l1_mul64%=:				\n"
		"	push	r26				\n"		// save X reg original contents (multiplier)
		"	push	r27				\n"
		"	push	r30				\n"		// save Z reg original contents (multiplicand)
		"	push	r31				\n"

		"	mov	r23, r22			\n"		// initialize secondary counter
		"	ldi	r28, 12				\n"		// initialize Y reg as result register pointer
		"	ldi	r29, 0				\n"
		"	add	r28, r23			\n"		// point to byte offset in result register
		"	inc	r23					\n"		// bump secondary counter up by one
		"	add	r30, r23			\n"		// set up multiplicand pointer
		"	adc	r31, r29			\n"

		"l2_mul64%=:				\n"
		"	push	r28				\n"		// save Y reg result register pointer
		"	push	r29				\n"
		"	ld	r0, X+				\n"		// load in ascending order the byte of multiplier
		"	ld	r1, -Z				\n"		// load in descending order the byte of multiplicand
		"	mul	r0, r1				\n"		// perform multiplication (since mul will trash r0 and r1 anyway, why not use them for input?)
		"	ld	r24, Y				\n"		// load first intermediate byte of result register
		"	add	r24, r0				\n"		// add low byte of mul result
		"	st	Y+, r24				\n"		// save first intermediate byte of result register
		"	ld	r24, Y				\n"		// load next byte of result register
		"	adc	r24, r1				\n"		// add high byte of mul result
		"	st	Y+, r24				\n"		// save next byte of result register
		"	ld	r24, Y				\n"		// load third byte of result register
		"	adc	r24, r29			\n"		// add impromptu __zero_reg__ with carry
		"	st	Y+, r24				\n"		// save third byte of result register
		"	pop	r29					\n"		// restore Y reg result register pointer
		"	pop	r28					\n"

		"	dec	r23					\n"		// bump down secondary counter
		"	brne	l2_mul64%=		\n"

		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"
		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"

		"	inc	r22					\n"		// bump up multiplication counter
		"	cpi	r22, 8				\n"		// check if done
		"	brlo	l1_mul64%=		\n"		// if not, go process another byte

		"	st	X+, r12				\n"		// save result register to multiplier
		"	st	X+, r13				\n"
		"	st	X+, r14				\n"
		"	st	X+, r15				\n"
		"	st	X+, r16				\n"
		"	st	X+, r17				\n"
		"	st	X+, r18				\n"
		"	st	X+, r19				\n"

		"	pop	r29					\n"		// restore Y reg original contents
		"	pop	r28					\n"

		"	pop	r24					\n"		// restore multiplication workspace

		"	pop	r23					\n"		// restore secondary counter original contents

		"	pop	r22					\n"		// restore counter original contents

		"	pop	r21					\n"		// throw away result byte 10
		"	pop	r20					\n"		// throw away result byte 10
		"	pop	r19					\n"		// restore result register original contents
		"	pop	r18					\n"
		"	pop	r17					\n"
		"	pop	r16					\n"
		"	pop	r15					\n"
		"	pop	r14					\n"
		"	pop	r13					\n"
		"	pop	r12					\n"

		"	pop	r1					\n"		// restore this register which 'mul' trashed
		"	pop	r0					\n"		// restore this register which 'mul' trashed
		:
		: "x" (an), "z" (multiplicand)
	);

#else // useAssemblyLanguage
	union union_64 * multiplier = (union union_64 *)(&s64reg[s64reg4]);	// define multiplier term as register 4

	uint16_t enn;
	union union_16 * n = (union union_16 *)(&enn);

	uint16_t pee;
	union union_16 * p = (union union_16 *)(&pee);

	uint8_t c;

	copy64(multiplier, an); // load multiplier
	init64byt(an, 0); // zero out result

	for (uint8_t m = 0; m < 8; m++)
	{

		for (uint8_t x = 0; x <= m; x++)
		{

			enn = multiplier->u8[(uint16_t)(x)] * multiplicand->u8[(uint16_t)(m - x)];

			c = 0;

			for (uint8_t i = 0; i < 2; i++)
			{

				if (m + i < 8)
				{

					p->u8[0] = c;
					p->u8[1] = 0;
					pee += an->u8[(uint16_t)(m + i)];
					pee += n->u8[(uint16_t)(i)];
					an->u8[(uint16_t)(m + i)] = p->u8[0];
					c = p->u8[1];

				}

			}

			if ((c) && (m < 6)) an->u8[(uint16_t)(m + 2)]++;

		}

	}

#endif // useAssemblyLanguage
#endif // useObsoleteMult64
}

#ifndef useSWEET64div
static void SWEET64::div64(void) // uses algorithm for non-restoring hardware division
{

	union union_64 * ann = (union union_64 *)(&s64reg[s64reg1]);	// remainder in ann
	union union_64 * divisor = (union union_64 *)(&s64reg[s64reg5]);
#ifdef useDiv64Assembly

	asm volatile(
		"	push	r28				\n"		// save Y reg original contents
		"	push	r29				\n"
		"	push	r25				\n"

		"	clr	r28					\n"		// point Y reg to start of register contents
		"	clr	r29					\n"
		"d64_iloop%=:				\n"
		"	ld	r25, y+				\n"		// save register original contents
		"	push	r25				\n"
		"	cpi	r28, 25				\n"
		"	brne	d64_iloop%=		\n"

		"	push	r30				\n"		// save Z reg original contents (divisor)
		"	push	r31				\n"
		"	push	r26				\n"		// save X reg original contents (remainder)
		"	push	r27				\n"

		"	clr	r25					\n"		// initialize zero-test register

		"	ldi	r26, 16				\n"		// point X reg to divisor register
		"	clr	r27					\n"
		"d64_cpd%=:					\n"
		"	ld	r24, z				\n"		// load divisor contents into divisor register
		"	or	r25, r24			\n"		// also perform check if divisor is zero
		"	st	x+, r24				\n"
		"	st	z+, r27				\n"		// store a zero in divisor variable
		"	cpi	r26, 24				\n"
		"	brne	d64_cpd%=		\n"

		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"

		"	tst	r25					\n"
		"	brne	d64_cont1%=		\n"

		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"

		"	clr	r24					\n"		// generate overflow value
		"	com	r24					\n"
		"	ldi	r25, 16				\n"		// set remainder contents as overflow value

		"d64_ovfl%=:				\n"
		"	st	x+, r24				\n"		// store overflow value into remainder and quotient
		"	dec	r25					\n"
		"	brne	d64_ovfl%=		\n"
		"	rjmp	d64_exit%=		\n"		// go exit

		"d64_cont1%=:				\n"
		"	push	r26				\n"		// save X reg original contents (remainder)
		"	push	r27				\n"

		"	clr	r0					\n"		// zero out remainder register
		"	clr	r1					\n"
		"	clr	r2					\n"
		"	clr	r3					\n"
		"	clr	r4					\n"
		"	clr	r5					\n"
		"	clr	r6					\n"
		"	clr	r7					\n"

		"	adiw	r26, 8			\n"		// skip over remainder

		"	ldi	r30, 8				\n"		// point Z reg to quotient register
		"	clr	r31					\n"
		"d64_cpq%=:					\n"
		"	ld	r24, x+				\n"		// move quotient contents from quotient variable into quotient register
		"	st	z+, r24				\n"
		"	cpi	r30, 16				\n"
		"	brne	d64_cpq%=		\n"

		"	ldi	r24, 64				\n"		// load bit count into counter

		"d64_loop0%=:				\n"
		"	tst	r15					\n"		// is high byte of quotient zero?
		"	brne	d64_loop1%=		\n"		// if not, proceed with division
		"	mov	r15, r14			\n"		// otherwise, do a byte shift leftward
		"	mov	r14, r13			\n"
		"	mov	r13, r12			\n"
		"	mov	r12, r11			\n"
		"	mov	r11, r10			\n"
		"	mov	r10, r9				\n"
		"	mov	r9, r8				\n"
		"	clr	r8					\n"
		"	subi	r24, 8			\n"		// bump down the bit count by 8
		"	brne	d64_loop0%=		\n"		// if bit count is non-zero, loop back

		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"
		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"
		"	rjmp	d64_cont4%=		\n"		// otherwise, div64 is done

		"d64_add%=:					\n"
		"	add	r0, r16				\n"		// add divisor to remainder
		"	adc	r1, r17				\n"
		"	adc	r2, r18				\n"
		"	adc	r3, r19				\n"
		"	adc	r4, r20				\n"
		"	adc	r5, r21				\n"
		"	adc	r6, r22				\n"
		"	adc	r7, r23				\n"
		"	ret						\n"

		"d64_loop1%=:				\n"
		"	lsl	r8					\n"		// shift quotient left one bit
		"	rol	r9					\n"
		"	rol	r10					\n"
		"	rol	r11					\n"
		"	rol	r12					\n"
		"	rol	r13					\n"
		"	rol	r14					\n"
		"	rol	r15					\n"
		"	rol	r0					\n"		// shift remainder left one bit, shifting in most significant bit of quotient
		"	rol	r1					\n"
		"	rol	r2					\n"
		"	rol	r3					\n"
		"	rol	r4					\n"
		"	rol	r5					\n"
		"	rol	r6					\n"
		"	rol	r7					\n"
		"	brcc	d64_l1s%=		\n"		// if last operation resulted in a positive remainder, jump ahead

		"	rcall	d64_add%=		\n"		// go add divisor to remainder
		"	rjmp	d64_l1b%=		\n"

		"d64_l1s%=:					\n"
		"	sub	r0, r16				\n"		// subtract divisor from remainder
		"	sbc	r1, r17				\n"
		"	sbc	r2, r18				\n"
		"	sbc	r3, r19				\n"
		"	sbc	r4, r20				\n"
		"	sbc	r5, r21				\n"
		"	sbc	r6, r22				\n"
		"	sbc	r7, r23				\n"

		"d64_l1b%=:					\n"
		"	tst	r7					\n"
		"	brmi	d64_l1c%=		\n"
		"	inc	r8					\n"
		"d64_l1c%=:					\n"
		"	dec	r24					\n"
		"	brne	d64_loop1%=		\n"

		"	tst	r7					\n"
		"	brpl	d64_cont2%=		\n"		// if last operation resulted in a positive remainder, jump ahead
		"	rcall	d64_add%=		\n"		// go add divisor to remainder

		"d64_cont2%=:				\n"
		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"
		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"

		"	lsr	r23					\n"		// divide divisor by 2
		"	ror	r22					\n"
		"	ror	r21					\n"
		"	ror	r20					\n"
		"	ror	r19					\n"
		"	ror	r18					\n"
		"	ror	r17					\n"
		"	ror	r16					\n"

		"	cp	r7, r23				\n"		// compare r8[7] to d8[7]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r6, r22				\n"		// compare r8[6] to d8[6]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r5, r21				\n"		// compare r8[5] to d8[5]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r4, r20				\n"		// compare r8[4] to d8[4]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r3, r19				\n"		// compare r8[3] to d8[3]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r2, r18				\n"		// compare r8[2] to d8[2]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r1, r17				\n"		// compare r8[1] to d8[1]
		"	brne	d64_cont3%=		\n"		// if not equal, go figure out if less or greater
		"	cp	r0, r16				\n"		// compare r8[0] to d8[0]

		"d64_cont3%=:				\n"
		"	brlo	d64_cont4%=		\n"		// if remainder is less than divisor / 2, skip ahead
		"	inc	r24					\n"
		"	st	z, r24				\n"		// store quotient adjust value

		"d64_cont4%=:				\n"
		"	clr	r30					\n"		// point z register to point to remainder register
		"	clr	r31					\n"
		"	ldi	r25, 16				\n"		// initialize counter
		"d64_saveqr%=:				\n"
		"	ld	r24, z+				\n"		// move remainder contents to remainder variable
		"	st	x+, r24				\n"		// also move quotient contents to quotient variable
		"	dec	r25					\n"
		"	brne	d64_saveqr%=	\n"

		"d64_exit%=:				\n"
		"	ldi	r28, 25				\n"		// point Y reg to end of register contents
		"	clr	r29					\n"
		"d64_eloop%=:				\n"
		"	pop	r25					\n"		// restore register original contents
		"	st	-y, r25				\n"
		"	tst	r28					\n"
		"	brne	d64_eloop%=		\n"

		"	pop	r25					\n"
		"	pop	r29					\n"		// restore Y reg original contents
		"	pop	r28					\n"
		:
		: "x" (ann), "z" (divisor)
	);

#else // useDiv64Assembly
	union union_64 * an = (union union_64 *)(&s64reg[s64reg2]);	// quotient in an

	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t s;

	init64byt(ann, 0); // zero out remainder

	registerTest64(divisor);

	if (SWEET64processorFlags & SWEET64zeroFlag) // if divisor is zero, mark as overflow, then exit
	{

		init64byt(an, 1); // set quotient to 1
		sbc64(ann, an, 1); // subtract 1 from zeroed-out remainder to generate overflow value
		copy64(an, ann); // copy overflow value to remainder

	}
	else
	{

		x = 64;							// start off with a dividend size of 64 bits

		y = 7; //
		while ((x) && (an->u8[(unsigned int)(y)] == 0))		// examine dividend for leading zero bytes
		{

			y--;						// if this byte is zero, skip to look at next byte
			x -= 8;						// shrink dividend size by 8 bits

		}

		if ((x) && (y < 7)) // if upper-region zero bytes were found, shift quotient left to eliminate them
		{

			for (z = 7; z < 8; z--)
			{

				if (y < 7) an->u8[(unsigned int)(z)] = an->u8[(unsigned int)(y)];
				else an->u8[(unsigned int)(z)] = 0;
				y--;

			}

		}

		s = 0;

		while (x) // while there are still bits in dividend to process
		{

			SWEET64processorFlags &= ~(SWEET64carryFlag); // default is "previous operation resulted in A < 0"
			shl64(an);					// shift Q left one bit
			shl64(ann);					// shift A left one bit

			if (s) adc64(ann, divisor); // add M to A if previous operation resulted in A < 0
			else sbc64(ann, divisor, 1); // subtract M from A if previous operation resulted in A >= 0

			s = (ann->u8[7] & 0x80); // get previous operation sign bit

			if (s == 0) an->u8[0] |= 0x01; // if previous operation resulted in A >= 0, then adjust quotient

			x--; // reduce dividend bit count by one

		}

		if (s) adc64(ann, divisor); // if most recent operation resulted in A < 0, make A (remainder) positive by adding M to it

		// provide a means to round to the next integer
		// if (R / D) < 0.5, then round down (add 0 to quotient)
		// otherwise, if (R / D) >= 0.5, round up (add 1 to quotient)

		shr64(divisor);			// divide divisor by 2 by shifting right one bit
		sbc64(ann, divisor, 0);

		if (SWEET64processorFlags & SWEET64carryFlag) s = 0; // if remainder is less than divisor / 2
		else s = 1;	// otherwise, store a 1

		init64byt(divisor, s); // store quotient adjust term

	}

#endif // useDiv64Assembly
}

#endif // useSWEET64div
static void SWEET64::init64byt(union union_64 * an, uint8_t byt)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"	st	%a0,   %A1	\n"
		"	std	%a0+1, __zero_reg__	\n"
		"	std	%a0+2, __zero_reg__	\n"
		"	std	%a0+3, __zero_reg__	\n"
		"	std	%a0+4, __zero_reg__	\n"
		"	std	%a0+5, __zero_reg__	\n"
		"	std	%a0+6, __zero_reg__	\n"
		"	std	%a0+7, __zero_reg__	\n"
		: "+e" (an)
		: "r" (byt)
	);
#else // useAssemblyLanguage
	for (uint8_t x = 1; x < 8; x++) an->u8[(uint16_t)(x)] = 0;
	an->u8[0] = byt;
#endif // useAssemblyLanguage

}

static void SWEET64::init64(union union_64 * an, unsigned long dWordL)
{

#ifdef useAssemblyLanguage
	asm volatile(
		"	st	%a0,   %A1	\n"
		"	std	%a0+1, %B1	\n"
		"	std	%a0+2, %C1	\n"
		"	std	%a0+3, %D1	\n"
		"	std	%a0+4, __zero_reg__	\n"
		"	std	%a0+5, __zero_reg__	\n"
		"	std	%a0+6, __zero_reg__	\n"
		"	std	%a0+7, __zero_reg__	\n"
		: "+e" (an)
		: "r" (dWordL)
	);
#else // useAssemblyLanguage
	an->ul[1] = 0;
	an->ul[0] = dWordL;
#endif // useAssemblyLanguage

}

static void SWEET64::flagSet(uint8_t condition, const uint8_t flag)
{

	if (condition) SWEET64processorFlags |= (flag);
	else SWEET64processorFlags &= ~(flag);

}

static void SWEET64::flagSet64(uint8_t n, uint8_t z, uint8_t c)
{

	flagSet((n & 0x80), SWEET64minusFlag);
	flagSet((z == 0), SWEET64zeroFlag);
	flagSet((c), SWEET64carryFlag);

//	if (n & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
//	else SWEET64processorFlags &= ~(SWEET64minusFlag);

//	if (z) SWEET64processorFlags &= ~(SWEET64zeroFlag);
//	else SWEET64processorFlags |= (SWEET64zeroFlag);

//	if (c) SWEET64processorFlags |= (SWEET64carryFlag);
//	else SWEET64processorFlags &= ~(SWEET64carryFlag);

}

#ifdef useIsqrt
static unsigned int iSqrt(unsigned int n)
{

	unsigned long w = 4096; // square factor guess
	unsigned int t = 4096; // proposed square root
	int d; // difference between guess and proposed
	int od = 0;

	for (uint8_t x = 0; x < 6; x++)
	{

		od = d;
		d = n - (unsigned int)w;
		d >>= 1;
		t += d;

		od += d;

		if ((d == 0) || (od == 0)) break;

		w = (unsigned long)t * (unsigned long)t;
		w >>= 12;

	}

	return t;

}

#endif // useIsqrt
static const uint8_t prgmMultiplyBy100[] PROGMEM = {
	instrMul2byByte, 100,								// multiply result by 100
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static const uint8_t prgmMultiplyBy10[] PROGMEM = {
	instrMul2byByte, 10,								// multiply result by 10
	instrAddIndexToX, 0x02		,						// add whatever's in the trip variable index to result
	instrDone											// exit to caller
};

static unsigned long str2ull(char * strBuffer)
{

	uint8_t c;
	uint8_t n;

	uint8_t x;
	uint8_t f;
	uint8_t loopFlag;

	x = 0;
	n = 0;
	f = 1;
	loopFlag = 1;

	SWEET64::init64byt((union union_64 *)(&s64reg[s64reg2]), 0); // initialize 64-bit number to zero

	while ((loopFlag) && (x < 17))
	{

		if ((c = strBuffer[(unsigned int)(x++)])) // if a non-NULL character is read in
		{

			n *= 10; // shift accumulator left one digit
			if (c != ' ') n += (uint8_t)(c) - 48; // if character is not a leading space, go add it to accumulator
			f= 1 - f; // flip-flop the SWEET64 addition flag

			if (f)
			{

				SWEET64::runPrgm(prgmMultiplyBy100, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 100 + n
				n = 0;

			}

		}
		else loopFlag = 0; // otherwise, terminate loop upon receipt of a NULL character

	}

	if (f == 0) SWEET64::runPrgm(prgmMultiplyBy10, n); // call SWEET64 routine to perform (accumulated 64-bit number) * 10 + n

	return ((union union_64 *)(&s64reg[s64reg2]))->ul[0];

}

static void storeDigit(uint8_t value, char * strBuffer, uint8_t &strPos, uint8_t &decPos, char &zeroChar, uint8_t &digCnt, uint8_t flg)
{

	decPos--; // bump down decimal position

	if ((value) || (decPos == 1)) // if either this digit is not zero, or at 1s position
	{

		value += '0'; // shift value into character 0-9 range
		zeroChar = '0'; // turn leading zero character into a 0

	}
	else value = zeroChar; // otherwise, use previously defined leading zero character

	if (value != ' ') digCnt++;

	if ((decPos == 0) && (flg)) strBuffer[(uint16_t)(strPos++)] = '.'; // if at 1/10ths position, store a decimal point
	strBuffer[(uint16_t)(strPos++)] = value; // store converted character value

}

// converts the 64-digit number stored in SWEET64 register 2 into an up-to 10-digit decimal number string
// a SWEET64 indexed program is called to do the initial processing of the number from 64-bit to an initial BCD string
// the SWEET64 program must specify the leading zero character and the number of BCD bytes
//
// the returned string will always have at least one digit at the rightmost string position
// the string is terminated with a NULL character
//
// this routine can handle numbers up to 9999999999
//
// if called with prgmIdx = tRoundOffNumber, also inserts the decimal point in the string specified by the value in decimalPlaces
//
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t prgmIdx)
{

	union union_64 * tmpPtr2 = (union union_64 *)(&s64reg[s64reg2]);

	uint8_t l;
	uint8_t value;
	uint8_t strPos;
	uint8_t decPos;
	uint8_t digCnt;
	uint8_t flg;
	char zeroChar;

	SWEET64::doCalculate(decimalPlaces, prgmIdx); // call SWEET64 routine to perform decimal point rounding to next nearest decimal place

	l = tmpPtr2->u8[6];	// load total length of binary-coded decimal bytes of converted number

	if (l == 255) strcpy_P(strBuffer, overFlowStr); // if length is 255, this number overflowed
	else
	{

		if (prgmIdx == tRoundOffNumber) flg = 1; // if using tRoundOffNumber to process number, do decimal conversion
		else flg = 0;

		if (flg) decPos = 11 - decimalPlaces; // if using tRoundOffNumber, compute decimal position
		else decPos = 11;

		zeroChar = (char)(tmpPtr2->u8[7]);	// load leading zero character
		strPos = 0; // set initial string buffer position
		digCnt = 0; // set initial digit count

		for (uint8_t x = 0; x < l; x++) // go through all of the binary-coded decimal bytes of converted number
		{

			value = tmpPtr2->u8[(uint16_t)(x)];	// load a binary-coded decimal byte of number

			storeDigit(value / 10, strBuffer, strPos, decPos, zeroChar, digCnt, flg); // store 10's place digit in string buffer
			storeDigit(value % 10, strBuffer, strPos, decPos, zeroChar, digCnt, flg); // store 1's place digit in string buffer

		}

		strBuffer[(unsigned int)(strPos++)] = 0; // mark end of string buffer with a NULL character
		strBuffer[(unsigned int)(strPos++)] = digCnt; // store digit count at 1 past the string buffer end

	}

	return strBuffer; // return pointer to string buffer containing the number

}

static const uint8_t prgmAutoRangeNumber[] PROGMEM = {
	instrLdReg, 0x23,									// save contents of register 2
	instrAddIndex, 0xFE,								// bump window length down by 2
	instrCmpIndex, 0xFF,								// was window length 1 digit?
	instrBranchIfE, 8,									// if so, go process corner case
	instrCmpIndex, 9,									// check window length for validity
	instrBranchIfLT, 9,									// if valid, skip ahead
	instrLxdI, 8,										// assume window length of 8 digits
	instrSkip, 5,										// skip ahead
	instrLdRegConst, 0x02, idxDecimalPoint,				// window length is 1 digit, load equivalent of decimal formatting term
	instrSkip, 4,										// skip ahead

//cont:
	instrLdRegConstIndexed, 0x02,						// load power of 10 corresponding to window into register 2
	instrMul2byConst, idxDecimalPoint,					// adjust by decimal formatting term

//cont2:
	instrLxdI, 0,										// initialize decimal count with 0
	instrCmpXtoY, 0x32,									// compare register 2 contents to initial window value
	instrBranchIfGT, 18,								// if larger or equal, there are no decimal digits to output
	instrAddIndex, 1,									// bump decimal count up by 1
	instrDiv2byByte, 10,								// shift window right 1 digit (should be 0.1 equivalent)
	instrCmpXtoY, 0x32,									// compare register 2 contents to initial window value
	instrBranchIfGT, 10,								// if register 2 contents are not less than window, go save decimal count
	instrAddIndex, 1,									// bump decimal count up by 1 
	instrDiv2byByte, 10,								// shift window right 1 digit (should be 0.01 equivalent)
	instrCmpXtoY, 0x32,									// compare register 2 contents to initial window value
	instrBranchIfGT, 2,									// if register 2 contents are not less than window, go save decimal count
	instrAddIndex, 1,									// bump decimal count up by 1

//storeDecCnt:
	instrLdRegByteFromIndex, 0x02,						// save obtained decimal count
	instrSwapReg, 0x32,									// swap register 2 contents with obtained decimal count
	instrDone											// exit to caller
};

// converts the 64-digit number stored in SWEET64 register 2 into an up-to 10-digit decimal number string
//
// this routine is called to perform initial conversion into a 10 character long string containing
//    the up-to 10 digit number with inserted decimal point as appropriate, and leading spaces
//
// this routine will autorange the number being converted to automatically support the number of decimal digits being output
//    unless disabled
//
// if no window length is specified, this routine just removes all leading spaces
//
// decimalFlag currently has two bits defined:
// 1xxx xxxx - fill overflow string from all 9s instead of all '-' characters
// x1xx xxxx - ignore decimal point character in window length consideration (used in useBigNumberDisplay)
//
// sample debug monitor outputs:
//
// ]0<6.2u (overflow='-', do not ignore decimal point)        ]c0<6.2u (overflow='9', ignore decimal point) 
// 00: 00 06 02                                               00: c0 06 02
//     0000000000000005 -   0.01 -                                0000000000000005 -    0.01 -
// 01: 0000000000000037 -   0.06 -                            01: 0000000000000037 -    0.06 -
// 02: 000000000000022B -   0.56 -                            02: 000000000000022B -    0.56 -
// 03: 00000000000015B3 -   5.56 -                            03: 00000000000015B3 -    5.56 -
// 04: 000000000000D903 -  55.56 -                            04: 000000000000D903 -   55.56 -
// 05: 0000000000087A23 - 555.56 -                            05: 0000000000087A23 -  555.56 -
// 06: 000000000054C563 - 5555.6 -                            06: 000000000054C563 - 5555.56 -
// 07: 00000000034FB5E3 -  55556 -                            07: 00000000034FB5E3 - 55555.6 -
// 08: 00000000211D1AE3 - 555556 -                            08: 00000000211D1AE3 - 555556 -
// 09: 000000014B230CE3 - ------ -                            09: 000000014B230CE3 - 999999 -
// 0A: 0000000CEF5E80E3 - ------ -                            0A: 0000000CEF5E80E3 - 999999 -
// 0B: 0000008159B108E3 - ------ -                            0B: 0000008159B108E3 - 999999 -
// 0C: 0000050D80EA58E3 - ------ -                            0C: 0000050D80EA58E3 - 999999 -
// 0D: 00003287092778E3 - ------ -                            0D: 00003287092778E3 - 999999 -
//
static char * ull2str(char * strBuffer, uint8_t decimalPlaces, uint8_t windowLength, uint8_t decimalFlag) // format number for output
{

	union union_64 * tmpPtr3 = (union union_64 *)(&s64reg[s64reg3]);

	uint8_t d;
	uint8_t e;
	uint8_t f;

	if (decimalPlaces > 3) decimalPlaces = 0; // if too many decimal digits specified, reset to 0

	if ((windowLength > 10) || (windowLength < 2)) windowLength = 0; // validity check for window length
	else if (decimalPlaces > (windowLength - 1)) decimalPlaces = windowLength - 1; // ensure complete number fits window

	f = ((decimalFlag & dfIgnoreDecimalPoint) ? 0 : 1); // shrink window if decimal point is considered

	SWEET64::runPrgm(prgmAutoRangeNumber, windowLength - f); // fetch supportable decimal digit count for window
	d = tmpPtr3->u8[0];

	if (decimalPlaces > d) decimalPlaces = d; // if supportable digit count is less than specified, use supportable instead

	ull2str(strBuffer, decimalPlaces, tRoundOffNumber); // perform rounding of number to nearest decimal place, then format for ASCII output and insert a decimal point

	if (strBuffer[2] == '-') f = 1; // if number overflowed
	else
	{

		f = 0; // initially signal no overflow occurred
		d = 0;

		if (windowLength) // if there is a valid windowLength
		{

			e = ((decimalPlaces) ? strBuffer[12] - decimalPlaces: strBuffer[11]); // get whole digit count
			d = 9 - decimalPlaces; // compute position of decimal 1s position

			if (windowLength == e) // if window length == whole digit count
			{

				strBuffer[(uint16_t)(++d)] = 0; // replace the decimal point with a null
				d -= windowLength; // set up for left adjust string copy

			}
			else if (windowLength < e) // if window length < whole digit count
			{

				f = 1; // signal that number overflowed
				d = 0; // signal 'do not perform string copy'

			}
			else // window length > whole digit count
			{

				d -= (e - 1); // point to largest whole digit stored

				// adjust window length if instructed to ignore the decimal point for length consideration
				f = windowLength;
				if ((decimalFlag & dfIgnoreDecimalPoint) && (decimalPlaces)) f++;

				// find the string end position minus adjusted window length
				e = ((decimalPlaces) ? 11 : 10) - f;

				// if string end position minus window length is less than the position for the largest whole digit, use it instead
				if (e < d) d = e;

				// catch the special case where the string has decimal point but no decimals
				if (strBuffer[(uint16_t)(d + f - 1)] == '.') d--;

				strBuffer[(uint16_t)(d + f)] = 0; // mark end of string

				f = 0;

			}

		}
		else // no windowLength specified, just eliminate leading spaces
		{

			d = 0;

			while (strBuffer[(uint16_t)(d)] == ' ' && (d < 11)) d++; // find position of first non-blank character

		}

		if (d) // move string buffer left to remove extra spaces, and to move number into window if required
		{

			e = 0;

			while (d < 12) strBuffer[(uint16_t)(e++)] = strBuffer[(uint16_t)(d++)];

		}

	}

	if (f) // if an overflow occurred
	{

		strcpy_P(strBuffer, ((decimalFlag & dfOverflow9s) ? overFlow9Str : overFlowStr)); // copy overflow string into buffer

		if (windowLength) strBuffer[(unsigned int)(windowLength)] = 0; // mark new end of string buffer

	}

	return strBuffer;

}

