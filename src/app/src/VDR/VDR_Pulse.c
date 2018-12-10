/************************************************************************
//�������ƣ�VDR_Pulse.c
//���ܣ�����ģ�鴦�����������ۼ���ʻ��̡�����˲ʱ�ٶȡ��������ٶȵļ��㡣
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1���ۼ���ʻ��̣�������������ķ�ʽ��ÿ���յ�һ�������źţ���ʻ����ۼ�һ��
//����˲ʱ�ٶȣ�ÿ�������źſ��Լ����Ӧ��һ���ٶ�ֵ��ÿ5�������źŶ�Ӧ5���ٶ�ֵ�������ֵ����Сֵȥ������ƽ���õ�˲ʱ�ٶ�
//�������ٶȣ�ÿ��50ms��ȡһ��˲ʱ�ٶȣ�����20��˲ʱ�ٶȵ�ƽ��ֵ�õ����ٶ�
//ÿ����ƽ���ٶȣ�ÿ���Ӷ�ȡһ�����ٶȣ�����60�����ٶȵ�ƽ��ֵ�õ�ÿ����ƽ���ٶ�
*************************************************************************/

/********************�ļ�����*************************/
//#include "stm32f2xx_tim.h"
//#include "stm32f2xx_exti.h"
//#include "stm32f2xx_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "Gps_App.h"
#include "turnspeed.h"
#include "Io.h"
#include "other.h"

/********************���ر���*************************/
static void VDRPulse_InitTim3(void);
static void VDRPulse_InitExti10(void);

/********************ȫ�ֱ���*************************/
u32 PulseTotalMile = 0;//�ۼ���ʻ��̣��������,��λ��0.01����
u32 PulseCoef = 0;//����ϵ��


/********************�ⲿ����*************************/
extern u8  GBTestFlag;//0:������������ģʽ;1:������ģʽ
extern  u8  SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS
extern  u32 VdrCoefficientPulseCount;//�����������1�����壬������1�����յ�PulseCoef������ʱ��ʾ����1����
extern  u8  VdrDocimasyStatus;//����״̬;E1H,�������������;E2H,��������ϵ��������;E3H,����ʵʱʱ��������

/********************���غ�������*********************/
static  u16 PulseWidthMin = 0;//�����ȼ�����Сֵ��С�ڸ�ֵ��Ϊ�Ǹ�������
static  u16 PulseWidth[VDR_INSTANT_SPEED_ARRAY_SIZE+1] = {0};
static  u8  PulseWidthCount = 0;

static  u32 PulseCount = 0;//�����������1�����壬������1�����յ�PulseCoef������ʱ��ʾ����1����

static  u32     PulseExtiCount = 0;//�ⲿ�жϽ����������

static  u8  PulseInstantSpeed = 0;//����˲ʱ�ٶȣ�km/h
static  u8  LastPulseInstantSpeed = 0;//��һ�������ٶ�
static  u8      PulseSecondSpeed = 0;//����ÿ��ƽ���ٶ�

/********************��������*************************/
/*********************************************************************
//��������  :VDRPulse_TimeTask
//����      :���嶨ʱ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :���Ƿ���ENABLE
//��ע      :50ms�����1�Σ���ʱ��¼��1�룩�ۼ���ʻ��̣�������ƽ���ٶ�
*********************************************************************/
/*dxl,2015.12.17
FunctionalState  VDRPulse_TimeTask(void)
{
  static  u8    InitFlag = 0;
  static  u16   Count = 0;
  static  u32   LasePulseTotalMile = 0;//��һ���ۼ���ʻ��̣��������,��λ��0.01����
  static  u32   LastTimeCount = 0;
  static  u8      SecondSpeedData[20] = {0};//50ms��ȡһ��˲ʱ�ٶȣ�20��˲ʱ�ٶȵ�ƽ��ֵΪÿ��ƽ���ٶ�
  static  u8      SecondSpeedCount = 0;
     
  u8  TmpData[5];
  u8  i;
  u16 sum;
  u32 TimeCount;
  TIME_T CurTime;
        
  if(0 == InitFlag)
  {
    InitFlag = 1;
    VDRPulse_Init();
    VDRPulse_UpdateParameter();
    return ENABLE;
  }
        
  if(0 == (SpeedFlag&0x01))//�����ٶ�
  {
    Count++;
    if(Count >= 20)//1��дһ�����,50ms����һ��
    {
      Count = 0;
      if(LasePulseTotalMile != PulseTotalMile)
      {
           Public_ConvertLongToBuffer(PulseTotalMile,TmpData);
           FRAM_BufferWrite(FRAM_VDR_MILEAGE_ADDR, TmpData, FRAM_VDR_MILEAGE_LEN); //�洢�����
         FRAM_BufferWrite(FRAM_VDR_MILEAGE_BACKUP_ADDR, TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN); //�洢�����
      } 
    } 
    RTC_GetCurTime(&CurTime);
    TimeCount = RTC_GetCounter();
    if(TimeCount != LastTimeCount)//��仯ʱ�̼������ٶ�
    {  
      if(SecondSpeedCount >= 20)
      {
        SecondSpeedCount = 19;
      }
      SecondSpeedData[SecondSpeedCount++] = VDRPulse_GetInstantSpeed();
      sum = 0;
      for(i=0; i<SecondSpeedCount; i++)
      {
        sum += SecondSpeedData[i];
      }
      PulseSecondSpeed = sum/SecondSpeedCount;
                        
      SecondSpeedCount = 0;
      for(i=0; i<20; i++)
      {
    SecondSpeedData[i] = 0;
      }
      LastTimeCount = TimeCount;
    }
    else
    {
      if(SecondSpeedCount >= 20)
      {
        SecondSpeedCount = 19;
      }
      SecondSpeedData[SecondSpeedCount++] = VDRPulse_GetInstantSpeed();
    }       
  }
  return ENABLE;
}
*/
FunctionalState  VDRPulse_TimeTask(void)
{
  static  u8    InitFlag = 0;
  static  u16   Count = 0;
  static  u32   LasePulseTotalMile = 0;//��һ���ۼ���ʻ��̣��������,��λ��0.01����
  static  u32   LastTimeCount = 0;
  static  u8      SecondSpeedData[20] = {0};//50ms��ȡһ��˲ʱ�ٶȣ�20��˲ʱ�ٶȵ�ƽ��ֵΪÿ��ƽ���ٶ�
  static  u8      SecondSpeedCount = 0;
    static  u8  Count2 = 0;
     
  u8  TmpData[5];
  u8  i;
  u16 sum;
  u32 TimeCount;
  TIME_T CurTime;
        
  if(0 == InitFlag)
  {
    InitFlag = 1;
    VDRPulse_Init();
    VDRPulse_UpdateParameter();
    return ENABLE;
  }
        
  if(0 == (SpeedFlag&0x01))//�����ٶ�
  {
    Count++;
    if(Count >= 20)//1��дһ�����,50ms����һ��
    {
      Count = 0;
      if(LasePulseTotalMile != PulseTotalMile)
      {
           Public_ConvertLongToBuffer(PulseTotalMile,TmpData);
           FRAM_BufferWrite(FRAM_VDR_MILEAGE_ADDR, TmpData, FRAM_VDR_MILEAGE_LEN); //�洢�����
         FRAM_BufferWrite(FRAM_VDR_MILEAGE_BACKUP_ADDR, TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN); //�洢�����
      } 
    } 
    RTC_GetCurTime(&CurTime);
    TimeCount = RTC_GetCounter();
    if(TimeCount != LastTimeCount)//��仯ʱ�̼������ٶ�
    {  
      if(SecondSpeedCount >= 6)
      {
        SecondSpeedCount = 5;
      }
      SecondSpeedData[SecondSpeedCount++] = VDRPulse_GetInstantSpeed();
      sum = 0;
      for(i=0; i<SecondSpeedCount; i++)
      {
        sum += SecondSpeedData[i];
      }
      PulseSecondSpeed = sum/SecondSpeedCount;
                        
      SecondSpeedCount = 0;
      for(i=0; i<20; i++)
      {
          SecondSpeedData[i] = 0;
      }
      LastTimeCount = TimeCount;
    }
    else
    {
            Count2++;
      if(Count2 >= 4)//0.2��ȡһ��˲ʱ�ٶ�
      {
        Count2 = 0;
                SecondSpeedData[SecondSpeedCount++] = VDRPulse_GetInstantSpeed();
      }
    }       
  }
  return ENABLE;
}
/*********************************************************************
//��������  :VDRPulse_UpdateParameter
//����      :���²���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��������ϵ�����ۼ���ʻ���
//      :������ϵ������Ϊ3600��7200ʱ�ᴥ������ϵ���Զ�У׼���ܣ�������ʱ���ܿ���
*********************************************************************/
void VDRPulse_UpdateParameter(void)
{
    u8  TmpData[4] = {0};
    u8  PramLen = 0;
    u32 temp;

    //��������ϵ��
    PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, TmpData);
    if(E2_CAR_FEATURE_COEF_LEN != PramLen)
    {
            PulseCoef = 7200;//Ĭ��Ϊ7200��ȡ�����ٶȴ�����ϵ��
    }
    else
    {
            PulseCoef = 0;
            PulseCoef |= (TmpData[1] << 8);//���ֽ�
            PulseCoef |= TmpData[2];//���ֽ�λ
    }

    //������������С��ֵ
    temp = (3600*(u32)1000000)/(50*PulseCoef*255);
    PulseWidthMin = temp&0xffff;

    //�����ۼ���ʻ���
    PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_LEN, FRAM_VDR_MILEAGE_ADDR);
    if(FRAM_VDR_MILEAGE_LEN == PramLen)//У�����ȷ
    {
        PulseTotalMile = 0;
        PulseTotalMile |= TmpData[0] << 24; //���ֽ�
        PulseTotalMile |= TmpData[1] << 16; //�θ��ֽ�
        PulseTotalMile |= TmpData[2] << 8;  //���ֽ�
        PulseTotalMile |= TmpData[3];       //���ֽ�
    }
        else
        {
                //�����ݵ��ۼ���ʻ���
            PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN, FRAM_VDR_MILEAGE_BACKUP_ADDR);
            if(FRAM_VDR_MILEAGE_BACKUP_LEN == PramLen)//У�����ȷ
            {
                PulseTotalMile = 0;
                PulseTotalMile |= TmpData[0] << 24; //���ֽ�
                PulseTotalMile |= TmpData[1] << 16; //�θ��ֽ�
                PulseTotalMile |= TmpData[2] << 8;  //���ֽ�
                PulseTotalMile |= TmpData[3];       //���ֽ�
            }
                else
                {
                        PulseTotalMile = 0;
                }
        }
}
/*********************************************************************
//��������  :VDRPulse_GetInstantSpeed
//����      :��ȡ����˲ʱ�ٶ�,��λkm/h
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :���ٶ��쳣ʱ���������ٶ���GPS�ٶȴ���
//      :
*********************************************************************/
u8 VDRPulse_GetInstantSpeed(void)
{
	  if(1 == GBTestFlag)
		{
        if((1 == VDRSpeedStatus_GetFlag())&&(0 == PulseInstantSpeed))
        {
            return Gps_ReadSpeed();
        }
        else
        {
            return PulseInstantSpeed;
        }
		}
		else
		{
        return PulseInstantSpeed;
		}
}
/*********************************************************************
//��������  :VDRPulse_GetPulseSpeed
//����      :��ȡ����˲ʱ�ٶ�,��λkm/h
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��VDRPulse_GetInstantSpeed��ȣ�VDRPulse_GetPulseSpeed���ص��Ǵ�����
//      :������GPS��������
*********************************************************************/
u8 VDRPulse_GetPulseSpeed(void)
{
	#if(TACHOGRAPHS_19056_TEST)
		return Tachographs_Test_Get_Speed(); 
	#else
		return PulseInstantSpeed;
	#endif
}
/*********************************************************************
//��������  :VDRPulse_GetSecondSpeed
//����      :��ȡ������ƽ���ٶ�,��λkm/h
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
//      :
*********************************************************************/
u8 VDRPulse_GetSecondSpeed(void)
{
	  if(1 == GBTestFlag)
		{
        if((1 == VDRSpeedStatus_GetFlag())&&(0 == PulseSecondSpeed))
        {
            return Gps_ReadSpeed();
        }
        else
        {
            return PulseSecondSpeed;
        }
	  }
		else
		{
		    return PulseSecondSpeed;
		}
}
/*********************************************************************
//��������  :VDRPulse_GetTotalMile
//����      :��ȡ���������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��λ��0.01����
//      :
*********************************************************************/
u32 VDRPulse_GetTotalMile(void)
{
    return PulseTotalMile;
}


