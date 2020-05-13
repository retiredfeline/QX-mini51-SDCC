// Sample program for the QX-mini51 STC89C52 development board
//
#include <8051.h>

#include <periphs.h>

#define	WINDOW_XADDR_START	0x0050	// Horizontal Start Address Set
#define	WINDOW_XADDR_END	0x0051	// Horizontal End Address Set
#define	WINDOW_YADDR_START	0x0052	// Vertical Start Address Set
#define	WINDOW_YADDR_END	0x0053	// Vertical End Address Set
#define	GRAM_XADDR		0x0020	// GRAM Horizontal Address Set
#define	GRAM_YADDR		0x0021	// GRAM Vertical Address Set
#define	GRAMWR			0x0022	// memory write

/* LCD color */
#define	White			0xFFFF
#define	Black			0x0000
#define	Blue			0x001F
#define	Blue2			0x051F
#define	Red			0xF800
#define	Magenta			0xF81F
#define	Green			0x07E0
#define	Cyan			0x7FFF
#define	Yellow			0xFFE0

__code unsigned char *pic;

void main(void);
void ILI9325_Initial(void);
void show_colour_bar(void);
void Init_data(unsigned char x, unsigned int y);
void Write_Cmd(unsigned char DH, unsigned char DL);
void Write_Data(unsigned char DH, unsigned char DL);
void delayms(unsigned int tt);
void show_photo(void);
void Show_RGB(unsigned int x0, unsigned int x1, unsigned int y0,
	      unsigned int y1, unsigned int Color);
void Write_Data_U16(unsigned int y);
static void LCD_SetPos(unsigned int x0, unsigned int x1, unsigned int y0,
		       unsigned int y1);
void ClearScreen(unsigned int bColor);
void LCD_PutChar8x16(unsigned short x, unsigned short y, char c,
		     unsigned int fColor, unsigned int bColor);
void LCD_PutChar(unsigned short x, unsigned short y, char c,
		 unsigned int fColor, unsigned int bColor);
void LCD_PutString(unsigned short x, unsigned short y, char *s,
		   unsigned int fColor, unsigned int bColor);

void LCD_PutChar8x8(unsigned short x, unsigned short y, char c,
		    unsigned int fColor, unsigned int bColor);
void PutGB1616(unsigned short x, unsigned short y, unsigned char c[2],
	       unsigned int fColor, unsigned int bColor);
void PutGB3232(unsigned short x, unsigned short y, unsigned char c[2],
	       unsigned int fColor, unsigned int bColor);

void main(void)
{
	LCD_PSB = 1;
	delayms(25);
	LCD_RES = 0;
	delayms(150);
	LCD_RES = 1;
	delayms(25);
	ILI9325_Initial();
	while (1) {
		ClearScreen(0x0);
		//show_colour_bar();    
		// Foreground color and background color can be directly pre-defined or can be directly filled with hexadecimal numbers
		LCD_PutString(2, 40, "http://ttsemb.taobao.com/", Red,
			      Blue);
		LCD_PutString(40, 60, "I LOVE MY JOB ", Blue2, Yellow);
		LCD_PutString(40, 80, "326电子工作室", Magenta,
			      Green);
		LCD_PutString(40, 100, "专业技术支持论坛", 0x07e0,
			      0xf800);
		LCD_PutString(40, 120, "专业开发板工控板", 0xF800,
			      Yellow);
		LCD_PutString(40, 140, "1234567890", 0xF800, Yellow);
		LCD_PutString(20, 160, "abcdefghijklmnopqistuvwxyz",
			      0xF800, Yellow);
		LCD_PutString(20, 180, "`,./<>';:[]{}\|?)(-=+*&^%$",
			      0xF800, Yellow);
		PutGB3232(20, 200, "我", Blue, Yellow);	//写入32x32汉字
		PutGB3232(60, 200, "爱", Blue, Red);
		PutGB3232(100, 200, "单", Blue, Magenta);
		PutGB3232(140, 200, "片", Blue, Green);
		PutGB3232(180, 200, "机", Blue, Cyan);
		PutGB3232(20, 240, "3", Blue, 0X00);
		PutGB3232(60, 240, "2", Blue, 0X00);
		PutGB3232(100, 240, "6", Blue, 0X00);
		PutGB3232(140, 240, "电", Blue, 0X00);
		PutGB3232(180, 240, "子", Blue, 0X00);
		PutGB3232(20, 280, "", Blue, 0X00);
		PutGB3232(60, 280, "欢", Blue, 0X00);
		PutGB3232(100, 280, "迎", Blue, 0X00);
		PutGB3232(140, 280, "你", Blue, 0X00);
		PutGB3232(180, 280, "！", Blue, 0X00);
		//      show_photo();
		while (1);
	}
}


