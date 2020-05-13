// Sample program for the QX-mini51 STC89C52 development board
//
// Display HHMM 24-hour clock on 7-segment LED display using multiplexing
// No setting facility, just runs when started
// J6 must be closed to supply the 7-segment LED display
//
#include <8051.h>

#include <typedefs.h>

static uchar second, minute, hour;
uint tcount;
uchar m;

// 0 bit means that segment is on
__code uchar table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};

void t0(void) __interrupt(1) __using(0)
{
	tcount++;

	if (tcount == 4000) {
		tcount = 0;
		second++;
		if (second == 60) {
			second = 0;
			minute++;
			if (minute == 60) {
				minute = 0;
				hour++;
				if (hour == 24) {
					hour = 0;
				}
			}
		}
	}
}

void delay(uchar i)
{
	uchar j, k;

	for (j = i; j > 0; j--)
		for (k = 125; k > 0; k--);
}

void display(void)
{

	P0 = table[minute / 10];
	P2_0 = 0;
	delay(5);
	P2_0 = 1;

	P0 = (table[minute % 10] & 0x7f);
	P2_1 = 0;
	delay(5);
	P2_1 = 1;

	P0 = table[second / 10];
	P2_2 = 0;
	delay(5);
	P2_2 = 1;

	P0 = table[second % 10];
	P2_3 = 0;
	delay(5);
	P2_3 = 1;

}

void main(void)
{
	uint a;

	TMOD = 0x02;
	TH0 = 0x06;
	TL0 = 0x06;
	TR0 = 1;
	ET0 = 1;
	EA = 1;

	while (1) {
		for (a = 100; a > 0; a--) {
			display();
		}
	}
}
