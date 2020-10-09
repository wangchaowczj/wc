#include "includes.h"

HOST_INFO HostInfo;

//static  OS_EVENT   *CH374Mutex = 0;

//void CH374Lock(void)
//{
//	INT8U  err;
//	
//	if(CH374Mutex == 0)
//	{
//		CH374Mutex = OSMutexCreate(RES_CH374_MUTEX_PRIO, &err);
//	}	
//	OSMutexPend(CH374Mutex,0,&err);
//}
//void CH374Unlock(void)
//{
//	if(CH374Mutex != 0)
//	{
//		OSMutexPost(CH374Mutex);	
//	}
//}

void SetHostEnumState(HOST_ENUM_STATE State)
{
	HostInfo.EnumState = State;
}

HOST_ENUM_STATE GetHostEnumState(void)
{
	return HostInfo.EnumState;
}
/**
  *************************************************************************************
  * @brief  CH374主机流程
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年3月30日
  * @note   做主机时用查询方式，需要单独开一个线程
  *************************************************************************************  
  */
void HostProcess(void)
{
//	UINT8	i, s;
//	UINT8 	buf[64];
//	u32 c_time;
//	u8 Tog = FALSE;
//	
//	while (1) 
//	{
//		mSaveDevEndpTog = 0;
//		Ch374HostInit(HostInfo.SupportDeviceType);
//		
//		HostSetBusReset();
//		Ch374DelayMs(50);  // 等待CH374复位完成
//		
//		HostSetBusFree();  // 设定USB主机空闲
//		while(1) 
//		{
//			SetHostEnumState(WAITING_INSERT);
//			if (GET_CH374_INT_PIN() == 0)
//			{
//				HostDetectInterrupt( );  // 如果有USB主机中断则处理
//			}
//			if (Query374DeviceIn())
//			{
//				SetHostEnumState(DEVICE_INSERT);
//				break;  // 有USB设备
//			}
//			Ch374DelayMs(1);
//		}
//		Ch374DelayMs(550);  // 由于USB设备刚插入尚未稳定，故等待USB设备数百毫秒，消除插拔抖动
//		if (GET_CH374_INT_PIN() == 0) 
//		{
//			HostDetectInterrupt( );  // 如果有USB主机中断则处理
//		}
//		HostSetBusReset( );  // USB总线复位
//		c_time = OSTimeGet();
//		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC))) 
//		{  // 等待USB设备复位后重新连接
//			if (Query374DeviceIn())
//			{
//				break;  // 有USB设备
//			}
//			Ch374DelayMs(1);
//		}
//		if (GET_CH374_INT_PIN() == 0) 
//		{
//			HostDetectInterrupt( );  // 如果有USB主机中断则处理
//		}
//		if (Query374DeviceIn())
//		{// 有USB设备
//			if ( Query374DevFullSpeed( ) ) 
//			{
//				HostSetFullSpeed( );  // 检测到全速USB设备
//			}
//			else 
//			{
//				HostSetLowSpeed( );  // 检测到低速USB设备
//			}
//		}
//		else 
//		{
//			continue;  // 设备已经断开,继续等待
//		}
//		c_time = OSTimeGet();
//		while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
//		{
//			Ch374DelayMs(50);  // 等待设备复位后稳定
//			if (Query374DeviceIn() == FALSE)
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//			}
//			s = GetDeviceDesInfo(buf, sizeof(buf));  // 获取配置描述符
//			if ( s == USB_INT_SUCCESS ) 
//			{
//				break;
//			}
//			HostSetBusReset( );  // USB总线复位
//			Ch374DelayMs(200);
//			if ( Query374DevFullSpeed( ) ) 
//			{
//				HostSetFullSpeed( );  // 检测到全速USB设备
//			}
//			else 
//			{
//				HostSetLowSpeed( );  // 检测到低速USB设备
//			}			
//		}
//		if( s != USB_INT_SUCCESS ) 
//		{
//			SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//			goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//		}

///* 分析配置描述符，获取端点数据/各端点地址/各端点大小等，更新变量endp_addr和endp_size等 */
//		mDiskInterfNumber = ( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceNumber;  /* 接口号 */
//		mDiskBulkInEndp = 0;
//		mDiskBulkOutEndp = 0;	
//		if (( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass == 0x03)
//		{//HID
//			if((HostInfo.SupportDeviceType&SUPPORT_DEVICE_MOUSE) || (HostInfo.SupportDeviceType&SUPPORT_DEVICE_KEYBOARD))
//			{//支持鼠标
//				u16 i, j;
//				if ((( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceProtocol == 0x01 ) ||
//					(( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceProtocol == 0x02 ))
//				{//0x01代表键盘，0X02代表鼠标
//					for(i=0;i<( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL;i++) 
//					{
//						if((buf[i]==0x09)&&(buf[i+1]==0x04)&&(buf[i+5]==0x03)&&(buf[i+7]==0x01)||(buf[i+7]==0x02))  //接口描述符为HID的鼠标、键盘   
//						{ 
//							for(j=0;j<( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL-i;j++) 
//							{
//								if((buf[i+j]==0x07)&&(buf[i+j+1]==0x05)&&(buf[i+j+3]==0x03))
//								{//判断是否为中断端点
//									s=buf[i+j+2];
//									if ( s&0x80 )
//									{									
//										if(mDiskBulkInEndp == 0)
//										{
//											mDiskBulkInEndp = s&0x0f;         // IN端点的地址
//										}									
//									}                                
//									else                                      // OUT端点 
//									{
//										if(mDiskBulkOutEndp == 0)
//										{
//											mDiskBulkOutEndp = s&0x0f;
//										}									
//									}								
//								}       

//							   if((mDiskBulkOutEndp != 0)&&(mDiskBulkInEndp != 0))   break;                                       
//							}
//						}          
//					}		

//					if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass != 0x03 || (mDiskBulkInEndp == 0 && mDiskBulkOutEndp == 0 )) 
//					{  /* 不是USB存储类设备,不支持 */
//						SetHostEnumState(DEVICE_TYPE_ERR);
//						goto WaitDeviceOut;  // 终止操作,等待USB设备拔出
//					}					
//					s = SetUsbConfig( ( (PUSB_CFG_DESCR)buf ) -> bConfigurationValue );  // 设置USB设备配置
//					if ( s != USB_INT_SUCCESS )
//					{
//						SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//						goto WaitDeviceOut;  // 终止操作,等待USB设备拔出
//					}
//					if((HostInfo.SupportDeviceType&SUPPORT_DEVICE_KEYBOARD) && (( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceProtocol == 0x01 ))
//					{
//						HostInfo.DeviceType = SUPPORT_DEVICE_KEYBOARD;
//					}
//					else if((HostInfo.SupportDeviceType&SUPPORT_DEVICE_MOUSE) && (( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceProtocol == 0x02 ))
//					{
//						HostInfo.DeviceType = SUPPORT_DEVICE_MOUSE;
//					}
//					else
//					{
//						SetHostEnumState(DEVICE_TYPE_ERR);
//						goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//					}
//					
//					//以下进行HID类的简单操作
//					s=Set_Idle( );                                        //设置IDLE，这个步骤是按照HID类的协议来做的
//					if(s!=USB_INT_SUCCESS)
//					{
//						if((s&0x0f)==USB_INT_RET_STALL)
//						{//返回STALL可能本身不支持
//							//goto next_operate1; //返回STALL可能本身不支持
//						}
//					}
////next_operate1:
//					s= Get_Hid_Des(buf);                                  // 获取报表描述符描述符
//					if(s != USB_INT_SUCCESS)
//					{
//						goto WaitDeviceOut;                             //出错退出
//					}

//					//对于键盘发Set_Report来点亮灯,对于鼠标则不需要这一步
//					if(SUPPORT_DEVICE_KEYBOARD == HostInfo.DeviceType)
//					{
//						buf[0]=0x01;
//						s=Set_Report(buf);                                 //设置报表
//						if(s != USB_INT_SUCCESS)   
//						{ //设置报告出错
//							if((s&0x0f) == USB_INT_RET_STALL)  
//							{//返回STALL可能本身不支持
//								//goto next_operate2;      //返回STALL可能本身不支持
//							}
//						}					
//					}					
//				}
//				else
//				{
//					SetHostEnumState(DEVICE_TYPE_ERR);
//					goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//				}				
//			}
//			else
//			{
//				SetHostEnumState(DEVICE_TYPE_ERR);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//			}
//		}
//		else if (((PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass == 0x08)
//		{//U盘
//			for ( i = 0; i < 2; i ++ ) 
//			{  /* 分析前两个端点 */
//				if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].wMaxPacketSize == 64 && ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bmAttributes == 2 ) 
//				{  /* 64字节长度的批量端点 */
//					if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
//					{
//						mDiskBulkInEndp = ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* IN端点 */
//					}
//					else
//					{
//						mDiskBulkOutEndp = ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* OUT端点 */
//					}
//				}
//			}
//			if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass != 0x08 || mDiskBulkInEndp == 0 || mDiskBulkOutEndp == 0 ) 
//			{  /* 不是USB存储类设备,不支持 */
//				SetHostEnumState(DEVICE_TYPE_ERR);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出
//			}

//			s = SetUsbConfig( ( (PUSB_CFG_DESCR)buf ) -> bConfigurationValue );  // 设置USB设备配置
//			if ( s != USB_INT_SUCCESS )
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出
//			}

//			s = mDiskInquiry( buf );  /* 获取磁盘特性 */
//			if ( s != ERR_SUCCESS ) 
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出
//			}
//			
//			c_time = OSTimeGet();
//			while(OSTimeGet() < (c_time+10000/(1000/OS_TICKS_PER_SEC)))
//			{
//				Ch374DelayMs( 200 );
//				if (Query374DeviceIn() == FALSE)
//				{
//					SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//					goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//				}				
//				s = mDiskCapacity( buf );  /* 获取磁盘容量 */
//				if ( s != ERR_SUCCESS ) 
//				{
//					mRequestSense( );
//				}
//				else 
//				{
//					HostInfo.Msc.BlackCount = ((u32)buf[0]<<24) + ((u32)buf[1]<<16) + ((u32)buf[2]<<8) + buf[3] + 1;//最大逻辑块地址+1
//					HostInfo.Msc.BlackSize = ((u32)buf[4]<<24) + ((u32)buf[5]<<16) + ((u32)buf[6]<<8) + buf[7];//块大小					
//					break;
//				}
//			}
//			if(s != ERR_SUCCESS)
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出			
//			}
//			
//			s = mDiskTestReady( );  /* 测试磁盘是否就绪 */
//			if ( s != ERR_SUCCESS ) 
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // 终止操作,等待USB设备拔出
//			}
//			HostInfo.DeviceType = SUPPORT_DEVICE_MSC;		
//		}
//		else
//		{//不是USB存储类设备
//			SetHostEnumState(DEVICE_TYPE_ERR);
//			goto WaitDeviceOut;  
//		}
//		Tog = FALSE;
//		SetHostEnumState(ENUM_SUCCESS);
//WaitDeviceOut:  // 等待USB设备拔出
//		while (1) 
//		{
//			CH374Lock();//主机连U盘时，会有多任务访问
//			if (GET_CH374_INT_PIN() == 0)
//			{
//				HostDetectInterrupt();  // 如果有USB主机中断则处理
//			}
//			s = Query374DeviceIn();
//			CH374Unlock();
//			if (s == FALSE)
//			{
//				break;  // 没有USB设备
//			}
//			Ch374DelayMs(1);
//			if(GetHostEnumState() == ENUM_SUCCESS)
//			{				
//				switch(HostInfo.DeviceType)
//				{
//					case SUPPORT_DEVICE_MSC:
//						break;
//					case SUPPORT_DEVICE_KEYBOARD:	
//						do
//						{
//							CH374Lock();
//							s = CH374HostScanKeyBoardProcess(&Tog);
//							CH374Unlock();
//							if(1 == s)
//							{
//								SetHostEnumState(DEVICE_TYPE_ERR);
//							}
//							else if(2 == s)
//							{
//								OSTimeDly(9);
//							}						
//						}
//						while(s == 0);
//						break;							
//					case SUPPORT_DEVICE_MOUSE:
//						CH374Lock();
//						s = CH374HostMouseProcess(&mSaveDevEndpTog);
//						CH374Unlock();
//						if(1 == s)
//						{
//							SetHostEnumState(DEVICE_TYPE_ERR);
//						}						
//						break;
//					default:
//						break;
//				}
//			}
//		}
//		Ch374DelayMs(100);  // 等待设备完全断开，消除插拔抖动
//		CH374Lock();
//		s = Query374DeviceIn();
//		CH374Unlock();
//		if (s)
//		{
//			goto WaitDeviceOut;  // 没有完全断开
//		}		
//	}	 
}
/**
  *************************************************************************************
  * @brief  CH374中断回调函数
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年7月21日
  * @note   做主机时用查询方式，从机用中断方式
  *************************************************************************************  
  */
void CH374InterruptCallBack(void)
{
//	if(HostInfo.SupportDeviceType == 0)
//	{//做主机时用查询方式，从机用中断方式
//		while(GET_CH374_INT_PIN() == 0)
//		{
//			CH374DeviceInterrupt();
//		}	
//	}	
    		while(GET_CH374_INT_PIN() == 0)
		{
			CH374DeviceInterrupt();
		}
}


