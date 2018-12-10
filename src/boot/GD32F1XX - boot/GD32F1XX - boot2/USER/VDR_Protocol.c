/************************************************************************
//程序名称：VDR_Protocol.c
//功能：记录仪串口协议
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：
*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "VDR_Usart.h"
#include "stm32f10x_rtc.h"

/********************本地变量*************************/
static u8 VDRProtocolParseBuffer[VDR_PROTOCOL_PARSE_BUFFER_SIZE] = {0};
static u8 VDRProtocolSendBuffer[VDR_PROTOCOL_SEND_BUFFER_SIZE] = {0};

/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static u16 VDRProtocol_ParseCmd08H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd09H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd10H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd11H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd12H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd13H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd14H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd15H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

static u16 VDRProtocol_ParseCmd82H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd83H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd84H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdC2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdC3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdC4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

static u16 VDRProtocol_ParseCmdE0H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE1H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRProtocol_Parse
//功能：协议解析入口
//输入：需要解析的数据、长度
//输出：无
//返回值：0：解析正确；1：解析错误
//备注：串口接收到一帧数据后需调用此函数解析
***************************************************************************/
u8 VDRProtocol_Parse(u8 *pBuffer, u16 BufferLen)
{
	u16 i,j;
	u16 length;
	u16 SendLen;
	u8 Cmd;
	u8 VerifyCode;
	u8 *p = NULL;

	if(BufferLen < 6)
	{
		return 1;//长度错误
	}

	p = pBuffer;
	for(i=0; i<BufferLen-1; i++)
	{
		if((0xaa == *(p+i))&&(0x75 == *(p+i+1)))
		{
			Cmd = *(p+i+2);
			length = 0;
			length |= *(p+i+3) << 8;
			length |= *(p+i+4);
			VerifyCode = 0;
			for(j=i;j<i+6+length;j++)
			{
				VerifyCode = VerifyCode ^ *(p+j);
			}
			if(VerifyCode == *(p+j))
			{
				break;
			}
		}
	}

	if((BufferLen-1) == i)//未找到合格的帧
	{
		return 2;
	}
	else
	{
		if(length < VDR_PROTOCOL_PARSE_BUFFER_SIZE)
		{
			SendLen = VDRProtocol_ParseCmd(VDRProtocolParseBuffer,Cmd, p+i+6, length);
			VDRProtocol_SendCmd(Cmd,VDRProtocolParseBuffer,SendLen);
			return 0;
		}
		else
		{
			return 3;
		}
	}
	
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd
//功能：解析指令
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据），Cmd:命令字
//输出：无
//返回值：应答数据长度
//备注：VDRProtocol_Parse调用该函数，
//当返回值为0xFFFF时表示采集数据命令帧接收出错；当返回值为0xFFFE时表示设置参数命令帧接收出错；
//当返回值为0xFFFD时表示不是合法的命令字(Cmd不合法)；
***************************************************************************/
u16 VDRProtocol_ParseCmd(u8 *pDstBuffer, u8 Cmd, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u32 TimeCount;


	TimeCount = RTC_GetCounter();

	switch(Cmd)
	{
		///////////数据采集命令/////////////
		case VDR_PROTOCOL_CMD_00H:
		{
			length = VDRProtocol_ParseCmd00H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_01H:
		{
			length = VDRProtocol_ParseCmd01H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_02H:
		{
			length = VDRProtocol_ParseCmd02H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_03H:
		{
			length = VDRProtocol_ParseCmd03H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_04H:
		{
			length = VDRProtocol_ParseCmd04H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_05H:
		{
			length = VDRProtocol_ParseCmd05H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_06H:
		{
			length = VDRProtocol_ParseCmd06H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_07H:
		{
			length = VDRProtocol_ParseCmd07H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_08H:
		{
			length = VDRProtocol_ParseCmd08H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_09H:
		{
			length = VDRProtocol_ParseCmd09H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_10H:
		{
			length = VDRProtocol_ParseCmd10H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_11H:
		{
			length = VDRProtocol_ParseCmd11H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_12H:
		{
			length = VDRProtocol_ParseCmd12H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_13H:
		{
			length = VDRProtocol_ParseCmd13H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_14H:
		{
			length = VDRProtocol_ParseCmd14H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_15H:
		{
			length = VDRProtocol_ParseCmd15H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		///////////参数设置命令/////////////
		case VDR_PROTOCOL_CMD_82H:
		{
			length = VDRProtocol_ParseCmd82H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_83H:
		{
			length = VDRProtocol_ParseCmd83H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_84H:
		{
			length = VDRProtocol_ParseCmd84H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_C2H:
		{
			length = VDRProtocol_ParseCmdC2H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_C3H:
		{
			length = VDRProtocol_ParseCmdC3H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_C4H:
		{
			length = VDRProtocol_ParseCmdC4H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		///////////检定命令/////////////
		case VDR_PROTOCOL_CMD_E0H:
		{
			length = VDRProtocol_ParseCmdE0H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E1H:
		{
			length = VDRProtocol_ParseCmdE1H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E2H:
		{
			length = VDRProtocol_ParseCmdE2H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E3H:
		{
			length = VDRProtocol_ParseCmdE3H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E4H:
		{
			length = VDRProtocol_ParseCmdE4H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		
		default: 
		{
			length = 0xFFFD;
			break;
		}
	}

	return length;
}
/**************************************************************************
//函数名：VDRProtocol_SendCmd
//功能：发送指令
//输入：Cmd：指令，pBuffer：指向指令数据，BufferLen：指令数据长度
//输出：无
//返回值：0：发送成功；1：发送失败
//备注：VDRProtocol_Parse调用该函数，
//当长度BufferLen为0xFFFF时表示采集数据命令帧接收出错；
//当长度BufferLen为0xFFFE时表示设置参数命令帧接收出错；
//当长度BufferLen为0xFFFD时表示不是合法的命令字(Cmd不合法),不应答；
***************************************************************************/
u8 VDRProtocol_SendCmd(u8 Cmd, u8 *pBuffer, u16 BufferLen)
{
	u16 SendLen;
	u16 i;
	u8 VerifyCode;
	

	if(0xffff == BufferLen)
	{
		SendLen = 0;
		VDRProtocolSendBuffer[SendLen++] = 0x55;
		VDRProtocolSendBuffer[SendLen++] = 0x7a;
		VDRProtocolSendBuffer[SendLen++] = 0xfa;
		VDRProtocolSendBuffer[SendLen++] = 0x00;

		VerifyCode = 0;
		for(i=0; i<SendLen; i++)
		{
			VerifyCode = VerifyCode ^ VDRProtocolSendBuffer[i];
		}
		VDRProtocolSendBuffer[SendLen++] = VerifyCode;
		if(SendLen == VDRUsart_SendData(VDRProtocolSendBuffer,SendLen))
		{
			return 0;
		}
		else
		{
			return 1;
		}
		
	}
	else if(0xfffe == BufferLen)
	{
		SendLen = 0;
		VDRProtocolSendBuffer[SendLen++] = 0x55;
		VDRProtocolSendBuffer[SendLen++] = 0x7a;
		VDRProtocolSendBuffer[SendLen++] = 0xfb;
		VDRProtocolSendBuffer[SendLen++] = 0x00;
		VerifyCode = 0;
		for(i=0; i<SendLen; i++)
		{
			VerifyCode = VerifyCode ^ VDRProtocolSendBuffer[i];
		}
		VDRProtocolSendBuffer[SendLen++] = VerifyCode;
		if(SendLen == VDRUsart_SendData(VDRProtocolSendBuffer,SendLen))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if(0xfffd == BufferLen)
	{
		return 0;
	}
	else if(BufferLen < 1000)
	{
		SendLen = 0;
		VDRProtocolSendBuffer[SendLen++] = 0x55;
		VDRProtocolSendBuffer[SendLen++] = 0x7a;
		VDRProtocolSendBuffer[SendLen++] = Cmd;
		VDRProtocolSendBuffer[SendLen++] = (BufferLen&0xff00)>>8;
		VDRProtocolSendBuffer[SendLen++] = BufferLen&0xff;
		VDRProtocolSendBuffer[SendLen++] = 0x00;

		memcpy(VDRProtocolSendBuffer+SendLen,pBuffer,BufferLen);
		SendLen += BufferLen;

		VerifyCode = 0;
		for(i=0; i<SendLen; i++)
		{
			VerifyCode = VerifyCode ^ VDRProtocolSendBuffer[i];
		}
		VDRProtocolSendBuffer[SendLen++] = VerifyCode;

		if(SendLen == VDRUsart_SendData(VDRProtocolSendBuffer,SendLen))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else//长度超出
	{
		return 0;
	}
	
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd00H
//功能：00H指令应答，采集记录仪执行标准版本
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd00H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;

		*p++ = 0x12;//记录仪标准,2012年标准
		length++;
		
		*p++ = 0x00;//修改单号,默认为00H
		length++;

		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd01H
//功能：01H指令应答，采集当前驾驶人信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd01H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 i;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;

		if(0 == VDROvertime_GetLoginFlag())
		{
			for(i=0; i<18; i++)
			{
				*p++ = 0x00;//驾驶员没有登录,填充0x00
			}
			length = 18;
		}
		else
		{
			length = VDROvertime_GetLicense(p);
		}

		return length;
		
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd02H
//功能：02H指令应答，采集记录仪实时时间
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd02H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
	//TIME_T *tt;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);
		length = 6;
		return length;
		
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd03H
//功能：03H指令应答，采集累计行驶里程
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd03H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
	u8 i;
	u8 Buffer[5];
	u8 BufferLen;
	u32 temp;
	TIME_T tt;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;	
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);//实时时间
		p += 6;
		length += 6;
		BufferLen = EepromPram_ReadPram(E2_INSTALL_TIME_ID,Buffer);//安装时间
		if(0 == BufferLen)//安装时间读取失败,用0x00填充
		{
			for(i=0; i<6; i++)
			{
				*p++ = 0x00;
			}
			length += 6;
		}
		else
		{
			temp = Public_ConvertBufferToLong(Buffer);
			Gmtime(&tt, temp);
			VDRPub_ConvertTimeToBCD(p,&tt);
			p += 6;
			length += 6;
		}
		BufferLen = EepromPram_ReadPram(E2_CAR_INIT_MILE_ID,Buffer);//初始里程
		if(0 == BufferLen)//
		{
			for(i=0; i<4; i++)
			{
				*p++ = 0x00;
			}
			length += 4;	
		}
		else
		{
			temp = Public_ConvertBufferToLong(Buffer);
			Public_ConvertLongToBCD(temp,p);
			p += 4;
			length += 4;
		}

		BufferLen = FRAM_BufferRead(Buffer, FRAM_VDR_MILEAGE_LEN, FRAM_VDR_MILEAGE_ADDR);//累计里程
		if(FRAM_VDR_MILEAGE_LEN == BufferLen)
		{
			temp = Public_ConvertBufferToLong(Buffer);
		}
		else
		{
			temp = 0;
		}
		Public_ConvertLongToBCD(temp,p);
		p += 4;
		length += 4;

		return length;
		
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd04H
//功能：04H指令应答，采集记录仪脉冲系数
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd04H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
//	u8 i;
	u8 Buffer[5];
	u8 BufferLen;
//	u32 temp;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);//实时时间
		p += 6;
		length += 6;
		
		BufferLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, Buffer);//03版特征系数是3字节,12版为2字节
		if(2 == BufferLen)
		{
			memcpy(p,Buffer,BufferLen);
			p += 2;
			length += 2;
		}
		else
		{
			*p++ = 0x00;
			*p++ = 0x00;
			length += 2;
		}

		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd05H
//功能：05H指令应答，采集车辆信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd05H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
	u8 i;
	u8 Buffer[20];
	u8 BufferLen;
//	u32 temp;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;

		BufferLen = EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID,Buffer);//车辆识别代号
		if(17 == BufferLen)
		{
			memcpy(p,Buffer,BufferLen);
			p += 17;
			length += 17;
		}
		else
		{
			for(i=0; i<17; i++)
			{
				*p++ = ' ';//填充空格
			}
			length += 17;
		}

		BufferLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,Buffer);//车牌号码
		if((BufferLen > 12)||(0 == BufferLen))
		{
			for(i=0; i<12; i++)
			{
				*p++ = ' ';//填充空格
			}
			length += 12;
		}
		else
		{
			memcpy(p,Buffer,BufferLen);
			p += BufferLen;
			length += BufferLen;
			for(i=BufferLen; i<12; i++)
			{
				*p++ = ' ';//填充空格
				length++;
			}
		}

		BufferLen = EepromPram_ReadPram(E2_CAR_TYPE_ID,Buffer);//车牌分类
		if((BufferLen > 12)||(0 == BufferLen))
		{
			for(i=0; i<12; i++)
			{
				*p++ = ' ';//填充空格
			}
			length += 12;
		}
		else
		{
			memcpy(p,Buffer,BufferLen);
			p += BufferLen;
			length += BufferLen;
			for(i=BufferLen; i<12; i++)
			{
				*p++ = ' ';//填充空格
				length++;
			}
		}
		return length;
		
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd06H
//功能：06H指令应答，采集记录仪状态信号配置信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd06H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);//实时时间
		p += 6;
		length += 6;

		*p++ = 1;//状态信号字节个数,固定为1
		length++;
		
		SPI_FLASH_BufferRead(p,FLASH_VDR_STATUS_SET_SECTOR,80);//状态信号配置
		p += 80;
		length += 80;

		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd07H
//功能：07H指令应答，采集记录仪唯一性编号
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：
***************************************************************************/
u16 VDRProtocol_ParseCmd07H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		length += EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,p);

		return length;
	}
	
}

//////////////////////////////文件内部函数/////////////////////
/**************************************************************************
//函数名：VDRProtocol_ParseCmd08H
//功能：08H指令应答，采集指定的行驶速度记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd08H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_SPEED,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd09H
//功能：09H指令应答，采集指定的位置信息记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd09H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_POSITION,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd10H
//功能：10H指令应答，采集指定的事故疑点记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd10H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_DOUBT,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd11H
//功能：11H指令应答，采集指定的超时驾驶记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd11H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDROvertime_GetLog(p, EndTime);
		p += length;
		length += VDRData_Read(p,VDR_DATA_TYPE_OVER_TIME,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd12H
//功能：12H指令应答，采集指定的驾驶人身份记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd12H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_DRIVER,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd13H
//功能：13H指令应答，采集指定的外部供电记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd13H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_POWER,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd14H
//功能：14H指令应答，采集指定的参数修改记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd14H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_PARAMETER,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd15H
//功能：15H指令应答，采集指定的速度状态日志记录
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd15H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_SPEED_STATUS,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd82H
//功能：82H指令应答，设置车辆信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd82H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{

	u16 length;
	u8 *p = NULL;
	
	if(SrcBufferLen != 41)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		length = strlen((const char *)p);
		if(length > 17)
		{
			length = 17;
		}
		EepromPram_WritePram(E2_CAR_IDENTIFICATION_CODE_ID,p,length);
		p += 17;
		
		length = strlen((const char *)p);
		if(length > 17)
		{
			length = 17;
		}
		EepromPram_WritePram(E2_CAR_PLATE_NUM_ID, p, length);
		p += 12;

		length = strlen((const char *)p);
		if(length > 12)
		{
			length = 12;
		}
		EepromPram_WritePram(E2_CAR_TYPE_ID, p, length);
		p += 12;
		
		return 0;//应答帧为空
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd83H
//功能：83H指令应答，设置初次安装日期
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd83H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u32 TimeCount;
	u8 *p = NULL;
	u8 Buffer[5];
	TIME_T Time;
	
	if(SrcBufferLen != 6)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		if(1 == VDRPub_ConvertBCDToTime(&Time,p))
		{
			TimeCount = ConverseGmtime(&Time);
			Public_ConvertLongToBuffer(TimeCount,Buffer);
			EepromPram_WritePram(E2_INSTALL_TIME_ID, Buffer, 4); 
		}

		return 0;//应答帧为空
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmd84H
//功能：84H指令应答，设置状态量配置信息
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmd84H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	
	if((0 != SrcBufferLen%80)||(0 == SrcBufferLen))
	{
		return 0xfffe;
	}
	else 
	{
		p = pSrcBuffer;
		SPI_FLASH_SectorErase(FLASH_VDR_STATUS_SET_SECTOR);
	        SPI_FLASH_BufferWrite(p,FLASH_VDR_STATUS_SET_SECTOR,SrcBufferLen);

		return 0;//应答帧为空
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdC2H
//功能：C2H指令应答，设置记录仪时间
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdC2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	TIME_T Time;
	
	if(SrcBufferLen != 6)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		if(1 == VDRPub_ConvertBCDToTime(&Time,p))
		{
			RTC_SetTime(&Time);
		}

		return 0;//应答帧为空
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdC3H
//功能：C3H指令应答，设置脉冲系数
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdC3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	
	if(SrcBufferLen != 8)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		p += 6;//跳过6字节的实时时间
		EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID,p,2); 

		return 0;//应答帧为空
	}
	 
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdC4H
//功能：C4H指令应答，设置起始里程
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdC4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	u8 Buffer[5];
	u32 StartMile;
	
	if(SrcBufferLen != 20)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		p += 6;//跳过6字节的实时时间
		p += 6;//跳过6字节的初次安装时间
		StartMile = Public_ConvertBCDToLong(p);//设置起始里程
		Public_ConvertLongToBuffer(StartMile,Buffer);
		EepromPram_WritePram(E2_CAR_INIT_MILE_ID, Buffer, 4); 
		//设置累计行驶里程（每次设置起始里程时会自动把累计行驶里程也设为起始里程）
//		GpsMile_SetPram(StartMile);

		return 0;//应答帧为空
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdE0H
//功能：E0H指令应答
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdE0H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE0H();
		return 0;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdE1H
//功能：E1H指令应答
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdE1H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	u8 PramLen;
	u16 length;
	u16 Speed;
//	u16 CarPulseCoff;
	u32 TotalMile;

	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE1H();

		length = 0;
		p = pDstBuffer;

		PramLen = EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,p);//唯一性编号,35字节
		p += PramLen;
		length += PramLen;

		PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, p);//脉冲系数,2字节
		p += PramLen;
		length += PramLen;

		Speed = VDRPulse_GetSecondSpeed()*10;//当前速度,2字节
		*p++ = (Speed&0xff00) >> 8;
		*p++ = Speed&0xff;
		length += 2;

		TotalMile = VDRPulse_GetTotalMile()*10;//当前检定的累计行驶里程,4字节
		Public_ConvertLongToBuffer(TotalMile,p);
		p += 4;
		length += 4;
		
		return length;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdE2H
//功能：E2H指令应答
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdE2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE2H();
		return 0;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdE3H
//功能：E3H指令应答
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdE3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE3H();
		return 0;
	}
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdE4H
//功能：E4H指令应答
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdE4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE4H();
		return 0;
	}
}













