// Sample program for the QX-mini51 STC89C52 development board
//
// Bluetooth LED display and LED indicator
// HC06 connection:
// RXD TXD
// TXD RXD
// Please refer to the pictures in directory for the Bluetooth connection method
// Android's own Bluetooth communication software has been modified
// Please refer to picture in directory for the installation of Bluetooth module
// Bluetooth module: http://item.taobao.com/item.htm?id=27378740053
//

#include <8051.h>

#include <typedefs.h>

#define	led1	P1_0
#define	led2	P1_1
#define	led3	P1_2
#define	led7	P1_7

uchar tmp;
uint c = 0;

__code uchar table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};

void delay1(uint z)		//1ms
{
	uint x, y;

	for (x = z; x > 0; x--)
		for (y = 120; y > 0; y--);
}

void display(uchar value)
{
	uchar shu1, value1;

	value1 = value - 48;
	// bai=value1/16;
	shu1 = value1 % 16;
	P0 = table[shu1];
	P2_0 = 0;
	delay1(1);
	P2_0 = 1;
}

// Serial port initialisation
void init()
{
	ES = 0;			// interrupt off
	SCON = 0x50;		// enable serial, mode 1


	TMOD = 0x20;		// timer 1 mode 2 for serial clock
	TH1 = TL1 = 0xFD;	// 9600 baud (11.0592MHz crystal)

	PCON &= 0x7f;		// baud rate not doubled
	TR1 = 1;		// enable timer 1

	TI = 0;			// send flag 0
	RI = 0;			// receive flag 0

	ES = 1;
}

void send(unsigned char a)
{
// Note: If the microcontroller TXD (P3.1) has no pull-up capability, a pull-up
// resistor must be connected to P3.1. This test requires a pull-up resistor
	TI = 0;
	SBUF = a;
	while (TI == 0);
	TI = 0;

	if (c % 2)		// send indicator, flash for every character
		led7 = 1;
	else
		led7 = 0;
	c++;
}

void ctrl(void)
{
	switch (tmp) {
	case '1':
		led1 = 1;
		send(tmp);
		break;
	case '2':
		led2 = 1;
		send(tmp);
		break;
	case '3':
		led1 = 0;
		led2 = 0;
		send(tmp);
		break;
	case '4':
		led1 = 1;
		led2 = 1;
		send(tmp);
		break;
	case '5':
		led3 = 0;
		send(tmp);
		break;
	default:
		led1 = 1;
		led2 = 1;
		led3 = 1;
		send(tmp);
	}
}

void main()
{
	init();
	while (1) {
		display(SBUF);
		if (RI == 1)	// data received?
		{
			RI = 0;
			ctrl();
		}
	}
}
