/********************************************************************
//版权说明  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称  :heartbeat.c        
//功能      :发送心跳包，保持线路连接
//版本号    :V0.1
//开发人    :dxl
//开发时间  :2009.12
//修改者    :
//修改时间  :
//修改简要说明  :
//备注      :
***********************************************************************/ 

//********************************头文件************************************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
//********************************自定义数据类型****************************

//********************************宏定义************************************

//********************************全局变量**********************************
u8 OneHourDelayTimeCountEnableFlag = 0;
u32 OneHourDelayTimeCount = 0;
//********************************外部变量**********************************
extern u8   BlindReportFlag;//盲区上报标志，1为要求上报或正上报，0为不要求
extern u8    LawlessAccAlarmEnableFlag;//非法点火报警使能标志，默认使能，收到人工确认报警后不使能,需要重启后才使能
extern u8   SpeedFlag;//速度类型,0为脉冲,1为GPS
//********************************本地变量**********************************
static u32  HeartbeatTimeSpace = 0;//心跳包时间间隔
static u32  Link1HeartbeatCount = 0;//连接1心跳时间计数
static u32  Link2HeartbeatCount = 0;//连接2心跳时间计数
//********************************函数声明**********************************

/*********************************************************************
//函数名称  :Heartbeat_TimeTask(void)
//功能      :每隔一定间隔发送一个心跳包
//输入      :
//输出      :
//使用资源  :
//全局变量  :
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
FunctionalState  HeartBeat_TimeTask(void)
{
    u8  Buffer[3] = {0};
    u8  SmsPhone[20] = {0};

    //计数加1
    Link1HeartbeatCount++;
        Link2HeartbeatCount++;
    
        if(HeartbeatTimeSpace < 10)//dxl,2015.4.21增加最小值限定,避免人工设置成1秒1条这种情况发生
        {
                HeartbeatTimeSpace = 10;
        }
    
    //if((Link1HeartbeatCount >= HeartbeatTimeSpace)&&(HeartbeatTimeSpace > 0))
        if(Link1HeartbeatCount >= HeartbeatTimeSpace)//dxl,2015.4.21
    {
        //计数清0
        Link1HeartbeatCount = 0;
        RadioProtocol_TerminalHeartbeat(CHANNEL_DATA_1,Buffer,SmsPhone);
    }
        
        //if((Link2HeartbeatCount >= HeartbeatTimeSpace)&&(HeartbeatTimeSpace > 0))
        if(Link2HeartbeatCount >= HeartbeatTimeSpace)//dxl,2015.4.21
    {
        //计数清0
        Link2HeartbeatCount = 0;
        RadioProtocol_TerminalHeartbeat(CHANNEL_DATA_2,Buffer,SmsPhone);
    }  
    return ENABLE;
}

/*********************************************************************
//函数名称  :HeartBeat_UpdatePram(void)
//功能      :更新心跳包时间间隔
//输入      :无参数
//输出      :
//使用资源  :
//全局变量  :
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
void HeartBeat_UpdatePram(void)
{
    u8  Buffer[5] = {0};
    u8  PramLen;
    PramLen = EepromPram_ReadPram(E2_TERMINAL_HEARTBEAT_ID,Buffer);
    if(E2_TERMINAL_HEARTBEAT_LEN == PramLen)
    {   
        HeartbeatTimeSpace = 0;
        HeartbeatTimeSpace |= Buffer[0] << 24;
        HeartbeatTimeSpace |= Buffer[1] << 16;
        HeartbeatTimeSpace |= Buffer[2] << 8;
        HeartbeatTimeSpace |= Buffer[3];
                if(HeartbeatTimeSpace < 10)//dxl,2015.4.21增加最小值限定,避免人工设置成1秒1条这种情况发生
                {
                        HeartbeatTimeSpace = 10;
                }
    }
    else
    {
        HeartbeatTimeSpace = 60;//默认值,实际应用使用
    }   
}
/*********************************************************************
//函数名称  :Heartbeat_ClrHeartbeatCount(u8 channel)
//功能      :清除变量HeartbeatCount
//输入      :无参数
//输出      :
//使用资源  :
//全局变量  :
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
void Heartbeat_ClrHeartbeatCount(u8 channel)
{
    if(CHANNEL_DATA_1 == channel)
    {
        Link1HeartbeatCount = 0;  
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        Link2HeartbeatCount = 0;
    }   
}


