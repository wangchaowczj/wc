#include "includes.h"

const	UINT8	SetupClrEndpStall[] = { 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const	UINT8	BoResetUsbDisk[] = { 0x21, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* ********** BulkOnly����Э��� ********** */

#ifdef BIG_ENDIAN
#define USB_BO_CBW_SIG		0x55534243	/* �����CBWʶ���־'USBC' */
#define USB_BO_CSW_SIG		0x55534253	/* ����״̬��CSWʶ���־'USBS' */
#else
#define USB_BO_CBW_SIG		0x43425355	/* �����CBWʶ���־'USBC' */
#define USB_BO_CSW_SIG		0x53425355	/* ����״̬��CSWʶ���־'USBS' */
#endif

#define USB_BO_CBW_SIZE			0x1F	/* �����CBW���ܳ��� */
#define USB_BO_CSW_SIZE			0x0D	/* ����״̬��CSW���ܳ��� */

typedef union _BULK_ONLY_CMD {
	struct {
		UINT32	mCBW_Sig;
		UINT32	mCBW_Tag;
		UINT32	mCBW_DataLen;			/* ����: ���ݴ��䳤�� */
		UINT8	mCBW_Flag;				/* ����: ���䷽��ȱ�־ */
		UINT8	mCBW_LUN;
		UINT8	mCBW_CB_Len;			/* ����: �����ĳ���,��Чֵ��1��16 */
		UINT8	mCBW_CB_Buf[16];		/* ����: �����,�û��������Ϊ16���ֽ� */
	} mCBW;								/* BulkOnlyЭ��������, ����CBW�ṹ */
	struct {
		UINT32	mCSW_Sig;
		UINT32	mCSW_Tag;
		UINT32	mCSW_Residue;			/* ����: ʣ�����ݳ��� */
		UINT8	mCSW_Status;			/* ����: ����ִ�н��״̬ */
	} mCSW;								/* BulkOnlyЭ�������״̬��, ���CSW�ṹ */
} BULK_ONLY_CMD;

BULK_ONLY_CMD	mBOC;			/* BulkOnly����ṹ */
UINT8 mSaveUsbPid;			/* �������һ��ִ�е�����PID */
UINT8 mSaveDevEndpTog;		/* ���浱ǰ�����˵��ͬ����־ */
UINT8 mDiskBulkInEndp;		/* IN�˵��ַ */
UINT8 mDiskBulkOutEndp;		/* OUT�˵��ַ */
UINT8 mDiskInterfNumber;		/* �ӿں� */


/* �������Ƿ����� */
// ����: ERR_SUCCESS �Ѿ�����, USB_INT_CONNECT������, USB_INT_DISCONNECT�Ͽ�
UINT8 IsDiskConnect( void )
{
	if ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_DETECT ) 
	{  /* ��⵽����¼� */
		SetHostUsbAddr( 0x00 );
		HostSetBusFree( );  // USB���߿���
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_TRANSFER );  /* ���жϱ�־ */
		if ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_ATTACH ) return( USB_INT_CONNECT );  /* USB�豸��������״̬ */
		return( USB_INT_DISCONNECT );
	}
	if ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_ATTACH ) 
	{  /* USB�豸��������״̬ */
		return( ERR_SUCCESS );  /* ����״̬ */
	}
	return( USB_INT_DISCONNECT );
}

