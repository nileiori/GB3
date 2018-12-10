/**
  ****************************************************************************** 
  * @file    modem_api.c
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2013-08-25
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#ifdef EYE_MODEM
#include "include.h"
#else
#include  <app_cfg.h>
#include  <includes.h>
#endif

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES   ���ر���
*********************************************************************************************************
*/
static u8  ApiPortLockFlg;//API�ӿ�������־ 0Ϊ����  1Ϊ����
static u8  ApiPortLockType;//API�ӿ������������� ��ӦModem_API_PORTö��

static u8  SocOpenWait[MODEM_SOC_NUM];//�����ӵȴ���־ ���ڵȴ���������Ӧ 1��ʾ���ڵȴ� 0Ϊ����
static u32 SocOpenOut[MODEM_SOC_NUM];//�ȴ���ʱʱ�� ��λʱ��Ϊ һ������
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES    ȫ�ֱ���
*********************************************************************************************************
*/
ST_MOD_API        modapi;//modemģ��api�ṹ��

CALL_SOC_FUNCTION     Modem_Call_Soc_Function[MODEM_SOC_NUM];//soc�ص��������飬���ڴ�����ݽ��պ���
CALL_SET_IP_FUNCTION  Modem_Call_SocIpConf_Function[MODEM_SOC_NUM];//soc ip�ص�����������socket������IP��Ϣ

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
  * @brief  API�ӿ�����
  * @param  ��ǰ��API�ӿ�����
  * @retval 0:����
  *         1:����API�ӿ����ڼ���
  */ 
