/*******************************************************************************
 * File Name:			Phone.c 
 * Function Describe:	�绰����ģ��
 * Relate Module:		�������Э�顣
 * Writer:				Joneming
 * Date:				2014-08-29
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
/*******************************************************************************/
////////////////
#ifdef USE_PHONE
//////////////
enum 
{
    PH_TIMER_TASK,              //0 timer
    PH_TIMER_CHECK,             //1 timer
    PH_TIMER_TEST,              //2 timer
    PH_TIMER_CALL,              //3 timer
    PH_TIMERS_MAX
}E_PHONETIME;

static LZM_TIMER s_sPHTimer[PH_TIMERS_MAX];
/////////////////////////////////////
typedef enum 
{
    PHONE_STATUS_HANG_UP,    
    PHONE_STATUS_CALL_TALK,
    PHONE_STATUS_NEW_PHONE,
    PHONE_STATUS_MAX
}E_PHONE_STATUS;

typedef struct
{
    unsigned long curMonthTalkTime;//����ͨ����ʱ��
    unsigned long monthStartTime;//���¿�ʼʱ��
    unsigned char saveFlag;//�����־
    unsigned char bak[3];    //
    unsigned long nextMonthStartTime;//�¸��µĿ�ʼʱ��
    unsigned long eachMaxTalkTime; //ÿ���ͨ��ʱ��
    unsigned long monthMaxTalkTime;//ÿ���ͨ��ʱ��
    unsigned long curEachtime;//��ǰͨ��ʱ�� 
    unsigned char status;     //�绰״̬
    unsigned char saveCnt;    //
    unsigned char recallTime;  //�ز�����
    unsigned char tryCallCnt;  //
    unsigned char monitorFlag;//
    unsigned char specialFlag;      //
    unsigned char firstRunFlag;
}ST_PHONE_ATTRIB;

