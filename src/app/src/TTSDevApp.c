/*******************************************************
Copyright (C),E-EYE Tech. Co.,Ltd.
Author: WYF
Version:V1.0
Finish Date:2012-6-12
Description:TTSЭ�����
*******************************************************/
#include "include.h"

#define S_OFF_SPK        0         //���ſ���
#define S_ON_SPK         1
#define S_OFF_TTS_POWER  0         //TTS POWER ����
#define S_ON_TTS_POWER   1 

#define VOICE_DATA_LEN        512 //�ܹ����ŵ��ֽڳ���
//�궨��
#define TTS_VOICE_SIZE      180                          //ÿ�������ֽ���
//�ṹ����
#pragma pack(1)
//TTS Э��ṹ
typedef struct
{
    u8 Head;     //0XFD
    u8 Len[2];   //���ֽ���ǰ�����ֽ��ں�
    u8 Cmd;      //ָ�� 0X01:�����ϳ� 0X88:PowerDown 0X22:IDLE 0XFF:WakeUp 
    u8 FontStyle;//���ֱ����ʽ 0X00:GB2313 0X01:GBK 0X02:BIG5 0X03:UNICODE
    u8 VoiceData[TTS_VOICE_SIZE]; //ָ���ַ�����ָ��
    u16 PageLen; //����Э�����ֽ���
}ST_TTS_PRO;
#pragma pack()
//////////////////////////////
#define TTS_MAX_SEND_LEN sizeof(ST_TTS_PRO)   //�����������ݵĳ���
//////////////////////////////////
//�ṹ����
typedef struct
{   
    u16 voiceDataLen;   //���������ܳ���
    u16 sendDataLen;    //��ǰ�������ݷ��ͼ���
    u8 status;          //TTS����״̬
    u8 curSendLen;      //��ǰ���η���ʵ�ʳ���
    u8 oneMaxLen;       //������󳤶�
    u8 oneMinLen;       //������С����
    u8 busy;            //TTSæ,�ڶ����������ʱ,TTS��ռ�õ�����æ��־ 
    u8 extTtsFlag;      //֧���ⲿTts
    u8 checkCnt;        //
    u8 checkRDYCnt;     //
    u8 *sendPtr;    //ָ���ƽ�����ǰ�Ĵ�������   
}ST_TTS_ATTRIB;
////////////////////////////
static ST_TTS_ATTRIB s_stTtsAttirb;//
static unsigned char s_ucTtsPlayVoiceData[VOICE_DATA_LEN+1]; //�������ݳ�   
//////////////////
typedef enum{E_VOICE_INIT=0,E_VOICE_LOAD, E_VOICE_PLAY, E_VOICE_END, E_VOICE_MAX}E_TTS_STATUS;

//��ѡ����ʾ�� ��
//������ʾ��   ������ʾ��
//sounda       sounde
//soundb       soundf
//soundc       soundg
//soundd
#define USE_INFO_SOUND        "soundb"        //�û���Ϣ��ʾ��
#define ALARM_INFO_SOUND      "soundc"        //������Ϣ��ʾ��

//��������
enum TTS_PLAY_TYPE
{
    TTS_PLAY_TYPE_USE = 0,                    //��ͨ��Ϣ
    TTS_PLAY_TYPE_ALARM,                      //������Ϣ
};

enum 
{
    TTS_USE_EMPTY = 0,//û���
    TTS_USE_EXT,    //�ⲿTTS
    TTS_USE_INT,    //�ڲ�TTS
    TTS_USE_MAX
};
//��������


u8   ttsVolume = 8;                          //����
u8   ttsValue[9]={0,1,3,5,7,9,11,13,16};       //TTS������8��,!-8,0�Ǿ���ʹ�á�

void IntoNextVoiceStatus(u8 Val) ; //״̬ת��
static unsigned char TTSApp_PackVoiceStr(u8 *ptr,u8 datalen) ;   //�������
static u8 TTSApp_PlayMulTTSVoiceStrbase(u8 *StrPtr,u8 type);

void TTSApp_StatusVoiceInit(void);
void TTSApp_StatusVoiceLoad(void);
void TTSApp_StatusVoicePlay(void);
void TTSApp_StatusVoiceEnd(void);
void TTSApp_TimeStatusTask(void);
void TTSApp_CheckExtTtsTimeOut(void);
////////////////////////////////////
enum 
{
    TTS_TIMER_TASK,                     //  
    TTS_TIMER_POWER,                //    
    TTS_TIMERS_MAX
}STTTSTIME;

