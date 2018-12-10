/*************************************************************************
* Copyright (c) 2014 All rights reserved., 
* �ļ����� : ICCARD_E2.h
* ��ǰ�汾 : 0.1
* ������   : MARK
* �༭ʱ�� : 2014��03��18��
* ����ʱ�� ��
*************************************************************************/

#ifndef _ICCARD_E2_H
#define _ICCARD_E2_H

#include "stm32f10x.h"
#include <string.h>
#include <inttypes.h>

/****************************�궨��E2�ĸ������ܲ���*********************/

#define RMM_COMM 0x30    //������������
#define UMM_COMM 0x38    //д����������
#define CVD_COMM 0x33    //У������
#define RSM_COMM 0x31    //������洢��
#define USM_COMM 0x39    //д����洢��
#define RPM_COMM 0x34    //�������洢��
#define WPM_COMM 0x3c    //д�����洢��

#define EE_24XX_HL_DEV_ADDR       0xA0            /* 24xx128���豸��ַ */
#define EE_24XX_HL_PAGE_SIZE      64              /* 24xx128��ҳ���С */
#define EE_24XX_HL_SIZE          16384            /* 24xx128 ������ */

#define EE_24C128_PAGE_TOTAL      256             /*256= 16384/64*/

#define EE_24XX_HL_SELTEST_ADDR   0               /*�Լ���ʼ��ַ*/
#define EE_24XX_HL_SELTEST_LEN    16              /*�Լ����򳤶�*/
///////////////////////
extern u8 EEPROM_24XX_HL_Byte_Write( const u16 Address ,  const u8 data );

extern u16 EEPROM_24XX_HL_Buf_Read(  const u16 InAddr, u8 * pOutBuf,  u16 OutLength );

extern u16 EEPROM_24XX_HL_Buf_Write( const u16 InAddr, u8 * pInBuf,  u16 InLength  );

extern u8 EEPROM_24XX_HL_Init(void );

extern u8 EEPROM_24XX_HL_SelCheck(void );

extern u8 EEPROM_HL_CheckOk(void);

extern u8 EEPROM_24C128_AllChip_Read( void  );

extern u8 EEPROM_24C128_AllChip_Read_From_EEPROM2RAM( void  );

extern u8 EEPROM_24C128_AllChip_Write_From_RAM2EEPROM( void  );

extern u8 SL4442_Buffer_Read(  u8 *pBuf, u16  LengthBuf );

/********************************************************************************************************
*	�� �� ��: E2_ReadBytes
*	����˵��: �Ӵ���EEPROMָ����ַ����ʼ��ȡ��������
*	��    �Σ�_pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*			 _ucDevAddr : �豸��ַ
*			 _usAddress : ��ʼ��ַ
*			 _usSize : ���ݳ��ȣ���λΪ�ֽ�
*			 _pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
********************************************************************************************************/
u8 E2_ReadBytes(u8 *_pReadBuf, u8 _ucDevAddr, u8 _usAddress, u16 _usSize);
/**********************************************************************************************************
*	�� �� ��: E2_WriteBytes
*	����˵��: ����EEPROMָ����ַд���������ݣ�����ҳд�������д��Ч��
*	��    �Σ�_pWriteBuf : ��Ŷ��������ݵĻ�����ָ��
*			  _ucDevAddr : �豸��ַ
*			  _usAddress : ��ʼ��ַ
*			  _usSize : ���ݳ��ȣ���λΪ�ֽ�
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
**********************************************************************************************************/
u8 E2_WriteBytes(u8 *_pWriteBuf, u16 _ucDevAddr, u16 _usAddress, u16 _usSize);
/**********************************************************************************************************
*	�� �� ��: E2_WriteOneByte
*	����˵��: ����EEPROMָ����ַд��һ������
*	��    �Σ�data : ����
*			  _ucDevAddr : �豸��ַ
*			  _ucAddress : д���ַ
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
**********************************************************************************************************/
u8 E2_WriteOneByte(u8 data, unsigned char _ucDevAddr, unsigned char _ucAddress);
/********************************************************************************************************
*	�� �� ��: E2_ReadOneByte
*	����˵��: �Ӵ���EEPROMָ����ַ����ʼ��ȡһ������
*	��    �Σ�_pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*			 _ucDevAddr : �豸��ַ
*			 _usAddress : ��ʼ��ַ
*			 _pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
********************************************************************************************************/
u8 E2_ReadOneByte(u8 *data, u8 _ucDevAddr, u8 _usAddress);
#endif
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

