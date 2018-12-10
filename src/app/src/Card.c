/************************************************************************
//程序名称：Card.c
//功能：实现部标驾驶员身份识别功能
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2016.4
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：驾驶员身份识别功能
//1.背景介绍：
//（1）要实现驾驶员身份识别功能，主机需接专用的IC卡读卡器（目前使用的是雄帝读卡器EMP7500）
//（2）使用连接2的连接指定服务器功能连接到IC卡认证平台，完成驾驶员身份认证或定时30分钟后切回正常使用第2个连接
//（3）此IC卡驾驶员身份认证功能与国标的IC卡签到签退功能是两个不同的功能，IC卡本身也不同，不要混淆
//（4）IC卡读卡器与主机连接的接口是16PIN上串口，与导航屏是共用一个接口，两者只能选择一个，接了导航屏就不能接IC卡读卡器
//（5）IC卡读卡器与主机通信的协议是标准的部标外设通信协议，见部标协议附录A
//（6）主机与IC卡认证平台的通信协议是标准的部标透传指令，消息类型是0x0B
//（7）使用雄帝读卡器实现的驾驶员签到、签退只报给平台，与行驶记录仪的签到签退无任何关联
//（理论上应该有，但现在由于该加密IC卡政策和格式不明确，所以没有做关联）
//2.功能介绍：
//（1）设置IC卡认证服务器IP地址或域名、TCP端口号、UDP端口号，检测时由检测平台下发这些参数的设置命令
//（2）插入IC卡到IC卡读卡器
//（3）IC卡读卡器发送卡片认证请求指令0x40H到主机，主机判断当前是否在线（即是否与IC卡认证平台连接上了）
//（4）若主机不在线，则需应答读卡器主机不在线，并尝试连接IC卡认证服务器（注意连上IC卡服务器后不需要发送注册或鉴权指令）
//（5）若主机在线，则需将卡片认证请求指令透传（透传指令0x0900）给IC卡认证平台
//（6）IC卡认证平台核准卡片认证请求后，会下发卡片认证应答，主机收到后将卡片认证应答透传给IC卡读卡器
//（7）IC卡读卡器收到卡片认证请求成功后，读取IC卡信息，并发送IC卡读取结果通知指令0x41H到主机
//（8）主机判断IC卡读取结果是成功还是失败，若成功，将IC卡信息上报（上报指令为0x0702）给IC卡认证平台；若失败，则语音播报提醒读卡失败
//上面（1）--（8）是驾驶员身份识别并上报平台驾驶员签到的流程，第（9）步以后是驾驶员签退流程
//（9）将IC卡从IC卡读卡器中拔出
//（10）IC卡读卡器发送拔卡通知指令0x42H，主机收到后发送驾驶员签退指令（0x0702）到IC卡认证平台
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include "Card.h"
#include "modem_core.h"
#include "EepromPram.h"
#include "Public.h"
#include "RadioProtocol.h"
#include "VDR_Pub.h"
#include "Usart2.h"
#include "other.h"
#include "queue.h"
#include "VDR_Usart.h"

/********************本地变量*************************/
static u8 state = 0;
static u8 CardInsert = 0;//0表示卡未插入；1表示卡插入了
static u8 LastCardInsert = 0;
static u8 LoginFlag = 0;//1表示有签到需求；2表示签到中，3表示签到完成；其它值无含义
static u8 LogoutFlag = 0;//1表示有签退需求；2表示签退中，3表示签退完成；其它值无含义
static u8 CardRxBuffer[100];//IC卡接收缓冲
//static u8 CardRxBusyFlag = 0;
static u8 CardTxBuffer[100];//IC卡发送缓冲
static u8 CardTxBusyFlag = 0;
static u8 CardAuthenticationRequest[70] = {0};//64字节的认证数据
static u8 CardAuthenticationAck[30] = {0};//24字节的认证应答数据

static u8 CardAuthenticationLinkFlag = 0;//IC卡认证连接标志，1表示当前为IC卡认证连接，0表示正常应用的连接
static u8 LastCardAuthenticationLinkFlag = 0;
static u16 CommunicationProtocolVersion = 0;//通信协议版本
static u16 ManufactureID = 0;//厂商编号
static u8 PeripheralType = 0;//外设类型
static u8 UsartCardRxBuffer[1000] = {0};
static u16 UsartCardRxCount = 0;

