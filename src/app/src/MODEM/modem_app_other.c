/*
********************************************************************************
*
*                                                              
*
* Filename      : modem_app_Other.c
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-10-27  
* 
*
*Note:��Ҫ����FTP������SMS����
********************************************************************************
*/

/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#include "include.h"

/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/
#ifdef MODEM_APP_OTHER_H
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
static Modem_APP_STATE NetOther_State;//״̬��
static APP_FTP         NetOther_FtpVal;//FTP��������

static u32 NetOther_SysRestDelay;//ϵͳ��λ��ʱʱ��
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

/*
  * @brief  ���ű������ʹ���
  * @param  None
  * @retval None
  */
void Sms_Set_Alarm(void)
{
    u8 tab_send[200]={0};
    u8 ntab[20]={0};
    u8 num[15]={0};
    u8 nlen=0;
    u8 len_send=0;

    EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, ntab);//���ƺ���
    nlen = strlen((char *)ntab);
    memcpy(tab_send,ntab,nlen);
    tab_send[nlen] = '|';
    len_send = nlen + 1;
    
    memset(ntab,0,sizeof(ntab));  
    EepromPram_ReadPram(E2_PRAM_BASE_CUSTOM_ID+0x07, ntab);//�ն��ֻ�����
    nlen = strlen((char *)ntab);
    nlen = ModLib_ByteStr(num, ntab, nlen);
    memcpy(tab_send+len_send,num+1,11);  
    len_send = len_send + 11;
    tab_send[len_send] = '|';
    len_send++;    

    memcpy(tab_send+len_send,"��������",8);//��������
    len_send = len_send + 8;
    tab_send[len_send] = '|';   
    len_send++;

    memset(ntab,0,sizeof(ntab)); 
    Public_ConvertNowTimeToBCDEx((u8*)&ntab);//ʵʱʱ��
    nlen = strlen((char *)ntab);
    nlen = ModLib_ByteStr(tab_send+len_send, ntab, nlen);
    len_send = len_send + nlen;
    tab_send[len_send] = '|';   
    len_send++;

    memset(ntab,0,sizeof(ntab));            //λ����Ϣ
    Public_GetCurPositionInfoDataBlock(ntab);
    nlen = strlen((char *)ntab);
    nlen = ModLib_ByteStr(tab_send+len_send, ntab, nlen);

    memset(ntab,0,sizeof(ntab));  
    EepromPram_ReadPram(E2_SMS_TEXT_ALARM_PHONE_ID, ntab);//�����ֻ�����
    
    nlen = strlen((char *)tab_send);
    Modem_Api_SmsSend(SMS_PDUUCS,ntab,tab_send,nlen);
}

/*
  * @brief  ���ż�����ô���
  * @param  type -> 0:���ƽ̨���ź���
                    1:��Ȩ���ź���
  * @retval None
  */
void Sms_Set_Num(u8 type ,u8 *pRec, u16 len)
{//smsnum
    u8 tab[20]={0};

    strcpy((char*)tab,(char*)modsms.num);
    if(type == 0)
    {
        EepromPram_WritePram(E2_MONITOR_SERVER_SMS_PHONE_ID,tab,E2_MONITOR_SERVER_SMS_PHONE_LEN);
    }
    else if(type == 1)
    {
        EepromPram_WritePram(E2_MONITOR_SERVER_PRIVILEGE_SMS_ID,tab,E2_MONITOR_SERVER_PRIVILEGE_SMS_LEN);
    }
}

/*
  * @brief  ��������IP
  * @param  type: IP��ַ����Ϊ0����������Ϊ1
  * @param  *p:ָ��������ַ
  * @param  len:������ַ���ݵĳ���
  * @retval None
 */
