/************************************************************************
//程序名称：VDR_Overtime.c
//功能：超时驾驶
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
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
/********************本地变量*************************/
//static    u8  OvertimeLastRunStatus = 0;//上一秒行驶状态
static  u8  DriverNum = 0;//当前驾驶员序号，取值范围0-5
static  u8  DriverLoginFlag = 0;//驾驶员登录标志，1为签到（登录），0为签退（未登录）
static  u8  OvertimeUnloginVoiceEnableFlag = 0;//驾驶员未登录语音提醒开关，1使能，0禁止
static  u8  OvertimeInitFlag = 0;
static  u8  OvertimeAlarmFlag = 0;//超时驾驶标志，1为已超时，0为未超时。该标志不包含夜间超时驾驶的情况，只用于非夜间超时
static  VDR_OVERTIME_STRUCT VdrOvertime;
static  u8  VdrOvertimeRunFlag = 0;
static  u32 MaxStopTime = 0xffffffff;   
static u32 StopAlarmCount = 0;

/********************全局变量*************************/
u32 TestTimeCount3 = 0;

/********************外部变量*************************/
extern u8   GBTestFlag;//0:正常出货运行模式;1:国标检测模式
extern u8  BBXYTestFlag;//0为正常模式，1为部标检测模式,
/********************本地函数声明*********************/
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
			  VDROvertime_JudgeRunOrStop();//行驶、停驶判定
			
			  VDROvertime_OnceRunReport();//部标协议测试超时驾驶时调用此函数，重点测试超时驾驶开始时刻和报警时刻
			
			  VDROvertime_StopAlarm();//超时停车
			
        VDROvertime_HandleAlarm();//超时预警、超时报警处理（针对当前驾驶员）
			
			  VDROvertime_HandleOneDayAlarm();//当天累计超时驾驶

        VDROvertime_HandleOtherDrivers();//将已到最小休息时间的驾驶员开始时间清0，保存已产生的超时驾驶记录（针对其它驾驶员）

        VDROvertime_HandleRunUnlogin();//驾驶员未登录行驶提醒（报警）
			
			  VDROvertime_HandleNightRunVoice();//夜间行驶提醒（语音报警）
    }
    
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
    u8  Buffer[35];
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

    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer);//报警语音持续时间
    if(E2_TIRED_DRIVE_ALARM_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.AlarmVoiceKeeptime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmVoiceKeeptime = 30;//单位分钟，默认为30分钟
    }
		
		BufferLen = EepromPram_ReadPram(E2_MAX_DRVIE_TIME_ONEDAY_ID, Buffer);//当天累计超时驾驶时间门限
    if(E2_MAX_DRVIE_TIME_ONEDAY_LEN == BufferLen)
    {
        VdrOvertime.OneDayAlarmTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.OneDayAlarmTime = 0xFFFFFFFF;//单位秒，默认为0xFFFFFFFF
    }
		
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer);//报警语音每组提示间隔
    if(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.AlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.AlarmVoiceGroupTime = 5;//单位分钟，默认为5分钟
    }

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

    BufferLen = EepromPram_ReadPram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer);//预警差值
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
        temp = 1800;//单位秒，默认为1800秒
    }
    
    if(VdrOvertime.AlarmTime > temp)
    {
      VdrOvertime.EarlyAlarmTime = VdrOvertime.AlarmTime - temp;//预警时间
    }
    else
    {
      VdrOvertime.EarlyAlarmTime = 0;
    }

    
    VdrOvertime.EarlyAlarmKeeptime = temp;//预警持续时间
    
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);//预警语音每组提示间隔
    if(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.EarlyAlarmVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.EarlyAlarmVoiceGroupTime = 5;//单位分钟，默认为5分钟
    }
    
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
		
		//BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_NIGHT_TIME_ID, Buffer);//夜间时间，夜间超时使用
		BufferLen = EepromPram_ReadPram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer);//夜间时间，开会讨论使用夜间超速时间
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
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_MAX_DRVIE_TIME_ID, Buffer);//夜间连续驾驶时间门限
    if(E2_NIGHT_MAX_DRVIE_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightAlarmTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightAlarmTime = 7200;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_MIN_RELAX_TIME_ID, Buffer);//夜间最小休息时间
    if(E2_NIGHT_MIN_RELAX_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightMinRestTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightMinRestTime = 1200;
    }
        
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer);//超时驾驶行驶判定速度
    if(E2_TIRED_DRIVE_RUN_SPEED_ID_LEN == BufferLen)
    {
        VdrOvertime.RunSpeed = Buffer[3];//前面3字节一般为0，取第4字节即可
    }
    else
    {
        VdrOvertime.RunSpeed = 0;
    }
        
    BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer);//超时驾驶行驶判定持续时间
    if(E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN == BufferLen)
    {
        VdrOvertime.RunKeeptime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.RunKeeptime = 10;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_TIME_ID, Buffer);//夜间行驶预警时间
    if(E2_NIGHT_DRIVE_PREALARM_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightVoiceKeeptime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightVoiceKeeptime = 10;
    }
		
		BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);//夜间行驶预警提示时间间隔
    if(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_LEN == BufferLen)
    {
        VdrOvertime.NightVoiceGroupTime = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        VdrOvertime.NightVoiceGroupTime = 5;
    }
		

    BufferLen = EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, Buffer);//驾驶员未登录提示语音使能开关,bit7:1国标检测模式;bit6:1部标检测模式
    if(E2_UNLOGIN_TTS_PLAY_ID_LEN == BufferLen)
    {
        OvertimeUnloginVoiceEnableFlag = Buffer[0]&0x01;
    }
    else
    {
        OvertimeUnloginVoiceEnableFlag = 0;
    }
		
		BufferLen = EepromPram_ReadPram(E2_MAX_STOP_CAR_TIME_ID, Buffer);//最长停车时间
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
    TimeCount = RTC_GetCounter();
    if((VdrOvertime.Driver[DriverNum].StartTimeCount > VdrOvertime.Driver[DriverNum].EndTimeCount)
        ||(VdrOvertime.Driver[DriverNum].EndTimeCount > TimeCount)
            ||((TimeCount-VdrOvertime.Driver[DriverNum].EndTimeCount) > VdrOvertime.MinRestTime))//重新计算时间
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }

    
    if(0 != DriverNum)
    {
        TimeCount++;//dxl,2016.7.20加1是为了避免当同一秒钟有签退、签到事件时，把签到漏记了
			
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
                
        Public_PlayTTSVoiceStr("驾驶员签到！");
                
        strcpy((char *)LCDDisplayBuf,"驾驶员签到 证号:");
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
        
    //写入当前驾驶证号码
    EepromPram_WritePram(E2_CURRENT_LICENSE_ID, VdrOvertime.Driver[DriverNum].License, 18);  
    
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
        u8      LCDDisplayBuf[50];
        u8      length;
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
                
        //写入当前驾驶证号码
        EepromPram_WritePram(E2_CURRENT_LICENSE_ID, VdrOvertime.Driver[DriverNum].License, 18);   
                
        Public_PlayTTSVoiceStr("驾驶员签退！");
                
        strcpy((char *)LCDDisplayBuf,"驾驶员签退");
        length = strlen((char *)LCDDisplayBuf);
        LCDDisplayBuf[length] = 0;
        Public_ShowTextInfo((char *)LCDDisplayBuf,PUBLICSECS(6));
				
				Io_WriteSelfDefine2Bit(DEFINE_BIT_5,RESET);
				
				Buffer[0] = 0x02;//下班
	      VDRPub_ConvertNowTimeToBCD(Buffer+1);//时间，6字节BCD
				RadioProtocol_DriverInformationReport(CHANNEL_DATA_1|CHANNEL_DATA_2,Buffer,7);
        
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
//函数名：VDROvertime_Get2daysLog
//功能：获取2个日历天的超时驾驶记录
//输入：无
//输出：
//返回值：数据长度
//备注：
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
                
        for(i=0; i<12; i++)//最多取12条记录
        {
                Len = VDRData_Read(Buffer,pSector,pStep,VDR_DATA_TYPE_OVERTIME,StartTime,EndTime,1);//1次读1条 
                if((length >= 600)||(EndTimeCount < EndTimeCountMin)||(Len != VDR_DATA_OVER_TIME_COLLECT_LEN))
                {
                        break;
                }
                q = Buffer+24;//指向结束时间首地址
                VDRPub_ConvertBCDToTime(&EndTime,q);
                EndTimeCount = ConverseGmtime(&EndTime);
                memcpy(p,Buffer,Len);
                p += Len;
                length += Len;    
        }
        
        return length;
}
/**************************************************************************
//函数名：VDROvertime_GetUnsaveLog
//功能：获取还未保存到flash的超时驾驶记录
//输入：Time:指定时间;DriverNum:驾驶员序号,DriverNum==DRIVER_NUM_MAX表示要读取RAM中所有的超时驾驶记录
//输出：
//返回值：数据长度
//备注：从RAM或铁电中读取还未写入到FLASH中的超时驾驶记录
***************************************************************************/
u16 VDROvertime_GetUnsaveLog(u8 *pBuffer, TIME_T Time, u8 DriverNum)
{
    u8  *p = NULL;
    u8  i,j,k;
        u8      flag;
    u16 length;
    u32 TimeCount;
    u32 temp;
        
        if(DriverNum > DRIVER_NUM_MAX)//参数错误
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
                         
                      //驾驶证号码
                      memcpy(p,VdrOvertime.Driver[i].License,18);
                      p += 18;
    
              //开始时间
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

                      //结束时间
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

                      //开始位置
                      memcpy(p,VdrOvertime.Driver[i].StartPosition,10);
                      p += 10;

                      //结束位置
                      memcpy(p,VdrOvertime.Driver[i].EndPosition,10);
                      p += 10;
                        
                      if(1 == flag)
                      {
                              p -= 50;//时间错误，抛弃这条数据
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
/**************************************************************************
//函数名：VDROvertime_GetDriveTime
//功能：获取当前驾驶员连续驾驶时间
//输入：无
//输出：
//返回值：连续驾驶时间，单位分钟
//备注：
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
	else//夜间时段
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
 
//////////////////////////////////以下为内部函数///////////////////////////////////////
/**************************************************************************
//函数名：VDROvertime_JudgeRunOrStop
//功能：行驶与停驶的判定
//输入：无
//输出：无
//返回值：无
//备注:
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
//函数名：VDROvertime_GetRunFlag
//功能：获取行驶状态标志
//输入：无
//输出：无
//返回值：为1为行驶，为0为停驶
//备注:
***************************************************************************/
static u8 VDROvertime_GetRunFlag(void)
{
    return VdrOvertimeRunFlag;
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
    VDROvertime_UpdateParameter();//更新超时驾驶参数
    VDROvertime_ReadAllDriverInfo();//读取所有驾驶员信息，自动签到上一个驾驶员。
    
}
/**************************************************************************
//函数名：VDROvertime_HandleAlarm
//功能：超时驾驶预警、超时驾驶报警
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
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
    u8  Buffer[40];//播报语音最长不超过为40字节
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

    if(0x02 == AlarmFlag)//有超时报警
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
                    strcpy((char *)Buffer,"请勿超时驾驶");
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
    else//无超时报警的情况下，再看有无超时预警。
    {
        AlarmVoiceCount = 0;
        AlarmVoiceNumCount = 0;
        AlarmVoiceNumTimeCount = 0; 
        
        CurrentTimeCount = RTC_GetCounter();
        if((CurrentTimeCount - VdrOvertime.Driver[DriverNum].StartTimeCount) <= (VdrOvertime.AlarmTime-10))
        {

					
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
                          strcpy((char *)Buffer,"即将超时驾驶");
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
        else//无超时预警
        {

            EarlyAlarmVoiceCount = 0;
            EarlyAlarmVoiceNumCount = 0;
            EarlyAlarmVoiceNumTimeCount = 0; 
        }
    }
    
}
/**************************************************************************
//函数名：VDROvertime_OneDayAlarm
//功能：当天累计超时驾驶报警
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
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
    if(((CurrentTimeCount > (LastTimeCount+2))&&(0 != LastTimeCount))//时间跳变
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
		
	  if((0 == CurTime.hour)&&(0 == CurTime.min)&&(0 == CurTime.sec))//0时0分0秒清0累计超时驾驶时间
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
			  if(1 == RunStatus)//行驶
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
                        strcpy((char *)Buffer,"累计超时驾驶");
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
//函数名：VDROvertime_HandleRunUnlogin
//功能：驾驶员未签到行驶提醒
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
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
                Public_PlayTTSVoiceStr("请停车插入IC卡");//播报提示语音
                Public_ShowTextInfo("请停车插入IC卡",100);
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
//函数名：VDROvertime_HandleNightRunVoice
//功能：夜间行驶语音提醒
//输入：无
//输出：无
//返回值：无
//备注：被VDROvertime_TimeTask调用，1秒调用1次
***************************************************************************/
static void VDROvertime_HandleNightRunVoice(void)
{
	
	  TIME_T CurTime;
    u8  Buffer[40];//播报语音最长不超过为40字节
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
                    strcpy((char *)Buffer,"即将进入夜间时段");
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
		    if(7 == count)//延时7秒播报，因为前面需要播报限速XX公里
				{
				    if(1 == VDROvertime_GetRunFlag())
						{
						    strcpy((char *)Buffer,"已进入夜间时段");
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
    static  u32 LastTimeCount = 0;

    u32 CurrentTimeCount;
    u32 OffsetTime;
    u8  RunStatus;
    u8  AlarmFlag = 0;//bit0置位表示超时预警；bit1置位表示超时报警

    RunStatus = VDROvertime_GetRunFlag();
    
    CurrentTimeCount = RTC_GetCounter();
    if((CurrentTimeCount > (LastTimeCount+2))&&(0 != LastTimeCount))//时间向前跳变
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
		else if((CurrentTimeCount < (LastTimeCount-2))&&(0 != LastTimeCount))//时间向后跳变
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
            
    if(OffsetTime >= 3*24*3600)//连续驾驶超过3天，认为是一种错误，重新计算
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }

    if(1 == RunStatus)//行驶中
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
    else//停驶
    {
        StopCount++;

        if(StopCount < (VdrOvertime.MinRestTime+2))//加2秒是为了配合部标协议检测，不影响其它功能
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
//函数名：VDROvertime_GetAlarmFlag
//功能：获取超时驾驶标志（不包含夜间超时）
//输入：无
//输出：无
//返回值：1：超时；0：未超时
//备注：
***************************************************************************/
static u8 VDROvertime_GetAlarmFlag(void)
{
    return OvertimeAlarmFlag;
}
/**************************************************************************
//函数名：VDROvertime_NightIsAlarm
//功能：超时驾驶判断
//输入：无
//输出：无
//返回值：bit0置位表示超时预警；bit1置位表示超时报警
//备注：bit0和bit1不会同时为1
***************************************************************************/
static u8 VDROvertime_NightIsAlarm(void)
{
    static  u32 NightStopCount = 0;
    static  u32 NightLastTimeCount = 0;
	  static  u8  NightFlag = 0;

    u32 CurrentTimeCount;
    u32 OffsetTime;
    u8  RunStatus;
    u8  AlarmFlag = 0;//bit0置位表示超时预警；bit1置位表示超时报警
	
	  if(0 == VDROvertime_NightIsArrived())
		{
			  if(1 == NightFlag)//进入过夜间时段
				{
				    Io_WriteOvertimeConrtolBit(OVER_TIME_BIT_NIGHT, RESET);//清除超时驾驶标志
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
    if((CurrentTimeCount > (NightLastTimeCount+2))&&(0 != NightLastTimeCount))//时间向前跳变
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
		else if((CurrentTimeCount < (NightLastTimeCount-2))&&(0 != NightLastTimeCount))//时间向后跳变
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
            
    if(OffsetTime >= 3*24*3600)//连续驾驶超过3天，认为是一种错误，重新计算
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }

    if(1 == RunStatus)//行驶中
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
    else//停驶
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
//函数名：VDROvertime_NightIsArrived
//功能：判断当前是否为夜间时间
//输入：无
//输出：无
//返回值：1：当前是夜间时间；0：当前不是夜间时间
//备注：
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
//函数名：VDROvertime_NightTimeIsOk
//功能：检查夜间时间是否正确
//输入：无
//输出：无
//返回值：1：正确；0：不正确
//备注：
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
//函数名：VDROvertime_Write
//功能：写超时驾驶
//输入：驾驶员序号，取值范围是0-5
//输出：无
//返回值：无
//备注：保存到flash VDR_DATA_TYPE_OVERTIME存储区域中
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

		
    //驾驶证号码
    memcpy(Buffer,VdrOvertime.Driver[DriverNum].License,18);

    //开始时间
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

    //结束时间
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

    //开始位置
    memcpy(Buffer+30,VdrOvertime.Driver[DriverNum].StartPosition,10);

    //结束位置
    memcpy(Buffer+40,VdrOvertime.Driver[DriverNum].EndPosition,10);

    VDRData_Write(VDR_DATA_TYPE_OVERTIME, Buffer, VDR_DATA_OVER_TIME_STEP_LEN-5, VdrOvertime.Driver[DriverNum].EndTimeCount);
    
}
/**************************************************************************
//函数名：VDROvertime_NightWrite
//功能：写超时驾驶
//输入：驾驶员序号，取值范围是0-5
//输出：无
//返回值：无
//备注：保存到flash VDR_DATA_TYPE_OVERTIME存储区域中
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


    //驾驶证号码
    memcpy(Buffer,VdrOvertime.Driver[DriverNum].License,18);

    //开始时间
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

    //结束时间
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

    //开始位置
    memcpy(Buffer+30,VdrOvertime.Driver[DriverNum].NightStartPosition,10);

    //结束位置
    memcpy(Buffer+40,VdrOvertime.Driver[DriverNum].NightEndPosition,10);

    VDRData_Write(VDR_DATA_TYPE_OVERTIME, Buffer, VDR_DATA_OVER_TIME_STEP_LEN-5, VdrOvertime.Driver[DriverNum].NightEndTimeCount);
    
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
//DRIVER_INFO_TYPE_NIGHT_START_TIME					
//DRIVER_INFO_TYPE_NIGHT_END_TIME						
//DRIVER_INFO_TYPE_NIGHT_START_POSITION			
//DRIVER_INFO_TYPE_NIGHT_END_POSITION				
//输出：无
//返回值：无
//备注：将当前RAM中的信息保存到FRAM中，内容见结构体VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_SaveCurrentDriverInfo(u16 Type)
{
    u8  Buffer[5];
    u32 TimeCount = 0;
	  TIME_T Time;
	  TIME_T CurTime;
	  
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
        TimeCount = VdrOvertime.Driver[DriverNum].StartTimeCount;
        if(TimeCount > 10)
        {
          TimeCount -= 10;
        }
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_START_TIME_ADDR, Buffer, 4);
    }

    //结束时间
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
		
		//夜间开始时间
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

    //夜间结束时间
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

    //夜间开始位置
    if(DRIVER_INFO_TYPE_NIGHT_START_POSITION == (Type&DRIVER_INFO_TYPE_NIGHT_START_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR, VdrOvertime.Driver[DriverNum].NightStartPosition, 10);
    }

    //夜间结束位置
    if(DRIVER_INFO_TYPE_NIGHT_END_POSITION == (Type&DRIVER_INFO_TYPE_NIGHT_END_POSITION))
    {
        FRAM_BufferWrite(FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR, VdrOvertime.Driver[DriverNum].NightEndPosition, 10);
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
		
		//每日累计驾驶时间
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
//函数名：VDROvertime_ReadCurrentDriverInfo
//功能：读当前驾驶员信息
//输入：无
//输出：无
//返回值：无
//备注：从保存到FRAM中的信息读取到RAM中，内容见结构体VDR_OVERTIME_DRIVER_STRUCT
***************************************************************************/
static void VDROvertime_ReadCurrentDriverInfo(void)
{
    u8  Buffer[30];
	  TIME_T Time;
	  TIME_T CurTime;

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
		
		//夜间开始时间
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_NIGHT_START_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //夜间结束时间
    if(4 == FRAM_BufferRead(Buffer, 4,FRAM_VDR_OVERTIME_NIGHT_END_TIME_ADDR))
    {
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = Public_ConvertBufferToLong(Buffer);
    }

    //夜间开始位置
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].NightStartPosition,Buffer,10);
    }

    //夜间结束位置
    if(10 == FRAM_BufferRead(Buffer, 10,FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR))
    {
        memcpy(VdrOvertime.Driver[DriverNum].NightEndPosition,Buffer,10);
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
		
		//每日累计驾驶时间
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
    Addr = FLASH_OVER_TIME_DRIVE_INFO_START_ADDR;
    sFLASH_EraseSector(Addr);
    
    //一个一个写入
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        //写入驾驶证号码
        memcpy(Buffer,VdrOvertime.Driver[i].License,18);
        sum = Public_GetSumVerify(Buffer,18);
        Buffer[18] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 19);
        Addr += 19;

        //写入开始时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].StartTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //写入结束时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].EndTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //写入开始位置
        memcpy(Buffer,VdrOvertime.Driver[i].StartPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;

        //写入结束位置
        memcpy(Buffer,VdrOvertime.Driver[i].EndPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;
				
				//写入夜间开始时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].NightStartTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //写入夜间结束时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].NightEndTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //写入夜间开始位置
        memcpy(Buffer,VdrOvertime.Driver[i].NightStartPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;

        //写入夜间结束位置
        memcpy(Buffer,VdrOvertime.Driver[i].NightEndPosition,10);
        sum = Public_GetSumVerify(Buffer,10);
        Buffer[10] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 11);
        Addr += 11;

        //写入签到时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LoginTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;

        //写入签退时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].LogoutTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
        Addr += 5;
				
				//写入每日累计驾驶时间
        Public_ConvertLongToBuffer(VdrOvertime.Driver[i].OneDayDriveTimeCount,Buffer);
        sum = Public_GetSumVerify(Buffer,4);
        Buffer[4] = sum;
        sFLASH_WriteBuffer(Buffer, Addr, 5);
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
	  TIME_T CurTime;
	  TIME_T Time;

    Addr = FLASH_OVER_TIME_DRIVE_INFO_START_ADDR;

    //一个一个读出
    for(i=0; i<DRIVER_NUM_MAX; i++)
    {
        flag[i] = 0;

        //读出驾驶证号码
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

        //读出开始时间
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
        
        

        //读出结束时间
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

        //读出开始位置
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

        //读出结束位置
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
				
				//读出夜间开始时间
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
        
        

        //读出夜间结束时间
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

        //读出夜间开始位置
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

        //读出夜间结束位置
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

        
        //读出签到时间
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
        
        

        //读出签退时间
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
				
				//读出每日累计驾驶时间
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
				
        if(1 == flag[i])//数据错误，清0
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
    if(VdrOvertime.Driver[DriverNum].StartTimeCount >= VdrOvertime.Driver[DriverNum].EndTimeCount)//重新计算时间
    {
        VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
    }
    else if((VdrOvertime.Driver[DriverNum].EndTimeCount >= TimeCount))//当前RTC时间错误
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
		
		if(VdrOvertime.Driver[DriverNum].NightStartTimeCount >= VdrOvertime.Driver[DriverNum].NightEndTimeCount)//重新计算时间
    {
        VdrOvertime.Driver[DriverNum].NightStartTimeCount = 0;
        VdrOvertime.Driver[DriverNum].NightEndTimeCount = 0;
    }
    else if((VdrOvertime.Driver[DriverNum].NightEndTimeCount >= TimeCount))//当前RTC时间错误
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

///////////////////////////以下为部标送检添加//////////////////////////////////////
/**************************************************************************
//函数名：VDROvertime_OnceRunReport
//功能：车辆一旦行驶或停驶就立即上报一条位置信息
//输入：无
//输出：无
//返回值：无
//备注：部标协议检测超时驾驶时，重点测试超时驾驶开始时刻和报警时刻
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
//函数名：VDROvertime_StopAlarm
//功能：超时停车报警
//输入：无
//输出：无
//返回值：无
//备注：部标协议检测
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
//函数名：VDROvertime_ClearStartTime
//功能：清除开始时间，重新获取开始时间
//输入：无
//输出：无
//返回值：无
//备注：部标协议检测疲劳驾驶时，当设置疲劳驾驶参数时需要调用此函数
***************************************************************************/
void VDROvertime_ClearStartTime(void)
{
    VdrOvertime.Driver[DriverNum].StartTimeCount = 0;
    VdrOvertime.Driver[DriverNum].EndTimeCount = 0;
}





