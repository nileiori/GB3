#include "VDR_Coefficient.h"
//#include "stm32f2xx.h"
//#include "stm32f2xx_rtc.h"
#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "misc.h"
#include "MileMeter.h"
#include "Gps_App.h"
#include "Io.h"
#include "EepromPram.h"
#include "Public.h"
#include "taskschedule.h"
#include "Pulse_App.h"
#include "SysTickApp.h"
#include "VDR_Pulse.h"

#include <string.h>

u32	VdrCoefficientPulseCount = 0;//脉冲计数，收1个脉冲，计数加1，当收到PulseCoef个脉冲时表示走了1公里
static u32 VdrCoefficientPulseTotalCount = 0;

static  u8  VdrCoefficientInitFlag = 0;
static u8  VdrCoefficientEnableFlag = 0;
static Point_Struct VdrCoefficientOldPoint;
static Point_Struct VdrCoefficientPoint;
static d64 VdrCoefficientGpsMileMeter = 0;
static u32 VdrCoefficientGpsMile = 0;
//static u32 VdrCoefficientPulseMile = 0;
static 	 u16 VdrCoefficientPulseCoef = 0;
//static  u16  VdrCoefficientCalCoef[3] = {0};//每次计算得到的脉冲系数放在该数组中
static  u16  VdrCoefficientCalCoef = 0;
//static  u8  VdrCoefficientCalCount = 0;//计算次数计数
static u32  VdrStartTimeCount = 0;
static u32  VdrEndTimeCount = 0;

static d64 PointDistanceArray[100];
static GPS_STRUCT GpsDataArray[100];

static void VDRCoefficient_GetPoint(Point_Struct *Point);//获取当前的位置点
static d64 VDRCoefficient_CalDistance(Point_Struct *Point1,Point_Struct *Point2);//计算两点间距离
static void VDRCoefficient_ModifyPulseCoef(void);//修改脉冲系数

