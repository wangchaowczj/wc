#include "includes.h"

DEVICE_MSC_STAGE DeviceMscStage;
u8 CBW[31];
u8 CSW[13];
u8 * DeviceMscEp2SendData;
u32 DeviceMscEp2DataLength;

static u8 MscBuffer[512];

uint32 ByteAddr;  //�ֽڵ�ַ

//�豸������
const unsigned char  MSC_DeviceDescriptor[24]={
	0x12,			//��������С
	0x01,			//����DEVICE
	0x10,				//USB�淶�汾��Ϣ
	0x01,
	0x00,			//����룬
	0x00,				//�������
	0x00,			//Э����
	0x08,				//�˵�0�������Ϣ����С
	0x48,				//����ID
	0x44,
	0x37,			//��ƷID
	0x55,
	0x00,			//�豸�汾��Ϣ
	0x01,
	0x00,			//����ֵ
	0x00,
	0x00,
	0x01,			//�������õ���Ŀ
	0x00,				//������
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};
//����������
const unsigned char  MSC_ConfigDescriptor[32]={			//����������
	0x09,					//��������С
	0x02,					//����CONFIG
	0x20,					//�����ô����������ݴ�С
	0x00,					//
	0x01,					//�ӿ���
	0x01,					//����ֵ
	0x00,					//����
	0x80,					//��Դ����
	0x32,					//��Ҫ���ߵ�Դ
	0x09,					//��������С
	0x04,					//����INTERFACE
	0x00,					//ʶ����
	0x00,					//������ֵ
	0x02,					//֧�ֵĶ˵���
	0x08,					//�����
	0x06,					//�������
	0x50,					//Э����
	0x00,					//����
	0x07,					//������С
	0x05,					//����ENDPOINT
	0x82,					//�˵���Ŀ������
	0x02,					//֧�ֵĴ�������
	0x40,					//֧�ֵ������Ϣ����С
	0x00,
	0x00,					//
	0x07,
	0x05,
	0x02,
	0x02,
	0x40,
	0x00,
	0x00					
};		//����������

//INQUIRY������Ҫ���ص�����
//���������INQUIRY������Ӧ���ݸ�ʽ
u8 DiskInf[36]=/*����36�ֽ�*/
{
 0x00, //�����豸
 0x00, //�������λD7ΪRMB��RMB=0����ʾ�����Ƴ��豸�����RMB=1����Ϊ���Ƴ��豸��
 0x00, //���ְ汾��0
 0x01, //������Ӧ��ʽ
 0x1F, //�������ݳ��ȣ�Ϊ31�ֽ�
 0x00, //����
 0x00, //����
 0x00, //����
 0xB7,0xA2,0XBB,0xF0,0xD7,0xB0,0xD6,0xC3, //���̱�ʶ��Ϊ�ַ���������װ�á�
 
 //��Ʒ��ʶ��Ϊ�ַ���������װ�á�
 0xB7,0xA2,0XBB,0xF0,0xD7,0xB0,0xD6,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x31,0x2E,0x30,0x31 //��Ʒ�汾�ţ�Ϊ1.01
};

//READ_FORMAT_CAPACITIES������Ҫ���ص�����
//���������READ_FORMAT_CAPACITIES������Ӧ���ݸ�ʽ
u8 MaximumCapacity[12]=
{
 0x00, 0x00, 0x00, //����
 0x08,  //�����б���
 0x10, 0x00, 0x00, 0x00,  //����(���֧��128GB)
 0x03, //����������Ϊ3����ʾ���֧�ֵĸ�ʽ������
 0x00, 0x02, 0x00 //ÿ���СΪ512�ֽ�
};

//READ_CAPACITY������Ҫ���ص�����
u8 DiskCapacity[8]=
{
 0x00,0x03,0xFF,0xFF, //�ܹ����ʵ�����߼����ַ
 0x00,0x00,0x02,0x00  //��ĳ���
 //���Ըô��̵�����Ϊ
 //(0x3FFFF+1)*0x200 = 0x8000000 = 128*1024*1024 = 128MB.
};

//REQUEST SENSE������Ҫ���ص����ݣ�����̶�Ϊ��Ч����
//��ο��������ݽṹ�Ľ���
u8 SenseData[18]=
{
 0x70, //������룬�̶�Ϊ0x70
 0x00, //����
 0x05, //Sense KeyΪ0x05����ʾ��Ч����ILLEGAL REQUEST��
 0x00, 0x00, 0x00, 0x00, //InformationΪ0
 0x0A, //�������ݳ���Ϊ10�ֽ�
 0x00, 0x00, 0x00, 0x00, //����
 0x20, //Additional Sense Code(ASC)Ϊ0x20����ʾ��Ч��������루INVALID COMMAND OPERATION CODE��
 0x00, //Additional Sense Code Qualifier(ASCQ)Ϊ0
 0x00, 0x00, 0x00, 0x00 //����
};


