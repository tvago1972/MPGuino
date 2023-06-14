#ifdef useLCDoutput
/* LCD hardware support section */

static void LCD::init(void)
{

#ifdef use4BitLCD
	uint8_t oldSREG;

#endif // use4BitLCD
	devLCD.chrOut = LCD::writeData;

#if defined(useLCDgraphics)
	for (uint8_t x = 0; x < 64; x++) CGRAMbuffer[(uint16_t)(x)] = cgramFlagDirty;

#endif // defined(useLCDgraphics)
#ifdef useParallaxSerialLCDmodule
	devLCDserial.controlFlags &= ~(odvFlagCRLF);

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
#ifdef useBufferedLCD
	ringBuffer::init(lcdBuffer, LCDdata);

#endif // useBufferedLCD
#ifdef useTWILCD
#ifdef useTWIbuttons
	disableIntSample(); // disable TWI button sampling

#endif // useTWIbuttons
	portLCD = 0; // reset LCD port byte
#ifdef useAdafruitRGBLCDshield
	portSwitches = 0; // reset button port byte (contains two of the three LCD backlight LED bits)

	adafruitRGBLCDsupport::setTransferMode(adaTWIbyteMode);
#endif // useAdafruitRGBLCDshield

#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
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

#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here

#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRA |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);
	DDRB |= (lcdBrightness | lcdContrast);

#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here

#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// set OC0A to non-inverting mode for LCD contrast
	TCCR0A &= ~(1 << COM0A0);
	TCCR0A |= (1 << COM0A1);

	// set OC1A to non-inverting mode for LCD brightness
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);

	// enable LCD pins
	DDRB |= (lcdBit3 | lcdBit2 | lcdBit1 | lcdBrightness);
	DDRD |= (lcdBit0 | lcdContrast | lcdEnable | lcdRegisterSelect);

#endif // defined(__AVR_ATmega328P__)
	setContrast(EEPROM::readByte(pContrastIdx));

#endif // useTWILCD
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	lcdDelayCount = 0;
	timer1Command &= ~(t1cDelayLCD); // turn off LCD delay

	SREG = oldSREG; // restore interrupt flag status

	// perform reset by instruction on LCD interface
	// the below 5 lines actually take up less space than using a PROGMEM table / loop to write these values
	writeNybble(lcdNullValue, lcdDelay0015ms); // wait for more than 15 msec
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay4100us); // send (B0011) to DB7-4, then wait for more than 4.1 ms
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(0b00110000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0011) to DB7-4, then wait for more than 100 us
	writeNybble(0b00100000, lcdCommandByte | lcdSendNybble | lcdDelay0100us); // send (B0010) to DB7-4 for 4 bit mode, then wait for more than 40 us

	// ready to use normal LCD output function now!
	writeCommand(lcdFunctionSet | lcdFSnumberOfLines); // 4-bit interface, 2 display lines, 5x8 font

#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // useBufferedLCD
#if useTWIbuttons && useTWILCD
	enableIntSample(); // enable TWI button sampling

#endif // useTWIbuttons && useTWILCD
#endif // use4BitLCD
	writeData(0x11); // set initial brightness
	writeData(0x16); // display control - turn on display, no cursor, no blink
	writeData(0x0C); // clear display, set cursor position to zero

#ifdef useBufferedLCD
#ifdef use4BitLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer

#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
	ringBuffer::flush(LCDserialBuffer); // clear the LCD buffer to force the LCD screen to clear

#endif // useParallaxSerialLCDmodule
#endif // useBufferedLCD
}

static void LCD::shutdown(void)
{

	writeData(0x12); // turn off LCD brightness
#ifdef useLegacyLCD
	setContrast(255); // turn off LCD contrast
#endif // useLegacyLCD

#ifdef useParallaxSerialLCDmodule
	LCDserialPort::chrOut(21); // turn off LCD display
#ifdef useBufferedLCD
	ringBuffer::flush(LCDserialBuffer); // flush LCD output buffer

#endif // useBufferedLCD
	LCDserialPort::shutdown(); // shut down LCD serial port

#endif // useParallaxSerialLCDmodule
#ifdef use4BitLCD
	writeCommand(lcdDisplayControl); // turn off LCD display
#ifdef useBufferedLCD
	ringBuffer::flush(lcdBuffer); // flush LCD output buffer
#endif // useBufferedLCD

#ifndef useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
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

#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here

#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	// disable LCD pins
	DDRA &= ~(lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0 | lcdEnable | lcdRegisterSelect);

	PORTB |= lcdContrast; // ensure LCD contrast is turned off
#ifdef useInvertedLegacyLCDbrightness
	PORTB |= lcdBrightness; // ensure LCD brightness is turned off
#else // useInvertedLegacyLCDbrightness
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#endif // useInvertedLegacyLCDbrightness

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here

#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	// disable LCD pins
	DDRB &= ~(lcdBit3 | lcdBit2 | lcdBit1);
	DDRD &= ~(lcdBit0 | lcdEnable | lcdRegisterSelect);

	PORTD |= lcdContrast;
#ifdef useInvertedLegacyLCDbrightness
	PORTB |= lcdBrightness; // ensure LCD brightness is turned off
#else // useInvertedLegacyLCDbrightness
	PORTB &= ~(lcdBrightness); // ensure LCD brightness is turned off
#endif // useInvertedLegacyLCDbrightness

	// set OC0A to disabled
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));

	// set OC1A to disabled
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD
#endif // use4BitLCD
}

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

