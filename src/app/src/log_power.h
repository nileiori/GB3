/*******************************************************************************
 * File Name:			log_power.c 
 * Function Describe:	
 * Relate Module:		�жϡ��洢��ͨ���ϵ��¼
 * Writer:				Joneming
 * Date:				2015-04-01
 * ReWriter:			
 * Date:
 *******************************************************************************/
#include "stm32f10x.h"
#ifndef __LOG__POWER__
#define __LOG__POWER__
/*******************************************************************************
* Function Name  : Power_ParamInitialize
* Description    : ͨ�ϵ����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Power_ParamInitialize(void);
/*******************************************************************************
* Function Name  : Power_GetLowPowerFlag
* Description    : 
* Input          : None
* Output         : None
* Return         :  �ϵ�Ϊ2,ͨ��Ϊ1,0δ֪
*******************************************************************************/
unsigned char Power_GetLowPowerFlag(void);
/*************************************************************
** ��������: CheckMainPowerRun_TimeTask
** ��������: ʵʱ��⣬��¼�ϵ纯��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
FunctionalState CheckMainPowerRun_TimeTask(void);

#endif

/*******************************************************************************
 *                            End of Module
 *******************************************************************************/
