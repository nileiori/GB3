/**
  ******************************************************************************
  * @file    ADC/3ADCs_DMA/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "usb_hcd_int.h"
#include "VDR.h"
#include "Can_Driver.h"
//********************???*****************
//*******************????*****************
u32 SysTickCount = 0;
extern const u8 SystemFaultType[][17];

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;
//????
extern uint32_t AUDIO_SAMPLE_LEN ;
extern uint16_t AUDIO_SAMPLE[] ;
extern u8 REC_TO_G711_FLAG ;

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_3ADCs_DMA
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADC2ConvertedValue;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
    u32 i,j;
    while(1)
    {
        Lcd_DisplayString(1, 0, 0, (unsigned char *)" NMI Handler! ", strlen(" NMI Handler! "));
      
        for(j=0; j<10; j++)
        {

        IWDG_ReloadCounter();
            for(i=0; i<5000000; i++)
            {
      
            }
        }
        NVIC_SystemReset();
    }
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
    u32 i;
    u8  j;
        
  /* Go to infinite loop when Hard Fault exception occurs */
        
  while (1)
  {
      Lcd_DisplayString(1, 0, 0, (unsigned char *)" HardFault Handler! ", strlen(" HardFault Handler! "));
      
      for(j=0; j<10; j++)
      {
      IWDG_ReloadCounter();
        for(i=0; i<5000000; i++)
        {
      
        }
      }
      NVIC_SystemReset();
  }
        
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
    u32 i;
    u8  j;
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
      Lcd_DisplayString(1, 0, 0, (unsigned char *)" MemManage Handler! ", strlen(" MemManage Handler! "));
      for(j=0; j<10; j++)
      {
      IWDG_ReloadCounter();
        for(i=0; i<5000000; i++)
        {
      
        }
      }
      NVIC_SystemReset();
  }
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
    u32     i;
    u8  j;
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
      Lcd_DisplayString(1, 0, 0, (unsigned char *)" BusFault Handler! ", strlen(" BusFault Handler! "));
      for(j=0; j<10; j++)
      {
      IWDG_ReloadCounter();
        for(i=0; i<5000000; i++)
        {
      
        }
      }
      NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    u32 i;
    u8  j;
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
      Lcd_DisplayString(1, 0, 0, (unsigned char *)" UsageFault Handler! ", strlen(" UsageFault Handler! "));
      for(j=0; j<10; j++)
      {

      IWDG_ReloadCounter();
        for(i=0; i<5000000; i++)
        {
      
        }
      }
      NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  static u8  count = 0;
    
  SysTickCount++;
    
  Timer_ISR();
    
    count++;
  if(count >= 10)//0.5秒更新1次时间
  {
        count = 0;
    ReadRtc();//只在这里更新rtc,其他地方不需要再更新了
  }
   
}


/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
    
    while (1)
  {
      
  }
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
//******************秒中断处理*********************
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    //清除中断标志
    RTC_ClearITPendingBit(RTC_IT_SEC);            
  }
}
/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
    
    SleepDeep_RTC_WKUP_IRQ();
}

/**********************************************                                                                                                                                                                                                                                                                                                                      *********************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
    
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)//usb过流中断，实际未使用
    {
            USB_Host.usr_cb->OverCurrentDetected();
            EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{
  while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    SleepDeep_ACC_EXTI_IRQ();
	  if(EXTI_GetITStatus(BMA250E_EXTI_LINE) != RESET) 
    {
        EXTI_ClearITPendingBit(BMA250E_EXTI_LINE);  
        Io_WriteAlarmBit(ALARM_BIT_IMPACT_PRE_ALARM, SET);			
    }
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
    USB_OTG_BSP_TimerIRQ();
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
  VDRPulse_TimIsr();
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
  VDRDocimasy_Tim4Isr();
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
  VDRUsart_Isr();
}
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
  COMM_IRQHandler(COM2);
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
    Usart3_Isr();
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line10))//PB10,脉冲输入脚
  {
    VDRPulse_ExtiIsr();
  }
  if(EXTI_GetITStatus(EXTI_Line14))//PB14,DB9的7脚
  {
    VDRDocimasy_ExtiIsr();
  }
}


/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
    while(1)
    {
    
    }
}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
    
}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
    if(GetUsartStatus(COM4, USART_IT_RXNE) != RESET)
  {
    Gps_RxIsr();
  }  
  if(GetUsartStatus(COM4, USART_IT_TXE) != RESET)
  {
    Gps_TxIsr();
  }
  if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
  {
    USART_ReceiveData(UART4);
  }
}
/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//串口5被通信模块使用，见Modem_USART_IRQHandler
/*******************************************************************************
* Function Name  : UART6_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//串口6未使用
    /**
      * @brief  OTG_FS_IRQHandler
      *          This function handles USB-On-The-Go FS global interrupt request.
      *          requests.
      * @param  None
      * @retval None
      */
#ifdef USE_USB_OTG_FS  
    void OTG_FS_IRQHandler(void)
#else
    void OTG_HS_IRQHandler(void)
