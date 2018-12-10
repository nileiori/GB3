/************************************************************************
//程序名称：Can_Driver.c
//功能：该模块实现CAN接口（CAN1）数据的收发
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.12
//版本记录：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include <string.h>

#include "Can_Driver.h"
#include "queue.h"
#include "rtc.h"
#include "Public.h"
#include "RadioProtocol.h"
#include "Can_App.h"
#include "EepromPram.h"

/********************本地变量*************************/

	
/********************全局变量*************************/
Queue CanRxQ;
u8 CanRxBuffer[CAN_RX_BUFFER_SIZE] = {0};

/********************外部变量*************************/
extern CAN_FILTER_STRUCT CanFilter[];
extern u32 Can1GatherTime;	//CAN1 采集时间间隔，为0表示不采集
extern u16 Can1UploadTime;	//CAN1 上传时间间隔，为0表示不上传
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
/********************本地函数声明*********************/
static void Can_1_Init(void);
static void Can_1_Baud_Init(void);
static void Can_1_Mask_Init(void);

/********************函数定义*************************/
/**************************************************************************
//函数名：Can_Init
//功能：实现CAN接口初始化
//输入：无
//输出：无
//返回值：无
//备注：包含CAN1，CAN2的初始化，由于现在CAN2没有使用，所以只有CAN1的，
//该函数需要在CAN应用任务运行前或一运行CAN应用任务就调用
***************************************************************************/
void Can_Init(void)
{
	/*
	u8 Buffer[10];
	
	Buffer[0] = 0x00;
  Buffer[1]	= 0x00;
  Buffer[2] = 0x00;
  Buffer[3] =	0x00;
  Buffer[4]	= 0x58;
  Buffer[5] =	0xF8;
  Buffer[6] =	0x1E;
  Buffer[7] =	0x9E;
	EepromPram_WritePram(E2_CAN_SET_ONLY_GATHER_0_ID,Buffer,E2_CAN_SET_ONLY_GATHER_LEN);
	
	Buffer[0] = 0x00;
  Buffer[1]	= 0x00;
  Buffer[2] = 0x00;
  Buffer[3] =	0x64;
  Buffer[4]	= 0x58;
  Buffer[5] =	0xFF;
  Buffer[6] =	0xD1;
  Buffer[7] =	0x17;
	EepromPram_WritePram(E2_CAN_SET_ONLY_GATHER_1_ID,Buffer,E2_CAN_SET_ONLY_GATHER_LEN);
	*/
	Can_1_Init();//CAN 1初始化
	
	
}
/**************************************************************************
//函数名：Can_1_Rx_Isr
//功能：实现CAN1接收中断处理
//输入：无
//输出：无
//返回值：无
//备注：中断服务函数需调用此函数
***************************************************************************/
void Can_1_Rx_Isr(void)
{
	CanRxMsg Rx;
	u8 Buffer[CAN_STEP_LEN_MAX+1]={0};
	u8 length;
	u32 temp;
	TIME_T time;

		
	CAN_Receive(CAN1,CAN_FIFO0,&Rx);

	if((Rx.IDE == CAN_ID_STD)&&(8 == Rx.DLC))
	{
		if(0 == Can_SoftwareFilter(0,Rx.StdId,0))
		{
			RTC_GetCurTime(&time);//根据行标协议0x0705组包，5字节时间，毫秒填充为0
			Buffer[1] = Public_HEX2BCD(time.hour);
			Buffer[2] = Public_HEX2BCD(time.min);
			Buffer[3] = Public_HEX2BCD(time.sec);
			Buffer[4] = 0;
			Buffer[5] = 0;
			
			temp = Rx.StdId;//4字节ID，bit31表示CAN通道号：0为CAN1，1为CAN2；bit30表示帧类型：0为标准帧，1为扩展帧；
			Public_ConvertLongToBuffer(temp,Buffer+6);//bit29表示数据采集方式，0为原始值，1为平均值；bit28~bit0表示帧ID

			memcpy(Buffer+10,&Rx.Data[0],8);//8字节数据

			length = TransMean(Buffer+1,17);//转义

			Buffer[0] = 0x7e;//补充帧头
			
			Buffer[length+1] = 0x7e;//补充帧尾

			if(0 != Can1UploadTime)
			{
				QueueInBuffer(Buffer,length+2,CanRxQ);
			}

		}
			
	}
	else if((Rx.IDE == CAN_ID_EXT)&&(8 == Rx.DLC))
	{
		if(0 == Can_SoftwareFilter(0,Rx.ExtId,1))
		{
			RTC_GetCurTime(&time);//根据行标协议0x0705组包，5字节时间，毫秒填充为0
			Buffer[1] = Public_HEX2BCD(time.hour);
			Buffer[2] = Public_HEX2BCD(time.min);
			Buffer[3] = Public_HEX2BCD(time.sec);
			Buffer[4] = 0;
			Buffer[5] = 0;
			 
			if(1 == BBGNTestFlag)//dxl,2016.5.21检测平台有bug,是扩展帧时不能将bit30置位
			{
			    temp = Rx.ExtId;
			}
			else
			{
			    temp = Rx.ExtId|(1 << 30);//4字节ID，bit31表示CAN通道号：0为CAN1，1为CAN2；bit30表示帧类型：0为标准帧，1为扩展帧；
			}
			Public_ConvertLongToBuffer(temp,Buffer+6);//bit29表示数据采集方式，0为原始值，1为平均值；bit28~bit0表示帧ID

			memcpy(Buffer+10,&Rx.Data[0],8);//8字节数据

			length = TransMean(Buffer+1,17);//转义

			Buffer[0] = 0x7e;//补充帧头
			
			Buffer[length+1] = 0x7e;//补充帧尾

			QueueInBuffer(Buffer,length+2,CanRxQ);
			
		}

	}
	else
	{
		//不处理

	}

}

