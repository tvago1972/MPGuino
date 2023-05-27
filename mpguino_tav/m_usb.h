#if defined(__AVR_ATmega32U4__)
/* start of baseline ATmega native USB support */

namespace USB
{

	static void init(void);
	static void shutdown(void);

};

#ifdef useUSBserial
namespace usbDevice
{

	static void init(void);
	static void shutdown(void);
	static void chrOut(uint8_t chr);
	static uint8_t chrIn(void);

};

interfaceDevice devUSB;

ringBufferVariable USBoutputBuffer;
ringBufferVariable USBinputBuffer;

volatile uint8_t USBoutputData[256];
volatile uint8_t USBinputData[256];

// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usbConfiguration;

// the time remaining before we transmit any partially full
// packet, or send a zero length packet.
static volatile uint8_t transmit_flush_timer;
static uint8_t transmit_previous_timeout;

// serial port settings (baud rate, control signals, etc) set
// by the PC.  These are ignored, but kept in RAM.
static uint8_t cdc_line_coding[7] = {0x00, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x08};
static uint8_t cdcLineRTSDTR;

// constants corresponding to the various serial parameters
static const uint8_t USB_SERIAL_DTR =			0x01;
static const uint8_t USB_SERIAL_RTS =			0x02;
static const uint8_t USB_SERIAL_1_STOP =		0;
static const uint8_t USB_SERIAL_1_5_STOP =		1;
static const uint8_t USB_SERIAL_2_STOP =		2;
static const uint8_t USB_SERIAL_PARITY_NONE =	0;
static const uint8_t USB_SERIAL_PARITY_ODD =	1;
static const uint8_t USB_SERIAL_PARITY_EVEN =	2;
static const uint8_t USB_SERIAL_PARITY_MARK =	3;
static const uint8_t USB_SERIAL_PARITY_SPACE =	4;
static const uint8_t USB_SERIAL_DCD =			0x01;
static const uint8_t USB_SERIAL_DSR =			0x02;
static const uint8_t USB_SERIAL_BREAK =			0x04;
static const uint8_t USB_SERIAL_RI =			0x08;
static const uint8_t USB_SERIAL_FRAME_ERR =		0x10;
static const uint8_t USB_SERIAL_PARITY_ERR =	0x20;
static const uint8_t USB_SERIAL_OVERRUN_ERR =	0x40;

static const uint8_t EP_TYPE_CONTROL =			0x00;
static const uint8_t EP_TYPE_BULK_IN =			0x81;
static const uint8_t EP_TYPE_BULK_OUT =			0x80;
static const uint8_t EP_TYPE_INTERRUPT_IN =		0xC1;
static const uint8_t EP_TYPE_INTERRUPT_OUT =	0xC0;
static const uint8_t EP_TYPE_ISOCHRONOUS_IN =	0x41;
static const uint8_t EP_TYPE_ISOCHRONOUS_OUT =	0x40;
static const uint8_t EP_SINGLE_BUFFER =			0x02;
static const uint8_t EP_DOUBLE_BUFFER =			0x06;
#define EP_SIZE(s)	((s) == 64 ? 0x30 :	\
			((s) == 32 ? 0x20 :	\
			((s) == 16 ? 0x10 :	\
			             0x00)))

#define MAX_ENDPOINT		4

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

//#define HW_CONFIG() (UHWCON = 0x01)

//#ifdef M1
//	#define PLL_CONFIG() (PLLCSR = 0x02) // fixed to 8MHz clock
//#else
//	#define PLL_CONFIG() (PLLCSR = 0x12) // 0001 0010 For a 16MHz clock
//#endif

//#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
//#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))

// standard control endpoint request types
static const uint8_t GET_STATUS =					0x00;
static const uint8_t CLEAR_FEATURE =				0x01;
static const uint8_t SET_FEATURE =					0x03;
static const uint8_t SET_ADDRESS =					0x05;
static const uint8_t GET_DESCRIPTOR =				0x06;
static const uint8_t GET_CONFIGURATION =			0x08;
static const uint8_t SET_CONFIGURATION =			0x09;
static const uint8_t GET_INTERFACE =				0x0A;
static const uint8_t SET_INTERFACE =				0x0B;

// HID (human interface device);
static const uint8_t HID_GET_REPORT =				0x01;
static const uint8_t HID_GET_PROTOCOL =				0x03;
static const uint8_t HID_SET_REPORT =				0x09;
static const uint8_t HID_SET_IDLE =					0x0A;
static const uint8_t HID_SET_PROTOCOL =				0x0B;

// CDC (communication class device);
static const uint8_t CDC_SET_LINE_CODING =			0x20;
static const uint8_t CDC_GET_LINE_CODING =			0x21;
static const uint8_t CDC_SET_CONTROL_LINE_STATE =	0x22;

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct USBstringDescriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};

#define USB_MANUFACTURER L"GPL"
#define USB_PRODUCT L"MPGuino"
#define USB_SERIAL_NUMBER L"195"

static const uint16_t USBvendorID = 0x27CD;
static const uint16_t USBproductID = 0x8010;

static const uint8_t PROGMEM USBdeviceDescriptor[] = {
	 18						// bLength
	,1						// bDescriptorType
	,0x00					// bcdUSB
	,0x02
	,2						// bDeviceClass
	,0						// bDeviceSubClass
	,0						// bDeviceProtocol
	,16						// bMaxPacketSize0
	,(USBvendorID & 0xFF)	// idVendor
	,(USBvendorID >> 8)
	,(USBproductID & 0xFF)	// idProduct
	,(USBproductID >> 8)
	,0x00					// bcdDevice
	,0x01
	,1						// iManufacturer
	,2						// iProduct
	,3						// iSerialNumber
	,1						// bNumConfigurations
};

