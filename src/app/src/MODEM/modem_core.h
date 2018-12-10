/**
  ******************************************************************************
  * @file    modem_core.h 
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2013-08-20 
  * @brief   
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
#include "modem.h"
	
#ifndef __MODEM_CORE_H
#define __MODEM_CORE_H



/*
********************************************************************************
*                               DEFINES
********************************************************************************
*/


/*
********************************************************************************
*                                 ENUM
********************************************************************************
*/
///////////////////////////////////////////////////////////////////////modem����
typedef enum
{
  MODEM_TYPE_IDE=0, 
  MODEM_TYPE_Q, 
  MODEM_TYPE_ZTE,
  MODEM_TYPE_MAX,
  
} MODEM_TYPE;
///////////////////////////////////////////////////////////////////////modem״̬
typedef enum
{
    MODEM_IDLE=0, //����
    MODEM_BSP,    //Ӳ��׼��
    MODEM_INIT,   //��ʼ��
    MODEM_ACT,    //����
    MODEM_OFF,    //�ػ�
    MODEM_ERROR,  //������
    MODEM_SYSRST, //ϵͳ��λ
    MODEM_ATI,    //��ȡmodem��Ϣ
    MODEM_SETAUD, //������������
    MODEM_NETS,   //��ѯ�������
    MODEM_SMS,    //���Ŵ���
    MODEM_NETC,   //��������
    MODEM_DIAL,   //����
    MODEM_ON,     //����  
    MODEM_TTS,    //TTS����ģʽ,û��װSIM��������½���
    MODEM_Max,
}MODEM_STA;//modem״̬

////////////////////////////////////////////////////////////////////////�ȴ�״̬
typedef enum {
    MODWT_IDLE=0,  //����
    MODWT_NO,      //ֱ������
    MODWT_CMD,     //����ָ��
    MODWT_SEARCH,  //����
    MODWT_CCID,    //��ѯCCID
    MODWT_ATI,     //��ѯ����
    MODWT_LOCIP,   //��ȡIP
    MODWT_IPSTAT,  //��ѯ״̬
    MODWT_IPOPEN,  //������
    MODWT_IPCLOSE, //�ر�����  
    MODWT_SEDATA,  //��������
    MODWT_REDATA,  //��������
    MODWT_FTP,     //FTP����
    MODWT_SMS,     //���Ŵ���
    MODWT_ATD,     //����绰
    MODWT_ERR,     //������
    MODWT_MAX,
  
}MODWT_STA;//�ȴ���������

////////////////////////////////////////////////////////////////////////����״̬
typedef enum
{
    MODON_IDLE=0,  //����
    MODON_CSQ,     //��ѯ����
    MODON_IPOPEN,  //������
    MODON_IPCLOSE, //�ر�����
    MODON_IPSEND,  //��������
    MODON_DNSIP,   //��������ʹ����������IP
    MODON_FTP,     //�ļ�����
    MODON_SMSREC,  //����ҵ����,����
    MODON_SMSSEND, //����ҵ����,����
    MODON_PHONE,   //�绰ҵ����
    MODON_OFF,     //�ػ�
    MODON_SYSREST, //ϵͳ��λ
    MODON_ERR,     //����
    MODON_MAX,

} MODON_STA;//����״̬�µĴ���

////////////////////////////////////////////////////////////////////////�绰״̬
typedef enum
{
    MODPH_IDLE=0,//����
    MODPH_ATD,   //����绰
    MODPH_ATA,   //�����绰
    MODPH_ATH,   //�Ҷϵ绰
    MODPH_RING,  //����
    MODPH_AUDCH, //�����л�
    MODPH_CLVL,  //����������С
    MODPH_VTS,   //����������ʾ����ļ�ֵ
    MODPH_ERR,   //����
    MODPH_MAX,   //���   

}MODPH_STA ;//�绰ҵ��״̬��

/////////////////////////////////////////////////////////////////////////TTS״̬
typedef enum
{
    MODTTS_IDLE=0,//����
    MODTTS_PLAY,  //����TTS
    MODTTS_STOP,  //ֹͣ����
    MODTTS_CLVL,  //����TTS���� 
    MODTTS_ERR,   //����

} MODTTS_STA;//TTSҵ��״̬��

/////////////////////////////////////////////////////////////////////////SOC״̬
typedef enum
{
    MODSOC_IDLE=0,//����
    MODSOC_WAIT,  //�ȴ�
    MODSOC_DOING, //��������   
    MODSOC_ONLN,  //����
    MODSOC_ERR,   //����
  
} MODSOC_STA;//sock����״̬

////////////////////////////////////////////////////////////////////////��������
typedef enum 
{
    MOD_IDLE=0,//����
    MOD_DO,    //æµ
    MOD_OK,    //�ɹ�
    MOD_ER,    //ʧ��
    MOD_TMOT,  //�ط�
    MOD_DOWN,  //����
}MODERR_TYPE;//��������

////////////////////////////////////////////////////////////////////////��������
typedef enum
{
    SMS_PDU=0, //PDU��ʽ
    SMS_PDU7,  //7λ����
    SMS_PDU8,  //8λ����
    SMS_PDUUCS,//���ı���
    SMS_TXT,   //�ı�ģʽ
    SMS_ERR,   //��������
}MODSMS_TYPE;//modem�Ķ��Ÿ�ʽ

////////////////////////////////////////////////////////////////////////��ӡ����
typedef enum
{
    MODPRTF_IDLE,   // 0����
    MODPRTF_ERR,    // 1���ִ���
    MODPRTF_WTAPP,  // 2�ȴ�Ӧ�ò��
    MODPRTF_STAR,   // 3��ʼ����
    MODPRTF_BSP,    // 4����׼������
    MODPRTF_INIT,   // 5��ʼ��
    MODPRTF_POWON,  // 6�򿪵�Դ
    MODPRTF_POWOFF, // 7�رյ�Դ
    MODPRTF_WTAT,   // 8�ȴ�AT��Ӧ
    MODPRTF_SDAT,   // 9������ͬ��
    MODPRTF_WTRD,   // 10�ȴ�����...
    MODPRTF_CPIN,   // 11��ѯSIM��״̬
    MODPRTF_CCID,   // 12��ѯCCID
    MODPRTF_CREG,   // 13��ѯCREGע�����
    MODPRTF_CGREG,  // 14��ѯCGREGע�����
    MODPRTF_CSCA,   // 15��ѯ�������ĺ���
    MODPRTF_CPMS,   // 16���ö��Ŵ洢��
    MODPRTF_CNMI,   // 17���ö�����ʾ
    MODPRTF_CPPP,   // 18��ѯ����״̬
    MODPRTF_DPPP,   // 19PPP����
    MODPRTF_GIP,    // 20��ȡ����IP
    MODPRTF_ONLN,   // 21���ųɹ�,������!!!
    MODPRTF_RECOK,  // 22���ݽ��ճɹ�  
    MODPRTF_DATOK,  // 23���ݷ��ͳɹ�  
    MODPRTF_DATFA,  // 24���ݷ���ʧ��
    MODPRTF_DATER,  // 25���ݷ��ʹ���
    MODPRTF_FTPOK,  // 26FTP���سɹ�
    MODPRTF_FTPFA,  // 27FTP����ʧ��
    MODPRTF_FTPER,  // 28FTP���ش���
    MODPRTF_SOCWT,  // 29�ȴ�����Ӧ��
    MODPRTF_SOCOK,  // 30����Ӧ��ɹ�
    MODPRTF_SOCOT,  // 31���Ӳ�Ӧ��
    MODPRTF_MAX,    // 32 MAX
}MODPRT_TYPE;//��ӡָ����Ϣ������
/*
********************************************************************************
*                                STRUCT
********************************************************************************
*/
////////////////////////////////////////////////////////////////////AT����ṹ��
typedef struct 
{
    u8  sendbuf[80]; //���͵�AT�����
    u8  recbuf[30];  //���յ�AT�����
    u16 outtime;     //ָ�ʱʱ��
    u8  trynum;      //ָ���ط����� 
    u8  tryadd;      //ÿ�ط�һ�μ�1
}ST_MODAT;//modem����ATָ��ṹ��

/////////////////////////////////////////////////////////////////////modem�ṹ��
typedef struct 
{		
    u8  csq;      //��������
    u8  csca[20]; //�������ĺ���
    u8  apn[30];  //apn
    u8  user[30]; //��������ʱ���˺�
    u8  pass[30]; //��������ʱ������
    u8  ccid[50]; //CCID���

    u8  index;	  //ִ��ָ�����
    u16 delay;    //ָ����ʱʱ��
    u8  socclose; //Ҫ�رյ�soc��
    u8  socopen;  //Ҫ�򿪵�soc��
    u8  socdns;   //��������,0ΪIP���ӣ���0Ϊ��������
    u8  atack;    //at������Ӧ������,ģ�鲻��Ӧ��1, ��Ӧ������,��Ҫ��������
    u8  sleep;    //ģ������ʱ�䣬��Ҫ����CMDAģ��

    MODEM_STA state;//modem��״̬
    MODON_STA onsta;//����״̬�µ�״̬ 
    MODWT_STA wait; //�ȴ�����ָ��
    
}ST_MODEM;//modem��Ϣ�ṹ��

/////////////////////////////////////////////////////////////modem������Ϣ�ṹ��
typedef struct 
{
    u32 Tim_Core;//�ں���������ʱ��
    u32 Tim_Onln;//��������ʱ��
    u32 Tim_Dwln;//��������ʱ��
    u32 Tim_SocEr;//��·�쳣ʱ��
    u32 Tim_SocOn[MODEM_SOC_NUM];//������·����ʱ��
    u32 Tim_SocDw[MODEM_SOC_NUM];//������·����ʱ��
    u32 Num_SocLn[MODEM_SOC_NUM];//������·���Ӵ���
    u32 Num_SocRx[MODEM_SOC_NUM];//������·��������  
    u32 Num_SocTx[MODEM_SOC_NUM];//������·��������      
    u32 Num_Rest;//modem��������
    
}ST_MODINF;//modem���е���Ϣ

///////////////////////////////////////////////////////////////////////FTP�ṹ��
typedef struct 
{
    u8  ip[15];    //ip��ַ
    u8  port[5];   //�˿�
    u8  user[20];  //����������
    u8  pass[20];  //����������    
    u8  fpath[50]; //�ļ�·��
    u8  fname[50]; //�ļ���
    u8  state;     //״̬ 
    u8  busy;      //ftpæ��־��������ʱ��0
    u8  fend;      //�ļ����ؽ�����־
    u8  fres;      //�ļ����ؽ�� 0:���� 1:��ȷ 2:����
    u32 fsize;     //�ļ���С
}ST_MODFTP;

//////////////////////////////////////////////////////////////////////���Žṹ��
typedef struct
{
    MODSMS_TYPE  typr;//���Ÿ�ʽ
    MODSMS_TYPE  typs;//���Ÿ�ʽ ���Ͷ���
    
    u8  busy;   //��Ϣ�����־��æΪ��0������Ϊ0
    u8  rec;    //�ն��ű�־ ��0��ʾ���¶���
    u8  send;   //���ŷ��ͱ�־λ����0��ʾ�ж��ŷ���
    u16 pdulen; //PDU���ŵ����ݳ��Ƚ�����PDU��ʽ

    u16 dlen;                   //�������ݳ���
    u8  dbuf[MODEM_SMS_SENDMAX];//���Ŵ洢����
    u8  num[30];                //���ź���

}ST_MODSMS;

//////////////////////////////////////////////////////////////////////�绰�ṹ��
typedef struct 
{
    MODON_STA otherwait;
    MODPH_STA state;  //�绰����״̬
    u8  busy;         //�绰�����־��æΪ��0������Ϊ0
    u8  audch;        //����ͨ��
    u8  clvl;         //������С   
    u8  otherflg;     //ͨ��ǰmodem��æ ��־    
    u8  num[30];      //�绰����
    u8  vts[20];      //ͨ���а���    
    u8  clvlvalue[10];//һ����9�������ȼ�

}ST_MODHP;

//////////////////////////////////////////////////////////////////����״̬�ṹ��
typedef struct 
{
    MODWT_STA othwait;//�ȴ���־
    MODON_STA othsta; //״̬  
    u16 othdly;       //��ʱ
    u8  othindex;     //����
    u8  othtryadd;    //�����Դ���
    u8  othtrynum;    //���Դ���

}ST_MODOTH;//���ڱ���״̬�ĵĽ׶�

///////////////////////////////////////////////////////////////////////TTS�ṹ��
typedef struct STRUCT_MODTTS{
    u8  cmdack; //ģ��Ӧ���־  �յ�OK����
    u8  index;  //TTS��������
    u8  busy;   //TTS�����־����0Ϊ���ڲ��� ,0Ϊ�������
    u8  wait;   //TTS�ȴ���־����������״̬����ͻ   
    u8  send;   //TTS���Ͳ�����־
    u8  type;   //�������� 0Ϊ��������  1Ϊ��ʱ����
    u8  len;    //TTS����
    u8  clvl;   //TTS����
    //u8  clvlvalue[10];   //һ����9�������ȼ�  dxl,2016.8.12
	  u16 clvlvalue[10];
    u8  buf[MODTTS_SIZE];//TTS����
    MODTTS_STA  state;   //TTS����״̬
}ST_MODTTS;

////////////////////////////////////////////////////////////////////IP�����ṹ��
typedef struct
{
    u8  mode[5];//TCP��UDP
    u8  ip[50]; //IP������
    u8  port[6];//�˿�
  
}SOC_IPVAL;//IP�����ṹ��

////////////////////////////////////////////////////////AT������������ݽṹ��
typedef struct 
{
    u8  soc;      //�����Ӧ��soc��
    u16 len;      //���ݻ���������
    u8  buf[MODEM_SOC_TX_SIZE];//���ݻ��������ڴ�Ŵ���������
}SOC_ATBUF;//socket���ͻ���,AT����


/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/      
extern MODEM_TYPE   Modem_Type;

extern ST_MODAT     modat;
extern ST_MODEM     modem; 
extern ST_MODINF    modInf;
extern ST_MODFTP    modftp;   
extern ST_MODSMS    modsms;   
extern ST_MODHP     modphone;
extern ST_MODTTS    modtts;
extern SM_PARAM     modsmspdu;

extern u8  QuectelAtTtsFlag;
extern u8  Modem_ActPinOnDly;
extern u8  Modem_ActPinOffDly;

extern u8         ModSocBuf  [];
extern MODLIB_BUF ModSocNdNum[];
extern MODLIB_NOD ModSocNode [];

extern MODSOC_STA Modem_IpSock_STA[MODEM_SOC_NUM];

/*
********************************************************************************
*                               FUNCTIONS
********************************************************************************
*/
void  Modem_Printf (char *format, ...);
void  Modem_PrtfTo(MODPRT_TYPE type);
void  Modem_Run (void);
void  Modem_Delay (u8 delay);
void  Modem_Tts_Cmd (u8 *pCmd);

void  Modem_Send_cmd(MODWT_STA type,u8 *pCmd, u8 *pReply,u8 Outtime, u8 Try);
void  Modem_Send_cmd_Base(u8 *pCmd, u8 *pReply,u8 Outtime, u8 Try);
void  Modem_Send_cmd_Ati(u8 Outtime, u8 Try);
void  Modem_Send_cmd_AtiFibocom(u8 Outtime, u8 Try);
void  Modem_Send_cmd_Search(u8 *pCmd,u8 Outtime, u8 Try);
void  Modem_Send_cmd_CCID(u8 *pCmd,u8 Outtime, u8 Try);
void  Modem_Send_cmd_Apn(u8 Outtime, u8 Try);
void  Modem_Send_cmd_IpSta(u8 Outtime, u8 Try);
void  Modem_Send_cmd_IpDnsGip  (u8 *pCmd ,u8 Outtime, u8 Try);
void  Modem_Send_cmd_IpOpen(u8 index,u8 Outtime);
void  Modem_Send_cmd_IpClose(u8 index,u8 Outtime, u8 Try);
void  Modem_Send_cmd_IpLoc(u8 *pCmd ,u8 Outtime, u8 Try);
void  Modem_Send_cmd_IpData(u8 SocNum, u16 len, u8 Outtime);
void  Modem_Send_cmd_FtpLogin(u8 type, u8 Outtime, u8 Try);
void  Modem_Send_cmd_FtpDown(u8 type, u8 Outtime, u8 Try);
void  Modem_Send_cmd_Sms(u8 type ,u8 Outtime, u8 Try);
u8 ModemState(void);

#endif







