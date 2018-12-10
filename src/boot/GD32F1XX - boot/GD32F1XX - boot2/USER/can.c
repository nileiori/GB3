/*************************************************************************
*
* Copyright (c) 2008,深圳市伊爱高新技术开发有限公司
* All rights reserved.
*
* 文件名称 : HandleCan.c
* 功能     : CAN总线的驱动(扩展帧模式)
*
* 当前版本 :1.0.1
* 开发者   :xiezm
* 修改时间 :2009.5.9
* 修改内容 : 1.修改CAN总线口GPIOA11/12
*
* 创建版本 : 1.0
* 开发者   : zhulin
* 创建时间 : 2009年4月25日
*
* 备注     : 1.如果采用 占先式系统调度方式,则要修改 HandleCan_send2Bus 和
*              HandleCan_receFromBus 两个函数.
*            2.发送的数据填充到CAN寄存器里的方式,要修改.(中断里)
*            3.CAN总线出错处理还没有实现.

*************************************************************************/
#include  <string.h>
#include "Can.h"
#include "stm32f10x_can.h"

//------------- 全局变量 --------------------------------------------
CAN_ISR_DATA    gCanIsrData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//CAN_REC_DATA Can_RecBuff[CAN_MAX_RECE_NUM];

void CanBus_SetIdentifie(u32 filterId, u32 filterMaskId);
void CANBus_Init(void);
void CanBus_SetBaudRate(u32 BandRateBuf);
void CanBus_SetIdentifie2(u32 filterId, u32 filterMaskId);
void CANBus_Init2(void);
void CanBus_SetBaudRate2(u32 BandRateBuf);
/********************************************************************
* 名称 : CanBusInit
* 功能 : 初始化 CAN的驱动.
*
* 输入 : memFlag : SYSTEM_MEN_ERROR:存储器的值被冲乱(开机,干扰...);
*                  SYSTEM_MEN_OK:存储器没有被冲乱(软件复位)
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.只能在特权模式下运行???
*       2.中断优先级 要考虑;
*       3.CAN时钟与CAN端口复用有关,只打开相应的IO端口
*       4.端口复用与硬件有关,每个硬件电路板都要检查相应的设置.(包含复用时钟是否需要打开)
********************************************************************/
void CanHwInit(void)
{
	NVIC_InitTypeDef    tempNVIC_InitStructure;
	GPIO_InitTypeDef    tempGPIO_InitStructure;

	//--------- 时钟初始化 --------------------------
	//使能/失能 APB1,APB2外设时钟 (CAN使用复用功能,开启复用功能时钟)
	  /* GPIOA and GPIO_LED clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	  /* CAN Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//--------- 复用IO端口初始化 ----------------------------

	  //功能复用设置
	  /* Configure CAN pin: RX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);
	  /* Configure CAN pin: TX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);

	/* 重映射CAN1的IO脚 */
	AFIO->MAPR |= GPIO_Remap2_CAN1;	//重映射 Weite

	//----------中断初始化 ---------------------------------------
	  /* Enable CAN RX0 interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;		//USB_LP_CAN_RX0_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

	/* Enable CAN TX interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;		//USB_HP_CAN_TX_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

		//------ 根据 配置参数, 初始化CAN总线处理的一些参数 ------
	CANBus_Init();
	/* CAN register init */

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//打开CAN的接收中断

	//-----------------CAN电源开关-----------------Weite
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);

	CAN_PWR_ON();	//开CAN电源  Weite
	
}
/********************************************************************
* 名称 : CanBusInit
* 功能 : 初始化 CAN的驱动.
*
* 输入 : memFlag : SYSTEM_MEN_ERROR:存储器的值被冲乱(开机,干扰...);
*                  SYSTEM_MEN_OK:存储器没有被冲乱(软件复位)
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.只能在特权模式下运行???
*       2.中断优先级 要考虑;
*       3.CAN时钟与CAN端口复用有关,只打开相应的IO端口
*       4.端口复用与硬件有关,每个硬件电路板都要检查相应的设置.(包含复用时钟是否需要打开)
********************************************************************/
void CanHwInit2(void)
{
	NVIC_InitTypeDef    tempNVIC_InitStructure;
	GPIO_InitTypeDef    tempGPIO_InitStructure;

	//--------- 时钟初始化 --------------------------
	//使能/失能 APB1,APB2外设时钟 (CAN使用复用功能,开启复用功能时钟)
	  /* GPIOA and GPIO_LED clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	  /* CAN Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//--------- 复用IO端口初始化 ----------------------------

	  //功能复用设置
	  /* Configure CAN pin: RX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &tempGPIO_InitStructure);
	  /* Configure CAN pin: TX */
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &tempGPIO_InitStructure);

	/* 重映射CAN1的IO脚 */
	//AFIO->MAPR |= GPIO_Remap2_CAN1;	//重映射 Weite

	//----------中断初始化 ---------------------------------------
	  /* Enable CAN RX0 interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;		//CAN2_RX0_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

	/* Enable CAN TX interrupt IRQ channel */
	tempNVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;		//CAN2_TX_IRQChannel;
	tempNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	tempNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&tempNVIC_InitStructure);

		//------ 根据 配置参数, 初始化CAN总线处理的一些参数 ------
	CANBus_Init2();
	/* CAN register init */

	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);//打开CAN的接收中断

	//-----------------CAN电源开关-----------------Weite
	tempGPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	tempGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	tempGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &tempGPIO_InitStructure);

	CAN_PWR_ON();	//开CAN电源  Weite
	
}

