#ifndef __VDR_POWER_H
#define __VDR_POWER_H

#include "stm32f10x.h"

#define FRAM_VDR_POWER_OFF_TIME_ADDR	263//5�ֽڣ�4�ֽ�ʱ�䣬1�ֽ�У����

/**************************************************************************
//��������VDRPower_TimeTask
//���ܣ��ϵ��ϵ��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��50ms����1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDRPower_TimeTask(void);

#endif
