#ifndef __REGISTER_H
#define __REGISTER_H

//************文件包含***************
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "rtc.h"

//*************宏定义****************
#define REGISTER_TYPE_SPEED			0//记录仪存储类型，48小时速度记录
#define REGISTER_TYPE_POSITION			1//360小时位置信息记录
#define REGISTER_TYPE_DOUBT			2//事故疑点数据记录
#define REGISTER_TYPE_OVER_TIME			3//超时驾驶记录
#define REGISTER_TYPE_DRIVER			4//驾驶员签到/签退记录
#define REGISTER_TYPE_POWER			5//记录仪供电/断电记录
#define REGISTER_TYPE_PRAMATER			6//设置参数记录
#define REGISTER_TYPE_SPEED_STATUS		7//速度异常记录
#define REGISTER_TYPE_MAX			8//类型不能等于或大于该值

#define REGISTER_SPEED_STEP_LEN			144//5+126+13，存储的步长：4字节时间+1字节校验+数据内容（采集内容+保留内容）
#define REGISTER_POSITION_STEP_LEN		681//5+666+10
#define REGISTER_DOUBT_STEP_LEN			251//5+234+12
#define REGISTER_OVER_TIME_STEP_LEN		67//5+50+12
#define REGISTER_DRIVER_STEP_LEN		36//5+25+6
#define REGISTER_POWER_STEP_LEN			18//5+7+6
#define REGISTER_PRAMATER_STEP_LEN		18//5+7+6
#define REGISTER_SPEED_STATUS_STEP_LEN		144//5+133+6
#define REGISTER_MAX_STEP_LEN			682////步长不能等于或大于该值

#define REGISTER_SPEED_COLLECT_LEN		126//采集的长度，即每条数据长度
#define REGISTER_POSITION_COLLECT_LEN		666
#define REGISTER_DOUBT_COLLECT_LEN		234
#define REGISTER_OVER_TIME_COLLECT_LEN		50
#define REGISTER_DRIVER_COLLECT_LEN		25
#define REGISTER_POWER_COLLECT_LEN		7
#define REGISTER_PRAMATER_COLLECT_LEN		7
#define REGISTER_SPEED_STATUS_COLLECT_LEN	133
//#define REGISTER_MAX_COLLECT_LEN		667

#define REGISTER_SPEED_PACKET_LIST		7//打包传输时，每包最大的打包条数
#define REGISTER_POSITION_PACKET_LIST		1
#define REGISTER_DOUBT_PACKET_LIST		4
#define REGISTER_OVER_TIME_PACKET_LIST		20
#define REGISTER_DRIVER_PACKET_LIST		40
#define REGISTER_POWER_PACKET_LIST		142
#define REGISTER_PRAMATER_PACKET_LIST		142
#define REGISTER_SPEED_STATUS_PACKET_LIST	7
//#define REGISTER_MAX_PACKET_LIST		143

#define REGISTER_BUFFER_SIZE			750

//*************数据结构定义***********
typedef struct
{
	s16 StartSector;
	s16 EndSector;
	s16 StepLen;
	s16 SectorStep;
	s16 OldestSector;
	s16 CurrentSector;
	s16 CurrentStep;
	s8  LoopFlag;
	s8  InitFlag;
}REGISTER_STRUCT;

