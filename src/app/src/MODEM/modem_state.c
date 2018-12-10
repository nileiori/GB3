/**
  ******************************************************************************
  * @file    modem_state.c
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2013-08-21
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
#define CSQSTAT_TIME                       (60*MODEM_TSEC)
#define PHONE_TIME_CLOSE                   (60*MODEM_TSEC)//�ر���·ǰ�ͨ��ʱ��

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES   ���ر���
*********************************************************************************************************
*/

static SOC_ATBUF    soc_atbuf;
static MODSOC_STA   Modem_Phone_SocSta[MODEM_SOC_NUM] 
                             = {MODSOC_IDLE};//���ڳ�ʱ��ͨ��ǿ�������־ʱ�ݴ�
static ST_MODOTH StOtherState;//�����״̬��״̬

static MODON_STA    ModemStateDelay;//��ʱָ��ʱ����л�״̬��״̬
static u32 ModemDelayCount;//��ʱָ��ʱ����л�״̬�ļ�����
static u8  ModemDelayStateFlg;//��ʱָ��ʱ����л�״̬�ı�־λ
static u32 ModemSimNoTime;//����û��װsim����ʱ��
static u32 ModemAtdOutTime;//����绰��ʱʱ��
static u32 ModemPhoneTm;//ͨ��ʱ�������
static u8  ModemOffFLag;//�ػ���־

static u16 TtsOutTimeCount;//tts�쳣��ʱ������

static u16 CsqStatIsCnt;//��������������״̬��ѯ������
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES    ȫ�ֱ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/*
  * @brief  modem��ʼ������Ҫ�д��ڡ�IO��.
  * @param  None
  * @retval None
  */
void Modem_State_Init (void)
{      
    u8 i;
    
    ///////////////////////////////////////////////////////////////modem��������
    modInf.Num_Rest++;
    //////////////////////////////////////////////////////////////��ʼ�����ݽṹ
    memset((u8*)&modsmspdu,0,sizeof(SM_PARAM));
    memset((u8*)&modsms,0,sizeof(ST_MODSMS));
    memset((u8*)&modphone,0,sizeof(ST_MODHP));
    memset((u8*)&modftp,0,sizeof(ST_MODFTP));
    memset(Modem_IpSock_STA,MODSOC_IDLE,sizeof(Modem_IpSock_STA));
    modem.csq   = 0;
    modem.onsta = MODON_IDLE;
    modem.state = MODEM_ACT;//����ģ��
    modem.index = 0;  

    for(i=0;i<MODEM_SOC_NUM;i++)
    {
        ModLib_NodRst(&ModSocNode[i]);
    }
                
    ////////////////////////////////////////////////////////////////��ʼ�����ӿ�
    Modem_Api_Init   (); 
    Modem_Port_Init  ();
    #ifdef MODEM_DEBUG_H
    Modem_Debug_Init ();
    #endif
    Modem_Uart_NodRst();
    
    Modem_PrtfTo(MODPRTF_INIT);
     
}
/**
  * @brief  ����״̬����ʱָ��ʱ����л�״̬
  * @param  delay:��ʱ��ʱ��,��λΪ��
  * @param  state:Ҫ�л���״̬
  * @retval None
  */
void  Modem_State_Delay(u16 delay,MODON_STA state)
{
    ModemDelayCount = delay*MODEM_TSEC;
    ModemStateDelay = state;
    ModemDelayStateFlg = 1;
}

/**
  * @brief  ����״̬����ʱָ��ʱ����л�״̬
  * @param  None
  * @retval None
  */
void Modem_State_DelayPro(void)
{
    if(modapi.wait != MODON_IDLE)
    {
        return ;//modem��æ
    } 
    if(ModemDelayStateFlg)
    {
        if(ModemDelayCount == 0)
        {
            modem.onsta = ModemStateDelay;
            ModemDelayStateFlg = 0;
        }
        else
        {
            ModemDelayCount--;
        }
    }
}
/*
  * @brief  modem����
  * @param  type : 0->ģ������;
  *                1->�б�����;
  * @retval ���ص�ǰģ������
  */
u8  Modem_State_Type (u8 type) 
{
    u8 value=0;
    
    if(type == 0)
    {
        value = Modem_Type;
    }
    else
    {
        if(Modem_Type == MODEM_TYPE_Q)////////////��Զ G��
        {              
            value = value | 0x01;
        }                
        else if(Modem_Type == MODEM_TYPE_ZTE)/////���� C��
        {
            value = value | 0x02;
        }  
        else
        {
            value = value | 0x01;//Ĭ��G��
        }
    }

    return value;
}

/*
  * @brief  modem��״̬.
  * @param  None
  * @retval ����modem������״̬
  */
MODEM_STA  Modem_State_Is (void) 
{          
    return modem.state;  
}
/**
  * @brief  ����״̬��״̬
  * @param  None.
  * @retval None.
  */
void Modem_State_Push (void) 
{
    StOtherState.othdly      = modem.delay;
    StOtherState.othwait     = modem.wait;    
    StOtherState.othindex    = modem.index;
    StOtherState.othsta      = modem.onsta;
    StOtherState.othtryadd   = modat.tryadd;
    StOtherState.othtrynum   = modat.trynum;

    modem.delay = 0;//����ִ���¶���
    modem.wait  = MODWT_IDLE;
}

/**
  * @brief  ��ԭ״̬��״̬
  * @param  None.
  * @retval None.
  */
void Modem_State_Pop (void) 
{
    modem.delay          = StOtherState.othdly;
    modem.wait           = StOtherState.othwait;
    modem.index          = StOtherState.othindex;
    modem.onsta      = StOtherState.othsta;
    modat.tryadd = StOtherState.othtryadd; 
    modat.trynum = StOtherState.othtrynum;
    modapi.wait          = StOtherState.othsta;
}
/*
  * @brief  �л�modem��״̬
  * @param  state:Ҫ�л���״̬
  * @retval None
  */
void  Modem_State_Switch (MODEM_STA state) 
{          
    modem.state    = state;
    modem.index    = 0;
    modem.delay    = 0;
}

/*
  * @brief  �л�����״̬�µ�modem״̬
  * @param  state:Ҫ�л���״̬
  * @retval None
  */
void  Modem_State_SwitchOn (MODON_STA state) 
{          
    modem.onsta= state;
    modem.index    = 0;
    if(state == MODON_OFF)
    {
        ModemOffFLag = 1;
    }    
}

/*
  * @brief  �л��绰״̬��
  * @param  state:Ҫ�л���״̬
  * @retval None
  */
void  Modem_State_SwitchPhone (MODPH_STA state) 
{          
    modphone.state= state;
    modem.index = 0;
}
/*
  * @brief  ����Ƿ���Ҫ���͵�����.
  * @param  None
  * @retval �����ݷ��ͷ���1, �����ݷ��ͷ���0
  */
u8  Modem_State_CheckSoc (void)
{
    u8 i;
	
    for(i=0;i<MODEM_SOC_NUM;i++)
    {
        if(ModLib_NodNum(&ModSocNode[i]))
        {
             soc_atbuf.soc = i;
             soc_atbuf.len = ModLib_NodRet(&ModSocNode[i],soc_atbuf.buf);
            //if(Modem_IpSock_STA[i] == MODSOC_ONLN) dxl,2017.4.12,����
					  if((Modem_IpSock_STA[i] == MODSOC_ONLN)&&(modem.onsta != MODON_IPSEND))
            {
                modapi.wait   = MODON_IPSEND;//����ȴ�״̬ 
                Modem_State_SwitchOn(MODON_IPSEND);//�л�״̬��
            }
            return 1;
        }
    }
    return 0;
}
/*
  * @brief  ����Ƿ��ж�����Ϣ
  * @param  None
  * @retval ����Ϣ�����棬���򷵻ؼ�
  */
BOOL  Modem_State_CheckSms (void)
{
    if(modapi.wait != MODON_IDLE)
    {
        return (__FALSE);//modem��æ
    }   
    if(modsms.busy != 0)
    {
        return (__FALSE);//���ڴ������ҵ��
    }

    if(modsms.rec)//���ն���
    {
        modsms.busy = 1;
        modapi.wait = MODON_SMSREC;//������ն���
        Modem_State_SwitchOn(MODON_SMSREC);
        return (__TRUE);
    }
    else if(modsms.send)//���Ͷ���
    {
        modsms.busy = 1;
        modapi.wait = MODON_SMSSEND;//�����Ͷ���
        Modem_State_SwitchOn(MODON_SMSSEND);
        return (__TRUE);
    }
    return (__FALSE);
}
/*
  * @brief  ����Ƿ��е绰Ҫ����
  * @param  None
  * @retval ����Ϣ�����棬���򷵻ؼ�
  */
