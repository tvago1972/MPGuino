/*

 SWEET64 is a low-level pseudocode interpreter, meant to save a lot of space in program memory for 64-bit calculations

 the opcode definitions below provide means of manipulating MPGuino trip variables, interacting with calculated and EEPROM parameters, and providing basic
 number formatting for interaction with string representations of numbers

 SWEET64 is inspired by an early pseudocode processor named SWEET16, which was written by Steve Wozniak of Apple Computer back in 1977. SWEET16 was written to
    perform 16-bit machine language instructions while running on an 8-bit microprocessor

*/

#if defined(useIsqrt)
static uint32_t iSqrt(uint32_t input);

#endif // defined(useIsqrt)
namespace SWEET64 /* 64-bit pseudo-processor section prototype */
{

	static s64pc_t makeProgmemProgram(s64prgm_ptr_t ptr);
	static s64pc_t makeRAMprogram(uint8_t * ptr);
#if defined(useSWEET64RAMprograms)
	static uint8_t readProgramRAM(uint8_t addr);
	static void writeProgramRAM(uint8_t addr, uint8_t value);
	static void fillProgramRAM(uint8_t value);
	static uint16_t getProgramRAMsize(void);
	static uint8_t * getProgramRAMaddress(uint8_t addr);
	static uint8_t getProgramRAMoffset(uint8_t * ptr);
	static s64pc_t makeRAMprogram(uint8_t addr);
	static void enableProgramRAMoverride(uint8_t prgmIdx, uint8_t ramAddr);
	static void disableProgramRAMoverride(void);
	static uint8_t isProgramRAMoverrideEnabled(void);
	static uint8_t getProgramRAMoverrideIndex(void);
	static uint8_t getProgramRAMoverrideAddress(void);
#endif // defined(useSWEET64RAMprograms)
#if defined(useSWEET64RAMprograms) || (defined(useDebugTerminalSWEET64) && defined(useDebugTerminalLabels))
	static uint16_t getProgramLength(uint8_t prgmIdx);
#endif // defined(useSWEET64RAMprograms) || (defined(useDebugTerminalSWEET64) && defined(useDebugTerminalLabels))
	static uint8_t isProgramValid(s64pc_t prgmPtr);
	static uint8_t readProgramByte(s64pc_t &prgmPtr);
	static s64pc_t getProgramPC(uint8_t prgmIdx);
	static s64prgm_ptr_t getProgramPointer(uint8_t prgmIdx);
	static uint32_t runPrgm(s64prgm_ptr_t sched, uint8_t tripIdx);
	static uint32_t runPrgm(s64pc_t sched, uint8_t tripIdx);
	static void fetchInstruction(union union_32 * instrLWord, s64pc_t &prgmPtr, uint8_t * prgmReg8);
	static void executeInstruction(union union_32 * instrLWord, s64pc_t &prgmPtr, s64pc_t prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8);
	static void setProgramError(uint8_t * prgmReg8, uint8_t errorCode);
	static void addProgramOffset(s64pc_t &prgmPtr, uint8_t offset);
	static void copy64(union union_64 * an, union union_64 * ann);
	static void swap64(union union_64 * an, union union_64 * ann);
	static void shr64(union union_64 * an);
	static void shl64(union union_64 * an);
	static void adc64(union union_64 * an, union union_64 * ann);
	static void sbc64(union union_64 * an, union union_64 * ann, uint8_t sbcFlag);
	static void mult64(uint64_t * prgmReg64);
	static void div64(uint64_t * prgmReg64);
	static void init64byt(union union_64 * an, uint8_t byt);
	static void init64(union union_64 * an, uint32_t dWordL);
	static void flagSet(uint8_t condition, uint8_t flag);
	static void flagSet64(uint8_t n, uint8_t z, uint8_t c);
	static void registerTest64(union union_64 * an);

}

static const uint8_t s64vRegisterOperation =		0b10000000;
static const uint8_t s64vReadInRegisterByte =		0b01000000;	// becomes de-facto "instruction is good" bit if instruction doesn't deal with 64-bit registers
static const uint8_t s64vReadInOperandByte =		0b00100000;
static const uint8_t s64vReadInExtraByte =			0b00010000;
static const uint8_t s64vOperandIndexed =			0b00001000;
static const uint8_t s64vExtraIndexed =				0b00000100;
static const uint8_t s64vExtraJump =				0b00000010;
static const uint8_t s64vRelativeOperand =			0b00000001;

enum {
	s64errNone,
	s64errBadProgramCounter,
	s64errBadOpcode,
	s64errBadRegisterOperand,
	s64errMissingRegisterOperand,
	s64errMissingPrimaryOperand,
	s64errMissingExtraOperand,
	s64errBadExpandedOpcode,
	s64errStackOverflow,
	s64errBadOperand
};

static const uint8_t r00 =	0;			// do not fetch register operand
static const uint8_t r01 =	r00 + 32;	// fetch rX and rY from program
static const uint8_t r02 =	r01 + 32;	// set rX = r5
static const uint8_t r03 =	r02 + 32;	// set rX = r5, rY = r1
static const uint8_t r04 =	r03 + 32;	// fetch rP and rS from program
static const uint8_t r05 =	r04 + 32;	// set rP = r5, rS = r2
static const uint8_t r06 =	r05 + 32;	// fetch rP from program, set rS = r5, and rX to r5
static const uint8_t r07 =	r06 + 32;	// fetch rX and rY from program, shift rY to rX if in metric mode

static const uint8_t rxxMask =	0b11100000;

static const uint8_t p00 =	0;			// do not fetch primary operand
static const uint8_t p01 =	p00 + 4;	// load primary operand from program
static const uint8_t p02 =	p01 + 4;	// load primary operand from index
static const uint8_t p03 =	p02 + 4;	// load primary operand from program + index
static const uint8_t p04 =	p03 + 4;	//
static const uint8_t p05 =	p04 + 4;	//
static const uint8_t p06 =	p05 + 4;	//
static const uint8_t p07 =	p06 + 4;	//

static const uint8_t pxxMask =	0b00011100;

static const uint8_t s00 =	0;			// do not fetch secondary operand
static const uint8_t s01 =	s00 + 1;	// fetch secondary operand from program
static const uint8_t s02 =	s01 + 1;	// fetch secondary operand from index
static const uint8_t s03 =	s02 + 1;	// fetch secondary operand from jump register

static const uint8_t sxxMask =	0b00000011;

