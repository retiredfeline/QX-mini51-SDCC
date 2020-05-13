// Sample program for the QX-mini51 STC89C52 development board
//
// Buzzer test program
// J1 must be closed to supply the buzzer
//
#include <8051.h>

#include <periphs.h>

#include <typedefs.h>

// Delay 500µs, might need tuning for SDCC generated code
void delay500(void)
{
	uchar i;

	for (i = 230; i > 0; i--);
}

void main(void)
{
	uchar j;

	while (1) {
		for (j = 200; j > 0; j--) {
			BEEP ^= 1;	// 1kHz
			delay500();
		}
		for (j = 200; j > 0; j--) {
			BEEP ^= 1;	// 500Hz
			delay500();
			delay500();
		}
	}
}
