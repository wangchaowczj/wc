#ifndef _ADC_H_
#define _ADC_H_

#define ADC1_RCC                RCC_APB2Periph_ADC1

//ADC12_IN10
#define AD_LINB_PORT            GPIOC
#define AD_LINB_PORT_RCC        RCC_APB2Periph_GPIOC
#define AD_LINB_PIN             GPIO_Pin_0

//ADC12_IN11
#define AD_HV_PORT              GPIOC
#define AD_HV_PORT_RCC          RCC_APB2Periph_GPIOC
#define AD_HV_PIN               GPIO_Pin_1

//ADC12_IN12
#define AD_LINE_I_PORT          GPIOC
#define AD_LINE_I_PORT_RCC      RCC_APB2Periph_GPIOC
#define AD_LINE_I_PIN           GPIO_Pin_2

//ADC12_IN13
#define AD_LINA_PORT            GPIOC
#define AD_LINA_PORT_RCC        RCC_APB2Periph_GPIOC
#define AD_LINA_PIN             GPIO_Pin_3


#define AD_HV_DEFAULT_PARAM          	 		8.86230
#define AD_LINA_DEFAULT_PARAM          	 		4.02832  			//分压比为1/(1+4)  //2.41699 //分压比为1/(2+1)
#define AD_LINB_DEFAULT_PARAM          	 		4.02832				//2.41699

#define AD_LV_CURRENT_DEFAULT_PARAM      		1.5012


#define LV_VOLTAGE_RQE_MIN   4500
#define LV_VOLTAGE_RQE_MAX   5500//标准值是5V

#define HV_VOLTAGE_RQE_MIN   10500
#define HV_VOLTAGE_RQE_MAX   11600//标准值是11.3V

typedef enum 
{
	AD_LINB = 10,
	AD_HV,
	AD_LINE_I,
	AD_LINA
}ADC1_CHANNEL;

typedef struct 
{
	float k ;
	u16 b ;	
}ADJUST_PARAMETER;

typedef struct
{
	ADJUST_PARAMETER Current; //电流校准参数
	ADJUST_PARAMETER Voltage; //电压校准参数
	u8 Time[6];//校准时间,年月日时分秒
}ADC_ADJUST;
extern ADC_ADJUST AdcAdjust;     //ADC校准参数

extern void AdcConfig(void);
extern u16 GetCurrentValue(u16* AdcMeasureValue);
extern u16 GetLinABVoltage(u16* AdcMeasureValue);
extern u16 GetVoltageValue(ADC1_CHANNEL channel);


#endif
