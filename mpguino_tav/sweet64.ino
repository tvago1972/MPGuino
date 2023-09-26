/*

 SWEET64 is a low-level pseudocode interpreter, meant to save a lot of space in program memory for 64-bit calculations

 the opcode definitions below provide means of manipulating MPGuino trip variables, interacting with calculated and EEPROM parameters, and providing basic
 number formatting for interaction with string representations of numbers

 SWEET64 is inspired by an early pseudocode processor named SWEET16, which was written by Steve Wozniak of Apple Computer back in 1977. SWEET16 was written to
    perform 16-bit machine language instructions while running on an 8-bit microprocessor

*/

static uint32_t SWEET64::doCalculate(uint8_t tripIdx, uint8_t calcIdx)
{

	return runPrgm((const uint8_t *)(pgm_read_word(&S64programList[(unsigned int)(calcIdx)])), tripIdx);

}

static uint32_t SWEET64::runPrgm(const uint8_t * sched, uint8_t tripIdx)
{

	uint32_t instrLWord;
	union union_32 * iLW = (union union_32 *)(&instrLWord);

#if defined(useDebugCPUreading)
	activity::record(arSWEET64, 0);

#endif // defined(useDebugCPUreading)
	s64reg8[(uint16_t)(si64reg8trip)] = tripIdx; // store user-provided trip index value
	s64reg8[(uint16_t)(si64reg8flags)] = 0; // initialize processor flags
	s64reg8[(uint16_t)(si64reg8spnt)] = 0; // initialize stack pointer
	s64reg8[(uint16_t)(si64reg8jump)] = 0; // initialize jump index register

	do
	{

		fetchInstruction(iLW, sched, s64reg8); // decode instruction

		if (s64reg8[(uint16_t)(si64reg8valid)]) executeInstruction(iLW, sched, s64stack, s64reg, s64reg8); // execute instruction
#if defined(useDebugTerminal)
		else terminal::dumpSWEET64information(iLW, sched, s64stack, s64reg, s64reg8); // invalid instruction encountered, output relevant information
#endif // defined(useDebugTerminal)

	}
	while (s64reg8[(uint16_t)(si64reg8valid)]);

#if defined(useDebugCPUreading)
	activity::record(0, arSWEET64);

#endif // defined(useDebugCPUreading)
	return ((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]))->ul[0];

}

