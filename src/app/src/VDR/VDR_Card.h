#ifndef __VDR_CARD_H
#define __VDR_CARD_H

//#include "stm32f2xx.h"
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
/**************************************************************************
//函数名：VDRCard_GetReportInfo
//功能：获得驾驶员签到上报信息
//输入：pBuffer:目标缓冲
//输出：
//返回值：0:表示未获取到数据
//备注：可兼容行标格式卡和国标格式卡
***************************************************************************/
u8 VDRCard_GetReportInfo(u8 *pBuffer);
/*************************************************************
** 函数名称: VDRCard_GetExternalDriverName
** 功能描述: 获取驾驶员姓名
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned char VDRCard_GetExternalDriverName(unsigned char *pBuffer);

#endif
