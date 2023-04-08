#ifdef useIsqrt
static unsigned int iSqrt(unsigned int n);
#endif // useIsqrt
/*

 SWEET64 is a low-level pseudocode interpreter, meant to save a lot of space in program memory for 64-bit calculations

 the opcode definitions below provide means of manipulating MPGuino trip variables, interacting with calculated and EEPROM parameters, and providing basic
 number formatting for interaction with string representations of numbers

 SWEET64 is inspired by an early pseudocode processor named SWEET16, which was written by Steve Wozniak of Apple Computer back in 1977. SWEET16 was written to
    perform 16-bit machine language instructions while running on an 8-bit microprocessor

*/

namespace SWEET64 /* 64-bit pseudo-processor section prototype */
{

#ifdef useSWEET64trace
	static void listProgram(uint8_t calcIdx);
#endif // useSWEET64trace
	static uint32_t doCalculate(uint8_t tripIdx, uint8_t calcIdx);
	static uint32_t runPrgm(const uint8_t * sched, uint8_t tripIdx);
	static uint8_t fetchByte(const uint8_t * &prgmPtr);
	static void copy64(union union_64 * an, union union_64 * ann);
	static void swap64(union union_64 * an, union union_64 * ann);
	static void shr64(union union_64 * an);
	static void shl64(union union_64 * an);
	static void adc64(union union_64 * an, union union_64 * ann);
	static void sbc64(union union_64 * an, union union_64 * ann, uint8_t sbcFlag);
#ifndef useSWEET64mult
	static void mult64(void);
#endif // useSWEET64mult
#ifndef useSWEET64div
	static void div64(void);
#endif // useSWEET64div
	static void init64byt(union union_64 * an, uint8_t byt);
	static void init64(union union_64 * an, uint32_t dWordL);
	static void flagSet(uint8_t condition, uint8_t flag);
	static void flagSet64(uint8_t n, uint8_t z, uint8_t c);
	static void registerTest64(union union_64 * an);

}

static const uint8_t r00 =	0;			// do not fetch register operand
static const uint8_t r01 =	r00 + 32;	// fetch rX and rY from program
static const uint8_t r02 =	r01 + 32;	// set rX = r5
static const uint8_t r03 =	r02 + 32;	// set rX = r5, rY = r1
static const uint8_t r04 =	r03 + 32;	// fetch rP and rS from program
static const uint8_t r05 =	r04 + 32;	// set rP = r5, rS = r2
static const uint8_t r06 =	r05 + 32;	// fetch rP from program, set rS = r5, and rX to r5
static const uint8_t r07 =	r06 + 32;	//

static const uint8_t rxxMask =	0b11100000;

static const uint8_t p00 =	0;			// do not fetch primary operand
static const uint8_t p01 =	p00 + 4;	// load primary operand from program
static const uint8_t p02 =	p01 + 4;	// load primary operand from index
static const uint8_t p03 =	p02 + 4;	// load primary operand from program + index
static const uint8_t p04 =	p03 + 4;	// load primary operand with EEPROM indirect[index]
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
static const uint8_t e00 =	0;			// no operation
static const uint8_t e01 =	e00 + 1;	// branch c=1 | z=1		(bgte)
static const uint8_t e02 =	e01 + 1;	// branch c=1			(bgt)
static const uint8_t e03 =	e02 + 1;	// branch c=0			(blte)
static const uint8_t e04 =	e03 + 1;	// branch z=0			(bne)
static const uint8_t e05 =	e04 + 1;	// branch z=1			(beq)
static const uint8_t e06 =	e05 + 1;	// branch n=0			(bpl)
static const uint8_t e07 =	e06 + 1;	// branch n=1			(bmi)
static const uint8_t e08 =	e07 + 1;	// branch v=0			(bvc)
static const uint8_t e09 =	e08 + 1;	// branch v=1			(bvs)
static const uint8_t e10 =	e09 + 1;	// branch c=0 & z=0		(blt)
static const uint8_t e11 =	e10 + 1;	// branch if metric mode
static const uint8_t e12 =	e11 + 1;	// branch if SAE mode
static const uint8_t e13 =	e12 + 1;	// branch if output is fuel/distance
static const uint8_t e14 =	e13 + 1;	// branch if output is distance/fuel
static const uint8_t e15 =	e14 + 1;	// branch always
static const uint8_t e16 =	e15 + 1;	// return
static const uint8_t e17 =	e16 + 1;	// return and restore trace flag
static const uint8_t e18 =	e17 + 1;	// test index
static const uint8_t e19 =	e18 + 1;	// trace restore
static const uint8_t e20 =	e19 + 1;	// trace on
static const uint8_t e21 =	e20 + 1;	// trace save
static const uint8_t e22 =	e21 + 1;	// trace off
static const uint8_t e23 =	e22 + 1;	// load index
static const uint8_t e24 =	e23 + 1;	// load index eeprom
static const uint8_t e25 =	e24 + 1;	// compare index
static const uint8_t e26 =	e25 + 1;	// load index eeprom parameter length
static const uint8_t e27 =	e26 + 1;	// call
static const uint8_t e28 =	e27 + 1;	// jump
static const uint8_t e29 =	e28 + 1;	// load jump register
static const uint8_t e30 =	e29 + 1;	// clear register flag
static const uint8_t e31 =	e30 + 1;	// set register flag

// operations where opcode prefix rxx is not r00
static const uint8_t m00 =	0;			// no operation
static const uint8_t m01 =	m00 + 1;	// add			rP = rP + rS
static const uint8_t m02 =	m01 + 1;	// sub			rP = rP - rS
static const uint8_t m03 =	m02 + 1;	// compare		na = rP - rS
static const uint8_t m04 =	m03 + 1;	// test			r5
static const uint8_t m05 =	m04 + 1;	// multiply		r2 = r2 * r5
static const uint8_t m06 =	m05 + 1;	// divide		r2 = r2 / r5 rmdr r1 and qadj r5
static const uint8_t m07 =	m06 + 1;	//

static const uint8_t mxxMask = 0b00000111;

