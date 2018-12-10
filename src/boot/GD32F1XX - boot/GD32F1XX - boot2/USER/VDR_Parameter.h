#ifndef __VDR_PARAMETER_H
#define __VDR_PARAMETER_H

#include "stm32f10x.h"

/**************************************************************************
//函数名：VDRParameter_Write
//功能：保存参数修改记录
//输入：EventTime:事件修改时间;EventType:事件类型
//输出：无
//返回值：无
//备注：协议解析函数VDRProtocol_ParseCmd需调用该接口
***************************************************************************/
void VDRParameter_Write(u32 EventTime, u8 EventType);

#endif

