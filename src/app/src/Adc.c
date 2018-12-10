/******************************************************************** 
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Ad.c       
//����      :AD�źŲɼ�
//�汾��    :
//������    :dxl
//����ʱ��  :2012.6
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :1)
//              :�ɼ�ֵ��ʵ��ֵ��׼����취:
//              :a.ֱ��ʹ�õ�ѹ������ɼ���ĵ�ѹֵ��
//              :b.������ƽ����������ֱ�Ӳɼ�ֵ�Ƕ��١�
//              :ADC_KEY��ؼ�ֵ:
//              :  ADC_key0=20K/(20K+10K)*4095=2730;
//              :  ADC_key1=10K/(10K+10K)*4095=2047;
//              :  ADC_key2=5.1K/(5.1K+10K)*4095=1383;
//              :  ADC_key3=2K/(2K+10K)*4095=682;
***********************************************************************/
//***************�����ļ�*****************
#include "HAL.h"

//****************�궨��****************


//***************���ر���******************
  __IO u16  ADCDualConvertedValue[ADC_CHAN_MAX];

//
//ADC�ɼ�����������,����˳�����ADC_CHAN_TYPE������ͬ
//


const uint16_t ADC_IN_PIN[ADC_CHAN_MAX] = {
                                              ADC_EXTERN1_PIN,
                                              ADC_EXTERN2_PIN,
                                              ADC_MAIN_POWER_PIN,
                                              ADC_VBATIN_PIN,
                                              ADC_IO_OUT_PIN,
                                              ADC_MIC_PIN,
                                              ADC_KEY_PIN,
                                              ADC_PRINTF_PIN,
                                              ADC_PRINTF_THE_PIN,
                                           };
GPIO_TypeDef* ADC_IN_PORT[ADC_CHAN_MAX] = {
                                              ADC_EXTERN1_GPIO_PORT,
                                              ADC_EXTERN2_GPIO_PORT,
                                              ADC_MAIN_POWER_GPIO_PORT,
                                              ADC_VBATIN_GPIO_PORT,
                                              ADC_IO_OUT_GPIO_PORT,
                                              ADC_MIC_GPIO_PORT,
                                              ADC_KEY_GPIO_PORT,
                                              ADC_PRINTF_GPIO_PORT,
                                              ADC_PRINTF_THE_GPIO_PORT,
                                            };
const uint32_t ADC_IN_CLK[ADC_CHAN_MAX] = {
                                               ADC_EXTERN1_GPIO_CLK,
                                               ADC_EXTERN2_GPIO_CLK,
                                               ADC_MAIN_POWER_GPIO_CLK,
                                               ADC_VBATIN_GPIO_CLK,
                                               ADC_IO_OUT_GPIO_CLK,
                                               ADC_MIC_GPIO_CLK,                                               
                                               ADC_KEY_GPIO_CLK,
                                               ADC_PRINTF_GPIO_CLK,
                                               ADC_PRINTF_THE_GPIO_CLK,
                                           };
const uint8_t ADC_CHAN[ADC_CHAN_MAX] = {
                                               ADC_EXTERN1_CHAN,
                                               ADC_EXTERN2_CHAN,
                                               ADC_MAIN_POWER_CHAN,
                                               ADC_VBATIN_CHAN,
                                               ADC_IO_OUT_CHAN,
                                               ADC_MIC_CHAN,
                                               ADC_KEY_CHAN,
                                               ADC_PRINTF_CHAN,
                                               ADC_PRINTF_THE_CHAN,
                                       };
//***************��������******************
//��ʱ΢��
/*******************************************************************************/
void Adc_Delay_uS(u32 x)
{ u32 i;
  for(i = 0; i <= x; i ++);
}

