/**
  *************************************************************************************
  * @file    SystemConfig.h
  * @author  王超
  * @version V1.0.0
  * @date    2020年8月7日
  * @brief   本文件为SystemConfig.c的头文件
  *************************************************************************************
  */
#ifndef _SYSTEMCONFIG_H_
#define _SYSTEMCONFIG_H_

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SystemConfig
  *@{
  */
/**@defgroup GpioConfig_PinDefines
  *@brief 引脚定义
  *@{
  */
//5V/6.7V电压切换引脚     
#define VSHIFT_PORT             GPIOC
#define VSHIFT_PORT_RCC         RCC_APB2Periph_GPIOC
#define VSHIFT_PIN              GPIO_Pin_9
#define VSHIFT_L()              GPIO_ResetBits(VSHIFT_PORT,VSHIFT_PIN)        //COM_OUT输出5V
#define VSHIFT_H()              GPIO_SetBits(VSHIFT_PORT,VSHIFT_PIN)          //COM_OUT输出6.7V

//5V使能引脚
#define SW5V_PORT               GPIOA
#define SW5V_PORT_RCC           RCC_APB2Periph_GPIOA
#define SW5V_PIN                GPIO_Pin_6
#define SW5V_L()                GPIO_ResetBits(SW5V_PORT,SW5V_PIN)            //输出0V
#define SW5V_H()                GPIO_SetBits(SW5V_PORT,SW5V_PIN)              //输出5V
#define SHORT_ST()              GPIO_ReadOutputDataBit(SW5V_PORT,SW5V_PIN)

//电压读取切换引脚
#define ITX_PORT                GPIOC
#define ITX_PORT_RCC            RCC_APB2Periph_GPIOC
#define ITX_PIN                 GPIO_Pin_7
#define ITX_L()                 GPIO_ResetBits(ITX_PORT,ITX_PIN)              //LINEA输入电流；LINEB为正极
#define ITX_H()                 GPIO_SetBits(ITX_PORT,ITX_PIN)                //LINEB输入电流；LINEA为正极

//电流量程切换引脚
#define IRANGE_PORT             GPIOC
#define IRANGE_RCC              RCC_APB2Periph_GPIOC
#define IRANGE_PIN              GPIO_Pin_8
#define IRANGE_L()              GPIO_ResetBits(IRANGE_PORT,IRANGE_PIN)  //切换到小量程
#define IRANGE_H()              GPIO_SetBits(IRANGE_PORT,IRANGE_PIN)    //切换到大量程

//输出电压
#define SOUND_PORT               GPIOA
#define SOUND_PORT_RCC           RCC_APB2Periph_GPIOA
#define SOUND_PIN                GPIO_Pin_0
#define SOUND_L()                GPIO_ResetBits(SOUND_PORT,SOUND_PIN)            //输出0V
#define SOUND_H()                GPIO_SetBits(SOUND_PORT,SOUND_PIN)              //输出12V
#define ALARM_ST()               GPIO_ReadOutputDataBit(SOUND_PORT,SOUND_PIN)
/**
 *@} SystemConfig_PinDefines
 */
/**
 *@} SystemConfig
 */
/**
 *@} PowerBoard
 */ 
extern void GpioConfig(void);
extern void SystemGpioConfig(void);
//extern ErrorStatus SystemHardwareInit(void);

#endif
