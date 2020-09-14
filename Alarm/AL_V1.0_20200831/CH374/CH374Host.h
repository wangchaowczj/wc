#ifndef _CH374HOST_H_
#define _CH374HOST_H_

// 附加的USB操作状态定义
#define		ERR_USB_UNKNOWN		0xFA		// 未知错误,不应该发生的情况,需检查硬件或者程序错误
#define		ERR_SUCCESS		0		/* 操作成功的返回状态 */

extern void Ch374DelayMs(u16 ms);
extern void Ch374DelayUs(u16 us);
extern u32 GetCurrentTime(void);
extern void HostSetBusFree(void);
extern u8 Query374DeviceIn(void);
extern void SetHostUsbAddr(UINT8 addr);
extern void HostSetBusReset(void);
extern u8 Query374DevFullSpeed(void);
extern void HostSetFullSpeed(void);
extern void HostSetLowSpeed(void);
extern UINT8 Wait374Interrupt(void);
extern UINT8 GetDeviceDescr(u8* buf);
extern UINT8 SetUsbAddress( UINT8 addr );
extern UINT8 GetConfigDescr( PUINT8 buf );
extern UINT8 SetUsbConfig(UINT8 cfg);
extern UINT8 WaitHostTransact374( UINT8 endp_addr, UINT8 pid, BOOL tog, UINT16 timeout );
extern UINT8 HostCtrlTransfer374( u8* ReqBuf, u8* DatBuf, u8* RetLen );
extern u8 GetDeviceDesInfo(UINT8* buf, u16 len);
extern UINT8  Set_Idle(void);
extern UINT8  Set_Report(unsigned char *p);
extern unsigned char Get_Hid_Des(unsigned char *p);
extern void Ch374Reset(void);

extern void HostDetectInterrupt(void);
extern void Ch374HostInit(u8 SupportDeviceType);

#endif
