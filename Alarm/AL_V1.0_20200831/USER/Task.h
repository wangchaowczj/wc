#ifndef _TASK_H_
#define _TASK_H_

#define VERSION_STRING   "AL_SW_V1.0_20200831"	     //系统版本号

typedef struct
{
	OS_STK Startup[APP_STARTUP_TASK_STK_SIZE];
    OS_STK LED[APP_LED_TASK_STK_SIZE];
    OS_STK OLED[APP_OLED_TASK_STK_SIZE];
    OS_STK Usb[APP_USB_TASK_STK_SIZE];
    OS_STK TEST[APP_TEST_TASK_STK_SIZE];

}APP_PRIVATE_STK;

typedef struct
{
	OS_EVENT* usb_rcv;
}MSG_SEM;
extern MSG_SEM MsgSem;

typedef struct
{		
	u8 UsbState; 
    u8 OLED_status;
    u8 det_status;
    u8 STM_status;
    u8 IZero_status;
    u8 I_ST;
//    u8 USB_INTER;
}OP_DATA;

extern OP_DATA op_data;

typedef struct
{
	u8 name;
	u8 ID[32];    //设备ID号
	u8 HW[32];    //硬件版本号
//    u8 DetClass;  //0表示XA，1表示BJ
}DEVICE_INFO;
extern DEVICE_INFO DeviceInfo;

typedef struct 
{
    u8 poweroff_flag;
}POWER_STATUS;
extern POWER_STATUS power_status;

extern void AppStartUpTask(void* p_arg);
extern INT8U TaskCreate(INT8U prio, void*  p_arg);

extern void AppLEDTask(void* p_arg);
extern void AppLED1Task(void* p_arg);
extern void AppUSBHostTask(void* p_arg);
extern void AppUSBDeviceTask(void* p_arg);
extern void AppUsbTask(void* p_arg);
extern void AppOLEDTask(void* p_arg);
extern void AppTestTask(void* p_arg);
#endif
