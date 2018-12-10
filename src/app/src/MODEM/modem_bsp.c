/**
  ******************************************************************************
  * @file    modem_bsp.c 
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2013-08-22
  * @brief   
  ******************************************************************************
  */
/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#ifdef EYE_MODEM
#include "include.h"
#else
//#include  <app_cfg.h>
#include  <includes.h>
#include "modem.h"
#endif

/*
********************************************************************************
*                            DEFINES
********************************************************************************
*/


/*
********************************************************************************
*                         LOCAL DATA TYPES
********************************************************************************
*/



/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/


/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/


/*
********************************************************************************
*                           FUNCTIONS EXTERN
********************************************************************************
*/


/*
********************************************************************************
*                              FUNCTIONS
********************************************************************************
*/

/*
  * @brief  ModemӲ��������ʼ��,����IO��Power��Active��DTR��RING.
  * @param  None
  * @retval None
  */
void  Modem_Bsp_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    u8 i;
    
    /*!< Enable GPIO clocks */
    RCC_APB2PeriphClockCmd(Modem_BSP_POWER_GPIO_CLK | Modem_BSP_ACTIVE_GPIO_CLK 
                           | Modem_BSP_DTR_GPIO_CLK | Modem_BSP_RING_GPIO_CLK
                           | Modem_BSP_MIC_GPIO_CLK | Modem_BSP_SPK_GPIO_CLK, ENABLE);
    
    /*!< Configure Modem  Power��ACTIVE��DTR��pin in output pushpull mode ************/  
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
   // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   // GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    
    GPIO_InitStructure.GPIO_Pin = Modem_BSP_POWER_PIN;////Power
    GPIO_Init(Modem_BSP_POWER_GPIO_PORT , &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin = Modem_BSP_ACTIVE_PIN;////ACTIVE
    GPIO_Init(Modem_BSP_ACTIVE_GPIO_PORT , &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin = Modem_BSP_DTR_PIN;////DTR
    GPIO_Init(Modem_BSP_DTR_GPIO_PORT , &GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Pin = Modem_BSP_MIC_PIN;////MIC
    GPIO_Init(Modem_BSP_MIC_GPIO_PORT , &GPIO_InitStructure);      

    GPIO_InitStructure.GPIO_Pin = Modem_BSP_SPK_PIN;////SPEAK
    GPIO_Init(Modem_BSP_SPK_GPIO_PORT , &GPIO_InitStructure);   

    /*!< Configure Modem  Ring pin in intput pushpull mode ********************/
    GPIO_InitStructure.GPIO_Pin   = Modem_BSP_RING_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
   // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(Modem_BSP_RING_GPIO_PORT, &GPIO_InitStructure);


    for(i=0;i<MODEM_SOC_NUM;i++)
    {
        ModLib_NodInit(&ModSocNode[i],
                       &ModSocNdNum[i*MODEM_SOC_ND_SIZE],MODEM_SOC_ND_SIZE,
                       &ModSocBuf[i*MODEM_SOC_TX_SIZE],MODEM_SOC_TX_SIZE);
    }

}


/**
  * @brief  ����modem�ĵ�Դ,
        ע�⣺����ʵ�ʼ�⣺
                ->  M35 �ŵ�ʱ��ԼΪ25�룬Ϊ�����������ϵ�30s;
  * @param  state : 0Ϊ�رա������
  * @retval None
  */

void Modem_Bsp_SetPower(u8 state)
{
    if (0 == state)
    { 
        /* ��modem��Դ */
        GPIO_ResetBits(Modem_BSP_POWER_GPIO_PORT, Modem_BSP_POWER_PIN);
    }
    else
    {
        /* �ر�modem��Դ */
        GPIO_SetBits(Modem_BSP_POWER_GPIO_PORT, Modem_BSP_POWER_PIN);
    }
}                           


/**
  * @brief  ����modem�������ŵ�ƽ�ź�
  * @param  state : 0Ϊ���͡���������
  * @retval None
  */

void Modem_Bsp_SetActive(u8 state)
{
    if (0 == state)
    { 
        /* ���� �������� ��ƽ*/
        GPIO_SetBits(Modem_BSP_ACTIVE_GPIO_PORT, Modem_BSP_ACTIVE_PIN);
    }
    else
    {
         /* ���� �������� ��ƽ*/
        GPIO_ResetBits(Modem_BSP_ACTIVE_GPIO_PORT, Modem_BSP_ACTIVE_PIN);
    }
}

/**
  * @brief  ����modem DTR���ŵ�ƽ�ź�
  * @param  state : 0Ϊ���͡���������
  * @retval None
  */

void Modem_Bsp_SetDTR(u8 state)
{
    if (0 == state)
    { 
        /* ���� DTR ��ƽ*/
        GPIO_ResetBits(Modem_BSP_DTR_GPIO_PORT, Modem_BSP_DTR_PIN);
    }
    else
    {
        /* ���� DTR ��ƽ */
        GPIO_SetBits(Modem_BSP_DTR_GPIO_PORT, Modem_BSP_DTR_PIN);
    }
}
/**
  * @brief  ��ȡmodem RING���ŵ�ƽ�ź�
  * @param  None
  * @retval ���ص�ƽ״̬
  */
u8 Modem_Bsp_ReadRing(void)
{
    u8 bitstatus = 0;
    
    bitstatus = GPIO_ReadInputDataBit(Modem_BSP_RING_GPIO_PORT, Modem_BSP_RING_PIN);

    return (bitstatus);
}

/**
  * @brief  ����˷�
  * @param  None
  * @retval None
  */

void  Modem_Bsp_MicOn(void)
{
#ifdef EYE_MODEM
    Io_WriteMicConrtolBit(MIC_BIT_TALK,SET);
#else
    GPIO_SetBits(Modem_BSP_MIC_GPIO_PORT, Modem_BSP_MIC_PIN);
#endif 

}

/**
  * @brief  �ر���˷�
  * @param  None
  * @retval None
  */

void  Modem_Bsp_MicOff(void)
{
#ifdef EYE_MODEM
    Io_WriteMicConrtolBit(MIC_BIT_TALK,RESET);
#else
    GPIO_ResetBits(Modem_BSP_MIC_GPIO_PORT, Modem_BSP_MIC_PIN);
#endif 
}
/**
  * @brief  ��������
  * @param  BSP_SPK_TALK
  *         BSP_SPK_TTS
  *
  * @retval None
  */
void  Modem_Bsp_SpkOn(u8 type)
{
#ifdef EYE_MODEM
    Io_WriteSpeakerConrtolBit((SPEAKER_BIT)type,SET);
#else
    GPIO_SetBits(Modem_BSP_SPK_GPIO_PORT, Modem_BSP_SPK_PIN);
#endif 
}

/**
  * @brief  �ر�������
  * @param  type-> 0: ͨ��;
  *                1: TTS;
  * @retval None
  */
void  Modem_Bsp_SpkOff(u8 type)
{
#ifdef EYE_MODEM
    Io_WriteSpeakerConrtolBit((SPEAKER_BIT)type,RESET);
#else
    GPIO_ResetBits(Modem_BSP_SPK_GPIO_PORT, Modem_BSP_SPK_PIN);
#endif 
}
                           

