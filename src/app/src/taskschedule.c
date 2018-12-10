/********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS 
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR.h"
#include "Can_App.h"
#include "DeepPlow.h"
#include "turnspeed.h"
/* extern function -----------------------------------------------------------*/
void EvNopHandleTask(void);
FunctionalState TimerNopHandleTask(void);

static u32	TimeTaskResumeSta[MAX_TIMETASK/32+1] = {0,};
static u32	EvTaskResumeSta			 = 0;

#define			BIT_SET(Val,bit)			(Val |= 0x1<<bit)
#define			BIT_CLR(Val,bit)			(Val &= ~(0x1<<bit))
#define			BIT_CHECK(Val,bit)		(Val & 0x1<<bit)

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIME_TASK MyTimerTask[MAX_TIMETASK] = {  
    /*
        {ENABLE, 0, 1, Communication_TimeTask},                            // 0:无线通信，默认开启
        {ENABLE, 0, SYSTICK_0p1SECOND, RadioProtocolParse_TimeTask},       // 1:无线协议解析，默认开启   
        {ENABLE, 0, 1,Io_TimeTask},                                        // 2:IO 状态检测，默认开启 
        {ENABLE, 0, 1*SYSTICK_1SECOND, Gps_TimeTask},                      // 3:GPS数据处理，默认开启
        {ENABLE,0,5*SYSTICK_0p1SECOND,GpsMile_TimeTask},                   // 4:GPS里程计算，默认开启
        {ENABLE,0,SYSTICK_1SECOND,Report_TimeTask},                        // 5:位置汇报，默认开启
        {ENABLE,1,1,Camera_TimeTask},                                      // 6:摄像头拍照，默认开启         
        {DISABLE, 0, 1*SYSTICK_1SECOND, Media_UploadTimeTask},             // 7:传输多媒体数据到平台，默认关闭
        {ENABLE,0,SYSTICK_1SECOND,HeartBeat_TimeTask},                     // 8:心跳包，默认开启
        {ENABLE,0,SYSTICK_0p1SECOND,Usart3_TimeTask},                      // 9:USART3串口3定时接收任务,摄像头用的此串口，默认开启
        {DISABLE, 0, 1*SYSTICK_1SECOND, TimerNopHandleTask},               // 10: 无此功能    
        {DISABLE, 0, 1, DelayTrigTimeTask},                                // 11:延时触发，默认关闭
        {ENABLE, 0, 1*SYSTICK_1SECOND, Reset_TimeTask},                    // 12:50小时后系统重启，默认开启
        //{ENABLE,0,1,BMA250EApp_TimeTask},                                // 13:碰撞报警检测，默认开启
        {DISABLE,0,5,BMA250_TimeTask},                                      // 13:碰撞报警检测，默认开启
        //{DISABLE,0,1,BMA220App_TimeTask},                                
        {DISABLE,0,SYSTICK_1SECOND,Link1Login_TimeTask},                   // 14:连接1登录鉴权，默认关闭 
        {ENABLE,0,1,Usb_AppTimeTask},                                      // 15:USB，默认开启
        {ENABLE,0,1,Adc_CollectApp_TimeTask},                              // 16:AD采集，默认开启
        {ENABLE,0,SYSTICK_1SECOND,Sleep_TimeTask},                         // 17:休眠 ，默认开启 
        {ENABLE,1,1,KeyBoardAdc_TimeTask},                                 // 18:按键扫描，默认开启
        {ENABLE,0,1,TaskMenuMessage_TimeTask},                             // 19:LCD集成显示屏显示，默认开启
        {ENABLE,0, SYSTICK_1SECOND,AreaManage_TimeTask},                   // 20:区域 ，默认开启
        {DISABLE,0, SYSTICK_1SECOND,Media_TimeTask},                        // 21:删除多媒体文件，默认开启
        {ENABLE,0,3*SYSTICK_0p1SECOND,Route_TimeTask},                     // 22:线路，默认开启
        {DISABLE,0,1*SECOND,ControlOil_TimeTask},                          // 23:断油路，默认关闭
        {DISABLE, 0, ICAUC_TIME_TICK, ICAUC_TimeTask},                     // 24:IC卡认证TCP连接任务,默认使用是开启的,第1个版本多中心连接不完善，因此关闭,dxl,2014.7.15
        {DISABLE,0,SECOND,NaviLcd_TimerTask},                              // 25: 导航屏 ，默认开启
        {DISABLE, 0, SECOND, MultiCenterConnect_TimeTask},                 // 26:多中心连接,北斗功能测试时使用 ，默认关闭
        {DISABLE, 0, 1, GnssRealData_TimeTask},                             // 27：北斗实时数据透传，默认开启
        {DISABLE, 0, 5, TimerNopHandleTask},                                 // 28:无此功能
        {DISABLE, 0, 1, OpenAccount_TimeTask},                             // 29:快速开户 ，默认关闭
        {DISABLE, 0, SECOND, Communication_CloseGprsTimeTask},             // 30:关闭GPRS，用于使用前锁定一 ，默认开启
        {ENABLE,0,SECOND,Photo_TimeTask},                                  // 31:无张数限制的定时定距拍照任务,，默认开启
        {DISABLE,0,2,SleepDeep_TimeTask},                                  // 32:深度休眠，默认开启
        {DISABLE,0,2,Polygon_TimeTask},                                    // 33:多边形，默认开启
        {DISABLE,0,1*SECOND,Updata_SendResultNoteTimeTask},                 // 34:发送升级结果通知，默认开启
        {DISABLE,0,1*SECOND,Corner_TimeTask},                               // 35:拐点补传，默认开启
        {DISABLE,0,2,LZM_PublicTimerTask},                                 // 36:公共
        {DISABLE,0,SYSTICK_1SECOND,Link2Login_TimeTask},                   // 37:连接2登录鉴权，默认开启
        {ENABLE,0,1,Blind_TimeTask},                                       // 38:盲区补报，默认开启
        {ENABLE,0,1,StatusAlarm_TimeTask},                                 // 39:状态变化或产生报警立即上报或拍，默认开启
                {ENABLE,    0, 1, VDRUsart_TimeTask},                                // 40:VDR 串口接收
                {ENABLE,    0, 1, VDRData_TimeTask},                                 // 41:VDR 数据记录和读取
                {ENABLE,    0, 1, VDRPulse_TimeTask},                                // 42:VDR 脉冲速度和脉冲里程
                {ENABLE,    0, 1, VDRPosition_TimeTask},                             // 43:VDR 位置信息 
                {ENABLE,    0, 1, VDRSpeed_TimeTask},                                // 44:VDR 行驶速度记录
                {ENABLE,    0, 1, VDRDoubt_TimeTask},                                // 45:VDR 疑点数据
                {ENABLE,    0, SECOND, VDROvertime_TimeTask},                        // 46:VDR 超时驾驶记录
                {ENABLE, 0, SYSTICK_0p1SECOND, VDRCard_TimeTask},                  // 47:VDR 驾驶人身份记录
                {ENABLE,    0, 1, VDRPower_TimeTask},                                // 48:VDR 外部供电记录
                {ENABLE,    0, SECOND, VDRSpeedStatus_TimeTask},                     // 49:VDR 速度状态
                {ENABLE,    0, SECOND, VDROverspeed_TimeTask},                       // 50:VDR 超速报警
                {DISABLE, 0, SECOND, VDRCoefficient_TimeTask},                      // 51:VDR 脉冲系数校准  
                {DISABLE, 0, SECOND, VDRGprs_TimeTask},                            // 52:VDR 无线采集行驶记录仪数据  
                */
        {ENABLE, 0, 1, Communication_TimeTask},                            // 0:无线通信，默认开启
        {ENABLE, 0, SYSTICK_0p1SECOND, RadioProtocolParse_TimeTask},       // 1:无线协议解析，默认开启   
        {ENABLE, 0, 1,Io_TimeTask},                                        // 2:IO 状态检测，默认开启 
        {ENABLE, 0, 1*SYSTICK_1SECOND, Gps_TimeTask},                      // 3:GPS数据处理，默认开启
        {ENABLE,0,5*SYSTICK_0p1SECOND,GpsMile_TimeTask},                   // 4:GPS里程计算，默认开启
        {ENABLE,0,1,Report_TimeTask},                                      // 5:位置汇报，默认开启
        {ENABLE,1,1,Camera_TimeTask},                                      // 6:摄像头拍照，默认开启         
        {DISABLE, 0, 1*SYSTICK_1SECOND, Media_UploadTimeTask},             // 7:传输多媒体数据到平台，默认关闭
        {ENABLE,0,SYSTICK_1SECOND,HeartBeat_TimeTask},                     // 8:心跳包，默认开启
        {ENABLE,0,SYSTICK_0p1SECOND,Usart3_TimeTask},                      // 9:USART3串口3定时接收任务,摄像头用的此串口，默认开启
        {DISABLE, 0, 1*SYSTICK_1SECOND, TimerNopHandleTask},               // 10: 无此功能    
        {DISABLE, 0, 1, DelayTrigTimeTask},                                // 11:延时触发，默认关闭
        {DISABLE, 0, 1*SYSTICK_1SECOND, Reset_TimeTask},                    // 12:50小时后系统重启，默认开启
        {ENABLE,0,3,BMA250E_TimeTask},                                      // 13:碰撞报警检测，默认开启
        {DISABLE,0,SYSTICK_1SECOND,Link1Login_TimeTask},                   // 14:连接1登录鉴权，默认关闭 
        {ENABLE,0,1,Usb_AppTimeTask},                                      // 15:USB，默认开启
        {ENABLE,0,1,Adc_CollectApp_TimeTask},                              // 16:AD采集，默认开启
        {ENABLE,0,SYSTICK_1SECOND,Sleep_TimeTask},                         // 17:休眠 ，默认开启 
        {ENABLE,1,1,KeyBoardAdc_TimeTask},                                 // 18:按键扫描，默认开启
        {ENABLE,0,1,TaskMenuMessage_TimeTask},                             // 19:LCD集成显示屏显示，默认开启
        {ENABLE,0, SYSTICK_1SECOND,AreaManage_TimeTask},                   // 20:区域 ，默认开启
        {ENABLE,0, SYSTICK_1SECOND,Media_TimeTask},                        // 21:删除多媒体文件，默认开启
        {ENABLE,0,3*SYSTICK_0p1SECOND,Route_TimeTask},                     // 22:线路，默认开启
        {DISABLE,0,1*SECOND,ControlOil_TimeTask},                          // 23:断油路，默认关闭
        {ENABLE, 0, 1, Card_TimeTask},                                     // 24:IC卡认证TCP连接任务
        {ENABLE,0,SYSTICK_50ms,NaviLcd_TimerTask},                              // 25: 导航屏 ，默认开启
        {DISABLE, 0, SECOND, MultiCenterConnect_TimeTask},                 // 26:多中心连接,北斗功能测试时使用 ，默认关闭
        {ENABLE, 0, 1, GnssRealData_TimeTask},                             // 27：北斗实时数据透传，默认开启
        {ENABLE,0,1,TTS_TimeTask},                                          // 28:TTS播报
        {DISABLE, 0, 1, TimerNopHandleTask},                             // 29:快速开户 ，默认关闭
        {DISABLE, 0, SECOND, TimerNopHandleTask},                         // 30:关闭GPRS，用于使用前锁定一 ，默认开启
        {ENABLE,0,SECOND,Photo_TimeTask},                                  // 31:无张数限制的定时定距拍照任务,默认开启
        {DISABLE,0,2,SleepDeep_TimeTask},                                  // 32:深度休眠，默认开启
        {ENABLE,0,2,Polygon_TimeTask},                                    // 33:多边形，默认开启
        {ENABLE,0,1*SECOND,Updata_SendResultNoteTimeTask},                 // 34:发送升级结果通知，默认开启
        {ENABLE,0,1*SECOND,Corner_TimeTask},                               // 35:拐点补传，默认开启
        {ENABLE,0,2,LZM_PublicTimerTask},                                 // 36:公共
        {DISABLE,0,SYSTICK_1SECOND,Link2Login_TimeTask},                   // 37:连接2登录鉴权，默认关闭
        {ENABLE,0,1,Blind_TimeTask},                                       // 38:盲区补报，默认开启
        {ENABLE,0,1,StatusAlarm_TimeTask},                                 // 39:状态变化或产生报警立即上报或拍，默认开启

        {ENABLE,    0, 1, VDRUsart_TimeTask},                              // 40:VDR 串口接收
        {ENABLE,    0, 1, VDRData_TimeTask},                                 // 41:VDR 数据记录和读取
        {ENABLE,    0, 1, VDRPulse_TimeTask},                                // 42:VDR 脉冲速度和脉冲里程
        {ENABLE,    0, 1, VDRPosition_TimeTask},                             // 43:VDR 位置信息 
        {ENABLE,    0, 1, VDRSpeed_TimeTask},                                // 44:VDR 行驶速度记录
        {ENABLE,    0, 1, VDRDoubt_TimeTask},                                // 45:VDR 疑点数据
        {ENABLE,    0, SECOND, VDROvertime_TimeTask},                        // 46:VDR 超时驾驶记录
        {ENABLE, 0, SYSTICK_0p1SECOND, VDRCard_TimeTask},                    // 47:VDR 驾驶人身份记录
        {ENABLE,    0, 1, VDRPower_TimeTask},                                // 48:VDR 外部供电记录
        {ENABLE,    0, 1, VDRSpeedStatus_TimeTask},                          // 49:VDR 速度状态
        {ENABLE,    0, SECOND, VDROverspeed_TimeTask},                       // 50:VDR 超速报警
        {ENABLE, 0, SECOND, VDRCoefficient_TimeTask},                      // 51:VDR 脉冲系数校准  
        {DISABLE, 0, 2*SECOND, VDRGprs_TimeTask},                            // 52:VDR 无线采集行驶记录仪数据 
        {ENABLE, 0, SECOND, VDRLog_TimeTask},                               // 53:VDR 打印VDR日志到DB9串口
        {ENABLE, 0, 1, Can_TimeTask},                                       // 54:can数据采集
        {ENABLE, 0, SECOND, Lock1_TimeTask},                                // 55:使用前锁定一
        {ENABLE, 0, SECOND, Lock2_TimeTask},                               // 56:使用前锁定二
				{DISABLE, 0, 2*SECOND, VDRGprs_ResendTimeTask},                    // 57:VDR数据分包补传
				{ENABLE, 0, SECOND, PacketUpdate_TimeTask},                       // 58:分包升级
				{ENABLE,0,1*SECOND,JointechOiLCost_TimeTask },                   //59:玖通油位传感器
				{ENABLE,0,1,Temperature_TimeTask},                               //60:温度采集器 
				{ENABLE,0,SECOND,DeepPlow_TimeTask},                              //61:耕深
				{ENABLE,0,SECOND,TurnSpeed_TimeTask},                             //62:农机转速
				{ENABLE,0,SECOND,Vss_TimeTask},                             //63:VSS故障判断
				{ENABLE,0,SECOND,A2OBD_TimeTask},                             //64:A2OBD模块任务		
				{DISABLE,0,SECOND,Acceleration_TimeTask},       //65:急加速。急减速。急转弯，利用gps速度和gps航向

};
			
