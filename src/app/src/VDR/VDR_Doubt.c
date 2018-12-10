/************************************************************************
//�������ƣ�VDR_Doubt.c
//���ܣ��ɵ����ݹ���
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
#include "fm25c160.h"
#include "EepromPram.h"
#include "Rtc.h"
#include "Public.h"
#include "Gps_App.h"
#include "Io.h"
#include "Adc.h"
#include "other.h"
/********************���ر���*************************/
static  u8  CarRunFlag = 0;//����ͣʻ����ʻ��־��1Ϊ��ʻ��0Ϊͣʻ��
static  u8  VdrDoubtInitFlag = 0;//��ʼ����־��1Ϊ�ѳ�ʼ����0Ϊδ��ʼ����
static  u8  VdrDoubtBuffer[VDR_DATA_DOUBT_STEP_LEN];//�ɵ����ݻ���
static  u16     VdrDoubtCount = 0;//�ɵ����ݵ�����¼
static  u8  VdrDoubtTestBuffer[300];//�ɵ����ݻ���
static  u8  VdrMyTestBuffer[300] = {0};
static u8 VdrDoubtLastPowerFlag = 0;//0Ϊ�ϵ磬1Ϊ�ϵ�
static u8   VdrDoubtPowerFlag = 0;
static u8 TestFlag1 = 0;
static TIME_T Flag1Time;
static u8 TestFlag2 = 0;
static TIME_T Flag2Time;
static u8 TestFlag3 = 0;
static TIME_T Flag3Time;
static u8 TestFlag4 = 0;
static TIME_T Flag4Time;

u32 TestTimeCount1 = 0;

/********************ȫ�ֱ���*************************/
u16 PowerShutValue = 50;//??5.0V
u8  VdrDoubtSecondSpeed = 0;
u8  VdrDoubtSecondSpeedReadyFlag1 = 0;//VDR_SpeedStatus.cʹ��
u8  VdrDoubtSecondSpeedReadyFlag2 = 0;//VDR_Speed.cʹ��
u8  VdrDoubtSecondSpeedReadyFlag3 = 0;


/********************�ⲿ����*************************/
extern u8   GBTestFlag;//0:������������ģʽ;1:������ģʽ���ñ�־ͨ���˵���ѡ��
extern u8  SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS,2�Զ�����,3�Զ�GPS
/********************���غ�������*********************/
static u16 VDRDoubt_Read1(u8 *pBuffer,u8 Type,u8 MoveStep,u8 NavigationFlag);
static u16 VDRDoubt_Read2(u8 *pBuffer,u8 Type,u8 NavigationFlag);
static void VDRDoubt_WritePositionNoChange(void);
static void VDRDoubt_WritePowerOff(void);
static void VDRDoubt_WritePowerOn(void);
static void VDRDoubt_WriteStop(void);
static void VDRDoubt_SaveCurrentSpeedStatus(void);
static void VDRDoubt_RunOrStop(void);
static void VDRDoubt_CalSecondSpeed(void);

/********************��������*************************/

