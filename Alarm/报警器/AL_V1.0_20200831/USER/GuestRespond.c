#include "includes.h"
/**
  *************************************************************************************
  * @file    GuestRespond.c
  * @author  
  * @version V1.0
  * @date    2020年8月28日
  * @brief   GuestRespond块源文件
  *************************************************************************************
  */
  
STR_COMM  GusetSendBuffer;
STR_COMM  GusetTempBuffer;

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
  * @brief  USB数据响应
  * @param  buffer_in：接收到的数据
  * @param  buffer_out：响应的数据
  * @retval 无
  * @author ZH
  * @date   2015年1月06日
  * @note   无
  *************************************************************************************  
  */
void  GuestRespond(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 Cmd = 0, HostPack = 0;
	u8 U8Value;
	u16 U16Value1, U16Value2, U16Value3;
	u32 U32Value;
    u8 Temp[100];

	memset((char*)buffer_out, 0, sizeof(STR_COMM));
	
	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == DeviceInfo.name) || (buffer_in->uch_buf[0] == 0x00)))
	{
		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
		switch(Cmd)
		{
			case 0x0000://读软件版本号
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
            case 0x0002://设置ID
                if(HostPack == 0xFFFF)
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
            case 0x0003://读硬件版本号
                if(HostPack == 0xFFFF)
                {
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					U16Value1 = GetBufferStringLenght(&DeviceInfo.HW[0], sizeof(DeviceInfo.HW));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.HW[0], U16Value1);
					
					buffer_out->bytenum += U16Value1;
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                }    
                 break; 
            case 0x0004://设置硬件版本号
                if(HostPack == 0xFFFF)
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
            case 0x0005://控制设备关闭总线电压
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
                    
                    OSTaskSuspend(APP_OLED_TASK_PRIO);
                    SW5V_L();
                    VSHIFT_L();
                                       
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                }
                 break; 
            case 0x0006://控制设备输出LV
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
                    
                    OSTaskSuspend(APP_OLED_TASK_PRIO);
                    VSHIFT_H();

                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                }
                 break; 
            case 0x0007://控制设备输出5V
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

                    OSTaskSuspend(APP_OLED_TASK_PRIO);
                    SW5V_H();
                    VSHIFT_L();

                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                }
                 break; 
            case 0x0008://读总线电流
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

                    OSTaskSuspend(APP_OLED_TASK_PRIO);
                    U16Value1 = GetBusCurrent(10, &U16Value2);					
//                    OSTaskResume(APP_OLED_TASK_PRIO);                    
                    
                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                    "%u\r%u\r", U16Value1, U16Value2); 
                }
                 break; 
            case 0x0009://读总线电压
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据

                    U16Value1 = GetBusVoltage(10, &U16Value2);	