static u32 CmdRecCount = 0;

/********************全局变量*************************/
u8 CardDriverWorkFlag = 0;//驾驶员上班标志，1为上班，2为下班，0为未签到过
u8 CardDriverInfo[100] = {0};//};//驾驶员身份上报消息体（0x0702）中的驾驶员姓名长度、驾驶员姓名、从业资格证编码、发证机构名称长度、发证机构名称、证件有效期
u8 CardDriverInfoLen = 0;
u8 CardDriverWorkStartTime[10] = {0};//驾驶员身份上报消息体（0x0702）中的状态(固定为1)，时间，IC卡读取结果，共8字节
u8 CardDriverWorkEndTime[10] = {0};//驾驶员身份上报消息体（0x0702）中的状态(固定为2)，时间，共7字节

/********************外部变量*************************/
extern u8 HexToAscBuffer[];
/********************本地函数声明*********************/
static void CARDState_Idle(void);
static void CARDState_ConnectNet(void);
static void CARDState_AuthenticationRequest(void);
static void CARDState_DriverLogin(void);
static void CARDState_DriverLogout(void);
static u8 Card_SendCmd(u8 cmd, u8 *pBuffer , u16 BufferLen);
static void Card_Init(void);
static void CARDState_Switch(u8 SwitchState);

/********************函数定义*************************/