u8  Modem_Api_PortLock (Modem_API_PORT api_port) 
{
    if(ApiPortLockFlg)//������
    {
        if(ApiPortLockType == api_port)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    modapi.index    = 0;
    ApiPortLockFlg  = 1;
    ApiPortLockType = api_port;
    return 0;
}
/*
  * @brief  API�ӿ��ͷ�
  * @param  ��ǰ��API�ӿ�����
  * @retval None
  */ 
void  Modem_Api_PortFree (Modem_API_PORT api_port) 
{
    if(ApiPortLockFlg)//������
    {
        ApiPortLockFlg = 0;
    }
    modapi.index = 0;
}
/*
  * @brief  API�ӿ�״̬
  * @param  None
  * @retval 0:����
  *         1:����API�ӿ����ڼ���
  */ 
u8  Modem_Api_PortIs (Modem_API_PORT api_port) 
{
    if(ApiPortLockFlg)//������
    {
        if(ApiPortLockType != api_port)
        {
            return 1;//����API�ӿ����ڼ���
        }
    }
    return 0;//����
}
/*
  * @brief  modem��ʼ������Ҫ��ʼ����������ʱ�Ķ���.
  * @param  None
  * @retval ���س�ʼ�����
  */ 
void  Modem_Api_Init (void) 
{      
#ifdef EYE_MODEM
    Net_First_Init();
    #ifdef MODEM_APP_SECOND_H
    Net_Second_Init();
    #endif
#endif
}
/*
  * @brief  modem��ʼ,����ͨѶģ���ں�,һ����ϵͳ��ʼ��ʱ���ô˺���
  * @param  None
  * @retval None
  */ 
void  Modem_Api_Start(void) 
{
    Modem_State_Switch(MODEM_BSP);
}
/*
  * @brief  modem��λ
  * @param  None
  * @retval None
  */ 
void  Modem_Api_Rst (void) 
{
    Modem_State_Switch(MODEM_ERROR);
}
/*
  * @brief  modem�ػ�
  * @param  delay :�ػ�ǰ��ʱʱ��������ƽ̨����ȷ��ָ��
  * @retval None
  */ 
void  Modem_Api_Off (u16 delay) 
{
    Modem_State_Delay(delay,MODON_OFF);
}
/*
  * @brief  modem����
  * @param  None
  * @retval None
  */  
void  Modem_Api_On (void) 
{
    Modem_State_Switch(MODEM_INIT);
}


/*
  * @brief  �������ݽ��ջص�����
  * @param  type :��������
  *
  * @param  *p :ָ����յ�����
  * @param  len:�������ݵĳ���
  * @retval None
  */
void  Modem_Api_CallSms(MODSMS_TYPE type, u8 *p, u16 len)
{
#ifdef EYE_MODEM
    Net_Other_SmsCall(type,p,len);
#endif
}
/*
  * @brief  ftp�������ݽ��ջص�����
  * @param  *p :ָ����յ�����
  * @param  len:�������ݵĳ���,��󳤶�Ϊ512
  * @retval None
  */
void  Modem_Api_CallFtp(u8 *p, u16 len)
{
    #ifdef MODEM_DEBUG_H
    if(ModDbInfType == MODDB_BASE)
    {
        MODDB_PRTFBAS("modem->Ftp_Data = %d\r\n",modftp.fsize);               
    }
    #endif   
#ifdef EYE_MODEM
    Net_Other_FtpCall(p,len);
#endif
} 

/*
  * @brief  ����ص�����,����������ص�һ��
  * @param  *p :ָ����յ�����
  * @param  len:�������ݵĳ���
  * @retval None
  */
void  Modem_Api_CallRing(u8 *p, u16 len)
{
}
/*
  * @brief  �绰״̬�ص�����
  * @param  *pPhone:ָ��绰����
  * @param  status: 0Ϊ�Ҷ�; 1:��ͨ��2:������
  * @retval ������ʱ����:  0:ֱ�Ӿܽ�����;  1:���ڵȴ��û���������
  */
u8  Modem_Api_CallPhoneStatus(u8 *pPhone,u8 status)
{
    u8 ch=0;
#if EYE_MODEM
    ch = Public_DisposePhoneStatus(pPhone,status);//֪ͨ��ʾ���Է��ѹҶ�
#else
    ch = 1;
#endif
    return ch;
}
/*
  * @brief  Ӧ�ó����ѯmodem��״̬
  * @param  Soc��
  * @retval ����modem������ҵ��
  */
MODON_STA  Modem_Api_OnSta(void)
{
    return (modem.onsta);
}
/*
  * @brief  ��ѯmodem����
  * @param  None
  * @retval 
            bit0��0����֧��GPRSͨ�ţ�     1��֧��GPRSͨ�ţ�
            bit1��0����֧��CDMAͨ�ţ�     1��֧��CDMAͨ�ţ�
            bit2��0����֧��TD-SCDMAͨ�ţ� 1��֧��TD-SCDMAͨ�ţ�
            bit3��0����֧��WCDMAͨ�ţ�    1��֧��WCDMAͨ�ţ�
            bit4��0����֧��CDMA2000ͨ�ţ� 1��֧��CDMA2000ͨ�š�
            bit5��0����֧��TD-LTEͨ�ţ�   1��֧��TD-LTEͨ�ţ�
            bit7��0����֧������ͨ�ŷ�ʽ�� 1��֧������ͨ�ŷ�ʽ��  
  */
u8  Modem_Api_Type(void)
{
    return Modem_State_Type(1);
}
/*
  * @brief  Ӧ�ò����CCID
  * @param  *p :ָ��ccid����
  * @retval None
  */
void  Modem_Api_CCID(u8 *p)
{
    memcpy(p,modem.ccid,10);
} 


/*
  * @brief  Ӧ�ó����ѯָ��soc����״̬
  * @param  Soc��
  * @retval ��������״̬,���߷����棬���򷵻ؼ�
  */
BOOL  Modem_Api_SocSta(u8 soc)
{
    return (Modem_State_SocSta(soc));
}
/**
  * @brief  �������ݵ�ָ��soc
  * @param  SocNum��soc��
  * @param  *data: ָ���͵�����
  * @param  len  : �������ݵĳ���.
  * @retval ����ʵ�ʷ������ݵĳ��ȣ�0��ʾʧ��.
  */
u16 Modem_Api_Socket_Send(u8 SocNum ,u8 *data, u16 len)
{
    u16 SendLen=0;

    if((modem.state == MODEM_ON)&&(Modem_State_SocSta(SocNum)))
    {
        if(ModLib_NodIns(&ModSocNode[SocNum],INS_ALL,data,len))
        {
            SendLen = len;
        }
    }

    return SendLen;
}
/*
  * @brief  �����Ƿ�ʹ���������ӣ���Ҫ���IP��ַ
  * @param  *pIp -> ָ��IP��ַ
  * @retval None
  */
void  Modem_Api_SocDnsSet(u8 *pIp)
{
    u8 result;

    result = ModLib_ChkIp(pIp);

    if(result == 0)
    {
        modem.socdns = 0;//IP��ַ����
    }
    else
    {
        modem.socdns = 1;//��������
    }
}

/*
  * @brief  ��ָ��soc����.ʧ��ÿ��1�����ط�һ�Σ���5��
  *          ע��!!!Ĭ����ʹ��IP���ӣ����ʹ����������Ҫ��Call_IpConf������
  *          ����Modem_Api_SocDnsSet(1)������.
  * @param SocNum:Ҫ�򿪵�soc��
  * @param  void(*Call_IpConf)( //������������soc����
                    u8 *pMode,  //ָ���������ͣ�tcp��udp
                    u8 *pAddr,  //ָ��ip��ַ
                    u8 *pPort,) //ָ��ip�Ķ˿ں�

   * @param    void(*Call_Soc)( //�����ش�soc���յ�������
                    u8 *pRec,   //ָ����յ�������
                    u16 Reclen) //�������ݵĳ���                                                   
  * @param  None
  * @retval ���ش�����
  */
MODERR_TYPE Modem_Api_Socket_Open(u8 SocNum
                            ,void(*Call_IpConf)(u8 *pMode ,u8 *pAddr, u8 *pPort)
                            ,void(*Call_Soc)(u8 *pRec, u16 Reclen))
{  
    u8  Tab_Mode[5]={0};
    u8  Tab_Addr[30]={0};
    u8  Tab_Port[7]={0};

    if(SocNum >= MODEM_SOC_NUM)
    {
        return MOD_ER;//����
    }

    if(modem.state != MODEM_ON)
    {
        return MOD_DOWN;//����״̬
    }
    
    if(modapi.wait != MODON_IDLE)
    {
        return MOD_DO;//modemæ
    }
    
    if(Modem_Api_PortIs(PORT_OPEN))
    {
        return MOD_DO;//����API�ӿ�����ʹ��
    }

    if(modem.socopen != SocNum)//����ϴ������Ƿ������
    {
        if(SocOpenWait[modem.socopen])
        {
            if((SocOpenWait[modem.socopen]&0x0f) != 0x0f)
            {
                return MOD_DO;//modemæ
            }
        }
    }
    
    if(SocOpenWait[SocNum] == 0)//��������
    {
        modapi.index = 0;
    }
    else if(SocOpenWait[SocNum] == 0x0f)//ȷ���Ѵ�����ɹ�
    {
        if(Modem_IpSock_STA[SocNum] == MODSOC_ONLN)//�����ж�
        {
            modapi.index = 0;
            SocOpenWait[SocNum] = 0;  
            Modem_PrtfTo(MODPRTF_SOCOK); 
            return MOD_OK;//�ɹ�
        }
        else if(Modem_IpSock_STA[SocNum] == MODSOC_ERR)//�����ж�
        {
            modapi.index = 0;
            SocOpenWait[SocNum] = 0;
            Modem_PrtfTo(MODPRTF_SOCOT);
            return MOD_ER;//����
        }
        
        if(SocOpenOut[SocNum] < (MODEM_TSEC*60))//��ʱ�ж� 1���ӳ�ʱʱ��
        {
            SocOpenOut[SocNum]++;
            return MOD_DO;//��æ
        }
        else
        {
            modapi.index = 0;
            SocOpenWait[SocNum] = 0;
            Modem_PrtfTo(MODPRTF_SOCOT);  
            return MOD_ER;//��ʱ
        }
    }

    Modem_Api_PortLock(PORT_OPEN);

    switch (modapi.index)
    {
        case 0:
        {
            Modem_Call_Soc_Function[SocNum] = Call_Soc;
            Modem_Call_SocIpConf_Function[SocNum] = Call_IpConf;//����soc ����ip������Ӧ�Ļص�����
            Call_IpConf(Tab_Mode,Tab_Addr,Tab_Port);//��ȡip��ַ�Ͷ˿�
            Modem_Api_SocDnsSet(Tab_Addr);
            SocOpenWait[SocNum] = 0x01;
            modem.socopen = SocNum;//Ҫ��soc��
            modapi.index++;
        }
        break;
        case 1:
        {
            SocOpenWait[SocNum] |= 0x02;
            modapi.wait = MODON_DNSIP; 
            Modem_State_SwitchOn(MODON_DNSIP);//������������
            modapi.index++;
        }
        break;
        case 2:
        {     
            SocOpenWait[SocNum] |= 0x04;
            modapi.wait = MODON_IPOPEN; 
            Modem_State_SwitchOn(MODON_IPOPEN);//���ӷ�����
            modapi.index++;
        }
        break;
        case 3://������ģ����Ӧ�ɹ�
        {     
            SocOpenWait[SocNum] |= 0x08;
            SocOpenOut[SocNum]  = 0;
            Modem_Api_PortFree(PORT_OPEN);
        }
        break;
        default://������ʧ��
        {
            SocOpenWait[SocNum] = 0;
            Modem_Api_PortFree(PORT_OPEN);
            return MOD_ER;
        }
    } 
    return (MOD_IDLE);
}
/*
  * @brief  �ر�ָ��Soc����.
  * @param  Soc��
  * @retval ���ش�����
  */
MODERR_TYPE  Modem_Api_Socket_Close(u8 SocNum)
{   

    if(SocNum >= MODEM_SOC_NUM)
    {
        return MOD_ER;//����
    }
    if(Modem_Api_PortIs(PORT_CLOSE))
    {
        return MOD_DO;//����API�ӿ�����ʹ��
    }
    if(modem.state != MODEM_ON)
    {
        return MOD_DOWN;//����״̬
    }
    if(modapi.wait != MODON_IDLE)
    {
        return MOD_DO;//modemæ
    }
    
    Modem_Api_PortLock(PORT_CLOSE);
    
    switch (modapi.index)
    {
        case 0:
        {
            modapi.index++;
            modem.socclose = SocNum;
            modapi.wait = MODON_IPCLOSE; 
            Modem_State_SwitchOn(MODON_IPCLOSE);
        }
        break;
        case 1://�ɹ�
        {     
            Modem_Api_PortFree(PORT_CLOSE);
            return MOD_OK;
        }
        default://ʧ��
        {
            Modem_Api_PortFree(PORT_CLOSE);
            return MOD_ER;
        }

    }
    return (MOD_IDLE);
}
                                            
/*
  * @brief  ��ָ�������������ļ������������Modem_Api_FtpIsOk�ж��ļ������Ƿ�����
  * @param  *pIp:ָ��IP��ַ
  * @param  *pPort:ָ��˿�
  * @param  *pUser:ָ���û���
  * @param  *pPass:ָ������
  * @param  *pFpath:ָ��·��
  * @param  *pFname:ָ���ļ���
  * @retval ���ش�����
  */
MODERR_TYPE  Modem_Api_Ftp(u8 *pIp,u8 *pPort
                                             ,u8 *pUser,u8 *pPass
                                             ,u8 *pFpath,u8 *pFname)
{
    if(modem.state != MODEM_ON)
    {
        return MOD_DOWN;//����״̬
    }
    
    if(modapi.wait != MODON_IDLE)
    {
        return MOD_DO;//modemæ
    }
    
    if(Modem_Api_PortIs(PORT_FTP))
    {
        return MOD_DO;//�����ӿ��Ѽ���
    }
    
    Modem_Api_PortLock(PORT_FTP);

    switch (modapi.index)
    {
        case 0://װ�ز���
        {
            modapi.index++;
            strcpy((char*)modftp.ip,(char*)pIp);       //ip��ַ
            strcpy((char*)modftp.port,(char*)pPort);   //�˿�
            strcpy((char*)modftp.user,(char*)pUser);   //�û���
            strcpy((char*)modftp.pass,(char*)pPass);   //����
            strcpy((char*)modftp.fpath,(char*)pFpath); //�ļ�·��
            strcpy((char*)modftp.fname,(char*)pFname); //�ļ���
            #ifdef MODEM_DEBUG_H
            if(ModDbInfType == MODDB_BASE)
            {
                MODDB_PRTFBAS("modem->Ftp_Ip   = %s\r\n",(char*)modftp.ip);
                MODDB_PRTFBAS("modem->Ftp_Port = %s\r\n",(char*)modftp.port);     
                MODDB_PRTFBAS("modem->Ftp_User = %s\r\n",(char*)modftp.user);
                MODDB_PRTFBAS("modem->Ftp_Pass = %s\r\n",(char*)modftp.pass);
                MODDB_PRTFBAS("modem->Ftp_Path = %s\r\n",(char*)modftp.fpath);
                MODDB_PRTFBAS("modem->Ftp_Name = %s\r\n\r\n",(char*)modftp.fname);                
            }
            #endif             
        }
        break;
        case 1://�ر�������·
        {
            modapi.index++;
            modapi.wait = MODON_IPCLOSE; 
            modem.socclose = MODEM_SOC_NUM;
            Modem_State_SwitchOn(MODON_IPCLOSE);
        }
        break;
        case 2://����ftp״̬
        {
            modapi.index++;  
            modapi.wait = MODON_FTP; 
            Modem_State_SwitchOn(MODON_FTP);
        }
        break;    
        case 3://�������
        {     
             Modem_Api_PortFree(PORT_FTP);
             return (MOD_OK);
        }
        default://�ر�����ʧ��
        {
            #ifdef MODEM_DEBUG_H
            if(ModDbInfType == MODDB_BASE)
            {
                MODDB_PRTFBAS("modem->Ftp_Err Please Check Value!!!\r\n");             
            }
            #endif        
             Modem_Api_PortFree(PORT_FTP);
            return MOD_ER;
        }
    }   
    return (MOD_IDLE);
}
/*
  * @brief  ftp�ļ����ؽ��
  * @param  None
  * @retval �����ļ����ؽ����0:���� 1:��ȷ 2:����
  */
u8  Modem_Api_FtpResult(void)
{
    return modftp.fres;
}
/*
  * @brief  ftp�����ļ��Ĵ�С
  * @param  None
  * @retval ����ftp�����ļ��Ĵ�С
  */
u32  Modem_Api_FtpSize(void)
{
    return modftp.fsize;
}
/*
  * @brief  ��ָ�����뷢���ֻ�����
  * @param  *pNum :ָ���ֻ�����,���ΪNULL��ʹ�ý��պ���
  * @param  *pData :ָ�������������
  * @param  len:�������ݵĳ���,һ��ֻ�ܷ���(MODEM_SMS_SENDMAX)���ֽڣ��ݲ�֧�ֶ��Ų��
  * @retval ���ش�����
  */
BOOL  Modem_Api_SmsSend(MODSMS_TYPE Type,u8 *pNum,u8 *pData, u8 len)
{
    if(modsms.send)
    {
        return (__FALSE);
    }
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);
    }
    
    return (Modem_State_Sms_Dt(Type,pNum,pData,len));

}
/*
  * @brief  ����绰
  * @param  *pData :ָ��绰����
  * @param  TypeSpeak:������״̬ 1Ϊ��������  0Ϊ�ر�
  * @retval ���ش�����
  */
