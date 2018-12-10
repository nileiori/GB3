
#include "stm32f10x.h"
#include "rtcformat.h"
#ifndef __POSITION__
#define __POSITION__

/**************************外部引用***********************************************/
/*******************************************************************************
* Function Name  : Position_TimeTask
* Description    : 位置信息采集事件处理,每1min调用一次 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState Position_TimeTask(void);  
/*******************************************************************************
* Function Name  : Position_ParamInitialize
* Description    : 位置信息采集事件处理,每1min调用一次 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Position_ParamInitialize(void);
/*************************************************************
** 函数名称: Position_ReadBackPositionRecordData
** 功能描述: 从铁电里读取指定时间段速度记录
** 入口参数: 起始时间startTime
** 出口参数: pBuffer保存数据的首地址
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned short Position_ReadBackPositionRecordData(unsigned char *pBuffer,unsigned long startTime);
/*************************************************************
** 函数名称: Position_ReadCurPositionRecordData
** 功能描述: 从铁电里读取指定时间段速度记录
** 入口参数: 起始时间StartTime,结束时间EndTime
** 出口参数: pBuffer保存数据的首地址
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned short Position_ReadCurPositionRecordData(unsigned char *pBuffer,TIME_T StartTime,TIME_T EndTime);
/*************************************************************
** 函数名称: Position_ReadPositionRecordData
** 功能描述: 读取指定时间段、指定最大数据块速度记录数据
** 入口参数: 起始时间StartTime,结束时间EndTime,MaxBlock指定最大数据块
** 出口参数: pBuffer保存数据的首地址
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned short Position_ReadPositionRecordData(u8 *pBuffer, TIME_T StartTime, TIME_T EndTime, u16 MaxBlock);

#endif
