/********************************************************************
//��Ȩ˵��	:
//�ļ�����	:Usart1.c		
//����		:USART1��������,����Debug�����
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
#ifndef _USART3_H
#define _USART3_H

#ifdef __cplusplus
 extern "C" {
#endif 

//****************�궨��********************
//****************�ⲿ����*******************   

//****************ȫ�ֱ���*******************

//****************��������*******************     
void COM3_Init(u32 baud_rate);
void COM3_putc(int ch);
void COM3_putstr(char* str);
u8  COM3_WriteBuff(u8 *buff, u16 len);
u16 COM3_ReadBuff(u8 *buff,u16 len);
void COM3_RxIsr(void);
void COM3_TxIsr(void);

#ifdef __cplusplus
}
#endif   

#endif /* _USART3_H */