u8  Sms_Set_Ip(u8 type,u8 *pRec, u16 len)
{

    u16 i=0;// ip,219.142.106.193,port,8500#  surl,www.gps7788.com,port,8500#
    u8  *p=NULL;
    u8  tab[50]={0};
    u8  ip[20]={0};
    u8  port[5]={0};
    u16 dport=0;
    u8  cnt1=0;//����','�ĸ���
    u8  cnt2=0;//����'.'�ĸ���
    u8  cnt3=0;//����'#'�ĸ���
    u8  addrbuf[5]={0};

    if(len > sizeof(tab))
    {
        return 0;//�ڴ����
    }
    memcpy(tab,pRec,len);
    p=tab;

    for(i=0;i<len;i++)
    {
        if(*(p+i) == ',')
        {
            addrbuf[cnt1++] = i;
        }
        else if(*(p+i) == '.')
        {
            cnt2++;
        }
        else if(*(p+i) == '#')
        {
            cnt3++;
            addrbuf[cnt1] = i;
        }        
    }
    if(type == 0)//ip��ַ����
    {
        if((cnt1 != 3)||(cnt2 != 3)||(cnt3 != 1))
        {
            return 0;//��ʽ������Ҫ�󷵻ش���
        }
    }
    else//��������
    {
        if((cnt1 != 3)||(cnt3 != 1))
        {
            return 0;//��ʽ������Ҫ�󷵻ش���
        }
    }
    
    memcpy(ip,p+addrbuf[0]+1,addrbuf[1]-addrbuf[0]-1);//IP��ַ������
    memcpy(port,p+addrbuf[2]+1,addrbuf[3]-addrbuf[2]-1);//�˿�
    
    dport =  atoi((char*)port);
    port[0] = 0;
    port[1] = 0;
    port[2] = dport>>8;
    port[3] = dport&0xff;
    port[4] = 0;
    EepromPram_WritePram(E2_MAIN_SERVER_IP_ID,ip,E2_MAIN_SERVER_IP_LEN);
    EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID,port,E2_MAIN_SERVER_TCP_PORT_LEN);
    return 1;
} 

/*
  * @brief  ���������ն��ֻ���
  * @param  *p:ָ��������ַ
  * @param  len:������ַ���ݵĳ���
  * @retval None
 */
u8  Sms_Set_DevicePhone(u8 *pRec, u16 len)
{//ID,0000xxxxxxx0#  (11λID)

    u8 tab[20]={0};
    u8 num[10]={0};
    u8 tabnum[20]={0};
    u8 nlen=0;

    u8 i;
    u8 *p = pRec;
    u8 cnt=0;
    u8 addr =0;
    
    for(i=0;i<len;i++)
    {
        if(*(p+i) == '#')
        {
            cnt++;
            addr=i;
        }
    }
    if(cnt != 1)
    {
        return 0;
    }


    memcpy(tab,p+3,addr-3);
    nlen = strlen((char *)tab);
    if(nlen%2)
    {
        tabnum[0]='0';
        memcpy(tabnum+1,tab,nlen);
        nlen++;
    }
    else
    {
        memcpy(tabnum,tab,nlen);
    }
    ModLib_StrByte(tabnum,num,nlen);  
    EepromPram_WritePram(E2_PRAM_BASE_CUSTOM_ID+0x07,num,E2_DEVICE_PHONE_ID_LEN); 

    return 1;
}
/*
  * @brief  ��������ȱʡʱ����
  * @param  *p:ָ��������ַ
  * @param  len:������ַ���ݵĳ���
  * @retval None
 */
u8  Sms_Set_ReportTime(u8 *pRec, u16 len)
{
    u8  tab[15]={0};//report=XXXX
    u32 val=0;

    val =  atoi((char*)pRec+7);
    tab[0] = val>>24;
    tab[1] = (val>>16)&0xff;
    tab[2] = (val>>8)&0xff;
    tab[3] = val&0xff;

    EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID,tab,E2_ACCON_REPORT_TIME_LEN); 
    EepromPram_UpdateVariable(E2_ACCON_REPORT_TIME_ID);
    return 1;
}
/*
  * @brief  ������������ʱ��
  * @param  *p:ָ��������ַ
  * @param  len:������ַ���ݵĳ���
  * @retval None
 */
u8  Sms_Set_Heartbeat(u8 *pRec, u16 len)
{
    u8  tab[15]={0};//heart=
    u32 val=0;

    val =  atoi((char*)pRec+6);
    tab[0] = val>>24;
    tab[1] = (val>>16)&0xff;
    tab[2] = (val>>8)&0xff;
    tab[3] = val&0xff;

    EepromPram_WritePram(E2_TERMINAL_HEARTBEAT_ID,tab,E2_TERMINAL_HEARTBEAT_LEN); 
    EepromPram_UpdateVariable(E2_TERMINAL_HEARTBEAT_ID);
    return 1;
}
/*
  * @brief  ���������ն�ID
  * @param  *p:ָ��������ַ
  * @param  len:������ַ���ݵĳ���
  * @retval None
 */
