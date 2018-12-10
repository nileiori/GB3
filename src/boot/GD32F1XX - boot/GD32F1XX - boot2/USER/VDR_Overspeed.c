/************************************************************************
//程序名称：VDR_Overspeed.c
//功能：超速报警
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：
//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "EepromPram.h"

/********************本地变量*************************/
static	VDR_OVERSPEED_STRUCT	VdrOverspeed;
static 	u8	VdrOverspeedAlarmFlag = 0;//超速报警标志，相应位为1表示产生了该报警，为0表示该报警清除了。
//在播报超速报警语音时若已有另外的一个bit置位了（已有超速报警），则不需要播报本次超速报警语音，避免重复播报。
//bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速
static	u8	VdrOverspeedEarlyAlarmFlag = 0;//超速预警标志，相应位为1表示产生了该预警，为0表示该预警清除了。
//在播报超速预警语音时若已有另外的一个bit置位了（已有超速预警），则不需要播报本次超速预警语音，避免重复播报。
//bit0表示普通超速，bit1表示区域超速，bit2表示线路超速，bit3表示夜间普通超速，bit4表示夜间区域超速，bit5表示夜间线路超速

static  u32 VdrOverspeedEarlyAlramCount = 0;
static  u32 VdrOverspeedAlramCount = 0;
/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static u8 VDROverspeed_IsAlarm(void);
static u8 VDROverspeed_IsEarlyAlarm(void);
static void VDROverspeed_Init(void);
static u8 VDROverspeed_ReadEarlyAlarmFlag(void);
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
						//Public_PlayTTSVoiceAlarmStr(VdrOverspeed.AlarmVoiceBuffer);
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
							//Public_PlayTTSVoiceAlarmStr(VdrOverspeed.EarlyAlarmVoiceBuffer);
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
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, 0);
		}
	}
	else
	{
		if(0 == VdrOverspeedAlarmFlag)
		{
			VdrOverspeedAlarmFlag |= 1<<bit;
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, 1);
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
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, 0);
		}
	}
	else
	{
		if(0 == VdrOverspeedEarlyAlarmFlag)
		{
			VdrOverspeedEarlyAlarmFlag |= 1<<bit;
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, 1);
		}
	}
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

	Speed = VDRPulse_GetSecondSpeed();
	if(Speed > VdrOverspeed.AlarmSpeed)
	{
		VdrOverspeedAlramCount++;
		if(VdrOverspeedAlramCount >= VdrOverspeed.AlarmKeeptime)
		{
			VdrOverspeedEarlyAlramCount = 0;//预警计数清0
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

	Speed = VDRPulse_GetSecondSpeed();
	if(Speed > VdrOverspeed.EarlyAlramSpeed)
	{
		VdrOverspeedEarlyAlramCount++;
		if(VdrOverspeedEarlyAlramCount >= VdrOverspeed.EarlyAlramKeeptime)
		{
			VDROverspeed_WriteEarlyAlarmFlagBit(0, 1);//预警报警置1
			return 1;
		}
	}
	else
	{
		VdrOverspeedEarlyAlramCount = 0;
	}

	VDROverspeed_WriteEarlyAlarmFlagBit(0, 0);//预警报警清0

	return 0;
}
/**************************************************************************
//函数名：VDROverspeed_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：无
//备注：主要是更新相关变量
***************************************************************************/
static void VDROverspeed_Init(void)
{
	u8	Buffer[40];
	u16	BufferLen;
	u16	temp;

	BufferLen = EepromPram_ReadPram(E2_MAX_SPEED_ID, Buffer);//超速报警限速值
	if(BufferLen == E2_MAX_SPEED_LEN)
	{
		VdrOverspeed.AlarmSpeed = Buffer[3];//一般前3字节均为0
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
		if(temp < VdrOverspeed.AlarmSpeed)
		{
			VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - (temp&0xff);
		}
		
	}
	else
	{
		VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - 5;
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

	
}





