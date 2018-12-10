/**
  ******************************************************************************
  * @file    modem_bsp.h
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2013-08-21 
  * @brief   
  ******************************************************************************
  * @attention
  ******************************************************************************
  */ 
#ifndef __MODEM_BSP_H
#define __MODEM_BSP_H

#include "my_typedef.h"
#include "system_stm32f10x.h"
#include "stm32f10x.h"
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#ifdef EYE_MODEM

#else
    typedef unsigned char    BIT;
    typedef unsigned char    BOOL;
#endif

#ifndef __TRUE
 #define __TRUE         1
#endif
#ifndef __FALSE
 #define __FALSE        0
#endif

/*******************************************************************************
*        ģ����Ϣ: �Ϻ���Զ Mϵ��ģ��  G��
*        ����    : 2s
*        �ػ�    : 1s
*        ��ע    : Ŀǰֻ��M12DE��M50֧��TTS
*******************************************************************************/
//#define Modem_Quectel               "Revision: M"// Revision: M35,dxl,2015.5.13,֮ǰģ���ͺź��ڲ�TTS��ʶ����Ҫ������ATI 1��,�����޸ĺ�1�ξͿ�����
#define Modem_Quectel               "Quectel"
#define Modem_QActOnDly              (2)
#define Modem_QActOffDly             (1)

/*******************************************************************************
*        ģ����Ϣ: ���ͨ G510  G��
*        ����    : 2s
*        �ػ�    : 1s
*        ��ע    : ��
*******************************************************************************/
#define Modem_FIBOCOM                 "G510_V0D"
#define Modem_FIBOCOMActOnDly         (2)
#define Modem_FIBOCOMActOffDly        (3)

/*******************************************************************************
*        ģ����Ϣ: ����ͨѶ MC8332  C��
*        ����    : 2s
*        �ػ�    : 2s
*        ��ע    : ��
*******************************************************************************/
//#define Modem_ZTE                    "Model: MC8332",2015.5.13
#define Modem_ZTE                    "Model: MC833"
#define Modem_ZTEActOnDly             (2)
#define Modem_ZTEActOffDly            (2)


/*******************************************************************************
*        ģ������������
*******************************************************************************/
#define  BSP_SPK_TALK                 (0)//ͨ�� 
#define  BSP_SPK_TTS                  (1)//TTS
 
////////////////////////// IO Config Start /////////////////////////////////////
/*******************************************************************************
*
*   Definitions �������豸 -> HB EGS701 һ���
*
*******************************************************************************/
#ifdef EYE_EGS701
#define Modem_BSP_POWER_PIN                 GPIO_Pin_10
#define Modem_BSP_POWER_GPIO_PORT           GPIOC
#define Modem_BSP_POWER_GPIO_CLK            RCC_APB2Periph_GPIOC

#define Modem_BSP_ACTIVE_PIN                GPIO_Pin_4
#define Modem_BSP_ACTIVE_GPIO_PORT          GPIOD
#define Modem_BSP_ACTIVE_GPIO_CLK           RCC_APB2Periph_GPIOD

#define Modem_BSP_DTR_PIN                   GPIO_Pin_5
#define Modem_BSP_DTR_GPIO_PORT             GPIOG
#define Modem_BSP_DTR_GPIO_CLK              RCC_APB2Periph_GPIOG

#define Modem_BSP_RING_PIN                  GPIO_Pin_13
#define Modem_BSP_RING_GPIO_PORT            GPIOG
#define Modem_BSP_RING_GPIO_CLK             RCC_APB2Periph_GPIOG

#define Modem_BSP_MIC_PIN                  GPIO_Pin_5
#define Modem_BSP_MIC_GPIO_PORT            GPIOD
#define Modem_BSP_MIC_GPIO_CLK             RCC_APB2Periph_GPIOD

#define Modem_BSP_SPK_PIN                  GPIO_Pin_7
#define Modem_BSP_SPK_GPIO_PORT            GPIOC
#define Modem_BSP_SPK_GPIO_CLK             RCC_APB2Periph_GPIOC

