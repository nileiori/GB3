/********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS 
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "taskschedule.h"
#include "SysTick.h"
#include "VDR.h"
#include "usb_app.h"

/* extern function -----------------------------------------------------------*/
void EvNopHandleTask(void);
FunctionalState TimerNopHandleTask(void);

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIME_TASK MyTimerTask[MAX_TIMETASK] = {             
				{ENABLE, 	0, 1, Usb_AppTimeTask},  					//usb
};
EVEN_TASK MyEvTask[MAX_EVENTASK] = {                                 
        {DISABLE, EvNopHandleTask},     		
};
                                               
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : InitTimerTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTimerTask(void)
{
  //u8 i;
  /*
  for(i = 1; i < MAX_TIMETASK; i++)
  {
    MyTimerTask[i].TaskTimerState = DISABLE;
  }
  */
  //增加初始化函数,并且增加执行函数时间  
}

/*******************************************************************************
* Function Name  : SetTimerTask
* Description    : Setup timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTimerTask(TIMERTASK task, u32 time)
{
  MyTimerTask[task].TaskTimerState = ENABLE;
  MyTimerTask[task].TaskExTimer = time;
  MyTimerTask[task].TaskTimerCount = time + SysTick_GetCounter();
}

/*******************************************************************************
* Function Name  : ClrTimerTask
* Description    : Cancel timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrTimerTask(TIMERTASK task)
{
  MyTimerTask[task].TaskTimerState = DISABLE;
}
/*******************************************************************************
* Function Name  : TimerTaskScheduler
* Description    : Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerTaskScheduler(void)
{
  u8 i;
  u32 nowtimer;
  FunctionalState validflag;
  
  nowtimer = SysTick_GetCounter();
  for(i = 0; i < MAX_TIMETASK; i++)
  {
     if(MyTimerTask[i].TaskTimerState != ENABLE)
     {
         continue;
     }

    if(nowtimer >= MyTimerTask[i].TaskTimerCount)
    {
        MyTimerTask[i].TaskTimerCount = nowtimer + MyTimerTask[i].TaskExTimer;
        validflag = MyTimerTask[i].operate();
        MyTimerTask[i].TaskTimerState = validflag;
    }
  }
}


/*******************************************************************************
* Function Name  : InitEvTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitEvTask(void)
{
  u8 i;
  
  for(i = 0; i < MAX_EVENTASK; i++)
  {
    MyEvTask[i].evTaskOnState = DISABLE;
  }
  //增加初始化函数
}

/*******************************************************************************
* Function Name  : SetEvTask
* Description    : Setup even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetEvTask(u8 ev)
{
  MyEvTask[ev].evTaskOnState = ENABLE;
}

/*******************************************************************************
* Function Name  : EvTaskScheduler
* Description    : Schedule even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvTaskScheduler(void)
{
  u8 i;

  for(i = 0; i < MAX_EVENTASK; i++)
  {
    if(MyEvTask[i].evTaskOnState != ENABLE)
      continue;
    MyEvTask[i].evTaskOnState = DISABLE;
    MyEvTask[i].operate();
   
  }
} 
/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : 空事件处理 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState TimerNopHandleTask(void)
{
  
  return DISABLE;
}

/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : 空事件处理 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvNopHandleTask(void)
{

}
