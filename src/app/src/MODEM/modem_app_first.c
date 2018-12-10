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

#define  EYENET_FIRST_SOC                         (0)//SOC��

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
static Modem_APP_STATE NetFirst_State;    //״̬��
static APP_TYPE_DOWN   NetFirst_TypeDown; //��������
static SOC_IPVAL       NetFirst_AppointIp;//ָ��IP
static APP_TYPE_VAL    NetFirst_IpVal;    //����

static u8  NetFirst_ConSta;//����״̬ 0Ϊ����  1Ϊ����
static u8  NetFirst_ConTry;//�������Դ���
static u32 NetFirst_ConCnt;//��ǰ�������ͳ���������
static u32 NetFirst_ConVal;//��ǰ�������ͳ���ʱ��

static u32 NetFirst_HeartVal;//����ֵ
static u32 NetFirst_HeartCnt;//����������

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
  * @brief  ��������2��IP
  * @param  type: ��������
  * @retval ����2��ʹ�õ��ĸ�IP
  *****Author: fanqinghai
  *****data**: 2016.02.23
  */

APP_TYPE_VAL First_IP(void)
{
    
    return NetFirst_IpVal;

}


/**
  * @brief  �ӻ�ȡ����
  * @param  type: ��������
  * @retval ���ز����ṹ 1->�ɹ�  0->�Ƿ�����
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

    if(type == VAL_IPMAIN)//��IP
    {
         id_ip   = E2_MAIN_SERVER_IP_ID;
         id_port = E2_MAIN_SERVER_TCP_PORT_ID;
    }
    else if(type == VAL_IPBACK)//����IP
    {
         id_ip   = E2_BACKUP_SERVER_IP_ID;
         id_port = E2_MAIN_SERVER_TCP_PORT_ID;
    }
    else if(type == VAL_IPTO)//ָ��IP
    {
        *pstip = NetFirst_AppointIp;
        return 1;
    }
    else if(type == VAL_HEART)//����
    {
        if(EepromPram_ReadPram(E2_TERMINAL_HEARTBEAT_ID, tab) == 4)
        {
            *pHeart = (tab[0] << 24) + (tab[1] << 16) + (tab[2] << 8) + tab[3];
            return 1;//��������
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
			
			  ////////////////dxl,2017.4.27,����,��TCP�˿ں�Ϊ0ʱ��ʹ��UDP����/////////////////// 
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
        strcpy((char*)pstip->mode,"TCP");//ģʽ
        return 1;//��������
    }
		else if(result == 0xbb)//dxl,2017.4.27,����
    {
        strcpy((char*)pstip->mode,"UDP");//ģʽ
        return 1;//��������
    }
    return 0;//�����Ƿ�
}

/**
  * @brief  ���ӵ�¼����Ҫ�Ǵ���ע���Ȩ����
  * @param  type: ���� 
            state: ��һ��ִ�е�״̬
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
      * @brief  �ӻ�ȡ����
      * @param  type: ��������
      * @retval ���ز����ṹ 1->�ɹ�  0->�Ƿ�����
      * �޸���:fanqinghai 2016.01.07
     * ˵��:  ʹ��ǰ���������õ�
      */
void Net_First_Login(Modem_APP_STATE state)   
{
	  NetFirst_ConTry = 0;
    NetFirst_ConCnt = 0;
    NetFirst_State  = state;

    if(1 == Lock2_GetOpenAccountRequestFlag())   //��⿪����ɱ�־
    {
        ClrTimerTask(TIME_LINK1_LOGIN); 
        Lock2_OpenAccountOnline();		
    }
		else if(1 == MultiCenterConnect_GetRequestFlag())   //�����������ӱ�־
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
  * @brief  �������ߣ���Ҫ����������־
  * @param  type: ��������
            state: ��һ��ִ�е�״̬
  * @retval None
  */
void Net_First_Down(APP_TYPE_DOWN type, Modem_APP_STATE state)
{
    NetFirst_TypeDown = type;//ˢ������
    NetFirst_State    = state;//ˢ��״̬
    NetFirst_ConSta   = 0;//��������
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);//��Ȩ��־��0
    
    //////////////////////////////////////////////////////////////////////�л�IP
    if(type == DOWN_IPSW)
    {
        if((++NetFirst_ConTry) > 2)
        {
            NetFirst_ConTry = 0;
            if(NetFirst_IpVal == VAL_IPMAIN)
            {
                NetFirst_ConVal = 10*MODEM_TMIN;
                NetFirst_IpVal = VAL_IPBACK;//��IP-����ʧ��ת��-����IP
            }
            else if(NetFirst_IpVal == VAL_IPBACK)
            {
                NetFirst_IpVal = VAL_IPMAIN;//����IP-����ʧ��ת��-��IP
            }
            else if(NetFirst_IpVal == VAL_IPTO)
            {
                NetFirst_IpVal = VAL_IPMAIN;//ָ��IP-����ʧ��ת��-��IP
            }            
        }  
    }
    //////////////////////////////////////////////////////////////////////��ǰIP
    else if(type == DOWN_IPOP)
    {
        ;//����
    }
    ////////////////////////////////////////////////////////////////////////��IP
    else if(type == DOWN_IPMN)
    {
        NetFirst_IpVal = VAL_IPMAIN;//��IP
    }   
    //////////////////////////////////////////////////////////////////////ָ��IP
    else if(type == DOWN_IPTO)
    {
        NetFirst_IpVal = VAL_IPTO;//ָ��IP
    }     
}

