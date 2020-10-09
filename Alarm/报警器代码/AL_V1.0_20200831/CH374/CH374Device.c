#include "includes.h"


typedef struct
{
	CH374_DEVICE_TYPE Type;
	ONE_DESCRIPTOR Device;
	ONE_DESCRIPTOR Config;
	ONE_DESCRIPTOR Langage;
	ONE_DESCRIPTOR Manu;
	ONE_DESCRIPTOR Produce;
}CH374_DEVICE;

CH374_DEVICE Ch374Device;
								
  
// 语言描述符
const u8 LangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// 厂家信息
const u8 ManuInfo[] = { 0x0E, 0x03, 'X', 0, 'i', 0, 'n', 0, '-', 0, 'A', 0 , 'n', 0 };
// 产品信息
const u8 ProdInfo[] = { 0x0E, 0x03, 'X', 0, 'i', 0, 'n', 0, '-', 0, 'A', 0 , 'n', 0 };



unsigned char CH374ErrFlag;									//错误清0
UINT8	UsbConfig = 0;	// USB配置标志
unsigned char mVarSetupRequest;						 //USB请求码

unsigned char mVarSetupLength;				    	 //	控制传输后续数据长度
const unsigned char* VarSetupDescr;			     //描述符偏移地址




void CH374DeviceInit()  // 初始化USB设备
{
	
    Ch374Device.Device.Des = CustomHID_DeviceDescriptor;
    Ch374Device.Device.Size = sizeof(CustomHID_DeviceDescriptor);
    
    Ch374Device.Config.Des = CustomHID_ConfigDescriptor;
    Ch374Device.Config.Size = sizeof(CustomHID_ConfigDescriptor);


	
	Ch374Device.Langage.Des = LangDescr;
	Ch374Device.Langage.Size = sizeof(LangDescr);
	
	Ch374Device.Manu.Des = ManuInfo;
	Ch374Device.Manu.Size = sizeof(ManuInfo);
	
	Ch374Device.Produce.Des = ProdInfo;
	Ch374Device.Produce.Size = sizeof(ProdInfo);

	Write374Byte(0x05, Read374Byte(0x05)|BIT_CTRL_RESET_NOW);
	System72MDelay1ms(5);  // 等待CH374复位完成
	Write374Byte(0x05, Read374Byte(0x05)&(~BIT_CTRL_RESET_NOW));
	System72MDelay1ms(5);  // 等待CH374复位完成
	
	Write374Byte( REG_USB_ADDR, 0x00 );
	Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );
	Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( 0 ) );
	Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( 0 ) );
	Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // 清所有中断标志
	Write374Byte( REG_INTER_EN, BIT_IE_TRANSFER | BIT_IE_BUS_RESET | BIT_IE_USB_SUSPEND );  // 允许传输完成中断和USB总线复位中断以及USB总线挂起中断,芯片唤醒完成中断
	Write374Byte( REG_SYS_CTRL, BIT_CTRL_OE_POLAR );  // 对于CH374T或者UEN引脚悬空的CH374S必须置BIT_CTRL_OE_POLAR为1
	Write374Byte( REG_USB_SETUP, BIT_SETP_TRANS_EN | BIT_SETP_PULLUP_EN );  // 启动USB设备
}

