/******************************************************************** 
//��Ȩ˵��	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����	:fm25c160.c
//����		:��������
//�汾��	:V0.1
//������	:dxl
//����ʱ��	:2010.03
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
***********************************************************************/

//********************************ͷ�ļ�************************************
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "spi_flash.h"
#include "fm25c160.h"

//********************************�Զ�����������****************************

//********************************�궨��************************************
//�����ֽ�
#define	FRAM_WREN			0x06	//Write Enabled
#define FRAM_WRDI			0x04	//Write Disabled
#define FRAM_RDSR			0x05	//Read Status Register
#define FRAM_WRSR			0x01	//Write Status Register
#define FRAM_READ			0x03	//Read Data from Memory
#define FRAM_WRITE		0x02	//Write Data to Memory

#define FRAM_WEL_BIT			0x02	//bit1
#define FRAM_WPEN_BIT			0x80	//bit7
#define FRAM_BP1BP0_BIT		0x0C	//bit3,bit2
#define FRAM_DUMMY_BYTE         0xA5

//********************************ȫ�ֱ���**********************************

//********************************�ⲿ����**********************************

//********************************���ر���**********************************

//********************************��������**********************************
static void FRAM_WriteEnable(void);
static void FRAM_WriteDisable(void);
//static void FRAM_WriteStatusRegister(u8 Byte);
static u8   FRAM_ReadStatusRegister(void);

//********************************��������***********************************
/*********************************************************************
//��������	:FRAM_Init()
//����		:�����ʼ��
//����		:
//		:
//		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void FRAM_Init(void)
{
	
	SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable SPI1 and GPIO clocks */
  RCC_APB2PeriphClockCmd(SPI_FRAM_SPIx_RCC|SPI_FRAM_CLK_RCC|SPI_FRAM_MISO_RCC|SPI_FRAM_MOSI_RCC|
	SPI_FRAM_WP_RCC|SPI_FRAM_CS_RCC, ENABLE);

  /* Configure SPI1 pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = SPI_FRAM_CLK_PIN | SPI_FRAM_MISO_PIN | SPI_FRAM_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI_FRAM_CLK_PORT, &GPIO_InitStructure);

  /* Configure I/O for Flash Chip select */
  GPIO_InitStructure.GPIO_Pin = SPI_FRAM_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_FRAM_CS_PORT, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  FRAM_CS_HIGH();
  
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
/*********************************************************************
//��������	:FRAM_CheckChip(void)
//����		:�������оƬ�����Ƿ�����
//����		:�����ַ����������ֽ������Լ�
//		:
//		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
ErrorStatus FRAM_CheckChip(void)
{
	u8	ReadBuffer[3] ={0};
	u8	WriteBuffer[3] = {0};
	u16	Address;
	u8	temp;
	ErrorStatus	ErrorFlag = SUCCESS;
	
	//��״̬�Ĵ���
	temp = FRAM_ReadStatusRegister();
	if(0 != (FRAM_BP1BP0_BIT & temp))
	{
		return ERROR;
	}
	//���дæ��־
	FRAM_WriteDisable();
	
	Address = FRAM_CHIP_END_ADDR-2;
	WriteBuffer[0] = 0x5a;
	FRAM_BufferWrite(Address, WriteBuffer, 1);
	FRAM_BufferRead(ReadBuffer, 1, Address);
	if(ReadBuffer[0] == WriteBuffer[0])//Ԥ����������д��һ��ͬ����ֵ
	{
		WriteBuffer[0] = 0xa5;
		FRAM_BufferWrite(Address, WriteBuffer, 1);
		FRAM_BufferRead(ReadBuffer, 1, Address);
		if(ReadBuffer[0] == WriteBuffer[0])
		{
		
		}
		else
		{
			ErrorFlag = ERROR;
		}
	}
	else
	{
		ErrorFlag = ERROR;
	}
	return ErrorFlag;
	
}
/*********************************************************************
//��������	:FRAM_EraseChip(void)
//����		:������������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:״̬�Ĵ�����ֵ
//��ע		:
*********************************************************************/
void  FRAM_EraseChip(void)
{
	u16	i;
	u8	flag = 0;
        
  SPI_FLASH_Init();
  
	for(i=0; i<FRAM_CHIP_END_ADDR-2; i++)
	{
		FRAM_BufferWrite(i, &flag, 1);
	}
}
/*********************************************************************
//��������	:FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
//����		:��������д����,������ĳβ����У���ֽ�
//����		:WriteAddr�������ַ
//		:pBuffer�����ݻ���
//		:NumBytesToWrite��д����ֽ���
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
{
	u8	temp;
	u16	i;
	u8	*p;
	u8	sum = 0;
	static u8 count = 0;
	
	p = pBuffer;
        
	SPI_FLASH_Init();
        
	//��״̬�Ĵ���
	temp = FRAM_ReadStatusRegister();
	
	if(FRAM_WEL_BIT == (temp &FRAM_WEL_BIT))//����д��ֱ������
	{
		count++;
		if(count > 20)//����20��дʧ�����д��־
		{
			count = 0;
			FRAM_WriteDisable();
		}
		return ;
	}
	else
	{
		count = 0;
		//��У���
		sum = 0;
		for(i=0; i<NumBytesToWrite; i++)
		{
			sum += *(pBuffer+i);
		}
		//дʹ��
		FRAM_WriteEnable();

		//Ƭѡ����
		FRAM_CS_LOW();

		//����д����
		SPI_FLASH_SendByte(FRAM_WRITE);

		//���͵�ַ���ֽ�
		temp = WriteAddr >> 8;
		SPI_FLASH_SendByte(temp);

		//���͵�ַ���ֽ�
		temp = WriteAddr;
		SPI_FLASH_SendByte(temp);

		//д��������
		for(i=0; i<NumBytesToWrite; i++)
		{
			temp = *p++;
			SPI_FLASH_SendByte(temp);
		}
		
		//дУ���ֽ�
		SPI_FLASH_SendByte(sum);
		
		//Ƭѡ����
		FRAM_CS_HIGH();
	}
}
/*********************************************************************
//��������	:FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
//����		:�������������
//����		:ReadAddr�������ַ
//		:pBuffer��Ŀ�껺��
//		:NumBytesToRead���������ֽ���
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:ʵ�ʶ������ֽ���
//��ע		:
*********************************************************************/
u8 FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
{
	u8 	temp;
	u16	i;
	u8	*p;
	u8	verify;
	u8	sum;
	
	p = pBuffer;
        
	SPI_FLASH_Init();
        
	//Ƭѡ����
	FRAM_CS_LOW();

	//���Ͷ�����
	SPI_FLASH_SendByte(FRAM_READ);

	//���͵�ַ���ֽ�
	temp = ReadAddr >> 8;
	SPI_FLASH_SendByte(temp);

	//���͵�ַ���ֽ�
	temp = ReadAddr;
	SPI_FLASH_SendByte(temp);

	//���洢������
	for(i=0; i<NumBytesToRead; i++)
	{
		*p++ = SPI_FLASH_SendByte(FRAM_DUMMY_BYTE);
	}
	
	//��У���ֽ�
	verify = SPI_FLASH_SendByte(FRAM_DUMMY_BYTE);
	
	//Ƭѡ����
	FRAM_CS_HIGH();
	
	//����У���
	sum = 0;
	for(i=0; i<NumBytesToRead; i++)
	{
		sum += *(pBuffer+i);
	}
	
	//�ж�У����Ƿ�һ��
	if(verify == sum)
	{
		return NumBytesToRead;
	}
	else
	{
		return 0;
	}
	
}

