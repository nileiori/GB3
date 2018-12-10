/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : timer.c
* Author             : MCD Application Team 
* Version            : V1.0.0
* Date               : 04/07/2009
* Description        : 定时器程序.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
//*******************外部变量************************


//*******************全局变量************************
u32  TimerCount = 0;


/*******************************************************************************
* Function Name  : Timer_Val
* Description    : This function .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u32 Timer_Val()
{
  return TimerCount; 
}

/*******************************************************************************
* Function Name  : Timer_ISR
* Description    : This function .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer_ISR()
{
  TimerCount++; 
}





