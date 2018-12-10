
#include "my_typedef.h"

#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

//*********�궨��****************
//*********�궨��****************
//#define HZK16_IN_CHIP	1

#define ASCII_BASE_ADDR	0x0803f7ff	//Ӣ���ַ��洢�׵�ַ
#define HZK16_BASE_ADDR	0x0803ffff	//���ֿ�洢�׵�ַ
#define READ_HZK16_BYTE(i) (*(u8 *)(HZK16_BASE_ADDR+(i)))
#define READ_ASCII_BYTE(i) (*(u8 *)(ASCII_BASE_ADDR+(i)))

#define	HZK16_END_ADDR	0x41000

//********A0--PC.8**************
#define LCD_A0_HIGH()	GpioOutOn(LCD_AO);lcd_delay(4)   //����,����ѡ����
#define LCD_A0_LOW()	GpioOutOff(LCD_AO);lcd_delay(4)
//********SCK--PB.13**************
#define LCD_SCK_HIGH()	GpioOutOn(LCD_SPI_SCK);lcd_delay(4) //delay(4),����Ϊ1.42us,���е͵�ƽΪ440ns,����ʱΪ�ߵ�ƽ
#define LCD_SCK_LOW()	GpioOutOff(LCD_SPI_SCK);lcd_delay(4)
//********DA--PB.14**************
#define LCD_DA_HIGH()	GpioOutOn(LCD_SPI_MISO);lcd_delay(4)
#define LCD_DA_LOW()	GpioOutOff(LCD_SPI_MISO);lcd_delay(4)
//********CS--PE.8**************
#define LCD_CS_HIGH()	GpioOutOn(LCD_CS);lcd_delay(4)
#define LCD_CS_LOW()	GpioOutOff(LCD_CS);lcd_delay(4)

//*********��������**************
/*********************************************************************
//��������	:Lcd_Delayms(u32 n)
//����		:��ʱ����
//��ע		:
*********************************************************************/
void Lcd_Delayms(u32 n);
/*********************************************************************
//��������	:Lcd_DisplayClr(void)
//����		:���������Ļ
//��ע		:
*********************************************************************/
void Lcd_DisplayClr(void);
/*********************************************************************
//��������	:Lcd_WriteData(u8 dat)
//����		:дһ���ֽڵ�����
//��ע		:
*********************************************************************/
void Lcd_WriteData(u8 dat);
/*********************************************************************
//��������	:Lcd_WriteCom(u8 com)
//����		:дһ���ֽڵ�����
//��ע		:
*********************************************************************/
void Lcd_WriteCom(u8 com);
/*********************************************************************
//��������	:Lcd_IoInitial(void)
//����		:Һ��ģ��IO�ڳ�ʼ��
//��ע		:
*********************************************************************/
void Lcd_IoInitial(void);
/*********************************************************************
//��������	:Lcd_InitialST7567(void)
//����		:��ʼ��Һ����ʾ��

//��ע		:
*********************************************************************/
void Lcd_InitialST7567(void);
/*********************************************************************
//��������	:Lcd_DisplayOneChineseCharacter(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer)
//����		:��ʾһ������
//��ע		:
*********************************************************************/
u8 Lcd_DisplayOneChineseCharacter(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer);
/*********************************************************************
//��������	:Lcd_DisplayOneChineseCharacter(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer)
//����		:��ʾһ������
//��ע		:
*********************************************************************/
u8 Lcd_DisplayOneChineseCharacter(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer);
/*********************************************************************
//��������	:ReadOneChineseCharacterFromFlash(u32	Address, u8 *pBuffer)
//����		:��FLASH�ж�ȡһ�����ֵı���
//��ע		:
*********************************************************************/
u8 Lcd_ReadOneChineseCharacterFromFlash(u32	Address, u8 *pBuffer);
/*********************************************************************
//��������	:Lcd_DisplayOneEnglishCharacter(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer)
//����		:��ʾһ��Ӣ���ַ�
//��ע		:
*********************************************************************/
u8 Lcd_DisplayOneEnglishCharacter(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer);
/*********************************************************************
//��������	:Lcd_ReadOneEnglishCharacterFromFlash(u32 Address, u8 *pBuffer)
//����		:��FLASH�ж�ȡһ��Ӣ���ַ��ı���
//��ע		:
*********************************************************************/
u8 Lcd_ReadOneEnglishCharacterFromFlash(u32 Address, u8 *pBuffer);
/*********************************************************************
//��������	:Lcd_DisplayString(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer, u16 length)
//����		:��ʾһ���ַ��������ַ�����������Ӣ�Ļ��
//��ע		:
*********************************************************************/
void Lcd_DisplayString(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer, u16 length);
/*********************************************************************
//��������	:Lcd_Initial(void)
//����		:LCDģ���ʼ��
//��ע		:
*********************************************************************/
void Lcd_Initial(void);
/*********************************************************************
//��������	:Lcd_BeepIoInit(void)
//����		:������IO��ʼ��
//��ע		:
*********************************************************************/
void Lcd_BeepIoInit(void);
/*********************************************************************
//��������	:ChangeGraphFormatToChineseFormat(u8 *pGraphBuffer, u8 *pChineseBuffer)
//����		:��������֮ǰͼ�θ�ʽ�ı���ת���ɺ��ֱ���
//��ע		:����Buffer�ĳ��Ⱦ�Ϊ32�ֽ�
*********************************************************************/
void ChangeGraphFormatToChineseFormat(u8 *pChineseBuffer, u8 *pGraphBuffer);
/*********************************************************************
//��������	:Lcd_DisplayString(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer, u16 length)
//����		:��ʾһ���ַ��������ַ�����������Ӣ�Ļ��
//��ע		:
*********************************************************************/
void Lcd_DisplayString(u8 Line, u8 StartColumn, u8 DisplayModule, u8 *pBuffer, u16 length);

void displayfull(void);

void LCD_Delay(void);

/*********************************************************************
//��������	:lcd_delay(u32 n)
//����		:�ӳٺ���
//��ע		:��ʱn��for���ʱ��
*********************************************************************/
static void lcd_delay(u32 n);
#endif
