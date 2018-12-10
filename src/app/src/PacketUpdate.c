/************************************************************************
//�������ƣ�PacketUpdate.c
//���ܣ�ʵ�ְַ���������
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2016.4
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1���ְ��������ܣ��ù��ܽ����ڲ�����죬ʵ��Ӧ��ʱʹ��FTP�����������÷ְ���������
//1.�������̼���������ƽ̨��һ������̼��汾�뵱ǰ���еİ汾��һ����
//2.ƽ̨����ǰ��ѯ��ǰ�İ汾
//3.ƽ̨�·�������
//4.�ն˽���������������Ƿ���©��������©����©�İ���Ÿ�֪ƽ̨
//5.ƽ̨����
//6.����©��ʱ�����������ߣ������ظ�����4-5
//7.�ն�ִ������
//8.���������������ƽ̨

*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include "PacketUpdate.h"
#include "RadioProtocol.h"
#include "modem_app_first.h"
#include "other.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "taskschedule.h"

/********************���ر���*************************/
static u8 RxPacketList[1000];//�յ��İ��б���Ӧ��ֵΪ1��ʾ�յ��˸����ݰ���Ϊ0��ʾû���յ�
static u8 RxFlag = 0;//���ձ�־��2��ʾ������ɣ�1��ʾ���ڽ��շְ����ݣ�0Ϊδ���յ��ְ�����
static u16 RxTotalPacket = 0;//�ܰ���
static u32 RxTotalBytes = 0;//���ֽ���
static u8 UpdateType = 0;//��������
static u8 ManufactureID[5];//������ID
static u8 VersionLength;//�汾�ų���
static u8 Version[10];//�汾��
static u16 FirstPacketSerialNum;//��1�����к�

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/
extern MESSAGE_HEAD RadioProtocolRxHead;//���յĶ��������ݾ�������õ���֡ͷ���ڴ˽ṹ����
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

/********************���غ�������*********************/
static ProtocolACK PacketUpdate_SendPacketRequest(u16 PacketNum);
static ProtocolACK PacketUpdate_SendUpdateResultNote(u8 Result);
static ProtocolACK PacketUpdate_TerminalGeneralAck(u8 channel, u8 AckResult);

/********************��������*************************/

