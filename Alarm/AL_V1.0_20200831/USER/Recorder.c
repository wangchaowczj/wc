#include "includes.h"

/**
  *************************************************************************************
  * @file    Reorder.c
  * @author  
  * @version V1.0
  * @date    2020年8月28日
  * @brief   Reorder模块源文件
  *************************************************************************************
  */

//==================================================================================================
//| 函数名称 | ReadAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 读取保存在AT45DB中的数据并判断CRC校验字节
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | addr    要读取的数据起始地址
//|          | length  要读取的数据长度(不包括2字节CRC16校验)
//|          | ptr     要保存的数据缓冲区
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | TRUE    读取成功，校验通过
//|          | FALSE   校验失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 函数使用SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus ReadFlashDataWithCRC16(u32 addr, u16 length, u8* ptr)
{
	u16 i,value;
	u8 crc16[2];

	if((length%2) == 0)
	{
		if((addr >= FACTORY_CFG_START_ADDR) && ((addr+length) <= (FACTORY_CFG_END_ADDR+1)))
		{//防止地址超限		
			for(i=0;i<length/2;i++)
			{
				value = (*(__IO u16*)(addr+i*2));
				*(ptr+i*2) = value&0xFF;
				*(ptr+i*2+1) = (value>>8)&0xFF;
			}
			value = (*(__IO u16*)(addr+i*2));
			GetCrc16Value(0xFFFF, crc16, ptr, length);
			if((crc16[0] == (value&0xFF)) && (crc16[1] == ((value>>8)&0xFF)))
			{
				return SUCCESS;
			}
		}	
	}
	memset(ptr, 0, length);
	return ERROR;
}

//==================================================================================================
//| 函数名称 | WriteAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 保存数据到AT45DB中并增加CRC校验字节
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | addr    要写入的起始地址
//|          | length  要写入的数据长度(不包括2字节CRC16校验)
//|          | ptr     要写入的数据缓冲区
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | TRUE    写成功
//|          | FALSE   写失败
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 函数使用SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus WriteFlashDataWithCRC16(u32 addr, u16 length, u8* ptr)
{
	u16 i, value;
	u8 crc16[2];
	
	if((length%2) == 0)
	{
		if((addr >= FACTORY_CFG_START_ADDR) && ((addr+length) <= (FACTORY_CFG_END_ADDR+1)))
		{//防止地址超限
			GetCrc16Value(0xFFFF, crc16, ptr, length);
			FLASH_Unlock();
			for(i=0;i<length/2;i++)
			{
				value = *(ptr+i*2) + ((u16)*(ptr+i*2+1)<<8);
				FLASH_ProgramHalfWord(addr+i*2, value);
				if(value != (*(__IO u16*)(addr+i*2)))
				{
					FLASH_Lock();
					return ERROR;
				}
			}
			value = crc16[0] + ((u16)crc16[1]<<8);
			FLASH_ProgramHalfWord(addr+i*2, value);
			if(value != (*(__IO u16*)(addr+i*2)))
			{
				FLASH_Lock();
				return ERROR;
			}		
            FLASH_Lock();
			return SUCCESS;		
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  读出厂设置的参数值
  * @param  无 
  * @retval 无 
  * @author ZH
  * @date   2014年10月16日
  * @note   参数值MCU内部FLASH的最后一页，2K大小
  *************************************************************************************  
  */
void ReadFactoryConfigParam(void)
{
	u8 temp[100];
	
	//设备ID
	ReadFlashDataWithCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE, DeviceInfo.ID);
	
	//设备硬件版本
	ReadFlashDataWithCRC16(FACTORY_CFG_HW_VERSION_ADDR, FACTORY_CFG_HW_VERSION_SIZE, DeviceInfo.HW);
	
	//读校准参数
	if(ERROR == ReadFlashDataWithCRC16(FACTORY_CFG_ADJUST_ADDR, FACTORY_CFG_ADJUST_SIZE, temp))
	{
        adc_adjust.linab.k = 0;
        adc_adjust.linab.b = 0;				
        adc_adjust.linab.year = 0;
        adc_adjust.linab.month = 0;
        adc_adjust.linab.day = 0;
        adc_adjust.linab.hour = 0;
        adc_adjust.linab.minute = 0;
        adc_adjust.linab.second = 0;
        
        adc_adjust.current1.k = 0;
        adc_adjust.current1.b = 0;				
        adc_adjust.current1.year = 0;
        adc_adjust.current1.month = 0;
        adc_adjust.current1.day = 0;
        adc_adjust.current1.hour = 0;
        adc_adjust.current1.minute = 0;
        adc_adjust.current1.second = 0;
        
        adc_adjust.current2.k = 0;
        adc_adjust.current2.b = 0;
        adc_adjust.current2.year = 0;
        adc_adjust.current2.month = 0;
        adc_adjust.current2.day = 0;
        adc_adjust.current2.hour = 0;
        adc_adjust.current2.minute = 0;
        adc_adjust.current2.second = 0;
		
	}
	else
	{
		memcpy((char*)&adc_adjust, temp, sizeof(ADC_ADJUST));
	}
			
}
/**
  *************************************************************************************
  * @brief  保存出厂设置的参数值
  * @param  无 
  * @retval 保存结果
  *         @arg @b ERROR   保存失败
  *         @arg @b SUCCESS 保存成功
  * @author ZH
  * @date   2014年10月16日
  * @note   参数值保存在MCU内部FLASH的最后一页，2K大小
  *************************************************************************************  
  */
ErrorStatus WriteFactoryConfigParam(void)
{
	u8 temp[100];
	
	//擦除FLASH
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	if(FLASH_COMPLETE != FLASH_ErasePage(FACTORY_CFG_START_ADDR))
	{
		return ERROR;		
	}
	
	if(ERROR == WriteFlashDataWithCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE, DeviceInfo.ID))
	{
		return ERROR;
	}
	
	if(ERROR == WriteFlashDataWithCRC16(FACTORY_CFG_HW_VERSION_ADDR, FACTORY_CFG_HW_VERSION_SIZE, DeviceInfo.HW))
	{
		return ERROR;
	}	
	
	//写校准参数
	memset(temp, 0, sizeof(temp));
	memcpy(temp, (char*)&adc_adjust, sizeof(ADC_ADJUST));
	
	if(ERROR == WriteFlashDataWithCRC16(FACTORY_CFG_ADJUST_ADDR, FACTORY_CFG_ADJUST_SIZE, temp))
	{
		return ERROR;
	}
			
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  跳转到BootLoader
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2018年6月19日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SetStayInBoot(void)
{
	//擦除FLASH
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
	if(FLASH_COMPLETE != FLASH_ErasePage(BOOT_CFG_START_ADDR))
	{
		return ERROR;		
	}
	FLASH_ProgramHalfWord(BOOT_CFG_START_ADDR, 0x55AA);
	if(0x55AA != (*(__IO u16*)BOOT_CFG_START_ADDR))
	{
		return ERROR;
	}
	return SUCCESS;
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
