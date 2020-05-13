// Sample program for the QX-mini51 STC89C52 development board
//
// Display DHT11 readings on serial port
// Same socket as temperature probe, 3rd DHT11 pin is floating
// Settings: 9600 baud, no parity, 1 stop bit
//
#include <8051.h>

#include <typedefs.h>

typedef unsigned char U8;
typedef signed char S8;
typedef unsigned int U16;
typedef signed int S16;

#define Data_0_time 4

U8 U8FLAG, k;
U8 U8count, U8temp;
U8 U8T_data_H, U8T_data_L, U8RH_data_H, U8RH_data_L, U8checkdata;
U8 U8T_data_H_temp, U8T_data_L_temp, U8RH_data_H_temp, U8RH_data_L_temp,
    U8checkdata_temp;
U8 U8comdata;
U8 outdata[5];
U8 indata[5];
U8 count, count_r = 0;
U8 str[6] = "RS232";
U16 U16temp1, U16temp2;

void SendData(U8 * a)
{
	outdata[0] = a[0];
	outdata[1] = a[1];
	outdata[2] = a[2];
	outdata[3] = a[3];
	outdata[4] = a[4];
	count = 1;
	SBUF = outdata[0];
}

void Delay(U16 j)
{
	U8 i;

	for (; j > 0; j--) {
		for (i = 0; i < 27; i++);
	}
}

void Delay_10us(void)
{
	U8 i;

	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}

void COM(void)
{
	U8 i;

	for (i = 0; i < 8; i++) {
		U8FLAG = 2;

		P2_1 = 0;	//T
		P2_1 = 1;	//T

		while ((!P3_7) && U8FLAG++);
		Delay_10us();
		Delay_10us();

		U8temp = 0;
		if (P3_7)
			U8temp = 1;
		U8FLAG = 2;
		while ((P3_7) && U8FLAG++);

		P2_1 = 0;	//T
		P2_1 = 1;	//T

		if (U8FLAG == 1)
			break;

		U8comdata <<= 1;
		U8comdata |= U8temp;	//0
	}			//rof
}

// Variables:
// U8T_data_H: high 8-bits of temperature
// U8T_data_L: low 8-bits of temperature:
// U8RH_data_H: high 8-bits of humidity
// U8RH_data_L: low 8-bits of humidity
// U8checkdata: checksum
void RH(void)
{
	// host pulled down 18 ms
	P3_7 = 0;
	Delay(180);
	P3_7 = 1;
	// bus pulled high by pull up resistor, host waits 20 µs
	Delay_10us();
	Delay_10us();
	Delay_10us();
	Delay_10us();
	// set input to measure slave
	P3_7 = 1;
	// test if low
	if (!P3_7)		//T !
	{
		U8FLAG = 2;
		// test if low for 80 µs
		while ((!P3_7) && U8FLAG++);
		U8FLAG = 2;
		// test if high for 80 µs
		while ((P3_7) && U8FLAG++);
		// receiving data
		COM();
		U8RH_data_H_temp = U8comdata;
		COM();
		U8RH_data_L_temp = U8comdata;
		COM();
		U8T_data_H_temp = U8comdata;
		COM();
		U8T_data_L_temp = U8comdata;
		COM();
		U8checkdata_temp = U8comdata;
		P3_7 = 1;
		// validate data
		U8temp =
		    (U8T_data_H_temp + U8T_data_L_temp + U8RH_data_H_temp +
		     U8RH_data_L_temp);
		if (U8temp == U8checkdata_temp) {
			U8RH_data_H = U8RH_data_H_temp;
			U8RH_data_L = U8RH_data_L_temp;
			U8T_data_H = U8T_data_H_temp;
			U8T_data_L = U8T_data_L_temp;
			U8checkdata = U8checkdata_temp;
		}		//fi
	}			//fi
}

void RSINTR(void) __interrupt(4) __using(2)
{
	U8 InPut3;

	if (TI == 1) {		// transmit interrupt
		TI = 0;
		if (count != 5) {	// after 5 bytes
			SBUF = outdata[count];
			count++;
		}
	}
	if (RI == 1) {		// receive interrupt
		InPut3 = SBUF;
		indata[count_r] = InPut3;
		count_r++;
		RI = 0;
		if (count_r == 5) {	// after 4 bytes
			// process data
			count_r = 0;
			str[0] = indata[0];
			str[1] = indata[1];
			str[2] = indata[2];
			str[3] = indata[3];
			str[4] = indata[4];
			P0 = 0;
		}
	}
}

void main(void)
{
	TMOD = 0x20;		// timer1 mode 2
	TH1 = 253;		// initial value
	TL1 = 253;
	TR1 = 1;		// start timer
	SCON = 0x50;		// serial mode 1, 9600 baud, receive enabled
	ES = 1;
	EA = 1;			// enable global interrupts
	TI = 0;
	RI = 0;
	SendData(str);		// send to serial port
	Delay(1);		// delay 100 µs
	while (1) {
		// read temperature and humidity
		RH();
		// display to serial port
		str[0] = U8RH_data_H;
		str[1] = U8RH_data_L;
		str[2] = U8T_data_H;
		str[3] = U8T_data_L;
		str[4] = U8checkdata;
		SendData(str);
		// data cycle >= 2 s
		Delay(20000);
	}
}