// operations where opcode prefix rxx = r00
enum {
	e01,						// branch c=1 | z=1		(bgte)
	e02,						// branch c=1			(bgt)
	e03,						// branch c=0			(blte)
	e04,						// branch z=0			(bne)
	e05,						// branch z=1			(beq)
	e06,						// branch n=0			(bpl)
	e07,						// branch n=1			(bmi)
	e08,						// branch v=0			(bvc)
	e09,						// branch v=1			(bvs)
	e10,						// branch c=0 & z=0		(blt)
	e11,						// branch if metric mode
	e12,						// branch if SAE mode
	e13,						// branch if output is fuel/distance
	e14,						// branch if output is distance/fuel
	e15,						// branch always
	eMaxBranchInstrIdx,
	e16 = eMaxBranchInstrIdx,	// return
	e18,						// test index
	e23,						// load index
	e24,						// load index EEPROM
	e25,						// compare index
	e26,						// load index EEPROM parameter length
	e27,						// call
	e28,						// jump
	e29,						// load jump register
	e30,						// clear register flag
	e31							// set register flag
};

// operations where opcode prefix rxx is not r00
static const uint8_t m00 =	0;			// no operation
static const uint8_t m01 =	m00 + 1;	// add			rP = rP + rS
static const uint8_t m02 =	m01 + 1;	// sub			rP = rP - rS
static const uint8_t m03 =	m02 + 1;	// compare		na = rP - rS
static const uint8_t m04 =	m03 + 1;	// test			r5
static const uint8_t m05 =	m04 + 1;	// multiply		r2 = r2 * r5
static const uint8_t m06 =	m05 + 1;	// divide		r2 = r2 / r5 rmdr r1 and qadj r5
static const uint8_t m07 =	m06 + 1;

static const uint8_t mxxMask = 0b00000111;

enum {
	i00 = 0,					// no operation
	i01 = i00 + 8,				// load rX with rY
	i02 = i01 + 8,				// swap rX rY
	i03 = i02 + 8,				// load rX with EEPROM
	i04 = i03 + 8,				// store EEPROM rX
	i05 = i04 + 8,
	i06 = i05 + 8,
	i07 = i06 + 8,				// load rX with program variable
	i08 = i07 + 8,				// store program variable rX
	i09 = i08 + 8,
	i10 = i09 + 8,				// load rX with byte
	i11 = i10 + 8,
	i12 = i11 + 8,
	i13 = i12 + 8,
	i14 = i13 + 8,				// load rX with const
	i15 = i14 + 8,
	i16 = i15 + 8,
	i17 = i16 + 8,				// load rX with FEvT trip variable
	i18 = i17 + 8,				// load rX with trip variable
	i19 = i18 + 8,				// store trip variable rX
	i20 = i19 + 8,				// load rX with element of Matrix X
	i21 = i20 + 8,				// store Matrix X rX
	i22 = i21 + 8,				// load rX with element of Inverse Matrix
	i23 = i22 + 8,				// store Inverse Matrix rX
	i24 = i23 + 8,				// load rX with element of ExpData Matrix
	i25 = i24 + 8,				// store ExpData Matrix rX
	i26 = i25 + 8,				// load rX with element of Coefficient Matrix
	i27 = i26 + 8,				// store Coefficient Matrix rX
	i28 = i27 + 8,				// integer square root
	i29 = i28 + 8,				// shift rX left
	i30 = i29 + 8,				// shift rX right
	i31 = i30 + 8				// BCD adjust
};

static const uint8_t ixxMask = 0b11111000;

static uint8_t SWEET64processorFlags;

// x < y        - carry flag is clear, zero flag is clear
// x == y       - carry flag is set, zero flag is set
// x > y        - carry flag is set, zero flag is clear
//
// (x - y) >= 0 - minus flag is clear
// (x - y) < 0	- minus flag is set
//
static const uint8_t SWEET64carryFlag =			0b00000001;			// this is set for arithmetic and branch test operations
static const uint8_t SWEET64zeroFlag =			0b00000010;			// this is set for arithmetic and branch test operations
static const uint8_t SWEET64minusFlag =			0b00000100;			// this is set for arithmetic and branch test operations
static const uint8_t SWEET64overflowFlag =		0b00001000;			// this is set for arithmetic and branch test operations
static const uint8_t SWEET64errorFlag =			0b00010000;			// this is set when the SWEET64 engine detects malformed program flow or operands

static const uint8_t SWEET64traceCommandFlag =	0b01000000;			// commands whether trace mode is on or off
static const uint8_t SWEET64traceFlag =			0b10000000;

static const uint8_t SWEET64traceFlagGroup =	SWEET64traceCommandFlag | SWEET64traceFlag;

#if defined(useDebugTerminal)
static const char SWEET64processorFlagMarkers[] PROGMEM = {
	" S64status" tcEOS
	"T" tcEOS
	"TC" tcEOS
	"0" tcEOS
	"E" tcEOS
	"OVF" tcEOS
	"N" tcEOS
	"Z" tcEOS
	"C" tcEOS
};

#endif // defined(useDebugTerminal)
enum {
	s64reg64_1,							// general purpose
	s64reg64_2,							// output value / general purpose
	s64reg64_3,							// general purpose / temporary storage
	s64reg64_4,							// used in multiply, divide operations
	s64reg64_5,							// used in multiply, divide operations
#if defined(useDebugTerminal)
	s64reg64_6,							// used for debug terminal
	s64reg64_7,							// used for debug terminal
#endif // defined(useDebugTerminal)
	s64reg64count
};

static uint64_t s64reg[(uint16_t)(s64reg64count)];

static const uint8_t s64stackSize =		16;
static s64pc_t s64stack[(uint16_t)(s64stackSize)];
#if defined(useDebugTerminalSWEET64)
static uint8_t s64callIndexStack[s64stackSize + 1];
#endif // defined(useDebugTerminalSWEET64)

enum {
	s64oprRegXY,
	s64oprRegRS,
	s64oprPrimary,
	s64oprExtra,
	s64oprCount
};

static uint8_t s64operands[(uint16_t)(s64oprCount)];

enum {
	si64reg8flags,
	si64reg8valid,
	si64reg8trip,
	si64reg8spnt,
	si64reg8jump,
#if defined(useDebugTerminalSWEET64)
	si64reg8error,
#endif // defined(useDebugTerminalSWEET64)
	si64reg8count
};

static uint8_t s64reg8[(uint16_t)(si64reg8count)];

