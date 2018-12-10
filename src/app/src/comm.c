/*******************************************************************************
 * File Name:           comm.c 
 * Function Describe:   
 * Relate Module:       �����ж�
 * Writer:              Joneming
 * Date:                2014-02-21
 * ReWriter:            
 * Date:                
 *******************************************************************************/

//****************�����ļ�*****************
#include "hal.h"
#include "Comm.h"
#include "public.h"
#include "VDR_Usart.h"

////////////////////////
enum 
{
    COMM_ATTRIB_EMPTY=0,//
    COMM_ATTRIB_CTRL_VALID,//    
    COMM_ATTRIB_CTRL_SEND,//  
    COMM_ATTRIB_CTRL_RECV,// 
    COMM_ATTRIB_MAX
}E_COMMATTRIB;

typedef struct
{
    ST_QUEUE stRxQueue;//���ն���
    ST_QUEUE stTxQueue;//���Ͷ���
    unsigned char sendBusy;//
    unsigned char ctrlValidFlag;//
    unsigned char ctrlStatus;//
    void (*recvEnable)(void);//
    void (*sendEnable)(void);//
}ST_COMM_ATTRIB;
////////////////////////////////
static ST_COMM_ATTRIB s_stCommAttrib[COM_MAX];
///////////////////////
static QueueData s_QDCOMRxData1[COM1_MAX_LEN_RX+1];
static QueueData s_QDCOMTxData1[COM1_MAX_LEN_TX+1];
///////////////////////////
static QueueData s_QDCOMRxData2[COM2_MAX_LEN_RX+1];
static QueueData s_QDCOMTxData2[COM2_MAX_LEN_TX+1];
////////////////////////////
static QueueData s_QDCOMRxData3[COM3_MAX_LEN_RX+1];
static QueueData s_QDCOMTxData3[COM3_MAX_LEN_TX+1];
///////////////////////////
static QueueData s_QDCOMRxData4[COM4_MAX_LEN_RX+1];
static QueueData s_QDCOMTxData4[COM4_MAX_LEN_TX+1];
/////////////////////////////
static QueueData s_QDCOMRxData5[COM5_MAX_LEN_RX+1];
static QueueData s_QDCOMTxData5[COM5_MAX_LEN_TX+1];
///////////////////////
static QueueData s_QDCOMRxData6[COM6_MAX_LEN_RX+1];
static QueueData s_QDCOMTxData6[COM6_MAX_LEN_TX+1];
////////////////

//****************��������*******************
/////////////////////���ն���////////////////////////////////////////////
//////////////����к�////////////////////////
#define Insert_COMRXQueue(COM,message)\
        LZM_QueueInsert(&s_stCommAttrib[COM].stRxQueue,message)
/////////////���������к�////////////////////////   
#define BatchDelete_COMRXQueue(COM,dest,maxcount)\
        LZM_QueueDataBatchDelete(&s_stCommAttrib[COM].stRxQueue,dest,maxcount);
/////////////////////���Ͷ���////////////////////////////////////////////
//////////////����к�////////////////////////
#define Insert_COMTXQueue(COM,message)\
        LZM_QueueInsert(&s_stCommAttrib[COM].stTxQueue,message)
/////////////��������к�////////////////////////   
#define BatchInsert_COMTXQueue(COM,dest,datalen)\
        LZM_QueueDataBatchInsert(&s_stCommAttrib[COM].stTxQueue,dest,datalen);
/////////////�����к�////////////////////////
#define Delete_COMTXQueue(COM)\
        LZM_QueueDelete(&s_stCommAttrib[COM].stTxQueue)
//////////////�ж�Ϊ��//////////////////////////////////
#define Empty_COMTXQueue(COM)\
        LZM_QueueEmpty(&s_stCommAttrib[COM].stTxQueue)
/////////////����ʣ��ռ�////////////////////////
#define FreeSpace_COMTXQueue(COM)\
        LZM_QueueDataFreeSpace(&s_stCommAttrib[COM].stTxQueue)

