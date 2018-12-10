/**
  ******************************************************************************
  * @file    modem_uart.c
  * @author  miaoyahan 
  * @version V1.0.0
  * @date    2013-08-12
  * @brief   modem���ڲ�����������ʼ�����ں��Լ���ʼ����Ӧ�жϡ�
  ******************************************************************************
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
*                             LOCAL DEFINES
********************************************************************************
*/
#define MODEM_UART_ATSV_SIZE  (55)  //���һ��AT����(�ܿػ���,�����޸�!!!)

/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/
static u8  Modem_Node_AtBuf[MODEM_UART_ATSV_SIZE];
static u8  Modem_Node_AtCnt;

static u16 Modem_Node_SocAll;//���ݳ���
static u16 Modem_Node_SocCnt;//���ݼ�����
static u16 Modem_Node_SocOut;//���ݳ�ʱ������ �����յ���������

static u16 Modem_Node_FtpDwn;//FTP���ر�־ ��������Ϊ1  ����Ϊ0

static u8         Modem_Uart_RxBuf[MODEM_UART_RX_SIZE];//���ջ���
static MODLIB_BUF Modem_Uart_RxNdNum[MODEM_UART_NDRX_SIZE];//�ڵ㻺��
static MODLIB_NOD Modem_Uart_RxNode;//���ڽ��սڵ�ṹ��

static u8         Modem_Uart_TxBuf[MODEM_UART_TX_SIZE];//���ͻ���
static MODLIB_BUF Modem_Uart_TxNdNum[MODEM_UART_NDTX_SIZE];//�ڵ㻺��
static MODLIB_NOD Modem_Uart_TxNode;//���ڷ��ͽڵ�ṹ��

static u8 Modem_Uart_TxBusy;//���ڷ���Ϊ1  ����Ϊ0
/*
********************************************************************************
*                          GLOBAL VARIABLES
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
  * @brief  ���ݽ��ռ��������� 
  * @param  data_end -> ��ǰ������־״̬ 
  * @retval ���ؽ�����־���
  */
u16  Modem_Uart_RecCnt(u16 data_end)
{
    u16 result = data_end;//Ĭ�������ݽ���

    if(Modem_Type == MODEM_TYPE_Q)
    {
        if(Modem_Node_SocAll)
        {
            Modem_Node_SocCnt++;
            
            if(Modem_Node_SocAll == Modem_Node_SocCnt)
            {
                result = 0x0d0a;//�յ���������
            }
            else
            {
                result = 0;//����δ����
            }
        }
    }
    return result;
}

/*
  * @brief  ���ݽ���ָ��� 
  * @param  *pData : ָ��AT�����ַ
  *          len   : ��ǰ�������ݵĳ���
  * @retval ��ʾδ����־ -> 0;
  *         ���ؼ����洢 -> 1;
  */
u8  Modem_Uart_RecCmd(u8 *pData, u16 len)
{
    u8 *p = pData;
    u16 temp;
////////////////////////////////////////////////////////////////////////////////��Զ
    if(Modem_Type == MODEM_TYPE_Q)
    {
        if(Modem_Node_FtpDwn == 0)//��FTP������
        {
            if(ModLib_AtCmp(p,"+RECEIVE: ") == 0)//+RECEIVE: 0, 3
            {
                if(Modem_Node_SocAll == 0)
                {
                    p += 13;
                    Modem_Node_SocAll = atoi((char*)p);
                    Modem_Node_SocCnt = 0;
                    return 1;
                }
                Modem_Node_SocAll = 0;
                Modem_Node_SocCnt = 0;
            }
            ////////////////////////////////////////////////////////////////////
            else if(ModLib_AtCmp(p,"CONNECT") == 0)//��ʼ�����ļ�
            {
                Modem_Node_FtpDwn = 1;
                Modem_Node_SocAll = 512;//512���ֽ�һ��
            }
        }
        else//FTP������
        {
            Modem_Node_SocCnt = 0;//���������Ϊ��һ��512���ֽ���׼��
        }
    }
////////////////////////////////////////////////////////////////////////////////����
    else if(Modem_Type == MODEM_TYPE_ZTE)
    {
        if(ModLib_AtCmp(p,"+ZIPRECV:") == 0)//+ZIPRECV:0,4,1234
        {
            temp = ModLib_AtAddr(p,',')+1;
            p += temp;
            Modem_Node_SocAll = atoi((char*)p);//��ȡ����
            
            temp = ModLib_AtAddr(p,',') + temp + 1 + 2;//ʵ�ʳ���
            Modem_Node_SocCnt = len - temp;
            
            if((Modem_Node_SocCnt != Modem_Node_SocAll)
             &&(Modem_Node_SocCnt < Modem_Node_SocAll))
            {
                return 1;
            }
            Modem_Node_SocAll = 0;
            Modem_Node_SocCnt = 0;
        }
        else if(ModLib_AtCmp(p,"+ZFTPDATA:") == 0)//+ZFTPDATA:1024,1024\r\n +ZFTPDATA:CLOSED\r\n
        {
            if(ModLib_AtCmp(p,"+ZFTPDATA:CLOSED") == 0)
            {
                modftp.fend = 1;
            }
            else
            {
                temp = ModLib_AtAddr(p,',')+1;
                p += temp;
                Modem_Node_SocAll = atoi((char*)p);//��ȡ����
                p    = pData;
                temp = ModLib_AtLen(p)+2;
                if(len == temp)
                {
                    Modem_Node_SocCnt = 0;
                }
                else 
                {
                    Modem_Node_SocCnt = len - temp - 2;//ʵ�ʳ���
                }
                if((Modem_Node_SocCnt != Modem_Node_SocAll)
                 &&(Modem_Node_SocCnt < Modem_Node_SocAll))
                {
                    return 1;
                }
                Modem_Node_SocAll = 0;
                Modem_Node_SocCnt = 0;
            }
        }
    }
    
    return 0;
}

