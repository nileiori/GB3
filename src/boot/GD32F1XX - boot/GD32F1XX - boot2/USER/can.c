/*************************************************************************
*
* Copyright (c) 2008,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : HandleCan.c
* ����     : CAN���ߵ�����(��չ֡ģʽ)
*
* ��ǰ�汾 :1.0.1
* ������   :xiezm
* �޸�ʱ�� :2009.5.9
* �޸����� : 1.�޸�CAN���߿�GPIOA11/12
*
* �����汾 : 1.0
* ������   : zhulin
* ����ʱ�� : 2009��4��25��
*
* ��ע     : 1.������� ռ��ʽϵͳ���ȷ�ʽ,��Ҫ�޸� HandleCan_send2Bus ��
*              HandleCan_receFromBus ��������.
*            2.���͵�������䵽CAN�Ĵ�����ķ�ʽ,Ҫ�޸�.(�ж���)
*            3.CAN���߳�����û��ʵ��.

*************************************************************************/
#include  <string.h>
#include "Can.h"
#include "stm32f10x_can.h"

//------------- ȫ�ֱ��� --------------------------------------------
CAN_ISR_DATA    gCanIsrData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//CAN_REC_DATA Can_RecBuff[CAN_MAX_RECE_NUM];

void CanBus_SetIdentifie(u32 filterId, u32 filterMaskId);
void CANBus_Init(void);
void CanBus_SetBaudRate(u32 BandRateBuf);
void CanBus_SetIdentifie2(u32 filterId, u32 filterMaskId);
void CANBus_Init2(void);
void CanBus_SetBaudRate2(u32 BandRateBuf);
/********************************************************************
* ���� : CanBusInit
* ���� : ��ʼ�� CAN������.
*
* ���� : memFlag : SYSTEM_MEN_ERROR:�洢����ֵ������(����,����...);
*                  SYSTEM_MEN_OK:�洢��û�б�����(�����λ)
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.ֻ������Ȩģʽ������???
*       2.�ж����ȼ� Ҫ����;
*       3.CANʱ����CAN�˿ڸ����й�,ֻ����Ӧ��IO�˿�
*       4.�˿ڸ�����Ӳ���й�,ÿ��Ӳ����·�嶼Ҫ�����Ӧ������.(��������ʱ���Ƿ���Ҫ��)
********************************************************************/
void CanHwInit(void)
{
	NVIC_InitTypeDef    tempNVIC_InitStructure;
	GPIO_InitTypeDef    tempGPIO_InitStructure;

	//--------- ʱ�ӳ�ʼ�� --------------------------
	//ʹ��/ʧ�� APB1,APB2����ʱ�� (CANʹ�ø��ù���,�������ù���ʱ��)
	  /* GPIOA and GPIO_LED clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	  /* CAN Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//--------- ����IO�˿ڳ�ʼ�� ----------------------------

	  //���ܸ�������
	  /* Configure CAN pin: RX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);
	  /* Configure CAN pin: TX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);

	/* ��ӳ��CAN1��IO�� */
	AFIO->MAPR |= GPIO_Remap2_CAN1;	//��ӳ�� Weite

	//----------�жϳ�ʼ�� ---------------------------------------
	  /* Enable CAN RX0 interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;		//USB_LP_CAN_RX0_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

	/* Enable CAN TX interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;		//USB_HP_CAN_TX_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

		//------ ���� ���ò���, ��ʼ��CAN���ߴ����һЩ���� ------
	CANBus_Init();
	/* CAN register init */

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//��CAN�Ľ����ж�

	//-----------------CAN��Դ����-----------------Weite
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);

	CAN_PWR_ON();	//��CAN��Դ  Weite
	
}
/********************************************************************
* ���� : CanBusInit
* ���� : ��ʼ�� CAN������.
*
* ���� : memFlag : SYSTEM_MEN_ERROR:�洢����ֵ������(����,����...);
*                  SYSTEM_MEN_OK:�洢��û�б�����(�����λ)
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.ֻ������Ȩģʽ������???
*       2.�ж����ȼ� Ҫ����;
*       3.CANʱ����CAN�˿ڸ����й�,ֻ����Ӧ��IO�˿�
*       4.�˿ڸ�����Ӳ���й�,ÿ��Ӳ����·�嶼Ҫ�����Ӧ������.(��������ʱ���Ƿ���Ҫ��)
********************************************************************/
void CanHwInit2(void)
{
	NVIC_InitTypeDef    tempNVIC_InitStructure;
	GPIO_InitTypeDef    tempGPIO_InitStructure;

	//--------- ʱ�ӳ�ʼ�� --------------------------
	//ʹ��/ʧ�� APB1,APB2����ʱ�� (CANʹ�ø��ù���,�������ù���ʱ��)
	  /* GPIOA and GPIO_LED clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	  /* CAN Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//--------- ����IO�˿ڳ�ʼ�� ----------------------------

	  //���ܸ�������
	  /* Configure CAN pin: RX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &tempGPIO_InitStructure);
	  /* Configure CAN pin: TX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &tempGPIO_InitStructure);

	/* ��ӳ��CAN1��IO�� */
	//AFIO->MAPR |= GPIO_Remap2_CAN1;	//��ӳ�� Weite

	//----------�жϳ�ʼ�� ---------------------------------------
	  /* Enable CAN RX0 interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;		//CAN2_RX0_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

	/* Enable CAN TX interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;		//CAN2_TX_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

		//------ ���� ���ò���, ��ʼ��CAN���ߴ����һЩ���� ------
	CANBus_Init2();
	/* CAN register init */

	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);//��CAN�Ľ����ж�

	//-----------------CAN��Դ����-----------------Weite
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);

	CAN_PWR_ON();	//��CAN��Դ  Weite
	
}