/********************************************************************
* 名称 : Can_Isr_Tx
* 功能 : CAN总线发送服务函数.
*
* 输入 : 无
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
********************************************************************/
void Can_Isr_Tx(void)
{
	CanTxMsg  TxMessage;
	u8     *sendBufAddr;

	if((gCanIsrData.sendLen != 0) && (gCanIsrData.sendIndex < gCanIsrData.sendLen))
	{
		sendBufAddr = (u8*)&gCanIsrData.sendBuffer[gCanIsrData.sendIndex];
		//标准帧才设置此参数 //TxMessage.StdId = ((u32)sendBufAddr[0] << 6) | ((u32)sendBufAddr[1] >> 2);
		//TxMessage.ExtId = ((u32)sendBufAddr[0] << 24) | ((u32)sendBufAddr[1] << 16) | ((u32)sendBufAddr[2] << 8) | ((u32)sendBufAddr[3]);

#if (CUSTUMTYPE != 0x10)
		memcpy((u8*)&TxMessage.ExtId, sendBufAddr, 4);
		TxMessage.IDE   = CAN_ID_EXT;//扩展帧
#else
    memcpy((u8*)&TxMessage.StdId, sendBufAddr, 4);//StdId
		TxMessage.IDE   = CAN_ID_STD;//CAN_ID_EXT;//扩展帧
#endif

		TxMessage.RTR   = CAN_RTR_DATA;//数据帧
		TxMessage.DLC   = sendBufAddr[4] & 0x0F;
		memcpy(TxMessage.Data, &sendBufAddr[5], 8);

		if(CAN_Transmit(CAN1, &TxMessage) != CAN_NO_MB)
		{
			gCanIsrData.sendIndex ++;
			if(gCanIsrData.sendIndex >= gCanIsrData.sendLen)
			{
				gCanIsrData.sendLen = 0;
				CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE); //关闭发送中断
			}
		}
	}
	else
	{
		gCanIsrData.sendLen = 0;
		CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE); //关闭发送中断
	}
}
/********************************************************************
* 名称 : Can_Isr_Tx
* 功能 : CAN总线发送服务函数.
*
* 输入 : 无
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
********************************************************************/
void Can_Isr_Tx2(void)
{
	CanTxMsg  TxMessage;
	u8     *sendBufAddr;

	if((gCanIsrData.sendLen != 0) && (gCanIsrData.sendIndex < gCanIsrData.sendLen))
	{
		sendBufAddr = (u8*)&gCanIsrData.sendBuffer[gCanIsrData.sendIndex];
		//标准帧才设置此参数 //TxMessage.StdId = ((u32)sendBufAddr[0] << 6) | ((u32)sendBufAddr[1] >> 2);
		//TxMessage.ExtId = ((u32)sendBufAddr[0] << 24) | ((u32)sendBufAddr[1] << 16) | ((u32)sendBufAddr[2] << 8) | ((u32)sendBufAddr[3]);

#if (CUSTUMTYPE != 0x10)
		memcpy((u8*)&TxMessage.ExtId, sendBufAddr, 4);
		TxMessage.IDE   = CAN_ID_EXT;//扩展帧
#else
    memcpy((u8*)&TxMessage.StdId, sendBufAddr, 4);//StdId
		TxMessage.IDE   = CAN_ID_STD;//CAN_ID_EXT;//扩展帧
#endif

		TxMessage.RTR   = CAN_RTR_DATA;//数据帧
		TxMessage.DLC   = sendBufAddr[4] & 0x0F;
		memcpy(TxMessage.Data, &sendBufAddr[5], 8);

		if(CAN_Transmit(CAN2, &TxMessage) != CAN_NO_MB)
		{
			gCanIsrData.sendIndex ++;
			if(gCanIsrData.sendIndex >= gCanIsrData.sendLen)
			{
				gCanIsrData.sendLen = 0;
				CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE); //关闭发送中断
			}
		}
	}
	else
	{
		gCanIsrData.sendLen = 0;
		CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE); //关闭发送中断
	}
}

