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
#ifndef _USART1_H
#define _USART1_H

#ifdef __cplusplus
 extern "C" {
#endif 

//****************�궨��********************

//****************�ⲿ����*******************   

//****************ȫ�ֱ���*******************

//****************��������*******************     
extern void COM1_Init(unsigned int baud_rate);
void COM1_putc(int ch);
void COM1_putstr(char* str);
unsigned char  COM1_WriteBuff(unsigned char *buff, unsigned short len);
unsigned short COM1_ReadBuff(unsigned char *buff,unsigned short len);
void COM1_RxIsr(void);
void COM1_TxIsr(void);

#ifdef __cplusplus
}
#endif   

#endif /* _USART1_H */