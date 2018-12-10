/************************************************************************
//程序名称：Lock2.c
//功能：实现锁定二功能
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2016.4
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：锁定二功能：
//1.当锁定二使能标志（参数ID为0xF28C）设置为使能（1）时且未开过户（开户标志为0）才开启该功能；
//2.处于锁定二状态时，主机集成的LCD屏显示“使用前锁定二”，按键无反应，通信模块连接关闭；
//3.只有当主服务器域名设置为jt1.gghypt.net，备份服务器域名设置为jt2.gghypt.net前提下，且收到了导航屏的开户指令
//或定时时间（2分钟）到了才会发送快速开户指令到平台，实现解锁功能（开户标志置1）
//4.当锁定二使能标志设置为0（禁止）时，会顺带把开户标志清0，以便下一次使能锁定二时会重新发送快速开户指令

*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include "Lock2.h"
#include "modem_app_first.h"
#include "modem_app_second.h"
#include "taskschedule.h"
#include "Gdi.h"
#include "EepromPram.h"
#include "Lcd.h"
/********************本地变量*************************/
static u8 OpenAccountFlag = 0;//快速开户标志，1为已开户；0为未开户；其它值等同于未开户
static u8 Lock2EnableFlag = 0;//使能标志，1为使能；0为不使能；其它值为错误，这时将关闭该功能（等同于不使能）
static u8 Lock2Flag = 0;//锁定二标志，1为锁定（开启了快速开户流程），0为未锁定
static u8 InitFlag = 0;//初始化标志
static u8 OpenAccountCmdRxFlag = 0;//快速开户指令接收标志，1为已接收；0为未接收。指导航屏发给主机的快速开户指令
static u8 OpenAccountOnline = 0;//快速开户在线标志，1为连接在线；0为连接不在线
static u8 OpenAccountRequestFlag = 0;//快速开户请求标志，1:有请求；0:无请求
/********************全局变量*************************/


/********************外部变量*************************/
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
extern TIME_TASK MyTimerTask[MAX_TIMETASK];