/********************************************************************
* 名称 : Can_Isr_Rx
* 功能 : CAN总线接收服务函数.
*
* 输入 : 无
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
********************************************************************/
void Can_Isr_Rx(void)
{
	CanRxMsg  RxMessage;
	u8     i,num;

	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	num = gCanIsrData.receIndex;

	if(RxMessage.IDE == CAN_ID_STD)
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.StdId), 4);
	}
	else
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.ExtId), 4);
	}
	i = 4;
	gCanIsrData.receBuffer[num][i] = ((RxMessage.IDE | RxMessage.RTR) << 4) | (RxMessage.DLC);
	i++;
	memcpy((u8*)&gCanIsrData.receBuffer[num][i],RxMessage.Data, 8);

	//循环接收
	gCanIsrData.receIndex++;
	if(gCanIsrData.receIndex >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receIndex = 0;
	}

	//增加未读个数
	gCanIsrData.receNum++;
	if(gCanIsrData.receNum >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receNum = CAN_MAX_RECE_NUM;
		gCanIsrData.UnreadIndex = gCanIsrData.receIndex;
	}
	else
	{
		gCanIsrData.UnreadIndex = 0;
	}
}
/********************************************************************
* 名称 : Can_Isr_Rx
* 功能 : CAN总线接收服务函数.
*
* 输入 : 无
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
********************************************************************/
void Can_Isr_Rx2(void)
{
	CanRxMsg  RxMessage;
	u8     i,num;

	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	num = gCanIsrData.receIndex;

	if(RxMessage.IDE == CAN_ID_STD)
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.StdId), 4);
	}
	else
	{
		memcpy((u8*)&gCanIsrData.receBuffer[num][0], (u8*)(&RxMessage.ExtId), 4);
	}
	i = 4;
	gCanIsrData.receBuffer[num][i] = ((RxMessage.IDE | RxMessage.RTR) << 4) | (RxMessage.DLC);
	i++;
	memcpy((u8*)&gCanIsrData.receBuffer[num][i],RxMessage.Data, 8);

	//循环接收
	gCanIsrData.receIndex++;
	if(gCanIsrData.receIndex >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receIndex = 0;
	}

	//增加未读个数
	gCanIsrData.receNum++;
	if(gCanIsrData.receNum >= CAN_MAX_RECE_NUM)
	{
		gCanIsrData.receNum = CAN_MAX_RECE_NUM;
		gCanIsrData.UnreadIndex = gCanIsrData.receIndex;
	}
	else
	{
		gCanIsrData.UnreadIndex = 0;
	}
}

