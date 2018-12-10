/************************************************************************
//�������ƣ�VDR_Power.c
//���ܣ������¼
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
#include "Adc.h"

/********************���ر���*************************/


/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static void VDRPower_Init(void);
static u8 VDRPower_GetStatus(void);

/********************��������*************************/

/**************************************************************************
//��������VDRPower_TimeTask
//���ܣ��ϵ��ϵ��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��50ms����1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDRPower_TimeTask(void)
{
	static u8	VdrPowerInitFlag = 0;
	static u8	LastStatus = 0;
	static u8	count = 0;

	u8	Status;
	u8	Buffer[VDR_DATA_POWER_STEP_LEN];
	u32	TimeCount;
	u16	Addr;
	

	if(0 == VdrPowerInitFlag)
	{
		if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_POWER))//��¼���Լ������
		{
			VDRPower_Init();//��¼һ�ε����¼���ϵ��¼
			VdrPowerInitFlag = 1;
		}
	}
	else
	{
		Status = VDRPower_GetStatus();//0:����;1:����

		if((0 == LastStatus)&&(1 == Status))//�ɹ��絽����
		{
			TimeCount = RTC_GetCounter();
			VDRPub_ConvertNowTimeToBCD(Buffer);
			Buffer[6] = 2;//1:�ϵ�;2:�ϵ� 
			VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);
		}
		else if((1 == LastStatus)&&(0 == Status))//�ɵ��絽����
		{
			TimeCount = RTC_GetCounter();
			VDRPub_ConvertNowTimeToBCD(Buffer);
			Buffer[6] = 1;//1:�ϵ�;2:�ϵ� 
			VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);
		}

		LastStatus = Status;

		count++;
		if(count >= 20)//һ���Ӽ�¼һ��ʱ��
		{
			count = 0;
			TimeCount = RTC_GetCounter();
			Public_ConvertLongToBuffer(TimeCount,Buffer);
			Addr = FRAM_VDR_POWER_OFF_TIME_ADDR;
			FRAM_BufferWrite(Addr,Buffer,4);
		}
	}

	return ENABLE;		
}
/**************************************************************************
//��������VDRPower_Init
//���ܣ��ϵ��ʼ��
//���룺��
//�������
//����ֵ����
//��ע�����ϵ���ϵ��¼����浽��¼��
***************************************************************************/
static void VDRPower_Init(void)
{
	u8	Buffer[VDR_DATA_POWER_STEP_LEN];
	u8	BufferLen;
	u16	Addr;
	u32	TimeCount;
	TIME_T	Time;
	


	Addr = FRAM_VDR_POWER_OFF_TIME_ADDR;
	BufferLen = FRAM_BufferRead(Buffer, 4, Addr);
	if(0 != BufferLen)
	{
		TimeCount = Public_ConvertBufferToLong(Buffer);
		Gmtime(&Time, TimeCount);
		VDRPub_ConvertTimeToBCD(Buffer,&Time);
		Buffer[6] = 2;//1:�ϵ�;2:�ϵ� 
		VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);
	}

	TimeCount = RTC_GetCounter();
	Gmtime(&Time, TimeCount);
	VDRPub_ConvertTimeToBCD(Buffer,&Time);
	Buffer[6] = 1;//1:�ϵ�;2:�ϵ� 
	VDRData_Write(VDR_DATA_TYPE_POWER, Buffer, VDR_DATA_POWER_STEP_LEN-5, TimeCount);	
}
/**************************************************************************
//��������VDRPower_GetStatus
//���ܣ�����״̬���
//���룺��
//�������
//����ֵ��0:����;1:����
//��ע��50ms����1��
***************************************************************************/
static u8 VDRPower_GetStatus(void)
{
	static u8 PowerOnCount = 0;
	static u8 PowerOffCount = 0;
	static u8 PowerStatus = 0;
	
	u16	Ad = 0;

	Ad = Ad_GetValue(ADC_MAIN_POWER); 
	Ad = Ad*33*9/0xfff;//ת����V��ÿbit����0.1V
	if(Ad >= 90)//�ն˹�����ѹ��9~36V
	{
		PowerOffCount = 0;
		PowerOnCount++;
		if(PowerOnCount >= 21)
		{
			PowerOnCount--;
			PowerStatus = 0;
		}
	}
	else
	{
		PowerOnCount = 0;
		PowerOffCount++;
		if(PowerOffCount >= 21)
		{
			PowerOffCount--;
			PowerStatus = 1;
		}
	}

	return PowerStatus;	
}





