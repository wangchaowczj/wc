#include "includes.h"
/**
  *************************************************************************************
  * @file    MidFunction.c
  * @author  
  * @version V1.0
  * @date    2020年8月28日
  * @brief   MidFunction模块源文件
  *************************************************************************************
  */
  
//==================================================================================================
//| 函数名称 | Time3_Init 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 初始化定时器Time3
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | arr：自动重装值 psc：预分频数 
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：王超    时间：2020-08-28
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 中断时间计算：(arr + 1)*(psc + 1)/Tck_tim
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================  
void Time3_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim = 72M
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx	
}
//==================================================================================================
//| 函数名称 | STMOpen 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 |单片机上电发出提示音
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：王超    时间：2020-08-28
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void STMOpen(void)
{
    u16 value;

    value = GetBatteryVoltage();
    if((value > 10500) && (0x01 != (op_data.STM_status & 0x07)))
    {
        op_data.STM_status = 0x01;
        Sound(2);
    } 
}
//==================================================================================================
//| 函数名称 | AutoPowerOff 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 自动关机
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：王超    时间：2020-08-28
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 电量低的话就关掉一部分功能或者说进入睡眠模式，发出声音提醒（三遍），然后就关机
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void AutoPowerOff(void)
{
    u8 t,s,count=0;
    
    for(s=0;s<10;s++)
    {
        if(	GetAdcValue(ADC_12V_CHANNEL,10) < 2602)
        {
            count++;
//            OSTimeDly (10);
            System72MDelay1ms(1);            
        }
    }
    if(count == 10)
    {
        count = 0;
        for(t=0;t<4;t++)
        {
            Sound(3);
        }
        SW5V_L();
        VSHIFT_L();//这里不能真正关掉电压
//        LED1_ON();       
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, DISABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
        SW5V_H();
        VSHIFT_H(); ; 
    }

}
//==================================================================================================
//| 函数名称 | Sound 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 声音的几种工作方式
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | p_arg =0 一直响
//|						p_arg = 1 一直关闭
//|						p_arg = 2 间断性响几声，时间间隔不变
//|						p_arg = 3 间断性响几声，时间间隔衰减
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：王超    时间：2020-08-28
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void Sound(u8 p_arg)
{
    if(p_arg == 0)
    {
        SOUND_H();
    }
    else if(p_arg == 1)
    {
        SOUND_L();
    }
    else if(p_arg == 2)
    {
        SOUND_H();
        System72MDelay1ms(300);
        SOUND_L();
				System72MDelay1ms(300);
    }
    else if(p_arg == 3)
    {
        SOUND_H();
        System72MDelay1ms(100);
        SOUND_L();
        System72MDelay1ms(300);
                SOUND_H();
        System72MDelay1ms(100);
        SOUND_L();
        System72MDelay1ms(250);
                SOUND_H();
        System72MDelay1ms(100);
        SOUND_L();
        System72MDelay1ms(200);
                SOUND_H();
        System72MDelay1ms(100);
        SOUND_L();
        System72MDelay1ms(150);        
    }
}
//==================================================================================================
//| 函数名称 | OLEDDisplay 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | OLED显示
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：王超    时间：2020-08-28
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void OLEDDisplay(void)
{
    u16 ADC_12V,ADC_I1,ADC_I2,ADC_COMOUT;   
    char Temp1[10];
    char Temp2[10]; 
   

    if(0x01 != (op_data.OLED_status & 0x07))
    {
        op_data.OLED_status = 0x01;
        LcdShowStr2Center("鲲程电子","V1.0");
        OSTimeDly(1000);
    }
    else
    {
        if(BusIsShort() == 1)
        {
            SW5V_L();//输出0V
            VSHIFT_L();//COM_OUT输出0V
            Sound(0);
            LcdShowStrCenter("总线异常");
            OSTimeDly(1000);
            SW5V_H();//输出5V
            VSHIFT_H();//COM_OUT输出6.7V            
        }
        else if(BusLeakIsShort() == 1)
        {
            Sound(0);
            LcdShowStrCenter("漏电流大");
        }
        else
        {
            Sound(1);            
            ADC_12V = GetBatteryVoltage();   
            ADC_COMOUT = GetBusVoltage(10,NULL); 
            sprintf(Temp1,"电源:%.1fV",(float)ADC_12V/1000);
            sprintf(Temp2,"总线:%.1fV",(float)ADC_COMOUT/1000);
            LcdShowStr2Center(Temp1,Temp2);                    
            OSTimeDly(500);
            ADC_I1 = EliminateBusCurrentErr();
            ADC_I2 = GetBusLeakCurrent(NULL);
            sprintf(Temp1,"总:%.1fuA",(float)ADC_I1);
            sprintf(Temp2,"漏:%.1fuA",(float)ADC_I2);
            LcdShowStr2Center(Temp1,Temp2); 
            OSTimeDly(500);                
        }
    }       
    
              
}
//==================================================================================================
//| 函数名称 | UsbIdentify 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 判断USB是主模式还是从模式
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：王超    时间：2020-08-28
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void UsbIdentify(void)
{
    u8 USB_FLAG;
    USB_FLAG = READ_USB_GET_IN_PIN();
    
    if(USB_FLAG == 1)
    {
        LED4_ON();
        LED3_OFF();
        USB_VBUS_L();
    }    
    else
    {
        LED3_ON();
        LED4_OFF();
    }
   
//    if(op_data.UsbState != 0xFF)
//    {
//        if(USB_FLAG == 1)
//        {

//            if(0x01 != (op_data.UsbState & 0x07))
//            {
//                op_data.UsbState = 0x01;
//     
//                LED4_ON();
//                LED3_OFF();
//                USB_VBUS_L();
//            }
//            
//        }
//        else if(USB_FLAG == 0)
//        {
//            if(0x02 != (op_data.UsbState & 0x07))
//            {
//                op_data.UsbState = 0x02;
//                LED3_ON();
//                LED4_OFF();
//                USB_VBUS_H();
//            }
//        }
//    }      
}  

