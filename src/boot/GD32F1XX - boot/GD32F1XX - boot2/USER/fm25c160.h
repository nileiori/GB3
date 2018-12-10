#ifndef __FRAM_H
#define __FRAM_H

//*********************ͷ�ļ�********************************
#include "stm32f10x.h"
#include "spi_flash.h"

//*********************�Զ�����������************************


//**********************�궨��********************************


//******************����ռ����******************
#define FRAM_CHIP_START_ADDR		0
#define FRAM_CHIP_END_ADDR			2047//��ĩβ���ֽ������Լ죬���ܱ���������

#define	FRAM_UPDATA_FLAG_ADDR													1368//1�ֽڱ�־+1�ֽ�У��

#define FRAM_VDR_DOUBT_DATA_ADDR											1370//��ռ450�ֽڣ�150���㣬ÿ����3�ֽڣ�2�ֽ�����+1�ֽ�У�飩
#define FRAM_VDR_DOUBT_TIME_ADDR											1823//����6�ֽ�+1�ֽ�У�飬���ڼ�¼ͣ��ʱ��
#define FRAM_VDR_DOUBT_COUNT_ADDR											1830//����2�ֽ�+1�ֽ�У�飬���ڱ�ǵ�ǰ�¹��ɵ����ݼ�¼���ڼ�����
#define	FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR						1833//16*6�ֽ�,15����ͣ���ٶȼ�¼
#define	FRAM_VDR_SPEED_15_MINUTE_SPEED_COUNT_ADDR			1929//2�ֽڣ�1�ֽ�+1�ֽ�У��
#define	FRAM_VDR_SPEED_STOP_TIME_ADDR									1931//ͣ��ʱ�䣬4�ֽ�+1�ֽ�У��							
#define	FRAM_VDR_MILEAGE_BACKUP_ADDR									1936//�ۼ���ʻ��̱��ݣ�4�ֽ�+1�ֽ�У��		
#define	FRAM_VDR_MILEAGE_ADDR													1941//�ۼ���ʻ��̣�4�ֽ�+1�ֽ�У��										
#define	FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR						1946//ǩ��ʱ�䣬4�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR							1951//ǩ��ʱ�䣬4�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR						1956//��ǰ��ʻԱ��ţ�ȡֵ��Χ0-5��1�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_LICENSE_ADDR								1958//��ǰ��ʻ֤���룬18�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_START_TIME_ADDR							1977//��ʼʱ�䣬4�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_END_TIME_ADDR								1982//����ʱ�䣬4�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_START_POSITION_ADDR					1987//��ʼλ�ã�10�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_END_POSITION_ADDR						1998//����λ�ã�10�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_NIGHT_START_TIME_ADDR				2009//ҹ�俪ʼʱ�䣬4�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_NIGHT_END_TIME_ADDR					2014//ҹ�����ʱ�䣬4�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR		2019//ҹ�俪ʼλ�ã�10�ֽ�+1�ֽ�У��
#define	FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR			2030//ҹ�����λ�ã�10�ֽ�+1�ֽ�У��

#define FRAM_VDR_DOUBT_DATA_LEN											2
#define FRAM_VDR_DOUBT_TIME_LEN											6
#define FRAM_VDR_DOUBT_COUNT_LEN										2
#define	FRAM_VDR_SPEED_STOP_TIME_LEN								4
#define	FRAM_VDR_MILEAGE_BACKUP_LEN									4
#define	FRAM_VDR_MILEAGE_LEN												4
#define	FRAM_VDR_OVERTIME_LOGOUT_TIME_LEN						4
#define	FRAM_VDR_OVERTIME_LOGIN_TIME_LEN						4
#define	FRAM_VDR_OVERTIME_CURRENT_NUM_LEN						1
#define	FRAM_VDR_OVERTIME_LICENSE_LEN								18
#define	FRAM_VDR_OVERTIME_START_TIME_LEN						4
#define	FRAM_VDR_OVERTIME_END_TIME_LEN							4
#define	FRAM_VDR_OVERTIME_START_POSITION_LEN				10
#define	FRAM_VDR_OVERTIME_END_POSITION_LEN					10
#define	FRAM_VDR_OVERTIME_NIGHT_START_TIME_LEN			4
#define	FRAM_VDR_OVERTIME_NIGHT_END_TIME_LEN				4
#define	FRAM_VDR_OVERTIME_NIGHT_START_POSITION_LEN	10
#define	FRAM_VDR_OVERTIME_NIGHT_END_POSITION_LEN		10

