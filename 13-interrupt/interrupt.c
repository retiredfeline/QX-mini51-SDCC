// Sample program for the QX-mini51 STC89C52 development board
//
// Key triggered interrupt using P3_2 as interrupt source
// S4 level triggers interrupt, reverses LEDs
// If held down LED stays lit
// J2 must be closed to supply LEDs
//
#include <8051.h>

#define	LED	P1

void Key_INT(void) __interrupt(0) __using(1)
{
	LED = ~LED;		// flip LEDs
}

void main(void)
{
	LED = 0xff;		// all LEDs off
	EA = 1;			// enable global interrupts
	EX0 = 1;		// external interrupt 0 enabled
	IT0 = 0;		// level triggered
	while (1);		// other work can be done concurrently here
}