BOOL  Modem_State_CheckPhone (void)
{
    if(modapi.wait != MODON_IDLE)
    {
        return (__FALSE);//modem��æ
    }   
    return (__TRUE);
}
/*
  * @brief  modem״̬�����.�ú�����run�����в���״̬��Ӱ��
  * @param  None
  * @retval None
  */
void  Modem_State_CheckRun (void) 
{       
    static u32 CsqOutTimeCount;//��ѯ�ź�����������,���ڼ���ں��������

    if(modem.atack > 10)//����10�������Ӧ������쳣����
    {
        modem.atack = 0;
        Modem_State_Switch(MODEM_ERROR);
    }

    if(modem.onsta == MODON_CSQ)
    {
        CsqOutTimeCount = 0;
    }
    else if(modem.onsta == MODON_PHONE)
    {
        CsqOutTimeCount = 0;
    }
    
    if(modem.onsta != MODON_CSQ)
    {
        CsqOutTimeCount++;
        if(CsqOutTimeCount > MODEM_TSEC*1800)
        {
            CsqOutTimeCount = 0;
            Modem_State_Switch(MODEM_ERROR);//����30���Ӳ���ѯ�ź�������쳣����
        }
    }
}
/*
  * @brief  ������Ƿ������л�״̬
  * @param  None
  * @retval Ӧ�ò�������Ҫ���ͷ����棬���򷵻ؼ�
  */
BOOL  Modem_State_Check (void)
{
    if(modftp.busy)
    {
        return (__FALSE);//modemæ
    }
    Modem_State_CheckSoc();//���soc�����Ƿ�������Ҫ���� 
    Modem_State_CheckSms();//����Ƿ����¶��Ż�Ҫ���͵Ķ���
    Modem_State_CheckPhone();//����Ƿ��������Ҫ����ĵ绰
    Modem_State_DelayPro();
    return (__TRUE);
}

/*
  * @brief  modem������
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @retval None
  */
void  Modem_State_Err (MODEM_STA staok)
{
    ////////////////////////////////////////////////////////////////��������־
    Modem_State_Init();

    ////////////////////////////////////////////////////////////////////�л�״̬
    Modem_PrtfTo(MODPRTF_ERR);
    Modem_State_Switch(staok);
}

/*
  * @brief  �ر�modem,.
  * @param  None ��һ״̬�ɺ����ڲ�����
  * @retval None
  */
void  Modem_State_Off (void)
{   
    if(modtts.busy)
    {
        return;//�ȴ��������
    }
    switch (modem.index)
    {
        ////////////////////////////////////////////////////////////////////�ػ�
        case 0:
            {
                Modem_Bsp_SetActive(0);
                Modem_Delay(Modem_ActPinOffDly);
                modem.index++;
            }
            break;
        ////////////////////////////////////////////////////////////////ע������
        case 1:
            {
                Modem_Bsp_SetActive(1);
                Modem_Delay(15);
                modem.index++;
            }
            break;
        ////////////////////////////////////////////////////////////////////�ŵ�
        case 2:
            {                
                Modem_Bsp_SetPower(0);
                Modem_Delay(30);
                modem.index++;
                Modem_PrtfTo(MODPRTF_POWOFF);
            }
            break; 
        ////////////////////////////////////////////////////////�ж��Ƿ��ڹػ� 
        case 3:
            {                
                if(ModemOffFLag)
                {
                    ModemOffFLag = 0;
                    Modem_State_Switch(MODEM_IDLE);//�ȴ�Ӧ�ÿ���
                    Modem_PrtfTo(MODPRTF_IDLE);
                    Modem_PrtfTo(MODPRTF_WTAPP);
                }
                else
                {
                    Modem_State_Switch(MODEM_ACT);//��������

                }
            }
            break;
    }
}

