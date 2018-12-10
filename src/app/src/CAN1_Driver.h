/*************************************************************************
* Copyright (c) 2013,�������������¼����������޹�˾
* All rights reserved.
* �ļ����� : CAN1_Driver.c
* ��ǰ�汾 : 1.0
* ������   : Shigle
* �޸�ʱ�� : 20113��6��10��
*************************************************************************/

#ifndef _CAN1_DRIVER_H_
#define _CAN1_DRIVER_H_


#include "stm32f10x.h"







//==========================================================================
extern void CAN1_Init(void);

extern  void Can_Isr_Tx(void);

/********************************************************************
* ���� : CAN1_ID_Filter_Set
* ���� : CAN1 ����ID������
* ���� :  FilterNum: ����ID����ţ������ظ� �� ȡֵ��Χ ����1~13
           filterId: ����ID , ��28λΪ��Ч��IDλ�� ����λ����Ϊ0 
       filterMaskId: ��������λ�� 1: ��������  0:����
********************************************************************/
extern void CAN1_ID_Filter_Set(u8 FilterNum, u32 filterId, u32 filterMaskId);


 //����ID0x0100//CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ� 

extern void UpdataOnePram_SamplePeriod_CAN1( void );


//����ID0x0101//CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�

extern void UpdataOnePram_UploadPeriod_CAN1( void );


//����ID0x0110//CAN ����ID �����ɼ�����BYTE[8]
  

//extern void UpdataOnePram_Sample_Single_CAN1( void );




u8 CanBus_Send(u8 *dataAddr, u32 dataLen);
extern u8 CanBus_GetData( u8 iCANChannel , u8 *pdata );


//==========================================================================

#endif
