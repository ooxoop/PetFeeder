#ifndef __RTC_H
#define	__RTC_H


#include "stm32f10x.h"

/* 如果定义了下面这个宏的话,PC13就会输出频率为RTC Clock/64的时钟 */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void RTC_NVIC_Configuration(void);
void RTC_Configuration(void);
uint32_t Time_Regulate(void);
void Time_Adjust(void);
void Time_Display(uint32_t TimeVar);
void Time_Show(void);
void Time_Show_Once(void);
int Time_GetCounter(void);
uint8_t USART_Scanf(uint32_t value);

#endif /* __XXX_H */
