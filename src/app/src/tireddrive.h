/*******************************************************************************
 * File Name:			TexiService.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2011-09-08
 * Rewriter:
 * Date:		
 *******************************************************************************/
#ifndef __TIRED_DRIVE_H
#define __TIRED_DRIVE_H

//*********************头文件********************************
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "rtc.h"
////////////////////////////////////
#define ONE_TIRED_DRIVER_DATA_LEN       REGISTER_OVER_TIME_COLLECT_LEN //一条超时驾驶数据长度
#define SAVE_ONE_TIRED_DRIVER_DATA_LEN  (REGISTER_OVER_TIME_STEP_LEN-5) //保存一条超时驾驶数据长度(包括保留字节)
//***********************函数声明********************************
/*************************************************************
** 函数名称: TiredDrive_TimeTask
** 功能描述: 超时驾驶模块任务,1s调度一次
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
FunctionalState TiredDrive_TimeTask(void);
/*************************************************************
** 函数名称: TiredDrive_ParamInitialize
** 功能描述: 超时驾驶参数初始化
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void TiredDrive_ParamInitialize(void);
/*************************************************************
** 函数名称: TiredDrive_UpdatePram
** 功能描述: 设置超时驾驶参数时，初始化超时驾驶变量
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void TiredDrive_UpdatePram(void);
/*************************************************************
** 函数名称: TiredDrive_Login
** 功能描述: 驾驶员登录时调用(初始化相关变量)
** 入口参数: 
** 出口参数: 无
** 返回参数: 1:登录成功,0:登录失败
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char TiredDrive_Login(void);
/*************************************************************
** 函数名称: TiredDrive_Logout
** 功能描述: :驾驶员签退时调用
** 入口参数: 
** 出口参数: 无
** 返回参数: 1:签退成功,0:签退失败
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char TiredDrive_Logout(void);
/*************************************************************
** 函数名称: TiredDrive_GetTimeAbnormalflag
** 功能描述: 取得时间异常标志
** 入口参数: 
** 出口参数: 
** 返回参数: 0:未异常,1异常
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_GetTimeAbnormalflag(void);
/*************************************************************
** 函数名称: TiredDrive_CheckTimeIsNotNight
** 功能描述: 判断时间是否为晚上时间段
** 入口参数: checkTime:需要判断的时间,nightStartVal:晚上开始时间点,nightEndVal:晚上结束时间点,
** 出口参数: 
** 返回参数: 1为晚上时间段,0不是晚上时间段
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_CheckTimeIsNotNight(unsigned long checkTime,unsigned long nightStartVal,unsigned long nightEndVal);
/*************************************************************
** 函数名称: TiredDrive_ReadTiredDriveFromFram
** 功能描述: 从铁电里读取超时驾驶记录(所有驾驶员)
** 入口参数: 
** 出口参数: 保存数据的首地址
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned short TiredDrive_ReadTiredDriveFromFram(unsigned char *pBuffer);
/*************************************************************
** 函数名称: TiredDrive_ReadTiredDriveFromFramEx
** 功能描述: 从铁电里读取指定时间段超时驾驶记录(所有驾驶员)
** 入口参数: 起始时间StartTime,结束时间EndTime
** 出口参数: pBuffer保存数据的首地址
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned short TiredDrive_ReadTiredDriveFromFramEx(unsigned char *pBuffer,TIME_T StartTime, TIME_T EndTime);
/*************************************************************
** 函数名称: TiredDrive_GetLoginFlag
** 功能描述: 取得驾驶员登录标志
** 入口参数: 
** 出口参数: 
** 返回参数: 0:未登录,1已登录
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_GetLoginFlag(void);
/*************************************************************
** 函数名称: TiredDrive_ReadTiredRecord
** 功能描述: 查找指定时间段的超时驾驶记录
** 入口参数: 起始时间StartTime,结束时间EndTime
** 出口参数: 
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块: Register_Read
*************************************************************/
unsigned short TiredDrive_ReadTiredRecord(unsigned char *pBuffer,TIME_T StartTime,TIME_T EndTime,u16 MaxBlock);
/*************************************************************
** 函数名称: TiredDrive_Read2daysTiredRecord
** 功能描述: 指定时间点开始两个日历天的超时驾驶记录
** 入口参数: 起始时间StartTime
** 出口参数: 
** 返回参数: pBuffer字节长度
** 全局变量: 无
** 调用模块: Register_Read
*************************************************************/
unsigned short TiredDrive_Read2daysTiredRecord(unsigned char *pBuffer,TIME_T StartTime);
/*************************************************************
** 函数名称: TiredDrive_ClearAllDriverRunInfo
** 功能描述: 清空当前暂存在铁电中所有驾驶员的相关驾驶信息(已保存到flash的数据除外)
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
void TiredDrive_ClearCurAllDriverRunInfo(void);
/*************************************************************
** 函数名称: TiredDrive_GetCurRunStatus
** 功能描述: 获取超时驾驶当前车辆的状态(行驶或停止)
** 入口参数: 
** 出口参数: 
** 返回参数: 1为行驶，0为停止
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_GetCurRunStatus(void); 
/*************************************************************
** 函数名称: TiredDrive_GetCurContDriveTime
** 功能描述: 取得当前连续驾驶时间(单位分钟)
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned long TiredDrive_GetCurContDriveTime(void);
/*************************************************************
** 函数名称: TiredDrive_GetCurContDriveTimeSec
** 功能描述: 取得当前连续驾驶时间(单位秒)
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned long TiredDrive_GetCurContDriveTimeSec(void);
/*************************************************************
** 函数名称: TiredDrive_GetCurOverDriveTime
** 功能描述: 取得当前超过疲劳驾驶设定值的多少秒(单位:秒)
** 入口参数: 
** 出口参数: 
** 返回参数: 0:表示未超时;非零:表示超过设定值多少秒
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned long TiredDrive_GetCurOverDriveTime(void);
/*************************************************************
** 函数名称: TiredDrive_GetCurdriverLicence
** 功能描述: 获取当前驾驶员的驾驶证号码(18个字节)
** 入口参数: 
** 出口参数: 保存数据的首地址
** 返回参数: 驾驶证号码长度
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_GetCurdriverLicence(unsigned char *buffer);
/*************************************************************
** 函数名称: TiredDrive_GetDriverLastRunTime
** 功能描述: 取得保存在电铁里的最后行驶时间(用于重启时读取)
** 入口参数: 
** 出口参数: 无
** 返回参数: 最后行驶时间
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned long TiredDrive_GetDriverLastRunTime(void);
/*************************************************************
** 函数名称: TiredDrive_GetTiredStatusForReport
** 功能描述: 获取根据超时驾驶状态来上报位置信息
** 入口参数: 
** 出口参数: 
** 返回参数: 0:不根据超时驾驶状态来上报, 1:根据超时驾驶状态来上报
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_GetTiredStatusForReport(void);
/*************************************************************
** 函数名称: TiredDrive_GetNeedReportFlag
** 功能描述: 获取是否需要上报位置信息标志
** 入口参数: 
** 出口参数: 
** 返回参数: 0:不需要上报,1:需要上报
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char TiredDrive_GetNeedReportFlag(void);
/*************************************************************
** 函数名称: TiredDrive_TiredDriverInfoInit
** 功能描述: 超时驾驶相关驾驶员信息初始化
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void TiredDrive_TiredDriverInfoInit(void);
#endif//__TIRED_DRIVE_H
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