void ClearScreen(unsigned int bColor)
{
	unsigned int i, j;

	LCD_SetPos(0, 240, 0, 320);	//320x240
	for (i = 0; i < 325; i++) {

		for (j = 0; j < 240; j++)
			Write_Data_U16(bColor);

	}
}

#include "8x16.h"

void LCD_PutChar8x16(unsigned short x, unsigned short y, char c,
		     unsigned int fColor, unsigned int bColor)
{
	unsigned int i, j;

	LCD_SetPos(x, x + 8 - 1, y, y + 16 - 1);
	for (i = 0; i < 16; i++) {
		unsigned char m = Font8x16[c * 16 + i];

		for (j = 0; j < 8; j++) {
			if ((m & 0x80) == 0x80) {
				Write_Data_U16(fColor);
			} else {
				Write_Data_U16(bColor);
			}
			m <<= 1;
		}
	}
}


void LCD_PutChar(unsigned short x, unsigned short y, char c,
		 unsigned int fColor, unsigned int bColor)
{

	LCD_PutChar8x16(x, y, c, fColor, bColor);
}


#include "GB1616.h"		// 16 * 16 Chinese font

void PutGB1616(unsigned short x, unsigned short y, unsigned char c[2],
	       unsigned int fColor, unsigned int bColor)
{
	unsigned int i, j, k;

	LCD_SetPos(x, x + 16 - 1, y, y + 16 - 1);

	for (k = 0; k < 64; k++) {
		// 64 indicates the number in the self-built Chinese character library, and the inner code is cyclically queried
		if ((codeGB_16[k].Index[0] == c[0])
		    && (codeGB_16[k].Index[1] == c[1])) {
			for (i = 0; i < 32; i++) {
				unsigned short m = codeGB_16[k].Msk[i];

				for (j = 0; j < 8; j++) {
					if ((m & 0x80) == 0x80) {
						Write_Data_U16(fColor);
					} else {
						Write_Data_U16(bColor);
					}
					m <<= 1;
				}
			}
		}
	}
}

#include "GB3232.h"		// 32 * 32 Chinese character font

void PutGB3232(unsigned short x, unsigned short y, unsigned char c[2],
	       unsigned int fColor, unsigned int bColor)
{
	unsigned int i, j, k;

	LCD_SetPos(x, x + 32 - 1, y, y + 32 - 1);

	for (k = 0; k < 15; k++) {
		// 15 indicates the number in the self-built Chinese character library, and the internal code is cyclically queried
		if ((codeGB_32[k].Index[0] == c[0])
		    && (codeGB_32[k].Index[1] == c[1])) {
			for (i = 0; i < 128; i++) {
				unsigned short m = codeGB_32[k].Msk[i];

				for (j = 0; j < 8; j++) {
					if ((m & 0x80) == 0x80) {
						Write_Data_U16(fColor);
					} else {
						Write_Data_U16(bColor);
					}
					m <<= 1;
				}
			}
		}
	}
}

void LCD_PutString(unsigned short x, unsigned short y, unsigned char *s,
		   unsigned int fColor, unsigned int bColor)
{
	unsigned char l = 0;

	while (*s) {
		if (*s < 0x80) {
			LCD_PutChar(x + l * 8, y, *s, fColor, bColor);
			s++;
			l++;
		} else {
			PutGB1616(x + l * 8, y, (unsigned char *) s,
				  fColor, bColor);
			s += 2;
			l += 2;
		}
	}
}

