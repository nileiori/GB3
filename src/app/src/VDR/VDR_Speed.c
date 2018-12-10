/************************************************************************
//程序名称：VDR_Speed.c
//功能：48h行驶速度记录
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
#include "MileMeter.h"
#include "Gps_App.h"
#include "Io.h"

/********************本地变量*************************/
static u8   CurrentSecondSpeed[VDR_DATA_SPEED_STEP_LEN];//当前分钟的每秒钟速度和状态缓冲
//static u8 LastSecondSpeed[VDR_DATA_SPEED_STEP_LEN];//上一分钟的每秒钟速度和状态缓冲，前6字节为开始时间
static u8   MinuteSpeed = 0;//每分钟平均速度
static s8   MinuteSpeedCount = 0;//记录的位置
u32 TestTimeCount2 = 0;

/********************全局变量*************************/

/********************外部变量*************************/
extern u8   SpeedFlag;//速度类型,0为脉冲,1为GPS,2自动脉冲,3自动GPS
extern u8  VdrDoubtSecondSpeedReadyFlag2;//求每分钟平均速度使用
extern u8  GBTestFlag;//0:正常出货运行模式;1:国标检测模式
/********************本地函数声明*********************/
static void VDRSpeed_Init(void);
static void VDRSpeed_SaveMinuteSpeed(u32 TimeCount, u8 MinuteSpeed);

