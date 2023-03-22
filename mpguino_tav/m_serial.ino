#ifdef useSerial0Port
static void serial0::init(void)
{

	uint16_t myubbr0;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useBufferedSerial0Port
	ringBuffer::init(serial0Buffer, serial0Data);

#endif // useBufferedSerial0Port
#ifdef useSerial0PortInput
	ringBuffer::init(serial0InputBuffer, serial0InputData);

#endif // useSerial0PortInput
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

#ifdef useSerial0PortInput
	UCSR0B |= ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0)); // enable USART0 receive complete interrupt, receiver, and transmitter
#else // useSerial0PortInput
	UCSR0B |= (1 << TXEN0); // enable USART0 transmitter
#endif // useSerial0PortInput

	devSerial0.chrOut = chrOut;
#ifdef useSerial0PortInput
	devSerial0.chrIn = chrIn;
#endif // useSerial0PortInput
	devSerial0.controlFlags |= (odvFlagCRLF);

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

#ifdef useBufferedSerial0Port
	ringBuffer::push(serial0Buffer, chr);
	UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
#else // useBufferedSerial0Port
	while ((UCSR0A & (1 << UDRE0)) == 0) idleProcess(); // wait until USART0 data buffer is empty

	UCSR0A &= ~(1 << TXC0); // clear transmit complete flag
	UDR0 = chr; //send the data
#endif // useBufferedSerial0Port

}

#ifdef useSerial0PortInput
static uint8_t serial0::chrIn(void)
{

	return ringBuffer::pull(serial0InputBuffer);

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
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	errFlags = (UCSR0A & ((1 << FE0) | (1 << DOR0) | (1 << UPE0)));

	chr = UDR0; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial0InputBuffer, chr);

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useSerial0PortInput
#ifdef useBufferedSerial0Port
// triggered once serial hardware is ready to receive another character for transmission
#if defined(__AVR_ATmega2560__)
ISR( USART0_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega328P__)
{

#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // useDebugCPUreading
	if (ringBuffer::isBufferNotEmpty(serial0Buffer)) UDR0 = ringBuffer::pull(serial0Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR0B &= ~(1 << UDRIE0); // otherwise, disable data register empty interrupt

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useBufferedSerial0Port
#endif // useSerial0Port
#ifdef useSerial1Port
static void serial1::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#ifdef useBufferedSerial1Port

	ringBuffer::init(serial1Buffer, serial1Data);
#endif // useBufferedSerial1Port
#ifdef useSerial1PortInput
	ringBuffer::init(serial1InputBuffer, serial1InputData);

#endif // useSerial1PortInput
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

#ifdef useSerial0PortInput
	UCSR1B |= ((1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); // enable USART1 receive complete interrupt, receiver, and transmitter
#else // useSerial0PortInput
	UCSR1B |= (1 << TXEN1); // enable USART1 transmitter
#endif // useSerial0PortInput

	devSerial1.chrOut = chrOut;
#ifdef useSerial1PortInput
	devSerial1.chrIn = chrIn;
#endif // useSerial1PortInput
	devSerial1.controlFlags |= (odvFlagCRLF);

	SREG = oldSREG; // restore interrupt flag status

}

static void serial1::shutdown(void)
{

	UCSR1B &= ~((1 << UDRIE1) | (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); // disable USART0 I/O and utilized interrupts
	PRR1 |= (1 << PRUSART1); // shut off USART1 for power reduction

}

static void serial1::chrOut(uint8_t chr)
{

#ifdef useBufferedSerial1Port
	ringBuffer::push(serial1Buffer, chr);
	UCSR1B |= (1 << UDRIE1); // enable data register empty interrupt
#else // useBufferedSerial1Port
	while ((UCSR1A & (1 << UDRE1)) == 0) idleProcess(); // wait until USART1 data buffer is empty

	UCSR1A &= ~(1 << TXC1); // clear transmit complete flag
	UDR1 = chr; //send the data
#endif // useBufferedSerial1Port

}

#ifdef useSerial1PortInput
static uint8_t serial1::chrIn(void)
{

	return ringBuffer::pull(serial1InputBuffer);

}

// triggered once serial hardware receives a character
ISR( USART1_RX_vect ) // called whenever USART receiver gets a character
{

	uint8_t errFlags;
	uint8_t chr;
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	errFlags = (UCSR1A & ((1 << FE1) | (1 << DOR1) | (1 << UPE1)));

	chr = UDR1; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::pushInterrupt(serial1InputBuffer, chr);

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useSerial1PortInput
#ifdef useBufferedSerial1Port
// triggered once serial hardware is ready to receive another character for transmission
#if defined(__AVR_ATmega32U4__)
ISR( USART_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
ISR( USART1_UDRE_vect ) // called whenever USART data buffer empties
#endif // defined(__AVR_ATmega2560__)
{

#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

#endif // useDebugCPUreading
	if (ringBuffer::isBufferNotEmpty(serial1Buffer)) UDR1 = ringBuffer::pull(serial1Buffer); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR1B &= ~(1 << UDRIE1); // otherwise, disable data register empty interrupt

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

#endif // useBufferedSerial1Port
#endif // useSerial1Port
