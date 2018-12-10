/************************************************************************
//程序名称：Vss.c
//功能：VSS连接异常判断
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2016.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V1.0:
//1.当GPS速度一直在15km/h以上时，判断脉冲速度，脉冲速度持续1分钟大于0表明
//   脉冲速度传感器已连接，清除VSS故障报警；若脉冲速度持续1分钟等于0表明脉冲速度传感器没有
//   连接，置位VSS故障报警。
//2.当速度类型选择为自动速度类型时（SpeedFlag值为0x03或0x02），VSS故障时会将速度类型
//   设置为0x03；VSS连接正常时（没有VSS故障报警）会将速度类型设置为0x02
*************************************************************************/
/********************文件包含*************************/
#include "Vss.h"
#include "Io.h"
#include "VDR_Doubt.h"
#include "Gps_App.h"
#include "EepromPram.h"
#include "other.h"
#include "VDR_Pulse.h"

/********************本地变量*************************/

/********************全局变量*************************/

/********************外部变量*************************/
extern u8  SpeedFlag;//速度类型,0为脉冲,1为GPS,2自动脉冲,3自动GPS

/********************本地函数声明*********************/

/********************函数定义*************************/

/**************************************************************************
//函数名：Vss_TimeTask
//功能：VSS连接异常判断
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：VSS连接异常定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState Vss_TimeTask(void)
{
	static u8 PulseRunCount = 0;
	static u8 PulseStopCount = 0;
	
	u8 PulseSpeed;
	u8 GpsSpeed;
	u8 flag;
	
	PulseSpeed = VDRPulse_GetInstantSpeed();
	GpsSpeed = Gps_ReadSpeed();
	if((1 == Io_ReadStatusBit(STATUS_BIT_ACC))
	   &&(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION)))
	{
		if(GpsSpeed > 15)
		{
			if(0 == PulseSpeed)
			{
				PulseRunCount = 0;
				PulseStopCount++;
				if(PulseStopCount >= 60)//停车60秒
				{
					PulseStopCount = 0;
          Io_WriteAlarmBit(ALARM_BIT_VSS_FAULT, SET);
          if((0x03 != SpeedFlag)&&(0x02 == SpeedFlag))
          {
            flag = 0x03;
						SpeedFlag = 0x03;
            EepromPram_WritePram(E2_SPEED_SELECT_ID, &flag, 1);
            SpeedFlagUpdatePram();
          }
				}
			}
			else
			{
				PulseStopCount = 0;
				PulseRunCount++;
				if(PulseRunCount >= 60)//行驶60秒
				{
					PulseRunCount = 0;
					Io_WriteAlarmBit(ALARM_BIT_VSS_FAULT, RESET);
          if((0x02 != SpeedFlag)&&(0x03 == SpeedFlag))
          {
            flag = 0x02;
						SpeedFlag = 0x02;
            EepromPram_WritePram(E2_SPEED_SELECT_ID, &flag, 1);
            SpeedFlagUpdatePram();
          }
				}
			}
		}
		else
		{
			PulseStopCount = 0;
			PulseRunCount = 0;
		}
	}
	else
	{
		PulseStopCount = 0;
		PulseRunCount = 0;
	}
	
	return ENABLE;
}








