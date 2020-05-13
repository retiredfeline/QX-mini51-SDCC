// Sample program for the QX-mini51 STC89C52 development board
//
// Display text on 1602 LCD
// Connect LCD with display facing outwards
// J6 must be open to disable 7-segment LCD display, it cannot be shared
// RV1 may need to be adjusted for best contrast
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

#define	_nop_()	__asm__("nop")

__code uchar dis1[] = "      QXMCU     ";
__code uchar dis2[] = "http://qxmcu.com";

void delay(uchar m)
{
	uchar i, j;

	for (i = 0; i < m; i++)
		for (j = 0; j < 253; j++);
}

uchar lcd_bz(void)
{
	uchar result;

	LCD_RS = 0;		// read busy signal
	LCD_RW = 1;
	LCD_EP = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	result = P2 & 0x80;
	LCD_EP = 0;
	return result;
}

void lcd_wcmd(uchar cmd)
{
	while (lcd_bz());
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_EP = 0;
	_nop_();
	_nop_();
	P2 = cmd;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LCD_EP = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LCD_EP = 0;
}

void lcd_pos(uchar pos)
{
	lcd_wcmd(pos | 0x80);
}

void lcd_wdat(uchar dat)
{
	while (lcd_bz());
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_EP = 0;
	_nop_();
	_nop_();
	P2 = dat;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LCD_EP = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LCD_EP = 0;
}

void lcd_init(void)
{
	lcd_wcmd(0x38);
	delay(1);
	lcd_wcmd(0x0c);
	delay(1);
	lcd_wcmd(0x06);
	delay(1);
	lcd_wcmd(0x01);
	delay(1);
}

void main(void)
{
	uchar i;

	lcd_init();
	delay(10);

	lcd_pos(0);
	i = 0;
	while (dis1[i] != '\0') {
		lcd_wdat(dis1[i]);
		i++;
	}
	lcd_pos(0x40);
	i = 0;
	while (dis2[i] != '\0') {
		lcd_wdat(dis2[i]);
		i++;
	}
	while (1);
}
