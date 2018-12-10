/************************************************************************
//程序名称：Can_App.c
//功能：该模块实现CAN数据上报补报功能。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.12
//版本记录：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include <string.h>

#include "Can_App.h"
#include "Can_Driver.h"
#include "queue.h"
#include "SysTickApp.h"
#include "RadioProtocol.h"
#include "modem_lib.h"
#include "modem_app_com.h"
#include "EepromPram.h"
#include "Public.h"
#include "VDR_Pub.h"


/********************本地变量*************************/
static u8  CanUploadBuffer[CAN_UPLOAD_BUFFER_SIZE] = {0};

/********************全局变量*************************/
CAN_FILTER_STRUCT CanFilter[CAN_FILTER_ID_NUM];
u32 Can1GatherTime = 1;	//CAN1 采集时间间隔，单位为ms，为0表示不采集，默认是采集的
u16 Can1UploadTime = 0;	//CAN1 上传时间间隔，为0表示不上传

/********************外部变量*************************/
extern Queue CanRxQ;
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式

/********************本地函数声明*********************/

/********************函数定义*************************/
/**************************************************************************
//函数名：Can_TimeTask
//功能：实现CAN数据上报功能
//输入：无
//输出：无
//返回值：无
//备注：CAN数据上报定时任务，50ms调度1次，任务调度器需要调用此函数。
//CAN接口每收到一帧数据先判断是否需要软件滤除，若不滤除则放入接收队列，
//当队列未读取的数据长度超过（CAN_RX_BUFFER_SIZE-200）字节或者上传时间间隔到时，
//上传给平台；当上传时间间隔设置为0时表示不上传。
***************************************************************************/
FunctionalState Can_TimeTask(void)
{
	static u32 SendTimeCount = 0;
	static u32 TimeCount = 0;
	static u8 InitFlag = 0;
	static u32 SendPacketCount = 0;
	static u32 QlengthTimeCount = 0;
	
	u8 Buffer[CAN_STEP_LEN_MAX+1] = {0};
	u8 Data = 0;
	u8 Flag = 0;
	u8 CanFrameCount = 0;//帧个数计数
	u8 CanFrameLength = 0;//每一帧长度计数
	u16 CanFrameTotoalLength = 0;//各帧累计长度
	u8 CanFrameStartFlag = 0;
	u8 *p = NULL;
	u8 SendFlag = 0;
	u32 Qlength;
	TIME_T Time;

	if(0 == InitFlag)
	{
		InitFlag = 1;
		Can_Init();
		Can_UpdatePram();
	}
	else if(0 == Can1UploadTime)
	{
		return ENABLE;
	}

	Qlength = QueueLength(CanRxQ);

	if(Qlength >= 1000)
	{
		QlengthTimeCount = 0;
		SendFlag = 1;
	}
	else
	{
		if(0 == Qlength)
		{
		   TimeCount++;
		   if(TimeCount >= (10*SECOND))
		   {
			    TimeCount = 0;
		      SendPacketCount = 0;
			 }
			 QlengthTimeCount = 0;
		}
		else
		{
		    TimeCount = 0;
			  QlengthTimeCount++;
			  if(QlengthTimeCount >= (15*SECOND))
				{
					  QlengthTimeCount = 0;
				    SendFlag = 1;
				}
		}
	}

	if(1 == SendFlag)
	{
		p = CanUploadBuffer+2;//头2个字节预留给总项数
		CanFrameCount = 0;
		CanFrameLength = 0;
		CanFrameTotoalLength = 0;
		while(1)
		{
			Data = QueueFrontAndOut2(CanRxQ,&Flag);
			if(Flag)
			{
				if(0x7e == Data)
				{
					if(0 == CanFrameStartFlag)
					{
						CanFrameStartFlag = 1;
						CanFrameLength = 0;
					}
					else if(CanFrameLength > 0)
					{
						CanFrameLength = unTransMean(Buffer,CanFrameLength);
						if(17 == CanFrameLength)
						{
							if(0 == CanFrameCount)
							{
								if(1 == BBGNTestFlag)//dxl,2016.5.23检测要求上传的接收时间必须间隔10秒
								{
								    if(0 == SendPacketCount)
										{
										    memcpy(p,Buffer,17);//第1个CAN数据包含时间信息
								        p = p+17;
								        CanFrameTotoalLength += 17;
											
											  RTC_GetCurTime(&Time);
											  Time.hour = VDRPub_BCD2HEX(Buffer[0]);
											  Time.min = VDRPub_BCD2HEX(Buffer[1]);
											  Time.sec = VDRPub_BCD2HEX(Buffer[2]);
											  
											  SendTimeCount = ConverseGmtime(&Time);
										}
										else
										{   
										    SendTimeCount += 10;
											  Gmtime(&Time, SendTimeCount);
											  Buffer[0] = VDRPub_HEX2BCD(Time.hour);
											  Buffer[1] = VDRPub_HEX2BCD(Time.min);
											  Buffer[2] = VDRPub_HEX2BCD(Time.sec);
											    
											  memcpy(p,Buffer,17);//第1个CAN数据包含时间信息
								        p = p+17;
								        CanFrameTotoalLength += 17;
										}
								}
								else
								{
								    memcpy(p,Buffer,17);//第1个CAN数据包含时间信息
								    p = p+17;
								    CanFrameTotoalLength += 17;
								}
							}
							else
							{
								memcpy(p,Buffer+5,12);
								p = p+12;
								CanFrameTotoalLength += 12;
							}
							CanFrameCount++;
						}
            CanFrameStartFlag = 0;
						CanFrameLength = 0;
					}

				}
				else
				{
					
					if(CanFrameLength >= CAN_STEP_LEN_MAX)
					{
						CanFrameStartFlag = 0;	
						CanFrameLength = 0;
					}
					if(1 == CanFrameStartFlag)
					{
						Buffer[CanFrameLength] = Data;
						CanFrameLength++;
					}
				}
			}
			else
			{
				break;
			}
			if(CanFrameCount >= 50)//每次最多只能打包50项CAN数据帧
			{
				break;
			}
		}

		if(CanFrameCount > 0)
		{
			p = CanUploadBuffer;
			*p++ = (CanFrameCount&0xff00) >> 8;
			*p++ = CanFrameCount&0xff;
			CanFrameTotoalLength += 2;
			RadioProtocol_CAN_UpTrans(CHANNEL_DATA_1|CHANNEL_DATA_2,CanUploadBuffer,CanFrameTotoalLength);
			CanFrameCount = 0;
			CanFrameTotoalLength = 0;
			SendPacketCount++;
		}
	}
	return ENABLE;
}
/**************************************************************************
//函数名：Can_UpdatePram
//功能：更新CAN1相关变量
//输入：无
//输出：无
//返回值：无
//备注：目前只使用了CAN1
***************************************************************************/
void Can_UpdatePram(void)
{
	u8 Buffer[10];
	u8 i;

	if(E2_CAN1_GATHER_TIME_LEN == EepromPram_ReadPram(E2_CAN1_GATHER_TIME_ID,Buffer))
	{
		Can1GatherTime = Public_ConvertBufferToLong(Buffer);
		if(0 == Can1GatherTime)
		{
			//为编译不报警告而添加
		}
	}

	if(E2_CAN1_UPLOAD_TIME_LEN == EepromPram_ReadPram(E2_CAN1_UPLOAD_TIME_ID,Buffer))
	{
		Can1UploadTime = Public_ConvertBufferToShort(Buffer);
	}

	for(i=0; i<CAN_FILTER_ID_NUM; i++)
	{
		CanFilter[i].GatherTime = 0;
		CanFilter[i].channel = 0;
		CanFilter[i].FrameType = 0;
		CanFilter[i].GatherMode = 0;
		CanFilter[i].Id = 0;
	}

	for(i=0; i<CAN_FILTER_ID_NUM; i++)
	{
	  if(E2_CAN_SET_ONLY_GATHER_LEN == EepromPram_ReadPram(E2_CAN_SET_ONLY_GATHER_0_ID+i,Buffer))
	  {
		  CanFilter[i].GatherTime = Public_ConvertBufferToLong(Buffer);
		  CanFilter[i].channel = (Buffer[4]&0x80) >> 7;
		  CanFilter[i].FrameType = (Buffer[4]&0x40) >> 6;
		  CanFilter[i].GatherMode = (Buffer[4]&0x20) >> 5;
		  CanFilter[i].Id = 0;
		  CanFilter[i].Id |= (Buffer[4]&0x1f) << 24;
		  CanFilter[i].Id |= Buffer[5] << 16;
		  CanFilter[i].Id |= Buffer[6] << 8;
		  CanFilter[i].Id |= Buffer[7];
	 }
  }
}
/**************************************************************************
//函数名：Can_SoftwareFilter
//功能：软件滤波
//输入：无
//输出：无
//返回值：1表示需要被滤除（过滤），0表示不需要滤除
//备注：通道号channel，0为CAN1，1为CAN2;帧类型FrameType，0为标准帧，1为扩展帧
***************************************************************************/
u8 Can_SoftwareFilter( u8 channel, u32 CanId, u8 FrameType)
{
	u8 i;

	for(i=0; i<CAN_FILTER_ID_NUM; i++)
	{
		if((CanId == CanFilter[i].Id)&&(FrameType == CanFilter[i].FrameType)&&(channel == CanFilter[i].channel))
		{
			break;
		}
	}

	if(i >= CAN_FILTER_ID_NUM)
	{
		
	}
	else
	{
		if(0 == CanFilter[i].GatherTime)
		{
			return 1;
		}
		else
		{

		}
	}

	return 0;
}





