/********************************************************************
* ���� : Can_Isr_Tx
* ���� : CAN���߷��ͷ�����.
*
* ���� : ��
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void Can_Isr_Tx(void)
{
	CanTxMsg  TxMessage;
	u8     *sendBufAddr;

	if((gCanIsrData.sendLen != 0) && (gCanIsrData.sendIndex < gCanIsrData.sendLen))
	{
		sendBufAddr = (u8*)&gCanIsrData.sendBuffer[gCanIsrData.sendIndex];
		//��׼֡�����ô˲��� //TxMessage.StdId = ((u32)sendBufAddr[0] << 6) | ((u32)sendBufAddr[1] >> 2);
		//TxMessage.ExtId = ((u32)sendBufAddr[0] << 24) | ((u32)sendBufAddr[1] << 16) | ((u32)sendBufAddr[2] << 8) | ((u32)sendBufAddr[3]);

#if (CUSTUMTYPE != 0x10)
		memcpy((u8*)&TxMessage.ExtId, sendBufAddr, 4);
		TxMessage.IDE   = CAN_ID_EXT;//��չ֡
#else
    memcpy((u8*)&TxMessage.StdId, sendBufAddr, 4);//StdId
		TxMessage.IDE   = CAN_ID_STD;//CAN_ID_EXT;//��չ֡
#endif

		TxMessage.RTR   = CAN_RTR_DATA;//����֡
		TxMessage.DLC   = sendBufAddr[4] & 0x0F;
		memcpy(TxMessage.Data, &sendBufAddr[5], 8);

		if(CAN_Transmit(CAN1, &TxMessage) != CAN_NO_MB)
		{
			gCanIsrData.sendIndex ++;
			if(gCanIsrData.sendIndex >= gCanIsrData.sendLen)
			{
				gCanIsrData.sendLen = 0;
				CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE); //�رշ����ж�
			}
		}
	}
	else
	{
		gCanIsrData.sendLen = 0;
		CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE); //�رշ����ж�
	}
}
/********************************************************************
* ���� : Can_Isr_Tx
* ���� : CAN���߷��ͷ�����.
*
* ���� : ��
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void Can_Isr_Tx2(void)
{
	CanTxMsg  TxMessage;
	u8     *sendBufAddr;

	if((gCanIsrData.sendLen != 0) && (gCanIsrData.sendIndex < gCanIsrData.sendLen))
	{
		sendBufAddr = (u8*)&gCanIsrData.sendBuffer[gCanIsrData.sendIndex];
		//��׼֡�����ô˲��� //TxMessage.StdId = ((u32)sendBufAddr[0] << 6) | ((u32)sendBufAddr[1] >> 2);
		//TxMessage.ExtId = ((u32)sendBufAddr[0] << 24) | ((u32)sendBufAddr[1] << 16) | ((u32)sendBufAddr[2] << 8) | ((u32)sendBufAddr[3]);

#if (CUSTUMTYPE != 0x10)
		memcpy((u8*)&TxMessage.ExtId, sendBufAddr, 4);
		TxMessage.IDE   = CAN_ID_EXT;//��չ֡
#else
    memcpy((u8*)&TxMessage.StdId, sendBufAddr, 4);//StdId
		TxMessage.IDE   = CAN_ID_STD;//CAN_ID_EXT;//��չ֡
#endif

		TxMessage.RTR   = CAN_RTR_DATA;//����֡
		TxMessage.DLC   = sendBufAddr[4] & 0x0F;
		memcpy(TxMessage.Data, &sendBufAddr[5], 8);

		if(CAN_Transmit(CAN2, &TxMessage) != CAN_NO_MB)
		{
			gCanIsrData.sendIndex ++;
			if(gCanIsrData.sendIndex >= gCanIsrData.sendLen)
			{
				gCanIsrData.sendLen = 0;
				CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE); //�رշ����ж�
			}
		}
	}
	else
	{
		gCanIsrData.sendLen = 0;
		CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE); //�رշ����ж�
	}
}

/********************************************************************
* ���� : Can_Isr_Rx
* ���� : CAN���߽��շ�����.
*
* ���� : ��
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void Can_Isr_Rx(void)
{
	CanRxMsg  RxMessage;
	u8     i,num;

	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	num = gCanIsrData.receIndex;

	if(RxMessage.IDE == CAN_ID_STD)
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.StdId), 4);
	}
	else
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.ExtId), 4);
	}
	i = 4;
	gCanIsrData.receBuffer[num][i] = ((RxMessage.IDE | RxMessage.RTR) << 4) | (RxMessage.DLC);
	i++;
	memcpy((u8*)&gCanIsrData.receBuffer[num][i],RxMessage.Data, 8);

	//ѭ������
	gCanIsrData.receIndex++;
	if(gCanIsrData.receIndex >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receIndex = 0;
	}

	//����δ������
	gCanIsrData.receNum++;
	if(gCanIsrData.receNum >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receNum = CAN_MAX_RECE_NUM;
		gCanIsrData.UnreadIndex = gCanIsrData.receIndex;
	}
	else
	{
		gCanIsrData.UnreadIndex = 0;
	}
}
/********************************************************************
* ���� : Can_Isr_Rx
* ���� : CAN���߽��շ�����.
*
* ���� : ��
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void Can_Isr_Rx2(void)
{
	CanRxMsg  RxMessage;
	u8     i,num;

	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	num = gCanIsrData.receIndex;

	if(RxMessage.IDE == CAN_ID_STD)
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.StdId), 4);
	}
	else
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.ExtId), 4);
	}
	i = 4;
	gCanIsrData.receBuffer[num][i] = ((RxMessage.IDE | RxMessage.RTR) << 4) | (RxMessage.DLC);
	i++;
	memcpy((u8*)&gCanIsrData.receBuffer[num][i],RxMessage.Data, 8);

	//ѭ������
	gCanIsrData.receIndex++;
	if(gCanIsrData.receIndex >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receIndex = 0;
	}

	//����δ������
	gCanIsrData.receNum++;
	if(gCanIsrData.receNum >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receNum = CAN_MAX_RECE_NUM;
		gCanIsrData.UnreadIndex = gCanIsrData.receIndex;
	}
	else
	{
		gCanIsrData.UnreadIndex = 0;
	}
}

/********************************************************************
* ���� : CanBus_Send
* ���� : ������ ͨ��CAN �������ݵ����� ����Դ.
*
* ���� : dataAddr : ���͵����ݵ�ַ;
*        dataLen  : ���͵����ݳ���;
*        type     : ��������;  0:data[0]����С�ڵ���7;   1(��0):data[0]����Ϊ������;
* ��� : err      : 0:�ɹ�; 1:��Դ(����)����; 2:���ݳ��ȴ���;
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.������� ռ��ʽϵͳ���ȷ�ʽ,���ڵ��ô˺���ǰ��Ҫ ��/�� �ж�.
********************************************************************/
u8 CanBus_Send(u8 *dataAddr, u32 dataLen)
{
	assert_param((dataAddr != NULL));

	//��������CAN��������,������ݲ�����300�ֽ�
	if((dataLen == 0) || (dataLen > CAN_MAX_SEND_LEN*13))
	{
		return(2);
	}
	if(gCanIsrData.sendLen != 0)	//�����������ڷ���
	{
		return(1);
	}

	memcpy((u8*)&gCanIsrData.sendBuffer[0][0], dataAddr, dataLen);
	//��Ϣ���ǰ�13���ֽ�Ϊ��λ���͵�.
	gCanIsrData.sendLen   = (dataLen+12)/13;//����������"ȥβ��",����,������Ϣ��(sendLen)��,����[0..(sendLen-1)].
	gCanIsrData.sendIndex = 0;

	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE); //�򿪷����ж�  //((u32)0x00000001)
	Can_Isr_Tx();

	return(0);
}
/********************************************************************
* ���� : CanBus_Send
* ���� : ������ ͨ��CAN �������ݵ����� ����Դ.
*
* ���� : dataAddr : ���͵����ݵ�ַ;
*        dataLen  : ���͵����ݳ���;
*        type     : ��������;  0:data[0]����С�ڵ���7;   1(��0):data[0]����Ϊ������;
* ��� : err      : 0:�ɹ�; 1:��Դ(����)����; 2:���ݳ��ȴ���;
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.������� ռ��ʽϵͳ���ȷ�ʽ,���ڵ��ô˺���ǰ��Ҫ ��/�� �ж�.
********************************************************************/
u8 CanBus_Send2(u8 *dataAddr, u32 dataLen)
{
	assert_param((dataAddr != NULL));

	//��������CAN��������,������ݲ�����300�ֽ�
	if((dataLen == 0) || (dataLen > CAN_MAX_SEND_LEN*13))
	{
		return(2);
	}
	if(gCanIsrData.sendLen != 0)	//�����������ڷ���
	{
		return(1);
	}

	memcpy((u8*)&gCanIsrData.sendBuffer[0][0], dataAddr, dataLen);
	//��Ϣ���ǰ�13���ֽ�Ϊ��λ���͵�.
	gCanIsrData.sendLen   = (dataLen+12)/13;//����������"ȥβ��",����,������Ϣ��(sendLen)��,����[0..(sendLen-1)].
	gCanIsrData.sendIndex = 0;

	CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); //�򿪷����ж�  //((u32)0x00000001)
	Can_Isr_Tx2();

	return(0);
}

