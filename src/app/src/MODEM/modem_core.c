/*
********************************************************************************                                                           
*
* @file    modem_core.c
* @author  miaoyahan
* @version V1.0.0
* @date    2013-08-20
* @brief
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
#include  <app_cfg.h>
#include  <includes.h>
#endif

#include <stdarg.h>
/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/

/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/


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
static u8  mbuf[MODEM_AT_SIZE];               
static u16 mlen; 

static u32 soc_rec_len;//���յ�soc���ݳ��ȣ����ڵ��Թ۲⣬��ʵ��Ӧ��

static u8  ZTE_IpStaNum;//��ѯָ��soc����״̬0-5������ר��

static u8  FtpSectorBuf[520];//FTP�������ݻ��棬��׼512���ֽ�
static u16 FtpSectorBufLen;//FTPר���������ݻ�������

/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/
MODEM_TYPE  Modem_Type;//ģ������

ST_MODAT    modat;     //AT�����
ST_MODEM    modem;     //modemȫ�ֽṹ��
ST_MODINF   modInf;    //modem���е���Ϣ
ST_MODFTP   modftp;    //ftpҵ��ȫ�ֽṹ��
ST_MODSMS   modsms;    //����ҵ��ȫ�ֽṹ��
ST_MODHP    modphone;  //�绰ҵ��ȫ�ֽṹ��
ST_MODTTS   modtts;    //ttsҵ��ȫ�ֽṹ��
SM_PARAM    modsmspdu; //PDU���Žṹ��

u8          ModSocBuf[MODEM_SOC_TX_SIZE*MODEM_SOC_NUM];
MODLIB_BUF  ModSocNdNum[MODEM_SOC_ND_SIZE*MODEM_SOC_NUM];
MODLIB_NOD  ModSocNode[MODEM_SOC_NUM];//soc�ڵ�����ṹ��
MODSOC_STA  Modem_IpSock_STA[MODEM_SOC_NUM];//sockö�����飬��Ÿ���״̬


u8  Modem_ActPinOnDly  = 2;//Modem�������ſ�����ʱʱ�䣬 Ĭ��2s
u8  Modem_ActPinOffDly = 2;//Modem�������Źػ���ʱʱ�䣬Ĭ��2s

u8 QuectelAtTtsFlag;//��ԶM35 TTS����ר�ñ�־,���ڽ�һ��ȷ���Ƿ�֧��TTS����

u8 SimCardInsertFlag = 1;//dxl,2015.11.5,SIM��δ�����־,0Ϊδ����,��û�в���SIM��ʱֻ��TTS������ʹ��

/*
********************************************************************************
*                          EXTERN VARIABLES
********************************************************************************
*/
extern u8 NetFirstManualCloseFlag;
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
   
/*
  * @brief  modem��׼��ӡ����
  * @param  None
  * @retval ���ؽ��
  */
