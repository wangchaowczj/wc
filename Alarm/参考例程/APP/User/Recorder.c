#include "includes.h"

static  FIL f_fdst;      /* file objects */
static  BYTE f_Buffer[512];   /* file copy buffer */   
static  OS_EVENT   *FileSystemMutex;

void FileSystemInitOS(void)
{
	INT8U  err;
	
	FileSystemMutex = OSMutexCreate(RES_FILE_MUTEX_PRIO, &err);
}
void FileSystemLock(void)
{
	INT8U  err;
	
    if(FileSystemMutex == 0)
    {
        FileSystemInitOS();
    }
	OSMutexPend(FileSystemMutex,0,&err);
}
void FileSystemUnlock(void)
{	
	OSMutexPost(FileSystemMutex);
}

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
			FLASH_Unlock();
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
		AdcAdjust.Current.k = 0;
		AdcAdjust.Current.b	= 0;
		
		AdcAdjust.Voltage.k = 0;
		AdcAdjust.Voltage.b	= 0;
		
		memset(&AdcAdjust.Time[0], 0, 6);	
	}
	else
	{
		memcpy((char*)&AdcAdjust, temp, sizeof(ADC_ADJUST));
	}
	
		
	//测试参数
	if(ERROR == ReadFlashDataWithCRC16(FACTORY_CFG_TEST_PARAM_ADDR, FACTORY_CFG_TEST_PARAM_SIZE, temp))
	{	
		TestParam.DeviceType = 0x01;	
		TestParam.DetType = 0x01;
		TestParam.LVCurrentMin = 1000;
		TestParam.LVCurrentMax = 3200;
		TestParam.LVCurrentTimeout = 30;
		TestParam.HVCurrentMin = 1000;
		TestParam.HVCurrentMax = 3400;
		TestParam.HVCurrentTimeout = 150;
		TestParam.FreqMin = 60000;
		TestParam.FreqMax = 105000;				
		TestParam.CtrlMaskBit = 0x03;
	}
	else
	{
		memcpy((char*)&TestParam, temp, sizeof(TEST_PARAM));
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
	memcpy(temp, (char*)&AdcAdjust, sizeof(ADC_ADJUST));
	
	if(ERROR == WriteFlashDataWithCRC16(FACTORY_CFG_ADJUST_ADDR, FACTORY_CFG_ADJUST_SIZE, temp))
	{
		return ERROR;
	}
	
	memset(temp, 0, sizeof(temp));
	memcpy(temp, (char*)&TestParam, sizeof(TEST_PARAM));
	if(ERROR == WriteFlashDataWithCRC16(FACTORY_CFG_TEST_PARAM_ADDR, FACTORY_CFG_TEST_PARAM_SIZE, temp))
	{
		return ERROR;
	}	
	
	return SUCCESS;
}


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
  * @brief  日志文件初始化
  * @param  无
  * @retval 最新日志文件名
  * @author ZH
  * @date   2020年8月4日
  * @note   无
  *************************************************************************************  
  */
u8 LogInit(void)
{
	char buffer[100];
	u16 NameLegth;
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
    int NameMax = 0;
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

	FileSystemLock();//申请文件资源，必需成对出现

    res = f_opendir(&dir, (char*)LOG_PATH);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) 
			{                    /* It is a directory */
//				sprintf(buffer, "%s/%s/",path, fn);
//				res = f_unlink((const TCHAR *)&buffer[0]);
//				if (res != FR_OK)
//				{
//					return ERROR;
//				}
            } 
			else 
			{  /* It is a file. */
                NameLegth = strlen(fn);
                
                //0.txt至50.txt
				if ((NameLegth > 4) && (NameLegth < 7) &&
                    ((0 == memcmp(fn+NameLegth-4, ".TXT", 4)) || (0 == memcmp(fn+NameLegth-4, ".txt", 4))))
				{//文件名格式检查
                    for(i=0; i < (NameLegth-4);i++)
                    {
                        if((*(fn+i) < '0' ) || (*(fn+i) > '9'))
                        {
                            break;
                        }
                    }
                    if(i == (NameLegth-4))
                    {
                        int Index = 0;
                    
                        memset(buffer, 0, sizeof(buffer));
                        memcpy(buffer, fn, NameLegth-4);
                        sscanf(buffer, "%d", &Index);
                        
                        if(Index > NameMax)
                        {
                            NameMax = Index;
                        }
                        continue;
                    }
				}
                sprintf(buffer, "%s/%s",LOG_PATH, fn);
                res = f_unlink(buffer);
            }
        }
    }
    else
    {
        f_mkdir(LOG_PATH);
    }
	FileSystemUnlock();//释放资源，必需成对出现
    
    return NameMax;
}
/**
  *************************************************************************************
  * @brief  获取当前日志大小
  * @param  无
  * @retval 日志大小
  * @author ZH
  * @date   2020年8月4日
  * @note   无
  *************************************************************************************  
  */
