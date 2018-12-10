/*******************************************************************************
 * File Name:			OilControl.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2012-12-05
 * Rewriter:
 * Date:		
 *******************************************************************************/
#include "stm32f10x.h"

#ifndef __OILCONTROL_H_
#define __OILCONTROL_H_  

/*******************************************************************************
**  ��������  : ControlOil_TimeTask
**  ��������  : ��·����������Ƚӿں���
**  ��    ��  : ��				
**  ��    ��  : �������״̬:  ʹ�ܻ��ֹ
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
FunctionalState ControlOil_TimeTask(void);
/*******************************************************************************
**  ��������  : OilControl_ParameterInitialize
**  ��������  : ����·��ʼ��
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_ParameterInitialize(void);
/*******************************************************************************
**  ��������  : OilControl_OpenOil
**  ��������  : ͨ��
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_OpenOil(void);
/*******************************************************************************
**  ��������  : OilControl_OffOil
**  ��������  : ����
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_OffOil(void);
#endif//__OILCONTROL_H_
/*******************************************************************************
 *                             end of module
 *******************************************************************************/