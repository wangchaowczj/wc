#include "includes.h"

SCAN_KEYBOARD ScanKeyBoard;

/*ͨ���ж϶˵��ȡ��ꡢ�����ϴ������� */
unsigned char InterruptDataTrans(unsigned char *ptr, unsigned char* MouseTog, unsigned char* Bytes)
{
	UINT8	s;
	u8 len;
	u8 Tog;
	
	Tog = *MouseTog;
	
	s = WaitHostTransact374(mDiskBulkInEndp, DEF_USB_PID_IN, Tog, 200);  // IN����
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
*��귢�͸�PC������ÿ��4���ֽ� 
BYTE1 BYTE2 BYTE3 BYTE4 
����ֱ��ǣ� 
BYTE1 -- 
       |--bit7:   1   ��ʾ   Y   ����ı仯��������256   ~   255�ķ�Χ,0��ʾû�����  
       |--bit6:   1   ��ʾ   X   ����ı仯��������256   ~   255�ķ�Χ��0��ʾû�����  
       |--bit5:   Y   ����仯�ķ���λ��1��ʾ����������������ƶ�  
       |--bit4:   X   ����仯�ķ���λ��1��ʾ����������������ƶ�  
       |--bit3:     ��Ϊ1  
       |--bit2:     1��ʾ�м�����  
       |--bit1:     1��ʾ�Ҽ�����  
       |--bit0:     1��ʾ�������  
BYTE2 -- X����仯������byte��bit4���9λ������,������ʾ�����ƣ����������ơ��ò����ʾ�仯��  
BYTE3 -- Y����仯������byte��bit5���9λ��������������ʾ�����ƣ����������ơ��ò����ʾ�仯�� 
BYTE4 -- ���ֱ仯��  
*/
void MouseState(u8* Buffer)
{
//	GUI_PID_STATE MouseState;
//	u8 Value;
//	
//	GUI_PID_GetState(&MouseState);
//	if(Buffer[2]&0x80)
//	{//����
//		Value = Buffer[2]-1;//����
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
//	{//����
//		MouseState.y += Buffer[2];
//		if(MouseState.y >= 477)
//		{
//			MouseState.y = 477;
//		}
//	}
//	if(Buffer[1]&0x80)
//	{//����
//		Value = Buffer[1]-1;//����
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
//	{//����
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
���̷��͸�PC������ÿ��8���ֽ� 
BYTE1 BYTE2 BYTE3 BYTE4 BYTE5 BYTE6 BYTE7 BYTE8 
����ֱ��ǣ� 
BYTE1 -- 
       |--bit0:   Left Control�Ƿ��£�����Ϊ1  
       |--bit1:   Left Shift  �Ƿ��£�����Ϊ1  
       |--bit2:   Left Alt    �Ƿ��£�����Ϊ1  
       |--bit3:   Left GUI    �Ƿ��£�����Ϊ1  
       |--bit4:   Right Control�Ƿ��£�����Ϊ1   
       |--bit5:   Right Shift �Ƿ��£�����Ϊ1  
       |--bit6:   Right Alt   �Ƿ��£�����Ϊ1  
       |--bit7:   Right GUI   �Ƿ��£�����Ϊ1  
BYTE2 -- �ݲ�������еĵط�˵�Ǳ���λ 
BYTE3--BYTE8 -- ������Ϊ��ͨ���� 
���̾������ԡ� 
���磺���̷���һ֡����   02 00 0x04 0x05 00 00 00 00 
��ʾͬʱ������Left Shift + ��a��+��b��������
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
			{//����SHIFT����ת�ɴ�д
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
			{//����SHIFT����ת�ɴ�д
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
			{//����SHIFT��
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
			{//����SHIFT��
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
			{//����SHIFT��
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
	
//	mSetBulkIn();  /* ���� */	
	s = InterruptDataTrans(Buffer, MouseTog, &Bytes);
	if(s == USB_INT_SUCCESS)
	{
		if(Bytes == 4)
		{//��׼��USB���������4�ֽ�
			MouseState(&Buffer[0]);
			return 0;
		}
		return 1;
	} 
	return 2;//û�ж���������ݻ����ǲ�֧�ֵ��豸(����ƶ����ߵ��ʱ���ܶ�������)	
}

u8 CH374HostScanKeyBoardProcess(u8* MouseTog)
{
	u8 i, s;
	u8 Buffer[64];
	u8 Bytes = 0;
	
//	mSetBulkIn();  /* ���� */	
	s = InterruptDataTrans(Buffer, MouseTog, &Bytes);
	if(s == USB_INT_SUCCESS)
	{
		if((Bytes <= 64) && ((Bytes%8) == 0))
		{//��׼��USB����������8�ֽ�
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
	return 2;//û�ж���������ݻ����ǲ�֧�ֵ��豸(����ƶ����ߵ��ʱ���ܶ�������)	
}
