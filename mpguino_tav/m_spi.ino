#if defined(useHardwareSPI)
/* Serial Peripheral Interface support section */

static void spi::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega32U4__)
	PRR0 &= ~(1 << PRSPI);  // turn on SPI hardware

	PORTB |= (1 << PORTB0); // ensure /SS is set high to ensure SPI is set to master when it is enabled
#if defined(useBluetoothAdaFruitSPI)
	PORTB &= ~(1 << PORTB4); // disable CS when this pin becomes an output
	PORTD |= (1 << PORTD4); // force /RST high when this pin becomes an output
#endif // defined(useBluetoothAdaFruitSPI)

	DDRB &= ~((1 << DDB3) | (1 << DDB2) | (1 << DDB1)); // force MISO, MOSI, and SCLK as input pins
#if defined(useBluetoothAdaFruitSPI)
	DDRE &= ~(1 << DDE6); // turn IRQ to an input pin
#endif // defined(useBluetoothAdaFruitSPI)
	DDRB |= (1 << DDB0); // enable /SS as output pin
#if defined(useBluetoothAdaFruitSPI)
	DDRB |= (1 << DDB4); // turn CS to an output pin
	DDRD |= (1 << DDD4); // turn /RST to an output pin
#endif // defined(useBluetoothAdaFruitSPI)

	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI and set as master

	DDRB |= ((1 << DDB2) | (1 << DDB1)); // enable MOSI and SCLK as output pins (this order prevents inadvertent SPI behavior when SPI is enabled)

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRSPI); // turn on SPI hardware

	PORTB |= (1 << PORTB0); // ensure /SS is set high to ensure SPI is set to master when it is enabled
#if defined(useBluetoothAdaFruitSPI)
	PORTH &= ~(1 << PORTH5); // disable CS when this pin becomes an output
	PORTG |= (1 << PORTG5); // force /RST high when this pin becomes an output
#endif // defined(useBluetoothAdaFruitSPI)

	DDRB &= ~((1 << DDB3) | (1 << DDB2) | (1 << DDB1)); // force MISO, MOSI, and SCLK as input pins
#if defined(useBluetoothAdaFruitSPI)
	DDRH &= ~(1 << DDH4); // turn IRQ to an input pin
#endif // defined(useBluetoothAdaFruitSPI)
	DDRB |= (1 << DDB0); // enable /SS as output pin
#if defined(useBluetoothAdaFruitSPI)
	DDRH |= (1 << DDH5); // turn CS to an output pin
	DDRG |= (1 << DDG5); // turn /RST to an output pin
#endif // defined(useBluetoothAdaFruitSPI)

	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI and set as master

	DDRB |= ((1 << DDB2) | (1 << DDB1)); // enable MOSI and SCLK as output pins (this order prevents inadvertent SPI behavior when SPI is enabled)

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRSPI); // turn on SPI hardware

	PORTB |= (1 << PORTB2); // ensure /SS is set high to ensure SPI is set to master when it is enabled
#if defined(useBluetoothAdaFruitSPI)
	PORTB &= ~(1 << PORTB0); // disable CS when this pin becomes an output
	PORTD |= (1 << PORTD4); // force /RST high when this pin becomes an output
#endif // defined(useBluetoothAdaFruitSPI)

	DDRB &= ~((1 << DDB5) | (1 << DDB4) | (1 << DDB3)); // force MISO, MOSI, and SCLK as input pins
#if defined(useBluetoothAdaFruitSPI)
	DDRD &= ~(1 << DDD7); // turn IRQ to an input pin
#endif // defined(useBluetoothAdaFruitSPI)
	DDRB |= (1 << DDB2); // enable /SS as output pin
#if defined(useBluetoothAdaFruitSPI)
	DDRB |= (1 << DDB0); // turn CS to an output pin
	DDRD |= (1 << DDD4); // turn /RST to an output pin
#endif // defined(useBluetoothAdaFruitSPI)

	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI and set as master

	DDRB |= ((1 << DDB5) | (1 << DDB3)); // enable MOSI and SCLK as output pins (this order prevents inadvertent SPI behavior when SPI is enabled)

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

}

static void spi::shutdown(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega32U4__)
	SPCR &= ~((1 << SPE) | (1 << MSTR)); // disable SPI

	PORTB &= ~(1 << PORTB0); // make /SS output as low

	DDRB &= ~((1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0)); // turn /SS, MISO, MOSI, and SCLK into input pins

