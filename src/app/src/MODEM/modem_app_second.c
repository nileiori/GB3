/*
********************************************************************************
*
*                                                              
*
* Filename      : modem_app_second.c
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-10-23  
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
#else
//#include  <app_cfg.h>
#include  <includes.h>
#endif

/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/
#ifdef MODEM_APP_SECOND_H
/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/

#define  EYENET_SECOND_SOC                         (1)//SOC��

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
static Modem_APP_STATE NetSecond_State;    //״̬��
static APP_TYPE_DOWN   NetSecond_TypeDown; //��������
static SOC_IPVAL       NetSecond_AppointIp;//ָ��IP
static APP_TYPE_VAL    NetSecond_IpVal;    //����

static u8  NetSecond_ConSta;//����״̬ 0Ϊ����  1Ϊ����
static u8  NetSecond_ConTry;//�������Դ���
static u32 NetSecond_ConCnt;//��ǰ�������ͳ���������
static u32 NetSecond_ConVal;//��ǰ�������ͳ���ʱ��

static u32 NetSecond_HeartVal;//����ֵ
static u32 NetSecond_HeartCnt;//����������

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
  
/**
  * @brief  ��������2��IP
  * @param  type: ��������
  * @retval ����2��ʹ�õ��ĸ�IP
  *****Author: fanqinghai
  *****data**: 2016.02.23
  */

APP_TYPE_VAL Second_IP(void)
{
    
    return NetSecond_IpVal;

}


/*
********************************************************************************
*                              FUNCTIONS
********************************************************************************
*/
/**
  * @brief  �ӻ�ȡ����
  * @param  type: ��������
  * @retval ���ز����ṹ 1->�ɹ�  0->�Ƿ�����
  */
