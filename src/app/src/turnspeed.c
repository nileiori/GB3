/*******************************************************************************
 * File Name:			turnspeed.c 
 * Function Describe:	转速
 * Relate Module:		
 * Writer:				Joneming
 * Date:				2016-06-06
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
#include "turnspeed.h"
/*******************************************************************************/
#define TURNSPEED_INFO_ID       0xEB//转速附加信息ID
////////////////////////////////////////////
static unsigned char s_ucTurnSpeedCtrlId;
extern ST_ESPIN s_stESpin[];
extern ST_SPIN s_stSpin[];

typedef struct
{
    float curPulseVal;
    unsigned short curTurnSpeed;//
    unsigned short pulseNum;//转速值
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
** 函数名称: TurnSpeed_CheckSelectShow
** 功能描述: 
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
unsigned char TurnSpeed_CheckSelectShow(void)
{
    LcdClearScreen(); 
    if(Task_GetCurSystemIsTurnSpeedFlag())return 1;
    s_ucTurnSpeedCtrlId = 0xff;
    LcdShowCaption("请先选择转速", 12);
    LcdShowCaption("主界面,再操作",36);    
    LZM_SetAlarm(TEST_TIMER, SECS(5));
    return 0;
}
/*************************************************************
** 函数名称: TaskSystemTurnSpeedPulseNum
** 功能描述: 转速脉冲个数标定
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
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
            LcdShowCaption("每转脉冲个数:", 0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("按<确认>保存",47);
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
                case KEY_BACK://返回
                    LZM_KillTimer(REFRESH_TIMER);
                    LZM_StartNewTask(TaskMenuSystemDebug);//系统管理
                    break; 
                case KEY_ENTER://确定
                    LZM_KillTimer(REFRESH_TIMER);
                    val=atol(s_stESpin[s_ucTurnSpeedCtrlId].buffer);
                    if(0==val)
                    {
                        LcdShowMsgEx("脉冲数不能为0",SECS(4));
                        break;
                    }
                    LcdClearScreen();
                    LcdShowCaption("正在保存...", 24);
                    s_stTurnSpeedAttrib.pulseNum = val;                    
                    Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.pulseNum, buffer);
                    Public_WriteDataToFRAM(FRAM_TURNSPEED_PULSE_NUM_ADDR,buffer,FRAM_TURNSPEED_PULSE_NUM_LEN);
                    Public_ReadDataFromFRAM(FRAM_TURNSPEED_PULSE_NUM_ADDR,buffer,FRAM_TURNSPEED_PULSE_NUM_LEN);
                    LcdClearArea(0,19,131,44);
                    if(Public_ConvertBufferToShort(buffer)==s_stTurnSpeedAttrib.pulseNum)
                    {
                        LcdShowCaption("保存成功", 24);                        
                    }
                    else
                    {
                       LcdShowCaption("保存错误,请检查", 24);
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
            LZM_StartNewTask(TaskMenuSystemDebug);//系统管理
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
** 函数名称: TurnSpeed_UpdatePram
** 功能描述: 
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
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
** 函数名称: TurnSpeed_ParameterInitialize
** 功能描述: 
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
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
** 函数名称: TurnSpeed_GetSubjoinInfoCurTurnSpeedVal
** 功能描述: 取得当前转速值附加信息包(包括附加信息ID、长度、附加信息体)
** 入口参数: 需要保存转速值附加信息包的首地址
** 出口参数: 
** 返回参数: 转速值附加信息包的总长度
*******************************************************************************/
unsigned char TurnSpeed_GetSubjoinInfoCurTurnSpeedVal(unsigned char *data)
{
    unsigned char buffer[10]={0};
    unsigned char len;
    if(0==Task_GetCurSystemIsTurnSpeedFlag())return 0;//未选择转速
    len = 0;    
    buffer[len++] = TURNSPEED_INFO_ID;//附加信息ID
    buffer[len++] = 4;   //附加长度
    Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.curTurnSpeed,&buffer[len]);//附加信息体
    len += 2;
    Public_ConvertShortToBuffer(s_stTurnSpeedAttrib.pulseNum,&buffer[len]);//附加信息体
    //////////////////////////
    len += 2;
    ///////////////
    memcpy(data,buffer,len);
    /////////////////
    return len;
}
/*******************************************************************************
** 函数名称:TurnSpeed_InsertCurPulseVal
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
void TurnSpeed_InsertCurPulseVal(float val)
{
    s_stTurnSpeedAttrib.curPulseVal = val;
}
/*******************************************************************************
** 函数名称:TurnSpeed_CalcCurTurnSpeed
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
void TurnSpeed_CalcCurTurnSpeed(void)
{
    float val; 
    val = s_stTurnSpeedAttrib.curPulseVal / s_stTurnSpeedAttrib.pulseNum;
    s_stTurnSpeedAttrib.curTurnSpeed=(int)val;
    if(val-s_stTurnSpeedAttrib.curTurnSpeed>0.5)s_stTurnSpeedAttrib.curTurnSpeed++;
}
/*******************************************************************************
** 函数名称:TurnSpeed_GetCurTurnSpeed
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
unsigned short TurnSpeed_GetCurTurnSpeed(void)
{
    return s_stTurnSpeedAttrib.curTurnSpeed;
}

/*********************************************************************
//函数名称	:TurnSpeed_TimeTask
//功能		:定时任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:1秒调用1次
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