FunctionalState VDRCoefficient_TimeTask(void) 
{
  u8  PulseSpeed = 0;
  u8  GpsSpeed = 0;
  u8  NavigationFlag = 0;
  u8  Acc = 0;
  u16 VdrCoefficientTempCoef = 0;
  float timecount;
  d64 PointDistance;
  
  static u8 count = 0;
  TIME_T t1,t2;
  static u8 State = 0;
  static u8 CalCount = 0;
  static u8 ErrorCount = 0;
  
  if(0 == VdrCoefficientInitFlag)
  {
    VDRCoefficient_Init();
    VdrCoefficientInitFlag = 1;
    State = 0;
  }
  else if(1 == VdrCoefficientEnableFlag)
  {
    PulseSpeed = VDRPulse_GetSecondSpeed();
    GpsSpeed = Gps_ReadSpeed();
    NavigationFlag = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);
    Acc = Io_ReadStatusBit(STATUS_BIT_ACC);
    if((GpsSpeed > 40)&&(PulseSpeed > 0)&&(1 == NavigationFlag)&&(1 == Acc)) 
    {
      switch(State)
      {
        case 0://获取第1个位置点（即使OldPoint有个初始值）
        {
          VDRCoefficient_GetPoint(&VdrCoefficientOldPoint);
          State++;
          ErrorCount = 0;
          count = 0;
          break;
        }
        case 1://确定里程计算的起点
        {
          VDRCoefficient_GetPoint(&VdrCoefficientPoint);
          if((VdrCoefficientPoint.LonDegree != VdrCoefficientOldPoint.LonDegree)
            ||(VdrCoefficientPoint.LatDegree != VdrCoefficientOldPoint.LatDegree))
          {
            VdrCoefficientOldPoint.LonDegree = VdrCoefficientPoint.LonDegree;			
            VdrCoefficientOldPoint.LatDegree = VdrCoefficientPoint.LatDegree;	
      
            VdrCoefficientGpsMileMeter = 0;
            VdrCoefficientGpsMile = 0;
            VdrCoefficientPulseCount = 0;
            VdrCoefficientPulseTotalCount = 0;
            
            VdrStartTimeCount = Timer_Val();
            RTC_GetCurTime(&t1);
            Gps_CopygPosition(&GpsDataArray[0]);
            
            CalCount = 0;
            ErrorCount = 0;
            count = 0;
            State++;
          }
          else
          {
            ErrorCount++;
            if(ErrorCount >= 25)//50ms one time 
            {
              ErrorCount = 0;
              State = 0;
              VdrCoefficientInitFlag = 0;
            }
          }
          break;
        }
        case 2://计算GPS里程和脉冲里程
        {
          VDRCoefficient_GetPoint(&VdrCoefficientPoint);
          if((VdrCoefficientPoint.LonDegree != VdrCoefficientOldPoint.LonDegree)
            ||(VdrCoefficientPoint.LatDegree != VdrCoefficientOldPoint.LatDegree))
          {
            count++;
            if(count >= 99)
            {
              count = 1;
            }
            Gps_CopygPosition(&GpsDataArray[count]);
        
            PointDistance = VDRCoefficient_CalDistance(&VdrCoefficientOldPoint, &VdrCoefficientPoint);	
            
            PointDistanceArray[count] = PointDistance;
            if(0 == PointDistanceArray[count])
            {
              PointDistanceArray[count] = 0;
            }
            
            VdrCoefficientGpsMileMeter += PointDistance;			
            VdrCoefficientGpsMile = (u32)VdrCoefficientGpsMileMeter;//单位米
            
            VdrCoefficientPulseTotalCount += VdrCoefficientPulseCount;
            VdrCoefficientPulseCount = 0;
            
            VdrCoefficientOldPoint.LonDegree = VdrCoefficientPoint.LonDegree;			
            VdrCoefficientOldPoint.LatDegree = VdrCoefficientPoint.LatDegree;	
      
            CalCount++;
            
            ErrorCount = 0;
            
            if(CalCount >= 60)
            //if(CalCount >= 30)
            {
              VdrEndTimeCount = Timer_Val();
              
              RTC_GetCurTime(&t2);
              
              timecount = (VdrEndTimeCount-VdrStartTimeCount)/1200.0;
              //timecount = (VdrEndTimeCount-VdrStartTimeCount)/600.0;
              
              VdrCoefficientPulseTotalCount = (u32)(VdrCoefficientPulseTotalCount/timecount);
             
              VdrCoefficientTempCoef = 1000*VdrCoefficientPulseTotalCount/VdrCoefficientGpsMile;
              if((VdrCoefficientTempCoef > 0)&&(VdrCoefficientTempCoef <= 20000))
              {
                //if(VdrCoefficientCalCount > 2)//程序出错，从头再来(目前只计算2次求平均)
                //if(VdrCoefficientCalCount > 1)
                //{
                  //VdrCoefficientCalCount = 0;
                //}
                //VdrCoefficientCalCoef[VdrCoefficientCalCount] = VdrCoefficientTempCoef;
                VdrCoefficientCalCoef = VdrCoefficientTempCoef;
                //VdrCoefficientCalCount++;
                //if(VdrCoefficientCalCount >= 2)//目前只计算2次求平均
                //if(VdrCoefficientCalCount >= 1)
                //{
                  State++;
                //}
                //else
                //{
                 // VdrCoefficientInitFlag = 0;//本次校准不算,重新开始
                 // State = 0;
                //}
              }
              else
              {
                 VdrCoefficientInitFlag = 0;//本次校准不算,重新开始
                 State = 0;
              }
            }
          }
          else
          {
            ErrorCount++;
            if(ErrorCount >= 25)//50ms one time 
            {
              ErrorCount = 0;
              State = 0;
              VdrCoefficientInitFlag = 0;
            }
          }
          break;
        }
        case 3:
        {
          VDRCoefficient_ModifyPulseCoef();
          VdrCoefficientEnableFlag = 0;
          VdrCoefficientInitFlag = 0;
          State = 0;
          VdrCoefficientCalCoef = 0;
          //VdrCoefficientCalCount = 0;
          //VdrCoefficientCalCoef[0] = 0;
          //VdrCoefficientCalCoef[1] = 0;
         // VdrCoefficientCalCoef[2] = 0;
          break;
        }
      }
    }
    else
    {
      State = 0;
      VdrCoefficientInitFlag = 0;
    }
  }
  else
  {
    State = 0;
    VdrCoefficientInitFlag = 0;
    //VdrCoefficientCalCount = 0;
    //VdrCoefficientCalCoef[0] = 0;
   // VdrCoefficientCalCoef[1] = 0;
    //VdrCoefficientCalCoef[2] = 0;
    VdrCoefficientCalCoef = 0;
    return DISABLE;
  }
  
  return ENABLE;
}

