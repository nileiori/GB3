/*
********************************************************************************
*
* Filename      : A2_OBD.h
* Version       : V1.00
* Programmer(s) : zengliang
* @date         : 2017-9-6
* 
********************************************************************************
*/


/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#include "A2_OBD.h"

/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/

/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/

/*
********************************************************************************
*                         LOCAL DATA TYPES
********************************************************************************
*/
static u8 hardRply=0;//读取A2型OBD模块版本应答标志
static s16 extendSeril=-1;//扩展流水号
static u8 plateFlag=0;//平台是否放过请求指令，1为发送过
static u8 A2OBD_RadioTxBuffer[500];//解析之后缓冲
static u8 A2OBD_Serial=0;
static u8 A2OBD_TxBuffer[100];//A2OBD发送缓冲
static u16 A2OBD_AlarmFlag=0;//报警标志
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
A2OBD_StreamData A2OBD_Stream={0};
A2OBD_RouteData A2OBD_Route={0};
u8 A2OBD_DATA_FLAG=0;
/*
********************************************************************************
*                          EXTERN VARIABLES
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
void A2OBD_Protocol_Paramter(u8 cmd,u8 *pBuffer,u8 datalen)
{
	A2OBD_DATA_FLAG=1;
  switch(cmd)
  {
    case 0x10:
				{
					//A2OBD_SentCmd(0x0200,NULL,0);break;//A2OBD读取硬件版本信息
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=1;
				}
				break;
    case 0x11:
				{
				//A2OBD_SentCmd(0x0201,NULL,0);break;//A2OBD读取软件版本信息
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=2;
				}
				break;
    case 0x12:
				{
				//A2OBD_SentCmd(0x0300,NULL,0);break;//A2OBD模块复位
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=3;
				}
				break;
    case 0x13:
				{		//A2OBD_SentCmd(0x0500,NULL,0);break;//开启A2OBD实时数据流
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=4;
				}
				break;
    case 0x14:
				{
					//A2OBD_SentCmd(0x0501,NULL,0);break;//关闭A2OBD实时数据流
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=5;
				}
				break;
    case 0x15:
				{
					//A2OBD_SentCmd(0x0701,pBuffer,datalen-2);break;//A2OBD参数查询   
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=6;
				}
				break;
    case 0x16:
				{
					//A2OBD_SentCmd(0x0700,pBuffer,datalen-2);break;//A2OBD参数设置
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=7;
				}
				break;
    case 0x19:
				{//A2OBD_SentCmd(0x0A00,NULL,0);break;//A2OBD行程结束
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=8;
				}
				break;
    case 0x1A:
				{
			//A2OBD_SentCmd(0x0A01,NULL,0);break;//A2OBD行程开始
					extendSeril=(((u16)pBuffer[0])<<8)|pBuffer[1];
					COM2_WriteBuff(pBuffer+2,datalen-2);
					plateFlag=9;
				}
				break;
		default :break;
  }
}


//读A2OBD报警位
u8 A2OBD_ReadAlarmBit(u32 bit)
{
	if(bit >= 32)return 0;
    return PUBLIC_CHECKBIT_EX(A2OBD_AlarmFlag, bit);
}
//写A2OBD报警位
void A2OBD_WriteAlarmBit(u32 bit, FlagStatus state)
{
	if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(A2OBD_AlarmFlag, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(A2OBD_AlarmFlag, bit);
    }
}
//三急扩展标志位，急加速，急减速，急转弯，强刹车
u16  ThreeEmerInfo(u8 *pDst)
{
    u16 Alarm;
    Alarm = A2OBD_AlarmFlag;
    if(Alarm == 0)
    {
        return 0;
    }
    *pDst++ = 0xE9;//三急ID
    *pDst++ = 2;//长度
		*pDst++ = Alarm>>8;
		*pDst++ = Alarm;
		A2OBD_AlarmFlag=0;//上报后立即清除
    return (4);
}
//透传数据到平台
u8 A2OBD_RadioProtocolOriginalDataUpTrans(s16 seril,u8* a2obd_data,u8 cmd,u16 len)
{
			ProtocolACK ack;
      A2OBD_RadioTxBuffer[0]=0x01;
      A2OBD_RadioTxBuffer[1]=0x01;
      A2OBD_RadioTxBuffer[2]=0xF5;
      A2OBD_RadioTxBuffer[3]=cmd; 
			if(seril==-1)
			{
				A2OBD_RadioTxBuffer[4]=len>>8; 
				A2OBD_RadioTxBuffer[5]=len; 
				memcpy(A2OBD_RadioTxBuffer+6,a2obd_data,len);
				ack = RadioProtocol_OriginalDataUpTrans(CHANNEL_DATA_1|CHANNEL_DATA_2,0xFD, A2OBD_RadioTxBuffer, len+6);
			}
			else
			{
				len=len+2;
				A2OBD_RadioTxBuffer[4]=len>>8; //加上扩展指令流水号两个字节的长度
				A2OBD_RadioTxBuffer[5]=len; 
				A2OBD_RadioTxBuffer[6]=seril>>8; 
				A2OBD_RadioTxBuffer[7]=seril; 
				memcpy(A2OBD_RadioTxBuffer+8,a2obd_data,len-2);
				ack = RadioProtocol_OriginalDataUpTrans(CHANNEL_DATA_1|CHANNEL_DATA_2,0xFD, A2OBD_RadioTxBuffer, len+8);
			}
			if(ACK_OK == ack)
			{
				return 1;
			}
			else
			{
				return 0;
			}
}
//发送A2OBD数据
void A2OBD_SentCmd(u16 cmd,u8 *pBuffer,u16 datalen)
{
	u16 packagelen=0;
	
	A2OBD_Serial++;//包序号
	packagelen=datalen+3;//包长度	
	
	A2OBD_TxBuffer[0]=0X96;
	A2OBD_TxBuffer[1]=0X69;
	A2OBD_TxBuffer[2]=packagelen>>8;
	A2OBD_TxBuffer[3]=packagelen;
	A2OBD_TxBuffer[4]=A2OBD_Serial;
  A2OBD_TxBuffer[5]=cmd>>8;
  A2OBD_TxBuffer[6]=cmd;

	memcpy(A2OBD_TxBuffer+7,pBuffer,datalen);
	A2OBD_TxBuffer[datalen+7]=Public_GetXorVerify(A2OBD_TxBuffer+2,packagelen+2);
	COM2_WriteBuff(A2OBD_TxBuffer,8+datalen);
}
//A2OBD报警标志
void A2OBD_Alarm_Proce(u8 *buff,u8 len)
{
	u8 *p=buff+5;
	u8 i;
	u8 data_id=0;
	u8 tmp=0;
	for(i=0;i<len-8;i=i+2)
	{
		data_id=*(p+i+2);
		//tmp=*(p+i+1);	
		switch(data_id)
		{
			case 0x01:
				A2OBD_WriteAlarmBit(0,SET);tmp=1;break;//急踩油门，急加速
			case 0x02:
					A2OBD_WriteAlarmBit(1,SET);tmp=1;break;//急踩刹车，急减速
			case 0x0F:
					A2OBD_WriteAlarmBit(2,SET);tmp=1;break;//急转向，急转弯
			default:
			break;
		}
	}
	if(tmp==1)
	{
		 tmp= Report_GetPositionInfo(A2OBD_RadioTxBuffer);
	   RadioProtocol_PostionInformationReport(CHANNEL_DATA_1|CHANNEL_DATA_2,A2OBD_RadioTxBuffer,tmp);
			tmp=0;
	}
	else
	{
		A2OBD_RadioProtocolOriginalDataUpTrans(-1,buff,0x18,len);//不是三急直接透传上报
	}
}
void A2OBD_Data(u8 *buff,u8 len)
{
	u16 cmd;
	u8 tmp=0;
	cmd=*(buff+5);
	cmd=(*(buff+6))|cmd<<8;
	switch(cmd)
	{
		case A2OBD_HARD_VERSION:
		{
			if(plateFlag==1)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x10,len);
			}
			else
			{
				hardRply=1;
			}
		}
		break;
		case A2OBD_SOFTE_VERSION:
		{
			if(plateFlag==2)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x11,len);
			}
		}
		break;
		case A2OBD_RESET:
		{
			if(plateFlag==3)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x12,len);
			}
		}
		break;
		case A2OBD_ON_STREAM_DATA:
		{
			if(plateFlag==4)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x13,len);
			}
		}
		break;
		case A2OBD_OFF_STREAM_DATA:
		{
			if(plateFlag==5)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x14,len);
			}
		}
		break;
		case A2OBD_PRA_SET:
		{
			if(plateFlag==7)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x16,len);
			}
		}
		break;
		case A2OBD_PRA_READ:
		{
			if(plateFlag==6)
			{
				plateFlag=0;
				A2OBD_RadioProtocolOriginalDataUpTrans(extendSeril,buff,0x15,len);
			}
		}
		break;
		case A2OBD_STREM_DATA://实时数据流
		{
			hardRply=1;
			//A2OBD_Stream_Proce(buff+2,len);
			//A2OBD_RadioProtocolOriginalDataUpTrans(buff+2,0x17,len);
		}
		break;
		///////////////////
		case A2OBD_ALARM://报警输出
		{
			A2OBD_Alarm_Proce(buff,len);
		}
		break;
		////////////////////
		case A2OBD_ROUTE_END://行程数据
		{
			//A2OBD_Route_Proce(buff+2,len);
			if(A2OBD_RadioProtocolOriginalDataUpTrans(-1,buff,0x19,len)==1)
			{
				tmp=0;
				A2OBD_SentCmd(0x0A00,&tmp,1);//应答A2OBD
			}
			else
			{
				tmp=1;
				A2OBD_SentCmd(0x0A00,&tmp,1);//应答A2OBD
			}
		}
		break;
		///////////////////
		case A2OBD_ROUTE_BEGIN://行程开始
		{
			tmp=0;
			A2OBD_SentCmd(0x0A01,&tmp,1);//应答A2OBD
		}
		break;		
		
		default:break;
	}
}


//版权说明	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称	:A2_OBD.c	
//功能		:查找有效的A2_OBD模块数据
//版本号	:
//开发人	:liamtsen
//开发时间：2017-09-06
//修改者	:
//修改时间	:
//修改简要说明	:
//备注		:
void A2OBD_UartDataProtocolParse(unsigned char *pBuffer,unsigned short Len)
{
    u8 *Ptr;     //指向缓存区
    u16 i; 
    u8 ChkSum=0;  
    u8 obd_len=0;
    Ptr=pBuffer;  
   
    for(i=0; i<Len-6; i++)
    {
//				Ptr=Ptr+i;
        if(*(Ptr+i)==0x96&&*(Ptr+i+1)==0x69)
        {
					obd_len=*(Ptr+2+i);
					obd_len=(*(Ptr+3+i))|(obd_len<<8);
					ChkSum=Public_GetXorVerify(Ptr+2+i,obd_len+2);
           if(ChkSum==*(Ptr+obd_len+4+i))
           {
							A2OBD_Data(Ptr+i,obd_len+5);//传送命令字以及数据段，传入数据长度为数据段数据长度
           }
						i=i+obd_len+4;
        }
    }
}


/**************************************************************************
//函数名：Vss_TimeTask
//功能：VSS连接异常判断
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：VSS连接异常定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState A2OBD_TimeTask(void)
{
	static u8 cnt=0;
	static u8 errtime=0;
	if(!ReadPeripheral2TypeBit(6))
	{
		return ENABLE;//未接A2型OBD模块
	}
	else if(Io_ReadStatusBit(STATUS_BIT_ACC)==1)
	{
		cnt++;
	}
	else
	{
		cnt=0;
	}
	if(cnt%20==0)
	{
		A2OBD_SentCmd(0x0200,0,0);//读取硬件版本
	}
	if(cnt%30==0&&hardRply==0)
	{
		hardRply=0;
		errtime++;
		if(errtime==3)
		{
			Io_WriteSelfDefine2Bit(DEFINE_BIT_6, SET);
		}
	}
	else if(cnt%40==0&&hardRply==1)
	{
		hardRply=0;
		errtime=0;
		Io_WriteSelfDefine2Bit(DEFINE_BIT_6, RESET);
	}
	return ENABLE;
}