/*
  * @brief  ����modem,.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Act (MODEM_STA staok,MODEM_STA staerr) 
{   

    switch (modem.index)
    {
        case 0:
            {
                Modem_PrtfTo(MODPRTF_POWON);
                Modem_Bsp_SetPower(1);
                modem.index++;
            }
            break;
        case 1:
            {  
                Modem_Bsp_SetActive(0);
                Modem_Delay(Modem_ActPinOnDly);
                modem.index++;
            }
            break;
        case 2:
            {
                Modem_Bsp_SetActive(1);
                Modem_PrtfTo(MODPRTF_WTAT);
                Modem_Delay(5);
                modem.index++;
            }
            break;               
        case 3:
            {
                Modem_PrtfTo(MODPRTF_SDAT);
                Modem_Send_cmd_Base("AT\r","OK\r",2,30);
            }
            break; 
        case 4://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                Modem_State_Switch(staerr);
            }
            break;            
    }
}




/*
  * @brief  ��ȡmodem��Ϣ,.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Ati (MODEM_STA staok,MODEM_STA staerr)
{
    switch (modem.index)
    {
        case 0://�رջ���
            {
                Modem_Send_cmd_Base("ATE0\r","OK\r",1,3);
            }
            break;      
        case 1://��ѯ��Ϣ
            {
                Modem_Send_cmd_Ati(2,5);
            }
            break;
        case 2://�ȴ�10Sģ�����
            {
                Modem_Delay(20);
                modem.index++;
                Modem_PrtfTo(MODPRTF_WTRD);
            }
            break;        
        case 3://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
            }
            break;         
        default://���ʱ���л���staerr״̬
            {
                Modem_State_Switch(staerr);
            }
            break; 
    }   
}

/*
  * @brief  ����modem��������,.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Aud (MODEM_STA staok,MODEM_STA staerr)
{   
    u8  Tab_t[50];
    //u8  value; dxl,2016.8.12
	  s16 value;
	  u8  i;
    u8  *pCmd =NULL;
    u8 tab_value_Q[10]   = {0,10,20,30,50,65,80,90,100,100};
    u8 tab_value_ZTE[10] = {0,1,2,2,3,4,5,6,7,7};      
		s16 tab_value_TTS[10] = {-32768,-8192,-2048,-512,0,512,2048,8192,32767,32767};
    
    static u8 index_back;
    
    switch (modem.index)
    {
        case 0://��ȡ����
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    memcpy(modphone.clvlvalue,tab_value_Q,10);
                }      
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    memcpy(modphone.clvlvalue,tab_value_ZTE,10);
                } 
//                memcpy(modtts.clvlvalue,modphone.clvlvalue,10);//TTS�����͵绰��������   dxl,2016.8.12
                for(i=0; i<10; i++)
                {
								    modtts.clvlvalue[i] = tab_value_TTS[i];
								}								
                Modem_Port_ReadAud();
                modem.index++;
            }
            break;    
        case 1://����������ʾ
            {
                Modem_Send_cmd_Base("AT+CLIP=1\r","OK\r",1,3);
            }
            break;
        case 2://����ȥ����ʾ
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    Modem_Send_cmd_Base("AT+COLP=1\r","OK\r",1,3);
                }               
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    modem.index++;//����
                }            
            }
            break;
        case 3://����Ĭ������ͨ��
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+QAUDCH=0\r";
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+SPEAKER=1\r";
                }     
                Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
            }
            break;
        case 4://����Ĭ��ͨ��������С
            {
                value = modphone.clvlvalue[modphone.clvl];
                sprintf((char*)Tab_t,"AT+CLVL=%d\r",value);

                if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    sprintf((char*)Tab_t,"AT+VGR=%d\r",value);
                }   
                pCmd = Tab_t;
                Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->Phone Clvl = %d\r\n\r\n",value);
                }
                #endif
            }
            break;
        case 5://����TTS������С
            {
                value = modtts.clvlvalue[modtts.clvl];
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {         
                    strcpy((char*)Tab_t,"AT\r");
                    if(QuectelAtTtsFlag)
                    {
                        //sprintf((char*)Tab_t,"AT+QMEDVL=%d\r",value);dxl,2016.8.12
											  if(value >= 0)
												{
											      sprintf((char*)Tab_t,"AT+QTTSETUP=1,2,%d\r",value);
												}
												else
												{
													  value = -value;
												    sprintf((char*)Tab_t,"AT+QTTSETUP=1,2,-%d\r",value);
												}
                    }
                }       
                else if(Modem_Type == MODEM_TYPE_ZTE)/////���� AT+ZTTSP=3,1,3
                {
                    sprintf((char*)Tab_t,"AT+ZTTSP=%d,1,3\r",value);
                } 
                pCmd = Tab_t;
                Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->TTS   Clvl = %d\r\n\r\n",value);
                }
                #endif                
            }
            break;            
        case 6://����Ĭ��ͨ����˷����
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+QMIC=0,13\r";
                }                 
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+VGT=0\r";
                } 
                Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
            }
            break;
        case 7://���ò�������
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+QSIDET=0\r";
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+SIDET=1,0\r";//Ԥ��
                } 
                Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
            }
            break;
        case 8://���û�������
            {
                //if(Modem_Type == MODEM_TYPE_Q)////////////��Զ dxl,2015.9.2
                //{              
                   // pCmd = "AT+QECHO=221,100,100,374,1\r";
                //}                
                //else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                //{
                    //pCmd = "AT+ECHO=1\r";
                //} 
                //Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
                modem.index++;//����
            }
            break;
        case 9://������������
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+CRSL=100\r";
                    Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+ZCVF=2\r";
                    Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
                } 
            }
            break;
        case 10://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                #if 0
                    Modem_State_Switch(staerr);
                #else
                    modem.index = index_back+1;//�������,Ϊ��֤���������ʱͨ��,���������
                #endif
            }
            break; 
    }
    index_back = modem.index;
}

/*
  * @brief  ��ѯmodem�������.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_NetSer (MODEM_STA staok,MODEM_STA staerr)
{   
    switch (modem.index)
    {
        case 0://���SIM��
            {
                Modem_PrtfTo(MODPRTF_CPIN);
                Modem_Send_cmd_Search("AT+CPIN?\r",10,5);//Ϊ�˼��ݸ���SIM��������ô��ʱ��
            }
            break;
        case 1://��ѯCCID
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    Modem_Send_cmd_CCID("AT+QCCID\r",1,3);
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    Modem_Send_cmd_CCID("AT+GETICCID\r",1,3);
                }
                Modem_PrtfTo(MODPRTF_CCID);
            }
            break;   
        case 2://��ѯ����ע�����
            {
                Modem_PrtfTo(MODPRTF_CREG);
                Modem_Send_cmd_Search("AT+CREG?\r",3,10);
            }
            break;
        case 3://��ѯ��������
            {
                Modem_Send_cmd_Search("AT+CSQ\r",5,15);                               
            }
            break;
        case 4://///////////////////////////////////////////////////////////////////�����ڴ˽���
            {
                modem.index++;
                if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    Modem_State_Switch(staok);
                }
            }
            break;
        case 5://��ѯGPRS����ע�����
            {
                Modem_PrtfTo(MODPRTF_CGREG);
                Modem_Send_cmd_Search("AT+CGREG?\r",3,10);
            }
            break;
        case 6://��ѯ���總��ע�����
            {
                Modem_Send_cmd_Base("AT+CGATT?\r","OK\r",3,10);
            }
            break;           
        case 7://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                Modem_State_Switch(staerr);
            }
            break; 
    }
}
/*
  * @brief  �������绷��.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_NetCfg (MODEM_STA staok,MODEM_STA staerr)
{       
    u8 *pCmd =NULL;
    u8 Tab_t[50];
    
    static u8 index_back;
    
    switch (modem.index)
    {
        case 0://��ȡ����
            {
                memset(modem.user,0,sizeof(modem.user));
                memset(modem.pass,0,sizeof(modem.pass));

                if(Modem_Type == MODEM_TYPE_Q)
                {              
                    strcpy((char*)modem.apn,"cmnet");//G��Ĭ�Ϲ���
                }
                else if(Modem_Type == MODEM_TYPE_ZTE)
                {
                    strcpy((char*)modem.user,"card");//Ĭ��ֵ
                    strcpy((char*)modem.pass,"card");
                }
                Modem_Port_ReadUserPass();//��ȡ�˺�����
                Modem_Port_ReadApn();     //��ȡapn
                Modem_Port_ReadSleep();   //��ȡ����ֵ 
                modem.index++;
            }
            break;
        case 1://����APN
            {
                Modem_Send_cmd_Apn(1,3);
            }
            break;
        case 2://����tcpnet����
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+QIFGCNT=0\r";//���ó���
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    Modem_PrtfTo(MODPRTF_CPPP);
                    pCmd = "AT+ZPPPSTATUS\r";//��ѯPPP״̬
                } 
                Modem_Send_cmd_Base(pCmd,"OK\r",1,3);
            }
            break;
        case 3://��������
            {   
                modem.index++;
                if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    modem.index--;
                    sprintf((char*)Tab_t,"AT+CTA=%d\r",modem.sleep);
                    Modem_Send_cmd_Base(Tab_t,"OK\r",1,3);
                }
            }
            break;             
        case 4://���ö�����
            {               
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    Modem_Send_cmd_Base("AT+QIMUX=1\r","OK\r",1,3);
                }
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    Modem_State_Switch(staok);  
                } 
            }
            break;
        ////////////////////////////////////////////////////////////��Զר������
        case 5://���÷�͸��ģʽ
            {
                Modem_Send_cmd_Base("AT+QIMODE=0\r","OK\r",1,3);  
            }
            break;
        case 6://�ȹرճ�����ȷ����ʼ����ǰ״̬����ȷ��
            {
                Modem_Send_cmd_Base("AT+QIDEACT\r","DEACT OK\r",3,5);
            }
            break;
        case 7://AT+QISEND������,���ͳɹ�ʱ����ʾ�D>�����بDSEND OK
            {
                //Modem_Send_cmd_Base("AT+QIPROMPT=0\r","OK\r",3,5);   dxl,2015.9.2
                modem.index++;
            }
            break;
        case 8://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                modem.index = index_back+1;//Ϊ�˱���汾����,����������ǿ��תΪ�ɹ�
            }
            break; 
    } 
    index_back = modem.index;
}

/*
  * @brief  ���ö��Ŵ洢��.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Sms (MODEM_STA staok,MODEM_STA staerr)
{       
    u8 *pCmd;
    
    static u8 index_back;
    
    switch (modem.index)
    {
        case 0://��ѯ�������ĺ���
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    Modem_Send_cmd_Search("AT+CSCA?\r",10,15);//Ϊ�˼��ݸ���SIM��������ô��ʱ��
                }                 
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    modem.index++;//����
                }
                Modem_PrtfTo(MODPRTF_CSCA);
            }
            break;     
        case 1://���ö��Ŵ洢��  
            {              
                if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+CPMS=\"MT\",\"MO\"\r";
                } 
                else
                {
                    pCmd = "AT+CPMS=\"SM\",\"SM\",\"SM\"\r";
                }
                Modem_PrtfTo(MODPRTF_CPMS);
                Modem_Send_cmd_Base(pCmd,"OK\r",10,15);//Ϊ�˼��ݸ���SIM��������ô��ʱ��
            }
            break;  
        case 2://���ö�����ʾ�� 
            {              
                if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT\r";
                } 
                else
                {
                    pCmd = "AT+CNMI=2,1\r";
                }
                Modem_PrtfTo(MODPRTF_CNMI);
                Modem_Send_cmd_Base(pCmd,"OK\r",10,15);//Ϊ�˼��ݸ���SIM��������ô��ʱ��
            }
            break;            
        case 3://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                modem.index = index_back+1;//Ϊ�˶����SIM��,���Ŵ���ǿ��תΪ�ɹ�
            }
            break; 
    }
    index_back = modem.index;
}
/*
  * @brief  ��������.
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Dial (MODEM_STA staok,MODEM_STA staerr)
{       
    u8  *pCmd =NULL,*pRec =NULL;
    u8  outtime =0;
    
    switch (modem.index)
    {
        case 0://PPP����
            {
                Modem_PrtfTo(MODPRTF_DPPP);
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+QIREGAPP\r";
                    pRec = "OK\r";
                }                  
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+ZPPPOPEN\r";
                    pRec = "+ZPPPSTATUS: OPENED";
                }
                Modem_Send_cmd_Base(pCmd,pRec,30,5);
            }
            break;
        case 1://�����
            {
                modem.index++;
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {    
                    modem.index--;
                    Modem_Send_cmd_Base("AT+QIACT\r","OK\r",8,5);
                }
            }
            break;
        case 2://��ѯ��վ�����IP
            {
                Modem_PrtfTo(MODPRTF_GIP);
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    pCmd = "AT+QILOCIP\r";
                    outtime = 3;                    
                }               
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    pCmd = "AT+ZIPGETIP\r";
                    outtime = 3;
                } 
                Modem_Send_cmd_IpLoc(pCmd,outtime,3);
            }
            break;
        case 3://������ȷ����л���staok״̬
            {
                Modem_State_Switch(staok);
                Modem_PrtfTo(MODPRTF_ONLN);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                Modem_State_Switch(staerr);
            }
            break;             
    }  
}

/*
  * @brief  ��װ���͵Ķ�������
  * @param  *pNum :ָ���ֻ�����,���ΪNULL��ʹ�ý��պ���
  * @param  *pData :ָ�������������
  * @param  len:�������ݵĳ���,һ��ֻ�ܷ���(MODEM_SMS_SENDMAX)���ֽڣ��ݲ�֧�ֶ��Ų��
  * @retval ���ش�����
  */