/**************************************************************************
//函数名：Card_TimeTask
//功能：部标驾驶员身份识别功能
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：锁定一定时任务，50ms秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState Card_TimeTask(void)
{

	  static u8 InitFlag = 0;
	  static u32 count = 0;
	  static u8 flag;
	  static u32 count2 = 0;
	
	  u16 length;
	  
	  if(0 == ReadPeripheral2TypeBit(6))
		{
		    return ENABLE;
		}
	  
		length = COM2_ReadBuff(UsartCardRxBuffer+UsartCardRxCount,100); 
		if(0 != length)
		{
		    UsartCardRxCount += length;
			  count2 = 0;
		}
		else
		{
		    count2++;
			  if(count2 >= 2)
				{
				    count2 = 0;
					  if(UsartCardRxCount >= 8)
						{
					      Card_ParseCmd(UsartCardRxBuffer,UsartCardRxCount);
						}
					  UsartCardRxCount = 0;
				}
		}
		
		
	  if(0 == InitFlag)
		{
			  Card_Init();
			  InitFlag = 1;
		}
	
    switch(state)
		{
			case CARD_STATE_IDLE://空闲状态
			{
				CARDState_Idle();
			  break;
			}
			case CARD_STATE_CONNECT_NET://连认证中心网络
			{
				CARDState_ConnectNet();
			  break;
			}
			case CARD_STATE_AUTHENTICATION_REQUEST://认证请求
			{
				CARDState_AuthenticationRequest();
			  break;
			}
			case CARD_STATE_DRIVER_LOGIN://驾驶员签到
			{
				CARDState_DriverLogin();
			  break;
			}
			case CARD_STATE_DRIVER_LOGOUT://驾驶员签退
			{
				CARDState_DriverLogout();
			   break;
			}
			default:
			{
			   state = CARD_STATE_IDLE;
				 break;
			}
		}
		
		if(LastCardAuthenticationLinkFlag != CardAuthenticationLinkFlag)
		{
			  count = 0;  
			  flag = 1;
		}
		if(1 == flag)//30分钟内没有完成签到或签退，重新初始化通信模块
		{
			  count++;
			  if(count >= 36000)
				{
			      count = 0;
			      flag = 0;
				    CardInsert = 0;
				    LastCardInsert = 0;
				    LoginFlag = 0;
				    LogoutFlag = 0;
				    CardAuthenticationLinkFlag = 0;
					  LastCardAuthenticationLinkFlag = 0;
				    Communication_Init();
				}
		}
		else
		{
		    count = 0;
		}
		
		LastCardAuthenticationLinkFlag = CardAuthenticationLinkFlag;

    return ENABLE;      
}
/**************************************************************************
//函数名：Card_GetLinkFlag
//功能：获取IC卡认证连接标志
//输入：无
//输出：无
//返回值：1：表示IC卡连接；0：不是IC卡连接
//备注：当为IC卡连接时，不能走注册鉴权流程
***************************************************************************/
u8 Card_GetLinkFlag(void)
{
    return CardAuthenticationLinkFlag;
	  //return 1;
}
/**************************************************************************
//函数名：Card_AuthenticationAck
//功能：认证应答
//输入：无
//输出：无
//返回值：无
//备注：下一个状态为空闲
***************************************************************************/
void Card_AuthenticationAck(u8 *pBuffer , u16 BufferLen)
{
	  u16 length;
		
	  if(24 == BufferLen)
		{
        memcpy(CardAuthenticationAck+1,pBuffer,BufferLen);
			  CardAuthenticationAck[0] = 0;
			  length = BufferLen+1;
			  Card_SendCmd(0x40,CardAuthenticationAck,length);//发送指令到IC卡读卡器
		}
}
/**************************************************************************
//函数名：Card_ParseCmd
//功能：解析IC卡读卡器的指令
//输入：无
//输出：无
//返回值：无
//备注：无
***************************************************************************/
void Card_ParseCmd(u8 *pBuffer , u16 BufferLen)
{
	  u8 *p = NULL;
	  u8 i,j,k,m,n,count;
	  u8 VerifyCode;
	  u8 Buffer[20];

		
		p=pBuffer;
		count = 0;
		for(i=0; i<BufferLen; i++) 
		{
		    if(0x7e == *(p+i))//找到标识位头
				{
					  count++;
					  if(1 == count)//第1个7E
						{
						    j = i;
						}
						else if(2 == count)//第2个7E
						{
							  k = i;
						    break;
						}
				}
		}
		
		if((2 == count)&&(k-j < 100))
		{
			  memcpy(CardRxBuffer,pBuffer,k-j);
			  p = CardRxBuffer;
			  m = unTransMean(p+j+1, k-j-1);//j+1:去掉帧头;k-j-1:去掉帧尾
			  VerifyCode = 0;
			  for(n=0; n<m-3; n++)
			  {
				    VerifyCode += *(p+j+4+n);
				}
				if((VerifyCode == *(p+j+1))&&(0x0B == *(p+j+6)))//校验码和外设类型均正确
				{
					  CommunicationProtocolVersion = 0;
					  CommunicationProtocolVersion |= *(p+j+2) << 8;
					  CommunicationProtocolVersion |= *(p+j+3);
					  ManufactureID = 0;
					  ManufactureID |= *(p+j+4) << 8;
					  ManufactureID |= *(p+j+5);
					  PeripheralType = *(p+j+6);
				    switch(*(p+j+7))
						{
							case 0x40://插卡
							{
								  CmdRecCount = 0;
								  if(0 == *(p+j+8))//读卡成功
									{
									    memcpy(CardAuthenticationRequest,p+j+9,64);
                      Public_ShowTextInfo("IC卡识别成功",100);
									    LastCardInsert = 0;
											CardInsert = 1;
										  Buffer[0] = 0x01;
										  //Card_SendCmd(0x40,Buffer,1);//发送指令到IC卡读卡器，告知读卡器终端不在线
									}
									else 
									{
									    if(1 == *(p+j+8))//IC卡未插入
									    {
										      Public_ShowTextInfo("IC卡未插入",100);
									    }
									    else if(2 == *(p+j+8))//IC卡读卡失败
									    {
									        Public_ShowTextInfo("IC卡识别失败",100);
									    }
									    else if(3 == *(p+j+8))//非IC卡从业资格证
									    {
									        Public_ShowTextInfo("非IC卡从业资格证",100);
									    }
									    else if(4 == *(p+j+8))//IC卡被锁定
									    {
									        Public_ShowTextInfo("IC卡被锁定",100);
									    }
									    else
									    {
									         Public_ShowTextInfo("识别卡未知错误",100);
									    }
									    CardInsert = 0;
											LastCardInsert = 0;
										  LoginFlag = 0;
										  LogoutFlag = 0;
									}
							    break;
							}
							case 0x41://读卡
							{
								  CmdRecCount++;
								  if(CmdRecCount > 1)
									{
									    CmdRecCount = 0;
									}
								  if(0 == *(p+j+8))//读卡成功
									{
									    memcpy(CardDriverInfo,p+j+9,m-8);
										  CardDriverInfoLen = m-8;										  
                      Public_ShowTextInfo("IC卡读卡成功",100);
										  CARDState_Switch(CARD_STATE_DRIVER_LOGIN);
										  
										  //Card_SendCmd(0x41,Buffer,0);
									}
									else 
									{
									    if(1 == *(p+j+8))//卡片密钥认证未通过
									    {
										      Public_ShowTextInfo("卡片密钥认证未通过",100);
									    }
									    else if(2 == *(p+j+8))//卡片已被锁定
									    {
									        Public_ShowTextInfo("卡片已被锁定",100);
									    }
									    else if(3 == *(p+j+8))//卡内信息为空
									    {
									        Public_ShowTextInfo("卡内信息为空",100);
									    }
									    else
									    {
									         Public_ShowTextInfo("读卡未知错误",100);
									    }
									    CardInsert = 0;
											LastCardInsert = 0;
										  LoginFlag = 0;
										  LogoutFlag = 0;
											//Card_SendCmd(0x41,Buffer,0);
									}
									
							    break;
							}
							case 0x42://拔卡
							{
								  if((1 == CardInsert)&&(3 == LoginFlag))
									{
									    CardInsert = 0;
										  Card_SendCmd(0x42,Buffer,0);
										  Public_ShowTextInfo("拔卡，开始签退流程",100);
									}
								  else
									{
										  CardInsert = 0;
										  LastCardInsert = 0;
										  LoginFlag = 0;
										  LogoutFlag = 0;
									    Public_ShowTextInfo("签到未完成拔卡无效",100);
									}
							    break;
							}
							case 0x43://自动触发40H指令重新读卡
							{

								break;
							}
							default:
								break;
						}
				}  
		}
		
}
/*************************************************************
** 函数名称: Card_GetExternalDriverInfo
** 功能描述: 获取驾驶员姓名
** 入口参数: 无
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned char Card_GetExternalDriverName(unsigned char *pBuffer)
{
	unsigned char DriverNameLen;

	DriverNameLen = CardDriverInfo[8];
	memcpy(pBuffer,CardDriverInfo+9,DriverNameLen);//拷贝姓名

	return DriverNameLen;
}

/**************************************************************************
//函数名：CARDState_Switch
//功能：状态切换
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void CARDState_Switch(u8 SwitchState)
{
    state = SwitchState;
}
/**************************************************************************
//函数名：Card_Init
//功能：初始化
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void Card_Init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin_11);//开启电源
	
	  CardInsert = 0;
		LastCardInsert = 0;
		LoginFlag = 0;
		LogoutFlag = 0;
	  CardAuthenticationLinkFlag = 0;
	  LastCardAuthenticationLinkFlag = 0;
}
/**************************************************************************
//函数名：CARDState_Idle
//功能：空闲状态，检测是否有连网需要，有则转入CARD_STATE_CONNECT_NET状态
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void CARDState_Idle(void)
{

	  static u32 unLinkCount = 0;
	  static u32 LinkCount = 0;
	
	  u8 Link;
	
	  Link = Modem_State_SocSta(1);
	
	  if((1 == CardInsert)&&(0 == LastCardInsert))//卡插入
		{
			  LoginFlag = 1;
			  LogoutFlag = 0;
				unLinkCount = 0;
			  CARDState_Switch(CARD_STATE_CONNECT_NET);
		}
		else if((0 == CardInsert)&&(1 == LastCardInsert))//卡拔出
		{
		    LoginFlag = 0;
			  LogoutFlag = 1;
				unLinkCount = 0;
			  CARDState_Switch(CARD_STATE_CONNECT_NET);
		}
		else if((1 == CardInsert)&&(0 != LoginFlag)&&(LoginFlag < 3))//卡在时定时检测是否在线，当完成签到时LoginFlag==3，当完成签退时LogoutFlag==3
		{
			  if(0 == Link)
				{
					  LinkCount = 0;
					  unLinkCount++;
					  if(unLinkCount >= 1200)//60秒
		        {
							  unLinkCount = 0;
				        //CARDState_Switch(CARD_STATE_CONNECT_NET);
								LoginFlag = 1;
				    }
				}
				else if(1 == Link)
				{
					  unLinkCount = 0;
					  LinkCount++;
					  if((1 == LoginFlag)&&(LinkCount > 140))
						{
							  LinkCount = 0;
						    LoginFlag = 2;
							  CARDState_Switch(CARD_STATE_AUTHENTICATION_REQUEST);
						}	
				}
		}
		else if((0 == CardInsert)&&(0 != LogoutFlag)&&(LogoutFlag < 3))//无卡时
		{
		    if(0 == Link)
				{
					  LinkCount = 0;
					  unLinkCount++;
					  if(unLinkCount >= 1200)
		        {
							  unLinkCount = 0;
                LogoutFlag = 1;
								//CARDState_Switch(CARD_STATE_CONNECT_NET);
				    }
				}
				else if(1 == Link)
				{
					  unLinkCount = 0;
					  LinkCount++;
					  if((1 == LogoutFlag)&&(LinkCount > 140))
						{
							  LinkCount = 0;
						    LogoutFlag = 2;
							  CARDState_Switch(CARD_STATE_DRIVER_LOGOUT);
						}	
				}
			  
		}
		
		LastCardInsert =  CardInsert;

		
}
/**************************************************************************
//函数名：CARDState_ConnectNet
//功能：连网
//输入：无
//输出：无
//返回值：无
//备注：下一个状态为空闲
***************************************************************************/
static void CARDState_ConnectNet(void)
{
	/*
	     SOC_IPVAL ip_value;
       u32 temp;
       u8  tab[50];
       u8  len;

			
        memset(tab,0,50);
        len = EepromPram_ReadPram(E2_IC_MAIN_SERVER_IP_ID, tab);
	      if((len > 0)&&(len < E2_IC_MAIN_SERVER_IP_LEN))
		    {
            strcpy((char *)ip_value.ip,(char*)tab);
		    }
		    else
		    {
			      CARDState_Switch(CARD_STATE_IDLE);
			      Public_ShowTextInfo("IC卡服务器IP错误",100);
			      return ;
		    }
	
	      memset(tab,0,50);
        len = EepromPram_ReadPram(E2_IC_MAIN_SERVER_TCP_PORT_ID, tab);
        if(4 == len)
        {
            temp = Public_ConvertBufferToLong(tab);
            sprintf((char*)ip_value.port,"%d",temp);
        }
		    else
		    {
			      CARDState_Switch(CARD_STATE_IDLE);
			      Public_ShowTextInfo("IC卡服务器TCP错误",100);
			      return ;
		    }
    
        strcpy((char *)&ip_value.mode,"TCP");
		
        Net_Second_ConTo(ip_value, 10);//强制连接时间为10分钟
				*/
		
		    CardAuthenticationLinkFlag = 1;
		
		    CARDState_Switch(CARD_STATE_IDLE);
}
/**************************************************************************
//函数名：CARDState_AuthenticationRequest
//功能：认证请求
//输入：无
//输出：无
//返回值：无
//备注：下一个状态为空闲
***************************************************************************/
static void CARDState_AuthenticationRequest(void)
{
	  static u8 count = 0;
	
    if(ACK_OK == RadioProtocol_OriginalDataUpTrans(CHANNEL_DATA_2, 0x0B, CardAuthenticationRequest, 64))
    {		
        count = 0;			
	      CARDState_Switch(CARD_STATE_IDLE);
			  Public_ShowTextInfo("认证请求送成功",100);
		}
		else
		{
			  //提示发送错误
		    count++;
			  if(count >= 5)
				{
				    count = 0;
					  CARDState_Switch(CARD_STATE_IDLE);
				}
				Public_ShowTextInfo("认证请求送失败",100);
		}		
}
/**************************************************************************
//函数名：CARDState_DriverLogin
//功能：驾驶员签到
//输入：无
//输出：无
//返回值：无
//备注：下一个状态为空闲
***************************************************************************/
static void CARDState_DriverLogin(void)
{
    static u8 count = 0;
	
	  u8 Buffer[150];
	  u8 BufferLen;
	
	  Buffer[0] = 0x01;//上班
	  VDRPub_ConvertNowTimeToBCD(Buffer+1);//时间，6字节BCD
	  Buffer[7] = 0x00;//IC卡读卡结果
	  memcpy(Buffer+8,CardDriverInfo,CardDriverInfoLen);//IC卡信息
	  BufferLen = CardDriverInfoLen+8;
	
    if(ACK_OK == RadioProtocol_DriverInformationReport(CHANNEL_DATA_1,Buffer,BufferLen))
    {		
        count = 0;		
        LoginFlag = 3;		
        CardAuthenticationLinkFlag = 0;		
        CardDriverWorkFlag = 1;		
        memcpy(CardDriverWorkStartTime,Buffer,8);			
	      CARDState_Switch(CARD_STATE_IDLE);
			  Public_ShowTextInfo("驾驶员签到发送成功",100);
		}
		else
		{
			  //提示发送错误
		    count++;
			  if(count >= 5)
				{
				    count = 0;
					  CARDState_Switch(CARD_STATE_IDLE);
				}
				Public_ShowTextInfo("驾驶员签到发送失败",100);
		}
}
/**************************************************************************
//函数名：CARDState_DriverLogout
//功能：驾驶员签退
//输入：无
//输出：无
//返回值：无
//备注：下一个状态为空闲
***************************************************************************/
static void CARDState_DriverLogout(void)
{
    static u8 count = 0;
	
	  u8 Buffer[10];
	  u8 BufferLen = 7;
	
	  Buffer[0] = 0x02;//下班
	  VDRPub_ConvertNowTimeToBCD(Buffer+1);//时间，6字节BCD

	
    if(ACK_OK == RadioProtocol_DriverInformationReport(CHANNEL_DATA_1,Buffer,BufferLen))
    {		
			  
        count = 0;		
        LogoutFlag = 3;		
        CardAuthenticationLinkFlag = 0;			
			  CardDriverWorkFlag = 2;
			  memcpy(CardDriverWorkEndTime,Buffer,7);	
	      CARDState_Switch(CARD_STATE_IDLE);
			  Public_ShowTextInfo("驾驶员签退发送成功",100);
			
		}
		else
		{
		    count++;
			  if(count >= 5)
				{
				    count = 0;
					  CARDState_Switch(CARD_STATE_IDLE);
				}
				Public_ShowTextInfo("驾驶员签退发送失败",100);
		}
}
/**************************************************************************
//函数名：Card_SendCmd
//功能：发送指令到IC卡读卡器
//输入：无
//输出：无
//返回值：0表示成功，非0表示失败
//备注：无
***************************************************************************/
static u8 Card_SendCmd(u8 cmd, u8 *pBuffer , u16 BufferLen)
{
	  u8 VerifyCode = 0;
	  u8 i,j;
	  u8 length;
	
	  if(BufferLen >= 100)
		{
		    return 1;
		}
		
    if(0 == CardTxBusyFlag)
		{
			  //CardTxBuffer[0]//标识位，暂时为空
			  //CardTxBuffer[1]//校验码，暂时为空
			  CardTxBuffer[2] = (CommunicationProtocolVersion&0xff00) >> 8;//通信协议版本
			  CardTxBuffer[3] = CommunicationProtocolVersion&0xff;
			  CardTxBuffer[4] = (ManufactureID&0xff00) >> 8;;//厂商编号
			  CardTxBuffer[5] = ManufactureID&0xff;
			  CardTxBuffer[6] = PeripheralType;//外设类型
			  CardTxBuffer[7] = cmd;//命令类型
			  memcpy(CardTxBuffer+8,pBuffer,BufferLen);
			  VerifyCode = 0;
			  for(i=0,j=4; i<BufferLen+4; i++)
			  {
				    VerifyCode += CardTxBuffer[i+j];
				}
			  CardTxBuffer[1] = VerifyCode;//校验码：从厂商编号到用户数据的和校验
				
				length = TransMean(CardTxBuffer+1, BufferLen+7);
				
				CardTxBuffer[0] = 0x7e;//标识位头
				CardTxBuffer[1+length] = 0x7e;//标识位尾
				

				return COM2_WriteBuff(CardTxBuffer,2+length);
		}
		else
		{
		    return 2;
		}
}































