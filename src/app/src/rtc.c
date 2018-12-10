/********************************************************************
//版权说明  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称  :realtime.c     
//功能      :实现实时时钟功能
//版本号    :V0.1
//开发人    :dxl
//开发时间  :2009.12
//修改者    :
//修改时间  :
//修改简要说明  :
//备注      :
***********************************************************************/

//********************************头文件************************************
#include "include.h"

//********************************自定义数据类型****************************

//********************************宏定义************************************
#define RTCClockOutput_Enable   1

//********************************全局变量**********************************
// u32 CurTimeID = 0;//????,?ID?????
TIME_T  CurTime;//????,????????
//static unsigned char s_ucRTCWeekDay;

//********************************外部变量**********************************
#ifdef REALTIME_SINGLE_TEST

extern  u8      UART4_RxStartCounter;    
extern  u8      UART4_RxStartFlag;
extern  u8  UART4_TxBuffer[];

#endif

//********************************本地变量**********************************

//********************************函数声明**********************************

/*********************************************************************
//函数名称  :InitRtc(void)
//功能      :初始化RTC模块
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
void InitRtc(void)
{
    TIME_T tt;

    
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA9A9)
  {
        // RTC Configuration
    RTC_Configuration();

        //Write Backup Register when first enter
    BKP_WriteBackupRegister(BKP_DR1, 0xA9A9);
        
        // Set Time
        tt.year = 15;
        tt.month = 1;
        tt.day = 1;
        tt.hour = 0;
        tt.min = 0;
        tt.sec = 0;
        SetRtc(&tt);
  }
    else
  {
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
            //Power On Reset
    }
    //Check if the Pin Reset flag is set
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
            //External Reset
    }
    }
    // Enable PWR and BKP clocks 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  // Allow access to BKP Domain 
  PWR_BackupAccessCmd(ENABLE);

  //Disable the Tamper Pin 
  BKP_TamperPinCmd(DISABLE); //To output RTCCLK/64 on Tamper pin, the tamper
                                // functionality must be disabled 

  // Enable RTC Clock Output on Tamper Pin 
  BKP_RTCOutputConfig(BKP_RTCOutputSource_None);

  // Clear reset flags
  RCC_ClearFlag();
    
}
/*********************************************************************
//函数名称  :RTC_PowerOnHandle(void)
//功能      :RTC上电启动处理
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :    
//备注      :注意该函数需要在完成RTC初始化和铁电初始化后调用
*********************************************************************/
void RTC_PowerOnHandle(void)
{
    TIME_T tt;
    u8  Buffer[5];
    u32 RtcCount;
    u32 Count;
    u32 temp;
    
  RtcCount = RTC_GetCounter();
    temp = RTC_GetCounter();
    
    if(FRAM_RTC_TIME_LEN == FRAM_BufferRead(Buffer, FRAM_RTC_TIME_LEN,FRAM_RTC_TIME_ADDR))
    {
      Count = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt, Count);
        if(1 == Public_CheckTimeStruct(&tt))
      {
        if(Count > RtcCount)
            {
              RtcCount = Count+1;//加1是为了确保当前时间总是比之前记录的时间要大
            }
      }
    }
    if(FRAM_RTC_TIME_BACKUP_LEN == FRAM_BufferRead(Buffer, FRAM_RTC_TIME_BACKUP_LEN,FRAM_RTC_TIME_BACKUP_ADDR))
    {
      Count = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt, Count);
        if(1 == Public_CheckTimeStruct(&tt))
      {
        if(Count > RtcCount)
            {
              RtcCount = Count+1;//加1是为了确保当前时间总是比之前记录的时间要大
            }
      }
    }
    if(FRAM_VDR_OVERTIME_END_TIME_LEN == FRAM_BufferRead(Buffer, FRAM_VDR_OVERTIME_END_TIME_LEN,FRAM_VDR_OVERTIME_END_TIME_ADDR))
    {
      Count = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt, Count);
        if(1 == Public_CheckTimeStruct(&tt))
      {
        if(Count > RtcCount)
            {
              RtcCount = Count+1;//加1是为了确保当前时间总是比之前记录的时间要大
            }
      }
    }
    if(RtcCount > temp)
    {
      RTC_SetCounter(RtcCount);
    }
}
/*********************************************************************
//函数名称  :RTC_Configuration(void)
//功能      :设置RTC
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :    
//备注      :
*********************************************************************/
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  
  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32768); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