/*********************************************************************
//��������	:FRAM_WriteEnable(void)
//����		:����дʹ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
static void FRAM_WriteEnable(void)
{
	//����Ƭѡ
	FRAM_CS_LOW();

	//����дʹ������
	SPI_FLASH_SendByte(FRAM_WREN);

	//����Ƭѡ
	FRAM_CS_HIGH();
}
/*********************************************************************
//��������	:FRAM_WriteDisable(void)
//����		:����д��ֹ
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
static void FRAM_WriteDisable(void)
{
	//����Ƭѡ
	FRAM_CS_LOW();

	//���Ͳ�ʹ��д����
	SPI_FLASH_SendByte(FRAM_WRDI);

	//����Ƭѡ
	FRAM_CS_HIGH();
}
/*********************************************************************
//��������	:FRAM_WriteStatusRegister(u8 Byte)
//����		:д����״̬�Ĵ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:/WP�����Ǹߵ�ƽ����Ч��ƽ,/WP�����Ǳ���д����״̬�Ĵ���
*********************************************************************/
/*
static void FRAM_WriteStatusRegister(u8 Byte)
{

  SPI_FLASH_Init();
         
	//����Ƭѡ
 	FRAM_CS_LOW();

	//����д״̬�Ĵ�������
	SPI_FLASH_SendByte(FRAM_WRSR);

	//������д�ֽ�
	SPI_FLASH_SendByte(Byte);

	//Ƭѡ����
	FRAM_CS_HIGH();
}
*/
/*********************************************************************
//��������	:FRAM_ReadStatusRegister(void)
//����		:������״̬�Ĵ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:״̬�Ĵ�����ֵ
//��ע		:
*********************************************************************/
static u8   FRAM_ReadStatusRegister(void)
{
	u8	temp;
        
  //SPI_FLASH_Init();
        
	//����Ƭѡ
 	FRAM_CS_LOW();

	//���Ͷ�״̬�Ĵ�������
	SPI_FLASH_SendByte(FRAM_RDSR);

	//��״̬�Ĵ���
	temp = SPI_FLASH_SendByte(FRAM_DUMMY_BYTE);

	//Ƭѡ����
	FRAM_CS_HIGH();

	//����
	return temp;
}


