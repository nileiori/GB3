/************************************************************************
//程序名称：VDR_Power.c
//功能：供电记录
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
//#include "stm32f2xx_rtc.h"
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "Adc.h"

/********************本地变量*************************/


/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static void VDRPower_Init(void);
static u8 VDRPower_GetStatus(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRPower_TimeTask
//功能：上电或断电记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRPower_TimeTask(void)
{
	static u8	VdrPowerInitFlag = 0;
	static u8	LastStatus = 0;
	static u8	count = 0;

	u8	Status;
	u8	Buffer[VDR_DATA_POWER_STEP_LEN];
	u32	TimeCount;
	u16	Addr;
	

	if(0 == VdrPowerInitFlag)
	{
		if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_POWER))//记录区自检已完成
		{
			VDRPower_Init();//记录一次掉电记录和上电记录
			VdrPowerInitFlag = 1;
		}
	}
	else
	{
		Status = VDRPower_GetStatus();//0:供电;1:掉电

		if((0 == LastStatus)&&(1 == Status))//由供电到掉电
		{
			TimeCount = RTC_GetCounter();
			VDRPub_ConvertNowTimeToBCD(Buffer);
			Buffer[6] = 2;//1:上电;2:断电 
			VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);
		}
		else if((1 == LastStatus)&&(0 == Status))//由掉电到供电
		{
			TimeCount = RTC_GetCounter();
			VDRPub_ConvertNowTimeToBCD(Buffer);
			Buffer[6] = 1;//1:上电;2:断电 
			VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);
		}

		LastStatus = Status;

		count++;
		if(count >= 20)//一秒钟记录一次时间
		{
			count = 0;
			TimeCount = RTC_GetCounter();
			Public_ConvertLongToBuffer(TimeCount,Buffer);
			Addr = FRAM_VDR_POWER_OFF_TIME_ADDR;
			FRAM_BufferWrite(Addr,Buffer,4);
		}
	}

	return ENABLE;		
}
/**************************************************************************
//函数名：VDRPower_Init
//功能：上电初始化
//输入：无
//输出：无
//返回值：无
//备注：将断电和上电事件保存到记录区
***************************************************************************/
static void VDRPower_Init(void)
{
	u8	Buffer[VDR_DATA_POWER_STEP_LEN];
	u8	BufferLen;
	u16	Addr;
	u32	TimeCount;
	TIME_T	Time;
	


	Addr = FRAM_VDR_POWER_OFF_TIME_ADDR;
	BufferLen = FRAM_BufferRead(Buffer, 4, Addr);
	if(0 != BufferLen)
	{
		TimeCount = Public_ConvertBufferToLong(Buffer);
		Gmtime(&Time, TimeCount);
		VDRPub_ConvertTimeToBCD(Buffer,&Time);
		Buffer[6] = 2;//1:上电;2:断电 
		VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);
	}

	TimeCount = RTC_GetCounter();
	Gmtime(&Time, TimeCount);
	VDRPub_ConvertTimeToBCD(Buffer,&Time);
	Buffer[6] = 1;//1:上电;2:断电 
	VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);	
}
/**************************************************************************
//函数名：VDRPower_GetStatus
//功能：主电状态检测
//输入：无
//输出：无
//返回值：0:供电;1:掉电
//备注：50ms调度1次
***************************************************************************/
static u8 VDRPower_GetStatus(void)
{
	static u8 PowerOnCount = 0;
	static u8 PowerOffCount = 0;
	static u8 PowerStatus = 0;
	
	u16	Ad = 0;

	Ad = Ad_GetValue(ADC_MAIN_POWER); 
	Ad = Ad*33*9/0xfff;//转换成V，每bit代表0.1V
	if(Ad >= 90)//终端工作电压是9~36V
	{
		PowerOffCount = 0;
		PowerOnCount++;
		if(PowerOnCount >= 21)
		{
			PowerOnCount--;
			PowerStatus = 0;
		}
	}
	else
	{
		PowerOnCount = 0;
		PowerOffCount++;
		if(PowerOffCount >= 21)
		{
			PowerOffCount--;
			PowerStatus = 1;
		}
	}

	return PowerStatus;	
}





