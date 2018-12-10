#ifndef __VDR_POSITION_H
#define __VDR_POSITION_H

#include "stm32f10x.h"

/**************************************************************************
//函数名：VDRPosition_TimeTask
//功能：360h位置信息记录，每分钟1条位置信息，按小时记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRPosition_TimeTask(void);

#endif

