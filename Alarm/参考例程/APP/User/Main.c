#include "includes.h"

int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC800); //与配置文件相对应
	OSInit();
	TaskCreate(APP_STARTUP_TASK_PRIO, (void*)0);	
	OSStart();
	return 0;
}

