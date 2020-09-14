#include "includes.h"
USB_STAT usb_stat;

/**@addtogroup PowerBoard
  *@{
  */
/**@defgroup Main
  *@brief ϵͳ���ģ��
  *@{
  */
/**@defgroup Main_Variable
  *@brief ȫ�ֱ�������
  *@{
  */ 
/**
  *@} Main_Variable
  */
/**@defgroup Main_Functions
  *@brief ����
  *@{
  */ 
/**
  *************************************************************************************
  * @brief  ϵͳ��ں���
  * @param  ��
  * @retval ��
  *************************************************************************************
  */ 
int main(void)
{
    
    usb_stat.status = 0;
	BootToUserApp();
	
	SystemConfig();		
	
	LED3_ON();
	
	HidData.bytenum = 0;
	HidData.status = RECIEVING;
	while(1)
	{
//        LED3_TOGGLE();
//        System72MDelay1ms(100);

		if(RECIEVED == HidData.status)
		{//usb
			if(SUCCESS == Unpack(&HidData, &GusetTempBuffer))
			{
				UsbRespond(&GusetTempBuffer, &GusetSendBuffer);//�Խ��յ������ݽ��д���
				if(GusetSendBuffer.bytenum != 0)
				{
					Pack(&GusetSendBuffer, &GusetTempBuffer);
					DeviceHidSendData(&GusetTempBuffer.uch_buf[0], GusetTempBuffer.bytenum);
				}				
			}
			HidData.bytenum = 0;
			HidData.status = RECIEVING;		
		}	
	}
}

/**
  *@} Main_Functions
  */
/**
  *@} Main
  */
/**
  *@} PowerBoard
  */