u8  Modem_State_Sms_Dt(MODSMS_TYPE Type,u8 *pNum, u8 *pData, u8 len)
{
    u8  *pSendNum=modsms.num;
    
    len = strlen((char*)pData);//����α0

    if(pNum != NULL)
    {
        strcpy((char*)modsms.num,(char*)pNum);
    } 
    else
    {
        if(strlen((char*)modsms.num) == 0)
        {
            return 0;//�Ƿ�����
        }
    }
    #ifdef MODEM_DEBUG_H
    if(ModDbInfType == MODDB_BASE)
    {
        MODDB_PRTFBAS("modem-> SMS=%s, Type=%d, Snd_Len=%d\r\n",modsms.num,Type,len);
        MODDB_PRTFBAS("%s",pData);
        MODDB_PRTFBAS("\r\n");
    }
    #endif 

    if(len > 140)
    {
        return 0;
    }
    memset(modsms.dbuf,0,MODEM_SMS_SENDMAX);
    ////////////////////////////////////////////////////////////////////���봦��
    if(Type == SMS_PDU)
    { 
        memset((u8*)&modsmspdu,0,sizeof(SM_PARAM));
        strcpy((char*)modsmspdu.SCA,(char*)modem.csca); //�������ĺ���  
        strcpy((char*)modsmspdu.TPA,(char*)pSendNum);   //���ŷ��ͺ���  

        memcpy(modsmspdu.TP_UD,pData,len);  
        modsmspdu.udLen = len;

        modsms.dlen   = ModLib_PduEncode(&modsmspdu,modsms.dbuf);
        modsms.pdulen = modsmspdu.PDU_LN;
    }
    else if(Type == SMS_TXT)
    {
        memcpy(modsms.dbuf,pData,len);
        modsms.dlen = len;           
    }
    else
    {
        return 0;//�Ƿ���������
    }
    ////////////////////////////////////////////////////////////////////���ݴ���
    modsms.send = 1;
    modsms.typs = Type;//���Ͷ���ģʽ��������
    return (__TRUE);

}
/*
  * @brief  ����ҵ�񣬽���
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Sms_Rec (MODON_STA staok,MODON_STA staerr)
{       
    u8 *pCmd;
    u8 Tab_t[30];

    switch (modem.index)
    {
        ////////////////////////////////////////////////////////////////////////
        case 0://���ö��Ÿ�ʽ
            {
                Modem_Send_cmd_Sms(0,3,2);//���ø�ʽ,��ʱ3��,����2��
            }
            break;
        case 1://�����ַ���
            {
                Modem_Send_cmd_Sms(1,3,2);//�����ַ���,��ʱ3��,����2��
            }
            break;  
        case 2://��ѯ����
            {
                Modem_Send_cmd_Sms(3,3,2);//��ѯ����,��ʱ3��,����2��
            }
            break;  
        case 3://��ȡ��Ϣ
            {
                strcpy((char*)Tab_t,"AT+CMGR=1\r");
                if(Modem_Type == MODEM_TYPE_ZTE)//����
                {
                    strcpy((char*)Tab_t,"AT+CMGR=0\r");    
                }                
                pCmd = Tab_t; 
                Modem_Send_cmd(MODWT_SMS,pCmd,NULL,2,1);
            }
            break; 
        case 4://ɾ����Ϣ
            {
                Modem_Send_cmd_Sms(4,3,2);//ɾ������,��ʱ3��,����2��
            }
            break; 
        case 5://������ȷ����л���staok״̬
            {
                modsms.busy = 0;
                modsms.rec  = 0;
                Modem_State_SwitchOn(staok);  
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                modsms.busy = 0;
                modsms.rec  = 0;
                Modem_State_SwitchOn(staok);  
            }
            break; 
    }

}

/*
  * @brief  ����ҵ��,����
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Sms_Send (MODON_STA staok,MODON_STA staerr)
{       
    switch (modem.index)
    {
        ////////////////////////////////////////////////////////////////////////
        case 0://���ö��Ÿ�ʽ
            {
                Modem_Send_cmd_Sms(0,3,2);//���ø�ʽ,��ʱ3��,����2��
            }
            break;
        case 1://�����ַ���
            {
                Modem_Send_cmd_Sms(1,3,2);//�����ַ���,��ʱ3��,����2��
            }
            break;            
        case 2://������Ϣ
            {
                Modem_Send_cmd_Sms(2,0,0);//������Ϣ,��������
            }
            break;
        case 3://50ms�ȴ�">"��
            {
                modem.index++;
            }
            break;             
        case 4://���;�������
            {
                modsms.dbuf[modsms.dlen++]=0x1a;
                Modem_Uart_Send(modsms.dbuf,modsms.dlen);
                Modem_Send_cmd(MODWT_SMS,NULL,NULL,10,1);//10s��ʱ�ȴ�ok 
                modem.index++;               
            }
            break;   
        case 5://�ȴ�
            {
                ;//�ط������ṩ��ʱ
            }
            break;            
        case 6://������ȷ����л���staok״̬
            {
                modsms.busy = 0;
                modsms.send = 0;
                Modem_State_SwitchOn(staok); 
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem-> SMS Send Ok!!!\r\n\r\n");
                }
                #endif                 
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                modsms.busy = 0;
                modsms.send = 0;
                Modem_State_SwitchOn(staerr);
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem-> SMS Send Err!!!\r\n\r\n");
                }
                #endif                    
            }
            break;
    }

}

/*
  * @brief  ��ѯָ��soc����״̬
  * @param  Soc��
  * @retval ��������״̬,���߷����棬���򷵻ؼ�
  */
BOOL  Modem_State_SocSta (u8 soc) 
{  
    if((soc < MODEM_SOC_NUM)&&(Modem_IpSock_STA[soc] == MODSOC_ONLN))
    {
        return (__TRUE);
    }
    return (__FALSE);   
}
/*
  * @brief  ��ѯmodem����״̬��CSQ�����ʧ��������modem
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_CSQ (MODON_STA state) 
{
    switch (modem.index)
    {
        case 0://��ѯ����
            {
                Modem_Send_cmd_Search("AT+CSQ\r",1,1); 
            }
            break;
        case 1://���Ͳ�ѯ����
            {
                Modem_Send_cmd_IpSta(1,3);
            }
            break;
        case 2://���SIM��
            {
                Modem_Send_cmd_Search("AT+CPIN?\r",3,5);
            }
            break;
        case 3:
            {
                Modem_State_SwitchOn(state);
            }
            break;
        default://ָ��ش���
            {
                Modem_State_Switch(MODEM_ERROR);//�����쳣����
            }
            break;
    }
}

/*
  * @brief  ��������Ϊ��������
  * @param  dnsis : 0ΪIP���ӣ���0Ϊ��������
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_DNSIP (MODON_STA state)
{       
    u8 *pCmd;
    switch (modem.index)
    {
        case 0://����������������
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {                                      
                    if(modem.socdns == 0)
                    {
                        pCmd = "AT+QIDNSIP=0\r";//ip����
                    }
                    else
                    {
                        pCmd = "AT+QIDNSIP=1\r";//��������                    
                    }
                    Modem_Send_cmd_Base(pCmd,"OK\r",3,3); 
                }
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    modem.index++;//����
                } 
            }
            break;
        case 1://������ȷ����л���staok״̬
            {
                Modem_State_SwitchOn(state);
            }
            break;
        default://���ʱ���л���staerr״̬
            {
                Modem_State_SwitchOn(state);
            }
            break; 
    }
}

/*
  * @brief  ��һ������.
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Ip_Open (MODON_STA state ,u8 SocNum)
{

    switch (modem.index)     
    {
        case 0://��ѯ����״̬
            {      
                Modem_Send_cmd_IpSta(1,3);
            }
            break;  
        case 1://�ж��Ƿ���Ҫ�ر�����
            {      
                if(Modem_IpSock_STA[SocNum] == MODSOC_IDLE)
                {
                    modem.index = 3;//ֱ�Ӵ�
                }
                else
                {
                    modem.index = 2;//�ȹرպ��
                }
            }
            break;
        case 2://�ر�ָ������
            {  
                Modem_Send_cmd_IpClose(SocNum,3,3);
            }
            break;
        case 3://��ָ������
            {      
                Modem_Send_cmd_IpOpen(SocNum,3);
                Modem_PrtfTo(MODPRTF_SOCWT);
            }
            break;
        case 4://�ɹ�
            {  
                modInf.Num_SocLn[SocNum]++;//���Ӵ�������
                Modem_IpSock_STA[SocNum] = MODSOC_WAIT;//�ȴ�������Ӧ��
                Modem_State_SwitchOn(state);
            }
            break;
        default://ʧ��
            {
                modInf.Num_SocLn[SocNum]++;//���Ӵ�������
                Modem_IpSock_STA[SocNum] = MODSOC_IDLE;//תΪ����
                Modem_State_SwitchOn(state);
            }
            break;
    }   
}

/*
  * @brief  �ر�ָ��Soc����.
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @param  index:Soc��
  * @retval None
  */
