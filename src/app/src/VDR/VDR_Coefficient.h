#ifndef __VDR_COEFFICIENT_H
#define __VDR_COEFFICIENT_H

//*********�ļ�����************
//#include "stm32f2xx.h"
#include "stm32f10x.h"

//*********�궨��**************

//**********��������**********
FunctionalState VDRCoefficient_TimeTask(void);//����ϵ��У׼����
void VDRCoefficient_EnableCalibration(void); //ʹ��У׼
u8 VDRCoefficient_GetFlag(void); //1:У׼�У�æ��,0:У׼��ϣ����У�
void VDRCoefficient_Init(void);//��ʼ��

#endif