static LZM_TIMER s_stTTSTimer[TTS_TIMERS_MAX];
/********************************************************************
* ��  ��:TTSApp_TTSPowerOff
* ��  ��:ֹͣ��ǰ���ڲ���������
* ��  ��:
* ȫ�ֱ���:
* ��ע: 
********************************************************************/
void TTSApp_TurnOffPower(void) 
{
    s_stTtsAttirb.sendDataLen=0;
    s_stTtsAttirb.curSendLen = 0;
    s_stTtsAttirb.voiceDataLen = 0;
    IntoNextVoiceStatus(E_VOICE_MAX);
    LZM_PublicKillTimer(&s_stTTSTimer[TTS_TIMER_POWER]);
    s_stTtsAttirb.busy=0;            //�ر�TTSȫ��æ��־    
    #ifndef EYE_MODEM
    if (gPhone.status == GSM_PHONE_IDLE)                          //dxl  
    #endif
    Io_WriteSpeakerConrtolBit(SPEAKER_BIT_TTS, RESET);  
    TTSDerv_CtrlTTSPower(S_OFF_TTS_POWER);    //�ر�TTS��Դ  
    ClrTimerTask(TIME_TTSDELAY);
}
/********************************************************************
* ����:TTSApp_TTSPowerOff
* ����:
* ���:
* ȫ�ֱ���:
*
* ��ע: 
********************************************************************/
void TTSApp_TurnOnPower(void) 
{
    TTSDerv_CtrlTTSPower(S_ON_TTS_POWER);            //��TTS��Դ
    s_stTtsAttirb.sendDataLen = 0;
    Io_WriteSpeakerConrtolBit(SPEAKER_BIT_TTS, SET);
    TTSDev_ResetTTS();
    SetTimerTask(TIME_TTSDELAY,1);
}
/********************************************************************
* ����:�������
* ����:ָ�������ֽڵ�ָ��
* ���:
* ȫ�ֱ���:
*
* ��ע: �������ⲿ��������.
********************************************************************/
static unsigned char TTSApp_CheckPackVoiceStr(void) 
{
    ST_TTS_PRO stTTSApp;
    u8 vlen = 0;
    if(GpioInGetState(TTS_RDY))
    {
        memset(&stTTSApp.VoiceData[0],0,TTS_VOICE_SIZE);                  //��ջ�����
        sprintf((char*)&(stTTSApp.VoiceData[0]),"[v%d]",0);
        vlen = strlen((char*)&(stTTSApp.VoiceData[0]));
        memcpy((u8 *)&(stTTSApp.VoiceData[vlen]),"...",3);  
        vlen = 3 + vlen;          //���¼����ܳ���
        stTTSApp.Head=0xFD;  //��ͷ
        stTTSApp.Cmd = 1;    //�����ϳ�
        stTTSApp.FontStyle=0; //GB2313
        stTTSApp.Len[0]=((2+vlen)>>8) & 0xFF;
        stTTSApp.Len[1]=(2+vlen) & 0xFF;
        vlen+=5; 
        stTTSApp.PageLen = vlen;
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_POWER],PUBLICSECS(8),TTSApp_TurnOffPower); 
        return TTSDerv_SendUartData((u8 *)&stTTSApp,stTTSApp.PageLen); //������������������,��������
    }
    return 0;
}
/********************************************************************
* ��  ��:TTSApp_TTSPowerOff
* ��  ��:ֹͣ��ǰ���ڲ���������
* ��  ��:
* ȫ�ֱ���:
* ��ע: 
********************************************************************/
static unsigned char TTSApp_PlayExtTtsFlag(void)
{
    return (s_stTtsAttirb.extTtsFlag==TTS_USE_EXT)?1:0;
}
/********************************************************************
* ����:�������
* ����:ָ�������ֽڵ�ָ��
* ���:
* ȫ�ֱ���:
*
* ��ע: �������ⲿ��������.
********************************************************************/
static unsigned char TTSApp_PackVoiceStr(u8 *ptr,u8 datalen) 
{
    ST_TTS_PRO stTTSApp;
    u32 Len = 0;
    u8 vlen = 0;
    u8 time = 0; 
    char *pvstrd ="[v%d]";       //���� yan add
    if(0==datalen)return 1;
    if(GpioInGetState(TTS_RDY))
    {
        //���Э��
        Len = datalen;
        if(Len > TTS_VOICE_SIZE-5)Len = TTS_VOICE_SIZE-5;
        memset(&stTTSApp.VoiceData[0],0,TTS_VOICE_SIZE);                  //��ջ�����
        sprintf((char*)&(stTTSApp.VoiceData[0]),pvstrd,ttsValue[ttsVolume]);
        vlen = strlen((char*)&(stTTSApp.VoiceData[0]));
        memcpy((u8 *)&(stTTSApp.VoiceData[vlen]),ptr,Len);  
        Len = Len + vlen;          //���¼����ܳ���
        stTTSApp.Head=0xFD;  //��ͷ
        stTTSApp.Cmd = 1;    //�����ϳ�
        stTTSApp.FontStyle=0; //GB2313
        stTTSApp.Len[0]=((2+Len)>>8) & 0xFF;
        stTTSApp.Len[1]=(2+Len) & 0xFF;
        Len+=5;
        if(Len > TTS_MAX_SEND_LEN)
            Len=TTS_MAX_SEND_LEN;
        stTTSApp.PageLen = Len;
        time = (stTTSApp.PageLen/5) + 6;
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_POWER],PUBLICSECS(time),TTSApp_TurnOffPower); 
        return TTSDerv_SendUartData((u8 *)&stTTSApp,stTTSApp.PageLen); //������������������,��������
    }
    /////////
    return 0;
}
/*************************************************************
** ��������: PlayMulTTSVoiceBuffExternal
** ��������: ʹ���ⲿTTS��������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
u8 PlayMulTTSVoiceBuffExternal(u8 *buff,u16 len,unsigned char type)              //������������
{   
    u16 Time;
    u8 psoundlen = 0;
    char *psound =USE_INFO_SOUND" ";              //��Ҫ���ӿո�,��ͨ��Ϣ��ʾ��.
    char *psoundAlarm =ALARM_INFO_SOUND" ";       //��Ҫ���ӿո�,������ʾ��.
    
    //LOG_PR("TTSApp_PlayMulTTSVoiceStrbase:[%s] \r\n",buff);
    
    #ifdef EYE_MODEM
    if((buff == NULL)||MODON_PHONE == Modem_Api_OnSta()) 
    #else
    if((buff == NULL)||(gPhone.status == GSM_PHONE_TALK))
    #endif
    {
        return 0;
    }  
    #ifndef EYE_MODEM
    if (gPhone.status == GSM_PHONE_IDLE)                          //dxl  
	    Io_WriteSpeakerConrtolBit(SPEAKER_BIT_TTS, RESET);
    #endif
    ///////////
    TTSApp_TurnOnPower(); //
    //////////
    if(type == TTS_PLAY_TYPE_USE)
    {
        strcpy((char *)&s_ucTtsPlayVoiceData[0],(char const *)psound);       //yan add- ��ͨ��Ϣ
        psoundlen = strlen(psound);
    }else{
        strcpy((char *)&s_ucTtsPlayVoiceData[0],(char const *)psoundAlarm);  //yan add- ������Ϣ
        psoundlen = strlen(psoundAlarm);        
    }
    
    s_stTtsAttirb.voiceDataLen = len;
    if((s_stTtsAttirb.voiceDataLen+psoundlen) >VOICE_DATA_LEN)
    {
        s_stTtsAttirb.voiceDataLen = VOICE_DATA_LEN-psoundlen;
    }   

    strncpy((char *)&s_ucTtsPlayVoiceData[psoundlen] ,(char const *)buff,s_stTtsAttirb.voiceDataLen);
    s_stTtsAttirb.voiceDataLen += psoundlen;
      
    s_ucTtsPlayVoiceData[s_stTtsAttirb.voiceDataLen]='\0';
    s_stTtsAttirb.busy = 1;
    IntoNextVoiceStatus(E_VOICE_INIT);
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],LZM_AT_ONCE,TTSApp_TimeStatusTask);
    ///////////////////////////
    Time=s_stTtsAttirb.voiceDataLen/5 ; //5���ֽ�1s 
    if(s_stTtsAttirb.voiceDataLen%5)Time++;
    Time += 10;   //���10s��ʱ
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_POWER],PUBLICSECS(Time),TTSApp_TurnOffPower);
    return 1; //�ɹ�
}
/*************************************************************
** ��������: PlayMulTTSVoiceBuffInternal
** ��������: ʹ���ڲ�TTS��������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
u8 PlayMulTTSVoiceBuffInternal(u8 *buff,u16 len)              //������������
{
    #ifdef EYE_MODEM
    if(MODON_PHONE == Modem_Api_OnSta())return 0;  
    /////////////////////////
    if(2==Modem_Api_Type())////����ģ�����ֻ֧��140���ֽ�
    {
        s_stTtsAttirb.oneMaxLen = 100;
        s_stTtsAttirb.oneMinLen = 64;
    }
    else
    {
        s_stTtsAttirb.oneMaxLen = 226;
        s_stTtsAttirb.oneMinLen = 180;
    }
    ////////////////////////
    if(len>s_stTtsAttirb.oneMaxLen)
    {
        s_stTtsAttirb.sendDataLen = 0;                   //�������
        s_stTtsAttirb.voiceDataLen = len;
        if(s_stTtsAttirb.voiceDataLen >VOICE_DATA_LEN)
        {
            s_stTtsAttirb.voiceDataLen = VOICE_DATA_LEN;
        } 
        memcpy(s_ucTtsPlayVoiceData,buff,s_stTtsAttirb.voiceDataLen);
        ///////////////////////
        s_stTtsAttirb.busy = 1;
        IntoNextVoiceStatus(E_VOICE_LOAD);
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],LZM_AT_ONCE,TTSApp_TimeStatusTask);
        Io_WriteSpeakerConrtolBit(SPEAKER_BIT_TTS, RESET);
        SetTimerTask(TIME_TTSDELAY,1);
        len=s_stTtsAttirb.voiceDataLen/5 ; //5���ֽ�1s 
        if(s_stTtsAttirb.voiceDataLen%5)len++;
        len += 10;   //���10s��ʱ
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_POWER],PUBLICSECS(len),TTSApp_TurnOffPower);        
    }
    else
    {
        TTSApp_TurnOffPower(); //
        return Modem_Api_Tts_Play(0,buff,len);
    }
    #endif
    return 1;
}

//-----------------------------------------------------------------------------------
//����:�ֶβ�������,��ͨ��Ϣ
//���������StrPtr �ַ���
//          type    ��ͨ��Ϣ�򱨾���Ϣ
//���أ�
//-----------------------------------------------------------------------------------
u8 PlayMulTTSVoiceBuff(u8 *buff,u16 len,unsigned char type)              //������������
{
	  s_stTtsAttirb.extTtsFlag = TTS_USE_INT;//dxl,2016.9.1ǿ��ʹ���ڲ�TTS
    if(0==len)return 0; 
    if(TTS_USE_EMPTY==s_stTtsAttirb.extTtsFlag)return 0;
    if(TTSApp_PlayExtTtsFlag())
    {
        s_stTtsAttirb.oneMaxLen = 172;
        s_stTtsAttirb.oneMinLen = 128;
        return PlayMulTTSVoiceBuffExternal(buff,len,type);
    }
    #ifdef EYE_MODEM
    else
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
       return PlayMulTTSVoiceBuffInternal(buff,len); 
    }
    #endif
    return 0;
}
/*************************************************************
** ��������: TTSApp_PlayMulTTSVoiceStrbase
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
static u8 TTSApp_PlayMulTTSVoiceStrbase(u8 *StrPtr,u8 type) 
{   
    return PlayMulTTSVoiceBuff(StrPtr,strlen((char const *)StrPtr),type);
}
/*************************************************************
** ��������: TTSApp_StatusVoiceInit
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void TTSApp_StatusVoiceInit(void)
{
    s_stTtsAttirb.sendDataLen = 0;                   //�������
    Io_WriteSpeakerConrtolBit(SPEAKER_BIT_TTS, SET);
    IntoNextVoiceStatus(E_VOICE_LOAD);
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(0.7),TTSApp_TimeStatusTask);
}
/*************************************************************
** ��������: TTSApp_StatusVoiceInit
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void TTSApp_PackVoiceStrToSend(void)
{
    unsigned char flag;
    if(TTSApp_PlayExtTtsFlag())
    {
        flag=TTSApp_PackVoiceStr(s_stTtsAttirb.sendPtr,s_stTtsAttirb.curSendLen);    //�������        
    }
    #ifdef EYE_MODEM 
    else
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        flag=Modem_Api_Tts_Play(0,s_stTtsAttirb.sendPtr,s_stTtsAttirb.curSendLen);
        flag = 1;
    }    
    #endif    
    ///////////////////
    if(flag||(0==s_stTtsAttirb.curSendLen))
    {
        IntoNextVoiceStatus(E_VOICE_PLAY);
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(4),TTSApp_TimeStatusTask); 
    }
    else
    {
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(0.1),TTSApp_PackVoiceStrToSend); 
    }
}
/*************************************************************
** ��������: TTSApp_StatusVoiceInit
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void TTSApp_StatusVoiceLoad(void)
{
    u16 len;
    u8 chr,temp;
    ////////////////
    s_stTtsAttirb.curSendLen = 0;
    //////////////////////
    if(s_stTtsAttirb.sendDataLen<s_stTtsAttirb.voiceDataLen)
    {
        s_stTtsAttirb.sendPtr=&s_ucTtsPlayVoiceData[s_stTtsAttirb.sendDataLen];
        ////////////////////
        if(0==s_stTtsAttirb.oneMaxLen)//
        {
            s_stTtsAttirb.oneMaxLen = 100;
            s_stTtsAttirb.oneMinLen = 64;
        }
        len=s_stTtsAttirb.voiceDataLen-s_stTtsAttirb.sendDataLen;
        if(len>s_stTtsAttirb.oneMaxLen)len = s_stTtsAttirb.oneMaxLen;
        for( ; s_stTtsAttirb.curSendLen<len; ) //
        {
            if(s_stTtsAttirb.curSendLen<s_stTtsAttirb.oneMinLen)
            {   //������50���ɶ�����
                chr = s_stTtsAttirb.sendPtr[s_stTtsAttirb.curSendLen++];
                if(chr >=0x80)
                {
                    s_stTtsAttirb.curSendLen++;
                }
            }
            else
            {
                temp = s_stTtsAttirb.curSendLen;
                chr  = s_stTtsAttirb.sendPtr[s_stTtsAttirb.curSendLen++];
                if(chr >=0x80)
                {
                    s_stTtsAttirb.curSendLen++;
                    if((0==strncmp((char *)&s_stTtsAttirb.sendPtr[temp],"��",2)) 
                     ||(0==strncmp((char *)&s_stTtsAttirb.sendPtr[temp],"��",2))
                     ||(0==strncmp((char *)&s_stTtsAttirb.sendPtr[temp],"��",2)))
                    {
                        break;
                    }                    
                }
                else
                {  //�ҵ�','����'.'Ϊֹ
                    if(chr==',' || chr=='.')
                    {
                        break;
                    }
                }
            }
        }
        ///////////////////
        s_stTtsAttirb.sendDataLen +=s_stTtsAttirb.curSendLen; 
        if(s_stTtsAttirb.sendDataLen>s_stTtsAttirb.voiceDataLen)//���һ�����ֲ�ȫ,����ʱ�����
        {
            if(s_stTtsAttirb.curSendLen>2)
            {
                s_stTtsAttirb.curSendLen-=2;
            }
            else
            {
                s_stTtsAttirb.curSendLen =0;
            }
        }
        if(s_stTtsAttirb.curSendLen)
        {
            TTSApp_PackVoiceStrToSend();
            return;
        }
    }
    //////////////////
    IntoNextVoiceStatus(E_VOICE_END);
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],LZM_AT_ONCE,TTSApp_TimeStatusTask);
}
/*************************************************************
** ��������: TTSApp_StatusVoiceInit
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void TTSApp_StatusVoicePlay(void)
{
    unsigned char flag;
    if(TTSApp_PlayExtTtsFlag())
    {
        flag=GpioInGetState(TTS_RDY);
        flag^=1;
    }    
    #ifdef EYE_MODEM 
    else
    if(Modem_Api_Tts_IsSpt())
    {
        flag=Modem_Api_Tts_IsSta();
    }    
    #endif    
    ///////////////////////
    if(0==flag)                 //�öβ������,׼��װ����һ��
    {
        IntoNextVoiceStatus(E_VOICE_LOAD);
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],LZM_AT_ONCE,TTSApp_TimeStatusTask);
    }
    else
    {
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(0.1),TTSApp_TimeStatusTask);
    }
}
/*************************************************************
** ��������: TTSApp_StatusVoiceEnd
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void TTSApp_StatusVoiceEnd(void)
{
    IntoNextVoiceStatus(E_VOICE_MAX);
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_POWER],LZM_AT_ONCE,TTSApp_TurnOffPower); 
}
/*******************************************************************************
**  ��������  : CommPJA_TimeStatusTask
**  ��������  : ������ģ��ʱ��������Ƚӿں���
**  ��    ��  : ��				
**  ��    ��  : �������״̬:  ʹ�ܻ��ֹ
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void TTSApp_TimeStatusTask(void) 
{
    #ifdef EYE_MODEM
    if(0==s_stTtsAttirb.busy||MODON_PHONE == Modem_Api_OnSta())
    #else
    if(0==s_stTtsAttirb.busy||gPhone.status != GSM_PHONE_IDLE)
    #endif
    {
        TTSApp_TurnOffPower();
        return;
    }
    switch(s_stTtsAttirb.status)
    {
        case E_VOICE_INIT:
            TTSApp_StatusVoiceInit();
            break;
        case E_VOICE_LOAD:
            TTSApp_StatusVoiceLoad();
            break;
        case E_VOICE_PLAY:
            TTSApp_StatusVoicePlay();
            break;
        case E_VOICE_END:
            TTSApp_StatusVoiceEnd();
            break;
        default:
            TTSApp_StatusVoiceEnd();
            break;
    }
}


/********************************************************************
* ����:״̬ת��
* ����:״̬��
* ���:
* ȫ�ֱ���:
*
* ��ע:
********************************************************************/
void IntoNextVoiceStatus(u8 Val) 
{
	s_stTtsAttirb.status = Val;
}
//-----------------------------------------------------------------------------------
//����:�ֶβ�������,��ͨ��Ϣ
//����������ַ���
//���أ�
//-----------------------------------------------------------------------------------
u8 PlayMulTTSVoiceStr(u8 *StrPtr) 
{   
    return TTSApp_PlayMulTTSVoiceStrbase(StrPtr,TTS_PLAY_TYPE_USE);
}

