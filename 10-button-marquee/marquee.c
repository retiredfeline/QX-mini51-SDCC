// Sample program for the QX-mini51 STC89C52 development board
//
// S1 makes the marquee flash once, S2 twice, S3 thrice and S4 four times
// J2 must be closed to supply the LEDs
//
#include <8051.h>

#include <periphs.h>

#include <typedefs.h>

void delay(uint z)
{
	uint i, j;

	for (i = z; i > 0; i--)
		for (j = 110; j > 0; j--);
}

void main(void)
{
	P0 = 0;

	while (1) {
		if (K1 == 0) {
			delay(5);	// debounce
			if (K1 == 0)	// confirm press
			{
				while (!K1)	// release detection
					;
				P1 = 0;
				delay(200);
				P1 = 0XFF;
			}

		}
		if (K2 == 0) {
			delay(5);	// debounce
			if (K2 == 0)	// confirm press
			{
				while (!K2)	// release detection
					;
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
				P1 = 0;
				delay(200);
				P1 = 0XFF;
			}
		}
		if (K3 == 0) {
			delay(5);	// debounce
			if (K3 == 0)	// confirm press
			{
				while (!K3);	// release detection
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
			}
		}
		if (K4 == 0) {
			delay(5);	// debounce
			if (K4 == 0)	// confirm press
			{
				while (!K4)	// release detection
					;
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
				P1 = 0;
				delay(200);
				P1 = 0XFF;
				delay(200);
			}
		}
		delay(10);
	}
}