void  Modem_State_Ip_Close (MODON_STA state ,u8 SocNum) 
{
    static u8 closenum;
    
    switch (modem.index)
    {

        case 0://��ѯ����״̬
            {      
                Modem_Send_cmd_IpSta(1,3);
            }
            break;
            
        case 1://ǿ�ƴ�0��ʼ�ر�ȫ��
            {      
                closenum = SocNum;
                
                if(closenum >= MODEM_SOC_NUM)
                {
                    closenum = 0;
                }
                modem.index++;
            }
            break;
        case 2://�ж��Ƿ���Ҫ�ر�����
            {      
                if(Modem_IpSock_STA[closenum] == MODSOC_IDLE)
                {
                    modem.index += 2;//ֱ������ �����ظ��ر�
                }
                else
                {
                    modem.index++;//ִ�йرն���
                }
            }
            break;
        case 3:
            {    
                Modem_Send_cmd_IpClose(closenum,3,3);
            }
            break;
        
        case 4:
            {
                Modem_IpSock_STA[closenum] = MODSOC_IDLE;//��sock�ɹ�����

                if(SocNum >= MODEM_SOC_NUM)
                {
                    if((++closenum) < MODEM_SOC_NUM)
                    {
                        modem.index = 2;
                        break;
                    }
                }
                Modem_State_SwitchOn(state);
            }
            break;
            
        default://ָ��ش���
            {
                Modem_State_SwitchOn(state);
            }
            break;
    }
}

/*
  * @brief  �������ݵ�ָ��Soc����.
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @param  Outtime: �ȴ�SEND OK ��ʱʱ��,��λs.
  * @param  Try: ���Դ���
  * @retval None
  */
void  Modem_State_Ip_Send (MODON_STA state ,u8 Outtime,u8 Try) 
{
    u8 SocNum = soc_atbuf.soc;
    u8 *pSend = soc_atbuf.buf;
    u16 len   = soc_atbuf.len;

    static u16 timecnt;
    static u8  trycnt;
	  static u8  count = 0;
	
    switch(modem.index)
    {
        case 0://��������
            {
                modtts.	wait = 1;// lock
                Modem_Send_cmd_IpData(SocNum,len,3);
                trycnt++;
							  count = 0;
            }
            break;
        case 1://���;�������
            { u8  buf[610];
								
							  count++;
							  if(count >= 1)  
								{
								count = 0;
                Modem_Uart_Send(pSend,len);
                Modem_Send_cmd(MODWT_SEDATA,NULL,NULL,Outtime,1);//5s��ʱ�ȴ�send ok 
                modem.index++;
                timecnt = 0;

                #ifdef MODEM_DEBUG_H
               
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem-> Soc = %d   Snd_Len = %04d\r\n",SocNum,len);
                    if(len > 200)
                    {
                        len = 200;
                    }
                    memset(buf,0,600);
                    ModLib_ByteStrEx(buf,pSend,len);
                    MODDB_PRTFBAS("%s",buf);
                    MODDB_PRTFBAS("\r\n");
                }
                #endif
							  }
							  
            }
            break;
        case 2://�ȴ���� 
            {
                if((timecnt++) > Outtime);
                {
                    timecnt = 0;
                    modem.index++;//�ط����Ƴ�ʱʧ�ܺ�ʹ�ø�ֵ
                }
            }
            break;
        case 3://����Ƿ������ݷ���
            {              
                trycnt      = 0;
                modtts.wait = 0;
                //if(Modem_State_CheckSoc()==0) dxl,2017.4.12,����
                //{
                    modem.index++;
                //}
            } 
            break;
        case 4://�������ݷ��ͳɹ�
            {              
                Modem_State_SwitchOn(state);
            } 
            break;            
        default://��������ʧ��
            {
                modtts.wait = 0;// unlock
                if(trycnt < Try)
                {
                    modem.index = 0;//�ط�
                    break;
                }
                trycnt = 0;
                Modem_State_SwitchOn(MODON_IPCLOSE);//��·�쳣 �رո���·
                modem.socclose = SocNum;
            }
            break;
    }
}

/*
  * @brief  Ftpҵ��
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Ftp (MODON_STA state)
{
    switch(modem.index)
    {
        ////////////////////////////////////////////////////////////////////////��¼
        case 0://�����˺� (��������IP���˿ڡ��˺š�����)
            {
                Modem_Send_cmd_FtpLogin(0,30,3);
            }
            break;
        case 1://�������⴦��
            {
                modem.index++;
                if(Modem_Type == MODEM_TYPE_ZTE)//����
                {
                    modem.index = 4;//�ɹ���¼��������״̬
                    modem.delay = MODEM_TSEC*3;//����ʱ3�����Ῠ��
                }
            }
            break;
        case 2://��������
            {
                Modem_Send_cmd_FtpLogin(1,3,3);
            }
            break;
        case 3://����IP���˿�
            {
                Modem_Send_cmd_FtpLogin(2,30,3);
            }
            break;
        ////////////////////////////////////////////////////////////////////////���ļ�
        case 4://����·��
            {
                Modem_Send_cmd_FtpDown(0,30,3);
            }
            break;
        case 5://�������⴦��
            {
                modem.index++;
                if(Modem_Type == MODEM_TYPE_ZTE)//����
                {
                    modem.index  = 7;//�ɹ���¼��������״̬
                    modftp.busy  = 1;
                    modftp.fend  = 0;
                    modftp.fres  = 0;
                    modftp.fsize = 0;                    
                }
            }
            break;
        case 6://�����ļ���
            {
                Modem_Send_cmd_FtpDown(1,30,3);
            }
            break;
        ////////////////////////////////////////////////////////////////////////������
        case 7://��־λ����
            {
                modem.wait = MODWT_FTP;//��ʱ����
                modem.delay = MODEM_TSEC*60;
                modat.trynum = 0;
                modem.index++;
            }
            break;         
        case 8://
            {
                if(modftp.fres)//�ȴ��������
                {
                    modem.index++;
                }
            }
            break; 
        case 9://�������
            {
                modftp.state = 0;//����   
                Modem_State_SwitchOn(state);
            }
            break;                         
        default://��������ʧ��
            {
                modftp.state = 0;//����   
                Modem_State_SwitchOn(state);
            }
            break;
    }
}
/*
  * @brief  �绰ҵ��->����绰״̬,��Ҫ��������
  * @param  None
  * @retval None
  */
void  Modem_State_Phone_Enter(void)
{
    
    if(modem.state != MODEM_ON)
    {
        return ;//����״̬
    }

    if(modem.onsta == MODON_IPOPEN)
    {
        Modem_State_Push();
        modphone.otherflg = 1;
    }
    else if(modem.onsta != MODON_IDLE)
    {
        return ;//���ڴ�����������
    }
    modphone.busy = 1;
    modphone.state= MODPH_RING;
    modapi.wait = MODON_PHONE;     
    Modem_State_SwitchOn(MODON_PHONE);

}
/*
  * @brief  �绰ҵ��->�˳��绰״̬,��Ҫ���ڹҶϵ绰
  * @param  None
  * @retval None
  */
