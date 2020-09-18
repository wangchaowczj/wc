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
  * @brief  CH374��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��3��30��
  * @note   ������ʱ�ò�ѯ��ʽ����Ҫ������һ���߳�
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
//		Ch374DelayMs(50);  // �ȴ�CH374��λ���
//		
//		HostSetBusFree();  // �趨USB��������
//		while(1) 
//		{
//			SetHostEnumState(WAITING_INSERT);
//			if (GET_CH374_INT_PIN() == 0)
//			{
//				HostDetectInterrupt( );  // �����USB�����ж�����
//			}
//			if (Query374DeviceIn())
//			{
//				SetHostEnumState(DEVICE_INSERT);
//				break;  // ��USB�豸
//			}
//			Ch374DelayMs(1);
//		}
//		Ch374DelayMs(550);  // ����USB�豸�ղ�����δ�ȶ����ʵȴ�USB�豸���ٺ��룬������ζ���
//		if (GET_CH374_INT_PIN() == 0) 
//		{
//			HostDetectInterrupt( );  // �����USB�����ж�����
//		}
//		HostSetBusReset( );  // USB���߸�λ
//		c_time = OSTimeGet();
//		while(OSTimeGet() < (c_time+500/(1000/OS_TICKS_PER_SEC))) 
//		{  // �ȴ�USB�豸��λ����������
//			if (Query374DeviceIn())
//			{
//				break;  // ��USB�豸
//			}
//			Ch374DelayMs(1);
//		}
//		if (GET_CH374_INT_PIN() == 0) 
//		{
//			HostDetectInterrupt( );  // �����USB�����ж�����
//		}
//		if (Query374DeviceIn())
//		{// ��USB�豸
//			if ( Query374DevFullSpeed( ) ) 
//			{
//				HostSetFullSpeed( );  // ��⵽ȫ��USB�豸
//			}
//			else 
//			{
//				HostSetLowSpeed( );  // ��⵽����USB�豸
//			}
//		}
//		else 
//		{
//			continue;  // �豸�Ѿ��Ͽ�,�����ȴ�
//		}
//		c_time = OSTimeGet();
//		while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
//		{
//			Ch374DelayMs(50);  // �ȴ��豸��λ���ȶ�
//			if (Query374DeviceIn() == FALSE)
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//			}
//			s = GetDeviceDesInfo(buf, sizeof(buf));  // ��ȡ����������
//			if ( s == USB_INT_SUCCESS ) 
//			{
//				break;
//			}
//			HostSetBusReset( );  // USB���߸�λ
//			Ch374DelayMs(200);
//			if ( Query374DevFullSpeed( ) ) 
//			{
//				HostSetFullSpeed( );  // ��⵽ȫ��USB�豸
//			}
//			else 
//			{
//				HostSetLowSpeed( );  // ��⵽����USB�豸
//			}			
//		}
//		if( s != USB_INT_SUCCESS ) 
//		{
//			SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//			goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//		}

///* ������������������ȡ�˵�����/���˵��ַ/���˵��С�ȣ����±���endp_addr��endp_size�� */
//		mDiskInterfNumber = ( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceNumber;  /* �ӿں� */
//		mDiskBulkInEndp = 0;
//		mDiskBulkOutEndp = 0;	
//		if (( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass == 0x03)
//		{//HID
//			if((HostInfo.SupportDeviceType&SUPPORT_DEVICE_MOUSE) || (HostInfo.SupportDeviceType&SUPPORT_DEVICE_KEYBOARD))
//			{//֧�����
//				u16 i, j;
//				if ((( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceProtocol == 0x01 ) ||
//					(( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceProtocol == 0x02 ))
//				{//0x01������̣�0X02�������
//					for(i=0;i<( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL;i++) 
//					{
//						if((buf[i]==0x09)&&(buf[i+1]==0x04)&&(buf[i+5]==0x03)&&(buf[i+7]==0x01)||(buf[i+7]==0x02))  //�ӿ�������ΪHID����ꡢ����   
//						{ 
//							for(j=0;j<( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL-i;j++) 
//							{
//								if((buf[i+j]==0x07)&&(buf[i+j+1]==0x05)&&(buf[i+j+3]==0x03))
//								{//�ж��Ƿ�Ϊ�ж϶˵�
//									s=buf[i+j+2];
//									if ( s&0x80 )
//									{									
//										if(mDiskBulkInEndp == 0)
//										{
//											mDiskBulkInEndp = s&0x0f;         // IN�˵�ĵ�ַ
//										}									
//									}                                
//									else                                      // OUT�˵� 
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
//					{  /* ����USB�洢���豸,��֧�� */
//						SetHostEnumState(DEVICE_TYPE_ERR);
//						goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�
//					}					
//					s = SetUsbConfig( ( (PUSB_CFG_DESCR)buf ) -> bConfigurationValue );  // ����USB�豸����
//					if ( s != USB_INT_SUCCESS )
//					{
//						SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//						goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�
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
//						goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//					}
//					
//					//���½���HID��ļ򵥲���
//					s=Set_Idle( );                                        //����IDLE����������ǰ���HID���Э��������
//					if(s!=USB_INT_SUCCESS)
//					{
//						if((s&0x0f)==USB_INT_RET_STALL)
//						{//����STALL���ܱ���֧��
//							//goto next_operate1; //����STALL���ܱ���֧��
//						}
//					}
////next_operate1:
//					s= Get_Hid_Des(buf);                                  // ��ȡ����������������
//					if(s != USB_INT_SUCCESS)
//					{
//						goto WaitDeviceOut;                             //�����˳�
//					}

