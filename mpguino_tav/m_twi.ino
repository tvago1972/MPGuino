#ifdef useTWIsupport
ISR( TWI_vect )
{

	uint8_t twiStatus;
#ifdef useDebugCPUreading
	uint8_t a;
	uint8_t b;
	uint16_t c;

	a = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles
#endif // useDebugCPUreading

	twiStatus = TW_STATUS;

	switch (twiStatus)
	{

		case TW_START:     // sent start condition
		case TW_REP_START: // sent repeated start condition
			TWDR = twiSlaveAddress; // copy device address and r/w bit to output register
			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK
			break;

		case TW_MT_SLA_ACK:  // slave receiver ACKed address
		case TW_MT_DATA_ACK: // slave receiver ACKed data
			if(twiDataBufferIdx < twiDataBufferLen) // if there is data to send, send it
			{

				TWDR = twiDataBuffer[(uint16_t)(twiDataBufferIdx++)]; // copy data to output register
				TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK

			}
			else twiStatusFlags |= (twiFinished);

			break;

		case TW_MR_DATA_NACK: // data received, nack sent
			twiDataBuffer[(uint16_t)(twiDataBufferIdx++)] = TWDR; // put final byte into buffer
			twiStatusFlags |= (twiFinished);
			break;

		case TW_MR_DATA_ACK: // data received, ACK sent
			twiDataBuffer[(unsigned int)(twiDataBufferIdx++)] = TWDR; // put byte into buffer
		case TW_MR_SLA_ACK:  // address sent, ACK received
			if(twiDataBufferIdx < twiDataBufferLen) TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK if more bytes are expected
			else TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWIE)); // otherwise, send NEGATIVE ACK
			break;

		case TW_MR_SLA_NACK: // address sent, NACK received
			twiStatusFlags |= (twiFinished | twiClose);
			break;

		case TW_MT_ARB_LOST: // lost bus arbitration (also handles TW_MR_ARB_LOST)
			twiErrorCode = TW_MT_ARB_LOST;
			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // send ACK
			twiStatusFlags &= ~(twiOpen); // set TWI state to ready
			break;

		case TW_MT_SLA_NACK:  // address sent, NACK received
		case TW_MT_DATA_NACK: // data sent, NACK received
		case TW_BUS_ERROR: // bus error, illegal stop/start
			twiStatusFlags |= (twiErrorFlag | twiFinished | twiClose); // set error condition
			twiErrorCode = twiStatus;
			break;

		case TW_NO_INFO: // no state information
		default:
			break;

	}

	if (twiStatusFlags & twiFinished)
	{

		twiStatusFlags &= ~(twiOpen | twiFinished); // set TWI state to ready

		if (twiStatusFlags & twiClose)
		{

			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTO)); // send STOP condition
			while (TWCR & (1 << TWSTO)); // wait for STOP condition to be executed on the TWI bus

			twiStatusFlags &= ~(twiRemainOpen);

		}
		else
		{

			twiStatusFlags |= (twiRemainOpen);	// keep the TWI channel open
			TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTA));

		}

	}

#ifdef useDebugCPUreading
	b = TCNT0; // do a microSeconds() - like read to determine interrupt length in cycles

	if (b < a) c = 256 - a + b; // an overflow occurred
	else c = b - a;

	volatileVariables[(uint16_t)(vInterruptAccumulatorIdx)] += c;

#endif // useDebugCPUreading
}

static void TWI::init(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

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
	TWBR = (uint8_t)(((unsigned int)(F_CPU / (twiFrequency * 1000UL)) - 16) / 2); // set TWI frequency
	TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)); // enable TWI module, acks, and interrupt

	twiStatusFlags = twiClose; // initialize TWI state to ready, default send stop, and no repeated start in progress

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::shutdown(void) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	while (twiStatusFlags & twiOpenMain) idleProcess(); // wait for all TWI transactions to complete

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

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

	twiStatusFlags = 0;

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::openChannelMain(uint8_t address, uint8_t writeFlag) // this is in main program context
{

	uint8_t oldSREG;

	while (twiStatusFlags & twiOpenMain) idleProcess(); // wait for TWI to become available

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	openChannel(address, writeFlag);

	SREG = oldSREG; // restore interrupt flag status

}

static void TWI::openChannel(uint8_t address, uint8_t writeFlag) // this is in interrupt context
{

	twiSlaveAddress = (address << 1); // initial slave address
	twiSlaveAddress |= (writeFlag); // slap on direction

	twiStatusFlags |= (twiOpen); // signal twi is in use
	twiStatusFlags &= ~(twiErrorFlag); // clear error condition
	twiErrorCode = 0x00; // reset error state

	twiDataBufferLen = 0; // initialize buffer length
	twiDataBufferIdx = 0; // initialize buffer index

}

static uint8_t TWI::writeByte(uint8_t data) // this can be in either main program or interrupt context
{

	if(twiDataBufferLen < twiDataBufferSize)
	{

		twiDataBuffer[(unsigned int)(twiDataBufferLen++)] = data;
		return 0;

	}
	else
	{

		changeBitFlags(twiStatusFlags, twiOpen, 0); // free up TWI for main program use
		return 1; // signal buffer overflow

	}

}

