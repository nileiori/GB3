#ifndef __VDR_PARAMETER_H
#define __VDR_PARAMETER_H

#include "stm32f10x.h"

/**************************************************************************
//��������VDRParameter_Write
//���ܣ���������޸ļ�¼
//���룺EventTime:�¼��޸�ʱ��;EventType:�¼�����
//�������
//����ֵ����
//��ע��Э���������VDRProtocol_ParseCmd����øýӿ�
***************************************************************************/
void VDRParameter_Write(u32 EventTime, u8 EventType);

#endif

