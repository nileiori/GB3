/*
********************************************************************************
*
*                                                              
*
* Filename      : modem_debug.c 
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-11-07  
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
#include "modem_debug.h"
#include "VDR_Usart.h"
#else
#include  <app_cfg.h>
#include  <includes.h>
#endif


/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/
#ifdef MODEM_DEBUG_H
/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/
#define  MODEM_DEBUG_RX_SIZE   (1024)//���ջ����С
#define  MODEM_DEBUG_NDRX_SIZE (5)//���յ��������
#define  MODDB_PRTF_INF_SIZE   (20)//״̬��Ϣ��С
#define  MODDB_SOC_NUM         (MODEM_SOC_NUM-1)//ʹ�����һ������

/*
********************************************************************************
*                          DATA TYPES
********************************************************************************
*/
const char ModDbPrtf[MODPRTF_MAX][35]={
"modem->����                        ",// 0
"modem->���ִ���                    ",// 1
"modem->�ȴ�Ӧ�ò��...           ",// 2
"modem->��ʼ����                    ",// 3
"modem->����׼������...             ",// 4
"modem->��ʼ��                      ",// 5
"modem->�򿪵�Դ                    ",// 6
"modem->�رյ�Դ                    ",// 7
"modem->�ȴ�AT��Ӧ                  ",// 8
"modem->������ͬ��                  ",// 9
"modem->�ȴ�����...                 ",// 10
"modem->��ѯSIM��״̬?              ",// 11
"modem->��ѯCCID?                   ",// 12
"modem->��ѯCREGע�����?           ",// 13
"modem->��ѯCGREGע�����?          ",// 14
"modem->��ѯ�������ĺ���?           ",// 15
"modem->���ö��Ŵ洢��              ",// 16
"modem->���ö�����ʾ                ",// 17
"modem->��ѯ����״̬?               ",// 18
"modem->PPP����...                  ",// 19
"modem->��ȡ����IP...               ",// 20
"modem->���ųɹ�,������!!!          ",// 21
"modem->Data Rec  Ok!               ",// 22
"modem->Data Send Ok!               ",// 23
"modem->Data Send Fail!             ",// 24
"modem->Data Send Err!              ",// 25
"modem->Ftp���سɹ�!                ",// 26
"modem->Ftp����ʧ��!                ",// 27
"modem->Ftp����!!!!!                ",// 28
"modem->�ȴ�������Ӧ��...           ",// 29
"modem->�������ɹ�Ӧ��!!!           ",// 30
"modem->ע��!!!��������Ӧ��!!!      ",// 31
};


const char ModDbMtyp[MODEM_TYPE_MAX][MODDB_PRTF_INF_SIZE]={
"->.....IDLE.....<-\r\n",
"->...Quectel....<-\r\n",
"->......ZTE.....<-\r\n",
};

const char ModDbInfSta[MODEM_Max][MODDB_PRTF_INF_SIZE]={
"->MODEM_IDLE    <-\r\n",
"->MODEM_BSP     <-\r\n",
"->MODEM_INIT    <-\r\n",
"->MODEM_ACT     <-\r\n",
"->MODEM_OFF     <-\r\n",
"->MODEM_ERROR   <-\r\n",
"->MODEM_SYSRST  <-\r\n",
"->MODEM_ATI     <-\r\n",
"->MODEM_SETAUD  <-\r\n",
"->MODEM_NETS    <-\r\n",
"->MODEM_SMS     <-\r\n",
"->MODEM_NETC    <-\r\n",
"->MODEM_DIAL    <-\r\n",
"->MODEM_ON      <-\r\n",
"->MODEM_TTS     <-\r\n",
};

