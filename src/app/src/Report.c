/************************************************************************
//程序名称：Report.c
//功能：该模块实现定时上报、定距上报、定时+定距上报、实时跟踪功能，支持双连接的位置汇报。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.10
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：需要实现双连接的位置汇报，连接1按照行标标准进行位置汇报，连接2的TCP端口若设置为0表示关闭第2个连接，
//即也会关闭连接2的位置汇报，当为非0时表示开启连接2。当第2个连接开启后，第2个连接的ACC ON和ACC OFF上报时间间隔设置为非0，
//则表示开启第2个连接的特殊上报功能，否则按连接1的方式上报。
//V0.2：增加了附加信息ID 0xE9，当参数0xF283设置为1时才会增加此附加信息ID 
//V0.3：修改脉冲速度附加ID替换方法，2015.5.11，dxl
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include "Report.h"
#include "spi_flashapi.h"
#include "EepromPram.h"
#include "Public.h"
#include "fm25c160.h"
#include "Blind.h"
#include "Gps_App.h"
#include "RadioProtocol.h"
#include "GPIOControl.h"
#include "Io.h"
#include "NaviLcd.h"
#include "SpeedMonitor.h"
#include "MileMeter.h"
#include "JointechOiL_Protocol.h"
#include "LzmSys.h"
#include "AreaManage.h"
#include "Route.h"
#include "temperature.h"
#include "VDR.h"
#include "VDR_Pulse.h"
#include "DeepPlow.h"
#include "turnspeed.h"

/********************本地变量*************************/
static u32 Link1ReportTime = 0;//连接1位置汇报时间间隔，单位秒
static u32 Link1ReportTimeCount = 0;//连接1位置汇报时间计数
static u32 Link1LastReportDistance = 0;//连接1上一个上报点的距离，单位米
static u32 Link1ReportDistance = 0;//连接1下一个上报点的距离
static u8 Link1ReportAckFlag = 0;//连接1上一条位置信息应答标志，1表示有应答，0表示无应答
static u8 Link1ReportBuffer[FLASH_BLIND_STEP_LEN] = {0};//连接1位置上报缓冲
static u8 Link1ReportBufferLen = 0;//连接1位置上报缓冲内容长度
static u32 Link2AccOnReportTimeSpace = 0;//连接2 ACC ON位置汇报时间间隔，为0表示关闭该功能
static u32 Link2AccOffReportTimeSpace = 0;//连接2 ACC OFF位置汇报时间间隔，为0表示关闭该功能
static u8 Link2OpenFlag = 0;//通道2开启标志，1为开启，0为关闭
static u32 Link2ReportTime = 0;//连接2 位置汇报时间间隔，单位秒
static u32 Link2ReportTimeCount = 0;//连接2 位置汇报时间计数
static u8 Link2ReportAckFlag = 0;//上一条位置信息应答标志，1表示有应答，0表示无应答
static u8 Link2ReportBuffer[FLASH_BLIND_STEP_LEN] = {0};//位置上报缓冲
static u8 Link2ReportBufferLen = 0;//位置上报缓冲内容长度
static u32 Link2ReportDistanceSpace = 0;//连接2位置上报时间间隔，单位米
static u32 ReportStrategy = 0;//汇报策略
static u32 ReportScheme = 0;//汇报方案
static u32 EmergencyReportTimeSpace = 0;//紧急报警汇报时间间隔
static u32 EmergencyReportDistanceSpace = 0;//紧急报警汇报距离间隔
static u32 OverSpeedReportTimeSpace = 0;//超速报警汇报时间间隔
static u32 OverTimeReportTimeSpace = 0;//超时报警汇报时间间隔
static u32 AccOnReportTimeSpace = 0;//缺省汇报时间间隔
static u32 AccOnReportDistanceSpace = 0;//缺省汇报时间间隔
static u32 AccOffReportTimeSpace = 0;//休眠汇报时间间隔
static u32 AccOffReportDistanceSpace = 0;//休眠汇报时间间隔
static u32 UnloginReportTimeSpace = 0;//驾驶员未登录汇报时间间隔
static u32 UnloginReportDistanceSpace = 0;//驾驶员未登录汇报时间间隔
static u32 TempTrackTimeSpace = 0;//临时跟踪时间间隔
static u32 TempTrackCount = 0;//临时跟踪次数
static u32 ReportEventSwitch = 0;//位置上报事件项开关，bit0:为1表示开启超速报警位置上报；bit1:为1表示开启超时报警位置上报

/********************全局变量*************************/
u16 Link1LastReportSerialNum = 0;//连接1上一条位置上报的流水号
u16 Link2LastReportSerialNum = 0;//连接2上一条位置上报的流水号
u32 Link1ReportTimeSpace = 0;//同Link1ReportTime一样，用作外部访问
u32 Link1ReportDistanceSpace = 0;//连接1位置上报时间间隔，单位米
u8  AreaInOutAlarmClearEnableFlag = 0;//1为使能，0为禁止
u8  RouteInToOutFlag = 0;
/********************外部变量*************************/
extern TIME_T CurTime;
extern u8 SpeedFlag;
//extern u8 UpdataReportPram;   
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
extern u8  BBXYTestFlag;//0为正常模式，1为部标协议检测模式
extern u8  HYTestMachine1Flag;//0为正常模式，1为协议检测1号机模式，脉冲+GPS速度
extern u8  HYTestMachine2Flag;//0为正常模式，1为协议检测2号机模式，纯GPS速度
extern u8  HYTestMachine3Flag;//0为正常模式，1为协议检测3号机模式，纯脉冲速度 
//extern u8  RouteOutToInFlag;
/********************本地函数声明*********************/
static void Report_GetLink1NextReportPoint(u32 *NextReportTime, u32 *NextReportDistance);//获取下一个上报点
static u32 Report_GetCurDistance(void);//获取当前的累计行驶里程
static u16 Report_GetPositionAdditionalInfo(u8 *pBuffer);//获取位置附加信息
static u16 Report_GetPositionStdAdditionalInfo(u8 *pBuffer);//获取标准的位置附加信息
static u16 Report_GetPositionEiAdditionalInfo(u8 *pBuffer);//获取伊爱自定义的位置附加信息
static u8 Report_Link2ReportOpenFlag(void);//连接2特殊上报开启标志，1为开启，0为关闭
static u8 Report_GetManualAlarmSubjoinInfo(u8 *pBuffer);
/********************函数定义*************************/

