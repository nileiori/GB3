/*******************************************************************************
 * File Name:           SleepDeep.c 
 * Function Describe:   �������
 * Relate Module:       
 * Writer:              Joneming
 * Date:                2013-07-11
 * ReWriter:            
 * Date:
 ***************************************************************************
 *******************************************************************************/
#include "include.h"

extern u8	GBTestFlag;//0:������������ģʽ;1:������ģʽ���ñ�־ͨ���˵���ѡ��
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
////////////////////////////////////
#define SLEEPDEEP_VOLT_12V_MIN  110 //(0.1V)
#define SLEEPDEEP_VOLT_24V_MIN  220 //(0.1V)
#define SLEEPDEEP_VOLT_12V_MAX  146 //(0.1V)
#define SLEEPDEEP_VOLT_24V_MAX  292 //(0.1V)
#define WAKEUP_VOLT_12V  126 //(0.1V)
#define WAKEUP_VOLT_24V  252 //(0.1V)
///////////////////////////////////////
#define SLEEP_DEEP_MIN_STOP_TIME  240 //��)
//#define SLEEP_DEEP_MIN_STOP_TIME  10 //��)
//////////////////////////
#define SLEEP_DEEP_ACC_GPIO_CLK         RCC_APB2Periph_GPIOE
#define SLEEP_DEEP_ACC_PORT             GPIOE
#define SLEEP_DEEP_ACC_PIN              GPIO_Pin_7
#define SLEEP_DEEP_ACC_EXTI_GPIO        GPIO_PortSourceGPIOE
#define SLEEP_DEEP_ACC_GPIO_PINSOURCE   GPIO_PinSource7//ע�⣬�ⲿ�ж�����pin��һ��
#define SLEEP_DEEP_ACC_EXTI_LINE        EXTI_Line7//�ⲿ�ж�����pin��һ��
#define SLEEP_DEEP_ACC_EXTI_IRQn        EXTI9_5_IRQn//�ⲿ�ж�����pin��һ��
/////////////////////
/////////////////////////
enum 
{
    EXTI_TYPE_EMPTY,//   
    EXTI_TYPE_ACC,  // 
    EXTI_TYPE_RTC,  //
    EXTI_TYPE_OTHER, // 
    EXTI_TYPE_BREAK,  //
    EXTI_TYPE_STOP,  // 
    EXTI_TYPE_MAX
}E_EXTITYPE;
////////////////////////
enum 
{
    SLEEP_TIMER_TASK,                        //   
    SLEEP_TIMER_TEST,                       //
    SLEEP_TIMER_SEND,                       //    
    SLEEP_TIMERS_MAX
}STDSLEEPTIME;

static LZM_TIMER s_stDSleepTimer[SLEEP_TIMERS_MAX];

typedef struct
{
    unsigned long StopTime;     //����ͣ��ʱ��
    unsigned long AccOffTimeCnt;   //Acc�ص�ʱ��
    unsigned short recoverVolt; //�ָ���ѹֵ
    unsigned char stopFlag;     //ͣ����־
    unsigned char clearRestartFlag;//������������־
    unsigned char CheckCnt;     //��������
    unsigned char ExtiType;     //�жϱ�־
    unsigned char VoltType;     //��ѹ����
}STSLEEP_ATTRIB; 
static STSLEEP_ATTRIB s_stSleepDAttrib;
//////////////////////////////////////////////////////
/*******************************************************************************
**  ��������  : SleepDeep_ClearDeepSleep
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : 
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
#define SCB_SCR_SLEEPDEEP_Pos               2                                             /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_Msk              (1ul << SCB_SCR_SLEEPDEEP_Pos)                 /*!< SCB SCR: SLEEPDEEP Mask */