const char ModDbInfStaOn[MODON_MAX][MODDB_PRTF_INF_SIZE]={
"->MODON_IDLE    <-\r\n",
"->MODON_CSQ     <-\r\n",
"->MODON_IPOPEN  <-\r\n",
"->MODON_IPCLOSE <-\r\n",
"->MODON_IPSEND  <-\r\n",
"->MODON_DNSIP   <-\r\n",
"->MODON_FTP     <-\r\n",
"->MODON_SMSREC  <-\r\n",
"->MODON_SMSSEND <-\r\n",
"->MODON_PHONE   <-\r\n",   
"->MODON_OFF     <-\r\n",    
"->MODON_SYSREST <-\r\n", 
"->MODON_ERR     <-\r\n",
};

const char ModDbInfWt[MODWT_MAX][MODDB_PRTF_INF_SIZE]={
"->MODWT_IDLE    <-\r\n",
"->MODWT_NO      <-\r\n",
"->MODWT_CMD     <-\r\n",
"->MODWT_SEARCH  <-\r\n",
"->MODWT_CCID    <-\r\n",
"->MODWT_ATI     <-\r\n",
"->MODWT_LOCIP   <-\r\n",
"->MODWT_IPSTAT  <-\r\n",
"->MODWT_IPOPEN  <-\r\n",
"->MODWT_IPCLOSE <-\r\n", 
"->MODWT_SEDATA  <-\r\n",
"->MODWT_REDATA  <-\r\n",
"->MODWT_FTP     <-\r\n",
"->MODWT_SMS     <-\r\n",
"->MODWT_ATD     <-\r\n",
"->MODWT_ERR     <-\r\n",
};

const char ModDbInfPh[MODPH_MAX][MODDB_PRTF_INF_SIZE]={
"->MODPH_IDLE    <-\r\n",
"->MODPH_ATD     <-\r\n",
"->MODPH_ATA     <-\r\n",
"->MODPH_ATH     <-\r\n",
"->MODPH_RING    <-\r\n",
"->MODPH_AUDCH   <-\r\n",
"->MODPH_CLVL    <-\r\n",
"->MODPH_VTS     <-\r\n",
"->MODPH_ERR     <-\r\n",
};
/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/
static u8         ModDb_RxBuf[MODEM_DEBUG_RX_SIZE];//���ջ���
static MODLIB_BUF ModDb_RxNdNum[MODEM_DEBUG_NDRX_SIZE];//�ڵ㻺��
static MODLIB_NOD ModDb_RxNode;//���սڵ�ṹ��

static Modem_APP_STATE NetDb_State;//״̬��
static SOC_IPVAL       NetDb_IpVal;

/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/
#ifdef EYE_MODEM 
MODDB_INF  ModDbInfType = MODDB_OFF;//��������Ĭ�Ϲر�  
#else
MODDB_INF  ModDbInfType = MODDB_BASE;//����ʱĬ�ϴ�
#endif  


/*
********************************************************************************
*                          EXTERN VARIABLES
********************************************************************************
*/
extern Queue  VdrTxQ;//VDR���Ͷ���
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
  * @brief  modem����ģ���ʼ��
  * @param  None
  * @retval None
  */
void Modem_Debug_Init(void)
{   
    ModLib_NodInit(&ModDb_RxNode,ModDb_RxNdNum,MODEM_DEBUG_NDRX_SIZE,
                                    ModDb_RxBuf,MODEM_DEBUG_RX_SIZE);                        
}

/*
  * @brief  ���ڽ����ж�
  * @param  None
  * @retval None
  */
void Modem_Debug_UartIrq(u8 ch)
{
    static u16 AtEnd;//��������� \r\n
    
    ModLib_NodIns(&ModDb_RxNode,INS_HALF,&ch,1);

    AtEnd = AtEnd <<8;
    AtEnd |= ch;
    
    if(AtEnd == 0x0d0a)
    {
        ch = 0x24;//������
        ModLib_NodIns(&ModDb_RxNode,INS_ALL,&ch,1);
    }
}

/*
  * @brief  ����ָ�����ݵ����Դ���
  * @param  None
  * @retval None
  */
