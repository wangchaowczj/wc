#ifndef _CH374HOSTPROCESS_H_
#define _CH374HOSTPROCESS_H_


#define SUPPORT_DEVICE_MOUSE  			0x01
#define SUPPORT_DEVICE_MSC    			0x02
#define SUPPORT_DEVICE_KEYBOARD    		0x04

typedef enum
{
	BUS_FREE,
	WAITING_INSERT,
	DEVICE_INSERT,
	DEVICE_TYPE_ERR,
	ENUM_SUCCESS,	
	ENUM_ERROR_WAIING_LEAVE
}HOST_ENUM_STATE;


typedef struct
{
	HOST_ENUM_STATE EnumState;
	u8 SupportDeviceType;
	u8 DeviceType;
	struct
	{
		u32 BlackCount;//块数量
		u32 BlackSize;//块大小
	}Msc;	
}HOST_INFO;
extern HOST_INFO HostInfo;
extern void CH374Lock(void);
extern void CH374Unlock(void);
extern void SetHostEnumState(HOST_ENUM_STATE State);
extern HOST_ENUM_STATE GetHostEnumState(void);
extern void HostProcess(void);
extern void CH374InterruptCallBack(void);

#endif
