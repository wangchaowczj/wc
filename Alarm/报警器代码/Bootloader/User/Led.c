#include "includes.h"


/**
  *************************************************************************************
  * @brief  LED管脚配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年6月8日
  * @note   无
  *************************************************************************************  
  */
void LedConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC | LED2_PORT_RCC| LED3_PORT_RCC| RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//PA15 默认是JTAG下载引脚
	
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Pin = LED1_PIN;
	GPIO_Init(LED1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LED3_PIN;
	GPIO_Init(LED3_PORT, &GPIO_InitStruct);
	
}

