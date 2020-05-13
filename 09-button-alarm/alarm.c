// Sample program for the QX-mini51 STC89C52 development board
//
// S1 starts di-di alarm sound, S2 stops it
// J1 must be closed to supply the buzzer
//
#include <8051.h>

#include <periphs.h>

#include <typedefs.h>

uchar Count1, Count2;
uchar flag;

void Time0(void) __interrupt(1) __using(0)
{
	TH0 = 0xfc;
	TL0 = 0x66;		// 1ms timing
	Count1++;
	if (Count1 == 100) {
		Count2++;
		if (Count2 == 4) {
			Count1 = 0;
			Count2 = 0;
			flag = !flag;
		}
	}
	if (!flag) {
		BEEP ^= 1;
	}

}

void main(void)
{
	P0 = 0X0;
	Count1 = 0x00;
	Count2 = 0x00;
	BEEP = 1;
	flag = 0;
	TMOD = 0x01;
	TH0 = 0xfc;
	TL0 = 0x66;		// 1ms timing
	EA = 1;
	ET0 = 1;

	while (1) {
		if (K1 == 0) {
			while (K1 == 0);
			TR0 = 1;
		}
		if (K2 == 0) {
			while (K2 == 0);
			TR0 = 0;
		}
	}
}
