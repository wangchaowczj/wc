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
	buffer->uch_buf[buffer->bytenum++] = DeviceInfo.name;
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
  * @brief  ����1������Ӧ
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
//	static u16 GuestPack = 0, LastValue;
	//u8 U8Value;
	u16 U16Value1, U16Value2;
	u32 U32Value;
	u8 Temp1[100]/*, Temp2[100]*/;
	
	memset((char*)buffer_out, 0, sizeof(STR_COMM));
	
	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == DeviceInfo.name) || (buffer_in->uch_buf[0] == 0x00)))
	{
		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
		switch(Cmd)
		{
			case 0x0000://������汾
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;		
			case 0x0001://��ID
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&DeviceInfo.ID[0], sizeof(DeviceInfo.ID));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.ID[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}
				break;
			case 0x0002://дID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(DeviceInfo.ID))
					{
						memset(&DeviceInfo.ID[0], 0, sizeof(DeviceInfo.ID));
						if (U16Value1 > 0)
						{
							memcpy((char*)&DeviceInfo.ID[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}						

						if(ERROR != WriteFactoryConfigParam())
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
						}
					}
					else
					{//Ҫд���ID����̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0003://��Ӳ���汾
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&DeviceInfo.HW[0], sizeof(DeviceInfo.HW));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.HW[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0004://дӲ���汾
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					U16Value1 = GetBufferStringLenght(&buffer_in->uch_buf[5], buffer_in->bytenum - 6);
					if(U16Value1 <= sizeof(DeviceInfo.HW))
					{
						memset(&DeviceInfo.HW[0], 0, sizeof(DeviceInfo.HW));
						if (U16Value1 > 0)
						{
							memcpy((char*)&DeviceInfo.HW[0], (char*)&buffer_in->uch_buf[5], U16Value1);
						}						

						if(ERROR != WriteFactoryConfigParam())
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '1';								
						}
						else
						{
							buffer_out->uch_buf[buffer_out->bytenum++] = '2';								
						}
					}
					else
					{//Ҫд���ID����̫��
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;	
			case 0x0005://�ر����ߵ�ѹ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					CLOSE_HV_SW();
					SET_LIN_SHORT();
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
			case 0x0006://�����ߵ�ѹ(��ѹ)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					SET_LIN_OPEN();
					CLOSE_HV_SW();								
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;	
			case 0x0007://�����ߵ�ѹ(��ѹ)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					SET_LIN_OPEN();
					OPEN_HV_SW();								
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
				
			case 0x0008://���ߵ���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
									
					U16Value1 = GetCurrentValue(&U16Value2);						
										
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r", U16Value1, U16Value2);					
				}				
				break;
			case 0x0009://���ߵ�ѹ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
									
					U16Value1 = GetLinABVoltage(&U16Value2);						
										
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r", U16Value1, U16Value2);					
				}				
				break;				
			case 0x000A://������У׼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum  >  5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������

					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					if(U32Value == 0)
					{//����
						U16Value1 = (u16)(AdcAdjust.Current.k*1000);
						U16Value2 = AdcAdjust.Current.b;					
					}
					else
					{
						U16Value1 = (u16)(AdcAdjust.Voltage.k*1000);
						U16Value2 = AdcAdjust.Voltage.b;										
					}


					memset(&Temp1[0], 0, sizeof(Temp1));
					memcpy(&Temp1[0], &AdcAdjust.Time[0], sizeof(AdcAdjust.Time));
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r", 
						U16Value1, U16Value2,
						2000+Temp1[0],Temp1[1],Temp1[2],Temp1[3],Temp1[4],Temp1[5]);
				}			
				break;
			case 0x000B://дADУ׼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					u8 item = 0;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}	
					item = (u8)U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value1 = (u16)U32Value;//K
					
					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U16Value2 = (u16)U32Value;	//B	
					
					if(item == 0)
					{//У׼����
						AdcAdjust.Current.k = U16Value1/1000.0;
						AdcAdjust.Current.b = U16Value2;					
					}
					else
					{
						AdcAdjust.Voltage.k = U16Value1/1000.0;
						AdcAdjust.Voltage.b = U16Value2;										
					}

					
					//У׼ʱ��
					if(ERROR == GetDecValueFromSeparator('\r',4,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					AdcAdjust.Time[0] = U32Value-2000;
					
					if(ERROR == GetDecValueFromSeparator('\r',5,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					AdcAdjust.Time[1] = U32Value;
					if(ERROR == GetDecValueFromSeparator('\r',6,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					AdcAdjust.Time[2] = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',7,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					AdcAdjust.Time[3] = U32Value;
					if(ERROR == GetDecValueFromSeparator('\r',8,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					AdcAdjust.Time[4] = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',9,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					AdcAdjust.Time[5] = U32Value;					

		
					if(ERROR == WriteFactoryConfigParam())
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
			case 0x000C://����ADУ׼����
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������				

					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					if(U32Value == 0)
					{
						AdcAdjust.Current.k = 0;
						AdcAdjust.Current.b = 0;					
					}
					else
					{
						AdcAdjust.Voltage.k = 0;
						AdcAdjust.Voltage.b = 0;					
					}


					if(ERROR == WriteFactoryConfigParam())
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
			case 0x000D://�����Բ���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������				
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r", 
						TestParam.DeviceType, TestParam.DetType, 
						TestParam.LVCurrentMin, TestParam.LVCurrentMax, TestParam.LVCurrentTimeout,
						TestParam.HVCurrentMin, TestParam.HVCurrentMax, TestParam.HVCurrentTimeout,
						TestParam.FreqMin,TestParam.FreqMax,
						TestParam.CtrlMaskBit); 
				}			
				break;
			case 0x000E://���ò��Բ���
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������				
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.DeviceType = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',2,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.DetType = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',3,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.LVCurrentMin = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',4,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.LVCurrentMax = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',5,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.LVCurrentTimeout = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',6,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.HVCurrentMin = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',7,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.HVCurrentMax = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',8,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.HVCurrentTimeout = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',9,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.FreqMin = U32Value;	

					if(ERROR == GetDecValueFromSeparator('\r',10,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.FreqMax = U32Value;
					
					if(ERROR == GetDecValueFromSeparator('\r',11,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					TestParam.CtrlMaskBit = U32Value;	
					
					if(ERROR == WriteFactoryConfigParam())
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
			case 0x00F0://����Bootloader
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{							
					SetStayInBoot();
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
