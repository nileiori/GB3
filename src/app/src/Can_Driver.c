/************************************************************************
//�������ƣ�Can_Driver.c
//���ܣ���ģ��ʵ��CAN�ӿڣ�CAN1�����ݵ��շ�
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2014.12
//�汾��¼���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include <string.h>

#include "Can_Driver.h"
#include "queue.h"
#include "rtc.h"
#include "Public.h"
#include "RadioProtocol.h"
#include "Can_App.h"
#include "EepromPram.h"

/********************���ر���*************************/

	
/********************ȫ�ֱ���*************************/
Queue CanRxQ;
u8 CanRxBuffer[CAN_RX_BUFFER_SIZE] = {0};

/********************�ⲿ����*************************/
extern CAN_FILTER_STRUCT CanFilter[];
extern u32 Can1GatherTime;	//CAN1 �ɼ�ʱ������Ϊ0��ʾ���ɼ�
extern u16 Can1UploadTime;	//CAN1 �ϴ�ʱ������Ϊ0��ʾ���ϴ�
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
/********************���غ�������*********************/
static void Can_1_Init(void);
static void Can_1_Baud_Init(void);
static void Can_1_Mask_Init(void);

/********************��������*************************/
/**************************************************************************
//��������Can_Init
//���ܣ�ʵ��CAN�ӿڳ�ʼ��
//���룺��
//�������
//����ֵ����
//��ע������CAN1��CAN2�ĳ�ʼ������������CAN2û��ʹ�ã�����ֻ��CAN1�ģ�
//�ú�����Ҫ��CANӦ����������ǰ��һ����CANӦ������͵���
***************************************************************************/
void Can_Init(void)
{
	/*
	u8 Buffer[10];
	
	Buffer[0] = 0x00;
  Buffer[1]	= 0x00;
  Buffer[2] = 0x00;
  Buffer[3] =	0x00;
  Buffer[4]	= 0x58;
  Buffer[5] =	0xF8;
  Buffer[6] =	0x1E;
  Buffer[7] =	0x9E;
	EepromPram_WritePram(E2_CAN_SET_ONLY_GATHER_0_ID,Buffer,E2_CAN_SET_ONLY_GATHER_LEN);
	
	Buffer[0] = 0x00;
  Buffer[1]	= 0x00;
  Buffer[2] = 0x00;
  Buffer[3] =	0x64;
  Buffer[4]	= 0x58;
  Buffer[5] =	0xFF;
  Buffer[6] =	0xD1;
  Buffer[7] =	0x17;
	EepromPram_WritePram(E2_CAN_SET_ONLY_GATHER_1_ID,Buffer,E2_CAN_SET_ONLY_GATHER_LEN);
	*/
	Can_1_Init();//CAN 1��ʼ��
	
	
}
/**************************************************************************
//��������Can_1_Rx_Isr
//���ܣ�ʵ��CAN1�����жϴ���
//���룺��
//�������
//����ֵ����
//��ע���жϷ���������ô˺���
***************************************************************************/
void Can_1_Rx_Isr(void)
{
	CanRxMsg Rx;
	u8 Buffer[CAN_STEP_LEN_MAX+1]={0};
	u8 length;
	u32 temp;
	TIME_T time;

		
	CAN_Receive(CAN1,CAN_FIFO0,&Rx);

	if((Rx.IDE == CAN_ID_STD)&&(8 == Rx.DLC))
	{
		if(0 == Can_SoftwareFilter(0,Rx.StdId,0))
		{
			RTC_GetCurTime(&time);//�����б�Э��0x0705�����5�ֽ�ʱ�䣬�������Ϊ0
			Buffer[1] = Public_HEX2BCD(time.hour);
			Buffer[2] = Public_HEX2BCD(time.min);
			Buffer[3] = Public_HEX2BCD(time.sec);
			Buffer[4] = 0;
			Buffer[5] = 0;
			
			temp = Rx.StdId;//4�ֽ�ID��bit31��ʾCANͨ���ţ�0ΪCAN1��1ΪCAN2��bit30��ʾ֡���ͣ�0Ϊ��׼֡��1Ϊ��չ֡��
			Public_ConvertLongToBuffer(temp,Buffer+6);//bit29��ʾ���ݲɼ���ʽ��0Ϊԭʼֵ��1Ϊƽ��ֵ��bit28~bit0��ʾ֡ID

			memcpy(Buffer+10,&Rx.Data[0],8);//8�ֽ�����

			length = TransMean(Buffer+1,17);//ת��

			Buffer[0] = 0x7e;//����֡ͷ
			
			Buffer[length+1] = 0x7e;//����֡β

			if(0 != Can1UploadTime)
			{
				QueueInBuffer(Buffer,length+2,CanRxQ);
			}

		}
			
	}
	else if((Rx.IDE == CAN_ID_EXT)&&(8 == Rx.DLC))
	{
		if(0 == Can_SoftwareFilter(0,Rx.ExtId,1))
		{
			RTC_GetCurTime(&time);//�����б�Э��0x0705�����5�ֽ�ʱ�䣬�������Ϊ0
			Buffer[1] = Public_HEX2BCD(time.hour);
			Buffer[2] = Public_HEX2BCD(time.min);
			Buffer[3] = Public_HEX2BCD(time.sec);
			Buffer[4] = 0;
			Buffer[5] = 0;
			 
			if(1 == BBGNTestFlag)//dxl,2016.5.21���ƽ̨��bug,����չ֡ʱ���ܽ�bit30��λ
			{
			    temp = Rx.ExtId;
			}
			else
			{
			    temp = Rx.ExtId|(1 << 30);//4�ֽ�ID��bit31��ʾCANͨ���ţ�0ΪCAN1��1ΪCAN2��bit30��ʾ֡���ͣ�0Ϊ��׼֡��1Ϊ��չ֡��
			}
			Public_ConvertLongToBuffer(temp,Buffer+6);//bit29��ʾ���ݲɼ���ʽ��0Ϊԭʼֵ��1Ϊƽ��ֵ��bit28~bit0��ʾ֡ID

			memcpy(Buffer+10,&Rx.Data[0],8);//8�ֽ�����

			length = TransMean(Buffer+1,17);//ת��

			Buffer[0] = 0x7e;//����֡ͷ
			
			Buffer[length+1] = 0x7e;//����֡β

			QueueInBuffer(Buffer,length+2,CanRxQ);
			
		}

	}
	else
	{
		//������

	}

}

