/************************************************************************
//程序名称：VDR_Pulse.c
//功能：脉冲模块处理，包括脉冲累计行驶里程、脉冲瞬时速度、脉冲秒速度的计算。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：累计行驶里程：采用脉冲计数的方式，每接收到一个脉冲信号，行驶里程累计一点
//脉冲瞬时速度：每个脉冲信号可以计算对应的一个速度值，每5个脉冲信号对应5个速度值，把最大值和最小值去掉再求平均得到瞬时速度
//脉冲秒速度：每个50ms读取一个瞬时速度，求这20个瞬时速度的平均值得到秒速度
//每分钟平均速度：每秒钟读取一个秒速度，求这60个秒速度的平均值得到每分钟平均速度
*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"

/********************本地变量*************************/
static void VDRPulse_InitTim(void);
static void VDRPulse_InitExti(void);
static void VDRPulse_Init(void);

/********************全局变量*************************/
u32	PulseTotalMile = 0;//累计行驶里程，即总里程,单位是0.01公里
u32	PulseCoef = 0;//脉冲系数
u8	VdrRunMode = 0;//0:正常出货运行模式;1:国标检测模式，该标志通过菜单可选择

/********************外部变量*************************/
extern	u8	SpeedFlag;//速度类型,0为脉冲,1为GPS
extern 	vu32 	myPulseCnt;             //校准计数使用
extern 	u8	TestStatus;//E0H:进入检定状态;E1H:进入里程误差测量;E2H:进入脉冲系数误差测量;E3H:进入实时时钟误差测量;E4H:退出检定状态

/********************本地函数声明*********************/
static	u16	PulseWidthMin = 0;//脉冲宽度计数最小值，小于该值认为是干扰脉冲
static 	u16	PulseWidth[VDR_INSTANT_SPEED_ARRAY_SIZE+1] = {0};
static 	u8	PulseWidthCount = 0;

static 	u32	PulseCount = 0;//脉冲计数，收1个脉冲，计数加1，当收到PulseCoef个脉冲时表示走了1公里

static 	u32 	PulseExtiCount = 0;//外部中断进入次数计数

static 	u8	PulseInstantSpeed = 0;//脉冲瞬时速度，km/h
static 	u8	LastPulseInstantSpeed = 0;//上一次脉冲速度
static 	u8      PulseSecondSpeed = 0;//脉冲每秒平均速度

