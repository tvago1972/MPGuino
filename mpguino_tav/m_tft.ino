#if defined(useTFToutput)

static void TFT::init(void)
{

	ILI9341::init();

}

static void TFT::shutdown(void)
{

	ILI9341::shutdown();

}

#if defined(useILI9341)
/* ILI9341 TFT hardware support section */

static void ILI9341::init(void)
{

	const uint8_t * str;
	uint8_t strLen;
	uint8_t cmd;
	uint8_t dly;

	SPIconfigTFT = spi::configuration(4000000ul, SPI_MSBFIRST, SPI_MODE0);

	// enable TFT brightness
#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	DDRB |= (1 << DDB6); // backlight enable (PB6) as output
	DDRL |= (1 << DDL1); // DC (PL1 / Arduino pin 48) as output
	DDRC |= (1 << DDC7); // hardware /RESET (PC7 / Arduino pin 30) as output

	// pulse the ILI9341 hardware reset. the controller ignores all SPI traffic
	// until it is brought out of hardware reset, so this must happen before any
	// commands are sent. RESET is on PC7 (Arduino pin 30) per the original
	// abbalooga TFT_ILI9341 User_Setup.h; timing follows that library (5/20/150ms,
	// rounded up here).
	PORTC |= (1 << PORTC7); // /RESET high (inactive)
	heart::wait0(5);
	PORTC &= ~(1 << PORTC7); // assert /RESET (active low)
	heart::wait0(20);
	PORTC |= (1 << PORTC7); // release /RESET
	heart::wait0(200); // wait for the controller to become ready
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	DDRH |= ((1 << DDH4) | (1 << DDH3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
	setBrightness(1);

	spi::set(SPIconfigTFT);

	str = ILI9341initCmdTable;

	do
	{

		strLen = pgm_read_byte(str++);

		if (strLen)
		{

			cmd = pgm_read_byte(str++);

			if (cmd == ILI9341_SWRESET) heart::wait0(500);

			writeCommandByte(cmd);

			writeDataByteString(str, strLen - 1);

			if (cmd == ILI9341_SWRESET) heart::wait0(200);
			else if (cmd == ILI9341_SLPOUT) heart::wait0(200);

		}

	}
	while (strLen);

	// --- bring-up go/no-go test: cycle solid colors so a live panel is obvious
	// and the R/G/B order can be verified. remove once the TFT UI layer exists.
	fillScreen(ILI9341_RED);
	heart::wait0(500);
	fillScreen(ILI9341_GREEN);
	heart::wait0(500);
	fillScreen(ILI9341_BLUE);
	heart::wait0(500);
	fillScreen(ILI9341_BLACK);
	// --- end bring-up test

}

static void ILI9341::shutdown(void)
{

	setBrightness(0);

	// disable TFT brightness
#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	DDRB &= ~(1 << DDB6);
	DDRL &= ~(1 << DDL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	DDRH &= ~((1 << DDH4) | (1 << DDH3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

static void ILI9341::writeCommandByte(uint8_t byt)
{

	commandMode();
	assertCS();

	spi::transfer(byt);

	releaseCS();

}

static void ILI9341::writeDataByte(uint8_t byt)
{

	dataMode();
	assertCS();

	spi::transfer(byt);

	releaseCS();

}

static void ILI9341::writeDataWord(uint16_t wrd)
{

	union union_16 * W = (union union_16 *)(&wrd);

	dataMode();
	assertCS();

	spi::transfer(W->u08[1]);
	spi::transfer(W->u08[0]);

	releaseCS();

}

static void ILI9341::writeDataByteString(const uint8_t * &str, uint8_t strLen)
{

	if (strLen)
	{

		dataMode();
		assertCS();

		while (strLen--) spi::transfer(pgm_read_byte(str++));

		releaseCS();

	}

}

// set the active drawing rectangle, then issue RAMWR so pixel data can stream
static void ILI9341::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

	writeCommandByte(ILI9341_CASET); // column address set
	writeDataByte(x0 >> 8);
	writeDataByte(x0);
	writeDataByte(x1 >> 8);
	writeDataByte(x1);

	writeCommandByte(ILI9341_PASET); // page (row) address set
	writeDataByte(y0 >> 8);
	writeDataByte(y0);
	writeDataByte(y1 >> 8);
	writeDataByte(y1);

	writeCommandByte(ILI9341_RAMWR); // memory write

}

// fill the whole panel with one RGB565 color - a visible go/no-go bring-up test
static void ILI9341::fillScreen(uint16_t color)
{

	union union_16 * c = (union union_16 *)(&color);
	uint32_t pixels = (uint32_t)(ILI9341_TFTWIDTH) * (uint32_t)(ILI9341_TFTHEIGHT);

	setAddrWindow(0, 0, ILI9341_TFTWIDTH - 1, ILI9341_TFTHEIGHT - 1);

	// stream every pixel with CS held the whole time (MSB first)
	dataMode();
	assertCS();

	while (pixels--)
	{

		spi::transfer(c->u08[1]);
		spi::transfer(c->u08[0]);

	}

	releaseCS();

}

__attribute__((always_inline))
static void ILI9341::setBrightness(uint8_t brightIdx)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	if (brightIdx) PORTB |= (1 << PORTB6);
	else PORTB &= ~(1 << PORTB6);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	if (brightIdx) PORTH |= (1 << PORTH4);
	else PORTH &= ~(1 << PORTH4);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static uint8_t ILI9341::isCSreleased(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	return (PORTL & (1 << PORTL2));
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	return (PORTE & (1 << PORTE3));
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::releaseCS(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL |= (1 << PORTL2);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTE |= (1 << PORTE3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::assertCS(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL &= ~(1 << PORTL2);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTE &= ~(1 << PORTE3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::dataMode(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL |= (1 << PORTL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTH |= (1 << PORTH3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

__attribute__((always_inline))
static void ILI9341::commandMode(void)
{

#if defined(__AVR_ATmega2560__)
#if defined(useMPGuinoColourTouch)
	PORTL &= ~(1 << PORTL1);
#endif // defined(useMPGuinoColourTouch)
#if defined(useSeeedStudioTFTtouchShield)
	PORTH &= ~(1 << PORTH3);
#endif // defined(useSeeedStudioTFTtouchShield)

#endif // defined(__AVR_ATmega2560__)
}

#endif // defined(useILI9341)
#endif // defined(useTFToutput)
