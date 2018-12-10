/************************************************************************
//程序名称：VDR_SpeedStatus.c
//功能：速度状态记录。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.10
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
#include "Gps_App.h"
#include "Io.h"

/********************本地变量*************************/
static u8   VdrSpeedStatusBuffer[VDR_DATA_SPEED_STATUS_STEP_LEN];
static u8   VdrSpeedStatusEnableFlag = 0;//速度状态使能标志，1为使能；0为禁止
static u8 VdrSpeedStatusFlag = 0;//速度状态标志，1为异常，0为正常
/********************全局变量*************************/


/********************外部变量*************************/
extern u8 SpeedFlag;
extern u8   GBTestFlag;//0:正常出货运行模式;1:国标检测模式
extern u8  VdrDoubtSecondSpeedReadyFlag1;//VDR_SpeedStatus.c使用
/********************本地函数声明*********************/
static void VDRSpeedStatus_Write(u8 SpeedStatus, u32 StartTime, u32 EndTime);
static void VDRSpeedStatus_Init(void);
static void VDRSpeedStatus_CheckEnableFlag(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRSpeedStatus_TimeTask
//功能：速度状态记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRSpeedStatus_TimeTask(void)
{
    static u16  SpeedAbnormalCount = 0;
    static u16  SpeedNormalCount = 0;
    static u8   VdrSpeedStatusInitFlag = 0;
  static u32 LastTimeCount = 0;
    
    u8  PulseSecondSpeed;
    u8  PulseInstantSpeed;
    u8  GpsSpeed;
    u8  DifferenceSpeed;
    u16 Addr;
    u32 TimeCount;
    TIME_T  Time;


    if(0 == VdrSpeedStatusInitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED_STATUS))
        {
            VDRSpeedStatus_Init();
            VdrSpeedStatusInitFlag = 1;
            VDRSpeedStatus_CheckEnableFlag();
        }
    }
    else
    {
        RTC_GetCurTime(&Time);
        if(1 == Public_CheckTimeStruct(&Time))
        {
            if((0==Time.hour)&&(0==Time.min)&&(0==Time.sec))
            {
                VdrSpeedStatusEnableFlag = 1;
                Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
                FRAM_BufferWrite(Addr, &VdrSpeedStatusEnableFlag, FRAM_VDR_SPEED_STATUS_ENABLE_LEN);    
            }
        }
        if(1 == VdrSpeedStatusEnableFlag)
        {
            TimeCount = RTC_GetCounter();
            if((LastTimeCount != TimeCount)&&(1 == VdrDoubtSecondSpeedReadyFlag1))
            {
                 VdrDoubtSecondSpeedReadyFlag1 = 0;
               PulseSecondSpeed = VDRDoubt_GetSecondSpeed();
                 PulseInstantSpeed = VDRPulse_GetPulseSpeed();
               GpsSpeed = Gps_ReadSpeed();
                
        
               if(PulseSecondSpeed >= GpsSpeed)
               {
                   DifferenceSpeed = PulseSecondSpeed - GpsSpeed;
               }
               else
               {
                   DifferenceSpeed = GpsSpeed - PulseSecondSpeed;
               }

               if((GpsSpeed > 40)
                   &&(PulseSecondSpeed > 0)
                    &&(PulseInstantSpeed > 0)
                   &&((100*DifferenceSpeed) >= (11*GpsSpeed))
                     &&(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION)))//差值大于11%
               {
                   SpeedNormalCount = 0;
                   SpeedAbnormalCount++;
                   if(SpeedAbnormalCount <= 60)
                   {
                       VdrSpeedStatusBuffer[13+(SpeedAbnormalCount-1)*2] = PulseSecondSpeed;
                       VdrSpeedStatusBuffer[13+(SpeedAbnormalCount-1)*2+1] = GpsSpeed;
                   }
                   else if(SpeedAbnormalCount >= 300)
                   {
                       SpeedAbnormalCount = 0;
                       TimeCount = RTC_GetCounter();
                       VDRSpeedStatus_Write(0x02, TimeCount-300, TimeCount);
             VdrSpeedStatusFlag = 1;//异常
                       Addr = FRAM_VDR_SPEED_STATUS_ADDR;
                     FRAM_BufferWrite(Addr, &VdrSpeedStatusFlag, FRAM_VDR_SPEED_STATUS_LEN);
                   }
              }
              else if((GpsSpeed > 40)
                  &&(PulseSecondSpeed > 0)
                   &&(PulseInstantSpeed > 0)
                  &&((100*DifferenceSpeed) < (11*GpsSpeed))
                    &&(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION)))//差值小于11%
              {
                  SpeedAbnormalCount = 0;
                  SpeedNormalCount++;
                  if(SpeedNormalCount <= 60)
                  {
                      VdrSpeedStatusBuffer[13+(SpeedNormalCount-1)*2] = PulseSecondSpeed;
                      VdrSpeedStatusBuffer[13+(SpeedNormalCount-1)*2+1] = GpsSpeed;
                  }
                  else if(SpeedNormalCount >= 300)
                  {
                      SpeedNormalCount = 0;
                      TimeCount = RTC_GetCounter();
                      VDRSpeedStatus_Write(0x01, TimeCount-300, TimeCount);
            VdrSpeedStatusFlag = 0;//正常
                      Addr = FRAM_VDR_SPEED_STATUS_ADDR;
                    FRAM_BufferWrite(Addr, &VdrSpeedStatusFlag, FRAM_VDR_SPEED_STATUS_LEN);
                  }
              }
              else
              {
                  SpeedAbnormalCount = 0;
                  SpeedNormalCount = 0;
              }
              LastTimeCount = TimeCount;
          }
        }
        
    }

    return ENABLE;      
}
/**************************************************************************
//函数名：VDRSpeedStatus_GetFlag
//功能：获取速度状态标志
//输入：无
//输出：无
//返回值：0:正常;1:异常
//备注：
***************************************************************************/
u8 VDRSpeedStatus_GetFlag(void)
{
    
     return VdrSpeedStatusFlag;
 
}
/**************************************************************************
//函数名：VDRSpeedStatus_CheckEnableFlag
//功能：检查速度状态使能标志
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void VDRSpeedStatus_CheckEnableFlag(void)
{
    TIME_T  Time;
    TIME_T  Time2; 
    u16 TotalList; 
    u32 TimeCount;
     
    TotalList = VDRData_ReadTotalList(VDR_DATA_TYPE_SPEED_STATUS);
    if(TotalList > 0)
    {   
        TotalList--; 
        TimeCount = VDRData_ReadListTime(VDR_DATA_TYPE_SPEED_STATUS,TotalList); 
        Gmtime(&Time2, TimeCount);
        RTC_GetCurTime(&Time);
        if(1 == Public_CheckTimeStruct(&Time2))
        {
            if((Time.year == Time2.year)&&(Time.month == Time2.month)&&(Time.day == Time2.day))
            { 
                VdrSpeedStatusEnableFlag = 0;   
            }
            else
            {  
                VdrSpeedStatusEnableFlag = 1;   
            }
        }
        else
        { 
            VdrSpeedStatusEnableFlag = 1;
        }
    }   
    else
    { 
        VdrSpeedStatusEnableFlag = 1;   
    }
}
/**************************************************************************
//函数名：VDRSpeedStatus_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：无
//备注：主要是上电更新变量VdrSpeedStatusEnableFlag
***************************************************************************/
static void VDRSpeedStatus_Init(void)
{
    u16 Addr;
    u8  Buffer[2];
    u8  BufferLen;

    Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;

    BufferLen = FRAM_BufferRead(Buffer, FRAM_VDR_SPEED_STATUS_ENABLE_LEN, Addr);
    if(0 != BufferLen)
    {
        VdrSpeedStatusEnableFlag = Buffer[0];
    }
    else
    {
        VdrSpeedStatusEnableFlag = 1;
    }

    Addr = FRAM_VDR_SPEED_STATUS_ADDR;

    BufferLen = FRAM_BufferRead(Buffer, FRAM_VDR_SPEED_STATUS_LEN, Addr);
    if(0 != BufferLen)
    {
        VdrSpeedStatusFlag = Buffer[0];
    }
    else
    {
        VdrSpeedStatusFlag = 0;
    }
}
/**************************************************************************
//函数名：VDRSpeedStatus_Write
//功能：保存一条速度状态记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
static void VDRSpeedStatus_Write(u8 SpeedStatus, u32 StartTime, u32 EndTime)
{

    TIME_T Time;
    u16 Addr;


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

    VdrSpeedStatusBuffer[0] = SpeedStatus;//速度状态，1字节，0x01:正常;0x02:异常

    Gmtime(&Time, StartTime);
    VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+1,&Time);//开始时间
                
    Gmtime(&Time, EndTime);
    VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+7,&Time);//结束时间
    
    VDRData_Write(VDR_DATA_TYPE_SPEED_STATUS, VdrSpeedStatusBuffer, VDR_DATA_SPEED_STEP_LEN-5, EndTime);


    VdrSpeedStatusEnableFlag = 0;
    Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
    FRAM_BufferWrite(Addr, &VdrSpeedStatusEnableFlag, FRAM_VDR_SPEED_STATUS_ENABLE_LEN);                
            
}






