/************************************************************************
//�������ƣ�Card.c
//���ܣ�ʵ�ֲ����ʻԱ���ʶ����
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2016.4
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1����ʻԱ���ʶ����
//1.�������ܣ�
//��1��Ҫʵ�ּ�ʻԱ���ʶ���ܣ��������ר�õ�IC����������Ŀǰʹ�õ����۵۶�����EMP7500��
//��2��ʹ������2������ָ���������������ӵ�IC����֤ƽ̨����ɼ�ʻԱ�����֤��ʱ30���Ӻ��л�����ʹ�õ�2������
//��3����IC����ʻԱ�����֤����������IC��ǩ��ǩ�˹�����������ͬ�Ĺ��ܣ�IC������Ҳ��ͬ����Ҫ����
//��4��IC�����������������ӵĽӿ���16PIN�ϴ��ڣ��뵼�����ǹ���һ���ӿڣ�����ֻ��ѡ��һ�������˵������Ͳ��ܽ�IC��������
//��5��IC��������������ͨ�ŵ�Э���Ǳ�׼�Ĳ�������ͨ��Э�飬������Э�鸽¼A
//��6��������IC����֤ƽ̨��ͨ��Э���Ǳ�׼�Ĳ���͸��ָ���Ϣ������0x0B
//��7��ʹ���۵۶�����ʵ�ֵļ�ʻԱǩ����ǩ��ֻ����ƽ̨������ʻ��¼�ǵ�ǩ��ǩ�����κι���
//��������Ӧ���У����������ڸü���IC�����ߺ͸�ʽ����ȷ������û����������
//2.���ܽ��ܣ�
//��1������IC����֤������IP��ַ��������TCP�˿ںš�UDP�˿ںţ����ʱ�ɼ��ƽ̨�·���Щ��������������
//��2������IC����IC��������
//��3��IC�����������Ϳ�Ƭ��֤����ָ��0x40H�������������жϵ�ǰ�Ƿ����ߣ����Ƿ���IC����֤ƽ̨�������ˣ�
//��4�������������ߣ�����Ӧ����������������ߣ�����������IC����֤��������ע������IC������������Ҫ����ע����Ȩָ�
//��5�����������ߣ����轫��Ƭ��֤����ָ��͸����͸��ָ��0x0900����IC����֤ƽ̨
//��6��IC����֤ƽ̨��׼��Ƭ��֤����󣬻��·���Ƭ��֤Ӧ�������յ��󽫿�Ƭ��֤Ӧ��͸����IC��������
//��7��IC���������յ���Ƭ��֤����ɹ��󣬶�ȡIC����Ϣ��������IC����ȡ���ָ֪ͨ��0x41H������
//��8�������ж�IC����ȡ����ǳɹ�����ʧ�ܣ����ɹ�����IC����Ϣ�ϱ����ϱ�ָ��Ϊ0x0702����IC����֤ƽ̨����ʧ�ܣ��������������Ѷ���ʧ��
//���棨1��--��8���Ǽ�ʻԱ���ʶ���ϱ�ƽ̨��ʻԱǩ�������̣��ڣ�9�����Ժ��Ǽ�ʻԱǩ������
//��9����IC����IC���������аγ�
//��10��IC�����������Ͱο�ָ֪ͨ��0x42H�������յ����ͼ�ʻԱǩ��ָ�0x0702����IC����֤ƽ̨
*************************************************************************/

/********************�ļ�����*************************/
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

/********************���ر���*************************/
static u8 state = 0;
static u8 CardInsert = 0;//0��ʾ��δ���룻1��ʾ��������
static u8 LastCardInsert = 0;
static u8 LoginFlag = 0;//1��ʾ��ǩ������2��ʾǩ���У�3��ʾǩ����ɣ�����ֵ�޺���
static u8 LogoutFlag = 0;//1��ʾ��ǩ������2��ʾǩ���У�3��ʾǩ����ɣ�����ֵ�޺���
static u8 CardRxBuffer[100];//IC�����ջ���
//static u8 CardRxBusyFlag = 0;
static u8 CardTxBuffer[100];//IC�����ͻ���
static u8 CardTxBusyFlag = 0;
static u8 CardAuthenticationRequest[70] = {0};//64�ֽڵ���֤����
static u8 CardAuthenticationAck[30] = {0};//24�ֽڵ���֤Ӧ������

static u8 CardAuthenticationLinkFlag = 0;//IC����֤���ӱ�־��1��ʾ��ǰΪIC����֤���ӣ�0��ʾ����Ӧ�õ�����
static u8 LastCardAuthenticationLinkFlag = 0;
static u16 CommunicationProtocolVersion = 0;//ͨ��Э��汾
static u16 ManufactureID = 0;//���̱��
static u8 PeripheralType = 0;//��������
static u8 UsartCardRxBuffer[1000] = {0};
static u16 UsartCardRxCount = 0;

static u32 CmdRecCount = 0;

/********************ȫ�ֱ���*************************/
u8 CardDriverWorkFlag = 0;//��ʻԱ�ϰ��־��1Ϊ�ϰ࣬2Ϊ�°࣬0Ϊδǩ����
u8 CardDriverInfo[100] = {0};//};//��ʻԱ����ϱ���Ϣ�壨0x0702���еļ�ʻԱ�������ȡ���ʻԱ��������ҵ�ʸ�֤���롢��֤�������Ƴ��ȡ���֤�������ơ�֤����Ч��
u8 CardDriverInfoLen = 0;
u8 CardDriverWorkStartTime[10] = {0};//��ʻԱ����ϱ���Ϣ�壨0x0702���е�״̬(�̶�Ϊ1)��ʱ�䣬IC����ȡ�������8�ֽ�
u8 CardDriverWorkEndTime[10] = {0};//��ʻԱ����ϱ���Ϣ�壨0x0702���е�״̬(�̶�Ϊ2)��ʱ�䣬��7�ֽ�

/********************�ⲿ����*************************/
extern u8 HexToAscBuffer[];
/********************���غ�������*********************/
static void CARDState_Idle(void);
static void CARDState_ConnectNet(void);
static void CARDState_AuthenticationRequest(void);
static void CARDState_DriverLogin(void);
static void CARDState_DriverLogout(void);
static u8 Card_SendCmd(u8 cmd, u8 *pBuffer , u16 BufferLen);
static void Card_Init(void);
static void CARDState_Switch(u8 SwitchState);

/********************��������*************************/