/**
  * @brief  ���ӳ�ʼ��
  * @param  None
  * @retval None
  */
void Net_First_Init(void)
{
    SOC_IPVAL ip;
    u32 heartval;
    
    NetFirst_State  = MODEM_APP_IDLE;
    NetFirst_IpVal  = VAL_IPMAIN;//Ĭ��������
    NetFirst_ConVal = 0;//Ĭ��һֱ����
    NetFirst_ConSta = 0;//Ĭ������
    NetFirst_HeartVal = MODEM_TMIN*5;//Ĭ��5��������
    
    //////////////////////////////////////////////////////////////////��Ȩ��־��
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);
    
    //////////////////////////////////////////////////////////////////////IP����
    if(Net_First_Value(NetFirst_IpVal,&ip) == 0)
    {
        return;//δ��������IP���������
    }
    NetFirst_State = MODEM_APP_OPEN;//������
    
    ////////////////////////////////////////////////////////////////////��������
    if(Net_First_Value(VAL_HEART,&heartval))
    {
        NetFirst_HeartVal = heartval*3*MODEM_TSEC;// 3������ֵ��ʱ
    }
    if((NetFirst_HeartVal > (MODEM_TMIN*5))||(NetFirst_HeartVal < 30))//dxl,2015.4.21,������Сֵ����,��ֹ�������������ô�������
    {
        NetFirst_HeartVal = MODEM_TMIN*5;//ǿ��תΪ5����
    }
    
}
/*
  * @brief  ���ӻص�����,ƽ̨�·�����ʱ������
  * @param  *p��ָ�������·�������
  * @param  len�����ݳ���
  * @retval None
  */
void Net_First_Call(u8 *p,u16 len)
{
    NetFirst_ConSta   = 1;//�����߱�־
    NetFirst_HeartCnt = 0;//����������
    RadioProtocol_AddRecvDataForParse(CHANNEL_DATA_1,p,len);   
}
/*
  * @brief  ����IP����,����IPʱ������
  * @param  *pMode��ָ����������
  * @param  *pAddr��ָ��IP��ַ
  * @param  *pPort��ָ��˿ں�
  * @retval None
  */
void Net_First_IP(u8 *pMode ,u8 *pAddr, u8 *pPort)
{ 
    SOC_IPVAL ip;

    memset((void*)&ip,0,sizeof(SOC_IPVAL));//��սṹ��
    
    if(Net_First_Value(NetFirst_IpVal,&ip) == 0)
    {
        NetFirst_IpVal = VAL_IPMAIN;//�����Ƿ���ǿ��תΪ��������
        Net_First_Value(NetFirst_IpVal,&ip);
    }
    strcpy((char*)pMode,(char*)ip.mode); //TCP��UDP
    strcpy((char*)pAddr,(char*)ip.ip);   //IP��ַ������
    strcpy((char*)pPort,(char*)ip.port); //�˿�
}


/*
  * @brief  ������
  * @param  None
  * @retval None
  */
void Net_First_Open(void)
{
    if((NetFirst_State == MODEM_APP_OPEN))//���ڴ�����
    {
        return;
    }
    NetFirst_State = MODEM_APP_OPEN;
}
/*
  * @brief  �ر�����
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
  * @brief  ���ӵ�ָ��IP
  * @param  ip_value: ip����;
            time:��Ч��ֹʱ��
  * @retval None
  */