void Modem_Debug_UartTx(u8 *data, u16 len)
{
    #ifdef EYE_MODEM 
        #ifdef   EYE_EGS701
            //COM1_WriteBuff(data,len); dxl,2016.3.30
            QueueInBuffer(data,len,VdrTxQ);             
        #elif    defined EYE_EGS702
            TaximeterCommSendData(data,len);
        #else
            //COM1_WriteBuff(data,len);dxl,2016.3.30
	          QueueInBuffer(data,len,VdrTxQ);
        #endif
    #else
        Bsp_Uart_Send(PRT_COM,data,len);
    #endif
}
/*
  * @brief  ���ڽ���
  * @param  None
  * @retval None
  */
void Modem_Debug_UartRx(u8 *data, u16 len)
{
    ModLib_NodIns(&ModDb_RxNode,INS_ALL,data,len);
}
/*
  * @brief  �ַ���ֱ�����봮��
  * @param  None
  * @retval None
  */
void Modem_Debug_UartStr(const char *str)
{
    u8 len = strlen(str);
    
    Modem_Debug_UartTx((u8*)str,len);
}
/*
  * @brief  ���������
  * @param  None
  * @retval None
  */
void Modem_Debug_UartPro(void)
{
    const char prtfoff []="modem->��ӡ�ر�\r\n\r\n";
    const char prtfall []="modem->��ӡȫ����Ϣ\r\n\r\n";  
    const char prtfbase[]="modem->��ӡ������Ϣ\r\n\r\n"; 
    const char prtfcon []="modem->����ģ��\r\n\r\n";
    const char prtfrst []="modem->��λģ��\r\n\r\n";     
    const char prtfsta []="modem->��ǰ״̬\r\n\r\n";  
    const char prtfopn []="modem->������\r\n\r\n";       
    
    u8  cmdbuf[MODEM_DEBUG_RX_SIZE]={0};
    u16 cmdlen;

    if(ModLib_NodNum(&ModDb_RxNode)==0)
    {
        return;
    }
    cmdlen = ModLib_NodRet(&ModDb_RxNode,(u8*)cmdbuf);
    cmdbuf[--cmdlen]=0;
    ////////////////////////////////////////////////////////////////////��ӡ����
    if(ModLib_AtCmp(cmdbuf,"prtf_off")==0)
    {
        ModDbInfType = MODDB_OFF;
        Modem_Debug_UartStr(prtfoff);
    }
    else if(ModLib_AtCmp(cmdbuf,"prtf_base")==0)
    {
        ModDbInfType = MODDB_BASE;
        MODDB_PRTFSTA("%s",prtfbase);      
    }
    else if(ModLib_AtCmp(cmdbuf,"prtf_all")==0)
    {
        ModDbInfType = MODDB_ALL;
        Modem_Debug_UartStr(prtfall);
    }
    else if(ModLib_AtCmp(cmdbuf,"prtf_con")==0)
    {    
        ModDbInfType = MODDB_CON;
        Modem_Debug_UartStr(prtfcon);
    }
    else if(ModLib_AtCmp(cmdbuf,"prtf_sta")==0)
    {    
        ModDbInfType = MODDB_STA;
        MODDB_PRTFSTA("%s",prtfsta);
    }
    /////////////////////////////////////////////////////////////////////API����
    else if(ModLib_AtCmp(cmdbuf,"api_rst")==0)
    {    
        Modem_Api_Rst();//��λģ��
        MODDB_PRTFSTA("%s",prtfrst);        
    }
    else if(ModLib_AtCmp(cmdbuf,"api_open=")==0)
    {    
        cmdlen = ModLib_AtLen(cmdbuf);
        cmdbuf[cmdlen] = 0;
        cmdlen = ModLib_AtAddr(cmdbuf,',');
        cmdbuf[cmdlen++] = 0;
        strcpy((char*)NetDb_IpVal.ip,(char*)cmdbuf+9);
        strcpy((char*)NetDb_IpVal.port,(char*)cmdbuf+cmdlen);
        NetDb_State = MODEM_APP_OPEN;//������
        MODDB_PRTFSTA("%s",prtfopn);
    }   
    else if(ModLib_AtCmp(cmdbuf,"api_send=")==0)
    {
        Modem_Api_Socket_Send(MODDB_SOC_NUM,cmdbuf+9,ModLib_AtLen(cmdbuf+9));
    }    
    else if(ModLib_AtCmp(cmdbuf,"api_close")==0)
    {
        NetDb_State = MODEM_APP_CLOSE;
    }     
    else if(ModLib_AtCmp(cmdbuf,"api_tts=")==0)
    {
        cmdlen = ModLib_AtLen(cmdbuf+8);
        Modem_Api_Tts_Play(0,cmdbuf+8,cmdlen);
    }  
    else if(ModLib_AtCmp(cmdbuf,"api_stp")==0)
    {
        Modem_Api_Tts_Stop();
    }
    else if(ModLib_AtCmp(cmdbuf,"api_tcl=")==0)
    {
        Modem_Api_Tts_Clvl(atoi((char*)cmdbuf+8));
    }    
    else if(ModLib_AtCmp(cmdbuf,"api_atd=")==0)
    {
        Modem_Api_Phone_Atd(cmdbuf+8,1);
    }  
    else if(ModLib_AtCmp(cmdbuf,"api_ath")==0)
    {
        Modem_Api_Phone_Ath();
    }  
    else if(ModLib_AtCmp(cmdbuf,"api_ata")==0)
    {
        Modem_Api_Phone_Ata();
    }  
    else if(ModLib_AtCmp(cmdbuf,"api_pcl=")==0)
    {
        Modem_Api_Phone_Clvl(atoi((char*)cmdbuf+8));
    }   
    else if(ModLib_AtCmp(cmdbuf,"api_vts=")==0)
    {
        Modem_Api_Phone_Vts(cmdbuf+8);
    }    
    ////////////////////////////////////////////////////////////////////ִ�ж���
    if(ModDbInfType == MODDB_CON)
    {
        Modem_Uart_Send(cmdbuf,cmdlen);
    }
}

