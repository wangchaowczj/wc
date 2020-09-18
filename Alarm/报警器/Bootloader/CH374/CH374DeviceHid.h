#ifndef _CH374DEVICEHID_H_
#define _CH374DEVICEHID_H_

extern const u8 CustomHID_DeviceDescriptor[18];
extern const u8 CustomHID_ConfigDescriptor[41] ;
extern const uint8_t CustomHID_ReportDescriptor[38];

extern ErrorStatus DeviceHidSendData(u8* Data, u16 Bytes);

extern void DeviceHidOutCallBack(void);
extern void DeviceHidSendFinishCallBack(void);


#endif
