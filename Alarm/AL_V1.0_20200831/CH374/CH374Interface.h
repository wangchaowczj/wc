#ifndef _CH374INTERFACE_H_
#define _CH374INTERFACE_H_

#define CH374_SPI_SCLK_PORT              GPIOB 
#define CH374_SPI_SCLK_PORT_RCC          RCC_APB2Periph_GPIOB
#define CH374_SPI_SCLK_PIN               GPIO_Pin_13

#define CH374_SPI_MISO_PORT              GPIOB
#define CH374_SPI_MISO_PORT_RCC          RCC_APB2Periph_GPIOB
#define CH374_SPI_MISO_PIN               GPIO_Pin_14

#define CH374_SPI_MOSI_PORT              GPIOB
#define CH374_SPI_MOSI_PORT_RCC          RCC_APB2Periph_GPIOB
#define CH374_SPI_MOSI_PIN               GPIO_Pin_15

#define CH374_SPI_CS_PORT       		 GPIOB
#define CH374_SPI_CS_PORT_RCC    		 RCC_APB2Periph_GPIOB
#define CH374_SPI_CS_PIN         		 GPIO_Pin_12

#define CH374_SPI_CS_H()                 GPIO_SetBits(CH374_SPI_CS_PORT, CH374_SPI_CS_PIN)
#define CH374_SPI_CS_L()                 GPIO_ResetBits(CH374_SPI_CS_PORT, CH374_SPI_CS_PIN)


#define CH374_INT_PORT       		     GPIOC
#define CH374_INT_PORT_RCC    		     RCC_APB2Periph_GPIOC
#define CH374_INT_PIN         		     GPIO_Pin_6

#define GET_CH374_INT_PIN()              GPIO_ReadInputDataBit(CH374_INT_PORT, CH374_INT_PIN)

#define CH374_SPI_RCC                    RCC_APB1Periph_SPI2
#define CH374_SPI                        SPI2

#define USB_GET_IN_PORT       		     GPIOC
#define USB_GET_IN_PORT_RCC    		     RCC_APB2Periph_GPIOC
#define USB_GET_IN_PIN         		     GPIO_Pin_11

#define USB_CONNET_DEVICE   0
#define READ_USB_GET_IN_PIN()            GPIO_ReadInputDataBit(USB_GET_IN_PORT, USB_GET_IN_PIN)

#define USB_VBUS_PORT       		     GPIOA
#define USB_VBUS_PORT_RCC    		     RCC_APB2Periph_GPIOA
#define USB_VBUS_PIN         		     GPIO_Pin_8
#define USB_VBUS_H()                     GPIO_SetBits(USB_VBUS_PORT, USB_VBUS_PIN)
#define USB_VBUS_L()                     GPIO_ResetBits(USB_VBUS_PORT, USB_VBUS_PIN)

extern void Ch374HardWareConfig(void);
extern u8 Read374Byte(u8 mAddr);
extern void Write374Byte(u8 mAddr, u8 mData);
extern void CH374WriteBlockZero(u8 mAddr, u8 mLen);
extern void Read374Block(u8 mAddr, u8 mLen, u8* mBuf);
extern void Write374Block(u8 mAddr, u8 mLen, u8* mBuf);
extern ErrorStatus Ch374HardwareState(void);
//extern void Ch374UsbStatus(void);
#endif

