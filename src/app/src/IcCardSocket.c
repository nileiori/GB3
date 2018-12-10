/**
  ******************************************************************************
  * @file    IcCardSocket.c
  * @author  fanqinghai
  * @version V1.0.0
  * @date    2016.02.22
  * @brief   ¼ÝÊ»Ô±Éí·ÝÑéÖ¤¼ì²â×¨ÓÃ
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

    u8 HeadStart;  //±êÊ¾Í·
    u8 Verity;     //Ð£Ñé
    u8 Version[2]; //°æ±¾ºÅ
    u8 PID[2];     //³§ÉÌ±àºÅ
    u8 VID;        //ÍâÉèID
    u8 Type;       //ÃüÁîÀàÐÍ
    u8 Cmd;        //ÃüÁî
    u8 Data[70];   //ÓÃ»§Êý¾Ý
    u8 HeadEnd;    //±êÊ¾Î´

}STRUCT_IC_DATA;

#define  EYENET_SECOND_SOC                         (1)//SOCºÅ

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES   ±¾µØ±äÁ¿
*********************************************************************************************************
*/
static IC_MODULE_STATE    IcCardState = IC_MODULE_IDLE;
static IC_MODULE_STATE    IcCardSwitch_State = IC_MODULE_IDLE;
static STRUCT_IC_DATA     IcStrData;

static u16 IcCardSwitch_Delay;//ÑÓÊ±ÇÐ»»×´Ì¬

static u8  IcBufSendGprs[100];
static u16 IcBufSendLen;

static u8  IcBufPublic[100];//¹«ÓÃIC»º´æ

static u8  IcSendGprsFlag;
u8 ModuleOnLine = 0;
//static u16 IcSendAckNum;//0900×¨ÓÃÁ÷Ë®ºÅ

static u8  IcBufDriverInfor[100];//¼ÝÊ»Ô±ÐÅÏ¢
static u8  IcBufDriverTime[10];//²å°Î¿¨Ê±¼ä
static u8  IcDriverFlag;//¼ÝÊ»Ô±µÄ×´Ì¬

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES    È«¾Ö±äÁ¿
*********************************************************************************************************
*/
extern u16 RadioProtocolRxLen;//
extern u8  RadioProtocolRx2Buffer[];//
extern u8 TerminalAuthorizationFlag ;
/*
*********************************************************************************************************
*                                          extern VARIABLES    Íâ²¿±äÁ¿
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/**
  * @brief  ½¨Á¢ÓÚIC¿¨ÈÏÖ¤ÖÐÐÄ½¨Á¢Á¬½Ó,ICÈÏÖ¤ÖÐÐÄIPµØÖ·´ÓÅäÖÃ²ÎÊýÖÐ¶ÁÈ¡¡£
  * @param  None.
  * @retval ³É¹¦·µ»ØICAUC_RET_SUCCED,·ñÔò·µ»Ø´íÎóÂë.
  */
ICAUC_RetType ICAUC_OpenLnk(void)
{
    return ICAUC_RET_SUCCED;
}
/**
  * @brief  ÏòIC¿¨ÈÏÖ¤ÖÐÐÄ·¢ËÍÊý¾Ý
  * @param  None.
  * @retval ³É¹¦·µ»ØICAUC_RET_SUCCED,·ñÔò·µ»Ø´íÎóÂë.
  */
ICAUC_RetType ICAUC_SendBuff(u8* buff,u16 len)
{

    return ICAUC_RET_SENDBUF_NO_CMD;    
}
/**
  * @brief  ×¢²áÒ»¸ö¶ÁÈ¡º¯Êý
  * @param  pFun :Êý¾Ý¶ÁÈ¡º¯Êý¶¨Òå.
  * @retval None
  */
void ICAUC_RegReadFun(ICAUC_FUN_READ pFun)
{

}

/**
  * @brief  »ñÈ¡Á¬½Ó×´Ì¬
  * @param  None
  * @retval None
  */
u8 ICAUC_GetLnkStatus(void)
{
    return 0;
}
/**
  * @brief  IC¿¨³õÊ¼»¯
  * @param  None
  * @retval None
  */
  
void ICCard_ParameterInitialize(void)
{

    if(ReadPeripheral1TypeBit(6))   //ÍâÉèÒÑÆôÓÃ fanqinghai 2015.12.1
    {
       u8 MainIp[20]= {0};
       u8 Port[8] ={0};
       unsigned char val;

       IcCardNet_Init();   //´ò¿ªÍâ²¿ic¿¨Ä£¿éµçÔ´
        ////////////////////    
       COMM_Initialize(COM2,9600);
       SetTimerTask(TIME_ICAUC,ICAUC_TIME_TICK);   
       IcCardNet_Open(10);        //´ò¿ªic¿¨Á¬½Ó

       memset(MainIp,0,20);
       memset(Port,0,8);
       EepromPram_ReadPram(E2_IC_MAIN_SERVER_IP_ID,MainIp);
       EepromPram_ReadPram(E2_IC_MAIN_SERVER_TCP_PORT_ID,Port);

       val=strlen((char*)MainIp);
       EepromPram_WritePram(E2_UPDATA_IP_ID, MainIp,val);
       
       EepromPram_WritePram(E2_UPDATA_TCP_PORT_ID, Port+2,2);     //È¡ºó2¸ö×Ö½Ú£¬ÎÞÏÞÉý¼¶tcp¶Ë¿Ú³¤¶ÈÎª2 
       
    }
    else                 //Ê¹ÓÃÄÚ²¿ic¿¨
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
    GPIO_SetBits(GPIOE, GPIO_Pin_11);//¿ªµçÔ´
    
}
/*
  * @brief  ÑÓÊ±ÇÐ»»º¯Êý
  * @param  None
  * @retval None
  */
