/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : spi_flash.c
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : This file provides a set of functions needed to manage the
*                      communication between SPI peripheral and SPI M25P64 FLASH.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "spi_flash.h"
#include "spi_flash_app.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"


/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize    0x100

/* Private define ------------------------------------------------------------*/
#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define READ_0B	   0x0B
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0x20  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */

#define UPROTECT   0x39

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xA5

#define FLASH_DELAY_COUNT_MAX	100	//FLASH操作最大延时0.5s，超出0.3s后跳出死循环

//************本地变量*****************
static u32 FlashDelayCount = 0;	//FLASH读写延时计数

u8	ZhikuAreaWriteEnableFlag = 0;//字库区域写使能标志,0为不使能,1为使能
/*******************************************************************************
* Function Name  : spi_Delay_uS(u32 x)
* Description    : 延时，单位微妙
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void spi_Delay_uS(u32 x)
{ u32 i;
  for(i = 0; i <= x; i ++);
}
/*******************************************************************************
* Function Name  : IncFlashDelayCount
* Description    : FlashDelayCount变量加1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IncFlashDelayCount(void)
{
	if(0xFFFFFFF0 == FlashDelayCount)
	{
		FlashDelayCount = 0;//防止产生溢出中断
	}
	FlashDelayCount++;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA |
                         RCC_APB2Periph_FLASH_CS, ENABLE);

  /* Configure SPI1 pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure I/O for Flash Chip select */
  GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_FLASH_CS_PORT, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
  
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  //SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
  
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
	if(0 == ZhikuAreaWriteEnableFlag)
	{
		//保护汉字库
		if(SectorAddr < FLASH_HZK16_END_ADDR)
		{
			return ;
		}
	}
	/* Flash Unprotect */
	SPI_FLASH_AllUnprotect();
	
  	/* Send write enable instruction */
  	SPI_FLASH_WriteEnable();

  	/* Sector Erase */
  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();

  	/* Send Sector Erase instruction */
  	SPI_FLASH_SendByte(SE);

  	/* Send SectorAddr high nibble address byte */
  	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);

  	/* Send SectorAddr medium nibble address byte */
  	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);

  	/* Send SectorAddr low nibble address byte */
  	SPI_FLASH_SendByte(SectorAddr & 0xFF);

  	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
  	SPI_FLASH_WaitForWriteEnd();

	/* Flash Protect */
	SPI_FLASH_AllProtect();

}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(void)
{
	/* Flash Unprotect */
	SPI_FLASH_AllUnprotect();

  	/* Send write enable instruction */
  	SPI_FLASH_WriteEnable();

  	/* Bulk Erase */
  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();
  	/* Send Bulk Erase instruction  */
  	SPI_FLASH_SendByte(BE);
  	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();

  	/* Wait the end of Flash writing */
  	SPI_FLASH_WaitForWriteEnd();

	/* Flash Protect */
	SPI_FLASH_AllProtect();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{	
	if(0 == ZhikuAreaWriteEnableFlag)
	{
		//保护汉字库
		if(WriteAddr < FLASH_HZK16_END_ADDR)
		{
			return ;
		}
	}

 	 /* Enable the write access to the FLASH */
  	SPI_FLASH_WriteEnable();
	
  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();

  	/* Send "Write to Memory " instruction */
  	SPI_FLASH_SendByte(WRITE);

  	/* Send WriteAddr high nibble address byte to write to */
  	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);

  	/* Send WriteAddr medium nibble address byte to write to */
  	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);

  	/* Send WriteAddr low nibble address byte to write to */
  	SPI_FLASH_SendByte(WriteAddr & 0xFF);

  	/* while there is data to be written on the FLASH */
  	while (NumByteToWrite--)
  	{
    		/* Send the current byte */
    		SPI_FLASH_SendByte(*pBuffer);

    		/* Point on the next byte to be written */
    		pBuffer++;
  	}

  	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();

  	/* Wait the end of Flash writing */
  	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  //u8 Status = 0;
  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	
  	if(0 == ZhikuAreaWriteEnableFlag)
	{
  		//保护汉字库
		if(WriteAddr < FLASH_HZK16_END_ADDR)
		{
			return ;
		}
	}

	SPI_FLASH_AllUnprotect();
	//Status = SPI_FLASH_ReadStatus();
  if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  
  SPI_FLASH_AllProtect();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
