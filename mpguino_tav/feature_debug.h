#if defined(useActivityLED)
namespace activityLED /* Activity status LED support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static void assert(uint8_t flag);
	static void release(uint8_t flag);
	static void toggle(uint8_t flag);
	static void output(void);

};

// flags for use with m8ActivityStatusIdx, m8ActivityOutputIdx
static const uint8_t arMainProcess =		0b10000000;
static const uint8_t arIdleProcess =		0b01000000;
static const uint8_t arMainDevices =		0b00100000;
static const uint8_t arMainActivity =		0b00010000;
static const uint8_t arMainSample =			0b00001000;
static const uint8_t arMainOutput =			0b00000100;
static const uint8_t arMainOther =			0b00000010;
static const uint8_t arSWEET64 =			0b00000001;

#endif // defined(useActivityLED)
#if defined(useCPUreading) || defined(useDebugCPUreading)
namespace systemInfo /* CPU loading and RAM availability support section prototype */
{

#if defined(useCPUreading) && defined(useButtonInput)
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void showCPUload(void);
	static void showCPUloading(void);
#endif // defined(useCPUreading) && defined(useButtonInput)

};

#if defined(useCPUreading) || defined(useDebugCPUreading)
extern char __bss_end;
extern char *__brkval;

#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useSimulatedFIandVSS)
namespace signalSim /* VSS / fuel injector on-board simulator support section prototype */
{

#if defined(useButtonInput)
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static uint16_t getSignalSimPageFormats(uint8_t formatIdx);
#endif // defined(useButtonInput)
	static void configurePorts(uint8_t newMode);

}

// bit flags for use with v8SignalSimModeIdx
static const uint8_t debugFIPready =			0b10000000;
static const uint8_t debugVSSready =			0b01000000;
static const uint8_t debugFIPfiring =			0b00100000;
static const uint8_t debugFIPpeak =				0b00010000;
static const uint8_t debugFIPhold =				0b00001000;
static const uint8_t debugFIsaturatedFlag =		0b00000100;
static const uint8_t debugVSSflag =				0b00000010;
static const uint8_t debugInjectorFlag =		0b00000001;

static const uint8_t debugEnableFlags =			(debugFIsaturatedFlag | debugVSSflag | debugInjectorFlag);
static const uint8_t debugPeakHoldFlags =		(debugFIPpeak | debugFIPhold);
static const uint8_t debugOutputFlags =			(debugVSSflag | debugInjectorFlag);
static const uint8_t debugVSreadyFlags =		(debugVSSready | debugVSSflag);
static const uint8_t debugFIreadyFlags =		(debugFIPready | debugInjectorFlag);

#if defined(useButtonInput)
static const uint16_t signalSimPageFormats[4] PROGMEM = {
	(instantIdx << 8) |		(tInjectorTotalTime), 		// Debug
	(instantIdx << 8) |		(tVSStotalTime),
	(instantIdx << 8) |		(tInjectorPulseCount),
	(instantIdx << 8) |		(tVSSpulseEdgeCount),
};

static const char debugScreenFuncNames[] PROGMEM = {
	"FI SAT  VSS ON" tcEOS
	"FI OFF  VSS ON" tcEOS
	"FI OFF  VSS OFF" tcEOS
	"FI SAT  VSS OFF" tcEOS
	"FI PH   VSS ON" tcEOS
	"FI OFF  VSS ON" tcEOS
	"FI OFF  VSS OFF" tcEOS
	"FI PH   VSS OFF" tcEOS
};

#endif // defined(useButtonInput)
#if defined(useDebugTerminalLabels)
static const char terminalSignalSimHelp[] PROGMEM = {
	"signal simulation off" tcEOS
	"peak/hold fuel injector signal simulation only" tcEOS
	"VSS signal simulation only" tcEOS
	"peak/hold fuel injection and VSS signal simulation" tcEOS
	"signal simulation off" tcEOS
	"saturated fuel injector signal simulation only" tcEOS
	"VSS signal simulation only" tcEOS
	"saturated fuel injection and VSS signal simulation" tcEOS
};

