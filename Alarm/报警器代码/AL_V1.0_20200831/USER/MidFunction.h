#ifndef _MIDFUNCTION_H_
#define _MIDFUNCTION_H_

#define ARRAY_MAX 100

typedef struct
{
    u16 current[ARRAY_MAX];
    u8 front;
    u8 rear;
    u8 count;
    u8 queue_flag;
}QUEUE;    

extern QUEUE queue;

//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

extern void Time3_Init(u16 arr, u16 psc);
extern void STMOpen(void);
extern void AutoPowerOff(void);
extern void Sound(u8 p_arg);
extern void OLEDDisplay(void);
extern void UsbIdentify(void);
extern void Sys_Enter_Standby(void);
extern void PowerOffCheck(void);
extern void Queue_Init(void);
extern ErrorStatus Queue_push(u16 data);

#endif

