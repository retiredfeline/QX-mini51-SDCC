// Sample program for the QX-mini51 STC89C52 development board
//
// Demonstration of pulse width modulation control of LEDs
// S1 increases the brighness, S2 decreases (remember 0 is on for LED port)
// The buzzer sounds at the lowest and highest limits
//
#include <8051.h>

#include <periphs.h>

unsigned char PWM = 0x7f;

void timer0(void) __interrupt(1)
{
	TR1 = 0;
	TH0 = 0xfc;
	TL0 = 0x66;
	TH1 = PWM;
	TR1 = 1;
	P1 = 0x00;
}

void timer1(void) __interrupt(3)
{
	TR1 = 0;
	P1 = 0xff;
}

void delay(unsigned char t)
{
	while (t--);
}

void delayms(unsigned char ms)
{
	unsigned char i;

	while (ms--) {
		for (i = 0; i < 120; i++);
	}
}

void Beep(void)
{
	unsigned char i;

	for (i = 0; i < 100; i++) {
		delay(100);
		BEEP ^= 1;
	}
	BEEP = 1;
	delayms(100);
}

void main(void)
{
	P1 = 0xff;
	TMOD = 0x21;
	TH0 = 0xfc;
	TL0 = 0x66;

	TH1 = PWM;
	TL1 = 0;

	EA = 1;
	ET0 = 1;
	ET1 = 1;

	TR0 = 1;

	while (1) {
		do {
			if (PWM != 0xff) {
				PWM++;
				delayms(10);
			} else
				Beep();
		} while (K2 == 0);
		do {
			if (PWM != 0x02) {
				PWM--;
				delayms(10);
			} else
				Beep();
		} while (K1 == 0);
	}
}
