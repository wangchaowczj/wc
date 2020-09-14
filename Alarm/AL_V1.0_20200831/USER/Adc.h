#ifndef _AD_H_
#define _AD_H_

#define ADC1_RCC               		RCC_APB2Periph_ADC1

//ADC123_IN1 ���������һ
#define AD_I1_PORT             		GPIOA
#define AD_I1_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I1_PIN              		GPIO_Pin_1
#define ADC_I1_CHANNEL		   			1

//ADC123_IN2 ��������̶�
#define AD_I2_PORT             		GPIOA
#define AD_I2_PORT_RCC         		RCC_APB2Periph_GPIOA
#define AD_I2_PIN              		GPIO_Pin_2
#define ADC_I2_CHANNEL		   			2

//ADC123_IN10 ��Դ��ѹ
#define AD_12V_PORT            		GPIOC
#define AD_12V_PORT_RCC        		RCC_APB2Periph_GPIOC
#define AD_12V_PIN             		GPIO_Pin_0
#define ADC_12V_CHANNEL		   			10

//ADC123_IN11 LINEA
#define AD_LINEA_PORT            	GPIOC
#define AD_LINEA_PORT_RCC        	RCC_APB2Periph_GPIOC
#define AD_LINEA_PIN             	GPIO_Pin_1
#define ADC_LINEA_CHANNEL		  	 	11

//ADC123_IN11 LINEA2
#define AD_LINEAA_PORT            	GPIOA
#define AD_LINEAA_PORT_RCC        	RCC_APB2Periph_GPIOC
#define AD_LINEAA_PIN             	GPIO_Pin_3
#define ADC_LINEAA_CHANNEL		  	 	3

//ADC123_IN12 LINEB
#define AD_LINEB_PORT               GPIOC
#define AD_LINEB_PORT_RCC           RCC_APB2Periph_GPIOC
#define AD_LINEB_PIN                GPIO_Pin_2
#define ADC_LINEB_CHANNEL		   		12

//ADC123_IN13 ���������ѹ
#define AD_COMOUT_PORT              GPIOC
#define AD_COMOUT_PORT_RCC          RCC_APB2Periph_GPIOC
#define AD_COMOUT_PIN               GPIO_Pin_3
#define ADC_COMOUT_CHANNEL		   	    13

typedef struct    
{                 
	float k ;       
	u16 b ;	  
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;    
}ADJUST_PARAMETER;

typedef struct
{					  
		ADJUST_PARAMETER linab;		        //���ߵ�ѹУ׼
		ADJUST_PARAMETER current1;          //����У׼������С����0-1mA��
		ADJUST_PARAMETER current2;          //����У׼������������0-10mA��	    
}ADC_ADJUST;//�ṹ��Ƕ��
extern ADC_ADJUST adc_adjust; //ADCУ׼����


#define DEFAULT_ADC_PARAM 		    4.0045      //������ѹ��ѹ��20.3K:80.6K����λΪmV
#define DEFAULT_POWER_PARAM         4.035       //��Դ��ѹ��ѹ��49.9K:200K
#define DEFAULT_I1_PARAM			1.07        //��������Ϊ49.9R��С���̣��Ŵ���25����ѹ�����20.3K:30.9K (3.3/4096)*(51.2/30.9)/25/49.9 
#define DEFAULT_I2_PARAM            5.35		//��������Ϊ49.9R�������̣��Ŵ���5����ѹ�����20.3K:30.9K(3.3/4096)*(51.2/30.9)/5/49.9 

void AdcConfig(void);
extern u16 GetAdcValue(uint8_t adc1_channel, u8 retry);
extern u16 GetBatteryVoltage(void);
extern s16 GetLinABAdcValue(void);
extern s16 GetBusVoltageFromAdcValue(s16 AdcValue);
extern u16 GetBusVoltage(u8 retry, u16* AdcValue);
extern u16 GetChannelxVoltage(u8 ADC_Channel_x, u8 retry);
extern u16 Adc2VoltageOrCurrent(u16 Adc, ADJUST_PARAMETER* Adjust, float Default);
extern u16 GetBusCurrent(u8 rd_count, u16* AdcValue);
extern u16 GetBusLeakCurrent(u16* AdcValue);
extern u8 BusIsShort(void);
extern u8 BusLeakIsShort(void);
extern u16  EliminateBusCurrentErr(void);
//extern u8 FirstDetInsert(void);
extern u8 BusCurrentIsZero(void);
extern u8 DetIsInsert(void);

#endif

