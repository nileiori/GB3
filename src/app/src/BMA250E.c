/************************************************************************
//�������ƣ�BMA250E.c
//���ܣ��෭��������ײ����
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2016.3
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1�����档��ײ����ģ����IC�����ýӿڣ�ʹ��IO��ģ��I2C
*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "BMA250E.h"
#include "ICCARD_E2.h"
#include "ICCARD_M3.h"
#include "EepromPram.h"
#include "Io.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "Public.h"
#include "fm25c160.h"


/********************���ر���*************************/
static u8 InitFlag = 0;
static u8 AngleInitFlag = 0;	
static s16 XYZdata[3] = {0,0,0};//xyz������,XZYdata[0]Ϊx�ᡢXZYdata[1]Ϊy�ᡢXZYdata[2]Ϊz��
static float XYZgArray[BMA250E_G_ARRAY_LEN][3];

static float BMA250EAngle = 0;//Z����б�Ƕ�

static float BMA250ESetAngle = 0.0;//���õ���б����ֵ
static float BMA250ESetG = 0.0;//���õ���ײ���ٶ���ֵ
static float BMA250EInitAngle = 361.0;//��ʼ�Ƕ�
static u8 BMA250EInitAngleEnable = 0;
static u8 gRangeType = 0;
static u8 BMA250ECrashAlarmFlag = 0;
static u8 BMA250ERolloverAlarmFlag = 0;

static u8 BMA250ERegisterArray[0x40] = {0};
//static u8 BMA250ERegister26SetValue = 0;//�Ĵ���0x26������ֵ
//static u8 BMA250ERegister28SetValue = 0;//�Ĵ���0x28������ֵ
/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static u8 BMA250E_ReadRegister(u8 RegisterID, u8 *Value);
static u8 BMA250E_WriteRegister(u8 RegisterID, u8 Value);
static u8 BMA250E_SetGRange(u16 Value);
static void BMA250E_ReadXYZAcceleration(void);
static void BMA250E_RolloverAlarm(void);
static void BMA250E_CrashAlarm(void);
static void BMA250E_Delay(u32 time);
static void BMA250E_ReadAllRegister(void);
static void BMA250E_InitExit(void);
static u8 BMA250E_WriteRegisterAndCheck(u8 RegisterID, u8 Value, u8 CheckValue);
/********************���ر���*************************/

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

