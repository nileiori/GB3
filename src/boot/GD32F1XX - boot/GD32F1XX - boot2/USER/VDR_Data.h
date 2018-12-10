#ifndef __VDR_DATA_H
#define __VDR_DATA_H

//************文件包含***************
#include "stm32f10x.h"
#include "Rtc.h"

//*************宏定义****************
#define VDR_DATA_TYPE_SPEED			0//记录仪存储类型，48小时速度记录
#define VDR_DATA_TYPE_POSITION			1//360小时位置信息记录
#define VDR_DATA_TYPE_DOUBT			2//事故疑点数据记录
#define VDR_DATA_TYPE_OVER_TIME			3//超时驾驶记录
#define VDR_DATA_TYPE_DRIVER			4//驾驶人身份(签到/签退)记录
#define VDR_DATA_TYPE_POWER			5//记录仪供电/断电记录
#define VDR_DATA_TYPE_PARAMETER			6//设置参数记录
#define VDR_DATA_TYPE_SPEED_STATUS		7//速度异常记录
#define VDR_DATA_TYPE_MAX			8//类型不能等于或大于该值

#define VDR_DATA_SPEED_STEP_LEN			144//5+126+13，存储的步长：4字节时间+数据内容（采集内容+保留内容）+1字节校验
#define VDR_DATA_POSITION_STEP_LEN		681//5+666+10
#define VDR_DATA_DOUBT_STEP_LEN			251//5+234+12
#define VDR_DATA_OVER_TIME_STEP_LEN		67//5+50+12
#define VDR_DATA_DRIVER_STEP_LEN		36//5+25+6
#define VDR_DATA_POWER_STEP_LEN			18//5+7+6
#define VDR_DATA_PARAMETER_STEP_LEN		18//5+7+6
#define VDR_DATA_SPEED_STATUS_STEP_LEN		144//5+133+6
#define VDR_DATA_MAX_STEP_LEN			682////步长不能等于或大于该值

#define VDR_DATA_SPEED_COLLECT_LEN		126//采集的长度，即每条数据长度
#define VDR_DATA_POSITION_COLLECT_LEN		666
#define VDR_DATA_DOUBT_COLLECT_LEN		234
#define VDR_DATA_OVER_TIME_COLLECT_LEN		50
#define VDR_DATA_DRIVER_COLLECT_LEN		25
#define VDR_DATA_POWER_COLLECT_LEN		7
#define VDR_DATA_PARAMETER_COLLECT_LEN		7
#define VDR_DATA_SPEED_STATUS_COLLECT_LEN	133
//#define VDR_DATA_MAX_COLLECT_LEN		667

#define VDR_DATA_SPEED_PACKET_LIST		7//打包传输时，每包最大的打包条数
#define VDR_DATA_POSITION_PACKET_LIST		1
#define VDR_DATA_DOUBT_PACKET_LIST		4
#define VDR_DATA_OVER_TIME_PACKET_LIST		20
#define VDR_DATA_DRIVER_PACKET_LIST		40
#define VDR_DATA_POWER_PACKET_LIST		142
#define VDR_DATA_PARAMETER_PACKET_LIST		142
#define VDR_DATA_SPEED_STATUS_PACKET_LIST	7
//#define VDR_DATA_MAX_PACKET_LIST		143



#define VDR_DATA_BUFFER_SIZE			VDR_DATA_MAX_STEP_LEN

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
	u8	WriteEnableFlag;
}VDR_DATA_STRUCT;

