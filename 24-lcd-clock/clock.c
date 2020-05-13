// Sample program for the QX-mini51 STC89C52 development board
//
// Adjustable time display on 1602 LCD
// S4 selects item to adjust
// S3 increases item
// S2 decreases item
//
// J6 must be open to disable 7-segment LED display
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

uchar count, s1num;
uint miao, shi, fen;		// sec, hour, min
__code uchar table[] = "   2014-4-01";
__code uchar table1[] = "    00:00:00";

void delay(uint z)
{
	uint x, y;

	for (x = z; x > 0; x--)
		for (y = 110; y > 0; y--);
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

void write_date(uchar date)
{
	LCD_RS = 1;
	LCD_EP = 0;
	P2 = date;
	delay(5);
	LCD_EP = 1;
	delay(5);
	LCD_EP = 0;
}

void write_sfm(uchar add, uchar date)
{
	uchar shi, ge;

	shi = date / 10;
	ge = date % 10;
	write_com(0x80 + 0x40 + add);
	write_date(0x30 + shi);
	write_date(0x30 + ge);
}

void timer0(void) __interrupt(1)
{
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	count++;
	if (count == 18) {
		count = 0;
		miao++;
		if (miao == 60) {
			miao = 0;
			fen++;
			if (fen == 60) {
				fen = 0;
				shi++;
				if (shi == 24) {
					shi = 0;
				}
				write_sfm(4, shi);
			}
			write_sfm(7, fen);
		}
		write_sfm(10, miao);
	}
}

void init()
{
	uchar num;

	LCD_RW = 0;
	LCD_EP = 0;
//    min=59;
//    sec=53;
//    hour=23;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	write_com(0x80);
	for (num = 0; num < 15; num++) {
		write_date(table[num]);
		delay(5);
	}
	write_com(0x80 + 0x40);
	for (num = 0; num < 12; num++) {
		write_date(table1[num]);
		delay(5);
	}
	TMOD = 0x01;
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

void keyscan(void)
{
	if (K4 == 0) {
		delay(5);
		if (K4 == 0) {
			s1num++;
			while (!K4);
			if (s1num == 1) {
				TR0 = 0;
				write_com(0x80 + 0x40 + 10);
				write_com(0x0f);
			}
		}
		if (s1num == 2) {
			write_com(0x80 + 0x40 + 7);
		}
		if (s1num == 3) {
			write_com(0x80 + 0x40 + 4);
		}
		if (s1num == 4) {
			s1num = 0;
			write_com(0x0c);
			TR0 = 1;
		}
	}
	if (s1num != 0) {
		if (K3 == 0) {
			delay(5);
			if (K3 == 0) {
				while (!K3);
				if (s1num == 1) {
					miao++;
					if (miao == 60)
						miao = 0;
					write_sfm(10, miao);
					write_com(0x80 + 0x40 + 10);
				}
				if (s1num == 2) {
					fen++;
					if (fen == 60)
						fen = 0;
					write_sfm(7, fen);
					write_com(0x80 + 0x40 + 7);
				}
				if (s1num == 3) {
					shi++;
					if (shi == 24)
						shi = 0;
					write_sfm(4, shi);
					write_com(0x80 + 0x40 + 4);
				}
			}
		}
		if (K2 == 0) {
			delay(5);
			if (K2 == 0) {
				while (!K2);
				if (s1num == 1) {
					/*  if(miao==0)
					   {
					   miao=59;
					   write_sfm(10,miao);
					   write_com(0x80+0x40+10);
					   } */
					miao--;
					if (miao == -1)
						miao = 59;
					write_sfm(10, miao);
					write_com(0x80 + 0x40 + 10);
				}
				if (s1num == 2) {
					fen--;
					if (fen == -1)
						fen = 59;
					write_sfm(7, fen);
					write_com(0x80 + 0x40 + 7);
				}
				if (s1num == 3) {
					shi--;
					if (shi == -1)
						shi = 23;
					write_sfm(4, shi);
					write_com(0x80 + 0x40 + 4);
				}
			}
		}
	}
}

void main(void)
{
	init();
	while (1) {
		keyscan();

	}
//  while(1);
}
