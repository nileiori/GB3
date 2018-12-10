/************************************************************************
//�������ƣ�VDR_Gprs.c
//���ܣ�ͨ��GPRS�����ȡ��ʻ��¼������
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1��
*************************************************************************/

/********************�ļ�����*************************/
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

/********************���ر���*************************/
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


/********************ȫ�ֱ���*************************/
u8  VdrGprsResendRequestFlag = 0;//�ط������־
u8  VdrGprsResendLen;
u8  VdrGprsRxBuffer[VDR_GPRS_RX_BUFFER_SIZE] = {0};
u16 VdrGprsRxBufferLen = 0;
u8  VdrGprsParseEnableFlag = 0;//����Э�����ʹ�ܱ�־��1ʹ�ܣ�0��ֹ
u8  VdrGprsAckCmd =0xff;//��¼��Ӧ������
u8  VdrGprsResendCmd = 0xff;//��¼���ش�������������
u8  VdrGprsResendType = 0;
u16 VdrGprsResendPacketNum[VDR_GPRS_RESEND_PACKET_MAX];//�ش������
u8  VdrGprsResendCountMax = 0;//�ش������������ΪVDR_GPRS_RESEND_PACKET_MAX
u8  VdrGprsResendCount = 0;//�ش�����
s16 VdrGprsTotalPacket;//08H-15H����İ�����
u8  VdrGprsAckChannel = CHANNEL_DATA_1;
u16 VdrGprsAckSerialNum = 0;
u8  VdrGprsResendAckChannel = CHANNEL_DATA_1;
u16 VdrGprsResendAckSerialNum = 0;
s16 VdrGprsPacketNum = 0;//��ǰ���͵İ���ţ���1��ʼ
s16 VdrGprsPacketEndNum = -1;//�ܰ�������VdrGprsPacketNum == VdrGprsPacketEndNum��ʾ�������,-1��ʾ��û�д�flash��ȡ�ܰ���
s16 VdrGprsPacketStartNum = -1;
TIME_T VdrGprsStartTime;
TIME_T VdrGprsEndTime;
s16 VdrGprsPacketStartSector = 0;
s16 VdrGprsPacketStartStep = 0;
s16 VdrGprsPacketEndSector = 0;
s16 VdrGprsPacketEndStep = 0;
s16 VdrGprsPacketStartList = 0;
s16 VdrGprsPacketEndList = 0;
/********************�ⲿ����*************************/
extern const u16 DataPacketList[];
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern const u16 DataCollectLen[];
extern const u16 DataPacketList[];
extern TIME_TASK MyTimerTask[];
extern u16 Link1FirstPackageSerialNum;//�ְ�����ʱ��1������ˮ��

