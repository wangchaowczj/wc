/**
  *************************************************************************************
  * @file    SdCard.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   SD��ģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SdCard
  *@{
  */
static u8 SdCardType;			/**<SD������*/
static u32 SdCardDelay1sCount;  /**<��ʱ1�������*/

/**@defgroup SdCard_Functions
  *@{
  */

/**
  *************************************************************************************
  * @brief  �ȴ�SD��׼���� 
  * @param  ��
  * @retval �ȴ����
  *         @arg @b ERROR   û��׼����
  *         @arg @b SUCCESS �Ѿ�׼����  
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SdCardWaitReady(void)
{
	uint8_t res;
    uint32_t retry;

    retry = SdCardDelay1sCount >> 1;        // ��Ҫ��ʱ500ms
    
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
  * @brief  SD������ָ��
  * @param[in]  cmd: ��Ҫ���͵�ָ��
  * @param[in]  arg: ָ�����
  * @param[out] rtn: ָ��ִ����ɺ󷵻ص�״̬
  * @param[in]  rlength: ����״̬����
  * @retval ��
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
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

    SPI1_CS_H();                               // CS��������
    SPI1_CS_L();                                // CS��������
    if(SdCardWaitReady()==ERROR)
    {
        *rtn = 0xFF;
        return;
    }

    /* ����ָ�� */
    Spi1SendByte(cmd);
    Spi1SendByte((uint8_t)(arg>>24));
    Spi1SendByte((uint8_t)(arg>>16));
    Spi1SendByte((uint8_t)(arg>>8));
    Spi1SendByte((uint8_t)arg);
    if(SDCARD_CMD0 == cmd)
    {
		Spi1SendByte(0x95);                   // 8λCRCУ�飬��ʱû��ʵ���������ݶ�ʹ��CRCУ��
	}
    else if(SDCARD_CMD8 == cmd)
    {
		Spi1SendByte(0x87);                   // 8λCRCУ�飬��ʱû��ʵ���������ݶ�ʹ��CRCУ��
	}    
    else
    {
		Spi1SendByte(0x01);                   // 8λCRCУ�飬����ָ��û��У��
	}    

	/* Receive command response */
	if (SDCARD_CMD12 == cmd)
	{
		Spi1SendByte(0xFF); /* Skip a stuff byte when stop reading */
	}		

    /* �ȴ�SD������ */
    if(rlength>0)
    {
        retry = 0;
        do
        {// �ȴ�SD������״̬
            *rtn = Spi1SendByte(0xFF);
            if(*rtn!=0xFF)
            {// SD��״̬���سɹ�
                for (i=0; i<rlength-1; i++)
                {// ����SD�����ص�״̬
                    rtn++;
                    *rtn = Spi1SendByte(0xFF);
                }
                break; // �˳�
            }
        } while(retry++<10);   // ָ��ͳ�ȥ��ָ�����1-8���ȴ�ʱ�䣬��λ��8 clock cycles
    }
}

/**
  *************************************************************************************
  * @brief  SD���������ݿ�
  * @param[out] buff: ���ݻ�������ַ
  * @param[in]  bytes: ���ݳ���
  * @retval ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ� 
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SdCardRcvDataBlock(uint8_t *buff, uint32_t bytes)
{
    uint8_t tmp[7];
    uint32_t retry;
    uint32_t i;
    ErrorStatus rtn = SUCCESS;

    retry = SdCardDelay1sCount >> 3;                  // ��ʱ125ms
    
    do
    {
        tmp[0] = Spi1SendByte(0xFF);

    }while((tmp[0]!=SDCARD_CMD17_TOKEN)&&(--retry));
    
    if(retry>0)
    {// ������Ƴɹ�
        for (i=0; i<bytes; i++)
        {
            *buff++ = Spi1SendByte(0xFF);
        }
        Spi1SendByte(0xFF);        // ��CRCУ��,��ʱ��������
        Spi1SendByte(0xFF);
    }
    else
    {// �������ʧ��
        rtn = ERROR;
    }

    return rtn;
}




/**
  *************************************************************************************
  * @brief  SD����1������
  * @param[out] buff:  ���ݻ�����
  * @param[in]  sector:��ʼ������
  * @param[in]  count: ������
  * @retval ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ� 
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
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
        /* ����CMD17 */
        SdCardSendCmd(SDCARD_CMD17, sector, tmp, 1);       // ���Ͷ���������ָ��,��������ת��Ϊ�ֽڵ�ַ��
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD17ָ��ִ������
            rtn = SdCardRcvDataBlock(buff, 512);
        }
        else
        {// CMD17ָ��ִ��ʧ��
            rtn = ERROR;
        }
    }
    else
    {
        /* ����CMD18 */
        SdCardSendCmd(SDCARD_CMD18, sector, tmp, 1);       // ���Ͷ��������ָ��,��������ת��Ϊ�ֽڵ�ַ��
        do
        {// ѭ����ȡ��������ݣ�ֱ����ȡ��ɻ���ִ���
            rtn = SdCardRcvDataBlock(buff, 512);
            if(rtn==ERROR)
            {
                break;
            }
            buff += 512;
        } while(--count);
        SdCardSendCmd(SDCARD_CMD12, 0x00, tmp, 1);     // ����ֹͣ��ȡָ��,������û�ж�ָֹͣ�����жϣ���֪��Ϊʲô��ʵ�ʷ���ֵҲ����

    }

    SPI1_CS_H();                               // CS��������
    Spi1SendByte(0xFF);
    return rtn;
}

