#include "includes.h"

/**
  *************************************************************************************
  * @brief  �������ú���
  * @param  ��
  * @retval �� 
  * @author ZH
  * @date   2018��6��8��
  * @note   UART1���ںͼ����ͨ�ţ�UART3��ת����ͨ��
  *************************************************************************************  
  */
void UartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(UART1_TX_PORT_RCC| UART1_RX_PORT_RCC | UART3_TX_PORT_RCC| UART3_RX_PORT_RCC
		| RCC_APB2Periph_AFIO |RCC_APB2Periph_USART1, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Configure USART2 Rx (PA.3) as input floating */
	GPIO_InitStruct.GPIO_Pin = UART1_RX_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART1_RX_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = UART3_RX_PIN;
	GPIO_Init(UART3_RX_PORT, &GPIO_InitStruct);
	

	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStruct.GPIO_Pin = UART1_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UART1_TX_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = UART3_TX_PIN;
	GPIO_Init(UART3_TX_PORT, &GPIO_InitStruct);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    /* Configure the USART */
    USART_Init(USART3, &USART_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_Init(USART1, &USART_InitStructure);
	
    
    /* Enable the USART Receive interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    //��������
    USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_Init(&NVIC_InitStructure); 	
}


/**
  *************************************************************************************
  * @brief  �������ݷ��ͺ���
  * @param  USARTx  Ҫ���͵Ĵ���
  * @param  dat  Ҫ���͵������׵�ַ
  * @param  len  Ҫ���͵����ݳ���  
  * @retval �� 
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void UartSendData(USART_TypeDef* USARTx, u8* dat, u16 len)
{
    u8 i = 0;
	
    for(i = 0; i < len; i++)
    {
        USART_SendData(USARTx, *(dat + i));
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); 
    }
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET); 

}
/**
  *************************************************************************************
  * @brief  ����1���ݷ��ͺ���
  * @param  dat  Ҫ���͵������׵�ַ
  * @param  len  Ҫ���͵����ݳ���  
  * @retval �� 
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void Uart1SendData(u8* dat, u16 len)
{
    UartSendData(USART1, dat, len);
}

/**
  *************************************************************************************
  * @brief  ����3���ݷ��ͺ���
  * @param  dat  Ҫ���͵������׵�ַ
  * @param  len  Ҫ���͵����ݳ���  
  * @retval �� 
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void Uart3SendData(u8* dat, u16 len)
{
    UartSendData(USART3, dat, len);
}

