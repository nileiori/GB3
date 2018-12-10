#ifndef __VDR_COEFFICIENT_H
#define __VDR_COEFFICIENT_H

//*********文件包含************
//#include "stm32f2xx.h"
#include "stm32f10x.h"

//*********宏定义**************

//**********函数声明**********
FunctionalState VDRCoefficient_TimeTask(void);//脉冲系数校准任务
void VDRCoefficient_EnableCalibration(void); //使能校准
u8 VDRCoefficient_GetFlag(void); //1:校准中（忙）,0:校准完毕（空闲）
void VDRCoefficient_Init(void);//初始化

#endif
