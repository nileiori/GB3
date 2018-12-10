/**
  ******************************************************************************
  * @file    IcCardSocket.c
  * @author  fanqinghai
  * @version V1.0.0
  * @date    2016.02.22
  * @brief   ��ʻԱ�����֤���ר��
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "include.h"
//#include "IcCardSocket.h"

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
typedef struct  {

    u8 HeadStart;  //��ʾͷ
    u8 Verity;     //У��
    u8 Version[2]; //�汾��
    u8 PID[2];     //���̱��
    u8 VID;        //����ID
    u8 Type;       //��������
    u8 Cmd;        //����
    u8 Data[70];   //�û�����
    u8 HeadEnd;    //��ʾδ

}STRUCT_IC_DATA;

#define  EYENET_SECOND_SOC                         (1)//SOC��

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES   ���ر���
*********************************************************************************************************
*/
static IC_MODULE_STATE    IcCardState = IC_MODULE_IDLE;
static IC_MODULE_STATE    IcCardSwitch_State = IC_MODULE_IDLE;
static STRUCT_IC_DATA     IcStrData;

static u16 IcCardSwitch_Delay;//��ʱ�л�״̬

static u8  IcBufSendGprs[100];
static u16 IcBufSendLen;

static u8  IcBufPublic[100];//����IC����

static u8  IcSendGprsFlag;
u8 ModuleOnLine = 0;
//static u16 IcSendAckNum;//0900ר����ˮ��

static u8  IcBufDriverInfor[100];//��ʻԱ��Ϣ
static u8  IcBufDriverTime[10];//��ο�ʱ��
static u8  IcDriverFlag;//��ʻԱ��״̬

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES    ȫ�ֱ���
*********************************************************************************************************
*/
extern u16 RadioProtocolRxLen;//
extern u8  RadioProtocolRx2Buffer[];//
extern u8 TerminalAuthorizationFlag ;
/*
*********************************************************************************************************
*                                          extern VARIABLES    �ⲿ����
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/**
  * @brief  ������IC����֤���Ľ�������,IC��֤����IP��ַ�����ò����ж�ȡ��
  * @param  None.
  * @retval �ɹ�����ICAUC_RET_SUCCED,���򷵻ش�����.
  */
ICAUC_RetType ICAUC_OpenLnk(void)
{
    return ICAUC_RET_SUCCED;
}
/**
  * @brief  ��IC����֤���ķ�������
  * @param  None.
  * @retval �ɹ�����ICAUC_RET_SUCCED,���򷵻ش�����.
  */
ICAUC_RetType ICAUC_SendBuff(u8* buff,u16 len)
{

    return ICAUC_RET_SENDBUF_NO_CMD;    
}
/**
  * @brief  ע��һ����ȡ����
  * @param  pFun :���ݶ�ȡ��������.
  * @retval None
  */
void ICAUC_RegReadFun(ICAUC_FUN_READ pFun)
{

}

/**
  * @brief  ��ȡ����״̬
  * @param  None
  * @retval None
  */
u8 ICAUC_GetLnkStatus(void)
{
    return 0;
}
/**
  * @brief  IC����ʼ��
  * @param  None
  * @retval None
  */
  
void ICCard_ParameterInitialize(void)
{

    if(ReadPeripheral1TypeBit(6))   //���������� fanqinghai 2015.12.1
    {
       u8 MainIp[20]= {0};
       u8 Port[8] ={0};
       unsigned char val;

       IcCardNet_Init();   //���ⲿic��ģ���Դ
        ////////////////////    
       COMM_Initialize(COM2,9600);
       SetTimerTask(TIME_ICAUC,ICAUC_TIME_TICK);   
       IcCardNet_Open(10);        //��ic������

       memset(MainIp,0,20);
       memset(Port,0,8);
       EepromPram_ReadPram(E2_IC_MAIN_SERVER_IP_ID,MainIp);
       EepromPram_ReadPram(E2_IC_MAIN_SERVER_TCP_PORT_ID,Port);

       val=strlen((char*)MainIp);
       EepromPram_WritePram(E2_UPDATA_IP_ID, MainIp,val);
       
       EepromPram_WritePram(E2_UPDATA_TCP_PORT_ID, Port+2,2);     //ȡ��2���ֽڣ���������tcp�˿ڳ���Ϊ2 
       
    }
    else                 //ʹ���ڲ�ic��
    {
    
        ICCARD_M3_Init();
    }
        
}
void IcCardNet_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin_11);//����Դ
    
}
/*
  * @brief  ��ʱ�л�����
  * @param  None
  * @retval None
  */
