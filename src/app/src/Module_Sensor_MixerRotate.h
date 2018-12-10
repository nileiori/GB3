#ifndef __MODULE_SENSOR_MIXERROTATE_H_
#define __MODULE_SENSOR_MIXERROTATE_H_


#ifdef ZHENGFANZHUAN

//*************�ļ�����***************
#include "stm32f10x.h"



/***************   �� ������   ********************************/



//��/��ת״̬
#define ROTATE_STATE_STOP       0 //00:ֹͣ
#define ROTATE_STATE_REVERSE    1 //01:��ת
#define ROTATE_STATE_POSITIVE   2 //10:��ת
#define ROTATE_STATE_UNKNOWNED  3 //11:δ֪



//һ����Ͳ�ϰ�װ�ĴŸ�����
#define NUM_MAGNETIC_CYLINDER  2  //Ĭ��Ϊ 2


//���ն�N ��δ���յ��жϴ���ʱ������Ϊ��Ͳֹͣ
#define NUM_SENCOND_NOEXTI 60 //Ĭ��Ϊ60��
/***************   �ṹ�� ������   ********************************/


typedef struct 
{
	u32  oldTime;    //�����µ�ʱ��(��λ:15.625ms)
}TIME_SYSTEM;



typedef struct
{
  
    u8  RevState ;    //����ת����
    u8  RunState:1 ; //����״̬ 0:����  1������
    u8  RevErrorFlag:1 ; //����ת������״̬
    u8  Reserver:4 ; //����ת������״̬  
    
    u8  bMixterStateChange ;
    u8  Mixer_Rotate_Polarity_last;
    u8  Mixer_Rotate_Polarity_current;
    
    
    u8  Rev;        //���賵ת�� 
    u8  PulseCnt;     //�������
    u8  PulseStart; //������ʼ����
    u8  updata; //�Ƿ����
    
    u8  NumMagneticCylinder;

    


    u32   RevSampleTime;          //ת�ټ�ʱ
    u32   CarRunStopCounter;
   
    u32     MixerFailTotal;
    u32     CarRunTimeTotal;
    u32     CarWorkTimeTotal;
    u32     OneLoopTime;                //����ͰתһȦ���ѵ�ʱ�䣬��λ15.625ms   
    
}MIXER_CAR_OBJECT;



/***************   �ⲿȫ�ֱ���������   ********************************/


/***************   �ⲿȫ�ֺ���������   ********************************/



/*******************************************************************************
*  ��������  : u8 MixerRotate_Get_RotateState()
*  ��������  : ��ý��賵��Ͳ��״̬: 00��ֹͣ 01:��ת 10:��ת 11��δ֪
*******************************************************************************/
extern u8 MixerRotate_Get_RotateState(void);


/*******************************************************************************
*  ��������  : u8 MixerRotate_Get_RotateSpeed()
*  ��������  : ��ý��賵��Ͳ��ת��
*******************************************************************************/
extern u8 MixerRotate_Get_RotateSpeed(void);


/*******************************************************************************
*  ��������  : MixerRotate_Task_Init
*  ��������  : ���賵��Ͳ�����ʼ��
*******************************************************************************/
extern void MixerRotate_Task_Init(void);


/*******************************************************************************
*  ��������  : MixerRotate_TimeTask()
*  ��������  : ���賵��Ͳ��ʱ����
*******************************************************************************/
extern FunctionalState MixerRotate_TimeTask(void);


/*******************************************************************************
**  ��������  :	MixerRotate_Exti_Isr
**  ��������  : ���賵����ת������ ���� ����ⲿ�жϷ������
*******************************************************************************/
extern void MixerRotate_Exti_Isr(void);





#endif


#endif
