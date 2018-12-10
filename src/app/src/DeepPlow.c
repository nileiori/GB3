/*******************************************************************************
 * File Name:			DeepPlow.c 
 * Function Describe:	深耕
 * Relate Module:		
 * Writer:				Joneming
 * Date:				2013-05-25
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
#include "DeepPlow.h"
/*******************************************************************************/
#define DEEP_SENSOR_ADC         ADC_EXTERN1//
////////////////////////////////////////////
static unsigned char s_ucDeepCtrlId;
extern ST_ESPIN s_stESpin[];
extern ST_SPIN s_stSpin[];

typedef struct
{
    float tmpA;
    float tmpAS;
    float bate;
    unsigned short curHeight;//
    unsigned short sensorMin;//最小值对应的耕深值
    unsigned short deepMin;//最小值对应的耕深值
    unsigned short sensorMax;//最大值对应的耕深值
    unsigned short deepMax;//最大值对应的耕深值
    unsigned short armLength;//基准臂长
    unsigned short maxLength;//基准高度    
    unsigned short baseHeight;//耕深基准值
    unsigned char result:1;//
}ST_DEEP_ATTRIB;

static ST_DEEP_ATTRIB s_stDeepAttrib;
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/
extern LZM_RET TaskMenuSystemDebug(LZM_MESSAGE *Msg);
extern LZM_RET TaskMenuSystem(LZM_MESSAGE *Msg);
/*************************************************************
** 函数名称: DeepPlow_CheckSelectShow
** 功能描述: 
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
unsigned char DeepPlow_CheckSelectShow(void)
{
    LcdClearScreen(); 
    if(Task_GetCurSystemIsDeepFlag())return 1;
    s_ucDeepCtrlId = 0xff;
    LcdShowCaption("请先选择耕深", 12);
    LcdShowCaption("主界面,再操作",36);    
    LZM_SetAlarm(TEST_TIMER, SECS(5));
    return 0;
}
/*************************************************************
** 函数名称: TaskSystemDeepBaseCalib
** 功能描述: 耕深基准标定
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
LZM_RET TaskSystemDeepBaseCalib(LZM_MESSAGE *Msg)
{
    unsigned char buffer[6];
    switch(Msg->message)
    {
        case TASK_START:
            if(0==DeepPlow_CheckSelectShow())return 1;            
            s_ucDeepCtrlId=0;
            LcdShowCaption("基准标定", 0);                 
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowStr(0,24,"基准臂长:   厘米",0);   
            LcdShowStr(0,46,"基准高度:   厘米",0);   
            ESpinInit(&s_stESpin[0], 76, 24, 2,s_stDeepAttrib.armLength);
            ESpinInit(&s_stESpin[1], 76, 46, 2,s_stDeepAttrib.maxLength);
            s_stESpin[0].loop = 0;
            s_stESpin[1].loop = 0;
            ESpinShow(&s_stESpin[0]);
            ESpinShow(&s_stESpin[1]);
            ESpinSetFocus(&s_stESpin[s_ucDeepCtrlId]);            
            return 1;
        case KEY_PRESS:
            if(s_ucDeepCtrlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://返回
                    LZM_StartNewTask(TaskMenuSystemDebug);//系统管理
                    break; 
                case KEY_ENTER://确定
                    LcdClearScreen();
                    LcdShowCaption("正在保存...", 24);
                    s_stDeepAttrib.armLength = atol(s_stESpin[0].buffer);
                    s_stDeepAttrib.maxLength = atol(s_stESpin[1].buffer);
                    Public_ConvertShortToBuffer(s_stDeepAttrib.armLength, buffer);
                    EepromPram_WritePram(E2_DEEP_ARM_LENGTH_ID,buffer,E2_DEEP_ARM_LENGTH_ID_LEN);
                    Public_ConvertShortToBuffer(s_stDeepAttrib.maxLength, buffer);
                    EepromPram_WritePram(E2_DEEP_MAX_LENGTH_ID,buffer,E2_DEEP_MAX_LENGTH_ID_LEN);
                    EepromPram_ReadPram(E2_DEEP_MAX_LENGTH_ID,buffer);
                    LcdClearArea(0,19,131,44);
                    if(Public_ConvertBufferToShort(buffer)==s_stDeepAttrib.maxLength)
                    {
                        LcdShowCaption("保存成功", 24);                        
                    }
                    else
                    {
                       LcdShowCaption("保存错误,请检查", 24);
                    }
                    s_ucDeepCtrlId=0xff;                    
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    DeepPlow_UpdatePram();
                    break;                               
                default:
                    if(ESpinResponse(&s_stESpin[s_ucDeepCtrlId],TaskPara))
                    {
                        ESpinLoseFocus(&s_stESpin[s_ucDeepCtrlId]);
                        s_ucDeepCtrlId ^=1;
                        ESpinSetFocus(&s_stESpin[s_ucDeepCtrlId]);
                    }
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemDebug);//系统管理
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** 函数名称: TaskSystemDeepMinCalib
** 功能描述: 耕深基准最小值标定
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
LZM_RET TaskSystemDeepMinCalib(LZM_MESSAGE *Msg)
{
    unsigned char buffer[10];
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(REFRESH_TIMER);
            if(0==DeepPlow_CheckSelectShow())return 1;
            s_ucDeepCtrlId=0;
            LcdShowCaption("传感器最小值:", 0);
            sprintf((char *)buffer,"%4d",Ad_GetValue(DEEP_SENSOR_ADC));
            LcdShowStr(10, 20, (char *)buffer,0);
            sprintf((char *)buffer,"%d",s_stDeepAttrib.sensorMin);
            LcdShowStr(76, 20, (char *)buffer,0);
            LcdShowStr(0,40,"对应耕深:   厘米",0); 
            ESpinInit(&s_stESpin[s_ucDeepCtrlId], 76, 40, 2,s_stDeepAttrib.deepMin);
            ESpinShow(&s_stESpin[s_ucDeepCtrlId]);            
            ESpinSetFocus(&s_stESpin[s_ucDeepCtrlId]);
            LZM_SetAlarm(REFRESH_TIMER, SECS(0.2));
            return 1;
        case KEY_PRESS:
            if(s_ucDeepCtrlId==0xff)
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
                    LcdClearScreen();
                    LcdShowCaption("正在保存...", 24);
                    s_stDeepAttrib.sensorMin = Ad_GetValue(DEEP_SENSOR_ADC);
                    s_stDeepAttrib.deepMin = atol(s_stESpin[s_ucDeepCtrlId].buffer);                    
                    Public_ConvertShortToBuffer(s_stDeepAttrib.sensorMin, buffer);
                    EepromPram_WritePram(E2_DEEP_SENSORMIN_ID,buffer,E2_DEEP_SENSORMIN_ID_LEN);
                    Public_ConvertShortToBuffer(s_stDeepAttrib.deepMin, buffer);
                    EepromPram_WritePram(E2_DEEP_SENSORMIN_LENGTH_ID,buffer,E2_DEEP_SENSORMIN_LENGTH_ID_LEN);                    
                    EepromPram_ReadPram(E2_DEEP_SENSORMIN_LENGTH_ID,buffer);
                    LcdClearArea(0,19,131,44);
                    if(Public_ConvertBufferToShort(buffer)==s_stDeepAttrib.deepMin)
                    {
                        LcdShowCaption("保存成功", 24);                        
                    }
                    else
                    {
                       LcdShowCaption("保存错误,请检查", 24);
                    }
                    s_ucDeepCtrlId=0xff;                    
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    DeepPlow_UpdatePram();
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_ucDeepCtrlId],TaskPara);
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_KillTimer(REFRESH_TIMER);
            LZM_StartNewTask(TaskMenuSystemDebug);//系统管理
            break;
        case REFRESH_TIMER:
            sprintf((char *)buffer,"%4d",Ad_GetValue(DEEP_SENSOR_ADC));
            LcdShowStr(10, 20, (char *)buffer,0);
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
** 函数名称: TaskSystemDeepMaxCalib
** 功能描述: 耕深基准最大值标定
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
LZM_RET TaskSystemDeepMaxCalib(LZM_MESSAGE *Msg)
{
    unsigned char buffer[10];
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(REFRESH_TIMER);
            if(0==DeepPlow_CheckSelectShow())return 1;
            s_ucDeepCtrlId=0;
            LcdShowCaption("传感器最大值:", 0);
            sprintf((char *)buffer,"%4d",Ad_GetValue(DEEP_SENSOR_ADC));
            LcdShowStr(10, 20, (char *)buffer,0);
            sprintf((char *)buffer,"%d",s_stDeepAttrib.sensorMax);
            LcdShowStr(76, 20, (char *)buffer,0);
            LcdShowStr(0,40,"对应耕深:   厘米",0); 
            ESpinInit(&s_stESpin[s_ucDeepCtrlId], 76, 40, 2,s_stDeepAttrib.deepMax);
            ESpinShow(&s_stESpin[s_ucDeepCtrlId]);            
            ESpinSetFocus(&s_stESpin[s_ucDeepCtrlId]);
            LZM_SetAlarm(REFRESH_TIMER, SECS(0.2));
            return 1;
        case KEY_PRESS:
            if(s_ucDeepCtrlId==0xff)
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
                    LcdClearScreen();
                    LcdShowCaption("正在保存...", 24);
                    s_stDeepAttrib.sensorMax= Ad_GetValue(DEEP_SENSOR_ADC);
                    s_stDeepAttrib.deepMax= atol(s_stESpin[s_ucDeepCtrlId].buffer);                    
                    Public_ConvertShortToBuffer(s_stDeepAttrib.sensorMax, buffer);
                    EepromPram_WritePram(E2_DEEP_SENSORMAX_ID,buffer,E2_DEEP_SENSORMAX_ID_LEN);
                    Public_ConvertShortToBuffer(s_stDeepAttrib.deepMax, buffer);
                    EepromPram_WritePram(E2_DEEP_SENSORMAX_LENGTH_ID,buffer,E2_DEEP_SENSORMAX_LENGTH_ID_LEN);                    
                    EepromPram_ReadPram(E2_DEEP_SENSORMAX_LENGTH_ID,buffer);
                    LcdClearArea(0,19,131,44);
                    if(Public_ConvertBufferToShort(buffer)==s_stDeepAttrib.deepMax)
                    {
                        LcdShowCaption("保存成功", 24);
                    }
                    else
                    {
                       LcdShowCaption("保存错误,请检查", 24);
                    }
                    s_ucDeepCtrlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    DeepPlow_UpdatePram();
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_ucDeepCtrlId],TaskPara);
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_KillTimer(REFRESH_TIMER);
            LZM_StartNewTask(TaskMenuSystemDebug);//系统管理
            break;
        case REFRESH_TIMER:
            sprintf((char *)buffer,"%4d",Ad_GetValue(DEEP_SENSOR_ADC));
            LcdShowStr(10, 20, (char *)buffer,0);
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
** 函数名称: TaskSystemDeepBaseSet
** 功能描述: 耕深基准设置
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/  
LZM_RET TaskSystemDeepBaseSet(LZM_MESSAGE *Msg)
{
    unsigned char buffer[6];
    switch(Msg->message)
    {
        case TASK_START:
            if(0==DeepPlow_CheckSelectShow())return 1;            
            s_ucDeepCtrlId=0;
            LcdShowCaption("耕深基准设置", 0);
            LcdShowStr(0,23,"基准值:     厘米",0);    
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("按<确认>保存",47);
            ESpinInit(&s_stESpin[s_ucDeepCtrlId], 70, 23, 2,s_stDeepAttrib.baseHeight);
            ESpinShow(&s_stESpin[s_ucDeepCtrlId]);            
            ESpinSetFocus(&s_stESpin[s_ucDeepCtrlId]);            
            return 1;
        case KEY_PRESS:
            if(s_ucDeepCtrlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://返回
                    LZM_StartNewTask(TaskMenuSystem);//系统管理
                    break; 
                case KEY_ENTER://确定
                    LcdClearScreen();
                    LcdShowCaption("正在保存...", 24);
                    s_stDeepAttrib.baseHeight=atol(s_stESpin[s_ucDeepCtrlId].buffer);
                    Public_ConvertShortToBuffer(s_stDeepAttrib.baseHeight, buffer);
                    EepromPram_WritePram(E2_DEEP_BASE_HEIGHT_ID,buffer,E2_DEEP_BASE_HEIGHT_ID_LEN);                    
                    EepromPram_ReadPram(E2_DEEP_BASE_HEIGHT_ID,buffer);
                    LcdClearArea(0,19,131,44);
                    if(Public_ConvertBufferToShort(buffer)==s_stDeepAttrib.baseHeight)
                    {
                        LcdShowCaption("保存成功", 24);
                    }
                    else
                    {
                       LcdShowCaption("保存错误,请检查", 24);
                    }
                    s_ucDeepCtrlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    DeepPlow_UpdatePram();
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_ucDeepCtrlId],TaskPara);
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystem);//系统管理
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*******************************************************************************
** 函数名称:DeepPlow_CalcDeepParam
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
void DeepPlow_CalcDeepParam(void)
{
    float angleMax,sensorWidth,angleWidth,temp,armLength,tmpA,tmpB,angleMin,tmpAS;
    ////////////////////////
    temp = s_stDeepAttrib.maxLength - s_stDeepAttrib.deepMin;
    /////////////////////////
    armLength = s_stDeepAttrib.armLength;
    ////////////////////
    tmpA = temp / armLength;
    //////////////////////////
    s_stDeepAttrib.result = 0;
    ////////////////////////
    if(temp>armLength)s_stDeepAttrib.result = 1;
    /////////////////////////////
    temp = s_stDeepAttrib.maxLength - s_stDeepAttrib.deepMax;
    ///////////////////////////
    if(temp>armLength)s_stDeepAttrib.result = 1;
    ////////////////////////
    tmpB = temp / armLength;
    angleMin = acos(tmpA);
    angleMax = acos(tmpB);
    sensorWidth = fabs(s_stDeepAttrib.sensorMax - s_stDeepAttrib.sensorMin);
    angleWidth = fabs(angleMax - angleMin);
    /////////////////////
    tmpAS = sqrt(1.0-tmpA*tmpA);
    ///////////////////////////
    s_stDeepAttrib.bate = angleWidth /sensorWidth;     
    s_stDeepAttrib.tmpAS= tmpAS;
    s_stDeepAttrib.tmpA = tmpA;
}
/*************************************************************
** 函数名称: DeepPlow_UpdatePram
** 功能描述: 
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void DeepPlow_UpdatePram(void)
{
    unsigned char buffer[6];
    if(0==Task_GetCurSystemIsDeepFlag())return;
    if(EepromPram_ReadPram(E2_DEEP_SENSORMIN_ID, buffer))
    {
        s_stDeepAttrib.sensorMin = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.sensorMin = 200;
        Public_ConvertShortToBuffer(s_stDeepAttrib.sensorMin, buffer);
        EepromPram_WritePram(E2_DEEP_SENSORMIN_ID,buffer,E2_DEEP_SENSORMIN_ID_LEN);
    }
    ////////////////
    if(EepromPram_ReadPram(E2_DEEP_SENSORMIN_LENGTH_ID, buffer))
    {
        s_stDeepAttrib.deepMin = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.deepMin = 15;
        Public_ConvertShortToBuffer(s_stDeepAttrib.deepMin, buffer);
        EepromPram_WritePram(E2_DEEP_SENSORMIN_LENGTH_ID,buffer,E2_DEEP_SENSORMIN_LENGTH_ID_LEN);
    }
    //////////////////////////////
    if(EepromPram_ReadPram(E2_DEEP_SENSORMAX_ID, buffer))
    {
        s_stDeepAttrib.sensorMax = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.sensorMax = 4000;
        Public_ConvertShortToBuffer(s_stDeepAttrib.sensorMax, buffer);
        EepromPram_WritePram(E2_DEEP_SENSORMAX_ID,buffer,E2_DEEP_SENSORMAX_ID_LEN);
    }
    ////////////////
    if(EepromPram_ReadPram(E2_DEEP_SENSORMAX_LENGTH_ID, buffer))
    {
        s_stDeepAttrib.deepMax = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.deepMax = 45;
        Public_ConvertShortToBuffer(s_stDeepAttrib.deepMax, buffer);
        EepromPram_WritePram(E2_DEEP_SENSORMAX_LENGTH_ID,buffer,E2_DEEP_SENSORMAX_LENGTH_ID_LEN);
    }
    ////////////////
    if(EepromPram_ReadPram(E2_DEEP_BASE_HEIGHT_ID, buffer))
    {
        s_stDeepAttrib.baseHeight = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.baseHeight = 20;
        Public_ConvertShortToBuffer(s_stDeepAttrib.baseHeight, buffer);
        EepromPram_WritePram(E2_DEEP_BASE_HEIGHT_ID,buffer,E2_DEEP_BASE_HEIGHT_ID_LEN);
    }
    ////////////////////////
    if(EepromPram_ReadPram(E2_DEEP_ARM_LENGTH_ID, buffer))
    {
        s_stDeepAttrib.armLength = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.armLength = 42;
        Public_ConvertShortToBuffer(s_stDeepAttrib.armLength, buffer);
        EepromPram_WritePram(E2_DEEP_ARM_LENGTH_ID,buffer,E2_DEEP_ARM_LENGTH_ID_LEN);
    }
    //////////////
    if(EepromPram_ReadPram(E2_DEEP_MAX_LENGTH_ID, buffer))
    {
        s_stDeepAttrib.maxLength = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        s_stDeepAttrib.maxLength = 55;
        Public_ConvertShortToBuffer(s_stDeepAttrib.maxLength, buffer);
        EepromPram_WritePram(E2_DEEP_MAX_LENGTH_ID,buffer,E2_DEEP_MAX_LENGTH_ID_LEN);
    }
    ///////////////////////
    DeepPlow_CalcDeepParam();
    //////////////////////
}

/*************************************************************
** 函数名称: DeepPlow_ParameterInitialize
** 功能描述: 
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void DeepPlow_ParameterInitialize(void)
{
    static unsigned char sucDeepFirstRun=0;
    if(sucDeepFirstRun)return;
    sucDeepFirstRun = 1;
    memset(&s_stDeepAttrib,0,sizeof(s_stDeepAttrib));
    DeepPlow_UpdatePram();
}
/*******************************************************************************
** 函数名称: DeepPlow_GetSubjoinInfoCurDeepVal
** 功能描述: 取得当前耕深值附加信息包(包括附加信息ID、长度、附加信息体)
** 入口参数: 需要保存耕深值附加信息包的首地址
** 出口参数: 
** 返回参数: 耕深值附加信息包的总长度
*******************************************************************************/
unsigned char DeepPlow_GetSubjoinInfoCurDeepVal(unsigned char *data)
{
    unsigned char buffer[10]={0};
    unsigned char len;
    if(0==Task_GetCurSystemIsDeepFlag())return 0;//未选择耕深
    len = 0;    
    buffer[len++] = 0xEA;//附加信息ID
    buffer[len++] = 4;   //附加长度
    Public_ConvertShortToBuffer(s_stDeepAttrib.curHeight,&buffer[len]);//附加信息体
    len += 2;
    Public_ConvertShortToBuffer(s_stDeepAttrib.baseHeight,&buffer[len]);//附加信息体
    //////////////////////////
    len += 2;
    ///////////////
    memcpy(data,buffer,len);
    /////////////////
    return len;
}
/*******************************************************************************
** 函数名称:DeepPlow_CalcCurDeepLength
** 功能描述: 取得当前油量附加信息包(包括附加信息ID、长度、附加信息体)
** 入口参数: 需要保存油量附加信息包的首地址
** 出口参数: 
** 返回参数: 油量附加信息包的总长度
*******************************************************************************/
void DeepPlow_CalcCurDeepLength(void)
{
    float bate,temp,sensorCur,tmpA,tmpAS;
    ///////////////
    sensorCur =Ad_GetValue(DEEP_SENSOR_ADC);
    ///////////////////////////
    tmpA  = s_stDeepAttrib.tmpA;
    tmpAS = s_stDeepAttrib.tmpAS;
    bate  = s_stDeepAttrib.bate;
    bate  = bate * fabs(sensorCur-(float)s_stDeepAttrib.sensorMin);
    temp  = s_stDeepAttrib.armLength;
    temp  = temp * (tmpA*cos(bate) - tmpAS*sin(bate));
    s_stDeepAttrib.curHeight = (unsigned short)((float)s_stDeepAttrib.maxLength  - temp + 0.5); 
}
/*******************************************************************************
** 函数名称:DeepPlow_GetCurDeepLength
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
*******************************************************************************/
unsigned short DeepPlow_GetCurDeepLength(void)
{
    return s_stDeepAttrib.curHeight;
}
/*******************************************************************************
** 函数名称:DeepPlow_GetResultIsQualified
** 功能描述: 是否合格
** 入口参数: 
** 出口参数: 
** 返回参数: 0合格,1不合格
*******************************************************************************/
unsigned char DeepPlow_GetResultIsQualified(void)
{
    return (s_stDeepAttrib.curHeight<s_stDeepAttrib.baseHeight)?1:0;
}
/*******************************************************************************
** 函数名称:DeepPlow_GetCheckParamResult
** 功能描述: 是否合格
** 入口参数: 
** 出口参数: 
** 返回参数: 0合格,1不合格
*******************************************************************************/
unsigned char DeepPlow_GetCheckParamResult(void)
{
    return s_stDeepAttrib.result;
}
/*********************************************************************
//函数名称	:DeepPlow_TimeTask
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
FunctionalState DeepPlow_TimeTask(void)
{
    if(0==Task_GetCurSystemIsDeepFlag())return ENABLE;
    DeepPlow_ParameterInitialize();
    DeepPlow_CalcCurDeepLength();
    return ENABLE;
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

