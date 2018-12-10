#ifndef	__AD_H
#define	__AD_H

//*************�ļ�����***************
#include "stm32f10x.h"
//*************�궨��****************
//ADC���ú�DMA����
#define EVAL_ADC                           ADC3
#define ADC_CLOCK                          RCC_APB2Periph_ADC3
#define ADC_DR_ADDRESS                     (&(ADC3->DR))           //ADC���ݵ�ַ

#define ADC_DMA_CLOCK                      RCC_AHBPeriph_DMA2
//#define ADC_DMA_CHAN                       DMA1_Channel2
#define ADC_DMA_STREAM                     DMA2_Channel5

//ADC Ӳ����������
#define ADC_EXTERN1_PIN                     GPIO_Pin_1
#define ADC_EXTERN1_GPIO_PORT               GPIOC
#define ADC_EXTERN1_GPIO_CLK                RCC_APB2Periph_GPIOC
#define ADC_EXTERN1_CHAN                    ADC_Channel_11

#define ADC_EXTERN2_PIN                     GPIO_Pin_0
#define ADC_EXTERN2_GPIO_PORT               GPIOC
#define ADC_EXTERN2_GPIO_CLK                RCC_APB2Periph_GPIOC
#define ADC_EXTERN2_CHAN                    ADC_Channel_10

#define ADC_MAIN_POWER_PIN                  GPIO_Pin_10
#define ADC_MAIN_POWER_GPIO_PORT            GPIOF
#define ADC_MAIN_POWER_GPIO_CLK             RCC_APB2Periph_GPIOF
#define ADC_MAIN_POWER_CHAN                 ADC_Channel_8

#define ADC_VBATIN_PIN                       GPIO_Pin_8
#define ADC_VBATIN_GPIO_PORT                 GPIOF
#define ADC_VBATIN_GPIO_CLK                  RCC_APB2Periph_GPIOF
#define ADC_VBATIN_CHAN                      ADC_Channel_6

#define ADC_IO_OUT_PIN                       GPIO_Pin_9
#define ADC_IO_OUT_GPIO_PORT                 GPIOF
#define ADC_IO_OUT_GPIO_CLK                  RCC_APB2Periph_GPIOF
#define ADC_IO_OUT_CHAN                      ADC_Channel_7

#define ADC_MIC_PIN                          GPIO_Pin_7
#define ADC_MIC_GPIO_PORT                    GPIOF
#define ADC_MIC_GPIO_CLK                     RCC_APB2Periph_GPIOF
#define ADC_MIC_CHAN                         ADC_Channel_5

#define ADC_KEY_PIN                          GPIO_Pin_6
#define ADC_KEY_GPIO_PORT                    GPIOF
#define ADC_KEY_GPIO_CLK                     RCC_APB2Periph_GPIOF
#define ADC_KEY_CHAN                         ADC_Channel_4

#define ADC_PRINTF_PIN                        GPIO_Pin_5
#define ADC_PRINTF_GPIO_PORT                  GPIOF
#define ADC_PRINTF_GPIO_CLK                   RCC_APB2Periph_GPIOF
#define ADC_PRINTF_CHAN                       ADC_Channel_15

#define ADC_PRINTF_THE_PIN                     GPIO_Pin_4
#define ADC_PRINTF_THE_GPIO_PORT               GPIOF
#define ADC_PRINTF_THE_GPIO_CLK                RCC_APB2Periph_GPIOF
#define ADC_PRINTF_THE_CHAN                    ADC_Channel_14


//ADת��ͨ����
#define GPS_CHANNEL      8    //GPS���߼��ͨ��
#define BAT_CHANNEL      11   //��ؼ��ͨ����
#define LOAD_CHANNEL     12   //���ؼ��ͨ����
#define POWER_CHANNEL    6   //��Դ���ͨ����
#define OIL_CHANNEL      14   //�������ͨ����
#define KEY_CHANNEL      4   //������ͨ����

//************�ṹ������******************
//�ɼ����ֵ�ѹ����
typedef enum {
  ADC_EXTERN1=0,  //�ⲿ��ѹ1,PC1,ADC3_IN11
  ADC_EXTERN2,    //�ⲿ��ѹ2,PC0,ADC3_IN10
  ADC_MAIN_POWER, //����ѹ,PF10,ADC3_IN8
  ADC_VBATIN,     //����ѹ�ɼ�,PF8,ADC3_IN6
  ADC_IO_OUT,     //IO�����·���,PF9,ADC3_IN7
  ADC_MIC,        //��˷���,PF7,ADC3_IN5
  ADC_KEY,        //���̼��,PF6,ADC3_IN4,
  ADC_PRINTF,     //��ӡ�����,PF5,ADC3_IN15
  ADC_PRINTF_THE, //��ӡ���¶ȼ��,PF4,ADC3_IN14
  ADC_CHAN_MAX,   //ADCͨ������
}ADC_CHAN_TYPE;

//*************��������**************
/*********************************************************************
//��������	:Ad_Init
//����		:AD��ʼ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Ad_Init(void);
/*********************************************************************
//��������	:ADC3_Config
//����		:AD��ʼ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
 void Ad_Config(void);
 /*********************************************************************
//��������	:Ad_Dma_Config
//����		:��AD������ص�DMA����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Ad_Dma_Config(void);
/*********************************************************************
//��������	:Ad_Gpio_Config
//����		:AD��IO��ʼ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Ad_Gpio_Config(u8 IoNum);
/*********************************************************************
//��������	:Ad_GetValue
//����		:����AD���,��õ�ǰADֵ
//����		:adNum AD���,�ο�ADC_CHAN_TYPEö��.
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:��ǰ�ɼ�����ADֵ.
//��ע		:
*********************************************************************/
u16 Ad_GetValue(u8 adNum);
//*******************************************************
//AD����
u16 Ad_GetValue(u8 Channel) ;//��� ģ��ת����ֵ

/********************************************************************
* ���� : ��ʼ�� ģ��ת�� ����
*
* ���� : NULL
* ��� : NULL
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void DriveAdc_init(void) ;

#endif
