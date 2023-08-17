#if defined(useLCDoutput)
/* LCD hardware support section */

static void LCD::init(void)
{

#if defined(useSerialLCD)
	devLCDserial.controlFlags &= ~(odvFlagCRLF);

	heart::wait0(delay0100msTick); // wait for 100 ms to allow serial LCD to initialize

#endif // defined(useSerialLCD)
#if defined(use4BitLCD)
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	devLCD.chrOut = LCD::writeData;
	devLCD.controlFlags |= (odvFlagEnableOutput);

#if defined(useLCDbufferedOutput)
	ringBuffer::init(lcdBuffer, LCDdata);

#endif // defined(useLCDbufferedOutput)
	lcdDelayCount = 0; // reset LCD delay count
	timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

	SREG = oldSREG; // restore interrupt flag status

#if defined(usePort4BitLCD)
#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
	// set OC1A to non-inverting mode for LCD contrast
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// set OC1B to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdContrast | lcdBrightness);
	DDRD |= (lcdBit2 | lcdBit1);
	DDRE |= lcdEnable;
	DDRF |= (lcdRegisterSelect | lcdBit0 | lcdDirection);
	PORTF &= ~lcdDirection; // write a zero to this pin - MPGuino has no need to read anything from the LCD module

#else // defined(useTinkerkitLCDmodule)
	// any port commands for any other ATmega32U4 board goes here

#endif // defined(useTinkerkitLCDmodule)
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRA |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);
	DDRB |= (lcdBrightness | lcdContrast);

#else // defined(useArduinoMega2560)
	// any port commands for any other ATmega2560 board goes here

#endif // defined(useArduinoMega2560)
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#if defined(useLegacyLCD)
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBrightness);
	DDRD |= (lcdBit0 | lcdContrast | lcdEnable | lcdRegisterSelect);

#endif // defined(useLegacyLCD)
#if defined(useDFR0009LCD)
	// set OC1B to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1B0);
	TCCR1A |= (1 << COM1B1);

	// enable LCD pins
	DDRD |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
	DDRB |= (lcdBrightness | lcdEnable | lcdRegisterSelect);

#endif // defined(useDFR0009LCD)
	// any port commands for any other ATmega168/328/328P board goes here

#endif // defined(__AVR_ATmega328P__)
#endif // defined(usePort4BitLCD)
#if defined(useTWI4BitLCD)
#if defined(useInterruptBasedTWI)
	TWI::disableISRactivity(); // disable ISR-based TWI activity

#endif // defined(useInterruptBasedTWI)
	portLCD = 0; // reset LCD port byte
#if defined(useAdafruitRGBLCDshield)
	portSwitches = 0; // reset button port byte (contains two of the three LCD backlight LED bits)

	MCP23017portExpanderSupport::setTransferMode(adaTWIbyteMode);
#endif // defined(useAdafruitRGBLCDshield)
#if defined(useInterruptBasedTWI)
	TWI::enableISRactivity(); // enable ISR-based TWI activity

#endif // defined(useInterruptBasedTWI)
#endif // defined(useTWI4BitLCD)

	// perform reset by instruction on LCD interface
	// the below 5 lines actually take up less space than using a PROGMEM table / loop to write these values
	writeNybble(lcdNullValue, lcdDelay0015ms); // wait for more than 15 msec
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay4100us); // send (B0011) to DB7-4, then wait for more than 4.1 ms
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(0b00100000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0010) to DB7-4 for 4 bit mode, then wait for more than 40 us

	// ready to use normal LCD output function now!
	writeCommand(lcdFunctionSet | lcdFSnumberOfLines); // 4-bit interface, 2 display lines, 5x8 font

#if defined(useLCDbufferedOutput)
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // defined(useLCDbufferedOutput)
#endif // defined(use4BitLCD)
#if defined(useLCDgraphics)
	for (uint8_t x = 0; x < 64; x++) CGRAMbuffer[(uint16_t)(x)] = cgramFlagDirty;

#endif // defined(useLCDgraphics)
#if defined(useLCDcontrast)
	setContrast(EEPROM::readByte(pContrastIdx));

#endif // defined(useLCDcontrast)
	setBrightness(EEPROM::readByte(pBrightnessIdx));
	writeData(0x16); // display control - turn on display, no cursor, no blink
	writeData(0x0C); // clear display, set cursor position to zero

#if defined(useLCDbufferedOutput)
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer to force the LCD screen to clear

