/**
  ******************************************************************************
  * @file    SysTickApp.h 
  * @author  
  * @version 
  * @date    
  * @brief   SySTick��Ӧ�ã��ӳٺ����ͼ�ʱ����
  *
  *               
  ******************************************************************************
  */
#include "include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay = 0;
static __IO uint32_t TimingCount = 0;    //��ʱ����
static __IO uint32_t TimingCurrent = 0;  //ϵͳ�������е�ʱ���ۼ�(ms),�ɼ���1193��Сʱ����
__IO uint8_t TimingCountFlag = 0;        //��ʼ��ʱ��־

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ϵͳ��ʱ���жϵ���
  * @param  None
  * @retval None
  */
void Systick_Time_IRQ(void)
{
//  TimingCurrent_Insr();
  TimingCurrent++;                               //�Ż���ʡ��������ʱ��
  TimingDelay_Decrement();
  if(TimingCountFlag)
  {
    TimingCount_Insr();
  }
}

/**
  * @brief  ϵͳ����ʱ��ͳ��,��������TimingCurrent.
  * @param  None
  * @retval None
  */
void TimingCurrent_Insr(void)
{
   TimingCurrent++;
}

/**
  * @brief  ��ȡϵͳ��ǰ����ʱ��
  * @param  None
  * @retval None
  */
/*
uint32_t Timer_Val(void)
{
   return TimingCurrent;
}
*/

/**
  * @brief  ����һ���ӳ�ʱ�䡣
  * @param  nTime: ָ�����ӳ�ʱ��ĳ��ȣ��Ժ���Ϊ��λ��
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  SysTick �жϴ������,�ݼ�TimingDelay������
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
/**
  * @brief  ��ʼ��ʱ��
  * @param  None
  * @retval None
  */
void StartTimingCount(void)
{
  TimingCount = 0;
  TimingCountFlag = 1;
}
/**
  * @brief  ֹͣ��ʱ��
  * @param  None
  * @retval �����ۼƺ�����
  */
uint32_t StopTimingCount(void)
{
  TimingCountFlag = 0;
  return TimingCount;
}

/**
  * @brief  SysTick �жϴ������,����TimingCount������
  * @nots   TimingCount���ֵΪ0xFFFFFFFF,���Լ�ʱ�����ֵΪ1193Сʱ.
  * @param  None
  * @retval None
  */
void TimingCount_Insr(void)
{
  if (TimingCount != 0xFFFFFFFF)
  { 
    TimingCount++;
  }
}

/**
  * @brief  ʱ��Ӳ�ӳ� 
  * @param  TimeOut:ms���
  * @retval None
  */
void Timer_Delay_ms(u32 TimeOut)
{
  u32 tmp = 0;
  tmp = TimingCurrent;
  while((TimingCurrent - tmp) < TimeOut)
  {
  }
}

/**
  * @brief  ʱ��Ӳ�ӳ� 
  * @param  TimeOut:s���
  * @retval None
  */
void Timer_Delay_sec(u32 TimeOut)
{
  u32 tmp = 0;
  tmp = TimingCurrent;
  while((TimingCurrent - tmp) < (TimeOut*SYSTICK_1SECOND))
  {
  }
}

/**
  * @brief  �������������� 
  * @nots   x���ֵΪ0xFFFFFFFF,�����ӳٵ����ֵΪ1193Сʱ.
  * @param  None
  * @retval None
  */
void SpaceTime_Start(u32 *currTime)
{
  *currTime = TimingCurrent;
}

/**
  * @brief  �����ӳټ�� 
  * @param  currTime:����Ŀ�ʼʱ��
  * @param  TimeOut:����������λ����
  * @retval �����������1�����򷵻�0
  */
u8 SpaceTime_Ms(u32 currTime,u32 TimeOut)
{
  if((TimingCurrent - currTime) > TimeOut)
  {
    return 1;
  }else{
    return 0;
  }
}

/**
  * @brief  �����ӳټ�� 
  * @param  currTime:����Ŀ�ʼʱ��
  * @param  TimeOut:����������λ��
  * @retval �����������1�����򷵻�0
  */
u8 SpaceTime_Sec(u32 currTime,u32 TimeOut)
{
  if((TimingCurrent - currTime) > (TimeOut*SYSTICK_1SECOND))
  {
    return 1;
  }else{
    return 0;
  }
}