/*
  * @brief  �ڵ㸴λ
  * @param  None  
  * @retval None
  */
void  Modem_Uart_NodRst(void)
{
    memset((u8*)Modem_Node_AtBuf,0,sizeof(Modem_Node_AtBuf));
    Modem_Node_AtCnt  = 0;
    Modem_Node_SocAll = 0;
    Modem_Node_SocCnt = 0;
    Modem_Node_SocOut = 0;
    Modem_Node_FtpDwn = 0; 
    ModLib_NodRst(&Modem_Uart_RxNode); 
    ModLib_NodRst(&Modem_Uart_TxNode); 
}

/*
  * @brief  ��ǰ�ڵ���
  * @param  None
  * @retval ���ؽڵ���
  */
u8 Modem_Uart_NodNum(void)
{
    u8 len;
    
    len = ModLib_NodNum(&Modem_Uart_RxNode);//��ѯ�Ƿ���������Ҫ����

    return len;
}

/*
  * @brief  ���ʣ��ڵ�
  * @param  None
  * @retval None
  */
void Modem_Uart_NodClr(void)
{
    if(ModLib_NodNum(&Modem_Uart_RxNode))
    {
        ModLib_NodRst(&Modem_Uart_RxNode);
    }
}
/*
  * @brief  �ӽڵ���ȡ������
  * @param  None
  * @retval �������ݳ���
  */
u16 Modem_Uart_NodRet(u8 *pData)
{
    u16 len;

    len = ModLib_NodRet(&Modem_Uart_RxNode,pData);//��ȡ����
    if(len)
    {
        len -= 1;//��ȡ1�ֽڽ�����
    }
    return len;
}
/*
  * @brief  ����ڵ��ڵ�����
  * @param  ch:Ҫ����ڵ���ֽ�
  * @retval ���ؽ��յ����ֽ�
  */
u8 Modem_Uart_NodIns(u8 ch)
{
    static u16 AtEnd;//��������� \r\n
    
    ////////////////////////////////////////////////////////////////////��������
    if(ModLib_NodIns(&Modem_Uart_RxNode,INS_HALF,&ch,1) == 0)
    {
        return 0;
    }
    ////////////////////////////////////////////////////////////////\r\n��������
    AtEnd = AtEnd <<8;
    AtEnd |= ch;

    //////////////////////////////////////////////////////////////����һ��AT����
    if(Modem_Node_AtCnt < MODEM_UART_ATSV_SIZE)
    {
        Modem_Node_AtBuf[Modem_Node_AtCnt++] = ch;
    }

    //////////////////////////////////////////////////////////////���ݼ���������
    AtEnd = Modem_Uart_RecCnt(AtEnd);

    /////////////////////////////////////////////////////////���0X0D,0X0A������
   if(AtEnd == 0x0d0a)
    {
        if(Modem_Uart_RecCmd(Modem_Node_AtBuf,ModLib_NodLen(&Modem_Uart_RxNode)))
        {
            return ch;
        }
        ch = 0x24;//������
        ModLib_NodIns(&Modem_Uart_RxNode,INS_ALL,&ch,1);//�ڵ���
        //////////////////////////////////////////////////////////���AT�����
        Modem_Node_AtCnt = 0;
        memset(Modem_Node_AtBuf,0,MODEM_UART_ATSV_SIZE);
    }
    return ch;
}

/*
  * @brief  �ڵ�ڵ㳬ʱ����
  * @param  None  
  * @retval None
  */