BOOL  Modem_Api_Phone_Atd(u8 *pData, u8 TypeSpeak)
{
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);//����״̬
    }
    
    if(modapi.wait == MODON_IPOPEN)
    {
        Modem_State_Push();
        modphone.otherflg = 1;
    }
    else if(modapi.wait != MODON_IDLE)
    {
        return (__FALSE);//modemæ
    }

    
    if(modphone.state != MODPH_IDLE)
    {
        return (__FALSE);//modem���ڴ���绰ҵ��
    }
          
    memset(modphone.num,0,sizeof(modphone.num));
    memcpy(modphone.num,pData,ModLib_AtLen(pData)); 
    strcat((char*)modphone.num,";"); 
    
    Modem_State_SwitchPhone(MODPH_ATD);
    modapi.wait = MODON_PHONE; 
    Modem_State_SwitchOn(MODON_PHONE);

    if(TypeSpeak == 0)
    {
        Modem_Bsp_SpkOff(BSP_SPK_TALK);
    }
    else
    {
        Modem_Bsp_SpkOn(BSP_SPK_TALK);
    }

    return (__TRUE);

}


/*
  * @brief  �Ҷϵ绰
  * @param  None
  * @retval ���ش�����
  */
BOOL  Modem_Api_Phone_Ath(void)
{
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);//����״̬
    }

    memset(modphone.num,0,sizeof(modphone.num));
    Modem_State_SwitchPhone(MODPH_ATH);
    modapi.wait = MODON_PHONE; 
    Modem_State_SwitchOn(MODON_PHONE);

    if(modem.wait != MODWT_IDLE)
    {
        modem.wait = MODWT_IDLE;
    }    
    return (__TRUE);

}
/*
  * @brief  �����绰
  * @param  None
  * @retval ���ش�����
  */
