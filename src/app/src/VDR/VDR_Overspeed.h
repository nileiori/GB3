#ifndef __VDR_OVERSPEED_H
#define __VDR_OVERSPEED_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include "rtcformat.h"

typedef struct
{
	u8	EarlyAlramSpeed;//预警速度，单位km/h，对应参数ID 0x005B，国标检测时设置为50即可。
	u32	EarlyAlramKeeptime;//预警持续时间，单位秒，对应参数ID 0xF250，国标检测时设置为0即可。
	u8	EarlyAlarmVoiceBuffer[30];//预警语音，对应参数ID 0xF251，
	u32	EarlyAlarmVoiceGroupTime;//预警语音每组提示间隔，单位分钟，对应参数ID 0xF252，国标检测时设置为5即可。
	u32	EarlyAlarmVoiceNum;//预警语音每组提示次数，对应参数ID 0xF253，国标检测时设置为3即可。
	u32	EarlyAlarmVoiceNumTime;//预警语音每次提示间隔，单位秒，对应参数ID 0xF254

	u8	AlarmSpeed;//报警速度，单位km/h，对应参数ID 0x0055
	u32	AlarmKeeptime;//报警持续时间，单位秒，对应参数ID 0x0056，国标检测时设置为0即可。
	u8	AlarmVoiceBuffer[30];//报警语音，对应参数ID 0xF255
	u32	AlarmVoiceGroupTime;//报警语音每组提示间隔，单位分钟，对应参数ID 0xF256，国标检测时设置为5即可。
	u32	AlarmVoiceNum;//报警语音每组提示次数，对应参数ID 0xF257，国标检测时设置为3即可。
	u32	AlarmVoiceNumTime;//报警语音每次提示间隔，单位秒，对应参数ID 0xF258
	
	TIME_T  NightStartTime;//夜间开始时间
	TIME_T  NightEndTime;//夜间结束时间
	u16     NightTimeAttribute;//夜间时间属性
	u32     NightSpeedPercent;//夜间速度百分比，默认值为80
	
	
}VDR_OVERSPEED_STRUCT;

typedef struct
{
  u8 Valid;//有效标志，1为区域超速有效，0为区域超速无效
	u8 MaxSpeed;//最大限速值
	u8 EarlyMaxSpeed;//最大预警值
	u8 KeepTime;//持续时间
	u8 EarlyKeepTime;
	u32 Id;//区域ID
}AREA_ROAD_OVERSPEED_STRUCT;


/**************************************************************************
//函数名：VDROverspeed_TimeTask
//功能：超速报警或预警
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDROverspeed_TimeTask(void);
/**************************************************************************
//函数名：VDROverspeed_UpdateParameter
//功能：更新参数
//输入：无
//输出：无
//返回值：无
//备注：主要是更新相关变量
***************************************************************************/
void VDROverspeed_UpdateParameter(void);
/**************************************************************************
//函数名：VDROverspeed_SetAreaParameter
//功能：设置区域超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_SetAreaParameter(u8 Type, u32 Id, u8 MaxSpeed, u8 KeepTime);
/**************************************************************************
//函数名：VDROverspeed_ResetAreaParameter
//功能：清除区域超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_ResetAreaParameter(u8 Type);
/**************************************************************************
//函数名：VDROverspeed_SetRoadParameter
//功能：设置线路超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_SetRoadParameter(u8 Type, u32 Id, u8 MaxSpeed, u8 KeepTime);
/**************************************************************************
//函数名：VDROverspeed_ResetRoadParameter
//功能：清除线路超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_ResetRoadParameter(u8 Type);
/**************************************************************************
//函数名：VDROverspeed_ReadAlarmFlag
//功能：读超速报警标志字节
//输入：无
//输出：无
//返回值：无
//备注：bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，
//bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
//bit6,bit7保留
***************************************************************************/
u8 VDROverspeed_ReadAlarmFlag(void);
/**************************************************************************
//函数名：VDROverspeed_GetAlarmSpeed
//功能：获取限速值
//输入：无
//输出：无
//返回值：限速值
//备注：无
***************************************************************************/
u8 VDROverspeed_GetAlarmSpeed(void);
/**************************************************************************
//函数名：VDROverspeed_ClearTime
//功能：超速持续时间清0
//输入：无
//输出：无
//返回值：无
//备注：部标协议检测超速时，设置超速限速值时需调用此函数
***************************************************************************/
void VDROverspeed_ClearTime(void);
#endif


