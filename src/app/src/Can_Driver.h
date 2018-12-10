#ifndef __CAN_DRIVER_H
#define __CAN_DRIVER_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"



#define CAN_RX_BUFFER_SIZE	1200//���ɴ洢1000/CAN_STEP_LEN��CAN����
#define CAN_STEP_LEN_MAX	  25//��󲽳���ÿ��CAN���ݰ���֡ͷ1�ֽ�+5�ֽ�ʱ�䣨ʱ���룬����2�ֽڣ�Ĭ��Ϊ0��+4�ֽ�ID+8�ֽ�����+֡β1�ֽ�+ת�������ֽ�
#define CAN_SOFTWARE_MASK_NUM   8//����˳�ID������


#define CAN_1_CLK		RCC_AHB1Periph_GPIOD//CAN1�ӿ�ģ��
#define CAN_1_PORT		GPIOD
#define CAN_1_RX_PIN		GPIO_Pin_0
#define CAN_1_TX_PIN		GPIO_Pin_1
#define CAN_1_RX_PIN_SOURCE	GPIO_PinSource0
#define CAN_1_TX_PIN_SOURCE	GPIO_PinSource1

#define CAN_1_POWER_CLK		RCC_AHB1Periph_GPIOD//CAN1��������
#define CAN_1_POWER_PORT	GPIOD
#define CAN_1_POWER_PIN		GPIO_Pin_15
#define CAN_1_POWER_ON()	GPIO_WriteBit(CAN_1_POWER_PORT,CAN_1_POWER_PIN,Bit_SET)
#define CAN_1_POWER_OFF()	GPIO_WriteBit(CAN_1_POWER_PORT,CAN_1_POWER_PIN,Bit_RESET)

#define CAN_1_BAUDRATE		250000

/**************************************************************************
//��������Can_Init
//���ܣ�ʵ��CAN�ӿڳ�ʼ��
//���룺��
//�������
//����ֵ����
//��ע������CAN1��CAN2�ĳ�ʼ������������CAN2û��ʹ�ã�����ֻ��CAN1�ģ�
//�ú�����Ҫ��CANӦ����������ǰ��һ����CANӦ������͵���
***************************************************************************/
void Can_Init(void);
/**************************************************************************
//��������Can_1_Rx_Isr
//���ܣ�ʵ��CAN1�����жϴ���
//���룺��
//�������
//����ֵ����
//��ע���жϷ���������ô˺���
***************************************************************************/
void Can_1_Rx_Isr(void);
void CAN_1_ID_Filter(u8 FilterNum, u32 FilterId, u32 FilterMaskID, u8 IdType);

#endif