EVEN_TASK MyEvTask[MAX_EVENTASK] = {                                 
                   {DISABLE, EvNopHandleTask},          // 0:发送CAN数据SendCanData_EvTask
                   {DISABLE, EvNopHandleTask},          // 1:CAN总线数据处理 ,DecodeCan_EvTask
                   {DISABLE, Gps_EvTask},               // 2:GPS数据处理 Gps_EvTask
                   {DISABLE, EvNopHandleTask},          // 3:上报一条位置信息 
                   {DISABLE, GetAkey_EvTask},           // 4: 获取鉴权码任务 
                   {DISABLE, ChangeServer_EvTask},      // 5: 切换到政府监管平台 
                   {DISABLE, FtpUpdata_EvTask},         // 6: FTP无线升级 
                   {DISABLE, Dialling_EvTask},          // 7: 一键通话 
                   {DISABLE, SevDial_EvTask},           // 8: 服务器回拔电话功能 
                   {DISABLE, EvRing},                           // 9:来电响铃 
                   {DISABLE, EvRingOver},                       // 10:终止响铃 
                   {DISABLE, Lcd_ShowText_EvTask},              // 11:显示文本信息  
                   {DISABLE, Lcd_ShowText_EvTask},              // 11:显示文本信息  
                   };
                                               
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : InitTimerTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTimerTask(void)
{
  //u8 i;
  /*
  for(i = 1; i < MAX_TIMETASK; i++)
  {
    MyTimerTask[i].TaskTimerState = DISABLE;
  }
  */
  //增加初始化函数,并且增加执行函数时间  
}