#endif // defined(useDebugTerminalLabels)
static const uint16_t debugVSSvalues[] PROGMEM = {
	65535,
	2258,
	1128,
	752,
	564,
	451,
	375,
	322,
	281,
	250,
	225,
	204,
	187,
	173,
	160,
	150,
	140,
	132,
	124,
	118,
	112,
	107,
	102,
	97,
	93,
	89,
	86,
	83,
	80,
	77,
	74,
	72,
	70,
	67,
	65,
	64,
	62,
	60,
	58,
	57,
	55,
	54,
	53,
	52,
	50,
	49,
	48,
	47,
	46,
	45,
	44,
};

static const uint16_t debugFIPvalues[] PROGMEM = {
	313,
	326,
	341,
	358,
	375,
	395,
	417,
	442,
	470,
	501,
	537,
	578,
	626,
	683,
	752,
	836,
	940,
	1075,
	1254,
	1505,
	1881,
	2509,
	3764,
	7528,
	18823,
	65535,
};

static const uint16_t debugFIPWvalues[] PROGMEM = {
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	48,
	49,
	53,
	58,
	64,
	73,
	87,
	111,
	157,
	296,
	715,
	0,
//	22,
//	22,
//	23,
//	23,
//	23,
//	24,
//	24,
//	24,
//	25,
//	26,
//	26,
//	27,
//	28,
//	29,
//	30,
//	32,
//	34,
//	36,
//	39,
//	44,
//	51,
//	63,
//	86,
//	156,
//	365,
//	0,
};

static const uint8_t debugVSSlength = ( sizeof(debugVSSvalues) / sizeof(uint16_t) );
static const uint8_t debugFIPlength = ( sizeof(debugFIPvalues) / sizeof(uint16_t) );

#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugTerminal)
namespace terminal /* debug terminal section prototype */
{

