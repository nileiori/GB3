#ifndef	__GPS_DRIVER_H
#define	__GPS_DRIVER_H

#include "stm32f10x.h"

extern u8	GpsOnOffFlag;//GPS���ر�־,0ΪGPS��,1ΪGPS��

//*******************�궨��***********************
#define 	GPS_POWER_OFF()    	GpioOutOff(GPS_CTR);GpsOnOffFlag=0      //�ر�GPSģ���Դ
//#define 	GPS_POWER_OFF()    	GpioOutOn(GPS_CTR);GpsOnOffFlag=1	//��GPS��Դ
#define 	GPS_POWER_ON()    	GpioOutOn(GPS_CTR);GpsOnOffFlag=1	//��GPS��Դ
#define		GPS_RX_BUFFER_SIZE	100//���ջ����С
#define		GPRMC_BUFFER_SIZE	100
#define		GPGGA_BUFFER_SIZE	100
#define		GPGSV_BUFFER_SIZE	100
#define		GPS_SEND_BUFFER_SIZE	200
typedef struct
{
	u8	SendBuffer[GPS_SEND_BUFFER_SIZE];
	u16	SendLen;
	u16	SendCount;
}USART3_SEND_STRUCT;

/*********************************************************************
//��������	:Gps_Init(void)
//����		:GPS��ʼ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Gps_Init(void);
/*********************************************************************
//��������	:GPS_RxIsr(void)
//����		:GPS�����ж�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Gps_RxIsr(void);
/*********************************************************************
//��������	:GPS_TxIsr(void)
//����		:GPS�����ж�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Gps_TxIsr(void);
/*********************************************************************
//��������	:GPS_SendData
//����		:GPS��������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:�ɹ�����ACK_OK��ʧ�ܷ���ACK_ERROR
//��ע		:
*********************************************************************/
ProtocolACK  Gps_SendData(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:GPS_ReadGPRMC(u8 *pBuffer)
//����		:��GPRMCԭʼ����
//��ע		:
*********************************************************************/
u8 Gps_ReadGPRMC(u8 *pBuffer);
/*********************************************************************
//��������	:GPS_ReadGPGSV(u8 *pBuffer)
//����		:��GPGSVԭʼ����
//��ע		:
*********************************************************************/
u8 Gps_ReadGPGSV(u8 *pBuffer);
void GPS_putc(int ch);
void GPS_putstr(char* str);
#endif