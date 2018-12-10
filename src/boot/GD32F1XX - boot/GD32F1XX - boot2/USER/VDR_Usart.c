#include "Vdr_Usart.h"
#include "VDR_Protocol.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include <string.h>

VDR_USART_STRUCT	VdrUsart;

static void VDRUsart_RxIsr(void);
static void VDRUsart_TxIsr(void);

FunctionalState VDRUsart_TimeTask(void) 
{
	VdrUsart.RxIdleCount++;
	if((VdrUsart.RxIdleCount >= 3)&&(0 != VdrUsart.RxCount))
	{
		VDRProtocol_Parse(VdrUsart.RxBuffer, VdrUsart.RxCount);
//		VDRUsart_SendData(VdrUsart.RxBuffer, VdrUsart.RxCount);
		VdrUsart.RxIdleCount = 0;
		VdrUsart.RxCount = 0;
	}
	return ENABLE;
}

void VDRUsart_Init(void)
{
		GPIO_InitTypeDef    GPIO_InitStructure;
  	USART_InitTypeDef   USART_InitStructure;
  
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1, ENABLE);
	
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
	u8	ch;

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
