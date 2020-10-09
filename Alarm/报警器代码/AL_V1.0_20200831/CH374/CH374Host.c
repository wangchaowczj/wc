#include "includes.h"


// ��ȡ�豸������
const	u8	SetupGetDevDescr[] = { 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00 };
// ��ȡ����������
const	u8	SetupGetCfgDescr[] = { 0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x04, 0x00 };
// ����USB��ַ
const	u8	SetupSetUsbAddr[] = { 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
// ����USB����
const	u8	SetupSetUsbConfig[] = { 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/* SET IDLE */
const	u8  SetupSetidle[]={0x21,0x0a,0x00,0x00,0x00,0x00,0x00,0x00}; 
/* SET REPORT */
const	u8  SetupSetReport[]={0x21,0x09,0x00,0x02,0x00,0x00,0x01,0x00};
/* ��ȡHID ���������� */
const	u8  SetupGetHidDes[]={0x81,0x06,0x00,0x22,0x00,0x00,0x81,0x00};

UINT8    hid_des_leng;      // HID�౨���������ĳ���

UINT8	UsbDevEndpSize = DEFAULT_ENDP0_SIZE;	/* USB�豸�Ķ˵�0�������ߴ� */
UINT8	FlagDeviceStatus;						/* ��ǰUSB�豸״̬��ͨ�������жϷ�ʽ��ȫ�ֱ�������������δʹ�� */





void Ch374DelayMs(u16 ms)
{
	OSTimeDly(ms);
}
void Ch374DelayUs(u16 us)
{
	while(us--)
	{
		System72MDelay1us();
	}	
}
u32 GetCurrentTime(void)
{
	return OSTimeGet();
}
void HostSetBusFree(void)  // USB���߿���
{
//	Write374Byte( REG_USB_SETUP, M_SET_USB_BUS_FREE( Read374Byte( REG_USB_SETUP ) ) );  // USB���߿���
	Write374Byte( REG_USB_SETUP, BIT_SETP_HOST_MODE );  // USB������ʽ
	Write374Byte( REG_USB_SETUP, BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF );  // USB������ʽ,����SOF
}
void SetHostUsbAddr(UINT8 addr)  // ����USB������ǰ������USB�豸��ַ
{
	Write374Byte( REG_USB_ADDR, addr );
}

u8 Query374DeviceIn(void)
{
	return ( ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_ATTACH ) ? TRUE : FALSE );
}
void HostSetBusReset(void)  // USB���߸�λ
{
	UsbDevEndpSize = DEFAULT_ENDP0_SIZE;  /* USB�豸�Ķ˵�0�������ߴ� */
	SetHostUsbAddr(0x00);
	Write374Byte( REG_USB_H_CTRL, 0x00);
//	Write374Byte( REG_USB_SETUP, M_SET_USB_BUS_RESET( Read374Byte( REG_USB_SETUP ) & ~ BIT_SETP_AUTO_SOF ) );  // USB���߸�λ
	Write374Byte( REG_USB_SETUP, M_SET_USB_BUS_RESET( BIT_SETP_HOST_MODE ) );  // USB���߸�λ
	Ch374DelayMs(15);  // USB���߸�λ�ڼ�,10mS��20mS
//	Write374Byte( REG_USB_SETUP, M_SET_USB_BUS_FREE( Read374Byte( REG_USB_SETUP ) ) );  // USB���߿���
	HostSetBusFree( );  // USB���߿���
	Ch374DelayMs(1);
	Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_USB_SUSPEND );  // ���жϱ�־
}

u8 Query374DevFullSpeed(void)
{
	return ( ( Read374Byte( REG_SYS_INFO ) & BIT_INFO_USB_DP ) ? TRUE : FALSE );
}
void HostSetFullSpeed(void)  // �趨ȫ��USB�豸���л���
{
	Write374Byte( REG_USB_SETUP, BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF );  // ȫ���ҷ�SOF
	Write374Byte( REG_HUB_SETUP, BIT_HUB0_EN );  // ʹ��HUB0�˿�
}