void  Modem_State_Phone_Break(void)
{
	  u8 cmptab[20];
	
    Modem_Bsp_SpkOff(BSP_SPK_TALK);
	  strcpy((char *)cmptab,"AT+QAUDCH=0\r");//dxl,2016.3.30������������л���Ƶͨ�������GC65��bug
		Modem_Uart_Send(cmptab,strlen((char*)cmptab));
    Modem_Bsp_MicOff();
    modphone.busy = 0;
    modphone.state= MODPH_IDLE;
    Modem_Api_CallPhoneStatus(modphone.num,0);//֪ͨ�ⲿ

    if(modphone.otherflg)
    {
        modphone.otherflg = 0;
        Modem_State_Pop();
    }
    else
    {
        if(ModemPhoneTm > PHONE_TIME_CLOSE)//����ͨ����ر�������·
        {
            modem.socclose = MODEM_SOC_NUM;//�ر�������·
            Modem_State_SwitchOn(MODON_IPCLOSE);
        }
        else
        {
            Modem_State_SwitchOn(MODON_IDLE);//�˳��绰ģʽ  
        }
        ModemPhoneTm = 0;
    }
}
/*
  * @brief  �绰ҵ��->����绰
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
/*dxl,2016.3.31,����绰ǰ�ȹرվ�����GC65ģ����bug��TTS������Ὺ��������
void  Modem_State_Phone_Atd(MODPH_STA stat)
{
    u8 tab[30];

    switch(modem.index)
    {      
        case 0://����绰
            {          
                sprintf((char*)tab,"ATD%s\r",modphone.num);//�绰����
                Modem_Uart_Send(tab,strlen((char*)tab));
                Modem_Bsp_MicOff();
                modem.index++;
                ModemAtdOutTime = 0;
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->%s\r\n",tab);
                }
                #endif                  
            }
            break; 
        case 1://�ȴ��Է�����
            {
                if((ModemAtdOutTime++)>(90*MODEM_TSEC))
                {
                    modem.index = 0xaa;
                    ModemPhoneTm = 0;//�Է���Ӧ����������
                }
            }
            break;  
        case 2://���MC8332ͨ����Х������ȡ����������
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    modem.index++;
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    Modem_Send_cmd_Base("AT+SIDET=0,1\r","OK\r",3,3);
                    //modem.index++; 
                }                 
            }
            break;            
        case 3://����绰�ɹ�
            {
                ModemPhoneTm = 0;//�Է���������������
                Modem_Bsp_MicOn();
                Modem_State_SwitchPhone(stat);
                Modem_Api_CallPhoneStatus(modphone.num,1);
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->ATD OK!!!\r\n\r\n");
                }
                #endif                
            }
            break;            
        default://����绰ʧ��
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }
    
}
*/

void  Modem_State_Phone_Atd(MODPH_STA stat)
{
    u8 tab[30];

    switch(modem.index)
    {     
        case 0://���л�ͨ�������GC65ģ��bug��
            {          
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {       
										Modem_Send_cmd_Base("AT+QAUDCH=0\r","OK\r",10,3);											
                    //modem.index++;
									  ModemAtdOutTime = 0;
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                  
                    modem.index++; 
									  ModemAtdOutTime = 1*MODEM_TSEC;
                }                
            }
            break;							
        case 1://����绰
            {    
                if((ModemAtdOutTime++)>=(1*MODEM_TSEC))
                {		
                ModemAtdOutTime = 0;									
                sprintf((char*)tab,"ATD%s\r",modphone.num);//�绰����
                Modem_Uart_Send(tab,strlen((char*)tab));
                Modem_Bsp_MicOff();
                modem.index++;
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->%s\r\n",tab);
                }
                #endif   
							  }								
            }
            break; 
        case 2://�ȴ��Է�����
            {
                if((ModemAtdOutTime++)>(90*MODEM_TSEC))
                {
                    modem.index = 0xaa;
                    ModemPhoneTm = 0;//�Է���Ӧ����������
                }
            }
            break;  
        case 3://���MC8332ͨ����Х������ȡ����������
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    modem.index++;
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    Modem_Send_cmd_Base("AT+SIDET=0,1\r","OK\r",3,3);
                    //modem.index++; 
                }                 
            }
            break;            
        case 4://����绰�ɹ�
            {
                ModemPhoneTm = 0;//�Է���������������
                Modem_Bsp_MicOn();
                Modem_State_SwitchPhone(stat);
                Modem_Api_CallPhoneStatus(modphone.num,1);
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->ATD OK!!!\r\n\r\n");
                }
                #endif                
            }
            break;            
        default://����绰ʧ��
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }
    
}

/*
  * @brief  �绰ҵ��->�����绰
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Phone_Ata(MODPH_STA state)
{
    switch(modem.index)
    {    
        case 0://�����绰
            {
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->����...\r\n\r\n");
                }
                #endif             
                Modem_Send_cmd_Base("ATA\r","OK\r",3,3);
            }
            break;       
        case 1://�����绰�ɹ�
            {
                Modem_Bsp_MicOn();
                modphone.state= state; 
                ModemPhoneTm = 0;//ͨ��ǰ���������
                Modem_Api_CallPhoneStatus(modphone.num,1);//֪ͨ�ⲿ�ѽ�ͨ
            }
            break;            
        default://�����绰ʧ��
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }
}
/*
  * @brief  �绰ҵ��->�Ҷϵ绰
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @param  onstate:�˳��绰ҵ����л�״̬
  * @retval None
  */
void  Modem_State_Phone_Ath(void)
{
    static u8 atherrcnt;
    
    switch(modem.index)
    {
        case 0://����绰
            {
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->�Ҷ�...\r\n\r\n");
                }
                #endif              
                atherrcnt++;
                Modem_Send_cmd_Base("ATH\r","OK\r",3,3);
            }
            break;       
        case 1://�Ҷϵ绰�ɹ�
            {
                atherrcnt = 0;
                Modem_State_Phone_Break();
            }
            break;            
        default://�Ҷϵ绰ʧ��
            {
                if(atherrcnt < 3)
                {
                    Modem_State_SwitchPhone(MODPH_ERR);
                }
                else
                {
                    atherrcnt = 0;
                    modem.index = 1;//ǿ�ƹҶ�
                }
            }
            break;
    }
}
/*
  * @brief  �绰ҵ��->�����л�
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Phone_Audch(MODPH_STA state)
{
    u8 tab[30]={0};

    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {              
        memcpy(tab,"AT+QAUDCH=",10);
        sprintf((char*)tab+10,"%d",modphone.audch);
        strcat((char*)tab,"\r");
    }      
    else if(Modem_Type == MODEM_TYPE_ZTE)/////����
    {
        memcpy(tab,"AT+SPEAKER=",11);
        sprintf((char*)tab+11,"%d",modphone.audch);
        strcat((char*)tab,"\r");
    } 
    switch(modem.index)
    {
        case 0://����绰
            {
                Modem_Send_cmd_Base(tab,"OK\r",3,3);
            }
            break;             
        case 1://����绰�ɹ�
            {
                Modem_State_SwitchPhone(state);//    
            }
            break;            
        default://����绰ʧ��
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }
}
/*
  * @brief  �绰ҵ��->��������
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Phone_Clvl(MODPH_STA state)
{
    u8 tab[30]={0};
    u8 value = modphone.clvlvalue[modphone.clvl];

    switch(modem.index)
    {
        case 0://��������
            {
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->Phone Clvl=%02d\r\n",value);
                }
                #endif            
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    sprintf((char*)tab,"AT+CLVL=%d\r",value);
                }     
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    sprintf((char*)tab,"AT+VGR=%d\r",value);        
                } 
                Modem_Send_cmd_Base(tab,"OK\r",3,3);
            }
            break;             
        case 1://�ɹ�
            {
                Modem_State_SwitchPhone(state);//    
            }
            break;            
        default://ʧ��
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }
}
/*
  * @brief  �绰ҵ��->����������ʾ�����ֵ����
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Phone_Vts(MODPH_STA state)
{
    char str_t[30];

    switch(modem.index)
    {
        case 0://���ð���
            {
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->Phone Vts=%s\r\n",modphone.vts);
                }
                #endif                   
                sprintf(str_t,"AT+VTS=%s\r\n",modphone.vts);
                Modem_Send_cmd_Base((u8*)str_t,"OK\r",3,3);
            }
            break;             
        case 1://���ð����ɹ�
            {
                Modem_State_SwitchPhone(state);//    
            }
            break;            
        default://���ð���ʧ��
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }
}
/*
  * @brief  �绰ҵ��->���紦����Ҫ��������봫�ݵ��ⲿ,�ȴ��ⲿ����
  * @param  None
  * @retval None
  */
