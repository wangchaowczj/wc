#include "includes.h"

static aes_context AesCtx;

/**
  *************************************************************************************
  * @brief  �����Կ��ʼ��
  * @param ��
  * @retval ��
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
void PackageKeyInit(void)
{
	aes_set_key(&AesCtx, "0123456789abcdef", 128);
}

/**
  *************************************************************************************
  * @brief  ���ݽ������
  * @param[in]  data_in��  ���ǰ�����ݽṹ
  * @param[out] data_out�� ���������ݽṹ 
  * @retval ������
  *         @arg @b ERROR   ���ʧ��
  *         @arg @b SUCCESS ����ɹ�  
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus Unpack(STR_COMM* data_in, STR_COMM* data_out)
{
	u16 Count, Number;
	u8 crc[2];
	
	//slip���
	if(ERROR == Slip_Unpack(data_in, data_out))
	{
		return ERROR;
	}
	//CRCУ��
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
  * @brief  ���ݴ������
  * @param[in]  data_in��  ���ǰ�����ݽṹ
  * @param[out] data_out�� ���������ݽṹ 
  * @retval �� 
  * @author ZH
  * @date   2014��9��11��
  * @note   ��
  *************************************************************************************  
  */
void Pack(STR_COMM* data_in, STR_COMM* data_out)
{
	u16 AesLen, Count, Number;
	
	if((data_in->bytenum >= 0x05) && ((data_in->bytenum+2) < ((BUF_MAX-2)/2)))
	{
		//AES����
		AesLen = data_in->bytenum - 5;//����AES���ܵ���Ч���ݳ��ȣ�ǰ��5���ֽڲ�����
		memset(&data_in->uch_buf[data_in->bytenum], 0, BUF_MAX - data_in->bytenum);//ÿ�μ���16�ֽڣ�����16�ֽڵ���0���

		data_in->bytenum = 5 + (AesLen + 2 + 16)/16*16;//��չΪ16�ı���
		
		data_in->uch_buf[data_in->bytenum-2] = (u8)(AesLen>>8);//������ʵ���ݳ���
		data_in->uch_buf[data_in->bytenum-1] = (u8)(AesLen&0xFF);//������ʵ���ݳ���
		Number = (data_in->bytenum - 5)/16;
		Count = 0;
		while(Count < Number)
		{
			aes_encrypt(&AesCtx, &data_in->uch_buf[5+Count*16], &data_in->uch_buf[5+Count*16]);
			Count++;
		}
		
		//CRCУ��
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