u32 GetLogFileSize(u8 Index)
{
	u32 fileSize = 0;

    if(!OpData.HasSdCard)
    {
        return 0;
    }
    
    FileSystemLock();//申请文件资源，必需成对出现
    
	sprintf((char*)f_Buffer, "%s/%d.txt", LOG_PATH, Index);		
	if(FR_OK != f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_EXISTING))
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return 0;
	}
    fileSize = f_size(&f_fdst);
    f_close(&f_fdst);
    
    FileSystemUnlock();//释放资源，必需成对出现
	return fileSize;
}

/**
  *************************************************************************************
  * @brief  获取当前日志大小
  * @param  无
  * @retval 日志大小
  * @author ZH
  * @date   2020年8月4日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus DeleteLogFile(u8 Index)
{
    if(!OpData.HasSdCard)
    {
        return ERROR;
    }
    
    FileSystemLock();//申请文件资源，必需成对出现
    sprintf((char*)f_Buffer, "%s/%d.txt", LOG_PATH, Index);	
    if(FR_OK != f_unlink((char*)f_Buffer))
    {
        FileSystemUnlock();//释放资源，必需成对出现
        return ERROR;
    }

    FileSystemUnlock();//释放资源，必需成对出现
	return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  保存系统运行日志
  * @param  data 要保存的数据数据指针
  * @param  len   要保存的数据长度
  * @param  op_type   要保存的数据格式
  * @retval ErrorStatus  保存结果
  *    @arg @b TRUE    写入成功
  *    @arg @b FALSE   写入失败 
  * @author ZH
  * @date   2014年12月03日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus WriteLog(void* data, u16 len, DATE_WRITE_TYPE op_type)
{
    FRESULT res;         /* FatFs function common result code */
    UINT  bw;         /* File read/write count */
//	DIR dir;
	u16 i,j, w_size, w_start=0;
    
    if(!OpData.HasSdCard)
    {
        return ERROR;
    }
    
	FileSystemLock();//申请文件资源，必需成对出现
	
