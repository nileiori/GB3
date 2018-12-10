/********************************************************************
//��Ȩ˵��	:
//�ļ�����	:Usart_drive.c		
//����		:USART��������
//�汾��	:
//������	:yjb
//����ʱ��	:2012.6
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
//              1.COM1:
//               TX1->PA9
//               RX1->PA10
//             2.COM2:
//               TX2->PA3
//               RX2->PA2
//
//
***********************************************************************/
#include "my_typedef.h"
#include "stm32f10x.h"
#ifndef __STM32F2xx_USART_DRIVE_H
#define __STM32F2xx_USART_DRIVE_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
//****************�궨��********************
   
/* ���崮�ڸ���*/     
#define COMn                             6

/* STM32F2xxһ����6������*/   
typedef enum 
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2,
  COM4 = 3,
  COM5 = 4,
  COM6 = 5,
  COM_MAX,
} COM_TypeDef;

#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_TX_SOURCE              GPIO_PinSource9
#define EVAL_COM1_TX_AF                  GPIO_AF_USART1
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_SOURCE              GPIO_PinSource10
#define EVAL_COM1_RX_AF                  GPIO_AF_USART1
#define EVAL_COM1_IRQn                   USART1_IRQn

#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_TX_SOURCE              GPIO_PinSource2
#define EVAL_COM2_TX_AF                  GPIO_AF_USART2
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_SOURCE              GPIO_PinSource3
#define EVAL_COM2_RX_AF                  GPIO_AF_USART2
#define EVAL_COM2_IRQn                   USART2_IRQn

#define EVAL_COM3                        USART3
#define EVAL_COM3_CLK                    RCC_APB1Periph_USART3
#define EVAL_COM3_TX_PIN                 GPIO_Pin_8
#define EVAL_COM3_TX_GPIO_PORT           GPIOD
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM3_TX_SOURCE              GPIO_PinSource8
#define EVAL_COM3_TX_AF                  GPIO_AF_USART3
#define EVAL_COM3_RX_PIN                 GPIO_Pin_9
#define EVAL_COM3_RX_GPIO_PORT           GPIOD
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM3_RX_SOURCE              GPIO_PinSource9
#define EVAL_COM3_RX_AF                  GPIO_AF_USART3
#define EVAL_COM3_IRQn                   USART3_IRQn

#define EVAL_COM4                        UART4
#define EVAL_COM4_CLK                    RCC_APB1Periph_UART4
#define EVAL_COM4_TX_PIN                 GPIO_Pin_0
#define EVAL_COM4_TX_GPIO_PORT           GPIOA
#define EVAL_COM4_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM4_TX_SOURCE              GPIO_PinSource0
#define EVAL_COM4_TX_AF                  GPIO_AF_UART4
#define EVAL_COM4_RX_PIN                 GPIO_Pin_1
#define EVAL_COM4_RX_GPIO_PORT           GPIOA
#define EVAL_COM4_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM4_RX_SOURCE              GPIO_PinSource1
#define EVAL_COM4_RX_AF                  GPIO_AF_UART4
#define EVAL_COM4_IRQn                   UART4_IRQn

#define EVAL_COM5                        UART5
#define EVAL_COM5_CLK                    RCC_APB1Periph_UART5
#define EVAL_COM5_TX_PIN                 GPIO_Pin_12
#define EVAL_COM5_TX_GPIO_PORT           GPIOC
#define EVAL_COM5_TX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM5_TX_SOURCE              GPIO_PinSource12
#define EVAL_COM5_TX_AF                  GPIO_AF_UART5
#define EVAL_COM5_RX_PIN                 GPIO_Pin_2
#define EVAL_COM5_RX_GPIO_PORT           GPIOD
#define EVAL_COM5_RX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM5_RX_SOURCE              GPIO_PinSource2
#define EVAL_COM5_RX_AF                  GPIO_AF_UART5
#define EVAL_COM5_IRQn                   UART5_IRQn

#define EVAL_COM6                        USART6
//#define EVAL_COM6_CLK                    RCC_APB2Periph_USART6
#define EVAL_COM6_TX_PIN                 GPIO_Pin_14
#define EVAL_COM6_TX_GPIO_PORT           GPIOG
#define EVAL_COM6_TX_GPIO_CLK            RCC_APB2Periph_GPIOG
#define EVAL_COM6_TX_SOURCE              GPIO_PinSource14
//#define EVAL_COM6_TX_AF                  GPIO_AF_USART6
#define EVAL_COM6_RX_PIN                 GPIO_Pin_9
#define EVAL_COM6_RX_GPIO_PORT           GPIOG
#define EVAL_COM6_RX_GPIO_CLK            RCC_APB2Periph_GPIOG
#define EVAL_COM6_RX_SOURCE              GPIO_PinSource9
//#define EVAL_COM6_RX_AF                  GPIO_AF_USART6
//#define EVAL_COM6_IRQn                   USART6_IRQn

#define UART_SEND_BUFFER_SIZE           256

//****************�ṹ����*******************
//�������ݷ��ͽṹ
typedef struct
{
         u8* pSendBuffer;                    //���ͻ�����ָ��
         u16 SendBufferMaxLen;               //���ͻ��������ֵ
         u16 SendLen;                        //�����ܳ���
         u16 SendCount;                      //�ѷ��ͳ���
}stUartSend;

//�������ݽ��սṹ
typedef struct
{
         u8* pRecvBuffer;                    //���ջ�����ָ��
         u16 RecvBufferMaxLen;               //���ջ��������ֵ
         u16 RecvLen;                        //�ѽ������ݳ���
         u8  RecvFullFlag;                   //���ջ���������־,0-δ��,1-����
         u8  RecvInitFlag;                   //���ڳ�ʼ�����յ�һ���ַ���־��0-δ�����κ��ַ���1-�ѽ��յ�һ���ַ�
         u16 ReadCount;                      //�Ѷ�ȡ���ݳ���
}stUartRecv;

//****************�ⲿ����*******************
extern USART_TypeDef* COM_USART[];
//****************��������*******************
void COM_Init(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
void COM_NVIC_Config(COM_TypeDef COM);  
u8 GetUsartIrq(COM_TypeDef COM);
ITStatus GetUsartStatus(COM_TypeDef COM, uint16_t USART_IT);
void ClearUsartStatus(COM_TypeDef COM, uint16_t USART_IT);
u8 GetUsartReceiveData(COM_TypeDef COM);
void SetUsartITConfig(COM_TypeDef COM, uint16_t USART_IT, FunctionalState NewState);
void UsartSendData(COM_TypeDef COM, u8 Data);
USART_TypeDef* GetUsartPtr(COM_TypeDef COM);

void SetUsartSendBuff(u8 buff,u16 maxlen);

#ifdef __cplusplus
}
#endif   

#endif /* __STM32F2xx_USART_DRIVE_H */