/************************************************************************
//�������ƣ�VDR_Pulse.c
//���ܣ�����ģ�鴦�����������ۼ���ʻ��̡�����˲ʱ�ٶȡ��������ٶȵļ��㡣
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1���ۼ���ʻ��̣�������������ķ�ʽ��ÿ���յ�һ�������źţ���ʻ����ۼ�һ��
//����˲ʱ�ٶȣ�ÿ�������źſ��Լ����Ӧ��һ���ٶ�ֵ��ÿ5�������źŶ�Ӧ5���ٶ�ֵ�������ֵ����Сֵȥ������ƽ���õ�˲ʱ�ٶ�
//�������ٶȣ�ÿ��50ms��ȡһ��˲ʱ�ٶȣ�����20��˲ʱ�ٶȵ�ƽ��ֵ�õ����ٶ�
//ÿ����ƽ���ٶȣ�ÿ���Ӷ�ȡһ�����ٶȣ�����60�����ٶȵ�ƽ��ֵ�õ�ÿ����ƽ���ٶ�
*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"

/********************���ر���*************************/
static void VDRPulse_InitTim(void);
static void VDRPulse_InitExti(void);
static void VDRPulse_Init(void);

/********************ȫ�ֱ���*************************/
u32	PulseTotalMile = 0;//�ۼ���ʻ��̣��������,��λ��0.01����
u32	PulseCoef = 0;//����ϵ��
u8	VdrRunMode = 0;//0:������������ģʽ;1:������ģʽ���ñ�־ͨ���˵���ѡ��

/********************�ⲿ����*************************/
extern	u8	SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS
extern 	vu32 	myPulseCnt;             //У׼����ʹ��
extern 	u8	TestStatus;//E0H:����춨״̬;E1H:�������������;E2H:��������ϵ��������;E3H:����ʵʱʱ��������;E4H:�˳��춨״̬

/********************���غ�������*********************/
static	u16	PulseWidthMin = 0;//�����ȼ�����Сֵ��С�ڸ�ֵ��Ϊ�Ǹ�������
static 	u16	PulseWidth[VDR_INSTANT_SPEED_ARRAY_SIZE+1] = {0};
static 	u8	PulseWidthCount = 0;

static 	u32	PulseCount = 0;//�����������1�����壬������1�����յ�PulseCoef������ʱ��ʾ����1����

static 	u32 	PulseExtiCount = 0;//�ⲿ�жϽ����������

static 	u8	PulseInstantSpeed = 0;//����˲ʱ�ٶȣ�km/h
static 	u8	LastPulseInstantSpeed = 0;//��һ�������ٶ�
static 	u8      PulseSecondSpeed = 0;//����ÿ��ƽ���ٶ�

/********************��������*************************/
/*********************************************************************
//��������	:VDRPulse_TimeTask
//����		:���嶨ʱ����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:���Ƿ���ENABLE
//��ע		:50ms�����1�Σ���ʱ��¼��1�룩�ۼ���ʻ��̣�������ƽ���ٶ�
*********************************************************************/
FunctionalState  VDRPulse_TimeTask(void)
{
	static	u8	InitFlag = 0;
  static  u16	Count = 0;
	static  u32	LasePulseTotalMile = 0;//��һ���ۼ���ʻ��̣��������,��λ��0.01����
  static  u8      SecondSpeedData[20] = {0};//50ms��ȡһ��˲ʱ�ٶȣ�20��˲ʱ�ٶȵ�ƽ��ֵΪÿ��ƽ���ٶ�
  static  u8      SecondSpeedCount = 0;
     
  u8	TmpData[5];
  u8      i;
  u16     sum;
	TIME_T CurTime,Time;
        
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
		
		RTC_ReadTime(&CurTime);
    if(CurTime.sec != Time.sec)//��仯ʱ�̼������ٶ�
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
      Time.sec = CurTime.sec;
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
/*********************************************************************
//��������	:VDRPulse_UpdateParameter
//����		:���²���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:��������ϵ�����ۼ���ʻ���
//		:������ϵ������Ϊ3600��7200ʱ�ᴥ������ϵ���Զ�У׼���ܣ�������ʱ���ܿ���
*********************************************************************/
void VDRPulse_UpdateParameter(void)
{
	u8	TmpData[4] = {0};
	u8	PramLen = 0;
	u32	temp;

	//��������ϵ��
	PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, TmpData);
	if(2 != PramLen)
	{
			PulseCoef = 7200;//Ĭ��Ϊ7200��ȡ�����ٶȴ�����ϵ��
	}
	else
	{
			PulseCoef = 0;
			PulseCoef |= (TmpData[0] << 8);//���ֽ�
			PulseCoef |= TmpData[1];//���ֽ�λ
	}

	//������������С��ֵ
	temp = (3600*(u32)1000000)/(50*PulseCoef*255);
	PulseWidthMin = temp&0xffff;

	//�����ۼ���ʻ���
	PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_LEN, FRAM_VDR_MILEAGE_ADDR);
	if(FRAM_VDR_MILEAGE_LEN == PramLen)//У�����ȷ
	{
		PulseTotalMile = 0;
		PulseTotalMile |= TmpData[0] << 24;	//���ֽ�
		PulseTotalMile |= TmpData[1] << 16;	//�θ��ֽ�
		PulseTotalMile |= TmpData[2] << 8;	//���ֽ�
		PulseTotalMile |= TmpData[3];		//���ֽ�
	}
        else
        {
                //�����ݵ��ۼ���ʻ���
	        PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN, FRAM_VDR_MILEAGE_BACKUP_ADDR);
	        if(FRAM_VDR_MILEAGE_BACKUP_LEN == PramLen)//У�����ȷ
	        {
		        PulseTotalMile = 0;
		        PulseTotalMile |= TmpData[0] << 24;	//���ֽ�
		        PulseTotalMile |= TmpData[1] << 16;	//�θ��ֽ�
		        PulseTotalMile |= TmpData[2] << 8;	//���ֽ�
		        PulseTotalMile |= TmpData[3];		//���ֽ�
	        }
                else
                {
                        PulseTotalMile = 0;
                }
        }

	if(0 == VdrRunMode)//��������ģʽ
	{
		if((7200 == PulseCoef)||(3600 == PulseCoef))
		{
//			setAdjustFunction(ENABLE);//��������ϵ��У׼����
		}
	}
	else
	{
		//������ģʽ
	}
	
}
/*********************************************************************
//��������	:VDRPulse_GetInstantSpeed
//����		:��ȡ����˲ʱ�ٶ�,��λkm/h
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
//		:
*********************************************************************/
u8 VDRPulse_GetInstantSpeed(void)
{
	return PulseInstantSpeed;
}
/*********************************************************************
//��������	:VDRPulse_GetSecondSpeed
//����		:��ȡ������ƽ���ٶ�,��λkm/h
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
//		:
*********************************************************************/
u8 VDRPulse_GetSecondSpeed(void)
{
	return PulseSecondSpeed;
}
/*********************************************************************
//��������	:VDRPulse_GetTotalMile
//����		:��ȡ���������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:��λ��0.01����
//		:
*********************************************************************/
u32 VDRPulse_GetTotalMile(void)
{
	return PulseTotalMile;
}