void CH374DeviceInterrupt(void)  // USB设备中断服务程序
{
	u8 InterruptFlag, InterruptStatus;
	UINT8	length;
	
	InterruptFlag = Read374Byte( REG_INTER_FLAG);  // 读取中断标志位，获取中断状态
	if ( InterruptFlag & BIT_IF_BUS_RESET ) 
	{// USB总线复位			
		
		Write374Byte( REG_USB_ADDR, 0x00 );  // 清USB设备地址
		Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );
		Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( 0 ) );
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( 0 ) );
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_BUS_RESET );  // 清中断标志
	}
	else if ( InterruptFlag & BIT_IF_TRANSFER ) 
	{  // USB传输完成
		InterruptStatus = Read374Byte( REG_USB_STATUS );
		switch(InterruptStatus & BIT_STAT_PID_ENDP ) 
		{  // USB设备中断状态
			case USB_INT_EP2_OUT: // 批量端点下传成功 					
				//if (InterruptStatus & BIT_STAT_TOG_MATCH ) 
				{  // 仅同步包						
						DeviceHidOutCallBack();					
				}
				break;
			case USB_INT_EP2_IN: // 批量端点上传成功,未处理 
				Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_TRAN_TOG );

					DeviceHidSendFinishCallBack();

				break;
			case USB_INT_EP1_IN:  // 中断端点上传成功,未处理
				Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( Read374Byte( REG_USB_ENDP1 ) ) ^ BIT_EP1_TRAN_TOG );
				break;
			case USB_INT_EP0_SETUP: // 控制传输
			{  
				USB_SETUP_REQ	SetupReqBuf;
				
				CH374ErrFlag = 0;
				
				length = Read374Byte( REG_USB_LENGTH );
				if (length == 8) 
				{
					
					Read374Block( RAM_ENDP0_RECV, length, (PUINT8)&SetupReqBuf );
					mVarSetupLength = SetupReqBuf.wLengthL;//数据过程(如果有)所需要传输的字节数
					if ( SetupReqBuf.wLengthH || mVarSetupLength > 0x7F )
					{
						mVarSetupLength = 0x7F;  // 限制总长度
					}
					length = 0;  // 默认为成功并且上传0长度
					//D7位表示传输方向0 表示主机到设备，1表示设备到主机
					//D6~5位表示请求的类型，0为标准请求，1为类请求，2为厂商请求，3为保留
					//D4~0位表示请求的接收者 0表示设备，1表示接口，2表示端点 3表示其他 4-31保留
					if((SetupReqBuf.bType)&0x40)
					{//厂商请求，未处理
					
					}
					if((SetupReqBuf.bType)&0x20)
					{//类请求，未处理
						if(SetupReqBuf.bReq==0xfe)
						{//类请求得到逻辑盘数目，这里只有一个盘所以		
							Write374Byte( RAM_ENDP0_TRAN, 0 );
							if ( mVarSetupLength >= 1 ) length = 1;							
						}
						else if(SetupReqBuf.bReq==0xff)
						{//复位逻辑单元，这里未处理
//							DeviceMscStage = COMMAND_STAGE;
						}							
					}
					if ( ( SetupReqBuf.bType & DEF_USB_REQ_TYPE ) == DEF_USB_REQ_STAND ) //
					{// 标准请求
						mVarSetupRequest = SetupReqBuf.bReq;  // 请求码
						switch( mVarSetupRequest ) 
						{
							case DEF_USB_GET_DESCR://获取描述符请求
								switch( SetupReqBuf.wValueH ) 
								{
									case 1://设备描述符
										VarSetupDescr = (u8*)Ch374Device.Device.Des;
										length = Ch374Device.Device.Size;
										break;
									case 2://配置描述符
										VarSetupDescr = (u8*)Ch374Device.Config.Des;
										length = Ch374Device.Config.Size;
										break;
									case 3://字符串描述符
										switch( SetupReqBuf.wValueL ) 
										{
											case 1:
												VarSetupDescr = (u8*)Ch374Device.Manu.Des;
												length = Ch374Device.Manu.Size;
												break;
											case 2:
												VarSetupDescr = (u8*)Ch374Device.Produce.Des;
												length = Ch374Device.Produce.Size;	
												break;
											case 0:
												VarSetupDescr = (u8*)Ch374Device.Langage.Des;
												length = Ch374Device.Langage.Size;											
												break;
											case USB_REPORT_DESCR_TYPE://HID设备需要
												VarSetupDescr = (PUINT8)( &CustomHID_ReportDescriptor[0] );
												length = sizeof( CustomHID_ReportDescriptor );											
												break;
											default:
												CH374ErrFlag = 1;  // 操作失败
												break;
										}
										break;
									case USB_REPORT_DESCR_TYPE://HID设备需要
										VarSetupDescr = (PUINT8)( &CustomHID_ReportDescriptor[0] );
										length = sizeof( CustomHID_ReportDescriptor );											
										break;										
									default:
										CH374ErrFlag = 1;  // 操作失败
										break;
								}
								if ( mVarSetupLength > length )
								{
									mVarSetupLength = length;  // 限制总长度
								}
								length = mVarSetupLength >= RAM_ENDP0_SIZE ? RAM_ENDP0_SIZE : mVarSetupLength;  // 本次传输长度
								Write374Block( RAM_ENDP0_TRAN, length, (u8*)VarSetupDescr );  /* 加载上传数据 */
								mVarSetupLength -= length;
								VarSetupDescr += length;
								break;
							case DEF_USB_SET_ADDRESS://设置地址请求
								mVarSetupLength = SetupReqBuf.wValueL;  // 暂存USB设备地址
								break;
							case DEF_USB_GET_CONFIG://获取配置请求
								Write374Byte( RAM_ENDP0_TRAN, UsbConfig );
								if ( mVarSetupLength >= 1 ) length = 1;
								break;
							case DEF_USB_SET_CONFIG://设置配置请求
								UsbConfig = SetupReqBuf.wValueL;
								break;
							case DEF_USB_CLR_FEATURE://清除特性请求
								if ( ( SetupReqBuf.bType & 0x1F ) == 0x02 ) 
								{  // 不是端点不支持
									switch( SetupReqBuf.wIndexL ) 
									{
										case 0x82:
											Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( Read374Byte( REG_USB_ENDP2 ) ) );
											break;
										case 0x02:
											Write374Byte( REG_USB_ENDP2, M_SET_EP2_RECV_ACK( Read374Byte( REG_USB_ENDP2 ) ) );
											break;
										case 0x81:
											Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( Read374Byte( REG_USB_ENDP1 ) ) );
											break;
										case 0x01:
											Write374Byte( REG_USB_ENDP1, M_SET_EP1_RECV_ACK( Read374Byte( REG_USB_ENDP1 ) ) );
											break;
										default:
											CH374ErrFlag = 1;  // 操作失败
											break;
									}
								}
								else
								{
									CH374ErrFlag = 1;  // 操作失败
								}
								break;
							case DEF_USB_GET_INTERF://获取接口请求
								Write374Byte( RAM_ENDP0_TRAN, 0 );
								if ( mVarSetupLength >= 1 ) length = 1;
								break;
							case DEF_USB_GET_STATUS://获取状态请求
								Write374Byte( RAM_ENDP0_TRAN, 0 );
								Write374Byte( RAM_ENDP0_TRAN + 1, 0 );
								if ( mVarSetupLength >= 2 ) length = 2;
								else length = mVarSetupLength;
								break;
							default:
								CH374ErrFlag = 1;  // 操作失败
								break;
						}
					}
					else
					{/* 不支持的请求 */
						CH374ErrFlag = 1;  // 操作失败
					}					
				}
				else
				{
					CH374ErrFlag = 1;  // 操作失败
				}
				if (CH374ErrFlag)
				{  // 操作失败
					Write374Byte( REG_USB_ENDP0, M_SET_EP0_RECV_STA( M_SET_EP0_TRAN_STA( 0 ) ) );  // STALL
				}
				else
				{
					if (length <= RAM_ENDP0_SIZE ) 
					{  // 上传数据
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_ACK( M_SET_EP0_RECV_ACK( Read374Byte( REG_USB_ENDP0 ) ), length ) | BIT_EP0_TRAN_TOG );  // DATA1
					}
					else 
					{  // 下传数据或其它
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( M_SET_EP0_RECV_ACK( Read374Byte( REG_USB_ENDP0 ) ) ) | BIT_EP0_RECV_TOG );  // DATA1
					}				
				}
				break;
			}
			case USB_INT_EP0_IN: 
				switch( mVarSetupRequest ) 
				{
					case DEF_USB_GET_DESCR:
						length = mVarSetupLength >= RAM_ENDP0_SIZE ? RAM_ENDP0_SIZE : mVarSetupLength;  // 本次传输长度
						Write374Block( RAM_ENDP0_TRAN, length, (u8*)VarSetupDescr );  /* 加载上传数据 */
						mVarSetupLength -= length;
						VarSetupDescr += length;
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_ACK( Read374Byte( REG_USB_ENDP0 ), length ) ^ BIT_EP0_TRAN_TOG );
						break;
					case DEF_USB_SET_ADDRESS:
						Write374Byte( REG_USB_ADDR, mVarSetupLength );
					default:
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );  // 结束
						break;
				}
				break;
			case USB_INT_EP0_OUT: 
				Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );  // 结束
				break;
			default: 
				break;
		}
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_TRANSFER );  // 清中断标志
	}
	else if (InterruptFlag & BIT_IF_USB_SUSPEND ) 
	{  // USB总线挂起
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_USB_SUSPEND );  // 清中断标志
		Write374Byte( REG_SYS_CTRL, Read374Byte( REG_SYS_CTRL ) | BIT_CTRL_OSCIL_OFF );  // 时钟振荡器停止振荡,进入睡眠状态
	}
	else if (InterruptFlag & BIT_IF_WAKE_UP ) 
	{  // 芯片唤醒完成
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_WAKE_UP );  // 清中断标志
	}
	else 
	{  // 意外的中断,不可能发生的情况,除了硬件损坏
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // 清中断标志
	}
	//Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // 清中断标志
}



