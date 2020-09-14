/**
  *************************************************************************************
  * @file    slip.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SLIP打包与解包模块头文件
  *************************************************************************************
  */
#ifndef __SLIP_H__
#define __SLIP_H__ 

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Slip
  *@brief SLIP打包与解包模块
  *@{
  */

/**@defgroup STR_COMM
  *@brief 数据结构
  *@{
  */
#define BUF_MAX    1024  /**<接收发送数据帧缓冲区大小 */

/**
 *@brief 数据状态
 */
typedef enum 
{
    RECIEVING = 0x10,
    RECIEVED  = 0x11,
    SENDING   = 0x20,
    SENDED    = 0x21,
    DECODING  = 0x30,
    DECODED   = 0x31,
    ENCODING  = 0x40,
    ENCODED   = 0x41,
    IDL       = 0x00,
}COMStatus;

/**
 *@brief 接收发送数据帧格式
 */
typedef struct
{
    u16 bytenum;
    COMStatus status;
    u8 uch_buf[BUF_MAX];
}STR_COMM; 
/**
 *@} STR_COMM
 */
/**
 *@} Slip
 */
/**
 *@} PowerBoard
 */

//-------------------------------------------------------------------
ErrorStatus Comm_RcvNByte(STR_COMM *rcv_temp, u8 *p_data, u16 uin_num);
u8 Get_Sum_Verify(u8 *buffer, u16 length);
ErrorStatus Slip_Unpack(STR_COMM *buf_in, STR_COMM *buf_frame);
void Slip_Pack(STR_COMM *buf_in, STR_COMM *buf_frame);
ErrorStatus SlipSimpleCheck(STR_COMM * buffer);


#endif