#endif
    {
      USBH_OTG_ISR_Handler(&USB_OTG_Core);
    }
    
    
    /**
      * @brief I2S DMA的接收完成中断.
      * @param  None
      * @retval None
      */
    void DMA1_Stream3_IRQHandler(void)
    {
        
 /*      static uint32_t Audio_Play_LEN = 0;
       if(DMA_GetFlagStatus(AUDIO_REC_DMA_STREAM,AUDIO_REC_DMA_FLAG_HT) == SET)
       {
         REC_TO_G711_FLAG = 1;
         DMA_ClearFlag(AUDIO_REC_DMA_STREAM,AUDIO_REC_DMA_FLAG_HT);
       }
       if(DMA_GetFlagStatus(AUDIO_REC_DMA_STREAM,AUDIO_REC_DMA_FLAG_TC) == SET)
       {
         REC_TO_G711_FLAG = 2;
         DMA_ClearFlag(AUDIO_REC_DMA_STREAM,AUDIO_REC_DMA_FLAG_TC);                 //需要手动清除该标志
         DMA_Cmd(AUDIO_REC_DMA_STREAM, DISABLE);
         Audio_Play_LEN += DMA_MAX_SZE;
         if((Audio_Play_LEN+DMA_MAX_SZE) >= AUDIO_SAMPLE_LEN){                      //(Audio_Play_LEN+DMA_MAX_SZE) 怕最后一次越位,这里截取掉不足DMA_MAX_SZE部分
           Audio_Play_LEN = 0; 
         }
         DMA_MemoryTargetConfig(AUDIO_REC_DMA_STREAM,(uint32_t)&AUDIO_SAMPLE[Audio_Play_LEN],DMA_Memory_0);
         DMA_SetCurrDataCounter(AUDIO_REC_DMA_STREAM,DMA_MAX_SZE); 
         DMA_Cmd(AUDIO_REC_DMA_STREAM, ENABLE);   
       }
  */  }
    
    
    
    
    /*******************************************************************************
    * Function Name  : USB_HP_CAN_TX_IRQHandler
    * Description    : This function handles USB High Priority or CAN TX interrupts
    *                  requests.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN1_TX_IRQHandler(void)   //Weite
    {
        /* dxl,2015.9,
        CAN1->TSR |= 0x000F0F0F;//清除中断标志位
        Can_Isr_Tx();
        */
    }
    
    /*******************************************************************************
    * Function Name  : USB_LP_CAN_RX0_IRQHandler
    * Description    : This function handles USB Low Priority or CAN RX0 interrupts
    *                  requests.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN1_RX0_IRQHandler(void)  
    {
       
        CAN1->RF0R |= 0x00000018;
        if(CAN_MessagePending(CAN1, CAN_FIFO0) != 0)
        {    
            //CAN1->TSR |= 0x000F0F0F; dxl,2016.5.21屏蔽，可能可以不需要这句
            Can_1_Rx_Isr();					
				}               
    }
    
    /*******************************************************************************
    * Function Name  : CAN_RX1_IRQHandler
    * Description    : This function handles CAN RX1 interrupt request.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN1_RX1_IRQHandler(void)  //Weite
    {
        CAN1->RF1R |= 0x00000018;//清除中断标志位
        if(CAN_MessagePending(CAN1, CAN_FIFO1) != 0)
        {    
           
        }
    }
    
    /*******************************************************************************
    * Function Name  : CAN_SCE_IRQHandler
    * Description    : This function handles CAN SCE interrupt request.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN1_SCE_IRQHandler(void)  //Weite
    {
        CAN1->IER  &= 0xFFFC7FFF;//清除中断允许位
    }
    
    
    
    
    
    /*******************************************************************************
    * Function Name  : USB_HP_CAN_TX_IRQHandler
    * Description    : This function handles USB High Priority or CAN TX interrupts
    *                  requests.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN2_TX_IRQHandler(void)   //Shigle
    {
        CAN2->TSR |= 0x000F0F0F;//清除中断标志位
        //Can2_Isr_Tx();
    }
    
    /*******************************************************************************
    * Function Name  : USB_LP_CAN_RX0_IRQHandler
    * Description    : This function handles USB Low Priority or CAN RX0 interrupts
    *                  requests.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN2_RX0_IRQHandler(void)  //Shigle
    {
        CAN2->RF0R |= 0x00000018;//清除中断标志位
    
        
    }
    
    /*******************************************************************************
    * Function Name  : CAN_RX2_IRQHandler
    * Description    : This function handles CAN RX1 interrupt request.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN2_RX1_IRQHandler(void)  //Shigle
    {
        CAN2->RF1R |= 0x00000018;//清除中断标志位
            
        if(CAN_MessagePending(CAN2, CAN_FIFO1) != 0)
        {
              
            
        }
            
        //CAN2->IER  &= 0xFFFFFF0F;//清除中断允许位
    }
    
    /*******************************************************************************
    * Function Name  : CAN_SCE_IRQHandler
    * Description    : This function handles CAN SCE interrupt request.
    * Input          : None
    * Output         : None
    * Return         : None
    *******************************************************************************/
    void CAN2_SCE_IRQHandler(void)  //Shigle
    {
        CAN2->IER  &= 0xFFFC7FFF;//清除中断允许位
    }
    
    
    
    
    
    
    
    

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
