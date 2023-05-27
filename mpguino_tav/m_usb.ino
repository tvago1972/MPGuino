#if defined(__AVR_ATmega32U4__)
static void USB::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	PRR1 &= ~(1 << PRUSB); // turn on USB hardware

	UHWCON = (1 << UVREGE); // enable USB pad regulator

	USBCON = ((1 << USBE) | (1 << FRZCLK)); // enable USB controller, disable clock inputs

#if F_CPU == 16000000UL
	PLLCSR = ((1 << PINDIV) | (1 << PLLE)); // set PLL input prescaler for 16 MHz clock source, and enable PLL
#elif F_CPU == 8000000UL
	PLLCSR = (1 << PLLE); // set PLL input prescaler for 8 MHz clock source, and enable PLL
#else // F_CPU == 16000000UL
#error *** F_CPU clock rate not supported!!! ***
#endif // F_CPU == 16000000UL

	while (!(PLLCSR & (1 << PLOCK))); // wait for PLL lock

	USBCON = ((1 << USBE) | (1 << OTGPADE)); // keep USB controller enabled, enable clock inputs, enable VBUS pad

	UDCON = 0; // select USB high speed mode

	usbConfiguration = 0;

	UDIEN = ((1 << EORSTE) | (1 << SOFE)); // enable End-Of-Reset, Start-Of-Frame interrupts

	SREG = oldSREG; // restore interrupt flag status

}

static void USB::shutdown(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	UDIEN = 0; // disable all USB interrupts

	USBCON = 0; // disable USB controller

	PLLCSR = 0; // disable PLL

	UHWCON = 0; // disable USB pad regulator

	PRR1 |= (1 << PRUSB); // turn off USB hardware

	SREG = oldSREG; // restore interrupt flag status

}

#ifdef useUSBserial
static void usbDevice::init(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	PRR1 &= ~(1 << PRUSB); // turn on USB hardware

	UHWCON = (1 << UVREGE); // enable USB pad regulator

	USBCON = ((1 << USBE) | (1 << FRZCLK)); // enable USB controller, disable clock inputs

	PLLCSR = ((1 << PINDIV) | (1 << PLLE)); // set PLL input prescaler for 16 MHz clock source, and enable PLL

	while (!(PLLCSR & (1 << PLOCK))); // wait for PLL lock

	USBCON = ((1 << USBE) | (1 << OTGPADE)); // keep USB controller enabled, enable clock inputs, enable VBUS pad

	UDCON = 0; // select USB high speed mode

	usbConfiguration = 0;
	cdcLineRTSDTR = 0;

	UDIEN = ((1 << EORSTE) | (1 << SOFE)); // enable End-Of-Reset, Start-Of-Frame interrupts

	ringBuffer::init(USBoutputBuffer, USBoutputData);
	ringBuffer::init(USBinputBuffer, USBinputData);

	devUSB.chrOut = chrOut;
	devUSB.chrIn = chrIn;

	SREG = oldSREG; // restore interrupt flag status

}

static void usbDevice::shutdown(void)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

	usbConfiguration = 0;
	cdcLineRTSDTR = 0;

	UDIEN = 0; // disable all USB interrupts

	USBCON = ((1 << USBE) | (1 << FRZCLK)); // enable USB controller, disable clock inputs
	PLLCSR = (1 << PINDIV);  // disable PLL

	UHWCON = 0; // disable USB pad regulator

	PRR1 |= (1 << PRUSB); // turn off USB hardware to save power

	SREG = oldSREG; // restore interrupt flag status

}

static void usbDevice::chrOut(uint8_t chr)
{

	ringBuffer::push(USBoutputBuffer, chr);

}

static uint8_t usbDevice::chrIn(void)
{

	return ringBuffer::pull(USBinputBuffer);

}

// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR( USB_COM_vect )
{

	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t descriptorValue;
	const uint8_t * listPtr;
	const uint8_t * cfg;
	const uint8_t * descriptorPtr;
	uint8_t endpointFlags;
	uint8_t i;
	uint8_t n;
	uint8_t len;
	uint8_t en;
	uint8_t * p;
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t	descriptorLength;

	UENUM = 0;
	endpointFlags = UEINTX;

	if (endpointFlags & (1 << RXSTPI))
	{

		bmRequestType = UEDATX;
		bRequest = UEDATX;

		wValue = UEDATX;
		wValue |= (UEDATX << 8);

		wIndex = UEDATX;
		wIndex |= (UEDATX << 8);

		wLength = UEDATX;
		wLength |= (UEDATX << 8);

		UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));

		if (bRequest == GET_DESCRIPTOR)
		{

			listPtr = (const uint8_t *)(thisEndpointDescriptorList);

			for (i=0; ; i++)
			{

				if (i >= NUM_DESC_LIST)
				{

					UECONX = ((1 << STALLRQ) | (1 << EPEN));  //stall
					return;

				}

				descriptorValue = pgm_read_word(listPtr);
				if (descriptorValue != wValue)
				{

					listPtr += sizeof(struct endpointDescriptorList);
					continue;

				}

				listPtr += 2;
				descriptorValue = pgm_read_word(listPtr);

				if (descriptorValue != wIndex)
				{

					listPtr += sizeof(struct endpointDescriptorList) - 2;
					continue;

				}

				listPtr += 2;
				descriptorPtr = (const uint8_t *)pgm_read_word(listPtr);

				listPtr += 2;
				descriptorLength = pgm_read_byte(listPtr);

				break;

			}

			len = (wLength < 256) ? wLength : 255;
			if (len > descriptorLength) len = descriptorLength;

			do
			{

				// wait for host ready for IN packet
				do
				{

					i = UEINTX;

				} while (!(i & ((1 << TXINI)|(1 << RXOUTI))));

				if (i & (1 << RXOUTI)) return;	// abort

				// send IN packet
				n = (len < ENDPOINT0_SIZE) ? len : ENDPOINT0_SIZE;
				for (i = n; i; i--) UEDATX = pgm_read_byte(descriptorPtr++);

				len -= n;
				UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission

			} while ((len) || (n == ENDPOINT0_SIZE));

			return;

		}

		if (bRequest == SET_ADDRESS)
		{

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission

			UDADDR = wValue | (1 << ADDEN);
			return;

		}

		if (bRequest == SET_CONFIGURATION && bmRequestType == 0)
		{

			usbConfiguration = wValue;
			cdcLineRTSDTR = 0;
			transmit_flush_timer = 0;

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission

			cfg = endpoint_config_table;

			for (i = 1; i < 5; i++)
			{

				UENUM = i;
				en = pgm_read_byte(cfg++);
				UECONX = en;

				if (en)
				{

					UECFG0X = pgm_read_byte(cfg++);
					UECFG1X = pgm_read_byte(cfg++);

				}

			}

			UERST = 0x1E;
			UERST = 0;
			return;

		}

		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80)
		{

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission
			UEDATX = usbConfiguration;
			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == CDC_GET_LINE_CODING && bmRequestType == 0xA1)
		{

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission

			p = cdc_line_coding;
			for (i = 0; i < 7; i++) UEDATX = *p++;

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == CDC_SET_LINE_CODING && bmRequestType == 0x21)
		{

		while (!(UEINTX & (1 << RXOUTI)));

			p = cdc_line_coding;

			for (i=0; i<7; i++) *p++ = UEDATX;

			UEINTX = ~(1 << RXOUTI);
			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == CDC_SET_CONTROL_LINE_STATE && bmRequestType == 0x21)
		{

			cdcLineRTSDTR = wValue;

			while (!(UEINTX & (1 << TXINI))); // wait for hardware to finish transmission

			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

		if (bRequest == GET_STATUS)
		{

			while (!(UEINTX & (1<<TXINI))); // wait for hardware to finish transmission
			i = 0;

#ifdef supportUSBendpointHalt
			if (bmRequestType == 0x82)
			{

				UENUM = wIndex;
				if (UECONX & (1 << STALLRQ)) i = 1;
				UENUM = 0;

			}

#endif // supportUSBendpointHalt
			UEDATX = i;
			UEDATX = 0;
			UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
			return;

		}

#ifdef supportUSBendpointHalt
		if (((bRequest == CLEAR_FEATURE) || (bRequest == SET_FEATURE)) && (bmRequestType == 0x02) && (wValue == 0))
		{

			i = wIndex & 0x7F;
			if ((i >= 1) && (i <= MAX_ENDPOINT))
			{

				UEINTX = ~(1 << TXINI); // clear TXINI flag to signal to hardware to start transmission
				UENUM = i;

				if (bRequest == SET_FEATURE)
				{

					UECONX = (1 << STALLRQ)|(1 << EPEN);

				}
				else
				{

					UECONX = ((1 << STALLRQC) | (1 << RSTDT) | (1 << EPEN));
					UERST = (1 << i);
					UERST = 0;

				}

				return;

			}

		}

#endif // supportUSBendpointHalt
	}

	UECONX = ((1 << STALLRQ) | (1 << EPEN));	// stall

}


// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{

	uint8_t intbits;
	uint8_t t;

	intbits = UDINT;
	UDINT = 0;

	if (intbits & (1 << EORSTI))
	{

		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1 << RXSTPE);
		usb_configuration = 0;
		cdc_line_rtsdtr = 0;

	}

	if (intbits & (1 << SOFI))
	{

		if (usb_configuration)
		{

			t = transmit_flush_timer;
			if (t)
			{

				transmit_flush_timer = --t;

				if (!t)
				{

					UENUM = CDC_TX_ENDPOINT;
					UEINTX = 0x3A;

				}

			}

		}

	}

}

#endif // useUSBserial
#endif // defined(__AVR_ATmega32U4__)