void Modem_Printf (char *format, ...)
{
#ifdef MODEM_DEBUG_H
    va_list   v_args;
    char bufstr[MODDB_PRTF_SIZE + 1u];
    u16  len;

    va_start(v_args, format);
   (void)vsnprintf((char       *)&bufstr[0],
                   (u32         ) sizeof(bufstr),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

    len = strlen(bufstr);
    
    if(ModDbInfType != MODDB_OFF)
    {
        Modem_Debug_UartTx((u8*)bufstr,len);
    }
#endif
}

/*
  * @brief  modem��ӡָ����Ϣ
  * @param  ��ӡ������
  * @retval ���ؽ��
  */
void Modem_PrtfTo (MODPRT_TYPE type)
{   
#ifdef MODEM_DEBUG_H
    u8  strbuf[sizeof(ModDbPrtf[0])+5]={0};
    u8  len = sizeof(ModDbPrtf[0]);

    if(ModDbInfType != MODDB_BASE)
    {
        return;
    }

    memcpy(strbuf,(u8*)&ModDbPrtf[type][0],len);
    strcat((char*)strbuf,"\r\n\r\n");
    Modem_Printf("%s",strbuf);
#endif    
}

/**
  * @brief  �򴮿ڷ���AT����
  * @param  *str: ָ����modem��ATָ��.
  * @retval None.
  */
static void send_cmd (u8 *str) 
{
    u8 len = strlen((char*)str);
    Modem_Uart_NodClr();                      //���������������add jonening
    Modem_Uart_Send(str,len);

    if(modem.state > MODEM_OFF)//ֻ��ģ���������ۼ�
    {
        modem.atack++;//ÿ����һ��AT�����1�������Ӧ������0
    }
}
/**
  * @brief  ״̬����ʱ,��λΪ��
  * @param  Ҫ��ʱ��ʱ��.
  * @retval None.
  */
void Modem_Delay (u8 delay) 
{
    if(delay != 0)
    {
       modem.delay= MODEM_TSEC*delay;
    }
    else
    {
        modem.delay= 1;//50ms
    }
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @param  *Call_Ati: modem ��Ϣ�Ļص�����.
  * @retval ���ش�����.
  */
static BOOL  Modem_StrCmp_Base(u8 *pRecbuf , u16 nRecLen)
{
    if(ModLib_AtCmp(mbuf,"ERROR") == 0)
    {   
        modem.delay = 0;
    }
    else if(ModLib_AtCmp(mbuf,modat.recbuf) == 0)
    {
        return (__TRUE);
    }
    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @param  *Call_Ati: modem ��Ϣ�Ļص�����.
  * @retval ���ش�����.
  */
static BOOL  Modem_StrCmp_Ati(u8 *pRecbuf , u16 nRecLen)
{

    if(Modem_Type == MODEM_TYPE_IDE)
    {
        if(ModLib_AtCmp(pRecbuf,Modem_Quectel) == 0)//��ԶMϵ��ģ��  G��
        {
            Modem_Type         = MODEM_TYPE_Q;
            Modem_ActPinOnDly  = Modem_QActOnDly;
            Modem_ActPinOffDly = Modem_QActOffDly;
        }
        else if(ModLib_AtCmp(pRecbuf,Modem_ZTE) == 0)//����ϵ��ģ��  C��
        {
            Modem_Type         = MODEM_TYPE_ZTE;
            Modem_ActPinOnDly  = Modem_ZTEActOnDly ;
            Modem_ActPinOffDly = Modem_ZTEActOffDly;      
        }
        
    }
    else if(Modem_Type == MODEM_TYPE_Q)
    {
        if(ModLib_AtCmp(pRecbuf,"Revision:") == 0)
        {
            //if(ModLib_AtCmp(pRecbuf,"Revision: M35FA") == 0) dxl,2015.9.11
                        if(ModLib_AtCmp(pRecbuf,"Revision: GC65") == 0)
            {
                QuectelAtTtsFlag = 1;//֧��TTS
            }
            return (__TRUE);
        }
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)
    {
        if(ModLib_AtCmp(pRecbuf,"Revision:") == 0)
        {
            return (__TRUE);
        }
    }
    return (__FALSE);

}

/**
  * @brief  ������վ�����IP.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @param  *Call_LocIp: ��ȡ����IP�ص�����.
  * @retval ���ش�����.
  */
static BOOL  Modem_StrCmp_IpLoc(u8 *pRecbuf , u16 nRecLen,void(*Call_LocIp)(u8 *pCall_Data, u8 Call_Len))
{
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        if((*pRecbuf >= 0x30)&&(*pRecbuf <= 0x39))
        {
            Call_LocIp(pRecbuf,nRecLen-2);//��ȡ��վ�����IP
            return (__TRUE);
        }
    }     
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        if(ModLib_AtCmp(pRecbuf,"+ZIPGETIP:") == 0)
        {
            Call_LocIp(pRecbuf+10,nRecLen-12);
            return (__TRUE);
        }
    } 

    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */
static BOOL  Modem_StrCmp_Search(u8 *pRecbuf , u16 nRecLen)
{
    u8 *p =NULL;
    u8 addr;
    ////////////////////////////////////////////////////////////////��ȡ�ź����� 
    if(ModLib_AtCmp(pRecbuf,"+CSQ: ") == 0)///////////////////////////////+CSQ: 19,0 
    {
        p = pRecbuf + 6;
        modem.csq = atoi((char *)p);

        #ifdef MODEM_DEBUG_H
        if(ModDbInfType == MODDB_BASE)
        {
            MODDB_PRTFBAS("modem->CSQ = %d\r\n",modem.csq);
        }
        #endif
        
        if(modem.csq == 99)
        {
            modem.csq = 0;
        }
        if((modem.csq >= 10)||(modem.state == MODEM_ON))
        {
            return (__TRUE);
        }
    } 
    ///////////////////////////////////////////////////////////////////SIM��״̬
    else if(ModLib_AtCmp(pRecbuf,"+CPIN: READY\r") == 0) 
    { 
        #ifdef MODEM_DEBUG_H
        if(ModDbInfType == MODDB_BASE)
        {
            MODDB_PRTFBAS("modem->SIM = READY\r\n");
        }
        #endif    
        return (__TRUE);    
    }
    ////////////////////////////////////////////////////////////////////���ĺ��� 
    else if(ModLib_AtCmp(pRecbuf,"+CSCA: ") == 0)//+CSCA: "+8613800755500",145
    {
           p = pRecbuf;
        addr = ModLib_AtAddr(p,'"')+1;
          p += addr;               //���ݵ�ַ
        addr = ModLib_AtAddr(p,'"');//����
        
        memset(modem.csca,0,sizeof(modem.csca));
        memcpy(modem.csca,p,addr);
        return (__TRUE); 
    }
    ////////////////////////////////////////////////////////////��ѯGPRSע����� 
    else if(ModLib_AtCmp(pRecbuf,"+CGREG: ") == 0)//+CGREG: 0,1
    {
        p = pRecbuf+10;
        if((*p == '1')||(*p == '5'))
        {
            return (__TRUE);
        }
    } 
    ////////////////////////////////////////////////////////////��ѯ����ע�����    
    if(ModLib_AtCmp(pRecbuf,"+CREG:") == 0)
    {
        if(Modem_Type == MODEM_TYPE_Q)//��Զ +CREG: 1,1
        {
            p = pRecbuf+9;
        } 
        else if(Modem_Type == MODEM_TYPE_ZTE)//���� +CREG:1,1
        {
            p = pRecbuf+8;
        }
        if((*p == '1')||(*p == '5'))
        {
            return (__TRUE);
        }
    }
    return (__FALSE);
}
/**
  * @brief  ��ȡCCID����
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */
static BOOL  Modem_StrCmp_Ccid(u8 *pRecbuf , u16 nRecLen)
{
  u8 len=0;
  u8 addr=0;
  u8 *p =NULL;
  
    if(nRecLen > 5)
    {
        if(Modem_Type == MODEM_TYPE_Q)////////////��Զ 89860008191249803743
        {              
            p    = pRecbuf;
            len = ModLib_AtLen(p);
        }
        else if(Modem_Type == MODEM_TYPE_ZTE)/////���� +GETICCID:0x89860310707551338748
        {
            p    = pRecbuf;
            addr = ModLib_AtAddr(p,'x')+1;
            p   += addr;
            len = ModLib_AtLen(p);
        }        
        memset(modem.ccid,0,sizeof(modem.ccid));
        ModLib_StrByte(p,modem.ccid,len);
        Modem_Port_Call_CCID(modem.ccid,len/2);
    }
    if(ModLib_AtCmp(pRecbuf,"OK") == 0)
    {
        if(ModLib_AtLen(modem.ccid))
        {
            return (__TRUE);
        }
    }
    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��-��Զģ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @param  *Call_IpStat:(index : soc��
                          *pMode : tcp or ucp
                          *pAddr ��ip��ַ
                          *pPort ���˿ں�).
  * @retval ���ش�����.
  */                             
static BOOL  Modem_StrCmp_IpSta_Q(u8 *pRecbuf , u16 nRecLen)
{
    u8  SockNum;
    u8  *pCmp = pRecbuf;
    u8  tmp;

    //+QISTAT: 0, "TCP", "121.15.212.251", 10002,
    //+QISTAT: 1, "", "",
    
    if(ModLib_AtCmp(pCmp,"+QISTATE: ") == 0)//dxl,2015.9.2,+QISTAT:
    {
        SockNum = *(pCmp+10)-0x30;//soc��dxl,SockNum = *(pCmp+9)-0x30

        if(SockNum < MODEM_SOC_NUM)
        {
            tmp = ModLib_AtLen(pCmp);
            //if(tmp == 19)dxl,2015.9.2
                        if(tmp == 32)
            {
                Modem_IpSock_STA[SockNum] = MODSOC_IDLE;//��sock����
            }
            else if(tmp > 32)
            {
                //pCmp += 13;dxl,2015.9.2
                                pCmp += 14;
                if((ModLib_AtCmp(pCmp,(u8 *)"TCP") == 0)||(ModLib_AtCmp(pCmp,(u8 *)"UDP") == 0))
                {
                    tmp = ModLib_AtAddr(pCmp,',')+1;
                    pCmp += tmp;
									  tmp = ModLib_AtAddr(pCmp,',')+1;
                    pCmp += tmp;
									  tmp = ModLib_AtAddr(pCmp,'"')+1;
                    pCmp += tmp;
									  if(ModLib_AtCmp(pCmp,(u8 *)"CONNECTED") == 0)//lzm,2016.4.22ȥ��������
										{
											Modem_IpSock_STA[SockNum] = MODSOC_ONLN;//��sock����
                    #ifdef MODEM_DEBUG_H
                    if(ModDbInfType == MODDB_BASE)
                    {
                        if(Modem_IpSock_STA[SockNum] == MODSOC_ONLN)
                        {
                            MODDB_PRTFBAS("modem->OLN = %d\r\n",SockNum);
                        }
                    }
                    #endif  
									  }	
                    else	
										{
											if(Modem_IpSock_STA[SockNum] == MODSOC_ONLN)
                    {
                        Modem_IpSock_STA[SockNum] = MODSOC_ERR;//��sock����                  
                    }
										}											
                }
            }
        }
        if(SockNum == 5)//���һ��soc���ؽ��
        {
            return (__TRUE);
        }
    }
    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��-����MC8331ģ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */                             
static BOOL  Modem_StrCmp_IpSta_ZTE(u8 *pRecbuf , u16 nRecLen)
{
    u8  *pCmp = pRecbuf+12;
    u8  flag = 0;

    //+ZIPSTATUS: CLOSED
    //+ZIPSTATUS: ESTABLISHED
    //+ZIPSTATUS: SYN_SENT 
    if(ModLib_AtCmp(pCmp,"CLOSED") == 0)
    {
        Modem_IpSock_STA[ZTE_IpStaNum] = MODSOC_IDLE;
        flag = 1;
    }
    else if(ModLib_AtCmp(pCmp,"ESTABLISHED") == 0)
    {
        if(Modem_IpSock_STA[ZTE_IpStaNum] == MODSOC_IDLE)
        {
            Modem_IpSock_STA[ZTE_IpStaNum] = MODSOC_ONLN;//��sock����            
        }
        #ifdef MODEM_DEBUG_H
        if(ModDbInfType == MODDB_BASE)
        {
            if(Modem_IpSock_STA[ZTE_IpStaNum] == MODSOC_ONLN)
            {
                MODDB_PRTFBAS("modem->OLN = %d\r\n",ZTE_IpStaNum);
            }
        }
        #endif  
        flag = 1;
    }
    else if(ModLib_AtCmp(pCmp,"SYN_SENT") == 0)
    {
        Modem_IpSock_STA[ZTE_IpStaNum] = MODSOC_DOING;
        flag = 1;
    }
    
    if(flag)
    {
        if((++ZTE_IpStaNum) < MODEM_SOC_NUM)
        {
            modem.index--;
        }
        else 
        {
            ZTE_IpStaNum = 0;
        }
        return (__TRUE);
    }
    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��-���ͨ 
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */                             
static BOOL  Modem_StrCmp_IpSta(u8 *pRecbuf , u16 nRecLen)
{
    u8 flag = 0;
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        if (Modem_StrCmp_IpSta_Q(pRecbuf,nRecLen)  == __TRUE)
        {   
            flag = 1; 
        }
    }                  
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        if(Modem_StrCmp_IpSta_ZTE(pRecbuf,nRecLen) == __TRUE)
        {
            flag = 1; 
        }    
    } 

    if(flag)
    {
        return (__TRUE); 
    }
    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��-��Զģ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */    
static BOOL  Modem_StrCmp_IpOpen(u8 *pRecbuf , u16 nRecLen)
{ 
    if(ModLib_AtCmp(pRecbuf,modat.recbuf) == 0)
    {
        return (__TRUE);
    }
    
    return (__FALSE);
}

/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��-��Զģ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  *pLen   : ָ����յ����ݳ���.
  * @param  *pSoc   : ָ����յ�SOC����.
  * @retval �������ݵ�ַ.
  */ 
u8*  Modem_StrCmp_RecData_Q(u8 *pRecbuf, u16 *pLen, u8 *pSoc)
{  
    u16 len;
    u8  soc;
    u8  *p;
    u8  temp;

    if(ModLib_AtCmp(pRecbuf,"+RECEIVE:") == 0)//+RECEIVE: 0, 20
    {
        p=pRecbuf;
        temp = ModLib_AtAddr(p,',');
        p += temp;

        soc = *(p-1)-0x30;          //soc��
        len = atoi((char*)(p+2));   //����

        p=pRecbuf;
        temp = ModLib_AtLen(p)+2;
        p += temp;                  //���ݵ�ַ

        *pSoc = soc;                //soc��
        *pLen = len;                //����
        
        return p;                   //�������ݵ�ַ
    }
    return NULL;
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��-����ģ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  *pLen   : ָ����յ����ݳ���.
  * @param  *pSoc   : ָ����յ�SOC����.
  * @retval �������ݵ�ַ.
  */                            
u8*  Modem_StrCmp_RecData_ZTE(u8 *pRecbuf, u16 *pLen, u8 *pSoc)
{
    u16 len;
    u8  soc;
    u8  *p;
    u8  temp;
    
    if(ModLib_AtCmp(pRecbuf,"+ZIPRECV:") == 0)//+ZIPRECV:0,24,
    {
        p=pRecbuf;
        temp = ModLib_AtAddr(p,',');
        p += temp;

        soc = *(p-1)-0x30;          //soc��
        len = atoi((char*)(p+1));   //����

        p++;
        temp = ModLib_AtAddr(p,',')+1;
        p += temp;                  //���ݵ�ַ
        
        *pSoc = soc;                //soc��
        *pLen = len;                //����
        
        return p;                   //�������ݵ�ַ
    }
    return NULL;
}

/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */                             
static BOOL  Modem_StrCmp_SendData(u8 *pRecbuf , u16 nRecLen)
{
    u8 type = 0;// 1:�ɹ�  2:ʧ�� 3:����
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {
        //if(ModLib_AtCmp(pRecbuf,"SEND OK") == 0)//dxl,2015.9.6
                if(ModLib_AtCmp(pRecbuf+2,"SEND OK") == 0)
        {
            type = 1;
        }
        else if(ModLib_AtCmp(pRecbuf,"ERROR") == 0)
        {
            type = 3;
        }
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        if(ModLib_AtCmp(pRecbuf,"+ZIPSEND: -") == 0)
        {
            type = 2;
        }
        else if(ModLib_AtCmp(pRecbuf,"+ZIPSEND:") == 0)
        {
            type = 1;
        }
    }

    if(type == 1)//�ɹ�
    {
        Modem_PrtfTo(MODPRTF_DATOK);
        return (__TRUE);
    }
    else if(type == 2)//ʧ��
    {
        Modem_PrtfTo(MODPRTF_DATFA);
    }
    else if(type == 3)//����
    {
        modem.delay = 0;
        modem.index = MODEM_ATERR;
        Modem_PrtfTo(MODPRTF_DATER);
    }
    return (__FALSE);
}


/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_Ftp_Q(u8 *pRecbuf , u16 nRecLen)
{   
    u16 addr;
    u8  tab[30];
    
    if(modftp.fend)
    {
        modftp.fend = 0;//���ؽ���
        modem.delay = 0;//��������

        addr = ModLib_AtCmpEx(pRecbuf,"\r\n+QFTPGET:",nRecLen);
        modftp.fsize += addr;
        if(addr)
        {
            Modem_Api_CallFtp(pRecbuf,addr);//���һ��
        }
        sprintf((char*)tab,"\r\n+QFTPGET:%d",modftp.fsize);
        if(ModLib_AtCmp(pRecbuf+addr,tab) == 0)
        {
            modem.wait  = MODWT_IDLE;
            modftp.fres = 1;//�ɹ�
            Modem_PrtfTo(MODPRTF_FTPOK);
        }
        else
        {
            modftp.fres = 2;//ʧ��
            Modem_PrtfTo(MODPRTF_FTPFA);
        }

        return (__TRUE);
    }
    else if(modftp.busy)
    {
        if(modftp.fres == 0)
        {
            Modem_Api_CallFtp(pRecbuf,512);//������������
            modftp.fsize += 512;
            modem.delay = (120*MODEM_TSEC);//�����ļ�ʱֹͣһ�д���
            return (__TRUE);
        }
    }
    else if(ModLib_AtCmp(pRecbuf,"CONNECT") == 0)
    {
        modftp.busy  = 1;
        modftp.fend  = 0;
        modftp.fres  = 0;
        modftp.fsize = 0;
        return (__TRUE);
    }

    return (__FALSE);
}
/**
  * @brief  �Ա�Modem���ص������Ƿ�����Ҫ����ͬ��.
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_Ftp_ZTE(u8 *pRecbuf , u16 nRecLen)
{   
    static u32 atlen;
    u32 reclen;
    u16 temp;
    u8  *p;

    u16  i;
    u16  sector_int;
    u16  sector_rem;
    
    if(ModLib_AtCmp(pRecbuf,"+ZFTPDATA:CLOSED") == 0)
    { 
        modftp.fend = 0;//���ؽ���
        modem.delay = 0;//��������
        Modem_Api_CallFtp(FtpSectorBuf,FtpSectorBufLen);//���һ��
        FtpSectorBufLen = 0;
        if((modftp.fsize)&&(modftp.fsize == atlen))
        {
            modem.wait  = MODWT_IDLE;
            modftp.fres = 1;//�ɹ�
            Modem_PrtfTo(MODPRTF_FTPOK);
        }
        else
        {
            modftp.fres = 2;//ʧ��
           Modem_PrtfTo(MODPRTF_FTPFA);
        }
        return (__TRUE);
    }
    else if(ModLib_AtCmp(pRecbuf,"+ZFTPDATA:") == 0)//+ZFTPDATA:1024,1024\r\n
    {
        ////////////////////////////////////////////////////////////��ȡ�ۼƳ���
        p = pRecbuf;
        temp = ModLib_AtAddr(p,':')+1;
        p += temp;  
        atlen = atoi((char*)p);
        
        ////////////////////////////////////////////////////////////////ʵ�ʳ���
        p = pRecbuf;
        temp = ModLib_AtLen(p)+2;
        reclen = nRecLen - temp - 2;
        
        //////////////////////////////////////////////////////////ʵ�ʵ��ۼƳ���
        modftp.fsize += reclen;
        
        ////////////////////////////////////////////////////////////���ûص�����
        p += temp;//�ƶ������ݵ�ַ

        if(FtpSectorBufLen)
        {
            temp = 512-FtpSectorBufLen;
            if(temp > reclen)
            {
                temp = reclen;
            }
            memcpy(FtpSectorBuf+FtpSectorBufLen,p,temp);
            Modem_Api_CallFtp(FtpSectorBuf,512);
            p += temp;
            reclen -= temp;
        }        
        
        sector_int = reclen/512;
        sector_rem = reclen%512;

        for(i=0;i<sector_int;i++,p+=512)//��������
        {
            Modem_Api_CallFtp(p,512);
        }

        FtpSectorBufLen = sector_rem;//��������
        memcpy(FtpSectorBuf,p,sector_rem);

        modem.delay = (120*MODEM_TSEC);//�����ļ�ʱֹͣһ�д���

        return (__TRUE);
        
    }

    return (__FALSE);
}
/**
  * @brief  �����¶��ŵ�������
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_Sms(u8 *pRecbuf , u16 nRecLen)
{
    u16 len =0;
    u8  *p =NULL;
    u8  addr;
    static u8 state;
/*
    ////////////////////////////// PDU //////////////////////////////
    AT+CMGR=1

    +CMGR: 0,,24
    0891683108707515F0240D91685163515733F200003121110150412304E832BA0C

    OK
    
    /////////////////////////// �ı�ģʽ ////////////////////////////
    AT+CMGR=1

    +CMGR: "REC UNREAD","+8615361575332","","2013/12/11 14:47:07+32"
    uir

    OK  
    /////////////////////////// ���Ͷ��� ////////////////////////////
    AT+CMGS="1252015814451069"

    > 82D74E9A7FF0003A0020003300320034003300340035
    +CMGS: 57

    OK
    
*/

    if(state == 1)
    {
        state = 0;
        if(modsms.typr == SMS_PDU)//PDU��ʽ����ת��
        {
            memset((u8*)&modsmspdu,0,sizeof(SM_PARAM));
            len = ModLib_PduDecode(pRecbuf,&modsmspdu);        //���ݳ���
            p   = modsmspdu.TP_UD;                         //���ݵ�ַ
            strcpy((char*)modsms.num,(char*)modsmspdu.TPA);//���ƺ���
            if(modsmspdu.PDU_TP == GSM_8BIT)
            {
                modsms.typr = SMS_PDU8;
            }
            else if(modsmspdu.PDU_TP == GSM_7BIT)
            {
                modsms.typr = SMS_PDU7;
            }
            else
            {
                modsms.typr = SMS_ERR;
            }
            if(len == 0)
            {
                return (__TRUE); 
            }

        }
        else if(modsms.typr == SMS_TXT)//�ı�ģʽ�����޸�
        {
            *(pRecbuf+nRecLen-2)=0;
            len = nRecLen-2;
            p   = pRecbuf;        
        }
        #ifdef MODEM_DEBUG_H
        if(ModDbInfType == MODDB_BASE)
        {
            MODDB_PRTFBAS("modem-> SMS=%s, Type=%d, Rec_Len=%d\r\n",modsms.num,modsms.typr,len);
            MODDB_PRTFBAS("%s\r\n",p);
            MODDB_PRTFBAS("modem-> SMS End...\r\n\r\n");            
        }
        #endif 
        Modem_Api_CallSms(modsms.typr,p,len);
        return (__TRUE);  
    }
    else if(ModLib_AtCmp(pRecbuf,"OK\r") == 0)//���ͳɹ��ж�
    {
        return (__TRUE); 
    }
    else if(ModLib_AtCmp(pRecbuf,"+CMGR: ") == 0)
    {
        state = 1;
        if(modsms.typr == SMS_TXT)//�ı�ģʽ��ȡ����ͷ��Ϣ
        {
            p    = pRecbuf;
            addr = ModLib_AtAddr(p,',')+2;
            p   += addr;
            pRecbuf = p;//�����ַ
            addr = ModLib_AtAddr(p,',')-1;
            *(p+addr) = 0;
            strcpy((char*)modsms.num,(char*)pRecbuf);
        }
    }
    return (__FALSE);
}


/**
  * @brief  �ԱȲ���绰�󷵻������Ƿ����Ҫ��
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_Atd(u8 *pRecbuf , u16 nRecLen)
{
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        if(ModLib_AtCmp(pRecbuf,"+COLP:") == 0)
        {
            return (__TRUE);
        }
    }  
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        if(ModLib_AtCmp(pRecbuf,"+ZCANS:") == 0)
        {
            return (__TRUE);
        }
    } 
    return (__FALSE);
}
/**
  * @brief  ����socket����״̬,��Ҫ�Ǵ򿪺ͱ����ر�
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_LisCon(u8 *pRecbuf , u16 nRecLen)
{
    u8  soc=0;
    u8  type=0;// 1:��  2:����ʧ�� 3:�Ͽ�  
    
    if (Modem_Type == MODEM_TYPE_Q)//������յĺ���
    {   
        if(ModLib_AtCmp(pRecbuf+1,", CONNECT OK") == 0)//0, CONNECT OK
        {
            soc = *pRecbuf - 0x30;
            type = 1;
        }
        else if(ModLib_AtCmp(pRecbuf+1,", CONNECT FAIL") == 0)//0, CONNECT FAIL
        {
            soc = *pRecbuf - 0x30;
            type = 2;
        }        
        else if(ModLib_AtCmp(pRecbuf+1,", CLOSED") == 0)//0, CLOSED
        {
            soc = *pRecbuf - 0x30;
            type = 3;
        }
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)
    {
        if(ModLib_AtCmp(pRecbuf,"+ZTCPESTABLISHED:") == 0)//+ZTCPESTABLISHED:0
        {
            soc = (*(pRecbuf+17) - 0x30);
            type = 1;
        }
        else if(ModLib_AtCmp(pRecbuf,"+ZTCPCLOSED:") == 0)//+ZTCPCLOSED:0
        {
            soc = *(pRecbuf+12) - 0x30;
            type = 3;
            if(modem.onsta == MODON_IPCLOSE)
            {
                type = 0;//�������洦��
            }
        }
    }
    
    if(type == 0)
    {
        return (__FALSE); 
    }

    if(type == 1)//������
    {
        Modem_IpSock_STA[soc] = MODSOC_ONLN;      
    }
    else if(type == 2)//����ʧ�� 
    {
        Modem_IpSock_STA[soc] = MODSOC_ERR;
    }
    else if(type == 3)//�����ر�
    {
        if(Modem_IpSock_STA[soc] == MODSOC_ONLN)
        {
            Modem_IpSock_STA[soc] = MODSOC_IDLE;//�����ߵ�����
            #ifdef MODEM_DEBUG_H
            if(ModDbInfType == MODDB_BASE)
            {
                MODDB_PRTFBAS("modem->Soc=%d Closed\r\n",soc);
            }
            #endif              
        }
        else
        {
            Modem_IpSock_STA[soc] = MODSOC_ERR;//������ʱ���ִ���
        }       
    }
    return (__TRUE); 
}
/**
  * @brief  ����socket��·
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_LisRec(u8 *pRecbuf , u16 nRecLen)
{
    u8  *p;
    u16 len;
    u8  soc;

    p   = pRecbuf;
    len = 0;
    
    if (Modem_Type == MODEM_TYPE_Q)//��Զ 
    {   
        p = Modem_StrCmp_RecData_Q(p,&len,&soc);
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)//����
    {
        p = Modem_StrCmp_RecData_ZTE(p,&len,&soc);
    }
    if(len)
    {
        soc_rec_len += len;
        modInf.Tim_SocEr = 0;
        modInf.Num_SocRx[soc]++;            //��������
        Modem_Call_Soc_Function[soc](p,len);//����Ӧ�ó���
        #ifdef MODEM_DEBUG_H
        if(ModDbInfType == MODDB_BASE)
        {
            MODDB_PRTFBAS("modem-> Soc = %d   Rec_Len = %04d\r\n",soc,len);
            if(len > 200)
            {
                len = 200;
            }
            memset(mbuf+(MODEM_AT_SIZE-600),0,600);
            ModLib_ByteStrEx(mbuf+(MODEM_AT_SIZE-600),p,len);
            p = mbuf+(MODEM_AT_SIZE-600);
            MODDB_PRTFBAS("%s",p);
            MODDB_PRTFBAS("\r\n");
            Modem_PrtfTo(MODPRTF_RECOK);
        }
        #endif
        return (__TRUE);
    }
    return (__FALSE);
}
/**
  * @brief  ����FT����
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_LisFtp(u8 *pRecbuf , u16 nRecLen)
{
    u8 result = __FALSE;

    if(modftp.state == 0)
    {
        return result;
    }
    
    if (Modem_Type == MODEM_TYPE_Q)//��Զ 
    {   
        result = Modem_StrCmp_Ftp_Q(pRecbuf,nRecLen);
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)//����
    {
        result = Modem_StrCmp_Ftp_ZTE(pRecbuf,nRecLen);
    }
    return result;
}
/**
  * @brief  ����modem����������
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_ListenData(u8 *pRecbuf , u16 nRecLen)
{
    u8 result = __FALSE;
    
    if((Modem_StrCmp_LisCon(pRecbuf,nRecLen)  == __TRUE))//socket״̬����
    {
        result = __TRUE;
    } 
    else if((Modem_StrCmp_LisRec(pRecbuf,nRecLen)  == __TRUE))
    {
        result = __TRUE;
    }
    return result; 
}
/**
  * @brief  �����¶��ŵ�������
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_ListenSms(u8 *pRecbuf , u16 nRecLen)
{
    u8 addr;
    static u8 value;
    
    if(ModLib_AtCmp(pRecbuf,"+CMTI:") == 0)//+CMTI: "SM",2
    {
        addr = ModLib_AtAddr(pRecbuf,',')+1;
        pRecbuf += addr;
        value = atoi((char *)pRecbuf);//���Ÿ���
        
        if(Modem_Type == MODEM_TYPE_ZTE)
        {
            modsms.typr = SMS_TXT;
        }
        else
        {
            modsms.rec = value;
            modsms.typr = SMS_PDU;
        }
        return (__TRUE);
    }
    else if(ModLib_AtCmp(pRecbuf,"SMS READY") == 0)//����ר��
    {
        modsms.rec = 1;
        return (__TRUE);
    }
    return (__FALSE);
}
/**
  * @brief  ��������
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_ListenPhone(u8 *pRecbuf , u16 nRecLen)
{
    u16 i=0;
    u16 ch=0;
	  u8 cmptab[20];
    
    if(ModLib_AtCmp(pRecbuf,"+CLIP:") == 0)
    {
     //+CLIP: "15361575332",161,"",,"",0  ��Զ
     //+CLIP: "15361575332",161,,,"",0    ���ͨ
     //+CLIP:"18938993721",129            ����
        if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
        {              
            ch = 8;
        }
        else if(Modem_Type == MODEM_TYPE_ZTE)/////����
        {
            ch = 7;
        } 
        for(i=ch;i<nRecLen;i++)
        {
            if(*(pRecbuf+i)=='"')
            {
                break;
            }
        }
        if(i != ch)
        {
            if(modphone.busy == 0)
            {
                memset(modphone.num,0,sizeof(modphone.num));
                memcpy(modphone.num,pRecbuf+ch,i-ch);
                Modem_Bsp_SpkOff(BSP_SPK_TTS);//�ر������� 	
                strcpy((char *)cmptab,"AT+QAUDCH=0\r");//dxl,2016.3.30������������л���Ƶͨ�������GC65��bug
						    Modem_Uart_Send(cmptab,strlen((char*)cmptab));							
                Modem_Bsp_MicOff();
                Modem_State_Phone_Enter();                
            }
        }
        #ifdef MODEM_DEBUG_H
        if(ModDbInfType == MODDB_BASE)
        {
            MODDB_PRTFBAS("modem->RING:%s;\r\n",(char*)modphone.num);
        }
        #endif  
        return (__TRUE);
    }
 
    if(modapi.wait != MODON_PHONE)
    {
        return (__FALSE);
    }
    if((modphone.state == MODPH_RING)||(modphone.state == MODPH_IDLE)
      ||(modphone.state == MODPH_ATD)||(modphone.state == MODPH_VTS))
    {
        ch = 0;
        if(ModLib_AtCmp(pRecbuf,"NO CARRIER") == 0)
        {   
            ch = 1;
        }   
        else if(ModLib_AtCmp(pRecbuf,"NO DIALTONE") == 0)
        {   
            ch = 1;
        }         
        else if(ModLib_AtCmp(pRecbuf,"+ZCEND:") == 0)        
        {   
            ch = 1;
        }
        else if(ModLib_AtCmp(pRecbuf,"BUSY") == 0)         
        {   
            ch = 1;
        }         
        if(ch == 1)
        {
            #ifdef MODEM_DEBUG_H
            if(ModDbInfType == MODDB_BASE)
            {
                MODDB_PRTFBAS("modem->�绰�ѶϿ�!!!\r\n\r\n");
            }
            #endif        
            modem.delay   = 0;   
            modem.wait    = MODWT_IDLE; 
            Modem_State_Phone_Break();//�˳��绰ģʽ  
        }
    }
    return (__FALSE);
}
/**
  * @brief  ����TTS�Ƿ񲥱����
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_ListenTts(u8 *pRecbuf , u16 nRecLen)
{
    u8 cmptab[15]={0};
    u8 cmplen = 0;
    u8 len;
    
    if(Modem_Type == MODEM_TYPE_ZTE)/////���� 
    {
        cmplen = ModLib_AtCmy(cmptab,"+ZTTS:0");        
    } 
    else if(Modem_Type == MODEM_TYPE_Q)/////��Զ
    {
        cmplen = ModLib_AtCmy(cmptab,"+QTTS: 0");         
    }       
    if(ModLib_AtCmp(pRecbuf,"OK") == 0) 
    {
        modtts.cmdack = 0;
    }
    
    len = strlen((char *)cmptab);
    if(len)
    {
         if(strncmp((char *)pRecbuf,(char *)cmptab,cmplen) == 0)
        {
            modtts.busy = 0;
            #ifdef MODEM_DEBUG_H
            if(ModDbInfType == MODDB_BASE)
            {
                MODDB_PRTFBAS("modem->Tts Play End\r\n\r\n");
            }
            #endif             
            Modem_Bsp_SpkOff(BSP_SPK_TTS);//�ر�������  
//						strcpy((char *)cmptab,"AT+QAUDCH=0\r");//dxl,2016.3.30������������л���Ƶͨ�������GC65��bug dxl,2016.8.27
	//					Modem_Uart_Send(cmptab,strlen((char*)cmptab));
            return (__TRUE);
        }   
    }
    return (__FALSE);
}

/**
  * @brief  ����ģ�鿪��
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_ListenOn(u8 *pRecbuf , u16 nRecLen)
{
    u8 flag = 0;

    if(modem.state >= MODEM_ATI)//��鿪�������ֹ�����ػ�ʱ��ߵ�,ֻ�ڿ���ǰ���
    {
        return (__FALSE);
    }
    
    if(ModLib_AtCmp(pRecbuf,"RDY")==0)//������Զ����ָ��
    {      
        Modem_Type         = MODEM_TYPE_Q;
        Modem_ActPinOnDly  = Modem_QActOnDly;
        Modem_ActPinOffDly = Modem_QActOffDly;
        flag = 1;  
    }
    else if(ModLib_AtCmp(pRecbuf,"+ZIND:8")==0)//�������˿���ָ��
    {
        Modem_Type         = MODEM_TYPE_ZTE;
        Modem_ActPinOnDly  = Modem_ZTEActOnDly;
        Modem_ActPinOffDly = Modem_ZTEActOffDly;
        flag = 1; 
    }
    
    if(flag)
    {
        Modem_State_Switch(MODEM_ATI);
        return (__TRUE);
    }

    return (__FALSE);
}

/**
  * @brief  ����ģ���쳣
  * @param  *pRecbuf: ָ��Ҫ���Ա����ݵĻ�����.
  * @param  nRecLen: ��Ҫ���Ա����ݻ������ĳ���.
  * @retval ���ش�����.
  */ 
static BOOL  Modem_StrCmp_ListenFalt(u8 *pRecbuf , u16 nRecLen)
{
    u8 flag = 0;

    if(modem.state < MODEM_ATI)//����쳣����,ֻ�ڿ�������
    {
        return (__FALSE);
    }
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {
        if (ModLib_AtCmp(pRecbuf,"Call Ready")==0)
        {
            if(modem.state > MODEM_ATI)
            {
                Modem_State_Switch(MODEM_SETAUD);//ģ��׼������ dxl,2015.5.13
            }
        }
        else if(ModLib_AtCmp(pRecbuf,"+CPIN: NOT INSERTED")==0)
        {
            Modem_State_Switch(MODEM_TTS);//δ�忨תTTS
        } 
        else if(ModLib_AtCmp(pRecbuf,"+CPIN: NOT READY")==0)
        {
            flag = 1;//������λ,��λ�ں�
        }
        else if(ModLib_AtCmp(pRecbuf,"+PDP DEACT")==0)
        {
            flag = 1;//ģ���쳣,,��λ�ں�
        } 
				else if(ModLib_AtCmp(pRecbuf,"STATE: PDP DEACT")==0)
        {
            flag = 1;//GPRS�����쳣�ر�
            //Modem_Send_cmd_Base("AT+QIDEACT\r","DEACT OK\r",3,5);
        }
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        if(ModLib_AtCmp(pRecbuf,"+ZIND:0")==0)//���SIM�Ƿ���λ
        {
            //������λ��λȱʡֵmodem�ں�
        }
    }
    if(flag)
    {
        Modem_State_Switch(MODEM_ERROR);//���������
    }
    return (__FALSE);
}

/**
  * @brief  ��modem����ATָ������������ͳ�ʱʱ�䡢�ط�����.
  * @param  type: ָ��ȴ���״̬.
  * @param  *pCmd: ָ��Ҫ���͵�ATָ��.
  * @param  *pReply: ָ�򷵻���ȷ��ATָ��.
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval ���ش�����.
  */
void  Modem_Send_cmd(MODWT_STA type,u8 *pCmd, u8 *pReply,u8 Outtime, u8 Try)
{
    memset(modat.sendbuf,0,sizeof(modat.sendbuf));//���㻺��
    memset(modat.recbuf,0,sizeof(modat.recbuf));  //���㻺��
    strcpy((char*)modat.sendbuf,(char*)pCmd);     //���Ʒ���ָ��
    strcpy((char*)modat.recbuf,(char*)pReply);    //���ƽ���ָ��
    modat.outtime = Outtime*MODEM_TSEC;           //���ó�ʱʱ��
    if(Try != NULL)
    {
        modat.trynum = Try;                       //�����ط�����
        modat.tryadd = 0;
    }
    modem.wait = type;                            //���õȴ�״̬
    if(pCmd != NULL)
    {
        //Modem_Uart_NodClr();                      //��������������� modify joneming
        send_cmd (pCmd);                          //����ָ��
    }
    modem.delay = modat.outtime;                  //�״���ʱʱ��
}
/**
  * @brief  ֱ�ӷ���AT����жϽ��
  * @param  *pCmd: ָ��Ҫ���͵�ATָ��.
  * @retval none.
  */
void  Modem_Send_cmd_Only(u8 *pCmd)
{
    modem.delay = 0;
    modem.wait  = MODWT_NO; 
    modem.index++;
    send_cmd (pCmd);
}

/**
  * @brief  ��modem���ͻ���ATָ�������ʱʱ�䡢�ط�����.
  * @param  *pCmd: ָ��Ҫ���͵�ATָ��.
  * @param  *pReply: ָ�򷵻���ȷ��ATָ��.
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval none.
  */
void  Modem_Send_cmd_Base(u8 *pCmd, u8 *pReply,u8 Outtime, u8 Try)
{
    Modem_Send_cmd(MODWT_CMD,pCmd,pReply,Outtime,Try);    
}
/**
  * @brief  ��modem����APN����ָ�������ʱʱ�䡢�ط�����.
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval None.
  */
void  Modem_Send_cmd_Apn(u8 Outtime, u8 Try)
{   
    u8  *pCmd,*pRec;   
    
    u8 Tab_r[20];
    u8 Tab_t[(sizeof(modem.apn)*3)+20];//G�� : AT+QICSGP=1,"CMNET","user","pass"   
                                       //C�� : AT+ZPIDPWD=card,card
    memset(Tab_t,0,sizeof(Tab_t));
    memset(Tab_r,0,sizeof(Tab_r));
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        sprintf((char*)Tab_t,"AT+QICSGP=1,\"%s\",\"%s\",\"%s\"\r",modem.apn,modem.user,modem.pass);
        strcpy((char*)Tab_r,"OK\r");
    }    
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        sprintf((char*)Tab_t,"AT+ZPIDPWD=%s,%s\r",modem.user,modem.pass);
        strcpy((char*)Tab_r,"OK\r");   
    } 

    pCmd = Tab_t;
    pRec = Tab_r;
    
    Modem_Send_cmd_Base(pCmd,pRec,Outtime,Try);
    #ifdef MODEM_DEBUG_H
    if(ModDbInfType == MODDB_BASE)
    {
        MODDB_PRTFBAS("modem->Apn  = %s\r\n",modem.apn);
        MODDB_PRTFBAS("modem->User = %s\r\n",modem.user);     
        MODDB_PRTFBAS("modem->Pass = %s\r\n\r\n",modem.pass);
    }
    #endif        

}
/**
  * @brief  ��modem����ATI�����ѯģ����Ϣ.
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval None
  */
void  Modem_Send_cmd_Ati(u8 Outtime, u8 Try)
{
    Modem_Send_cmd(MODWT_ATI,"ATI\r",NULL,Outtime,Try);  
}
/**
  * @brief  ��modem����ATI�����ѯģ����Ϣ.���ͨר��
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval None
  */
void  Modem_Send_cmd_AtiFibocom(u8 Outtime, u8 Try)
{
    Modem_Send_cmd(MODWT_ATI,"ATI8\r",NULL,Outtime,Try);  
}
/**
  * @brief  ��modem���Ͳ�ѯ����IP����.
  * @param  *pCmd: ָ��Ҫ���͵�ATָ�
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval ���ش�����.
  */
void  Modem_Send_cmd_IpLoc(u8 *pCmd ,u8 Outtime, u8 Try)
{     
    Modem_Send_cmd(MODWT_LOCIP,pCmd,NULL,Outtime,Try);/*��ȡ��վ�����IPָ���*/
}



/**
  * @brief  ��modem�����������͵�ATָ������ź��������������ĺ��롢����ע�����,
            ��Щ��Ϣ�����洢����֮��Ӧ�Ľӿڻ������С�
  * @param  *pCmd: ָ��Ҫ���͵�ATָ�
            ע�⣺��֧��CSQ��CREG��CGREG��CSCA,������֧�֣�������������
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval ���ش�����.
  */
void  Modem_Send_cmd_Search(u8 *pCmd,u8 Outtime, u8 Try)
{
    Modem_Send_cmd(MODWT_SEARCH,pCmd,NULL,Outtime,Try);    
}


/**
  * @brief  ��ѯCCID����
  * @retval None
  */
void  Modem_Send_cmd_CCID(u8 *pCmd,u8 Outtime, u8 Try)
{
      Modem_Send_cmd(MODWT_CCID,pCmd,NULL,Outtime,Try);  
}
/**
  * @brief  ��modem����,��ѯ��ǰ����״ָ̬��
  * @param  *pCmd: ָ��Ҫ���͵�ATָ�
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @param  nSoc: ��ѯ��soc�ţ�0xff��ʾȫ��.
  * @retval ���ش�����.
  */
void  Modem_Send_cmd_IpSta(u8 Outtime, u8 Try)
{
    u8 *pCmd =NULL;
    u8  Tab[20];
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        pCmd = "AT+QISTAT\r";
    }                  
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        sprintf((char*)Tab,"AT+ZIPSTATUS=%d\r",ZTE_IpStaNum);
        pCmd = Tab;
    }

    Modem_Send_cmd(MODWT_IPSTAT,pCmd,NULL,Outtime,Try);
    
}
/**
  * @brief  ��ָ����·
  * @param  index: Ҫ�򿪵�����soc��
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @retval None
  */
