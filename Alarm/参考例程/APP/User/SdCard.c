/**
  *************************************************************************************
  * @file    SdCard.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SD卡模块源文件
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SdCard
  *@{
  */
static u8 SdCardType;			/**<SD卡类型*/
static u32 SdCardDelay1sCount;  /**<延时1秒计数器*/

/**@defgroup SdCard_Functions
  *@{
  */

/**
  *************************************************************************************
  * @brief  等待SD卡准备好 
  * @param  无
  * @retval 等待结果
  *         @arg @b ERROR   没有准备好
  *         @arg @b SUCCESS 已经准备好  
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardWaitReady(void)
{
	uint8_t res;
    uint32_t retry;

    retry = SdCardDelay1sCount >> 1;        // 需要延时500ms
    
	Spi1SendByte(0xFF);
	do
	{
		res = Spi1SendByte(0xFF);
	}	
	while ((res != 0xFF) && (--retry));

    if(0 != retry)
    {
	    return SUCCESS;
    }

    return ERROR;
}

/**
  *************************************************************************************
  * @brief  SD卡发送指令
  * @param[in]  cmd: 需要发送的指令
  * @param[in]  arg: 指令参数
  * @param[out] rtn: 指令执行完成后返回的状态
  * @param[in]  rlength: 返回状态长度
  * @retval 无
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
void SdCardSendCmd(u8 cmd, u32 arg, u8 *rtn, u8 rlength)
{
    uint8_t i;
    uint8_t retry;

	if (cmd & 0x80) 
	{	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		SdCardSendCmd(SDCARD_CMD55, 0, rtn, rlength);
        if(*rtn > 1)
		{
			return;
		}			
	}

    SPI1_CS_H();                               // CS引脚拉高
    SPI1_CS_L();                                // CS引脚拉低
    if(SdCardWaitReady()==ERROR)
    {
        *rtn = 0xFF;
        return;
    }

    /* 发送指令 */
    Spi1SendByte(cmd);
    Spi1SendByte((uint8_t)(arg>>24));
    Spi1SendByte((uint8_t)(arg>>16));
    Spi1SendByte((uint8_t)(arg>>8));
    Spi1SendByte((uint8_t)arg);
    if(SDCARD_CMD0 == cmd)
    {
		Spi1SendByte(0x95);                   // 8位CRC校验，暂时没有实现所有数据都使用CRC校验
	}
    else if(SDCARD_CMD8 == cmd)
    {
		Spi1SendByte(0x87);                   // 8位CRC校验，暂时没有实现所有数据都使用CRC校验
	}    
    else
    {
		Spi1SendByte(0x01);                   // 8位CRC校验，其他指令没有校验
	}    

	/* Receive command response */
	if (SDCARD_CMD12 == cmd)
	{
		Spi1SendByte(0xFF); /* Skip a stuff byte when stop reading */
	}		

    /* 等待SD卡返回 */
    if(rlength>0)
    {
        retry = 0;
        do
        {// 等待SD卡返回状态
            *rtn = Spi1SendByte(0xFF);
            if(*rtn!=0xFF)
            {// SD卡状态返回成功
                for (i=0; i<rlength-1; i++)
                {// 保存SD卡返回的状态
                    rtn++;
                    *rtn = Spi1SendByte(0xFF);
                }
                break; // 退出
            }
        } while(retry++<10);   // 指令发送出去到指令返回有1-8个等待时间，单位是8 clock cycles
    }
}

