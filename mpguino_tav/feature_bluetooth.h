#if defined(useBluetooth)
typedef struct
{

	uint8_t btFuncChar;
	uint16_t btFuncFormat;

} bluetoothFunction;

namespace bluetooth /* Bluetooth interface terminal section prototype */
{

	static void init(void);
	static void shutdown(void);
#if defined(useBluetoothAdaFruitSPI)
	static uint8_t chrIn(void);
#endif // defined(useBluetoothAdaFruitSPI)
	static uint16_t findFormat(uint8_t inpChar);
	static void mainProcess(void);
	static void mainOutput(void);

}

/*

list of commands sent from MPGuino Blue android app

MM                - output selected EEPROM parameter values
MR                - reset current trip and save to EEPROM
MT                - reset tank trip (and partial, if configured) and save to EEPROM
!                 - initialize and output selected trip functions
RdddddddddddK     - store pPulsesPerDistanceIdx value to EEPROM
SdddddddddddK     - store pMicroSecondsPerGallonIdx value to EEPROM
TdddddddddddK     - store pInjPer2CrankRevIdx value to EEPROM
UdddddddddddK     - store pMetricModeIdx value to EEPROM
VdddddddddddK     - store pTankSizeIdx value to EEPROM
WdddddddddddK     - store pInjectorSettleTimeIdx value to EEPROM
XdddddddddddK     - store pVSSpauseIdx value to EEPROM
YdddddddddddK     - store pInjEdgeTriggerIdx value to EEPROM
ZdddddddddddK     - store pVoltageOffset value to EEPROM
@dddddddddddK     - store pRefuelSizeIdx value to EEPROM

list of items that get output to MPGuino Blue android app

output set from ! command
-------------------------
Uddddddddddd      - pMetricModeIdx
Addddddddddd      - Instant vehicle speed
Bddddddddddd      - Alternator voltage
Cddddddddddd      - Tank fuel quantity consumed
Dddddddddddd      - Current distance traveled
Eddddddddddd      - Tank distance traveled
Fddddddddddd      - Tank distance to empty
Gddddddddddd      - Instant engine speed
Hddddddddddd      - Instant fuel economy if moving / Instant fuel flow rate if stopped
Iddddddddddd      - Current fuel economy
Jddddddddddd      - Tank fuel economy
Lddddddddddd      - Current fuel quantity consumed

output set from MM command
--------------------------
Rddddddddddd      - pPulsesPerDistanceIdx
Sddddddddddd      - pMicroSecondsPerGallonIdx
Tddddddddddd      - pInjPer2CrankRevIdx
Uddddddddddd      - pMetricModeIdx
Vddddddddddd      - pTankSizeIdx
Wddddddddddd      - pInjectorSettleTimeIdx
Xddddddddddd      - pVSSpauseIdx
Yddddddddddd      - pInjEdgeTriggerIdx
Zddddddddddd      - pVoltageOffset

*/

static uint8_t btInputState;

static const uint8_t btiResetFlag =				0b10000000;
static const uint8_t btiStringInput =			0b01000000;
static const uint8_t btiGetSecondaryCharacter =	0b00100000;
static const uint8_t btiAllowPolling =			0b00010000;

static uint8_t btOutputState;

static const uint8_t btoFlagActiveOutput =		0b10000000;
static const uint8_t btoFlagSingleShotOutput =	0b01000000;
static const uint8_t btoFlagContinuousOutput =	0b00100000;
static const uint8_t btoFlagDelay =				0b00010000;
static const uint8_t btoFlagFlushBuffer =		0b00001000;

static const uint8_t btoOutputFlags =			(btoFlagActiveOutput | btoFlagSingleShotOutput | btoFlagContinuousOutput | btoFlagDelay | btoFlagFlushBuffer);
static const uint8_t btoOutputEnabledFlags =	(btoFlagSingleShotOutput | btoFlagContinuousOutput);
static const uint8_t btoOutputActiveFlags =		(btoFlagActiveOutput | btoFlagDelay | btoFlagFlushBuffer);

static uint8_t btOutputListIdx;

static const uint8_t btolTripFunctionIdx =		0;
static const uint8_t btolParameterIdx =			btolTripFunctionIdx + 1;

static char btInpBuff[20];
static uint8_t btInpBuffIdx;
static const char * btOutputStringPtr;

static const char btOutputList[] PROGMEM = { // any undefined letters simply get ignored - handy for optional compile-time features
	"UABCDEFGHIJL" tcEOS
	"RSTUVWXYZ" tcEOS
};

static const bluetoothFunction btFunctionList[] PROGMEM = {
	{'A',	((instantIdx << 8 ) |					(tSpeed))},
#if defined(useAlternatorVoltage)
	{'B',	(										(tAlternatorChannel))},
#endif // defined(useAlternatorVoltage)
	{'C',	((tankIdx << 8 ) |						(tFuelUsed))},
	{'D',	((currentIdx << 8 ) |					(tDistance))},
	{'E',	((tankIdx << 8 ) |						(tDistance))},
	{'F',	((tankIdx << 8 ) |						(tDistanceToEmpty))},
	{'G',	((instantIdx << 8 ) |					(tEngineSpeed))},
	{'H',	((instantIdx << 8 ) |					(tFuelEcon))},
	{'I',	((currentIdx << 8 ) |					(tFuelEcon))},
	{'J',	((tankIdx << 8 ) |						(tFuelEcon))},
	{'L',	((currentIdx << 8 ) |					(tFuelUsed))},
	{'!',	((m32BluetoothMainValue << 8 ) |		(tGetProgramVariableValue))},

#if defined(usePartialRefuel)
	{'@',	((pRefuelSizeIdx << 8 ) |				(tGetBTparameterValue))},
#endif // defined(usePartialRefuel)
	{'R',	((pPulsesPerDistanceIdx << 8 ) |		(tGetBTparameterValue))},
	{'S',	((pMicroSecondsPerGallonIdx << 8 ) |	(tGetBTparameterValue))},
	{'T',	((pInjPer2CrankRevIdx << 8 ) |			(tGetBTparameterValue))},
	{'U',	((pMetricModeIdx << 8 ) |				(tGetBTparameterValue))},
	{'V',	((pTankSizeIdx << 8 ) |					(tGetBTparameterValue))},
	{'W',	((pInjectorSettleTimeIdx << 8 ) |		(tGetBTparameterValue))},
	{'X',	((pVSSpauseIdx << 8 ) |					(tGetBTparameterValue))},
	{'Y',	((pInjEdgeTriggerIdx << 8 ) |			(tGetBTparameterValue))},
#if defined(useAlternatorVoltage)
	{'Z',	((pVoltageOffset << 8 ) |				(tGetBTparameterValue))},
#endif // defined(useAlternatorVoltage)
};

static const uint8_t btFunctionListLength = (sizeof(btFunctionList) / sizeof(bluetoothFunction));

#if defined(useBluetoothAdaFruitSPI)
static const uint8_t btResetString[] PROGMEM = {
	"AT+GATTCLEAR" tcEOS
	"AT+GATTADDSERVICE=UUID=0XFFE0" tcEOS
	"AT+GATTADDCHAR=UUID=0XFFE1,PROPERTIES=0X16,MIN_LEN=1,MAX_LEN=32,DATATYPE=STRING,DESCRIPTION=MPGuino" tcEOS
	"ATZ" tcEOS
};

static const char btOutputString[] PROGMEM = {
	"AT+GATTCHAR=1," tcEOS
};

static const char btInputString[] PROGMEM = {
	"AT+GATTCHAR=1" tcEOS
};

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useBluetooth)
