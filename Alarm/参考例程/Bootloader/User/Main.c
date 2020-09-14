#include "includes.h"
USB_STAT usb_stat;

/**@addtogroup PowerBoard
  *@{
  */
/**@defgroup Main
  *@brief 系统入口模块
  *@{
  */
/**@defgroup Main_Variable
  *@brief 全局变量定义
  *@{
  */ 
/**
  *@} Main_Variable
  */
/**@defgroup Main_Functions
  *@brief 函数
  *@{
  */ 
/**
  *************************************************************************************
  * @brief  系统入口函数
  * @param  无
  * @retval 无
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
				UsbRespond(&GusetTempBuffer, &GusetSendBuffer);//对接收到的数据进行处理
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