u8 Net_Second_Value(APP_TYPE_VAL type, void *pValue)
{
    u8  tab[50]={0};
    u8  len;
    u8  result=0;
    u32 id_ip =0;
    u32 id_port =0;
    u16 temp;

    SOC_IPVAL *pstip = pValue;
    u32 *pHeart = pValue;

    if(type == VAL_IPMAIN)//��IP
    {
         id_ip   = E2_SECOND_MAIN_SERVER_IP_ID;
         id_port = E2_SECOND_MAIN_SERVER_TCP_PORT_ID;
    }
    else if(type == VAL_IPBACK)//����IP
    {
         id_ip   = E2_SECOND_BACKUP_SERVER_IP_ID;
         id_port = E2_SECOND_MAIN_SERVER_TCP_PORT_ID;
    }
    else if(type == VAL_IPTO)//ָ��IP
    {
        *pstip = NetSecond_AppointIp;
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
        sprintf((char*)pstip->port,"%d",temp);
        if(temp == 0)
        {
            result = 0;//�ڶ����Ӷ˿�Ϊ0�ǲ���
        }
    }
    if(result == 0xaa)
    {
        strcpy((char*)pstip->mode,"TCP");//ģʽ
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
void Net_Second_Login(Modem_APP_STATE state)
{
    NetSecond_ConTry = 0;
    NetSecond_ConCnt = 0;
    NetSecond_State  = state;
    
    if(Card_GetLinkFlag())//��ΪIC�������֤ʱ����ע���Ȩ����
    {
       
    }
    else
    {
        SetLink2LoginAuthorizationState(LOGIN_READ_AUTHORIZATION_CODE);
        SetTimerTask(TIME_LINK2_LOGIN, SYSTICK_1SECOND);    
    }
}
/**
  * @brief  �������ߣ���Ҫ����������־
  * @param  type: ��������
            state: ��һ��ִ�е�״̬
  * @retval None
  */
void Net_Second_Down(APP_TYPE_DOWN type, Modem_APP_STATE state)
{
    NetSecond_TypeDown = type;//ˢ������
    NetSecond_State    = state;//ˢ��״̬
    NetSecond_ConSta   = 0;//��������
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_2);//��Ȩ��־��0
    
    //////////////////////////////////////////////////////////////////////�л�IP
    if(type == DOWN_IPSW)
    {
        if((++NetSecond_ConTry) > 2)
        {
            NetSecond_ConTry = 0;
            if(NetSecond_IpVal == VAL_IPMAIN)
            {
                NetSecond_ConVal = 10*MODEM_TMIN;
                NetSecond_IpVal = VAL_IPBACK;//��IP-����ʧ��ת��-����IP
            }
            else if(NetSecond_IpVal == VAL_IPBACK)
            {
                NetSecond_IpVal = VAL_IPMAIN;//����IP-����ʧ��ת��-��IP
            }
            else if(NetSecond_IpVal == VAL_IPTO)
            {
                NetSecond_IpVal = VAL_IPMAIN;//ָ��IP-����ʧ��ת��-��IP
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
        NetSecond_IpVal = VAL_IPMAIN;//��IP
    }   
    //////////////////////////////////////////////////////////////////////ָ��IP
    else if(type == DOWN_IPTO)
    {
        NetSecond_IpVal = VAL_IPTO;//ָ��IP
    }     
}

/**
  * @brief  ���ӳ�ʼ��
  * @param  None
  * @retval None
  */
void Net_Second_Init(void)
{
    SOC_IPVAL ip;
    u32 heartval;
    
    NetSecond_State  = MODEM_APP_IDLE;
    NetSecond_IpVal  = VAL_IPMAIN;//Ĭ��������
    NetSecond_ConVal = 0;//Ĭ��һֱ����
    NetSecond_ConSta = 0;//Ĭ������
    NetSecond_HeartVal = MODEM_TMIN*5;//Ĭ��5��������

    //////////////////////////////////////////////////////////////////��Ȩ��־��
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_2);

    //////////////////////////////////////////////////////////////////////IP����
    if(Net_Second_Value(NetSecond_IpVal,&ip) == 0)
    {
        return;//δ��������IP���������
    }
    NetSecond_State = MODEM_APP_OPEN;//������
    
    ////////////////////////////////////////////////////////////////////��������
    if(Net_Second_Value(VAL_HEART,&heartval))
    {
        NetSecond_HeartVal = heartval*3*MODEM_TSEC;// 3������ֵ��ʱ
    }
    if((NetSecond_HeartVal > (MODEM_TMIN*5))||(NetSecond_HeartVal < 30))//dxl,2015.4.21,������Сֵ���޶�,��ֹ����ʱ�������ô�����������
    {
        NetSecond_HeartVal = MODEM_TMIN*5;//ǿ��תΪ5����
    }
    
}
/*
  * @brief  ���ӻص�����,ƽ̨�·�����ʱ������
  * @param  *p��ָ�������·�������
  * @param  len�����ݳ���
  * @retval None
  */
void Net_Second_Call(u8 *p,u16 len)
{
    NetSecond_ConSta   = 1;//�����߱�־
    NetSecond_HeartCnt = 0;//����������
    RadioProtocol_AddRecvDataForParse(CHANNEL_DATA_2,p,len);   
}
/*
  * @brief  ����IP����,����IPʱ������
  * @param  *pMode��ָ����������
  * @param  *pAddr��ָ��IP��ַ
  * @param  *pPort��ָ��˿ں�
  * @retval None
  */
void Net_Second_IP(u8 *pMode ,u8 *pAddr, u8 *pPort)
{ 
    SOC_IPVAL ip;

    memset((void*)&ip,0,sizeof(SOC_IPVAL));//��սṹ��
    
    if(Net_Second_Value(NetSecond_IpVal,&ip) == 0)
    {
        NetSecond_IpVal = VAL_IPMAIN;//�����Ƿ���ǿ��תΪ��������
        Net_Second_Value(NetSecond_IpVal,&ip);
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
void Net_Second_Open(void)
{
    if((NetSecond_State == MODEM_APP_OPEN))//���ڴ�����
    {
        return;
    }
    NetSecond_State = MODEM_APP_OPEN;
}
/*
  * @brief  �ر�����
  * @param  None
  * @retval None
  */
void Net_Second_Close(void)
{
    if(NetSecond_State != MODEM_APP_CLOSE)
    {
        NetSecond_State = MODEM_APP_CLOSE;
    }
}
/*
  * @brief  ���ӵ�ָ��IP
  * @param  ip_value: ip����;
            time:��Ч��ֹʱ��
  * @retval None
  */
void Net_Second_ConTo(SOC_IPVAL ip_value, u16 time)
{
    NetSecond_AppointIp = ip_value;//IP����
    NetSecond_ConVal    = MODEM_TMIN*time;//�������ӵ�ʱ�� 
    NetSecond_State     = MODEM_APP_DOWN;//��ǰ����ʱ���ѵ�
    NetSecond_TypeDown  = DOWN_IPTO;//�������ӵ�ָ��IP  
}

/*
  * @brief  �������ݵ�����ƽ̨
  * @param  None
  * @retval �ɹ����س��ȣ�ʧ�ܷ���0
  */
u16  Net_Second_Send(u8 *p, u16 len)
{
    u16 sendlen=0;

    if(Modem_State_SocSta(EYENET_SECOND_SOC))
    {
        sendlen = Modem_Api_Socket_Send(EYENET_SECOND_SOC,p,len);
    }
   
    return sendlen;
}
/**
  * @brief  ���Ӽ���(���ڴ��ڸ�״̬)
  * @param  None
  * @retval None
  */
void Net_Second_Listen(void)
{
    ////////////////////////////////////////////////////////////////////��·���
    if(Modem_Api_SocSta(EYENET_SECOND_SOC) == __FALSE)
    {
        Net_Second_Down(DOWN_IPOP,MODEM_APP_OPEN);
        return;
    } 
    //////////////////////////////////////////////////////////���ݻ�ָ�����Ӽ��
    if(NetSecond_ConVal)
    {
        if((NetSecond_ConCnt++) > NetSecond_ConVal)
        {
            NetSecond_ConCnt   = 0;
            NetSecond_ConVal   = 0;
            NetSecond_State    = MODEM_APP_DOWN;//�ر����� -> ����ʱ���ѵ�
            NetSecond_TypeDown = DOWN_IPMN;//�����л�����IP
        }
    }
    ////////////////////////////////////////////////////////////////������ʱ���
    if((NetSecond_HeartCnt++) > NetSecond_HeartVal)
    {
        NetSecond_HeartCnt = 0;
        NetSecond_State    = MODEM_APP_DOWN;//�ر����� -> ������ʱ
        NetSecond_TypeDown = DOWN_IPOP;//�������´�(IP����)   
    }
    
}

/**
  * @brief  ����״̬
  * @param  None
  * @retval 0 -> ������
            1 -> ����
  */
u8 Net_Second_ConSta(void)
{
    return NetSecond_ConSta;
}

/**
  * @brief  ���Ӽ��
  * @param  None
  * @retval None
  */
void Net_Second_Check(void)
{

}


/**
  * @brief  ����ר��
  * @param  None
  * @retval None
  */
void Net_Second_Debug(void)
{
    static u8 Second_debug;
    
    if(Second_debug)
    {
        Second_debug = 0;
    }
}
/**
  * @brief  ���ӵ�ָ��������
  * @param  None
  * @retval None
  ****:��ע:��ǰû����Ӧ��ֱ�ӽӿ��������ӵ�ָ��������
  ****:����:fanqinghai 2016.02.19
  */
void Second_connect_To_Specific_Server(void)
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
        temp = (tab[0] << 8) |tab[1];
        sprintf((char*)ip_value.port,"%d",temp);
    }
    EepromPram_ReadPram(E2_UPDATA_TIME_ID, tab);
    time = (tab[0] << 8)|tab[1];
    strcpy((char *)&ip_value.mode,"TCP");

    Net_Second_ConTo(ip_value, time);
}


/**

  ****:����:��ָ����ip�л�����ip
  ****:����:fanqinghai 2016.02.19
  */
void Second_Switch_Net_Specific_To_MainIP(void)
{

    NetSecond_State = MODEM_APP_DOWN;   //��ǰָ����������
    Net_Second_Down(DOWN_IPSW,MODEM_APP_OPEN);
}


/**
  * @brief  ���Ӵ���
  * @param  None
  * @retval None
  */
void Net_Second_Run(void)
{

    MODERR_TYPE modemack;

    Net_Second_Debug();

    Net_Second_Check();
    
    switch(NetSecond_State)
    {
        case MODEM_APP_IDLE:
            {
                ;
            }
        break; 
        case MODEM_APP_OPEN://����һ������
            {
                modemack = Modem_Api_Socket_Open(EYENET_SECOND_SOC,
                                                 Net_Second_IP,
                                                 Net_Second_Call);
                if(modemack == MOD_OK)
                {
                    Net_Second_Login(MODEM_APP_LISTEN);//�ɹ�->�������
                }
                else if(modemack == MOD_ER)
                {
                    Net_Second_Down(DOWN_IPSW,MODEM_APP_OPEN);//ʧ��->�л�IP
                }
            }
        break;     
        case MODEM_APP_LISTEN://����һ������,���ڴ��ڸ�״̬
            {
                Net_Second_Listen();
            }
        break;  						
        case MODEM_APP_CLOSE://�ر�һ������
        case MODEM_APP_DOWN://��������
            {
                modemack = Modem_Api_Socket_Close(EYENET_SECOND_SOC);
                if(modemack == MOD_OK)
                {
									  if(MODEM_APP_DOWN==NetSecond_State)
										{
											NetSecond_State = MODEM_APP_OPEN;
										}
										else
										{
											NetSecond_State=MODEM_APP_IDLE;
										}
                    Net_Second_Down(NetSecond_TypeDown,NetSecond_State);
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