/********************************************************************
�������ܣ���CBW�л�ȡ�������ݵ��ֽ�����
��ڲ������ޡ�
��    �أ���Ҫ������ֽ�����
��    ע���ޡ�
********************************************************************/
uint32 GetDataTransferLength(void)
{
 uint32 Len;
 
 //CBW[8]~CBW[11]Ϊ���䳤�ȣ�С�˽ṹ��
 
 Len=CBW[11];
 Len=Len*256+CBW[10];
 Len=Len*256+CBW[9];
 Len=Len*256+CBW[8];
 
 return Len;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ���CBW�л�ȡ�߼����ַLBA���ֽ�����
��ڲ������ޡ�
��    �أ��߼����ַLBA��
��    ע���ޡ�
********************************************************************/
uint32 GetLba(void)
{
	uint32 Lba;

	//����д����ʱ��CBW[17]~CBW[20]Ϊ�߼����ַ����˽ṹ��

	Lba=CBW[17];
	Lba=Lba*256+CBW[18];
	Lba=Lba*256+CBW[19];
	Lba=Lba*256+CBW[20];

	return Lba;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����CSW��
��ڲ�����Residue��ʣ���ֽ�����Status������ִ�е�״̬��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SetCsw(uint32 Residue, uint8 Status)
{
 //����CSW��ǩ������ʵ���Բ���ÿ�ζ����õģ�
 //��ʼ��ʼ������һ�ξ����ˣ�����ÿ�ζ�����
 CSW[0]='U';
 CSW[1]='S';
 CSW[2]='B';
 CSW[3]='S';
 
 //����dCBWTag��CSW��dCSWTag��ȥ
 CSW[4]=CBW[4];
 CSW[5]=CBW[5];
 CSW[6]=CBW[6];
 CSW[7]=CBW[7];
    
 //ʣ���ֽ���
 CSW[8]=Residue&0xFF;
 CSW[9]=(Residue>>8)&0xFF;
 CSW[10]=(Residue>>16)&0xFF;
 CSW[11]=(Residue>>24)&0xFF;
 
 //����ִ�е�״̬��0��ʾ�ɹ���1��ʾʧ�ܡ�
 CSW[12]=Status;
}
/********************************************************************
�������ܣ���ȡ�������ݺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void GetDiskData(void)
{
	if((ByteAddr%512) == 0)
	{
		if(ERROR == MscReadSector(ByteAddr/512,&MscBuffer[0]))
		{
			DeviceMscEp2DataLength = 1;
			SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //����CSWΪʧ��
		}
		DeviceMscEp2SendData = MscBuffer;	
	}

	ByteAddr+=64; //�����ֽڵ�ַ��ÿ�η����������ȵ�����
}

/********************************************************************
�������ܣ�������ͨ���˵�2���͡�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע�����������ݳ���Ϊ0�����Ҵ������ݽ׶�ʱ�����Զ�����CSW��
********************************************************************/
void DeviceMscSendData(void)
{
	if(DeviceMscEp2DataLength==0) //�����Ҫ���͵����ݳ���Ϊ0
	{
		if(DeviceMscStage == DATA_STAGE) //���Ҵ������ݽ׶�
		{
			//��ֱ�ӽ���״̬�׶�
			DeviceMscStage = STATUS_STAGE;
			DeviceMscEp2DataLength=sizeof(CSW); //���ݳ���ΪCSW�Ĵ�С
			DeviceMscEp2SendData = CSW; //���ص�����ΪCSW
		}
		else
		{
			return; //�����״̬�׶ε����ݷ�����ϣ��򷵻�
		}
	}

	//���Ҫ���͵ĳ��ȱȶ˵�2������Ҫ�࣬��ֶ��������
	if(DeviceMscEp2DataLength > 64)
	{
		//���Ͷ˵�2��󳤶��ֽ�
		//D12WriteEndpointBuffer(5,64,pEp2SendData);
		Write374Block( RAM_ENDP2_TRAN, 64, DeviceMscEp2SendData ); 
		Write374Byte( REG_USB_LENGTH, 64);
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_ACK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_RECV_TOG );		
		
		//ָ���ƶ�EP2_SIZE�ֽ�
		DeviceMscEp2SendData += 64;
		DeviceMscEp2DataLength -= 64;
		//�����READ(10)������������ݽ׶Σ�����Ҫ��ȡ��������
		if((CBW[15]==READ_10)&&(DeviceMscStage==DATA_STAGE))
		{
			GetDiskData(); //��ȡ��������
		}
	}
	else
	{
		//����ȫ��������
		//D12WriteEndpointBuffer(5,(uint8)Ep2DataLength,pEp2SendData);
		Write374Block( RAM_ENDP2_TRAN, DeviceMscEp2DataLength, DeviceMscEp2SendData );  // ��ʾ�ش�
		Write374Byte( REG_USB_LENGTH, DeviceMscEp2DataLength);
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_ACK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_RECV_TOG );			
		
		DeviceMscEp2DataLength=0;  //���䳤��Ϊ0  
		//��������ݷ�����ϣ���������������Э���״̬�׶�
		if(DeviceMscStage == DATA_STAGE)
		{
			DeviceMscStage = STATUS_STAGE;
			DeviceMscEp2DataLength = sizeof(CSW); //���ݳ���ΪCSW�Ĵ�С
			DeviceMscEp2SendData = CSW; //���ص�����ΪCSW
		}
		else if(DeviceMscStage == STATUS_STAGE) //�����״̬�׶���ϣ�����뵽����׶�
		{
			DeviceMscStage = COMMAND_STAGE;  //���뵽����׶�
		}
	}
}

