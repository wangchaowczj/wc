/**
  *************************************************************************************
  * @file    SystemConfig.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SystemConfig模块源文件
  *************************************************************************************
  */
#include "includes.h"
/**
  *************************************************************************************
  * @brief  IO口配置
  * @param  无
  * @retval 无
  * @author 王超
  * @date   2020年8月7日
  * @note   无
  *************************************************************************************  
  */
void GpioConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(VSHIFT_PORT_RCC | SW5V_PORT_RCC | ITX_PORT_RCC | IRANGE_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    
//5V/6.7V电压切换引脚    
    GPIO_InitStructure.GPIO_Pin = VSHIFT_PIN; 
    GPIO_Init(VSHIFT_PORT, &GPIO_InitStructure);
    VSHIFT_H();//COM_OUT输出6.7V
    
//5V使能引脚
    GPIO_InitStructure.GPIO_Pin = SW5V_PIN; 
    GPIO_Init(SW5V_PORT, &GPIO_InitStructure);
    SW5V_H();//输出5V
    
//电压读取切换引脚
	GPIO_InitStructure.GPIO_Pin = ITX_PIN; 
	GPIO_Init(ITX_PORT, &GPIO_InitStructure);
	ITX_L();//LINEA输入电流；LINEB为正极 只能为低，高电平不能为零，会有IO电压2.7V，原因在于1613芯片的电源输入和COMOUT共用了

//电流量程切换引脚
	GPIO_InitStructure.GPIO_Pin = IRANGE_PIN; 
	GPIO_Init(IRANGE_PORT, &GPIO_InitStructure);
    IRANGE_H();//高是放大25倍量程
    
//输出电压引脚
	GPIO_InitStructure.GPIO_Pin = SOUND_PIN; 
	GPIO_Init(SOUND_PORT, &GPIO_InitStructure);
    SOUND_L();//不输出电压
}


/**
  *************************************************************************************
  * @brief  系统配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
void SystemGpioConfig(void)
{
    DeviceInfo.name = 'A';
    op_data.UsbState = 0;
    op_data.OLED_status = 0;
    op_data.STM_status  = 0; 
    SysTickConfig();//中断一次	
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
//    Time3_Init(4999,7199);//100ms中断一次
    
}
///**
//  *************************************************************************************
//  * @brief  硬件初始化
//  * @param  无
//  * @retval 无
//  * @author ZH
//  * @date   2014年9月10日
//  * @note   无
//  *************************************************************************************  
//  */
//ErrorStatus SystemHardwareInit(void)
//{
//    Ch374HardwareState();//用来测试CH374硬件工作是否正常
//	return SUCCESS;		
//}