void  Modem_Send_cmd_IpOpen(u8 index,u8 Outtime) 
{
    u8  *pCmd,*pRec;
    u8  Tab_Mode[5]={0};
    u8  Tab_Addr[30]={0};
    u8  Tab_Port[7]={0};
    u8  Tab_t[50]={0};
    u8  Tab_r[30]={0};
    u8  NodNum;
 
    ////////////////////////////////////////////////////����Ƿ�������·�ɹ���
    NodNum = Modem_Uart_NodNum();
    for(;NodNum;NodNum--)
    {
        mlen = Modem_Uart_NodRet(mbuf);
        Modem_StrCmp_LisCon(mbuf,mlen);
    }
 
    Modem_Call_SocIpConf_Function[index](Tab_Mode,Tab_Addr,Tab_Port);//��ȡip��ַ�Ͷ˿�
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {
        //AT+QIOPEN=0,"TCP","121.15.212.251",8666
        sprintf((char*)Tab_t,"AT+QIOPEN=%d,\"%s\",\"%s\",%s\r",
                             index,Tab_Mode,Tab_Addr,Tab_Port);
    }                
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        if(strncmp((char *)Tab_Mode,"UDP",3) == 0)
        {
            sprintf((char*)Tab_t,"AT+ZIPSETUP=%d,%s,%s\r",
                                index,Tab_Addr,Tab_Port);
        }
        else
        {
            //AT+ZIPSETUP=0,121.15.212.251,8666
            //+ZTCPESTABLISHED:0
            sprintf((char*)Tab_t,"AT+ZIPSETUP=%d,%s,%s\r",
                                index,Tab_Addr,Tab_Port);
        }
    }
    strcpy((char*)Tab_r,"OK\r");
    pRec = Tab_r;
    pCmd = Tab_t;
    Modem_Send_cmd(MODWT_IPOPEN,pCmd,pRec,Outtime,1);//���÷���ģʽ�����Դ���
    #ifdef MODEM_DEBUG_H
    if(ModDbInfType == MODDB_BASE)
    {
        MODDB_PRTFBAS("modem->Soc = %d\r\n",index);
        MODDB_PRTFBAS("modem->IP  = %s\r\n",Tab_Addr);    
        MODDB_PRTFBAS("modem->Port= %s\r\n",Tab_Port);
        MODDB_PRTFBAS("modem->Mode= %s\r\n\r\n",Tab_Mode);
    }
    #endif   
   
}
/**
  * @brief  �ر�ָ��soc��·
  * @param  index: Ҫ�򿪵�����soc��
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval None
  */