/* ��������,��Ԥ������mSaveUsbPid,PID����+Ŀ�Ķ˵��ַ,ͬ����־,����ͬCH375,NAK����,�������� */
UINT8	HostTransactInter( void )
{
	u32 TimeOut = 0;
	UINT8	s, r, retry;
	
	TimeOut = GetCurrentTime() + 5000;
	for ( retry = 3; retry != 0; retry--) 
	{  /* �������Լ��� */
		if(GetCurrentTime() > TimeOut)
		{//����ʱ������������ѭ��
			break;
		}
		s = Wait374Interrupt( );
		if ( s == ERR_USB_UNKNOWN )
		{
			return( s );  // �жϳ�ʱ,������Ӳ���쳣
		}
		s = Read374Byte( REG_INTER_FLAG );  // ��ȡ�ж�״̬
		if ( s & BIT_IF_DEV_DETECT ) 
		{  /* USB�豸����¼� */
			Ch374DelayUs( 200 );  // �ȴ��������
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_TRANSFER );  // ���жϱ�־
			if ( s & BIT_IF_DEV_ATTACH ) 
			{  /* USB�豸�����¼� */
				if ( s & BIT_IF_USB_DX_IN ) 
				{  /* �ٶ�ƥ�䣬����Ҫ�л��ٶ� */
					return( USB_INT_CONNECT );  /* ȫ��USB�豸 */
				}
				else 
				{  /* �ٶ�ʧ�䣬��Ҫ�л��ٶ� */
					return( USB_INT_CONNECT_LS );  /* ����USB�豸 */
				}
			}
			else {  /* USB�豸�Ͽ��¼� */
				return( USB_INT_DISCONNECT );
			}
		}
		else if ( s & BIT_IF_TRANSFER ) 
		{  /* ������� */
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_TRANSFER );  /* ���жϱ�־ */
			s = Read374Byte( REG_USB_STATUS );  /* USB״̬ */
			r = s & BIT_STAT_DEV_RESP;  /* USB�豸Ӧ��״̬ */
			if ( mSaveUsbPid == DEF_USB_PID_IN ) 
			{  /* IN */
				if ( M_IS_HOST_IN_DATA( s ) ) 
				{  /* DEF_USB_PID_DATA0 or DEF_USB_PID_DATA1 */
					if ( s & BIT_STAT_TOG_MATCH ) return( ERR_SUCCESS );  /* IN����ͬ��,�����ɹ� */
					goto mHostTransRetry;  /* ��ͬ�����趪�������� */
				}
			}
			else 
			{  /* OUT */
				if ( r == DEF_USB_PID_ACK )
				{
					return( ERR_SUCCESS );  /* OUT or SETUP,�����ɹ� */
				}
			}
			if ( r == DEF_USB_PID_NAK ) 
			{  /* NAK���� */
				retry ++;
				goto mHostTransRetry;
			}
			else if ( ! M_IS_HOST_TIMEOUT( s ) ) return( r | 0x20 );  /* ��������������Ի�������Ӧ��,���ǳ�ʱ/���� */
		}
		else 
		{  /* �����ж�,��Ӧ�÷�������� */
			Ch374DelayUs( 200 );  // �ȴ��������
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // ���жϱ�־
		}
mHostTransRetry:
		if ( retry > 1 ) 
		{  /* �������� */
			Write374Byte( REG_USB_H_CTRL, Read374Byte( REG_USB_H_CTRL ) | BIT_HOST_START );  /* �ٴ��������� */
		}
	}
	return( 0x20 );  /* Ӧ��ʱ */
}

/* ���������������� */
void mSetBulkIn(void)
{
	mSaveUsbPid = DEF_USB_PID_IN;  /* ���� */
	Write374Byte( REG_USB_H_PID, M_MK_HOST_PID_ENDP( DEF_USB_PID_IN, mDiskBulkInEndp ) );  /* ָ������PID��Ŀ�Ķ˵�� */
	Write374Byte( REG_USB_H_CTRL, mSaveDevEndpTog | BIT_HOST_START );  /* ����ͬ����־���������� */
}

/* ��������������� */
void mSetBulkOut(void)
{
	mSaveUsbPid = DEF_USB_PID_OUT;  /* ��� */
	Write374Byte( REG_USB_H_PID, M_MK_HOST_PID_ENDP( DEF_USB_PID_OUT, mDiskBulkOutEndp ) );  /* ָ������PID��Ŀ�Ķ˵�� */
	Write374Byte( REG_USB_H_CTRL, mSaveDevEndpTog | BIT_HOST_START );  /* ����ͬ����־���������� */
}