void HostSetLowSpeed(void)  // �趨����USB�豸���л���
{
	Write374Byte( REG_USB_SETUP, BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF | BIT_SETP_LOW_SPEED );  // �����ҷ�SOF
	Write374Byte( REG_HUB_SETUP, BIT_HUB0_EN | BIT_HUB0_POLAR );  // ʹ��HUB0�˿�
}
// �ȴ�CH374�ж�(INT#�͵�ƽ)����ʱ�򷵻�ERR_USB_UNKNOWN
UINT8 Wait374Interrupt(void)
{
	UINT16	i;
	for ( i = 0; i < 500; i ++ ) 
	{  // ������ֹ��ʱ
		if (GET_CH374_INT_PIN() == 0)
		{
			return( 0 );
		}
		Ch374DelayMs(1);
	}
	return( ERR_USB_UNKNOWN );  // ��Ӧ�÷��������
}
// CH374������������Ŀ�Ķ˵��ַ/PID����/ͬ����־������ͬCH375��NAK�����ԣ���ʱ/��������
UINT8	HostTransact374( UINT8 endp_addr, UINT8 pid, BOOL tog )
{  // ���ӳ��������������,����ʵ��Ӧ����,Ϊ���ṩ�����ٶ�,Ӧ�öԱ��ӳ����������Ż�
	UINT8	retry;
	UINT8	s, r, u;
	
	for ( retry = 0; retry < 3; retry ++ ) 
	{
		Write374Byte( REG_USB_H_PID, M_MK_HOST_PID_ENDP( pid, endp_addr ) );  // ָ������PID��Ŀ�Ķ˵��
//		Write374Byte( REG_USB_H_CTRL, BIT_HOST_START | ( tog ? ( BIT_HOST_TRAN_TOG | BIT_HOST_RECV_TOG ) : 0x00 ) );  // ����ͬ����־����������
		Write374Byte( REG_USB_H_CTRL, ( tog ? ( BIT_HOST_START | BIT_HOST_TRAN_TOG | BIT_HOST_RECV_TOG ) : BIT_HOST_START ) );  // ����ͬ����־����������
//		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE );  // ȡ����ͣ
		s = Wait374Interrupt( );
		if ( s == ERR_USB_UNKNOWN )
		{
			return( s );  // �жϳ�ʱ,������Ӳ���쳣
		}
		s = Read374Byte( REG_INTER_FLAG );  // ��ȡ�ж�״̬
		if ( s & BIT_IF_DEV_DETECT ) 
		{  // USB�豸����¼�
			Ch374DelayUs( 200 );  // �ȴ��������
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_TRANSFER );  // ���жϱ�־
			if ( s & BIT_IF_DEV_ATTACH ) 
			{  // USB�豸�����¼�
				u = Read374Byte( REG_USB_SETUP );
				if ( s & BIT_IF_USB_DX_IN ) 
				{  // �ٶ�ƥ�䣬����Ҫ�л��ٶ�
					if ( u & BIT_SETP_USB_SPEED )
					{
						return( USB_INT_CONNECT_LS );  // ����USB�豸
					}
					return( USB_INT_CONNECT );  // ȫ��USB�豸
				}
				else 
				{  // �ٶ�ʧ�䣬��Ҫ�л��ٶ�
					if ( u & BIT_SETP_USB_SPEED )
					{
						return( USB_INT_CONNECT );  // ȫ��USB�豸
					}
					return( USB_INT_CONNECT_LS );  // ����USB�豸
				}
			}
			else 
			{
				return( USB_INT_DISCONNECT );  // USB�豸�Ͽ��¼�
			}
		}
		else if ( s & BIT_IF_TRANSFER ) 
		{  // �������
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_TRANSFER );  // ���жϱ�־
			s = Read374Byte( REG_USB_STATUS );  // USB״̬
			r = s & BIT_STAT_DEV_RESP;  // USB�豸Ӧ��״̬
			switch ( pid ) 
			{
				case DEF_USB_PID_SETUP:
				case DEF_USB_PID_OUT:
					if ( r == DEF_USB_PID_ACK )
					{
						return( USB_INT_SUCCESS );
					}
					else if ( r == DEF_USB_PID_STALL || r == DEF_USB_PID_NAK )
					{
						return( r | 0x20 );
					}
					else if ( ! M_IS_HOST_TIMEOUT( s ) )
					{
						return( r | 0x20 );  // ���ǳ�ʱ/��������Ӧ��
					}
					break;
				case DEF_USB_PID_IN:
					if ( M_IS_HOST_IN_DATA( s ) ) 
					{  // DEF_USB_PID_DATA0 or DEF_USB_PID_DATA1
						if ( s & BIT_STAT_TOG_MATCH )
						{
							return( USB_INT_SUCCESS );  // ��ͬ�����趪��������
						}
					}
					else if ( r == DEF_USB_PID_STALL || r == DEF_USB_PID_NAK ) 
					{
						return( r | 0x20 );
					}
					else if ( ! M_IS_HOST_TIMEOUT( s ) ) 
					{
						return( r | 0x20 );  // ���ǳ�ʱ/��������Ӧ��
					}
					break;
				default:
					return( ERR_USB_UNKNOWN );  // �����ܵ����
//					break;
			}
		}
		else 
		{  // �����ж�,��Ӧ�÷��������
			Ch374DelayUs( 200 );  // �ȴ��������
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  /* ���жϱ�־ */
			if ( retry ) 
			{
				return( ERR_USB_UNKNOWN );  /* ���ǵ�һ�μ�⵽�򷵻ش��� */
			}
		}
	}
	return( 0x20 );  // Ӧ��ʱ
}
// CH374������������Ŀ�Ķ˵��ַ/PID����/ͬ����־/��mSΪ��λ��NAK������ʱ��(0xFFFF��������)������ͬCH375��NAK���ԣ���ʱ��������
UINT8 WaitHostTransact374( UINT8 endp_addr, UINT8 pid, BOOL tog, UINT16 timeout )
{
	UINT8 s;
	u32 c_time;
	
	c_time = OSTimeGet();
	while(OSTimeGet() < (c_time+timeout/(1000/OS_TICKS_PER_SEC)))
	{
		s = HostTransact374( endp_addr, pid, tog );
		if (s != ( DEF_USB_PID_NAK | 0x20 )) 
		{
			return( s );
		}
		if(Query374DeviceIn() == FALSE)
		{
			break;
		}
		Ch374DelayMs(2);
	}
	return( s );
}
UINT8	HostCtrlTransfer374( u8* ReqBuf, u8* DatBuf, u8* RetLen )  // ִ�п��ƴ���,ReqBufָ��8�ֽ�������,DatBufΪ�շ�������
// �����Ҫ���պͷ������ݣ���ôDatBuf��ָ����Ч���������ڴ�ź������ݣ�ʵ�ʳɹ��շ����ܳ��ȱ�����ReqLenָ����ֽڱ�����
{
	UINT8	s, len, count, total;
	BOOL	tog;
	
	Write374Block( RAM_HOST_TRAN, 8, ReqBuf );
	Write374Byte( REG_USB_LENGTH, 8 );
	Ch374DelayUs(100);
	s = WaitHostTransact374( 0, DEF_USB_PID_SETUP, FALSE, 200 );  // SETUP�׶Σ�200mS��ʱ
	if ( s == USB_INT_SUCCESS ) 
	{  // SETUP�ɹ�
		tog = TRUE;  // Ĭ��DATA1,Ĭ�������ݹ�״̬�׶�ΪIN
		total = *( ReqBuf + 6 );
		if ( total && DatBuf ) 
		{  // ��Ҫ�շ�����
			len = total;
			if ( *ReqBuf & 0x80 ) 
			{  // ��
				while ( len ) 
				{
					Ch374DelayUs( 100 );
					s = WaitHostTransact374( 0, DEF_USB_PID_IN, tog, 200 );  // IN����
					if ( s != USB_INT_SUCCESS ) break;
					count = Read374Byte( REG_USB_LENGTH );
					Read374Block( RAM_HOST_RECV, count, DatBuf );
					DatBuf += count;
					if ( count <= len ) len -= count;
					else len = 0;
					if ( count == 0 || ( count & ( UsbDevEndpSize - 1 ) ) ) break;  // �̰�
					tog = tog ? FALSE : TRUE;
				}
				tog = FALSE;  // ״̬�׶�ΪOUT
			}
			else 
			{  // ��
				while ( len ) 
				{
					Ch374DelayUs( 100 );
					count = len >= UsbDevEndpSize ? UsbDevEndpSize : len;
					Write374Block( RAM_HOST_TRAN, count, DatBuf );
					Write374Byte( REG_USB_LENGTH, count );
					s = WaitHostTransact374( 0, DEF_USB_PID_OUT, tog, 200 );  // OUT����
					if ( s != USB_INT_SUCCESS ) break;
					DatBuf += count;
					len -= count;
					tog = tog ? FALSE : TRUE;
				}
				tog = TRUE;  // ״̬�׶�ΪIN
			}
			total -= len;  // ��ȥʣ�೤�ȵ�ʵ�ʴ��䳤��
		}
		if ( s == USB_INT_SUCCESS ) 
		{  // ���ݽ׶γɹ�
			Write374Byte( REG_USB_LENGTH, 0 );
			Ch374DelayUs( 100 );
			s = WaitHostTransact374( 0, ( tog ? DEF_USB_PID_IN : DEF_USB_PID_OUT ), TRUE, 200 );  // STATUS�׶�
			if ( tog && s == USB_INT_SUCCESS ) 
			{  // ���IN״̬�������ݳ���
				if ( Read374Byte( REG_USB_LENGTH ) ) s = USB_INT_BUF_OVER;  // ״̬�׶δ���
			}
		}
	}
	if ( RetLen ) *RetLen = total;  // ʵ�ʳɹ��շ����ܳ���
	return( s );
}
UINT8 GetDeviceDescr(u8* buf)  // ��ȡ�豸������
{
	UINT8	s, len;
	
	s = HostCtrlTransfer374((PUINT8)SetupGetDevDescr, buf, &len );  // ִ�п��ƴ���
	if ( s == USB_INT_SUCCESS ) 
	{
		UsbDevEndpSize = ( (PUSB_DEV_DESCR)buf ) -> bMaxPacketSize0;  // �˵�0��������,���Ǽ򻯴���,����Ӧ���Ȼ�ȡǰ8�ֽں���������UsbDevEndpSize�ټ���
		if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL ) s = USB_INT_BUF_OVER;  // ���������ȴ���
	}
	return(s);
}
UINT8 SetUsbAddress( UINT8 addr )  // ����USB�豸��ַ
{
	UINT8	s;
	UINT8	BufSetAddr[ sizeof( SetupSetUsbAddr ) ] ;
	memcpy ( BufSetAddr, SetupSetUsbAddr, sizeof( SetupSetUsbAddr ) );
	( (PUSB_SETUP_REQ)BufSetAddr ) -> wValueL = addr;  // USB�豸��ַ
	s = HostCtrlTransfer374( BufSetAddr, NULL, NULL );  // ִ�п��ƴ���
	if ( s == USB_INT_SUCCESS ) {
		SetHostUsbAddr( addr );  // ����USB������ǰ������USB�豸��ַ
	}
	Ch374DelayMs( 3 );  // �ȴ�USB�豸��ɲ���
	return( s );
}
UINT8 GetConfigDescr(PUINT8 buf)  // ��ȡ����������
{
	UINT8	s, len;
	UINT8	BufLogDescr[ sizeof( SetupGetCfgDescr ) ] ;
	s = HostCtrlTransfer374((PUINT8)SetupGetCfgDescr, buf, &len );  // ִ�п��ƴ���
	if ( s == USB_INT_SUCCESS ) {
		if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL ) s = USB_INT_BUF_OVER;  // ���س��ȴ���
		else {
			memcpy ( BufLogDescr, SetupGetCfgDescr, sizeof( SetupGetCfgDescr ) );
			( (PUSB_SETUP_REQ)BufLogDescr ) -> wLengthL = ( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL;  // �����������������ܳ���
			s = HostCtrlTransfer374( BufLogDescr, buf, &len );  // ִ�п��ƴ���
			if ( s == USB_INT_SUCCESS ) {
				if ( len < ( (PUSB_CFG_DESCR)buf ) -> wTotalLengthL ) s = USB_INT_BUF_OVER;  // ���������ȴ���
			}
		}
	}
	return( s );
}
UINT8 SetUsbConfig(UINT8 cfg)  // ����USB�豸����
{
	UINT8	BufSetCfg[ sizeof( SetupSetUsbConfig ) ] ;
	memcpy ( BufSetCfg, SetupSetUsbConfig, sizeof( SetupSetUsbConfig ) );
	( (PUSB_SETUP_REQ)BufSetCfg ) -> wValueL = cfg;  // USB�豸����
	return( HostCtrlTransfer374( BufSetCfg, NULL, NULL ) );  // ִ�п��ƴ���
}
/* ����Idle */
UINT8  Set_Idle(void)    
{
	UINT8  s;
	s=HostCtrlTransfer374((u8*)SetupSetidle,NULL,NULL);
	return s;
}
/* ���ñ��� */
UINT8  Set_Report(unsigned char *p)
{
	UINT8  s,l=1;
	s=HostCtrlTransfer374((u8*)SetupSetReport,p,&l);            //ʵ�ʵ����ݿ���д������ݣ����������ü����ץ�������ڷ���ȥ
	return s;
}
/* ��ȡ���������� */
unsigned char Get_Hid_Des(unsigned char *p)//��ȡ����������
{
	unsigned char s;
//	leng=SetupGetHidDes[0x06]-0x40;//�����������ĳ����ڷ������ݳ��ȵĻ����ϼ�ȥ0X40
	unsigned char buffer[8];
	memcpy ( buffer, SetupGetHidDes, 8 );
	buffer[0x06] = hid_des_leng+0x40;
	s=HostCtrlTransfer374(buffer,p,&buffer[0x06]);
	return s;
}	
void HostDetectInterrupt(void)
{
	UINT8	s, u;
	
	s = Read374Byte( REG_INTER_FLAG );  // ��ȡ�ж�״̬
	if ( s & BIT_IF_DEV_DETECT ) 
	{  // USB�豸����¼�
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT );  // ���жϱ�־
		if ( s & BIT_IF_DEV_ATTACH ) 
		{  // USB�豸�����¼�
			u = Read374Byte( REG_USB_SETUP );
			if ( s & BIT_IF_USB_DX_IN ) 
			{  // �ٶ�ƥ�䣬����Ҫ�л��ٶ�
				if ( u & BIT_SETP_USB_SPEED ) FlagDeviceStatus = USB_INT_CONNECT_LS;  // ����USB�豸
				else FlagDeviceStatus = USB_INT_CONNECT;  // ȫ��USB�豸
			}
			else 
			{  // �ٶ�ʧ�䣬��Ҫ�л��ٶ�
				if ( u & BIT_SETP_USB_SPEED ) FlagDeviceStatus = USB_INT_CONNECT;  // ȫ��USB�豸
				else FlagDeviceStatus = USB_INT_CONNECT_LS;  // ����USB�豸
			}
		}
		else FlagDeviceStatus = USB_INT_DISCONNECT;  // USB�豸�Ͽ��¼�
	}
	else 
	{  // ������ж�
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_TRANSFER | BIT_IF_USB_SUSPEND | BIT_IF_WAKE_UP );  // ���жϱ�־
	}
}
void Ch374Reset(void)
{
	Write374Byte(0x05, Read374Byte(0x05)|BIT_CTRL_RESET_NOW);
}
/**
  *************************************************************************************
  * @brief  ��ʼ��USB����
  * @param  SupportDeviceType ֧�ֵ�USB�豸���� 
  * @retval ��
  * @author ZH
  * @date   2015��8��22��
  * @note   ��
  *************************************************************************************  
  */