void  Modem_Send_cmd_IpClose(u8 index,u8 Outtime, u8 Try)
{
    u8  *pCmd,*pRec;
    u8  Tab_t[50]={0};
    u8  Tab_r[30]={0};
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        // 3, CLOSE OK
        sprintf((char*)Tab_t,"AT+QICLOSE=%d\r",index);
        sprintf((char*)Tab_r,"%d, CLOSE OK\r",index);
    }    
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        sprintf((char*)Tab_t,"AT+ZIPCLOSE=%d\r",index);
        sprintf((char*)Tab_r,"+ZTCPCLOSED:%d\r",index);//+ZTCPCLOSED:0
    } 

    pCmd = Tab_t;
    pRec = Tab_r;
    Modem_Send_cmd_Base(pCmd,pRec,Outtime,Try);
    #ifdef MODEM_DEBUG_H
    if(ModDbInfType == MODDB_BASE)
    {
        MODDB_PRTFBAS("modem->Soc=%d Close Do\r\n",index);
    }
    #endif       
}
/**
  * @brief  ����ָ����������.
  * @param  SocNum : Soc��.
  * @param   len   : ָ��Ҫ�������ݵĳ���.  
  * @param  Outtime: ָ�ʱʱ��,��λs
  * @retval None.
  */
void  Modem_Send_cmd_IpData(u8 SocNum, u16 len, u8 Outtime)
{
    u8  *pCmd;
    u8  Tab_t[30];  
    
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {
        sprintf((char*)Tab_t,"AT+QISEND=%d,%d\r",SocNum,len);
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        sprintf((char*)Tab_t,"AT+ZIPSEND=%d,%d\r",SocNum,len);
    }  
    pCmd = Tab_t;
    Modem_Send_cmd_Only(pCmd);//����AT����
    modInf.Num_SocTx[SocNum]++;//��������
}

