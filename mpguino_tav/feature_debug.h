#if defined(useActivityRecord)
namespace activity /* Process activity marking support section prototype */
{

	static void record(uint8_t assertFlag, uint8_t releaseFlag);

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

static const uint8_t arMainDebug =			(arMainDevices | arMainActivity | arMainSample | arMainOutput | arMainOther);

#endif // defined(useActivityRecord)
#if defined(useActivityLED)
namespace activityLED /* Activity status LED support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static void assert(uint8_t flag);
	static void release(uint8_t flag);
	static void toggle(uint8_t flag);
	static void output(uint8_t val);

};

#endif // defined(useActivityLED)
#if defined(useCPUreading) || defined(useDebugCPUreading)
namespace systemInfo /* CPU loading and RAM availability support section prototype */
{

	static void mainProcess(void);
#if defined(useCPUreading)
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static void showCPUload(void);
	static void showCPUloading(void);
#endif // defined(useCPUreading)

};

#if defined(useCPUreading)
extern char __bss_end;
extern char *__brkval;

#endif // defined(useCPUreading)
#endif // defined(useCPUreading) || defined(useDebugCPUreading)
#if defined(useSimulatedFIandVSS)
namespace signalSim /* VSS / fuel injector on-board simulator support section prototype */
{

#if defined(useButtonInput)
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos);
	static uint16_t getSignalSimPageFormats(uint8_t formatIdx);
#endif // defined(useButtonInput)
	static void configurePorts(void);
	static void mainProcessFuel(void);
	static void mainProcessVSS(void);

}

// bit flags for use with v8SignalSimModeFlags
static const uint8_t debugVSSflag =				0b10000000;
static const uint8_t debugInjectorFlag =		0b01000000;
static const uint8_t debugFIPready =			0b00100000;
static const uint8_t debugVSSready =			0b00010000;

static const uint8_t debugEnableFlags =			debugVSSflag | debugInjectorFlag;
static const uint8_t debugVSreadyFlags =		debugVSSready | debugVSSflag;
static const uint8_t debugFIreadyFlags =		debugFIPready | debugInjectorFlag;

static const unsigned long debugVSSresetLength = (unsigned long)(ceil)((2ul * F_CPU) / (2ul * 255ul)) - 1; // 2 sec
static const unsigned long debugFIPresetLength = (unsigned long)(ceil)((4ul * F_CPU) / (3ul * 2ul * 255ul)) - 1; // 4/3 sec

#if defined(useButtonInput)
static const uint16_t signalSimPageFormats[4] PROGMEM = {
	(instantIdx << 8 ) |		(tInjectorTotalTime), 		// Debug
	(instantIdx << 8 ) |		(tVSStotalTime),
	(instantIdx << 8 ) |		(tInjectorPulseCount),
	(instantIdx << 8 ) |		(tVSSpulseCount),
};

static const char debugScreenFuncNames[] PROGMEM = {
	"FI ON   VSS ON" tcEOS
	"FI OFF  VSS ON" tcEOS
	"FI OFF  VSS OFF" tcEOS
	"FI ON   VSS OFF" tcEOS
};

#endif // defined(useButtonInput)
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
	22,
	22,
	23,
	23,
	23,
	24,
	24,
	24,
	25,
	26,
	26,
	27,
	28,
	29,
	30,
	32,
	34,
	36,
	39,
	44,
	51,
	63,
	86,
	156,
	365,
	0,
};

static const uint8_t debugVSSlength = ( sizeof(debugVSSvalues) / sizeof(uint16_t) );
static const uint8_t debugFIPlength = ( sizeof(debugFIPvalues) / sizeof(uint16_t) );

static volatile uint16_t debugVSStickLength;
static volatile uint16_t debugFIPtickLength;
static volatile uint16_t debugFIPWtickLength;

static volatile uint16_t debugVSScount;
static volatile uint16_t debugFIPcount;
static volatile uint16_t debugFIPWcount;

static uint8_t debugVSSidx;
static uint8_t debugFIPidx;

static uint8_t debugVSSstate;
static uint8_t debugFIPstate;

static uint16_t debugFIPWgoodTickLength; // to force debug injector pulse width to a maximum good engine speed-dependent value
static uint16_t debugFIPWreadTickLength;

#endif // defined(useSimulatedFIandVSS)
#if defined(useDebugTerminal)
namespace terminal /* debug terminal section prototype */
{