/**
  *************************************************************************************
  * @brief  SD卡接收数据块
  * @param[out] buff: 数据缓冲区首址
  * @param[in]  bytes: 数据长度
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardRcvDataBlock(uint8_t *buff, uint32_t bytes)
{
    uint8_t tmp[7];
    uint32_t retry;
    uint32_t i;
    ErrorStatus rtn = SUCCESS;

    retry = SdCardDelay1sCount >> 3;                  // 延时125ms
    
    do
    {
        tmp[0] = Spi1SendByte(0xFF);

    }while((tmp[0]!=SDCARD_CMD17_TOKEN)&&(--retry));
    
    if(retry>0)
    {// 获得令牌成功
        for (i=0; i<bytes; i++)
        {
            *buff++ = Spi1SendByte(0xFF);
        }
        Spi1SendByte(0xFF);        // 读CRC校验,暂时不做处理
        Spi1SendByte(0xFF);
    }
    else
    {// 获得令牌失败
        rtn = ERROR;
    }

    return rtn;
}




/**
  *************************************************************************************
  * @brief  SD卡读1块数据
  * @param[out] buff:  数据缓冲区
  * @param[in]  sector:开始扇区号
  * @param[in]  count: 扇区数
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardReadBlock(uint8_t *buff, uint32_t sector, uint32_t count)
{
    uint8_t tmp[7];
    ErrorStatus rtn = SUCCESS;

	if (!(SdCardType & SDCARD_TYPE_BLOCK)) 
	{
		sector *= 512;	/* Convert to byte address if needed */
	}
    if(count==1)
    {
        /* 发送CMD17 */
        SdCardSendCmd(SDCARD_CMD17, sector, tmp, 1);       // 发送读单块数据指令,把扇区号转化为字节地址，
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD17指令执行正常
            rtn = SdCardRcvDataBlock(buff, 512);
        }
        else
        {// CMD17指令执行失败
            rtn = ERROR;
        }
    }
    else
    {
        /* 发送CMD18 */
        SdCardSendCmd(SDCARD_CMD18, sector, tmp, 1);       // 发送读多块数据指令,把扇区号转化为字节地址，
        do
        {// 循环读取多个块数据，直至读取完成或出现错误
            rtn = SdCardRcvDataBlock(buff, 512);
            if(rtn==ERROR)
            {
                break;
            }
            buff += 512;
        } while(--count);
        SdCardSendCmd(SDCARD_CMD12, 0x00, tmp, 1);     // 发送停止读取指令,例程中没有对停止指令做判断，不知道为什么，实际返回值也不对

    }

    SPI1_CS_H();                               // CS引脚拉高
    Spi1SendByte(0xFF);
    return rtn;
}

/**
  *************************************************************************************
  * @brief  SD卡发送数据
  * @param[in] buff:  数据缓冲区
  * @param[in] token: 令牌
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardSendDataBlock(const uint8_t *buff, uint8_t token)
{
    uint8_t value;

	if (SdCardWaitReady() != SUCCESS)
	{
		return ERROR;
	}		
    
    value = Spi1SendByte(token);

    if(token == SDCARD_CMD25_ST_TOKEN)
	{
		return SUCCESS;    // 如果是停止令牌，直接返回
	}		
    
    value = 0;
    do
    {
        Spi1SendByte(*buff++);
        Spi1SendByte(*buff++);
    } while(--value);    

    value = Spi1SendByte(0xFF);
    value = Spi1SendByte(0xFF);
    
    value = Spi1SendByte(0xFF);   // 实际应该是在此返回状态，测试结果是有时在此返回
    if((value&0x1F)==SDCARD_DATA_RESP_ACCEPTED)
    {
        return SUCCESS;
    }

    return ERROR;
}

//==================================================================================================
//| 函数名称 | MMC_blockwrite 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 写数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | buff:数据缓冲区
//|          | sector:开始扇区号
//|          | count:扇区数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：    时间：2013-04-17  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus SdCardWriteBlock(const uint8_t *buff, uint32_t sector, uint32_t count)
{
    uint8_t tmp[7];
    ErrorStatus rtn = SUCCESS;

	if (!(SdCardType & SDCARD_TYPE_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

    if(count==1)
    {
        /* 发送CMD24 */
        SdCardSendCmd(SDCARD_CMD24, sector, tmp, 1);       // 发送写单块数据指令,把扇区号转化为字节地址，
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD24指令执行正常
            rtn = SdCardSendDataBlock(buff, SDCARD_CMD17_TOKEN);
        }
        else
        {
            rtn = ERROR;
        }
    }
    else
    {
		if (SdCardType & SDCARD_TYPE_SDC)
		{
			SdCardSendCmd(SDCARD_ACMD23, count, tmp, 1);
		}			
        /* 发送CMD25 */
        SdCardSendCmd(SDCARD_CMD25, sector, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD25指令执行正常
            do
            {
                rtn = SdCardSendDataBlock(buff, SDCARD_CMD25_TOKEN);
                if(rtn==ERROR)
                {
                    break;
                }
                buff += 512;
            } while(--count);
            rtn = SdCardSendDataBlock(tmp, SDCARD_CMD25_ST_TOKEN);         // 停止传送
        }
        else
        {
            rtn = ERROR;
        }
    }

    SPI1_CS_H();                               // CS引脚拉高

    Spi1SendByte(0xFF);

    return rtn;
}

