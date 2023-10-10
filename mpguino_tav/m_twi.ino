#if defined(useTWIsupport)
/* Two-Wire Interface support */

ISR( TWI_vect )
{

	uint8_t twiStatus;
#if defined(useDebugCPUreading)
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // defined(useDebugCPUreading)

	twiStatus = TW_STATUS;

	switch (twiStatus)
	{

		case TW_START:     // sent start condition
		case TW_REP_START: // sent repeated start condition
			TWDR = twiSlaveAddress; // copy device address and r/w bit to output register
			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA)); // send ACK
			break;

		case TW_MT_SLA_ACK:  // slave receiver ACKed address
		case TW_MT_DATA_ACK: // slave receiver ACKed data
			if(twiDataBufferIdx < twiDataBufferLen) // if there is data to send, send it
			{

				TWDR = twiDataBuffer[(uint16_t)(twiDataBufferIdx++)]; // copy data to output register
				TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA)); // send ACK

			}
			else volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiFinished);

			break;

		case TW_MR_DATA_NACK: // data received, nack sent
			twiDataBuffer[(uint16_t)(twiDataBufferIdx++)] = TWDR; // put final byte into buffer
			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiFinished);
			break;

		case TW_MR_DATA_ACK: // data received, ACK sent
			twiDataBuffer[(uint16_t)(twiDataBufferIdx++)] = TWDR; // put byte into buffer
		case TW_MR_SLA_ACK:  // address sent, ACK received
			if(twiDataBufferIdx < twiDataBufferLen) TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA)); // send ACK if more bytes are expected
			else TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE)); // otherwise, send NEGATIVE ACK
			break;

		case TW_MR_SLA_NACK: // address sent, NACK received
			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiFinished | twiClose);
			break;

		case TW_MT_ARB_LOST: // lost bus arbitration (also handles TW_MR_ARB_LOST)
			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA)); // send ACK
			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiInUse); // set TWI state to ready
			volatile8Variables[(uint16_t)(v8TWIerrorIdx - v8VariableStartIdx)] = TW_MT_ARB_LOST;
			break;

		case TW_MT_SLA_NACK:  // address sent, NACK received
		case TW_MT_DATA_NACK: // data sent, NACK received
		case TW_BUS_ERROR: // bus error, illegal stop/start
			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiErrorFlag | twiFinished | twiClose); // set error condition
			volatile8Variables[(uint16_t)(v8TWIerrorIdx - v8VariableStartIdx)] = twiStatus;
			break;

		case TW_NO_INFO: // no state information
		default:
			break;

	}

	if (volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] & twiFinished)
	{

		volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiOpen | twiFinished); // set TWI state to ready

		if (volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] & twiClose)
		{

			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWSTO)); // send STOP condition
			while (TWCR & (1 << TWSTO)); // wait for STOP condition to be executed on the TWI bus

			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiRemainOpen);

		}
		else
		{

			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiRemainOpen);	// keep the TWI channel open
			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTA));

		}

	}

#if defined(useDebugCPUreading)
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatile32Variables[(uint16_t)(v32WorkingInterruptProcessIdx - v32VariableStartIdx)] += c;

#endif // defined(useDebugCPUreading)
}

static void TWI::init(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

#if defined(__AVR_ATmega32U4__)
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PRR0 &= ~(1 << PRTWI); // turn on TWI module
	PORTD |= ((1 << PORTD1) | (1 << PORTD0)); // enable port D TWI pin pullup resistors
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PRR &= ~(1 << PRTWI); // turn on TWI module
	PORTC |= ((1 << PORTC5) | (1 << PORTC4)); // enable port C TWI pin pullup resistors
#endif // defined(__AVR_ATmega328P__)
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // set TWI prescaler to 1
	TWBR = (uint8_t)(((uint16_t)(F_CPU / (twiFrequency * 1000UL)) - 16) / 2); // set TWI frequency
	TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA)); // enable TWI module, acks, and interrupt

	volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] = (twiClose); // initialize TWI state to ready, default send stop, and no repeated start in progress

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::shutdown(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	// while there's a TWI transaction in progress, go perform idle sleep mode
	while (volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] & twiInUse) heart::performSleepMode(SLEEP_MODE_IDLE);

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	TWCR &= ~((1 << TWEN) | (1 << TWIE)); // disable TWI module and interrupt
#if defined(__AVR_ATmega32U4__)
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
	PORTD &= ~((1 << PORTD1) | (1 << PORTD0)); // disable port D TWI pin pullup resistors
	PRR0 |= (1 << PRTWI); // turn off TWI module
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	PORTC &= ~((1 << PORTC5) | (1 << PORTC4)); // disable port C TWI pin pullup resistors
	PRR |= (1 << PRTWI); // turn off TWI module