void IcCardNet_Delay(void)
{
    static u32 CurrentCount     = 0;//��ǰ��ʱ����
    static u32 LastCount        = 0;//��һ�ζ�ʱ����

    CurrentCount = Timer_Val();     //׼ȷ��ʱ1����
    if((CurrentCount - LastCount) < SECOND)
    {
        return;
    }
    LastCount = CurrentCount;
    IcCardSwitch_Delay--;
    if(IcCardSwitch_Delay == 0)
    {
       IcCardState = IcCardSwitch_State;
    }
}
/**********************************************************************************
* ?? : UTY_hex2str
* ?? : ??????ASIC
* ?? : u8 dec
* ?? : u8 *psend
       :u8 ??????
� ?? : 2009/10/24
* ?? : ????,0-0
***********************************************************************************/
u8 Eye_hexstr(u8 *pStr,  u16 Value, u8 len)
{
    u8 ucTmp = 0;
    u8 i = 0, n = 0;
    u8 flag = 0;
    u8 ucBuf[6];
    u8 ascTable[] = "0123456789";
    
    u16 Base = 10000;
    
    if (pStr == NULL)
        return 0;
    
    for (i=0; i<5; i++) {
        ucTmp = Value / Base;
        Value %= Base;
        Base /= 10;
        if (flag == 0) {
            if (ucTmp != 0) {
                flag = 1;
                ucBuf[n++] = ascTable[ucTmp];
            }
        } else {
            ucBuf[n++] = ascTable[ucTmp];
        }
    }
    
    if (n == 0) {
        n = 1;
        pStr[0] = '0';
    }
    i=0;
    if(n > len) {
        len = n;
    } else if(n < len) {    //??
        for (i=0; i<(len - n); i++) {
            pStr[i] = '0';
        }
    }
    memcpy(&pStr[i], ucBuf, n);

    return len;
}

/*
  * @brief  IC����������
  * @param  *pMode��ָ����������
  * @param  *pAddr��ָ��IP��ַ
  * @param  *pPort��ָ��˿ں�
  * @retval None
  */
void IcCardNet_Main(u8 *pMode ,u8 *pAddr, u8 *pPort)
{ 

    u8  Tab[25]={0};
    u8  len=0;
    u16 port=0;
    
    memcpy(pMode,"TCP",3);
    memset(Tab,0,sizeof(Tab));
    len = EepromPram_ReadPram(E2_IC_MAIN_SERVER_IP_ID, Tab);
    if((len > 0)&&(len < 21))
    {
        memcpy(pAddr,Tab,len);//��ȡ��ȷ
    }
    else
    {
        memcpy(pAddr,"0.0.0.0",15);//����Ĭ��ֵ
    }

    memset(Tab,0,sizeof(Tab));
    len = EepromPram_ReadPram(E2_IC_MAIN_SERVER_TCP_PORT_ID, Tab);
    if((len > 0)&&(len < 5))
    {
        port = (Tab[0] << 24) + (Tab[1] << 16) + (Tab[2] << 8) + Tab[3];
        memset(Tab,0,sizeof(Tab));
        len = Eye_hexstr(Tab,port,0);
        memcpy(pPort,Tab,len);//��ȡ��ȷ
    }
    else
    {
        memcpy(pPort,"00000",5);//����Ĭ��ֵ
    }
}

/*
  * @brief  IC���ص�����,��������
  * @param  *p��ָ�������·�������
  * @param  len�����ݳ���
  * @retval None
  */
void IcCardNet_Call(u8 *p,u16 len)
{
    if (0 == RadioProtocol_GetRadioProtocolRxBufferBusyFlag()) 
    {
        RadioProtocolRxLen = len;
        memcpy(RadioProtocolRx2Buffer,p, RadioProtocolRxLen);
        RadioProtocol_AddRadioParseList();
        SetTimerTask(TIME_RADIO_PARSE, 1*SYSTICK_0p1SECOND);
    }
}
/*
  * @brief  ��ѯ�Ƿ���IC�����ݵ���֤����
  * @param  None
  * @retval None
  */
u8 IcCardNet_SendIs(void)
{
    return IcSendGprsFlag;
}
/*
  * @brief  �������IC�����ı�־
  * @param  None
  * @retval None
  */
void IcCardNet_SendClear(void)
{
    IcSendGprsFlag = 0;
}
/*
  * @brief  �Ѵ�IC�����豸��������0900͸������֤����
  * @param  None
  * @retval None
  */
