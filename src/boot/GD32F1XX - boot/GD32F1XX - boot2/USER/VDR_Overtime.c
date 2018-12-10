/************************************************************************
//程序名称：VDR_Overtime.c
//功能：超时驾驶
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_rtc.h"
#include "Can.h"
/********************本地变量*************************/
//static    u8  OvertimeRunSpeed = 0;//行驶判定速度
//static    u32 OvertimeRunKeepTime = 0;//行驶判定持续时间
//static    u8  OvertimeLastRunStatus = 0;//上一秒行驶状态
static  u8  DriverNum = 0;//当前驾驶员序号，取值范围0-5
static  u8  DriverLoginFlag = 0;//驾驶员登录标志，1为签到（登录），0为签退（未登录）
static  u8  OvertimeUnloginVoiceEnableFlag = 0;//驾驶员未登录语音提醒开关，1使能，0禁止
static  u8  OvertimeInitFlag = 0;
static  VDR_OVERTIME_STRUCT VdrOvertime;
/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
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

/********************函数定义*************************/
/**************************************************************************
//函数名：VDROvertime_TimeTask
//功能：超时驾驶
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：超时驾驶定时任务，1秒钟进入1次，任务调度器需要调用此函数
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
        VDROvertime_HandleNormalAlarm();//常规超时预警、超时报警处理（针对当前驾驶员）

        //VDROvertime_HandleNightAlarm();//夜间超时预警、超时报警处理（针对当前驾驶员）

        VDROvertime_HandleOtherDrivers();//将已到最小休息时间的驾驶员开始时间清0，保存已产生的超时驾驶记录（针对其它驾驶员）

        VDROvertime_HandleRunUnlogin();//驾驶员未登录行驶提醒（报警）
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
//函数名：VDROvertime_UpdateParameter
//功能：更新超时驾驶相关参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDROvertime_UpdateParameter(void)
{
    u32 temp;
    u8  Buffer[5];
    u8  BufferLen;

    BufferLen = EepromPram_ReadPram(E2_MAX_DRVIE_TIME_ID, Buffer);//超时驾驶报警时间
  if(E2_MAX_DRVIE_TIME_LEN == BufferLen)
  {
    VdrOvertime.AlarmTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmTime = 14400;//单位秒，默认4小时
    }
    //VdrOvertime.AlarmTime = 300;//调试时使用该语句

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer);//报警语音持续时间
  if(E2_TIRED_DRIVE_ALARM_TIME_ID_LEN == BufferLen)
  {
    VdrOvertime.AlarmVoiceKeeptime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmVoiceKeeptime = 30;//单位分钟，默认为30分钟
    }
    //VdrOvertime.AlarmVoiceKeeptime = 5;//调试时使用该语句
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer);//报警语音每组提示间隔
  if(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN == BufferLen)
  {
    VdrOvertime.AlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmVoiceGroupTime = 5;//单位分钟，默认为5分钟
    }
    //VdrOvertime.AlarmVoiceGroupTime = 1;//调试时使用该语句

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_NUMBER_ID, Buffer);//报警语音每组提示间隔
  if(E2_TIRED_DRIVE_ALARM_NUMBER_ID_LEN == BufferLen)
  {
    VdrOvertime.AlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmVoiceNum = 3;
    }
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID, Buffer);//报警语音每组提示间隔
  if(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_LEN == BufferLen)
  {
     VdrOvertime.AlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.AlarmVoiceNumTime = 10;//单位秒，默认为10秒
    }
    
    BufferLen = EepromPram_ReadPram(E2_MIN_RELAX_TIME_ID, Buffer);//最小休息时间
  if(E2_MIN_RELAX_TIME_LEN == BufferLen)
  {
     VdrOvertime.MinRestTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.MinRestTime = 1200;//单位秒，默认为1200秒
    }
    //VdrOvertime.MinRestTime = 120;//调试时使用该语句

    BufferLen = EepromPram_ReadPram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer);//预警差值
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
        temp = 1800;//单位秒，默认为1800秒
    }
    //temp = 120;//调试时使用该语句
    VdrOvertime.EarlyAlarmTime = VdrOvertime.AlarmTime - temp;//预警时间

    
    VdrOvertime.EarlyAlarmKeeptime = VdrOvertime.AlarmVoiceKeeptime;//预警持续时间，默认等于报警语音持续时间
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);//预警语音每组提示间隔
  if(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN == BufferLen)
  {
     VdrOvertime.EarlyAlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.EarlyAlarmVoiceGroupTime = 5;//单位分钟，默认为5分钟
    }
    //VdrOvertime.EarlyAlarmVoiceGroupTime = 1;//调试时使用该语句
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_NUMBER_ID, Buffer);//预警语音每组提示次数
  if(E2_TIRED_DRIVE_PREALARM_NUMBER_ID_LEN == BufferLen)
  {
    VdrOvertime.EarlyAlarmVoiceNum = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.EarlyAlarmVoiceNum = 3;
    }

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID, Buffer);//预警语音每次提示间隔
  if(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_LEN == BufferLen)
  {
    VdrOvertime.EarlyAlarmVoiceNumTime = Public_ConvertBufferToLong(Buffer);
  }
    else
    {
        VdrOvertime.EarlyAlarmVoiceNumTime = 3;
    }

    BufferLen = EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, Buffer);//驾驶员未登录提示语音使能开关
  if(E2_UNLOGIN_TTS_PLAY_ID_LEN == BufferLen)
  {
    OvertimeUnloginVoiceEnableFlag = Buffer[0];
  }
    else
    {
        OvertimeUnloginVoiceEnableFlag = 0;
    }
    //OvertimeUnloginVoiceEnableFlag = 1;//调试时使用该语句
    
}
/**************************************************************************
//函数名：VDROvertime_DriverLogin
//功能：驾驶员签到（登录）
//输入：pBuffer:指向驾驶证号码的指针;BufferLen:数据长度
//输出：无
//返回值：0:登录成功；非0:登录失败，为1表示长度错误，为2表示驾驶证号码格式错误
//为3表示驾驶证号全为ASCII 0（即禁止系统默认驾驶员签到），为4表示该驾驶员已签到
//备注：驾驶证号码长度BufferLen必须为18字节，否则登录失败
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
        if(((*(pBuffer+i) >= '0')&&((*(pBuffer+i) <= '9')))//驾驶证号码即为身份证号码，全为数字组成，校验码有可能是10（X）
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
    if(DRIVER_NUM_MAX == Num)//在已记录的驾驶证里未找到，停车时间最老的那个驾驶员让给当前驾驶员
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
    else//在已记录的驾驶证里找到了
    {
        if(Num == DriverNum)
        {
            return 0x04;
        }
        else//更换了驾驶员
        {
            
        }
    }

    //先签退上一个驾驶员
    VDROvertime_DriverLogout();

    //再签到当前驾驶员
    DriverNum = Num;
    memcpy(VdrOvertime.Driver[DriverNum].License,pBuffer,18);
    
    if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
        ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount)
            ||((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime))//重新计算时间
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }

    
    if(0 != Num)
    {
        TimeCount = RTC_GetCounter();
        Gmtime(&Time, TimeCount);
        
        p = Buffer;
        VDRPub_ConvertTimeToBCD(p,&Time);//签到记录 
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
    
    VDROvertime_SaveAllDriverInfo();//将所有驾驶员信息保存到flash
    VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ALL);//将当前驾驶员信息保存到fram


    return 0;
}
/**************************************************************************
//函数名：VDROvertime_DriverLogout
//功能：驾驶员签退（未登录）
//输入：pBuffer:指向驾驶证号码的指针;BufferLen:数据长度
//输出：无
//返回值：0:登录成功；非0:登录失败，为1表示长度错误，为2表示驾驶证号码格式错误
//为3表示驾驶证号全为ASCII 0（即禁止系统默认驾驶员签到），为4表示该驾驶员已签到
//备注：驾驶证号码长度BufferLen必须为18字节，否则登录失败
***************************************************************************/
void VDROvertime_DriverLogout(void)
{
    u8  Buffer[VDR_DATA_DRIVER_STEP_LEN+1];
    u8  *p = NULL;
    u32 TimeCount;
    TIME_T  Time;

    if(0 != DriverNum)//自动切换为默认驾驶员
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
        
        VDRData_Write(VDR_DATA_TYPE_DRIVER,Buffer,VDR_DATA_DRIVER_STEP_LEN-5,TimeCount);//保存签退记录

        //发送签退命令到监控中心
        
        DriverNum = 0;
        memset(VdrOvertime.Driver[DriverNum].License,'0',18);
        if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
            ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount)
                ||((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime))//重新计算时间
        {
            VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
            VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
        }
        
    }

    DriverLoginFlag = 0;

    VDROvertime_SaveAllDriverInfo();//将所有驾驶员信息保存到flash

    VDROvertime_SaveCurrentDriverInfo(DRIVER_INFO_TYPE_ALL);//将当前驾驶员信息保存到fram
}
/**************************************************************************
//函数名：VDROvertime_GetInitFlag
//功能：获得模块初始化标志
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
u8 VDROvertime_GetInitFlag(void)
{
    return OvertimeInitFlag;
}
/**************************************************************************
//函数名：VDROvertime_GetLoginFlag
//功能：获取登录标志
//输入：无
//输出：登录标志
//返回值：1:登录；0:未登录
//备注：无
***************************************************************************/
u8 VDROvertime_GetLoginFlag(void)
{
    return DriverLoginFlag;
}
/**************************************************************************
//函数名：VDROvertime_GetLog
//功能：从临时存储区（RAM或铁电）获取指定时间之前的超时驾驶记录
//输入：指定时间
//输出：
//返回值：数据长度
//备注：从RAM或铁电中读取还未写入到FLASH中的超时驾驶记录
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
            //驾驶证号码
            memcpy(p,VdrOvertime.Driver[i].License,18);
            p += 18;
    
            //开始时间
            Gmtime(&Time, VdrOvertime.Driver[i].StartTimeCount);
            if(0 == Public_CheckTimeStruct(&Time))
            {
                p -= 18;
                break;
            }
            VDRPub_ConvertTimeToBCD(p,&Time);
            p += 6;

            //结束时间
            Gmtime(&Time, VdrOvertime.Driver[i].EndTimeCount);
            if(0 == Public_CheckTimeStruct(&Time))
            {
                p -= 24;
                break;
            }
            VDRPub_ConvertTimeToBCD(p,&Time);
            p += 6;

            //开始位置
            memcpy(p,VdrOvertime.Driver[i].StartPosition,10);
            p += 10;

            //结束位置
            memcpy(p,VdrOvertime.Driver[i].EndPosition,10);
            p += 10;
        }
    }

    return length;  
}
/**************************************************************************
//函数名：VDROvertime_GetLicense
//功能：获取当前驾驶证号码
//输入：指向目的缓冲指针
//输出：
//返回值：数据长度
//备注：
***************************************************************************/
u8 VDROvertime_GetLicense(u8 *pBuffer)
{
    memcpy(pBuffer,VdrOvertime.Driver[DriverNum].License,18);
    return 18;
}