//*************函数声明***********
/**************************************************************************
//函数名：VDRData_TimeTask(void)
//功能：记录仪数据定时任务
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：50ms调度1次，本任务只是对存储的变量做初始化，实时监测，发现问题进行纠错处理
***************************************************************************/
FunctionalState VDRData_TimeTask(void);
/**************************************************************************
//函数名：VDRData_Init
//功能：数据模块初始化
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：50ms调度1次，本任务只是对存储的变量做初始化，实时监测，发现问题进行纠错处理
***************************************************************************/
void VDRData_Init(void);
/**************************************************************************
//函数名：VDRData_Write
//功能：写一条行驶记录仪数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；length:数据长度；Time:32位hex表示的RTC时间
//输出：无
//返回值：成功SUCCESS,失败ERROR
//备注：行驶记录仪数据记录到flash的格式为：时间（4字节）+数据内容（固定长度）+校验和（1字节，是前面所有字节内容的校验和）
//VDR_DATA_TYPE_SPEED数据内容格式为：行驶记录仪标准表A.17规定的格式（126字节）+13字节预留空间
//VDR_DATA_TYPE_POSITION数据内容格式为：行驶记录仪标准表A.19规定的格式（666字节）+10字节预留空间
//VDR_DATA_TYPE_DOUBT数据内容格式为：行驶记录仪标准表A.21规定的格式（234字节）+12字节预留空间
//VDR_DATA_TYPE_OVER_TIME数据内容格式为：行驶记录仪标准表A.24规定的格式（50字节）+12字节预留空间
//VDR_DATA_TYPE_DRIVER数据内容格式为：行驶记录仪标准表A.26规定的格式（25字节）+6字节预留空间
//VDR_DATA_TYPE_POWER数据内容格式为：行驶记录仪标准表A.28规定的格式（7字节）+6字节预留空间
//VDR_DATA_TYPE_PRAMATER数据内容格式为：行驶记录仪标准表A.30规定的格式（7字节）+6字节预留空间
//VDR_DATA_TYPE_SPEED_STATUS数据内容格式为：行驶记录仪标准表A.32规定的格式（133字节）+6字节预留空间
***************************************************************************/
ErrorStatus VDRData_Write(u8 Type, u8 *pBuffer, u16 length, u32 Time);
/**************************************************************************
//函数名：VDRData_Read
//功能：读取指定结束时间前的N块行驶记录仪数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；EndTime:结束时间；block:块数
//输出：无
//返回值：读取的数据长度
//备注：采用闭区间的方式，即如果有一条记录的时间刚好等于结束时间，该记录将被检出
***************************************************************************/
u16 VDRData_Read(u8 *pBuffer,u8 Type,TIME_T StartTime,TIME_T EndTime, u8 block);
/**************************************************************************
//函数名：VDRData_SetWriteEnableFlag
//功能：设置某个数据类型的写使能标志(禁止或使能)
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//Value:0为禁止,1为使能
//输出：无
//返回值：成功返回SUCCESS,失败返回ERROR
//备注:!!!!!特别注意谁禁止的谁必须负责开启,否则影响后面数据的记录!!!!
//在远程读取行驶记录仪分包数据时建议先禁止写,然后传输完成后再开启写,
//这样做的原因是:远程读取数据的过程中若没有禁止写,则很可能会有新数据记录,
//从而导致分包信息动态变化,读取的分包数据就不准确了
***************************************************************************/
ErrorStatus VDRData_SetWriteEnableFlag(u8 Type, u8 Value);
/**************************************************************************
//函数名：VDRData_ReadWriteEnableFlag
//功能：读某个数据类型的写使能标志
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：//Value:0为禁止,1为使能,若返回0xFF表示参数错误
//备注:!!!!!特别注意谁禁止的谁必须负责开启,否则影响后面数据的记录!!!!
//在远程读取行驶记录仪分包数据时建议先禁止写,然后传输完成后再开启写,
//这样做的原因是:远程读取数据的过程中若没有禁止写,则很可能会有新数据记录,
//从而导致分包信息动态变化,读取的分包数据就不准确了
***************************************************************************/
u8 VDRData_ReadWriteEnableFlag(u8 Type);
/**************************************************************************
//函数名：VDRData_ReadPacket
//功能：读取指定序号的那包数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；PacketNum:条序号,从1开始,时间最老的为第1包
//Direction:0:顺序读出(老时间排在前面,新时间排在后面);1:逆序读出(新时间排在前面,老时间排在后面)
//输出：无
//返回值：读取的数据长度
//备注：读取的数据内容为各命令数据块的组合，组合条数如下：
//VDR_DATA_SPEED_PACKET_LIST		7//打包传输时，每包打包的条数
//VDR_DATA_POSITION_PACKET_LIST		1
//VDR_DATA_DOUBT_PACKET_LIST		4
//VDR_DATA_OVER_TIME_PACKET_LIST	20
//VDR_DATA_DRIVER_PACKET_LIST		40
//VDR_DATA_POWER_PACKET_LIST		142
//VDR_DATA_PARAMETER_PACKET_LIST		142
//VDR_DATA_SPEED_STATUS_PACKET_LIST	7
***************************************************************************/
u16 VDRData_ReadPacket(u8 *pBuffer,u8 Type,u16 PacketNum,u8 Direction);
/**************************************************************************
//函数名：VDRData_ReadList
//功能：读取指定序号的那条数据
//输入：Type:类型，可选值为DATA_TYPE_SPEED、DATA_TYPE_POSITION、
//DATA_TYPE_DOUBT、DATA_TYPE_OVER_TIME、DATA_TYPE_DRIVER、
//DATA_TYPE_POWER、DATA_TYPE_PRAMATER、DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；ListNum:条序号,从1开始,时间最老的那条为第1条
//输出：无
//返回值：读取的数据长度
//备注：读取的数据内容格式为各命令的数据块格式,例如行驶速度数据块格式为表A.17
***************************************************************************/
u16 VDRData_ReadList(u8 *pBuffer,u8 Type,u16 ListNum);
/**************************************************************************
//函数名：VDRData_ReadTotalPacket(u8 Type)
//功能：读某个记录类型存储的总包数 
//输入：Type:类型，可选值为DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：总包数 
//备注：
//VDR_DATA_SPEED_PACKET_LIST		7//打包传输时，每包最大的打包条数
//VDR_DATA_POSITION_PACKET_LIST		1
//VDR_DATA_DOUBT_PACKET_LIST		4
//VDR_DATA_OVER_TIME_PACKET_LIST	20
//VDR_DATA_DRIVER_PACKET_LIST		40
//VDR_DATA_POWER_PACKET_LIST		142
//VDR_DATA_PARAMETER_PACKET_LIST		142
//VDR_DATA_SPEED_STATUS_PACKET_LIST	7
***************************************************************************/
u16 VDRData_ReadTotalPacket(u8 Type);
/**************************************************************************
//函数名：VDRData_ReadTotalList(u8 Type)
//功能：读某个记录类型存储的总条数
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：总条数
//备注：
***************************************************************************/
u16 VDRData_ReadTotalList(u8 Type);

/**************************************************************************
//函数名：VDRData_ReadNewestTime(u8 Type)
//功能：获取存储的最新记录时间
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：32位hex值表示的时间
//备注：返回值为0表示该存储区还没有记录任何数据,返回值为0xffffffff表示参数出错
***************************************************************************/
u32 VDRData_ReadNewestTime(u8 Type);
/**************************************************************************
//函数名：VDRData_ReadOldestTime(u8 Type)
//功能：获取存储的最老记录时间
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：32位hex值表示的时间
//备注：返回值为0表示还没有记录数据
***************************************************************************/
u32 VDRData_ReadOldestTime(u8 Type);

/**************************************************************************
//函数名：VDRData_ReadInitFlag
//功能：获取初始化标志
//输入：无
//输出：无
//返回值：初始化标志
//备注：1表示已完成初始化，0表示未完成初始化，0xFF表示参数错误，只有已完成初始化写才有效。
***************************************************************************/
u8 VDRData_ReadInitFlag(u8 Type);
/**************************************************************************
//函数名：VDRData_Erase(u8 Type)
//功能：擦除某个存储区的数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVER_TIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDRData_Erase(u8 Type);

#endif