u8  Sms_Set_DeviceId(u8 *pRec, u16 len)
{//ID,0000xxxxxxx0#  (11λID)

    u8 tab[20]={0};
    u8 tabnum[20]={0};
    u8 num[10]={0};
    u8 nlen=0;
    u8 i;
    u8 *p = pRec;
    u8 cnt=0;
    
    for(i=0;i<len;i++)
    {
        if(*(p+i) == '#')
        {
            cnt++;
        }
    }
    if(cnt != 1)
    {
        return 0;
    }

    memcpy(tab,p+3,len-4);
    nlen = strlen((char *)tab);
    if(nlen%2)
    {
        tabnum[0]='0';
        memcpy(tabnum+1,tab,nlen);
        nlen++;
    }
    else
    {
        memcpy(tabnum,tab,nlen);
    }
    ModLib_StrByte(tabnum,num,nlen);
 
    EepromPram_WritePram(E2_PRAM_BASE_CUSTOM_ID+0x06,num,E2_DEVICE_ID_LEN); 
    return 1;
}
/*
  * @brief  ����汾��ʱ��
  * @param  *p:ָ��������ַ
  * @param  len:������ַ���ݵĳ���
  * @retval ���س���
 */
u8  Sms_Set_VerNo(u8 *pData)
{
    u8 len=0;

/******************************************************************************///EGS702
#ifdef   EYE_EGS702
    len = Version_GetFirmwareInformation(pData);

/******************************************************************************/
#elif    defined EYE_EGS701
    u8 tmp;
    u8 *p = pData;
    
    tmp = strlen(FIRMWARE_VERSION);
    memcpy(p,FIRMWARE_VERSION,tmp);
    len = tmp;
    p = p + len;

    *p = ',';
    p++;
    len++;

    tmp = strlen(FIRMWARE_TIME);
    memcpy(p,FIRMWARE_TIME,tmp);
    len = len + tmp;    
#endif

    return len;
}
/*
  * @brief  ϵͳ����������Ȩ���뷢�Ͷ���
  * @param  None
  * @retval ���ؽ��
 */
u8  Sms_Set_SmsRst(u8 *pData)
{
    u8 addr;
    u8 tab[40];

    EepromPram_ReadPram(E2_MONITOR_SERVER_PRIVILEGE_SMS_ID,tab);
    if(ModLib_AtCmp(tab,modsms.num))
    {
        return 0;
    }
    addr = ModLib_AtAddr(pData,'=')+1;
    if(*(pData+addr) == '1')
    {
        tab[0] = 1;//���������Ͷ��� 
    }
    else
    {
        tab[0] = 0;//�ر��������Ͷ���
    }
//    FRAM_BufferWrite(FRAM_HY_UPDATA_ADDR,tab,1);//�ۼ����
    tab[0] = 0XAA;
//    FRAM_BufferRead(tab,1,FRAM_HY_UPDATA_ADDR);//�ۼ����    
    return 1;
}
/**
  * @brief  �������Ͷ���
  * @param  None
  * @retval None
  */