/********************************************************************
* 名称 : CanBus_Send
* 功能 : 请求获得 通过CAN 发送数据到外设 的资源.
*
* 输入 : dataAddr : 发送的数据地址;
*        dataLen  : 发送的数据长度;
*        type     : 发送类型;  0:data[0]必须小于等于7;   1(非0):data[0]可以为任意数;
* 输出 : err      : 0:成功; 1:资源(缓存)不足; 2:数据长度错误;
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.如果采用 占先式系统调度方式,则在调用此函数前后要 开/关 中断.
********************************************************************/
u8 CanBus_Send(u8 *dataAddr, u32 dataLen)
{
	assert_param((dataAddr != NULL));

	//启动发送CAN控制数据,最长的数据不超过300字节
	if((dataLen == 0) || (dataLen > CAN_MAX_SEND_LEN*13))
	{
		return(2);
	}
	if(gCanIsrData.sendLen != 0)	//还有数据正在发送
	{
		return(1);
	}

	memcpy((u8*)&gCanIsrData.sendBuffer[0][0], dataAddr, dataLen);
	//消息都是按13个字节为单位发送的.
	gCanIsrData.sendLen   = (dataLen+12)/13;//编译器采用"去尾法",所以,发送消息有(sendLen)条,索引[0..(sendLen-1)].
	gCanIsrData.sendIndex = 0;

	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE); //打开发送中断  //((u32)0x00000001)
	Can_Isr_Tx();

	return(0);
}
/********************************************************************
* 名称 : CanBus_Send
* 功能 : 请求获得 通过CAN 发送数据到外设 的资源.
*
* 输入 : dataAddr : 发送的数据地址;
*        dataLen  : 发送的数据长度;
*        type     : 发送类型;  0:data[0]必须小于等于7;   1(非0):data[0]可以为任意数;
* 输出 : err      : 0:成功; 1:资源(缓存)不足; 2:数据长度错误;
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.如果采用 占先式系统调度方式,则在调用此函数前后要 开/关 中断.
********************************************************************/
u8 CanBus_Send2(u8 *dataAddr, u32 dataLen)
{
	assert_param((dataAddr != NULL));

	//启动发送CAN控制数据,最长的数据不超过300字节
	if((dataLen == 0) || (dataLen > CAN_MAX_SEND_LEN*13))
	{
		return(2);
	}
	if(gCanIsrData.sendLen != 0)	//还有数据正在发送
	{
		return(1);
	}

	memcpy((u8*)&gCanIsrData.sendBuffer[0][0], dataAddr, dataLen);
	//消息都是按13个字节为单位发送的.
	gCanIsrData.sendLen   = (dataLen+12)/13;//编译器采用"去尾法",所以,发送消息有(sendLen)条,索引[0..(sendLen-1)].
	gCanIsrData.sendIndex = 0;

	CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE); //打开发送中断  //((u32)0x00000001)
	Can_Isr_Tx2();

	return(0);
}

/********************************************************************
* 名称 : CanBus_Rece
* 功能 : 请求获得 从CAN总线接收到的数据.
*
* 输入 : dataAddr : 任务(taskId)用于装数据的地址;
*        maxLen   : 任务(taskId)用于装数据的最大空间;
*        err      : 0:成功; 1:接收时缓冲溢出; 2:没有收到新的数据;
* 输出 : receLen  : 收到的数据包个数;
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.如果采用 占先式系统调度方式,则在调用此函数前后要 开/关 中断.
********************************************************************/
u8 CanBus_GetData(u8 *dataAddr)
{
	u8   readindex;
	u16  isrLen;

	assert_param((dataAddr != NULL));
	assert_param((maxLen > 0));

	isrLen = gCanIsrData.receNum;
	readindex = gCanIsrData.UnreadIndex;
	while(gCanIsrData.receNum > 0)
	{
		memcpy(dataAddr, (u8*)&gCanIsrData.receBuffer[readindex][0], 13);//readindex, cpyLen,
		dataAddr += 13;
		if(readindex >= CAN_MAX_RECE_NUM)
			readindex = 0;
		gCanIsrData.receNum--;
	}

	gCanIsrData.UnreadIndex = 0;
	gCanIsrData.receIndex = 0;
	gCanIsrData.receNum = 0;

	return(isrLen);
}