//==================== 显示R G B 顔色 ====================//
void Show_RGB(unsigned int x0, unsigned int x1, unsigned int y0,
	      unsigned int y1, unsigned int Color)
{
	unsigned int i, j;

	//address_set();
	LCD_SetPos(x0, x1, y0, y1);
	for (i = y0; i <= y1; i++) {
		for (j = x0; j <= x1; j++)
			Write_Data_U16(Color);

	}


}

//====================== 显示彩条 ======================//
void show_colour_bar(void)
{
	int V, H;

	LCD_SetPos(0, 240, 0, 320);	//320x240

	for (H = 0; H < 240; H++) {
		for (V = 0; V < 40; V++)
			Write_Data(0xf8, 0x00);
	}

	for (H = 0; H < 240; H++) {
		for (V = 40; V < 80; V++)
			Write_Data(0x07, 0xe0);
	}

	for (H = 0; H < 240; H++) {
		for (V = 80; V < 120; V++)
			Write_Data(0x00, 0x1f);
	}

	for (H = 0; H < 240; H++) {
		for (V = 120; V < 160; V++)
			Write_Data(0xff, 0xe0);
	}

	for (H = 0; H < 240; H++) {
		for (V = 160; V < 200; V++)
			Write_Data(0xf8, 0x1f);
	}

	for (H = 0; H < 240; H++) {
		for (V = 200; V < 240; V++)
			Write_Data(0x07, 0xff);
	}

	for (H = 0; H < 240; H++) {
		for (V = 240; V < 280; V++)
			Write_Data(0xff, 0xff);
	}

	for (H = 0; H < 240; H++) {
		for (V = 280; V < 320; V++)
			Write_Data(0x00, 0x00);
	}

}

void show_photo(void)
{
	unsigned char j;
	unsigned int i;
	unsigned long s = 0;

	LCD_SetPos(0, 240, 0, 320);	//320x240

	for (i = 0; i < 75; i++) {
		for (j = 0; j < 240; j++)
			Write_Data(0xff, 0xff);

	}

	for (i = 0; i < 170; i++) {
		for (j = 0; j < 55; j++)
			Write_Data(0xff, 0xff);
		for (j = 0; j < 130; j++)
			Write_Data(pic[s++], pic[s++]);
		for (j = 0; j < 55; j++)
			Write_Data(0xff, 0xff);
	}

	for (i = 0; i < 75; i++) {
		for (j = 0; j < 240; j++)
			Write_Data(0xff, 0xff);

	}

}

//=======================================================//

void Init_data(unsigned char x, unsigned int y)
{
	unsigned char m, n;

	m = y >> 8;
	n = y;
	Write_Cmd(0x00, x);
	Write_Data(m, n);
}


void Write_Data_U16(unsigned int y)
{
	unsigned char m, n;

	m = y >> 8;
	n = y;
	Write_Data(m, n);
}

//====================== 写命令 ==========================//
void Write_Cmd(unsigned char DH, unsigned char DL)
{
	LCD_PSB = 0;
	LCD_RS = 0;
	P2 = DH;
	LCD_RW = 0;
	LCD_RW = 1;
	P2 = DL;
	LCD_RW = 0;
	LCD_RW = 1;
	LCD_PSB = 1;
}

//===================== 写数据 ===========================//
void Write_Data(unsigned char DH, unsigned char DL)
{
	LCD_PSB = 0;
	LCD_RS = 1;
	P2 = DH;
	LCD_RW = 0;
	LCD_RW = 1;
	P2 = DL;
	LCD_RW = 0;
	LCD_RW = 1;
	LCD_PSB = 1;
}

//=======================================================
void delayms(unsigned int count)
{
	int i, j;

	for (i = 0; i < count; i++) {
		for (j = 0; j < 260; j++);
	}
}



