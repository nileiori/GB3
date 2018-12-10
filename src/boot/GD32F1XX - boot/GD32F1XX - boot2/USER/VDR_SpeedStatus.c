/************************************************************************
//程序名称：VDR_SpeedStatus.c
//功能：速度状态记录。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2014.10
//版本记录：
//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_rtc.h"

/********************本地变量*************************/
static u8	VdrSpeedStatusBuffer[VDR_DATA_SPEED_STATUS_STEP_LEN];
static u8	VdrSpeedStatusEnableFlag = 0;

/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static void VDRSpeedStatus_Write(u8 SpeedStatus, u32 StartTime, u32 EndTime);
static void VDRSpeedStatus_Init(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRSpeedStatus_TimeTask
//功能：速度状态记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRSpeedStatus_TimeTask(void)
{
	static u16 	SpeedAbnormalCount = 0;
	static u16 	SpeedNormalCount = 0;
	static u8	VdrSpeedStatusInitFlag = 0;

	u8	PulseSpeed;
	u8	GpsSpeed;
	u8	DifferenceSpeed;
	u16	Addr;
	u32	TimeCount;
	TIME_T	Time;

	if(0 == VdrSpeedStatusInitFlag)
	{
		if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED_STATUS))
		{
			VDRSpeedStatus_Init();
			VdrSpeedStatusInitFlag = 1;
		}
	}
	else
	{
		RTC_ReadTime(&Time);
		if(1 == Public_CheckTimeStruct(&Time))
		{
			if((0==Time.hour)&&(0==Time.min)&&(0==Time.sec))
			{
				VdrSpeedStatusEnableFlag = 1;
				Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
				FRAM_BufferWrite(Addr, &VdrSpeedStatusEnableFlag, 1);	
			}
		}
		if(1 == VdrSpeedStatusEnableFlag)
		{
			PulseSpeed = VDRPulse_GetSecondSpeed();
			GpsSpeed = 50;
//			GpsSpeed = Gps_ReadSpeed();
			if(PulseSpeed >= GpsSpeed)
			{
				DifferenceSpeed = PulseSpeed - GpsSpeed;
			}
			else
			{
				DifferenceSpeed = GpsSpeed - PulseSpeed;
			}

			if((GpsSpeed > 40)&&((100*DifferenceSpeed) >= (11*GpsSpeed)))//差值大于11%
			{
				SpeedNormalCount = 0;
				SpeedAbnormalCount++;
				if(SpeedAbnormalCount <= 60)
				{
					VdrSpeedStatusBuffer[13+(SpeedAbnormalCount-1)*2] = PulseSpeed;
					VdrSpeedStatusBuffer[13+(SpeedAbnormalCount-1)*2+1] = GpsSpeed;
				}
				else if(SpeedAbnormalCount >= 300)
				{
					SpeedAbnormalCount = 0;
					TimeCount = RTC_GetCounter();
					VDRSpeedStatus_Write(0x02, TimeCount-300, TimeCount);
				}
			}
			else if((GpsSpeed > 40)&&((100*DifferenceSpeed) < (11*GpsSpeed)))//差值小于11%
			{
				SpeedAbnormalCount = 0;
				SpeedNormalCount++;
				if(SpeedNormalCount <= 60)
				{
					VdrSpeedStatusBuffer[13+(SpeedNormalCount-1)*2] = PulseSpeed;
					VdrSpeedStatusBuffer[13+(SpeedNormalCount-1)*2+1] = GpsSpeed;
				}
				else if(SpeedNormalCount >= 300)
				{
					SpeedNormalCount = 0;
					TimeCount = RTC_GetCounter();
					VDRSpeedStatus_Write(0x01, TimeCount-300, TimeCount);
				}

			}
			else
			{
				SpeedAbnormalCount = 0;
				SpeedNormalCount = 0;
			}
		}
	}

	return ENABLE;		
}
/**************************************************************************
//函数名：VDRSpeedStatus_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：无
//备注：主要是上电更新变量VdrSpeedStatusEnableFlag
***************************************************************************/
static void VDRSpeedStatus_Init(void)
{
	u16	Addr;
	u8	Buffer[2];
	u8	BufferLen;

	Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;

	BufferLen = FRAM_BufferRead(Buffer, 1, Addr);
	if(0 != BufferLen)
	{
		VdrSpeedStatusEnableFlag = Buffer[0];
	}
	else
	{
		VdrSpeedStatusEnableFlag = 1;
	}

}
/**************************************************************************
//函数名：VDRSpeedStatus_Write
//功能：保存一条速度状态记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
static void VDRSpeedStatus_Write(u8 SpeedStatus, u32 StartTime, u32 EndTime)
{

	TIME_T Time;
	u16	Addr;

	if(SpeedStatus > 0x02)
	{
		return;
	}

	Gmtime(&Time, StartTime);
	if(0 == Public_CheckTimeStruct(&Time))
	{
		return;
	}
	
	Gmtime(&Time, EndTime);
	if(0 == Public_CheckTimeStruct(&Time))
	{
		return;
	}

	if(EndTime != (StartTime+300))
	{
		return;
	}

	VdrSpeedStatusBuffer[0] = SpeedStatus;//速度状态，1字节，0x01:正常;0x02:异常

	Gmtime(&Time, StartTime);
	VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+1,&Time);//开始时间
				
	Gmtime(&Time, EndTime);
	VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+7,&Time);//结束时间

	VDRData_Write(VDR_DATA_TYPE_SPEED_STATUS, VdrSpeedStatusBuffer, VDR_DATA_SPEED_STEP_LEN-5, EndTime);


	VdrSpeedStatusEnableFlag = 0;
	Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
	FRAM_BufferWrite(Addr, &VdrSpeedStatusEnableFlag, 1);				
			
}