static void SWEET64::fetchInstruction(union union_32 * instrLWord, const uint8_t * &prgmPtr, uint8_t * prgmReg8)
{

	uint8_t isValid;
	uint8_t reg;

	if (prgmPtr) instrLWord->u8[0] = pgm_read_byte(prgmPtr++); // read opcode byte

	if (instrLWord->u8[0] < maxValidSWEET64instr) // if opcode byte is valid
	{

		isValid = s64vRegisterOperation; // initially assume this instruction deals with 64-bit registers

		instrLWord->ui[0] = pgm_read_word(&opcodeFetchWord[(uint16_t)(instrLWord->u8[0])]); // read expanded opcode word
		instrLWord->ui[1] = 0; // reset 64-bit register byte definitions

		switch (instrLWord->u8[1] & rxxMask) // categorize opcode according to how it accesses the 64 bit registers
		{

			case r00:	// do not fetch register operand
				isValid &= ~(s64vRegisterOperation); // this instruction does not deal with 64-bit registers
				isValid |= (s64vReadInRegisterByte); // must ensure that isValid remains non-zero ("register byte read in" is nonsensical with no "register operation" flag)
				if (instrLWord->u8[0] < eMaxBranchInstrIdx) isValid |= (s64vRelativeOperand);
				break;

			case r01:	// fetch rX and rY from program
				isValid |= (s64vReadInRegisterByte);

				if (prgmPtr)
				{

					reg = pgm_read_byte(prgmPtr++);

					if (((reg & 0x70) || (reg & 0x07)) && ((reg & 0x88) == 0)) s64operands[(uint16_t)(s64oprRegXY)] = reg; // ensure valid 64-bit register reference
					else isValid = 0;

				}
				break;

			case r02:	// set rX = r5
				s64operands[(uint16_t)(s64oprRegXY)] = 0x05;	// signal to move external contents to arithmetic register 5
				break;

			case r03:	// set rX = r5, rY = r1
				s64operands[(uint16_t)(s64oprRegXY)] = 0x15;	// signal to move register 1 contents to arithmetic register 5
				break;

			case r04:	// fetch rP and rS from program
				isValid |= (s64vReadInRegisterByte);

				if (prgmPtr)
				{

					reg = pgm_read_byte(prgmPtr++);

					if (((reg & 0x70) || (reg & 0x07)) && ((reg & 0x88) == 0)) // ensure valid 64-bit register reference
					{

						s64operands[(uint16_t)(s64oprRegRS)] = reg;
						s64operands[(uint16_t)(s64oprRegXY)] = reg;

					}
					else isValid = 0;

				}
				break;

			case r05:	// set rP = r5, rS = r2
				s64operands[(uint16_t)(s64oprRegRS)] = 0x52;	// signal to add half remainder flag to quotient
				break;

			case r06:	// fetch rP from program, set rS = r5, and rX to r5
				isValid |= (s64vReadInRegisterByte);

				if (prgmPtr)
				{

					reg = pgm_read_byte(prgmPtr++);

					if ((reg & 0x07) && ((reg & 0xF8) == 0)) // ensure valid 64-bit register reference
					{

						s64operands[(uint16_t)(s64oprRegRS)] = reg | 0x50; // set rS to r5
						s64operands[(uint16_t)(s64oprRegXY)] = 0x05;

					}
					else isValid = 0;

				}
				break;

			case r07:	// fetch rX and rY from program, shift rY to rX if in metric mode, or throw rY away if in SAE mode
				isValid |= (s64vReadInRegisterByte);

				if (prgmPtr)
				{

					reg = pgm_read_byte(prgmPtr++);

					if ((reg & 0x77) && ((reg & 0x88) == 0)) // ensure valid 64-bit register reference
					{


						if (mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayMetric) reg >>= 4; // if in metric mode, shift rY into rX
						else reg &= 0x0F; // otherwise, throw rY away and keep rX

						s64operands[(uint16_t)(s64oprRegXY)] = reg;

					}
					else isValid = 0;

				}
				break;

			default:	// invalid rxx code detected, exit program
				isValid = 0;
				break;

		}

		if (isValid)
		{

			if (isValid & s64vRegisterOperation)
			{
				instrLWord->u8[2] = instrLWord->u8[0] & ixxMask;
				instrLWord->u8[3] = instrLWord->u8[0] & mxxMask;
			}

			s64operands[(uint16_t)(s64oprPrimary)] = 0;

			switch (instrLWord->u8[1] & pxxMask) // pull primary operand to work with
			{

				case p00:	// do not fetch primary operand
					break;

				case p03:	// load primary operand with program + index
					isValid |= (s64vOperandIndexed);
				case p01:	// load primary operand from program
					isValid |= (s64vReadInOperandByte);

					if (prgmPtr) s64operands[(uint16_t)(s64oprPrimary)] += pgm_read_byte(prgmPtr++);
					break;

				case p02:	// load primary operand from index
					isValid |= (s64vOperandIndexed);
					break;

				default:	// invalid pxx code detected, exit program
					isValid = 0;
					break;

			}

		}

		if (isValid)
		{

			s64operands[(uint16_t)(s64oprExtra)] = 0;

			switch (instrLWord->u8[1] & sxxMask) // pull extra operand to work with
			{

				case s00:	// do not fetch secondary operand
					break;

				case s01:	// fetch secondary operand from program
					isValid |= (s64vReadInExtraByte);
					if (prgmPtr) s64operands[(uint16_t)(s64oprExtra)] += pgm_read_byte(prgmPtr++);
					break;

				case s02:	// fetch secondary operand from index
					isValid |= (s64vExtraIndexed);
					break;

				case s03:	// fetch secondary operand from jump register
					isValid |= (s64vExtraJump);
					break;

				default:	// invalid sxx code detected, exit program
					isValid = 0;
					break;

			}

		}

	}
	else isValid = 0; // otherwise, opcode byte is not valid

	prgmReg8[(uint16_t)(si64reg8valid)] = isValid;

}

