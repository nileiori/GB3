/************************************************************************
//�������ƣ�VDR_Position.c
//���ܣ�360hλ����Ϣ��¼
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
//#include "stm32f2xx_rtc.h"
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "Rtc.h"
#include "spi_flash.h"
#include "Public.h"
#include "Io.h"
#include "spi_flashapi.h"

/********************���ر���*************************/
static u8   VdrPositionBuffer[VDR_DATA_POSITION_STEP_LEN] = {0};

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static void VDRPosition_Init(void);
static void VDRPosition_SaveStartTime(void);
static void VDRPosition_SavePosition(u8 RunFlag,u8 Offset,u8 NavigationFlag);
static void VDRPosition_Write(void);
/********************��������*************************/

/**************************************************************************
//��������VDRPosition_TimeTask
//���ܣ�360hλ����Ϣ��¼��ÿ����1��λ����Ϣ����Сʱ��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��50ms����1�Σ������������Ҫ���ô˺���
***************************************************************************/
/*2015.12.15
FunctionalState VDRPosition_TimeTask(void)
{
    static  u32 LastTimeCount;
    static u8   AlreadyRunFlag = 0;//1Ϊ����ʻ����0Ϊδ��ʻ��
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
        if(CurrentTimeCount != LastTimeCount)//���Ӷ���
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
                    if(1 == AlreadyRunFlag)//��������޶���������Сʱ��û����ʻ�����Ҳ���¼һ����ʼʱ��
                    {
                      VDRPosition_SaveStartTime();
                    }
                    WriteEnableFlag = 1;
                    WriteDelayCount = 0;
                                        
                }
                if(1 == WriteEnableFlag)
                {
                  WriteDelayCount++;
                    if(WriteDelayCount >= 9)//����һ���ӵĵ�9��д����һ���ӵ�ƽ���ٶ�
                    {
                      if(0 == CurrentTime.min)//��Сʱ
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
        if(CurrentTimeCount != LastTimeCount)//���Ӷ���
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
                  if((WriteDelayCount >= 9)&&(1 == WriteEnableFlag))//9��
                    {
                        
                        if((1 == LastMinuteNavigationFlag)||(1 == AlreadyRunFlag))
                        {
                          VDRPosition_SaveStartTime();
                        }
                      if(0 == CurrentTime.min)//��Сʱ
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
//��������VDRPosition_GetCurHourPosition
//���ܣ���ȡ��ǰ���Сʱ��λ����Ϣ��¼
//���룺��
//�������
//����ֵ����
//��ע��
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
        sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//��ȡ�ݴ������
    p = VdrPositionBuffer;
    for(i=0;i<60;i++)
    {
      if(*(p+6+i*11+10) != 0xff)
      {
        break;
      }
    }
    if(60 != i)//�ж��Ƿ�ȫ����0xFF
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
//��������VDRPosition_Get15MinuteSpeed
//���ܣ���ȡͣ��ǰ15�����ٶ�
//���룺��
//�������
//����ֵ����
//��ע������Ϊ0��ʾû�ж�ȡ����ȷ��ֵ������Ϊ51��6�ֽ�+3�ֽ�*15����ʾ��ȡ����ȷ��ֵ
//��������˳���ǣ�
//ͣ��ʱ�䣨6�ֽڣ�
//ͣ��ǰ1���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ(BCD��)��1�ֽڷ���(BCD��)��1�ֽ�ƽ���ٶ�(hex)
//ͣ��ǰ2���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
//ͣ��ǰ3���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
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
        sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//��ȡ�ݴ������
    p = VdrPositionBuffer+6;
    for(i=59;i>=0;i--)
    {
      if(*(p+i*11+10) != 0xff)
      {
        break;
      }
    }
    if(i >= 0)//��ǰСʱ������
    {
           length = FRAM_BufferRead(Buffer,FRAM_VDR_SPEED_STOP_TIME_LEN,FRAM_VDR_SPEED_STOP_TIME_ADDR);
             if(4 == length)
             {
                 p = pBuffer;
               TimeCount = Public_ConvertBufferToLong(Buffer);
                 if(TimeCount >= 10)//����ͣ������ʱ10���ж�
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
        else//��ǰСʱû������
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
                 if(TimeCount >= 10)//����ͣ������ʱ10���ж�
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
//��������VDRPosition_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ��0:δ��ɳ�ʼ����1:����ɳ�ʼ��
//��ע���ϵ�����λ����Ϣ�ݴ������б�Ҫ��ת������¼��
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
    sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//λ����Ϣ�鳤����666
    CurrentTimeCount = RTC_GetCounter();
    Gmtime(&CurrentTime, CurrentTimeCount);
    VDRPub_ConvertBCDToTime(&StartTime,VdrPositionBuffer);

        if((StartTime.month == CurrentTime.month)&&(StartTime.day == CurrentTime.day)&&(StartTime.hour == CurrentTime.hour))
        {
                //�ϴε���ͱ����ϵ�������ͬһ��Сʱ�ڣ������洢����
        }
        else if(StartTime.hour != CurrentTime.hour)
        {
            flag = 0;
            for(i=0; i<666; i++)//ǰ��6���ֽ���ʱ��
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
              VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, StartTimeCount);//���浽��¼��
            }
            
            sFLASH_EraseSector(Addr);//��������
        }
            
}
/**************************************************************************
//��������VDRPosition_SaveStartTime
//���ܣ��ݴ�λ����Ϣ��¼�Ŀ�ʼʱ��
//���룺��
//�������
//����ֵ����
//��ע�����ѱ����������д�룬�������ʱ������򽫸���������
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
                                    //д���ʱ����ȷ
            }
            else
            {
                    sFLASH_EraseSector(Addr);//��������
            }
        }
        else
        {
            sFLASH_EraseSector(Addr);//��������
        }
    }
}
/**************************************************************************
//��������VDRPosition_SavePosition
//���ܣ��ݴ�һ��λ����Ϣ
//���룺��
//�������
//����ֵ����
//��ע��ÿ����1��λ����Ϣ����Ϣ���ݼ���׼A.20
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
            Public_GetCurPositionInfoDataBlock(Buffer);//10�ֽ�
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
//��������VDRPosition_Write
//����:дλ����Ϣ
//���룺��
//�������
//����ֵ����
//��ע��ÿ����1��λ����Ϣ����Ϣ���ݼ���׼A.20
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
        sFLASH_ReadBuffer(VdrPositionBuffer, Addr, 666);//��ȡ�ݴ������
        VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, TimeCount);//���浽��¼��
             
    }
    sFLASH_EraseSector(Addr);//��������
}






