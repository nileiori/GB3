/************************************************************************* 
*
* Copyright (c) 2013,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : E2prom_25LC320A.c
* ����     : E2prom_25LC320A_SPI�Ķ�д����.
*
* ��ǰ�汾 : 1.0
* ������   : Senon Dou
* �޸�ʱ�� : 2013.07.26
*
* ��ʷ�汾 : 
* ������   : 
* ���ʱ�� : 
* 
* ��ע :    1 - ʹ��SPI1���в�����ע��EEPROM��FRAM��FLASH������SPI1��
                ͨ��Ƭѡ����ѡ����в�����

            2 - �������е�����spi_flash.c�Ĳ��ֺ�����������
                uint8_t SPI_FLASH_SendByte(uint8_t byte);
                uint8_t SPI_FLASH_ReadByte(void);

            3 - SPI1�ĳ�ʼ����spi_flash.c�У���������ִ�г�ʼ��������
*************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "spi_flash.h"
#include "E2prom_25LC320A.h"

/********************************************************************
* ���� : E2prom_Init
* ���� : 25LC320A_SPI��GPIO��ʼ��������25LC320A_CS��25LC320A_WP��
*
* ����:   none
* ���:   none
*
* ȫ�ֱ���:   none
* ���ú���:  none
*
* �ж���Դ:  none
*
* ��ע:   none
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
* ���� : E2prom_ReadByte
* ���� : E2PROM_25LC320A���ֽڲ���
*
* ����: Address ������ʼ��ַ
        *destin �������ݴ�ŵ�ַָ��
        bytenum �����ֽ���
* ���: none
*
* ȫ�ֱ���:  none
* ���ú���: uint8_t SPI_FLASH_SendByte(uint8_t byte);
            uint8_t SPI_FLASH_ReadByte(void);
            u8 E2prom_ReadStatusRegister();
*
* �ж���Դ: none
*
* ��ע: 
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
  
    //sFLASH_LowLevel_Init(); //��ʼ��SPI1������EEPROM��FRAM��FLASH����SPI1������Ĭ��SPI1�ѱ���ʼ��
    
    //���д��־���ȴ�д���
    while(1)    
    {
        count++;
        //��״̬�Ĵ���
        temp = E2prom_ReadStatusRegister();
        if(E2PROM_WIP_BIT == (temp &E2PROM_WIP_BIT))//����д������
	    {
            spi_Delay_uS(1000);//1ms���ȴ�
        }
        else
        {
            break;  //����
        }
        if(count > 50) //���ȴ�50ms
        {
            return;  //����
        }
    }
    
    E2PROM_CS_LOW();
      
    SPI_FLASH_SendByte(READ);    //���Ͷ�����
     
    addr = Address >> 8;
    SPI_FLASH_SendByte(addr);  //���͵�ַ���ֽ�  
    addr = Address;
    SPI_FLASH_SendByte(addr);  //���͵�ַ���ֽ�
     
    for(i = 0; i < bytenum; i++)
    {
        *p++ = SPI_FLASH_ReadByte();  //���洢������
    }
    
    E2PROM_CS_HIGH();
  
}

/********************************************************************
* ���� : E2prom_WriteByte
* ���� : E2PROM_25LC320Aд�ֽڲ���
*
* ����: Address д�����ʼ��ַ
        data    д�������
* ���: SUCCESS �ɹ�
        ERROR   ʧ��
*
* ȫ�ֱ���:  none
* ���ú���: uint8_t SPI_FLASH_SendByte(uint8_t byte);
            uint8_t SPI_FLASH_ReadByte(void);
            u8 E2prom_ReadStatusRegister();
*
* �ж���Դ: none
*
* ��ע:  none
********************************************************************/
u8 E2prom_WriteByte(u16 Address, u8 data)
{
    u8 addr;
    u8 temp;
    u8 count = 0;

    //sFLASH_LowLevel_Init();//��ʼ��SPI1������EEPROM��FRAM��FLASH����SPI1������Ĭ��SPI1�ѱ���ʼ��
    
    //���д��־���ȴ�д���
    while(1)    
    {
        count++;
        //��״̬�Ĵ���
        temp = E2prom_ReadStatusRegister();
        if(E2PROM_WIP_BIT == (temp &E2PROM_WIP_BIT))//����д������
	    {
            spi_Delay_uS(1000);//1ms���ȴ�
        }
        else
        {
            break;  //����
        }
        if(count > 50) //���ȴ�50ms
        {
            return ERROR;  //����
        }
    }
    
    E2PROM_CS_LOW();        
    SPI_FLASH_SendByte(WREN);    //дʹ������
    E2PROM_CS_HIGH();
    
    E2PROM_CS_LOW();
    
    SPI_FLASH_SendByte(WRITE);    //����д����
    
    addr = Address >> 8;
    SPI_FLASH_SendByte(addr);  //���͵�ַ���ֽ�
    addr = Address;
    SPI_FLASH_SendByte(addr);  //���͵�ַ���ֽ�
    
    SPI_FLASH_SendByte(data); //�������ݣ�������ɺ� д����=RESET
  
    E2PROM_CS_HIGH();

    return SUCCESS;
}

/********************************************************************
* ���� : E2prom_ReadStatusRegister(void)
* ���� : ��״̬�Ĵ���
*
* ����:   none
* ���: ״̬�Ĵ�����ֵ
*
* ȫ�ֱ���:  none
* ���ú���: uint8_t SPI_FLASH_SendByte(uint8_t byte);
            uint8_t SPI_FLASH_ReadByte(void);
*
* �ж���Դ: none
*
* ��ע:  none
********************************************************************/
u8 E2prom_ReadStatusRegister(void)
{
	u8	temp;
        
    //sFLASH_LowLevel_Init();
        
	//����Ƭѡ
 	E2PROM_CS_LOW();

	//���Ͷ�״̬�Ĵ�������
	SPI_FLASH_SendByte(RDSR);

	//��״̬�Ĵ���
	temp = SPI_FLASH_ReadByte();

	//Ƭѡ����
	E2PROM_CS_HIGH();

	//����
	return temp;
}

/********************************************************************
* ���� : Eeprom_CheckChip(void)
* ���� : ���EEPROMоƬ�����Ƿ�����
*        eeprom��ַ����������ֽ��������Լ���
* ����:  none
* ���:  none
*
* ȫ�ֱ���: E2_LAST_ADDR  E2prom����������ֽڵĵ�ַ
* ���ú���: u8 E2prom_WriteByte(u16 Address, u8 data)
            void E2prom_ReadByte(u16 Address, u8 *destin, u8 bytenum)
*
* �ж���Դ: none
*
* ��ע: none
********************************************************************/
ErrorStatus Eeprom_CheckChip(void)
{
	u8	i;
	u8	ReadBuffer[3] = {0};
	u8	WriteBuffer[3] = {"OK"};
	u16	Address;
	
	Address = E2PROM_CHIP_END_ADDR-2;   //���һ���ֽ�2*1024 - 2
	for(i=0; i<3; i++)
	{
		//����ַ��������ֽ�д����
		E2prom_WriteByte(Address, WriteBuffer[0]);
		E2prom_WriteByte(Address+1, WriteBuffer[1]);
        
		//������д����ֽ���
		E2prom_ReadByte(Address,ReadBuffer,2);
		
		//�Ƚ�д�������������������Ƿ�һ��
		if((WriteBuffer[0] == ReadBuffer[0])&&(WriteBuffer[1] == ReadBuffer[1]))
		{
		    return SUCCESS;
		}
	}
	
	return ERROR;
}
