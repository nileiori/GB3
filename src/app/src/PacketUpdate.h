#ifndef __PACKET_UPDATE_H
#define __PACKET_UPDATE_H

#include "stm32f10x.h"

/**************************************************************************
//��������PacketUpdate_TimeTask
//���ܣ�ʵ�ְַ���������
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע���ְ�������ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState PacketUpdate_TimeTask(void);
/**************************************************************************
//��������PacketUpdate_HandleCmd8108H
//���ܣ�����ƽ̨�·���0x8108ָ��
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע���ְ�������ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
void PacketUpdate_HandleCmd8108H(u8 *pBuffer, u16 BufferLen);


#endif