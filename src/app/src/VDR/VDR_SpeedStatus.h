#ifndef __VDR_SPEED_STATUS_H
#define __VDR_SPEED_STATUS_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"

#define VDR_SPEED_STATUS_BUFFER_SIZE		VDR_DATA_SPEED_STEP_LEN



/**************************************************************************
//��������VDRSpeedStatus_TimeTask
//���ܣ��ٶ�״̬��¼
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDRSpeedStatus_TimeTask(void);
/**************************************************************************
//��������VDRSpeedStatus_GetFlag
//���ܣ���ȡ�ٶ�״̬��־
//���룺��
//�������
//����ֵ��0:����;1:�쳣
//��ע��
***************************************************************************/
u8 VDRSpeedStatus_GetFlag(void);

#endif
