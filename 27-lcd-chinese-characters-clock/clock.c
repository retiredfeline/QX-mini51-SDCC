// Sample program for the QX-mini51 STC89C52 development board
//
// Chinese character clock
// S1 selects unit to adjust
// S2 increases
// S3 decreases
// S4 sets
//
// J6 must be open to disable 7-segment LED display
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

uchar num, key1num, count;
signed char hour, min, sec;

// YMD in Chinese
__code uchar cg_1602[] = { 0x08, 0x0f, 0x12, 0x0f, 0x0a, 0x1f, 0x02, 0x02,
	0x0f, 0x09, 0x0f, 0x09, 0x0f, 0x09, 0x11, 0x00,
	0x0f, 0x09, 0x09, 0x0f, 0x09, 0x09, 0x0f, 0x00
};

void delay(uint xms)
{
	uint i, j;

	for (i = xms; i > 0; i--)
		for (j = 114; j > 0; j--);
}

void write_com(uint com)
{
	LCD_RS = 0;
	LCD_EP = 0;
	P2 = com;
	delay(5);
	LCD_EP = 1;
	delay(5);
	LCD_EP = 0;
}

void write_data(uchar date)
{
	LCD_RS = 1;
	LCD_EP = 0;
	P2 = date;
	delay(5);
	LCD_EP = 1;
	delay(5);
	LCD_EP = 0;
}

void write_by_xy(uchar x, uchar y)
{
	uchar address;

	if (y == 0)
		address = 0x80 + x;
	else
		address = 0xc0 + x;
	write_com(address);
}

void write_string(uchar x, uchar y, uchar * s)
{
	write_by_xy(x, y);
	while (*s) {
		P2 = *s;
		write_data(*s);
		s++;
	}
}

void write_sfm(uchar add, uchar date)
{
	uchar hour, ge;

	hour = date / 10;
	ge = date % 10;
	write_com(0x80 + 0x40 + add);
	write_data(0x30 + hour);
	write_data(0x30 + ge);
}

// Write Chinese characters and symbols
void write_cg(void)
{
	write_com(0x40);
	for (num = 0; num < 24; num++) {	// write custom font to CGRAM
		write_data(cg_1602[num]);
	}
	write_com(0x84);
	{
		write_data(0);
	}
	write_com(0x87);
	{
		write_data(1);
	}
	write_com(0x8a);
	{
		write_data(2);
	}
	write_string(2, 1, ":");
	write_string(5, 1, ":");
	write_string(9, 1, "QX-MCU");
}

void init_1602(void)
{
	LCD_RW = 0;
	hour = 22;
	min = 33;
	sec = 55;
	key1num = 0;
	count = 0;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	write_string(0, 0, "2014");
	write_string(5, 0, "07");
	write_string(8, 0, "12");
	write_sfm(0, hour);
	write_sfm(3, min);
	write_sfm(6, sec);
	write_cg();
	TMOD = 0x01;
	TH0 = (65536 - 50000) / 256;	// timer initial value
	TL0 = (65536 - 50000) % 256;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

void keyscan()
{
	K4 = 0;
	if (K1 == 0) {
		delay(5);
		if (K1 == 0) {			// S1 pressed?
			key1num++;
			while (!K1);		// wait for release
			if (key1num == 1) {
				TR0 = 0;	// timer off
				write_com(0x80 + 0x40 + 7);	// cursor to position 2
				write_com(0x0f);	// blink cursor
			}
			if (key1num == 2) {	// cursor to second position
				write_com(0x80 + 0x40 + 4);
			}
			if (key1num == 3) {	// cursor to hour position
				write_com(0x80 + 0x40 + 1);
			}
			if (key1num == 4) {
				key1num = 0;	// zero key count
				write_com(0x0c);	// stop blink
				TR0 = 1;	// restart timer
			}
		}
	}
	// increment and decrement keys only active after function key
	if (key1num != 0) {
		if (K2 == 0) {			// increment
			delay(5);
			if (K2 == 0) {
				while (!K2);	// wait for release
				if (key1num == 1) {	// adjust seconds
					sec++;
					if (sec == 60)
						sec = 0;
					write_sfm(6, sec);
					write_com(0x80 + 0x40 + 7);
				}
				if (key1num == 2) {	// adjust minutes
					min++;
					if (min == 60)
						min = 0;
					write_sfm(3, min);
					write_com(0x80 + 0x40 + 4);
				}
				if (key1num == 3) {	// adjust hours
					hour++;
					if (hour == 24)
						hour = 0;
					write_sfm(0, hour);;
					write_com(0x80 + 0x40 + 1);
				}
			}
		}
		if (K3 == 0) {			// decrement
			delay(5);
			if (K3 == 0) {
				while (!K3);	// wait for release
				if (key1num == 1) {
					sec--;		// adjust seconds
					if (sec == -1)
						sec = 59;
					write_sfm(6, sec);
					write_com(0x80 + 0x40 + 7);
				}
				if (key1num == 2) {
					min--;		// adjust minutes
					if (min == -1)
						min = 59;
					write_sfm(3, min);
					write_com(0x80 + 0x40 + 4);
				}
				if (key1num == 3) {
					hour--;		// adjust hours
					if (hour == -1)
						hour = 23;
					write_sfm(0, hour);
					write_com(0x80 + 0x40 + 1);
				}
			}
		}
	}
}

void timer0(void) __interrupt(1)
{
	TH0 = (65536 - 50000) / 256;	// reload initial values
	TL0 = (65536 - 50000) % 256;
	count++;
	if (count == 20) {		// 20 * 50 ms = 1 s
		count = 0;
		sec++;			// increment time
		if (sec == 60) {
			sec = 0;
			min++;
			if (min == 60) {
				min = 0;
				hour++;
				if (hour == 24) {
					hour = 0;
				}
				write_sfm(0, hour);
			}
			write_sfm(3, min);
		}
		write_sfm(6, sec);
	}
}

void main(void)
{
	LCD_RW = 0;
	init_1602();
	while (1) {
		keyscan();
	}
}
