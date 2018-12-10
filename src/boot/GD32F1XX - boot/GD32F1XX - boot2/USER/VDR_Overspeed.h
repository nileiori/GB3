#ifndef __VDR_OVERSPEED_H
#define __VDR_OVERSPEED_H

#include "stm32f10x.h"

typedef struct
{
	u8	EarlyAlramSpeed;//Ԥ���ٶȣ���λkm/h����Ӧ����ID 0x005B��������ʱ����Ϊ50���ɡ�
	u32	EarlyAlramKeeptime;//Ԥ������ʱ�䣬��λ�룬��Ӧ����ID 0xF250��������ʱ����Ϊ0���ɡ�
	u8	EarlyAlarmVoiceBuffer[30];//Ԥ����������Ӧ����ID 0xF251��
	u32	EarlyAlarmVoiceGroupTime;//Ԥ������ÿ����ʾ�������λ���ӣ���Ӧ����ID 0xF252��������ʱ����Ϊ5���ɡ�
	u32	EarlyAlarmVoiceNum;//Ԥ������ÿ����ʾ��������Ӧ����ID 0xF253��������ʱ����Ϊ3���ɡ�
	u32	EarlyAlarmVoiceNumTime;//Ԥ������ÿ����ʾ�������λ�룬��Ӧ����ID 0xF254

	u8	AlarmSpeed;//�����ٶȣ���λkm/h����Ӧ����ID 0x0055
	u32	AlarmKeeptime;//��������ʱ�䣬��λ�룬��Ӧ����ID 0x0056��������ʱ����Ϊ0���ɡ�
	u8	AlarmVoiceBuffer[30];//������������Ӧ����ID 0xF255
	u32	AlarmVoiceGroupTime;//��������ÿ����ʾ�������λ���ӣ���Ӧ����ID 0xF256��������ʱ����Ϊ5���ɡ�
	u32	AlarmVoiceNum;//��������ÿ����ʾ��������Ӧ����ID 0xF257��������ʱ����Ϊ3���ɡ�
	u32	AlarmVoiceNumTime;//��������ÿ����ʾ�������λ�룬��Ӧ����ID 0xF258
}VDR_OVERSPEED_STRUCT;

/**************************************************************************
//��������VDROverspeed_TimeTask
//���ܣ����ٱ�����Ԥ��
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDROverspeed_TimeTask(void);
/**************************************************************************
//��������VDROverspeed_ReadAlarmFlag
//���ܣ������ٱ�����־�ֽ�
//���룺��
//�������
//����ֵ����
//��ע��bit0��ʾ��ͨ���٣�bit1��ʾ�����٣�bit2��ʾ��·���٣�
//bit3��ʾҹ����ͨ���٣�bit4��ʾҹ�������٣�bit5��ʾҹ����·����
//bit6,bit7����
***************************************************************************/
u8 VDROverspeed_ReadAlarmFlag(void);


#endif