//	res = f_opendir(&dir, LOG_PATH);
//	if (res != FR_OK)
//	{//创建目录
//		res = f_mkdir(LOG_PATH);
//		if (res != FR_OK)
//		{
//			FileSystemUnlock();//释放资源，必需成对出现
//			return ERROR;
//		}
//	}
//	res = f_chmod (LOG_PATH, AM_ARC, AM_RDO|AM_SYS|AM_HID|AM_ARC);
//	if (res != FR_OK)
//	{
//		FileSystemUnlock();//释放资源，必需成对出现
//		return ERROR;
//	}
	sprintf((char*)f_Buffer, "%s/%d.txt", LOG_PATH, OpData.LogName);		
	res = f_open(&f_fdst, (const char*)f_Buffer, FA_OPEN_ALWAYS | FA_WRITE);
	if(res)
	{
		FileSystemUnlock();//释放资源，必需成对出现
		return ERROR;
	}
	f_lseek(&f_fdst,f_size(&f_fdst));
	if(NEW_STR_INFO == op_type)
	{
        if(len == 0)
        {
            len = strlen(data);
        }
        f_write(&f_fdst, "\r\n------Start------\r\n", strlen("\r\n------Start------\r\n"), &bw);
		
		res = f_write(&f_fdst, data, len, &bw);		
	}
	else if(STR_INFO == op_type)
	{
        if(len == 0)
        {
            len = strlen(data);
        }
		f_write(&f_fdst, "\r\n", strlen((const char*)"\r\n"), &bw);
		res = f_write(&f_fdst, data, len, &bw);
	}
	else if(U16_DATA == op_type)
	{
		f_write(&f_fdst, "\r\n", strlen((const char*)"\r\n"), &bw);
		f_printf(&f_fdst, "%d", *((u16*)data));            
	}
	else if(HEX_ARRAY == op_type)
	{
		if(len > 100)
		{
			w_size = 100;
			w_start = len-100;
			sprintf((char*)f_Buffer,"\r\n数据%d字节,以下为后100字节:\r\n", len);	
		}
		else
		{
			w_size = len;
			sprintf((char*)f_Buffer,"\r\n数据%d字节:\r\n", len);		
		}
		f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);
		j = 0;
		for(i=0;i<w_size;i++)
		{
			j += sprintf((char*)&f_Buffer[j], "%02X ", *((u8*)data+w_start+i));
		}
		f_write(&f_fdst, (char*)f_Buffer, j, &bw);
	}
	else if(HEX_ARRAY_NO_TITLE == op_type)
	{
		if(len > 100)
		{
			w_size = 100;
		}
		else
		{
			w_size = len;
		}
		j = 0;
		for(i=0;i<w_size;i++)
		{
			j += sprintf((char*)&f_Buffer[j], "%02X ", *((u8*)data+i));
		}
		f_write(&f_fdst, "\r\n", strlen("\r\n"), &bw);
		f_write(&f_fdst, (char*)f_Buffer, j, &bw);
	}	
	else if(H32_ARRAY == op_type)
	{       
		f_write(&f_fdst, "\r\n", strlen("\r\n"), &bw);

		for(i=0;i<len;i++)
		{
            f_printf(&f_fdst, "%lu ", *((u32*)data+i));
		}		
	}
	else if(DEV_START == op_type)
	{
		f_write(&f_fdst, "\r\n------Start------", strlen("\r\n------Start------"), &bw);	
		
		f_write(&f_fdst, "\r\n设备上电", strlen("\r\n设备上电"), &bw);	
		f_write(&f_fdst, "\r\n软件版本:", strlen("\r\n软件版本:"), &bw);	
		f_write(&f_fdst, VERSION_STRING, strlen(VERSION_STRING), &bw);
		f_write(&f_fdst, "\r\n硬件版本:", strlen("\r\n硬件版本:"), &bw);	
		f_write(&f_fdst, DeviceInfo.HW, strlen((char*)DeviceInfo.HW), &bw);

//        f_write(&f_fdst, "\r\nSN:", strlen("\r\nSN:"), &bw);
//        GetSTM32ChipIDStr(f_Buffer);
//        f_write(&f_fdst, f_Buffer, strlen((const char*)f_Buffer), &bw);	
	}
    else if(LOG_DET_INFO == op_type)
    {
        char Outside[14];
        
        if(len == 0)
        {
            len = strlen(data);
        }
        f_write(&f_fdst, "\r\n------Start------\r\n", strlen("\r\n------Start------\r\n"), &bw);
		
		res = f_write(&f_fdst, data, len, &bw);
        
        memset(Outside, 0, 14);
        if(XA3_DetInside2Outside(XA3_DetInfo.Serl.U32Value, XA3_DetInfo.Pwd.U32Value, (u8*)Outside) == ERROR)
        {
            strcpy(Outside, "未知");
        }
        
        sprintf((char*)f_Buffer, "0x%04X 电流:%duA %duA 内码:%02X%02X%02X%02X%02X%02X%02X%02X 管码:%s", 
            (u16)XA3_DetInfo.state, XA3_DetInfo.lv_current/100, XA3_DetInfo.lv_reverse_current/100,
            XA3_DetInfo.Serl.U8Buffer[0], XA3_DetInfo.Serl.U8Buffer[1], XA3_DetInfo.Serl.U8Buffer[2], XA3_DetInfo.Serl.U8Buffer[3], 
            XA3_DetInfo.Pwd.U8Buffer[0], XA3_DetInfo.Pwd.U8Buffer[1], XA3_DetInfo.Pwd.U8Buffer[2], XA3_DetInfo.Pwd.U8Buffer[3], Outside);
        f_write(&f_fdst, (char*)f_Buffer, strlen((const char*)f_Buffer), &bw);
    }
	f_close(&f_fdst);
	FileSystemUnlock();//释放资源，必需成对出现
	
	return SUCCESS;		
}