/********************��������*************************/
/**************************************************************************
//��������BMA250E_TimeTask(void)
//���ܣ�BMA250E��ʱ����
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��50ms����1��
***************************************************************************/
FunctionalState BMA250E_TimeTask(void)
{
	  u8 BufferLen;
	
    if(0 == InitFlag)
    {

			  InitFlag = 1;
			  BMA250E_Init();
    }
    else
    {
        BMA250E_ReadXYZAcceleration();
        BMA250E_CrashAlarm();
			  
			  if((0 == AngleInitFlag)&&(1 == BMA250EInitAngleEnable))
				{
//						BMA250EInitAngle = BMA250E_GetAngle();
			      BufferLen = FRAM_BufferRead((u8 *)&BMA250EInitAngle,FRAM_ROLL_OVER_INIT_ANGLE_LEN,FRAM_ROLL_OVER_INIT_ANGLE_ADDR);
		        if(FRAM_ROLL_OVER_INIT_ANGLE_LEN == BufferLen)
		        {
		
		        }
		        else
		        {
		            BMA250EInitAngle = 0.0;
		        }
						AngleInitFlag = 1;
					
				}
    }
		

    return ENABLE;
}
/**************************************************************************
//��������BMA250E_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��
***************************************************************************/
void BMA250E_Init(void)
{
	  BMA250E_Delay(80000);
	  if(0 == ICCARD_M3_I2C_CheckDevice(BMA250E_G_SENSOR_ADDR))
	  {
			  ICCARD_M3_I2C_Stop();
				BMA250E_Delay(20000);
			  if(0 == ICCARD_M3_I2C_CheckDevice(BMA250E_G_SENSOR_ADDR))
		    {
			       ICCARD_M3_I2C_Stop();
				     BMA250E_Delay(20000);
		    }
	  }
		
		BMA250E_Delay(80000);
		BMA250E_InitExit();
	
		BMA250E_WriteRegister(BMA250E_REGISTER_14, 0xB6);
		BMA250E_Delay(80000);
		
		BMA250E_UpdateRolloverParameter();
		BMA250E_UpdateCrashParameter();
		BMA250E_ReadAllRegister();
		
		 AngleInitFlag = 0;
			 
}
/**************************************************************************
//��������BMA250E_Calibration
//���ܣ��Ƕ�У׼
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void BMA250E_Calibration(void)
{
    BMA250EInitAngle = BMA250E_GetAngle();
	  FRAM_BufferWrite(FRAM_ROLL_OVER_INIT_ANGLE_ADDR, (u8 *)&BMA250EInitAngle,FRAM_ROLL_OVER_INIT_ANGLE_LEN);
	  InitFlag = 0;
}
/**************************************************************************
//��������BMA250E_InitExit
//���ܣ���ʼ���ⲿ�ж�
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��
***************************************************************************/
static void BMA250E_InitExit(void)
{
	  EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
	
	
    RCC_APB2PeriphClockCmd(BMA250E_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BMA250E_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BMA250E_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(BMA250E_EXTI_GPIO, BMA250E_GPIO_PINSOURCE);
    
    EXTI_InitStructure.EXTI_Line = BMA250E_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);   
    
    NVIC_InitStructure.NVIC_IRQChannel = BMA250E_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
		EXTI_ClearITPendingBit(BMA250E_EXTI_LINE);
	
	   
}
/**************************************************************************
//��������BMA250E_ReadAllRegister
//���ܣ������еļĴ�����ֵ
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
static void BMA250E_ReadAllRegister(void)
{
	
    u8 RegisterID;
	  u8 flag;

	  for(RegisterID=0; RegisterID <= 0x10; RegisterID++)
	  {
		    flag = BMA250E_ReadRegister(RegisterID,BMA250ERegisterArray+RegisterID);
			  if(0 == flag)
				{
				    
				}
				
		}
		
}
/**************************************************************************
//��������BMA250E_UpdateRolloverParameter
//���ܣ����²෭����
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void BMA250E_UpdateRolloverParameter(void)
{
    u8 Buffer[5];
	  u16 temp;
	
	  if(E2_SET_ROLLOVER_ALARM_LEN == EepromPram_ReadPram(E2_SET_ROLLOVER_ALARM_ID,Buffer))
		{
		    temp = Buffer[0]<<8;
			  temp |= Buffer[1];
			  BMA250ESetAngle = temp;
		}
		else
		{
		    BMA250ESetAngle = 45.0;
		}
		
		
}
/**************************************************************************
//��������BMA250E_UpdateCrashParameter
//���ܣ�������ײ����
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void BMA250E_UpdateCrashParameter(void)
{
	 
	  u8 Buffer[5];
	  u16  temp;
	
	  
	
    if(E2_SET_CRASH_ALARM_LEN == EepromPram_ReadPram(E2_SET_CRASH_ALARM_ID,Buffer))
		{
			  BMA250ESetG = Buffer[0]*0.1;
		}
		else
		{
		    BMA250ESetG = 2.0;
		}
	
		if(1 == BBGNTestFlag)//dxl,2016.5.28
		{
		    BMA250ESetG = BMA250ESetG+1.0;//��1.0g��ƫ����������������
		}
		
		temp = (u16)(BMA250ESetG*1000);
		
		BMA250E_SetGRange(temp);
		
#ifdef BMA250E_USE_HIGH_G_INT
		BMA250E_WriteRegister(BMA250E_REGISTER_17, 0x07);
		BMA250E_WriteRegister(BMA250E_REGISTER_19, 0x02);
#endif
		
#ifdef BMA250E_USE_SLOPE_INT
		BMA250E_WriteRegister(BMA250E_REGISTER_16, 0x07);
		BMA250E_WriteRegister(BMA250E_REGISTER_19, 0x04);
		BMA250E_WriteRegister(BMA250E_REGISTER_20, 0x01);
#endif
		
		BMA250E_ReadAllRegister();
		
		
		
		
}
/**************************************************************************
//��������BMA250E_ClearInitFlag
//���ܣ������ʼ����־
//���룺��
//�������
//����ֵ����
//��ע����־�����������³�ʼ��
***************************************************************************/
void BMA250E_ClearInitFlag(void)
{
    InitFlag = 0;
}
/**************************************************************************
//��������BMA250E_GetAngle
//���ܣ���ȡ�Ƕ�ֵ
//���룺��
//�������
//����ֵ���Ƕ�ֵ
//��ע��
***************************************************************************/
u8 BMA250E_GetAngle(void)
{
		u8 Angle;
	
	  Angle = (u8)BMA250EAngle;
	
	  return Angle;
}
/**************************************************************************
//��������BMA250E_GetRolloverAlarmFlag
//���ܣ���ȡ�෭������־
//���룺��
//�������
//����ֵ���෭������־
//��ע��1Ϊ����������0Ϊû��
***************************************************************************/
u8 BMA250E_GetRolloverAlarmFlag(void)
{
    return BMA250ERolloverAlarmFlag;
}
/**************************************************************************
//��������BMA250E_GetCrashAlarmFlag
//���ܣ���ȡ��ײ������־
//���룺��
//�������
//����ֵ����ײ������־
//��ע��1Ϊ����������0Ϊû��
***************************************************************************/
u8 BMA250E_GetCrashAlarmFlag(void)
{
    return BMA250ECrashAlarmFlag;
}
/**************************************************************************
//��������BMA250E_SetGRange
//���ܣ������������ٶȷ�Χ
//���룺��
//�������
//����ֵ��0��ʾ��ȷ����0��ʾ����
//��ע������Value��ֵ�����ú��ʵ�����,Value��λΪmg
***************************************************************************/
u8 BMA250E_SetGRange(u16 Value)
{
	  u8 temp;
	  u8 flag = 0;
	  static u8 count = 0;
	
	
	  if(Value >= 16000)
		{
		    return 1;
		}
    else if(Value >= 8000)
    {
			if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_0F, BMA250E_16G_RANGE, BMA250E_16G_RANGE))
			{
			
			}
			else
			{
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("�������̳���",100);
				  flag = 1;
			}
    }
    else if(Value >= 4000)
    {
			if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_0F, BMA250E_8G_RANGE, BMA250E_8G_RANGE))
			{
			
			}
			else
			{
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("�������̳���",100);
				  flag = 1;
			}

    }
    else if(Value >= 2000)
    {
			if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_0F, BMA250E_4G_RANGE, BMA250E_4G_RANGE))
			{
			
			}
			else
			{
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("�������̳���",100);
				  flag = 1;
			}
    }
    else
    {
			if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_0F, BMA250E_2G_RANGE, BMA250E_2G_RANGE))
			{
			
			}
			else
			{
			   //��ʾ���ò���ȷ
				  Public_ShowTextInfo("�������̳���",100);
				  flag = 1;
			}
      
    }
		BMA250E_Delay(20000);
		BMA250E_ReadRegister(BMA250E_REGISTER_0F,&temp);
		switch(temp)
		{
			case BMA250E_2G_RANGE:
			{
				gRangeType = 2;
#ifdef BMA250E_USE_HIGH_G_INT		
				temp = (u8)(Value/(3.91*gRangeType));
				//BMA250ERegister26SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_26, temp, temp))
#endif
					
#ifdef BMA250E_USE_SLOPE_INT
				temp = (u8)(Value*2.0/(3.91*gRangeType));
//				BMA250ERegister28SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_28, temp, temp))
#endif
			  {
			
			  }
			  else
			  {
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("������ײ����ֵ����",100);
					flag = 1;
			  }
			  break;
			}
			case BMA250E_4G_RANGE:
			{
				gRangeType = 4;
#ifdef BMA250E_USE_HIGH_G_INT			
				temp = (u8)(Value/(3.91*gRangeType));
				//BMA250ERegister26SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_26, temp, temp))
#endif			
#ifdef BMA250E_USE_SLOPE_INT
				temp = (u8)(Value*2.0/(3.91*gRangeType));
//				BMA250ERegister28SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_28, temp, temp))
#endif
			  {
			
			  }
			  else
			  {
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("������ײ����ֵ����",100);
					flag = 1;
			  }
			  break;
			}
			case BMA250E_8G_RANGE:
			{
				gRangeType = 8;
#ifdef BMA250E_USE_HIGH_G_INT			
				temp = (u8)(Value/(3.91*gRangeType));
				//BMA250ERegister26SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_26, temp, temp))
#endif				
#ifdef BMA250E_USE_SLOPE_INT
				temp = (u8)(Value*2.0/(3.91*gRangeType));
///				BMA250ERegister28SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_28, temp, temp))
#endif
			  {
			
			  }
			  else
			  {
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("������ײ����ֵ����",100);
					flag = 1;
			  }
			  break;
			}
			case BMA250E_16G_RANGE:
			{
				gRangeType = 16;
#ifdef BMA250E_USE_HIGH_G_INT			
				temp = (u8)(Value/(3.91*gRangeType));
				//BMA250ERegister26SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_26, temp, temp))
#endif					
#ifdef BMA250E_USE_SLOPE_INT
				temp = (u8)(Value*2.0/(3.91*gRangeType));
//				BMA250ERegister28SetValue = temp;
				if(0 == BMA250E_WriteRegisterAndCheck(BMA250E_REGISTER_28, temp, temp))
#endif
			  {
			
			  }
			  else
			  {
			    //��ʾ���ò���ȷ
				  Public_ShowTextInfo("������ײ����ֵ����",100);
					flag = 1;
			  }
			  break;
			}
			default :
			{
				flag = 1;
				break;
			}
		}
		
		if(1 == flag)
		{
			  count++;
			  if(count<3)
				{
		        //��ʾ���ò���ȷ
				    Public_ShowTextInfo("���ó������³�ʼ����ײ����ģ��",100);
			      BMA250E_ClearInitFlag();
				}
				else
				{
				    count = 0;
					  Public_ShowTextInfo("��ʼ��������ײ����ģ�����",100);
				}
		}
		
		return 0;
}
/**************************************************************************
//��������BMA250E_RolloverAlarm
//���ܣ��෭����
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void BMA250E_RolloverAlarm(void)
{
    float Angle;
	  float gZMin = 16.0;
	  float gZMax = -16.0;
	  float gZ;

	  u8 i;
	
	  static u32 count1 = 0;
	  static u32 count2 = 0;
	  static u32 count3 = 0;
	
	  static float LastAngle = 0;
    
	  gZ = 0;

	  for(i=0; i<BMA250E_G_ARRAY_LEN; i++)
	  {
		    if(gZMin > XYZgArray[i][2])
				{
				    gZMin = XYZgArray[i][2];
				}
				if(gZMax < XYZgArray[i][2])
				{
				    gZMax = XYZgArray[i][2];
				}
				gZ += XYZgArray[i][2];
		}
		
	  gZ -= gZMin;
		gZ -= gZMax;
		gZ /= (BMA250E_G_ARRAY_LEN-2);
		
		if(gZ > 1.0)
		{
		  gZ = 1.0;
		}
		else if(gZ < -1.0)
		{
		  gZ = -1.0;
		}
		
    Angle = acos(gZ/1.0);
    Angle = Angle*360.0/(2*BMA250E_PI);//0~180��֮��
		if((fabs(Angle-LastAngle) <= 2.0)&&(0 == BMA250EInitAngleEnable))
		{
		    count3++;
			  if(count3 >= 100)
				{
				    count3 = 0;
					  BMA250EInitAngleEnable = 1;
				}
		}
		else
		{
		     count3 = 0;
		}
		LastAngle = Angle;
		
    BMA250EAngle = Angle;
	
	  if(((BMA250EAngle >= BMA250EInitAngle)&&((BMA250EAngle-BMA250EInitAngle) >= BMA250ESetAngle)&&(0 != BMA250ESetAngle)&&(BMA250EInitAngle < 360))||
		  ((BMA250EAngle < BMA250EInitAngle)&&((BMA250EInitAngle-BMA250EAngle) >= BMA250ESetAngle)&&(0 != BMA250ESetAngle)&&(BMA250EInitAngle < 360)))
		{
			  count2 = 0;
			  count1++;
			  if(count1 >= 3)
				{
			      BMA250ERolloverAlarmFlag = 1;
					  Io_WriteAlarmBit(ALARM_BIT_SIDE_TURN_PRE_ALARM, SET);
				}
		}
		else
		{
			  count1 = 0;
		    count2++;
			  if(count2 >= 3)
				{
				    BMA250ERolloverAlarmFlag = 0;
					  Io_WriteAlarmBit(ALARM_BIT_SIDE_TURN_PRE_ALARM, RESET);
				}
		}
		
    
    
}
/**************************************************************************
//��������BMA250E_CrashAlarm
//���ܣ���ײ����
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void BMA250E_CrashAlarm(void)
{
	  static u32 count = 0;
	  if(Io_ReadAlarmBit(ALARM_BIT_IMPACT_PRE_ALARM))
		{
		    count++;
			  if(count >= 10)
				{
				    count = 0;
					  Io_WriteAlarmBit(ALARM_BIT_IMPACT_PRE_ALARM, RESET);
				}
			  
		}
		else
		{
		    count = 0;
		}
}
/**************************************************************************
//��������BMA250E_ReadXYZAcceleration
//���ܣ���x�ᡢy�ᡢz����ٶ�ֵ
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void BMA250E_ReadXYZAcceleration(void)
{
    s16 temp;
    u8 HighByte;
    u8 LowByte;
    float TempXYZg[3];
	  float xyz;
    u8 i;
	  u8 flag;
	   
	  static u8 count = 0;
    
    for(i=0; i<3; i++)
    {
        flag = BMA250E_ReadRegister(BMA250E_REGISTER_02+i*2,&LowByte)&0xc0;
		    if(0 != flag)
				{
				    return ;
				}
        flag = BMA250E_ReadRegister(BMA250E_REGISTER_03+i*2,&HighByte);
		    if(0 != flag)
				{
				    return ;
				}
        temp = HighByte << 8;
        temp = temp | LowByte;
        
        XYZdata[i] = (temp >> 6); 
        
    }
		
    switch(gRangeType)
    {
        case 2:
        case 4:
        case 8:
        case 16:
        {
				
            for(i=0; i<3; i++)
            {
              TempXYZg[i] = (float)XYZdata[i]*3.91*gRangeType/2000;
            }
						xyz = fabs(TempXYZg[0]) + fabs(TempXYZg[1]) + fabs(TempXYZg[2]);
						if(xyz >= 0.8)
						{
						    for(i=0; i<3; i++)
                {
									   if(count >= BMA250E_G_ARRAY_LEN)
								     {
								         count = 0;
								     }
									   XYZgArray[count][i] = TempXYZg[i];
                }
								count++;
								if(count >= BMA250E_G_ARRAY_LEN)
								{
								    count = 0;
								}
								BMA250E_RolloverAlarm();
						}
            break;
        }
        default :
        {
					  gRangeType = 0;
					  Public_ShowTextInfo("��ײ�������̷�Χֵ����",100);
					  BMA250E_Init();
            break;
        }
    }
    
}
/**************************************************************************
//��������BMA250E_ReadRegister
//���ܣ����Ĵ�����ֵ
//���룺��
//�������
//����ֵ��0��ʾ�ɹ�����0��ʾʧ��
//��ע��
***************************************************************************/
u8 BMA250E_ReadRegister(u8 RegisterID, u8 *Value)
{
    if(RegisterID > BMA250E_REGISTER_3F)
    {
        return 1;
    }
    
    if(0 == E2_ReadOneByte(Value,BMA250E_G_SENSOR_ADDR,RegisterID))
		{
		    return 1;
		}
		
		BMA250E_Delay(1000);
		
		return 0;
    
}
/**************************************************************************
//��������BMA250E_WriteRegister
//���ܣ�д�Ĵ�����ֵ
//���룺��
//�������
//����ֵ��0��ʧ�ܣ�1���ɹ�
//��ע��
***************************************************************************/
static u8 BMA250E_WriteRegister(u8 RegisterID, u8 Value)
{
    u8 Addr;
    u8 flag;
    
	  
	
    if(RegisterID > BMA250E_REGISTER_3F)
    {
        return 0;
    }
    
    Addr = RegisterID;
    
    flag = E2_WriteOneByte(Value,BMA250E_G_SENSOR_ADDR,Addr);
		
		BMA250E_Delay(1000);
    
    return flag;
    
}
/**************************************************************************
//��������BMA250E_WriteRegisterAndCheck
//���ܣ�д�Ĵ�����ֵ���ض�д��ļĴ���
//���룺��
//�������
//����ֵ��0���ɹ�����0������ʧ��
//��ע����ȡ�ļĴ��������CheckValue��Ƚϣ���������CheckValue��������ʧ��
***************************************************************************/
static u8 BMA250E_WriteRegisterAndCheck(u8 RegisterID, u8 Value, u8 CheckValue)
{
	  u8  count = 0;
	  u8  temp = 0;
	
    if(RegisterID > BMA250E_REGISTER_3F)
    {
        return 1;
    }
    
		do
	  {
			 
			 count++;
			 if(count >= 5)
			 {
			     return 2;
			 }
       E2_WriteOneByte(Value,BMA250E_G_SENSOR_ADDR,RegisterID);
			 BMA250E_Delay(1000);
			 BMA250E_ReadRegister(RegisterID,&temp);
			 
		}while(temp != CheckValue);
		
    
    return 0;
    
}
/**************************************************************************
//��������BMA250E_Delay(u32 time)
//���ܣ���ʱ����
//���룺��
//�������
//����ֵ����
//��ע��1����λ��time�ȼ���һ��for����ִ��ʱ��
***************************************************************************/
static void BMA250E_Delay(u32 time)
{
    u32 i;
	
	  for(i=0; i<time; i++)
	  {
		
		}
}