#if defined(useDebugTerminalLabels)
static const char terminalSWEET64registerLabels[] PROGMEM = {
	"s64reg64_1" tcEOS
	"s64reg64_2" tcEOS
	"s64reg64_3" tcEOS
	"s64reg64_4" tcEOS
	"s64reg64_5" tcEOS
	"s64reg64_6" tcEOS
	"s64reg64_7" tcEOS
	"si64reg8flags" tcEOS
	"si64reg8valid" tcEOS
	"si64reg8trip" tcEOS
	"si64reg8spnt" tcEOS
	"si64reg8jump" tcEOS
#if defined(useDebugTerminalSWEET64)
	"si64reg8error" tcEOS
#endif // defined(useDebugTerminalSWEET64)
};

#endif // defined(useDebugTerminalLabels)
#if defined(useMatrixMath)
/*
for useCoastDownCalculator -
matrix_x - coastdown sample storage
matrix_e - coastdown energy value storage
matrix_c - coastdown coefficient matrix (what we are trying to solve for)
    [ pCoefficientDidx
      pCoefficientVidx
      pCoefficientRRidx ]

for useFuelParamCalculator -

(fillup amount)(pMicroSecondsPerGallonIdx) + (injector open count)(pInjectorSettleTimeIdx) = (raw injector open time)

         fillup amount - user provided
   injector open count - measured by MPGuino
raw injector open time - measured by MPGuino

matrix_x
    [ fillup amount 1      injector open count 1
      fillup amount 2      injector open count 2 ]

matrix_e
	[ raw injector open time 1
      raw injector open time 2 ]

matrix_c - fuel injector coefficient matrix (what we are trying to solve for)
    [ pMicroSecondsPerGallonIdx
      pInjectorSettleTimeIdx ]

for both cases -

matrix_r - inverse matrix
matrix_r = matrix_x ^ -1

(matrix_x ^ -1)(matrix_x) = I or identity matrix

               (matrix_x)(matrix_c) = matrix_e

(matrix_x ^ -1)(matrix_x)(matrix_c) = (matrix_x ^ -1)(matrix_e)

                      (I)(matrix_c) = (matrix_x ^ -1)(matrix_e)

                           matrix_c = (matrix_x ^ -1)(matrix_e)

*/

uint64_t matrix_x[3][3];
uint64_t matrix_r[3][3];
uint64_t matrix_e[3];
uint64_t matrix_c[3];

#endif // defined(useMatrixMath)

enum {
	instrTestReg,							// tests 64-bit register for zero condition or high bit set
	instrTestIndex,							// tests primary index for zero condition or high bit set
	instrCmpXtoY,							// compares 64-bit register Y from 64-bit register X
	instrCmpIndex,							// compares primary index from operand

	instrBranchIfVclear,					// branches if result[0..63] is not all 1s
	instrBranchIfVset,						// branches if result[0..63] is all 1s
	instrBranchIfMclear,					// branches if result most significant bit is clear
	instrBranchIfMset,						// branches if result most significant bit is set
	instrBranchIfZclear,					// branches if result zero flag is clear (X != Y or X != 0)
	instrBranchIfZset,						// branches if result zero flag is set (X == Y or X == 0)
	instrBranchIfCclear,					// branches if result carry flag is clear (X <= Y)
	instrBranchIfCset,						// branches if result carry flag is set (X > Y)
	instrBranchIfLT,						// branches if X < Y or index < operand
	instrBranchIfGTorE,						// branches if X >= Y or index >= operand
	instrBranchIfMetricMode,				// branches if MPGuino is in metric mode
	instrBranchIfSAEmode,					// branches if MPGuino is in SAE mode
	instrBranchIfFuelOverDist,				// branches if MPGuino outputs fuel/distance
	instrBranchIfDistOverFuel,				// branches if MPGuino outputs distance/fuel

