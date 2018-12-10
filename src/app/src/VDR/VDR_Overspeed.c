/************************************************************************
//�������ƣ�VDR_Overspeed.c
//���ܣ����ٱ���
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��
//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "Public.h"
#include "Io.h"
#include "Report.h"
#include "modem_app_com.h"
#include "other.h"

#include <stdio.h>

/********************���ر���*************************/
static	AREA_ROAD_OVERSPEED_STRUCT	AreaRoadOverspeed[5];
static	VDR_OVERSPEED_STRUCT	VdrOverspeed;
static 	u8	VdrOverspeedAlarmFlag = 0;//���ٱ�����־����ӦλΪ1��ʾ�����˸ñ�����Ϊ0��ʾ�ñ�������ˡ�
//�ڲ������ٱ�������ʱ�����������һ��bit��λ�ˣ����г��ٱ�����������Ҫ�������γ��ٱ��������������ظ�������
//bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
static	u8	VdrOverspeedEarlyAlarmFlag = 0;//����Ԥ����־����ӦλΪ1��ʾ�����˸�Ԥ����Ϊ0��ʾ��Ԥ������ˡ�
//�ڲ�������Ԥ������ʱ�����������һ��bit��λ�ˣ����г���Ԥ����������Ҫ�������γ���Ԥ�������������ظ�������
//bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����

static  u32 VdrOverspeedEarlyAlarmCount = 0;
static  u32 VdrOverspeedAlramCount = 0;
static  u8 VdrOverspeedAlarmSpeedOffset = 5;//����Ԥ����ֵ
/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ

/********************���غ�������*********************/
static u8 VDROverspeed_IsAlarm(void);
static u8 VDROverspeed_IsEarlyAlarm(void);
static void VDROverspeed_Init(void);
static u8 VDROverspeed_ReadEarlyAlarmFlag(void);
static void VDROverspeed_OnceOverReport(void);
static u8 VDROverspeed_NightIsArrived(void);
static u8 VDROverspeed_NightTimeIsOk(TIME_T Time);
/********************��������*************************/

/**************************************************************************
//��������VDROverspeed_TimeTask
//���ܣ����ٱ�����Ԥ��
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
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
		
		VDROverspeed_OnceOverReport();//dxl,2016.3.21 ����Э����ʱ���������������ã��ص��ⳬ�ٿ�ʼʱ�̺ͳ��ٱ���ʱ��
		
		if(1 == VDROverspeed_IsAlarm())//�г��ٱ���
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
						Public_PlayTTSVoiceStr(VdrOverspeed.AlarmVoiceBuffer);
					}
					else
					{
						//���������͵ĳ��ٱ������򲻲�������
					}
				}
				AlarmVoiceNumTimeCount++;
			}
			
		}
		else//�޳��ٱ���������£��ڿ����޳���Ԥ����
		{
			AlarmVoiceCount = 0;
			AlarmVoiceNumCount = 0;
			AlarmVoiceNumTimeCount = 0; 

			if(1 == VDROverspeed_IsEarlyAlarm())//�г���Ԥ��
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
							Public_PlayTTSVoiceStr(VdrOverspeed.EarlyAlarmVoiceBuffer);
						}
						else
						{
							//���������͵ĳ��ٱ������򲻲�������
						}
					}
					EarlyAlarmVoiceNumTimeCount++; 
					
				}
			}
			else//�޳���Ԥ��
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
//��������VDROverspeed_UpdateParameter
//���ܣ����²���
//���룺��
//�������
//����ֵ����
//��ע����Ҫ�Ǹ�����ر���
***************************************************************************/
void VDROverspeed_UpdateParameter(void)
{
	u8	Buffer[40];
	u16	BufferLen;
	u16	temp;

	BufferLen = EepromPram_ReadPram(E2_MAX_SPEED_ID, Buffer);//���ٱ�������ֵ
	if(BufferLen == E2_MAX_SPEED_LEN)
	{
		VdrOverspeed.AlarmSpeed = Public_ConvertBufferToLong(Buffer);//һ��ǰ3�ֽھ�Ϊ0
		if(VdrOverspeed.AlarmSpeed > 220)
		{
		    VdrOverspeed.AlarmSpeed = 100;
		}
	}
	else
	{
		VdrOverspeed.AlarmSpeed = 100;
	}

	BufferLen = EepromPram_ReadPram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer);//���ٱ�������ʱ��
	if(BufferLen == E2_OVER_SPEED_KEEP_TIME_LEN)
	{
		VdrOverspeed.AlarmKeeptime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmKeeptime = 10;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_VOICE_ID, Buffer);//���ٱ�������
	if(0 != BufferLen)
	{
		Buffer[BufferLen] = 0;
		strcpy((char *)VdrOverspeed.AlarmVoiceBuffer,(const char *)Buffer);
	}
	else
	{
		strcpy((char *)VdrOverspeed.AlarmVoiceBuffer,"���ѳ��٣��������ʻ");
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_GPROUP_TIME_ID, Buffer);//���ٱ���ÿ��������ʾʱ����
	if(BufferLen == E2_OVERSPEED_ALARM_GPROUP_TIME_ID_LEN)
	{
		VdrOverspeed.AlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmVoiceGroupTime = 5;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_NUMBER_ID, Buffer);//���ٱ���ÿ��������ʾ����
	if(BufferLen == E2_OVERSPEED_ALARM_NUMBER_ID_LEN)
	{
		VdrOverspeed.AlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmVoiceNum = 3;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_NUMBER_TIME_ID, Buffer);//���ٱ���ÿ��������ʾʱ����
	if(BufferLen == E2_OVERSPEED_ALARM_NUMBER_TIME_ID_LEN)
	{
		VdrOverspeed.AlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.AlarmVoiceNumTime = 10;
	}


	////////////////////////////////////////////////////////////
	BufferLen = EepromPram_ReadPram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer);//����Ԥ������ֵ
	if(BufferLen == E2_SPEED_EARLY_ALARM_DVALUE_LEN)
	{
		temp = Public_ConvertBufferToShort(Buffer);
		temp = temp/10;
		VdrOverspeedAlarmSpeedOffset = temp&0xff;
		if(VdrOverspeed.AlarmSpeed > temp)
		{
			VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - VdrOverspeedAlarmSpeedOffset;
		}
		else
		{
			VdrOverspeedAlarmSpeedOffset = 5;
		  VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - VdrOverspeedAlarmSpeedOffset;
		}
		
	}
	else
	{
		VdrOverspeedAlarmSpeedOffset = 5;
		VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - VdrOverspeedAlarmSpeedOffset;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_KEEPTIME_ID, Buffer);//����Ԥ������ʱ��
	if(BufferLen == E2_OVERSPEED_PREALARM_KEEPTIME_ID_LEN)
	{
		VdrOverspeed.EarlyAlramKeeptime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlramKeeptime = 3;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_VOICE_ID, Buffer);//����Ԥ������
	if(0 != BufferLen)
	{
		Buffer[BufferLen] = 0;
		strcpy((char *)VdrOverspeed.EarlyAlarmVoiceBuffer,(const char *)Buffer);
	}
	else
	{
		strcpy((char *)VdrOverspeed.EarlyAlarmVoiceBuffer,"����Ƴ���");
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_GPROUP_TIME_ID, Buffer);//����Ԥ��ÿ��������ʾʱ����
	if(BufferLen == E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_LEN)
	{
		VdrOverspeed.EarlyAlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlarmVoiceGroupTime = 5;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_NUMBER_ID, Buffer);//����Ԥ��ÿ��������ʾ����
	if(BufferLen == E2_OVERSPEED_PREALARM_NUMBER_ID_LEN)
	{
		VdrOverspeed.EarlyAlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlarmVoiceNum = 3;
	}

	BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_NUMBER_TIME_ID, Buffer);//����Ԥ��ÿ��������ʾʱ����
	if(BufferLen == E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_LEN)
	{
		VdrOverspeed.EarlyAlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
	}
	else
	{
		VdrOverspeed.EarlyAlarmVoiceNumTime = 10;
	}
	
	  BufferLen = EepromPram_ReadPram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer);//ҹ��ʱ�䣬ҹ�䳬��ʹ��
    if(E2_OVERSPEED_NIGHT_TIME_ID_LEN == BufferLen)
    {
			  VdrOverspeed.NightTimeAttribute = (Buffer[0] << 8)|Buffer[1];  
			
        VdrOverspeed.NightStartTime.year = VDRPub_BCD2HEX(Buffer[2]);
	      VdrOverspeed.NightStartTime.month = VDRPub_BCD2HEX(Buffer[3]);
	      VdrOverspeed.NightStartTime.day = VDRPub_BCD2HEX(Buffer[4]);
	      VdrOverspeed.NightStartTime.hour = VDRPub_BCD2HEX(Buffer[5]);
	      VdrOverspeed.NightStartTime.min = VDRPub_BCD2HEX(Buffer[6]);
	      VdrOverspeed.NightStartTime.sec = VDRPub_BCD2HEX(Buffer[7]);
	
	      VdrOverspeed.NightEndTime.year = VDRPub_BCD2HEX(Buffer[8]);
	      VdrOverspeed.NightEndTime.month = VDRPub_BCD2HEX(Buffer[9]);
	      VdrOverspeed.NightEndTime.day = VDRPub_BCD2HEX(Buffer[10]);
	      VdrOverspeed.NightEndTime.hour = VDRPub_BCD2HEX(Buffer[11]);
	      VdrOverspeed.NightEndTime.min = VDRPub_BCD2HEX(Buffer[12]);
	      VdrOverspeed.NightEndTime.sec = VDRPub_BCD2HEX(Buffer[13]);
    }
    else
    {
        VdrOverspeed.NightTimeAttribute = 0;  
			
        VdrOverspeed.NightStartTime.year = 0;
	      VdrOverspeed.NightStartTime.month = 0;
	      VdrOverspeed.NightStartTime.day = 0;
	      VdrOverspeed.NightStartTime.hour = 0;
	      VdrOverspeed.NightStartTime.min = 0;
	      VdrOverspeed.NightStartTime.sec = 0;
	
	      VdrOverspeed.NightEndTime.year = 0;
	      VdrOverspeed.NightEndTime.month = 0;
	      VdrOverspeed.NightEndTime.day = 0;
	      VdrOverspeed.NightEndTime.hour = 0;
	      VdrOverspeed.NightEndTime.min = 0;
	      VdrOverspeed.NightEndTime.sec = 0;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_OVERSPEED_PERCENT_ID, Buffer);//ҹ�䳬�ٱ�����Ԥ���ٷֱ�
	  if(BufferLen == E2_NIGHT_OVERSPEED_PERCENT_ID_LEN)
	  {
		    VdrOverspeed.NightSpeedPercent = Public_ConvertBufferToLong(Buffer);
			  if(VdrOverspeed.NightSpeedPercent > 100)
				{
				    VdrOverspeed.NightSpeedPercent = 80;  
				}
	  }
	  else
	  {
		    VdrOverspeed.NightSpeedPercent = 80;
	  }
		
		
}
/**************************************************************************
//��������VDROverspeed_SetAreaParameter
//���ܣ�������������ز���
//���룺
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDROverspeed_SetAreaParameter(u8 Type, u32 Id, u8 MaxSpeed, u8 KeepTime)
{
	if((0 == Type)||(Type > 3))
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 1;
	AreaRoadOverspeed[Type].Id = Id;
	AreaRoadOverspeed[Type].MaxSpeed = MaxSpeed;
	AreaRoadOverspeed[Type].KeepTime = KeepTime;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = MaxSpeed - VdrOverspeedAlarmSpeedOffset;
	AreaRoadOverspeed[Type].EarlyKeepTime = KeepTime;
}
/**************************************************************************
//��������VDROverspeed_ResetAreaParameter
//���ܣ������������ز���
//���룺
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDROverspeed_ResetAreaParameter(u8 Type)
{
	if((0 == Type)||(Type > 3))
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 0;
	AreaRoadOverspeed[Type].Id = 0;
	AreaRoadOverspeed[Type].MaxSpeed = 0;
	AreaRoadOverspeed[Type].KeepTime = 0;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = 0;
	AreaRoadOverspeed[Type].EarlyKeepTime = 0;
}
/**************************************************************************
//��������VDROverspeed_SetRoadParameter
//���ܣ�������·������ز���
//���룺
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDROverspeed_SetRoadParameter(u8 Type, u32 Id, u8 MaxSpeed, u8 KeepTime)
{
	if(4 != Type)
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 1;
	AreaRoadOverspeed[Type].Id = Id;
	AreaRoadOverspeed[Type].MaxSpeed = MaxSpeed;
	AreaRoadOverspeed[Type].KeepTime = KeepTime;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = MaxSpeed - VdrOverspeedAlarmSpeedOffset;
	AreaRoadOverspeed[Type].EarlyKeepTime = KeepTime;
}
/**************************************************************************
//��������VDROverspeed_ResetRoadParameter
//���ܣ������·������ز���
//���룺
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDROverspeed_ResetRoadParameter(u8 Type)
{
	if(4 != Type)
	{
	  return ;
	}

  AreaRoadOverspeed[Type].Valid = 0;
	AreaRoadOverspeed[Type].Id = 0;
	AreaRoadOverspeed[Type].MaxSpeed = 0;
	AreaRoadOverspeed[Type].KeepTime = 0;
	AreaRoadOverspeed[Type].EarlyMaxSpeed = 0;
	AreaRoadOverspeed[Type].EarlyKeepTime = 0;
}
/**************************************************************************
//��������VDROverspeed_ReadAlarmFlag
//���ܣ������ٱ�����־�ֽ�
//���룺��
//�������
//����ֵ����
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
***************************************************************************/
u8 VDROverspeed_ReadAlarmFlag(void)
{
	return VdrOverspeedAlarmFlag;
}
/**************************************************************************
//��������VDROverspeed_ReadAlarmFlagBit
//���ܣ������ٱ�����־��ĳ��λ
//���룺��
//�������
//����ֵ����������1����0��������0xff��ʾ��������
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
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
//��������VDROverspeed_WriteAlarmFlagBit
//���ܣ�д���ٱ�����־��ĳ��λ
//���룺bit:0-7;value:0-1;
//�������
//����ֵ����������1����0��������0xff��ʾ��������
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
////����ĳ�����͵ı��������ˣ���������λд�뽫���ɹ�����ͬһʱ��ֻ����һ�ֳ��ٱ�����
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
			//Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED,RESET); dxl,2016.5.6
			Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT_ORDINARY,RESET);
		}
	}
	else
	{
		if(0 == VdrOverspeedAlarmFlag)
		{
			VdrOverspeedAlarmFlag |= 1<<bit;
			//Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, SET);
			Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT_ORDINARY,SET);
		}
	}
}
/**************************************************************************
//��������VDROverspeed_ReadEarlyAlarmFlag
//���ܣ�������Ԥ����־�ֽ�
//���룺��
//�������
//����ֵ����
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
***************************************************************************/
static u8 VDROverspeed_ReadEarlyAlarmFlag(void)
{
	return VdrOverspeedEarlyAlarmFlag;
}
/**************************************************************************
//��������VDROverspeed_ReadEarlyAlarmFlagBit
//���ܣ�������Ԥ����־��ĳ��λ
//���룺��
//�������
//����ֵ����������1����0��������0xff��ʾ��������
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
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
//��������VDROverspeed_WriteEarlyAlarmFlagBit
//���ܣ�д����Ԥ����־��ĳ��λ
//���룺bit:0-7;value:0-1;
//�������
//����ֵ����������1����0��������0xff��ʾ��������
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
////����ĳ�����͵ı��������ˣ���������λд�뽫���ɹ�����ͬһʱ��ֻ����һ�ֳ��ٱ�����
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
			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, RESET);
		}
	}
	else
	{
		if(0 == VdrOverspeedEarlyAlarmFlag)
		{
			VdrOverspeedEarlyAlarmFlag |= 1<<bit;
			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, SET);
		}
	}
}
/**************************************************************************
//��������VDROverspeed_GetAlarmSpeed
//���ܣ���ȡ����ֵ
//���룺��
//�������
//����ֵ������ֵ
//��ע����
***************************************************************************/
u8 VDROverspeed_GetAlarmSpeed(void)
{
  return VdrOverspeed.AlarmSpeed;
}
/**************************************************************************
//��������VDROverspeed_IsAlarm
//���ܣ��Ƿ��ٱ���
//���룺��
//�������
//����ֵ��1:��;0:��
//��ע����
***************************************************************************/
static u8 VDROverspeed_IsAlarm(void)
{
	u8	Speed;
	u8  i;
	u8  j;
	u8  SpeedMax;
	u32 KeepTime;
	u8  Buffer[50];
	
	static u8 LastSpeedMax = 0;
	static u8 TtsPlayFlag = 0;
	static u8 TtsPlayDelay = 0;

	
	AreaRoadOverspeed[0].MaxSpeed = VdrOverspeed.AlarmSpeed;//��ͨ����ֵ
	AreaRoadOverspeed[0].KeepTime = VdrOverspeed.AlarmKeeptime;
  AreaRoadOverspeed[0].Valid = 1;
	Speed = VDRSpeed_GetCurSpeed();
	SpeedMax = 0xff;
	j = 0;
	for(i=0; i<5; i++)
	{
		if(1 == AreaRoadOverspeed[i].Valid)
	  {
      if((AreaRoadOverspeed[i].MaxSpeed < SpeedMax)&&(AreaRoadOverspeed[i].MaxSpeed > 0))
	    {
		    SpeedMax = AreaRoadOverspeed[i].MaxSpeed;
	      j = i;
	    }
	  }
  }
	KeepTime = AreaRoadOverspeed[j].KeepTime;
			
	if(1 == VDROverspeed_NightIsArrived())
	{
			SpeedMax = SpeedMax*VdrOverspeed.NightSpeedPercent/100;
	}
	else
	{
	    //��
	}
	
	if((SpeedMax != LastSpeedMax)&&(0 != SpeedMax)&&(Speed > 0))
	{
		  TtsPlayFlag = 1;
		  TtsPlayDelay = 0;
	    
	}
	
	if(1 == TtsPlayFlag)
	{
	    TtsPlayDelay++;
		  if(TtsPlayDelay >= 15)
			{
			    TtsPlayFlag = 0;
				  TtsPlayDelay = 0;
				  sprintf((char *)Buffer,"��ǰ����%d����",SpeedMax); 
		      Public_PlayTTSVoiceStr(Buffer);
          Public_ShowTextInfo((char *)Buffer,100);
			}
	}
	
	if((Speed > SpeedMax)&&(0 != SpeedMax))
	{
		VdrOverspeedAlramCount++;
		if(VdrOverspeedAlramCount >= KeepTime)
		{
			VdrOverspeedEarlyAlarmCount = 0;//Ԥ��������0
			VDROverspeed_WriteEarlyAlarmFlagBit(0, 0);//Ԥ��������0
			VDROverspeed_WriteAlarmFlagBit(0, 1);//���ٱ�����1
			
			return 1;
		}
	}
	else
	{
		VdrOverspeedAlramCount = 0;
	}

	VDROverspeed_WriteAlarmFlagBit(0, 0);//���ٱ�����0
	
	LastSpeedMax = SpeedMax;

	return 0;
}
/**************************************************************************
//��������VDROverspeed_IsEarlyAlarm
//���ܣ��Ƿ���Ԥ��
//���룺��
//�������
//����ֵ��1:��;0:��
//��ע����
***************************************************************************/
static u8 VDROverspeed_IsEarlyAlarm(void)
{
	u8	Speed;
	u8  SpeedMax;
	u8  i;
	u8  j;
	u32 KeepTime;

	AreaRoadOverspeed[0].EarlyMaxSpeed = VdrOverspeed.EarlyAlramSpeed;//��ͨ����ֵ
	AreaRoadOverspeed[0].EarlyKeepTime = VdrOverspeed.EarlyAlramKeeptime;
  AreaRoadOverspeed[0].Valid = 1;
	Speed = VDRSpeed_GetCurSpeed();
	SpeedMax = 0xff;
	j = 0;
	for(i=0; i<5; i++)
	{
		if(1 == AreaRoadOverspeed[i].Valid)
	  {
      if((AreaRoadOverspeed[i].EarlyMaxSpeed < SpeedMax)&&(AreaRoadOverspeed[i].EarlyMaxSpeed > 0))
	    {
		    SpeedMax = AreaRoadOverspeed[i].EarlyMaxSpeed;
	      j = i;
	    }
	  }
  }
	KeepTime = AreaRoadOverspeed[j].EarlyKeepTime;
	
	if(1 == VDROverspeed_NightIsArrived())
	{
			SpeedMax = SpeedMax*VdrOverspeed.NightSpeedPercent/100;
	}
	else
	{
	    //��
	}
	if(Speed > SpeedMax)
	{
		VdrOverspeedEarlyAlarmCount++;
		if(VdrOverspeedEarlyAlarmCount >= KeepTime)
		{
			VDROverspeed_WriteEarlyAlarmFlagBit(0, 1);//Ԥ��������1
			return 1;
		}
	}
	else
	{
		VdrOverspeedEarlyAlarmCount = 0;
	}

	VDROverspeed_WriteEarlyAlarmFlagBit(0, 0);//Ԥ��������0

	return 0;
}
/**************************************************************************
//��������VDROverspeed_Init
//���ܣ���ʼ��
//���룺��
//�������
//����ֵ����
//��ע����Ҫ�Ǹ�����ر���
***************************************************************************/
static void VDROverspeed_Init(void)
{
	u8 i;
	
  VDROverspeed_UpdateParameter();
	
	for(i=0; i<5; i++)
	{
	  AreaRoadOverspeed[i].Valid = 0;
	  AreaRoadOverspeed[i].Id = 0;
	  AreaRoadOverspeed[i].MaxSpeed = 0;
	  AreaRoadOverspeed[i].KeepTime = 0;
		AreaRoadOverspeed[i].EarlyMaxSpeed = 0;
	  AreaRoadOverspeed[i].EarlyKeepTime = 0;
	}
}
/**************************************************************************
//��������VDROverspeed_NightIsArrived
//���ܣ��жϵ�ǰ�Ƿ�Ϊҹ��ʱ��
//���룺��
//�������
//����ֵ��1����ǰ��ҹ��ʱ�䣻0����ǰ����ҹ��ʱ��
//��ע��
***************************************************************************/
static u8 VDROverspeed_NightIsArrived(void)
{
	  TIME_T CurTime;
	  u32 TimeCount;
	  u32 NightStartTimeCount;
	  u32 NightEndTimeCount;
	
	  if(1 != VDROverspeed_NightTimeIsOk(VdrOverspeed.NightStartTime))
		{
		    return 0;
		}
		
		if(1 != VDROverspeed_NightTimeIsOk(VdrOverspeed.NightEndTime))
		{
		    return 0;
		}
		
		if(0 == VdrOverspeed.NightSpeedPercent)
		{
		    return 0;
		}
		
		RTC_GetCurTime(&CurTime);
		NightStartTimeCount = VdrOverspeed.NightStartTime.hour*3600+VdrOverspeed.NightStartTime.min*60+VdrOverspeed.NightStartTime.sec;
		NightEndTimeCount = VdrOverspeed.NightEndTime.hour*3600+VdrOverspeed.NightEndTime.min*60+VdrOverspeed.NightEndTime.sec;
		TimeCount = CurTime.hour*3600+CurTime.min*60+CurTime.sec;
		if(NightStartTimeCount < NightEndTimeCount)
		{
		    if((TimeCount > NightStartTimeCount)&&(TimeCount < NightEndTimeCount))
				{
				    return 1;
				}
				else
				{
				    return 0;
				}
		}
		else if(NightStartTimeCount > NightEndTimeCount)
		{
		    if((TimeCount > NightStartTimeCount)&&(TimeCount < (24*3600)))
				{
				    return 1;
				}
				else if(TimeCount < NightEndTimeCount)
				{
				    return 1;
				}
				else
				{
				    return 0;
				}
		}
		else
		{
		    return 0;
		}
}
/**************************************************************************
//��������VDROverspeed_NightTimeIsOk
//���ܣ����ҹ��ʱ���Ƿ���ȷ
//���룺��
//�������
//����ֵ��1����ȷ��0������ȷ
//��ע��
***************************************************************************/
static u8 VDROverspeed_NightTimeIsOk(TIME_T Time)
{
    if((Time.hour > 23)||(Time.hour < 0)||
    (Time.min > 59)||(Time.min < 0)||
    (Time.sec > 59)||(Time.sec < 0))
    {
        return 0;
    }
		else
		{
		    return 1;
		}
}
///////////////////������Ϊ�����ͼ������///////////////////////
/**************************************************************************
//��������VDROverspeed_OnceOverReport
//���ܣ�����Э���ⳬ�ٹ���ʱҪ���ٶ�һ����������ֵ�����ϱ�
//���룺��
//�������
//����ֵ����
//��ע���ص��ǳ��ٿ�ʼʱ�̣����ٱ���ʱ��
***************************************************************************/
static void VDROverspeed_OnceOverReport(void)
{
    u8	Speed;
    static u8 ReportFlag = 0;
	  static u8 count = 0;
	
	
	  if(0 == BBXYTestFlag)
		{
		    return ;
		}
	
	  Speed = VDRSpeed_GetCurSpeed();
	  if((Speed >= VdrOverspeed.AlarmSpeed)&&(0 == ReportFlag))
	  {
		    Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);
			  ReportFlag = 1;
			  count++;
			  if(2 == count)
				{
				    count = 0;
				}
	  }
		else if(Speed < VdrOverspeed.AlarmSpeed)
		{
			  ReportFlag = 0;
		}
}
/**************************************************************************
//��������VDROverspeed_ClearTime
//���ܣ����ٳ���ʱ����0
//���룺��
//�������
//����ֵ����
//��ע������Э���ⳬ��ʱ�����ó�������ֵʱ����ô˺���
***************************************************************************/
void VDROverspeed_ClearTime(void)
{
    VdrOverspeedEarlyAlarmCount = 0;
	  VdrOverspeedAlramCount = 0;
}






