#include "includes.h"

/**
  *************************************************************************************
  * @file    Reorder.c
  * @author  
  * @version V1.0
  * @date    2020��8��28��
  * @brief   Reorderģ��Դ�ļ�
  *************************************************************************************
  */

//==================================================================================================
//| �������� | ReadAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| �������� | ��ȡ������AT45DB�е����ݲ��ж�CRCУ���ֽ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | addr    Ҫ��ȡ��������ʼ��ַ
//|          | length  Ҫ��ȡ�����ݳ���(������2�ֽ�CRC16У��)
//|          | ptr     Ҫ��������ݻ�����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | TRUE    ��ȡ�ɹ���У��ͨ��
//|          | FALSE   У��ʧ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ����ʹ��SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus ReadFlashDataWithCRC16(u32 addr, u16 length, u8* ptr)
{
	u16 i,value;
	u8 crc16[2];

	if((length%2) == 0)
	{
		if((addr >= FACTORY_CFG_START_ADDR) && ((addr+length) <= (FACTORY_CFG_END_ADDR+1)))
		{//��ֹ��ַ����		
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
//| �������� | WriteAt45dbInfoCRC16 
//|----------|--------------------------------------------------------------------------------------
//| �������� | �������ݵ�AT45DB�в�����CRCУ���ֽ�
//|----------|--------------------------------------------------------------------------------------
//| ������� | addr    Ҫд�����ʼ��ַ
//|          | length  Ҫд������ݳ���(������2�ֽ�CRC16У��)
//|          | ptr     Ҫд������ݻ�����
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | TRUE    д�ɹ�
//|          | FALSE   дʧ��
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�֣��    ʱ�䣺2013-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | ����ʹ��SetSysTickTimer2
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus WriteFlashDataWithCRC16(u32 addr, u16 length, u8* ptr)
{
	u16 i, value;
	u8 crc16[2];
	
	if((length%2) == 0)
	{
		if((addr >= FACTORY_CFG_START_ADDR) && ((addr+length) <= (FACTORY_CFG_END_ADDR+1)))
		{//��ֹ��ַ����
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
  * @brief  ���������õĲ���ֵ
  * @param  �� 
  * @retval �� 
  * @author ZH
  * @date   2014��10��16��
  * @note   ����ֵMCU�ڲ�FLASH�����һҳ��2K��С
  *************************************************************************************  
  */
void ReadFactoryConfigParam(void)
{
	u8 temp[100];
	
	//�豸ID
	ReadFlashDataWithCRC16(FACTORY_CFG_DEV_ID_ADDR, FACTORY_CFG_DEV_ID_SIZE, DeviceInfo.ID);
	
	//�豸Ӳ���汾
	ReadFlashDataWithCRC16(FACTORY_CFG_HW_VERSION_ADDR, FACTORY_CFG_HW_VERSION_SIZE, DeviceInfo.HW);
	
	//��У׼����
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
  * @brief  ����������õĲ���ֵ
  * @param  �� 
  * @retval ������
  *         @arg @b ERROR   ����ʧ��
  *         @arg @b SUCCESS ����ɹ�
  * @author ZH
  * @date   2014��10��16��
  * @note   ����ֵ������MCU�ڲ�FLASH�����һҳ��2K��С
  *************************************************************************************  
  */
ErrorStatus WriteFactoryConfigParam(void)
{
	u8 temp[100];
	
	//����FLASH
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
	
	//дУ׼����
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
  * @brief  ��ת��BootLoader
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2018��6��19��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SetStayInBoot(void)
{
	//����FLASH
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