/********************************************************************
* 名称 : CANBus_Init
* 功能 : 根据 配置参数, 初始化CAN总线处理的一些参数.
*
* 输入 : 无
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.打开的中断类型还要修改，要包含错误信息的中断???
********************************************************************/
void CANBus_Init(void)
{
	/* CAN register init */
	CAN_DeInit(CAN1);

	CanBus_SetBaudRate(250000L);//设置CAN总线的 波特率(250KHz).

	CanBus_SetIdentifie(0x00, 0x00);//设置CAN总线的 滤波掩码.

	gCanIsrData.UnreadIndex = 0;
	gCanIsrData.receIndex = 0;
	gCanIsrData.receNum = 0;
}
/********************************************************************
* 名称 : CANBus_Init
* 功能 : 根据 配置参数, 初始化CAN总线处理的一些参数.
*
* 输入 : 无
* 输出 : 无
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.打开的中断类型还要修改，要包含错误信息的中断???
********************************************************************/
void CANBus_Init2(void)
{
	/* CAN register init */
	CAN_DeInit(CAN2);

	CanBus_SetBaudRate2(250000L);//设置CAN总线的 波特率(250KHz).

	CanBus_SetIdentifie(0x00, 0x00);//设置CAN总线的 滤波掩码.

	gCanIsrData.UnreadIndex = 0;
	gCanIsrData.receIndex = 0;
	gCanIsrData.receNum = 0;
}

