// Sample program for the QX-mini51 STC89C52 development board
//
// Simulate fire truck alarm
// S1 starts sound, S2 stops sound
// J1 must be closed to supply the buzzer
//
#include <8051.h>

#include <periphs.h>

#include <typedefs.h>

uchar Count;

void Time0(void) __interrupt(1) __using(0)
{
	TH0 = 0xfe;
	TL0 = Count;
	BEEP ^= 1;
}

void delay(void)
{
	uint k;

	for (k = 1700; k > 0; k--);
}

void key_control(void)
{

	if (K1 == 0) {
		while (K1 == 0);
		TR0 = 1;
	}
	if (K2 == 0) {
		while (K2 == 0);
		TR0 = 0;
	}
}

void main(void)
{
	P0 = 0x0;
	P1 = 0xff;
	P2 = 0xff;
	Count = 0x00;
	BEEP = 1;

	TMOD = 0x01;
	TH0 = 0x00;
	TL0 = 0xff;
	EA = 1;
	ET0 = 1;

	while (1) {
		do {
			Count++;
			key_control();
			delay();
		} while (Count != 0xff);

		do {
			Count--;
			key_control();
			delay();
		} while (Count != 0x00);
	}
}
