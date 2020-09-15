#include "includes.h"
/**
  *************************************************************************************
  * @file    Task.c
  * @author  
  * @version V1.0
  * @date    2020年8月28日
  * @brief   Task块源文件
  *************************************************************************************
  */
  
__align(8) APP_PRIVATE_STK PrivateStk; //汇编字节对齐

MSG_SEM MsgSem;//信号量
DEVICE_INFO DeviceInfo;//设备信息
OP_DATA op_data;//状态标志位

/**
  *************************************************************************************
  * @brief  创建任务
  * @param[in]  prio：任务优先级 
  * @param[in]  p_arg：任务参数
  * @retval 结果 
  * @author 
  * @date   2016年7月5日
  * @note   无
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
  * @brief  程序启动任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2018年3月1日
  * @note   无
  *************************************************************************************  
  */
void AppStartUpTask(void* p_arg)
{   

    System72MDelay1ms(20);//硬件上电速度太慢，加一个延时等待硬件上电，这里只能用这个来延时，不能够用OSTimeDly来延时
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
  * @brief  LED闪烁任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2020年8月28日
  * @note   无
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
  * @brief OLED显示任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2020年8月28日
  * @note   无
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
  * @brief  USB通信任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2020年8月28日
  * @note   无
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
				GuestRespond(&GusetTempBuffer, &GusetSendBuffer);//对接收到的数据进行处理
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
  * @brief  第一发雷管上线测试任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author 
  * @date   2020年8月28日
  * @note   无
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