//                    OSTaskResume(APP_OLED_TASK_PRIO);                    

                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                    "%u\r%u\r", U16Value1, U16Value2);                  
                }
                 break; 
            case 0x000A://读总线电压校准参数 读总线电流(小量程)校准参数 读总线电流(大量程)校准参数
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U8Value = (u16)U32Value;//K
					if(U8Value == 0)
					{
						U16Value1 = (u16)(adc_adjust.linab.k*1000);
						U16Value2 = adc_adjust.linab.b;	
                        U16Value3 = adc_adjust.linab.year + 2000;
                        Temp[0] = adc_adjust.linab.month;
                        Temp[1] = adc_adjust.linab.day;
                        Temp[2] = adc_adjust.linab.hour;
                        Temp[3] = adc_adjust.linab.minute;
                        Temp[4] = adc_adjust.linab.second;
                        
					}
					else if(U8Value == 1)
					{
						U16Value1 = (u16)(adc_adjust.current1.k*1000);
						U16Value2 = adc_adjust.current1.b;	
                        U16Value3 = adc_adjust.linab.year + 2000;
                        Temp[0] = adc_adjust.current1.month;
                        Temp[1] = adc_adjust.current1.day;
                        Temp[2] = adc_adjust.current1.hour;
                        Temp[3] = adc_adjust.current1.minute;
                        Temp[4] = adc_adjust.current1.second;                        
					}
					else if(U8Value == 2)
					{
						U16Value1 = (u16)(adc_adjust.current2.k*1000);
						U16Value2 = adc_adjust.current2.b;	
                        U16Value3 = adc_adjust.current2.year + 2000;
                        Temp[0] = adc_adjust.current2.month;
                        Temp[1] = adc_adjust.current2.day;
                        Temp[2] = adc_adjust.current2.hour;
                        Temp[3] = adc_adjust.current2.minute;
                        Temp[4] = adc_adjust.current2.second;                        
					}					
					else
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;	
                    }                       					
					buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%u\r%u\r%04d\r%02d\r%02d\r%02d\r%02d\r%02d\r", U16Value1, U16Value2, U16Value3,Temp[0],Temp[1],Temp[2],Temp[3],Temp[4]);

                }
                 break; 
            case 0x000B://设置总线电压校准参数 设置总线电流(小量程)校准参数 设置总线电流(大量程)校准参数
                if(HostPack == 0xFFFF)
                {
                GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据
					
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					U8Value = (u16)U32Value;//通道				
					
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
                    if(ERROR == GetDecValueFromSeparator('\r',4,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    Temp[0] = (u16)U32Value - 2000;	//年
                    
                    if(ERROR == GetDecValueFromSeparator('\r',5,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Temp[1] = (u16)U32Value;	//月
                    
                    if(ERROR == GetDecValueFromSeparator('\r',6,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Temp[2] = (u16)U32Value;	//日
                    
                    if(ERROR == GetDecValueFromSeparator('\r',7,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Temp[3] = (u16)U32Value;	//时
                    
                    if(ERROR == GetDecValueFromSeparator('\r',8,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Temp[4] = (u16)U32Value;	//分

					if(ERROR == GetDecValueFromSeparator('\r',9,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
					Temp[5] = (u16)U32Value;	//秒                    
										
					if(U8Value == 0)
					{
						adc_adjust.linab.k = U16Value1/1000.0;
						adc_adjust.linab.b = U16Value2;
                        adc_adjust.linab.year = Temp[0];
                        adc_adjust.linab.month = Temp[1];
                        adc_adjust.linab.day = Temp[2];
                        adc_adjust.linab.hour = Temp[3];
                        adc_adjust.linab.minute = Temp[4];
                        adc_adjust.linab.second = Temp[5];
                        OSTaskResume(APP_OLED_TASK_PRIO);
					}
					else if(U8Value == 1)
					{
						adc_adjust.current1.k = U16Value1/1000.0;
						adc_adjust.current1.b = U16Value2;
                        adc_adjust.current1.year = Temp[0];
                        adc_adjust.current1.month = Temp[1];
                        adc_adjust.current1.day = Temp[2];
                        adc_adjust.current1.hour = Temp[3];
                        adc_adjust.current1.minute = Temp[4];
                        adc_adjust.current1.second = Temp[5]; 
                        OSTaskResume(APP_OLED_TASK_PRIO);
					}
					else if(U8Value == 2)
					{
						adc_adjust.current2.k = U16Value1/1000.0;
						adc_adjust.current2.b = U16Value2;		
                        adc_adjust.current2.year = Temp[0];
                        adc_adjust.current2.month = Temp[1];
                        adc_adjust.current2.day = Temp[2];
                        adc_adjust.current2.hour = Temp[3];
                        adc_adjust.current2.minute = Temp[4];
                        adc_adjust.current2.second = Temp[5];
                        OSTaskResume(APP_OLED_TASK_PRIO);
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
            case 0x000C://删除电压校准参数  删除电流(小量程)校准参数 删除电流(大量程)校准参数
                if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);//返回数据	
                        
                    if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    U8Value = (u16)U32Value;//通道	
                    if(U8Value == 0)
                    {
                        adc_adjust.linab.k = 0;
                        adc_adjust.linab.b = 0;				
                    }
                    else if(U8Value == 1)
                    {
                        adc_adjust.current1.k = 0;
                        adc_adjust.current1.b = 0;				
                    }
                    else if(U8Value == 2)
                    {
                        adc_adjust.current2.k = 0;
                        adc_adjust.current2.b = 0;					
                    }       
                    else
                    {
                        memset((char*)&adc_adjust, 0, sizeof(ADC_ADJUST));
                    }					                                     
                    buffer_out->uch_buf[buffer_out->bytenum++] = '1';                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;					
                }
                 break; 
            case 0x00F0://控制设备重启后停留在bootload
                if(HostPack == 0xFFFF)
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

