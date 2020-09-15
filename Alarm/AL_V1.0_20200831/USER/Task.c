#include "includes.h"
/**
  *************************************************************************************
  * @file    Task.c
  * @author  
  * @version V1.0
  * @date    2020��8��28��
  * @brief   Task��Դ�ļ�
  *************************************************************************************
  */
  
__align(8) APP_PRIVATE_STK PrivateStk; //����ֽڶ���

MSG_SEM MsgSem;//�ź���
DEVICE_INFO DeviceInfo;//�豸��Ϣ
OP_DATA op_data;//״̬��־λ

/**
  *************************************************************************************
  * @brief  ��������
  * @param[in]  prio���������ȼ� 
  * @param[in]  p_arg���������
  * @retval ��� 
  * @author 
  * @date   2016��7��5��
  * @note   ��
  *************************************************************************************  
  */
INT8U TaskCreate(INT8U prio, void* p_arg)
{
	INT8U Result = OS_ERR_PRIO_INVALID;
	
	switch(prio)
	{
		case APP_STARTUP_TASK_PRIO:
			Result = OSTaskCreateExt(AppStartUpTask, (void*)p_arg,
						&PrivateStk.Startup[APP_STARTUP_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.Startup[0], APP_STARTUP_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);		
			break;
        case APP_LED_TASK_PRIO:
			Result = OSTaskCreateExt(AppLEDTask, (void*)p_arg,
						&PrivateStk.LED[APP_LED_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.LED[0], APP_LED_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);	
            break;
        case APP_OLED_TASK_PRIO:
			Result = OSTaskCreateExt(AppOLEDTask, (void*)p_arg,
						&PrivateStk.OLED[APP_OLED_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.OLED[0], APP_OLED_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); 
            break;
        case APP_USB_TASK_PRIO:
			Result = OSTaskCreateExt(AppUsbTask, (void*)p_arg,
						&PrivateStk.Usb[APP_USB_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.Usb[0], APP_USB_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); 
            break;
        case APP_TEST_TASK_PRIO:
			Result = OSTaskCreateExt(AppTestTask, (void*)p_arg,
						&PrivateStk.TEST[APP_TEST_TASK_STK_SIZE-1],prio,
						prio,&PrivateStk.TEST[0], APP_TEST_TASK_STK_SIZE,
						(void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); 
            break;	
		default:
			break;
	}
	return Result;	
}

/**
  *************************************************************************************
  * @brief  ������������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2018��3��1��
  * @note   ��
  *************************************************************************************  
  */
void AppStartUpTask(void* p_arg)
{   

    System72MDelay1ms(20);//Ӳ���ϵ��ٶ�̫������һ����ʱ�ȴ�Ӳ���ϵ磬����ֻ�����������ʱ�����ܹ���OSTimeDly����ʱ
    SystemGpioConfig();

    TaskCreate(APP_USB_TASK_PRIO, (void*)0);
    TaskCreate(APP_TEST_TASK_PRIO, (void*)0);
    TaskCreate(APP_OLED_TASK_PRIO, (void*)0);
    TaskCreate(APP_LED_TASK_PRIO, (void*)0);

    while(1)
    {   
    WatchDogFeed();        
    STMOpen();
    AutoPowerOff();
    OSTimeDly(1); 
    }
}
/**
  *************************************************************************************
  * @brief  LED��˸����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2020��8��28��
  * @note   ��
  *************************************************************************************  
  */
void AppLEDTask(void* p_arg)
{
    while(1)
    {
        LED1_TOGGLE();
        OSTimeDly(200);         
    }
}
/**
  *************************************************************************************
  * @brief OLED��ʾ����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2020��8��28��
  * @note   ��
  *************************************************************************************  
  */
void AppOLEDTask(void* p_arg)
{   
    while(1)
    {
        OLEDDisplay();
        OSTimeDly(1);        
    }
}
/**
  *************************************************************************************
  * @brief  USBͨ������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2020��8��28��
  * @note   ��
  *************************************************************************************  
  */
void AppUsbTask(void* p_arg)
{	
	INT8U Err;
		
	MsgSem.usb_rcv =  OSSemCreate(0);
	
	while(1)
	{
        UsbIdentify();
        OSSemPend(MsgSem.usb_rcv,0,&Err);        
		if(RECIEVED == HidData.status)
		{//usb
			if(SUCCESS == Unpack(&HidData, &GusetTempBuffer))
			{
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//�Խ��յ������ݽ��д���
				if(GusetSendBuffer.bytenum != 0)
				{
					Pack(&GusetSendBuffer, &GusetTempBuffer);
					DeviceHidSendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
				}				
			}
			HidData.bytenum = 0;
			HidData.status = RECIEVING;		
		}	
	}
}
/**
  *************************************************************************************
  * @brief  ��һ���׹����߲�������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author 
  * @date   2020��8��28��
  * @note   ��
  *************************************************************************************  
  */
void AppTestTask(void* p_arg)
{
    while(1)
   {
        op_data.IZero_status = BusCurrentIsZero();
        if((DetIsInsert() == 1) && (op_data.I_ST == 1))
        {
            Sound(2);            
        }
        OSTimeDly(1); 
    }       
}
