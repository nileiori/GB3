/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include<stdlib.h>
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "usbh_usr.h"
#include "usb_app.h"
#include "ff.h"       /* FATFS */
#include "spi_flash_app.h"
#include "spi_flash.h"
#include "Public.h"
//#include "modem_app_com.h"
#include "EepromPram.h"
#include "E2prom_25LC320A.h"
#include "taskschedule.h"
#include "stm32f10x_iwdg.h"
#include "SysTick.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
//#define IMAGE_BUFFER_SIZE    10
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
FATFS fatfs;
FIL file;
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
//uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0; 
///////////////////////////

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level);
static void USB_USR_CurrentApplication(void);
extern void Usb_SetCurrentUsbStatus(unsigned char status);
extern unsigned long Public_ConvertBufferToLong(unsigned char *buf);
/**
* @}
*/ 

u16* ff_NameConver(u8 *pStr);
void ff_NameForward(u8 *pDst,u16 *pSrc);

/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{

}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
  //LCD_UsrLog((void *)MSG_DEV_ATTACHED);
  //Usb_SetCurrentUsbStatus(USB_WORK_STATUS_INSERT);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
  //LCD_ErrLog((void *)MSG_UNREC_ERROR); 
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
  Usb_SetCurrentUsbStatus(USB_WORK_STATUS_DEL);
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{

}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 

}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{

}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  //LCD_UsrLog("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
  //LCD_UsrLog("Product : %s\n", (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  //LCD_UsrLog( "Serial Number : %s\n", (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
    Usb_SetCurrentUsbStatus(USB_WORK_STATUS_ENUM);
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  //LCD_ErrLog ("> Device not supported."); 
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  
  usbh_usr_status = USBH_USR_NO_RESP;  
#if 1
  /*Key B3 is in polling mode to detect user action */
  //if(STM_EVAL_PBGetState(Button_KEY) == RESET) 
  {
    
    usbh_usr_status = USBH_USR_RESP_OK;
    
  } 
#endif
  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
  //LCD_ErrLog ("Overcurrent detected.");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
    switch(USBH_USR_ApplicationState)
    {
        case USH_USR_FS_INIT:  
            f_mount(0, NULL); 
            /* Initialises the File System*/
            if ( f_mount( 0, &fatfs ) != FR_OK ) 
            {
                /* efs initialisation fails*/
                Usb_SetCurrentUsbStatus(USB_WORK_STATUS_ERROR);
                return(-1);
            }
	    f_mount(0, NULL); 
            //if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
            //{
                //LCD_ErrLog((void *)MSG_WR_PROTECT);
            //}
            USBH_USR_ApplicationState = USH_USR_FS_READLIST;
            break;
        case USH_USR_FS_READLIST:
            //Explore_Disk("0:/", 1);myh,2013.7.31,��Щʱ�������ﵼ������,�ļ��б��ʱ̫��
            line_idx = 0;
            USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_READLIST);
            break;

        case USH_USR_FS_WRITEFILE:            
            USB_OTG_BSP_mDelay(200);
            //if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
            //{
                //USBH_USR_ApplicationState = USH_USR_FS_DRAW;
                //break;
            //}
            /* Register work area for logical drives */
            f_mount(0, &fatfs);
            ////////////////////////
            USB_USR_CurrentApplication();
            /////////////////////////
            f_mount(0, NULL); 
            USBH_USR_ApplicationState = USH_USR_FS_DRAW; 
            break;

        case USH_USR_FS_DRAW:
            //if(ucUsbWriteDataFlag)
            //{
               //USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE; 
               //USB_USR_WriterDataToUsbApp();
            //}
            break;
        default: break;
    }
    return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level)
{

  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  char tmp[14];
  u8 FileName_1[_MAX_LFN];
  
  res = f_opendir(&dir, ff_NameConver((u8 *)path));
  if (res == FR_OK) {
    while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) 
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }    
      ff_NameForward(FileName_1,fno.fname);
      fn = (char *)FileName_1;
      
      strcpy(tmp, fn); 

      line_idx++;
      if(line_idx > 9)
      {
        line_idx = 0;
      } 
      if(((fno.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
      {
        Explore_Disk(fn, 2);
      }
    }
  }
  return res;
}

#if 0
static uint8_t Image_Browser (char* path)
{
  FRESULT res;
  uint8_t ret = 1;
  FILINFO fno;
  DIR dir;
  char *fn;
  
  res = f_opendir(&dir, path);
  if (res == FR_OK) {
    
    for (;;) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) break;
      if (fno.fname[0] == '.') continue;

      fn = fno.fname;
 
      if (fno.fattrib & AM_DIR) 
      {
        continue;
      } 
      else 
      {
        if((strstr(fn, "bmp")) || (strstr(fn, "BMP")))
        {
          res = f_open(&file, fn, FA_OPEN_EXISTING | FA_READ);
          
          USB_OTG_BSP_mDelay(100);
          ret = 0;
          f_close(&file);
          
        }
      }
    }  
  }
  //USBH_USR_ApplicationState = USH_USR_FS_READLIST;
  return ret;
}
#endif
/**
* @brief  Show_Image 
*         Displays BMP image
* @param  None
* @retval None
*/