#endif // defined(__AVR_ATmega328P__)

	volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] = 0;

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::open(uint8_t address, uint8_t writeFlag) // this is in interrupt context
{

	twiSlaveAddress = (address << 1); // initial slave address
	twiSlaveAddress |= (writeFlag); // slap on direction

	volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiOpen); // signal twi is in use
	volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiErrorFlag); // clear error condition
	volatile8Variables[(uint16_t)(v8TWIerrorIdx - v8VariableStartIdx)] = 0x00; // reset error state

	twiDataBufferLen = 0; // initialize buffer length
	twiDataBufferIdx = 0; // initialize buffer index

}

static uint8_t TWI::writeByte(uint8_t data) // this is in interrupt context
{

	if(twiDataBufferLen < twiDataBufferSize)
	{

		twiDataBuffer[(uint16_t)(twiDataBufferLen++)] = data;
		return 0;

	}
	else
	{

		volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiInUse); // free up TWI for main program use
		return 1; // signal buffer overflow

	}

}

static void TWI::transmit(uint8_t sendStop) // this is in interrupt context
{

	if ((twiSlaveAddress & TW_WRITE) && (twiDataBufferLen == 0))
	{

		// if attempting to transmit an empty buffer, just cancel
		volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiInUse);

	}
	else
	{

		if (sendStop) volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] |= (twiClose); // if stop was requested as end of transmission, remember it
		else volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiClose); // otherwise, remember to do a repeated start

		// if we're in the repeated start state, then we've already sent the start,
		// and the TWI state machine is just waiting for the address byte.
		// We need to remove ourselves from the repeated start state before we enable interrupts,
		// since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
		// up. Also, don't enable the START interrupt. There may be one pending from the
		// repeated start that we sent outselves, and that would really confuse things.

		if (volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] & twiRemainOpen) // if in repeated start state
		{

			volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] &= ~(twiRemainOpen); // clear repeated start state
			TWDR = twiSlaveAddress; // set data address to slave address
			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));  // send ACK

		}
		else TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWSTA)); // enable INT and send start condition

	}

}

/* Two-Wire Interface main program facing support */

static void TWImain::open(uint8_t address, uint8_t writeFlag) // this is in main program context
{

	uint8_t oldSREG;
	uint8_t waitFlag;

	do
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operations atomic

		waitFlag = (volatile8Variables[(uint16_t)(v8TWIstatusIdx - v8VariableStartIdx)] & (twiInUse | twiInterruptInUse)); // is a TWI transaction already in progress?

		if (waitFlag == 0) TWI::open(address, writeFlag); // if not, open TWI hardware for main program

		SREG = oldSREG; // restore interrupt flag status

		if (waitFlag) heart::performSleepMode(SLEEP_MODE_IDLE); // if a TWI transaction was already in progress, wait for timer0 to complete request

	}
	while (waitFlag); // loop while the already-in-progress TWI transaction completes

}

static uint8_t TWImain::writeByte(uint8_t data) // this is in main program context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	TWI::writeByte(data);

	SREG = oldSREG; // restore interrupt flag status

}

static void TWImain::transmit(uint8_t sendStop) // this is in main program context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	TWI::transmit(sendStop);

	SREG = oldSREG; // restore interrupt flag status

}

