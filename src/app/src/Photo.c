/************************************************************************
//�������ƣ�Photo.c
//���ܣ�ʵ���¼��������գ�������ʱ���������չ���
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2014.11
//�汾��¼���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1��ͨ������Camera.c�ṩ�����սӿ�Camera_Photoʵ���¼��������գ�������ʱ���������չ���
//�������ȼ��Ӹߵ��ͷֱ�Ϊ��ƽ̨�·�ָ�����գ��¼��������ա���ʱ�������ա�
//����������գ�ֻ���ڴ���������ȼ������������Żᴦ������ȼ�����������
*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include <string.h>

#include "Photo.h"
#include "EepromPram.h"
#include "Public.h"
#include "GPIOControl.h"
#include "Io.h"
#include "modem_lib.h"
#include "modem_app_com.h"
#include "Blind.h"
#include "Camera_App.h"
#include "SpeedMonitor.h"
//#include "Pulse_App.h"
#include "MileMeter.h"
#include "fm25c160.h"
#include "E2prom_25LC320A.h"
#include "other.h"
#include "VDR.h"

/********************���ر���*************************/
static u32 AlarmPhotoSwitch = 0;//�������տ��أ���ӦλΪ1��ʾʹ��
static u32 AlarmPhotoStoreFlag = 0;//�������մ洢��־����ӦλΪ1��ʾ�洢
static u32 StatusPhotoSwitch = 0;//״̬�仯���տ��أ���ӦλΪ1��ʾʹ��
static u32 StatusPhotoStoreFlag = 0;//״̬�仯���մ洢��־����ӦλΪ1��ʾ�洢

static u32 LastAlarm = 0;//��һ�α�����
static u32 LastStatus = 0;//��һ��״̬��
//static u32 LastExtCarStatus = 0;//��һ����չ����״̬��
//static u16 LastIoStatus = 0;//��һ��IO״̬��
//static u32 LastSelfDefine2 = 0;//��һ���Զ����2����״̬��

static u32 NextPhotoDistance = 0;//��һ�����վ���

static u32 TimePhotoControl = 0;//��ʱ���տ��Ʋ���
static u32 DistancePhotoControl = 0;//�������տ��Ʋ���
static u32 TimePhotoControlBackup = 0;//��ʱ���տ��Ʋ�������
static u32 DistancePhotoControlBackup = 0;//�������տ��Ʋ�������

static u32 TimePhotoSpace = 0;//��ʱ���ռ��
static u32 DistancePhotoSpace = 0;//�������ռ��
static u8  TimePhotoChannel = 0;//��ʱ����ͨ��
static u8  DistancePhotoChannel = 0;//��������ͨ��

/********************ȫ�ֱ���*************************/

/********************�ⲿ����*************************/
extern CAMERA_STRUCT Camera[];

/********************���غ�������*********************/
static u8 Photo_AlarmChange(void);
static u8 Photo_StatusChange(void);
static u8 Photo_TimeArive(void);
static u8 Photo_DistanceArive(void);
static u32 Photo_GetCurDistance(void);

