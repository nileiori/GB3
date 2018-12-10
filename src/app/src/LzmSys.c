/*******************************************************************************
 * File Name:           LzmSys.c
 * Function Describe:   application entry
 * Relate Module:
 * Writer:              joneming 
 * Date:                2012-01-12
 *******************************************************************************/

/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2004-09-16
** Last Version:        1.0
** Descriptions:        The main() function example template
**
**------------------------------------------------------------------------------------------------------
** Created by:          Chenmingji
** Created date:        2004-09-16
** Version:             1.0
** Descriptions:        The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:         joneming
** Modified date:       2009-01-17
** Version:             
** Descriptions:        �µļܹ�(������ϵͳ,��ϵͳ����֧�ֶ����񲢷���������۴��е�ִ��һ������)
**                      ��3����ת���û�������mainִ�У���main����ɣ� 
**                          a.���Ի���Ӳ���豸��  
**                          b.��ʼ�������ģ�飻 
**                          c.������ѭ��������ѭ���������ø�ģ��Ĵ�����
********************************************************************************************************/
/****************************************************/
/*                                                  */
/* Included files                                   */
/*                                                  */
/****************************************************/ 
#include "include.h"
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/ 

/****************************************************/
/*                                                  */
/* Global variables                                 */
/*                                                  */
/****************************************************/
//��ǰִ��������ָ��
LZM_RET (*LZM_TaskFunction)(LZM_MESSAGE *Msg);
//ǰһִ��������ָ��  
LZM_RET (*LZM_PreTaskFunction)(LZM_MESSAGE *Msg);

void (*LZM_OperateFunction)(void);                          //
/****************************************************/
/*                                                  */
/* Local Variables                                  */
/*                                                  */
/****************************************************/
//��Ϣ�ṹ
static LZM_MESSAGE s_sutMessage;    
//ʱ���б�
static LZM_TIMER  s_stLzmTimers[MAX_TIMERS];


/****************************************************/
/*                                                  */
/* Forward Declaration of local functions           */
/*                                                  */
/****************************************************/

/*****************************************************
*
*       Private  code
*
*****************************************************/

/*************************************************************OK
** ��������: LZM_DevInitHook()
** ��������: �˿ڼ��ⲿ�豸�ĳ�ʼ��
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static void LZM_DevInitHook(void)
{
    LZM_QueueInit();                                            //���г�ʼ��
}

/************************************************************
** ��������: LZM_TaskInitHook()
** ��������: ���г�ʼ��,���������ʼ��,��ʼ����
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
static void LZM_TaskInitHook(void)
{
    LZM_TaskFunction = LZM_TaskEmpty;                    //�޵�ǰ����
    LZM_PreTaskFunction = LZM_TaskEmpty;                 //��ǰһ����

    ////////////////////
    PublicSysParamterInit();
    /////////////////////////
    LZM_StartNewTask(TaskShowLogo);                    //��ʼ����   
}

/************************************************************
** ��������: LZM_GetMessage()
** ��������: ����Ϣ���л�ȡ��Ϣ
** ��ڲ���: ��Ϣ�ṹָ��
** ���ڲ���:
************************************************************/ 
LZM_RET LZM_GetMessage(LZM_MESSAGE *Msg)
{
    if(!Empty_INTQueue())                                  //�ж���Ϣ
    {
        Msg->message = Delete_INTQueue();
        return 1;
    }
    else
    if(!Empty_SYSQueue())                                  //ϵͳ��Ϣ
    {
        Msg->message = Delete_SYSQueue();
        if(Msg->message == KEY_PRESS)                      //������Ϣ
        {
            Msg->para = Delete_SYSQueue();
        }
        else 
        if(Msg->message == TASK_START)
        {
            Msg->para  = Delete_SYSQueue();
        }
        return 1;
    }   
    else
    Msg->message = EMPTY_MESSAGE;                       //����Ϣ
    return 0;
}