/********************************************************************
* ���� : CanBus_Rece
* ���� : ������ ��CAN���߽��յ�������.
*
* ���� : dataAddr : ����(taskId)����װ���ݵĵ�ַ;
*        maxLen   : ����(taskId)����װ���ݵ����ռ�;
*        err      : 0:�ɹ�; 1:����ʱ�������; 2:û���յ��µ�����;
* ��� : receLen  : �յ������ݰ�����;
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.������� ռ��ʽϵͳ���ȷ�ʽ,���ڵ��ô˺���ǰ��Ҫ ��/�� �ж�.
********************************************************************/
u8 CanBus_GetData(u8 *dataAddr)
{
	u8   readindex;
	u16  isrLen;

	assert_param((dataAddr != NULL));
	assert_param((maxLen > 0));

	isrLen = gCanIsrData.receNum;
	readindex = gCanIsrData.UnreadIndex;
	while(gCanIsrData.receNum > 0)
	{
		memcpy(dataAddr, (u8*)&gCanIsrData.receBuffer[readindex][0], 13);//readindex, cpyLen,
		dataAddr += 13;
		if(readindex >= CAN_MAX_RECE_NUM)
			readindex = 0;
		gCanIsrData.receNum--;
	}

	gCanIsrData.UnreadIndex = 0;
	gCanIsrData.receIndex = 0;
	gCanIsrData.receNum = 0;

	return(isrLen);
}