#if defined(useBluetoothAdaFruitSPI)
	PORTB &= ~(1 << PORTB4);
	PORTE &= ~(1 << PORTD6);
	PORTD &= ~(1 << PORTD4);

	DDRB &= ~(1 << DDB4); // turn CS to an input pin
	DDRE &= ~(1 << DDE6); // turn IRQ to an input pin
	DDRD &= ~(1 << DDD4); // turn /RST to an input pin

#endif // defined(useBluetoothAdaFruitSPI)
	PRR0 |= (1 << PRSPI); // turn off SPI hardware to conserve power

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	SPCR &= ~((1 << SPE) | (1 << MSTR)); // disable SPI

	PORTB &= ~(1 << PORTB0); // make /SS output as low

	DDRB &= ~((1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0)); // turn /SS, MISO, MOSI, and SCLK into input pins

#if defined(useBluetoothAdaFruitSPI)
	PORTH &= ~((1 << PORTH5) | (1 << PORTH4));
	PORTG &= ~(1 << PORTG5);

	DDRH &= ~((1 << DDH5) | (1 << DDH4)); // turn CS and IRQ to input pins
	DDRG &= ~(1 << DDG5); // turn /RST to an input pin

#endif // defined(useBluetoothAdaFruitSPI)
	PRR0 |= (1 << PRSPI); // turn off SPI hardware to conserve power

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	SPCR &= ~((1 << SPE) | (1 << MSTR)); // disable SPI

	PORTB &= ~(1 << PORTB2); // make /SS output as low

	DDRB &= ~((1 << DDB5) | (1 << DDB4) | (1 << DDB3) | (1 << DDB2)); // turn /SS, MISO, MOSI, and SCLK into input pins

#if defined(useBluetoothAdaFruitSPI)
	PORTB &= ~(1 << PORTB0);
	PORTD &= ~((1 << PORTD7) | (1 << PORTD4));

	DDRB |= (1 << DDB0); // turn CS to an input pin
	DDRD &= ~((1 << DDD7) | (1 << DDD4)); // turn IRQ and /RST to input pins

#endif // defined(useBluetoothAdaFruitSPI)
	PRR |= (1 << PRSPI); // turn off SPI hardware to conserve power

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

}

/*
this function generates a configuration byte value that can then be used to set the various hardware SPI parameters

byte format:

xxxx xfff - frequency divider
			xxxx x001 - f/2
			xxxx x000 - f/4
			xxxx x011 - f/8
			xxxx x010 - f/16
			xxxx x101 - f/32
			xxxx x100 - f/64
			xxxx x111 - f/64
			xxxx x110 - f/128
xxxm mxxx - mode
			xxx0 0xxx - SCK idle low, data sampled on leading edge
			xxx0 1xxx - SCK idle low, data sampled on trailing edge
			xxx1 0xxx - SCK idle high, data sampled on leading edge
			xxx1 1xxx - SCK idle high, data sampled on trailing edge
xdxx xxxx - bit direction
			x0xx xxxx - high bit transmitted first
			x1xx xxxx - low bit transmitted first

*/
static uint8_t spi::configuration(uint32_t maxClock, uint8_t bitOrder, uint8_t dataMode)
{

	uint8_t configVal;
	uint32_t clockDivValue;

	clockDivValue = F_CPU / 2;
	configVal = 0;

	while ((configVal < 6) && (maxClock < clockDivValue)) // find the most suitable value of n for f(osc)/n
	{

		clockDivValue >>= 1;
		configVal++;

	}

	if (configVal == 6) configVal = 7; // compensate for the duplicate f(osc)/64

	configVal ^= 0x1; // Invert the SPI2X bit
	configVal |= (bitOrder & SPI_DIR_MASK); // add bit direction bit
	configVal |= (dataMode & SPI_MODE_MASK); // add clock phase and polarity bits

	return configVal;

}

static void spi::set(uint8_t SPIconfiguration)
{

	uint8_t i;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	i = (SPIconfiguration >> 1); // fetch SPI clock divider, clock phase, clock polarity, and data direction bits
	i |= ((1 << SPE) | (1 << MSTR)); // ensure that SPI enable and SPI master bits are set
	SPIconfiguration &= (1 << SPI2X); // strip out all but the SPI double speed bit setting

	SPCR = i; // set SPI control register to passed-in configuration
	SPSR = SPIconfiguration; // set SPI double-speed bit to passed-in setting

	SREG = oldSREG; // restore interrupt flag status

}

static uint8_t spi::transfer(uint8_t byt)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	SPDR = byt;

	asm volatile("nop"); // small delay that can prevent wait loop at maximum speed, not noticed otherwise

	while ((SPSR & (1 << SPIF)) == 0); // wait

	byt = SPDR;

	SREG = oldSREG; // restore interrupt flag status

	return byt;

}

