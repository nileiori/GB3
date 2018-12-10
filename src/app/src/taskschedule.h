/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_dma.h
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : This file contains all the functions prototypes for the
*                      DMA firmware library.
********************************************************************************

*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASKSCHEDULE_H
#define __TASKSCHEDULE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//*****************�¼�����**************************
typedef enum 
{
        EV_SEND_CAN_DATA = 0,           // 0:����CAN����
      EV_CANREC,                      // 1:CAN�������ݴ���
        EV_GPS_PARSE,                   // 2:GPS���ݴ���
        EV_NULL,                        // 3:��
        EV_GET_LOGIN_PASSWORD,          // 4: ��ȡ��Ȩ������
        EV_LINK_GOV_SERVER,             // 5: �л����������ƽ̨
        EV_FTP_UPDATA,                  // 6: FTP��������
        EV_CALLING,                     // 7: һ��ͨ��
        EV_SEV_CALL,                    // 8: �������ذε绰����
        EV_RING,                        // 9:�����¼���   
        EV_RING_OVER,                   // 10:ֹͣ����
        EV_SHOW_TEXT,                   // 11:�ı���ʾ
      EV_KEY_PARSE,                   // 12:�а����¼�����
      MAX_EVENTASK

}EVENTTASK;

//******************ʱ������***********************
typedef enum 
{
        TIME_COMMUNICATION = 0,         //0:����ͨ��
        TIME_RADIO_PARSE,               //1:����Э�����
        TIME_IO_MONITOR,                //2:IO ״̬��� 
        TIME_GPSREC,                    //3:GPS���ݴ��� 
        TIME_GPS_MILE,                  //4:GPS��̼���
        TIME_POSITION,                  //5:λ�û㱨 
        TIME_CAMERA,                    //6:����ͷ���� 
        TIME_SEND_MEDIA,                //7:�����ý�����ݵ�ƽ̨
        TIME_HEART_BEAT,                //8:������
        TIME_USART3,                    //9:USART3����3(����ͷ����ʹ��)��ʱ��������
        TIME_RADIO_RESEND,              //10:����Э���ط������꣩
        TIME_DELAY_TRIG,                //11:��ʱ����
        TIME_SYSRESET,                  //12:ϵͳ����
        TIME_BMA220,                    //13:��ײ����
        TIME_LINK1_LOGIN,               //14:����1ע���Ȩ
        TIME_USB,                       //15:USB
        TIME_AD,                        //16:AD����
        TIME_SLEEP,                     //17:����
        TIME_KEY_BOARD,                 //18:����ɨ��
        TIME_MENU,                      //19:LCD������ʾ����ʾ
        TIME_AREA,                      //20:����
        TIME_MEDIA,                     //21:ɾ����ý���ļ�
        TIME_ROUTE,                     //22:��·
        TIME_OIL_CTRL,                  //23:����·
        TIME_ICAUC,                     //24:IC����֤TCP�������� 
      TIME_NVLCD,                     //25: ������ 
      TIME_MULTI_CONNECT,             //26: ����������,��������ʱʹ��
      TIME_GNSS_REAL,                 //27������ʵʱ��������
      TIME_TTSDELAY,                  //28:TTS����
      TIME_ACCOUNT,                   //29:���ٿ���
      TIME_CLOSE_GPRS,                //30:GPRS����
        TIME_PHOTO,                     //31:���������ƵĶ�ʱ������������,
        TIME_SLEEPDEEP,                 //32:�������
      TIME_POLYGON,                   //33:�����
      TIME_UPDATA_RESULT_NOTE,        //34:�����������֪ͨ
      TIME_CORNER,                    //35:�յ㲹��
      TIME_PUBLIC,                    //36:����
      TIME_LINK2_LOGIN,               //37:����2ע���Ȩ
      TIME_BLIND,                       //38:ä������
      TIME_STATUS_ALARM,                //39:״̬�仯��������������ϱ�������
            TIME_VDR_USART,             //40:VDR ���ڽ���
        TIME_VDR_DATA,                  //41:VDR ���ݼ�¼�Ͷ�ȡ
        TIME_VDR_PULSE,                 //42:VDR �����ٶȺ��������
        TIME_VDR_POSITION,              //43:VDR λ����Ϣ
        TIME_VDR_SPEED,                 //44:VDR �ٶȼ�¼
        TIME_VDR_DOUBT,                 //45:VDR �ɵ�����
        TIME_VDR_OVERTIME,              //46:VDR ��ʱ��ʻ
        TIME_VDR_CARD,                  //47:VDR ��ʻ�����
        TIME_VDR_POWER,                 //48:VDR �ⲿ����
        TIME_VDR_SPEED_STATUS,          //49:VDR �ٶ�״̬
        TIME_VDR_OVER_SPEED,            //50:VDR ����
        TIME_VDR_COEFFICIENT,           //51:VDR ����ϵ��У׼
        TIME_VDR_GPRS,                  //52:VDR ���߲ɼ���ʻ��¼������
        TIME_VDR_LOG,                   //53:VDR ��ӡVDR��־��DB9����
        TIME_CAN_SEND,                  //54:can���ݲɼ�
        TIME_LOCK_ONE,                  //55.ʹ��ǰ����һ     
        TIME_LOCK_TWO,                  //56:ʹ��ǰ������
			  TIME_VDR_PACKET_RESEND,         //57:VDR���ݷְ�����
				TIME_PACKET_UPDATE,             // 58:�ְ�����
				TIME_JOINTECHOILCOST,           //59:��ͨ��λ������
				TIME_TEMPERATURE,               //60:�¶Ȳɼ��� 
				TIME_DEEPPLOW,                  //61:����
				TIME_TURNSPEED,                 //62:ũ��ת��
				TIME_VSS,                       //63:VSS�����ж�
				TIME_A2OBD,											//64:A2OBDģ������	
				TIME_ACCELERATION,              //65:�����١������١���ת�䣬����gps�ٶȺ�gps����
        MAX_TIMETASK,   
}TIMERTASK;
/*------------------------ Execute task by timer--------------------------------
Max task is 32. when power on dev this task is init.
Include 
------------------------------------------------------------------------------*/
typedef struct{
  FunctionalState TaskTimerState;       //task valid,1:valid;0:invalid
  u32 TaskTimerCount;           //variables task time count
  u32 TaskExTimer;              //set fixed param by independently funtion module
  FunctionalState (* operate)();     //handle independtly funtion
}TIME_TASK;

