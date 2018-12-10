/************************************************************************
//�������ƣ�VDR_Speed.c
//���ܣ�48h��ʻ�ٶȼ�¼
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
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "MileMeter.h"
#include "Gps_App.h"
#include "Io.h"

/********************���ر���*************************/
static u8   CurrentSecondSpeed[VDR_DATA_SPEED_STEP_LEN];//��ǰ���ӵ�ÿ�����ٶȺ�״̬����
//static u8 LastSecondSpeed[VDR_DATA_SPEED_STEP_LEN];//��һ���ӵ�ÿ�����ٶȺ�״̬���壬ǰ6�ֽ�Ϊ��ʼʱ��
static u8   MinuteSpeed = 0;//ÿ����ƽ���ٶ�
static s8   MinuteSpeedCount = 0;//��¼��λ��
u32 TestTimeCount2 = 0;

/********************ȫ�ֱ���*************************/

/********************�ⲿ����*************************/
extern u8   SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS,2�Զ�����,3�Զ�GPS
extern u8  VdrDoubtSecondSpeedReadyFlag2;//��ÿ����ƽ���ٶ�ʹ��
extern u8  GBTestFlag;//0:������������ģʽ;1:������ģʽ
/********************���غ�������*********************/
static void VDRSpeed_Init(void);
static void VDRSpeed_SaveMinuteSpeed(u32 TimeCount, u8 MinuteSpeed);

