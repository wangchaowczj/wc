#include "includes.h"

const	UINT8	SetupClrEndpStall[] = { 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const	UINT8	BoResetUsbDisk[] = { 0x21, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* ********** BulkOnly传输协议层 ********** */

#ifdef BIG_ENDIAN
#define USB_BO_CBW_SIG		0x55534243	/* 命令块CBW识别标志'USBC' */
#define USB_BO_CSW_SIG		0x55534253	/* 命令状态块CSW识别标志'USBS' */
#else
#define USB_BO_CBW_SIG		0x43425355	/* 命令块CBW识别标志'USBC' */
#define USB_BO_CSW_SIG		0x53425355	/* 命令状态块CSW识别标志'USBS' */
#endif

#define USB_BO_CBW_SIZE			0x1F	/* 命令块CBW的总长度 */
#define USB_BO_CSW_SIZE			0x0D	/* 命令状态块CSW的总长度 */

typedef union _BULK_ONLY_CMD {
	struct {
		UINT32	mCBW_Sig;
		UINT32	mCBW_Tag;
		UINT32	mCBW_DataLen;			/* 输入: 数据传输长度 */
		UINT8	mCBW_Flag;				/* 输入: 传输方向等标志 */
		UINT8	mCBW_LUN;
		UINT8	mCBW_CB_Len;			/* 输入: 命令块的长度,有效值是1到16 */
		UINT8	mCBW_CB_Buf[16];		/* 输入: 命令块,该缓冲区最多为16个字节 */
	} mCBW;								/* BulkOnly协议的命令块, 输入CBW结构 */
	struct {
		UINT32	mCSW_Sig;
		UINT32	mCSW_Tag;
		UINT32	mCSW_Residue;			/* 返回: 剩余数据长度 */
		UINT8	mCSW_Status;			/* 返回: 命令执行结果状态 */
	} mCSW;								/* BulkOnly协议的命令状态块, 输出CSW结构 */
} BULK_ONLY_CMD;

BULK_ONLY_CMD	mBOC;			/* BulkOnly传输结构 */
UINT8 mSaveUsbPid;			/* 保存最近一次执行的事务PID */
UINT8 mSaveDevEndpTog;		/* 保存当前批量端点的同步标志 */
UINT8 mDiskBulkInEndp;		/* IN端点地址 */
UINT8 mDiskBulkOutEndp;		/* OUT端点地址 */
UINT8 mDiskInterfNumber;		/* 接口号 */


/* 检查磁盘是否连接 */
// 返回: ERR_SUCCESS 已经连接, USB_INT_CONNECT刚连接, USB_INT_DISCONNECT断开
UINT8 IsDiskConnect( void )
{
	if ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_DETECT ) 
	{  /* 检测到插拔事件 */
		SetHostUsbAddr( 0x00 );
		HostSetBusFree( );  // USB总线空闲
		Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_TRANSFER );  /* 清中断标志 */
		if ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_ATTACH ) return( USB_INT_CONNECT );  /* USB设备处于连接状态 */
		return( USB_INT_DISCONNECT );
	}
	if ( Read374Byte( REG_INTER_FLAG ) & BIT_IF_DEV_ATTACH ) 
	{  /* USB设备处于连接状态 */
		return( ERR_SUCCESS );  /* 连接状态 */
	}
	return( USB_INT_DISCONNECT );
}

