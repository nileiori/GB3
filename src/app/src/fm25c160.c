/******************************************************************** 
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :fm25c160.c
//����      :��������
//�汾��    :V0.1
//������    :dxl
//����ʱ��  :2010.03
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/

//********************************ͷ�ļ�************************************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"

//********************************�Զ�����������****************************

//********************************�궨��************************************
//�����ֽ�
#define FRAM_WREN   0x06    //Write Enabled
#define FRAM_WRDI   0x04    //Write Disabled
#define FRAM_RDSR   0x05    //Read Status Register
#define FRAM_WRSR   0x01    //Write Status Register
#define FRAM_READ   0x03    //Read Data from Memory
#define FRAM_WRITE  0x02    //Write Data to Memory

#define FRAM_WEL_BIT    0x02    //bit1
#define FRAM_WPEN_BIT   0x80    //bit7
#define FRAM_BP1BP0_BIT 0x0C    //bit3,bit2


//********************************ȫ�ֱ���**********************************
//****************���ļ�û���õ��κ�ȫ�ֱ���**************

//********************************�ⲿ����**********************************
#ifdef FRAM_SINGLE_TEST
extern u8 UART4_TxBuffer[];
#endif
//********************************���ر���**********************************
//****************���ļ�û���õ��κα��ر���**************

//********************************��������**********************************

//********************************��������***********************************
/*********************************************************************
//��������  :FRAM_Init()
//����      :�����ʼ��
//����      :
//      :
//      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void FRAM_Init(void)
{
        GpioOutInit(FR_CS);
        
        //Ƭѡ��ʼ��Ϊ�ߵ�ƽ
        FRAM_CS_HIGH();
}
/*********************************************************************
//��������  :CheckFramChip(void)
//����      :�������оƬ�����Ƿ�����
//����      :�����ַ����������ֽ������Լ�
//      :
//      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
ErrorStatus CheckFramChip(void)
{
    u8  ReadBuffer[3] ={0};
    u8  WriteBuffer[3] = {0};
    u16 Address;
    u8  temp;
    ErrorStatus ErrorFlag = SUCCESS;
    
    //��״̬�Ĵ���
    temp = FRAM_ReadStatusRegister();
    if(0 != (FRAM_BP1BP0_BIT & temp))
    {
        return ERROR;
    }
    //���дæ��־
    FRAM_WriteDisable();
    
    Address = FRAM_LAST_ADDR;
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
//��������  :FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
//����      :��������д����,������ĳβ����У���ֽ�
//����      :WriteAddr�������ַ
//      :pBuffer�����ݻ���
//      :NumBytesToWrite��д����ֽ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
{
    u8  temp;
    u16 i;
    u8  *p;
    u8  sum = 0;
    static u8 count = 0;
    
    p = pBuffer;
        
    SPI_INTERFACE_INIT();
        
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
        sFLASH_SendByte(FRAM_WRITE);

        //���͵�ַ���ֽ�
        temp = WriteAddr >> 8;
        sFLASH_SendByte(temp);

        //���͵�ַ���ֽ�
        temp = WriteAddr;
        sFLASH_SendByte(temp);

        //д��������
        for(i=0; i<NumBytesToWrite; i++)
        {
            temp = *p++;
            sFLASH_SendByte(temp);
        }
        
        //дУ���ֽ�
        sFLASH_SendByte(sum);
        
        //Ƭѡ����
        FRAM_CS_HIGH();
    }
}
/*********************************************************************
//��������  :FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
//����      :�������������
//����      :ReadAddr�������ַ
//      :pBuffer��Ŀ�껺��
//      :NumBytesToRead���������ֽ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :ʵ�ʶ������ֽ���
//��ע      :
*********************************************************************/
u8 FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
{
    u8  temp;
    u16 i;
    u8  *p;
    u8  verify;
    u8  sum;
    u8	zeroCount = 0;
	
    p = pBuffer;
        
    SPI_INTERFACE_INIT();
        
    //Ƭѡ����
    FRAM_CS_LOW();

    //���Ͷ�����
    sFLASH_SendByte(FRAM_READ);

    //���͵�ַ���ֽ�
    temp = ReadAddr >> 8;
    sFLASH_SendByte(temp);

    //���͵�ַ���ֽ�
    temp = ReadAddr;
    sFLASH_SendByte(temp);

    //���洢������
    for(i=0; i<NumBytesToRead; i++)
    {
        *p++ = sFLASH_ReadByte();
    }
    
    //��У���ֽ�
    verify = sFLASH_ReadByte();
    
    //Ƭѡ����
    FRAM_CS_HIGH();
    
    //����У���
    sum = 0;
    for(i=0; i<NumBytesToRead; i++)
    {
        sum += *(pBuffer+i);
		if(0==sum)zeroCount++;
    }
    if(zeroCount == NumBytesToRead)return 0;//o shit all the data is zero
    
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
//��������  :FRAM_WriteEnable(void)
//����      :����дʹ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void FRAM_WriteEnable(void)
{
    //����Ƭѡ
    FRAM_CS_LOW();

    //����дʹ������
    sFLASH_SendByte(FRAM_WREN);

    //����Ƭѡ
    FRAM_CS_HIGH();
}
/*********************************************************************
//��������  :FRAM_WriteDisable(void)
//����      :����д��ֹ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void FRAM_WriteDisable(void)
{
    //����Ƭѡ
    FRAM_CS_LOW();

    //���Ͳ�ʹ��д����
    sFLASH_SendByte(FRAM_WRDI);

    //����Ƭѡ
    FRAM_CS_HIGH();
}
/*********************************************************************
//��������  :FRAM_WriteStatusRegister(u8 Byte)
//����      :д����״̬�Ĵ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :/WP�����Ǹߵ�ƽ����Ч��ƽ,/WP�����Ǳ���д����״̬�Ĵ���
*********************************************************************/
void FRAM_WriteStatusRegister(u8 Byte)
{

        SPI_INTERFACE_INIT();
         
    //����Ƭѡ
    FRAM_CS_LOW();

    //����д״̬�Ĵ�������
    sFLASH_SendByte(FRAM_WRSR);

    //������д�ֽ�
    sFLASH_SendByte(Byte);

    //Ƭѡ����
    FRAM_CS_HIGH();
}
/*********************************************************************
//��������  :FRAM_ReadStatusRegister(void)
//����      :������״̬�Ĵ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :״̬�Ĵ�����ֵ
//��ע      :
*********************************************************************/
u8   FRAM_ReadStatusRegister(void)
{
    u8  temp;
        
        SPI_INTERFACE_INIT();
        
    //����Ƭѡ
    FRAM_CS_LOW();

    //���Ͷ�״̬�Ĵ�������
    sFLASH_SendByte(FRAM_RDSR);

    //��״̬�Ĵ���
    temp = sFLASH_ReadByte();

    //Ƭѡ����
    FRAM_CS_HIGH();

    //����
    return temp;
}
/*********************************************************************
//��������  :FRAM_EraseChip(void)
//����      :������������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :״̬�Ĵ�����ֵ
//��ע      :
*********************************************************************/
void  FRAM_EraseChip(void)
{
    u16 i;
    u8  flag = 0;
        
  SPI_INTERFACE_INIT();
    for(i=0; i<FRAM_LAST_ADDR; i++)
    {
        FRAM_BufferWrite(i, &flag, 1);
    }
}
/*********************************************************************
//��������  :FRAM_EraseAllDvrArea(void)
//����      :��������DVR�õ��Ĵ洢��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :״̬�Ĵ�����ֵ
//��ע      :
*********************************************************************/
void  FRAM_EraseAllDvrArea(void)
{
    u16 i;
    u8  flag = 0;
        
  SPI_INTERFACE_INIT();
    for(i=FRAM_VDR_DOUBT_DATA_ADDR; i<FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR+10; i++)
    {
        FRAM_BufferWrite(i, &flag, 1);
    }
}

