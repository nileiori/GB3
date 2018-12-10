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
                uint8_t sFLASH_SendByte(uint8_t byte);
                uint8_t sFLASH_ReadByte(void);

            3 - SPI1�ĳ�ʼ����spi_flash.c�У���������ִ�г�ʼ��������
*************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"

/********************************************************************
* ���� : E2prom_InitIO
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
void E2prom_InitIO(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    /* ʵ��GPIOʱ��*/
    RCC_APB2PeriphClockCmd(E2PROM_25LC320A_CS_GPIO_CLK, ENABLE);
   
    /*!< Configure E2PROM 25LC320A CS pin in output pushpull mode ********************/
    GPIO_InitStructure.GPIO_Pin = E2PROM_25LC320A_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(E2PROM_25LC320A_CS_GPIO_PORT, &GPIO_InitStructure);
  
    RCC_APB2PeriphClockCmd(MEM_WP_GPIO_CLK, ENABLE);  
    /*!< Configure E2PROM 25LC320A wp pin in output pushpull mode ********************/
    GPIO_InitStructure.GPIO_Pin = MEM_WP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(MEM_WP_GPIO_PORT, &GPIO_InitStructure);
  
    //sFLASH_LowLevel_Init();//��ʼ��SPI1������EEPROM��FRAM��FLASH����SPI1������Ĭ��SPI1�ѱ���ʼ��
  
    E2PROM_25LC320A_CS_HIGH();
    E2PROM_25LC320A_WP_HIGH();
   
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
* ���ú���: uint8_t sFLASH_SendByte(uint8_t byte);
            uint8_t sFLASH_ReadByte(void);
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
    
    E2PROM_25LC320A_CS_LOW();
      
    sFLASH_SendByte(READ);    //���Ͷ�����
     
    addr = Address >> 8;
    sFLASH_SendByte(addr);  //���͵�ַ���ֽ�  
    addr = Address;
    sFLASH_SendByte(addr);  //���͵�ַ���ֽ�
     
    for(i = 0; i < bytenum; i++)
    {
        *p++ = sFLASH_ReadByte();  //���洢������
    }
    
    E2PROM_25LC320A_CS_HIGH();
  
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
* ���ú���: uint8_t sFLASH_SendByte(uint8_t byte);
            uint8_t sFLASH_ReadByte(void);
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
    
    E2PROM_25LC320A_CS_LOW();        
    sFLASH_SendByte(WREN);    //дʹ������
    E2PROM_25LC320A_CS_HIGH();
    
    E2PROM_25LC320A_CS_LOW();
    
    sFLASH_SendByte(WRITE);    //����д����
    
    addr = Address >> 8;
    sFLASH_SendByte(addr);  //���͵�ַ���ֽ�
    addr = Address;
    sFLASH_SendByte(addr);  //���͵�ַ���ֽ�
    
    sFLASH_SendByte(data); //�������ݣ�������ɺ� д����=RESET
  
    E2PROM_25LC320A_CS_HIGH();

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
* ���ú���: uint8_t sFLASH_SendByte(uint8_t byte);
            uint8_t sFLASH_ReadByte(void);
*
* �ж���Դ: none
*
* ��ע:  none
********************************************************************/
u8 E2prom_ReadStatusRegister(void)
{
	u8	temp;
        
    sFLASH_LowLevel_Init();
        
	//����Ƭѡ
 	E2PROM_25LC320A_CS_LOW();

	//���Ͷ�״̬�Ĵ�������
	sFLASH_SendByte(RDSR);

	//��״̬�Ĵ���
	temp = sFLASH_ReadByte();

	//Ƭѡ����
	E2PROM_25LC320A_CS_HIGH();

	//����
	return temp;
}

/********************************************************************
* ���� : CheckEepromChip(void)
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
ErrorStatus CheckEepromChip(void)
{
	u8	i;
	u8	ReadBuffer[2] = {0};
	u8	WriteBuffer[2] = {"OK"};
	u16	Address;
	
	Address = E2_LAST_ADDR;   //���һ���ֽ�2*1024 - 2
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