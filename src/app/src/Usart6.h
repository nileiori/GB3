/********************************************************************
//��Ȩ˵��	:
//�ļ�����	:Usart6.c		
//����		:USART6��������,����IC��д
//�汾��	:
//������	:yjb
//����ʱ��	:2012.6
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
//
***********************************************************************/
#include "my_typedef.h"

#ifndef _USART6_H
#define _USART6_H

#ifdef __cplusplus
 extern "C" {
#endif 

//****************�궨��********************
#define COM6_TX_MAX_LEN   512                                          //���ջ���������
#define COM6_RX_MAX_LEN   512                                          //���ջ���������

   
//****************�ⲿ����*******************   

//****************ȫ�ֱ���*******************

//****************��������*******************     
void COM6_Init(u32 baud_rate);
void COM6_putc(int ch);
void COM6_putstr(char* str);
u8  COM6_WriteBuff(u8 *buff, u16 len);
u16 COM6_ReadBuff(u8 *buff,u16 len);
void COM6_RxIsr(void);
void COM6_TxIsr(void);

#ifdef __cplusplus
}
#endif   

#endif /* _USART6_H */