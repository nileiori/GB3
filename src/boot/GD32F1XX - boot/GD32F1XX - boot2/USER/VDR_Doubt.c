/************************************************************************
//程序名称：VDR_Doubt.c
//功能：疑点数据功能
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "fm25c160.h"
#include "EepromPram.h"
#include "Rtc.h"
#include "stm32f10x_rtc.h"
/********************本地变量*************************/
static 	u8 	CarRunFlag = 0;//车辆停驶、行驶标志，1为行驶，0为停驶。
static 	u8 	VdrDoubtInitFlag = 0;//初始化标志，1为已初始化，0为未初始化。
static	u8 	VdrDoubtBuffer[VDR_DATA_DOUBT_STEP_LEN];//疑点数据缓冲
static 	u16 	VdrDoubtCount = 0;//疑点数据点数记录
static	u8 	VdrDoubtTestBuffer[300];//疑点数据缓冲
/********************全局变量*************************/
u16	PowerShutValue = 50;//??5.0V

/********************外部变量*************************/


/********************本地函数声明*********************/
static u16 VDRDoubt_Read(u8 *pBuffer, u16 OffsetPoint);
static void VDRDoubt_WritePositionNoChange(void);
static void VDRDoubt_WritePowerOff(void);
static void VDRDoubt_WritePowerOn(void);
static void VDRDoubt_WriteStop(void);
static void VDRDoubt_SaveCurrentSpeedStatus(void);
static void VDRDoubt_RunOrStop(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRDoubt_TimeTask
//功能：实现疑点数据的记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：疑点数据定时任务，50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRDoubt_TimeTask(void)
{
	if(0 == VdrDoubtInitFlag)
	{
		if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_DOUBT))
		{
			VDRDoubt_WritePowerOn();//保存上电重启时的疑点数据，相当于没有备电的断电疑点数据
			VdrDoubtInitFlag = 1;
		}
	}
	else
	{
		VDRDoubt_RunOrStop();//行驶与停驶的判定

		VDRDoubt_SaveCurrentSpeedStatus();//记录当前的速度和状态，0.2秒一个

		VDRDoubt_WriteStop();//保存正常停车的疑点数据

		VDRDoubt_WritePowerOff();//保存断电的疑点数据

		VDRDoubt_WritePositionNoChange();//保存行驶但位置无变化的疑点数据
	}

	return ENABLE;		
}
/**************************************************************************
//函数名：VDRDoubt_GetCarRunFlag
//功能：获取车辆行驶标志
//输入：无
//输出：无
//返回值：1为行驶，0为停驶
//备注：行驶速度大于0持续10秒为行驶，速度等于0持续10秒为停驶
***************************************************************************/
u8 VDRDoubt_GetCarRunFlag(void)
{
	return CarRunFlag;
}
/**************************************************************************
//函数名：VDRDoubt_RunOrStop
//功能：行驶与停驶的判定
//输入：无
//输出：无
//返回值：无
//备注：对本地变量CarRunFlag(值为1为行驶，为0为停驶)有影响
***************************************************************************/
static void VDRDoubt_RunOrStop(void)
{
	static u16 RunCount = 0;
	static u16 StopCount = 0;

	u8	Speed;

	Speed = VDRPulse_GetInstantSpeed();

	if(Speed > 0)
	{
		StopCount = 0;
		RunCount++;
		if(RunCount >= 10*SECOND)
		{
			RunCount--;
			CarRunFlag = 1;
		}
	}
	else
	{
		RunCount = 0;
		StopCount++;
		if(StopCount >= 10*SECOND)
		{
			StopCount--;
			CarRunFlag = 0;
		}
	}
}
/**************************************************************************
//函数名：VDRDoubt_SaveCurrentSpeedStatus
//功能：保存当前的速度和状态
//输入：无
//输出：无
//返回值：无
//备注：0.2秒1个
***************************************************************************/
static void VDRDoubt_SaveCurrentSpeedStatus(void)
{
	static u8 Count1 = 0;
	static u8 Count2 = 0;

	u8	Status;
	u8	Buffer[10];
	u16	WriteAddr;

	Count1++;
	if(Count1 >= (2*SYSTICK_0p1SECOND))
	{
		Count1 = 0;

		Status = 0;
//		Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:刹车
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:左转
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:右转
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:远光
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:近光
		//bit2-bit1未定义
	//	Status |= Io_ReadExtCarStatusBit(STATUS_BIT_ACC);//bit0:ACC

		Buffer[0] = VDRPulse_GetInstantSpeed();
		Buffer[1] = Status;

		WriteAddr = FRAM_VDR_DOUBT_DATA_ADDR+3*VdrDoubtCount;
		FRAM_BufferWrite(WriteAddr,Buffer,2);//记录速度与状态
		
		VdrDoubtTestBuffer[2*VdrDoubtCount] = Buffer[0];
		VdrDoubtTestBuffer[2*VdrDoubtCount+1] = Buffer[1];
		
		VdrDoubtCount++;
		if(VdrDoubtCount >= 150)
		{
			VdrDoubtCount = 0;
		}

		Public_ConvertShortToBuffer(VdrDoubtCount,Buffer);
		WriteAddr = FRAM_VDR_DOUBT_COUNT_ADDR;
		FRAM_BufferWrite(WriteAddr,Buffer,2);//记录疑点数据点数

		
		Count2++;
		if(Count2 >= 5)//每秒记录一次时间
		{
			Count2 = 0;
			VDRPub_ConvertNowTimeToBCD(Buffer);
			WriteAddr = FRAM_VDR_DOUBT_TIME_ADDR;
			FRAM_BufferWrite(WriteAddr,Buffer,6);//记录当前时间
		}
		
	}
}
/**************************************************************************
//函数名：VDRDoubt_WriteStop
//功能：写停车事故疑点
//输入：无
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WriteStop(void)
{
	static u8 LastRunFlag = 0;//1为行驶，0为停驶
	
	u8	RunFlag;
	u16	length;
	u32	Time;
	s16	i;
	s16	OffsetPoint = 50;
	s16	j;
	u8	flag;
	
	RunFlag = VDRDoubt_GetCarRunFlag();

	if((1 == LastRunFlag)&&(0 == RunFlag))//由行驶变为停驶
	{
		length = VDRDoubt_Read(VdrDoubtBuffer,50);	
		if(0 != length)
		{
			//比较铁电存储的数据与内存存储的数据是否一致
			flag = 0;
			i = VdrDoubtCount;
			if(i >= OffsetPoint)
			{
				i -= OffsetPoint;
			}
			else
			{
				i = i+150;
				i -= OffsetPoint;
			}
			i--;
			if(i < 0)
			{
				i = 149;
			}
			for(j=0;j<100;j++)
			{
				if((VdrDoubtBuffer[24+2*j] == VdrDoubtTestBuffer[2*i])&&
					(VdrDoubtBuffer[24+2*j+1] == VdrDoubtTestBuffer[2*i+1]))
				{
					
				}
				else
				{
					flag = 1;
					break;
				}
				i--;
				if(i < 0)
				{
					i = 149;
				}
			}
			if(0 == flag)
			{
				Time = RTC_GetCounter();
				VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
			}
		}

	}
	
	LastRunFlag = RunFlag;
	
}
/**************************************************************************
//函数名：VDRDoubt_WritePowerOn
//功能：写上电事故疑点
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WritePowerOn(void)
{
	u8	Buffer[10];
	u8	BufferLen;
	u16	i;
	u16	Addr;
	u16	length;
	TIME_T	Time;
	u32	TimeCount;

	BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//读上一次记录的点位置
	if(0 != BufferLen)
	{
		VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
		length = VDRDoubt_Read(VdrDoubtBuffer,0);	
		if(0 != length)
		{
			Public_ConvertBcdToValue(Buffer, VdrDoubtBuffer, 6);
			Time.year = 2000+Buffer[0];
			Time.month = Buffer[1];
			Time.day = Buffer[2];
			Time.hour = Buffer[3];
			Time.min = Buffer[4];
			Time.sec = Buffer[5];
			if(1 == Public_CheckTimeStruct(&Time))
			{
				TimeCount = ConverseGmtime(&Time);
				VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
			}
		}
		
	}
	else
	{
		
	}
	VdrDoubtCount = 0;
	Buffer[0] = 0;
	Buffer[1] = 0;
	for(i=0; i<150; i++)
	{
		Addr = FRAM_VDR_DOUBT_DATA_ADDR+3*i;
		FRAM_BufferWrite(Addr,Buffer,2);
		VdrDoubtTestBuffer[2*i] = Buffer[0];
		VdrDoubtTestBuffer[2*i+1] = Buffer[1];
		if((1 == VdrDoubtTestBuffer[2*i+1])||(1 == VdrDoubtTestBuffer[2*i]))
		{
			VdrDoubtTestBuffer[2*i+1] = 0;
			VdrDoubtTestBuffer[2*i] = 0;
		}
	}
}
/**************************************************************************
//函数名：VDRDoubt_WritePowerOff
//功能：写掉电事故疑点
//输入：无
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WritePowerOff(void)
{
	static u8 LastPowerFlag = 0;//0为上电，1为断电
	
	u8	PowerFlag;
	u16	length;
	u16	Ad = 0;
	u32	Time;

	if(0 == VDRDoubt_GetCarRunFlag())//停驶
	{
		return ;
	}

//	Ad = Ad_GetValue(ADC_MAIN_POWER);//读取主电电压
//	Ad = Ad*33*9/0xfff;//转成以0.1V为单位的值
	Ad = 120;

	if(Ad <= PowerShutValue)
	{
		PowerFlag = 1;
	}
	else
	{
		PowerFlag = 0;
	}

	if((0 == LastPowerFlag)&&(1 == PowerFlag))//由上电变为掉电
	{
		length = VDRDoubt_Read(VdrDoubtBuffer,0);	
		if(0 != length)
		{
			Time = RTC_GetCounter();
			VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
		}

	}
	
	LastPowerFlag = PowerFlag;
}
/**************************************************************************
//函数名：VDRDoubt_WritePositionNoChange
//功能：写位置信息无变化的事故疑点数据
//输入：无
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WritePositionNoChange(void)
{
	static 	u8 		LastNoChangeFlag = 0;//0为有变化，1为没有变化
	static  u16		Count = 0;
	static 	GPS_STRUCT 	LastPosition;//上一个点的位置信息

	u8	NoChangeFlag;
	u16	length;
	u32	Time;
	GPS_STRUCT 	Position;//当前点的位置信息

	if(0 == VDRDoubt_GetCarRunFlag())//停驶
	{
		return ;
	}

	//if(0 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))//不定位
   // 	{
    //    	return;
   //	}

	NoChangeFlag = 0;
	//Gps_CopygPosition(&Position);//获取有效位置信息
	if(((Position.Latitue_D   == LastPosition.Latitue_D)
       	    &&(Position.Latitue_F  == LastPosition.Latitue_F))
            &&(Position.Latitue_FX == LastPosition.Latitue_FX))  
	{
		Count++;
		if(Count >= 10*SECOND)
		{
			Count--;
			NoChangeFlag = 1;
		}
	}
	else
	{
		Count = 0;
	}

	if((0 == LastNoChangeFlag)&&(1 == NoChangeFlag))//由位置信息有变化变为位置信息无变化
	{
		length = VDRDoubt_Read(VdrDoubtBuffer,0);	
		if(0 != length)
		{
			Time = RTC_GetCounter();
			VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
		}

	}
	
	LastNoChangeFlag = NoChangeFlag;
}
/**************************************************************************
//函数名：VDRDoubt_Read
//功能：读事故疑点数据
//输入：OffsetPoint:偏移点数
//输出：pBuffer:指向数据缓冲；
//返回值：获取的数据长度
//备注：数据内容符合表A.22要求，疑点数据共150个点，0.2秒一个点，记录了最近30秒的数据，
//当获取正常停车时的疑点数据时，OffsetPoint取值为50（因为有10秒的停车判断，要偏移10秒，即50个点）
//当获取断电或位置无变化时的疑点数据时OffsetPoint取值为0，即没有偏移。
***************************************************************************/
static u16 VDRDoubt_Read(u8 *pBuffer, u16 OffsetPoint)
{
	u8	*p = NULL;
	u8	Buffer[3];
	u8	BufferLen;
	u8	count;
	u8	flag;
	u16	Addr;
	u16	length;
	s16	i;
	s16	j;

	p = pBuffer;//结束时间，6字节，BCD码
	Addr = FRAM_VDR_DOUBT_TIME_ADDR;
	length = FRAM_BufferRead(p,6,Addr);
	if(0 == length)
	{
		return 0;
	}
	p += 6;

	length = EepromPram_ReadPram(E2_CURRENT_LICENSE_ID,p);//驾驶证号码，18字节，ASCII码
	if(0 == length)
	{
		memset(p,'0',18);
	}
	p += 18;

		
	
	BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//读上一次记录的点位置
	if(0 != BufferLen)
	{
		VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
	}
	else
	{
		return 0;
	}
	i = VdrDoubtCount;
	if(i >= OffsetPoint)
	{
		i -= OffsetPoint;
	}
	else
	{
		i = i+150;
		i -= OffsetPoint;
	}
	i--;
	if(i < 0)
	{
		i = 149;
	}

	count = 0;
	flag = 0;
	for(j=0; j<100; j++)//疑点数据，200字节，hex值
	{
		Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
		length = FRAM_BufferRead(Buffer,2,Addr);
		if(0 == length)
		{
			return 0;
		}
		if(Buffer[0] > 0)
		{
				count++;
				if(count >= 50)//表明行驶过
				{
					flag = 1;
				}
		}
		else
		{
				count = 0;
		}
		memcpy(p,Buffer,2);
		p += 2;

		i--;
		if(i < 0)
		{
			i = 149;
		}
	}

	if(0 == flag)
	{
		return 0;
	}
	
	//有效位置点
	//Public_GetCurPositionInfoDataBlock(p);

	return 234;
	
}