void  Modem_Uart_NodOut(void)
{
    u16 outtime = MODEM_TSEC*30;

    Modem_Node_SocOut++;
    
    if(Modem_Node_SocOut < outtime)
    {
        return;
    }
    Modem_Node_SocOut = 0;

    if(Modem_Node_SocAll)
    {
        Modem_Node_SocAll = Modem_Node_SocCnt+1;
        Modem_Uart_NodIns(0x55);
        
        if(Modem_Node_FtpDwn)
        {
            Modem_Node_FtpDwn = 0;//�������ر�־
            Modem_Node_SocAll = 0;
            Modem_Node_SocCnt = 0;
            modftp.fend = 1;//���ؽ���
        }
    }
}
    
/*
  * @brief  ModemӲ��������ʼ��,��������.
  * @param  None
  * @retval None
  */
void  Modem_Uart_Init (void)
{  
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; 

      /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(Modem_UART_TX_GPIO_CLK | Modem_UART_RX_GPIO_CLK, ENABLE);

    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(Modem_UART_CLK, ENABLE);
    
    USART_InitStructure.USART_BaudRate = Modem_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    /* Connect PXx to USARTx_Tx*/
   // GPIO_PinAFConfig(Modem_UART_TX_GPIO_PORT, Modem_UART_TX_SOURCE, Modem_UART_TX_AF);

    /* Connect PXx to USARTx_Rx*/
   // GPIO_PinAFConfig(Modem_UART_RX_GPIO_PORT, Modem_UART_RX_SOURCE, Modem_UART_RX_AF);
    //GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    /* Configure USART Tx as alternate function  */
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = Modem_UART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(Modem_UART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = Modem_UART_RX_PIN;
    GPIO_Init(Modem_UART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* USART configuration */
    USART_Init(Modem_UART, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(Modem_UART, ENABLE);
    
    /* configure Modem_UART IT and Enable RxNE IT*/
    USART_ITConfig(Modem_UART, USART_IT_RXNE, ENABLE);

    /* Enable the Modem_UART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = Modem_UART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ModLib_NodInit(&Modem_Uart_RxNode,Modem_Uart_RxNdNum,MODEM_UART_NDRX_SIZE,
                                    Modem_Uart_RxBuf,MODEM_UART_RX_SIZE);
                                    
    ModLib_NodInit(&Modem_Uart_TxNode,Modem_Uart_TxNdNum,MODEM_UART_NDTX_SIZE,
                                    Modem_Uart_TxBuf,MODEM_UART_TX_SIZE);     
}
/*
  * @brief  �򴮿ڷ���ָ����������
  * @param  None
  * @retval None
  */
u16 Modem_Uart_Send(u8* data,  u16 len)
{
    u16 SendLen = 0;

    //SendLen = ModLib_NodIns(&Modem_Uart_TxNode,INS_ALL,data,len); dxl,2016.8.27����
    if(Modem_Uart_TxBusy == 0)
    {
			  SendLen = ModLib_NodIns(&Modem_Uart_TxNode,INS_ALL,data,len);
        Modem_Uart_TxBusy = 1;
        USART_ITConfig(Modem_UART, USART_IT_TXE, ENABLE);
    }
  return SendLen;
}
/*
  * @brief  �����жϴ���
  * @param  None
  * @retval None
  */

void Modem_USART_IRQHandler (void)
{
    u8 byte;
    u8 insfla = 0;
    ////////////////////////////////////////////////////////////////////////����
    if (USART_GetITStatus(Modem_UART, USART_IT_TXE) == SET)
    {
        if(ModLib_NodRetEx(&Modem_Uart_TxNode,&byte,1))
        {
            USART_SendData(Modem_UART, byte);
            Modem_Node_SocOut = 0;
        #ifdef MODEM_DEBUG_H
            if((ModDbInfType == MODDB_CON)||(ModDbInfType == MODDB_ALL))
            {
                Modem_Debug_UartTx(&byte,1);
            }
        #endif            
        }
        else
        {
            USART_ITConfig(Modem_UART, USART_IT_TXE, DISABLE);
            Modem_Uart_TxBusy = 0;
        }
    }
    ////////////////////////////////////////////////////////////////////////����    
    if(USART_GetITStatus(Modem_UART, USART_IT_RXNE) == SET)
    {
        insfla = 1;
    }
    ////////////////////////////////////////////////////////////////////////���        
    if(USART_GetFlagStatus(Modem_UART, USART_FLAG_ORE) != RESET)
	{
        insfla = 1;//����Ӱ��,��ȡ�����ݷ�ֹ����
	}
    ////////////////////////////////////////////////////////////////////////����
	if(insfla)
	{
        byte = USART_ReceiveData(Modem_UART);
        Modem_Uart_NodIns(byte);
        Modem_Node_SocOut = 0;
    #ifdef MODEM_DEBUG_H
        if((ModDbInfType == MODDB_CON)||(ModDbInfType == MODDB_ALL))
        {
            Modem_Debug_UartTx(&byte,1);
        }
    #endif  
	}
}