#endif // defined(useLCDbufferedOutput)
#if defined(LCDserialBuffer)
	ringBuffer::flush(LCDserialBuffer); // clear the LCD buffer to force the LCD screen to clear

#endif // defined(LCDserialBuffer)
}

static void LCD::shutdown(void)
{

	setBrightness(0); // turn off LCD brightness
#if defined(useLCDcontrast)
	setContrast(255); // turn off LCD contrast
#endif // defined(useLCDcontrast)
	writeData(0x15); // display control - turn off display
#if defined(LCDserialBuffer)
	ringBuffer::flush(LCDserialBuffer); // flush LCD output buffer to force the LCD display to turn off
#endif // defined(LCDserialBuffer)
#if defined(useLCDbufferedOutput)
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer
#endif // defined(useLCDbufferedOutput)

#if defined(use4BitLCD)
#if defined(usePort4BitLCD)
#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
	// disable LCD pins
	DDRB &= ~(lcdBit3);
	DDRD &= ~(lcdBit2 | lcdBit1);
	DDRE &= ~(lcdEnable);
	DDRF &= ~(lcdRegisterSelect | lcdBit0 | lcdDirection);

	PORTB |= lcdContrast; // ensure LCD contrast is turned off
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

#endif // defined(useTinkerkitLCDmodule)
	// any port commands for any other ATmega32U4 board goes here

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
	// disable LCD pins
	DDRA &= ~(lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);

	PORTB |= lcdContrast; // ensure LCD contrast is turned off
#if defined(useInvertedLegacyLCDbrightness)
	PORTB |= lcdBrightness; // ensure LCD brightness is turned off
#else // defined(useInvertedLegacyLCDbrightness)
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#endif // defined(useInvertedLegacyLCDbrightness)

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

#endif // defined(useArduinoMega2560)
	// any port commands for any other ATmega2560 board goes here

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#if defined(useLegacyLCD)
	// disable LCD pins
	DDRB &= ~(lcdBit3 | lcdBit2 | lcdBit1);
	DDRD &= ~(lcdBit0 | lcdEnable | lcdRegisterSelect);

	PORTD |= lcdContrast;
#if defined(useInvertedLegacyLCDbrightness)
	PORTB |= lcdBrightness; // ensure LCD brightness is turned off
#else // defined(useInvertedLegacyLCDbrightness)
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#endif // defined(useInvertedLegacyLCDbrightness)

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

#endif // defined(useLegacyLCD)
#if defined(useDFR0009LCD)
	// disable LCD pins
	DDRD &= ~(lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
	DDRB &= ~(lcdBrightness | lcdEnable | lcdRegisterSelect);

	// set OC1B to disabled
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

#endif // defined(useDFR0009LCD)
	// any port commands for any other ATmega168/328/328P board goes here

#endif // defined(__AVR_ATmega328P__)
#endif // defined(usePort4BitLCD)
#endif // defined(use4BitLCD)
}

static void LCD::setBrightness(uint8_t idx)
{

#if defined(use4BitLCD)
#if defined(usePort4BitLCD)
#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
	OCR1B = pgm_read_byte(&brightness[(uint16_t)(idx)]);

#endif // defined(useTinkerkitLCDmodule)
	// any port commands for any other ATmega32U4 board goes here

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
	OCR1A = pgm_read_byte(&brightness[(uint16_t)(idx)]);

#endif // defined(useArduinoMega2560)
	// any port commands for any other ATmega2560 board goes here

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#if defined(useLegacyLCD)
	OCR1A = pgm_read_byte(&brightness[(uint16_t)(idx)]);

#endif // defined(useLegacyLCD)
#if defined(useDFR0009LCD)
	OCR1B = pgm_read_byte(&brightness[(uint16_t)(idx)]);

#endif // defined(useDFR0009LCD)
	// any port commands for any other ATmega168/328/328P board goes here

#endif // defined(__AVR_ATmega328P__)
#endif // defined(usePort4BitLCD)
#if defined(useTWI4BitLCD)
#if defined(useAdafruitRGBLCDshield)
	if (idx) idx = EEPROM::readByte(pLCDcolorIdx); // get LCD backlight color

	setRGBcolor(idx); // set LCD backlight color

#endif // defined(useAdafruitRGBLCDshield)
#if defined(useSainSmart2004LCD) || defined(useGenericTWILCD)
	if (idx) portLCD |= lcdBrightness; // turn on LCD backlight
	else portLCD &= ~(lcdBrightness); // turn off LCD backlight

#endif // defined(useSainSmart2004LCD) || defined(useGenericTWILCD)
#endif // defined(useTWI4BitLCD)
#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
	if (idx) idx = 0x11;
	else idx = 0x12;

	LCDserialPort::chrOut(idx);

#endif // defined(useSerialLCD)
}

#if defined(useLCDcontrast)
static void LCD::setContrast(uint8_t idx)
{

#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
	OCR1A = idx;

#endif // defined(useTinkerkitLCDmodule)
	// any port commands for any other ATmega32U4 board goes here

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
	OCR0A = idx;

#endif // defined(useArduinoMega2560)
	// any port commands for any other ATmega2560 board goes here

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#if defined(useLegacyLCD)
	OCR0A = idx;

#endif // defined(useLegacyLCD)
	// any port commands for any other ATmega168/328/328P board goes here

#endif // defined(__AVR_ATmega328P__)
}

#endif // defined(useLCDcontrast)
#if defined(useAdafruitRGBLCDshield)
static void LCD::setRGBcolor(uint8_t idx)
{

	uint8_t RGBbitMask;
	uint8_t byt;

	if (EEPROM::readByte(pBrightnessIdx) == 0) idx = 0;

	RGBbitMask = pgm_read_byte(&RGBcolors[(uint16_t)(idx & 0x07)]); // read the LCD backlight color bitmask pattern

	byt = RGBbitMask; // get the color bitmask pattern
	byt ^= portSwitches; // flip the color bitmask by what's stored in the portSwitches register
	byt &= (lcdBrightnessRed | lcdBrightnessGreen); // strip out all but the relevant bits
	portSwitches ^= byt; // flip again - restores non-relevant bits and causes relevant bits to change according to RGBbitMask

	byt = RGBbitMask; // get the color bitmask
	byt ^= portLCD; // flip the color bitmask by what's stored in the portLCD register
	byt &= (lcdBrightnessBlue); // strip out all but the relevant bit
	portLCD ^= byt; // flip again - restores non-relevant bits and causes relevant bit to change according to RGBbitMask

#if defined(useInterruptBasedTWI)
	TWI::disableISRactivity(); // disable ISR-based TWI activity

#endif // defined(useInterruptBasedTWI)
	MCP23017portExpanderSupport::writeRegister16Bit(MCP23017_B0_OLATx, portSwitches, portLCD); // write out 16-bit register (which sets address mode to toggle)

	MCP23017portExpanderSupport::setTransferMode(adaTWIbyteMode); // set address mode to byte mode

#if defined(useInterruptBasedTWI)
	TWI::enableISRactivity(); // enable ISR-based TWI activity

#endif // defined(useInterruptBasedTWI)
}

#endif // defined(useAdafruitRGBLCDshield)
#if defined(useLCDfonts)
static void LCD::loadCGRAMfont(const char * fontPtr)
{

	uint8_t numChars;

	numChars = pgm_read_byte(fontPtr++); // get the number of characters in the font

	for (uint8_t chr = 0; chr < numChars * 8; chr++) writeCGRAMbyte(chr, pgm_read_byte(fontPtr++)); // copy the CGRAM character data into RAM

}

#endif // defined(useLCDfonts)
#if defined(useLCDgraphics)
static void LCD::loadCGRAMcharacter(uint8_t chr, char * chrData)
{

	uint8_t cgrAddress;

	cgrAddress = ((chr & 0x07) << 3);

	for (uint8_t x = 0; x < 8; x++) writeCGRAMbyte(cgrAddress + x, (*chrData++) & 0x1F);

}

static void LCD::writeCGRAMbyte(uint8_t cgrAddress, uint8_t chr)
{

	// if there is any bit change in the CGRAM byte
	if ((chr ^ CGRAMbuffer[(uint16_t)(cgrAddress)]) & 0x9F) CGRAMbuffer[(uint16_t)(cgrAddress)] = (chr | cgramFlagDirty);

}

static uint8_t LCD::peekCGRAMbyte(uint8_t cgrAddress)
{

	// if there is any bit change in the CGRAM byte
	return CGRAMbuffer[(uint16_t)(cgrAddress)] & 0x1F;

}

static void LCD::flushCGRAM(void)
{

	uint8_t cgrAddress;
	uint8_t f;
	uint8_t y;
	uint8_t b;

#if defined(use4BitLCD)
	f = 0;
	y = 255;

	// cycle through the entire CGRAM buffer
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress++)
	{

		b = CGRAMbuffer[(uint16_t)(cgrAddress)];

		if (b & cgramFlagDirty)
		{

			f = 1;

			if (y != cgrAddress)
			{

				y = cgrAddress;
				writeCommand(lcdSetCGRAMaddress | cgrAddress); // set CGRAM address

			}

			writeByte(b & ~(cgramFlagDirty), lcdDataByte, lcdDelay0040us);
			y++;

		}

	}

	if (f) writeCommand(LCDgotoXYaddress); // set DDRAM to whatever the screen position was

#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
	// cycle through all 8 CGRAM characters
	for (uint8_t x = 0; x < 8; x++)
	{

		cgrAddress = x << 3;

		f = 0;

		// scan this CGRAM block for any dirty bytes
		for (y = 0; y < 8; y++) if (CGRAMbuffer[(uint16_t)(cgrAddress + y)] & cgramFlagDirty) f = 1;

		if (f)
		{

			LCDserialPort::chrOut(248 + x); // set CGRAM character

			for (y = 0; y < 8; y++)
			{

				f = CGRAMbuffer[(uint16_t)(cgrAddress + y)] & ~(cgramFlagDirty);
				LCDserialPort::chrOut(f); // write the character data to the character generator ram

			}

		}

	}

#endif // defined(useSerialLCD)
	// clear dirty flag on every byte in the buffer
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress++) CGRAMbuffer[(uint16_t)(cgrAddress)] &= ~(cgramFlagDirty);

}