/************************************************************
** ��������: LZM_SetTimer
** ��������: �ظ���ʱ
** ��ڲ���: index:��ʱ�����
             value:��ʱֵ
** ���ڲ���:
************************************************************/ 
void LZM_SetTimer(LZM_TIMER32 index,LZM_TIMER32 value)
{
    s_stLzmTimers[index].enabled  = TIMER_ENABLE;
    s_stLzmTimers[index].counter  = value+Timer_Val();
    s_stLzmTimers[index].interval = value;
    s_stLzmTimers[index].operate = NULL;
}
//   
/************************************************************
** ��������: LZM_SetAlarm
** ��������: ���ζ�ʱ
** ��ڲ���: index:��ʱ�����
             value:��ʱֵ
** ���ڲ���:
************************************************************/ 
void LZM_SetAlarm(LZM_TIMER32 index,LZM_TIMER32 value)
{
    s_stLzmTimers[index].enabled  = TIMER_ENABLE;
    s_stLzmTimers[index].counter  = value+Timer_Val();
    s_stLzmTimers[index].interval = ONCE;
    s_stLzmTimers[index].operate = NULL;
}
/************************************************************
** ��������: LZM_KillTimer()
** ��������: �ر�ָ����ʱ��
** ��ڲ���: index:��ʱ�����
** ���ڲ���:
************************************************************/ 
void LZM_KillTimer(LZM_TIMER32 index)
{
    s_stLzmTimers[index].enabled  = TIMER_DISENABLE;
}

/************************************************************
** ��������: LZM_TimerGetEnabled()
** ��������: 
** ��ڲ���: index:��ʱ�����
** ���ڲ���:
************************************************************/ 
LZM_TIMER32 LZM_TimerGetEnabled(LZM_TIMER32 index)
{
    return s_stLzmTimers[index].enabled;
}
/************************************************************
** ��������: LZM_TimerHandler()
** ��������: Ӧ�ó���ʱ�Ӵ�����
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_TimerHandler(void)
{
    unsigned char i;
    unsigned long timerVal;
    timerVal = Timer_Val();
    for(i = 0; i < MAX_TIMERS; i++)
    {
        if(s_stLzmTimers[i].enabled == TIMER_ENABLE)
        {
            if(s_stLzmTimers[i].counter <= timerVal)
            {
                if(s_stLzmTimers[i].interval == ONCE)
                {
                    s_stLzmTimers[i].enabled = TIMER_DISENABLE;
                }
                else
                {
                    s_stLzmTimers[i].counter += s_stLzmTimers[i].interval;
                    if(s_stLzmTimers[i].counter < timerVal)
                        s_stLzmTimers[i].counter = timerVal;
                }
                Insert_INTQueue(i);
            }
        }
    }
}
/*****************************************************
*
*       Public  code
*****************************************************/
/************************************************************
** ��������: GetSutMessage()
** ��������: ��ȡ��Ϣ�ṹ
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
LZM_MESSAGE *LZM_GetSutMessage(void) 
{
    return &s_sutMessage;
}

/************************************************************
** ��������: Insert_STARTQueue()
** ��������: ������ֵ�������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Insert_STARTQueue(unsigned char param)
{
    Insert_SYSQueue(TASK_START); 
    Insert_SYSQueue(param);
}

/********************************************************************************
 * Function:KeyFormatValue
 * Describe:change Key's value to ASCII (Key 0 to Key 9)
 *******************************************************************************/
void KeyFormatValue(unsigned char *key)
{
    switch(*key)
    {
        case KEY_OK://ȷ��
        case KEY_EXIT://����
        case KEY_UP://�Ϸ�
        case KEY_DOWN://�·�
            *key = *key;            
            break;
        default:
            *key=0;
            break;
    }
}

