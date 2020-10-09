/**
  *************************************************************************************
  * @file    SystemConfig.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   SystemConfigģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"
/**
  *************************************************************************************
  * @brief  IO������
  * @param  ��
  * @retval ��
  * @author ����
  * @date   2020��8��7��
  * @note   ��
  *************************************************************************************  
  */
void GpioConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(VSHIFT_PORT_RCC | SW5V_PORT_RCC | ITX_PORT_RCC | IRANGE_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    
//5V/6.7V��ѹ�л�����    
    GPIO_InitStructure.GPIO_Pin = VSHIFT_PIN; 
    GPIO_Init(VSHIFT_PORT, &GPIO_InitStructure);
    VSHIFT_H();//COM_OUT���6.7V
    
//5Vʹ������
    GPIO_InitStructure.GPIO_Pin = SW5V_PIN; 
    GPIO_Init(SW5V_PORT, &GPIO_InitStructure);
    SW5V_H();//���5V
    
//��ѹ��ȡ�л�����
	GPIO_InitStructure.GPIO_Pin = ITX_PIN; 
	GPIO_Init(ITX_PORT, &GPIO_InitStructure);
	ITX_L();//LINEA���������LINEBΪ���� ֻ��Ϊ�ͣ��ߵ�ƽ����Ϊ�㣬����IO��ѹ2.7V��ԭ������1613оƬ�ĵ�Դ�����COMOUT������

//���������л�����
	GPIO_InitStructure.GPIO_Pin = IRANGE_PIN; 
	GPIO_Init(IRANGE_PORT, &GPIO_InitStructure);
    IRANGE_H();//���ǷŴ�25������
    
//�����ѹ����
	GPIO_InitStructure.GPIO_Pin = SOUND_PIN; 
	GPIO_Init(SOUND_PORT, &GPIO_InitStructure);
    SOUND_L();//�������ѹ
}


/**
  *************************************************************************************
  * @brief  ϵͳ����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
void SystemGpioConfig(void)
{
    DeviceInfo.name = 'A';
    op_data.UsbState = 0;
    op_data.OLED_status = 0;
    op_data.STM_status  = 0; 
    SysTickConfig();//�ж�һ��	
//    PowerOffCheck();
    ReadFactoryConfigParam();
    PackageKeyInit();
    LEDConfig();	
    Ch374HardWareConfig();
    CH374DeviceInit();
    I2C_GPIO_Config();
    LcdInit();
    AdcConfig();
    GpioConfig(); 
    Queue_Init(); 
    WatchDogInit();    
//    Time3_Init(4999,7199);//100ms�ж�һ��
    
}
///**
//  *************************************************************************************
//  * @brief  Ӳ����ʼ��
//  * @param  ��
//  * @retval ��
//  * @author ZH
//  * @date   2014��9��10��
//  * @note   ��
//  *************************************************************************************  
//  */
//ErrorStatus SystemHardwareInit(void)
//{
//    Ch374HardwareState();//��������CH374Ӳ�������Ƿ�����
//	return SUCCESS;		
//}



