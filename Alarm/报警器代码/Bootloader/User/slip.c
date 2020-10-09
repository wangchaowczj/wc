/**
  *************************************************************************************
  * @file    slip.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   SLIP�������ģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Slip
  *@brief SLIP�������ģ��
  *@{
  */
/**@defgroup Slip_Functions
  *@brief SLIP�������ģ��
  *@{
  */
/**
  *************************************************************************************
  * @brief  �򵥼�����ݻ������е������Ƿ���SLIP�İ�ͷ��β
  * @param[out] rcv_temp: �򵥽��������ݽṹָ��(�����غ��а�ͷ�������)
  * @param[in]  p_data:   ���յ�������ָ��  
  * @param[in]  uin_num:  ���յ������ݳ���
  * @retval ������
  *         @arg @b ERROR ���յ�������û�а�ͷ��β������֡������4���ֽ�
  *         @arg @b SUCCESS ���յ��������а�ͷ��β��������֡����Ҫ4���ֽ�
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus Comm_RcvNByte(STR_COMM *rcv_temp, u8 *p_data, u16 uin_num)
{
    u8 temp = 0;   
    u16 i = 0;

    for(i = 0; i < uin_num; i++)
    {
        temp = *p_data++;
        rcv_temp->uch_buf[rcv_temp->bytenum++] = temp;

        if(rcv_temp->bytenum == 1)
        {
            if(temp != 0xc0)                //���ǰ�ͷ
            {
                rcv_temp->bytenum = 0;
            }
        }
        else if(rcv_temp->uch_buf[rcv_temp->bytenum - 1] == 0xc0)
        {
            if(rcv_temp->bytenum == 2)        //��ͷ
            {
                rcv_temp->bytenum = 1;
            }
            else                               //������һ֡
            {
                if(rcv_temp->bytenum < 4)      //����֡����Ҫ4���ֽ�
                {
                    rcv_temp->bytenum = 0;    
                }
                else
                {
                    return SUCCESS;
                }
            }
        }
        if(rcv_temp->bytenum >= BUF_MAX)
        {
            rcv_temp->bytenum = 0;
        }
    }
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  �򵥼�����ݻ������е������Ƿ���SLIP�İ�ͷ��β
  * @param[in,out] buffer: Ҫ�������ݽṹָ�벢���ؼ��������
  * @retval ������
  *         @arg @b ERROR δ�ɹ���⵽��ͷ��β������֡������5���ֽ�
  *         @arg @b SUCCESS �ɹ���⵽��ͷ��β��������֡����Ҫ5���ֽ�
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus SlipSimpleCheck(STR_COMM * buffer)
{
	u16 i,bytenum_temp;
	u8 start_flag= 0;
	
	bytenum_temp = buffer->bytenum;
	for(i=0; i < bytenum_temp; i++)
	{
		if(start_flag == 0)
		{
			if(buffer->uch_buf[i] == 0xC0)
			{
				start_flag = 1;
				buffer->bytenum = 0;
				buffer->uch_buf[buffer->bytenum++] = buffer->uch_buf[i];
			}
		}
		else
		{
			buffer->uch_buf[buffer->bytenum++] = buffer->uch_buf[i];
			if(buffer->uch_buf[i] == 0xC0)
			{
				if(buffer->bytenum >= 5)//����1�ֽ�����+2�ֽ�CRC16У��+2�ֽ�ͷβ
				{
					buffer->status = RECIEVED;//���յ���ͷ��β
					return SUCCESS;
				}
				else
				{
					buffer->bytenum = 0;
					buffer->uch_buf[buffer->bytenum++] = buffer->uch_buf[i];
				}					
			}
		}
	}
	if(buffer->bytenum == BUF_MAX)
	{
		buffer->bytenum = 0;//����Ѿ���������û���յ���β�����¿�ʼ����ֹ���
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ��ȡ8λ��У��ֵ
  * @param  buffer: Ҫ��ȡУ��͵�����
  * @param  length: Ҫ��ȡУ��͵����ݳ���
  * @retval У���
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
u8 Get_Sum_Verify(u8 *buffer, u16 length)
{
    u8 temp = 0;
    u16 i = 0;

    for(i = 0; i < length; i++)           //����Ӻ�У��
    {
        temp = (u8)(temp + *buffer++);
    }

	return temp;
}
/**
  *************************************************************************************
  * @brief  ������SLIP���ݰ����н��
  * @param[in]  buf_in:     ���������ָ��
  * @param[out] buf_frame: ���������֡
  * @retval ������
  *         @arg @b ERROR ����ɹ�
  *         @arg @b SUCCESS ���ʧ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus Slip_Unpack(STR_COMM *buf_in, STR_COMM *buf_frame)
{
	u8 begin_flag = 0,esc_flag = 0;
	u16 i = 0;

//    buf_frame->status = DECODING;
    buf_frame->bytenum = 0;	

	for(i = 0; i < buf_in->bytenum; i++)
	{
		if(begin_flag == 0)
		{
			if(buf_in->uch_buf[i] == 0xC0)
			{
				begin_flag = 1;	//���յ���ͷ
			}		
		}
		else
		{
			if(esc_flag == 1) //����ת��״̬
			{
				switch (buf_in->uch_buf[i])
				{
					case 0xDD:  //ת��ESC�ַ�
						 buf_frame->uch_buf[buf_frame->bytenum++] = 0xDB;
						 break;
					case 0xDC:  //ת��END�ַ�
						 buf_frame->uch_buf[buf_frame->bytenum++] = 0xC0;
						 break;
					default:	  //���¿�ʼ����
						 buf_frame->bytenum = 0;						
						 begin_flag = 0;      
						 break;
				}
				esc_flag = 0;			
			}
			else
			{
				switch (buf_in->uch_buf[i])
				{
					case 0xDB:
						 esc_flag = 1; //��һ�ֽ�Ϊת���ֽ�
						 break;
					case 0xC0:
						 if(buf_frame->bytenum >= 2)
						 {
							buf_frame->status = RECIEVED;//���յ���ͷ��β 
							return SUCCESS;	
						 }

					 	 buf_frame->bytenum = 0;
						 esc_flag = 0;
						 break;
					default:
						 buf_frame->uch_buf[buf_frame->bytenum++] = buf_in->uch_buf[i];
						 break;
				}			
			}
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  SLIP�������
  * @param[in]  buf_in:     ���������ָ��
  * @param[out] buf_frame: ���������֡
  * @retval ��
  * @author ZH
  * @date   2014��9��10��
  * @note   ��
  *************************************************************************************  
  */