	static void mainProcess(void);
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
	static void dumpSWEET64information(union union_32 * instrLWord, const uint8_t * &prgmPtr, const uint8_t * prgmStack[], uint64_t * prgmReg64, uint8_t * prgmReg8);
#if defined(useDebugTerminalSWEET64)
	static void outputSWEET64registerContents(uint8_t lineNumber);
	static void outputSWEET64registerExtra(uint8_t lineNumber);
	static void outputSWEET64byte(uint8_t byt);
	static void outputSWEET64operand(uint8_t flag, uint8_t &byt);
	static void outputSWEET64opcode(uint8_t lineNumber);
#if defined(useDebugTerminalLabels)
	static void outputSWEET64prgmOperand(const uint8_t * prgmPtr, uint8_t flag, uint8_t byt, uint8_t labelIdx);
#else // defined(useDebugTerminalLabels)
	static void outputSWEET64prgmOperand(const uint8_t * prgmPtr, uint8_t flag, uint8_t byt);
#endif // defined(useDebugTerminalLabels)
	static void outputSWEET64prgmLine(union union_32 * instrLWord, const uint8_t * &prgmPtr, uint8_t traceFlag);
#endif // defined(useDebugTerminalSWEET64)
	static void processMath(uint8_t cmd);
	static void outputDecimalSettings(void);
#if defined(useBluetoothAdaFruitSPI)
	static void outputBluetoothResponse(void);
#endif // defined(useBluetoothAdaFruitSPI)

}

#define nextAllowedValue 0
static const uint8_t tsError =				nextAllowedValue;
static const uint8_t tsBell =				tsError + 1;
static const uint8_t tsInitInput =			tsBell + 1;
static const uint8_t tsUserInput =			tsInitInput + 1;
static const uint8_t tsInitTerminalCmd =	tsUserInput + 1;
static const uint8_t tsInitProcessing =		tsInitTerminalCmd + 1;
static const uint8_t tsProcessCommand =		tsInitProcessing + 1;
static const uint8_t tsInitListDecimal =	tsProcessCommand + 1;
static const uint8_t tsInitListReadOnly =	tsInitListDecimal + 1;
static const uint8_t tsInitList =			tsInitListReadOnly + 1;
static const uint8_t tsProcessList =		tsInitList + 1;
#define nextAllowedValue tsProcessList + 1;
#if defined(useDebugTerminalHelp)
static const uint8_t tsOutputHelpLine =		nextAllowedValue;
#define nextAllowedValue tsOutputHelpLine + 1;
#endif // defined(useDebugTerminalHelp)
#if defined(useDebugButtonInjection)
static const uint8_t tsInjectButtonPress =	nextAllowedValue;
static const uint8_t tsInjectButtonsUp =	tsInjectButtonPress + 1;
#define nextAllowedValue tsInjectButtonsUp + 1;
#endif // defined(useDebugButtonInjection)
#if defined(useDebugTerminalSWEET64)
static const uint8_t tsOutputSWEET64line =	nextAllowedValue;
static const uint8_t tsTraceSWEET64line =	tsOutputSWEET64line + 1;
#define nextAllowedValue tsTraceSWEET64line + 1;
#endif // defined(useDebugTerminalSWEET64)
#if defined(useBluetoothAdaFruitSPI)
static const uint8_t tsOutputBLEfriend =	nextAllowedValue;
#define nextAllowedValue tsOutputBLEfriend + 1;
#endif // defined(useBluetoothAdaFruitSPI)

#define nextAllowedValue 0
static const uint8_t tseIdxLineCancel =		nextAllowedValue;
static const uint8_t tseIdxSyntax =			tseIdxLineCancel + 1;
static const uint8_t tseIdxState =			tseIdxSyntax + 1;
static const uint8_t tseIdxSourceVal =		tseIdxState + 1;
static const uint8_t tseIdxTargetVal =		tseIdxSourceVal + 1;
static const uint8_t tseIdxAddressVal =		tseIdxTargetVal + 1;
static const uint8_t tseIdxBadIndex =		tseIdxAddressVal + 1;
static const uint8_t tseIdxNoAddress =		tseIdxBadIndex + 1;
#define nextAllowedValue tseIdxNoAddress + 1
#if defined(useDebugTerminalSWEET64)
static const uint8_t tseIdxBadSWEET64addr =	nextAllowedValue;
#define nextAllowedValue tseIdxBadSWEET64addr + 1;
#endif // defined(useDebugTerminalSWEET64)

static uint8_t errIdx;
#if defined(useDebugButtonInjection)
static uint8_t buttonInjDelay;
#endif // defined(useDebugButtonInjection)

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
	"       [y].[x]P - list stored parameters, optionally between [y] and [x]" tcEOSCR
	"xP:y [y] [y]... - store one or more y values, starting at stored parameter x" tcCR tcEOSCR

	"       [y].[x]V - list program variables, optionally between [y] and [x]" tcEOSCR
	"xV:y [y] [y]... - store one or more y values, starting at program variable x" tcCR tcEOSCR

	"       [y].[x]T - list terminal trip variable values, optionally between [y]" tcEOSCR
	"                  and [x]" tcEOSCR
	"xT:y [y] [y]... - store one or more y values, starting at terminal trip" tcEOSCR
	"                  variable x" tcCR tcEOSCR

