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

static volatile u8 Timer2StartFlag = 0;

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
//void PendSV_Handler(void) ��OS_CPU_PendSVHandler����
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick(); 
	OSIntExit(); 
}
void USART3_IRQHandler(void)
{	
	u8 temp;
	
	OSIntEnter();
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {				
        temp = USART_ReceiveData(USART3);
		if(Uart3Data.status == RECIEVING)
		{
			if((OSTimeGet() - Uart3RecTimeLatest > 5) || (Uart3Data.bytenum >= BUF_MAX))
			{
				Uart3Data.bytenum = 0;
			}
            Uart3Data.uch_buf[Uart3Data.bytenum++] = temp;						
		}
		Uart3RecTimeLatest = OSTimeGet();
    }
	OSIntExit();
}
/**
  * @brief  This function handles USART2 Handler.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{	
	u8 temp;
	
	OSIntEnter();

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		temp = USART_ReceiveData(USART1);
		if(Uart1Data.status == RECIEVING)//����Ѿ����յ����ݰ�ͷ��β��ȴ�������ɺ��ټ�������
		{
			if((OSTimeGet() - Uart1RecTimeLatest > 5) || (Uart1Data.bytenum >= BUF_MAX))
			{
				Uart1Data.bytenum = 0;
			}
			else if((Uart1Data.bytenum != 0) && (0xC0 != Uart1Data.uch_buf[0]))
			{
				Uart1Data.bytenum = 0;
			}
			
			if(0xC0 == temp)
			{//����ͷ����β					
				if(Uart1Data.bytenum >= 0x04)//����1�ֽ�����+2�ֽ�CRC16У��+2�ֽ�ͷβ
				{
					Uart1Data.status = RECIEVED;//���յ���ͷ��β
					
					if(OS_ERR_NONE != OSSemPost(MsgSem.Uart1))
					{
						Uart1Data.bytenum = 0;
						Uart1Data.status = RECIEVING;
					}
				}
				else
				{
					Uart1Data.bytenum = 0;
				}
				Uart1Data.uch_buf[Uart1Data.bytenum++] = temp;
			}
			else if(0x00 != Uart1Data.bytenum)
			{//�����������Ѿ����յ�����
				Uart1Data.uch_buf[Uart1Data.bytenum++] = temp;
			}
			else
			{//�������ݣ���û�н��յ���ͷ
				;
			}
		}
		Uart1RecTimeLatest = OSTimeGet();
	}
	OSIntExit();
}
void TIM2_IRQHandler(void)
{
	OSIntEnter();
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )  //TIMx->DIER	TIMx->SR
	{		
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update); //���жϱ�־λ
		switch(DetProtocol.SendBuffer.State)
		{
			case DET_SEND://���ڷ�������״̬
				SET_LIN_REVERSE();
				if(DET_FIXED_PULSE == DetProtocol.Mode)
				{
					if(DetProtocol.SendBuffer.Index < DetProtocol.SendBuffer.Count)
					{
						TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index&0x01]);
						DetProtocol.SendBuffer.Index++;
					}
					else
					{
						DetProtocol.SendBuffer.State = DET_END;//����������
						TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
					}				
				}
				else
				{
					if(DetProtocol.SendBuffer.Index < DetProtocol.SendBuffer.Count)
					{
						TIM_SetAutoreload(TIM2, DetProtocol.SendBuffer.Timer[DetProtocol.SendBuffer.Index++]);
					}				
					else if(DetProtocol.RecData.ExpentCount != 0)
					{//��Ҫ��������					
						TIM_SetAutoreload(TIM2, 0xFFFF);
						DetProtocol.SendBuffer.State = DET_DELAY_REC;
					}
					else
					{
						DetProtocol.SendBuffer.State = DET_END;//����������
						TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
					}				
				}				
				break;
			case DET_DELAY_REC://������ʱ�ȴ�����״̬
				DetProtocol.SendBuffer.State = DET_WAITING_REC;//��ʼ����
				
				TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
				TIM_SetCounter(TIM2, 0);
				Timer2StartFlag = 0;
				//���ⲿ�ж�
				ExtiConfig(DET_RX_EXTI_LINE, EXTI_Trigger_Rising_Falling, ENABLE);				
				break;
			case DET_WAITING_REC:
			default:
				DetProtocol.SendBuffer.State = DET_TIMEOUT;
				TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
				TIM_Cmd(TIM2, DISABLE);  //�ر�ʱ��
				TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
				NVIC_DisableIRQ(TIM2_IRQn);//ʧ���ⲿ�ж�

				ExtiConfig(DET_RX_EXTI_LINE, EXTI_Trigger_Rising_Falling, DISABLE);				
				break;
		}
	}
	OSIntExit();
}
void TIM3_IRQHandler(void)
{
	OSIntEnter();
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update); //���жϱ�־λ	
	}
	OSIntExit(); 
}
void TIM4_IRQHandler(void)
{
	OSIntEnter();
	
	if ( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update); //���жϱ�־λ
		TIM_Cmd(TIM4, DISABLE);  //�ر�ʱ��	
	}
	OSIntExit();  
}
void EXTI9_5_IRQHandler(void)
{
	OSIntEnter();
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		/* Clear the  EXTI line 9 pending bit */
        EXTI_ClearFlag(EXTI_Line8);
		
		TIM_Cmd(TIM2, ENABLE);
		if(Timer2StartFlag == 0)
		{//������ʱ��
			TIM_Cmd(TIM2, ENABLE);
			Timer2StartFlag = 1;
		}
		else
		{
			if(DetProtocol.RecBuffer.Count < DET_REC_BUFFER_MAX)
			{
				DetProtocol.RecBuffer.Timer[DetProtocol.RecBuffer.Count++] = TIM_GetCounter(TIM2);
				TIM_SetCounter(TIM2, 0);
			}		
		}
	}
	OSIntExit();	
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
