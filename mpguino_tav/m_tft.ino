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
	DDRB |= (1 << DDB6);
	DDRL |= (1 << DDL1);
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

			if (cmd == ILI9341_SWRESET) heart::wait0(delay0Tick500ms);

			writeCommandByte(cmd);

			writeDataByteString(str, strLen - 1);

			if (cmd == ILI9341_SWRESET) heart::wait0(delay0Tick200ms);
			else if (cmd == ILI9341_SLPOUT) heart::wait0(delay0Tick200ms);

		}

	}
	while (strLen);

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

	spi::transfer(W->u8[1]);
	spi::transfer(W->u8[0]);

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
