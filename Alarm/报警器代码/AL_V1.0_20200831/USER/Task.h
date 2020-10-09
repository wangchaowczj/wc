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
    OS_STK POWEROFF[APP_POWEROFF_TASK_STK_SIZE];
    OS_STK QUEUE[APP_QUEUE_TASK_STK_SIZE];


}APP_PRIVATE_STK;

typedef struct
{
	OS_EVENT* usb_rcv; //USB信号量
}MSG_SEM;
extern MSG_SEM MsgSem;

typedef struct
{		
	u8 UsbState;      //usb判断主从模式——这个功能被删掉了
    u8 OLED_status;   //OLED是否进入欢迎页面 1：已进入 0：未进入
    u8 STM_status;    //单片机上电提醒是否发生标志 1：已发生 0：未发生
    u8 IZero_status;  //总线电流是否为零 1：是 0：否
    u8 I_ST;          //定时器中总线电流是否为零 1：是 0：否
}OP_DATA;
extern OP_DATA op_data;

typedef struct
{
	u8 name;
	u8 ID[32];    //设备ID号
	u8 HW[32];    //硬件版本号
}DEVICE_INFO;
extern DEVICE_INFO DeviceInfo;

extern void AppStartUpTask(void* p_arg);
extern INT8U TaskCreate(INT8U prio, void*  p_arg);
extern void AppLEDTask(void* p_arg);
extern void AppLED1Task(void* p_arg);
extern void AppUSBHostTask(void* p_arg);
extern void AppUSBDeviceTask(void* p_arg);
extern void AppUsbTask(void* p_arg);
extern void AppOLEDTask(void* p_arg);
extern void AppTestTask(void* p_arg);
extern void AppPowerOffTask(void* p_arg);
extern void AppQueueTask(void* p_arg);


#endif