void  Modem_State_Phone_Ring(void)
{
    switch(modem.index)
    {
        case 0://���MC8332ͨ����Х������ȡ����������
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    modem.index++;
                }                  
                else if(Modem_Type == MODEM_TYPE_ZTE)/////����
                {
                    //Modem_Send_cmd_Base("AT+VGT=0\r","OK\r",3,3);
                    modem.index++;  
                }  
            }    
        case 1://�ж��������
            {
                if(Modem_Api_CallPhoneStatus(modphone.num,2))
                {
                    modem.index++;
                    Modem_Bsp_SpkOn(BSP_SPK_TALK);
                }
            }
            break;                  
        case 2://�����κεȴ�Ӧ�ò㴦��
            {
                 ;
            }
            break; 
        default://
            {
                Modem_State_SwitchPhone(MODPH_ERR);
            }
            break;
    }  

}
/*
  * @brief  �绰ҵ��->����ģʽ��������ͨ����
  * @param  None
  * @retval None
  */
void  Modem_State_Phone_Idle(void)
{
    ModemPhoneTm++;//ͨ����

    if(ModemPhoneTm > PHONE_TIME_CLOSE)//����ͨ����ر�������·��־
    {
        if(Modem_IpSock_STA[0] == MODSOC_ONLN)//�ж��������Ƿ�����
        {
            //�ݴ�Modem_IpSock_STA������־�����ڹر�����ʱʹ��
            memcpy(Modem_Phone_SocSta,Modem_IpSock_STA,sizeof(Modem_IpSock_STA));

            //����������߱�־
            memset(Modem_IpSock_STA,MODSOC_IDLE,sizeof(Modem_IpSock_STA));
        }
    }   
}
/*
  * @brief  �绰ҵ����
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Phone_Pro(MODON_STA state)
{
    switch(modphone.state)
    {
        case MODPH_IDLE:
            {
                Modem_State_Phone_Idle();
            }
            break;
        case MODPH_ATD:
            {
                Modem_State_Phone_Atd(MODPH_IDLE);
            }
            break;
        case MODPH_ATA:
            {
                Modem_State_Phone_Ata(MODPH_IDLE);
            }
            break;
        case MODPH_RING:
            {
                Modem_State_Phone_Ring();
            }
            break; 
        case MODPH_ATH:
            {
                Modem_State_Phone_Ath();
            }
            break;            
        case MODPH_AUDCH:
            {
                Modem_State_Phone_Audch(MODPH_IDLE);
            }
            break;
        case MODPH_CLVL:
            {
                Modem_State_Phone_Clvl(MODPH_IDLE);
            }
            break;       
        case MODPH_VTS:
            {
                Modem_State_Phone_Vts(MODPH_IDLE);
            }
            break;  
        case MODPH_ERR:
            {
                Modem_Bsp_MicOff();
                Modem_State_SwitchPhone(MODPH_ATH);
            }
            break;            
            
    }

}
/*
  * @brief  modem״̬���е�TTS,��װSIM��������½����״̬
  * @param  staok : ������ִ�гɹ����л���״̬��
  * @param  staerr : .........ʧ�ܺ�..........��
  * @retval None
  */
void  Modem_State_Tts (MODEM_STA staok,MODEM_STA staerr)
{
    ModemSimNoTime++;
    if(ModemSimNoTime > MODEM_TSEC*600)//10���Ӽ��һ��,dxl,2015.3.27Ϊ��ʻ��¼�Ǽ����޸�,ԭ��ʱ���С
    {
        ModemSimNoTime = 0;
        Modem_State_Switch(staerr);
    }
}

/*
  * @brief  ��װ�����ı�����
  * @param  type : 0Ϊ��������,��������ڲ�����������ᱻ��ϣ�
  *                1Ϊ��������,��������ڲ�����������ȴ�������ٲ����µ�;
  * @param  *pSrc  :����������
  * @param  SrcLen :�������ݵĳ���,�������Ϊ0���ַ���\0Ϊ����
  * @retval �Ϸ�����1, �Ƿ�����0
  */
u8  Modem_State_Tts_Dt(u8 type, const u8 *pSrc, u16 SrcLen)
{
    u16 CmpSrcLen;
    
    CmpSrcLen = ModLib_AtLen((u8*)pSrc);//����α0����\r\n

    if(CmpSrcLen < SrcLen)
    {
        SrcLen = CmpSrcLen;
    }
    
    if(SrcLen == 0)
    {
        return 0;//������
    }
    #ifdef MODEM_DEBUG_H
    if(ModDbInfType == MODDB_BASE)
    {
        MODDB_PRTFBAS("modem->Tts Send Len=%03d\r\n",SrcLen);
        MODDB_PRTFBAS("%s\r\n",(char*)pSrc);
        MODDB_PRTFBAS("modem->Tts Send End\r\n\r\n");
    }
    #endif 
    if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
    {      
        if(QuectelAtTtsFlag == 0)
        {
            return 0;//��Զģ����Щ��֧��TTS
        }
        if((SrcLen+15) >= MODTTS_SIZE)
        {
            return 0;//���ݳ���
        } 
        memset(modtts.buf,0,MODTTS_SIZE);//�������
        modtts.len = ModLib_AtCmy(modtts.buf,"AT+QTTS=2,\"");
        memcpy(modtts.buf+modtts.len,pSrc,SrcLen);
        strcat((char*)modtts.buf,"\"\r\n");
    }               
    else if(Modem_Type == MODEM_TYPE_ZTE)/////���� 
    {
        if(((SrcLen+15)*2) >= MODTTS_SIZE)//ByteToAsc
        {
            return 0;//���ݳ���
        }
        memset(modtts.buf,0,MODTTS_SIZE);//�������
        modtts.len = ModLib_AtCmy(modtts.buf,"AT+ZTTS=2,\"");
        ModLib_ByteStr(modtts.buf+modtts.len,pSrc,SrcLen);
        strcat((char*)modtts.buf,"\"\r\n");
    } 
    else
    {
        return 0;
    }
    
    modtts.len = strlen((char*)modtts.buf);

		/* dxl,2016.3.24 ����������ʱ����û�����ã����Ǵ������⣬������
    if(type)//��Ҫ��ʱ����
    {
        if(modtts.busy)//��һ����û������
        {
            modtts.type = 1;
        } 
    }
		*/
    return 1;
}
/*
  * @brief  TTS��������״̬
  * @param  type : 0 -> ��ѯ����״̬
  *                1 -> ��ѯ�Ƿ�֧��TTS
  * @retval ���ڲ�������1, ������Ϸ���0
  */
