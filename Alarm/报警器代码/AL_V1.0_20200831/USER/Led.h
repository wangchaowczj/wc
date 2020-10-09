#ifndef _LED_H_
#define _LED_H_


#define LED1_PORT          GPIOB
#define LED1_PORT_RCC      RCC_APB2Periph_GPIOB
#define LED1_PIN           GPIO_Pin_5
#define LED1_ON()          GPIO_ResetBits(LED1_PORT, LED1_PIN) 
#define LED1_OFF()         GPIO_SetBits(LED1_PORT, LED1_PIN)
#define LED1_TOGGLE()      LED1_PORT->ODR ^=LED1_PIN

#define LED3_PORT          GPIOA
#define LED3_PORT_RCC      RCC_APB2Periph_GPIOA
#define LED3_PIN           GPIO_Pin_15
#define LED3_ON()          GPIO_ResetBits(LED3_PORT, LED3_PIN) 
#define LED3_OFF()         GPIO_SetBits(LED3_PORT, LED3_PIN)
#define LED3_TOGGLE()      LED3_PORT->ODR ^=LED3_PIN

#define LED4_PORT          GPIOB
#define LED4_PORT_RCC      RCC_APB2Periph_GPIOB
#define LED4_PIN           GPIO_Pin_7
#define LED4_ON()          GPIO_ResetBits(LED4_PORT, LED4_PIN) 
#define LED4_OFF()         GPIO_SetBits(LED4_PORT, LED4_PIN)
#define LED4_TOGGLE()      LED4_PORT->ODR ^=LED4_PIN

void LEDConfig(void);

#endif