/**
* @brief  Toggle_Leds
*         Toggle leds to shows user input state
* @param  None
* @retval None
*/

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}
/*************************************************************
** ��������: EraseUpdataFlashArea
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
#if 0
static void USB_USR_TestReadAndWrite(void)
{
    #define USB_DEBUG_TEST
    #ifdef USB_DEBUG_TEST
    FRESULT res;
    uint8_t writeTextBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS   ";
    uint16_t bytesWritten, bytesToWrite;
    uint8_t readTextBuff[64] ;
   // char apcfileName[16]="0:STM.TXT";
    uint16_t bytesRead,filesize,tempbytes;
    //if(f_open(&file, "0:STM32.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    if(f_open(&file, ff_NameConver("0:����.TXT"),FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    { 
        /* Write buffer to file */
        bytesToWrite = sizeof(writeTextBuff); 
        res= f_write (&file, writeTextBuff, 20, (void *)&bytesWritten);   

        if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
        {
        //LCD_ErrLog("> STM32.TXT CANNOT be writen.\n");
        }
        else
        {
        //LCD_UsrLog("> 'STM32.TXT' file created\n");
        }

        /*close file and filesystem*/
        f_close(&file);

    }

    else
    {
        //LCD_UsrLog ("> STM32.TXT created in the disk\n");
    }
    ///////////
   // res=f_open(&file, "0:STM32.TXT",FA_READ );
    res=f_open(&file, ff_NameConver("0:����.TXT"),FA_READ );
    if(res == FR_OK)
    {
        filesize=file.fsize;
        if(filesize>10)filesize-=10;
        //f_lseek(&file,10);  //�ƶ��ļ�ָ��
        bytesRead=0;
        res= f_read (&file, (unsigned char *)&readTextBuff[bytesRead], 5, (void *)&tempbytes); 
        bytesRead+=tempbytes;
        readTextBuff[bytesRead]='\0';
        filesize -=tempbytes;
        f_close(&file);
    } 
    #if 1//
    {
        
        uint8_t buff2[]="safdsafewqrewq";
        unsigned long b;
        f_unlink(ff_NameConver("0:STM.TXT"));   //ɾ���ļ�
        b = f_open(&file,ff_NameConver("0:STM.TXT"),FA_CREATE_NEW);    //�������ļ�
        f_close(&file);    //�ر��ļ�
        b = f_open(&file,ff_NameConver("0:STM.TXT"), FA_WRITE);   //��д��ʽ���ļ�
        //f_puts((char *)buff2,&file);  //�ļ���д���ַ���
        //f_puts((char *)buff2,&file);  //�ļ���д���ַ���
        bytesToWrite = sizeof(buff2);
        f_write(&file, buff2,bytesToWrite, (void *)&bytesWritten); //�ļ���д���ַ���
        f_close(&file);  //�ر��ļ�
        b = f_open(&file,ff_NameConver("0:STM.TXT"),FA_WRITE);   //��д��ʽ���ļ�
        b = file.fsize;       //����ļ���С
        f_lseek(&file,b);  //�ƶ��ļ�ָ��
        bytesToWrite=sizeof(writeTextBuff);
        f_write(&file,writeTextBuff,bytesToWrite, (void *)&bytesWritten);  //���ļ������ݵ����д���ַ���
        f_close(&file);    //�ر��ļ�
        b = f_open(&file,ff_NameConver("0:STM.TXT"),FA_READ);  //�Զ���ʽ���ļ�
        f_read(&file,readTextBuff,50,(void *)&bytesWritten);  //���ļ��ڶ�50�ֽڸ��� readTextBuff����
        f_close(&file);   //�ر��ļ�
        // f_unlink(apcfileName);   //ɾ���ļ�
    }
    #endif
    #endif
}
#endif
/*************************************************************
** ��������: EraseUpdataFlashArea
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
#if 0
static unsigned char USB_ReadAndUpdataMultimedia(void)
{
    uint8_t readBuffer[120] = {0};
    FRESULT res;
    UINT ReadByte;
    s32 length;   
    u32 WriteAddr;    
    //******************д����Ƶ****************   
    if(FR_OK == f_open(&file,"audio2.mp3",FA_READ))
    {
        ReadByte = 0;
        length = 0;
        //WriteAddr = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
        //������Ƶ����
        //EraseAudioFlashArea();
        for(;;)
        {
            //��ͼƬ���ݣ�ÿ��100�ֽ�
            res = f_read(&file,readBuffer,100,&ReadByte);
            //�����ۼ�
            length += ReadByte;
            //�ж��Ƿ�Ϊ���һ�����ݿ�
            if(ReadByte != 100)
            {
                break;
            }
            //д��FLASH��
            SPI_FLASH_BufferWrite(readBuffer, WriteAddr, 100);
            //����д�������
            //SPI_FLASH_BufferRead(textFileBuffer2, WriteAddr, 100);
            WriteAddr += 100;   
        }
        if(0 != ReadByte)
        {
            //д���һ�����ݿ�
            SPI_FLASH_BufferWrite(readBuffer, WriteAddr, ReadByte);
            //����д�������
            //SPI_FLASH_BufferRead(textFileBuffer2, WriteAddr, ReadByte);
        }
        //д�����ֽ�
        readBuffer[0] = (length & 0xff000000) >> 24;
        readBuffer[1] = (length & 0xff0000) >> 16;
        readBuffer[2] = (length & 0xff00) >> 8;
        readBuffer[3] = length;
        //WriteAddr = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
        //д�볤��
        SPI_FLASH_BufferWrite(readBuffer, WriteAddr, 4);
        //����д�������
        //SPI_FLASH_BufferRead(textFileBuffer2, WriteAddr, 4);
    }
    f_close(&file);



    //******************д����Ƶ****************

    if(FR_OK == f_open(&file,"video2.avi",FA_READ))
    {
        ReadByte = 0;
        length = 0;
        //WriteAddr = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
        //������Ƶ����
        //EraseVideoFlashArea();
        for(;;)
        {
            //��ͼƬ���ݣ�ÿ��100�ֽ�
            res = f_read(&file,readBuffer,100,&ReadByte);
            //�����ۼ�
            length += ReadByte;
            //�ж��Ƿ�Ϊ���һ�����ݿ�
            if(ReadByte != 100)
            {
                break;
            }
            //д��FLASH��
            SPI_FLASH_BufferWrite(readBuffer, WriteAddr, 100);
            //����д�������
            //SPI_FLASH_BufferRead(textFileBuffer2, WriteAddr, 100);
            WriteAddr += 100;   
        }
        if(0 != ReadByte)
        {
            //д���һ�����ݿ�
            SPI_FLASH_BufferWrite(readBuffer, WriteAddr, ReadByte);
            //����д�������
            //SPI_FLASH_BufferRead(textFileBuffer2, WriteAddr, ReadByte);
        }
        //д�����ֽ�
        readBuffer[0] = (length & 0xff000000) >> 24;
        readBuffer[1] = (length & 0xff0000) >> 16;
        readBuffer[2] = (length & 0xff00) >> 8;
        readBuffer[3] = length;
        //WriteAddr = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
        //д�볤��
        SPI_FLASH_BufferWrite(readBuffer, WriteAddr, 4);
        //����д�������
        //SPI_FLASH_BufferRead(textFileBuffer2, WriteAddr, 4);
    }
    f_close(&file);
    return 0;
}
#endif
/*************************************************************
** ��������: EraseUpdataFlashArea
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
static void EraseUpdataFlashArea(void)
{
    u16 i;
    u32 Address;
    for(i = FLASH_UPDATE_START_SECTOR; i <= FLASH_UPDATE_END_SECTOR; i++)
    {
        Address = i*FLASH_ONE_SECTOR_BYTES;
        SPI_FLASH_SectorErase(Address);
        //ÿ���һ������ι��һ��
        IWDG_ReloadCounter();
    }
}
/*************************************************************
** ��������: USB_ReadAndUpdataZhiku
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
#if 0//dxl,2014.9.26
static unsigned char USB_ReadAndUpdataSystem(void)
{
    UINT cuReadLen,readlen;
    unsigned long CalCrcCode;
    unsigned long ReadCrcCode;
    unsigned long WriteAddr;
    s32 length;
    s32 filelength;
    s16 i = 0;
    s16 j = 0;
    s16 k = 0;
    unsigned char cnt;
    FRESULT res;
    unsigned char readBuffer[513] = {0};  
    u8 Buffer[23] = {0};
    unsigned char temp[7];
    //////////////////////////////////////
    
    //****************��������****************
    if(FR_OK != f_open(&file,ff_NameConver("updata.txt"),FA_READ))
    {
        return 0;
    }
    ////////////////////
    readlen=file.fsize;
    if(readlen>32||readlen<5)return 0;
    res = f_read(&file,readBuffer,readlen,&cuReadLen);
    if(FR_OK !=res)return 0; ////////
    f_close(&file); 
    readBuffer[cuReadLen]='\0';
    ///////////////////////////////
    if(strncmp((char const *)&readBuffer[cuReadLen-4],".bin",4)!=0)return 0;
    //��1������ļ���С
    if(FR_OK == f_open(&file,ff_NameConver(readBuffer),FA_READ))
    //if(FR_OK == f_open(&file,"updata.bin",FA_READ))
    {
        //�жϴ�С     
        filelength = file.fsize - 22;
        WriteAddr=(FLASH_UPDATE_END_SECTOR-FLASH_UPDATE_START_SECTOR)*FLASH_ONE_SECTOR_BYTES;
        if(filelength > 512000||filelength<10240 || filelength > WriteAddr)
        {
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
            return 0;//����
        }
        //����Ϊ���Ӱ汾��֧��⹦�ܶ����,�����Ʒ�ͺźͰ汾��֧����,������,dxl,2014.6.5
       //,2�ֽڹ�˾��ʶ,8�ֽڲ�Ʒ��ʶ,2�ֽ�PCB�汾,6�ֽڹ̼��汾,4�ֽ�CRCУ����
        res = f_lseek(&file, file.fsize-22);
        if(FR_OK !=res)return 0; ////////
        res = f_read(&file,Buffer,22,&cuReadLen);
        //if((0 == strncmp((char const *)Buffer+12,SOFTWARE_VERSION+17,4))//dxl,2014.8.28,ȥ���̼������汾��֧����
           if((0 == strncmp((char const *)Buffer+2,PRODUCT_MODEL+14,6))
           &&(FR_OK == res))//�汾�ŵ�ǰ4λ������ͬ���������
        {
            
        }
        else
        {
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
            return 0;
        }
        //ָ���ļ���ʼλ��
        res = f_lseek(&file, 0);
        if(FR_OK !=res)return 0; ////////
        //����CRC,����У����
        MOD_InitCRC();
        cuReadLen = 0;
        length = 0;
        cnt =0;
        readlen =512;
        j = file.fsize/readlen;
        k = file.fsize%readlen;
        if((0 != k)&&(k >= 22))
        {
            j++;
        }
        else if((0 != k)&&(k < 22))
        {
            //j--;//�����һ���ϲ���ǰ��һ��һ���ȡ
        }
        for(i=0; i<j; i++)
        {
            if(i == (j-1))//���һ��
            {
                //��һ������
                if(0 == k)
                {
                    readlen = 512;
                }
                else if(k < 22)
                {
                    readlen = k+512;
                }
                else
                {
                    readlen = k;
                }
                res = f_read(&file,readBuffer,readlen,&cuReadLen); 
                if(FR_OK ==res)
                {
                    CalCrcCode = MOD_CalcCRC(readBuffer, readlen-22);
                }
                else
                {
                    Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
                    return 0;
                }
            }
            else
            {
                //��һ������
                readlen = 512;
                res = f_read(&file,readBuffer,readlen,&cuReadLen); 
                if(FR_OK ==res)
                {
                    CalCrcCode = MOD_CalcCRC(readBuffer, cuReadLen);
                }
                else
                {
                    Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
                    return 0;
                }
            }
            IWDG_ReloadCounter();
        }
        MOD_StopCRC();
        //�˶�У����
        //ָ���ļ���ʼλ��
        ReadCrcCode = 0;
        ReadCrcCode |= readBuffer[readlen-4] << 24;
        ReadCrcCode |= readBuffer[readlen-3] << 16;
        ReadCrcCode |= readBuffer[readlen-2] << 8;
        ReadCrcCode |= readBuffer[readlen-1];
        if(ReadCrcCode == CalCrcCode)
        {
            
        }
        else
        {
             Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
             return 0;
        }
        
                
        ////��3��д���ļ�//////////
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA);
        //�Ȳ����洢�����FLASH�ռ�
        EraseUpdataFlashArea();
         //ָ���ļ���ʼλ��
        res = f_lseek(&file, 0);
        if(FR_OK !=res)return 0; ////////
        //д��Ƭ��flash
        WriteAddr = FLASH_UPDATE_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
        cuReadLen = 0;
        length = 0;
        cnt =0;
        readlen =512;
        j = file.fsize/readlen;
        k = file.fsize%readlen;
        if((0 != k)&&(k >= 22))
        {
            j++;
        }
        else if((0 != k)&&(k < 22))
        {
            //j--;//�����һ���ϲ���ǰ��һ��һ���ȡ
        }
        for(i=0; i<j; i++)
        {
            if(i == (j-1))//���һ��
            {
                //��һ������
                if(0 == k)
                {
                    readlen = 512;
                }
                else if(k < 22)
                {
                    readlen = k+512;
                }
                else
                {
                    readlen = k;
                }
                res = f_read(&file,readBuffer,readlen,&cuReadLen); 
                if(FR_OK ==res)
                {
                    //CalCrcCode = MOD_CalcCRC(readBuffer, cuReadLen);
                    //д�뵽FLASH
                    SPI_FLASH_BufferWrite(readBuffer, WriteAddr, cuReadLen);
                    WriteAddr += readlen;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                //��һ������
                readlen = 512;
                res = f_read(&file,readBuffer,readlen,&cuReadLen); 
                if(FR_OK ==res)
                {
                    //CalCrcCode = MOD_CalcCRC(readBuffer, cuReadLen);
                    //д�뵽FLASH
                    SPI_FLASH_BufferWrite(readBuffer, WriteAddr, cuReadLen);
                    WriteAddr += readlen;
                }
                else
                {
                    return 0;
                }
            }
          
            //////////////////
            IWDG_ReloadCounter();
            ///////////////////
        }
        /////////////////////
    	cnt = 0;
        length = file.fsize - 22;
        temp[0] = length>>24;
        temp[1] = length>>16;
        temp[2] = length>>8;
        temp[3] = length&0xff;
        for (i=0; i<4; i++)
        {
    		cnt += temp[i];
            E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR + i, temp[i]) ;
        }
    	E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR + i, cnt);
        //������־
        E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR, 0xaa);  
        E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR+1, 0xaa);  
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_FINISH);
        f_close(&file);
	
    }
    f_close(&file);
    return 0;
}
#endif
static unsigned char USB_ReadAndUpdataSystem(void)
{
    unsigned long ulcrcCode;
    unsigned long readcrcCode;
    UINT cuReadLen,readlen;
    unsigned long length;
    unsigned long filelength;
    unsigned long WriteAddr;
    unsigned long ReadAddr;
    unsigned char temp[7];
    unsigned short i,j,k;
    unsigned char cnt;
    FRESULT res;
    unsigned char readBuffer[513] = {0};   
    //////////////////////////////////////
    
    //****************��������****************
    if(FR_OK != f_open(&file,ff_NameConver("updata.txt"),FA_READ))
    {
        return 0;
    }
    ////////////////////
    readlen=file.fsize;
    if(readlen>64||readlen<5)return 0;
    res = f_read(&file,readBuffer,readlen,&cuReadLen);
    if(FR_OK !=res)return 0; ////////
    f_close(&file); 
    readBuffer[cuReadLen]='\0';
    ///////////////////////////////
    if(strncmp((char const *)&readBuffer[cuReadLen-4],".bin",4)!=0)return 0;

    if(FR_OK == f_open(&file,ff_NameConver(readBuffer),FA_READ))
    //if(FR_OK == f_open(&file,"updata.bin",FA_READ))
    {
        //��һ��������ļ���С�Ͱ汾��־��       
        filelength = file.fsize;
        filelength-=22;
        WriteAddr=(FLASH_UPDATE_END_SECTOR-FLASH_UPDATE_START_SECTOR)*FLASH_ONE_SECTOR_BYTES;
        if(filelength > 512000||filelength<10240 || filelength > WriteAddr)
        {
            return 0;//����
        }
        res = f_lseek(&file, filelength);
        if(FR_OK !=res)return 0; ////////
        res = f_read(&file,readBuffer,22,&cuReadLen);
        if(FR_OK !=res|| cuReadLen!=22)return 0; //////////
        //dxl,2015.8,if(0==Public_CheckSystemLogoAndVerSion(readBuffer,&readcrcCode))
        //{
            //return 0; 
        //}
        //�ڶ��������У����
        res = f_lseek(&file, 0);
        if(FR_OK !=res)return 0; ////////
        //����CRC
       //dxl,2015.8, MOD_InitCRC();
        cuReadLen = 0;
        length = 0;
        cnt =0;
        readlen =512;
        for(;filelength>0;)
        {
            //��һ������
            res = f_read(&file,readBuffer,readlen,&cuReadLen); 
            if(FR_OK ==res&&cuReadLen)
            {
                cnt =0;
                //�����ۼ�
                length += cuReadLen;
                ///////////////////////////
                if(filelength>cuReadLen)
                {
                    filelength -=cuReadLen;
                }
                else filelength = 0;
                ////////////////////
                if(cuReadLen)
                //dxl,2015.8,ulcrcCode=MOD_CalcCRC(readBuffer, cuReadLen);

                //////////////////////
                if(filelength<readlen)readlen=filelength;
            }
            else////�����󣬳�������
            {
                cnt++;
                if(cnt>3)return 0; 
            }
            //////////////////
            IWDG_ReloadCounter();
            ///////////////////
        }
        ////////////////////
       //dxl,2015.8, MOD_StopCRC();
        ///////////////////////////
        filelength = file.fsize-22;
        if((length != filelength)||(readcrcCode != ulcrcCode))
    	//if(length != filelength)
        {
            return 0; 
        }
        //��3������U�̿����ļ���flash
        res = f_lseek(&file, 0);
        if(FR_OK !=res)return 0; ////////
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA);
        //�Ȳ����洢�����FLASH�ռ�
        EraseUpdataFlashArea();
        //���������ļ�,д�뵽FLASH
        //����CRC
        //dxl,2015.8,MOD_InitCRC();
        cuReadLen = 0;
        length = 0;
        cnt =0;
        ////////////////////
        readlen =512;
        /////////////////
        WriteAddr = FLASH_UPDATE_START_SECTOR*FLASH_ONE_SECTOR_BYTES;//��ʼ��ַ
        ////////////////////////
        for(;filelength>0;)
        {
            //��һ������
            res = f_read(&file,readBuffer,readlen,&cuReadLen); 
            if(FR_OK ==res&&cuReadLen)
            {
                cnt =0;
                //�����ۼ�
                length += cuReadLen; 
                if(filelength>cuReadLen)
                {
                    filelength -=cuReadLen;
                }
                else filelength = 0;
                //д�뵽FLASH
                SPI_FLASH_BufferWrite(readBuffer, WriteAddr, cuReadLen);
                //��ַ�ۼ�
                WriteAddr += cuReadLen;
                ////////////////
                if(cuReadLen)
                {
                   //dxl,2015.8, ulcrcCode=MOD_CalcCRC(readBuffer, cuReadLen);
                }
                /////////////////
                if(filelength<readlen)readlen = filelength;
                /////////////////////////
            }
            else//�����󣬳�������
            {
                cnt++;
                if(cnt>3)
                {
                    Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
                    return 0; 
                }
            }
            //////////////////
            IWDG_ReloadCounter();
            ///////////////////
        }
        ////////////////////
        //dxl,2015.8,MOD_StopCRC(); 
        ///////////////
        filelength = file.fsize;
        ///////�ڽ�βд��汾��Ϣ///////////////////////////////
        res = f_read(&file,readBuffer,22,&cuReadLen);
        if(FR_OK !=res|| cuReadLen!=22)
        {
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
            return 0; //////////
        }
        length += cuReadLen; 
        //д�뵽FLASH
        SPI_FLASH_BufferWrite(readBuffer, WriteAddr, cuReadLen);
        /////////////////////////////////////////
        if((length != filelength)||(readcrcCode != ulcrcCode))
        //if(length != filelength)
        {
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_ERROR);
            return 0; 
        }
        ////////////////////
        f_close(&file);
        
        //���Ĳ������flash����ļ�
        ReadAddr = FLASH_UPDATE_START_SECTOR*FLASH_ONE_SECTOR_BYTES;//��ʼ��ַ
        //dxl,2015.8,MOD_InitCRC();
        filelength -= 22;//ȥ�����ӵ�22�ֽڱ�ʶ��
        if(0 == filelength%512)
        {
            k = filelength/512;
        }
        else
        {
            k = filelength/512+1;
        }
        for(j =0; j<k; j++)
        {
            if(j == (k-1))//���һ��
            {
                if(0 == filelength%512)
                {
                    cuReadLen = 512;
                }
                else
                {
                    cuReadLen = filelength%512;
                }
            }
            else
            {
                cuReadLen = 512;
            }
            //��һ������
            SPI_FLASH_BufferRead(readBuffer, ReadAddr, cuReadLen);
            //dxl,2015.8,ulcrcCode=MOD_CalcCRC(readBuffer, cuReadLen);
            IWDG_ReloadCounter();
            ReadAddr += cuReadLen;
        }
        ////////////////////
        //dxl,2015.8,MOD_StopCRC();
        SPI_FLASH_BufferRead(readBuffer, ReadAddr, 22);//�����ӵ�22�ֽ�
        //dxl,2015.8,if(0==Public_CheckSystemLogoAndVerSion(readBuffer,&readcrcCode))
        //{
            //return 0; 
        //}
        if(readcrcCode != ulcrcCode)
        {
            return 0; 
        }
        else
        {
    	    cnt = 0;
            temp[0] = length>>24;
            temp[1] = length>>16;
            temp[2] = length>>8;
            temp[3] = length&0xff;
            for (i=0; i<4; i++)
            {
                cnt += temp[i];
                E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR + i, temp[i]) ;
            }
    	    E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR + i, cnt);
            //������־
            E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR, 0xaa);  
            E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR+1, 0xaa);  
        
           //dxl,2015.8, ClrTimerTask(TIME_POSITION);//���������йر�λ����Ϣ�ϱ���ä��,ԭ���Ƿ�ֹ��ͬһ��flash����д
            //dxl,2015.8,ClrTimerTask(TIME_BLIND);
             
            /////////////////////////
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_UPDATA_FINISH);
            ///////////////////////
        }
    }
    f_close(&file);
    return 0;
}
/*************************************************************
** ��������: USB_UpdateZhiKu
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void USB_UpdateZhiKu(void)
{
	FIL file;
	u32 length = 0;
	u8	Buffer[513];
	u32	BufferLen;
	u32	i;
	u32	WriteAddr;
	u32	Delay;
	FRESULT res;
	
	
	if(FR_OK == f_open(&file,ff_NameConver((u8 *)"zhiku.bin"),FA_READ))
	{
		if((file.fsize > 256*1024)&&(file.fsize < 260*1024))
		{
			 for(i=FLASH_GB2312_16_START_SECTOR; i<FLASH_GB2312_16_END_SECTOR; i++)
        		{
           	 		WriteAddr = i*FLASH_ONE_SECTOR_BYTES;
            			SPI_FLASH_SectorErase(WriteAddr);
            			//ÿ���һ������ι��һ��
            			IWDG_ReloadCounter();
        		}
			
			res = f_lseek(&file, 0);//��ԭ�㿪ʼ
        		if(FR_OK !=res)return ; 
			
			WriteAddr = 0;//��ʼ��ַ
			for(;;)
			{
				//ÿ���һ������ι��һ��
            			IWDG_ReloadCounter();
				res = f_read(&file,Buffer,512,&BufferLen); 
            			if((FR_OK ==res)&&BufferLen)
				{
					//д�뵽FLASH
                			SPI_FLASH_BufferWrite(Buffer, WriteAddr, BufferLen);
                			//��ַ�ۼ�
                			WriteAddr += BufferLen;
					//�����ۼ�
					length += BufferLen;
					if(length >= file.fsize)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		if(length == file.fsize)
		{
			//ÿ���һ������ι��һ��
            		IWDG_ReloadCounter();
			//dxl,2015.8,BEEP_ON();
			//dxl,2015.8,LCD_LIGHT_ON();
			Delay  = SysTick_GetCounter();
			while((SysTick_GetCounter() - Delay) < 10)
			{;}//��ʱ0.5s
			//dxl,2015.8,BEEP_OFF();
			 //dxl,2015.8,Public_ShowTextInfo("�ֿ������ɹ�,��γ�U��",100);
			//dxl,2015.8,LCD_LIGHT_OFF();
			
			WriteAddr = 0;//Ϊ�˵��Զ���
		}
		
	}
	f_close(&file);
}
/*************************************************************
** ��������: USB_ReadAndUpdataZhiku
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
#if 0
static unsigned char USB_ReadAndUpdataZhiku(void)
{
    uint8_t flag;
    uint8_t readBuffer[513] = {0};
    u32 CrcCode;
    u32 readcrcCode;
    s16 CrcByte;
    UINT cuReadLen,readlen;
    s32 length;
    s32 filelength;
    u32 WriteAddr;
    s8 i;
    s32 j;
    FRESULT res;
    unsigned char cnt;
    //***************д���ֿ�***************
    //��1������ļ���С
    flag = 0;
    if(FR_OK == f_open(&file,"zhiku.bin",FA_READ))
    {
        //���������ļ�,д�뵽FLASH
        cuReadLen = 0;
        filelength = file.fsize;
        if(filelength >= 0x41000)
        {
            return 0; 
        }
        res = f_lseek(&file, filelength);
        if(FR_OK !=res)return 0; ////////
        res = f_read(&file,readBuffer,22,&cuReadLen);
        if(FR_OK !=res|| cuReadLen!=22)return 0; //////////
        if(0==USB_CheckSystemLogoAndVerSion(readBuffer,&readcrcCode))
        {
            return 0; 
        }
        //���У����
        res = f_lseek(&file, 0);
        if(FR_OK !=res)return 0; ////////
        //����CRC
        MOD_InitCRC();
        cuReadLen = 0;
        length = 0;
        cnt =0;
        readlen =512;
        for(;filelength>0;)
        {
            //��һ������
            res = f_read(&file,readBuffer,readlen,&cuReadLen); 
            if(FR_OK ==res&&cuReadLen)
            {
                cnt =0;
                //�����ۼ�
                length += cuReadLen;
                ///////////////////////////
                if(filelength>cuReadLen)
                {
                    filelength -=cuReadLen;
                }
                else filelength = 0;
                ////////////////////
                if(cuReadLen)
                CrcCode=MOD_CalcCRC(readBuffer, cuReadLen);

                //////////////////////
                if(filelength<readlen)readlen=filelength;
            }
            else//
            {
                cnt++;
                if(cnt>3)return 0; 
            }
            //////////////////
            IWDG_ReloadCounter();
            ///////////////////
        }
        ////////////////////
        MOD_StopCRC();
        ///////////////////////////
        filelength = file.fsize-22;
        if((length != filelength)||(readcrcCode != CrcCode))
        {
            return 0; 
        }
        ////��3��д���ļ�//////////
        res = f_lseek(&file, 0);
        if(FR_OK !=res)return 0; ////////
        //�Ȳ����洢�����FLASH�ռ�
        for(i=0; i<=64; i++)
        {
            WriteAddr = i*FLASH_ONE_SECTOR_BYTES+2;
            SPI_FLASH_SectorErase(WriteAddr);
            //ÿ���һ������ι��һ��
            IWDG_ReloadCounter();
        }
        //���������ļ�,д�뵽FLASH
        //����CRC
        MOD_InitCRC();
        cuReadLen = 0;
        length = 0;
        cnt =0;
        ////////////////////
        readlen =512;
        /////////////////
        WriteAddr = 0;//��ʼ��ַ
        ////////////////////////
        for(;filelength>0;)
        {
            //��һ������
            res = f_read(&file,readBuffer,readlen,&cuReadLen); 
            if(FR_OK ==res&&cuReadLen)
            {
                cnt =0;
                //�����ۼ�
                length += cuReadLen; 
                if(filelength>cuReadLen)
                {
                    filelength -=cuReadLen;
                }
                else filelength = 0;
                //д�뵽FLASH
                SPI_FLASH_BufferWrite(readBuffer, WriteAddr, cuReadLen);
                //��ַ�ۼ�
                WriteAddr += cuReadLen;
                ////////////////
                if(cuReadLen)
                {
                    CrcCode=MOD_CalcCRC(readBuffer, cuReadLen);
                }
                /////////////////
                if(filelength<readlen)readlen = filelength;
                /////////////////////////
            }
            else//
            {
                cnt++;
                if(cnt>3)return 0; 
            }
            //////////////////
            IWDG_ReloadCounter();
            ///////////////////
        }
        ////////////////////
        MOD_StopCRC(); 
        ///////////////
        filelength = file.fsize;
        ///////�ڽ�βд��汾��Ϣ///////////////////////////////
        res = f_read(&file,readBuffer,22,&cuReadLen);
        if(FR_OK !=res|| cuReadLen!=22)return 0; //////////
        length += cuReadLen; 
        //д�뵽FLASH
        SPI_FLASH_BufferWrite(readBuffer, WriteAddr, cuReadLen);
        /////////////////////////////////////////
        if((length != filelength)||(readcrcCode != CrcCode))
        {
            return 0; 
        }
        ////////////////////
        f_close(&file);
        for(i=0; i<10; i++)
        {
            IWDG_ReloadCounter();
            for(j=0; j<5000000; j++)
            {

            }
        }
        //NVIC_SystemReset();dxl,2014.9.25
        
    }
    f_close(&file);
    return 0;
}
#endif





/*************************************************************
** ��������: USB_USR_CurrentApplication
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
#if 0
static void USB_USR_CurrentTestDebug(void)
{
    u8 FileName1[_MAX_LFN];
    FIL File1;
    u8 FileName2[_MAX_LFN];
    FIL File2;
    u8 Buffer[1001];
    u32 ReadLen;
    u32 Bw;
    u32 length;
    u32	delay;
    //��SD�����ļ�,WAV17\11060946.wav
    strcpy((char *)FileName1,"1:/WAV17/11060946.wav");
    //strcpy((char *)FileName1,"1:/JPG12/20015901.jpg");
    length = 0;
    if(f_open(&File1,(char const *)FileName1,FA_READ) == FR_OK)  //��SD���ļ�
    {
        //U�̸�Ŀ¼������ͬ�ļ������ļ�
        strcpy((char *)FileName2,(char const *)FileName1+9);
        if(f_open(&File2,(char const *)FileName2,FA_OPEN_ALWAYS|FA_READ|FA_WRITE) == FR_OK)
        {

        }

    }
    for(;;)
    {
        //��100���ֽ�
        if(FR_OK == f_read(&File1,Buffer,1000,&ReadLen))
        {
            for(delay=0; delay<100000; delay++)
            {

            }
            //д��U���ļ���
            if(f_write(&File2,Buffer,ReadLen,&Bw)== FR_OK) //д�ļ�
            {
                length += Bw;
                if(length >= File1.fsize)
                {
                    break;
                }
            }
        }
    }
    f_close(&File1);
    f_close(&File2);
}
#endif
/*************************************************************
** ��������: USB_USR_CurrentApplication
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
static void USB_USR_ReadDataForUpdataApp(void)
{
    //�����ֿ�
    USB_UpdateZhiKu();
    //USB_USR_TestReadAndWrite();
    //if(USB_GetReadUsbUpdataFlag())return;
    //�����̼�
    USB_ReadAndUpdataSystem();    
    f_close(&file);
    
}

/*************************************************************
** ��������: USB_USR_CheckInsertUsbDisk
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char USB_USR_CheckInsertUsbDisk(void)
{
    if((USH_USR_FS_WRITEFILE!=USBH_USR_ApplicationState)&&(USH_USR_FS_DRAW!=USBH_USR_ApplicationState))
    {
        return 0;
    }
    //////////////////////////
    return 1;
}
/*************************************************************
** ��������: USB_USR_DelayUs
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void USB_USR_DelayUs(unsigned long delay)
{
    for(; delay>0; delay--)
    {

    }
}
/*************************************************************
** ��������: USB_USR_WriterDataToUsbDiskApp
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char USB_USR_WriterDataToUsbDiskApp(const TCHAR* readfileName,const TCHAR* writerfileName)
{
    FIL file1;
    FRESULT res;    
    UINT readlen;  
    UINT writerlen; 
    unsigned long filesize;
    unsigned char s_ucUsbBuffer[1040];
    
    res = f_open(&file1,readfileName, FA_READ);     
    if(FR_OK!=res)
    {
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_READ_FILE_ERROR);
        f_close(&file1);  //�ر��ļ�  
        return 0;
    }
    /////////////////////////////////////   
    res = f_open(&file,writerfileName, FA_CREATE_ALWAYS);
    if(FR_WRITE_PROTECTED==res)
    {
        f_close(&file);  //�ر��ļ�  
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_PROTECTED);
        return 0;
    }
    f_close(&file);  //�ر��ļ�  
    //////////////////////////////
    res = f_open(&file,writerfileName, FA_WRITE);
    if(FR_OK==res)
    {
        filesize=file1.fsize;
        while(filesize)
        {
            res = f_read(&file1,s_ucUsbBuffer,1024, (void *)&readlen);  //���ļ������ݵ����д���ַ��� 
            if(readlen)
            {
                USB_USR_DelayUs(1000);
                res = f_write(&file,s_ucUsbBuffer,readlen, (void *)&writerlen);  //���ļ������ݵ����д���ַ���       
            }
            else
            {
                break;
            }
            ////////////////////////
            filesize-=readlen;
            IWDG_ReloadCounter();
        }
        f_close(&file);  //�ر��ļ� 
        f_close(&file1);  //�ر��ļ� 
        return 1;
    }
    else
    if(FR_WRITE_PROTECTED==res)
    {
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_PROTECTED);
    }
    ////////////////////////////////
    f_close(&file);  //�ر��ļ� 
    f_close(&file1);  //�ر��ļ� 
    return 0; 
    ////////////////////
     
}
/*************************************************************
** ��������: USB_USR_WriterDataToUsb
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char USB_USR_ReadAndWriteOnefileToUsb(unsigned char folder,unsigned char type,unsigned long ulTimeVal)
{
    TIME_T stTime;
    char fileSD[32];
    char fileUsb[32]; 
    Gmtime(&stTime,ulTimeVal);
    //dxl,2015.8,if(MEDIA_TYPE_WAV==type)//¼���ļ�������+��+ʱ+�֣���ѭ8.3�ļ�����ʽ
    {
        sprintf(fileUsb,"%02d%02d%02d%02d.wav",stTime.month,stTime.day,stTime.hour,stTime.min);
        sprintf(fileSD,"1:/WAV%d/%02d%02d%02d%02d.wav",folder,stTime.month,stTime.day,stTime.hour,stTime.min);
    }
    //dxl,2015.8,else
    //dxl,2015.8,if(MEDIA_TYPE_JPG==type)//ͼ���ļ�������+ʱ+��+�룬��ѭ8.3�ļ�����ʽ
    {
        sprintf(fileUsb,"%02d%02d%02d%02d.JPG",stTime.day,stTime.hour,stTime.min,stTime.sec);
        sprintf(fileSD,"1:/JPG%d/%02d%02d%02d%02d.JPG",folder,stTime.day,stTime.hour,stTime.min,stTime.sec);
    }
    //////////////////////////////
    return USB_USR_WriterDataToUsbDiskApp(ff_NameConver((u8 *)fileSD),ff_NameConver((u8 *)fileUsb));
}
/*************************************************************
** ��������: USB_USR_WriterDataToUsb
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void USB_USR_WriterDataToUsb(unsigned char folder,unsigned char folder2,unsigned char *buffer1,unsigned char *buffer2,unsigned char type)
{
		unsigned long ulTimeVal;
    unsigned char datalen;
    unsigned char result;
    unsigned char ErrCnt=0;
	
    if(!USB_USR_CheckInsertUsbDisk())
    {
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_NO_U_DISK);
        return;
    }
    ///////////////////////////
    
    datalen =0;
    for(;;)
    {
        if(ErrCnt>3)
        {
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_SAVE_FINISH);
            return;
        }
        else
        if(folder)
        {
            ulTimeVal=Public_ConvertBufferToLong(&buffer1[datalen]);
            if(ulTimeVal)
            {
                result=USB_USR_ReadAndWriteOnefileToUsb(folder,type,ulTimeVal);
                if(!result)ErrCnt++;
                datalen+=4;
            }
            else
            {
                folder=0;
                datalen = 0;
            }
        }
        else
        if(folder2)
        {
            ulTimeVal=Public_ConvertBufferToLong(&buffer2[datalen]);
            if(ulTimeVal)
            {
                result = USB_USR_ReadAndWriteOnefileToUsb(folder2,type,ulTimeVal);
                if(!result)ErrCnt++;
                datalen+=4;
            }
            else
            {
                folder2 =0;
            }
        }
        else
        {
            Usb_SetCurrentUsbStatus(USB_WORK_STATUS_SAVE_FINISH);
            return ;
        }
    }    
}
/*************************************************************
** ��������: USB_USR_CurrentApplication
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
static void USB_USR_CurrentApplication(void)
{
    USB_USR_ReadDataForUpdataApp();    
}
/*******************************************************************************
* Function Name  : ff_NameConver
* Description    : ���ڴ򿪳��ļ���������ff_open����ʱҪ��stringתΪ16λ����
*            ���磺f_open(&file,"12345.txt",FA_CREATE_ALWAYS | FA_WRITE)
*                  f_open(&file,ff_OpenConver("12345.txt"),FA_CREATE_ALWAYS | FA_WRITE)
* Input          : ָ��Ҫת��8λ����
* Output         : None
* Return         : ����8λ�ļ�����Ӧ��16λ����
*******************************************************************************/ 
u16* ff_NameConver(u8 *pStr)
{
    static u16 nTab[100];
    u8 i;
    
    memset(nTab,0,sizeof(nTab));
    
    for(i=0;i<100;i++)
    {
        nTab[i] = *(pStr+i);
        
        if(*(pStr+i) == 0)
        {
           break;
        }
    }
    
    return nTab;
}
 
 
/*******************************************************************************
* Function Name  : ff_NameForward
* Description    : ���ڴ򿪳��ļ�������Ҫ�ļ���ת��ʱ����
* Input          : - > *pDst  : ָ��Ҫת��8λ����
*                  - > *pSrc  : ָ��16λ�ļ����ַ�������
* Output         : None
* Return         : None
*******************************************************************************/ 
void ff_NameForward(u8 *pDst,u16 *pSrc)
{
    u8 i;  
    
    for(i=0;i<100;i++)
    {
        *(pDst+i) = (*(pSrc+i) & 0xff);
        
        if(*(pSrc+i) == 0)
        {
           break;
        }
    }
}
/*************************************************************
** ��������: USB_USR_CurrentApplication
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

