#ifndef __VDR_USB_H
#define __VDR_USB_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include <integer.h>

#define VDR_USB_READ_BUFFER_SIZE  1024

/**************************************************************************
//函数名：VDRUsb_CreatVdrFile
//功能：生成*.VDR文件
//输入：无
//输出：无
//返回值：无
//备注：VDR文件的格式见GB/T19056-2012附录B
//该函数仅被USB_USR_ReadDataForUpdataApp函数调用
***************************************************************************/
void VDRUsb_CreatVdrFile(void);
void Vdr_Usb_UniNm2(WCHAR *p_name);
#endif