void SleepDeep_ResetSCRSleepDeepBit(void)
{
    // Reset SLEEPDEEP bit of Cortex System Control Register //
   SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);  
}
/*******************************************************************************
**  ��������  : SleepDeep_SetAccExtiType
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : 
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_SetAccExtiType(void)
{
    s_stSleepDAttrib.ExtiType = EXTI_TYPE_ACC;
}
/*******************************************************************************
**  ��������  : SleepDeep_SetAccExtiType
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_SetRtcExtiType(void)
{
    s_stSleepDAttrib.ExtiType = EXTI_TYPE_RTC;
}
/*******************************************************************************
**  ��������  : SleepDeep_SetExtiTypeBreak
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_SetExtiTypeBreak(void)
{
    s_stSleepDAttrib.ExtiType = EXTI_TYPE_BREAK;
}
/*******************************************************************************
**  ��������  : SleepDeep_PortCtrlAccExtiEnable(void)
**  ��������  : ʹ��ACC1�ߵ�ƽ�ϵ����ж�
**  ��    ��  : �� 
**  ��    ��  : ��
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_PortCtrlAccExtiEnable(void)
{
    EXTI_InitTypeDef    EXTI_InitStructure;
    // Connect EXTI Line to PE7 pin //
//    SYSCFG_EXTILineConfig(SLEEP_DEEP_ACC_EXTI_GPIO, SLEEP_DEEP_ACC_GPIO_PINSOURCE);//�趨Ϊ�ⲿ�ж�����

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource7);

    EXTI_ClearITPendingBit(SLEEP_DEEP_ACC_EXTI_LINE);                 //����ⲿ�жϱ�־    
    // Configure EXTI Line //
    EXTI_InitStructure.EXTI_Line = SLEEP_DEEP_ACC_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���ģʽ
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}
/*******************************************************************************
**  ��������  : SleepDeep_PortCtrlAccExtiInit
**  ��������  : ��ʼ���ߵ�ƽ���롢���Ǳ��������𿪹������Ϊ�ն������
**  ��    ��  : �� 
**  ��    ��  : 
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_PortCtrlAccExtiInit(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    // Enable GPIOE clock //
    RCC_APB2PeriphClockCmd(SLEEP_DEEP_ACC_GPIO_CLK, ENABLE);

    // Enable SYSCFG clock //
  //  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Configure PE7 pin as input floating //
    GPIO_InitStructure.GPIO_Pin = SLEEP_DEEP_ACC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // Enable SYSCFG clock //
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // Enable and set EXTI7 Interrupt to the lowest priority //
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    NVIC_InitStructure.NVIC_IRQChannel = SLEEP_DEEP_ACC_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //////////////////////////
    SleepDeep_PortCtrlAccExtiEnable();
}
/*******************************************************************************
**  ��������  : SleepDeep_PortCtrlAccExtiDisable
**  ��������  : ��ֹACC1�ߵ�ƽ����жϷ���
**  ��    ��  : �� 
**  ��    ��  : ��
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_PortCtrlAccExtiDisable(void)
{
    EXTI_InitTypeDef    EXTI_InitStructure;
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, SLEEP_DEEP_ACC_GPIO_PINSOURCE);

    //SYSCFG_EXTILineConfig(SLEEP_DEEP_ACC_EXTI_GPIO, SLEEP_DEEP_ACC_GPIO_PINSOURCE); //�趨Pe3Ϊ�ⲿ�ж�����
    ///////////////////////////
    EXTI_ClearITPendingBit(SLEEP_DEEP_ACC_EXTI_LINE);                 //����ⲿ�жϱ�־
    ////////////////
    EXTI_InitStructure.EXTI_Line = SLEEP_DEEP_ACC_EXTI_LINE ;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //�ⲿ�ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//EXTI_Trigger_Falling;         //�����ش���ģʽ
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);  
}
/*******************************************************************************
**  ��������  : SleepDeep_RTCWakeUpDisable
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : ��
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_RTCWakeUpDisable(void)
{
  //  RTC_WakeUpCmd(DISABLE);
      RTC_ITConfig(RTC_IT_ALR,DISABLE);
    
}
/*******************************************************************************
**  ��������  : SleepDeep_ConfigRTCWakeUp
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : ��
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_ConfigRTCWakeUp(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    // Enable the RTC Interrupt //
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Exti configuration ////////    
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    RCC_RTCCLKCmd(ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
    
    
     RTC_SetAlarm(RTC_GetCounter() + 2); 
    // Enable Wakeup Counter //
     RTC_ITConfig(RTC_IT_ALR,ENABLE);
     RTC_WaitForLastTask();
     RCC_ClearFlag();
}
/********************************************************************
* ���� : SleepDeep_ResetWatchdog
* ���� : ���³�ʼ�����Ź���Ԥ��Ƶ�Ĵ��������ؼĴ�����ֵ
*
* ���� : ��
* ��� : ��
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
*        1.���³�ʼ�����Ź���Ԥ��Ƶ�Ĵ��������ؼĴ�����ֵ��ʹ��STOP MODEģʽ�¿��Ź���ι��ʱ��ӳ���
********************************************************************/
void SleepDeep_ResetWatchdog(void)
{
    #ifdef WATCHDOG_OPEN
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//�����޸� Ԥ��Ƶ�Ĵ��� �� ��װ�ؼĴ���;
    IWDG_SetPrescaler(IWDG_Prescaler_256);;//���� "Ԥ��Ƶϵ��" Ϊ 256;//IWDG counter clock: 40KHz(LSI) / 256 = 0.15625 KHz
    IWDG_SetReload(4000);//�������Ź� ��ʱʱ�� T= 6.4ms *4000 = 25600ms
    IWDG_ReloadCounter();//�忴�Ź�
    IWDG_Enable();//�� �������Ź�  
    #endif
}
/*******************************************************************************
**  ��������  : SleepDeep_ResetIoInit
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : �� 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_ResetIoInit(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);

    //--------- IO�˿ڳ�ʼ�� ----------------------------
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 
   // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    GPIO_Init(GPIOF, &GPIO_InitStructure); 
    GPIO_Init(GPIOG, &GPIO_InitStructure); 
    //GPIO_Init(GPIOH, &GPIO_InitStructure); 
  //  GPIO_Init(GPIOI, &GPIO_InitStructure);    
}

/*******************************************************************************
**  ��������  : SleepDeep_GetAccPortInValue
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : �� 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
unsigned char SleepDeep_GetAccPortInValue(void)
{
    return GPIO_ReadInputDataBit(SLEEP_DEEP_ACC_PORT,SLEEP_DEEP_ACC_PIN);
}
/*******************************************************************************
**  ��������  : SleepDeep_CloseAllUserPower
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : ��
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_CloseAllUserPower(void)
{
    GpioOutOff(IC_P);
//    TTSDerv_CtrlTTSPower(0);
	if(0 == GBTestFlag)
	{
    GPS_POWER_OFF();      //�ر� GPS��Դ
	}
    GSM_PWR_OFF();      //GPRSģ��ϵ�.
    CAMERA_POWER_OFF();
    //Comm485PowerOff(POWER_STATUS_ALL);
    SD_POWER_OFF();
    NaviLcd_AccStatusOff();//    
    LCD_LIGHT_OFF();
}
/*******************************************************************************
**  ��������  : SleepDeep_OpenAllUserPower
**  ��������  : 
**  ��    ��  : �� 
**  ��    ��  : ��
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/  
void SleepDeep_OpenAllUserPower(void)
{
    GPS_POWER_ON();      //�ر� GPS��Դ
    GSM_PWR_ON();      //GPRSģ��ϵ�.
    CAMERA_POWER_ON();
    //Comm485PowerOn(POWER_STATUS_ALL);
    SD_POWER_ON();
    NaviLcd_AccStatusOn();//
}
/*************************************************************
** ��������: SleepDeep_GetCurPowerValue
** ��������: ȡ�õ�ǰ����ѹֵ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: ��ǰ����ѹֵ(0.1V)
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short SleepDeep_GetCurPowerValue(void)
{
    unsigned long AdSum = 0;//ADת��ֵ�ۼƺ�
    unsigned short AdMin = 0xffff;//ADת��ֵ��Сֵ
    unsigned short AdMax = 0;//ADת��ֵ���ֵ    
    unsigned short Ad;
    unsigned char Count = 0;//ADת������
    
    for(Count = 0; Count<10; Count++)
    {
        Ad = Ad_GetValue(ADC_MAIN_POWER); 
        if(AdMin > Ad)
        {
            AdMin = Ad;
        }
        if(AdMax < Ad)
        {
            AdMax = Ad;
        }
        AdSum += Ad;
    }
    if(Count >= 10)
    {
        //����ƽ��ֵ
        AdSum -= AdMin;
        AdSum -= AdMax;
        AdSum = AdSum >> 3;
        //ת����0.1VΪ��λ��ֵ
        AdSum = AdSum*33*9/0xfff;        
    }
    return AdSum;
}
/*******************************************************************************
**  ��������  : SleepDeep_DelayMs
**  ��������  : 
**  ��    ��  : ��
**  ��    ��  : 
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_DelayMs(unsigned char num)
{
    unsigned char i;
    unsigned short j;
    for(i= num; i>0; i--)
        for(j=1000; j>0; j--);
}
/*******************************************************************************
**  ��������  : SYSCLKConfig_STOP
**  ��������  : �˳�ֹͣģʽ����������ʱ��
**  ��    ��  : ��
**  ��    ��  : 
 
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
    ErrorStatus HSEStartUpStatus;
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {

#ifdef STM32F10X_CL
    /* Enable PLL2 */ 
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {
    }

