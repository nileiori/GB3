/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main program body.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
#include "spi_flash.h"
#include "spi_flash_app.h"
#include "fm25c160.h"
#include "E2prom_25LC320A.h"
 
 
extern void MOD_InitCRC(void);
extern u32 MOD_CalcCRC(u8 *blockdata, u32 blocksize);

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Private define ------------------------------------------------------------*/

#define E2_PROGRAM_UPDATA_REQUEST_ID_ADDR				735//注意这3个地址需要与应用程序中定义的相同
#define E2_PROGRAM_UPDATA_VERIFYCODE_ID_ADDR		740
#define E2_PROGRAM_UPDATA_LENGTH_ID_ADDR				745

#define BEEP_OFF()       GPIO_ResetBits(GPIOG, GPIO_Pin_12);spi_Delay_uS(30)
#define BEEP_ON()        GPIO_SetBits(GPIOG, GPIO_Pin_12);spi_Delay_uS(30)

#define  ApplicationAddress    0x08004000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static	u8	UpdateBuffer[600];
static	u32 JumpAddress;
static  pFunction Jump_To_Application;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(__IO uint32_t nCount);
void Beep_Init(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
ErrorStatus flag;
void FLASH_ReadDatas(u32 Addr,u8 *Buffer,u16 Length)  
{        
    while(Length--)*Buffer++ = *(__IO uint32_t*)Addr++;  
} 
u32 FLASH_ReadDatas2(u32 Addr)  
{   
	  return *(__IO uint32_t*)Addr++; 
} 
#define StartServerManageFlashAddress    ((u32)0x08036000)//读写起始地址（内部flash的主存储块地址从0x08036000开始）

//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint32_t *writeData,uint16_t countToWrite)
{  
	uint16_t dataIndex;
	
	FLASH_Unlock();         //解锁写保护

	FLASH_ErasePage(startAddress);//擦除这个扇区

	for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
	{
	 FLASH_ProgramWord(startAddress+dataIndex,writeData[dataIndex]);
	}

	FLASH_Lock();//上锁写保护
}
#define	HIGH_BIT_IN_FRONT	0//高位在前
#define	LOW_BIT_IN_FRONT	1//低位在前
/*************************************************************
** 函数名称: ConvertBufferToLong
** 功能描述: 把4位unsigned char,高位在前的数据转成unsigned long;
             (与Public_ConvertLongToBuffer功能相反)
** 入口参数: buf:需要转换的数据的首地址
** 出口参数: 
** 返回参数: 转换结果
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned long ConvertBufferToLong(unsigned char *buf,unsigned char mode)
{
    unsigned long value;
    value = 0;
		if(HIGH_BIT_IN_FRONT == mode)
    {
    value |= buf[0]<<24;
    value |= buf[1]<<16;
    value |= buf[2]<<8;
    value |= buf[3];
		}
		else
		{
			value |= buf[3]<<24;
			value |= buf[2]<<16;
			value |= buf[1]<<8;
			value |= buf[0];
		}
    return value;
}
static u16	value_bkp = 0;
int main(void)
{  
	u8	Buffer[20];
	u8	UpdateRequestFlag = 0;//升级请求标志,1为有升级请求
	s32	UpdateFileLen = 0;//升级文件长度
	u32  ulcrcCode,readcrcCode;	//升级文件校验码
	s32	i;
	s32	j;
	u32	ReadAddr;
	u32	WriteAddr,WriteAddrBackup;
	u32	Data;
	u32 ReadData;
	u8  err = 0;	
	
	RCC_ClocksTypeDef RCC_ClocksStatus;
	
  RCC_Configuration();   
	RCC_GetClocksFreq(&RCC_ClocksStatus);

	NVIC_Configuration();

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|
								RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  PWR_BackupAccessCmd(ENABLE);
	
  Beep_Init();
	
	SPI_FLASH_Init();
	
	E2prom_Init();
			
	Eeprom_CheckChip();
	
	FLASH_CheckChip();
	
	E2prom_ReadByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR, Buffer, 1);
	UpdateRequestFlag = Buffer[0];
	 
  E2prom_ReadByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR, Buffer, 4);
		UpdateFileLen = ConvertBufferToLong(Buffer,HIGH_BIT_IN_FRONT);
		//读校验码  
		E2prom_ReadByte(E2_PROGRAM_UPDATA_VERIFYCODE_ID_ADDR, Buffer, 4);
	  readcrcCode = ConvertBufferToLong(Buffer,HIGH_BIT_IN_FRONT);   
		if((UpdateFileLen > 0)&&(UpdateFileLen < (512-16)*1024)&&(0xaa == UpdateRequestFlag))
		{ 
			FLASH_Unlock();//解锁
		
			for(i=8; i<256; i++)//擦除flash，每个扇区2K，最大是512K的flash,应用程序从地址0x8004000开始
			{
				WriteAddr = i*2048+0x8000000;
				FLASH_ErasePage(WriteAddr);
			}
			
			//写flash，每次写512字节
			ReadAddr = FLASH_UPDATE_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
			WriteAddr = 0x8004000;
			MOD_InitCRC();
			for(;;)
			{
				if(UpdateFileLen >= 512)
				{
					SPI_FLASH_BufferRead(UpdateBuffer, ReadAddr, 512);
					if(!err)
						ulcrcCode=MOD_CalcCRC(UpdateBuffer, 512);
					
					j = 512/4;
					WriteAddrBackup = WriteAddr;//
					for(i=0; i<j; i++)
					{
						Data = ConvertBufferToLong(&UpdateBuffer[i*4],LOW_BIT_IN_FRONT);
						FLASH_ProgramWord(WriteAddr, Data);
						ReadData = FLASH_ReadDatas2(WriteAddr);
						if(Data != ReadData)//数据不对擦除扇区重新写数据
						{
						  err = 1;
							WriteAddr = WriteAddrBackup;
							FLASH_ErasePage(WriteAddr);
							i = 0;
							break;
						}
						else  
						{
							err = 0;
							WriteAddr += 4;
						}
					}
					if(!err)
					{
						UpdateFileLen -= 512;
						ReadAddr += 512;
					}
				}
				else
				{
					WriteAddrBackup = WriteAddr;
					memset(UpdateBuffer,0xff,sizeof(UpdateBuffer));
					SPI_FLASH_BufferRead(UpdateBuffer, ReadAddr, UpdateFileLen);
					if(!err)
						ulcrcCode=MOD_CalcCRC(UpdateBuffer, UpdateFileLen-22);
					
					if(0 == UpdateFileLen%4)
					{
						j = UpdateFileLen/4;
					}
					else
					{
						j = UpdateFileLen/4+1;
					}
					for(i=0; i<j; i++)
					{
						Data = ConvertBufferToLong(&UpdateBuffer[i*4],LOW_BIT_IN_FRONT);
						FLASH_ProgramWord(WriteAddr, Data);
						ReadData = FLASH_ReadDatas2(WriteAddr);
						if(Data != ReadData)//数据不对擦除扇区重新写数据
						{
						  err = 1;
							WriteAddr = WriteAddrBackup;
							FLASH_ErasePage(WriteAddr);
							i = 0;
							break;
						}
						else
						{
							err = 0;
							WriteAddr += 4;
						}
					}
					if(!err)break;
				}
				 
				if((UpdateFileLen < 0)
						||(ReadAddr >= FLASH_UPDATE_END_SECTOR*FLASH_ONE_SECTOR_BYTES)
							||(WriteAddr >= 0x8080000))
				{
					break;
				}
				          
			}
			//校验不通过重新来过
			if(readcrcCode != ulcrcCode)
			{ 
				value_bkp = BKP_ReadBackupRegister(BKP_DR2);				
				if(value_bkp < 3)
				{
					BKP_WriteBackupRegister(BKP_DR2, value_bkp+1);
					NVIC_SystemReset();
				}
				else
				{
					BKP_WriteBackupRegister(BKP_DR2, 0);
					//升级标志，长度清0
					E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR, 0xbb);
					for(i=0; i<4; i++)
					{
						E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR+i,0);
					}
				}
			} 
			else
			{
				//升级标志，长度清0
				E2prom_WriteByte(E2_PROGRAM_UPDATA_REQUEST_ID_ADDR, 0xbb);
				for(i=0; i<4; i++)
				{
					E2prom_WriteByte(E2_PROGRAM_UPDATA_LENGTH_ID_ADDR+i,0);
				}
			}
			FLASH_Lock();//锁定
		}
	  
	JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
	Jump_To_Application = (pFunction) JumpAddress;
	__set_MSP(*(__IO uint32_t*) ApplicationAddress);
	Jump_To_Application();
	
	BEEP_ON();
	for(i=0; i<1000000; i++)
	{
			
	}
	BEEP_OFF();
	
	
  while (1)
  {
		
  }
}
/**
  * @brief  NVIC_Configuration.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

}
/**
  * @brief  GPIO_Init.
  * @param  None
  * @retval None
  */
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	BEEP_OFF();

}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */

void RCC_Configuration(void)
{   
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
