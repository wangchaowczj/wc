#include "includes.h"

static aes_context AesCtx;

/**
  *************************************************************************************
  * @brief  封包密钥初始化
  * @param 无
  * @retval 无
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
void PackageKeyInit(void)
{
	aes_set_key(&AesCtx, "0123456789abcdef", 128);
}

/**
  *************************************************************************************
  * @brief  数据解包函数
  * @param[in]  data_in：  解包前的数据结构
  * @param[out] data_out： 解包后的数据结构 
  * @retval 解包结果
  *         @arg @b ERROR   解包失败
  *         @arg @b SUCCESS 解包成功  
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus Unpack(STR_COMM* data_in, STR_COMM* data_out)
{
	u16 Count, Number;
	u8 crc[2];
	
	//slip解包
	if(ERROR == Slip_Unpack(data_in, data_out))
	{
		return ERROR;
	}
	//CRC校验
	GetCrc16Value(0xFFFF, crc, data_out->uch_buf, data_out->bytenum-2);
	if((crc[0] == data_out->uch_buf[data_out->bytenum-2]) && (crc[1]) == data_out->uch_buf[data_out->bytenum-1])
	{
		data_out->bytenum -= 2;
		if ((data_out->bytenum > 5) && (((data_out->bytenum-5)%16) == 0))
		{
			Number = (data_out->bytenum - 5)/16;
			Count = 0;
			while(Count < Number)
			{
				aes_decrypt(&AesCtx, &data_out->uch_buf[5+Count*16], &data_out->uch_buf[5+Count*16]);
				Count++;
			}
			data_out->bytenum = 5 + (data_out->uch_buf[data_out->bytenum-2]<<8) + data_out->uch_buf[data_out->bytenum-1];
			data_in->bytenum = 0;
			data_in->status = RECIEVING;
			return SUCCESS;		
		}
	}
	return ERROR;
}

/**
  *************************************************************************************
  * @brief  数据打包函数
  * @param[in]  data_in：  打包前的数据结构
  * @param[out] data_out： 打包后的数据结构 
  * @retval 无 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
void Pack(STR_COMM* data_in, STR_COMM* data_out)
{
	u16 AesLen, Count, Number;
	
	if((data_in->bytenum >= 0x05) && ((data_in->bytenum+2) < ((BUF_MAX-2)/2)))
	{
		//AES加密
		AesLen = data_in->bytenum - 5;//进行AES加密的有效数据长度，前边5个字节不加密
		memset(&data_in->uch_buf[data_in->bytenum], 0, BUF_MAX - data_in->bytenum);//每次加密16字节，不足16字节的以0填充

		data_in->bytenum = 5 + (AesLen + 2 + 16)/16*16;//扩展为16的倍数
		
		data_in->uch_buf[data_in->bytenum-2] = (u8)(AesLen>>8);//保存真实数据长度
		data_in->uch_buf[data_in->bytenum-1] = (u8)(AesLen&0xFF);//保存真实数据长度
		Number = (data_in->bytenum - 5)/16;
		Count = 0;
		while(Count < Number)
		{
			aes_encrypt(&AesCtx, &data_in->uch_buf[5+Count*16], &data_in->uch_buf[5+Count*16]);
			Count++;
		}
		
		//CRC校验
		GetCrc16Value(0xFFFF, &data_in->uch_buf[data_in->bytenum], data_in->uch_buf, data_in->bytenum);
		
		data_in->bytenum += 2;
		
		//slip
		Slip_Pack(data_in, data_out);	
	}
	else
	{
		data_out->bytenum = 0x00;
	}
}