static void SWEET64::executeInstruction(union union_32 * instrLWord, const uint8_t * &prgmPtr, const uint8_t * prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8)
{

	uint8_t oldSREG;
	const uint8_t * s64BCDptr;
	uint8_t isValid;
	uint8_t instr;
	uint8_t operand;
	uint8_t extra;
	uint8_t branchFlag;

	union union_64 * regX;
	union union_64 * regY;
	union union_64 * regP;
	union union_64 * regS;

	SWEET64processorFlags = prgmReg8[(uint16_t)(si64reg8flags)];
	isValid = prgmReg8[(uint16_t)(si64reg8valid)];

#if defined(useDebugTerminalSWEET64)
	operand = (SWEET64processorFlags & SWEET64traceFlagGroup);
	if (operand == SWEET64traceFlag) SWEET64processorFlags &= ~(SWEET64traceFlag);
	if (operand == SWEET64traceCommandFlag) SWEET64processorFlags |= (SWEET64traceFlag);

#endif // defined(useDebugTerminalSWEET64)
	operand = s64operands[(uint16_t)(s64oprPrimary)];
	extra = s64operands[(uint16_t)(s64oprExtra)];

	if (isValid & s64vOperandIndexed) operand += prgmReg8[(uint16_t)(si64reg8trip)];
	if (isValid & s64vExtraIndexed) extra += prgmReg8[(uint16_t)(si64reg8trip)];
	if (isValid & s64vExtraJump) extra += prgmReg8[(uint16_t)(si64reg8jump)];

	if (isValid & s64vRegisterOperation) // instruction does something with the 64 bit registers
	{

		if (s64operands[(uint16_t)(s64oprRegXY)] & 0x07) regX = (union union_64 *)(&prgmReg64[(uint16_t)((s64operands[(uint16_t)(s64oprRegXY)] & 0x07) - 1)]);
		if (s64operands[(uint16_t)(s64oprRegXY)] & 0x70) regY = (union union_64 *)(&prgmReg64[(uint16_t)(((s64operands[(uint16_t)(s64oprRegXY)] & 0x70) >> 4) - 1)]);
		if (s64operands[(uint16_t)(s64oprRegRS)] & 0x07) regP = (union union_64 *)(&prgmReg64[(uint16_t)((s64operands[(uint16_t)(s64oprRegRS)] & 0x07) - 1)]);
		if (s64operands[(uint16_t)(s64oprRegRS)] & 0x70) regS = (union union_64 *)(&prgmReg64[(uint16_t)(((s64operands[(uint16_t)(s64oprRegRS)] & 0x70) >> 4) - 1)]);

		branchFlag = false;

		// perform supplemental opcode parsing, and set up to disable interrupts according to instruction and operand index value
		switch (instrLWord->u8[2])
		{

			case i18:	// load rX with trip variable
			case i19:	// store trip variable rX
				if (operand < instantIdx) branchFlag = true; // if accessing raw trip variables, disable interrupts
				break;

			case i14:	// load rX with const
				switch (operand)
				{

					case (idxConstantStart) ... (idxConstantEnd - 1):
						extra = idxConstantStart;
						break;

					case (pSettingsIdxStart) ... (pSettingsIdxEnd - 1):
						extra = pSettingsIdxStart;
						break;

					default:
						extra = 0;
						break;

				}

				operand -= extra;
				break;

			case i07:	// load rX with program variable
			case i08:	// store program variable rX
				switch (operand)
				{

					case (v8VariableStartIdx) ... (v8VariableEndIdx - 1):
						extra = v8VariableStartIdx;
						branchFlag = true;
						break;

					case (m8VariableStartIdx) ... (m8VariableEndIdx - 1):
						extra = m8VariableStartIdx;
						break;

					case (v16VariableStartIdx) ... (v16VariableEndIdx - 1):
						extra = v16VariableStartIdx;
						branchFlag = true;
						break;

					case (v32VariableStartIdx) ... (v32VariableEndIdx - 1):
						extra = v32VariableStartIdx;
						branchFlag = true;
						break;

					case (m32VariableStartIdx) ... (m32VariableEndIdx - 1):
						extra = m32VariableStartIdx;
						break;

					case (m64VariableStartIdx) ... (m64VariableEndIdx - 1):
						extra = m64VariableStartIdx;
						break;

					default:
						extra = 255;
						break;

				}

				operand -= extra;
			default:
				break;

#if defined(useBarFuelEconVsTime)
			case i17:	// load rX with FEvT trip variable
				branchFlag = true;
				break;

#endif // defined(useBarFuelEconVsTime)
		}

		if (branchFlag)
		{

			oldSREG = SREG; // save interrupt flag status
			cli(); // disable interrupts

		}

		switch (instrLWord->u8[2]) // perform load or store operation, according to ixx
		{

			case i00:	// do nothing
				break;

			case i01:	// load rX with rY
				copy64(regX, regY);
				break;

			case i02:	// swap rX rY
				swap64(regX, regY);
				break;

			case i03:	// load rX with EEPROM
				EEPROM::read64(regX, operand);
				break;

			case i04:	// store EEPROM rX
				EEPROM::write64(regX, operand);
				break;

			case i07:	// load rX with program variable
				switch (extra)
				{

					case v8VariableStartIdx:
						init64byt(regX, volatile8Variables[(uint16_t)(operand)]);
						break;

					case m8VariableStartIdx:
						init64byt(regX, mainProgram8Variables[(uint16_t)(operand)]);
						break;

					case v16VariableStartIdx:
						init64(regX, volatile16Variables[(uint16_t)(operand)]);
						break;

					case v32VariableStartIdx:
						init64(regX, volatile32Variables[(uint16_t)(operand)]);
						break;

					case m32VariableStartIdx:
						init64(regX, mainProgram32Variables[(uint16_t)(operand)]);
						break;

					case m64VariableStartIdx:
						copy64(regX, (union union_64 *)(&mainProgram64Variables[(uint16_t)(operand)]));
						break;

					default:
						break;

				}
				break;

			case i08:	// store program variable rX
				switch (extra)
				{

					case v8VariableStartIdx:
						volatile8Variables[(uint16_t)(operand)] = regX->u8[0];
						break;

					case m8VariableStartIdx:
						mainProgram8Variables[(uint16_t)(operand)] = regX->u8[0];
						break;

					case v16VariableStartIdx:
						volatile16Variables[(uint16_t)(operand)] = regX->ui[0];
						break;

					case v32VariableStartIdx:
						volatile32Variables[(uint16_t)(operand)] = regX->ul[0];
						break;

					case m32VariableStartIdx:
						mainProgram32Variables[(uint16_t)(operand)] = regX->ul[0];
						break;

					case m64VariableStartIdx:
						copy64((union union_64 *)(&mainProgram64Variables[(uint16_t)(operand)]), regX);
						break;

					default:
						break;

				}
				break;

			case i10:	// load rX with byte
				init64byt(regX, operand);
				break;

			case i14:	// load rX with const
				switch (extra)
				{

					case idxConstantStart:
						init64(regX, pgm_read_dword(&constantNumberList[(uint16_t)(operand)]));
						break;

					case pSettingsIdxStart:
						init64(regX, pgm_read_dword(&params[(uint16_t)(operand)]));
						break;

					default:
						break;

				}
				break;

#if defined(useBarFuelEconVsTime)
			case i17:	// load rX with FEvT trip variable
				instr = FEvTperiodIdx; // get current fuel econ vs time trip variable
				instr -= FEvsTimeIdx; // translate out of trip index space

				if (operand >= bgDataSize) operand = 0; // shift index
				else operand++;

				operand += instr; // add to trip variable value
				if (operand >= bgDataSize) operand -= bgDataSize; // perform wrap-around if required

				operand += FEvsTimeIdx; // shift back into trip index space
#endif // defined(useBarFuelEconVsTime)
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

					instr = tripVar::getBaseEEPROMaddress(operand, extra);
					if (instr) EEPROM::read64(regX, instr);
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

					instr = tripVar::getBaseEEPROMaddress(operand, extra);
					if (instr) EEPROM::write64(regX, instr);

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
#if defined(useIsqrt)
			case i28:	// integer square root
#if defined(useDebugCPUreading)
				mainProgram32Variables[(uint16_t)(m32DbgWorkingMathStartIdx - m32VariableStartIdx)] = heart::cycles0();

#endif // defined(useDebugCPUreading)
				regX->ul[0] = iSqrt(regX->ul[0]);
#if defined(useDebugCPUreading)

				mainProgram32Variables[(uint16_t)(m32DebugAccS64sqrtIdx - m32VariableStartIdx)] += heart::findCycle0Length(m32DbgWorkingMathStartIdx - m32VariableStartIdx);
				mainProgram32Variables[(uint16_t)(m32DebugCountS64sqrtIdx - m32VariableStartIdx)]++;

#endif // defined(useDebugCPUreading)
				break;

#endif // defined(useIsqrt)
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
				isValid = 0;
				break;

		}

		if (branchFlag) SREG = oldSREG; // restore interrupt flag status

		switch (instrLWord->u8[3]) // perform basic arithmetic operation, given mxx
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
#if defined(useDebugCPUreading)
				mainProgram32Variables[(uint16_t)(m32DbgWorkingMathStartIdx - m32VariableStartIdx)] = heart::cycles0();

#endif // defined(useDebugCPUreading)
				mult64(prgmReg64);
#if defined(useDebugCPUreading)

				mainProgram32Variables[(uint16_t)(m32DebugAccS64multIdx - m32VariableStartIdx)] += heart::findCycle0Length(m32DbgWorkingMathStartIdx - m32VariableStartIdx);
				mainProgram32Variables[(uint16_t)(m32DebugCountS64multIdx - m32VariableStartIdx)]++;

#endif // defined(useDebugCPUreading)
				break;

			case m06:	// divide		r2 = r2 / r5 rmdr r1 and qadj r5
#if defined(useDebugCPUreading)
				mainProgram32Variables[(uint16_t)(m32DbgWorkingMathStartIdx - m32VariableStartIdx)] = heart::cycles0();

#endif // defined(useDebugCPUreading)
				div64(prgmReg64);
#if defined(useDebugCPUreading)

				mainProgram32Variables[(uint16_t)(m32DebugAccS64divIdx - m32VariableStartIdx)] += heart::findCycle0Length(m32DbgWorkingMathStartIdx - m32VariableStartIdx);
				mainProgram32Variables[(uint16_t)(m32DebugCountS64divIdx - m32VariableStartIdx)]++;

#endif // defined(useDebugCPUreading)
				break;

			default:	// invalid mxx opcode detected, exit program
				isValid = 0;
				break;

		}

	}
	else // instruction does nothing at all with the 64-bit registers
	{

		if (isValid & s64vRelativeOperand) // instruction is a conditional branching instruction
		{

			switch (instrLWord->u8[0])
			{

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
					branchFlag = (mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayMetric);
					break;

				case e12:	// instrBranchIfSAEmode
					branchFlag = ((mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayMetric) == 0);
					break;

				case e13:	// instrBranchIfFuelOverDist (L/100km or G/100mi)
					branchFlag = (mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayAlternateFE);
					break;

				case e14:	// instrBranchIfDistOverFuel (MPG or KPL)
					branchFlag = ((mainProgram8Variables[(uint16_t)(m8MetricModeFlags - m8VariableStartIdx)] & mmDisplayAlternateFE) == 0);
					break;

				case e15:	// instrSkip
					branchFlag = true;
					break;

				default:
					branchFlag = false;
					break;

			}

			if (branchFlag)
			{

				if (extra < 128) prgmPtr += extra;
				else prgmPtr -= (256 - extra);

			}

		}
		else // instruction is not a conditional branching instruction
		{

			switch (instrLWord->u8[0])
			{

				case e18:	// instrTestIndex
					break;

				case e17:	// instrTraceDone
#if defined(useDebugTerminalSWEET64)
					if (SWEET64processorFlags & SWEET64traceSaveFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
					else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // defined(useDebugTerminalSWEET64)
				case e16:	// instrDone
					if (prgmReg8[(uint16_t)(si64reg8spnt)]--) prgmPtr = prgmStack[(unsigned int)(prgmReg8[(uint16_t)(si64reg8spnt)])];
					else isValid = 0;
					break;

				case e19:	// instrTraceRestore
#if defined(useDebugTerminalSWEET64)
					if (SWEET64processorFlags & SWEET64traceSaveFlag) SWEET64processorFlags |= (SWEET64traceCommandFlag);
					else SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
					break;

#endif // defined(useDebugTerminalSWEET64)
				case e20:	// instrTraceOn
#if defined(useDebugTerminalSWEET64)
					SWEET64processorFlags |= (SWEET64traceCommandFlag | SWEET64traceSaveFlag);
					break;

#endif // defined(useDebugTerminalSWEET64)
				case e22:	// instrTraceOff
#if defined(useDebugTerminalSWEET64)
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag | SWEET64traceSaveFlag);
					break;

#endif // defined(useDebugTerminalSWEET64)
				case e21:	// instrTraceSave
#if defined(useDebugTerminalSWEET64)
					SWEET64processorFlags &= ~(SWEET64traceCommandFlag);
#endif // defined(useDebugTerminalSWEET64)
					break;

				case e23:	// load index
					prgmReg8[(uint16_t)(si64reg8trip)] = operand;
					break;

				case e24:	// load index EEPROM
					prgmReg8[(uint16_t)(si64reg8trip)] = EEPROM::readByte(operand);
					break;

				case e25:	// compare index
					if (operand < prgmReg8[(uint16_t)(si64reg8trip)]) SWEET64processorFlags |= (SWEET64carryFlag);
					else SWEET64processorFlags &= ~(SWEET64carryFlag);

					if (operand == prgmReg8[(uint16_t)(si64reg8trip)]) SWEET64processorFlags |=(SWEET64zeroFlag);
					else SWEET64processorFlags &= ~(SWEET64zeroFlag);

					if ((operand - prgmReg8[(uint16_t)(si64reg8trip)]) & 0x80) SWEET64processorFlags |=(SWEET64minusFlag);
					else SWEET64processorFlags &= ~(SWEET64minusFlag);

					break;

				case e26:	// load index EEPROM parameter length
					prgmReg8[(uint16_t)(si64reg8trip)] = EEPROM::getLength(operand);
					break;

				case e27:	// call
					prgmStack[(unsigned int)(prgmReg8[(uint16_t)(si64reg8spnt)]++)] = prgmPtr;
					if (prgmReg8[(uint16_t)(si64reg8spnt)] > 15) isValid = 0;
				case e28:	// jump
					prgmPtr = (const uint8_t *)pgm_read_word(&S64programList[(unsigned int)(extra)]);
					break;

				case e29:	// load jump register
					prgmReg8[(uint16_t)(si64reg8jump)] = operand;
					break;

				case e30:	// clear register flag
				case e31:	// set register flag
					flagSet((instrLWord->u8[0] == e31), operand);
					break;

				default:	// invalid sxx code detected, exit program
					isValid = 0;
					break;

			}

			switch (instrLWord->u8[0]) // this is to allow multiple instructions to test the index register
			{

				case e23:	// load index
				case e24:	// load index EEPROM
				case e26:	// load index EEPROM parameter length
				case e18:	// instrTestIndex
					if (prgmReg8[(uint16_t)(si64reg8trip)]) SWEET64processorFlags &= ~(SWEET64zeroFlag);
					else SWEET64processorFlags |= (SWEET64zeroFlag);

					if (prgmReg8[(uint16_t)(si64reg8trip)] & 0x80) SWEET64processorFlags |= (SWEET64minusFlag);
					else SWEET64processorFlags &= ~(SWEET64minusFlag);

					break;

				default:	// ignore the code
					break;

			}

		}

	}

	prgmReg8[(uint16_t)(si64reg8valid)] = isValid;
	prgmReg8[(uint16_t)(si64reg8flags)] = SWEET64processorFlags;

}

