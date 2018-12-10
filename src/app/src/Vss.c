/************************************************************************
//�������ƣ�Vss.c
//���ܣ�VSS�����쳣�ж�
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2016.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V1.0:
//1.��GPS�ٶ�һֱ��15km/h����ʱ���ж������ٶȣ������ٶȳ���1���Ӵ���0����
//   �����ٶȴ����������ӣ����VSS���ϱ������������ٶȳ���1���ӵ���0���������ٶȴ�����û��
//   ���ӣ���λVSS���ϱ�����
//2.���ٶ�����ѡ��Ϊ�Զ��ٶ�����ʱ��SpeedFlagֵΪ0x03��0x02����VSS����ʱ�Ὣ�ٶ�����
//   ����Ϊ0x03��VSS��������ʱ��û��VSS���ϱ������Ὣ�ٶ���������Ϊ0x02
*************************************************************************/
/********************�ļ�����*************************/
#include "Vss.h"
#include "Io.h"
#include "VDR_Doubt.h"
#include "Gps_App.h"
#include "EepromPram.h"
#include "other.h"
#include "VDR_Pulse.h"

/********************���ر���*************************/

/********************ȫ�ֱ���*************************/

/********************�ⲿ����*************************/
extern u8  SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS,2�Զ�����,3�Զ�GPS

/********************���غ�������*********************/

/********************��������*************************/

/**************************************************************************
//��������Vss_TimeTask
//���ܣ�VSS�����쳣�ж�
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��VSS�����쳣��ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
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
				if(PulseStopCount >= 60)//ͣ��60��
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
				if(PulseRunCount >= 60)//��ʻ60��
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








