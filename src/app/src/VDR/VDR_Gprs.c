/************************************************************************
//程序名称：VDR_Gprs.c
//功能：通过GPRS网络读取行驶记录仪数据
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：
*************************************************************************/

/********************文件包含*************************/
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "Public.h"
#include "MileMeter.h"
#include "RadioProtocol.h"
#include "modem_app_com.h"
#include "taskschedule.h"
#include "ff.h"
#include "usbh_usr.h"

/********************本地变量*************************/
static u8  VdrGprsParseBuffer[VDR_GPRS_PARSE_BUFFER_SIZE] = {0};
static u16 VdrGprsParseBufferLen = 0;
static FIL VdrFile;

const u8 VdrFileName[8][15]=
{ 
  {"1:BDJC_08H.VDR"},
  {"1:BDJC_09H.VDR"},
  {"1:BDJC_10H.VDR"},
	{"1:BDJC_11H.VDR"},
  {"1:BDJC_12H.VDR"},
  {"1:BDJC_13H.VDR"},
	{"1:BDJC_14H.VDR"},
  {"1:BDJC_15H.VDR"}
};


/********************全局变量*************************/
u8  VdrGprsResendRequestFlag = 0;//重发请求标志
u8  VdrGprsResendLen;
u8  VdrGprsRxBuffer[VDR_GPRS_RX_BUFFER_SIZE] = {0};
u16 VdrGprsRxBufferLen = 0;
u8  VdrGprsParseEnableFlag = 0;//无线协议解析使能标志，1使能，0禁止
u8  VdrGprsAckCmd =0xff;//记录仪应答命令
u8  VdrGprsResendCmd = 0xff;//记录仪重传（补传）命令
u8  VdrGprsResendType = 0;
u16 VdrGprsResendPacketNum[VDR_GPRS_RESEND_PACKET_MAX];//重传包序号
u8  VdrGprsResendCountMax = 0;//重传包总数，最多为VDR_GPRS_RESEND_PACKET_MAX
u8  VdrGprsResendCount = 0;//重传计数
s16 VdrGprsTotalPacket;//08H-15H命令的包总数
u8  VdrGprsAckChannel = CHANNEL_DATA_1;
u16 VdrGprsAckSerialNum = 0;
u8  VdrGprsResendAckChannel = CHANNEL_DATA_1;
u16 VdrGprsResendAckSerialNum = 0;
s16 VdrGprsPacketNum = 0;//当前发送的包序号，从1开始
s16 VdrGprsPacketEndNum = -1;//总包数，当VdrGprsPacketNum == VdrGprsPacketEndNum表示发送完成,-1表示还没有从flash读取总包数
s16 VdrGprsPacketStartNum = -1;
TIME_T VdrGprsStartTime;
TIME_T VdrGprsEndTime;
s16 VdrGprsPacketStartSector = 0;
s16 VdrGprsPacketStartStep = 0;
s16 VdrGprsPacketEndSector = 0;
s16 VdrGprsPacketEndStep = 0;
s16 VdrGprsPacketStartList = 0;
s16 VdrGprsPacketEndList = 0;
/********************外部变量*************************/
extern const u16 DataPacketList[];
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
extern const u16 DataCollectLen[];
extern const u16 DataPacketList[];
extern TIME_TASK MyTimerTask[];
extern u16 Link1FirstPackageSerialNum;//分包传输时第1包的流水号

/********************本地函数声明*********************/
static FunctionalState VDRGprs_TimeTask1(void);
static FunctionalState VDRGprs_TimeTask2(void);
static FunctionalState VDRGprs_ResendTimeTask1(void);
static FunctionalState VDRGprs_ResendTimeTask2(void);
/********************函数定义*************************/
/**************************************************************************
//函数名：VDRGprs_TimeTask
//功能：远程传输行驶记录仪数据
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒调度1次
***************************************************************************/
FunctionalState VDRGprs_TimeTask(void)
{
    if(1 == BBGNTestFlag)
		{
		   return VDRGprs_TimeTask2();
		}
		else
		{
		    return VDRGprs_TimeTask1();
		}
}
/**************************************************************************
//函数名：VDRGprs_ResendTimeTask
//功能：分包补传行驶记录仪数据
//输入：无
//输出：无
//返回值：分包没有传完时返回ENABLE，传完时返回DISABLE
//备注：1秒调度1次
***************************************************************************/
FunctionalState VDRGprs_ResendTimeTask(void)
{
    if(1 == BBGNTestFlag)
		{
		   return VDRGprs_ResendTimeTask2();
		}
		else
		{
		    return VDRGprs_ResendTimeTask1();
		}
}