static const uint8_t i00 =	0;			// no operation
static const uint8_t i01 =	i00 + 8;	// load rX with byte
static const uint8_t i02 =	i01 + 8;	// load rX with rY
static const uint8_t i03 =	i02 + 8;	// load rX with eeprom
static const uint8_t i04 =	i03 + 8;	// load rX with main program
static const uint8_t i05 =	i04 + 8;	// load rX with volatile
static const uint8_t i06 =	i05 + 8;	// load rX with trip variable
static const uint8_t i07 =	i06 + 8;	// load rX with const
static const uint8_t i08 =	i07 + 8;	// load rX with eeprom init
static const uint8_t i09 =	i08 + 8;	// load rX with voltage
static const uint8_t i10 =	i09 + 8;	// load rX with numerator for constant
static const uint8_t i11 =	i10 + 8;	// integer square root
static const uint8_t i12 =	i11 + 8;	// load rX with denominator for constant
static const uint8_t i13 =	i12 + 8;	//
static const uint8_t i14 =	i13 + 8;	//
static const uint8_t i15 =	i14 + 8;	// swap rX rY
static const uint8_t i16 =	i15 + 8;	//
static const uint8_t i17 =	i16 + 8;	// store eeprom rX
static const uint8_t i18 =	i17 + 8;	// store volatile rX
static const uint8_t i19 =	i18 + 8;	// store main program rX
static const uint8_t i20 =	i19 + 8;	// store trip variable rX
static const uint8_t i21 =	i20 + 8;	//
static const uint8_t i22 =	i21 + 8;	//
static const uint8_t i23 =	i22 + 8;	// load rX with FEvT trip variable
static const uint8_t i24 =	i23 + 8;	// shift rX left
static const uint8_t i25 =	i24 + 8;	// shift rX right
static const uint8_t i26 =	i25 + 8;	// BCD adjust
static const uint8_t i27 =	i26 + 8;	// store rX byte to bargraph data array
static const uint8_t i28 =	i27 + 8;	// load rX with byte[operand]
static const uint8_t i29 =	i28 + 8;	//
static const uint8_t i30 =	i29 + 8;	//
static const uint8_t i31 =	i30 + 8;	//

static const uint8_t ixxMask = 0b11111000;
uint8_t SWEET64processorFlags;

// x < y        - carry flag is clear, zero flag is clear
// x == y       - carry flag is set, zero flag is set
// x > y        - carry flag is set, zero flag is clear
//
// (x - y) >= 0 - minus flag is clear
// (x - y) < 0	- minus flag is set
//
const uint8_t SWEET64carryFlag =			0b00000001;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64zeroFlag =				0b00000010;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64minusFlag =			0b00000100;			// this is set for arithmetic and branch test operations
const uint8_t SWEET64overflowFlag =			0b00001000;			// this is set for arithmetic and branch test operations

const uint8_t SWEET64traceCommandFlag =		0b01000000;			// this flag is ignored if #useSWEET64trace is not used
const uint8_t SWEET64traceFlag =			0b10000000;			// this flag is ignored if #useSWEET64trace is not used

const uint8_t SWEET64traceFlagGroup =	SWEET64traceCommandFlag | SWEET64traceFlag;

#ifdef useDebugTerminal
uint64_t s64reg[6];
#else // useDebugTerminal
uint64_t s64reg[5];
#endif // useDebugTerminal

static const uint8_t s64reg1 = 0;	// general purpose
static const uint8_t s64reg2 = 1;	// output value / general purpose
static const uint8_t s64reg3 = 2;	// general purpose / temporary storage
static const uint8_t s64reg4 = 3;	// used in multiply, divide operations
static const uint8_t s64reg5 = 4;	// used in multiply, divide operations
#ifdef useDebugTerminal
static const uint8_t s64reg6 = 5;	// used for debug terminal
#endif // useDebugTerminal

#define nextAllowedValue 0
static const uint8_t instrTestReg =					nextAllowedValue;						// tests 64-bit register for zero condition or high bit set
static const uint8_t instrTestIndex =				instrTestReg + 1;						// tests primary index for zero condition or high bit set
static const uint8_t instrCmpXtoY =					instrTestIndex + 1;						// compares 64-bit register Y from 64-bit register X
static const uint8_t instrCmpIndex =				instrCmpXtoY + 1;						// compares primary index from operand

static const uint8_t instrBranchIfVclear =			instrCmpIndex + 1;						// branches if result[0..63] is not all 1s
static const uint8_t instrBranchIfVset =			instrBranchIfVclear + 1;				// branches if result[0..63] is all 1s
static const uint8_t instrBranchIfMclear =			instrBranchIfVset + 1;					// branches if result most significant bit is clear
static const uint8_t instrBranchIfMset =			instrBranchIfMclear + 1;				// branches if result most significant bit is set
static const uint8_t instrBranchIfZclear =			instrBranchIfMset + 1;					// branches if result zero flag is clear (X != Y or X != 0)
static const uint8_t instrBranchIfZset =			instrBranchIfZclear + 1;				// branches if result zero flag is set (X == Y or X == 0)
static const uint8_t instrBranchIfCclear =			instrBranchIfZset + 1;					// branches if result carry flag is clear (X <= Y)
static const uint8_t instrBranchIfCset =			instrBranchIfCclear + 1;				// branches if result carry flag is set (X > Y)
static const uint8_t instrBranchIfLT =				instrBranchIfCset + 1;					// branches if X < Y or index < operand
static const uint8_t instrBranchIfGTorE =			instrBranchIfLT + 1;					// branches if X >= Y or index >= operand
static const uint8_t instrBranchIfMetricMode =		instrBranchIfGTorE + 1;					// branches if MPGuino is in metric mode
static const uint8_t instrBranchIfSAEmode =			instrBranchIfMetricMode + 1;			// branches if MPGuino is in SAE mode
static const uint8_t instrBranchIfFuelOverDist =	instrBranchIfSAEmode + 1;				// branches if MPGuino outputs fuel/distance
static const uint8_t instrBranchIfDistOverFuel =	instrBranchIfFuelOverDist + 1;			// branches if MPGuino outputs distance/fuel

static const uint8_t instrSkip =					instrBranchIfDistOverFuel + 1;			// branches always
static const uint8_t instrCall =					instrSkip + 1;							// push current program counter onto stack, then call indexed routine
static const uint8_t instrCallImplied =				instrCall + 1;							// push current program counter onto stack, then call indexed routine stored in jump register
static const uint8_t instrJump =					instrCallImplied + 1;					// jump to indexed routine

