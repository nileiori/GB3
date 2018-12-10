/******************************************************************** 
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Camera_Driver.c        
//����      :����ͷ��������
//�汾��    :
//������    :dxl
//����ʱ��  :2012.3
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :����ͷ�������ӿ�Ϊ485���ߣ���һ���ӿڻ������������豸
***********************************************************************/
//***************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"

//***************�ⲿ����******************
extern u8 CameraStateAck[];//״̬��־��0Ϊ��ʼֵ��1ΪӦ����ȷ��2Ϊ��Ӧ�𣨳�ʱ����3ΪӦ�����
extern u8 CameraDataBuffer[];//ͼ�����ݻ��壬�洢��������ͷͼƬ���ݣ�����������ͷЭ���֡ͷ��֡β������
extern u16  CameraRxBufferLen;//����ͷ���ջ������ݳ���
extern u16 CameraDataBufferLen;//ͼ�����ݳ���
extern u8 CameraCmdAckBuffer[];//����ͷ����Ӧ�𻺳壬�洢����ͷӦ��ָ���������ȡͼ��Ӧ�𣬰���֡ͷ��֡β����
extern u8 CameraCmdAckBufferLen;//����Ӧ�𳤶�
//extern u8 CameraDataBufferBusyFlag;//æ��־��1æ��0����
extern u8  Uart1DataDownTransFlag;//����1����͸����־��
extern u16 Uart1DataDownTransCmd;//����1����͸�������е�������
extern u8  DelayBuffer[100];//��ʱ���壬�ɰ�Ҫ��ʱ���͵����ݿ��������ע����󳤶�����
extern u8  DelayBufferLen;//��ʱ�������ݳ���
extern u16     DelayAckCommand;//��ʱӦ������
//***************���ر���******************
u8  Usart3RxBuffer[USART3_RX_BUFFER_SIZE];//���ջ���
static UART_SEND_STRUCT Tx;//���ͽṹ��
u8  Usart3BusyFlag = 0;//����1æ��־��1Ϊæ��0Ϊ���У�æʱ�������ⷢ������
static u32  Usart3RxIdleCount = 0;
u16 Usart3RxCount = 0;
//****************ȫ�ֱ���*****************

