#ifdef useCPUreading

uint32_t mainStart;
uint32_t idleTimerLength;
#ifdef useDebugCPUreading
uint8_t monitorState;
uint32_t idleProcessTimerLength;
uint32_t displayTimerLength;
uint32_t SWEET64timerLength;
#endif // useDebugCPUreading

namespace systemInfo /* CPU loading and RAM availability support section prototype */
{

	static void idleProcess(void);
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void showCPUload(void);
	static void showCPUloading(void);
	static uint16_t getAvailableRAMsize(void);
	static uint32_t findCycleLength(unsigned long lastCycle, unsigned long thisCycle);
	static uint32_t cycles0(void);

};

extern char __bss_end;
extern char *__brkval;

#endif // useCPUreading
#if defined(useActivityLED)
namespace activityLED /* Activity status LED support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static void output(uint8_t val);

};

#endif // defined(useActivityLED)
#ifdef useSimulatedFIandVSS
namespace debugReading /* VSS / fuel injector on-board simulator support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void configurePorts(void);
	static void idleProcessFuel(void);
	static void idleProcessVSS(void);

}

volatile uint8_t debugFlags;

const uint8_t debugVSSflag =			0b00000001;
const uint8_t debugInjectorFlag =		0b00000010;
const uint8_t debugFIPready =			0b00000100;
const uint8_t debugVSSready =			0b00001000;

const uint8_t debugEnableFlags =		debugVSSflag | debugInjectorFlag;
const uint8_t debugVSreadyFlags =		debugVSSready | debugVSSflag;
const uint8_t debugFIreadyFlags =		debugFIPready | debugInjectorFlag;

const unsigned long debugVSSresetLength = (unsigned long)(ceil)((2ul * F_CPU) / (2ul * 255ul)) - 1; // 2 sec
const unsigned long debugFIPresetLength = (unsigned long)(ceil)((4ul * F_CPU) / (3ul * 2ul * 255ul)) - 1; // 4/3 sec

volatile uint16_t debugVSStickLength;
volatile uint16_t debugFIPtickLength;
volatile uint16_t debugFIPWtickLength;

volatile uint16_t debugVSScount;
volatile uint16_t debugFIPcount;
volatile uint16_t debugFIPWcount;

#endif // useSimulatedFIandVSS
#ifdef useTestButtonValues
namespace buttonView /* Button input value viewer section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

}

#endif // useTestButtonValues
#ifdef useDebugTerminal
namespace terminal /* debug terminal section prototype */
{

	static void mainProcess(void);
	static void outputFlags(uint8_t flagRegister, const char * flagStr);
	static void outputTripFunctionValue(uint8_t lineNumber);
	static void outputConstantValue(uint8_t lineNumber);
	static void outputParameterValue(uint8_t lineNumber);
	static void outputParameterExtra(uint8_t lineNumber);
	static void outputVolatileValue(uint8_t lineNumber);
	static void outputMainProgramValue(uint8_t lineNumber);
	static void outputTripVarMeasuredValue(uint8_t lineNumber);
	static void outputTripVarMeasuredExtra(uint8_t lineNumber);
	static void outputDecimalValue(uint8_t lineNumber);
	static void outputDecimalExtra(uint8_t lineNumber);
	static void processMath(uint8_t cmd);

}

static char termNumberBuff[17];

static const uint8_t tBuffLength = 40;

#if defined(useDebugTerminalHelp)
static const char terminalHelp[] PROGMEM = {
	"    [y].[x]P - list stored parameters, optionally between [y] and [x]" "\r"
	"    [y].[x]V - list volatile variables, optionally between [y] and [x]" "\r"
	"    [y].[x]M - list main program variables, optionally between [y] and [x]" "\r"
	"    [y].[x]T - list terminal trip variable values, optionally between [y] and [x]" "\r"
	"    [y].[x]O - list program constants, optionally between [y] and [x]" "\r"
	"[z]<[y].[x]L - list terminal trip variable function outputs, optionally between [y] and [x]" "\r"
	"                [z] - decimal window length (optional)" "\r"
	"[z]<[y].[x]U - list decimal number sample for output" "\r"
	"                [z] - decimal window length (optional)" "\r"
	"                [y] - decimal digit count (optional)" "\r"
	"                [x] - decimal processing flag (optional)" "\r"
#ifdef useSWEET64trace
	"              x^L - list SWEET64 source code for trip function" "\r"
#endif // useSWEET64trace
	"   [z]<[y].x - enters a number x into the 64-bit math accumulator" "\r"
	"                [z] - decimal window length (optional)" "\r"
	"                [y] - decimal digit count (optional)" "\r"
	"          +x - adds x to math accumulator" "\r"
	"		  -x - subtracts x from math accumulator" "\r"
	"		  *x - multiplies math accumulator by x" "\r"
	"		  /x - divides math accumulator by x" "\r"
	"		  =x - enters a number x into the 64-bit math accumulator" "\xEF" "\r"
	"  x:Py [y] [y]... - store one or more y values, starting at stored parameter x" "\r"
	"  x:Vy [y] [y]... - store one or more y values, starting at volatile variable x" "\r"
	"  x:My [y] [y]... - store one or more y values, starting at main program variable x" "\r"
	"  x:Ty [y] [y]... - store one or more y values, starting at terminal trip variable x" "\xEF" "\r"
	"    [y]<[x]R - read trip variable x into trip variable y" "\r"
	"                default for x and y is terminal trip variable" "\r"
	"                if no x or y specified, lists available trip variables" "\r"
	"           S - output system status" "\r"
	"           I - inject button press" "\r"
#ifdef useLegacyButtons
	"                short (l, c, r)" "\r"
	"                 long (L, C, R)" "\xEF" "\r"
#else // useLegacyButtons
	"                short (l, c, r, u, d)" "\r"
	"                 long (L, C, R, U, D)" "\xEF" "\r"
#endif // useLegacyButtons
	"           ? - displays this help\r"
	"\0"
};

