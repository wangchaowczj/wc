#ifndef _LED_H_
#define _LED_H_

#define LED1_PORT                 GPIOA
#define LED1_PORT_RCC             RCC_APB2Periph_GPIOA
#define LED1_PIN                  GPIO_Pin_0
#define LED1_OFF()          	  GPIO_ResetBits(LED1_PORT, LED1_PIN)
#define LED1_ON()                 GPIO_SetBits(LED1_PORT, LED1_PIN)
#define LED1_TOGGLE()             LED1_PORT->ODR ^=LED1_PIN

#define LED2_PORT                 GPIOA
#define LED2_PORT_RCC             RCC_APB2Periph_GPIOA
#define LED2_PIN                  GPIO_Pin_1
#define LED2_OFF()          	  GPIO_ResetBits(LED2_PORT, LED2_PIN)
#define LED2_ON()                 GPIO_SetBits(LED2_PORT, LED2_PIN)
#define LED2_TOGGLE()             LED2_PORT->ODR ^=LED2_PIN

#define LED3_PORT                 GPIOA
#define LED3_PORT_RCC             RCC_APB2Periph_GPIOA
#define LED3_PIN                  GPIO_Pin_15
#define LED3_OFF()          	  GPIO_ResetBits(LED3_PORT, LED3_PIN)
#define LED3_ON()                 GPIO_SetBits(LED3_PORT, LED3_PIN)
#define LED3_TOGGLE()             LED3_PORT->ODR ^=LED3_PIN


extern void LedConfig(void);

#endif
