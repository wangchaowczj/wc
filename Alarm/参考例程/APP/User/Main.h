#ifndef _MAIN_H_
#define _MAIN_H_

#define VERSION_STRING  "MM01_V2.5_200805"



typedef struct
{
	u8 name;
	u8 ID[32];    //设备ID号
	u8 HW[32];    //硬件版本号
}DEVICE_INFO;
extern DEVICE_INFO DeviceInfo;

typedef struct
{
	u16 ref_min;
	u16 ref_max;
	u16 dual_min;
	u16 dual_max;
	u16 linab_min;
	u16 linab_max;
	u16 power_5v_min;
	u16 power_5v_max;
	u16 hv_min;
	u16 hv_max;
	u16 lv_min;
	u16 lv_max;	
	u16 ref_plus_min;
	u16 ref_plus_max;		
}SELF_PARAM;
extern SELF_PARAM self_param;


extern u32 event_state;
#endif
