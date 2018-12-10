/******************************************************************** 
//版权说明  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称  :Ad.c       
//功能      :AD信号采集
//版本号    :
//开发人    :dxl
//开发时间  :2012.6
//修改者    :
//修改时间  :
//修改简要说明  :
//备注      :1)
//              :采集值和实际值不准解决办法:
//              :a.直接使用电压表测量采集点的电压值。
//              :b.不进行平均采样，看直接采集值是多少。
//              :ADC_KEY相关键值:
//              :  ADC_key0=20K/(20K+10K)*4095=2730;
//              :  ADC_key1=10K/(10K+10K)*4095=2047;
//              :  ADC_key2=5.1K/(5.1K+10K)*4095=1383;
//              :  ADC_key3=2K/(2K+10K)*4095=682;
***********************************************************************/
//***************包含文件*****************
#include "HAL.h"

//****************宏定义****************


//***************本地变量******************
  __IO u16  ADCDualConvertedValue[ADC_CHAN_MAX];

//
//ADC采集因引脚配置,排列顺序必须ADC_CHAN_TYPE定义相同
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
//***************函数定义******************
//延时微秒
/*******************************************************************************/
void Adc_Delay_uS(u32 x)
{ u32 i;
  for(i = 0; i <= x; i ++);
}

/*********************************************************************
//函数名称  :Ad_Init
//功能      :AD初始化
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
void Ad_Init(void)
{       
        u8 i = 0;
  //  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    //使能时钟
    RCC_AHBPeriphClockCmd(ADC_DMA_CLOCK, ENABLE);  //RCC_APB2Periph_ADC3   , ADC_CLOCK
    RCC_APB2PeriphClockCmd(ADC_CLOCK, ENABLE);

    Ad_Dma_Config();
	//AD相关的IO初始化
        for(i = 0 ; i < ADC_CHAN_MAX ; i++)
        {
	    Ad_Gpio_Config(i);
        }
	//ADC初始化
        Ad_Config();
}
/*********************************************************************
//函数名称  :ADC3_Config
//功能      :AD初始化
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
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
//函数名称  :Ad_Dma_Config
//功能      :与AD采样相关的DMA配置
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
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
//函数名称  :Ad_Gpio_Config
//功能      :AD的IO初始化
//输入      :IoNum 引脚编号
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
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
//函数名称  :Ad_GetValue
//功能      :根据AD编号,获得当前AD值
//输入      :adNum AD编号,参考ADC_CHAN_TYPE枚举.
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :当前采集到的AD值.
//备注      :
*********************************************************************/
u16 Ad_GetValue(u8 adNum)
{
    u16 tmp = 0;
    tmp = ADCDualConvertedValue[adNum];
    return tmp;
}
/********************************************************************
* 功能 : 初始化 模数转换 驱动
*
* 输入 : NULL
* 输出 : NULL
*
* 备注: (修改记录内容、时间)
********************************************************************/
void DriveAdc_init(void) 
{
    GPIO_InitTypeDef     tempGPIO_InitStructure;
    ADC_InitTypeDef      ADC_InitStructure;
    //u16   temp = 0;
    //--------- 时钟初始化 --------------------------
    /* Enable ADC3 and GPIOB GPIOC clocks */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOF|RCC_APB2Periph_AFIO, ENABLE);
    //--------- IO端口初始化 ----------------------------
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
    //----------中断初始化 ---------------------------------------
    /* Configure and enable ADC interrupt */
    //  tempNVIC_InitStructure.NVIC_IRQChannel = ADC3_2_IRQChannel;
    //  tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //  tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    //  tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //  NVIC_Init(&tempNVIC_InitStructure);

    //----------------------------------------------------------------------------
    /* ADC3 configuration ------------------------------------------------------*/
    ADC_DeInit(ADC3);  //复位ADC3,将外设 ADC3 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //ADC3和ADC2独立进行转换
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;       //扫描模式关闭
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //连续模式关闭
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//SWSTART触发规则转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;   //            // 1个转换

    ADC_Init(ADC3, &ADC_InitStructure);
    //设置指定ADC的规则组通道，设置它们的转化顺序和采样时间

    /* DISABLE ADC3 EOC interupt */
    ADC_ITConfig(ADC3, ADC_IT_EOC, DISABLE);//不产生中断
    /* Enable ADC3 */
    ADC_Cmd(ADC3, ENABLE);
    Adc_Delay_uS(40);
    //启动校准前,ADC必须处于关电状态(ADON='0')超过至少两个ADC时钟周期.
    //重置指定的ADC的校准寄存器
    ADC_ResetCalibration(ADC3);//复位校准
    //获取ADC重置校准寄存器的状态
    while(ADC_GetResetCalibrationStatus(ADC3));

    //开始指定ADC的校准状态
    ADC_StartCalibration(ADC3);
    //获取指定ADC的校准程序
    while(ADC_GetCalibrationStatus(ADC3));

    //使能或者失能指定的ADC的软件转换启动功能
  ///  ADC_SoftwareStartConvCmd(ADC3, ENABLE);
    
    //temp = GetPowerV();
}

/********************************************************************
* 名称 : DriveAdc_getVar
* 功能 : 获得 模数转换 的值
*
* 输入 : 通道号
* 输出 : 采集到的AD值;
*
* 备注: 
********************************************************************/
u16 Ad_GetValue_two(u8 Channel) 
{

    ADC_RegularChannelConfig(ADC3, Channel, 1, ADC_SampleTime_239Cycles5);// 28.5周期
    ADC_ClearFlag(ADC3, ADC_FLAG_EOC);
    ADC_Cmd(ADC3, ENABLE);
	Adc_Delay_uS(40);
    ADC_SoftwareStartConvCmd(ADC3, ENABLE);         //使能指定的ADC3的软件转换启动功能          
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