/**************************************************************************
//��������PacketUpdate_TimeTask
//���ܣ�ʵ�ְַ���������
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע���ְ�������ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
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
			case 0://�������ݽ׶�
			{
				  if(0 == RxFlag)//δ�յ�����
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
						                  result = 0;//1���ɹ���2��ʧ�ܣ�0��û�н�����������
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
				  else if(1 == RxFlag)//�ѽ��յ���1��
					{
					    count++;
						  if(count >= 120)//120���ǿ�ƽ�����һ�׶�
							{
							    count = 0;
								  state = 1;
							}
					}
					else if(2 == RxFlag)//�ѽ��յ����һ��
					{
						  count = 0;
					    state = 1;
					}
					else//����
					{
					
					}
			    break;
			}
			case 1://��ѯ�����İ���ÿ��ֻ��ѯ1��
			{
				  for(i=1; i<=RxTotalPacket; i++)
					{
							if(0 == RxPacketList[i])
							{
									break;
							}
				  }
					if(i == (RxTotalPacket+1))//û�в����ְ���
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
			case 2://���󲹴�1��
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
					else if(count >= 5)//�������ӵȴ�ƽ̨Ӧ��
					{
					    count = 0;
						  state = 1;
					}
				  
			    break;
			}
			case 3://����������
			{
				  count++;
				  if(1 == count)//Ҫ���Ȼظ�ƽ̨���1���������ݵ�ͨ��Ӧ��
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
			case 4://��λ��ر�־
			{
				  count++;
				  if(1 == count)
					{
				      result = 1;//1���ɹ���2��ʧ�ܣ�0��û�н�����������
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
//��������PacketUpdate_HandleCmd8108H
//���ܣ�����ƽ̨�·���0x8108ָ��
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע���ְ�������ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
void PacketUpdate_HandleCmd8108H(u8 *pBuffer, u16 BufferLen)
{
	  u16 i;
	  u8 *p=NULL;
	
	  p=pBuffer;
	
    if(1 == RadioProtocolRxHead.CurrentPackage)//��1��
		{
			  RxFlag = 1;
			
			  for(i=0; i<1000; i++)
			  {
		        RxPacketList[i] = 0;
				}
				
				RxPacketList[RadioProtocolRxHead.CurrentPackage] = 1;//��ǵ�ǰ�����յ�
				RxTotalPacket = RadioProtocolRxHead.TotalPackage;//�ܰ���
				FirstPacketSerialNum = RadioProtocolRxHead.SerialNum;//��1����ˮ��
				
				UpdateType = *p++;//��������
        memcpy(ManufactureID,p,5);//������ID
				p += 5;
        VersionLength = *p++;//�汾�ų���
				if(VersionLength < 10)
				{
					 memcpy(Version,p,VersionLength);//�汾��
				   p += 5;
				}
				RxTotalBytes = 0;//���ֽ���
				RxTotalBytes |= *p++ << 24;
				RxTotalBytes |= *p++ << 16;
				RxTotalBytes |= *p++ << 8;
				RxTotalBytes |= *p++;
				
				
		}
		else if(RadioProtocolRxHead.CurrentPackage == RxTotalPacket)
		{
		     RxPacketList[RadioProtocolRxHead.CurrentPackage] = 1;//��ǵ�ǰ�����յ�
			   RxFlag = 2;//��ǽ������
		}
		else//�м��
		{
		     RxPacketList[RadioProtocolRxHead.CurrentPackage] = 1;//��ǵ�ǰ�����յ�
		}
}
/**************************************************************************
//��������PacketUpdate_SendPacketRequest
//���ܣ����ͷְ�����
//���룺��
//�������
//����ֵ����
//��ע����ʱֻ��һ��ֻ����1����������Կ���һ������10���Լӿ��ٶ�
***************************************************************************/
static ProtocolACK PacketUpdate_SendPacketRequest(u16 PacketNum)
{
    u8 Buffer[50];
	  u8 BufferLen;
	
	  if(0 == PacketNum)
		{
		    //ԭʼ��Ϣ��1������ˮ��
	      BufferLen = 0;
	      Buffer[0] = (FirstPacketSerialNum&0xff00) >> 8;
	      Buffer[1] = FirstPacketSerialNum&0xff;
	      BufferLen += 2;
	      //�ش�������
	      Buffer[2] = 0;
	      BufferLen++;
	      //����
	      return RadioProtocol_ResendPacketRequest(CHANNEL_DATA_1,Buffer,BufferLen);
		}
		else
		{
	      //ԭʼ��Ϣ��1������ˮ��
	      BufferLen = 0;
	      Buffer[0] = (FirstPacketSerialNum&0xff00) >> 8;
	      Buffer[1] = FirstPacketSerialNum&0xff;
	      BufferLen += 2;
	      //�ش�������
	      Buffer[2] = 1;
	      BufferLen++;
	      //�ش����б�
	      Buffer[3] = (PacketNum&0xff00) >> 8;
	      Buffer[4] = PacketNum&0xff;
	      BufferLen += 2;
	      //����
	      return RadioProtocol_ResendPacketRequest(CHANNEL_DATA_1,Buffer,BufferLen);
		}
}
/**************************************************************************
//��������PacketUpdate_SendUpdateResultNote
//���ܣ������������֪ͨ
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
static ProtocolACK PacketUpdate_SendUpdateResultNote(u8 Result)
{
    u8 Buffer[10];
	  u8 BufferLen;
	
	  //��������
	  Buffer[0] = UpdateType;
    Buffer[1] = Result;

	  BufferLen = 2;
	  //����
	  return RadioProtocol_FirmwareUpdataResultNote(CHANNEL_DATA_1,Buffer,BufferLen);

}
/*********************************************************************
//��������  :PacketUpdate_TerminalGeneralAck(u16 SerialNum, u16 MessageID, u8 Result)
//����      :�ն�ͨ��Ӧ��
//����      :Result:Ӧ����,0Ϊ�ɹ���1Ϊʧ�ܣ�2��Ϣ����3��֧��
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :�ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0001
*********************************************************************/
static ProtocolACK PacketUpdate_TerminalGeneralAck(u8 channel, u8 AckResult)
{

    u8  Buffer[5];
    u8  length;
    u8  SmsPhone[20];
	  u16 SerialNum;
    MESSAGE_HEAD    head;
	
	  RadioProtocol_GetDownloadCmdChannel(0x8108, &SerialNum);
    
    //��ϢID
    head.MessageID = 0x0001;
    //��Ϣ����
    length = 5;
    head.Attribute = length;
	
	
    //��Ϣ����
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




























