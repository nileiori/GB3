#ifndef __VDR_SPEED_STATUS_H
#define __VDR_SPEED_STATUS_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"

#define VDR_SPEED_STATUS_BUFFER_SIZE		VDR_DATA_SPEED_STEP_LEN



/**************************************************************************
//函数名：VDRSpeedStatus_TimeTask
//功能：速度状态记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRSpeedStatus_TimeTask(void);
/**************************************************************************
//函数名：VDRSpeedStatus_GetFlag
//功能：获取速度状态标志
//输入：无
//输出：无
//返回值：0:正常;1:异常
//备注：
***************************************************************************/
u8 VDRSpeedStatus_GetFlag(void);

#endif
