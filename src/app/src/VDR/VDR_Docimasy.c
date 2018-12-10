/********************************************************************
//版权说明  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称  :VDR_Docimasy.c     
//功能      :实现行驶记录仪的检定功能 
//版本号    :
//开发人    :dxl
//开发时间  :2013.3
//修改者    :
//修改时间  :
//修改简要说明  :
//备注      :
***********************************************************************/
//***************包含文件*****************
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

//****************宏定义****************
//***************常量定义***************
//*****************变量定义****************
u8  VdrDocimasyFlag = 0;//0:退出了检定状态;1:当前处于检定状态,收到检定命令E0时置1,收到检定命令E4时清0
u8  VdrDocimasyStatus = 0;//测试状态;E1H,进入里程误差测量;E2H,进入脉冲系数误差测量;E3H,进入实时时钟误差测量
static u8 EnterE0TimeCount = 0;//进入E0状态计时,收到E0后，PC机需6秒内发送E1或E2或E3进入检定检测，否则6秒后自动退出检定状态
u8  VdrOnlyOneID[36] = {0};//产品唯一性编号，之前是直接从eeprom读取，每秒1次，测试发现有时会出现0的情况（读取不成功）
//***************函数声明*******************
static void VDRDocimasy_InitExti14(void);
static void VDRDocimasy_InitExti10(void);
static void VDRDocimasy_InitTim4(u32 Time);
static void VDRDocimasy_InitGpioPB14(void);
static void VDRDocimasy_AckE1H(void);
//****************函数定义*****************
/*********************************************************************
//函数名称  :VDRDocimasy_GetFlag
//功能      :获得检定状态值
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :0:退出了检定状态;1:当前处于检定状态
*********************************************************************/
u8 VDRDocimasy_GetFlag(void)
{
    return VdrDocimasyFlag;
}
/*********************************************************************
//函数名称  :VDRDocimasy_EnterE0H
//功能      :E0H,进入或保持检定状态
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :收到检定命令E0时调用此函数
*********************************************************************/
void VDRDocimasy_EnterE0H(void)
{
//if(0 == VdrDocimasyFlag)//当前已为检定状态时PC机发送E0指令不理会
//{
  VdrDocimasyStatus = 0xE0;
    
  VdrDocimasyFlag = 1;
                
  //开启定时器4，定时1秒(参数值20000对应1秒)
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
//函数名称  :VDRDocimasy_EnterE1H
//功能      :E1H,进入里程误差测量
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :收到检定命令E1时调用此函数
*********************************************************************/
void VDRDocimasy_EnterE1H(void)
{
if(1 == VdrDocimasyFlag)
{   
  //初始化DB9的7脚为中断输入
  VDRDocimasy_InitExti14();
        
  //打开定时器4,定时1秒(参数值20000对应1秒),每秒发送一条E1指令到PC机
  VDRDocimasy_InitTim4(20000);
    
  VdrDocimasyStatus = 0xE1;
        
  EnterE0TimeCount = 0;
}
}
/*********************************************************************
//函数名称  :VDRDocimasy_EnterE2H
//功能      :E2H,进入脉冲系数误差测量
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :收到检定命令E2时调用此函数
*********************************************************************/
void VDRDocimasy_EnterE2H(void)
{
      
        if(1 == VdrDocimasyFlag)
        {   
            //初始化测试引脚为输出
            VDRDocimasy_InitGpioPB14();
                
                VDRDocimasy_InitExti10();
    
            VdrDocimasyStatus = 0xE2;
        
                EnterE0TimeCount = 0;
        }
    
}
/*********************************************************************
//函数名称  :VDRDocimasy_EnterE3H
//功能      :E3H,进入实时时钟误差测量
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :收到检定命令E3时调用此函数
*********************************************************************/
void VDRDocimasy_EnterE3H(void)
{
    
        if(1 == VdrDocimasyFlag)
        {   
            //初始化测试引脚为输出
            VDRDocimasy_InitGpioPB14();
        
                //打开定时器4,定时0.5秒(参数值20000对应1秒)
            VDRDocimasy_InitTim4(10000);
    
            VdrDocimasyStatus = 0xE3;
        
                EnterE0TimeCount = 0;
        }
    
}
/*********************************************************************
//函数名称  :VDRDocimasy_EnterE4H
//功能      :E4H,退出检定状态,恢复成正常状态
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :收到检定命令E4时调用此函数
*********************************************************************/
void VDRDocimasy_EnterE4H(void)
{
    //关闭定时器4
  TIM_Cmd(TIM4, DISABLE);
        
  //DB9脚设置为输出
    VDRDocimasy_InitGpioPB14();
        
  VDRPulse_Init();//恢复正常的脉冲线设置
        
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
//函数名称  :VDRDocimasy_ExtiIsr
//功能      :外部中断
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
//      :
*********************************************************************/
void VDRDocimasy_ExtiIsr(void)
{
        if((0xE1 == VdrDocimasyStatus)&&(1 == VdrDocimasyFlag))
        {
            VDRPulse_AddMile();
                    VDRPulse_CalInstantSpeed();//计算瞬时速度
        }
    
        EXTI_ClearITPendingBit(EXTI_Line14);
}
/*********************************************************************
//函数名称  :VDRDocimasy_Time4Isr
//功能      :定时器4中断
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :用于检定功能，正常应用时关闭，检定时打开
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
      VDRDocimasy_AckE1H();//每秒钟发送一个应答帧
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
////////////////////以下是内部函数/////////////////////
/*********************************************************************
//函数名称  :VDRDocimasy_InitExti14
//功能      :外部中断初始化
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
//      :
*********************************************************************/
static void VDRDocimasy_InitExti14(void)
{
    /* 以下是使用STM32F205的代码
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
//dxl,2015.8,以下是STM32F105的初始化
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
//函数名称  :VDRDocimasy_InitTim4
//功能      :定时器4初始化
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :Time:2000对应0.1秒的定时
*********************************************************************/
static void VDRDocimasy_InitTim4(u32 Time)
{ 
    /* 以下是使用STM32F205的代码
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

  //TIM_TimeBaseStructure.TIM_Period = 4000;//0.2秒
  //TIM_TimeBaseStructure.TIM_Period = 20000;//1s,与2399对应
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
     //dxl,2015.9,以下是STM32F105的初始化
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

  PrescalerValue = 3599;//50us,TIM2,3,4时钟为72MHz
    
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
//函数名称  :VDRDocimasy_InitGpioPB14
//功能      :测试引脚初始化为GPIO
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :
*********************************************************************/
static void VDRDocimasy_InitGpioPB14(void)
{
    /* 以下是使用STM32F205的代码
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    */
    // 以下是使用STM32F205的代码
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}
/*********************************************************************
//函数名称  :VDRDocimasy_InitExti10
//功能      :外部中断初始化
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :使用的是PB10，上升沿，下降沿均产生中断，仅用于检定功能
*********************************************************************/
static void VDRDocimasy_InitExti10(void)
{
    /* 以下是使用STM32F205的代码
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
    //dxl,2015.8,以下是STM32F105的初始化
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
* Description    : 记录仪鉴定命令->里程误差,需每秒调用一次
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
    nTab[5] = 0x12;//保留字节
    
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