//===================== 初始化代码 =======================//
void ILI9325_Initial(void)
{
	//////////////////////////////////////////////////////////
	delayms(150);		//根据不同晶振速度可以调整延时，保障稳定显示
	Init_data(0x00E5, 0x8000);	// Set the Vcore voltage and this setting is must.
	Init_data(0x0000, 0x0001);	// Start internal OSC.
	Init_data(0x0001, 0x0100);	// set SS and SM bit
	Init_data(0x0002, 0x0700);	// set 1 line inversion
	Init_data(0x0003, 0x0030);	// set GRAM write direction and BGR=0.
	Init_data(0x0004, 0x0000);	// Resize register
	Init_data(0x0008, 0x0202);	// set the back porch and front porch
	Init_data(0x0009, 0x0000);	// set non-display area refresh cycle ISC[3:0]
	Init_data(0x000A, 0x0000);	// FMARK function
	Init_data(0x000C, 0x0000);	// RGB interface setting
	Init_data(0x000D, 0x0000);	// Frame marker Position
	Init_data(0x000F, 0x0000);	// RGB interface polarity
//*************Power On sequence ****************//
	Init_data(0x0010, 0x0000);	// SAP, BT[3:0], AP, DSTB, SLP, STB
	Init_data(0x0011, 0x0007);	// DC1[2:0], DC0[2:0], VC[2:0]
	Init_data(0x0012, 0x0000);	// VREG1OUT voltage
	Init_data(0x0013, 0x0000);	// VDV[4:0] for VCOM amplitude
	delayms(200);			// Dis-charge capacitor power voltage
	Init_data(0x0010, 0x17B0);	// SAP, BT[3:0], AP, DSTB, SLP, STB
	Init_data(0x0011, 0x0037);	// DC1[2:0], DC0[2:0], VC[2:0]
	delayms(50);			// Delay 50ms
	Init_data(0x0012, 0x013E);	// VREG1OUT voltage
	delayms(50);			// Delay 50ms
	Init_data(0x0013, 0x1F00);	// VDV[4:0] for VCOM amplitude
	Init_data(0x0029, 0x0013);	// VCM[4:0] for VCOMH
	delayms(50);
	Init_data(0x0020, 0x0000);	// GRAM horizontal Address
	Init_data(0x0021, 0x0000);	// GRAM Vertical Address
// ----------- Adjust the Gamma Curve ----------//
	Init_data(0x0030, 0x0000);
	Init_data(0x0031, 0x0404);
	Init_data(0x0032, 0x0404);
	Init_data(0x0035, 0x0004);
	Init_data(0x0036, 0x0404);
	Init_data(0x0037, 0x0404);
	Init_data(0x0038, 0x0404);
	Init_data(0x0039, 0x0707);
	Init_data(0x003C, 0x0500);
	Init_data(0x003D, 0x0607);
//------------------ Set GRAM area ---------------//
	Init_data(0x0050, 0x0000);	// Horizontal GRAM Start Address

	Init_data(0x0051, 0x00EF);	// Horizontal GRAM End Address
	Init_data(0x0052, 0x0000);	// Vertical GRAM Start Address
	Init_data(0x0053, 0x013F);	// Vertical GRAM Start Address
	Init_data(0x0060, 0x2700);	// Gate Scan Line
	Init_data(0x0061, 0x0001);	// NDL,VLE, REV
	Init_data(0x006A, 0x0000);	// set scrolling line
//-------------- Partial Display Control ---------//
	Init_data(0x0080, 0x0000);
	Init_data(0x0081, 0x0000);
	Init_data(0x0082, 0x0000);
	Init_data(0x0083, 0x0000);
	Init_data(0x0084, 0x0000);
	Init_data(0x0085, 0x0000);
//-------------- Panel Control -------------------//
	Init_data(0x0090, 0x0010);
	Init_data(0x0092, 0x0000);
	Init_data(0x0093, 0x0003);
	Init_data(0x0095, 0x0110);
	Init_data(0x0097, 0x0000);
	Init_data(0x0098, 0x0000);
	Init_data(0x0007, 0x0173);	// 262K color and display ON
//      Write_Cmd_Data(0x0022);//               
}

static void LCD_SetPos(unsigned int x0, unsigned int x1, unsigned int y0,
		       unsigned int y1)
{
	Init_data(WINDOW_XADDR_START, x0);
	Init_data(WINDOW_XADDR_END, x1);
	Init_data(WINDOW_YADDR_START, y0);
	Init_data(WINDOW_YADDR_END, y1);
	Init_data(GRAM_XADDR, x0);
	Init_data(GRAM_YADDR, y0);
	Write_Cmd(0x00, 0x22);	//LCD_WriteCMD(GRAMWR);
}
