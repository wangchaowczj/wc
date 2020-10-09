#include "includes.h"

DEVICE_MSC_STAGE DeviceMscStage;
u8 CBW[31];
u8 CSW[13];
u8 * DeviceMscEp2SendData;
u32 DeviceMscEp2DataLength;

static u8 MscBuffer[512];

uint32 ByteAddr;  //字节地址

//设备描述符
const unsigned char  MSC_DeviceDescriptor[24]={
	0x12,			//描述符大小
	0x01,			//常数DEVICE
	0x10,				//USB规范版本信息
	0x01,
	0x00,			//类别码，
	0x00,				//子类别码
	0x00,			//协议码
	0x08,				//端点0的最大信息包大小
	0x48,				//厂商ID
	0x44,
	0x37,			//产品ID
	0x55,
	0x00,			//设备版本信息
	0x01,
	0x00,			//索引值
	0x00,
	0x00,
	0x01,			//可能配置的数目
	0x00,				//无意义
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};
//配置描述符
const unsigned char  MSC_ConfigDescriptor[32]={			//配置描述符
	0x09,					//描述符大小
	0x02,					//常数CONFIG
	0x20,					//此配置传回所有数据大小
	0x00,					//
	0x01,					//接口数
	0x01,					//配置值
	0x00,					//索引
	0x80,					//电源设置
	0x32,					//需要总线电源
	0x09,					//描述符大小
	0x04,					//常数INTERFACE
	0x00,					//识别码
	0x00,					//代替数值
	0x02,					//支持的端点数
	0x08,					//类别码
	0x06,					//子类别码
	0x50,					//协议码
	0x00,					//索引
	0x07,					//述符大小
	0x05,					//常数ENDPOINT
	0x82,					//端点数目及方向
	0x02,					//支持的传输类型
	0x40,					//支持的最大信息包大小
	0x00,
	0x00,					//
	0x07,
	0x05,
	0x02,
	0x02,
	0x40,
	0x00,
	0x00					
};		//配置描述符

//INQUIRY命令需要返回的数据
//请对照书中INQUIRY命令响应数据格式
u8 DiskInf[36]=/*保持36字节*/
{
 0x00, //磁盘设备
 0x00, //其中最高位D7为RMB。RMB=0，表示不可移除设备。如果RMB=1，则为可移除设备。
 0x00, //各种版本号0
 0x01, //数据响应格式
 0x1F, //附加数据长度，为31字节
 0x00, //保留
 0x00, //保留
 0x00, //保留
 0xB7,0xA2,0XBB,0xF0,0xD7,0xB0,0xD6,0xC3, //厂商标识，为字符串“发火装置”
 
 //产品标识，为字符串“发火装置”
 0xB7,0xA2,0XBB,0xF0,0xD7,0xB0,0xD6,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x31,0x2E,0x30,0x31 //产品版本号，为1.01
};

//READ_FORMAT_CAPACITIES命令需要返回的数据
//请对照书中READ_FORMAT_CAPACITIES命令响应数据格式
u8 MaximumCapacity[12]=
{
 0x00, 0x00, 0x00, //保留
 0x08,  //容量列表长度
 0x10, 0x00, 0x00, 0x00,  //块数(最大支持128GB)
 0x03, //描述符代码为3，表示最大支持的格式化容量
 0x00, 0x02, 0x00 //每块大小为512字节
};

//READ_CAPACITY命令需要返回的数据
u8 DiskCapacity[8]=
{
 0x00,0x03,0xFF,0xFF, //能够访问的最大逻辑块地址
 0x00,0x00,0x02,0x00  //块的长度
 //所以该磁盘的容量为
 //(0x3FFFF+1)*0x200 = 0x8000000 = 128*1024*1024 = 128MB.
};

//REQUEST SENSE命令需要返回的数据，这里固定为无效命令
//请参看书总数据结构的解释
u8 SenseData[18]=
{
 0x70, //错误代码，固定为0x70
 0x00, //保留
 0x05, //Sense Key为0x05，表示无效请求（ILLEGAL REQUEST）
 0x00, 0x00, 0x00, 0x00, //Information为0
 0x0A, //附加数据长度为10字节
 0x00, 0x00, 0x00, 0x00, //保留
 0x20, //Additional Sense Code(ASC)为0x20，表示无效命令操作码（INVALID COMMAND OPERATION CODE）
 0x00, //Additional Sense Code Qualifier(ASCQ)为0
 0x00, 0x00, 0x00, 0x00 //保留
};


