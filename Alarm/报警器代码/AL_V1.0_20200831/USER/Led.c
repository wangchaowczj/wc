#include "includes.h"
/**
  *************************************************************************************
  * @brief  LED配置设置
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2018年8月8日
  * @note   
  *************************************************************************************  
  */
void LEDConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC|LED3_PORT_RCC|RCC_APB2Periph_AFIO, ENABLE);	
    RCC_APB2PeriphClockCmd(LED3_PORT_RCC, ENABLE);
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Pin = LED1_PIN|LED4_PIN;	
	GPIO_Init(LED1_PORT, &GPIO_InitStruct);
    LED1_OFF();
    LED4_OFF();
    
	GPIO_InitStruct.GPIO_Pin = LED3_PIN;
    GPIO_Init(LED3_PORT, &GPIO_InitStruct);
    LED3_OFF();	

}