/*

	operands specifying 64-bit registers take the form of YX as a hexadecimal value
	
	e.g., instrLdReg, 0x24,

	means "load 64-bit register 4 with the contents of 64-bit register 2"

*/
static const uint8_t instrLdReg =					instrJump + 1;							// load 64-bit register X with contents of 64-bit register Y
static const uint8_t instrLdRegByteFromIndex =		instrLdReg + 1;							// load 64-bit register X with primary index byte value
static const uint8_t instrLdRegByte =				instrLdRegByteFromIndex + 1;			// load 64-bit register X with immediate byte value	
static const uint8_t instrLdRegByteFromY =			instrLdRegByte + 1;						// load 64-bit register X with value of specified byte index into 64-bit register Y
static const uint8_t instrLdRegTripVar =			instrLdRegByteFromY + 1;				// load 64-bit register X with specified trip specified read-in register
static const uint8_t instrLdRegTripVarIndexed =		instrLdRegTripVar + 1;					// load 64-bit register X with indexed trip specified read-in register
static const uint8_t instrLdRegTripVarOffset =		instrLdRegTripVarIndexed + 1;			// load 64-bit register X with offset indexed trip specified read-in register
static const uint8_t instrLdRegTripVarIndexedRV =	instrLdRegTripVarOffset + 1;			// load 64-bit register X with specified trip indexed read-in register
static const uint8_t instrStRegTripVarIndexed =		instrLdRegTripVarIndexedRV + 1;			// store 64-bit register X value to indexed trip specified read-in register
static const uint8_t instrStRegTripVarIndexedRV =	instrStRegTripVarIndexed + 1;			// store 64-bit register X value to specified trip indexed read-in register
static const uint8_t instrLdRegConst =				instrStRegTripVarIndexedRV + 1;			// load 64-bit register X with stored constant value
static const uint8_t instrLdRegConstIndexed =		instrLdRegConst + 1;					// load 64-bit register X with indexed stored constant value
static const uint8_t instrDoBCDadjust =				instrLdRegConstIndexed + 1;					// perform BCD-style conversion of 64-bit register X, using format stored in 64-bit register 3
static const uint8_t instrLdRegEEPROM =				instrDoBCDadjust + 1;					// load 64-bit register X with EEPROM parameter value
static const uint8_t instrLdRegEEPROMindexed =		instrLdRegEEPROM + 1;					// load 64-bit register X with indexed EEPROM parameter value
static const uint8_t instrLdRegEEPROMindirect =		instrLdRegEEPROMindexed + 1;			// load 64-bit register X with value of EEPROM parameter indexed in the SAE/metric reference conversion table
static const uint8_t instrLdRegEinit =				instrLdRegEEPROMindirect + 1;			// load 64-bit register X with initial EEPROM parameter value
static const uint8_t instrLdRegEinitIndexed =		instrLdRegEinit + 1;					// load 64-bit register X with indexed initial EEPROM parameter value
static const uint8_t instrStRegEEPROM =				instrLdRegEinitIndexed + 1;				// store 64-bit register X value to EEPROM parameter
static const uint8_t instrStRegEEPROMindexed =		instrStRegEEPROM + 1;					// store 64-bit register X value to indexed EEPROM parameter
static const uint8_t instrStRegEEPROMindirect =		instrStRegEEPROMindexed + 1;			// store 64-bit register X value to EEPROM parameter indexed in the SAE/metric reference conversion table
static const uint8_t instrLdRegMain =				instrStRegEEPROMindirect + 1;			// load 64-bit register X with main program register value
static const uint8_t instrLdRegMainIndexed =		instrLdRegMain + 1;						// load 64-bit register X with indexed main program register value
static const uint8_t instrLdRegMainOffset =			instrLdRegMainIndexed + 1;				// load 64-bit register X with offset indexed main program register value
static const uint8_t instrStRegMain =				instrLdRegMainOffset + 1;				// store 64-bit register X value to main program register
static const uint8_t instrStRegMainIndexed =		instrStRegMain + 1;						// store 64-bit register X value to indexed main program register
static const uint8_t instrLdRegVolatile =			instrStRegMainIndexed + 1;				// load 64-bit register X with volatile register value
static const uint8_t instrLdRegVolatileIndexed =	instrLdRegVolatile + 1;					// load 64-bit register X with indexed volatile register value
static const uint8_t instrStRegVolatile =			instrLdRegVolatileIndexed + 1;			// store 64-bit register X value to volatile register
static const uint8_t instrStRegVolatileIndexed =	instrStRegVolatile + 1;					// store 64-bit register X value to indexed volatile register
static const uint8_t instrLxdI =					instrStRegVolatileIndexed + 1;			// load primary index register with immediate byte value
static const uint8_t instrLxdIEEPROM =				instrLxdI + 1;							// load primary index register with EEPROM parameter value
static const uint8_t instrLxdIEEPROMoffset =		instrLxdIEEPROM + 1;					// load primary index register with offset indexed EEPROM parameter
static const uint8_t instrLxdIParamLength =			instrLxdIEEPROMoffset + 1;				// load primary index register with bit length of EEPROM parameter
static const uint8_t instrLxdIParamLengthIndexed =	instrLxdIParamLength + 1;				// load primary index register with bit length of indexed EEPROM parameter
static const uint8_t instrLdRegNumer =				instrLxdIParamLengthIndexed + 1;		// load 64-bit register X with constant numerator (SAE mode) or denominator (metric mode)
static const uint8_t instrLdRegDenom =				instrLdRegNumer + 1;					// load 64-bit register X with constant denominator (SAE mode) or numerator (metric mode)
static const uint8_t instrSwapReg =					instrLdRegDenom + 1;					// swap contents of 64-bit registers X and Y
static const uint8_t instrSubYfromX =				instrSwapReg + 1;						// subtract 64-bit register Y from 64-bit register X
static const uint8_t instrSubMainFromX =			instrSubYfromX + 1;						// subtract main program register value from 64-bit register X
static const uint8_t instrAddYtoX =					instrSubMainFromX + 1;					// add 64-bit register Y to 64-bit register X
static const uint8_t instrAdjustQuotient =			instrAddYtoX + 1;						// bumps 64-bit register 2 by 1 if last division resulted in remainder * 2 > divisor
static const uint8_t instrAddByteToX =				instrAdjustQuotient + 1;				// add immediate byte value to 64-bit register X
static const uint8_t instrAddConstToX =				instrAddByteToX + 1;					// add stored constant value to 64-bit register X
static const uint8_t instrAddEEPROMtoX =			instrAddConstToX + 1;					// add EEPROM parameter value to 64-bit register X
static const uint8_t instrAddMainToX =				instrAddEEPROMtoX + 1;					// add main program register value to 64-bit register X
static const uint8_t instrAddIndexToX =				instrAddMainToX + 1;					// add contents of primary index to 64-bit register X
static const uint8_t instrMul2by1 =					instrAddIndexToX + 1;					// multiply 64-bit register 2 by contents of 64-bit register 1
static const uint8_t instrMul2byByte =				instrMul2by1 + 1;    					// multiply 64-bit register 2 by immediate byte value
static const uint8_t instrMul2byConst =				instrMul2byByte + 1; 					// multiply 64-bit register 2 by stored constant value
static const uint8_t instrMul2byEEPROM =			instrMul2byConst + 1; 					// multiply 64-bit register 2 by EEPROM parameter value
static const uint8_t instrMul2byMain =				instrMul2byEEPROM + 1;					// multiply 64-bit register 2 by main program register value
static const uint8_t instrMul2byVolatile =			instrMul2byMain + 1;					// multiply 64-bit register 2 by volatile register value
static const uint8_t instrMul2byTripVarIndexed =	instrMul2byVolatile + 1;				// multiply 64-bit register 2 by indexed trip specified read-in register
static const uint8_t instrDiv2by1 =					instrMul2byTripVarIndexed + 1;			// divide 64-bit register 2 by contents of 64-bit register 1
static const uint8_t instrDiv2byConst =				instrDiv2by1 + 1;						// divide 64-bit register 2 by stored constant value
static const uint8_t instrDiv2byEEPROM =			instrDiv2byConst + 1;					// divide 64-bit register 2 by EEPROM parameter value
static const uint8_t instrDiv2byMain =				instrDiv2byEEPROM + 1;					// divide 64-bit register 2 by main program register value
static const uint8_t instrDiv2byTripVarIndexed =	instrDiv2byMain + 1;					// divide 64-bit register 2 by indexed trip specified read-in register
static const uint8_t instrDiv2byByte =				instrDiv2byTripVarIndexed + 1;			// divide 64-bit register 2 by immediate byte value
static const uint8_t instrShiftRegLeft =			instrDiv2byByte + 1;					// shift 64-bit register X one bit left
static const uint8_t instrShiftRegRight =			instrShiftRegLeft + 1;					// shift 64-bit register X one bit right
static const uint8_t instrAddIndex =				instrShiftRegRight + 1;					// add immediate byte value to primary index register
static const uint8_t instrTraceOn =					instrAddIndex + 1;						// turn on SWEET64 trace
static const uint8_t instrTraceOff =				instrTraceOn + 1;						// turn off SWEET64 trace
static const uint8_t instrTraceSave =				instrTraceOff + 1;						// save status of SWEET64 trace, then turn off
static const uint8_t instrTraceRestore =			instrTraceSave + 1;						// restore status of SWEET64 trace
static const uint8_t instrTraceDone =				instrTraceRestore + 1;					// restore status of SWEET64 trace, then return to caller
static const uint8_t instrLdJumpReg =				instrTraceDone + 1;						// load jump register with routine index value
static const uint8_t instrClearFlag =				instrLdJumpReg + 1;						// clear SWEET64 status flag
static const uint8_t instrSetFlag =					instrClearFlag + 1;						// set SWEET64 status flag
#define nextAllowedValue instrSetFlag + 1

