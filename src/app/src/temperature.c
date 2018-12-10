/*
********************************************************************************
*
* Filename      : temperature.c
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-12-4  
* 
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
#ifdef TEMPERATURE_H

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
static u8  Temp_16PinFlg;//16PIN�ӿڽ����¶ȴ�������־  1Ϊ��Ч
static u8  Temp_ValBuf[50];
static u16 Temp_SendNum=0;//��ˮ��
static u16 Temp_ErrCnt;//���������
static u16 Temp_ErrFlg;//�����־ 0:����  1:�쳣
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
/**
  * @brief  λ����Ϣ������Ϣ 0xe8
  * @param  ָ�����ݵ�ַ
  * @retval �������ݳ���
  */
u16  Temperature_PosEx(u8 *pDst)
{
    u8 len;
    len = Temp_ValBuf[0];
    if((len == 0)||(Temp_ErrFlg))
    {
        return 0;
    }
    *pDst++ = 0xE8;//������ϢID
    *pDst++ = len;//���ӳ���
    memcpy(pDst,Temp_ValBuf+1,len);
    return (len+2);
}
/**
  * @brief  �¶ȳ�ʼ��
  * @param  None
  * @retval None
  */
void Temperature_Init(void)
{
    static u8 flag;
    u8 tab[5];

    if(flag)
    {
        return;//�ѳ�ʼ�����
    }
    flag = 1;

    if(EepromPram_ReadPram(E2_POS_PORT_ID, tab))
    {
        if(tab[1]&0x20)// bit5: 16PIN ����Ϊ�¶ȴ�����
        {
            Temp_16PinFlg = 1;//
            NaviLcdPowerOn();
        }
    }
}
/**
  * @brief  �������践�ص��¶�����
  * @param  None
  * @retval None
  */
void Temperature_ComRec(u8 *pSrc, u16 len)
{
    TEMP_DATA  tempData;
    
    u8  *pVer=pSrc;
    u8   verf=0;
    u8   i;

    if(Temp_16PinFlg == 0)
    {
        return;//δ���봫����
    }
    if(*pSrc++ != 0xff)
    {
        return;//��ʾ������
    }    
    if(*pSrc++ != 0x55)
    {
        return;//��ʾ������
    }
    if(*pSrc++ != 0xaa)
    {
        return;//��ʾ������
    }

    len -= 4;
    pVer = pSrc;
    for(i=3;i<len;i++)
    {
        verf ^= *pVer++;
    }
    
    if(verf != *pVer)
    {
        return;//У�����
    }

    tempData.len  = *pSrc++;//����
    tempData.len  = (tempData.len<<8)|*pSrc++;
    tempData.len -= 4;

    if(*pSrc++ != 0xf0)//�豸����  
    {
        return;//���¶Ȳɼ���
    }
    
    pSrc++;//���̱�ʾ
    
    Temp_ErrCnt=0;//����
    Temp_ErrFlg=0;//����
    Io_WriteSelfDefine2Bit(DEFINE_BIT_15,RESET);//�¶ȴ���������
    
    tempData.cmd  = *pSrc++;//������
    tempData.cmd  = (tempData.cmd<<8)|*pSrc++;

    memcpy(tempData.buf,pSrc,tempData.len);//����

    if((tempData.cmd == 0x0110)||(tempData.cmd == 0x0111))//�¶�����
    {
        Temp_ValBuf[0]=tempData.len-2;//����������
        memcpy(Temp_ValBuf+1,tempData.buf+2,Temp_ValBuf[0]);//У����ȷˢ���¶�ֵ        
    }
    else if(tempData.cmd == 0x0112)//������д
    {
        Temperature_8900Send(&tempData);//͸����ƽ̨
    }
}
/**
  * @brief  �����跢�ͻ�ȡ�¶�����
  * @param  None
  * @retval None
  */
void Temperature_ComSend(TEMP_DATA *pTemp)
{
    u8  send_buf[50];
    u8  send_len;
    u8  verf;
    u8  i;

    send_len = 0;
    send_buf[send_len++]=0xff;
    send_buf[send_len++]=0x55;    
    send_buf[send_len++]=0xaa;  
    send_buf[send_len++]=0;                 
    send_buf[send_len++]=pTemp->len+4;      //����
    send_buf[send_len++]=0xf0;              //�豸����  
    send_buf[send_len++]=0x00;              //���̱�ʶ��
    send_buf[send_len++]=pTemp->cmd>>8;     //������
    send_buf[send_len++]=pTemp->cmd&0xff;

    memcpy(send_buf+send_len,pTemp->buf, pTemp->len);
    send_len += pTemp->len;

    for(i=3,verf=0;i<send_len;i++)
    {
        verf ^= send_buf[i];
    }
    send_buf[send_len++]=verf;               //У��
    send_buf[send_len++]=0x55;
    send_buf[send_len++]=0xaa;
    send_buf[send_len++]=0xff;

    Temp_SendNum++;                          //�ۼ�
    
    COM2_WriteBuff(send_buf,send_len);       //����
    if(Temp_ErrCnt++ >= 3)
    {
        Temp_ErrFlg=1;//�쳣
        Io_WriteSelfDefine2Bit(DEFINE_BIT_15,SET);//�¶ȴ���������
    }
}
/**
  * @brief  ����ƽ̨�·�͸������
  * @param  None
  * @retval None
  */
void Temperature_DisposeRadioProtocol(u8 cmd, u8 *pSrc, u16 len)
{
    TEMP_DATA  tempData;

    if(cmd != 0x00)
    {
        return;//ָ�����
    }

    tempData.cmd = 0x0112;
    tempData.len = len;
    memcpy(tempData.buf,pSrc,len);
    Temperature_ComSend(&tempData);//ת�����¶Ȳɼ���
}
/**
  * @brief  ��ƽ̨����͸������
  * @param  None
  * @retval None
  */
void Temperature_8900Send(TEMP_DATA *pTemp)
{
    EIExpand_PotocolSendData(0xf8f0,pTemp->buf,pTemp->len);
}

/**
  * @brief  �¶ȴ�������
  * @param  None
  * @retval None
  */
FunctionalState Temperature_TimeTask(void)
{
    static u16 timecnt;
    TEMP_DATA  tempData;

    //////////////////////////////////////////////////////////////////////��ʼ��
    Temperature_Init();
    if(Temp_16PinFlg == 0)
    {
        return DISABLE;//���¶����裬�ر�����
    }
    
    //////////////////////////////////////////////////////////////////5S��ȡ�¶�
    if(timecnt++ > (SYSTICK_1SECOND*5))
    {
        timecnt = 0;
        tempData.cmd    = 0x0111;
        tempData.buf[0] = Temp_SendNum>>8;
        tempData.buf[1] = Temp_SendNum&0xff;
        tempData.len    = 2;
        Temperature_ComSend(&tempData);
    }
    
    return ENABLE;
}

/*
********************************************************************************
*                 DEFIEN END
********************************************************************************
*/
#endif 
