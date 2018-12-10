/*
********************************************************************************
*
*                                                              
*
* Filename      : modem_app_first.c
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-10-28  
* 
*
********************************************************************************
*/

/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#ifdef EYE_MODEM
#include "include.h"
#include "VDR_Pub.h"
#include "VDR_Data.h"
#else
//#include  <app_cfg.h>
#include  <includes.h>
#endif

/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/
#ifdef MODEM_APP_FIRST_H
/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/

#define  EYENET_FIRST_SOC                         (0)//SOC号

/*
********************************************************************************
*                         LOCAL DATA TYPES
********************************************************************************
*/

/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/
static Modem_APP_STATE NetFirst_State;    //状态机
static APP_TYPE_DOWN   NetFirst_TypeDown; //下线类型
static SOC_IPVAL       NetFirst_AppointIp;//指定IP
static APP_TYPE_VAL    NetFirst_IpVal;    //参数

static u8  NetFirst_ConSta;//连接状态 0为下线  1为在线
static u8  NetFirst_ConTry;//连接重试次数
static u32 NetFirst_ConCnt;//当前连接类型持续计数器
static u32 NetFirst_ConVal;//当前连接类型持续时间

static u32 NetFirst_HeartVal;//心跳值
static u32 NetFirst_HeartCnt;//心跳计数器

/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/

/*
********************************************************************************
*                          EXTERN VARIABLES
********************************************************************************
*/

/*
********************************************************************************
*                           FUNCTIONS EXTERN
********************************************************************************
*/


/*
********************************************************************************
*                              FUNCTIONS
********************************************************************************
*/


/**
  * @brief  返回连接2的IP
  * @param  type: 参数类型
  * @retval 连接2的使用的哪个IP
  *****Author: fanqinghai
  *****data**: 2016.02.23
  */

APP_TYPE_VAL First_IP(void)
{
    
    return NetFirst_IpVal;

}


/**
  * @brief  接获取参数
  * @param  type: 参数类型
  * @retval 返回参数结构 1->成功  0->非法数据
  */
u8 Net_First_Value(APP_TYPE_VAL type, void *pValue)
{
    u8  tab[50]={0};
    u8  len;
    u8  result=0;
    u32 id_ip =0;
    u32 id_port = 0;
    u16 temp;

    SOC_IPVAL *pstip = pValue;
    u32 *pHeart = pValue;

    if(type == VAL_IPMAIN)//主IP
    {
         id_ip   = E2_MAIN_SERVER_IP_ID;
         id_port = E2_MAIN_SERVER_TCP_PORT_ID;
    }
    else if(type == VAL_IPBACK)//备份IP
    {
         id_ip   = E2_BACKUP_SERVER_IP_ID;
         id_port = E2_MAIN_SERVER_TCP_PORT_ID;
    }
    else if(type == VAL_IPTO)//指定IP
    {
        *pstip = NetFirst_AppointIp;
        return 1;
    }
    else if(type == VAL_HEART)//心跳
    {
        if(EepromPram_ReadPram(E2_TERMINAL_HEARTBEAT_ID, tab) == 4)
        {
            *pHeart = (tab[0] << 24) + (tab[1] << 16) + (tab[2] << 8) + tab[3];
            return 1;//参数可用
        }
    }
    len = EepromPram_ReadPram(id_ip, tab);
    if((len > 0)&&(len < 30))
    {
        result = 0x0a;
        memcpy(pstip->ip,tab,len);
    }

    len = EepromPram_ReadPram(id_port, tab);
    if((len > 0)&&(len < 5))
    {
        result <<= 0x04;
        result |= 0x0a;
        temp = (tab[0] << 24) + (tab[1] << 16) + (tab[2] << 8) + tab[3];
			
			  ////////////////dxl,2017.4.27,增加,当TCP端口号为0时，使用UDP连接/////////////////// 
        if(0 == temp)
				{
            len = EepromPram_ReadPram(E2_MAIN_SERVER_UDP_PORT_ID, tab);
					  if((len > 0)&&(len < 5))
						{
                temp = (tab[0] << 24) + (tab[1] << 16) + (tab[2] << 8) + tab[3];
							  result = 0xbb;
            }
        }
        ////////////////end///////////////////////////////////////////////////////////////			
			
        sprintf((char*)pstip->port,"%d",temp);
    }
    if(result == 0xaa)
    {
        strcpy((char*)pstip->mode,"TCP");//模式
        return 1;//参数可用
    }
		else if(result == 0xbb)//dxl,2017.4.27,新增
    {
        strcpy((char*)pstip->mode,"UDP");//模式
        return 1;//参数可用
    }
    return 0;//参数非法
}

