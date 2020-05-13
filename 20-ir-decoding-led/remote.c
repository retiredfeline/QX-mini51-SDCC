// Sample program for the QX-mini51 STC89C52 development board
//
// Display infrared code 0-9
// Pay attention to the direction of the infrared receiver
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

#define	_nop_()		__asm__("nop")

__code uchar table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};

uchar IRCOM[7];
uchar m;

void delay(uchar x)
{
	uchar i;

	while (x--) {		// delay x * 0.14 ms
		for (i = 0; i < 13; i++) {
		}
	}
}

void IR_IN(void) __interrupt(0) __using(0)
{
	uchar j, k, N = 0;
	uchar q = 0;

	EX0 = 0;
	delay(15);
	if (IRIN == 1) {
		EX0 = 1;
		return;
	}
	while (!IRIN)		// Wait for IR signal
		delay(1);	// wait 9 ms
	for (j = 0; j < 4; j++) {	// collect 4 octets
		for (k = 0; k < 8; k++) {
			while (IRIN) {
				delay(1);	// wait 4.5 ms
			}
			while (!IRIN)	// wait for IR high
				delay(1);
			while (IRIN) {	// calculate duration of high
				delay(1);
				N++;
				if (N >= 30) {
					EX0 = 1;
					return;
				}	// leave 0.14 ms when overflow
			}
			IRCOM[j] = IRCOM[j] >> 1;	// shift in 0
			if (N >= 8) {
				IRCOM[j] = IRCOM[j] | 0x80;	// shift in 1
			}
			N = 0;
		}
	}
	if (IRCOM[2] != ~IRCOM[3]) {
		EX0 = 1;
		return;
	}
	IRCOM[5] = IRCOM[2] & 0x0F;	// take low nybble
	IRCOM[6] = IRCOM[2] >> 4;	// shift right high nybble
	if (IRCOM[5] > 9) {
		IRCOM[5] = IRCOM[5] + 0x37;
	} else
		IRCOM[5] = IRCOM[5] + 0x30;
	if (IRCOM[6] > 9) {
		IRCOM[6] = IRCOM[6] + 0x37;
	} else
		IRCOM[6] = IRCOM[6] + 0x30;
	q = (((IRCOM[6] & 0x0f) << 4) + (IRCOM[5] & 0x0f));
	// determine key code
	switch (q) {
	case 0x16:
		m = 0;
		break;
	case 0x03:
		m = 1;
		break;
	case 0x18:
		m = 2;
		break;
	case 0x55:
		m = 3;
		break;
	case 0x08:
		m = 4;
		break;
	case 0x13:
		m = 5;
		break;
	case 0x51:
		m = 6;
		break;
	case 0x42:
		m = 7;
		break;
	case 0x52:
		m = 8;
		break;
	case 0x41:
		m = 9;
		break;
	default:
		break;
	}
	EX0 = 1;
}

void display(void)
{

	P0 = table[m];
	P2_3 = 0;
	delay(5);
	P2_3 = 1;

}

void delay1(int ms)
{
	uchar y;

	while (ms--) {
		for (y = 0; y < 250; y++) {
			_nop_();
			_nop_();
			_nop_();
			_nop_();
		}
	}
}

void delay1ms(void)
{
	uint i;			// delay 124 * 8 + 10 = 1020 µs

	for (i = 8024; i > 0; i--);
}

void main(void)
{
	uint a;

	IRIN = 1;		// initialise I/O port
	IE = 0x81;		// enable global interrupts and INT0 interrupt
	TCON = TCON | 0x01;	// negative edge trigger
	while (1) {
		for (a = 100; a > 0; a--) {
			display();
		}
	}
}
