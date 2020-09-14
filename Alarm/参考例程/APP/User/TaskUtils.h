#ifndef _TASKUTILS_H_
#define _TASKUTILS_H_

typedef struct
{
	OS_STK Startup[APP_STARTUP_TASK_STK_SIZE];
	OS_STK Led1[APP_LED1_TASK_STK_SIZE];
}APP_PRIVATE_STK;


typedef struct
{
	OS_STK Stk[APP_PUBLIC_TASK_STK_SIZE];
}APP_PUBLIC_STK;

extern u8 TaskIsExist(u8 prio);
extern void RequestTaskDelAndWait(u8 prio);
extern INT8U TaskCreate(INT8U prio, void    *p_arg);

#endif