/**
  * @brief  连接登录，主要是触发注册鉴权任务
  * @param  type: 保留 
            state: 下一步执行的状态
  * @retval None
  */
/*void Net_First_Login(Modem_APP_STATE state)
{
    NetFirst_ConTry = 0;
    NetFirst_ConCnt = 0;
    NetFirst_State  = state;
    SetLink1LoginAuthorizationState(LOGIN_READ_AUTHORIZATION_CODE);
    SetTimerTask(TIME_LINK1_LOGIN, SYSTICK_1SECOND);    
}*/
    /********************************************************************************
    */
    /**
      * @brief  接获取参数
      * @param  type: 参数类型
      * @retval 返回参数结构 1->成功  0->非法数据
      * 修改人:fanqinghai 2016.01.07
     * 说明:  使用前锁定功能用到
      */
void Net_First_Login(Modem_APP_STATE state)   
{
	  NetFirst_ConTry = 0;
    NetFirst_ConCnt = 0;
    NetFirst_State  = state;

    if(1 == Lock2_GetOpenAccountRequestFlag())   //检测开户完成标志
    {
        ClrTimerTask(TIME_LINK1_LOGIN); 
        Lock2_OpenAccountOnline();		
    }
		else if(1 == MultiCenterConnect_GetRequestFlag())   //检测多中心连接标志
    {
        ClrTimerTask(TIME_LINK1_LOGIN); 		
    }
    else
    {
			  SetLink1LoginAuthorizationState(LOGIN_READ_AUTHORIZATION_CODE);
        SetTimerTask(TIME_LINK1_LOGIN, SYSTICK_1SECOND);   	
    }
	
}

/**
  * @brief  连接下线，主要是清除捡软标志
  * @param  type: 下线类型
            state: 下一步执行的状态
  * @retval None
  */
void Net_First_Down(APP_TYPE_DOWN type, Modem_APP_STATE state)
{
    NetFirst_TypeDown = type;//刷新类型
    NetFirst_State    = state;//刷新状态
    NetFirst_ConSta   = 0;//连接下线
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);//鉴权标志清0
    
    //////////////////////////////////////////////////////////////////////切换IP
    if(type == DOWN_IPSW)
    {
        if((++NetFirst_ConTry) > 2)
        {
            NetFirst_ConTry = 0;
            if(NetFirst_IpVal == VAL_IPMAIN)
            {
                NetFirst_ConVal = 10*MODEM_TMIN;
                NetFirst_IpVal = VAL_IPBACK;//主IP-重试失败转到-备份IP
            }
            else if(NetFirst_IpVal == VAL_IPBACK)
            {
                NetFirst_IpVal = VAL_IPMAIN;//备份IP-重试失败转到-主IP
            }
            else if(NetFirst_IpVal == VAL_IPTO)
            {
                NetFirst_IpVal = VAL_IPMAIN;//指定IP-重试失败转到-主IP
            }            
        }  
    }
    //////////////////////////////////////////////////////////////////////当前IP
    else if(type == DOWN_IPOP)
    {
        ;//保留
    }
    ////////////////////////////////////////////////////////////////////////主IP
    else if(type == DOWN_IPMN)
    {
        NetFirst_IpVal = VAL_IPMAIN;//主IP
    }   
    //////////////////////////////////////////////////////////////////////指定IP
    else if(type == DOWN_IPTO)
    {
        NetFirst_IpVal = VAL_IPTO;//指定IP
    }     
}

/**
  * @brief  联接初始化
  * @param  None
  * @retval None
  */