/********************************************************************
函数功能：从CBW中获取传输数据的字节数。
入口参数：无。
返    回：需要传输的字节数。
备    注：无。
********************************************************************/
uint32 GetDataTransferLength(void)
{
 uint32 Len;
 
 //CBW[8]~CBW[11]为传输长度（小端结构）
 
 Len=CBW[11];
 Len=Len*256+CBW[10];
 Len=Len*256+CBW[9];
 Len=Len*256+CBW[8];
 
 return Len;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：从CBW中获取逻辑块地址LBA的字节数。
入口参数：无。
返    回：逻辑块地址LBA。
备    注：无。
********************************************************************/
uint32 GetLba(void)
{
	uint32 Lba;

	//读和写命令时，CBW[17]~CBW[20]为逻辑块地址（大端结构）

	Lba=CBW[17];
	Lba=Lba*256+CBW[18];
	Lba=Lba*256+CBW[19];
	Lba=Lba*256+CBW[20];

	return Lba;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
函数功能：填充CSW。
入口参数：Residue：剩余字节数；Status：命令执行的状态。
返    回：无。
备    注：无。
********************************************************************/
void SetCsw(uint32 Residue, uint8 Status)
{
 //设置CSW的签名，其实可以不用每次都设置的，
 //开始初始化设置一次就行了，这里每次都设置
 CSW[0]='U';
 CSW[1]='S';
 CSW[2]='B';
 CSW[3]='S';
 
 //复制dCBWTag到CSW的dCSWTag中去
 CSW[4]=CBW[4];
 CSW[5]=CBW[5];
 CSW[6]=CBW[6];
 CSW[7]=CBW[7];
    
 //剩余字节数
 CSW[8]=Residue&0xFF;
 CSW[9]=(Residue>>8)&0xFF;
 CSW[10]=(Residue>>16)&0xFF;
 CSW[11]=(Residue>>24)&0xFF;
 
 //命令执行的状态，0表示成功，1表示失败。
 CSW[12]=Status;
}
/********************************************************************
函数功能：获取磁盘数据函数。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void GetDiskData(void)
{
	if((ByteAddr%512) == 0)
	{
		if(ERROR == MscReadSector(ByteAddr/512,&MscBuffer[0]))
		{
			DeviceMscEp2DataLength = 1;
			SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //设置CSW为失败
		}
		DeviceMscEp2SendData = MscBuffer;	
	}

	ByteAddr+=64; //调整字节地址，每次发送最大包长度的数据
}

/********************************************************************
函数功能：将数据通过端点2发送。
入口参数：无。
返    回：无。
备    注：当发送数据长度为0，并且处于数据阶段时，将自动发送CSW。
********************************************************************/
void DeviceMscSendData(void)
{
	if(DeviceMscEp2DataLength==0) //如果需要发送的数据长度为0
	{
		if(DeviceMscStage == DATA_STAGE) //并且处于数据阶段
		{
			//则直接进入状态阶段
			DeviceMscStage = STATUS_STAGE;
			DeviceMscEp2DataLength=sizeof(CSW); //数据长度为CSW的大小
			DeviceMscEp2SendData = CSW; //返回的数据为CSW
		}
		else
		{
			return; //如果是状态阶段的数据发送完毕，则返回
		}
	}

	//如果要发送的长度比端点2最大包长要多，则分多个包发送
	if(DeviceMscEp2DataLength > 64)
	{
		//发送端点2最大长度字节
		//D12WriteEndpointBuffer(5,64,pEp2SendData);
		Write374Block( RAM_ENDP2_TRAN, 64, DeviceMscEp2SendData ); 
		Write374Byte( REG_USB_LENGTH, 64);
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_ACK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_RECV_TOG );		
		
		//指针移动EP2_SIZE字节
		DeviceMscEp2SendData += 64;
		DeviceMscEp2DataLength -= 64;
		//如果是READ(10)命令，并且是数据阶段，则需要获取磁盘数据
		if((CBW[15]==READ_10)&&(DeviceMscStage==DATA_STAGE))
		{
			GetDiskData(); //获取磁盘数据
		}
	}
	else
	{
		//可以全部发送完
		//D12WriteEndpointBuffer(5,(uint8)Ep2DataLength,pEp2SendData);
		Write374Block( RAM_ENDP2_TRAN, DeviceMscEp2DataLength, DeviceMscEp2SendData );  // 演示回传
		Write374Byte( REG_USB_LENGTH, DeviceMscEp2DataLength);
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_ACK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_RECV_TOG );			
		
		DeviceMscEp2DataLength=0;  //传输长度为0  
		//如果是数据发送完毕，则进入仅批量传输协议的状态阶段
		if(DeviceMscStage == DATA_STAGE)
		{
			DeviceMscStage = STATUS_STAGE;
			DeviceMscEp2DataLength = sizeof(CSW); //数据长度为CSW的大小
			DeviceMscEp2SendData = CSW; //返回的数据为CSW
		}
		else if(DeviceMscStage == STATUS_STAGE) //如果是状态阶段完毕，则进入到命令阶段
		{
			DeviceMscStage = COMMAND_STAGE;  //进入到命令阶段
		}
	}
}

