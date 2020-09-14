#include "includes.h"

u8 dev_addr = 0;




/**
  *************************************************************************************
  * @brief  软件复位
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年6月19日
  * @note   无
  *************************************************************************************  
  */
void SoftReset(void)
{
	__set_FAULTMASK(1);// 关闭所有中端
	NVIC_SystemReset();// 复位
}
/**
  *************************************************************************************
  * @brief  测试点管脚配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年6月8日
  * @note   无
  *************************************************************************************  
  */
void TestPointConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(TEST_POINT1_PORT_RCC | TEST_POINT2_PORT_RCC, ENABLE);
	
	TEST_POINT1_OFF();
	TEST_POINT2_OFF();
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Pin = TEST_POINT1_PIN;
	GPIO_Init(TEST_POINT1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = TEST_POINT2_PIN;
	GPIO_Init(TEST_POINT2_PORT, &GPIO_InitStruct);
}

/**
  *************************************************************************************
  * @brief  配置GPIO
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2018年6月8日
  * @note   无
  *************************************************************************************  
  */
void SystemConfig(void)
{	
//	TestPointConfig();
	
	LedConfig();
	PackageKeyInit();
	//串口配置
//	UartConfig();
    Ch374HardWareConfig();
    CH374DeviceInit();	
}