/*
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  // Select the FLASH: Chip Select low
  SPI_FLASH_CS_LOW();

  // Send "Read from Memory " instruction 
  SPI_FLASH_SendByte(READ);

  // Send ReadAddr high nibble address byte to read from 
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  // Send ReadAddr medium nibble address byte to read from 
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  // Send ReadAddr low nibble address byte to read from 
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) // while there is data to be read
  {
    // Read a byte from the FLASH 
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    // Point to the next location where the byte read will be saved 
    pBuffer++;
  }

  // Deselect the FLASH: Chip Select high 
  SPI_FLASH_CS_HIGH();
}
*/

void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  // Select the FLASH: Chip Select low
  SPI_FLASH_CS_LOW();

  // Send "Read from Memory " instruction 
  SPI_FLASH_SendByte(READ_0B);

  // Send ReadAddr high nibble address byte to read from 
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  // Send ReadAddr medium nibble address byte to read from 
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  // Send ReadAddr low nibble address byte to read from 
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  
  // Send a Dummy Byte
  SPI_FLASH_SendByte(Dummy_Byte);

  while (NumByteToRead--) // while there is data to be read
  {
    // Read a byte from the FLASH 
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    // Point to the next location where the byte read will be saved 
    pBuffer++;
  }

  // Deselect the FLASH: Chip Select high 
  SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0, Temp3 = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(0x9F);

  /* Read a byte from the FLASH */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
  
  Temp3 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  Temp = (Temp0 << 24) | (Temp1 << 16) | (Temp2 << 8) | Temp3;

  return Temp;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(READ);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
  //FLASH操作延时计数清0
  FlashDelayCount = 0;
	
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
  {
  	if(FlashDelayCount > FLASH_DELAY_COUNT_MAX)
	{
		break;//超出规定时间，跳出循环
	}
  }

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);
  
  //FLASH操作延时计数清0
  FlashDelayCount = 0;
  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
  {
  	if(FlashDelayCount > FLASH_DELAY_COUNT_MAX)
	{
		break;//超出规定时间，跳出循环
	}
  }

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  //FLASH操作延时计数清0
  FlashDelayCount = 0;
  
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
  {
  	if(FlashDelayCount > FLASH_DELAY_COUNT_MAX)
	{
		break;//超出规定时间，跳出循环
	}
  }

  /* Send Half Word through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, HalfWord);

  //FLASH操作延时计数清0
  FlashDelayCount = 0;
  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
  {
  	if(FlashDelayCount > FLASH_DELAY_COUNT_MAX)
	{
		break;//超出规定时间，跳出循环
	}
  }

  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(WREN);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorUnprotect
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorUnprotect(u32 SectorAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Unprotect" instruction */
  SPI_FLASH_SendByte(UPROTECT);
  
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_AllUnprotect
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_AllUnprotect(void)
{
	u8 statereg = 0x00;
	
		 /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Unprotect" instruction */
  SPI_FLASH_SendByte(WRSR);
  
  SPI_FLASH_SendByte(statereg);
 
  SPI_FLASH_CS_HIGH();
  
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_AllUnprotect
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_AllProtect(void)
{
	u8 statereg = 0x3C;
	

	/* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Unprotect" instruction */
  SPI_FLASH_SendByte(WRSR);
  
  SPI_FLASH_SendByte(statereg);
 
  SPI_FLASH_CS_HIGH();
  
  SPI_FLASH_WaitForWriteEnd();
}
/*******************************************************************************
* Function Name  : SPI_FLASH_ReadStatus
* Description    : Read Status Register
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 SPI_FLASH_ReadStatus(void)
{
	u8 FLASH_Status = 0;
	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();
	
	/* Send "Read Status Register" instruction */
  	SPI_FLASH_SendByte(RDSR);
	
	/* Send a dummy byte to generate the clock needed by the FLASH
    	and put the value of the status register in FLASH_Status variable */
	FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
	
	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();
	
	return FLASH_Status;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0, FLASH_Status1 = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(RDSR);

  /*FLASH操作延时计数清0*/
  FlashDelayCount = 0;
  
  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
    FLASH_Status1 = SPI_FLASH_SendByte(Dummy_Byte);
    
    if(FlashDelayCount > FLASH_DELAY_COUNT_MAX)
    {
	break;//超出规定时间，跳出循环
    }
    
  }
  while ((FLASH_Status & WIP_Flag) == WIP_Flag); /* Write in progress */

	FLASH_Status = FLASH_Status1;
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
