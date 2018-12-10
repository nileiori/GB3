/*******************************************************************************
 * File Name:           OpenAccount.c 
 * Function Describe:   
 * Relate Module:       快速开户功能
 * Writer:              Joneming
 * Date:                2013-06-17 
 * ReWriter:            Joneming
 * Date:                //进行开户操作之前先把开户服务器设到相应的IP地址上,开户服务器的端口号设到相应的端口号地址上,
                        //开户流程:终端从主IP上下线,把开户服务器地址与主IP地址互换,终端再连接开户服务器,连接成功后发开户指令
                        //接收到解锁应答后,终端再恢复原来的主IP地址,之后再进行正常的上线
 *******************************************************************************/
#include "include.h"
////////////////////////////////
typedef enum
{
    OA_STATUS_SAVE_IP,   //保存IP地址
    OA_STATUS_UNLOCK,    //解锁,
    OA_STATUS_LOCK,      //锁定,
    OA_STATUS_VALID_FLAG,//有效标志
    OA_STATUS_MAX         //
}E_OA_STATUS;

enum 
{
    ACCOUNT_TIMER_TASK,//
    ACCOUNT_TIMER_SEND,//
    ACCOUNT_TIMERS_MAX
}T_STACCOUNTTIME;

static LZM_TIMER s_stAccountTimer[ACCOUNT_TIMERS_MAX];

static unsigned char s_ucfisrtRunFlag = 0;
static unsigned char s_ucOpenAccountStatus = 0;

#define  EYENET_FIRST_SOC                         (0)//SOC号