//************函数声明**************** 
/**************************************************************************
//函数名：Register_TimeTask(void)
//功能：记录仪数据存储定时任务
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：50ms调度1次，记录仪数据存储由各应用程序调用Register_Write函数实现，本任务只是对
//存储的变量做初始化，实时监测，发现问题进行纠错处理
***************************************************************************/
FunctionalState Register_TimeTask(void);
/**************************************************************************
//函数名：Register_Write
//功能：写一条行驶记录仪数据
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；length:数据长度；Time:32位hex表示的RTC时间
//输出：无
//返回值：成功SUCCESS,失败ERROR
//备注：行驶记录仪数据记录到flash的格式为：时间（4字节）+数据内容（固定长度）+校验和（1字节，是前面所有字节内容的校验和）
//REGISTER_TYPE_SPEED数据内容格式为：行驶记录仪标准表A.17规定的格式（126字节）+13字节预留空间
//REGISTER_TYPE_POSITION数据内容格式为：行驶记录仪标准表A.19规定的格式（666字节）+10字节预留空间
//REGISTER_TYPE_DOUBT数据内容格式为：行驶记录仪标准表A.21规定的格式（234字节）+12字节预留空间
//REGISTER_TYPE_OVER_TIME数据内容格式为：行驶记录仪标准表A.24规定的格式（50字节）+12字节预留空间
//REGISTER_TYPE_DRIVER数据内容格式为：行驶记录仪标准表A.26规定的格式（25字节）+6字节预留空间
//REGISTER_TYPE_POWER数据内容格式为：行驶记录仪标准表A.28规定的格式（7字节）+6字节预留空间
//REGISTER_TYPE_PRAMATER数据内容格式为：行驶记录仪标准表A.30规定的格式（7字节）+6字节预留空间
//REGISTER_TYPE_SPEED_STATUS数据内容格式为：行驶记录仪标准表A.32规定的格式（133字节）+6字节预留空间
***************************************************************************/
ErrorStatus Register_Write(u8 Type, u8 *pBuffer, u16 length, u32 Time);
/**************************************************************************
//函数名：Register_Read
//功能：读取指定结束时间前的N块行驶记录仪数据
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；EndTime:结束时间；block:块数
//输出：无
//返回值：读取的数据长度
//备注：采用闭区间的方式，即如果有一条记录的时间刚好等于结束时间，该记录将被检出
***************************************************************************/
u16 Register_Read(u8 Type,u8 *pBuffer,TIME_T StartTime,TIME_T EndTime, u8 block); 
/**************************************************************************
//函数名：Register_GetNewestTime(u8 Type)
//功能：获取存储的最新记录时间
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS
//输出：无
//返回值：32位hex值表示的时间
//备注：返回值为0表示该存储区还没有记录任何数据,返回值为0xffffffff表示参数出错
***************************************************************************/
u32 Register_GetNewestTime(u8 Type);
/**************************************************************************
//函数名：Register_GetOldestTime(u8 Type)
//功能：获取存储的最老记录时间
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS
//输出：无
//返回值：32位hex值表示的时间
//备注：返回值为0表示还没有记录数据
***************************************************************************/
u32 Register_GetOldestTime(u8 Type);
/**************************************************************************
//函数名：Register_EraseRecorderData(void)
//功能：擦除记录仪所有存储的数据
//输入：无
//输出：无
//返回值：无
//备注：擦除记录仪flash存储的数据，对于铁电或其他地方存储的记录仪数据没有去擦除
***************************************************************************/
void Register_EraseRecorderData(void);
/**************************************************************************
//函数名：Register_GetPacketData(u8 *pBuffer, u8 Type, u16 PacketNum)
//功能：获取分包数据
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS
//PacketNum:包序号，从1开始，该值不大于总包数，时间最新的那一包为第1包
//输出：分包数据
//返回值：读取的数据长度
//备注：数据长度最大为1000字节，组包条数见如下，该值为固定值：
//REGISTER_TYPE_SPEED_PACKET_LIST		7//打包传输时，每包最大的打包条数
//REGISTER_TYPE_POSITION_PACKET_LIST		1
//REGISTER_TYPE_DOUBT_PACKET_LIST		4
//REGISTER_TYPE_OVER_TIME_PACKET_LIST		20
//REGISTER_TYPE_DRIVER_PACKET_LIST		40
//REGISTER_TYPE_POWER_PACKET_LIST		142
//REGISTER_TYPE_PRAMATER_PACKET_LIST		142
//REGISTER_TYPE_SPEED_STATUS_PACKET_LIST	7
//--------------------------------特别注意-----------------------------------
//特别注意：在调用Register_GetPacketData之前需调用1次Register_GetTotalPacketNum！！！
//在反复调用Register_GetPacketData期间不能再调用Register_GetTotalPacketNum，否则可能数据出错
//要分包读取行驶记录仪数据的操作步骤是：
//1.调用Register_GetTotalPacketNum
//2.调用Register_GetPacketData
***************************************************************************/
u16 Register_GetPacketData(u8 *pBuffer, u8 Type, u16 PacketNum);
/**************************************************************************
//函数名：Register_GetTotalPacketNum(u8 Type)
//功能：获取总包数
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS
//输出：无
//返回值：总包数 
//备注：数据长度最大为1000字节，组包条数见如下，该值为固定值：
//REGISTER_TYPE_SPEED_PACKET_LIST		7//打包传输时，每包最大的打包条数
//REGISTER_TYPE_POSITION_PACKET_LIST		1
//REGISTER_TYPE_DOUBT_PACKET_LIST		4
//REGISTER_TYPE_OVER_TIME_PACKET_LIST		20
//REGISTER_TYPE_DRIVER_PACKET_LIST		40
//REGISTER_TYPE_POWER_PACKET_LIST		142
//REGISTER_TYPE_PRAMATER_PACKET_LIST		142
//REGISTER_TYPE_SPEED_STATUS_PACKET_LIST	7
***************************************************************************/
u16 Register_GetTotalPacket(u8 Type);
/**************************************************************************
//函数名：Register_GetInitFlag(u8 Type)
//功能：获取初始化标志
//输入：Type:类型，可选值为REGISTER_TYPE_SPEED、REGISTER_TYPE_POSITION、
//REGISTER_TYPE_DOUBT、REGISTER_TYPE_OVER_TIME、REGISTER_TYPE_DRIVER、
//REGISTER_TYPE_POWER、REGISTER_TYPE_PRAMATER、REGISTER_TYPE_SPEED_STATUS
//输出：无
//返回值：初始化标志，1表示该存储区域的初始化已完成；0表示未完成。
//备注：只有该存储区域的初始化标志为1时，往该区域写数据才会生效，否则写失败
***************************************************************************/
u8 Register_GetInitFlag(u8 Type);
#endif