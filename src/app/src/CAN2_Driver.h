/*************************************************************************
* Copyright (c) 2013,�������������¼����������޹�˾
* All rights reserved.
* �ļ����� : CAN2_Driver.h
* ��ǰ�汾 : 1.0
* ������   : Shigle
* �޸�ʱ�� : 20113��6��10��
*************************************************************************/
#include "my_typedef.h"

#ifndef _CAN2_DRIVER_H_
#define _CAN2_DRIVER_H_



/********************************************************************
* ���� : CAN2_Init
* ���� : CAN2 ��ʼ��
********************************************************************/
extern void CAN2_Init(void);


/********************************************************************
* ���� : CAN2_ID_Filter_Set
* ���� : CAN2 ����ID������
* ���� :  FilterNum: ����ID����ţ������ظ� �� ȡֵ��Χ ����1~13
           filterId: ����ID , ��28λΪ��Ч��IDλ�� ����λ����Ϊ0 
       filterMaskId: ��������λ�� 1: ��������  0:����
********************************************************************/
extern void CAN2_ID_Filter_Set(u8 FilterNum, u32 filterId, u32 filterMaskId);


//����ID0x0102//CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ� 

extern void UpdataOnePram_SamplePeriod_CAN2( void );

//����ID0x0103//CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�

extern void UpdataOnePram_UploadPeriod_CAN2( void );

//����ID0x0110//CAN ����ID �����ɼ�����BYTE[8]

extern void UpdataOnePram_Sample_Single_CAN2( void );


//==========================================================================

#endif
