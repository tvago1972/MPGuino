#if defined(useSerial0Port)
static void serial0::init(void)
{

	uint16_t myubbr0;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(useBufferedSerial0Port)
	ringBuffer::init(serial0Buffer, serial0Data);

#endif // defined(useBufferedSerial0Port)
#if defined(useSerial0PortInput)
	ringBuffer::init(serial0InputBuffer, serial0InputData);

#endif // defined(useSerial0PortInput)
	// turn on USART0 transmitter
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRUSART0);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRUSART0);
#endif // defined(__AVR_ATmega328P__)

	// calculate initial baudrate setting
	myubbr0 = (F_CPU / 4 / myBaudRate0 - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR0A = (1 << U2X0);

	if (myubbr0 > 4095)
	{

		UCSR0A = 0;
		myubbr0 = (F_CPU / 8 / myBaudRate0 - 1) / 2;

	}

	// set USART0 baud rate
	UBRR0H = (uint8_t)(myubbr0 >> 8);
	UBRR0L = (uint8_t)(myubbr0);

	// set for 8 data bits, no parity, and 1 stop bit
	UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));

#if defined(useSerial0PortInput)
	UCSR0B |= ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)); // enable USART0 receive complete interrupt, receiver, and transmitter
#else // defined(useSerial0PortInput)
	UCSR0B |= (1 << TXEN0); // enable USART0 transmitter
#endif // defined(useSerial0PortInput)

	devSerial0.chrOut = chrOut;
#if defined(useSerial0PortInput)
	devSerial0.chrIn = chrIn;
#endif // defined(useSerial0PortInput)
	devSerial0.controlFlags |= (odvFlagCRLF | odvFlagEnableOutput);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial0::shutdown(void)
{

	UCSR0B &= ~((1 << UDRIE0) | (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)); // disable USART0 I/O and utilized interrupts
#if defined(__AVR_ATmega2560__)
	PRR0 |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR |= (1 << PRUSART0); // shut off USART0 for power reduction
#endif // defined(__AVR_ATmega328P__)

}

static void serial0::chrOut(uint8_t chr)
{

#if defined(useBufferedSerial0Port)
	ringBuffer::push(serial0Buffer, chr);
	UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
#else // defined(useBufferedSerial0Port)
	while ((UCSR0A & (1 << UDRE0)) == 0) idleProcess(); // wait until USART0 data buffer is empty

	UCSR0A &= ~(1 << TXC0); // clear transmit complete flag
	UDR0 = chr; //send the data
#endif // defined(useBufferedSerial0Port)

}

#if defined(useSerial0PortInput)
static uint8_t serial0::chrIn(void)
{

	return ringBuffer::pullMain(serial0InputBuffer);

}

// triggered once serial hardware receives a character
#if defined(__AVR_ATmega2560__)
ISR( USART0_RX_vect ) // called whenever USART receiver gets a character
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( _VECTOR(18) ) // called whenever USART receiver gets a character
#endif // defined(__AVR_ATmega328P__)
{

	uint8_t errFlags;
	uint8_t chr;
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	errFlags = (UCSR0A & ((1 << FE0) | (1 << DOR0) | (1 << UPE0)));
	devSerial0.controlFlags |= errFlags;

	chr = UDR0; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial0InputBuffer, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useSerial0PortInput)