typedef struct{
  FunctionalState evTaskOnState;       //task valid,1:valid;0:invalid
  void (*operate)();     //handle independtly funtion
}EVEN_TASK;

/* Private define ------------------------------------------------------------*/



/*******************************************************************************
* Function Name  : InitTimerTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTimerTask(void);

/*******************************************************************************
* Function Name  : TimerTaskScheduler
* Description    : Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerTaskScheduler(void);

/*******************************************************************************
* Function Name  : SetTimerTask
* Description    : Setup timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTimerTask(TIMERTASK task, u32 time);

/*******************************************************************************
* Function Name  : ClrTimerTask
* Description    : Cancel timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrTimerTask(TIMERTASK task);

/*******************************************************************************
* Function Name  : InitEvTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitEvTask(void);
/*******************************************************************************
* Function Name  : SetEvTask
* Description    : Setup even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetEvTask(u8 ev);
/*******************************************************************************
* Function Name  : EvTaskScheduler
* Description    : Schedule even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*******************************************************************************
* Function Name  : ClrEvTask
* Description 	 : Cancel ev task .
* Input 				 : None
* Output				 : None
* Return				 : None
*******************************************************************************/
void ClrEvTask(EVENTTASK task);
/*******************************************************************************
* Function Name  : ResumeTimerTask
* Description    : resume timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResumeTimerTask(TIMERTASK task);
/*******************************************************************************
* Function Name  : SuspendTimerTask
* Description    : suspend timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SuspendTimerTask(TIMERTASK task);
/*******************************************************************************
* Function Name  : ResumeEvTask
* Description    : resume ev task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResumeEvTask(EVENTTASK ev);
/*******************************************************************************
* Function Name  : SuspendEvTask
* Description    : suspend ev task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SuspendEvTask(EVENTTASK task);

void EvTaskScheduler(void);
/*******************************************************************************
* Function Name  : SendCanData_EvTask
* Description    : ����CAN����
* Input          : None
* Output         : None
* Return         : None
* Note           :��Ҫ���͵�CAN���ݷ���RadioProtocolMiddleBuffer,����ΪRadioProtocolMiddleLen
*******************************************************************************/
void SendCanData_EvTask(void);
#endif
