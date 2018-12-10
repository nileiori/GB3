#ifndef __BMA250E_H
#define __BMA250E_H

//************文件包含***************
//#include "stm32f2xx.h"
#include "stm32f10x.h"

//*************宏定义****************
#define BMA250E_REGISTER_00    0x00  //Chip ID   默认值0x03
#define BMA250E_REGISTER_01    0x01
#define BMA250E_REGISTER_02    0x02  //LSB part of x-axis acceleration  x轴加速度低2位，bit7-6有效，bit0为1表示有新数据
#define BMA250E_REGISTER_03    0x03  //MSB part of x-axis acceleration  x轴加速度高8位（共10位来表示x轴加速度）
#define BMA250E_REGISTER_04    0x04  //LSB part of y-axis acceleration  y轴加速度低2位，bit7-6有效，bit0为1表示有新数据
#define BMA250E_REGISTER_05    0x05  //MSB part of y-axis acceleration  y轴加速度高8位（共10位来表示x轴加速度）
#define BMA250E_REGISTER_06    0x06  //LSB part of z-axis acceleration  y轴加速度低2位，bit7-6有效，bit0为1表示有新数据
#define BMA250E_REGISTER_07    0x07  //MSB part of z-axis acceleration  y轴加速度高8位（共10位来表示x轴加速度）
#define BMA250E_REGISTER_08    0x08
#define BMA250E_REGISTER_09    0x09  //Interrupt Status 使用了bit1---high_int
#define BMA250E_REGISTER_0A    0x0A
#define BMA250E_REGISTER_0B    0x0B
#define BMA250E_REGISTER_0C    0x0C  //Flat and orientation Status 使用了bit2-0
#define BMA250E_REGISTER_0D    0x0D
#define BMA250E_REGISTER_0E    0x0E
#define BMA250E_REGISTER_0F    0x0F  //g-range，bit3-0有效
#define BMA250E_REGISTER_10    0x10
#define BMA250E_REGISTER_11    0x11
#define BMA250E_REGISTER_12    0x12
#define BMA250E_REGISTER_13    0x13  //Acceleration data acquistion & data output format，bit7-6有效 
#define BMA250E_REGISTER_14    0x14  //Softreset 软件复位，复位后寄存器的值为默认值，复位时需写入0xB6
#define BMA250E_REGISTER_15    0x15
#define BMA250E_REGISTER_16    0x16
#define BMA250E_REGISTER_17    0x17  //Interrupt setting  使用了bit2-0
#define BMA250E_REGISTER_18    0x18
#define BMA250E_REGISTER_19    0x19
#define BMA250E_REGISTER_1A    0x1A
#define BMA250E_REGISTER_1B    0x1B
#define BMA250E_REGISTER_1C    0x1C
#define BMA250E_REGISTER_1D    0x1D
#define BMA250E_REGISTER_1E    0x1E
#define BMA250E_REGISTER_1F    0x1F
#define BMA250E_REGISTER_20    0x20
#define BMA250E_REGISTER_21    0x21
#define BMA250E_REGISTER_22    0x22
#define BMA250E_REGISTER_23    0x23
#define BMA250E_REGISTER_24    0x24  //contains the high-g interrupt hysteresis setting  使用了bit7-6
#define BMA250E_REGISTER_25    0x25  //contains the delay time definition for the high-g interrupt
#define BMA250E_REGISTER_26    0x26  //contains the threshold definition for the high-g interrupt
#define BMA250E_REGISTER_27    0x27
#define BMA250E_REGISTER_28    0x28
#define BMA250E_REGISTER_29    0x29
#define BMA250E_REGISTER_2A    0x2A
#define BMA250E_REGISTER_2B    0x2B
#define BMA250E_REGISTER_2C    0x2C
#define BMA250E_REGISTER_2D    0x2D
#define BMA250E_REGISTER_2E    0x2E
#define BMA250E_REGISTER_2F    0x2F
#define BMA250E_REGISTER_30    0x30
#define BMA250E_REGISTER_31    0x31
#define BMA250E_REGISTER_32    0x32
#define BMA250E_REGISTER_33    0x33
#define BMA250E_REGISTER_34    0x34
#define BMA250E_REGISTER_35    0x35
#define BMA250E_REGISTER_36    0x36
#define BMA250E_REGISTER_37    0x37
#define BMA250E_REGISTER_38    0x38
#define BMA250E_REGISTER_39    0x39
#define BMA250E_REGISTER_3A    0x3A
#define BMA250E_REGISTER_3B    0x3B
#define BMA250E_REGISTER_3C    0x3C
#define BMA250E_REGISTER_3D    0x3D
#define BMA250E_REGISTER_3E    0x3E
#define BMA250E_REGISTER_3F    0x3F

