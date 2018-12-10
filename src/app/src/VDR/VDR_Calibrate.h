#ifndef __VDR_COEFFICIENT_H
#define __VDR_COEFFICIENT_H

//*********文件包含************
#include "stm32f2xx.h"

//*********宏定义**************

//**********函数声明**********
FunctionalState VDRCoefficient_TimeTask(void);//脉冲系数校准任务
void VDRCoefficient_EnableCalibration(void); //使能校准
void VDRCoefficient_DisableCalibration(void); //关闭校准

#endif