#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}
/*******************************************************************************
**  ��������  : SleepDeep_StopMode
**  ��������  : �ж��Ƿ��˳�STOP MODE
**  ��    ��  : ��
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_StopMode(void)
{
    unsigned char ucAccCnt;
    unsigned char ucCount,ucBackSleepCnt;
    ucAccCnt = 0;           //ACC��Ч��ʱ 
    ucCount = 0;
    ucBackSleepCnt = 0;     //����˯��״̬����
    /////////////////////////////////
    s_stSleepDAttrib.CheckCnt = 0;
    //////////////////////    
    //Request to enter STOP mode with regulator in Low Power //

    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    //////////////////////////////
          
    SYSCLKConfig_STOP();     

   while(1)
    { 

        
        if(EXTI_TYPE_ACC == s_stSleepDAttrib.ExtiType)
        {
            for(ucCount =0; ucCount< 100 ; ucCount++)
            {
                IWDG_ReloadCounter();   //�� �������Ź�            
                if(SleepDeep_GetAccPortInValue())                  //���ACC ON��Ч
                {
                    ucAccCnt ++;           //ACC ON������Ч����
                    ucBackSleepCnt = 0;     //        
                }
                else
                {
                    ucAccCnt = 0;      
                    ucBackSleepCnt ++;  
                }
                ////////////////
                if(ucBackSleepCnt >= 50)    //���������㣬��������ģʽ��
                {
                    ucCount = 0;
                    ucAccCnt = 0;              //ACC ON������Ч����
                    ucBackSleepCnt = 0;                    
                    s_stSleepDAttrib.ExtiType = EXTI_TYPE_OTHER;//
                    break;
                }
                if((ucAccCnt >= 4)&&SleepDeep_GetAccPortInValue())//���㿪�Ǵ���30S����ACC����2S���˳�����
                {
                    ucCount = 0;
                    ucAccCnt = 0;               //ACC ON������Ч���� 
                    ucBackSleepCnt = 0;                    
                    SleepDeep_PortCtrlAccExtiDisable();
                    SleepDeep_RTCWakeUpDisable();
                    s_stSleepDAttrib.ExtiType = EXTI_TYPE_BREAK;//�˳�����״̬
                    return;                      //�ⲿ�жϵ�������˳�STOP MODE
                }
            }                             
        }
        else
        if(EXTI_TYPE_RTC == s_stSleepDAttrib.ExtiType)//
        {
            IWDG_ReloadCounter();   //��������Ź�
            RTC_SetAlarm(RTC_GetCounter() + 1);  
            s_stSleepDAttrib.ExtiType = EXTI_TYPE_OTHER;// 
            s_stSleepDAttrib.CheckCnt++;
            if(s_stSleepDAttrib.CheckCnt>3)
            {
                
                s_stSleepDAttrib.CheckCnt = 0;
                SleepDeep_DelayMs(1);
                IWDG_ReloadCounter();   //��������Ź�
                if(SleepDeep_GetCurPowerValue()>s_stSleepDAttrib.recoverVolt+2)
                {
                    s_stSleepDAttrib.ExtiType = EXTI_TYPE_BREAK;// 
                }
            }                       
        
				}
        else 
        if(EXTI_TYPE_BREAK == s_stSleepDAttrib.ExtiType)
        {
            return;
        }
        else
        if(EXTI_TYPE_OTHER == s_stSleepDAttrib.ExtiType)//
        { 
            s_stSleepDAttrib.ExtiType = EXTI_TYPE_STOP;
            
            //RTC_SetAlarm(RTC_GetCounter() + 2);  
   		    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
            SYSCLKConfig_STOP();
        }
    } 
}

/*******************************************************************************
**  ��������  : fUsartIntoSleep
**  ��������  : �ж��Ƿ��˳�STOP MODE
**  ��    ��  : ��
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_UsartIntoSleep(void)
{
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);    
}

/*******************************************************************************
**  ��������  : SleepDeep_EnterStopMode
**  ��������  : ��������ģʽ
**  ��    ��  : ��
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_EnterStopMode(void)
{
    s_stSleepDAttrib.AccOffTimeCnt =0;
    s_stSleepDAttrib.ExtiType =EXTI_TYPE_EMPTY;    
   // SleepDeep_ResetSCRSleepDeepBit();
    ///////////////////////
    SleepDeep_CloseAllUserPower();
    /////////////////
    SleepDeep_ResetWatchdog();  //���³�ʼ�����Ź���Ԥ��Ƶ�Ĵ��������ؼĴ�����ֵ
    ///////////////////////
    SleepDeep_ResetIoInit();
    //////////////
    SleepDeep_UsartIntoSleep();
    ////////////////
    ///////////////////////////////////////
    //PWR_FlashPowerDownCmd(DISABLE);

    IWDG_ReloadCounter();   //�� �������Ź�

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR| RCC_APB1Periph_BKP, ENABLE);//Enable PWR clock

    PWR_BackupAccessCmd(ENABLE);
    ////////////////
    SleepDeep_ConfigRTCWakeUp();
    ///////////////////////////////
    SleepDeep_PortCtrlAccExtiInit();   
    ////////////////////////
    SleepDeep_StopMode();

   // PWR_FlashPowerDownCmd(ENABLE);
    ////////////////////////////////    
    SleepDeep_PortCtrlAccExtiDisable();
    SleepDeep_RTCWakeUpDisable();
    SleepDeep_ResetSCRSleepDeepBit();
    NVIC_SystemReset();
}
/*******************************************************************************
**  ��������  : SleepDeep_TerminalShutdown
**  ��������  : �����ն˹ػ�����,Acc������,Acc�ؽ����������,
                ֻҪ��ǰ��ѹ������0.5v������Acc�ر�ΪAcc�������¿���
**  ��    ��  : ��
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_TerminalShutdown(void)
{
    if(Io_ReadStatusBit(STATUS_BIT_ACC))//
    {
        NVIC_SystemReset();
    }
    else
    {
        LcdClearScreen();
        s_stSleepDAttrib.recoverVolt = SleepDeep_GetCurPowerValue()+5;
        SleepDeep_EnterStopMode();
    }
}
/*******************************************************************************
**  ��������  : SleepDeep_DisposeTerminalShutdown
**  ��������  : �����ն˹ػ�����,Acc����ʱ2������,Acc����ʱ2������������,
                ֻҪ��ǰ��ѹ������0.5v������Acc�ر�ΪAcc�������¿���
**  ��    ��  : ��
**  ��    ��  :  
**  ȫ�ֱ���  : 
**  ���ú���  : ��
**  �ж���Դ  : ��
**  ��    ע  :
*******************************************************************************/
void SleepDeep_DisposeTerminalShutdown(void)
{
    LZM_PublicSetOnceTimer(&s_stDSleepTimer[SLEEP_TIMER_TASK],PUBLICSECS(2),SleepDeep_TerminalShutdown);       
}
/*******************************************************************************
**  ��������  : SleepDeep_ReportServerEnterStopMode
**  ��������  : �������
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_ReportServerEnterStopMode(void)
{
	  
    if(1 == BBGNTestFlag)
		{
										
		}
		else
		{
        Report_UploadPositionInfo(CHANNEL_DATA_1);
        Report_UploadPositionInfo(CHANNEL_DATA_2);
        LZM_PublicSetOnceTimer(&s_stDSleepTimer[SLEEP_TIMER_SEND],PUBLICSECS(1.7),SleepDeep_ReportServerEnterStopMode);
		}
}
/*******************************************************************************
**  ��������  : SleepDeep_CheckForEnterStopMode
**  ��������  : �������
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_CheckForEnterStopMode(void)
{
    unsigned char acc;
    ///////////////////////////////
    if(0xffffffff == s_stSleepDAttrib.StopTime)
    {
        return;
    }
    ////////////////////////////////
    acc = Io_ReadStatusBit(STATUS_BIT_ACC);
    if(acc)
    {
        s_stSleepDAttrib.AccOffTimeCnt = 0;
        s_stSleepDAttrib.stopFlag = 0;
        if(s_stSleepDAttrib.clearRestartFlag)
        {
            s_stSleepDAttrib.clearRestartFlag =0;
            SetTimerTask(TIME_SYSRESET,12*SYSTICK_1HOUR);
        }
    }
    else
    {
        if(0==s_stSleepDAttrib.clearRestartFlag)
        {
            s_stSleepDAttrib.clearRestartFlag = 1;
            ClrTimerTask(TIME_SYSRESET);
        }
        s_stSleepDAttrib.AccOffTimeCnt++;
       if(s_stSleepDAttrib.AccOffTimeCnt>=s_stSleepDAttrib.StopTime)

        {
            ////////////////////
            if(SleepDeep_GetCurPowerValue()<s_stSleepDAttrib.recoverVolt)
            {
                if(0==s_stSleepDAttrib.stopFlag)
                {
                    s_stSleepDAttrib.stopFlag = 1;
                    ////////////////////////////////////
                    Io_WriteIoStatusBit(IO_STATUS_BIT_DEEP_SLEEP,SET);
                    /////////////////////////////////
                    SleepDeep_ReportServerEnterStopMode();
                    ////////////////////////////////
                    Public_ShowTextInfo("���������", PUBLICSECS(10));
                    ////////////////////////////////
                    LZM_PublicSetCycTimer(&s_stDSleepTimer[SLEEP_TIMER_TEST],PUBLICSECS(6),SleepDeep_EnterStopMode);
                }
            }
        }
    }
}
/*******************************************************************************
**  ��������  : SleepDeep_CheckCurrentVoltType
**  ��������  : ��鵱ǰ���ĸ���ѹϵͳ
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_CheckCurrentVoltType(void)
{
    unsigned short val;
    val = SleepDeep_GetCurPowerValue();
    if(val<180)//12Vϵͳ
    {
        s_stSleepDAttrib.VoltType =0;
    }
    else
    {
        s_stSleepDAttrib.VoltType =1;
    }
}
/*******************************************************************************
**  ��������  : SleepDeep_StartScanTask
**  ��������  : �������
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_StartScanTask(void)
{
    SleepDeep_CheckCurrentVoltType();
    SleepDeep_UpdateParam();
    LZM_PublicSetCycTimer(&s_stDSleepTimer[SLEEP_TIMER_TASK],PUBLICSECS(1),SleepDeep_CheckForEnterStopMode); 
}

/*******************************************************************************
**  ��������  : SleepDeep_UpdateParam
**  ��������  : �������
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_UpdateParam(void)
{
    #ifdef E2_SHUTDOWN_MODE_POWER_ID
    unsigned char buffer[6];
    if(EepromPram_ReadPram(E2_SHUTDOWN_MODE_TIME_ID,buffer))
    {
        s_stSleepDAttrib.StopTime = Public_ConvertBufferToLong(buffer);
        if(0 == s_stSleepDAttrib.StopTime)
        {
          s_stSleepDAttrib.StopTime =0xffffffff;
        }
        else
        if(s_stSleepDAttrib.StopTime < SLEEP_DEEP_MIN_STOP_TIME)
        {
            s_stSleepDAttrib.StopTime = SLEEP_DEEP_MIN_STOP_TIME;
        }
    }
    else
    {
        s_stSleepDAttrib.StopTime =0xffffffff;
    }
    ////////////////////////////////////
    if(EepromPram_ReadPram(E2_SHUTDOWN_MODE_POWER_ID,buffer))
    {
        s_stSleepDAttrib.recoverVolt = Public_ConvertBufferToShort(buffer);
    }
    else
    {
        if(0==s_stSleepDAttrib.VoltType)//12V
        {
            s_stSleepDAttrib.recoverVolt = WAKEUP_VOLT_12V;
        }
        else
        {
            s_stSleepDAttrib.recoverVolt = WAKEUP_VOLT_24V;
        }
    }
    #else
    s_stSleepDAttrib.StopTime =0xffffffff;
    #endif
    //////////////////////////
    if(0==s_stSleepDAttrib.VoltType)//12Vϵͳ
    {
        if((s_stSleepDAttrib.recoverVolt > SLEEPDEEP_VOLT_12V_MAX)||(s_stSleepDAttrib.recoverVolt<SLEEPDEEP_VOLT_12V_MIN))//14.6
        {
            s_stSleepDAttrib.recoverVolt = WAKEUP_VOLT_12V;
        }
    }
    else
    {
        if((s_stSleepDAttrib.recoverVolt > (SLEEPDEEP_VOLT_24V_MAX))||(s_stSleepDAttrib.recoverVolt<SLEEPDEEP_VOLT_24V_MIN))//27.2
        {
            s_stSleepDAttrib.recoverVolt = WAKEUP_VOLT_24V;
        }
    }
   
}
/*******************************************************************************
**  ��������  : SleepDeep_ParameterInitialize
**  ��������  : ������߳�ʼ��
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_ParameterInitialize(void)
{
    LZM_PublicKillTimerAll(s_stDSleepTimer,SLEEP_TIMERS_MAX);
    s_stSleepDAttrib.ExtiType =EXTI_TYPE_EMPTY;
    s_stSleepDAttrib.AccOffTimeCnt = 0;
    s_stSleepDAttrib.clearRestartFlag = 0;
    s_stSleepDAttrib.VoltType =0;//12v
    SleepDeep_PortCtrlAccExtiDisable();
    SleepDeep_RTCWakeUpDisable();    
    /////////////////////////////////////////    
    SetTimerTask(TIME_SLEEPDEEP, SYSTICK_0p1SECOND);
    LZM_PublicSetOnceTimer(&s_stDSleepTimer[SLEEP_TIMER_TASK],PUBLICSECS(10),SleepDeep_StartScanTask);
}

/*******************************************************************************
**  ��������  : SleepDeep_RTC_WKUP_IRQ
**  ��������  : ��ʱ���жϴ���
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_RTC_WKUP_IRQ(void)
{
    RTC_ITConfig(RTC_IT_ALR,ENABLE);
    if(RTC_GetITStatus(RTC_IT_ALR) != RESET) 
    {
     
            EXTI_ClearITPendingBit(EXTI_Line17);
     if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
            {
              /* Clear Wake Up flag */
            //  PWR_ClearFlag(PWR_FLAG_WU);
            }

        RTC_WaitForLastTask();
          
        /* Clear RTC Alarm interrupt pending bit */
        RTC_ClearITPendingBit(RTC_IT_ALR);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask(); 
        SleepDeep_SetRtcExtiType();
  }        
}
/*******************************************************************************
**  ��������  : SleepDeep_ACC_EXTI_IRQ
**  ��������  : Acc�жϴ���
**  ��    ��  : ��              
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void SleepDeep_ACC_EXTI_IRQ(void)
{
    if(EXTI_GetITStatus(SLEEP_DEEP_ACC_EXTI_LINE) != RESET) 
    {
        SleepDeep_SetAccExtiType();
        EXTI_ClearITPendingBit(SLEEP_DEEP_ACC_EXTI_LINE);                 //����ⲿ�жϱ�־
    }
}
/*************************************************************
** ��������: SleepDeep_TimeTask
** ��������: ��ʱ��������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState SleepDeep_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stDSleepTimer,SLEEP_TIMERS_MAX);
    return ENABLE;
}
/******************************************************************************
**                            End Of File
******************************************************************************/

