// Sample program for the QX-mini51 STC89C52 development board
//
// Use serial port to receive LED instructions from host computer
// Settings: 9600 baud, no parity, 1 stop bit
// Character: LED affected
// 01: LED1 on
// 02: LED2 on
// ..
// 08: LED8 on
//
// J2 must be closed to supply LEDs
//
#include <8051.h>

#include <typedefs.h>

#define LED P1

uchar dat, num;

void ser(void) __interrupt(4)
{
	RI = 0;
	dat = SBUF;		// read received character
	ES = 0;			// acknowledge interrupt
	num = 1;
}

void main(void)
{
	TMOD = 0x20;		// 9600 baud
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;
	REN = 1;		// serial port initialisation
	SM0 = 0;
	SM1 = 1;
	EA = 1;			// enable global interrupts
	ES = 1;			// enable serial port interrupt
	while (1) {
		if (num == 1) {
			switch (dat) {	// light LED according to received data
			case 0:
				LED = 0xff;
				break;
			case 1:
				LED = 0xfe;	// remember that 0 bit turns on LED
				break;
			case 2:
				LED = 0xfd;
				break;
			case 3:
				LED = 0xfb;
				break;
			case 4:
				LED = 0xf7;
				break;
			case 5:
				LED = 0xef;
				break;
			case 6:
				LED = 0xdf;
				break;
			case 7:
				LED = 0xbf;
				break;
			case 8:
				LED = 0x7f;
				break;
			}
			ES = 1;	// reenable serial port interrupt
			num = 0;
		}
	}
}
