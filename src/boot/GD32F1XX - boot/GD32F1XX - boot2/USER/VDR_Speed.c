/************************************************************************
//�������ƣ�VDR_Speed.c
//���ܣ�48h��ʻ�ٶȼ�¼
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_rtc.h"

/********************���ر���*************************/
static u8	CurrentSecondSpeed[VDR_DATA_SPEED_STEP_LEN];//��ǰ���ӵ�ÿ�����ٶȺ�״̬����
static u8	LastSecondSpeed[VDR_DATA_SPEED_STEP_LEN];//��һ���ӵ�ÿ�����ٶȺ�״̬���壬ǰ6�ֽ�Ϊ��ʼʱ��
static u8	MinuteSpeed = 0;//ÿ����ƽ���ٶ�
static u8	MinuteSpeedCount = 0;//��¼��λ��

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


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
FunctionalState VDRSpeed_TimeTask(void)
{
	static u32 	LastTimeCount = 0;
	static u8	AlreadyRunFlag = 0;//1Ϊ����ʻ����0Ϊδ��ʻ��
	static u8	LastRunFlag = 0;
	static u8	InitFlag = 0;

	u32 	CurrentTimeCount;
	u32	TimeCount;
	u16	sum;
	u16	Addr;
	u8	Speed;
	u8	Status;
	u8	i;
	u8	RunFlag;
	u8	Buffer[5];
	TIME_T	CurrentTime;

	if(0 == InitFlag)
	{
		if( 1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_SPEED))
		{
			VDRSpeed_Init();
			InitFlag = 1;
		}
	}
	else
	{
		CurrentTimeCount = RTC_GetCounter();

		if(CurrentTimeCount != LastTimeCount)//��ʱ�Ӷ���
		{
			RTC_ReadTime(&CurrentTime);
			if(1 == Public_CheckTimeStruct(&CurrentTime))
			{
				Speed = VDRPulse_GetSecondSpeed();//�ٶ�

				Status = 0;//״̬
				//Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:ɲ��
				//Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:��ת
				//Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:��ת
				///Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:Զ��
				//Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:����
				//bit2-bit1δ����
				//Status |= Io_ReadExtCarStatusBit(STATUS_BIT_ACC);//bit0:ACC
			
				CurrentSecondSpeed[2*CurrentTime.sec] = Speed;
				CurrentSecondSpeed[2*CurrentTime.sec+1] = Status;

				RunFlag = VDRDoubt_GetCarRunFlag();
				if(1 == RunFlag)
				{
					AlreadyRunFlag = 1;
				}
				if((1 == LastRunFlag)&&(0 == RunFlag))//����ʻ��Ϊͣʻ����¼ͣ��ʱ��
				{
					Public_ConvertLongToBuffer(CurrentTimeCount,Buffer);
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
						if(sum > 0)
						{
							TimeCount = CurrentTimeCount;
							TimeCount -= 69;
							VDRData_Write(VDR_DATA_TYPE_SPEED, LastSecondSpeed, VDR_DATA_SPEED_STEP_LEN-5, TimeCount);//��¼ÿ���ӵ���ʻ�ٶȺ�״̬
							
							VDRSpeed_SaveMinuteSpeed(TimeCount, MinuteSpeed);//��¼ͣ��ǰ15�����ٶ�
						}

					}
					AlreadyRunFlag = 0;
				}

				LastRunFlag = RunFlag;

			
			}
			else//ʱ�����
			{
			
			}
		}
	
		LastTimeCount = CurrentTimeCount;
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
//��ע������Ϊ0��ʾû�ж�ȡ����ȷ��ֵ������Ϊ45��3�ֽ�*15����ʾ��ȡ����ȷ��ֵ
//��������˳���ǣ�
//ͣ��ǰ1���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
//ͣ��ǰ2���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
//ͣ��ǰ3���ӵ����ݣ�3�ֽڣ�1�ֽ�Сʱ��1�ֽڷ��ӣ�1�ֽ�ƽ���ٶȣ���Ϊhexֵ��
//......
***************************************************************************/
u8 VDRSpeed_Get15MinuteSpeed(u8 *pBuffer)
{
	u8	Buffer[10];
	u8	BufferLen;
	u8	Speed[20];
	u8	*p = NULL;
	u8	length;
	s16	i;
	s16	j;
	s16	k;
	u16	Addr;
	u32	StopTimeCount;
	u32	MinuteTimeCount;
	u32	LastMinuteTimeCount;
	TIME_T	Time;

	Addr = FRAM_VDR_SPEED_STOP_TIME_ADDR;//��ͣ��ʱ��
	BufferLen = FRAM_BufferRead(Buffer, 4, Addr);
	if(0 == BufferLen)
	{
		Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+i*6;//6 = 4�ֽ�ʱ��+1�ֽ�ƽ���ٶ�+1�ֽ�У��
		BufferLen = FRAM_BufferRead(Buffer, 5, Addr);
		if(0 == BufferLen)
		{
			return 0;
		}
		else
		{	
			LastMinuteTimeCount = Public_ConvertBufferToLong(Buffer);
			StopTimeCount = LastMinuteTimeCount;
		}
	}
	else
	{
		LastMinuteTimeCount = Public_ConvertBufferToLong(Buffer);
		StopTimeCount = LastMinuteTimeCount;
	}

	length = 0;//������ʼ��
	for(i=0; i<15; i++)
	{
		Speed[i] = 0;
	}
	p = Speed;
	i = MinuteSpeedCount;
	i--;
	if(i < 0)
	{
		i = 15;
	}

	for(;;)
	{
		Addr = FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR+i*6;
		BufferLen = FRAM_BufferRead(Buffer, 5, Addr);
		if(0 == BufferLen)
		{
			*p++ = 0;
			length++;
		}
		else
		{
			MinuteTimeCount = Public_ConvertBufferToLong(Buffer);
			if((0xFFFFFFFF == MinuteTimeCount)||(0 == MinuteTimeCount))//����������δд��ֵ
			{
				*p++ = 0;
				length++;
			}
			else if(MinuteTimeCount > LastMinuteTimeCount)//��ǰ����ʱ�������һ���������Ǵ���
			{
				return 0;
			}
			else
			{
				k = (LastMinuteTimeCount - MinuteTimeCount)/60 - 1;//��Ҫ���0�ĸ���
				if((k+length) > 15)
				{
					k = 15-length;
				}
				for(j=0; j<k; j++)
				{
					*p++ = 0;
					length++;
				}
				*p++ = Buffer[5];
				length++;
				LastMinuteTimeCount = MinuteTimeCount;
			}	
		}

		i--;
		if(i < 0)
		{
			i = 15;
		}

		if(length >= 15)
		{
			break;
		}	
	}

	p = pBuffer;
	Gmtime(&Time, StopTimeCount);
	StopTimeCount -= Time.sec;
	StopTimeCount -= 60;
	for(i=0; i<15; i++)
	{
		Gmtime(&Time, StopTimeCount);
		*p++ = Time.hour;
		*p++ = Time.min;
		*p++ = Speed[i];
		StopTimeCount -= 60;
	}
	

	return 45;	
}

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
	u8	i;
	u8	Buffer[5];
	u8	BufferLen;
	u16	Addr;
	u32	TimeCount;
	//u32	MinTimeCount = 0xFFFFFFFF;
	u32	MaxTimeCount = 0;
	
	for(i=0; i<120; i++)
	{
			
		CurrentSecondSpeed[i] = 0;
	}

	for(i=0; i<126; i++)
	{
			
		LastSecondSpeed[i] = 0;
	}

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
					MinuteSpeedCount = i+1;
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


	u16	Addr;
	u8	Buffer[6];

	
	if(MinuteSpeedCount < 16)
	{
		Public_ConvertLongToBuffer(TimeCount,Buffer);
		Buffer[5] = MinuteSpeed;
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