/* ��С������ת�� */
UINT32	mSwapEndian( UINT32 dat )
{
	return( ( dat << 24 ) & 0xFF000000 | ( dat << 8 ) & 0x00FF0000 | ( dat >> 8 ) & 0x0000FF00 | ( dat >> 24 ) & 0x000000FF );
}

UINT8	mClearEndpStall( UINT8 endp )
{
	UINT8	ReqBuf[ sizeof( SetupClrEndpStall ) ];
	memcpy( ReqBuf, SetupClrEndpStall, sizeof( SetupClrEndpStall ) );  // ����˵�Ĵ���
	( (PUSB_SETUP_REQ)ReqBuf ) -> wIndexL = endp;  // �˵��ַ
	return( HostCtrlTransfer374( ReqBuf, NULL, NULL ) );  /* ����˵�Ĵ��� */
}

UINT8	mResetErrorBOC( UINT8 LastError )
{
	UINT8	s;
	UINT8	ReqBuf[ sizeof( BoResetUsbDisk ) ];
	s = IsDiskConnect( );  // ���U���Ƿ�����
	if ( s != ERR_SUCCESS ) return( s );  // �Ͽ����߸�����
	memcpy( ReqBuf, BoResetUsbDisk, sizeof( BoResetUsbDisk ) );  // ��λUSB�洢��
	( (PUSB_SETUP_REQ)ReqBuf ) -> wIndexL = mDiskInterfNumber;  // �ӿں�
	s = HostCtrlTransfer374( ReqBuf, NULL, NULL );  // ִ�п��ƴ���
	if ( s == USB_INT_DISCONNECT ) return( s );  /* ��⵽USB�豸�Ͽ��¼�,�����Ѿ��Ͽ� */
	mClearEndpStall( mDiskBulkInEndp | 0x80 );  /* ���IN�˵���� */
	mClearEndpStall( mDiskBulkOutEndp );  /* ���OUT�˵���� */
	mSaveDevEndpTog &= ~ ( BIT_HOST_TRAN_TOG | BIT_HOST_RECV_TOG );
	return( LastError );  /* ����ԭ������ */
}

