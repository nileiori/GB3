#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "ptr_stepmotor_ctrl.h"
#include "ptr_timer.h" 

u32 __20us; //ʱ�䵥λ��10΢��
u32 __1ms; //ʱ�䵥λ��10΢��
u32 __50ms;
static u32 __cnt;
static u32 __50mscnt;


/*************************************************************
** ��ʱ����ʼ��
** �޸�:fanqinghai 2015.09.01
**************************************************************/
void ptr_timer_cfg(void)
{
  /* NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = PTR_CTRL_TIMER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PTR_CTRL_TIMER, ENABLE);
    
    TIM_DeInit(PTR_CTRL_TIMER);
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Period = 19;    //����ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  //Ԥ��Ƶϵ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���

    TIM_TimeBaseInit(PTR_CTRL_TIMER, &TIM_TimeBaseStructure);
    TIM_ClearITPendingBit(PTR_CTRL_TIMER, TIM_IT_Update); 
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); //���ж� ����ж�
    TIM_Cmd(PTR_CTRL_TIMER, DISABLE);
*/
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    NVIC_InitStructure.NVIC_IRQChannel = PTR_CTRL_TIMER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PTR_CTRL_TIMER, ENABLE);

    TIM_DeInit(PTR_CTRL_TIMER);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(PTR_CTRL_TIMER, &TIM_TimeBaseStructure);

    TIM_PrescalerConfig(PTR_CTRL_TIMER, 71, TIM_PSCReloadMode_Immediate); //ϵͳʱ����96MHz������(95+1)��Ƶ�󣬵õ�1MHz����ʱ��

    TIM_SetAutoreload(PTR_CTRL_TIMER, 20); //20us�ж�һ��
    TIM_ARRPreloadConfig(PTR_CTRL_TIMER, ENABLE);
    TIM_ClearITPendingBit(PTR_CTRL_TIMER, TIM_IT_Update); 
    TIM_ITConfig(PTR_CTRL_TIMER, TIM_IT_Update, ENABLE);

    TIM_Cmd(PTR_CTRL_TIMER, DISABLE);



}

/******************************************************
** ������ӡ����ʱ��
*******************************************************/
void start_ptr_timer(void)
{
    __cnt = 0;
    __20us = 0;
    __1ms = 0;
    __50ms = 0;
    __50mscnt = 0;
    TIM_Cmd(PTR_CTRL_TIMER, ENABLE);
    
}

/******************************************************
** �رմ�ӡ����ʱ��
*******************************************************/
void close_ptr_timer(void)
{
    TIM_Cmd(PTR_CTRL_TIMER, DISABLE);
    __cnt = 0;
    __20us = 0;
    __1ms = 0;
    __50ms =0;
    __50mscnt = 0;
    PTR_PWR_OFF;
}

static u16 pcnt=0,ptime=0;

//-----20us�ж�һ��---------
void ptr_timer_irq(void)
{
    extern void ptr_heat_tick(void);
    extern void ptr_step_tick(void);
    extern void ptr_pwr_ctrl_tick(void);
    extern void heater_detect_tick(void);
    extern void _ptr_ctrl(void);
   pcnt++;
   if(pcnt==50000)
   {pcnt =0;
        ptime++;
   }
    
    __20us = 1;
    

    if(__cnt++ >= 50)
    {
        __1ms = 1;
        __cnt = 0;
        
        __50mscnt++;
        if(__50mscnt >= 50)
        {
            __50ms = 1;  
        }
    }

    if(__20us)
        ptr_heat_tick();

    if(__20us)
        ptr_step_tick();    
    
    
    if(__20us)
    {
        ptr_pwr_ctrl_tick();
    }
    //-----------------------------------
      
    if(__1ms)
    {  
        heater_detect_tick();
    }
    
    if(__20us)
        _ptr_ctrl();
    
    __1ms = 0;
    __50ms = 0;
}

void TIM6_IRQHandler(void)
{
    ptr_timer_irq();  
    TIM_ClearFlag(TIM6,TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);           //����жϱ�־λ
}
//---------------------------------------------------------