/* 传输事务,需预先输入mSaveUsbPid,PID令牌+目的端点地址,同步标志,返回同CH375,NAK重试,出错重试 */
UINT8	HostTransactInter( void )
{
	u32 TimeOut = 0;
	UINT8	s, r, retry;
	
	TimeOut = GetCurrentTime() + 5000;
	for ( retry = 3; retry != 0; retry--) 
	{  /* 错误重试计数 */
		if(GetCurrentTime() > TimeOut)
		{//作超时处理，避免无限循环
			break;
		}
		s = Wait374Interrupt( );
		if ( s == ERR_USB_UNKNOWN )
		{
			return( s );  // 中断超时,可能是硬件异常
		}
		s = Read374Byte( REG_INTER_FLAG );  // 获取中断状态
		if ( s & BIT_IF_DEV_DETECT ) 
		{  /* USB设备插拔事件 */
			Ch374DelayUs( 200 );  // 等待传输完成
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_DEV_DETECT | BIT_IF_TRANSFER );  // 清中断标志
			if ( s & BIT_IF_DEV_ATTACH ) 
			{  /* USB设备连接事件 */
				if ( s & BIT_IF_USB_DX_IN ) 
				{  /* 速度匹配，不需要切换速度 */
					return( USB_INT_CONNECT );  /* 全速USB设备 */
				}
				else 
				{  /* 速度失配，需要切换速度 */
					return( USB_INT_CONNECT_LS );  /* 低速USB设备 */
				}
			}
			else {  /* USB设备断开事件 */
				return( USB_INT_DISCONNECT );
			}
		}
		else if ( s & BIT_IF_TRANSFER ) 
		{  /* 传输完成 */
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_TRANSFER );  /* 清中断标志 */
			s = Read374Byte( REG_USB_STATUS );  /* USB状态 */
			r = s & BIT_STAT_DEV_RESP;  /* USB设备应答状态 */
			if ( mSaveUsbPid == DEF_USB_PID_IN ) 
			{  /* IN */
				if ( M_IS_HOST_IN_DATA( s ) ) 
				{  /* DEF_USB_PID_DATA0 or DEF_USB_PID_DATA1 */
					if ( s & BIT_STAT_TOG_MATCH ) return( ERR_SUCCESS );  /* IN数据同步,操作成功 */
					goto mHostTransRetry;  /* 不同步则需丢弃后重试 */
				}
			}
			else 
			{  /* OUT */
				if ( r == DEF_USB_PID_ACK )
				{
					return( ERR_SUCCESS );  /* OUT or SETUP,操作成功 */
				}
			}
			if ( r == DEF_USB_PID_NAK ) 
			{  /* NAK重试 */
				retry ++;
				goto mHostTransRetry;
			}
			else if ( ! M_IS_HOST_TIMEOUT( s ) ) return( r | 0x20 );  /* 错误或者无需重试或者意外应答,不是超时/出错 */
		}
		else 
		{  /* 其它中断,不应该发生的情况 */
			Ch374DelayUs( 200 );  // 等待传输完成
			Write374Byte( REG_INTER_FLAG, BIT_IF_USB_PAUSE | BIT_IF_INTER_FLAG );  // 清中断标志
		}
mHostTransRetry:
		if ( retry > 1 ) 
		{  /* 继续重试 */
			Write374Byte( REG_USB_H_CTRL, Read374Byte( REG_USB_H_CTRL ) | BIT_HOST_START );  /* 再次启动传输 */
		}
	}
	return( 0x20 );  /* 应答超时 */
}

/* 设置批量数据输入 */
void mSetBulkIn(void)
{
	mSaveUsbPid = DEF_USB_PID_IN;  /* 输入 */
	Write374Byte( REG_USB_H_PID, M_MK_HOST_PID_ENDP( DEF_USB_PID_IN, mDiskBulkInEndp ) );  /* 指定令牌PID和目的端点号 */
	Write374Byte( REG_USB_H_CTRL, mSaveDevEndpTog | BIT_HOST_START );  /* 设置同步标志并启动传输 */
}

/* 设置批量数据输出 */
void mSetBulkOut(void)
{
	mSaveUsbPid = DEF_USB_PID_OUT;  /* 输出 */
	Write374Byte( REG_USB_H_PID, M_MK_HOST_PID_ENDP( DEF_USB_PID_OUT, mDiskBulkOutEndp ) );  /* 指定令牌PID和目的端点号 */
	Write374Byte( REG_USB_H_CTRL, mSaveDevEndpTog | BIT_HOST_START );  /* 设置同步标志并启动传输 */
}