/********************���غ�������*********************/
static FunctionalState VDRGprs_TimeTask1(void);
static FunctionalState VDRGprs_TimeTask2(void);
static FunctionalState VDRGprs_ResendTimeTask1(void);
static FunctionalState VDRGprs_ResendTimeTask2(void);
/********************��������*************************/
/**************************************************************************
//��������VDRGprs_TimeTask
//���ܣ�Զ�̴�����ʻ��¼������
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1�����1��
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
//��������VDRGprs_ResendTimeTask
//���ܣ��ְ�������ʻ��¼������
//���룺��
//�������
//����ֵ���ְ�û�д���ʱ����ENABLE������ʱ����DISABLE
//��ע��1�����1��
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
//��������VDRGprs_TimeTask1
//���ܣ�Զ�̴�����ʻ��¼������
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1�����1�Σ�ʵ��Ӧ��ʱVDRGprs_TimeTask�����������
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
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsAckCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
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
        if(VdrGprsPacketEndNum < 0)//��ʾ��δ��ȡ�ܰ���
        {
          VDRGprs_PacketAckPreHandle(Type);
          WriteEnableFlag = VDRData_ReadWriteEnableFlag(Type);//�ݴ�дʹ�ܱ�־
        }
        if(VdrGprsPacketEndNum > 0)//������
        {
          if(VdrGprsPacketNum < VdrGprsPacketEndNum)//���ϴ����
          {
            VdrGprsAckCmd = 0xff;
            VDRData_SetWriteEnableFlag(Type, WriteEnableFlag);//�ָ�дʹ�ܱ�־
          }
          else
          {
            TotalPacket = VdrGprsPacketStartNum-VdrGprsPacketEndNum+1;
						VdrGprsTotalPacket = TotalPacket;
            CurrentPacket = VdrGprsPacketStartNum-VdrGprsPacketNum+1;
            VDRData_SetWriteEnableFlag(Type, 0);//��ֹд
            if(1 == CurrentPacket)
            {
               VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
               VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
               VdrGprsParseBuffer[2] = VdrGprsAckCmd;
               VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer+3,Type,VdrGprsPacketNum,1);//���ְ�����
               VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
               VdrGprsParseBufferLen += 3;
            }
            else
            {
               VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer,Type,VdrGprsPacketNum,1);//���ְ�����
               VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer, VdrGprsParseBufferLen);//�������
            }
            
            if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer,VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0))
            {
               VdrGprsPacketNum--;
            }
          }
        }
        else//������
        {
          TotalPacket = 0;
          CurrentPacket = 0;
          VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
          VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
          VdrGprsParseBuffer[2] = VdrGprsAckCmd;
          VdrGprsParseBufferLen = 0;
          VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
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
//��������VDRGprs_TimeTask2
//���ܣ�Զ�̴�����ʻ��¼������
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��1�����1�Σ����깦�ܼ��ʱVDRGprs_TimeTask�����������
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
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsAckCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
        VdrGprsParseBufferLen += 3;
        if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel,VdrGprsParseBuffer, VdrGprsParseBufferLen,0,0,0))
        {
          VdrGprsAckCmd = 0xff;
          
        }
        break;
      }
			case VDR_PROTOCOL_CMD_00H://���깦�ܼ��ʱ00H-07Hָ���һ�����·�������Ӧ��һ���ٷ���һ����ģʽ
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
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsAckCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
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
				if(0 == Step)//���ļ�
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
						FileSize -= 25;//���ݿ����2�ֽ�+���ݴ���1�ֽ�+��������18�ֽ�+���ݳ���4�ֽ�
						FileSize --;//�ټ�ȥһ���ֽڵ�У���룬��У�������ļ���ĩβ
						PacketSize = DataCollectLen[Index]*DataPacketList[Index];
						if(PacketSize >= 1000)//�ﵽ1000�ֽں󣬼�����ʻ��¼������ͷβ���б�ͷβ�ܳ��ȳ���ÿ������ֵ1024
						{
						  PacketSize = PacketSize >> 1;//���ȼ��봫��
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
				else if(1 == Step)//�������ݰ�
				{
					if((1== CurrentPacket)&&(0 == TotalPacket))//����Ϊ��
					{
					  TotalPacket = 0;
            CurrentPacket = 0;
            VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
            VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
            VdrGprsParseBuffer[2] = VdrGprsAckCmd;
            VdrGprsParseBufferLen = 0;
            VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
            VdrGprsParseBufferLen += 3;
            if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,0))
            {
              VdrGprsAckCmd = 0xff;
							f_close(&VdrFile);
            }
					}
					else if((1== CurrentPacket)&&(0 != TotalPacket))//��1��
					{
					  VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
            VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
            VdrGprsParseBuffer[2] = VdrGprsAckCmd;
						f_read(&VdrFile,VdrGprsParseBuffer+3,PacketSize,&flen);
            VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, flen);//�������
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
					else//������
					{
						VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
            VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
            VdrGprsParseBuffer[2] = VdrGprsAckCmd;
						f_read(&VdrFile,VdrGprsParseBuffer+3,PacketSize,&flen);
            VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer+3, flen);//�������
            VdrGprsParseBufferLen += 3;
						
						//f_read(&VdrFile,VdrGprsParseBuffer,PacketSize,&flen);
            //VdrGprsParseBufferLen = VDRProtocol_Packet(VdrCmd, VdrGprsParseBuffer, flen);//�������
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
//��������VDRGprs_ResendTimeTask1
//���ܣ��ְ�������ʻ��¼������
//���룺��
//�������
//����ֵ���ְ�û�д���ʱ����ENABLE������ʱ����DISABLE
//��ע��1�����1��
***************************************************************************/
FunctionalState VDRGprs_ResendTimeTask1(void)
{
	u16 CurrentPacket;
	
	if((VdrGprsResendCount >= VdrGprsResendCountMax)||(VdrGprsResendCount >= VDR_GPRS_RESEND_PACKET_MAX))
	{
		//ʹ��VDRGprs_TimeTask���񣬵�ʹ��VDRGprs_ResendTimeTask����ʱVDRGprs_TimeTask��ر�
	  VDRData_SetWriteEnableFlag(VdrGprsResendType, 1);//ʹ��д�����������л��ֹд
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
			VDRData_SetWriteEnableFlag(VdrGprsResendType, 0);//��ֹд
			CurrentPacket = VdrGprsResendPacketNum[VdrGprsResendCount];
      if(1 == CurrentPacket)
      {
        VdrGprsParseBuffer[0] = (VdrGprsAckSerialNum&0xff00) << 8;
        VdrGprsParseBuffer[1] = VdrGprsAckSerialNum&0xff;
        VdrGprsParseBuffer[2] = VdrGprsAckCmd;
        VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer+3,VdrGprsResendType,VdrGprsPacketNum,1);//���ְ�����
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer+3, VdrGprsParseBufferLen);//�������
        VdrGprsParseBufferLen += 3;
      }
      else
      {
        VdrGprsParseBufferLen = VDRData_ReadPacket(VdrGprsParseBuffer,VdrGprsResendType,VdrGprsPacketNum,1);//���ְ�����
        VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer, VdrGprsParseBufferLen);//�������
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
//��������VDRGprs_ResendTimeTask2
//���ܣ��ְ�������ʻ��¼������
//���룺��
//�������
//����ֵ���ְ�û�д���ʱ����ENABLE������ʱ����DISABLE
//��ע��1�����1�Σ����깦�ܼ��ʱ����VDRGprs_ResendTimeTask2��ʵ��Ӧ��ʱ����VDRGprs_ResendTimeTask1
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
      if(0 == Step)//���ļ�
			{
				strcpy((char *)FileName,(const char *)VdrFileName[VdrGprsResendType]);
				if(f_open(&VdrFile,(WCHAR*)ff_NameConver(FileName),FA_READ) == FR_OK)
        {
				  FileSize = VdrFile.fsize;
					FileSize -= 25;//���ݿ����2�ֽ�+���ݴ���1�ֽ�+��������18�ֽ�+���ݳ���4�ֽ�
					FileSize --;//�ټ�ȥһ���ֽڵ�У���룬��У�������ļ���ĩβ
					PacketSize = DataCollectLen[VdrGprsResendType]*DataPacketList[VdrGprsResendType];
					if(PacketSize >= 1000)//�ﵽ1000�ֽں󣬼�����ʻ��¼������ͷβ���б�ͷβ�ܳ��ȳ���ÿ������ֵ1024
					{
						PacketSize = PacketSize >> 1;//���ȼ��봫��
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
			else if(1 == Step)//�������ݰ�
			{
				CurrentPacket = VdrGprsResendPacketNum[VdrGprsResendCount];
				if((CurrentPacket > TotalPacket)||(VdrGprsResendCount >= VdrGprsResendCountMax))
			  {
						f_close(&VdrFile);
					  Step = 0;
					  VdrGprsResendCount = 0;
			      VdrGprsResendCountMax = 0;
			  }
				else if((1== CurrentPacket)&&(0 != TotalPacket))//��1��
				{
					VdrGprsParseBuffer[0] = (VdrGprsResendAckSerialNum&0xff00) << 8;
          VdrGprsParseBuffer[1] = VdrGprsResendAckSerialNum&0xff;
          VdrGprsParseBuffer[2] = VdrGprsResendCmd;
				  f_read(&VdrFile,VdrGprsParseBuffer+3,PacketSize,&flen);
          VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer+3, flen);//�������
          VdrGprsParseBufferLen += 3;
				  if(ACK_OK == RadioProtocol_RecorderDataReport(VdrGprsResendAckChannel, VdrGprsParseBuffer, VdrGprsParseBufferLen,TotalPacket,CurrentPacket,1))
          {
					  VdrGprsResendCount++;
          }
				}
				else if(0 != TotalPacket)//������
				{
					f_lseek(&VdrFile, 25+(CurrentPacket-1)*PacketSize);
					f_read(&VdrFile,VdrGprsParseBuffer,PacketSize,&flen);
          VdrGprsParseBufferLen = VDRProtocol_Packet(VdrGprsResendCmd, VdrGprsParseBuffer, flen);//�������
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
//��������VDRGprs_RecData
//���ܣ�����Զ�̼�¼������
//���룺��
//�������
//����ֵ����
//��ע����
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
//��������VDRGprs_PacketResendRequest
//���ܣ�ƽ̨�·��ְ���������
//���룺��
//�������
//����ֵ����
//��ע�����յ�ƽ̨�·��ķְ�����ָ��0x8003ʱ�����ô˺���
***************************************************************************/
void VDRGprs_PacketResendRequest(u8 channel, u8 *pBuffer, u16 BufferLen)
{
	  u16 temp;
	  u16 FirstPacketSerialNum;//��1����ˮ��
	  u8  ResendTotalPacket;//�ش�������
	  u8  *p=NULL;
	  u8  i;
	
	  if(NULL != p)
		{
		    return ;
		}
		
		if(ENABLE == MyTimerTask[TIME_VDR_PACKET_RESEND].TaskTimerState)//���ڷְ�����ʱ����Ӧ��ǰ��������Ŀǰֻ����ʻ��¼�Ƿְ�����
    {
		    return ;
		}
		
		p=pBuffer;
		FirstPacketSerialNum = 0;
		FirstPacketSerialNum |= *p++ << 8;
		FirstPacketSerialNum |= *p++;
		Link1FirstPackageSerialNum = FirstPacketSerialNum;
		
		ResendTotalPacket = *p++;
		if((ResendTotalPacket > VDR_GPRS_RESEND_PACKET_MAX)||(0 == ResendTotalPacket))//dxl,2016.5.12 ȫ�������ƽ̨Ҳ���·�0
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
//��������VDRGprs_PacketAckPreHandle
//���ܣ��ְ�Ӧ��Ԥ����
//���룺
//�������
//����ֵ����
//��ע��
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
    
  if(length >= 14)//����ʱ�����
  {
    OldestTimeCount = VDRData_ReadOldestTime(Type);
    OldestTimeCount++;//ʹ���������ʱ��
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
        
        StartTimeCount++;//��Ϊ���ҵĽ����С�ڸ���ֵ������Ҫ++
        //EndTimeCount++;
		}
        
    Gmtime(&VdrGprsStartTime, StartTimeCount);
    Gmtime(&VdrGprsEndTime, EndTimeCount);
     
    VDRData_Read(VdrGprsParseBuffer,&VdrGprsPacketStartSector,&VdrGprsPacketStartStep,Type,VdrGprsStartTime,VdrGprsEndTime, 1);//�������
    VDRData_Read(VdrGprsParseBuffer,&VdrGprsPacketEndSector,&VdrGprsPacketEndStep,Type,OldestTime,VdrGprsStartTime, 1);//�յ�
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
    if((0 == VdrGprsPacketEndList)&&(0 != VdrGprsPacketStartList))//�����ĳ�ʼʱ��̫С
    {
      VdrGprsPacketEndList = 1;
    }

    VdrGprsPacketEndNum = VdrGprsPacketEndList/DataPacketList[Type]+1;
    
    VdrGprsPacketStartNum = VdrGprsPacketStartList/DataPacketList[Type]+1;
    
    VdrGprsPacketNum = VdrGprsPacketStartNum;
        
  }
  else//û�д�ʱ�������ֻ��������
  {
    VdrGprsPacketStartNum = VDRData_ReadTotalPacket(Type);
    VdrGprsPacketEndNum = 1;
    VdrGprsPacketNum = VdrGprsPacketStartNum;
  }
          
  if(VdrGprsPacketStartNum < VdrGprsPacketEndNum)//����
    {
      VdrGprsPacketStartNum = 0;
      VdrGprsPacketEndNum = 0;
      VdrGprsPacketNum = 0;
    }
  
}













