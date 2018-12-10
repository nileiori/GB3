/*******************************************************************************
 * File Name:           OpenAccount.c 
 * Function Describe:   
 * Relate Module:       ���ٿ�������
 * Writer:              Joneming
 * Date:                2013-06-17 
 * ReWriter:            Joneming
 * Date:                //���п�������֮ǰ�Ȱѿ����������赽��Ӧ��IP��ַ��,�����������Ķ˿ں��赽��Ӧ�Ķ˿ںŵ�ַ��,
                        //��������:�ն˴���IP������,�ѿ�����������ַ����IP��ַ����,�ն������ӿ���������,���ӳɹ��󷢿���ָ��
                        //���յ�����Ӧ���,�ն��ٻָ�ԭ������IP��ַ,֮���ٽ�������������
 *******************************************************************************/
#include "include.h"
////////////////////////////////
typedef enum
{
    OA_STATUS_SAVE_IP,   //����IP��ַ
    OA_STATUS_UNLOCK,    //����,
    OA_STATUS_LOCK,      //����,
    OA_STATUS_VALID_FLAG,//��Ч��־
    OA_STATUS_MAX         //
}E_OA_STATUS;

enum 
{
    ACCOUNT_TIMER_TASK,//
    ACCOUNT_TIMER_SEND,//
    ACCOUNT_TIMERS_MAX
}T_STACCOUNTTIME;

static LZM_TIMER s_stAccountTimer[ACCOUNT_TIMERS_MAX];

static unsigned char s_ucfisrtRunFlag = 0;
static unsigned char s_ucOpenAccountStatus = 0;

#define  EYENET_FIRST_SOC                         (0)//SOC��

