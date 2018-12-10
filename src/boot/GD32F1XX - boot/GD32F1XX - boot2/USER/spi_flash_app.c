
#include "stm32f10x.h"
#include "spi_flash.h"
#include "spi_flash_app.h"
#include <string.h>

ErrorStatus FLASH_CheckChip(void)
{
	ErrorStatus	ErrorFlag = SUCCESS;
	u8	i;
	u8	ReadBuffer[51] = {0};
	u8	WriteBuffer[51] = {"abcdefghijklmnopqrstuvwxyz0123456789"};
	u8	length = 0;
	u32	VenderID = 0;
	u32	Address = 0;
	

	VenderID = SPI_FLASH_ReadID();
	if((0 == VenderID)||(0xFFFFFFFF == VenderID))
	{
		ErrorFlag = ERROR;
		return ErrorFlag;
	}

	Address = FLASH_CHIP_END_SECTOR*FLASH_ONE_SECTOR_BYTES;
	SPI_FLASH_SectorErase(Address);
	
	length = strlen((char const *)WriteBuffer);
	SPI_FLASH_BufferWrite(WriteBuffer, Address, length);

	SPI_FLASH_BufferRead(ReadBuffer, Address, length);

	for(i=0; i<length; i++)
	{
		if(ReadBuffer[i] != WriteBuffer[i])
		{
			ErrorFlag = ERROR;
			break;
		}
	}

	return ErrorFlag;
}
