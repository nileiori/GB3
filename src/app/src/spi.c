#include "include.h" 
#include "spi.h"

#define SPIx_IO_Simulate_DEBUG 0

SPI_InitTypeDef  SPI_InitStructure;

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25X16/24L01/JF24C							  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI3�ĳ�ʼ��

#if SPIx_IO_Simulate_DEBUG

void SPIx_Init(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //!< Enable GPIO clocks 
  //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
  /*
  //!< Configure sd SD_Power pin in output pushpull mode 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  */
  //!< Configure sd Card CS pin in output pushpull mode 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //!< Configure SCK pin in output pushpull mode 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL;dxl,2014.2.28
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  //!< Configure MOSI pin in output pushpull mode 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL;dxl,2014.2.28
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
  
  //!< Configure MISO pin in Input pushpull mode 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;dxl,2014.2.28
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  //!< Configure SD_CD pin in Input pushpull mode 
  //���SD������״̬,0�������룬1���޿�����������û��ʹ�ô˹��ܡ�
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;dxl,2014.2.28
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //SD_SD_Power_RESET();//SD������
  
  //!< Deselect the FLASH: Chip Select high 
  SD_CS_HIGH();
  
  SD_SPIx_SCK_SET();//sck=1

  //!< Enable the sFLASH_SPI 
  SPIx_ReadWriteByte(0xff);    //��������	

}

void SPIx_SetSpeed(u8 SpeedSet)
{

} 

//IOģ��SPI���߶�д������ģʽ3��CPOL = 1��CPHA = 1��
//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPIx_ReadWriteByte(u8 TxData)
{
    u8 i,RxData = 0,TxDataTemp;
    
    TxDataTemp = TxData;   
    SD_SPIx_SCK_SET();  //sck = 1������Ϊ��
    
    for (i = 0; i < 8; i++)
    {  
        SD_SPIx_SCK_RESET();    //��һ���������䣬MOSI���ݲ��ᱻ����
        if(TxDataTemp & 0x80)
        {
            SD_SPIx_MOSI_SET(); //MOSI = 1
        }
        else
        {
            SD_SPIx_MOSI_RESET(); //MOSI = 0
        }
        
        TxDataTemp <<= 1; 
        SD_SPIx_SCK_SET();  //������д�룬MOSI���ݱ�������ͬʱMISO���������Ѿ��������

        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2))    //MISO���ݲ���
        {
            RxData |= 0x01; 
        }
        if(i < 7)
        {
            RxData <<= 1;
        }       
    }
    return RxData;
}

#else

void SPIx_Init(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//���һ��Ҫ���ϣ���ΪPB3/4��JTAG����
	
  /*!< Enable the SPI clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF6 */  
 // GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
  //GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);
  //GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);
 //  GPIO_PinRemapConfig(GPIO_Remap_SPI3,ENABLE);
	
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 // GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; //GPIO_PuPd_NOPULL; GPIO_PuPd_DOWN;
        
  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure sd Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
  
  //!< Configure SD_CD pin in Input pushpull mode 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//dxl,2014.2.28
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /*!< Deselect the FLASH: Chip Select high */
  SD_CS_HIGH();
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI3, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(SPI3, ENABLE);
  SPIx_ReadWriteByte(0xff);    //��������
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_4   4��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_32  32��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_64  64��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_128 128��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_256 256��Ƶ (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3,ENABLE);
} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPIx_ReadWriteByte(u8 TxData)
{
    u8 retry=0;
    /*!< Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET)  //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
    {//���ͻ���ǿ�
        retry++;    
    	if(retry>200)return 0;
    }
    //���ͻ����
    /*!< Send byte through the SPI3 peripheral */
    SPI_I2S_SendData(SPI3,TxData);  //ͨ������SPIx����һ������
    retry=0;
    /*!< Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{//���ջ����
		retry++;
		if(retry>200)return 0;
	}//���ջ���ǿ�
    /*!< Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI3); //����ͨ��SPIx������յ�����	
}

#endif













