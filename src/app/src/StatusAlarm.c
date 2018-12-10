/******************************************************************** 
//��Ȩ˵��	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����	:StatusAlarm.c		
//����		:ʵ��״̬λ�仯�򱨾������ϱ����������չ���
//�汾��	:V0.1
//������	:dxl
//����ʱ��	:2014.11
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
***********************************************************************/
//***************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"

#include "GPIOControl.h"
#include "Io.h"
#include "modem_lib.h"
#include "modem_app_com.h"
#include "Report.h"
#include "Blind.h"
#include "Other.h"
#include "RadioProtocol.h"
#include "EepromPram.h"
#include "taskschedule.h"
#include "SysTickApp.h"
#include "Camera_App.h"
#include "StatusAlarm.h"
#include "fm25c160.h"

/********************���ر���*************************/
/********************ȫ�ֱ���*************************/
//u8  RouteOutToInFlag = 0;

/********************�ⲿ����*************************/
extern u32	AlarmPhotoSwitch;//�������տ���,1Ϊʹ��,0��ʹ��
extern u32	AlarmPhotoStoreFlag;//��������洢��־,1Ϊ�洢,0Ϊ���洢
extern u32	StatusPhotoSwitch;//״̬�仯���տ���,1Ϊʹ��,0��ʹ��
extern u32	StatusPhotoStoreFlag;//״̬�仯���մ洢��־,1Ϊ�洢,0Ϊ���洢
extern u32      PhotoID;//��ý��ID
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
/********************���غ�������*********************/