static void SWEET64::copy64(union union_64 * an, union union_64 * ann) // an = ann
{

#if defined(useAssemblyLanguage)
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
#else // defined(useAssemblyLanguage)
	for (uint8_t x = 0; x < 4; x++) an->ui[(unsigned int)(x)] = ann->ui[(unsigned int)(x)];
#endif // defined(useAssemblyLanguage)

}

static void SWEET64::swap64(union union_64 * an, union union_64 * ann) // swap ann and an
{

	uint16_t aing;
	uint8_t x;

#if defined(useAssemblyLanguage)
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
#else // defined(useAssemblyLanguage)
	for (x = 0; x < 4; x++)
	{

		aing = ann->ui[(unsigned int)(x)];
		ann->ui[(unsigned int)(x)] = an->ui[(unsigned int)(x)];
		an->ui[(unsigned int)(x)] = aing;

	}
#endif // defined(useAssemblyLanguage)

}

static void SWEET64::shr64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#if defined(useAssemblyLanguage)

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
#else // defined(useAssemblyLanguage)
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

#endif // defined(useAssemblyLanguage)
	m = an->u8[7];

	flagSet64(m, z, c);

}

static void SWEET64::shl64(union union_64 * an)
{

	uint8_t c;
	uint8_t z;
	uint8_t m;
	uint8_t x;
#if defined(useAssemblyLanguage)

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
#else // defined(useAssemblyLanguage)
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
#endif // defined(useAssemblyLanguage)

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
#if defined(useAssemblyLanguage)

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
#else // defined(useAssemblyLanguage)
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
#endif // defined(useAssemblyLanguage)

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
#if defined(useAssemblyLanguage)

	asm volatile (
		"	clc						\n"
		"	clr	%A2					\n"		// initialize zero flag
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
		"	ld	%A1, %a0+			\n"		// there is apparently no way to directly increment %a0
		"	dec	%A4					\n"
		"	brne l_sub64%=			\n"

		"	mov	%A1, __zero_reg__	\n"		// initialize carry flag
		"	adc	%A1, __zero_reg__	\n"		// save carry flag
		: "+e" (an), "+d" (c), "+d" (z), "+d" (m), "+d" (x)
		: "e" (ann), "d" (sbcFlag)
	);
#else // defined(useAssemblyLanguage)
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
#endif // defined(useAssemblyLanguage)

	flagSet64(m, z, c);

}

