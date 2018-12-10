/************************************************************************
//�������ƣ�VDR_Overtime.c
//���ܣ���ʱ��ʻ
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
//#include "stm32f2xx_rtc.h"
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "Io.h"
#include "other.h"
#include "Report.h"
#include "modem_app_com.h"
#include "RadioProtocol.h"
#include <string.h>
#include "modem_debug.h"
/********************���ر���*************************/
//static    u8  OvertimeLastRunStatus = 0;//��һ����ʻ״̬
static  u8  DriverNum = 0;//��ǰ��ʻԱ��ţ�ȡֵ��Χ0-5
static  u8  DriverLoginFlag = 0;//��ʻԱ��¼��־��1Ϊǩ������¼����0Ϊǩ�ˣ�δ��¼��
static  u8  OvertimeUnloginVoiceEnableFlag = 0;//��ʻԱδ��¼�������ѿ��أ�1ʹ�ܣ�0��ֹ
static  u8  OvertimeInitFlag = 0;
static  u8  OvertimeAlarmFlag = 0;//��ʱ��ʻ��־��1Ϊ�ѳ�ʱ��0Ϊδ��ʱ���ñ�־������ҹ�䳬ʱ��ʻ�������ֻ���ڷ�ҹ�䳬ʱ
static  VDR_OVERTIME_STRUCT VdrOvertime;
static  u8  VdrOvertimeRunFlag = 0;
static  u32 MaxStopTime = 0xffffffff;   
static u32 StopAlarmCount = 0;

/********************ȫ�ֱ���*************************/
u32 TestTimeCount3 = 0;