#endif // defined(useDebugTerminalHelp)
const uint8_t tmByteReadIn =		0x80;
const uint8_t tmSourceReadIn =		0x40;
const uint8_t tmTargetReadIn =		0x20;
const uint8_t tmAddressReadIn =		0x10;

const uint8_t tmHexInput =			0x08;
const uint8_t tmDecimalInput =		0x04;
const uint8_t tmButtonInput =		0x02;
const uint8_t tmInitInput =			0x01;

const uint8_t tmInputMask =			(tmHexInput | tmDecimalInput | tmButtonInput | tmInitInput);
const uint8_t tmInitHex =			(tmInitInput | tmHexInput);
const uint8_t tmInitDecimal =		(tmInitInput | tmDecimalInput);
const uint8_t tmInitButton =		(tmInitInput | tmButtonInput);

uint8_t terminalState;
uint8_t nextTerminalState;
uint8_t terminalCmd;
uint8_t terminalMode;

uint8_t terminalByte;
uint8_t terminalSource;
uint8_t terminalTarget;

uint8_t chr;
uint8_t inpIdx;
uint8_t readIdx;
uint8_t terminalAddress;
uint8_t terminalLine;
uint8_t maxLine;
uint8_t decPlace;
uint8_t decWindow;
uint8_t decMode;

char terminalBuff[tBuffLength];

const char * labelList;
const uint8_t * prgmPtr;
void (* primaryFunc)(uint8_t);
void (* extraFunc)(uint8_t);

#endif // useDebugTerminal
#ifdef useSimulatedFIandVSS
const uint8_t debugScreenFormats[4][2] PROGMEM = {
	 {lblInstantIdx,	tInjectorTotalTime} 		// Debug
	,{lblInstantIdx,	tVSStotalTime}
	,{lblInstantIdx,	tInjectorPulseCount}
	,{lblInstantIdx,	tVSSpulseCount}
};

const char debugScreenFuncNames[] PROGMEM = {
	"FI ON   VSS ON\r"
	"FI OFF  VSS ON\r"
	"FI OFF  VSS OFF\r"
	"FI ON   VSS OFF\r"
};

const uint16_t debugVSSvalues[] PROGMEM = {
 65535
,2258
,1128
,752
,564
,451
,375
,322
,281
,250
,225
,204
,187
,173
,160
,150
,140
,132
,124
,118
,112
,107
,102
,97
,93
,89
,86
,83
,80
,77
,74
,72
,70
,67
,65
,64
,62
,60
,58
,57
,55
,54
,53
,52
,50
,49
,48
,47
,46
,45
,44
};

const uint16_t debugFIPvalues[] PROGMEM = {
 313
,326
,341
,358
,375
,395
,417
,442
,470
,501
,537
,578
,626
,683
,752
,836
,940
,1075
,1254
,1505
,1881
,2509
,3764
,7528
,18823
,65535
};

const uint16_t debugFIPWvalues[] PROGMEM = {
 22
,22
,23
,23
,23
,24
,24
,24
,25
,26
,26
,27
,28
,29
,30
,32
,34
,36
,39
,44
,51
,63
,86
,156
,365
,0
};

const uint8_t debugVSSlength = ( sizeof(debugVSSvalues) / sizeof(uint16_t) );
const uint8_t debugFIPlength = ( sizeof(debugFIPvalues) / sizeof(uint16_t) );

uint8_t debugVSSidx;
uint8_t debugFIPidx;

uint8_t debugVSSstate;
uint8_t debugFIPstate;

uint16_t debugFIPWgoodTickLength; // to force debug injector pulse width to a maximum good engine speed-dependent value
uint16_t debugFIPWreadTickLength;

#endif // useSimulatedFIandVSS