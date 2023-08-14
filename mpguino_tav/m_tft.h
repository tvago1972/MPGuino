#if defined(useTFToutput)
namespace TFT /* TFT hardware support section prototype */
{

	static void init(void);
	static void shutdown(void);

};

interfaceDevice devLCD; // this will be used to emulate LCD output on the TFT screen

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

#define SPI_DATA 11		// pin 17 / PB3 / MOSI
#define SPI_CLOCK 13	// pin 19 / PB5 / SCK
#define SPI_SELECT 10	// pin 16 / PB2 / /SS
#define BRIGHTNESS 

#elif defined(__AVR_ATmega2560__)

#define SPI_DATA 51		// pin 21 / PB2 / MOSI
#define SPI_CLOCK 52	// pin 20 / PB1 / SCK
#define SPI_SELECT 53	// pin 19 / PB0 / /SS
#define BRIGHTNESS 12	// pin 25 / PB6 / OC1B

/*
1 -  Vcc
2 -  GND
3 -  CS			47	//	pin 37	//	PL2			chip select active low
4 -  RESET		44	//	pin 40	//	PL5			active low
5 -  DC			48	//	pin 36	//	PL1			data/command d=1 c=0
6 -  SDI (MOSI)	51	//	pin 21	//	PB2			hardware spi
7 -  SCK		52	//	pin 20	//	PB1			hardware spi
8 -  LED		12	//	pin 25	//	PB6 / OC1B
9 -  SDO (MISO)

10 - T_CLK		11	//	pin 23	//	PB5			software spi
11 - T_CS		10	//	pin 24	//	PB4			
12 - T_DIN		9	//	pin 18	//	PH6	output	software spi
13 - T_DO		8	//	pin 17	//	PH5	input	software spi
14 - T_IRQ		7	//	pin 16	//	PH4			

*/

#elif defined(__AVR_ATmega32U4__)

//#define SPI_DATA xx	// pin 10 / PB2 / MOSI
//#define SPI_CLOCK xx	// pin 9  / PB1 / SCK
//#define SPI_SELECT xx	// pin 8  / PB0 / /SS
//#define BRIGHTNESS 

#endif //
#endif // defined(useTFToutput)