//****************��������******************
/*********************************************************************
//��������  :Init_COMRXQueue
//����      
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void COMM_QueueInit(COM_TypeDef COM)
{
    switch(COM)
    {
        case COM1:
            LZM_QueueDataInit(&s_stCommAttrib[COM].stTxQueue,s_QDCOMTxData1,COM1_MAX_LEN_TX);
            LZM_QueueDataInit(&s_stCommAttrib[COM].stRxQueue,s_QDCOMRxData1,COM1_MAX_LEN_RX);
            break;
        case COM2:
            LZM_QueueDataInit(&s_stCommAttrib[COM].stTxQueue,s_QDCOMTxData2,COM2_MAX_LEN_TX);
            LZM_QueueDataInit(&s_stCommAttrib[COM].stRxQueue,s_QDCOMRxData2,COM2_MAX_LEN_RX);
            break;
        case COM3:
            LZM_QueueDataInit(&s_stCommAttrib[COM].stTxQueue,s_QDCOMTxData3,COM3_MAX_LEN_TX);
            LZM_QueueDataInit(&s_stCommAttrib[COM].stRxQueue,s_QDCOMRxData3,COM3_MAX_LEN_RX);
            break;
        case COM4:
            LZM_QueueDataInit(&s_stCommAttrib[COM].stTxQueue,s_QDCOMTxData4,COM4_MAX_LEN_TX);
            LZM_QueueDataInit(&s_stCommAttrib[COM].stRxQueue,s_QDCOMRxData4,COM4_MAX_LEN_RX);
            break;
        case COM5:
            LZM_QueueDataInit(&s_stCommAttrib[COM].stTxQueue,s_QDCOMTxData5,COM5_MAX_LEN_TX);
            LZM_QueueDataInit(&s_stCommAttrib[COM].stRxQueue,s_QDCOMRxData5,COM5_MAX_LEN_RX);
            break;
        case COM6:
            LZM_QueueDataInit(&s_stCommAttrib[COM].stTxQueue,s_QDCOMTxData6,COM6_MAX_LEN_TX);
            LZM_QueueDataInit(&s_stCommAttrib[COM].stRxQueue,s_QDCOMRxData6,COM6_MAX_LEN_RX);
            break;
        default:
            break;
    }
}

/*************************************************************
** ��������: COMM_SetBaudRateEx
** ��������: ���ô��ڲ�����(��ӡ��ר��)(9λ,��У��)
** ��ڲ���: COM����,baud_rate:���ڲ�����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
void COMM_SetBaudRateEx(COM_TypeDef COM,unsigned long baud_rate)
{
    USART_InitTypeDef USART_InitStructure;
    //////////////////////////////
    if(COM>=COM_MAX)return;  
    memset(&USART_InitStructure,0,sizeof(USART_InitTypeDef));    
    USART_InitStructure.USART_BaudRate = baud_rate;                //ͨ�Ų���·
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;    //9λ����λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;         //1λֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_Odd ;          //��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(GetUsartPtr(COM), &USART_InitStructure);
}
/*************************************************************
** ��������: COMM_SetBaudRate
** ��������: ���ô��ڲ�����
** ��ڲ���: COM����,baud_rate:���ڲ�����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:USART_Init
*************************************************************/ 
void COMM_SetBaudRate(COM_TypeDef COM,unsigned long baud_rate)
{
    USART_InitTypeDef USART_InitStructure;
    //////////////////////////////
    if(COM>=COM_MAX)return;
    memset(&USART_InitStructure,0,sizeof(USART_InitTypeDef));    
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    
    USART_Init(GetUsartPtr(COM), &USART_InitStructure);
}
/*************************************************************
** ��������: COMM_SetBaudRateInitialize
** ��������: ���ô��ڲ�����
** ��ڲ���: COM����,baud_rate:���ڲ�����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: COM_Init
*************************************************************/ 
void COMM_SetBaudRateInitialize(COM_TypeDef COM,unsigned long baud_rate)
{
    USART_InitTypeDef USART_InitStructure;
    //////////////////////////////
    if(COM>=COM_MAX)return;
    //default reset state
    USART_DeInit(GetUsartPtr(COM));
    //////////////////////
    memset(&USART_InitStructure,0,sizeof(USART_InitTypeDef));    
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    
    COM_Init(COM, &USART_InitStructure);
    //�򿪴����ж�.
    COM_NVIC_Config(COM);                                //ʹ�ܴ����ж�
    /////ʹ�ܴ��ڽ����ж�//////////////////
    SetUsartITConfig(COM,USART_IT_RXNE,ENABLE);          //ʹ�ܴ��ڽ����ж�
}
/*************************************************************
** ��������: COMM_Initialize
** ��������: ��ʼ������,(�����շ����г�ʼ��������ʹ�ܼ������ж�)
** ��ڲ���: COM����,baud_rate:���ڲ�����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
void COMM_Initialize(COM_TypeDef COM,unsigned long baud_rate)
{
    if(COM>=COM_MAX)return;
    ////////////////
    memset(&s_stCommAttrib[COM],0,sizeof(ST_COMM_ATTRIB));
    ////�����շ�������/////////////
    COMM_QueueInit(COM);
    ////////////////////// 
    COMM_SetBaudRateInitialize(COM,baud_rate);
    ////////////////////
}

/*************************************************************OK
** ��������: Comm_CtrlEnableRecv
** ��������: ʹ�ܴ��ڽ���
** ��ڲ���: COM ����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Comm_CtrlEnableRecv(COM_TypeDef COM)
{
    if(COMM_ATTRIB_CTRL_VALID != s_stCommAttrib[COM].ctrlValidFlag)return;
    if(COMM_ATTRIB_CTRL_RECV == s_stCommAttrib[COM].ctrlStatus)return;
    s_stCommAttrib[COM].ctrlStatus = COMM_ATTRIB_CTRL_RECV;
    if(NULL!=s_stCommAttrib[COM].recvEnable)
    {
        s_stCommAttrib[COM].recvEnable();
    }
}
/*************************************************************OK
** ��������: Comm_CtrlEnableSend
** ��������: ʹ�ܴ��ڷ���
** ��ڲ���: COM ����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Comm_CtrlEnableSend(COM_TypeDef COM)
{
    if(COMM_ATTRIB_CTRL_VALID != s_stCommAttrib[COM].ctrlValidFlag)return;
    if(COMM_ATTRIB_CTRL_SEND == s_stCommAttrib[COM].ctrlStatus)return;
    s_stCommAttrib[COM].ctrlStatus = COMM_ATTRIB_CTRL_SEND;
    if(NULL != s_stCommAttrib[COM].sendEnable)
    {
        s_stCommAttrib[COM].sendEnable();
    }
}
/*************************************************************
** ��������: COMM_CtrlEnableInitialize
** ��������: �����շ�������Ƴ�ʼ��
** ��ڲ���: COM����,recvEnable:ʹ�ܽ���,sendEnable:ʹ�ܷ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void COMM_CtrlEnableInitialize(COM_TypeDef COM,void (*recvEnable)(void),void (*sendEnable)(void))
{
    s_stCommAttrib[COM].ctrlStatus      = COMM_ATTRIB_EMPTY;
    s_stCommAttrib[COM].ctrlValidFlag   = COMM_ATTRIB_CTRL_VALID;
    s_stCommAttrib[COM].recvEnable      = recvEnable;
    s_stCommAttrib[COM].sendEnable      = sendEnable;
    //////////////////////
    Comm_CtrlEnableRecv(COM);
}
/*************************************************************
** ��������: COMM_putc
** ��������: ����һ���ַ�
** ��ڲ���: COM����,ch
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
void COMM_putc(COM_TypeDef COM,int ch)
{
    while((GetUsartPtr(COM)->SR & USART_FLAG_TC) == RESET);
    GetUsartPtr(COM)->DR = (unsigned char)ch;
}

/*************************************************************
** ��������: COMM_putstr
** ��������: ����һ���ַ���
** ��ڲ���: COM����,str
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
void COMM_putstr(COM_TypeDef COM,char* str)
{
    for(; *str; str++)
    {
        COMM_putc(COM,*str);
    }
}

/*************************************************************
** ��������: COMM_RxIsr
** ��������: ���ݽ����жϴ�����
** ��ڲ���: COM����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
void COMM_RxIsr(COM_TypeDef COM)
{
    unsigned char tmp = 0;
    tmp = GetUsartReceiveData(COM);//ȡ����
    Insert_COMRXQueue(COM,tmp);
}
/*************************************************************
** ��������: COMM_TxIsr
** ��������: �����жϴ�����
** ��ڲ���: COM����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
void COMM_TxIsr(COM_TypeDef COM)
{
    if(!Empty_COMTXQueue(COM))    
    {
        UsartSendData(COM, Delete_COMTXQueue(COM));
    } 
    else 
    { //----- ���ݷ������ -----
        Comm_CtrlEnableRecv(COM);
        //////////////////
        s_stCommAttrib[COM].sendBusy = 0;
        SetUsartITConfig(COM,  USART_IT_TC, DISABLE);
    } 
}
/*************************************************************
** ��������: COMM_IRQHandler
** ��������: �����ж�
** ��ڲ���: COM:����
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void COMM_IRQHandler(COM_TypeDef COM)
{
    if(GetUsartStatus(COM, USART_IT_RXNE) != RESET)
    {
        COMM_RxIsr(COM);
    }
    ////////////////////////////////////
    if(GetUsartStatus(COM, USART_IT_TC) != RESET)                                         
    {
        COMM_TxIsr(COM);
    }
}
/*************************************************************
** ��������: COMM_SendData
** ��������: ��������
** ��ڲ���: COM:����,buff�����׵�ַ,len���ݳ���
** ���ڲ���: 
** ���ز���: 0:�ɹ���1:ʧ��,0xff:�������泤��
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char COMM_SendData(COM_TypeDef COM,unsigned char *buff, unsigned short len)
{
    //��鴮���Ƿ����ڷ�������
    if(Empty_COMTXQueue(COM))//û�з�������
    {
        if(len > FreeSpace_COMTXQueue(COM)) 
        {
            return 0xff;
        }
        else
        {
            Comm_CtrlEnableSend(COM);
            ////////////////////
            BatchInsert_COMTXQueue(COM,buff,len);
            s_stCommAttrib[COM].sendBusy = 1;
            SetUsartITConfig(COM, USART_IT_TC, ENABLE);
            return 0;
        }
    }
    else//���ڷ�������
    {
        if(FreeSpace_COMTXQueue(COM)>len)
        {
            Comm_CtrlEnableSend(COM);
            BatchInsert_COMTXQueue(COM,buff,len);
            s_stCommAttrib[COM].sendBusy = 1;
            SetUsartITConfig(COM, USART_IT_TC, ENABLE);
            return 0;
        }
        else
        {
            return 1;
        }
    }
}
/*************************************************************
** ��������: COMM_ReadData
** ��������: ��ȡ���������������
** ��ڲ���: COM:����,buff���ݻ�����,len��ȡ����
** ���ڲ���: 
** ���ز���: �������򷵻ض�ȡ���ֽ���,���򷵻�0
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned short COMM_ReadData(COM_TypeDef COM,unsigned char *buff,unsigned short len)
{
    return BatchDelete_COMRXQueue(COM,buff,len);
}
/*************************************************************
** ��������: COMM_GetSendBusy
** ��������: ��õ�ǰ���ڷ���æ��־
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 1:����æ,0���ڿ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char COMM_GetSendBusy(COM_TypeDef COM)
{
    return (1==s_stCommAttrib[COM].sendBusy)?1:0;
}
/******************************************************************************
**                            End Of File
******************************************************************************/

