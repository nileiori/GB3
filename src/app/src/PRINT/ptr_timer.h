#ifndef _PTR_TIMER_H
#define _PTR_TIMER_H

#include "stm32f10x.h"

//-----------Ӳ����ض���---------------------------------
#define PTR_CTRL_TIMER  TIM6 //�����ӡ������ר�õĶ�ʱ��
#define RCC_APB1Periph_PTR_CTRL_TIMER RCC_APB1Periph_TIM6
#define PTR_CTRL_TIMER_IRQn TIM6_IRQn
//--------------------------------------------------------

//----------�������---------------------
extern u32 __20us; //ʱ�䵥λ��10΢��
extern u32 __1ms; //ʱ�䵥λ��10΢��
//---------------------------------------


//----------�������---------------------
extern void ptr_timer_cfg(void);
extern void start_ptr_timer(void);
extern void close_ptr_timer(void);
//---------------------------------------

#endif