/*********************************************************************
//��������  :Ad_Init
//����      :AD��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Ad_Init(void)
{       
        u8 i = 0;
  //  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    //ʹ��ʱ��
    RCC_AHBPeriphClockCmd(ADC_DMA_CLOCK, ENABLE);  //RCC_APB2Periph_ADC3   , ADC_CLOCK
    RCC_APB2PeriphClockCmd(ADC_CLOCK, ENABLE);

    Ad_Dma_Config();
	//AD��ص�IO��ʼ��
        for(i = 0 ; i < ADC_CHAN_MAX ; i++)
        {
	    Ad_Gpio_Config(i);
        }
	//ADC��ʼ��
        Ad_Config();
}
/*********************************************************************
//��������  :ADC3_Config
//����      :AD��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Ad_Config(void)
{
  u8 i = 0;
  ADC_InitTypeDef ADC_InitStructure;
 // ADC_CommonInitTypeDef ADC_CommonInitStructure;
  memset(&ADC_InitStructure,0,sizeof(ADC_InitTypeDef));
 // memset(&ADC_CommonInitStructure,0,sizeof(ADC_CommonInitTypeDef));
  
  /* ADC ????? */
 /* ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 
  ADC_CommonInit(&ADC_CommonInitStructure); 
*/
  /* ADC ?? ------------------------------------------------------*/
 /* ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; 
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = ADC_CHAN_MAX;
  ADC_Init(EVAL_ADC, &ADC_InitStructure);
*/
  /* ADC3 configuration ------------------------------------------------------*/
 RCC_ADCCLKConfig(RCC_PCLK2_Div8);

 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = ADC_CHAN_MAX;
  ADC_Init(EVAL_ADC, &ADC_InitStructure);
  /* ADC???????? */
 for(i = 0 ; i < ADC_CHAN_MAX ; i++)
  {
      ADC_RegularChannelConfig(EVAL_ADC, ADC_CHAN[i], (i+1), ADC_SampleTime_28Cycles5);
  }
//  ADC_RegularChannelConfig(EVAL_ADC, ADC_Channel_4, 1, ADC_SampleTime_1Cycles5);
  
  /* ??ADC?DMA?? */
  ADC_DMACmd(EVAL_ADC, ENABLE);
  
  /* ?? ADC */
  ADC_Cmd(EVAL_ADC, ENABLE);
  //????40us
 //Adc_Delay_uS(40);
  
  ADC_ResetCalibration(EVAL_ADC);
  /* Check the end of ADC3 reset calibration register */
 while(ADC_GetResetCalibrationStatus(EVAL_ADC));

  /* Start ADC3 calibration */
  ADC_StartCalibration(EVAL_ADC);
  /* Check the end of ADC3 calibration */
  while(ADC_GetCalibrationStatus(EVAL_ADC));
  /* ??ADC????? */ 
  ADC_SoftwareStartConvCmd(EVAL_ADC,ENABLE);
}

/*********************************************************************
//��������  :Ad_Dma_Config
//����      :��AD������ص�DMA����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Ad_Dma_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
        memset(&DMA_InitStructure,0,sizeof(DMA_InitTypeDef));
    /*    
    DMA_InitStructure.DMA_Channel = ;          
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCDualConvertedValue;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_DR_ADDRESS;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = ADC_CHAN_MAX;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(ADC_DMA_STREAM, &DMA_InitStructure);
*/
  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(ADC_DMA_STREAM);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCDualConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_CHAN_MAX;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(ADC_DMA_STREAM, &DMA_InitStructure);
      /* DMA2_Stream0 enable */
  DMA_Cmd(ADC_DMA_STREAM, ENABLE);
}