/********************��������*************************/
/**************************************************************************
//��������VDRSpeed_TimeTask
//���ܣ�48h��ʻ�ٶȼ�¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��λ�û㱨��ʱ����50ms����1�Σ������������Ҫ���ô˺���
***************************************************************************/
/* dxl,2015.10.30,�м���ж�ʧ���ٶȵ����
FunctionalState VDRSpeed_TimeTask(void)
{
    static u32  LastTimeCount = 0;
    static u8   AlreadyRunFlag = 0;//1Ϊ����ʻ����0Ϊδ��ʻ��
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

        if(CurrentTimeCount > LastTimeCount)//��ʱ�Ӷ���
        {
            j = CurrentTimeCount-LastTimeCount;
            RTC_GetCurTime(&CurrentTime);
            //if(1 == Public_CheckTimeStruct(&CurrentTime))
            //{
                Speed = VDRPulse_GetSecondSpeed();//�ٶ�

                Status = 0;//״̬
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:ɲ��
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:��ת
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:��ת
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:Զ��
                Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:����
                //bit2-bit1δ����
                Status |= Io_ReadStatusBit(STATUS_BIT_ACC);//bit0:ACC
            
                if(1 == j)
                {
                  CurrentSecondSpeed[2*CurrentTime.sec] = Speed;
                  CurrentSecondSpeed[2*CurrentTime.sec+1] = Status;
                }
                else if(j > 1)//dxl,2015.10.30,��ʱRTC���ܴ���ż����������
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
                if((1 == LastRunFlag)&&(0 == RunFlag))//����ʻ��Ϊͣʻ����¼ͣ��ʱ��
                {
                    temp = CurrentTimeCount;
                    temp -= 2;//ʵ�ʲ��Է��ָ�ʱ����¹��ɵ��ͣ��ʱ����1-2��,��˼�2
                    Public_ConvertLongToBuffer(temp,Buffer);
                    Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;
                    FRAM_BufferWrite(Addr, Buffer, 4);
                }

                if(59 == CurrentTime.sec)//���һ��������ݴ�
                {
                    VDRPub_ConvertNowTimeToBCD(LastSecondSpeed);//6�ֽ�ʱ��
                    LastSecondSpeed[5] = 0;//��ǿ��Ϊ0
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
                else if(9 == CurrentTime.sec)//ÿ���ڵ�9���ж��Ƿ�Ҫ����һ��������д�뵽flash
                {
                    if(1 == AlreadyRunFlag)//����ʻ��
                    {
                        sum = 0;
                        for(i=0; i<60; i++)
                        {
                            if(0 != LastSecondSpeed[6+2*i])
                            {
                                sum++;
                            }
                        }
                        //if(sum > 0)Ҳ���ܳ���ֻ��ʻ�˼��룬ÿ����ƽ���ٶȻ���Ϊ0�����
                        //{
                            TimeCount = CurrentTimeCount;
                            TimeCount -= 69;
                            VDRData_Write(VDR_DATA_TYPE_SPEED, LastSecondSpeed, VDR_DATA_SPEED_STEP_LEN-5, TimeCount);//��¼ÿ���ӵ���ʻ�ٶȺ�״̬
                            
                            VDRSpeed_SaveMinuteSpeed(TimeCount, MinuteSpeed);//��¼ͣ��ǰ15�����ٶ�
                        //}

                    }
                    AlreadyRunFlag = 0;
                }

                LastRunFlag = RunFlag;
            //}
            //else//ʱ�����
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
    //static u8 AlreadyRunFlag = 0;//1Ϊ����ʻ����0Ϊδ��ʻ��
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
                  SpeedCount--;//��ǰʱ��ʹ�õ���ƽ���ٶ���ʵ����һ����ٶȣ�������1
                }
              InitFlag = 1;
            }
        }
    }
    else
    {
        CurrentTimeCount = RTC_GetCounter();

        if(CurrentTimeCount != LastTimeCount)//��ʱ�Ӷ���
        {
            if(CurrentTimeCount != (LastTimeCount+1))//Уʱ
            {
                if(CurrentTimeCount < LastTimeCount)
                {
                  for(i=0; i<60; i++)
                {
                    CurrentSecondSpeed[6+2*i] = 0x00;   //�ٶ�Ϊ0   
            CurrentSecondSpeed[6+2*i+1] = 0x00; //δʹ�õ�bit2��Ϊ1         
                }
              }
                else
                {
                   RunFlag = VDRDoubt_GetCarRunFlag();
                     if(1 == RunFlag)//��ʻ������
                     {
                       if(CurrentTimeCount >= (LastTimeCount+60))
                         {
                           for(i=0; i<60; i++)
                     {
                         CurrentSecondSpeed[6+2*i] = 0x00;  //�ٶ�Ϊ0   
                 CurrentSecondSpeed[6+2*i+1] = 0x00;    //δʹ�õ�bit2��Ϊ1         
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
                       CurrentSecondSpeed[6+2*i] = 0x00;    //�ٶ�Ϊ0   
               CurrentSecondSpeed[6+2*i+1] = 0x00;  //δʹ�õ�bit2��Ϊ1         
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
                  SpeedCount--;//��ǰʱ��ʹ�õ���ƽ���ٶ���ʵ����һ����ٶȣ�������1
                }
              return ENABLE;
            }
            
            RunFlag = VDRDoubt_GetCarRunFlag();
            if((1 == LastRunFlag)&&(0 == RunFlag))//����ʻ��Ϊͣʻ����¼ͣ��ʱ��
            {
                    temp = CurrentTimeCount;
                    temp -= 2;//ʵ�ʲ��Է��ָ�ʱ����¹��ɵ��ͣ��ʱ����1-2��,��˼�2
                    Public_ConvertLongToBuffer(temp,Buffer);
                    Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;
                    FRAM_BufferWrite(Addr, Buffer, 4);
            }
            LastRunFlag = RunFlag;
                
            RTC_GetCurTime(&CurrentTime);
            
            if((1 == Public_CheckTimeStruct(&CurrentTime))&&(1 == VdrDoubtSecondSpeedReadyFlag2))
            {
                VdrDoubtSecondSpeedReadyFlag2 = 0;
                
              Speed = VDRSpeed_GetCurSpeed();//�ٶ�

              Status = 0;//״̬
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:ɲ��
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:��ת
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:��ת
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:Զ��
              Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:����
              //bit2-bit1δ����
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
                        
                        //VDRPub_ConvertNowTimeToBCD(CurrentSecondSpeed);//6�ֽ�ʱ��
                        Gmtime(&Time, TimeCount);
                        VDRPub_ConvertTimeToBCD(CurrentSecondSpeed, &Time);
                  CurrentSecondSpeed[5] = 0;//��ǿ��Ϊ0
                        
                    VDRData_Write(VDR_DATA_TYPE_SPEED, CurrentSecondSpeed, VDR_DATA_SPEED_STEP_LEN-5, TimeCount);//��¼ÿ���ӵ���ʻ�ٶȺ�״̬
                    
                    VDRSpeed_SaveMinuteSpeed(TimeCount, MinuteSpeed);//��¼ͣ��ǰ15�����ٶ�
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
//��������VDRSpeed_GetMinuteSpeed
//���ܣ���ȡÿ����ƽ���ٶ�
//���룺��
//�������
//����ֵ��ÿ����ƽ���ٶ�
//��ע����
***************************************************************************/
u8 VDRSpeed_GetMinuteSpeed(void)
{
    return MinuteSpeed;
}
/**************************************************************************
//��������VDRSpeed_Get15MinuteSpeed
//���ܣ���ȡͣ��ǰ15ÿ�����ٶ�
//���룺��
//�������
//����ֵ�����ݳ���
//��ע������Ϊ0��ʾû�ж�ȡ����ȷ��ֵ������Ϊ51��6�ֽ�+3�ֽ�*15����ʾ��ȡ����ȷ��ֵ
//��������˳���ǣ�
//ͣ��ʱ�䣨6�ֽڣ�
//ͣ��ǰ1���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ(BCD��)��1�ֽڷ���(BCD��)��1�ֽ�ƽ���ٶ�(hex)
//ͣ��ǰ2���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
//ͣ��ǰ3���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
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
                
            Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;//��ͣ��ʱ��
            BufferLen = FRAM_BufferRead(Buffer, 4, Addr);
            if(0 == BufferLen)
            {
                Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+count*6;//6 = 4�ֽ�ʱ��+1�ֽ�ƽ���ٶ�+1�ֽ�У��
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
    length = 0;//������ʼ��
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
              //����ͣ���Ƿ��ӵ�����
            //}
            //else if((0xFFFFFFFF == MinuteTimeCount)||(0 == MinuteTimeCount))//����������δд��ֵ
            if((0xFFFFFFFF == MinuteTimeCount)||(0 == MinuteTimeCount))//����������δд��ֵ
            {
                *p++ = 0;
                length++;
            }
            else if(MinuteTimeCount >= LastMinuteTimeCount)//��ǰ����ʱ�������һ���������Ǵ���
            {
               
            }
            else
            {
                k = (LastMinuteTimeCount - MinuteTimeCount)/60;
                if(k >= 1)
                {
                  k--;//��Ҫ���0�ĸ���
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
        StopTimeCount -= 10;//����10��Ϊ0����Ϊͣʻ
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
** ��������: VDRSpeed_GetCurSpeed
** ��������: ��ȡ��ǰ�ٶ�(�ѿ������ٶ����ͣ��������ж��ٶ�������)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
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
        //��ȡ�ٶ�ѡ��
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
** ��������: VDRSpeed_GetCurSpeedType
** ��������: ��ȡ�ٶ�����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0Ϊ����,1ΪGPS
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char VDRSpeed_GetCurSpeedType(void)
{
    u8  flag;
    
    flag = 0x01 & SpeedFlag;
    
    return flag;
}
/**************************************************************************
//��������VDRSpeed_UpdateParameter
//���ܣ����²���
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDRSpeed_UpdateParameter(void)
{
        u8  PramLen;
        
        //��ȡ�ٶ�ѡ��ֵ
    PramLen = EepromPram_ReadPram(E2_SPEED_SELECT_ID, &SpeedFlag);
        if(1 == PramLen)
        {
    
          //�����������
          if(0 == (SpeedFlag&0x01))//dxl,2015.5.11
          {
            VDRPulse_UpdateParameter();
          }
              else
              {
                      //ΪGPS�ٶ�
                      GpsMile_UpdatePram();
                  MileMeter_ClearOldPoint();
              }
        }
        else
        {
        //����ΪGPS�ٶ�
                SpeedFlag = 0x03;//ΪGPS�ٶ�//dxl,2015.5.11
                GpsMile_UpdatePram();
            MileMeter_ClearOldPoint();
        }
}
////////////////////�������ڲ�����/////////////////////////
/**************************************************************************
//��������VDRSpeed_Init
//���ܣ��ϵ��ʼ��
//���룺��
//�������
//����ֵ����
//��ע��
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
//��������VDRSpeed_SaveMinuteSpeed
//���ܣ�����ÿ����ƽ���ٶ�
//���룺��
//�������
//����ֵ����
//��ע����������ݣ�4�ֽ�ʱ��+1�ֽ�ƽ���ٶȣ����浽������ 
***************************************************************************/
static void VDRSpeed_SaveMinuteSpeed(u32 TimeCount, u8 MinuteSpeed)
{


    u16 Addr;
    u8  Buffer[6];

    
    if(MinuteSpeedCount < 16)
    {
        Public_ConvertLongToBuffer(TimeCount,Buffer);
        Buffer[4] = MinuteSpeed;
        Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+MinuteSpeedCount*6;//����6����Ϊ��5�ֽ�����+1���ֽڵ�У����
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






