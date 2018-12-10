/*******************************************************************************
 * File Name:           LzmSys.h
 * Function Describe:   application entry
 * Relate Module:
 * Writer:              joneming
 * Date:                2012-01-12
 *******************************************************************************/
#ifndef _LZMSYS_H_
#define _LZMSYS_H_
//���Ͷ���
/****************************************************/
/*                                                  */
/* Included files                                   */
/*                                                  */
/****************************************************/
#include "stm32f10x.h"
#include "Public.h"
/****************************************************/
/*                                                  */
/* Definitions                                      */
/*                                                  */
/****************************************************/
#define _TASK(task) ((void (*)(void))(task)) 
//////////////��Ϣ������//////////////////////
#define TaskMessage (LZM_GetSutMessage()->message)
#define TaskPara (LZM_GetSutMessage()->para)
#define pMessage LZM_GetSutMessage()
//���Ͷ���													
///////////////��ʱ��������////////////////////////////
#define MENU_SECOND      SECOND//100ms                           
#define SECS(x)         (LZM_TIMER32)((x) * MENU_SECOND) 

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////													
													
/****************************************************/
/*                                                  */
/* Structures                                       */
/*                                                  */
/****************************************************/
//��Ϣ����(��Ϣ��������Ϣ���еõ�,��Ϣ�������ͳ���Ӧ��С�ڶ����������ͳ���)
typedef struct tagLZM_MESSAGE {
    unsigned short message;
    unsigned short para;
    unsigned short para1;
    unsigned short para2;
} LZM_MESSAGE, *PLZM_MESSAGE;

//ʱ�ӽṹ

//ϵͳ�е���Ϣ
enum TASK_MESSAGE
{    
    ///////////////////timer////////////////////////////
    SHOW_TIMER,							//1 timer
    FLASH_TIMER,						//2 timer
    OPERATE_TIMER,						//3 timer
    ///////����ϵͳҲռ��,������//////////////////////////////////////
    REFRESH_TIMER,						//4 timer
    TEST_TIMER,							//5 timer
    MAX_TIMERS,
    ///////////////////message////////////////////////////
    //����Ϣ
    EMPTY_MESSAGE,	                                    //�� LZM_GetMessage()���뵱ǰ������Ϣ
    ///�ж���Ϣ    
    INT_RTC,		                                    //�� InterruptIrqRtc()�����ж϶���
    //������Ϣ
    KEY_PRESS,		                                    //�� LZM_DefTaskProc()�����ж϶���    
    //�����ʼ����Ϣ
    TASK_START,		                                    //�� LZM_StartNewTask()����ϵͳ����
    //������Ϣ    
    //SYS_RECV0,		                                //�� LZM_GetMessage()���뵱ǰ������Ϣ(������InterruptIrqUART0���봮�ڽ������ݶ���)    
    MESSAGE_MAX		
};


/****************************************************/
/*                                                  */
/* Function declarations                            */
/*                                                  */
/****************************************************/
////////////////��ǰ������ָ��//////////////
extern LZM_RET (*LZM_TaskFunction)(LZM_MESSAGE *Msg);
////////////////ǰһ������ָ��//////////////
extern LZM_RET (*LZM_PreTaskFunction)(LZM_MESSAGE *Msg);

///////////////��Ϣ������///////////////////
LZM_RET LZM_TaskEmpty(LZM_MESSAGE *Msg);
LZM_RET LZM_DefTaskProc(LZM_MESSAGE *Msg);
void LZM_TaskMenuStart(void);
LZM_MESSAGE *LZM_GetSutMessage(void);
void Insert_KEYQueue(unsigned char key);
void Insert_STARTQueue(unsigned char param);
void LZM_KeyHandler(void);
void LZM_TimerHandler(void);
//��ʼ������(���µ�ǰ������)	
void LZM_StartNewTask(LZM_RET (*function)(LZM_MESSAGE *Msg));
void LZM_RefuseNowTask(LZM_RET (*function)(LZM_MESSAGE *Msg));
void LZM_RefuseNowTaskEx(LZM_RET (*function)(LZM_MESSAGE *Msg));
void LZM_StartNewTaskEx(LZM_RET (*function)(LZM_MESSAGE *Msg));
//ֻˢ�µ�ǰ����//
void LZM_FreshNowTask(void);

/////////ʱ������//////////////////////
FunctionalState TaskMenuMessage_TimeTask(void);
////////////////////////////////
void LZM_Nothing(void);
void LZM_ReadyNewTask(void);
void LZM_ReturnPreTask(LZM_RET (*Task)(LZM_MESSAGE *Msg));
void LZM_ReturnOldTaskEx(void);
/////////////////////////////////////////////////////////////
void LZM_SetTimer(LZM_TIMER32 index,LZM_TIMER32 t);
void LZM_SetAlarm(LZM_TIMER32 index,LZM_TIMER32 t);
void LZM_KillTimer(LZM_TIMER32 index);
void LZM_SetOperateTimer(void (*function)(void),LZM_TIMER32 time);
LZM_TIMER32 LZM_TimerGetEnabled(LZM_TIMER32 index);

/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif//_LZMSYS_H_
/*******************************************************************************
 *                            End of Module
 *******************************************************************************/