#ifdef use4BitLCD
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

#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
	// cycle through all 8 CGRAM characters
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress = cgrAddress + 8)
	{

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

#endif // useParallaxSerialLCDmodule
	// clear dirty flag on every byte in the buffer
	for (cgrAddress = 0; cgrAddress < 64; cgrAddress++) CGRAMbuffer[(uint16_t)(cgrAddress)] &= ~(cgramFlagDirty);

}

#endif // defined(useLCDgraphics)
static void LCD::setBrightness(uint8_t idx)
{

#ifdef use4BitLCD
#ifdef useTWILCD
#ifdef useAdafruitRGBLCDshield
	if (idx) idx = EEPROM::readByte(pLCDcolorIdx); // get LCD backlight color

	setRGBcolor(idx); // set LCD backlight color

#else // useAdafruitRGBLCDshield
	if (idx) portLCD |= lcdBrightness; // turn on LCD backlight
	else portLCD &= ~(lcdBrightness); // turn off LCD backlight
#endif // useAdafruitRGBLCDshield
#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
	OCR1B = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR1A = pgm_read_byte(&brightness[(unsigned int)(idx)]);
#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD
#endif // use4BitLCD

}

#ifdef useLegacyLCD
static void LCD::setContrast(uint8_t idx)
{

#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
	OCR1A = idx;
#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
	OCR0A = idx;
#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	OCR0A = idx;
#endif // defined(__AVR_ATmega328P__)

}

#endif // useLegacyLCD
#ifdef useAdafruitRGBLCDshield
const uint8_t RGBcolors[8] PROGMEM =
{

	 0b11000001	// off
	,0b01000001	// green
	,0b10000001	// red
	,0b00000001	// yeller
	,0b11000000	// blue
	,0b01000000	// cyan
	,0b10000000	// magenta
	,0b00000000	// white

};

static void LCD::setRGBcolor(uint8_t idx)
{

	uint8_t RGBbitMask;
	uint8_t byt;

	RGBbitMask = pgm_read_byte(&RGBcolors[(unsigned int)(idx & 0x07)]); // read the LCD backlight color bitmask pattern

	byt = RGBbitMask; // get the color bitmask pattern
	byt ^= portSwitches; // flip the color bitmask by what's stored in the portSwitches register
	byt &= (lcdBrightnessRed | lcdBrightnessGreen); // strip out all but the relevant bits
	byt ^= portSwitches; // flip again - restores non-relevant bits and causes relevant bits to change according to RGBbitMask
	portSwitches = byt; // save the modified portSwitches register

	byt = RGBbitMask; // get the color bitmask
	byt ^= portLCD; // flip the color bitmask by what's stored in the portLCD register
	byt &= (lcdBrightnessBlue); // strip out all but the relevant bit
	byt ^= portLCD; // flip again - restores non-relevant bits and causes relevant bit to change according to RGBbitMask
	portLCD = byt; // save the modified portLCD register

#ifdef useTWIbuttons
	disableIntSample(); // disable TWI button sampling

#endif // useTWIbuttons
	adafruitRGBLCDsupport::writeRegister16Bit(MCP23017_B0_OLATx, portSwitches, portLCD); // write out 16-bit register (which sets address mode to toggle)

	adafruitRGBLCDsupport::setTransferMode(adaTWIbyteMode); // set address mode to byte mode

#ifdef useTWIbuttons
	enableIntSample(); // enable TWI button sampling

#endif // useTWIbuttons
}

#endif // useAdafruitRGBLCDshield
#ifdef use4BitLCD
static const uint8_t lcdCharBackwardX =	0b10000000;
static const uint8_t lcdCharBackwardY =	0b01000000;
static const uint8_t lcdCharForwardX =	0b00100000;
static const uint8_t lcdCharForwardY =	0b00010000;
static const uint8_t lcdCharZeroX =		0b00001000;
static const uint8_t lcdCharZeroY =		0b00000100;
static const uint8_t lcdCharGotoXY =	0b00000010;
static const uint8_t lcdCharOutput =	0b00000001;

