#ifndef _TASK_H_
#define _TASK_H_

#define VERSION_STRING   "AL_SW_V1.0_20200831"	     //ϵͳ�汾��

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
	OS_EVENT* usb_rcv; //USB�ź���
}MSG_SEM;
extern MSG_SEM MsgSem;

typedef struct
{		
	u8 UsbState;      //usb�ж�����ģʽ����������ܱ�ɾ����
    u8 OLED_status;   //OLED�Ƿ���뻶ӭҳ�� 1���ѽ��� 0��δ����
    u8 STM_status;    //��Ƭ���ϵ������Ƿ�����־ 1���ѷ��� 0��δ����
    u8 IZero_status;  //���ߵ����Ƿ�Ϊ�� 1���� 0����
    u8 I_ST;          //��ʱ�������ߵ����Ƿ�Ϊ�� 1���� 0����
}OP_DATA;
extern OP_DATA op_data;

typedef struct
{
	u8 name;
	u8 ID[32];    //�豸ID��
	u8 HW[32];    //Ӳ���汾��
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
