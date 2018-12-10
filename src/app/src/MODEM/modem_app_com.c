/*
********************************************************************************
*
*                                                              
*
* Filename      : modem_app_com.c
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-10-23  
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
#else
//#include  <app_cfg.h>
#include  <includes.h>
#endif

/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/
#ifdef MODEM_APP_COM_H

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
static u8 communication_tasksta = 1;//ͨѶģ������״̬ 0Ϊ�ر����� 1Ϊ������

/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/
CALL_RECORD     gPhone;             //ͨ�����
SMS_RECORD      gSmsRecord;         //�������
UART2_RECEIVE   uart2_recdat;       //����2����

/******************************************************************************///EGS701
#ifdef   EYE_EGS701
    u8 NetFirstManualCloseFlag = 0;//����1�ֶ�������ͨѶģ������ر����ӣ���������ģ��ر����ӣ��رձ�־��1��ʾ����ģʽ�ر�������1��0��ʾû��
/******************************************************************************///EGS702
#elif    defined EYE_EGS702
    extern u8 NetFirstManualCloseFlag;
/******************************************************************************///ȱʡ
#else
    u8 NetFirstManualCloseFlag = 0;
#endif

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
/**
  * @brief  ��ͨѶģ���Դ
  * @param  None.
  * @retval None.
  */
#ifndef   EYE_EGS702  
void GSM_PWR_ON(void)
{

}
#endif
/**
  * @brief  �ر�ͨѶģ���Դ
  * @param  None.
  * @retval None.
  */
void GSM_PWR_OFF(void)
{

}

/**
  * @brief  ͨѶģ�鴦������
  * @param  None.
  * @retval ���������������.
  */
FunctionalState Communication_TimeTask(void)
{
#ifdef EYE_MODEM
    Modem_Run();  
    Net_First_Run();
    Net_Other_Run();
    #ifdef EYE_EGS701
    Net_Second_Run();
    #endif
#endif    

    if(communication_tasksta == 0)
    {
        return DISABLE;
    }
    return ENABLE;
}
/**
  * @brief  ͨѶģ����������
  * @param  state 0 -> �ر�����;
  *               1 -> ������;
  * @retval None.
  */
void Communication_TaskSet(u8 state)
{
    if(state == 0)
    {
        communication_tasksta = 0;
    }
    else
    {
        communication_tasksta = 1;
    }
}
/**
  * @brief  ͨѶģ���ʼ��
  * @param  None.
  * @retval None.
  */
void Communication_Init(void)
{
#ifdef EYE_MODEM
    Modem_Api_Start();
#endif
}
/**
  * @brief  ����绰�ص�����
  * @param  *pCode :ָ��绰����
  * @param  type   :�绰����
  *                 PHONE_NORMAL  : ����ͨ�� 
  *                 PHONE_MONITOR : ���� 
  * @retval �ɹ�����1  ʧ�ܷ���0
  */
u8 communication_CallPhone(u8 *pCode, CALL_TYPE type)
{
#ifdef EYE_MODEM
    if (type > PHONE_END)
    {
        return 0;
    }    
    if(type == PHONE_NORMAL)////////////////////////////////////////////����绰
    {
        if(Modem_Api_Phone_Atd(pCode,1))
        {
            
            return 1;
        }
    }
    else if(type == PHONE_MONITOR)//////////////////////////////////////�����绰
    {
        if(Modem_Api_Phone_Atd(pCode,0))
        {
            
            return 1;
        }
    }
    else if(type == PHONE_TALK)/////////////////////////////////////////////����
    {
        if(Modem_Api_Phone_Ata())
        {
            return 1;
        }
    }
    else if(type == PHONE_END)//////////////////////////////////////////////�Ҷ�
    {
        if(Modem_Api_Phone_Ath())
        {
            return 1;
        }
    }
#endif    
    return  0;
}

/**
  * @brief  ��ȡ�ź�ǿ��
  * @param  None.
  * @retval �����ź�ǿ��.
  */
u8 communicatio_GetSignalIntensity(void)
{
#ifdef EYE_MODEM
    return Modem_Port_Csq();
#else
    return 0;
#endif
}
/**
  * @brief  ���ͻ������ݵ�����
  * @param    channel:����ͨ��
  * @param    srcdat :ָ��������
  * @param    srclen :�������ݳ���
  * @retval ���ط��ͽ��
  *                              0 :ʧ��  
  *                     ����srclen :�ɹ�
  */