static const uint8_t lcdCharCalcXY =	(lcdCharBackwardX | lcdCharBackwardY | lcdCharForwardX | lcdCharForwardY | lcdCharZeroX | lcdCharZeroY | lcdCharGotoXY);

static const uint8_t lcdBaseYposition[] PROGMEM = {
	 lcdSetDDRAMaddress
	,lcdSetDDRAMaddress | 0x40
	,lcdSetDDRAMaddress | 0x14
	,lcdSetDDRAMaddress | 0x54
};

static const uint8_t lcdDisplayModes[] PROGMEM = {
	 lcdDisplayControl																		// turn off display
	,lcdDisplayControl | lcdDCdisplayShow													// turn on display, no cursor, no character blink (default)
	,lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorBlinkControl							// turn on display, no cursor, with character blink
	,lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorControl								// turn on display, cursor, no character blink
	,lcdDisplayControl | lcdDCdisplayShow | lcdDCcursorControl | lcdDCcursorBlinkControl	// turn on display, cursor, with character blink
};

#endif // use4BitLCD
static void LCD::writeData(uint8_t value)
{

	uint8_t charFlags;
	uint8_t x;

	charFlags = 0;

	switch (value)
	{

		case 0x08: // go back one character
			charFlags |= (lcdCharGotoXY | lcdCharBackwardX);
			break;

		case 0x09:	// tab (go forward one character)
			charFlags |= (lcdCharGotoXY | lcdCharForwardX);
			break;

		case 0x0D: // carriage return with clreol
#ifdef blankScreenOnMessage
			if ((timer0Command & t0cDisplayDelay) == 0)
#else // blankScreenOnMessage
			if (((timer0Command & t0cDisplayDelay) == 0) || (LCDaddressY > 0))
#endif // blankScreenOnMessage
			{

				while (LCDaddressX < 20)
				{

#ifdef use4BitLCD
					writeByte(' ', lcdDataByte, lcdDelay0040us);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
					LCDserialPort::chrOut(' ');
#endif // useParallaxSerialLCDmodule
					LCDaddressX++;

				}

			}
			charFlags |= (lcdCharGotoXY | lcdCharZeroX);
		case 0x0A: // linefeed
			charFlags |= (lcdCharGotoXY | lcdCharForwardY);
			break;

		case 0x0C: // clear screen
#ifdef use4BitLCD
			writeCommand(lcdClearDisplay); // clear display, set cursor position to zero
			charFlags |= (lcdCharZeroX | lcdCharZeroY);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
			charFlags |= (lcdCharOutput | lcdCharZeroX | lcdCharZeroY);
#endif // useParallaxSerialLCDmodule
			break;

		case 0x80 ... 0xCF: // hijack the gotoxy characters
			value &= 0x7F;
			LCDaddressY = value / 20;
			LCDaddressX = value % 20;
			charFlags |= (lcdCharGotoXY);
			break;

#ifndef useBinaryLCDbrightness
		case 0x0F:	// LCD backlight full bright
		case 0x10:	// LCD backlight medium bright
#endif // useBinaryLCDbrightness
		case 0x11:	// turn on LCD backlight / LCD backlight low bright
		case 0x12:	// turn off LCD backlight
			brightnessIdx = 0x12 - value;
#ifdef use4BitLCD
			setBrightness(brightnessIdx);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
			charFlags |= (lcdCharOutput);
#endif // useParallaxSerialLCDmodule
			break;

		case 0x15:	// turn off display
		case 0x16:	// turn on display with no cursor and no blink (default)
		case 0x17:	// turn on display with no cursor and character blink
		case 0x18:	// turn on display with cursor and no blink
		case 0x19:	// turn on display with cursor and character blink
#ifdef use4BitLCD
			x = value - 0x15;
			writeCommand(pgm_read_byte(&lcdDisplayModes[(unsigned int)(x)])); // set display mode
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
			charFlags |= (lcdCharOutput);
#endif // useParallaxSerialLCDmodule
			break;

		case 0x0B: // undefined character
		case 0x0E: // undefined character
#ifdef useBinaryLCDbrightness
		case 0x0F ... 0x10:	// undefined character range
#endif // useBinaryLCDbrightness
		case 0x13 ... 0x14: // undefined character range
		case 0x1A ... 0x1F: // undefined character range
		case 0xE9 ... 0xF7: // undefined character range
		case 0xF8 ... 0xFF: // CGRAM definition stuff is processed elsewhere
			break;

		case 0x00 ... 0x07: // print defined CGRAM characters 0 through 7
		case 0x20 ... 0x7F: // print normal characters
#ifdef blankScreenOnMessage
			if (timer0Command & t0cDisplayDelay) charFlags |= (lcdCharGotoXY | lcdCharForwardX | lcdCharForwardY);
#else // blankScreenOnMessage
			if ((timer0Command & t0cDisplayDelay) && (LCDaddressY < 1)) charFlags |= (lcdCharGotoXY | lcdCharForwardX | lcdCharForwardY);
#endif // blankScreenOnMessage
			else charFlags |= (lcdCharOutput | lcdCharForwardX | lcdCharForwardY);
			break;

		case 0xD0 ... 0xE8: // apparently, the Parallax LCD can play tunes
			charFlags |= (lcdCharOutput);
			break;

		default:
			charFlags |= (lcdCharOutput);
			break;

	}

	if (charFlags & lcdCharForwardX)
	{

		LCDaddressX++;
		if (LCDaddressX < 20) charFlags &= ~(lcdCharForwardY);
		else charFlags |= (lcdCharZeroX);

	}

	if (charFlags & lcdCharForwardY)
	{

		LCDaddressY++;
		if (LCDaddressY > 3) charFlags |= (lcdCharZeroY);

	}

	if (charFlags & lcdCharZeroX) LCDaddressX = 0;
	if (charFlags & lcdCharZeroY) LCDaddressY = 0;

	if (charFlags & lcdCharBackwardX)
	{

		if (LCDaddressX) LCDaddressX--;
		else LCDaddressX = 19;

	}

	if (charFlags & lcdCharBackwardY)
	{

		if (LCDaddressY) LCDaddressY--;
		else LCDaddressY = 3;

	}

#ifdef use4BitLCD
	if (charFlags & lcdCharCalcXY) LCDgotoXYaddress = pgm_read_byte(&lcdBaseYposition[LCDaddressY]) + LCDaddressX;

#endif // use4BitLCD
	if (charFlags & lcdCharGotoXY)
	{

#ifdef use4BitLCD
		writeCommand(LCDgotoXYaddress);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
		LCDserialPort::chrOut(0x80 + LCDaddressY * 20 + LCDaddressX);
#endif // useParallaxSerialLCDmodule

	}

	if (charFlags & lcdCharOutput)
	{

#ifdef use4BitLCD
		writeByte(value, lcdDataByte, lcdDelay0040us);
#endif // use4BitLCD
#ifdef useParallaxSerialLCDmodule
		LCDserialPort::chrOut(value);
		if (value == 0x0C) delay0(delay0005msTick); // wait for 5 ms
#endif // useParallaxSerialLCDmodule

	}

}