#if defined(useBluetoothAdaFruitSPI)
/* AdaFruit BLE Friend support section */

static void blefriend::init(void)
{

	SPIconfigBluetooth = spi::configuration(4000000ul, SPI_MSBFIRST, SPI_MODE0);

	text::initDev(devIdxBLEfriend, (odvFlagCRLF | odvFlagEnableOutput), chrOut);

	heart::changeBitFlagBits(bfBLEstatus, 0, (bleResetFlags)); // request hardware reset

}

static void blefriend::shutdown(void)
{
}

static void blefriend::chrOut(uint8_t chr)
{

	ringBuffer::push(rbIdxBLEfriendOut, chr);

}

// returns 1 if there are still outstanding packets to be read-in, 0 otherwise
static uint8_t blefriend::outputBufferWithResponse(void)
{

	uint8_t f;
	uint8_t k;

	f = 0;
	k = outputBuffer(SDEP_CMDTYPE_AT_WRAPPER); // transfer entire contents of output buffer, as an AT wrapper

	while (k) // if output buffer successfully transmitted
	{

		if (inputCheck()) // if IRQ is pulled high
		{

			if (readPacket()) // read in a packet, and look at return flag to see if any packets remain
			{

				// if remaining space is less than packet payload size, finish anyhow
				if (ringBuffer::free(rbIdxBLEfriendIn) < 16)
				{

					f = 1;
					k = 0;

				}

			}
			else k = 0; // otherwise, all input packets have been read

		}
		else // otherwise, IRQ hasn't yet been pulled high
		{

			idleMainProcess(); // perform idle process housekeeping
			k = (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting); // update loop flag with packet wait delay status

		}

	}

	return f;

}

// returns 1 if the output buffer was successfully transmitted out, or 0 if a timeout occurred
static uint8_t blefriend::outputBuffer(uint16_t cmdWord)
{

	uint16_t outLen;
	uint8_t i;
	uint8_t j;
	uint8_t k;

	outLen = ringBuffer::length(rbIdxBLEfriendOut);

	do
	{

		if (outLen > 16) j = 0x80 + 16; // set partial length with 'more data' bit set
		else j = outLen; // get complete length

		k = writePacketHeader(cmdWord, j); // write packet header

		if (k)
		{

			j &= 0x1F; // strip 'more data' bit from partial length
			outLen -= j;

			while (j--) spi::transfer(ringBuffer::pull(rbIdxBLEfriendOut)); // output a byte of SDEP payload

		}

	}
	while ((k) && (outLen));

	releaseCS();

	return k;

}

// returns 1 if a packet header was successfully transmitted out, or 0 if a timeout occurred
static uint8_t blefriend::writePacketHeader(uint16_t cmdWord, uint8_t loadLen)
{

	union union_16 * cW = (union union_16 *)(&cmdWord);
	uint8_t k;

	spi::set(SPIconfigBluetooth); // ensure SPI is set for bluetooth transceiver

	heart::changeBitFlagBits(bfBLEstatus, 0, (blePacketWaitFlags)); // start tranceiver delay

//	text::hexByteOut(devIdxDebugTerminal, outLen);
//	text::newLine(devIdxDebugTerminal);

	do
	{

		do
		{

			heart::changeBitFlagBits(bfBLEstatus, 0, (bleAssertFlags)); // assert chip select

			while (bitFlags[(uint16_t)(bfBLEstatus)] & bleAsserting);

			k = spi::transfer(SPI_COMMAND_BYTE); // transmit command byte

			if (k == SPI_IGNORED_BYTE) // BLE board might not be ready
			{

				if (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) k = 1; // if we're still within the response delay period, signal to retry
				else k = 0; // otherwise, we ran out of time

			}
			else k = 0; // otherwise, we probably successfully transmitted the command byte

		}
		while (k); // if retrying, loop back

		if (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) // if still within valid packet delay
		{

			spi::transfer(cW->u8[0]); // output SDEP command high byte
			spi::transfer(cW->u8[1]); // output SDEP command low byte

			spi::transfer(loadLen); // output SDEP payload length

			if (loadLen == 0) releaseCS();

		}
		else k = 1;

	}
	while ((bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) && (k));

	return (k == 0); // return a 1 upon successful transmission of a packet header, or 0 on timeout

}

