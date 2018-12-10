/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_dma.h
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : This file contains all the functions prototypes for the
*                      DMA firmware library.
********************************************************************************

*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASKSCHEDULE_H
#define __TASKSCHEDULE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

//*****************事件任务**************************
typedef enum 
{
			EV_TASK_XXX = 0, //0:未知任务
    	MAX_EVENTASK

}EVENTTASK;

//******************时间任务***********************
typedef enum 
{
	TIME_TASK_USB,										//USB
  MAX_TIMETASK,   
}TIMERTASK;
/*------------------------ Execute task by timer--------------------------------
Max task is 32. when power on dev this task is init.
Include 
------------------------------------------------------------------------------*/
typedef struct{
  FunctionalState TaskTimerState;       //task valid,1:valid;0:invalid
  u32 TaskTimerCount;   		//variables task time count
  u32 TaskExTimer;      		//set fixed param by independently funtion module
  FunctionalState (* operate)();     //handle independtly funtion
}TIME_TASK;

typedef struct{
  FunctionalState evTaskOnState;       //task valid,1:valid;0:invalid
  void (*operate)();     //handle independtly funtion
}EVEN_TASK;

/* Private define ------------------------------------------------------------*/



/*******************************************************************************
* Function Name  : InitTimerTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTimerTask(void);

/*******************************************************************************
* Function Name  : TimerTaskScheduler
* Description    : Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerTaskScheduler(void);

/*******************************************************************************
* Function Name  : SetTimerTask
* Description    : Setup timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTimerTask(TIMERTASK task, u32 time);

/*******************************************************************************
* Function Name  : ClrTimerTask
* Description    : Cancel timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrTimerTask(TIMERTASK task);

/*******************************************************************************
* Function Name  : InitEvTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitEvTask(void);
/*******************************************************************************
* Function Name  : SetEvTask
* Description    : Setup even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetEvTask(u8 ev);
/*******************************************************************************
* Function Name  : EvTaskScheduler
* Description    : Schedule even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvTaskScheduler(void);

#endif
