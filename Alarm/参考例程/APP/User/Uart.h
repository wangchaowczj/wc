#ifndef _UART_H_
#define _UART_H_

#define UART1_TX_PORT                 GPIOA
#define UART1_TX_PORT_RCC             RCC_APB2Periph_GPIOA
#define UART1_TX_PIN                  GPIO_Pin_9

#define UART1_RX_PORT                 GPIOA
#define UART1_RX_PORT_RCC             RCC_APB2Periph_GPIOA
#define UART1_RX_PIN                  GPIO_Pin_10

#define UART3_TX_PORT                 GPIOB
#define UART3_TX_PORT_RCC             RCC_APB2Periph_GPIOB
#define UART3_TX_PIN                  GPIO_Pin_10

#define UART3_RX_PORT                 GPIOB
#define UART3_RX_PORT_RCC             RCC_APB2Periph_GPIOB
#define UART3_RX_PIN                  GPIO_Pin_11

extern void UartConfig(void);
extern void Uart1SendData(u8* dat,u16 len);
extern void Uart3SendData(u8* dat,u16 len);

#endif