/* 大小端数据转换 */
UINT32	mSwapEndian( UINT32 dat )
{
	return( ( dat << 24 ) & 0xFF000000 | ( dat << 8 ) & 0x00FF0000 | ( dat >> 8 ) & 0x0000FF00 | ( dat >> 24 ) & 0x000000FF );
}

UINT8	mClearEndpStall( UINT8 endp )
{
	UINT8	ReqBuf[ sizeof( SetupClrEndpStall ) ];
	memcpy( ReqBuf, SetupClrEndpStall, sizeof( SetupClrEndpStall ) );  // 清除端点的错误
	( (PUSB_SETUP_REQ)ReqBuf ) -> wIndexL = endp;  // 端点地址
	return( HostCtrlTransfer374( ReqBuf, NULL, NULL ) );  /* 清除端点的错误 */
}

UINT8	mResetErrorBOC( UINT8 LastError )
{
	UINT8	s;
	UINT8	ReqBuf[ sizeof( BoResetUsbDisk ) ];
	s = IsDiskConnect( );  // 检查U盘是否连接
	if ( s != ERR_SUCCESS ) return( s );  // 断开或者刚连接
	memcpy( ReqBuf, BoResetUsbDisk, sizeof( BoResetUsbDisk ) );  // 复位USB存储器
	( (PUSB_SETUP_REQ)ReqBuf ) -> wIndexL = mDiskInterfNumber;  // 接口号
	s = HostCtrlTransfer374( ReqBuf, NULL, NULL );  // 执行控制传输
	if ( s == USB_INT_DISCONNECT ) return( s );  /* 检测到USB设备断开事件,磁盘已经断开 */
	mClearEndpStall( mDiskBulkInEndp | 0x80 );  /* 清除IN端点错误 */
	mClearEndpStall( mDiskBulkOutEndp );  /* 清除OUT端点错误 */
	mSaveDevEndpTog &= ~ ( BIT_HOST_TRAN_TOG | BIT_HOST_RECV_TOG );
	return( LastError );  /* 返回原错误码 */
}