/************************************************************
** ��������: Insert_KEYQueue()
** ��������: ������ֵ�������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Insert_KEYQueue(unsigned char key)
{
    BEEP_ON();
    Insert_SYSQueue(KEY_PRESS); 
    Insert_SYSQueue(key); 
}
/************************************************************
** ��������: LZM_RefuseNowTask()
** ��������: ֻ���µ�ǰ����
** ��ڲ���:param =0
** ���ڲ���:
************************************************************/ 
void LZM_RefuseNowTask(LZM_RET (*function)(LZM_MESSAGE *Msg))
{
    LZM_ReadyNewTask();    
    LZM_TaskFunction = function;
    Insert_STARTQueue(0);
}
/************************************************************
** ��������: LZM_RefuseNowTaskEx()
** ��������: ֻ���µ�ǰ����
** ��ڲ���: param=0xff
** ���ڲ���:
************************************************************/ 
void LZM_RefuseNowTaskEx(LZM_RET (*function)(LZM_MESSAGE *Msg))
{
    LZM_ReadyNewTask();    
    LZM_TaskFunction = function;
    Insert_STARTQueue(0xff);
} 
/************************************************************
** ��������: LZM_StartNewTask()
** ��������: ����������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_StartNewTask(LZM_RET (*function)(LZM_MESSAGE *Msg))
{
    if(LZM_TaskFunction != function)
    LZM_PreTaskFunction = LZM_TaskFunction;
    LZM_RefuseNowTask(function);
}

/************************************************************
** ��������: LZM_FreshNowTask()
** ��������: ֻˢ�µ�ǰ����
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_FreshNowTask(void)
{
    LZM_ReadyNewTask();    
    Insert_STARTQueue(0);
}
/************************************************************
** ��������: LZM_StartNewTaskEx()
** ��������: paramΪ0xff
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_StartNewTaskEx(LZM_RET (*function)(LZM_MESSAGE *Msg))
{
    LZM_RefuseNowTaskEx(function);
}

/************************************************************
** ��������: LZM_DispatchMessage()
** ��������: ��ǰ��������Ϣ
             ��ǰ�����ŷ��͵�ǰ��Ϣִ�в�ͬ�������� 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_DispatchMessage(LZM_MESSAGE *Msg)
{
    LZM_TaskFunction(Msg);
}  
/************************************************************
** ��������: LZM_DispatchMessage()
** ��������: ��ǰ��������Ϣ
             ��ǰ�����ŷ��͵�ǰ��Ϣִ�в�ͬ�������� 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_KeyHandler(void)
{
    static unsigned char s_ucCount;             //key press timer counter const
    static unsigned char s_ucKeyCount;          //key press timer counter
    //static unsigned char s_ucKeyTimes;            //Key times counter
    static unsigned char KeyCode;
    static unsigned char s_ucKeyValue=0;
    static unsigned short s_usTotalCnt=0;           //key press timer counter
    /////////////////////    
    KeyCode=KeyValue_Read();
    KeyFormatValue(&KeyCode);
    /////////       
    /////////////////////////////
    if(!KeyCode)                                    //no key press
    {
        s_ucKeyValue = 0;//
        s_usTotalCnt = 0;
    }
    else                                        //key pressed
    {
        SysAutoRunCountReset();
        if(KeyCode != s_ucKeyValue)                 //key counter reset
        {
            s_ucCount       = 10;                       //first counter = 2
            s_ucKeyCount    = 0;
            s_usTotalCnt    = 0;        
            s_ucKeyValue    = KeyCode;            
            Insert_KEYQueue(s_ucKeyValue);
        }
        else                                    //key count
        {
            if(s_usTotalCnt>SYSTICK_10SECOND)//һֱ����,����10s
            {
                BEEP_ON();
                return;//
            }
            s_usTotalCnt++;
            s_ucKeyCount ++;
            if(s_ucKeyCount >= s_ucCount)
            {       
                s_ucCount = 3;              //other counter = 3
                s_ucKeyCount = 0;
                //if(s_ucKeyMode == 0)
                {
                    Insert_KEYQueue(s_ucKeyValue);
                }                       //key up
            }
        }
    }
}
/************************************************************
** ��������: LZM_SetOperateTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_SetOperateTimer(void (*function)(void),LZM_TIMER32 time)
{
    LZM_OperateFunction=function;
    LZM_SetAlarm(OPERATE_TIMER, time);
}
/************************************************************
** ��������: LZM_DisposeOperateTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_DisposeOperateTimer(void)
{
    if(LZM_OperateFunction!=NULL)
    {
        LZM_OperateFunction();
        LZM_OperateFunction = NULL;
    }
}
/************************************************************
** ��������: LZM_DefTaskProc()
** ��������: Ĭ�ϴ�����������ʱ��ɨ�衢��������ʹ��������շ�
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
LZM_RET LZM_DefTaskProc(LZM_MESSAGE *Msg)
{   
    switch(Msg->message)
    { 
        case OPERATE_TIMER:
            LZM_DisposeOperateTimer();
            break;
        case REFRESH_TIMER:
            break;
        case INT_RTC:           
            break; 
        case SHOW_TIMER:
            Lcd_SetMainRedrawFlag();
            Lcd_ClearDisplayCtrlFlag();
            LcdPasteScreen();
            break;
        case FLASH_TIMER:
            FlashLcdBackLightFinish();
            break;
        case EMPTY_MESSAGE:      
        default:
            break;
    }
    return 0;
}

/*************************************************************
** ��������: LZM_TaskEmpty()
** ��������: ������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET LZM_TaskEmpty(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            break;
        case INT_RTC:
            break;  
        case KEY_PRESS:
            break;              
        default:
            LZM_DefTaskProc(Msg);
    }   
    return 0;
}

/************************************************************************************
*�պ���.
*Button��ʼ��ʱ������Ӧ�¼�(����/�ͷ�)���óɿղ���.
************************************************************************************/
void LZM_Nothing(void)
{
    //Do Nothing
}
/*************************************************************OK
** ��������: ReadyNweTask()
** ��������: ������׼��
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: Clear_SYSQueue();ClearLcdCopyFlag();
             Clear_INTQueue();
*************************************************************/
void LZM_ReadyNewTask(void)
{
    unsigned char i,OpTime,flashTime;
    ClearLcdCopyFlag();
    LZM_KillTimer(SHOW_TIMER); 
    LZM_KillTimer(REFRESH_TIMER);    
    LZM_KillTimer(TEST_TIMER);
    /////////////////////////////
    //�����������Ϣ//
    Clear_SYSQueue();
    ////�����ʱ����Ϣ///
    OpTime =0;
    flashTime =0;
    while(!Empty_INTQueue())//�ж���Ϣ
    {
        i = Delete_INTQueue();
        if(OPERATE_TIMER==i)
        {
            OpTime=1;
        }
        else
        if(FLASH_TIMER==i)
        {
            flashTime=1;
        }
    }
    //////////////////////
    if(OpTime)//
    {
        Insert_INTQueue(OPERATE_TIMER);
    }
    ////////////////////
    if(flashTime)//
    {
        Insert_INTQueue(FLASH_TIMER);
    }    
}
/*************************************************************OK
** ��������: LZM_ReturnPreTask()
** ��������: ֻ���µ�ǰ����,���ı��丸����
** ��ڲ���: Task:Ҫִ�е�����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
             
** ����ģ��: 
*************************************************************/
void LZM_ReturnPreTask(LZM_RET (*Task)(LZM_MESSAGE *Msg))
{
    LZM_RefuseNowTask(Task);
}
/*************************************************************OK
** ��������: LZM_ReturnOldTaskEx()
** ��������: ���񷵻�,����ǰһ������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: :
             :
** ����ģ��: 
*************************************************************/
void LZM_ReturnOldTaskEx(void)
{
    LZM_RET (*LZM_TempTaskFunction)(LZM_MESSAGE *Msg);
    if(LZM_PreTaskFunction!=LZM_TaskFunction)
    {
        LZM_TempTaskFunction =LZM_PreTaskFunction;
        LZM_PreTaskFunction = LZM_TaskFunction;
    }
    else
    {
        LZM_TempTaskFunction = TaskShowMainInterface;
    }
    LZM_RefuseNowTaskEx(LZM_TempTaskFunction);
}