#if defined(useDebugTerminalSWEET64)
	"      [y].[x]^E - list SWEET64 register contents" tcEOSCR
	"                   [z] - decimal window length (optional)" tcEOSCR
	"                   [y] - decimal digit count (optional)" tcEOSCR
	"                   [x] - decimal processing flag (optional)" tcCR tcEOSCR
	"x^E:y           - store one or more y values, starting at SWEET64 register x" tcCR tcEOSCR

#endif // defined(useDebugTerminalSWEET64)
	"    [y].[x]O - list program constants, optionally between [y] and [x]" tcEOSCR
	"[z]<[y].[x]L - list terminal trip variable function outputs, optionally" tcEOSCR
	"               between [y] and [x]" tcEOSCR
	"                [z] - decimal window length (optional)" tcEOSCR
	"[z]<[y].[x]U - list decimal number sample for output" tcEOSCR
	"                [z] - decimal window length (optional)" tcEOSCR
	"                [y] - decimal digit count (optional)" tcEOSCR
	"                [x] - decimal processing flag (optional)" tcCR tcEOSCR
#if defined(useDebugTerminalSWEET64)
	"   [y].[x]^I - list SWEET64 instructions, along with their operands, optionally" tcEOSCR
	"               between [y] and [x]" tcEOSCR
#if defined(useDebugTerminalLabels)
	"   [y].[x]^F - list all available SWEET64 functions, optionally between [y]" tcEOSCR
	"               and [x]" tcEOSCR
#endif // defined(useDebugTerminalLabels)
	"       [x]^L - list 20 lines of SWEET64 program code, optionally beginning at" tcEOSCR
	"               trip function [x]" tcEOSCR
	"   [y]<[x]^T - trace execution of [x] lines of SWEET64 program code, optionally" tcEOSCR
	"               beginning at trip function [y]" tcEOSCR
	"               if [x] is omitted, traces 1 line" tcEOSCR
	"               if [x] is explicitly set to 0, traces until program completes" tcCR tcEOSCR

#endif // defined(useDebugTerminalSWEET64)
	"    [y]<[x]R - read trip variable x into trip variable y" tcEOSCR
	"                default for x and y is terminal trip variable" tcEOSCR
#if defined(useDebugTerminalLabels)
	"                if no x or y specified, lists available trip variables" tcCR tcEOSCR
#else // defined(useDebugTerminalLabels)
	"                either or both of x or y must be specified" tcCR tcEOSCR
#endif // defined(useDebugTerminalLabels)

	"   [z]<[y].x - enters a number x into the 64-bit math accumulator" tcEOSCR
	"                [z] - decimal window length (optional)" tcEOSCR
	"                [y] - decimal digit count (optional)" tcEOSCR
	"          +x - adds x to math accumulator" tcEOSCR
	"          -x - subtracts x from math accumulator" tcEOSCR
	"          *x - multiplies math accumulator by x" tcEOSCR
#if defined(useIsqrt)
	"          |  - finds square root of math accumulator" tcEOSCR
#endif // defined(useIsqrt)
	"          /x - divides math accumulator by x" tcEOSCR
	"          =x - enters a number x into the 64-bit math accumulator" tcCR tcEOSCR

#if defined(useDebugButtonInjection)
	"           I - inject button press" tcEOSCR
#if defined(useLegacyButtons)
	"                short (l, c, r)" tcEOSCR
	"                 long (L, C, R)" tcCR tcEOSCR
#else // defined(useLegacyButtons)
	"                short (l, c, r, u, d)" tcEOSCR
	"                 long (L, C, R, U, D)" tcCR tcEOSCR
#endif // defined(useLegacyButtons)
#endif // defined(useDebugButtonInjection)

#if defined(useBluetoothAdaFruitSPI)
	"           Y - sends the rest of the input string to BLEfriend shield" tcEOSCR
#endif // defined(useBluetoothAdaFruitSPI)
	"          ^S - displays supplemental system information" tcEOSCR
	"           ? - displays this help" tcEOSCR
	tcEOS
};

#endif // defined(useDebugTerminalHelp)
#if defined(useDebugTerminalSWEET64)
static const uint8_t * terminalListSched;
static const uint8_t * terminalExecSched;

static const uint8_t * terminalStack[16];

static uint8_t terminalS64reg8[(uint16_t)(si64reg8count)];

static uint64_t terminalS64reg64[(uint16_t)(s64reg64count)];

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
static const uint8_t * prgmPtr;
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

#endif // defined(useDebugTerminal)
