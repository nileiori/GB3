#ifndef SYS_TICK_H
#define SYS_TICK_H

#include "stm32f10x.h"

#define	SYSTICK_50ms			1              //50ms
#define	SYSTICK_100ms			2             //100ms
#define SYSTICK_0p1SECOND	2							//100ms

#define SYSTICK_1SECOND	20	////1s

#define SYSTICK_10SECOND	(10*SYSTICK_1SECOND)	

#define SYSTICK_1MINUTE		(60*SYSTICK_1SECOND)	

#define SYSTICK_10MINUTE	(600*SYSTICK_1SECOND)	

#define SYSTICK_1HOUR		(3600*SYSTICK_1SECOND)	

#define	SECOND	SYSTICK_1SECOND
#define MINUTE 	(SECOND*60)

void SysTick_Isr(void);
u32 SysTick_GetCounter(void);

#endif