/*********************************************************************
//��������	:StatusAlarm_TimeTask
//����		:ʵ��״̬λ�仯��������������ϱ������չ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		: 50ms����һ�Σ��¼��������ͼƬ�����ϴ�+�洢
*********************************************************************/
FunctionalState StatusAlarm_TimeTask(void)
{
	
  static u32	LastStatus = 0;//��һ��IO״̬
	static u32	LastAlarm = 0;//��һ�α���״̬
	static u32	EmergencyDelayCount = 0;//������������
	static u32	EnterCount = 0;
//	static u8  LastLoadExcursionAlarmFlag = 0;
	
	u8	EmergencyFlag = 0;//��ǰ��������״̬
	u8	PhoneCallBackType = 0;//�绰�ز�����
	u8	Buffer[21];
	u8	PramLen = 0;
	u8 	i;
	u8	PhotoEnable = 0;//����ʹ��
	u8	bit;//λ��ƫ��
	u8	ReportFlag = 0;
	u8	PhotoNum = 1;//��������
	u8	PhotoEventType = 0;//�����¼�����
	u8  flag;
//	u8  CurrentLoadExcursionAlarmFlag;
  u32	temp;
	u32 Status;
	u32 Alarm;
	
	
	//if(1 == BBGNTestFlag)
	//{
	  //CurrentLoadExcursionAlarmFlag = Io_ReadAlarmBit(ALARM_BIT_LOAD_EXCURSION);
	  //if((0 == CurrentLoadExcursionAlarmFlag)&&(1 == LastLoadExcursionAlarmFlag))//ƫ��·�߱������
		//{
		   // RouteOutToInFlag = 1;//��·�߱�־
			 // Report_UploadPositionInfo(CHANNEL_DATA_1);
		//}
		//LastLoadExcursionAlarmFlag = CurrentLoadExcursionAlarmFlag;
	//}
	
	Status = Io_ReadStatus();
	Alarm = Io_ReadAlarm();
        
    //��������
	EmergencyFlag =  Io_ReadAlarmBit(ALARM_BIT_EMERGENCY);
	if(1 == EmergencyFlag)
	{
                //PhotoStoreFlag = AlarmPhotoStoreFlag&((1 << ALARM_BIT_EMERGENCY)>>ALARM_BIT_EMERGENCY);
                
		EmergencyDelayCount++;
		if(1 == EmergencyDelayCount)
		{
                        Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);//�ϱ�һ��λ����Ϣ
                        ReportFlag = 1;
			Io_WriteRecordConrtolBit(RECORD_BIT_EMERGENCY, SET);//����¼��
		}
		else if(2 == EmergencyDelayCount)
		{ 
			if((0 == Io_ReadAlarmBit(ALARM_BIT_CAMERA_FAULT))
			   &&(0 != Camera_GetTotalNum())
		           &&((AlarmPhotoSwitch&(1 << ALARM_BIT_EMERGENCY))==(1 << ALARM_BIT_EMERGENCY))
			   &&(0 != GetTerminalAuthorizationFlag()))
			{
				if(1 == Blind_GetLink2OpenFlag())
			        {
				        RadioProtocol_MultiMediaEvenReport(CHANNEL_DATA_1|CHANNEL_DATA_2,PhotoID+1, 0, 0, 2, 0);//���Ͷ�ý���¼�
			        }
			        else
			        {
				        RadioProtocol_MultiMediaEvenReport(CHANNEL_DATA_1,PhotoID+1, 0, 0, 2, 0);//���Ͷ�ý���¼�
			        }
				
			}
		}

		else if(3 == EmergencyDelayCount)
		{
			//�ж��Ƿ�������ͷ����
			if((0 == Io_ReadAlarmBit(ALARM_BIT_CAMERA_FAULT))
			   &&(0 != Camera_GetTotalNum())
			   &&((AlarmPhotoSwitch&(1 << ALARM_BIT_EMERGENCY))==(1 << ALARM_BIT_EMERGENCY))
		           &&(0 != GetTerminalAuthorizationFlag()))
			{
				
			        if(1 == Blind_GetLink2OpenFlag())
			        {
				        Camera_Photo(CHANNEL_DATA_1|CHANNEL_DATA_2, 0, 1, 10, 1, 0x01, 1, 0x02);
			        }
			        else
			        {
				        Camera_Photo(CHANNEL_DATA_1, 0, 1, 10, 1, 0x01, 1, 0x02);
			        }
			}
			else
			{
				PhoneCallBackType = 1;
				PramLen = EepromPram_ReadPram(E2_MONITOR_PHONE_ID, Buffer);//��ȡԤ��ļ����绰����
				if((PramLen>0)&&(PramLen<20))
				{	
					EepromPram_WritePram(E2_PHONE_CALLBACK_ID, Buffer, PramLen);//�ѵ绰����д��eeprom
					EepromPram_WritePram(E2_CALLBACK_FLAG_ID, &PhoneCallBackType, 1);//д���־λ
					SetEvTask(EV_SEV_CALL);//�����绰�ز�����
				}
			}
			
		}
		else if((200 == EmergencyDelayCount)&&(1 == BBXYTestFlag))//10��������Э����Ҫ����������Զ����
		{
		    Io_WriteAlarmBit( ALARM_BIT_EMERGENCY , RESET );    //0 1����������,�յ�Ӧ������
        flag = 0;
        FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, &flag, FRAM_EMERGENCY_FLAG_LEN);//�������洢��־
			  EmergencyDelayCount = 0;
		}
	}
	else
	{
		EmergencyDelayCount = 0;
	}
        
        PhotoEnable = 0;
	//���������¼�����
	if(LastAlarm != Alarm)
	{
		temp = LastAlarm ^ Alarm;
		temp = temp & Alarm;
		bit = 0;
		for(i=0; i<32; i++)
		{
			if((1 << i) == (temp&(1 << i)))
			{
				bit = i;
				if((1 << bit) == (AlarmPhotoSwitch&(1 << bit)))
				{
					switch(bit)
					{
						case ALARM_BIT_EMERGENCY://����������ר�Ŵ�������,�����ⴥ��
							{
								PhotoEnable = 0;//ʹ������
								break;
							}
						case ALARM_BIT_SIDE_TURN_PRE_ALARM://�෭����
						case ALARM_BIT_IMPACT_PRE_ALARM://��ײ����
							{
								//PhotoStoreFlag = (AlarmPhotoStoreFlag&(1 << bit)) >> bit;
								PhotoNum = 1;
								PhotoEnable = 1;//ʹ������
								PhotoEventType = 3;
								break;
							}
						default :
							{
								//PhotoStoreFlag = (AlarmPhotoStoreFlag&(1 << bit)) >> bit;
								PhotoNum = 1;
								PhotoEnable = 1;//ʹ������
								PhotoEventType = bit+224;
								break;
							}
					}
				}
			}
			if(1 == PhotoEnable)
			{
				break;
			}
		}
		
	}
	
        //״̬�仯�����¼�����
	if((LastStatus != Status)&&(0 == PhotoEnable))
	{
		temp = LastStatus ^ Status;
		bit = 0;
		for(i=0; i<32; i++)
		{
			if((1 << i) == (temp&(1 << i)))
			{
				bit = i;
				if((1 << bit) == (StatusPhotoSwitch&(1 << bit)))
				{
					switch(bit)
					{
						case STATUS_BIT_NAVIGATION://��λ
						case STATUS_BIT_SOUTH://��γ
						case STATUS_BIT_EAST://����
						case STATUS_BIT_ENCRYPT://��γ�ȱ���
							{
								PhotoEnable = 0;//��ʹ������
								break;
							}
						case STATUS_BIT_DOOR1://��1
						case STATUS_BIT_DOOR2://��2
						case STATUS_BIT_DOOR3://��3
						case STATUS_BIT_DOOR4://��4
						case STATUS_BIT_DOOR5://��5
							{
								if((1 << bit) == (Status&(1 << bit)))//��������
								{
									PhotoEnable = 1;//ʹ������
									//PhotoStoreFlag = (StatusPhotoStoreFlag&(1 << bit)) >> bit;
									PhotoNum = 1;
									PhotoEventType = 4;
								}
								break;
							}
						case STATUS_BIT_FULL_LOAD2://���س�
							{
								if((1 << bit) == (Status&(1 << bit)))//��ת������
								{
									PhotoEnable = 1;//ʹ������
									//PhotoStoreFlag = (StatusPhotoStoreFlag&(1 << bit)) >> bit;
									PhotoEventType = bit+192;
								}
								break;
							}
						default :
							{
								PhotoEnable = 1;//ʹ������
								//PhotoStoreFlag = (StatusPhotoStoreFlag&(1 << bit)) >> bit;
								PhotoNum = 1;
								PhotoEventType = bit+192;
								break;
							}
					}
				}
			}
			if(1 == PhotoEnable)
			{
				break;
			}
		}
		
	}
        EnterCount++;
	if((1 == PhotoEnable)&&(EnterCount >= 60*SECOND))//�ϵ�60���Ժ����������
	{
		if(1 == Blind_GetLink2OpenFlag())
		{
			Camera_Photo(CHANNEL_DATA_1|CHANNEL_DATA_2, 0, PhotoNum, 10, 1, 0x01, 1, PhotoEventType);
		}
		else
		{
			Camera_Photo(CHANNEL_DATA_1, 0, PhotoNum, 10, 1, 0x01, 1, PhotoEventType); 
		}
	}
        
        //״̬�仯�����ϱ�
        if(((LastStatus != Status)&&(0 == ReportFlag))&&(EnterCount >= 60*SECOND))//�ϵ�60���Ժ������
        {
                Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);//�ϱ�һ��λ����Ϣ
                ReportFlag = 1;
        }
        //�������������ϱ�
        if(((LastAlarm != Alarm)&&(0 == ReportFlag))&&(EnterCount >= 60*SECOND))//�ϵ�60���Ժ������
        {
					      if(1 == BBGNTestFlag)//dxl,2016.5.24��������ʱ�������ϱ������ʱû��Ҫ�������ϱ����ܷ��������鷳�����¼�ⲻͨ��
								{
								    temp = LastAlarm^Alarm; 
									  if(temp == (Alarm&temp))
										{
										    Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);//�ϱ�һ��λ����Ϣ
                        ReportFlag = 1;
										}
								}
								else
								{
                    Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);//�ϱ�һ��λ����Ϣ
                    ReportFlag = 1;
								}
        }
        LastAlarm = Alarm;
	      LastStatus = Status;
	
        
        return ENABLE;
}