#ifndef	VDR_USART_H
#define	VDR_USART_H

#include "stm32f10x.h"

#define VDR_USART_TX_PIN 					GPIO_Pin_9
#define VDR_USART_RX_PIN 					GPIO_Pin_10
#define VDR_USART_PORT						GPIOA
#define VDR_USART									USART1
#define VDR_USART_BAUDRATE				115200

#define VDR_USART_RX_BUFFER_SIZE	1024
#define VDR_USART_TX_BUFFER_SIZE	1024

typedef struct
{
	u8	TxBuffer[VDR_USART_TX_BUFFER_SIZE];
	u16	TxLen;
	u16	TxCount;
	
	u8	RxBuffer[VDR_USART_RX_BUFFER_SIZE];
	u16	RxCount;
	u32	RxIdleCount;
	
}VDR_USART_STRUCT;

FunctionalState VDRUsart_TimeTask(void);
void VDRUsart_Init(void);
void VDRUsart_Isr(void);
u16  VDRUsart_SendData(u8 *pBuffer, u16 BufferLen);

#endif
