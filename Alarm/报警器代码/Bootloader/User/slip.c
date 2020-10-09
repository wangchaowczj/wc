/**
  *************************************************************************************
  * @file    slip.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SLIP打包与解包模块源文件
  *************************************************************************************
  */
#include "includes.h"

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Slip
  *@brief SLIP打包与解包模块
  *@{
  */
/**@defgroup Slip_Functions
  *@brief SLIP打包与解包模块
  *@{
  */
/**
  *************************************************************************************
  * @brief  简单检查数据缓冲区中的数据是否有SLIP的包头包尾
  * @param[out] rcv_temp: 简单解包后的数据结构指针(仅返回含有包头后的数据)
  * @param[in]  p_data:   接收到的数据指针  
  * @param[in]  uin_num:  接收到的数据长度
  * @retval 解包结果
  *         @arg @b ERROR 接收到的数据没有包头包尾或数据帧不大于4个字节
  *         @arg @b SUCCESS 接收到的数据有包头包尾，且数据帧最少要4个字节
  * @author ZH
  * @date   2014年9月10日
  * @note   无
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
            if(temp != 0xc0)                //不是包头
            {
                rcv_temp->bytenum = 0;
            }
        }
        else if(rcv_temp->uch_buf[rcv_temp->bytenum - 1] == 0xc0)
        {
            if(rcv_temp->bytenum == 2)        //包头
            {
                rcv_temp->bytenum = 1;
            }
            else                               //接收完一帧
            {
                if(rcv_temp->bytenum < 4)      //数据帧最少要4个字节
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
  * @brief  简单检查数据缓冲区中的数据是否有SLIP的包头包尾
  * @param[in,out] buffer: 要检查的数据结构指针并返回检查后的数据
  * @retval 解包结果
  *         @arg @b ERROR 未成功检测到包头包尾或数据帧不大于5个字节
  *         @arg @b SUCCESS 成功检测到包头包尾，且数据帧最少要5个字节
  * @author ZH
  * @date   2014年9月10日
  * @note   无
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
				if(buffer->bytenum >= 5)//至少1字节数据+2字节CRC16校验+2字节头尾
				{
					buffer->status = RECIEVED;//接收到包头包尾
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
		buffer->bytenum = 0;//如果已经接收满还没接收到包尾则重新开始，防止溢出
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  获取8位和校验值
  * @param  buffer: 要获取校验和的数据
  * @param  length: 要获取校验和的数据长度
  * @retval 校验和
  * @author ZH
  * @date   2014年9月10日
  * @note   无
  *************************************************************************************  
  */
u8 Get_Sum_Verify(u8 *buffer, u16 length)
{
    u8 temp = 0;
    u16 i = 0;

    for(i = 0; i < length; i++)           //计算加和校验
    {
        temp = (u8)(temp + *buffer++);
    }

	return temp;
}
/**
  *************************************************************************************
  * @brief  对完整SLIP数据包进行解包
  * @param[in]  buf_in:     待解包数据指针
  * @param[out] buf_frame: 解包后数据帧
  * @retval 解包结果
  *         @arg @b ERROR 解包成功
  *         @arg @b SUCCESS 解包失败
  * @author ZH
  * @date   2014年9月10日
  * @note   无
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
				begin_flag = 1;	//接收到包头
			}		
		}
		else
		{
			if(esc_flag == 1) //处于转义状态
			{
				switch (buf_in->uch_buf[i])
				{
					case 0xDD:  //转义ESC字符
						 buf_frame->uch_buf[buf_frame->bytenum++] = 0xDB;
						 break;
					case 0xDC:  //转义END字符
						 buf_frame->uch_buf[buf_frame->bytenum++] = 0xC0;
						 break;
					default:	  //重新开始接受
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
						 esc_flag = 1; //下一字节为转义字节
						 break;
					case 0xC0:
						 if(buf_frame->bytenum >= 2)
						 {
							buf_frame->status = RECIEVED;//接收到包头包尾 
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
  * @brief  SLIP打包程序
  * @param[in]  buf_in:     待打包数据指针
  * @param[out] buf_frame: 打包后数据帧
  * @retval 无
  * @author ZH
  * @date   2014年9月10日
  * @note   无
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

