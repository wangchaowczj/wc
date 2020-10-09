/**
  *************************************************************************************
  * @file    Delay.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   ��ʱģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Delay
  *@brief ��ʱģ��
  *@{
  */

/**@defgroup Delay_Functions
  *@brief ����
  *@{
  */
  
/**
  *************************************************************************************
  * @brief  �δ�ʱ������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   10ms�ж�1��
  *************************************************************************************  
  */
void SysTickConfig(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.SYSCLK_Frequency/OS_TICKS_PER_SEC);//��ʼ����ʹ��SysTick��ʱ��
}
/**
  *************************************************************************************
  * @brief  ��ʱ����
  * @param  dly����ʱ΢����
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   �����ȵ���SysTickConfig��ʼ���δ�ʱ��
  *************************************************************************************  
  */
void System72MDelay1us(void)
{
	u8 i=4;
	
	while(i--);
}
/**
  *************************************************************************************
  * @brief  ��ʱ����
  * @param  dly����ʱ΢����
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   �����ȵ���SysTickConfig��ʼ���δ�ʱ��
  *************************************************************************************  
  */
void System72MDelay10us(u8 num)
{
	u16 i;

	for(i = 0; i<10*num;i++)
	{
		System72MDelay1us();	
	}

}
/**
  *************************************************************************************
  * @brief  ��ʱ����
  * @param  dly����ʱ΢����
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   �����ȵ���SysTickConfig��ʼ���δ�ʱ��
  *************************************************************************************  
  */
void System72MDelay100us(u8 num)
{
	u8 i;
	u16 j;
	
	for(i = 0; i<num;i++)
	{
		for(j=0; j<793;j++)
		{
			;
		}	
	}

}
void System72MDelay1ms(u16 num)
{
	u16 i;
	
	for(i = 0; i<num;i++)
	{
		System72MDelay100us(10);
	}

}

/**
  *@} Delay_Functions
  */
/**
  *@} Delay
  */
/**
  *@} PowerBoard
  */

