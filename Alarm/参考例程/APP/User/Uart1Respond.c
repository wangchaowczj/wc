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
	buffer->uch_buf[buffer->bytenum++] = DeviceInfo.name;
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
  * @brief  串口1数据响应
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
			case 0x0000://读软件版本
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;		
			case 0x0001://读ID
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&DeviceInfo.ID[0], sizeof(DeviceInfo.ID));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.ID[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}
				break;
			case 0x0002://写ID
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
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
					{//要写入的ID长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;
			case 0x0003://读硬件版本
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&DeviceInfo.HW[0], sizeof(DeviceInfo.HW));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.HW[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;
			case 0x0004://写硬件版本
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum >= 6) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
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
					{//要写入的ID长度太长
						buffer_out->uch_buf[buffer_out->bytenum++] = '3';								
					}
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}				
				break;	
			case 0x0005://关闭总线电压
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					CLOSE_HV_SW();
					SET_LIN_SHORT();
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
			case 0x0006://打开总线电压(低压)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					SET_LIN_OPEN();
					CLOSE_HV_SW();								
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;	
			case 0x0007://打开总线电压(高压)
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					SET_LIN_OPEN();
					OPEN_HV_SW();								
					
					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
				}				
				break;
				
			case 0x0008://总线电流
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
									
					U16Value1 = GetCurrentValue(&U16Value2);						
										
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r", U16Value1, U16Value2);					
				}				
				break;
			case 0x0009://总线电压
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
									
					U16Value1 = GetLinABVoltage(&U16Value2);						
										
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r", U16Value1, U16Value2);					
				}				
				break;				
			case 0x000A://读电流校准参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum  >  5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					if(U32Value == 0)
					{//电流
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
			case 0x000B://写AD校准参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 7) && (buffer_in->uch_buf[buffer_in->bytenum - 1] == 0x0D))
				{
					u8 item = 0;
					
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
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
					{//校准电流
						AdcAdjust.Current.k = U16Value1/1000.0;
						AdcAdjust.Current.b = U16Value2;					
					}
					else
					{
						AdcAdjust.Voltage.k = U16Value1/1000.0;
						AdcAdjust.Voltage.b = U16Value2;										
					}

					
					//校准时间
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
			case 0x000C://擦除AD校准参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据				

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
			case 0x000D://读测试参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据				
					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r%u\r", 
						TestParam.DeviceType, TestParam.DetType, 
						TestParam.LVCurrentMin, TestParam.LVCurrentMax, TestParam.LVCurrentTimeout,
						TestParam.HVCurrentMin, TestParam.HVCurrentMax, TestParam.HVCurrentTimeout,
						TestParam.FreqMin,TestParam.FreqMax,
						TestParam.CtrlMaskBit); 
				}			
				break;
			case 0x000E://设置测试参数
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据				
					
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
			case 0x00F0://进入Bootloader
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
