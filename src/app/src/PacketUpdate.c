/************************************************************************
//程序名称：PacketUpdate.c
//功能：实现分包升级功能
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2016.4
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：分包升级功能（该功能仅用于部标过检，实际应用时使用FTP升级，不采用分包升级）：
//1.将升级固件给到升级平台（一般这个固件版本与当前运行的版本不一样）
//2.平台升级前查询当前的版本
//3.平台下发升级包
//4.终端接收升级，并检查是否有漏包，若有漏包把漏的包序号告知平台
//5.平台补包
//6.当无漏包时，流程往下走；否则重复步骤4-5
//7.终端执行升级
//8.把升级结果反馈给平台

*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include "PacketUpdate.h"
#include "RadioProtocol.h"
#include "modem_app_first.h"
#include "other.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "taskschedule.h"

/********************本地变量*************************/
static u8 RxPacketList[1000];//收到的包列表，对应的值为1表示收到了该数据包，为0表示没有收到
static u8 RxFlag = 0;//接收标志，2表示接收完成，1表示正在接收分包数据，0为未接收到分包数据
static u16 RxTotalPacket = 0;//总包数
static u32 RxTotalBytes = 0;//总字节数
static u8 UpdateType = 0;//升级类型
static u8 ManufactureID[5];//制造商ID
static u8 VersionLength;//版本号长度
static u8 Version[10];//版本号
static u16 FirstPacketSerialNum;//第1包序列号

/********************全局变量*************************/


/********************外部变量*************************/
extern MESSAGE_HEAD RadioProtocolRxHead;//接收的二进制数据经解析后得到的帧头放在此结构体中
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式

/********************本地函数声明*********************/
static ProtocolACK PacketUpdate_SendPacketRequest(u16 PacketNum);
static ProtocolACK PacketUpdate_SendUpdateResultNote(u8 Result);
static ProtocolACK PacketUpdate_TerminalGeneralAck(u8 channel, u8 AckResult);

/********************函数定义*************************/

