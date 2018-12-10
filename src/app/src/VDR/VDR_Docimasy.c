/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :VDR_Docimasy.c     
//����      :ʵ����ʻ��¼�ǵļ춨���� 
//�汾��    :
//������    :dxl
//����ʱ��  :2013.3
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/
//***************�����ļ�*****************
//#include  "stm32f2xx.h"
//#include  "stm32f2xx_lib.h"
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"
#include  "VDR.h"
#include  "EepromPram.h"
#include  "Io.h"
#include  "VDR.h"
#include  "taskschedule.h"

//****************�궨��****************
//***************��������***************
//*****************��������****************
u8  VdrDocimasyFlag = 0;//0:�˳��˼춨״̬;1:��ǰ���ڼ춨״̬,�յ��춨����E0ʱ��1,�յ��춨����E4ʱ��0
u8  VdrDocimasyStatus = 0;//����״̬;E1H,�������������;E2H,��������ϵ��������;E3H,����ʵʱʱ��������
static u8 EnterE0TimeCount = 0;//����E0״̬��ʱ,�յ�E0��PC����6���ڷ���E1��E2��E3����춨��⣬����6����Զ��˳��춨״̬
u8  VdrOnlyOneID[36] = {0};//��ƷΨһ�Ա�ţ�֮ǰ��ֱ�Ӵ�eeprom��ȡ��ÿ��1�Σ����Է�����ʱ�����0���������ȡ���ɹ���
//***************��������*******************
static void VDRDocimasy_InitExti14(void);
static void VDRDocimasy_InitExti10(void);
static void VDRDocimasy_InitTim4(u32 Time);
static void VDRDocimasy_InitGpioPB14(void);
static void VDRDocimasy_AckE1H(void);
//****************��������*****************
/*********************************************************************
//��������  :VDRDocimasy_GetFlag
//����      :��ü춨״ֵ̬
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :0:�˳��˼춨״̬;1:��ǰ���ڼ춨״̬
*********************************************************************/
u8 VDRDocimasy_GetFlag(void)
{
    return VdrDocimasyFlag;
}
/*********************************************************************
//��������  :VDRDocimasy_EnterE0H
//����      :E0H,����򱣳ּ춨״̬
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�յ��춨����E0ʱ���ô˺���
*********************************************************************/
void VDRDocimasy_EnterE0H(void)
{
//if(0 == VdrDocimasyFlag)//��ǰ��Ϊ�춨״̬ʱPC������E0ָ����
//{
  VdrDocimasyStatus = 0xE0;
    
  VdrDocimasyFlag = 1;
                
  //������ʱ��4����ʱ1��(����ֵ20000��Ӧ1��)
  VDRDocimasy_InitTim4(20000);
        
//}
  EnterE0TimeCount = 0;
    
    ClrTimerTask(TIME_VDR_DATA);
    ClrTimerTask(TIME_VDR_PULSE);
    ClrTimerTask(TIME_VDR_POSITION);
    ClrTimerTask(TIME_VDR_SPEED);
    ClrTimerTask(TIME_VDR_DOUBT);
    ClrTimerTask(TIME_VDR_OVERTIME);
    ClrTimerTask(TIME_VDR_CARD);
    ClrTimerTask(TIME_VDR_POWER);
    ClrTimerTask(TIME_VDR_SPEED_STATUS);
    ClrTimerTask(TIME_VDR_OVER_SPEED);
    
}
/*********************************************************************
//��������  :VDRDocimasy_EnterE1H
//����      :E1H,�������������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�յ��춨����E1ʱ���ô˺���
*********************************************************************/
void VDRDocimasy_EnterE1H(void)
{
if(1 == VdrDocimasyFlag)
{   
  //��ʼ��DB9��7��Ϊ�ж�����
  VDRDocimasy_InitExti14();
        
  //�򿪶�ʱ��4,��ʱ1��(����ֵ20000��Ӧ1��),ÿ�뷢��һ��E1ָ�PC��
  VDRDocimasy_InitTim4(20000);
    
  VdrDocimasyStatus = 0xE1;
        
  EnterE0TimeCount = 0;
}
}
/*********************************************************************
//��������  :VDRDocimasy_EnterE2H
//����      :E2H,��������ϵ��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�յ��춨����E2ʱ���ô˺���
*********************************************************************/
void VDRDocimasy_EnterE2H(void)
{
      
        if(1 == VdrDocimasyFlag)
        {   
            //��ʼ����������Ϊ���
            VDRDocimasy_InitGpioPB14();
                
                VDRDocimasy_InitExti10();
    
            VdrDocimasyStatus = 0xE2;
        
                EnterE0TimeCount = 0;
        }
    
}
/*********************************************************************
//��������  :VDRDocimasy_EnterE3H
//����      :E3H,����ʵʱʱ��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�յ��춨����E3ʱ���ô˺���
*********************************************************************/
void VDRDocimasy_EnterE3H(void)
{
    
        if(1 == VdrDocimasyFlag)
        {   
            //��ʼ����������Ϊ���
            VDRDocimasy_InitGpioPB14();
        
                //�򿪶�ʱ��4,��ʱ0.5��(����ֵ20000��Ӧ1��)
            VDRDocimasy_InitTim4(10000);
    
            VdrDocimasyStatus = 0xE3;
        
                EnterE0TimeCount = 0;
        }
    
}
/*********************************************************************
//��������  :VDRDocimasy_EnterE4H
//����      :E4H,�˳��춨״̬,�ָ�������״̬
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�յ��춨����E4ʱ���ô˺���
*********************************************************************/
void VDRDocimasy_EnterE4H(void)
{
    //�رն�ʱ��4
  TIM_Cmd(TIM4, DISABLE);
        
  //DB9������Ϊ���
    VDRDocimasy_InitGpioPB14();
        
  VDRPulse_Init();//�ָ�����������������
        
    VdrDocimasyStatus = 0;
    VdrDocimasyFlag = 0;
        EnterE0TimeCount = 0;
    
    SetTimerTask(TIME_VDR_DATA,1);
    SetTimerTask(TIME_VDR_PULSE,1);
    SetTimerTask(TIME_VDR_POSITION,1);
    SetTimerTask(TIME_VDR_SPEED,1);
    SetTimerTask(TIME_VDR_DOUBT,1);
    SetTimerTask(TIME_VDR_OVERTIME,SECOND);
    SetTimerTask(TIME_VDR_CARD,SYSTICK_0p1SECOND);
    SetTimerTask(TIME_VDR_POWER,1);
    SetTimerTask(TIME_VDR_SPEED_STATUS,1);
    SetTimerTask(TIME_VDR_OVER_SPEED,SECOND);
}