void Net_Other_SmsRst(void)
{
    u8   Num[20];
    u8   StrLen;
    u8   StrBuf[100];
    static u8  State;
    static u8  Value[5];
    static u16 TimDly;            

    switch(State)
    {
        case 0:
        {
            State++;
//            FRAM_BufferRead(Value,1,FRAM_HY_UPDATA_ADDR);            
        }
        break;
        case 1:
        {
            if(Value[0] == 1)
            {
                State++;
            }
        }
        break;
        case 2:
        {
            if(TimDly++ > MODEM_TMIN)
            {
                State++;
                EepromPram_ReadPram(E2_MONITOR_SERVER_PRIVILEGE_SMS_ID,Num);
                Num[0] = ModLib_AtLen(Num);
                if((Num[0]==13)||(Num[0]==11))
                {
                    memset(StrBuf,0,sizeof(StrBuf));
                    StrLen=ModLib_AtCmy(StrBuf,"SysRst!!!,");
                    
                    Public_ConvertNowTimeToBCDEx(Num);
                    StrLen += ModLib_ByteStr(StrBuf+StrLen,Num,6);
                    StrLen += ModLib_AtCmy(StrBuf+StrLen,",");
                    
                    EepromPram_ReadPram(E2_FIRMWARE_VERSION_ID,StrBuf+StrLen);
                    StrLen += ModLib_AtLen(StrBuf+StrLen);
                    StrLen += ModLib_AtCmy(StrBuf+StrLen,",");

                    StrLen += ModLib_AtCmy(StrBuf+StrLen,FIRMWARE_TIME);
                    StrLen += ModLib_AtCmy(StrBuf+StrLen,",");
                    
                    EepromPram_ReadPram(E2_DEVICE_ID,Num);
                    StrLen += ModLib_ByteStr(StrBuf+StrLen,Num,7);
                    StrLen += ModLib_AtCmy(StrBuf+StrLen,",");
                    
                    EepromPram_ReadPram(E2_DEVICE_PHONE_ID,Num);
                    StrLen += ModLib_ByteStr(StrBuf+StrLen,Num,6);
                    
                    StrLen  = ModLib_AtLen(StrBuf);
                    EepromPram_ReadPram(E2_MONITOR_SERVER_PRIVILEGE_SMS_ID,Num);
                    Modem_Api_SmsSend(SMS_TXT,Num,StrBuf,StrLen);
                }
            }
        }
        break; 
        default:
        break;
    
    }
}

/**
  * @brief  ���Ų�ѯ�豸״̬
  * @param  None
  * @retval None
  */
void Net_Other_SmsSta(void)
{
    char buf[200];
    u8   timebcd[10];
    char timeasc[20];    
    u8  csq; 
    u32 timecore;
    u32 timeon;
    u8  len;

    memset(buf,0,sizeof(buf));
    Public_ConvertNowTimeToBCDEx(timebcd);//��ǰʱ��
    Public_ConvertBcdToAsc((u8*)timeasc,timebcd,6);
    timeasc[12] = 0;
    
    csq = Modem_Port_Csq();       //��������
    timecore = modInf.Tim_Core/60;//�ں�����ʱ��      ��λ ����
    timeon   = modInf.Tim_Onln/60;//���ųɹ�����ʱ��  ��λ ����


    sprintf(buf,"timenow=%s,timecore=%06d,timeon=%06d,csq=%02d",
                                   timeasc,timecore,timeon,csq);
    len = strlen(buf);
    
    Modem_Api_SmsSend(SMS_TXT,NULL,(u8*)buf,len);
}
/*
  * @brief  ��ʾ��ʾ��Ϣ
  * @param  None
  * @retval None
  */
void Net_Other_DisPlay(u8 *pSrc)
{
    LcdShowMsgEx((char*)pSrc,100);
}
/*
  * @brief  ��ӡ��½��Ȩʱ��ʱ�䣬ÿ�η��͵�½���Ȩʱ������
  * @param  ch: 
  *            0Ϊע��  
  *            1Ϊ��Ȩ  
  *            2Ϊλ����Ϣ   
  *            3ΪGNSS͸��ʱ��
  * @retval None
  */

void Net_Other_Printf(NET_PRINTF_TYPE type)
{
    TIME_T nowTime;
    char  tab[50];
    
    RTC_GetCurTime(&nowTime);

    switch(type)
    {
        case PRTF_NOWTIME://��ǰʱ��
        {
            strcpy(tab,"��ǰʱ��\r\n");
            break;
        }
        case PRTF_AUTHORIZATION://��Ȩ
        {
            strcpy(tab,"��Ȩʱ��:%d:%d:%d\r\n\r\n");
            break;
        }
        case PRTF_REGISTER://ע��
        {
            strcpy(tab,"ע��ʱ��:%d:%d:%d\r\n\r\n");
            break;
        }    
        case PRTF_POSITION://λ��
        {
            strcpy(tab,"λ����Ϣʱ��:%d:%d:%d\r\n\r\n");
            break;
        }
        case PRTF_GNSS://λ��
        {
            strcpy(tab,"GNSS͸��ʱ��:%d:%d:%d\r\n\r\n");
            break;
        }        
        default:
        {
            return;
        }

    }
    
    MODAPP_PRTF(tab, nowTime.hour,nowTime.min,nowTime.sec );
}
/**
  * @brief  ϵͳ��λ
  * @param  None
  * @retval None
  */
void Net_Other_SysRest(void)
{
    if((NetOther_SysRestDelay--) == 0)
    {
        NVIC_SystemReset();
    }
}

/*
  * @brief  ftp�ļ�����CRCУ���ʼ��
  * @param  None
  * @retval None
  */
void Net_Other_FtpCrcInit(void)
{
	/* Enable CRC Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	/* Reset CRC generator */
  	CRC->CR = CRC_CR_RESET;
}
/*
  * @brief  ftp�ļ����ؼ���CRCУ��
  * @param  *p:ָ��ҪУ�������
  * @param  len:ҪУ�����ݵĳ���
  * @retval ���ؼ�����У��ֵ
  */
u32 Net_Other_FtpCrcDo(u8 *p, u16 len)
{
    u32  i = 0;
    u32  ch = 0;

    if(p != NULL)
	{
        for(i=0;i<len;i++)
        {
            ch = *((u8*)p + i);
            CRC->DR = ch;
        }
	}
	ch = CRC->DR;
	return(ch);
}

/*
  * @brief  ��ȡftp�ļ����ز���,IP���˿ڵ�
  * @param  None
  * @retval �����Ƿ�ִ������, 1Ϊ������0Ϊ���������ⲻ����
  */
u8 Net_Other_FtpVal(void)
{
    //"183.238.13.194","21","egs701","000000","/", "huoyun.bin"); 
    //huoyun.bin//V20109.bin//ftptest.txt//hb201308151659V1.01.01updata.bin
    u8  *p=NULL;
    u8  len=0;
    u8  addr=0;
    u8  tab[100]={0};
    u8  num=0;//","����
    u8  numaddr[5]={0};//","�ĵ�ַ
    u16 ch=0;
    u8  i;

    EepromPram_ReadPram(E2_UPDATA_MANUFACTURE_ID, tab);
    if (strncmp((char *)(tab),"70108",5) != 0)
    {
        return 0;
    }
    
    p = NetOther_FtpVal.ip;
    len = sizeof(NetOther_FtpVal.ip);
    memset(p,0,len);
    EepromPram_ReadPram(E2_UPDATA_IP_ID, p);

    p = NetOther_FtpVal.port;
    len = sizeof(NetOther_FtpVal.port);
    memset(p,0,len);
    EepromPram_ReadPram(E2_UPDATA_TCP_PORT_ID, tab);
    ch = (tab[0]<<8)|(tab[1]);
    sprintf((char*)p,"%d",ch);

    memset(tab,0,sizeof(tab));
    len = EepromPram_ReadPram(E2_UPDATA_URL_ID, tab);
    num = 0;
    for(i=0;i<len;i++)
    {
        if(tab[i] == ',')
        {
            numaddr[num] = i;
            num++;
        }
    }
    if(num != 3)
    {
        return 0;
    }
        
    p = NetOther_FtpVal.fpath;
    len = sizeof(NetOther_FtpVal.fpath);
    memset(p,0,len); 
    memcpy(NetOther_FtpVal.fpath,tab,numaddr[0]); 

    p = NetOther_FtpVal.fname;
    len = sizeof(NetOther_FtpVal.fname);
    memset(p,0,len);
    addr = numaddr[0] + 1;
    len  = numaddr[1] - numaddr[0] - 1;
    memcpy(p,tab+addr,len); 

    p = NetOther_FtpVal.user;
    len = sizeof(NetOther_FtpVal.user);
    memset(p,0,len);
    addr = numaddr[1] + 1;
    len  = numaddr[2] - numaddr[1] - 1;    
    memcpy(p,tab+addr,len); 

    p = NetOther_FtpVal.pass;
    len = sizeof(NetOther_FtpVal.pass);
    memset(p,0,len);
    addr = numaddr[2] + 1;
    len  = strlen((char *)tab) - addr;    
    memcpy(p,tab+addr,len);   
    return 1;
}

/*
  * @brief  ���ftp�����ļ�crcУ��
  * @param  *pType : ָ���������� 
  *                  0 -> �ն�;
  *                  9 -> ���ؿ�����;
  * @retval ����У�������ɹ�����IAR���ɵ�BIN�ļ���С��ʧ�ܷ���0
  */