/**************************************************************************
//函数名：PacketUpdate_TimeTask
//功能：实现分包升级功能
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：分包升级定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState PacketUpdate_TimeTask(void)
{
	  static u8 state = 0;
	  static u16 PacketNum = 0;
	  static u16 count = 0;
	  static u16 SendCount = 0;
	
	  u8 result;
	  u8 Buffer[10];
	  u8 BufferLen;
	  u16 i;
	
	  if(0 == BBGNTestFlag)
		{
		    return ENABLE;
		}
		
		switch(state)
		{
			case 0://接收数据阶段
			{
				  if(0 == RxFlag)//未收到数据
					{
						  if(0x02 == (0x02&GetTerminalAuthorizationFlag()))
				      {
								  SendCount++;
								  if(20 == SendCount)
									{
			                BufferLen = FRAM_BufferRead(Buffer, FRAM_PACKET_UPDATE_RESULT_LEN, FRAM_PACKET_UPDATE_RESULT_ADDR);
			                if((BufferLen == FRAM_PACKET_UPDATE_RESULT_LEN)&&(Buffer[0] != 0))
				              {
												  result = Buffer[0] - 1;
				                  if(ACK_OK == PacketUpdate_SendUpdateResultNote(result))
						              {
						                  result = 0;//1：成功；2：失败；0：没有进行升级操作
                              FRAM_BufferWrite(FRAM_PACKET_UPDATE_RESULT_ADDR, &result, FRAM_PACKET_UPDATE_RESULT_LEN);
														  
						              }
                          else
                          {
													    SendCount--;
													}														
				              }
											else
											{
												
											}
								  }
			        }
							else
							{
							    SendCount = 0;
							}
							count = 0;
					}
				  else if(1 == RxFlag)//已接收到第1包
					{
					    count++;
						  if(count >= 120)//120秒后强制进入下一阶段
							{
							    count = 0;
								  state = 1;
							}
					}
					else if(2 == RxFlag)//已接收到最后一包
					{
						  count = 0;
					    state = 1;
					}
					else//错误
					{
					
					}
			    break;
			}
			case 1://查询补传的包，每次只查询1包
			{
				  for(i=1; i<=RxTotalPacket; i++)
					{
							if(0 == RxPacketList[i])
							{
									break;
							}
				  }
					if(i == (RxTotalPacket+1))//没有补传分包了
					{
						  count = 0;
					    state = 3;
					}
					else
					{
					    PacketNum = i;
						  state = 2;
						  count = 0;
					}
			    break;
			}
			case 2://请求补传1包
			{
				  count++;
				  if(1 == count)
					{
					    if(ACK_OK ==PacketUpdate_SendPacketRequest(PacketNum))
					    {
								  //RxPacketList[PacketNum] = 1;
					        
					    }
					    else
					    {
						      count = 0;
					    }
					}
					else if(count >= 5)//留几秒钟等待平台应答
					{
					    count = 0;
						  state = 1;
					}
				  
			    break;
			}
			case 3://补传包结束
			{
				  count++;
				  if(1 == count)//要求先回复平台最后1个补传数据的通用应答
					{
					    if(ACK_OK == PacketUpdate_TerminalGeneralAck(CHANNEL_DATA_1,0))
							{
							
							}
							else
							{
							    count--;
							}
					}
					else if(count >= 3)
					{
				      if(ACK_OK ==PacketUpdate_SendPacketRequest(0))
					    {
						      count = 0;
					        state = 4;
					    }
					    else
					    {
					        count--;
					    }
				  }
			    break;
			}
			case 4://置位相关标志
			{
				  count++;
				  if(1 == count)
					{
				      result = 1;//1：成功；2：失败；0：没有进行升级操作
              FRAM_BufferWrite(FRAM_PACKET_UPDATE_RESULT_ADDR, &result, FRAM_PACKET_UPDATE_RESULT_LEN);
						  Net_First_Close();
					}
					else if(count >= 10)
					{
					    NVIC_SystemReset();
					}
			    break;
			}
			default :
			{
			   break;
			}
		}

    return ENABLE;      
}
/**************************************************************************
//函数名：PacketUpdate_HandleCmd8108H
//功能：处理平台下发的0x8108指令
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：分包升级定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
void PacketUpdate_HandleCmd8108H(u8 *pBuffer, u16 BufferLen)
{
	  u16 i;
	  u8 *p=NULL;
	
	  p=pBuffer;
	
    if(1 == RadioProtocolRxHead.CurrentPackage)//第1包
		{
			  RxFlag = 1;
			
			  for(i=0; i<1000; i++)
			  {
		        RxPacketList[i] = 0;
				}
				
				RxPacketList[RadioProtocolRxHead.CurrentPackage] = 1;//标记当前包已收到
				RxTotalPacket = RadioProtocolRxHead.TotalPackage;//总包数
				FirstPacketSerialNum = RadioProtocolRxHead.SerialNum;//第1包流水号
				
				UpdateType = *p++;//升级类型
        memcpy(ManufactureID,p,5);//制造商ID
				p += 5;
        VersionLength = *p++;//版本号长度
				if(VersionLength < 10)
				{
					 memcpy(Version,p,VersionLength);//版本号
				   p += 5;
				}
				RxTotalBytes = 0;//总字节数
				RxTotalBytes |= *p++ << 24;
				RxTotalBytes |= *p++ << 16;
				RxTotalBytes |= *p++ << 8;
				RxTotalBytes |= *p++;
				
				
		}
		else if(RadioProtocolRxHead.CurrentPackage == RxTotalPacket)
		{
		     RxPacketList[RadioProtocolRxHead.CurrentPackage] = 1;//标记当前包已收到
			   RxFlag = 2;//标记接收完成
		}
		else//中间包
		{
		     RxPacketList[RadioProtocolRxHead.CurrentPackage] = 1;//标记当前包已收到
		}
}
/**************************************************************************
//函数名：PacketUpdate_SendPacketRequest
//功能：发送分包请求
//输入：无
//输出：无
//返回值：无
//备注：暂时只做一次只请求1包，后面可以考虑一次请求10包以加快速度
***************************************************************************/
static ProtocolACK PacketUpdate_SendPacketRequest(u16 PacketNum)
{
    u8 Buffer[50];
	  u8 BufferLen;
	
	  if(0 == PacketNum)
		{
		    //原始消息第1包的流水号
	      BufferLen = 0;
	      Buffer[0] = (FirstPacketSerialNum&0xff00) >> 8;
	      Buffer[1] = FirstPacketSerialNum&0xff;
	      BufferLen += 2;
	      //重传包总数
	      Buffer[2] = 0;
	      BufferLen++;
	      //发送
	      return RadioProtocol_ResendPacketRequest(CHANNEL_DATA_1,Buffer,BufferLen);
		}
		else
		{
	      //原始消息第1包的流水号
	      BufferLen = 0;
	      Buffer[0] = (FirstPacketSerialNum&0xff00) >> 8;
	      Buffer[1] = FirstPacketSerialNum&0xff;
	      BufferLen += 2;
	      //重传包总数
	      Buffer[2] = 1;
	      BufferLen++;
	      //重传包列表
	      Buffer[3] = (PacketNum&0xff00) >> 8;
	      Buffer[4] = PacketNum&0xff;
	      BufferLen += 2;
	      //发送
	      return RadioProtocol_ResendPacketRequest(CHANNEL_DATA_1,Buffer,BufferLen);
		}
}
/**************************************************************************
//函数名：PacketUpdate_SendUpdateResultNote
//功能：发送升级结果通知
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static ProtocolACK PacketUpdate_SendUpdateResultNote(u8 Result)
{
    u8 Buffer[10];
	  u8 BufferLen;
	
	  //升级类型
	  Buffer[0] = UpdateType;
    Buffer[1] = Result;

	  BufferLen = 2;
	  //发送
	  return RadioProtocol_FirmwareUpdataResultNote(CHANNEL_DATA_1,Buffer,BufferLen);

}
/*********************************************************************
//函数名称  :PacketUpdate_TerminalGeneralAck(u16 SerialNum, u16 MessageID, u8 Result)
//功能      :终端通用应答
//输入      :Result:应答结果,0为成功，1为失败，2消息有误，3不支持
//输出      :
//使用资源  :
//全局变量  :
//调用函数  :
//中断资源  :
//返回      :成功,返回ACK_OK;失败返回ACK_ERROR;
//备注      :指令ID：0x0001
*********************************************************************/
static ProtocolACK PacketUpdate_TerminalGeneralAck(u8 channel, u8 AckResult)
{

    u8  Buffer[5];
    u8  length;
    u8  SmsPhone[20];
	  u16 SerialNum;
    MESSAGE_HEAD    head;
	
	  RadioProtocol_GetDownloadCmdChannel(0x8108, &SerialNum);
    
    //消息ID
    head.MessageID = 0x0001;
    //消息属性
    length = 5;
    head.Attribute = length;
	
	
    //消息内容
    Buffer[0] = (SerialNum & 0xff00) >> 8;
    Buffer[1] = SerialNum;
    Buffer[2] = 0x81;
    Buffer[3] = 0x08;
    Buffer[4] = AckResult;
    
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        
    }
    else
    {
        return ACK_ERROR;
    }
    return ACK_OK;
}




