void IcCardNet_SendGprs(u8 *pRec, u16 RecLen)
{
    u8  *p  = pRec;
    u16 len = RecLen;

    IcSendGprsFlag = 1;
  //  AreaIc_SendOriginalNumEx(IcSendAckNum++);
    RadioProtocol_OriginalDataUpTrans(CHANNEL_DATA_2,0x0B,p,len); 
}
/*
  * @brief  ��ƽ̨�·�8900�����������IC������
  * @param  None
  * @retval None
  */
void IcCardNet_SendValueToCom(u8 *pRec, u16 RecLen)
{
    u8  *p   = pRec;
    u16 len  = RecLen;
    u16 i,j;

    u16 Verity = 0;

    memset(IcStrData.Data,0,sizeof(IcStrData.Data));
    memcpy(IcStrData.Data,p,len);

    memset(IcBufPublic,0,sizeof(IcBufPublic));
    memcpy(IcBufPublic,(u8*)&IcStrData,sizeof(STRUCT_IC_DATA));

    Verity  = 0;
    for(i=0,j=0;i<29;i++,j++)
    {
        Verity = Verity + IcBufPublic[j+4];
    }
    IcBufPublic[1] = Verity&0xff;


    len = TransMean(IcBufPublic+1,32);
    IcBufPublic[len+1] = 0x7e;
    IcBufPublic[0]   = 0x7e;
    
    COM2_WriteBuff(IcBufPublic,len+2);
}

/*
  * @brief  ��IC�����ڷ���Ӧ������
  * @param  None
  * @retval None
  */
void IcCardNet_SendComAck(void)
{
    u16 i,j;
    u16 Verity = 0;
    u16 len;

    memset(IcBufPublic,0,sizeof(IcBufPublic));
    memcpy(IcBufPublic,(u8*)&IcStrData,sizeof(STRUCT_IC_DATA));

    Verity  = 0;
    for(i=0,j=0;i<4;i++,j++)
    {
        Verity = Verity + IcBufPublic[j+4];
    }
    IcBufPublic[1] = Verity&0xff;

    len = TransMean(IcBufPublic+1,7);
    IcBufPublic[len+1] = 0x7e;
    IcBufPublic[0]   = 0x7e;
    
    COM2_WriteBuff(IcBufPublic,len+2);
}
/*
  * @brief  ��ƽ̨���ͼ�ʻԱ�ϰ�
  * @param  None
  * @retval None
  */
void IcCardNet_SendDriverWork(void)
{
    u16 len  = 0;
    u16 temp = 0;
    u8  *p   = IcBufDriverInfor;

    memset(IcBufPublic,0,sizeof(IcBufPublic));

    if(IcDriverFlag)
    {
        IcBufPublic[len++] = 0x01;                //�ϰ�
    }
    else
    {
        IcBufPublic[len++] = 0x02;                //�°�
    }

    memcpy(&IcBufPublic[len],IcBufDriverTime,6);
    len = len + 6;

    if(IcDriverFlag == 0)
    {
        RadioProtocol_DriverInformationReport(CHANNEL_DATA_2,IcBufPublic,len);
        return;
    }

    IcBufPublic[len++] = 0x00;                        //�����ɹ�

    temp = *p;                                        //��ʻԱ����
    p = p + 1;
    IcBufPublic[len++] = temp;
    
    memcpy(&IcBufPublic[len],p,temp);                  //��ʻԱ����
    p = p + temp;
    len = len + temp;

    temp = 20;
    memcpy(&IcBufPublic[len],p,temp);                  //��ҵ�ʸ�֤��
    p = p + temp;
    len = len + temp;

    temp = *p;                                        //��֤��������
    p = p + 1;
    IcBufPublic[len++] = temp;    

    memcpy(&IcBufPublic[len],p,temp);                  //��֤����
    p = p + temp;
    len = len + temp;

    temp = 4;
    memcpy(&IcBufPublic[len],p,temp);                  //��֤ʱ��
    len = len + temp;
    
    RadioProtocol_DriverInformationReport(CHANNEL_DATA_2,IcBufPublic,len);
    
}
/*
  * @brief  ��ƽ̨���ͼ�ʻԱ�°�
  * @param  None
  * @retval None
  */
void IcCardNet_SendDriverRest(void)
{
    u16 len = 0;
    
    memset(IcBufPublic,0,sizeof(IcBufPublic));

    IcBufPublic[len++] = 0x02;//�°�

    Public_ConvertNowTimeToBCDEx(&IcBufPublic[len]);
    len = len + 6;

    RadioProtocol_DriverInformationReport(CHANNEL_DATA_2,IcBufPublic,len);
}
/*
  * @brief  IC���߱�־
  * @param  None
  * @retval None
  */
u8 Get_ICModuole_OnLine_fFlag(void)
{
 return   ModuleOnLine ; 
}
/*
  * @brief  IC�������ר��
  * @param  None
  * @retval None
  */
