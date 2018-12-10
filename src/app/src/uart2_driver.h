/*******************(C)�������������¼����������޹�˾��Ȩ����*******************

**  �ļ�����:   Isr.h
**  ��������:   ������
**  �汾���:   V1.1.0.0
**  ��������:   20100113
**  ����˵��:  	�ļ�uart2_driver.c��ͷ�ļ�
**  �޸ļ�¼:

**  ��    ע:
*******************************************************************************/
#ifndef _UART2_DRIVER_H_
#define _UART2_DRIVER_H_

#include "stm32f10x.h"

//#define UART2_REC_MAXLEN	    1024	//���ջ����С
#define UART2_REC_MAXLEN	    2048	//���ջ����С
#define UART2_SEND_MAXLEN     	1400	//���ͻ���Ĵ�С

#define IsUart2RecvQueueNotEmpty() (uart2_recdat.uart2_reclen > 0)//(uart2_recdat.iEnd != uart2_recdat.iStart)

typedef struct {
	u8	rec_chanel;				//1:GPRS;0:����
	u16 	uart2_timeout;				//���ճ�ʱ
	u16  	uart2_reclen;	  			//���ڽ��ճ���
	u16     iStart;
	u16     iEnd;
	u8    	uart2_recBuff[UART2_REC_MAXLEN];	//���ջ�����
}UART2_RECEIVE;

enum {
	UART_MODE = 0,	//PA2��PA3��ΪIO
	IO_MODE		
};

typedef struct {
	u16  	uart2_sendlen;  			//���ڷ��������ܳ���
	u16  	uart2_sendinglen;  			//�������ڳ���
	u8    	uart2_sendBuff[UART2_SEND_MAXLEN]; 	//���ͻ�����
}UART2_SENDER;

extern UART2_RECEIVE	uart2_recdat;				//����2����

extern UART2_SENDER	uart2_senddat;				//����2����

void Uart2_RX_ISR(void);
u8 Uart2_TX_ISR(void);
void Uart2_HwInit(int type);
u16 uart2driver_addsend(const u8 *senddat, u16 length);
/********************************************************************
* ���� : Q228_hwInit
* ���� : ��ʼ��Q228ģ���CPU��Դ
*
* ����: ��
* ���: ��
* �ж���Դ:  uart2�շ��ж�
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void Q228_hwInit(void);
#endif
