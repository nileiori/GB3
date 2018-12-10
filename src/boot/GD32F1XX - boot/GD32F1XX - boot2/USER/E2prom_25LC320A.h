/*************************************************************************
*
* Copyright (c) 2008,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : E2prom_25LC320A.h
* ����     : E2prom_25LC320A�Ķ�д����.
*
* ��ǰ�汾 : 1.0
* ������   : Senon Dou
* �޸�ʱ�� : 2013.07.26
*
* ��ʷ�汾 : 
* ������   : 
* ���ʱ�� : 
* 
* ��ע :
*************************************************************************/
#ifndef _E2prom_25LC320A_H_
#define _E2prom_25LC320A_H_

#include "stm32f10x.h"

#define READ       0x03  /* Read data from memory array beginning at selected address */
#define WRITE      0x02  /* Write data to memory array beginning at selected address  */
#define WREN       0x06  /* Reset the write enable latch (disable write operations) */
#define EWDI       0x04  /* Set the write enable latch (enable write operations) */
#define RDSR       0x05  /* Read STATUS register */
#define WRSR       0x01  /* Write STATUS register */
#define E2PROM_WIP_BIT	0x01	//bit1
#define E2PROM_WEL_BIT	0x02	//bit1
#define E2PROM_WPEN_BIT	0x80	//bit7
#define E2PROM_BP1BP0_BIT	0x0C	//bit3,bit2

/* E2PROM 25LC320A Microwire �ӿ�����  */  

#define	SPI_E2PROM_SPIx_RCC							RCC_APB2Periph_SPI1//SPIX

#define	SPI_E2PROM_CLK_RCC							RCC_APB2Periph_GPIOA//CLK
#define	SPI_E2PROM_CLK_PORT							GPIOA
#define	SPI_E2PROM_CLK_PIN							GPIO_Pin_5

#define	SPI_E2PROM_MISO_RCC							RCC_APB2Periph_GPIOA//MISO
#define	SPI_E2PROM_MISO_PORT						GPIOA
#define	SPI_E2PROM_MISO_PIN							GPIO_Pin_6

#define	SPI_E2PROM_MOSI_RCC							RCC_APB2Periph_GPIOA//MOSI
#define	SPI_E2PROM_MOSI_PORT						GPIOA
#define	SPI_E2PROM_MOSI_PIN							GPIO_Pin_7

#define SPI_E2PROM_WP_RCC	   						RCC_APB2Periph_GPIOA//WP
#define SPI_E2PROM_WP_PORT        			GPIOA
#define SPI_E2PROM_WP_PIN         			GPIO_Pin_15

#define SPI_E2PROM_CS_RCC   						RCC_APB2Periph_GPIOG//CS
#define SPI_E2PROM_CS_PORT        			GPIOG
#define SPI_E2PROM_CS_PIN        				GPIO_Pin_4 


#define E2PROM_CS_LOW()       GPIO_ResetBits(SPI_E2PROM_CS_PORT, SPI_E2PROM_CS_PIN);spi_Delay_uS(30)
#define E2PROM_CS_HIGH()      GPIO_SetBits(SPI_E2PROM_CS_PORT, SPI_E2PROM_CS_PIN);spi_Delay_uS(30)

#define E2PROM_WP_LOW()       GPIO_ResetBits(SPI_E2PROM_WP_PORT, SPI_E2PROM_WP_PIN);spi_Delay_uS(30)
#define E2PROM_WP_HIGH()      GPIO_SetBits(SPI_E2PROM_WP_PORT, SPI_E2PROM_WP_PIN);spi_Delay_uS(30)

////////////////////e2prom ��ַ�ռ䶨��///////////////////
#define E2PROM_CHIP_START_ADDR		0
#define E2PROM_CHIP_END_ADDR			4095//��ĩβ���ֽ������Լ죬���ܱ���������

void E2prom_Init(void);
void E2prom_ReadByte(u16 Address, u8 * destin, u8 bytenum);
u8 E2prom_WriteByte(u16 Address, u8 data);
u8 E2prom_ReadStatusRegister(void);
ErrorStatus Eeprom_CheckChip(void);
#endif
