#include "Rtc.h"
#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"

static  TIME_T  CurrentTime;

static u16 YearDay(s16 yy,s8 mm,s8 dd);
static u8 LeapYear(s16 year);
static void RTC_Configuration(void);


void RTC_Init(void)
{
    TIME_T tt;
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
        // RTC Configuration
    RTC_Configuration();

        //Write Backup Register when first enter
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
        
        // Set Time
        tt.year = 2015;
        tt.month = 1;
        tt.day = 1;
        tt.hour = 0;
        tt.min = 0;
        tt.sec = 0;
        RTC_SetTime(&tt);
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
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);

  // Clear reset flags
  RCC_ClearFlag();

}

void RTC_SetTime(TIME_T *tt)
{
    u32 RtcCounterValue;
    
    RtcCounterValue = ConverseGmtime(tt);
         
    RTC_SetCounter(RtcCounterValue);
}

void RTC_ReadTime(TIME_T *tt)
{
    tt->year = CurrentTime.year;
    tt->month = CurrentTime.month;
    tt->day = CurrentTime.day;
    tt->hour = CurrentTime.hour;
    tt->min = CurrentTime.min;
    tt->sec = CurrentTime.sec;
}

void RTC_UpdateTime(void)
{
    u32 RtcCounterValue;
        
    RtcCounterValue = RTC_GetCounter();
      
    Gmtime(&CurrentTime, RtcCounterValue);
}

static void RTC_Configuration(void)
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
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

u32 ConverseGmtime(TIME_T *tt)
{
    s16 i;
    s16 year;
    u32 TotalDay;
    u32 TotalSec;

    TotalDay = 0;
    TotalSec = 0;
    year = tt->year;

    if((year >= START_YEAR)&&(year <= END_YEAR))    
    {
        for(i = START_YEAR; i < tt->year; i++)  
        {
            TotalDay += (365+LeapYear(i));
      }
        TotalDay += YearDay(tt->year, tt->month, tt->day);  
        TotalSec = (TotalDay-1)*DAY_SECOND + tt->hour*HOUR_SECOND+ tt->min*60 + tt->sec;    
    }
    
    return TotalSec;
}

void Gmtime(TIME_T *tt, u32 counter)
{
    s16 i;
    u32 sum ;
    u32 temp;
    u32 counter1 = 0;
    u8 MonTab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

    if(counter < 0xBC191380)  /*2070*/
    {
        /* get the year */
        sum = 0;
        temp = 0;
        for(i=START_YEAR; sum<=counter;i++)   
        {
          sum += (LeapYear(i)*DAY_SECOND + YEAR_SECOND);
          if(sum <= counter)
          {
                temp= sum;
          }
        }
        tt->year = (i-1);   

        /* get the month */
        counter1= counter - temp;  
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
        counter1 = counter1 - temp; 
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
        counter1 = counter1 - temp; 
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
        counter1 = counter1 - temp; 
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
        tt->sec = counter1 - temp;  
    }   
    
}

static u8 LeapYear(s16 year)
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

static u16 YearDay(s16 yy,s8 mm,s8 dd)
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
