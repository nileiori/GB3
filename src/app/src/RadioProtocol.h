#ifndef __RADIO_PROTOCOL_H
#define __RADIO_PROTOCOL_H

//*********�ļ�����************
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "rtc.h"
#include "main.h"

//********�궨��****************
#define	MAX_PROTOCOL_FUN_LIST		(43+5)//Modify By Shigle 2013-04-01

#define DEVICE_LOGIN_PASSWORD_LEN	20
#define LOGIN_PASSWORD_LEN		22
#define RADIO_PROTOCOL_RESEND_STEP_LEN	100
#define	RADIO_PROTOCOL_BUFFER_SIZE	1200
#define	RADIO_PROTOCOL_RX1_BUFFER_SIZE	1200
#define	RADIO_PROTOCOL_RX2_BUFFER_SIZE	1200
#define	RADIO_PROTOCOL_RX_SMS_BUFFER_SIZE	400
#define RADIOPROTOCOL_RESEND_BUFFER_SIZE  1100//ֻ�洢�����ط���ָ��,ÿһ��100�ֽ�

//*****************�Զ������ID******************
#define SELF_DEFINE_PRAMID_BASE		0x0200
#define DIAL_POINT_NAME_PRAMID		(SELF_DEFINE_PRAMID_BASE+1)//���ŵ����Ʋ���ID,�Զ���
#define DIAL_USER_NAME_PRAMID		(SELF_DEFINE_PRAMID_BASE+2)//�����û�������ID,�Զ���
#define DIAL_PASSWORD_PRAMID		(SELF_DEFINE_PRAMID_BASE+3)//�����������ID,�Զ���
#define WIRELESS_UPDATA_IP_PRAMID	(SELF_DEFINE_PRAMID_BASE+4)//��ַ����ID,�Զ���
#define WIRELESS_UPDATA_PORT_PRAMID	(SELF_DEFINE_PRAMID_BASE+5)//�˿ڲ���ID,�Զ���
#define WIRELESS_UPDATA_HARDWARE_PRAMID	(SELF_DEFINE_PRAMID_BASE+6)//Ӳ���汾����ID,�Զ���
#define WIRELESS_UPDATA_SOFTWARE_PRAMID	(SELF_DEFINE_PRAMID_BASE+7)//�̼��汾����ID,�Զ���
#define WIRELESS_UPDATA_URL_PRAMID	(SELF_DEFINE_PRAMID_BASE+8)//URL����ID,�Զ���
#define WIRELESS_UPDATA_TIME_PRAMID	(SELF_DEFINE_PRAMID_BASE+9)//ʱ�޲���ID,�Զ���
//#define DIAL_POINT_NAME_PRAMID		(SELF_DEFINE_PRAMID_BASE+10)//���ŵ����Ʋ���ID,�Զ���
//#define DIAL_USER_NAME_PRAMID		(SELF_DEFINE_PRAMID_BASE+11)//�����û�������ID,�Զ���
//#define DIAL_PASSWORD_PRAMID		(SELF_DEFINE_PRAMID_BASE+12)//�����������ID,�Զ���
#define CONTROL_CONNECT_IP_PRAMID	(SELF_DEFINE_PRAMID_BASE+13)//��ַ����ID,�Զ���
#define CONTROL_CONNECT_PORT_PRAMID	(SELF_DEFINE_PRAMID_BASE+14)//�˿ڲ���ID,�Զ���
#define CONTROL_CONNECT_HARDWARE_PRAMID	(SELF_DEFINE_PRAMID_BASE+15)//Ӳ���汾����ID,�Զ���
#define CONTROL_CONNECT_SOFTWARE_PRAMID	(SELF_DEFINE_PRAMID_BASE+16)//�̼��汾����ID,�Զ���
#define CONTROL_CONNECT_URL_PRAMID	(SELF_DEFINE_PRAMID_BASE+17)//URL����ID,�Զ���
#define CONTROL_CONNECT_TIME_PRAMID	(SELF_DEFINE_PRAMID_BASE+18)//ʱ�޲���ID,�Զ���

//Э��������������Ӧ������
typedef enum {ACK_OK = 0, ACK_ERROR, FORMAT_ERROR, NOACK = 0xFF}ProtocolACK;