BOOL  Modem_Api_Phone_Ata(void)
{
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);//����״̬
    }
    
    if(modapi.wait != MODON_PHONE)
    {
        return (__FALSE);//modem���ڵ绰״̬��
    }
    if(modphone.state != MODPH_RING)
    {
        return (__FALSE);//modem���ڴ���绰ҵ��
    }

    Modem_State_SwitchPhone(MODPH_ATA);
    
    return (__TRUE);

}

/*
  * @brief  ��������
  * @param  value :������С����ֵ
  * @retval ���ش�����
  */
BOOL  Modem_Api_Phone_Clvl(u8 value)
{
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);//����״̬
    }
    
    if(modapi.wait != MODON_PHONE)
    {
        return (__FALSE);//modem���ڵ绰״̬��
    }
    if(modphone.state != MODPH_IDLE)
    {
        return (__FALSE);//modem���ڴ���绰ҵ��
    }
    if(value > 9)
    {
        value = 9;
    }
          
    modphone.clvl = value;
    Modem_State_SwitchPhone(MODPH_CLVL);
    
    return (__TRUE);

}
/*
  * @brief  ��ȡ��ǰ����
  * @param  None
  * @retval ���ص�ǰ����
  */
u8    Modem_Api_Phone_ClvlGet(void)
{
    return modphone.clvl;
}
/*
  * @brief  ������Ƶͨ��
  * @param  channel :��Ƶͨ����
  * @retval ���ش�����
  */
