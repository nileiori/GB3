/******************************************************************** 
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Gps_Driver.c       
//����      :GPSģ������
//�汾��    :
//������    :dxl
//����ʱ��  :2011.8
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :1)GPS������TTS���ô��ڣ�����Ҫ�뷢�������ݣ�������λTTS_P.
***********************************************************************/

//****************�ļ�����**************************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"

//****************�궨��********************

//***************��������********************
static stUartSend   Tx;
static u8   GpsRxBuffer[GPS_RX_BUFFER_SIZE];//���ջ���
static u8   GpsRxCount = 0;//���ռ���
static u8   GpsRxFrameCount = 0;//���յ�֡����
u8  GprmcBuffer[GPRMC_BUFFER_SIZE];//���GPRMC����
u8  GprmcBufferBusyFlag = 0;//GprmcBuffer����æ��־
u8  GpggaBuffer[GPGGA_BUFFER_SIZE];//���GPGGA����
u8  GpggaBufferBusyFlag = 0;//GpggaBuffer����æ��־
u8  GpgsvBuffer[GPGSV_BUFFER_SIZE];//���GPGGA����
u8  GpgsvBufferBusyFlag = 0;//GpggaBuffer����æ��־

u8 GPSSendBuff[GPS_SEND_BUFFER_SIZE];
//extern u8 Recorder_SendData(u8 *pData, u16 Len);dxl,2015.9,
extern u8   BlindReportFlag;//ä���ϱ���־��1ΪҪ���ϱ������ϱ���0Ϊ��Ҫ��
//****************��������*******************

