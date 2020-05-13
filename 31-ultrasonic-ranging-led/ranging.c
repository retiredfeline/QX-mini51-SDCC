// Sample program for the QX-mini51 STC89C52 development board
//
// Use HC-SR04 ultrasonic distance measuring module
// Wiring:
// Module TRIG is connected to P1.2 ECH0 to P1.1
// Gongyang 7-segment display connected to P0, P2.2 ~ P2.0 to digit enable
//
#include <8051.h>

#define	RX	P1_1
#define	TX	P1_2

#define	_nop_()	__asm__("nop")

unsigned int time = 0;
unsigned int timer = 0;
unsigned char posit = 0;
unsigned long S = 0;
char flag = 0;

// 1 bit is segment on
unsigned char const discode[] =
    { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xBF,
	0xff /*-*/
};
unsigned char const positon[3] = { 0xfe, 0xfd, 0xfb };
unsigned char disbuff[4] = { 0, 0, 0, 0, };

// Timer0 interrupt is used for range overflow
void zd0() __interrupt(1)
{
	flag = 1;
}

// multiplex 7-segment display
void Display(void)
{
	if (posit == 0) {
		P0 = (discode[disbuff[posit]]) & 0x7f;
	} else {
		P0 = discode[disbuff[posit]];
	}
	P2 = positon[posit];
	if (++posit >= 3)
		posit = 0;
}

// Timer1 interrupt is used to multiplex display and 800 ms startup delay
void zd3() __interrupt(3)
{
	TH1 = 0xf8;
	TL1 = 0x30;
	Display();
	timer++;
	if (timer >= 400) {
		timer = 0;
		TX = 1;		// 800 ms starts the module once
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
}

void Count(void)
{
	time = TH0 * 256 + TL0;
	TH0 = 0;
	TL0 = 0;

	S = (time * 1.7) / 100;		// calculated as cm
	if ((S >= 700) || flag == 1) {	// display "-" outside range
		flag = 0;
		disbuff[0] = 10;	// "-"
		disbuff[1] = 10;
		disbuff[2] = 10;
	} else {
		disbuff[0] = S % 1000 / 100;
		disbuff[1] = S % 1000 % 100 / 10;
		disbuff[2] = S % 1000 % 10 % 10;
	}
}

void main(void)
{
	TMOD = 0x11;		// timer0 mode 1, gate = 1
	TH0 = 0;
	TL0 = 0;
	TH1 = 0xf8;		// 2 ms timing
	TL1 = 0x30;
	ET0 = 1;		// enable timer0 interrupt
	ET1 = 1;		// enable timer1 interrupt
	TR1 = 1;		// enable timer
	EA = 1;			// enable global interrupts
	while (1) {
		while (!RX);	// wait for RX == 0
		TR0 = 1;	// start counting
		while (RX);	// until RX == 1
		TR0 = 0;	// turn off counting
		Count();
	}
}
