// Sample program for the QX-mini51 STC89C52 development board
//
// Use serial port to echo data from host computer
// Settings: 9600 baud, no parity, 1 stop bit
//
#include <8051.h>

#include <typedefs.h>

uchar num, a;

void ser(void) __interrupt(4)
{
	RI = 0;
	P2 = SBUF;		// read input buffer
	a = SBUF;
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
	ES = 1;
	while (1) {
		if (num == 1) {	// has data arrived?
			ES = 0;
			num = 0;
			SBUF = a;	// echo data to host
			while (!TI);
			TI = 0;
			ES = 1;
		}
	}
}