/**************************************************************************
//��������Card_TimeTask
//���ܣ������ʻԱ���ʶ����
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע������һ��ʱ����50ms���ӽ���1�Σ������������Ҫ���ô˺���
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
			case CARD_STATE_IDLE://����״̬
			{
				CARDState_Idle();
			  break;
			}
			case CARD_STATE_CONNECT_NET://����֤��������
			{
				CARDState_ConnectNet();
			  break;
			}
			case CARD_STATE_AUTHENTICATION_REQUEST://��֤����
			{
				CARDState_AuthenticationRequest();
			  break;
			}
			case CARD_STATE_DRIVER_LOGIN://��ʻԱǩ��
			{
				CARDState_DriverLogin();
			  break;
			}
			case CARD_STATE_DRIVER_LOGOUT://��ʻԱǩ��
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
		if(1 == flag)//30������û�����ǩ����ǩ�ˣ����³�ʼ��ͨ��ģ��
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
//��������Card_GetLinkFlag
//���ܣ���ȡIC����֤���ӱ�־
//���룺��
//�������
//����ֵ��1����ʾIC�����ӣ�0������IC������
//��ע����ΪIC������ʱ��������ע���Ȩ����
***************************************************************************/
u8 Card_GetLinkFlag(void)
{
    return CardAuthenticationLinkFlag;
	  //return 1;
}
/**************************************************************************
//��������Card_AuthenticationAck
//���ܣ���֤Ӧ��
//���룺��
//�������
//����ֵ����
//��ע����һ��״̬Ϊ����
***************************************************************************/
void Card_AuthenticationAck(u8 *pBuffer , u16 BufferLen)
{
	  u16 length;
		
	  if(24 == BufferLen)
		{
        memcpy(CardAuthenticationAck+1,pBuffer,BufferLen);
			  CardAuthenticationAck[0] = 0;
			  length = BufferLen+1;
			  Card_SendCmd(0x40,CardAuthenticationAck,length);//����ָ�IC��������
		}
}
/**************************************************************************
//��������Card_ParseCmd
//���ܣ�����IC����������ָ��
//���룺��
//�������
//����ֵ����
//��ע����
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
		    if(0x7e == *(p+i))//�ҵ���ʶλͷ
				{
					  count++;
					  if(1 == count)//��1��7E
						{
						    j = i;
						}
						else if(2 == count)//��2��7E
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
			  m = unTransMean(p+j+1, k-j-1);//j+1:ȥ��֡ͷ;k-j-1:ȥ��֡β
			  VerifyCode = 0;
			  for(n=0; n<m-3; n++)
			  {
				    VerifyCode += *(p+j+4+n);
				}
				if((VerifyCode == *(p+j+1))&&(0x0B == *(p+j+6)))//У������������;���ȷ
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
							case 0x40://�忨
							{
								  CmdRecCount = 0;
								  if(0 == *(p+j+8))//�����ɹ�
									{
									    memcpy(CardAuthenticationRequest,p+j+9,64);
                      Public_ShowTextInfo("IC��ʶ��ɹ�",100);
									    LastCardInsert = 0;
											CardInsert = 1;
										  Buffer[0] = 0x01;
										  //Card_SendCmd(0x40,Buffer,1);//����ָ�IC������������֪�������ն˲�����
									}
									else 
									{
									    if(1 == *(p+j+8))//IC��δ����
									    {
										      Public_ShowTextInfo("IC��δ����",100);
									    }
									    else if(2 == *(p+j+8))//IC������ʧ��
									    {
									        Public_ShowTextInfo("IC��ʶ��ʧ��",100);
									    }
									    else if(3 == *(p+j+8))//��IC����ҵ�ʸ�֤
									    {
									        Public_ShowTextInfo("��IC����ҵ�ʸ�֤",100);
									    }
									    else if(4 == *(p+j+8))//IC��������
									    {
									        Public_ShowTextInfo("IC��������",100);
									    }
									    else
									    {
									         Public_ShowTextInfo("ʶ��δ֪����",100);
									    }
									    CardInsert = 0;
											LastCardInsert = 0;
										  LoginFlag = 0;
										  LogoutFlag = 0;
									}
							    break;
							}
							case 0x41://����
							{
								  CmdRecCount++;
								  if(CmdRecCount > 1)
									{
									    CmdRecCount = 0;
									}
								  if(0 == *(p+j+8))//�����ɹ�
									{
									    memcpy(CardDriverInfo,p+j+9,m-8);
										  CardDriverInfoLen = m-8;										  
                      Public_ShowTextInfo("IC�������ɹ�",100);
										  CARDState_Switch(CARD_STATE_DRIVER_LOGIN);
										  
										  //Card_SendCmd(0x41,Buffer,0);
									}
									else 
									{
									    if(1 == *(p+j+8))//��Ƭ��Կ��֤δͨ��
									    {
										      Public_ShowTextInfo("��Ƭ��Կ��֤δͨ��",100);
									    }
									    else if(2 == *(p+j+8))//��Ƭ�ѱ�����
									    {
									        Public_ShowTextInfo("��Ƭ�ѱ�����",100);
									    }
									    else if(3 == *(p+j+8))//������ϢΪ��
									    {
									        Public_ShowTextInfo("������ϢΪ��",100);
									    }
									    else
									    {
									         Public_ShowTextInfo("����δ֪����",100);
									    }
									    CardInsert = 0;
											LastCardInsert = 0;
										  LoginFlag = 0;
										  LogoutFlag = 0;
											//Card_SendCmd(0x41,Buffer,0);
									}
									
							    break;
							}
							case 0x42://�ο�
							{
								  if((1 == CardInsert)&&(3 == LoginFlag))
									{
									    CardInsert = 0;
										  Card_SendCmd(0x42,Buffer,0);
										  Public_ShowTextInfo("�ο�����ʼǩ������",100);
									}
								  else
									{
										  CardInsert = 0;
										  LastCardInsert = 0;
										  LoginFlag = 0;
										  LogoutFlag = 0;
									    Public_ShowTextInfo("ǩ��δ��ɰο���Ч",100);
									}
							    break;
							}
							case 0x43://�Զ�����40Hָ�����¶���
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
** ��������: Card_GetExternalDriverInfo
** ��������: ��ȡ��ʻԱ����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Card_GetExternalDriverName(unsigned char *pBuffer)
{
	unsigned char DriverNameLen;

	DriverNameLen = CardDriverInfo[8];
	memcpy(pBuffer,CardDriverInfo+9,DriverNameLen);//��������

	return DriverNameLen;
}

/**************************************************************************
//��������CARDState_Switch
//���ܣ�״̬�л�
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
static void CARDState_Switch(u8 SwitchState)
{
    state = SwitchState;
}
/**************************************************************************
//��������Card_Init
//���ܣ���ʼ��
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
static void Card_Init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin_11);//������Դ
	
	  CardInsert = 0;
		LastCardInsert = 0;
		LoginFlag = 0;
		LogoutFlag = 0;
	  CardAuthenticationLinkFlag = 0;
	  LastCardAuthenticationLinkFlag = 0;
}
/**************************************************************************
//��������CARDState_Idle
//���ܣ�����״̬������Ƿ���������Ҫ������ת��CARD_STATE_CONNECT_NET״̬
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
static void CARDState_Idle(void)
{

	  static u32 unLinkCount = 0;
	  static u32 LinkCount = 0;
	
	  u8 Link;
	
	  Link = Modem_State_SocSta(1);
	
	  if((1 == CardInsert)&&(0 == LastCardInsert))//������
		{
			  LoginFlag = 1;
			  LogoutFlag = 0;
				unLinkCount = 0;
			  CARDState_Switch(CARD_STATE_CONNECT_NET);
		}
		else if((0 == CardInsert)&&(1 == LastCardInsert))//���γ�
		{
		    LoginFlag = 0;
			  LogoutFlag = 1;
				unLinkCount = 0;
			  CARDState_Switch(CARD_STATE_CONNECT_NET);
		}
		else if((1 == CardInsert)&&(0 != LoginFlag)&&(LoginFlag < 3))//����ʱ��ʱ����Ƿ����ߣ������ǩ��ʱLoginFlag==3�������ǩ��ʱLogoutFlag==3
		{
			  if(0 == Link)
				{
					  LinkCount = 0;
					  unLinkCount++;
					  if(unLinkCount >= 1200)//60��
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
		else if((0 == CardInsert)&&(0 != LogoutFlag)&&(LogoutFlag < 3))//�޿�ʱ
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
//��������CARDState_ConnectNet
//���ܣ�����
//���룺��
//�������
//����ֵ����
//��ע����һ��״̬Ϊ����
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
			      Public_ShowTextInfo("IC��������IP����",100);
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
			      Public_ShowTextInfo("IC��������TCP����",100);
			      return ;
		    }
    
        strcpy((char *)&ip_value.mode,"TCP");
		
        Net_Second_ConTo(ip_value, 10);//ǿ������ʱ��Ϊ10����
				*/
		
		    CardAuthenticationLinkFlag = 1;
		
		    CARDState_Switch(CARD_STATE_IDLE);
}
/**************************************************************************
//��������CARDState_AuthenticationRequest
//���ܣ���֤����
//���룺��
//�������
//����ֵ����
//��ע����һ��״̬Ϊ����
***************************************************************************/
static void CARDState_AuthenticationRequest(void)
{
	  static u8 count = 0;
	
    if(ACK_OK == RadioProtocol_OriginalDataUpTrans(CHANNEL_DATA_2, 0x0B, CardAuthenticationRequest, 64))
    {		
        count = 0;			
	      CARDState_Switch(CARD_STATE_IDLE);
			  Public_ShowTextInfo("��֤�����ͳɹ�",100);
		}
		else
		{
			  //��ʾ���ʹ���
		    count++;
			  if(count >= 5)
				{
				    count = 0;
					  CARDState_Switch(CARD_STATE_IDLE);
				}
				Public_ShowTextInfo("��֤������ʧ��",100);
		}		
}
/**************************************************************************
//��������CARDState_DriverLogin
//���ܣ���ʻԱǩ��
//���룺��
//�������
//����ֵ����
//��ע����һ��״̬Ϊ����
***************************************************************************/
static void CARDState_DriverLogin(void)
{
    static u8 count = 0;
	
	  u8 Buffer[150];
	  u8 BufferLen;
	
	  Buffer[0] = 0x01;//�ϰ�
	  VDRPub_ConvertNowTimeToBCD(Buffer+1);//ʱ�䣬6�ֽ�BCD
	  Buffer[7] = 0x00;//IC���������
	  memcpy(Buffer+8,CardDriverInfo,CardDriverInfoLen);//IC����Ϣ
	  BufferLen = CardDriverInfoLen+8;
	
    if(ACK_OK == RadioProtocol_DriverInformationReport(CHANNEL_DATA_1,Buffer,BufferLen))
    {		
        count = 0;		
        LoginFlag = 3;		
        CardAuthenticationLinkFlag = 0;		
        CardDriverWorkFlag = 1;		
        memcpy(CardDriverWorkStartTime,Buffer,8);			
	      CARDState_Switch(CARD_STATE_IDLE);
			  Public_ShowTextInfo("��ʻԱǩ�����ͳɹ�",100);
		}
		else
		{
			  //��ʾ���ʹ���
		    count++;
			  if(count >= 5)
				{
				    count = 0;
					  CARDState_Switch(CARD_STATE_IDLE);
				}
				Public_ShowTextInfo("��ʻԱǩ������ʧ��",100);
		}
}
/**************************************************************************
//��������CARDState_DriverLogout
//���ܣ���ʻԱǩ��
//���룺��
//�������
//����ֵ����
//��ע����һ��״̬Ϊ����
***************************************************************************/
static void CARDState_DriverLogout(void)
{
    static u8 count = 0;
	
	  u8 Buffer[10];
	  u8 BufferLen = 7;
	
	  Buffer[0] = 0x02;//�°�
	  VDRPub_ConvertNowTimeToBCD(Buffer+1);//ʱ�䣬6�ֽ�BCD

	
    if(ACK_OK == RadioProtocol_DriverInformationReport(CHANNEL_DATA_1,Buffer,BufferLen))
    {		
			  
        count = 0;		
        LogoutFlag = 3;		
        CardAuthenticationLinkFlag = 0;			
			  CardDriverWorkFlag = 2;
			  memcpy(CardDriverWorkEndTime,Buffer,7);	
	      CARDState_Switch(CARD_STATE_IDLE);
			  Public_ShowTextInfo("��ʻԱǩ�˷��ͳɹ�",100);
			
		}
		else
		{
		    count++;
			  if(count >= 5)
				{
				    count = 0;
					  CARDState_Switch(CARD_STATE_IDLE);
				}
				Public_ShowTextInfo("��ʻԱǩ�˷���ʧ��",100);
		}
}
/**************************************************************************
//��������Card_SendCmd
//���ܣ�����ָ�IC��������
//���룺��
//�������
//����ֵ��0��ʾ�ɹ�����0��ʾʧ��
//��ע����
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
			  //CardTxBuffer[0]//��ʶλ����ʱΪ��
			  //CardTxBuffer[1]//У���룬��ʱΪ��
			  CardTxBuffer[2] = (CommunicationProtocolVersion&0xff00) >> 8;//ͨ��Э��汾
			  CardTxBuffer[3] = CommunicationProtocolVersion&0xff;
			  CardTxBuffer[4] = (ManufactureID&0xff00) >> 8;;//���̱��
			  CardTxBuffer[5] = ManufactureID&0xff;
			  CardTxBuffer[6] = PeripheralType;//��������
			  CardTxBuffer[7] = cmd;//��������
			  memcpy(CardTxBuffer+8,pBuffer,BufferLen);
			  VerifyCode = 0;
			  for(i=0,j=4; i<BufferLen+4; i++)
			  {
				    VerifyCode += CardTxBuffer[i+j];
				}
			  CardTxBuffer[1] = VerifyCode;//У���룺�ӳ��̱�ŵ��û����ݵĺ�У��
				
				length = TransMean(CardTxBuffer+1, BufferLen+7);
				
				CardTxBuffer[0] = 0x7e;//��ʶλͷ
				CardTxBuffer[1+length] = 0x7e;//��ʶλβ
				

				return COM2_WriteBuff(CardTxBuffer,2+length);
		}
		else
		{
		    return 2;
		}
}































