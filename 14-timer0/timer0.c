// Sample program for the QX-mini51 STC89C52 development board
//
// Use timer0 to flash LEDs
// J2 must be closed to supply the LEDs
//
#include <8051.h>

#define	LED	P1

void Init_Timer0(void)
{
	TMOD |= 0x01;		// mode 1, 16-bit timer
	TH0 = 0x00;		// overflow at 65536
	TL0 = 0x00;
	EA = 1;			// enable global interrupts
	ET0 = 1;		// enable timer0 interrupt
	TR0 = 1;		// timer on
}

void Timer0_isr(void) __interrupt(1) __using(1)
{
	TH0 = 0x00;		// reload
	TL0 = 0x00;
	LED = ~LED;		// flip LED
}

void main(void)
{
	Init_Timer0();
	while (1);
}
