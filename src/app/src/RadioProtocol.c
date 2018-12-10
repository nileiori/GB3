
//********************************ͷ�ļ�************************************
#include "stm32f10x.h" 
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR_Gprs.h"
#include "VDR_Usart.h"
#include "Card.h"


extern const u8 ProductID[];// ������ID Bytes:5
extern const u8 TerminalTypeID[]; // �ն��ͺ�ID Bytes:20
extern u16  MemoryUploadSendCount;//���ʹ�������
extern u32  AccOnReportTime;//ȱʡ�㱨ʱ����
extern u16 NavigationReportAckSerialNum1;//��λ����λ����Ϣ�ϱ�Ӧ����ˮ��
extern u16 NavigationReportAckSerialNum2;//��λ����λ����Ϣ�ϱ�Ӧ����ˮ��
extern u8  NavigationReportEnableFlag;//��λ����λ����Ϣ�ϱ�ʹ�ܱ�־,1Ϊʹ��
extern u8  NavigationReportAddEnableFlag;//ʹ�ܿ��أ�1Ϊʹ��
//extern char TerminalVersionSW[];//�ն˹̼��汾�� STRING  Bytes:1
extern u8   BlindReportFlag;//ä���ϱ���־��1ΪҪ���ϱ������ϱ���0Ϊ��Ҫ��
extern u16     BlindAckSerialNum;//ä��Ӧ����ˮ��
extern u8   BlindReportFlag;//ä���ϱ���־��1ΪҪ���ϱ������ϱ���0Ϊ��Ҫ��
extern s16  BlindReportSector;//ä������������
extern s16  BlindReportStep;//ä�������Ĳ���
//extern HISTORY_AREA_STATUS  BlindStatus;
extern u8   BlindDelayCount;//ä����ʱ����,�������ﵽ20��ʱ������һ������ǰ���ط�1�Σ���ˮ�Ż��1��
//extern u8  SubpacketUploadType;//�ְ���������,1:��ʻ�ٶ�
// 2:λ����Ϣ��¼;3:�¹��ɵ��¼;4:��ʱ��ʻ��¼;5:��ʻ����ݼ�¼;6:�ⲿ�����¼;
// 7:�����޸ļ�¼;8:�ٶ�״̬��־
//extern u16 SubpackageTotalPacket;//�ְ����ܰ���
extern u8 GprsOneSendOverFlg;
extern u8 FirmwareVersion[];//�̼��汾��,�̶�Ϊ5�ֽ�
extern u32 ResetTimeCount;//��λ�����������յ�ƽ̨Ӧ�����0������1���ۼ�1���ﵽ4Сʱ��λ����
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
extern Queue  VdrTxQ;//VDR���Ͷ���
extern u8 CardDriverWorkFlag;//��ʻԱ�ϰ��־��1Ϊ�ϰ࣬0Ϊ�°�
extern u8 CardDriverInfo[];//};//��ʻԱ����ϱ���Ϣ�壨0x0702���еļ�ʻԱ�������ȡ���ʻԱ��������ҵ�ʸ�֤���롢��֤�������Ƴ��ȡ���֤�������ơ�֤����Ч��
extern u8 CardDriverInfoLen;
extern u8 CardDriverWorkStartTime[];//��ʻԱ����ϱ���Ϣ�壨0x0702���е�״̬(�̶�Ϊ1)��ʱ�䣬IC����ȡ�������8�ֽ�
extern u8 CardDriverWorkEndTime[];//��ʻԱ����ϱ���Ϣ�壨0x0702���е�״̬(�̶�Ϊ2)��ʱ�䣬��7�ֽ�
extern u32 PhotoID_0800;//ר������0800��ý���¼��ϱ�
extern u8 MediaType_0800;
extern u8 MediaFormat_0800;
extern u8 MediaEvenCode_0800;
extern u8 MediaChannelID_0800;
extern u8 MediaEventSendChannel_0800;
extern u8 PhotoMediaEvUploadFlag;
extern u8  AreaInOutAlarmClearEnableFlag;//1Ϊʹ�ܣ�0Ϊ��ֹ
//const char TerminalVersionSW[] = "102";//�ն˹̼��汾�� STRING  Bytes:1

u8 TerminalVersionHW[] = "100"; //�ն�Ӳ���汾�� STRING
u8    LawlessAccAlarmEnableFlag = 1;//�Ƿ���𱨾�ʹ�ܱ�־��Ĭ��ʹ�ܣ��յ��˹�ȷ�ϱ�����ʹ��
u8    LogoutFlag = 0;//�ն�ע����־

//static ProtocolACK RadioProtocol_ParamQueryAll(u8 *pBuffer, u16 BufferLen);
u8  Uart1DataDownTransFlag = 0;//����1����͸����־��
u16 Uart1DataDownTransCmd = 0;//����1����͸�������е�������
u8  UpdataResultNoteAckFlag = 0;//�������֪ͨӦ���־,1Ϊ�յ�ƽ̨��ͨ��Ӧ����
u8  Connect_To_Specific_Server =0;  
u16 Link1FirstPackageSerialNum = 0;//�ְ�����ʱ��1������ˮ��
u8 CarControlFlag = 0;//�������Ʊ�־
//*******************************Ӧ����*********************************
const u8    CmdOperationIndex[11] = { 0,  4,  13, 17, 22, 24, 25, 33, 36, 44, 45 };//Modify By Shigle 2013-04-01
const u8    IDLowByteMax[11] = {      3,  8,  3,  4,  1,  0,  7,  2,  7,  0,  0 };//Modify By Shigle 2013-04-01

ProtocolACK (* CmdOperate[MAX_PROTOCOL_FUN_LIST])(u8 channel, u8 *pBuffer, u16 BufferLen) = {
  
        
    NullOperate,                // 0,0x8000 ,�ղ���                     
    RadioProtocol_ServerGeneralAck,     // 1,0x8001 , P10 ƽ̨ͨ��Ӧ��,
    NullOperate,                // 2,0x8002 , P�ղ���//Add By Shigle 2013-04-01     
    RadioProtocol_UploadFromBreak_Request,  // 3,0x8003 , P10 �����ְ���������,0x8003//Add By Shigle 2013-04-01


    RadioProtocol_TerminalRegisterAck,  // 4,0x8100 ; P11 �ն�ע��Ӧ��,
    NullOperate,                // 5,0x8101 ; �ղ��� , 
    NullOperate,                // 6,0x8102 ; �ղ��� , 
    RadioProtocol_PramSet,          // 7,0x8103  ; P12 �����ն˲���,
    RadioProtocol_PramSearch,               // 10,0x8104 ; P20 �ղ���, //Add By Shigle 2013-04-01
    RadioProtocol_TerminalControl,      // 9,0x8105  ; P18 �ն˿���,
    RadioProtocol_PramSearch,       // 8,0x8106  ; P18 ��ѯ�ն˲���, //Modify By Shigle 2013-04-01
    RadioProtocol_TerminalAttribute,    // 11,0x8107 ; P20 ��ѯ�ն�����,//Add By Shigle 2013-04-01
    RadioProtocol_DownloadUpdatePacket, // 12,0x8108 ; P21 �·��ն����İ�,//Add By Shigle 2013-04-01
        
        
    NullOperate,                // 13,0x8200 ; �ղ���
    RadioProtocol_PositionSearch,       // 14,0x8201 ; P26 λ����Ϣ��ѯ
    RadioProtocol_TempPositionControl,  // 15,0x8202 ; P27 ��ʱλ�ø��ٿ���
    RadioProtocol_AlarmConfirmManual,       // 16,0x8203 ; P27 �˹�ȷ�˱���  //Add By Shigle 2013-04-01



    RadioProtocol_TextMessageDownload,  // 17,0x8300 ; P28 �ı���Ϣ�·�, 
    RadioProtocol_EvenSet,          // 18,0x8301 ; P28 �¼�����,
    RadioProtocol_QuestionDownload,     // 19,0x8302 ; P29 �����·�,
    RadioProtocol_InformationOrderSet,  // 20,0x8303 ; P30 ��Ϣ�㲥�˵�����,
    RadioProtocol_InformationService,   // 21,0x8304 ; P31 ��Ϣ����,



    RadioProtocol_TelephoneCallback,    // 22,0x8400 ;P31 �绰�ز�,
    RadioProtocol_TelephoneBookSet,     // 23,0x8401 ;P31 ���õ绰��,


    RadioProtocol_CarControl,       // 24, 0x8500 ; P32 ��������,


    RadioProtocol_SetRoundArea,     // 25,0x8600 ; P33 ����Բ������,
    RadioProtocol_DelRoundArea,     // 26,0x8601 ; P34 ɾ��Բ������,
    RadioProtocol_SetRectangleArea,     // 27,0x8602 ; P34 ���þ�������,
    RadioProtocol_DelRectangleArea,     // 28,0x8603 ; P35 ɾ����������,
    RadioProtocol_SetPolygonArea,       // 29,0x8604 ; P35 ���ö��������,
    RadioProtocol_DelPolygonArea,       // 30,0x8605 ; P36 ɾ�����������,
    RadioProtocol_SetRoad,          // 31,0x8606 ; P36 ����·��,
    RadioProtocol_DelRoad,          // 32,0x8607 ; P37 ɾ��·��,
    RadioProtocol_RecorderDataCollection,   // 33,0x8700 ; P38 ��ʻ��¼����Ϣ�ɼ�,
    RadioProtocol_RecorderPramSet,      // 34,0x8701 ; P38 ��ʻ��¼�ǲ����´�,
    RadioProtocol_DriverInformationUploadRequest, // 35,0x8702 �ϱ���ʻԱ�����Ϣ����
        
        
    RadioProtocol_MultiMediaUploadAck,  // 36,0x8800 ; P40 ��ý�������ϴ�Ӧ��,
    RadioProtocol_CameraPhoto,      // 37,0x8801 ; P42 ����ͷ��������,
    RadioProtocol_MemoryIndex,      // 38,0x8802 ; P43 �洢��ý�����ݼ���,
    RadioProtocol_MemoryUpload,     // 39,0x8803 ; P43 �洢��ý�������ϴ�,
    RadioProtocol_StartSoundRecord,     // 40,0x8804 ; P44 ��ʼ¼��,
    RadioProtocol_SingleMemoryIndex,    // 41,0x8805 ; P44 �����洢��ý�����ݼ����ϴ�����,
    NullOperate,                // 42,0x8806 ; �ղ���,�Ա�Э����չ��
    RadioProtocol_JiutongPhoto,     // 43,0x8807 ; ��ͨ���գ�����Ϊ0x8811������ʱ����ת��

    RadioProtocol_OriginalDataDownTrans,    // 44,0x8900 ; P44 ��������͸��

    RadioProtocol_ServerRSA,        // 45,0x8A00 ; P45 ƽ̨��Կ

};

#define UPLOAD_CMD_NUM  27
static u16 UploadCmdList[UPLOAD_CMD_NUM][4] = {//���������¼������,��ˮ��,ͨ����
  {0x0001,0,0},//0,�ն�ͨ��Ӧ��
  {0x0002,0,0},//1,�ն�����
  {0x0003,0,0},//2,�ն�ע��
  {0x0100,0,0},//3,�ն�ע��
  {0x0102,0,0},//4,�ն˼�Ȩ
  {0x0104,0,0},//5,��ѯ�ն�Ӧ��
  {0x0107,0,0},//6,��ѯ�ն�����Ӧ��
  {0x0108,0,0},//7,�ն��������֪ͨ,��Ҫ�����ҵ��ģ�鱣������ָ��ͨ����
  {0x0110,0,0},//8,���ٿ���ָ��
  {0x0200,0,0},//9,λ�û㱨
  {0x0201,0,0},//10,λ����Ϣ��ѯӦ��
  {0x0301,0,0},//11,�¼�����
  {0x0302,0,0},//12,����Ӧ��,��Ҫ�����ҵ��ģ�鱣������ָ��ͨ����
  {0x0303,0,0},//13,��Ϣ�㲥/ȡ��
  {0x0500,0,0},//14,��������Ӧ��,��Ҫ�����ҵ��ģ�鱣������ָ��ͨ����
  {0x0700,0,0},//15,��¼�������ϴ�,��Ҫ�����ҵ��ģ�鱣������ָ��ͨ����
  {0x0701,0,0},//16,�����˵��ϱ�
  {0x0702,0,0},//17,��ʻԱ����ϱ�
  {0x0704,0,0},//18,��λ���������ϴ�
  {0x0705,0,0},//19,CAN���������ϴ�
  {0x0800,0,0},//20,��ý���¼���Ϣ�ϴ�,��Ҫ�����ҵ��ģ�鱣������ָ��ͨ����
  {0x0801,0,0},//21,��ý�������ϴ�,��Ҫ�����ҵ��ģ�鱣������ָ��ͨ����
  {0x0802,0,0},//22,�洢��ý�����Ӧ��
  {0x0805,0,0},//23,����ͷ������������Ӧ��
  {0x0900,0,0},//24,��������͸��
  {0x0901,0,0},//25,����ѹ���ϱ�
  {0x0a00,0,0},//26,�ն�RSA��Կ
};
#define DOWNLOAD_CMD_NUM  39
static u16 DownloadCmdList[DOWNLOAD_CMD_NUM][6] = {//���������¼������,��ˮ��,ͨ����
  {0x8001,0,0},//0
  {0x8003,0,0},//1
  {0x8100,0,0},//2
  {0x8103,0,0},//3
  {0x8104,0,0},//4
  {0x8105,0,0},//5
  {0x8106,0,0},//6
  {0x8107,0,0},
  {0x8108,0,0},
  {0x8201,0,0},//9
  {0x8202,0,0},//10
  {0x8203,0,0},
  {0x8300,0,0},
  {0x8301,0,0},
  {0x8302,0,0},
  {0x8303,0,0},
  {0x8304,0,0},
  {0x8400,0,0},
  {0x8401,0,0},
  {0x8500,0,0},//19
  {0x8600,0,0},//20
  {0x8601,0,0},
  {0x8602,0,0},
  {0x8603,0,0},
  {0x8604,0,0},
  {0x8605,0,0},
  {0x8606,0,0},
  {0x8607,0,0},
  {0x8700,0,0},
  {0x8701,0,0},//29
  {0x8702,0,0},//30
  {0x8800,0,0},
  {0x8801,0,0},
  {0x8802,0,0},
  {0x8803,0,0},
  {0x8804,0,0},
  {0x8805,0,0},
  {0x8900,0,0},
  {0x8a00,0,0},//38
};



//********************�ⲿ����*******************

//���ڽ���IC����֤ƽ̨ ͸���·�����֤��Ϣ
//extern u8 gICCard_Radio_Buffer_Valid;
//extern u8 gICCard_Radio_Buffer[];

//extern u8 ICCard_SM ;
//extern u16 ICCard_SM_Timer;


//----------------------------------------------------
extern const u8 EepromPramLength[];
extern UART2_RECEIVE    uart2_recdat;   //����2����
extern TIME_TASK MyTimerTask[];
extern u8   ResetEnableFlag;//����ʹ�ܱ�־
extern u16  MultiSendMax;//��η��ͼ��������ֵ
extern u16  MultiSendCount;//��η��ͼ���
extern u16  RecorderTotalList;//��ʷ�켣��¼������
extern u16  MultiSendRemainder;//�������һ��������
extern s16  RecorderSector;//��ʷ�켣��ȡ������
extern s16  RecorderStep;//��ʷ�켣��ȡ�Ĳ���
extern u32  RecorderTimeCount;//��ʷ�켣��ȡ��ʱ��
//extern HISTORY_AREA_STATUS  HistoryCarStatus;
//extern CAN_REC  CanData; dxl,2015.9,
//extern HISTORY_AREA_STATUS  HistoryTrackStatus;
extern u16  CurPacketCount;//��ǰ���ڷ��ͻ�ȡͼ��Ϊ�ڼ���
extern u16  CameraRxBufferLen;//����ͷ���ջ������ݳ���
extern u8   CameraRxBuffer[];//���ջ���
extern u32     PhotoID;//��ý��ID
extern u8      MediaType;//��ý�����ͣ�0ͼ��1��Ƶ��2��Ƶ
extern u8      MediaFormat;//��ý���ʽ����,0JPEG,1TIF,2MP3,3WAV,4WMV
extern u8        MediaRatio;//��ý��ֱ���
extern u8      MediaChannelID;//��ý��ͨ��ID
extern u8      MediaEvenCode;//��ý���¼�����
extern u16     MediaCommand;//��ý������  0:ֹͣ  N:����  0XFFFF:¼�� 0XFFFE:һֱ����  BY WYF
extern u16     MediaPhotoSpace; //���ռ��ʱ�� BY WYF
extern u8        MemoryUploadFileName[];//�洢��ý�������ϴ��������ļ���
extern u8      MultiMediaSendFlag;//��ý�巢�ͱ�־��0Ϊ���ڷ��ͣ�1Ϊ�ڷ���
extern u8      MultiCenterLinkFlag;//�������������ñ�׼,1Ϊ������
extern u8   CurrentJpgFolder;//��ǰͼ���ļ���
//extern HISTORY_AREA_STATUS  NavigationRead;
//********************************�Զ�����������****************************

extern void MOD_GetIccidBcdValue(u8 *Bcdbuff);
extern void Area_ClearRectArea(void);
//********************************�궨��************************************

//********************************��������**********************************

//TERMINAL_UPDATA_PACKET gUpdatePacket;

//STRUCT_UPLOADFROMBREAK gUploadFromBreak; 
//********************��Ҫʹ�õĻ���*******************
u8  RadioShareBuffer[RADIO_PROTOCOL_BUFFER_SIZE];//����������
u8  RadioProtocolRx1Buffer[RADIO_PROTOCOL_RX1_BUFFER_SIZE];//����1���ջ���
Queue  RadioProtocolRx1Q;//����1���ն���
u8  RadioProtocolRx2Buffer[RADIO_PROTOCOL_RX2_BUFFER_SIZE];//����2���ջ���
Queue  RadioProtocolRx2Q;//����2���ն���
u8  RadioProtocolRxSmsBuffer[RADIO_PROTOCOL_RX_SMS_BUFFER_SIZE];//���Ž��ջ���
Queue  RadioProtocolRxSmsQ;//���Ž��ն���
static u8   RadioProtocolTxBuffer[RADIO_PROTOCOL_BUFFER_SIZE];//Э�鷢�ͻ���,һ�����������,æ��־��1;�˳�����ʱ,��0
static u8   RadioProtocolParseBuffer[RADIO_PROTOCOL_BUFFER_SIZE];//Э���������,�ڽ���һ��ָ��ǰ�轫���ݷ��ڸû���
u8  RadioProtocolRxBufferBusyFlag = 0;//RadioProtocolRxBufferæ��־,0����,1Ϊæ,æʱ�������û���д����
u8  CommonShareBufferBusyFlag = 0;//CommonShareBufferæ��־,0����,1Ϊæ,æʱ�������û���д����
u16 RadioProtocolRxLen = 0;//RadioProtocolRxBuffer�������ݳ���
u16 CommonShareBufferLen = 0;//CommonShareBuffer���ݳ���
//********************ģ������ʹ�õı���*******************
MESSAGE_HEAD RadioProtocolRxHead;//���յĶ��������ݾ�������õ���֡ͷ���ڴ˽ṹ����
u8  TerminalAuthorizationFlag = 0;//�ն˼�Ȩ��־,bit0:����,bit1:����1;bit2:����2;bit3~bit7����;
//ÿ���ն˼�Ȩ�ɹ������Ӧλ����1,����Ͽ�����Ӧλ����0
u8  BigLittleFlag = 0;//ϵͳ���С�˱�־,0ΪС��
static  u8  TerminalAuthorizationCode[LOGIN_PASSWORD_LEN] = {0};//��Ȩ��,��1���ֽڴ洢��Ȩ��ĳ���
static u16  QuestionAckSerialNum = 0;//����Ӧ����ˮ��

//********************�ⲿģ����õı���*******************
u16     DelayAckCommand = 0;//��ʱӦ������
u8  DelaySendChannel = 0;//��ʱ����ͨ����
u8  DelaySmsPhone[20];//��ʱ���Ͷ��ź���
u16     CarControlAckSerialNum = 0;//��������Ӧ����ˮ��
u16     CarControlAckCommandID = 0;//��������Ӧ������ID
u8      TerminalPhoneNumber[6] = {0};//�ն��ֻ���
static u16 SmsChannelSerialNum = 0;//����ͨ����ˮ��
static u16 Data1ChannelSerialNum = 0;//����ͨ������1��ˮ��
static u16 Data2ChannelSerialNum = 0;//����ͨ������2��ˮ��
u8  RecorderProtocolFormatFlag = 0;//0:�·����ϴ��ļ�¼���������֡ͷ,1:�·����ϴ��ļ�¼���������֡ͷ
u16 RecorderSerialNum;//��ʻ��¼��Ӧ����ˮ��
u8  RecorderCmd;//��ʻ��¼������
u8      MultiMediaIndexSendFlag = 0;//��ý������ϴ���־,1Ϊ��Ҫ�����ϴ�,0Ϊ����Ҫ
u8      RecordSendFlag = 0;//¼���ļ��ϴ���־,1Ϊ�ϴ�
TIME_T  PhotoSaveTime;
//********************************��������**********************************
//*****************��һ���֣���׼��ƽ̨�·�ָ��**********************
/*********************************************************************
//��������  :RadioProtocol_ServerGeneralAck(u8 *pBuffer, u16 BufferLen)
//����      :ƽ̨����������ͨ��Ӧ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8001
*********************************************************************/
ProtocolACK RadioProtocol_ServerGeneralAck(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u16 SerialNum;
    u16 ID;
    u8  Result;
    u8  *p;
    u8  flag;
    u8  HighByte;
    u8  LowByte;
	  u8  Buffer[25];
	  u8 length;
    static u8   Count = 0;
    //static u16  count = 0;
        
    p = pBuffer;
    //�жϳ����Ƿ���ȷ
    if((5 != BufferLen))
    {
        return ACK_ERROR;
    }
   
    /////////////////////////////
    //��ȡӦ����ˮ��
    HighByte = *p++;
    LowByte = *p++;
    SerialNum = (HighByte << 8)|LowByte;
    if(0 == SerialNum)
    {
        //Ϊ���벻����
    }
    
    //��ȡӦ��ID
    HighByte = *p++;
    LowByte = *p++;
    ID = (HighByte << 8)|LowByte;
     
    if(0x0200 == ID)
    {
        Report_CmdAck(channel);
    }
    //�ж��Ƿ�Ϊä������Ӧ��
    else if(0x0704 == ID)
    {
        Blind_ReportAck(channel);
    }
    //�ж��Ƿ�Ϊ��ȨӦ��
    else if(0x0102 == ID)
    {
        //��ȡ���
        Result = *p;
        if(0 == Result)
        {           
            //�ɹ�
            Count++;
            SetTerminalAuthorizationFlag(channel);
            #ifdef MODEM_EYE_D_H
            EyeNet_Back_Open();
            #endif
                        if(CHANNEL_DATA_1 == channel)
                        {
                                Public_ShowTextInfo("ͨ��1��Ȩ�ɹ�",100);//dxl,2013.11.9
                        }
                        else if(CHANNEL_DATA_2 == channel)
                        {
                                Public_ShowTextInfo("ͨ��2��Ȩ�ɹ�",100);//dxl,2013.11.9
                        }
            Blind_StartReport(channel);//����ä������
            //SetEvTask(EV_REPORT);//��Ȩ�ɹ��������ϱ�1��λ����Ϣ,dxl,2014.2.14
						if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
						{
										
						}
						else
						{
                Report_UploadPositionInfo(channel);
						}
        }
        else
        {
            //���־
            ClearTerminalAuthorizationFlag(channel);
            //��ʾ��Ȩʧ��
            //Lcd_DisplayString(2, 0, 0, (u8 *)RegisterInfo[7], 16);
            if(CHANNEL_DATA_1 == channel)
            {
                  Public_ShowTextInfo("ͨ��1��Ȩʧ��",100);//dxl,2013.11.9
            }
            else if(CHANNEL_DATA_2 == channel)
            {
                  Public_ShowTextInfo("ͨ��2��Ȩʧ��",100);//dxl,2013.11.9
            }
						if(1 == BBGNTestFlag)
						{
							   //����ע��ָ��
							   //RadioProtocol_TerminalLogout(channel);
							   strcpy((char *)Buffer,(const char *)"2016042100");//�Ǽ�ʱд�������Ȩ��
		             length = strlen((char *)Buffer);
							   Buffer[length] = (TerminalPhoneNumber[5]&0x0f)+'0';
							   length++;
		              EepromPram_WritePram(E2_LOGINGPRS_PASSWORD_ID, Buffer, length);
						}
						else
						{
                 ClearTerminalAuthorizationCode(channel);//������Ȩ���Ա������ע������
					  }
        }
        
    }
    else if(0x0003 == ID)//�ն�ע��
    {
        if(CHANNEL_DATA_1 == channel)
        {
            Public_ShowTextInfo("ͨ��1ע���ɹ�",100);//dxl,2013.11.9
        }
        else if(CHANNEL_DATA_2 == channel)
        {
            Public_ShowTextInfo("ͨ��2ע���ɹ�",100);//dxl,2013.11.9
        }
        TerminalAuthorizationFlag = 0;
				if(1 == BBGNTestFlag)//dxl,2016.5.13
				{
				    ClearTerminalAuthorizationCode(channel);
					  NVIC_SystemReset();
				}
				//���־
        ClearTerminalAuthorizationFlag(channel);
				
    }
    else if(0x0108 == ID)//�ն��������֪ͨ
    {
        UpdataResultNoteAckFlag = 1;
    }
		else if(0x0110 == ID)//���ٿ���Ӧ��
    {
        Lock2_OpenAccountAck(pBuffer,BufferLen);
    }
    else
    {
        
        if(Count >= 1)
        {
            //ȫ����ʾ
            //SetMainFullDisplayFlag();
            //������������ʾ
            //ClrDisplayControlFlag();
        }
        Count = 0;
        //��ȡ���
        Result = *p;

        if(0 == Result)
        {
            //�ɹ�
        }
        else if(1 == Result)
        {
            //ʧ��
        }
        else if(2 == Result)
        {
            //��Ϣ����
        }
        else if(3 == Result)
        {
            //��֧��
        }
        else if(4 == Result)//��������ȷ��
        {
            //���������Ҫƽ̨ȷ�ϵı��� 
            if(SET == Io_ReadRecordConrtolBit(RECORD_BIT_EMERGENCY))
            {
                //�ر����������������¼��
                Io_WriteRecordConrtolBit(RECORD_BIT_EMERGENCY, RESET);
            }
            //����������˹�ȷ�ϵı���
            //�����������
            Io_WriteAlarmBit(ALARM_BIT_EMERGENCY, RESET);
            //���Σ��Ԥ��
            Io_WriteAlarmBit(ALARM_BIT_DANGER_PRE_ALARM, RESET);
            //����������򱨾�
            Io_WriteAlarmBit(ALARM_BIT_IN_OUT_AREA, RESET);
            //���������·����
            Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD, RESET);
            //�����·��ʻ�������㱨��
            Io_WriteAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE, RESET);
            
            //����Ƿ����
            Io_WriteAlarmBit(ALARM_BIT_LAWLESS_ACC, RESET);
            //����Ƿ�λ��
            Io_WriteAlarmBit(ALARM_BIT_LAWLESS_MOVEMENT, RESET);
            //����Ƿ�������
            Io_WriteAlarmBit(ALARM_BIT_LAWLESS_OPEN_DOOR, RESET);
            
            OUTPUT2_RESET();//output2Ϊ�͵�ƽ
            flag = 0;
            FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, &flag, FRAM_EMERGENCY_FLAG_LEN);//�������洢��־
        }
    }
		
		if(1 == BBGNTestFlag)//dxl,2016.5.23
		{
			  if((1 == Io_ReadAlarmBit(ALARM_BIT_IN_OUT_AREA))&&(1 == AreaInOutAlarmClearEnableFlag))
				{
					  AreaInOutAlarmClearEnableFlag = 0;
		        Io_WriteAlarmBit(ALARM_BIT_IN_OUT_AREA, RESET);//����������򱨾�
				}
		}
    return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_TerminalRegisterAck(u8 *pBuffer, u16 BufferLen)
