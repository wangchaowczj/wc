#ifndef _CH374HOSTMSC_H_
#define _CH374HOSTMSC_H_

extern UINT8 mSaveUsbPid;			/* �������һ��ִ�е�����PID */
extern UINT8 mSaveDevEndpTog;		/* ���浱ǰ�����˵��ͬ����־ */
extern UINT8 mDiskBulkInEndp;		/* IN�˵��ַ */
extern UINT8 mDiskBulkOutEndp;		/* OUT�˵��ַ */
extern UINT8 mDiskInterfNumber;		/* �ӿں� */

extern void mSetBulkIn(void);
extern void mSetBulkOut(void);
extern UINT8 mDiskInquiry( PUINT8 DataBuf );
extern UINT8 mDiskCapacity( PUINT8 DataBuf );
extern UINT8 mRequestSense( void );
extern UINT8 mDiskTestReady( void );
extern UINT8 Ch374HostMscReadSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );
extern UINT8 Ch374HostMscWriteSector( UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf );
#endif
