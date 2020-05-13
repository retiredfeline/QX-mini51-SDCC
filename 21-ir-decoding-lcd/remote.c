// Sample program for the QX-mini51 STC89C52 development board
//
// Display key code of IR, e.g. "IR-CODE: 8DH"
// J6 must be open to disable 7-segment LED
//
#include <8051.h>

#include <typedefs.h>

#include <periphs.h>

#define	_nop_()		__asm__("nop")
#define	delayNOP()	__asm__("nop;nop;nop;nop")

__code uchar cdis1[] = " Red Control ";
__code uchar cdis2[] = "  IR-CODE: --H  ";

uchar IRCOM[7];

void delay(unsigned char x)
{
	unsigned char i;

	while (x--) {		// delay x * 0.14 ms
		for (i = 0; i < 13; i++) {
		}
	}
}

void delay1(int ms)
{
	unsigned char y;

	while (ms--) {
		for (y = 0; y < 250; y++) {
			_nop_();
			_nop_();
			_nop_();
			_nop_();
		}
	}
}

// Check LCD busy status
uchar lcd_busy()
{
	uchar result;

	LCD_RS = 0;
	LCD_RW = 1;
	LCD_EP = 1;
	delayNOP();
	result = P2 & 0x80;
	LCD_EP = 0;
	return (result);
}

// Write LCD command, RS = 0, RW = 0, pulse E, D0-D7 = command
void lcd_wcmd(uchar cmd)
{
	while (lcd_busy());
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_EP = 0;
	_nop_();
	_nop_();
	P2 = cmd;
	delayNOP();
	LCD_EP = 1;
	delayNOP();
	LCD_EP = 0;
}

// Write LCD data, RS = 1, RW = 0, pulse E, D0-D7 = data
void lcd_wdat(uchar dat)
{
	while (lcd_busy());
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_EP = 0;
	P2 = dat;
	delayNOP();
	LCD_EP = 1;
	delayNOP();
	LCD_EP = 0;
}

void lcd_init()
{
	delay1(15);
	lcd_wcmd(0x38);		// 16x2, 5x7 dot matrix, 8-bit data
	delay1(5);
	lcd_wcmd(0x38);
	delay1(5);
	lcd_wcmd(0x38);
	delay1(5);

	lcd_wcmd(0x0c);		// display blinking cursor
	delay1(5);
	lcd_wcmd(0x06);		// move cursor
	delay1(5);
	lcd_wcmd(0x01);		// clear LCD
	delay1(5);
}

// Set display position
void lcd_pos(uchar pos)
{
	lcd_wcmd(pos | 0x80);
}

void IR_IN(void) __interrupt(0) __using(0)
{
	unsigned char j, k, N = 0;

	EX0 = 0;
	delay(15);
	if (IRIN == 1) {
		EX0 = 1;
		return;
	}
	while (!IRIN)		// Wait for IR signal
		delay(1);	// wait 9 ms
	for (j = 0; j < 4; j++) {	// collect 4 octets
		for (k = 0; k < 8; k++) {
			while (IRIN) {
				delay(1);	// wait 4.5 ms
			}
			while (!IRIN)	// wait for IR high
				delay(1);
			while (IRIN) {	// calculate duration of high
				delay(1);
				N++;
				if (N >= 30) {
					EX0 = 1;
					return;
				}	// leave 0.14 ms when overflow
			}
			IRCOM[j] = IRCOM[j] >> 1;	// shift in 0
			if (N >= 8) {
				IRCOM[j] = IRCOM[j] | 0x80;	// shift in 1
			}
			N = 0;
		}
	}

	if (IRCOM[2] != ~IRCOM[3]) {
		EX0 = 1;
		return;
	}

	IRCOM[5] = IRCOM[2] & 0x0F;	// take low nybble
	IRCOM[6] = IRCOM[2] >> 4;	// shift right high nybble

	if (IRCOM[5] > 9) {
		IRCOM[5] = IRCOM[5] + 0x37;
	} else
		IRCOM[5] = IRCOM[5] + 0x30;

	if (IRCOM[6] > 9) {
		IRCOM[6] = IRCOM[6] + 0x37;
	} else
		IRCOM[6] = IRCOM[6] + 0x30;

	lcd_pos(0x4b);
	lcd_wdat(IRCOM[6]);
	lcd_pos(0x4c);
	lcd_wdat(IRCOM[5]);
	EX0 = 1;
}

void main(void)
{
	uchar m;

	IRIN = 1;		// I/O port initialisation
	delay1(10);
	lcd_init();		// Initialise LCD
	lcd_pos(0);		// Set position to line 1, char 1
	m = 0;
	while (cdis1[m] != '\0') {
		lcd_wdat(cdis1[m]);	// Display characters
		m++;
	}

	lcd_pos(0x40);		// Set position to line 2, char 1
	m = 0;
	while (cdis2[m] != '\0') {
		lcd_wdat(cdis2[m]);	// Display characters
		m++;
	}

	IE = 0x81;		// Enable global interrupts and INT0 interrupt
	TCON = 0x01;		// Negative edge trigger mode

	while (1);
}