/*********************************************************************
//��������	:VDRPulse_AddMile
//����		:�ۼ����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
//		:
*********************************************************************/
void VDRPulse_AddMile(void)
{
	u32	TimeCount;
	
	PulseExtiCount++;
	PulseCount++;
	myPulseCnt++; //У׼����ʹ��

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

			if(0 == VdrRunMode)
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
			myPulseCnt--;
			PulseCount--;
		}

		TIM_SetCounter(TIM3, 0);//��ʱ��3������0
	}
}
/*********************************************************************
//��������	:VDRPulse_CalInstantSpeed
//����		:����˲ʱ�ٶ�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
//		:
*********************************************************************/
void VDRPulse_CalInstantSpeed(void)
{
	static u16	Count = 0;
  u8	i = 0;
	u8	InstantSpeed;
	u16	CountMin = 0xffff;
	u16	CountMax = 0;
	u32	sum = 0;
	u32	v;
	float	temp = 0;
	

	if(0 == VdrRunMode)
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

	temp = (3600.0*(u32)1000000)/((float)sum*50.0*(float)PulseCoef);//��Ӧ50us������λ
        v = (int)(temp);
        if((temp-v)>=0.5)
        {
        	v ++;
        }
	InstantSpeed = v&0xff;

	if(0 == VdrRunMode)
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
//��������	:VDRPulse_Init
//����		:��ʼ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
static void VDRPulse_Init(void)
{
	VDRPulse_InitTim();
	VDRPulse_InitExti();
}
/*********************************************************************
//��������	:VDRPulse_InitTimer
//����		:��ʼ����ʱ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
static void VDRPulse_InitTim(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	u16	PrescalerValue;
	
	 /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Compute the prescaler value */
  PrescalerValue = 3599;//50us,TIM2,3,4ʱ��Ϊ72MHz
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 20000;//1s
  //TIM_TimeBaseStructure.TIM_Period = 8000;//0.4s
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM3, 0);
  
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
/*********************************************************************
//��������	:VDRPulse_InitExti
//����		:��ʼ���ⲿ�ж�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
static void VDRPulse_InitExti(void)
{
		EXTI_InitTypeDef   EXTI_InitStructure;
  	GPIO_InitTypeDef   GPIO_InitStructure;

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
}
/*********************************************************************
//��������	:VDRPulse_TimIsr
//����		:��ʼ����ʱ���жϴ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void VDRPulse_TimIsr(void)
{
	u8	i = 0;

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
		
	}
}
/*********************************************************************
//��������	:VDRPulse_ExtiIsr
//����		:�ⲿ�ж�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:�յ������źţ��������ⲿ�жϣ���Ĵ���
//		:
*********************************************************************/
void VDRPulse_ExtiIsr(void)
{
	static u8	count = 0;
	
	if(0xE2 == TestStatus)//�춨����E2
	{
		count++;
    if(count >= 2)
    {
			count = 0;
    }
		if(0 == count)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_14);
		}
		else
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
		}
	}
	else if(0 == TestStatus)//����Ӧ��
	{
		VDRPulse_AddMile();//�ۼ���ʻ���
		VDRPulse_CalInstantSpeed();//����˲ʱ�ٶ�
	}

	/* Clear the EXTI line 0 pending bit */
    	EXTI_ClearITPendingBit(VDR_PULSE_EXTI_LINE);
        
}