void Net_First_Init(void)
{
    SOC_IPVAL ip;
    u32 heartval;
    
    NetFirst_State  = MODEM_APP_IDLE;
    NetFirst_IpVal  = VAL_IPMAIN;//默认主连接
    NetFirst_ConVal = 0;//默认一直连接
    NetFirst_ConSta = 0;//默认下线
    NetFirst_HeartVal = MODEM_TMIN*5;//默认5分钟心跳
    
    //////////////////////////////////////////////////////////////////鉴权标志清
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);
    
    //////////////////////////////////////////////////////////////////////IP参数
    if(Net_First_Value(NetFirst_IpVal,&ip) == 0)
    {
        return;//未设置连接IP或参数错误
    }
    NetFirst_State = MODEM_APP_OPEN;//打开链接
    
    ////////////////////////////////////////////////////////////////////心跳参数
    if(Net_First_Value(VAL_HEART,&heartval))
    {
        NetFirst_HeartVal = heartval*3*MODEM_TSEC;// 3个心跳值超时
    }
    if((NetFirst_HeartVal > (MODEM_TMIN*5))||(NetFirst_HeartVal < 30))//dxl,2015.4.21,增加最小值限制,防止心跳包参数设置错误的情况
    {
        NetFirst_HeartVal = MODEM_TMIN*5;//强制转为5分钟
    }
    
}
/*
  * @brief  连接回调函数,平台下发数据时被调用
  * @param  *p；指向连接下发的数据
  * @param  len：数据长度
  * @retval None
  */
void Net_First_Call(u8 *p,u16 len)
{
    NetFirst_ConSta   = 1;//置在线标志
    NetFirst_HeartCnt = 0;//清心跳计数
    RadioProtocol_AddRecvDataForParse(CHANNEL_DATA_1,p,len);   
}
/*
  * @brief  连接IP参数,连接IP时被调用
  * @param  *pMode：指向连接类型
  * @param  *pAddr：指向IP地址
  * @param  *pPort：指向端口号
  * @retval None
  */
void Net_First_IP(u8 *pMode ,u8 *pAddr, u8 *pPort)
{ 
    SOC_IPVAL ip;

    memset((void*)&ip,0,sizeof(SOC_IPVAL));//清空结构体
    
    if(Net_First_Value(NetFirst_IpVal,&ip) == 0)
    {
        NetFirst_IpVal = VAL_IPMAIN;//参数非法将强制转为主服务器
        Net_First_Value(NetFirst_IpVal,&ip);
    }
    strcpy((char*)pMode,(char*)ip.mode); //TCP或UDP
    strcpy((char*)pAddr,(char*)ip.ip);   //IP地址或域名
    strcpy((char*)pPort,(char*)ip.port); //端口
}


/*
  * @brief  打开连接
  * @param  None
  * @retval None
  */
void Net_First_Open(void)
{
    if((NetFirst_State == MODEM_APP_OPEN))//正在打开连接
    {
        return;
    }
    NetFirst_State = MODEM_APP_OPEN;
}
/*
  * @brief  关闭连接
  * @param  None
  * @retval None
  */
void Net_First_Close(void)
{
    if(NetFirst_State != MODEM_APP_CLOSE)
    {
        NetFirst_State = MODEM_APP_CLOSE;
    }
}
/*
  * @brief  连接到指定IP
  * @param  ip_value: ip参数;
            time:有效截止时间
  * @retval None
  */
void Net_First_ConTo(SOC_IPVAL ip_value, u16 time)
{
    NetFirst_AppointIp = ip_value;//IP参数
    NetFirst_ConVal    = MODEM_TMIN*time;//允许连接的时间 
    NetFirst_State     = MODEM_APP_DOWN;//当前连接时间已到
    NetFirst_TypeDown  = DOWN_IPTO;//下线连接到指定IP  
   // NetFirst_State     = MODEM_APP_OPEN;//当前连接时间已到

}

/*
  * @brief  发送数据到连接平台
  * @param  None
  * @retval 成功返回长度，失败返回0
  */
u16  Net_First_Send(u8 *p, u16 len)
{
    u16 sendlen=0;

    if(Modem_State_SocSta(EYENET_FIRST_SOC))
    {
        sendlen = Modem_Api_Socket_Send(EYENET_FIRST_SOC,p,len);
    }
   
    return sendlen;
}
/**
  * @brief  连接监听(长期处于该状态)
  * @param  None
  * @retval None
  */
void Net_First_Listen(void)
{
    ////////////////////////////////////////////////////////////////////链路检查
    if(Modem_Api_SocSta(EYENET_FIRST_SOC) == __FALSE)
    {
        Net_First_Down(DOWN_IPOP,MODEM_APP_OPEN);
        return;
    } 
    //////////////////////////////////////////////////////////备份或指定连接检查
    if(NetFirst_ConVal)
    {
        if((NetFirst_ConCnt++) > NetFirst_ConVal)
        {
            NetFirst_ConCnt   = 0;
            NetFirst_ConVal   = 0;
            NetFirst_State    = MODEM_APP_DOWN;//关闭连接 -> 连接时间已到
            NetFirst_TypeDown = DOWN_IPMN;//下线切换到主IP
        }
    }
    ////////////////////////////////////////////////////////////////心跳超时检查
    if((NetFirst_HeartCnt++) > NetFirst_HeartVal)
    {
        NetFirst_HeartCnt = 0;
        NetFirst_State    = MODEM_APP_DOWN;//关闭连接 -> 心跳超时
        NetFirst_TypeDown = DOWN_IPOP;//下线重新打开(IP不变)   
    }
    
}