/* 执行基于BulkOnly协议的命令,该子程序比较简化,并未处理各种错误,实际应用必须处理 */
UINT8	mBulkOnlyCmd( PUINT8 DataBuf )
{
	UINT8	s, len;
	UINT32	TotalLen;
	mBOC.mCBW.mCBW_Sig = USB_BO_CBW_SIG;
	mBOC.mCBW.mCBW_Tag = 0x03740374;
	mBOC.mCBW.mCBW_LUN = 0;
	Write374Block( RAM_HOST_TRAN, USB_BO_CBW_SIZE, (PUINT8)( & mBOC.mCBW ) );  /* 向CH374主机端点的发送缓冲区写入CBW */
	Write374Byte( REG_USB_LENGTH, USB_BO_CBW_SIZE );
	mSetBulkOut( );  /* 输出 */
#ifdef DEBUG_NOW
	printf( "*B:C=%02XH\n", (UINT16)( mBOC.mCBW.mCBW_CB_Buf[0] ) );
#endif
	s = HostTransactInter( );  /* 向USB盘的OUT端点输出CBW */
	if ( s == USB_INT_DISCONNECT ) return( s );  /* 检测到USB设备断开事件,磁盘已经断开 */
	if ( s != ERR_SUCCESS ) 
	{  /* 发送CBW失败 */
#ifdef DEBUG_NOW
		printf( "*B:C1=%02XH\n", (UINT16)s );
#endif
		mResetErrorBOC( s );
		Write374Block( RAM_HOST_TRAN, USB_BO_CBW_SIZE, (PUINT8)( & mBOC.mCBW ) );  /* 向CH374主机端点的发送缓冲区写入CBW */
		Write374Byte( REG_USB_LENGTH, USB_BO_CBW_SIZE );
		mSetBulkOut( );  /* 输出 */
#ifdef DEBUG_NOW
		printf( "*B:C2=%02XH\n", (UINT16)( mBOC.mCBW.mCBW_CB_Buf[0] ) );
#endif
		s = HostTransactInter( );  /* 向USB盘的OUT端点输出CBW */
		if ( s != ERR_SUCCESS ) return( mResetErrorBOC( s ) );
	}
	mSaveDevEndpTog ^= BIT_HOST_TRAN_TOG;  /* OUT端点的数据同步标志翻转 */
	if ( mBOC.mCBW.mCBW_DataLen ) 
	{  /* 有数据需要传输,此处使用双缓冲区提高效率,但导致程序复杂 */
#ifdef BIG_ENDIAN
		TotalLen = mSwapEndian( mBOC.mCBW.mCBW_DataLen );
#else
		TotalLen = mBOC.mCBW.mCBW_DataLen;
#endif
#ifdef CH374_DOUBLE_BUF
		if ( mBOC.mCBW.mCBW_Flag & 0x80 ) 
		{  /* 接收 */
			Write374Byte( REG_USB_SETUP, M_SET_RAM_MODE_2RX( BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF ) );  /* 全速USB主机模式,USB总线空闲,发SOF,双缓冲区 */
			mSetBulkIn( );  /* 输入 */
			while ( TotalLen ) 
			{  /* 有数据需要传输 */
#ifdef DEBUG_NOW
				printf( "*B:I=%lX\n", TotalLen );
#endif
				s = HostTransactInter( );  /* 接收数据 */
				if ( s != ERR_SUCCESS ) 
				{  /* 操作失败 */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkInEndp | 0x80 );  /* 清除端点错误 */
					break;
				}
				mSaveDevEndpTog ^= BIT_HOST_RECV_TOG;  /* IN端点的数据同步标志翻转 */
				len = Read374Byte( REG_USB_LENGTH );
				if ( TotalLen > len ) TotalLen -= len;
				else TotalLen = 0;
				if ( len == 64 && TotalLen ) mSetBulkIn( );  /* 准备下一次输入 */
				Read374Block( mSaveDevEndpTog & BIT_HOST_RECV_TOG ? RAM_HOST_RECV : RAM_HOST_EXCH, len, DataBuf );
				DataBuf += len;
				if ( len < 64 ) break;
			}
		}
		else 
		{  /* 发送 */
			Write374Byte( REG_USB_SETUP, M_SET_RAM_MODE_2TX( BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF ) );  /* 全速USB主机模式,USB总线空闲,发SOF,双缓冲区 */
			len = TotalLen >= 64 ? 64 : TotalLen;
			Write374Block( mSaveDevEndpTog & BIT_HOST_TRAN_TOG ? RAM_HOST_EXCH : RAM_HOST_TRAN, len, DataBuf );
			Write374Byte( REG_USB_LENGTH, len );
			mSetBulkOut( );  /* 输出 */
			while ( TotalLen ) 
			{  /* 有数据需要传输 */
#ifdef DEBUG_NOW
				printf( "*B:O=%lX\n", TotalLen );
#endif
				TotalLen -= len;
				DataBuf += len;
				if ( len == 64 && TotalLen ) 
				{  /* 准备下一次输出 */
					len = TotalLen >= 64 ? 64 : TotalLen;								
					Write374Block( mSaveDevEndpTog & BIT_HOST_TRAN_TOG ? RAM_HOST_TRAN : RAM_HOST_EXCH, len, DataBuf );
				}
				else len = 0;
				s = HostTransactInter( );  /* 发送数据 */
				if ( s != ERR_SUCCESS ) 
				{  /* 操作失败 */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkOutEndp );  /* 清除端点错误 */
					break;
				}
				mSaveDevEndpTog ^= BIT_HOST_TRAN_TOG;  /* OUT端点的数据同步标志翻转 */
				if ( len ) 
				{
					Write374Byte( REG_USB_LENGTH, len );
					mSetBulkOut( );  /* 输出 */
				}
				else break;
			}
		}
		Write374Byte( REG_USB_SETUP, M_SET_RAM_MODE_OFF( BIT_SETP_HOST_MODE | BIT_SETP_AUTO_SOF ) );  /* 全速USB主机模式,USB总线空闲,发SOF */