/**************************************************************************
//��������Can_1_Gpio_Init
//���ܣ�CAN1 GPIO��ʱ�ӳ�ʼ��������CANģ���Դ��������
//���룺��
//�������
//����ֵ����
//��ע����
***************************************************************************/
static void Can_1_Init(void)
{

	NVIC_InitTypeDef    NVIC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//dxl,2016.5.21,�����������ΪGPIO_Mode_IPU
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//dxl,2016.5.21���� 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	AFIO->MAPR |= GPIO_Remap2_CAN1;	
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	Can_1_Baud_Init();//CAN1�����ʳ�ʼ����250KHZ
	Can_1_Mask_Init();//CAN1�����ʼ��

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//��CAN�Ľ����ж�

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	CAN_1_POWER_ON();
	

	
}
/**************************************************************************
//��������Can_1_Baud_Init
//���ܣ�CAN1�����ʳ�ʼ��
//���룺��
//�������
//����ֵ����
//��ע����
***************************************************************************/
static void Can_1_Baud_Init(void)
{
	u32 BaudRate;
	CAN_InitTypeDef	CAN_InitStructure;

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);

	CAN_DeInit(CAN1);

	CAN_StructInit(&CAN_InitStructure);

	BaudRate = CAN_1_BAUDRATE;

	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = ENABLE;//�Զ�����ģʽ��Ӳ����⵽CAN����ʱ�Զ��뿪����ģʽ
	CAN_InitStructure.CAN_NART = ENABLE;//���Զ��ش���ģʽ
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = ENABLE;//����FIFO���ȼ�
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//CAN������==PCLK1/[(1+CAN_BS1+CAN_BS2)*CAN_Prescaler]
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	if(BaudRate <= 500000)
	{
		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
		CAN_InitStructure.CAN_Prescaler = 36000000/(BaudRate*16);//PCLK1ʱ��Ϊ36000000,
	}
	else if(BaudRate <= 800000)
	{
		CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
		CAN_InitStructure.CAN_Prescaler = 36000000/(BaudRate*10);
	}
	else
	{
		CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
		CAN_InitStructure.CAN_Prescaler = 36000000/(BaudRate*8);
	}

	CAN_Init(CAN1,&CAN_InitStructure);
}
/**************************************************************************
//��������Can_1_Mask_Init
//���ܣ�CAN1�����ʼ��
//���룺��
//�������
//����ֵ����
//��ע��ֻ��������ID�ı���ͨ��
***************************************************************************/
static void Can_1_Mask_Init(void)
{
	//��Ӳ�����빦��
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	CAN_FilterInitStructure.CAN_FilterNumber = 1;      //ָ���˴���ʼ���Ĺ����������ķ�Χ��1��13
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdMask;//����������ʼ��Ϊ��ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit;//�����˹�����λ��
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;//(u16)(filterId>>16); //�����趨��������ʶ��(32λλ��ʱΪ��߶�λ)
	CAN_FilterInitStructure.CAN_FilterIdLow  = 0x0000;//(u16)(filterId&0xFFFF);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//(u16)(filterMaskId>>16); //�����趨���������α�ʶ�����߹�������ʶ��(32λλ��ʱΪ��߶�λ)
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0x0000;//(u16)(filterMaskId&0xFFFF);  //��ӦλΪ 0:���Ƚ�; 1:����ƥ��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; //ͨ�����˲�����Ϣ����������FIFO0��
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;    //ʹ��/ʧ�� ������
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	
}
/**************************************************************************
//��������CAN_1_ID_Filter
//���ܣ�CAN1 ID��������
//���룺��
//�������
//����ֵ����
//��ע��IdType,0:��׼֡��1:��չ֡
***************************************************************************/
void CAN_1_ID_Filter(u8 FilterNum, u32 FilterId, u32 FilterMaskId, u8 IdType)
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	CAN_FilterInitStructure.CAN_FilterNumber = FilterNum;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((FilterId << 3)&0xFFFF0000) >> 16;
	if(0 == IdType)
	{
		CAN_FilterInitStructure.CAN_FilterIdLow = (FilterId << 3)&0xFFFF;
	}
	else
	{
		CAN_FilterInitStructure.CAN_FilterIdLow = ((FilterId << 3)|0x0004)&0xFFFF;
	}
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((FilterMaskId << 3)&0xFFFF0000) >> 16;
	if(0 == IdType)
	{
		CAN_FilterInitStructure.CAN_FilterMaskIdLow = (FilterMaskId << 3)&0xFFFF;	
	}
	else
	{
		CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((FilterMaskId << 3)|0x0004)&0xFFFF;
	}
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}



























