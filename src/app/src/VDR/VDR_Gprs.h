#ifndef __VDR_GPRS_H
#define __VDR_GPRS_H

#include "stm32f10x.h"

#define VDR_GPRS_PARSE_BUFFER_SIZE    1024
#define VDR_GPRS_RX_BUFFER_SIZE        255

#define VDR_GPRS_RESEND_PACKET_MAX        100

/**************************************************************************
//函数名：VDRGprs_TimeTask
//功能：远程读取行驶记录仪数据
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒调度1次
***************************************************************************/
FunctionalState VDRGprs_TimeTask(void);
/**************************************************************************
//函数名：VDRGprs_ResendTimeTask
//功能：分包补传行驶记录仪数据
//输入：无
//输出：无
//返回值：分包没有传完时返回ENABLE，传完时返回DISABLE
//备注：1秒调度1次
***************************************************************************/
FunctionalState VDRGprs_ResendTimeTask(void);
/**************************************************************************
//函数名：VDRGprs_RecData
//功能：接收远程记录仪命令
//输入：无
//输出：无
//返回值：无
//备注：无
***************************************************************************/
void VDRGprs_RecData(u8 channel, u16 SerialNum, u8 *pBuffer, u16 BufferLen);

void VDRGprs_PacketAckPreHandle(u8 Type);
/**************************************************************************
//函数名：VDRGprs_PacketResendRequest
//功能：平台下发分包补传请求
//输入：无
//输出：无
//返回值：无
//备注：当收到平台下发的分包补传指令0x8003时，调用此函数
***************************************************************************/
void VDRGprs_PacketResendRequest(u8 channel, u8 *pBuffer, u16 BufferLen);
#endif
