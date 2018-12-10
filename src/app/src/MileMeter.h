/*************************************************************************
*
* Copyright (c) 2008,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : MileMeter.h
* ����     : 
*
* ��ǰ�汾 : 
* ������   : 
* �޸�ʱ�� : 
*
* ��ʷ�汾 : 
* ������   : 
* ���ʱ�� : 
* 
* ��ע :
*************************************************************************/

#ifndef _MileMeter_H
#define _MileMeter_H

#include <math.h>
#include "stm32f10x.h"

#define REarth 6371004  //m
#define PI 3.14159265358979323846 
#define StillFloatDistanceMax 11.5//m
#define StillFloatDistanceAverage 4.80//m
#define StillFloatDistanceAverageExceptMax 4.00//m
#define StillFloatDistanceAverageStandard 3.12//m
 
#define Test 0
typedef double d64;
typedef struct
{
	d64 LonDegree;
	d64 LatDegree;
} Point_Struct;

#if Test
#define N   21


typedef struct
{

	u8  LatitueD; //��
	u8  LatitueM; //��
	u16 LatitueMF; //��С������,��λΪ0.0001��
	u16 LongitueD;
	u8  LongitueM; 
	u16 LongitueMF; //���ȷֵ�С������,��λΪ0.0001��

} Test_Pos_Data;
d64 Max,Average,TwoPointDistanceTest[N-1];

void FloatResult(void);
void MileMeter_DistanceSumTest(Test_Pos_Data *TestGpsData);
void SimulateDistanceSumTest(void);
void MileMeter_Dis(void);
#endif

FunctionalState GpsMile_TimeTask(void);
u32 ReadMileSum(void);
u32 MileMeter_GetTotalMile(void);
u8 ReadGpsPosSpeed(void);
void MileMeter_MinuteSpeed(void);
u8 MileMeter_GetMinuteSpeed(void);
/********************************************************************
* ���� : GpsMile_UpdatePram(void)
* ���� : ����GPS�����
* ��ע: 
********************************************************************/
void GpsMile_UpdatePram(void);
/********************************************************************
* ���� : GpsMile_SetPram(u32 TmpMilege)
* ���� : ����GPS�����
* ��ע: 
********************************************************************/
void GpsMile_SetPram(u32 TmpMilege);
/********************************************************************
* ���� : MileMeter_ClearOldPoint
* ���� : ��һ���������0
* ��ע: 
********************************************************************/
void MileMeter_ClearOldPoint(void);
#endif
















