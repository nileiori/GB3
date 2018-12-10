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
//*****************事件任务**************************
typedef enum 
{
        EV_SEND_CAN_DATA = 0,           // 0:发送CAN数据
      EV_CANREC,                      // 1:CAN总线数据处理
        EV_GPS_PARSE,                   // 2:GPS数据处理
        EV_NULL,                        // 3:空
        EV_GET_LOGIN_PASSWORD,          // 4: 获取鉴权码任务
        EV_LINK_GOV_SERVER,             // 5: 切换到政府监管平台
        EV_FTP_UPDATA,                  // 6: FTP无线升级
        EV_CALLING,                     // 7: 一键通话
        EV_SEV_CALL,                    // 8: 服务器回拔电话功能
        EV_RING,                        // 9:响铃事件号   
        EV_RING_OVER,                   // 10:停止响铃
        EV_SHOW_TEXT,                   // 11:文本显示
      EV_KEY_PARSE,                   // 12:有按键事件发生
      MAX_EVENTASK

}EVENTTASK;

//******************时间任务***********************
typedef enum 
{
        TIME_COMMUNICATION = 0,         //0:无线通信
        TIME_RADIO_PARSE,               //1:无线协议解析
        TIME_IO_MONITOR,                //2:IO 状态检测 
        TIME_GPSREC,                    //3:GPS数据处理 
        TIME_GPS_MILE,                  //4:GPS里程计算
        TIME_POSITION,                  //5:位置汇报 
        TIME_CAMERA,                    //6:摄像头拍照 
        TIME_SEND_MEDIA,                //7:传输多媒体数据到平台
        TIME_HEART_BEAT,                //8:心跳包
        TIME_USART3,                    //9:USART3串口3(摄像头拍照使用)定时接收任务
        TIME_RADIO_RESEND,              //10:无线协议重发（国标）
        TIME_DELAY_TRIG,                //11:延时触发
        TIME_SYSRESET,                  //12:系统重启
        TIME_BMA220,                    //13:碰撞报警
        TIME_LINK1_LOGIN,               //14:连接1注册鉴权
        TIME_USB,                       //15:USB
        TIME_AD,                        //16:AD采样
        TIME_SLEEP,                     //17:休眠
        TIME_KEY_BOARD,                 //18:按键扫描
        TIME_MENU,                      //19:LCD集成显示屏显示
        TIME_AREA,                      //20:区域
        TIME_MEDIA,                     //21:删除多媒体文件
        TIME_ROUTE,                     //22:线路
        TIME_OIL_CTRL,                  //23:断油路
        TIME_ICAUC,                     //24:IC卡认证TCP连接任务 
      TIME_NVLCD,                     //25: 导航屏 
      TIME_MULTI_CONNECT,             //26: 多中心连接,北斗测试时使用
      TIME_GNSS_REAL,                 //27：北斗实时解析数据
      TIME_TTSDELAY,                  //28:TTS播报
      TIME_ACCOUNT,                   //29:快速开户
      TIME_CLOSE_GPRS,                //30:GPRS下线
        TIME_PHOTO,                     //31:无张数限制的定时定距拍照任务,
        TIME_SLEEPDEEP,                 //32:深度休眠
      TIME_POLYGON,                   //33:多边形
      TIME_UPDATA_RESULT_NOTE,        //34:发送升级结果通知
      TIME_CORNER,                    //35:拐点补传
      TIME_PUBLIC,                    //36:公共
      TIME_LINK2_LOGIN,               //37:连接2注册鉴权
      TIME_BLIND,                       //38:盲区补报
      TIME_STATUS_ALARM,                //39:状态变化或产生报警立即上报或拍照
            TIME_VDR_USART,             //40:VDR 串口接收
        TIME_VDR_DATA,                  //41:VDR 数据记录和读取
        TIME_VDR_PULSE,                 //42:VDR 脉冲速度和脉冲里程
        TIME_VDR_POSITION,              //43:VDR 位置信息
        TIME_VDR_SPEED,                 //44:VDR 速度记录
        TIME_VDR_DOUBT,                 //45:VDR 疑点数据
        TIME_VDR_OVERTIME,              //46:VDR 超时驾驶
        TIME_VDR_CARD,                  //47:VDR 驾驶人身份
        TIME_VDR_POWER,                 //48:VDR 外部供电
        TIME_VDR_SPEED_STATUS,          //49:VDR 速度状态
        TIME_VDR_OVER_SPEED,            //50:VDR 超速
        TIME_VDR_COEFFICIENT,           //51:VDR 脉冲系数校准
        TIME_VDR_GPRS,                  //52:VDR 无线采集行驶记录仪数据
        TIME_VDR_LOG,                   //53:VDR 打印VDR日志到DB9串口
        TIME_CAN_SEND,                  //54:can数据采集
        TIME_LOCK_ONE,                  //55.使用前锁定一     
        TIME_LOCK_TWO,                  //56:使用前锁定二
			  TIME_VDR_PACKET_RESEND,         //57:VDR数据分包补传
				TIME_PACKET_UPDATE,             // 58:分包升级
				TIME_JOINTECHOILCOST,           //59:玖通油位传感器
				TIME_TEMPERATURE,               //60:温度采集器 
				TIME_DEEPPLOW,                  //61:耕深
				TIME_TURNSPEED,                 //62:农机转速
				TIME_VSS,                       //63:VSS故障判断
				TIME_A2OBD,											//64:A2OBD模块任务	
				TIME_ACCELERATION,              //65:急加速。急减速。急转弯，利用gps速度和gps航向
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
* Description    : 发送CAN数据
* Input          : None
* Output         : None
* Return         : None
* Note           :需要发送的CAN数据放在RadioProtocolMiddleBuffer,长度为RadioProtocolMiddleLen
*******************************************************************************/
void SendCanData_EvTask(void);
#endif