/*
  * @brief  ��ӡmodem��״̬
  * @param  None
  * @retval None
  */
void Modem_Debug_PrtfSta(void)
{   
    static u8 secdly;
    static u8 timcnt;
    u8   size = sizeof(ModDbInfSta[0]);
    char strbuf[100]={0};
    char tab[sizeof(ModDbInfSta[0])+5];
    u8 i;
    
    if((secdly++) < MODEM_TSEC)
    {
        return;
    }
    secdly = 0;

    if(ModDbInfType != MODDB_STA)
    {
        return;
    }
    MODDB_PRTFSTA("\r\n");
    MODDB_PRTFSTA(MODEM_VER);
    MODDB_PRTFSTA("->..Star=%03d....<-\r\n",timcnt++);
    MODDB_PRTFSTA("->..............<-\r\n");    
    MODDB_PRTFSTA("->MODDY_DLY=%04d<-\r\n",modem.delay);
    MODDB_PRTFSTA("->MODNM_INX=%04d<-\r\n",modem.index);
    MODDB_PRTFSTA("->MODEM_CSQ=%04d<-\r\n",modem.csq);
    i = Modem_State_Type(0);
    memcpy(tab,(u8*)&ModDbMtyp[i][0],size);
    tab[size] = 0;
    strcat(strbuf,tab);    
    memcpy(tab,(u8*)&ModDbInfWt[modem.wait][0],size);
    tab[size] = 0;
    strcat(strbuf,tab);
    memcpy(tab,(u8*)&ModDbInfSta[modem.state][0],size);
    tab[size] = 0;
    strcat(strbuf,tab);
    memcpy(tab,(u8*)&ModDbInfStaOn[modem.onsta][0],size);
    tab[size] = 0;
    strcat(strbuf,tab);
    memcpy(tab,(u8*)&ModDbInfPh[modphone.state][0],size);
    tab[size] = 0;
    strcat(strbuf,tab);
    MODDB_PRTFSTA("%s",strbuf);
    
    MODDB_PRTFSTA("->..............<-\r\n");
    MODDB_PRTFSTA("->TimCore=%06d<-\r\n",modInf.Tim_Core/60);
    MODDB_PRTFSTA("->TimOnln=%06d<-\r\n",modInf.Tim_Onln);
    MODDB_PRTFSTA("->TimDwln=%06d<-\r\n",modInf.Tim_Dwln);
    MODDB_PRTFSTA("->NumRest=%06d<-\r\n",modInf.Num_Rest);   
    for(i=0;i<MODEM_SOC_NUM;i++)
    {
        MODDB_PRTFSTA("->....Soc=%d.....<-\r\n",i); 
        MODDB_PRTFSTA("->TimSO=%08d<-\r\n",modInf.Tim_SocOn[i]);   
        MODDB_PRTFSTA("->TimSD=%08d<-\r\n",modInf.Tim_SocDw[i]); 
        MODDB_PRTFSTA("->NumSL=%08d<-\r\n",modInf.Num_SocLn[i]);
        MODDB_PRTFSTA("->NumSR=%08d<-\r\n",modInf.Num_SocRx[i]);
        MODDB_PRTFSTA("->NumST=%08d<-\r\n",modInf.Num_SocTx[i]);
    }
    //////////////////////////////////////////////////////////////socket���߼�ʱ
    MODDB_PRTFSTA("->.......End....<-\r\n\r\n");    
    
}