#define BMA250E_G_SENSOR_ADDR  0x30  //(0x18<<1)

#define BMA250E_2G_RANGE       0x03
#define BMA250E_4G_RANGE       0x05
#define BMA250E_8G_RANGE       0x08
#define BMA250E_16G_RANGE      0x0C

#define BMA250E_HIGH_G_X_EN       0x01
#define BMA250E_HIGH_G_Y_EN       0x02
#define BMA250E_HIGH_G_Z_EN       0x04

#define BMA250E_PI   3.1415926535

#define BMA250E_G_ARRAY_LEN        12

#define BMA250E_USE_HIGH_G_INT     1//两种中断模式，只需选择一种，选择这种中断模式1.4g以上较难报警
//#define BMA250E_USE_SLOPE_INT      2//选择这种中断模式，全量程0-7.9g都很容易报警

#define BMA250E_GPIO_CLK         RCC_APB2Periph_GPIOD
#define BMA250E_PORT             GPIOD
#define BMA250E_PIN              GPIO_Pin_6
#define BMA250E_EXTI_GPIO        GPIO_PortSourceGPIOD
#define BMA250E_GPIO_PINSOURCE   GPIO_PinSource6//注意，外部中断须与pin脚一致
#define BMA250E_EXTI_LINE        EXTI_Line6//外部中断须与pin脚一致
#define BMA250E_EXTI_IRQn        EXTI9_5_IRQn//外部中断须与pin脚一致

//*************数据结构定义***********


//*************函数声明***********
/**************************************************************************
//函数名：BMA250E_TimeTask(void)
//功能：BMA250E定时任务
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：50ms调度1次
***************************************************************************/
FunctionalState BMA250E_TimeTask(void);
/**************************************************************************
//函数名：BMA250E_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：
***************************************************************************/
void BMA250E_Init(void);
/**************************************************************************
//函数名：BMA250E_Calibration
//功能：角度校准
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void BMA250E_Calibration(void);
/**************************************************************************
//函数名：BMA250E_GetAngle
//功能：获取角度值
//输入：无
//输出：无
//返回值：角度值
//备注：范围是0-180度
***************************************************************************/
u8 BMA250E_GetAngle(void);
/**************************************************************************
//函数名：BMA250E_GetRolloverAlarmFlag
//功能：获取侧翻报警标志
//输入：无
//输出：无
//返回值：侧翻报警标志
//备注：1为产生报警，0为没有
***************************************************************************/
u8 BMA250E_GetRolloverAlarmFlag(void);
/**************************************************************************
//函数名：BMA250E_GetCrashAlarmFlag
//功能：获取碰撞报警标志
//输入：无
//输出：无
//返回值：碰撞报警标志
//备注：1为产生报警，0为没有
***************************************************************************/
u8 BMA250E_GetCrashAlarmFlag(void);
/**************************************************************************
//函数名：BMA250E_UpdateRolloverParameter
//功能：更新侧翻参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void BMA250E_UpdateRolloverParameter(void);
/**************************************************************************
//函数名：BMA250E_UpdateCrashParameter
//功能：更新碰撞参数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void BMA250E_UpdateCrashParameter(void);
/**************************************************************************
//函数名：BMA250E_ClearInitFlag
//功能：清除初始化标志
//输入：无
//输出：无
//返回值：无
//备注：标志被清除后会重新初始化
***************************************************************************/
void BMA250E_ClearInitFlag(void);
#endif