void IcCardNet_Delay(void)
{
    static u32 CurrentCount     = 0;//µ±Ç°¶¨Ê±¼ÆÊý
    static u32 LastCount        = 0;//ÉÏÒ»´Î¶¨Ê±¼ÆÊý

    CurrentCount = Timer_Val();     //×¼È·¶¨Ê±1ÃëÖÓ
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
× ?? : 2009/10/24
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
  * @brief  IC¿¨Ö÷·þÎñÆ÷
  * @param  *pMode£ºÖ¸ÏòÁ¬½ÓÀàÐÍ
  * @param  *pAddr£ºÖ¸ÏòIPµØÖ·
  * @param  *pPort£ºÖ¸Ïò¶Ë¿ÚºÅ
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
        memcpy(pAddr,Tab,len);//¶ÁÈ¡ÕýÈ·
    }
    else
    {
        memcpy(pAddr,"0.0.0.0",15);//´íÎóÄ¬ÈÏÖµ
    }

    memset(Tab,0,sizeof(Tab));
    len = EepromPram_ReadPram(E2_IC_MAIN_SERVER_TCP_PORT_ID, Tab);
    if((len > 0)&&(len < 5))
    {
        port = (Tab[0] << 24) + (Tab[1] << 16) + (Tab[2] << 8) + Tab[3];
        memset(Tab,0,sizeof(Tab));
        len = Eye_hexstr(Tab,port,0);
        memcpy(pPort,Tab,len);//¶ÁÈ¡ÕýÈ·
    }
    else
    {
        memcpy(pPort,"00000",5);//´íÎóÄ¬ÈÏÖµ
    }
}

/*
  * @brief  IC¿¨»Øµ÷º¯Êý,½ÓÊÕÊý¾Ý
  * @param  *p£»Ö¸ÏòÁ¬½ÓÏÂ·¢µÄÊý¾Ý
  * @param  len£ºÊý¾Ý³¤¶È
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
  * @brief  ²éÑ¯ÊÇ·ñ·¢ËÍIC¿¨Êý¾Ýµ½ÈÏÖ¤ÖÐÐÄ
  * @param  None
  * @retval None
  */
u8 IcCardNet_SendIs(void)
{
    return IcSendGprsFlag;
}
/*
  * @brief  Çå³ý·¢ËÍIC¿¨ÖÐÐÄ±êÖ¾
  * @param  None
  * @retval None
  */
void IcCardNet_SendClear(void)
{
    IcSendGprsFlag = 0;
}
/*
  * @brief  °Ñ´ÓIC·¢¸øÉè±¸µÄÊý¾ÝÓÃ0900Í¸´«µ½ÈÏÖ¤ÖÐÐÄ
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
  * @brief  °ÑÆ½Ì¨ÏÂ·¢8900Êý¾Ý×é°ü·¢µ½IC¿¨´®¿Ú
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
  * @brief  ÏòIC¿¨´®¿Ú·¢ËÍÓ¦´ðÊý¾Ý
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
  * @brief  ÏòÆ½Ì¨·¢ËÍ¼ÝÊ»Ô±ÉÏ°à
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
        IcBufPublic[len++] = 0x01;                //ÉÏ°à
    }
    else
    {
        IcBufPublic[len++] = 0x02;                //ÏÂ°à
    }

    memcpy(&IcBufPublic[len],IcBufDriverTime,6);
    len = len + 6;

    if(IcDriverFlag == 0)
    {
        RadioProtocol_DriverInformationReport(CHANNEL_DATA_2,IcBufPublic,len);
        return;
    }

    IcBufPublic[len++] = 0x00;                        //¶Á¿¨³É¹¦

    temp = *p;                                        //¼ÝÊ»Ô±³¤¶È
    p = p + 1;
    IcBufPublic[len++] = temp;
    
    memcpy(&IcBufPublic[len],p,temp);                  //¼ÝÊ»Ô±ÐÕÃû
    p = p + temp;
    len = len + temp;

    temp = 20;
    memcpy(&IcBufPublic[len],p,temp);                  //´ÓÒµ×Ê¸ñÖ¤ºÅ
    p = p + temp;
    len = len + temp;

    temp = *p;                                        //·¢Ö¤»ú¹¹³¤¶È
    p = p + 1;
    IcBufPublic[len++] = temp;    

    memcpy(&IcBufPublic[len],p,temp);                  //·¢Ö¤»ú¹¹
    p = p + temp;
    len = len + temp;

    temp = 4;
    memcpy(&IcBufPublic[len],p,temp);                  //·¢Ö¤Ê±¼ä
    len = len + temp;
    
    RadioProtocol_DriverInformationReport(CHANNEL_DATA_2,IcBufPublic,len);
    
}
/*
  * @brief  ÏòÆ½Ì¨·¢ËÍ¼ÝÊ»Ô±ÏÂ°à
  * @param  None
  * @retval None
  */