/* ִ�л���BulkOnlyЭ�������,���ӳ���Ƚϼ�,��δ������ִ���,ʵ��Ӧ�ñ��봦�� */
UINT8	mBulkOnlyCmd( PUINT8 DataBuf )
{
	UINT8	s, len;
	UINT32	TotalLen;
	mBOC.mCBW.mCBW_Sig = USB_BO_CBW_SIG;
	mBOC.mCBW.mCBW_Tag = 0x03740374;
	mBOC.mCBW.mCBW_LUN = 0;
	Write374Block( RAM_HOST_TRAN, USB_BO_CBW_SIZE, (PUINT8)( & mBOC.mCBW ) );  /* ��CH374�����˵�ķ��ͻ�����д��CBW */
	Write374Byte( REG_USB_LENGTH, USB_BO_CBW_SIZE );
	mSetBulkOut( );  /* ��� */
#ifdef DEBUG_NOW
	printf( "*B:C=%02XH\n", (UINT16)( mBOC.mCBW.mCBW_CB_Buf[0] ) );
#endif
	s = HostTransactInter( );  /* ��USB�̵�OUT�˵����CBW */
	if ( s == USB_INT_DISCONNECT ) return( s );  /* ��⵽USB�豸�Ͽ��¼�,�����Ѿ��Ͽ� */
	if ( s != ERR_SUCCESS ) 
	{  /* ����CBWʧ�� */
#ifdef DEBUG_NOW
		printf( "*B:C1=%02XH\n", (UINT16)s );
#endif
		mResetErrorBOC( s );
		Write374Block( RAM_HOST_TRAN, USB_BO_CBW_SIZE, (PUINT8)( & mBOC.mCBW ) );  /* ��CH374�����˵�ķ��ͻ�����д��CBW */
		Write374Byte( REG_USB_LENGTH, USB_BO_CBW_SIZE );
		mSetBulkOut( );  /* ��� */
#ifdef DEBUG_NOW
		printf( "*B:C2=%02XH\n", (UINT16)( mBOC.mCBW.mCBW_CB_Buf[0] ) );
#endif
		s = HostTransactInter( );  /* ��USB�̵�OUT�˵����CBW */
		if ( s != ERR_SUCCESS ) return( mResetErrorBOC( s ) );
	}
	mSaveDevEndpTog ^= BIT_HOST_TRAN_TOG;  /* OUT�˵������ͬ����־��ת */
	if ( mBOC.mCBW.mCBW_DataLen ) 
	{  /* ��������Ҫ����,�˴�ʹ��˫���������Ч��,�����³����� */
#ifdef BIG_ENDIAN
		TotalLen = mSwapEndian( mBOC.mCBW.mCBW_DataLen );
#else
		TotalLen = mBOC.mCBW.mCBW_DataLen;
#endif
#ifdef CH374_DOUBLE_BUF
		if ( mBOC.mCBW.mCBW_Flag & 0x80 ) 
		{  /* ���� */
			Write374Byte( REG_USB_SETUP, M_SET_RAM_MODE_2RX( BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF ) );  /* ȫ��USB����ģʽ,USB���߿���,��SOF,˫������ */
			mSetBulkIn( );  /* ���� */
			while ( TotalLen ) 
			{  /* ��������Ҫ���� */
#ifdef DEBUG_NOW
				printf( "*B:I=%lX\n", TotalLen );
#endif
				s = HostTransactInter( );  /* �������� */
				if ( s != ERR_SUCCESS ) 
				{  /* ����ʧ�� */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkInEndp | 0x80 );  /* ����˵���� */
					break;
				}
				mSaveDevEndpTog ^= BIT_HOST_RECV_TOG;  /* IN�˵������ͬ����־��ת */
				len = Read374Byte( REG_USB_LENGTH );
				if ( TotalLen > len ) TotalLen -= len;
				else TotalLen = 0;
				if ( len == 64 && TotalLen ) mSetBulkIn( );  /* ׼����һ������ */
				Read374Block( mSaveDevEndpTog & BIT_HOST_RECV_TOG ? RAM_HOST_RECV : RAM_HOST_EXCH, len, DataBuf );
				DataBuf += len;
				if ( len < 64 ) break;
			}
		}
		else 
		{  /* ���� */
			Write374Byte( REG_USB_SETUP, M_SET_RAM_MODE_2TX( BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF ) );  /* ȫ��USB����ģʽ,USB���߿���,��SOF,˫������ */
			len = TotalLen >= 64 ? 64 : TotalLen;
			Write374Block( mSaveDevEndpTog & BIT_HOST_TRAN_TOG ? RAM_HOST_EXCH : RAM_HOST_TRAN, len, DataBuf );
			Write374Byte( REG_USB_LENGTH, len );
			mSetBulkOut( );  /* ��� */
			while ( TotalLen ) 
			{  /* ��������Ҫ���� */
#ifdef DEBUG_NOW
				printf( "*B:O=%lX\n", TotalLen );
#endif
				TotalLen -= len;
				DataBuf += len;
				if ( len == 64 && TotalLen ) 
				{  /* ׼����һ����� */
					len = TotalLen >= 64 ? 64 : TotalLen;								
					Write374Block( mSaveDevEndpTog & BIT_HOST_TRAN_TOG ? RAM_HOST_TRAN : RAM_HOST_EXCH, len, DataBuf );
				}
				else len = 0;
				s = HostTransactInter( );  /* �������� */
				if ( s != ERR_SUCCESS ) 
				{  /* ����ʧ�� */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkOutEndp );  /* ����˵���� */
					break;
				}
				mSaveDevEndpTog ^= BIT_HOST_TRAN_TOG;  /* OUT�˵������ͬ����־��ת */
				if ( len ) 
				{
					Write374Byte( REG_USB_LENGTH, len );
					mSetBulkOut( );  /* ��� */
				}
				else break;
			}
		}
		Write374Byte( REG_USB_SETUP, M_SET_RAM_MODE_OFF( BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF ) );  /* ȫ��USB����ģʽ,USB���߿���,��SOF */