	static void mainProcess(void);
	static uint8_t removeTerminalInputByte(void);
	static void backspaceTerminalInput(void);
	static void outputTripFunctionValue(uint8_t lineNumber);
	static void outputConstantValue(uint8_t lineNumber);
	static void outputConstantExtra(uint8_t lineNumber);
	static void outputParameterValue(uint8_t lineNumber);
	static void outputParameterExtra(uint8_t lineNumber);
	static void outputVariableValue(uint8_t lineNumber);
	static void outputVariableExtra(uint8_t lineNumber);
	static void outputTripVarMeasuredValue(uint8_t lineNumber);
	static void outputTripVarMeasuredExtra(uint8_t lineNumber);
	static void outputDecimalValue(uint8_t lineNumber);
	static void outputDecimalExtra(uint8_t lineNumber);
#if defined(useSimulatedFIandVSS)
	static void outputSignalSimSetting(uint8_t lineNumber);
#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugTerminalSWEET64)
	static void outputSWEET64programCounter(s64pc_t prgmPtr);
	static void outputSWEET64error(uint8_t errorCode);
	static uint8_t isSWEET64errorLatched(uint8_t errorCode, s64pc_t prgmPtr);
	static void clearSWEET64errorLatch(void);
	static void outputSWEET64errorLatch(void);
	static void reportSWEET64error(union union_32 * instrLWord, s64pc_t &prgmPtr, s64pc_t prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8);
	static void dumpSWEET64information(union union_32 * instrLWord, s64pc_t &prgmPtr, s64pc_t prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8);
	static void hexByteOut(uint8_t devIdx, uint8_t val);
#if defined(useDebugTerminalLabels)
	static void outputSWEET64functionLength(uint8_t lineNumber);
#endif // defined(useDebugTerminalLabels)
	static void outputSWEET64registerContents(uint8_t lineNumber);
	static void outputSWEET64registerExtra(uint8_t lineNumber);
	static void outputSWEET64byte(uint8_t byt);
	static void outputSWEET64operand(uint8_t flag, uint8_t &byt);
	static void outputSWEET64opcode(uint8_t lineNumber);
#if defined(useSWEET64RAMprograms)
	static void outputSWEET64programRAMline(uint8_t lineNumber);
	static void outputSWEET64programRAMoverride(void);
	static uint8_t getSWEET64instructionByteCount(uint8_t opCode, uint8_t format);
	static void outputSWEET64sourceByte(uint8_t byt);
	static uint8_t outputSWEET64sourceLabelByte(uint8_t byt, uint8_t labelIdx);
	static uint8_t outputSWEET64programRAMsourceLine(uint8_t lineNumber);
	static uint8_t chrEqualIgnoreCase(uint8_t inputChar, uint8_t storedChar);
	static uint8_t matchSWEET64opcodeAlias(char * token, const char * opCodePtr);
	static uint8_t findSWEET64opcode(char * token);
	static uint8_t pullSWEET64assemblerToken(void);
	static uint8_t parseSWEET64assemblerByte(char * token, uint8_t &byt);
	static uint8_t pullSWEET64assemblerOperand(uint8_t &byt, uint8_t labelIdx);
#if defined(useDebugTerminalLabels)
	static uint8_t findSWEET64labelByte(char * token, uint8_t labelIdx, uint8_t &byt);
#endif // defined(useDebugTerminalLabels)
	static uint8_t assembleSWEET64programRAMline(void);
#endif // defined(useSWEET64RAMprograms)
#if defined(useDebugTerminalLabels)
	static void getSWEET64operandLabelIndexes(uint8_t instr, uint8_t format, uint8_t &operandLabelIdx, uint8_t &extraLabelIdx);
	static void outputSWEET64prgmOperand(s64pc_t prgmPtr, uint8_t flag, uint8_t byt, uint8_t labelIdx);
#else // defined(useDebugTerminalLabels)
	static void outputSWEET64prgmOperand(s64pc_t prgmPtr, uint8_t flag, uint8_t byt);
#endif // defined(useDebugTerminalLabels)
	static void outputSWEET64prgmLine(union union_32 * instrLWord, s64pc_t &prgmPtr, uint8_t traceFlag);
#endif // defined(useDebugTerminalSWEET64)
	static void processMath(uint8_t cmd);
	static void outputDecimalSettings(void);
	static void outputFlagStatusGroup(const char * flagGroup, uint8_t flags, uint8_t labelFlag);
	static void outputSystemStatusFlags(void);
	static void outputSystemStatusBytes(void);
#if defined(useBluetoothAdaFruitSPI)
	static void outputBluetoothResponse(void);
#endif // defined(useBluetoothAdaFruitSPI)

}

#define nextAllowedValue 0
static const uint8_t tsError =					nextAllowedValue;
static const uint8_t tsBell =					tsError + 1;
static const uint8_t tsInitInput =				tsBell + 1;
static const uint8_t tsUserInput =				tsInitInput + 1;
static const uint8_t tsInitTerminalCmd =		tsUserInput + 1;
static const uint8_t tsInitProcessing =			tsInitTerminalCmd + 1;
static const uint8_t tsProcessCommand =			tsInitProcessing + 1;
static const uint8_t tsInitListDecimal =		tsProcessCommand + 1;
static const uint8_t tsInitListReadOnly =		tsInitListDecimal + 1;
static const uint8_t tsInitList =				tsInitListReadOnly + 1;
static const uint8_t tsProcessList =			tsInitList + 1;
#define nextAllowedValue tsProcessList + 1;
#if defined(useDebugTerminalHelp)
static const uint8_t tsOutputHelpLine =			nextAllowedValue;
#define nextAllowedValue tsOutputHelpLine + 1;
#endif // defined(useDebugTerminalHelp)
#if defined(useDebugButtonInjection)
static const uint8_t tsInjectButtonPress =		nextAllowedValue;
static const uint8_t tsInjectButtonsUp =		tsInjectButtonPress + 1;
#define nextAllowedValue tsInjectButtonsUp + 1;
#endif // defined(useDebugButtonInjection)
#if defined(useDebugTerminalSWEET64)
static const uint8_t tsOutputSWEET64line =		nextAllowedValue;
static const uint8_t tsTraceSWEET64line =		tsOutputSWEET64line + 1;
#define nextAllowedValue tsTraceSWEET64line + 1;
#if defined(useSWEET64RAMprograms)
static const uint8_t tsOutputSWEET64RAMline =	nextAllowedValue;
static const uint8_t tsInitSWEET64assembler =	tsOutputSWEET64RAMline + 1;
static const uint8_t tsSWEET64assemblerInput =	tsInitSWEET64assembler + 1;
static const uint8_t tsOutputSWEET64RAMsourceLine = tsSWEET64assemblerInput + 1;
#define nextAllowedValue tsOutputSWEET64RAMsourceLine + 1;
#endif // defined(useSWEET64RAMprograms)
#endif // defined(useDebugTerminalSWEET64)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t tsOutputBLEfriend =		nextAllowedValue;
#define nextAllowedValue tsOutputBLEfriend + 1;
#endif // defined(useBluetoothAdaFruitSPI)

