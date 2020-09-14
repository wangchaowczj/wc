#include "includes.h"
#include "Char8_16.h"
#include "Hz16_16.h"
#include "Char12_24.h"
#include "Hz24_24.h"

u8 LcdBuffer[4][LCD_WIDTH_PIX];//像素96*32,单色

struct {
	u8 CWidth;				//字符的宽度
	u8 CHigh;				//字符的高度
	u8 Clenth;				//字符占的字节数
	u8 HWidth;				//汉字的宽度
	u8 HHigh;				//汉字的高度
	u8 Hlenth;				//汉字占的字节数
} LcdFont;

struct {
	u16 FrontColor;				//前景色
	u16 BackColor;				//背景色
	LCDFONT Font;				//字体
	LCDREVERSE BackCoState;		//是否更新背景色
} LcdState;

void LcdSetFontState(LCDREVERSE state)
{
	LcdState.BackCoState = state;
}

LCDREVERSE LcdGetFontState(void)
{
	return LcdState.BackCoState;
}
void LcdSetFrontBackColor(u16 Front, u16 Back)
{
	LcdState.FrontColor = Front;
	LcdState.BackColor = Back;
}
u16 LcdGetFrontColor(void)
{
	return LcdState.FrontColor;
}
u16 LcdGetBackColor(void)
{
	return LcdState.BackColor;
}
void LcdSetFont(LCDFONT state)
{
	LcdState.Font = state;
	switch (LcdState.Font)
	{
		case FONT_16:
			LcdFont.CWidth = FONT16_CWIDTH;
			LcdFont.CHigh = FONT16_CHIGH;
			LcdFont.Clenth = FONT16_CLEN;
			LcdFont.HWidth = FONT16_HWIDTH;
			LcdFont.HHigh = FONT16_HHIGH;
			LcdFont.Hlenth = FONT16_HLEN;
			break;
		case FONT_24:
			LcdFont.CWidth = FONT24_CWIDTH;
			LcdFont.CHigh = FONT24_CHIGH;
			LcdFont.Clenth = FONT24_CLEN;
			LcdFont.HWidth = FONT24_HWIDTH;
			LcdFont.HHigh = FONT24_HHIGH;
			LcdFont.Hlenth = FONT24_HLEN;
			break;
	}
}
LCDFONT LcdGetFont(void)
{
	return LcdState.Font;
}
void LcdBufferClear(void)
{
    u8 x,y;
    
    for(y=0;y<4;y++)
    {
        for(x=0;x<96;x++)
        {
            LcdBuffer[y][x] = 0;
        }     
    }
}
//实测耗时45ms
void LcdBufferPaint(void)
{
    u8 y;
    
    for(y=0;y<4;y++)
    {
        LcdSetPos(0, y);
        LcdWriteDataBytes(&LcdBuffer[y][0], sizeof(LcdBuffer[0]));     
    }
}
void LcdDrawPoint(u8 xAddr, u8 yAddr, u8 Color)
{  
    if(xAddr >= LCD_WIDTH_PIX)
    {
        return;
    }
    if(yAddr >= LCD_HEIGHT_PIX)
    {
        return;
    }
    
    if(Color)
    {
        LcdBuffer[yAddr/8][xAddr] |= 1<<(yAddr%8);
    }
    else
    {
        LcdBuffer[yAddr/8][xAddr] &= ~(1<<(yAddr%8));
    }
    
}
void LcdDrawLineH(u8 xSAddr, u8 xEAddr, u8 yAddr, u8 Color)
{
    u8 i;
    
    for(i=xSAddr; i<=xEAddr; i++)
    {
        LcdDrawPoint(i, yAddr, Color);
    }
}

void LcdDrawLineV(u8 xSAddr, u8 ySAddr, u8 yEAddr, u8 Color)
{
    u8 i;
    
    for(i=ySAddr; i<=yEAddr; i++)
    {
        LcdDrawPoint(xSAddr, i, Color);
    }
}