//****************��������****************
/*********************************************************************
//��������  :Camera_Init
//����      :����ͷӲ����ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Camera_Init(void)
{
    GPIO_InitTypeDef            GPIO_InitStructure;


    Usart3_Init();
        
        
    //*********����ͷ��Դ��������*********
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    CAMERA_POWER_ON();
    
    //*************���շ��Ϳ�������**************
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;              
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_10);//PD10Ϊ485��д�����ߣ�Ĭ�������Ϊ����
}
/*********************************************************************
//��������  :Camera_ClrRxBuffer
//����      :����6���ջ�����0
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Usart3_ClrRxBuffer(void)
{
    u16 i;
    for(i=0; i<USART3_RX_BUFFER_SIZE; i++)
    {
        Usart3RxBuffer[i] = 0;
    }
}
/*********************************************************************
//��������  :Camera_Init
//����      :����6��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Usart3_Init(void)
{
       
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);

   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

 
  GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);


  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
    USART_InitStructure.USART_BaudRate = 115200;               //����ͷ������Ϊ115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //8λ����λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;        //1λֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;           //��У��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure); 

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  

    USART_Cmd(USART3, ENABLE);                       
    
         
    
}
/*********************************************************************
//��������  :Usart3_Isr
//����      :����3�жϷ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Usart3_Isr(void)
{
  
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//�����ж�
    {
        Usart3_RxIsr();
    } 
  
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)//���Ϳ��ж�
    //if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)//��������ж�
    {
        Usart3_TxIsr();
    }
        

    
}
/*********************************************************************
//��������  :Usart3_RxIsr
//����      :����3�����ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Usart3_RxIsr(void)
{
    u8 ch;
    
    Usart3RxIdleCount = 0;
    
    ch = USART_ReceiveData(USART3);
    Usart3RxBuffer[Usart3RxCount] = ch;
    Usart3RxCount++;
    if(Usart3RxCount >= USART3_RX_BUFFER_SIZE)
    {
        Usart3RxCount = 0;
    }
}
/*********************************************************************
//��������  :Usart3_TxIsr
//����      :����3�����ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Usart3_TxIsr(void)
{
    u8  ch;
    u32 i;
    
    if(Tx.SendCount < Tx.SendLen)//��������
    {
        
        if(Tx.SendCount < USART3_TX_BUFFER_SIZE)
        {
            ch = Tx.SendBuffer[Tx.SendCount];
            USART_SendData(USART3, ch);
            Tx.SendCount++;
        }
        else
        {
            Tx.SendCount = 0;
            Tx.SendLen = 0;
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);//���Ϳ��жϷ���
            CAMERA_485RX_EN();//Ĭ�������485Ϊ����
      for(i=0; i<200; i++)
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //ʹ�ܽ����ж�
      //USART_ITConfig(USART3, USART_IT_TC, DISABLE);//��������жϷ���
        }
    }
    else//������0���رմ���
    {
        Tx.SendCount = 0;
        Tx.SendLen = 0;
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);//���Ϳ��жϷ���
        CAMERA_485RX_EN();//Ĭ�������485Ϊ����
    for(i=0; i<200; i++)
    ///USART_ITConfig(USART3, USART_IT_TC, DISABLE);//��������жϷ���
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //ʹ�ܽ����ж�
    }   
}
/*********************************************************************
//��������  :Usart3_SendData
//����      :����3��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :�ɹ�����ACK_OK��ʧ�ܷ���ACK_ERROR
//��ע      :
*********************************************************************/
ProtocolACK  Usart3_SendData(u8 *pBuffer, u16 BufferLen)
{
    u32 i;
    //��鴮���Ƿ����ڷ�������
    if(0 == Tx.SendCount)//û�з�������
    {
        if(BufferLen > USART3_TX_BUFFER_SIZE)   
        {
            return ACK_ERROR;
        }
        else
        {
            if(0 == Usart3BusyFlag)
            {
                Usart3BusyFlag = 1;//��λæ��־
                Usart3RxIdleCount = 0;
                memcpy(Tx.SendBuffer,pBuffer,BufferLen);
                Tx.SendLen = BufferLen;
        Tx.SendBuffer[BufferLen] = 0;//�����һ���ֽ�����һ��0���������һ���ֽ��ܷ��ͳ�ȥ,dxl,2014.3.17
        Tx.SendLen++;
                CAMERA_485TX_EN();//����485Ϊ����
                for(i=0; i<200; i++)//��ʱ����Ҫ��Ϊ���ȶ�485���Ƶ�ƽ
                {

                }
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); //���͹����н�ֹ���գ���Ϊ485�ǰ�˫��ͨ�ţ�
                USART_ITConfig(USART3, USART_IT_TXE, ENABLE);//���Ϳ��жϷ���
                //USART_SendData(USART3,0xff);����ʹ��
        //USART_SendData(USART3,0x00);
                //USART_ITConfig(USART3, USART_IT_TC, ENABLE);
                
                return ACK_OK;
            }
            else
            {
                return ACK_ERROR;
            }
        }
    }
    else//���ڷ�������
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :Usart3_TimeTask
//����      :����3��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :100�������1��
*********************************************************************/
FunctionalState Usart3_TimeTask(void) 
{
    u16 i;
    if(1 == Usart3BusyFlag)
    {
        Usart3RxIdleCount++;
        if(1 == Usart3_CameraProtocolParse())
        {
            Usart3RxIdleCount = 0;
            Usart3BusyFlag = 0;
                  Usart3RxCount = 0;//���ռ�����0
            for(i=0; i<USART3_RX_BUFFER_SIZE; i++)
            {
                Usart3RxBuffer[i] = 0;
            }
        }
        else
        {
            if(Usart3RxIdleCount >= 5)//500ms����Ӧ��ǿ�����æ��־
            {
                Usart3BusyFlag = 0;
                Usart3RxCount = 0;//���ռ�����0
                for(i=0; i<USART3_RX_BUFFER_SIZE; i++)
                {
                    Usart3RxBuffer[i] = 0;
                }
            }
        }
    }
    else
    {
        Usart3RxIdleCount = 0;
    }
    return ENABLE;
}
/*********************************************************************
//��������  :Usart3_OilProtocolParse
//����      :��ͨ�ͺ�Э�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
/*,dxl,2014.5.12����,�ͺ���32PIN 485�ӿڸĵ�16PIN 232�ӿ�
u8 Usart3_OilProtocolParse(void)
{
    u16 i;
    u16 j;
    u8  flag = 0;
    u8  HighByte;
    u8  LowByte;
    u8  sum;
    
    for(i=0; i<Usart3RxCount; i++)
    {
        if('@' == Usart3RxBuffer[i])
        {
                break;
        }
    }
    for(j=i; j<Usart3RxCount; j++)
    {
        if('#' == Usart3RxBuffer[j])
        {
            flag = 1;
            break;
        }
    }
    if(1 == flag)
    {
        sum = 0;
        for(j=i+1;j<Usart3RxCount-3;j++)
        {
            sum += Usart3RxBuffer[j];
        }
        
        HighByte = Usart3RxBuffer[Usart3RxCount-3];
        if((HighByte >= '0')&&(HighByte <= '9'))
        {
            HighByte -= '0';
        }
        else if((HighByte >= 'A')&&(HighByte <= 'F'))
        {
            HighByte -= 55;
        }
        else if((HighByte >= 'a')&&(HighByte <= 'f'))
        {
            HighByte -= 87;
        }
        else
        {
            flag = 0;//�Ƿ��ַ�
        }
            
        LowByte = Usart3RxBuffer[Usart3RxCount-2];
        if((LowByte >= '0')&&(LowByte <= '9'))
        {
            LowByte -= '0';
        }
        else if((LowByte >= 'A')&&(LowByte <= 'F'))
        {
            LowByte -= 55;
        }
        else if((LowByte >= 'a')&&(LowByte <= 'f'))
        {
            LowByte -= 87;
        }
        else
        {
            flag = 0;//�Ƿ��ַ�
        }
            
        if((sum == ((HighByte<<4)|LowByte))&&(1 == flag))
        {
            //���͵�ƽ̨
            if((Uart1DataDownTransCmd == *(Usart3RxBuffer+i+3))&&(1 == Uart1DataDownTransFlag))
            {
                Uart1DataDownTransFlag = 0;
                DelayBuffer[0] = 1;
                memcpy(DelayBuffer+1,Usart3RxBuffer+i,Usart3RxCount-i);
                DelayBufferLen = Usart3RxCount-i+1;
                DelayAckCommand = 13;//��һ����Ӧ���͵�ƽ̨
                SetTimerTask(TIME_DELAY_TRIG, 1*SECOND);//������ʱ����
            }
            JointechOiLCost_CopyBuf(Usart3RxBuffer+i,Usart3RxCount-i); 
            
        }
        else
        {
            flag = 0;
        }
    }
    return flag;
    
}
*/
/*********************************************************************
//��������  :Usart3_CameraProtocolParse
//����      :����ͷЭ�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :����0��ʾ��������ͷЭ�飬����1��ʾ������ͷЭ��
//��ע      :
*********************************************************************/
u8 Usart3_CameraProtocolParse(void)
{
    u16 i;
    u16 j;
    u16 length;
    u8  flag = 0;
    u8  sum;
    //��������
    for(i=0; i<Usart3RxCount; i++)
    {
        if((0x40 == Usart3RxBuffer[i])&&(0x40 == Usart3RxBuffer[i+1]))
        {
             break;
        }
    }
    if(Usart3RxCount != i)//�ҵ�����ͷָ���֡ͷ
    {
         for(j=i; j<Usart3RxCount; j++)
         {
             if((0x0d == Usart3RxBuffer[j])&&(0x0a == Usart3RxBuffer[j+1]))
             {
                  j++;
                  flag = 1;//ȷ��Ϊ����ͷָ��
                  break;
              }
         }
     }
     if(1 == flag)//����ͷָ��
     {
        if(0x64 == Usart3RxBuffer[i+2])//����Ӧ��
        {
            CameraCmdAckBufferLen = Usart3RxCount-i;
      if(CameraCmdAckBufferLen > 12)
      {
        CameraCmdAckBufferLen = 12;
      }
            memcpy(CameraCmdAckBuffer,Usart3RxBuffer+i,CameraCmdAckBufferLen);
      CameraStateAck[CAMERA_STATE_PHOTO] = 1;
        }
        else if(0x6a == Usart3RxBuffer[i+2])//����ָ��
        {
                        CameraCmdAckBufferLen = Usart3RxCount-i;
                        if(CameraCmdAckBufferLen > 12)
                        {
                                CameraCmdAckBufferLen = 12;
                        }
            memcpy(CameraCmdAckBuffer,Usart3RxBuffer+i,CameraCmdAckBufferLen);
                        CameraStateAck[CAMERA_STATE_CHECK] = 1;
        }
        else if(0x63 == Usart3RxBuffer[i+2])//ȡͼӦ��
        {
            //��鳤��
            length = 0;
            length |= Usart3RxBuffer[i+6] << 8;
            length |= Usart3RxBuffer[i+5];
            if(length == Usart3RxCount-i-11)//������ȷ
            {
                sum = 0;
                for(j=0; j<length; j++)
                {
                    sum += Usart3RxBuffer[i+8+j];
                }
                if(sum == Usart3RxBuffer[i+8+j])
                {
                    //if(0 == CameraDataBufferBusyFlag)
                    //{
                        CameraDataBufferLen = length;
                                        if(CameraDataBufferLen > 512)
                                        {
                                                CameraDataBufferLen =  512;
                                        }
                        memcpy(CameraDataBuffer,Usart3RxBuffer+i+8,CameraDataBufferLen);//ֻȡͼ������
                                        CameraStateAck[CAMERA_STATE_FETCH] = 1;
                    //}
                    //else
                    //{
                       // CameraStateAck[CAMERA_STATE_FETCH] = 3;
                       // flag = 0;
                   // }
                }
                else
                {
                    CameraStateAck[CAMERA_STATE_FETCH] = 3;
                    flag = 0;
                }
            }
            else
            {
                CameraStateAck[CAMERA_STATE_FETCH] = 3;
                flag = 0;
            }
        }
    }
    return flag;
}