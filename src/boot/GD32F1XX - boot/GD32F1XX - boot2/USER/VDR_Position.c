/************************************************************************
//程序名称：VDR_Position.c
//功能：360h位置信息记录
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "Rtc.h"
#include "spi_flash_app.h"
#include "spi_flash.h"
#include "stm32f10x_rtc.h"

/********************本地变量*************************/
static u8	VdrPositionBuffer[VDR_DATA_POSITION_STEP_LEN] = {0};

/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static void VDRPosition_Init(void);
static void VDRPosition_SaveStartTime(void);
static void VDRPosition_SavePosition(u8 EnableFlag,u8 Offset);
static void VDRPosition_Write(void);
/********************函数定义*************************/

/**************************************************************************
//函数名：VDRPosition_TimeTask
//功能：360h位置信息记录，每分钟1条位置信息，按小时记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRPosition_TimeTask(void)
{
	static 	u32	LastTimeCount;
	static u8	AlreadyRunFlag = 0;//1为已行驶过，0为未行驶过
	static u8	InitFlag = 0;
	static 	TIME_T	LastTime;

	u32	CurrentTimeCount;
//	u8	*p = NULL;
	u8	RunFlag;
	TIME_T	CurrentTime;

	if(0 == InitFlag)
	{
		if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_POSITION))
		{
			VDRPosition_Init();
			InitFlag = 1;
		}
	}
	else
	{
		CurrentTimeCount = RTC_GetCounter();
		if(CurrentTimeCount != LastTimeCount)//秒钟动了
		{
				RunFlag = VDRDoubt_GetCarRunFlag();
				if(1 == RunFlag)
				{
					AlreadyRunFlag = 1;
				}
				RTC_ReadTime(&CurrentTime);
				if(CurrentTime.min != LastTime.min)//分钟动了
				{
					VDRPosition_SaveStartTime();
					
					VDRPosition_SavePosition(AlreadyRunFlag,CurrentTime.min);
					
					if(59 == CurrentTime.min)//59分时把数据（60分钟的位置信息和平均速度）由暂存区转入记录区
					{	
						VDRPosition_Write();
					}
					AlreadyRunFlag = 0;
				}
				LastTime.year = CurrentTime.year;
				LastTime.month = CurrentTime.month;
				LastTime.day = CurrentTime.day;
				LastTime.hour = CurrentTime.hour;
				LastTime.min = CurrentTime.min;
				LastTime.sec = CurrentTime.sec;
		}
		LastTimeCount = CurrentTimeCount;
	}

	return ENABLE;		
}
/**************************************************************************
//函数名：VDRPosition_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：0:未完成初始化，1:已完成初始化
//备注：上电后会检测位置信息暂存区，有必要会转移至记录区
***************************************************************************/
static void VDRPosition_Init(void)
{
	u32	StartTimeCount;
	u32	Addr;
	u16	i;
	u8	flag;
	TIME_T 	StartTime;
	TIME_T 	CurrentTime;

	
	Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_BufferRead(VdrPositionBuffer, Addr, 666);//位置信息块长度是666
	flag = 0;
	for(i=0; i<666; i++)
	{
		if(VdrPositionBuffer[i] != 0xff)
		{
			flag = 1;
			break;
		}
	}
	if(0 == flag)
	{
		return ;
	}
	
	RTC_ReadTime(&CurrentTime);
	VDRPub_ConvertBCDToTime(&StartTime,VdrPositionBuffer);
	if(1 == Public_CheckTimeStruct(&StartTime))
	{
		if((StartTime.day == CurrentTime.day)&&(StartTime.hour == CurrentTime.hour))
		{
				//上次掉电和本次上电是属于同一个小时内，继续存储即可
		}
		else if(StartTime.hour != CurrentTime.hour)
		{
			StartTime.min = 0;
			StartTime.sec = 0;
			StartTimeCount = ConverseGmtime(&StartTime);
			Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
			SPI_FLASH_BufferRead(VdrPositionBuffer, Addr, 666);//读取暂存的数据
			
			VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, StartTimeCount);//保存到记录区
			
			SPI_FLASH_SectorErase(Addr);//擦除扇区
		}
		else
		{
			SPI_FLASH_SectorErase(Addr);
		}
	}
	else
	{
		SPI_FLASH_SectorErase(Addr);	
	}				
}
/**************************************************************************
//函数名：VDRPosition_SaveStartTime
//功能：暂存位置信息记录的开始时间
//输入：无
//输出：无
//返回值：无
//备注：若已保存过，则不再写入，若保存的时间出错，则将该扇区擦除
***************************************************************************/
static void VDRPosition_SaveStartTime(void)
{
	u32	Addr;
	u8	Buffer[20];
	u8	i;
	u8	flag;
	TIME_T	Time;
	TIME_T	StartTime;
	
	Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_BufferRead(Buffer, Addr, 6);
	flag = 0;
	for(i=0; i<6; i++)
	{
		if(0xff != Buffer[i])
		{
				flag = 1;
				break;
		}
	}
	
	if(0 == flag)
	{
		RTC_ReadTime(&Time);
		if(1 == Public_CheckTimeStruct(&Time))
		{
			Time.min = 0;
			Time.sec = 0;
			VDRPub_ConvertTimeToBCD(Buffer,&Time);
			SPI_FLASH_BufferWrite(Buffer, Addr, 6);
		}
	}
	else
	{
		if(1 == VDRPub_ConvertBCDToTime(&StartTime,Buffer))
		{
			if(1 == Public_CheckTimeStruct(&StartTime))
			{
									//写入的时间正确
			}
			else
			{
					//SPI_FLASH_SectorErase(Addr);//擦除扇区
			}
		}
		else
		{
			//SPI_FLASH_SectorErase(Addr);//擦除扇区
		}
	}
}
/**************************************************************************
//函数名：VDRPosition_SavePosition
//功能：暂存一条位置信息
//输入：无
//输出：无
//返回值：无
//备注：每分钟1条位置信息，信息内容见标准A.20
***************************************************************************/
static void VDRPosition_SavePosition(u8 EnableFlag,u8 Offset)
{
	u32	Addr;
	u8	flag;
	u8	i;
	u8	Buffer[20];
	
	Addr = 6+FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES+11*Offset;
	SPI_FLASH_BufferRead(Buffer, Addr, 11);
	flag = 0;
	for(i=0; i<11; i++)
	{
		if(0xff != Buffer[i])
		{
			flag = 1;
			break;
		}
	}
	if((0 == flag)&&(1 == EnableFlag))//暂存每分钟位置信息和平均速度
	{
					//	if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
					//	{
							//Public_GetCurPositionInfoDataBlock(Buffer);//10字节
							Buffer[10] = VDRSpeed_GetMinuteSpeed();
					//	}
						//else
						//{
							//memset(Buffer,0xff,10);//无效位置
							//Buffer[0] = 0x7F;
							//Buffer[4] = 0x7F;
							//Buffer[10] = VDRSpeed_GetMinuteSpeed();
						//}
			SPI_FLASH_BufferWrite(Buffer, Addr, 11);
	}
	else 
	{
			//暂不处理
	}
}
/**************************************************************************
//函数名：VDRPosition_Write
//功能:写位置信息
//输入：无
//输出：无
//返回值：无
//备注：每分钟1条位置信息，信息内容见标准A.20
***************************************************************************/
static void VDRPosition_Write(void)
{
	u32	Addr;
	u32	TimeCount;
	u8	Buffer[10];
	
	TIME_T	Time;
	
	Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_BufferRead(Buffer, Addr, 6);
					
	VDRPub_ConvertBCDToTime(&Time,Buffer);
	if(1 == Public_CheckTimeStruct(&Time))
	{
						
		TimeCount = ConverseGmtime(&Time);
		Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
		SPI_FLASH_BufferRead(VdrPositionBuffer, Addr, 666);//读取暂存的数据
		VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, TimeCount);//保存到记录区
	}
	SPI_FLASH_SectorErase(Addr);//擦除扇区
}






