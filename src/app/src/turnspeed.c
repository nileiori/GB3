/*******************************************************************************
 * File Name:			turnspeed.c 
 * Function Describe:	ת��
 * Relate Module:		
 * Writer:				Joneming
 * Date:				2016-06-06
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
#include "turnspeed.h"
/*******************************************************************************/
#define TURNSPEED_INFO_ID       0xEB//ת�ٸ�����ϢID
////////////////////////////////////////////
static unsigned char s_ucTurnSpeedCtrlId;
extern ST_ESPIN s_stESpin[];
extern ST_SPIN s_stSpin[];

typedef struct
{
    float curPulseVal;
    unsigned short curTurnSpeed;//
    unsigned short pulseNum;//ת��ֵ
}ST_TURNSPEED_ATTRIB;

static ST_TURNSPEED_ATTRIB s_stTurnSpeedAttrib;
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/
extern LZM_RET TaskMenuSystemDebug(LZM_MESSAGE *Msg);
extern LZM_RET TaskMenuSystem(LZM_MESSAGE *Msg);
/*************************************************************
** ��������: TurnSpeed_CheckSelectShow
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char TurnSpeed_CheckSelectShow(void)
{
    LcdClearScreen(); 
    if(Task_GetCurSystemIsTurnSpeedFlag())return 1;
    s_ucTurnSpeedCtrlId = 0xff;
    LcdShowCaption("����ѡ��ת��", 12);
    LcdShowCaption("������,�ٲ���",36);    
    LZM_SetAlarm(TEST_TIMER, SECS(5));
    return 0;
}
/*************************************************************
** ��������: TaskSystemTurnSpeedPulseNum
** ��������: ת����������궨
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemTurnSpeedPulseNum(LZM_MESSAGE *Msg)
{
    unsigned long val;
    unsigned char buffer[10];
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(REFRESH_TIMER);
            if(0==TurnSpeed_CheckSelectShow())return 1;
            s_ucTurnSpeedCtrlId=0;
            LcdShowCaption("ÿת�������:", 0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            ESpinInit(&s_stESpin[s_ucTurnSpeedCtrlId], 48, 23, 3,s_stTurnSpeedAttrib.pulseNum);
            ESpinShow(&s_stESpin[s_ucTurnSpeedCtrlId]);            
            ESpinSetFocus(&s_stESpin[s_ucTurnSpeedCtrlId]);
            //LZM_SetAlarm(REFRESH_TIMER, SECS(0.2));
            return 1;
        case KEY_PRESS:
            if(s_ucTurnSpeedCtrlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_KillTimer(REFRESH_TIMER);
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    LZM_KillTimer(REFRESH_TIMER);
                    val=atol(s_stESpin[s_ucTurnSpeedCtrlId].buffer);
                    if(0==val)
                    {
                        LcdShowMsgEx("����������Ϊ0",SECS(4));
                        break;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    s_stTurnSpeedAttrib.pulseNum = val;                    
                    Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.pulseNum, buffer);
                    Public_WriteDataToFRAM(FRAM_TURNSPEED_PULSE_NUM_ADDR,buffer,FRAM_TURNSPEED_PULSE_NUM_LEN);
                    Public_ReadDataFromFRAM(FRAM_TURNSPEED_PULSE_NUM_ADDR,buffer,FRAM_TURNSPEED_PULSE_NUM_LEN);
                    LcdClearArea(0,19,131,44);
                    if(Public_ConvertBufferToShort(buffer)==s_stTurnSpeedAttrib.pulseNum)
                    {
                        LcdShowCaption("����ɹ�", 24);                        
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24);
                    }
                    s_ucTurnSpeedCtrlId=0xff;                    
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    TurnSpeed_UpdatePram();
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_ucTurnSpeedCtrlId],TaskPara);
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_KillTimer(REFRESH_TIMER);
            LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
            break;
        case REFRESH_TIMER:
            //sprintf((char *)buffer,"%4d",Ad_GetValue(DEEP_SENSOR_ADC));
            //LcdShowStr(10, 20, (char *)buffer,0);
            LZM_SetAlarm(REFRESH_TIMER, SECS(0.2));
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}


/*************************************************************
** ��������: TurnSpeed_UpdatePram
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void TurnSpeed_UpdatePram(void)
{
    unsigned char buffer[6];
    if(0==Task_GetCurSystemIsTurnSpeedFlag())return;
    if(Public_ReadDataFromFRAM(FRAM_TURNSPEED_PULSE_NUM_ADDR,buffer,FRAM_TURNSPEED_PULSE_NUM_LEN))
    {
        s_stTurnSpeedAttrib.pulseNum = Public_ConvertBufferToShort(buffer);
    }
    //else
    if(0==s_stTurnSpeedAttrib.pulseNum)//
    {
        s_stTurnSpeedAttrib.pulseNum = 1;
        Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.pulseNum, buffer);
        Public_WriteDataToFRAM(FRAM_TURNSPEED_PULSE_NUM_ADDR,buffer,FRAM_TURNSPEED_PULSE_NUM_LEN);
    }
}

/*************************************************************
** ��������: TurnSpeed_ParameterInitialize
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void TurnSpeed_ParameterInitialize(void)
{
    static unsigned char suTurnSpeedFirstRun=0;
    if(suTurnSpeedFirstRun)return;
    suTurnSpeedFirstRun = 1;
    memset(&s_stTurnSpeedAttrib,0,sizeof(s_stTurnSpeedAttrib));
    TurnSpeed_UpdatePram();
}
/*******************************************************************************
** ��������: TurnSpeed_GetSubjoinInfoCurTurnSpeedVal
** ��������: ȡ�õ�ǰת��ֵ������Ϣ��(����������ϢID�����ȡ�������Ϣ��)
** ��ڲ���: ��Ҫ����ת��ֵ������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: ת��ֵ������Ϣ�����ܳ���
*******************************************************************************/
unsigned char TurnSpeed_GetSubjoinInfoCurTurnSpeedVal(unsigned char *data)
{
    unsigned char buffer[10]={0};
    unsigned char len;
    if(0==Task_GetCurSystemIsTurnSpeedFlag())return 0;//δѡ��ת��
    len = 0;    
    buffer[len++] = TURNSPEED_INFO_ID;//������ϢID
    buffer[len++] = 4;   //���ӳ���
    Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.curTurnSpeed,&buffer[len]);//������Ϣ��
    len += 2;
    Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.pulseNum,&buffer[len]);//������Ϣ��
    //////////////////////////
    len += 2;
    ///////////////
    memcpy(data,buffer,len);
    /////////////////
    return len;
}
/*******************************************************************************
** ��������:TurnSpeed_InsertCurPulseVal
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*******************************************************************************/
void TurnSpeed_InsertCurPulseVal(float val)
{
    s_stTurnSpeedAttrib.curPulseVal = val;
}
/*******************************************************************************
** ��������:TurnSpeed_CalcCurTurnSpeed
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*******************************************************************************/
void TurnSpeed_CalcCurTurnSpeed(void)
{
    float val; 
    val = s_stTurnSpeedAttrib.curPulseVal / s_stTurnSpeedAttrib.pulseNum;
    s_stTurnSpeedAttrib.curTurnSpeed=(int)val;
    if(val-s_stTurnSpeedAttrib.curTurnSpeed>0.5)s_stTurnSpeedAttrib.curTurnSpeed++;
}
/*******************************************************************************
** ��������:TurnSpeed_GetCurTurnSpeed
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*******************************************************************************/
unsigned short TurnSpeed_GetCurTurnSpeed(void)
{
    return s_stTurnSpeedAttrib.curTurnSpeed;
}

/*********************************************************************
//��������	:TurnSpeed_TimeTask
//����		:��ʱ����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:1�����1��
*********************************************************************/
FunctionalState TurnSpeed_TimeTask(void)
{
    if(0==Task_GetCurSystemIsTurnSpeedFlag())return ENABLE;
    TurnSpeed_ParameterInitialize();
    TurnSpeed_CalcCurTurnSpeed();
    return ENABLE;
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