/**
  *************************************************************************************
  * @brief  SD����������
  * @param[in] buff:  ���ݻ�����
  * @param[in] token: ����
  * @retval ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ� 
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
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
		return SUCCESS;    // �����ֹͣ���ƣ�ֱ�ӷ���
	}		
    
    value = 0;
    do
    {
        Spi1SendByte(*buff++);
        Spi1SendByte(*buff++);
    } while(--value);    

    value = Spi1SendByte(0xFF);
    value = Spi1SendByte(0xFF);
    
    value = Spi1SendByte(0xFF);   // ʵ��Ӧ�����ڴ˷���״̬�����Խ������ʱ�ڴ˷���
    if((value&0x1F)==SDCARD_DATA_RESP_ACCEPTED)
    {
        return SUCCESS;
    }

    return ERROR;
}

//==================================================================================================
//| �������� | MMC_blockwrite 
//|----------|--------------------------------------------------------------------------------------
//| �������� | д����
//|----------|--------------------------------------------------------------------------------------
//| ������� | buff:���ݻ�����
//|          | sector:��ʼ������
//|          | count:������
//|----------|--------------------------------------------------------------------------------------       
//| ���ز��� | 
//|----------|--------------------------------------------------------------------------------------       
//| ������� | ��д�ˣ�    ʱ�䣺2013-04-17  
//|----------|-------------------------------------------------------------------------------------- 
//|   ��ע   | 
//|----------|-------------------------------------------------------------------------------------- 
//| �޸ļ�¼ | �޸��ˣ�          ʱ�䣺         �޸����ݣ� 
//==================================================================================================
ErrorStatus SdCardWriteBlock(const uint8_t *buff, uint32_t sector, uint32_t count)
{
    uint8_t tmp[7];
    ErrorStatus rtn = SUCCESS;

	if (!(SdCardType & SDCARD_TYPE_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

    if(count==1)
    {
        /* ����CMD24 */
        SdCardSendCmd(SDCARD_CMD24, sector, tmp, 1);       // ����д��������ָ��,��������ת��Ϊ�ֽڵ�ַ��
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD24ָ��ִ������
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
        /* ����CMD25 */
        SdCardSendCmd(SDCARD_CMD25, sector, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD25ָ��ִ������
            do
            {
                rtn = SdCardSendDataBlock(buff, SDCARD_CMD25_TOKEN);
                if(rtn==ERROR)
                {
                    break;
                }
                buff += 512;
            } while(--count);
            rtn = SdCardSendDataBlock(tmp, SDCARD_CMD25_ST_TOKEN);         // ֹͣ����
        }
        else
        {
            rtn = ERROR;
        }
    }

    SPI1_CS_H();                               // CS��������

    Spi1SendByte(0xFF);

    return rtn;
}

