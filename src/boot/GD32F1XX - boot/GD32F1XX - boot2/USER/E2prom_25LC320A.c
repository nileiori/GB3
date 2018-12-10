/************************************************************************* 
*
* Copyright (c) 2013,深圳市伊爱高新技术开发有限公司
* All rights reserved.
*
* 文件名称 : E2prom_25LC320A.c
* 功能     : E2prom_25LC320A_SPI的读写操作.
*
* 当前版本 : 1.0
* 开发者   : Senon Dou
* 修改时间 : 2013.07.26
*
* 历史版本 : 
* 开发者   : 
* 完成时间 : 
* 
* 备注 :    1 - 使用SPI1进行操作，注意EEPROM、FRAM、FLASH共用了SPI1，
                通过片选引脚选择进行操作。

            2 - 本程序中调用了spi_flash.c的部分函数，包括：
                uint8_t SPI_FLASH_SendByte(uint8_t byte);
                uint8_t SPI_FLASH_ReadByte(void);

            3 - SPI1的初始化在spi_flash.c中，本程序不再执行初始化操作。
*************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "spi_flash.h"
#include "E2prom_25LC320A.h"

/********************************************************************
* 名称 : E2prom_Init
* 功能 : 25LC320A_SPI的GPIO初始化，包括25LC320A_CS、25LC320A_WP。
*
* 输入:   none
* 输出:   none
*
* 全局变量:   none
* 调用函数:  none
*
* 中断资源:  none
*
* 备注:   none
********************************************************************/
void E2prom_Init(void)
{ 
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(SPI_E2PROM_SPIx_RCC|SPI_E2PROM_CLK_RCC|SPI_E2PROM_MISO_RCC
	|SPI_E2PROM_MOSI_RCC|SPI_E2PROM_WP_RCC|SPI_E2PROM_CS_RCC, ENABLE);

  /* Configure SPI1 pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_E2PROM_CLK_PIN | SPI_E2PROM_MISO_PIN | SPI_E2PROM_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI_E2PROM_CLK_PORT, &GPIO_InitStructure);

  /* Configure I/O for Flash Chip select */
  GPIO_InitStructure.GPIO_Pin = SPI_E2PROM_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_E2PROM_CS_PORT, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  E2PROM_CS_HIGH();
  
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  //SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
   
}

/********************************************************************
* 名称 : E2prom_ReadByte
* 功能 : E2PROM_25LC320A读字节操作
*
* 输入: Address 读的起始地址
        *destin 读出数据存放地址指针
        bytenum 读的字节数
* 输出: none
*
* 全局变量:  none
* 调用函数: uint8_t SPI_FLASH_SendByte(uint8_t byte);
            uint8_t SPI_FLASH_ReadByte(void);
            u8 E2prom_ReadStatusRegister();
*
* 中断资源: none
*
* 备注: 
********************************************************************/
void E2prom_ReadByte(u16 Address, u8 *destin, u8 bytenum)
{
    u8 i;
    u8 temp;
    u8 count = 0;
    u8 *p;
    u8 addr;
  
    if(bytenum == 0)
        return;
    p = destin;
  
    //sFLASH_LowLevel_Init(); //初始化SPI1。由于EEPROM、FRAM、FLASH共用SPI1，这里默认SPI1已被初始化
    
    //检查写标志，等待写完成
    while(1)    
    {
        count++;
        //读状态寄存器
        temp = E2prom_ReadStatusRegister();
        if(E2PROM_WIP_BIT == (temp &E2PROM_WIP_BIT))//正在写进行中
	    {
            spi_Delay_uS(1000);//1ms，等待
        }
        else
        {
            break;  //跳出
        }
        if(count > 50) //最多等待50ms
        {
            return;  //返回
        }
    }
    
    E2PROM_CS_LOW();
      
    SPI_FLASH_SendByte(READ);    //发送读命令
     
    addr = Address >> 8;
    SPI_FLASH_SendByte(addr);  //发送地址高字节  
    addr = Address;
    SPI_FLASH_SendByte(addr);  //发送地址低字节
     
    for(i = 0; i < bytenum; i++)
    {
        *p++ = SPI_FLASH_ReadByte();  //读存储器内容
    }
    
    E2PROM_CS_HIGH();
  
}