void IcCardNet_SendDriverRest(void)
{
    u16 len = 0;
    
    memset(IcBufPublic,0,sizeof(IcBufPublic));

    IcBufPublic[len++] = 0x02;//ÏÂ°à

    Public_ConvertNowTimeToBCDEx(&IcBufPublic[len]);
    len = len + 6;

    RadioProtocol_DriverInformationReport(CHANNEL_DATA_2,IcBufPublic,len);
}
/*
  * @brief  ICÔÚÏß±êÖ¾
  * @param  None
  * @retval None
  */
u8 Get_ICModuole_OnLine_fFlag(void)
{
 return   ModuleOnLine ; 
}
/*
  * @brief  IC¿¨ÃüÁî´¦Àí×¨ÓÃ
  * @param  None
  * @retval None
  */
void IcCardNet_CmdHandle(u8 *pRec, u16 RecLen)
{
    u8  *p   = pRec;
    u16 len  = RecLen;

    /***************************************************************************/
    memset(IcBufPublic,0,sizeof(IcBufPublic));//ÌáÈ¡Êý¾Ý
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
            Public_PlayTTSVoiceStr("¼ÝÊ»Ô±ÉÏ°à");                   
            memcpy(IcBufDriverInfor,IcStrData.Data,sizeof(IcStrData.Data));
            Public_ConvertNowTimeToBCDEx(IcBufDriverTime);
            IcCardNet_SendDriverWork();
            IcCardNet_SendComAck();
            break;
        }
        case 0x42:
        {
            IcDriverFlag = 0;
            Public_PlayTTSVoiceStr("¼ÝÊ»Ô±ÏÂ°à");
            Public_ConvertNowTimeToBCDEx(IcBufDriverTime);
            IcCardNet_SendDriverRest();
            IcCardNet_SendComAck();
            IcCardState = IC_MODULE_CLOSE;
            break;
        }        
    }
}

/*
  * @brief  IC¿¨¼àÌýº¯Êý
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
  * @brief  ´ò¿ªIC¿¨Á¬½Ó
  * @param  ÑÓÊ±´ò¿ªÁ¬½Ó
  * @retval None
  */
void IcCardNet_Open(u16 delay)
{
    if(IcCardState != IC_MODULE_IDLE)
    {
        return;
    }
    IcCardState = IC_MODULE_IDLE;
    Net_Second_Open();     //Á¬½Ó2´ò¿ª
}
/**
  * @brief  IC¿¨ÈÏÖ¤ÖÐÐÄÁ¬½ÓÈÎÎñ
  * @param  None.
  * @retval ·µ»ØÈÎÎñ×´Ì¬.
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
        case IC_MODULE_OPEN:    //¿ª´òÒ»¸öÁ¬½Ó
            {
            Second_connect_To_Specific_Server();
            IcCardState = IC_MODULE_LISTEN;
            if((Modem_IpSock_STA[EYENET_SECOND_SOC] == MODSOC_ONLN)&&(Second_IP() == VAL_IPTO))  //ÅÐ¶ÏÁ¬½Ó2ÔÚÏß·ñ                  
            {
                Public_ShowTextInfo("·þÎñÆ÷ÒÑÁ¬½Ó", PUBLICSECS(3)); 
            }
            }
        break;     
        case IC_MODULE_LISTEN://¼àÌýÒ»¸öÁ¬½Ó,³¤ÆÚ´¦ÓÚ¸Ã×´Ì¬
            {
            if((Modem_IpSock_STA[EYENET_SECOND_SOC] == MODSOC_ONLN)&&(Second_IP() == VAL_IPTO))//Ö¸¶¨ipÔÚÏß
            {
                TerminalAuthorizationFlag |= 0x04;    //Ç¿ÖÆÖÃÎ»¼øÈ¨³É¹¦±êÖ¾  
            }
            else
            {
                TerminalAuthorizationFlag &= 0xfb;

            }
            IcCardNet_Lisen();
            }
        break;        
        case IC_MODULE_CLOSE://¹Ø±ÕÒ»¸öÁ¬½Ó
            {   
                Second_Switch_Net_Specific_To_MainIP();       //Ö¸¶¨Á¬½ÓÏÂÏß£¬ÇÐ»»»ØÖ÷Á¬½Ó
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

    



