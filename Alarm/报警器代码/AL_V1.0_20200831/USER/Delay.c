/**
  *************************************************************************************
  * @file    Delay.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   延时模块源文件
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Delay
  *@brief 延时模块
  *@{
  */

/**@defgroup Delay_Functions
  *@brief 函数
  *@{
  */
  
/**
  *************************************************************************************
  * @brief  滴答定时器配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   10ms中断1次
  *************************************************************************************  
  */
void SysTickConfig(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.SYSCLK_Frequency/OS_TICKS_PER_SEC);//初始化并使能SysTick定时器
}
/**
  *************************************************************************************
  * @brief  延时函数
  * @param  dly：延时微秒数
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   必需先调用SysTickConfig初始化滴答定时器
  *************************************************************************************  
  */
void System72MDelay1us(void)
{
	u8 i=4;
	
	while(i--);
}
/**
  *************************************************************************************
  * @brief  延时函数
  * @param  dly：延时微秒数
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   必需先调用SysTickConfig初始化滴答定时器
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
  * @brief  延时函数
  * @param  dly：延时微秒数
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   必需先调用SysTickConfig初始化滴答定时器
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