	instrSkip,								// branches always
	instrCall,								// push current program counter onto stack, then call indexed routine
	instrCallImplied,						// push current program counter onto stack, then call indexed routine stored in jump register
	instrJump,								// jump to indexed routine

/*

	operands specifying 64-bit registers take the form of YX as a hexadecimal value

	e.g., instrLdReg, 0x24,

	means "load 64-bit register 4 with the contents of 64-bit register 2"

*/
	instrLdReg,								// load 64-bit register X with contents of 64-bit register Y
	instrLdRegByteFromIndex,				// load 64-bit register X with primary index byte value
	instrLdRegByte,							// load 64-bit register X with immediate byte value
	instrLdRegTripVar,						// load 64-bit register X with specified trip specified read-in register
	instrLdRegTripVarIndexed,				// load 64-bit register X with indexed trip specified read-in register
	instrLdRegTripVarOffset,				// load 64-bit register X with offset indexed trip specified read-in register
	instrLdRegTripVarIndexedRV,				// load 64-bit register X with specified trip indexed read-in register
	instrStRegTripVarIndexed,				// store 64-bit register X value to indexed trip specified read-in register
	instrStRegTripVarIndexedRV,				// store 64-bit register X value to specified trip indexed read-in register
	instrLdRegConst,						// load 64-bit register X with constant value
	instrLdRegConstIndexed,					// load 64-bit register X with indexed constant value
	instrLdRegConstOffset,					// load 64-bit register X with offset indexed constant value
	instrLdRegConstMetric,					// load 64-bit register X (or Y if in metric mode) with constant value
	instrDoBCDadjust,						// perform BCD-style conversion of 64-bit register X, using format stored in 64-bit register 3
	instrLdRegEEPROM,						// load 64-bit register X with EEPROM parameter value
	instrLdRegEEPROMindexed,				// load 64-bit register X with indexed EEPROM parameter value
	instrStRegEEPROM,						// store 64-bit register X value to EEPROM parameter
	instrStRegEEPROMindexed,				// store 64-bit register X value to indexed EEPROM parameter
	instrLdRegVariable,						// load 64-bit register X with program variable register value
	instrLdRegVariableIndexed,				// load 64-bit register X with indexed program variable register value
	instrLdRegVariableOffset,				// load 64-bit register X with offset indexed program variable register value
	instrStRegVariable,						// store 64-bit register X value to program variable register
	instrStRegVariableIndexed,				// store 64-bit register X value to indexed program variable register
	instrStRegVariableOffset,				// store 64-bit register X value to offset indexed program variable register
	instrLxdI,								// load primary index register with immediate byte value
	instrLxdIEEPROM,						// load primary index register with EEPROM parameter value
	instrLxdIEEPROMoffset,					// load primary index register with offset indexed EEPROM parameter
	instrLxdIParamLength,					// load primary index register with bit length of EEPROM parameter
	instrLxdIParamLengthIndexed,			// load primary index register with bit length of indexed EEPROM parameter
	instrSwapReg,							// swap contents of 64-bit registers X and Y
	instrSubYfromX,							// subtract 64-bit register Y from 64-bit register X
	instrSubByteFromX,						// subtract immediate byte value from 64-bit register X
	instrSubVariableFromX,					// subtract program register value from 64-bit register X
	instrAddYtoX,							// add 64-bit register Y to 64-bit register X
	instrAdjustQuotient,					// bumps 64-bit register 2 by 1 if last division resulted in remainder * 2 > divisor
	instrAddByteToX,						// add immediate byte value to 64-bit register X
	instrAddConstToX,						// add stored constant value to 64-bit register X
	instrAddEEPROMtoX,						// add EEPROM parameter value to 64-bit register X
	instrAddVariableToX,					// add program register value to 64-bit register X
	instrAddIndexToX,						// add contents of primary index to 64-bit register X
	instrMul2by1,							// multiply 64-bit register 2 by contents of 64-bit register 1
	instrMul2byByte,						// multiply 64-bit register 2 by immediate byte value
	instrMul2byConst,						// multiply 64-bit register 2 by constant value
	instrMul2byEEPROM,						// multiply 64-bit register 2 by EEPROM parameter value
	instrMul2byVariable,					// multiply 64-bit register 2 by program variable register value
	instrMul2byTripVarIndexed,				// multiply 64-bit register 2 by indexed trip specified read-in register
	instrDiv2by1,							// divide 64-bit register 2 by contents of 64-bit register 1
	instrDiv2byConst,						// divide 64-bit register 2 by constant value
	instrDiv2byEEPROM,						// divide 64-bit register 2 by EEPROM parameter value
	instrDiv2byVariable,					// divide 64-bit register 2 by program variable register value
	instrDiv2byTripVarIndexed,				// divide 64-bit register 2 by indexed trip specified read-in register
	instrDiv2byByte,						// divide 64-bit register 2 by immediate byte value
	instrShiftRegLeft,						// shift 64-bit register X one bit left
	instrShiftRegRight,						// shift 64-bit register X one bit right
	instrAddIndex,							// add immediate byte value to primary index register
	instrLdJumpReg,							// load jump register with routine index value
	instrClearFlag,							// clear SWEET64 status flag
	instrSetFlag,							// set SWEET64 status flag

#if defined(useIsqrt)
	instrIsqrt,							// perform integer square root on lower 32 bits of 64-bit register
#endif // defined(useIsqrt)

#if defined(useFEvTdata)
	instrLdRegTripFEvTindexed,				// load trip index from fuel econ vs time trip array
#endif // defined(useFEvTdata)

#if defined(useMatrixMath)
	instrLdRegXColIndexedRow,				// load 64-bit register X with contents of Matrix X indexed row specified column
	instrStRegXColIndexedRow,				// store contents of 64-bit register X to Matrix X indexed row specified column
	instrLdRegRColIndexedRow,				// load 64-bit register X with contents of Inverse Matrix indexed row specified column
	instrStRegRColIndexedRow,				// store contents of 64-bit register X to Inverse Matrix indexed row specified column
	instrLdRegEIndexedRow,					// load 64-bit register X with contents of ExpData Matrix indexed row
	instrStRegEIndexedRow,					// store contents of 64-bit register X to ExpData Matrix indexed row
	instrLdRegCIndexedRow,					// load 64-bit register X with contents of Coefficient Matrix indexed row
	instrStRegCIndexedRow,					// store contents of 64-bit register X to Coefficient Matrix indexed row
#endif // defined(useMatrixMath)

	instrDone,								// return to caller - if at topmost level of stack, return to main program
	maxValidSWEET64instr
};

// Compatibility aliases for the former read-only constant instruction names.
static const uint8_t instrLdRegRdOnly =				instrLdRegConst;
static const uint8_t instrLdRegRdOnlyIndexed =		instrLdRegConstIndexed;
static const uint8_t instrLdRegRdOnlyOffset =		instrLdRegConstOffset;
static const uint8_t instrLdRegRdOnlyMetric =		instrLdRegConstMetric;
static const uint8_t instrMul2byRdOnly =			instrMul2byConst;
static const uint8_t instrDiv2byRdOnly =			instrDiv2byConst;