/********************函数定义*************************/
/**************************************************************************
//函数名：VDRSpeed_TimeTask
//功能：48h行驶速度记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：位置汇报定时任务，50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
/* dxl,2015.10.30,中间会有丢失秒速度的情况
FunctionalState VDRSpeed_TimeTask(void)
{
    static u32  LastTimeCount = 0;
    static u8   AlreadyRunFlag = 0;//1为已行驶过，0为未行驶过
    static u8   LastRunFlag = 0;
    static u8   InitFlag = 0;

    u32 CurrentTimeCount;
    u32 TimeCount;
    u32 temp;
    u16 sum;
    u16 Addr;
    u8  Speed;
    u8  Status;
    u8  i,j,k;
    u8  RunFlag;
    u8  Buffer[5];
    TIME_T  CurrentTime;
  TIME_T    Time;
    if(0 == InitFlag)
    {
        if( 1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED))
        {
            VDRSpeed_Init();
            InitFlag = 1;
            //VDRGprs_PacketAckPreHandle(0);
        }
    }
    else
    {
        CurrentTimeCount = RTC_GetCounter();

        if(CurrentTimeCount > LastTimeCount)//秒时钟动了
        {
            j = CurrentTimeCount-LastTimeCount;
            RTC_GetCurTime(&CurrentTime);
            //if(1 == Public_CheckTimeStruct(&CurrentTime))
            //{
                Speed = VDRPulse_GetSecondSpeed();//速度

                Status = 0;//状态
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:刹车
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:左转
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:右转
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:远光
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:近光
                //bit2-bit1未定义
                Status |= Io_ReadStatusBit(STATUS_BIT_ACC);//bit0:ACC
            
                if(1 == j)
                {
                  CurrentSecondSpeed[2*CurrentTime.sec] = Speed;
                  CurrentSecondSpeed[2*CurrentTime.sec+1] = Status;
                }
                else if(j > 1)//dxl,2015.10.30,有时RTC可能存在偶尔跳秒的情况
                {
                    TimeCount = CurrentTimeCount;
                    CurrentSecondSpeed[2*CurrentTime.sec] = Speed;
                  CurrentSecondSpeed[2*CurrentTime.sec+1] = Status;
                    for(k=1; k<j; k++)
                    {
                      TimeCount--;
                      Gmtime(&Time, TimeCount);
                      CurrentSecondSpeed[2*Time.sec] = Speed;
                    CurrentSecondSpeed[2*Time.sec+1] = Status;
                    }
                  
                }

                RunFlag = VDRDoubt_GetCarRunFlag();
                if(1 == RunFlag)
                {
                    AlreadyRunFlag = 1;
                }
                if((1 == LastRunFlag)&&(0 == RunFlag))//由行驶变为停驶，记录停车时间
                {
                    temp = CurrentTimeCount;
                    temp -= 2;//实际测试发现该时间比事故疑点的停车时间慢1-2秒,因此减2
                    Public_ConvertLongToBuffer(temp,Buffer);
                    Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;
                    FRAM_BufferWrite(Addr, Buffer, 4);
                }

                if(59 == CurrentTime.sec)//最后一秒把数据暂存
                {
                    VDRPub_ConvertNowTimeToBCD(LastSecondSpeed);//6字节时间
                    LastSecondSpeed[5] = 0;//秒强制为0
                    sum = 0;
                    for(i=0; i<60; i++)
                    {
                        LastSecondSpeed[6+2*i] = CurrentSecondSpeed[2*i];
                        LastSecondSpeed[6+2*i+1] = CurrentSecondSpeed[2*i+1];
                        sum += CurrentSecondSpeed[2*i];
                    }
                    MinuteSpeed = sum/60;
                    for(i=0; i<120; i++)
                    {
                        CurrentSecondSpeed[i] = 0;
                    }
                
                }
                else if(9 == CurrentTime.sec)//每次在第9秒判断是否要把上一分钟数据写入到flash
                {
                    if(1 == AlreadyRunFlag)//已行驶过
                    {
                        sum = 0;
                        for(i=0; i<60; i++)
                        {
                            if(0 != LastSecondSpeed[6+2*i])
                            {
                                sum++;
                            }
                        }
                        //if(sum > 0)也可能出现只行驶了几秒，每分钟平均速度还是为0的情况
                        //{
                            TimeCount = CurrentTimeCount;
                            TimeCount -= 69;
                            VDRData_Write(VDR_DATA_TYPE_SPEED, LastSecondSpeed, VDR_DATA_SPEED_STEP_LEN-5, TimeCount);//记录每分钟的行驶速度和状态
                            
                            VDRSpeed_SaveMinuteSpeed(TimeCount, MinuteSpeed);//记录停车前15分钟速度
                        //}

                    }
                    AlreadyRunFlag = 0;
                }

                LastRunFlag = RunFlag;
            //}
            //else//时间错误
            //{
            
            //}
        }
    
        LastTimeCount = CurrentTimeCount;
    }

    return ENABLE;      
}
*/
FunctionalState VDRSpeed_TimeTask(void)
{
    static u32  LastTimeCount = 0;
    //static u8 AlreadyRunFlag = 0;//1为已行驶过，0为未行驶过
    static u8   LastRunFlag = 0;
    static u8   InitFlag = 0;
    static u8 SpeedCount = 0;
    //static u8 EnableWriteFlag = 0;
  //static u8 EnableWriteCount = 0;
    
    u32 CurrentTimeCount;
    u32 TimeCount;
    u32 temp;
    u16 sum;
    u16 Addr;
    u8  Speed;
    u8  Status;
    u32 i,j,k;
    u8  RunFlag;
    u8  Buffer[5];
    TIME_T  CurrentTime;
    TIME_T  Time;
    
    if(0 == InitFlag)
    {
        if( 1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED))
        {
            VDRSpeed_Init();
            CurrentTimeCount = RTC_GetCounter();
            Gmtime(&CurrentTime, CurrentTimeCount);
            if(1 == Public_CheckTimeStruct(&CurrentTime))
            {
                LastTimeCount = CurrentTimeCount;
                SpeedCount = CurrentTime.sec;
                if(0 == SpeedCount)
                {
                 SpeedCount = 59;
                }
                else
                {
                  SpeedCount--;//当前时刻使用的秒平均速度其实是上一秒的速度，因此需减1
                }
              InitFlag = 1;
            }
        }
    }
    else
    {
        CurrentTimeCount = RTC_GetCounter();

        if(CurrentTimeCount != LastTimeCount)//秒时钟动了
        {
            if(CurrentTimeCount != (LastTimeCount+1))//校时
            {
                if(CurrentTimeCount < LastTimeCount)
                {
                  for(i=0; i<60; i++)
                {
                    CurrentSecondSpeed[6+2*i] = 0x00;   //速度为0   
            CurrentSecondSpeed[6+2*i+1] = 0x00; //未使用的bit2置为1         
                }
              }
                else
                {
                   RunFlag = VDRDoubt_GetCarRunFlag();
                     if(1 == RunFlag)//行驶中跳秒
                     {
                       if(CurrentTimeCount >= (LastTimeCount+60))
                         {
                           for(i=0; i<60; i++)
                     {
                         CurrentSecondSpeed[6+2*i] = 0x00;  //速度为0   
                 CurrentSecondSpeed[6+2*i+1] = 0x00;    //未使用的bit2置为1         
                     }
                         }
                         else
                         {
                           j = CurrentTimeCount-LastTimeCount;
                             Gmtime(&Time, LastTimeCount);
                             Speed = CurrentSecondSpeed[6+2*Time.sec];
                             Status = CurrentSecondSpeed[6+2*Time.sec+1];
                             k = Time.sec;
                             for(i=0; i<j; i++)
                             {
                               k++;
                                 if(k >= 60)
                                 {
                                   k = 0;
                                 }
                                 CurrentSecondSpeed[6+2*k] = Speed;
                                 CurrentSecondSpeed[6+2*k+1] = Status;
                             }
                         }
                     }
                     else
                     {
                       for(i=0; i<60; i++)
                   {
                       CurrentSecondSpeed[6+2*i] = 0x00;    //速度为0   
               CurrentSecondSpeed[6+2*i+1] = 0x00;  //未使用的bit2置为1         
                   }
                     }
                }
                LastTimeCount = CurrentTimeCount;
                Gmtime(&CurrentTime, CurrentTimeCount);
                SpeedCount = CurrentTime.sec;           
                if(0 == SpeedCount)
                {
                 SpeedCount = 59;
                }
                else
                {
                  SpeedCount--;//当前时刻使用的秒平均速度其实是上一秒的速度，因此需减1
                }
              return ENABLE;
            }
            
            RunFlag = VDRDoubt_GetCarRunFlag();
            if((1 == LastRunFlag)&&(0 == RunFlag))//由行驶变为停驶，记录停车时间
            {
                    temp = CurrentTimeCount;
                    temp -= 2;//实际测试发现该时间比事故疑点的停车时间慢1-2秒,因此减2
                    Public_ConvertLongToBuffer(temp,Buffer);
                    Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;
                    FRAM_BufferWrite(Addr, Buffer, 4);
            }
            LastRunFlag = RunFlag;
                
            RTC_GetCurTime(&CurrentTime);
            
            if((1 == Public_CheckTimeStruct(&CurrentTime))&&(1 == VdrDoubtSecondSpeedReadyFlag2))
            {
                VdrDoubtSecondSpeedReadyFlag2 = 0;
                
              Speed = VDRSpeed_GetCurSpeed();//速度

              Status = 0;//状态
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:刹车
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:左转
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:右转
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:远光
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:近光
              //bit2-bit1未定义
              Status |= Io_ReadStatusBit(STATUS_BIT_ACC);//bit0:ACC
            
                CurrentSecondSpeed[6+2*SpeedCount] = Speed;
            CurrentSecondSpeed[6+2*SpeedCount+1] = Status;
            
                SpeedCount++;
                if(SpeedCount >= 60)
              {
                    SpeedCount = 0;
          sum = 0;
                  j = 0;
                  for(i=0; i<60; i++)
                  {
                      sum += CurrentSecondSpeed[6+2*i];
                      if(CurrentSecondSpeed[6+2*i] > 0)
            {
                        j++;
                      }                     
                  }
                    MinuteSpeed = sum/60;
                  if(j >= 1)
                  {
                    TimeCount = CurrentTimeCount;
                    TimeCount -= 50;
                        
                        //VDRPub_ConvertNowTimeToBCD(CurrentSecondSpeed);//6字节时间
                        Gmtime(&Time, TimeCount);
                        VDRPub_ConvertTimeToBCD(CurrentSecondSpeed, &Time);
                  CurrentSecondSpeed[5] = 0;//秒强制为0
                        
                    VDRData_Write(VDR_DATA_TYPE_SPEED, CurrentSecondSpeed, VDR_DATA_SPEED_STEP_LEN-5, TimeCount);//记录每分钟的行驶速度和状态
                    
                    VDRSpeed_SaveMinuteSpeed(TimeCount, MinuteSpeed);//记录停车前15分钟速度
                  }
                  for(i=0; i<60; i++)
                  {
                    CurrentSecondSpeed[6+2*i] = 0;      
            CurrentSecondSpeed[6+2*i+1] = 0;                        
                  }
              }
                LastTimeCount = CurrentTimeCount;
          }
        }
    }
    return ENABLE;
}
/**************************************************************************
//函数名：VDRSpeed_GetMinuteSpeed
//功能：获取每分钟平均速度
//输入：无
//输出：无
//返回值：每分钟平均速度
//备注：无
***************************************************************************/
u8 VDRSpeed_GetMinuteSpeed(void)
{
    return MinuteSpeed;
}
/**************************************************************************
//函数名：VDRSpeed_Get15MinuteSpeed
//功能：获取停车前15每分钟速度
//输入：无
//输出：无
//返回值：数据长度
//备注：长度为0表示没有读取到正确的值，长度为51（6字节+3字节*15）表示读取了正确的值
//数据排列顺序是：
//停车时间（6字节）
//停车前1分钟的数据（3字节，1字节小时(BCD码)，1字节分钟(BCD码)，1字节平均速度(hex)
//停车前2分钟的数据（3字节，1字节小时，1字节分钟，1字节平均速度，均为hex值）
//停车前3分钟的数据（3字节，1字节小时，1字节分钟，1字节平均速度，均为hex值）
//......
***************************************************************************/
u8 VDRSpeed_Get15MinuteSpeed(u8 *pBuffer)
{
    u8  Buffer[10];
    u8  BufferLen;
    u8  Speed[20];
    u8  *p = NULL;
    u8  length;
    s32 i;
    s32 j;
    s32 k;
        s8      count;
    u16 Addr;
    u32 StopTimeCount;
    u32 MinuteTimeCount;
    u32 LastMinuteTimeCount;
    TIME_T  Time;

        
        count = MinuteSpeedCount;
        count--;
        if(count < 0)
        {
               count = 15;
        }
        if(0 == VDRDoubt_GetCarRunFlag())
        {
                
            Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;//读停车时间
            BufferLen = FRAM_BufferRead(Buffer, 4, Addr);
            if(0 == BufferLen)
            {
                Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+count*6;//6 = 4字节时间+1字节平均速度+1字节校验
                BufferLen = FRAM_BufferRead(Buffer, 5, Addr);
                if(0 == BufferLen)
                {
                    return 0;
                }
                else
                {   
                    StopTimeCount = Public_ConvertBufferToLong(Buffer);
                }
            }
            else
            {
                StopTimeCount = Public_ConvertBufferToLong(Buffer);
            }
        }
        else
        {
                StopTimeCount = RTC_GetCounter();
        }
        
        Gmtime(&Time, StopTimeCount);
        if(0 == Public_CheckTimeStruct(&Time))
        {
                return 0;
        }

        LastMinuteTimeCount = StopTimeCount;
    length = 0;//变量初始化
    p = Speed;
    for(i=0; i<15; i++)
    {
        Speed[i] = 0;
    }
		i = 0;
    for(;;)
    {
        Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+count*6;
        BufferLen = FRAM_BufferRead(Buffer, 5, Addr);
        if(0 == BufferLen)
        {
            *p++ = 0;
            length++;
        }
        else
        {
            MinuteTimeCount = Public_ConvertBufferToLong(Buffer);
            //if((MinuteTimeCount >= (StopTimeCount-60))&&(MinuteTimeCount < StopTimeCount))
            //{
              //这是停车那分钟的数据
            //}
            //else if((0xFFFFFFFF == MinuteTimeCount)||(0 == MinuteTimeCount))//可能是铁电未写入值
            if((0xFFFFFFFF == MinuteTimeCount)||(0 == MinuteTimeCount))//可能是铁电未写入值
            {
                *p++ = 0;
                length++;
            }
            else if(MinuteTimeCount >= LastMinuteTimeCount)//当前步的时间大于上一步，这种是错误
            {
               
            }
            else
            {
                k = (LastMinuteTimeCount - MinuteTimeCount)/60;
                if(k >= 1)
                {
                  k--;//需要填补的0的个数
                }
                if((k+length) > 15)
                {
                    k = 15-length;
                }
                for(j=0; j<k; j++)
                {
                    *p++ = 0;
                    length++;
                }
                *p++ = Buffer[4];
                length++;
                LastMinuteTimeCount = MinuteTimeCount;
            }   
        }

        count--;
        if(count < 0)
        {
            count = 15;
        }

        if(length >= 15)
        {
            break;
        }
        i++;
        if(i >= 15)
        {
				    break;
				}					
    }

    p = pBuffer;
        StopTimeCount -= 10;//持续10秒为0才认为停驶
        Gmtime(&Time, StopTimeCount);
        if(0 == Public_CheckTimeStruct(&Time))
        {
                return 0;
        }
        VDRPub_ConvertTimeToBCD(p,&Time);
        p += 6;
        
    StopTimeCount -= Time.sec;
    for(i=0; i<15; i++)
    {
        Gmtime(&Time, StopTimeCount);
        *p++ = VDRPub_HEX2BCD(Time.hour);
        *p++ = VDRPub_HEX2BCD(Time.min);
        *p++ = Speed[i];
        StopTimeCount -= 60;
    }
    
    return 51;  
}
/*************************************************************
** 函数名称: VDRSpeed_GetCurSpeed
** 功能描述: 获取当前速度(已考虑了速度类型，不需再判断速度类型了)
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char VDRSpeed_GetCurSpeed(void)
{
    u8  CurrentSpeed;   
	
	  if(1 == GBTestFlag)
		{
		    if(VDRDoubt_GetSecondSpeed() > 0)
				{
				    CurrentSpeed = VDRDoubt_GetSecondSpeed();
				}
				else
				{
				    CurrentSpeed = Gps_ReadSpeed();
				}
		}
		else
		{
        //获取速度选择
        if(0 == VDRSpeed_GetCurSpeedType())
        {
            CurrentSpeed = VDRDoubt_GetSecondSpeed();
        }
        else
        {
            CurrentSpeed = Gps_ReadSpeed();
        }   
	  }		
    return CurrentSpeed;
}
/*************************************************************
** 函数名称: VDRSpeed_GetCurSpeedType
** 功能描述: 获取速度类型
** 入口参数: 
** 出口参数: 
** 返回参数: 0为脉冲,1为GPS
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char VDRSpeed_GetCurSpeedType(void)
{
    u8  flag;
    
    flag = 0x01 & SpeedFlag;
    
    return flag;
}
/**************************************************************************
//函数名：VDRSpeed_UpdateParameter
//功能：更新参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDRSpeed_UpdateParameter(void)
{
        u8  PramLen;
        
        //读取速度选择值
    PramLen = EepromPram_ReadPram(E2_SPEED_SELECT_ID, &SpeedFlag);
        if(1 == PramLen)
        {
    
          //更新总里程数
          if(0 == (SpeedFlag&0x01))//dxl,2015.5.11
          {
            VDRPulse_UpdateParameter();
          }
              else
              {
                      //为GPS速度
                      GpsMile_UpdatePram();
                  MileMeter_ClearOldPoint();
              }
        }
        else
        {
        //设置为GPS速度
                SpeedFlag = 0x03;//为GPS速度//dxl,2015.5.11
                GpsMile_UpdatePram();
            MileMeter_ClearOldPoint();
        }
}
////////////////////以下是内部函数/////////////////////////
/**************************************************************************
//函数名：VDRSpeed_Init
//功能：上电初始化
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void VDRSpeed_Init(void)
{
    u8  i;
    u8  Buffer[5];
    u8  BufferLen;
    u16 Addr;
    u32 TimeCount;
    u32 MaxTimeCount = 0;
    
        
        VDRSpeed_UpdateParameter();
        
    for(i=0; i<120; i++)
    {
            
        CurrentSecondSpeed[i] = 0;
    }

    //for(i=0; i<126; i++)
    //{
            
        //LastSecondSpeed[i] = 0;
    //}

    MinuteSpeedCount = 0;
    Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_COUNT_ADDR;
    BufferLen = FRAM_BufferRead(Buffer, 1, Addr);
    if(0 != BufferLen)
    {
        MinuteSpeedCount = Buffer[0];
    }
    else
    {
        for(i=0; i<16; i++)
        {
            Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+i*6;
            BufferLen = FRAM_BufferRead(Buffer, 5, Addr);
            if(0 != BufferLen)
            {
                TimeCount = Public_ConvertBufferToLong(Buffer);
                if((TimeCount > MaxTimeCount)&&(0xFFFFFFFF != TimeCount))
                {
                    MinuteSpeedCount = i;  
                                        MinuteSpeedCount++;
                }
            
            }
        }
                 
        if(MinuteSpeedCount > 15)
        {
            MinuteSpeedCount = 0;
        }
    }
}


/**************************************************************************
//函数名：VDRSpeed_SaveMinuteSpeed
//功能：保存每分钟平均速度
//输入：无
//输出：无
//返回值：无
//备注：保存的内容：4字节时间+1字节平均速度，保存到铁电中 
***************************************************************************/
static void VDRSpeed_SaveMinuteSpeed(u32 TimeCount, u8 MinuteSpeed)
{


    u16 Addr;
    u8  Buffer[6];

    
    if(MinuteSpeedCount < 16)
    {
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        Buffer[4] = MinuteSpeed;
        Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+MinuteSpeedCount*6;//乘以6是因为：5字节数据+1个字节的校验码
        FRAM_BufferWrite(Addr, Buffer, 5);
        MinuteSpeedCount++;
        if(MinuteSpeedCount >= 16)
        {
            MinuteSpeedCount = 0;
        }
        Buffer[0] = MinuteSpeedCount;
        Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_COUNT_ADDR;
        FRAM_BufferWrite(Addr, Buffer, 1);
    }
    else
    {
        MinuteSpeedCount = 0;
    }
}