/**
  *************************************************************************************
  * @brief  SD卡状态控制
  * @param[out] buff:  数据缓冲区
  * @param[in]  sector:开始扇区号
  * @param[in]  count: 扇区数
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   用于FATFS文件系统作大容量存储模式时使用
  *************************************************************************************  
  */
ErrorStatus SdCardIoCtrl(uint8_t cmd, uint8_t *buff)
{
    uint8_t tmp[7];
	ErrorStatus res = ERROR;
	uint8_t n, csd[16], *ptr = buff;
	uint16_t csize;

	if (cmd == CTRL_POWER) 
	{
		switch (*ptr) 
		{
			case 0:		/* Sub control code == 0 (POWER_OFF) */
				res = SUCCESS;
				break;
			case 1:		/* Sub control code == 1 (POWER_ON) */
				res = SUCCESS;
				break;
			case 2:		/* Sub control code == 2 (POWER_GET) */
				*(ptr+1) = 1;
				res = SUCCESS;
				break;
			default :
				res = ERROR;
				break;
		}
	}
	else 
	{
		switch (cmd) 
		{
			case CTRL_SYNC :		/* Make sure that no pending write process */
				SPI1_CS_L();                                // CS引脚拉低
				if (SdCardWaitReady() != ERROR)
				{
					res = SUCCESS;
				}	
				break;
			case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
				SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD9指令执行正常
					if(SdCardRcvDataBlock(csd, 16) != ERROR)
					{
						if ((csd[0] >> 6) == 1) 
						{	/* SDC version 2.00 */
							csize = csd[9] + ((WORD)csd[8] << 8) + 1;
							*(uint32_t *)buff = (uint32_t)csize << 10;
						} 
						else 
						{					/* SDC version 1.XX or MMC*/
							n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
							csize = (csd[8] >> 6) + ((uint32_t)csd[7] << 2) + ((uint32_t)(csd[6] & 3) << 10) + 1;
							*(uint32_t*)buff = (uint32_t)csize << (n - 9);
						}
						res = SUCCESS;
					}
				}
				break;
			case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
				*(uint32_t*)buff = 512;
				res = SUCCESS;
				break;
			case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
				if (SdCardType & SDCARD_TYPE_SD2) 
				{	/* SDC version 2.00 */
					SdCardSendCmd(SDCARD_ACMD13, 0, tmp, 2);       // 发送写多块数据指令,把扇区号转化为字节地址，
					if(tmp[0]==SDCARD_R1_NOERR)
					{// ACMD13指令执行正常
						if(SdCardRcvDataBlock(csd, 16) == SUCCESS)
						{
							for (n = 64 - 16; n; n--)
							{
								Spi1SendByte(0xFF);	/* Purge trailing data */
							}						
							*(uint32_t *)buff = 16UL << (csd[10] >> 4);
							res = SUCCESS;
						}
					}
				} 
				else 
				{					/* SDC version 1.XX or MMC */
					SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
					if(tmp[0]==SDCARD_R1_NOERR)
					{// CMD13指令执行正常
						if(SdCardRcvDataBlock(csd, 16) == SUCCESS)
						{
							if (SdCardType & SDCARD_TYPE_SD1) 
							{	/* SDC version 1.XX */
								*(uint32_t *)buff = (((csd[10] & 63) << 1) + ((uint32_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
							} 
							else 
							{					/* MMC */
								*(uint32_t*)buff = ((uint32_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
							}
							res = SUCCESS;
						}
					}
				}
				break;
			case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
				*ptr = SdCardType;
				res = SUCCESS;
				break;
			case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
				SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0] == SDCARD_R1_NOERR)
				{// CMD9指令执行正常
					if(SdCardRcvDataBlock(csd, 16)==SUCCESS)
					{
						res = SUCCESS;
					}
				}
				break;

			case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
				SdCardSendCmd(SDCARD_CMD10, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD10指令执行正常
					if(SdCardRcvDataBlock(csd, 16)== SUCCESS)
					{
						res = SUCCESS;
					}
				}
				break;
			case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
				SdCardSendCmd(SDCARD_CMD58, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD58指令执行正常
					for (n = 4; n; n--) 
					{
						*ptr++ = Spi1SendByte(0xFF);
					}
					res = SUCCESS;
				}
				break;
			case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
				SdCardSendCmd(SDCARD_ACMD13, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// ACMD13指令执行正常
					Spi1SendByte(0xFF);
					if (SdCardRcvDataBlock(ptr, 64) == SUCCESS)
					{
						res = SUCCESS;
					}	
				}
				break;
			default:
				res = ERROR;
				break;
		}

        SPI1_CS_H();                               // CS引脚拉高   
        Spi1SendByte(0xFF);
	}

	return res;
}

/**
  *************************************************************************************
  * @brief  SD卡上电
  * @param  无
  * @retval 上电结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardPowerOn(void)
{
    uint16_t baud;                                  // 备份SPI时钟频率使用，中间不可更改
    uint8_t tmp[7];
    uint32_t retry;
    uint8_t cdtype = 0x00;
    ErrorStatus rtn = ERROR;

    baud = Spi1GetBaud();
    Spi1SetBaud(SPI_BaudRatePrescaler_256);        // 进入低速模式
    
    SPI1_CS_H();                               // CS引脚拉高

    for(retry=0; retry<15; retry++)                 // MMC/SD的DI引脚拉高，并发送>74时钟脉冲
    {
        Spi1SendByte(0xFF);
    }

    SdCardDelay1sCount = Spi1GetFrequency() >> 3;  // 延时1s需要发送的字节数    
    
    /* 选中SD卡 */
    SPI1_CS_L();                                // CS拉低

    SdCardSendCmd(SDCARD_CMD0, 0x00, tmp, 1);       // 发送指令0,并接收R1,复位进入IDLE模式

    if(tmp[0]==0x01)
    {// 指令0执行成功
        SdCardSendCmd(SDCARD_CMD8, 0x1AA, tmp, 5);  // 发送指令8,检查设备电压
        if(tmp[0]==0x01)                       // SDHC
        {
            if((tmp[3]==0x01)&&(tmp[4]==0xAA)) // 设备可以工作在2.7-3.3V
            {//ocr = 0x1AA
                /* 发送ACMD41 */
                retry = SdCardDelay1sCount;
                do
                {// ACMD41是组合指令
                    SdCardSendCmd(SDCARD_ACMD41, 1UL<<30, tmp, 1);
                } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1返回成功
                if(retry)
                {
                    SdCardSendCmd(SDCARD_CMD58, 0x00, tmp, 5);
                    cdtype = (tmp[1] & 0x40) ? SDCARD_TYPE_SD2 | SDCARD_TYPE_BLOCK : SDCARD_TYPE_SD2;
                    rtn = SUCCESS;
                }
            }
        }
        else                                   // SDSC or MMC
        {
            cdtype = SDCARD_TYPE_SD1;
            retry = SdCardDelay1sCount;
            do
            {// ACMD41是组合指令
                SdCardSendCmd(SDCARD_ACMD41, 0x00, tmp, 1);
            } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1返回成功
            if(!retry)
            {// ACMD41指令执行失败，尝试使用CMD1
                cdtype = SDCARD_TYPE_MMC;
                retry = retry;
                do
                {
                    SdCardSendCmd(SDCARD_CMD1, 0x00, tmp, 1);
                } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1返回成功
            }
            if(retry)
            {
                SdCardSendCmd(SDCARD_CMD16, 512, tmp, 1);           // 设置扇区大小为512字节
                if(tmp[0]==0x00)
                {
                    rtn = SUCCESS;
                }
            }
            else
            {
                cdtype = 0x00;
            }
        }
    }

    SdCardType = cdtype;

    SPI1_CS_H();                               // CS引脚拉高

    Spi1SendByte(0xFF);

    Spi1SetBaud(baud);                              // 恢复总线速度

    SdCardDelay1sCount = Spi1GetFrequency() >> 3;  // 延时1s需要发送的字节数

    return rtn;
}

/**
 *@} SdCard_Functions
 */
/**
 *@} SdCard
 */
/**
 *@} PowerBoard
 */  