/*********************************************************************
//��������  :Gps_Init(void)
//����      :GPS��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    
    memset(&USART_InitStructure,0,sizeof(USART_InitTypeDef));
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    COM_Init(COM4, &USART_InitStructure);  
    COM_NVIC_Config(COM4);                               //ʹ�ܴ����ж�
    
    SetUsartITConfig(COM4,USART_IT_RXNE,ENABLE);        //ʹ�ܴ��ڽ����ж�
    SetUsartITConfig(COM4,USART_IT_PE,DISABLE);        //��ʹ��
    SetUsartITConfig(COM4,USART_IT_TXE,DISABLE);        //��ʹ��
    SetUsartITConfig(COM4,USART_IT_TC,DISABLE);        //��ʹ��
    SetUsartITConfig(COM4,USART_IT_IDLE,DISABLE);        //��ʹ��
    SetUsartITConfig(COM4,USART_IT_LBD,DISABLE);        //��ʹ��
    SetUsartITConfig(COM4,USART_IT_ERR,DISABLE);        //��ʹ��
    
    GpioOutInit(GPS_CTR);                               //��ʼGPS��Դ���ƽ�
    
    GpioOutOff(GPS_CTR);                                //��ʼ�ر�GPS��Դ
    GpioOutOn(GPS_CTR);                                //��ʼ����GPS��Դ
    
    //���÷��ͻ�����
    Tx.pSendBuffer = GPSSendBuff;
    Tx.SendBufferMaxLen = GPS_SEND_BUFFER_SIZE;
}
/*********************************************************************
//��������  :GPS_RxIsr(void)
//����      :GPS�����ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_RxIsr(void)
{
    u8  temp;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//GPSģ����CPUͨ���Ǵ���4,���ռĴ����Ƿ�������
    {
        if(GpsRxCount >= GPS_RX_BUFFER_SIZE)//��ֹд�뵽�����洢����
        {
            GpsRxCount = 0;
        }
        temp = USART_ReceiveData(UART4);//ȡ����
        //----GNSS---
        Gnss_CommRxIsr(temp);//modify by joneming
        if('$' == temp)//��ʼ��
        {
            GpsRxCount = 0;
            GpsRxBuffer[GpsRxCount] = temp; 
            GpsRxCount++;

                
        }
        else if(0x0a == temp)//��β��
        {
            GpsRxBuffer[GpsRxCount] = temp; 
            GpsRxCount++;
                        //if(0 == BlindReportFlag)//��ä������ʱ�����Ͷ�λ��Ϣ,�б���ʱʹ�ã��Ա�ɼ�ģ�ⳡ��������
                        //{
                                  //Recorder_SendData(GpsRxBuffer, GpsRxCount);dxl,2015.9,
                        //}
            if((0 == strncmp("$GPRMC",(char const *)GpsRxBuffer,6))//GPS��ʽ
               ||(0 == strncmp("$GNRMC",(char const *)GpsRxBuffer,6))//GPS+BD��ʽ
               ||(0 == strncmp("$BDRMC",(char const *)GpsRxBuffer,6)))//BD��ʽ
            {
                if(0 == GprmcBufferBusyFlag)
                {
                     if('P' == GpsRxBuffer[2])
                        {
                            Io_WriteStatusBit(STATUS_BIT_GPS, SET);
                        Io_WriteStatusBit(STATUS_BIT_COMPASS, RESET);
                        }
                        else if('N' == GpsRxBuffer[2])
                        {
                            Io_WriteStatusBit(STATUS_BIT_GPS, SET);
                        Io_WriteStatusBit(STATUS_BIT_COMPASS, SET);
                        
                        }
                        else if('D' == GpsRxBuffer[2])
                        {
                            Io_WriteStatusBit(STATUS_BIT_GPS, RESET);
                        Io_WriteStatusBit(STATUS_BIT_COMPASS, SET);
                        }
                    Io_WriteStatusBit(STATUS_BIT_GLONASS, RESET);
                    Io_WriteStatusBit( STATUS_BIT_GALILEO, RESET);
                    //��������
                    memcpy(GprmcBuffer,GpsRxBuffer,GpsRxCount);
                    //֡������1
                    GpsRxFrameCount++;
                            //������������
                            //SetEvTask(EV_GPS_PARSE); //�б��ͼ�����,dxl,2013.6.18
                }
            }
            else if((0 == strncmp("$GPGGA",(char const *)GpsRxBuffer,6))//GPS��ʽ
                ||(0 == strncmp("$GNGGA",(char const *)GpsRxBuffer,6))//GPS+BD��ʽ
                ||(0 == strncmp("$BDGGA",(char const *)GpsRxBuffer,6)))//BD��ʽ
            {
                if(0 == GpggaBufferBusyFlag)
                {
                    //��������
                    memcpy(GpggaBuffer,GpsRxBuffer,GpsRxCount);
                    //֡������1
                    GpsRxFrameCount++;
                }
            }   
            else if((0 == strncmp("$GPGSV",(char const *)GpsRxBuffer,6))//GPS��ʽ
                ||(0 == strncmp("$GNGSV",(char const *)GpsRxBuffer,6))//GPS+BD��ʽ
                ||(0 == strncmp("$BDGSV",(char const *)GpsRxBuffer,6)))//BD��ʽ
            {
                if(0 == GpgsvBufferBusyFlag)
                {
                    //��������
                    memcpy(GpgsvBuffer,GpsRxBuffer,GpsRxCount);
                }
            }
            //----����GNSS TXT �ĵ�---
                    else if((0 == strncmp("$GPTXT",(char const *)GpsRxBuffer,6))//GPS��ʽ
                        ||(0 == strncmp("$GNTXT",(char const *)GpsRxBuffer,6))//GPS+BD��ʽ
                        ||(0 == strncmp("$BDTXT",(char const *)GpsRxBuffer,6)))//BD��ʽ
                        {
                                //  g_GnssGroup.end=true;
                        }   
            //----------------------------
            else if(0 == strncmp("$CFGSYS",(char const *)GpsRxBuffer,7))//GPS��ʽ
            {
                if(temp == 0)
                {
                    temp = 0;
                }
            }
        }
        else//�м��ַ�
        {
            GpsRxBuffer[GpsRxCount] = temp; 
            GpsRxCount++;
        }

        if(GpsRxFrameCount >= 2)
        {
            //֡������0
            GpsRxFrameCount = 0;
            //������������
            SetEvTask(EV_GPS_PARSE); 
        }       
    }
    
}
/*********************************************************************
//��������  :GPS_TxIsr(void)
//����      :GPS�����ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_TxIsr(void)
{
    u8  ch;

    if(Tx.SendCount < Tx.SendLen)//��������
    {
        
        if(Tx.SendCount < Tx.SendBufferMaxLen)
        {
            ch = Tx.pSendBuffer[Tx.SendCount];
            UsartSendData(COM4, ch);
            Tx.SendCount++;
        }
        else
        {
            Tx.SendCount = 0;
            Tx.SendLen = 0;
            SetUsartITConfig(COM4, USART_IT_TXE, DISABLE);
        }
    }
    else//������0���رմ���
    {
        Tx.SendCount = 0;
        Tx.SendLen = 0;
        SetUsartITConfig(COM4,  USART_IT_TXE, DISABLE);
    }   
}
/*********************************************************************
//��������  :GPS_ReadGPRMC(u8 *pBuffer)
//����      :��GPRMCԭʼ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Gps_ReadGPRMC(u8 *pBuffer)
{
    u8  i;
    u8  temp;
    
    for(i=0; i<GPRMC_BUFFER_SIZE; i++)
    {
         temp = GprmcBuffer[i];
         *(pBuffer+i) = temp;
        if(0x0a == temp)
        {
            return (i+1);
        }
    }
    return 100;
}
/*********************************************************************
//��������  :GPS_ReadGPGSV(u8 *pBuffer)
//����      :��GPGSVԭʼ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Gps_ReadGPGSV(u8 *pBuffer)
{
    u8  i;
    u8  temp;
    
    for(i=0; i<GPGSV_BUFFER_SIZE; i++)
    {
        temp = GpgsvBuffer[i];
        *(pBuffer+i) = temp;
        if(0x0a == temp)
        {
            return (i+1);
        }
    }
    return 100;
}
/*********************************************************************
//��������  :GPS_SendData
//����      :GPS��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :�ɹ�����ACK_OK��ʧ�ܷ���ACK_ERROR
//��ע      :
*********************************************************************/
ProtocolACK  Gps_SendData(u8 *pBuffer, u16 BufferLen)
{
    //��鴮���Ƿ����ڷ�������
    if(0 == Tx.SendCount)//û�з�������
    {
        if(BufferLen > Tx.SendBufferMaxLen) 
        {
            return ACK_ERROR;
        }
        else
        {
            memcpy(Tx.pSendBuffer,pBuffer,BufferLen);
            Tx.SendLen = BufferLen;
            SetUsartITConfig(COM4, USART_IT_TXE, ENABLE);
            return ACK_OK;
        }
    }
    else//���ڷ�������
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :GPS_putc
//����      :����һ���ַ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void GPS_putc(int ch)
{
  while((EVAL_COM4->SR & USART_FLAG_TC) == RESET);
  EVAL_COM4->DR = (u8)ch;
}

/*********************************************************************
//��������  :GPS_putstr
//����      :����һ���ַ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void GPS_putstr(char* str)
{
    for(; *str; str++)
    {
        GPS_putc(*str);
    }
}
