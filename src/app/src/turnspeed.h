/*******************************************************************************
 * File Name:			turnspeed.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2016-06-06
 * Rewriter:
 * Date:		
 *******************************************************************************/
#ifndef __TURNSPEED__H__
#define __TURNSPEED__H__

#include "stm32f10x.h"
#include "LzmSys.h"
/*******************************************************************************
** 函数名称: TurnSpeed_GetSubjoinInfoCurTurnSpeedVal
** 功能描述: 取得当前转速值附加信息包(包括附加信息ID、长度、附加信息体)
** 入口参数: 需要保存转速值附加信息包的首地址
** 出口参数: 
** 返回参数: 转速值附加信息包的总长度
*******************************************************************************/
unsigned char TurnSpeed_GetSubjoinInfoCurTurnSpeedVal(unsigned char *data);
/*************************************************************
** 函数名称: TurnSpeed_UpdatePram
** 功能描述: 
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void TurnSpeed_UpdatePram(void);
/*******************************************************************************
** 函数名称:TurnSpeed_GetCurTurnSpeed
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
unsigned short TurnSpeed_GetCurTurnSpeed(void);
/*******************************************************************************
** 函数名称:TurnSpeed_InsertCurPulseVal
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
void TurnSpeed_InsertCurPulseVal(float val);
/*********************************************************************
//函数名称	:TurnSpeed_TimeTask
//功能		:定时任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:1秒调用1次
*********************************************************************/
FunctionalState TurnSpeed_TimeTask(void);
/*************************************************************
** 函数名称: TaskSystemTurnSpeedPulseNum
** 功能描述: 转速脉冲个数标定
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
LZM_RET TaskSystemTurnSpeedPulseNum(LZM_MESSAGE *Msg);
#endif//__DEEPPLOW_H_
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