u16 Communication_SndMsg2Svr(u8 channel, u8 * srcdat, u16 srclen, u8 *phone)
{
    u16 len = 0;
    u8  tab[MODEM_SMS_SENDMAX];
#ifdef EYE_MODEM
    ////////////////////////////////////////////////////////////////////����ͨ��    
    if(channel == CHANNEL_SMS)
    {
        if(Modem_Type == MODEM_TYPE_ZTE)/////���� ǿ��תΪ�ı�ģʽ
        {
            if((srclen*2) > MODEM_SMS_SENDMAX)
            {
                return 0;
            }
            len  = ModLib_ByteStr(tab,srcdat,srclen);
            Modem_Api_SmsSend(SMS_TXT,NULL,tab,len);
            return len;
        } 
        len = srclen;
        Modem_Api_SmsSend(SMS_PDU,NULL,srcdat,srclen);
    }
    ////////////////////////////////////////////////////////////////////�ڶ�ͨ��    
    #ifdef MODEM_APP_SECOND_H
    else if(channel == CHANNEL_DATA_2)
    {
        len = Net_Second_Send(srcdat,srclen);
    }
    #endif
    ////////////////////////////////////////////////////////////////////��һͨ��
    else
    {
        len = Net_First_Send(srcdat,srclen);
    }
#endif    
    return len;
}

/**
  * @brief  ��ȡģ��״̬
  * @param  type :��ȡ����������
  *              COMMUNICATE_STATE :ͨѶģ��״̬
  *              SIGN_STRENTH      :�ź�����
  * @retval ����������Ϣ
  */
u16 communicatio_GetMudulState(GPRSSTATE type)
{
    u8 ch=0;

    if(COMMUNICATE_STATE == type)
    {
       ch = Net_First_ConSta();
    } 
    else if(SIGN_STRENTH == type) 
    {
        ch = Modem_Port_Csq();
    }
 
    return (ch);
}
/**
  * @brief  ��ȡ����״̬
  * @param  channel : ͨ����
  * @retval ����������Ϣ
  */
u8 communicatio_NetState(u8 channel)
{
    u8 ch=0;
    
    #ifdef MODEM_APP_SECOND_H
    if(channel == CHANNEL_DATA_2) 
    {
       ch = Net_Second_ConSta();
    } 
    else
    #endif
    {
        ch = Net_First_ConSta();
    }
  
    return (ch);
}

/*******************************************************************************
* Function Name  : Communication_Open
* Description    : ��������ͨ��
* Input          : None
* Output         : None
* Return         : �б걱������ä����������ʱ,���������Ҫ�����������
*******************************************************************************/
void Communication_Open(void)
{
#ifdef EYE_MODEM
    Net_First_Open();
    NetFirstManualCloseFlag = 0;
#endif    
}
/*******************************************************************************
* Function Name  : Communication_Close
* Description    : �ر�����ͨ��
* Input          : None
* Output         : None
* Return         : �б걱������ä����������ʱ,���������Ҫ�����������
*******************************************************************************/
void Communication_Close(void)
{
#ifdef EYE_MODEM
    Net_First_Close();  
    NetFirstManualCloseFlag = 1;
#endif    
}
/*******************************************************************************
* Function Name  : Communication_Close2
* Description    : �ر�����ͨ��
* Input          : None
* Output         : None
* Return         : �б걱������һ���ܲ���ʱ,��Ҫ�����������
*******************************************************************************/
void Communication_Close2(void)
{     
#ifdef EYE_MODEM
    Net_First_Close(); 
    NetFirstManualCloseFlag = 1;
#endif    
}

/**
  * @brief  �ر���ͨ����ip����
  * @param  None.
  * @retval None.
  */
void MOD_CloseMainIp(void)
{
#ifdef EYE_MODEM
    Net_First_Close(); 
	  NetFirstManualCloseFlag = 1;
#endif  
}
/**
  * @brief  ��ȡ�ն�SIM��ICCID��
  * @param  *Bcdbuff :ָ�򱻱���Ļ���.
  * @retval None.
  */
void MOD_GetIccidBcdValue(u8 *Bcdbuff)
{
#ifdef EYE_MODEM
    Modem_Api_CCID(Bcdbuff);
#endif    
}
/**
  * @brief  ��������
  * @param  None.
  * @retval None.
  */
void SetTelVolumeIncr(void)
{
#ifdef EYE_MODEM
    u8 value=0;
    FRAM_BufferRead((u8*)&value,FRAM_TEL_VOLUME__LEN,FRAM_TEL_VOLUME_ADDR);

    if(value < 8)
    {
        value++;
        if(Modem_Api_Phone_Clvl(value))
        {
            FRAM_BufferWrite(FRAM_TEL_VOLUME_ADDR,(u8*)&value,FRAM_TEL_VOLUME__LEN);  
        }
    }
#endif   
}
/**
  * @brief  ��С����
  * @param  None.
  * @retval None.
  */
void SetTelVolumeDecr(void)
{
#ifdef EYE_MODEM
    u8 value=0;
    FRAM_BufferRead((u8*)&value,FRAM_TEL_VOLUME__LEN,FRAM_TEL_VOLUME_ADDR);
    
    if(value > 1)
    {
        value--;
        if(Modem_Api_Phone_Clvl(value))
        {
            FRAM_BufferWrite(FRAM_TEL_VOLUME_ADDR,(u8*)&value,FRAM_TEL_VOLUME__LEN);
        }      
    }
#endif    
}
/**
  * @brief  ��ȡ��ǰ�绰����
  * @param  None.
  * @retval None.
  */
