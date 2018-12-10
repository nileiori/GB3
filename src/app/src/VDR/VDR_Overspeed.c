/************************************************************************
//程序名称：VDR_Overspeed.c
//功能：超速报警
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：
//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "Public.h"
#include "Io.h"
#include "Report.h"
#include "modem_app_com.h"
#include "other.h"

#include <stdio.h>

/********************本地变量*************************/
static	AREA_ROAD_OVERSPEED_STRUCT	AreaRoadOverspeed[5];
static	VDR_OVERSPEED_STRUCT	VdrOverspeed;
static 	u8	VdrOverspeedAlarmFlag = 0;//超速报警标志，相应位为1表示产生了该报警，为0表示该报警清除了。
//在播报超速报警语音时若已有另外的一个bit置位了（已有超速报警），则不需要播报本次超速报警语音，避免重复播报。
//bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
static	u8	VdrOverspeedEarlyAlarmFlag = 0;//超速预警标志，相应位为1表示产生了该预警，为0表示该预警清除了。
//在播报超速预警语音时若已有另外的一个bit置位了（已有超速预警），则不需要播报本次超速预警语音，避免重复播报。
//bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速

static  u32 VdrOverspeedEarlyAlarmCount = 0;
static  u32 VdrOverspeedAlramCount = 0;
static  u8 VdrOverspeedAlarmSpeedOffset = 5;//超速预警差值
/********************全局变量*************************/


/********************外部变量*************************/
extern u8  BBXYTestFlag;//0为正常模式，1为部标协议检测模式

/********************本地函数声明*********************/
static u8 VDROverspeed_IsAlarm(void);
static u8 VDROverspeed_IsEarlyAlarm(void);
static void VDROverspeed_Init(void);
static u8 VDROverspeed_ReadEarlyAlarmFlag(void);
static void VDROverspeed_OnceOverReport(void);
static u8 VDROverspeed_NightIsArrived(void);
static u8 VDROverspeed_NightTimeIsOk(TIME_T Time);
/********************函数定义*************************/