/********************************************************************
�������ܣ�����������ݡ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void ProcScsiOutData(void)
{
	u8 length;
	//u8 Ep2Buffer[64];
	
	//���˵�2����
	length = Read374Byte(REG_USB_LENGTH);
	Read374Block( RAM_ENDP2_RECV, length,&MscBuffer[ByteAddr%512]);	 

	ByteAddr += 64;
	if((ByteAddr%512) == 0)
	{
		if(ERROR == MscWriteSector((ByteAddr/512)-1, MscBuffer))
		{
			SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //����CSWΪʧ��
		}
	}
	
	DeviceMscEp2DataLength -= length;
	
	//���ݴ�����ϣ����뵽״̬�׶�
	if(DeviceMscEp2DataLength==0)
	{
		//��ʱEp2DataLengthΪ0�����Ҵ������ݽ׶Σ����÷������ݺ���������CSW
		DeviceMscSendData();		
	}
}



/********************************************************************
�������ܣ�����SCSI����ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע����Ȼ��SCSI�������ʵ��ʹ�õ���UFI���
********************************************************************/
void ProcScsiCommand(void)
{
//	uint64_t  cap = 0;
	
	DeviceMscStage=DATA_STAGE; //���뵽���ݽ׶�

	//CBW��ƫ����Ϊ15���ֶ�Ϊ���������
	switch(CBW[15])
	{
		case INQUIRY:  //INQUIRY����
			DeviceMscEp2SendData = DiskInf; //���ش�����Ϣ
			DeviceMscEp2DataLength = GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
			SetCsw(DeviceMscEp2DataLength-sizeof(DiskInf),0); //����ʣ���ֽ����Լ�״̬�ɹ�
			if(DeviceMscEp2DataLength > sizeof(DiskInf)) //�����������ݱ�ʵ�ʵ�Ҫ��
			{
				DeviceMscEp2DataLength=sizeof(DiskInf); //��ֻ����ʵ�ʵĳ���
			}
			DeviceMscSendData(); //��������
			break;

		case READ_FORMAT_CAPACITIES: //����ʽ������
			DeviceMscEp2SendData=MaximumCapacity; //��������ʽ��������Ϣ
			DeviceMscEp2DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
			SetCsw(DeviceMscEp2DataLength-sizeof(MaximumCapacity),0); //����ʣ���ֽ����Լ�״̬�ɹ�
			if(DeviceMscEp2DataLength>sizeof(MaximumCapacity)) //�����������ݱ�ʵ�ʵ�Ҫ��
			{
				DeviceMscEp2DataLength=sizeof(MaximumCapacity); //��ֻ����ʵ�ʵĳ���
			}
			DeviceMscSendData(); //��������
			break;

//		case READ_CAPACITY: //����������
//			cap = SdCardGetCapacity();
//			if(cap != 0)
//			{
//				cap /= 512;
//				DiskCapacity[0] = cap>>24;
//				DiskCapacity[1] = cap>>16;
//				DiskCapacity[2] = cap>>8;
//				DiskCapacity[3] = cap;

//				DeviceMscEp2SendData=DiskCapacity; //���ش�������
//				DeviceMscEp2DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
//				SetCsw(DeviceMscEp2DataLength-sizeof(DiskCapacity),0); //����ʣ���ֽ����Լ�״̬�ɹ�
//				if(DeviceMscEp2DataLength > sizeof(DiskCapacity)) //�����������ݱ�ʵ�ʵ�Ҫ��
//				{
//					DeviceMscEp2DataLength=sizeof(DiskCapacity); //��ֻ����ʵ�ʵĳ���
//				}				
//			}
//			else
//			{
//				DeviceMscEp2DataLength = 1;
//				SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //����CSWΪʧ��
//			}

//			DeviceMscSendData(); //��������
//			break;

		case READ_10: //READ(10)����
			//DeviceMscEp2DataLength = GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
			DeviceMscEp2DataLength = (u32)((u32)CBW[22]*256UL+(u32)CBW[23])*512UL;
			ByteAddr=(u32)GetLba()*512UL; //��ȡ�ֽڵ�ַ���ֽڵ�ַΪ�߼����ַ����ÿ���С
			
			SetCsw(0,0); //����ʣ���ֽ���Ϊ0��״̬�ɹ�
			GetDiskData(); //��ȡ��Ҫ���ص�����
			DeviceMscSendData(); //��������
			break;

		case WRITE_10: //WRITE(10)����
			//DeviceMscEp2DataLength = GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
			DeviceMscEp2DataLength = (u32)((u32)CBW[22]*256UL+(u32)CBW[23])*512UL;
			ByteAddr = (u32)GetLba()*512UL; //��ȡ�ֽڵ�ַ
			SetCsw(0,0); //����ʣ���ֽ���Ϊ0��״̬�ɹ�
			break;

		case REQUEST_SENSE: //������ѯ��ǰһ������ִ��ʧ�ܵ�ԭ��
			DeviceMscEp2SendData=SenseData; //����̽������
			DeviceMscEp2DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
			SetCsw(DeviceMscEp2DataLength-sizeof(SenseData),0); //����ʣ���ֽ����Լ�״̬�ɹ�
			if(DeviceMscEp2DataLength>sizeof(SenseData)) //�����������ݱ�ʵ�ʵ�Ҫ��
			{
				DeviceMscEp2DataLength=sizeof(SenseData); //��ֻ����ʵ�ʵĳ���
			}
			DeviceMscSendData(); //��������
			break;

		case TEST_UNIT_READY: //���Դ����Ƿ�׼����
			DeviceMscEp2DataLength=0; //���ó���Ϊ0���������ݽ�����CSW
			SetCsw(0,0); //����CSWΪ�ɹ�
			DeviceMscSendData(); //����CSW
			break;

		default: //��������ϣ�����ִ��ʧ��
			if(CBW[12]&0x80) DeviceMscEp2DataLength=1; //���Ϊ������������㷵��1�ֽ�
			else DeviceMscEp2DataLength=0; //����Ϊ������������ó���Ϊ0��ֱ�ӷ���CSW
			SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //����CSWΪʧ��
			DeviceMscSendData(); //����CSW
			break;
	}
}

void DeviceMscOutCallBack(void)
{
	u8 length;
	
	if(DeviceMscStage == COMMAND_STAGE)
	{
		length = Read374Byte(REG_USB_LENGTH);
		if(length == 31) //������յ������ݳ�����31�ֽڣ���˵����CBW
		{
			//�����ݶ��ص�CBW��������
			Read374Block( RAM_ENDP2_RECV, sizeof(CBW),CBW);	
			//���CBW��ǩ���Ƿ���ȷ��ǩ��Ϊ��USBC��
			if((CBW[0]=='U')&&(CBW[1]=='S')&&(CBW[2]=='B')&&(CBW[3]=='C'))
			{
				//CBWǩ����ȷ������SCSI����
				ProcScsiCommand();
			}
		}
	}
	else if(DeviceMscStage == DATA_STAGE)
	{
		//Ch374mDelayuS(200);
		ProcScsiOutData(); //����SCSI�������
	}
	else 
	{
		;
	}
}

/**
  *************************************************************************************
  * @brief  �����̺���
  * @param  Sector ������
  * @param  Readbuff ������  
  * @retval ���
  * @author ZH
  * @date   2015��9��7��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus MscReadSector(u32 Sector, u8 *Readbuff)
{
//	if(SdCardReadBlock(Readbuff, Sector, 1) != ERROR)
//	{
//		return SUCCESS;
//	}
//	return SdCardReadBlock(Readbuff, Sector,1);
}

/**
  *************************************************************************************
  * @brief  д���̺���
  * @param  Sector ������
  * @param  Readbuff ������  
  * @retval ���
  * @author ZH
  * @date   2015��8��22��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus MscWriteSector(u32 Sector, u8 *Writebuff)
{
//	if(ERROR != SdCardWriteBlock(Writebuff, Sector, 1))
//	{
//		return SUCCESS;
//	}
//	return SdCardWriteBlock(Writebuff, Sector, 1);
}
