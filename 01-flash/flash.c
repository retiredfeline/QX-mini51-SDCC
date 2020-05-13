// Sample program for the QX-mini51 STC89C52 development board
//
// Flash all 8 LEDs on port 1 once (I think)
// J2 must be closed to supply the LEDs
// Note that the code is not enclosed in a loop which is why I
// have not called this program blink.
//
#include <8051.h>

void delay(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

void main(void)
{
	P1 = 0x00;
	delay(600);
	P1 = 0xff;
	delay(600);
}
