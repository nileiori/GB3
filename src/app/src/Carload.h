/*******************************************************************************
 * File Name:			carload.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2013-05-20
 * Rewriter:
 * Date:		
 *******************************************************************************/
#include "stm32f10x.h"
#include "RadioProtocol.h"
#ifndef __CARLOAD_H
#define __CARLOAD_H

#define USE_SYS_CAR_LOAD
//*********************头文件********************************
//////////////////////////////////
//***********************函数声明********************************
/*************************************************************
** 函数名称: CarLoad_ParamInitialize
** 功能描述: 参数初始化
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void CarLoad_ParamInitialize(void);
/*******************************************************************************
**  函数名称  : CarLoad_TimeTask
**  函数功能  : 时间任务调度接口函数
**  输    入  : 无				
**  输    出  : 任务调度状态:  使能或禁止
**  全局变量  : 
**  调用函数  : 
**  中断资源  : 
**  备    注  :
*******************************************************************************/
FunctionalState CarLoad_TimeTask(void);
/*******************************************************************************
** 函数名称: CarLoad_GetCurWeightSubjoinInfo
** 功能描述: 取得当前重量附加信息包(包括附加信息ID、长度、附加信息体)
** 入口参数: 需要保存重量附加信息包的首地址
** 出口参数: 
** 返回参数: 重量附加信息包的总长度
*******************************************************************************/
unsigned char CarLoad_GetCurWeightSubjoinInfo(unsigned char *data);
/*************************************************************
** 函数名称: Carload_GetCurSensorAbnormalValue
** 功能描述: 取得传感器异常值
** 入口参数: 无
** 出口参数: 无
** 返回参数: 每一位对应一路传感器，当该位为1表示该路传感器故障
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned long Carload_GetCurSensorAbnormalValue(void);
/*************************************************************
** 函数名称: Carload_GetClearSensorResult
** 功能描述: 取得传感器清零结果
** 入口参数: 无
** 出口参数: 无
** 返回参数: 每一位对应一路传感器，当该位为1表示该路传感器故障
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned long Carload_GetClearSensorResult(void);
/*************************************************************
** 函数名称: Carload_GetSensorAbnormalFlag
** 功能描述: 取得传感器是否异常标志
** 入口参数: 无
** 出口参数: 无
** 返回参数: 0:正常; 1:异常
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char Carload_GetSensorAbnormalFlag(void);
/*************************************************************
** 函数名称: CarLoad_CommProtocolParse
** 功能描述: 载重传感器协议解释
** 入口参数: ParseBuffer数据首地址,usDataLenght数据长度
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char CarLoad_CommProtocolParse(unsigned char *ParseBuffer,unsigned short usDataLenght);
/*************************************************************
** 函数名称: Carload_SendCmdClearServerSensorAbnormal
** 功能描述: 清除传感器
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void Carload_SendCmdClearSensorAbnormal(void);
/*************************************************************
** 函数名称: Carload_LcdSendCmdQuerySensorAbnormal
** 功能描述: 查询传感器故障
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void Carload_LcdSendCmdQuerySensorAbnormal(void);
/*************************************************************
** 函数名称: Carload_GetCurTotalWeight
** 功能描述: 取得总重量
** 入口参数: 无
** 出口参数: 无
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned long Carload_GetCurTotalWeight(void);
/*************************************************************OK
** 函数名称: CarLoad_UpdatePram
** 功能描述: 更新参数
** 入口参数:  
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
void CarLoad_UpdatePram(void);
/*************************************************************
** 函数名称: CarLoad_DisposeRadioProtocol
** 功能描述: 载重透传协议解析
** 入口参数: cmd命令,pBuffer透传消息首地址，BufferLen透传消息长度
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/
void CarLoad_DisposeRadioProtocol(unsigned char cmd,unsigned char *pBuffer,unsigned short BufferLen);
/*************************************************************
** 函数名称: CarLoad_SendFreightInformation
** 功能描述: 载重上传运货信息
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/	
void CarLoad_SendFreightInformationToService(unsigned char flag);
/*************************************************************
** 函数名称: CarLoad_GetCurOverLoadFlag
** 功能描述: 取得是否超载标志
** 入口参数: 无
** 出口参数: 无
** 返回参数: 0:正常; 1:超载
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char CarLoad_GetCurOverLoadFlag(void);

/*************************************************************OK
** 函数名称: CarLoad_GetCurOnlineStatus
** 功能描述: 在线标志
** 入口参数:  
** 出口参数: 
** 返回参数: 1:在线;0不在线
** 全局变量: 无
** 调用模块:
*************************************************************/
unsigned char CarLoad_GetCurOnlineStatus(void);
/*************************************************************
** 函数名称: CarLoad_DisposeLcdTransToLoadSensor
** 功能描述: 处理Lcd透传给载重传感器数据
** 入口参数: 
** 出口参数: 无
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/	
void CarLoad_DisposeLcdTransToLoadSensor(unsigned char *data,unsigned short len);
/*************************************************************
** 函数名称: CarLoad_PactAndSendData
** 功能描述: 对数据进行打包,并发送
** 入口参数: cmd:命令，CmdType:0是读,1是写,data:数据块首地址,datalen:长度		 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
ProtocolACK CarLoad_PactAndSendData(unsigned char cmd,unsigned char CmdType,unsigned char *data,unsigned short datalen);
#endif//__CARLOAD_H
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