/********************************************************************
函数功能：处理输出数据。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void ProcScsiOutData(void)
{
	u8 length;
	//u8 Ep2Buffer[64];
	
	//读端点2数据
	length = Read374Byte(REG_USB_LENGTH);
	Read374Block( RAM_ENDP2_RECV, length,&MscBuffer[ByteAddr%512]);	 

	ByteAddr += 64;
	if((ByteAddr%512) == 0)
	{
		if(ERROR == MscWriteSector((ByteAddr/512)-1, MscBuffer))
		{
			SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //设置CSW为失败
		}
	}
	
	DeviceMscEp2DataLength -= length;
	
	//数据传输完毕，进入到状态阶段
	if(DeviceMscEp2DataLength==0)
	{
		//此时Ep2DataLength为0，并且处于数据阶段，调用发送数据函数将返回CSW
		DeviceMscSendData();		
	}
}



/********************************************************************
函数功能：处理SCSI命令的函数。
入口参数：无。
返    回：无。
备    注：虽然叫SCSI命令，但是实际使用的是UFI命令。
********************************************************************/
void ProcScsiCommand(void)
{
//	uint64_t  cap = 0;
	
	DeviceMscStage=DATA_STAGE; //进入到数据阶段

	//CBW中偏移量为15的字段为命令的类型
	switch(CBW[15])
	{
		case INQUIRY:  //INQUIRY命令
			DeviceMscEp2SendData = DiskInf; //返回磁盘信息
			DeviceMscEp2DataLength = GetDataTransferLength(); //获取需要返回的长度
			SetCsw(DeviceMscEp2DataLength-sizeof(DiskInf),0); //设置剩余字节数以及状态成功
			if(DeviceMscEp2DataLength > sizeof(DiskInf)) //如果请求的数据比实际的要长
			{
				DeviceMscEp2DataLength=sizeof(DiskInf); //则只返回实际的长度
			}
			DeviceMscSendData(); //返回数据
			break;

		case READ_FORMAT_CAPACITIES: //读格式化容量
			DeviceMscEp2SendData=MaximumCapacity; //返回最大格式化容量信息
			DeviceMscEp2DataLength=GetDataTransferLength(); //获取需要返回的长度
			SetCsw(DeviceMscEp2DataLength-sizeof(MaximumCapacity),0); //设置剩余字节数以及状态成功
			if(DeviceMscEp2DataLength>sizeof(MaximumCapacity)) //如果请求的数据比实际的要长
			{
				DeviceMscEp2DataLength=sizeof(MaximumCapacity); //则只返回实际的长度
			}
			DeviceMscSendData(); //返回数据
			break;

//		case READ_CAPACITY: //读容量命令
//			cap = SdCardGetCapacity();
//			if(cap != 0)
//			{
//				cap /= 512;
//				DiskCapacity[0] = cap>>24;
//				DiskCapacity[1] = cap>>16;
//				DiskCapacity[2] = cap>>8;
//				DiskCapacity[3] = cap;

//				DeviceMscEp2SendData=DiskCapacity; //返回磁盘容量
//				DeviceMscEp2DataLength=GetDataTransferLength(); //获取需要返回的长度
//				SetCsw(DeviceMscEp2DataLength-sizeof(DiskCapacity),0); //设置剩余字节数以及状态成功
//				if(DeviceMscEp2DataLength > sizeof(DiskCapacity)) //如果请求的数据比实际的要长
//				{
//					DeviceMscEp2DataLength=sizeof(DiskCapacity); //则只返回实际的长度
//				}				
//			}
//			else
//			{
//				DeviceMscEp2DataLength = 1;
//				SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //设置CSW为失败
//			}

//			DeviceMscSendData(); //返回数据
//			break;

		case READ_10: //READ(10)命令
			//DeviceMscEp2DataLength = GetDataTransferLength(); //获取需要返回的长度
			DeviceMscEp2DataLength = (u32)((u32)CBW[22]*256UL+(u32)CBW[23])*512UL;
			ByteAddr=(u32)GetLba()*512UL; //获取字节地址，字节地址为逻辑块地址乘以每块大小
			
			SetCsw(0,0); //设置剩余字节数为0，状态成功
			GetDiskData(); //获取需要返回的数据
			DeviceMscSendData(); //返回数据
			break;

		case WRITE_10: //WRITE(10)命令
			//DeviceMscEp2DataLength = GetDataTransferLength(); //获取需要返回的长度
			DeviceMscEp2DataLength = (u32)((u32)CBW[22]*256UL+(u32)CBW[23])*512UL;
			ByteAddr = (u32)GetLba()*512UL; //获取字节地址
			SetCsw(0,0); //设置剩余字节数为0，状态成功
			break;

		case REQUEST_SENSE: //该命令询问前一个命令执行失败的原因
			DeviceMscEp2SendData=SenseData; //返回探测数据
			DeviceMscEp2DataLength=GetDataTransferLength(); //获取需要返回的长度
			SetCsw(DeviceMscEp2DataLength-sizeof(SenseData),0); //设置剩余字节数以及状态成功
			if(DeviceMscEp2DataLength>sizeof(SenseData)) //如果请求的数据比实际的要长
			{
				DeviceMscEp2DataLength=sizeof(SenseData); //则只返回实际的长度
			}
			DeviceMscSendData(); //返回数据
			break;

		case TEST_UNIT_READY: //测试磁盘是否准备好
			DeviceMscEp2DataLength=0; //设置长度为0，发送数据将返回CSW
			SetCsw(0,0); //设置CSW为成功
			DeviceMscSendData(); //返回CSW
			break;

		default: //其它命令不认，返回执行失败
			if(CBW[12]&0x80) DeviceMscEp2DataLength=1; //如果为输入请求，则随便返回1字节
			else DeviceMscEp2DataLength=0; //否则为输出请求，则设置长度为0，直接返回CSW
			SetCsw(GetDataTransferLength()-DeviceMscEp2DataLength,1); //设置CSW为失败
			DeviceMscSendData(); //返回CSW
			break;
	}
}

