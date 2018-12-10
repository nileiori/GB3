
#include "stm32f10x.h"
#include "rtcformat.h"
#ifndef __POSITION__
#define __POSITION__

/**************************�ⲿ����***********************************************/
/*******************************************************************************
* Function Name  : Position_TimeTask
* Description    : λ����Ϣ�ɼ��¼�����,ÿ1min����һ�� .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState Position_TimeTask(void);  
/*******************************************************************************
* Function Name  : Position_ParamInitialize
* Description    : λ����Ϣ�ɼ��¼�����,ÿ1min����һ�� .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Position_ParamInitialize(void);
/*************************************************************
** ��������: Position_ReadBackPositionRecordData
** ��������: ���������ȡָ��ʱ����ٶȼ�¼
** ��ڲ���: ��ʼʱ��startTime
** ���ڲ���: pBuffer�������ݵ��׵�ַ
** ���ز���: pBuffer�ֽڳ���
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned short Position_ReadBackPositionRecordData(unsigned char *pBuffer,unsigned long startTime);
/*************************************************************
** ��������: Position_ReadCurPositionRecordData
** ��������: ���������ȡָ��ʱ����ٶȼ�¼
** ��ڲ���: ��ʼʱ��StartTime,����ʱ��EndTime
** ���ڲ���: pBuffer�������ݵ��׵�ַ
** ���ز���: pBuffer�ֽڳ���
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned short Position_ReadCurPositionRecordData(unsigned char *pBuffer,TIME_T StartTime,TIME_T EndTime);
/*************************************************************
** ��������: Position_ReadPositionRecordData
** ��������: ��ȡָ��ʱ��Ρ�ָ��������ݿ��ٶȼ�¼����
** ��ڲ���: ��ʼʱ��StartTime,����ʱ��EndTime,MaxBlockָ��������ݿ�
** ���ڲ���: pBuffer�������ݵ��׵�ַ
** ���ز���: pBuffer�ֽڳ���
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned short Position_ReadPositionRecordData(u8 *pBuffer, TIME_T StartTime, TIME_T EndTime, u16 MaxBlock);

#endif
