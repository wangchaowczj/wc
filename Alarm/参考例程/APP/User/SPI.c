#include "includes.h"

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SPI
  *@{
  */
/**@defgroup SPI_Functions
  *@{
  */

/**
  *************************************************************************************
  * @brief  SPI1����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
void Spi1Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	RCC_APB2PeriphClockCmd(SPI1_CS_PORT_RCC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStruct.GPIO_Pin = SPI1_CS_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI1_CS_PORT, &GPIO_InitStruct);
	
	/* ����SPI1�������� */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	
	/*!< SPI1 configuration */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;        // ��72Mhz����Ƶ�£�APB1Ϊ36Mhz(2��Ƶ)��������8��Ƶ��Լ4.5MHz
																	// ��ΪSD����SPIģʽ��Ҫ��100-400kHz���ٶ��³�ʼ������ʹ��Ϊ
																	// 256��Ƶ�������ƵҲ���ܳ���102.4MHz��
																	// �豸��ʱ��SPI�ڹ���ʱΪ2��Ƶ��SD��ʼ��ʱΪ256��Ƶ

	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStruct);

	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(SPI1, ENABLE);	
}	

/**
  *************************************************************************************
  * @brief  ����Spi1������
  * @param  baud������,ȡֵ���£�
  *         @arg @b SPI_BaudRatePrescaler_2
  *         @arg @b SPI_BaudRatePrescaler_4
  *         @arg @b SPI_BaudRatePrescaler_8
  *         @arg @b SPI_BaudRatePrescaler_16
  *         @arg @b SPI_BaudRatePrescaler_32
  *         @arg @b SPI_BaudRatePrescaler_64
  *         @arg @b SPI_BaudRatePrescaler_128
  *         @arg @b SPI_BaudRatePrescaler_256
  * @retval ��
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
void Spi1SetBaud(u16 baud)
{
    u16 tmpreg = 0;
    
    tmpreg = SPI1->CR1;
    tmpreg &= ~SPI_BaudRatePrescaler_BM;
    tmpreg |= baud;
    SPI1->CR1 = tmpreg;
}
/**
  *************************************************************************************
  * @brief  ��ȡSpi1������
  * @param  ��
  * @retval SPI����,ֵ���£�
  *         @arg @b SPI_BaudRatePrescaler_2
  *         @arg @b SPI_BaudRatePrescaler_4
  *         @arg @b SPI_BaudRatePrescaler_8
  *         @arg @b SPI_BaudRatePrescaler_16
  *         @arg @b SPI_BaudRatePrescaler_32
  *         @arg @b SPI_BaudRatePrescaler_64
  *         @arg @b SPI_BaudRatePrescaler_128
  *         @arg @b SPI_BaudRatePrescaler_256
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
u16 Spi1GetBaud(void)
{
    u16 tmpreg = 0;
    
    tmpreg = SPI1->CR1;
    tmpreg &= SPI_BaudRatePrescaler_BM;

    return tmpreg;
}
/**
  *************************************************************************************
  * @brief  ��ȡspi1ʱ��Ƶ��
  * @param  ��
  * @retval spi1ʱ��Ƶ��
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
u32 Spi1GetFrequency(void)
{
    uint32_t freq;    
    RCC_ClocksTypeDef rcc;
    uint16_t baud;

    RCC_GetClocksFreq(&rcc);   // ���ϵͳʱ��Ƶ��
    
    freq = rcc.PCLK2_Frequency;
    baud = Spi1GetBaud() >> SPI_BaudRatePrescaler_BP;
    freq = freq >> (baud + 1);
    
    return freq;
}
/**
  *************************************************************************************
  * @brief  SPI1���Ͳ�����һ���ֽ�����
  * @param  byte: Ҫ���͵�����
  * @retval ���յ�������
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
u8 Spi1SendByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, byte);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

	
/**
 *@} SPI_Functions
 */
/**
 *@} SPI
 */
/**
 *@} PowerBoard
 */

