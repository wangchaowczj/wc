/**
  *************************************************************************************
  * @file    SystemConfig.h
  * @author  ����
  * @version V1.0.0
  * @date    2020��8��7��
  * @brief   ���ļ�ΪSystemConfig.c��ͷ�ļ�
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
  *@brief ���Ŷ���
  *@{
  */
//5V/6.7V��ѹ�л�����     
#define VSHIFT_PORT             GPIOC
#define VSHIFT_PORT_RCC         RCC_APB2Periph_GPIOC
#define VSHIFT_PIN              GPIO_Pin_9
#define VSHIFT_L()              GPIO_ResetBits(VSHIFT_PORT,VSHIFT_PIN)        //COM_OUT���5V
#define VSHIFT_H()              GPIO_SetBits(VSHIFT_PORT,VSHIFT_PIN)          //COM_OUT���6.7V

//5Vʹ������
#define SW5V_PORT               GPIOA
#define SW5V_PORT_RCC           RCC_APB2Periph_GPIOA
#define SW5V_PIN                GPIO_Pin_6
#define SW5V_L()                GPIO_ResetBits(SW5V_PORT,SW5V_PIN)            //���0V
#define SW5V_H()                GPIO_SetBits(SW5V_PORT,SW5V_PIN)              //���5V
#define SHORT_ST()              GPIO_ReadOutputDataBit(SW5V_PORT,SW5V_PIN)

//��ѹ��ȡ�л�����
#define ITX_PORT                GPIOC
#define ITX_PORT_RCC            RCC_APB2Periph_GPIOC
#define ITX_PIN                 GPIO_Pin_7
#define ITX_L()                 GPIO_ResetBits(ITX_PORT,ITX_PIN)              //LINEA���������LINEBΪ����
#define ITX_H()                 GPIO_SetBits(ITX_PORT,ITX_PIN)                //LINEB���������LINEAΪ����

//���������л�����
#define IRANGE_PORT             GPIOC
#define IRANGE_RCC              RCC_APB2Periph_GPIOC
#define IRANGE_PIN              GPIO_Pin_8
#define IRANGE_L()              GPIO_ResetBits(IRANGE_PORT,IRANGE_PIN)  //�л���С����
#define IRANGE_H()              GPIO_SetBits(IRANGE_PORT,IRANGE_PIN)    //�л���������

//�����ѹ
#define SOUND_PORT               GPIOA
#define SOUND_PORT_RCC           RCC_APB2Periph_GPIOA
#define SOUND_PIN                GPIO_Pin_0
#define SOUND_L()                GPIO_ResetBits(SOUND_PORT,SOUND_PIN)            //���0V
#define SOUND_H()                GPIO_SetBits(SOUND_PORT,SOUND_PIN)              //���12V
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