/*******************************************************************************
*
*   Definitions �������豸 -> EGS702  �����
*
*******************************************************************************/
#elif defined EYE_EGS702
#define Modem_BSP_POWER_PIN                 GPIO_Pin_10
#define Modem_BSP_POWER_GPIO_PORT           GPIOD
#define Modem_BSP_POWER_GPIO_CLK            RCC_APB2Periph_GPIOD

#define Modem_BSP_ACTIVE_PIN                GPIO_Pin_14
#define Modem_BSP_ACTIVE_GPIO_PORT          GPIOE
#define Modem_BSP_ACTIVE_GPIO_CLK           RCC_APB2Periph_GPIOE

#define Modem_BSP_DTR_PIN                   GPIO_Pin_5
#define Modem_BSP_DTR_GPIO_PORT             GPIOG
#define Modem_BSP_DTR_GPIO_CLK              RCC_APB2Periph_GPIOG

#define Modem_BSP_RING_PIN                  GPIO_Pin_13
#define Modem_BSP_RING_GPIO_PORT            GPIOG
#define Modem_BSP_RING_GPIO_CLK             RCC_APB2Periph_GPIOG

#define Modem_BSP_MIC_PIN                  GPIO_Pin_12
#define Modem_BSP_MIC_GPIO_PORT            GPIOF
#define Modem_BSP_MIC_GPIO_CLK             RCC_APB2Periph_GPIOF

#define Modem_BSP_SPK_PIN                  GPIO_Pin_15
#define Modem_BSP_SPK_GPIO_PORT            GPIOF
#define Modem_BSP_SPK_GPIO_CLK             RCC_APB2Periph_GPIOF

/*******************************************************************************
*
*   Definitions �������豸 -> ȱʡ�豸,��ʱĬ�� -> EGS701 һ���
*
*******************************************************************************/
#else
#define Modem_BSP_POWER_PIN                 GPIO_Pin_0
#define Modem_BSP_POWER_GPIO_PORT           GPIOA
#define Modem_BSP_POWER_GPIO_CLK            RCC_APB2Periph_GPIOA

#define Modem_BSP_ACTIVE_PIN                GPIO_Pin_4
#define Modem_BSP_ACTIVE_GPIO_PORT          GPIOD
#define Modem_BSP_ACTIVE_GPIO_CLK           RCC_AHB1Periph_GPIOD

#define Modem_BSP_DTR_PIN                   GPIO_Pin_5
#define Modem_BSP_DTR_GPIO_PORT             GPIOG
#define Modem_BSP_DTR_GPIO_CLK              RCC_AHB1Periph_GPIOG

#define Modem_BSP_RING_PIN                  GPIO_Pin_13
#define Modem_BSP_RING_GPIO_PORT            GPIOG
#define Modem_BSP_RING_GPIO_CLK             RCC_AHB1Periph_GPIOG

#define Modem_BSP_MIC_PIN                  GPIO_Pin_5
#define Modem_BSP_MIC_GPIO_PORT            GPIOD
#define Modem_BSP_MIC_GPIO_CLK             RCC_AHB1Periph_GPIOD

#define Modem_BSP_SPK_PIN                  GPIO_Pin_7
#define Modem_BSP_SPK_GPIO_PORT            GPIOC
#define Modem_BSP_SPK_GPIO_CLK             RCC_AHB1Periph_GPIOC
#endif

////////////////////////// IO Config End ///////////////////////////////////////

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void  Modem_Bsp_Init (void);
void  Modem_Bsp_SetPower(uint8_t state);
void  Modem_Bsp_SetActive(uint8_t state);
void  Modem_Bsp_SetDTR(uint8_t state);
void  Modem_Bsp_SpkOn(u8 type);
void  Modem_Bsp_SpkOff(u8 type);
void  Modem_Bsp_MicOn(void);
void  Modem_Bsp_MicOff(void);

u8 Modem_Bsp_ReadRing(void);

#endif







