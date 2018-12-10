#ifndef __VDR_CARD_H
#define __VDR_CARD_H

#include "stm32f10x.h"
#include "VDR.h"

#define VDR_CARD_BUFFER_SIZE	150

#define CARD_STATE_MONITOR   	0
#define CARD_STATE_READ   		1
#define CARD_STATE_LOGIN   		2
#define CARD_STATE_LOGOUT   	3

#define CARD_READ_START   	0
#define CARD_READ_24XX   		1
#define CARD_READ_4442   		2
#define CARD_STATE_LOGOUT   	3

/**************************************************************************
//函数名：VDRCard_TimeTask
//功能：刷卡签到、拔卡签退
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：0.1秒钟进入1次，任务调度器需要调用此函数，
//注意该任务需要在超时驾驶定时任务完成初始化后才能开启
***************************************************************************/
FunctionalState VDRCard_TimeTask(void);

#endif