u32 Net_Other_FtpCheck(u8 *pType)
{
    u32 ftpfilesize = Modem_Api_FtpSize();
    u32 Addrbase   =  UPDATE_BASE_ADDRESS; 
    u32 Address    =  Addrbase;

    u32 verify;//�����ļ��е�У��
    u32 verifycmp =0;//������У��
    u16 integer;//����
    u16 remainder;//����
    u16 i;
    u8  type;
    
    u8  ntab[520];

    if(ftpfilesize < 512)
    {
        return 0;
    }

    ftpfilesize = ftpfilesize - 22;
    
    sFLASH_ReadBuffer(ntab,Addrbase+ftpfilesize,22);

    if(Public_CheckTerminalAndPeripheralVerify(ntab,(unsigned long *)&verify,&type) == 0)
    {
        return 0;
    }
    *pType = type;//��������

    Net_Other_FtpCrcInit();//��ʼ��Ӳ��CRCУ��
    integer =  ftpfilesize / 512;//��������ҳ��  
    remainder = ftpfilesize % 512;//��������ҳ��

    for(i = 0; i < integer;)
    {
        sFLASH_ReadBuffer(ntab,Address,512);
        verifycmp = Net_Other_FtpCrcDo(ntab,512);
        i++;            
        Address = Addrbase + (i * 512);
    }
    
    if(remainder)
    {
        sFLASH_ReadBuffer(ntab,Address,remainder);
        verifycmp = Net_Other_FtpCrcDo(ntab,remainder); 
    }

    if(verify == verifycmp)
    {
        return (ftpfilesize+22);
    }
    
    return 0;
}
/*
  * @brief  ��ʼ����ftp�ļ�����
  * @param  None
  * @retval �����Ƿ�ִ������, 1Ϊ������0Ϊ���������ⲻ����
  */
u8 Net_Other_FtpStart(void)
{
    if(NetOther_FtpVal.flag == 0)
    {
        memset((u8*)&NetOther_FtpVal,0,sizeof(APP_FTP));

        if(Net_Other_FtpVal() == 0)
        {
            return 0;//�����·������⣬��ִ������
        }
        sFLASH_EraseSector(UPDATE_BASE_ADDRESS + (NetOther_FtpVal.sector * SECTOR_SIZE));

			
        ClrTimerTask(TIME_CAMERA);//�ر�����
    	  ClrTimerTask(TIME_BLIND);
    	  ClrTimerTask(TIME_POSITION);
    	
        NetOther_FtpVal.flag = 1;//��λftp������־
        NetOther_State = MODEM_APP_FTP;//����FTP��������
        
        return 1;
    }
    return 0;
}

/*
  * @brief  ftp�ļ����ػص�����
  * @param  None
  * @retval None
  */
void Net_Other_FtpCall(u8 *pData, u16 len)
{
    u32 addrbase   =  UPDATE_BASE_ADDRESS;
    u32 addrsector =  NetOther_FtpVal.sector * SECTOR_SIZE;
    u32 addrpage   =  NetOther_FtpVal.page * 512;
    u32 Address    =  addrbase + addrsector + addrpage;
    
    if(NetOther_FtpVal.flag == 0)
    {
        return;
    }

    sFLASH_WriteBuffer(pData, Address, len);

    NetOther_FtpVal.page++;
    if((NetOther_FtpVal.page * 512) >= SECTOR_SIZE)
    {
        NetOther_FtpVal.page = 0;
        NetOther_FtpVal.sector++;
        sFLASH_EraseSector(UPDATE_BASE_ADDRESS + NetOther_FtpVal.sector * SECTOR_SIZE);
    }
}
/*
  * @brief  ftp�ļ��������
  * @param  None
  * @retval None
  */