/*
  * @brief  ��¼ftp������
  * @param  type: 0-> �û���  (������ʱΪ(IP���˿ڡ��û���������))
  *               1-> ����
  *               2-> IP���˿�
  *
  * @param  Outtime: ָ�ʱʱ�� 
  * @param  Try    : ָ�����Դ���
  * @retval None
  */
void  Modem_Send_cmd_FtpLogin(u8 type, u8 Outtime, u8 Try)
{
    u8  *pCmd,*pRec;
    u8  Tab_t[60]={0};
    u8  Tab_r[20]={0};
    
    if(Modem_Type == MODEM_TYPE_Q)//��Զ
    {
        if(type == 0)
        {
            sprintf((char*)Tab_t,"AT+QFTPUSER=\"%s\"\r",modftp.user);
            strcpy((char*)Tab_r,"OK\r");
        }
        else if(type == 1)
        {
            sprintf((char*)Tab_t,"AT+QFTPPASS=\"%s\"\r",modftp.pass);
            strcpy((char*)Tab_r,"OK\r");
        }
        else if(type == 2)
        {
            sprintf((char*)Tab_t,"AT+QFTPOPEN=\"%s\",%s\r",modftp.ip,modftp.port);
            strcpy((char*)Tab_r,"+QFTPOPEN:0\r");
        }
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)//����
    {
        if(type == 0)
        {
            sprintf((char*)Tab_t,"AT+ZFTPLOGIN=%s,%s,%s,%s\r",
            modftp.ip,modftp.port,modftp.user,modftp.pass);
            strcpy((char*)Tab_r,"+ZFTPCTL:OPENED\r");
        }
    }
    modftp.state = 1;//��¼
    pCmd = Tab_t;
    pRec = Tab_r;
    Modem_Send_cmd_Base(pCmd,pRec,Outtime,Try);
}
/*
  * @brief  ftp�����������ļ�
  * @param  type: 0-> ����·��  (������ʱΪ(·�����ļ���))
  *               1-> �����ļ�����
  *
  * @param  Outtime: ָ�ʱʱ�� 
  * @param  Try    : ָ�����Դ���
  * @retval None
  */
