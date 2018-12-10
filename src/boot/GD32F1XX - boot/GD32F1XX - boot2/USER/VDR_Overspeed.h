#ifndef __VDR_OVERSPEED_H
#define __VDR_OVERSPEED_H

#include "stm32f10x.h"

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
}VDR_OVERSPEED_STRUCT;

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


#endif