/*********************************************************************
//��������  :Ad_Gpio_Config
//����      :AD��IO��ʼ��
//����      :IoNum ���ű��
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Ad_Gpio_Config(u8 IoNum)
{
    GPIO_InitTypeDef GPIO_InitStructure;
        memset(&GPIO_InitStructure,0,sizeof(GPIO_InitTypeDef));
        
     RCC_APB2PeriphClockCmd(ADC_IN_CLK[IoNum], ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADC_IN_PIN[IoNum];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 //   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(ADC_IN_PORT[IoNum], &GPIO_InitStructure);   
}
/*********************************************************************
//��������  :Ad_GetValue
//����      :����AD���,��õ�ǰADֵ
//����      :adNum AD���,�ο�ADC_CHAN_TYPEö��.
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ǰ�ɼ�����ADֵ.
//��ע      :
*********************************************************************/
u16 Ad_GetValue(u8 adNum)
{
    u16 tmp = 0;
    tmp = ADCDualConvertedValue[adNum];
    return tmp;
}
/********************************************************************
* ���� : ��ʼ�� ģ��ת�� ����
*
* ���� : NULL
* ��� : NULL
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void DriveAdc_init(void) 
{
    GPIO_InitTypeDef     tempGPIO_InitStructure;
    ADC_InitTypeDef      ADC_InitStructure;
    //u16   temp = 0;
    //--------- ʱ�ӳ�ʼ�� --------------------------
    /* Enable ADC3 and GPIOB GPIOC clocks */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOF|RCC_APB2Periph_AFIO, ENABLE);
    //--------- IO�˿ڳ�ʼ�� ----------------------------
    /* Configure PC (ADC Channel 2,4,5,15) as analog inputs */
   /* tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    tempGPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
    tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &tempGPIO_InitStructure);      
 */
    tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
    tempGPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
    tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOF, &tempGPIO_InitStructure);

    
    tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
    tempGPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
    tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOF, &tempGPIO_InitStructure);
    //----------�жϳ�ʼ�� ---------------------------------------
    /* Configure and enable ADC interrupt */
    //  tempNVIC_InitStructure.NVIC_IRQChannel = ADC3_2_IRQChannel;
    //  tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //  tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    //  tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //  NVIC_Init(&tempNVIC_InitStructure);

    //----------------------------------------------------------------------------
    /* ADC3 configuration ------------------------------------------------------*/
    ADC_DeInit(ADC3);  //��λADC3,������ ADC3 ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //ADC3��ADC2��������ת��
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;       //ɨ��ģʽ�ر�
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����ģʽ�ر�
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//SWSTART��������ת��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;   //            // 1��ת��

    ADC_Init(ADC3, &ADC_InitStructure);
    //����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��

    /* DISABLE ADC3 EOC interupt */
    ADC_ITConfig(ADC3, ADC_IT_EOC, DISABLE);//�������ж�
    /* Enable ADC3 */
    ADC_Cmd(ADC3, ENABLE);
    Adc_Delay_uS(40);
    //����У׼ǰ,ADC���봦�ڹص�״̬(ADON='0')������������ADCʱ������.
    //����ָ����ADC��У׼�Ĵ���
    ADC_ResetCalibration(ADC3);//��λУ׼
    //��ȡADC����У׼�Ĵ�����״̬
    while(ADC_GetResetCalibrationStatus(ADC3));

    //��ʼָ��ADC��У׼״̬
    ADC_StartCalibration(ADC3);
    //��ȡָ��ADC��У׼����
    while(ADC_GetCalibrationStatus(ADC3));

    //ʹ�ܻ���ʧ��ָ����ADC�����ת����������
  ///  ADC_SoftwareStartConvCmd(ADC3, ENABLE);
    
    //temp = GetPowerV();
}

/********************************************************************
* ���� : DriveAdc_getVar
* ���� : ��� ģ��ת�� ��ֵ
*
* ���� : ͨ����
* ��� : �ɼ�����ADֵ;
*
* ��ע: 
********************************************************************/
u16 Ad_GetValue_two(u8 Channel) 
{

    ADC_RegularChannelConfig(ADC3, Channel, 1, ADC_SampleTime_239Cycles5);// 28.5����
    ADC_ClearFlag(ADC3, ADC_FLAG_EOC);
    ADC_Cmd(ADC3, ENABLE);
	Adc_Delay_uS(40);
    ADC_SoftwareStartConvCmd(ADC3, ENABLE);         //ʹ��ָ����ADC3�����ת����������          
   // Adc_Delay_uS(40);
    while(!(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC)));
   /* {
        if((delay--) < 0x07) {
            break;
        }
    }*/
    //return((ADC_GetConversionValue(ADC3)) & 0x0FFF);
   return((ADC_GetConversionValue(ADC3)) );
   
}