#else
		if ( mBOC.mCBW.mCBW_Flag & 0x80 ) 
		{  /* ���� */
			while ( TotalLen ) 
			{  /* ��������Ҫ���� */
#ifdef DEBUG_NOW
				printf( "*B:I=%lX\n", TotalLen );
#endif
				mSetBulkIn( );  /* ���� */
				s = HostTransactInter( );  /* �������� */
				if ( s != ERR_SUCCESS ) 
				{  /* ����ʧ�� */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkInEndp | 0x80 );  /* ����˵���� */
					break;
				}
				len = Read374Byte( REG_USB_LENGTH );
				Read374Block( RAM_HOST_RECV, len, DataBuf );
				if ( TotalLen > len ) TotalLen -= len;
				else TotalLen = 0;
				DataBuf += len;
				mSaveDevEndpTog ^= BIT_HOST_RECV_TOG;  /* IN�˵������ͬ����־��ת */
				if ( len < 64 ) break;
			}
		}
		else 
		{  /* ���� */
			while ( TotalLen ) 
			{  /* ��������Ҫ���� */
#ifdef DEBUG_NOW
				printf( "*B:O=%lX\n", TotalLen );
#endif
				len = TotalLen >= 64 ? 64 : TotalLen;
				Write374Block( RAM_HOST_TRAN, len, DataBuf );
				Write374Byte( REG_USB_LENGTH, len );
				mSetBulkOut( );  /* ��� */
				TotalLen -= len;
				DataBuf += len;
				s = HostTransactInter( );  /* �������� */
				if ( s != ERR_SUCCESS ) 
				{  /* ����ʧ�� */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkOutEndp );  /* ����˵���� */
					break;
				}
				mSaveDevEndpTog ^= BIT_HOST_TRAN_TOG;  /* OUT�˵������ͬ����־��ת */
				if ( len < 64 ) break;
			}
		}
#endif
		if ( s != ERR_SUCCESS ) 
		{  /* ���ݴ���ʧ�� */
			if ( s != ( DEF_USB_PID_STALL | 0x20 ) ) return( s );
		}
	}
	mSetBulkIn( );  /* ���� */
#ifdef DEBUG_NOW
	printf( "*B:S\n" );
#endif
	s = HostTransactInter( );  /* ��USB�̵�IN�˵�����CSW */
	if ( s != ERR_SUCCESS ) return( s );  /* ����CSWʧ�� */
	mSaveDevEndpTog ^= BIT_HOST_RECV_TOG;  /* IN�˵������ͬ����־��ת */
	len = Read374Byte( REG_USB_LENGTH );
	Read374Block( RAM_HOST_RECV, len, (PUINT8)( & mBOC.mCSW ) );
	if ( len != USB_BO_CSW_SIZE || mBOC.mCSW.mCSW_Sig != USB_BO_CSW_SIG ) return( USB_INT_DISK_ERR );
	if ( mBOC.mCSW.mCSW_Status == 0 ) return( ERR_SUCCESS );
	else if ( mBOC.mCSW.mCSW_Status >= 2 ) return( mResetErrorBOC( USB_INT_DISK_ERR ) );
	else return( USB_INT_DISK_ERR );  /* ���̲������� */
}
/* ********** SCSI/RBC/UFI����� ********** */

/* �����̴���״̬ */
UINT8	mRequestSense( void )
{
	UINT8	buf[0x12];
	Ch374DelayMs(20);  // ��ʱ20����
#ifdef BIG_ENDIAN
	mBOC.mCBW.mCBW_DataLen = 0x12000000;
#else
	mBOC.mCBW.mCBW_DataLen = 0x00000012;
#endif
	mBOC.mCBW.mCBW_Flag = 0x80;
	mBOC.mCBW.mCBW_CB_Len = 6;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x03;  /* ������ */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x12;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	return( mBulkOnlyCmd( buf ) );  /* ִ�л���BulkOnlyЭ������� */
}

