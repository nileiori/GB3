#ifndef __VDR_CARD_H
#define __VDR_CARD_H

#include "stm32f10x.h"
#include "VDR.h"

#define VDR_CARD_BUFFER_SIZE	150

#define CARD_STATE_MONITOR   	0
#define CARD_STATE_READ   		1
#define CARD_STATE_LOGIN   		2
#define CARD_STATE_LOGOUT   	3

#define CARD_READ_START   	0
#define CARD_READ_24XX   		1
#define CARD_READ_4442   		2
#define CARD_STATE_LOGOUT   	3

/**************************************************************************
//��������VDRCard_TimeTask
//���ܣ�ˢ��ǩ�����ο�ǩ��
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��0.1���ӽ���1�Σ������������Ҫ���ô˺�����
//ע���������Ҫ�ڳ�ʱ��ʻ��ʱ������ɳ�ʼ������ܿ���
***************************************************************************/
FunctionalState VDRCard_TimeTask(void);

#endif
