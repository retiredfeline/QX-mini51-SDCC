// Sample program for the QX-mini51 STC89C52 development board
//
// Plays a song called Only Mothers in the World using the buzzer
// J1 must be closed to supply the buzzer
//
#include <8051.h>

#include <periphs.h>

#include <typedefs.h>

uchar timer0h, timer0l, time;

// The note durations
__code uchar sszymmh[] =
	{ 6, 2, 3, 5, 2, 1, 3, 2, 2, 5, 2, 2, 1, 3, 2, 6, 2, 1, 5, 2, 1,
	6, 2, 4, 3, 2, 2, 5, 2, 1, 6, 2, 1, 5, 2, 2, 3, 2, 2, 1, 2, 1,
	6, 1, 1, 5, 2, 1, 3, 2, 1, 2, 2, 4, 2, 2, 3, 3, 2, 1, 5, 2, 2,
	5, 2, 1, 6, 2, 1, 3, 2, 2, 2, 2, 2, 1, 2, 4, 5, 2, 3, 3, 2, 1,
	2, 2, 1, 1, 2, 1, 6, 1, 1, 1, 2, 1, 5, 1, 6, 0, 0, 0
};

// High byte of frequency table
__code uchar FREQH[] = { 0xF2, 0xF3, 0xF5, 0xF5, 0xF6, 0xF7, 0xF8,
	0xF9, 0xF9, 0xFA, 0xFA, 0xFB, 0xFB, 0xFC, 0xFC,
	0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE,
	0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF,
};

// Low byte of frequency table
__code uchar FREQL[] = { 0x42, 0xC1, 0x17, 0xB6, 0xD0, 0xD1, 0xB6,
	0x21, 0xE1, 0x8C, 0xD8, 0x68, 0xE9, 0x5B, 0x8F,
	0xEE, 0x44, 0x6B, 0xB4, 0xF4, 0x2D,
	0x47, 0x77, 0xA2, 0xB6, 0xDA, 0xFA, 0x16,
};

void t0int(void) __interrupt(1)
{
	TR0 = 0;
	BEEP ^= 1;
	TH0 = timer0h;
	TL0 = timer0l;
	TR0 = 1;
}

void delay(uchar t)
{
	uchar t1;
	unsigned int t2;

	for (t1 = 0; t1 < t; t1++) {
		for (t2 = 0; t2 < 8000; t2++);
	}
	TR0 = 0;
}

void song(void)
{
	TH0 = timer0h;
	TL0 = timer0l;
	TR0 = 1;
	delay(time);
}

void main(void)
{
	uchar k, i;

	TMOD = 1;
	EA = 1;
	ET0 = 1;
	while (1) {
		i = 0;
		while (i < 100) {
			k = sszymmh[i] + 7 * sszymmh[i + 1] - 1;
			timer0h = FREQH[k];
			timer0l = FREQL[k];
			time = sszymmh[i + 2];
			i = i + 3;
			song();
		}
	}
}
