/************************************************************************
//�������ƣ�VDR_Overtime.c
//���ܣ���ʱ��ʻ
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
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_rtc.h"
#include "Can.h"
/********************���ر���*************************/
//static    u8  OvertimeRunSpeed = 0;//��ʻ�ж��ٶ�
//static    u32 OvertimeRunKeepTime = 0;//��ʻ�ж�����ʱ��
//static    u8  OvertimeLastRunStatus = 0;//��һ����ʻ״̬
static  u8  DriverNum = 0;//��ǰ��ʻԱ��ţ�ȡֵ��Χ0-5
static  u8  DriverLoginFlag = 0;//��ʻԱ��¼��־��1Ϊǩ������¼����0Ϊǩ�ˣ�δ��¼��
static  u8  OvertimeUnloginVoiceEnableFlag = 0;//��ʻԱδ��¼�������ѿ��أ�1ʹ�ܣ�0��ֹ
static  u8  OvertimeInitFlag = 0;
static  VDR_OVERTIME_STRUCT VdrOvertime;
/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static u8 VDROvertime_RunOrStop(void);
static void VDROvertime_Init(void);
static void VDROvertime_HandleNormalAlarm(void);
static void VDROvertime_HandleOtherDrivers(void);
static void VDROvertime_HandleRunUnlogin(void);
static u8 VDROvertime_IsAlarm(void);
static void VDROvertime_Write(u8 DriverNum);
static void VDROvertime_SaveCurrentDriverInfo(u8 Type);
static void VDROvertime_ReadCurrentDriverInfo(void);
static void VDROvertime_SaveAllDriverInfo(void);
static void VDROvertime_ReadAllDriverInfo(void);

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
    //static u32    CanID = 0;
    //static    u8  CanBuffer[13] = {0,0,0,0,0,1,2,3,4,5,6,7,8};
    
    if(DriverNum >= DRIVER_NUM_MAX)
    {
        DriverNum = 0;
        DriverLoginFlag = 0;
    }

    if(0 == OvertimeInitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED))
        {
            VDROvertime_Init();
            OvertimeInitFlag = 1;
        }
    }
    else
    {
        VDROvertime_HandleNormalAlarm();//���泬ʱԤ������ʱ����������Ե�ǰ��ʻԱ��

        //VDROvertime_HandleNightAlarm();//ҹ�䳬ʱԤ������ʱ����������Ե�ǰ��ʻԱ��

        VDROvertime_HandleOtherDrivers();//���ѵ���С��Ϣʱ��ļ�ʻԱ��ʼʱ����0�������Ѳ����ĳ�ʱ��ʻ��¼�����������ʻԱ��

        VDROvertime_HandleRunUnlogin();//��ʻԱδ��¼��ʻ���ѣ�������
    }
    /*
    CanID++;
    CanBuffer[0] = (CanID&0xFF000000) >> 24;
    CanBuffer[1] = (CanID&0xFF0000) >> 16;
    CanBuffer[2] = (CanID&0xFF00) >> 8;
    CanBuffer[3] = CanID&0xFF;
    CanBuffer[4] = 8;
    CanBuffer[5] = (CanID&0xFF000000) >> 24;
    CanBuffer[6] = (CanID&0xFF0000) >> 16;
    CanBuffer[7] = (CanID&0xFF00) >> 8;
    CanBuffer[8] = CanID&0xFF;
    CanBuffer[9] = (CanID&0xFF000000) >> 24;
    CanBuffer[10] = (CanID&0xFF0000) >> 16;
    CanBuffer[11] = (CanID&0xFF00) >> 8;
    CanBuffer[12] = CanID&0xFF;
    CanBus_Send(CanBuffer, 13);
    */
    
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
    u8  Buffer[5];
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
    //VdrOvertime.AlarmTime = 300;//����ʱʹ�ø����

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer);//������������ʱ��
  if(E2_TIRED_DRIVE_ALARM_TIME_ID_LEN == BufferLen)
  {
    VdrOvertime.AlarmVoiceKeeptime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmVoiceKeeptime = 30;//��λ���ӣ�Ĭ��Ϊ30����
    }
    //VdrOvertime.AlarmVoiceKeeptime = 5;//����ʱʹ�ø����
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer);//��������ÿ����ʾ���
  if(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN == BufferLen)
  {
    VdrOvertime.AlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmVoiceGroupTime = 5;//��λ���ӣ�Ĭ��Ϊ5����
    }
    //VdrOvertime.AlarmVoiceGroupTime = 1;//����ʱʹ�ø����

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
    //VdrOvertime.MinRestTime = 120;//����ʱʹ�ø����

    BufferLen = EepromPram_ReadPram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer);//Ԥ����ֵ
  if(E2_TIRE_EARLY_ALARM_DVALUE_LEN == BufferLen)
  {
    temp = Public_ConvertBufferToLong(Buffer);
        if(temp > VdrOvertime.AlarmTime)
        {
            temp = 1800;
        }
  }
    else
    {
        temp = 1800;//��λ�룬Ĭ��Ϊ1800��
    }
    //temp = 120;//����ʱʹ�ø����
    VdrOvertime.EarlyAlarmTime = VdrOvertime.AlarmTime - temp;//Ԥ��ʱ��

    
    VdrOvertime.EarlyAlarmKeeptime = VdrOvertime.AlarmVoiceKeeptime;//Ԥ������ʱ�䣬Ĭ�ϵ��ڱ�����������ʱ��
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);//Ԥ������ÿ����ʾ���
  if(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN == BufferLen)
  {
     VdrOvertime.EarlyAlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.EarlyAlarmVoiceGroupTime = 5;//��λ���ӣ�Ĭ��Ϊ5����
    }
    //VdrOvertime.EarlyAlarmVoiceGroupTime = 1;//����ʱʹ�ø����
    
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

    BufferLen = EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, Buffer);//��ʻԱδ��¼��ʾ����ʹ�ܿ���
  if(E2_UNLOGIN_TTS_PLAY_ID_LEN == BufferLen)
  {
    OvertimeUnloginVoiceEnableFlag = Buffer[0];
  }
    else
    {
        OvertimeUnloginVoiceEnableFlag = 0;
    }
    //OvertimeUnloginVoiceEnableFlag = 1;//����ʱʹ�ø����
    
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
    u8  flag;
    u8  Num;
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
    
    if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
        ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount)
            ||((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime))//���¼���ʱ��
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }

    
    if(0 != Num)
    {
        TimeCount = RTC_GetCounter();
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
    }
    else
    {
        DriverLoginFlag = 0;
    }
    
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
//��������VDROvertime_GetLog
//���ܣ�����ʱ�洢����RAM�����磩��ȡָ��ʱ��֮ǰ�ĳ�ʱ��ʻ��¼
//���룺ָ��ʱ��
//�����
//����ֵ�����ݳ���
//��ע����RAM�������ж�ȡ��δд�뵽FLASH�еĳ�ʱ��ʻ��¼
***************************************************************************/
u16 VDROvertime_GetLog(u8 *pBuffer, TIME_T Time)
{
    u8  *p = NULL;
    u8  i;
    u16 length;
    u32 TimeCount;

    p = pBuffer;
    TimeCount = ConverseGmtime(&Time);
    length = 0;
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        if((0 != VdrOvertime.Driver[i].StartTimeCount)
            &&((VdrOvertime.Driver[i].EndTimeCount-VdrOvertime.Driver[i].StartTimeCount) > VdrOvertime.AlarmTime)
                &&(TimeCount >= VdrOvertime.Driver[i].EndTimeCount))
        {
            //��ʻ֤����
            memcpy(p,VdrOvertime.Driver[i].License,18);
            p += 18;
    
            //��ʼʱ��
            Gmtime(&Time, VdrOvertime.Driver[i].StartTimeCount);
            if(0 == Public_CheckTimeStruct(&Time))
            {
                p -= 18;
                break;
            }
            VDRPub_ConvertTimeToBCD(p,&Time);
            p += 6;

            //����ʱ��
            Gmtime(&Time, VdrOvertime.Driver[i].EndTimeCount);
            if(0 == Public_CheckTimeStruct(&Time))
            {
                p -= 24;
                break;
            }
            VDRPub_ConvertTimeToBCD(p,&Time);
            p += 6;

            //��ʼλ��
            memcpy(p,VdrOvertime.Driver[i].StartPosition,10);
            p += 10;

            //����λ��
            memcpy(p,VdrOvertime.Driver[i].EndPosition,10);
            p += 10;
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

//////////////////////////////////����Ϊ�ڲ�����///////////////////////////////////////
/**************************************************************************
//��������VDROvertime_RunOrStop
//���ܣ���ʻ��ͣʻ���ж�
//���룺��
//�������
//����ֵ��Ϊ1Ϊ��ʻ��Ϊ0Ϊͣʻ
//��ע:
***************************************************************************/
static u8 VDROvertime_RunOrStop(void)
{
    static u16 RunCount = 0;
    static u16 StopCount = 0;
    static u8   RunFlag = 0;
    
    u8  Speed;

    Speed = VDRPulse_GetInstantSpeed();

    if(Speed > 0)
    {
        StopCount = 0;
        RunCount++;
        if(RunCount >= 10)
        {
            RunCount--;
            RunFlag = 1;
        }
    }
    else
    {
        RunCount = 0;
        StopCount++;
        if(StopCount >= 10)
        {
            StopCount--;
            RunFlag = 0;
        }
    }
    
    return RunFlag;
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
    VDROvertime_ReadAllDriverInfo();//��ȡ���м�ʻԱ��Ϣ���Զ�ǩ����һ����ʻԱ��
    VDROvertime_UpdateParameter();//���³�ʱ��ʻ����
}
/**************************************************************************
//��������VDROvertime_HandleNormalAlarm
//���ܣ����泬ʱ��ʻԤ������ʱ��ʻ����
//���룺��
//�������
//����ֵ����
//��ע����VDROvertime_TimeTask���ã�1�����1��
***************************************************************************/
static void VDROvertime_HandleNormalAlarm(void)
{
    static  u32 EarlyAlarmVoiceCount = 0;
    static  u32 EarlyAlarmVoiceNumCount = 0;
    static  u32 EarlyAlarmVoiceNumTimeCount = 0; 
    static  u32 AlarmVoiceCount = 0;
    static  u32 AlarmVoiceNumCount = 0;
    static  u32 AlarmVoiceNumTimeCount = 0; 

    u8  AlarmFlag;
    u8  Buffer[40];//���������������Ϊ40�ֽ�
    u8  BufferLen;

    AlarmFlag = VDROvertime_IsAlarm();
    if(0x02 == AlarmFlag)//�г�ʱ����
    {
        EarlyAlarmVoiceCount = 0;
        EarlyAlarmVoiceNumCount = 0;
        EarlyAlarmVoiceNumTimeCount = 0; 

        AlarmVoiceCount++;
        if((0 == (AlarmVoiceCount%(VdrOvertime.AlarmVoiceGroupTime*60)))&&(AlarmVoiceCount <= (VdrOvertime.AlarmVoiceKeeptime*60)))
        {
            //AlarmVoiceCount = 0;
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
                //Public_PlayTTSVoiceAlarmStr(Buffer);
            }
            AlarmVoiceNumTimeCount++;
        }
            
    }
    else//�޳�ʱ����������£��ٿ����޳�ʱԤ����
    {
        AlarmVoiceCount = 0;
        AlarmVoiceNumCount = 0;
        AlarmVoiceNumTimeCount = 0; 

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
                        strcpy((char *)Buffer,"����ʱ��ʻ");
                    }
                    else
                    {
                        Buffer[BufferLen] = 0;
                    }
                    //Public_PlayTTSVoiceAlarmStr(Buffer);
                }
                AlarmVoiceNumTimeCount++;
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
    static  u32 PlayVoiceTimeCount;
    u32 CurrentTimeCount;
    u8  RunStatus;

    CurrentTimeCount = RTC_GetCounter();

    if((0 == DriverNum)&&(CurrentTimeCount < (VdrOvertime.Driver[DriverNum].StartTimeCount+1800)))
    {
        RunStatus = VDROvertime_RunOrStop();
        if((1 == RunStatus)&&(1 == OvertimeUnloginVoiceEnableFlag))
        {
            if(CurrentTimeCount > PlayVoiceTimeCount)
            {
                //Public_PlayTTSVoiceAlarmStr("��ͣ������IC��");//������ʾ����
                PlayVoiceTimeCount = CurrentTimeCount+300;//5����һ��
            }
        }
        else
        {

        }
    }
    else
    {
        PlayVoiceTimeCount = 0;
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

    u32 CurrentTimeCount;
    u32 OffsetTime;
    u8  RunStatus;
    u8  AlarmFlag = 0;//bit0��λ��ʾ��ʱԤ����bit1��λ��ʾ��ʱ����

    RunStatus = VDROvertime_RunOrStop();
    RunStatus = 1;

    if(1 == RunStatus)//��ʻ��
    {
        StopCount = 0;
        CurrentTimeCount = RTC_GetCounter();
        if((CurrentTimeCount < VdrOvertime.Driver[DriverNum].StartTimeCount)||(0 == VdrOvertime.Driver[DriverNum].StartTimeCount))
        {
            AlarmFlag = 0;
            VdrOvertime.Driver[DriverNum].StartTimeCount = CurrentTimeCount;
            //Public_GetCurPositionInfoDataBlock(VdrOvertime.Driver[DriverNum].StartPosition);
            VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME|DRIVER_INFO_TYPE_START_POSITION);
        }
        else
        {
            OffsetTime = CurrentTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
            if(OffsetTime >= VdrOvertime.AlarmTime)
            {
                AlarmFlag = 0x02;
            }
            else if(OffsetTime >= VdrOvertime.EarlyAlarmTime)
            {
                AlarmFlag = 0x01;
            }
            else
            {
                AlarmFlag = 0;
            }
        }
        VdrOvertime.Driver[DriverNum].EndTimeCount = CurrentTimeCount;
        //Public_GetCurPositionInfoDataBlock(VdrOvertime.Driver[DriverNum].EndPosition);
        VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_END_TIME|DRIVER_INFO_TYPE_END_POSITION);
    }
    else//ͣʻ
    {
        StopCount++;

        if(StopCount < VdrOvertime.MinRestTime)
        {
            if(VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
            {
                AlarmFlag = 0;
                VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
                VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME);
            }
            else
            {
                OffsetTime = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
                if(OffsetTime >= VdrOvertime.AlarmTime)
                {
                    AlarmFlag = 0x02;
                }
                else if(OffsetTime >= VdrOvertime.EarlyAlarmTime)
                {
                    AlarmFlag = 0x01;
                }
                else
                {
                    AlarmFlag = 0;
                }
            }
        }
        else if(StopCount == VdrOvertime.MinRestTime)
        {
            AlarmFlag = 0;
            OffsetTime = VdrOvertime.Driver[DriverNum].EndTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount;
            if(OffsetTime >= VdrOvertime.AlarmTime)
            {
                VDROvertime_Write(DriverNum);
            }
            VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
            VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME);
        }
        else 
        {
            AlarmFlag = 0;
        }
    }

    return AlarmFlag;
    
}
/**************************************************************************
//��������VDROvertime_Write
//���ܣ�д��ʱ��ʻ
//���룺��ʻԱ��ţ�ȡֵ��Χ��0-5
//�������
//����ֵ����
//��ע�����浽flash VDR_DATA_TYPE_OVER_TIME�洢������
***************************************************************************/
static void VDROvertime_Write(u8 DriverNum)
{
    u8  Buffer[VDR_DATA_OVER_TIME_STEP_LEN];
    TIME_T  Time;

    if((0 == VdrOvertime.Driver[DriverNum].StartTimeCount)
           ||(VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
       ||(DriverNum >= DRIVER_NUM_MAX))
    {
        return ;
    }


    //��ʻ֤����
    memcpy(Buffer,VdrOvertime.Driver[DriverNum].License,18);

    //��ʼʱ��
    Gmtime(&Time, VdrOvertime.Driver[DriverNum].StartTimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+18,&Time);

    //����ʱ��
    Gmtime(&Time, VdrOvertime.Driver[DriverNum].EndTimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+24,&Time);

    //��ʼλ��
    memcpy(Buffer+30,VdrOvertime.Driver[DriverNum].StartPosition,10);

    //����λ��
    memcpy(Buffer+40,VdrOvertime.Driver[DriverNum].EndPosition,10);

    VDRData_Write(VDR_DATA_TYPE_OVER_TIME, Buffer, VDR_DATA_OVER_TIME_STEP_LEN-5, VdrOvertime.Driver[DriverNum].EndTimeCount);
    
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
//�������
//����ֵ����
//��ע������ǰRAM�е���Ϣ���浽FRAM�У����ݼ��ṹ��VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_SaveCurrentDriverInfo(u8 Type)
{
    u8  Buffer[5];

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
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].StartTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_START_TIME_ADDR, Buffer, 4);
    }

    //����ʱ��
    if(DRIVER_INFO_TYPE_END_TIME == (Type&DRIVER_INFO_TYPE_END_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].EndTimeCount,Buffer);
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
    u8  Buffer[20];

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
    Addr = FLASH_VDR_OVERTIME_START_ADDR;
    SPI_FLASH_SectorErase(Addr);
    
    //һ��һ��д��
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        //д���ʻ֤����
        memcpy(Buffer,VdrOvertime.Driver[i].License,18);
        sum = Public_GetSumVerify(Buffer,18);
        Buffer[18] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 19);
        Addr += 19;

        //д�뿪ʼʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].StartTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;

        //д�����ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].EndTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;

        //д�뿪ʼλ��
        memcpy(Buffer,VdrOvertime.Driver[i].StartPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 11);
        Addr += 11;

        //д�����λ��
        memcpy(Buffer,VdrOvertime.Driver[i].EndPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 11);
        Addr += 11;

        //д��ǩ��ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LoginTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;

        //д��ǩ��ʱ��
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LogoutTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
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

    Addr = FLASH_VDR_OVERTIME_START_ADDR;

    //һ��һ������
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        flag[i] = 0;

        //������ʻ֤����
        SPI_FLASH_BufferRead(Buffer,Addr,19);
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
        SPI_FLASH_BufferRead(Buffer,Addr,5);
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
        SPI_FLASH_BufferRead(Buffer,Addr,5);
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
        SPI_FLASH_BufferRead(Buffer,Addr,11);
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
        SPI_FLASH_BufferRead(Buffer,Addr,11);
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

        
        //����ǩ��ʱ��
        SPI_FLASH_BufferRead(Buffer,Addr,5);
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
        SPI_FLASH_BufferRead(Buffer,Addr,5);
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

        if(1 == flag[i])//���ݴ�����0
        {
            for(j=0;j<20;j++)
            {
                Buffer[j] = 0;
            }
            memcpy(VdrOvertime.Driver[i].License,Buffer,18);
            memcpy(VdrOvertime.Driver[i].StartPosition,Buffer,10);
            memcpy(VdrOvertime.Driver[i].EndPosition,Buffer,10);
            VdrOvertime.Driver[i].StartTimeCount = 0;
            VdrOvertime.Driver[i].EndTimeCount = 0;
            VdrOvertime.Driver[i].LoginTimeCount = 0;
            VdrOvertime.Driver[i].LogoutTimeCount = 0;
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
    if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
        ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount))//���¼���ʱ��
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }
    if((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime)
    {
        VDROvertime_Write(DriverNum);   
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
        VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_START_TIME);
    }
}