/**
  * @brief  连接状态
  * @param  None
  * @retval 0 -> 不在线
            1 -> 在线
  */
u8 Net_First_ConSta(void)
{
    return NetFirst_ConSta;
}

/**
  * @brief  连接检查
  * @param  None
  * @retval None
  */
void Net_First_Check(void)
{

}


/**
  * @brief  调试专用
  * @param  None
  * @retval None
  */
void Net_First_Debug(void)
{
    static u8 First_debug;
    
    if(First_debug)
    {
        First_debug = 0;
    }
}
/**
  * @brief  连接到指定服务器
  * @param  None
  * @retval None
  ****:备注:以前没有相应的直接接口用来连接到指定服务器
  ****:作者:fanqinghai 2016.02.19
  */
void First_connect_To_Specific_Server(void)
{
    SOC_IPVAL ip_value;
    u16 time;
    u16 temp;
    u8  tab[50]={0};
    u8  len;

    memset(tab,0,50);
    EepromPram_ReadPram(E2_UPDATA_IP_ID, tab);
    strcpy((char *)ip_value.ip,(char*)tab);
    len = EepromPram_ReadPram(E2_UPDATA_TCP_PORT_ID, tab);
    if((len > 0)&&(len < 3))
    {
        temp = (tab[0] << 8 )| tab[1] ;
        sprintf((char*)ip_value.port,"%d",temp);
    }
    EepromPram_ReadPram(E2_UPDATA_TIME_ID, tab);
    time = (tab[0] << 8)|tab[1];
    strcpy((char *)&ip_value.mode,"TCP");

    Net_First_ConTo(ip_value, time);
}


/**

  ****:功能:从指定的ip切换到主ip
  ****:作者:fanqinghai 2016.02.19
  */
void First_Switch_Net_Specific_To_MainIP(void)
{

    NetFirst_State = MODEM_APP_DOWN;   //当前指定连接下线
    Net_First_Down(DOWN_IPSW,MODEM_APP_OPEN);
}

/**
  * @brief  连接处理
  * @param  None
  * @retval None
  */
void Net_First_Run(void)
{

    MODERR_TYPE modemack;

    Net_First_Debug();

    Net_First_Check();
    
    switch(NetFirst_State)
    {
        case MODEM_APP_IDLE:
            {
                ;
            }
        break; 
        case MODEM_APP_OPEN://开打一个连接
            {
                modemack = Modem_Api_Socket_Open(EYENET_FIRST_SOC,
                                                 Net_First_IP,
                                                 Net_First_Call);
                if(modemack == MOD_OK)
                {
                    Net_First_Login(MODEM_APP_LISTEN);//成功->进入监听
                }
                else if(modemack == MOD_ER)
                {
                    Net_First_Down(DOWN_IPSW,MODEM_APP_OPEN);//失败->切换IP
                }
            }
        break;     
        case MODEM_APP_LISTEN://监听一个连接,长期处于该状态
            {
                Net_First_Listen();
            }
        break;        
        case MODEM_APP_CLOSE://关闭一个连接
            {   
                modemack = Modem_Api_Socket_Close(EYENET_FIRST_SOC);
                if(modemack == MOD_OK)
                {
                    NetFirst_State = MODEM_APP_IDLE;//等待应用打开
									  ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);//dxl,2016.4.20
                }
                else if(modemack == MOD_ER)
                {
                    Modem_Api_Rst();//模块异常->重启模块
                }
            }
        break;        
        case MODEM_APP_DOWN://主动下线
            {
                modemack = Modem_Api_Socket_Close(EYENET_FIRST_SOC);
                if(modemack == MOD_OK)
                {
                    Net_First_Down(NetFirst_TypeDown,MODEM_APP_OPEN);
                }
                else if(modemack == MOD_ER)
                {
                    Modem_Api_Rst();//关闭连接失败后重启通讯模块
                }
            }
        break; 
        default:
        break;

    }
}


/*
********************************************************************************
*                 DEFIEN END
********************************************************************************
*/
#endif 







