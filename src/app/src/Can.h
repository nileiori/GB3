/*************************************************************************
*
* Copyright (c) 2008,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : HandleCan.h
* ����     : STM32��CAN���ߵ�����.
*
* ��ǰ�汾 :
* ������   :
* �޸�ʱ�� :
*
* �����汾 : 1.0
* ������   : zhulin
* ����ʱ�� : 2009��4��25��
*
* ��ע     :
*************************************************************************/
#ifndef _CANDRIVER_H_
#define _CANDRIVER_H_

#include "stm32f10x.h"

//---------- ����ΪBASICCAN SJA1000�Ĵ����� ----------
#define     CAN_INTERFACE_0K    0                            //SJA1000�ӿ�����
//#define     CAN_BUS_0K          0                          //CAN���߹�������
#define     CAN_INTERFACE_ERR   0XFF                         //SJA1000�ӿڴ���?
#define     CAN_QUITRESET_ERR   0XFD                         //SJA1000�����˳���λģʽ
#define     CAN_INITOBJECT_ERR  0XFC                         //��ʼ�����������˲�����
#define     CAN_INITBTR_ERR     0XFB                         //��ʼ�����߶�ʱ������
#define     CAN_INITOUTCTL_ERR  0XFA                         //��ʼ��������ƴ���
#define     CAN_INITSET_ERR     0XF9                         //��ʼ��ʱ�ӷ�Ƶ��������
//#define     CAN_BUS_ERR         0XF8                         //SJA1000�������ݴ�

#define CAN_PWR_ON()    GPIO_WriteBit(GPIOD, GPIO_Pin_15, Bit_SET)
#define CAN_PWR_OFF()   GPIO_WriteBit(GPIOD, GPIO_Pin_15, Bit_RESET)
//#define     NO_BANDRATE_SIZE    0xe7

#define  CAN_SEND_OK    0
#define  CAN_SEND_ERR   1




//==========================================================================

void CanHwInit(void);
void Can_Isr_Rx(void);
void Can_Isr_Tx(void);
u8 CanBus_Send(u8 *dataAddr, u32 dataLen);
void CanHwInit2(void);
void Can_Isr_Rx2(void);
void Can_Isr_Tx2(void);
u8 CanBus_Send2(u8 *dataAddr, u32 dataLen);
u8 CanBus_GetData(u8 *dataAddr);

//==========================================================================

#endif