/********************************************************************
* ���� : CANBus_Init
* ���� : ���� ���ò���, ��ʼ��CAN���ߴ����һЩ����.
*
* ���� : ��
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.�򿪵��ж����ͻ�Ҫ�޸ģ�Ҫ����������Ϣ���ж�???
********************************************************************/
void CANBus_Init(void)
{
	/* CAN register init */
	CAN_DeInit(CAN1);

	CanBus_SetBaudRate(250000L);//����CAN���ߵ� ������(250KHz).

	CanBus_SetIdentifie(0x00, 0x00);//����CAN���ߵ� �˲�����.

	gCanIsrData.UnreadIndex = 0;
	gCanIsrData.receIndex = 0;
	gCanIsrData.receNum = 0;
}
/********************************************************************
* ���� : CANBus_Init
* ���� : ���� ���ò���, ��ʼ��CAN���ߴ����һЩ����.
*
* ���� : ��
* ��� : ��
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.�򿪵��ж����ͻ�Ҫ�޸ģ�Ҫ����������Ϣ���ж�???
********************************************************************/
void CANBus_Init2(void)
{
	/* CAN register init */
	CAN_DeInit(CAN2);

	CanBus_SetBaudRate2(250000L);//����CAN���ߵ� ������(250KHz).

	CanBus_SetIdentifie(0x00, 0x00);//����CAN���ߵ� �˲�����.

	gCanIsrData.UnreadIndex = 0;
	gCanIsrData.receIndex = 0;
	gCanIsrData.receNum = 0;
}