#else
		if ( mBOC.mCBW.mCBW_Flag & 0x80 ) 
		{  /* 接收 */
			while ( TotalLen ) 
			{  /* 有数据需要传输 */
#ifdef DEBUG_NOW
				printf( "*B:I=%lX\n", TotalLen );
#endif
				mSetBulkIn( );  /* 输入 */
				s = HostTransactInter( );  /* 接收数据 */
				if ( s != ERR_SUCCESS ) 
				{  /* 操作失败 */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkInEndp | 0x80 );  /* 清除端点错误 */
					break;
				}
				len = Read374Byte( REG_USB_LENGTH );
				Read374Block( RAM_HOST_RECV, len, DataBuf );
				if ( TotalLen > len ) TotalLen -= len;
				else TotalLen = 0;
				DataBuf += len;
				mSaveDevEndpTog ^= BIT_HOST_RECV_TOG;  /* IN端点的数据同步标志翻转 */
				if ( len < 64 ) break;
			}
		}
		else 
		{  /* 发送 */
			while ( TotalLen ) 
			{  /* 有数据需要传输 */
#ifdef DEBUG_NOW
				printf( "*B:O=%lX\n", TotalLen );
#endif
				len = TotalLen >= 64 ? 64 : TotalLen;
				Write374Block( RAM_HOST_TRAN, len, DataBuf );
				Write374Byte( REG_USB_LENGTH, len );
				mSetBulkOut( );  /* 输出 */
				TotalLen -= len;
				DataBuf += len;
				s = HostTransactInter( );  /* 发送数据 */
				if ( s != ERR_SUCCESS ) 
				{  /* 操作失败 */
					if ( s == ( DEF_USB_PID_STALL | 0x20 ) ) mClearEndpStall( mDiskBulkOutEndp );  /* 清除端点错误 */
					break;
				}
				mSaveDevEndpTog ^= BIT_HOST_TRAN_TOG;  /* OUT端点的数据同步标志翻转 */
				if ( len < 64 ) break;
			}
		}
#endif
		if ( s != ERR_SUCCESS ) 
		{  /* 数据传输失败 */
			if ( s != ( DEF_USB_PID_STALL | 0x20 ) ) return( s );
		}
	}
	mSetBulkIn( );  /* 输入 */
#ifdef DEBUG_NOW
	printf( "*B:S\n" );
#endif
	s = HostTransactInter( );  /* 从USB盘的IN端点输入CSW */
	if ( s != ERR_SUCCESS ) return( s );  /* 接收CSW失败 */
	mSaveDevEndpTog ^= BIT_HOST_RECV_TOG;  /* IN端点的数据同步标志翻转 */
	len = Read374Byte( REG_USB_LENGTH );
	Read374Block( RAM_HOST_RECV, len, (PUINT8)( & mBOC.mCSW ) );
	if ( len != USB_BO_CSW_SIZE || mBOC.mCSW.mCSW_Sig != USB_BO_CSW_SIG ) return( USB_INT_DISK_ERR );
	if ( mBOC.mCSW.mCSW_Status == 0 ) return( ERR_SUCCESS );
	else if ( mBOC.mCSW.mCSW_Status >= 2 ) return( mResetErrorBOC( USB_INT_DISK_ERR ) );
	else return( USB_INT_DISK_ERR );  /* 磁盘操作错误 */
}
/* ********** SCSI/RBC/UFI命令层 ********** */

/* 检查磁盘错误状态 */
UINT8	mRequestSense( void )
{
	UINT8	buf[0x12];
	Ch374DelayMs(20);  // 延时20毫秒
#ifdef BIG_ENDIAN
	mBOC.mCBW.mCBW_DataLen = 0x12000000;
#else
	mBOC.mCBW.mCBW_DataLen = 0x00000012;
#endif
	mBOC.mCBW.mCBW_Flag = 0x80;
	mBOC.mCBW.mCBW_CB_Len = 6;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x03;  /* 命令码 */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x12;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	return( mBulkOnlyCmd( buf ) );  /* 执行基于BulkOnly协议的命令 */
}