/*********************************************************************
//��������  :VDRDocimasy_ExtiIsr
//����      :�ⲿ�ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
//      :
*********************************************************************/
void VDRDocimasy_ExtiIsr(void)
{
        if((0xE1 == VdrDocimasyStatus)&&(1 == VdrDocimasyFlag))
        {
            VDRPulse_AddMile();
                    VDRPulse_CalInstantSpeed();//����˲ʱ�ٶ�
        }
    
        EXTI_ClearITPendingBit(EXTI_Line14);
}
/*********************************************************************
//��������  :VDRDocimasy_Time4Isr
//����      :��ʱ��4�ж�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :���ڼ춨���ܣ�����Ӧ��ʱ�رգ��춨ʱ��
*********************************************************************/
void VDRDocimasy_Tim4Isr(void)
{
  static u8 count = 0;
        
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        
    TIM_SetCounter(TIM4, 0);
    
    if((0xE0 == VdrDocimasyStatus)&&(1 == VdrDocimasyFlag))
    {
      EnterE0TimeCount++;
      if(EnterE0TimeCount >= 6)
      {
        VDRDocimasy_EnterE4H();
      }
    }
    else if((0xE1 == VdrDocimasyStatus)&&(1 == VdrDocimasyFlag))
    {
      VDRDocimasy_AckE1H();//ÿ���ӷ���һ��Ӧ��֡
    }
    else if((0xE3 == VdrDocimasyStatus)&&(1 == VdrDocimasyFlag))
    {
      count++;
      if(count >= 2)
      {
        count = 0;
      }
      if(0 == count)
      {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
      }
      else
      {
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
      }
    }
  }
}
////////////////////�������ڲ�����/////////////////////
/*********************************************************************
//��������  :VDRDocimasy_InitExti14
//����      :�ⲿ�жϳ�ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
//      :
*********************************************************************/
static void VDRDocimasy_InitExti14(void)
{
    /* ������ʹ��STM32F205�Ĵ���
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
         
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);

  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
*/  
//dxl,2015.8,������STM32F105�ĳ�ʼ��
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
    
  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);   
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/*********************************************************************
//��������  :VDRDocimasy_InitTim4
//����      :��ʱ��4��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :Time:2000��Ӧ0.1��Ķ�ʱ
*********************************************************************/
static void VDRDocimasy_InitTim4(u32 Time)
{ 
    /* ������ʹ��STM32F205�Ĵ���
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  u16   PrescalerValue;
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
        
  PrescalerValue = 2399;

  //TIM_TimeBaseStructure.TIM_Period = 4000;//0.2��
  //TIM_TimeBaseStructure.TIM_Period = 20000;//1s,��2399��Ӧ
  TIM_TimeBaseStructure.TIM_Period = Time;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM4, 0);
  
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM4, ENABLE);
    */
     //dxl,2015.9,������STM32F105�ĳ�ʼ��
    NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  u16   PrescalerValue;
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  PrescalerValue = 3599;//50us,TIM2,3,4ʱ��Ϊ72MHz
    
  //TIM_TimeBaseStructure.TIM_Period = 20000;//1s
  //TIM_TimeBaseStructure.TIM_Period = 8000;//0.4s
    TIM_TimeBaseStructure.TIM_Period = Time;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM4, 0);
  
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM4, ENABLE);
    
}
/*********************************************************************
//��������  :VDRDocimasy_InitGpioPB14
//����      :�������ų�ʼ��ΪGPIO
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
static void VDRDocimasy_InitGpioPB14(void)
{
    /* ������ʹ��STM32F205�Ĵ���
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    */
    // ������ʹ��STM32F205�Ĵ���
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}
/*********************************************************************
//��������  :VDRDocimasy_InitExti10
//����      :�ⲿ�жϳ�ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ʹ�õ���PB10�������أ��½��ؾ������жϣ������ڼ춨����
*********************************************************************/
static void VDRDocimasy_InitExti10(void)
{
    /* ������ʹ��STM32F205�Ĵ���
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);

  EXTI_InitStructure.EXTI_Line = EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    */
    //dxl,2015.8,������STM32F105�ĳ�ʼ��
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    
  EXTI_InitStructure.EXTI_Line = EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);   
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
}
/*******************************************************************************
* Function Name  : VDRDocimasy_AckE1H
* Description    : ��¼�Ǽ�������->������,��ÿ�����һ��
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
static void VDRDocimasy_AckE1H(void)
{
    u8 nTab[55];
    u8 nBuff[5];
 
    u16 temp;
    u32 temp_hex;
    
    u8  nData;
    u8  i;
    u8  verify;
    
    memset(nTab,0,sizeof(nTab));                                                     
    
    nTab[0] = 0x55;
    nTab[1] = 0x7a;
    nTab[2] = 0xe1;
    nTab[3] = 0;
    nTab[4] = 44;
    nTab[5] = 0x12;//�����ֽ�
    
    if(0 == VdrOnlyOneID[0])
        {
      EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,VdrOnlyOneID);
            return ;
        }
        else
        {
          memcpy(nTab+6,VdrOnlyOneID,35);
        }
        
    
    EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID,nBuff);                       

    nTab[35+6] = nBuff[1];
    nTab[36+6] = nBuff[2];
    
    temp = VDRPulse_GetInstantSpeed();                                      
    temp = temp * 10;
    nTab[37+6] = (temp&0xff00)>>8;
    nTab[38+6] = temp;
      
    temp_hex = VDRPulse_GetTotalMile()*10;                                           
    nTab[39+6] = temp_hex>>24;
    nTab[40+6] = (temp_hex>>16)&0x0000ff;
    nTab[41+6] = (temp_hex>>8) &0x0000ff;
    nTab[42+6] = temp_hex & 0x000000ff; 
        
    nData = Io_ReadExtCarStatus();  
    nData = (nData<<1) | Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE);          
    nData = (nData<<1) | Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT);   
    nData = (nData<<1) | Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT); 
    nData = (nData<<1) | Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT);   
    nData = (nData<<1) | Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT);  
    nData = (nData<<3);                                                      
       
    nTab[43+6] = nData;     
        
    verify = 0;    
    for(i=0; i<(44+6); i++)
    {
         verify ^= nTab[i];
    }
    nTab[50] = verify;
    
    VDRUsart_SendData(nTab,51); 
  
}