#ifdef useIsqrt
static const uint8_t instrIsqrt =					nextAllowedValue;						// perform integer square root on 64-bit register
#define nextAllowedValue instrIsqrt + 1
#endif // useIsqrt

#if defined(useAnalogRead)
static const uint8_t instrLdRegVoltage =			nextAllowedValue;						// load 64-bit register with specified raw 10-bit analog voltage value
static const uint8_t instrLdRegVoltageIndexed =		instrLdRegVoltage + 1;					// load 64-bit register with indexed raw 10-bit analog voltage value
#define nextAllowedValue instrLdRegVoltageIndexed + 1
#endif // useAnalogRead

#ifdef useBarFuelEconVsTime
static const uint8_t instrLdRegTripFEvTindexed =	nextAllowedValue;						// load trip index from fuel econ vs time trip array
#define nextAllowedValue instrLdRegTripFEvTindexed + 1
#endif // useBarFuelEconVsTime

#ifdef useBarGraph
static const uint8_t instrStRegBGdataIndexed =		nextAllowedValue;							// store register byte to bargraph data array
#define nextAllowedValue instrStRegBGdataIndexed + 1
#endif // useBarFuelEconVsTime

static const uint8_t instrDone =					nextAllowedValue;						// return to caller - if at topmost level of stack, return to main program
#define nextAllowedValue instrDone + 1

static const uint8_t maxValidSWEET64instr =			nextAllowedValue;

#ifdef useSWEET64trace
static const char opcodeList[] PROGMEM = {
	"instrTestReg\r"
	"instrTestIndex\r"
	"instrCmpXtoY\r"
	"instrCmpIndex\r"
	"instrBranchIfMclear\r"
	"instrBranchIfMset\r"
	"instrBranchIfZclear\r"
	"instrBranchIfZset\r"
	"instrBranchIfCclear\r"
	"instrBranchIfCset\r"
	"instrBranchIfLT\r"
	"instrBranchIfGTorE\r"
	"instrBranchIfMetricMode\r"
	"instrBranchIfSAEmode\r"
	"instrBranchIfFuelOverDist\r"
	"instrBranchIfDistOverFuel\r"
	"instrSkip\r"
	"instrLdReg\r"
	"instrLdRegByteFromIndex\r"
	"instrLdRegByte\r"
	"instrLdRegByteFromY\r"
	"instrLdRegTripVar\r"
	"instrLdRegTripVarIndexed\r"
	"instrLdRegTripVarOffset\r"
	"instrLdRegTripVarIndexedRV\r"
	"instrStRegTripVarIndexed\r"
	"instrStRegTripVarIndexedRV\r"
	"instrLdRegConst\r"
	"instrDoBCDadjust\r"
	"instrLdRegEEPROM\r"
	"instrLdRegEEPROMindexed\r"
	"instrLdRegEEPROMindirect\r"
	"instrLdRegEinit\r"
	"instrLdRegEinitIndexed\r"
	"instrStRegEEPROM\r"
	"instrStRegEEPROMindexed\r"
	"instrStRegEEPROMindirect\r"
	"instrLdRegMain\r"
	"instrLdRegMainIndexed\r"
	"instrLdRegMainOffset\r"
	"instrStRegMain\r"
	"instrStRegMainIndexed\r"
	"instrLdRegVolatile\r"
	"instrLdRegVolatileIndexed\r"
	"instrStRegVolatile\r"
	"instrStRegVolatileIndexed\r"
	"instrLxdI\r"
	"instrLxdIEEPROM\r"
	"instrLxdIEEPROMindexed\r"
	"instrLxdIParamLength\r"
	"instrLxdIParamLengthIndexed\r"
	"instrLdRegNumer\r"
	"instrLdRegDenom\r"
	"instrCall\r"
	"instrJump\r"
	"instrSwapReg\r"
	"instrSubYfromX\r"
	"instrSubMainFromX\r"
	"instrAddYtoX\r"
	"instrAdjustQuotient\r"
	"instrAddByteToX\r"
	"instrAddConstToX\r"
	"instrAddEEPROMtoX\r"
	"instrAddMainToX\r"
	"instrAddIndexToX\r"
	"instrMul2by1\r"
	"instrMul2byByte\r"
	"instrMul2byConst\r"
	"instrMul2byEEPROM\r"
	"instrMul2byMain\r"
	"instrMul2byVolatile\r"
	"instrMul2byTripVarIndexed\r"
	"instrDiv2by1\r"
	"instrDiv2byConst\r"
	"instrDiv2byEEPROM\r"
	"instrDiv2byMain\r"
	"instrDiv2byTripVarIndexed\r"
	"instrDiv2byByte\r"
	"instrShiftRegLeft\r"
	"instrShiftRegRight\r"
	"instrAddIndex\r"
	"instrTraceOn\r"
	"instrTraceOff\r"
	"instrTraceSave\r"
	"instrTraceRestore\r"
	"instrTraceDone\r"
	"instrLdJumpReg\r"
	"instrClearFlag\r"
	"instrSetFlag\r"
#ifdef useIsqrt
	"instrIsqrt\r"
#endif // useIsqrt
#if defined(useAnalogRead)
	"instrLdRegVoltage\r"
	"instrLdRegVoltageIndexed\r"
#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
	"instrLdRegTripFEvTindexed\r"
#endif // useBarFuelEconVsTime
#ifdef useBarGraph
	"instrStRegBGdataIndexed\r"
#endif // useBarGraph
	"instrDone\r"
};