static ST_PHONE_ATTRIB s_stPhoneAttrib;
#define PHONE_SAVE_FLAG 0xAA
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/
/*************************************************************
** ��������: Phone_SetPhoneSpecialFlag
** ��������: ���õ绰������Ȩ��־
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Phone_SetPhoneSpecialFlag(unsigned char value)
{
    s_stPhoneAttrib.specialFlag = value;
}
/*************************************************************
** ��������: Phone_GetPhoneSpecialFlag
** ��������: ��ȡ�绰������Ȩ��־
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Phone_GetPhoneSpecialFlag(void)
{
    return s_stPhoneAttrib.specialFlag?1:0;
}
/*************************************************************OK
** ��������: Phone_GetCurMonthTotalSec
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Phone_GetCurMonthTotalSec(void)
{
    unsigned char MonTab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};    
    TIME_T stTime;
    unsigned long result;
    RTC_GetCurTime(&stTime);
    result= MonTab[stTime.month];
    //if(LeapYear(stTime.year)&&(stTime.month == 2)) dxl,2015.9,
    if(LeapYear(stTime.year+2000)&&(stTime.month == 2))
        result++;
    result *= DAY_SECOND;
    return result;    
}
/*************************************************************OK
** ��������: Phone_GetCurMonthStartTime
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Phone_GetCurMonthStartTime(void)
{  
    TIME_T stTime;
    RTC_GetCurTime(&stTime);
    stTime.day  = 1;
    stTime.hour = 0;
    stTime.min  = 0;
    stTime.sec  = 0;
    return ConverseGmtime(&stTime);
}
/*************************************************************
** ��������: Phone_CheckMonthStartTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_GetCurNextMonthStartTime(void)
{
    s_stPhoneAttrib.nextMonthStartTime=Phone_GetCurMonthTotalSec()+s_stPhoneAttrib.monthStartTime;
}
/*************************************************************OK
** ��������: Phone_SaveMonthParam
** ��������: ���²���
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_SaveMonthParam(void)
{
    if(0==s_stPhoneAttrib.saveCnt)return;
    s_stPhoneAttrib.saveCnt = 0;
    s_stPhoneAttrib.saveFlag = PHONE_SAVE_FLAG;
    Public_WriteDataToFRAM(FRAM_PHONE_MONTH_PARAM_ADDR, (unsigned char *)&s_stPhoneAttrib,12);
}
/*************************************************************
** ��������: Phone_GetCurMonthStartTimeAndSave
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_GetCurMonthStartTimeAndSave(void)
{
    s_stPhoneAttrib.curMonthTalkTime = 0;
    s_stPhoneAttrib.monthStartTime = Phone_GetCurMonthStartTime();
    Phone_GetCurNextMonthStartTime();
    s_stPhoneAttrib.saveCnt = 1;
    Phone_SaveMonthParam();
}
/*************************************************************OK
** ��������: Phone_ReadMonthParam
** ��������: ���²���
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_ReadMonthParam(void)
{
    Public_ReadDataFromFRAM(FRAM_PHONE_MONTH_PARAM_ADDR, (unsigned char *)&s_stPhoneAttrib, 12);
    if(PHONE_SAVE_FLAG!=s_stPhoneAttrib.saveFlag)
    {
        Phone_GetCurMonthStartTimeAndSave();
    }
}

/*************************************************************
** ��������: Phone_GetEachMaxTalkTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Phone_GetEachMaxTalkTime(void)
{
    return s_stPhoneAttrib.eachMaxTalkTime;
}
/*************************************************************
** ��������: Phone_GetMonthMaxTalkTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Phone_GetMonthMaxTalkTime(void)
{
    return s_stPhoneAttrib.monthMaxTalkTime;
}
/*************************************************************
** ��������: Phone_GetCurMonthTalkTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Phone_GetCurMonthTalkTime(void)
{
    return s_stPhoneAttrib.curMonthTalkTime;
}
/*******************************************************************************
** ��������: Phone_DisposeHangUp
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void Phone_DisposeHangUp(void)
{
    communication_CallPhone("13800000000",PHONE_END);
}
/*************************************************************
** ��������: Phone_CheckMonthStartTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_CheckMonthStartTime(void)
{
    unsigned long curtime;
    curtime = RTC_GetCounter();
    if(curtime<s_stPhoneAttrib.monthStartTime||curtime>=s_stPhoneAttrib.nextMonthStartTime)
    {
        Phone_GetCurMonthStartTimeAndSave();
    }
}
/*************************************************************
** ��������: Phone_GetOverMonthTalkTimeFlag
** ��������: ��ó�������ͨ��ʱ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 1:����,0:δ����
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char Phone_GetOverMonthTalkTimeFlag(void)
{
    return (s_stPhoneAttrib.curMonthTalkTime>=s_stPhoneAttrib.monthMaxTalkTime)?1:0;
}
/*************************************************************
** ��������: Phone_CheckMonthStartTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_CheckTalkTime(void)
{
    if(PHONE_STATUS_CALL_TALK!=s_stPhoneAttrib.status)
    {
        LZM_PublicKillTimer(&s_sPHTimer[PH_TIMER_CHECK]);
        Phone_SaveMonthParam();
        return;
    }
    s_stPhoneAttrib.saveCnt++;
    s_stPhoneAttrib.curEachtime++;
    s_stPhoneAttrib.curMonthTalkTime++;
    if(s_stPhoneAttrib.saveCnt>3)//3s����һ��FRAM
    {
        Phone_SaveMonthParam();
    }
    ////////////////////
    if(Phone_GetOverMonthTalkTimeFlag()||(s_stPhoneAttrib.curEachtime>=s_stPhoneAttrib.eachMaxTalkTime))
    {
        LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CHECK],LZM_AT_ONCE,Phone_DisposeHangUp);
        return;
    }
}
/*************************************************************
** ��������: Phone_StartCheckTalkTime
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_StartCheckTalkTime(void)
{
    if(Phone_GetOverMonthTalkTimeFlag())
    {
        LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CHECK],LZM_AT_ONCE,Phone_DisposeHangUp);
        return;
    }
    s_stPhoneAttrib.saveCnt = 0;
    s_stPhoneAttrib.curEachtime = 0;
    LZM_PublicSetCycTimer(&s_sPHTimer[PH_TIMER_CHECK],PUBLICSECS(1),Phone_CheckTalkTime);
}
/*******************************************************************************
* Function Name  : Phone_SetDialEvTask
* Description    : �绰�ز�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Phone_CallBackPhone(void)
{
    unsigned char ucTmp[30] = {0};	//����洢
    unsigned char len = 0;
    unsigned char flag;
    len = EepromPram_ReadPram(E2_PHONE_CALLBACK_ID, ucTmp);
    if (len != 0) 
    {
        ucTmp[len] = '\0';
        EepromPram_ReadPram(E2_CALLBACK_FLAG_ID, &flag);
        if(0 == flag) 
        {
            if(communication_CallPhone(ucTmp, PHONE_NORMAL))
            {
                Phone_SetPhoneSpecialFlag(1);
                return;
            }
        }
        else
        {
            if(MODON_PHONE == Modem_Api_OnSta())//�绰ҵ����
            {
                if(0==s_stPhoneAttrib.monitorFlag)
                {
                    LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CHECK],LZM_AT_ONCE,Phone_DisposeHangUp);
                    LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CALL],PUBLICSECS(2),Phone_CallBackPhone);
                }
                return;
            }
            else
            if(communication_CallPhone(ucTmp, PHONE_MONITOR))
            {
                Phone_SetPhoneSpecialFlag(1);
                s_stPhoneAttrib.tryCallCnt = 0;
                s_stPhoneAttrib.monitorFlag = 1;                
                if(s_stPhoneAttrib.recallTime)s_stPhoneAttrib.recallTime--;
                return;
            }
        }
        /////////////////
        if(s_stPhoneAttrib.tryCallCnt<10)
        {
            s_stPhoneAttrib.tryCallCnt++;
            LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CALL],PUBLICSECS(1),Phone_CallBackPhone);
        }
        else
        {
            s_stPhoneAttrib.tryCallCnt = 0;
            if(s_stPhoneAttrib.recallTime)
            {
                s_stPhoneAttrib.recallTime--;
                LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CALL],PUBLICSECS(30),Phone_CallBackPhone);
            }
        }
    }
}
/*******************************************************************************
* Function Name  : Phone_SetDialEvTask
* Description    : �绰�ز�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Phone_SetDialEvTask(void)
{
    unsigned char flag;
    EepromPram_ReadPram(E2_CALLBACK_FLAG_ID, &flag);
    if(0 == flag) 
    {
        if(PHONE_STATUS_CALL_TALK==s_stPhoneAttrib.status)return;//ͨ����,ֱ�ӷ���
        s_stPhoneAttrib.recallTime = 0;
    } 
    else 
    {
        if(MODON_PHONE == Modem_Api_OnSta())//ͨ����
        {
            if(s_stPhoneAttrib.monitorFlag)return;//������,ֱ�ӷ���
            LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CHECK],LZM_AT_ONCE,Phone_DisposeHangUp);
        }
        s_stPhoneAttrib.recallTime = 3;
    }
    s_stPhoneAttrib.tryCallCnt = 0;
    LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CALL],PUBLICSECS(3),Phone_CallBackPhone);
}
/*************************************************************
** ��������: Phone_DisposePhoneHangup
** ��������: ����绰״̬����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Phone_DisposeHangUpStatus(void)
{
    s_stPhoneAttrib.monitorFlag = 0;
    if(PHONE_STATUS_HANG_UP != s_stPhoneAttrib.status)
    {
        LZM_PublicKillTimer(&s_sPHTimer[PH_TIMER_CHECK]);
        Phone_SaveMonthParam();
    }
    ///////////////////////
    if(s_stPhoneAttrib.recallTime)
    {
        LZM_PublicSetOnceTimer(&s_sPHTimer[PH_TIMER_CALL],PUBLICSECS(30),Phone_CallBackPhone);
    }
}
/*************************************************************
** ��������: Phone_DisposePhoneHangup
** ��������: ����绰״̬����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Phone_DisposeCallTalkStatus(void)
{
    if(PHONE_STATUS_CALL_TALK != s_stPhoneAttrib.status)
    {
        if(s_stPhoneAttrib.monitorFlag)s_stPhoneAttrib.recallTime = 0;
        if(0==Phone_GetPhoneSpecialFlag())
        Phone_StartCheckTalkTime();
    }
}
/*************************************************************
** ��������: Phone_DisposePhoneStatus
** ��������: ����绰״̬����
** ��ڲ���: phone:�绰����,status:�绰״̬��0��Ϊ�Ҷ�;1:��ͨ;2:������;
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Phone_DisposePhoneStatus(unsigned char *phone,unsigned char status)
{
    switch(status)
    {
        case PHONE_STATUS_HANG_UP://�һ�
            Phone_DisposeHangUpStatus();            
            break;
        case PHONE_STATUS_CALL_TALK://ͨ��
            Phone_DisposeCallTalkStatus();
            break;
        case PHONE_STATUS_NEW_PHONE://������
            s_stPhoneAttrib.monitorFlag = 0;
            break;
    }
    s_stPhoneAttrib.status = status;    
}
/*************************************************************OK
** ��������: Phone_UpdateMaxPhoneTimeEach
** ��������: ���²���
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_UpdateMaxPhoneTimeEach(void)
{
    unsigned char buffer[10];
    if(EepromPram_ReadPram(E2_MAX_PHONE_TIME_EACH_ID,buffer))
    {
        s_stPhoneAttrib.eachMaxTalkTime = Public_ConvertBufferToLong(buffer);
    }
    else
    {
        s_stPhoneAttrib.eachMaxTalkTime = 0;
    }
}
/*************************************************************OK
** ��������: Phone_UpdateMaxPhoneTimeMonth
** ��������: ���²���
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_UpdateMaxPhoneTimeMonth(void)
{
    unsigned char buffer[10];
    if(EepromPram_ReadPram(E2_MAX_PHONE_TIME_MONTH_ID,buffer))
    {
        s_stPhoneAttrib.monthMaxTalkTime = Public_ConvertBufferToLong(buffer);
    }
    else
    {
        s_stPhoneAttrib.monthMaxTalkTime = 0;
    }
    ////////////////
    if(s_stPhoneAttrib.firstRunFlag)
    {
        Phone_GetCurMonthStartTimeAndSave();
    }
    s_stPhoneAttrib.firstRunFlag = 1;
}
/*************************************************************OK
** ��������: Phone_UpdatePram
** ��������: ���²���
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_UpdateParam(void)
{
    Phone_UpdateMaxPhoneTimeEach();
    //////////////
    Phone_UpdateMaxPhoneTimeMonth();
}
/*************************************************************OK
** ��������: Phone_ParamInitialize
** ��������: ��ʼ��
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Phone_ParamInitialize(void)
{ 
    LZM_PublicKillTimerAll(s_sPHTimer,PH_TIMERS_MAX);
    memset(&s_stPhoneAttrib,0,sizeof(ST_PHONE_ATTRIB));
    Phone_ReadMonthParam();    
    Phone_UpdateParam(); 
    Phone_GetCurNextMonthStartTime();
    //SetTimerTask(TIME_PHONE, LZM_TIME_BASE);
    LZM_PublicSetCycTimer(&s_sPHTimer[PH_TIMER_TASK],PUBLICSECS(1),Phone_CheckMonthStartTime);
}

/*******************************************************************************
**  ��������  : Phone_TimeTask
**  ��������  : ʱ��������Ƚӿں���
**  ��    ��  : ��				
**  ��    ��  : �������״̬:  ʹ�ܻ��ֹ
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
FunctionalState Phone_TimeTask(void) 
{
    LZM_PublicTimerHandler(s_sPHTimer,PH_TIMERS_MAX);
    return ENABLE;
}
#endif
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