/**************************************************************************
//函数名：Can_1_Gpio_Init
//功能：CAN1 GPIO、时钟初始化，包括CAN模块电源控制引脚
//输入：无
//输出：无
//返回值：无
//备注：无
***************************************************************************/
static void Can_1_Init(void)
{

	NVIC_InitTypeDef    NVIC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//dxl,2016.5.21,这里必须设置为GPIO_Mode_IPU
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//dxl,2016.5.21新增 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	AFIO->MAPR |= GPIO_Remap2_CAN1;	
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	Can_1_Baud_Init();//CAN1波特率初始化，250KHZ
	Can_1_Mask_Init();//CAN1掩码初始化

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//打开CAN的接收中断

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	CAN_1_POWER_ON();
	

	
}
/**************************************************************************
//函数名：Can_1_Baud_Init
//功能：CAN1波特率初始化
//输入：无
//输出：无
//返回值：无
//备注：无
***************************************************************************/
static void Can_1_Baud_Init(void)
{
	u32 BaudRate;
	CAN_InitTypeDef	CAN_InitStructure;

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);

	CAN_DeInit(CAN1);

	CAN_StructInit(&CAN_InitStructure);

	BaudRate = CAN_1_BAUDRATE;

	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = ENABLE;//自动唤醒模式：硬件检测到CAN报文时自动离开休眠模式
	CAN_InitStructure.CAN_NART = ENABLE;//非自动重传输模式
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = ENABLE;//发送FIFO优先级
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//CAN波特率==PCLK1/[(1+CAN_BS1+CAN_BS2)*CAN_Prescaler]
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	if(BaudRate <= 500000)
	{
		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
		CAN_InitStructure.CAN_Prescaler = 36000000/(BaudRate*16);//PCLK1时钟为36000000,
	}
	else if(BaudRate <= 800000)
	{
		CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
		CAN_InitStructure.CAN_Prescaler = 36000000/(BaudRate*10);
	}
	else
	{
		CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
		CAN_InitStructure.CAN_Prescaler = 36000000/(BaudRate*8);
	}

	CAN_Init(CAN1,&CAN_InitStructure);
}
/**************************************************************************
//函数名：Can_1_Mask_Init
//功能：CAN1掩码初始化
//输入：无
//输出：无
//返回值：无
//备注：只允许以下ID的报文通过
***************************************************************************/
static void Can_1_Mask_Init(void)
{
	//无硬件掩码功能
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	CAN_FilterInitStructure.CAN_FilterNumber = 1;      //指定了待初始化的过滤器，它的范围是1到13
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdMask;//过滤器被初始化为标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit;//给出了过滤器位宽
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;//(u16)(filterId>>16); //用来设定过滤器标识符(32位位宽时为其高段位)
	CAN_FilterInitStructure.CAN_FilterIdLow  = 0x0000;//(u16)(filterId&0xFFFF);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//(u16)(filterMaskId>>16); //用来设定过滤器屏蔽标识符或者过滤器标识符(32位位宽时为其高段位)
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0x0000;//(u16)(filterMaskId&0xFFFF);  //对应位为 0:不比较; 1:必须匹配
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; //通过此滤波器信息包将被放在FIFO0里
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;    //使能/失能 过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	
}
/**************************************************************************
//函数名：CAN_1_ID_Filter
//功能：CAN1 ID过滤设置
//输入：无
//输出：无
//返回值：无
//备注：IdType,0:标准帧，1:扩展帧
***************************************************************************/
void CAN_1_ID_Filter(u8 FilterNum, u32 FilterId, u32 FilterMaskId, u8 IdType)
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	CAN_FilterInitStructure.CAN_FilterNumber = FilterNum;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((FilterId << 3)&0xFFFF0000) >> 16;
	if(0 == IdType)
	{
		CAN_FilterInitStructure.CAN_FilterIdLow = (FilterId << 3)&0xFFFF;
	}
	else
	{
		CAN_FilterInitStructure.CAN_FilterIdLow = ((FilterId << 3)|0x0004)&0xFFFF;
	}
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((FilterMaskId << 3)&0xFFFF0000) >> 16;
	if(0 == IdType)
	{
		CAN_FilterInitStructure.CAN_FilterMaskIdLow = (FilterMaskId << 3)&0xFFFF;	
	}
	else
	{
		CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((FilterMaskId << 3)|0x0004)&0xFFFF;
	}
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}



