u8  Modem_State_Tts_Is(u8 type)
{
    ////////////////////////////////////////////////////////////////////��ѯ״̬
    if(type == 0)
    {
        if(modtts.busy)
        {
            return 1;
        }
    }
    ////////////////////////////////////////////////////////////////////��ѯ֧��   
    else
    {
        if(Modem_Type == MODEM_TYPE_Q)///��Զ
        {    
            if(QuectelAtTtsFlag)
            {
                return 1;
            }
        }                 
        else if(Modem_Type == MODEM_TYPE_ZTE)/////���� 
        {
            return 1;
        }  
    }
    return 0;
}
/*
  * @brief  TTSҵ���� -> ����TTS����
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Tts_Clvl(MODTTS_STA state)
{
    u8 tab[30]={0};  
    //u8 value = modtts.clvlvalue[modtts.clvl]; dxl,2016.8.12
		s16 value = modtts.clvlvalue[modtts.clvl];
    
    switch(modtts.index)
    {      
        case 0://������������
            {
                #ifdef MODEM_DEBUG_H
                if(ModDbInfType == MODDB_BASE)
                {
                    MODDB_PRTFBAS("modem->Tts Clvl=%02d\r\n",value);
                }
                #endif
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {
                    //sprintf((char*)tab,"AT+QMEDVL=%d\r",value); dxl,2016.8.12
									  if(value >= 0)
										{
											  sprintf((char*)tab,"AT+QTTSETUP=1,2,%d\r",value);
									  }
										else
									  {
												value = -value;
												sprintf((char*)tab,"AT+QTTSETUP=1,2,-%d\r",value);
									  }
                }      
                else if(Modem_Type == MODEM_TYPE_ZTE)/////���� AT+ZTTSP=3,1,3
                {
                    sprintf((char*)tab,"AT+ZTTSP=%d,1,3\r",value);        
                }             
                Modem_Tts_Cmd(tab);
                modtts.index++;
            }
            break;            
        default://TTSʧ��
            {
                modtts.state = state;
                modtts.index = 0;
                modtts.send  = 0;//���㷢�����ݱ�־λ
            }
            break;
    }
}
/*
  * @brief  TTSҵ���� -> ��������
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Tts_Play(MODTTS_STA state)
{ 
    static u8 delay;
    static u8 outtime;
    
    switch(modtts.index)
    {      
        case 0://������TTS����ǰ��ֹͣ����
            {
                ////////////////////////////////////////////////////////////////��ͻ����
                if(modtts.wait)
                {
                    if((outtime++) < 5)//��ʱ5����ѯ
                    {
                        return;
                    }
                }
                outtime = 0;

                ////////////////////////////////////////////////////////////////ֹͣ����
                modtts.cmdack = 0;//�����Ҫ�ж�Ӧ��
                modtts.index  = 1;//������ֹͣ����ֱ������                
                if(modtts.busy == 0)//TTS���ڿ���
                {
                    modtts.busy  = 1;
                    break;
                }
                
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    Modem_Tts_Cmd("AT+QTTS=0\r");
                }               
                else if(Modem_Type == MODEM_TYPE_ZTE)/////���� 
                {
                    Modem_Tts_Cmd("AT+ZTTS=0\r");
                }
                modtts.cmdack = 1;//������Ҫ�ж�Ӧ��
                delay = 0;  
            }
            break; 
        case 1://��ʱ���ڵȴ�OK
            {
                if(modtts.cmdack == 0)
                {
                    Modem_Bsp_SpkOn(BSP_SPK_TTS);//��������
                    modtts.index++;
                }
                else
                {
                    if((delay++) > (MODEM_TSEC))
                    {
                        delay = 0;
                        Modem_Bsp_SpkOn(BSP_SPK_TTS);//��������
                        modtts.index++;
                    }
                }
            }
            break;                 
        case 2://����TTS���� ����+����
            {
                if(0 != Modem_Uart_Send(modtts.buf,modtts.len))
								{
                    modtts.index++;
                    modtts.cmdack = 1;//������Ҫ�ж�Ӧ��
                    delay = 0;  
								}									
            }
            break;  
        case 3://����
            {
                if(modtts.cmdack == 0)
                {
                    modtts.index++;
                }
                else
                {
                    if((delay++) > (3*MODEM_TSEC))
                    {
                        delay = 0;
                        modtts.busy = 0;
                        modtts.index++;
                    }
                }
            }
            break;  
        default://���ͽ���
            {
                modtts.state = state;
                modtts.index = 0;
                modtts.send  = 0;//���㷢�����ݱ�־λ    
            }
            break;           
    }
}
/*
  * @brief  TTSҵ���� -> ֹͣ����
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Tts_Stop(MODTTS_STA state)
{
    switch(modtts.index)
    {      
        case 0://����ֹͣ����
            {
                if(Modem_Type == MODEM_TYPE_Q)////////////��Զ
                {              
                    Modem_Tts_Cmd("AT+QTTS=0\r");
                }                
                else if(Modem_Type == MODEM_TYPE_ZTE)/////���� 
                {
                    Modem_Tts_Cmd("AT+ZTTS=0\r");
                }   

                modtts.index++;
            }
            break;            
        default://TTSʧ��
            {
                modtts.state = state;
                modtts.index = 0;
            }
            break;
    }

}
/*
  * @brief  TTSҵ�����Ƿ�����Ҫ�������ı���Ϣ
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Tts_Check(void)
{
	  u8 cmptab[20];
	
    ////////////////////////////////////////////////////////////////////////////����
    if(modtts.type == 0)//��������
    {
        if(modtts.send) 
        {
            modtts.state= MODTTS_PLAY;
        }
    }
    else//��ʱ����
    {
        if(modtts.busy == 0)
        {
            modtts.type = 0;
            modtts.state= MODTTS_PLAY;
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////��ʱ����
    if(modtts.busy == 0)
    {
        TtsOutTimeCount=0;
    }
    else
    {
        TtsOutTimeCount++;
        if(TtsOutTimeCount > MODEM_TSEC*120)//����120sû�в��������ǿ�Ʒ���
        {
            modtts.busy = 0;
            TtsOutTimeCount = 0;
            Modem_Bsp_SpkOff(BSP_SPK_TTS);//�ر������� 
            strcpy((char *)cmptab,"AT+QAUDCH=0\r");//dxl,2016.3.30������������л���Ƶͨ�������GC65��bug dxl,2016.8.27����
						Modem_Uart_Send(cmptab,strlen((char*)cmptab));					
        }
    }
}
/*
  * @brief  TTSҵ����
  * @param  state:��״ִ̬����Ϻ���¸�״̬
  * @retval None
  */
void  Modem_State_Tts_Pro(void)
{
    switch(modtts.state)
    {
        case MODTTS_IDLE:
            {
                Modem_State_Tts_Check();
            }
            break;
        case MODTTS_PLAY:
            {
                Modem_State_Tts_Play(MODTTS_IDLE);
            }
            break;
        case MODTTS_STOP:
            {
                Modem_State_Tts_Stop(MODTTS_IDLE);
            }
            break;  
        case MODTTS_CLVL:
            {
                Modem_State_Tts_Clvl(MODTTS_IDLE);
            }
            break;             
        default:
            {
                modtts.state = MODTTS_IDLE;
                modtts.index = 0;
            }
            break;        
            
    }

}
/*
  * @brief  modem����.
  * @param  None
  * @retval None
  */
void  Modem_State_OnLine (void) 
{           
    switch (modem.onsta)
    {
        ////////////////////////////////////////////////////////////////////����
        case MODON_IDLE:/*---------------------------------------------*/
            {                  //���ڴ��ڸ�״̬
                if(modapi.wait != MODON_IDLE)
                {     
                    modapi.wait = MODON_IDLE;
                }
                if((CsqStatIsCnt++) > CSQSTAT_TIME)
                {
                    CsqStatIsCnt = 0;
                    modapi.wait = MODON_CSQ;
                    modem.onsta = MODON_CSQ;
                }
                Modem_State_Check();
            }
            break;
        ////////////////////////////////////////////////////////////////////��ѯ
        case MODON_CSQ:
            {
                Modem_State_CSQ(MODON_IDLE);
            }
            break;
        ////////////////////////////////////////////////////////////////������            
        case MODON_IPOPEN:
            {
                Modem_State_Ip_Open(MODON_IDLE,modem.socopen);
            }
            break;
        ////////////////////////////////////////////////////////////////�ر�����              
        case MODON_IPCLOSE:
            {
                Modem_State_Ip_Close(MODON_IDLE,modem.socclose);
            }
            break;
        ////////////////////////////////////////////////////////////////��������            
        case  MODON_IPSEND:
            {              
                Modem_State_Ip_Send(MODON_IDLE,10,3);
            }
        break;
        ////////////////////////////////////////////////////////////////��������        
        case  MODON_DNSIP:
            {              
                Modem_State_DNSIP(MODON_IDLE);
            }    
        break;
        /////////////////////////////////////////////////////////////////////FTP          
        case  MODON_FTP:
            {              
                Modem_State_Ftp(MODON_IDLE);
            }    
        break;    
        ////////////////////////////////////////////////////////////////���Ž���         
        case  MODON_SMSREC:
            {              
                Modem_State_Sms_Rec(MODON_IDLE,MODON_IDLE);
            }    
        break;   
        ////////////////////////////////////////////////////////////////���ŷ���        
        case  MODON_SMSSEND:
            {              
                Modem_State_Sms_Send(MODON_IDLE,MODON_IDLE);
            }    
        break;
        ////////////////////////////////////////////////////////////////////�绰       
        case  MODON_PHONE:
            {              
                Modem_State_Phone_Pro(MODON_IDLE);
            }    
        break;  
        ////////////////////////////////////////////////////////////////////�ػ�        
        case  MODON_OFF:
            {              
                Modem_State_Switch(MODEM_OFF);
            }    
        break; 
        ////////////////////////////////////////////////////////////////////��λ
        case  MODON_SYSREST:
            {              
                NVIC_SystemReset();//ϵͳ��λ
            }    
        break;
        
    } 

}