//////////////////////////////////以下为内部函数///////////////////////////////////////
/**************************************************************************
//函数名：VDROvertime_RunOrStop
//功能：行驶与停驶的判定
//输入：无
//输出：无
//返回值：为1为行驶，为0为停驶
//备注:
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
//函数名：VDROvertime_Init
//功能：超时驾驶模块初始化
//输入：无
//输出：无
//返回值：无
//备注：只有完成超时驾驶模块初始化后才能开启IC卡定时任务
***************************************************************************/
static void VDROvertime_Init(void)
{
    VDROvertime_ReadAllDriverInfo();//读取所有驾驶员信息，自动签到上一个驾驶员。
    VDROvertime_UpdateParameter();//更新超时驾驶参数
}
/**************************************************************************
//函数名：VDROvertime_HandleNormalAlarm
//功能：常规超时驾驶预警、超时驾驶报警
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
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
    u8  Buffer[40];//播报语音最长不超过为40字节
    u8  BufferLen;

    AlarmFlag = VDROvertime_IsAlarm();
    if(0x02 == AlarmFlag)//有超时报警
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
                    strcpy((char *)Buffer,"请勿超时驾驶");
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
    else//无超时报警的情况下，再看有无超时预警。
    {
        AlarmVoiceCount = 0;
        AlarmVoiceNumCount = 0;
        AlarmVoiceNumTimeCount = 0; 

        if(0x01 == AlarmFlag)//有超时预警
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
                        strcpy((char *)Buffer,"请勿超时驾驶");
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
        else//无超时预警
        {

            EarlyAlarmVoiceCount = 0;
            EarlyAlarmVoiceNumCount = 0;
            EarlyAlarmVoiceNumTimeCount = 0; 
        }
    }
    
}
/**************************************************************************
//函数名：VDROvertime_HandleOtherDrivers
//功能：处理其它驾驶员的超时驾驶
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
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
//函数名：VDROvertime_HandleRunUnlogin
//功能：驾驶员未签到行驶提醒
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
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
                //Public_PlayTTSVoiceAlarmStr("请停车插入IC卡");//播报提示语音
                PlayVoiceTimeCount = CurrentTimeCount+300;//5分钟一次
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
//函数名：VDROvertime_IsAlarm
//功能：超时驾驶判断
//输入：无
//输出：无
//返回值：bit0置位表示超时预警；bit1置位表示超时报警
//备注：bit0和bit1不会同时为1
***************************************************************************/
static u8 VDROvertime_IsAlarm(void)
{
    static  u32 StopCount = 0;

    u32 CurrentTimeCount;
    u32 OffsetTime;
    u8  RunStatus;
    u8  AlarmFlag = 0;//bit0置位表示超时预警；bit1置位表示超时报警

    RunStatus = VDROvertime_RunOrStop();
    RunStatus = 1;

    if(1 == RunStatus)//行驶中
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
    else//停驶
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
//函数名：VDROvertime_Write
//功能：写超时驾驶
//输入：驾驶员序号，取值范围是0-5
//输出：无
//返回值：无
//备注：保存到flash VDR_DATA_TYPE_OVER_TIME存储区域中
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


    //驾驶证号码
    memcpy(Buffer,VdrOvertime.Driver[DriverNum].License,18);

    //开始时间
    Gmtime(&Time, VdrOvertime.Driver[DriverNum].StartTimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+18,&Time);

    //结束时间
    Gmtime(&Time, VdrOvertime.Driver[DriverNum].EndTimeCount);
    if(0 == Public_CheckTimeStruct(&Time))
    {
        return ;
    }
    VDRPub_ConvertTimeToBCD(Buffer+24,&Time);

    //开始位置
    memcpy(Buffer+30,VdrOvertime.Driver[DriverNum].StartPosition,10);

    //结束位置
    memcpy(Buffer+40,VdrOvertime.Driver[DriverNum].EndPosition,10);

    VDRData_Write(VDR_DATA_TYPE_OVER_TIME, Buffer, VDR_DATA_OVER_TIME_STEP_LEN-5, VdrOvertime.Driver[DriverNum].EndTimeCount);
    
}

