/**
  *************************************************************************************
  * @file    SPI.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   SPIģ��ͷ�ļ�
  *************************************************************************************
  */
#ifndef _SPI_H_
#define _SPI_H_

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SPI
  *@{
  */
  
/**@defgroup SPI_PinDefines
  *@{
  */  
#define SPI1_CS_PORT       				GPIOB
#define SPI1_CS_PORT_RCC    			RCC_APB2Periph_GPIOB
#define SPI1_CS_PIN         			GPIO_Pin_6
#define SPI1_CS_H()                     GPIO_SetBits(SPI1_CS_PORT, SPI1_CS_PIN)
#define SPI1_CS_L()                     GPIO_ResetBits(SPI1_CS_PORT, SPI1_CS_PIN)

/**
 *@} SPI_PinDefines
 */
 
#define SPI_BaudRatePrescaler_BM         0x00000038u         /**<SPIԤ��Ƶ����λ*/
#define SPI_BaudRatePrescaler_BP         (0x03u)             /**<SPI��Ƶ��λ��*/ 

/**
 *@} SPI
 */
/**
 *@} PowerBoard
 */
extern void Spi1Config(void);
extern void Spi1SetBaud(u16 baud);
extern u16 Spi1GetBaud(void);
extern u32 Spi1GetFrequency(void);
extern u8 Spi1SendByte(u8 byte);

#endif
