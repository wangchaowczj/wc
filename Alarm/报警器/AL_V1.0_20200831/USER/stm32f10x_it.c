/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "includes.h"

volatile uint32_t TickCounter;
extern OP_DATA Op_Data;



/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void TIM3_IRQHandler(void)   //TIM3中断
{
    OSIntEnter();
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
        op_data.I_ST = op_data.IZero_status;      
    }
    OSIntExit();
}

void EXTI9_5_IRQHandler(void)
{	
	OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6);//要放在回调函数的前面，因为USB进入后需要不断拉低INT进入中断，放在后面就不能及时进入中断，就会和电脑主机通信错误，无法正常枚举
        CH374InterruptCallBack();//只能用回调函数，不能用设备中断函数，因为缺少对中断状态的判断，具体的原因还没研究清楚		
	}			
	OSIntExit();
}
/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	OSIntEnter(); 
	OSTimeTick(); 
	TickCounter++;
	OSIntExit(); 
}


//void USART1_IRQHandler(void)
//{
//	OSIntEnter(); 
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {
//		SaveTransceiverByte(&UartTransceiver1, USART_ReceiveData(USART1));
//		RS485_S_timecount=0;
////		RS4852_S_timecount=0;
//	}
//	OSIntExit();  	
//}

//void USART2_IRQHandler(void)
//{
//	OSIntEnter(); 
//    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//    {			
//		SaveTransceiverByte(&UartTransceiver2, USART_ReceiveData(USART2));
//		RS4852_S_timecount=0;
//		RS485_S_timecount=0;		
//	}
//	OSIntExit(); 	
//}
/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