void IcCardNet_CmdHandle(u8 *pRec, u16 RecLen)
{
    u8  *p   = pRec;
    u16 len  = RecLen;

    /***************************************************************************/
    memset(IcBufPublic,0,sizeof(IcBufPublic));//��ȡ����
    memcpy(IcBufPublic,p,len);

    len = unTransMean(IcBufPublic,len);

    memset((STRUCT_IC_DATA*)&IcStrData,0,sizeof(STRUCT_IC_DATA));
    memcpy((STRUCT_IC_DATA*)&IcStrData,IcBufPublic,len);
    
    /***************************************************************************/
    switch(IcStrData.Type)
    {
        case 0x40:
        {
            IcCardState = IC_MODULE_LISTEN;
            IcCardNet_SendGprs(IcStrData.Data,64);
            ModuleOnLine = 1;
            IcCardState = IC_MODULE_OPEN;

            break;
        }
        case 0x41:
        {
            IcDriverFlag = 1;
            Public_PlayTTSVoiceStr("��ʻԱ�ϰ�");                   
            memcpy(IcBufDriverInfor,IcStrData.Data,sizeof(IcStrData.Data));
            Public_ConvertNowTimeToBCDEx(IcBufDriverTime);
            IcCardNet_SendDriverWork();
            IcCardNet_SendComAck();
            break;
        }
        case 0x42:
        {
            IcDriverFlag = 0;
            Public_PlayTTSVoiceStr("��ʻԱ�°�");
            Public_ConvertNowTimeToBCDEx(IcBufDriverTime);
            IcCardNet_SendDriverRest();
            IcCardNet_SendComAck();
            IcCardState = IC_MODULE_CLOSE;
            break;
        }        
    }
}

/*
  * @brief  IC����������
  * @param  None
  * @retval None
  */

void IcCardNet_Lisen(void)
{
    u16 len;
    static u8  DelayIdle;
    
    //len = COM2_ReadBuff(IcBufSendGprs+IcBufSendLen,sizeof(IcBufSendGprs));
    if(len)
    {
        IcBufSendLen = IcBufSendLen + len;
        DelayIdle = 1;
    }
    else if(DelayIdle)
    {
        DelayIdle++;
        if(DelayIdle > 3)
        {
            DelayIdle = 0;
           // EyeNet_Printf(6); 
            IcCardNet_CmdHandle(IcBufSendGprs,IcBufSendLen);
            IcBufSendLen = 0;
            memset(IcBufSendGprs,0,sizeof(IcBufSendGprs));
        }
    }
}

/*
  * @brief  ��IC������
  * @param  ��ʱ������
  * @retval None
  */
void IcCardNet_Open(u16 delay)
{
    if(IcCardState != IC_MODULE_IDLE)
    {
        return;
    }
    IcCardState = IC_MODULE_IDLE;
    Net_Second_Open();     //����2��
}
/**
  * @brief  IC����֤������������
  * @param  None.
  * @retval ��������״̬.
  */
FunctionalState ICAUC_TimeTask(void)
{   

    switch(IcCardState)
    {
        case IC_MODULE_IDLE:
            {
               IcCardNet_Lisen();
            }
        break; 
        case IC_MODULE_OPEN:    //����һ������
            {
            Second_connect_To_Specific_Server();
            IcCardState = IC_MODULE_LISTEN;
            if((Modem_IpSock_STA[EYENET_SECOND_SOC] == MODSOC_ONLN)&&(Second_IP() == VAL_IPTO))  //�ж�����2���߷�                  
            {
                Public_ShowTextInfo("������������", PUBLICSECS(3)); 
            }
            }
        break;     
        case IC_MODULE_LISTEN://����һ������,���ڴ��ڸ�״̬
            {
            if((Modem_IpSock_STA[EYENET_SECOND_SOC] == MODSOC_ONLN)&&(Second_IP() == VAL_IPTO))//ָ��ip����
            {
                TerminalAuthorizationFlag |= 0x04;    //ǿ����λ��Ȩ�ɹ���־  
            }
            else
            {
                TerminalAuthorizationFlag &= 0xfb;

            }
            IcCardNet_Lisen();
            }
        break;        
        case IC_MODULE_CLOSE://�ر�һ������
            {   
                Second_Switch_Net_Specific_To_MainIP();       //ָ���������ߣ��л���������
                IcCardState = IC_MODULE_IDLE;
        }
        break;  
        case IC_MODULE_DELAY:
            {
                IcCardNet_Delay();
            }
        break; 
              default:break ;
    }
    return ENABLE;
}

    