/*******************************************************************************
* Function Name  : SetTimerTask
* Description    : Setup timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTimerTask(TIMERTASK task, u32 time)
{
  MyTimerTask[task].TaskTimerState = ENABLE;
  MyTimerTask[task].TaskExTimer = time;
  MyTimerTask[task].TaskTimerCount = time + Timer_Val();
}

/*******************************************************************************
* Function Name  : ClrTimerTask
* Description    : Cancel timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrTimerTask(TIMERTASK task)
{
  	MyTimerTask[task].TaskTimerState = DISABLE;
}

/*******************************************************************************
* Function Name  : ResumeTimerTask
* Description    : resume timer task .
* Input          : task MAX_TIMETASK: resume all task otherwise resume appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void ResumeTimerTask(TIMERTASK task)
{
	u8 i,j,k;
	if (task > MAX_TIMETASK)return;
	if (task == MAX_TIMETASK)//恢复所有任务
	{
		for(i = 0; i < MAX_TIMETASK; i++)
		{
			j = i/32;
			k = i%32;
			if(BIT_CHECK(TimeTaskResumeSta[j],k))
			{
				BIT_CLR(TimeTaskResumeSta[j],k);
				MyTimerTask[i].TaskTimerState = ENABLE;
			}
		}
	}
	else
	{
  	MyTimerTask[task].TaskTimerState = ENABLE;
	}
}

/*******************************************************************************
* Function Name  : SuspendTimerTask
* Description    : suspend timer task .
* Input          : task MAX_TIMETASK: suspend all task otherwise suspend appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void SuspendTimerTask(TIMERTASK task)
{
	u8 i,j,k;
	if (task > MAX_TIMETASK)return;
	if (task == MAX_TIMETASK)//挂起所有任务
	{
		for(i = 0; i < MAX_TIMETASK; i++)
		{
			if(MyTimerTask[i].TaskTimerState == ENABLE)
			{
				j = i/32;
				k = i%32;
				BIT_SET(TimeTaskResumeSta[j],k);
				MyTimerTask[i].TaskTimerState = DISABLE;
			}
		}
	}
	else
	{
  	MyTimerTask[task].TaskTimerState = DISABLE;
	}
}

/*******************************************************************************
* Function Name  : TimerTaskScheduler
* Description    : Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerTaskScheduler(void)  
{
  u8 i;
  u32 nowtimer;
  FunctionalState validflag;
  nowtimer = Timer_Val();
  for(i = 0; i < MAX_TIMETASK; i++)
  {
     if(MyTimerTask[i].TaskTimerState != ENABLE) 
     {
         continue; 
     }

    if(nowtimer >= MyTimerTask[i].TaskTimerCount)
    {
        MyTimerTask[i].TaskTimerCount = nowtimer + MyTimerTask[i].TaskExTimer;
        validflag = MyTimerTask[i].operate();
        MyTimerTask[i].TaskTimerState = validflag;   
    }
  }
}


/*******************************************************************************
* Function Name  : InitEvTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitEvTask(void)
{
  u8 i;
  
  for(i = 0; i < MAX_EVENTASK; i++)
  {
    MyEvTask[i].evTaskOnState = DISABLE;
  }
  //增加初始化函数
}

/*******************************************************************************
* Function Name  : SetEvTask
* Description    : Setup even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetEvTask(u8 ev)
{
  MyEvTask[ev].evTaskOnState = ENABLE;
}
/*******************************************************************************
* Function Name  : ClrEvTask
* Description    : Cancel ev task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrEvTask(EVENTTASK ev)
{
  	MyEvTask[ev].evTaskOnState = DISABLE;
}
/*******************************************************************************
* Function Name  : ResumeEvTask
* Description    : resume ev task .
* Input          : ev MAX_EVENTASK: resume all task otherwise resume appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void ResumeEvTask(EVENTTASK ev)
{
	u8 i;
	if (ev > MAX_EVENTASK)return;
	if (ev == MAX_EVENTASK)//挂起所有任务
	{
		for(i = 0; i < MAX_EVENTASK; i++)
		{
			if(BIT_CHECK(EvTaskResumeSta,i))
			{
				BIT_CLR(EvTaskResumeSta,i);
				MyEvTask[i].evTaskOnState = ENABLE;
			}
		}
	}
	else
	{
  	MyEvTask[ev].evTaskOnState = ENABLE;
	}
}

/*******************************************************************************
* Function Name  : SuspendEvTask
* Description    : suspend ev task .
* Input          : ev MAX_EVENTASK: suspend all task otherwise suspend appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void SuspendEvTask(EVENTTASK ev)
{
	u8 i;
	if (ev > MAX_EVENTASK)return;
	if (ev == MAX_EVENTASK)//挂起所有任务
	{
		for(i = 0; i < MAX_EVENTASK; i++)
		{
			if(MyEvTask[i].evTaskOnState == ENABLE)
			{
				BIT_SET(EvTaskResumeSta,i);
				MyEvTask[i].evTaskOnState = DISABLE;
			}
		}
	}
	else
	{
  	MyEvTask[ev].evTaskOnState = DISABLE;
	}
}

/*******************************************************************************
* Function Name  : EvTaskScheduler
* Description    : Schedule even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvTaskScheduler(void) 
{
  u8 i;

  for(i = 0; i < MAX_EVENTASK; i++)
  {
    if(MyEvTask[i].evTaskOnState != ENABLE)
      continue;
    MyEvTask[i].evTaskOnState = DISABLE;
    MyEvTask[i].operate();
   
  }
} 
/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : 空事件处理 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState TimerNopHandleTask()
{
  
  return DISABLE;
}

/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : 空事件处理 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvNopHandleTask()
{

}
/*******************************************************************************
* Function Name  : SendCanData_EvTask
* Description    : 发送CAN数据
* Input          : None
* Output         : None
* Return         : None
* Note           :需要发送的CAN数据放在RadioProtocolMiddleBuffer,长度为RadioProtocolMiddleLen
*******************************************************************************/
void SendCanData_EvTask(void)
{
        //调用发送CAN数据的函数
        
}
