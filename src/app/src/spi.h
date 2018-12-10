#ifndef __SPI_H
#define __SPI_H
//#include "sys.h"
#include "my_typedef.h"
#include "spi_flash.h"

//�ı���ʱֵ���Ըı����Ƶ��
#define SD_CS_LOW()       GPIO_ResetBits(GPIOD, GPIO_Pin_14);spi_Delay_uS(50)//������ʱֵ��С��20
#define SD_CS_HIGH()      GPIO_SetBits(GPIOD, GPIO_Pin_14);spi_Delay_uS(50) 
#define SD_SPIx_SCK_RESET() GPIO_ResetBits(GPIOB, GPIO_Pin_10);spi_Delay_uS(20)//������ʱֵ��С��10  
#define SD_SPIx_SCK_SET()   GPIO_SetBits(GPIOB, GPIO_Pin_10);spi_Delay_uS(20) 
#define SD_SPIx_MOSI_RESET()    GPIO_ResetBits(GPIOC, GPIO_Pin_3);spi_Delay_uS(20)//������ʱֵ��С��10
#define SD_SPIx_MOSI_SET()      GPIO_SetBits(GPIOC, GPIO_Pin_3);spi_Delay_uS(20)  
/*
//������IO_Init�г�ʼ��SD_Power�˿ڣ���ǰ��SD����Դ�������̳�ʼ��ʱ�䡣
#define SD_SD_Power_RESET() GPIO_ResetBits(GPIOG, GPIO_Pin_3);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000);\
                            spi_Delay_uS(65000)
#define SD_SD_Power_SET()   GPIO_SetBits(GPIOG, GPIO_Pin_3);spi_Delay_uS(5000); 
*/				  	    													  
void SPIx_Init(void);			 //��ʼ��SPI��
void SPIx_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif

