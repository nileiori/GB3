
#include "my_typedef.h"

#ifndef	__SYSTEM_INIT_H
#define	__SYSTEM_INIT_H

//*************�ļ�����***************

//*************�궨��****************
#define USER_PLL_M	8//PLL����Ƶ�ʷ�Ƶ,��Ƶ��Ƶ�������1Mhz-2Mhz
#define USER_PLL_N   384//PLLCLK = 1Mhz*PLL_N,����Ϊ192-432֮��
#define USER_PLL_P	4//SYSCLK= PLLCLK/PLL_P,Ϊ96Mhz
#define USER_PLL_Q   8//USBCLK=48Mhz


#define RCC_USB_SW    RCC_APB2Periph_GPIOA      /* GPIO��A��˿�ʱ�� */
#define GPIO_USB_SW   GPIOA                       /* ѡ��A��˿�*/ 
#define PIN_USB_SW    GPIO_Pin_8  
#define USB_SW_1()    GPIO_SetBits(GPIO_USB_SW,PIN_USB_SW)	 /* IO = 1 */
#define USB_SW_0()    GPIO_ResetBits(GPIO_USB_SW,PIN_USB_SW)	 /* IO = 0 */
#define USB_SW_READ() GPIO_ReadInputDataBit(GPIO_USB_SW,PIN_USB_SW)	/* ��IO����״̬ */

//************�ṹ������******************
//*************��������**************
extern u8      IC_Reader_flag;
//*************��������**************
/*********************************************************************
//��������	:PowerOnUpdata
//����		:�ϵ���±���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:���µı����У�IO��GPS��λ�û㱨������ͷ
*********************************************************************/
void PowerOnUpdata(void);
/*********************************************************************
//��������	:PowerOnPeripheralSelfCheck
//����		:�ϵ������Լ�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:�Լ�������У�����ͷ
*********************************************************************/
void PowerOnPeripheralSelfCheck(void);
/*********************************************************************
//��������	:MemoryChipSelfCheck
//����		:�ϵ�洢оƬ�Լ�
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:�Լ�Ĵ洢оƬ�У�FRAM��FLASH��EEPROM
*********************************************************************/
void MemoryChipSelfCheck(void);
/*********************************************************************
//��������	:PeripheralInit
//����		:�����ʼ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:��ʼ���������У�GPS��GPRS��FRAM��FLASH��EEPROM������ͷ
*********************************************************************/
void PeripheralInit(void);
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void);
/*******************************************************************************
* Function Name  : IDWG_Configuration
* Description    : Configures IDWG clock source and prescaler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IDWG_Configuration(void);
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void);
/*******************************************************************************
* Function Name  : PWR_PVDInit
* Description    : ��ԴPVD��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWR_PVDInit(void);
/*******************************************************************************
* Function Name  : USB_SW_Init
* Description    : USB�������ų�ʼ��������������Ӳ����λLCD��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void   USB_SW_Init(void);
#endif
