/************************************************************************
//程序名称：VDR_Protocol.c
//功能：记录仪串口协议
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：
*************************************************************************/

/********************文件包含*************************/
//#include "stm32f2xx_rtc.h"
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "VDR_Usart.h"
#include "MileMeter.h"
#include "RadioProtocol.h"

/********************本地变量*************************/
static u8 VDRProtocolParseBuffer[VDR_PROTOCOL_PARSE_BUFFER_SIZE] = {0};
static u8 VDRProtocolSendBuffer[VDR_PROTOCOL_SEND_BUFFER_SIZE] = {0};

/********************全局变量*************************/


/********************外部变量*************************/
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式

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

static u16 VDRProtocol_ParseCmdD0H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

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

    if((BufferLen-1) == i)
    {
        return 2;//未找到合格的帧
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
            return 3;//长度超出
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
                ///////////////参数读取或设置（伊爱自定义）////////////////
                case VDR_PROTOCOL_CMD_D0H:
                {
                        length = VDRProtocol_ParseCmdD0H(pDstBuffer,pSrcBuffer,SrcBufferLen);
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
//函数名：VDRProtocol_Packet
//功能：将需要发送的数据打包
//输入：Cmd：指令，pBuffer：指向需要发送的数据，BufferLen：数据长度
//输出：无
//返回值：打包后的长度
//备注：调用该函数后pBuffer里面的内容会被改写
***************************************************************************/
u16 VDRProtocol_Packet(u8 Cmd, u8 *pBuffer, u16 BufferLen)
{
  u8  *p = NULL;
    u8  VerifyCode;
    u16  i;
    u16  length;
    
    
    if(BufferLen > 1000)
    {
      return 0;
    }
    
    p = pBuffer;
    
    memmove(p+6,p,BufferLen);
    
    *p++ = 0x55;
    *p++ = 0x7a;
    *p++ = Cmd;
    *p++ = (BufferLen&0xff00) >> 8;
    *p++ = BufferLen&0xff;
    *p++ = 0xff;//保留字
    
    VerifyCode = 0;
    for(i=0; i<BufferLen; i++)
    {
        VerifyCode = VerifyCode ^ *p;
        p++;
    }
        
    *p = VerifyCode;
    
    length = BufferLen+7;
    
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
        VDRLog_Write(VDRProtocolSendBuffer, SendLen);
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
        VDRLog_Write(VDRProtocolSendBuffer, SendLen);
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
      VDRLog_Write(VDRProtocolSendBuffer, SendLen);
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

			  if(1 == BBGNTestFlag)//dxl,2016.6.1北斗功能检测时要求当前驾驶员与预置的行驶记录仪数据驾驶员一致
				{
						    memcpy(p,"410504198808125013",18);
							  length = 18;
			  }
				else
				{
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
            *p++ = 0x15;
          *p++ = 0x01;
          *p++ = 0x01;
          *p++ = 0x00;
          *p++ = 0x00;
          *p++ = 0x00;
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
      temp = temp/10;//化成单位为0.1km
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
    u8 Buffer[5];
    u8 BufferLen;

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
        
        BufferLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, Buffer);
        if(E2_CAR_FEATURE_COEF_LEN == BufferLen)
        {
            memcpy(p,Buffer+1,BufferLen-1);
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
                //*p++ = ' ';//填充空格
                *p++ = 0;
            }
            length += 17;
        }

        BufferLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,Buffer);//车牌号码
        if((BufferLen > 12)||(0 == BufferLen))
        {
            for(i=0; i<12; i++)
            {
                //*p++ = ' ';//填充空格
                *p++ = 0;
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
                //*p++ = ' ';//填充空格
                *p++ = 0;
                length++;
            }
        }

        BufferLen = EepromPram_ReadPram(E2_CAR_TYPE_ID,Buffer);//车牌分类
        if((BufferLen > 12)||(0 == BufferLen))
        {
            for(i=0; i<12; i++)
            {
                //*p++ = ' ';//填充空格
                *p++ = 0;
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
                //*p++ = ' ';//填充空格
                *p++ = 0;
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
        
        sFLASH_ReadBuffer(p,FLASH_VDR_STATUS_SET_ADDR,80);//状态信号配置
        if((0xff == *p)&&(0xff == *(p+1)))//表示没有写入过
        {
          memcpy(p,"自定义1   ",10);
            p += 10;
            memcpy(p,"自定义2   ",10);
            p += 10;
            memcpy(p,"自定义3   ",10);
            p += 10;
            memcpy(p,"近光      ",10);
            p += 10;
            memcpy(p,"远光      ",10);
            p += 10;
            memcpy(p,"右转向    ",10);
            p += 10;
            memcpy(p,"左转向    ",10);
            p += 10;
            memcpy(p,"制动      ",10);
            p += 10;
        }
        else
        {
          p += 80;
        }
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
        if(35 != length)//没有设置过唯一性编码
        {
          *p++ = 'C';//7字节,3C认证代码
            *p++ = '0';
            *p++ = '0';
            *p++ = '0';
            *p++ = '1';
            *p++ = '1';
            *p++ = '6';
            
            *p++ = 'E';//16字节,产品型号
            *p++ = 'G';
            *p++ = 'S';
            *p++ = '7';
            *p++ = '0';
            *p++ = '1';
            *p++ = 'G';
            *p++ = 'B';
            *p++ = '-';
            *p++ = 'I';
            *p++ = 'I';
            *p++ = 'I';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            *p++ = ' ';
            
            *p++ = 0x15;//3字节,生产日期
            *p++ = 0x09;
            *p++ = 0x23;
            
            *p++ = 0x00;//4字节,流水号
            *p++ = 0x00;
            *p++ = 0x00;
            *p++ = 0x00;
            
            *p++ = 0x00;//5字节,备用字节
            *p++ = 0x00;
            *p++ = 0x00;
            *p++ = 0x00;
            *p++ = 0x00;
            
            length = 35;
        }
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_SPEED,StartTime,EndTime,Block);
        
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
    TIME_T Time;
    u32 TimeCount;
    u32 EndTimeCount;
    s16  *pSector;
    s16  *pStep;
    
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

        RTC_GetCurTime(&Time);
        Time.min = 0;
        Time.sec = 0;
        TimeCount = ConverseGmtime(&Time);
        EndTimeCount = ConverseGmtime(&EndTime);
        
        p = pDstBuffer;
        length = 0;
        if(EndTimeCount >= TimeCount)
        {
          length = VDRPosition_GetCurHourPosition(p);//获取最新的那个小时数据
            if(0 == length)
            {
              length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_POSITION,StartTime,EndTime,Block);
            }
        }
        else
        {
          length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_POSITION,StartTime,EndTime,Block);
        }
        
        
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_DOUBT,StartTime,EndTime,Block);
        
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDROvertime_GetUnsaveLog(p, EndTime, DRIVER_NUM_MAX);
        p += length;
        length += VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_OVERTIME,StartTime,EndTime,Block);
        
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_DRIVER,StartTime,EndTime,Block);
        
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_POWER,StartTime,EndTime,Block);
        
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_PARAMETER,StartTime,EndTime,Block);
        
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
    s16  *pSector;
    s16  *pStep;
    
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
        length = VDRData_Read(p,pSector,pStep,VDR_DATA_TYPE_SPEED_STATUS,StartTime,EndTime,Block);
        
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
    
    if(80 == SrcBufferLen)//为了兼容之前的行驶记录仪设置而增加这项，注意80字节是有误的，这里只是做兼容处理
    {
        p = pSrcBuffer;//格式见表A1.12
        sFLASH_EraseSector(FLASH_VDR_STATUS_SET_ADDR);
        sFLASH_WriteBuffer(p,FLASH_VDR_STATUS_SET_ADDR,SrcBufferLen);
			  return 0;//应答帧为空
    }
    else if(87 == SrcBufferLen)
    {
        p = pSrcBuffer+7;//格式见表A1.12
        sFLASH_EraseSector(FLASH_VDR_STATUS_SET_ADDR);
        sFLASH_WriteBuffer(p,FLASH_VDR_STATUS_SET_ADDR,SrcBufferLen-7);

        return 0;//应答帧为空
    }
		else
		{
		    return 0xfffe;
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
            SetRtc(&Time);
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
        u8  Buffer[4];
    
    if(SrcBufferLen != 8)
    {
        return 0xfffe;
    }
    else
    {
        p = pSrcBuffer;
        p += 6;//跳过6字节的实时时间
                Buffer[0] = 0;
                memcpy(Buffer+1,p,2);
        EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID,Buffer,3); 
    VDRPulse_UpdateParameter();
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
    
    if(SrcBufferLen == 4)//只有起始里程
    {
        p = pSrcBuffer;
        StartMile = Public_ConvertBCDToLong(p);
        Public_ConvertLongToBuffer(StartMile,Buffer);
        EepromPram_WritePram(E2_CAR_INIT_MILE_ID, Buffer, 4); //设置起始里程
        //设置累计行驶里程（每次设置起始里程时会自动把累计行驶里程也设为起始里程）
        GpsMile_SetPram(StartMile);

        return 0;//应答帧为空
    }
    else if(SrcBufferLen == 20)//表A.9的全部内容
    {
      p = pSrcBuffer+12;
        StartMile = Public_ConvertBCDToLong(p);
        Public_ConvertLongToBuffer(StartMile,Buffer);
        EepromPram_WritePram(E2_CAR_INIT_MILE_ID, Buffer, 4); //设置起始里程
        //设置累计行驶里程（每次设置起始里程时会自动把累计行驶里程也设为起始里程）
        p = pSrcBuffer+16;
        StartMile = Public_ConvertBCDToLong(p);
        GpsMile_SetPram(StartMile);
        
        return 0;
    }
    else
    {
      return 0xfffe;
    }
}
/**************************************************************************
//函数名：VDRProtocol_ParseCmdD0H
//功能：D0H指令应答
//输入：pSrcBuffer:指向源数据（指令数据），SrcBufferLen:源数据长度
//    ：pSrcBuffer:指向目标数据（应答数据）
//输出：无
//返回值：应答数据长度
//备注：仅VDRProtocol_ParseCmd调用该函数
***************************************************************************/
static u16 VDRProtocol_ParseCmdD0H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
        u16 length = 0;
        s16 temp;
        u8  *p = NULL;
        
        
    if(SrcBufferLen == 0)
    {
        return 0xfffd;
    }
    else
    {
        if(0 == *pSrcBuffer)
                {
                        length = 0;
                        p = pDstBuffer;
                        *p++ = *pSrcBuffer;
                        length++;
                        RadioProtocol_ReadPram(p,&temp, pSrcBuffer+1,SrcBufferLen-1,0);
                        length += temp;
                }
                else if(1 == *pSrcBuffer)
                {
                        length = 0;
                        p = pDstBuffer;
                        *p++ = *pSrcBuffer;
                        length++;
                        RadioProtocol_WritePram(0,pSrcBuffer+1,SrcBufferLen-1);
                }
                else
                {
                        return 0xfffd;   
                }
    }
        
        return length;
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
        VDRDocimasy_EnterE0H();
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
    u8  *p = NULL;
        u8  Buffer[4];
    u8  PramLen;
    u16 length;
    u16 Speed;
    u32 TotalMile;

    if(SrcBufferLen != 0)
    {
        return 0xfffd;
    }
    else
    {
        VDRDocimasy_EnterE1H();

        length = 0;
        p = pDstBuffer;

        PramLen = EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,p);//唯一性编号,35字节
        p += PramLen;
        length += PramLen;

        PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, Buffer);//脉冲系数,2字节
                memcpy(p,Buffer+1,2);
        p += 2;
        length += 2;

        Speed = VDRSpeed_GetCurSpeed()*10;//当前速度,2字节
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
        VDRDocimasy_EnterE2H();
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
        VDRDocimasy_EnterE3H();
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
        VDRDocimasy_EnterE4H();
        return 0;
    }
}













