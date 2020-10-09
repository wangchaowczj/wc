#ifndef _CH374DEVICEMSC_H_
#define _CH374DEVICEMSC_H_

typedef enum
{
	COMMAND_STAGE,
	DATA_STAGE,
	STATUS_STAGE
}DEVICE_MSC_STAGE;
extern DEVICE_MSC_STAGE DeviceMscStage;

extern u8 CBW[31];
extern u8 CSW[13];
extern const unsigned char  MSC_DeviceDescriptor[24];
extern const unsigned char  MSC_ConfigDescriptor[32];

extern void DeviceMscSendData(void);
extern void DeviceMscOutCallBack(void);

extern ErrorStatus MscReadSector(u32 Sector, u8 *Readbuff);
extern ErrorStatus MscWriteSector(u32 Sector, u8 *Writebuff);
#endif