BOOL  Modem_Api_Phone_Audch(u8 channel)
{
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);//����״̬
    }
    
    if(modapi.wait != MODON_PHONE)
    {
        return (__FALSE);//modem���ڵ绰״̬��
    }
    if(modphone.state != MODPH_IDLE)
    {
        return (__FALSE);//modem���ڴ���绰ҵ��
    }
          
    modphone.audch= channel;
    Modem_State_SwitchPhone(MODPH_AUDCH);
    
    return (__TRUE);

}
/*
  * @brief  ����ͨ���а���
  * @param  *p :ָ���ֵ
  * @retval ���ش�����
  */
BOOL  Modem_Api_Phone_Vts(u8 *p)
{
    if(modem.state != MODEM_ON)
    {
        return (__FALSE);//����״̬
    }
    
    if(modapi.wait != MODON_PHONE)
    {
        return (__FALSE);//modem���ڵ绰״̬��
    }
    if(modphone.state != MODPH_IDLE)
    {
        return (__FALSE);//modem���ڴ���绰ҵ��
    }
          
    memset(modphone.vts,0,sizeof(modphone.vts));
    memcpy(modphone.vts,p,1);
    Modem_State_SwitchPhone(MODPH_VTS);
    
    return (__TRUE);

}
/*
  * @brief  ��ѯģ���Ƿ�֧��TTS����
  * @param  None
  * @retval ֧��TTS����1, ��֧�ַ���0
  */