void  Modem_Send_cmd_FtpDown(u8 type, u8 Outtime, u8 Try)
{   
    u8  *pCmd,*pRec;
    u8  Tab_t[70]={0};
    u8  Tab_r[20]={0};
    
    if(Modem_Type == MODEM_TYPE_Q)//��Զ
    {
        if(type == 0)
        {
            sprintf((char*)Tab_t,"AT+QFTPPATH=\"%s\"\r",modftp.fpath);
            strcpy((char*)Tab_r,"+QFTPPATH:0\r");  
        }
        else if(type == 1)
        {
            sprintf((char*)Tab_t,"AT+QFTPGET=\"%s\"\r",modftp.fname);
            strcpy((char*)Tab_r,"OK\r");
        }
    }
    else if(Modem_Type == MODEM_TYPE_ZTE)//����
    {
        if(type == 0)
        {
            sprintf((char*)Tab_t,"AT+ZFTPDOWN=%s%s,0\r",modftp.fpath,modftp.fname);
            strcpy((char*)Tab_r,"OK\r");  
        }
    }
    modftp.state = 2;//����   
    pCmd = Tab_t;
    pRec = Tab_r;
    Modem_Send_cmd_Base(pCmd,pRec,Outtime,Try);
}    
/**
  * @brief  ��modem���Ͷ��Ŵ���ָ��,��Ҫ�ǻ���ָ��
  * @param  *pCmd: ָ��Ҫ���͵�ATָ�
  * @param  Outtime: ָ�ʱʱ��,��λs.
  * @param  Try: ָ���ط�����.
  * @retval ���ش�����.
  */
