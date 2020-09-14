/**
  *************************************************************************************
  * @file    SPI.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SPI模块头文件
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
 
#define SPI_BaudRatePrescaler_BM         0x00000038u         /**<SPI预分频屏蔽位*/
#define SPI_BaudRatePrescaler_BP         (0x03u)             /**<SPI分频的位置*/ 

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