//-----------------------------------------------------------------------------------
//����:�ֶβ�������,������Ϣ
//����������ַ���
//���أ�
//-----------------------------------------------------------------------------------
u8 PlayMulTTSVoiceAlarmStr(u8 *StrPtr) 
{   
    return TTSApp_PlayMulTTSVoiceStrbase(StrPtr,TTS_PLAY_TYPE_ALARM);
}

//-----------------------------------------------------------------------------------
//����:��������
//���������
//���أ�
//----------------------------------------------------------------------------------- 
void SetTtsVolumeIncr(void)
{
    if(TTSApp_PlayExtTtsFlag())
    {
        if(ttsVolume < 8) 
        {
            ttsVolume++;         //�������ֵ��16
            FRAM_BufferWrite(FRAM_TTS_VOLUME_ADDR,(u8*)&ttsVolume,FRAM_TTS_VOLUME_LEN);
        }
    }
    #ifdef EYE_MODEM 
    else
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        SetModTtsVolumeIncr();
    }    
    #endif
    
}
//-----------------------------------------------------------------------------------
//����:�����ݼ�
//���������
//���أ�
//----------------------------------------------------------------------------------- 
void SetTtsVolumeDecr(void)
{
    if(TTSApp_PlayExtTtsFlag())
    {
        if(ttsVolume > 1)
        {
            ttsVolume--;         //������Сָ��0,����
            FRAM_BufferWrite(FRAM_TTS_VOLUME_ADDR,(u8*)&ttsVolume,FRAM_TTS_VOLUME_LEN);
        }
    }
    #ifdef EYE_MODEM 
    else
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        SetModTtsVolumeDecr();
    }    
    #endif    
}