/********************��������*************************/
/**************************************************************************
//��������Photo_TimeTask
//���ܣ�ʵ���¼��������գ�������ʱ���������չ���
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע�����ն�ʱ����1�����1�Σ������������Ҫ���ô˺�����
//�������ȼ��Ӹߵ��ͷֱ�Ϊ��ƽ̨�·�ָ�����գ��¼��������ա���ʱ�������ա�
//����������գ�ֻ���ڴ���������ȼ������������Żᴦ������ȼ�����������
//ͨ������Camera.c�ṩ�����սӿ�Camera_Photoʵ���¼��������գ�������ʱ���������չ���
//�����¼����ͣ�0Ϊƽ̨�·�ָ����ļ��ò����������ͣ���1Ϊ��ʱ��2Ϊ���٣�������������
//3Ϊ��ײ��෭��4Ϊ���ţ�5Ϊ���ţ�6Ϊ�����ɿ���أ�ʱ����<20km/h������20km/h��
//7Ϊ���࣬��������
***************************************************************************/
FunctionalState Photo_TimeTask(void)
{

	static u8 UpdatePramFlag = 0;

	if(0 == UpdatePramFlag)
	{
		UpdatePramFlag = 1;
		Photo_UpdatePram();
	}
     
	if(0 != Photo_AlarmChange())//����λ��λ�¼�����
	{
		return ENABLE;
	}

	if(0 != Photo_StatusChange())//״̬�仯�¼�����
	{
		return ENABLE;
	}

	if(0 != Photo_TimeArive())//������ʱ����
	{
		return ENABLE;
	}

	if(0 != Photo_DistanceArive())//������������
	{
		return ENABLE;
	}
        
       

	return ENABLE;
}
/**************************************************************************
//��������Photo_UpdatePram
//���ܣ��������ղ���
//���룺��
//�������
//����ֵ����
//��ע�������ö�ʱ���տ��ƣ��������տ��Ʋ���ʱ��Ҫ���ô˺���
***************************************************************************/
void Photo_UpdatePram(void)
{
	u8 Buffer[5];
	u8 flag = 0;
	u32 temp;
	u16 Address;
	u8 sum;
	u8 i;

	if(FRAM_PARAMETER_TIME_PHOTO_CHANNEL_LEN == FRAM_BufferRead(Buffer, 1, FRAM_PARAMETER_TIME_PHOTO_CHANNEL_ADDR))
	{
		TimePhotoChannel = Buffer[0];
	}

	if(FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_LEN == FRAM_BufferRead(Buffer, 1, FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_ADDR))
	{
		DistancePhotoChannel = Buffer[0];
	}

	if(E2_TIME_PHOTO_CONTROL_LEN == EepromPram_ReadPram(E2_TIME_PHOTO_CONTROL_ID,Buffer))
	{
		TimePhotoControl = Public_ConvertBufferToLong(Buffer);
		temp = TimePhotoControl;
		flag = (temp&(1 << 16)) >> 16;
		if(1 == flag)
		{
			TimePhotoSpace = ((temp&0xfffe0000) >> 17)*60;
		}
		else
		{
			TimePhotoSpace = (temp&0xfffe0000) >> 17;
		}
		if(TimePhotoSpace < 5)
		{
			TimePhotoSpace = 5;
		}
	}
	Address = E2_TIME_PHOTO_CONTROL_BACKUP_ADDR;
	sum = 0;
	E2prom_ReadByte(Address,Buffer,5);
	for(i=0; i<4; i++)
	{
		sum += Buffer[i];
	}
	if(sum == Buffer[4])
	{
		TimePhotoControlBackup = 0;
		TimePhotoControlBackup |= Buffer[0] << 24;
		TimePhotoControlBackup |= Buffer[1] << 16;
		TimePhotoControlBackup |= Buffer[2] << 8;
		TimePhotoControlBackup |= Buffer[3];
	}
	else
	{
		TimePhotoControlBackup = 0;
	}

	if(E2_MILE_PHOTO_CONTROL_LEN == EepromPram_ReadPram(E2_MILE_PHOTO_CONTROL_ID,Buffer))
	{
		DistancePhotoControl = Public_ConvertBufferToLong(Buffer);
		temp = DistancePhotoControl;
		flag = (temp&(1 << 16)) >> 16;
		if(1 == flag)
		{
			DistancePhotoSpace = ((temp&0xfffe0000) >> 17)*1000;
		}
		else
		{
			DistancePhotoSpace = (temp&0xfffe0000) >> 17;
		}
		if(DistancePhotoSpace < 100)
		{
			DistancePhotoSpace = 100;
		}
	}

	Address = E2_MILE_PHOTO_CONTROL_BACKUP_ADDR;
	sum = 0;
	E2prom_ReadByte(Address,Buffer,5);
	for(i=0; i<4; i++)
	{
		sum += Buffer[i];
	}
	if(sum == Buffer[4])
	{
		DistancePhotoControlBackup = 0;
		DistancePhotoControlBackup |= Buffer[0] << 24;
		DistancePhotoControlBackup |= Buffer[1] << 16;
		DistancePhotoControlBackup |= Buffer[2] << 8;
		DistancePhotoControlBackup |= Buffer[3];
	}
	else
	{
		DistancePhotoControlBackup = 0;
	}
	

	if(E2_ALARM_PHOTO_SWITCH_LEN == EepromPram_ReadPram(E2_ALARM_PHOTO_SWITCH_ID,Buffer))
	{
		AlarmPhotoSwitch = Public_ConvertBufferToLong(Buffer);
	}
	
	if(E2_ALARM_PHOTO_STORE_FLAG_LEN == EepromPram_ReadPram(E2_ALARM_PHOTO_STORE_FLAG_ID,Buffer))
	{
		AlarmPhotoStoreFlag = Public_ConvertBufferToLong(Buffer);
	}

	if(E2_STATUS_PHOTO_SWITCH_ID_LEN == EepromPram_ReadPram(E2_STATUS_PHOTO_SWITCH_ID,Buffer))
	{
		StatusPhotoSwitch = Public_ConvertBufferToLong(Buffer);
	}
	
	if(E2_STATUS_PHOTO_STORE_FLAG_ID_LEN == EepromPram_ReadPram(E2_STATUS_PHOTO_STORE_FLAG_ID,Buffer))
	{
		StatusPhotoStoreFlag = Public_ConvertBufferToLong(Buffer);
	}
	
}
/**************************************************************************
//��������Photo_AlarmChange
//���ܣ�ʵ�ֱ���λ��λ�¼����չ���
//���룺��
//�������
//����ֵ��0��ʾ����Ҫ���գ���0��ʾ��Ҫ���գ�����Ϊ�¼���������
//��ע�������¼����ͣ�0Ϊƽ̨�·�ָ����ļ��ò����������ͣ���1Ϊ��ʱ��2Ϊ���٣�������������
//3Ϊ��ײ��෭��4Ϊ���ţ�5Ϊ���ţ�6Ϊ�����ɿ���أ�ʱ����<20km/h������20km/h��
//7Ϊ���࣬��������
***************************************************************************/
static u8 Photo_AlarmChange(void)
{
	u8 flag = 0;
	u32 Alarm = 0;
	u32 temp = 0;
	u8 bit = 0;
	u8 i = 0;
	u8 PhotoEnable = 0;
	u8 PhotoEventType = 0;
	u8 PhotoStoreFlag = 0;
	u8 PhotoNum = 0;

	Alarm = Io_ReadAlarm();

	if(Alarm != LastAlarm)
	{
		temp = LastAlarm^Alarm;
		temp = temp&Alarm;
		bit = 32;
		for(i=0; i<32; i++)
		{
			if((1 << i) == (temp&(1 << i)))
			{
				if((1 << i) == (AlarmPhotoSwitch&(1 << i)))
				{
					bit = i;
					break;
				}
			}
		}
		if(bit < 32)
		{
			switch(bit)
			{
				case ALARM_BIT_EMERGENCY://����������ר�ŵĴ������̣������⴦��
				{
					PhotoEnable = 0;
					break;
				}
				case ALARM_BIT_SIDE_TURN_PRE_ALARM:
				case ALARM_BIT_IMPACT_PRE_ALARM:
				{
					PhotoEventType = 3;
					PhotoStoreFlag = (AlarmPhotoStoreFlag&(1 << bit)) >> bit;
					PhotoNum = 1;
					PhotoEnable = 1;
					break;
				}
				default:
				{
					PhotoEventType = bit+224;
					PhotoStoreFlag = (AlarmPhotoStoreFlag&(1 << bit)) >> bit;
					PhotoNum = 1;
					PhotoEnable = 1;
					break;
				}
			}
		}
		if(1 == PhotoEnable)
		{
			flag = 1;
			PhotoStoreFlag = 3;//�������ն��豣����ϴ�
			if(1 == Blind_GetLink2OpenFlag())
			{
				Camera_Photo(CHANNEL_DATA_1|CHANNEL_DATA_2, 0, PhotoNum, 10, 1, PhotoStoreFlag, 1, PhotoEventType);
			}
			else
			{
				Camera_Photo(CHANNEL_DATA_1, 0, PhotoNum, 10, 1, PhotoStoreFlag, 1, PhotoEventType);
			}
		}
	}	

	LastAlarm = Alarm;

	return flag;
}
/**************************************************************************
//��������Photo_StatusChange
//���ܣ�ʵ��״̬�仯�¼����չ���
//���룺��
//�������
//����ֵ��0��ʾ����Ҫ���գ���0��ʾ��Ҫ���գ�����Ϊ�¼���������
//��ע�������¼����ͣ�0Ϊƽ̨�·�ָ����ļ��ò����������ͣ���1Ϊ��ʱ��2Ϊ���٣�������������
//3Ϊ��ײ��෭��4Ϊ���ţ�5Ϊ���ţ�6Ϊ�����ɿ���أ�ʱ����<20km/h������20km/h��
//7Ϊ���࣬��������
***************************************************************************/
static u8 Photo_StatusChange(void)
{
	u8 flag = 0;
	u32 Status = 0;
	u32 temp = 0;
	u8 bit = 0;
	u8 i = 0;
	u8 PhotoEnable = 0;
	u8 PhotoEventType = 0;
	u8 PhotoStoreFlag = 0;
	u8 PhotoNum = 0;

	Status = Io_ReadStatus();

	if(Status != LastStatus)
	{
		temp = LastStatus^Status;
		temp = temp&Status;
		bit = 32;
		for(i=0; i<32; i++)
		{
			if((1 << i) == (temp&(1 << i)))
			{
				if((1 << i) == (StatusPhotoSwitch&(1 << i)))
				{
					bit = i;
					break;
				}
			}
		}
		if(bit < 32)
		{
			switch(bit)
			{
				case STATUS_BIT_NAVIGATION:
				case STATUS_BIT_SOUTH:
				case STATUS_BIT_EAST:
				case STATUS_BIT_ENCRYPT:
				{
					break;
				}
				case STATUS_BIT_DOOR1:
				case STATUS_BIT_DOOR2:
				case STATUS_BIT_DOOR3:
				case STATUS_BIT_DOOR4:
				case STATUS_BIT_DOOR5:
				{
					if((1 << bit) == (Status&(1 << bit)))
					{
						PhotoEventType = 4;
						PhotoStoreFlag = (StatusPhotoStoreFlag&(1 << bit)) >> bit;
						PhotoNum = 1;
						PhotoEnable = 1;
					}
					break;
				}
				case STATUS_BIT_FULL_LOAD2:
				{
					if((1 << bit) == (Status&(1 << bit)))
					{
						PhotoEventType = bit+192;
						PhotoStoreFlag = (StatusPhotoStoreFlag&(1 << bit)) >> bit;
						PhotoNum = 1;
						PhotoEnable = 1;
					}	
					break;
				}
				default:
				{
					PhotoEventType = bit+192;
					PhotoStoreFlag = (StatusPhotoStoreFlag&(1 << bit)) >> bit;
					PhotoNum = 1;
					PhotoEnable = 1;
					break;
				}
			}
		}
		if(1 == PhotoEnable)
		{
			flag = 1;
		
			PhotoStoreFlag = 3;//״̬�仯���ն��豣����ϴ�
			if(1 == Blind_GetLink2OpenFlag())
			{
				Camera_Photo(CHANNEL_DATA_1|CHANNEL_DATA_2, 0, PhotoNum, 10, 1, PhotoStoreFlag, 1, PhotoEventType);
			}
			else
			{
				Camera_Photo(CHANNEL_DATA_1, 0, PhotoNum, 10, 1, PhotoStoreFlag, 1, PhotoEventType);
			}
		}
	}	

	LastStatus = Status;

	return flag;
}
/**************************************************************************
//��������Photo_TimeArive
//���ܣ�ʵ�ֲ�����ʱ���չ���
//���룺��
//�������
//����ֵ��0��ʾ����Ҫ���գ���0��ʾ��Ҫ����
//��ע�������¼����ͣ�0Ϊƽ̨�·�ָ����ļ��ò����������ͣ���1Ϊ��ʱ��2Ϊ���٣�������������
//3Ϊ��ײ��෭��4Ϊ���ţ�5Ϊ���ţ�6Ϊ�����ɿ���أ�ʱ����<20km/h������20km/h��
//7Ϊ���࣬��������
***************************************************************************/
static u8 Photo_TimeArive(void)
{
	u8 flag = 0;
	u8 i = 0;
	u32 temp = 0;
	u8 PhotoStoreFlag = 0;
	static u32 TimeCount = 0;
	static u32 count = 0;

	count++;//ÿ�����1��
	if(count >= 2*3600)
	{
		count = 0;
		Photo_UpdatePram();
	}

	if(TimePhotoControlBackup != TimePhotoControl)
	{
		return 1;	
	}

	TimeCount++;
	if((TimeCount >= TimePhotoSpace)&&(TimePhotoSpace > 0))
	{
		TimeCount = 0;
		for(i=1; i<=CAMERA_ID_MAX; i++)
		{
			if((Camera[i-1].TimePhotoNum > 0)&&(1 == Camera[i-1].OnOffFlag))
			{
				return 1;
			}
		}
		temp = TimePhotoControl;
		for(i=1; i<=CAMERA_ID_MAX; i++)
		{
			if((1 << (i-1)) == (temp&(1 << (i-1))))
			{
				flag = 1;
				PhotoStoreFlag = (temp&(1 << (i-1+8))) >> (i-1+8);
				if(0 == PhotoStoreFlag)
				{
					PhotoStoreFlag = 0x02;
				}
				else
				{
					PhotoStoreFlag = 0x01;
				}
				Camera_Photo(TimePhotoChannel, i, 1, 10, 1, PhotoStoreFlag, 1, 1); 
			}
		}
	}

	return flag;
}
/**************************************************************************
//��������Photo_DistanceArive
//���ܣ�ʵ�ֲ����������չ���
//���룺��
//�������
//����ֵ��0��ʾ����Ҫ���գ���0��ʾ��Ҫ����
//��ע�������¼����ͣ�0Ϊƽ̨�·�ָ����ļ��ò����������ͣ���1Ϊ��ʱ��2Ϊ���٣�������������
//3Ϊ��ײ��෭��4Ϊ���ţ�5Ϊ���ţ�6Ϊ�����ɿ���أ�ʱ����<20km/h������20km/h��
//7Ϊ���࣬��������
***************************************************************************/
static u8 Photo_DistanceArive(void)
{
	u8 flag = 0;
	u8 i = 0;
	u32 temp = 0;
	u8 PhotoStoreFlag = 0;
	u32 CurDistance = 0;
	static u32 count = 0;

	count++;//ÿ�����1��
	if(count >= 2*3600)
	{
		count = 0;
		Photo_UpdatePram();
	}

	if(DistancePhotoControlBackup != DistancePhotoControl)
	{
		return 1;	
	}

	CurDistance = Photo_GetCurDistance();

	if((CurDistance >= NextPhotoDistance)&&(DistancePhotoSpace > 0))
	{
		NextPhotoDistance = CurDistance+ DistancePhotoSpace;

		temp = DistancePhotoControl;
		for(i=1; i<=CAMERA_ID_MAX; i++)
		{
			if((1 << (i-1)) == (temp&(1 << (i-1))))
			{
				flag = 1;
				PhotoStoreFlag = (temp&(1 << (i-1+8))) >> (i-1+8);
				if(0 == PhotoStoreFlag)
				{
					PhotoStoreFlag = 0x02;
				}
				else
				{
					PhotoStoreFlag = 0x01;
				}
				
				Camera_Photo(DistancePhotoChannel, i, 1, 10, 1, PhotoStoreFlag, 1, 7);
				
			}
		}
	}

	return flag;
}
/**************************************************************************
//��������Photo_GetCurDistance
//���ܣ���ȡ��ǰ�ľ��루�ۼ���ʻ��̣�
//���룺��
//�������
//����ֵ�����루�ۼ���ʻ��̣�
//��ע����λ��
***************************************************************************/
static u32 Photo_GetCurDistance(void)
{

	u32 CurMile;

	if(0 == VDRSpeed_GetCurSpeedType())
	{
		CurMile = VDRPulse_GetTotalMile()*10;
	}
	else
	{
		CurMile = MileMeter_GetTotalMile()*10;
	}

	return CurMile;
}