#define UPDATA_PACKET_SIZE_MAX 1024
typedef struct{
  
u8 Type;
u8 ManufacturerID[5];
u8 VersionLength;
u8 StrVersion[20];
u32 PacketLength;
u8  PacketBuffer[ UPDATA_PACKET_SIZE_MAX ];

}TERMINAL_UPDATA_PACKET;


/********P23 �����ְ����� �ṹ��************/
typedef struct{
  
u16 OriginalMsgSerialNum;
u8  ReUploadPacketNum;
u16 ReUploadPacketIDList[2*255];

}STRUCT_UPLOADFROMBREAK; 
        
        

typedef struct{
	u16 	MessageID;//��ϢID
	u16 	Attribute;//��Ϣ������
	u8 	PhoneNum[6];//�ֻ���,BCD��
	u16 	SerialNum;//��Ϣ��ˮ��
	u16 	TotalPackage;//��Ϣ�ܰ���
	u16 	CurrentPackage;//��ǰ����ţ���1��ʼ
}MESSAGE_HEAD;

typedef struct{
	u16	List[20];//��������б�,��10��,û������ʱΪ0;��������ǰ��0
	u8	ListCount;//��������Э��ָ������
	u8	EnterCount;//����Э���������Ĵ���;��������ǰ��0
}RADIO_PARSE_LIST;

//***********��������**************
//*****************��һ���֣���׼��ƽ̨�·�ָ��**********************
//*******************************************************************
void Clear_SmsChannelSerialNum(void);