static u8 UnlockResult =0;
/////////////////////////
void OpenAccount_RecoverOriginalIPAddress(void);
/*************************************************************
** 函数名称: OpenAccount_SaveStatus
** 功能描述: 保存快速开户锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_SaveStatus(void)
{
    PUBLIC_SETBIT(s_ucOpenAccountStatus, OA_STATUS_VALID_FLAG);    
    Public_WriteDataToFRAM(FRAM_QUICK_OPEN_ACCOUNT_ADDR, &s_ucOpenAccountStatus,FRAM_QUICK_OPEN_ACCOUNT_LEN);
}
/*************************************************************
** 函数名称: OpenAccount_ReadLockStatus
** 功能描述: 读取快速开户锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_ReadLockStatus(void)
{
    unsigned char max;
    unsigned char flag = 0;    
    if(Public_ReadDataFromFRAM(FRAM_QUICK_OPEN_ACCOUNT_ADDR, &flag,FRAM_QUICK_OPEN_ACCOUNT_LEN))
    {
        max = 0;
        PUBLIC_SETBIT(max, OA_STATUS_MAX);
        if(flag>=max)
        {
            flag = 0;
        }
        else
        if(!PUBLIC_CHECKBIT(flag,OA_STATUS_VALID_FLAG))
        {
            flag = 0;
        }
        else
        if((PUBLIC_CHECKBIT(flag,OA_STATUS_UNLOCK)&&PUBLIC_CHECKBIT(flag,OA_STATUS_LOCK))
         ||(!PUBLIC_CHECKBIT(flag,OA_STATUS_UNLOCK)&&!PUBLIC_CHECKBIT(flag,OA_STATUS_LOCK)))
        {
            flag = 0;
        }         
    }
    s_ucOpenAccountStatus = flag;
    ///////////////////
    if(0==s_ucOpenAccountStatus)//
    {
        s_ucOpenAccountStatus = 0;
        PUBLIC_SETBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK);
        PUBLIC_CLRBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK);
        OpenAccount_SaveStatus();
    }    
}
/*************************************************************
** 函数名称: OpenAccount_GetCurLockStatus
** 功能描述: 获得快速开户锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char OpenAccount_GetCurLockStatus(void)
{
    if(PUBLIC_CHECKBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK)&&0==PUBLIC_CHECKBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK))
        return 1;
    return 0;
}
/*************************************************************
** 函数名称: OpenAccount_SetLockStatus
** 功能描述: 快速开户置为锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_SetLockStatus(void)
{
    PUBLIC_SETBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK);
    PUBLIC_CLRBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK);
    OpenAccount_SaveStatus();
}
/*************************************************************
** 函数名称: OpenAccount_ClearLockStatus
** 功能描述: 快速开户清除锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_ClearLockStatus(void)
{
    ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
    ClearTerminalAuthorizationCode(CHANNEL_DATA_2);
    ///////////////////////
    PUBLIC_SETBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK);
    PUBLIC_CLRBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK);
    OpenAccount_SaveStatus();
    LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND]);
    LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
    ///////////////////////////////////
    OpenAccount_RecoverOriginalIPAddress();
}
/*************************************************************
** 函数名称: OpenAccount_CheckOnlineAndSendQuickOpenAccount
** 功能描述: 
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_CheckOnlineAndSendQuickOpenAccount(void)
{
    u8 channel = CHANNEL_DATA_1;//为编译通过而添加
    static u8 time = 0;

     if((Modem_IpSock_STA[EYENET_FIRST_SOC] == MODSOC_ONLN)&&(First_IP() == VAL_IPTO))//指定ip在线

      {
          if(ACK_OK==RadioProtocol_OpenAccount(channel))//
          {
              LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND]);
              LZM_PublicSetCycTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK],PUBLICSECS(1),Check_Result);
          }
      }
    
    time ++;
    if(time > 30)  //连接失败
    {
       Public_ShowTextInfo("无连接!", PUBLICSECS(3)); 
       LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND]);

    }
}
/*************************************************************
** 函数名称: OpenAccount_LinkToFreightPlatform
** 功能描述: 连接到指定的货运平台
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_LinkToFreightPlatform(void)
{
     SOC_IPVAL ip_value;
     u8 tab[2];
     u8 time = 0;
     //char MainIp[] = "121.15.212.249";   //测试使用
     //char Port[] = "9201"; 
     char MainIp[] = "jt1.gghypt.net";
     char Port[] = "1008"; 
     /*u8 buff[20];
         memset(buff,0,20);
         EepromPram_WritePram(E2_UPDATA_DIAL_NAME_ID, 0, 20);   //鉴权码清零
    */
     strcpy((char *)ip_value.ip,MainIp);
     strcpy((char *)ip_value.port,Port);
     strcpy((char *)&ip_value.mode,"TCP");
    
     EepromPram_ReadPram(E2_UPDATA_TIME_ID, tab);
     time = (tab[0] << 8)|tab[1];
     ClrTimerTask(TIME_CLOSE_GPRS);
     Communication_Open();
     Net_First_ConTo(ip_value, time);       //切换到指定服务器
     OpenAccount_SaveStatus();
     LZM_PublicSetCycTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND],PUBLICSECS(1),OpenAccount_CheckOnlineAndSendQuickOpenAccount); 
}
/*****************************************************************
** 函数名称: OpenAccount_CheckAndDisposeLockStatus
** 功能描述: 检查和处理快速开户锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_CheckAndDisposeLockStatus(void)
{

    if(!Check_First_Register_Flag())    //第一次开户 fanqinghai 2015.11.23

    {
        if(0==s_ucfisrtRunFlag)
        {
            OpenAccount_ParameterInitialize();
        }
        ///////////////////////
      
        if(OpenAccount_GetCurLockStatus())//连到
        {
            Public_ShowTextInfo("快速开户中", PUBLICSECS(20));
            OpenAccount_LinkToFreightPlatform();
        }
        else
        {
            Public_ShowTextInfo("终端未锁定", PUBLICSECS(5));
        }

    }
    else
    {
        Public_ShowTextInfo("开户已完成", PUBLICSECS(5));

    }
    ///////////////////////
}
/*************************************************************
** 函数名称: Check_Result
** 功能描述: 检查开户结果
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
** 作者:fanqinghai 2015.11.23
*************************************************************/
void Check_Result(void)
{
    static u8 time = 0 ;
    if(3 == UnlockResult)
    {
       UnlockResult = 0;
       OpenAccount_ClearLockStatus();
       LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
       Set_First_Register_Flag();                //置位开户标志
       Public_ShowTextInfo("终端解锁成功", PUBLICSECS(5));
       LZM_PublicSetOnceTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK],PUBLICSECS(4),ShowMainInterface);         
    }
    else if(1 == UnlockResult)
    {
          UnlockResult = 0;
          OpenAccount_SetLockStatus();
          LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
          OpenAccount_RecoverOriginalIPAddress();  
          Public_ShowTextInfo("终端解锁失败", PUBLICSECS(5));
    }
    else if(2 == UnlockResult)   
    {
          UnlockResult = 0;
          OpenAccount_SetLockStatus();
          LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
          OpenAccount_RecoverOriginalIPAddress();  
          Public_ShowTextInfo("消息有误", PUBLICSECS(5));
    }
    else
    {
        time ++ ;
        if(time > 10)
        {
            time = 0;
            UnlockResult = 0;
            LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
            Public_ShowTextInfo("无应答", PUBLICSECS(5));

        }
    }

}
/*************************************************************
** 函数名称: OpenAccount_RecoverOriginalIPAddress
** 功能描述: 检查和处理快速开户锁定状态
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_RecoverOriginalIPAddress(void)
{

        First_Switch_Net_Specific_To_MainIP();
        OpenAccount_SaveStatus();
}
/*************************************************************
** 函数名称: OpenAccount_DisposeRadioProtocol
** 功能描述: 快速开户协议解析
** 入口参数: pBuffer:数据首地址,BufferLen:数据长度
** 出口参数: 无
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned short ID;
    unsigned char result;
    ID=Public_ConvertBufferToShort(&pBuffer[2]);
    UnlockResult = 0;
    if(0x0110==ID)//快速开户
    {
        result = pBuffer[4];
        if((0==result)||(5==result))      // 开户成功,dxl,2016.4.15增加了result==5的情况（已开过户应答为5）
        {
            UnlockResult = 3;
        }
        else if(1 == result)//失败
        
        {
            UnlockResult = 1;
        }
        else if(2== result)    //消息有误
       {
           UnlockResult = 2;
       }
        else
        {
            UnlockResult = 0;
        }
    }
}
/*************************************************************
** 函数名称: OpenAccount_GetEncryptFlag
** 功能描述: 是否加密
** 入口参数: 
** 出口参数: 无
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char OpenAccount_GetEncryptFlag(void)
{
    unsigned char flag;
    if(!EepromPram_ReadPram(E2_TOPLIGHT_CONTROL_ID, &flag))
    {
        flag = 0;
    }
    return (1==flag)?1:0;
}
/*************************************************************
** 函数名称: OpenAccount_TimeTask
** 功能描述: 快速开户定时器处理函数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
FunctionalState OpenAccount_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stAccountTimer,ACCOUNT_TIMERS_MAX);
    return ENABLE;
}
/*************************************************************
** 函数名称: OpenAccountParameterInitialize
** 功能描述: 快速开户参数初始化
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void OpenAccount_ParameterInitialize(void) 
{
    LZM_PublicKillTimerAll(s_stAccountTimer,ACCOUNT_TIMERS_MAX);
    ////////////////////////////// 
    OpenAccount_ReadLockStatus();    
  /*  if(OpenAccount_GetCurLockStatus())     //fanqinghai 2015.11.18
    {
        LZM_PublicSetOnceTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK],PUBLICSECS(15),OpenAccount_CheckAndDisposeLockStatus);
    }*/
    /////////////////////////
    s_ucfisrtRunFlag = 1;
   SetTimerTask(TIME_ACCOUNT,SYSTICK_50ms);          
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

