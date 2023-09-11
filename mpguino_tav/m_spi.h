#if defined(useHardwareSPI)
namespace spi /* Serial Peripheral Interface support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static uint8_t configuration(uint32_t maxClock, uint8_t bitOrder, uint8_t dataMode);
	static void set(uint8_t SPIconfiguration);
	static uint8_t transfer(uint8_t byt);

};

static const uint8_t SPI_LSBFIRST =	0x40;	// DORD = bit 5 on SPCR
static const uint8_t SPI_MSBFIRST =	0x00;	// DORD = bit 5 on SPCR

static const uint8_t SPI_MODE0 =	0x00;	// CPOL = bit 3, CPHA = bit 2 on SPCR
static const uint8_t SPI_MODE1 =	0x08;	// CPOL = bit 3, CPHA = bit 2 on SPCR
static const uint8_t SPI_MODE2 =	0x10;	// CPOL = bit 3, CPHA = bit 2 on SPCR
static const uint8_t SPI_MODE3 =	0x18;	// CPOL = bit 3, CPHA = bit 2 on SPCR

static const uint8_t SPI_DIR_MASK =		0x40;
static const uint8_t SPI_MODE_MASK =	0x18;

#if defined(useBluetoothAdaFruitSPI)
namespace blefriend /* AdaFruit BLE Friend support section prototype */
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
	static uint8_t outputBufferWithResponse(void);
	static uint8_t outputBuffer(uint16_t cmdWord);
	static uint8_t writePacketHeader(uint16_t cmdWord, uint8_t loadLen);
	static uint8_t readPacket(void);
	static uint8_t inputCheck(void);
	static uint8_t isCSreleased(void);
	static void releaseCS(void);
	static void assertCS(void);
	static void pushByte(uint8_t chr);

};

static uint8_t SPIconfigBluetooth;

static const uint16_t SDEP_CMDTYPE_INITIALIZE =	0xBEEF;
static const uint16_t SDEP_CMDTYPE_AT_WRAPPER =	0x0A00;
static const uint16_t SDEP_CMDTYPE_BLE_UARTTX =	0x0A01;
static const uint16_t SDEP_CMDTYPE_BLE_UARTRX =	0x0A02;

static const uint8_t SPI_COMMAND_BYTE =		0x10;
static const uint8_t SPI_RESPONSE_BYTE =	0x20;
static const uint8_t SPI_ALERT_BYTE =		0x40;
static const uint8_t SPI_ERROR_BYTE =		0x80;
static const uint8_t SPI_IGNORED_BYTE =		0xFE;
static const uint8_t SPI_OVERREAD_BYTE =	0xFF;

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useHardwareSPI)