void  Modem_Send_cmd_Sms(u8 type ,u8 Outtime, u8 Try)
{  
    u8 *pCmd,*pRec;
    u8  Tab_t[30];
    u8  Tab_r[5]={"OK\r"};
    u8  cmdonly=0;

    ////////////////////////////////////////////////////////////////////���ø�ʽ
    if(type == 0)
    {
        strcpy((char*)Tab_t,"AT+CMGF=1\r");//Ĭ���ı�ģʽ
        if((modsms.send)&&(modsms.typs== SMS_PDU))
        {
            strcpy((char*)Tab_t,"AT+CMGF=0\r");//PDU��ʽ
        }
        else if((modsms.rec)&&(modsms.typr== SMS_PDU))
        {
            strcpy((char*)Tab_t,"AT+CMGF=0\r");//PDU��ʽ
        }
    }
    //////////////////////////////////////////////////////////////////�����ַ���
    else if(type == 1)
    {
        if(Modem_Type == MODEM_TYPE_Q)
        {              
            strcpy((char*)Tab_t,"AT+CSCS=\"GSM\"\r");//ʹ��GSM�ַ���
        }
        else if(Modem_Type == MODEM_TYPE_ZTE)/////����
        {
            strcpy((char*)Tab_t,"AT+ZMSGL=1,2\r");//ʹ��GSM�ַ���
        } 
    }
    ////////////////////////////////////////////////////////////////////������Ϣ
    else if(type == 2)
    {
        cmdonly = 1;
        if(modsms.typs == SMS_PDU)
        {
            sprintf((char*)Tab_t,"AT+CMGS=%d\r",modsms.pdulen);
        }
        else if(modsms.typs == SMS_TXT)
        {
            sprintf((char*)Tab_t,"AT+CMGS=\"%s\"\r",modsms.num);
        }
    }
    ////////////////////////////////////////////////////////////////////��ѯ����
    else if(type == 3)
    {
        strcpy((char*)Tab_t,"AT+CPMS?\r");//��ѯδ������
    }    
    ////////////////////////////////////////////////////////////////////ɾ������
    else if(type == 4)
    {
        strcpy((char*)Tab_t,"AT+CMGD=1,4\r");//��ѯδ������
    }  
    ////////////////////////////////////////////////////////////////////////////
    pCmd = Tab_t;
    pRec = Tab_r; 
    if(cmdonly)
    {
        Modem_Send_cmd_Only(pCmd);
        return;
    }
    Modem_Send_cmd_Base(pCmd,pRec,Outtime,Try);

}
/**
  * @brief  ����TTS����
  * @param  None.
  * @retval None.
  */
void  Modem_Tts_Cmd (u8 *pCmd)
{
    send_cmd(pCmd);
}
/**
  * @brief  Modem tts ����
  * @param  None.
  * @retval None.
  */
void Modem_Tts_Run (void) 
{
    static u8 kongdebug;

    if(kongdebug)
    {  
#if 1
        kongdebug = 0;
        Modem_Api_Tts_Play(0,"1234�������������¼����������޹�˾",0);
        //Modem_Api_SmsSend(SMS_TXT,"15361575332","1234567",7);
#else
        MODERR_TYPE modemack;
        modemack = Modem_Api_Ftp("113.105.77.214","21","dbftp","0000",\
                                 "/","HB-EGS701GB_BZ-V1.01.32.3U.bin");//HB-EGS701GB_BZ-V1.01.32.3U.bin//ftptest.txt
        //modemack = Modem_Api_Socket_Close(9);
        if(modemack == MOD_OK)
        {                  
            kongdebug = 0;
        }
        else if(modemack == MOD_ER)
        {
            kongdebug = 0;
        }
#endif        
    }
    
    Modem_State_Tts_Pro();
}

/**
  * @brief  Modem character process event handler.
  * @param  None.
  * @retval None.
  */
void Modem_Process (void) 
{
    u8 return_flg=0;
    
      if(0 == strncmp("+CME ERROR: 10",(const char *)mbuf,14))//dxl,2015.11.5
        {
          SimCardInsertFlag = 0;
        }

    if((Modem_StrCmp_LisFtp(mbuf,mlen)  == __TRUE))
    {
        memset(mbuf,0,sizeof(mbuf));
        mlen = 0;
        return;
    }       
    /////////////////////////////////////////////////////////  
    if((Modem_StrCmp_ListenOn(mbuf,mlen)  == __TRUE))//����ģ�鿪��
    {
        return_flg = 1;
    }
    /////////////////////////////////////////////////////////  
    if((Modem_StrCmp_ListenFalt(mbuf,mlen)  == __TRUE))//����ģ���쳣
    {
        return_flg = 1;
    }
    /////////////////////////////////////////////////////////  
    if((Modem_StrCmp_ListenSms(mbuf,mlen)  == __TRUE))//�¶��ż���
    {
        return_flg = 1;//����Ϣ������ݲ�����
    }
    /////////////////////////////////////////////////////////  
    if((Modem_StrCmp_ListenPhone(mbuf,mlen)  == __TRUE))//���������
    {
        return_flg = 1;
    } 
    /////////////////////////////////////////////////////////  
    if((Modem_StrCmp_ListenTts(mbuf,mlen)  == __TRUE))//TTS������ϼ���
    {
        return_flg = 1;
    }     
    /////////////////////////////////////////////////////////  
    if((Modem_StrCmp_ListenData(mbuf,mlen)  == __TRUE))//socket״̬����
    {
        return_flg = 1;
    }

    if(return_flg)
    {
        memset(mbuf,0,sizeof(mbuf));
        mlen = 0;
        return;
    }
    ////////////////////////  start wait_for  //////////////////////////////////    
    if(modem.wait == MODWT_CMD)/* һ��ATָ��ָ��� */ 
    {
        if (Modem_StrCmp_Base(mbuf,mlen) == __TRUE)
        {   
            modem.wait  = MODWT_IDLE; 
            modem.delay = 0;
            modem.index++;
        }         
    }
    /////////////////////////////////////////////////////////
    else if(modem.wait == MODWT_SEARCH)/* ����ָ���*/
    {
        if (Modem_StrCmp_Search(mbuf,mlen) == __TRUE)
        {   
            modem.wait  = MODWT_IDLE; 
            modem.delay = 0;
            modem.index++;
        } 
    }
    /////////////////////////////////////////////////////////
    else if(modem.wait == MODWT_CCID)
    {
        if (Modem_StrCmp_Ccid(mbuf,mlen) == __TRUE)
        {   
            modem.wait  = MODWT_IDLE; 
            modem.delay = 0;
            modem.index++;
        } 
    }    
    /////////////////////////////////////////////////////////  
    else if (modem.wait == MODWT_ATI)/* �ɼ�Modem��Ϣ,�����ͺš�����汾�� */ 
    {                               
        if (Modem_StrCmp_Ati(mbuf,mlen)  == __TRUE)
        {   
            modem.wait = MODWT_IDLE; 
            modem.delay = 0;
            modem.index++;
        }
    } 
    /////////////////////////////////////////////////////////  
    else if (modem.wait == MODWT_LOCIP)/* �ɼ���վ�����IP */ 
    {
        if (Modem_StrCmp_IpLoc(mbuf,mlen,Modem_Port_Call_LocIp)  == __TRUE)
        {   
            modem.wait = MODWT_IDLE; 
            modem.delay = 0;
            modem.index++;
        }
    }   
    ////////////////////////////////////////////////////////////////////////////
    if(modem.state== MODEM_ON)/*����״̬�´���IPָ��*/
    {
        //////////////////////////////////////////////////////////
        if (modem.wait == MODWT_IPSTAT)/* ��ѯ��ǰ����״̬ */
        {
            if(Modem_StrCmp_IpSta(mbuf,mlen)  == __TRUE)
            {
                modem.wait = MODWT_IDLE; 
                modem.delay = 0;
                modem.index++;  
            }
        }
        ////////////////////////////////////////////////////////////
        else if(modem.wait == MODWT_IPOPEN)/*��ָ��IP����*/
        {
            if((Modem_StrCmp_IpOpen(mbuf,mlen)  == __TRUE))
            {
                modem.wait = MODWT_IDLE; 
                modem.delay = 0;
                modem.index++;         
            }
        }
        ////////////////////////////////////////////////////////////
        else if ((modem.wait == MODWT_SEDATA)||(modem.wait == MODWT_NO))
        {
            if ((Modem_StrCmp_SendData(mbuf,mlen)  == __TRUE))
            {   
                modem.wait = MODWT_IDLE; 
                modem.delay = 0;
                modem.index++; 
            }      
        }
        ///////////////////////////////////////////////////////////        
        else if (modem.wait == MODWT_SMS)/* ���Ŵ��� */  
        {     
            if (Modem_StrCmp_Sms(mbuf,mlen)  == __TRUE)
            {   
                modem.wait = MODWT_IDLE; 
                modem.delay = 0;
                modem.index++;
            }
        }
       ///////////////////////////////////////////////����绰�����ȴ��Է���ͨ       
        if ((modem.onsta == MODON_PHONE)&&(modphone.state == MODPH_ATD))
        {     
            /* 'modem_run()' is waiting for modem reply */
            if (Modem_StrCmp_Atd(mbuf,mlen)  == __TRUE)
            {   
                modem.wait = MODWT_IDLE; 
                modem.delay = 0;
                modem.index++;
            }
        }
    }    
    ////////////////////////  end wait_for  ////////////////////////////////////
    memset(mbuf,0,sizeof(mbuf));
    mlen = 0;
  /* Modem not connected, return FALSE */
}


/**
  * @brief  modem���մ�����Ҫ�жϻ������Ƿ���Ҫ���������
  * @param  None.
  * @retval None.
  */
void Modem_Rec_Handle(void)
{  
    if(Modem_Uart_NodNum())
    {
        for(mlen=1;mlen!=0;Modem_Process())
        {
            modem.atack = 0;
            mlen = Modem_Uart_NodRet(mbuf);
            
        }
    }
}

/*
  * @brief  modemָ���ط�����
  * @param  None
  * @retval ��������ԭ��
  */