/********************�ⲿ����*************************/
extern u8   GBTestFlag;//0:������������ģʽ;1:������ģʽ
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ������ģʽ,
/********************���غ�������*********************/
static void VDROvertime_JudgeRunOrStop(void);
static void VDROvertime_Init(void);
static void VDROvertime_HandleAlarm(void);
static void VDROvertime_HandleOneDayAlarm(void);
static void VDROvertime_HandleOtherDrivers(void);
static void VDROvertime_HandleRunUnlogin(void);
static void VDROvertime_HandleNightRunVoice(void);
static u8 VDROvertime_IsAlarm(void);
static u8 VDROvertime_NightIsAlarm(void);
static void VDROvertime_Write(u8 DriverNum);
static void VDROvertime_NightWrite(u8 DriverNum);
static void VDROvertime_SaveCurrentDriverInfo(u16 Type);
static void VDROvertime_ReadCurrentDriverInfo(void);
static void VDROvertime_SaveAllDriverInfo(void);
static void VDROvertime_ReadAllDriverInfo(void);
static u8 VDROvertime_GetRunFlag(void);
static void VDROvertime_OnceRunReport(void);
static void VDROvertime_StopAlarm(void);
static u8 VDROvertime_NightIsArrived(void);
static u8 VDROvertime_NightTimeIsOk(TIME_T Time);
static u8 VDROvertime_GetAlarmFlag(void);
/********************��������*************************/
/**************************************************************************
//��������VDROvertime_TimeTask
//���ܣ���ʱ��ʻ
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע����ʱ��ʻ��ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDROvertime_TimeTask(void)
{
    
    
    if(DriverNum >= DRIVER_NUM_MAX)
    {
        DriverNum = 0;
        DriverLoginFlag = 0;
    }

    if(0 == OvertimeInitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_OVERTIME))
        {
            VDROvertime_Init();
            OvertimeInitFlag = 1;
						#if	TACHOGRAPHS_19056_TEST
						Modem_Debug_UartTx("overtime_init",strlen("overtime_init"));
						#endif
        }
    }
    else
    {
			  VDROvertime_JudgeRunOrStop();//��ʻ��ͣʻ�ж�
			
			  VDROvertime_OnceRunReport();//����Э����Գ�ʱ��ʻʱ���ô˺������ص���Գ�ʱ��ʻ��ʼʱ�̺ͱ���ʱ��
			
			  VDROvertime_StopAlarm();//��ʱͣ��
			
        VDROvertime_HandleAlarm();//��ʱԤ������ʱ����������Ե�ǰ��ʻԱ��
			
			  VDROvertime_HandleOneDayAlarm();//�����ۼƳ�ʱ��ʻ

        VDROvertime_HandleOtherDrivers();//���ѵ���С��Ϣʱ��ļ�ʻԱ��ʼʱ����0�������Ѳ����ĳ�ʱ��ʻ��¼�����������ʻԱ��

        VDROvertime_HandleRunUnlogin();//��ʻԱδ��¼��ʻ���ѣ�������
			
			  VDROvertime_HandleNightRunVoice();//ҹ����ʻ���ѣ�����������
    }
    
    return ENABLE;  
    
}
/**************************************************************************
//��������VDROvertime_UpdateParameter
//���ܣ����³�ʱ��ʻ��ز���
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDROvertime_UpdateParameter(void)
{
    u32 temp;
    u8  Buffer[35];
    u8  BufferLen;

    BufferLen = EepromPram_ReadPram(E2_MAX_DRVIE_TIME_ID, Buffer);//��ʱ��ʻ����ʱ��
    if(E2_MAX_DRVIE_TIME_LEN == BufferLen)
    {
        VdrOvertime.AlarmTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmTime = 14400;//��λ�룬Ĭ��4Сʱ
    }

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer);//������������ʱ��
    if(E2_TIRED_DRIVE_ALARM_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.AlarmVoiceKeeptime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmVoiceKeeptime = 30;//��λ���ӣ�Ĭ��Ϊ30����
    }
		
		BufferLen = EepromPram_ReadPram(E2_MAX_DRVIE_TIME_ONEDAY_ID, Buffer);//�����ۼƳ�ʱ��ʻʱ������
    if(E2_MAX_DRVIE_TIME_ONEDAY_LEN == BufferLen)
    {
        VdrOvertime.OneDayAlarmTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.OneDayAlarmTime = 0xFFFFFFFF;//��λ�룬Ĭ��Ϊ0xFFFFFFFF
    }
		
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer);//��������ÿ����ʾ���
    if(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.AlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmVoiceGroupTime = 5;//��λ���ӣ�Ĭ��Ϊ5����
    }

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_NUMBER_ID, Buffer);//��������ÿ����ʾ���
    if(E2_TIRED_DRIVE_ALARM_NUMBER_ID_LEN == BufferLen)
    {
        VdrOvertime.AlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmVoiceNum = 3;
    }
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID, Buffer);//��������ÿ����ʾ���
    if(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.AlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmVoiceNumTime = 10;//��λ�룬Ĭ��Ϊ10��
    }
    
    BufferLen = EepromPram_ReadPram(E2_MIN_RELAX_TIME_ID, Buffer);//��С��Ϣʱ��
    if(E2_MIN_RELAX_TIME_LEN == BufferLen)
    {
        VdrOvertime.MinRestTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.MinRestTime = 1200;//��λ�룬Ĭ��Ϊ1200��
    }

    BufferLen = EepromPram_ReadPram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer);//Ԥ����ֵ
    if(E2_TIRE_EARLY_ALARM_DVALUE_LEN == BufferLen)
    {
        temp = Public_ConvertBufferToShort(Buffer);
        if(temp > VdrOvertime.AlarmTime)
        {
            temp = 1800;
        }
    }
    else
    {
        temp = 1800;//��λ�룬Ĭ��Ϊ1800��
    }
    
    if(VdrOvertime.AlarmTime > temp)
    {
      VdrOvertime.EarlyAlarmTime = VdrOvertime.AlarmTime - temp;//Ԥ��ʱ��
    }
    else
    {
      VdrOvertime.EarlyAlarmTime = 0;
    }

    
    VdrOvertime.EarlyAlarmKeeptime = temp;//Ԥ������ʱ��
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);//Ԥ������ÿ����ʾ���
    if(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.EarlyAlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.EarlyAlarmVoiceGroupTime = 5;//��λ���ӣ�Ĭ��Ϊ5����
    }
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_NUMBER_ID, Buffer);//Ԥ������ÿ����ʾ����
    if(E2_TIRED_DRIVE_PREALARM_NUMBER_ID_LEN == BufferLen)
    {
        VdrOvertime.EarlyAlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.EarlyAlarmVoiceNum = 3;
    }

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID, Buffer);//Ԥ������ÿ����ʾ���
    if(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.EarlyAlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.EarlyAlarmVoiceNumTime = 3;
    }
		
		//BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_NIGHT_TIME_ID, Buffer);//ҹ��ʱ�䣬ҹ�䳬ʱʹ��
		BufferLen = EepromPram_ReadPram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer);//ҹ��ʱ�䣬��������ʹ��ҹ�䳬��ʱ��
    if(E2_TIRED_DRIVE_NIGHT_TIME_ID_LEN == BufferLen)
    {
			  VdrOvertime.NightTimeAttribute = (Buffer[0] << 8)|Buffer[1];  
			
        VdrOvertime.NightStartTime.year = VDRPub_BCD2HEX(Buffer[2]);
	      VdrOvertime.NightStartTime.month = VDRPub_BCD2HEX(Buffer[3]);
	      VdrOvertime.NightStartTime.day = VDRPub_BCD2HEX(Buffer[4]);
	      VdrOvertime.NightStartTime.hour = VDRPub_BCD2HEX(Buffer[5]);
	      VdrOvertime.NightStartTime.min = VDRPub_BCD2HEX(Buffer[6]);
	      VdrOvertime.NightStartTime.sec = VDRPub_BCD2HEX(Buffer[7]);
	
	      VdrOvertime.NightEndTime.year = VDRPub_BCD2HEX(Buffer[8]);
	      VdrOvertime.NightEndTime.month = VDRPub_BCD2HEX(Buffer[9]);
	      VdrOvertime.NightEndTime.day = VDRPub_BCD2HEX(Buffer[10]);
	      VdrOvertime.NightEndTime.hour = VDRPub_BCD2HEX(Buffer[11]);
	      VdrOvertime.NightEndTime.min = VDRPub_BCD2HEX(Buffer[12]);
	      VdrOvertime.NightEndTime.sec = VDRPub_BCD2HEX(Buffer[13]);
    }
    else
    {
        VdrOvertime.NightTimeAttribute = 0;  
			
        VdrOvertime.NightStartTime.year = 0;
	      VdrOvertime.NightStartTime.month = 0;
	      VdrOvertime.NightStartTime.day = 0;
	      VdrOvertime.NightStartTime.hour = 0;
	      VdrOvertime.NightStartTime.min = 0;
	      VdrOvertime.NightStartTime.sec = 0;
	
	      VdrOvertime.NightEndTime.year = 0;
	      VdrOvertime.NightEndTime.month = 0;
	      VdrOvertime.NightEndTime.day = 0;
	      VdrOvertime.NightEndTime.hour = 0;
	      VdrOvertime.NightEndTime.min = 0;
	      VdrOvertime.NightEndTime.sec = 0;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_MAX_DRVIE_TIME_ID, Buffer);//ҹ��������ʻʱ������
    if(E2_NIGHT_MAX_DRVIE_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightAlarmTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightAlarmTime = 7200;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_MIN_RELAX_TIME_ID, Buffer);//ҹ����С��Ϣʱ��
    if(E2_NIGHT_MIN_RELAX_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightMinRestTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightMinRestTime = 1200;
    }
        
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer);//��ʱ��ʻ��ʻ�ж��ٶ�
    if(E2_TIRED_DRIVE_RUN_SPEED_ID_LEN == BufferLen)
    {
        VdrOvertime.RunSpeed = Buffer[3];//ǰ��3�ֽ�һ��Ϊ0��ȡ��4�ֽڼ���
    }
    else
    {
        VdrOvertime.RunSpeed = 0;
    }
        
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer);//��ʱ��ʻ��ʻ�ж�����ʱ��
    if(E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN == BufferLen)
    {
        VdrOvertime.RunKeeptime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.RunKeeptime = 10;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_TIME_ID, Buffer);//ҹ����ʻԤ��ʱ��
    if(E2_NIGHT_DRIVE_PREALARM_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightVoiceKeeptime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightVoiceKeeptime = 10;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);//ҹ����ʻԤ����ʾʱ����
    if(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightVoiceGroupTime = 5;
    }
		

    BufferLen = EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, Buffer);//��ʻԱδ��¼��ʾ����ʹ�ܿ���,bit7:1������ģʽ;bit6:1������ģʽ
    if(E2_UNLOGIN_TTS_PLAY_ID_LEN == BufferLen)
    {
        OvertimeUnloginVoiceEnableFlag = Buffer[0]&0x01;
    }
    else
    {
        OvertimeUnloginVoiceEnableFlag = 0;
    }
		
		BufferLen = EepromPram_ReadPram(E2_MAX_STOP_CAR_TIME_ID, Buffer);//�ͣ��ʱ��
    if(E2_MAX_STOP_CAR_TIME_LEN == BufferLen)
    {
        MaxStopTime = 0;
			  MaxStopTime |= Buffer[0] << 24;
			  MaxStopTime |= Buffer[1] << 16;
			  MaxStopTime |= Buffer[2] << 8;
			  MaxStopTime |= Buffer[3];
    }
    else
    {
        MaxStopTime = 0;
    }
        
    UpdateRunMode();
    
}
/**************************************************************************
//��������VDROvertime_DriverLogin
//���ܣ���ʻԱǩ������¼��
//���룺pBuffer:ָ���ʻ֤�����ָ��;BufferLen:���ݳ���
//�������
//����ֵ��0:��¼�ɹ�����0:��¼ʧ�ܣ�Ϊ1��ʾ���ȴ���Ϊ2��ʾ��ʻ֤�����ʽ����
//Ϊ3��ʾ��ʻ֤��ȫΪASCII 0������ֹϵͳĬ�ϼ�ʻԱǩ������Ϊ4��ʾ�ü�ʻԱ��ǩ��
//��ע����ʻ֤���볤��BufferLen����Ϊ18�ֽڣ������¼ʧ��
***************************************************************************/
u8 VDROvertime_DriverLogin(u8 *pBuffer, u8 BufferLen)
{
    u8  i;
    u8  *p = NULL;
    u8  Buffer[VDR_DATA_OVER_TIME_STEP_LEN];
	  u8  DriverBuffer[128];
    u8  flag;
    u8  Num;
        u8      LCDDisplayBuf[50];
        u8      length;
    u32 TimeCount;
    TIME_T  Time;


    if(18 != BufferLen)
    {
        return 0x01;
    }

    for(i=0; i<BufferLen; i++)
    {
        if(((*(pBuffer+i) >= '0')&&((*(pBuffer+i) <= '9')))//��ʻ֤���뼴Ϊ���֤���룬ȫΪ������ɣ�У�����п�����10��X��
            ||(0 == *(pBuffer+i))
                ||('X' == *(pBuffer+i))
                    ||('x' == *(pBuffer+i))
                        ||(' ' == *(pBuffer+i)))
        {
            
        }
        else
        {
            return 0x02;
        }
    }

    
    Num = DRIVER_NUM_MAX;
    for(i=1; i<DRIVER_NUM_MAX; i++)
    {
        if(0 == strncmp((char *)pBuffer,(const char *)VdrOvertime.Driver[i].License,BufferLen))
        {
            Num = i;
            break;
        }
    }
    if(DRIVER_NUM_MAX == Num)//���Ѽ�¼�ļ�ʻ֤��δ�ҵ���ͣ��ʱ�����ϵ��Ǹ���ʻԱ�ø���ǰ��ʻԱ
    {
        flag = 0;
        for(i=0; i<BufferLen; i++)
        {
            if('0' != *(pBuffer+i))
            {
                flag = 1;
                break;
            }
        }
        if(0 == flag)
        {
            Num = 0;
            memcpy(VdrOvertime.Driver[0].License,pBuffer,BufferLen);
            if(0 == DriverNum)
            {
                return 0x04;
            }
        }
        else
        {
            TimeCount = 0xFFFFFFFF;
            for(i=1; i<DRIVER_NUM_MAX; i++)
            {
                if(VdrOvertime.Driver[i].EndTimeCount < TimeCount)
                {
                    TimeCount = VdrOvertime.Driver[i].EndTimeCount;
                    Num = i;
                }
            }
        }
        
    }
    else//���Ѽ�¼�ļ�ʻ֤���ҵ���
    {
        if(Num == DriverNum)
        {
            return 0x04;
        }
        else//�����˼�ʻԱ
        {
            
        }
    }

    //��ǩ����һ����ʻԱ
    VDROvertime_DriverLogout();

    //��ǩ����ǰ��ʻԱ
    DriverNum = Num;
    memcpy(VdrOvertime.Driver[DriverNum].License,pBuffer,18);
    TimeCount = RTC_GetCounter();
    if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
        ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount)
            ||((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime))//���¼���ʱ��
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }

    
    if(0 != DriverNum)
    {
        TimeCount++;//dxl,2016.7.20��1��Ϊ�˱��⵱ͬһ������ǩ�ˡ�ǩ���¼�ʱ����ǩ��©����
			
        Gmtime(&Time, TimeCount);
        
        p = Buffer;
        VDRPub_ConvertTimeToBCD(p,&Time);//ǩ����¼ 
        p += 6;
        memcpy(p,VdrOvertime.Driver[DriverNum].License,18);
        p += 18;
        *p++ = 0x01;
        VDRData_Write(VDR_DATA_TYPE_DRIVER,Buffer,VDR_DATA_DRIVER_STEP_LEN-5,TimeCount);
        
        VdrOvertime.Driver[DriverNum].LoginTimeCount = TimeCount;
        
        DriverLoginFlag = 1;
                
        Public_PlayTTSVoiceStr("��ʻԱǩ����");
                
        strcpy((char *)LCDDisplayBuf,"��ʻԱǩ�� ֤��:");
        length = strlen((char *)LCDDisplayBuf);
        memcpy(LCDDisplayBuf+length,VdrOvertime.Driver[DriverNum].License,18);
        LCDDisplayBuf[length+18] = 0;
        Public_ShowTextInfo((char *)LCDDisplayBuf,PUBLICSECS(6));
				
				Io_WriteSelfDefine2Bit(DEFINE_BIT_5,SET);
				
				length = VDRCard_GetReportInfo(DriverBuffer);
				RadioProtocol_DriverInformationReport(CHANNEL_DATA_1|CHANNEL_DATA_2,DriverBuffer,length);
    }
    else
    {
        DriverLoginFlag = 0;
    }
        
    //д�뵱ǰ��ʻ֤����
    EepromPram_WritePram(E2_CURRENT_LICENSE_ID, VdrOvertime.Driver[DriverNum].License, 18);  
    
    VDROvertime_SaveAllDriverInfo();//�����м�ʻԱ��Ϣ���浽flash
    VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ALL);//����ǰ��ʻԱ��Ϣ���浽fram


    return 0;
}
/**************************************************************************
//��������VDROvertime_DriverLogout
//���ܣ���ʻԱǩ�ˣ�δ��¼��
//���룺pBuffer:ָ���ʻ֤�����ָ��;BufferLen:���ݳ���
//�������
//����ֵ��0:��¼�ɹ�����0:��¼ʧ�ܣ�Ϊ1��ʾ���ȴ���Ϊ2��ʾ��ʻ֤�����ʽ����
//Ϊ3��ʾ��ʻ֤��ȫΪASCII 0������ֹϵͳĬ�ϼ�ʻԱǩ������Ϊ4��ʾ�ü�ʻԱ��ǩ��
//��ע����ʻ֤���볤��BufferLen����Ϊ18�ֽڣ������¼ʧ��
***************************************************************************/
void VDROvertime_DriverLogout(void)
{
    u8  Buffer[VDR_DATA_DRIVER_STEP_LEN+1];
    u8  *p = NULL;
        u8      LCDDisplayBuf[50];
        u8      length;
    u32 TimeCount;
    TIME_T  Time;

    if(0 != DriverNum)//�Զ��л�ΪĬ�ϼ�ʻԱ
    {
        TimeCount = RTC_GetCounter();
        VdrOvertime.Driver[DriverNum].LogoutTimeCount = TimeCount;
        
        Gmtime(&Time, TimeCount);
        p = Buffer;
        VDRPub_ConvertTimeToBCD(p,&Time);   
        p += 6;
        memcpy(p,VdrOvertime.Driver[DriverNum].License,18);
        p += 18;
        *p++ = 0x02;
        
        VDRData_Write(VDR_DATA_TYPE_DRIVER,Buffer,VDR_DATA_DRIVER_STEP_LEN-5,TimeCount);//����ǩ�˼�¼

        //����ǩ������������
        
        DriverNum = 0;
        memset(VdrOvertime.Driver[DriverNum].License,'0',18);
        if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
            ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount)
                ||((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime))//���¼���ʱ��
        {
            VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
        }
                
        //д�뵱ǰ��ʻ֤����
        EepromPram_WritePram(E2_CURRENT_LICENSE_ID, VdrOvertime.Driver[DriverNum].License, 18);   
                
        Public_PlayTTSVoiceStr("��ʻԱǩ�ˣ�");
                
        strcpy((char *)LCDDisplayBuf,"��ʻԱǩ��");
        length = strlen((char *)LCDDisplayBuf);
        LCDDisplayBuf[length] = 0;
        Public_ShowTextInfo((char *)LCDDisplayBuf,PUBLICSECS(6));
				
				Io_WriteSelfDefine2Bit(DEFINE_BIT_5,RESET);
				
				Buffer[0] = 0x02;//�°�
	      VDRPub_ConvertNowTimeToBCD(Buffer+1);//ʱ�䣬6�ֽ�BCD
				RadioProtocol_DriverInformationReport(CHANNEL_DATA_1|CHANNEL_DATA_2,Buffer,7);
        
    }

    DriverLoginFlag = 0;

    VDROvertime_SaveAllDriverInfo();//�����м�ʻԱ��Ϣ���浽flash

    VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ALL);//����ǰ��ʻԱ��Ϣ���浽fram
}
/**************************************************************************
//��������VDROvertime_GetInitFlag
//���ܣ����ģ���ʼ����־
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
u8 VDROvertime_GetInitFlag(void)
{
    return OvertimeInitFlag;
}
/**************************************************************************
//��������VDROvertime_GetLoginFlag
//���ܣ���ȡ��¼��־
//���룺��
//�������¼��־
//����ֵ��1:��¼��0:δ��¼
//��ע����
***************************************************************************/
u8 VDROvertime_GetLoginFlag(void)
{
    return DriverLoginFlag;
}