u8 GetTelVolume(void)
{
#ifdef EYE_MODEM
    u8 value;
    
    value = Modem_Api_Phone_ClvlGet();

    return value;
#else
    return 0;
#endif  
}
/**
  * @brief  ��������
  * @param  None.
  * @retval None.
  */
void SetModTtsVolumeIncr(void)
{
#ifdef EYE_MODEM
    u8 value=0;
    FRAM_BufferRead((u8*)&value,FRAM_TTS_VOLUME_LEN,FRAM_TTS_VOLUME_ADDR);

    if(value < 8)
    {
        value++;
        if(Modem_Api_Tts_Clvl(value))
        {
            FRAM_BufferWrite(FRAM_TTS_VOLUME_ADDR,(u8*)&value,FRAM_TTS_VOLUME_LEN);  
        }
    }
#endif   
}
/**
  * @brief  ��С����
  * @param  None.
  * @retval None.
  */
void SetModTtsVolumeDecr(void)
{
#ifdef EYE_MODEM
    u8 value=0;
    FRAM_BufferRead((u8*)&value,FRAM_TTS_VOLUME_LEN,FRAM_TTS_VOLUME_ADDR);
    
    if(value > 1)
    {
        value--;
        if(Modem_Api_Tts_Clvl(value))
        {
            FRAM_BufferWrite(FRAM_TTS_VOLUME_ADDR,(u8*)&value,FRAM_TTS_VOLUME_LEN);
        }      
    }
#endif    
}
/**
  * @brief  ��ȡ��ǰTTS����
  * @param  None.
  * @retval None.
  */
u8 GetModTtsVolume(void)
{
#ifdef EYE_MODEM
    u8 value;
    
    value = Modem_Api_Tts_IsClvl();

    return value;
#else
    return 0;
#endif  
}
/**
  * @brief  IC����֤������������
  * @param  None.
  * @retval ��������״̬.
  */
/*
FunctionalState ICAUC_TimeTask(void)
{   

    return ENABLE;
}
*/
/**
  * @brief  ������IC����֤���Ľ�������,IC��֤����IP��ַ�����ò����ж�ȡ��
  * @param  None.
  * @retval �ɹ�����ICAUC_RET_SUCCED,���򷵻ش�����.
  */
/*
ICAUC_RetType ICAUC_OpenLnk(void)
{
    return ICAUC_RET_SUCCED;
}*/
/**
  * @brief  ��IC����֤���ķ�������
  * @param  None.
  * @retval �ɹ�����ICAUC_RET_SUCCED,���򷵻ش�����.
  */
    /*
ICAUC_RetType ICAUC_SendBuff(u8* buff,u16 len)
{

    return ICAUC_RET_SENDBUF_NO_CMD;    
}*/
/**
  * @brief  ע��һ����ȡ����
  * @param  pFun :���ݶ�ȡ��������.
  * @retval None
  */
    /*
void ICAUC_RegReadFun(ICAUC_FUN_READ pFun)
{

}
*/

/*********************************************************************
//��������  :ICAUC_RegReadFun
//����      :��ȡ����״̬
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      : ���ӳɹ�����1,ʧ�ܷ���0
//��ע      :
*********************************************************************/
/**
  * @brief  ��ȡ����״̬
  * @param  None
  * @retval None
  */
/*u8 ICAUC_GetLnkStatus(void)
{
    return 0;
}
*/
#if 0
/********************************************************************
* ����    :  MOD_InitCRC
* ����    :  FTP����������CRCУ��
* ����    :  ��
* ���    :  ��
* ����    :  bool
* ���ú���:
* ����    :2011.09.22 Wangj
* ˵��    ://dxl,2012.6.1
********************************************************************/
void MOD_InitCRC(void)
{
    /* Enable CRC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    /* Reset CRC generator */
    CRC->CR = CRC_CR_RESET;
}
/********************************************************************
* ����    :  MOD_StopCRC
* ����    :  ֹͣCRCʱ��
* ����    :  ��
* ���    :  ��
* ����    :  bool
* ���ú���:
* ����    :2011.09.22 Wangj
* ˵��    :dxl,2012.6.1
********************************************************************/
void MOD_StopCRC(void)
{
    /* Disable CRC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
}
/********************************************************************
* ����    :  MOD_CalcCRC
* ����    :  ͨ��STM32Ӳ��CRC����У���
* ����    :  *pBuffer �����������
              BufferLength ���������ݳ���
* ���    :  ��
* ����    :  CRCֵ
* ���ú���:
* ����    :2011.09.22 Wangj
* ˵��    :
********************************************************************/
u32 MOD_CalcCRC(u8 *pBuffer, u32 BufferLength)
{
    u32  i = 0;
    u32  ulTmp;
    
    if (pBuffer != NULL) {
        for(i = 0; i < BufferLength; i++) {
            ulTmp = *((uint8_t*)pBuffer + i);
            CRC->DR = ulTmp;            
        }
    }
        
    return(CRC->DR);
}
#endif
/*
********************************************************************************
*                               DEFIEN END
********************************************************************************
*/
#endif