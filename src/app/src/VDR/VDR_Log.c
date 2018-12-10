/********************************************************************
//版权说明  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称  :VDR_Log.c      
//功能      :实现行驶记录仪日志记录（指令记录）功能 
//版本号    :
//开发人    :dxl
//开发时间  :2015.11
//修改者    :
//修改时间  :
//修改简要说明  :
//备注      :
//1.在记录日志之前需要将FLASH_VDR_LOG_START_ADDR到FLASH_VDR_LOG_END_ADDR空间擦除
//2.记录长度不定，因为不同的命令有不同的长度，目的是把这条命令记录完整
//3.考虑到48h速度，360h位置信息数据量很大，记录空间不够，因此应答指令只记录前面几十个字节
//4.FLASH_VDR_LOG_START_ADDR到FLASH_VDR_LOG_END_ADDR存储满日志后不能再记录了，需人工手动执行擦除
//5.需要人工手动触发才能将记录的日志打印到DB9口
***********************************************************************/
//***************包含文件*****************
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


//****************宏定义****************
//***************常量定义***************
//*****************变量定义****************
extern u8   GBTestFlag;//0:正常出货运行模式;1:国标检测模式，该标志通过菜单可选择
extern FIL file;
extern u8 VdrUsbReadBuffer[];

//u8 VdrLogPrintBuffer[600] = {0};为节约RAM，使用VdrUsbReadBuffer代替VdrLogPrintBuffer
u32 VdrLogPrintCount = 0;
u32 VdrLogWriteCount = 0;//指针计数,从FLASH_VDR_LOG_START_ADDR开始到FLASH_VDR_LOG_END_ADDR结束

static u8 VdrLogInitFlag = 0;
static u8 VdrLogPrintEnableFlag = 0;//日志打印使能标志，1为使能，0为禁止
static WCHAR filename[50];
//***************函数声明*******************
static u32 VDRLog_GetWriteCount(void);

//****************函数定义*****************
/*********************************************************************
//函数名称  :VDRLog_TimeTask
//功能      :打印日志数据到DB9串口
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :始终返回ENABLE
//备注：1秒调度1次
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
          //sFLASH_ReadBuffer(VdrUsbReadBuffer,Addr,500);取消串口打印日志的方式，改为U盘导出的方式
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
//函数名称  :VDRLog_Write
//功能      :写DVR日志
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :0：成功；1：失败
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
//函数名称  :VDRLog_SaveLogFile
//功能      :保存日志文件
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
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
    
        Vdr_Usb_UniNm2(filename);//创建文件名
    
        if(f_open(&file,(WCHAR*)&filename,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
        {
                  res = f_lseek(&file, 0);//从原点开始
          if(FR_OK !=res)return 0; 
                
            LcdClearScreen();
            LcdShowCaption("导出日志,请稍候", 2);           
                  
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
//函数名称  :VDRLog_EraseFlashArea
//功能      :擦除DVR日志记录区
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
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
//函数名称  :VDRLog_GetWriteCount
//功能      :获取记录的起点位置
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :记录的起点位置
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
//函数名称  :VDRLog_EnablePrint
//功能      :使能DVR日志打印
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
*********************************************************************/
void VDRLog_EnablePrint(void)
{
  VdrLogPrintEnableFlag = 1;
    SetTimerTask(TIME_VDR_LOG, SECOND);
}
