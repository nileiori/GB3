#ifndef __FRAM_H
#define __FRAM_H

//*********************头文件********************************
#include "stm32f10x.h"
#include "spi_flash.h"

//*********************自定义数据类型************************


//**********************宏定义********************************


//******************铁电空间分配******************
#define FRAM_CHIP_START_ADDR		0
#define FRAM_CHIP_END_ADDR			2047//最末尾两字节用于自检，不能被用作其它

#define	FRAM_UPDATA_FLAG_ADDR													1368//1字节标志+1字节校验

#define FRAM_VDR_DOUBT_DATA_ADDR											1370//共占450字节，150个点，每个点3字节（2字节数据+1字节校验）
#define FRAM_VDR_DOUBT_TIME_ADDR											1823//数据6字节+1字节校验，用于记录停车时间
#define FRAM_VDR_DOUBT_COUNT_ADDR											1830//数据2字节+1字节校验，用于标记当前事故疑点数据记录到第几个点
#define	FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR						1833//16*6字节,15分钟停车速度记录
#define	FRAM_VDR_SPEED_15_MINUTE_SPEED_COUNT_ADDR			1929//2字节，1字节+1字节校验
#define	FRAM_VDR_SPEED_STOP_TIME_ADDR									1931//停车时间，4字节+1字节校验							
#define	FRAM_VDR_MILEAGE_BACKUP_ADDR									1936//累计行驶里程备份，4字节+1字节校验		
#define	FRAM_VDR_MILEAGE_ADDR													1941//累计行驶里程，4字节+1字节校验										
#define	FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR						1946//签到时间，4字节+1字节校验
#define	FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR							1951//签到时间，4字节+1字节校验
#define	FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR						1956//当前驾驶员序号，取值范围0-5，1字节+1字节校验
#define	FRAM_VDR_OVERTIME_LICENSE_ADDR								1958//当前驾驶证号码，18字节+1字节校验
#define	FRAM_VDR_OVERTIME_START_TIME_ADDR							1977//开始时间，4字节+1字节校验
#define	FRAM_VDR_OVERTIME_END_TIME_ADDR								1982//结束时间，4字节+1字节校验
#define	FRAM_VDR_OVERTIME_START_POSITION_ADDR					1987//开始位置，10字节+1字节校验
#define	FRAM_VDR_OVERTIME_END_POSITION_ADDR						1998//结束位置，10字节+1字节校验
#define	FRAM_VDR_OVERTIME_NIGHT_START_TIME_ADDR				2009//夜间开始时间，4字节+1字节校验
#define	FRAM_VDR_OVERTIME_NIGHT_END_TIME_ADDR					2014//夜间结束时间，4字节+1字节校验
#define	FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR		2019//夜间开始位置，10字节+1字节校验
#define	FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR			2030//夜间结束位置，10字节+1字节校验

#define FRAM_VDR_DOUBT_DATA_LEN											2
#define FRAM_VDR_DOUBT_TIME_LEN											6
#define FRAM_VDR_DOUBT_COUNT_LEN										2
#define	FRAM_VDR_SPEED_STOP_TIME_LEN								4
#define	FRAM_VDR_MILEAGE_BACKUP_LEN									4
#define	FRAM_VDR_MILEAGE_LEN												4
#define	FRAM_VDR_OVERTIME_LOGOUT_TIME_LEN						4
#define	FRAM_VDR_OVERTIME_LOGIN_TIME_LEN						4
#define	FRAM_VDR_OVERTIME_CURRENT_NUM_LEN						1
#define	FRAM_VDR_OVERTIME_LICENSE_LEN								18
#define	FRAM_VDR_OVERTIME_START_TIME_LEN						4
#define	FRAM_VDR_OVERTIME_END_TIME_LEN							4
#define	FRAM_VDR_OVERTIME_START_POSITION_LEN				10
#define	FRAM_VDR_OVERTIME_END_POSITION_LEN					10
#define	FRAM_VDR_OVERTIME_NIGHT_START_TIME_LEN			4
#define	FRAM_VDR_OVERTIME_NIGHT_END_TIME_LEN				4
#define	FRAM_VDR_OVERTIME_NIGHT_START_POSITION_LEN	10
#define	FRAM_VDR_OVERTIME_NIGHT_END_POSITION_LEN		10

//*******************铁电操作相关*****************
#define	SPI_FRAM_SPIx_RCC							RCC_APB2Periph_SPI1//SPIX

#define	SPI_FRAM_CLK_RCC							RCC_APB2Periph_GPIOA//CLK
#define	SPI_FRAM_CLK_PORT							GPIOA
#define	SPI_FRAM_CLK_PIN							GPIO_Pin_5

#define	SPI_FRAM_MISO_RCC							RCC_APB2Periph_GPIOA//MISO
#define	SPI_FRAM_MISO_PORT						GPIOA
#define	SPI_FRAM_MISO_PIN							GPIO_Pin_6

#define	SPI_FRAM_MOSI_RCC							RCC_APB2Periph_GPIOA//MOSI
#define	SPI_FRAM_MOSI_PORT						GPIOA
#define	SPI_FRAM_MOSI_PIN							GPIO_Pin_7

#define SPI_FRAM_WP_RCC	   						RCC_APB2Periph_GPIOA//WP
#define SPI_FRAM_WP_PORT        			GPIOA
#define SPI_FRAM_WP_PIN         			GPIO_Pin_15

#define SPI_FRAM_CS_RCC   						RCC_APB2Periph_GPIOG//CS
#define SPI_FRAM_CS_PORT        			GPIOG
#define SPI_FRAM_CS_PIN        				GPIO_Pin_8 


#define FRAM_CS_LOW()       GPIO_ResetBits(SPI_FRAM_CS_PORT, SPI_FRAM_CS_PIN);spi_Delay_uS(60)
#define FRAM_CS_HIGH()      GPIO_SetBits(SPI_FRAM_CS_PORT, SPI_FRAM_CS_PIN);spi_Delay_uS(60)

#define FRAM_WP_LOW()       GPIO_ResetBits(SPI_FRAM_WP_PORT, SPI_FRAM_WP_PIN);spi_Delay_uS(60)
#define FRAM_WP_HIGH()      GPIO_SetBits(SPI_FRAM_WP_PORT, SPI_FRAM_WP_PIN);spi_Delay_uS(60)


//***********************函数声明********************************
/*********************************************************************
//函数名称	:FRAM_Init()
//功能		:铁电初始化
//备注		:
*********************************************************************/
void FRAM_Init(void);
/*********************************************************************
//函数名称	:FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
//功能		:往铁电里写数据,在数据某尾加入校验字节
//输入		:WriteAddr，铁电地址
//		:pBuffer，数据缓冲
//		:NumBytesToWrite，写入的字节数
//备注		:
*********************************************************************/
void FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite);
/*********************************************************************
//函数名称	:FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
//功能		:从铁电里读数据
//输入		:ReadAddr，铁电地址
//		:pBuffer，目标缓冲
//		:NumBytesToRead，读出的字节数 
//返回		:实际读出的字节数
//备注		:
*********************************************************************/
u8 FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr);
/*********************************************************************
//函数名称	:FRAM_CheckChip(void)
//功能		:检查铁电芯片工作是否正常
//输入		:铁电地址的最后两个字节用于自检
//		:
//		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
ErrorStatus FRAM_CheckChip(void);
/*********************************************************************
//函数名称	:FRAM_EraseChip(void)
//功能		:擦除整个铁电
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:状态寄存器的值
//备注		:
*********************************************************************/
void  FRAM_EraseChip(void);
#endif
