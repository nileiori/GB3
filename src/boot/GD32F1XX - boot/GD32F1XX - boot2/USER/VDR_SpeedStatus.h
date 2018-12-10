#ifndef __VDR_SPEED_STATUS_H
#define __VDR_SPEED_STATUS_H

#include "stm32f10x.h"

#define VDR_SPEED_STATUS_BUFFER_SIZE		VDR_DATA_SPEED_STEP_LEN

#define FRAM_VDR_SPEED_STATUS_ENABLE_ADDR	268//2�ֽڣ�1�ֽڱ�־��1�ֽ�У����

/**************************************************************************
//��������VDRSpeedStatus_TimeTask
//���ܣ��ٶ�״̬��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDRSpeedStatus_TimeTask(void);

#endif