void VDRCoefficient_EnableCalibration(void) //使能校准
{
  //if(0 == VdrCoefficientEnableFlag)
  //{
    VdrCoefficientInitFlag = 0;
    VdrCoefficientEnableFlag = 1;
    //VdrCoefficientCalCount = 0;
    //VdrCoefficientCalCoef[0] = 0;
    //VdrCoefficientCalCoef[1] = 0;
   // VdrCoefficientCalCoef[2] = 0;
    VdrCoefficientCalCoef = 0;
    SetTimerTask(TIME_VDR_COEFFICIENT, 1);
  //}
}

void VDRCoefficient_DisableCalibration(void) //关闭校准
{
  VdrCoefficientEnableFlag = 0;
}

u8 VDRCoefficient_GetFlag(void) //1:校准中（忙）,0:校准完毕（空闲）
{
  return VdrCoefficientEnableFlag;
}

void VDRCoefficient_Init(void)
{
  u8  TmpData[4] = {0};
  u8  PramLen = 0;
 

  PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, TmpData);
  if(E2_CAR_FEATURE_COEF_LEN != PramLen)
  {		
    VdrCoefficientPulseCoef = 7200;//默认为7200，取消了速度传感器系数
  }
  else
  {		
    VdrCoefficientPulseCoef = 0;	
    VdrCoefficientPulseCoef |= (TmpData[0] << 16);//正常情况下TmpData[0]==0
    VdrCoefficientPulseCoef |= (TmpData[1] << 8);//高字节		
    VdrCoefficientPulseCoef |= TmpData[2];//低字节位
  }
  
  if((7200 == VdrCoefficientPulseCoef)||(3600 == VdrCoefficientPulseCoef))
  {
    VDRCoefficient_EnableCalibration();
  }
}

static void VDRCoefficient_GetPoint(Point_Struct *Point)//获取当前的位置点
{
          
        GPS_STRUCT GpsData;
        d64 temp;
	
        Gps_CopygPosition(&GpsData);
        //Gps_CopyPosition(&GpsData);
        
	temp = GpsData.Longitue_FX/10000.0;
	temp = temp + GpsData.Longitue_F;
	temp = temp/60.0;
	temp = temp + GpsData.Longitue_D;
	Point->LonDegree = temp;
	temp = GpsData.Latitue_FX/10000.0;
	temp = temp + GpsData.Latitue_F;
	temp = temp/60.0;
	temp = temp + GpsData.Latitue_D;
	Point->LatDegree = temp;
}

static d64 VDRCoefficient_CalDistance(Point_Struct *Point1,Point_Struct *Point2)//计算两点间距离
{

	d64 distance,temp,RadLon1,RadLat1,RadLon2,RadLat2;
	u8	flag = 0;
		
	RadLon1 = Point1->LonDegree*PI/180.0;
	RadLat1 = Point1->LatDegree*PI/180.0;
	RadLon2 = Point2->LonDegree*PI/180.0;
	RadLat2 = Point2->LatDegree*PI/180.0;
	
	//纬度或经度变化要超过一定范围,(1分==1852米,推出1米==0.000009度)
	temp = (d64)fabs(Point1->LonDegree-Point2->LonDegree);
	if(temp > 0.000009)
	{
		flag = 1;
	}
	temp = (d64)fabs(Point1->LatDegree-Point2->LatDegree);
	if(temp > 0.000009)
	{
		flag = 1;
	}
	if(1 == flag)
	{
		temp = 2.0-2.0*cos(RadLat1)*cos(RadLat2)*cos(RadLon2-RadLon1)-2.0*sin(RadLat2)*sin(RadLat1);
		if(temp > 0)//保证为正
		{
			distance = REarth*sqrt(temp);
			return distance;
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

static void VDRCoefficient_ModifyPulseCoef(void)//修改脉冲系数
{
  s32 sum = 0;
  u8  Buffer[5];
  
 // if((0 != VdrCoefficientCalCoef[0])&&(0 != VdrCoefficientCalCoef[1]))
  if(0 != VdrCoefficientCalCoef)
  {
    //sum = (VdrCoefficientCalCoef[0]+VdrCoefficientCalCoef[1])/2;
    sum = VdrCoefficientCalCoef;
  
    if((sum > 0)&&(sum <= 20000))
    {
      Public_ConvertLongToBuffer(sum, Buffer);
      if(0 == EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &Buffer[1], E2_CAR_FEATURE_COEF_LEN))
      {
        EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &Buffer[1], E2_CAR_FEATURE_COEF_LEN);
      }
//      Pulse_UpdataPram();
			  VDRPulse_UpdateParameter();
    }
  }
 
}