void Slip_Pack(STR_COMM *buf_in, STR_COMM *buf_frame)
{
    u16 i = 0;

//    buf_frame->status = ENCODING;
    buf_frame->bytenum = 1;

	memset((u8*)&buf_frame->uch_buf[0], 0x00, BUF_MAX);

    buf_frame->uch_buf[0] = 0xc0;

    for(i = 0; i < buf_in->bytenum; i++)
    {
        if(buf_in->uch_buf[i] == 0xc0)
        {
            buf_frame->uch_buf[buf_frame->bytenum++] = 0xdb;
            buf_frame->uch_buf[buf_frame->bytenum++] = 0xdc; 
        }
        else if(buf_in->uch_buf[i] == 0xdb)
        {
            buf_frame->uch_buf[buf_frame->bytenum++] = 0xdb;
            buf_frame->uch_buf[buf_frame->bytenum++] = 0xdd; 
        }
        else
        {
            buf_frame->uch_buf[buf_frame->bytenum++] = buf_in->uch_buf[i];
        }
    }
    buf_frame->uch_buf[buf_frame->bytenum++] = 0xc0;

//    buf_frame->status = ENCODED;
}
/**
 *@} Slip_Functions
 */
/**
 *@} Slip
 */
/**
 *@} PowerBoard
 */