/**************************************************************************
//��������VDRDoubt_TimeTask
//���ܣ�ʵ���ɵ����ݵļ�¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע���ɵ����ݶ�ʱ����50ms����1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDRDoubt_TimeTask(void)
{
    if(0 == VdrDoubtInitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_DOUBT))
        {
            if(0 == GBTestFlag)//������ʱʹ�ñ���
            {
              VDRDoubt_WritePowerOn();
            }
            VdrDoubtInitFlag = 1;
        }
    }
    else
    {
        VDRDoubt_RunOrStop();//��ʻ��ͣʻ���ж�
        
        VDRDoubt_CalSecondSpeed();//������ƽ���ٶ�

        VDRDoubt_SaveCurrentSpeedStatus();//��¼��ǰ���ٶȺ�״̬��0.2��һ��

        VDRDoubt_WriteStop();//��������ͣ�����ɵ�����

        VDRDoubt_WritePowerOff();//����ϵ���ɵ�����

        VDRDoubt_WritePositionNoChange();//������ʻ��λ���ޱ仯���ɵ�����
    }

    return ENABLE;      
}
/**************************************************************************
//��������VDRDoubt_GetCarRunFlag
//���ܣ���ȡ������ʻ��־
//���룺��
//�������
//����ֵ��1Ϊ��ʻ��0Ϊͣʻ
//��ע����ʻ�ٶȴ���0����10��Ϊ��ʻ���ٶȵ���0����10��Ϊͣʻ
***************************************************************************/
u8 VDRDoubt_GetCarRunFlag(void)
{
    return CarRunFlag;
}
/**************************************************************************
//��������VDRDoubt_GetSecondSpeed
//���ܣ���ȡÿ�����ٶ�
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
u8 VDRDoubt_GetSecondSpeed(void)
{
	#if(TACHOGRAPHS_19056_TEST)
		return Tachographs_Test_Get_Speed(); 
	#else
		return VdrDoubtSecondSpeed;
	#endif
}
/**************************************************************************
//��������VDRDoubt_RunOrStop
//���ܣ���ʻ��ͣʻ���ж�
//���룺��
//�������
//����ֵ����
//��ע���Ա��ر���CarRunFlag(ֵΪ1Ϊ��ʻ��Ϊ0Ϊͣʻ)��Ӱ��
***************************************************************************/
static void VDRDoubt_RunOrStop(void)
{
    static u16 RunCount = 0;
    static u16 StopCount = 0;
	
	  

    u8  Speed;

	  if(0x01 == (0x01&SpeedFlag))
		{
		    Speed = Gps_ReadSpeed();
		}
		else
		{
        Speed = VDRPulse_GetInstantSpeed();
		}

    if(Speed > 0)
    {
        StopCount = 0;
        RunCount++;
        if(RunCount >= 10*SECOND)
        {
            RunCount--;
            CarRunFlag = 1;
        }
    }
    else
    {
        RunCount = 0;
        StopCount++;
        if(StopCount >= 10*SECOND)
        {
            StopCount--;
            CarRunFlag = 0;
        }
    }
}
/**************************************************************************
//��������VDRDoubt_CalSecondSpeed
//���ܣ�����ÿ����ƽ���ٶ�
//���룺��
//�������
//����ֵ����
//��ע��ֻ���¹��ɵ���ٶ���˲ʱ�ٶȣ������루ƽ�����ٶȣ�ÿ����ƽ���ٶ�
//���������Ƶ�����������֮��Ĺ�ϵ�ǣ���˲ʱ�ٶ�--->�¹��ɵ��ٶ�--->���ٶ�--->
//ÿ����ƽ���ٶȣ�����ֵ��ע����ǣ�ֻ�����ٶȻ�����ٶȼ�����������ʹ�ã�
//���ҵ�ǰʱ��ʹ�õ��ٶ�ֵ��ǰһ���ǰһ���ӵ�ֵ
***************************************************************************/
static void VDRDoubt_CalSecondSpeed(void)
{
        static u8 Count1 = 0;
      static u8 Count2 = 0;
      static u8 SecondSpeed[10] = {0};
      static u32 LastTimeCount = 0;
        
        u32 TimeCount;
        u16 sum;
        u8  i;
        
        Count1++;
      if(Count1 >= (2*SYSTICK_0p1SECOND))
      {
          Count1 = 0;
				  if(0x01 == (SpeedFlag&0x01))
				  {
				      SecondSpeed[Count2++] = Gps_ReadSpeed();
				  }
				  else
				  {
              SecondSpeed[Count2++] = VDRPulse_GetInstantSpeed();
				  }
          if(Count2 > 5)
          {
            Count2 = 5;
          }
      }
        
        TimeCount = RTC_GetCounter();
    if(TimeCount != LastTimeCount)//��仯ʱ�̼������ٶ�
    { 
          sum = 0;
          for(i=0;i<Count2;i++)
          {
            sum += SecondSpeed[i];
          }
          VdrDoubtSecondSpeed = sum/Count2;
            VdrDoubtSecondSpeedReadyFlag1 = 1;
            VdrDoubtSecondSpeedReadyFlag2 = 1;
            VdrDoubtSecondSpeedReadyFlag3 = 1;
          for(i=0;i<5;i++)
          {
            SecondSpeed[i] = 0;
          }
          Count2 = 0;
          LastTimeCount = TimeCount;
     }
}
/**************************************************************************
//��������VDRDoubt_SaveCurrentSpeedStatus
//���ܣ����浱ǰ���ٶȺ�״̬
//���룺��
//�������
//����ֵ����
//��ע��ÿ��0.2��ȡһ��˲ʱ�ٶȣ��¹��ɵ����ݣ���ȡ5���ٶ����ȡƽ�����루ƽ�����ٶ�
//��������Ŀ�ģ�ͨ���¹��ɵ���ٶȼ�¼���Ƶ�����ƽ���ٶȣ�ʹ��ƽ���ٶ������ݣ�
//������ȡ��˲ʱ�ٶȣ�ֻ���¹��ɵ���ٶ���˲ʱ�ٶȣ�������ƽ���ٶȣ�ÿ����ƽ���ٶ�
//���������Ƶ�����������֮��Ĺ�ϵ�ǣ���˲ʱ�ٶ�--->�¹��ɵ��ٶ�--->���ٶ�--->
//ÿ����ƽ���ٶ�
***************************************************************************/
static void VDRDoubt_SaveCurrentSpeedStatus(void)
{
    static u8 Count1 = 0;

    

    u8  Status;
    u8  Buffer[10];
    
        u16     WriteAddr;
        u32 TimeCount;
        
    Count1++;
    if(Count1 >= (2*SYSTICK_0p1SECOND))
    {
        Count1 = 0;

        Status = 0;
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:ɲ��
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:��ת
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:��ת
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:Զ��
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:����
        //bit2-bit1δ����
        Status |= Io_ReadStatusBit(STATUS_BIT_ACC);//bit0:ACC

			  if(0x01 == (SpeedFlag&0x01))
				{
				    Buffer[0] = Gps_ReadSpeed();
				}
				else
				{
            Buffer[0] = VDRPulse_GetInstantSpeed();
				}

        Buffer[1] = Status;

        WriteAddr = FRAM_VDR_DOUBT_DATA_ADDR+3*VdrDoubtCount;
        FRAM_BufferWrite(WriteAddr,Buffer,2);//��¼�ٶ���״̬
        
        
        
        VdrDoubtTestBuffer[2*VdrDoubtCount] = Buffer[0];
        VdrDoubtTestBuffer[2*VdrDoubtCount+1] = Buffer[1];
        
        VdrDoubtCount++;
        if(VdrDoubtCount >= 150)
        {
            VdrDoubtCount = 0;
        }

        Public_ConvertShortToBuffer(VdrDoubtCount,Buffer);
        WriteAddr = FRAM_VDR_DOUBT_COUNT_ADDR;
        FRAM_BufferWrite(WriteAddr,Buffer,2);//��¼�ɵ����ݵ���

        
        //Count2++;
        //if(Count2 >= 5)//ÿ���¼һ��ʱ��
        //{
            //Count2 = 0;
            //VDRPub_ConvertNowTimeToBCD(Buffer);
            //WriteAddr = FRAM_VDR_DOUBT_TIME_ADDR;
            //FRAM_BufferWrite(WriteAddr,Buffer,6);//��¼��ǰʱ��
                        TimeCount = RTC_GetCounter();
                        Public_ConvertLongToBuffer(TimeCount,Buffer);
                FRAM_BufferWrite(FRAM_VDR_DOUBT_TIME_ADDR, Buffer, FRAM_VDR_DOUBT_TIME_LEN);
        //}
        
    }
    
    
}
/**************************************************************************
//��������VDRDoubt_WriteStop
//���ܣ�дͣ���¹��ɵ�
//���룺��
//�������
//����ֵ����
//��ע�����ɵ����ݴ�FRAM�������浽FLASH�У����浽FLASH���������ݷ��ϱ�A.22Ҫ��
***************************************************************************/
static void VDRDoubt_WriteStop(void)
{
    static u8 LastRunFlag = 0;//1Ϊ��ʻ��0Ϊͣʻ
    static u8 NavigationFlag = 0;//1Ϊ������0Ϊ������
    
    u8  RunFlag;
    u16 length;
    u32 TimeCount;

    
    RunFlag = VDRDoubt_GetCarRunFlag();

    if((1 == LastRunFlag)&&(0 == RunFlag)&&(0 == VdrDoubtPowerFlag))//����ʻ��Ϊͣʻ
    {
        //length = VDRDoubt_Read(VdrDoubtBuffer,50);    
    length = VDRDoubt_Read2(VdrDoubtBuffer,0x03,NavigationFlag);
    NavigationFlag = 0;     
        if(0 != length)
        {
            TimeCount = RTC_GetCounter();
            TimeCount -= 10;
            VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
            TestFlag1++;
            RTC_GetCurTime(&Flag1Time);
        }

    }
    else if((0 == LastRunFlag)&&(1 == RunFlag))//��ͣʻתΪ��ʻ
    {
      NavigationFlag = 0;
    }
    else if(1 == RunFlag)
    {
      if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
        {
          NavigationFlag = 1;
        }
    }
    LastRunFlag = RunFlag;
    
}
/**************************************************************************
//��������VDRDoubt_WritePowerOn
//���ܣ�д�ϵ��¹��ɵ�
//�������
//����ֵ����
//��ע�����ɵ����ݴ�FRAM�������浽FLASH�У����浽FLASH���������ݷ��ϱ�A.22Ҫ��
***************************************************************************/
static void VDRDoubt_WritePowerOn(void)
{
    u8  Buffer[10];
    u8  BufferLen;
    u16 i;
    u16 Addr;
    u16 length;
    TIME_T  Time;
    u32 TimeCount;

    BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//����һ�μ�¼�ĵ�λ��
    if(0 != BufferLen)
    {
        VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
        length = VDRDoubt_Read2(VdrDoubtBuffer,0x00,0); 
        if(0 != length)
        {
            Public_ConvertBcdToValue(Buffer, VdrDoubtBuffer, 6);
            Time.year = Buffer[0];
            Time.month = Buffer[1];
            Time.day = Buffer[2];
            Time.hour = Buffer[3];
            Time.min = Buffer[4];
            Time.sec = Buffer[5];
            if(1 == Public_CheckTimeStruct(&Time))
            {
                TimeCount = ConverseGmtime(&Time);
                VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
                TestFlag2++;
              RTC_GetCurTime(&Flag2Time);
            }
        }
        
    }
    else
    {
        
    }
    VdrDoubtCount = 0;
    Buffer[0] = 0;
    Buffer[1] = 0;
    for(i=0; i<150; i++)
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+3*i;
        FRAM_BufferWrite(Addr,Buffer,2);
        VdrDoubtTestBuffer[2*i] = Buffer[0];
        VdrDoubtTestBuffer[2*i+1] = Buffer[1];
        if((1 == VdrDoubtTestBuffer[2*i+1])||(1 == VdrDoubtTestBuffer[2*i]))
        {
            VdrDoubtTestBuffer[2*i+1] = 0;
            VdrDoubtTestBuffer[2*i] = 0;
        }
    }
}
/**************************************************************************
//��������VDRDoubt_WritePowerOff
//���ܣ�д�����¹��ɵ�
//���룺��
//�������
//����ֵ����
//��ע�����ɵ����ݴ�FRAM�������浽FLASH�У����浽FLASH���������ݷ��ϱ�A.22Ҫ��
***************************************************************************/
static void VDRDoubt_WritePowerOff(void)
{

    static u8 PowerOffCount = 0;
    static u8 PowerOnCount = 0;
    static u8 NavigationFlag = 0;
    
    u16 length;
    u16 Ad = 0;
    u32 Time;
    s16 ListNum;
    s16 i;
    u8  flag = 0;

    if(0 == VDRDoubt_GetCarRunFlag())//ͣʻ
    {
        return ;
    }

    Ad = Ad_GetValue(ADC_MAIN_POWER);//��ȡ�����ѹ
    Ad = Ad*33*9/0xfff;//ת����0.1VΪ��λ��ֵ

    if(Ad <= PowerShutValue)
    {
        PowerOnCount = 0;
        PowerOffCount++;
        if(PowerOffCount >= 4)
        {
            PowerOffCount = 0;
          VdrDoubtPowerFlag = 1;
        }
    }
    else
    {
        PowerOffCount = 0;
        PowerOnCount++;
        if(PowerOnCount >= 4)
        {
            PowerOnCount = 0;
          VdrDoubtPowerFlag = 0;
        }
    }

    if((0 == VdrDoubtLastPowerFlag)&&(1 == VdrDoubtPowerFlag))//���ϵ��Ϊ����
    {
        length = VDRDoubt_Read1(VdrDoubtBuffer,0x00,6,NavigationFlag);//��ǰŲ��6������Ϊ�ϵ�ʶ����ҪԼ1���ʱ��
        NavigationFlag = 0;
        if(0 != length)
        {
            Time = RTC_GetCounter();
            VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
            TestFlag3++;
            RTC_GetCurTime(&Flag3Time);
            //���ո�д�����������
            ListNum = VDRData_ReadTotalList(VDR_DATA_TYPE_DOUBT);
            ListNum--;//��0��ʼ������Ҫ--
            length = VDRData_ReadList(VdrMyTestBuffer,VDR_DATA_TYPE_DOUBT,ListNum);
            flag = 0;
            for(i=0; i<length; i++)
            {
              if((VdrMyTestBuffer[i] != VdrDoubtBuffer[i])||(0 == VdrMyTestBuffer[24]))
                {
                  flag = 1;
                    break;
                }
            }
            if(1 == flag)
            {
              flag++;
            }
        }
    }
    else if((1 == VdrDoubtLastPowerFlag)&&(0 == VdrDoubtPowerFlag))//�ɵ�����ϵ�
    {
      NavigationFlag = 0;
    }
    else
    {
      if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
      {
        NavigationFlag = 1;
      }
  }
    
    VdrDoubtLastPowerFlag = VdrDoubtPowerFlag;
}
/**************************************************************************
//��������VDRDoubt_WritePositionNoChange
//���ܣ�дλ����Ϣ�ޱ仯���¹��ɵ�����
//���룺��
//�������
//����ֵ����
//��ע�����ɵ����ݴ�FRAM�������浽FLASH�У����浽FLASH���������ݷ��ϱ�A.22Ҫ��
***************************************************************************/
static void VDRDoubt_WritePositionNoChange(void)
{
    static  u8      LastNoChangeFlag = 0;//0Ϊ�б仯��1Ϊû�б仯
    static  u16     Count = 0;
    static  GPS_STRUCT  LastPosition;//��һ�����λ����Ϣ

    u8  NoChangeFlag;
    u16 length;
    u32 TimeCount;
    GPS_STRUCT  Position;//��ǰ���λ����Ϣ

    if(0 == VDRDoubt_GetCarRunFlag())//ͣʻ
    {
        LastNoChangeFlag = 0;
        Count = 0;
        return ;
    }

    if(0 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))//����λ
  {
        Count = 0;
        LastNoChangeFlag = 0;
    return;
  }

    NoChangeFlag = 0;
    Gps_CopygPosition(&Position);//��ȡ��Чλ����Ϣ
    if(((Position.Latitue_D   == LastPosition.Latitue_D)
            &&(Position.Latitue_F  == LastPosition.Latitue_F))
            &&(Position.Latitue_FX == LastPosition.Latitue_FX))  
    {
        Count++;
        if(Count >= 10*SECOND)
        {
            Count = 0;
            NoChangeFlag = 1;
        }
    }
    else
    {
        Count = 0;
    }
  Gps_CopygPosition(&LastPosition);
    
    if((0 == LastNoChangeFlag)&&(1 == NoChangeFlag))//��λ����Ϣ�б仯��Ϊλ����Ϣ�ޱ仯
    {
        length = VDRDoubt_Read2(VdrDoubtBuffer,0x00,1); 
        if(0 != length)
        {
            TimeCount = RTC_GetCounter();
            VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
            TestFlag4++;
            RTC_GetCurTime(&Flag4Time);
        }
    LastNoChangeFlag = NoChangeFlag;
    }
    
    
}
/**************************************************************************
//��������VDRDoubt_Read2
//���ܣ����¹��ɵ�����
//���룺Type:bit0Ϊ1��ʾ��ǰʱ����Ҫ��10�룬Ϊ0��ʾ����Ҫ��10��;bit1Ϊ1��ʾ�¹��ɵ��ٶ���Ҫһ��Ϊ0�����ݣ�Ϊ0��ʾ����Ҫ����ȫΪ��0
//���룺MoveStep:��ǰ�ƶ��Ĳ���,0.2��1��
//�����pBuffer:ָ�����ݻ��壻
//����ֵ����ȡ�����ݳ���
//��ע���������ݷ��ϱ�A.22Ҫ��
***************************************************************************/
static u16 VDRDoubt_Read1(u8 *pBuffer,u8 Type,u8 MoveStep, u8 NavigationFlag)
{
    u8  *p = NULL;
    u8  Buffer[10];
    u8  BufferLen;
    u8  count;
    u8  flag;
    u16 Addr;
    u16 length;
    s16 i;
    s16 j;
        u32     TimeCount;
        TIME_T  Time;
    
    length = FRAM_BufferRead(Buffer,FRAM_VDR_DOUBT_TIME_LEN,FRAM_VDR_DOUBT_TIME_ADDR);
    if((0 == length)||(Type > 3))
    {
        return 0;
    }
        TimeCount = Public_ConvertBufferToLong(Buffer);
          if(TimeCount > 10)
                {
                    if(0x01 == (Type&0x01))
                    {
            TimeCount -= 10;
                    }
                }
        Gmtime(&Time, TimeCount);
        if(0 == Public_CheckTimeStruct(&Time))
        {
                return 0;
        }
        p = pBuffer;//����ʱ�䣬6�ֽڣ�BCD��,��Ҫ�����ʱ���ϼ�ȥ10��
        VDRPub_ConvertTimeToBCD(p, &Time);
    p += 6;

    length = EepromPram_ReadPram(E2_CURRENT_LICENSE_ID,p);//��ʻ֤���룬18�ֽڣ�ASCII��
    if(0 == length)
    {
        memset(p,'0',18);
    }
    p += 18;

        
    
    BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//����һ�μ�¼�ĵ�λ��
    if(0 != BufferLen)
    {
        VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
    }
    else
    {
        return 0;
    }
    i = VdrDoubtCount;
  i--;
    if(i < 0)
    {
        i = 150+i;
    }
    for(j=0; j<100; j++)//�ɵ����ݣ�200�ֽڣ�hexֵ
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(Buffer[0] > 0)
        {
                    break;//�ҵ����ٶȵ�1����Ϊ0�ĵ�
        }
        else
        {
                
        }
        i--;
        if(i < 0)
        {
            i = 150+i;
        }
    }
    if(0x02 == (Type&0x02))
    {
      i++;
    }
    else
    {
    
    }
    if(i >= 150)
    {
        i = 0;
    }
    
    if(i < MoveStep)
    {
      i = 150+i-MoveStep;
    }
    else
    {
      i = i-MoveStep;
    }
                
    count = 0;
    flag = 0;
    for(j=0; j<100; j++)//�ɵ����ݣ�200�ֽڣ�hexֵ
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(0 == length)
        {
            return 0;
        }
        if(Buffer[0] > 0)
        {
                count++;
                if(count >= 1)  //������ʻ��
                {
                    flag = 1;
                }
        }
        else
        {
                count = 0;
        }
        memcpy(p,Buffer,2);
        p += 2;

        i--;
        if(i < 0)
        {
            i = 150+i;
        }
    }

    if(0 == flag)
    {
        return 0;
    }
    
    //��Чλ�õ�
    //if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    if(1 == NavigationFlag)
    {
      Public_GetCurPositionInfoDataBlock(p);
    }
    else
    {
      VDRPub_GetNoNavigationPostion(p);
    }

    return 234;
    
}
/**************************************************************************
//��������VDRDoubt_Read2
//���ܣ����¹��ɵ�����
//���룺Type:bit0Ϊ1��ʾ��ǰʱ����Ҫ��10�룬Ϊ0��ʾ����Ҫ��10��;bit1Ϊ1��ʾ�¹��ɵ��ٶ���Ҫһ��Ϊ0�����ݣ�Ϊ0��ʾ����Ҫ����ȫΪ��0
//�����pBuffer:ָ�����ݻ��壻
//����ֵ����ȡ�����ݳ���
//��ע���������ݷ��ϱ�A.22Ҫ��
***************************************************************************/
static u16 VDRDoubt_Read2(u8 *pBuffer,u8 Type,u8 NavigationFlag)
{
    u8  *p = NULL;
    u8  Buffer[10];
    u8  BufferLen;
    u8  count;
    u8  flag;
    u16 Addr;
    u16 length;
    s16 i;
    s16 j;
        u32     TimeCount;
        TIME_T  Time;
    
    length = FRAM_BufferRead(Buffer,FRAM_VDR_DOUBT_TIME_LEN,FRAM_VDR_DOUBT_TIME_ADDR);
    if((0 == length)||(Type > 3))
    {
        return 0;
    }
        TimeCount = Public_ConvertBufferToLong(Buffer);
          if(TimeCount > 10)
                {
                    if(0x01 == (Type&0x01))
                    {
            TimeCount -= 10;
                    }
                }
        Gmtime(&Time, TimeCount);
        if(0 == Public_CheckTimeStruct(&Time))
        {
                return 0;
        }
        p = pBuffer;//����ʱ�䣬6�ֽڣ�BCD��,��Ҫ�����ʱ���ϼ�ȥ10��
        VDRPub_ConvertTimeToBCD(p, &Time);
    p += 6;

    length = EepromPram_ReadPram(E2_CURRENT_LICENSE_ID,p);//��ʻ֤���룬18�ֽڣ�ASCII��
    if(0 == length)
    {
        memset(p,'0',18);
    }
    p += 18;

        
    
    BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//����һ�μ�¼�ĵ�λ��
    if(0 != BufferLen)
    {
        VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
    }
    else
    {
        return 0;
    }
    i = VdrDoubtCount;
        i--;
    if(i < 0)
    {
        i = 149;
    }
    for(j=0; j<100; j++)//�ɵ����ݣ�200�ֽڣ�hexֵ
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(Buffer[0] > 0)
        {
                    break;//�ҵ����ٶȵ�1����Ϊ0�ĵ�
        }
        else
        {
                
        }
        i--;
        if(i < 0)
        {
            i = 149;
        }
    }
    if(0x02 == (Type&0x02))
    {
      i++;
    }
    else
    {
    
    }
  
    if(i >= 150)
    {
        i = 0;
    }
                
    count = 0;
    flag = 0;
    for(j=0; j<100; j++)//�ɵ����ݣ�200�ֽڣ�hexֵ
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(0 == length)
        {
            return 0;
        }
        if(Buffer[0] > 0)
        {
                count++;
                if(count >= 1)//������ʻ��
                {
                    flag = 1;
                }
        }
        else
        {
                count = 0;
        }
        memcpy(p,Buffer,2);
        p += 2;

        i--;
        if(i < 0)
        {
            i = 149;
        }
    }

    if(0 == flag)
    {
        return 0;
    }
    
    //��Чλ�õ�
    //if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    if(1 == NavigationFlag)
    {
      Public_GetCurPositionInfoDataBlock(p);
    }
    else
    {
      VDRPub_GetNoNavigationPostion(p);
    }

    return 234;
    
}






