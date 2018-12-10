/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :realtime.c     
//����      :ʵ��ʵʱʱ�ӹ���
//�汾��    :V0.1
//������    :dxl
//����ʱ��  :2009.12
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/

//********************************ͷ�ļ�************************************
#include "include.h"

//********************************�Զ�����������****************************

//********************************�궨��************************************
#define RTCClockOutput_Enable   1

//********************************ȫ�ֱ���**********************************
// u32 CurTimeID = 0;//????,?ID?????
TIME_T  CurTime;//????,????????
//static unsigned char s_ucRTCWeekDay;

//********************************�ⲿ����**********************************
#ifdef REALTIME_SINGLE_TEST

extern  u8      UART4_RxStartCounter;    
extern  u8      UART4_RxStartFlag;
extern  u8  UART4_TxBuffer[];

#endif

//********************************���ر���**********************************

//********************************��������**********************************

/*********************************************************************
//��������  :InitRtc(void)
//����      :��ʼ��RTCģ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
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
//��������  :RTC_PowerOnHandle(void)
//����      :RTC�ϵ���������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :    
//��ע      :ע��ú�����Ҫ�����RTC��ʼ���������ʼ�������
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
              RtcCount = Count+1;//��1��Ϊ��ȷ����ǰʱ�����Ǳ�֮ǰ��¼��ʱ��Ҫ��
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
              RtcCount = Count+1;//��1��Ϊ��ȷ����ǰʱ�����Ǳ�֮ǰ��¼��ʱ��Ҫ��
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
              RtcCount = Count+1;//��1��Ϊ��ȷ����ǰʱ�����Ǳ�֮ǰ��¼��ʱ��Ҫ��
            }
      }
    }
    if(RtcCount > temp)
    {
      RTC_SetCounter(RtcCount);
    }
}
/*********************************************************************
//��������  :RTC_Configuration(void)
//����      :����RTC
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :    
//��ע      :
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
//��������  :SetRtc(TIME_T *tt)
//����      :����RTC��������ֵ
//����      :tt,ʱ��
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 RtcCounterValue;
void SetRtc(TIME_T *tt)
{
    
    //���RTC����ֵ
    RtcCounterValue = ConverseGmtime(tt);
         
    //д�뵽RTC������
    RTC_SetCounter(RtcCounterValue);
        
    
}

/*********************************************************************
//��������  :ReadRtc(TIME_T *tt)
//����      :��ʵʱʱ��
//����      :
//���      :��ȡ�Ľ������ڽṹ��tt��
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :    
//��ע      :
*********************************************************************/
void ReadRtc()
{
    //u32   RtcCounterValue;
        
    //��RTC��������ֵ
    RtcCounterValue = RTC_GetCounter();

    //������ꡢ�¡��ա�ʱ���֡����ʾ��ʱ��        
    Gmtime(&CurTime, RtcCounterValue);
    
//    CurTimeID = ConverseGmtime(&CurTime);
}
/*********************************************************************
//��������  :RTC_GetCurTime
//����      :��ȡ��ǰʱ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :       
//��ע:
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
//��������  :LeapYear
//����      :�ж��Ƿ�Ϊ����
//����      :���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :�����귵��1���������귵��0
//��ע      :
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
//��������  :YearDay
//����      :�����ꡢ�¡��գ������Ǹ���ĵڼ���
//����      :�ꡢ�¡���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :���ظ���ĵڼ���
//��ע      :
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
//��������  :ConverseGmtime
//����      :�ѽṹ������ʱ��ת����32λ�޷��ŵ�ͳһ����ֵ
//����      :TIME_T �ṹ������ʱ��
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :32λ�޷��ŵ�ͳһ����ֵ
//��ע      :
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

    if((year >= START_YEAR)&&(year <= END_YEAR))    //�ж����Ƿ���Ϸ�Χ
    {
        for(i = START_YEAR; i < tt->year; i++)  //����year��֮ǰ������
        {
                    TotalDay += (365+LeapYear(i+2000));
            }
        //TotalDay += YearDay(tt->year, tt->month, tt->day);  //����year��������� dxl,2015.9,
                TotalDay += YearDay(year+2000, month, day);
        TotalSec = (TotalDay-1)*DAY_SECOND + hour*HOUR_SECOND+ min*60 + sec;    //�����ܵ�����
    }

    return TotalSec;
    
}
/*********************************************************************
//��������  :Gmtime
//����      :��32λ�޷��ŵ�ͳһ����ֵת���ɽṹ������ʱ��
//����      :ָ��TIME_T���͵�ָ�룬����ֵ
//���      :ָ��TIME_T���͵�ָ��
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gmtime(TIME_T *tt, u32 counter)
{
    s16 i;
    u32 sum ;
    u32 temp;
    u32 counter1 = 0;
    u8 MonTab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

    if(counter < 0xBC191380)  /* ��ݲ�����2070*/
    {
    /* get the year */
    sum = 0;
    temp = 0;
    for(i=START_YEAR; sum<=counter;i++)   /* ������� */
        {
          sum += (LeapYear(i+2000)*DAY_SECOND + YEAR_SECOND);
          if(sum <= counter)
            {
                      temp= sum;
               }
        }
    tt->year = (i-1);  
                
    /* get the month */
    counter1= counter - temp;  /*�������Ժ�ʣ��ʱ��*/
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
    counter1 = counter1 - temp; /*�����º�ʣ���ʱ��*/
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
    counter1 = counter1 - temp; /*�����պ�ʣ���ʱ��*/
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
    counter1 = counter1 - temp; /*����Сʱ��ʣ���ʱ�� */
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
    tt->sec = counter1 - temp;  /*������Ӻ�ʣ���ʱ�� */
        }   
    
}
/*********************************************************************
//��������  :
//����      :
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :       
//��ע:
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
        if(10 == i)  //��NumֵΪ0ʱ
        {
          *p = '0';
          p++;
          *p = '0';
          p++;
          *p = '\0';
        }
        else if(9 == i) //��NumֵС��10ʱ
        {
          *p = '0';
          p++;
          *p = NumChar[i];
          p++;
          *p = '\0';
        }
        else //��Numֵ���ڵ���10ʱ
        {
      for(; i<10; i++,p++)
      {
        *p = NumChar[i];
      }
    *p = '\0';
        }
       
}
/*********************************************************************
//��������  :SendTimeToUsart(TIME_T *tt, u8 *pBuffer)
//����      :��ʱ�����������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :    
//��ע      :
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