/********************函数定义*************************/
/*********************************************************************
//函数名称	:VDRPulse_TimeTask
//功能		:脉冲定时任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:总是返回ENABLE
//备注		:50ms秒调用1次，定时记录（1秒）累计行驶里程，计算秒平均速度
*********************************************************************/
FunctionalState  VDRPulse_TimeTask(void)
{
	static	u8	InitFlag = 0;
  static  u16	Count = 0;
	static  u32	LasePulseTotalMile = 0;//上一次累计行驶里程，即总里程,单位是0.01公里
  static  u8      SecondSpeedData[20] = {0};//50ms读取一个瞬时速度，20个瞬时速度的平均值为每秒平均速度
  static  u8      SecondSpeedCount = 0;
     
  u8	TmpData[5];
  u8      i;
  u16     sum;
	TIME_T CurTime,Time;
        
	if(0 == InitFlag)
	{
		InitFlag = 1;
		VDRPulse_Init();
		VDRPulse_UpdateParameter();
		return ENABLE;
	}
        
  if(0 == (SpeedFlag&0x01))//脉冲速度
	{
		Count++;
	  if(Count >= 20)//1秒写一次里程,50ms调度一次
	  {
			Count = 0;
				if(LasePulseTotalMile != PulseTotalMile)
					{
						Public_ConvertLongToBuffer(PulseTotalMile,TmpData);
			      FRAM_BufferWrite(FRAM_VDR_MILEAGE_ADDR, TmpData, FRAM_VDR_MILEAGE_LEN); //存储总里程
            FRAM_BufferWrite(FRAM_VDR_MILEAGE_BACKUP_ADDR, TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN); //存储总里程
          }
	  } 
		
		RTC_ReadTime(&CurTime);
    if(CurTime.sec != Time.sec)//秒变化时刻计算秒速度
    {  
			if(SecondSpeedCount >= 20)
			{
				SecondSpeedCount = 19;
      }
      SecondSpeedData[SecondSpeedCount++] = VDRPulse_GetInstantSpeed();
      sum = 0;
      for(i=0; i<SecondSpeedCount; i++)
      {
				sum += SecondSpeedData[i];
      }
      PulseSecondSpeed = sum/SecondSpeedCount;
                        
      SecondSpeedCount = 0;
      for(i=0; i<20; i++)
      {
				SecondSpeedData[i] = 0;
      }
      Time.sec = CurTime.sec;
    }
    else
    {
      if(SecondSpeedCount >= 20)
      {
        SecondSpeedCount = 19;
      }
      SecondSpeedData[SecondSpeedCount++] = VDRPulse_GetInstantSpeed();
		}       
  }
  return ENABLE;
}
/*********************************************************************
//函数名称	:VDRPulse_UpdateParameter
//功能		:更新参数
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:包括脉冲系数、累计行驶里程
//		:当脉冲系数设置为3600或7200时会触发脉冲系数自动校准功能，国标检测时不能开启
*********************************************************************/
void VDRPulse_UpdateParameter(void)
{
	u8	TmpData[4] = {0};
	u8	PramLen = 0;
	u32	temp;

	//更新脉冲系数
	PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, TmpData);
	if(2 != PramLen)
	{
			PulseCoef = 7200;//默认为7200，取消了速度传感器系数
	}
	else
	{
			PulseCoef = 0;
			PulseCoef |= (TmpData[0] << 8);//高字节
			PulseCoef |= TmpData[1];//低字节位
	}

	//更新脉冲宽度最小限值
	temp = (3600*(u32)1000000)/(50*PulseCoef*255);
	PulseWidthMin = temp&0xffff;

	//更新累计行驶里程
	PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_LEN, FRAM_VDR_MILEAGE_ADDR);
	if(FRAM_VDR_MILEAGE_LEN == PramLen)//校验和正确
	{
		PulseTotalMile = 0;
		PulseTotalMile |= TmpData[0] << 24;	//高字节
		PulseTotalMile |= TmpData[1] << 16;	//次高字节
		PulseTotalMile |= TmpData[2] << 8;	//中字节
		PulseTotalMile |= TmpData[3];		//低字节
	}
        else
        {
                //读备份的累计行驶里程
	        PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN, FRAM_VDR_MILEAGE_BACKUP_ADDR);
	        if(FRAM_VDR_MILEAGE_BACKUP_LEN == PramLen)//校验和正确
	        {
		        PulseTotalMile = 0;
		        PulseTotalMile |= TmpData[0] << 24;	//高字节
		        PulseTotalMile |= TmpData[1] << 16;	//次高字节
		        PulseTotalMile |= TmpData[2] << 8;	//中字节
		        PulseTotalMile |= TmpData[3];		//低字节
	        }
                else
                {
                        PulseTotalMile = 0;
                }
        }

	if(0 == VdrRunMode)//正常出货模式
	{
		if((7200 == PulseCoef)||(3600 == PulseCoef))
		{
//			setAdjustFunction(ENABLE);//开启脉冲系数校准功能
		}
	}
	else
	{
		//国标检测模式
	}
	
}
/*********************************************************************
//函数名称	:VDRPulse_GetInstantSpeed
//功能		:获取脉冲瞬时速度,单位km/h
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
u8 VDRPulse_GetInstantSpeed(void)
{
	return PulseInstantSpeed;
}
/*********************************************************************
//函数名称	:VDRPulse_GetSecondSpeed
//功能		:获取脉冲秒平均速度,单位km/h
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
u8 VDRPulse_GetSecondSpeed(void)
{
	return PulseSecondSpeed;
}
/*********************************************************************
//函数名称	:VDRPulse_GetTotalMile
//功能		:获取脉冲总里程
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:单位是0.01公里
//		:
*********************************************************************/
u32 VDRPulse_GetTotalMile(void)
{
	return PulseTotalMile;
}