#ifdef use4BitLCD
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

#ifdef useBufferedLCD
	ringBuffer::push(lcdBuffer, (value & 0xF0) | (flags & 0x0F));
#else // useBufferedLCD
	uint8_t oldSREG;

	while (timer1Command & t1cDelayLCD) idleProcess(); // wait for LCD timer delay to complete

#ifdef useTWILCD
	if (flags & lcdSendNybble)
	{

#ifdef useTWIbuttons
		disableIntSample(); // disable TWI button sampling
#endif // useTWIbuttons
		TWI::openChannelMain(lcdAddress, TW_WRITE); // open TWI as master transmitter
		TWI::writeByte(MCP23017_B1_OLATB); // specify bank B output latch register address

	}

#endif // useTWILCD
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	outputNybble((value & 0xF0) | (flags & 0x0F));

	SREG = oldSREG; // restore interrupt flag status
#ifdef useTWILCD

	if (flags & lcdSendNybble)
	{

		TWI::transmitChannel(TWI_STOP); // commit LCD port expander write
#ifdef useTWIbuttons
		enableIntSample(); // enable TWI button sampling
#endif // useTWIbuttons

	}
#endif // useTWILCD
#endif // useBufferedLCD

	changeBitFlags(timer1Command, 0, t1cDelayLCD); // enable LCD delay

}

