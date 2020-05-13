// Sample program for the QX-mini51 STC89C52 development board
//
// Use HC-SR04 ultrasonic distance measuring module
// Wiring:
// Module TRIG is connected to P1.2 ECH0 to P1.1
// Serial 9600 baud, no parity, 1 stop bit
//
#include <8051.h>
#include <stdio.h>

#include <typedefs.h>

#define	RX	P1_1
#define	TX	P1_2

#define	_nop_()	__asm__("nop")

unsigned int time = 0;
unsigned int timer = 0;
float S = 0;
char flag = 0;

// Timer0 interrupt is used for range overflow
void zd0(void) __interrupt(1)
{
	flag = 1;
}

int putchar(int c)
{
	while (!TI);		/* wait until transmitter ready */
	TI = 0;
	SBUF = c;		/* output character */
	return (c);
}

void Count(void)
{
	time = TH0 * 256 + TL0;
	TH0 = 0;
	TL0 = 0;
	S = (time * 1.87) / 100;	// in cm for 11.0592 MHz crystal
	if (flag == 1) {		// overflow
		flag = 0;
		printf("-----\n");
	}
	printf("S=%f\n", S);
}

void delayms(unsigned int ms)
{
	unsigned char i = 100, j;

	for (; ms; ms--) {
		while (--i) {
			j = 10;
			while (--j);
		}
	}
}

void StartModule(void)
{
	TX = 1;		// 800 ms starts module once
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	TX = 0;
}

void main(void)
{
	TMOD = 0x21;	// timer0 mode 1, gate = 1
	SCON = 0x50;
	TH1 = 0xFD;
	TL1 = 0xFD;
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
	ET0 = 1;	// enable timer0 interrupt
	TR1 = 1;	// enable timer1
	TI = 1;
	EA = 1;		// enable global interrupts
	while (1) {
		StartModule();	// calculate
		while (!RX);	// wait for RX == 0
		TR0 = 1;	// start counting
		while (RX);	// until RX == 1
		TR0 = 0;	// turn off counting
		Count();
		delayms(100);
	}
}