//-----------------------------------------------------------------------------------
//����:��ȡTTS��ǰ����
//���������
//���أ�
//----------------------------------------------------------------------------------- 
u8 GetTtsVolume(void)
{
    if(TTSApp_PlayExtTtsFlag())
    {
        return ttsVolume;
    }
    #ifdef EYE_MODEM 
    else
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        return GetModTtsVolume();
    }    
    #endif 
    return 0;
}
/********************************************************************
* ���� : TtsVolumeInit
* ���� : TTS������ʼ��
* ���� : ��
* ��� : ��
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע:
********************************************************************/
void TtsVolumeInit(void)
{
    FRAM_BufferRead((u8*)&ttsVolume,FRAM_TTS_VOLUME_LEN,FRAM_TTS_VOLUME_ADDR);
    if((ttsVolume == 0) || (ttsVolume >8))
    {
        ttsVolume = 5;
    }    
}
/********************************************************************
* ��   �� : TTS_GetPlayBusyFlag
* ��   �� : ȡ�ò���æ��־
* ��   �� : ��
* ��   �� : ���ڲ�������1, ������Ϸ���0
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  
* ��ע:
********************************************************************/
unsigned char TTS_GetPlayBusyFlag(void)
{
    if(TTSApp_PlayExtTtsFlag())
    {
        return TtsPowerFlag;
    }
    #ifdef EYE_MODEM
    else
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        return Modem_Api_Tts_IsSta();
    }
    #endif
    return 0;    
}
/************************************************************
��������: TTSApp_CheckExtTtsFlag
��������: �豸��ȫ�ֱ�����ʼ��
��ڲ���: 
���ڲ���:
************************************************************/
void TTSApp_CheckRDYFlag(void)
{
    s_stTtsAttirb.checkRDYCnt++;
    if(0==GpioInGetState(TTS_RDY))
    {
        s_stTtsAttirb.checkCnt =0;
        s_stTtsAttirb.checkRDYCnt = 0;
        s_stTtsAttirb.extTtsFlag = TTS_USE_EXT;
        TTSApp_TurnOffPower();
    }
    else   
    if(s_stTtsAttirb.checkRDYCnt>50)
    {
        TTSApp_CheckExtTtsTimeOut();
    }
    else
    {
       LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(0.1),TTSApp_CheckRDYFlag);
    }
    
}
/************************************************************
��������: TTSApp_CheckExtTtsFlag
��������: �豸��ȫ�ֱ�����ʼ��
��ڲ���: 
���ڲ���:
************************************************************/
void TTSApp_CheckExtTtsFlag(void)
{
    unsigned char result;
    s_stTtsAttirb.checkCnt++;
    result=TTSApp_CheckPackVoiceStr();
    if(result)
    {
        s_stTtsAttirb.checkRDYCnt = 0;
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],1,TTSApp_CheckRDYFlag); 
    }
    else
    {
        TTSApp_CheckExtTtsTimeOut();
    }    
}
/************************************************************
��������: TTSApp_CheckExtTtsFlag
��������: �豸��ȫ�ֱ�����ʼ��
��ڲ���: 
���ڲ���:
************************************************************/
void TTSApp_CheckExtTtsTimeOut(void)
{
    s_stTtsAttirb.checkRDYCnt = 0;
    if(s_stTtsAttirb.checkCnt<3)
    {
        LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(0.5),TTSApp_CheckExtTtsFlag); 
    }
    else
    {
        s_stTtsAttirb.checkCnt =0;
        s_stTtsAttirb.checkRDYCnt = 0;
        s_stTtsAttirb.extTtsFlag = TTS_USE_INT;
        TTSApp_TurnOffPower();
    }
}
/************************************************************
��������: 
��������: �豸��ȫ�ֱ�����ʼ��
��ڲ���: 
���ڲ���:
************************************************************/
void TTS_Init(void)
{
    memset(&s_stTtsAttirb,0,sizeof(s_stTtsAttirb));
    TtsVolumeInit();
    //�豸��ʼ��
    TTSDev_UartInit();             //TTS���ڳ�ʼ�� 
    //ȫ�ֱ�����ʼ��
    TTSApp_TurnOffPower();    
    /////////////
    s_stTtsAttirb.oneMaxLen = 172;
    s_stTtsAttirb.oneMinLen = 120;
    ///////////////////
    Io_WriteSpeakerConrtolBit(SPEAKER_BIT_TTS, RESET); 
    LZM_PublicKillTimerAll(s_stTTSTimer,TTS_TIMERS_MAX);
    /////////////////////////
    TTSApp_TurnOnPower();
    ///////////////////
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_TASK],PUBLICSECS(1),TTSApp_CheckExtTtsFlag);    
    ///////////////////
    LZM_PublicSetOnceTimer(&s_stTTSTimer[TTS_TIMER_POWER],PUBLICSECS(25),TTSApp_TurnOffPower);
}
/************************************************************
��������: 
��������: ��������
��ڲ���: 
���ڲ���:
��ע: ÿ20ms����һ��
************************************************************/ 
FunctionalState TTS_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stTTSTimer,TTS_TIMERS_MAX);
    return ENABLE;
}
/******************************************************************************
**                            End Of File
******************************************************************************/