/********************************************************************
* 名称 : E2prom_WriteByte
* 功能 : E2PROM_25LC320A写字节操作
*
* 输入: Address 写入的起始地址
        data    写入的数据
* 输出: SUCCESS 成功
        ERROR   失败
*
* 全局变量:  none
* 调用函数: uint8_t SPI_FLASH_SendByte(uint8_t byte);
            uint8_t SPI_FLASH_ReadByte(void);
            u8 E2prom_ReadStatusRegister();
*
* 中断资源: none
*
* 备注:  none
********************************************************************/
u8 E2prom_WriteByte(u16 Address, u8 data)
{
    u8 addr;
    u8 temp;
    u8 count = 0;

    //sFLASH_LowLevel_Init();//初始化SPI1。由于EEPROM、FRAM、FLASH共用SPI1，这里默认SPI1已被初始化
    
    //检查写标志，等待写完成
    while(1)    
    {
        count++;
        //读状态寄存器
        temp = E2prom_ReadStatusRegister();
        if(E2PROM_WIP_BIT == (temp &E2PROM_WIP_BIT))//正在写进行中
	    {
            spi_Delay_uS(1000);//1ms，等待
        }
        else
        {
            break;  //跳出
        }
        if(count > 50) //最多等待50ms
        {
            return ERROR;  //返回
        }
    }
    
    E2PROM_CS_LOW();        
    SPI_FLASH_SendByte(WREN);    //写使能命令
    E2PROM_CS_HIGH();
    
    E2PROM_CS_LOW();
    
    SPI_FLASH_SendByte(WRITE);    //发送写命令
    
    addr = Address >> 8;
    SPI_FLASH_SendByte(addr);  //发送地址高字节
    addr = Address;
    SPI_FLASH_SendByte(addr);  //发送地址低字节
    
    SPI_FLASH_SendByte(data); //发送数据，发送完成后 写保护=RESET
  
    E2PROM_CS_HIGH();

    return SUCCESS;
}

/********************************************************************
* 名称 : E2prom_ReadStatusRegister(void)
* 功能 : 读状态寄存器
*
* 输入:   none
* 输出: 状态寄存器的值
*
* 全局变量:  none
* 调用函数: uint8_t SPI_FLASH_SendByte(uint8_t byte);
            uint8_t SPI_FLASH_ReadByte(void);
*
* 中断资源: none
*
* 备注:  none
********************************************************************/
u8 E2prom_ReadStatusRegister(void)
{
	u8	temp;
        
    //sFLASH_LowLevel_Init();
        
	//拉低片选
 	E2PROM_CS_LOW();

	//发送读状态寄存器命令
	SPI_FLASH_SendByte(RDSR);

	//读状态寄存器
	temp = SPI_FLASH_ReadByte();

	//片选拉高
	E2PROM_CS_HIGH();

	//返回
	return temp;
}

/********************************************************************
* 名称 : Eeprom_CheckChip(void)
* 功能 : 检查EEPROM芯片工作是否正常
*        eeprom地址的最后两个字节用来做自检用
* 输入:  none
* 输出:  none
*
* 全局变量: E2_LAST_ADDR  E2prom的最后两个字节的地址
* 调用函数: u8 E2prom_WriteByte(u16 Address, u8 data)
            void E2prom_ReadByte(u16 Address, u8 *destin, u8 bytenum)
*
* 中断资源: none
*
* 备注: none
********************************************************************/
ErrorStatus Eeprom_CheckChip(void)
{
	u8	i;
	u8	ReadBuffer[3] = {0};
	u8	WriteBuffer[3] = {"OK"};
	u16	Address;
	
	Address = E2PROM_CHIP_END_ADDR-2;   //最后一个字节2*1024 - 2
	for(i=0; i<3; i++)
	{
		//往地址最后两个字节写数据
		E2prom_WriteByte(Address, WriteBuffer[0]);
		E2prom_WriteByte(Address+1, WriteBuffer[1]);
        
		//读出刚写入的字节数
		E2prom_ReadByte(Address,ReadBuffer,2);
		
		//比较写入的内容与读出的内容是否一致
		if((WriteBuffer[0] == ReadBuffer[0])&&(WriteBuffer[1] == ReadBuffer[1]))
		{
		    return SUCCESS;
		}
	}
	
	return ERROR;
}
