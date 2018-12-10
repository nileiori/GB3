/************************************************************************
//�������ƣ�VDR_Overspeed.c
//���ܣ����ٱ���
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.7
//�汾��¼��
//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "EepromPram.h"

/********************���ر���*************************/
static	VDR_OVERSPEED_STRUCT	VdrOverspeed;
static 	u8	VdrOverspeedAlarmFlag = 0;//���ٱ�����־����ӦλΪ1��ʾ�����˸ñ�����Ϊ0��ʾ�ñ�������ˡ�
//�ڲ������ٱ�������ʱ�����������һ��bit��λ�ˣ����г��ٱ�����������Ҫ�������γ��ٱ��������������ظ�������
//bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
static	u8	VdrOverspeedEarlyAlarmFlag = 0;//����Ԥ����־����ӦλΪ1��ʾ�����˸�Ԥ����Ϊ0��ʾ��Ԥ������ˡ�
//�ڲ�������Ԥ������ʱ�����������һ��bit��λ�ˣ����г���Ԥ����������Ҫ�������γ���Ԥ�������������ظ�������
//bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����

static  u32 VdrOverspeedEarlyAlramCount = 0;
static  u32 VdrOverspeedAlramCount = 0;
/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static u8 VDROverspeed_IsAlarm(void);
static u8 VDROverspeed_IsEarlyAlarm(void);
static void VDROverspeed_Init(void);
static u8 VDROverspeed_ReadEarlyAlarmFlag(void);
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
						//Public_PlayTTSVoiceAlarmStr(VdrOverspeed.AlarmVoiceBuffer);
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
							//Public_PlayTTSVoiceAlarmStr(VdrOverspeed.EarlyAlarmVoiceBuffer);
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
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, 0);
		}
	}
	else
	{
		if(0 == VdrOverspeedAlarmFlag)
		{
			VdrOverspeedAlarmFlag |= 1<<bit;
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, 1);
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
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, 0);
		}
	}
	else
	{
		if(0 == VdrOverspeedEarlyAlarmFlag)
		{
			VdrOverspeedEarlyAlarmFlag |= 1<<bit;
//			Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED_PRE_ALARM, 1);
		}
	}
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

	Speed = VDRPulse_GetSecondSpeed();
	if(Speed > VdrOverspeed.AlarmSpeed)
	{
		VdrOverspeedAlramCount++;
		if(VdrOverspeedAlramCount >= VdrOverspeed.AlarmKeeptime)
		{
			VdrOverspeedEarlyAlramCount = 0;//Ԥ��������0
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

	Speed = VDRPulse_GetSecondSpeed();
	if(Speed > VdrOverspeed.EarlyAlramSpeed)
	{
		VdrOverspeedEarlyAlramCount++;
		if(VdrOverspeedEarlyAlramCount >= VdrOverspeed.EarlyAlramKeeptime)
		{
			VDROverspeed_WriteEarlyAlarmFlagBit(0, 1);//Ԥ��������1
			return 1;
		}
	}
	else
	{
		VdrOverspeedEarlyAlramCount = 0;
	}

	VDROverspeed_WriteEarlyAlarmFlagBit(0, 0);//Ԥ��������0

	return 0;
}
/**************************************************************************
//��������VDROverspeed_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ����
//��ע����Ҫ�Ǹ�����ر���
***************************************************************************/
static void VDROverspeed_Init(void)
{
	u8	Buffer[40];
	u16	BufferLen;
	u16	temp;

	BufferLen = EepromPram_ReadPram(E2_MAX_SPEED_ID, Buffer);//���ٱ�������ֵ
	if(BufferLen == E2_MAX_SPEED_LEN)
	{
		VdrOverspeed.AlarmSpeed = Buffer[3];//һ��ǰ3�ֽھ�Ϊ0
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
		if(temp < VdrOverspeed.AlarmSpeed)
		{
			VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - (temp&0xff);
		}
		
	}
	else
	{
		VdrOverspeed.EarlyAlramSpeed = VdrOverspeed.AlarmSpeed - 5;
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

	
}