/**
  *************************************************************************************
  * @brief  SD��״̬����
  * @param[out] buff:  ���ݻ�����
  * @param[in]  sector:��ʼ������
  * @param[in]  count: ������
  * @retval ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ� 
  * @author ZH
  * @date   2014��9��11��
  * @note   ����FATFS�ļ�ϵͳ���������洢ģʽʱʹ��
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
				SPI1_CS_L();                                // CS��������
				if (SdCardWaitReady() != ERROR)
				{
					res = SUCCESS;
				}	
				break;
			case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
				SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD9ָ��ִ������
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
					SdCardSendCmd(SDCARD_ACMD13, 0, tmp, 2);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
					if(tmp[0]==SDCARD_R1_NOERR)
					{// ACMD13ָ��ִ������
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
					SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
					if(tmp[0]==SDCARD_R1_NOERR)
					{// CMD13ָ��ִ������
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
				SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
				if(tmp[0] == SDCARD_R1_NOERR)
				{// CMD9ָ��ִ������
					if(SdCardRcvDataBlock(csd, 16)==SUCCESS)
					{
						res = SUCCESS;
					}
				}
				break;

			case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
				SdCardSendCmd(SDCARD_CMD10, 0, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD10ָ��ִ������
					if(SdCardRcvDataBlock(csd, 16)== SUCCESS)
					{
						res = SUCCESS;
					}
				}
				break;
			case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
				SdCardSendCmd(SDCARD_CMD58, 0, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD58ָ��ִ������
					for (n = 4; n; n--) 
					{
						*ptr++ = Spi1SendByte(0xFF);
					}
					res = SUCCESS;
				}
				break;
			case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
				SdCardSendCmd(SDCARD_ACMD13, 0, tmp, 1);       // ����д�������ָ��,��������ת��Ϊ�ֽڵ�ַ��
				if(tmp[0]==SDCARD_R1_NOERR)
				{// ACMD13ָ��ִ������
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

        SPI1_CS_H();                               // CS��������   
        Spi1SendByte(0xFF);
	}

	return res;
}

/**
  *************************************************************************************
  * @brief  SD���ϵ�
  * @param  ��
  * @retval �ϵ���
  *         @arg @b ERROR   ʧ��
  *         @arg @b SUCCESS �ɹ� 
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SdCardPowerOn(void)
{
    uint16_t baud;                                  // ����SPIʱ��Ƶ��ʹ�ã��м䲻�ɸ���
    uint8_t tmp[7];
    uint32_t retry;
    uint8_t cdtype = 0x00;
    ErrorStatus rtn = ERROR;

    baud = Spi1GetBaud();
    Spi1SetBaud(SPI_BaudRatePrescaler_256);        // �������ģʽ
    
    SPI1_CS_H();                               // CS��������

    for(retry=0; retry<15; retry++)                 // MMC/SD��DI�������ߣ�������>74ʱ������
    {
        Spi1SendByte(0xFF);
    }

    SdCardDelay1sCount = Spi1GetFrequency() >> 3;  // ��ʱ1s��Ҫ���͵��ֽ���    
    
    /* ѡ��SD�� */
    SPI1_CS_L();                                // CS����

    SdCardSendCmd(SDCARD_CMD0, 0x00, tmp, 1);       // ����ָ��0,������R1,��λ����IDLEģʽ

    if(tmp[0]==0x01)
    {// ָ��0ִ�гɹ�
        SdCardSendCmd(SDCARD_CMD8, 0x1AA, tmp, 5);  // ����ָ��8,����豸��ѹ
        if(tmp[0]==0x01)                       // SDHC
        {
            if((tmp[3]==0x01)&&(tmp[4]==0xAA)) // �豸���Թ�����2.7-3.3V
            {//ocr = 0x1AA
                /* ����ACMD41 */
                retry = SdCardDelay1sCount;
                do
                {// ACMD41�����ָ��
                    SdCardSendCmd(SDCARD_ACMD41, 1UL<<30, tmp, 1);
                } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1���سɹ�
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
            {// ACMD41�����ָ��
                SdCardSendCmd(SDCARD_ACMD41, 0x00, tmp, 1);
            } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1���سɹ�
            if(!retry)
            {// ACMD41ָ��ִ��ʧ�ܣ�����ʹ��CMD1
                cdtype = SDCARD_TYPE_MMC;
                retry = retry;
                do
                {
                    SdCardSendCmd(SDCARD_CMD1, 0x00, tmp, 1);
                } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1���سɹ�
            }
            if(retry)
            {
                SdCardSendCmd(SDCARD_CMD16, 512, tmp, 1);           // ����������СΪ512�ֽ�
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

    SPI1_CS_H();                               // CS��������

    Spi1SendByte(0xFF);

    Spi1SetBaud(baud);                              // �ָ������ٶ�

    SdCardDelay1sCount = Spi1GetFrequency() >> 3;  // ��ʱ1s��Ҫ���͵��ֽ���

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