void Net_Other_FtpEnd(void)
{
    u32 ftpfilesize = Modem_Api_FtpSize();
    u8  tab[5]={0};
    u8  i=0;
    u8  sum=0;
    u8  type=0;//�������� 

    if(Modem_Api_FtpResult())
    {
        ftpfilesize = Net_Other_FtpCheck(&type);//��������ļ�CRCУ��

        if(ftpfilesize == 0)
        {
            MODAPP_PRTF("modem->Ftp_APP У�����!!!\r\n");
            
            tab[0] = 0;//��������
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_TYPE_ADDR, tab, FRAM_FIRMWARE_UPDATA_TYPE_LEN);

            tab[0] = 3;//������־ ʧ�� 3
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,tab,1);
            
            return;//У��ʧ�ܣ�����������
        }
        MODAPP_PRTF("modem->Ftp_APP У����ȷ!!!\r\n");
    	tab[0] = ftpfilesize>>24;
    	tab[1] = (ftpfilesize>>16)&0xff;
    	tab[2] = (ftpfilesize>>8)&0xff;  
    	tab[3] =  ftpfilesize&0xff;   

        sum = 0;
		for (i=0; i<4; i++) 
		{
			sum += tab[i];
			E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR + i, tab[i]) ;
		}
		E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR + i, sum);

        ////////////////////////////////////////////////////////////////////////�ն�
        if(type == 0)
        {
            MODAPP_PRTF("modem->Ftp_APP �����ն�!!!\r\n");
            MODAPP_PRTF("modem->Ftp_APP ��������...����IAP..\r\n");
            tab[0] = 0;//��������
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_TYPE_ADDR, tab, FRAM_FIRMWARE_UPDATA_TYPE_LEN);

            tab[0] = 2;//�����ɹ�
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,tab,1);        

            //���³����־  
    		E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR, 0xaa);
        	E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR+1, 0xaa);
        }
        ////////////////////////////////////////////////////////////////////////���ؿ�����
        #ifdef  EYE_EGS701
        else if(type == 9)
        {
            MODAPP_PRTF("modem->Ftp_APP ��������!!!\r\n");
            MODAPP_PRTF("modem->Ftp_APP ����������������...\r\n");
            tab[0] = 9;//��������
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_TYPE_ADDR, tab, FRAM_FIRMWARE_UPDATA_TYPE_LEN);
            

            Communication_TaskSet(0);//�ر�����ͨѶģ������
        }
        #endif
        MODAPP_PRTF("\r\n\r\n");
    }
}

/*
  * @brief  �����б��Ϣ����
  * @param  None
  * @retval None
  */
void Net_Other_SmsHB(u8 *pData, u8 len)
{
    u8 *p=pData;
    if((*p != 0x7e)||(*(p+len-1) != 0x7e))
    {
        return;
    } 
    uart2_recdat.rec_chanel = CHANNEL_SMS;//�л�������ͨ��
    
    RadioProtocol_AddRecvDataForParse(CHANNEL_SMS,p,len);
}
/*
  * @brief  ������ź�������
  * @param  None
  * @retval ��Ч���� -> 0;
  *         �������� -> 1;
  *         �б����� -> 2;
  */
