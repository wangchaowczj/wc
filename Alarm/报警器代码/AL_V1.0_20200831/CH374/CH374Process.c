#include "includes.h"



/**
  *************************************************************************************
  * @brief  CH374�жϻص�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2015��7��21��
  * @note   ������ʱ�ò�ѯ��ʽ���ӻ����жϷ�ʽ
  *************************************************************************************  
  */
void CH374InterruptCallBack(void)
{
//	if(HostInfo.SupportDeviceType == 0)
//	{//������ʱ�ò�ѯ��ʽ���ӻ����жϷ�ʽ
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


