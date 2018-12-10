#ifndef __CAN_DRIVER_H
#define __CAN_DRIVER_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"



#define CAN_RX_BUFFER_SIZE	1200//最多可存储1000/CAN_STEP_LEN条CAN数据
#define CAN_STEP_LEN_MAX	  25//最大步长，每个CAN数据包含帧头1字节+5字节时间（时分秒，毫秒2字节，默认为0）+4字节ID+8字节数据+帧尾1字节+转义多出的字节
#define CAN_SOFTWARE_MASK_NUM   8//软件滤除ID的数量


#define CAN_1_CLK		RCC_AHB1Periph_GPIOD//CAN1接口模块
#define CAN_1_PORT		GPIOD
#define CAN_1_RX_PIN		GPIO_Pin_0
#define CAN_1_TX_PIN		GPIO_Pin_1
#define CAN_1_RX_PIN_SOURCE	GPIO_PinSource0
#define CAN_1_TX_PIN_SOURCE	GPIO_PinSource1

#define CAN_1_POWER_CLK		RCC_AHB1Periph_GPIOD//CAN1供电引脚
#define CAN_1_POWER_PORT	GPIOD
#define CAN_1_POWER_PIN		GPIO_Pin_15
#define CAN_1_POWER_ON()	GPIO_WriteBit(CAN_1_POWER_PORT,CAN_1_POWER_PIN,Bit_SET)
#define CAN_1_POWER_OFF()	GPIO_WriteBit(CAN_1_POWER_PORT,CAN_1_POWER_PIN,Bit_RESET)

#define CAN_1_BAUDRATE		250000

/**************************************************************************
//函数名：Can_Init
//功能：实现CAN接口初始化
//输入：无
//输出：无
//返回值：无
//备注：包含CAN1，CAN2的初始化，由于现在CAN2没有使用，所以只有CAN1的，
//该函数需要在CAN应用任务运行前或一运行CAN应用任务就调用
***************************************************************************/
void Can_Init(void);
/**************************************************************************
//函数名：Can_1_Rx_Isr
//功能：实现CAN1接收中断处理
//输入：无
//输出：无
//返回值：无
//备注：中断服务函数需调用此函数
***************************************************************************/
void Can_1_Rx_Isr(void);
void CAN_1_ID_Filter(u8 FilterNum, u32 FilterId, u32 FilterMaskID, u8 IdType);

#endif