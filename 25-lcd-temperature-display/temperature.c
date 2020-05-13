// Sample program for the QX-mini51 STC89C52 development board
//
// Display temperature on 1602 LCD
// J6 must be open to disable 7-segment LED display
//
#include <8051.h>
#include <stdlib.h>		// for abs()

#include <typedefs.h>

#include <periphs.h>

// 500 µs delay for 11.0592 Mhz crystal
void delay500(void)
{
	uchar i;

	for (i = 230; i > 0; i--);
}

void delayUs(void)
{
	__asm__("nop");
}

void delayMs(uint a)
{
	uint i, j;

	for (i = a; i > 0; i--)
		for (j = 100; j > 0; j--);
}


void writeComm(uint comm)
{
	LCD_RS = 0;
	P2 = comm;
	LCD_EP = 1;
	delayUs();
	LCD_EP = 0;
	delayMs(1);
}

void writeData(uchar dat)
{
	LCD_RS = 1;
	P2 = dat;
	LCD_EP = 1;
	delayUs();
	LCD_EP = 0;
	delayMs(1);
}


void init(void)
{
	LCD_RW = 0;
	writeComm(0x38);
	writeComm(0x0c);
	writeComm(0x06);
	writeComm(0x01);
}

void writeString(uchar * str, uchar length)
{
	uchar i;

	for (i = 0; i < length; i++) {
		writeData(str[i]);
	}
}

// Initialise DS18B20
void dsInit(void)
{

	unsigned int i;

	LCD_RS = 0;
	i = 100;
	while (i > 0)
		i--;
	LCD_RS = 1;
	i = 4;
	while (i > 0)
		i--;
}

void dsWait(void)
{
	unsigned int i;

	while (LCD_RS);
	while (~LCD_RS);
	i = 4;
	while (i > 0)
		i--;
}


uchar readBit(void)
{
	unsigned int i;
	uchar b;

	LCD_RS = 0;
	i++;
	LCD_RS = 1;
	i++;
	i++;
	b = LCD_RS;
	i = 8;
	while (i > 0)
		i--;
	return b != 0;
}

unsigned char readByte(void)
{
	unsigned int i;
	unsigned char j, dat;

	dat = 0;
	for (i = 0; i < 8; i++) {
		j = readBit();
		dat = (j << 7) | (dat >> 1);
	}
	return dat;
}


void writeByte(unsigned char dat)
{
	unsigned int i;
	unsigned char j;
	uchar b;

	for (j = 0; j < 8; j++) {
		b = dat & 0x01;
		dat >>= 1;
		if (b) {
			LCD_RS = 0;
			i++;
			i++;
			LCD_RS = 1;
			i = 8;
			while (i > 0)
				i--;
		} else {
			LCD_RS = 0;
			i = 8;
			while (i > 0)
				i--;
			LCD_RS = 1;
			i++;
			i++;
		}
	}
}

void sendChangeCmd(void)
{
	dsInit();
	dsWait();
	delayMs(1);
	writeByte(0xcc);
	writeByte(0x44);
}

void sendReadCmd(void)
{
	dsInit();
	dsWait();
	delayMs(1);
	writeByte(0xcc);
	writeByte(0xbe);
}


int getTmpValue(void)
{
	unsigned int tmpvalue;
	int value;
	float t;
	unsigned char low, high;

	sendReadCmd();

	low = readByte();
	high = readByte();

	tmpvalue = high;
	tmpvalue <<= 8;
	tmpvalue |= low;
	value = tmpvalue;


	t = value * 0.0625;

	value = t * 100 + (value > 0 ? 0.5 : -0.5);
	return value;
}

void display(int v)
{
	unsigned char count;
	unsigned char datas[] = { 0, 0, 0, 0, 0 };
	unsigned int tmp = abs(v);

	datas[0] = tmp / 10000;
	datas[1] = tmp % 10000 / 1000;
	datas[2] = tmp % 1000 / 100;
	datas[3] = tmp % 100 / 10;
	datas[4] = tmp % 10;
	writeComm(0xc0 + 3);
	if (v < 0) {
		writeString("- ", 2);
	} else {
		writeString("+ ", 2);
	}
	if (datas[0] != 0) {
		writeData('0' + datas[0]);
	}
	for (count = 1; count != 5; count++) {
		writeData('0' + datas[count]);
		if (count == 2) {
			writeData('.');
		}
	}
}

/*****************************DS18B20*******************************/
void main(void)
{
	uchar table[] = " xianzaiwendu:  ";
	int i;			// converted temperatue
	uchar j;

	sendChangeCmd();
	init();
	writeComm(0x80);
	writeString(table, 16);
	while (1) {
		delayMs(1000);	// conversion needs > 750 ms
		writeComm(0xc0);
		i = getTmpValue();
		if (i > 2300) {
			for (j = 200; j > 0; j--) {	// output 1 kHz
				BEEP ^= 1;
				delay500();
			}
			for (j = 200; j > 0; j--) {	// output 500 Hz
				BEEP ^= 1;
				delay500();
				delay500();
			}
		}
		display(i);
		sendChangeCmd();
	}
}