static void LCD::outputNybble(uint8_t LCDchar)
{

	uint8_t x;

	if (LCDchar & lcdSendNybble)
	{

#ifdef useTWILCD
		portLCD &= ~(lcdRegisterSelect | lcdDirection | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) portLCD |= lcdRegisterSelect; // set nybble type (command or data)
		if (LCDchar & 0b10000000) portLCD |= lcdBit3; // set bit 3
		if (LCDchar & 0b01000000) portLCD |= lcdBit2; // set bit 2
		if (LCDchar & 0b00100000) portLCD |= lcdBit1; // set bit 1
		if (LCDchar & 0b00010000) portLCD |= lcdBit0; // set bit 0

		portLCD |= lcdEnable; // set enable high

		TWI::writeByte(portLCD); // write LCD port expander register

		portLCD &= ~(lcdEnable); // set enable low to 'tickle' enable bit (it will take TWI subsystem at least 22.5 us to write the update, which is larger than 50 ns required for enable 'tickling'

		TWI::writeByte(portLCD); // write LCD port expander register
#else // useTWILCD
#if defined(__AVR_ATmega32U4__)
#ifdef useTinkerkitLCDmodule
		PORTF &= ~(lcdDirection); // set data direction to write

		PORTE |= lcdEnable; // set enable high

		if (LCDchar & lcdDataByte) PORTF |= lcdRegisterSelect; // set nybble type (command or data)
		else PORTF &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTB |= lcdBit3; // set bit 3
		else PORTB &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTD |= lcdBit2; // set bit 2
		else PORTD &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTD |= lcdBit1; // set bit 1
		else PORTD &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTF |= lcdBit0; // set bit 0
		else PORTF &= ~(lcdBit0);

		PORTE &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#else // useTinkerkitLCDmodule
	// any port commands for any other ATmega32U4 board goes here
#endif // useTinkerkitLCDmodule
#endif // defined(__AVR_ATmega32U4__)
#if defined(__AVR_ATmega2560__)
#ifdef useArduinoMega2560
		PORTA |= lcdEnable; // set enable high

		PORTA &= ~(lcdRegisterSelect | lcdBit3 | lcdBit2 | lcdBit1 | lcdBit0);
		if (LCDchar & lcdDataByte) PORTA |= lcdRegisterSelect; // set nybble type (command or data)
		else PORTA &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTA |= lcdBit3; // set bit 3
		else PORTA &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTA |= lcdBit2; // set bit 2
		else PORTA &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTA |= lcdBit1; // set bit 1
		else PORTA &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTA |= lcdBit0; // set bit 0
		else PORTA &= ~(lcdBit0);

		PORTA &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#else // useArduinoMega2560
	// any port commands for any other ATmega2560 board goes here
#endif // useArduinoMega2560
#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
		PORTD |= lcdEnable; // set enable high

		if (LCDchar & lcdDataByte) PORTD |= lcdRegisterSelect; // set nybble type (command or data)
		else PORTD &= ~(lcdRegisterSelect);

		if (LCDchar & 0b10000000) PORTB |= lcdBit3; // set bit 3
		else PORTB &= ~(lcdBit3);

		if (LCDchar & 0b01000000) PORTB |= lcdBit2; // set bit 2
		else PORTB &= ~(lcdBit2);

		if (LCDchar & 0b00100000) PORTB |= lcdBit1; // set bit 1
		else PORTB &= ~(lcdBit1);

		if (LCDchar & 0b00010000) PORTD |= lcdBit0; // set bit 0
		else PORTD &= ~(lcdBit0);

		PORTD &= ~(lcdEnable); // set enable low to 'tickle' enable bit
#endif // defined(__AVR_ATmega328P__)
#endif // useTWILCD

	}

	x = LCDchar & lcdDelayFlags;

	switch (x) // lcdDelayCount is an accumulated tick count due to TWI output mechanism
	{

		case lcdDelay0015ms:
			lcdDelayCount += delayLCD15000usTick;
			break;

		case lcdDelay4100us:
			lcdDelayCount += delayLCD04100usTick;
			break;

		case lcdDelay0100us:
			lcdDelayCount += delayLCD00100usTick;
			break;

		case lcdDelay0040us:
			lcdDelayCount += delayLCD00040usTick;
			break;

		case lcdDataByte | lcdDelay0040us:
			lcdDelayCount += delayLCD00040usTick;
			break;

		default:
			lcdDelayCount += delayLCD04100usTick;
			break;

	}

}

#if useTWIbuttons && useTWILCD
static void LCD::disableIntSample(void)
{

	changeBitFlags(timer0Command, t0cEnableTWIsample, 0); // disable TWI button sampling as it interferes with TWI LCD output
	while (twiStatusFlags & twiBlockMainProgram) idleProcess(); // wait for any in-progress TWI button samples to finish

}

static void LCD::enableIntSample(void)
{

	changeBitFlags(timer0Command, 0, t0cEnableTWIsample); // re-enable TWI button sampling

}

#endif // useTWIbuttons && useTWILCD
#endif // use4BitLCD
#endif // useLCDoutput
