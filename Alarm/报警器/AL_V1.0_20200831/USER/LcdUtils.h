#ifndef _LCDUTILS_H_
#define _LCDUTILS_H_

typedef enum
{
	FONT_16,
	FONT_24
}LCDFONT;

typedef enum
{
	NORMAL_MODE,//不刷新背影色
	REVERSE_MODE//刷新背景色
}LCDREVERSE;

#define LCDCBRIGHT_16		1
#define LCDCBRIGHT_15		3
#define LCDCBRIGHT_14		5
#define LCDCBRIGHT_13		7
#define LCDCBRIGHT_12		9
#define LCDCBRIGHT_11		11
#define LCDCBRIGHT_10		13
#define LCDCBRIGHT_9		15

#define LCDCBRIGHT_8		17
#define LCDCBRIGHT_7		19
#define LCDCBRIGHT_6		21
#define LCDCBRIGHT_5		23
#define LCDCBRIGHT_4		25
#define LCDCBRIGHT_3		27
#define LCDCBRIGHT_2		29
#define LCDCBRIGHT_1		31
#define LCDCBRIGHT_0		32

#define COLOR_BLACK			0x0000			//黑色		0x000000
#define COLOR_WHITE			0xFFFF			//白色		0xFFFFFF
#define COLOR_YALLOW		0xFFE0			//黄色		0xFFFF00
#define COLOR_BLUE			0x001F			//蓝色		0x0000FF
#define COLOR_DARKGREEN		0X06E0			//深绿色	0x00DC00	可用于电池显示
#define COLOR_CYAN			0x07FF			//青色		0x00FFFF
#define COLOR_DARKCYAN		0x0FEA
#define COLOR_GREEN			0x07E0			//绿色		0x00FF00
#define COLOR_RED			0xF800			//红色		0xFF0000
#define COLOR_LIRED			0xF808

#define COLOR_BLUEXUAN		0x001F
#define COLOR_BLUEWU		0x711F
#define COLOR_BLUEAN		0x000F
#define COLOR_BROWN			0x8145			//棕色		0x802A2A




//****修改界面后加入的颜色****//
//#define COLOR_GRAY_1		0xDEFB			//浅灰色	0xDCDCDC	对应RGB：220	220		220
#define COLOR_GRAY_1		0x8FF9			//浅灰色	0xDCDCDC	对应RGB：142	252		202
#define COLOR_GRAY_2		0x52AA			//深灰色	0x545555	对应RGB：84		85		85
#define COLOR_BLACK_1		0x0000			//黑色		0x000000	对应RGB：0		0		0
#define COLOR_BLUE_2		0x003F			//蓝色		
#define COLOR_BLUE_1		0x27FE			//蓝色		0x2B9FDB	对应RGB：35		255		240
#define COLOR_GREEN_1		0x07E7			//绿色		0x63B72F	对应RGB：0		253		60
#define COLOR_YALLOW_1		0xFF82			//黄色		0xF7AB00	对应RGB：251	240		20
#define COLOR_RED_1			0xF800			//红色		0xE60012	对应RGB：255	0		0






#define FONT16_CWIDTH		8				//8*16字符宽度
#define FONT16_CHIGH		16				//8*16字符高度
#define FONT16_CLEN			16				//8*16字符占的字节数

#define FONT16_HWIDTH		16				//16*16汉字宽度
#define FONT16_HHIGH		16				//16*16汉字高度
#define FONT16_HLEN			32				//16*16汉字占的字节数

#define FONT24_CWIDTH		12				//12*24字符宽度
#define FONT24_CHIGH		24				//12*24字符高度
#define FONT24_CLEN			48				//12*24字符占的字节数

#define FONT24_HWIDTH		24				//24*24汉字宽度
#define FONT24_HHIGH		24				//24*24汉字高度
#define FONT24_HLEN			72				//24*24汉字占的字节数




#define BOOTH_FUHAO			0
#define USB_FUHAO			1


#define LCD_WIDTH_PIX       96
#define LCD_HEIGHT_PIX      32

extern void LcdSetFontState(LCDREVERSE state);
extern LCDREVERSE LcdGetFontState(void);
extern void LcdSetFont(LCDFONT state);
extern LCDFONT LcdGetFont(void);

extern void LcdBufferClear(void);
extern void LcdDrawPoint(u8 xAddr, u8 yAddr, u8 Color);
extern void LcdBufferPaint(void);
extern void LcdDrawLineH(u8 xSAddr, u8 xEAddr, u8 yAddr, u8 Color);
extern void LcdDrawLineV(u8 xSAddr, u8 ySAddr, u8 yEAddr, u8 Color);
extern void LcdDrawString(u16 xAddr, u16 yAddr, char *StrBuf);
extern void LcdDrawStringHCenter(u16 yAddr, char *StrBuf);
extern void LcdDrawStringCenter(char *StrBuf);
extern void LcdShowStrCenter(char *StrBuf);
extern void LcdShowStr2Center(char *Str1, char *Str2);

#endif