/**************************************************************************
//函数名：VDROverspeed_TimeTask
//功能：超速报警或预警
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDROverspeed_TimeTask(void)
{
	static 	u8	InitFlag = 0;
	static 	u32	EarlyAlarmVoiceCount = 0;
	static 	u32	EarlyAlarmVoiceNumCount = 0;
	static	u32	EarlyAlarmVoiceNumTimeCount = 0; 
	static 	u32	AlarmVoiceCount = 0;
	static 	u32	AlarmVoiceNumCount = 0;
	static	u32	AlarmVoiceNumTimeCount = 0; 
	
	if(0 == InitFlag)
	{
		if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED))
		{
			VDROverspeed_Init();
			InitFlag = 1;
		}
	}
	else
	{
		
		VDROverspeed_OnceOverReport();//dxl,2016.3.21 部标协议检测时需添加这个函数调用，重点检测超速开始时刻和超速报警时刻
		
		if(1 == VDROverspeed_IsAlarm())//有超速报警
		{
			EarlyAlarmVoiceCount = 0;
			EarlyAlarmVoiceNumCount = 0;
			EarlyAlarmVoiceNumTimeCount = 0; 

			AlarmVoiceCount++;
			if(AlarmVoiceCount >= (VdrOverspeed.AlarmVoiceGroupTime*60))
			{
				AlarmVoiceCount = 0;
				AlarmVoiceNumCount = 0;
				AlarmVoiceNumTimeCount = 0; 
			}
			if(AlarmVoiceNumCount < VdrOverspeed.AlarmVoiceNum)
			{
				if(0 == (AlarmVoiceNumTimeCount%VdrOverspeed.AlarmVoiceNumTime))
				{
					AlarmVoiceNumCount++; 
					if(0x01 == VDROverspeed_ReadAlarmFlag())
					{
						Public_PlayTTSVoiceStr(VdrOverspeed.AlarmVoiceBuffer);
					}
					else
					{
						//有其它类型的超速报警，则不播报语音
					}
				}
				AlarmVoiceNumTimeCount++;
			}
			
		}
		else//无超速报警的情况下，在看有无超速预警。
		{
			AlarmVoiceCount = 0;
			AlarmVoiceNumCount = 0;
			AlarmVoiceNumTimeCount = 0; 

			if(1 == VDROverspeed_IsEarlyAlarm())//有超速预警
			{
				EarlyAlarmVoiceCount++;
				if(EarlyAlarmVoiceCount >= (VdrOverspeed.EarlyAlarmVoiceGroupTime*60))
				{
					EarlyAlarmVoiceCount = 0;
					EarlyAlarmVoiceNumCount = 0;
					EarlyAlarmVoiceNumTimeCount = 0; 
				}
				if(EarlyAlarmVoiceNumCount < VdrOverspeed.EarlyAlarmVoiceNum)
				{
					if(0 == (EarlyAlarmVoiceNumTimeCount%VdrOverspeed.EarlyAlarmVoiceNumTime))
					{
						EarlyAlarmVoiceNumCount++;
						if(0x01 == VDROverspeed_ReadEarlyAlarmFlag())
						{
							Public_PlayTTSVoiceStr(VdrOverspeed.EarlyAlarmVoiceBuffer);
						}
						else
						{
							//有其它类型的超速报警，则不播报语音
						}
					}
					EarlyAlarmVoiceNumTimeCount++; 
					
				}
			}
			else//无超速预警
			{

				EarlyAlarmVoiceCount = 0;
				EarlyAlarmVoiceNumCount = 0;
				EarlyAlarmVoiceNumTimeCount = 0; 
			}
		}
	}
	
	return ENABLE;		
}
/**************************************************************************
//函数名：VDROverspeed_UpdateParameter
//功能：更新参数
//输入：无
//输出：无
//返回值：无
//备注：主要是更新相关变量
***************************************************************************/
void VDROverspeed_UpdateParameter(void)
{
	u8	Buffer[40];
	u16	BufferLen;
	u16	temp;

	BufferLen = EepromPram_ReadPram(E2_MAX_SPEED_ID, Buffer);//超速报警限速值
	if(BufferLen == E2_MAX_SPEED_LEN)
	{
		VdrOverspeed.AlarmSpeed = Public_ConvertBufferToLong(Buffer);//一般前3字节均为0
		if(VdrOverspeed.AlarmSpeed > 220)
		{
		    VdrOverspeed.AlarmSpeed = 100;
		}
	}
	else
	{
		VdrOverspeed.AlarmSpeed = 100;
	}

	BufferLen = EepromPram_ReadPram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer);//超速报警持续时间
	if(BufferLen == E2_OVER_SPEED_KEEP_TIME_LEN)
	{
		VdrOverspeed.AlarmKeeptime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmKeeptime = 10;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_VOICE_ID, Buffer);//超速报警语音
	if(0 != BufferLen)
	{
		Buffer[BufferLen] = 0;
		strcpy((char *)VdrOverspeed.AlarmVoiceBuffer,(const char *)Buffer);
	}
	else
	{
		strcpy((char *)VdrOverspeed.AlarmVoiceBuffer,"您已超速，请减速行驶");
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_GPROUP_TIME_ID, Buffer);//超速报警每组语音提示时间间隔
	if(BufferLen == E2_OVERSPEED_ALARM_GPROUP_TIME_ID_LEN)
	{
		VdrOverspeed.AlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmVoiceGroupTime = 5;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_NUMBER_ID, Buffer);//超速报警每组语音提示次数
	if(BufferLen == E2_OVERSPEED_ALARM_NUMBER_ID_LEN)
	{
		VdrOverspeed.AlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmVoiceNum = 3;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_NUMBER_TIME_ID, Buffer);//超速报警每次语音提示时间间隔
	if(BufferLen == E2_OVERSPEED_ALARM_NUMBER_TIME_ID_LEN)
	{
		VdrOverspeed.AlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmVoiceNumTime = 10;
	}


	////////////////////////////////////////////////////////////
	BufferLen = EepromPram_ReadPram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer);//超速预警限速值
	if(BufferLen == E2_SPEED_EARLY_ALARM_DVALUE_LEN)
	{
		temp = Public_ConvertBufferToShort(Buffer);
		temp = temp/10;
		VdrOverspeedAlarmSpeedOffset = temp&0xff;
		if(VdrOverspeed.AlarmSpeed > temp)
		{
			VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - VdrOverspeedAlarmSpeedOffset;
		}
		else
		{
			VdrOverspeedAlarmSpeedOffset = 5;
		  VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - VdrOverspeedAlarmSpeedOffset;
		}
		
	}
	else
	{
		VdrOverspeedAlarmSpeedOffset = 5;
		VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - VdrOverspeedAlarmSpeedOffset;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_KEEPTIME_ID, Buffer);//超速预警持续时间
	if(BufferLen == E2_OVERSPEED_PREALARM_KEEPTIME_ID_LEN)
	{
		VdrOverspeed.EarlyAlramKeeptime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlramKeeptime = 3;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_VOICE_ID, Buffer);//超速预警语音
	if(0 != BufferLen)
	{
		Buffer[BufferLen] = 0;
		strcpy((char *)VdrOverspeed.EarlyAlarmVoiceBuffer,(const char *)Buffer);
	}
	else
	{
		strcpy((char *)VdrOverspeed.EarlyAlarmVoiceBuffer,"请控制车速");
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_GPROUP_TIME_ID, Buffer);//超速预警每组语音提示时间间隔
	if(BufferLen == E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_LEN)
	{
		VdrOverspeed.EarlyAlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlarmVoiceGroupTime = 5;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_NUMBER_ID, Buffer);//超速预警每组语音提示次数
	if(BufferLen == E2_OVERSPEED_PREALARM_NUMBER_ID_LEN)
	{
		VdrOverspeed.EarlyAlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlarmVoiceNum = 3;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_NUMBER_TIME_ID, Buffer);//超速预警每次语音提示时间间隔
	if(BufferLen == E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_LEN)
	{
		VdrOverspeed.EarlyAlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlarmVoiceNumTime = 10;
	}
	
	  BufferLen = EepromPram_ReadPram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer);//夜间时间，夜间超速使用
    if(E2_OVERSPEED_NIGHT_TIME_ID_LEN == BufferLen)
    {
			  VdrOverspeed.NightTimeAttribute = (Buffer[0] << 8)|Buffer[1];  
			
        VdrOverspeed.NightStartTime.year = VDRPub_BCD2HEX(Buffer[2]);
	      VdrOverspeed.NightStartTime.month = VDRPub_BCD2HEX(Buffer[3]);
	      VdrOverspeed.NightStartTime.day = VDRPub_BCD2HEX(Buffer[4]);
	      VdrOverspeed.NightStartTime.hour = VDRPub_BCD2HEX(Buffer[5]);
	      VdrOverspeed.NightStartTime.min = VDRPub_BCD2HEX(Buffer[6]);
	      VdrOverspeed.NightStartTime.sec = VDRPub_BCD2HEX(Buffer[7]);
	
	      VdrOverspeed.NightEndTime.year = VDRPub_BCD2HEX(Buffer[8]);
	      VdrOverspeed.NightEndTime.month = VDRPub_BCD2HEX(Buffer[9]);
	      VdrOverspeed.NightEndTime.day = VDRPub_BCD2HEX(Buffer[10]);
	      VdrOverspeed.NightEndTime.hour = VDRPub_BCD2HEX(Buffer[11]);
	      VdrOverspeed.NightEndTime.min = VDRPub_BCD2HEX(Buffer[12]);
	      VdrOverspeed.NightEndTime.sec = VDRPub_BCD2HEX(Buffer[13]);
    }
    else
    {
        VdrOverspeed.NightTimeAttribute = 0;  
			
        VdrOverspeed.NightStartTime.year = 0;
	      VdrOverspeed.NightStartTime.month = 0;
	      VdrOverspeed.NightStartTime.day = 0;
	      VdrOverspeed.NightStartTime.hour = 0;
	      VdrOverspeed.NightStartTime.min = 0;
	      VdrOverspeed.NightStartTime.sec = 0;
	
	      VdrOverspeed.NightEndTime.year = 0;
	      VdrOverspeed.NightEndTime.month = 0;
	      VdrOverspeed.NightEndTime.day = 0;
	      VdrOverspeed.NightEndTime.hour = 0;
	      VdrOverspeed.NightEndTime.min = 0;
	      VdrOverspeed.NightEndTime.sec = 0;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_OVERSPEED_PERCENT_ID, Buffer);//夜间超速报警及预警百分比
	  if(BufferLen == E2_NIGHT_OVERSPEED_PERCENT_ID_LEN)
	  {
		    VdrOverspeed.NightSpeedPercent = Public_ConvertBufferToLong(Buffer);
			  if(VdrOverspeed.NightSpeedPercent > 100)
				{
				    VdrOverspeed.NightSpeedPercent = 80;  
				}
	  }
	  else
	  {
		    VdrOverspeed.NightSpeedPercent = 80;
	  }
		
		
}
/**************************************************************************
//函数名：VDROverspeed_SetAreaParameter
//功能：设置区域超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_SetAreaParameter(u8 Type, u32 Id, u8 MaxSpeed, u8 KeepTime)
{
	if((0 == Type)||(Type > 3))
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 1;
	AreaRoadOverspeed[Type].Id = Id;
	AreaRoadOverspeed[Type].MaxSpeed = MaxSpeed;
	AreaRoadOverspeed[Type].KeepTime = KeepTime;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = MaxSpeed - VdrOverspeedAlarmSpeedOffset;
	AreaRoadOverspeed[Type].EarlyKeepTime = KeepTime;
}
/**************************************************************************
//函数名：VDROverspeed_ResetAreaParameter
//功能：清除区域超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_ResetAreaParameter(u8 Type)
{
	if((0 == Type)||(Type > 3))
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 0;
	AreaRoadOverspeed[Type].Id = 0;
	AreaRoadOverspeed[Type].MaxSpeed = 0;
	AreaRoadOverspeed[Type].KeepTime = 0;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = 0;
	AreaRoadOverspeed[Type].EarlyKeepTime = 0;
}
/**************************************************************************
//函数名：VDROverspeed_SetRoadParameter
//功能：设置线路超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_SetRoadParameter(u8 Type, u32 Id, u8 MaxSpeed, u8 KeepTime)
{
	if(4 != Type)
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 1;
	AreaRoadOverspeed[Type].Id = Id;
	AreaRoadOverspeed[Type].MaxSpeed = MaxSpeed;
	AreaRoadOverspeed[Type].KeepTime = KeepTime;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = MaxSpeed - VdrOverspeedAlarmSpeedOffset;
	AreaRoadOverspeed[Type].EarlyKeepTime = KeepTime;
}
/**************************************************************************
//函数名：VDROverspeed_ResetRoadParameter
//功能：清除线路超速相关参数
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROverspeed_ResetRoadParameter(u8 Type)
{
	if(4 != Type)
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 0;
	AreaRoadOverspeed[Type].Id = 0;
	AreaRoadOverspeed[Type].MaxSpeed = 0;
	AreaRoadOverspeed[Type].KeepTime = 0;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = 0;
	AreaRoadOverspeed[Type].EarlyKeepTime = 0;
}
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
u8 VDROverspeed_ReadAlarmFlag(void)
{
	return VdrOverspeedAlarmFlag;
}
/**************************************************************************
//函数名：VDROverspeed_ReadAlarmFlagBit
//功能：读超速报警标志的某个位
//输入：无
//输出：无
//返回值：正常返回1或者0，若返回0xff表示参数错误
//备注：bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，
//bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
//bit6,bit7保留
***************************************************************************/
u8 VDROverspeed_ReadAlarmFlagBit(u8 bit)
{
	u8 	value;

	if(bit > 5)
	{
		return 0xff;
	}

	value = (VdrOverspeedAlarmFlag & (1 << bit)) >> bit;

	return value;
}
/**************************************************************************
//函数名：VDROverspeed_WriteAlarmFlagBit
//功能：写超速报警标志的某个位
//输入：bit:0-7;value:0-1;
//输出：无
//返回值：正常返回1或者0，若返回0xff表示参数错误
//备注：bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，
//bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
//bit6,bit7保留
////已有某个类型的报警产生了，其它报警位写入将不成功。即同一时刻只能有一种超速报警。
***************************************************************************/
void VDROverspeed_WriteAlarmFlagBit(u8 bit, u8 value)
{
	if(bit > 5)
	{
		return ;
	}

	if(value > 1)
	{
		return ;
	}

	if(0 == value)
	{
		VdrOverspeedAlarmFlag &= !(1<<bit);

		if(VdrOverspeedAlarmFlag == 0)
		{
			//Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED,RESET); dxl,2016.5.6
			Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT_ORDINARY,RESET);
		}
	}
	else
	{
		if(0 == VdrOverspeedAlarmFlag)
		{
			VdrOverspeedAlarmFlag |= 1<<bit;
			//Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, SET);
			Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT_ORDINARY,SET);
		}
	}
}
/**************************************************************************
//函数名：VDROverspeed_ReadEarlyAlarmFlag
//功能：读超速预警标志字节
//输入：无
//输出：无
//返回值：无
//备注：bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，
//bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
//bit6,bit7保留
***************************************************************************/
static u8 VDROverspeed_ReadEarlyAlarmFlag(void)
{
	return VdrOverspeedEarlyAlarmFlag;
}
/**************************************************************************
//函数名：VDROverspeed_ReadEarlyAlarmFlagBit
//功能：读超速预警标志的某个位
//输入：无
//输出：无
//返回值：正常返回1或者0，若返回0xff表示参数错误
//备注：bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，
//bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
//bit6,bit7保留
***************************************************************************/
u8 VDROverspeed_ReadEarlyAlarmFlagBit(u8 bit)
{
	u8 	value;

	if(bit > 5)
	{
		return 0xff;
	}

	value = (VdrOverspeedEarlyAlarmFlag & (1 << bit)) >> bit;

	return value;
}
/**************************************************************************
//函数名：VDROverspeed_WriteEarlyAlarmFlagBit
//功能：写超速预警标志的某个位
//输入：bit:0-7;value:0-1;
//输出：无
//返回值：正常返回1或者0，若返回0xff表示参数错误
//备注：bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，
//bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
//bit6,bit7保留
////已有某个类型的报警产生了，其它报警位写入将不成功。即同一时刻只能有一种超速报警。
***************************************************************************/
void VDROverspeed_WriteEarlyAlarmFlagBit(u8 bit, u8 value)
{
	if(bit > 5)
	{
		return ;
	}

	if(value > 1)
	{
		return ;
	}

	if(0 == value)
	{
		VdrOverspeedEarlyAlarmFlag &= !(1<<bit);

		if(VdrOverspeedEarlyAlarmFlag == 0)
		{
			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, RESET);
		}
	}
	else
	{
		if(0 == VdrOverspeedEarlyAlarmFlag)
		{
			VdrOverspeedEarlyAlarmFlag |= 1<<bit;
			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, SET);
		}
	}
}
/**************************************************************************
//函数名：VDROverspeed_GetAlarmSpeed
//功能：获取限速值
//输入：无
//输出：无
//返回值：限速值
//备注：无
***************************************************************************/
u8 VDROverspeed_GetAlarmSpeed(void)
{
  return VdrOverspeed.AlarmSpeed;
}
/**************************************************************************
//函数名：VDROverspeed_IsAlarm
//功能：是否超速报警
//输入：无
//输出：无
//返回值：1:是;0:否
//备注：无
***************************************************************************/
static u8 VDROverspeed_IsAlarm(void)
{
	u8	Speed;
	u8  i;
	u8  j;
	u8  SpeedMax;
	u32 KeepTime;
	u8  Buffer[50];
	
	static u8 LastSpeedMax = 0;
	static u8 TtsPlayFlag = 0;
	static u8 TtsPlayDelay = 0;

	
	AreaRoadOverspeed[0].MaxSpeed = VdrOverspeed.AlarmSpeed;//普通限速值
	AreaRoadOverspeed[0].KeepTime = VdrOverspeed.AlarmKeeptime;
  AreaRoadOverspeed[0].Valid = 1;
	Speed = VDRSpeed_GetCurSpeed();
	SpeedMax = 0xff;
	j = 0;
	for(i=0; i<5; i++)
	{
		if(1 == AreaRoadOverspeed[i].Valid)
	  {
      if((AreaRoadOverspeed[i].MaxSpeed < SpeedMax)&&(AreaRoadOverspeed[i].MaxSpeed > 0))
	    {
		    SpeedMax = AreaRoadOverspeed[i].MaxSpeed;
	      j = i;
	    }
	  }
  }
	KeepTime = AreaRoadOverspeed[j].KeepTime;
			
	if(1 == VDROverspeed_NightIsArrived())
	{
			SpeedMax = SpeedMax*VdrOverspeed.NightSpeedPercent/100;
	}
	else
	{
	    //空
	}
	
	if((SpeedMax != LastSpeedMax)&&(0 != SpeedMax)&&(Speed > 0))
	{
		  TtsPlayFlag = 1;
		  TtsPlayDelay = 0;
	    
	}
	
	if(1 == TtsPlayFlag)
	{
	    TtsPlayDelay++;
		  if(TtsPlayDelay >= 15)
			{
			    TtsPlayFlag = 0;
				  TtsPlayDelay = 0;
				  sprintf((char *)Buffer,"当前限速%d公里",SpeedMax); 
		      Public_PlayTTSVoiceStr(Buffer);
          Public_ShowTextInfo((char *)Buffer,100);
			}
	}
	
	if((Speed > SpeedMax)&&(0 != SpeedMax))
	{
		VdrOverspeedAlramCount++;
		if(VdrOverspeedAlramCount >= KeepTime)
		{
			VdrOverspeedEarlyAlarmCount = 0;//预警计数清0
			VDROverspeed_WriteEarlyAlarmFlagBit(0, 0);//预警报警清0
			VDROverspeed_WriteAlarmFlagBit(0, 1);//超速报警置1
			
			return 1;
		}
	}
	else
	{
		VdrOverspeedAlramCount = 0;
	}

	VDROverspeed_WriteAlarmFlagBit(0, 0);//超速报警清0
	
	LastSpeedMax = SpeedMax;

	return 0;
}
/**************************************************************************
//函数名：VDROverspeed_IsEarlyAlarm
//功能：是否超速预警
//输入：无
//输出：无
//返回值：1:是;0:否
//备注：无
***************************************************************************/
static u8 VDROverspeed_IsEarlyAlarm(void)
{
	u8	Speed;
	u8  SpeedMax;
	u8  i;
	u8  j;
	u32 KeepTime;

	AreaRoadOverspeed[0].EarlyMaxSpeed = VdrOverspeed.EarlyAlramSpeed;//普通限速值
	AreaRoadOverspeed[0].EarlyKeepTime = VdrOverspeed.EarlyAlramKeeptime;
  AreaRoadOverspeed[0].Valid = 1;
	Speed = VDRSpeed_GetCurSpeed();
	SpeedMax = 0xff;
	j = 0;
	for(i=0; i<5; i++)
	{
		if(1 == AreaRoadOverspeed[i].Valid)
	  {
      if((AreaRoadOverspeed[i].EarlyMaxSpeed < SpeedMax)&&(AreaRoadOverspeed[i].EarlyMaxSpeed > 0))
	    {
		    SpeedMax = AreaRoadOverspeed[i].EarlyMaxSpeed;
	      j = i;
	    }
	  }
  }
	KeepTime = AreaRoadOverspeed[j].EarlyKeepTime;
	
	if(1 == VDROverspeed_NightIsArrived())
	{
			SpeedMax = SpeedMax*VdrOverspeed.NightSpeedPercent/100;
	}
	else
	{
	    //空
	}
	if(Speed > SpeedMax)
	{
		VdrOverspeedEarlyAlarmCount++;
		if(VdrOverspeedEarlyAlarmCount >= KeepTime)
		{
			VDROverspeed_WriteEarlyAlarmFlagBit(0, 1);//预警报警置1
			return 1;
		}
	}
	else
	{
		VdrOverspeedEarlyAlarmCount = 0;
	}

	VDROverspeed_WriteEarlyAlarmFlagBit(0, 0);//预警报警清0

	return 0;
}
/**************************************************************************
//函数名：VDROverspeed_Init
//功能：初始化
//输入：无
//输出：无
//返回值：无
//备注：主要是更新相关变量
***************************************************************************/
static void VDROverspeed_Init(void)
{
	u8 i;
	
  VDROverspeed_UpdateParameter();
	
	for(i=0; i<5; i++)
	{
	  AreaRoadOverspeed[i].Valid = 0;
	  AreaRoadOverspeed[i].Id = 0;
	  AreaRoadOverspeed[i].MaxSpeed = 0;
	  AreaRoadOverspeed[i].KeepTime = 0;
		AreaRoadOverspeed[i].EarlyMaxSpeed = 0;
	  AreaRoadOverspeed[i].EarlyKeepTime = 0;
	}
}
/**************************************************************************
//函数名：VDROverspeed_NightIsArrived
//功能：判断当前是否为夜间时间
//输入：无
//输出：无
//返回值：1：当前是夜间时间；0：当前不是夜间时间
//备注：
***************************************************************************/
static u8 VDROverspeed_NightIsArrived(void)
{
	  TIME_T CurTime;
	  u32 TimeCount;
	  u32 NightStartTimeCount;
	  u32 NightEndTimeCount;
	
	  if(1 != VDROverspeed_NightTimeIsOk(VdrOverspeed.NightStartTime))
		{
		    return 0;
		}
		
		if(1 != VDROverspeed_NightTimeIsOk(VdrOverspeed.NightEndTime))
		{
		    return 0;
		}
		
		if(0 == VdrOverspeed.NightSpeedPercent)
		{
		    return 0;
		}
		
		RTC_GetCurTime(&CurTime);
		NightStartTimeCount = VdrOverspeed.NightStartTime.hour*3600+VdrOverspeed.NightStartTime.min*60+VdrOverspeed.NightStartTime.sec;
		NightEndTimeCount = VdrOverspeed.NightEndTime.hour*3600+VdrOverspeed.NightEndTime.min*60+VdrOverspeed.NightEndTime.sec;
		TimeCount = CurTime.hour*3600+CurTime.min*60+CurTime.sec;
		if(NightStartTimeCount < NightEndTimeCount)
		{
		    if((TimeCount > NightStartTimeCount)&&(TimeCount < NightEndTimeCount))
				{
				    return 1;
				}
				else
				{
				    return 0;
				}
		}
		else if(NightStartTimeCount > NightEndTimeCount)
		{
		    if((TimeCount > NightStartTimeCount)&&(TimeCount < (24*3600)))
				{
				    return 1;
				}
				else if(TimeCount < NightEndTimeCount)
				{
				    return 1;
				}
				else
				{
				    return 0;
				}
		}
		else
		{
		    return 0;
		}
}
/**************************************************************************
//函数名：VDROverspeed_NightTimeIsOk
//功能：检查夜间时间是否正确
//输入：无
//输出：无
//返回值：1：正确；0：不正确
//备注：
***************************************************************************/
static u8 VDROverspeed_NightTimeIsOk(TIME_T Time)
{
    if((Time.hour > 23)||(Time.hour < 0)||
    (Time.min > 59)||(Time.min < 0)||
    (Time.sec > 59)||(Time.sec < 0))
    {
        return 0;
    }
		else
		{
		    return 1;
		}
}
///////////////////以下是为部标送检所添加///////////////////////
/**************************************************************************
//函数名：VDROverspeed_OnceOverReport
//功能：部标协议检测超速功能时要求：速度一旦大于限速值立马上报
//输入：无
//输出：无
//返回值：无
//备注：重点是超速开始时刻，超速报警时刻
***************************************************************************/
static void VDROverspeed_OnceOverReport(void)
{
    u8	Speed;
    static u8 ReportFlag = 0;
	  static u8 count = 0;
	
	
	  if(0 == BBXYTestFlag)
		{
		    return ;
		}
	
	  Speed = VDRSpeed_GetCurSpeed();
	  if((Speed >= VdrOverspeed.AlarmSpeed)&&(0 == ReportFlag))
	  {
		    Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);
			  ReportFlag = 1;
			  count++;
			  if(2 == count)
				{
				    count = 0;
				}
	  }
		else if(Speed < VdrOverspeed.AlarmSpeed)
		{
			  ReportFlag = 0;
		}
}
/**************************************************************************
//函数名：VDROverspeed_ClearTime
//功能：超速持续时间清0
//输入：无
//输出：无
//返回值：无
//备注：部标协议检测超速时，设置超速限速值时需调用此函数
***************************************************************************/
void VDROverspeed_ClearTime(void)
{
    VdrOverspeedEarlyAlarmCount = 0;
	  VdrOverspeedAlramCount = 0;
}






