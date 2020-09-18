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
								
  
// ����������
const u8 LangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// ������Ϣ
const u8 ManuInfo[] = { 0x0E, 0x03, 'X', 0, 'i', 0, 'n', 0, '-', 0, 'A', 0 , 'n', 0 };
// ��Ʒ��Ϣ
const u8 ProdInfo[] = { 0x0E, 0x03, 'X', 0, 'i', 0, 'n', 0, '-', 0, 'A', 0 , 'n', 0 };



unsigned char CH374ErrFlag;									//������0
UINT8	UsbConfig = 0;	// USB���ñ�־
unsigned char mVarSetupRequest;						 //USB������

unsigned char mVarSetupLength;				    	 //	���ƴ���������ݳ���
const unsigned char* VarSetupDescr;			     //������ƫ�Ƶ�ַ




void CH374DeviceInit()  // ��ʼ��USB�豸
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
	System72MDelay1ms(5);  // �ȴ�CH374��λ���
	Write374Byte(0x05, Read374Byte(0x05)&(~BIT_CTRL_RESET_NOW));
	System72MDelay1ms(5);  // �ȴ�CH374��λ���
	
	Write374Byte( REG_USB_ADDR, 0x00 );
	Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );
	Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( 0 ) );
	Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( 0 ) );
	Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // �������жϱ�־
	Write374Byte( REG_INTER_EN, BIT_IE_TRANSFER | BIT_IE_BUS_RESET | BIT_IE_USB_SUSPEND );  // ����������жϺ�USB���߸�λ�ж��Լ�USB���߹����ж�,оƬ��������ж�
	Write374Byte( REG_SYS_CTRL, BIT_CTRL_OE_POLAR );  // ����CH374T����UEN�������յ�CH374S������BIT_CTRL_OE_POLARΪ1
	Write374Byte( REG_USB_SETUP, BIT_SETP_TRANS_EN | BIT_SETP_PULLUP_EN );  // ����USB�豸
}

