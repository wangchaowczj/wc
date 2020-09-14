#include "includes.h"

/**
  *************************************************************************************
  * @brief  检查硬件版本是否有TF卡
  * @param  无 
  * @retval 0 没有， 1 有
  * @date   2020年8月5日
  * @note   SingleTest_V1.4(含)之后的版本才支持TF卡
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
  * @brief  软件复位
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年6月19日
  * @note   无
  *************************************************************************************  
  */
void SoftReset(void)
{
	__set_FAULTMASK(1);// 关闭所有中端
	NVIC_SystemReset();// 复位
}

/**
  *************************************************************************************
  * @brief  测试点管脚配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年6月8日
  * @note   无
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
  * @brief  配置GPIO
  * @param  无
  * @retval 无 
  * @author ZH
  * @date   2018年6月8日
  * @note   无
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
        //TF卡配置
        if(ERROR == SdCardPowerOn())
        {
            OSTimeDlyHMSM(0,0,0,200);
            if(ERROR == SdCardPowerOn())
            {
                return ERROR;		
            }
        }

        //加载sd卡文件系统
        if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS文件系统
        {	
            if(FR_OK != f_mount(&SdCardFatfs, "0:", 1))            //FATFS文件系统
            {
                return ERROR;//文件系统异常		
            }
        }

        res = f_getfree("0:", &fre_clust, &fs);//总容量：((fs->n_fatent - 2) * fs->csize)/2;剩余容量：(fre_clust * fs->csize)/2;
        if((FR_OK != res) || ((fre_clust * fs->csize) < 10240))//没有格式化或者容量小于5M
        {
            return ERROR;//文件系统异常
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
            
            //为了从文件中能看出哪个是当前日志，把最老的删掉
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

