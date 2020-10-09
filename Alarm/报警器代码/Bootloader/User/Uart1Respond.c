#include "includes.h"

STR_COMM  Uart1Data;
STR_COMM  Uart1TempBuffer;

/**
  *************************************************************************************
  * @brief  获取常规的响应数据
  * @param  buffer：数据缓冲区
  * @param  Cmd：命令
  * @param  PackIndex：分包号  
  * @retval 无 
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
void GetGeneralRespondData(STR_COMM* buffer, u16 Cmd, u16 PackIndex)
{
	buffer->bytenum = 0;
	buffer->uch_buf[buffer->bytenum++] = 'A';
	buffer->uch_buf[buffer->bytenum++] = (u8)(Cmd>>8);
	buffer->uch_buf[buffer->bytenum++] = (u8)(Cmd&0xFF);
	buffer->uch_buf[buffer->bytenum++] = (PackIndex>>8)&0xFF;
	buffer->uch_buf[buffer->bytenum++] = PackIndex&0xFF;
}
/**
  *************************************************************************************
  * @brief  获取缓冲区中的字符串长度
  * @param  buffer：数据缓冲区
  * @param  MaxLenght：数据缓冲区长度 
  * @retval 字符串长度 
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
u16 GetBufferStringLenght(u8* Buffer, u16 MaxLenght)
{
	u16 i;
	
	for(i=0;i<MaxLenght;i++)
	{
		if(Buffer[i] == 0x00)
		{
			break;
		}
	}
	return i;
}

/**
  *************************************************************************************
  * @brief  从缓冲区中的数值
  * @param  buffer：数据缓冲区
  * @param  MaxLenght：数据缓冲区长度 
  * @retval 数值
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
u32 GetDecValueFromBuffer(u8* Buffer, u16 MaxLenght)
{
	u8 i;
	u32 Value = 0;
	
	for(i=0;i<MaxLenght;i++)
	{
		if ((Buffer[i] < '0') || (Buffer[i] > '9'))
		{
			break;
		}
		Value *= 10;
		Value += (Buffer[i]-'0');
	}
	return Value;
}

/**
  *************************************************************************************
  * @brief  从分隔符中取数据
  * @param  Separator：分隔符
  * @param  SeparatorNumber：分隔符序号
  * @param  Buffer：原始数据
  * @param  Bytes：原始数据长度
  * @param  StartIndex：得到的数据在Buffer中的起始索引值
  * @param  DataBytes：得到的数据长度  
  * @retval 无
  * @author ZH
  * @date   2015年1月06日
  * @note   “123\rABCD\rGL\r”,如想取出"ABCD"，Separator SeparatorNumber 分别为'\r',2
  *************************************************************************************  
  */
ErrorStatus GetDataFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u16* StartIndex, u16* DataBytes)
{
	u16 i;
	s32 LastIndex = -1;
	u8 Number = 0;
	
	for(i=0;i<Bytes;i++)
	{
		if(Separator == Buffer[i])
		{
			Number++;
			if(SeparatorNumber == Number)
			{
				*StartIndex = LastIndex+1;
				*DataBytes = i-LastIndex-1;
				return SUCCESS;
			}
			else
			{
				LastIndex = i;
			}
		}
	}
	return ERROR;//分隔符不足
}

/**
  *************************************************************************************
  * @brief  从分隔符中取10进制数值
  * @param  Separator：分隔符
  * @param  SeparatorNumber：分隔符序号
  * @param  Buffer：原始数据
  * @param  Bytes：原始数据长度
  * @param  Value：数值
  * @retval 无
  * @author ZH
  * @date   2015年1月06日
  * @note   “123\rABCD\rGL\r”,如想取出"123"，Separator SeparatorNumber 分别为'\r',1
  *************************************************************************************  
  */
ErrorStatus GetDecValueFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u32* Value)
{
	u16 i;
	s32 LastIndex = -1;
	u8 Number = 0;
	u8 temp[20];
	
	for(i=0;i<Bytes;i++)
	{
		if(Separator == Buffer[i])
		{
			Number++;
			if(SeparatorNumber == Number)
			{
				memset(temp,0,20);
				memcpy(&temp[0], &Buffer[LastIndex+1], i-LastIndex-1);
				sscanf((const char*)&temp[0],"%uld",Value);
				return SUCCESS;
			}
			else
			{
				LastIndex = i;
			}
		}
	}
	return ERROR;//分隔符不足
}

/**
  *************************************************************************************
  * @brief  串口3数据响应
  * @param  buffer_in：接收到的数据
  * @param  buffer_out：响应的数据
  * @retval 无
  * @author ZH
  * @date   2018年6月11日
  * @note   串口3主要是用来和上位机通讯的，校准及设置参数
  *************************************************************************************  
  */
void Uart1Respond(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 Cmd = 0, HostPack = 0;
	u32 StartAddr;
	
	memset((char*)buffer_out, 0, sizeof(STR_COMM));
	
	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == 'A') || (buffer_in->uch_buf[0] == 0x00)))
	{
		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
		switch(Cmd)
		{
			case 0x0000://读软件版本
			case 0x0100://读软件版本
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0101://开始更新程序, 擦除应用程序
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					if(EraseUserAppUpdate(APP_START_ADDRESS, APP_ADDRESS_SIZE))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
					}
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}			
				break;				
			case 0x0102://写入数据
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 9))
				{	
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					StartAddr = (buffer_in->uch_buf[5]<<24) + (buffer_in->uch_buf[6]<<16) + (buffer_in->uch_buf[7]<<8) + buffer_in->uch_buf[8];
                    
					if(ERROR == UserAppUpdate(StartAddr, &buffer_in->uch_buf[9], buffer_in->bytenum-9))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
				
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;				
				}			
				break;	
			case 0x0103://完成更新
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{												
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					if(EraseUserAppUpdate(BOOT_FLAG_START_ADDRESS, BOOT_FLAG_ADDRESS_SIZE))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					}
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
					}					

					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}			
				break;				
			case 0x0104://软件复位
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{							
					SoftReset();
				}			
				break;				
			default:
				break;
		}
		if(buffer_in->uch_buf[0] == 0x00)
		{
			buffer_out->bytenum = 0;
		}
	}
}
