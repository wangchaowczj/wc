/**
  *************************************************************************************
  * @file    Delay.h
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   本文件为Delay.c的头文件
  *************************************************************************************
  */
#ifndef _DELAY_H_
#define _DELAY_H_


extern void SysTickConfig(void);
extern void SystemDelay1us(void);
extern void SystemDelay10us(u8 num);
extern void SystemDelay100us(u8 num);
extern void SystemDelay1ms(u8 num);
#endif
