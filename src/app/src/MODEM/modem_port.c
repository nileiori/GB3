/**
  ******************************************************************************
  * @file    modem_prot.c 
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
*                                       LOCAL GLOBAL VARIABLES   ���ر���
*********************************************************************************************************
*/




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
  * @brief  modem�ӿں�����ʼ��,modem��ʼ��ʱ������
  * @param  None
  * @retval None
  */
void Modem_Port_Init(void)
{
#ifdef EYE_MODEM
    Public_SaveCurSIMICcidNumberToE2Param();
#endif
}

/**
  * @brief  modem��Ϣ�ص�����.
  * @param  *p  :ָ��modem���ص���Ϣ����.
  * @param  len :����modem��Ϣ����.
  * @retval None
  */
void Modem_Port_Call_Ati (u8 *p, u16 len) 
{

}

/**
  * @brief  ��ѯ��վ����ı���IP��ַ�ص�����.
  * @param  *p  :ָ�򱾵�IP��ַ����.
  * @param  len :IP��ַ����.
  * @retval None
  */
void Modem_Port_Call_LocIp (u8 *p, u8 len) 
{
}
/**
  * @brief  ��ѯCCID����ص�����.
  * @param  *p  :ָ���ַ����.
  * @param  len :IP��ַ����.
  * @retval None
  */
void Modem_Port_Call_CCID (u8 *p, u8 len) 
{
#ifdef EYE_MODEM
    Public_SaveCurSIMICcidNumberToE2Param();
#endif
}
/**
  * @brief  ��ѯָ��sock���ӻص�����.
  * @param  index  :�������
  * @param  *pMode :ָ���������ͣ� ��TCP����UDP��
  * @param  *pAddr :Զ�����ӻ����IP ��ַ
  * @param  port   :Զ�����ӻ����˿ں� 
  * @retval None
  */
void Modem_Port_Call_Ip_STAT (u8 *pIndex,u8 *pMode ,u8 *pAddr, u8 *pPort) 
{
#if 0
    printf("num  : %s\r\n",pIndex);
    printf("type : %s\r\n",pMode);
    printf("ip addr :%s\r\n",pAddr);
    printf("port : %s\r\n",pPort);
#endif
}
/**
  * @brief  ��ȡ����ʱ��,��Ҫ����CDMAģ��
  * @param  None.
  * @retval None.
  */
void Modem_Port_ReadSleep (void) 
{
    u8 tab[5];

    modem.sleep = 0;//Ĭ�Ϲر�����
    
#ifdef EYE_MODEM
    
    EepromPram_ReadPram(E2_CDMA_SLEEP_ID, tab);
    if(tab[0] == 1)
    {
        modem.sleep = 8;//8������ʱ��
    }
#endif

}
/**
  * @brief  �Ӵ洢���ж�ȡ��������,��Ҫ�ǳ�ʼ����Ƶ����
  * @param  None.
  * @retval None.
  */
void Modem_Port_ReadAud (void) 
{ 
#ifdef EYE_MODEM
    u8 value=0;
    
    FRAM_BufferRead((u8*)&value,FRAM_TEL_VOLUME__LEN,FRAM_TEL_VOLUME_ADDR);
    if((value > 0)&&(value < 10))
    {
        modphone.clvl = value;
    }
    else
    {
        value = 6;//�����쳣ʹ��Ĭ��6���� 
        modphone.clvl = value;
        FRAM_BufferWrite(FRAM_TEL_VOLUME_ADDR,(u8*)&value,FRAM_TEL_VOLUME__LEN);
    }
    
    FRAM_BufferRead((u8*)&value,FRAM_TTS_VOLUME_LEN,FRAM_TTS_VOLUME_ADDR);
    if((value > 0)&&(value < 10))
    {
        modtts.clvl = value;
    }
    else
    {
        value = 4;//�����쳣ʹ��Ĭ��4����   
        //modphone.clvl = value;  //dxl,2015.5.14  
        modtts.clvl = value;
        FRAM_BufferWrite(FRAM_TTS_VOLUME_ADDR,(u8*)&value,FRAM_TTS_VOLUME_LEN);
    }    
#else
    modphone.clvl = 6;//Ĭ��6����
    modtts.clvl   = 4;
#endif    
}

/**
  * @brief  �Ӵ洢���ж�ȡapn
  * @param  None.
  * @retval None.
  */
void Modem_Port_ReadApn (void) 
{
#ifdef EYE_MODEM 
    u8 tab[30]={0};
    u8 len=0;
 
    memset(modem.apn,0,sizeof(modem.apn));  
    strcpy((char*)modem.apn,"cmnet");//G��Ĭ�Ϲ���

    len = EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, tab);
    len = strlen((char*)tab);
    if ((len >0)&&(len <= 20)) 
    {
        memset(modem.apn,0,sizeof(modem.apn));
        memcpy(modem.apn,tab,len);
    }
#endif
}

/**
  * @brief  �Ӵ洢���ж�ȡ�˺�����
  * @param  None.
  * @retval None.
  */
void Modem_Port_ReadUserPass (void) 
{
#ifdef EYE_MODEM  
    u8 tab[30]={0};
    u8 len=0;
    
    len = EepromPram_ReadPram(E2_MAIN_SERVER_DIAL_NAME_ID,tab);
    len = strlen((char*)tab);
    if ((len >0)&&(len <= 20)) 
    {
        memset(modem.user,0,sizeof(modem.user));
        memcpy(modem.user,tab,len);
    }
  
    len = EepromPram_ReadPram(E2_MAIN_SERVER_DIAL_PASSWORD_ID,tab);
    len = strlen((char*)tab);
    if ((len >0)&&(len <= 20)) 
    {
        memset(modem.pass,0,sizeof(modem.pass));
        memcpy(modem.pass,tab,len);
    }
#endif
}

/**
  * @brief  ����modeme��apn����ӿ�.
  * @param  *p  : ָ��APN����.
  * @param  len : APN�ĳ���.
  * @retval �������ý�����ɹ�����1�����򷵻�0.
  */
u8 Modem_Port_Apn_Set (u8 *p, u8 len) 
{
    if(len < sizeof(modem.apn))
    {
        memcpy(modem.apn,p,len);
        return 1;
    } 
    return 0;
}
/**
  * @brief  ��ѯmodem�ź���������ӿ�.
  * @param  None.
  * @retval ����Modem���ź�����.
  */
u8 Modem_Port_Csq (void) 
{
    return (modem.csq);
}


    

