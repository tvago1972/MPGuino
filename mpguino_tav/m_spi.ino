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

	DDRB &= ~((1 << DDB2) | (1 << DDB1)); // force MOSI and SCLK as input pins
	DDRB |= (1 << DDB0); // enable /SS as output pin

	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI and set as master

	DDRB |= ((1 << DDB2) | (1 << DDB1)); // enable MOSI and SCLK as output pins (this order prevents inadvertent SPI behavior when SPI is enabled)

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRSPI); // turn on SPI hardware

	PORTB |= (1 << PORTB0); // ensure /SS is set high to ensure SPI is set to master when it is enabled

	DDRB &= ~((1 << DDB2) | (1 << DDB1)); // force MOSI and SCLK as input pins
	DDRB |= (1 << DDB0); // enable /SS as output pin

	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI and set as master

	DDRB |= ((1 << DDB2) | (1 << DDB1)); // enable MOSI and SCLK as output pins (this order prevents inadvertent SPI behavior when SPI is enabled)

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRSPI); // turn on SPI hardware

	PORTB |= (1 << PORTB2); // ensure /SS is set high to ensure SPI is set to master when it is enabled

	DDRB &= ~((1 << DDB5) | (1 << DDB3)); // force MOSI and SCLK as input pins
	DDRB |= (1 << DDB2); // enable /SS as output pin

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

	DDRB &= ~((1 << DDB2) | (1 << DDB1) | (1 << DDB0)); // turn /SS, MOSI, and SCLK into input pins

	PORTB &= ~(1 << PORTB0); // make /SS output as low

	PRR0 |= (1 << PRSPI); // turn off SPI hardware to conserve power

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	SPCR &= ~((1 << SPE) | (1 << MSTR)); // disable SPI

	DDRB &= ~((1 << DDB2) | (1 << DDB1) | (1 << DDB0)); // turn /SS, MOSI, and SCLK into input pins

	PORTB &= ~(1 << PORTB0); // make /SS output as low

	PRR0 |= (1 << PRSPI); // turn off SPI hardware to conserve power

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	SPCR &= ~((1 << SPE) | (1 << MSTR)); // disable SPI

	DDRB &= ~((1 << DDB5) | (1 << DDB3) | (1 << DDB2)); // turn /SS, MOSI, and SCLK into input pins

	PORTB &= ~(1 << PORTB2); // make /SS output as low

	PRR |= (1 << PRSPI); // turn off SPI hardware to conserve power

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

}

#endif // defined(useHardwareSPI)