#endif // defined(useLCDgraphics)
static void LCD::writeData(uint8_t value)
{

	uint8_t charFlags;
	uint8_t x;

	charFlags = 0;

	switch (value)
	{

		case 0x08: // go back one character
			charFlags |= (lcdCharGotoXY);
			LCDaddressX--;
			break;

		case 0x09:	// tab (go forward one character)
			charFlags |= (lcdCharGotoXY);
			LCDaddressX++;
			break;

		case 0x0D: // carriage return with clreol
#if defined(blankScreenOnMessage)
			if (timer0DisplayDelayFlags == 0)
#else // defined(blankScreenOnMessage)
			if ((timer0DisplayDelayFlags == 0) || (LCDaddressY))
#endif // defined(blankScreenOnMessage)
			{

				while (LCDaddressX < LCDcharWidth)
				{

#if defined(use4BitLCD)
					writeByte(' ', lcdDataByte, lcdDelay0040us);
#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
					LCDserialPort::chrOut(' ');
#endif // defined(useSerialLCD)
					LCDaddressX++;

				}

			}
			LCDaddressX = 0;
		case 0x0A: // linefeed
			LCDaddressY++;
			charFlags |= (lcdCharGotoXY);
			break;

		case 0x0C: // clear screen
#if defined(use4BitLCD)
			writeCommand(lcdClearDisplay); // clear display, set cursor position to zero
			charFlags |= (lcdCharGotoXY);
#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
			charFlags |= (lcdCharOutput);
#endif // defined(useSerialLCD)
			LCDaddressY = 0;
			LCDaddressX = 0;
			break;

		case 0x80 ... 0xCF: // hijack the gotoxy characters
			value &= 0x7F;
			LCDaddressY = value / 20;
			LCDaddressX = value % 20;
			if (devLCD.controlFlags & odvFlagDoubleHeight) LCDaddressY += 2;
			charFlags |= (lcdCharGotoXY);
			break;

		case 0x15:	// turn off display
		case 0x16:	// turn on display with no cursor and no blink (default)
		case 0x17:	// turn on display with no cursor and character blink
		case 0x18:	// turn on display with cursor and no blink
		case 0x19:	// turn on display with cursor and character blink
#if defined(use4BitLCD)
			x = value - 0x15;
			writeCommand(pgm_read_byte(&lcdDisplayModes[(uint16_t)(x)])); // set display mode
#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
			charFlags |= (lcdCharOutput);
#endif // defined(useSerialLCD)
			break;

		case 0x0B: // undefined character
		case 0x0E ... 0x14: // undefined character range (LCD brightness is processed elsewhere)
		case 0x1A ... 0x1F: // undefined character range
		case 0xE9 ... 0xF7: // undefined character range
		case 0xF8 ... 0xFF: // CGRAM definition stuff is processed elsewhere
			break;

		case 0x00 ... 0x07: // print defined CGRAM characters 0 through 7
		case 0x20 ... 0x7F: // print normal characters
#if defined(blankScreenOnMessage)
			if (timer0DisplayDelayFlags == 0)
#else // defined(blankScreenOnMessage)
			if ((timer0DisplayDelayFlags == 0) || (LCDaddressY))
#endif // defined(blankScreenOnMessage)
			{

				if ((LCDaddressX < LCDcharWidth) && (LCDaddressY < LCDcharHeight)) charFlags |= (lcdCharOutput);
				LCDaddressX++;

			}
			break;

		case 0xD0 ... 0xE8: // apparently, the Parallax LCD can play tunes
			charFlags |= (lcdCharOutput);
			break;

		default:
			charFlags |= (lcdCharOutput);
			break;

	}

#if defined(use4BitLCD)
	LCDgotoXYaddress = pgm_read_byte(&lcdBaseYposition[(uint16_t)(LCDaddressY & 0x03)]) + LCDaddressX;

#endif // defined(use4BitLCD)
	if (charFlags & lcdCharGotoXY)
	{

#if defined(use4BitLCD)
		writeCommand(LCDgotoXYaddress);
#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
		LCDserialPort::chrOut(0x80 + LCDaddressY * 20 + LCDaddressX);
#endif // defined(useSerialLCD)

	}

	if (charFlags & lcdCharOutput)
	{

#if defined(use4BitLCD)
		writeByte(value, lcdDataByte, lcdDelay0040us);
#endif // defined(use4BitLCD)
#if defined(useSerialLCD)
		LCDserialPort::chrOut(value);
		if (value == 0x0C) heart::wait0(delay0005msTick); // wait for 5 ms to allow cls to complete
#endif // defined(useSerialLCD)

	}

}

