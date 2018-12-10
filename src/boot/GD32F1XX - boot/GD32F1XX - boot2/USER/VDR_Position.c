/************************************************************************
//�������ƣ�VDR_Position.c
//���ܣ�360hλ����Ϣ��¼
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "Rtc.h"
#include "spi_flash_app.h"
#include "spi_flash.h"
#include "stm32f10x_rtc.h"

/********************���ر���*************************/
static u8	VdrPositionBuffer[VDR_DATA_POSITION_STEP_LEN] = {0};

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static void VDRPosition_Init(void);
static void VDRPosition_SaveStartTime(void);
static void VDRPosition_SavePosition(u8 EnableFlag,u8 Offset);
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
FunctionalState VDRPosition_TimeTask(void)
{
	static 	u32	LastTimeCount;
	static u8	AlreadyRunFlag = 0;//1Ϊ����ʻ����0Ϊδ��ʻ��
	static u8	InitFlag = 0;
	static 	TIME_T	LastTime;

	u32	CurrentTimeCount;
//	u8	*p = NULL;
	u8	RunFlag;
	TIME_T	CurrentTime;

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
				RunFlag = VDRDoubt_GetCarRunFlag();
				if(1 == RunFlag)
				{
					AlreadyRunFlag = 1;
				}
				RTC_ReadTime(&CurrentTime);
				if(CurrentTime.min != LastTime.min)//���Ӷ���
				{
					VDRPosition_SaveStartTime();
					
					VDRPosition_SavePosition(AlreadyRunFlag,CurrentTime.min);
					
					if(59 == CurrentTime.min)//59��ʱ�����ݣ�60���ӵ�λ����Ϣ��ƽ���ٶȣ����ݴ���ת���¼��
					{	
						VDRPosition_Write();
					}
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
//��������VDRPosition_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ��0:δ��ɳ�ʼ����1:����ɳ�ʼ��
//��ע���ϵ�����λ����Ϣ�ݴ������б�Ҫ��ת������¼��
***************************************************************************/
static void VDRPosition_Init(void)
{
	u32	StartTimeCount;
	u32	Addr;
	u16	i;
	u8	flag;
	TIME_T 	StartTime;
	TIME_T 	CurrentTime;

	
	Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_BufferRead(VdrPositionBuffer, Addr, 666);//λ����Ϣ�鳤����666
	flag = 0;
	for(i=0; i<666; i++)
	{
		if(VdrPositionBuffer[i] != 0xff)
		{
			flag = 1;
			break;
		}
	}
	if(0 == flag)
	{
		return ;
	}
	
	RTC_ReadTime(&CurrentTime);
	VDRPub_ConvertBCDToTime(&StartTime,VdrPositionBuffer);
	if(1 == Public_CheckTimeStruct(&StartTime))
	{
		if((StartTime.day == CurrentTime.day)&&(StartTime.hour == CurrentTime.hour))
		{
				//�ϴε���ͱ����ϵ�������ͬһ��Сʱ�ڣ������洢����
		}
		else if(StartTime.hour != CurrentTime.hour)
		{
			StartTime.min = 0;
			StartTime.sec = 0;
			StartTimeCount = ConverseGmtime(&StartTime);
			Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
			SPI_FLASH_BufferRead(VdrPositionBuffer, Addr, 666);//��ȡ�ݴ������
			
			VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, StartTimeCount);//���浽��¼��
			
			SPI_FLASH_SectorErase(Addr);//��������
		}
		else
		{
			SPI_FLASH_SectorErase(Addr);
		}
	}
	else
	{
		SPI_FLASH_SectorErase(Addr);	
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
	u32	Addr;
	u8	Buffer[20];
	u8	i;
	u8	flag;
	TIME_T	Time;
	TIME_T	StartTime;
	
	Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_BufferRead(Buffer, Addr, 6);
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
		RTC_ReadTime(&Time);
		if(1 == Public_CheckTimeStruct(&Time))
		{
			Time.min = 0;
			Time.sec = 0;
			VDRPub_ConvertTimeToBCD(Buffer,&Time);
			SPI_FLASH_BufferWrite(Buffer, Addr, 6);
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
					//SPI_FLASH_SectorErase(Addr);//��������
			}
		}
		else
		{
			//SPI_FLASH_SectorErase(Addr);//��������
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
static void VDRPosition_SavePosition(u8 EnableFlag,u8 Offset)
{
	u32	Addr;
	u8	flag;
	u8	i;
	u8	Buffer[20];
	
	Addr = 6+FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES+11*Offset;
	SPI_FLASH_BufferRead(Buffer, Addr, 11);
	flag = 0;
	for(i=0; i<11; i++)
	{
		if(0xff != Buffer[i])
		{
			flag = 1;
			break;
		}
	}
	if((0 == flag)&&(1 == EnableFlag))//�ݴ�ÿ����λ����Ϣ��ƽ���ٶ�
	{
					//	if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
					//	{
							//Public_GetCurPositionInfoDataBlock(Buffer);//10�ֽ�
							Buffer[10] = VDRSpeed_GetMinuteSpeed();
					//	}
						//else
						//{
							//memset(Buffer,0xff,10);//��Чλ��
							//Buffer[0] = 0x7F;
							//Buffer[4] = 0x7F;
							//Buffer[10] = VDRSpeed_GetMinuteSpeed();
						//}
			SPI_FLASH_BufferWrite(Buffer, Addr, 11);
	}
	else 
	{
			//�ݲ�����
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
	u32	Addr;
	u32	TimeCount;
	u8	Buffer[10];
	
	TIME_T	Time;
	
	Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_BufferRead(Buffer, Addr, 6);
					
	VDRPub_ConvertBCDToTime(&Time,Buffer);
	if(1 == Public_CheckTimeStruct(&Time))
	{
						
		TimeCount = ConverseGmtime(&Time);
		Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;
		SPI_FLASH_BufferRead(VdrPositionBuffer, Addr, 666);//��ȡ�ݴ������
		VDRData_Write(VDR_DATA_TYPE_POSITION, VdrPositionBuffer, VDR_DATA_POSITION_STEP_LEN-5, TimeCount);//���浽��¼��
	}
	SPI_FLASH_SectorErase(Addr);//��������
}






