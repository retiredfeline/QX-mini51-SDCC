// Sample program for the QX-mini51 STC89C52 development board
//
// Ultrasonic ranging
/*
SMC1602A (16 * 2) analog port wiring method
Connection diagram:
       -----------------------------------------------------
       | LCM ----- 51   | LCM ----- 51   | LCM ------ 51   |
       ----------------------------------------------------|
       | DB0 ----- P2.0 | DB4 ----- P2.4 | RW ------- P0.6 |
       | DB1 ----- P2.1 | DB5 ----- P2.5 | RS ------- P0.7 |
       | DB2 ----- P2.2 | DB6 ----- P2.6 | E -------- P0.5 |
       | DB3 ----- P2.3 | DB7 ----- P2.7 | Connect VLCD with 1K resistor to GND |
       -------------------------------------------------- -
Wiring: Module TRIG connected to P1.2 ECH0 connected to P1.1

The source code of this program is for learning reference only and should not be used for commercial purposes. Please contact the author if necessary.

[Note: AT89x51 uses 12M or 11.0592M crystal, actual measurement uses 11.0592M]
=============================================================*/

#include <8051.h>

#include <periphs.h>

#define	RX		P1_1
#define	TX		P1_2
#define	LCM_Data	P2

#define	Busy		0x80

#define	_nop_()		__asm__("nop")

__code unsigned char mcustudio[] = " WWW.QXMCU.COM  ";
__code unsigned char email[] = "863888999@QQ.COM";
__code unsigned char Cls[] = "                ";
__code unsigned char ASCII[15] = "0123456789.-M";

static unsigned char DisNum = 0;	// display pointer
unsigned int time = 0;
unsigned long S = 0;
unsigned char flag = 0;
unsigned char disbuff[4] = { 0, 0, 0, 0, };

void zd0(void) __interrupt(1)
{
	flag = 1;
}

void Delay5Ms(void)
{
	unsigned int TempCyc = 5552;

	while (TempCyc--);
}

void Delay400Ms(void)
{
	unsigned char TempCycA = 5;
	unsigned int TempCycB;

	while (TempCycA--) {
		TempCycB = 7269;
		while (TempCycB--);
	};
}

// Read status
unsigned char ReadStatusLCM(void)
{
	LCM_Data = 0xFF;
	LCD_RS = 0;
	LCD_RW = 1;
	LCD_EP = 0;
	LCD_EP = 0;
	LCD_EP = 1;
	while (LCM_Data & Busy);// detect busy
	return (LCM_Data);
}

// Write data
void WriteDataLCM(unsigned char WDLCM)
{
	ReadStatusLCM();	// check busy
	LCM_Data = WDLCM;
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_EP = 0;		// add delay here if crystal speed too high
	LCD_EP = 0;		// delay
	LCD_EP = 1;
}

// Write command
void WriteCommandLCM(unsigned char WCLCM, unsigned char BuysC)
{
	if (BuysC)		// check busy if needed
		ReadStatusLCM();
	LCM_Data = WCLCM;
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_EP = 0;
	LCD_EP = 0;
	LCD_EP = 1;
}

// Read data
unsigned char ReadDataLCM(void)
{
	LCD_RS = 1;
	LCD_RW = 1;
	LCD_EP = 0;
	LCD_EP = 0;
	LCD_EP = 1;
	return (LCM_Data);
}

// LCM initialisation
void LCMInit(void)
{
	LCM_Data = 0;
	WriteCommandLCM(0x38, 0);	// 3 display mode setting, no busy
	Delay5Ms();			
	WriteCommandLCM(0x38, 0);
	Delay5Ms();			
	WriteCommandLCM(0x38, 0);
	Delay5Ms();
	WriteCommandLCM(0x38, 1);	// display mode setting, detect busy
	WriteCommandLCM(0x08, 1);	// disable display
	WriteCommandLCM(0x01, 1);	// clear display
	WriteCommandLCM(0x06, 1);	// display cursor setting
	WriteCommandLCM(0x0F, 1);	// enable display and cursor
}

// Display chracter at indicated position
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	Y &= 0x1;			// Y <= 1
	X &= 0xF;			// X <= 15
	if (Y)				// + 0x40 for second line
		X |= 0x40;
	X |= 0x80;			// set code
	WriteCommandLCM(X, 1);		// send command
	WriteDataLCM(DData);		// send data
}

// Display string at position
void DisplayListChar(unsigned char X, unsigned char Y,
		     __code unsigned char *DData)
{
	unsigned char ListLength;

	ListLength = 0;
	Y &= 0x1;			// Y <= 1
	X &= 0xF;			// X <= 15
	while (DData[ListLength] > 0x19) {
		if (X <= 0xF) {
			DisplayOneChar(X, Y, DData[ListLength]);
			ListLength++;
			X++;
		}
	}
}

void Count(void)
{
	time = TH0 * 256 + TL0;
	TH0 = 0;
	TL0 = 0;

	S = (time * 1.7) / 100;		// calculated as cm
	if ((S >= 700) || flag == 1) {	// "-" displayed for out of range
		flag = 0;

		DisplayOneChar(0, 1, ASCII[11]);
		DisplayOneChar(1, 1, ASCII[10]);	// decimal point
		DisplayOneChar(2, 1, ASCII[11]);
		DisplayOneChar(3, 1, ASCII[11]);
		DisplayOneChar(4, 1, ASCII[12]);
	} else {
		disbuff[0] = S % 1000 / 100;
		disbuff[1] = S % 1000 % 100 / 10;
		disbuff[2] = S % 1000 % 10 % 10;
		DisplayOneChar(0, 1, ASCII[disbuff[0]]);
		DisplayOneChar(1, 1, ASCII[10]);	// decimal point
		DisplayOneChar(2, 1, ASCII[disbuff[1]]);
		DisplayOneChar(3, 1, ASCII[disbuff[2]]);
		DisplayOneChar(4, 1, ASCII[12]);	// display M
	}
}

void StartModule()
{
	TX = 1;		// start the module
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

void main(void)
{
	unsigned char TempCyc;

	Delay400Ms();		// wait for LCM ready
	LCMInit();		// LCM initialisation
	Delay5Ms();
	DisplayListChar(0, 0, mcustudio);
	DisplayListChar(0, 1, email);
	ReadDataLCM();		// discard data
	for (TempCyc = 0; TempCyc < 10; TempCyc++)
		Delay400Ms();
	DisplayListChar(0, 1, Cls);
	while (1) {
		TMOD = 0x01;	// timer0 mode1, gate = 1
		TH0 = 0;
		TL0 = 0;
		ET0 = 1;	// enable timer0 interrupt
		EA = 1;		// enable global interrupts

		while (1) {
			StartModule();
			while (!RX);	// wait unti RX = 0
			TR0 = 1;
			while (RX);	// when RX = 1, count and wait
			TR0 = 0;
			Count();	// calculate
			delayms(80);

		}
	}
}