/**************************************************************************
//函数名：VDRGprs_TimeTask1
//功能：远程传输行驶记录仪数据
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒调度1次，实际应用时VDRGprs_TimeTask调用这个函数
***************************************************************************/
static FunctionalState VDRGprs_TimeTask1(void)
{
    static u8 WriteEnableFlag = 0;
	
    u16  TotalPacket;
    u16  CurrentPacket;
    u16  length;
    u8   Type;
    u16   i;
    u8   VdrCmd;
    
  
  if(1 == VdrGprsParseEnableFlag)
  {
    if((0xaa == VdrGprsRxBuffer[0])&&(0x75 == VdrGprsRxBuffer[1])&&(VdrGprsAckCmd == VdrGprsRxBuffer[2]))
    {
      length = (VdrGprsRxBuffer[3] << 8)|VdrGprsRxBuffer[4];
      if(length > VDR_GPRS_RX_BUFFER_SIZE)
      {
         VdrGprsAckCmd = 0xff;
      }
      else
      {
         VdrCmd = VdrGprsRxBuffer[2];
      }
    }
    else
    {
      VdrGprsAckCmd = 0xff;
    }
    
    switch(VdrGprsAckCmd)
    {
      case VDR_PROTOCOL_CMD_00H:
      case VDR_PROTOCOL_CMD_01H:
      case VDR_PROTOCOL_CMD_02H:
      case VDR_PROTOCOL_CMD_03H:
      case VDR_PROTOCOL_CMD_04H:
      case VDR_PROTOCOL_CMD_05H:
      case VDR_PROTOCOL_CMD_06H:
      case VDR_PROTOCOL_CMD_07H:
      case VDR_PROTOCOL_CMD_82H:
      case VDR_PROTOCOL_CMD_83H:
      case VDR_PROTOCOL_CMD_84H:
      case VDR_PROTOCOL_CMD_C2H:
      case VDR_PROTOCOL_CMD_C3H:
      case VDR_PROTOCOL_CMD_C4H:
      {
				VdrGprsResendCmd = 0;
				VdrGprsResendCount = 0;
				VdrGprsResendCountMax = 0;
				VdrGprsTotalPacket = 0;
        VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
        VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
        VdrGprsParseBuffer[2] = VdrGprsAckCmd;
        VdrGprsParseBufferLen = VDRProtocol_ParseCmd(VdrGprsParseBuffer+3,VdrGprsAckCmd, VdrGprsRxBuffer+6, length);
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsAckCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
        VdrGprsParseBufferLen += 3;
        if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer, VdrGprsParseBufferLen, 0, 0,0))
        {
          VdrGprsAckCmd = 0xff;
          
        }
        break;
      }
      case VDR_PROTOCOL_CMD_08H:
      case VDR_PROTOCOL_CMD_09H:
      case VDR_PROTOCOL_CMD_10H:
      case VDR_PROTOCOL_CMD_11H:
      case VDR_PROTOCOL_CMD_12H:
      case VDR_PROTOCOL_CMD_13H:
      case VDR_PROTOCOL_CMD_14H:
      case VDR_PROTOCOL_CMD_15H:
      {
        if(VdrCmd >= VDR_PROTOCOL_CMD_10H)
        {
          Type = VdrCmd-VDR_PROTOCOL_CMD_10H+2;
        }
        else
        {
          Type = VdrCmd-VDR_PROTOCOL_CMD_08H;
        }
				VdrGprsResendType = Type;
				VdrGprsResendCmd = VdrCmd;
        if(VdrGprsPacketEndNum < 0)//表示还未读取总包数
        {
          VDRGprs_PacketAckPreHandle(Type);
          WriteEnableFlag = VDRData_ReadWriteEnableFlag(Type);//暂存写使能标志
        }
        if(VdrGprsPacketEndNum > 0)//有数据
        {
          if(VdrGprsPacketNum < VdrGprsPacketEndNum)//已上传完毕
          {
            VdrGprsAckCmd = 0xff;
            VDRData_SetWriteEnableFlag(Type, WriteEnableFlag);//恢复写使能标志
          }
          else
          {
            TotalPacket = VdrGprsPacketStartNum-VdrGprsPacketEndNum+1;
						VdrGprsTotalPacket = TotalPacket;
            CurrentPacket = VdrGprsPacketStartNum-VdrGprsPacketNum+1;
            VDRData_SetWriteEnableFlag(Type, 0);//禁止写
            if(1 == CurrentPacket)
            {
               VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
               VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
               VdrGprsParseBuffer[2] = VdrGprsAckCmd;
               VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer+3,Type,VdrGprsPacketNum,1);//读分包数据
               VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
               VdrGprsParseBufferLen += 3;
            }
            else
            {
               VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer,Type,VdrGprsPacketNum,1);//读分包数据
               VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer, VdrGprsParseBufferLen);//打包数据
            }
            
            if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer,VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0))
            {
               VdrGprsPacketNum--;
            }
          }
        }
        else//无数据
        {
          TotalPacket = 0;
          CurrentPacket = 0;
          VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
          VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
          VdrGprsParseBuffer[2] = VdrGprsAckCmd;
          VdrGprsParseBufferLen = 0;
          VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
          VdrGprsParseBufferLen += 3;
          if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0))
          {
             VdrGprsAckCmd = 0xff;
          }
        }
        break;
      } 
      default:
        VdrGprsParseEnableFlag = 0;
        VdrGprsPacketEndNum = -1;
        VdrGprsPacketStartList = 0;
        VdrGprsPacketEndList = 0;
        VdrGprsAckCmd = 0xff;
        VdrGprsAckChannel = CHANNEL_DATA_1;
        VdrGprsRxBufferLen = 0;
        for(i=0; i<VDR_GPRS_RX_BUFFER_SIZE; i++)
        {
          VdrGprsRxBuffer[i] = 0;
        }
        break;
    }
    return ENABLE;
  }
  else
  {
    return DISABLE;
  }
  
}
/**************************************************************************
//函数名：VDRGprs_TimeTask2
//功能：远程传输行驶记录仪数据
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：1秒调度1次，部标功能检测时VDRGprs_TimeTask调用这个函数
***************************************************************************/
static FunctionalState VDRGprs_TimeTask2(void)
{
	  
    u16  length;
    u16   i;
    u8   VdrCmd;
	  u8   Index;
	  u8   FileName[20];
	  FRESULT res;
	  u32 flen;
	
	  static u8 Step = 0;
    static u32  FileSize;
    static u16  TotalPacket;
    static u16  CurrentPacket;
    static u16  PacketSize;
	
  if(1 == VdrGprsParseEnableFlag)
  {
    if((0xaa == VdrGprsRxBuffer[0])&&(0x75 == VdrGprsRxBuffer[1])&&(VdrGprsAckCmd == VdrGprsRxBuffer[2]))
    {
      length = (VdrGprsRxBuffer[3] << 8)|VdrGprsRxBuffer[4];
      if(length > VDR_GPRS_RX_BUFFER_SIZE)
      {
         VdrGprsAckCmd = 0xff;
      }
      else
      {
         VdrCmd = VdrGprsRxBuffer[2];
      }
    }
    else
    {
      VdrGprsAckCmd = 0xff;
    }
    
    switch(VdrGprsAckCmd)
    {
      case VDR_PROTOCOL_CMD_82H:
      case VDR_PROTOCOL_CMD_83H:
      case VDR_PROTOCOL_CMD_84H:
      case VDR_PROTOCOL_CMD_C2H:
      case VDR_PROTOCOL_CMD_C3H:
      case VDR_PROTOCOL_CMD_C4H:
      {
				VdrGprsResendCmd = 0;
				VdrGprsResendCount = 0;
				VdrGprsResendCountMax = 0;
				VdrGprsTotalPacket = 0;
        VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
        VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
        VdrGprsParseBuffer[2] = VdrGprsAckCmd;
        VdrGprsParseBufferLen = VDRProtocol_ParseCmd(VdrGprsParseBuffer+3,VdrGprsAckCmd, VdrGprsRxBuffer+6, length);
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsAckCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
        VdrGprsParseBufferLen += 3;
        if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer, VdrGprsParseBufferLen,0,0,0))
        {
          VdrGprsAckCmd = 0xff;
          
        }
        break;
      }
			case VDR_PROTOCOL_CMD_00H://部标功能检测时00H-07H指令会一次性下发，不是应答一条再发下一条的模式
			case VDR_PROTOCOL_CMD_01H:
      case VDR_PROTOCOL_CMD_02H:
      case VDR_PROTOCOL_CMD_03H:
      case VDR_PROTOCOL_CMD_04H:
      case VDR_PROTOCOL_CMD_05H:
      case VDR_PROTOCOL_CMD_06H:
      case VDR_PROTOCOL_CMD_07H:
			{
				VdrGprsResendCmd = 0;
				VdrGprsResendCount = 0;
				VdrGprsResendCountMax = 0;
				VdrGprsTotalPacket = 0;
        VdrGprsParseBuffer[0] = ((VdrGprsAckSerialNum+VdrGprsAckCmd)&0xff00) << 8;
        VdrGprsParseBuffer[1] = (VdrGprsAckSerialNum+VdrGprsAckCmd)&0xff;
        VdrGprsParseBuffer[2] = VdrGprsAckCmd;
        VdrGprsParseBufferLen = VDRProtocol_ParseCmd(VdrGprsParseBuffer+3,VdrGprsAckCmd, VdrGprsRxBuffer+6, length);
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsAckCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
        VdrGprsParseBufferLen += 3;
        if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer, VdrGprsParseBufferLen, 0, 0, 0))
        {
          VdrGprsAckCmd++;
					VdrGprsRxBuffer[2] = VdrGprsAckCmd;
					if(VdrGprsAckCmd >= 0x08)
					{
					    VdrGprsAckCmd = 0xff;
					}
        }
			  break;
			}
      case VDR_PROTOCOL_CMD_08H:
      case VDR_PROTOCOL_CMD_09H:
      case VDR_PROTOCOL_CMD_10H:
      case VDR_PROTOCOL_CMD_11H:
      case VDR_PROTOCOL_CMD_12H:
      case VDR_PROTOCOL_CMD_13H:
      case VDR_PROTOCOL_CMD_14H:
      case VDR_PROTOCOL_CMD_15H:
      {
				if(0 == Step)//打开文件
				{
					Index = VdrGprsAckCmd;
					if(Index >= 0x10)
					{
					    Index -= 6;
					}
					Index -= 8;
				  strcpy((char *)FileName,(const char *)VdrFileName[Index]);
				  if(f_open(&VdrFile,(WCHAR*)ff_NameConver(FileName),FA_READ) == FR_OK)
          {
						FileSize = VdrFile.fsize;
						FileSize -= 25;//数据块个数2字节+数据代码1字节+数据名称18字节+数据长度4字节
						FileSize --;//再减去一个字节的校验码，该校验码在文件的末尾
						PacketSize = DataCollectLen[Index]*DataPacketList[Index];
						if(PacketSize >= 1000)//达到1000字节后，加上行驶记录仪命令头尾，行标头尾总长度超过每包上限值1024
						{
						  PacketSize = PacketSize >> 1;//长度减半传输
						}
						TotalPacket = FileSize/PacketSize;
						if(0 != FileSize%PacketSize)
						{
						  TotalPacket++;
						}
						CurrentPacket = 1;
						
						VdrGprsResendType = Index;
				    VdrGprsResendCmd = VdrGprsAckCmd;
						
						res = f_lseek(&VdrFile, 25);
            if(FR_OK !=res) 
						{
							VdrGprsAckCmd = 0xff;
							f_close(&VdrFile);
						}
						else
						{
				      Step++;
						}
						
				  }
					else
					{
					  VdrGprsAckCmd = 0xff;
						f_close(&VdrFile);
					}
				}
				else if(1 == Step)//传输数据包
				{
					if((1== CurrentPacket)&&(0 == TotalPacket))//数据为空
					{
					  TotalPacket = 0;
            CurrentPacket = 0;
            VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
            VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
            VdrGprsParseBuffer[2] = VdrGprsAckCmd;
            VdrGprsParseBufferLen = 0;
            VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
            VdrGprsParseBufferLen += 3;
            if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0))
            {
              VdrGprsAckCmd = 0xff;
							f_close(&VdrFile);
            }
					}
					else if((1== CurrentPacket)&&(0 != TotalPacket))//第1包
					{
					  VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
            VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
            VdrGprsParseBuffer[2] = VdrGprsAckCmd;
						f_read(&VdrFile,VdrGprsParseBuffer+3,PacketSize,&flen);
            VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, flen);//打包数据
            VdrGprsParseBufferLen += 3;
						//if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0)) 
						if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,0,0,0))
						{
							if(CurrentPacket >= TotalPacket)
							{
                VdrGprsAckCmd = 0xff;
								f_close(&VdrFile);
							}
							else
							{
							  CurrentPacket++;
							}
            }
					}
					else//其它包
					{
						VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
            VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
            VdrGprsParseBuffer[2] = VdrGprsAckCmd;
						f_read(&VdrFile,VdrGprsParseBuffer+3,PacketSize,&flen);
            VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, flen);//打包数据
            VdrGprsParseBufferLen += 3;
						
						//f_read(&VdrFile,VdrGprsParseBuffer,PacketSize,&flen);
            //VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer, flen);//打包数据
						//if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0)) 
						if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,0,0,0)) 
						{
							if(CurrentPacket >= TotalPacket)
							{
                VdrGprsAckCmd = 0xff;
								f_close(&VdrFile);
							}
							else
							{
							  CurrentPacket++;
							}
            }
					}
				}
        break;
      } 
      default:
				Step = 0;
			  CurrentPacket = 0;
			  TotalPacket = 0;
        VdrGprsParseEnableFlag = 0;
        VdrGprsPacketEndNum = -1;
        VdrGprsPacketStartList = 0;
        VdrGprsPacketEndList = 0;
        VdrGprsAckCmd = 0xff;
        VdrGprsAckChannel = CHANNEL_DATA_1;
        VdrGprsRxBufferLen = 0;
        for(i=0; i<VDR_GPRS_RX_BUFFER_SIZE; i++)
        {
          VdrGprsRxBuffer[i] = 0;
        }
        break;
    }
    return ENABLE;
  }
  else
  {
    return DISABLE;
  }
  
}
/**************************************************************************
//函数名：VDRGprs_ResendTimeTask1
//功能：分包补传行驶记录仪数据
//输入：无
//输出：无
//返回值：分包没有传完时返回ENABLE，传完时返回DISABLE
//备注：1秒调度1次
***************************************************************************/
FunctionalState VDRGprs_ResendTimeTask1(void)
{
	u16 CurrentPacket;
	
	if((VdrGprsResendCount >= VdrGprsResendCountMax)||(VdrGprsResendCount >= VDR_GPRS_RESEND_PACKET_MAX))
	{
		//使能VDRGprs_TimeTask任务，当使能VDRGprs_ResendTimeTask任务时VDRGprs_TimeTask会关闭
	  VDRData_SetWriteEnableFlag(VdrGprsResendType, 1);//使能写，补传过程中会禁止写
	  return DISABLE;
	}
  switch(VdrGprsResendCmd)
	{
	case VDR_PROTOCOL_CMD_08H:
  case VDR_PROTOCOL_CMD_09H:
  case VDR_PROTOCOL_CMD_10H:
  case VDR_PROTOCOL_CMD_11H:
  case VDR_PROTOCOL_CMD_12H:
  case VDR_PROTOCOL_CMD_13H:
  case VDR_PROTOCOL_CMD_14H:
  case VDR_PROTOCOL_CMD_15H:
	  {
			VDRData_SetWriteEnableFlag(VdrGprsResendType, 0);//禁止写
			CurrentPacket = VdrGprsResendPacketNum[VdrGprsResendCount];
      if(1 == CurrentPacket)
      {
        VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
        VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
        VdrGprsParseBuffer[2] = VdrGprsAckCmd;
        VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer+3,VdrGprsResendType,VdrGprsPacketNum,1);//读分包数据
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//打包数据
        VdrGprsParseBufferLen += 3;
      }
      else
      {
        VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer,VdrGprsResendType,VdrGprsPacketNum,1);//读分包数据
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer, VdrGprsParseBufferLen);//打包数据
      }
            
      if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer,VdrGprsParseBufferLen,VdrGprsTotalPacket,CurrentPacket,1))
      {
         VdrGprsResendCount++;
      }
	    break;
	  }
	default:
	  {
	    VdrGprsResendCount = 0;
			VdrGprsResendCountMax = 0;
	  }
		break;
	}
	return ENABLE;
}
/**************************************************************************
//函数名：VDRGprs_ResendTimeTask2
//功能：分包补传行驶记录仪数据
//输入：无
//输出：无
//返回值：分包没有传完时返回ENABLE，传完时返回DISABLE
//备注：1秒调度1次，部标功能检测时调用VDRGprs_ResendTimeTask2，实际应用时调用VDRGprs_ResendTimeTask1
***************************************************************************/
FunctionalState VDRGprs_ResendTimeTask2(void)
{
	u8   FileName[20];
	FRESULT res;
	u32 flen;
//	u8  flag = 0;
	
	static u8 Step = 0;
  static u32  FileSize;
  static u16  TotalPacket;
  static u16  CurrentPacket;
  static u16  PacketSize;
	
	
	if((VdrGprsResendCount >= VdrGprsResendCountMax)||(VdrGprsResendCount >= VDR_GPRS_RESEND_PACKET_MAX))
	{
		Step = 0;
	  return DISABLE;
	}
  switch(VdrGprsResendCmd)
	{
	case VDR_PROTOCOL_CMD_08H:
  case VDR_PROTOCOL_CMD_09H:
  case VDR_PROTOCOL_CMD_10H:
  case VDR_PROTOCOL_CMD_11H:
  case VDR_PROTOCOL_CMD_12H:
  case VDR_PROTOCOL_CMD_13H:
  case VDR_PROTOCOL_CMD_14H:
  case VDR_PROTOCOL_CMD_15H:
	  {
      if(0 == Step)//打开文件
			{
				strcpy((char *)FileName,(const char *)VdrFileName[VdrGprsResendType]);
				if(f_open(&VdrFile,(WCHAR*)ff_NameConver(FileName),FA_READ) == FR_OK)
        {
				  FileSize = VdrFile.fsize;
					FileSize -= 25;//数据块个数2字节+数据代码1字节+数据名称18字节+数据长度4字节
					FileSize --;//再减去一个字节的校验码，该校验码在文件的末尾
					PacketSize = DataCollectLen[VdrGprsResendType]*DataPacketList[VdrGprsResendType];
					if(PacketSize >= 1000)//达到1000字节后，加上行驶记录仪命令头尾，行标头尾总长度超过每包上限值1024
					{
						PacketSize = PacketSize >> 1;//长度减半传输
					}
					TotalPacket = FileSize/PacketSize;
					if(0 != FileSize%PacketSize)
					{
						TotalPacket++;
					}
					VdrGprsResendCount = 0;
					res = f_lseek(&VdrFile, 25);
          if(FR_OK !=res) 
					{
						f_close(&VdrFile);
						VdrGprsResendCount = 0;
			      VdrGprsResendCountMax = 0;
					}
					else
					{
				     Step++;
					}
				}
				else
				{
					f_close(&VdrFile);
					VdrGprsResendCount = 0;
			    VdrGprsResendCountMax = 0;
				}
			}
			else if(1 == Step)//传输数据包
			{
				CurrentPacket = VdrGprsResendPacketNum[VdrGprsResendCount];
				if((CurrentPacket > TotalPacket)||(VdrGprsResendCount >= VdrGprsResendCountMax))
			  {
						f_close(&VdrFile);
					  Step = 0;
					  VdrGprsResendCount = 0;
			      VdrGprsResendCountMax = 0;
			  }
				else if((1== CurrentPacket)&&(0 != TotalPacket))//第1包
				{
					VdrGprsParseBuffer[0] = (VdrGprsResendAckSerialNum&0xff00) << 8;
          VdrGprsParseBuffer[1] = VdrGprsResendAckSerialNum&0xff;
          VdrGprsParseBuffer[2] = VdrGprsResendCmd;
				  f_read(&VdrFile,VdrGprsParseBuffer+3,PacketSize,&flen);
          VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer+3, flen);//打包数据
          VdrGprsParseBufferLen += 3;
				  if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsResendAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,1))
          {
					  VdrGprsResendCount++;
          }
				}
				else if(0 != TotalPacket)//其它包
				{
					f_lseek(&VdrFile, 25+(CurrentPacket-1)*PacketSize);
					f_read(&VdrFile,VdrGprsParseBuffer,PacketSize,&flen);
          VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer, flen);//打包数据
					if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsResendAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,1))
          {
				    VdrGprsResendCount++;
          }
				}
			}      
	    break;
	  }
	default:
	  {
	    VdrGprsResendCount = 0;
			VdrGprsResendCountMax = 0;
			Step = 0;
	  }
		break;
	}
	return ENABLE;
}
/**************************************************************************
//函数名：VDRGprs_RecData
//功能：接收远程记录仪命令
//输入：无
//输出：无
//返回值：无
//备注：无
***************************************************************************/
void VDRGprs_RecData(u8 channel, u16 SerialNum, u8 *pBuffer, u16 BufferLen)
{
  u8 *p = NULL;
    
    if((0 == VdrGprsParseEnableFlag)&&(BufferLen < VDR_GPRS_RX_BUFFER_SIZE))
    {
        VdrGprsAckChannel = channel;
        VdrGprsAckSerialNum = SerialNum;
			
			  VdrGprsResendAckChannel = channel;
        VdrGprsResendAckSerialNum = SerialNum;
        
        p = pBuffer;
        VdrGprsAckCmd = *p++;
			
			  if(1 == BBGNTestFlag)
				{
					/*
				    if((0xaa == VdrGprsRxBuffer[0])&&(0x75 == VdrGprsRxBuffer[1])&&(VdrGprsAckCmd == VdrGprsRxBuffer[2]))
    {
      length = (VdrGprsRxBuffer[3] << 8)|VdrGprsRxBuffer[4];
      if(length > VDR_GPRS_RX_BUFFER_SIZE)
      {
         VdrGprsAckCmd = 0xff;
      }
      else
      {
         VdrCmd = VdrGprsRxBuffer[2];
      }
					
    }
    else
    {
      VdrGprsAckCmd = 0xff;
    }
					*/
					VdrGprsRxBuffer[0] = 0xaa;
					VdrGprsRxBuffer[1] = 0x75;
					VdrGprsRxBuffer[2] = VdrGprsAckCmd;
					VdrGprsRxBuffer[3] = 0x00;
					VdrGprsRxBuffer[4] = 0x00;
				}
				else
				{
            memcpy(VdrGprsRxBuffer,p,BufferLen-1);
				}
        
        VdrGprsParseEnableFlag = 1;
        
        SetTimerTask(TIME_VDR_GPRS,2*SECOND);
        
    }
}
/**************************************************************************
//函数名：VDRGprs_PacketResendRequest
//功能：平台下发分包补传请求
//输入：无
//输出：无
//返回值：无
//备注：当收到平台下发的分包补传指令0x8003时，调用此函数
***************************************************************************/
void VDRGprs_PacketResendRequest(u8 channel, u8 *pBuffer, u16 BufferLen)
{
	  u16 temp;
	  u16 FirstPacketSerialNum;//第1包流水号
	  u8  ResendTotalPacket;//重传包总数
	  u8  *p=NULL;
	  u8  i;
	
	  if(NULL != p)
		{
		    return ;
		}
		
		if(ENABLE == MyTimerTask[TIME_VDR_PACKET_RESEND].TaskTimerState)//正在分包补传时不响应当前补传请求，目前只做行驶记录仪分包补传
    {
		    return ;
		}
		
		p=pBuffer;
		FirstPacketSerialNum = 0;
		FirstPacketSerialNum |= *p++ << 8;
		FirstPacketSerialNum |= *p++;
		Link1FirstPackageSerialNum = FirstPacketSerialNum;
		
		ResendTotalPacket = *p++;
		if((ResendTotalPacket > VDR_GPRS_RESEND_PACKET_MAX)||(0 == ResendTotalPacket))//dxl,2016.5.12 全部传完后平台也会下发0
		{
		    return ;
		}
		VdrGprsResendCountMax = ResendTotalPacket;
    VdrGprsResendCount = 0;
		for(i=0; i<ResendTotalPacket; i++)
		{
			  temp = 0;
			  temp |= *p++ << 8;
			  temp |= *p++;
		    VdrGprsResendPacketNum[i] = temp;
		}
		SetTimerTask(TIME_VDR_PACKET_RESEND, 2*SECOND);

}
/**************************************************************************
//函数名：VDRGprs_PacketAckPreHandle
//功能：分包应答预处理
//输入：
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDRGprs_PacketAckPreHandle(u8 Type)
{
    
    u16 length;
    u32 OldestTimeCount = 0;
    TIME_T OldestTime;
//  TIME_T Time;
    u32 StartTimeCount;
    u32 EndTimeCount;
    u32 TimeCount;
    //s16 Sector;
//  s16 Step;
    
    if(Type >= VDR_DATA_TYPE_MAX)
    {
      return;
    }
  length = (VdrGprsRxBuffer[3] << 8)|VdrGprsRxBuffer[4];
    
  if(length >= 14)//带了时间参数
  {
    OldestTimeCount = VDRData_ReadOldestTime(Type);
    OldestTimeCount++;//使其大于最老时间
    Gmtime(&OldestTime, OldestTimeCount);
        
    VDRPub_ConvertBCDToTime(&VdrGprsStartTime,VdrGprsRxBuffer+6);
    VDRPub_ConvertBCDToTime(&VdrGprsEndTime,VdrGprsRxBuffer+12);
    
		if(VDR_DATA_TYPE_POSITION == Type)
		{
        VdrGprsStartTime.min = 0;
        VdrGprsStartTime.sec = 0;	
		    VdrGprsEndTime.min = 0;
        VdrGprsEndTime.sec = 0;
			  StartTimeCount = ConverseGmtime(&VdrGprsStartTime);
        EndTimeCount = ConverseGmtime(&VdrGprsEndTime);
		}
		else
		{
        StartTimeCount = ConverseGmtime(&VdrGprsStartTime);
        EndTimeCount = ConverseGmtime(&VdrGprsEndTime);
        
        StartTimeCount++;//因为查找的结果是小于给定值，所以要++
        //EndTimeCount++;
		}
        
    Gmtime(&VdrGprsStartTime, StartTimeCount);
    Gmtime(&VdrGprsEndTime, EndTimeCount);
     
    VDRData_Read(VdrGprsParseBuffer,&VdrGprsPacketStartSector,&VdrGprsPacketStartStep,Type,VdrGprsStartTime,VdrGprsEndTime, 1);//倒序，起点
    VDRData_Read(VdrGprsParseBuffer,&VdrGprsPacketEndSector,&VdrGprsPacketEndStep,Type,OldestTime,VdrGprsStartTime, 1);//终点
    VdrGprsPacketStartList = VDRData_ReadListNum(Type,VdrGprsPacketStartSector,VdrGprsPacketStartStep);
    VdrGprsPacketEndList = VDRData_ReadListNum(Type,VdrGprsPacketEndSector,VdrGprsPacketEndStep);
    if(0 != VdrGprsPacketEndList)
    {
      for(;;)
      {
        TimeCount = VDRData_ReadListTime(Type,VdrGprsPacketEndList);
        if(TimeCount >= StartTimeCount)
        {
          break;
        }
        if(VdrGprsPacketEndList >= VDRData_ReadTotalList(Type))
        {
          break;
        }
        VdrGprsPacketEndList++;
      }
    }
    if((0 == VdrGprsPacketEndList)&&(0 != VdrGprsPacketStartList))//给定的初始时间太小
    {
      VdrGprsPacketEndList = 1;
    }

    VdrGprsPacketEndNum = VdrGprsPacketEndList/DataPacketList[Type]+1;
    
    VdrGprsPacketStartNum = VdrGprsPacketStartList/DataPacketList[Type]+1;
    
    VdrGprsPacketNum = VdrGprsPacketStartNum;
        
  }
  else//没有带时间参数，只有命令字
  {
    VdrGprsPacketStartNum = VDRData_ReadTotalPacket(Type);
    VdrGprsPacketEndNum = 1;
    VdrGprsPacketNum = VdrGprsPacketStartNum;
  }
          
  if(VdrGprsPacketStartNum < VdrGprsPacketEndNum)//出错
    {
      VdrGprsPacketStartNum = 0;
      VdrGprsPacketEndNum = 0;
      VdrGprsPacketNum = 0;
    }
  
}













