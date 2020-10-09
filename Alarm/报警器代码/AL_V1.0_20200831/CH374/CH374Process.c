#include "includes.h"



/**
  *************************************************************************************
  * @brief  CH374中断回调函数
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2015年7月21日
  * @note   做主机时用查询方式，从机用中断方式
  *************************************************************************************  
  */
void CH374InterruptCallBack(void)
{
//	if(HostInfo.SupportDeviceType == 0)
//	{//做主机时用查询方式，从机用中断方式
//		while(GET_CH374_INT_PIN() == 0)
//		{
//			CH374DeviceInterrupt();
//		}	
//	}	
    		while(GET_CH374_INT_PIN() == 0)
		{
			CH374DeviceInterrupt();
		}
}