static void SWEET64::registerTest64(union union_64 * an)
{

	uint8_t z;
	uint8_t m;
	uint8_t v;
	uint8_t x;

#if defined(useAssemblyLanguage)
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
#else // defined(useAssemblyLanguage)
	z = 0;
	v = 0;

	for (x = 0; x < 8; x++)
	{

		m = an->u8[(uint16_t)(x)];
		z |= m;
		if (m != 0xFF) v = 1;

	}
#endif // defined(useAssemblyLanguage)

	flagSet((m & 0x80), SWEET64minusFlag);
	flagSet((z == 0), SWEET64zeroFlag);
	flagSet((v == 0), SWEET64overflowFlag);

}

static void SWEET64::mult64(uint64_t * prgmReg64)
{

	union union_64 * an = (union union_64 *)(&prgmReg64[(uint16_t)(s64reg64_2)]);	// multiplier in an, result to an
	union union_64 * multiplicand = (union union_64 *)(&prgmReg64[(uint16_t)(s64reg64_5)]);	// define multiplicand term as register 5
#if defined(useAssemblyLanguage)

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

#else // defined(useAssemblyLanguage)
	union union_64 * multiplier = (union union_64 *)(&prgmReg64[(uint16_t)(s64reg64_4)]);	// define multiplier term as register 4

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

#endif // defined(useAssemblyLanguage)
}