u8  Modem_Api_Tts_IsSpt(void)
{
    return Modem_State_Tts_Is(1);
}
/*
  * @brief  ��ѯ������״̬
  * @param  None
  * @retval ���ڲ�������1, ������Ϸ���0
  */
u8  Modem_Api_Tts_IsSta(void)
{
    return Modem_State_Tts_Is(0);
}
/*
  * @brief  ��ѯTTS����
  * @param  None
  * @retval ����������С ��С 0--9 
  */
u8  Modem_Api_Tts_IsClvl(void)
{
    return modtts.clvl;
}
/*
  * @brief  ����TTS����
  * @param  value:������С 0--9 
  * @retval �ɹ�����1, ʧ�ܷ���0
  */
u8  Modem_Api_Tts_Clvl(u8 value)
{
    if(modtts.state != MODTTS_IDLE)
    {
        return 0;
    }
    if(value > 9)
    {
        value = 9;
    }
    modtts.clvl  = value;
    modtts.state = MODTTS_CLVL;
    modtts.send  = 1;
    return 1;
}
/*
  * @brief  ֹͣ����
  * @param  None
  * @retval �ɹ�����1, ʧ�ܷ���0
  */
u8  Modem_Api_Tts_Stop(void)
{
    if(modem.state < MODEM_ATI)
    {
        return 0;//���ܲ���
    }
    
    if(modtts.busy)//ֻ�����ڲ���ʱ�ŷ���ֹͣ����
    {
        modtts.busy = 0;
        modtts.state= MODTTS_STOP;
    }
    return 1;
}
/*
  * @brief  �����ı�����
  * @param  type : 0Ϊ��������,��������ڲ�����������ᱻ��ϣ�
  *                1Ϊ��������,��������ڲ�����������ȴ�������ٲ����µ�;
  * @param  *p :����������
  * @param  lentts :�������ݵĳ���,�������Ϊ0���ַ���\0Ϊ����
  * @retval �ɹ�����1, ʧ�ܷ���0
  */
u8  Modem_Api_Tts_Play(u8 type,u8 *pTts, u8 lentts)
{
    if(modem.state < MODEM_ATI)
    {
        return 0;//���ܲ���
    }

    if(modem.onsta == MODON_PHONE)
    {
        return 0;//ͨ���н�ֹ����TTS
    }
    
    if(modem.onsta == MODON_FTP)
    {
        return 0;//FTP�ļ�����ʱ��ֹ����TTS
    }

    if(modtts.send)
    {
        return 0;//����ϴε����ݻ�û�������ֹ�µ����ݲ���
    }

    if(modtts.type)
    {
        return 0;//�������Ѿ���һ���ȴ�����������
    }

    if(Modem_State_Tts_Dt(type,pTts,lentts) == 0)
    {
        return 0;//�Ƿ�����
    }
    modtts.send = 1;
    
    return 1;
}




