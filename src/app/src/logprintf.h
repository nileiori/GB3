/********************************************************************
//��Ȩ˵��	:
//�ļ�����	:logprintf.h	
//����		:��־��ӡ����
//�汾��	:
//������	:yjb
//����ʱ��	:2012.6
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
***********************************************************************/
#include "Usart_drive.h"
#ifndef __LOGPRINTF_H
#define __LOGPRINTF_H

#ifdef __cplusplus
 extern "C" {
#endif 
#include    <stm32f10x.h>
   
//****************�궨��********************

   
#if(LOG_TYPE == 0)   
#define LOG_PR(fmt, args ...)
#define LOG_PR_N(fmt)   
#define LOG_DB(desc,info,ilen) 
#define LOG_DB2(desc,dlen,info,ilen)  
#define LOG_B(info,ilen)   
#endif

#if(LOG_TYPE == 1)   
#define LOG_PR(fmt,...) printu(fmt, __VA_ARGS__)                                     //�ɴ��������
#define LOG_PR_N(fmt)   printu(fmt)                                                  //���ɴ��������
#define LOG_DB(desc,info,ilen) printuHex((u8*)desc,strlen((char*)desc),info,ilen)    //������ϢΪ�ַ���,������ת��Ϊ16�����ַ�д��־
#define LOG_DB2(desc,dlen,info,ilen) printuHex((u8*)desc,dlen,info,ilen)             //������ϢΪһ�������ݿ�,������ת��Ϊ16�����ַ�д��־
#define LOG_B(info,ilen) printuHex((u8*)((void*)0),0,info,ilen)                      //������ת��Ϊ16�����ַ�д��־
#endif
   
//****************�ṹ����*******************

//****************�ⲿ����*******************

//****************��������*******************

//****************��������******************
void LOG_SetUsart(COM_TypeDef COM);
void printu(char* ctrl, ...);
void printuHex(u8 *desc, u16 dlen,u8* buf,u16 len);

#ifdef __cplusplus
}
#endif   

#endif /* __LOGPRINTF_H */