static void SWEET64::div64(uint64_t * prgmReg64) // uses algorithm for non-restoring hardware division
{

	union union_64 * ann = (union union_64 *)(&prgmReg64[(uint16_t)(s64reg64_1)]);	// remainder in ann
	union union_64 * divisor = (union union_64 *)(&prgmReg64[(uint16_t)(s64reg64_5)]);
#if defined(useAssemblyLanguage)
	uint8_t v; // overflow result

	asm volatile(
		"	push	r30				\n"		// save Z[0] reg original contents
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
		"	clr	r27					\n"		// no, I do not trust __zero_reg__

		"	ldi	r26, 16				\n"		// point X reg to divisor register
		"d64_cpd%=:					\n"
		"	ld	r24, z				\n"		// load divisor byte value
		"	or	r25, r24			\n"		// also perform check if divisor is zero
		"	st	x+, r24				\n"		// copy divisor byte value into register space
		"	st	z+, r27				\n"		// store a zero in divisor variable
		"	cpi	r26, 24				\n"
		"	brne	d64_cpd%=		\n"

		"	pop	r27					\n"		// restore X reg original contents
		"	pop	r26					\n"

		"	tst	r25					\n"		// if divisor is not zero, continue ahead
		"	brne	d64_cont1%=		\n"

		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"

		"	com	r25					\n"		// generate overflow value
		"	ldi	r24, 16				\n"		// set remainder contents as overflow value

		"d64_ovfl%=:				\n"
		"	st	x+, r25				\n"		// store overflow value into remainder and quotient
		"	dec	r24					\n"
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

		"	pop	r27					\n"		// otherwise, div64 is done - restore X reg original contents
		"	pop	r26					\n"
		"	pop	r31					\n"		// restore Z reg original contents
		"	pop	r30					\n"
		"	rjmp	d64_cont4%=		\n"

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
		"	tst	r7					\n"		// is result < 0?
		"	brmi	d64_l1c%=		\n"		// if so, skip ahead to check how many bits are left to process
		"	inc	r8					\n"
		"d64_l1c%=:					\n"
		"	dec	r24					\n"		// done processing bits?
		"	brne	d64_loop1%=		\n"		// if not, loop back

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

		"	cp	r0, r16				\n"		// compare remainder to divisor / 2
		"	cpc	r1, r17				\n"
		"	cpc	r2, r18				\n"
		"	cpc	r3, r19				\n"
		"	cpc	r4, r20				\n"
		"	cpc	r5, r21				\n"
		"	cpc	r6, r22				\n"
		"	cpc	r7, r23				\n"
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
		"	mov	r30, r25			\n"		// save overflow flag result in Z[0] reg
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
		"	mov %A0, r30			\n"		// save overflow result
		"	pop	r30					\n"		// restore Z[0] reg original contents
		: "=r" (v)
		: "x" (ann), "z" (divisor)
	);

	flagSet(v, SWEET64overflowFlag);

#else // defined(useAssemblyLanguage)
	union union_64 * an = (union union_64 *)(&prgmReg64[(uint16_t)(s64reg64_2)]);	// quotient in an

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
		SWEET64processorFlags |= (SWEET64overflowFlag); // signal that overflow occurred

	}
	else
	{

		SWEET64processorFlags &= ~(SWEET64overflowFlag); // clear overflow flag

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

#endif // defined(useAssemblyLanguage)
}

