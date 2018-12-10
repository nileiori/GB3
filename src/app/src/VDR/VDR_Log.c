/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :VDR_Log.c      
//����      :ʵ����ʻ��¼����־��¼��ָ���¼������ 
//�汾��    :
//������    :dxl
//����ʱ��  :2015.11
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
//1.�ڼ�¼��־֮ǰ��Ҫ��FLASH_VDR_LOG_START_ADDR��FLASH_VDR_LOG_END_ADDR�ռ����
//2.��¼���Ȳ�������Ϊ��ͬ�������в�ͬ�ĳ��ȣ�Ŀ���ǰ����������¼����
//3.���ǵ�48h�ٶȣ�360hλ����Ϣ�������ܴ󣬼�¼�ռ䲻�������Ӧ��ָ��ֻ��¼ǰ�漸ʮ���ֽ�
//4.FLASH_VDR_LOG_START_ADDR��FLASH_VDR_LOG_END_ADDR�洢����־�����ټ�¼�ˣ����˹��ֶ�ִ�в���
//5.��Ҫ�˹��ֶ��������ܽ���¼����־��ӡ��DB9��
***********************************************************************/
//***************�����ļ�*****************
#include <integer.h>
//#include  "stm32f2xx.h"
//#include  "stm32f2xx_lib.h"
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"
#include  "VDR.h"
#include  "EepromPram.h"
#include  "Io.h"
#include  "VDR.h"
#include  "taskschedule.h"
#include "fm25c160.h"
#include "Public.h"
#include "spi_flashapi.h"
#include "Lcd.h"
#include "VDR_Usb.h"


//****************�궨��****************
//***************��������***************
//*****************��������****************
extern u8   GBTestFlag;//0:������������ģʽ;1:������ģʽ���ñ�־ͨ���˵���ѡ��
extern FIL file;
extern u8 VdrUsbReadBuffer[];

//u8 VdrLogPrintBuffer[600] = {0};Ϊ��ԼRAM��ʹ��VdrUsbReadBuffer����VdrLogPrintBuffer
u32 VdrLogPrintCount = 0;
u32 VdrLogWriteCount = 0;//ָ�����,��FLASH_VDR_LOG_START_ADDR��ʼ��FLASH_VDR_LOG_END_ADDR����

static u8 VdrLogInitFlag = 0;
static u8 VdrLogPrintEnableFlag = 0;//��־��ӡʹ�ܱ�־��1Ϊʹ�ܣ�0Ϊ��ֹ
static WCHAR filename[50];
//***************��������*******************
static u32 VDRLog_GetWriteCount(void);

