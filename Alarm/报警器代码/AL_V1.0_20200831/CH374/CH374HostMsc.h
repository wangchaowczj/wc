#ifndef _CH374HOSTMSC_H_
#define _CH374HOSTMSC_H_

extern UINT8 mSaveUsbPid;			/* 保存最近一次执行的事务PID */
extern UINT8 mSaveDevEndpTog;		/* 保存当前批量端点的同步标志 */
extern UINT8 mDiskBulkInEndp;		/* IN端点地址 */
extern UINT8 mDiskBulkOutEndp;		/* OUT端点地址 */
extern UINT8 mDiskInterfNumber;		/* 接口号 */

extern void mSetBulkIn(void);
extern void mSetBulkOut(void);
extern UINT8 mDiskInquiry( PUINT8 DataBuf );
extern UINT8 mDiskCapacity( PUINT8 DataBuf );
extern UINT8 mRequestSense( void );
extern UINT8 mDiskTestReady( void );
extern UINT8 Ch374HostMscReadSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );
extern UINT8 Ch374HostMscWriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );
#endif
