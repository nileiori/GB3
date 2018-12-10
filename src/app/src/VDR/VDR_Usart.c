#include "Vdr_Usart.h"
#include "VDR_Protocol.h"
//#include "stm32f2xx.h"
//#include "stm32f2xx_usart.h"
//#include "stm32f2xx_gpio.h"
//#include "stm32f2xx_rcc.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "VDR_Log.h"
#include "modem_debug.h"
//#include "Carload.h"
#include <string.h>
#include "include.h"

VDR_USART_STRUCT    VdrUsart;

u8  VdrTxBuffer[VDR_TX_Q_BUFFER_SIZE];//VDR发送队列缓冲
Queue  VdrTxQ;//VDR发送队列

u8  VdrTxTempBuffer[VDR_TX_TEMP_BUFFER_SIZE];

static void VDRUsart_RxIsr(void);
static void VDRUsart_TxIsr(void);

FunctionalState VDRUsart_TimeTask(void) 
{
	  u16 ReadLen;
	
    VdrUsart.RxIdleCount++;
    if((VdrUsart.RxIdleCount >= 3)&&(0 != VdrUsart.RxCount))
    {
        VDRLog_Write(VdrUsart.RxBuffer, VdrUsart.RxCount);
        VDRProtocol_Parse(VdrUsart.RxBuffer, VdrUsart.RxCount);
        
        Modem_Debug_UartRx(VdrUsart.RxBuffer, VdrUsart.RxCount);
                
        VdrUsart.RxIdleCount = 0;
        VdrUsart.RxCount = 0;
    }
		
		if((0 == VdrUsart.TxCount)&&(0 == VdrUsart.TxLen))//仅仅是针对通信模块数据转发至DB9串口的情况
		{
		    ReadLen = QueueOutBuffer(VdrTxTempBuffer, VDR_TX_TEMP_BUFFER_SIZE, VdrTxQ);
			  if(ReadLen > 0)
				{
			      VDRUsart_SendData(VdrTxTempBuffer, ReadLen);
				}
		}
		
		
    return ENABLE;
}

void VDRUsart_Init(void)
{
  // dxl,2015.8,以下是针对STM32F105的初始化代码
      GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
      NVIC_InitTypeDef    NVIC_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = VDR_USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = VDR_USART_TX_PIN;          
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(VDR_USART_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = VDR_USART_RX_PIN;                   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(VDR_USART_PORT, &GPIO_InitStructure);
    
    USART_InitStructure.USART_BaudRate = VDR_USART_BAUDRATE;                
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;    
    //USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;         
    USART_InitStructure.USART_Parity = USART_Parity_Odd ;          
    //USART_InitStructure.USART_Parity = USART_Parity_No; 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(VDR_USART, &USART_InitStructure);         
  
    USART_ITConfig(VDR_USART, USART_IT_RXNE, ENABLE);    
    USART_Cmd(VDR_USART, ENABLE);  
  
    /* 以下是STM32F205的代码
        GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = VDR_USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

        RCC_APB2PeriphClockCmd(VDR_USART_RCC, ENABLE);

    GPIO_PinAFConfig(VDR_USART_PORT, VDR_USART_TX_PinSource, VDR_USART_AF);

    GPIO_PinAFConfig(VDR_USART_PORT, VDR_USART_RX_PinSource, VDR_USART_AF);

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

    GPIO_InitStructure.GPIO_Pin = VDR_USART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(VDR_USART_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = VDR_USART_RX_PIN;
    GPIO_Init(VDR_USART_PORT, &GPIO_InitStructure);
  
    USART_InitStructure.USART_BaudRate = 115200;               //摄像头波特率为115200
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;   //8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;        //1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_Odd;           //无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(VDR_USART, &USART_InitStructure);         //配置串口3

    USART_Cmd(VDR_USART, ENABLE);                        //使能USART3
    
    USART_ITConfig(VDR_USART, USART_IT_RXNE, ENABLE);    //使能发送空中断和接收空中断 
*/  
}   

void VDRUsart_Isr(void)
{
    if(USART_GetITStatus(VDR_USART, USART_IT_RXNE) != RESET)
    {
        VDRUsart_RxIsr();
    } 
  
    if(USART_GetITStatus(VDR_USART, USART_IT_TXE) != RESET)
    {
        VDRUsart_TxIsr();
    }
		
		if(USART_GetITStatus(VDR_USART, USART_IT_TC) != RESET)
    {
        USART_ITConfig(VDR_USART, USART_IT_TC, DISABLE);
    }
}

u16  VDRUsart_SendData(u8 *pBuffer, u16 BufferLen)
{

    if(0 == VdrUsart.TxCount)
    {
        if(BufferLen > VDR_USART_TX_BUFFER_SIZE)    
        {
            return 0;
        }
        else
        {
            memcpy(VdrUsart.TxBuffer,pBuffer,BufferLen);
            VdrUsart.TxLen = BufferLen;
            USART_ITConfig(VDR_USART, USART_IT_TXE, ENABLE);
            return BufferLen;
        }
    }
    else
    {
        return 0;
    }
}

static void VDRUsart_TxIsr(void)
{
    u8  ch;

    if(VdrUsart.TxCount < VdrUsart.TxLen)
    {
        
        if(VdrUsart.TxCount < VDR_USART_TX_BUFFER_SIZE)
        {
            ch = VdrUsart.TxBuffer[VdrUsart.TxCount];
            USART_SendData(VDR_USART, ch);
            VdrUsart.TxCount++;
        }
        else
        {
            VdrUsart.TxCount = 0;
            VdrUsart.TxLen = 0;
            USART_ITConfig(VDR_USART, USART_IT_TXE, DISABLE);
        }
    }
    else
    {
        VdrUsart.TxCount = 0;
        VdrUsart.TxLen = 0;
        USART_ITConfig(VDR_USART, USART_IT_TXE, DISABLE);
    }   
}   

static void VDRUsart_RxIsr(void)
{
    u8 ch;
    
    VdrUsart.RxIdleCount = 0;
    
    ch = USART_ReceiveData(VDR_USART);
    VdrUsart.RxBuffer[VdrUsart.RxCount] = ch;
    VdrUsart.RxCount++;
    if(VdrUsart.RxCount >= VDR_USART_RX_BUFFER_SIZE)
    {
        VdrUsart.RxCount = 0;
    }
}