#if defined(use4BitLCD)
static void LCD::writeCommand(uint8_t value)
{

	uint8_t delay;

	switch (value)
	{

		case lcdClearDisplay:
		case lcdReturnHome:
		case lcdReturnHome | lcdClearDisplay:
			delay = lcdDelay0015ms;
			break;

		default:
			delay = lcdDelay0040us;
			break;

	}

	writeByte(value, lcdCommandByte, delay);

}

static void LCD::writeByte(uint8_t value, uint8_t flags, uint8_t delay)
{

	flags |= lcdSendNybble;

	writeNybble(value, (flags | lcdDelay0040us)); // send the high nybble with standard 40 us delay
	writeNybble(value << 4, (flags | delay)); // send the low nybble with the specified delay

}

static void LCD::writeNybble(uint8_t value, uint8_t flags)
{

#if defined(useLCDbufferedOutput)
	ringBuffer::push(lcdBuffer, (value & 0xF0) | (flags & 0x0F));
#else // defined(useLCDbufferedOutput)
#if defined(usePort4BitLCD)
	uint8_t oldSREG;

#endif // defined(usePort4BitLCD)
	while (timer1Command & t1cDelayLCD) idleProcess(); // wait for LCD timer delay to complete

#if defined(useTWI4BitLCD)
	if (flags & lcdSendNybble)
	{

#if defined(useInterruptBasedTWI)
		TWI::disableISRactivity(); // disable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)
		TWI::openChannelMain(lcdAddress, TW_WRITE); // open TWI as master transmitter
#if defined(useAdafruitRGBLCDshield)
		TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address
#endif // defined(useAdafruitRGBLCDshield)

	}

#endif // defined(useTWI4BitLCD)
#if defined(usePort4BitLCD)
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts
#endif // defined(usePort4BitLCD)

	outputNybble((value & 0xF0) | (flags & 0x0F));

#if defined(usePort4BitLCD)
	SREG = oldSREG; // restore interrupt flag status
#endif // defined(usePort4BitLCD)
#if defined(useTWI4BitLCD)

	if (flags & lcdSendNybble)
	{

		TWI::transmitChannel(TWI_STOP); // commit LCD port expander write
#if defined(useInterruptBasedTWI)
		TWI::enableISRactivity(); // enable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)

	}
#endif // defined(useTWI4BitLCD)
#endif // defined(useLCDbufferedOutput)

	heart::changeBitFlags(timer1Command, 0, t1cDelayLCD); // enable LCD delay

}