/*********************************************************************
//函数名称	:VDRPulse_AddMile
//功能		:累计里程
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
void VDRPulse_AddMile(void)
{
	u32	TimeCount;
	
	PulseExtiCount++;
	PulseCount++;
	myPulseCnt++; //校准脉冲使用

	if(1 == PulseExtiCount)
	{
		TIM_SetCounter(TIM3, 0);//定时器3计数清0
		TIM_Cmd(TIM3, ENABLE);//打开定时器3
	}
	else 
	{
		TimeCount = TIM_GetCounter(TIM3);//读计数器的值
		if(TimeCount >= PulseWidthMin)
		{
			//计算里程
			if(PulseCount > (PulseCoef/50))//大于等于0.02公里
			{
				PulseTotalMile += PulseCount*100/PulseCoef;
				PulseCount = 0;
			}

			if(0 == VdrRunMode)
			{
				PulseWidthCount++;
				if(PulseWidthCount >= VDR_INSTANT_SPEED_ARRAY_SIZE)
				{
					PulseWidthCount = 0;
				}
				PulseWidth[PulseWidthCount] = TimeCount;
			}
			else
			{
				PulseWidth[0] = TimeCount; 
                	}
		}
		else
		{
			myPulseCnt--;
			PulseCount--;
		}

		TIM_SetCounter(TIM3, 0);//定时器3计数清0
	}
}
/*********************************************************************
//函数名称	:VDRPulse_CalInstantSpeed
//功能		:计算瞬时速度
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
//		:
*********************************************************************/
void VDRPulse_CalInstantSpeed(void)
{
	static u16	Count = 0;
  u8	i = 0;
	u8	InstantSpeed;
	u16	CountMin = 0xffff;
	u16	CountMax = 0;
	u32	sum = 0;
	u32	v;
	float	temp = 0;
	

	if(0 == VdrRunMode)
	{
		for(i=0; i<VDR_INSTANT_SPEED_ARRAY_SIZE; i++)
		{
			if(PulseWidth[i] > CountMax)
			{
				CountMax = PulseWidth[i];
			}
			if(PulseWidth[i] < CountMin)
			{
				CountMin = PulseWidth[i];
			}
			sum += PulseWidth[i];
     		}
		sum -= CountMin;
		sum -= CountMax;
		sum = sum/3;
	}
	else
	{
		sum = PulseWidth[0];
	}
		
        if(0 == sum)
        {
               	return;
        }

	temp = (3600.0*(u32)1000000)/((float)sum*50.0*(float)PulseCoef);//对应50us计数单位
        v = (int)(temp);
        if((temp-v)>=0.5)
        {
        	v ++;
        }
	InstantSpeed = v&0xff;

	if(0 == VdrRunMode)
	{
		if(InstantSpeed > LastPulseInstantSpeed)
		{
			if((InstantSpeed-LastPulseInstantSpeed) >= 50)//跳变50认为是干扰
			{
				Count++;
				if(Count >= 100)//干扰产生连续100次，认为是异常掉电造成突变
				{
					Count = 0;
					PulseInstantSpeed = InstantSpeed;
					LastPulseInstantSpeed = InstantSpeed;
				}
			}
			else
			{
				Count = 0;
				PulseInstantSpeed = InstantSpeed;
				LastPulseInstantSpeed = InstantSpeed;
			}
		}
		else
		{
			Count = 0;
			PulseInstantSpeed = InstantSpeed;
			LastPulseInstantSpeed = InstantSpeed;
		}
	}
	else
	{
		PulseInstantSpeed = InstantSpeed;
		LastPulseInstantSpeed = InstantSpeed;
	}	
}
/*********************************************************************
//函数名称	:VDRPulse_Init
//功能		:初始化
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
static void VDRPulse_Init(void)
{
	VDRPulse_InitTim();
	VDRPulse_InitExti();
}
/*********************************************************************
//函数名称	:VDRPulse_InitTimer
//功能		:初始化定时器
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
static void VDRPulse_InitTim(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	u16	PrescalerValue;
	
	 /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* Compute the prescaler value */
  PrescalerValue = 3599;//50us,TIM2,3,4时钟为72MHz
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 20000;//1s
  //TIM_TimeBaseStructure.TIM_Period = 8000;//0.4s
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_SetCounter(TIM3, 0);
  
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* TIM Interrupts enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
/*********************************************************************
//函数名称	:VDRPulse_InitExti
//功能		:初始化外部中断
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
static void VDRPulse_InitExti(void)
{
		EXTI_InitTypeDef   EXTI_InitStructure;
  	GPIO_InitTypeDef   GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
	
  	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	
}
/*********************************************************************
//函数名称	:VDRPulse_TimIsr
//功能		:初始化定时器中断处理
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void VDRPulse_TimIsr(void)
{
	u8	i = 0;

	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
	  TIM_SetCounter(TIM3, 0);//定时器3计数清0
                
		TIM_Cmd(TIM3, DISABLE);//关闭定时器3

		PulseExtiCount = 0;//外部中断计数清0

		PulseInstantSpeed = 0;//瞬时速度清0
		LastPulseInstantSpeed = 0;

		for(i=0; i<VDR_INSTANT_SPEED_ARRAY_SIZE; i++)//脉宽计数清0
		{
			PulseWidth[i] = 0;
		}
		PulseWidthCount = 0;
		
	}
}
/*********************************************************************
//函数名称	:VDRPulse_ExtiIsr
//功能		:外部中断
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:收到脉冲信号（即产生外部中断）后的处理
//		:
*********************************************************************/
void VDRPulse_ExtiIsr(void)
{
	static u8	count = 0;
	
	if(0xE2 == TestStatus)//检定功能E2
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
	else if(0 == TestStatus)//正常应用
	{
		VDRPulse_AddMile();//累计行驶里程
		VDRPulse_CalInstantSpeed();//计算瞬时速度
	}

	/* Clear the EXTI line 0 pending bit */
    	EXTI_ClearITPendingBit(VDR_PULSE_EXTI_LINE);
        
}