void Ch374HostInit(u8 SupportDeviceType)
{
	HostInfo.SupportDeviceType = SupportDeviceType;	
	HostInfo.DeviceType = 0;
	SetHostEnumState(BUS_FREE);
	
	Write374Byte(0x05, Read374Byte(0x05)|BIT_CTRL_RESET_NOW);
	Ch374DelayMs(5);  // �ȴ�CH374��λ���
	Write374Byte(0x05, Read374Byte(0x05)&(~BIT_CTRL_RESET_NOW));
	Ch374DelayMs(5);  // �ȴ�CH374��λ���
	Write374Byte( REG_USB_SETUP, 0x00 );
	SetHostUsbAddr( 0x00 );
	Write374Byte( REG_USB_H_CTRL, 0x00 );
	Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // �������жϱ�־
//	Write374Byte( REG_INTER_EN, BIT_IE_TRANSFER );  // ����������ж�,��Ϊ������ʹ�ò�ѯ��ʽ���USB�豸���,��������USB�豸����ж�
	Write374Byte( REG_INTER_EN, BIT_IE_TRANSFER | BIT_IE_DEV_DETECT );  // ����������жϺ�USB�豸����ж�
	Write374Byte( REG_SYS_CTRL, BIT_CTRL_OE_POLAR );  // ����CH374T����UEN�������յ�CH374S������BIT_CTRL_OE_POLARΪ1
	HostSetBusFree( );  // USB���߿���	
}
u8 GetDeviceDesInfo(UINT8* buf, u16 len)
{
	u8 s;
	
	memset(buf, 0, len);
	s = GetDeviceDescr(buf);  // ��ȡ�豸������
	if ( s != USB_INT_SUCCESS ) 
	{
		return s;  // ��ֹ����,�ȴ�USB�豸�γ�
	}
	
	s = SetUsbAddress( 0x02 );  // ����USB�豸��ַ,������Щ�ӻ�����Ӧ�������
//	if ( s != USB_INT_SUCCESS ) 
//	{
//		return s;  // ��ֹ����,�ȴ�USB�豸�γ�
//	}
	
	memset(buf, 0, len);
	s = GetConfigDescr( buf );  // ��ȡ����������
	if ( s != USB_INT_SUCCESS )
	{
		return s;  // ��ֹ����,�ȴ�USB�豸�γ�
	}
	return USB_INT_SUCCESS;
}