u8 Net_Other_SmsDB(u8 *pData, u8 len)
{
    u8  flag=0;
	u8  tab_rec[80]={0};
	u8  tab[20]={0};
    
    MODSMS_TYPE  smstpye = SMS_TXT;//��������
  
    if(ModLib_AtCmp(pData,"smsnum") == 0)////////SMS��غ��� 
    {//smsnum
        Sms_Set_Num(0,pData,len);
        sprintf((char*)tab_rec,"Set Ok ! %s",modsms.num);
        Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec));
        return 0;
    }
    EepromPram_ReadPram(E2_MONITOR_SERVER_SMS_PHONE_ID,tab);
    if(ModLib_AtLen(tab) == 0)
    {
        return 2;
    }
    if(ModLib_AtCmp(tab,modsms.num))
    {
        return 2;
    }
    if ((ModLib_AtCmp(pData,"ip,") == 0)||(ModLib_AtCmp(pData,"surl,") == 0))////IP����
    {   //ip,219.142.106.193,port,8500#  surl,www.gps7788.com,port,8500# 
        if(Sms_Set_Ip(1,pData,len))
        {
            sprintf((char*)tab_rec,"Set Ok ! %s",pData);
            NetOther_SysRestDelay = MODEM_TMIN;//��ʱ1��������
            NetOther_State        = MODEM_APP_SYSREST;
        }
        else
        {
            ModLib_AtCmy(tab_rec,"Set Err !!!");
        }
        Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec));
        flag = 1;
    } 
    else if(ModLib_AtCmp(pData,"rest") == 0)///////////////////ϵͳ��λ
    {
        Modem_Api_SmsSend(smstpye,NULL,"rest is OK!",11);
        NetOther_SysRestDelay = MODEM_TMIN;//��ʱ1��������
        NetOther_State        = MODEM_APP_SYSREST;
        flag = 1;
    }
    else if(ModLib_AtCmp(pData,"ID,") == 0)//////////////////�ն��ֻ�������  
    {//ID,0000xxxxxxx0#  (11λID)
        if(Sms_Set_DevicePhone(pData,len))
        {
            sprintf((char*)tab_rec,"Set Ok ! %s",pData);
            Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec));            
        }
        else
        {
            Modem_Api_SmsSend(smstpye,NULL,"ID SET ERR",10);
        } 
        flag = 1; 
    }   
    else if (ModLib_AtCmp(pData,"verno#") == 0)/////////////////////����汾
    {
        len = Sms_Set_VerNo(tab_rec);
        Modem_Api_SmsSend(smstpye,NULL,tab_rec,len);       
        flag = 1; 
    }      
    else if (ModLib_AtCmp(pData,"heart=") == 0)/////////////////����ʱ������
    {//heart=XXXX
        Sms_Set_Heartbeat(pData,len);
        sprintf((char*)tab_rec,"Set Ok ! %s",pData);
        Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec));  
        flag = 1;
    }
    else if (ModLib_AtCmp(pData,"report=") == 0)///////////////ȱʡʱ������
    {//report=XXXX
        Sms_Set_ReportTime(pData,len);
        sprintf((char*)tab_rec,"Set Ok ! %s",pData);
        Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec));  
        flag = 1;
    }
    else if (ModLib_AtCmp(pData,"numvip") == 0)///////////////��Ȩ���ź�������
    {
        Sms_Set_Num(1,pData,len);
        sprintf((char*)tab_rec,"Set Ok ! %s",modsms.num);
        Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec));  
    }   
    else if (ModLib_AtCmp(pData,"smsrst=") == 0)//////ϵͳ����������Ȩ���뷢�Ͷ���
    {
        if(Sms_Set_SmsRst(pData))
        {
            sprintf((char*)tab_rec,"Set Ok ! %s",pData);
            Modem_Api_SmsSend(smstpye,NULL,tab_rec,ModLib_AtLen(tab_rec)); 
        }
    }  
    else if (ModLib_AtCmp(pData,"smssta") == 0)////////////////////////��ѯ�豸״̬
    {
        Net_Other_SmsSta(); 
    }      
    else
    {
        flag = 2;
    }
    return flag;
}

/*
  * @brief  ���Żص�����
  * @param  None
  * @retval None
  */
void Net_Other_SmsCall(MODSMS_TYPE type, u8 *p, u16 len)
{
    u8 tab[70]={0};

    if(type == SMS_TXT)
    {
        if((len/2) < sizeof(tab))
        {
            if(Net_Other_SmsDB(p,len) == 2)//�����ַ�����
            {
                len = ModLib_StrByte(p,tab,len);
                Net_Other_SmsHB(tab,len);//�����б�����
            }
        }
    }
    else if(type == SMS_PDU7)
    {
        Net_Other_SmsDB(p,len);//7bit �ַ�����
    }
    else if(type == SMS_PDU8)
    {
        Net_Other_SmsHB(p,len);//8bit ������
    }
}
/**
  * @brief  ���Ӵ���
  * @param  None
  * @retval None
  */
void Net_Other_Run(void)
{
    MODERR_TYPE modemack;

    Net_Other_SmsRst();//�������Ͷ���

    switch(NetOther_State)
    {
        case MODEM_APP_IDLE:
            {
                ;
            }
        break; 
        case MODEM_APP_FTP://FTP����
            {
                modemack = Modem_Api_Ftp(NetOther_FtpVal.ip,
                                         NetOther_FtpVal.port,
                                         NetOther_FtpVal.user,
                                         NetOther_FtpVal.pass,
                                         NetOther_FtpVal.fpath,
                                         NetOther_FtpVal.fname);
                if(modemack == MOD_OK)
                {
                    Net_Other_FtpEnd();//���ؽ��� ��ʱ��λ
                    NetOther_SysRestDelay = MODEM_TSEC*2;
                    NetOther_State        = MODEM_APP_SYSREST;
                }
                else if(modemack == MOD_ER)
                {
                    NetOther_SysRestDelay = 0;//����ʧ�� ������λ
                    NetOther_State = MODEM_APP_SYSREST;
                }
            }
        break;
        case MODEM_APP_SYSREST://��λϵͳ
            {
    			 Net_Other_SysRest();
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
