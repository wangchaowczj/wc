#include "includes.h"

#define COMPANY_CODE    0x0010//�������ͬ�����Ƿ���ģ���Ҫ��

STR_COMM  Uart3Data;
STR_COMM  Uart3TempBuffer;

/**
  *************************************************************************************
  * @brief  ���õ����Ĵ�����ֵ
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ������0x06
  *************************************************************************************  
  */
void SetSingleRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 Value = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];

	switch(RegAdd)
	{
		case 0x0010://���ָ��,ת���巢�ͣ�01 06 00 10 00 01 49 CF
			if(Value == 0x0001)
			{			
				DetTest();//����׹�

                WriteLog("�����:", 0, LOG_DET_INFO);

				buffer_out->bytenum = 4;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);
				buffer_out->uch_buf[buffer_out->bytenum++] = ((u16)XA3_DetInfo.state)>>8;//�����
				buffer_out->uch_buf[buffer_out->bytenum++] = ((u16)XA3_DetInfo.state)&0xFF;							
			}
			break;
		default:
			break;			
	}
}

/**
  *************************************************************************************
  * @brief  ������Ĵ�����ֵ
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ������0x03
  *************************************************************************************  
  */
void ReadMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	
	switch(RegStartAdd)
	{
        case 0x0011://��ѯоƬ״ָ̬��
            if(RegCount == 0x000D)
            {               
                u8 Outside[13];
                u8 UID[13];
                u32 Pwd32 = 0;
                u16 Status = 0;

                DetInfoReadTest();//��оƬ����
                Status = XA3_DetInfo.state;
                
                WriteLog("��ѯ���:", 0, LOG_DET_INFO);
                
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
                                                            
                    //������ת������
                    if(XA3_DetInside2Outside(XA3_DetInfo.Serl.U32Value, XA3_DetInfo.Pwd.U32Value, Outside) == ERROR)
                    {//��û��ע��
                        Status = (u16)DET_NEW_NO_CODE;
                        memset(Outside, 0, sizeof(Outside));
                    }
                    else if(XA3_Outside2UID(Outside, UID) == ERROR)//����ת����UID��
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
                
                //����,8�� ASCII��
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
  * @brief  ���ö���Ĵ�����ֵ
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ������0x10
  *************************************************************************************  
  */
void SetMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	u16 Bytes = buffer_in->uch_buf[6];	
	
	switch(RegStartAdd)
	{
		case 0x0012://ע��
			if((RegCount == 0x000C) && (Bytes == 0x18))
			{
                u32 Pwd32 = 0;
                U32_UNION Serl, Pwd;
                u8 Outside[13];
                
                //�Ȱ�UID��ת���ɹ���
                if(XA3_UID2Outside(&buffer_in->uch_buf[18], Outside) == ERROR)
                {
                    XA3_DetInfo.state = UID_ERR;
                }
                else
                {
                    //�ӹܿ�����������ݣ�ת������
                    if(ERROR == XA3_DetOutside2Inside(TestParam.DetType, Outside, &Serl.U32Value, &Pwd.U32Value))
                    {
                        XA3_DetInfo.state = DET_CODE_CONVER_ERR;
                    }
                    else
                    {
                        memcpy(DetCodeWrite, Serl.U8Buffer, 4);
                        memcpy(&DetCodeWrite[4], Pwd.U8Buffer, 4);
                        
                        DetWriteCodeTest();//д������
                        Pwd32 = CRC32Default(DetCodeWrite, 8);//���Ƿ��ظ���ͬ�����ǵ�����                   					
                    }                
                }
                WriteLog("ע����:", 0, LOG_DET_INFO);
                
				buffer_out->bytenum = buffer_in->bytenum - 2;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);//����ԭ������
                
				buffer_out->uch_buf[7] = ((u16)XA3_DetInfo.state)>>8;//ע����
				buffer_out->uch_buf[8] = ((u16)XA3_DetInfo.state)&0xFF;
                sprintf((char*)&buffer_out->uch_buf[9], "%08X",Pwd32);
			}
			break;
			
	}
}
/**
  *************************************************************************************
  * @brief  ���
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ��ÿ�����ݵ�ǰ/������������ʱҲҪ�ܽ����ɹ�
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
  * @brief  ����3��Ӧ����
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
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
		case 0x06://���õ����Ĵ���, ��׼MODBUSЭ���ʽ
			SetSingleRegValue(buffer_in,  buffer_out);
			break;
		case 0x03://���Ĵ���, ��׼MODBUSЭ���ʽ
			ReadMultRegValue(buffer_in,  buffer_out);			
			break;			
		case 0x10://���ö���Ĵ���, �Ǳ�׼MODBUSЭ���ʽ
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