#endif // defined(useTWIsupport)
#if defined(useMCP23017portExpander)
// initialize MCP23017 port expander to a known initial state
void MCP23017portExpanderSupport::init(void)
{

	uint16_t MCP23017registers;

	mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)] = 0; // initially set all control register bits to 0

	// we might have inadvertently entered into MCP23017 bank mode - if we didn't, we'll just end up disabling all of bank B's interrupt enable bits
	writeRegister8Bit(MCP23017_B1_IOCON, mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)]); // write initialization value to IO control register

	// if we did happen to enter bank mode prior to initialization, we are now known to not be in bank mode
	writeRegister8Bit(MCP23017_B0_IOCON, mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)]); // write initialization value to IO control register

	setTransferMode(adaTWItoggleMode);

	MCP23017registers = 0;

	writeRegister16Bit(MCP23017_B0_GPINTENx, MCP23017registers); // write out interrupt enable register (the Adafruit RGB LCD shield wasn't wired to support this)
	writeRegister16Bit(MCP23017_B0_GPINTENx, MCP23017registers); // write out GPIO bit interrupt assignment register (no GPIO bits associated with interrupts)
	writeRegister16Bit(MCP23017_B0_DEFVALx, MCP23017registers); // write out interrupt default value register (all default bits 0)
	writeRegister16Bit(MCP23017_B0_INTCONx, MCP23017registers); // write out interrupt control register (all interrupts trigger on GPIO input pin change)
	writeRegister16Bit(MCP23017_B0_GPIOx, MCP23017registers); // write out GPIO state (all 0) - also writes out OLAT register

	configureOutputPort(MCP23017registers); // finish up by initializing pin outputs and going to address byte mode

}

void MCP23017portExpanderSupport::configureOutputPort(uint16_t registerValue)
{

	writeRegister16Bit(MCP23017_B0_IODIRx, registerValue); // write out port direction
	writeRegister16Bit(MCP23017_B0_GPPUx, registerValue); // write out pull-up resistor config
	writeRegister16Bit(MCP23017_B0_IPOLx, registerValue); // write out input pin polarity config

	setTransferMode(adaTWIbyteMode); // ensure address mode is in byte mode

}

void MCP23017portExpanderSupport::writeRegister16Bit(uint8_t registerAddress, uint16_t registerValue)
{

	union union_16 * rV = (union union_16 *)(&registerValue);

	writeRegister16Bit(registerAddress, rV->u8[0], rV->u8[1]);

}

void MCP23017portExpanderSupport::writeRegister16Bit(uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte)
{

	if (mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)] & afRGBLCDbankMode) setTransferMode(adaTWItoggleMode); // if address mode isn't set to access 16-bit registers, configure as such

	TWImain::open(TWIaddressMCP23017, TW_WRITE); // open TWI as master transmitter
	TWImain::writeByte(registerAddress | MCP23017_B0_PORTA); // specify bank A register address
	TWImain::writeByte(portAbyte); // write desired value to register bank A byte
	TWImain::writeByte(portBbyte); // write desired value to register bank B byte
	TWImain::transmit(TWI_STOP); // go write out register contents

}

void MCP23017portExpanderSupport::writeRegister8Bit(uint8_t registerAddress, uint8_t portByte)
{

	TWImain::open(TWIaddressMCP23017, TW_WRITE); // open TWI as master transmitter
	TWImain::writeByte(registerAddress); // specify register address
	TWImain::writeByte(portByte); // write desired value to register address
	TWImain::transmit(TWI_STOP); // go write out register contents

}

void MCP23017portExpanderSupport::setTransferMode(uint8_t mode)
{

	uint8_t address;
	uint8_t newFlags = mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)];

	switch (mode)
	{

		case adaTWIsequentialMode:
			newFlags &= ~(afRGBLCDbankMode | afRGBLCDbyteMode); // set registers to be sequential A-B byte pairs from 0x00-0x15, enable sequential address update
			break;

		case adaTWItoggleMode: // this weird mode causes successive register accesses to flip between byte A and byte B on the shield
			newFlags &= ~(afRGBLCDbankMode); // set registers to be sequential A-B byte pairs from 0x00-0x15
			newFlags |= (afRGBLCDbyteMode); // disable sequential address update
			break;

		case adaTWIbyteMode:
			newFlags |= (afRGBLCDbankMode | afRGBLCDbyteMode); // set registers to be separated into separate A and B bytes, disable sequential address update
			break;

		default:
			break;

	}

	if (mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)] != newFlags) // if a change was detected
	{

		if (mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)] & afRGBLCDbankMode) address = MCP23017_B1_IOCON;
		else address = MCP23017_B0_IOCON;

		writeRegister8Bit(address, newFlags); // write new value to IO control register

		mainProgram8Variables[(uint16_t)(m8MCP23017statusFlags - m8VariableStartIdx)] = newFlags; // changes take effect just as soon as IOCON is written

	}

}

#endif // defined(useMCP23017portExpander)