/*********************************************************************
//函数名称  :SetRtc(TIME_T *tt)
//功能      :设置RTC计数器的值
//输入      :tt,时间
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
u32 RtcCounterValue;
void SetRtc(TIME_T *tt)
{
    
    //获得RTC计数值
    RtcCounterValue = ConverseGmtime(tt);
         
    //写入到RTC计数器
    RTC_SetCounter(RtcCounterValue);
        
    
}

/*********************************************************************
//函数名称  :ReadRtc(TIME_T *tt)
//功能      :读实时时钟
//输入      :
//输出      :读取的结果存放在结构体tt中
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :    
//备注      :
*********************************************************************/
void ReadRtc()
{
    //u32   RtcCounterValue;
        
    //读RTC计数器的值
    RtcCounterValue = RTC_GetCounter();

    //获得用年、月、日、时、分、秒表示的时间        
    Gmtime(&CurTime, RtcCounterValue);
    
//    CurTimeID = ConverseGmtime(&CurTime);
}
/*********************************************************************
//函数名称  :RTC_GetCurTime
//功能      :获取当前时间
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :       
//备注:
*********************************************************************/
void RTC_GetCurTime(TIME_T *tt)
{

        tt->year = CurTime.year;
        tt->month = CurTime.month;
        tt->day = CurTime.day;
        tt->hour = CurTime.hour;
        tt->min = CurTime.min;
        tt->sec = CurTime.sec;

}
/*********************************************************************
//函数名称  :LeapYear
//功能      :判断是否为闰年
//输入      :年份
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :是闰年返回1，不是闰年返回0
//备注      :
*********************************************************************/
u8 LeapYear(s16 year)
{
    if(((year%4 == 0)&&(year%100 !=0))||(year%400 == 0))
        {
        return 1;
        }
    else
        {
        return 0;
        }
}
/*********************************************************************
//函数名称  :YearDay
//功能      :输入年、月、日，计算是该年的第几天
//输入      :年、月、日
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :返回该年的第几天
//备注      :
*********************************************************************/
u16 YearDay(s16 yy,s8 mm,s8 dd)
{
    u8 i;
    u16 TotalDay = 0;
    u8 MonTab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    for(i = 1; i < mm; i++)
      {
           TotalDay += MonTab[i];
      }
    TotalDay += dd;
    if((LeapYear(yy) == LEAP_YEAR)&&(mm > 2))
        TotalDay++;
        return TotalDay;
}
/*********************************************************************
//函数名称  :ConverseGmtime
//功能      :把结构体类型时间转换成32位无符号的统一计数值
//输入      :TIME_T 结构体类型时间
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :32位无符号的统一计数值
//备注      :
*********************************************************************/
u32 ConverseGmtime(TIME_T *tt)
{
    s16 i;
    s8 year;
      s8 month;
      s8 day;
      s8 hour;
      s8 min;
      s8 sec;
    u32 TotalDay;
    u32 TotalSec;

    TotalDay = 0;
    TotalSec = 0;
    year = tt->year; 
      month = tt->month;
      day = tt->day;
      hour = tt->hour;
      min = tt->min;
      sec = tt->sec;

    if((year >= START_YEAR)&&(year <= END_YEAR))    //判断年是否符合范围
    {
        for(i = START_YEAR; i < tt->year; i++)  //计算year年之前的天数
        {
                    TotalDay += (365+LeapYear(i+2000));
            }
        //TotalDay += YearDay(tt->year, tt->month, tt->day);  //加上year当年的天数 dxl,2015.9,
                TotalDay += YearDay(year+2000, month, day);
        TotalSec = (TotalDay-1)*DAY_SECOND + hour*HOUR_SECOND+ min*60 + sec;    //计算总的秒数
    }

    return TotalSec;
    
}
/*********************************************************************
//函数名称  :Gmtime
//功能      :把32位无符号的统一计数值转换成结构体类型时间
//输入      :指向TIME_T类型的指针，计数值
//输出      :指向TIME_T类型的指针
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
void Gmtime(TIME_T *tt, u32 counter)
{
    s16 i;
    u32 sum ;
    u32 temp;
    u32 counter1 = 0;
    u8 MonTab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

    if(counter < 0xBC191380)  /* 年份不大于2070*/
    {
    /* get the year */
    sum = 0;
    temp = 0;
    for(i=START_YEAR; sum<=counter;i++)   /* 计算年份 */
        {
          sum += (LeapYear(i+2000)*DAY_SECOND + YEAR_SECOND);
          if(sum <= counter)
            {
                      temp= sum;
               }
        }
    tt->year = (i-1);  
                
    /* get the month */
    counter1= counter - temp;  /*计算年以后剩余时间*/
    sum = 0;
    temp = 0;
    for(i=1; sum<=counter1;i++)
        {
                  sum += MonTab[i]*DAY_SECOND;
             if((2 == i)&&(LeapYear(tt->year)))
                 {
                       sum +=  DAY_SECOND;  
             }
             if(sum <= counter1)
                {
                       temp = sum;
            }
           }
    tt->month = (i-1);

    /* get the day */
    counter1 = counter1 - temp; /*计算月后剩余的时间*/
    sum = 0;
    temp = 0;
    for(i=1; sum <= counter1; i++)
        {
                  sum += DAY_SECOND;
             if(sum <= counter1)
                {
                        temp = sum;
            }
           }
    tt->day = (i-1);

    /* get the hour */
    counter1 = counter1 - temp; /*计算日后剩余的时间*/
    sum = 0;
    temp = 0;
    for(i=0; sum <= counter1; i++)
        {
                 sum += HOUR_SECOND;
            if(sum <= counter1)
                {
                       temp = sum;
            }
           }
    tt->hour = (i-1);

    /* get the minute */
    counter1 = counter1 - temp; /*计算小时后剩余的时间 */
    sum = 0;
    temp = 0;
    for(i=0; sum <= counter1; i++)
        {
                 sum += MIN_SECOND;
            if(sum <= counter1)
                {
                       temp = sum;
            }
           }
    tt->min = (i-1);

    /* get the second */
    tt->sec = counter1 - temp;  /*计算分钟后剩余的时间 */
        }   
    
}
/*********************************************************************
//函数名称  :
//功能      :
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :       
//备注:
*********************************************************************/
void Num2NumStr(u32 Num, u8 *NumStr)
{
    u8 *p;
    u8 NumChar[10];
    u8 i;
        p = NumChar;
    
    if(0 == Num)
    {
        *NumStr = 0;
        return ;
    }

        *p = Num/1000000000 + 48;
        p++;
        *p = Num%1000000000/100000000 + 48;
        p++;
        *p = Num%100000000/10000000 + 48;
        p++;
        *p = Num%10000000/1000000 + 48;
        p++;
        *p = Num%1000000/100000 + 48;
        p++;
        *p = Num%100000/10000 + 48;
        p++;
        *p = Num%10000/1000 + 48;
        p++;
        *p = Num%1000/100 + 48;
        p++;
        *p = Num%100/10 + 48;
        p++;
        *p = Num%10 + 48;

    p = NumStr;
    i =0;
    while(NumChar[i] == '0')
    {
        i++;
    }
        if(10 == i)  //当Num值为0时
        {
          *p = '0';
          p++;
          *p = '0';
          p++;
          *p = '\0';
        }
        else if(9 == i) //当Num值小于10时
        {
          *p = '0';
          p++;
          *p = NumChar[i];
          p++;
          *p = '\0';
        }
        else //当Num值大于等于10时
        {
      for(; i<10; i++,p++)
      {
        *p = NumChar[i];
      }
    *p = '\0';
        }
       
}
/*********************************************************************
//函数名称  :SendTimeToUsart(TIME_T *tt, u8 *pBuffer)
//功能      :把时间输出到串口
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :    
//备注      :
*********************************************************************/

/*********************************************************************
*********************************************************************/
ErrorStatus CheckTimeStruct(TIME_T *tt)
{

    if((tt->year > END_YEAR)||(tt->year < START_YEAR)||
    (tt->month > 12)||(tt->month < 1)||
    (tt->day > 31)||(tt->day < 1)||
    (tt->hour > 23)||(tt->hour < 0)||
    (tt->min > 59)||(tt->min < 0)||
    (tt->sec > 59)||(tt->sec < 0))
    {
        return ERROR;
    }
    else
    {
        return SUCCESS;
    }
}


#ifdef RTC_SINGLE_TEST
void SendTimeToUsart(TIME_T *tt, u8 *pBuffer)
{
    TimeStruct2TimeStructStr(tt, pBuffer);
    strcpy(UART4_TxBuffer,pBuffer);
    USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
    Delay_nms(20);
}

#endif