#if defined(useBufferedSerial0Port)
// triggered once serial hardware is ready to receive another character for transmission
#if defined(__AVR_ATmega2560__)
ISR( USART0_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega328P__)
{

#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // defined(useDebugCPUreading)
	if (ringBuffer::isBufferNotEmpty(serial0Buffer)) UDR0 = ringBuffer::pull(serial0Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR0B &= ~(1 << UDRIE0); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useBufferedSerial0Port)
#endif // defined(useSerial0Port)
#if defined(useSerial1Port)
static void serial1::init(void)
{

	uint16_t myubbr1;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#if defined(useBufferedSerial1Port)

	ringBuffer::init(serial1Buffer, serial1Data);
#endif // defined(useBufferedSerial1Port)
#if defined(useSerial1PortInput)
	ringBuffer::init(serial1InputBuffer, serial1InputData);

#endif // defined(useSerial1PortInput)
	// turn on USART1 transmitter
	PRR1 &= ~(1 << PRUSART1);

	// calculate initial baudrate setting
	myubbr1 = (F_CPU / 4 / myBaudRate1 - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR1A = (1 << U2X1);

	if (myubbr1 > 4095)
	{

		UCSR1A = 0;
		myubbr1 = (F_CPU / 8 / myBaudRate1 - 1) / 2;

	}

	// set USART1 baud rate
	UBRR1H = (uint8_t)(myubbr1 >> 8);
	UBRR1L = (uint8_t)(myubbr1);

	// set for 8 data bits, no parity, and 1 stop bit
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));

#if defined(useSerial1PortInput)
	UCSR1B |= ((1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); // enable USART1 receive complete interrupt, receiver, and transmitter
#else // defined(useSerial1PortInput)
	UCSR1B |= (1 << TXEN1); // enable USART1 transmitter
#endif // defined(useSerial1PortInput)

	devSerial1.chrOut = chrOut;
#if defined(useSerial1PortInput)
	devSerial1.chrIn = chrIn;
#endif // defined(useSerial1PortInput)
	devSerial1.controlFlags |= (odvFlagCRLF | odvFlagEnableOutput);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial1::shutdown(void)
{

	UCSR1B &= ~((1 << UDRIE1) | (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); // disable USART1 I/O and utilized interrupts
	PRR1 |= (1 << PRUSART1); // shut off USART1 for power reduction

}

static void serial1::chrOut(uint8_t chr)
{

#if defined(useBufferedSerial1Port)
	ringBuffer::push(serial1Buffer, chr);
	UCSR1B |= (1 << UDRIE1); // enable data register empty interrupt
#else // defined(useBufferedSerial1Port)
	while ((UCSR1A & (1 << UDRE1)) == 0) idleProcess(); // wait until USART1 data buffer is empty

	UCSR1A &= ~(1 << TXC1); // clear transmit complete flag
	UDR1 = chr; //send the data
#endif // defined(useBufferedSerial1Port)

}

#if defined(useSerial1PortInput)
static uint8_t serial1::chrIn(void)
{

	return ringBuffer::pullMain(serial1InputBuffer);

}

// triggered once serial hardware receives a character
ISR( USART1_RX_vect ) // called whenever USART receiver gets a character
{

	uint8_t errFlags;
	uint8_t chr;
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	errFlags = (UCSR1A & ((1 << FE1) | (1 << DOR1) | (1 << UPE1)));
	devSerial1.controlFlags |= errFlags;

	chr = UDR1; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial1InputBuffer, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useSerial1PortInput)
#if defined(useBufferedSerial1Port)
// triggered once serial hardware is ready to receive another character for transmission
#if defined(__AVR_ATmega32U4__)
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( USART1_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega2560__)
{

#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // defined(useDebugCPUreading)
	if (ringBuffer::isBufferNotEmpty(serial1Buffer)) UDR1 = ringBuffer::pull(serial1Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR1B &= ~(1 << UDRIE1); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useBufferedSerial1Port)
#endif // defined(useSerial1Port)
#if defined(useSerial2Port)
static void serial2::init(void)
{

	uint16_t myubbr2;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#if defined(useBufferedSerial2Port)

	ringBuffer::init(serial2Buffer, serial2Data);
#endif // defined(useBufferedSerial2Port)
#if defined(useSerial2PortInput)
	ringBuffer::init(serial2InputBuffer, serial2InputData);

#endif // defined(useSerial2PortInput)
	// turn on USART2 transmitter
	PRR1 &= ~(1 << PRUSART2);

	// calculate initial baudrate setting
	myubbr2 = (F_CPU / 4 / myBaudRate2 - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR2A = (1 << U2X2);

	if (myubbr2 > 4095)
	{

		UCSR2A = 0;
		myubbr2 = (F_CPU / 8 / myBaudRate2 - 1) / 2;

	}

	// set USART2 baud rate
	UBRR2H = (uint8_t)(myubbr2 >> 8);
	UBRR2L = (uint8_t)(myubbr2);

	// set for 8 data bits, no parity, and 1 stop bit
	UCSR2C = ((1 << UCSZ21) | (1 << UCSZ20));

#if defined(useSerial2PortInput)
	UCSR2B |= ((1 << RXCIE2) | (1 << RXEN2) | (1 << TXEN2)); // enable USART2 receive complete interrupt, receiver, and transmitter
#else // defined(useSerial2PortInput)
	UCSR2B |= (1 << TXEN2); // enable USART2 transmitter
#endif // defined(useSerial2PortInput)

	devSerial2.chrOut = chrOut;
#if defined(useSerial2PortInput)
	devSerial2.chrIn = chrIn;
#endif // defined(useSerial2PortInput)
	devSerial2.controlFlags |= (odvFlagCRLF | odvFlagEnableOutput);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial2::shutdown(void)
{

	UCSR2B &= ~((1 << UDRIE2) | (1 << RXCIE2) | (1 << RXEN2) | (1 << TXEN2)); // disable USART2 I/O and utilized interrupts
	PRR1 |= (1 << PRUSART2); // shut off USART2 for power reduction

}

static void serial2::chrOut(uint8_t chr)
{

#if defined(useBufferedSerial2Port)
	ringBuffer::push(serial2Buffer, chr);
	UCSR2B |= (1 << UDRIE2); // enable data register empty interrupt
#else // defined(useBufferedSerial2Port)
	while ((UCSR2A & (1 << UDRE2)) == 0) idleProcess(); // wait until USART2 data buffer is empty

	UCSR2A &= ~(1 << TXC2); // clear transmit complete flag
	UDR2 = chr; //send the data
#endif // defined(useBufferedSerial2Port)

}

#if defined(useSerial2PortInput)
static uint8_t serial2::chrIn(void)
{

	return ringBuffer::pullMain(serial2InputBuffer);

}

// triggered once serial hardware receives a character
ISR( USART2_RX_vect ) // called whenever USART receiver gets a character
{

	uint8_t errFlags;
	uint8_t chr;
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	errFlags = (UCSR2A & ((1 << FE2) | (1 << DOR2) | (1 << UPE2)));
	devSerial2.controlFlags |= errFlags;

	chr = UDR2; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial2InputBuffer, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useSerial2PortInput)
#if defined(useBufferedSerial2Port)
// triggered once serial hardware is ready to receive another character for transmission
ISR( USART2_UDRE_vect ) // called whenever USART data buffer empties
{

#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // defined(useDebugCPUreading)
	if (ringBuffer::isBufferNotEmpty(serial2Buffer)) UDR2 = ringBuffer::pull(serial2Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR2B &= ~(1 << UDRIE2); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useBufferedSerial2Port)
#endif // defined(useSerial2Port)
#if defined(useSerial3Port)
static void serial3::init(void)
{

	uint16_t myubbr3;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#if defined(useBufferedSerial3Port)

	ringBuffer::init(serial3Buffer, serial3Data);
#endif // defined(useBufferedSerial3Port)
#if defined(useSerial3PortInput)
	ringBuffer::init(serial3InputBuffer, serial3InputData);

#endif // defined(useSerial3PortInput)
	// turn on USART3 transmitter
	PRR1 &= ~(1 << PRUSART3);

	// calculate initial baudrate setting
	myubbr3 = (F_CPU / 4 / myBaudRate3 - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR3A = (1 << U2X3);

	if (myubbr3 > 4095)
	{

		UCSR3A = 0;
		myubbr3 = (F_CPU / 8 / myBaudRate3 - 1) / 2;

	}

	// set USART3 baud rate
	UBRR3H = (uint8_t)(myubbr3 >> 8);
	UBRR3L = (uint8_t)(myubbr3);

	// set for 8 data bits, no parity, and 1 stop bit
	UCSR3C = ((1 << UCSZ31) | (1 << UCSZ30));

#if defined(useSerial3PortInput)
	UCSR3B |= ((1 << RXCIE3) | (1 << RXEN3) | (1 << TXEN3)); // enable USART3 receive complete interrupt, receiver, and transmitter
#else // defined(useSerial3PortInput)
	UCSR3B |= (1 << TXEN3); // enable USART3 transmitter
#endif // defined(useSerial3PortInput)

	devSerial3.chrOut = chrOut;
#if defined(useSerial3PortInput)
	devSerial3.chrIn = chrIn;
#endif // defined(useSerial3PortInput)
	devSerial3.controlFlags |= (odvFlagCRLF | odvFlagEnableOutput);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial3::shutdown(void)
{

	UCSR3B &= ~((1 << UDRIE3) | (1 << RXCIE3) | (1 << RXEN3) | (1 << TXEN3)); // disable USART3 I/O and utilized interrupts
	PRR1 |= (1 << PRUSART3); // shut off USART3 for power reduction

}

static void serial3::chrOut(uint8_t chr)
{

#if defined(useBufferedSerial3Port)
	ringBuffer::push(serial3Buffer, chr);
	UCSR3B |= (1 << UDRIE3); // enable data register empty interrupt
#else // defined(useBufferedSerial3Port)
	while ((UCSR3A & (1 << UDRE3)) == 0) idleProcess(); // wait until USART3 data buffer is empty

	UCSR3A &= ~(1 << TXC3); // clear transmit complete flag
	UDR3 = chr; //send the data
#endif // defined(useBufferedSerial3Port)

}

#if defined(useSerial3PortInput)
static uint8_t serial3::chrIn(void)
{

	return ringBuffer::pullMain(serial3InputBuffer);

}

// triggered once serial hardware receives a character
ISR( USART3_RX_vect ) // called whenever USART receiver gets a character
{

	uint8_t errFlags;
	uint8_t chr;
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	errFlags = (UCSR3A & ((1 << FE3) | (1 << DOR3) | (1 << UPE3)));
	devSerial3.controlFlags |= errFlags;

	chr = UDR3; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial3InputBuffer, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useSerial3PortInput)
#if defined(useBufferedSerial3Port)
// triggered once serial hardware is ready to receive another character for transmission
ISR( USART3_UDRE_vect ) // called whenever USART data buffer empties
{

#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // defined(useDebugCPUreading)
	if (ringBuffer::isBufferNotEmpty(serial3Buffer)) UDR3 = ringBuffer::pull(serial3Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR3B &= ~(1 << UDRIE3); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useBufferedSerial3Port)
#endif // defined(useSerial3Port)