static void TWI::transmitChannel(uint8_t sendStop) // this can be in either main program or interrupt context
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	if ((twiSlaveAddress & TW_WRITE) && (twiDataBufferLen == 0)) twiStatusFlags &= ~(twiOpen);
	else
	{

		if (sendStop) twiStatusFlags |= (twiClose); // if stop was requested as end of transmission, remember it
		else twiStatusFlags &= ~(twiClose); // otherwise, remember to do a repeated start

		// if we're in the repeated start state, then we've already sent the start,
		// and the TWI state machine is just waiting for the address byte.
		// We need to remove ourselves from the repeated start state before we enable interrupts,
		// since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
		// up. Also, don't enable the START interrupt. There may be one pending from the
		// repeated start that we sent outselves, and that would really confuse things.

		if (twiStatusFlags & twiRemainOpen) // if in repeated start state
		{

			twiStatusFlags &= ~(twiRemainOpen); // clear repeated start state
			TWDR = twiSlaveAddress; // set data address to slave address
			TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE));  // send ACK

		}
		else TWCR = ((1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWSTA)); // enable INT and send start condition

	}

	SREG = oldSREG; // restore interrupt flag status

}

#endif // useTWIsupport
#ifdef useAdafruitRGBLCDshield
// initialize Adafruit RGB LGB shield to a known initial state
void adafruitRGBLCDsupport::init(void)
{

	uint16_t MCP23017registers;

	adafruitRGBLCDflags = 0; // initially set all control register bits to 0

	// we might have inadvertently entered into MCP23017 bank mode - if we didn't, we'll just end up disabling all of bank B's interrupt enable bits
	writeRegister8Bit(MCP23017_B1_IOCON, adafruitRGBLCDflags); // write initialization value to IO control register

	// if we did happen to enter bank mode prior to initialization, we are now known to not be in bank mode
	writeRegister8Bit(MCP23017_B0_IOCON, adafruitRGBLCDflags); // write initialization value to IO control register

	setTransferMode(adaTWItoggleMode);

	MCP23017registers = 0;

	writeRegister16Bit(MCP23017_B0_GPINTENx, (union_16 *)(&MCP23017registers)); // write out interrupt enable register (the Adafruit RGB LCD shield wasn't wired to support this)
	writeRegister16Bit(MCP23017_B0_IODIRx, (union_16 *)(&MCP23017registers)); // write out port direction (no inputs)
	writeRegister16Bit(MCP23017_B0_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (input bits same as GPIO bits)
	writeRegister16Bit(MCP23017_B0_GPINTENx, (union_16 *)(&MCP23017registers)); // write out GPIO bit interrupt assignment register (no GPIO bits associated with interrupts)
	writeRegister16Bit(MCP23017_B0_DEFVALx, (union_16 *)(&MCP23017registers)); // write out interrupt default value register (all default bits 0)
	writeRegister16Bit(MCP23017_B0_INTCONx, (union_16 *)(&MCP23017registers)); // write out interrupt control register (all interrupts trigger on GPIO input pin change)
	writeRegister16Bit(MCP23017_B0_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (no pull-up resistors)
	writeRegister16Bit(MCP23017_B0_GPIOx, (union_16 *)(&MCP23017registers)); // write out GPIO state (all 0) - also writes out OLAT register

}

void adafruitRGBLCDsupport::writeRegister16Bit(uint8_t registerAddress, union union_16 * registerValue)
{

	writeRegister16Bit(registerAddress, registerValue->u8[0], registerValue->u8[1]);

}

void adafruitRGBLCDsupport::writeRegister16Bit(uint8_t registerAddress, uint8_t portAbyte, uint8_t portBbyte)
{

	if (adafruitRGBLCDflags & afRGBLCDbankMode) setTransferMode(adaTWItoggleMode); // if address mode isn't set to access 16-bit registers, configure as such

	TWI::openChannelMain(adafruitRGBLCDaddress, TW_WRITE); // open TWI as master transmitter
	TWI::writeByte(registerAddress | MCP23017_B0_PORTA); // specify bank A register address
	TWI::writeByte(portAbyte); // write desired value to register bank A byte
	TWI::writeByte(portBbyte); // write desired value to register bank B byte
	TWI::transmitChannel(TWI_STOP); // go write out register contents

}

void adafruitRGBLCDsupport::writeRegister8Bit(uint8_t registerAddress, uint8_t portByte)
{

	TWI::openChannelMain(adafruitRGBLCDaddress, TW_WRITE); // open TWI as master transmitter
	TWI::writeByte(registerAddress); // specify register address
	TWI::writeByte(portByte); // write desired value to register address
	TWI::transmitChannel(TWI_STOP); // go write out register contents

}

void adafruitRGBLCDsupport::setTransferMode(uint8_t mode)
{

	uint8_t address;
	uint8_t newFlags = adafruitRGBLCDflags;

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

	if (adafruitRGBLCDflags != newFlags) // if a change was detected
	{

		if (adafruitRGBLCDflags & afRGBLCDbankMode) address = MCP23017_B1_IOCON;
		else address = MCP23017_B0_IOCON;

		writeRegister8Bit(address, newFlags); // write new value to IO control register

		adafruitRGBLCDflags = newFlags; // changes take effect just as soon as IOCON is written

	}

}

#endif // useAdafruitRGBLCDshield
