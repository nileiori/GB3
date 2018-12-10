#ifndef __VDR_OVERTIME_H
#define __VDR_OVERTIME_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include "Rtc.h"

#define	DRIVER_NUM_MAX											6//最多不超过6个驾驶员，序号为0-5，系统默认驾驶员为0
#define	DRIVER_INFO_TYPE_DRIVER_NUM					      0x0001
#define	DRIVER_INFO_TYPE_DRIVER_LICENSE			      0x0002
#define	DRIVER_INFO_TYPE_START_TIME					      0x0004
#define	DRIVER_INFO_TYPE_END_TIME						      0x0008
#define	DRIVER_INFO_TYPE_START_POSITION			      0x0010
#define	DRIVER_INFO_TYPE_END_POSITION				      0x0020
#define	DRIVER_INFO_TYPE_LOGIN_TIME					      0x0040
#define	DRIVER_INFO_TYPE_LOGOUT_TIME				      0x0080
#define	DRIVER_INFO_TYPE_NIGHT_START_TIME					0x0100
#define	DRIVER_INFO_TYPE_NIGHT_END_TIME						0x0200
#define	DRIVER_INFO_TYPE_NIGHT_START_POSITION			0x0400
#define	DRIVER_INFO_TYPE_NIGHT_END_POSITION				0x0800
#define	DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME				0x1000

#define	DRIVER_INFO_TYPE_ALL								0xFFFF 

typedef struct
{
	u8	License[18];//驾驶证号，所有字节为0xff表示未登记
	u32	StartTimeCount;//超时驾驶开始时间，值为0表示未写入过
	u32	EndTimeCount;//超时驾驶结束时间，所有字节为0xff表示未写入过
	u8	StartPosition[10];//超时驾驶开始位置，所有字节为0xff或0x00表示未写入过
	u8	EndPosition[10];//超时驾驶开始位置
	u32	LoginTimeCount;//签到时间
	u32	LogoutTimeCount;//签退时间
	u32	NightStartTimeCount;//夜间超时驾驶开始时间，值为0表示未写入过
	u32	NightEndTimeCount;//夜间超时驾驶结束时间，所有字节为0xff表示未写入过
	u8	NightStartPosition[10];//夜间超时驾驶开始位置，所有字节为0xff或0x00表示未写入过
	u8	NightEndPosition[10];//夜间超时驾驶开始位置	
	u32 OneDayDriveTimeCount;//每日驾驶总时间，0点0时0秒清0

}VDR_OVERTIME_DRIVER_STRUCT;

typedef struct
{
	u32	EarlyAlarmTime;//预警时间，单位秒，对应（参数ID 0x0057值 - 参数ID 0x005c值），国标检测时0x0057设置为14400，0x005c设置为1800即可。
	u32	EarlyAlarmKeeptime;//预警持续时间，单位秒，对应参数ID 0x005c，国标检测时设置为1800即可。
		//预警语音，使用局部变量，对应参数ID 0xF26e，
	u32	EarlyAlarmVoiceGroupTime;//预警语音每组提示间隔，单位分钟，对应参数ID 0xF26f，国标检测时设置为5即可。
	u32	EarlyAlarmVoiceNum;//预警语音每组提示次数，对应参数ID 0xF270，国标检测时设置为3即可。
	u32	EarlyAlarmVoiceNumTime;//预警语音每次提示间隔，单位秒，对应参数ID 0xF271

	u32	AlarmTime;//报警时间，单位秒，对应参数ID 0x0057
	u32	AlarmVoiceKeeptime;//报警语音持续时间，单位分钟，对应参数ID 0xF26D，国标检测时设置为30即可。
		//报警语音，使用局部变量，对应参数ID 0xF272
	u32	AlarmVoiceGroupTime;//报警语音每组提示间隔，单位分钟，对应参数ID 0xF273，国标检测时设置为5即可。
	u32	AlarmVoiceNum;//报警语音每组提示次数，对应参数ID 0xF274，国标检测时设置为3即可。
	u32	AlarmVoiceNumTime;//报警语音每次提示间隔，单位秒，对应参数ID 0xF275

	u32	MinRestTime;//最小休息时间，单位秒，对应参数ID 0x0059，国标检测时设置为1200即可。
        
        u8      RunSpeed;//行驶判定速度，单位为km/h，国标检测时设置为0即可
        u32     RunKeeptime;//行驶判定持续时间，单位为秒，国标检测时设置为10即可

	u32	NightEarlyAlarmTime;//夜间预警时间，单位秒，对应（参数ID 0xF26B值 - 参数ID 0x005c值），国标检测时不开启夜间功能即可。
	u32	NightAlarmTime;//夜间报警时间，单位秒，对应参数ID 0xF26B
	u32	NightMinRestTime;//夜间最小休息时间，单位秒，对应参数ID 0xF26C，国标检测时不开启夜间功能即可。
	//u8  NightTime[14];//夜间时间，共14字节，1-2字节是属性，3-8字节是开始时间，9-14字节是结束时间
	TIME_T NightStartTime;//夜间开始时间
	TIME_T NightEndTime;//夜间结束时间
	u16    NightTimeAttribute;//夜间时间属性
	
	u32	NightVoiceKeeptime;//夜间语音持续时间，单位分钟，对应参数ID 0xF25C
	u32	NightVoiceGroupTime;//夜间语音每次提示间隔，单位分钟，对应参数ID 0xF25E
	
	u32 OneDayAlarmTime;//当天累计驾驶时间，单位秒，对应参数ID 0x0058

	VDR_OVERTIME_DRIVER_STRUCT Driver[DRIVER_NUM_MAX];//包含默认驾驶员在内，最多支持6个驾驶员，序号分别为0-5，默认的驾驶员序号为0
}VDR_OVERTIME_STRUCT;