void DeviceMscOutCallBack(void)
{
	u8 length;
	
	if(DeviceMscStage == COMMAND_STAGE)
	{
		length = Read374Byte(REG_USB_LENGTH);
		if(length == 31) //如果接收到的数据长度是31字节，则说明是CBW
		{
			//将数据读回到CBW缓冲区中
			Read374Block( RAM_ENDP2_RECV, sizeof(CBW),CBW);	
			//检查CBW的签名是否正确，签名为“USBC”
			if((CBW[0]=='U')&&(CBW[1]=='S')&&(CBW[2]=='B')&&(CBW[3]=='C'))
			{
				//CBW签名正确，处理SCSI命令
				ProcScsiCommand();
			}
		}
	}
	else if(DeviceMscStage == DATA_STAGE)
	{
		//Ch374mDelayuS(200);
		ProcScsiOutData(); //处理SCSI输出数据
	}
	else 
	{
		;
	}
}

/**
  *************************************************************************************
  * @brief  读磁盘函数
  * @param  Sector 扇区号
  * @param  Readbuff 缓冲区  
  * @retval 结果
  * @author ZH
  * @date   2015年9月7日
  * @note   无
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
  * @brief  写磁盘函数
  * @param  Sector 扇区号
  * @param  Readbuff 缓冲区  
  * @retval 结果
  * @author ZH
  * @date   2015年8月22日
  * @note   无
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