#endif // useSWEET64trace
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

static const uint8_t opcodeFetchPrefix[(uint16_t)(maxValidSWEET64instr)] PROGMEM = {
	 r04 | p00 | s00	// instrTestReg
	,r00 | p00 | s00	// instrTestIndex
	,r04 | p00 | s00	// instrCmpXtoY
	,r00 | p01 | s00	// instrCmpIndex
	,r00 | p00 | s01	// instrBranchIfVclear
	,r00 | p00 | s01	// instrBranchIfVset
	,r00 | p00 | s01	// instrBranchIfMclear
	,r00 | p00 | s01	// instrBranchIfMset
	,r00 | p00 | s01	// instrBranchIfZclear
	,r00 | p00 | s01	// instrBranchIfZset
	,r00 | p00 | s01	// instrBranchIfCclear
	,r00 | p00 | s01	// instrBranchIfCset
	,r00 | p00 | s01	// instrBranchIfLT
	,r00 | p00 | s01	// instrBranchIfGTorE
	,r00 | p00 | s01	// instrBranchIfMetricMode
	,r00 | p00 | s01	// instrBranchIfSAEmode
	,r00 | p00 | s01	// instrBranchIfFuelOverDist
	,r00 | p00 | s01	// instrBranchIfDistOverFuel
	,r00 | p00 | s01	// instrSkip
	,r00 | p00 | s01	// instrCall
	,r00 | p00 | s03	// instrCallImplied
	,r00 | p00 | s01	// instrJump
	,r01 | p00 | s00	// instrLdReg
	,r01 | p02 | s00	// instrLdRegByteFromIndex
	,r01 | p01 | s00	// instrLdRegByte
	,r01 | p01 | s00	// instrLdRegByteFromY
	,r04 | p01 | s01	// instrLdRegTripVar
	,r04 | p02 | s01	// instrLdRegTripVarIndexed
	,r04 | p03 | s01	// instrLdRegTripVarOffset
	,r04 | p01 | s02	// instrLdRegTripVarIndexedRV
	,r01 | p02 | s01	// instrStRegTripVarIndexed
	,r01 | p01 | s02	// instrStRegTripVarIndexedRV
	,r01 | p01 | s00	// instrLdRegConst
	,r01 | p02 | s00	// instrLdRegConstIndexed
	,r01 | p01 | s00	// instrDoBCDadjust
	,r01 | p01 | s00	// instrLdRegEEPROM
	,r01 | p02 | s00	// instrLdRegEEPROMindexed
	,r01 | p04 | s00	// instrLdRegEEPROMindirect
	,r01 | p01 | s00	// instrLdRegEinit
	,r01 | p02 | s00	// instrLdRegEinitIndexed
	,r01 | p01 | s00	// instrStRegEEPROM
	,r01 | p02 | s00	// instrStRegEEPROMindexed
	,r01 | p04 | s00	// instrStRegEEPROMindirect
	,r01 | p01 | s00	// instrLdRegMain
	,r01 | p02 | s00	// instrLdRegMainIndexed
	,r01 | p03 | s00	// instrLdRegMainOffset
	,r01 | p01 | s00	// instrStRegMain
	,r01 | p02 | s00	// instrStRegMainIndexed
	,r01 | p01 | s00	// instrLdRegVolatile
	,r01 | p02 | s00	// instrLdRegVolatileIndexed
	,r01 | p01 | s00	// instrStRegVolatile
	,r01 | p02 | s00	// instrStRegVolatileIndexed
	,r00 | p01 | s00	// instrLxdI
	,r00 | p01 | s00	// instrLxdIEEPROM
	,r00 | p03 | s00	// instrLxdIEEPROMoffset
	,r00 | p01 | s00	// instrLxdIParamLength
	,r00 | p02 | s00	// instrLxdIParamLengthIndexed
	,r01 | p02 | s00	// instrLdRegNumer
	,r01 | p02 | s00	// instrLdRegDenom
	,r01 | p00 | s00	// instrSwapReg
	,r04 | p00 | s00	// instrSubYfromX
	,r06 | p01 | s00	// instrSubMainFromX
	,r04 | p00 | s00	// instrAddYtoX
	,r05 | p00 | s00	// instrAdjustQuotient
	,r06 | p01 | s00	// instrAddByteToX
	,r06 | p01 | s00	// instrAddConstToX
	,r06 | p01 | s00	// instrAddEEPROMtoX
	,r06 | p01 | s00	// instrAddMainToX
	,r06 | p02 | s00	// instrAddIndexToX
	,r03 | p00 | s00	// instrMul2by1
	,r02 | p01 | s00	// instrMul2byByte
	,r02 | p01 | s00	// instrMul2byConst
	,r02 | p01 | s00	// instrMul2byEEPROM
	,r02 | p01 | s00	// instrMul2byMain
	,r02 | p01 | s00	// instrMul2byVolatile
	,r02 | p02 | s01	// instrMul2byTripVarIndexed
	,r03 | p00 | s00	// instrDiv2by1
	,r02 | p01 | s00	// instrDiv2byConst
	,r02 | p01 | s00	// instrDiv2byEEPROM
	,r02 | p01 | s00	// instrDiv2byMain
	,r02 | p02 | s01	// instrDiv2byTripVarIndexed
	,r02 | p01 | s00	// instrDiv2byByte
	,r01 | p00 | s00	// instrShiftRegLeft
	,r01 | p00 | s00	// instrShiftRegRight
	,r00 | p03 | s00	// instrAddIndex
	,r00 | p00 | s00	// instrTraceOn
	,r00 | p00 | s00	// instrTraceOff
	,r00 | p00 | s00	// instrTraceSave
	,r00 | p00 | s00	// instrTraceRestore
	,r00 | p00 | s00	// instrTraceDone
	,r00 | p02 | s00	// instrLdJumpReg
	,r00 | p01 | s00	// instrClearFlag
	,r00 | p01 | s00	// instrSetFlag
#ifdef useIsqrt
	,r01 | p00 | s00	// instrIsqrt
#endif // useIsqrt
#if defined(useAnalogRead)
	,r01 | p01 | s00	// instrLdRegVoltage
	,r01 | p02 | s00	// instrLdRegVoltageIndexed
#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
	,r04 | p02 | s01	// instrLdRegTripFEvTindexed
#endif // useBarFuelEconVsTime
#ifdef useBarGraph
	,r01 | p02 | s00	// instrStRegBGdataIndexed
#endif // useBarGraph
	,r00 | p00 | s00	// instrDone
};

