#ifndef	TIM_H
#define	TIM_H

#include "stm32f10x.h"

void Tim3_Isr(void);
void Tim3_Init(void);
u32 Tim3_GetCounter(void);

#endif
