/************************************************************************
//程序名称：Lock1.c
//功能：实现锁定一功能
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2016.4
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：锁定一功能：
//1.当锁定一使能标志（参数ID为0xF28B）设置为使能（1）时才开启该功能；
//2.当主服务器域名不为jt1.gghypt.net或者备份服务器域名jt2.gghypt.net时需锁定，处于锁定一状态时通信模块是关闭的，无法进行连网操作，显示屏显示“使用前锁定一”
//3.解锁的方法是：
//（1）将主服务器域名设置为jt1.gghypt.net，将备份服务器域名设置为jt2.gghypt.net，两个缺一不可；
//（2）将锁定一使能标志设置为不使能，即禁止该功能；
//（3）当处于部标功能检测（参数ID 0xF24C的bit6为1）时，为了操作上的方便，定时2分钟解锁，解锁后锁定一使能标志被设置为0即不使能
//4.注意事项：
//（1）锁定一解锁后主备服务器的IP地址分别已为jt1.gghypt.net和jt2.gghypt.net，这时检测平台是看不到终端在线的，因为终端去连接
//全国公共货运平台jt1.gghypt.net和jt2.gghypt.net了，而检测平台要求解锁后要能重新连回去，因此锁定一解锁后需要通过显示屏菜单或串口将
//备服务器地址设回检测平台的IP。解锁后在设回检测平台IP之前请确认使用前锁定一的使能标志是否为禁止，需要为禁止。
//否则你一旦设回了检测平台IP又会被锁定
//（2）检测时，锁定一使能标志和锁定二使能标志只建议开启一个。以免带来不必要的干扰。检测锁定一功能时开启锁定一使能，禁止锁定二使能；
//检测锁定二功能时开启锁定二使能，禁止锁定一使能标志
//（3）实际应用时锁定一使能和锁定二使能可以同时开启
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include "Lock1.h"
#include "modem_app_first.h"
#include "modem_app_second.h"
#include "taskschedule.h"
#include "Gdi.h"
#include "EepromPram.h"
#include "Lcd.h"
#include "VDR_Pub.h"
#include "VDR_Data.h"

/********************本地变量*************************/
static u8 Lock1Flag = 0;//锁定标志，1为锁定状态；0为未锁定；其它值为错误，这时将关闭该功能
static u8 Lock1EnableFlag = 0;//使能标志，1为使能；0为不使能；其它值为错误，这时将关闭该功能（等同于不使能）
static u8 InitFlag = 0;//初始化标志

/********************全局变量*************************/


/********************外部变量*************************/
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
extern TIME_TASK MyTimerTask[MAX_TIMETASK];
extern Queue  VdrTxQ;//VDR发送队列
/********************本地函数声明*********************/
static u8 Lock1_ParameterIsRight(void);
static void Lock1_Init(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：Lock1_TimeTask
//功能：实现锁定一功能
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：锁定一定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState Lock1_TimeTask(void)
{
	
   static u8 state = 0;
   
   if(0 == InitFlag)
   {
       state = 0;
       InitFlag = 1;
       Lock1_Init();
   }
   else
   {
       if(1 == Lock1EnableFlag)//开启了锁定一功能
       {
           if(0 == state)//判断是否需要锁定
           {
               if(1 == Lock1_ParameterIsRight())
               {
                   state = 3;
               }
               else
               {
                   state++;
               }
           }
           else if(1 == state)//关闭通信模块任务和显示任务
           {
						   Lock1Flag = 1;
							 state++;
               ClrTimerTask(TIME_COMMUNICATION);
               ClrTimerTask(TIME_MENU);
						   LcdClearScreen();
               LcdShowStr(10,20,"锁定一关闭通信和显示任务",0);
           }
           else if(2 == state)//锁定，并等待主备服务器IP地址设置为合法的全国货运平台地址
           {
						   if(1 == Lock1_ParameterIsRight())
               {
                   state++;
               }
							 else
							 {
							     LcdClearScreen();
                   LcdShowStr(10,20,"使用前锁定一",0);
							 }
           }
           else//解锁
           {
               Lock1EnableFlag = 0;	   
           }
       }
       else
       { 
           state = 0;
				   if(1 == Lock1Flag)
           {
						   Lock1Flag = 0;
						   LcdClearScreen();
               LcdShowStr(10,20,"锁定一解锁",0);
						   if(0 == Lock2_GetLock2Flag())
							 {
								   Lcd_SetMainRedrawFlag();
                   Communication_Init();
                   SetTimerTask(TIME_COMMUNICATION,1);
                   SetTimerTask(TIME_MENU,1);
						       LcdClearScreen();
                   LcdShowStr(10,20,"锁定一开启通信和显示任务",0);
							 }
           }
       }
   }
    return ENABLE;      
}
/**************************************************************************
//函数名：Lock1_UpdataParameter
//功能：更新相关参数
//输入：无
//输出：无
//返回值：无
//备注：当设置参数（主服务器、备份服务器、锁定一使能）时需要调用该函数
***************************************************************************/
void Lock1_UpdataParameter(void)
{
    InitFlag = 0;
}
/**************************************************************************
//函数名：Lock1_GetLock1Flag
//功能：获取锁定一标志
//输入：无
//输出：无
//返回值：无
//备注：1为锁定；0为未锁定
***************************************************************************/
u8 Lock1_GetLock1Flag(void)
{
    return Lock1Flag;
}
/**************************************************************************
//函数名：Lock1_Init
//功能：更新相关参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void Lock1_Init(void)
{
    u8 Buffer[5];
    u8 BufferLen;
    
    BufferLen = EepromPram_ReadPram(E2_LOCK1_ENABLE_FLAG_ID,Buffer);
    if(E2_LOCK1_ENABLE_FLAG_ID_LEN == BufferLen)
    {
        Lock1EnableFlag = Buffer[0];
        if(Lock1EnableFlag > 1)
        {
            Lock1EnableFlag = 0;
        }
    }
    else
    {
        Lock1EnableFlag = 0;
    }
}
/**************************************************************************
//函数名：Lock1_ParameterIsRight
//功能：检查参数是否正确
//输入：无
//输出：无
//返回值：参数正确返回1，错误返回0
//备注：若主服务器域名或备份服务器域名设置不正确，则锁定开启
***************************************************************************/
static u8 Lock1_ParameterIsRight(void)
{
    u8 Buffer[25];
    u8 BufferLen;
    
    BufferLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,Buffer);
    if(BufferLen < 8)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}