/**************************************************************************
//函数名：VDROvertime_TimeTask
//功能：超时驾驶
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：超时驾驶定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDROvertime_TimeTask(void);
/**************************************************************************
//函数名：VDROvertime_UpdateParameter
//功能：更新超时驾驶相关参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROvertime_UpdateParameter(void);
/**************************************************************************
//函数名：VDROvertime_DriverLogin
//功能：驾驶员签到（登录）
//输入：pBuffer:指向驾驶证号码的指针;BufferLen:数据长度
//输出：无
//返回值：0:登录成功；非0:登录失败，为1表示长度错误，为2表示驾驶证号码格式错误
//为3表示驾驶证号全为ASCII 0（即禁止系统默认驾驶员签到），为4表示该驾驶员已签到
//备注：驾驶证号码长度BufferLen必须为18字节，否则登录失败
***************************************************************************/
u8 VDROvertime_DriverLogin(u8 *pBuffer, u8 BufferLen);
/**************************************************************************
//函数名：VDROvertime_DriverLogout
//功能：驾驶员签退（未登录）
//输入：pBuffer:指向驾驶证号码的指针;BufferLen:数据长度
//输出：无
//返回值：0:登录成功；非0:登录失败，为1表示长度错误，为2表示驾驶证号码格式错误
//为3表示驾驶证号全为ASCII 0（即禁止系统默认驾驶员签到），为4表示该驾驶员已签到
//备注：驾驶证号码长度BufferLen必须为18字节，否则登录失败
***************************************************************************/
void VDROvertime_DriverLogout(void);
/**************************************************************************
//函数名：VDROvertime_GetLoginFlag
//功能：获取登录标志
//输入：无
//输出：登录标志
//返回值：1:登录；0:未登录
//备注：无
***************************************************************************/
u8 VDROvertime_GetLoginFlag(void);
/**************************************************************************
//函数名：VDROvertime_Get2daysLog
//功能：获取2个日历天的超时驾驶记录
//输入：无
//输出：
//返回值：数据长度
//备注：
***************************************************************************/
u16 VDROvertime_Get2daysLog(u8 *pBuffer);
/**************************************************************************
//函数名：VDROvertime_GetUnsaveLog
//功能：获取还未保存到flash的超时驾驶记录
//输入：指定时间
//输出：
//返回值：数据长度
//备注：从RAM或铁电中读取还未写入到FLASH中的超时驾驶记录
***************************************************************************/
u16 VDROvertime_GetUnsaveLog(u8 *pBuffer, TIME_T Time, u8 DriverNum);
/**************************************************************************
//函数名：VDROvertime_GetInitFlag
//功能：获得模块初始化标志
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
u8 VDROvertime_GetInitFlag(void);
/**************************************************************************
//函数名：VDROvertime_GetLicense
//功能：获取当前驾驶证号码
//输入：指向目的缓冲指针
//输出：
//返回值：数据长度
//备注：
***************************************************************************/
u8 VDROvertime_GetLicense(u8 *pBuffer);
/**************************************************************************
//函数名：VDROvertime_GetDriveTime
//功能：获取当前驾驶员连续驾驶时间
//输入：无
//输出：
//返回值：连续驾驶时间，单位分钟
//备注：
***************************************************************************/
u32 VDROvertime_GetDriveTime(void);
/**************************************************************************
//函数名：VDROvertime_ClearStartTime
//功能：清除开始时间，重新获取开始时间
//输入：无
//输出：无
//返回值：无
//备注：部标协议检测疲劳驾驶时，当设置疲劳驾驶参数时需要调用此函数
***************************************************************************/
void VDROvertime_ClearStartTime(void);
#endif