/*********************************************************************
//��������  :VDRPulse_AddMile
//����      :�ۼ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
//      :
*********************************************************************/
void VDRPulse_AddMile(void)
{
    u32 TimeCount;
	  float v = 0.0;
    
    PulseExtiCount++;
    PulseCount++;
    VdrCoefficientPulseCount++; //У׼����ʹ��
	

    if(1 == PulseExtiCount)
    {
        TIM_SetCounter(TIM3, 0);//��ʱ��3������0
        TIM_Cmd(TIM3, ENABLE);//�򿪶�ʱ��3
    }
    else 
    {
        TimeCount = TIM_GetCounter(TIM3);//����������ֵ
        if(TimeCount >= PulseWidthMin)
        {
            //�������
            if(PulseCount > (PulseCoef/50))//���ڵ���0.02����
            {
                PulseTotalMile += PulseCount*100/PulseCoef;
                PulseCount = 0;
            }

            if(0 == GBTestFlag)
            {
                PulseWidthCount++;
                if(PulseWidthCount >= VDR_INSTANT_SPEED_ARRAY_SIZE)
                {
                    PulseWidthCount = 0;
                }
                PulseWidth[PulseWidthCount] = TimeCount;
            }
            else
            {
                PulseWidth[0] = TimeCount; 
            }
        }
        else
        {
					  if(TimeCount>=25)//modify by joneming
            {
                v=1000000.0/((float)TimeCount*50.0);//modify by joneming
                TurnSpeed_InsertCurPulseVal(v);//modify by joneming
            }
            VdrCoefficientPulseCount--;
            PulseCount--;
					  
        }

        TIM_SetCounter(TIM3, 0);//��ʱ��3������0
    }
}
/*********************************************************************
//��������  :VDRPulse_CalInstantSpeed
//����      :����˲ʱ�ٶ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
//      :
*********************************************************************/
void VDRPulse_CalInstantSpeed(void)
{
    static u16  Count = 0;
        u8  i = 0;
    u8  InstantSpeed;
    u16 CountMin = 0xffff;
    u16 CountMax = 0;
    u32 sum = 0;
    u32 v;
    float   temp = 0.0;
	
    if(0 == GBTestFlag)
    {
        for(i=0; i<VDR_INSTANT_SPEED_ARRAY_SIZE; i++)
        {
            if(PulseWidth[i] > CountMax)
            {
                CountMax = PulseWidth[i];
            }
            if(PulseWidth[i] < CountMin)
            {
                CountMin = PulseWidth[i];
            }
            sum += PulseWidth[i];
            }
        sum -= CountMin;
        sum -= CountMax;
        sum = sum/3;
    }
    else
    {
        sum = PulseWidth[0];
    }
        
  if(0 == sum)
  {
    return;
  }
  
	temp=1000000.0/((float)sum*50.0);//modify by joneming������ũ��ת�ٹ���
  TurnSpeed_InsertCurPulseVal(temp);//modify by joneming
	
  temp = (3600.0*(u32)1000000)/((float)sum*50.0*(float)PulseCoef);//�����ٶȼ���
  v = (int)(temp);
  //if((temp-v)>=0.5)
  //{
    //v++;
  //}
    InstantSpeed = v&0xff;

    if(0 == GBTestFlag)
    {
        if(InstantSpeed > LastPulseInstantSpeed)
        {
            if((InstantSpeed-LastPulseInstantSpeed) >= 50)//����50��Ϊ�Ǹ���
            {
                Count++;
                if(Count >= 100)//���Ų�������100�Σ���Ϊ���쳣�������ͻ��
                {
                    Count = 0;
                    PulseInstantSpeed = InstantSpeed;
                    LastPulseInstantSpeed = InstantSpeed;
                }
            }
            else
            {
                Count = 0;
                PulseInstantSpeed = InstantSpeed;
                LastPulseInstantSpeed = InstantSpeed;
            }
        }
        else
        {
            Count = 0;
            PulseInstantSpeed = InstantSpeed;
            LastPulseInstantSpeed = InstantSpeed;
        }
    }
    else
    {
        PulseInstantSpeed = InstantSpeed;
        LastPulseInstantSpeed = InstantSpeed;
    }   
}
/*********************************************************************
//��������  :VDRPulse_Init
//����      :��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�˳��춨״̬ʱ����øú���
*********************************************************************/
void VDRPulse_Init(void)
{
    VDRPulse_InitTim3();
    VDRPulse_InitExti10();
}
/*********************************************************************
//��������  :VDRPulse_InitTim3
//����      :��ʼ����ʱ��3
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
static void VDRPulse_InitTim3(void)
{
 //dxl,2015.8,������STM32F105�ĳ�ʼ��
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
  u16   PrescalerValue;
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  PrescalerValue = 3599;//50us,TIM2,3,4ʱ��Ϊ72MHz
    
  //TIM_TimeBaseStructure.TIM_Period = 20000;//1s
  TIM_TimeBaseStructure.TIM_Period = 6000;//0.4s--8000
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM3, 0);
  
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM3, ENABLE);
 
    /* dxl,2015.9,������STM32F205�ĳ�ʼ��
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  u16   PrescalerValue;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  PrescalerValue = 2399;//50us ,TIM2 Ϊ48MHZ

  //TIM_TimeBaseStructure.TIM_Period = 65535;
  //TIM_TimeBaseStructure.TIM_Period = 20000;//1s,��2399��Ӧ
  TIM_TimeBaseStructure.TIM_Period = 8000;//0.4s,��2399��Ӧ
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM3, 0);
  
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM3, ENABLE);
    */
}
/*********************************************************************
//��������  :VDRPulse_InitExti10
//����      :��ʼ���ⲿ�ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
static void VDRPulse_InitExti10(void)
{
  //dxl,2015.8,������STM32F105�ĳ�ʼ��
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    
  EXTI_InitStructure.EXTI_Line = EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);   
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    /* dxl,2015.9,������STM32F205�ĳ�ʼ��
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);

  EXTI_InitStructure.EXTI_Line = EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    */
}
/*********************************************************************
//��������  :VDRPulse_TimIsr
//����      :��ʼ����ʱ���жϴ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void VDRPulse_TimIsr(void)
{

  u8    i = 0;

  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
    TIM_SetCounter(TIM3, 0);//��ʱ��3������0
                
    TIM_Cmd(TIM3, DISABLE);//�رն�ʱ��3

    PulseExtiCount = 0;//�ⲿ�жϼ�����0

    PulseInstantSpeed = 0;//˲ʱ�ٶ���0
        
    LastPulseInstantSpeed = 0;

    for(i=0; i<VDR_INSTANT_SPEED_ARRAY_SIZE; i++)//���������0
    {
         PulseWidth[i] = 0;
    }
    
    PulseWidthCount = 0;
		
		TurnSpeed_InsertCurPulseVal(0);//modify by joneming
  }
}
/*********************************************************************
//��������  :VDRPulse_ExtiIsr
//����      :�ⲿ�ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�յ������źţ��������ⲿ�жϣ���Ĵ���
//      :
*********************************************************************/
void VDRPulse_ExtiIsr(void)
{

  static u32    count = 0;
    
  if(0xE2 == VdrDocimasyStatus)//�춨����E2
  {
    count++;

    if(0 == count%2)
    {
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    }
    else
    {
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    }
  }
  else if(0 == VdrDocimasyStatus)//����Ӧ��
  {
    VDRPulse_AddMile();//�ۼ���ʻ���
    VDRPulse_CalInstantSpeed();//����˲ʱ�ٶ�
  }
  
  EXTI_ClearITPendingBit(VDR_PULSE_EXTI_LINE);   
}








