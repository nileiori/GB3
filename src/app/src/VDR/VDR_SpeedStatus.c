/************************************************************************
//�������ƣ�VDR_SpeedStatus.c
//���ܣ��ٶ�״̬��¼��
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2014.10
//�汾��¼��
//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
//#include "stm32f2xx_rtc.h"
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "Gps_App.h"
#include "Io.h"

/********************���ر���*************************/
static u8   VdrSpeedStatusBuffer[VDR_DATA_SPEED_STATUS_STEP_LEN];
static u8   VdrSpeedStatusEnableFlag = 0;//�ٶ�״̬ʹ�ܱ�־��1Ϊʹ�ܣ�0Ϊ��ֹ
static u8 VdrSpeedStatusFlag = 0;//�ٶ�״̬��־��1Ϊ�쳣��0Ϊ����
/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/
extern u8 SpeedFlag;
extern u8   GBTestFlag;//0:������������ģʽ;1:������ģʽ
extern u8  VdrDoubtSecondSpeedReadyFlag1;//VDR_SpeedStatus.cʹ��
/********************���غ�������*********************/
static void VDRSpeedStatus_Write(u8 SpeedStatus, u32 StartTime, u32 EndTime);
static void VDRSpeedStatus_Init(void);
static void VDRSpeedStatus_CheckEnableFlag(void);

/********************��������*************************/

/**************************************************************************
//��������VDRSpeedStatus_TimeTask
//���ܣ��ٶ�״̬��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��50ms����1�Σ������������Ҫ���ô˺���
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
                     &&(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION)))//��ֵ����11%
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
             VdrSpeedStatusFlag = 1;//�쳣
                       Addr = FRAM_VDR_SPEED_STATUS_ADDR;
                     FRAM_BufferWrite(Addr, &VdrSpeedStatusFlag, FRAM_VDR_SPEED_STATUS_LEN);
                   }
              }
              else if((GpsSpeed > 40)
                  &&(PulseSecondSpeed > 0)
                   &&(PulseInstantSpeed > 0)
                  &&((100*DifferenceSpeed) < (11*GpsSpeed))
                    &&(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION)))//��ֵС��11%
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
            VdrSpeedStatusFlag = 0;//����
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
//��������VDRSpeedStatus_GetFlag
//���ܣ���ȡ�ٶ�״̬��־
//���룺��
//�������
//����ֵ��0:����;1:�쳣
//��ע��
***************************************************************************/
u8 VDRSpeedStatus_GetFlag(void)
{
    
     return VdrSpeedStatusFlag;
 
}
/**************************************************************************
//��������VDRSpeedStatus_CheckEnableFlag
//���ܣ�����ٶ�״̬ʹ�ܱ�־
//���룺��
//�������
//����ֵ����
//��ע��
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
//��������VDRSpeedStatus_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ����
//��ע����Ҫ���ϵ���±���VdrSpeedStatusEnableFlag
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
//��������VDRSpeedStatus_Write
//���ܣ�����һ���ٶ�״̬��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
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

    VdrSpeedStatusBuffer[0] = SpeedStatus;//�ٶ�״̬��1�ֽڣ�0x01:����;0x02:�쳣

    Gmtime(&Time, StartTime);
    VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+1,&Time);//��ʼʱ��
                
    Gmtime(&Time, EndTime);
    VDRPub_ConvertTimeToBCD(VdrSpeedStatusBuffer+7,&Time);//����ʱ��
    
    VDRData_Write(VDR_DATA_TYPE_SPEED_STATUS, VdrSpeedStatusBuffer, VDR_DATA_SPEED_STEP_LEN-5, EndTime);


    VdrSpeedStatusEnableFlag = 0;
    Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
    FRAM_BufferWrite(Addr, &VdrSpeedStatusEnableFlag, FRAM_VDR_SPEED_STATUS_ENABLE_LEN);                
            
}