void CH374DeviceInterrupt(void)  // USB�豸�жϷ������
{
	u8 InterruptFlag, InterruptStatus;
	UINT8	length;
	
	InterruptFlag = Read374Byte( REG_INTER_FLAG);  // ��ȡ�жϱ�־λ����ȡ�ж�״̬
	if ( InterruptFlag & BIT_IF_BUS_RESET ) 
	{// USB���߸�λ			
		
		Write374Byte( REG_USB_ADDR, 0x00 );  // ��USB�豸��ַ
		Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );
		Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( 0 ) );
		Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( 0 ) );
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_BUS_RESET );  // ���жϱ�־
	}
	else if ( InterruptFlag & BIT_IF_TRANSFER ) 
	{  // USB�������
		InterruptStatus = Read374Byte( REG_USB_STATUS );
		switch(InterruptStatus & BIT_STAT_PID_ENDP ) 
		{  // USB�豸�ж�״̬
			case USB_INT_EP2_OUT: // �����˵��´��ɹ� 					
				//if (InterruptStatus & BIT_STAT_TOG_MATCH ) 
				{  // ��ͬ����						
						DeviceHidOutCallBack();					
				}
				break;
			case USB_INT_EP2_IN: // �����˵��ϴ��ɹ�,δ���� 
				Write374Byte( REG_USB_ENDP2, M_SET_EP2_TRAN_NAK( Read374Byte( REG_USB_ENDP2 ) ) ^ BIT_EP2_TRAN_TOG );

					DeviceHidSendFinishCallBack();

				break;
			case USB_INT_EP1_IN:  // �ж϶˵��ϴ��ɹ�,δ����
				Write374Byte( REG_USB_ENDP1, M_SET_EP1_TRAN_NAK( Read374Byte( REG_USB_ENDP1 ) ) ^ BIT_EP1_TRAN_TOG );
				break;
			case USB_INT_EP0_SETUP: // ���ƴ���
			{  
				USB_SETUP_REQ	SetupReqBuf;
				
				CH374ErrFlag = 0;
				
				length = Read374Byte( REG_USB_LENGTH );
				if (length == 8) 
				{
					
					Read374Block( RAM_ENDP0_RECV, length, (PUINT8)&SetupReqBuf );
					mVarSetupLength = SetupReqBuf.wLengthL;//���ݹ���(�����)����Ҫ������ֽ���
					if ( SetupReqBuf.wLengthH || mVarSetupLength > 0x7F )
					{
						mVarSetupLength = 0x7F;  // �����ܳ���
					}
					length = 0;  // Ĭ��Ϊ�ɹ������ϴ�0����
					//D7λ��ʾ���䷽��0 ��ʾ�������豸��1��ʾ�豸������
					//D6~5λ��ʾ��������ͣ�0Ϊ��׼����1Ϊ������2Ϊ��������3Ϊ����
					//D4~0λ��ʾ����Ľ����� 0��ʾ�豸��1��ʾ�ӿڣ�2��ʾ�˵� 3��ʾ���� 4-31����
					if((SetupReqBuf.bType)&0x40)
					{//��������δ����
					
					}
					if((SetupReqBuf.bType)&0x20)
					{//������δ����
						if(SetupReqBuf.bReq==0xfe)
						{//������õ��߼�����Ŀ������ֻ��һ��������		
							Write374Byte( RAM_ENDP0_TRAN, 0 );
							if ( mVarSetupLength >= 1 ) length = 1;							
						}
						else if(SetupReqBuf.bReq==0xff)
						{//��λ�߼���Ԫ������δ����
//							DeviceMscStage = COMMAND_STAGE;
						}							
					}
					if ( ( SetupReqBuf.bType & DEF_USB_REQ_TYPE ) == DEF_USB_REQ_STAND ) //
					{// ��׼����
						mVarSetupRequest = SetupReqBuf.bReq;  // ������
						switch( mVarSetupRequest ) 
						{
							case DEF_USB_GET_DESCR://��ȡ����������
								switch( SetupReqBuf.wValueH ) 
								{
									case 1://�豸������
										VarSetupDescr = (u8*)Ch374Device.Device.Des;
										length = Ch374Device.Device.Size;
										break;
									case 2://����������
										VarSetupDescr = (u8*)Ch374Device.Config.Des;
										length = Ch374Device.Config.Size;
										break;
									case 3://�ַ���������
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
											case USB_REPORT_DESCR_TYPE://HID�豸��Ҫ
												VarSetupDescr = (PUINT8)( &CustomHID_ReportDescriptor[0] );
												length = sizeof( CustomHID_ReportDescriptor );											
												break;
											default:
												CH374ErrFlag = 1;  // ����ʧ��
												break;
										}
										break;
									case USB_REPORT_DESCR_TYPE://HID�豸��Ҫ
										VarSetupDescr = (PUINT8)( &CustomHID_ReportDescriptor[0] );
										length = sizeof( CustomHID_ReportDescriptor );											
										break;										
									default:
										CH374ErrFlag = 1;  // ����ʧ��
										break;
								}
								if ( mVarSetupLength > length )
								{
									mVarSetupLength = length;  // �����ܳ���
								}
								length = mVarSetupLength >= RAM_ENDP0_SIZE ? RAM_ENDP0_SIZE : mVarSetupLength;  // ���δ��䳤��
								Write374Block( RAM_ENDP0_TRAN, length, (u8*)VarSetupDescr );  /* �����ϴ����� */
								mVarSetupLength -= length;
								VarSetupDescr += length;
								break;
							case DEF_USB_SET_ADDRESS://���õ�ַ����
								mVarSetupLength = SetupReqBuf.wValueL;  // �ݴ�USB�豸��ַ
								break;
							case DEF_USB_GET_CONFIG://��ȡ��������
								Write374Byte( RAM_ENDP0_TRAN, UsbConfig );
								if ( mVarSetupLength >= 1 ) length = 1;
								break;
							case DEF_USB_SET_CONFIG://������������
								UsbConfig = SetupReqBuf.wValueL;
								break;
							case DEF_USB_CLR_FEATURE://�����������
								if ( ( SetupReqBuf.bType & 0x1F ) == 0x02 ) 
								{  // ���Ƕ˵㲻֧��
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
											CH374ErrFlag = 1;  // ����ʧ��
											break;
									}
								}
								else
								{
									CH374ErrFlag = 1;  // ����ʧ��
								}
								break;
							case DEF_USB_GET_INTERF://��ȡ�ӿ�����
								Write374Byte( RAM_ENDP0_TRAN, 0 );
								if ( mVarSetupLength >= 1 ) length = 1;
								break;
							case DEF_USB_GET_STATUS://��ȡ״̬����
								Write374Byte( RAM_ENDP0_TRAN, 0 );
								Write374Byte( RAM_ENDP0_TRAN + 1, 0 );
								if ( mVarSetupLength >= 2 ) length = 2;
								else length = mVarSetupLength;
								break;
							default:
								CH374ErrFlag = 1;  // ����ʧ��
								break;
						}
					}
					else
					{/* ��֧�ֵ����� */
						CH374ErrFlag = 1;  // ����ʧ��
					}					
				}
				else
				{
					CH374ErrFlag = 1;  // ����ʧ��
				}
				if (CH374ErrFlag)
				{  // ����ʧ��
					Write374Byte( REG_USB_ENDP0, M_SET_EP0_RECV_STA( M_SET_EP0_TRAN_STA( 0 ) ) );  // STALL
				}
				else
				{
					if (length <= RAM_ENDP0_SIZE ) 
					{  // �ϴ�����
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_ACK( M_SET_EP0_RECV_ACK( Read374Byte( REG_USB_ENDP0 ) ), length ) | BIT_EP0_TRAN_TOG );  // DATA1
					}
					else 
					{  // �´����ݻ�����
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( M_SET_EP0_RECV_ACK( Read374Byte( REG_USB_ENDP0 ) ) ) | BIT_EP0_RECV_TOG );  // DATA1
					}				
				}
				break;
			}
			case USB_INT_EP0_IN: 
				switch( mVarSetupRequest ) 
				{
					case DEF_USB_GET_DESCR:
						length = mVarSetupLength >= RAM_ENDP0_SIZE ? RAM_ENDP0_SIZE : mVarSetupLength;  // ���δ��䳤��
						Write374Block( RAM_ENDP0_TRAN, length, (u8*)VarSetupDescr );  /* �����ϴ����� */
						mVarSetupLength -= length;
						VarSetupDescr += length;
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_ACK( Read374Byte( REG_USB_ENDP0 ), length ) ^ BIT_EP0_TRAN_TOG );
						break;
					case DEF_USB_SET_ADDRESS:
						Write374Byte( REG_USB_ADDR, mVarSetupLength );
					default:
						Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );  // ����
						break;
				}
				break;
			case USB_INT_EP0_OUT: 
				Write374Byte( REG_USB_ENDP0, M_SET_EP0_TRAN_NAK( 0 ) );  // ����
				break;
			default: 
				break;
		}
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_TRANSFER );  // ���жϱ�־
	}
	else if (InterruptFlag & BIT_IF_USB_SUSPEND ) 
	{  // USB���߹���
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_USB_SUSPEND );  // ���жϱ�־
		Write374Byte( REG_SYS_CTRL, Read374Byte( REG_SYS_CTRL ) | BIT_CTRL_OSCIL_OFF );  // ʱ������ֹͣ��,����˯��״̬
	}
	else if (InterruptFlag & BIT_IF_WAKE_UP ) 
	{  // оƬ�������
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_WAKE_UP );  // ���жϱ�־
	}
	else 
	{  // ������ж�,�����ܷ��������,����Ӳ����
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // ���жϱ�־
	}
	//Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // ���жϱ�־
}