/*********************************************************************
//��������	:RadioProtocol_ServerGeneralAck(u8 *pBuffer, u16 BufferLen)
//����		:ƽ̨����������ͨ��Ӧ��
//��ע		:ָ��ID��0x8001
*********************************************************************/
ProtocolACK RadioProtocol_ServerGeneralAck(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TerminalRegisterAck(u8 *pBuffer, u16 BufferLen)
//����		:�ն�ע��Ӧ��
//��ע		:ָ��ID��0x8100
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRegisterAck(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_PramSet(u8 *pBuffer, u16 BufferLen)
//����		:�����ն˲���
//��ע		:ָ��ID��0x8103
*********************************************************************/
ProtocolACK RadioProtocol_PramSet(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_WritePram
//����		:д����
//��ע		:����д����������д�������ô˺������ú�����0x8103�������
*********************************************************************/
ProtocolACK RadioProtocol_WritePram(u8 channel,u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_PramSearch(u8 *pBuffer, u16 BufferLen)
//����		:��ѯ�ն˲���
//��ע		:ָ��ID��0x8104
*********************************************************************/
ProtocolACK RadioProtocol_PramSearch(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_ReadPram
//����		:������
//����		:����Ҫ���Ĳ���ID�ŷ��뻺��pIDBuffer�У�ÿ��ID���ֽڣ�����ǰ
//����		:��ȡ�Ĳ����ܸ�����*pDstBuffer��ID�ţ�ID���ȣ�ID���ݵ�˳���ţ�*DstBufferLen�ܵĶ�ȡ���ȣ������������ܸ����ֽ�
//��ע		:ReadType:0Ϊ����(����)��ÿ��ֻ�ܶ�550�ֽڣ�1Ϊ���ߣ�GPRS����ÿ�οɶ�1000�ֽ�
*********************************************************************/
u8 RadioProtocol_ReadPram(u8 *pDstBuffer, s16 *DstBufferLen, u8 *pIDBuffer, s16 IDBufferLen,u8 ReadType);
/*********************************************************************
//��������	:RadioProtocol_TerminalControl(u8 *pBuffer, u16 BufferLen)
//����		:�ն˿���
//��ע		:ָ��ID��0x8105
*********************************************************************/
ProtocolACK RadioProtocol_TerminalControl(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_PositionSearch(u8 *pBuffer, u16 BufferLen)
//����		:λ����Ϣ��ѯ
//��ע		:ָ��ID��0x8201
*********************************************************************/
ProtocolACK RadioProtocol_PositionSearch(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TempPositionControl(u8 *pBuffer, u16 BufferLen)
//����		:��ʱλ�ø��ٿ���
//��ע		:ָ��ID��0x8202
*********************************************************************/
ProtocolACK RadioProtocol_TempPositionControl(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TextMessageDownload(u8 *pBuffer, u16 BufferLen)
//����		:�ı���Ϣ�·�
//��ע		:ָ��ID��0x8300
*********************************************************************/
ProtocolACK RadioProtocol_TextMessageDownload(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_EvenSet(u8 *pBuffer, u16 BufferLen)
//����		:�¼�����
//��ע		:ָ��ID��0x8301
*********************************************************************/
ProtocolACK RadioProtocol_EvenSet(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_QuestionDownload(u8 *pBuffer, u16 BufferLen)
//����		:�����·�
//��ע		:ָ��ID��0x8302
*********************************************************************/
ProtocolACK RadioProtocol_QuestionDownload(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_InformationOrderSet(u8 *pBuffer, u16 BufferLen)
//����		:��Ϣ�㲥�˵�����
//��ע		:ָ��ID��0x8303
*********************************************************************/
ProtocolACK RadioProtocol_InformationOrderSet(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_InformationService(u8 *pBuffer, u16 BufferLen)
//����		:��Ϣ����
//��ע		:ָ��ID��0x8304
*********************************************************************/
ProtocolACK RadioProtocol_InformationService(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TelephoneCallback(u8 *pBuffer, u16 BufferLen)
//����		:�绰�ز�
//��ע		:ָ��ID��0x8400
*********************************************************************/
ProtocolACK RadioProtocol_TelephoneCallback(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TelephoneBookSet(u8 *pBuffer, u16 BufferLen)
//����		:���õ绰��
//��ע		:ָ��ID��0x8401
*********************************************************************/
ProtocolACK RadioProtocol_TelephoneBookSet(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_CarControl(u8 *pBuffer, u16 BufferLen)
//����		:��������
//��ע		:ָ��ID��0x8500
*********************************************************************/
ProtocolACK RadioProtocol_CarControl(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_SetRoundArea(u8 *pBuffer, u16 BufferLen)
//����		:����Բ������
//��ע		:ָ��ID��0x8600
*********************************************************************/
ProtocolACK RadioProtocol_SetRoundArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DelRoundArea(u8 *pBuffer, u16 BufferLen)
//����		:ɾ��Բ������
//��ע		:ָ��ID��0x8601
*********************************************************************/
ProtocolACK RadioProtocol_DelRoundArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_SetRectangleArea(u8 *pBuffer, u16 BufferLen)
//����		:���þ�������
//��ע		:ָ��ID��0x8602
*********************************************************************/
ProtocolACK RadioProtocol_SetRectangleArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DelRectangleArea(u8 *pBuffer, u16 BufferLen)
//����		:ɾ����������
//��ע		:ָ��ID��0x8603
*********************************************************************/
ProtocolACK RadioProtocol_DelRectangleArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_SetPolygonArea(u8 *pBuffer, u16 BufferLen)
//����		:���ö��������
//��ע		:ָ��ID��0x8604
*********************************************************************/
ProtocolACK RadioProtocol_SetPolygonArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DelPolygonArea(u8 *pBuffer, u16 BufferLen)
//����		:ɾ�����������
//��ע		:ָ��ID��0x8605
*********************************************************************/
ProtocolACK RadioProtocol_DelPolygonArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_SetRoad(u8 *pBuffer, u16 BufferLen)
//����		:����·��
//��ע		:ָ��ID��0x8606
*********************************************************************/
ProtocolACK RadioProtocol_SetRoad(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DelRoad(u8 *pBuffer, u16 BufferLen)
//����		:ɾ��·��
//��ע		:ָ��ID��0x8607
*********************************************************************/
ProtocolACK RadioProtocol_DelRoad(u8 channel, u8 *pBuffer, u16 BufferLen);

/*********************************************************************
//��������	:RadioProtocol_SetMuckDumpArea
//����		:���������㵹����(0x860a)
//��ע		:ָ��ID��0x860a
*********************************************************************/
ProtocolACK RadioProtocol_SetMuckDumpArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DeleteMuckDumpArea
//����		:ɾ�������㵹����(0x860b)
//��ע		:ָ��ID��0x860b
*********************************************************************/
ProtocolACK RadioProtocol_DeleteMuckDumpArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_QueryMuckDumpArea(u8 *pBuffer, u16 BufferLen)
//����		:ɾ�������㵹����
//��ע		:ָ��ID��0x860c
*********************************************************************/
ProtocolACK RadioProtocol_QueryMuckDumpArea(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_RecorderDataCollection(u8 *pBuffer, u16 BufferLen)
//����		:��ʻ��¼����Ϣ�ɼ�
//��ע		:ָ��ID��0x8700
*********************************************************************/
ProtocolACK RadioProtocol_RecorderDataCollection(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_RecorderPramSet(u8 channel, u8 *pBuffer, u16 BufferLen)
//����		:��ʻ��¼�ǲ����´�
//��ע		:ָ��ID��0x8701
*********************************************************************/
ProtocolACK RadioProtocol_RecorderPramSet(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������  :RadioProtocol_DriverInformationUploadRequest(u8 channel, u8 *pBuffer, u16 BufferLen)
//����      :�ϱ���ʻԱ�����Ϣ����
//��ע      :ָ��ID��0x8702
*********************************************************************/
ProtocolACK RadioProtocol_DriverInformationUploadRequest(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_MultiMediaUploadAck(u8 *pBuffer, u16 BufferLen)
//����		:��ý�������ϴ�Ӧ��
//��ע		:ָ��ID��0x8800
*********************************************************************/
ProtocolACK RadioProtocol_MultiMediaUploadAck(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_CameraPhoto(u8 *pBuffer, u16 BufferLen)
//����		:����ͷ��������
//��ע		:ָ��ID��0x8801
*********************************************************************/
ProtocolACK RadioProtocol_CameraPhoto(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_MemoryIndex(u8 *pBuffer, u16 BufferLen)
//����		:�洢��ý�����ݼ���
//��ע		:ָ��ID��0x8802
*********************************************************************/
ProtocolACK RadioProtocol_MemoryIndex(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_MemoryUpload(u8 *pBuffer, u16 BufferLen)
//����		:�洢��ý�������ϴ�
//��ע		:ָ��ID��0x8803
*********************************************************************/
ProtocolACK RadioProtocol_MemoryUpload(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_StartSoundRecord(u8 *pBuffer, u16 BufferLen)
//����		:��ʼ¼��
//��ע		:ָ��ID��0x8804
*********************************************************************/
ProtocolACK RadioProtocol_StartSoundRecord(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_SingleMemoryIndex(u8 *pBuffer, u16 BufferLen)
//����		:�����洢��ý�����ݼ����ϴ�
//��ע		:ָ��ID��0x8805
*********************************************************************/
ProtocolACK RadioProtocol_SingleMemoryIndex(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_OriginalDataDownTrans(u8 *pBuffer, u16 BufferLen)
//����		:��������͸��
//��ע		:ָ��ID��0x8900
*********************************************************************/
ProtocolACK RadioProtocol_OriginalDataDownTrans(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_ServerRSA(u8 *pBuffer, u16 BufferLen)
//����		:ƽ̨��Կ
//��ע		:ָ��ID��0x8A00
*********************************************************************/
ProtocolACK RadioProtocol_ServerRSA(u8 channel, u8 *pBuffer, u16 BufferLen);

//*****************�ڶ����֣���׼���ն��Ϸ�ָ��**********************
//*******************************************************************
/*********************************************************************
//��������	:RadioProtocol_TerminalGeneralAck(u16 SerialNum, u16 MessageID, u8 Result)
//����		:�ն�ͨ��Ӧ��
//��ע		:ָ��ID��0x0001
*********************************************************************/
ProtocolACK RadioProtocol_TerminalGeneralAck(u8 channel, u8 AckResult);
/*********************************************************************
//��������	:RadioProtocol_TerminalHeartbeat(void)
//����		:�ն�����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע		:ָ��ID��0x0002
*********************************************************************/
ProtocolACK RadioProtocol_TerminalHeartbeat(u8 channel,u8 *pBuffer, u8 *SmsPhone );
/*********************************************************************
//��������	:RadioProtocol_TerminalLogout(void)
//����		:�ն�ע��
//��ע		:ָ��ID��0x0003
*********************************************************************/
ProtocolACK RadioProtocol_TerminalLogout(u8 channel);
/*********************************************************************
//��������  :RadioProtocol_ResendPacketRequest(void)
//����      :�����ְ�����
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x8003��ƽ̨���ն˾�ʹ�ø�ָ��������ְ�����
*********************************************************************/
ProtocolACK RadioProtocol_ResendPacketRequest(u8 channel,u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TerminalRegister(void)
//����		:�ն�ע��
//��ע		:ָ��ID��0x0100
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRegister(u8 channel);
/*********************************************************************
//��������	:RadioProtocol_TerminalQualify(void)
//����		:�ն˼�Ȩ
//��ע		:ָ��ID��0x0102
*********************************************************************/
ProtocolACK RadioProtocol_TerminalQualify(u8 channel);
/*********************************************************************
//��������	:DownloadUpdatePacket_Result
//��������	:�ն��������֪ͨ
//��ע		:ָ��ID��0x0108
*********************************************************************/
extern ProtocolACK DownloadUpdatePacket_Result( u8 channel, u8 UpdataType , u8 AckResult );
/*********************************************************************
//��������	:RadioProtocol_OpenAccount(void)
//����		:�ն˿���
//����		:  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע		:ָ��ID��0x0110
*********************************************************************/
ProtocolACK RadioProtocol_OpenAccount(u8 channel);
/*********************************************************************
//��������	:RadioProtocol_FirmwareUpdataResultNote(u8 *pBuffer, u16 BufferLen)
//����		:Զ�̹̼��������֪ͨ
//����		:  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע		:ָ��ID��0x0108
*********************************************************************/
ProtocolACK RadioProtocol_FirmwareUpdataResultNote(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_PostionInformationReport(u8 *pBuffer, u16 length)
//����		:λ����Ϣ�㱨
//��ע		:ָ��ID��0x0200
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformationReport(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_EvenReport(u8 EvenID)
//����		:�¼�����
//��ע		:ָ��ID��0x0301
*********************************************************************/
ProtocolACK RadioProtocol_EvenReport(u8 channel, u8 EvenID);
/*********************************************************************
//��������	:RadioProtocol_QuestionAck(u8 AnswerID)
//����		:����Ӧ��
//��ע		:ָ��ID��0x0302
*********************************************************************/
ProtocolACK RadioProtocol_QuestionAck(u8 channel, u8 AnswerID);
/*********************************************************************
//��������	:RadioProtocol_InforamtionOrder(u8 InformationType, u8 ControlFlg)
//����		:��Ϣ�㲥
//��ע		:ָ��ID��0x0303
*********************************************************************/
ProtocolACK RadioProtocol_InforamtionOrder(u8 channel, u8 InformationType, u8 ControlFlg);
/*********************************************************************
//��������	:RadioProtocol_RecorderDataReport(u8 *pBuffer, u8 BufferLen)
//����		:��ʻ��¼�������ϴ�
//��ע		:ָ��ID��0x0700
*********************************************************************/
ProtocolACK RadioProtocol_RecorderDataReport(u8 channel, u8 *pBuffer, u16 BufferLen, u16 TotalPacket, u16 Packet, u8 SerialNumType);
/*********************************************************************
//��������	:RadioProtocol_ElectricReceiptReport(u8 *pBuffer, u8 BufferLen)
//����		:�����˵��ϱ�
//��ע		:ָ��ID��0x0701
*********************************************************************/
ProtocolACK RadioProtocol_ElectricReceiptReport(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DriverInformationReport(u8 *pBuffer, u8 BufferLen)
//����		:��ʻԱ�����Ϣ�ϱ�
//��ע		:ָ��ID��0x0702
*********************************************************************/
ProtocolACK RadioProtocol_DriverInformationReport(u8 channel, u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_PostionInformation_BulkUpTrans(u8 *pBuffer, u16 length)
//����		:��λ���������ϴ�
//����		:pBuffer:ָ�����ݣ�λ����Ϣ��ָ��;BufferLen:���ݵĳ���;
//����		: �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע		: ָ��ID��0x0704
//�μ�          : ҳ�� P45  <<��·���䳵�����Ƕ�λϵͳ�������ݳ����ն�ͨѶЭ�鼼���淶2013-01��>> 
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformation_BulkUpTrans(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_MultiMediaEvenReport(u32 DataID, u8 Type, u8 Format, u8 Even, u8 ChID)
//����		:��ý���¼���Ϣ�ϴ�
//��ע		:ָ��ID��0x0800
*********************************************************************/
ProtocolACK RadioProtocol_MultiMediaEvenReport(u8 channel, u32 DataID, u8 Type, u8 Format, u8 Even, u8 ChID);
/*********************************************************************
//��������	:RadioProtocol_MultiMediaDataReport(u8 *pBuffer, u16 BufferLen)
//����		:��ý�������ϴ�
//��ע		:  ָ��ID��0x0801
*********************************************************************/
ProtocolACK RadioProtocol_MultiMediaDataReport(u8 channel, u8 *pBuffer, u16 BufferLen, u16 TotalPacket, u16 Packet);
/*********************************************************************
//��������	:RadioProtocol_MemoryIndexAck(u8 *pBuffer, u16 BufferLen)
//����		:�洢��ý�����Ӧ��
//����		:pBuffer:ָ�����ݣ���Ҫ������ý��ID;��ý������;ͨ��ID;�¼������;λ����Ϣ�㱨;
//��ע		:  ָ��ID��0x0802
*********************************************************************/
ProtocolACK RadioProtocol_MemoryIndexAck(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_PhotoAck(u8 *pBuffer, u16 BufferLen)
//����		:����ͷ��������Ӧ��
//����		:  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע		:  ָ��ID��0x0805
*********************************************************************/
ProtocolACK RadioProtocol_PhotoAck(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_JiutongPhoto(u8 *pBuffer, u16 BufferLen)
//����		:����ͷ��������(��ͨЭ��)
//��ע		:ָ��ID��0x8811
*********************************************************************/
ProtocolACK RadioProtocol_JiutongPhoto(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_OriginalDataUpTrans(u8 Type, u32 TotalLen, u8 *pBuffer, u16 BufferLen)
//����		:��������͸��
//��ע		:ָ��ID��0x0900
*********************************************************************/
ProtocolACK RadioProtocol_OriginalDataUpTrans(u8 channel, u8 Type, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_DataCompressUpTrans(u8 *pBuffer, u16 BufferLen)
//����		:����ѹ���ϴ�
//��ע		:ָ��ID��0x0901
*********************************************************************/
ProtocolACK RadioProtocol_DataCompressUpTrans(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_TerminalRSA(u32 RSAe, u32 RSAn)
//����		:�ն�RSA��Կ
//��ע		:ָ��ID��0x0A00
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRSA(u8 channel, u32 RSAe, u8 *pRSAn);

//*****************�������֣��Զ����ƽ̨�·�ָ��**********************
//*********************************************************************


//*****************���Ĳ��֣��Զ�����ն��Ϸ�ָ��**********************
//*********************************************************************

//*****************���岿�֣�����ӿں���******************************
//********************************************************************
/*********************************************************************
//��������	:Get_Conncet_State(void)
//����		:�����ն˵绰����
//��ע		:������ӵ�ָ����������׼
*********************************************************************/
u8 Get_Conncet_State(void);
void Clear_Conncet_State(void);


/*********************************************************************
//��������	:RadioProtocol_UpdateTerminalPhoneNum(void)
//����		:�����ն˵绰����
//��ע		:�ϵ��ʼ��ʱ�����
*********************************************************************/
void RadioProtocol_UpdateTerminalPhoneNum(void);
/*********************************************************************
//��������	:RadioProtocol_GetRadioProtocolRxBufferBusyFlag(void)
//����		:�����ջ���æ��־
//��ע		:ͨ��ģ����յ����ݺ�����ô˺���,�ж��Ƿ�����Э����ջ���RadioProtocolRxBuffer
//		:�������ݣ�������ֵΪ0��ʾ���п�����Э����ջ��壬Ϊ1��ʾ��æ����ǰ���յ����ݶ���
*********************************************************************/
u8  RadioProtocol_GetRadioProtocolRxBufferBusyFlag(void);
/*********************************************************************
//��������	:RadioProtocol_SetRadioProtocolRxBufferBusyFlag
//����		:��λRadioProtocolRxBufferBusyFlag
//��ע		:����Э����ջ��忽�������ݺ�����λЭ����ջ���æ��׼,
//		:ֻ�е�Э����ջ���������ݶ����������ñ�׼��ϵͳ�Զ���0
*********************************************************************/
void RadioProtocol_SetRadioProtocolRxBufferBusyFlag(void);
/*********************************************************************
//��������	:RadioProtocol_AddRadioParseList(void)
//����		:������ݵ����߽����б�
//��ע		:����Э����ջ��忽�������ݺ�����ô˺���,���½��յ�������
//		:������ͬʱ�յ��ü�������,����Ҫ��һ�����У��Ǽǽ������б�,
//		:Ȼ�������߽�����ʱ����
*********************************************************************/
void RadioProtocol_AddRadioParseList(void);
/*********************************************************************
//��������	:RadioProtocol_ParseTimeTask(void)
//����		:���߽���ʱ������
//��ע		:1���ӵ���1�Σ����ϲ��ҽ����б����б��л���δ���������ݣ�
//		:����ý����������н�������û�У���رո�����
*********************************************************************/
FunctionalState RadioProtocolParse_TimeTask(void);
/*********************************************************************
//��������	:RadioProtocol_SwitchChannel
//����		:�л�ͨ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:
//��ע		:CHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2����ͨ�������л�
*********************************************************************/
u8 RadioProtocol_SwitchChannel(u8 channel);
/*********************************************************************
//��������	:RadioProtocol_SendMessage(MESSAGE_HEAD	head, u8 *SrcData, u16 SrcLen, u8 SendCh)
//����		:������Ϣ
//����		:MESSAGE_HEAD	head:��Ϣͷ;SrcData:ָ����Ϣ���ָ��;
//		    :SrcLen:��Ϣ�峤��;SendCh:����ͨ��,��ѡֵֻ��ΪCHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2,
//          :������������ֵ�����
//����		:bit0��Ӧ��CHANNEL_SMSͨ��,bit1��Ӧ��CHANNLE_DATA_1ͨ��,bit2��Ӧ��CHANNLE_DATA_2ͨ��,
//          :����bit��ʱ����,��Ӧ��λΪ1��ʾ��ͨ�����ͳɹ���
//��ע		:������һ��ָ���ͨ����ҪӦ��Ӧ��ʱ����øú�����
//		:�����Ϸ�һ����Ϣ����ʱҲ��Ҫ���øú�����
*********************************************************************/
u8 RadioProtocol_SendCmd(u8 SendCh , MESSAGE_HEAD	*head, u8 *SrcData, u16 SrcLen, u8 *pSmsPhone);
/*********************************************************************
//��������	:RadioProtocolResend_TimeTask(void)
//����		:�����ط���ʱ����
//��ע		:1���ӵ���1�Σ�ֱ�����ͳɹ�����ɺ���Զ��رա�
*********************************************************************/
FunctionalState RadioProtocolResend_TimeTask(void);
/*********************************************************************
//��������	:RadioProtocol_PositionPacketSendTimeTask(void)
//����		:λ����Ϣ����ϴ��������ڶ�λ���Ȳ��ԣ��б����޸�
//��ע		:0.5�����1��
*********************************************************************/
FunctionalState RadioProtocol_PositionPacketSendTimeTask(void);
//*****************�������֣��ڲ��ӿں���******************************
//********************************************************************
/*********************************************************************
//��������	:RadioProtocol_ProtocolParse(u8 channel, u8 *pBuffer, u16 BufferLen)
//����		:Э�����,����һ��ָ��
//��ע		:ƽ̨�·�������������ɸú����������ú������������
//		:��7E��ͷ,7E��β,����ת�崦���һ���ֽ�����
*********************************************************************/
ErrorStatus RadioProtocol_ProtocolParse(u8 channel, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_GetDownloadCmdChannel(u16 DownloadCmd, u16 *SerialNum, u8 *channel)
//����		:��ȡ�����������ˮ�ţ�ͨ����
//����		:0����1�ɹ�
//��ע		:ֻ�ܻ�ȡ���µ������������ˮ�ţ�ͨ����
*********************************************************************/
u8 RadioProtocol_GetDownloadCmdChannel(u16 DownloadCmd, u16 *SerialNum);
/*********************************************************************
//��������	:RadioProtocol_GetUploadCmdChannel(u16 UploadCmd, u16 *SerialNum, u8 *channel)
//����		:��ȡ�����������ˮ�ţ�ͨ����
//����		:0����1�ɹ�
//��ע		:ֻ�ܻ�ȡ���µ������������ˮ�ţ�ͨ����
*********************************************************************/
u8 RadioProtocol_GetUploadCmdChannel(u16 UploadCmd, u16 *SerialNum);
/*********************************************************************
//��������	:RadioProtocol_UpdateDownloadCmdList(u16 Cmd, u16 SerialNum, u8 channel)
//����		:�������������б�
//��ע		:������ǰ�����ˮ�ţ�ͨ���ű����������Ա����ʹ��
*********************************************************************/
ErrorStatus RadioProtocol_UpdateDownloadCmdList(u16 Cmd, u16 SerialNum, u8 channel);
/*********************************************************************
//��������	:RadioProtocol_UpdateUploadCmdList(u16 Cmd, u16 SerialNum, u8 channel)
//����		:�������������б�
//��ע		:������ǰ�����ˮ�ţ�ͨ���ű����������Ա����ʹ��
*********************************************************************/
ErrorStatus RadioProtocol_UpdateUploadCmdList(u16 Cmd, u16 SerialNum, u8 channel);
/*********************************************************************
//��������	:unTransMean
//����		:ʵ�ַ�ת�幦��
//		:0x7e----->0x7d���һ��0x02;0x7d----->0x7d���һ��0x01
//����		:����ǰ�ֽ���������ǰ�ֽ�������
//���		:�������ֽ���
//����		:ת��������ֽ�������
//��ע		:Э�������������ô˺������Ƚ��з�ת�壬Ȼ���ٽ�����
*********************************************************************/
u16 unTransMean(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD *head)
//����		:����Ϣͷ����ֽ���
//��ע		:
*********************************************************************/
u8 RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD *head);
/*********************************************************************
//��������	:TransMean
//����		:ʵ��ת�幦��
//		:0x7e----->0x7d���һ��0x02;0x7d----->0x7d���һ��0x01
//����		:ת��ǰ�ֽ�����ת��ǰ�ֽ�������
//���		:ת����ֽ���
//����		:ת����ֽ�������
//��ע		:
*********************************************************************/
u16 TransMean(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//�������� :RadioProtocol_OriginalDataUpTransEx(u8 Type, u8 *pBuffer, u16 BufferLen)
//���� :��������͸��,����Ϣ��ˮ��
//��ע :
*********************************************************************/
ProtocolACK RadioProtocol_OriginalDataUpTransEx(u8 Type, u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//��������	:RadioProtocol_IsBigEndian(void)
//����		:���������ֽ����Ƿ�Ϊ���ϵͳ
//��ע		:
*********************************************************************/
u8 RadioProtocol_IsBigEndian(void);
/*********************************************************************
//��������	:RadioProtocol_ntohs(u16 value)
//����		:�����ֽ���ת�����ֽ���
//��ע		:
*********************************************************************/
u16 RadioProtocol_ntohs(u16 value);
/*********************************************************************
//��������	:RadioProtocol_ntoh(u32 value)
//����		:�����ֽ���ת�����ֽ���
//��ע		:
*********************************************************************/
u32 RadioProtocol_ntoh(u32 value);
/*********************************************************************
//��������	:RadioProtocol_htons(u16 value)
//����		:�����ֽ���ת�����ֽ���
//��ע		:
*********************************************************************/
u16 RadioProtocol_htons(u16 value);
/*********************************************************************
//��������	:RadioProtocol_hton(u32 value)
//����		:�����ֽ���ת�����ֽ���
//��ע		:
*********************************************************************/
u32 RadioProtocol_hton(u32 value);
/*********************************************************************
//��������	:NullOperate(u8 *pBuffer, u16 BufferLen)
//����		:�ղ���,�����κ���
//��ע		:
*********************************************************************/
ProtocolACK NullOperate(u8 channel,u8 *pBuffer, u16 BufferLen);

extern u16 RadioProtocol_Packet_ICCard_AuthInfo( u16 MessageID, u8 ExdeviceCode , u8 *pBuffer, u16 BufferLen, u8 *pOutBuf );
u16 IC_Card_Radio_Heartbeat( u8 *pOutBuf );
ProtocolACK RadioProtocol_UploadFromBreak_Request(u8 channel,u8 *pBuffer, u16 BufferLen);
ProtocolACK RadioProtocol_DownloadUpdatePacket(u8 channel, u8 *pBuffer, u16 BufferLen);
ProtocolACK RadioProtocol_AlarmConfirmManual(u8 channel, u8 *pBuffer, u16 BufferLen);
ProtocolACK RadioProtocol_DriverInformationUploadRequest(u8 channel, u8 *pBuffer, u16 BufferLen);
ProtocolACK RadioProtocol_TerminalAttribute(u8 channel,u8 *pBuffer, u16 BufferLen);
ProtocolACK RadioProtocol_UpdataResultNote(u8 channel,u8 UpdataType,u8 AckResult);
extern ProtocolACK  RadioProtocol_CAN_UpTrans(u8 channel,u8 *pBuffer, u16 BufferLen);
void RadioProtocol_AddRecvDataForParse(unsigned char ch,unsigned char *recvData,unsigned short recvDataLen);;

//****************�ⲿʹ�ñ�������*****************

#endif
