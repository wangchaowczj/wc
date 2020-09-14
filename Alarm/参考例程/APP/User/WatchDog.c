#include "includes.h"


/**
  *************************************************************************************
  * @brief  独立看门狗IWDG设置
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2018年8月8日
  * @note   时间大概是3秒,IWDG_ReloadCounter();//喂狗
  *************************************************************************************  
  */
void WatchDogInit(void) 
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* 
        独立看门狗的时钟是一个内部RC时钟，所以并不是准确的40Khz，
        而是在30~60Khz之间的一个可变化的时钟，只是我们在估算的时候，
        以40Khz的频率来计算，看门狗对时间的要求不是很精确
        IWDG counter clock: 32KHz(LSI) / 32 = 1KHz 
    */
    
    IWDG_SetPrescaler(IWDG_Prescaler_32); //独立看门狗预分频为32
    IWDG_SetReload(3000);//设置IWDG重装载值 范围为 0~0x0FFF;3000对应时间大概是3秒
    IWDG_ReloadCounter();//按照重装载的寄存器的值来重装载IWDG计数器
    IWDG_Enable();//使能 独立看门狗
}
/**
  *************************************************************************************
  * @brief  独立看门狗,喂狗
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2018年8月8日
  * @note   
  *************************************************************************************  
  */
void WatchDogFeed(void) 
{
    IWDG_ReloadCounter();
}