static u8 UnlockResult =0;
/////////////////////////
void OpenAccount_RecoverOriginalIPAddress(void);
/*************************************************************
** ��������: OpenAccount_SaveStatus
** ��������: ������ٿ�������״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_SaveStatus(void)
{
    PUBLIC_SETBIT(s_ucOpenAccountStatus, OA_STATUS_VALID_FLAG);    
    Public_WriteDataToFRAM(FRAM_QUICK_OPEN_ACCOUNT_ADDR, &s_ucOpenAccountStatus,FRAM_QUICK_OPEN_ACCOUNT_LEN);
}
/*************************************************************
** ��������: OpenAccount_ReadLockStatus
** ��������: ��ȡ���ٿ�������״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_ReadLockStatus(void)
{
    unsigned char max;
    unsigned char flag = 0;    
    if(Public_ReadDataFromFRAM(FRAM_QUICK_OPEN_ACCOUNT_ADDR, &flag,FRAM_QUICK_OPEN_ACCOUNT_LEN))
    {
        max = 0;
        PUBLIC_SETBIT(max, OA_STATUS_MAX);
        if(flag>=max)
        {
            flag = 0;
        }
        else
        if(!PUBLIC_CHECKBIT(flag,OA_STATUS_VALID_FLAG))
        {
            flag = 0;
        }
        else
        if((PUBLIC_CHECKBIT(flag,OA_STATUS_UNLOCK)&&PUBLIC_CHECKBIT(flag,OA_STATUS_LOCK))
         ||(!PUBLIC_CHECKBIT(flag,OA_STATUS_UNLOCK)&&!PUBLIC_CHECKBIT(flag,OA_STATUS_LOCK)))
        {
            flag = 0;
        }         
    }
    s_ucOpenAccountStatus = flag;
    ///////////////////
    if(0==s_ucOpenAccountStatus)//
    {
        s_ucOpenAccountStatus = 0;
        PUBLIC_SETBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK);
        PUBLIC_CLRBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK);
        OpenAccount_SaveStatus();
    }    
}
/*************************************************************
** ��������: OpenAccount_GetCurLockStatus
** ��������: ��ÿ��ٿ�������״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char OpenAccount_GetCurLockStatus(void)
{
    if(PUBLIC_CHECKBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK)&&0==PUBLIC_CHECKBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK))
        return 1;
    return 0;
}
/*************************************************************
** ��������: OpenAccount_SetLockStatus
** ��������: ���ٿ�����Ϊ����״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_SetLockStatus(void)
{
    PUBLIC_SETBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK);
    PUBLIC_CLRBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK);
    OpenAccount_SaveStatus();
}
/*************************************************************
** ��������: OpenAccount_ClearLockStatus
** ��������: ���ٿ����������״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_ClearLockStatus(void)
{
    ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
    ClearTerminalAuthorizationCode(CHANNEL_DATA_2);
    ///////////////////////
    PUBLIC_SETBIT(s_ucOpenAccountStatus,OA_STATUS_UNLOCK);
    PUBLIC_CLRBIT(s_ucOpenAccountStatus,OA_STATUS_LOCK);
    OpenAccount_SaveStatus();
    LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND]);
    LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
    ///////////////////////////////////
    OpenAccount_RecoverOriginalIPAddress();
}
/*************************************************************
** ��������: OpenAccount_CheckOnlineAndSendQuickOpenAccount
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_CheckOnlineAndSendQuickOpenAccount(void)
{
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    static u8 time = 0;

     if((Modem_IpSock_STA[EYENET_FIRST_SOC] == MODSOC_ONLN)&&(First_IP() == VAL_IPTO))//ָ��ip����

      {
          if(ACK_OK==RadioProtocol_OpenAccount(channel))//
          {
              LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND]);
              LZM_PublicSetCycTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK],PUBLICSECS(1),Check_Result);
          }
      }
    
    time ++;
    if(time > 30)  //����ʧ��
    {
       Public_ShowTextInfo("������!", PUBLICSECS(3)); 
       LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND]);

    }
}
/*************************************************************
** ��������: OpenAccount_LinkToFreightPlatform
** ��������: ���ӵ�ָ���Ļ���ƽ̨
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_LinkToFreightPlatform(void)
{
     SOC_IPVAL ip_value;
     u8 tab[2];
     u8 time = 0;
     //char MainIp[] = "121.15.212.249";   //����ʹ��
     //char Port[] = "9201"; 
     char MainIp[] = "jt1.gghypt.net";
     char Port[] = "1008"; 
     /*u8 buff[20];
         memset(buff,0,20);
         EepromPram_WritePram(E2_UPDATA_DIAL_NAME_ID, 0, 20);   //��Ȩ������
    */
     strcpy((char *)ip_value.ip,MainIp);
     strcpy((char *)ip_value.port,Port);
     strcpy((char *)&ip_value.mode,"TCP");
    
     EepromPram_ReadPram(E2_UPDATA_TIME_ID, tab);
     time = (tab[0] << 8)|tab[1];
     ClrTimerTask(TIME_CLOSE_GPRS);
     Communication_Open();
     Net_First_ConTo(ip_value, time);       //�л���ָ��������
     OpenAccount_SaveStatus();
     LZM_PublicSetCycTimer(&s_stAccountTimer[ACCOUNT_TIMER_SEND],PUBLICSECS(1),OpenAccount_CheckOnlineAndSendQuickOpenAccount); 
}
/*****************************************************************
** ��������: OpenAccount_CheckAndDisposeLockStatus
** ��������: ���ʹ�����ٿ�������״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_CheckAndDisposeLockStatus(void)
{

    if(!Check_First_Register_Flag())    //��һ�ο��� fanqinghai 2015.11.23

    {
        if(0==s_ucfisrtRunFlag)
        {
            OpenAccount_ParameterInitialize();
        }
        ///////////////////////
      
        if(OpenAccount_GetCurLockStatus())//����
        {
            Public_ShowTextInfo("���ٿ�����", PUBLICSECS(20));
            OpenAccount_LinkToFreightPlatform();
        }
        else
        {
            Public_ShowTextInfo("�ն�δ����", PUBLICSECS(5));
        }

    }
    else
    {
        Public_ShowTextInfo("���������", PUBLICSECS(5));

    }
    ///////////////////////
}
/*************************************************************
** ��������: Check_Result
** ��������: ��鿪�����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
** ����:fanqinghai 2015.11.23
*************************************************************/
void Check_Result(void)
{
    static u8 time = 0 ;
    if(3 == UnlockResult)
    {
       UnlockResult = 0;
       OpenAccount_ClearLockStatus();
       LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
       Set_First_Register_Flag();                //��λ������־
       Public_ShowTextInfo("�ն˽����ɹ�", PUBLICSECS(5));
       LZM_PublicSetOnceTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK],PUBLICSECS(4),ShowMainInterface);         
    }
    else if(1 == UnlockResult)
    {
          UnlockResult = 0;
          OpenAccount_SetLockStatus();
          LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
          OpenAccount_RecoverOriginalIPAddress();  
          Public_ShowTextInfo("�ն˽���ʧ��", PUBLICSECS(5));
    }
    else if(2 == UnlockResult)   
    {
          UnlockResult = 0;
          OpenAccount_SetLockStatus();
          LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
          OpenAccount_RecoverOriginalIPAddress();  
          Public_ShowTextInfo("��Ϣ����", PUBLICSECS(5));
    }
    else
    {
        time ++ ;
        if(time > 10)
        {
            time = 0;
            UnlockResult = 0;
            LZM_PublicKillTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK]);
            Public_ShowTextInfo("��Ӧ��", PUBLICSECS(5));

        }
    }

}
/*************************************************************
** ��������: OpenAccount_RecoverOriginalIPAddress
** ��������: ���ʹ�����ٿ�������״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_RecoverOriginalIPAddress(void)
{

        First_Switch_Net_Specific_To_MainIP();
        OpenAccount_SaveStatus();
}
/*************************************************************
** ��������: OpenAccount_DisposeRadioProtocol
** ��������: ���ٿ���Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned short ID;
    unsigned char result;
    ID=Public_ConvertBufferToShort(&pBuffer[2]);
    UnlockResult = 0;
    if(0x0110==ID)//���ٿ���
    {
        result = pBuffer[4];
        if((0==result)||(5==result))      // �����ɹ�,dxl,2016.4.15������result==5��������ѿ�����Ӧ��Ϊ5��
        {
            UnlockResult = 3;
        }
        else if(1 == result)//ʧ��
        
        {
            UnlockResult = 1;
        }
        else if(2== result)    //��Ϣ����
       {
           UnlockResult = 2;
       }
        else
        {
            UnlockResult = 0;
        }
    }
}
/*************************************************************
** ��������: OpenAccount_GetEncryptFlag
** ��������: �Ƿ����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char OpenAccount_GetEncryptFlag(void)
{
    unsigned char flag;
    if(!EepromPram_ReadPram(E2_TOPLIGHT_CONTROL_ID, &flag))
    {
        flag = 0;
    }
    return (1==flag)?1:0;
}
/*************************************************************
** ��������: OpenAccount_TimeTask
** ��������: ���ٿ�����ʱ��������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState OpenAccount_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stAccountTimer,ACCOUNT_TIMERS_MAX);
    return ENABLE;
}
/*************************************************************
** ��������: OpenAccountParameterInitialize
** ��������: ���ٿ���������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void OpenAccount_ParameterInitialize(void) 
{
    LZM_PublicKillTimerAll(s_stAccountTimer,ACCOUNT_TIMERS_MAX);
    ////////////////////////////// 
    OpenAccount_ReadLockStatus();    
  /*  if(OpenAccount_GetCurLockStatus())     //fanqinghai 2015.11.18
    {
        LZM_PublicSetOnceTimer(&s_stAccountTimer[ACCOUNT_TIMER_TASK],PUBLICSECS(15),OpenAccount_CheckAndDisposeLockStatus);
    }*/
    /////////////////////////
    s_ucfisrtRunFlag = 1;
   SetTimerTask(TIME_ACCOUNT,SYSTICK_50ms);          
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