/* 获取磁盘特性 */
UINT8	mDiskInquiry( PUINT8 DataBuf )
{
#ifdef BIG_ENDIAN
	mBOC.mCBW.mCBW_DataLen = 0x24000000;
#else
	mBOC.mCBW.mCBW_DataLen = 0x00000024;
#endif
	mBOC.mCBW.mCBW_Flag = 0x80;
	mBOC.mCBW.mCBW_CB_Len = 6;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x12;  /* 命令码 */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x24;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	return( mBulkOnlyCmd( DataBuf ) );  /* 执行基于BulkOnly协议的命令 */
}

/* 获取磁盘容量 */
UINT8	mDiskCapacity( PUINT8 DataBuf )
{
#ifdef BIG_ENDIAN
	mBOC.mCBW.mCBW_DataLen = 0x08000000;
#else
	mBOC.mCBW.mCBW_DataLen = 0x00000008;
#endif
	mBOC.mCBW.mCBW_Flag = 0x80;
	mBOC.mCBW.mCBW_CB_Len = 10;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x25;  /* 命令码 */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[6] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[7] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[8] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[9] = 0x00;
	return( mBulkOnlyCmd( DataBuf ) );  /* 执行基于BulkOnly协议的命令 */
}

/* 测试磁盘是否就绪 */
UINT8	mDiskTestReady( void )
{
	mBOC.mCBW.mCBW_DataLen = 0;
	mBOC.mCBW.mCBW_Flag = 0x00;
	mBOC.mCBW.mCBW_CB_Len = 6;
	mBOC.mCBW.mCBW_CB_Buf[0] = 0x00;  /* 命令码 */
	mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[2] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[3] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[4] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[5] = 0x00;
	return( mBulkOnlyCmd( NULL ) );  /* 执行基于BulkOnly协议的命令 */
}

/* 以扇区为单位从磁盘读取数据 */
UINT8 Ch374HostMscReadSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
	UINT8	err, s;
	UINT32	len;
	len = (UINT32)SectCount << 9;
	for ( err = 0; err < 3; err ++ ) {  /* 错误重试 */
#ifdef BIG_ENDIAN
		mBOC.mCBW.mCBW_DataLen = mSwapEndian( len );
#else
		mBOC.mCBW.mCBW_DataLen = len;
#endif
		mBOC.mCBW.mCBW_Flag = 0x80;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[0] = 0x28;  /* 命令码 */
		mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[2] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[3] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[4] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[5] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[6] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[7] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[8] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[9] = 0x00;
		s = mBulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( s == ERR_SUCCESS ) return( s );  /* 操作成功 */
		if ( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT || s == USB_INT_CONNECT_LS ) return( s );  /* 检测到USB设备断开事件,磁盘已经断开或者刚刚重新插上 */
		mRequestSense( );
	}
	return( s );  /* 错误 */
}

/* 以扇区为单位将数据写入磁盘 */
UINT8 Ch374HostMscWriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
	UINT8	err, s;
	UINT32	len;
	len = (UINT32)SectCount << 9;
	for ( err = 0; err < 3; err ++ ) {  /* 错误重试 */
#ifdef BIG_ENDIAN
		mBOC.mCBW.mCBW_DataLen = mSwapEndian( len );
#else
		mBOC.mCBW.mCBW_DataLen = len;
#endif
		mBOC.mCBW.mCBW_Flag = 0x00;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[0] = 0x2A;  /* 命令码 */
		mBOC.mCBW.mCBW_CB_Buf[1] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[2] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[3] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[4] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[5] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[6] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[7] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[8] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[9] = 0x00;
		s = mBulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( s == ERR_SUCCESS ) return( s );  /* 操作成功 */
		if ( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT || s == USB_INT_CONNECT_LS ) return( s );  /* 检测到USB设备断开事件,磁盘已经断开或者刚刚重新插上 */
		mRequestSense( );
	}
	return( s );  /* 错误 */
}