#define nextAllowedValue 0
static const uint8_t tseIdxLineCancel =			nextAllowedValue;
static const uint8_t tseIdxSyntax =				tseIdxLineCancel + 1;
static const uint8_t tseIdxState =				tseIdxSyntax + 1;
static const uint8_t tseIdxSourceVal =			tseIdxState + 1;
static const uint8_t tseIdxTargetVal =			tseIdxSourceVal + 1;
static const uint8_t tseIdxAddressVal =			tseIdxTargetVal + 1;
static const uint8_t tseIdxBadIndex =			tseIdxAddressVal + 1;
static const uint8_t tseIdxNoAddress =			tseIdxBadIndex + 1;
#define nextAllowedValue tseIdxNoAddress + 1
#if defined(useDebugTerminalSWEET64)
static const uint8_t tseIdxBadSWEET64addr =		nextAllowedValue;
#define nextAllowedValue tseIdxBadSWEET64addr + 1;
#endif // defined(useDebugTerminalSWEET64)

static uint8_t errIdx;

static const char tseErrorStringList[] PROGMEM = {
	"\\" tcEOSCR
	tcCR "syntax" tcEOS
	tcCR "bad state" tcEOS
	tcCR "bad source" tcEOS
	tcCR "bad target" tcEOS
	tcCR "bad address" tcEOS
	tcCR "bad index value" tcEOS
	tcCR "no address" tcEOS
#if defined(useDebugTerminalSWEET64)
	tcCR "nope" tcEOS
#endif // defined(useDebugTerminalSWEET64)
};

#if defined(useDebugTerminalSWEET64)
static const char terminalSWEET64errorList[] PROGMEM = {
	"none" tcEOS
	"bad PC" tcEOS
	"bad opcode" tcEOS
	"bad reg operand" tcEOS
	"missing reg operand" tcEOS
	"missing operand" tcEOS
	"missing extra" tcEOS
	"bad expanded opcode" tcEOS
	"stack overflow" tcEOS
	"bad operand" tcEOS
};

#endif // defined(useDebugTerminalSWEET64)

static const char tseBadAddress[] PROGMEM = {
	tcCR "No Index" tcEOSCR
};

#if defined(useDebugTerminalSWEET64) && defined(useDebugTerminalLabels)
typedef struct
{

	uint8_t labelType;
	const uint8_t * labelStringPointer;
	uint8_t labelLength;

} dS64label_t;

#define nextAllowedValue 1
static const uint8_t dslIdxConst =				nextAllowedValue;
static const uint8_t dslIdxConstSWEET64 =		dslIdxConst + 1;
static const uint8_t dslIdxEEPROM =				dslIdxConstSWEET64 + 1;
static const uint8_t dslIdxExpandedEEPROM =		dslIdxEEPROM + 1;
static const uint8_t dslIdxProgramVariable =	dslIdxExpandedEEPROM + 1;
static const uint8_t dslIdxTripVariable =		dslIdxProgramVariable + 1;
static const uint8_t dslIdxTripMeasurement =	dslIdxTripVariable + 1;
static const uint8_t dslIdxFunction =			dslIdxTripMeasurement + 1;
static const uint8_t dslIdxBCDformat =			dslIdxFunction + 1;
#define nextAllowedValue dslIdxBCDformat + 1