/********************************************************************
* 名称 : CanBus_SetBaudRate
* 功能 : 设置CAN总线的 波特率.
*
* 输入 : BandRateBuf  : 设置的 波特率.(Hz)
* 输出 : 无.
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.
********************************************************************/
void CanBus_SetBaudRate(u32 BandRateBuf)
{
	CAN_InitTypeDef        CAN_InitStructure;

	CAN_StructInit(&CAN_InitStructure);
	 //------- CAN_MCR ------------
	CAN_InitStructure.CAN_TTCM = DISABLE; //使能/失能 时间触发通讯模式.0:时间触发通信模式关闭;
	CAN_InitStructure.CAN_ABOM = DISABLE;  //使能/失能 自动离线管理.    1:一旦监控到128次11个连续隐形位,自动退出离线状态;
	CAN_InitStructure.CAN_AWUM = ENABLE;  //使能/失能 自动唤醒模式.    1:硬件检测到CAN报文时自动离开休眠模式;
	CAN_InitStructure.CAN_NART = ENABLE; //使能/失能 非自动重传输模式.0:CAN硬件发送失败后会一直重发直到发送成功;
	CAN_InitStructure.CAN_RFLM = DISABLE; //使能/失能 能接收FIFO锁定模式.0:接收FIFO满了,下一条传入的报文将覆盖前一条;
	CAN_InitStructure.CAN_TXFP = ENABLE;  //使能/失能 发送FIFO优先级.    1:由发送请求的顺序(时间先后顺序)来决定优先级.
	 //------- CAN_BTR ------------
	CAN_InitStructure.CAN_Mode =CAN_Mode_Normal ;//CAN硬件工作在正常模式,CAN_Mode_LoopBack,CAN_Mode_Normal
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;     //重新同步跳跃宽度1个时间单位

	//CAN波特率: PCLK1 /  [(1 + CAN_BS1 + CAN_BS2) * CAN_Prescaler]
	//  Ki = (1 + CAN_BS1) / (1 + CAN_BS1 + CAN_BS2)
	//另外尽可能的把采样点设置为 CiA 推荐的值：
	//                           Ki=75%     when 波特率 > 800K
	//                           Ki=80%     when 波特率 > 500K
	//                           Ki=87.5%   when 波特率 <= 500K
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;   //CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 9;//(波特率:250K, PCLK1:36MHz, CAN_BS1:CAN_BS1_13tq, CAN_BS2:CAN_BS2_2tq, CAN_Prescaler:9)

//	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
//	if(BandRateBuf <= 500000L)         //BandRateBuf <= 500KHz
//	{
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/16;
//	} else if(BandRateBuf <= 800000L){ //500KHz < BandRateBuf <= 800KHz
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/10;
//	} else {                           //800KHz < BandRateBuf
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/8;
//	}

	CAN_Init(CAN1, &CAN_InitStructure);
}
/********************************************************************
* 名称 : CanBus_SetBaudRate
* 功能 : 设置CAN总线的 波特率.
*
* 输入 : BandRateBuf  : 设置的 波特率.(Hz)
* 输出 : 无.
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
*       1.
********************************************************************/
void CanBus_SetBaudRate2(u32 BandRateBuf)
{
	CAN_InitTypeDef        CAN_InitStructure;

	CAN_StructInit(&CAN_InitStructure);
	 //------- CAN_MCR ------------
	CAN_InitStructure.CAN_TTCM = DISABLE; //使能/失能 时间触发通讯模式.0:时间触发通信模式关闭;
	CAN_InitStructure.CAN_ABOM = DISABLE;  //使能/失能 自动离线管理.    1:一旦监控到128次11个连续隐形位,自动退出离线状态;
	CAN_InitStructure.CAN_AWUM = ENABLE;  //使能/失能 自动唤醒模式.    1:硬件检测到CAN报文时自动离开休眠模式;
	CAN_InitStructure.CAN_NART = ENABLE; //使能/失能 非自动重传输模式.0:CAN硬件发送失败后会一直重发直到发送成功;
	CAN_InitStructure.CAN_RFLM = DISABLE; //使能/失能 能接收FIFO锁定模式.0:接收FIFO满了,下一条传入的报文将覆盖前一条;
	CAN_InitStructure.CAN_TXFP = ENABLE;  //使能/失能 发送FIFO优先级.    1:由发送请求的顺序(时间先后顺序)来决定优先级.
	 //------- CAN_BTR ------------
	CAN_InitStructure.CAN_Mode =CAN_Mode_Normal ;//CAN硬件工作在正常模式,CAN_Mode_LoopBack,CAN_Mode_Normal
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;     //重新同步跳跃宽度1个时间单位

	//CAN波特率: PCLK1 /  [(1 + CAN_BS1 + CAN_BS2) * CAN_Prescaler]
	//  Ki = (1 + CAN_BS1) / (1 + CAN_BS1 + CAN_BS2)
	//另外尽可能的把采样点设置为 CiA 推荐的值：
	//                           Ki=75%     when 波特率 > 800K
	//                           Ki=80%     when 波特率 > 500K
	//                           Ki=87.5%   when 波特率 <= 500K
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;   //CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;    //CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 9;//(波特率:250K, PCLK1:36MHz, CAN_BS1:CAN_BS1_13tq, CAN_BS2:CAN_BS2_2tq, CAN_Prescaler:9)

//	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
//	if(BandRateBuf <= 500000L)         //BandRateBuf <= 500KHz
//	{
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/16;
//	} else if(BandRateBuf <= 800000L){ //500KHz < BandRateBuf <= 800KHz
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/10;
//	} else {                           //800KHz < BandRateBuf
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
//		CAN_InitStructure.CAN_Prescaler = (RCC_ClocksStatus.PCLK1_Frequency/BandRateBuf)/8;
//	}

	CAN_Init(CAN2, &CAN_InitStructure);
}
/********************************************************************
* 名称 : CanBus_SetIdentifie
* 功能 : 设置CAN总线的 滤波掩码.
*
* 输入 : filterId     : 设置 过滤器标识符
*        filterMaskId : 设置 过滤器屏蔽标识符
* 输出 : 无.
*
* 全局变量:
* 调用函数:
*
* 备注: (修改记录内容、时间)
********************************************************************/
void CanBus_SetIdentifie(u32 filterId, u32 filterMaskId)
{
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

