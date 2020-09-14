#ifndef _SYSTEMCONFIG_H_
#define _SYSTEMCONFIG_H_

//#define TEST_POINT1_PORT                 GPIOA
//#define TEST_POINT1_PORT_RCC             RCC_APB2Periph_GPIOA
//#define TEST_POINT1_PIN                  GPIO_Pin_5
//#define TEST_POINT1_OFF()          	     GPIO_ResetBits(TEST_POINT1_PORT, TEST_POINT1_PIN)
//#define TEST_POINT1_ON()                 GPIO_SetBits(TEST_POINT1_PORT, TEST_POINT1_PIN)
//#define TEST_POINT1_TOGGLE()             TEST_POINT1_PORT->ODR ^=TEST_POINT1_PIN

#define TEST_POINT2_PORT                 GPIOA
#define TEST_POINT2_PORT_RCC             RCC_APB2Periph_GPIOA
#define TEST_POINT2_PIN                  GPIO_Pin_4
#define TEST_POINT2_OFF()          	     GPIO_ResetBits(TEST_POINT2_PORT, TEST_POINT2_PIN)
#define TEST_POINT2_ON()                 GPIO_SetBits(TEST_POINT2_PORT, TEST_POINT2_PIN)
#define TEST_POINT2_TOGGLE()             TEST_POINT2_PORT->ODR ^=TEST_POINT2_PIN

extern void SoftReset(void);
extern void SystemConfig(void);
extern ErrorStatus HardwareInit(void);
extern void GetSTM32ChipIDStr(u8* IdStr);

#endif