//					//���ڼ��̷�Set_Report��������,�����������Ҫ��һ��
//					if(SUPPORT_DEVICE_KEYBOARD == HostInfo.DeviceType)
//					{
//						buf[0]=0x01;
//						s=Set_Report(buf);                                 //���ñ���
//						if(s != USB_INT_SUCCESS)   
//						{ //���ñ������
//							if((s&0x0f) == USB_INT_RET_STALL)  
//							{//����STALL���ܱ���֧��
//								//goto next_operate2;      //����STALL���ܱ���֧��
//							}
//						}					
//					}					
//				}
//				else
//				{
//					SetHostEnumState(DEVICE_TYPE_ERR);
//					goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//				}				
//			}
//			else
//			{
//				SetHostEnumState(DEVICE_TYPE_ERR);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//			}
//		}
//		else if (((PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass == 0x08)
//		{//U��
//			for ( i = 0; i < 2; i ++ ) 
//			{  /* ����ǰ�����˵� */
//				if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].wMaxPacketSize == 64 && ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bmAttributes == 2 ) 
//				{  /* 64�ֽڳ��ȵ������˵� */
//					if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
//					{
//						mDiskBulkInEndp = ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* IN�˵� */
//					}
//					else
//					{
//						mDiskBulkOutEndp = ( (PUSB_CFG_DESCR_LONG)buf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* OUT�˵� */
//					}
//				}
//			}
//			if ( ( (PUSB_CFG_DESCR_LONG)buf ) -> itf_descr.bInterfaceClass != 0x08 || mDiskBulkInEndp == 0 || mDiskBulkOutEndp == 0 ) 
//			{  /* ����USB�洢���豸,��֧�� */
//				SetHostEnumState(DEVICE_TYPE_ERR);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�
//			}

//			s = SetUsbConfig( ( (PUSB_CFG_DESCR)buf ) -> bConfigurationValue );  // ����USB�豸����
//			if ( s != USB_INT_SUCCESS )
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�
//			}

//			s = mDiskInquiry( buf );  /* ��ȡ�������� */
//			if ( s != ERR_SUCCESS ) 
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�
//			}
//			
//			c_time = OSTimeGet();
//			while(OSTimeGet() < (c_time+10000/(1000/OS_TICKS_PER_SEC)))
//			{
//				Ch374DelayMs( 200 );
//				if (Query374DeviceIn() == FALSE)
//				{
//					SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//					goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//				}				
//				s = mDiskCapacity( buf );  /* ��ȡ�������� */
//				if ( s != ERR_SUCCESS ) 
//				{
//					mRequestSense( );
//				}
//				else 
//				{
//					HostInfo.Msc.BlackCount = ((u32)buf[0]<<24) + ((u32)buf[1]<<16) + ((u32)buf[2]<<8) + buf[3] + 1;//����߼����ַ+1
//					HostInfo.Msc.BlackSize = ((u32)buf[4]<<24) + ((u32)buf[5]<<16) + ((u32)buf[6]<<8) + buf[7];//���С					
//					break;
//				}
//			}
//			if(s != ERR_SUCCESS)
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�			
//			}
//			
//			s = mDiskTestReady( );  /* ���Դ����Ƿ���� */
//			if ( s != ERR_SUCCESS ) 
//			{
//				SetHostEnumState(ENUM_ERROR_WAIING_LEAVE);
//				goto WaitDeviceOut;  // ��ֹ����,�ȴ�USB�豸�γ�
//			}
//			HostInfo.DeviceType = SUPPORT_DEVICE_MSC;		
//		}
//		else
//		{//����USB�洢���豸
//			SetHostEnumState(DEVICE_TYPE_ERR);
//			goto WaitDeviceOut;  
//		}
//		Tog = FALSE;
//		SetHostEnumState(ENUM_SUCCESS);
//WaitDeviceOut:  // �ȴ�USB�豸�γ�
//		while (1) 
//		{
//			CH374Lock();//������U��ʱ�����ж��������
//			if (GET_CH374_INT_PIN() == 0)
//			{
//				HostDetectInterrupt();  // �����USB�����ж�����
//			}
//			s = Query374DeviceIn();
//			CH374Unlock();
//			if (s == FALSE)
//			{
//				break;  // û��USB�豸
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
//		Ch374DelayMs(100);  // �ȴ��豸��ȫ�Ͽ���������ζ���
//		CH374Lock();
//		s = Query374DeviceIn();
//		CH374Unlock();
//		if (s)
//		{
//			goto WaitDeviceOut;  // û����ȫ�Ͽ�
//		}		
//	}	 
}
/**
  *************************************************************************************
  * @brief  CH374�жϻص�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��7��21��
  * @note   ������ʱ�ò�ѯ��ʽ���ӻ����жϷ�ʽ
  *************************************************************************************  
  */
void CH374InterruptCallBack(void)
{
//	if(HostInfo.SupportDeviceType == 0)
//	{//������ʱ�ò�ѯ��ʽ���ӻ����жϷ�ʽ
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