/*
  * @brief  ����IP����,����IPʱ������
  * @param  *pMode��ָ����������
  * @param  *pAddr��ָ��IP��ַ
  * @param  *pPort��ָ��˿ں�
  * @retval None
  */
void Modem_Debug_SocIP(u8 *pMode ,u8 *pAddr, u8 *pPort)
{ 
    strcpy((char*)pMode,"TCP"); //TCP��UDP
    strcpy((char*)pAddr,(char*)NetDb_IpVal.ip);   //IP��ַ������
    strcpy((char*)pPort,(char*)NetDb_IpVal.port); //�˿�
}
/*
  * @brief  ���ӻص�����,ƽ̨�·�����ʱ������
  * @param  *p��ָ�������·�������
  * @param  len�����ݳ���
  * @retval None
  */
void Modem_Debug_SocCall(u8 *p,u16 len)
{
    static u32 ModDbSocLenR;

    ModDbSocLenR += len;
}
/*
  * @brief  Soc����
  * @param  None
  * @retval None
  */
void Modem_Debug_SocRun(void)
{
    MODERR_TYPE modemack;
    
    switch(NetDb_State)
    {
        case MODEM_APP_IDLE:
            {
                ;
            }
        break; 
        case MODEM_APP_OPEN://����һ������
            {
                modemack = Modem_Api_Socket_Open(MODDB_SOC_NUM,
                                                 Modem_Debug_SocIP,
                                                 Modem_Debug_SocCall);
                if(modemack == MOD_OK)
                {
                    NetDb_State = MODEM_APP_LISTEN;//�ɹ�->�������
                }
                else if(modemack == MOD_ER)
                {
                    NetDb_State = MODEM_APP_IDLE;
                }
            }
        break;     
        case MODEM_APP_LISTEN://����һ������,���ڴ��ڸ�״̬
            {
                ;
            }
        break;        
        case MODEM_APP_CLOSE://�ر�һ������
            {   
                modemack = Modem_Api_Socket_Close(MODDB_SOC_NUM);
                if(modemack == MOD_OK)
                {
                    NetDb_State = MODEM_APP_IDLE;//�ȴ�Ӧ�ô�
                }
                else if(modemack == MOD_ER)
                {
                    Modem_Api_Rst();//ģ���쳣->����ģ��
                }
            }
        break;
        default:
        break;

    }
}
/*
  * @brief  modem���Դ�����
  * @param  None
  * @retval None
  */
void Modem_Debug_Run(void)
{   
    Modem_Debug_UartPro();
    
    Modem_Debug_PrtfSta();

    Modem_Debug_SocRun();
}


#endif