//����      :�ն�ע��Ӧ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8100
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRegisterAck(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  Result;
    u8  *p;
    u8  i;
    u8  VerifyCode;
    u8  temp;
//	  u8  Buffer[20];
//	  u8  length;
    u16 AckSerialNum;
    p = pBuffer;


    //�жϳ����Ƿ���ȷ
    if((BufferLen < 3)||(BufferLen >= 1024))
    {
        return ACK_ERROR;
    }
    
    //��ȡӦ����ˮ��
    AckSerialNum = 0;
    temp = *p++;
    AckSerialNum |= temp << 8;
    temp = *p++;
    AckSerialNum |= temp;
    
    //��ȡ���
    Result = *p++;  

    //�жϽ��
    if(0 == Result)
    {
        //��ȡ��Ȩ��
        if((BufferLen-3) > (LOGIN_PASSWORD_LEN-2))
        {
                        Public_ShowTextInfo("��Ȩ�볤�ȳ���",100);//dxl,2013.11.9
            return ACK_ERROR;
        }
        //д���Ȩ�볤��
        TerminalAuthorizationCode[0] = BufferLen-3;
        //������Ȩ��
        memcpy(TerminalAuthorizationCode+1,p,BufferLen-3);
        //д��У���ֽ�,��У��
        VerifyCode = 0;
        for(i=0; i<BufferLen-3; i++)
        {
            VerifyCode += TerminalAuthorizationCode[1+i];
        }
        TerminalAuthorizationCode[1+BufferLen-3] = VerifyCode;
        
                if(CHANNEL_DATA_1 == channel)
                {
                  //�����Ȩ��
                if(0 == EepromPram_WritePram(E2_LOGINGPRS_PASSWORD_ID, p, BufferLen-3))
                {
                    //���������дһ��
                    if(0 == EepromPram_WritePram(E2_LOGINGPRS_PASSWORD_ID, p, BufferLen-3))
                                {
                                        Public_ShowTextInfo("ͨ��1��Ȩ�뱣��ʧ��",100);//dxl,2013.11.9
                                }
                                else
                                {
                                        SetLink1LoginAuthorizationState(LOGIN_SEND_AUTHORIZATION_CMD);
                                SetTimerTask(TIME_LINK1_LOGIN, SYSTICK_1SECOND);
                                        Public_ShowTextInfo("ͨ��1�ն�ע��ɹ�",100);//dxl,2013.11.9
                                }
                    }
                        else
                        {
                                SetLink1LoginAuthorizationState(LOGIN_SEND_AUTHORIZATION_CMD);
                        SetTimerTask(TIME_LINK1_LOGIN, SYSTICK_1SECOND);
                                Public_ShowTextInfo("ͨ��1�ն�ע��ɹ�",100);//dxl,2013.11.9
                        }
                }
                else if(CHANNEL_DATA_2 == channel)
                {
                      //�����Ȩ��
            if(0 == EepromPram_WritePram(E2_SECOND_LOGINGPRS_PASSWORD_ID, p, BufferLen-3))
            {
                //���������дһ��
                if(0 == EepromPram_WritePram(E2_SECOND_LOGINGPRS_PASSWORD_ID, p, BufferLen-3))
                            {
                                      Public_ShowTextInfo("ͨ��2��Ȩ�뱣��ʧ��",100);//dxl,2013.11.9
                            }
                            else
                            {
                                      SetLink2LoginAuthorizationState(LOGIN_SEND_AUTHORIZATION_CMD);
                              SetTimerTask(TIME_LINK2_LOGIN, SYSTICK_1SECOND);
                                      Public_ShowTextInfo("ͨ��2�ն�ע��ɹ�",100);//dxl,2013.11.9
                            }
            }
                    else
                    {
                              SetLink2LoginAuthorizationState(LOGIN_SEND_AUTHORIZATION_CMD);
                      SetTimerTask(TIME_LINK2_LOGIN, SYSTICK_1SECOND);
                              Public_ShowTextInfo("ͨ��2�ն�ע��ɹ�",100);//dxl,2013.11.9
                    }
              }
              else
              {
                        Public_ShowTextInfo("�ն�ע��Ӧ��ͨ������",100);//dxl,2013.11.9
                        RadioProtocol_TerminalLogout(channel);
              }
    }
    else if(1 == Result)
    {
            Public_ShowTextInfo("�����ѱ�ע��",100);//dxl,2013.11.9
            RadioProtocol_TerminalLogout(channel);
    }
    else if(2 == Result)
    {
            Public_ShowTextInfo("���ݿ����޸ó���",100);//dxl,2013.11.9
            RadioProtocol_TerminalLogout(channel);
    }
    else if(3 == Result)
    {
            Public_ShowTextInfo("�ն��ѱ�ע��",100);//dxl,2013.11.9
			      //if(1 == BBGNTestFlag)//dxl,2016.5.9
						//{
						    //strcpy((char *)Buffer,(const char *)"2016042100");//�Ǽ�ʱд�������Ȩ��
		            //length = strlen((char *)Buffer);
							  //Buffer[length] = (TerminalPhoneNumber[5]&0x0f)+'0';
							 // length++;
		            //EepromPram_WritePram(E2_LOGINGPRS_PASSWORD_ID, Buffer, length);
						//}
						//else
						//{
                 RadioProtocol_TerminalLogout(channel);
						//}
    }
    else if(4 == Result)
    {
            Public_ShowTextInfo("���ݿ����޸��ն�",100);//dxl,2013.11.9
            RadioProtocol_TerminalLogout(channel);
    }
    return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_PramSet(u8 *pBuffer, u16 BufferLen)
//����      :�����ն˲���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8103
*********************************************************************/
ProtocolACK RadioProtocol_PramSet(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    s16 TotalPramCount;//��������
    //s16   PacketPramCount;//����������
    u8  *p;
    s16 length;
    u8  AckResult = 0;
    
    p = pBuffer;
    
    //���Ȳ�������1024
    if(BufferLen >= 1024)
    {
        //Ӧ����,��Ϣ����
        AckResult = 0;
    }
    else
    {
        //��ȡ����
        length = BufferLen;

        //��ȡ��������
        TotalPramCount = *p++;
        length--;
        
        if(0 == TotalPramCount)
        {
          
        }
				else
				{
            RadioProtocol_WritePram(channel,p, length);
				}
        
        
        //Ӧ����
        AckResult = 0;
    }

    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
    
}
/*********************************************************************
//��������  :RadioProtocol_WritePram(channel,u8 *pBuffer, u16 BufferLen)
//����      :д����
//����      :pBuffer����ID+ID����+���ݵ�˳������д����ֽ����У��޲����ܸ�����ֻ���ֽ����г���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�����ߴ������ò���������GPRS���ò���ͳһ���øú������ú�����0x8103�������
*********************************************************************/
ProtocolACK RadioProtocol_WritePram(u8 channel,u8 *pBuffer, u16 BufferLen)
{
    return E2ParamApp_DisposeWriteParam(channel,pBuffer,BufferLen);
}
/*********************************************************************
//��������  :RadioProtocol_PramSearch(u8 *pBuffer, u16 BufferLen)
//����      :��ѯ�ն˲���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8104
*********************************************************************/
/*
static ProtocolACK RadioProtocol_ParamQueryAll(u8 *pBuffer, u16 BufferLen)
{
  
    u32 PramID;
    u8  *p = NULL;
    //u8    *p1 = NULL;
    u8  *p2 = NULL;
    //u8    ReadPramCount;
    u8  PramLen;
    u8  AckPramCount;//����������
    u8  Buffer[30];//�������ȱ���С��30������Ҫ���󻺳�
    u16 length;
    //s16   BufferLength;
    //u8    AckResult;
    u16 AckSerialNum;
    u8  SmsPhone[20];
    u8  channel = 0;
    MESSAGE_HEAD    head;


    //���Ȳ�������1024
    //if((0 != BufferLen)||(1 == CommonShareBufferBusyFlag))
    //{
        //AckResult = 2;
        //return RadioProtocol_TerminalGeneralAck(AckResult);
    //}
    //��ȡӦ����ˮ��
    AckSerialNum = RadioProtocolRxHead.SerialNum;
    //Ӧ����ˮ��д�뻺��
    p = RadioShareBuffer;
    if(NULL == p)
    {
        return ACK_ERROR;
    }
    p2 = p;
    *p++ = (AckSerialNum & 0xff00) >> 8;
    *p++ = AckSerialNum & 0xff;
    *p++;//Ԥ��1���ֽڴ洢Ӧ���������
    length = 0;
    AckPramCount = 0;
  
  
  
    //��ȡȫ���ڶ��Ĳ���
        //-----------------ƽָ̨���Ĳ���---------------
        for(PramID=1; PramID<0x0110; PramID++)
        {
            //�жϳ���
            if(length >=1024)
            {
                break;
            }
            ///if((PramID == 0x002C)||(PramID == 0x002D))//ģ���ն˻�û�м���������
            //{

            //}
            //else
            //{
            //������
            PramLen = EepromPram_ReadPram(PramID, Buffer);
      
            if((PramLen != 0)&&(PramLen < 30)){
        
                //д����ID
                *p++ = 0;
                *p++ = 0;
                *p++ = (PramID & 0xff00) >> 8;
                *p++ = PramID & 0xff;
                //д��������
                *p++ = PramLen;
                //д��������
                memcpy(p,Buffer,PramLen);
                p += PramLen;
                AckPramCount++;
                length += (PramLen+1+4);
        
                if(length >= 1000){
                    break;
                }
                
            }
            //}
        }   //for(PramID=1; PramID<0x0110; PramID++)
    
        //д�����������
        *(p2+2) = AckPramCount;
        //���ȼ���Ӧ����ˮ��,Ӧ���������
        length += 3;
        //��Ϣͷ
        //��ϢID
        head.MessageID = 0x0104;
        head.Attribute = length;
        //�ж��Ƿ�Ϊ����Ϣ
        if(CHANNEL_SMS == uart2_recdat.rec_chanel)
        {
            memcpy(SmsPhone,gSmsRecord.msgParam.TPA,16);
            channel = CHANNEL_SMS;
        }
    
        if(0 == RadioProtocol_SendCmd(channel,&head, p2, length, SmsPhone))
        {
            return ACK_OK;
        }
        else
        {
            return ACK_ERROR;
        }
}
*/
/*********************************************************************
//��������  :RadioProtocol_PramSearch(u8 *pBuffer, u16 BufferLen)
//����      :��ѯ�ն˲���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8104
*********************************************************************/

ProtocolACK RadioProtocol_ParamQueryAll(u8 channel,u8 *pBuffer, u16 BufferLen)
{
  
    u32 PramID;
    u8  *p = NULL;
    u8  *p1 = NULL;
    u8  *p2 = NULL;
    u8  ReadPramCount;
    u8  PramLen;
    u8  AckPramCount;//����������
    u8  Buffer[30];//�������ȱ���С��30������Ҫ���󻺳�
    u16 length;
    //s16   BufferLength;
    //u8    AckResult;
    u16 AckSerialNum;
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;


    //���Ȳ�������1024
    //if((0 != BufferLen)||(1 == CommonShareBufferBusyFlag))
    //{
        //AckResult = 2;
        //return RadioProtocol_TerminalGeneralAck(AckResult);
    //}
    //��ȡӦ����ˮ��
    AckSerialNum = RadioProtocolRxHead.SerialNum;
    //Ӧ����ˮ��д�뻺��
    p = RadioShareBuffer;
    if(NULL == p)
    {
        return ACK_ERROR;
    }
    p2 = p;
    *p++ = (AckSerialNum & 0xff00) >> 8;
    *p++ = AckSerialNum & 0xff;
    *p++;//Ԥ��1���ֽڴ洢Ӧ���������
    length = 0;
    AckPramCount = 0;
    if(0 == BufferLen)//��ȡȫ���Ĳ���
    {
        //-------------ƽָ̨���Ĳ���-------------
        for(PramID=1; PramID<0x0110; PramID++)
        {
            //�жϳ���
            if(length >=1024)
            {
                break;
            }
            ///if((PramID == 0x002C)||(PramID == 0x002D))//ģ���ն˻�û�м���������
            //{

            //}
            //else
            //{
            //������
            PramLen = EepromPram_ReadPram(PramID, Buffer);
      
            if((PramLen != 0)&&(PramLen < 30)){
        
                //д����ID
                *p++ = 0;
                *p++ = 0;
                *p++ = (PramID & 0xff00) >> 8;
                *p++ = PramID & 0xff;
                //д��������
                *p++ = PramLen;
                //д��������
                memcpy(p,Buffer,PramLen);
                p += PramLen;
                AckPramCount++;
                length += (PramLen+1+4);
        
                if(length >= 1000){
                    break;
                }
                
            }
            //}
        }   
        //-------------------�Զ���Ĳ���--------------------
        for(PramID=E2_PRAM_BASE_CUSTOM_ID; PramID<E2_SELF_DEFINE_ID_MAX; PramID++)
        {
            if((PramID == E2_ALL_DRIVER_INFORMATION_ID)||(PramID == E2_UPDATA_URL_ID))
            {
                
            }
            else
            {
                //������
                PramLen = EepromPram_ReadPram(PramID, Buffer);
                if(PramLen >= 30)
                {
                    PramID++;
                }
                else if((PramLen != 0)&&(PramLen < 30))
                {
                    //д����ID
                    *p++ = 0;
                    *p++ = 0;
                    *p++ = (PramID & 0xff00) >> 8;
                    *p++ = PramID & 0xff;
                    //д��������
                    *p++ = PramLen;
                    //д��������
                    memcpy(p,Buffer,PramLen);
                    p += PramLen;
                    AckPramCount++;
                    length += (PramLen+1+4);
                    if(length >= 1000)
                    {
                        break;
                    }
                
                }
            }
        }   
        //д�����������
        *(p2+2) = AckPramCount;
        //���ȼ���Ӧ����ˮ��,Ӧ���������
        length += 3;
        //��Ϣͷ
        //��ϢID
        head.MessageID = 0x0104;
        head.Attribute = length;
    
        if(0 != RadioProtocol_SendCmd(channel,&head, p2, length, SmsPhone))
        {
            return ACK_OK;
        }
        else
        {
            return ACK_ERROR;
        }
        
        
    }
    else//��ȡָ���Ĳ���
    {
        p1 = pBuffer;//��1��Ϊ��Ҫ��ѯ�Ĳ����ܸ���
        ReadPramCount = *p1++;
         //-----------------ƽָ̨���Ĳ���-----------------
        while(ReadPramCount != 0)
        {
            //�жϳ���
            if(length >=1024)
            {
                break;
            }
            ///if((PramID == 0x002C)||(PramID == 0x002D))//ģ���ն˻�û�м���������
            //{

            //}
            //else
            //{
            PramID = 0;
            PramID |= *p1 << 24;
            p1++;
            PramID |= *p1 << 16;
            p1++;
            PramID |= *p1 << 8;
            p1++;
            PramID |= *p1;
            p1++;
            ReadPramCount--;
            //������
            //Shigle 2013-05-13
            if( PramID > 0x110  )
            {
                PramID = PramID - 0xD00;
            }
            
            PramLen = EepromPram_ReadPram(PramID, Buffer);
            if((PramLen != 0)&&(PramLen < 30))
            {
                //д����ID
                *p++ = 0;
                *p++ = 0;
                *p++ = (PramID & 0xff00) >> 8;
                *p++ = PramID & 0xff;
                //д��������
                *p++ = PramLen;
                //д��������
                memcpy(p,Buffer,PramLen);
                p += PramLen;
                AckPramCount++;
                length += (PramLen+1+4);
                if(length >= 1000)
                {
                    break;
                }
                
            }
            //}
        }   
            
        //д�����������
        *(p2+2) = AckPramCount;
        //���ȼ���Ӧ����ˮ��,Ӧ���������
        length += 3;
        //��Ϣͷ
        //��ϢID
        head.MessageID = 0x0104;
        head.Attribute = length;
    
        if(0 != RadioProtocol_SendCmd(channel,&head, p2, length, SmsPhone))
        {
            return ACK_OK;
        }
        else
        {
            return ACK_ERROR;
        }
        
    }
}


ProtocolACK RadioProtocol_PramSearch(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  *p = NULL;
    u8  *p1 = NULL;
    u8  AckPramCount = 0;//����������
    s16 length = 0;
    u16 AckSerialNum = 0;
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;
    
    //��ȡӦ����ˮ��
    AckSerialNum = RadioProtocolRxHead.SerialNum;
    //Ӧ����ˮ��д�뻺��
    p = RadioShareBuffer;
    if(NULL == p)
    {
        return ACK_ERROR;
    }
    *p++ = (AckSerialNum & 0xff00) >> 8;
    *p++ = AckSerialNum & 0xff;
    *p++;//Ԥ��1���ֽڴ洢Ӧ���������
    if(0 == BufferLen)//��ȡȫ���Ĳ���
    {
        AckPramCount=E2ParamApp_DisposeReadAllParam(p,&length,1);
    }
    else//��ȡָ���Ĳ���
    {
        p1 = pBuffer;//��1��Ϊ��Ҫ��ѯ�Ĳ����ܸ���
        p1++;//�Թ�
        AckPramCount = RadioProtocol_ReadPram(p, &length, p1, BufferLen-1 , 1);
    }
    //д�����������
    *(RadioShareBuffer+2) = AckPramCount;
    //���ȼ���Ӧ����ˮ��,Ӧ���������
    length += 3;
    //��Ϣͷ
    //��ϢID
    head.MessageID = 0x0104;
    head.Attribute = length;
    
    if(0 != RadioProtocol_SendCmd(channel,&head, RadioShareBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}

/*********************************************************************
//��������  :RadioProtocol_ReadPram
//����      :������
//����      :����Ҫ���Ĳ���ID�ŷ��뻺��pIDBuffer�У�ÿ��ID���ֽڣ�����ǰ��IDBufferLenΪ4��������
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȡ�Ĳ����ܸ�����*pDstBuffer��ID�ţ�ID���ȣ�ID���ݵ�˳���ţ�*DstBufferLen�ܵĶ�ȡ���ȣ������������ܸ����ֽ�
//��ע      :ReadType:0Ϊ����(����)��ÿ��ֻ�ܶ�550�ֽڣ�1Ϊ���ߣ�GPRS����ÿ�οɶ�1000�ֽ�
*********************************************************************/
u8 RadioProtocol_ReadPram(u8 *pDstBuffer, s16 *DstBufferLen, u8 *pIDBuffer, s16 IDBufferLen,u8 ReadType)
{
    return E2ParamApp_DisposeReadParam(pDstBuffer,DstBufferLen,pIDBuffer,IDBufferLen,ReadType);
}


/*********************************************************************
//��������  :RadioProtocol_TerminalControl(u8 *pBuffer, u16 BufferLen)
//����      :�ն˿���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8105
*********************************************************************/
ProtocolACK RadioProtocol_TerminalControl(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  Command;
    u8  *p;
    u8  Count;
    s16 length;
    u32 i,j,k;
    u16 PramLen;
    u8  AckResult;
    u8  ControlLinkWord;
    u8  Buffer[200];
    u8  ErrorFlag = 0;
    //u8    ManufactureID[7];
        SOC_IPVAL ip_value;//dxl,2015.5.5
        u16 port;
 //   u16 linktime;

    p = pBuffer;
    length = BufferLen;

    //��ȡ������
    Command = *p++;
    length--;
    
    if(1 == Command)//��������
    {
        //��������Ϊ��URL��ַ;���ŵ�����;�����û���;��������;��ַ;TCP�˿�;
        //UDP�˿�;������ID;Ӳ���汾;�̼��汾;���ӵ�ָ��������ʱ��

        i = 0;
        j = 0;
        Count = 0;
    
        while(length > 2)
        {
            if(';' == *(p+i))//�ҵ��˷ָ��
            {   
                Count++;
                PramLen = i-j;
                switch(Count)
                {
                    
                    case 1://URL��ַ
                    {
                        EepromPram_WritePram(E2_UPDATA_URL_ID, p+j, PramLen);
                        EepromPram_ReadPram(E2_UPDATA_URL_ID, Buffer);
                        for(k=0; k<PramLen; k++)
                        {
                            if(*(p+j+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 2://���ŵ�����
                    {
                        EepromPram_WritePram(E2_UPDATA_DIAL_NAME_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_DIAL_NAME_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 3://�����û���
                    {
                        EepromPram_WritePram(E2_UPDATA_DIAL_USER_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_DIAL_USER_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 4://��������
                    {
                        EepromPram_WritePram(E2_UPDATA_DIAL_PASSWORD_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_DIAL_PASSWORD_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 5://��ַ
                    {
                        EepromPram_WritePram(E2_UPDATA_IP_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_IP_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 6://TCP�˿�
                    {
                        EepromPram_WritePram(E2_UPDATA_TCP_PORT_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_TCP_PORT_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 7://UDP�˿�
                    {
                        EepromPram_WritePram(E2_UPDATA_UDP_PORT_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_UDP_PORT_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }

                    case 8://������ID
                    {
                        
                        EepromPram_WritePram(E2_UPDATA_MANUFACTURE_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_MANUFACTURE_ID, Buffer);
                        
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        /*
                        //��������ID
                        EepromPram_ReadPram(E2_MANUFACTURE_ID, ManufactureID);
                        
                        for(k=0; k<E2_MANUFACTURE_LEN; k++)
                        {
                            if(ManufactureID[k] != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        */
                        break;
                    }
                    case 9://Ӳ���汾
                    {
                        EepromPram_WritePram(E2_UPDATA_HARDWARE_VERSION_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_HARDWARE_VERSION_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    case 10://�̼��汾
                    {
                        EepromPram_WritePram(E2_UPDATA_FIRMWARE_VERSION_ID, p+j+1, PramLen-1);
                        EepromPram_ReadPram(E2_UPDATA_FIRMWARE_VERSION_ID, Buffer);
                        for(k=0; k<PramLen-1; k++)
                        {
                            if(*(p+j+1+k) != Buffer[k])
                            {
                                ErrorFlag = 1;
                                break;
                            }
                        }
                        break;
                    }
                    
                    default :
                        break;
                }
                j = i;
            }
            i++;
            length--;   
            if(10 == Count)
            {
                break;
            }
            
            
        }
        //���ӵ�������ʱ��
        EepromPram_WritePram(E2_UPDATA_TIME_ID, p+i, 2);
        EepromPram_ReadPram(E2_UPDATA_TIME_ID, Buffer);
        for(k=0; k<2; k++)
        {
            if(*(p+i+k) != Buffer[k])
            {
                ErrorFlag = 1;
                break;
            }
        }
        if(0 == ErrorFlag)//ֻ������ȫ����ȷ�˲ŻῪ��FTPԶ������,��Ӧ��
        {
            #ifdef EYE_MODEM
                if(Net_Other_FtpStart())
                {
                    AckResult = 0;//�ɹ�
                    Buffer[0] = 3;//������־ ʧ�� 3
                    FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,Buffer,1);                    
                }
                else
                {
                    AckResult = 1;//ʧ��
                }
            #else
                if(MyTimerTask[TIME_DELAY_TRIG].TaskTimerState == DISABLE)
                    {
                    // ����������������
                    //SetEvTask(EV_FTP_UPDATA);//2���Ӻ󴥷�
                    DelayAckCommand = 2;
                    //������ʱ����
                        SetTimerTask(TIME_DELAY_TRIG, 1);
                }
                AckResult = 0;
            #endif

            return RadioProtocol_TerminalGeneralAck(channel,AckResult);
        }
    }
  
    else if(2 == Command)//�����ն�����ָ��������
    {
        //��������Ϊ�����ӿ���;���ƽ̨��Ȩ��;���ŵ�����;�����û���;��������;��ַ;TCP�˿�;
        //UDP�˿�;���ӵ�ָ��������ʱ��
        //��ȡ���ӿ�����
        ControlLinkWord = *p;
        if(0 == ControlLinkWord)//���ӵ�ָ��������
        {
            i = 0;
            j = 0;
            Count = 0;
            while(length > 2)
            {
                if(';' == *(p+i))//�ҵ��˷ָ��
                {   
                    Count++;
                    PramLen = i-j;
                    switch(Count)
                    {
                        case 1://���ӿ���
                        {
                            break;
                        }
                        case 2://���ƽ̨��Ȩ��
                        {
                            EepromPram_WritePram(E2_UPDATA_MONITOR_SERVER_CODE_ID, p+j+1, PramLen-1);
                            EepromPram_ReadPram(E2_UPDATA_MONITOR_SERVER_CODE_ID, Buffer);
                            for(k=0; k<PramLen-1; k++)
                            {
                                if(*(p+j+1+k) != Buffer[k])
                                {
                                    ErrorFlag = 1;
                                    break;
                                }
                            }
                            break;
                        }
                        case 3://���ŵ�����
                        {
                            EepromPram_WritePram(E2_UPDATA_DIAL_NAME_ID, p+j+1, PramLen-1);
                            EepromPram_ReadPram(E2_UPDATA_DIAL_NAME_ID, Buffer);
                            for(k=0; k<PramLen-1; k++)
                            {
                                if(*(p+j+1+k) != Buffer[k])
                                {
                                    ErrorFlag = 1;
                                    break;
                                }
                            }
                            break;
                        }
                        case 4://�����û���
                        {
                            EepromPram_WritePram(E2_UPDATA_DIAL_USER_ID, p+j+1, PramLen-1);
                            EepromPram_ReadPram(E2_UPDATA_DIAL_USER_ID, Buffer);
                            for(k=0; k<PramLen-1; k++)
                            {
                                if(*(p+j+1+k) != Buffer[k])
                                {
                                    ErrorFlag = 1;
                                    break;
                                }
                            }
                            break;
                        }
                        case 5://��������
                        {
                            EepromPram_WritePram(E2_UPDATA_DIAL_PASSWORD_ID, p+j+1, PramLen-1);
                            EepromPram_ReadPram(E2_UPDATA_DIAL_PASSWORD_ID, Buffer);
                            for(k=0; k<PramLen-1; k++)
                            {
                                if(*(p+j+1+k) != Buffer[k])
                                {
                                    ErrorFlag = 1;
                                    break;
                                }
                            }
                            break;
                        }
                        case 6://��ַ
                        {
                            EepromPram_WritePram(E2_UPDATA_IP_ID, p+j+1, PramLen-1);
                            EepromPram_ReadPram(E2_UPDATA_IP_ID, Buffer);
                            for(k=0; k<PramLen-1; k++)
                            {
                                if(*(p+j+1+k) != Buffer[k])
                                {
                                    ErrorFlag = 1;
                                    break;
                                }
                            }
                            if(0 == ErrorFlag)
                            {
                                    memset(&ip_value.ip,0,50);
                                    if(PramLen < 50)
                                    {
                                            memcpy(&ip_value.ip,Buffer,PramLen-1);
                                    }
                            }
                            break;
                        }
        case 7://TCP�˿�
        {
            EepromPram_WritePram(E2_UPDATA_TCP_PORT_ID, p+j+1, PramLen-1);
            EepromPram_ReadPram(E2_UPDATA_TCP_PORT_ID, Buffer);
            for(k=0; k<PramLen-1; k++)
            {
                if(*(p+j+1+k) != Buffer[k])
                {
                    ErrorFlag = 1;
                    break;
                }
            }
            if(0 == ErrorFlag)
            {
                    memset(&ip_value.port,0,6);
                    port = (Buffer[0] << 8)|Buffer[1];
                    Num2NumStr(port, Buffer);
                    strcpy((char *)&ip_value.port,(char const *)Buffer);
                    memset(&ip_value.mode,0,5);
                    strcpy((char *)&ip_value.mode,"TCP");
            }
            break;
        }
        case 8://UDP�˿�
        {
            EepromPram_WritePram(E2_UPDATA_UDP_PORT_ID, p+j+1, PramLen-1);
            EepromPram_ReadPram(E2_UPDATA_UDP_PORT_ID, Buffer);
            for(k=0; k<PramLen-1; k++)
            {
                if(*(p+j+1+k) != Buffer[k])
                {
                    ErrorFlag = 1;
                    break;
                }
            }
            break;
        }
        default :
            break;
    }
            j = i;
            }
            i++;
            length--;
            if(Count == 8)
            {
                break;
            }
        }
            EepromPram_WritePram(E2_UPDATA_TIME_ID, p+i, 2);
            EepromPram_ReadPram(E2_UPDATA_TIME_ID, Buffer);
            for(k=0; k<2; k++)
            {
                if(*(p+i+k) != Buffer[k])
                {
                    ErrorFlag = 1;
                    break;
                }
            }
            if((0 == ErrorFlag)&&(MyTimerTask[TIME_DELAY_TRIG].TaskTimerState == DISABLE))
            {
                              //  linktime = (Buffer[0] << 8)|Buffer[1];
                                Connect_To_Specific_Server =1;      
                                if(CHANNEL_DATA_1 == channel)
                                {
                                        
                                       // Net_First_ConTo(ip_value, linktime);
                                        First_connect_To_Specific_Server();    //fanqinghai 2016.03.22
                                }
                                else if(CHANNEL_DATA_2 == channel)
                                {
                                    Second_connect_To_Specific_Server();
                                        //Net_Second_ConTo(ip_value, linktime);
                                }
                                
                // �������Ӽ��ƽ̨����
                //SetEvTask(EV_LINK_GOV_SERVER);
                DelayAckCommand = 6;
                //������ʱ����
                    SetTimerTask(TIME_DELAY_TRIG, 1);
                AckResult = 0;
                return RadioProtocol_TerminalGeneralAck(channel,AckResult);
            }
        }
        else if((1 == ControlLinkWord)&&(MyTimerTask[TIME_DELAY_TRIG].TaskTimerState == DISABLE))
        {
        
            //��������ͨ��ģ��,ʹ����������,����Ĭ�ϵ���������
            //MOD_StateSwitch(MOD_PWROFF);
            DelayAckCommand = 4;
            //������ʱ����
                SetTimerTask(TIME_DELAY_TRIG, 1);
            AckResult = 0;
            return RadioProtocol_TerminalGeneralAck(channel,AckResult);
        }
    }
    else if(3 == Command)//�ն˹ػ�
    {
                if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))
                {
                          //���ߣ�����
                          DelayAckCommand = 7;
                      //������ʱ����
                          SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
                }
                else
                {
                          //���ߣ������������
                          DelayAckCommand = 11;
                      //������ʱ����
                          SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
                }
        AckResult = 0;
        return RadioProtocol_TerminalGeneralAck(channel,AckResult);
              
    }
    else if(4 == Command)//�ն˸�λ
    {
        AckResult = 0;
        RadioProtocol_TerminalGeneralAck(channel,AckResult);
          //���ߣ�����
                DelayAckCommand = 7;
              //������ʱ����
                SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
        //Lcd_DisplayString(1, 0, 0, "�ն˼���������  ", 16);
    }
    else if(5 == Command)//�ָ���������
    {
        EepromPram_DefaultSet();
        AckResult = 0;
        RadioProtocol_TerminalGeneralAck(channel,AckResult);
                 //���ߣ�����
                DelayAckCommand = 7;
              //������ʱ����
                SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
    }
    else if(6 == Command)//�ر�����ͨ��
    {
    
        //ָ�ر�GPRS
        AckResult = 0;
        RadioProtocol_TerminalGeneralAck(channel,AckResult);
#ifdef HUOYUN_DEBUG_OPEN
                DelayAckCommand = 9;//��һ���ǹر�ͨ��ģ������,��ʱ1Сʱ������
                //������ʱ����
                SetTimerTask(TIME_DELAY_TRIG, 5*SECOND);
#endif
    }
    else if(7 == Command)//�ر���������ͨ��
    {
        //ָ��ϵGPRS,����
        AckResult = 0;
        RadioProtocol_TerminalGeneralAck(channel,AckResult);
#ifdef HUOYUN_DEBUG_OPEN
                DelayAckCommand = 9;//��һ���ǹر�ͨ��ģ������,��ʱ1Сʱ������
                //������ʱ����
                SetTimerTask(TIME_DELAY_TRIG, 5*SECOND);
#endif
    }
    return ACK_OK;

}
/*********************************************************************
//��������  :Get_Conncet_State
//����      :��ȡ���ӵ�ָ�������������־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :

*********************************************************************/
u8 Get_Conncet_State(void)
{
    return Connect_To_Specific_Server;
}
/*********************************************************************
//��������  :Clear_Conncet_State
//����      :�������ָ����������־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :

*********************************************************************/
void Clear_Conncet_State(void)
{
    Connect_To_Specific_Server = 0;
}

/*********************************************************************
//��������  :RadioProtocol_PositionSearch(u8 *pBuffer, u16 BufferLen)
//����      :λ����Ϣ��ѯ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8201
*********************************************************************/


ProtocolACK RadioProtocol_PositionSearch(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u16 length;
    u16 AckSerialNum;
    u8  Buffer[FLASH_BLIND_STEP_LEN+1];
    u8  SmsPhone[20];
    u8  PramLen;
    MESSAGE_HEAD    head;

    
    
    if(0 != BufferLen)
    {
        return ACK_ERROR;
    }
    length = 0;
    //���Ӧ����ˮ��
    AckSerialNum = RadioProtocolRxHead.SerialNum;
    Buffer[0] = (AckSerialNum & 0xff00) >> 8;
    Buffer[1] = AckSerialNum & 0xff;
    length += 2;
    //��ȡλ����Ϣ,������Buffer��
    PramLen = Report_GetPositionInfo(Buffer+2);
    length += PramLen;
    //����λ�û㱨��Ϣ
    //��ϢID
    head.MessageID = 0x0201;
    head.Attribute = length;

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
                //��ӵ��ط��б�
                //RadioProtocol_AddCmdToResendList(RadioProtocolRxHead.SerialNum, head.MessageID, Buffer, length, 0);
        return ACK_ERROR;

    }
    
}
/*********************************************************************
//��������  :RadioProtocol_TempPositionControl(u8 *pBuffer, u16 BufferLen)
//����      :��ʱλ�ø��ٿ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8202
*********************************************************************/
ProtocolACK RadioProtocol_TempPositionControl(u8 channel, u8 *pBuffer, u16 BufferLen)
{

    u8  *p;
    u8  AckResult;
    u16 TimeSpace;//ʱ����
    u32 ValidTime;//��Ч��
    u8  Buffer[5];
    
    

    p = pBuffer;
    
    //if(6 != BufferLen),����������,��Ҫֹͣ���ٿ���ʱ��2�ֽ�,HUOYUN_DEBUG_OPEN����ʱ������
    //{
        //AckResult = 2;
        //return RadioProtocol_TerminalGeneralAck(AckResult);
    //}
    
    //д����ʱ����ʱ����
    TimeSpace = 0;
    TimeSpace |= *p << 8;
    Buffer[0] = *p;
    p++;
    TimeSpace |= *p;
    Buffer[1] = *p;
    p++;
    FRAM_BufferWrite(FRAM_TEMP_TRACK_SPACE_ADDR, Buffer, FRAM_TEMP_TRACK_SPACE_LEN);
    Report_UpdateOnePram(PRAM_TEMP_TRACK_TIME);
        if(6 == BufferLen)
        {
            //д����ʱ������Ч��
            ValidTime = 0;
            ValidTime |= *p << 24;
            p++;
            ValidTime |= *p << 16;
            p++;
            ValidTime |= *p << 8;
            p++;
            ValidTime |= *p;
            ValidTime = ValidTime/TimeSpace;
            Buffer[0] = (ValidTime&0xff000000) >> 24;
            Buffer[1] = (ValidTime&0xff0000) >> 16;
            Buffer[2] = (ValidTime&0xff00) >> 8;
            Buffer[3] = ValidTime&0xff;
            FRAM_BufferWrite(FRAM_TEMP_TRACK_NUM_ADDR, Buffer, FRAM_TEMP_TRACK_NUM_LEN);
            Report_UpdateOnePram(PRAM_TEMP_TRACK_COUNT);
        }
    
    AckResult = 0;
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_TextMessageDownload(u8 *pBuffer, u16 BufferLen)
//����      :�ı���Ϣ�·�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8300
*********************************************************************/
ProtocolACK RadioProtocol_TextMessageDownload(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    unsigned short len;
    TIME_T stTime;
    u8 flag;
    ProtocolACK AckResult = ACK_OK;
    //��ȡ��־
    flag = *pBuffer;
    if(BufferLen >= 1024)
    {
        AckResult = ACK_ERROR;
    }
    else
    {
        len = 0;
        AckResult = ACK_OK;        
        ///////////////////////////
        if(PUBLIC_CHECKBIT(flag, 0))
        {
            strcpy((char *)&RadioShareBuffer[len],"������Ϣ:");
            len = strlen((char *)RadioShareBuffer);
        }
        ////////////////////////////
        if(PUBLIC_CHECKBIT(flag, 5))
        {
            strcpy((char *)&RadioShareBuffer[len],"CAN��������Ϣ:");
            len = strlen((char *)RadioShareBuffer);
        }
        else
        {
            strcpy((char *)&RadioShareBuffer[len],"���ĵ�����Ϣ:");
            len = strlen((char *)RadioShareBuffer);
        }
        ////////////////////
        memcpy(&RadioShareBuffer[len],pBuffer+1,BufferLen-1);
        len +=BufferLen-1;
        RadioShareBuffer[len] = 0;//��β��־
        CommonShareBufferLen = strlen((char *)RadioShareBuffer);
        //////����ʱ��//////////////////// ////////////       
        RTC_GetCurTime(&stTime);
        sprintf((char *)&RadioShareBuffer[CommonShareBufferLen],"(20%02d-%02d-%02d %02d:%02d:%02d)",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
        CommonShareBufferLen = strlen((char *)RadioShareBuffer);
        ///////////////////////////////////
        CommonShareBufferBusyFlag = 1;//CommonShareBufferæ��־
        if(PUBLIC_CHECKBIT(flag, 2))
        {
            DownloadGB2313_WriteToFlash(RadioShareBuffer,CommonShareBufferLen);
            SetEvTask(EV_SHOW_TEXT);
            #ifdef USE_NAVILCD
            NaviLcdShowPlatformText(RadioShareBuffer,CommonShareBufferLen);
            #endif
        }
        ////////////////////////
        if(PUBLIC_CHECKBIT(flag, 3))
        {
        
           Modem_Api_Tts_Play(0,pBuffer+1,BufferLen-1);
                    
        }
        ///////////////////
        if(PUBLIC_CHECKBIT(flag, 4))//ƽ̨��Ҫ�޸�
        {
            CommonShareBufferBusyFlag = 0;
        }

    }
    //return RadioProtocol_TerminalGeneralAck(channel,AckResult);
        DelayAckCommand = 0x8300;
    SetTimerTask(TIME_DELAY_TRIG, 10);// ��Ҫ��ʱ���ͣ���Ϊ�������TTS������Ӱ��
    return AckResult;
}
/*********************************************************************
//��������  :RadioProtocol_EvenSet(u8 *pBuffer, u16 BufferLen)
//����      :�¼�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8301
*********************************************************************/
ProtocolACK RadioProtocol_EvenSet(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult = 0;
    
        AckResult=EventReport_DisposeRadioProtocol(pBuffer,BufferLen);
         //��ʵ�ʶ�����ֻ��Ӧ��
        return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_QuestionDownload(u8 *pBuffer, u16 BufferLen)
//����      :�����·�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8302
*********************************************************************/
ProtocolACK RadioProtocol_QuestionDownload(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult = 0;
    QuestionAckSerialNum = RadioProtocolRxHead.SerialNum;//��ȡ����Ӧ����ˮ��  
    Question_DisposeRadioProtocol(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_InformationOrderSet(u8 *pBuffer, u16 BufferLen)
//����      :��Ϣ�㲥�˵�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8303
*********************************************************************/
ProtocolACK RadioProtocol_InformationOrderSet(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult;
        
    //��ʵ�ʶ�����ֻ��Ӧ��
    AckResult = InfoDemand_DisposeRadioProtocol(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_InformationService(u8 *pBuffer, u16 BufferLen)
//����      :��Ϣ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8304
*********************************************************************/
ProtocolACK RadioProtocol_InformationService(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult;
    //////////////////////// 
    AckResult=InfoService_DisposeRadioProtocol(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_TelephoneCallback(u8 *pBuffer, u16 BufferLen)
//����      :�绰�ز�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8400
*********************************************************************/
ProtocolACK RadioProtocol_TelephoneCallback(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult;
    u8  *p;
    u8  flag;
    s16 length;
    u8  Buffer[30];
    u8  PramLen;


    
    p = pBuffer;
    length = BufferLen;

    flag = *p++;
    length--;
    
    if((length < 0)||(length > 20))
    {
        AckResult = 2;
    }
    else
    {
        AckResult = 0;
        
        //��ȡ�ز�����
        //PhoneCallBackType = flag;
    
        //�ѵ绰����д��eeprom
        EepromPram_WritePram(E2_PHONE_CALLBACK_ID, p, length);
        
        //д���־λ
        EepromPram_WritePram(E2_CALLBACK_FLAG_ID, &flag, 1);
        
        //���绰����
        PramLen = EepromPram_ReadPram(E2_PHONE_CALLBACK_ID, Buffer);
        
        //�� ��־
        PramLen = EepromPram_ReadPram(E2_CALLBACK_FLAG_ID, Buffer);

                if(PramLen < 20)
                {

                }
    
        //�����绰�ز�����
        SetEvTask(EV_SEV_CALL);
                
    
        
    }
    //Ӧ��
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_TelephoneBookSet(u8 *pBuffer, u16 BufferLen)
//����      :���õ绰��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8401
*********************************************************************/
ProtocolACK RadioProtocol_TelephoneBookSet(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult = 0;
    
    AckResult=PhoneBook_DisposeRadioProtocol(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_CarControl(u8 *pBuffer, u16 BufferLen)
//����      :��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8500������ʹ��ר��Ӧ��0x0500������ʹ��ͨ��Ӧ��
*********************************************************************/
ProtocolACK RadioProtocol_CarControl(u8 channel, u8 *pBuffer, u16 BufferLen) 
{

    CarControlFlag = *pBuffer;
    if(PUBLIC_CHECKBIT(CarControlFlag,6))
    {
        if(PUBLIC_CHECKBIT(CarControlFlag,7))//����
        {
            OilControl_OffOil();
        }
        else//ͨ��
        {
            OilControl_OpenOil();
        }
    }
  
    //���³�������Ӧ����ˮ��
    if(MyTimerTask[TIME_DELAY_TRIG].TaskTimerState == DISABLE)
    {
        CarControlAckSerialNum = RadioProtocolRxHead.SerialNum;
        DelayAckCommand = 0x0500;
        DelaySendChannel = channel;
        if(CHANNEL_SMS == DelaySendChannel)
        {
            memcpy(DelaySmsPhone,gSmsRecord.msgParam.TPA,16);
        }
        //������ʱ����
        SetTimerTask(TIME_DELAY_TRIG, 100);//2���Ӧ��,��Ҫ��Ϊ��ȷ������/�������״̬�ܲɼ���
    }
    return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_SetRoundArea(u8 *pBuffer, u16 BufferLen)
//����      :����Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8600
*********************************************************************/
ProtocolACK RadioProtocol_SetRoundArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
u8 AckResult;
    
//ֻ��Ӧ��
AckResult = Area_SetRoundArea(pBuffer,BufferLen);
return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_DelRoundArea(u8 *pBuffer, u16 BufferLen)
//����      :ɾ��Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8601
*********************************************************************/
ProtocolACK RadioProtocol_DelRoundArea(u8 channel, u8 *pBuffer, u16 BufferLen) //SSSS
{
u8 AckResult;
 
//ֻ��Ӧ��
AckResult = Area_DelRoundArea(pBuffer,BufferLen);
return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_SetRectangleArea(u8 *pBuffer, u16 BufferLen)
//����      :���þ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8602
*********************************************************************/
ProtocolACK RadioProtocol_SetRectangleArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
  u8 AckResult;
  //u16 AreaAtribute;
  //u32 temp;
  //u8  Buffer[5];

/*
  //�б��ͼ��޸ģ���ʾ������Ŀ���
  AreaAtribute = (*(pBuffer+6) << 8)|*(pBuffer+7);
  if((AreaAtribute&0x8000)==0x8000)
  {
            BdTestCount1 = 0;
            BdTestCount2 = 0;
            Area_ClearRectArea();//�����������,���֮ǰ������,�����������������������
            ClrRoute();//���֮ǰ��·��
  }
  else if((AreaAtribute&0x4000)==0x4000)
  {
            BdTestCount1 = 0;
            BdTestCount2 = 0;
            Report_EraseBlindArea();//���ä��
            Area_ClearRectArea();//�����������
            ClrRoute();//���֮ǰ��·��
            
            //����ACC �ϱ�ʱ����Ϊ1��
            temp = 1;
        Buffer[0] = (temp & 0xff000000) >> 24;
        Buffer[1] = (temp & 0xff0000) >> 16;
        Buffer[2] = (temp & 0xff00) >> 8;
        Buffer[3] = temp;
        EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID, Buffer, E2_ACCON_REPORT_TIME_LEN);
            EepromPram_UpdateVariable(E2_ACCON_REPORT_TIME_ID);
            
            //�رղ���ɵ�����
        ClrTimerTask(TIME_TRACK_RECORD);
        ClrTimerTask(TIME_CAN_SEND);
        ClrTimerTask(TIME_CAMERA);
        ClrTimerTask(TIME_SPEED_MONITOR);
        ClrTimerTask(TIME_TIRED_DRIVE);
        ClrTimerTask(TIME_DOUBT_POINT);
       
      
        ClrTimerTask(TIME_RECORD);
       
        ClrTimerTask(TIME_PULSE_ADJUST);
        ClrTimerTask(TIME_IC_CARD);
        ClrTimerTask(TIME_ICAUC);
         ClrTimerTask(TIME_GPS_UPDATE);
         
        ClrTimerTask(TIME_BMA220);//22:��ײ����
        ClrTimerTask(TIME_PULSE);//23:�����ٶ�
        ClrTimerTask(TIME_USB);//25:USB
        ClrTimerTask(TIME_TTSDELAY);//26: TTS
        ClrTimerTask(TIME_SLEEP);//28:����
        ClrTimerTask(TIME_SPEED_LOG);//31:�ٶ��쳣��¼
        ClrTimerTask(TIME_POWER_ON_LOG);//32:�����¼
        ClrTimerTask(TIME_MEDIA);//39:ɾ����ý���ļ�
        ClrTimerTask(TIME_ROUTE);//41:��·����
        ClrTimerTask(TIME_GNSS_REAL);  //53������ʵʱ��������
        ClrTimerTask(TIME_GNSS_APP);//54:�����������ݵ�ƽ̨����
            
  }
 */
  
//ֻ��Ӧ��
AckResult = Area_SetRectArea(pBuffer,BufferLen);
return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_DelRectangleArea(u8 *pBuffer, u16 BufferLen)
//����      :ɾ����������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8603
*********************************************************************/
ProtocolACK RadioProtocol_DelRectangleArea(u8 channel, u8 *pBuffer, u16 BufferLen) //SSSS
{
u8 AckResult;
//ֻ��Ӧ��
AckResult = Area_DelRectArea(pBuffer,BufferLen);
return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_SetPolygonArea(u8 *pBuffer, u16 BufferLen)
//����      :���ö��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8604
*********************************************************************/
ProtocolACK RadioProtocol_SetPolygonArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult;
    AckResult = Polygon_SetArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_DelPolygonArea(u8 *pBuffer, u16 BufferLen)
//����      :ɾ�����������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8605
*********************************************************************/
ProtocolACK RadioProtocol_DelPolygonArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult;
    AckResult = Polygon_DeleteArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_SetRoad(u8 *pBuffer, u16 BufferLen)
//����      :����·��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8606
*********************************************************************/
ProtocolACK RadioProtocol_SetRoad(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult = 0;               //����ֵ
    u8 flag = 0;
    if(BufferLen==0)
    {
        return RadioProtocol_TerminalGeneralAck(channel,2);//��Ϣ����
    }
    else
    if(PUBLIC_CHECKBIT(RadioProtocolRxHead.Attribute,13)&&((RadioProtocolRxHead.CurrentPackage > RadioProtocolRxHead.TotalPackage) || (RadioProtocolRxHead.CurrentPackage==0)))
    {
        return RadioProtocol_TerminalGeneralAck(channel,2);//��Ϣ����
    }
    else
    if(PUBLIC_CHECKBIT(RadioProtocolRxHead.Attribute,13)&&(RadioProtocolRxHead.TotalPackage>=ROUTE_NODE_SUM))
    {
        return RadioProtocol_TerminalGeneralAck(channel,1);//ʧ��
    }
    //////////////////////
    //�ӵ�һ�����л����·��Ϣ    
    if(0==PUBLIC_CHECKBIT(RadioProtocolRxHead.Attribute, 13))
    {
        flag = 0;
    }
    else
    if(1==RadioProtocolRxHead.CurrentPackage)//��һ��
    {
        flag = 1;
    }
    else
    {
        flag = 2;
    }
    AckResult = Route_DisposeSetRoad(flag,pBuffer,BufferLen);
 
    /////////////////////
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_DelRoad(u8 *pBuffer, u16 BufferLen)
//����      :ɾ��·��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8607
*********************************************************************/
ProtocolACK RadioProtocol_DelRoad(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult;   
    if(BufferLen==0)
    {
        return FORMAT_ERROR;
    }           
    //ֻ��Ӧ��
    AckResult = Route_DisposeDelRoad(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_SetMuckDumpArea
//����      :���������㵹����(0x860a)
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x860a
*********************************************************************/
ProtocolACK RadioProtocol_SetMuckDumpArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult=0;
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_DeleteMuckDumpArea
//����      :ɾ�������㵹����(0x860b)
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x860b
*********************************************************************/
ProtocolACK RadioProtocol_DeleteMuckDumpArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult=0;
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}
/*********************************************************************
//��������  :RadioProtocol_QueryMuckDumpArea(u8 *pBuffer, u16 BufferLen)
//����      :��ѯ�����㵹����(0x860c)
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��(0x860c)
*********************************************************************/
ProtocolACK RadioProtocol_QueryMuckDumpArea(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u16 length;
    u8  *Buffer;
    u8  SmsPhone[20];
    u8  PramLen = 0;
    MESSAGE_HEAD    head;

    length = 0;
    Buffer = RadioShareBuffer;    
    length += PramLen;
    //���������㵹������Ϣ
    //��ϢID
    head.MessageID = 0x060c;
    head.Attribute = length;

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        //��ӵ��ط��б�
        //RadioProtocol_AddCmdToResendList(RadioProtocolRxHead.SerialNum, head.MessageID, Buffer, length, 0);
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_RecorderDataCollection(u8 *pBuffer, u16 BufferLen)
//����      :��ʻ��¼����Ϣ�ɼ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8700
*********************************************************************/
ProtocolACK RadioProtocol_RecorderDataCollection(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    /* dxl,2015.9,
    //��ȡӦ����ˮ��
    Vdr_WireAck = RadioProtocolRxHead.SerialNum;

    if(channel == CHANNEL_DATA_2)
    {
        Vdr_Data_Rec(VDR_WIRE_2,0,pBuffer,BufferLen);//�ɼ�����
    }
    else
    {
        Vdr_Data_Rec(VDR_WIRE_1,0,pBuffer,BufferLen);//�ɼ�����
    }
    */
    VDRGprs_RecData(channel, RadioProtocolRxHead.SerialNum, pBuffer, BufferLen);
  return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_RecorderPramSet(u8 *pBuffer, u16 BufferLen)
//����      :��ʻ��¼�ǲ����´�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8701
*********************************************************************/
ProtocolACK RadioProtocol_RecorderPramSet(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    /* dxl,2015.9,
    //��ȡӦ����ˮ��
    Vdr_WireAck = RadioProtocolRxHead.SerialNum;
    
    if(channel == CHANNEL_DATA_2)
    {
        Vdr_Data_Rec(VDR_WIRE_2,1,pBuffer,BufferLen);//��������
    }
    else
    {
        Vdr_Data_Rec(VDR_WIRE_1,1,pBuffer,BufferLen);//��������
    }
    */
      VDRGprs_RecData(channel, RadioProtocolRxHead.SerialNum, pBuffer, BufferLen);
    return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_DriverInformationUploadRequest(u8 channel, u8 *pBuffer, u16 BufferLen)
//����      :�ϱ���ʻԱ�����Ϣ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8702
*********************************************************************/
ProtocolACK RadioProtocol_DriverInformationUploadRequest(u8 channel, u8 *pBuffer, u16 BufferLen)
{
	  u8 Buffer[150];
	  u8 length;
	
    if(0 == BufferLen)
		{
			  if(1 == CardDriverWorkFlag)//�ϰ�
				{
					  length = 0;
			      memcpy(Buffer,CardDriverWorkStartTime,8);//�ϰ�ʱ��
					  length += 8;
			      memcpy(Buffer+8,CardDriverInfo,CardDriverInfoLen);//IC����Ϣ
					  length += CardDriverInfoLen;
			      return RadioProtocol_DriverInformationReport(channel,Buffer,length);
				}
				else if(2 == CardDriverWorkFlag)//�°�
				{
				    length = 0;
			      memcpy(Buffer,CardDriverWorkEndTime,7);//�ϰ�ʱ��
					  length += 7;
			      return RadioProtocol_DriverInformationReport(channel,Buffer,length);
				}
		}
		
		return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_MultiMediaUploadAck(u8 *pBuffer, u16 BufferLen)
//����      :��ý�������ϴ�Ӧ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8800
*********************************************************************/
ProtocolACK RadioProtocol_MultiMediaUploadAck(u8 channel, u8 *pBuffer, u16 BufferLen)
{

  Camera_MediaUploadAck(pBuffer, BufferLen);//����ͨ��channel���Ժ��ԣ���Ϊ֮ǰ����ʱ��֪�����ĸ�ͨ������

  return ACK_OK;
    
}
/*********************************************************************
//��������  :RadioProtocol_CameraPhoto(u8 *pBuffer, u16 BufferLen)
//����      :����ͷ��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8801
*********************************************************************/
ProtocolACK RadioProtocol_CameraPhoto(u8 channel, u8 *pBuffer, u16 BufferLen) 
{
//        u8  AckResult = 0;
        u8  StoreFlag;
        u16 temp;
        u16 Address;
        u8  Buffer[50] = {0};
        u8  i;
				
				if(1 == BBXYTestFlag)//Э����ʱֱ�ӻ�0x0801
				{
            RecorderSerialNum = RadioProtocolRxHead.SerialNum;
            temp  = (*(pBuffer+1)<<8)|*(pBuffer+2); 
            MediaCommand=temp;
            MediaType = 0;//��ý�����ͣ�0ͼ��1��Ƶ��2��Ƶ
            MediaFormat = 0;//��ý���ʽ����,0JPEG,1TIF,2MP3,3WAV,4WMV
            MediaChannelID = *pBuffer;//��ý��ͨ��ID
            MediaPhotoSpace =(*(pBuffer+3)<<8)|*(pBuffer+4); 
            MediaEvenCode = 0;//��ý���¼�����
					
					  Buffer[0] = 0x00;//��ý��ID��4�ֽ�
					  Buffer[1] = 0x00;
					  Buffer[2] = 0x00;
					  Buffer[3] = 0x01;
					  Buffer[4] = 0x00;//��ý������
					  Buffer[5] = 0x00;//��ý������ʽ
					  Buffer[6] = 0x00;//�¼������
					  Buffer[7] = *pBuffer;//ͨ��ID
					  Report_GetPositionBasicInfo(Buffer+8);
					  if(0 == MediaCommand)//ֹͣ����
						{
						    Camera_Photo(channel,MediaChannelID,MediaCommand,MediaPhotoSpace,MediaRatio,StoreFlag,0,0);
						}
						else
						{
				        RadioProtocol_MultiMediaDataReport(CHANNEL_DATA_1,Buffer,36,1,1);
						}
				}
				else
					
				{
                
        //��ȡӦ����ˮ��
        RecorderSerialNum = RadioProtocolRxHead.SerialNum;
        //��ȡ�������������
        temp  = (*(pBuffer+1)<<8)|*(pBuffer+2); 
        MediaCommand=temp;
        //MediaID++;//��ý��ID��
    //u8    MediaIDBuffer[5] = {0};
    //MediaIDBuffer[0] = PhotoID << 24;
    //MediaIDBuffer[1] = PhotoID << 16;
    //MediaIDBuffer[2] = PhotoID << 8;
    //MediaIDBuffer[3] = PhotoID;
    //FRAM_BufferWrite(FRAM_MEDIA_ID_ADDR,MediaIDBuffer,4);
        MediaType = 0;//��ý�����ͣ�0ͼ��1��Ƶ��2��Ƶ
        MediaFormat = 0;//��ý���ʽ����,0JPEG,1TIF,2MP3,3WAV,4WMV
        MediaChannelID = *pBuffer;//��ý��ͨ��ID
        MediaPhotoSpace =(*(pBuffer+3)<<8)|*(pBuffer+4); 
        MediaEvenCode = 0;//��ý���¼�����
				
				PhotoID_0800 = PhotoID+1;
				MediaType_0800 = MediaType;
				MediaFormat_0800 = MediaFormat;
				MediaEvenCode_0800 = MediaEvenCode;
				MediaChannelID_0800 = MediaChannelID;
				MediaEventSendChannel_0800 = channel;
				
        StoreFlag = *(pBuffer+5);//�����־
				if(1 == StoreFlag)//dxl,2016.5.16��¼����ʱ��
				{
					  if(1 == BBGNTestFlag)
						{
				        RTC_GetCurTime(&PhotoSaveTime);
						}
					  PhotoMediaEvUploadFlag = 0;
				}
				else
				{
				    PhotoMediaEvUploadFlag = 1;
				}
        

        //EepromPram_WritePram(E2_MEDIA_SAVE_FLAG_ID, &MediaSaveFlag, 1);
        temp=*(pBuffer+6); //ͼ��ֱ���
        if(temp>2 || temp==0)
        {
             temp=1;
        }
        MediaRatio=temp;      
    //��������,����˳��CameraID,PhotoNum,PhotoSpace,Resolution,StoreFlag, PhotoType,EventType
    
        if(0 == StoreFlag)
        {
            StoreFlag = 0x01;//bit0Ϊ1��ʾ��Ҫ�ϴ�
        }
        else if(1 == StoreFlag)
        {
            StoreFlag = 0x02;//bit1Ϊ1��ʾ��Ҫ����
        }
        Camera_Photo(channel,MediaChannelID,MediaCommand,MediaPhotoSpace,MediaRatio,StoreFlag,0,0);//�б���ʱʹ�����м���,��һ��Camera_Photo��ȥ��
    
				DelayAckCommand = 0x8801;    //������ʱ����
        SetTimerTask(TIME_DELAY_TRIG, 1);
        
        if(0xfffe == MediaCommand)
        {
        
        }
        else if(MediaCommand > 255)
        {
                MediaCommand = 255;
        }
        Buffer[0] = (MediaCommand&0xff00)>>8;//��������������eeprom��
        Buffer[1] = MediaCommand&0xff;
        Buffer[2] = Buffer[0]+Buffer[1];
        Address = E2_CAMERA_PHOTO_CMD_ADDR;
        for(i=0; i<3; i++)
        {
                E2prom_WriteByte(Address+i, Buffer[i]);
        }
				
				
			  }
       
				return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_MemoryIndex(u8 *pBuffer, u16 BufferLen)
//����      :�洢��ý�����ݼ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8802
*********************************************************************/
ProtocolACK RadioProtocol_MemoryIndex(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  MediaType;//��ý������
    u8  ChannelID;//ͨ��ID,0Ϊ����ȫ��ͨ��
    u8  EvCode;//�¼�����
    u8  HighBcd;//BCD��ĸ߰��ֽ�
    u8  LowBcd;//BCD��ĵͰ��ֽ�
    u8  *p;
	  u8  flag = 0;
    u16   length;
	  u32  TimeCount;
//	  u32  CurTimeCount;
    TIME_T  StartTime;
    TIME_T  EndTime;
    
    MediaType = *pBuffer;
    ChannelID = *(pBuffer+1);
    EvCode = *(pBuffer+2);
    //**********Ϊ���벻������������**********
    if(0 == MediaType)
    {
    
    }
    if(0 == ChannelID)
    {
    
    }
    if(0 == EvCode)
    {
    
    }
    p = pBuffer+3;
    //****************��ȡ��ʼʱ��***********************
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    StartTime.year = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    StartTime.month = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    StartTime.day = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    StartTime.hour = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    StartTime.min = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    StartTime.sec = HighBcd*10+LowBcd;
    
    //****************��ȡ����ʱ��***********************
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    EndTime.year = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    EndTime.month = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    EndTime.day = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    EndTime.hour = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    EndTime.min = HighBcd*10+LowBcd;
    HighBcd = ((*p) & 0xf0)>>4;
    LowBcd = ((*p) & 0x0f);
    p++;
    EndTime.sec = HighBcd*10+LowBcd;
		
		if(1 == BBGNTestFlag)//dxl,2016.5.16���ʱ�·�������������ʱ���붼Ϊ0
		{
				TimeCount = ConverseGmtime(&PhotoSaveTime);
			  if(TimeCount >= 180)
				{
			      TimeCount -= 180;
					  flag = 1;
				}
				else if(TimeCount >= 60)
				{
				    TimeCount -= 60;
					  flag = 2;
				}
				else
				{
				    TimeCount = 1;
					  flag =3;
				}
			  Gmtime(&StartTime,  TimeCount);
	
				if(1 == flag)
				{
			      TimeCount += 360;
				}
				else if(2 == flag)
				{
				    TimeCount += 120;
				}
				else
				{
				    TimeCount = RTC_GetCounter();
				}
			  Gmtime(&EndTime,  TimeCount);
		}
	
    
    length = MediaSearch_GetMediaIndexInformation(RadioShareBuffer+4, MediaType, &StartTime, &EndTime);

    //Ӧ����ˮ��
    RadioShareBuffer[0] = (RadioProtocolRxHead.SerialNum & 0xff00) >> 8;
    RadioShareBuffer[1] = RadioProtocolRxHead.SerialNum;
    //������
    RadioShareBuffer[2] = 0;
    RadioShareBuffer[3] = length/35;
    length += 4;
    RadioProtocol_MemoryIndexAck(channel,RadioShareBuffer, length);
    return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_MemoryUpload(u8 *pBuffer, u16 BufferLen)
//����      :�洢��ý�������ϴ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8803
*********************************************************************/
ProtocolACK RadioProtocol_MemoryUpload(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    
    u8  AckResult;
    
    //ֻ��Ӧ��,����ʱ����ͨ
    AckResult = 0;
    SetTimerTask(TIME_SEND_MEDIA, 15*SYSTICK_0p1SECOND);
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
    
}
/*********************************************************************
//��������  :RadioProtocol_StartSoundRecord(u8 *pBuffer, u16 BufferLen)
//����      :��ʼ¼��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8804
*********************************************************************/
ProtocolACK RadioProtocol_StartSoundRecord(u8 channel, u8 *pBuffer, u16 BufferLen)
{

    u8  AckResult;
	
    AckResult = 0;
    
   
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);//��֧��¼������
}
/*********************************************************************
//��������  :RadioProtocol_SingleMemoryIndex(u8 *pBuffer, u16 BufferLen)
//����      :�����洢��ý�����ݼ����ϴ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8805
*********************************************************************/
ProtocolACK RadioProtocol_SingleMemoryIndex(u8 channel, u8 *pBuffer, u16 BufferLen)
{
   
  
	u32	MediaID;
	u8	DelFlag;
	u8	*p;
        
	u8	FileName[33];
	u8	length;
	u8	AckResult;
	
	p = pBuffer;

	MediaID = 0;
	MediaID |= *p << 24;
	p++;
	MediaID |= *p << 16;
	p++;
	MediaID |= *p << 8;
	p++;
	MediaID |= *p;
	p++;

	DelFlag = *p;
	if(DelFlag)
	{
	
	}
	

	MediaSearch_GetMediaIDFileName(FileName,MEDIA_TYPE_JPG, MediaID);

	length = strlen((char const *)FileName);
	if(0 != length)
	{
		if(0 == strlen((char const *)MemoryUploadFileName))//??
		{
			strcpy((char *)MemoryUploadFileName,(char const *)FileName);
			SetTimerTask(TIME_SEND_MEDIA, 15*SYSTICK_0p1SECOND);
		}
		else
		{	
			
		}
	}
	else
	{
			
	}
  

	AckResult = 0;
	return RadioProtocol_TerminalGeneralAck(channel,AckResult);
	
}
/*********************************************************************
//��������  :RadioProtocol_JiutongPhoto(u8 *pBuffer, u16 BufferLen)
//����      :����ͷ��������(��ͨЭ��)
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8811
*********************************************************************/
ProtocolACK RadioProtocol_JiutongPhoto(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  AckResult;  
    u8  MediaChannelID;
        MediaChannelID = *pBuffer;//��ý��ͨ��ID
    
    Camera_Photo(channel,MediaChannelID,1,0,1,0x01,1,0);//ƽ̨����1�����¼����մ���
    
    AckResult = 0;
    return RadioProtocol_TerminalGeneralAck(channel,AckResult);
} 
/*********************************************************************
//��������  :RadioProtocol_OriginalDataDownTrans(u8 *pBuffer, u16 BufferLen)
//����      :��������͸��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8900
*********************************************************************/
extern u8 A2OBD_DATA_FLAG;
ProtocolACK RadioProtocol_OriginalDataDownTrans(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult;
    u8 flag;
    
    AckResult = 0;
    if ((BufferLen > 1023)||(BufferLen <= 1))
    {
        return ACK_ERROR;
    }

    flag=*pBuffer;
  
		if(EIEXPAND_PROTOCOL_TRAN_TYPE == flag)//������չЭ��
    {
        EIExpand_RadioProtocolParse(pBuffer+1,BufferLen-1);
    }
    else if(0x0B == flag) //IC����֤�����·�����֤��Կ��Ϣ
    {
         Card_AuthenticationAck(pBuffer+1,BufferLen-1);
    }
    else if((0x41 == flag)||(0x42 == flag))
    {
        if(1 == *(pBuffer+1))//0x01��ʾ͸�������ͺ�����
        {
            Uart1DataDownTransFlag = 1;
            Uart1DataDownTransCmd = *(pBuffer+5);
            JointechOiLCost_COM_Buf_Write(pBuffer+2,BufferLen-2);
        }
    }
    else 
    {
            
    }

		if(A2OBD_DATA_FLAG==1)
		{
			A2OBD_DATA_FLAG=0;
			return ACK_OK;
		}
		else
		{
			return RadioProtocol_TerminalGeneralAck(channel,AckResult);
		}
}

/*********************************************************************
//��������  :RadioProtocol_ServerRSA(u8 *pBuffer, u16 BufferLen)
//����      :ƽ̨��Կ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ָ��ID��0x8A00
*********************************************************************/
ProtocolACK RadioProtocol_ServerRSA(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u32 RSAServerE;
    //u8    RSAServerN[128];
    u8  temp;
    u8  *p;
    //u8    AckResult;
        u32     RSATerminalE;
        u8     RSATerminalN[128] = {1,2,3,4,5,6,7,8,9,10};

    p = pBuffer;


    //��ȡƽ̨eֵ
    RSAServerE = 0;
    temp = *p++;
    RSAServerE |= temp << 24;
    temp = *p++;
    RSAServerE |= temp << 16;
    temp = *p++;
    RSAServerE |= temp << 8;
    temp = *p++;
    RSAServerE |= temp;
    //��ȡƽ̨nֵ
    //memcpy(RSAServerN,p,128);

    //��ȡ�ն�eֵ
        RSATerminalE = 1;

        return RadioProtocol_TerminalRSA(channel,RSATerminalE, RSATerminalN);
}
//*****************�ڶ����֣���׼���ն��Ϸ�ָ��**********************
/*********************************************************************
//��������  :RadioProtocol_TerminalGeneralAck(u16 SerialNum, u16 MessageID, u8 Result)
//����      :�ն�ͨ��Ӧ��
//����      :Result:Ӧ����,0Ϊ�ɹ���1Ϊʧ�ܣ�2��Ϣ����3��֧��
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :�ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0001
*********************************************************************/
ProtocolACK RadioProtocol_TerminalGeneralAck(u8 channel, u8 AckResult)
{

    u8  Buffer[5];
    u8  length;
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;
    
    //��ϢID
    head.MessageID = 0x0001;
    //��Ϣ����
    length = 5;
    head.Attribute = length;
    //��Ϣ����
    Buffer[0] = (RadioProtocolRxHead.SerialNum & 0xff00) >> 8;
    Buffer[1] = RadioProtocolRxHead.SerialNum;
    Buffer[2] = (RadioProtocolRxHead.MessageID & 0xff00) >> 8;
    Buffer[3] = RadioProtocolRxHead.MessageID;
    Buffer[4] = AckResult;
    
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        
    }
    else
    {
         //��ӵ��ط��б�
        //RadioProtocol_AddCmdToResendList(RadioProtocolRxHead.SerialNum, head.MessageID, Buffer, length, 0);
        return ACK_ERROR;
    }
    return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_TerminalRegister(void)
//����      :�ն�ע��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0100
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRegister(u8 channel)
{

    u8  *p;
    u8  *q;
    u8  Buffer[150];
    u8  Buffer2[30];
    u8  Buffer3[50];
    u8  PramLen;
    u8  SmsPhone[20];
    u16 length;
    u8  i;
    u8  BcdHigh;
    u8  BcdLow;
    u8  value;
    MESSAGE_HEAD    head;
    u32 ID;
    p = Buffer;
    length = 0;
    
    

    //��ȡʡ��ID
    PramLen = EepromPram_ReadPram(E2_CAR_PROVINCE_ID, Buffer2);
    if(2 == PramLen)
    {
        *p++ = Buffer2[0];
        *p++ = Buffer2[1];
        length += 2;
    }
    else
    {
        *p++ = 0;
        *p++ = 0;
        length += 2;
    }
    

    //��ȡ����ID
    PramLen = EepromPram_ReadPram(E2_CAR_CITY_ID, Buffer2);
    if(2 == PramLen)
    {
        *p++ = Buffer2[0];
        *p++ = Buffer2[1] & 0xff;
        length += 2;
    }
    else
    {
        *p++ = 0;
        *p++ = 0;
        length += 2;
    }

    //��ȡ������ID
    PramLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, Buffer2);
    if(5 == PramLen)
    {
        memcpy(p,Buffer2,5);
        p += 5;
        length += 5;
    }
    else
    {
        memcpy(p,"70108",5);
        p += 5;
        length += 5;
    }
    
    
    //��ȡ�ն��ͺ�
    for(i=0; i<20; i++)
    {
        Buffer2[i] = 0;
    }
#ifdef HUOYUN_DEBUG_OPEN
        Buffer2[0] = 'E';
        Buffer2[1] = 'G';
        Buffer2[2] = 'S';
        Buffer2[3] = '7';
        Buffer2[4] = '0';
        Buffer2[5] = '1';
        Buffer2[6] = 'G';
        Buffer2[7] = 'B';
#else
    if(E2_CAR_ONLY_NUM_ID_LEN == EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID, Buffer3))//�����������ն��ͺŲ����ַ�ֻ����0x00�������ǿո񣬷���ȫ������ƽ̨���ϣ���ʾ���ݿ��޴��ն�
    {
        for(i=0; i<16; i++)
        {
            if(Buffer3[7+i] < 128)//�ж��Ƿ�ΪASCII��,ǰ��7���ֽ�Ϊ3C���
            {
                Buffer2[i] = Buffer3[7+i];
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        memcpy(Buffer2,TerminalTypeID,6);//���û�����ù�Ψһ�Ա�ţ���Ʒ�ͺ�Ĭ��ΪEGS701
        
    }
#endif
    memcpy(p,Buffer2,20);
    p += 20;
    length += 20;

    //��ȡ�ն�ID
    PramLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer2);
    if(7 == PramLen)
    {
    
        //ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
        q = Buffer2;
        q += 3;
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[0] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[1] = BcdHigh + '0';
        Buffer3[2] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[3] = BcdHigh + '0';
        Buffer3[4] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[5] = BcdHigh + '0';
        Buffer3[6] = BcdLow + '0';
        memcpy(p,Buffer3,7);
        p += 7;
        length += 7;
    }
    else
    {
        //��ȡ���ݵ��ն�ID��
        PramLen = EepromPram_ReadPram(E2_DEVICE_BACKUP_ID, Buffer2);
        if(7 == PramLen)
        {
            //�����֮ǰhex��(2011.12.16֮ǰ10λID��)����BCD��(2011.12.16֮��13λID��)
            if((0 == Buffer2[0])&&(0 == Buffer2[1]))//��ӦV1.0��Ӳ���汾
            {
            ID = 0;
            ID |= Buffer2[3] << 24;
            ID |= Buffer2[4] << 16;
            ID |= Buffer2[5] << 8;
            ID |= Buffer2[6];
            Buffer3[0] = ID%10000000/1000000+'0';
            Buffer3[1] = ID%1000000/100000+'0';
            Buffer3[2] = ID%100000/10000+'0';
            Buffer3[3] = ID%10000/1000+'0';
            Buffer3[4] = ID%1000/100+'0';
            Buffer3[5] = ID%100/10+'0';
            Buffer3[6] = ID%10+'0';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
            }
            else
            {
            //ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
            q = Buffer2;
            q += 3;
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[0] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[1] = BcdHigh + '0';
            Buffer3[2] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[3] = BcdHigh + '0';
            Buffer3[4] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[5] = BcdHigh + '0';
            Buffer3[6] = BcdLow + '0';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
            }
        }
        else
        {
            //�̶�ID
            Buffer3[0] = '2';
            Buffer3[1] = '0';
            Buffer3[2] = '1';
            Buffer3[3] = '2';
            Buffer3[4] = '0';
            Buffer3[5] = '0';
            Buffer3[6] = '1';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
        }
    }
    
    //��ȡ������ɫ
    PramLen = EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, Buffer2);
        if(1 == PramLen)
        {
                *p++ = Buffer2[0];
            length++;

        }
        else
        {

                *p++ = 0;
                length++;
        }
    
#ifdef HUOYUN_DEBUG_OPEN
        if(0 == Buffer2[0])//��ȡVIN,��VIN��ע��
        {
                PramLen = EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, Buffer2);
                if((PramLen < 20)&&(0 != PramLen))
                {
                      memcpy(p,Buffer2,PramLen);
                  length += PramLen;
                }
                else
                {
                      memcpy(p,"VIN123456789",12);
                  length += 12;
                }
        }
        else
#endif
        {
            //��ȡ����,�ó��ƺ���ע��
            PramLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, Buffer2);
                if((PramLen < 20)&&(0 != PramLen))
                {
                      memcpy(p,Buffer2,PramLen);
                  length += PramLen;
                      //p += PramLen;
                      //*p = 0;
                      //length++;
                }
                else
                {
                      memcpy(p,"��b012345",9);
                  length += 9;
                      // p += 9;
                      //*p = 0;
                      //length++;
                }
    }

    //��ϢID
    head.MessageID = 0x0100;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);
    
    //��ȡע�����Ϣ����
    
    
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
    
}
/*********************************************************************
//��������  :RadioProtocol_FirmwareUpdataResultNote(u8 *pBuffer, u16 BufferLen)
//����      :Զ�̹̼��������֪ͨ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0108
*********************************************************************/
ProtocolACK RadioProtocol_FirmwareUpdataResultNote(u8 channel, u8 *pBuffer, u16 BufferLen)
{
        
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;
    
        if(2 != BufferLen)
        {
                return ACK_ERROR;
        }
    //��ϢID
    head.MessageID = 0x0108;
    //��Ϣ����
    head.Attribute = BufferLen;
    //��Ϣ����
    if(0 != RadioProtocol_SendCmd( channel,&head, pBuffer, BufferLen,SmsPhone))
    {
        
    }
        else
    {
                //��ӵ��ط��б�
                //RadioProtocol_AddCmdToResendList(RadioProtocolRxHead.SerialNum, head.MessageID, Buffer, length, 0);
        return ACK_ERROR;
        }
         return ACK_OK;
}
/*********************************************************************
//��������  :RadioProtocol_OpenAccount(void)
//����      :�ն˿���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0110
*********************************************************************/
/*�б걱������ʱʹ�øú���
ProtocolACK RadioProtocol_OpenAccount(void)
{
    u8  Buffer[100]={0};
    u8  temp[30];
    u8  temp1[10];
    u8  *p = NULL;
    u8  *q = NULL;
    s16 length;
    u8  PramLen;
    u8  BcdHigh;
    u8  BcdLow;
    u8  value;
    u8  i;  
    u8  SmsPhone[20];
    u8  channel = 0;
    MESSAGE_HEAD    head;

    p = Buffer;
    length = 0;

    //���ܱ�ʶ,δ����
    *p++ = 0;
    length++;
    //������Կ,Ĭ��Ϊ0
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    length += 4;
    //�����ֻ���
    //��eeprom�ж�ȡ�����ֻ���
    PramLen =Public_ReadDataFromFRAM(FRAM_CAR_OWNER_PHONE_ADDR, temp,FRAM_CAR_OWNER_PHONE_LEN);//add by joneming 20130619
    if(FRAM_CAR_OWNER_PHONE_LEN == PramLen)
    {
        memcpy(p,temp,FRAM_CAR_OWNER_PHONE_LEN);
        p += FRAM_CAR_OWNER_PHONE_LEN;
        length += FRAM_CAR_OWNER_PHONE_LEN;
    }
    else
    {
        memcpy(p,"000000000000",FRAM_CAR_OWNER_PHONE_LEN);
        p += FRAM_CAR_OWNER_PHONE_LEN;
        length += FRAM_CAR_OWNER_PHONE_LEN;
    }
    //��ȡʡ��ID
    PramLen = EepromPram_ReadPram(E2_CAR_PROVINCE_ID, temp);
    if(E2_CAR_PROVINCE_ID_LEN == PramLen)
    {
        memcpy(p,temp,E2_CAR_CITY_ID_LEN);
        p += E2_CAR_CITY_ID_LEN;
        length += E2_CAR_CITY_ID_LEN;
    }
    else
    {
        *p++ = 0;
        *p++ = 0;
        length += 2;
    }

    //��ȡ����ID
    PramLen = EepromPram_ReadPram(E2_CAR_CITY_ID,temp);
    if(E2_CAR_CITY_ID_LEN == PramLen)
    {
        memcpy(p,temp,E2_CAR_CITY_ID_LEN);
        p += E2_CAR_CITY_ID_LEN;
        length += E2_CAR_CITY_ID_LEN;
    }
    else
    {
        *p++ = 0;
        *p++ = 0;
        length += 2;
    }

    //��ȡ������ID
    PramLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, temp);
    if(E2_MANUFACTURE_LEN == PramLen)
    {
        memcpy(p,temp,E2_MANUFACTURE_LEN);
        p += E2_MANUFACTURE_LEN;
        length += E2_MANUFACTURE_LEN;
    }
    else
    {
        memcpy(p,"70108",5);
        p += 5;
        length += 5;
    }
    //��ȡ�ն��ͺ�
    temp[0] = 'E';
    temp[1] = 'G';
    temp[2] = 'S';
    temp[3] = '7';
    temp[4] = '0';
    temp[5] = '1';
    temp[6] = 'B';
    for(i=0; i<13; i++)
    {
        temp[7+i] = 0;
    }
    memcpy(p,temp,20);
    p += 20;
    length += 20;
    //��ȡ�ն�ID
    PramLen = EepromPram_ReadPram(E2_DEVICE_ID, temp);
    if(E2_DEVICE_ID_LEN == PramLen)
    {
        //ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
        q = temp;
        q += 3;
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        temp1[0] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        temp1[1] = BcdHigh + '0';
        temp1[2] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        temp1[3] = BcdHigh + '0';
        temp1[4] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        temp1[5] = BcdHigh + '0';
        temp1[6] = BcdLow + '0';
        memcpy(p,temp1,7);
        p += 7;
        length += 7;

    }
    else
    {
        //��ȡ���ݵ��ն�ID��
        PramLen = EepromPram_ReadPram(E2_DEVICE_BACKUP_ID, temp);
        if(7 == PramLen)
        {

            //ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
            q = temp;
            q += 3;
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            temp1[0] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            temp1[1] = BcdHigh + '0';
            temp1[2] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            temp1[3] = BcdHigh + '0';
            temp1[4] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            temp1[5] = BcdHigh + '0';
            temp1[6] = BcdLow + '0';
            memcpy(p,temp1,7);
            p += 7;
            length += 7;

        }
        else
        {
            //�̶�ID
            temp1[0] = '2';
            temp1[1] = '0';
            temp1[2] = '1';
            temp1[3] = '2';
            temp1[4] = '0';
            temp1[5] = '0';
            temp1[6] = '1';
            memcpy(p,temp1,7);
            p += 7;
            length += 7;
        }
    }
    //��ȡ������ɫ
    PramLen = EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, temp);
    if(1 == PramLen)
    {
        *p++ = temp[0];
        length++;
    }
    else
    {

        *p++ = 0;
        length++;
    }

    //��ȡ���ƺ���
    PramLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, temp);
    if((PramLen < 10)&&(0 != PramLen))
    {
        memcpy(p,temp,PramLen);
        length += PramLen;
        p += PramLen; 
        for(i=0; i<9-PramLen; i++)//����9λ��0x00
        {
        *p++ = 0;
        length++;
        }
    }
    else
    {
        memcpy(p,"��B12345",8);
        length += 8;
        p += 8;
        *p++ = 0;
        length++;
    }
    //��ȡ����VIN��
    PramLen = EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, temp);
    if(0 != PramLen)
    {
        memcpy(p,temp,PramLen);
        length += PramLen;
        p += PramLen; 
        *p++ = 0;
        length++;
    }
    //��ϢID
    head.MessageID = 0x0110;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);

    //��ȡע�����Ϣ����

    if(0 == RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }

}
*/
//ȫ������ƽ̨����ʹ�øú�������֪���б걱���ͼ����޲��죬�ܷ���ݣ���
ProtocolACK RadioProtocol_OpenAccount(u8 channel)
{
    u8  Buffer[100]={0};
    u8  temp[30];
    u8  temp1[20];
    s16 length;
    u8  PramLen; 
    u8  SmsPhone[20];
    u32 key;
    u8 cryptflag;
		u8  flag;
    MESSAGE_HEAD    head;

    key       = 0;
    length    = 0;    
    cryptflag = 0;
	
		EepromPram_ReadPram(E2_TOPLIGHT_CONTROL_ID, &flag);//���ƿ��������˿��ٿ������ܲ�����
    if(1 == flag)
    {
        PramLen =EepromPram_ReadPram(E2_MANUFACTURE_ID, temp);//������ID
        if(E2_MANUFACTURE_LEN == PramLen)
        {
            key = atoi((char *)temp);
            if(key)//
            cryptflag = 1;
        }
    }
    //���ܱ�ʶ,
    Buffer[length++] = cryptflag;
    //������Կ,
    Public_ConvertLongToBuffer(key,&Buffer[length]);
    length += 4;
    //�����ֻ���
    //��eeprom�ж�ȡ�����ֻ���
    PramLen =Public_ReadDataFromFRAM(FRAM_CAR_OWNER_PHONE_ADDR, temp,FRAM_CAR_OWNER_PHONE_LEN);//add by joneming 20130619
    if(FRAM_CAR_OWNER_PHONE_LEN == PramLen)
    {
        memcpy(&Buffer[length],temp,FRAM_CAR_OWNER_PHONE_LEN);
    }
    else
    {
        memcpy(&Buffer[length],"000000000000",FRAM_CAR_OWNER_PHONE_LEN);
    }
    length += FRAM_CAR_OWNER_PHONE_LEN;
    ///////////////////////
    //��ȡʡ��ID
    PramLen = EepromPram_ReadPram(E2_CAR_PROVINCE_ID, temp);
    if(E2_CAR_PROVINCE_ID_LEN == PramLen)
    {
        memcpy(&Buffer[length],temp,E2_CAR_PROVINCE_ID_LEN); 
    }    
    length += E2_CAR_PROVINCE_ID_LEN;
    ////////////////
    //��ȡ����ID
    PramLen = EepromPram_ReadPram(E2_CAR_CITY_ID,temp);
    if(E2_CAR_CITY_ID_LEN == PramLen)
    {
        memcpy(&Buffer[length],temp,E2_CAR_CITY_ID_LEN);
    }    
    length += E2_CAR_CITY_ID_LEN;
    /////////////
    //��ȡ������ID
    PramLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, temp);
    if(E2_MANUFACTURE_LEN == PramLen)
    {
        memcpy(&Buffer[length],temp,E2_MANUFACTURE_LEN);
    }
    else
    {
        memcpy(&Buffer[length],"70108",5);
    }
    length += E2_MANUFACTURE_LEN;
    //////////////////////
    //��ȡ�ն��ͺ�
    memset(temp,0,20);
    GetCurentProductModel((char *)temp);
    memcpy(&Buffer[length],temp,20);
    length += 20;
    //��ȡ�ն�ID
    PramLen = EepromPram_ReadPram(E2_DEVICE_ID, temp);
    if(E2_DEVICE_ID_LEN == PramLen)//ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
    {
        Public_ConvertBcdToAsc(temp1,&temp[3],4);
    }
    else
    {
        //��ȡ���ݵ��ն�ID��
        PramLen = EepromPram_ReadPram(E2_DEVICE_BACKUP_ID, temp);
        if(7 == PramLen)//ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
        { 
            Public_ConvertBcdToAsc(temp1,&temp[3],4);        
        }
        else//�̶�ID
        {
            memset(temp1,0,10);
            strcpy((char *)&temp1[1],"3100028");
        }
    }
    ///��ȡ�ն�ID///////////////
    memcpy(&Buffer[length],&temp1[1],7);
    length += 7;
    ///////////////
    //��ȡ������ɫ
    PramLen = EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, temp);
    if(1 != PramLen)
    {
        temp[0] = 0;
    }
    Buffer[length++] = temp[0];
    //��ȡ���ƺ���
    PramLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, temp);
    if((PramLen < 10)&&(0 != PramLen))
    {
        memcpy(&Buffer[length],temp,PramLen);
    }
    else
    {
        memcpy(&Buffer[length],"��A80055",8);
    }
    length += 9;
    //��ȡ����VIN��
    PramLen = EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, temp);
    if(0 != PramLen)
    {
        memcpy(&Buffer[length],temp,PramLen);
        length += PramLen;
        Buffer[length++] = 0;
    }
    ///////////////////
    if(cryptflag)
    Encrypt(key,&Buffer[5],length-5);
    //��ϢID
    head.MessageID = 0x0110;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);

    //��ȡע�����Ϣ����

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }

}
/*********************************************************************
//��������  :RadioProtocol_TerminalHeartbeat()
//����      :�ն�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0002
*********************************************************************/
ProtocolACK RadioProtocol_TerminalHeartbeat(u8 channel,u8 *Buffer, u8 *SmsPhone )
{
    //u8  Buffer[3] = {0};
   // u8  SmsPhone[20];
    MESSAGE_HEAD    head;

    //��ϢID
    head.MessageID = 0x0002;
    //��Ϣ����
    head.Attribute = 0;
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, 0, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_TerminalLogout(void)
//����      :�ն�ע��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0003
*********************************************************************/
ProtocolACK RadioProtocol_TerminalLogout(u8 channel)
{
    u8  Buffer[3] = {0};
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;


        
#ifdef HUOYUN_DEBUG_OPEN
        if(0 != LogoutFlag)
        {
             return ACK_ERROR;
        }
#endif
 
        
    //��ϢID
    head.MessageID = 0x0003;
    //��Ϣ����
    head.Attribute = 0;
    //��ע����Ϣ����
    
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, 0, SmsPhone))
    {
                LogoutFlag = 1;
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_ResendPacketRequest(void)
//����      :�����ְ�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x8003��ƽ̨���ն˾�ʹ�ø�ָ��������ְ�����
*********************************************************************/
ProtocolACK RadioProtocol_ResendPacketRequest(u8 channel,u8 *pBuffer, u16 BufferLen)
{

    u8  SmsPhone[20];
    MESSAGE_HEAD    head;
 
    if(BufferLen >= 1024)
    {
        return ACK_ERROR;
    }
		
    //��ϢID
    head.MessageID = 0x8003;
    //��Ϣ����
    head.Attribute = BufferLen;
    //��ע����Ϣ����
    
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, BufferLen, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_TerminalQualify(void)
//����      :�ն˼�Ȩ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0102
*********************************************************************/
ProtocolACK RadioProtocol_TerminalQualify(u8 channel)      
{
    u8  Buffer[20] = {"abced"};  
    u8  length;
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;

    //����Ȩ��
		length = EepromPram_ReadPram(E2_LOGINGPRS_PASSWORD_ID, Buffer);//dxl,2016.5.9֮ǰ����û�У�����
		
    //��ϢID
    head.MessageID = 0x0102;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_PostionInformationReport(u8 *pBuffer, u16 length)
//����      :λ����Ϣ�㱨
//����      :pBuffer:ָ�����ݣ�λ����Ϣ��ָ��;BufferLen:���ݵĳ���;
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :    �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0200
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformationReport(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    u16 length;
        //u8      byte;
        //u32     temp;
       // u8      ACC;

    /*�б���ʱʹ��
    //�жϳ���
    if(1 == RecordSendFlag)//�ϴ�¼��ʱҲ���ϴ�λ����Ϣ
        {
        
        }
        else if((BufferLen >= 1024)||(0 != MultiMediaSendFlag))
    {
        return ACK_ERROR;
    }
    */
    if((BufferLen >= 1024)||(BufferLen < 28))
    {
        return ACK_OK;
    }
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0200;
    //��Ϣ����
    head.Attribute = (BufferLen & 0x3ff);

    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_EvenReport(u8 EvenID)
//����      :�¼�����
//����      :EvenID:�¼�ID
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :    �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0301
*********************************************************************/
ProtocolACK RadioProtocol_EvenReport(u8 channel, u8 EvenID)
{
    
    MESSAGE_HEAD    head;
    u16 length;
    u8  Buffer[2];
    u8  SmsPhone[20];
    //��ϢID
    head.MessageID = 0x0301;
    //��Ϣ����
    length =  1;
    head.Attribute = (length & 0x3ff);
    //��Ϣ����
    Buffer[0] = EvenID;

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_QuestionAck(u8 AnswerID)
//����      :����Ӧ��
//����      :AnswerID:Ӧ��ID
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0302
*********************************************************************/
ProtocolACK RadioProtocol_QuestionAck(u8 channel, u8 AnswerID)
{
    
    MESSAGE_HEAD    head;
    u16 length;
    u8  Buffer[3];
    u8  SmsPhone[20];
    //��ϢID
    head.MessageID = 0x0302;
    //��Ϣ����
    length =  3;
    head.Attribute = (length & 0x3ff);
    //��Ϣ����
    Buffer[0] = QuestionAckSerialNum >> 8;
    Buffer[1] = QuestionAckSerialNum;
    Buffer[2] = AnswerID;
    
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_InforamtionOrder(u8 InformationType, u8 ControlFlg)
//����      :��Ϣ�㲥
//����      :InformationType:��Ϣ����;ControlFlg���Ʊ�־
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0303
*********************************************************************/
ProtocolACK RadioProtocol_InforamtionOrder(u8 channel, u8 InformationType, u8 ControlFlg)
{
    
    MESSAGE_HEAD    head;
    u16 length;
    u8  Buffer[2];
    u8  SmsPhone[20];
    //��ϢID
    head.MessageID = 0x0303;
    //��Ϣ����
    length =  2;
    head.Attribute = (length & 0x3ff);
    //��Ϣ����
    Buffer[0] = InformationType;
    Buffer[1] = ControlFlg;

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_RecorderDataReport
//����      :��ʻ��¼�������ϴ�
//����      :pBuffer:ָ�����ݣ��˵���Ϣ��ָ��;BufferLen:���ݳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0700
*********************************************************************/
ProtocolACK RadioProtocol_RecorderDataReport(u8 channel, u8 *pBuffer, u16 BufferLen, u16 TotalPacket, u16 Packet, u8 SerialNumType)
{
  MESSAGE_HEAD  head;
    u8  SmsPhone[20];
    u16 length;
    
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0700;
    //��Ϣ����
  if(TotalPacket > 1)
  {
		head.Attribute = 0x2000;
    head.Attribute |= BufferLen;
    head.TotalPackage = TotalPacket;
    head.CurrentPackage = Packet;
		if(0 == SerialNumType)
		{
      
		}
		else
		{
		  head.Attribute |= 0x8000;//��ˮ�����⴦��
		}
  }
  else
  {
    head.Attribute = (BufferLen&0x3ff);
  }
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_ElectricReceiptReport(u8 *pBuffer, u8 BufferLen)
//����      :�����˵��ϱ�
//����      :pBuffer:ָ�����ݣ��˵���Ϣ��ָ��;BufferLen:���ݳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0701
*********************************************************************/
ProtocolACK RadioProtocol_ElectricReceiptReport(u8 channel, u8 *pBuffer, u16 BufferLen)
{
        MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    u16 length;
    
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0701;
    //��Ϣ����
    head.Attribute = (BufferLen & 0x3ff);

    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_DriverInformationReport(u8 *pBuffer, u8 BufferLen)
//����      :��ʻԱ�����Ϣ�ϱ�
//����      :pBuffer:ָ�����ݣ���ʻԱ��Ϣ��ָ��;BufferLen:���ݳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0702
*********************************************************************/
ProtocolACK RadioProtocol_DriverInformationReport(u8 channel, u8 *pBuffer, u8 BufferLen)
{
    
    MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    u16 length;
    
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0702;
    //��Ϣ����
    head.Attribute = (BufferLen & 0x3ff);

    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_MultiMediaEvenReport(u8 channel, u32 DataID, u8 Type, u8 Format, u8 Even, u8 ChID)
//����      :��ý���¼���Ϣ�ϴ�
//����      :DataID:��ý������ID;Type:��ý������;Format:��ý���ʽ����;Even:�¼������;ChID:ͨ������
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :   �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0800
*********************************************************************/
ProtocolACK RadioProtocol_MultiMediaEvenReport(u8 channel, u32 DataID, u8 Type, u8 Format, u8 Even, u8 ChID)
{
    
    MESSAGE_HEAD    head;
    u16 length;
    u8  Buffer[10];
    u8  *p;
    u8  temp;
    u8  SmsPhone[20];
    
    p = Buffer;

    //��ϢID
    head.MessageID = 0x0800;
    //��Ϣ����
    length = 8;
    head.Attribute = (length & 0x3ff);
    //��Ϣ����
    temp = (DataID & 0xff000000) >> 24;
    *p++ = temp;
    temp = (DataID & 0xff0000) >> 16;
    *p++ = temp;
    temp = (DataID & 0xff00) >> 8;
    *p++ = temp;
    temp = DataID & 0xff;
    *p++ = temp;
    *p++ = Type;
    *p++ = Format;
    *p++ = Even;
    *p++ = ChID;
    
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_MultiMediaDataReport(u8 *pBuffer, u16 BufferLen)
//����      :��ý�������ϴ�
//����      :pBuffer:ָ�����ݣ���Ҫ������ý������ID;��ý������;��ý���ʽ����;�¼������;ͨ������;�����ܰ���;
//      :��ID;��ý�����ݰ�����ָ��;
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :  ָ��ID��0x0801
*********************************************************************/
ProtocolACK RadioProtocol_MultiMediaDataReport(u8 channel, u8 *pBuffer, u16 BufferLen, u16 TotalPacket, u16 Packet)
{
    
    MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    u16 length;
    
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0801;
    //��Ϣ����
        if(TotalPacket != 0)
        {

               head.Attribute = 0x2000;
               head.Attribute |= BufferLen;
               head.TotalPackage = TotalPacket;
               head.CurrentPackage = Packet;
        }
        else
        {
              head.Attribute = (BufferLen&0x3ff);
        }
    
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_MemoryIndexAck(u8 *pBuffer, u16 BufferLen)
//����      :�洢��ý�����Ӧ��
//����      :pBuffer:ָ�����ݣ���Ҫ������ý��ID;��ý������;ͨ��ID;�¼������;λ����Ϣ�㱨;
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :  ָ��ID��0x0802
*********************************************************************/
ProtocolACK RadioProtocol_MemoryIndexAck(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    
    MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    
    //��ϢID
    head.MessageID = 0x0802;
    //��Ϣ����
    head.Attribute = BufferLen;
        //�ж��Ƿ�Ϊ����Ϣ
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, BufferLen, SmsPhone))
    {
            return ACK_OK;
    }
    else
    {
            return ACK_ERROR;
    }   
}
/*********************************************************************
//��������  :RadioProtocol_PhotoAck(u8 *pBuffer, u16 BufferLen)
//����      :����ͷ��������Ӧ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :  ָ��ID��0x0805
*********************************************************************/
ProtocolACK RadioProtocol_PhotoAck(u8 channel, u8 *pBuffer, u16 BufferLen)
{
        MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    
    //��ϢID
    head.MessageID = 0x0805;
    //��Ϣ����
    head.Attribute = BufferLen;
   
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, BufferLen, SmsPhone))
    {
            return ACK_OK;
    }
    else
    {
            return ACK_ERROR;
    }   
}
/*********************************************************************
//��������  :RadioProtocol_OriginalDataUpTrans(u8 Type, u32 TotalLen, u8 *pBuffer, u16 BufferLen)
//����      :��������͸��
//����      :pBuffer:ָ�����ݣ�͸�����ݣ���ָ��;BufferLen:��ǰ͸�����ݰ�����
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :    �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0900
*********************************************************************/
ProtocolACK RadioProtocol_OriginalDataUpTrans(u8 channel, u8 Type, u8 *pBuffer, u16 BufferLen)
{
    unsigned char *buffer;
    u8 SmsPhone[20];
    u16 length;
    MESSAGE_HEAD head;
    if(BufferLen >= 1023)
    {
        return ACK_ERROR;
    }
    buffer =s_ucMsgFlashBuffer;//lzm,2014.9.24
    //ָ���׵�ַ 
    memcpy(buffer+1,pBuffer,BufferLen);
    buffer[0] = Type;
    length = BufferLen+1;
    //��ϢID
    head.MessageID = 0x0900;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);
   
    if(0 != RadioProtocol_SendCmd(channel,&head, buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}

/*********************************************************************
//��������  :RadioProtocol_DataCompressUpTrans(u8 *pBuffer, u16 BufferLen)
//����      :����ѹ���ϴ�
//����      :pBuffer:ָ�����ݣ�ѹ�����ݣ���ָ��;BufferLen:ѹ�����ݳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :    �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0901
*********************************************************************/
ProtocolACK RadioProtocol_DataCompressUpTrans(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    
    u8  temp;
    u16 length;
    u8  SmsPhone[20];
    u8  *p = NULL;
    u8  *p1 = NULL;
    u8  Buffer[1024];
    MESSAGE_HEAD    head;
    p = Buffer;
    p1 = p;
    if(NULL == p)
    {
        return ACK_ERROR;
    }
    //ѹ�����ݳ���
    temp = (BufferLen & 0xff000000) >> 24;
    *p++ = temp;
    temp = (BufferLen & 0xff0000) >> 16;
    *p++ = temp;
    temp = (BufferLen & 0xff00) >> 8;
    *p++ = temp;
    temp = BufferLen & 0xff;
    *p++ = temp;

    memcpy(p,pBuffer,BufferLen);
    p += BufferLen;
    length = BufferLen+4;


    //��ϢID
    head.MessageID = 0x0901;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);

    if(0 != RadioProtocol_SendCmd(channel,&head, p1, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//��������  :RadioProtocol_TerminalRSA(u32 RSAe, u32 RSAn)
//����      :�ն�RSA��Կ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :  �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :ָ��ID��0x0A00
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRSA(u8 channel, u32 RSAe, u8 *pRSAn)
{
    
    u8  temp;
    u16 length;
    u8  Buffer[140];
    u8  SmsPhone[20];
    MESSAGE_HEAD    head;
    
    //�洢eֵ
    temp = (RSAe & 0xff000000) >> 24;
    Buffer[0] = temp;
    temp = (RSAe & 0xff0000) >> 16;
    Buffer[1] = temp;
    temp = (RSAe & 0xff00) >> 8;
    Buffer[2] = temp;
    temp = RSAe & 0xff;
    Buffer[3] = temp;
    //�洢nֵ
    memcpy(Buffer+4,pRSAn,128);

    length = 132;


    //��ϢID
    head.MessageID = 0x0a00;
    //��Ϣ����
    head.Attribute = (length & 0x3ff);

    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
//*****************�������֣��Զ����ƽ̨�·�ָ��**********************
//*****************���Ĳ��֣��Զ�����ն��Ϸ�ָ��**********************
//*****************���岿�֣�����ӿں���**********************
/*********************************************************************
//��������  :RadioProtocol_UpdateTerminalPhoneNum(void)
//����      :�����ն˵绰����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :�ϵ��ʼ��ʱ�����
*********************************************************************/
void RadioProtocol_UpdateTerminalPhoneNum(void)
{
    u8      Buffer[13];
    u8  BufferLen;

    BufferLen = EepromPram_ReadPram(E2_DEVICE_PHONE_ID, Buffer);
    if(E2_DEVICE_PHONE_ID_LEN == BufferLen)
    {
        memcpy(TerminalPhoneNumber,Buffer,E2_DEVICE_PHONE_ID_LEN);
    }
    else
    {
        //���ֻ����뱸������
        BufferLen = EepromPram_ReadPram(E2_DEVICE_PHONE_BACKUP_ID, Buffer);
        if(E2_DEVICE_PHONE_BACKUP_ID_LEN == BufferLen)
        {
            memcpy(TerminalPhoneNumber,Buffer,E2_DEVICE_PHONE_BACKUP_ID_LEN);
        }
        else
        {
                    /*dxl,2014.4.29���Σ���Ϊ����
                    TerminalPhoneNumber[0] = 0x01;
                    TerminalPhoneNumber[1] = 0x37;
                    TerminalPhoneNumber[2] = 0x60;
                    TerminalPhoneNumber[3] = 0x25;
                    TerminalPhoneNumber[4] = 0x00;
                    TerminalPhoneNumber[5] = 0x00;
                    */
            //���ն�ID
            BufferLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer);
            if(E2_DEVICE_ID_LEN == BufferLen)
            {
                Buffer[1] = Buffer[1]&0x0f;//ǿ�Ƹ�4λΪ0
                memcpy(TerminalPhoneNumber,Buffer+1,E2_DEVICE_PHONE_BACKUP_ID_LEN);//ȡ�ն�ID�ŵĺ���11λ
            }
            else
            {
                TerminalPhoneNumber[0] = 0x01;
                TerminalPhoneNumber[1] = 0x37;
                TerminalPhoneNumber[2] = 0x60;
                TerminalPhoneNumber[3] = 0x25;
                TerminalPhoneNumber[4] = 0x00;
                TerminalPhoneNumber[5] = 0x00;
            }
            
        }

    }
}
/*********************************************************************
//��������  :RadioProtocol_GetRadioProtocolRxBufferBusyFlag(void)
//����      :�����ջ���æ��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :ͨ��ģ����յ����ݺ�����ô˺���,�ж��Ƿ�����Э����ջ��忽������
//      :������ֵΪ0��ʾ���п�����Э����ջ��壬Ϊ1��ʾ��æ����ǰ���ݶ���
*********************************************************************/
u8  RadioProtocol_GetRadioProtocolRxBufferBusyFlag(void)
{
    
    return RadioProtocolRxBufferBusyFlag;
}
/*********************************************************************
//��������  :RadioProtocol_SetRadioProtocolRxBufferBusyFlag
//����      :��λRadioProtocolRxBufferBusyFlag
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :����Э����ջ��忽�������ݺ�����λЭ����ջ���æ��׼,
//      :ֻ�е�Э����ջ���������ݶ����������ñ�׼��ϵͳ�Զ���0
*********************************************************************/
void RadioProtocol_SetRadioProtocolRxBufferBusyFlag(void)
{
    //��λæ��־
    RadioProtocolRxBufferBusyFlag = 1;
}
/*********************************************************************
//��������  :RadioProtocol_AddRadioParseList(void)
//����      :������ݵ����߽����б�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :����Э����ջ��忽�������ݺ�����ô˺���,���½��յ�������
//      :������ͬʱ�յ��ü�������,����Ҫ��һ�����У��Ǽǽ������б�,
//      :Ȼ�󴥷���������
*********************************************************************/
void RadioProtocol_AddRadioParseList(void)
{
    
}

/*********************************************************************
//��������  :RadioProtocol_AddRadioParseList(void)
//����      :������ݵ����߽����б�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :����Э����ջ��忽�������ݺ�����ô˺���,���½��յ�������
//      :������ͬʱ�յ��ü�������,����Ҫ��һ�����У��Ǽǽ������б�,
//      :Ȼ�󴥷���������
*********************************************************************/
void RadioProtocol_UpdateRadioParseList(void)
{
    
    
}

/*********************************************************************
//��������  :RadioProtocol_AddRecvDataForParse
//����      :�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :����Э����ջ��忽�������ݺ�����ô˺���,���½��յ�������
//      :������ͬʱ�յ��ü�������,����Ҫ��һ�����У��Ǽǽ������б�,
//      :Ȼ�󴥷���������
*********************************************************************/
void RadioProtocol_AddRecvDataForParse(unsigned char channel,unsigned char *recvData,unsigned short recvDataLen)
{
    if(CHANNEL_SMS == channel)//����ͨ��
    {
        QueueInBuffer(recvData, recvDataLen, RadioProtocolRxSmsQ); 
    }
    else if(CHANNEL_DATA_1 == channel)//����ͨ������1
    {
        QueueInBuffer(recvData, recvDataLen, RadioProtocolRx1Q);
			  
    }
    else if(CHANNEL_DATA_2 == channel)//����ͨ������2
    {
        QueueInBuffer(recvData, recvDataLen, RadioProtocolRx2Q);
    }
}

/*********************************************************************
//��������  :RadioProtocol_ParseTimeTask(void)
//����      :���߽���ʱ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :0.1���ӵ���1�Σ������������š�����ͨ��1������ͨ��2��3�����У�
//          :ÿ��ȡ��һ�����ݽ��н�����ÿ��������10��ĳ�ʱʱ�䣬��10����
//          :��û�з���֡β��0x7E����������������֡
*********************************************************************/
FunctionalState RadioProtocolParse_TimeTask(void)
{
    static u8 ParseCh = CHANNEL_SMS;//��һ�ν�����ͨ��
    static u8 ParseFlag = 0;//������־,0��ʾ����������û������,1��ʾ�ҵ�֡ͷ����,��û���ҵ�֡β
    static u8 TimeCount = 0;
    static u16 length = 0;
    static Queue LastQ;
    
    Queue Q;
    u8  i;
    u8  Flag;//����������Ч��־��1Ϊ��Ч��0Ϊ��Ч���Ѵﵽ����β��
    u8  Data;
    
    u8  *p = NULL;
    
    
    
    if(0 == ParseFlag)//�������
    {
        TimeCount = 0;
        for(i=1; i<4; i++)
        {
            if(CHANNEL_SMS == ParseCh)
            {
                Q = RadioProtocolRxSmsQ;
            }
            else if(CHANNEL_DATA_1 == ParseCh)
            {
                Q = RadioProtocolRx1Q;
            }
            else if(CHANNEL_DATA_2 == ParseCh)
            {
                Q = RadioProtocolRx2Q;
            }
            else
            {
                Q = RadioProtocolRxSmsQ;
                ParseCh = CHANNEL_SMS;
            }
            if(QueueIsEmpty(Q))//��
            {
                //�л�ͨ��
                ParseCh = RadioProtocol_SwitchChannel(ParseCh);
            }
            else//�ǿ�
            {
                break;
            }
        }
        if(4 == i)//3��������о�û��������Ҫ����
        {
            
        }
        else//����Ҫ����������,��ȡЭ��֡
        {
            while(1)
            {
                Data = QueueFrontAndOut2(Q, &Flag);
                if(Flag)
                {
                    if(0x7e == Data)
                    {
                        if(0 == ParseFlag)//֡��ʼ
                        {
                            ParseFlag = 1;
                            length = 0;
                            p = RadioProtocolParseBuffer;
                            LastQ = Q;
                        }
                        else
                        {
                            if(length > 0)//֡����
                            {
                                RadioProtocol_ProtocolParse(ParseCh,RadioProtocolParseBuffer, length);
                                                            
                                ParseFlag = 0;
                                //�л�ͨ��
                                ParseCh = RadioProtocol_SwitchChannel(ParseCh);
                                break;
                            }
                        }
                    }
                    else
                    {
                        *p++ = Data;
                        length++;
                        if(length >= RADIO_PROTOCOL_BUFFER_SIZE)
                        {
                            ParseFlag = 0;
                            ParseCh = CHANNEL_SMS;
                            break;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
    else//�������ݰ�ƴ�ӳ�һ֡����
    {
        Q = LastQ;
        TimeCount++;
        if(TimeCount > 20)//3����ʱ
        {
            ParseFlag = 0;
            ParseCh = CHANNEL_SMS;
        }
        while(1)
        {
            Data = QueueFrontAndOut2(Q, &Flag);
            if(Flag)
            {
                if(0x7e == Data)
                {
                     if(length > 0)//֡����
                     {
                            RadioProtocol_ProtocolParse(ParseCh,RadioProtocolParseBuffer, length);
                            ParseFlag = 0;
                            //�л�ͨ��
                            ParseCh = RadioProtocol_SwitchChannel(ParseCh);
                            break;
                     }
                }
                else
                {
                    RadioProtocolParseBuffer[length] = Data;
                    length++;
                    if(length >= RADIO_PROTOCOL_BUFFER_SIZE)
                    {
                        ParseFlag = 0;
                        ParseCh = CHANNEL_SMS;
                        break;
                    }
                }
            }
            else
            {
                
                break;
            }
        }
    }

    return ENABLE;
}
/*********************************************************************
//��������  :RadioProtocol_SwitchChannel
//����      :�л�ͨ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :CHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2����ͨ�������л�
*********************************************************************/
u8 RadioProtocol_SwitchChannel(u8 channel)
{
    u8 NewChannel;
    
    if(CHANNEL_SMS == channel)
    {
        NewChannel = CHANNEL_DATA_1;
    }
    else if(CHANNEL_DATA_1 == channel)
    {
        NewChannel = CHANNEL_DATA_2;
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        NewChannel = CHANNEL_SMS;
    }
    else
    {
        NewChannel = 0;
    }
    
    return NewChannel;
}
/*********************************************************************
//��������  :RadioProtocol_SendMessage(MESSAGE_HEAD head, u8 *SrcData, u16 SrcLen, u8 SendCh)
//����      :������Ϣ
//����      :MESSAGE_HEAD   head:��Ϣͷ;SrcData:ָ����Ϣ���ָ��;
//          :SrcLen:��Ϣ�峤��;SendCh:����ͨ��,��ѡֵֻ��ΪCHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2,
//          :������������ֵ�����
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :bit0��Ӧ��CHANNEL_SMSͨ��,bit1��Ӧ��CHANNLE_DATA_1ͨ��,bit2��Ӧ��CHANNLE_DATA_2ͨ��,
//          :����bit��ʱ����,��Ӧ��λΪ1��ʾ��ͨ�����ͳɹ���
//��ע      :������һ��ָ���ͨ����ҪӦ��Ӧ��ʱ����øú�����
//      :�����Ϸ�һ����Ϣ����ʱҲ��Ҫ���øú�����
*********************************************************************/
/*
u8 RadioProtocol_SendCmd(u8 SendCh , MESSAGE_HEAD   head, u8 *SrcData, u16 SrcLen, u8 *pSmsPhone)
{
    u8  temp2[20];
    u8  temp;
    u8  VerifyCode;
    u8  *p;
    u8  flag = 0;
    u16 i;
    u16 HeadLen;
    u16 length;
    u16 SendLen;
    
    *****************����ͨ������*******************
    if(CHANNEL_SMS == (SendCh&CHANNEL_SMS))
    {
        head.SerialNum = SmsChannelSerialNum;
        SmsChannelSerialNum++;

        if(0x2000 == (0x2000&head.Attribute))//���Ƿְ����ݣ���ˮ��ȡ�ְ����
        {
            head.SerialNum = head.CurrentPackage;//�б���ʱҪ��ְ����ݵ���ˮ��Ҫ����
        }
    
        //�ն��ֻ���
        memcpy(head.PhoneNum,TerminalPhoneNumber,6);
    
        //MESSAGE_HEAD�ṹ����ֽ���
        HeadLen = RadioProtocol_HeadtoBytes(temp2,&head);
        //����У����
        VerifyCode = 0;
        p = temp2;
        for(i=0; i<HeadLen; i++)
        {
            temp = *p;
            VerifyCode = VerifyCode ^ temp;
            p++;
        }
        p = SrcData;
        for(i=0; i<SrcLen; i++)
        {
            temp = *p;
            VerifyCode = VerifyCode ^ temp;
            p++;
        }
        //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
        memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
        if(SrcLen > 0)
        {
            memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
        }
        RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
        //ת��
        length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
        length++;
        //����һͷһβ�ı�־λ
        RadioProtocolTxBuffer[0] = 0x7e;
        RadioProtocolTxBuffer[length] = 0x7e;
        length++;

        //����
        SendLen = Communication_SndMsg2Svr(CHANNEL_SMS, RadioProtocolTxBuffer, length, pSmsPhone);
        if(length == SendLen)
        {
            flag = flag|CHANNEL_SMS;
        }
    }
    ****************����ͨ��1����*******************

    if(CHANNEL_DATA_1 == (SendCh&CHANNEL_DATA_1))
    {
        head.SerialNum = Data1ChannelSerialNum;
        Data1ChannelSerialNum++;
  
        if(0x2000 == (0x2000&head.Attribute))//���Ƿְ����ݣ���ˮ��ȡ�ְ����
        {
            head.SerialNum = head.CurrentPackage;//�б���ʱҪ��ְ����ݵ���ˮ��Ҫ����
        }
    
    
        if(CHANNEL_DATA_1 != (CHANNEL_DATA_1&TerminalAuthorizationFlag))//Ȩ�޴���(ֻ����Ӧ��ͨ����Ȩ�ɹ������������ָ��)
        {
            if(0x0100 == head.MessageID)//ע��
            {
          
            }
            else if(0x0003 == head.MessageID)//�ն�ע��
            {
        
            }
            else if(0x0102 == head.MessageID)//��Ȩ
            {

            }
            else if(0x0110 == head.MessageID)//��������
            {
        
            }
            else
            {
                flag = flag|CHANNEL_DATA_1;
            }
        }
    
        if(CHANNEL_DATA_1 != (flag&CHANNEL_DATA_1))
        {
            //�ն��ֻ���
            memcpy(head.PhoneNum,TerminalPhoneNumber,6);
    
            //MESSAGE_HEAD�ṹ����ֽ���
            HeadLen = RadioProtocol_HeadtoBytes(temp2,&head);
            //����У����
            VerifyCode = 0;
            p = temp2;
            for(i=0; i<HeadLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            p = SrcData;
            for(i=0; i<SrcLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
            memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
            if(SrcLen > 0)
            {
                memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
            }
            RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
            //ת��
            length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
            length++;
            //����һͷһβ�ı�־λ
            RadioProtocolTxBuffer[0] = 0x7e;
            RadioProtocolTxBuffer[length] = 0x7e;
            length++;

            //����
            SendLen = Communication_SndMsg2Svr(CHANNEL_DATA_1, RadioProtocolTxBuffer, length, pSmsPhone);
            if(length == SendLen)
            {
                flag = flag|CHANNEL_DATA_1;
                        #ifdef GPRS_DATA_PRINT_TO_DB9
                        //VDRUsart_SendData(RadioProtocolTxBuffer,length);
												QueueInBuffer(RadioProtocolTxBuffer,length,VdrTxQ); //��ӡ��DB9��
                        #endif
            }
        }
				else
				{
				    flag = 0;
				}
    }
    ****************����ͨ��2����*******************
    if(CHANNEL_DATA_2 == (SendCh&CHANNEL_DATA_2))
    {
        head.SerialNum = Data2ChannelSerialNum;
        Data2ChannelSerialNum++;

        if(0x2000 == (0x2000&head.Attribute))//���Ƿְ����ݣ���ˮ��ȡ�ְ����
        {
            head.SerialNum = head.CurrentPackage;//�б���ʱҪ��ְ����ݵ���ˮ��Ҫ����
        }
        
        if(CHANNEL_DATA_2 != (CHANNEL_DATA_2&TerminalAuthorizationFlag))//Ȩ�޴���(ֻ����Ӧ��ͨ����Ȩ�ɹ������������ָ��)
        {
            if(0x0100 == head.MessageID)//ע��
            {
          
            }
            else if(0x0003 == head.MessageID)//�ն�ע��
            {
        
            }
            else if(0x0102 == head.MessageID)//��Ȩ
            {

            }
            else if(0x0110 == head.MessageID)//��������
            {
        
            }
            else
            {
                flag = flag|CHANNEL_DATA_2;
            }
        }
        if(CHANNEL_DATA_2 != (flag&CHANNEL_DATA_2))
        {
            //�ն��ֻ���
            memcpy(head.PhoneNum,TerminalPhoneNumber,6);
    
            //MESSAGE_HEAD�ṹ����ֽ���
            HeadLen = RadioProtocol_HeadtoBytes(temp2,&head);
            //����У����
            VerifyCode = 0;
            p = temp2;
            for(i=0; i<HeadLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            p = SrcData;
            for(i=0; i<SrcLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
            memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
            if(SrcLen > 0)
            {
                memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
            }
            RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
            //ת��
            length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
            length++;
            //����һͷһβ�ı�־λ
            RadioProtocolTxBuffer[0] = 0x7e;
            RadioProtocolTxBuffer[length] = 0x7e;
            length++;

            //����
            SendLen = Communication_SndMsg2Svr(CHANNEL_DATA_2, RadioProtocolTxBuffer, length, pSmsPhone);
            if(length == SendLen)
            {
                flag = flag|CHANNEL_DATA_2;
                        #ifdef GPRS_DATA_PRINT_TO_DB9
                        VDRUsart_SendData(RadioProtocolTxBuffer,length);
                        #endif
            }
        }
				else
				{
				    flag = 0;
				}
    }
    return flag;
}
*/
u8 RadioProtocol_SendCmd(u8 SendCh , MESSAGE_HEAD   *head, u8 *SrcData, u16 SrcLen, u8 *pSmsPhone)
{
    u8  temp2[20];
    u8  temp;
    u8  VerifyCode;
    u8  *p;
    u8  flag = 0;
    u16 i;
    u16 HeadLen;
    u16 length;
    u16 SendLen;
	  
	  static u16 Link1PackageSerialNum = 0;//�ְ���ˮ��
	  static u16 Link2PackageSerialNum = 0;
	  static u16 SmsPackageSerialNum = 0;
    
    //*****************����ͨ������*******************
    if(CHANNEL_SMS == (SendCh&CHANNEL_SMS))
    {
			/*
        head->SerialNum = SmsChannelSerialNum;
        SmsChannelSerialNum++;

        if(0x2000 == (0x2000&head->Attribute))//���Ƿְ����ݣ���ˮ��ȡ�ְ����
        {
            head->SerialNum = head->CurrentPackage;//�б���ʱҪ��ְ����ݵ���ˮ��Ҫ����
        }
			*/
			  if(0x2000 == (0x2000&head->Attribute))//dxl,2016.5.13�ְ����ݴ���ʱ��ˮ��Ҫ�������ҵ�1�������1������ˮ��Ҫ��ǰ/�����ݵ���ˮ���ν���
        {
					  if(1 == head->CurrentPackage)//��1�������ִ���ֻ�ʺ��ڵ�ǰֻ��һ�ְַ������ڴ��䣬���ж��ְַ�������ͬʱ���䣬����ˮ�Ż��ܵ����ţ����ʱһ�㲻�����
						{
						    SmsPackageSerialNum = SmsChannelSerialNum;
							  Data1ChannelSerialNum += head->TotalPackage;
						}
						head->SerialNum = SmsPackageSerialNum;
            SmsPackageSerialNum++;
        }
				else//���ְ�
				{
				    head->SerialNum = SmsChannelSerialNum;
            SmsChannelSerialNum++;
				}
    
        //�ն��ֻ���
        memcpy(head->PhoneNum,TerminalPhoneNumber,6);
    
        //MESSAGE_HEAD�ṹ����ֽ���
        HeadLen = RadioProtocol_HeadtoBytes(temp2,head);
        //����У����
        VerifyCode = 0;
        p = temp2;
        for(i=0; i<HeadLen; i++)
        {
            temp = *p;
            VerifyCode = VerifyCode ^ temp;
            p++;
        }
        p = SrcData;
        for(i=0; i<SrcLen; i++)
        {
            temp = *p;
            VerifyCode = VerifyCode ^ temp;
            p++;
        }
        //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
        memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
        if(SrcLen > 0)
        {
            memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
        }
        RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
        //ת��
        length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
        length++;
        //����һͷһβ�ı�־λ
        RadioProtocolTxBuffer[0] = 0x7e;
        RadioProtocolTxBuffer[length] = 0x7e;
        length++;

        //����
        SendLen = Communication_SndMsg2Svr(CHANNEL_SMS, RadioProtocolTxBuffer, length, pSmsPhone);
        if(length == SendLen)
        {
            flag = flag|CHANNEL_SMS;
        }
				else
				{
				    flag = flag&0xFE;
				}
    }
    //****************����ͨ��1����*******************

    if(CHANNEL_DATA_1 == (SendCh&CHANNEL_DATA_1))
    {
        if(CHANNEL_DATA_1 != (CHANNEL_DATA_1&TerminalAuthorizationFlag))//Ȩ�޴���(ֻ����Ӧ��ͨ����Ȩ�ɹ������������ָ��)
        {
            if(0x0100 == head->MessageID)//ע��
            {
                flag = flag|CHANNEL_DATA_1;
            }
            else if(0x0003 == head->MessageID)//�ն�ע��
            {
                flag = flag|CHANNEL_DATA_1;
            }
            else if(0x0102 == head->MessageID)//��Ȩ
            {
                flag = flag|CHANNEL_DATA_1;
            }
            else if(0x0110 == head->MessageID)//��������
            {
                flag = flag|CHANNEL_DATA_1;
            }
            else
            {
                flag = flag&0xFD;
            }
        }
				else
				{
				    flag = flag|CHANNEL_DATA_1;
				}
    
        if(CHANNEL_DATA_1 == (flag&CHANNEL_DATA_1))
        {
					  if(0x8000 == (0x8000&head->Attribute))//dxl,2016.5.31�����ڷְ����������
						{
						     head->SerialNum = Link1FirstPackageSerialNum+head->CurrentPackage-1;
						}
					  else if(0x2000 == (0x2000&head->Attribute))//dxl,2016.5.18֮ǰ��if(CHANNEL_DATA_1 != (CHANNEL_DATA_1&TerminalAuthorizationFlag))����
            {
					      if(1 == head->CurrentPackage)//dxl,2016.5.13��1�������ִ���ֻ�ʺ��ڵ�ǰֻ��һ�ְַ������ڴ��䣬���ж��ְַ�������ͬʱ���䣬����ˮ�Ż��ܵ����ţ����ʱһ�㲻�����
						    {
						        Link1PackageSerialNum = Data1ChannelSerialNum;
							      Data1ChannelSerialNum += head->TotalPackage;
						    }
						    head->SerialNum = Link1PackageSerialNum;
                Link1PackageSerialNum++;
            }
				    else//���ְ�
				    {
				        head->SerialNum = Data1ChannelSerialNum;
                Data1ChannelSerialNum++;
				    }
    
            //�ն��ֻ���
            memcpy(head->PhoneNum,TerminalPhoneNumber,6);
    
            //MESSAGE_HEAD�ṹ����ֽ���
            HeadLen = RadioProtocol_HeadtoBytes(temp2,head);
            //����У����
            VerifyCode = 0;
            p = temp2;
            for(i=0; i<HeadLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            p = SrcData;
            for(i=0; i<SrcLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
            memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
            if(SrcLen > 0)
            {
                memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
            }
            RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
            //ת��
            length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
            length++;
            //����һͷһβ�ı�־λ
            RadioProtocolTxBuffer[0] = 0x7e;
            RadioProtocolTxBuffer[length] = 0x7e;
            length++;

            //����
            SendLen = Communication_SndMsg2Svr(CHANNEL_DATA_1, RadioProtocolTxBuffer, length, pSmsPhone);
            if(length == SendLen)
            {
                flag = flag|CHANNEL_DATA_1;
                        //#ifdef GPRS_DATA_PRINT_TO_DB9
//							          QueueInBuffer(RadioProtocolTxBuffer,length,VdrTxQ); //��ӡ��DB9��
//							VDRUsart_SendData(RadioProtocolTxBuffer,length);
                        //#endif
            }
						else
						{
						    flag = flag&0xFD;
						}
        }
				else
				{
				    flag = flag&0xFD;
				}
    }
    //****************����ͨ��2����*******************
    if(CHANNEL_DATA_2 == (SendCh&CHANNEL_DATA_2))
    {
			/*
        head->SerialNum = Data2ChannelSerialNum;
        Data2ChannelSerialNum++;

        if(0x2000 == (0x2000&head->Attribute))//���Ƿְ����ݣ���ˮ��ȡ�ְ����
        {
            head->SerialNum = head->CurrentPackage;//�б���ʱҪ��ְ����ݵ���ˮ��Ҫ����
        }
			*/
        /*
        if(CHANNEL_DATA_2 != (CHANNEL_DATA_2&TerminalAuthorizationFlag))//Ȩ�޴���(ֻ����Ӧ��ͨ����Ȩ�ɹ������������ָ��)
        {
            if(0x0100 == head->MessageID)//ע��
            {
                flag = flag|CHANNEL_DATA_2;
            }
            else if(0x0003 == head->MessageID)//�ն�ע��
            {
                flag = flag|CHANNEL_DATA_2;
            }
            else if(0x0102 == head->MessageID)//��Ȩ
            {
                flag = flag|CHANNEL_DATA_2;
            }
            else if(0x0110 == head->MessageID)//��������
            {
                flag = flag|CHANNEL_DATA_2;
            }
						else if(0x0900 == head->MessageID)//����͸����IC����֤ʱ�õ���
            {
                flag = flag|CHANNEL_DATA_2;
            }
            else if(0x0702 == head->MessageID)//��ʻԱǩ��ǩ�ˣ�IC����֤ʱ�õ���
            {
                flag = flag|CHANNEL_DATA_2;
            }
            else
            {
                flag = flag&0xFB;
            }
        }
				else
				{
				    flag = flag|CHANNEL_DATA_2;
				}
				*/
				flag = flag|CHANNEL_DATA_2;
        if(CHANNEL_DATA_2 == (flag&CHANNEL_DATA_2))
        {
					  if(0x2000 == (0x2000&head->Attribute))//dxl,2016.5.13�ְ����ݴ���ʱ��ˮ��Ҫ�������ҵ�1�������1������ˮ��Ҫ��ǰ/�����ݵ���ˮ���ν���
            {
					      if(1 == head->CurrentPackage)//��1�������ִ���ֻ�ʺ��ڵ�ǰֻ��һ�ְַ������ڴ��䣬���ж��ְַ�������ͬʱ���䣬����ˮ�Ż��ܵ����ţ����ʱһ�㲻�����
						    {
						        Link2PackageSerialNum = Data2ChannelSerialNum;
							      Data2ChannelSerialNum += head->TotalPackage;
						    }
						    head->SerialNum = Link2PackageSerialNum;
                Link2PackageSerialNum++;
            }
				    else//���ְ�
				    {
				        head->SerialNum = Data2ChannelSerialNum;
                Data2ChannelSerialNum++;
				    }
				
            //�ն��ֻ���
            memcpy(head->PhoneNum,TerminalPhoneNumber,6);
    
            //MESSAGE_HEAD�ṹ����ֽ���
            HeadLen = RadioProtocol_HeadtoBytes(temp2,head);
            //����У����
            VerifyCode = 0;
            p = temp2;
            for(i=0; i<HeadLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            p = SrcData;
            for(i=0; i<SrcLen; i++)
            {
                temp = *p;
                VerifyCode = VerifyCode ^ temp;
                p++;
            }
            //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
            memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
            if(SrcLen > 0)
            {
                memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
            }
            RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
            //ת��
            length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
            length++;
            //����һͷһβ�ı�־λ
            RadioProtocolTxBuffer[0] = 0x7e;
            RadioProtocolTxBuffer[length] = 0x7e;
            length++;

            //����
            SendLen = Communication_SndMsg2Svr(CHANNEL_DATA_2, RadioProtocolTxBuffer, length, pSmsPhone);
            if(length == SendLen)
            {
                flag = flag|CHANNEL_DATA_2;
                       /// #ifdef GPRS_DATA_PRINT_TO_DB9
//							          QueueInBuffer(RadioProtocolTxBuffer,length,VdrTxQ); //��ӡ��DB9��
//							         VDRUsart_SendData(RadioProtocolTxBuffer,length);
                        //#endif
            }
						else
						{
						    flag = flag&0xFB;
						}
        }
				else
				{
				    flag = flag&0xFB;
				}
    }
    return flag;
}
//*****************�������֣�ģ���ڲ�����**********************
/*********************************************************************
//��������  :RadioProtocol_ProtocolParse(u8 *pBuffer, u16 BufferLen)
//����      :Э�����,����һ��ָ��
//����      :pBuffer:ָ������(������������)��ָ��;BufferLen:���ݵĳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :�ɹ�,����SUCCESS;ʧ��,����ERROR
//��ע      :ƽ̨�·�������������ɸú����������ú������������
//      :��7E��ͷ,7E��β��һ���ֽ�����
*********************************************************************/
ErrorStatus RadioProtocol_ProtocolParse(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    u8  *p;
    static u8   HighByte;
    static u8   LowByte;
    static u8   IDHighByte;
    static u8   IDLowByte;
    static u8   Index;
    u16 length;
    u16 MessageLen;
    u8  offset;
    u8  VerifyCode;
    u16 i;
    
    
	  
	
    if((channel == CHANNEL_SMS)||(channel == CHANNEL_DATA_1)||(channel == CHANNEL_DATA_2))
    {
    
    }
    else
    {
        return ERROR;
    }
    //������������0
    Heartbeat_ClrHeartbeatCount(channel);
    
        ResetTimeCount = 0;
    
    //��ת��
    length = unTransMean(pBuffer, BufferLen);

    //���У����
    VerifyCode = 0;
    for(i=0; i<length-1; i++)
    {
        VerifyCode = VerifyCode ^ pBuffer[i];
    }
    if(VerifyCode != pBuffer[length-1])//dxl,2013.11.30����У��,��ֹ��������
    {
        return ERROR;
    }
		
		//QueueInBuffer(pBuffer,length,VdrTxQ); //��ӡ��DB9��
		
    //ָ����ջ���
    p = pBuffer;

    //��ȡ��ϢID�����������ֽ�������������ֽ���ǰ
    IDHighByte = *p++;
    IDLowByte = *p++;
    RadioProtocolRxHead.MessageID = (IDHighByte << 8)|IDLowByte;

    //��ȡ��Ϣ������
    HighByte = *p++;
    LowByte = *p++;
    RadioProtocolRxHead.Attribute = (HighByte << 8)|LowByte;

    //��Ϣ����
    MessageLen = RadioProtocolRxHead.Attribute & 0x03ff;

    //�ն��ֻ���
    memcpy(RadioProtocolRxHead.PhoneNum,p,6);
    p += 6;
        
        //�ж��ֻ���
       // if(0 != strncmp((char const *)RadioProtocolRxHead.PhoneNum,(char const *)TerminalPhoneNumber,6))//dxl,2013.11.30����У��,��ֹ��������
       // {
                 // return ERROR;
       // }
        
        
   //�жϳ���
    if(0x2000 == (RadioProtocolRxHead.Attribute&0x2000))//�зְ���
    {
        length -= 16;//����Ϣͷ16�ֽ�
        length--;//��У����1�ֽ�
    }
    else//�޷ְ���
    {
        length -= 12;//����Ϣͷ12�ֽ�
        length--;//��У����1�ֽ�
    }
    if(length != MessageLen)
    {
         return ERROR;
    }
        

    //��Ϣ��ˮ��
    HighByte = *p++;
    LowByte = *p++;
    RadioProtocolRxHead.SerialNum = (HighByte << 8)|LowByte;

    //��ȡ��Ϣ����װ��
    if(0x2000 == (RadioProtocolRxHead.Attribute & 0x2000))
    {
        HighByte = *p++;
        LowByte = *p++;
        RadioProtocolRxHead.TotalPackage = (HighByte << 8)|LowByte;
        HighByte = *p++;
        LowByte = *p++;
        RadioProtocolRxHead.CurrentPackage = (HighByte << 8)|LowByte;
        offset = 16;
    }
    else
    {
        RadioProtocolRxHead.TotalPackage = 0;
        RadioProtocolRxHead.CurrentPackage = 0;
        offset = 12;
    }
    
    RadioProtocol_UpdateDownloadCmdList(RadioProtocolRxHead.MessageID,RadioProtocolRxHead.SerialNum,channel);

   //����任����ԷǱ�׼���
    if((0x88==IDHighByte)&&(0x11==IDLowByte))
    {
        IDLowByte = 0x07;
    }
    //��������
    if(IDLowByte > IDLowByteMax[IDHighByte-0x80])
    {
        return ERROR;
    }
    else
    {
        Index = CmdOperationIndex[IDHighByte-0x80]+IDLowByte;
        if(Index >= MAX_PROTOCOL_FUN_LIST)
        {
            return ERROR;
        }
        else
        {
            //ִ�к���
            (CmdOperate[Index])(channel,pBuffer+offset, MessageLen);
        }
    }
    return SUCCESS;
}
/*********************************************************************
//��������  :RadioProtocol_GetDownloadCmdChannel(u16 DownloadCmd, u16 *SerialNum)
//����      :��ȡ�����������ˮ�ţ�ͨ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :ͨ����
//��ע      :ֻ�ܻ�ȡ���µ������������ˮ�ţ�ͨ����
*********************************************************************/
u8 RadioProtocol_GetDownloadCmdChannel(u16 DownloadCmd, u16 *SerialNum)
{
    u16 i;
    u8  channel = 0;
    
    for(i=0; i<DOWNLOAD_CMD_NUM; i++)
    {
        if(DownloadCmdList[i][0] == DownloadCmd)
        {
            *SerialNum = DownloadCmdList[i][1];
            channel = DownloadCmdList[i][2];
            break;
        }
    }
    return channel;
}
/*********************************************************************
//��������  :RadioProtocol_GetUploadCmdChannel(u16 UploadCmd, u16 *SerialNum)
//����      :��ȡ�����������ˮ�ţ�ͨ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :ͨ����
//��ע      :ֻ�ܻ�ȡ���µ������������ˮ�ţ�ͨ����
*********************************************************************/
u8 RadioProtocol_GetUploadCmdChannel(u16 UploadCmd, u16 *SerialNum)
{
    u16 i;
    u8 channel = 0;
    
    for(i=0; i<DOWNLOAD_CMD_NUM; i++)
    {
        if(UploadCmdList[i][0] == UploadCmd)
        {
            *SerialNum = UploadCmdList[i][1];
            channel = UploadCmdList[i][2];
            break;
        }
    }
    return channel;
}
/*********************************************************************
//��������  :RadioProtocol_UpdateDownloadCmdList(u16 Cmd, u16 SerialNum, u8 channel)
//����      :�������������б�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :������ǰ�����ˮ�ţ�ͨ���ű����������Ա����ʹ��
*********************************************************************/
ErrorStatus RadioProtocol_UpdateDownloadCmdList(u16 Cmd, u16 SerialNum, u8 channel)
{
    u16 i;
    
    for(i=0; i<DOWNLOAD_CMD_NUM; i++)
    {
        if(DownloadCmdList[i][0] == Cmd)
        {
            DownloadCmdList[i][1] = SerialNum;
            DownloadCmdList[i][2] = channel;
            return SUCCESS;
        }
    }
    return ERROR;
}
/*********************************************************************
//��������  :RadioProtocol_UpdateUploadCmdList(u16 Cmd, u16 SerialNum, u8 channel)
//����      :�������������б�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :������ǰ�����ˮ�ţ�ͨ���ű����������Ա����ʹ��
*********************************************************************/
ErrorStatus RadioProtocol_UpdateUploadCmdList(u16 Cmd, u16 SerialNum, u8 channel)
{
    u16 i;
    
    for(i=0; i<UPLOAD_CMD_NUM; i++)
    {
        if(UploadCmdList[i][0] == Cmd)
        {
            UploadCmdList[i][1] = SerialNum;
            UploadCmdList[i][2] = channel;
            return SUCCESS;
        }
    }
    return ERROR;
}
/*********************************************************************
//��������  :unTransMean
//����      :ʵ�ַ�ת�幦��
//      :0x7e----->0x7d���һ��0x02;0x7d----->0x7d���һ��0x01
//����      :����ǰ�ֽ���������ǰ�ֽ�������
//���      :�������ֽ���
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :ת��������ֽ�������
//��ע      :Э�������������ô˺������Ƚ��з�ת�壬Ȼ���ٽ�����
*********************************************************************/
u16 unTransMean(u8 *pBuffer, u16 BufferLen)
{
    u16 i;
    u16 NewLen;
    
    i = 0;
    NewLen = BufferLen;
    while(i < BufferLen)
    {
        if(0x7d == *pBuffer)
        {
            if(0x02 == *(pBuffer+1))
            {
                *pBuffer = 0x7e;
                NewLen--;
                i++;
                memmove(pBuffer+1,pBuffer+2,BufferLen-i);
            }
            else if(0x01 == *(pBuffer+1))
            {
                *pBuffer = 0x7d;
                NewLen--;
                i++;
                memmove(pBuffer+1,pBuffer+2,BufferLen-i);
            }
        }
        i++;
        pBuffer++;
    }
    return NewLen;
}
/*********************************************************************
//��������  :RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD *head)
//����      :����Ϣͷ����ֽ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
u8 RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD *head)
{
    u8  length;
    u16 temp;
	  u8 *p = NULL;
	  u8 i;
	  

	  p = pBuffer;
    //�����ֽ���ת���������ֽ���
    //��ϢID
	  temp = head->MessageID;
	  *p++ = (temp&0xff00) >> 8;
	  *p++ = temp&0xff;

    //��Ϣ������
	  temp = head->Attribute;
	  *p++ = (temp&0xff00) >> 8;
	  *p++ = temp&0xff;
	
    //�ն��ֻ���
	  for(i=0; i<6; i++)
	  {
		    *p++ = TerminalPhoneNumber[i];
		}
    //��Ϣ��ˮ��
		temp = head->SerialNum;
	  *p++ = (temp&0xff00) >> 8;
	  *p++ = temp&0xff;
	
    //�ж����޷�װ��
    if(0x2000 == (head->Attribute & 0x2000))//�з�װ��
    {
			  temp = head->TotalPackage;
	      *p++ = (temp&0xff00) >> 8;
	      *p++ = temp&0xff;
			
			  temp = head->CurrentPackage;
	      *p++ = (temp&0xff00) >> 8;
	      *p++ = temp&0xff;
			
        length = 16;
    }
    else//û�з�װ��
    {
        length = 12;
    }
		
    return length;
}
/*********************************************************************
//��������  :TransMean
//����      :ʵ�ִ��ʱ��ת�崦����
//����      :ת��ǰ�ֽ�����ת��ǰ�ֽ�������
//���      :ת����ֽ���
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :ת����ֽ�������
//��ע      :0x7e----->0x7d���һ��0x02;0x7d----->0x7d���һ��0x01
*********************************************************************/
u16 TransMean(u8 *pBuffer, u16 BufferLen)
{
    u16 i;
    u16 NewLen;
    //u8    temp;
    i = 0;
    NewLen = BufferLen;
    while(i < BufferLen)
    {
        if(0x7e == *pBuffer)
        {
            *pBuffer = 0x7d;
            memmove(pBuffer+2,pBuffer+1,BufferLen-i);
            pBuffer++;
            *pBuffer = 0x02;
            NewLen++;
            
        }
        else if(0x7d == *pBuffer)
        {
            memmove(pBuffer+2,pBuffer+1,BufferLen-i);
            pBuffer++;
            *pBuffer = 0x01;
            NewLen++;
        }
        i++;
        pBuffer++;
    }
    return NewLen;
}
/*********************************************************************
//��������  :RadioProtocol_IsBigEndian(void)
//����      :���������ֽ����Ƿ�Ϊ���ϵͳ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :1Ϊ��ˣ�0ΪС��
//��ע      :
*********************************************************************/
u8 RadioProtocol_IsBigEndian(void)
{
    u16 value = 1;
    if(1 == (*(u8 *)&value))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/*********************************************************************
//��������  :RadioProtocol_ntohs(u16 value)
//����      :�����ֽ���ת�����ֽ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
u16 RadioProtocol_ntohs(u16 value)
{
    u8  flag;
    u8  byte;
    u8  *p = NULL;
    u16 ConvertValue;

    flag = BigLittleFlag;
    p = (u8 *)&value;

    if(1 == flag)
    {
        ConvertValue = value;
    }
    else //�����ֽ���
    {
        ConvertValue = 0;   
        byte = *p++;
        ConvertValue |= byte << 8;
        byte = *p++;
        ConvertValue |= byte;   
    }
    return ConvertValue;
    
}
/*********************************************************************
//��������  :RadioProtocol_ntoh(u32 value)
//����      :�����ֽ���ת�����ֽ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
u32 RadioProtocol_ntoh(u32 value)
{
    u8  flag;
    u8  byte;
    u32 ConvertValue;

    flag = BigLittleFlag;

    if(1 == flag)
    {
        ConvertValue = value;
    }
    else //�����ֽ���
    {
        ConvertValue = 0;
        //��ȡ���ֽ�
        byte = (value & 0xff000000) >> 24;
        ConvertValue |= byte;
        //��ȡ�θ��ֽ�
        byte = (value & 0xff0000) >> 16;
        ConvertValue |= (byte << 8);
        //��ȡ���ֽ�
        byte = (value & 0xff00) >> 8;
        ConvertValue |= (byte << 16);
        //��ȡ���ֽ�
        byte = value;
        ConvertValue |= (byte << 24);
        
    }
    return ConvertValue;
    
}
/*********************************************************************
//��������  :RadioProtocol_htons(u16 value)
//����      :�����ֽ���ת�����ֽ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
u16 RadioProtocol_htons(u16 value)
{
    u8  flag;
    u8  byte;
    u8  *p = NULL;
    u16 ConvertValue;

    flag = BigLittleFlag;
    p = (u8 *)&value;

    if(1 == flag)
    {
        ConvertValue = value;
    }
    else //�����ֽ���
    {
        ConvertValue = 0;   
        byte = *p++;
        ConvertValue |= byte << 8;
        byte = *p++;
        ConvertValue |= byte;   
    }
    return ConvertValue;
    
}
/*********************************************************************
//��������  :RadioProtocol_hton(u32 value)
//����      :�����ֽ���ת�����ֽ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
u32 RadioProtocol_hton(u32 value)
{
    u8  flag;
    u8  byte;
    u8  *p;
    u32 ConvertValue;

    flag = BigLittleFlag;
    p = (u8 *)&value;

    if(1 == flag)
    {
        ConvertValue = value;
    }
    else //�����ֽ���
    {
        ConvertValue = 0;   
        byte = *p++;
        ConvertValue |= byte << 24;
        byte = *p++;
        ConvertValue |= byte << 16;
        byte = *p++;
        ConvertValue |= byte << 8;
        byte = *p++;
        ConvertValue |= byte;   
    }
    return ConvertValue;
    
}
/*********************************************************************
//��������  :NullOperate(u8 *pBuffer, u16 BufferLen)
//����      :�ղ���,�����κ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :�ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :
*********************************************************************/
ProtocolACK NullOperate(u8 channel, u8 *pBuffer, u16 BufferLen)
{
    return ACK_OK;
}

u16 RadioProtocol_Packet_ICCard_AuthInfo( u16 MessageID, u8 ExdeviceCode , u8 *pBuffer, u16 BufferLen, u8 *pOutBuf )
{
u8 temp2[20];
u8 temp;
u8 VerifyCode;
u16 i;
u16 HeadLen;
u16 length;
    
MESSAGE_HEAD    head;
u8   *p = NULL;
u8   * SrcData;
  u16   SrcLen;
        
  static u16 MessageSerialID;
  
  u8  RadioProtocolMiddleBuffer[100];//��Ҫ���ʹ�������ʱ��RadioProtocolMiddleBuffer����
  
  if( BufferLen > sizeof( RadioProtocolMiddleBuffer)-1 ){
    return 0;
  }

  RadioProtocolMiddleBuffer[0]= ExdeviceCode;
    memcpy( &RadioProtocolMiddleBuffer[1],pBuffer,BufferLen);
  
    SrcLen = BufferLen+1;
  
  
    //��ϢID
    head.MessageID = MessageID;
  
    //��Ϣ����
    head.Attribute = (SrcLen & 0x3ff);
           
  SrcData = (u8 *)RadioProtocolMiddleBuffer;

  //=================================================================

    //�ն��ֻ���
  
  memcpy(head.PhoneNum,TerminalPhoneNumber,6); 
  
    //��Ϣ��ˮ��

    head.SerialNum = MessageSerialID;
  MessageSerialID++;
  
    //MESSAGE_HEAD�ṹ����ֽ���
  
    HeadLen = RadioProtocol_HeadtoBytes(temp2,&head);
  
    //����У����
  
    VerifyCode = 0;
    p = temp2;
    for(i=0; i<HeadLen; i++)
    {
        temp = *p;
        VerifyCode = VerifyCode ^ temp;
        p++;
    }
    p = SrcData;
    for(i=0; i<SrcLen; i++)
    {
        temp = *p;
        VerifyCode = VerifyCode ^ temp;
        p++;
    }
  
    //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
  
    memcpy( RadioProtocolTxBuffer+1,temp2,HeadLen);
  
    if(SrcLen > 0)
    {
        memcpy(RadioProtocolTxBuffer+1+HeadLen,SrcData,SrcLen);
    }
        
    RadioProtocolTxBuffer[1+HeadLen+SrcLen] = VerifyCode;
  
    //ת��

    length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+SrcLen);
    length++;
  
    //����һͷһβ�ı�־λ
    RadioProtocolTxBuffer[0] = 0x7e;
    RadioProtocolTxBuffer[length] = 0x7e;
    length++;

  memcpy( pOutBuf, RadioProtocolTxBuffer, length );
  
  return length;
        
}
/*********************************************************************
//��������  :ProtocolACK RadioProtocol_UploadFromBreak_Request(u8 *pBuffer, u16 BufferLen)
//����      :�����ְ�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :�ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      :0x8003
*********************************************************************/
//*********************************************************************************/
static ProtocolACK RadioProtocol_UploadFromBreak_Request(u8 channel, u8 *pBuffer, u16 BufferLen)
{
     VDRGprs_PacketResendRequest(channel, pBuffer,BufferLen);
     return RadioProtocol_TerminalGeneralAck(channel,0);
}
/*********************************************************************
//��������  :RadioProtocol_DownloadUpdatePacket(u8 *pBuffer, u16 BufferLen)
//����      :�·��ն�������
//��ע      :ָ��ID��0x8108
*********************************************************************/
static ProtocolACK RadioProtocol_DownloadUpdatePacket(u8 channel, u8 *pBuffer, u16 BufferLen)
{
	  PacketUpdate_HandleCmd8108H(pBuffer,BufferLen);
	  return ACK_OK;
    //return RadioProtocol_TerminalGeneralAck( channel,ACK_OK ); dxl,2016.6.1����Ҫÿ��Ӧ��ֻ�����1��Ӧ��
}
/*********************************************************************
//��������  :RadioProtocol_UpdataResultNote
//��������  :�ն��������֪ͨ
//��ע      :ָ��ID��0x0108
*********************************************************************/
ProtocolACK RadioProtocol_UpdataResultNote(u8 channel,u8 UpdataType,u8 AckResult)
{
  u8 Buffer[5];
  u8 length;
  u8 SmsPhone[20];
  MESSAGE_HEAD  head;
  
  
  if( AckResult > 2 )
  {
    return ACK_ERROR;    
  }
  
  
  //��ϢID
  head.MessageID = 0x0108;

  //��Ϣ����
  length = 2;
  head.Attribute = length;
    
  //��Ϣ����
  Buffer[0] = UpdataType;//��������
  Buffer[1] = AckResult; //�������

  if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
  {
      return ACK_OK;
  }
  else
  {
      return ACK_ERROR;
  }
         
}

static ProtocolACK RadioProtocol_AlarmConfirmManual(u8 channel, u8 *pBuffer, u16 BufferLen)
{

  u8 *p =NULL;
  u8 AckResult;
  static u16 AlarmSerialNum;
  static u32 AlarmType;
  u8 flag = 0;  
        
  if( BufferLen != 6 )
  {     
      AckResult = 1;              
  }
  else
  {


      p = pBuffer;

      /*** ��Ҫ�˹�ȷ�ϵı�����ˮ��, 0��ʾ����������Ϣ  ***/
      AlarmSerialNum = 0;
      AlarmSerialNum = *p++;
      AlarmSerialNum <<=8;
      AlarmSerialNum += *p++;
        
      /*** �˹�ȷ�ϱ�������  ***/
      
       AlarmType = 0;
       AlarmType += *p++;
        
       AlarmType <<= 8;
       AlarmType += *p++;
         
       AlarmType <<= 8;    
       AlarmType += *p++;
        
       AlarmType <<= 8;
       AlarmType += *p++;
       
       
       if( AlarmType & ( 1<< 0 ) ) {
        Io_WriteAlarmBit( ALARM_BIT_EMERGENCY , RESET );    //0 1����������,�յ�Ӧ������
    flag = 0;
    FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, &flag, FRAM_EMERGENCY_FLAG_LEN);//�������洢��־
       
       }
       
      if( AlarmType & ( 1<< 3 ) ) {
          Io_WriteAlarmBit( ALARM_BIT_DANGER_PRE_ALARM , RESET );        //3    1��Σ��Ԥ��,�յ�Ӧ�������
       
       }
       
       
      if( AlarmType & ( 1<< 20 ) ) {
          Io_WriteAlarmBit( ALARM_BIT_IN_OUT_AREA , RESET );        //20    1����������,�յ�Ӧ�������
       
       }  
        
      if( AlarmType & ( 1<< 21 ) ) {
          Io_WriteAlarmBit( ALARM_BIT_IN_OUT_ROAD , RESET );        //21    1������·��,�յ�Ӧ������
       
       } 
        
        
      if( AlarmType & ( 1<< 22 ) ) {
          Io_WriteAlarmBit( ALARM_BIT_LOAD_OVER_DRIVE , RESET );            //22    1��·����ʻʱ�䲻��/����,�յ�Ӧ������
        
       }
        
        
      if( AlarmType & ( 1<< 27 ) ) {
        if(1 == Io_ReadAlarmBit(ALARM_BIT_LAWLESS_ACC))
        {
              LawlessAccAlarmEnableFlag = 0;
          Io_WriteAlarmBit( ALARM_BIT_LAWLESS_ACC , RESET );        //27    1�������Ƿ����,�յ�Ӧ�������
              
        }
        
       }    
        
        
      if( AlarmType & ( 1<< 28 ) ) {
        Io_WriteAlarmBit( ALARM_BIT_LAWLESS_MOVEMENT , RESET );         //28    1�������Ƿ�λ��,�յ�Ӧ�������
       
       }
       
       
       AckResult = 0;       

  }

  return RadioProtocol_TerminalGeneralAck(channel,AckResult);
}



static ProtocolACK RadioProtocol_TerminalAttribute(u8 channel, u8 *pBuffer, u16 BufferLen)
{
  u8    *p;
    u8  *q;
    u8  Buffer[150];
    u8  Buffer2[30];
    u8  Buffer3[100];
    u8  PramLen;
    u8  SmsPhone[20];
    u16 length;
    u8  i,j;
    u8  BcdHigh;
    u8  BcdLow;
    u8  value;
        
        
    MESSAGE_HEAD    head;
    u32 ID;
    
    p = Buffer;
    length = 0;

    //��ȡ�ն����� Bytes:2 
    if(E2_TERMINAL_TYPE_ID_LEN == EepromPram_ReadPram( E2_TERMINAL_TYPE_ID , Buffer2))
    {
    
    }
    else
    {
        //Ĭ��Ϊ�����ڿ��ˡ�Σ��Ʒ�����ˣ����ͣ�һ���
        Buffer2[0] = 0;
        Buffer2[1] = 0x06;
    }
    memcpy(p,Buffer2,2);
    p += 2;
    length += 2;

    //������ID Bytes:5
    if( 0 == EepromPram_ReadPram( E2_MANUFACTURE_ID , Buffer2 ))
    {
        for( i = 0 ; i < 5; i++ )
        {
                Buffer2[i] = ProductID[i] ;
            }
    }
     memcpy(p,Buffer2,5);
    p += 5;
    length += 5;
        
        
    //�ն��ͺ� Bytes:20
    for(i=0; i<20; i++)
    {
        Buffer2[i] = 0;
    }
#ifdef HUOYUN_DEBUG_OPEN
        memcpy(Buffer2,"EGS701GB",8);//���û�����ù�Ψһ�Ա�ţ���Ʒ�ͺ�Ĭ��ΪEGS701
#else
    if(E2_CAR_ONLY_NUM_ID_LEN == EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID, Buffer3))
    {
        for(i=0; i<16; i++)
        {
            if(Buffer[7+i] < 128)//�ж��Ƿ�ΪASCII��,ǰ��7���ֽ�Ϊ3C���
            {
                Buffer2[i] = Buffer3[7+i];
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        memcpy(Buffer2,TerminalTypeID,6);//���û�����ù�Ψһ�Ա�ţ���Ʒ�ͺ�Ĭ��ΪEGS701
    }
#endif
    memcpy(p,Buffer2,20);
    p += 20;
    length += 20;
    //�ն�ID Bytes:7
    PramLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer2);
    if(7 == PramLen)
    {
        //�����֮ǰhex��(2011.12.16֮ǰ10λID��)����BCD��(2011.12.16֮��13λID��)
        if((0 == Buffer2[0])&&(0 == Buffer2[1]))//��ӦV1.0��Ӳ���汾
        {
            ID = 0;
            ID |= Buffer2[3] << 24;
            ID |= Buffer2[4] << 16;
            ID |= Buffer2[5] << 8;
            ID |= Buffer2[6];
            Buffer3[0] = ID%10000000/1000000+'0';
            Buffer3[1] = ID%1000000/100000+'0';
            Buffer3[2] = ID%100000/10000+'0';
            Buffer3[3] = ID%10000/1000+'0';
            Buffer3[4] = ID%1000/100+'0';
            Buffer3[5] = ID%100/10+'0';
            Buffer3[6] = ID%10+'0';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
        }
        else
        {
        //ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
        q = Buffer2;
        q += 3;
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[0] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[1] = BcdHigh + '0';
        Buffer3[2] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[3] = BcdHigh + '0';
        Buffer3[4] = BcdLow + '0';
        value = *q++;
        BcdHigh = (value & 0xF0)>>4;
        BcdLow = value & 0x0F;
        Buffer3[5] = BcdHigh + '0';
        Buffer3[6] = BcdLow + '0';
        memcpy(p,Buffer3,7);
        p += 7;
        length += 7;
        }
    }
    else
    {
        //��ȡ���ݵ��ն�ID��
        PramLen = EepromPram_ReadPram(E2_DEVICE_BACKUP_ID, Buffer2);
        if(7 == PramLen)
        {
            //�����֮ǰhex��(2011.12.16֮ǰ10λID��)����BCD��(2011.12.16֮��13λID��)
            if((0 == Buffer2[0])&&(0 == Buffer2[1]))//��ӦV1.0��Ӳ���汾
            {
            ID = 0;
            ID |= Buffer2[3] << 24;
            ID |= Buffer2[4] << 16;
            ID |= Buffer2[5] << 8;
            ID |= Buffer2[6];
            Buffer3[0] = ID%10000000/1000000+'0';
            Buffer3[1] = ID%1000000/100000+'0';
            Buffer3[2] = ID%100000/10000+'0';
            Buffer3[3] = ID%10000/1000+'0';
            Buffer3[4] = ID%1000/100+'0';
            Buffer3[5] = ID%100/10+'0';
            Buffer3[6] = ID%10+'0';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
            }
            else
            {
            //ȡ�ն�ID����7λ��Ϊ��¼ע����ն�ID
            q = Buffer2;
            q += 3;
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[0] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[1] = BcdHigh + '0';
            Buffer3[2] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[3] = BcdHigh + '0';
            Buffer3[4] = BcdLow + '0';
            value = *q++;
            BcdHigh = (value & 0xF0)>>4;
            BcdLow = value & 0x0F;
            Buffer3[5] = BcdHigh + '0';
            Buffer3[6] = BcdLow + '0';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
            }
        }
        else
        {
            //�̶�ID
            Buffer3[0] = '2';
            Buffer3[1] = '0';
            Buffer3[2] = '1';
            Buffer3[3] = '2';
            Buffer3[4] = '0';
            Buffer3[5] = '0';
            Buffer3[6] = '1';
            memcpy(p,Buffer3,7);
            p += 7;
            length += 7;
        }
    }
       //�ն�SIM��ICCID BCD[10]  Bytes:10
        MOD_GetIccidBcdValue(&Buffer2[0] );
        memcpy(p,Buffer2,10 );
    p += 10 ;
    length += 10 ;
        
        //�ն�Ӳ���汾���� BYTE  Bytes:1
        j = strlen((char const *)TerminalVersionHW);
        *p++ = j;
        length++;
    //�ն�Ӳ���汾
        for( i = 0 ; i < j; i++ )
        {
          Buffer2[i] = TerminalVersionHW[i] ; 
        }
        memcpy(p,Buffer2,j);//HUOYUN_DEBUG_OPEN����ƽ̨����ǰ����û�У�Ϊ��������
        p += j;
        length += j;

				if(1 == BBGNTestFlag)//dxl,2016.6.2
				{
					  PramLen = EepromPram_ReadPram(E2_FIRMWARE_VERSION_ID, Buffer2);
				    *p++ = PramLen;
            length++;
					  memcpy(p,Buffer2,PramLen);
            p += PramLen;
            length += PramLen;
				}
				else
				{
            //�ն˹̼��汾���� BYTE  Bytes:1
            j = strlen((char const *)FirmwareVersion);
            *p++ = j;
            length++;
            //�ն˹̼��汾�� STRING
            for(i=0; i<j; i++)
            {
               Buffer2[i] = FirmwareVersion[i];
            }
            memcpy(p,Buffer2,j);
            p += j ;
            length += j;
			  }
        
        //GNSSģ������
        if(E2_GPS_ATTRIBUTE_ID_LEN == EepromPram_ReadPram( E2_GPS_ATTRIBUTE_ID , Buffer2  ))
    {
        *p = Buffer2[0];
        p++;
            length++;  
    }
    else
    {
        *p = 0x03;//û�����ù��Ļ���֧��GPS+����
        p++;
            length++;  
    }
    
         
         //ͨ��ģ������
    #ifdef EYE_MODEM
    *p = Modem_Api_Type();//CDMAͨѶ
        //*p = Buffer2[0];
        //p++;
            //length++;  
    //}
    #else
    //{
#if (MOD_TYPE == CDMA_MODULE_MC8331)
    *p = 0x02;//CDMAͨѶ
#endif
    
#if (MOD_TYPE == GPRS_MODULE_M10)
    *p = 0x01;//GPRSͨѶ
#endif
    #endif
    p++;
        length++;  
    
    //��ϢID
    head.MessageID = 0x0107;
    head.Attribute = (length & 0x3ff);
  
    if(0 != RadioProtocol_SendCmd(channel,&head, Buffer, length, SmsPhone))
    {
        return ACK_OK;
    } 
    else 
    {
        return ACK_ERROR;

    } 

}



/*********************************************************************
//��������  :RadioProtocol_PostionInformation_BulkUpTrans(u8 *pBuffer, u16 length)
//����      :��λ���������ϴ�
//����      :pBuffer:ָ�����ݣ�λ����Ϣ��ָ��;BufferLen:���ݵĳ���;
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      : �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      : ָ��ID��0x0704
//�μ�          : ҳ�� P45  <<��·���䳵�����Ƕ�λϵͳ�������ݳ����ն�ͨѶЭ�鼼���淶2013-01��>> 
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformation_BulkUpTrans(u8 channel,u8 *pBuffer, u16 BufferLen)
{
    MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    u16 length;

    //�жϳ���
    if((BufferLen >= 1024)||(0 != MultiMediaSendFlag))
    {
        return ACK_ERROR;
    }
    
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0704;
    //��Ϣ����
    head.Attribute = (BufferLen & 0x3ff);
        
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}

/*********************************************************************
//��������  :RadioProtocol_CAN_UpTrans(u8 *pBuffer, u16 length)
//����      :CAN�����ϴ�����
//����      :pBuffer:ָ�����ݣ�λ����Ϣ��ָ��;BufferLen:���ݵĳ���;
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      : �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
//��ע      : ָ��ID��0x0705
*********************************************************************/
ProtocolACK  RadioProtocol_CAN_UpTrans(u8 channel,u8 *pBuffer, u16 BufferLen)
{
    MESSAGE_HEAD    head;
    u8  SmsPhone[20];
    u16 length;

    //�жϳ���
    if((BufferLen >= 1024)||(0 != MultiMediaSendFlag))
    {
        return ACK_ERROR;
    }
    
    length = BufferLen;
    //��ϢID
    head.MessageID = 0x0705;
    //��Ϣ����
    head.Attribute = (BufferLen & 0x3ff);
        
    if(0 != RadioProtocol_SendCmd(channel,&head, pBuffer, length, SmsPhone))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}


  

u16 IC_Card_Radio_Heartbeat( u8 *pOutBuf )
{
  
    u8 temp2[20];
    u8 temp;
    u8 VerifyCode;
    u16 i;
    u16 HeadLen;
    u16 length;
    
    MESSAGE_HEAD    head;
  u8   *p = NULL;
 
        
  u8 PhoneTemp[6];
  static u16 MessageSerialID;

    //��ϢID
    head.MessageID = 0x0002;
    //��Ϣ����
    head.Attribute = (0 & 0x3ff);
           
 
//=================================================================
        
    //�ն��ֻ���
#if 1
        
  PhoneTemp[0] = 0x01;
  PhoneTemp[1] = 0x38;
  PhoneTemp[2] = 0x00;
  PhoneTemp[3] = 0x13;
  PhoneTemp[4] = 0x80;
  PhoneTemp[5] = 0x00;
  memcpy(head.PhoneNum,PhoneTemp,6);
  
#else
        
  memcpy(head.PhoneNum,TerminalPhoneNumber,6); 
        
#endif

    //��Ϣ��ˮ��
    
    head.SerialNum = MessageSerialID;
  MessageSerialID++;
        
    //MESSAGE_HEAD�ṹ����ֽ���
    HeadLen = RadioProtocol_HeadtoBytes(temp2,&head);
  
    //����У����
    VerifyCode = 0;
    p = temp2;
    for(i=0; i<HeadLen; i++)
    {
        temp = *p;
        VerifyCode = VerifyCode ^ temp;
        p++;
    }
    
    //����Ϣͷ����Ϣ�壬У���뿽�����м仺��
    memcpy(RadioProtocolTxBuffer+1,temp2,HeadLen);
    
    RadioProtocolTxBuffer[1+HeadLen] = VerifyCode;
    //ת��
#if 1
    length = TransMean(RadioProtocolTxBuffer+1,1+HeadLen+0);
#else        
  length = 1+HeadLen+SrcLen;
#endif 
    length++;
    //����һͷһβ�ı�־λ
    RadioProtocolTxBuffer[0] = 0x7e;
    RadioProtocolTxBuffer[length] = 0x7e;
    length++;
   //CmdSendErrorCount = 0;
    //����
//  SendLen = Communication_SndMsg2Svr(channel, RadioProtocolTxBuffer, length, SmsPhone);
        
  memcpy( pOutBuf, RadioProtocolTxBuffer, length );
  return length;
}



