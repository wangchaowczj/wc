#include "includes.h"

volatile u32 Uart1RecTimeLatest = 0; 
volatile u32 Uart3RecTimeLatest  = 0; 


MSG_SEM MsgSem;
OP_DATA OpData;
DEVICE_INFO DeviceInfo;


/**
  *************************************************************************************
  * @brief  LED1ͨѶ����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
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
			ToggleTime = 100;//ͨѶʱ����
		}
		else
		{
			ToggleTime = 500;//��ͨѶʱ����
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
  * @brief  ����1ͨѶ����
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ����1�͵���ͨѶ
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
				Uart1Respond(&Uart1TempBuffer, &Uart1Data);//�Խ��յ������ݽ��д���
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
  * @brief  ������������
  * @param[in]  p_arg���������
  * @retval �� 
  * @author ZH
  * @date   2014��9��15��
  * @note   ��
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
			
            WriteLog("�������յ�����:", 0, NEW_STR_INFO);
			WriteLog(&Uart3Data.uch_buf[0], Uart3Data.bytenum, HEX_ARRAY);
            
			Uart3Respond(&Uart3Data, &Uart3TempBuffer);//��������
			
			if(Uart3TempBuffer.bytenum > 0)
			{
                WriteLog("��������Ӧ����:", 0, NEW_STR_INFO);
                WriteLog(&Uart3TempBuffer.uch_buf[0], Uart3TempBuffer.bytenum, HEX_ARRAY);
                
                Uart3SendData(Uart3TempBuffer.uch_buf, Uart3TempBuffer.bytenum);//��Ӧ����
			}			
			else
            {
                WriteLog("�����ڲ���Ӧ����", 0, NEW_STR_INFO);
            }
            
			Uart3Data.bytenum = 0;
			Uart3Data.status = RECIEVING;//���¿�ʼ����
		}  
	}
}