#if defined(useDebugTerminalSWEET64)
static const char opCodeList[] PROGMEM = {
	"TestReg" tcEOS
	"TestIndex" tcEOS
	"CmpXtoY" tcEOS
	"CmpIndex" tcEOS

	"BranchIfVclear" tcEOS
	"BranchIfVset" tcEOS
	"BranchIfMclear" tcEOS
	"BranchIfMset" tcEOS
	"BranchIfZclear" tcEOS
	"BranchIfZset" tcEOS
	"BranchIfCclear" tcEOS
	"BranchIfCset" tcEOS
	"BranchIfLT" tcEOS
	"BranchIfGTorE" tcEOS
	"BranchIfMetricMode" tcEOS
	"BranchIfSAEmode" tcEOS
	"BranchIfFuelOverDist" tcEOS
	"BranchIfDistOverFuel" tcEOS

	"Skip" tcEOS
	"Call" tcEOS
	"CallImplied" tcEOS
	"Jump" tcEOS

	"LdReg" tcEOS
	"LdRegByteFromIndex" tcEOS
	"LdRegByte" tcEOS
	"LdRegTripVar" tcEOS
	"LdRegTripVarIndexed" tcEOS
	"LdRegTripVarOffset" tcEOS
	"LdRegTripVarIndexedRV" tcEOS
	"StRegTripVarIndexed" tcEOS
	"StRegTripVarIndexedRV" tcEOS
	"LdRegConst" tcEOS
	"LdRegConstIndexed" tcEOS
	"LdRegConstOffset" tcEOS
	"LdRegConstMetric" tcEOS
	"DoBCDadjust" tcEOS
	"LdRegEEPROM" tcEOS
	"LdRegEEPROMindexed" tcEOS
	"StRegEEPROM" tcEOS
	"StRegEEPROMindexed" tcEOS
	"LdRegVariable" tcEOS
	"LdRegVariableIndexed" tcEOS
	"LdRegVariableOffset" tcEOS
	"StRegVariable" tcEOS
	"StRegVariableIndexed" tcEOS
	"StRegVariableOffset" tcEOS
	"LxdI" tcEOS
	"LxdIEEPROM" tcEOS
	"LxdIEEPROMoffset" tcEOS
	"LxdIParamLength" tcEOS
	"LxdIParamLengthIndexed" tcEOS
	"SwapReg" tcEOS
	"SubYfromX" tcEOS
	"SubByteFromX" tcEOS
	"SubVariableFromX" tcEOS
	"AddYtoX" tcEOS
	"AdjustQuotient" tcEOS
	"AddByteToX" tcEOS
	"AddConstToX" tcEOS
	"AddEEPROMtoX" tcEOS
	"AddVariableToX" tcEOS
	"AddIndexToX" tcEOS
	"Mul2by1" tcEOS
	"Mul2byByte" tcEOS
	"Mul2byConst" tcEOS
	"Mul2byEEPROM" tcEOS
	"Mul2byVariable" tcEOS
	"Mul2byTripVarIndexed" tcEOS
	"Div2by1" tcEOS
	"Div2byConst" tcEOS
	"Div2byEEPROM" tcEOS
	"Div2byVariable" tcEOS
	"Div2byTripVarIndexed" tcEOS
	"Div2byByte" tcEOS
	"ShiftRegLeft" tcEOS
	"ShiftRegRight" tcEOS
	"AddIndex" tcEOS
	"LdJumpReg" tcEOS
	"ClearFlag" tcEOS
	"SetFlag" tcEOS
#if defined(useIsqrt)
	"Isqrt" tcEOS
#endif // defined(useIsqrt)
#if defined(useFEvTdata)
	"LdRegTripFEvTindexed" tcEOS
#endif // defined(useFEvTdata)
#if defined(useMatrixMath)
	"LdRegXColIndexedRow" tcEOS
	"StRegXColIndexedRow" tcEOS
	"LdRegRColIndexedRow" tcEOS
	"StRegRColIndexedRow" tcEOS
	"LdRegEIndexedRow" tcEOS
	"StRegEIndexedRow" tcEOS
	"LdRegCIndexedRow" tcEOS
	"StRegCIndexedRow" tcEOS
#endif // defined(useMatrixMath)
	"Done" tcEOS
};

#endif // defined(useDebugTerminalSWEET64)
// these #defines can optionally be used to generate SWEET64 bytecodes
//
#define instrBranchIfOverflow		instrBranchIfVset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfPlus			instrBranchIfMclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfMinus			instrBranchIfMset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfNotE			instrBranchIfZclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfE				instrBranchIfZset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfNotZero		instrBranchIfZclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfZero			instrBranchIfZset					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfLTorE			instrBranchIfCclear					// provided as a convenience for aspiring SWEET64 coders
#define instrBranchIfGT				instrBranchIfCset					// provided as a convenience for aspiring SWEET64 coders

