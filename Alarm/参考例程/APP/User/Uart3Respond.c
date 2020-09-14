#include "includes.h"

#define COMPANY_CODE    0x0010//这个是张同来他们分配的，不要改

STR_COMM  Uart3Data;
STR_COMM  Uart3TempBuffer;

/**
  *************************************************************************************
  * @brief  设置单个寄存器得值
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   功能码0x06
  *************************************************************************************  
  */
void SetSingleRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 Value = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];

	switch(RegAdd)
	{
		case 0x0010://检测指令,转换板发送：01 06 00 10 00 01 49 CF
			if(Value == 0x0001)
			{			
				DetTest();//检测雷管

                WriteLog("检测结果:", 0, LOG_DET_INFO);

				buffer_out->bytenum = 4;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);
				buffer_out->uch_buf[buffer_out->bytenum++] = ((u16)XA3_DetInfo.state)>>8;//检测结果
				buffer_out->uch_buf[buffer_out->bytenum++] = ((u16)XA3_DetInfo.state)&0xFF;							
			}
			break;
		default:
			break;			
	}
}

/**
  *************************************************************************************
  * @brief  读多个寄存器得值
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   功能码0x03
  *************************************************************************************  
  */
void ReadMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	
	switch(RegStartAdd)
	{
        case 0x0011://查询芯片状态指令
            if(RegCount == 0x000D)
            {               
                u8 Outside[13];
                u8 UID[13];
                u32 Pwd32 = 0;
                u16 Status = 0;

                DetInfoReadTest();//读芯片内码
                Status = XA3_DetInfo.state;
                
                WriteLog("查询结果:", 0, LOG_DET_INFO);
                
				buffer_out->bytenum = 2;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);
                
				buffer_out->uch_buf[buffer_out->bytenum++] = RegCount*2;

				buffer_out->uch_buf[buffer_out->bytenum++] = COMPANY_CODE>>8;
				buffer_out->uch_buf[buffer_out->bytenum++] = COMPANY_CODE&0xFF;
                
                memset(Outside, 0, sizeof(Outside));
                memset(UID, 0, sizeof(UID));
                

                if(Status == DET_PASS)
                {
                    u8 Inside[8];
                    
                    memcpy(&Inside[0], XA3_DetInfo.Serl.U8Buffer, 4);
                    memcpy(&Inside[4], XA3_DetInfo.Pwd.U8Buffer, 4);
                                                            
                    //把内码转成外码
                    if(XA3_DetInside2Outside(XA3_DetInfo.Serl.U32Value, XA3_DetInfo.Pwd.U32Value, Outside) == ERROR)
                    {//还没有注码
                        Status = (u16)DET_NEW_NO_CODE;
                        memset(Outside, 0, sizeof(Outside));
                    }
                    else if(XA3_Outside2UID(Outside, UID) == ERROR)//管码转换成UID码
                    {//
                        Status = (u16)DET_NEW_NO_CODE;
                        memset(UID, 0, sizeof(UID));
                    }
                    else
                    {
                        Pwd32 = CRC32Default(Inside, 8);
                    }
                }
                
				buffer_out->uch_buf[buffer_out->bytenum++] = (Status>>8)&0xFF;
				buffer_out->uch_buf[buffer_out->bytenum++] = Status&0xFF;
                
                //密码,8个 ASCII码
                sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], "%08X", Pwd32);
                buffer_out->bytenum += 8;   
                
                //UID
                buffer_out->uch_buf[buffer_out->bytenum++] = 0;
                memcpy(&buffer_out->uch_buf[buffer_out->bytenum], UID, 13);
                buffer_out->bytenum += 13;                
            }
            break;
	}
}

/**
  *************************************************************************************
  * @brief  设置多个寄存器得值
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   功能码0x10
  *************************************************************************************  
  */
void SetMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	u16 Bytes = buffer_in->uch_buf[6];	
	
	switch(RegStartAdd)
	{
		case 0x0012://注码
			if((RegCount == 0x000C) && (Bytes == 0x18))
			{
                u32 Pwd32 = 0;
                U32_UNION Serl, Pwd;
                u8 Outside[13];
                
                //先把UID码转换成管码
                if(XA3_UID2Outside(&buffer_in->uch_buf[18], Outside) == ERROR)
                {
                    XA3_DetInfo.state = UID_ERR;
                }
                else
                {
                    //从管壳码中提出数据，转成内码
                    if(ERROR == XA3_DetOutside2Inside(TestParam.DetType, Outside, &Serl.U32Value, &Pwd.U32Value))
                    {
                        XA3_DetInfo.state = DET_CODE_CONVER_ERR;
                    }
                    else
                    {
                        memcpy(DetCodeWrite, Serl.U8Buffer, 4);
                        memcpy(&DetCodeWrite[4], Pwd.U8Buffer, 4);
                        
                        DetWriteCodeTest();//写入内码
                        Pwd32 = CRC32Default(DetCodeWrite, 8);//这是返回给张同来他们的密码                   					
                    }                
                }
                WriteLog("注码结果:", 0, LOG_DET_INFO);
                
				buffer_out->bytenum = buffer_in->bytenum - 2;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);//数据原样返回
                
				buffer_out->uch_buf[7] = ((u16)XA3_DetInfo.state)>>8;//注码结果
				buffer_out->uch_buf[8] = ((u16)XA3_DetInfo.state)&0xFF;
                sprintf((char*)&buffer_out->uch_buf[9], "%08X",Pwd32);
			}
			break;
			
	}
}
/**
  *************************************************************************************
  * @brief  解包
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   在每包数据的前/后有冗余数据时也要能解析成功
  *************************************************************************************  
  */
ErrorStatus Uart3UnPack(STR_COMM *buffer_in)
{
    u16 i,j,count;
    u8 CRC16[2];
    
	if(buffer_in->bytenum < 4)
	{
		return ERROR;
	}
    
    for(i=0;i<buffer_in->bytenum;i++)
    {
        if((buffer_in->uch_buf[i] == 0x01) || (buffer_in->uch_buf[i] == 0x02))
        {
            for(j=buffer_in->bytenum-1; j > (i+2); j--)
            {
                count = j-i+1;
                GetCrc16Value(0xFFFF, CRC16, &buffer_in->uch_buf[i], count-2);
                if((CRC16[1] == buffer_in->uch_buf[j-1]) && (CRC16[0] == buffer_in->uch_buf[j]))
                {
                    if(count < 4)
                    {
                        return ERROR;
                    }
                    buffer_in->bytenum = count;
                    memcpy(&buffer_in->uch_buf[0], &buffer_in->uch_buf[i], buffer_in->bytenum);
                    return SUCCESS;
                }                
            }
        }
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  串口3响应函数
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   
  *************************************************************************************  
  */
void Uart3Respond(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u8 Cmd;
	u8 Buffer[20];
	
	buffer_out->bytenum = 0;
	
	if(Uart3UnPack(buffer_in) == ERROR)
    {
        return ;
    }	
	Cmd = buffer_in->uch_buf[1];
	
	switch(Cmd)
	{
		case 0x06://设置单个寄存器, 标准MODBUS协议格式
			SetSingleRegValue(buffer_in,  buffer_out);
			break;
		case 0x03://读寄存器, 标准MODBUS协议格式
			ReadMultRegValue(buffer_in,  buffer_out);			
			break;			
		case 0x10://设置多个寄存器, 非标准MODBUS协议格式
			SetMultRegValue(buffer_in,  buffer_out);			
			break;
		default:
			break;
	}
	if(buffer_out->bytenum != 0)
	{
		GetCrc16Value(0xFFFF, Buffer, &buffer_out->uch_buf[0], buffer_out->bytenum);
		buffer_out->uch_buf[buffer_out->bytenum++] = Buffer[1];
		buffer_out->uch_buf[buffer_out->bytenum++] = Buffer[0];
	}
}