// returns 0x80 if there is more to be read, otherwise 0
static uint8_t blefriend::readPacket(void)
{

	uint16_t cmdWord;
	uint8_t outLen;
	uint8_t partialPayload;
	uint8_t i;
	uint8_t j;
	uint8_t k;
	union union_16 * cW = (union union_16 *)(&cmdWord);

	partialPayload = 0;

	if (inputCheck())
	{

		spi::set(SPIconfigBluetooth); // ensure SPI is set for bluetooth transceiver

		heart::changeBitFlagBits(bfBLEstatus, 0, (blePacketWaitFlags)); // start tranceiver delay

		do
		{

			do
			{

				heart::changeBitFlagBits(bfBLEstatus, 0, (bleAssertFlags)); //  assert chip select

				while (bitFlags[(uint16_t)(bfBLEstatus)] & bleAsserting);

				k = spi::transfer(0xFF); // transmit dummy byte

				if ((k == SPI_IGNORED_BYTE) || (k == SPI_OVERREAD_BYTE)) // BLE board might not be ready
				{

					if (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) i = 1; // if we're still within the response delay period, signal to retry
					else i = 0; // otherwise, we ran out of time

				}
				else i = 0; // otherwise, we either successfully transmitted the command byte, or ran out of time

			}
			while (i); // if retrying, loop back

			if (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) // if still within valid packet delay
			{

				cW->u8[0] = spi::transfer(0xFF); // transmit dummy byte to fetch command word low byte
				cW->u8[1] = spi::transfer(0xFF); // transmit dummy byte to fetch command word high byte
				outLen = spi::transfer(0xFF); // transmit dummy byte to fetch packet length
				i = 1;

				switch (k) // catch invalid message types
				{

					case SPI_RESPONSE_BYTE:
						if ((cmdWord != SDEP_CMDTYPE_AT_WRAPPER) && (cmdWord != SDEP_CMDTYPE_BLE_UARTTX) && (cmdWord != SDEP_CMDTYPE_BLE_UARTRX))
						{

							ringBuffer::push(rbIdxBLEfriendIn, '*');
							while (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) spi::transfer(0xFF); // transmit dummy byte

						}
						else
						{

							partialPayload = outLen & 0x80;
							j = outLen & 0x1F; // strip off non-relevant bits

							if ((j > 16) || ((j < 16) && (partialPayload))) // if this is an invalid message length
							{

								ringBuffer::push(rbIdxBLEfriendIn, '&');
								partialPayload = 0;

								while (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) spi::transfer(0xFF); // transmit dummy byte

							}
							else
							{

								while (j--) ringBuffer::push(rbIdxBLEfriendIn, spi::transfer(0xFF)); // read and push a character of response

								i = 0;

							}

						}

						break;

					case SPI_ERROR_BYTE:
						pushByte(k); // push message type
						pushByte(cW->u8[1]); // push command word high byte
						pushByte(cW->u8[0]); // push command word high byte
						pushByte(outLen); // push reserved byte
						i = 0;
						break;

					default:
						ringBuffer::push(rbIdxBLEfriendIn, '!');
						while (bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) spi::transfer(0xFF); // transmit dummy byte
						break;

				}

			}

		}
		while ((bitFlags[(uint16_t)(bfBLEstatus)] & blePacketWaiting) && (i));

		releaseCS();

	}

	return partialPayload;

}

static uint8_t blefriend::inputCheck(void)
{

	uint8_t btStatus;

#if defined(__AVR_ATmega32U4__)
	btStatus = (PINE & (1 << PINE6)); // read IRQ input pin
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	btStatus = (PINH & (1 << PINH4)); // read IRQ input pin
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	btStatus = (PIND & (1 << PIND7)); // read IRQ input pin
#endif // defined(__AVR_ATmega328P__)

	return btStatus;

}

static uint8_t blefriend::isCSreleased(void)
{

#if defined(__AVR_ATmega32U4__)
	return (PORTB & (1 << PORTB4));
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	return (PORTH & (1 << PORTH5));
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	return (PORTB & (1 << PORTB0));
#endif // defined(__AVR_ATmega328P__)

}
static void blefriend::releaseCS(void)
{

#if defined(__AVR_ATmega32U4__)
	PORTB |= (1 << PORTB4);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PORTH |= (1 << PORTH5);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PORTB |= (1 << PORTB0);
#endif // defined(__AVR_ATmega328P__)

}

static void blefriend::assertCS(void)
{

#if defined(__AVR_ATmega32U4__)
	PORTB &= ~(1 << PORTB4);
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PORTH &= ~(1 << PORTH5);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PORTB &= ~(1 << PORTB0);
#endif // defined(__AVR_ATmega328P__)

}

static void blefriend::pushByte(uint8_t chr)
{

	ringBuffer::push(rbIdxBLEfriendIn, text::nybble(chr >> 4));
	ringBuffer::push(rbIdxBLEfriendIn, text::nybble(chr));

}

#endif // defined(useBluetoothAdaFruitSPI)
#endif // defined(useHardwareSPI)