static const uint8_t dslIdxEnd =				nextAllowedValue;

static const dS64label_t debugSWEET64labelList[] PROGMEM = {
	{0,		terminalParameterNames, 			pSettingsIdxLen},
	{0,		terminalConstIdxNames,				idxConstantLength},
	{1,		terminalParameterNames, 			pSettingsIdxLen},
	{1,		terminalExpandedParameterNames, 	pExpandedSettingsIdxLen},
	{2,		terminalVariableLabels, 			programVariableMaxIdx},
	{3,		terminalTripVarNames, 				tripSlotTotalCount},
	{4,		terminalTripVarLabels, 				rvMeasuredCount},
	{5,		terminalTripFuncNames, 				dfMaxValTotalCount},
	{6,		terminalBCDformatNames, 			4},
};

#endif // defined(useDebugTerminalSWEET64) && defined(useDebugTerminalLabels)
static const uint8_t tmByteReadIn =			0x80;
static const uint8_t tmSourceReadIn =		0x40;
static const uint8_t tmTargetReadIn =		0x20;
static const uint8_t tmAddressReadIn =		0x10;
static const uint8_t tmHexInput =			0x08;
static const uint8_t tmDecimalInput =		0x04;
static const uint8_t tmButtonInput =		0x02;
static const uint8_t tmInitInput =			0x01;

static const uint8_t tmReadInMask =			(tmByteReadIn | tmSourceReadIn | tmTargetReadIn | tmAddressReadIn);
static const uint8_t tmInputMask =			(tmHexInput | tmDecimalInput | tmButtonInput | tmByteReadIn);
static const uint8_t tmInitHex =			(tmInitInput | tmHexInput);
static const uint8_t tmInitDecimal =		(tmInitInput | tmDecimalInput);
static const uint8_t tmInitButton =			(tmInitInput | tmButtonInput);
static const uint8_t tmButtonReadIn =		(tmButtonInput | tmByteReadIn);