#define CONFIG1_DESC_SIZE (9+9+5+5+4+5+7+9+7+7)
static const uint8_t PROGMEM USBconfigurationDescriptor[CONFIG1_DESC_SIZE] = {
// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	 9 							// bLength;
	,2							// bDescriptorType;
	,(CONFIG1_DESC_SIZE & 0xFF)	// wTotalLength
	,(CONFIG1_DESC_SIZE >> 8)
	,2							// bNumInterfaces
	,1							// bConfigurationValue
	,0							// iConfiguration
	,0xC0						// bmAttributes
	,50							// bMaxPower

// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	,9							// bLength
	,4							// bDescriptorType
	,0							// bInterfaceNumber
	,0							// bAlternateSetting
	,1							// bNumEndpoints
	,0x02						// bInterfaceClass
	,0x02						// bInterfaceSubClass
	,0x01						// bInterfaceProtocol
	,0							// iInterface

// CDC Header Functional Descriptor, CDC Spec 5.2.3.1, Table 26
	,5							// bFunctionLength
	,0x24						// bDescriptorType
	,0x00						// bDescriptorSubtype
	,0x10						// bcdCDC
	,0x01

// Call Management Functional Descriptor, CDC Spec 5.2.3.2, Table 27
	,5							// bFunctionLength
	,0x24						// bDescriptorType
	,0x01						// bDescriptorSubtype
	,0x01						// bmCapabilities
	,1							// bDataInterface

// Abstract Control Management Functional Descriptor, CDC Spec 5.2.3.3, Table 28
	,4							// bFunctionLength
	,0x24						// bDescriptorType
	,0x02						// bDescriptorSubtype
	,0x06						// bmCapabilities

// Union Functional Descriptor, CDC Spec 5.2.3.8, Table 33
	,5							// bFunctionLength
	,0x24						// bDescriptorType
	,0x06						// bDescriptorSubtype
	,0							// bMasterInterface
	,1							// bSlaveInterface0

// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	,7							// bLength
	,5							// bDescriptorType
	,CDC_ACM_ENDPOINT | 0x80	// bEndpointAddress
	,0x03						// bmAttributes (0x03=intr)
	,CDC_ACM_SIZE				// wMaxPacketSize
	,0
	,64							// bInterval

// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	,9							// bLength
	,4							// bDescriptorType
	,1							// bInterfaceNumber
	,0							// bAlternateSetting
	,2							// bNumEndpoints
	,0x0A						// bInterfaceClass
	,0x00						// bInterfaceSubClass
	,0x00						// bInterfaceProtocol
	,0							// iInterface

// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	,7							// bLength
	,5							// bDescriptorType
	,CDC_RX_ENDPOINT			// bEndpointAddress
	,0x02						// bmAttributes (0x02=bulk)
	,CDC_RX_SIZE				// wMaxPacketSize
	,0
	,0							// bInterval

// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	,7							// bLength
	,5							// bDescriptorType
	,CDC_TX_ENDPOINT | 0x80		// bEndpointAddress
	,0x02						// bmAttributes (0x02=bulk)
	,CDC_TX_SIZE				// wMaxPacketSize
	,0
	,0							// bInterval
};

// String descriptors
// All have a common format:
//    [byte] string_length
//   ,[byte] data_type (3 = string)
//   ,UTF-16 encoded string
//
static const struct USBstringDescriptor PROGMEM USBstrLanguage = {
	 4
	,3
	,{0x0409}
};

static const struct USBstringDescriptor PROGMEM USBstrManufacturer = {
	 sizeof(USB_MANUFACTURER)
	,3
	,USB_MANUFACTURER
};

static const struct USBstringDescriptor PROGMEM USBstrProductName = {
	 sizeof(USB_PRODUCT)
	,3
	,USB_PRODUCT
};

static const struct USBstringDescriptor PROGMEM USBstrSerialNumber = {
	 sizeof(USB_SERIAL_NUMBER)
	,3
	,USB_SERIAL_NUMBER
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct endpointDescriptorList {
	uint16_t wValue;
	uint16_t wIndex;
	const uint8_t * addr;
	uint8_t length;
} PROGMEM thisEndpointDescriptorList[] = {
	 {0x0100, 0x0000, USBdeviceDescriptor, sizeof(USBdeviceDescriptor)}
	,{0x0200, 0x0000, USBconfigurationDescriptor, sizeof(USBconfigurationDescriptor)}
	,{0x0300, 0x0000, (const uint8_t *)&USBstrLanguage, 4}
	,{0x0301, 0x0409, (const uint8_t *)&USBstrManufacturer, sizeof(USB_MANUFACTURER)}
	,{0x0302, 0x0409, (const uint8_t *)&USBstrProductName, sizeof(USB_PRODUCT)}
	,{0x0303, 0x0409, (const uint8_t *)&USBstrSerialNumber, sizeof(USB_SERIAL_NUMBER)}
};

#define NUM_DESC_LIST (sizeof(thisEndpointDescriptorList)/sizeof(struct endpointDescriptorList))

#endif // useUSBserial
#endif // defined(__AVR_ATmega32U4__)
