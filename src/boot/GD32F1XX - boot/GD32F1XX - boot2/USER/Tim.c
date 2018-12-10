#include "Tim.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

static	u32	Tim3Count = 0;

void Tim3_Isr(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	
		TIM_SetCounter(TIM3, 0);
		
		Tim3Count++;    
	}
	
}

u32 Tim3_GetCounter(void)
{
	return Tim3Count;
}

void Tim3_Init(void)
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	u16	PrescalerValue;
	
	 /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


  /* Compute the prescaler value */
  PrescalerValue = 3599;//50us,TIM2,3,4Ê±ÖÓÎª72MHz

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 20000;//1s
  //TIM_TimeBaseStructure.TIM_Period = 8000;//0.4s
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM3, 0);
  
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
