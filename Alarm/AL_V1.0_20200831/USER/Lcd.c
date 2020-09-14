#include "includes.h"

u8 LcdWriteCommand(u8 Cmd)
{
    return I2C_WriteByte(Cmd, 0x00, 0x78);
}

//д��1�ֽ�����
u8 LcdWriteData(u8 Data)
{
    return I2C_WriteByte(Data, 0x40, 0x78);
}

//д����ֽ�����
u8 LcdWriteDataBytes(u8* Data, u16 bytes)
{
    return I2C_WriteData(0x78, 0x40, Data, bytes);
}
//��������
void LcdSetPos(unsigned char x, unsigned char y) 
{
    LcdWriteCommand(0xb0+y);
	LcdWriteCommand(((x&0xf0)>>4)|0x10);
	LcdWriteCommand((x&0x0f)); 
}   	  
//����OLED��ʾ    
void LcdDisplayOn(void)
{
	LcdWriteCommand(0X8D);  //SET DCDC����
	LcdWriteCommand(0X14);  //DCDC ON
	LcdWriteCommand(0XAF);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void LcdDisplayOff(void)
{
	LcdWriteCommand(0X8D);  //SET DCDC����
	LcdWriteCommand(0X10);  //DCDC OFF
	LcdWriteCommand(0XAE);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void LcdClear(void)  
{  
	u8 i,n;
    
	for(i=0;i<8;i++)  
	{  
		LcdWriteCommand (0xb0+i);    //����ҳ��ַ��0~7��
		LcdWriteCommand (0x00);      //������ʾλ�á��е͵�ַ
		LcdWriteCommand (0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
        {
            LcdWriteData(0); 
        }      
	} 
}
void LcdOn(void)  
{  
	u8 i,n;
    
	for(i=0;i<8;i++)  
	{  
		LcdWriteCommand (0xb0+i);    //����ҳ��ַ��0~7��
		LcdWriteCommand (0x00);      //������ʾλ�á��е͵�ַ
		LcdWriteCommand (0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
        {
            LcdWriteData(0xFF); 
        }       
	}
}

//96*32
ErrorStatus LcdInit(void)
{
    if(LcdWriteCommand(0xAE) == 0)//�ر���ʾ
	{
        return ERROR;
    }
	LcdWriteCommand(0x00);//---set low column address
	LcdWriteCommand(0x10);//---set high column address

	
	LcdWriteCommand(0x40);//set display start line
	LcdWriteCommand(0xB0);//set page address

	LcdWriteCommand(0x81);//���öԱȶ�
	LcdWriteCommand(0x45);

	LcdWriteCommand(0xa1);//set segment remap a0��c8ʱ��ת180��
	LcdWriteCommand(0xc0);/*Com scan direction 0XC0 */
	LcdWriteCommand(0xa6);//normal / reverse
	
	LcdWriteCommand(0xa8);//��������·��
	LcdWriteCommand(0x1f);//duty = 1/32
	

	LcdWriteCommand(0xd3);/*set display offset*/
	LcdWriteCommand(0x00);
	
	LcdWriteCommand(0xd5);/*set osc division*/
	LcdWriteCommand(0x80);
	
	LcdWriteCommand(0xd9);/*set pre-charge period*/
	LcdWriteCommand(0x22);
	
	LcdWriteCommand(0xda);/*set COM pins*/
	LcdWriteCommand(0x12);
	
	LcdWriteCommand(0xdb);/*set vcomh*/
	LcdWriteCommand(0x20);
	
	LcdWriteCommand(0x8d);/*set vcomh*/
	LcdWriteCommand(0x14);/* Set_Charge_Pump 0x14:9v; 0x15 7.5v */
	
	LcdWriteCommand(0xaf);
    
	LcdClear();
    LcdBufferClear();
    
    LcdSetFont(FONT_16);
    LcdSetFontState(REVERSE_MODE);//ˢ�±���ɫ
//    LcdDrawStringHCenter(0, "�������");
//    LcdDrawStringHCenter(16, "V1.0");
//    LcdBufferPaint();

    return SUCCESS;
}