/**************************************************************************
//函数名：Report_TimeTask
//功能：实现位置汇报（包括实时跟踪、紧急报警、超速报警、超时报警、定时定距等位置汇报）
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：位置汇报定时任务，50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState Report_TimeTask(void)
{
	  
	  u32 CurTimeCount = 0;
    u32 Link1CurDistance = 0;//当前位置点距离
    u8 Link1ReportFlag = 0;//位置上报标志，1表示要上报，0表示不用上报
    u8 Link2ReportFlag = 0;
    u8 Link2ReportStatus = 0;//第2个连接上报状态，0表示静止上报，1表示按连接1的方式上报，2表示按连接2的特殊方式上报
    u8 Ack = 0;
    u8 Acc = 0;
    u8 TempBuffer[5];
	  GPS_STRUCT Position;
	  TIME_T Time;
    
    static u8 Link1LastSendFlag = 0;
    static u8 Link2LastSendFlag = 0;
	  static GPS_STRUCT LastPosition;
	  static u32 count = 0;
	  static u32 LastTimeCount = 0;

	  
	  count++;//dxl,2016.5.7增加
    if(count < 1200)//启动后前60秒不上报位置信息，因为这段时间是连网过程，肯定上报不上去，会作为盲区记录的，这样每次启动就会上来几条盲区，为避免这种情况发生加60秒延时
    {
			  return ENABLE;  
    }
		else if(count == 1200)
		{
			  Gps_CopyPosition(&LastPosition);
		    Report_UpdatePram();
			  LastTimeCount = RTC_GetCounter();
			  return ENABLE;
		}
		else
		{
			  if(1 == BBGNTestFlag)
				{
					  if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))//导航
						{
		            Gps_CopyPosition(&Position);
						}
						else//不导航
						{
						    RTC_GetCurTime(&Time);
							  Position.Second = Time.sec;
						}
						
			      if(Position.Second == LastPosition.Second)
				    {
				        return ENABLE;
				    }
				    else//1秒定时时间到
				    {
				        LastPosition.Second = Position.Second;
				    }
			  }
				else
				{
				    CurTimeCount = RTC_GetCounter();
				    if((CurTimeCount != LastTimeCount))
				    {
				         LastTimeCount = CurTimeCount;
				    }
				    else
				    {
				         return ENABLE;
				    }
				}		
		}

		
		
    if(0 != Link2OpenFlag)
    {
        if(1==Report_Link2ReportOpenFlag())
        {
            Link2ReportStatus = 2;//已开启连接2的特殊汇报
        }
        else
        {
            Link2ReportStatus = 1;//已开启第2个连接，按连接1的方式汇报
        }
    }
    else
    {
        Link2ReportStatus = 0;//已关闭连接2
    }

    

    Link1ReportTimeCount++;
		
    Report_GetLink1NextReportPoint(&Link1ReportTime,&Link1ReportDistance);

    if(0 != Link1ReportTime)//Link1ReportTime等于0表示不开启定时上报
    {
        if(Link1ReportTimeCount >= Link1ReportTime)
        {
            Link1ReportFlag = 1;
            if(1 == Link2ReportStatus)
            {
                Link2ReportFlag = 1;
                Link2ReportTime = Link1ReportTime;
                
            }
            if((TempTrackTimeSpace > 0)&&(TempTrackCount > 0))
            {
                TempTrackCount--;
                Public_ConvertLongToBuffer(TempTrackCount,TempBuffer);
                FRAM_BufferWrite(FRAM_TEMP_TRACK_NUM_ADDR,TempBuffer,4);
                if(0 == TempTrackCount)
                {
                    TempTrackTimeSpace = 0;
                    Public_ConvertLongToBuffer(TempTrackTimeSpace,TempBuffer);
                    FRAM_BufferWrite(FRAM_TEMP_TRACK_SPACE_ADDR,TempBuffer,4);
                }
            }
        }
    }

    if(0 != Link1ReportDistance)//Link1ReportDistance等于0表示不开启定距上报
    {
        Link1CurDistance = Report_GetCurDistance();
        if(Link1CurDistance >= Link1ReportDistance)
        {
            Link1ReportFlag = 1;
            if(1 == Link2ReportStatus)
            {
                Link2ReportFlag = 1;
                
            }
        }
    }

    if(1 == Link1ReportFlag)
    {
        if(1 == Link1LastSendFlag)
        {
            Link1LastSendFlag = 0;
            Link1LastReportDistance = Report_GetCurDistance();
            if(((0 == Link1ReportAckFlag)&&(Link1ReportTime >= 5))
                            ||((0 == Link1ReportAckFlag)&&(Link1ReportDistanceSpace >= 100)))//上一条位置信息汇报是否收到应答，1表示收到
            {
                //if(1 == Gps_ReadStatus())//只有导航才会上报，状态或报警变化也会上报当不在这个任务处理
                //{
                                  if(1 == BBGNTestFlag)//dxl,2016.4.7兼容部标检测和实际应用
                                        {
                                            Link1ReportBufferLen = 28;//只记录基本位置信息
                                        }
                    Blind_Save(CHANNEL_DATA_1,Link1ReportBuffer,Link1ReportBufferLen);
                //}
            }
        }

                
        Link1ReportBufferLen = Report_GetPositionInfo(Link1ReportBuffer);

        Ack = RadioProtocol_PostionInformationReport(CHANNEL_DATA_1,Link1ReportBuffer,Link1ReportBufferLen);
        RadioProtocol_GetUploadCmdChannel(0x0200,&Link1LastReportSerialNum);
        if(ACK_OK != Ack)
        {
            if(((Link1ReportTime < 5)&&(Link1ReportTime > 0))
                          ||((Link1ReportDistanceSpace < 100)&&(Link1ReportDistanceSpace > 0)))
            {
                //if(1 == Gps_ReadStatus())//只有导航才会上报，状态或报警变化也会上报当不在这个任务处理
                //{
                                  if(1 == BBGNTestFlag)//dxl,2016.4.7兼容部标检测和实际应用
                                        {
                                            Link1ReportBufferLen = 28;//只记录基本位置信息
                                        }
                    Blind_Save(CHANNEL_DATA_1,Link1ReportBuffer,Link1ReportBufferLen);
                //}
            }
                
        }
        Link1LastSendFlag = 1;
        Link1ReportTimeCount = 0;
        Link1LastReportDistance = Link1CurDistance;
        Link1ReportAckFlag = 0; 
        
    }

    if(2 == Link2ReportStatus)
    {
        Link2ReportTimeCount++;

        Acc = Io_ReadStatusBit(STATUS_BIT_ACC);
        if(1 == Acc)
        {
            Link2ReportTime = Link2AccOnReportTimeSpace;
        }
        else
        {
            Link2ReportTime = Link2AccOffReportTimeSpace;
        }

        if(0 != Link2ReportTime)//Link2ReportTime等于0表示不开启定时上报
        {
            if(Link2ReportTimeCount >= Link2ReportTime)
            {
                Link2ReportFlag = 1;
            }
        }
    }

    if(1 == Link2ReportFlag)
    {
        if(1 == Link2LastSendFlag)
        {
            Link2LastSendFlag = 0;
            if(((0 == Link2ReportAckFlag)&&(Link2ReportTime >= 5))
                            ||((0 == Link2ReportAckFlag)&&(Link2ReportDistanceSpace >= 100)))//上一条位置信息汇报是否收到应答，1表示收到
            {
                //if(1 == Gps_ReadStatus())
                //{
                                  if(1 == BBGNTestFlag)//dxl,2016.4.7兼容部标检测和实际应用
                                        {
                                            Link1ReportBufferLen = 28;//只记录基本位置信息
                                        }
                    Blind_Save(CHANNEL_DATA_2,Link2ReportBuffer,Link2ReportBufferLen);
                //}
            }
        }
        
        Link2ReportBufferLen = Report_GetPositionInfo(Link2ReportBuffer);
        Ack = RadioProtocol_PostionInformationReport(CHANNEL_DATA_2,Link2ReportBuffer,Link2ReportBufferLen);
        RadioProtocol_GetUploadCmdChannel(0x0200,&Link2LastReportSerialNum);
        if(ACK_OK != Ack)
        {
            if(((Link2ReportTime < 5)&&(Link2ReportTime > 0))||
                ((Link2ReportDistanceSpace > 0)&&(Link2ReportDistanceSpace < 100)))
            {
                
                //if(1 == Gps_ReadStatus())//只有导航才会上报，状态或报警变化也会上报当不在这个任务处理
                //{
                                  if(1 == BBGNTestFlag)//dxl,2016.4.7兼容部标检测和实际应用
                                        {
                                            Link1ReportBufferLen = 28;//只记录基本位置信息+附加ID为0x01（里程）的内容
                                        }
                    Blind_Save(CHANNEL_DATA_2,Link2ReportBuffer,Link2ReportBufferLen);
                //}
                
            }
        }
        Link2LastSendFlag = 1;  
        Link2ReportTimeCount = 0;
        Link2ReportAckFlag = 0;
        
    }

    return ENABLE;      
}
/**************************************************************************
//函数名：Report_UpdatePram
//功能：更新位置汇报所有变量
//输入：无
//输出：无
//返回值：无
//备注：主要是更新位置汇报模块所使用到的变量，运行任务调度之前需调用此函数
***************************************************************************/
void Report_UpdatePram(void)
{
    u8 i;
    for(i=PRAM_REPORT_STRATEGY; i<=PRAM_INDEX_MAX; i++)
    {
        Report_UpdateOnePram(i);
    }
    UpdateRunMode();
}
/**************************************************************************
//函数名：Report_CmdAck
//功能：位置上报应答处理
//输入：通道号
//输出：无
//返回值：无
//备注：收到位置上报的通用应答时需调用此函数
***************************************************************************/
void Report_CmdAck(u8 channel)
{
    if(CHANNEL_DATA_1 == channel)
    {
        Link1ReportAckFlag = 1;
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        Link2ReportAckFlag = 1;
    }
}
/**************************************************************************
//函数名：Report_UpdateOnePram
//功能：更新某个参数
//输入：参数类型
//输出：该参数类型对应的变量
//返回值：无
//备注：串口或远程设置位置汇报相关参数时需要调用此函数
***************************************************************************/
void Report_UpdateOnePram(u8 type)
{
    u8 Buffer[30];
    u8 BufferLen;
    u32 TcpPort;
    
    switch(type)
    {
    case PRAM_REPORT_STRATEGY://位置汇报策略
        {
            if(E2_POSITION_REPORT_STRATEGY_LEN == EepromPram_ReadPram(E2_POSITION_REPORT_STRATEGY_ID,Buffer))
            {
                ReportStrategy = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_REPORT_SCHEME://位置汇报方案
        {
            if(E2_POSITION_REPORT_SCHEME_LEN == EepromPram_ReadPram(E2_POSITION_REPORT_SCHEME_ID,Buffer))
            {
                ReportScheme = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_TEMP_TRACK_TIME://临时跟踪时间间隔
        {
            if(FRAM_TEMP_TRACK_SPACE_LEN == FRAM_BufferRead(Buffer, 2, FRAM_TEMP_TRACK_SPACE_ADDR))
            {
                TempTrackTimeSpace = Public_ConvertBufferToShort(Buffer);
            }
            break;
        }
    case PRAM_TEMP_TRACK_COUNT://临时跟踪次数
        {
            if(FRAM_TEMP_TRACK_NUM_LEN == FRAM_BufferRead(Buffer, 4, FRAM_TEMP_TRACK_NUM_ADDR))
            {
                TempTrackCount = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_EMERGENCY_REPORT_TIME://紧急报警汇报时间间隔
        {
            if(E2_EMERGENCY_REPORT_TIME_LEN == EepromPram_ReadPram(E2_EMERGENCY_REPORT_TIME_ID,Buffer))
            {
                EmergencyReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_EMERGENCY_REPORT_DISTANCE://紧急报警汇报距离间隔
        {
            if(E2_EMERGENCY_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_EMERGENCY_REPORT_DISTANCE_ID,Buffer))
            {
                EmergencyReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_OVER_SPEED_REPORT_TIME://超速报警汇报时间间隔
        {
            if(E2_OVERSPEED_ALARM_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_OVERSPEED_ALARM_REPORT_TIME_ID,Buffer))
            {
                OverSpeedReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_OVER_TIME_REPORT_TIME://超时报警汇报时间间隔
        {
            if(E2_TIRED_DRIVE_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_TIRED_DRIVE_REPORT_TIME_ID,Buffer))
            {
                OverTimeReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_ON_REPORT_TIME://缺省汇报时间间隔
        {
            if(E2_ACCON_REPORT_TIME_LEN == EepromPram_ReadPram(E2_ACCON_REPORT_TIME_ID,Buffer))
            {
                AccOnReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_ON_REPORT_DISTANCE://缺省汇报距离间隔
        {
            if(E2_ACCON_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_ACCON_REPORT_DISTANCE_ID,Buffer))
            {
                AccOnReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_OFF_REPORT_TIME://休眠汇报时间间隔
        {
            if(E2_SLEEP_REPORT_TIME_LEN == EepromPram_ReadPram(E2_SLEEP_REPORT_TIME_ID,Buffer))
            {
                AccOffReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_OFF_REPORT_DISTANCE://休眠汇报距离间隔
        {
            if(E2_SLEEP_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_SLEEP_REPORT_DISTANCE_ID,Buffer))
            {
                AccOffReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_UNLOGIN_REPORT_TIME://驾驶员未登录汇报时间间隔
        {
            if(E2_DRIVER_UNLOGIN_REPORT_TIME_LEN == EepromPram_ReadPram(E2_DRIVER_UNLOGIN_REPORT_TIME_ID,Buffer))
            {
                UnloginReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_UNLOGIN_REPORT_DISTANCE://驾驶员未登录汇报距离间隔
        {
            if(E2_DRIVER_UNLOGIN_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ID,Buffer))
            {
                UnloginReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_SECOND_LINK_ACC_ON_REPORT_TIME://连接2 ACC ON汇报时间间隔
        {
            if(E2_SECOND_ACC_ON_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_SECOND_ACC_ON_REPORT_TIME_ID,Buffer))
            {
                Link2AccOnReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            
            break;
        }
    case PRAM_SECOND_LINK_ACC_OFF_REPORT_TIME://连接2 ACC OFF汇报时间间隔
        {
            
            if(E2_SECOND_ACC_OFF_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_SECOND_ACC_OFF_REPORT_TIME_ID,Buffer))
            {
                Link2AccOffReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            
            break;
        }
    case PRAM_SECOND_LINK_OPEN_FLAG://连接2 开启标志
        {   
            BufferLen = EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_IP_ID, Buffer);
            if(0 == BufferLen)
            {
                Link2OpenFlag = 0;
            }
            else
            {
                BufferLen = EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_TCP_PORT_ID, Buffer);
                if(E2_SECOND_MAIN_SERVER_TCP_PORT_ID_LEN == BufferLen)
                {
                    TcpPort = Public_ConvertBufferToLong(Buffer);
                    if(0 == TcpPort)
                    {
                        Link2OpenFlag = 0;
                    }
                    else
                    {
                        Link2OpenFlag = 1;
                    }
                }
                else
                {
                    Link2OpenFlag = 0;
                }
        
            }
            
            break;
        }
    case PRAM_REPORT_EVENT_SWITCH://位置上报事件项开关
        {
            
            if(E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN == EepromPram_ReadPram(E2_REPORT_FREQ_EVENT_SWITCH_ID,Buffer))
            {
                ReportEventSwitch = Public_ConvertBufferToLong(Buffer);
            }
            
            break;
        }
    
        default:break;
    }
		
		Link1ReportTimeCount = 0;//设置参数时，定时计数清0
		Link2ReportTimeCount = 0;
		
}
/**************************************************************************
//函数名：Report_GetOnePram
//功能：获取某个参数
//输入：参数类型
//输出：该参数类型对应的变量
//返回值：无
//备注：
***************************************************************************/
u32 Report_GetOnePram(u8 type)
{
    u32 temp = 0;
    
    switch(type)
    {
    case PRAM_REPORT_STRATEGY://位置汇报策略
        {
            temp = ReportStrategy;
            break;
        }
    case PRAM_REPORT_SCHEME://位置汇报方案
        {
            temp = ReportScheme;
            break;
        }
    case PRAM_EMERGENCY_REPORT_TIME://紧急报警汇报时间间隔
        {
						temp = EmergencyReportTimeSpace;
            break;
        }
    case PRAM_EMERGENCY_REPORT_DISTANCE://紧急报警汇报距离间隔
        {
						temp = EmergencyReportDistanceSpace;
            break;
        }
    case PRAM_OVER_SPEED_REPORT_TIME://超速报警汇报时间间隔
        {
						temp = OverSpeedReportTimeSpace;
            break;
        }
    case PRAM_OVER_TIME_REPORT_TIME://超时报警汇报时间间隔
        {
						temp = OverTimeReportTimeSpace;
            break;
        }
    case PRAM_ACC_ON_REPORT_TIME://缺省汇报时间间隔
        {
						temp = AccOnReportTimeSpace;
            break;
        }
    case PRAM_ACC_ON_REPORT_DISTANCE://缺省汇报距离间隔
        {
						temp = AccOnReportDistanceSpace;
            break;
        }
    case PRAM_ACC_OFF_REPORT_TIME://休眠汇报时间间隔
        {
						temp = AccOffReportTimeSpace;
            break;
        }
    case PRAM_ACC_OFF_REPORT_DISTANCE://休眠汇报距离间隔
        {
						temp = AccOffReportDistanceSpace;
            break;
        }
    case PRAM_UNLOGIN_REPORT_TIME://驾驶员未登录汇报时间间隔
        {
						temp = UnloginReportTimeSpace;
            break;
        }
    case PRAM_UNLOGIN_REPORT_DISTANCE://驾驶员未登录汇报距离间隔
        {
						temp = UnloginReportDistanceSpace;
            break;
        }
    case PRAM_SECOND_LINK_ACC_ON_REPORT_TIME://连接2 ACC ON汇报时间间隔
        {
            temp = Link2AccOnReportTimeSpace;
            break;
        }
    case PRAM_SECOND_LINK_ACC_OFF_REPORT_TIME://连接2 ACC OFF汇报时间间隔
        {
            temp = Link2AccOffReportTimeSpace;
            break;
        }
    default:
			  break;
    }
		
		return temp;
}
/**************************************************************************
//函数名：Report_UploadPositionInfo
//功能：上传一条位置信息
//输入：通道号
//输出：无
//返回值：0上传成功，1上传失败
//备注：当前的位置信息==位置基本信息+位置附加信息，位置信息最长为150字节
***************************************************************************/
u8 Report_UploadPositionInfo(u8 channel)
{
    u8 Buffer[FLASH_BLIND_STEP_LEN+1];
    u8 BufferLen;
    u8 flag1;
    u8 flag2;
    BufferLen = Report_GetPositionInfo(Buffer);

    if(CHANNEL_DATA_1 == channel)
    {

        if(ACK_OK == RadioProtocol_PostionInformationReport(channel,Buffer,BufferLen))
        {
            return 0;
        }
        else
        {
					  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))//dxl,2016.5.16只有定时汇报时才记录盲区
						{
						
						}
						else
						{
                Blind_Save(channel, Buffer, BufferLen);
						}
            return 1;
        }
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        if(1 == Link2OpenFlag)
        {
            if(ACK_OK == RadioProtocol_PostionInformationReport(channel,Buffer,BufferLen))
            {
                return 0;
            }
            else
            {
							  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))//dxl,2016.5.16只有定时汇报时才记录盲区
						    {
						
						    }
								else
								{
                    Blind_Save(channel, Buffer, BufferLen);
								}
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
    else if(0x06 == channel)
    {
        if(ACK_OK == RadioProtocol_PostionInformationReport(CHANNEL_DATA_1,Buffer,BufferLen))
        {
            flag1 = 0;
        }
        else
        {
					  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))//dxl,2016.5.16只有定时汇报时才记录盲区
						{
						
						}
					  else
					  {
                Blind_Save(CHANNEL_DATA_1, Buffer, BufferLen);
						}
            flag1 = 1;
        }

        if(ACK_OK == RadioProtocol_PostionInformationReport(CHANNEL_DATA_2,Buffer,BufferLen))
        {
            flag2 = 0;
        }
        else
        {
					  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))//dxl,2016.5.16只有定时汇报时才记录盲区
						{
						
						}
					  else
						{
                Blind_Save(CHANNEL_DATA_2, Buffer, BufferLen);
						}
            flag2 = 1;
        }

        if((0 == flag1)&&(0 == flag2))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}
/**************************************************************************
//函数名：Report_GetPositionInfo
//功能：获取当前的位置信息
//输入：无
//输出：位置信息
//返回值：位置信息长度
//备注：当前的位置信息==位置基本信息+位置附加信息，位置信息最长为150字节
***************************************************************************/
u16 Report_GetPositionInfo(u8 *pBuffer)
{
    u8 *p = NULL;
    u16 length;

    p = pBuffer;
    length = 0;

    length = Report_GetPositionBasicInfo(p);//获取位置基本信息
	  //if(length != 28)//位置基本信息里数据异常时返回的长度将不等于28
		//{
		    //return 0;
		//}
    p = p+length;

    length += Report_GetPositionAdditionalInfo(p);//获取位置附加信息

    return length;

}
/**************************************************************************
//函数名：Report_GetPositionBasicInfo
//功能：获取位置基本信息
//输入：无
//输出：位置基本信息
//返回值：位置基本信息长度
//备注：长度固定为28字节，位置基本信息：报警字、状态字、纬度、经度、高程、速度、方向、时间
***************************************************************************/
u16 Report_GetPositionBasicInfo(u8 *pBuffer)
{
    u8 *p = NULL;
    u8  NavigationFlag;
	  u8  i;
	  u8  RtcTimeFlag;
	  u8  CurrentAlarmFlag;
    u16 temp2;
	  u32 temp;
	  u32 timecount;
    GPS_STRUCT Position;
    TIME_T time;
	  TIME_T Time;
	  static u32  AccOnCount = 0;
	  static u8  LastAlarmFlag = 0;
	
    p = pBuffer;
	  
    if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))
		{
			  RtcTimeFlag = 0;
		    Gps_CopyPosition(&Position);
			  if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))//ACC OFF时不解析定位数据，由OFF变ON时第1条位置信息的时间还是保持在OFF时的时间
		    {
					   AccOnCount++;
					   if(AccOnCount < 3)
						 {
		             RtcTimeFlag = 1;
						 }
		    }
				else
				{
				     AccOnCount = 0;
				}
		}
		else
		{
			  RtcTimeFlag = 1;
		    Gps_CopygPosition(&Position);
		}
		
		
		
    temp = Io_ReadAlarm();//报警字
	  if(1 == BBGNTestFlag)
	  {
	      CurrentAlarmFlag = Io_ReadAlarmBit(ALARM_BIT_LOAD_EXCURSION);
	      if((1 == CurrentAlarmFlag)&&(0 == LastAlarmFlag))//第1次报偏离路线
		    {
			      RouteInToOutFlag = 1;//出路线标志
		    }
		    LastAlarmFlag = CurrentAlarmFlag;
				
				if(1 == RouteInToOutFlag)
				{
				    temp |= (1<<21);//置位进出路线标志
				}
	  }
	
    Public_ConvertLongToBuffer(temp,p);
    p += 4;

    temp = Io_ReadStatus();//状态字
		NavigationFlag = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);
		if(((1 == BBGNTestFlag)&&(0 == NavigationFlag))||
			((1 == BBXYTestFlag)&&(0 == NavigationFlag)))//dxl,2016.5.17 不定位时，bit18-bit21全为0
		{
		    temp &= 0xffc3ffff;
		}
//		else if(1 == HYTestMachine3Flag)//dxl,2016.7.20 3号机协议检测时，bit18-bit21,bit1-bit3全为0
//		{
	//	    temp &= 0xffc3fff1;
	//	}
		
    Public_ConvertLongToBuffer(temp,p);
    p += 4;

	  //NavigationFlag = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);
		if(((1 == BBGNTestFlag)&&(0 == NavigationFlag))||
			((1 == BBXYTestFlag)&&(0 == NavigationFlag)))
//		  (1 == HYTestMachine3Flag))//dxl,2016.5.17 不定位时，全为0
		{
		    for(i=0; i<14; i++)
			  {
				    *p++ = 0;
				}
		}
		else
		{
        temp = (Position.Latitue_FX*100UL+Position.Latitue_F*1000000)/60+(Position.Latitue_D*1000000UL);//纬度
        Public_ConvertLongToBuffer(temp,p);
        p += 4;

        temp = (Position.Longitue_FX*100UL+Position.Longitue_F*1000000)/60+(Position.Longitue_D*1000000UL);//经度
        Public_ConvertLongToBuffer(temp,p);
        p += 4;

        temp2 = Position.High+Position.HighOffset;//高程
        if((Position.High+Position.HighOffset)<0)temp2 = 0;
        Public_ConvertShortToBuffer(temp2,p);
        p += 2;

        temp2 = 10*Gps_ReadSpeed();//速度
        Public_ConvertShortToBuffer(temp2,p);
        p += 2;

        temp2 = Position.Course;//方向
        Public_ConvertShortToBuffer(temp2,p);
        p += 2;
		}

    
    time.year = Position.Year;
    time.month = Position.Month;
    time.day = Position.Date;
    time.hour = Position.Hour;
    time.min = Position.Minute;
    time.sec = Position.Second;
    timecount = ConverseGmtime(&time);
    timecount += 8*3600;
    Gmtime(&time, timecount);
		
		RTC_GetCurTime(&Time);
    
    if((1==Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
			&&(1==Io_ReadStatusBit(STATUS_BIT_ACC))
		  &&((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
		  &&(0 == RtcTimeFlag))//检测时采用GPS时间
    {
        *p++ = Public_HEX2BCD(time.year);//GPS时间
        *p++ = Public_HEX2BCD(time.month);
        *p++ = Public_HEX2BCD(time.day);
        *p++ = Public_HEX2BCD(time.hour);      
        *p++ = Public_HEX2BCD(time.min);
        *p++ = Public_HEX2BCD(time.sec);
			/*
			  if(LastTimeCount == timecount)//过滤同一个时刻点，出现两条位置信息，1条是导航，另1条是不导航的情况，盲区测试时曾出现过
				{
				    if(NavigationFlag != LastNavigationFlag)
						{
						    return 0;
						}
				}
			  LastTimeCount = timecount;
				LastNavigationFlag = NavigationFlag;
			*/
    }
    else //实际应用时采用RTC时间
    {
        *p++ = Public_HEX2BCD(Time.year);    //RTC时间
        *p++ = Public_HEX2BCD(Time.month);
        *p++ = Public_HEX2BCD(Time.day);
        *p++ = Public_HEX2BCD(Time.hour);
        *p++ = Public_HEX2BCD(Time.min);
        *p++ = Public_HEX2BCD(Time.sec);
			/*
			  if(LastTimeCount == timecount)//过滤同一个时刻点，出现两条位置信息，1条是导航，另1条是不导航的情况，盲区测试时曾出现过
				{
				    if(NavigationFlag != LastNavigationFlag)
						{
						    return 0;
						}
				}
			  LastTimeCount = RTC_GetCounter();
				LastNavigationFlag = NavigationFlag;
			*/
    }

    return 28;
    
}
/**************************************************************************
//函数名：Report_ClearReportTimeCount
//功能：上报时间计数变量清0
//输入：无
//输出：无
//返回值：无
//备注：两个连接的计数变量都清0
***************************************************************************/
void Report_ClearReportTimeCount(void)
{
    Link1ReportTimeCount = 0;
    Link2ReportTimeCount = 0;
}
/**************************************************************************
//函数名：Report_GetReportEventSwitch
//功能：获取ReportEventSwitch的值
//输入：无
//输出：无
//返回值：无
//备注：ReportEventSwitch的值等于参数ID 0xF27D的设置值
***************************************************************************/
u32 Report_GetReportEventSwitch(void)
{
    return ReportEventSwitch;
}
/**************************************************************************
//函数名：Report_GetOverSpeedReportTimeSpace
//功能：获取OverSpeedReportTimeSpace的值
//输入：无
//输出：无
//返回值：无
//备注：OverSpeedReportTimeSpace的值等于参数ID 0xF259的设置值
***************************************************************************/
u32 Report_GetOverSpeedReportTimeSpace(void)
{
    return OverSpeedReportTimeSpace;
}
/**************************************************************************
//函数名：Report_GetOverTimeReportTimeSpace
//功能：获取OverTimeReportTimeSpace的值
//输入：无
//输出：无
//返回值：无
//备注：OverTimeReportTimeSpace的值等于参数ID 0xF277的设置值
***************************************************************************/
u32 Report_GetOverTimeReportTimeSpace(void)
{
    return OverTimeReportTimeSpace;
}
/**************************************************************************
//函数名：Report_Link2ReportOpenFlag
//功能：连接2自定义汇报是否需要开启
//输入：无
//输出：1：需要开启；0：不需要开启
//返回值：无
//备注：
***************************************************************************/
static u8 Report_Link2ReportOpenFlag(void)
{
    if((TempTrackTimeSpace > 0)&&(TempTrackCount > 0))//临时跟踪
    {
        return 0;
    }
    else if(1 == Io_ReadAlarmBit(ALARM_BIT_EMERGENCY))//紧急报警
    {
        return 0;
    }
    else if((1 == Io_ReadAlarmBit(ALARM_BIT_OVER_SPEED))&&(0 != OverSpeedReportTimeSpace)&&(0x00000001 == (0x00000001&ReportEventSwitch)))//超速报警
    {
        return 0;
    }
    else if((1 == Io_ReadAlarmBit(ALARM_BIT_TIRED_DRIVE))&&(0 != OverTimeReportTimeSpace)&&(0x00000002 == (0x00000002&ReportEventSwitch)))//超时报警
    {
        return 0;
    }
    else if((0 != Link2AccOnReportTimeSpace)&&(0 != Link2AccOffReportTimeSpace))//连接2参数已设置
    {
        return 1;
    }
    else 
    {
        return 0;
    }

    
}
/**************************************************************************
//函数名：Report_GetLink1NextReportPoint
//功能：获取连接1的下一个位置汇报点（汇报时间和汇报距离）
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void Report_GetLink1NextReportPoint(u32 *NextReportTime, u32 *NextReportDistance)
{
    u8 CurOverSpeedFlag = 0;
    u8 CurOverTimeFlag = 0;

    static u8 LastOverSpeedFlag = 0;
    static u8 LastOverTimeFlag = 0;

    CurOverSpeedFlag = Io_ReadAlarmBit(ALARM_BIT_OVER_SPEED);
    CurOverTimeFlag = Io_ReadAlarmBit(ALARM_BIT_TIRED_DRIVE);

    *NextReportTime = 0;
    *NextReportDistance = 0;

    if((TempTrackTimeSpace > 0)&&(TempTrackCount > 0))//临时跟踪
    {
        *NextReportTime = TempTrackTimeSpace;
        *NextReportDistance = 0;
        Link1ReportDistanceSpace = 0;
    }
    else
    {
        if(1 == Io_ReadAlarmBit(ALARM_BIT_EMERGENCY))//紧急报警
        {
            if(0 == ReportStrategy)//定时
            {
                *NextReportTime = EmergencyReportTimeSpace;
                *NextReportDistance = 0;
                Link1ReportDistanceSpace = 0;
            }
            else if(1 == ReportStrategy)//定距
            {
                *NextReportTime = 0;
                *NextReportDistance = EmergencyReportDistanceSpace+Link1LastReportDistance;
                Link1ReportDistanceSpace = EmergencyReportDistanceSpace;
            }
            else if(2 == ReportStrategy)//定时+定距
            {
                *NextReportTime = EmergencyReportTimeSpace;
                *NextReportDistance = EmergencyReportDistanceSpace+Link1LastReportDistance;
                Link1ReportDistanceSpace = EmergencyReportDistanceSpace;
            }
        }
        else if((1 == Io_ReadAlarmBit(ALARM_BIT_OVER_SPEED))&&(0 != OverSpeedReportTimeSpace)&&(0x00000001 == (0x00000001&ReportEventSwitch)))//超速报警
        {
            if((1 == CurOverSpeedFlag)&&(0 == LastOverSpeedFlag))
            {
                Link1ReportTimeCount = 0;
                Link2ReportTimeCount = 0;
            }
            *NextReportTime = OverSpeedReportTimeSpace;
            *NextReportDistance = 0;
            Link1ReportDistanceSpace =  0;
        }
        else if((1 == Io_ReadAlarmBit(ALARM_BIT_TIRED_DRIVE))&&(0 != OverTimeReportTimeSpace)&&(0x00000002 == (0x00000002&ReportEventSwitch)))//超时报警
        {
            if((1 == CurOverTimeFlag)&&(0 == LastOverTimeFlag))
            {
                Link1ReportTimeCount = 0;
                Link2ReportTimeCount = 0;
            }
            *NextReportTime = OverTimeReportTimeSpace;
            *NextReportDistance = 0;
            Link1ReportDistanceSpace = 0;
        }
        else 
        {
            if(0 == ReportScheme)//根据ACC状态
            {
                if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))//ACC ON
                {
                    if(0 == ReportStrategy)//定时
                    {
                        *NextReportTime = AccOnReportTimeSpace;
                        *NextReportDistance = 0;
                        Link1ReportDistanceSpace = 0;
                    }
                    else if(1 == ReportStrategy)//定距
                    {
                        *NextReportTime = 0;
                        *NextReportDistance = AccOnReportDistanceSpace+Link1LastReportDistance;
                        Link1ReportDistanceSpace = AccOnReportDistanceSpace;
                    }
                    else if(2 == ReportStrategy)//定时+定距
                    {
                        *NextReportTime = AccOnReportTimeSpace;
                        *NextReportDistance = AccOnReportDistanceSpace+Link1LastReportDistance;
                        Link1ReportDistanceSpace = AccOnReportDistanceSpace;
                    }
                    else
                    {
                        *NextReportTime = 0;
                        *NextReportDistance = 0;
                        Link1ReportDistanceSpace = 0;
                    }
                }
                else//ACC OFF
                {
                    if(0 == ReportStrategy)//定时
                    {
                        *NextReportTime = AccOffReportTimeSpace;
                        *NextReportDistance = 0;
                        Link1ReportDistanceSpace = 0;
                    }
                    else if(1 == ReportStrategy)//定距
                    {
                        *NextReportTime = 0;
                        *NextReportDistance = AccOffReportDistanceSpace+Link1LastReportDistance;
                        Link1ReportDistanceSpace = AccOffReportDistanceSpace;
                    }
                    else if(2 == ReportStrategy)//定时+定距
                    {
                        *NextReportTime = AccOffReportTimeSpace;
                        *NextReportDistance = AccOffReportDistanceSpace+Link1LastReportDistance;
                        Link1ReportDistanceSpace = AccOffReportDistanceSpace;
                    }
                    else
                    {
                        *NextReportTime = 0;
                        *NextReportDistance = 0;
                        Link1ReportDistanceSpace = 0;
                    }
                }
            }
            else if(1 == ReportScheme)//根据ACC状态和登录状态，先判断登录状态，再根据ACC
            {
                if(0 == VDROvertime_GetLoginFlag())//驾驶员未登录
                {
                    if(0 == ReportStrategy)//定时
                    {
                        *NextReportTime = UnloginReportTimeSpace;
                        *NextReportDistance = 0;
                        Link1ReportDistanceSpace = 0;
                    }
                    else if(1 == ReportStrategy)//定距
                    {
                        *NextReportTime = 0;
                        *NextReportDistance = UnloginReportDistanceSpace+Link1LastReportDistance;
                        Link1ReportDistanceSpace = UnloginReportDistanceSpace;
                    }
                    else if(2 == ReportStrategy)//定时+定距
                    {
                        *NextReportTime = UnloginReportTimeSpace;
                        *NextReportDistance = UnloginReportDistanceSpace+Link1LastReportDistance;
                        Link1ReportDistanceSpace = UnloginReportDistanceSpace;
                    }
                    else
                    {
                        *NextReportTime = 0;
                        *NextReportDistance = 0;
                        Link1ReportDistanceSpace = 0;
                    }
                }
                else//驾驶员已登录
                {
                    if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))//ACC ON
                    {
                        if(0 == ReportStrategy)//定时
                        {
                            *NextReportTime = AccOnReportTimeSpace;
                            *NextReportDistance = 0;
                            Link1ReportDistanceSpace = 0;
                        }
                        else if(1 == ReportStrategy)//定距
                        {
                            *NextReportTime = 0;
                            *NextReportDistance = AccOnReportDistanceSpace+Link1LastReportDistance;
                            Link1ReportDistanceSpace = AccOnReportDistanceSpace;
                        }
                        else if(2 == ReportStrategy)//定时+定距
                        {
                            *NextReportTime = AccOnReportTimeSpace;
                            *NextReportDistance = AccOnReportDistanceSpace+Link1LastReportDistance;
                            Link1ReportDistanceSpace = AccOnReportDistanceSpace;
                        }
                        else
                        {
                            *NextReportTime = 0;
                            *NextReportDistance = 0;
                            Link1ReportDistanceSpace = 0;
                        }
                    }
                    else//ACC OFF
                    {
                        if(0 == ReportStrategy)//定时
                        {
                            *NextReportTime = AccOffReportTimeSpace;
                            *NextReportDistance = 0;
                            Link1ReportDistanceSpace = 0;
                        }
                        else if(1 == ReportStrategy)//定距
                        {
                            *NextReportTime = 0;
                            *NextReportDistance = AccOffReportDistanceSpace+Link1LastReportDistance;
                            Link1ReportDistanceSpace = AccOffReportDistanceSpace;
                        }
                        else if(2 == ReportStrategy)//定时+定距
                        {
                            *NextReportTime = AccOffReportTimeSpace;
                            *NextReportDistance = AccOffReportDistanceSpace+Link1LastReportDistance;
                            Link1ReportDistanceSpace = AccOffReportDistanceSpace;
                        }
                        else
                        {
                            *NextReportTime = 0;
                            *NextReportDistance = 0;
                            Link1ReportDistanceSpace = 0;
                        }
                    }

                }
            }
            else
            {
                *NextReportTime = 0;
                *NextReportDistance = 0;
                Link1ReportDistanceSpace = 0;
            }
        }
    }
    Link2ReportDistanceSpace = Link1ReportDistanceSpace;
    Link1ReportTimeSpace = *NextReportTime;

    LastOverSpeedFlag = CurOverSpeedFlag;
    LastOverTimeFlag = CurOverTimeFlag;
}
/**************************************************************************
//函数名：Report_GetCurDistance
//功能：获取当前的距离（累计行驶里程）
//输入：无
//输出：无
//返回值：距离（累计行驶里程）
//备注：单位米
***************************************************************************/
static u32 Report_GetCurDistance(void)
{
    
    u32 CurMile;

    if(0 == VDRSpeed_GetCurSpeedType())
    {
        CurMile = VDRPulse_GetTotalMile()*10;
    }
    else
    {
        CurMile = MileMeter_GetTotalMile()*10;
    }

    return CurMile;
}
/**************************************************************************
//函数名：Report_GetPositionAdditionalInfo
//功能：获取位置附加信息
//输入：无
//输出：位置附加信息
//返回值：位置附加信息长度
//备注：
***************************************************************************/
static u16 Report_GetPositionAdditionalInfo(u8 *pBuffer)
{
    u8 *p = NULL;
    u16 length;

    p = pBuffer;
    length = 0;

    length = Report_GetPositionStdAdditionalInfo(p);//获取标准的位置附加信息
    p = p+length;

	  if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))//dxl,2016.5.7部标检测时自定义的附加ID不加入
		{
		
		}
		else
		{
			if(ReadPeripheral2TypeBit(8))// 广州日滨载重检测仪
			{
				length += Hand_WeightingSystem_GetPositionAdditionalInfo(p);
        	}
			else
			{
				length += Report_GetPositionEiAdditionalInfo(p);//获取伊爱自定义的位置附加信息
			}
		}

    return length;  
}
/**************************************************************************
//函数名：Report_GetPositionStdAdditionalInfo
//功能：获取标准的位置附加信息
//输入：无
//输出：标准的位置附加信息
//返回值：标准的位置附加信息长度
//备注：附加信息ID:0x01~0x04,0x11~0x13,0x25,0x2a,0x2b,0x30,0x31
***************************************************************************/
static u16 Report_GetPositionStdAdditionalInfo(u8 *pBuffer)
{
    u8 *p = NULL;
	  u8  NavigationFlag;
	  u8  Buffer[10];
	  u8  BufferLen;
	  u8  i;
	  u8  flag;
//	  u8  CurrentLoadExcursionAlarmFlag;
    u16 length;
    u16 temp2 = 0;
    u16 temp3;
    u16 TempLen;
	  u32 temp;
	
	  static u8 LastRouteAdditionalIDInfo[10];//路线附加ID信息
//	  static u8  LastLoadExcursionAlarmFlag = 0;

    p = pBuffer;
    length = 0;

    *p++ = 0x01;//里程，ID为0x01    
    *p++ = 4;
    temp = Public_GetCurTotalMile();
    Public_ConvertLongToBuffer(temp,p);
    p += 4;
    length += 6;
    
    TempLen = JointechOiLCost_GetSubjoinInfoCurOilVolume(p);//油量，ID为0x02，玖通油量，两者二选一
    p += TempLen;
    length += TempLen;
    
    /* dxl,2015.9,
    TempLen = OilWear_GetSubjoinInfoCurOilVal(p);//油量，ID为0x02，博实结油量
    p += TempLen;
    length += TempLen;
    */

    *p++ = 0x03;//行驶记录仪速度，ID为0x03，此处与标准要求不一样，做了处理
    *p++ = 0x02;//（按广州伊爱需求处理：当选择GPS速度时，若脉冲速度不为0使用脉冲速度，若为0使用GPS速度）
    /* dxl,2015.5.11屏蔽
    if(1 == SpeedMonitor_GetCurSpeedType())
    {
        if(0 == Pulse_GetSecondSpeed())
        {
            temp2 = 10*Gps_ReadSpeed();
        }
        else
        {
            temp2 = 10*Pulse_GetSecondSpeed();
        }
    }
    else
    {
        temp2 = 10*Pulse_GetSecondSpeed();
    }
    */
		if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))//dxl,2016.5.6兼容检测与实际应用
		{
		    temp2 = 10*VDRPulse_GetSecondSpeed();
		}
		else
		{
        if(0 == (SpeedFlag&0x01))//脉冲速度
        {
            temp2 = 10*VDRPulse_GetSecondSpeed();//dxl,2015.9,
        }
        else//GPS速度
        {
            temp2 = 10*Gps_ReadSpeed();
        }
	  }
    Public_ConvertShortToBuffer(temp2,p);
    p += 2;
    length += 4;

		TempLen = Report_GetManualAlarmSubjoinInfo(p);//需要人工确认报警时间的ID，ID为0x04,dxl,2016.5.6
    p += TempLen;
    length += TempLen;
		
    //超速附加信息，ID为0x11，当前只做了普通超速的，未做圆形区域、矩形区域、多边形区域、路段超速的,dxl,2016.5.6
		if(1 == Io_ReadOverSpeedConrtolBit(OVER_SPEED_BIT_ORDINARY))
		{
		    *p++ = 0x11;
			  *p++ = 1;
			  *p++ = 0;
			  length += 3;
		}
		if(1 == Io_ReadAlarmBit(ALARM_BIT_IN_OUT_AREA))//dxl,2016.5.23有进出区域报警时才增加对应的附加ID
		{
			  AreaInOutAlarmClearEnableFlag = 1;
        TempLen = Area_GetInOutAreaAlarmSubjoinInfo(p);//进出区域/路线报警附加信息，ID为0x12
        p += TempLen;
        length += TempLen;
		}
		
			  if(1 == BBGNTestFlag)
				{
				    flag = RouteInToOutFlag;
				}
				else
				{
			      flag = Io_ReadAlarmBit(ALARM_BIT_IN_OUT_ROAD);
				}
			  if(1 == flag)//dxl,2016.5.23有进出路线报警时才增加对应的附加ID
				{
					  RouteInToOutFlag = 0;
			      BufferLen = Route_GetInOutAlarmSubjoinInfo(Buffer);//dxl,2016.5.18只有进出路线那个时刻的位置汇报需要加附加ID 0x12
			      if(BufferLen > 0)
				    {
			          flag = 0;
			          for(i=0; i<8; i++)
			          {
				            if(Buffer[i] != LastRouteAdditionalIDInfo[i])
						        {
						            flag = 1;
					              memcpy(LastRouteAdditionalIDInfo,Buffer,8);
							          break;
						        }
				        }
				
						    if(1 == flag)
						    {
		                TempLen = BufferLen;
							      memcpy(p,Buffer,TempLen);
			              p += TempLen;
                    length += TempLen;
						    }
			      }
			  }

    TempLen = Route_GetDriveAlarmInfoSubjoinInfo(p);//路段行驶不足或过长附加信息，ID为0x13
    p += TempLen;
    length += TempLen;

    //TempLen = Route_GetDeviationAlarmInfo(p); //  //fanqinghai 2015.11.12 ??前面已有
    ///p += TempLen;                             //  主要是因为路线偏离报警根本没上报，以前上报的是超时驾驶
    
    *p++ = 0x25;//扩展车辆信号状态，ID为0x25    
    *p++ = 4;
    temp = Io_ReadExtCarStatus();
    Public_ConvertLongToBuffer(temp,p);
    p += 4;
    length += 6;

    *p++ = 0x2a;//IO状态，ID为0x2a  
    *p++ = 2;
    temp2 = Io_ReadIoStatus();
    if((1 == Io_ReadStatusBit(STATUS_BIT_ACC))&&(0x02 == (temp2&0x02)))//当出现ACC ON，SLEEP ON这样矛盾的情况时以ACC ON为准
    {
        temp2 = temp2&0xfc;
    }
    Public_ConvertShortToBuffer(temp2,p);
    p += 2;
    length += 4;

    temp2 = Ad_GetValue(ADC_EXTERN1);
    temp3 = Ad_GetValue(ADC_EXTERN2);
    *p++ = 0x2b;//模拟量，ID为0x2b，高16位为模拟量2，低16位为模拟量1
    *p++ = 4;
    Public_ConvertShortToBuffer(temp3,p);
    p += 2;
    Public_ConvertShortToBuffer(temp2,p);
    p += 2;
    length += 6;

    *p++ = 0x30;//无线网络信号强度，ID为0x30    
    *p++ = 1;
    *p++ = communicatio_GetSignalIntensity();
    length += 3;

		NavigationFlag = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);
		if((0 == NavigationFlag))
		//	||(1 == HYTestMachine3Flag))//dxl,2016.5.13检测要求不定位时为0
		{
		    *p++ = 0x31;//定位星数，ID为0x31    
        *p++ = 1;
        *p++ = 0;
        length += 3;
		}
		else
		{
        *p++ = 0x31;//定位星数，ID为0x31    
        *p++ = 1;
        *p++ = Gps_ReadStaNum();
        length += 3;
		}
    
    return length;
    
}
/**************************************************************************
//函数名：Report_GetManualAlarmSubjoinInfo
//功能：获取人工报警确认附加信息
//输入：无
//输出：人工报警确认附加信息
//返回值：人工报警确认附加信息长度
//备注：人工报警确认附加ID 0x04
//属人工报警确认的是bit0,bit3,bit20-22,bit27-28
***************************************************************************/
static u8 Report_GetManualAlarmSubjoinInfo(u8 *pBuffer)
{
	  static u32 LastAlarm = 0;
	  static u16 ManualAlarmEvID = 0;//人工报警确认事件ID，发生1次ID加1
	  static u8 LastAreaAlarmInfo[10] = {0};//区域报警信息
	  static u8 LastRouteAlarmInfo1[10] = {0};//路线报警信息1（进出路线）
		static u8 LastRouteAlarmInfo2[10] = {0};//路线报警信息2（行驶不足/过长）
		
    u32 ManualAlarmMask = 0x18000009;//bit0,bit3,bit27-28
	  u32 Alarm;
	  u32 temp;
	  u8 length;
	  u8 *p = NULL;
		u8  Buffer[10];
		u8  flag;
		u8  i;
	
	  Alarm = Io_ReadAlarm();
	  Alarm = Alarm & ManualAlarmMask;
	  temp = Alarm ^ LastAlarm;
	
		flag = 0;
	  if((LastAlarm != Alarm)&&(temp == (temp&Alarm)))//有人工报警产生
		{
		    flag = 1;
		}
		else//没有人工报警产生
		{
		    length = 0;
		}
	  LastAlarm = Alarm;  
		
		if(1 == Io_ReadAlarmBit(ALARM_BIT_IN_OUT_AREA))//有进出区域报警
		{
		    Area_GetInOutAreaAlarmSubjoinInfo(Buffer);
			  for(i=0; i<8; i++)
			  {
				    if(Buffer[i] != LastAreaAlarmInfo[i])
						{
						    flag = 1;
					      memcpy(LastAreaAlarmInfo,Buffer,8);
							  break;
						}
				}
			  //if(0 != strncmp((const char *)Buffer,(char *)LastAreaAlarmInfo,8)) dxl,2016.5.14这是一个非常错误的用法，因为可能不到8个字节时比较就结束了(如Buffer[2]==0)
				//{
				    //flag = 1;
					  //memcpy(LastAreaAlarmInfo,Buffer,8);
				//}
		}
		else if(1 == Io_ReadAlarmBit(ALARM_BIT_IN_OUT_ROAD))//有进出路线报警
		{
		    Route_GetInOutAlarmSubjoinInfo(Buffer);
			  for(i=0; i<8; i++)
			  {
				    if(Buffer[i] != LastRouteAlarmInfo1[i])
						{
						    flag = 1;
					      memcpy(LastRouteAlarmInfo1,Buffer,8);
							  break;
						}
				}
		}
		else if(1 == Io_ReadAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE))//有路段行驶不足/过程报警
		{
		    Route_GetDriveAlarmInfoSubjoinInfo(Buffer);
			  for(i=0; i<9; i++)
			  {
				    if(Buffer[i] != LastRouteAlarmInfo2[i])
						{
						    flag = 1;
					      memcpy(LastRouteAlarmInfo2,Buffer,9);
							  break;
						}
				}
		}
		if(1 == BBGNTestFlag)//dxl,2016.5.24
		{
				if(RouteInToOutFlag > 0)
				{
				    flag = 1;
				}
		}
		if(1 == flag)
		{
		    ManualAlarmEvID++;
			  p = pBuffer;
			  *p++ = 0x04;//附加ID
			  *p++ = 2;//附加ID长度
			  *p++ = (ManualAlarmEvID&0xff00) >> 8;
			  *p++ = ManualAlarmEvID&0xff;
			  length = 4;
		}
		else
		{
		    length = 0;
		}
		return length;
}
/**************************************************************************
//函数名：Report_GetPositionEiAdditionalInfo
//功能：获取伊爱自定义的位置附加信息
//输入：无
//输出：伊爱自定义的位置附加信息
//返回值：伊爱自定义的位置附加信息长度
//备注：
***************************************************************************/
static u16 Report_GetPositionEiAdditionalInfo(u8 *pBuffer)
{
    u8 length;
    u8 *p = NULL;
    u32 temp;
    u8 VerifyCode;
    u8 TempLen = 0; 
	
    length = 0;
    p = pBuffer;

    p += 2;//空开伊爱附加ID，标志符1字节，长度1字节

    *p++ = 0xE1;//自定义状态字2，ID为0xE1
    *p++ = 4;
    temp = Io_ReadSelfDefine2();
    Public_ConvertLongToBuffer(temp,p);
    p += 4;
    length += 6;
/* dxl,2015.9,
    TempLen = CarLoad_GetCurWeightSubjoinInfo(p);//载重重量，ID为0xE2
    p += TempLen;
    length += TempLen;
    */

    TempLen = JointechOiLCost_GetSubjoinInfoCurOilLevelPercent(p);//油位高度万分比，ID为0xE4
    p += TempLen;
    length += TempLen;
    
/* dxl,2015.9,
    TempLen = OilWear_GetSubjoinInfoCurOilResVal(p);//博实结油量电阻值，ID为0xE5
    p += TempLen;
    length += TempLen;
*/

    TempLen = Temperature_PosEx(p);//温度采集器，ID为0xE8
    p += TempLen;
    length += TempLen;

    //TempLen = Gyro_GetSubjoinInfo(p);//急加速、急减速、急转弯标志，ID为0xE9
    //p += TempLen;
    //length += TempLen;
		
		//liamtsen add
		if(!ReadPeripheral2TypeBit(6))
		{//没有接入A2OBD
			TempLen = Acceleration_PosEx(p);
			p += TempLen;
			length += TempLen;
		}
		else
		{
			TempLen = ThreeEmerInfo(p);
			p += TempLen;
			length += TempLen;
		}
		//////////////


    TempLen = DeepPlow_GetSubjoinInfoCurDeepVal(p);//耕深值,ID为0xEA
	  p += TempLen;
	  length += TempLen;
		
		TempLen = TurnSpeed_GetSubjoinInfoCurTurnSpeedVal(p);//转速,ID为0xEB
	  p += TempLen;
	  length += TempLen;
	
    VerifyCode = Public_GetSumVerify(pBuffer+2,length);//校验和
    *p++ = VerifyCode;
    length++;

    *pBuffer = 0xF0;//伊爱扩展附加信息标志
    *(pBuffer+1) = length;

    return length+2;
    
}
































