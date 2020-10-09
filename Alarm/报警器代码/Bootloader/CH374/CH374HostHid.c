#include "includes.h"

SCAN_KEYBOARD ScanKeyBoard;

/*通过中断端点获取鼠标、键盘上传的数据 */
unsigned char InterruptDataTrans(unsigned char *ptr, unsigned char* MouseTog, unsigned char* Bytes)
{
	UINT8	s;
	u8 len;
	u8 Tog;
	
	Tog = *MouseTog;
	
	s = WaitHostTransact374(mDiskBulkInEndp, DEF_USB_PID_IN, Tog, 200);  // IN数据
	if (s == USB_INT_SUCCESS) 
	{
		len = Read374Byte( REG_USB_LENGTH );
		if(len > 64)
		{
			len = 64;
		}
		Read374Block( RAM_HOST_RECV, len, ptr);
		*MouseTog = Tog ? FALSE : TRUE;		
		*Bytes = len;
	}
	return s;
}
/**
*鼠标发送给PC的数据每次4个字节 
BYTE1 BYTE2 BYTE3 BYTE4 
定义分别是： 
BYTE1 -- 
       |--bit7:   1   表示   Y   坐标的变化量超出－256   ~   255的范围,0表示没有溢出  
       |--bit6:   1   表示   X   坐标的变化量超出－256   ~   255的范围，0表示没有溢出  
       |--bit5:   Y   坐标变化的符号位，1表示负数，即鼠标向下移动  
       |--bit4:   X   坐标变化的符号位，1表示负数，即鼠标向左移动  
       |--bit3:     恒为1  
       |--bit2:     1表示中键按下  
       |--bit1:     1表示右键按下  
       |--bit0:     1表示左键按下  
BYTE2 -- X坐标变化量，与byte的bit4组成9位符号数,负数表示向左移，正数表右移。用补码表示变化量  
BYTE3 -- Y坐标变化量，与byte的bit5组成9位符号数，负数表示向下移，正数表上移。用补码表示变化量 
BYTE4 -- 滚轮变化。  
*/
void MouseState(u8* Buffer)
{
//	GUI_PID_STATE MouseState;
//	u8 Value;
//	
//	GUI_PID_GetState(&MouseState);
//	if(Buffer[2]&0x80)
//	{//向上
//		Value = Buffer[2]-1;//补码
//		Value = ~Value;
//		if(MouseState.y >= Value)
//		{
//			MouseState.y -= Value;
//		}
//		else
//		{
//			MouseState.y = 0;
//		}
//	}
//	else
//	{//向下
//		MouseState.y += Buffer[2];
//		if(MouseState.y >= 477)
//		{
//			MouseState.y = 477;
//		}
//	}
//	if(Buffer[1]&0x80)
//	{//向左
//		Value = Buffer[1]-1;//补码
//		Value = ~Value;	
//		if(MouseState.x >= Value)
//		{
//			MouseState.x -= Value;
//		}
//		else
//		{
//			MouseState.x = 0;
//		}
//	}
//	else
//	{//向右
//		MouseState.x += Buffer[1];
//		if(MouseState.x >= 637)
//		{
//			MouseState.x = 637;
//		}
//	}
//	MouseState.Pressed = Buffer[0]&0x01;
//	GUI_MOUSE_StoreState(&MouseState);
}
/*
键盘发送给PC的数据每次8个字节 
BYTE1 BYTE2 BYTE3 BYTE4 BYTE5 BYTE6 BYTE7 BYTE8 
定义分别是： 
BYTE1 -- 
       |--bit0:   Left Control是否按下，按下为1  
       |--bit1:   Left Shift  是否按下，按下为1  
       |--bit2:   Left Alt    是否按下，按下为1  
       |--bit3:   Left GUI    是否按下，按下为1  
       |--bit4:   Right Control是否按下，按下为1   
       |--bit5:   Right Shift 是否按下，按下为1  
       |--bit6:   Right Alt   是否按下，按下为1  
       |--bit7:   Right GUI   是否按下，按下为1  
BYTE2 -- 暂不清楚，有的地方说是保留位 
BYTE3--BYTE8 -- 这六个为普通按键 
键盘经过测试。 
例如：键盘发送一帧数据   02 00 0x04 0x05 00 00 00 00 
表示同时按下了Left Shift + ‘a’+‘b’三个键
*/
const u8 NumberShift[] = {'!','@', '#', '$', '%', '^', '&', '*', '('};
const u8 Chars1[] =      {' ','-','=', '[', ']', '\\'};
const u8 Chars1Shift[] = {' ','_','+', '{', '}', '|'};
const u8 Chars2[] =      {';','\'','`',',','.','/'};
const u8 Chars2Shift[] = {':','"','~','<','>','?'};

static u8 ScanKeyBoardCloseFlag = 0;