/* ��ȡ�������� */
UINT8	mDiskInquiry( PUINT8 DataBuf )
{
#ifdef BIG_ENDIAN
	mBOC.mCBW.mCBW_DataLen = 0x24000000;
#else
	mBOC.mCBW.mCBW_DataLen = 0x00000024;
#endif
	mBOC.mCBW.mCBW_Flag = 0x80;
	mBOC.mCBW.mCBW_CB_Len = 6;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x12;  /* ������ */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x24;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	return( mBulkOnlyCmd( DataBuf ) );  /* ִ�л���BulkOnlyЭ������� */
}

/* ��ȡ�������� */
UINT8	mDiskCapacity( PUINT8 DataBuf )
{
#ifdef BIG_ENDIAN
	mBOC.mCBW.mCBW_DataLen = 0x08000000;
#else
	mBOC.mCBW.mCBW_DataLen = 0x00000008;
#endif
	mBOC.mCBW.mCBW_Flag = 0x80;
	mBOC.mCBW.mCBW_CB_Len = 10;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x25;  /* ������ */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[6] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[7] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[8] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[9] = 0x00;
	return( mBulkOnlyCmd( DataBuf ) );  /* ִ�л���BulkOnlyЭ������� */
}

/* ���Դ����Ƿ���� */
UINT8	mDiskTestReady( void )
{
	mBOC.mCBW.mCBW_DataLen = 0;
	mBOC.mCBW.mCBW_Flag = 0x00;
	mBOC.mCBW.mCBW_CB_Len = 6;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x00;  /* ������ */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	return( mBulkOnlyCmd( NULL ) );  /* ִ�л���BulkOnlyЭ������� */
}

/* ������Ϊ��λ�Ӵ��̶�ȡ���� */
UINT8 Ch374HostMscReadSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
	UINT8	err, s;
	UINT32	len;
	len = (UINT32)SectCount << 9;
	for ( err = 0; err < 3; err ++ ) {  /* �������� */
#ifdef BIG_ENDIAN
		mBOC.mCBW.mCBW_DataLen = mSwapEndian( len );
#else
		mBOC.mCBW.mCBW_DataLen = len;
#endif
		mBOC.mCBW.mCBW_Flag = 0x80;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[0] = 0x28;  /* ������ */
		mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[2] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[3] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[4] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[5] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[6] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[7] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[8] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[9] = 0x00;
		s = mBulkOnlyCmd( DataBuf );  /* ִ�л���BulkOnlyЭ������� */
		if ( s == ERR_SUCCESS ) return( s );  /* �����ɹ� */
		if ( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT || s == USB_INT_CONNECT_LS ) return( s );  /* ��⵽USB�豸�Ͽ��¼�,�����Ѿ��Ͽ����߸ո����²��� */
		mRequestSense( );
	}
	return( s );  /* ���� */
}

/* ������Ϊ��λ������д����� */
UINT8 Ch374HostMscWriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
	UINT8	err, s;
	UINT32	len;
	len = (UINT32)SectCount << 9;
	for ( err = 0; err < 3; err ++ ) {  /* �������� */
#ifdef BIG_ENDIAN
		mBOC.mCBW.mCBW_DataLen = mSwapEndian( len );
#else
		mBOC.mCBW.mCBW_DataLen = len;
#endif
		mBOC.mCBW.mCBW_Flag = 0x00;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[0] = 0x2A;  /* ������ */
		mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[2] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[3] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[4] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[5] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[6] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[7] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[8] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[9] = 0x00;
		s = mBulkOnlyCmd( DataBuf );  /* ִ�л���BulkOnlyЭ������� */
		if ( s == ERR_SUCCESS ) return( s );  /* �����ɹ� */
		if ( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT || s == USB_INT_CONNECT_LS ) return( s );  /* ��⵽USB�豸�Ͽ��¼�,�����Ѿ��Ͽ����߸ո����²��� */
		mRequestSense( );
	}
	return( s );  /* ���� */
}


