/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : spi_flash.h
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : Header for spi_flash.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */


#define SPI_FLASH_WP_PORT        	GPIOA
#define SPI_FLASH_WP_PIN         	GPIO_Pin_15

#define SPI_FLASH_CS_PORT        	GPIOA
#define SPI_FLASH_CS_PIN        	GPIO_Pin_4 
#define RCC_APB2Periph_FLASH_CS   RCC_APB2Periph_GPIOA

/* Exported macro ------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN);spi_Delay_uS(10)
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN);spi_Delay_uS(10)

#define SPI_FLASH_WP_LOW()       GPIO_ResetBits(SPI_FLASH_WP_PORT, SPI_FLASH_WP_PIN);spi_Delay_uS(10)
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_WP_HIGH()      GPIO_SetBits(SPI_FLASH_WP_PORT, SPI_FLASH_WP_PIN);spi_Delay_uS(10)

/* Exported functions ------------------------------------------------------- */
void spi_Delay_uS(u32 x);
void IncFlashDelayCount(void);

/*----- High layer function -----*/
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32 SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadStatus(void);
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_SectorUnprotect(u32 SectorAddr);
void SPI_FLASH_AllUnprotect(void);
void SPI_FLASH_AllProtect(void);

#endif /* __SPI_FLASH_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
