/*******************************************************************************
 * File Name:			log_power.c 
 * Function Describe:	
 * Relate Module:		判断、存储、通、断电记录
 * Writer:				Joneming
 * Date:				2015-04-01
 * ReWriter:			
 * Date:
 *******************************************************************************/
#include "stm32f10x.h"
#ifndef __LOG__POWER__
#define __LOG__POWER__
/*******************************************************************************
* Function Name  : Power_ParamInitialize
* Description    : 通断电检查初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Power_ParamInitialize(void);
/*******************************************************************************
* Function Name  : Power_GetLowPowerFlag
* Description    : 
* Input          : None
* Output         : None
* Return         :  断电为2,通电为1,0未知
*******************************************************************************/
unsigned char Power_GetLowPowerFlag(void);
/*************************************************************
** 函数名称: CheckMainPowerRun_TimeTask
** 功能描述: 实时检测，记录断电函数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
FunctionalState CheckMainPowerRun_TimeTask(void);

#endif

/*******************************************************************************
 *                            End of Module
 *******************************************************************************/

