// Sample program for the QX-mini51 STC89C52 development board
//
// Display infrared code 0-9 on serial port
// Settings: 9600 baud, no parity, 1 stop bit
// Pay attention to the direction of the infrared receiver
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

#define	_nop_()		__asm__("nop")
#define	delayNOP()	__asm__("nop;nop;nop;nop")

uchar IRCOM[7];

void delay(uchar x)
{
	uchar i;		// delay x * 0.14 ms

	while (x--) {
		for (i = 0; i < 13; i++);
	}
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

void Initial_com(void)
{
	EA = 1;			// enable global interrupts
	ES = 1;			// enable serial port interrupt
	ET1 = 1;		// enable timer1 interrupt
	TMOD = 0x20;		// timer1 mode2 for baud rate
	PCON = 0x00;		// SMOD = 0
	SCON = 0x50;		// 9600 baud
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;		// start timer1
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
		SBUF = 0x00;
		break;
	case 0x03:
		SBUF = 0x01;
		break;
	case 0x18:
		SBUF = 0x02;
		break;
	case 0x55:
		SBUF = 0x03;
		break;
	case 0x08:
		SBUF = 0x04;
		break;
	case 0x13:
		SBUF = 0x05;
		break;
	case 0x51:
		SBUF = 0x06;
		break;
	case 0x42:
		SBUF = 0x07;
		break;
	case 0x52:
		SBUF = 0x08;
		break;
	case 0x41:
		SBUF = 0x09;
		break;
	default:
		break;
	}
	EX0 = 1;
}

void main(void)
{
	IRIN = 1;		// I/O port initialisation
	Initial_com();
	IE = 0x81;		// enable global interrupts and INT0 interrupt
	TCON = TCON | 0x01;	// negative edge trigger mode
	while (1);
}