static const uint16_t opcodeFetchWord[(uint16_t)(maxValidSWEET64instr)] PROGMEM = {
	(((r04 | p00 | s00) << 8) |			(m04 | i00)),			// instrTestReg
	(((r00 | p00 | s00) << 8) |			(e18)),					// instrTestIndex
	(((r04 | p00 | s00) << 8) |			(m03 | i00)),			// instrCmpXtoY
	(((r00 | p01 | s00) << 8) |			(e25)),					// instrCmpIndex

	(((r00 | p00 | s01) << 8) |			(e08)),					// instrBranchIfVclear
	(((r00 | p00 | s01) << 8) |			(e09)),					// instrBranchIfVset
	(((r00 | p00 | s01) << 8) |			(e06)),					// instrBranchIfMclear
	(((r00 | p00 | s01) << 8) |			(e07)),					// instrBranchIfMset
	(((r00 | p00 | s01) << 8) |			(e04)),					// instrBranchIfZclear
	(((r00 | p00 | s01) << 8) |			(e05)),					// instrBranchIfZset
	(((r00 | p00 | s01) << 8) |			(e03)),					// instrBranchIfCclear
	(((r00 | p00 | s01) << 8) |			(e02)),					// instrBranchIfCset
	(((r00 | p00 | s01) << 8) |			(e10)),					// instrBranchIfLT
	(((r00 | p00 | s01) << 8) |			(e01)),					// instrBranchIfGTorE
	(((r00 | p00 | s01) << 8) |			(e11)),					// instrBranchIfMetricMode
	(((r00 | p00 | s01) << 8) |			(e12)),					// instrBranchIfSAEmode
	(((r00 | p00 | s01) << 8) |			(e13)),					// instrBranchIfFuelOverDist
	(((r00 | p00 | s01) << 8) |			(e14)),					// instrBranchIfDistOverFuel
	(((r00 | p00 | s01) << 8) |			(e15)),					// instrSkip
	(((r00 | p00 | s01) << 8) |			(e27)),					// instrCall
	(((r00 | p00 | s03) << 8) |			(e27)),					// instrCallImplied
	(((r00 | p00 | s01) << 8) |			(e28)),					// instrJump
	(((r01 | p00 | s00) << 8) |			(m00 | i01)),			// instrLdReg
	(((r01 | p02 | s00) << 8) |			(m00 | i10)),			// instrLdRegByteFromIndex
	(((r01 | p01 | s00) << 8) |			(m00 | i10)),			// instrLdRegByte
	(((r04 | p01 | s01) << 8) |			(m04 | i18)),			// instrLdRegTripVar
	(((r04 | p02 | s01) << 8) |			(m04 | i18)),			// instrLdRegTripVarIndexed
	(((r04 | p03 | s01) << 8) |			(m04 | i18)),			// instrLdRegTripVarOffset
	(((r04 | p01 | s02) << 8) |			(m04 | i18)),			// instrLdRegTripVarIndexedRV
	(((r01 | p02 | s01) << 8) |			(m00 | i19)),			// instrStRegTripVarIndexed
	(((r01 | p01 | s02) << 8) |			(m00 | i19)),			// instrStRegTripVarIndexedRV
	(((r01 | p01 | s00) << 8) |			(m00 | i14)),			// instrLdRegConst
	(((r01 | p02 | s00) << 8) |			(m00 | i14)),			// instrLdRegConstIndexed
	(((r01 | p03 | s00) << 8) |			(m00 | i14)),			// instrLdRegConstOffset
	(((r07 | p01 | s00) << 8) |			(m00 | i14)),			// instrLdRegConstMetric
	(((r01 | p01 | s00) << 8) |			(m00 | i31)),			// instrDoBCDadjust
	(((r01 | p01 | s00) << 8) |			(m00 | i03)),			// instrLdRegEEPROM
	(((r01 | p02 | s00) << 8) |			(m00 | i03)),			// instrLdRegEEPROMindexed
	(((r01 | p01 | s00) << 8) |			(m00 | i04)),			// instrStRegEEPROM
	(((r01 | p02 | s00) << 8) |			(m00 | i04)),			// instrStRegEEPROMindexed
	(((r01 | p01 | s00) << 8) |			(m00 | i07)),			// instrLdRegVariable
	(((r01 | p02 | s00) << 8) |			(m00 | i07)),			// instrLdRegVariableIndexed
	(((r01 | p03 | s00) << 8) |			(m00 | i07)),			// instrLdRegVariableOffset
	(((r01 | p01 | s00) << 8) |			(m00 | i08)),			// instrStRegVariable
	(((r01 | p02 | s00) << 8) |			(m00 | i08)),			// instrStRegVariableIndexed
	(((r01 | p03 | s00) << 8) |			(m00 | i08)),			// instrStRegVariableOffset
	(((r00 | p01 | s00) << 8) |			(e23)),					// instrLxdI
	(((r00 | p01 | s00) << 8) |			(e24)),					// instrLxdIEEPROM
	(((r00 | p03 | s00) << 8) |			(e24)),					// instrLxdIEEPROMoffset
	(((r00 | p01 | s00) << 8) |			(e26)),					// instrLxdIParamLength
	(((r00 | p02 | s00) << 8) |			(e26)),					// instrLxdIParamLengthIndexed
	(((r01 | p00 | s00) << 8) |			(m00 | i02)),			// instrSwapReg
	(((r04 | p00 | s00) << 8) |			(m02 | i00)),			// instrSubYfromX
	(((r06 | p01 | s00) << 8) |			(m02 | i10)),			// instrSubByteFromX
	(((r06 | p01 | s00) << 8) |			(m02 | i07)),			// instrSubVariableFromX
	(((r04 | p00 | s00) << 8) |			(m01 | i00)),			// instrAddYtoX
	(((r05 | p00 | s00) << 8) |			(m01 | i00)),			// instrAdjustQuotient
	(((r06 | p01 | s00) << 8) |			(m01 | i10)),			// instrAddByteToX
	(((r06 | p01 | s00) << 8) |			(m01 | i14)),			// instrAddConstToX
	(((r06 | p01 | s00) << 8) |			(m01 | i03)),			// instrAddEEPROMtoX
	(((r06 | p01 | s00) << 8) |			(m01 | i07)),			// instrAddVariableToX
	(((r06 | p02 | s00) << 8) |			(m01 | i10)),			// instrAddIndexToX
	(((r03 | p00 | s00) << 8) |			(m05 | i01)),			// instrMul2by1
	(((r02 | p01 | s00) << 8) |			(m05 | i10)),			// instrMul2byByte
	(((r02 | p01 | s00) << 8) |			(m05 | i14)),			// instrMul2byConst
	(((r02 | p01 | s00) << 8) |			(m05 | i03)),			// instrMul2byEEPROM
	(((r02 | p01 | s00) << 8) |			(m05 | i07)),			// instrMul2byVariable
	(((r02 | p02 | s01) << 8) |			(m05 | i18)),			// instrMul2byTripVarIndexed
	(((r03 | p00 | s00) << 8) |			(m06 | i01)),			// instrDiv2by1
	(((r02 | p01 | s00) << 8) |			(m06 | i14)),			// instrDiv2byConst
	(((r02 | p01 | s00) << 8) |			(m06 | i03)),			// instrDiv2byEEPROM
	(((r02 | p01 | s00) << 8) |			(m06 | i07)),			// instrDiv2byVariable
	(((r02 | p02 | s01) << 8) |			(m06 | i18)),			// instrDiv2byTripVarIndexed
	(((r02 | p01 | s00) << 8) |			(m06 | i10)),			// instrDiv2byByte
	(((r01 | p00 | s00) << 8) |			(m00 | i29)),			// instrShiftRegLeft
	(((r01 | p00 | s00) << 8) |			(m00 | i30)),			// instrShiftRegRight
	(((r00 | p03 | s00) << 8) |			(e23)),					// instrAddIndex
	(((r00 | p02 | s00) << 8) |			(e29)),					// instrLdJumpReg
	(((r00 | p01 | s00) << 8) |			(e30)),					// instrClearFlag
	(((r00 | p01 | s00) << 8) |			(e31)),					// instrSetFlag
#if defined(useIsqrt)
	(((r01 | p00 | s00) << 8) |			(m00 | i28)),			// instrIsqrt
#endif // defined(useIsqrt)
#if defined(useFEvTdata)
	(((r04 | p02 | s01) << 8) |			(m04 | i17)),			// instrLdRegTripFEvTindexed
#endif // defined(useFEvTdata)
#if defined(useMatrixMath)
	(((r01 | p01 | s02) << 8) |			(m00 | i20)),			// instrLdRegXColIndexedRow
	(((r01 | p01 | s02) << 8) |			(m00 | i21)),			// instrStRegXColIndexedRow
	(((r01 | p01 | s02) << 8) |			(m00 | i22)),			// instrLdRegRColIndexedRow
	(((r01 | p01 | s02) << 8) |			(m00 | i23)),			// instrStRegRColIndexedRow
	(((r01 | p00 | s02) << 8) |			(m00 | i24)),			// instrLdRegEIndexedRow
	(((r01 | p00 | s02) << 8) |			(m00 | i25)),			// instrStRegEIndexedRow
	(((r01 | p00 | s02) << 8) |			(m00 | i26)),			// instrLdRegCIndexedRow
	(((r01 | p00 | s02) << 8) |			(m00 | i27)),			// instrStRegCIndexedRow
#endif // defined(useMatrixMath)
	(((r00 | p00 | s00) << 8) |			(e16)),					// instrDone
};

// indexes into SWEET64 constant number value table
//
// the order of the indices, representing the powers of 10 between 10 and 1000000000, is vitally important to the
//    proper functioning of the autoranging feature of ull2str
//
enum {
	idxTen,

	idxOneHundred,

	idxOneThousand,
	idxDecimalPoint = idxOneThousand,			// decimal point format (the basis for all of those '* 1000' parameters)

	idxTenThousand,