//*******************����������*****************
#define	SPI_FRAM_SPIx_RCC							RCC_APB2Periph_SPI1//SPIX

#define	SPI_FRAM_CLK_RCC							RCC_APB2Periph_GPIOA//CLK
#define	SPI_FRAM_CLK_PORT							GPIOA
#define	SPI_FRAM_CLK_PIN							GPIO_Pin_5

#define	SPI_FRAM_MISO_RCC							RCC_APB2Periph_GPIOA//MISO
#define	SPI_FRAM_MISO_PORT						GPIOA
#define	SPI_FRAM_MISO_PIN							GPIO_Pin_6

#define	SPI_FRAM_MOSI_RCC							RCC_APB2Periph_GPIOA//MOSI
#define	SPI_FRAM_MOSI_PORT						GPIOA
#define	SPI_FRAM_MOSI_PIN							GPIO_Pin_7

#define SPI_FRAM_WP_RCC	   						RCC_APB2Periph_GPIOA//WP
#define SPI_FRAM_WP_PORT        			GPIOA
#define SPI_FRAM_WP_PIN         			GPIO_Pin_15

#define SPI_FRAM_CS_RCC   						RCC_APB2Periph_GPIOG//CS
#define SPI_FRAM_CS_PORT        			GPIOG
#define SPI_FRAM_CS_PIN        				GPIO_Pin_8 


#define FRAM_CS_LOW()       GPIO_ResetBits(SPI_FRAM_CS_PORT, SPI_FRAM_CS_PIN);spi_Delay_uS(60)
#define FRAM_CS_HIGH()      GPIO_SetBits(SPI_FRAM_CS_PORT, SPI_FRAM_CS_PIN);spi_Delay_uS(60)

#define FRAM_WP_LOW()       GPIO_ResetBits(SPI_FRAM_WP_PORT, SPI_FRAM_WP_PIN);spi_Delay_uS(60)
#define FRAM_WP_HIGH()      GPIO_SetBits(SPI_FRAM_WP_PORT, SPI_FRAM_WP_PIN);spi_Delay_uS(60)


//***********************��������********************************
/*********************************************************************
//��������	:FRAM_Init()
//����		:�����ʼ��
//��ע		:
*********************************************************************/
void FRAM_Init(void);
/*********************************************************************
//��������	:FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
//����		:��������д����,������ĳβ����У���ֽ�
//����		:WriteAddr�������ַ
//		:pBuffer�����ݻ���
//		:NumBytesToWrite��д����ֽ���
//��ע		:
*********************************************************************/
void FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite);
/*********************************************************************
//��������	:FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
//����		:�������������
//����		:ReadAddr�������ַ
//		:pBuffer��Ŀ�껺��
//		:NumBytesToRead���������ֽ��� 
//����		:ʵ�ʶ������ֽ���
//��ע		:
*********************************************************************/
u8 FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr);
/*********************************************************************
//��������	:FRAM_CheckChip(void)
//����		:�������оƬ�����Ƿ�����
//����		:�����ַ����������ֽ������Լ�
//		:
//		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
ErrorStatus FRAM_CheckChip(void);
/*********************************************************************
//��������	:FRAM_EraseChip(void)
//����		:������������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:״̬�Ĵ�����ֵ
//��ע		:
*********************************************************************/
void  FRAM_EraseChip(void);
#endif