#if defined(useDebugTerminalHelp)
static const char terminalHelp[] PROGMEM = {
	tcSP7 "[y].[x]P - list stored parameters, optionally between [y] and [x]" tcEOSCR
	"xP:y [y] [y]... - store one or more y values, starting at stored parameter x" tcCR tcEOSCR

	tcSP7 "[y].[x]V - list program variables, optionally between [y] and [x]" tcEOSCR
	"xV:y [y] [y]... - store one or more y values, starting at program variable x" tcCR tcEOSCR

	tcSP7 "[y].[x]T - list terminal trip variable values, optionally between [y] and [x]" tcCR tcEOSCR
	"xT:y [y] [y]... - store one or more y values, starting at terminal trip variable x" tcCR tcEOSCR

#if defined(useTFToutput)
	tcSP7 "[x]G - cycle TFT display rotation, or with x (0..3) set a specific rotation" tcCR tcEOSCR

#endif // defined(useTFToutput)
#if defined(useTouchScreenInput) && defined(useTFToutput)
	tcSP7 "H - touch-screen test: crosshair on the TFT, raw touch coords to terminal" tcCR tcEOSCR
	tcSP7 "J - 4-corner touch calibration: tap each crosshair, stored to EEPROM" tcCR tcEOSCR

#endif // defined(useTouchScreenInput) && defined(useTFToutput)
#if defined(useDebugTerminalSWEET64)
	tcSP11 "N - toggle SWEET64 error mute (suppress/unsuppress repeated error output)" tcCR tcEOSCR
	tcSP6 "[y].[x]^E - list SWEET64 register contents" tcEOSCR
	tcSP19 "[z] - decimal window length (optional)" tcEOSCR
	tcSP19 "[y] - decimal digit count (optional)" tcEOSCR
	tcSP19 "[x] - decimal processing flag (optional)" tcCR tcEOSCR
	"x^E:y           - store one or more y values, starting at SWEET64 register x" tcCR tcEOSCR
#if defined(useSWEET64RAMprograms)
	tcSP7 "[x]!    - assemble SWEET64 into program RAM, starting at x" tcEOSCR
	tcSP7 "[y].[x]M - dump SWEET64 program RAM, optionally between [y] and [x]" tcEOSCR
	"xM:y [y] [y]... - store one or more bytes, starting at SWEET64 program RAM x" tcEOSCR
	tcSP7 "[x]<M    - fill SWEET64 program RAM with x, default 00" tcEOSCR
	tcSP7 "x<yM    - copy SWEET64 program y to program RAM, starting at x" tcEOSCR
	tcSP7 "x<^L    - list SWEET64 program RAM as pseudo-code, starting at x" tcEOSCR
	tcSP7 "x.y^T   - trace SWEET64 function x, optionally for y lines" tcEOSCR
	tcSP7 "z<y^T   - trace SWEET64 program RAM at z, optionally for y lines" tcEOSCR
	tcSP18 "if y is omitted, traces 1 line; if y is 0, traces until done" tcCR tcEOSCR
	tcSP7 "x.y^W   - export SWEET64 program RAM between x and y as source" tcEOSCR
	tcSP7 "x<y^O   - substitute program RAM at x for SWEET64 function y" tcEOSCR
	tcSP7 "^O      - disable SWEET64 program RAM substitution" tcCR tcEOSCR
#endif // defined(useSWEET64RAMprograms)

#endif // defined(useDebugTerminalSWEET64)
	tcSP4 "[y].[x]O - list program constants, optionally between [y] and [x]" tcEOSCR
	"[z]<[y].[x]L - list terminal trip variable function outputs, optionally between [y] and [x]" tcEOSCR
	tcSP16 "[z] - decimal window length (optional)" tcCR tcEOSCR
#if !defined(useSWEET64devMonitor)
	"[z]<[y].[x]U - list decimal number sample for output" tcEOSCR
	tcSP16 "[z] - decimal window length (optional)" tcEOSCR
	tcSP16 "[y] - decimal digit count (optional)" tcEOSCR
	tcSP16 "[x] - decimal processing flag (optional)" tcCR tcEOSCR
#endif // !defined(useSWEET64devMonitor)

#if defined(useDebugTerminalSWEET64)
	tcSP3 "[y].[x]^I - list SWEET64 instructions, along with their operands, optionally between [y] and [x]" tcCR tcEOSCR
#if defined(useDebugTerminalLabels)
	tcSP3 "[y].[x]^F - list all available SWEET64 functions and byte lengths, optionally between [y] and [x]" tcCR tcEOSCR
#endif // defined(useDebugTerminalLabels)
	tcSP7 "[x]^L - list 20 lines of SWEET64 program code, optionally beginning at trip function [x]" tcCR tcEOSCR
#if !defined(useSWEET64RAMprograms)
	tcSP7 "x.y^T - trace SWEET64 function x, optionally for y lines" tcEOSCR
	tcSP15 "if y is omitted, traces 1 line" tcEOSCR
	tcSP15 "if y is explicitly set to 0, traces until program completes" tcCR tcEOSCR
#endif // !defined(useSWEET64RAMprograms)

#endif // defined(useDebugTerminalSWEET64)
	tcSP4 "[y]<[x]R - read trip variable x into trip variable y" tcEOSCR
	tcSP16 "default for x and y is terminal trip variable" tcEOSCR
#if defined(useDebugTerminalLabels)
	tcSP16 "if no x or y specified, lists available trip variables" tcCR tcEOSCR
#else // defined(useDebugTerminalLabels)
	tcSP16 "either or both of x or y must be specified" tcCR tcEOSCR
#endif // defined(useDebugTerminalLabels)

#if !defined(useSWEET64devMonitor)
	tcSP3 "[z]<[y].x - enters a number x into the 64-bit math accumulator" tcEOSCR
	tcSP16 "[z] - decimal window length (optional)" tcEOSCR
	tcSP16 "[y] - decimal digit count (optional)" tcEOSCR
	tcSP10 "+x - adds x to math accumulator" tcEOSCR
	tcSP10 "-x - subtracts x from math accumulator" tcEOSCR
	tcSP10 "*x - multiplies math accumulator by x" tcEOSCR
#if defined(useIsqrt)
	tcSP10 "|  - finds square root of math accumulator" tcEOSCR
#endif // defined(useIsqrt)
	tcSP10 "/x - divides math accumulator by x" tcEOSCR
	tcSP10 "=x - enters a number x into the 64-bit math accumulator" tcCR tcEOSCR
#endif // !defined(useSWEET64devMonitor)

#if defined(useDebugButtonInjection)
	tcSP11 "I - inject button press" tcEOSCR
#if defined(useLegacyButtons)
	tcSP16 "short (l, c, r)" tcEOSCR
	tcSP17 "long (L, C, R)" tcCR tcEOSCR
#else // defined(useLegacyButtons)
	tcSP16 "short (l, c, r, u, d)" tcEOSCR
	tcSP17 "long (L, C, R, U, D)" tcCR tcEOSCR
#endif // defined(useLegacyButtons)
#endif // defined(useDebugButtonInjection)
#if defined(useSimulatedFIandVSS)
	tcSP11 "S - lists available signal simulator modes" tcEOSCR
	tcSP10 "yS - sets signal simulator mode to y" tcEOSCR
#endif // defined(useSimulatedFIandVSS)
#if defined(useBluetoothAdaFruitSPI)
	tcSP11 "Y - sends the rest of the input string to BLEfriend shield" tcEOSCR
#endif // defined(useBluetoothAdaFruitSPI)
	tcSP10 "^S - displays supplemental system information" tcEOSCR
	tcSP11 "? - displays this help" tcEOSCR
	tcEOS
};

