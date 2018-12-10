/************************************************************************
//�������ƣ�VDR_SpeedStatus.c
//���ܣ��ٶ�״̬��¼��
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2014.10
//�汾��¼��
//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_rtc.h"

/********************���ر���*************************/
static u8	VdrSpeedStatusBuffer[VDR_DATA_SPEED_STATUS_STEP_LEN];
static u8	VdrSpeedStatusEnableFlag = 0;

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static void VDRSpeedStatus_Write(u8 SpeedStatus, u32 StartTime, u32 EndTime);
static void VDRSpeedStatus_Init(void);

/********************��������*************************/

/**************************************************************************
//��������VDRSpeedStatus_TimeTask
//���ܣ��ٶ�״̬��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
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

			if((GpsSpeed > 40)&&((100*DifferenceSpeed) >= (11*GpsSpeed)))//��ֵ����11%
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
			else if((GpsSpeed > 40)&&((100*DifferenceSpeed) < (11*GpsSpeed)))//��ֵС��11%
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
//��������VDRSpeedStatus_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ����
//��ע����Ҫ���ϵ���±���VdrSpeedStatusEnableFlag
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
//��������VDRSpeedStatus_Write
//���ܣ�����һ���ٶ�״̬��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
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

	VdrSpeedStatusBuffer[0] = SpeedStatus;//�ٶ�״̬��1�ֽڣ�0x01:����;0x02:�쳣

	Gmtime(&Time, StartTime);
	VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+1,&Time);//��ʼʱ��
				
	Gmtime(&Time, EndTime);
	VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+7,&Time);//����ʱ��

	VDRData_Write(VDR_DATA_TYPE_SPEED_STATUS, VdrSpeedStatusBuffer, VDR_DATA_SPEED_STEP_LEN-5, EndTime);


	VdrSpeedStatusEnableFlag = 0;
	Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
	FRAM_BufferWrite(Addr, &VdrSpeedStatusEnableFlag, 1);				
			
}






