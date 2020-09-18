#include "includes.h"

STR_COMM  Uart1Data;
STR_COMM  Uart1TempBuffer;

/**
  *************************************************************************************
  * @brief  ��ȡ�������Ӧ����
  * @param  buffer�����ݻ�����
  * @param  Cmd������
  * @param  PackIndex���ְ���  
  * @retval �� 
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
  * @brief  ��ȡ�������е��ַ�������
  * @param  buffer�����ݻ�����
  * @param  MaxLenght�����ݻ��������� 
  * @retval �ַ������� 
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
  * @brief  �ӻ������е���ֵ
  * @param  buffer�����ݻ�����
  * @param  MaxLenght�����ݻ��������� 
  * @retval ��ֵ
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
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
  * @brief  �ӷָ�����ȡ����
  * @param  Separator���ָ���
  * @param  SeparatorNumber���ָ������
  * @param  Buffer��ԭʼ����
  * @param  Bytes��ԭʼ���ݳ���
  * @param  StartIndex���õ���������Buffer�е���ʼ����ֵ
  * @param  DataBytes���õ������ݳ���  
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��123\rABCD\rGL\r��,����ȡ��"ABCD"��Separator SeparatorNumber �ֱ�Ϊ'\r',2
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
	return ERROR;//�ָ�������
}

/**
  *************************************************************************************
  * @brief  �ӷָ�����ȡ10������ֵ
  * @param  Separator���ָ���
  * @param  SeparatorNumber���ָ������
  * @param  Buffer��ԭʼ����
  * @param  Bytes��ԭʼ���ݳ���
  * @param  Value����ֵ
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��123\rABCD\rGL\r��,����ȡ��"123"��Separator SeparatorNumber �ֱ�Ϊ'\r',1
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
	return ERROR;//�ָ�������
}

/**
  *************************************************************************************
  * @brief  ����3������Ӧ
  * @param  buffer_in�����յ�������
  * @param  buffer_out����Ӧ������
  * @retval ��
  * @author ZH
  * @date   2018��6��11��
  * @note   ����3��Ҫ����������λ��ͨѶ�ģ�У׼�����ò���
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
			case 0x0000://������汾
			case 0x0100://������汾
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0101://��ʼ���³���, ����Ӧ�ó���
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
			case 0x0102://д������
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
			case 0x0103://��ɸ���
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
			case 0x0104://�����λ
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