#endif // defined(useDebugTerminalHelp)
#if defined(useDebugTerminalSWEET64)
static s64pc_t terminalListSched;
static s64pc_t terminalExecSched;
static s64pc_t terminalS64lastErrorPC;

static s64pc_t terminalStack[(uint16_t)(s64stackSize)];

static uint8_t terminalS64reg8[(uint16_t)(si64reg8count)];
static uint8_t terminalS64lastErrorCode;
static uint8_t terminalS64errorLatched;
static uint8_t terminalS64errorMuted;
static uint16_t terminalS64lastErrorCount;

static uint64_t terminalS64reg64[(uint16_t)(s64reg64count)];

#if defined(useSWEET64RAMprograms)
static uint8_t s64programRAMdumpEnd;
static uint8_t s64programRAMassemblerAddr;
static char s64programRAMassemblerToken[24];
#endif // defined(useSWEET64RAMprograms)

static const char prgmLoadByteValue[] PROGMEM = {
	instrLdRegByteFromIndex, 0x02,						// load byte value
	instrMul2byRdOnly, idxDecimalPoint,					// multiply by decimal formatting term
	instrDone											// exit to caller
};

#endif // defined(useDebugTerminalSWEET64)
static uint8_t terminalState;
static uint8_t nextTerminalState;
static uint8_t terminalCmd;
static uint8_t terminalMode;

static uint8_t terminalByte;
static uint8_t terminalSource;
static uint8_t terminalTarget;

static uint8_t terminalAddress;
static uint8_t terminalLine;
static uint8_t maxLine;
static uint8_t decPlace;
static uint8_t decWindow;
static uint8_t decMode;

#if defined(useDebugTerminalLabels)
static const char * labelList;
static uint8_t labelListOffset;
#endif // defined(useDebugTerminalLabels)
static s64pc_t prgmPtr;
static void (* primaryFunc)(uint8_t);
static void (* extraFunc)(uint8_t);

static const char terminalPrimarySeparator[] PROGMEM = {
	": " tcEOS
};

static const char terminalSecondarySeparator[] PROGMEM = {
	" - " tcEOS
};

// bit flags for use with m8PeekFlags
static const uint8_t peekStatusMessage =		0b10000000;
static const uint8_t peekBluetoothInput =		0b01000000;
static const uint8_t peekBluetoothOutput =		0b00100000;
static const uint8_t peekBLEfriendEcho =		0b00010000;
static const uint8_t peekEnableCPUread =		0b00001000;
static const uint8_t peekOutputFlags =			0b00000100;

#endif // defined(useDebugTerminal)
