/*******************************************************
Copyright (C),E-EYE Tech. Co.,Ltd.
Author: WYF
Version:
Finish Date:
Description:
History: 
    1,Date:
      Author:
      Modification:
    2,
*******************************************************/
#include "my_typedef.h"

#ifndef _ERRORANDTIME_H_
#define _ERRORANDTIME_H_

//�궨��

//����ṹ����
typedef enum
{
    E_NO_ERR=0,                              //�޴�
    E_TTS_UART_OVERTIME,                     //TTS ���ڳ�ʱ
    E_TTS_UART_CMD_ERR,                      //����֡
}E_ERROR;

//ʱ�ӽṹ
typedef struct
{
    u32 Counter;	                    //��ʱʱ�� ����ʱ��Ϊ����ʱ��
    u32 Interval;	                    //��װ��ֵ
    u8  OverTimeF;                      //��ʱ��־
}T_TIMER; 

//��ʱ\��ʱ��                        
typedef enum
{                 
TTS_POWERON_TIMER=0,//TTS�ϵ���ʱ
TTS_POWEROFF_TIMER, //TTS�ص���ʱ
TTS_UART_RECV_TIMER,//TTS���ڽ��ն�ʱ
TTS_STATUS_TIMER,   //�ȴ����ж�ʱ   

SEARCH_FILE_TIMER,  //�����ļ���ʱ     

TIME_TEST_TIMER,
TIMER_SUM           //��ʱ������
}E_CAM_TIME;

//ʱ������ṹ
typedef struct{
    u8 TaskTimerState;       //task,1:valid;0:invalid
    u8 (*operate)(void);    //handle independtly funtion
}TIMES_TASK;

//ʱ������ID����
typedef enum 
{
    TIME_TTS=0,          
    TTS_SEND,
	TTS_UART_REC_TIMEOUT,
    MAX_TIMETASK_SUM,
}E_TIME_TASK;



//ȫ�ֱ�������

//��������
void AddError(u8 ErrNo) ; //��Ӵ����
void TimerHandle(void);  //ʱ�Ӵ�����
u8 IsOverTime(u8 Index); //�ж��Ƿ�ʱ
void StopTimer(u8 Index) ; //ֹͣ��ʱ
u8 GetOverTimeStatus(u8 Index) ; //��ó�ʱ״̬
void SetOnceTimer(u8 Index,u32 Value);//���õ��ζ�ʱ
void SetPeriodTimer(u8 Index,u32 CntValue, u32 ItvValue);
///FunctionalState TTSDelayTask(void) ; //��������,dxl
FunctionalState TTS_TimeTask(void) ;
void TTS_Init(void) ; //�豸��ȫ�ֱ�����ʼ��
void TimerTaskSch(void) ; //����
void EnTimerTask(u8 TaskID) ; //��������
void DisTimerTask(u8 TaskID) ; //��ֹ����

#endif