void OpenScanKeyBoard(void)
{
	ScanKeyBoardCloseFlag = 0;
}
void CloseScanKeyBoard(void)
{
	ScanKeyBoardCloseFlag = 1;
}
u8 ScanKeyBoardIsClose(void)
{
	return ScanKeyBoardCloseFlag;
}
void ClearScanKeyBoard(void)
{
	ScanKeyBoard.Bytes = 0;
	ScanKeyBoard.Status = SCAN_KEYBOARD_IDLE;
}
void KeyBoardValue(u8* Buff)
{
	u8 i;
	u8 Shift = 0;
	u8 KeyChar = 0;
	
	if(ScanKeyBoard.Status == SCAN_KEYBOARD_END)
	{
		return;
	}
	
	Shift = Buff[0]&0x22;
	for(i=2; i < 8; i++)
	{
		KeyChar = 0;
		if(Buff[i] == 0x00)
		{
			continue;
		}
		if((Buff[i] >= 0x04) && (Buff[i] <= 0x1D))
		{//a~z
			if(Shift)
			{//按下SHIFT键，转成大写
				KeyChar = Buff[i] - 0x04 + 'A';
			}
			else
			{
				KeyChar = Buff[i] - 0x04 + 'a';
			}
		}
		else if(Buff[i] == 0x27)
		{//0
			if(Shift)
			{//按下SHIFT键，转成大写
				KeyChar = ')';
			}
			else
			{
				KeyChar = '0';
			}			
		}
		else if((Buff[i] >= 0x1E) && (Buff[i] <= 0x26))
		{//1~9
			if(Shift)
			{//按下SHIFT键
				KeyChar = NumberShift[Buff[i] - 0x1E];
			}
			else
			{
				KeyChar = Buff[i] - 0x1E + '1';
			}		
		}
		else if((Buff[i] >= 0x2C) && (Buff[i] <= 0x31))
		{/* -=[]\*/
			if(Shift)
			{//按下SHIFT键
				KeyChar = Chars1Shift[Buff[i] - 0x2C];
			}
			else
			{
				KeyChar = Chars1[Buff[i] - 0x2C];
			}		
		}
		else if((Buff[i] >= 0x33) && (Buff[i] <= 0x38))
		{/*;'*/
			if(Shift)
			{//按下SHIFT键
				KeyChar = Chars2Shift[Buff[i] - 0x33];
			}
			else
			{
				KeyChar = Chars2[Buff[i] - 0x33];
			}		
		}
		else if(Buff[i] == 0x28)//0x0D
		{
			if(ScanKeyBoard.Bytes != 0x00)
			{
				ScanKeyBoard.Status = SCAN_KEYBOARD_END;			
			}

			break;
		}
		if(KeyChar != 0)
		{	
			if(ScanKeyBoard.Status != SCAN_KEYBOARD_DOING)
			{
				ScanKeyBoard.Bytes = 0;
			}
			ScanKeyBoard.Status = SCAN_KEYBOARD_DOING;
			
			if((ScanKeyBoard.Bytes+1) >= SCAN_KEYBOARD_DATA_LEN)
			{
				ScanKeyBoard.Bytes = 0;
			}
			
			ScanKeyBoard.Data[ScanKeyBoard.Bytes++] = KeyChar;
			ScanKeyBoard.Data[ScanKeyBoard.Bytes] = 0;			
		}
	}	
}
u8 CH374HostMouseProcess(u8* MouseTog)
{
	u8 s;
	u8 Buffer[64];
	u8 Bytes = 0;
	
//	mSetBulkIn();  /* 输入 */	
	s = InterruptDataTrans(Buffer, MouseTog, &Bytes);
	if(s == USB_INT_SUCCESS)
	{
		if(Bytes == 4)
		{//标准的USB鼠标数据是4字节
			MouseState(&Buffer[0]);
			return 0;
		}
		return 1;
	} 
	return 2;//没有读到鼠标数据或者是不支持的设备(鼠标移动或者点击时才能读到数据)	
}

u8 CH374HostScanKeyBoardProcess(u8* MouseTog)
{
	u8 i, s;
	u8 Buffer[64];
	u8 Bytes = 0;
	
//	mSetBulkIn();  /* 输入 */	
	s = InterruptDataTrans(Buffer, MouseTog, &Bytes);
	if(s == USB_INT_SUCCESS)
	{
		if((Bytes <= 64) && ((Bytes%8) == 0))
		{//标准的USB键盘数据是8字节
			if(!ScanKeyBoardIsClose())
			{
				for(i=0; i<Bytes;i+=8)
				{
					KeyBoardValue(&Buffer[i]);
				}							
			}
			return 0;			
		}
		return 1;
	}
	return 2;//没有读到鼠标数据或者是不支持的设备(鼠标移动或者点击时才能读到数据)	
}
