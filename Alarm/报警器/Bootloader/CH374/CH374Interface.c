#include "includes.h"


/**
  *************************************************************************************
  * @brief  CH374 SPI配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年7月21日
  * @note   
  *************************************************************************************  
  */
void CH374SpiConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
    /* 配置片选CS */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStruct.GPIO_Pin = CH374_SPI_CS_PIN;
	GPIO_Init(CH374_SPI_CS_PORT, &GPIO_InitStruct);
       
	
	/* 配置SPI引脚类型 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_InitStruct.GPIO_Pin = CH374_SPI_SCLK_PIN | CH374_SPI_MISO_PIN | CH374_SPI_MOSI_PIN;
    
    GPIO_Init(GPIOB, &GPIO_InitStruct);//SPI引脚
    

    CH374_SPI_CS_H();    
		
	/*!< SPI2 configuration */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//USB芯片和单片机间线太长，2分频时在从机MSC模式下，拷入大文件时数据有不对。4分频偶尔错。 

	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStruct);

	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(SPI2, ENABLE);
    
   
    
    
}

/**
  *************************************************************************************
  * @brief  CH374硬件配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年8月22日
  * @note   无
  *************************************************************************************  
  */
void Ch374HardWareConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(CH374_INT_PORT_RCC|USB_GET_IN_PORT_RCC|USB_VBUS_PORT_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能复用功能时钟
    
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_InitStruct.GPIO_Pin = CH374_INT_PIN;
	GPIO_Init(CH374_INT_PORT, &GPIO_InitStruct);//PC6，CH374_INT引脚
			
	GPIO_InitStruct.GPIO_Pin = USB_GET_IN_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(USB_GET_IN_PORT, &GPIO_InitStruct);//PC11,主从模式选择

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStruct.GPIO_Pin = USB_VBUS_PIN;
	GPIO_Init(USB_VBUS_PORT, &GPIO_InitStruct);	//PA8，VBUS
	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);//PC6
	
	/*外部下降沿中断*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//外部下降沿中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
//    
//    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//    EXTI_InitStructure.EXTI_Mode =  EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
//    
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //中断线所在通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//中断优先级
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//中断抢占优先级
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
//    NVIC_Init(&NVIC_InitStructure);
	
	CH374SpiConfig();
}
/**
  *************************************************************************************
  * @brief  SPI1发送并接收一个字节数据
  * @param  byte: 要发送的数据
  * @retval 接收到的数据
  * @author ZH
  * @date   2015年7月21日
  * @note   无
  *************************************************************************************  
  */
u8 CH374SpiSendByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(CH374_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(CH374_SPI, byte);
	while (SPI_I2S_GetFlagStatus(CH374_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(CH374_SPI);
}

/**
  *************************************************************************************
  * @brief  从指定寄存器读取数据
  * @param  mAddr: 要发送的数据
  * @retval 接收到的数据
  * @author ZH
  * @date   2015年7月21日
  * @note   无
  *************************************************************************************  
  */
u8 Read374Byte(u8 mAddr)
{
	u8 Value;
	
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374READ);

	Value = CH374SpiSendByte(0xFF);
	
	CH374_SPI_CS_H();
	
	return Value;
}

/**
  *************************************************************************************
  * @brief  向指定寄存器写入数据
  * @param  mAddr: 寄存器地址
  * @param  mData: 写入的数据  
  * @retval 无
  * @author ZH
  * @date   2015年7月21日
  * @note   无
  *************************************************************************************  
  */
void Write374Byte(u8 mAddr, u8 mData)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374WRITE);
	
	CH374SpiSendByte(mData);
	
	CH374_SPI_CS_H();
}

/**
  *************************************************************************************
  * @brief  从指定起始地址读出数据块
  * @param  mAddr: 寄存器地址
  * @param  mLen: 要读的数据长度
  * @param  mBuf: 数据缓冲区地址  
  * @retval 无
  * @author ZH
  * @date   2015年7月21日
  * @note   无
  *************************************************************************************  
  */
void Read374Block(u8 mAddr, u8 mLen, u8* mBuf)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374READ);

	while(mLen--)
	{
		*mBuf++ = CH374SpiSendByte(0xFF);
	}	
	CH374_SPI_CS_H();
}

/**
  *************************************************************************************
  * @brief  向指定起始地址写入数据块
  * @param  mAddr: 寄存器地址
  * @param  mLen: 要写的数据长度
  * @param  mBuf: 数据缓冲区地址  
  * @retval 无
  * @author ZH
  * @date   2015年7月21日
  * @note   无
  *************************************************************************************  
  */
void Write374Block(u8 mAddr, u8 mLen, u8* mBuf)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374WRITE);

	while (mLen--)
	{
		CH374SpiSendByte(*mBuf++);
	}
	CH374_SPI_CS_H();
}

void CH374WriteBlockZero(u8 mAddr, u8 mLen)
{
	CH374_SPI_CS_L();
	
	CH374SpiSendByte(mAddr);
	CH374SpiSendByte(CMD_SPI_374WRITE);

	while (mLen--)
	{
		CH374SpiSendByte(0x00);
	}
	CH374_SPI_CS_H();
}

ErrorStatus Ch374HardwareState(void)
{
	u8 Value;
    
	Value = Read374Byte(0x04);//硬件识别位，用于验证硬件连接无误和读操作成功
    
	if((Value&0x03) == 0x01)
	{
		return SUCCESS;
	}
	return ERROR;
}