//****************��������*****************
/*********************************************************************
//��������  :VDRLog_TimeTask
//����      :��ӡ��־���ݵ�DB9����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :ʼ�շ���ENABLE
//��ע��1�����1��
*********************************************************************/
FunctionalState VDRLog_TimeTask(void)
{
	
    //u32 Addr;
    
    if(0 == VdrLogInitFlag)
    {
      VdrLogWriteCount = VDRLog_GetWriteCount();
        VdrLogInitFlag = 1;
    }
    else
    {
      if((1 == VdrLogPrintEnableFlag)&&(1 == GBTestFlag))
      {
        //Addr = FLASH_VDR_LOG_START_ADDR+VdrLogPrintCount;
          //sFLASH_ReadBuffer(VdrUsbReadBuffer,Addr,500);ȡ�����ڴ�ӡ��־�ķ�ʽ����ΪU�̵����ķ�ʽ
            //VDRUsart_SendData(VdrUsbReadBuffer,500);
          VdrLogPrintCount += 500;
          if(VdrLogPrintCount >= VdrLogWriteCount)
          {
            VdrLogPrintEnableFlag = 0;
              VdrLogPrintCount = 0;
          }
      }
      else
      {
        VdrLogPrintCount = 0;
            VdrLogInitFlag = 0;
            return DISABLE;
      }
  }
    return ENABLE;
}
/*********************************************************************
//��������  :VDRLog_Write
//����      :дDVR��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :0���ɹ���1��ʧ��
*********************************************************************/
u8 VDRLog_Write(u8 *pBuffer, u16 BufferLen)
{
    u32 Addr;
    u8 Buffer[5];
    
    if(1 == GBTestFlag)
    {
    if((VdrLogWriteCount+BufferLen) > ((FLASH_VDR_LOG_END_SECTOR-FLASH_VDR_LOG_START_SECTOR)*FLASH_ONE_SECTOR_BYTES))
      {
        return 1;
      }
      else
      {
        Addr = FLASH_VDR_LOG_START_ADDR+VdrLogWriteCount;
          sFLASH_WriteBuffer(pBuffer, Addr, BufferLen);
          VdrLogWriteCount += BufferLen;
          Public_ConvertLongToBuffer(VdrLogWriteCount,Buffer);
          FRAM_BufferWrite(FRAM_VDR_LOG_COUNT_ADDR, Buffer, FRAM_VDR_LOG_COUNT_LEN);    
        FRAM_BufferWrite(FRAM_VDR_LOG_COUNT_BACKUP_ADDR, Buffer, FRAM_VDR_LOG_COUNT_BACKUP_LEN);
      }
  }
    return 0;
}
/*********************************************************************
//��������  :VDRLog_SaveLogFile
//����      :������־�ļ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
*********************************************************************/
u8 VDRLog_SaveLogFile(void)
{
    
      FRESULT res;
      u32   flen;
        u32 Addr;
    
        if(FR_OK != f_open(&file,ff_NameConver("log.txt"),FA_READ))
      {
                return 0;
      }
    
        LCD_LIGHT_ON();
    
        Vdr_Usb_UniNm2(filename);//�����ļ���
    
        if(f_open(&file,(WCHAR*)&filename,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
        {
                  res = f_lseek(&file, 0);//��ԭ�㿪ʼ
          if(FR_OK !=res)return 0; 
                
            LcdClearScreen();
            LcdShowCaption("������־,���Ժ�", 2);           
                  
                  for(VdrLogPrintCount = 0; VdrLogPrintCount < VdrLogWriteCount; )
            {
                Addr = FLASH_VDR_LOG_START_ADDR+VdrLogPrintCount;
                sFLASH_ReadBuffer(VdrUsbReadBuffer,Addr,500);
                f_write (&file, VdrUsbReadBuffer, 500, &flen); 
                        VdrLogPrintCount += 500;
            }
                  
            }
            f_close(&file);  
            
            return 1;
}
/*********************************************************************
//��������  :VDRLog_EraseFlashArea
//����      :����DVR��־��¼��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
*********************************************************************/
void VDRLog_EraseFlashArea(void)
{
    u32 i;
    u32 Addr;
    u8 Buffer[5];
    
  for(i=FLASH_VDR_LOG_START_SECTOR; i<FLASH_VDR_LOG_END_SECTOR; i++)
    {
        Addr = i*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Addr);
        IWDG_ReloadCounter();
    }
    
    VdrLogWriteCount = 0;
    Public_ConvertLongToBuffer(VdrLogWriteCount,Buffer);
    FRAM_BufferWrite(FRAM_VDR_LOG_COUNT_ADDR, Buffer, FRAM_VDR_LOG_COUNT_LEN);  
    FRAM_BufferWrite(FRAM_VDR_LOG_COUNT_BACKUP_ADDR, Buffer, FRAM_VDR_LOG_COUNT_BACKUP_LEN);
    
}
/*********************************************************************
//��������  :VDRLog_GetWriteCount
//����      :��ȡ��¼�����λ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��¼�����λ��
*********************************************************************/
static u32 VDRLog_GetWriteCount(void)
{
    u32 count = 0;
    u8  Buffer[5];
    
  if(FRAM_VDR_LOG_COUNT_LEN == FRAM_BufferRead(Buffer, FRAM_VDR_LOG_COUNT_LEN,FRAM_VDR_LOG_COUNT_ADDR))
    {
      count = Public_ConvertBufferToLong(Buffer);
    }
    else if(FRAM_VDR_LOG_COUNT_BACKUP_LEN == FRAM_BufferRead(Buffer, FRAM_VDR_LOG_COUNT_BACKUP_LEN,FRAM_VDR_LOG_COUNT_BACKUP_ADDR))
    {
      count = Public_ConvertBufferToLong(Buffer);
    }
    else
    {
      VDRLog_EraseFlashArea();
    }
    
    if(count > ((FLASH_VDR_LOG_END_SECTOR-FLASH_VDR_LOG_START_SECTOR)*FLASH_ONE_SECTOR_BYTES))
    {
      count = 0;
        VDRLog_EraseFlashArea();
    }
    
    return count;
}
/*********************************************************************
//��������  :VDRLog_EnablePrint
//����      :ʹ��DVR��־��ӡ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
*********************************************************************/
void VDRLog_EnablePrint(void)
{
  VdrLogPrintEnableFlag = 1;
    SetTimerTask(TIME_VDR_LOG, SECOND);
}