/**************************************************************************
//��������VDROvertime_Get2daysLog
//���ܣ���ȡ2��������ĳ�ʱ��ʻ��¼
//���룺��
//�����
//����ֵ�����ݳ���
//��ע��
***************************************************************************/
u16 VDROvertime_Get2daysLog(u8 *pBuffer)
{
        TIME_T  Time;
        TIME_T  StartTime;
        TIME_T  EndTime;
        u32     EndTimeCountMin;
        u32     EndTimeCount;
        u16     length;
        u16     Len;
        u8      *p = NULL;
        u8      *q = NULL;
        u8      i;
        u8      Buffer[60];
        s16     *pSector = NULL;
        s16     *pStep = NULL;
        
        RTC_GetCurTime(&Time);
        Time.hour = 0;
        Time.min = 0;
        Time.sec = 0;
        EndTimeCountMin = ConverseGmtime(&Time);
    
          if(EndTimeCountMin >= 24*3600)
                {
            EndTimeCountMin -= 24*3600;
                }
                else
                {
                        EndTimeCountMin = 0;
                }
        Gmtime(&StartTime, EndTimeCountMin);
        RTC_GetCurTime(&EndTime);
        
        length = 0;
        EndTimeCount = 0xffffffff;  
        p = pBuffer;
        Len = VDROvertime_GetUnsaveLog(p,EndTime,DRIVER_NUM_MAX);
        p += Len;
        length += Len;
                
        for(i=0; i<12; i++)//���ȡ12����¼
        {
                Len = VDRData_Read(Buffer,pSector,pStep,VDR_DATA_TYPE_OVERTIME,StartTime,EndTime,1);//1�ζ�1�� 
                if((length >= 600)||(EndTimeCount < EndTimeCountMin)||(Len != VDR_DATA_OVER_TIME_COLLECT_LEN))
                {
                        break;
                }
                q = Buffer+24;//ָ�����ʱ���׵�ַ
                VDRPub_ConvertBCDToTime(&EndTime,q);
                EndTimeCount = ConverseGmtime(&EndTime);
                memcpy(p,Buffer,Len);
                p += Len;
                length += Len;    
        }
        
        return length;
}
/**************************************************************************
//��������VDROvertime_GetUnsaveLog
//���ܣ���ȡ��δ���浽flash�ĳ�ʱ��ʻ��¼
//���룺Time:ָ��ʱ��;DriverNum:��ʻԱ���,DriverNum==DRIVER_NUM_MAX��ʾҪ��ȡRAM�����еĳ�ʱ��ʻ��¼
//�����
//����ֵ�����ݳ���
//��ע����RAM�������ж�ȡ��δд�뵽FLASH�еĳ�ʱ��ʻ��¼
***************************************************************************/
u16 VDROvertime_GetUnsaveLog(u8 *pBuffer, TIME_T Time, u8 DriverNum)
{
    u8  *p = NULL;
    u8  i,j,k;
        u8      flag;
    u16 length;
    u32 TimeCount;
    u32 temp;
        
        if(DriverNum > DRIVER_NUM_MAX)//��������
        {
                return 0;
        }
        else if(DriverNum == DRIVER_NUM_MAX)
        {
               j = 0;
               k = DRIVER_NUM_MAX;
        }
        else
        {
               j = DriverNum;
               k = DriverNum+1;
        }
        
         p = pBuffer;
     TimeCount = ConverseGmtime(&Time);
     length = 0;
     for(i=j; i<k; i++)
     {
               if((0 != VdrOvertime.Driver[i].StartTimeCount)
                  &&((VdrOvertime.Driver[i].EndTimeCount-VdrOvertime.Driver[i].StartTimeCount) > VdrOvertime.AlarmTime)
                    &&(TimeCount >= VdrOvertime.Driver[i].EndTimeCount)
                      &&(length <= 600))
               {
                      flag = 0;
                         
                      //��ʻ֤����
                      memcpy(p,VdrOvertime.Driver[i].License,18);
                      p += 18;
    
              //��ʼʱ��
                                      temp = VdrOvertime.Driver[i].StartTimeCount;
                                      if(temp > 10)
                                            {
                                        temp -= 10;
                                            }
                      Gmtime(&Time, temp);
                      if(0 == Public_CheckTimeStruct(&Time))
                      {
                              flag = 1;
                      }
                      VDRPub_ConvertTimeToBCD(p,&Time);
                      p += 6;

                      //����ʱ��
                                            temp = VdrOvertime.Driver[i].EndTimeCount;
                                            if(temp > 10)
                                            {
                                              temp -= 10;
                                            }
                      Gmtime(&Time, temp);
                      if(0 == Public_CheckTimeStruct(&Time))
                      {
                              flag = 1;
                      }
                      VDRPub_ConvertTimeToBCD(p,&Time);
                      p += 6;

                      //��ʼλ��
                      memcpy(p,VdrOvertime.Driver[i].StartPosition,10);
                      p += 10;

                      //����λ��
                      memcpy(p,VdrOvertime.Driver[i].EndPosition,10);
                      p += 10;
                        
                      if(1 == flag)
                      {
                              p -= 50;//ʱ�����������������
                      }
                      else
                      {
                              length += 50;
                      }
        }
    }

    return length;  
}
/**************************************************************************
//��������VDROvertime_GetLicense
//���ܣ���ȡ��ǰ��ʻ֤����
//���룺ָ��Ŀ�Ļ���ָ��
//�����
//����ֵ�����ݳ���
//��ע��
***************************************************************************/
u8 VDROvertime_GetLicense(u8 *pBuffer)
{
    memcpy(pBuffer,VdrOvertime.Driver[DriverNum].License,18);
    return 18;
}
/**************************************************************************
//��������VDROvertime_GetDriveTime
//���ܣ���ȡ��ǰ��ʻԱ������ʻʱ��
//���룺��
//�����
//����ֵ��������ʻʱ�䣬��λ����
//��ע��
***************************************************************************/
u32 VDROvertime_GetDriveTime(void)
{
  u32 TimeCount;
  
	if(0 == VDROvertime_NightIsArrived())
  {
    if(VdrOvertime.Driver[DriverNum].EndTimeCount > VdrOvertime.Driver[DriverNum].StartTimeCount)
    {
      TimeCount = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
    }
    else
    {
      TimeCount = 0;
    }
  }
	else//ҹ��ʱ��
	{
		if(1 == VDROvertime_GetAlarmFlag())
		{
		  if(VdrOvertime.Driver[DriverNum].EndTimeCount > VdrOvertime.Driver[DriverNum].StartTimeCount)
      {
        TimeCount = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
      }
      else
      {
        TimeCount = 0;
      }
		}
		else
		{
	    if(VdrOvertime.Driver[DriverNum].NightEndTimeCount > VdrOvertime.Driver[DriverNum].NightStartTimeCount)
      {
        TimeCount = VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount;
      }
      else
      {
        TimeCount = 0;
      }
	  }
    
	}
	return (TimeCount/60);
}
 
