/* button input support section */

static void button::init(void)
{

#ifdef useAdafruitRGBLCDshield
	uint16_t MCP23017registers;
#endif // useAdafruitRGBLCDshield
	uint8_t oldSREG;

#ifdef useAdafruitRGBLCDshield
#if useTWIbuttons && useTWILCD
	LCD::disableIntSample(); // disable TWI button sampling

#endif // useTWIbuttons && useTWILCD
	MCP23017registers = (uint16_t)(buttonMask);

	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_IODIRx, (union_16 *)(&MCP23017registers)); // write out port direction (only buttons as input)
	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_GPPUx, (union_16 *)(&MCP23017registers)); // write out pull-up resistor config (only buttons as input)
	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_IPOLx, (union_16 *)(&MCP23017registers)); // write out input pin polarity config (only buttons as input)

	adafruitRGBLCDsupport::setTransferMode(adaTWIbyteMode); // ensure address mode is in byte mode

#if useTWIbuttons && useTWILCD
	LCD::enableIntSample(); // enable TWI button sampling

#endif // useTWIbuttons && useTWILCD
#endif // useAdafruitRGBLCDshield
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#ifdef useLegacyButtons
#if defined(__AVR_ATmega2560__)
	DIDR2 &= ~((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // enable digital input on port K button pins

	PORTK |= ((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // enable port K button pullup resistors

	PCMSK2 |= ((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // enable port K button interrupts

	PCICR |= (1 << PCIE2); // enable selected interrupts on port K

	lastPINxState = PINK; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DIDR0 &= ~((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // enable digital input on port C button pins

	PORTC |= ((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // enable port C button pullup resistors

	PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // enable port C button interrupts

	PCICR |= (1 << PCIE1); // enable selected interrupts on port C

	lastPINxState = PINC; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega328P__)
#endif // useLegacyButtons
	SREG = oldSREG; // restore interrupt flag status

}

static void button::shutdown(void)
{
}

#if defined(useAnalogButtons) || defined(useDebugTerminal)
static void button::inject(uint8_t buttonValue)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	thisButtonState = buttonValue;
	timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

	SREG = oldSREG; // restore interrupt flag status

}

#endif // useAnalogButtons || useDebugTerminal