#if defined(useIsqrt)
// 32-bit integer square root based on digit-by-digit method
//
// original algorithm developed by Andrija Radovic found at https://www.andrijar.com/algorithms/algorithms.htm#qusr
//
// this implementation does not rely at all on multiplies or divides
//
static uint32_t iSqrt(uint32_t input)
{

	uint32_t output; // square root, running (2 * x * dx) term
#if defined(useAssemblyLanguage)

	asm volatile(
		"	push	r4				\n"		// save original contents for test reg
		"	push	r5				\n"
		"	push	r6				\n"
		"	push	r7				\n"
		"	push	r8				\n"		// save original contents for X2 reg
		"	push	r9				\n"
		"	push	r10				\n"
		"	push	r11				\n"
		"	push	r12				\n"		// save original contents for DX2 reg
		"	push	r13				\n"
		"	push	r14				\n"
		"	push	r28				\n"

		"	clr		%A0				\n"		// zero out output reg
		"	clr		%B0				\n"
		"	clr		%C0				\n"
		"	clr		%D0				\n"
		"	clr		r8				\n"		// zero out X2 reg
		"	clr		r9				\n"
		"	clr		r10				\n"
		"	clr		r11				\n"
		"	clr		r12				\n"		// initialize DX2 reg with 1073741824
		"	clr		r13				\n"
		"	clr		r14				\n"
		"	ldi		r28, 0x80		\n"

		"sq64_mloop%=:				\n"
		"	lsr		r28				\n"		// divide DX2 reg by 2
		"	ror		r14				\n"
		"	ror		r13				\n"
		"	ror		r12				\n"
		"	mov		r4, r8			\n"		// initialize test reg with X2 reg
		"	mov		r5, r9			\n"
		"	mov		r6, r10			\n"
		"	mov		r7, r11			\n"
		"	add		r4, %A0			\n"		// add 2 * X * DX reg to test reg
		"	adc		r5, %B0			\n"
		"	adc		r6, %C0			\n"
		"	adc		r7, %D0			\n"
		"	add		r4, r12			\n"		// add DX2 reg to test reg
		"	adc		r5, r13			\n"
		"	adc		r6, r14			\n"
		"	adc		r7, r28			\n"
		"	lsr		%D0				\n"		// divide output reg by 2
		"	ror		%C0				\n"
		"	ror		%B0				\n"
		"	ror		%A0				\n"

		"	cp		%A1, r4			\n"		// compare test reg to input reg
		"	cpc		%B1, r5			\n"
		"	cpc		%C1, r6			\n"
		"	cpc		%D1, r7			\n"
		"	brlt	sq64_cont%=		\n" 	// if input reg < test reg, skip

		"	mov		r8, r4			\n"		// save new (x ^ 2) reg
		"	mov		r9, r5			\n"
		"	mov		r10, r6			\n"
		"	mov		r11, r7			\n"
		"	add		%A0, r12		\n"		// add (dx ^ 2) reg to running (2 * x * dx) reg
		"	adc		%B0, r13		\n"
		"	adc		%C0, r14		\n"
		"	adc		%D0, r28		\n"

		"sq64_cont%=:				\n"
		"	lsr		r28				\n"		// divide DX2 reg by 2
		"	ror		r14				\n"
		"	ror		r13				\n"
		"	ror		r12				\n"
		"	brcc	sq64_mloop%=	\n"
		"sq64_exit%=:				\n"

		"	pop	r28					\n"		// restore original contents of DX2 reg
		"	pop	r14					\n"
		"	pop	r13					\n"
		"	pop	r12					\n"
		"	pop	r11					\n"		// restore original contents of X2 reg
		"	pop	r10					\n"
		"	pop	r9					\n"
		"	pop	r8					\n"
		"	pop	r7					\n"		// restore original contents of test reg
		"	pop	r6					\n"
		"	pop	r5					\n"
		"	pop	r4					\n"

		: "+d" (output)						// ensure this is in r16-r31 space
		: "d" (input)						// ensure this is in r16-r31 space
	);

#else // defined(useAssemblyLanguage)
	uint32_t test;

	uint32_t X2; // (x ^ 2) term
	uint32_t DX2; // (dx ^ 2) term

	output = 0;
	X2 = 0;

	DX2 = 1073741824; // set initial (dx ^ 2) term to 2 ^ 30

    do
	{

		test = X2 + output + DX2; // form test value

		output >>= 1;

		if (input >= test) // if input number is greater than or equal to current test value
		{

			X2 = test; // save new (x ^ 2) term
			output += DX2; // add (dx ^ 2) term to running (2 * x * dx) term

		}

		DX2 >>= 2;

	} while (DX2);

#endif // defined(useAssemblyLanguage)
	return output;

}

#endif // defined(useIsqrt)
static void SWEET64::init64byt(union union_64 * an, uint8_t byt)
{

#if defined(useAssemblyLanguage)
	asm volatile(
		"	st	%a0,   %A1			\n"
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
#else // defined(useAssemblyLanguage)
	for (uint8_t x = 1; x < 8; x++) an->u8[(uint16_t)(x)] = 0;
	an->u8[0] = byt;
#endif // defined(useAssemblyLanguage)

}

static void SWEET64::init64(union union_64 * an, unsigned long dWordL)
{

#if defined(useAssemblyLanguage)
	asm volatile(
		"	st	%a0,   %A1			\n"
		"	std	%a0+1, %B1			\n"
		"	std	%a0+2, %C1			\n"
		"	std	%a0+3, %D1			\n"
		"	std	%a0+4, __zero_reg__	\n"
		"	std	%a0+5, __zero_reg__	\n"
		"	std	%a0+6, __zero_reg__	\n"
		"	std	%a0+7, __zero_reg__	\n"
		: "+e" (an)
		: "r" (dWordL)
	);
#else // defined(useAssemblyLanguage)
	an->ul[1] = 0;
	an->ul[0] = dWordL;
#endif // defined(useAssemblyLanguage)

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

}