static const uint8_t opcodeFetchSuffix[(uint16_t)(maxValidSWEET64instr)] PROGMEM = {
	 m04 | i00			// instrTestReg
	,e18				// instrTestIndex
	,m03 | i00			// instrCmpXtoY
	,e25				// instrCmpIndex
	,e08				// instrBranchIfVclear
	,e09				// instrBranchIfVset
	,e06				// instrBranchIfMclear
	,e07				// instrBranchIfMset
	,e04				// instrBranchIfZclear
	,e05				// instrBranchIfZset
	,e03				// instrBranchIfCclear
	,e02				// instrBranchIfCset
	,e10				// instrBranchIfLT
	,e01				// instrBranchIfGTorE
	,e11				// instrBranchIfMetricMode
	,e12				// instrBranchIfSAEmode
	,e13				// instrBranchIfFuelOverDist
	,e14				// instrBranchIfDistOverFuel
	,e15				// instrSkip
	,e27				// instrCall
	,e27				// instrCallImplied
	,e28				// instrJump
	,m00 | i02			// instrLdReg
	,m00 | i01			// instrLdRegByteFromIndex
	,m00 | i01			// instrLdRegByte
	,m00 | i28			// instrLdRegByteFromY
	,m04 | i06			// instrLdRegTripVar
	,m04 | i06			// instrLdRegTripVarIndexed
	,m04 | i06			// instrLdRegTripVarOffset
	,m04 | i06			// instrLdRegTripVarIndexedRV
	,m00 | i20			// instrStRegTripVarIndexed
	,m00 | i20			// instrStRegTripVarIndexedRV
	,m00 | i07			// instrLdRegConst
	,m00 | i07			// instrLdRegConstIndexed
	,m00 | i26			// instrDoBCDadjust
	,m00 | i03			// instrLdRegEEPROM
	,m00 | i03			// instrLdRegEEPROMindexed
	,m00 | i03			// instrLdRegEEPROMindirect
	,m00 | i08			// instrLdRegEinit
	,m00 | i08			// instrLdRegEinitIndexed
	,m00 | i17			// instrStRegEEPROM
	,m00 | i17			// instrStRegEEPROMindexed
	,m00 | i17			// instrStRegEEPROMindirect
	,m00 | i04			// instrLdRegMain
	,m00 | i04			// instrLdRegMainIndexed
	,m00 | i04			// instrLdRegMainOffset
	,m00 | i19			// instrStRegMain
	,m00 | i19			// instrStRegMainIndexed
	,m00 | i05			// instrLdRegVolatile
	,m00 | i05			// instrLdRegVolatileIndexed
	,m00 | i18			// instrStRegVolatile
	,m00 | i18			// instrStRegVolatileIndexed
	,e23				// instrLxdI
	,e24				// instrLxdIEEPROM
	,e24				// instrLxdIEEPROMoffset
	,e26				// instrLxdIParamLength
	,e26				// instrLxdIParamLengthIndexed
	,m00 | i10			// instrLdRegNumer
	,m00 | i12			// instrLdRegDenom
	,m00 | i15			// instrSwapReg
	,m02 | i00			// instrSubYfromX
	,m02 | i04			// instrSubMainFromX
	,m01 | i00			// instrAddYtoX
	,m01 | i00			// instrAdjustQuotient
	,m01 | i01			// instrAddByteToX
	,m01 | i07			// instrAddConstToX
	,m01 | i03			// instrAddEEPROMtoX
	,m01 | i04			// instrAddMainToX
	,m01 | i01			// instrAddIndexToX
	,m05 | i02			// instrMul2by1
	,m05 | i01			// instrMul2byByte
	,m05 | i07			// instrMul2byConst
	,m05 | i03			// instrMul2byEEPROM
	,m05 | i04			// instrMul2byMain
	,m05 | i05			// instrMul2byVolatile
	,m05 | i06			// instrMul2byTripVarIndexed
	,m06 | i02			// instrDiv2by1
	,m06 | i07			// instrDiv2byConst
	,m06 | i03			// instrDiv2byEEPROM
	,m06 | i04			// instrDiv2byMain
	,m06 | i06			// instrDiv2byTripVarIndexed
	,m06 | i01			// instrDiv2byByte
	,m00 | i24			// instrShiftRegLeft
	,m00 | i25			// instrShiftRegRight
	,e23				// instrAddIndex
	,e20				// instrTraceOn
	,e22				// instrTraceOff
	,e21				// instrTraceSave
	,e19				// instrTraceRestore
	,e17				// instrTraceDone
	,e29				// instrLdJumpReg
	,e30				// instrClearFlag
	,e31				// instrSetFlag
#ifdef useIsqrt
	,m00 | i11			// instrIsqrt
#endif // useIsqrt
#if defined(useAnalogRead)
	,m00 | i09			// instrLdRegVoltage
	,m00 | i09			// instrLdRegVoltageIndexed
#endif // useAnalogRead
#ifdef useBarFuelEconVsTime
	,m04 | i23			// instrLdRegTripFEvTindexed
#endif // useBarFuelEconVsTime
#ifdef useBarGraph
	,m00 | i27			// instrStRegBGdataIndexed
#endif // useBarGraph
	,e16				// instrDone
};

