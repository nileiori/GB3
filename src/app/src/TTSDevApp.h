/*******************************************************
Copyright (C),E-EYE Tech. Co.,Ltd.
Author: WYF
Version:V1.0
Finish Date:
Description:TTSЭ�����
History: 
    1,Date:
      Author:
      Modification:
    2,
    
��ע��
    1,��෢����������500BYTES
    2,������෢����������180BYTES
    3,����˼·,��׼������,�ٴ򿪵�Դ,�ȴ��ȶ�����
    
*******************************************************/
#ifndef _TTSDEVAPP_H_
#define _TTSDEVAPP_H_

#include "stm32f10x.h"

u8 PlayMulTTSVoiceStr(u8 *StrPtr) ;      //�ֶβ�������
u8 PlayMulTTSVoiceAlarmStr(u8 *StrPtr);

void SetTtsVolumeIncr(void);
void SetTtsVolumeDecr(void);
u8 GetTtsVolume(void);
void TtsVolumeInit(void);
//-----------------------------------------------------------------------------------
//����:�ֶβ�������
//���������buff �ַ���,len����
//          type    ��ͨ��Ϣ�򱨾���Ϣ
//���أ�
//-----------------------------------------------------------------------------------
u8 PlayMulTTSVoiceBuff(u8 *buff,u16 len,unsigned char type);              //������������
/********************************************************************
* ��   �� : TTS_GetPlayBusyFlag
* ��   �� : ȡ�ò���æ��־
* ��   �� : ��
* ��   �� : ���ڲ�������1, ������Ϸ���0
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  
* ��ע:
********************************************************************/
unsigned char TTS_GetPlayBusyFlag(void);
/************************************************************
��������: 
��������: ��������
��ڲ���: 
���ڲ���:
��ע: ÿ20ms����һ��
************************************************************/ 
FunctionalState TTS_TimeTask(void);
/************************************************************
��������: 
��������: �豸��ȫ�ֱ�����ʼ��
��ڲ���: 
���ڲ���:
************************************************************/
void TTS_Init(void);

#endif