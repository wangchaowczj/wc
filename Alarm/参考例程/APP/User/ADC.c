#include "includes.h"

ADC_ADJUST AdcAdjust;     //ADCУ׼����
static  OS_EVENT   *AdcMutex;


static void AdcLock(void)
{
	INT8U  err;
	
	if(AdcMutex == 0)
	{
		AdcMutex = OSMutexCreate(APP_ADC_MUTEX_PRIO, &err);
	}
	OSMutexPend(AdcMutex,0,&err);
}
static void AdcUnlock(void)
{	
	OSMutexPost(AdcMutex);
}

/**
  *************************************************************************************
  * @brief  ADC�ܽ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void AdcConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	
	//GPIO��ADC1ʱ������
	RCC_APB2PeriphClockCmd(AD_LINE_I_PORT_RCC | AD_LINA_PORT_RCC | AD_LINB_PORT_RCC | AD_HV_PORT_RCC | ADC1_RCC, ENABLE);

	//GPIO����ΪADCģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
			
	GPIO_InitStruct.GPIO_Pin = AD_LINE_I_PIN;
	GPIO_Init(AD_LINE_I_PORT, &GPIO_InitStruct);
 	

 	
	GPIO_InitStruct.GPIO_Pin = AD_LINA_PIN;
	GPIO_Init(AD_LINA_PORT, &GPIO_InitStruct);
 	
	GPIO_InitStruct.GPIO_Pin = AD_LINB_PIN;
	GPIO_Init(AD_LINB_PORT, &GPIO_InitStruct);
 	 	
	GPIO_InitStruct.GPIO_Pin = AD_HV_PIN;
	GPIO_Init(AD_HV_PORT, &GPIO_InitStruct);
 
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//�ֲ�Ҫ��ADC��Ƶ�ʲ��ܳ�ʱ14M
	
	//ADC1��������
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;	  //������ת��
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	 //�����ⲿ��������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;	  //ͨ��Ϊ1
	ADC_Init(ADC1, &ADC_InitStruct);
	
	  
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);

	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);

	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	 	
}


/**
  *************************************************************************************
  * @brief  ������(16λ)�Ӵ�С��˳������
  * @param  ptr_s ָ�룬ָ������ǰ������
  * @param  ptr_d ָ�룬ָ������������
  * @param  lenghtҪ��������ݸ���  
  * @retval ��
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void U16DataFallRank(u16* ptr_s, u16* ptr_d, u16 lenght)
{
	u16 i,j,temp;
	u8 flag_bit;
	
	if(lenght == 0)
	{
		return ;
	}
	memcpy((u8*)ptr_d, (u8*)ptr_s, lenght*2);
	for(i=0; i < lenght-1; i++)
	{
		flag_bit = 1;
		for(j=0; j < lenght-i-1; j++)
		{
			 if(*(ptr_d+j)<*(ptr_d+j+1))
			 {
				 flag_bit = 0 ;
				 temp = *(ptr_d+j) ;
				 *(ptr_d+j) = *(ptr_d+j+1);
				 *(ptr_d+j+1)= temp ;
			 }
		}
		if(flag_bit)//��ǰ��������
		{
			break ;
		}		
	}
}

/**
  *************************************************************************************
  * @brief  ��ȡADC����ֵ
  * @param  adc1_channel  Ҫ������ͨ����
  * @param  retry ���Դ���
  * @retval ��
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
u16 GetAdcMeasureValue(ADC1_CHANNEL adc1_channel, u8 retry)
{
	u16 ad_value = 0;
	u16 ad_temp[10], rank_temp[10];
	u8 i;
	
	AdcLock();//����ʹ��ADC��Դ

	if(retry == 0)
	{
		return 0;
	}
	else if(retry > 10)
	{
		retry = 10;
	}
	
	ADC_RegularChannelConfig(ADC1, (uint8_t)adc1_channel, 1, ADC_SampleTime_1Cycles5);
 	for(i=0; i<retry; i++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //���ת���Ƿ�ʼ��1��ʾ��ʼ	
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT)); //���ת���Ƿ�ʼ��1��ʾ��ʼ
		
		ad_temp[i] = ADC_GetConversionValue(ADC1);  //��ȡDRʱ�Զ����־
		ad_value += ad_temp[i];	
		if(i != (retry-1))//���һ�β�������ʱ
		{
			SystemDelay10us(1);
		}		
	}
	
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	ADC_ClearFlag(ADC1,ADC_FLAG_STRT);//��������־
	
	if(retry > 5)
	{
		ad_value = 0;
		U16DataFallRank(ad_temp, rank_temp, retry);//�Ӵ�С����
		for(i=2; i < retry-2; i++)//ȥ��2�����ֵ��2����Сֵ����ƽ��ֵ
		{
			ad_value += rank_temp[i];
		}
		ad_value /= (retry-4);
	}
	else
	{
		ad_value /= retry;
	}
	
	AdcUnlock();//�ͷ���Դ
	
	return ad_value;
}

/**
  *************************************************************************************
  * @brief  ��ȡ��������ֵ
  * @param  AdcMeasureValue  ������Ӧ��ADCֵ
  * @retval ����ֵ,��λ0.01uA
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
u16 GetCurrentValue(u16* AdcMeasureValue)
{
	double  value = 0;
	
	value = (double)GetAdcMeasureValue(AD_LINE_I, 10);
	if(AdcMeasureValue != NULL)
	{
		*AdcMeasureValue = (u16)value;//����AD����ֵ	
	}
	
	if((u16)(AdcAdjust.Current.k*1000) == 0)
	{
		value = value*AD_LV_CURRENT_DEFAULT_PARAM;
	}
	else
	{
		if(AdcAdjust.Current.b&0x8000)
		{//����bΪ��
			value = value*AdcAdjust.Current.k - (AdcAdjust.Current.b&0x7FFF);
		}
		else
		{
			value = value*AdcAdjust.Current.k + (AdcAdjust.Current.b&0x7FFF);
		}
	}
	if(value < 0)
	{
		return 0;
	}	
	return ((u16)value);	
}
/**
  *************************************************************************************
  * @brief  ��ȡ��ѹ����ֵ
  * @param  AdcMeasureValue  ������Ӧ��ADCֵ
  * @retval ����ֵ,��λmV
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
u16 GetLinABVoltage(u16* AdcMeasureValue)
{
	u16 lina,linb;
	u16 value = 0;
	u8 i;
	u16 ad_temp[10], rank_temp[10];
	
	for(i = 0; i<10; i++)
	{
		SystemDelay1ms(1);//��ʱ1ms		
		lina = GetAdcMeasureValue(AD_LINA,1);
		linb = GetAdcMeasureValue(AD_LINB,1);
		ad_temp[i] = abs(lina - linb);		
	}
	U16DataFallRank(ad_temp, rank_temp, 10);//�Ӵ�С����
	for(i=2; i < 8; i++)//ȥ��2�����ֵ��2����Сֵ����ƽ��ֵ
	{
		value += rank_temp[i];
	}
	value /= 6;	
	if(AdcMeasureValue != NULL)
	{
		*AdcMeasureValue = value;
	}
	if((u16)(AdcAdjust.Voltage.k*1000) == 0)
	{
		value = value*AD_LINA_DEFAULT_PARAM;
	}
	else
	{
		if(AdcAdjust.Voltage.b&0x8000)
		{//����bΪ��
			value = value*AdcAdjust.Voltage.k - (AdcAdjust.Voltage.b&0x7FFF);
		}
		else
		{
			value = value*AdcAdjust.Voltage.k + (AdcAdjust.Voltage.b&0x7FFF);
		}
	}
	return value;	
}
/**
  *************************************************************************************
  * @brief  ��ȡ��ѹ����ֵ
  * @param  channel  ͨ��
  * @retval ����ֵ,��λmV
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
u16 GetVoltageValue(ADC1_CHANNEL channel)
{
	double  value = 0;
	
	value = (double)GetAdcMeasureValue(channel,10);
	switch(channel)
	{
		case AD_LINA:
			value *=AD_LINA_DEFAULT_PARAM;
			break;
		case AD_LINB:
			value *=AD_LINB_DEFAULT_PARAM;
			break;	
		case AD_HV:
			value *=AD_HV_DEFAULT_PARAM;
			break;
		default:
			break;
	}
	return (u16)value;
}