/********************本地函数声明*********************/
//static u8 Lock2_ParameterIsRight(void);
static void Lock2_Init(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：Lock2_TimeTask
//功能：实现锁定二功能
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：锁定二定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState Lock2_TimeTask(void)
{
	
   static u8 state = 0;
   static u32 count = 0;
	
  // u8 Buffer[5];	
	 
	
   if(0 == InitFlag)
   {
       state = 0;
       count = 0;
		   Lock2Flag = 0;
       InitFlag = 1;
       Lock2_Init();
   }
   else
   {
       if((1 == Lock2EnableFlag)&&(1 != OpenAccountFlag))//开启了锁定二功能
       {
				   if(0 == state)//检查主备服务器IP
           {
						 /*
					     if(0 == Lock2_ParameterIsRight())//主备服务器IP参数不正确
					     {
					         count++;
						       if(1 == count)//先关闭连接
							     {
							         Net_First_Close();
										   LcdClearScreen();
                       LcdShowStr(10,20,"开启使用前锁定二",0);
							     }
									 else if(count >= 10)//提示输入正确的域名
									 {
									     LcdClearScreen();
                       LcdShowStr(10,20,"请输入正确的域名和端口号",0);
									 }

					    }
							else
							{
							*/
								  LcdClearScreen();
                  LcdShowStr(10,20,"开启使用前锁定二",0);
								  OpenAccountCmdRxFlag = 0;
								  ClrTimerTask(TIME_COMMUNICATION);
                  ClrTimerTask(TIME_MENU);
									Lock2Flag = 1;
							    state++;
							//}
					
					 } 
           else if(1 == state)//检查是否收到快速开户指令
           {
							 if(1 == OpenAccountCmdRxFlag)//收到了导航屏发的快速开户指令
							 {
								   LcdClearScreen();
                   LcdShowStr(10,20,"收到导航屏开户请求",0);
								   OpenAccountCmdRxFlag = 0;
								   OpenAccountRequestFlag = 1;
								   OpenAccountOnline = 0;
								   count = 0;
							     Communication_Init();
									 SetTimerTask(TIME_COMMUNICATION,1);
								   state++;
							 }
							 else
							 {
								   LcdClearScreen();
                   LcdShowStr(10,20,"使用前锁定二",0);
							     if(1 == BBGNTestFlag)
									 {
									     count++;
										   if(count >= 1200)
											 {
												    LcdClearScreen();
                            LcdShowStr(10,20,"收到导航屏开户请求",0);
											      OpenAccountCmdRxFlag = 0;
								            OpenAccountRequestFlag = 1;
								            OpenAccountOnline = 0;
								            count = 0;
							              Communication_Init();
									          SetTimerTask(TIME_COMMUNICATION,1);
								            state++;
											 }
									 }
							 }
           }
           else if(2 == state)//发送快速开户指令
           {
							 count++;
							 if(count >= 1800)//30分钟后归位
							 {
							     count = 0;
								   InitFlag = 0;
							 }
							 else if(1 == OpenAccountOnline)
							 {
							     if(0 == count%30)
									 {
								       RadioProtocol_OpenAccount(CHANNEL_DATA_1);
										   LcdClearScreen();
                       LcdShowStr(10,20,"快速开户指令已发送",0);
									 }
							 }
           }
					 else
					 {
					     Lock2EnableFlag = 0;
					 }	 
       }
       else
       { 
           count = 0;
           state = 0;
				   if(1 == Lock2Flag)
           {
						   Lock2Flag = 0;
						   if(0 == Lock1_GetLock1Flag())
							 {
						      Lcd_SetMainRedrawFlag();
                  Communication_Init();
                  SetTimerTask(TIME_COMMUNICATION,1);
                  SetTimerTask(TIME_MENU,1);
								  LcdClearScreen();
                  LcdShowStr(10,20,"锁定二开启通信和显示任务",0);
							 }
           }
       }
   }
    
		
    return ENABLE;      
}
/**************************************************************************
//函数名：Lock2_UpdataParameter
//功能：更新相关参数
//输入：无
//输出：无
//返回值：无
//备注：当设置参数（主服务器、备份服务器、锁定二使能）时需要调用该函数
***************************************************************************/
void Lock2_UpdataParameter(void)
{
    InitFlag = 0;
}
/**************************************************************************
//函数名：Lock2_OpenAccountAck
//功能：处理快速开户应答
//输入：无
//输出：无
//返回值：无
//备注：当收到平台快速开户应答时调用该函数
***************************************************************************/
void Lock2_OpenAccountAck(u8 *pBuffer, u16 BufferLen)
{
	  u16 Addr;
	  u16 AckCmd;
	  u8 *p = NULL;
	
	  
	  p = pBuffer;
	  p += 2;
	  AckCmd = 0;
	  AckCmd |= *p++ << 8;
	  AckCmd |= *p++;
	  if(((0x0110 == AckCmd)&&(0 == *p))||((0x0110 == AckCmd)&&(5 == *p)))//第一次快速注册成功应答0，以后注册应答5
		{
	      OpenAccountRequestFlag = 0;
        OpenAccountFlag = 1;
	      Addr = FRAM_QUICK_ACCOUNT_ADDR;
        FRAM_BufferWrite(Addr, &OpenAccountFlag, FRAM_QUICK_ACCOUNT_LEN);
			  //擦除鉴权码，重新连接以便走注册流程
			  ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
		}
}
/**************************************************************************
//函数名：Lock2_OpenAccountOnline
//功能：置位OpenAccountOnline
//输入：无
//输出：无
//返回值：1：当前为快速开户连接；0：当前为其它连接（正常应用时属这种情况）
//备注：当通信模块连网成功后需调用此函数
***************************************************************************/
void Lock2_OpenAccountOnline(void)
{
    OpenAccountOnline = 1; 
}
/**************************************************************************
//函数名：Lock2_GetOpenAccountFlag
//功能：获取快速开户标志
//输入：无
//输出：无
//返回值：1:为已开户；0:未开户
***************************************************************************/
u8 Lock2_GetOpenAccountFlag(void)
{
    return OpenAccountFlag; 
}
/**************************************************************************
//函数名：Lock2_GetLock2Flag
//功能：获取锁定二标志
//输入：无
//输出：无
//返回值：1:为锁定；0:为未锁定
***************************************************************************/
u8 Lock2_GetLock2Flag(void)
{
    return Lock2Flag; 
}
/**************************************************************************
//函数名：Lock2_GetEnableFlag
//功能：获取锁定二使能标志
//输入：无
//输出：无
//返回值：1:为使能;0:为不使能
***************************************************************************/
u8 Lock2_GetEnableFlag(void)
{
    return Lock2EnableFlag; 
}
/**************************************************************************
//函数名：Lock2_GetOpenAccountRequestFlag
//功能：获取OpenAccountRequestFlag
//输入：无
//输出：无
//返回值：1：有快速开户需求；0：无快速开户需求
//备注：当通信模块连网成功后需调用此函数
***************************************************************************/
u8 Lock2_GetOpenAccountRequestFlag(void)
{
    return OpenAccountRequestFlag; 
}
/**************************************************************************
//函数名：Lock2_TrigOpenAccount
//功能： 触发快速开户
//输入：无
//输出：无
//返回值：无
//备注：当收到平台快速开户应答时调用该函数
***************************************************************************/
void Lock2_TrigOpenAccount(void)
{
	  if(0 == OpenAccountFlag)
		{
        OpenAccountCmdRxFlag = 1; 
		}
		else
		{
			  LcdClearScreen();
        LcdShowStr(10,20,"已开户",0);
		}
}

/**************************************************************************
//函数名：Lock2_Init
//功能：更新相关参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void Lock2_Init(void)
{
    u8 Buffer[5];
    u8 BufferLen;
    
    BufferLen = EepromPram_ReadPram(E2_LOCK2_ENABLE_FLAG_ID,Buffer);
    if(E2_LOCK2_ENABLE_FLAG_ID_LEN == BufferLen)
    {
        Lock2EnableFlag = Buffer[0];
        if(Lock2EnableFlag > 1)
        {
            Lock2EnableFlag = 0;
        }
    }
    else
    {
        Lock2EnableFlag = 0;
			  
    }
		
    BufferLen = FRAM_BufferRead(Buffer, FRAM_QUICK_ACCOUNT_LEN, FRAM_QUICK_ACCOUNT_ADDR);
		if(FRAM_QUICK_ACCOUNT_LEN == BufferLen)
    {
        OpenAccountFlag = Buffer[0];
        if(OpenAccountFlag > 1)
        {
            OpenAccountFlag = 0;
        }
    }
    else
    {
        OpenAccountFlag = 0;
    }
		
		///if((0 == Lock2EnableFlag)&&(1 == OpenAccountFlag))
		if(0 == Lock2EnableFlag)
		{
		    OpenAccountFlag = 0;
        FRAM_BufferWrite(FRAM_QUICK_ACCOUNT_ADDR, &OpenAccountFlag, FRAM_QUICK_ACCOUNT_LEN);
		}
}
/**************************************************************************
//函数名：Lock2_ParameterIsRight
//功能：检查参数是否正确
//输入：无
//输出：无
//返回值：参数正确返回1，错误返回0
//备注：若主服务器域名或备份服务器域名设置不正确，则锁定开启
***************************************************************************/
/*
static u8 Lock2_ParameterIsRight(void)
{
    u8 Buffer[25];
    u8 BufferLen;
    
    BufferLen = EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,Buffer);
    if(0 != BufferLen)
    {
        if(0 == strncmp((const char *)Buffer,"jt1.gghypt.net",14))
        {
            BufferLen = EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID,Buffer);
            if(0 != BufferLen)
            {
                if(0 == strncmp((const char *)Buffer,"jt2.gghypt.net",14))
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
*/































