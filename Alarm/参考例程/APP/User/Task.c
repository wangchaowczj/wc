#include "includes.h"

volatile u32 Uart1RecTimeLatest = 0; 
volatile u32 Uart3RecTimeLatest  = 0; 


MSG_SEM MsgSem;
OP_DATA OpData;
DEVICE_INFO DeviceInfo;


/**
  *************************************************************************************
  * @brief  LED1通讯任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   
  *************************************************************************************  
  */
void Led1Task(void* p_arg)
{
	u16 ToggleTime = 500;
	u32 TimeLatest = 0, NowTime;
	
    WatchDogInit();
	while(1)
	{
        WatchDogFeed();
        
		OSTimeDly(1);
		NowTime = OSTimeGet();
		if(((NowTime - Uart1RecTimeLatest) < 3000) || ((NowTime - Uart3RecTimeLatest) < 3000))
		{
			ToggleTime = 100;//通讯时快闪
		}
		else
		{
			ToggleTime = 500;//不通讯时慢闪
		}
		
		if(NowTime > (TimeLatest+ToggleTime))
		{
			LED1_TOGGLE();
			TimeLatest = NowTime;
		}
	}	
}

/**
  *************************************************************************************
  * @brief  串口1通讯任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   串口1和电脑通讯
  *************************************************************************************  
  */
void Uart1Task(void* p_arg)
{
	INT8U Err;

	Uart1Data.bytenum = 0;
	Uart1Data.status = RECIEVING;		
	MsgSem.Uart1 =  OSSemCreate(0);
	
	while(1)
	{
		OSSemPend(MsgSem.Uart1, 0, &Err);
		if(RECIEVED == Uart1Data.status)
		{
			if(SUCCESS == Unpack(&Uart1Data, &Uart1TempBuffer))
			{
				Uart1Respond(&Uart1TempBuffer, &Uart1Data);//对接收到的数据进行处理
				if(Uart1Data.bytenum != 0)
				{
					Pack(&Uart1Data, &Uart1TempBuffer);
					Uart1SendData(&Uart1TempBuffer.uch_buf[0], Uart1TempBuffer.bytenum);
				}				
			}
			Uart1Data.bytenum = 0;
			Uart1Data.status = RECIEVING;		
		}
	}	
}

/**
  *************************************************************************************
  * @brief  程序启动任务
  * @param[in]  p_arg：任务参数
  * @retval 无 
  * @author ZH
  * @date   2014年9月15日
  * @note   无
  *************************************************************************************  
  */
void AppStartUpTask(void* p_arg)
{	
	SystemConfig();	
    
	if(ERROR == HardwareInit())
    {
        LED1_ON();
        LED2_ON();
        LED3_ON();
        while(1)
        {
            LED1_TOGGLE();
            LED2_TOGGLE();
            LED3_TOGGLE();
            SystemDelay1ms(100);
        }
    }
    
	TaskCreate(APP_LED1_TASK_PRIO, (void*)0);		
	TaskCreate(APP_UART1_TASK_PRIO, (void*)0);	

	Uart3Data.bytenum = 0;
	Uart3Data.status = RECIEVING;
	
	while(1)
	{
		OSTimeDly(1);

		if((Uart3Data.bytenum > 0) && (Uart3Data.status == RECIEVING) && (OSTimeGet() - Uart3RecTimeLatest > 5))
		{
			Uart3Data.status = RECIEVED;
			
            WriteLog("主串口收到数据:", 0, NEW_STR_INFO);
			WriteLog(&Uart3Data.uch_buf[0], Uart3Data.bytenum, HEX_ARRAY);
            
			Uart3Respond(&Uart3Data, &Uart3TempBuffer);//解析数据
			
			if(Uart3TempBuffer.bytenum > 0)
			{
                WriteLog("主串口响应数据:", 0, NEW_STR_INFO);
                WriteLog(&Uart3TempBuffer.uch_buf[0], Uart3TempBuffer.bytenum, HEX_ARRAY);
                
                Uart3SendData(Uart3TempBuffer.uch_buf, Uart3TempBuffer.bytenum);//响应数据
			}			
			else
            {
                WriteLog("主串口不响应数据", 0, NEW_STR_INFO);
            }
            
			Uart3Data.bytenum = 0;
			Uart3Data.status = RECIEVING;//重新开始接收
		}  
	}
}
