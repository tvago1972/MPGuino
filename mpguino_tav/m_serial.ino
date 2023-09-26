#if defined(useSerial0Port)
static void serial0::init(void)
{

	uint16_t myubbr0;
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	// turn on USART0 transmitter
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRUSART0);
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRUSART0);
#endif // defined(__AVR_ATmega328P__)

	// calculate initial baudrate setting
	myubbr0 = (F_CPU / 4 / serial0BaudRate - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR0A = (1 << U2X0);

	if (myubbr0 > 4095)
	{

		UCSR0A = 0;
		myubbr0 = (F_CPU / 8 / serial0BaudRate - 1) / 2;

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

#if defined(useSerial0PortInput)
	text::initDev(m8DevSerial0idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut, chrIn);
#else // defined(useSerial0PortInput)
	text::initDev(m8DevSerial0idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut);
#endif // defined(useSerial0PortInput)

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
	ringBuffer::pushMain(rbIdxSerial0Out, chr);
	UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
#else // defined(useBufferedSerial0Port)
	while ((UCSR0A & (1 << UDRE0)) == 0) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

	UCSR0A &= ~(1 << TXC0); // clear transmit complete flag
	UDR0 = chr; //send the data
#endif // defined(useBufferedSerial0Port)

}

#if defined(useSerial0PortInput)
static uint8_t serial0::chrIn(void)
{

	return ringBuffer::pullMain(rbIdxSerial0In);

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
	volatile8Variables[(uint16_t)(v8Serial0StatusIdx - v8VariableStartIdx)] |= errFlags;

	chr = UDR0; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::push(rbIdxSerial0In, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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
	if (ringBuffer::isBufferNotEmpty(rbIdxSerial0Out)) UDR0 = ringBuffer::pull(rbIdxSerial0Out); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR0B &= ~(1 << UDRIE0); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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

	// turn on USART1 transmitter
	PRR1 &= ~(1 << PRUSART1);

	// calculate initial baudrate setting
	myubbr1 = (F_CPU / 4 / serial1BaudRate - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR1A = (1 << U2X1);

	if (myubbr1 > 4095)
	{

		UCSR1A = 0;
		myubbr1 = (F_CPU / 8 / serial1BaudRate - 1) / 2;

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

#if defined(useSerial1PortInput)
	text::initDev(m8DevSerial1idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut, chrIn);
#else // defined(useSerial1PortInput)
	text::initDev(m8DevSerial1idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut);
#endif // defined(useSerial1PortInput)

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
	ringBuffer::pushMain(rbIdxSerial1Out, chr);
	UCSR1B |= (1 << UDRIE1); // enable data register empty interrupt
#else // defined(useBufferedSerial1Port)
	while ((UCSR1A & (1 << UDRE1)) == 0) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

	UCSR1A &= ~(1 << TXC1); // clear transmit complete flag
	UDR1 = chr; //send the data
#endif // defined(useBufferedSerial1Port)

}

#if defined(useSerial1PortInput)
static uint8_t serial1::chrIn(void)
{

	return ringBuffer::pullMain(rbIdxSerial1In);

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
	volatile8Variables[(uint16_t)(v8Serial1StatusIdx - v8VariableStartIdx)] |= errFlags;

	chr = UDR1; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::push(rbIdxSerial1In, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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
	if (ringBuffer::isBufferNotEmpty(rbIdxSerial1Out)) UDR1 = ringBuffer::pull(rbIdxSerial1Out); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR1B &= ~(1 << UDRIE1); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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

	// turn on USART2 transmitter
	PRR1 &= ~(1 << PRUSART2);

	// calculate initial baudrate setting
	myubbr2 = (F_CPU / 4 / serial2BaudRate - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR2A = (1 << U2X2);

	if (myubbr2 > 4095)
	{

		UCSR2A = 0;
		myubbr2 = (F_CPU / 8 / serial2BaudRate - 1) / 2;

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

#if defined(useSerial2PortInput)
	text::initDev(m8DevSerial2idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut, chrIn);
#else // defined(useSerial2PortInput)
	text::initDev(m8DevSerial2idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut);
#endif // defined(useSerial2PortInput)

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
	ringBuffer::pushMain(rbIdxSerial2Out, chr);
	UCSR2B |= (1 << UDRIE2); // enable data register empty interrupt
#else // defined(useBufferedSerial2Port)
	while ((UCSR2A & (1 << UDRE2)) == 0) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

	UCSR2A &= ~(1 << TXC2); // clear transmit complete flag
	UDR2 = chr; //send the data
#endif // defined(useBufferedSerial2Port)

}

#if defined(useSerial2PortInput)
static uint8_t serial2::chrIn(void)
{

	return ringBuffer::pullMain(rbIdxSerial2In);

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
	volatile8Variables[(uint16_t)(v8Serial2StatusIdx - v8VariableStartIdx)] |= errFlags;

	chr = UDR2; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::push(rbIdxSerial2In, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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
	if (ringBuffer::isBufferNotEmpty(rbIdxSerial2Out)) UDR2 = ringBuffer::pull(rbIdxSerial2Out); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR2B &= ~(1 << UDRIE2); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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

	// turn on USART3 transmitter
	PRR1 &= ~(1 << PRUSART3);

	// calculate initial baudrate setting
	myubbr3 = (F_CPU / 4 / serial3BaudRate - 1) / 2;

	// clear all clearable flags, disable multiprocessor comms, set double speed
	UCSR3A = (1 << U2X3);

	if (myubbr3 > 4095)
	{

		UCSR3A = 0;
		myubbr3 = (F_CPU / 8 / serial3BaudRate - 1) / 2;

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

#if defined(useSerial3PortInput)
	text::initDev(m8DevSerial3idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut, chrIn);
#else defined(useSerial3PortInput)
	text::initDev(m8DevSerial3idx, (odvFlagCRLF | odvFlagEnableOutput), chrOut);
#endif // defined(useSerial3PortInput)

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
	ringBuffer::pushMain(rbIdxSerial3Out, chr);
	UCSR3B |= (1 << UDRIE3); // enable data register empty interrupt
#else // defined(useBufferedSerial3Port)
	while ((UCSR3A & (1 << UDRE3)) == 0) heart::performSleepMode(SLEEP_MODE_IDLE); // go perform idle sleep mode

	UCSR3A &= ~(1 << TXC3); // clear transmit complete flag
	UDR3 = chr; //send the data
#endif // defined(useBufferedSerial3Port)

}

#if defined(useSerial3PortInput)
static uint8_t serial3::chrIn(void)
{

	return ringBuffer::pullMain(rbIdxSerial3In);

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
	volatile8Variables[(uint16_t)(v8Serial3StatusIdx - v8VariableStartIdx)] |= errFlags;

	chr = UDR3; // clear receive buffer
	if ((errFlags) == 0) ringBuffer::push(rbIdxSerial3In, chr);

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

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
	if (ringBuffer::isBufferNotEmpty(rbIdxSerial3Out)) UDR3 = ringBuffer::pull(rbIdxSerial3Out); // if buffer is not empty, send a buffered character to the serial hardware
	else UCSR3B &= ~(1 << UDRIE3); // otherwise, disable data register empty interrupt

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

#endif // defined(useDebugCPUreading)
}

#endif // defined(useBufferedSerial3Port)
#endif // defined(useSerial3Port)