// SWEET64 stored parameter conversion list
//
const uint8_t convIdx[] PROGMEM = {
	 pPulsesPerDistanceIdx
	,pMinGoodSpeedidx
	,pTankSizeIdx
	,pTankBingoSizeIdx
#ifdef usePartialRefuel
	,pRefuelSizeIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,pDragSpeedIdx
	,pDragDistanceIdx
#endif // useDragRaceFunction
#ifdef useVehicleMass
	,pVehicleMassIdx
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,pVehicleFrontalAreaIdx
	,pLocustDensityIdx
#endif // useCoastDownCalculator
#ifdef useCalculatedFuelFactor
	,pSysFuelPressureIdx
	,pRefFuelPressureIdx
#endif // useCalculatedFuelFactor
#ifdef useChryslerMAPCorrection
	,pMAPsensorRangeIdx
	,pMAPsensorOffsetIdx
#ifdef useChryslerBaroSensor
	,pBaroSensorRangeIdx
	,pBaroSensorOffsetIdx
#else // useChryslerBaroSensor
	,pBarometricPressureIdx
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	,pBarLowSpeedCutoffIdx
	,pBarSpeedQuantumIdx
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,pCostPerQuantity
#endif // useFuelCost
};

const uint8_t convSize = (sizeof(convIdx) / sizeof(uint8_t));

// indexes into SWEET64 conversion factor value table
//
#define nextAllowedValue 0
const uint8_t idxNumerDistance =			nextAllowedValue;
const uint8_t idxDenomDistance =			idxNumerDistance + 1;
const uint8_t idxNumerVolume =				idxDenomDistance + 1;
const uint8_t idxDenomVolume =				idxNumerVolume + 1;
const uint8_t idxCycles0PerSecond =			idxDenomVolume + 1;
const uint8_t idxCycles0PerTick =			idxCycles0PerSecond + 1;		// known as the "N" in the (processor speed)/(N * prescaler) for timer0 fast PWM mode
const uint8_t idxTicksPerSecond =			idxCycles0PerTick + 1;
const uint8_t idxMicroSecondsPerSecond =	idxTicksPerSecond + 1;
const uint8_t idxDecimalPoint =				idxMicroSecondsPerSecond + 1;
const uint8_t idxMetricFE =					idxDecimalPoint + 1;
const uint8_t idxSecondsPerHour =			idxMetricFE + 1;
const uint8_t idxNumber500 =				idxSecondsPerHour + 1;
const uint8_t idxBCDdivisor =				idxNumber500 + 1;
#define nextAllowedValue idxBCDdivisor + 1
#ifdef useIsqrt
const uint8_t idxNumerPressure =			nextAllowedValue;
const uint8_t idxDenomPressure =			idxNumerPressure + 1;
#define nextAllowedValue idxDenomPressure + 1
#endif // useIsqrt
#if defined(useAnalogRead)
const uint8_t idxNumerVoltage =				nextAllowedValue;
const uint8_t idxDenomVoltage =				idxNumerVoltage + 1;
#define nextAllowedValue idxDenomVoltage + 1
#endif // useAnalogRead
#ifdef useCarVoltageOutput
const uint8_t idxResistanceR5 =				nextAllowedValue;		// resistor next to ground
const uint8_t idxResistanceR6 =				idxResistanceR5 + 1;	// resistor next to diode
#define nextAllowedValue idxResistanceR6 + 1
#endif // useCarVoltageOutput
#ifdef useVehicleMass
const uint8_t idxNumerMass =				nextAllowedValue;
const uint8_t idxDenomMass =				idxNumerMass + 1;
#define nextAllowedValue idxDenomMass + 1
#endif // useVehicleMass
#ifdef useCoastDownCalculator
const uint8_t idxNumerArea =				nextAllowedValue;
const uint8_t idxDenomArea =				idxNumerArea + 1;
const uint8_t idxNumerDensity =				idxDenomArea + 1;
const uint8_t idxDenomDensity =				idxNumerDensity + 1;
#define nextAllowedValue idxDenomDensity + 1
#endif // useCoastDownCalculator
#ifdef useClockDisplay
const uint8_t idxSecondsPerDay =			nextAllowedValue;
#define nextAllowedValue idxSecondsPerDay + 1
#endif // useClockDisplay
#ifdef useIsqrt
const uint8_t idxCorrectionFactor =			nextAllowedValue;
#define nextAllowedValue idxCorrectionFactor + 1
#endif // useIsqrt
#ifdef useImperialGallon
const uint8_t idxNumerImperialGallon =		nextAllowedValue;
const uint8_t idxDenomImperialGallon =		idxNumerImperialGallon + 1;
#define nextAllowedValue idxDenomImperialGallon + 1
#endif // useImperialGallon
#ifdef useDragRaceFunction
const uint8_t idxPowerFactor =				nextAllowedValue;
#define nextAllowedValue idxPowerFactor + 1
#endif // useDragRaceFunction

#define idxOneThousand idxDecimalPoint

const uint8_t idxMaxConstant =				nextAllowedValue;

#ifdef useDebugTerminalLabels
static const char terminalConstIdxNames[] PROGMEM = {
	"idxNumerDistance\r"
	"idxDenomDistance\r"
	"idxNumerVolume\r"
	"idxDenomVolume\r"
	"idxCycles0PerSecond\r"
	"idxCycles0PerTick\r"
	"idxTicksPerSecond\r"
	"idxMicroSecondsPerSecond\r"
	"idxDecimalPoint\r"
	"idxMetricFE\r"
	"idxSecondsPerHour\r"
	"idxNumber500\r"
	"idxBCDdivisor\r"
#ifdef useIsqrt
	"idxNumerPressure\r"
	"idxDenomPressure\r"
#endif // useIsqrt
#if defined(useAnalogRead)
	"idxNumerVoltage\r"
	"idxDenomVoltage\r"
#endif // useAnalogRead
#ifdef useCarVoltageOutput
	"idxResistanceR5\r"
	"idxResistanceR6\r"
#endif // useCarVoltageOutput
#ifdef useVehicleMass
	"idxNumerMass\r"
	"idxDenomMass\r"
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	"idxNumerArea\r"
	"idxDenomArea\r"
	"idxNumerDensity\r"
	"idxDenomDensity\r"
#endif // useCoastDownCalculator
#ifdef useClockDisplay
	"idxSecondsPerDay\r"
#endif // useClockDisplay
#ifdef useIsqrt
	"idxCorrectionFactor\r"
#endif // useIsqrt
#ifdef useImperialGallon
	"idxNumerImperialGallon\r"
	"idxDenomImperialGallon\r"
#endif // useImperialGallon
#ifdef useDragRaceFunction
	"idxPowerFactor\r"
#endif // useDragRaceFunction
};

