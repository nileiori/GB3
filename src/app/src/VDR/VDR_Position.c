/************************************************************************
//程序名称：VDR_Position.c
//功能：360h位置信息记录
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
#include "Rtc.h"
#include "spi_flash.h"
#include "Public.h"
#include "Io.h"
#include "spi_flashapi.h"

/********************本地变量*************************/
static u8   VdrPositionBuffer[VDR_DATA_POSITION_STEP_LEN] = {0};

/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static void VDRPosition_Init(void);
static void VDRPosition_SaveStartTime(void);
static void VDRPosition_SavePosition(u8 RunFlag,u8 Offset,u8 NavigationFlag);
static void VDRPosition_Write(void);
/********************函数定义*************************/

/**************************************************************************
//函数名：VDRPosition_TimeTask
//功能：360h位置信息记录，每分钟1条位置信息，按小时记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
/*2015.12.15
FunctionalState VDRPosition_TimeTask(void)
{
    static  u32 LastTimeCount;
    static u8   AlreadyRunFlag = 0;//1为已行驶过，0为未行驶过
    static u8   InitFlag = 0;
    static u8 WriteEnableFlag = 0;
    static u8 WriteDelayCount = 0;
    static  TIME_T  LastTime;
  static u8 NavigationFlag = 0;
    
    u32 CurrentTimeCount;
    u8  RunFlag;
        u8      Offset;
    TIME_T  CurrentTime;

    if(0 == InitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_POSITION))
        {
            VDRPosition_Init();
            InitFlag = 1;
        }
    }
    else
    {
        CurrentTimeCount = RTC_GetCounter();
        if(CurrentTimeCount != LastTimeCount)//秒钟动了
        {
              if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
                {
                  NavigationFlag = 1;
                }
                RunFlag = VDRDoubt_GetCarRunFlag();
                if(1 == RunFlag)
                {
                    AlreadyRunFlag = 1;
                }
                RTC_GetCurTime(&CurrentTime);
                if(CurrentTime.min != LastTime.min)
                {
                    if(1 == AlreadyRunFlag)//加入这个限定避免整个小时都没有行驶的情况也会记录一个开始时间
                    {
                      VDRPosition_SaveStartTime();
                    }
                    WriteEnableFlag = 1;
                    WriteDelayCount = 0;
                                        
                }
                if(1 == WriteEnableFlag)
                {
                  WriteDelayCount++;
                    if(WriteDelayCount >= 9)//在下一分钟的第9秒写入上一分钟的平均速度
                    {
                      if(0 == CurrentTime.min)//跨小时
            {
              Offset = 59;
              VDRPosition_SavePosition(AlreadyRunFlag,Offset,NavigationFlag);
              VDRPosition_Write();
                            NavigationFlag = 0;
            }
            else
            {
              Offset = CurrentTime.min-1; 
              VDRPosition_SavePosition(AlreadyRunFlag,Offset,NavigationFlag);
                            NavigationFlag = 0;
            }
                      WriteEnableFlag = 0;
                      AlreadyRunFlag = 0;
                    }
                }
                LastTime.year = CurrentTime.year;
                LastTime.month = CurrentTime.month;
                LastTime.day = CurrentTime.day;
                LastTime.hour = CurrentTime.hour;
                LastTime.min = CurrentTime.min;
                LastTime.sec = CurrentTime.sec;
        }
        LastTimeCount = CurrentTimeCount;
    }

    return ENABLE;      
}
*/
FunctionalState VDRPosition_TimeTask(void)
{
    static  u32 LastTimeCount;
    static u8   InitFlag = 0;
    static u8 WriteEnableFlag = 0;
    static u8 WriteDelayCount = 0;
    static  TIME_T  LastTime;
  static u8 LastMinuteNavigationFlag = 0;
    static u8 CurrentMinuteNavigationFlag = 0;
    static u8 AlreadyRunFlag = 0;
    
    u32 CurrentTimeCount;
  u8      Offset;
    TIME_T  CurrentTime;

    if(0 == InitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_POSITION)) 
        {
            VDRPosition_Init();
            InitFlag = 1;
        }
    }
    else
    {
        CurrentTimeCount = RTC_GetCounter();
        if(CurrentTimeCount != LastTimeCount)//秒钟动了
        {
              if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
                {
                  CurrentMinuteNavigationFlag = 1;
                }
                if(1 == VDRDoubt_GetCarRunFlag())
                {
                  AlreadyRunFlag = 1;
                }
                RTC_GetCurTime(&CurrentTime);
                if(59 == CurrentTime.sec)
                {
                  LastMinuteNavigationFlag =    CurrentMinuteNavigationFlag;            
                  CurrentMinuteNavigationFlag = 0;
                }
                if(CurrentTime.min != LastTime.min)
                {
                    WriteDelayCount = 0;  
          WriteEnableFlag = 1;                  
                }
                  WriteDelayCount++;
                  if((WriteDelayCount >= 9)&&(1 == WriteEnableFlag))//9秒
                    {
                        
                        if((1 == LastMinuteNavigationFlag)||(1 == AlreadyRunFlag))
                        {
                          VDRPosition_SaveStartTime();
                        }
                      if(0 == CurrentTime.min)//跨小时
            {
              Offset = 59;
              VDRPosition_SavePosition(AlreadyRunFlag,Offset,LastMinuteNavigationFlag);
              VDRPosition_Write();
            }
            else
            {
              Offset = CurrentTime.min-1; 
              VDRPosition_SavePosition(AlreadyRunFlag,Offset,LastMinuteNavigationFlag);
            }
                        WriteEnableFlag = 0;
                        AlreadyRunFlag = 0;
                    }
            
                LastTime.year = CurrentTime.year;
                LastTime.month = CurrentTime.month;
                LastTime.day = CurrentTime.day;
                LastTime.hour = CurrentTime.hour;
                LastTime.min = CurrentTime.min;
                LastTime.sec = CurrentTime.sec;
        }
        LastTimeCount = CurrentTimeCount;
    }

    return ENABLE;      
}
/**************************************************************************
//函数名：VDRPosition_GetCurHourPosition
//功能：获取当前这个小时的位置信息记录
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
u16 VDRPosition_GetCurHourPosition(u8 *pBuffer)
{
    u32 Addr;
    u8  Buffer[20];
  u8      *p = NULL;
  u16     i;
    u16 length = 0;
    TIME_T  Time;
    
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
    sFLASH_ReadBuffer(Buffer, Addr, 6);         
    VDRPub_ConvertBCDToTime(&Time,Buffer);
    if(1 == Public_CheckTimeStruct(&Time))
    {           
        Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
        sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//读取暂存的数据
    p = VdrPositionBuffer;
    for(i=0;i<60;i++)
    {
      if(*(p+6+i*11+10) != 0xff)
      {
        break;
      }
    }
    if(60 != i)//判断是否全部是0xFF
    {
           memcpy(pBuffer,p,666);
             length = 666;
    }
        else
        {
          length = 0;
        }
    }
    else
    {
      length = 0;
    }
    
    return length;
}
/**************************************************************************
//函数名：VDRPosition_Get15MinuteSpeed
//功能：获取停车前15分钟速度
//输入：无
//输出：无
//返回值：无
//备注：长度为0表示没有读取到正确的值，长度为51（6字节+3字节*15）表示读取了正确的值
//数据排列顺序是：
//停车时间（6字节）
//停车前1分钟的数据（3字节，1字节小时(BCD码)，1字节分钟(BCD码)，1字节平均速度(hex)
//停车前2分钟的数据（3字节，1字节小时，1字节分钟，1字节平均速度，均为hex值）
//停车前3分钟的数据（3字节，1字节小时，1字节分钟，1字节平均速度，均为hex值）
//......
***************************************************************************/
u16 VDRPosition_Get15MinuteSpeed(u8 *pBuffer)
{
      u32   Addr;
      u8    Buffer[20];
    u8      *p = NULL;
      u8   temp;
    s16     i,j;
      u16 ListNum;
      u16 length;
      u32 TimeCount;
      TIME_T    Time;
    
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
        sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//读取暂存的数据
    p = VdrPositionBuffer+6;
    for(i=59;i>=0;i--)
    {
      if(*(p+i*11+10) != 0xff)
      {
        break;
      }
    }
    if(i >= 0)//当前小时有数据
    {
           length = FRAM_BufferRead(Buffer,FRAM_VDR_SPEED_STOP_TIME_LEN,FRAM_VDR_SPEED_STOP_TIME_ADDR);
             if(4 == length)
             {
                 p = pBuffer;
               TimeCount = Public_ConvertBufferToLong(Buffer);
                 if(TimeCount >= 10)//正常停车会延时10秒判断
                 {
                   TimeCount -= 10;
                 }
                 Gmtime(&Time, TimeCount);
                 VDRPub_ConvertTimeToBCD(p,&Time);
         p += 6;
                 TimeCount -= Time.sec;
                 for(j=0;j<15;j++)
                 {
                     Gmtime(&Time, TimeCount);
                     *p++ = VDRPub_HEX2BCD(Time.hour);
               *p++ = VDRPub_HEX2BCD(Time.min);
                   if(i >= 0)
                     {
                       temp = VdrPositionBuffer[6+i*11+10];
                         if(0xff == temp)
                         {
                           *p++ = 0;
                         }
                         else
                         {
                           *p++ = temp;
                         }
                     }
                     else
                     {
                       *p++ = 0;
                     }
                     i--;
                     TimeCount -= 60;
                 }
             }
             else
             {
               return 0;
             }
    }
        else//当前小时没有数据
        {
            ListNum = VDRData_ReadTotalList(VDR_DATA_TYPE_POSITION);
            if(ListNum > 0)
            {
              ListNum--;
            VDRData_ReadList(VdrPositionBuffer,VDR_DATA_TYPE_POSITION,ListNum);
                p = VdrPositionBuffer+6;
        for(i=59;i>=0;i--)
        {
          if(*(p+i*11+10) != 0xff)
          {
            break;
          }
        }
                length = FRAM_BufferRead(Buffer,FRAM_VDR_SPEED_STOP_TIME_LEN,FRAM_VDR_SPEED_STOP_TIME_ADDR);
             if(4 == length)
             {
                 p = pBuffer;
               TimeCount = Public_ConvertBufferToLong(Buffer);
                 if(TimeCount >= 10)//正常停车会延时10秒判断
                 {
                   TimeCount -= 10;
                 }
                 Gmtime(&Time, TimeCount);
                 VDRPub_ConvertTimeToBCD(p,&Time);
         p += 6;
                 TimeCount -= Time.sec;
                 for(j=0;j<15;j++)
                 {
                     Gmtime(&Time, TimeCount);
                     *p++ = VDRPub_HEX2BCD(Time.hour);
               *p++ = VDRPub_HEX2BCD(Time.min);
                   if(i >= 0)
                     {
                       temp = VdrPositionBuffer[6+i*11+10];
                         if(0xff == temp)
                         {
                           *p++ = 0;
                         }
                         else
                         {
                           *p++ = temp;
                         }
                     }
                     else
                     {
                       *p++ = 0;
                     }
                     i--;
                     TimeCount -= 60;
                 }
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
        
        return 51;
}
/**************************************************************************
//函数名：VDRPosition_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：0:未完成初始化，1:已完成初始化
//备注：上电后会检测位置信息暂存区，有必要会转移至记录区
***************************************************************************/
static void VDRPosition_Init(void)
{
    u32 StartTimeCount;
    u32 CurrentTimeCount;
    u32 Addr;
    u16 i;
    u8  flag;
    TIME_T  StartTime;
    TIME_T  CurrentTime;

    
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
    sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//位置信息块长度是666
    CurrentTimeCount = RTC_GetCounter();
    Gmtime(&CurrentTime, CurrentTimeCount);
    VDRPub_ConvertBCDToTime(&StartTime,VdrPositionBuffer);

        if((StartTime.month == CurrentTime.month)&&(StartTime.day == CurrentTime.day)&&(StartTime.hour == CurrentTime.hour))
        {
                //上次掉电和本次上电是属于同一个小时内，继续存储即可
        }
        else if(StartTime.hour != CurrentTime.hour)
        {
            flag = 0;
            for(i=0; i<666; i++)//前面6个字节是时间
        {
            if(VdrPositionBuffer[i] != 0xff)
            {
                flag = 1;
                break;
            }
        }
        if(1 == flag)
        {
                StartTimeCount = ConverseGmtime(&StartTime);
              VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, StartTimeCount);//保存到记录区
            }
            
            sFLASH_EraseSector(Addr);//擦除扇区
        }
            
}
/**************************************************************************
//函数名：VDRPosition_SaveStartTime
//功能：暂存位置信息记录的开始时间
//输入：无
//输出：无
//返回值：无
//备注：若已保存过，则不再写入，若保存的时间出错，则将该扇区擦除
***************************************************************************/
static void VDRPosition_SaveStartTime(void)
{
    u32 Addr;
    u8  Buffer[20];
    u8  i;
    u8  flag;
    TIME_T  Time;
    TIME_T  StartTime;
    
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
    sFLASH_ReadBuffer(Buffer, Addr, 6);
    flag = 0;
    for(i=0; i<6; i++)
    {
        if(0xff != Buffer[i])
        {
                flag = 1;
                break;
        }
    }
    
    if(0 == flag)
    {
        RTC_GetCurTime(&Time);
        if(1 == Public_CheckTimeStruct(&Time))
        {
            Time.min = 0;
            Time.sec = 0;
            VDRPub_ConvertTimeToBCD(Buffer,&Time);
            sFLASH_WriteBuffer(Buffer, Addr, 6);
        }
    }
    else
    {
        if(1 == VDRPub_ConvertBCDToTime(&StartTime,Buffer))
        {
            if(1 == Public_CheckTimeStruct(&StartTime))
            {
                                    //写入的时间正确
            }
            else
            {
                    sFLASH_EraseSector(Addr);//擦除扇区
            }
        }
        else
        {
            sFLASH_EraseSector(Addr);//擦除扇区
        }
    }
}
/**************************************************************************
//函数名：VDRPosition_SavePosition
//功能：暂存一条位置信息
//输入：无
//输出：无
//返回值：无
//备注：每分钟1条位置信息，信息内容见标准A.20
***************************************************************************/
static void VDRPosition_SavePosition(u8 RunFlag,u8 Offset,u8 NavigationFlag)
{
    u32 Addr;
    u8  flag;
    u8  i;
    u8  Buffer[20];
    
    Addr = 6+FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES+11*Offset;
    sFLASH_ReadBuffer(Buffer, Addr, 11);
    flag = 0;
    for(i=0; i<11; i++)
    {
        if(0xff != Buffer[i])
        {
            flag = 1;
            break;
        }
    }
    if(((0 == flag)&&(1 == RunFlag))||((0 == flag)&&(1 == NavigationFlag)))
    {
        if(1 == NavigationFlag)
        {
            Public_GetCurPositionInfoDataBlock(Buffer);//10字节
            Buffer[10] = VDRSpeed_GetMinuteSpeed();
        }
        else
        {
            VDRPub_GetNoNavigationPostion(Buffer);
            Buffer[10] = VDRSpeed_GetMinuteSpeed();
        }
        sFLASH_WriteBuffer(Buffer, Addr, 11);
    }
}
/**************************************************************************
//函数名：VDRPosition_Write
//功能:写位置信息
//输入：无
//输出：无
//返回值：无
//备注：每分钟1条位置信息，信息内容见标准A.20
***************************************************************************/
static void VDRPosition_Write(void)
{
    u32 Addr;
    u32 TimeCount;
    u8  Buffer[20];
    
    TIME_T  Time;
    
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
    sFLASH_ReadBuffer(Buffer, Addr, 6);
                    
    VDRPub_ConvertBCDToTime(&Time,Buffer);
    if(1 == Public_CheckTimeStruct(&Time))
    {
                        
        TimeCount = ConverseGmtime(&Time);
        Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
        sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//读取暂存的数据
        VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, TimeCount);//保存到记录区
             
    }
    sFLASH_EraseSector(Addr);//擦除扇区
}






