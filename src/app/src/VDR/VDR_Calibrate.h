#ifndef __VDR_COEFFICIENT_H
#define __VDR_COEFFICIENT_H

//*********�ļ�����************
#include "stm32f2xx.h"

//*********�궨��**************

//**********��������**********
FunctionalState VDRCoefficient_TimeTask(void);//����ϵ��У׼����
void VDRCoefficient_EnableCalibration(void); //ʹ��У׼
void VDRCoefficient_DisableCalibration(void); //�ر�У׼

#endif
