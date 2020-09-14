#ifndef _TASK_H_
#define _TASK_H_

extern STR_COMM  Uart1Data;

extern volatile u32 Uart1RecTimeLatest; 
extern volatile u32 Uart3RecTimeLatest; 

typedef struct
{
	OS_EVENT* Uart1;
}MSG_SEM;
extern MSG_SEM MsgSem;

typedef struct
{
	u8 Result;
    u8 HasSdCard;
    u8 LogName;
}OP_DATA;
extern OP_DATA OpData;




extern void AppStartUpTask(void* p_arg);
extern void Led1Task(void* p_arg);
extern void Uart1Task(void* p_arg);

#endif