//////////////////////////////////����Ϊ�ڲ�����///////////////////////////////////////
/**************************************************************************
//��������VDROvertime_JudgeRunOrStop
//���ܣ���ʻ��ͣʻ���ж�
//���룺��
//�������
//����ֵ����
//��ע:
***************************************************************************/
static void VDROvertime_JudgeRunOrStop(void) 
{
    static u32 RunCount = 0;
    static u32 StopCount = 0;
    
    u8  Speed;

	  Speed = VDRSpeed_GetCurSpeed();

    if(Speed > VdrOvertime.RunSpeed)
    {
        StopCount = 0;
        RunCount++;
        if(RunCount >= VdrOvertime.RunKeeptime)
        {
            RunCount--;
            VdrOvertimeRunFlag = 1;
        }
    }
    else
    {
        RunCount = 0;
        StopCount++;
        if(StopCount >= VdrOvertime.RunKeeptime)
        {
            StopCount--;
            VdrOvertimeRunFlag = 0;
        }
    }
}
/**************************************************************************
//��������VDROvertime_GetRunFlag
//���ܣ���ȡ��ʻ״̬��־
//���룺��
//�������
//����ֵ��Ϊ1Ϊ��ʻ��Ϊ0Ϊͣʻ
//��ע:
***************************************************************************/
static u8 VDROvertime_GetRunFlag(void)
{
    return VdrOvertimeRunFlag;
}
/**************************************************************************
//��������VDROvertime_Init
//���ܣ���ʱ��ʻģ���ʼ��
//���룺��
//�������
//����ֵ����
//��ע��ֻ����ɳ�ʱ��ʻģ���ʼ������ܿ���IC����ʱ����
***************************************************************************/
static void VDROvertime_Init(void)
{
    VDROvertime_UpdateParameter();//���³�ʱ��ʻ����
    VDROvertime_ReadAllDriverInfo();//��ȡ���м�ʻԱ��Ϣ���Զ�ǩ����һ����ʻԱ��
    
}
/**************************************************************************
//��������VDROvertime_HandleAlarm
//���ܣ���ʱ��ʻԤ������ʱ��ʻ����
//���룺��
//�������
//����ֵ����
//��ע����VDROvertime_TimeTask���ã�1�����1��
***************************************************************************/
static void VDROvertime_HandleAlarm(void)
{
    static  u32 EarlyAlarmVoiceCount = 0;
    static  u32 EarlyAlarmVoiceNumCount = 0;
    static  u32 EarlyAlarmVoiceNumTimeCount = 0; 
    static  u32 AlarmVoiceCount = 0;
    static  u32 AlarmVoiceNumCount = 0;
    static  u32 AlarmVoiceNumTimeCount = 0; 

    u8  AlarmFlag;
	  u8  DayAlarm;
	  u8  NightAlarm;
    u8  Buffer[40];//���������������Ϊ40�ֽ�
    u8  BufferLen;
    u32 CurrentTimeCount;

    DayAlarm = VDROvertime_IsAlarm();
	
	  NightAlarm = VDROvertime_NightIsAlarm();
	
	  if((0x02 == NightAlarm)||(0x02 == DayAlarm))
		{
		    AlarmFlag = 0x02;
		}
		else if((0x01 == NightAlarm)||(0x01 == DayAlarm))
		{
		    AlarmFlag = 0x01;
		}
		else
		{
		    AlarmFlag = 0x00;
		}

    if(0x02 == AlarmFlag)//�г�ʱ����
    {
        EarlyAlarmVoiceCount = 0;
        EarlyAlarmVoiceNumCount = 0;
        EarlyAlarmVoiceNumTimeCount = 0; 

        AlarmVoiceCount++;
        if((0 == (AlarmVoiceCount%(VdrOvertime.AlarmVoiceGroupTime*60)))&&(AlarmVoiceCount <= (VdrOvertime.AlarmVoiceKeeptime*60)))
        {
            AlarmVoiceNumCount = 0;
            AlarmVoiceNumTimeCount = 0; 
        }
        if(AlarmVoiceNumCount < VdrOvertime.AlarmVoiceNum)
        {
            if(0 == (AlarmVoiceNumTimeCount%VdrOvertime.AlarmVoiceNumTime))
            {
                AlarmVoiceNumCount++; 
                BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_VOICE_ID, Buffer);
                if(0 == BufferLen)
                {
                    strcpy((char *)Buffer,"����ʱ��ʻ");
                }
                else
                {
                    Buffer[BufferLen] = 0;
                }
        if(1 == VDRDoubt_GetCarRunFlag())
        {
          Public_PlayTTSVoiceStr(Buffer);
          Public_ShowTextInfo((char *)Buffer,100);
        }
            }
            AlarmVoiceNumTimeCount++;
        }
            
    }
    else//�޳�ʱ����������£��ٿ����޳�ʱԤ����
    {
        AlarmVoiceCount = 0;
        AlarmVoiceNumCount = 0;
        AlarmVoiceNumTimeCount = 0; 
        
        CurrentTimeCount = RTC_GetCounter();
        if((CurrentTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount) <= (VdrOvertime.AlarmTime-10))
        {

					
          if(0x01 == AlarmFlag)//�г�ʱԤ��
          {
              EarlyAlarmVoiceCount++;
              if(EarlyAlarmVoiceCount >= (VdrOvertime.EarlyAlarmVoiceGroupTime*60))
              {
                  EarlyAlarmVoiceCount = 0;
                  EarlyAlarmVoiceNumCount = 0;
                  EarlyAlarmVoiceNumTimeCount = 0; 
              }
              if(EarlyAlarmVoiceNumCount < VdrOvertime.EarlyAlarmVoiceNum)
              {
                  if(0 == (EarlyAlarmVoiceNumTimeCount%VdrOvertime.EarlyAlarmVoiceNumTime))
                  {
                      EarlyAlarmVoiceNumCount++; 
                      BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_VOICE_ID, Buffer);
                      if(0 == BufferLen)
                      {
                          strcpy((char *)Buffer,"������ʱ��ʻ");
                      }
                      else
                      {
                          Buffer[BufferLen] = 0;
                      }
            if(1 == VDRDoubt_GetCarRunFlag())
            {
              Public_PlayTTSVoiceStr(Buffer);
                          Public_ShowTextInfo((char *)Buffer,100);
            }
                  }
                  EarlyAlarmVoiceNumTimeCount++;
              }
          }
        }
        else//�޳�ʱԤ��
        {

            EarlyAlarmVoiceCount = 0;
            EarlyAlarmVoiceNumCount = 0;
            EarlyAlarmVoiceNumTimeCount = 0; 
        }
    }
    
}
/**************************************************************************
//��������VDROvertime_OneDayAlarm
//���ܣ������ۼƳ�ʱ��ʻ����
//���룺��
//�������
//����ֵ����
//��ע����VDROvertime_TimeTask���ã�1�����1��
***************************************************************************/
static void VDROvertime_HandleOneDayAlarm(void)
{
	  u8 i;
	  u8 Buffer[50];
	  u8 RunStatus;
	  u32 CurrentTimeCount;
    TIME_T CurTime;
	  TIME_T LastTime;
	
	  static  u32 AlarmVoiceCount = 0;
    static  u32 AlarmVoiceNumCount = 0;
    static  u32 AlarmVoiceNumTimeCount = 0; 
	  static  u32 LastTimeCount = 0;
	
	  RTC_GetCurTime(&CurTime);
	
	  CurrentTimeCount = RTC_GetCounter();
    if(((CurrentTimeCount > (LastTimeCount+2))&&(0 != LastTimeCount))//ʱ������
			||((CurrentTimeCount < (LastTimeCount-2))&&(0 != LastTimeCount)))
    {
			  Gmtime(&LastTime, LastTimeCount);
			  if(LastTime.day != CurTime.day)
				{
					  VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount = 0; 
				    VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME);  
				}
    }
    LastTimeCount = CurrentTimeCount;
		
	  if((0 == CurTime.hour)&&(0 == CurTime.min)&&(0 == CurTime.sec))//0ʱ0��0����0�ۼƳ�ʱ��ʻʱ��
		{
		    for(i=0; i<DRIVER_NUM_MAX; i++)
        {
				    VdrOvertime.Driver[i].OneDayDriveTimeCount = 0;
					  VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME);  
				}
		}
		else
		{
		    RunStatus = VDROvertime_GetRunFlag();
			  if(1 == RunStatus)//��ʻ
				{
				    VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount++;
					  VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME);
					  if(VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount >= VdrOvertime.OneDayAlarmTime)
						{
                AlarmVoiceCount++;
                if((0 == (AlarmVoiceCount%(VdrOvertime.AlarmVoiceGroupTime*60)))&&(AlarmVoiceCount <= (VdrOvertime.AlarmVoiceKeeptime*60)))
                {
                    AlarmVoiceNumCount = 0;
                    AlarmVoiceNumTimeCount = 0; 
                }
                if(AlarmVoiceNumCount < VdrOvertime.AlarmVoiceNum)
                {
                    if(0 == (AlarmVoiceNumTimeCount%VdrOvertime.AlarmVoiceNumTime))
                    {
                        AlarmVoiceNumCount++; 
                        strcpy((char *)Buffer,"�ۼƳ�ʱ��ʻ");
                        Public_PlayTTSVoiceStr(Buffer);
                        Public_ShowTextInfo((char *)Buffer,100);
                    }
                    AlarmVoiceNumTimeCount++;
                }
								Io_WriteAlarmBit(ALARM_BIT_DAY_OVER_DRIVE,SET);
						}
						else
						{
						    AlarmVoiceCount = 0;
                AlarmVoiceNumCount = 0;
                AlarmVoiceNumTimeCount = 0; 
							  Io_WriteAlarmBit(ALARM_BIT_DAY_OVER_DRIVE,RESET);
						}
				}
				else
				{
					  if(VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount >= VdrOvertime.OneDayAlarmTime)
						{
						    Io_WriteAlarmBit(ALARM_BIT_DAY_OVER_DRIVE,SET);
						}
						else
						{
						    Io_WriteAlarmBit(ALARM_BIT_DAY_OVER_DRIVE,RESET);
						}
				    AlarmVoiceCount = 0;
            AlarmVoiceNumCount = 0;
            AlarmVoiceNumTimeCount = 0; 
				}
		}
}
/**************************************************************************
//��������VDROvertime_HandleOtherDrivers
//���ܣ�����������ʻԱ�ĳ�ʱ��ʻ
//���룺��
//�������
//����ֵ����
//��ע����VDROvertime_TimeTask���ã�1�����1��
***************************************************************************/
static void VDROvertime_HandleOtherDrivers(void)
{
    u8  i;
    u32 CurrentTimeCount;
    u32 OffsetTime;

    CurrentTimeCount = RTC_GetCounter();

    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        if(i != DriverNum)
        {
            if((VdrOvertime.Driver[i].StartTimeCount > VdrOvertime.Driver[i].EndTimeCount)
                ||(VdrOvertime.Driver[i].EndTimeCount > CurrentTimeCount))
            {
                VdrOvertime.Driver[i].StartTimeCount = 0;
                VdrOvertime.Driver[i].EndTimeCount = 0;
                VDROvertime_SaveAllDriverInfo();
            }
            
            OffsetTime = CurrentTimeCount - VdrOvertime.Driver[i].EndTimeCount;
            if((OffsetTime >= VdrOvertime.MinRestTime)&&(0 != VdrOvertime.Driver[i].EndTimeCount)&&(0 != VdrOvertime.Driver[i].StartTimeCount))
            {
                OffsetTime = VdrOvertime.Driver[i].EndTimeCount - VdrOvertime.Driver[i].StartTimeCount;
                if(OffsetTime >= VdrOvertime.AlarmTime)
                {
                    VDROvertime_Write(i);
                }
                VdrOvertime.Driver[i].StartTimeCount = 0;
                VdrOvertime.Driver[i].EndTimeCount = 0;
                VDROvertime_SaveAllDriverInfo();
            }
						
						if((VdrOvertime.Driver[i].NightStartTimeCount > VdrOvertime.Driver[i].NightEndTimeCount)
                ||(VdrOvertime.Driver[i].NightEndTimeCount > CurrentTimeCount))
            {
                VdrOvertime.Driver[i].NightStartTimeCount = 0;
                VdrOvertime.Driver[i].NightEndTimeCount = 0;
                VDROvertime_SaveAllDriverInfo();
            }
            
            OffsetTime = CurrentTimeCount - VdrOvertime.Driver[i].NightEndTimeCount;
            if((OffsetTime >= VdrOvertime.NightMinRestTime)&&(0 != VdrOvertime.Driver[i].NightEndTimeCount)&&(0 != VdrOvertime.Driver[i].NightStartTimeCount))
            {
                OffsetTime = VdrOvertime.Driver[i].NightEndTimeCount - VdrOvertime.Driver[i].NightStartTimeCount;
                if(OffsetTime >= VdrOvertime.NightAlarmTime)
                {
                    VDROvertime_NightWrite(i);
                }
                VdrOvertime.Driver[i].NightStartTimeCount = 0;
                VdrOvertime.Driver[i].NightEndTimeCount = 0;
                VDROvertime_SaveAllDriverInfo();
            }
        }
    }
}
/**************************************************************************
//��������VDROvertime_HandleRunUnlogin
//���ܣ���ʻԱδǩ����ʻ����
//���룺��
//�������
//����ֵ����
//��ע����VDROvertime_TimeTask���ã�1�����1��
***************************************************************************/
static void VDROvertime_HandleRunUnlogin(void)
{
    
    static u32 PlayVoiceTimeCount = 0;
    static u8 OnceTimeCount = 0;
    static u8 NumCount = 0;
    
    u32 CurrentTimeCount;
    
    CurrentTimeCount = RTC_GetCounter();
    
    if((1 == OvertimeUnloginVoiceEnableFlag)
        &&(0 == DriverNum)
       &&(0 != VdrOvertime.Driver[DriverNum].StartTimeCount)
        &&(CurrentTimeCount <= (VdrOvertime.Driver[DriverNum].StartTimeCount+1800))
          &&(1 == VDROvertime_GetRunFlag()))
    {
        if(CurrentTimeCount >= PlayVoiceTimeCount)
        {
          if(0 == OnceTimeCount%10)
            {
                Public_PlayTTSVoiceStr("��ͣ������IC��");//������ʾ����
                Public_ShowTextInfo("��ͣ������IC��",100);
                NumCount++;
                if(NumCount >= 3)
                {
                    NumCount = 0;
                  PlayVoiceTimeCount = CurrentTimeCount+300;
                }
            }
            if(OnceTimeCount >= 10)
            {
              OnceTimeCount = 0;
            }
            OnceTimeCount++;
        }
    }
    else if((0 == DriverNum)
                &&(CurrentTimeCount > (VdrOvertime.Driver[DriverNum].StartTimeCount+1800))
                &&(0 != VdrOvertime.Driver[DriverNum].StartTimeCount))
    {
      PlayVoiceTimeCount = 0;
        NumCount = 0;
        OnceTimeCount = 0;
    }
    else if((0 == DriverNum)
              &&(0 == VdrOvertime.Driver[DriverNum].StartTimeCount))
    {
      PlayVoiceTimeCount = 0;
        NumCount = 0;
        OnceTimeCount = 0;
    }
    
}
/**************************************************************************
//��������VDROvertime_HandleNightRunVoice
//���ܣ�ҹ����ʻ��������
//���룺��
//�������
//����ֵ����
//��ע����VDROvertime_TimeTask���ã�1�����1��
***************************************************************************/
static void VDROvertime_HandleNightRunVoice(void)
{
	
	  TIME_T CurTime;
    u8  Buffer[40];//���������������Ϊ40�ֽ�
    u8  BufferLen;
    u32 CurrentTimeCount;
	  u32 NightStartTimeCount;
	
	  static u32 count = 0;

	  if(((0 == VdrOvertime.NightStartTime.day)&&(0 == VdrOvertime.NightStartTime.month)&&(0 == VdrOvertime.NightStartTime.year))
			||((0 == VdrOvertime.NightEndTime.day)&&(0 == VdrOvertime.NightEndTime.month)&&(0 == VdrOvertime.NightEndTime.year)))
		{
		    return ;
		}

		RTC_GetCurTime(&CurTime);
		NightStartTimeCount = VdrOvertime.NightStartTime.hour*3600+VdrOvertime.NightStartTime.min*60+VdrOvertime.NightStartTime.sec;
		CurrentTimeCount = CurTime.hour*3600+CurTime.min*60+CurTime.sec;
		
		if((CurrentTimeCount < NightStartTimeCount)&&(CurrentTimeCount > (NightStartTimeCount-VdrOvertime.NightVoiceKeeptime*60)))
		{
		    if(0 == count%(VdrOvertime.NightVoiceGroupTime*60))
				{
				    if(1 == VDROvertime_GetRunFlag())
						{
						    BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_VOICE_ID, Buffer);
                if(0 == BufferLen)
                {
                    strcpy((char *)Buffer,"��������ҹ��ʱ��");
                }
                else
                {
                    Buffer[BufferLen] = 0;
                }
                Public_PlayTTSVoiceStr(Buffer);
                Public_ShowTextInfo((char *)Buffer,100);
						}
						else
						{
						    count--;
						}
				}
				count++;
		}
		else if((CurrentTimeCount > NightStartTimeCount)&&(CurrentTimeCount < (NightStartTimeCount+VdrOvertime.NightVoiceKeeptime*60)))
		{
		    if(7 == count)//��ʱ7�벥������Ϊǰ����Ҫ��������XX����
				{
				    if(1 == VDROvertime_GetRunFlag())
						{
						    strcpy((char *)Buffer,"�ѽ���ҹ��ʱ��");
                Public_PlayTTSVoiceStr(Buffer);
                Public_ShowTextInfo((char *)Buffer,100);
						}
						else
						{
						    count--;
						}
				}
				count++;  
		}
		else
		{
		    count = 0;
		}
            
    
}
/**************************************************************************
//��������VDROvertime_IsAlarm
//���ܣ���ʱ��ʻ�ж�
//���룺��
//�������
//����ֵ��bit0��λ��ʾ��ʱԤ����bit1��λ��ʾ��ʱ����
//��ע��bit0��bit1����ͬʱΪ1
***************************************************************************/
static u8 VDROvertime_IsAlarm(void)
{
    static  u32 StopCount = 0;
    static  u32 LastTimeCount = 0;

    u32 CurrentTimeCount;
    u32 OffsetTime;
    u8  RunStatus;
    u8  AlarmFlag = 0;//bit0��λ��ʾ��ʱԤ����bit1��λ��ʾ��ʱ����

    RunStatus = VDROvertime_GetRunFlag();
    
    CurrentTimeCount = RTC_GetCounter();
    if((CurrentTimeCount > (LastTimeCount+2))&&(0 != LastTimeCount))//ʱ����ǰ����
    {
			  if(CurrentTimeCount > (LastTimeCount+VdrOvertime.MinRestTime))
				{
					  if((VdrOvertime.Driver[DriverNum].EndTimeCount > VdrOvertime.Driver[DriverNum].StartTimeCount)
							&&((VdrOvertime.Driver[DriverNum].EndTimeCount-VdrOvertime.Driver[DriverNum].StartTimeCount)>=VdrOvertime.AlarmTime))
						{
					      VDROvertime_Write(DriverNum);
						}
				    VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
				}
    }
		else if((CurrentTimeCount < (LastTimeCount-2))&&(0 != LastTimeCount))//ʱ���������
		{
		    if((VdrOvertime.Driver[DriverNum].EndTimeCount > VdrOvertime.Driver[DriverNum].StartTimeCount)
					&&((VdrOvertime.Driver[DriverNum].EndTimeCount-VdrOvertime.Driver[DriverNum].StartTimeCount) < (LastTimeCount-CurrentTimeCount)))
			  {
					  VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
				}
				
		}
    LastTimeCount = CurrentTimeCount;
    
    if(VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }
    
    OffsetTime = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
            
    if(OffsetTime >= 3*24*3600)//������ʻ����3�죬��Ϊ��һ�ִ������¼���
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }

    if(1 == RunStatus)//��ʻ��
    {
        StopCount = 0;
        if((CurrentTimeCount < VdrOvertime.Driver[DriverNum].StartTimeCount)||(0 == VdrOvertime.Driver[DriverNum].StartTimeCount))
        {
            AlarmFlag = 0;
            VdrOvertime.Driver[DriverNum].StartTimeCount = CurrentTimeCount;
            if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
            {
              Public_GetCurPositionInfoDataBlock(VdrOvertime.Driver[DriverNum].StartPosition);
            }
            else
            {
              VDRPub_GetNoNavigationPostion(VdrOvertime.Driver[DriverNum].StartPosition);
            }
            VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME|DRIVER_INFO_TYPE_START_POSITION);
        }
        else
        {
            OffsetTime = CurrentTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
            
            if((OffsetTime >= VdrOvertime.AlarmTime)&&(0 != VdrOvertime.AlarmTime))
            {
                AlarmFlag = 0x02;
            }
            else if((OffsetTime >= VdrOvertime.EarlyAlarmTime)&&(0 != VdrOvertime.EarlyAlarmTime))
            {
                AlarmFlag = 0x01;
            }
            else
            {
                AlarmFlag = 0;
            }
        }
        VdrOvertime.Driver[DriverNum].EndTimeCount = CurrentTimeCount;
        TestTimeCount3 = VdrOvertime.Driver[DriverNum].EndTimeCount;
        if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
        {
          Public_GetCurPositionInfoDataBlock(VdrOvertime.Driver[DriverNum].EndPosition);
        }
        else
        {
          VDRPub_GetNoNavigationPostion(VdrOvertime.Driver[DriverNum].EndPosition);
        }
				
        VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_END_TIME|DRIVER_INFO_TYPE_END_POSITION);
        
    }
    else//ͣʻ
    {
        StopCount++;

        if(StopCount < (VdrOvertime.MinRestTime+2))//��2����Ϊ����ϲ���Э���⣬��Ӱ����������
        {
            if(VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
            {
                AlarmFlag = 0;
                VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
                VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
                VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME);
            }
            else
            {
                OffsetTime = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
                if((OffsetTime >= VdrOvertime.AlarmTime)&&(0 != VdrOvertime.AlarmTime))
                {
                    AlarmFlag = 0x02;
                }
                else if((OffsetTime >= VdrOvertime.EarlyAlarmTime)&&(0 != VdrOvertime.EarlyAlarmTime))
                {
                    AlarmFlag = 0x01;
                }
                else
                {
                    AlarmFlag = 0;
                }
            }
        }
        else if(StopCount == (VdrOvertime.MinRestTime+2))
        {
            AlarmFlag = 0;
            OffsetTime = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
            if((OffsetTime >= VdrOvertime.AlarmTime)&&(VdrOvertime.Driver[DriverNum].EndTimeCount > VdrOvertime.Driver[DriverNum].StartTimeCount))
            {
                VDROvertime_Write(DriverNum);
								#if	TACHOGRAPHS_19056_TEST
								Modem_Debug_UartTx("33",2);
								#endif
            }
            VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
            VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME);
        }
        else 
        {
            AlarmFlag = 0;
        }
    }

        if(0x02 == (AlarmFlag&0x02))
        {
              Io_WriteOvertimeConrtolBit(OVER_TIME_BIT_ORDINARY, SET);
        }
        else
        {
              Io_WriteOvertimeConrtolBit(OVER_TIME_BIT_ORDINARY, RESET);
        }
				
				if(0x01 == (AlarmFlag&0x01))
        {
              Io_WritePreOvertimeConrtolBit(PRE_OVER_TIME_BIT_ORDINARY, SET);
        }
        else
        {
              Io_WritePreOvertimeConrtolBit(PRE_OVER_TIME_BIT_ORDINARY, RESET);
        }
			
    OvertimeAlarmFlag = AlarmFlag;
				
    return AlarmFlag;
    
}
/**************************************************************************
//��������VDROvertime_GetAlarmFlag
//���ܣ���ȡ��ʱ��ʻ��־��������ҹ�䳬ʱ��
//���룺��
//�������
//����ֵ��1����ʱ��0��δ��ʱ
//��ע��
***************************************************************************/
static u8 VDROvertime_GetAlarmFlag(void)
{
    return OvertimeAlarmFlag;
}
/**************************************************************************
//��������VDROvertime_NightIsAlarm
//���ܣ���ʱ��ʻ�ж�
//���룺��
//�������
//����ֵ��bit0��λ��ʾ��ʱԤ����bit1��λ��ʾ��ʱ����
//��ע��bit0��bit1����ͬʱΪ1
***************************************************************************/
static u8 VDROvertime_NightIsAlarm(void)
{
    static  u32 NightStopCount = 0;
    static  u32 NightLastTimeCount = 0;
	  static  u8  NightFlag = 0;

    u32 CurrentTimeCount;
    u32 OffsetTime;
    u8  RunStatus;
    u8  AlarmFlag = 0;//bit0��λ��ʾ��ʱԤ����bit1��λ��ʾ��ʱ����
	
	  if(0 == VDROvertime_NightIsArrived())
		{
			  if(1 == NightFlag)//�����ҹ��ʱ��
				{
				    Io_WriteOvertimeConrtolBit(OVER_TIME_BIT_NIGHT, RESET);//�����ʱ��ʻ��־
				}
			  NightFlag = 0;
		    return 0;
		}
		else
		{
		    NightFlag = 1;
		}
		

    RunStatus = VDROvertime_GetRunFlag();
    
    CurrentTimeCount = RTC_GetCounter();
    if((CurrentTimeCount > (NightLastTimeCount+2))&&(0 != NightLastTimeCount))//ʱ����ǰ����
    {
			  if(CurrentTimeCount > (NightLastTimeCount+VdrOvertime.NightMinRestTime))
				{
					  if((VdrOvertime.Driver[DriverNum].NightEndTimeCount > VdrOvertime.Driver[DriverNum].NightStartTimeCount)
							&&((VdrOvertime.Driver[DriverNum].NightEndTimeCount-VdrOvertime.Driver[DriverNum].NightStartTimeCount)>=VdrOvertime.NightAlarmTime))
						{
					      VDROvertime_NightWrite(DriverNum);
						}
				    VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
				}
    }
		else if((CurrentTimeCount < (NightLastTimeCount-2))&&(0 != NightLastTimeCount))//ʱ���������
		{
		    if((VdrOvertime.Driver[DriverNum].NightEndTimeCount > VdrOvertime.Driver[DriverNum].NightStartTimeCount)
					&&((VdrOvertime.Driver[DriverNum].NightEndTimeCount-VdrOvertime.Driver[DriverNum].NightStartTimeCount) < (NightLastTimeCount-CurrentTimeCount)))
			  {
					  VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
				}
				
		}
    NightLastTimeCount = CurrentTimeCount;
    
    if(VdrOvertime.Driver[DriverNum].NightStartTimeCount > VdrOvertime.Driver[DriverNum].NightEndTimeCount)
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }
    
    OffsetTime = VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount;
            
    if(OffsetTime >= 3*24*3600)//������ʻ����3�죬��Ϊ��һ�ִ������¼���
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }

    if(1 == RunStatus)//��ʻ��
    {
        NightStopCount = 0;
        
        if((CurrentTimeCount < VdrOvertime.Driver[DriverNum].NightStartTimeCount)||(0 == VdrOvertime.Driver[DriverNum].NightStartTimeCount))
        {
            AlarmFlag = 0;
            VdrOvertime.Driver[DriverNum].NightStartTimeCount = CurrentTimeCount;
            if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
            {
              Public_GetCurPositionInfoDataBlock(VdrOvertime.Driver[DriverNum].NightStartPosition);
            }
            else
            {
              VDRPub_GetNoNavigationPostion(VdrOvertime.Driver[DriverNum].NightStartPosition);
            }
            VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_NIGHT_START_TIME|DRIVER_INFO_TYPE_NIGHT_START_POSITION);
        }
        else
        {
            OffsetTime = CurrentTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount;
            
            if((OffsetTime >= VdrOvertime.NightAlarmTime)&&(0 != VdrOvertime.NightAlarmTime))
            {
                AlarmFlag = 0x02;
            }
            else if((OffsetTime >= VdrOvertime.NightEarlyAlarmTime)&&(0 != VdrOvertime.NightEarlyAlarmTime))
            {
                AlarmFlag = 0x01;
            }
            else
            {
                AlarmFlag = 0;
            }
        }
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = CurrentTimeCount;
        TestTimeCount3 = VdrOvertime.Driver[DriverNum].NightEndTimeCount;
        if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
      {
          Public_GetCurPositionInfoDataBlock(VdrOvertime.Driver[DriverNum].NightEndPosition);
        }
        else
        {
          VDRPub_GetNoNavigationPostion(VdrOvertime.Driver[DriverNum].NightEndPosition);
        }
        VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_NIGHT_END_TIME|DRIVER_INFO_TYPE_NIGHT_END_POSITION);
        
    }
    else//ͣʻ
    {
        NightStopCount++;

        if(NightStopCount < VdrOvertime.NightMinRestTime)
        {
            if(VdrOvertime.Driver[DriverNum].NightStartTimeCount > VdrOvertime.Driver[DriverNum].NightEndTimeCount)
            {
                AlarmFlag = 0;
                VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
                VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
                VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_NIGHT_START_TIME);
            }
            else
            {
                OffsetTime = VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount;
                if((OffsetTime >= VdrOvertime.NightAlarmTime)&&(0 != VdrOvertime.NightAlarmTime))
                {
                    AlarmFlag = 0x02;
                }
                else if((OffsetTime >= VdrOvertime.NightEarlyAlarmTime)&&(0 != VdrOvertime.NightEarlyAlarmTime))
                {
                    AlarmFlag = 0x01;
                }
                else
                {
                    AlarmFlag = 0;
                }
            }
        }
        else if(NightStopCount == VdrOvertime.NightMinRestTime)
        {
            AlarmFlag = 0;
            OffsetTime = VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount;
            if(OffsetTime >= VdrOvertime.NightAlarmTime)
            {
                VDROvertime_NightWrite(DriverNum);
            }
            VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
            VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_NIGHT_START_TIME);
        }
        else 
        {
            AlarmFlag = 0;
        }
    }

        if(0x02 == (AlarmFlag&0x02))
        {
              Io_WriteOvertimeConrtolBit(OVER_TIME_BIT_NIGHT, SET);
        }
        else
        {
              Io_WriteOvertimeConrtolBit(OVER_TIME_BIT_NIGHT, RESET);
        }
				
				if(0x01 == (AlarmFlag&0x01))
        {
              Io_WritePreOvertimeConrtolBit(PRE_OVER_TIME_BIT_NIGHT, SET);
        }
        else
        {
              Io_WritePreOvertimeConrtolBit(PRE_OVER_TIME_BIT_NIGHT, RESET);
        }
				
    return AlarmFlag;
    
}
/**************************************************************************
//��������VDROvertime_NightIsArrived
//���ܣ��жϵ�ǰ�Ƿ�Ϊҹ��ʱ��
//���룺��
//�������
//����ֵ��1����ǰ��ҹ��ʱ�䣻0����ǰ����ҹ��ʱ��
//��ע��
***************************************************************************/
static u8 VDROvertime_NightIsArrived(void)
{
	  TIME_T CurTime;
	  u32 TimeCount;
	  u32 NightStartTimeCount;
	  u32 NightEndTimeCount;
	
	  if(1 != VDROvertime_NightTimeIsOk(VdrOvertime.NightStartTime))
		{
		    return 0;
		}
		
		if(1 != VDROvertime_NightTimeIsOk(VdrOvertime.NightEndTime))
		{
		    return 0;
		}
		
		RTC_GetCurTime(&CurTime);
		NightStartTimeCount = VdrOvertime.NightStartTime.hour*3600+VdrOvertime.NightStartTime.min*60+VdrOvertime.NightStartTime.sec;
		NightEndTimeCount = VdrOvertime.NightEndTime.hour*3600+VdrOvertime.NightEndTime.min*60+VdrOvertime.NightEndTime.sec;
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
//��������VDROvertime_NightTimeIsOk
//���ܣ����ҹ��ʱ���Ƿ���ȷ
//���룺��
//�������
//����ֵ��1����ȷ��0������ȷ
//��ע��
***************************************************************************/
static u8 VDROvertime_NightTimeIsOk(TIME_T Time)
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
/**************************************************************************
//��������VDROvertime_Write
//���ܣ�д��ʱ��ʻ
//���룺��ʻԱ��ţ�ȡֵ��Χ��0-5
//�������
//����ֵ����
//��ע�����浽flash VDR_DATA_TYPE_OVERTIME�洢������
***************************************************************************/
static void VDROvertime_Write(u8 DriverNum)
{
    u8  Buffer[VDR_DATA_OVER_TIME_STEP_LEN];
    TIME_T  Time;
    u32 TimeCount = 0;

    if((0 == VdrOvertime.Driver[DriverNum].StartTimeCount)
           ||(VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
       ||(DriverNum >= DRIVER_NUM_MAX))
    {
        return ;
    }

		
    //��ʻ֤����
    memcpy(Buffer,VdrOvertime.Driver[DriverNum].License,18);

    //��ʼʱ��
    TimeCount = VdrOvertime.Driver[DriverNum].StartTimeCount;
    if(TimeCount > 10)
    {
      TimeCount -= 10;
    }
    Gmtime(&Time, TimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+18,&Time);

    //����ʱ��
    TimeCount = VdrOvertime.Driver[DriverNum].EndTimeCount;
    if(TimeCount > 10)
    {
      TimeCount -= 10;
    }
    Gmtime(&Time, TimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+24,&Time);

    //��ʼλ��
    memcpy(Buffer+30,VdrOvertime.Driver[DriverNum].StartPosition,10);

    //����λ��
    memcpy(Buffer+40,VdrOvertime.Driver[DriverNum].EndPosition,10);

    VDRData_Write(VDR_DATA_TYPE_OVERTIME, Buffer, VDR_DATA_OVER_TIME_STEP_LEN-5, VdrOvertime.Driver[DriverNum].EndTimeCount);
    
}
/**************************************************************************
//��������VDROvertime_NightWrite
//���ܣ�д��ʱ��ʻ
//���룺��ʻԱ��ţ�ȡֵ��Χ��0-5
//�������
//����ֵ����
//��ע�����浽flash VDR_DATA_TYPE_OVERTIME�洢������
***************************************************************************/
static void VDROvertime_NightWrite(u8 DriverNum)
{
    u8  Buffer[VDR_DATA_OVER_TIME_STEP_LEN];
    TIME_T  Time;
    u32 TimeCount = 0;

    if((0 == VdrOvertime.Driver[DriverNum].NightStartTimeCount)
           ||(VdrOvertime.Driver[DriverNum].NightStartTimeCount > VdrOvertime.Driver[DriverNum].NightEndTimeCount)
           ||(DriverNum >= DRIVER_NUM_MAX)
		       ||(1 == Io_ReadOvertimeConrtolBit(OVER_TIME_BIT_ORDINARY)))
    {
        return ;
    }


    //��ʻ֤����
    memcpy(Buffer,VdrOvertime.Driver[DriverNum].License,18);

    //��ʼʱ��
    TimeCount = VdrOvertime.Driver[DriverNum].NightStartTimeCount;
    if(TimeCount > 10)
    {
      TimeCount -= 10;
    }
    Gmtime(&Time, TimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+18,&Time);

    //����ʱ��
    TimeCount = VdrOvertime.Driver[DriverNum].NightEndTimeCount;
    if(TimeCount > 10)
    {
      TimeCount -= 10;
    }
    Gmtime(&Time, TimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+24,&Time);

    //��ʼλ��
    memcpy(Buffer+30,VdrOvertime.Driver[DriverNum].NightStartPosition,10);

    //����λ��
    memcpy(Buffer+40,VdrOvertime.Driver[DriverNum].NightEndPosition,10);

    VDRData_Write(VDR_DATA_TYPE_OVERTIME, Buffer, VDR_DATA_OVER_TIME_STEP_LEN-5, VdrOvertime.Driver[DriverNum].NightEndTimeCount);
    
}

/**************************************************************************
//��������VDROvertime_SaveCurrentDriverInfo
//���ܣ����浱ǰ��ʻԱ��Ϣ
//���룺Type:�������������͵����
//DRIVER_INFO_TYPE_DRIVER_NUM
//DRIVER_INFO_TYPE_DRIVER_LICENSE
//DRIVER_INFO_TYPE_START_TIME
//DRIVER_INFO_TYPE_END_TIME
//DRIVER_INFO_TYPE_START_POSITION
//DRIVER_INFO_TYPE_END_POSITION
//DRIVER_INFO_TYPE_LOGIN_TIME
//DRIVER_INFO_TYPE_LOGOUT_TIME
//DRIVER_INFO_TYPE_NIGHT_START_TIME					
//DRIVER_INFO_TYPE_NIGHT_END_TIME						
//DRIVER_INFO_TYPE_NIGHT_START_POSITION			
//DRIVER_INFO_TYPE_NIGHT_END_POSITION				
//�������
//����ֵ����
//��ע������ǰRAM�е���Ϣ���浽FRAM�У����ݼ��ṹ��VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_SaveCurrentDriverInfo(u16 Type)
{
    u8  Buffer[5];
    u32 TimeCount = 0;
	  TIME_T Time;
	  TIME_T CurTime;
	  
    //��ʻԱ���
    if(DRIVER_INFO_TYPE_DRIVER_NUM == (Type&DRIVER_INFO_TYPE_DRIVER_NUM))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR, &DriverNum, 1);
    }

    //��ʻ֤����
    if(DRIVER_INFO_TYPE_DRIVER_LICENSE == (Type&DRIVER_INFO_TYPE_DRIVER_LICENSE))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_LICENSE_ADDR, VdrOvertime.Driver[DriverNum].License, 18);
    }

    //��ʼʱ��
    if(DRIVER_INFO_TYPE_START_TIME == (Type&DRIVER_INFO_TYPE_START_TIME))
    {
        TimeCount = VdrOvertime.Driver[DriverNum].StartTimeCount;
        if(TimeCount > 10)
        {
          TimeCount -= 10;
        }
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_START_TIME_ADDR, Buffer, 4);
    }

    //����ʱ��
    if(DRIVER_INFO_TYPE_END_TIME == (Type&DRIVER_INFO_TYPE_END_TIME))
    {
        TimeCount = VdrOvertime.Driver[DriverNum].EndTimeCount;
        if(TimeCount > 10)
        {
          TimeCount -= 10;
        }
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_END_TIME_ADDR, Buffer, 4);
    }

    //��ʼλ��
    if(DRIVER_INFO_TYPE_START_POSITION == (Type&DRIVER_INFO_TYPE_START_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_START_POSITION_ADDR, VdrOvertime.Driver[DriverNum].StartPosition, 10);
    }

    //����λ��
    if(DRIVER_INFO_TYPE_END_POSITION == (Type&DRIVER_INFO_TYPE_END_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_END_POSITION_ADDR, VdrOvertime.Driver[DriverNum].EndPosition, 10);
    }
		
		//ҹ�俪ʼʱ��
    if(DRIVER_INFO_TYPE_NIGHT_START_TIME == (Type&DRIVER_INFO_TYPE_NIGHT_START_TIME))
    {
        TimeCount = VdrOvertime.Driver[DriverNum].NightStartTimeCount;
        if(TimeCount > 10)
        {
          TimeCount -= 10;
        }
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_NIGHT_START_TIME_ADDR, Buffer, 4);
    }

    //ҹ�����ʱ��
    if(DRIVER_INFO_TYPE_NIGHT_END_TIME == (Type&DRIVER_INFO_TYPE_NIGHT_END_TIME))
    {
        TimeCount = VdrOvertime.Driver[DriverNum].NightEndTimeCount;
        if(TimeCount > 10)
        {
          TimeCount -= 10;
        }
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_NIGHT_END_TIME_ADDR, Buffer, 4);
    }

    //ҹ�俪ʼλ��
    if(DRIVER_INFO_TYPE_NIGHT_START_POSITION == (Type&DRIVER_INFO_TYPE_NIGHT_START_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR, VdrOvertime.Driver[DriverNum].NightStartPosition, 10);
    }

    //ҹ�����λ��
    if(DRIVER_INFO_TYPE_NIGHT_END_POSITION == (Type&DRIVER_INFO_TYPE_NIGHT_END_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR, VdrOvertime.Driver[DriverNum].NightEndPosition, 10);
    }

    //ǩ��ʱ��
    if(DRIVER_INFO_TYPE_LOGIN_TIME == (Type&DRIVER_INFO_TYPE_LOGIN_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].LoginTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR, Buffer, 4);
    }

    //ǩ��ʱ��
    if(DRIVER_INFO_TYPE_LOGOUT_TIME == (Type&DRIVER_INFO_TYPE_LOGOUT_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].LogoutTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR, Buffer, 4);
    }
		
		//ÿ���ۼƼ�ʻʱ��
    if(DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME == (Type&DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME))
    {
			  Gmtime(&Time, VdrOvertime.Driver[DriverNum].EndTimeCount);
	      RTC_GetCurTime(&CurTime);
		    if((CurTime.day != Time.day)
			    &&(0 != VdrOvertime.Driver[DriverNum].StartTimeCount)
		      &&(0 != VdrOvertime.Driver[DriverNum].EndTimeCount))
		    {
	          VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount = 0;
		    }
		
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_ONE_DAY_DRIVE_TIME_ADDR, Buffer, 4);
    }
		
		
    
}
/**************************************************************************
//��������VDROvertime_ReadCurrentDriverInfo
//���ܣ�����ǰ��ʻԱ��Ϣ
//���룺��
//�������
//����ֵ����
//��ע���ӱ��浽FRAM�е���Ϣ��ȡ��RAM�У����ݼ��ṹ��VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_ReadCurrentDriverInfo(void)
{
    u8  Buffer[30];
	  TIME_T Time;
	  TIME_T CurTime;

    //��ʻԱ���
    if(1 == FRAM_BufferRead(Buffer, 1,FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR))
    {
        DriverNum = Buffer[0];
    }

    //��ʻ֤����
    if(18 == FRAM_BufferRead(Buffer, 18,FRAM_VDR_OVERTIME_LICENSE_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].License,Buffer,18);
    }

    //��ʼʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_START_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //����ʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_END_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].EndTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //��ʼλ��
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_START_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].StartPosition,Buffer,10);
    }

    //����λ��
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_END_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].EndPosition,Buffer,10);
    }
		
		//ҹ�俪ʼʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_NIGHT_START_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //ҹ�����ʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_NIGHT_END_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //ҹ�俪ʼλ��
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].NightStartPosition,Buffer,10);
    }

    //ҹ�����λ��
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].NightEndPosition,Buffer,10);
    }

    //ǩ��ʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].LoginTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //ǩ��ʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].LogoutTimeCount = Public_ConvertBufferToLong(Buffer);
    }
		
		//ÿ���ۼƼ�ʻʱ��
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_ONE_DAY_DRIVE_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount = Public_ConvertBufferToLong(Buffer);
			  Gmtime(&Time, VdrOvertime.Driver[DriverNum].EndTimeCount);
	      RTC_GetCurTime(&CurTime);
		    if((CurTime.day != Time.day)
					&&(0 != VdrOvertime.Driver[DriverNum].StartTimeCount)
				  &&(0 != VdrOvertime.Driver[DriverNum].EndTimeCount))
		    {
	          VdrOvertime.Driver[DriverNum].OneDayDriveTimeCount = 0;
		    }
    }
	  
	  
    
}
/**************************************************************************
//��������VDROvertime_SaveAllDriverInfo
//���ܣ��������м�ʻԱ��Ϣ
//���룺��
//�������
//����ֵ����
//��ע����RAM�е���Ϣ���浽FLASH�У����ݼ��ṹ��VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_SaveAllDriverInfo(void)
{
    u8  i;
    u8  Buffer[20];
    u8  sum;
    u32 Addr;
    

    //�Ȳ���
    Addr = FLASH_OVER_TIME_DRIVE_INFO_START_ADDR;
    sFLASH_EraseSector(Addr);
    
    //һ��һ��д��
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        //д���ʻ֤����
        memcpy(Buffer,VdrOvertime.Driver[i].License,18);
        sum = Public_GetSumVerify(Buffer,18);
        Buffer[18] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 19);
        Addr += 19;

        //д�뿪ʼʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].StartTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //д�����ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].EndTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //д�뿪ʼλ��
        memcpy(Buffer,VdrOvertime.Driver[i].StartPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;

        //д�����λ��
        memcpy(Buffer,VdrOvertime.Driver[i].EndPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;
				
				//д��ҹ�俪ʼʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].NightStartTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //д��ҹ�����ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].NightEndTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //д��ҹ�俪ʼλ��
        memcpy(Buffer,VdrOvertime.Driver[i].NightStartPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;

        //д��ҹ�����λ��
        memcpy(Buffer,VdrOvertime.Driver[i].NightEndPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;

        //д��ǩ��ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LoginTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //д��ǩ��ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LogoutTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;
				
				//д��ÿ���ۼƼ�ʻʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].OneDayDriveTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;
            
    }

    //д�뵱ǰ��ʻԱ���
    FRAM_BufferWrite(FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR, &DriverNum, 1);
}
/**************************************************************************
//��������VDROvertime_ReadAllDriverInfo
//���ܣ������м�ʻԱ��Ϣ
//���룺��
//�������
//����ֵ����
//��ע���ӱ��浽FLASH�е���Ϣ��ȡ��RAM�У����ݼ��ṹ��VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_ReadAllDriverInfo(void)
{
    u8  i;
    u8  j;
    u8  flag[DRIVER_NUM_MAX];
    u8  Buffer[20];
    u8  sum;
    u32 Addr;
    u32 TimeCount;
	  TIME_T CurTime;
	  TIME_T Time;

    Addr = FLASH_OVER_TIME_DRIVE_INFO_START_ADDR;

    //һ��һ������
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        flag[i] = 0;

        //������ʻ֤����
        sFLASH_ReadBuffer(Buffer,Addr,19);
        sum = Public_GetSumVerify(Buffer,18);
        if(sum == Buffer[18])
        {
            memcpy(VdrOvertime.Driver[i].License,Buffer,18);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 19;

        //������ʼʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].StartTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;
        
        

        //��������ʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].EndTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;

        //������ʼλ��
        sFLASH_ReadBuffer(Buffer,Addr,11);
        sum = Public_GetSumVerify(Buffer,10);
        if(sum == Buffer[10])
        {
            memcpy(VdrOvertime.Driver[i].StartPosition,Buffer,10);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 11;

        //��������λ��
        sFLASH_ReadBuffer(Buffer,Addr,11);
        sum = Public_GetSumVerify(Buffer,10);
        if(sum == Buffer[10])
        {
            memcpy(VdrOvertime.Driver[i].EndPosition,Buffer,10);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 11;
				
				//����ҹ�俪ʼʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].NightStartTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;
        
        

        //����ҹ�����ʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].NightEndTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;

        //����ҹ�俪ʼλ��
        sFLASH_ReadBuffer(Buffer,Addr,11);
        sum = Public_GetSumVerify(Buffer,10);
        if(sum == Buffer[10])
        {
            memcpy(VdrOvertime.Driver[i].NightStartPosition,Buffer,10);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 11;

        //����ҹ�����λ��
        sFLASH_ReadBuffer(Buffer,Addr,11);
        sum = Public_GetSumVerify(Buffer,10);
        if(sum == Buffer[10])
        {
            memcpy(VdrOvertime.Driver[i].NightEndPosition,Buffer,10);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 11;

        
        //����ǩ��ʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].LoginTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;
        
        

        //����ǩ��ʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].LogoutTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;
				
				//����ÿ���ۼƼ�ʻʱ��
        sFLASH_ReadBuffer(Buffer,Addr,5);
        sum = Public_GetSumVerify(Buffer,4);
        if(sum == Buffer[4])
        {
            VdrOvertime.Driver[i].OneDayDriveTimeCount = Public_ConvertBufferToLong(Buffer);
        }
        else
        {
            flag[i] = 1;
        }
        Addr += 5;
				
				
				Gmtime(&Time, VdrOvertime.Driver[i].EndTimeCount);
	      RTC_GetCurTime(&CurTime);
		    if((CurTime.day != Time.day)
					&&(0 != VdrOvertime.Driver[i].StartTimeCount)
				  &&(0 != VdrOvertime.Driver[i].EndTimeCount))
		    {
	          VdrOvertime.Driver[i].OneDayDriveTimeCount = 0;
		    }
				
        if(1 == flag[i])//���ݴ�����0
        {
            for(j=0;j<20;j++)
            {
                Buffer[j] = 0;
            }
            memcpy(VdrOvertime.Driver[i].License,Buffer,18);
            memcpy(VdrOvertime.Driver[i].StartPosition,Buffer,10);
            memcpy(VdrOvertime.Driver[i].EndPosition,Buffer,10);
						memcpy(VdrOvertime.Driver[i].NightStartPosition,Buffer,10);
            memcpy(VdrOvertime.Driver[i].NightEndPosition,Buffer,10);
            VdrOvertime.Driver[i].StartTimeCount = 0;
            VdrOvertime.Driver[i].EndTimeCount = 0;
						VdrOvertime.Driver[i].NightStartTimeCount = 0;
            VdrOvertime.Driver[i].NightEndTimeCount = 0;
            VdrOvertime.Driver[i].LoginTimeCount = 0;
            VdrOvertime.Driver[i].LogoutTimeCount = 0;
						VdrOvertime.Driver[i].OneDayDriveTimeCount = 0;
        }
    }

    //������ǰ��ʻԱ���
    if(1 == FRAM_BufferRead(Buffer, 1, FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR))
    {
        if(Buffer[0] < DRIVER_NUM_MAX)
        {
                DriverNum = Buffer[0];
                VDROvertime_ReadCurrentDriverInfo();
        }
        else
        {
            DriverNum = 0;
        }
    }
    else
    {
        DriverNum = 0;  
    }
    
    if(0 == DriverNum)
    {
        for(j=0;j<20;j++)
        {
            Buffer[j] = '0';
        }
        memcpy(VdrOvertime.Driver[DriverNum].License,Buffer,18);
        VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_DRIVER_LICENSE);
        DriverLoginFlag = 0;
    }
    else
    {
        DriverLoginFlag = 1;
    }

    TimeCount = RTC_GetCounter();
    if(VdrOvertime.Driver[DriverNum].StartTimeCount >= VdrOvertime.Driver[DriverNum].EndTimeCount)//���¼���ʱ��
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }
    else if((VdrOvertime.Driver[DriverNum].EndTimeCount >= TimeCount))//��ǰRTCʱ�����
    {
      if((0 != VdrOvertime.MinRestTime)
      &&((VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount) > VdrOvertime.MinRestTime))
        {
          VDROvertime_Write(DriverNum); 
					#if	TACHOGRAPHS_19056_TEST
					Modem_Debug_UartTx("44",2);
					#endif
        }
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }
    else if(((VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount) > VdrOvertime.MinRestTime)
         &&(0 != VdrOvertime.MinRestTime)&&(1 == GBTestFlag))
    {
        VDROvertime_Write(DriverNum);   
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
        //VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME);
    }
    else if(((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime)&&(0 != VdrOvertime.MinRestTime))
    {
			  if((VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount) > VdrOvertime.AlarmTime)
				{
				    VDROvertime_Write(DriverNum);
						#if	TACHOGRAPHS_19056_TEST
						Modem_Debug_UartTx("55",2);
						Gmtime(&Time, VdrOvertime.Driver[DriverNum].EndTimeCount);
						Modem_Debug_UartTx((u8*)&Time.year,6);
						Gmtime(&Time, VdrOvertime.Driver[DriverNum].StartTimeCount);
						Modem_Debug_UartTx((u8*)&Time.year,6);
						#endif
				}
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }
		
		if(VdrOvertime.Driver[DriverNum].NightStartTimeCount >= VdrOvertime.Driver[DriverNum].NightEndTimeCount)//���¼���ʱ��
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }
    else if((VdrOvertime.Driver[DriverNum].NightEndTimeCount >= TimeCount))//��ǰRTCʱ�����
    {
      if((0 != VdrOvertime.NightMinRestTime)
      &&((VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount) > VdrOvertime.NightMinRestTime))
        {
          VDROvertime_NightWrite(DriverNum); 
        }
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }
    else if(((VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount) > VdrOvertime.NightMinRestTime)
         &&(0 != VdrOvertime.NightMinRestTime)&&(1 == GBTestFlag))
    {
        VDROvertime_NightWrite(DriverNum);   
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
        //VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_NIGHT_START_TIME);
    }
    else if(((TimeCount-VdrOvertime.Driver[DriverNum].NightEndTimeCount) > VdrOvertime.NightMinRestTime)&&(0 != VdrOvertime.NightMinRestTime))
    {
			  if((VdrOvertime.Driver[DriverNum].NightEndTimeCount - VdrOvertime.Driver[DriverNum].NightStartTimeCount) > VdrOvertime.NightAlarmTime)
				{
				    VDROvertime_NightWrite(DriverNum);
				}
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }
}

///////////////////////////����Ϊ�����ͼ����//////////////////////////////////////
/**************************************************************************
//��������VDROvertime_OnceRunReport
//���ܣ�����һ����ʻ��ͣʻ�������ϱ�һ��λ����Ϣ
//���룺��
//�������
//����ֵ����
//��ע������Э���ⳬʱ��ʻʱ���ص���Գ�ʱ��ʻ��ʼʱ�̺ͱ���ʱ��
***************************************************************************/
static void VDROvertime_OnceRunReport(void)
{
	  static u8 count1 = 0;
	  static u8 count2 = 0;
	  static u8 LastSpeed = 0;
	  u8	Speed;
	
	  if(0 == BBXYTestFlag)
		{
		    return ;
		}
	
	  Speed = VDRSpeed_GetCurSpeed();
	  if((Speed > 0)&&(Speed < VDROverspeed_GetAlarmSpeed())&&(0 == LastSpeed))
	  {
		    Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);
			  count1++;
			  if(count1 >= 2)
				{
				    count1 = 0;
				}
	  }
		else if((0 == Speed)&&(LastSpeed > 0))
	  {
		    Report_UploadPositionInfo(CHANNEL_DATA_1|CHANNEL_DATA_2);
			  count2++;
			  if(count2 >= 1)
				{
				    count2 = 0;
				}
	  }
		
		LastSpeed = Speed;
	
}
/**************************************************************************
//��������VDROvertime_StopAlarm
//���ܣ���ʱͣ������
//���룺��
//�������
//����ֵ����
//��ע������Э����
***************************************************************************/
static void VDROvertime_StopAlarm(void)
{
    u8	Speed;
  
	  //if(0 == BBXYTestFlag)
		//{
		    //return ;
		//}
	 
	  Speed = VDRSpeed_GetCurSpeed();
	  if(0 == Speed)
	  {
			  StopAlarmCount++;
			  if(StopAlarmCount > MaxStopTime)
				{ 
					  Io_WriteAlarmBit(ALARM_BIT_STOP_OVER, SET);
				}
				else
				{
				    Io_WriteAlarmBit(ALARM_BIT_STOP_OVER, RESET);
				}
	  }
		else
		{
			  StopAlarmCount = 0;
			  Io_WriteAlarmBit(ALARM_BIT_STOP_OVER, RESET);
		}
}
/**************************************************************************
//��������VDROvertime_ClearStartTime
//���ܣ������ʼʱ�䣬���»�ȡ��ʼʱ��
//���룺��
//�������
//����ֵ����
//��ע������Э����ƣ�ͼ�ʻʱ��������ƣ�ͼ�ʻ����ʱ��Ҫ���ô˺���
***************************************************************************/
void VDROvertime_ClearStartTime(void)
{
    VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
    VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
}