/**************************************************************************
//函数名：VDROvertime_SaveCurrentDriverInfo
//功能：保存当前驾驶员信息
//输入：Type:可以是如下类型的组合
//DRIVER_INFO_TYPE_DRIVER_NUM
//DRIVER_INFO_TYPE_DRIVER_LICENSE
//DRIVER_INFO_TYPE_START_TIME
//DRIVER_INFO_TYPE_END_TIME
//DRIVER_INFO_TYPE_START_POSITION
//DRIVER_INFO_TYPE_END_POSITION
//DRIVER_INFO_TYPE_LOGIN_TIME
//DRIVER_INFO_TYPE_LOGOUT_TIME
//输出：无
//返回值：无
//备注：将当前RAM中的信息保存到FRAM中，内容见结构体VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_SaveCurrentDriverInfo(u8 Type)
{
    u8  Buffer[5];

    //驾驶员序号
    if(DRIVER_INFO_TYPE_DRIVER_NUM == (Type&DRIVER_INFO_TYPE_DRIVER_NUM))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR, &DriverNum, 1);
    }

    //驾驶证号码
    if(DRIVER_INFO_TYPE_DRIVER_LICENSE == (Type&DRIVER_INFO_TYPE_DRIVER_LICENSE))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_LICENSE_ADDR, VdrOvertime.Driver[DriverNum].License, 18);
    }

    //开始时间
    if(DRIVER_INFO_TYPE_START_TIME == (Type&DRIVER_INFO_TYPE_START_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].StartTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_START_TIME_ADDR, Buffer, 4);
    }

    //结束时间
    if(DRIVER_INFO_TYPE_END_TIME == (Type&DRIVER_INFO_TYPE_END_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].EndTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_END_TIME_ADDR, Buffer, 4);
    }

    //开始位置
    if(DRIVER_INFO_TYPE_START_POSITION == (Type&DRIVER_INFO_TYPE_START_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_START_POSITION_ADDR, VdrOvertime.Driver[DriverNum].StartPosition, 10);
    }

    //结束位置
    if(DRIVER_INFO_TYPE_END_POSITION == (Type&DRIVER_INFO_TYPE_END_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_END_POSITION_ADDR, VdrOvertime.Driver[DriverNum].EndPosition, 10);
    }

    //签到时间
    if(DRIVER_INFO_TYPE_LOGIN_TIME == (Type&DRIVER_INFO_TYPE_LOGIN_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].LoginTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR, Buffer, 4);
    }

    //签退时间
    if(DRIVER_INFO_TYPE_LOGOUT_TIME == (Type&DRIVER_INFO_TYPE_LOGOUT_TIME))
    {
        Public_ConvertLongToBuffer(VdrOvertime.Driver[DriverNum].LogoutTimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR, Buffer, 4);
    }
    
}
/**************************************************************************
//函数名：VDROvertime_ReadCurrentDriverInfo
//功能：读当前驾驶员信息
//输入：无
//输出：无
//返回值：无
//备注：从保存到FRAM中的信息读取到RAM中，内容见结构体VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_ReadCurrentDriverInfo(void)
{
    u8  Buffer[20];

    //驾驶员序号
    if(1 == FRAM_BufferRead(Buffer, 1,FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR))
    {
        DriverNum = Buffer[0];
    }

    //驾驶证号码
    if(18 == FRAM_BufferRead(Buffer, 18,FRAM_VDR_OVERTIME_LICENSE_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].License,Buffer,18);
    }

    //开始时间
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_START_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //结束时间
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_END_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].EndTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //开始位置
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_START_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].StartPosition,Buffer,10);
    }

    //结束位置
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_END_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].EndPosition,Buffer,10);
    }

    //签到时间
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].LoginTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //签退时间
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].LogoutTimeCount = Public_ConvertBufferToLong(Buffer);
    }
    
}
/**************************************************************************
//函数名：VDROvertime_SaveAllDriverInfo
//功能：保存所有驾驶员信息
//输入：无
//输出：无
//返回值：无
//备注：将RAM中的信息保存到FLASH中，内容见结构体VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_SaveAllDriverInfo(void)
{
    u8  i;
    u8  Buffer[20];
    u8  sum;
    u32 Addr;
    

    //先擦除
    Addr = FLASH_VDR_OVERTIME_START_ADDR;
    SPI_FLASH_SectorErase(Addr);
    
    //一个一个写入
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        //写入驾驶证号码
        memcpy(Buffer,VdrOvertime.Driver[i].License,18);
        sum = Public_GetSumVerify(Buffer,18);
        Buffer[18] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 19);
        Addr += 19;

        //写入开始时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].StartTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;

        //写入结束时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].EndTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;

        //写入开始位置
        memcpy(Buffer,VdrOvertime.Driver[i].StartPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 11);
        Addr += 11;

        //写入结束位置
        memcpy(Buffer,VdrOvertime.Driver[i].EndPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 11);
        Addr += 11;

        //写入签到时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LoginTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;

        //写入签退时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LogoutTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        SPI_FLASH_BufferWrite(Buffer, Addr, 5);
        Addr += 5;
            
    }

    //写入当前驾驶员序号
    FRAM_BufferWrite(FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR, &DriverNum, 1);
}
/**************************************************************************
//函数名：VDROvertime_ReadAllDriverInfo
//功能：读所有驾驶员信息
//输入：无
//输出：无
//返回值：无
//备注：从保存到FLASH中的信息读取到RAM中，内容见结构体VDR_OVERTIME_DRIVER_STRUCT
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

    //一个一个读出
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        flag[i] = 0;

        //读出驾驶证号码
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

        //读出开始时间
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
        
        

        //读出结束时间
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

        //读出开始位置
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

        //读出结束位置
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

        
        //读出签到时间
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
        
        

        //读出签退时间
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

        if(1 == flag[i])//数据错误，清0
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

    //读出当前驾驶员序号
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
        ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount))//重新计算时间
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






