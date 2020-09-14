#include "includes.h"

u8 dev_addr = 0;




/**
  *************************************************************************************
  * @brief  �����λ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��6��19��
  * @note   ��
  *************************************************************************************  
  */
void SoftReset(void)
{
	__set_FAULTMASK(1);// �ر������ж�
	NVIC_SystemReset();// ��λ
}
/**
  *************************************************************************************
  * @brief  ���Ե�ܽ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
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
  * @brief  ����GPIO
  * @param  ��
  * @retval �� 
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void SystemConfig(void)
{	
//	TestPointConfig();
	
	LedConfig();
	PackageKeyInit();
	//��������
//	UartConfig();
    Ch374HardWareConfig();
    CH374DeviceInit();	
}

