#if defined(useTFToutput)
namespace TFT /* TFT device support section prototype */
{

	static void init(void);
	static void shutdown(void);

};

#if defined(useILI9341)
namespace ILI9341 /* ILI9341 TFT hardware support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static void writeCommandByte(uint8_t byt);
	static void writeDataByte(uint8_t byt);
	static void writeDataWord(uint16_t wrd);
	static void writeDataByteString(const uint8_t * &str, uint8_t strLen);
	static void setBrightness(uint8_t brightIdx);
	static uint8_t isCSreleased(void);
	static void releaseCS(void);
	static void assertCS(void);
	static void dataMode(void);
	static void commandMode(void);

};

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

static const uint8_t ILI9341_NOP =			0x00;
static const uint8_t ILI9341_SWRESET =		0x01;
static const uint8_t ILI9341_RDDID =		0x04;
static const uint8_t ILI9341_RDDST =		0x09;

static const uint8_t ILI9341_SLPIN =		0x10;
static const uint8_t ILI9341_SLPOUT =		0x11;
static const uint8_t ILI9341_PTLON =		0x12;
static const uint8_t ILI9341_NORON =		0x13;

static const uint8_t ILI9341_RDMODE =		0x0A;
static const uint8_t ILI9341_RDMADCTL =		0x0B;
static const uint8_t ILI9341_RDPIXFMT =		0x0C;
static const uint8_t ILI9341_RDIMGFMT =		0x0D;
static const uint8_t ILI9341_RDDSPSGNMODE =	0x0E;
static const uint8_t ILI9341_RDSELFDIAG =	0x0F;

static const uint8_t ILI9341_INVOFF =		0x20;
static const uint8_t ILI9341_INVON =		0x21;
static const uint8_t ILI9341_GAMMASET =		0x26;
static const uint8_t ILI9341_DISPOFF =		0x28;
static const uint8_t ILI9341_DISPON =		0x29;

static const uint8_t ILI9341_CASET =		0x2A;
static const uint8_t ILI9341_PASET =		0x2B;
static const uint8_t ILI9341_RAMWR =		0x2C;
static const uint8_t ILI9341_RAMRD =		0x2E;

static const uint8_t ILI9341_PTLAR =		0x30;
static const uint8_t ILI9341_MADCTL =		0x36;
static const uint8_t ILI9341_IDMOFF =		0x38;
static const uint8_t ILI9341_IDMON =		0x39;
static const uint8_t ILI9341_PIXFMT =		0x3A;

static const uint8_t ILI9341_FRMCTR1 =		0xB1;
static const uint8_t ILI9341_FRMCTR2 =		0xB2;
static const uint8_t ILI9341_FRMCTR3 =		0xB3;
static const uint8_t ILI9341_INVCTR =		0xB4;
static const uint8_t ILI9341_DFUNCTR =		0xB6;

static const uint8_t ILI9341_PWCTR1 =		0xC0;
static const uint8_t ILI9341_PWCTR2 =		0xC1;
static const uint8_t ILI9341_PWCTR3 =		0xC2;
static const uint8_t ILI9341_PWCTR4 =		0xC3;
static const uint8_t ILI9341_PWCTR5 =		0xC4;
static const uint8_t ILI9341_VMCTR1 =		0xC5;
static const uint8_t ILI9341_VMCTR2 =		0xC7;

static const uint8_t ILI9341_PWCTRLA =		0xCB;
static const uint8_t ILI9341_PWCTRLB =		0xCF;

static const uint8_t ILI9341_RDID1 =		0xDA;
static const uint8_t ILI9341_RDID2 =		0xDB;
static const uint8_t ILI9341_RDID3 =		0xDC;
static const uint8_t ILI9341_RDID4 =		0xDD;

static const uint8_t ILI9341_GMCTRP1 =		0xE0;
static const uint8_t ILI9341_GMCTRN1 =		0xE1;

static const uint8_t ILI9341_DRVTMACTL =	0xE8;
static const uint8_t ILI9341_DRVTMBCTL =	0xEA;
static const uint8_t ILI9341_PONSEQCTL =	0xED;
static const uint8_t ILI9341_UNDOCCMD1 =	0xEF;

static const uint8_t ILI9341_3GEN =			0xF2;
static const uint8_t ILI9341_PMPRAT =		0xF7;

static uint8_t SPIconfigTFT;

static const uint8_t ILI9341initCmdTable[] PROGMEM = {
	1, ILI9341_SWRESET,
	4, ILI9341_PWCTRLB, 0x00, 0x8B, 0X30,
	5, ILI9341_PONSEQCTL, 0x67, 0x03, 0X12, 0X81,
	4, ILI9341_DRVTMACTL, 0x85, 0x10, 0x7A,
	6, ILI9341_PWCTRLA, 0x39, 0x2C, 0x00, 0x34, 0x02,
	2, ILI9341_PMPRAT, 0x20,
	3, ILI9341_DRVTMBCTL, 0x00, 0x00,
	2, ILI9341_PWCTR1, 0x1B, // Power control - VRH[5:0]
	2, ILI9341_PWCTR2, 0x10, // Power control - SAP[2:0];BT[3:0]
	3, ILI9341_VMCTR1, 0x3F, 0x3C, // VCM control
	2, ILI9341_VMCTR2, 0XB7, // VCM control2
	2, ILI9341_MADCTL, 0x08, // Memory Access Control
	2, ILI9341_PIXFMT, 0x55,
	3, ILI9341_FRMCTR1, 0x00, 0x1B,
	4, ILI9341_DFUNCTR, 0x0A, 0xA2, 0x27, // Display Function Control
	2, ILI9341_3GEN, 0x00, // Gamma Function Disable
	2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
	16, ILI9341_GMCTRP1, 0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0xA9, 0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00, // Set Positive Gamma
	16, ILI9341_GMCTRN1, 0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56, 0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F, // Set Negative Gamma
	1, ILI9341_SLPOUT, // wake up
	1, ILI9341_DISPON,

//	4, ILI9341_UNDOCCMD1, 0x03, 0x80, 0x02,
	0
};

#endif // defined(useILI9341)
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

#define SPI_DATA 11		// pin 17 / PB3 / MOSI
#define SPI_CLOCK 13	// pin 19 / PB5 / SCK
#define SPI_SELECT 10	// pin 16 / PB2 / /SS
#define BRIGHTNESS

#elif defined(__AVR_ATmega2560__)

#define SPI_DATA 51		// pin 21 / PB2 / MOSI
#define SPI_CLOCK 52	// pin 20 / PB1 / SCK
#define SPI_SELECT 53	// pin 19 / PB0 / /SS
#define BRIGHTNESS 12	// pin 25 / PB6 / OC1B

/*
1 -  Vcc
2 -  GND
3 -  CS			47	//	pin 37	//	PL2			chip select active low
4 -  RESET		44	//	pin 40	//	PL5			active low
5 -  DC			48	//	pin 36	//	PL1			data/command d=1 c=0
6 -  SDI (MOSI)	51	//	pin 21	//	PB2			hardware spi
7 -  SCK		52	//	pin 20	//	PB1			hardware spi
8 -  LED		12	//	pin 25	//	PB6 / OC1B
9 -  SDO (MISO)

10 - T_CLK		11	//	pin 23	//	PB5			software spi
11 - T_CS		10	//	pin 24	//	PB4
12 - T_DIN		9	//	pin 18	//	PH6	output	software spi
13 - T_DO		8	//	pin 17	//	PH5	input	software spi
14 - T_IRQ		7	//	pin 16	//	PH4

*/

#elif defined(__AVR_ATmega32U4__)

//#define SPI_DATA xx	// pin 10 / PB2 / MOSI
//#define SPI_CLOCK xx	// pin 9  / PB1 / SCK
//#define SPI_SELECT xx	// pin 8  / PB0 / /SS
//#define BRIGHTNESS

#endif //
#endif // defined(useTFToutput)
