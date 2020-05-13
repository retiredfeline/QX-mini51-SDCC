// Sample program for the QX-mini51 STC89C52 development board
//
// Simulate gunshot sound
// S1 triggers
// J1 must be closed to supply the buzzer
//
#include <8051.h>

#include <periphs.h>

#include <typedefs.h>

uchar H_count, L_count;

void Time0(void) __interrupt(1) __using(0)
{
	BEEP ^= 1;
	if (L_count != 0x00) {
		L_count--;
		TH0 = H_count;
		TL0 = L_count;
		return;
	} else
		H_count--;

	if (H_count != 0xfc) {
		L_count--;
		TH0 = H_count;
		TL0 = L_count;
		return;
	} else {
		H_count = 0xff;
		L_count = 0xa0;
		TH0 = H_count;
		TL0 = L_count;
		BEEP = 1;
		TR0 = 0;
	}
}

void main(void)
{
	P0 = 0;
	BEEP = 1;
	TMOD = 0x01;
	TH0 = 0xff;
	TL0 = 0xa0;
	H_count = 0xff;
	L_count = 0xa0;
	EA = 1;
	ET0 = 1;

	while (1) {
		if (K1 == 0)
			TR0 = 1;
		if (K2 == 0)
			TR0 = 1;
		if (K3 == 0)
			TR0 = 1;
		if (K4 == 0)
			TR0 = 1;
	}
}
