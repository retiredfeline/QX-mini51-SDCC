// Sample program for the QX-mini51 STC89C52 development board
//
// Use single wire 18B29 temperature sensor and send temperature to serial port
// Take care to insert it correctly or it will be destroyed.
// Settings: 9600 baud, no parity, 1 stop bit
// Display integer temperature value
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

uchar tempL = 0;
uchar tempH = 0;
uint sdata;			// integer temperature
uchar xiaoshu1;			// 1st decimal place
uchar xiaoshu2;			// 2nd decimal place
uchar xiaoshu;			// 2 decimal places
uchar fg = 1;			// positive or negative flag

void Initial_com(void)
{
	SCON = 0x50;		// mode1 enable receive
	TMOD |= 0x20;		// set baud rate
	TH1 = 0xFD;
	TR1 = 1;
	TI = 1;			// enable serial port interrupt
}

void delay(uint i)
{
	while (i--);		// roughly i * 8 + 10 µs
}

void delay1ms(uint i)
{
	for (i = 124; i > 0; i--);	// delay 124 * 8 + 10 = 1002 µs
}

void Init_DS18B20(void)
{
	uchar x = 0;

	DQ = 1;			// set high first
	delay(8);
	DQ = 0;			// reset pulse
	delay(80);		// delay > 480 µs
	DQ = 1;			// pull up data line
	delay(5);		// wait 15 ~ 60 µs
	x = DQ;			// if 18B20 exists x == 0, else 1
	delay(20);
}

// pull data line low for > 1 µs, theh raise data line to read
uchar ReadOneChar(void)
{
	uchar i = 0;		// shortest duration is 60 µs and > 1 µs between cycles
	uchar dat = 0;

	for (i = 8; i > 0; i--) {
		DQ = 1;
		delay(1);
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if (DQ)
			dat |= 0x80;
		delay(4);
	}
	return (dat);
}

// pull data line low, then send bit within 15 µs
void WriteOneChar(uchar dat)
{
	uchar i = 0;		// sample data line between 15 ~ 60 µs

	for (i = 8; i > 0; i--) {
		DQ = 0;
		DQ = dat & 0x01;
		delay(5);	// > 1 µs between writes
		DQ = 1;
		dat >>= 1;
	}
	delay(4);
}

void ReadTemperature(void)
{
	Init_DS18B20();		// initialise
	WriteOneChar(0xcc);	// skip serial number
	WriteOneChar(0x44);	// start temperature conversion
	delay(125);
	Init_DS18B20();		// initialise
	WriteOneChar(0xcc);	// skip serial number
	WriteOneChar(0xbe);	// read temperature register
	tempL = ReadOneChar();	// LSB of temperature
	tempH = ReadOneChar();	// MSB of temperature
	if (tempH > 0x7f) {	// negative if high bit is 1
		tempL = ~tempL;	// make positive
		tempH = ~tempH + 1;
		fg = 0;		// and set minus indicator
	}
	sdata = tempL / 16 + tempH * 16;	// integer part
	xiaoshu1 = (tempL & 0x0f) * 10 / 16;	// 1st decimal place
	xiaoshu2 = (tempL & 0x0f) * 100 / 16 % 10;	// 2nd decimal place
	xiaoshu = xiaoshu1 * 10 + xiaoshu2;	// 2 decimal places
}

void main(void)
{
	Initial_com();
	while (1) {
		ReadTemperature();
		SBUF = sdata;
		delay1ms(50000);
		delay1ms(50000);
		delay1ms(50000);
		delay1ms(50000);
		SBUF = xiaoshu;
		delay1ms(50000);
		delay1ms(50000);
		delay1ms(50000);
		delay1ms(50000);
	}
}