	idxOneHundredThousand,
	idxMetricFE = idxOneHundredThousand,			// decimal point format * 100 for metric FE (L / 100km)
#if defined(useCoastDownCalculator)
	idxNumerDensity = idxOneHundredThousand,		// numerator to convert SAE density to metric density
#endif // defined(useCoastDownCalculator)
#ifdef useImperialGallon
	idxDenomImperialGallon = idxOneHundredThousand,	// denominator to convert Imperial gallons to liters
#endif // useImperialGallon

	idxOneMillion,
	idxMicroSecondsPerSecond = idxOneMillion,	// microseconds per second
	idxDenomDistance = idxOneMillion,			// denominator to convert miles to kilometers

	idxTenMillion,
#if defined(usePressure)
	idxDenomPressure = idxTenMillion,			// denominator to convert psig to kPa
#endif // defined(usePressure)

	idxOneHundredMillion,
	idxBCDdivisor = idxOneHundredMillion,		// divisor to separate lower 4 BCD bytes from 5th byte
#if defined(useCoastDownCalculator)
	idxDenomArea = idxOneHundredMillion,			// denominator to convert square feet to square meters
#endif // defined(useCoastDownCalculator)

	idxOneBillion,
	idxDenomVolume = idxOneBillion,				// denominator to convert US gallons to liters
#if defined(useVehicleParameters)
	idxNumerMass = idxOneBillion,				// numerator to convert pounds to kilograms
#endif // defined(useVehicleParameters)

	// these are not required to be in any particular order

	idxCycles0PerSecond,						// timer0 clock cycles per second
	idxCycles0PerTick,							// known as the "N" in the (processor speed)/(N * prescaler) for timer0 fast PWM mode
	idxTicks0PerSecond,							// timer0 clock ticks per second
	idxNumerDistance,							// numerator to convert miles to kilometers
	idxNumerVolume,								// numerator to convert US gallons to liters
	idxSecondsPerHour,							// number of seconds in an hour
#if defined(useTimer1Interrupt)
	idxCycles1PerSecond,						// timer1 clock cycles per second
	idxCycles1PerTick,							// known as the "N" in the (processor speed)/(N * prescaler) for timer1 phase correct PWM
	idxTicks1PerSecond,							// timer1 clock ticks per second
#endif // defined(useTimer1Interrupt)
#if defined(useClockSupport)
	idxSecondsPerDay,							// number of seconds in a day
#endif // defined(useClockSupport)
#if defined(usePressure)
	idxNumerPressure,							// numerator to convert psig to kPa
	idxCorrectionFactor,						// correction factor used for fuel calculations
	idxCorrectionFactor2,						// correction factor squared for square root function
#endif // defined(usePressure)
#if defined(useAnalogRead)
	idxNumerVoltage,							// numerator to convert volts DC to ADC steps
	idxDenomVoltage,							// denominator to convert volts DC to ADC steps
#endif // defined(useAnalogRead)
#if defined(useAlternatorVoltage)
	idxResistanceR5,							// resistor next to ground (via meelis11)
	idxResistanceR6,							// resistor next to diode  (via meelis11)
#endif // defined(useAlternatorVoltage)
#if defined(useVehicleParameters)
	idxDenomMass,								// denominator to convert pounds to kilograms
#if defined(useCoastDownCalculator)
	idxNumerArea,								// numerator to convert square feet to square meters
	idxDenomDensity,							// denominator to convert SAE density to metric density
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	idxPowerFactor,							// 22.84, or vehicle speed division factor for accel test power estimation function (228.4/10 for internal calculations)
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#ifdef useImperialGallon
	idxNumerImperialGallon,					// numerator to convert Imperial gallons to liters
#endif // useImperialGallon

	idxConstantEnd,
};

#if defined(useDebugTerminalLabels)
static const char terminalConstIdxNames[] PROGMEM = {
	"idxTen"
	tcEOS

	"idxOneHundred"
	tcEOS

	"idxOneThousand"
	"/idxDecimalPoint"
	tcEOS

	"idxTenThousand"
	tcEOS

	"idxOneHundredThousand"
	"/idxMetricFE"
#if defined(useCoastDownCalculator)
	"/idxNumerDensity"
#endif // defined(useCoastDownCalculator)
#ifdef useImperialGallon
	"/idxDenomImperialGallon"
#endif // useImperialGallon
	tcEOS

	"idxOneMillion"
	"/idxMicroSecondsPerSecond"
	"/idxDenomDistance"
	tcEOS

	"idxTenMillion"
#if defined(usePressure)
	"/idxDenomPressure"
#endif // defined(usePressure)
	tcEOS

	"idxOneHundredMillion"
	"/idxBCDdivisor"
#if defined(useCoastDownCalculator)
	"/idxDenomArea"
#endif // defined(useCoastDownCalculator)
	tcEOS

	"idxOneBillion"
	"/idxDenomVolume"
#if defined(useVehicleParameters)
	"/idxNumerMass"
#endif // defined(useVehicleParameters)
	tcEOS

	"idxCycles0PerSecond" tcEOS
	"idxCycles0PerTick" tcEOS
	"idxTicks0PerSecond" tcEOS
	"idxNumerDistance" tcEOS
	"idxNumerVolume" tcEOS
	"idxSecondsPerHour" tcEOS
#if defined(useTimer1Interrupt)
	"idxCycles1PerSecond" tcEOS
	"idxCycles1PerTick" tcEOS
	"idxTicks1PerSecond" tcEOS
#endif // defined(useTimer1Interrupt)
#if defined(useClockSupport)
	"idxSecondsPerDay" tcEOS
#endif // defined(useClockSupport)
#if defined(usePressure)
	"idxNumerPressure" tcEOS
	"idxCorrectionFactor" tcEOS
	"idxCorrectionFactor2" tcEOS
#endif // defined(usePressure)
#if defined(useAnalogRead)
	"idxNumerVoltage" tcEOS
	"idxDenomVoltage" tcEOS
#endif // defined(useAnalogRead)
#if defined(useAlternatorVoltage)
	"idxResistanceR5" tcEOS
	"idxResistanceR6" tcEOS
#endif // defined(useAlternatorVoltage)
#if defined(useVehicleParameters)
	"idxDenomMass" tcEOS
#if defined(useCoastDownCalculator)
	"idxNumerArea" tcEOS
	"idxDenomDensity" tcEOS
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	"idxPowerFactor" tcEOS
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#ifdef useImperialGallon
	"idxNumerImperialGallon" tcEOS
#endif // useImperialGallon
};