/************************************************************
** ��������: TaskMenuMessage_TimeTask()
** ��������: ��ȡ��Ϣ�����ø�����ģ�����Ϣ������,20ms����һ��
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
FunctionalState TaskMenuMessage_TimeTask(void)
{
    static unsigned char sucCnt=0;
    /////////////////
    sucCnt++;     
    //////////////////////////////
    LZM_GetMessage(pMessage);
    LZM_DispatchMessage(pMessage);    
    //////////////////////////////
    ///////////////////////
    LZM_KeyHandler();
    /////////////////////
    if(sucCnt%2==0)//100ms����һ��
    {
        ////�˵�����ʱ��//////////////////////////////
        LZM_TimerHandler(); 
    }
    ////////////////////////
    if(sucCnt>4)//200ms
    {
        sucCnt = 0;
        SysAutoRunToMainInfoTask();//�Զ�
    }
    return ENABLE;
}
/*************************************************************OK
** ��������: LZM_TaskMenuStart()
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void LZM_TaskMenuStart(void)
{
    //��ʼ����Ӳ���豸
    LZM_DevInitHook();
    //��ʼ�������ģ��
    LZM_TaskInitHook();
    /////////////////////    
    SetTimerTask(TIME_MENU,1);//�����˵�����
    /////////////////////
    //������ѭ��������ѭ���������ø�����ģ�����Ϣ������
    TaskMenuMessage_TimeTask();
}

/*******************************************************************************
 *                            End of Module
 *******************************************************************************/
