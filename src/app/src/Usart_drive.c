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
//
***********************************************************************/
//****************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "HAL.h"

//****************�궨��********************

//****************�ⲿ����*******************

//****************ȫ�ֱ���*******************
USART_TypeDef* COM_USART[COMn] = {EVAL_COM1,
                                  EVAL_COM2,
                                  EVAL_COM3,
                                  EVAL_COM4,
                                  EVAL_COM5,
                                  }; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {
	
	                                EVAL_COM1_TX_GPIO_PORT,
                                   EVAL_COM2_TX_GPIO_PORT,
                                   EVAL_COM3_TX_GPIO_PORT,
                                   EVAL_COM4_TX_GPIO_PORT,
                                   EVAL_COM5_TX_GPIO_PORT,
                                 
};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT,
                                   EVAL_COM2_RX_GPIO_PORT,
                                   EVAL_COM3_RX_GPIO_PORT,
                                   EVAL_COM4_RX_GPIO_PORT,
                                   EVAL_COM5_RX_GPIO_PORT,
                                  };

const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK,
                                      EVAL_COM2_CLK,
                                      EVAL_COM3_CLK,
                                      EVAL_COM4_CLK,
                                      EVAL_COM5_CLK,
                                   };

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK,
                                        EVAL_COM2_TX_GPIO_CLK,
                                        EVAL_COM3_TX_GPIO_CLK,
                                        EVAL_COM4_TX_GPIO_CLK,
                                        EVAL_COM5_TX_GPIO_CLK,
                                    };
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK,
                                        EVAL_COM2_RX_GPIO_CLK,
                                        EVAL_COM3_RX_GPIO_CLK,
                                        EVAL_COM4_RX_GPIO_CLK,
                                        EVAL_COM5_RX_GPIO_CLK,
                                    };

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN,
                                   EVAL_COM2_TX_PIN,
                                   EVAL_COM3_TX_PIN,
                                   EVAL_COM4_TX_PIN,
                                   EVAL_COM5_TX_PIN,
                                  };

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN,
                                   EVAL_COM2_RX_PIN,
                                   EVAL_COM3_RX_PIN,
                                   EVAL_COM4_RX_PIN,
                                   EVAL_COM5_RX_PIN,
                                  };
 
const uint8_t COM_TX_PIN_SOURCE[COMn] = {EVAL_COM1_TX_SOURCE,
                                         EVAL_COM2_TX_SOURCE,
                                         EVAL_COM3_TX_SOURCE,
                                         EVAL_COM4_TX_SOURCE,
                                         EVAL_COM5_TX_SOURCE,
                                        };

const uint8_t COM_RX_PIN_SOURCE[COMn] = {EVAL_COM1_RX_SOURCE,
                                         EVAL_COM2_RX_SOURCE,
                                         EVAL_COM3_RX_SOURCE,
                                         EVAL_COM4_RX_SOURCE,
                                         EVAL_COM5_RX_SOURCE,
                                        };

const uint32_t COM_TX_AF[COMn] = {GPIO_Remap_USART1,
                                 GPIO_Remap_USART2,
                                 GPIO_FullRemap_USART3,
                                 PCFR6_REMAP_USART4,
                               //  EVAL_COM5_TX_AF,
                                };
 /* 
const uint8_t COM_RX_AF[COMn] = {EVAL_COM1_RX_AF,
                                 EVAL_COM2_RX_AF,
                                 EVAL_COM3_RX_AF,
                                 EVAL_COM4_RX_AF,
                                 EVAL_COM5_RX_AF,
                                };
*/
const uint8_t COM_IAR[COMn] = {EVAL_COM1_IRQn,
                               EVAL_COM2_IRQn,
                               EVAL_COM3_IRQn,
                               EVAL_COM4_IRQn,
                               EVAL_COM5_IRQn,
                               };
//****************��������*******************


//****************��������******************

/**
  * @brief  ��ʼ��COM��.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void COM_Init(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* ʹ��GPIOʱ�� */
  RCC_APB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);
  /*ʹ��USARTʱ��,Usart1��USART6��APB2�ϣ�Usart2, Usart3,Usart4,Usart5��APB1��*/
  if ((COM == COM1))
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  }
  else
  {
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

  /* ���� PXx���ŵ�USART_Tx*/

  if ((COM == COM3))
  {
      GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
  }
  else if ((COM == COM4))
  {
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO  , ENABLE);
      GPIO_PinRemapConfig2(PCFR6,PCFR6_REMAP_USART4, ENABLE);
  }

  /* ���� PXx���ŵ�USART_Rx*/
  //GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_PIN_SOURCE[COM], COM_RX_AF[COM]);

  /* Configure USART Tx as alternate function  */
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}

/**
  * @brief  �򿪴����ж�.
  * @retval None
  */
void COM_NVIC_Config(COM_TypeDef COM)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* ����USARTx�ж�*/
  NVIC_InitStructure.NVIC_IRQChannel = COM_IAR[COM];
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  ��ȡ���ڵ��жϱ��.
  * @param  COM: ���ڱ��
  * @retval None
  */
u8 GetUsartIrq(COM_TypeDef COM)
{
  return COM_IAR[COM];
}

/**
  * @brief  ��ȡָ��״̬λֵ.
  * @param  COM: ���ڱ��
  * @param  USART_IT: ��Ҫ��ȡ��״̬λ
  * @retval ��Чλ����SET,��Ч����RESET.
  */
ITStatus GetUsartStatus(COM_TypeDef COM, uint16_t USART_IT)
{
    return USART_GetITStatus(COM_USART[COM], USART_IT);
}

/**
  * @brief  �������״̬λ.
  * @param  COM: ���ڱ��
  * @param  USART_IT: �������״̬λ
  * @retval None
  */
void ClearUsartStatus(COM_TypeDef COM, uint16_t USART_IT)
{
    USART_ClearITPendingBit(COM_USART[COM], USART_IT); 
}

/**
  * @brief  ��ȡ��������.
  * @param  COM: ���ں�
  * @retval ���ػ�ȡ������
  */
u8 GetUsartReceiveData(COM_TypeDef COM)
{
    return (USART_ReceiveData(COM_USART[COM])  & 0xFF);
}
/**
  * @brief  �����ж��Ƿ����úͽ���.
  * @param  COM: ���ڱ��
  * @param  USART_IT: �ж�����
  * @param  NewState: ����ENABLE,����DISABLE
  * @retval ���ػ�ȡ������
  */
void SetUsartITConfig(COM_TypeDef COM, uint16_t USART_IT, FunctionalState NewState)
{
    USART_ITConfig(COM_USART[COM], USART_IT, NewState);
}

/**
  * @brief  �����ж��Ƿ����úͽ���.
  * @param  COM: ���ڱ��
  * @param  Data: ����������
  * @retval None
  */
void UsartSendData(COM_TypeDef COM, u8 Data)
{
  USART_SendData(COM_USART[COM], Data);
}

/**
  * @brief  ���ݴ��ڱ�Ż�ȡ����ָ��.
  * @param  COM: ���ڱ��
  * @retval ��ǰ���ָ��Ĵ���ָ��
  */
USART_TypeDef* GetUsartPtr(COM_TypeDef COM)
{
    return COM_USART[COM];
}
