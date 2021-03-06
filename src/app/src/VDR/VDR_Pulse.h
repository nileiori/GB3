#ifndef __VDR_PULSE_H
#define __VDR_PULSE_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"

#define	VDR_INSTANT_SPEED_ARRAY_SIZE	5//瞬时速度记录数组长度，5个元素，去掉最大值，最小值，然后求平均得到瞬时速度
#define	VDR_PULSE_EXTI_LINE 					EXTI_Line10

/*********************************************************************
//函数名称	:VDRPulse_TimeTask
//功能		:脉冲定时任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:总是返回ENABLE
//备注		:50ms秒调用1次，定时记录（1秒）累计行驶里程，计算秒平均速度
*********************************************************************/
FunctionalState  VDRPulse_TimeTask(void);
/*********************************************************************
//函数名称	:VDRPulse_UpdateParameter
//功能		:更新参数
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:包括脉冲系数、累计行驶里程
//		:当脉冲系数设置为3600或7200时会触发脉冲系数自动校准功能，国标检测时不能开启
*********************************************************************/
void VDRPulse_UpdateParameter(void);
/*********************************************************************
//函数名称	:VDRPulse_GetInstantSpeed
//功能		:获取脉冲瞬时速度,单位km/h
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
u8 VDRPulse_GetInstantSpeed(void);
/*********************************************************************
//函数名称	:VDRPulse_GetPulseSpeed
//功能		:获取脉冲瞬时速度,单位km/h
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:与VDRPulse_GetInstantSpeed相比，VDRPulse_GetPulseSpeed返回的是纯脉冲
//		:不会有GPS替代的情况
*********************************************************************/
u8 VDRPulse_GetPulseSpeed(void);
/*********************************************************************
//函数名称	:VDRPulse_GetSecondSpeed
//功能		:获取脉冲秒平均速度,单位km/h
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
u8 VDRPulse_GetSecondSpeed(void);
/*********************************************************************
//函数名称	:VDRPulse_GetTotalMile
//功能		:获取脉冲总里程
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:单位是0.01公里
//		:
*********************************************************************/
u32 VDRPulse_GetTotalMile(void);
/*********************************************************************
//函数名称	:VDRPulse_ExtiIsr
//功能		:外部中断
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:收到脉冲信号（即产生外部中断）后的处理
//		:
*********************************************************************/
void VDRPulse_ExtiIsr(void);
/*********************************************************************
//函数名称	:VDRPulse_TimIsr
//功能		:初始化定时器中断处理
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void VDRPulse_TimIsr(void);
/*********************************************************************
//函数名称	:VDRPulse_AddMile
//功能		:累计里程
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
void VDRPulse_AddMile(void);
/*********************************************************************
//函数名称	:VDRPulse_CalInstantSpeed
//功能		:计算瞬时速度
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
void VDRPulse_CalInstantSpeed(void);
/*********************************************************************
//函数名称	:VDRPulse_Init
//功能		:初始化
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:退出检定状态时需调用该函数
*********************************************************************/
void VDRPulse_Init(void);

#endif
