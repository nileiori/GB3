#ifndef __VDR_PROTOCOL_H
#define __VDR_PROTOCOL_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include "VDR_Usart.h"

#define VDR_PROTOCOL_PARSE_BUFFER_SIZE	VDR_USART_RX_BUFFER_SIZE	//接收缓冲最大1024字节
#define VDR_PROTOCOL_SEND_BUFFER_SIZE 	VDR_USART_TX_BUFFER_SIZE	//发送缓冲最大1024字节

#define VDR_PROTOCOL_CMD_00H	0x00
#define VDR_PROTOCOL_CMD_01H	0x01
#define VDR_PROTOCOL_CMD_02H	0x02
#define VDR_PROTOCOL_CMD_03H	0x03
#define VDR_PROTOCOL_CMD_04H	0x04
#define VDR_PROTOCOL_CMD_05H	0x05
#define VDR_PROTOCOL_CMD_06H	0x06
#define VDR_PROTOCOL_CMD_07H	0x07
#define VDR_PROTOCOL_CMD_08H	0x08
#define VDR_PROTOCOL_CMD_09H	0x09
#define VDR_PROTOCOL_CMD_10H	0x10
#define VDR_PROTOCOL_CMD_11H	0x11
#define VDR_PROTOCOL_CMD_12H	0x12
#define VDR_PROTOCOL_CMD_13H	0x13
#define VDR_PROTOCOL_CMD_14H	0x14
#define VDR_PROTOCOL_CMD_15H	0x15

#define VDR_PROTOCOL_CMD_82H	0x82
#define VDR_PROTOCOL_CMD_83H	0x83
#define VDR_PROTOCOL_CMD_84H	0x84

#define VDR_PROTOCOL_CMD_C2H	0xC2
#define VDR_PROTOCOL_CMD_C3H	0xC3
#define VDR_PROTOCOL_CMD_C4H	0xC4

#define VDR_PROTOCOL_CMD_D0H    0xD0

#define VDR_PROTOCOL_CMD_E0H	0xE0
#define VDR_PROTOCOL_CMD_E1H	0xE1
#define VDR_PROTOCOL_CMD_E2H	0xE2
#define VDR_PROTOCOL_CMD_E3H	0xE3
#define VDR_PROTOCOL_CMD_E4H	0xE4

/**************************************************************************
//函数名：VDRProtocol_Parse
//功能：记录仪串行(DB9串口)协议解析
//输入：需要解析的数据、长度
//输出：无
//返回值：0：解析正确；1：解析错误
//备注：串口接收到一帧数据后需调用此函数解析
***************************************************************************/
u8 VDRProtocol_Parse(u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd
//功能：命令字解析
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据），Cmd:命令字
//输出：无
//返回值：应答数据长度
//备注：VDRProtocol_Parse调用该函数，
//当返回值为0xFFFF时表示采集数据命令帧接收出错；当返回值为0xFFFE时表示设置参数命令帧接收出错；
//当返回值为0xFFFD时表示不是合法的命令字(Cmd不合法)；
***************************************************************************/
u16 VDRProtocol_ParseCmd(u8 *pDstBuffer, u8 Cmd, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_Packet
//功能：将需要发送的数据打包
//输入：Cmd：指令，pBuffer：指向需要发送的数据，BufferLen：数据长度
//输出：无
//返回值：打包后的长度
//备注：调用该函数后pBuffer里面的内容会被改写
***************************************************************************/
u16 VDRProtocol_Packet(u8 Cmd, u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//函数名：VDRProtocol_SendCmd
//功能：命令发送
//输入：Cmd：指令，pBuffer：指向指令数据，BufferLen：指令数据长度
//输出：无
//返回值：0：发送成功；1：发送失败
//备注：VDRProtocol_Parse调用该函数，
//当长度BufferLen为0xFFFF时表示采集数据命令帧接收出错；
//当长度BufferLen为0xFFFE时表示设置参数命令帧接收出错；
//当长度BufferLen为0xFFFD时表示不是合法的命令字(Cmd不合法),不应答；
***************************************************************************/
u8 VDRProtocol_SendCmd(u8 Cmd, u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd00H
//功能：00H指令应答，采集记录仪执行标准版本
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd00H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd01H
//功能：01H指令应答，采集当前驾驶人信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd01H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd02H
//功能：02H指令应答，采集记录仪实时时间
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd02H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd03H
//功能：03H指令应答，采集累计行驶里程
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd03H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd04H
//功能：04H指令应答，采集记录仪脉冲系数
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd04H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd05H
//功能：05H指令应答，采集车辆信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd05H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd06H
//功能：06H指令应答，采集记录仪状态信号配置信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd06H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//函数名：VDRProtocol_ParseCmd07H
//功能：07H指令应答，采集记录仪唯一性编号
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd07H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);


#endif
