#include "includes.h"

/**
  *************************************************************************************
  * @brief  ���Ӳ���汾�Ƿ���TF��
  * @param  �� 
  * @retval 0 û�У� 1 ��
  * @date   2020��8��5��
  * @note   SingleTest_V1.4(��)֮��İ汾��֧��TF��
  *************************************************************************************  
  */
u8 HasSdCard(void)
{
    u8 i;
    u8 len;
    
    len = strlen((const char*)DeviceInfo.HW);
    if(len > 4)
    {
        len -= 4;
        for(i=0; i<=len; i++)
        {
            //SingleTest_V1.4
            if((DeviceInfo.HW[i] == '_') && (DeviceInfo.HW[i+1] == 'V') && (DeviceInfo.HW[i+3] == '.'))
            {
                if(memcmp(&DeviceInfo.HW[i+1], "V1.4", 4) >= 0)
                {
                    return 1;
                }
            }
        }    
    }

    return 0;
}
/**
  *************************************************************************************
  * @brief  �����λ
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��6��19��
  * @note   ��
  *************************************************************************************  
  */
void SoftReset(void)
{
	__set_FAULTMASK(1);// �ر������ж�
	NVIC_SystemReset();// ��λ
}

/**
  *************************************************************************************
  * @brief  ���Ե�ܽ�����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void TestPointConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(/*TEST_POINT1_PORT_RCC | */TEST_POINT2_PORT_RCC, ENABLE);
	
//	TEST_POINT1_OFF();
	TEST_POINT2_OFF();
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
//	GPIO_InitStruct.GPIO_Pin = TEST_POINT1_PIN;
//	GPIO_Init(TEST_POINT1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = TEST_POINT2_PIN;
	GPIO_Init(TEST_POINT2_PORT, &GPIO_InitStruct);
}

/**
  *************************************************************************************
  * @brief  ����GPIO
  * @param  ��
  * @retval �� 
  * @author ZH
  * @date   2018��6��8��
  * @note   ��
  *************************************************************************************  
  */
void SystemConfig(void)
{
	SysTickConfig();
	PackageKeyInit();
	TestPointConfig();
	LedConfig();
	AdcConfig();
	UartConfig();
	DetHardwareInit();
	ReadFactoryConfigParam();
    Spi1Config();
	DeviceInfo.name = 'A';
	SET_LIN_OPEN();
}

ErrorStatus HardwareInit(void)
{
	static FATFS SdCardFatfs;
	FATFS *fs;
	FRESULT res;
	DWORD fre_clust;
    
    OpData.HasSdCard = HasSdCard();
    if(OpData.HasSdCard)
    {        
        //TF������
        if(ERROR == SdCardPowerOn())
        {
            OSTimeDlyHMSM(0,0,0,200);
            if(ERROR == SdCardPowerOn())
            {
                return ERROR;		
            }
        }

        //����sd���ļ�ϵͳ
        if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS�ļ�ϵͳ
        {	
            if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS�ļ�ϵͳ
            {
                return ERROR;//�ļ�ϵͳ�쳣		
            }
        }

        res = f_getfree("0:", &fre_clust, &fs);//��������((fs->n_fatent - 2) * fs->csize)/2;ʣ��������(fre_clust * fs->csize)/2;
        if((FR_OK != res) || ((fre_clust * fs->csize) < 10240))//û�и�ʽ����������С��5M
        {
            return ERROR;//�ļ�ϵͳ�쳣
        }
        OpData.LogName = LogInit();
        if(GetLogFileSize(OpData.LogName) > LOG_FILE_MAX)
        {
            u8 Next;
            
            OpData.LogName += 1;
            if(OpData.LogName >= 50)
            {
                OpData.LogName = 0;
            }
            DeleteLogFile(OpData.LogName);
            
            //Ϊ�˴��ļ����ܿ����ĸ��ǵ�ǰ��־�������ϵ�ɾ��
            Next = OpData.LogName;
            Next += 1;
            if(Next >= 50)
            {
                Next = 0;
            }
            DeleteLogFile(Next);
        }
        WriteLog(NULL, 0, DEV_START);
    }
    return SUCCESS;
}

