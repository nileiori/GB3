/************************************************************************
//程序名称：VDR_Card.c
//功能：刷卡模块
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.8
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "stm32f10x.h"
#include "VDR.h"



/********************本地变量*************************/
static	u8	CardDataBuffer[VDR_CARD_BUFFER_SIZE] = {0};//读取的IC卡内容放在该缓冲
static	u8	CardState = 0;//卡状态
static	u8	CardReadStep = 0;//读卡步骤

/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static void VDRCard_Init(void);
static void VDRCard_StateMonitor(void);
static void VDRCard_StateRead(void);
static void VDRCard_StateLogin(void);
static void VDRCard_StateLogout(void);
static u8 VDRCard_IsInsert(void);
static u8 VDRCard_GetLicense(u8 *pLicense, u8 *pCard);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRCard_TimeTask
//功能：刷卡签到、拔卡签退
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：0.1秒钟进入1次，任务调度器需要调用此函数，
//注意该任务需要在超时驾驶定时任务完成初始化后才能开启
***************************************************************************/
FunctionalState VDRCard_TimeTask(void)
{
	static	u8	InitFlag = 0;//初始化标志，1为已初始化，0为未初始化

	if(0 == InitFlag)
	{
		if(1 == VDROvertime_GetInitFlag())
		{
			InitFlag = 1;
			VDRCard_Init();
			CardState = CARD_STATE_MONITOR;
		}
	}
	else
	{
		switch(CardState)
		{
			case CARD_STATE_MONITOR://当监测到有卡插入时，状态转为读卡；监测到有卡拔出时状态转为签退
			{
				VDRCard_StateMonitor();
				break;
			}
			case CARD_STATE_READ://读卡成功后，状态转为登录，失败后转为监测
			{
				VDRCard_StateRead();
				break;
			}
			case CARD_STATE_LOGIN://完成登录后状态转为监测
			{
				VDRCard_StateLogin();
				break;
			}
			case CARD_STATE_LOGOUT://完成登录后状态转为监测
			{
				VDRCard_StateLogout();
				break;
			}
			default:
			{
				CardState = CARD_STATE_MONITOR;
				break;
			}
		}

	}

	return ENABLE;		
}

/////////////////////////////以下是内部函数///////////////////////// 
/**************************************************************************
//函数名：VDRCard_Init
//功能：模块初始化
//输入：无
//输出：无
//返回值：1:初始化完成;0:初始化未完成
//备注：
***************************************************************************/
static void VDRCard_Init(void)
{
	ICCARD_M3_Init();
}
/**************************************************************************
//函数名：VDRCard_StateMonitor
//功能：读卡
//输入：无
//输出：无
//返回值：无
//备注：
//1.停车时监测到有卡插入时，跳转至读卡状态
//2.停车时监测到有卡拔出时，跳转至签退状态
//3.行驶变停驶时，有卡则跳转至读卡，无卡则跳转至签退。
//4.行驶中插卡或拔卡无效
***************************************************************************/
static void VDRCard_StateMonitor(void)
{
	static	u8	LastCardInsertFlag = 0;//插卡标志，1为已插卡，0为未插卡
	static	u8	LastCarRunFlag = 0;//行驶标志，1为行驶，0为停驶

	u8	CardInsertFlag = 0;
	u8	CarRunFlag = 0;

	CarRunFlag = VDRDoubt_GetCarRunFlag();
	CardInsertFlag = VDRCard_IsInsert();

	if((1 == CardInsertFlag)&&(0 == LastCardInsertFlag)&&(0 == CarRunFlag))//停车插卡
	{
		CardState = CARD_STATE_READ;
	}
	else if((0 == CardInsertFlag)&&(1 == LastCardInsertFlag)&&(0 == CarRunFlag))//停车拔卡
	{
		CardState = CARD_STATE_LOGOUT;
	}

	if((1 == LastCarRunFlag)&&(0 == CarRunFlag)&&(1 == CardInsertFlag))//由行驶变停驶时有卡
	{
		CardState = CARD_STATE_READ;
	}
	else if((1 == LastCarRunFlag)&&(0 == CarRunFlag)&&(0 == CardInsertFlag))//由行驶变停驶时无卡
	{
		CardState = CARD_STATE_LOGOUT;
	}

	LastCarRunFlag = CarRunFlag;
	LastCardInsertFlag = CardInsertFlag;
}
/**************************************************************************
//函数名：VDRCard_StateRead
//功能：读卡
//输入：无
//输出：无
//返回值：无
//备注：0.1秒钟进入1次，先判断是否为24XX卡，然后再判断是否为4442卡，
//读取的内容放在CardDataBuffer缓冲中，CardDataBuffer[0]表示读写结果，1为成功，0是失败，后面的内容为读取的内容
***************************************************************************/
static void VDRCard_StateRead(void)
{
	static	u8	count = 0;
	u8 Addr;
	u8	Len;
	
	switch(CardReadStep)
	{
		case	CARD_READ_START:
		{
			count = 0;
			CardReadStep++;
			break;
		}
		case	CARD_READ_24XX:
		{
			count++;
			if(count >= 5)
			{
				CardDataBuffer[0] = 0;
				Addr = 0;
				Len = 128;//国标卡长度
				if(0 == EEPROM_24XX_HL_Buf_Read(Addr, CardDataBuffer+1,Len))
				{
					CardReadStep++;
				}
				else
				{
					CardDataBuffer[0] = 1;
					CardState = CARD_STATE_LOGIN;
				}
				count = 0;
			}
			break;
		}
		case	CARD_READ_4442:
		{
			count++;
			if(count >= 5)
			{
				Len = 128;//国标卡长度
				if(0 == SL4442_Buffer_Read(CardDataBuffer+1,Len))
				{
					CardReadStep++;
				}
				else
				{
					CardDataBuffer[0] = 1;
					CardState = CARD_STATE_LOGIN;
				}
				count = 0;
			}
			break;
		}
		default: 
		{
			CardState = CARD_STATE_MONITOR;
			break;
		}
	}
}
/**************************************************************************
//函数名：VDRCard_StateLogin
//功能：IC卡签到
//输入：无
//输出：无
//返回值：无
//备注：0.1秒钟进入1次
//CardDataBuffer[0]表示读写结果，1为成功，0是失败，后面的内容为读取的内容
***************************************************************************/
static void VDRCard_StateLogin(void)
{
	u8	Buffer[20];
	u8	BufferLen;
	
	if(1 == CardDataBuffer[0])
	{
		BufferLen = VDRCard_GetLicense(Buffer, CardDataBuffer+1);
		if(18 == BufferLen)
		{
			VDROvertime_DriverLogin(Buffer,18);
		}
		
		CardDataBuffer[0] = 0;
	}
	CardState = CARD_STATE_MONITOR;
}
/**************************************************************************
//函数名：VDRCard_StateLogout
//功能：IC卡签退
//输入：无
//输出：无
//返回值：无
//备注：0.1秒钟进入1次
//CardDataBuffer[0]表示读写结果，1为成功，0是失败，后面的内容为读取的内容
***************************************************************************/
static void VDRCard_StateLogout(void)
{
	VDROvertime_DriverLogout();
	CardState = CARD_STATE_MONITOR;
}


static u8 VDRCard_IsInsert(void)
{
	static u8	InCount = 0;
	static u8	OutCount = 0;
	
	static u8	InsertFlag = 0;
	
	if(ICCARD_DETECT_READ())//无卡
  {
		InCount = 0;
		OutCount++;
		if(OutCount >= 10)
		{
			OutCount--;
			InsertFlag = 0;
		}
  }
  else//有卡
  {
    OutCount = 0;
		InCount++;
		if(InCount >= 10)
		{
			InCount--;
			InsertFlag = 1;
		} 
  }
	
	return InsertFlag;
}
/**************************************************************************
//函数名：VDRCard_GetLicense
//功能：从卡片数据中获取驾驶证号码
//输入：pCard:指向读取到的卡片数据
//输出：pLicense指向获取到的驾驶证数据
//返回值：0:表示未获取到合法的驾驶证号码;18表示获取到合法的驾驶证号码，长度为18
//备注：可兼容行标格式卡和国标格式卡
***************************************************************************/
static u8 VDRCard_GetLicense(u8 *pLicense, u8 *pCard)
{
	u8	*p = NULL;
	u8	i;
	u8	temp;
	u8	flag;
	
	p = pCard;
	
	//if(*(p+127) == Public_GetXorVerify(p,127))//4442卡没有加校验，因此去掉
	//{
		
		//先判断是否为国标卡
		flag = 0;
		p = pCard+32;//检查驾驶证号码
		for(i=0; i<18; i++)
		{
			temp = *p++;
			if(((temp >= '0')&&(temp <= '9'))
				||('x'==temp)
			    ||('X' == temp)
						||(' ' == temp)
							||(0 == temp))
			{

			}
			else
			{
					flag = 1;
					break;
			}
		}
		if(0 == flag)//检查驾驶证有效期
		{
			p = pCard+50;
			temp = *p++;
			if(temp <= 0x99)
			{
				
			}
			else
			{
				flag = 1;
			}
			temp = *p++;
			if((temp >= 0x01)&&(temp <= 0x12))
			{
				
			}
			else
			{
				flag = 1;
			}
			temp = *p++;
			if((temp >= 0x01)&&(temp <= 0x31))
			{
				
			}
			else
			{
				flag = 1;
			}
		}
		if(0 == flag)//是国标卡
		{
			p = pCard+32;
			memcpy(pLicense,p,18);
			for(i=0; i<18; i++)
			{
				if(0 == pLicense[i])
				{
					//pLicense[i] = ' ';
				}
			}
			return 18;
		}
		
	//}

	//再判断是否为行标卡
	p = pCard;
	temp = *p++;
	//if(0x00==(temp&0x0f))//卡片类型为0，表明是驾驶员身份卡
	//{
		temp = *p++;//驾驶员姓名长度，1字节
		p += temp;//跳过驾驶员姓名
		for(i=0; i<18; i++)
		{
			temp = *p++;
			if(((temp >= '0')&&(temp <= '9'))
				||('x'==temp)
					||('X' == temp)
						||(' ' == temp)
							||(0 == temp))
			{

			}
			else
			{
				return 0;
			}
		}
		p -= 18;
		memcpy(pLicense,p,18);
		for(i=0; i<18; i++)
		{
			if(0 == pLicense[i])
			{
					//pLicense[i] = ' ';
			}
		}
	//}
	//else
	//{
		//return 0;
	//}
	
	return 18;
}