static MODERR_TYPE  Modem_Cmd_Try(void)
{       
    if(modem.wait > MODWT_NO)
    {
        modat.tryadd++;

        ////////////////////////////////////////////////////////////////////////���Դ���
        if(modat.tryadd < modat.trynum)
        {
            modem.delay = modat.outtime;//���ó�ʱʱ��         
            send_cmd(modat.sendbuf);//���·�������
            return MOD_TMOT;
        }
        ////////////////////////////////////////////////////////////////////////����ʧ��
        else
        {
            modem.wait   = MODWT_IDLE;
            modapi.index = MODEM_ATERR;//֪ͨAPI�ӿڴ���
            modem.index  = MODEM_ATERR;//ִ��ָ���������ΪAT�������
        }
    }
    return MOD_IDLE;
}

/**
  * @brief  ���modem�������,ÿ����һ��
  * @param  None.
  * @retval None.
  */
void Modem_Chk_Run(void)
{
    static u32 LastErrorTime = 0;
    static u8 secdly;
    static u8 ResetCount = 0;
    u8 i;
    u32 CurrentErrorTime;

    //////////////////////////////////////////////////////////////////////����ʱ
    if((secdly++) < MODEM_TSEC)
    {
        return;
    }
    secdly = 0;
    
    ////////////////////////////////////////////////////////////////////�ں˼�ʱ
    modInf.Tim_Core ++;//�ں�

    ////////////////////////////////////////////////////////////////////��·�쳣    
    modInf.Tim_SocEr++;//ʱ��
    if((modInf.Tim_SocEr > 1800)&&(0==NetFirstManualCloseFlag))//1800 30����
    {
         modInf.Tim_SocEr = 0;
        //Modem_State_Switch(MODEM_ERROR);//���������
        Communication_Init();//dxl,2015.4.1,�����Ժ���ϵ������Ĵ�������һ����������ֺܳ�ʱ�䲻���ߵ�����
        CurrentErrorTime = RTC_GetCounter();
        if((CurrentErrorTime - LastErrorTime) <= 3600)//һ��Сʱ�ڳ���1�θ�λ����������1,�����ﵽ10���Ժ���������
        {
            ResetCount++;
            if((ResetCount >= 10)&&(0==Io_ReadStatusBit(STATUS_BIT_ACC)))
            {
                ResetCount = 0;
                NVIC_SystemReset();
            }
        }
        else//��1��ʱ��������1Сʱ���������0
        {
            ResetCount = 0;
        }
        LastErrorTime = CurrentErrorTime;
    }
    
    ///////////////////////////////////////////////////////////////modem���߼�ʱ
    if(modem.state == MODEM_ON) 
    {
        modInf.Tim_Onln++;//����ʱ��
    }
    else
    {
        modInf.Tim_Dwln++;//����ʱ��
    }
    //////////////////////////////////////////////////////////////socket���߼�ʱ
    for(i=0;i<MODEM_SOC_NUM;i++)
    {
        if(Modem_IpSock_STA[i] == MODSOC_ONLN)
        {
            modInf.Tim_SocOn[i]++;//����ʱ��
            modInf.Tim_SocDw[i] = 0;
        }
        else
        {
						if(modem.csq > 10)
            {
							if(modInf.Tim_SocDw[i]++ > 20*60*15)//��������ʱ�� ����15����
							{
								modInf.Tim_SocDw[i] = 0;
								Modem_State_Switch(MODEM_ERROR);//GSMģ������
							}
						}
        }
    }


}

/**
  * @brief  This is a main thread for MODEM Control module. It is called on every
            system timer timer tick to implement delays easy.
  * @param  None.
  * @param  None.
  * @param  None.
  * @retval None.
  */
void  Modem_Run (void)
{
   static u32 tts_play_flag = 0;
	 static u32 tts_play_count = 0;
	
    Modem_Uart_NodOut();               /*�ڵ㳬ʱ����                         */
    
    Modem_Rec_Handle();                /*����modem���յ�����                  */
	
	  if(modem.onsta != MODON_IPSEND)//dxl,2017.4.12,����GPRS����ʱ���ܲ�TTS
    {
        Modem_Tts_Run();                   /*TTS������������                      */ 
			  if(modtts.state == MODTTS_PLAY)//dxl,2017.4.12,����TTSʱ������������
				{
             tts_play_flag = 1;
        }
				if(1 == tts_play_flag)
				{
					   tts_play_count++;
					   if(tts_play_count >= 30)
						 {
                 tts_play_flag = 0;
							   tts_play_count = 0;
             }
             return ;
        }
		}
		else
		{
         tts_play_flag = 0;
			   tts_play_count = 0;
    }

    Modem_Chk_Run();                   /*���modem�������                    */
    
    #ifdef MODEM_DEBUG_H
    Modem_Debug_Run();                 /*modem���Դ�����                    */ 
    #endif

    if (modem.delay)                   /*��ʱ����                             */ 
    {
        if (--modem.delay) 
        {
            return;
        }
    }

    if(Modem_Cmd_Try() != MOD_IDLE)    /*modemָ���ط�����                    */
    {
        return;
    }

    Modem_State_CheckRun();            /*Modem״̬�����                      */
    
    switch (modem.state)               /*״̬������                           */ 
    {
        ////////////////////////////////////////////////////////////////ģ�����
        case MODEM_IDLE:
            {
                ;
            }
            break;
        //////////////////////////////////////////////////////////////Ӳ����ʼ��
        case MODEM_BSP:
            {
                modem.state = MODEM_INIT;
                Modem_Type  = MODEM_TYPE_IDE;//ϵͳ����Ĭ�Ͽ���
                Modem_Uart_Init();
                Modem_Bsp_Init();
                #ifdef MODEM_DEBUG_H
                Modem_Debug_Init();
                #endif                
                Modem_PrtfTo(MODPRTF_BSP);
                Modem_PrtfTo(MODPRTF_STAR);
                #ifdef EYE_MODEM
                Modem_Bsp_SetPower(0);//�رյ�Դ
                Modem_Delay(15);//15��Ӳ��ʱ
                #endif
            }
            break;     
        //////////////////////////////////////////////////////////////���ݳ�ʼ��
        case MODEM_INIT:
            {
                Modem_State_Init();
            }         
            break;
        ////////////////////////////////////////////////////////////////�ر�ģ��
        case MODEM_OFF:
            {
                              if(0 == SimCardInsertFlag)//dxl,2015.11.5
                                {
                                  modem.state = MODEM_ON;
                                }
                                else
                                {
                                  Modem_State_Off(); 
                                }
            }
            break;
        ////////////////////////////////////////////////////////////////����ģ��
        case MODEM_ACT:
            {          
                #if 0
                Modem_Debug_CtrOn();
                Modem_Bsp_SetPower(1);/*��modem��Դ*/
                Modem_Bsp_SetActive(0);/*���ͼ�������*/
                Modem_Bsp_MicOn();
                Modem_Bsp_SpkOn(BSP_SPK_TALK);
                #else
                Modem_State_Act(MODEM_ATI,MODEM_ERROR); 
                #endif
            }
            break;
        ////////////////////////////////////////////////////////////////��ѯ����
        case MODEM_ATI:
            {
                Modem_State_Ati(MODEM_SETAUD,MODEM_ERROR); 
            }
            break; 
        ////////////////////////////////////////////////////////////////������Ƶ          
        case MODEM_SETAUD:
            {
                Modem_State_Aud(MODEM_NETS,MODEM_ERROR);
            }
            break; 
        ////////////////////////////////////////////////////////////////��ѯ����             
         case MODEM_NETS:
            {
                Modem_State_NetSer(MODEM_SMS,MODEM_ERROR);
            }
            break;
        ////////////////////////////////////////////////////////////////���Ŵ���  
        case MODEM_SMS:
            {
                Modem_State_Sms(MODEM_NETC,MODEM_ERROR);
            }
            break;                 
        ////////////////////////////////////////////////////////////////��������            
        case MODEM_NETC:
            {
                Modem_State_NetCfg(MODEM_DIAL,MODEM_ERROR);
            }
            break;  
        /////////////////////////////////////////////////////////////////PPP����
        case MODEM_DIAL:
            {
                Modem_State_Dial(MODEM_ON,MODEM_ERROR);
            }
            break; 
        /////////////////////////////////////////////////////////////////TTS����            
        case MODEM_TTS:
            {                
                Modem_State_Tts(MODEM_ERROR,MODEM_ERROR);
            }
        break;    
        //////////////////////////////////////////////////////���ߴ���(�����ڴ�)        
        case MODEM_ON:
            {                
                Modem_State_OnLine();
            }
        break;
        ////////////////////////////////////////////////////////////////ģ���쳣
        case MODEM_ERROR:
            {
                              if(0 == SimCardInsertFlag)//dxl,2015.11.5
                                {
                                  modem.state = MODEM_ON;
                                }
                                else
                                {
                                  Modem_State_Err(MODEM_OFF);
                                }
            }
            break;
        case MODEM_SYSRST:///////////////////////////////////////////////////ϵͳ��λ
            {
                NVIC_SystemReset();
            }
            break;            
    }
    ///////////////////////////////////////////////////////////////////////////

    /////////////////////////////////  end   //////////////////////////////////


}



/**
  * @brief  

  * @retval ģ������״̬��1:ģ������ 0:ģ�鲻����
  */
u8 ModemState(void)
{
    if(MODEM_ON == modem.state)
    {
        return  1;
    }
    else
    {
        return 0 ;
    }
}