void Net_First_ConTo(SOC_IPVAL ip_value, u16 time)
{
    NetFirst_AppointIp = ip_value;//IP����
    NetFirst_ConVal    = MODEM_TMIN*time;//�������ӵ�ʱ�� 
    NetFirst_State     = MODEM_APP_DOWN;//��ǰ����ʱ���ѵ�
    NetFirst_TypeDown  = DOWN_IPTO;//�������ӵ�ָ��IP  
   // NetFirst_State     = MODEM_APP_OPEN;//��ǰ����ʱ���ѵ�

}

/*
  * @brief  �������ݵ�����ƽ̨
  * @param  None
  * @retval �ɹ����س��ȣ�ʧ�ܷ���0
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
  * @brief  ���Ӽ���(���ڴ��ڸ�״̬)
  * @param  None
  * @retval None
  */
void Net_First_Listen(void)
{
    ////////////////////////////////////////////////////////////////////��·���
    if(Modem_Api_SocSta(EYENET_FIRST_SOC) == __FALSE)
    {
        Net_First_Down(DOWN_IPOP,MODEM_APP_OPEN);
        return;
    } 
    //////////////////////////////////////////////////////////���ݻ�ָ�����Ӽ��
    if(NetFirst_ConVal)
    {
        if((NetFirst_ConCnt++) > NetFirst_ConVal)
        {
            NetFirst_ConCnt   = 0;
            NetFirst_ConVal   = 0;
            NetFirst_State    = MODEM_APP_DOWN;//�ر����� -> ����ʱ���ѵ�
            NetFirst_TypeDown = DOWN_IPMN;//�����л�����IP
        }
    }
    ////////////////////////////////////////////////////////////////������ʱ���
    if((NetFirst_HeartCnt++) > NetFirst_HeartVal)
    {
        NetFirst_HeartCnt = 0;
        NetFirst_State    = MODEM_APP_DOWN;//�ر����� -> ������ʱ
        NetFirst_TypeDown = DOWN_IPOP;//�������´�(IP����)   
    }
    
}

/**
  * @brief  ����״̬
  * @param  None
  * @retval 0 -> ������
            1 -> ����
  */
u8 Net_First_ConSta(void)
{
    return NetFirst_ConSta;
}

/**
  * @brief  ���Ӽ��
  * @param  None
  * @retval None
  */
void Net_First_Check(void)
{

}


/**
  * @brief  ����ר��
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
  * @brief  ���ӵ�ָ��������
  * @param  None
  * @retval None
  ****:��ע:��ǰû����Ӧ��ֱ�ӽӿ��������ӵ�ָ��������
  ****:����:fanqinghai 2016.02.19
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

  ****:����:��ָ����ip�л�����ip
  ****:����:fanqinghai 2016.02.19
  */
void First_Switch_Net_Specific_To_MainIP(void)
{

    NetFirst_State = MODEM_APP_DOWN;   //��ǰָ����������
    Net_First_Down(DOWN_IPSW,MODEM_APP_OPEN);
}

/**
  * @brief  ���Ӵ���
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
        case MODEM_APP_OPEN://����һ������
            {
                modemack = Modem_Api_Socket_Open(EYENET_FIRST_SOC,
                                                 Net_First_IP,
                                                 Net_First_Call);
                if(modemack == MOD_OK)
                {
                    Net_First_Login(MODEM_APP_LISTEN);//�ɹ�->�������
                }
                else if(modemack == MOD_ER)
                {
                    Net_First_Down(DOWN_IPSW,MODEM_APP_OPEN);//ʧ��->�л�IP
                }
            }
        break;     
        case MODEM_APP_LISTEN://����һ������,���ڴ��ڸ�״̬
            {
                Net_First_Listen();
            }
        break;        
        case MODEM_APP_CLOSE://�ر�һ������
            {   
                modemack = Modem_Api_Socket_Close(EYENET_FIRST_SOC);
                if(modemack == MOD_OK)
                {
                    NetFirst_State = MODEM_APP_IDLE;//�ȴ�Ӧ�ô�
									  ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);//dxl,2016.4.20
                }
                else if(modemack == MOD_ER)
                {
                    Modem_Api_Rst();//ģ���쳣->����ģ��
                }
            }
        break;        
        case MODEM_APP_DOWN://��������
            {
                modemack = Modem_Api_Socket_Close(EYENET_FIRST_SOC);
                if(modemack == MOD_OK)
                {
                    Net_First_Down(NetFirst_TypeDown,MODEM_APP_OPEN);
                }
                else if(modemack == MOD_ER)
                {
                    Modem_Api_Rst();//�ر�����ʧ�ܺ�����ͨѶģ��
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







