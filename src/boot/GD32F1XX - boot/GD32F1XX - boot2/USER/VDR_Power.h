#ifndef __VDR_POWER_H
#define __VDR_POWER_H

#include "stm32f10x.h"

#define FRAM_VDR_POWER_OFF_TIME_ADDR	263//5字节，4字节时间，1字节校验码

/**************************************************************************
//函数名：VDRPower_TimeTask
//功能：上电或断电记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRPower_TimeTask(void);

#endif