static const char terminalBCDformatNames[] PROGMEM = {
	"bcdFormat10digit" tcEOS
	"bcdFormatHHMMSS" tcEOS
	"bcdFormatH9MMSS" tcEOS
	"bcdFormatOverflow" tcEOS
};

#endif // defined(useDebugTerminalLabels)

static const uint32_t correctionFactor =	4096ul;
// SWEET64 conversion factor value table
//
// the order of the values, representing the powers of 10 between 10 and 1000000000, is vitally important to the
//    proper functioning of the autoranging feature of ull2str
//
static const uint32_t constantNumberList[(uint16_t)(idxConstantEnd)] PROGMEM = {
	10ul,									// idxTen

	100ul,									// idxOneHundred

	1000ul,									// idxOneThousand
											// idxDecimalPoint - decimal point format (the basis for all of those '* 1000' parameters)

	10000ul,								// idxTenThousand

	100000ul,								// idxOneHundredThousand
											// idxMetricFE - decimal point format * 100 for metric FE (L / 100km)
#if defined(useCoastDownCalculator)
											// idxNumerDensity - numerator to convert SAE density to metric density
#endif // defined(useCoastDownCalculator)
#ifdef useImperialGallon
											// idxDenomImperialGallon - denominator to convert Imperial gallons to liters
#endif // useImperialGallon

	1000000ul,								// idxOneMillion
											// idxMicroSecondsPerSecond - microseconds per second
											// idxDenomDistance - denominator to convert miles to kilometers

	10000000ul,								// idxTenMillion
#if defined(usePressure)
											// idxDenomPressure - denominator to convert psig to kPa
#endif // defined(usePressure)

	100000000ul,							// idxOneHundredMillion
											// idxBCDdivisor - divisor to separate lower 4 BCD bytes from 5th byte
#if defined(useCoastDownCalculator)
											// idxDenomArea - denominator to convert square feet to square meters
#endif // defined(useCoastDownCalculator)

	1000000000ul,							// idxOneBillion
											// idxDenomVolume - denominator to convert US gallons to liters
#if defined(useVehicleParameters)
											// idxNumerMass - numerator to convert pounds to kilograms
#endif // defined(useVehicleParameters)

	t0CyclesPerSecond,						// idxCycles0PerSecond - timer0 clock cycles per second
	256ul,									// idxCycles0PerTick - known as the "N" in the (processor speed)/(N * prescaler) for timer0 fast PWM mode
	t0TicksPerSecond,						// idxTicks0PerSecond - timer0 clock ticks per second
	1609344ul,								// idxNumerDistance - numerator to convert miles to kilometers
	3785411784ul,							// idxNumerVolume - numerator to convert US gallons to liters
	3600ul,									// idxSecondsPerHour - seconds per hour
#if defined(useTimer1Interrupt)
	t1CyclesPerSecond,						// idxCycles1PerSecond - timer1 clock cycles per second
	510ul,									// idxCycles1PerTick - known as the "N" in the (processor speed)/(N * prescaler) for timer1 phase correct PWM
	t1TicksPerSecond,						// idxTicks1PerSecond - timer1 clock ticks per second
#endif // defined(useTimer1Interrupt)
#if defined(useClockSupport)
	86400ul,								// idxSecondsPerDay - number of seconds in a day
#endif // defined(useClockSupport)
#if defined(usePressure)
	68947573ul,								// idxNumerPressure - numerator to convert psig to kPa
	correctionFactor,						// idxCorrectionFactor - correction factor used for fuel calculations
	correctionFactor * correctionFactor,	// idxCorrectionFactor2 - correction factor squared for square root function
#endif // defined(usePressure)
#if defined(useAnalogRead)
	1024ul,									// idxNumerVoltage - numerator to convert volts DC to ADC steps
	5000ul,									// idxDenomVoltage - denominator to convert volts DC to ADC steps
#endif // defined(useAnalogRead)
#if defined(useAlternatorVoltage)
	9600ul,									// idxResistanceR5 - resistor next to ground (via meelis11)
	27000ul,								// idxResistanceR6 - resistor next to diode  (via meelis11)
#endif // defined(useAlternatorVoltage)
#if defined(useVehicleParameters)
	2204622621ul,							// idxDenomMass - denominator to convert pounds to kilograms
#if defined(useCoastDownCalculator)
	9290304ul,								// idxNumerArea - numerator to convert square feet to square meters
	168555ul,								// idxDenomDensity - denominator to convert SAE density to metric density
#endif // defined(useCoastDownCalculator)
#if defined(useDragRaceFunction)
	22840ul,								// idxPowerFactor - 22.84, or vehicle speed division factor for accel test power estimation function (228.4/10 for internal calculations)
#endif // defined(useDragRaceFunction)
#endif // defined(useVehicleParameters)
#ifdef useImperialGallon
	454609ul,								// idxNumerImperialGallon - numerator to convert Imperial gallons to liters
#endif // useImperialGallon
};

// HHMMSS is always compiled (not gated on useClockSupport) so the BCD format
// indices stay stable across builds; the clock code that *uses* it is still
// gated by useClockSupport.  Costs only the 7-byte descriptor below.
enum {
	bcdFormat10digit,
	bcdFormatHHMMSS,
	bcdFormatH9MMSS,
	bcdFormatOverflow,
	bcdFormatCount
};

const uint8_t s64BCDformatList[] PROGMEM = {
	// 10 digit number format
	0x08,		// total entry length
	' ',		// leading zero character
	0x05,		// total BCD byte length / offset into 64-bit register for BCD LSB
	0x04,		// divisor string length
	100,		// 10s and 100s
	100,		// 1000s and 10000s
	100,		// 100000s and 1000000s
	100,		// 10000000s and 100000000s

	// hhmmss number format (always compiled so format indices stay build-stable)
	0x07,		// total entry length
	'0',		// leading zero character
	0x03,		// total BCD byte length / offset into 64-bit register for BCD LSB
	0x03,		// divisor string length
	60,			// seconds
	60,			// minutes
	24,			// hours

	// h9mmss number format
	0x07,		// total entry length
	'0',		// leading zero character
	0x03,		// total BCD byte length / offset into 64-bit register for BCD LSB
	0x03,		// divisor string length
	60,			// seconds
	60,			// minutes
	100,		// hours

	// overflow number format
	0x03,		// total entry length
	0x00,		// leading zero character
	0xFF,		// total BCD byte length // overflow signal

	0x00,		// total entry length == 0 for end of list
};
