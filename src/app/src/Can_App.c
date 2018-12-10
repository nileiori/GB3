/************************************************************************
//�������ƣ�Can_App.c
//���ܣ���ģ��ʵ��CAN�����ϱ��������ܡ�
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2014.12
//�汾��¼���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
*************************************************************************/

/********************�ļ�����*************************/
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


/********************���ر���*************************/
static u8  CanUploadBuffer[CAN_UPLOAD_BUFFER_SIZE] = {0};

/********************ȫ�ֱ���*************************/
CAN_FILTER_STRUCT CanFilter[CAN_FILTER_ID_NUM];
u32 Can1GatherTime = 1;	//CAN1 �ɼ�ʱ��������λΪms��Ϊ0��ʾ���ɼ���Ĭ���ǲɼ���
u16 Can1UploadTime = 0;	//CAN1 �ϴ�ʱ������Ϊ0��ʾ���ϴ�

/********************�ⲿ����*************************/
extern Queue CanRxQ;
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

/********************���غ�������*********************/

/********************��������*************************/
/**************************************************************************
//��������Can_TimeTask
//���ܣ�ʵ��CAN�����ϱ�����
//���룺��
//�������
//����ֵ����
//��ע��CAN�����ϱ���ʱ����50ms����1�Σ������������Ҫ���ô˺�����
//CAN�ӿ�ÿ�յ�һ֡�������ж��Ƿ���Ҫ����˳��������˳��������ն��У�
//������δ��ȡ�����ݳ��ȳ�����CAN_RX_BUFFER_SIZE-200���ֽڻ����ϴ�ʱ������ʱ��
//�ϴ���ƽ̨�����ϴ�ʱ��������Ϊ0ʱ��ʾ���ϴ���
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
	u8 CanFrameCount = 0;//֡��������
	u8 CanFrameLength = 0;//ÿһ֡���ȼ���
	u16 CanFrameTotoalLength = 0;//��֡�ۼƳ���
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
		p = CanUploadBuffer+2;//ͷ2���ֽ�Ԥ����������
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
								if(1 == BBGNTestFlag)//dxl,2016.5.23���Ҫ���ϴ��Ľ���ʱ�������10��
								{
								    if(0 == SendPacketCount)
										{
										    memcpy(p,Buffer,17);//��1��CAN���ݰ���ʱ����Ϣ
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
											    
											  memcpy(p,Buffer,17);//��1��CAN���ݰ���ʱ����Ϣ
								        p = p+17;
								        CanFrameTotoalLength += 17;
										}
								}
								else
								{
								    memcpy(p,Buffer,17);//��1��CAN���ݰ���ʱ����Ϣ
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
			if(CanFrameCount >= 50)//ÿ�����ֻ�ܴ��50��CAN����֡
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
//��������Can_UpdatePram
//���ܣ�����CAN1��ر���
//���룺��
//�������
//����ֵ����
//��ע��Ŀǰֻʹ����CAN1
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
			//Ϊ���벻����������
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
//��������Can_SoftwareFilter
//���ܣ�����˲�
//���룺��
//�������
//����ֵ��1��ʾ��Ҫ���˳������ˣ���0��ʾ����Ҫ�˳�
//��ע��ͨ����channel��0ΪCAN1��1ΪCAN2;֡����FrameType��0Ϊ��׼֡��1Ϊ��չ֡
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





















