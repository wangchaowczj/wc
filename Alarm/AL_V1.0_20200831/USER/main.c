//#include <stm32f10x.h>
#include "includes.h"

int main(void) 
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC800);
    OSInit();    
	TaskCreate(APP_STARTUP_TASK_PRIO, (void*)0);
	OSStart();	
}



