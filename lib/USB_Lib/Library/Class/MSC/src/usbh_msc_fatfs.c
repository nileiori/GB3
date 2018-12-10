
#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"
#include "spi.h"
#include "mmc_sd.h"
/*--------------------------------------------------------------------------

Module Private Functions and Variables

---------------------------------------------------------------------------*/
#if DISK_MODE == DISK_MODE_USE_FLASH
//对于W25Q64 
//前4.8M字节给fatfs用,4.8M字节后~4.8M+100K给用户用,4.9M以后,用于存放字库,字库占用3.09M.		 			    
u16	    FLASH_SECTOR_COUNT= 9832;	//4.8M字节,默认为W25Q64
uint32_t spi_flash_type;
#define FLASH_SECTOR_SIZE 	512		
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区
#endif

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
/* dxl,2013.11.30只有插了SD卡,u盘才能读写，没有插SD卡，u盘只能读，该函数有bug,苗亚翰修改为如下函数替换
DSTATUS disk_initialize (
                         BYTE drv		// Physical drive number (0) 
                           )
{   	 
  if(drv == SD_CARD) //BY WYF
  {
    Stat = SD_Initialize();
  }	    
  else if(HCD_IsDeviceConnected(&USB_OTG_Core))
  {  
    Stat &= ~STA_NOINIT;
  }
  
  return Stat;
}
*/
DSTATUS disk_initialize (
                         BYTE drv /* Physical drive number (0) */
                           )
{   
	
  DSTATUS usb_stat=STA_NOINIT;//定义局部变量，且对其进行初始化
  if(drv == SD_CARD) //BY WYF
  {
  	#if	DISK_MODE == DISK_MODE_USE_SDCARD
    Stat = SD_Initialize();
		#elif DISK_MODE == DISK_MODE_USE_FLASH
		sFLASH_DeInit();
  	sFLASH_Init();
    sFLASH_AllUnprotect();
		spi_flash_type = sFLASH_ReadID();
		return 0;
		#else

		#endif
  }     
  else if(HCD_IsDeviceConnected(&USB_OTG_Core))
  {  
    usb_stat &= ~STA_NOINIT;//使用已经初始化的usb_stat
    Stat = usb_stat;//传递给全局变量
  }
  return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
                     BYTE drv		/* Physical drive number (0) */
                       )
{
    if(drv == SD_CARD) //BY WYF
        return 0;
    //if (drv) return STA_NOINIT;		/* Supports only single drive */
    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
                   BYTE drv,			/* Physical drive number (0) */
                   BYTE *buff,			/* Pointer to the data buffer to store read data */
                   DWORD sector,		/* Start sector number (LBA) */
                   BYTE count			/* Sector count (1..255) */
                   )
{
    BYTE status = USBH_MSC_OK;
		
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		  
    if(drv==SD_CARD) //BY WYF
    {
    		#if	DISK_MODE == DISK_MODE_USE_SDCARD
        status = SD_ReadDisk(buff,sector,count);
				#elif	DISK_MODE == DISK_MODE_USE_FLASH
				for(;count>0;count--)
				{
					sFLASH_ReadBuffer(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
					sector++;
					buff+=FLASH_SECTOR_SIZE;
				}
				status=0;
				#else

				#endif
    }
    else
    {
        if (Stat & STA_NOINIT) return RES_NOTRDY;
        if(HCD_IsDeviceConnected(&USB_OTG_Core))
        {    
            do
            {
                status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512 * count);
                USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);    
                if(!HCD_IsDeviceConnected(&USB_OTG_Core))
                { 
                    return RES_ERROR;
                }      
            }
            while(status == USBH_MSC_BUSY );
        }
    } 
    if(status == USBH_MSC_OK)
        return RES_OK;
    return RES_ERROR;  
}




/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
                    BYTE drv,			/* Physical drive number (0) */
                    const BYTE *buff,	/* Pointer to the data to be written */
                    DWORD sector,		/* Start sector number (LBA) */
                    BYTE count			/* Sector count (1..255) */
                    )
{
		#if DISK_MODE == DISK_MODE_USE_FLASH
		u32  write_addr;
		#endif
    BYTE status = USBH_MSC_OK;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	
  
    if(drv == SD_CARD)
    {
    		#if	DISK_MODE == DISK_MODE_USE_SDCARD
        status=SD_WriteDisk((u8*)buff,sector,count);    //BY WYF
				#elif	DISK_MODE == DISK_MODE_USE_FLASH
				for(;count>0;count--)
				{		
					write_addr = sector<<12;
					sFLASH_EraseSector(write_addr);
					sFLASH_WriteBuffer((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
					sector++;
					buff+=FLASH_SECTOR_SIZE;
				}
				status=0;
				#else

				#endif
    }
    else
    { 
        if (Stat & STA_NOINIT) return RES_NOTRDY;
        if (Stat & STA_PROTECT) return RES_WRPRT; 
        if(HCD_IsDeviceConnected(&USB_OTG_Core))
        {  
            do
            {
                status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512 * count);
                USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);   
                if(!HCD_IsDeviceConnected(&USB_OTG_Core))
                { 
                    return RES_ERROR;
                }
            }  
            while(status == USBH_MSC_BUSY );    
        }
    }
    if(status == USBH_MSC_OK)
        return RES_OK;
    return RES_ERROR;
}

#endif /* _READONLY == 0 */


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL != 0
DRESULT disk_ioctl (
                    BYTE drv,		/* Physical drive number (0) */
                    BYTE ctrl,		/* Control code */
                    void *buff		/* Buffer to send/receive control data */
                    )
{
 
    DRESULT res = RES_OK;

  	if(drv==SD_CARD)//SD卡 BY WYF
	{
			#if	DISK_MODE == DISK_MODE_USE_SDCARD
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				SD_CS_LOW();
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				SD_CS_HIGH();
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
			#elif	DISK_MODE == DISK_MODE_USE_FLASH
			switch(ctrl)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
			#else

			#endif
    }
	else 
    {     
        res = RES_ERROR;
  
        if (Stat & STA_NOINIT) return RES_NOTRDY;
  
        switch (ctrl) 
        {
            case CTRL_SYNC :		/* Make sure that no pending write process */  
                res = RES_OK;
                break;
            
            case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */   
                *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
                res = RES_OK;
                break;
    
            case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
                *(WORD*)buff = 512;
                res = RES_OK;
                break;
    
            case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
                *(DWORD*)buff = 512;  
                break;   
            default:
                res = RES_PARERR;
        }
    }

    return res;       
}
#endif /* _USE_IOCTL != 0 */