u8* LcdCheckCharHz(char *pdata, u8 CharHz)
{
	u8 *paddr = 0;		//汉字/字符数据区
	u8 *ptempaddr;		//汉字索引区
	u16 i,num;

	i=0;
	if (CharHz)					//查找汉字
	{
		switch (LcdState.Font)
		{
			case FONT_16:					//16*16汉字
				ptempaddr = (u8 *)hzIndex16_16;
				paddr = (u8 *)hzdot16_16;
				num = hzNum16_16;
				break;
			case FONT_24:					//24*24汉字
				ptempaddr = (u8 *)hzIndex24_24;
				paddr = (u8 *)hzdot24_24;
				num = hzNum24_24;
				break;
			default:
				return (u8 *)0;
		}
		while(i<num)		//搜索字符所在的位置
		{
			if(memcmp(pdata,ptempaddr+(i*2),2) == 0)
			{
				paddr = paddr + (i * LcdFont.Hlenth);
				break;
			}
			i++;
		}
		if (i >= num)
		{
			return (void *)0;
		}
	}
	else
	{
		switch (LcdState.Font)
		{
			case FONT_16:					//8*16英文字符
				paddr = (u8 *)chardot8_16 + LcdFont.Clenth*((*pdata)-0x20);
				break;
			case FONT_24:					//12*24英文字符
				paddr = (u8 *)chardot12_24 + LcdFont.Clenth*((*pdata)-0x20);
				break;
			default:
				return (u8 *)0;
		}
	}
	return paddr;
}

void LcdDrawOne(u16 xAddr, u16 yAddr, char *CharName, u8 CharHz)
{
	u8 *pChar;
	u8 data;				//暂存获取的显示数据
	u8 i,j,k;
	u8 wlen,hlen;			//宽度和高度

	i=j=k=0;

//	if (LcdState.Font == FONT_16)
//	{
//		wlen = 8;
//		hlen = 16;
//		pChar = chardot8_16 + 16*(CharName-0x20);
//	}
//	else if (LcdState.Font == FONT_24)
//	{
//		wlen = 12;
//		hlen = 24;
//		pChar = chardot8_16 + 16*(CharName-0x20);
//	}

	pChar = LcdCheckCharHz(CharName,CharHz);
	if (pChar == 0)
	{
		return;
	}

	if (CharHz)
	{
		wlen = LcdFont.HWidth;
		hlen = LcdFont.HHigh;
	}
	else
	{
		wlen = LcdFont.CWidth;
		hlen = LcdFont.CHigh;
	}

	data = *(pChar++);
	for (i=0;i<hlen;i++)		//高度
	{
		for (j=0;j<wlen;j++)	//宽度
		{
			if (data & (1 << (7-k)))						//该位有数据，刷该点
			{
				LcdDrawPoint(xAddr+j,yAddr+i,1);		//刷前景色
			}
			else if (LcdState.BackCoState == REVERSE_MODE)	//判断是否刷新背景色
			{
				LcdDrawPoint(xAddr+j,yAddr+i,0);		//刷背景色
			}
			k++;
			if ((k > 7) || ((j+1) >= wlen))
			{
				k=0;
				data = *(pChar++);
			}
		}
	}
}
void LcdDrawString(u16 xAddr, u16 yAddr, char *StrBuf)
{
	u16 index, x,y;
    
    index = 0;
	x = xAddr;
	y = yAddr;

	while(*(StrBuf+index) != '\0')
	{
		if (*(StrBuf+index) & 0x80)						//汉字
		{
			LcdDrawOne(x,y,(StrBuf+index),1);
			x += LcdFont.HWidth;
			index += 2;
		}
		else									//英文字符
		{
			LcdDrawOne(x,y,(StrBuf+index),0);
			x += LcdFont.CWidth;
			index++;
		}
	}
}
void LcdDrawStringHCenter(u16 yAddr, char *StrBuf)
{
	u16 index, x;
    
    index = 0;
	x = 0;

	while(*(StrBuf+index) != '\0')
	{
		if (*(StrBuf+index) & 0x80)						//汉字
		{
			x += LcdFont.HWidth;
			index += 2;
		}
		else									//英文字符
		{
			x += LcdFont.CWidth;
			index++;
		}
	}
    if(x >= LCD_WIDTH_PIX)
    {
        x = 0;
    }
    else
    {
        x = (LCD_WIDTH_PIX-x)/2;
    }
	LcdDrawString(x, yAddr, StrBuf);
}
void LcdDrawStringCenter(char *StrBuf)
{
	u16 y;
    
    y = (LCD_HEIGHT_PIX-LcdFont.HHigh)/2;
	LcdDrawStringHCenter(y, StrBuf);
}

void LcdShowStrCenter(char *StrBuf)
{
    LcdBufferClear();
	LcdDrawStringCenter(StrBuf);
    LcdBufferPaint();
}
void LcdShowStr2Center(char *Str1, char *Str2)
{
    u16 Yspace;
    
    Yspace = (LCD_HEIGHT_PIX - LcdFont.HHigh*2)/3;
    
    LcdBufferClear();
  	LcdDrawStringHCenter(Yspace, Str1);
    LcdDrawStringHCenter(Yspace*2+LcdFont.HHigh, Str2);
    LcdBufferPaint();
}
