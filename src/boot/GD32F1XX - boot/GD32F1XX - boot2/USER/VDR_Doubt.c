/************************************************************************
//�������ƣ�VDR_Doubt.c
//���ܣ��ɵ����ݹ���
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "fm25c160.h"
#include "EepromPram.h"
#include "Rtc.h"
#include "stm32f10x_rtc.h"
/********************���ر���*************************/
static 	u8 	CarRunFlag = 0;//����ͣʻ����ʻ��־��1Ϊ��ʻ��0Ϊͣʻ��
static 	u8 	VdrDoubtInitFlag = 0;//��ʼ����־��1Ϊ�ѳ�ʼ����0Ϊδ��ʼ����
static	u8 	VdrDoubtBuffer[VDR_DATA_DOUBT_STEP_LEN];//�ɵ����ݻ���
static 	u16 	VdrDoubtCount = 0;//�ɵ����ݵ�����¼
static	u8 	VdrDoubtTestBuffer[300];//�ɵ����ݻ���
/********************ȫ�ֱ���*************************/
u16	PowerShutValue = 50;//??5.0V

/********************�ⲿ����*************************/


/********************���غ�������*********************/
static u16 VDRDoubt_Read(u8 *pBuffer, u16 OffsetPoint);
static void VDRDoubt_WritePositionNoChange(void);
static void VDRDoubt_WritePowerOff(void);
static void VDRDoubt_WritePowerOn(void);
static void VDRDoubt_WriteStop(void);
static void VDRDoubt_SaveCurrentSpeedStatus(void);
static void VDRDoubt_RunOrStop(void);

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
			VDRDoubt_WritePowerOn();//�����ϵ�����ʱ���ɵ����ݣ��൱��û�б���Ķϵ��ɵ�����
			VdrDoubtInitFlag = 1;
		}
	}
	else
	{
		VDRDoubt_RunOrStop();//��ʻ��ͣʻ���ж�

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

	u8	Speed;

	Speed = VDRPulse_GetInstantSpeed();

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
//��������VDRDoubt_SaveCurrentSpeedStatus
//���ܣ����浱ǰ���ٶȺ�״̬
//���룺��
//�������
//����ֵ����
//��ע��0.2��1��
***************************************************************************/
static void VDRDoubt_SaveCurrentSpeedStatus(void)
{
	static u8 Count1 = 0;
	static u8 Count2 = 0;

	u8	Status;
	u8	Buffer[10];
	u16	WriteAddr;

	Count1++;
	if(Count1 >= (2*SYSTICK_0p1SECOND))
	{
		Count1 = 0;

		Status = 0;
//		Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:ɲ��
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:��ת
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:��ת
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:Զ��
	//	Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:����
		//bit2-bit1δ����
	//	Status |= Io_ReadExtCarStatusBit(STATUS_BIT_ACC);//bit0:ACC

		Buffer[0] = VDRPulse_GetInstantSpeed();
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

		
		Count2++;
		if(Count2 >= 5)//ÿ���¼һ��ʱ��
		{
			Count2 = 0;
			VDRPub_ConvertNowTimeToBCD(Buffer);
			WriteAddr = FRAM_VDR_DOUBT_TIME_ADDR;
			FRAM_BufferWrite(WriteAddr,Buffer,6);//��¼��ǰʱ��
		}
		
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
	
	u8	RunFlag;
	u16	length;
	u32	Time;
	s16	i;
	s16	OffsetPoint = 50;
	s16	j;
	u8	flag;
	
	RunFlag = VDRDoubt_GetCarRunFlag();

	if((1 == LastRunFlag)&&(0 == RunFlag))//����ʻ��Ϊͣʻ
	{
		length = VDRDoubt_Read(VdrDoubtBuffer,50);	
		if(0 != length)
		{
			//�Ƚ�����洢���������ڴ�洢�������Ƿ�һ��
			flag = 0;
			i = VdrDoubtCount;
			if(i >= OffsetPoint)
			{
				i -= OffsetPoint;
			}
			else
			{
				i = i+150;
				i -= OffsetPoint;
			}
			i--;
			if(i < 0)
			{
				i = 149;
			}
			for(j=0;j<100;j++)
			{
				if((VdrDoubtBuffer[24+2*j] == VdrDoubtTestBuffer[2*i])&&
					(VdrDoubtBuffer[24+2*j+1] == VdrDoubtTestBuffer[2*i+1]))
				{
					
				}
				else
				{
					flag = 1;
					break;
				}
				i--;
				if(i < 0)
				{
					i = 149;
				}
			}
			if(0 == flag)
			{
				Time = RTC_GetCounter();
				VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
			}
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
	u8	Buffer[10];
	u8	BufferLen;
	u16	i;
	u16	Addr;
	u16	length;
	TIME_T	Time;
	u32	TimeCount;

	BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//����һ�μ�¼�ĵ�λ��
	if(0 != BufferLen)
	{
		VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
		length = VDRDoubt_Read(VdrDoubtBuffer,0);	
		if(0 != length)
		{
			Public_ConvertBcdToValue(Buffer, VdrDoubtBuffer, 6);
			Time.year = 2000+Buffer[0];
			Time.month = Buffer[1];
			Time.day = Buffer[2];
			Time.hour = Buffer[3];
			Time.min = Buffer[4];
			Time.sec = Buffer[5];
			if(1 == Public_CheckTimeStruct(&Time))
			{
				TimeCount = ConverseGmtime(&Time);
				VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
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
	static u8 LastPowerFlag = 0;//0Ϊ�ϵ磬1Ϊ�ϵ�
	
	u8	PowerFlag;
	u16	length;
	u16	Ad = 0;
	u32	Time;

	if(0 == VDRDoubt_GetCarRunFlag())//ͣʻ
	{
		return ;
	}

//	Ad = Ad_GetValue(ADC_MAIN_POWER);//��ȡ�����ѹ
//	Ad = Ad*33*9/0xfff;//ת����0.1VΪ��λ��ֵ
	Ad = 120;

	if(Ad <= PowerShutValue)
	{
		PowerFlag = 1;
	}
	else
	{
		PowerFlag = 0;
	}

	if((0 == LastPowerFlag)&&(1 == PowerFlag))//���ϵ��Ϊ����
	{
		length = VDRDoubt_Read(VdrDoubtBuffer,0);	
		if(0 != length)
		{
			Time = RTC_GetCounter();
			VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
		}

	}
	
	LastPowerFlag = PowerFlag;
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
	static 	u8 		LastNoChangeFlag = 0;//0Ϊ�б仯��1Ϊû�б仯
	static  u16		Count = 0;
	static 	GPS_STRUCT 	LastPosition;//��һ�����λ����Ϣ

	u8	NoChangeFlag;
	u16	length;
	u32	Time;
	GPS_STRUCT 	Position;//��ǰ���λ����Ϣ

	if(0 == VDRDoubt_GetCarRunFlag())//ͣʻ
	{
		return ;
	}

	//if(0 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))//����λ
   // 	{
    //    	return;
   //	}

	NoChangeFlag = 0;
	//Gps_CopygPosition(&Position);//��ȡ��Чλ����Ϣ
	if(((Position.Latitue_D   == LastPosition.Latitue_D)
       	    &&(Position.Latitue_F  == LastPosition.Latitue_F))
            &&(Position.Latitue_FX == LastPosition.Latitue_FX))  
	{
		Count++;
		if(Count >= 10*SECOND)
		{
			Count--;
			NoChangeFlag = 1;
		}
	}
	else
	{
		Count = 0;
	}

	if((0 == LastNoChangeFlag)&&(1 == NoChangeFlag))//��λ����Ϣ�б仯��Ϊλ����Ϣ�ޱ仯
	{
		length = VDRDoubt_Read(VdrDoubtBuffer,0);	
		if(0 != length)
		{
			Time = RTC_GetCounter();
			VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
		}

	}
	
	LastNoChangeFlag = NoChangeFlag;
}
/**************************************************************************
//��������VDRDoubt_Read
//���ܣ����¹��ɵ�����
//���룺OffsetPoint:ƫ�Ƶ���
//�����pBuffer:ָ�����ݻ��壻
//����ֵ����ȡ�����ݳ���
//��ע���������ݷ��ϱ�A.22Ҫ���ɵ����ݹ�150���㣬0.2��һ���㣬��¼�����30������ݣ�
//����ȡ����ͣ��ʱ���ɵ�����ʱ��OffsetPointȡֵΪ50����Ϊ��10���ͣ���жϣ�Ҫƫ��10�룬��50���㣩
//����ȡ�ϵ��λ���ޱ仯ʱ���ɵ�����ʱOffsetPointȡֵΪ0����û��ƫ�ơ�
***************************************************************************/
static u16 VDRDoubt_Read(u8 *pBuffer, u16 OffsetPoint)
{
	u8	*p = NULL;
	u8	Buffer[3];
	u8	BufferLen;
	u8	count;
	u8	flag;
	u16	Addr;
	u16	length;
	s16	i;
	s16	j;

	p = pBuffer;//����ʱ�䣬6�ֽڣ�BCD��
	Addr = FRAM_VDR_DOUBT_TIME_ADDR;
	length = FRAM_BufferRead(p,6,Addr);
	if(0 == length)
	{
		return 0;
	}
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
	if(i >= OffsetPoint)
	{
		i -= OffsetPoint;
	}
	else
	{
		i = i+150;
		i -= OffsetPoint;
	}
	i--;
	if(i < 0)
	{
		i = 149;
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
				if(count >= 50)//������ʻ��
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
	//Public_GetCurPositionInfoDataBlock(p);

	return 234;
	
}