#endif // useDebugTerminalLabels
const uint8_t convNumerIdx[] PROGMEM = {
	 idxDenomDistance						// pPulsesPerDistanceIdx
	,idxNumerDistance						// pMinGoodSpeedidx
	,idxNumerVolume							// pTankSizeIdx
	,idxNumerVolume							// pTankBingoSizeIdx
#ifdef usePartialRefuel
	,idxNumerVolume							// pRefuelSizeIdx
#endif // usePartialRefuel
#ifdef useDragRaceFunction
	,idxNumerDistance						// pDragSpeedIdx
	,idxNumerDistance						// pDragDistanceIdx
#endif // useDragRaceFunction
#ifdef useVehicleMass
	,idxNumerMass							// pVehicleMassIdx
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,idxNumerArea							// pVehicleFrontalAreaIdx
	,idxNumerDensity						// pLocustDensityIdx
#endif // useCoastDownCalculator
#ifdef useCalculatedFuelFactor
	,idxNumerPressure						// pSysFuelPressureIdx
	,idxNumerPressure						// pRefFuelPressureIdx
#endif // useCalculatedFuelFactor
#ifdef useChryslerMAPCorrection
	,idxNumerPressure						// pMAPsensorRangeIdx
	,idxNumerPressure						// pMAPsensorOffsetIdx
#ifdef useChryslerBaroSensor
	,idxNumerPressure						// pBaroSensorRangeIdx
	,idxNumerPressure						// pBaroSensorOffsetIdx
#else // useChryslerBaroSensor
	,idxNumerPressure						// pBarometricPressureIdx
#endif // useChryslerBaroSensor
#endif // useChryslerMAPCorrection
#ifdef useBarFuelEconVsSpeed
	,idxNumerDistance						// pBarLowSpeedCutoffIdx
	,idxNumerDistance						// pBarSpeedQuantumIdx
#endif // useBarFuelEconVsSpeed
#ifdef useFuelCost
	,idxDenomVolume							// pCostPerQuantity
#endif // useFuelCost
};

const uint32_t convNumbers[] PROGMEM = {
	 1609344ul						// idxNumerDistance - numerator to convert miles to kilometers
	,1000000ul						// idxDenomDistance - denominator to convert miles to kilometers
	,3785411784ul					// idxNumerVolume - numerator to convert US gallons to liters
	,1000000000ul					// idxDenomVolume - denominator to convert US gallons to liters
	,t0CyclesPerSecond				// idxCycles0PerSecond - timer0 clock cycles per second
	,256ul							// idxCycles0PerTick - timer0 clock cycles per timer0 tick
	,t0TicksPerSecond				// idxTicksPerSecond - timer0 clock ticks per second
	,1000000ul						// idxMicroSecondsPerSecond - microseconds per second
	,1000ul							// idxDecimalPoint - decimal point format (the basis for all of those '* 1000' parameters)
	,100000ul						// idxMetricFE - decimal point format * 100 for metric FE (L / 100km)
	,3600ul							// idxSecondsPerHour - seconds per hour
	,500ul							// idxNumber500 - round to nearest whole number, for numeric formatting
	,100000000ul					// idxBCDdivisor - divisor to separate lower 4 BCD bytes from 5th byte
#ifdef useIsqrt
	,68947573ul						// idxNumerPressure - numerator to convert psig to kPa
	,10000000ul						// idxDenomPressure - denominator to convert psig to kPa
#endif // useIsqrt
#if defined(useAnalogRead)
	,1024ul							// idxNumerVoltage - numerator to convert volts DC to ADC steps
	,5000ul							// idxDenomVoltage - denominator to convert volts DC to ADC steps
#endif // useAnalogRead
#ifdef useCarVoltageOutput
	,9600ul							// idxResistanceR5 - resistor next to ground (via meelis11)
	,27000ul						// idxResistanceR6 - resistor next to diode  (via meelis11)
#endif // useCarVoltageOutput
#ifdef useVehicleMass
	,1000000000ul					// idxNumerMass - numerator to convert pounds to kilograms
	,2204622621ul					// idxDenomMass - denominator to convert pounds to kilograms
#endif // useVehicleMass
#ifdef useCoastDownCalculator
	,9290304ul						// idxNumerArea - numerator to convert square feet to square meters
	,100000000ul					// idxDenomArea - denominator to convert square feet to square meters
	,100000ul						// idxNumerDensity - numerator to convert SAE density to metric density
	,168555ul						// idxDenomDensity - denominator to convert SAE density to metric density
#endif // useCoastDownCalculator
#ifdef useClockDisplay
	,86400ul						// idxSecondsPerDay - number of seconds in a day
#endif // useClockDisplay
#ifdef useIsqrt
	,4096ul							// idxCorrectionFactor - correction factor seed for square root function
#endif // useIsqrt
#ifdef useImperialGallon
	,454609ul						// idxNumerImperialGallon - numerator to convert Imperial gallons to liters
	,100000ul						// idxDenomImperialGallon - denominator to convert Imperial gallons to liters
#endif // useImperialGallon
#ifdef useDragRaceFunction
	,22840ul						// idxPowerFactor - 22.84, or vehicle speed division factor for accel test power estimation function (228.4/10 for internal calculations)
#endif // useDragRaceFunction
};

const uint8_t s64BCDformatList[] PROGMEM = {
	// 10 digit number format
	 0x08		// total entry length
	,' '		// leading zero character
	,0x05		// total BCD byte length / offset into 64-bit register for BCD LSB
	,0x04		// divisor string length
	,100		// 10s and 100s
	,100		// 1000s and 10000s
	,100		// 100000s and 1000000s
	,100		// 10000000s and 100000000s

	// hhmmss number format
	,0x07		// total entry length
	,'0'		// leading zero character
	,0x03		// total BCD byte length / offset into 64-bit register for BCD LSB
	,0x03		// divisor string length
	,60			// seconds
	,60			// minutes
	,24			// hours

	// overflow number format
	,0x03		// total entry length
	,0x00		// leading zero character
	,0xFF		// total BCD byte length // overflow signal

	,0x00		// total entry length == 0 for end of list
};