static void LCD::outputNybble(uint8_t LCDchar)
{

	uint8_t x;

	if (LCDchar & lcdSendNybble)
	{

#if defined(useTWI4BitLCD)
		portLCD &= ~(lcdRegisterSelect | lcdDirection | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) portLCD |= (lcdRegisterSelect); // set nybble type (command or data)
		if (LCDchar & 0b10000000) portLCD |= (lcdBit3); // set bit 3
		if (LCDchar & 0b01000000) portLCD |= (lcdBit2); // set bit 2
		if (LCDchar & 0b00100000) portLCD |= (lcdBit1); // set bit 1
		if (LCDchar & 0b00010000) portLCD |= (lcdBit0); // set bit 0

		portLCD |= (lcdEnable); // set enable high

		TWI::writeByte(portLCD); // write LCD port expander register

		portLCD &= ~(lcdEnable); // set enable low to 'tickle' enable bit (it will take TWI subsystem at least 22.5 us to write the update, which is larger than 50 ns required for enable 'tickling'

		TWI::writeByte(portLCD); // write LCD port expander register

#endif // defined(useTWI4BitLCD)
#if defined(usePort4BitLCD)
#if defined(__AVR_ATmega32U4__)
#if defined(useTinkerkitLCDmodule)
		PORTF &= ~(lcdDirection); // set data direction to write

		PORTE |= (lcdEnable); // set enable high

		if (LCDchar & lcdDataByte) PORTF |= (lcdRegisterSelect); // set nybble type (command or data)
		else PORTF &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTB |= (lcdBit3); // set bit 3
		else PORTB &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTD |= (lcdBit2); // set bit 2
		else PORTD &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTD |= (lcdBit1); // set bit 1
		else PORTD &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTF |= (lcdBit0); // set bit 0
		else PORTF &= ~(lcdBit0);

		PORTE &= ~(lcdEnable); // set enable low to 'tickle' enable bit

#endif // defined(useTinkerkitLCDmodule)
		// any port commands for any other ATmega32U4 board goes here

#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#if defined(useArduinoMega2560)
		PORTA |= (lcdEnable); // set enable high

		if (LCDchar & lcdDataByte) PORTA |= (lcdRegisterSelect); // set nybble type (command or data)
		else PORTA &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTA |= (lcdBit3); // set bit 3
		else PORTA &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTA |= (lcdBit2); // set bit 2
		else PORTA &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTA |= (lcdBit1); // set bit 1
		else PORTA &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTA |= (lcdBit0); // set bit 0
		else PORTA &= ~(lcdBit0);

		PORTA &= ~(lcdEnable); // set enable low to 'tickle' enable bit

#endif // defined(useArduinoMega2560)
		// any port commands for any other ATmega2560 board goes here

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
#if defined(useLegacyLCD)
		PORTD |= (lcdEnable); // set enable high

		if (LCDchar & lcdDataByte) PORTD |= (lcdRegisterSelect); // set nybble type (command or data)
		else PORTD &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTB |= (lcdBit3); // set bit 3
		else PORTB &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTB |= (lcdBit2); // set bit 2
		else PORTB &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTB |= (lcdBit1); // set bit 1
		else PORTB &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTD |= (lcdBit0); // set bit 0
		else PORTD &= ~(lcdBit0);

		PORTD &= ~(lcdEnable); // set enable low to 'tickle' enable bit

#endif // defined(useLegacyLCD)
		PORTB |= (lcdEnable); // set enable high

		if (LCDchar & lcdDataByte) PORTB |= (lcdRegisterSelect); // set nybble type (command or data)
		else PORTB &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTD |= (lcdBit3); // set bit 3
		else PORTD &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTD |= (lcdBit2); // set bit 2
		else PORTD &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTD |= (lcdBit1); // set bit 1
		else PORTD &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTD |= (lcdBit0); // set bit 0
		else PORTD &= ~(lcdBit0);

		PORTB &= ~(lcdEnable); // set enable low to 'tickle' enable bit

#if defined(useDFR0009LCD)
#endif // defined(useDFR0009LCD)
		// any port commands for any other ATmega168/328/328P board goes here

#endif // defined(__AVR_ATmega328P__)
#endif // defined(usePort4BitLCD)
	}

	x = LCDchar & lcdDelayFlags;

	switch (x) // lcdDelayCount is an accumulated tick count due to TWI output mechanism
	{

		case lcdDelay0015ms:
			lcdDelayCount += delayLCD015000usTick;
			break;

		case lcdDelay4100us:
			lcdDelayCount += delayLCD004100usTick;
			break;

		case lcdDelay0100us:
			lcdDelayCount += delayLCD000100usTick;
			break;

		case lcdDelay0040us:
			lcdDelayCount += delayLCD000040usTick;
			break;

		case lcdDataByte | lcdDelay0040us:
			lcdDelayCount += delayLCD000040usTick;
			break;

		default:
			lcdDelayCount += delayLCD004100usTick;
			break;

	}

}

#endif // defined(use4BitLCD)
#endif // defined(useLCDoutput)
