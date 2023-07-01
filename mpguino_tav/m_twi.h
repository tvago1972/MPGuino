#if defined(useTWIsupport)
#include <compat/twi.h>
namespace TWI
{

	static void init(void);
	static void shutdown(void);
	static void openChannelMain(uint8_t address, uint8_t writeFlag);
	static void openChannel(uint8_t address, uint8_t writeFlag);
	static uint8_t writeByte(uint8_t data);
	static void transmitChannel(uint8_t sendStop);
#if defined(useInterruptBasedTWI)
	static void disableISRactivity(void);
	static void enableISRactivity(void);
#endif // defined(useInterruptBasedTWI)

}

const uint8_t TWI_READ =				0;
const uint8_t TWI_WRITE =				1;

const uint8_t TWI_REPEAT_START =		0;
const uint8_t TWI_STOP =				1;

const unsigned int twiFrequency = 100L;
const uint8_t twiDataBufferSize = 16;

static uint8_t twiDataBuffer[twiDataBufferSize];

static volatile uint8_t twiStatusFlags;
static volatile uint8_t twiSlaveAddress;
static volatile uint8_t twiErrorCode;
static volatile uint8_t twiDataBufferIdx;
static volatile uint8_t twiDataBufferLen;

static const uint8_t twiBlockMainProgram =	0b10000000;
static const uint8_t twiAllowISRactivity =	0b01000000;
static const uint8_t twiOpen =				0b00100000;
static const uint8_t twiFinished =			0b00010000;
static const uint8_t twiClose =				0b00001000;
static const uint8_t twiRemainOpen =		0b00000100;
static const uint8_t twiErrorFlag =			0b00000010;

static const uint8_t twiOpenMain =			(twiOpen | twiBlockMainProgram);

#endif // defined(useTWIsupport)
#if defined(useMCP23017portExpander)
namespace MCP23017portExpanderSupport
{

	static void init(void);
	static void configureOutputPort(uint16_t registerValue);
	static void writeRegister16Bit(uint8_t registerAddress, uint16_t registerValue);
	static void writeRegister16Bit(uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte);
	static void writeRegister8Bit(uint8_t registerAddress, uint8_t portByte);
	static void setTransferMode(uint8_t mode);

}

const uint8_t MCP23017_B0_IODIRx =		0x00;
const uint8_t MCP23017_B0_IPOLx =		0x02;
const uint8_t MCP23017_B0_GPINTENx =	0x04;
const uint8_t MCP23017_B0_DEFVALx =		0x06;
const uint8_t MCP23017_B0_INTCONx =		0x08;
const uint8_t MCP23017_B0_IOCON =		0x0A;
const uint8_t MCP23017_B0_GPPUx =		0x0C;
const uint8_t MCP23017_B0_INTFx =		0x0E;
const uint8_t MCP23017_B0_INTCAPx =		0x10;
const uint8_t MCP23017_B0_GPIOx =		0x12;
const uint8_t MCP23017_B0_OLATx =		0x14;

const uint8_t MCP23017_B0_PORTA =		0x00;
const uint8_t MCP23017_B0_PORTB =		0x01;

const uint8_t MCP23017_B1_IODIRA =		0x00;
const uint8_t MCP23017_B1_IPOLA =		0x01;
const uint8_t MCP23017_B1_GPINTENA =	0x02;
const uint8_t MCP23017_B1_DEFVALA =		0x03;
const uint8_t MCP23017_B1_INTCONA =		0x04;
const uint8_t MCP23017_B1_IOCON =		0x05;
const uint8_t MCP23017_B1_GPPUA =		0x06;
const uint8_t MCP23017_B1_INTFA =		0x07;
const uint8_t MCP23017_B1_INTCAPA =		0x08;
const uint8_t MCP23017_B1_GPIOA =		0x09;
const uint8_t MCP23017_B1_OLATA =		0x0A;
const uint8_t MCP23017_B1_IODIRB =		0x10;
const uint8_t MCP23017_B1_IPOLB =		0x11;
const uint8_t MCP23017_B1_GPINTENB =	0x12;
const uint8_t MCP23017_B1_DEFVALB =		0x13;
const uint8_t MCP23017_B1_INTCONB =		0x14;
const uint8_t MCP23017_B1_GPPUB =		0x16;
const uint8_t MCP23017_B1_INTFB =		0x17;
const uint8_t MCP23017_B1_INTCAPB =		0x18;
const uint8_t MCP23017_B1_GPIOB =		0x19;
const uint8_t MCP23017_B1_OLATB =		0x1A;

volatile uint8_t adafruitRGBLCDflags;

const uint8_t afRGBLCDbankMode =		0b10000000;
const uint8_t afRGBLCDmirrorMode =		0b01000000;
const uint8_t afRGBLCDbyteMode =		0b00100000;

const uint8_t adaTWIsequentialMode =	0x00;
const uint8_t adaTWIbyteMode =			0x01;
const uint8_t adaTWItoggleMode =		0x02;

const uint8_t adafruitRGBLCDaddress =	0x20;

#endif // defined(useMCP23017portExpander)
