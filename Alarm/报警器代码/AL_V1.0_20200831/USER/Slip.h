/**
  *************************************************************************************
  * @file    slip.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   SLIP�������ģ��ͷ�ļ�
  *************************************************************************************
  */
#ifndef __SLIP_H__
#define __SLIP_H__ 

/**@defgroup PowerBoard
  *@{
  */
/**@defgroup Slip
  *@brief SLIP�������ģ��
  *@{
  */

/**@defgroup STR_COMM
  *@brief ���ݽṹ
  *@{
  */
#define BUF_MAX    1024  /**<���շ�������֡��������С */

/**
 *@brief ����״̬
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
 *@brief ���շ�������֡��ʽ
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