/********************************************************************
* ���� : CanBus_SetBaudRate
* ���� : ����CAN���ߵ� ������.
*
* ���� : BandRateBuf  : ���õ� ������.(Hz)
* ��� : ��.
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.
********************************************************************/
void CanBus_SetBaudRate(u32 BandRateBuf)
{
	CAN_InitTypeDef        CAN_InitStructure;

	CAN_StructInit(&CAN_InitStructure);
	 //------- CAN_MCR ------------
	CAN_InitStructure.CAN_TTCM = DISABLE; //ʹ��/ʧ�� ʱ�䴥��ͨѶģʽ.0:ʱ�䴥��ͨ��ģʽ�ر�;
	CAN_InitStructure.CAN_ABOM = DISABLE;  //ʹ��/ʧ�� �Զ����߹���.    1:һ����ص�128��11����������λ,�Զ��˳�����״̬;
	CAN_InitStructure.CAN_AWUM = ENABLE;  //ʹ��/ʧ�� �Զ�����ģʽ.    1:Ӳ����⵽CAN����ʱ�Զ��뿪����ģʽ;
	CAN_InitStructure.CAN_NART = ENABLE; //ʹ��/ʧ�� ���Զ��ش���ģʽ.0:CANӲ������ʧ�ܺ��һֱ�ط�ֱ�����ͳɹ�;
	CAN_InitStructure.CAN_RFLM = DISABLE; //ʹ��/ʧ�� �ܽ���FIFO����ģʽ.0:����FIFO����,��һ������ı��Ľ�����ǰһ��;
	CAN_InitStructure.CAN_TXFP = ENABLE;  //ʹ��/ʧ�� ����FIFO���ȼ�.    1:�ɷ��������˳��(ʱ���Ⱥ�˳��)���������ȼ�.
	 //------- CAN_BTR ------------
	CAN_InitStructure.CAN_Mode =CAN_Mode_Normal ;//CANӲ������������ģʽ,CAN_Mode_LoopBack,CAN_Mode_Normal
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;     //����ͬ����Ծ���1��ʱ�䵥λ

	//CAN������: PCLK1 /  [(1 + CAN_BS1 + CAN_BS2) * CAN_Prescaler]
	//  Ki = (1 + CAN_BS1) / (1 + CAN_BS1 + CAN_BS2)
	//���⾡���ܵİѲ���������Ϊ CiA �Ƽ���ֵ��
	//                           Ki=75%     when ������ > 800K
	//                           Ki=80%     when ������ > 500K
	//                           Ki=87.5%   when ������ <= 500K
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;   //CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 9;//(������:250K, PCLK1:36MHz, CAN_BS1:CAN_BS1_13tq, CAN_BS2:CAN_BS2_2tq, CAN_Prescaler:9)

//	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
//	if(BandRateBuf <= 500000L)         //BandRateBuf <= 500KHz
//	{
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/16;
//	} else if(BandRateBuf <= 800000L){ //500KHz < BandRateBuf <= 800KHz
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/10;
//	} else {                           //800KHz < BandRateBuf
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/8;
//	}

	CAN_Init(CAN1, &CAN_InitStructure);
}
/********************************************************************
* ���� : CanBus_SetBaudRate
* ���� : ����CAN���ߵ� ������.
*
* ���� : BandRateBuf  : ���õ� ������.(Hz)
* ��� : ��.
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*       1.
********************************************************************/
void CanBus_SetBaudRate2(u32 BandRateBuf)
{
	CAN_InitTypeDef        CAN_InitStructure;

	CAN_StructInit(&CAN_InitStructure);
	 //------- CAN_MCR ------------
	CAN_InitStructure.CAN_TTCM = DISABLE; //ʹ��/ʧ�� ʱ�䴥��ͨѶģʽ.0:ʱ�䴥��ͨ��ģʽ�ر�;
	CAN_InitStructure.CAN_ABOM = DISABLE;  //ʹ��/ʧ�� �Զ����߹���.    1:һ����ص�128��11����������λ,�Զ��˳�����״̬;
	CAN_InitStructure.CAN_AWUM = ENABLE;  //ʹ��/ʧ�� �Զ�����ģʽ.    1:Ӳ����⵽CAN����ʱ�Զ��뿪����ģʽ;
	CAN_InitStructure.CAN_NART = ENABLE; //ʹ��/ʧ�� ���Զ��ش���ģʽ.0:CANӲ������ʧ�ܺ��һֱ�ط�ֱ�����ͳɹ�;
	CAN_InitStructure.CAN_RFLM = DISABLE; //ʹ��/ʧ�� �ܽ���FIFO����ģʽ.0:����FIFO����,��һ������ı��Ľ�����ǰһ��;
	CAN_InitStructure.CAN_TXFP = ENABLE;  //ʹ��/ʧ�� ����FIFO���ȼ�.    1:�ɷ��������˳��(ʱ���Ⱥ�˳��)���������ȼ�.
	 //------- CAN_BTR ------------
	CAN_InitStructure.CAN_Mode =CAN_Mode_Normal ;//CANӲ������������ģʽ,CAN_Mode_LoopBack,CAN_Mode_Normal
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;     //����ͬ����Ծ���1��ʱ�䵥λ

	//CAN������: PCLK1 /  [(1 + CAN_BS1 + CAN_BS2) * CAN_Prescaler]
	//  Ki = (1 + CAN_BS1) / (1 + CAN_BS1 + CAN_BS2)
	//���⾡���ܵİѲ���������Ϊ CiA �Ƽ���ֵ��
	//                           Ki=75%     when ������ > 800K
	//                           Ki=80%     when ������ > 500K
	//                           Ki=87.5%   when ������ <= 500K
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;   //CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 9;//(������:250K, PCLK1:36MHz, CAN_BS1:CAN_BS1_13tq, CAN_BS2:CAN_BS2_2tq, CAN_Prescaler:9)

//	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
//	if(BandRateBuf <= 500000L)         //BandRateBuf <= 500KHz
//	{
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/16;
//	} else if(BandRateBuf <= 800000L){ //500KHz < BandRateBuf <= 800KHz
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/10;
//	} else {                           //800KHz < BandRateBuf
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/8;
//	}

	CAN_Init(CAN2, &CAN_InitStructure);
}
/********************************************************************
* ���� : CanBus_SetIdentifie
* ���� : ����CAN���ߵ� �˲�����.
*
* ���� : filterId     : ���� ��������ʶ��
*        filterMaskId : ���� ���������α�ʶ��
* ��� : ��.
*
* ȫ�ֱ���:
* ���ú���:
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void CanBus_SetIdentifie(u32 filterId, u32 filterMaskId)
{
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

