/*******************************************************************************
 * File Name:			NaviLcd.c 
 * Function Describe:	
 * Relate Module:		������
 * Writer:				Joneming
 * Date:				2012-05-18
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
#include "VDR_Data.h"

/*******************************************************************************/
#ifdef USE_NAVILCD
//1161789980

//#define DEBUG_TEST_NAVILCD 

extern u8 RadioShareBuffer[];
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
/*******************************************************************************/
/////////////////////////////////

/***************************************************************************************************
*  ��ʼλ	������	�豸����	���̱�ʶ	������	������	У����	 ����λ

*  2 byte	2 byte	1 byte	     1 byte	    2 byte	 n byte	 1 byte	  2byte
******************************************************************************
******************************************************************************
*��1��	��������ȶ���ΪЭ�����д��豸���͵��������ⲿ�����ݵ��ֽ�����
*       ��1�ֽ�Ϊ��������ȵĸ��ֽڣ���2�ֽ�Ϊ��������ȵĵ��ֽڣ�����
*       ���ĳ���Ϊ=���ֽ�*256+���ֽ�
*  3�� У���룺У���붨��Ϊ��Э�����дӰ����ȵ��������ⲿ�����ݵ��������
******************************************************************************
******************************************************************************

******************************************************************************************************/

/////////////////////////////////
/////////////�豸����ָ��//////////////////////////////////////////
#define NAVILCD_CMD_QUERY                       0x0000	//��ѯ״̬
#define NAVILCD_CMD_RESET                       0x0001	//�豸��λָ��
#define NAVILCD_CMD_SET_BAUDRATE                0x0002	//LCDͨѶ����������
#define NAVILCD_CMD_DISPLAY                     0x0003	//LCD���Ŀ���/�ر�
#define NAVILCD_CMD_CALI_TIME                   0x0004	//Уʱ
#define NAVILCD_CMD_SET_LCD_PARAM               0x0005	//LCD�豸��������ָ��
#define NAVILCD_CMD_HEART_BEAT                  0x0006	//��������
#define NAVILCD_CMD_SYSTEM_UPDATE               0x00FF	//�̼�����ָ��
/////////////////����/��������/////////////////////////////////////////
#define NAVILCD_CMD_TERMINAL_CONTROL            0x0101	//�����ն�����
#define NAVILCD_CMD_TERMINAL_SET_PARAM          0x0102	//���ò�����������
#define NAVILCD_CMD_TERMINAL_QUERY_PARAM        0x0103	//������ѯ����
#define NAVILCD_CMD_QUERY_STATUS_ALARM          0x0104	//��ѯ״̬������Ϣ����
#define NAVILCD_CMD_QUERY_WORK_STATUS           0x0105	//��ѯ����״̬����
#define NAVILCD_CMD_MODIFY_PASSWORD             0x0106	//�޸���������
#define NAVILCD_CMD_REGISTER_OUT_CITY           0x0107	//���ǵǼ�
#define NAVILCD_CMD_EXPORT_U_DISK               0x0108	//U�����ݵ���
#define NAVILCD_CMD_CARLOAD_WEIGHT              0x0109	//����ָ��
#define NAVILCD_CMD_SET_PJA_VALID               0x0110	//������Ч
#define NAVILCD_CMD_PJA_RESULT                  0x0111	//���۽��
///////////////�绰/////////////////////////
#define NAVILCD_CMD_DIAL_PHONE                  0x0201	//��������
#define NAVILCD_CMD_NEW_CALL_PHONE              0x0202	//����������
#define NAVILCD_CMD_LCD_KEY_VALUE               0x0203	//LCD���Ͱ���ֵ���ն�����
#define NAVILCD_CMD_TERMINAL_KEY_VALUE          0x0204	//�ն˷��Ͱ���ֵ��LCD����
#define NAVILCD_CMD_QUERY_PHONE_BOOK            0x0205	//��ѯ�ն˵绰������
#define NAVILCD_CMD_ADJUST_VOLUME               0x0206	//��������
//////////////////��Ϣ����///////////////////////////////////
#define NAVILCD_CMD_TEXT_DISPLAY                0x0301	//�ı���Ϣ�·�����
#define NAVILCD_CMD_QUESTION_DISPLAY            0x0302	//�����·�����
#define NAVILCD_CMD_QUESTION_RESPONSE           0x0303	//�������ʴ�����
#define NAVILCD_CMD_QUERY_FIX_TEXT              0x0304	//��ѯ�̶��ı���Ϣ����
#define NAVILCD_CMD_SEND_FIX_TEXT               0x0305	//���͹̶��ı���Ϣ����
#define NAVILCD_CMD_SEND_TICKLE_INFO            0x0306	//���ͷ�����Ϣ����
#define NAVILCD_CMD_INFO_SERVICE                0x0307	//��Ϣ��������
/////////////////���ٷ���/////////////////////////////////////
#define NAVILCD_CMD_CALL_TAXI_NEW               0x0401	//�µ�����Ϣ����
#define NAVILCD_CMD_CALL_TAXI_VIE               0x0402	//��������
#define NAVILCD_CMD_CALL_TAXI_DETAIL            0x0403	//������ϸ��Ϣ����
#define NAVILCD_CMD_CALL_TAXI_FINISH            0x0404	//������ɻ㱨����
#define NAVILCD_CMD_CALL_TAXI_CANCEL_TERMINAL   0x0405	//�ն�����ȡ����������
#define NAVILCD_CMD_CALL_TAXI_CANCEL_PLATFORM   0x0406	//ƽ̨ȡ����������
//////////////////��ʻԱ��Ϣ////////////////////////////////////
#define NAVILCD_CMD_DRIVER_SIGN                 0x0501	//��ʻԱǩ��/ǩ������
#define NAVILCD_CMD_DRIVER_ADD                  0x0502	//����/�޸ļ�ʻԱ��Ϣ����
#define NAVILCD_CMD_DRIVER_RECORD               0x0503	//��ʻ��¼��ѯ����
#define NAVILCD_CMD_DRIVER_REPORT               0x0504	//��ʻԱ�й�ҵ���ϱ�����
#define NAVILCD_CMD_DRIVER_QUERY_ALL            0x0505	//��ѯ���м�ʻԱ��Ϣ����
#define NAVILCD_CMD_DRIVER_ADD_RESULT           0x0506	//����/�޸ļ�ʻԱ��Ϣ�����Ӧ����
#define NAVILCD_CMD_DRIVER_MODIFY               0x0507	//�޸ĵ�ǰ��ʻԱ����/��������
#define NAVILCD_CMD_DRIVER_DEL_ALL              0x0508	//ɾ�����м�ʻԱ����
#define NAVILCD_CMD_DRIVER_BRUSH_CARD           0x0509	//��ʾ��ˢ��ʻԱ������
#define NAVILCD_CMD_WRITE_IC_CARD               0x050A	//д������
#define NAVILCD_CMD_READ_IC_CARD                0x050B	//��������
#define NAVILCD_CMD_CURRENT_DRIVER_INFO         0x050C	//��ǰ��ʻԱ�����Ϣ����
///////////////////��Ϣ͸��///////////////////////////////////
#define NAVILCD_CMD_PLATFORM_TRANS_DOWN         0x0601	//ƽ̨����͸����LCD����
#define NAVILCD_CMD_LCD_TRANS_UP                0x0602	//LCD����͸����ƽ̨����
////////////////////////////////
#define NAVILCD_CMD_TERMINAL_TRANS_TO_ICCARD    0x0603	//�ն�����͸����ˢ��ģ��
#define NAVILCD_CMD_ICCARD_TRANS_TO_TERMINAL    0x0604	//ˢ��ģ������͸�����ն�����
///////////////////////////////////////////////////////
#define NAVILCD_CMD_TERMINAL_TRANS_TO_CTRL_UNIT 0x0605	//�ն�͸�������ư�ָ��
#define NAVILCD_CMD_CTRL_UNIT_TRANS_TO_TERMINAL 0x0606	//�ն�͸�������ư�ָ��
/////////////////////////////////////////////////////
#define NAVILCD_CMD_LCD_TRANS_TO_LOAD_SENSOR    0x0607	//LCD͸�������ش�����ָ��
#define NAVILCD_CMD_LOAD_SENSOR_TRANS_TO_LCD    0x0608	//���ش�������͸��LCDָ��
////////////////////����//////////////////////////////////
////////////////////����//////////////////////////////////
#define NAVILCD_CMD_NAVIGATION                  0x0701	//��������
#define NAVILCD_CMD_ONE_KEY_NAVIGATION          0x0702	//һ����������
//////////////////////////////////////////////////////
/////////��������˳����չ����////////////////////////////////
#define NAVILCD_CMD_SF_EXPRESS_LCD_TRANS_UP         0x0801	//˳����ʾ��͸��ָ��
#define NAVILCD_CMD_SF_EXPRESS_LCD_TRANS_RESPONSE   0x0802	//˳����ʾ��͸��Ӧ��ָ��
#define NAVILCD_CMD_SF_EXPRESS_SET_LCD_PARAM        0x0803	//������ʾ������
#define NAVILCD_CMD_SF_EXPRESS_QUERY_LCD_PARAM      0x0804	//��ѯ��ʾ������
#define NAVILCD_CMD_SF_EXPRESS_BRUSH_CARD_DATA      0x0805	//ˢ˳�Ῠ��������
/////////////////////////////////////////////////////////
#define NAVILCD_CMD_GNSS_CHECK_UP                   0xFE01	//�ͼ�Э��ָ��

#define NAVILCD_USER_DATA_START 8

const unsigned char c_ucNaviLcdTag[2]={0x55,0xaa};    //��ʶ

#define NAVILCD_FACTORY_NO        0x03//0x03   //���̱��
#define EQUIP_TYPE_NAVILCD_NO     0x09         //�豸����(LCD)


typedef enum
{
    NV_RECV_OK,             //������ȷ
    NV_RECV_ERR_START,      //��ͷ��־����
    NV_RECV_ERR_TYPE,       //���Ͳ���
    NV_RECV_ERR_LEN,        //���Ȳ���
    NV_RECV_ERR_END,        //������־����
    NV_RECV_ERR_VERIFY,     //У�鲻��
    NV_RECV_MAX             //
}NAVILCDRECV;

/////////////////////////////////////
typedef struct
{
    unsigned short recvCmd;         //�յ�������
    unsigned short recvDatalen;     //��������������
    unsigned char onlinestaus;      //�Ƿ�����
    unsigned char CheckCnt;         //����ã�������
    unsigned char ProcessStep;      //���� 
    unsigned char RecvAnswerFlag;   //�յ�Ӧ��
    unsigned char AccStatus;        //Acc״̬
    unsigned char newphone;         //
    unsigned char logout;           //
    unsigned short AccOffCnt;           //Acc�صļ�����
    unsigned char LcdPowerOn;           //lcd��Դ����
    unsigned char AccTurnPrompt;        //��ʾ��ʻԱ��־
    unsigned char VoicePrompt;          //������ʾ��־
}
T_NAVILCD_ATTRIB;


static T_NAVILCD_ATTRIB s_stNaviLcdAttrib;

typedef enum
{
    NAVILCD_EMPTY_STATE,          //��
    NAVILCD_QUARY_STATE,          //
    NAVILCD_EQUIP_OPEN_STATE,     //
    NAVILCD_DRIVER_SIGN_OUT,      //
    NAVILCD_SERVICE_REPAIR,       //
    NAVILCD_MAX_STATE             //
}E_NAVILCD_STATE;

typedef enum
{
    TERMINAL_CTRL_EMPTY,
    TERMINAL_CTRL_REGISTER,         //�ն�ע��
    TERMINAL_CTRL_CONFIRM_ID,       //�ն˼�Ȩ
    TERMINAL_CTRL_LOGOUT,           //�ն�ע��
    TERMINAL_CTRL_RESET,            //�ն˸�λ
    TERMINAL_CTRL_FEATURE,          //��������ϵ��У׼
    TERMINAL_CTRL_DEFAULT_SET,      //�ָ�������������
    TERMINAL_CTRL_QUICK_OPEN_ACCOUNT,//���ٿ���
    TERMINAL_CTRL_CAR_LOAD_STATUS,   //�����ػ�״̬
    TERMINAL_CTRL_MAX       //
}E_NAVILCD_TERMINALCTRL;

typedef enum
{
    NAVILCD_KEYVALUE_EMPTY,                     //��
    NAVILCD_KEYVALUE_HOT_LINE  =0x0A,           //�������ߵ绰
    NAVILCD_KEYVALUE_ANSWER_CALL,               //�����绰
    NAVILCD_KEYVALUE_HANG_UP,                   //�Ҷ�
    NAVILCD_KEYVALUE_HF,                        //����
    NAVILCD_KEYVALUE_REDIAL,                    //�ز�
    NAVILCD_KEYVALUE_DEL,                       //ɾ��
    NAVILCD_KEYVALUE_VOL_ADD,                   //����+
    NAVILCD_KEYVALUE_VOL_DEC,                   //����-
    NAVILCD_KEYVALUE_ASTERISK='*',              //*�� 
    NAVILCD_KEYVALUE_POUND='#',                 //#��
    NAVILCD_KEYVALUE_KEY0   =0x30,              //0��
    NAVILCD_KEYVALUE_KEY1,                      //1��
    NAVILCD_KEYVALUE_KEY2,                      //2��
    NAVILCD_KEYVALUE_KEY3,                      //3��
    NAVILCD_KEYVALUE_KEY4,                      //4��
    NAVILCD_KEYVALUE_KEY5,                      //5��
    NAVILCD_KEYVALUE_KEY6,                      //6��
    NAVILCD_KEYVALUE_KEY7,                      //7��
    NAVILCD_KEYVALUE_KEY8,                      //8��
    NAVILCD_KEYVALUE_KEY9,                      //9��
    NAVILCD_KEYVALUE_MAX       //
}E_NAVILCD_KEYVALUE;

typedef enum
{
    FIXTEXT_TYPE_PHRASE,            //�̶�����
    FIXTEXT_TYPE_EVENT,             //�¼�����
    FIXTEXT_TYPE_INFO,              //��Ϣ�㲥
    FIXTEXT_TYPE_MAX                //
}E_NV_FIXTEXTTYPE;

typedef enum
{
    DRIVER_SPEED_RECORD,     //���ټ�¼
    DRIVER_TIRED_RECORD,     //ƣ�ͼ�ʻ��¼
    DRIVER_STOP_RECORD,      //ͣ����ӡ
    DRIVER_INFO_RECORD,      //��ʻԱ��Ϣ
    DRIVER_RECORD_MAX       //
}E_NAVILCD_DRIVER;

typedef enum
{
    TERMINAL_PARAM_EMPTY,               //0��
    TERMINAL_PARAM_MAIN_APN,            //1�������� APN
    TERMINAL_PARAM_RADIO_USER,          //2������������ͨ�Ų����û���
    TERMINAL_PARAM_RADIO_PASSWORD,      //3������������ͨ�Ų�������
    TERMINAL_PARAM_MAIN_IP,             //4����������ַ,IP ������
    TERMINAL_PARAM_BAK_APN,             //5���ݷ����� APN������ͨ�Ų��ŷ��ʵ�
    TERMINAL_PARAM_BAK_RADIO_USER,      //6���ݷ���������ͨ�Ų����û���
    TERMINAL_PARAM_BAK_RADIO_PASSWORD,  //7���ݷ���������ͨ�Ų�������
    TERMINAL_PARAM_BAK_IP,              //8���ݷ�������ַ,IP ������
    TERMINAL_PARAM_TCP,                 //9������ TCP �˿� 
    TERMINAL_PARAM_UDP,                 //10������ UDP �˿�
    TERMINAL_PARAM_SPEED_ALARM,         //11����ٶȣ���λΪ����ÿСʱ��km/h��
    TERMINAL_PARAM_TOTAL_MILE,          //12������̱������1/10km 
    TERMINAL_PARAM_CAR_PLATE,           //13������ͨ�����Ű䷢�Ļ���������
    TERMINAL_PARAM_CAR_COLOR,           //14������ɫ������ JT/T415-2006 �� 5.4.12
    TERMINAL_PARAM_PHONE_NUMBER,        //15�ն��ֻ���
    TERMINAL_PARAM_CAR_VIN,             //16����ʶ����
    TERMINAL_PARAM_CAR_PLATE_TYPE,      //17���Ʒ���
    TERMINAL_PARAM_SPEED_SENSOR,        //18�ٶȴ�����ϵ��
    TERMINAL_PARAM_FEATURE_COEF,        //19��������ϵ��
    TERMINAL_PARAM_SPEED_SELECT,        //20�ٶ�ѡ�񿪹�
    TERMINAL_PARAM_SET_BACKLIGHT,       //21����ѡ�񿪹�
    TERMINAL_PARAM_DOOR_LEVEL,          //22������������
    TERMINAL_PARAM_CDMA_SLEEP,          //23CDMA���߿���
    TERMINAL_PARAM_CAR_HORN_LEVEL,      //24�����ȵ�ƽѡ��
    TERMINAL_PARAM_TERMINAL_ID,         //25�ն�ID(LCDֻ�ɲ�ѯ������д��)
    TERMINAL_PARAM_PASSWORD,            //26����
    TERMINAL_PARAM_WEIGHT,              //27����
    TERMINAL_PARAM_OWNER_PHONE,         //28������ϵ�ֻ���
    TERMINAL_PARAM_CAR_PROVINCE_ID,     //29ʡ��ID
    TERMINAL_PARAM_CAR_CITY_ID,         //30������ID
    TERMINAL_PARAM_MAIN_DOMAIN_NAME,    //31�������������
    TERMINAL_PARAM_BACKUP_DOMAIN_NAME,  //32���ݼ����������
    TERMINAL_PARAM_MANUFACTURE_ID,      //33������ID
    TERMINAL_PARAM_MAX       //
}E_NAVILCD_TERMINALPARAM;


////////////////////////////////////////
typedef enum
{
    NV_GNSS_CHECK_EMPTY,            //��
    NV_GNSS_CHECK_TALK_PHONE,       //�δ�绰
    NV_GNSS_CHECK_ANSWER_PHONE,     //�����绰
    NV_GNSS_CHECK_HANG_UP_PHONE,    //�Ҷϵ绰
    NV_GNSS_CHECK_START_RECORD,     //��ʼ¼��
    NV_GNSS_CHECK_STOP_RECORD,      //ֹͣ¼��    
    NV_GNSS_CHECK_FIX_PHRASE,       //���͹̶����� 
    NV_GNSS_CHECK_RTU_REGISTER,     //�ն�ע��
    NV_GNSS_CHECK_RTU_QUALIFY,      //�ն˼�Ȩ
    NV_GNSS_CHECK_POSITION_INFO,    //λ����Ϣ�㱨
    NV_GNSS_CHECK_EVENT_REPORT,     //�¼�����    
    NV_GNSS_CHECK_INFO_DEMAND,      //��Ϣ�㲥   
    NV_GNSS_CHECK_CANCEL_IOD,       //ȡ����Ϣ�㲥
    NV_GNSS_CHECK_E_WAYBILL,        //�����˵��ϱ�
    NV_GNSS_CHECK_DRIVER_INFO,      //��ʻԱ�����Ϣ
    NV_GNSS_CHECK_MULIT_EVENT,      //�Ϸ���ý���¼�
    NV_GNSS_CHECK_DTU_DATA,         //��������͸��    
    NV_GNSS_CHECK_RESERVE1,         //��������1   
    NV_GNSS_CHECK_RESERVE2,         //��������2
    NV_GNSS_CHECK_RESERVE3,         //��������3
    NV_GNSS_CHECK_MAX       //
}E_NAVILCD_GNSS_CHECK;//�����ͼ�ָ��궨��


enum 
{
    NV_TIMER_CHECK_ACC,					//
    NV_TIMER_RESEND,					// 
    NV_TIMER_SEND_END,                  //
    NV_TIMER_BUSY_TIMEOUT,              //
    NV_TIMER_TASK,						//
    NV_TIMER_TEST,						//
    NV_TIMER_KEY,    
    NV_TIMER_STATUS,					//
    NV_TIMER_DRIVER_ADD,				//
    NV_TIMERS_MAX
}E_NAVILCDTIME;

static LZM_TIMER s_stNVTimer[NV_TIMERS_MAX];

typedef enum
{
    LCD_SF_PROTOCOL_EMPTY,                      //��  
    LCD_SF_PROTOCOL_DRIVE_LOG =0x37,              //�г���־
    LCD_SF_PROTOCOL_BRUSH_CARD_LOGIN =0x39,       //���°�򿨲�
    LCD_SF_PROTOCOL_MAX       //
}E_LCDSFPROTOCOL;


#define COMNAVILCD_MAX_REC_LEN     512 //�������ݵĳ���
#define COMNAVILCD_MAX_SEND_LEN    1024 //�������ݵĳ���

#define COMNAVILCD_MIN_LEN  11
//�ṹ����
typedef struct{	
	unsigned short recvlen;          //���ڽ������ݳ���,���ղ�����������,��ʼ����0	
	unsigned short datalen;         //������ݳ���
	unsigned short disposelen;      //�Ѵ������ݳ���	
	unsigned char sendBusy;         //���ڵķ���״̬ false:����;  true:æ  (����ǰ�ж�,����ǰ��λ,���ݷ���������,��ʼ����0)//3����ʱ�����Զ���
	unsigned char resendCnt;        //�����ط����ʹ���
	unsigned char recvtimeoutCnt;   //���ճ�ʱ����
	unsigned char contrecvtimeoutCnt; //�������ճ�ʱ����
	unsigned char conttryCnt;       //��������
	unsigned char responseFlag;     //�Ƿ���ҪӦ���־	
    unsigned char recvfreeCnt;      //���տ��м�����    
	unsigned char recvBuff[COMNAVILCD_MAX_REC_LEN+1];     //��������
}ST_COMNAVILCD;
////////////////////////////////
static unsigned char s_ucNaviLcdTxBuffer[COMNAVILCD_MAX_SEND_LEN+1];//����
static unsigned char s_ucNaviLcdParseBuffer[COMNAVILCD_MAX_REC_LEN+1];//����֮�󻺳�

////////////////////////////////
//��������
void GetNaviLcdCommRxData(void) ; //��������
void PutNaviLcdCommTxData(void) ;//������������
void NaviLcdCommSendEnd(void);//���ͽ���
unsigned char IsNaviLcdCommBusy(void);//�жϴ����Ƿ�æ
unsigned char MayNaviLcdCommSend(void); //�ж��Ƿ����㷢������
unsigned char NaviLcdProtocolParse(unsigned char *ParseBuffer,unsigned short usDataLenght);
unsigned char Public_GetXorVerify(const unsigned char *Pdata,unsigned short DataLength);
void NaviLcd_DisposeRecvDataResponse(void);
unsigned char NaviLcdCommResendData(unsigned char *Ptr,unsigned short Len);
unsigned char NaviLcd_CheckDailPhone(unsigned char *phone);
void NaviLcd_DisposeHangUpPhoneStatus(void);
void NaviLcd_BrushCardDataDebugTest(void);
void NaviLcd_SetTerminalPassWordToLcd(void);
void NaviLcd_DisposePJAResult(void);
////////////////////////////////
void NaviLcd_SetCurrentStatus(unsigned char Status);

const char * c_apcRegisterInfo[] = 
{
    "ע��ɹ�!",//0
    "�����ѱ�ע��!",//1
    "���ݿ��޸ó���!",//2
    "�ն��ѱ�ע��!",//3
    "���ݿ��޸��ն�!",//4
    "��Ȩ�볤�ȳ���!",//5
    "��Ȩ�ɹ�!",//6
    "��Ȩʧ��!",//7
};


static char *c_apcCarPlakeTypeItems[]=
{
    "���������ƺ�",
    "�ҳ��ƺ�",
    "С�������ƺ�",
    "�����ƺ�",
    ""
};
//��̬��������
static ST_COMNAVILCD s_stNaviLcdComm;  //���ڱ���

//�ṹ����
typedef struct{	
	unsigned char datalen;         //
	unsigned char Keybuff[32];     //
}STNVKEY;

static STNVKEY s_stNVKey;

//////////////////////////////////////////////
enum 
{
    NV_DRIVERLOG_STATUS_EMPTY =0x55,//
    NV_DRIVERLOG_STATUS_LOGOUT,     //
    NV_DRIVERLOG_STATUS_LOGIN,      //
    NV_DRIVERLOG_STATUS_MAX
}DRIVERLOGSTATUS;

enum 
{
    NV_CMD_FLAG_EMPTY,      //��
    NV_CMD_FLAG_BUSINESS,   //ҵ���ϱ� 
    NV_CMD_FLAG_LOGIN,      //ˢ��ǩ��  
    NV_CMD_FLAG_LOGOUT,     //ˢ��ǩ�� 
    NV_CMD_FLAG_ADD,        //���/�޸ļ�ʻԱ    
    NV_CMD_FLAG_MAX
}DRIVERLOGFLAG;

typedef struct
{  
     unsigned char statusFlag;   //״̬��־
     unsigned char recordNumber; //��¼��
     unsigned char cmdFlag;      //ָ���־
     unsigned char recordindex;   //���/�޸ĵ�˳���
     unsigned char resutltindex;  //˳���
     unsigned char result;   //�����־
     unsigned char drivercode[6]; //����
     unsigned char password[6];   //����
     unsigned char loginTime[6];//��¼ʱ��
}STDRIVERLOG;

static STDRIVERLOG s_stDriverLog;

///////////////////////////
const char *c_apcFixPhrase[] = 
{
    "����",
    "����",
    "�¹�",
    "��ê",
    "��ָʾ����վ",
    "ͣ������",
    "��������", 
    "�ѷ���",
    "�ѵ���",
    "��������",
    "ҽ������",
    "��·,��ָʾ",
    "������Σ��",
    "Υ��",
	""
};
#define FIX_PHRASE_ITEMS 14

void NaviLcdSaveDriverLogin(unsigned char recordnumber);
void NaviLcdSaveDriverLogout(void);
void NaviLcdSendCmdQueryStatus(void);
void NaviLcd_DebugToWriteICcard(void);

/*************************************************************OK
** ��������: NaviLcdPowerIoInit
** ��������: ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void NaviLcdPowerIoInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*************************************************************OK
** ��������: NaviLcdPowerOn
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void NaviLcdPowerOn(void)
{
    GPIO_SetBits(GPIOE, GPIO_Pin_11);//
}
/*************************************************************OK
** ��������: NaviLcdPowerOn
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void NaviLcdPowerOff(void)
{
    if(1 == ReadPeripheral2TypeBit(5))//bit5:�����¶ȴ�������ACC OFFʱ���رյ�Դ
    {
    
    }
    else if(1 == ReadPeripheral2TypeBit(15))//bit15:���˵�������ACC OFFʱ��رյ�Դ
    {
            //GPIO_ResetBits(GPIOE, GPIO_Pin_11);//dxl,2016.4.15,
    }
}

/*************************************************************OK
** ��������: NaviLcdPowerOn
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void NaviLcdDelayms(unsigned char ms)
{
    unsigned char i,j;
    for(i=ms; i>0;i--)
    {
        for(j=200; j>0;j--);
    }
}

/************************************************************
** ��������: NaviLcdCommBusyTimeOut
** ��������: 
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdCommBusyTimeOut(void)
{
    s_stNaviLcdComm.sendBusy = 0;
}
/************************************************************
** ��������: NaviLcdCommResponeTimeout
** ��������: û�д�Ӧ���ط�����
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdCommResponeTimeout(void)
{
    s_stNaviLcdComm.recvtimeoutCnt++;
    s_stNaviLcdComm.contrecvtimeoutCnt++;
    if(s_stNaviLcdComm.contrecvtimeoutCnt > 4)
    {
        LZM_PublicKillTimer(&s_stNVTimer[NV_TIMER_STATUS]);
        s_stNaviLcdComm.sendBusy = 0;
        s_stNaviLcdComm.recvtimeoutCnt = 0;
        s_stNaviLcdComm.conttryCnt++;
        s_stNaviLcdAttrib.onlinestaus = 0;
        if(s_stNaviLcdComm.conttryCnt<2)
        {
            #ifndef DEBUG_TEST_NAVILCD
            NaviLcdPowerOff();
            #endif
            LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_STATUS],PUBLICSECS(5),NaviLcdPowerOn); 
            s_stNaviLcdComm.contrecvtimeoutCnt = 0;
            LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TASK],PUBLICSECS(15),NaviLcdSendCmdQueryStatus); 
        }
        else
        {
            Io_WriteAlarmBit(ALARM_BIT_LCD_FAULT,SET);            
            LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TASK],PUBLICSECS(30),NaviLcdSendCmdQueryStatus); 
        }
    }
    else
    if(s_stNaviLcdComm.recvtimeoutCnt < 3)//////////////////
    {
        NaviLcdCommResendData(s_ucNaviLcdTxBuffer,s_stNaviLcdComm.datalen);
    }
    else////��ʱ//////////////////
    {
        s_stNaviLcdComm.sendBusy = 0;
        s_stNaviLcdComm.recvtimeoutCnt = 0;
        LZM_PublicKillTimer(&s_stNVTimer[NV_TIMER_STATUS]);
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TASK],PUBLICSECS(6),NaviLcdSendCmdQueryStatus);
    }
}
/************************************************************
** ��������: NaviLcdCommSendEnd
** ��������: �������
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdCommSendEnd(void)
{  
    if(s_stNaviLcdComm.responseFlag)//��Ҫ�жϽ��ճ�ʱ
    {
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TASK],PUBLICSECS(1.8),NaviLcdCommResponeTimeout);
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_BUSY_TIMEOUT], PUBLICSECS(0.8), NaviLcdCommBusyTimeOut);
    }
    else
    {
        s_stNaviLcdComm.sendBusy = 0;
        s_stNaviLcdComm.recvtimeoutCnt = 0;
    }
}
/************************************************************
** ��������: IsNaviLcdCommBusy
** ��������: ȡ�õ�ǰ����״̬
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
unsigned char IsNaviLcdCommBusy(void)
{
    return COM2_GetSendBusy();
}
/************************************************************
** ��������: NaviLcdCommStartToSendData
** ��������: 
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
unsigned char NaviLcdCommStartSendData(unsigned char *pBuffer,unsigned short datalen)
{     
    unsigned char result;
    result=COM2_WriteBuff(pBuffer,datalen);
    LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_SEND_END],LZM_AT_ONCE,NaviLcdCommSendEnd);    
    return result;
    //USART_ITConfig(USART1, USART_IT_TC, ENABLE); //��������
}

/************************************************************
** ��������: NaviLcdCommBusyForResendData
** ��������: �ط�ʱ�䵽��ִ���ط�����
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdCommBusyForResendData(void)
{
    if(0==NaviLcdCommStartSendData(s_ucNaviLcdTxBuffer,s_stNaviLcdComm.datalen))//��æ,ֱ�ӷ���
    {
        s_stNaviLcdComm.resendCnt = 0;
    }
    ///////////
    if(s_stNaviLcdComm.resendCnt)//�����ط�
    {
        s_stNaviLcdComm.resendCnt--;
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_RESEND], PUBLICSECS(0.15), NaviLcdCommBusyForResendData);  
    }
}
/************************************************************
** ��������: NaviLcdCommStartToSendData
** ��������: 
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdCommClearResponse(void)
{
    s_stNaviLcdComm.responseFlag = 0;
}
/************************************************************
** ��������: NaviLcdCommStartToSendData
** ��������: 
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
unsigned char NaviLcdCommSendData(unsigned char *pBuffer,unsigned short datalen)
{
    if(datalen>COMNAVILCD_MAX_SEND_LEN)return 0xff;
    s_stNaviLcdComm.recvtimeoutCnt = 0;
    s_stNaviLcdComm.responseFlag = 1;
    if(0==NaviLcdCommStartSendData(pBuffer,datalen))return 0;//���ͳɹ� 
    #if 1
    s_stNaviLcdComm.datalen = datalen;
    memcpy(s_ucNaviLcdTxBuffer,pBuffer,s_stNaviLcdComm.datalen);
    s_stNaviLcdComm.resendCnt = 3;//�ط��ʹ���
    LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_RESEND], PUBLICSECS(0.15), NaviLcdCommBusyForResendData); 
    return 1;//�ȴ��ط�
    #else
    if(0==s_stNaviLcdComm.resendCnt)
    {
        s_stNaviLcdComm.datalen = datalen;
        memcpy(s_ucNaviLcdTxBuffer,pBuffer,s_stNaviLcdComm.datalen);
        s_stNaviLcdComm.resendCnt = 3;//�ط��ʹ���
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_RESEND], PUBLICSECS(0.15), NaviLcdCommBusyForResendData); 
        return 1;//�ȴ��ط�
    }
    else//����æ,����ʧ��
    if((s_stNaviLcdComm.datalen+datalen)<COMNAVILCD_MAX_SEND_LEN)
    {
        memcpy(&s_ucNaviLcdTxBuffer[s_stNaviLcdComm.datalen],pBuffer,datalen);
        s_stNaviLcdComm.datalen += datalen;
        return 1;//�ȴ��ط�
    }  
    else//
    {
        return 2;//ʧ��
    } 
    #endif    
}
/************************************************************
** ��������: NaviLcdCommResendData
** ��������: 
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
unsigned char NaviLcdCommResendData(unsigned char *pBuffer,unsigned short datalen)
{
    if(datalen>COMNAVILCD_MAX_SEND_LEN)return 0xff;
    if(0==NaviLcdCommStartSendData(pBuffer,datalen))return 0;//���ͳɹ� 
    #if 1
    s_stNaviLcdComm.datalen = datalen;
    memcpy(s_ucNaviLcdTxBuffer,pBuffer,s_stNaviLcdComm.datalen);
    s_stNaviLcdComm.resendCnt = 3;//�ط��ʹ���
    LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_RESEND], PUBLICSECS(0.15), NaviLcdCommBusyForResendData); 
    return 1;//�ȴ��ط�
    #else
    if(0==s_stNaviLcdComm.resendCnt)
    {
        s_stNaviLcdComm.datalen = datalen;
        memcpy(s_ucNaviLcdTxBuffer,pBuffer,s_stNaviLcdComm.datalen);
        s_stNaviLcdComm.resendCnt = 3;//�ط��ʹ���
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_RESEND], PUBLICSECS(0.15), NaviLcdCommBusyForResendData); 
        return 1;//�ȴ��ط�
    }
    else//����æ,����ʧ��
    if((s_stNaviLcdComm.datalen+datalen)<COMNAVILCD_MAX_SEND_LEN)
    {
        memcpy(&s_ucNaviLcdTxBuffer[s_stNaviLcdComm.datalen],pBuffer,datalen);
        s_stNaviLcdComm.datalen += datalen;
        return 1;//�ȴ��ط�
    }  
    else//
    {
        return 2;//ʧ��
    } 
    #endif
}
/*************************************************************
** ��������: PactSendCmdForNaviLcd
** ��������: �����ݽ��д��,������
** ��ڲ���: cmd:���data:���ݿ��׵�ַ,datalen:����		 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PactSendCmdForNaviLcd(unsigned short cmd,unsigned char *data,unsigned short datalen)
{
    unsigned short i;
    unsigned short len;
    unsigned char VerifyCode;    
    i=0;
    s_ucNaviLcdTxBuffer[i++]=0xff;
    ///////////////////////
    s_ucNaviLcdTxBuffer[i++]=c_ucNaviLcdTag[0];
    s_ucNaviLcdTxBuffer[i++]=c_ucNaviLcdTag[1];
    //////////////////////////////////
    len=datalen+4;
    ///////////////////////////////////
    s_ucNaviLcdTxBuffer[i++] = (len>>8);
    //////////////////////////////////
    s_ucNaviLcdTxBuffer[i++] = (len & 0xff);
    //////////////////////////////////
    s_ucNaviLcdTxBuffer[i++] = EQUIP_TYPE_NAVILCD_NO;
    
    //s_ucNaviLcdTxBuffer[i++] = NAVILCD_FACTORY_NO;
    s_ucNaviLcdTxBuffer[i++] = 0x00;
    ///////////////////////
    s_ucNaviLcdTxBuffer[i++] = (cmd>>8);
    //////////////////////////////////
    s_ucNaviLcdTxBuffer[i++] = (cmd & 0xff);
    /////////////////////////////////////////////
    len = i;
    for(i=0;i<datalen;i++)s_ucNaviLcdTxBuffer[len+i]=data[i];
    /////////////////////////////////////////////
    len  +=datalen;
    VerifyCode=Public_GetXorVerify(&s_ucNaviLcdTxBuffer[3],len-3);
    /////////////
    s_ucNaviLcdTxBuffer[len++]=VerifyCode;
    //////////////////////
    s_ucNaviLcdTxBuffer[len++] = c_ucNaviLcdTag[0];
    s_ucNaviLcdTxBuffer[len++] = c_ucNaviLcdTag[1];
    /////////////////////////////////
    s_ucNaviLcdTxBuffer[len++] = 0xff;
    ///////////////////////////
    s_stNaviLcdAttrib.RecvAnswerFlag = 0;
    s_stNaviLcdComm.datalen = len;
    ///////////////////
    NaviLcdCommSendData(s_ucNaviLcdTxBuffer,len);
}
/*************************************************************
** ��������: PactSendCmdForNaviLcd
** ��������: �����ݽ��д��,������
** ��ڲ���: cmd:���data:���ݿ��׵�ַ,datalen:����		 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PactSendCmdForNaviLcdEx(unsigned short cmd,unsigned char *data,unsigned short datalen)
{
    unsigned short i;
    unsigned short len;
    unsigned char VerifyCode; 
    unsigned char TxBuffer[COMNAVILCD_MAX_SEND_LEN];
    i=0;
    TxBuffer[i++]=0xff;
    ///////////////////////
    TxBuffer[i++]=c_ucNaviLcdTag[0];
    TxBuffer[i++]=c_ucNaviLcdTag[1];
    //////////////////////////////////
    len=datalen+4;
    ///////////////////////////////////
    TxBuffer[i++] = (len>>8);
    //////////////////////////////////
    TxBuffer[i++] = (len & 0xff);
    //////////////////////////////////
    TxBuffer[i++] = EQUIP_TYPE_NAVILCD_NO;
    
    //TxBuffer[i++] = NAVILCD_FACTORY_NO;
    TxBuffer[i++] = 0x00;
    ///////////////////////
    TxBuffer[i++] = (cmd>>8);
    //////////////////////////////////
    TxBuffer[i++] = (cmd & 0xff);
    /////////////////////////////////////////////
    len = i;
    for(i=0;i<datalen;i++)TxBuffer[len+i]=data[i];
    /////////////////////////////////////////////
    len  +=datalen;
    VerifyCode=Public_GetXorVerify(&TxBuffer[3],len-3);
    /////////////
    TxBuffer[len++]=VerifyCode;
    //////////////////////
    TxBuffer[len++] = c_ucNaviLcdTag[0];
    TxBuffer[len++] = c_ucNaviLcdTag[1];
    /////////////////////////////////
    TxBuffer[len++] = 0xff;
    ///////////////////////////
    NaviLcdCommSendData(TxBuffer,len);
    s_stNaviLcdComm.responseFlag = 0;
    ///////////////////
    //if(!IsNaviLcdCommBusy())//��æ,ֱ�ӷ���
    //{
    //    NaviLcdCommStartToSendData(TxBuffer,len);
    //}
}
/************************************************************
** ��������: NaviLcdGetStartPoint
** ��������: �������ݿ�ʼ��־��λ��
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
/*dxl,2016.5.11����
unsigned short NaviLcdGetStartPoint(unsigned char *buffer,unsigned short datalen)
{
    unsigned short i;
    for(i=0; i< datalen-2; i++)
    {
        if(*(buffer+i)==0x55 && *(buffer+i+1)==0xAA&& *(buffer+i+2) != 0xFF)
        {
            return i;
        }
    }
    ///////////////////
    return 0xffff;
}
*/
unsigned short NaviLcdGetStartPoint(unsigned char *buffer,unsigned short datalen)
{
    unsigned short i;
    /////////////
    if(datalen<COMNAVILCD_MIN_LEN)return 0xffff;//modify by joneming
    //////////////////
    for(i=0; i< datalen-2; i++)
    {
        if(*(buffer+i)==0x55 && *(buffer+i+1)==0xAA&& *(buffer+i+2) != 0xFF)
        {
            return i;
        }
    }
    ///////////////////
    return 0xffff;
}
/************************************************************
** ��������: NaviLcdParseResultForRecvLen
** ��������: 
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdParseResultForRecvLen(unsigned char Resultflag)
{
    unsigned short datalen;
    switch(Resultflag)
    {
        case NV_RECV_ERR_START://�Ҳ�����ͷ
            s_stNaviLcdComm.recvlen = 0;
            s_stNaviLcdComm.disposelen = 0;
            break;
        case NV_RECV_ERR_TYPE:
        case NV_RECV_ERR_LEN:
        case NV_RECV_ERR_END:
            //////////�ҵ��µĿ�ʼ��־////////////////////////////
            s_stNaviLcdComm.disposelen +=2;//�����ϵĿ�ʼ��־
            datalen=NaviLcdGetStartPoint(&s_stNaviLcdComm.recvBuff[s_stNaviLcdComm.disposelen],s_stNaviLcdComm.recvlen-s_stNaviLcdComm.disposelen);
            if(0xffff == datalen)//û���ҵ��µĿ�ʼ��־///////
            {
                s_stNaviLcdComm.recvlen = 0;
            }
            else
            {
                s_stNaviLcdComm.disposelen +=datalen;
                if(s_stNaviLcdComm.recvlen>s_stNaviLcdComm.disposelen)
                {
                    s_stNaviLcdComm.recvlen -= s_stNaviLcdComm.disposelen;
                }
                else
                {
                    s_stNaviLcdComm.recvlen = 0;
                }
            }
            break;
        case NV_RECV_ERR_VERIFY:
        case NV_RECV_OK:
            if(s_stNaviLcdComm.recvlen>s_stNaviLcdComm.disposelen)
            {
                s_stNaviLcdComm.recvlen -= s_stNaviLcdComm.disposelen;
            }
            else
            {
                s_stNaviLcdComm.recvlen =0;
            }
            break;
    }
    /////////////////////////////////
    /////////////////////////////////////////////////
    if(s_stNaviLcdComm.recvlen < COMNAVILCD_MIN_LEN)//�ﲻ����С�������ݵĳ���
    {
        s_stNaviLcdComm.recvlen = 0;
    }
    else/////////////////////////////////
    {
        memmove((char *)&s_stNaviLcdComm.recvBuff[0], (char *)&s_stNaviLcdComm.recvBuff[s_stNaviLcdComm.disposelen], s_stNaviLcdComm.recvlen);
    }
}

/*************************************************************
** ��������: ProtocolParseForNaviLcd
** ��������: Э�����
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char NaviLcdProtocolParse(unsigned char *ParseBuffer,unsigned short usDataLenght)
{
    unsigned char VerifyCode;
    unsigned short datalen,cmd;
    unsigned char *buffer =NULL;
    ///////////���¸�λ///////////////
    s_stNaviLcdComm.disposelen = 0;
    //////////////////////////
    datalen=NaviLcdGetStartPoint(ParseBuffer,usDataLenght);
    if(0xffff != datalen)//�ҵ���ʼ��־
    {
        s_stNaviLcdComm.disposelen = datalen;
        buffer = ParseBuffer+datalen;
    }
    ///////////////////////////////////////////////
    if(c_ucNaviLcdTag[0] != buffer[0]||c_ucNaviLcdTag[1] != buffer[1])  return NV_RECV_ERR_START;
    ////�������ͱ��///�����ڱ�������------out--------------
    if(EQUIP_TYPE_NAVILCD_NO != buffer[4])  return NV_RECV_ERR_TYPE;
    //////////////////////////////////////
    datalen=(buffer[2]<<8)+buffer[3];
    if(usDataLenght<datalen+7) return NV_RECV_ERR_LEN;//���Ȳ���////
    ////////////////////////////////////
    //////////////������־����////////////////////
    if(c_ucNaviLcdTag[0] != buffer[datalen+5]||c_ucNaviLcdTag[1] != buffer[datalen+6])  return NV_RECV_ERR_END;
    /////////�����//////////
    //cmd=(buffer[6]<<8)+buffer[7];
    //////////////////////////////
    //if(s_stNaviLcdAttrib.recvCmd != cmd)return 0;//����
    //////////////////////////
    /////////////////////////////////////////////////////////////
    VerifyCode=Public_GetXorVerify(&buffer[2],datalen+2);
    /////////////////////////////////////////////////////////////
    //////////////////////////////////////
    s_stNaviLcdComm.disposelen += datalen+7;
    ///////////////////////////////////
    s_stNaviLcdComm.contrecvtimeoutCnt = 0;
    s_stNaviLcdComm.conttryCnt =0;
    Io_WriteAlarmBit(ALARM_BIT_LCD_FAULT,RESET);
    /////////////////////////////////////////
    if(VerifyCode==buffer[datalen+4])//У����
    {
        s_stNaviLcdAttrib.RecvAnswerFlag = 1;
        ///////////////////////////
        cmd=(buffer[6]<<8)+buffer[7];
        s_stNaviLcdAttrib.recvCmd =cmd;
        s_stNaviLcdAttrib.recvDatalen = datalen-4;
        /////////////////////////////////
        s_stNaviLcdAttrib.onlinestaus = 1;
        ///////////����������������/////////////////////
        memcpy((char *)&s_ucNaviLcdParseBuffer[0], (char *)&buffer[NAVILCD_USER_DATA_START], s_stNaviLcdAttrib.recvDatalen);
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TASK],LZM_AT_ONCE,NaviLcd_DisposeRecvDataResponse);
        ///////////////������ȷ,������ճ�ʱ/////////////////////////////
        LZM_PublicKillTimer(&s_stNVTimer[NV_TIMER_BUSY_TIMEOUT]);  
        s_stNaviLcdComm.sendBusy = 0;
        /////////////////////////////////////////
        return NV_RECV_OK;
    }    
    return NV_RECV_ERR_VERIFY;
}

/************************************************************
** ��������: NaviLcd_DisposeCommProtocolParse
** ��������: ��ʱ������ջ�����Ĵ�������
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcd_DisposeCommProtocolParse(unsigned char * pInBuf, unsigned short InLength)
{
    //������ⲿ���ô��ڵ���ؽ�������
    #ifndef DEBUG_TEST_NAVILCD
//    ICCard_ProtocolParse(pInBuf,InLength); dxl,2015.9,
    #endif
    ////////////////////
//    OilWear_UartDataProtocolParse(pInBuf,  InLength); dxl,2015.9,
    /////////////////
    JointechOiLCost_OilProtocolCheck(pInBuf, InLength);
    //////////////////
    Temperature_ComRec(pInBuf, InLength);
    //////////////////
    //Gyro_CommDisposeRecvData(pInBuf, InLength);
	  Card_ParseCmd(pInBuf,InLength);
		//liamtsen add
		A2OBD_UartDataProtocolParse(pInBuf,InLength);
	///////////////////////////////////////
	if(ReadPeripheral2TypeBit(8))
	{
		Hand_WeightingSystem_ProtocolParse(pInBuf, InLength);
	}
}
/************************************************************
** ��������: NaviLcdAutoToDisposeRecvData
** ��������: ��ʱ������ջ�����Ĵ�������
             
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void NaviLcdAutoToDisposeRecvData(void)
{
    unsigned char flag;
    unsigned short datalen;
	  
//	  if(1 == ReadPeripheral2TypeBit(6))//bit6ΪIC�����������Ӷ�����ʱ���������������ԭ�����ٶ�̫����1���ڲ�Ӧ��
	//	{
	//	    return ;
	//	}
		
    datalen=COM2_ReadBuff(&s_stNaviLcdComm.recvBuff[s_stNaviLcdComm.recvlen],COMNAVILCD_MAX_REC_LEN-s_stNaviLcdComm.recvlen);
    ///////////////////////////////
    s_stNaviLcdComm.recvlen += datalen;
    ///////////////////////////////////
    if(0 == datalen)////��������ݳ���Ϊ0//////////    
    {
        if(s_stNaviLcdComm.recvlen)
        {
            /////////////////////////////////
            s_stNaviLcdComm.recvfreeCnt++;
            ////////////////////////////////
            if(s_stNaviLcdComm.recvfreeCnt >1)//�������ε���
            {
                s_stNaviLcdComm.recvfreeCnt = 0;//
								/////////////////////////////////
                NaviLcd_DisposeCommProtocolParse(s_stNaviLcdComm.recvBuff,s_stNaviLcdComm.recvlen);                
                ///////////////////////
                if(s_stNaviLcdComm.recvlen < COMNAVILCD_MIN_LEN)
                {
                    s_stNaviLcdComm.recvlen = 0;
                }
                else
                {
                    flag=NaviLcdProtocolParse(s_stNaviLcdComm.recvBuff,s_stNaviLcdComm.recvlen);
                    NaviLcdParseResultForRecvLen(flag);
                }
            }
        }
    }
    else//////////////////
    if(s_stNaviLcdComm.recvlen+10>COMNAVILCD_MAX_REC_LEN)//����������ʱ��
    {
        ///////////////////////////////////////
        s_stNaviLcdComm.recvfreeCnt = 0;
        ////////////////////////
        flag=NaviLcdProtocolParse(s_stNaviLcdComm.recvBuff,s_stNaviLcdComm.recvlen);
        ///////////////////////////////////////
        NaviLcd_DisposeCommProtocolParse(s_stNaviLcdComm.recvBuff,s_stNaviLcdComm.recvlen);        
        ///////////////////////
        NaviLcdParseResultForRecvLen(flag);
    }
    else//////////////////
    {
        s_stNaviLcdComm.recvfreeCnt = 0;
    }
}

/* �蹤�ڼ���ڼ��ύ�Ĵ���
void NaviLcdAutoToDisposeRecvData(void)
{
	
//    unsigned char flag;
    unsigned short datalen;
	
	  if(1 == ReadPeripheral2TypeBit(6))//bit6ΪIC�����������Ӷ�����ʱ���������������ԭ�����ٶ�̫����1���ڲ�Ӧ��
		{
		    return ;
		}
    datalen=COM2_ReadBuff(&s_stNaviLcdComm.recvBuff[s_stNaviLcdComm.recvlen],COMNAVILCD_MAX_REC_LEN-s_stNaviLcdComm.recvlen);
    ///////////////////////////////
    s_stNaviLcdComm.recvlen += datalen;
    ///////////////////////////////////
    if(0 == datalen)////????????0//////////    
    {
        if(0==s_stNaviLcdComm.recvlen)return;//???,???
        /////////////////////////////////
        s_stNaviLcdComm.recvfreeCnt++;
        ////////////////////////////////
        if(s_stNaviLcdComm.recvfreeCnt < 2)return; //???????????,???
        //////////////
        s_stNaviLcdComm.recvfreeCnt = 0;
    }
    else//////////////////
    if(s_stNaviLcdComm.recvlen+10<COMNAVILCD_MAX_REC_LEN)//???????,?????,???
    {
        s_stNaviLcdComm.recvfreeCnt = 0;
        return;
    }
    ////////////
    //?????????????????????????    
    //////////////////////////////////
    //NaviLcd_DisposeCommProtocolParse(s_stNaviLcdComm.recvBuff,s_stNaviLcdComm.recvlen); 
    if(s_stNaviLcdComm.disposelen>=s_stNaviLcdComm.recvlen)s_stNaviLcdComm.disposelen = 0;
    ////////////////////////////////
    NaviLcd_DisposeCommProtocolParse(&s_stNaviLcdComm.recvBuff[s_stNaviLcdComm.disposelen],s_stNaviLcdComm.recvlen-s_stNaviLcdComm.disposelen);//modify by joneming
    ///////////////////
    //flag=NaviLcdProtocolParse(s_stNaviLcdComm.recvBuff,s_stNaviLcdComm.recvlen); 2016.5.11
    ///////////////////////////////////////
    //NaviLcdParseResultForRecvLen(flag);2016.5.11
		s_stNaviLcdComm.recvlen = 0;//2016.5.11
    ///////////////////
    s_stNaviLcdComm.disposelen = s_stNaviLcdComm.recvlen;//modify by joneming
		
}
*/
/*************************************************************
** ��������: NaviLcd_GetCurOnlineStatus
** ��������: �������ڲ����� 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_GetCurOnlineStatus(void)
{
    return (1==s_stNaviLcdAttrib.onlinestaus)?1:0;
}
/*************************************************************
** ��������: NaviLcdSendCmdQueryStatus
** ��������:  ��ѯ״ָ̬��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdQueryStatus(void)
{
    unsigned char data[20];    
    Public_ConvertNowTimeToBCD(data);
    PactSendCmdForNaviLcd(NAVILCD_CMD_QUERY,data,7);    
    //PactSendCmdForNaviLcdEx(NAVILCD_CMD_QUERY,data,7);
}

/*************************************************************
** ��������: NaviLcdSendCmdResetLcd
** ��������: ϵͳ��λ
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdResetLcd(void)
{
    unsigned char data[1];
    PactSendCmdForNaviLcd(NAVILCD_CMD_RESET,data,0);
}

/*************************************************************
** ��������: NaviLcdSendCmdSetBaudrate
** ��������: LCDͨѶ����������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdSetBaudrate(unsigned char baudrate)
{
    unsigned char data[2];
    data[0] = baudrate;
    PactSendCmdForNaviLcd(NAVILCD_CMD_SET_BAUDRATE,data,1);
}

/*************************************************************
** ��������: NaviLcdSendCmdDislpay
** ��������: LCD���Ŀ���/�ر�
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdDislpay(unsigned char OnOff)
{
    unsigned char data[2];
    data[0] = OnOff;
    PactSendCmdForNaviLcd(NAVILCD_CMD_DISPLAY,data,1);
}

/*************************************************************
** ��������: NaviLcdSendCmdCaliTime
** ��������: LCDУʱָ��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdCaliTime(void)
{
    unsigned char data[20];    
    Public_ConvertNowTimeToBCD(data);
    PactSendCmdForNaviLcd(NAVILCD_CMD_CALI_TIME,data,7);
}

/*************************************************************
** ��������: NaviLcdSendCmdSetLcdParam
** ��������: LCD�豸��������ָ��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdSetLcdParam(unsigned char *data,unsigned short len)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_SET_LCD_PARAM,data,len);
}

/*************************************************************
** ��������: NaviLcdSendCmdHeartBeat
** ��������: ��������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdHeartBeat(void)
{
    unsigned char buffer[30];
    unsigned char data[20];  
    Public_ConvertNowTimeToBCD(data);
    
    Public_GetCurBasicPositionInfo(buffer);
    memcpy(&data[7],&buffer[8],8); //����γ��  
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_HEART_BEAT,data,15);
}

/*************************************************************
** ��������: NaviLcdSendCmdGeneralAck
** ��������: �ն˷���Lcd��ͨ��Ӧ��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdGeneralAck(unsigned short cmd,unsigned char result)
{
    PactSendCmdForNaviLcdEx(cmd,&result,1);
    s_stNaviLcdComm.responseFlag = 0;
}
/*************************************************************
** ��������: NaviLcdSendCmdQueryParameter
** ��������: ������ѯ����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdQueryParameter(unsigned char *buffer,unsigned short datalen)
{
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_TERMINAL_QUERY_PARAM,buffer,datalen);
}

/*************************************************************
** ��������: NaviLcdSendCmdModifyPassWord
** ��������: �޸�����
** ��ڲ���: buffer:������׵�ַ,�ҳ���4λ�ĳ�������,��λ��ǰ
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdModifyPassWord(unsigned char *buffer)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_MODIFY_PASSWORD,buffer,4);
}
/*************************************************************
** ��������: NaviLcdSendCmdNewCallPhone
** ��������: ����������
** ��ڲ���: �绰����
** ���ڲ���: ��
** ���ز���: �ȴ�������־:0:ֱ�Ӿܽ�����;1:���ڵȴ��û���������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcdSendCmdNewCallPhone(unsigned char *phone)
{
    unsigned char name[24];
    unsigned char data[60];
    unsigned char len,len1,flag;
    //////////////////////////////
    len=strlen((char *)phone);
    data[0] = len;
    memcpy(&data[1],phone,len);
    len++;
    ////////////////////////
    flag=Public_CheckPhoneInformation(phone,name);//0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����
    if(2 == flag||0==flag)//�绰�����ں���,�����ں���
    {
        communication_CallPhone("13800000000",PHONE_END);
        //ֱ�Ӿܽӵ绰/////////////
        return 0;
    }
    else//�绰�����ں���,���ڿɺ���
    if(flag)
    {        
        len1 = strlen((char *)name);
        data[len++] = len1;
        memcpy(&data[len],name,len1);
        len += len1;
    }
    /////////////////////////////
    PactSendCmdForNaviLcd(NAVILCD_CMD_NEW_CALL_PHONE,data,len);
    s_stNaviLcdAttrib.newphone =1;
    return 1;
}

/*************************************************************
** ��������: NaviLcdSendCmdQueryPhoneBooks
** ��������: ��ѯ�绰��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdQueryPhoneBooks(unsigned char *buffer,unsigned short datalen)
{
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_QUERY_PHONE_BOOK,buffer,datalen);
}

/*************************************************************
** ��������: NaviLcdSendCmdNewText
** ��������: �ı���ʾ
** ��ڲ���: display:��ʾ��־:0:��ʾ;1: ����ʾ; voice����������־:0: ����;1: ������;save�洢��־:0: �洢1: ���洢
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdNewText(unsigned char display,unsigned char voice,unsigned char save,unsigned char *buffer,unsigned short datalen)
{
    unsigned char data[1024];
    unsigned short len;
    /////////////////////////////
    len = 0;
    data[len++] = display;
    data[len++] = voice;
    data[len++] = save;
    memcpy((char *)&data[len],buffer,datalen);
    len += datalen;
    PactSendCmdForNaviLcd(NAVILCD_CMD_TEXT_DISPLAY,data,len);
}

/*************************************************************
** ��������: NaviLcdShowPlatformText
** ��������: �ı�/������Ϣ�·�
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdShowPlatformText(unsigned char *buffer,unsigned short datalen)
{
    if(NaviLcd_GetCurOnlineStatus())
    NaviLcdSendCmdNewText(0,1,0,buffer,datalen);
}
/*************************************************************
** ��������: NaviLcdShowTempText
** ��������: ��ʾ��ʱ��Ϣ
** ��ڲ���: ��Ҫ��ʾ����ʱ��Ϣ
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdShowTempText(unsigned char *string)
{
    NaviLcdSendCmdNewText(0,1,1,string,strlen((char*)string));
}
/*************************************************************
** ��������: NaviLcdSendCmdQuestionDisplayEx
** ��������: �����·�(��׼���⳵Э��,������ID)
** ��ڲ���: voice �Ƿ񲥷�,bufferԴ�����׵�ַ,datalenԴ���ݳ���
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdQuestionDisplayEx(unsigned char voice,unsigned char *buffer,unsigned short datalen)
{
    unsigned char data[1024];
    unsigned short len;
    /////////////////////////////
    len = 0;
    data[len++] = voice;
    memcpy((char *)&data[len],buffer,datalen);
    len += datalen;
    PactSendCmdForNaviLcd(NAVILCD_CMD_QUESTION_DISPLAY,data,len);
}

/*************************************************************
** ��������: NaviLcdSendCmdQuestionDisplay
** ��������: �����·�(�б�Э��,��������ID)
** ��ڲ���: voice �Ƿ񲥷�,bufferԴ�����׵�ַ,datalenԴ���ݳ���
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdQuestionDisplay(unsigned char voice,unsigned char *buffer,unsigned short datalen)
{
    static unsigned long sulID=0; 
    unsigned char data[1024];
    unsigned short len,templen,tmplen1;
    ///////////////////
    /////////////////////////////
    sulID++;
    len = 0;    
    data[len++] = voice;
    data[len++] = 0;
    Public_ConvertLongToBuffer(sulID, &data[len]);
    len +=4;
    templen = 0;
    tmplen1 =buffer[templen++];
    memcpy((char *)&data[len],&buffer[templen],tmplen1);
    templen +=tmplen1;
    len +=tmplen1;
    data[len++]='\0';
    while(templen<datalen)
    {
        //��ID
        data[len++]=buffer[templen++];
        //�𰸳���
        tmplen1=(buffer[templen++]<<8);
        tmplen1 +=buffer[templen++];
        ////������////////////////////
        memcpy((char *)&data[len],&buffer[templen],tmplen1);
        templen += tmplen1;
        len  += tmplen1;
        data[len++]='\0';
    }
    /////////////////////////
    PactSendCmdForNaviLcd(NAVILCD_CMD_QUESTION_DISPLAY,data,len);
}
/*************************************************************
** ��������: NaviLcdSendCmdQueryFixText
** ��������: ��ѯ�̶��ı�
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdQueryFixText(unsigned char *buffer,unsigned short datalen)
{
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_QUERY_FIX_TEXT,buffer,datalen);
}

/*************************************************************
** ��������: NaviLcdSendCmdInfoService
** ��������: ��Ϣ��������
** ��ڲ���: voice �Ƿ񲥷�,bufferԴ�����׵�ַ,datalenԴ���ݳ���
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdInfoService(unsigned char voice,unsigned char *buffer,unsigned short datalen)
{
    unsigned char data[1024];
    unsigned short len;
    /////////////////////////////    
    len = 0;
    data[len++] = 0;    //����
    data[len++] = voice;//���ű�־
    memcpy((char *)&data[len],buffer,datalen);
    len += datalen;
    PactSendCmdForNaviLcd(NAVILCD_CMD_INFO_SERVICE,data,len);
}
/*************************************************************
** ��������: NaviLcdSendCmdCurrentDriverInfo
** ��������: ��ǰ��ʻԱ�����Ϣ����
** ��ڲ���: loginFlag��ǩ��־:(0:ǩ��,1:ǩ��); IDNumber֤������;driverName��ʻԱ����
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdCurrentDriverInfo(unsigned char loginFlag,char *IDNumber,char *driverName)
{
    unsigned char data[60];
    unsigned short len,temp;
    /////////////////////////////
    len = 0;
    data[len++] = 0;    //����
    data[len++] = loginFlag;//��ǩ��־
    if(0==loginFlag)
    {
        temp=strlen(IDNumber);
        data[len++] = temp;//
        memcpy(&data[len],IDNumber,temp);
        len +=temp;
        temp=strlen(driverName);
        data[len++] = temp;//
        if(temp)//
        {
            memcpy(&data[len],driverName,temp);
            len +=temp;
        }
    }
    PactSendCmdForNaviLcd(NAVILCD_CMD_CURRENT_DRIVER_INFO,data,len);
}

/*************************************************************
** ��������: NaviLcdSendCmdNewCallTaxi
** ��������: �µ�����Ϣ����
** ��ڲ���: voice �Ƿ񲥷� 0:����,1:������,bufferԴ�����׵�ַ,datalenԴ���ݳ���
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdNewCallTaxi(unsigned char voice,unsigned char *buffer,unsigned short datalen)
{
    unsigned char data[1024];
    unsigned short len;
    /////////////////////////////
    if(!NaviLcd_GetCurOnlineStatus())return;
    len = 0;
    data[len++] = voice;
    memcpy((char *)&data[len],buffer,datalen);
    len += datalen;
    PactSendCmdForNaviLcd(NAVILCD_CMD_CALL_TAXI_NEW,data,len);
    //////////////////////////////////////////////
}

/*************************************************************
** ��������: NaviLcdSendCmdCallTaxiDetail
** ��������: ������ϸ��Ϣ����
** ��ڲ���: voice �Ƿ񲥷�,bufferԴ�����׵�ַ,datalenԴ���ݳ���
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdCallTaxiDetail(unsigned char voice,unsigned char *buffer,unsigned short datalen)
{
    unsigned char data[1024];
    unsigned short len;
    /////////////////////////////
    if(!NaviLcd_GetCurOnlineStatus())return;
    len = 0;
    data[len++] = voice;
    memcpy((char *)&data[len],buffer,datalen);
    len += datalen;
    PactSendCmdForNaviLcd(NAVILCD_CMD_CALL_TAXI_DETAIL,data,len);
}

/*************************************************************
** ��������: NaviLcdSendCmdCallTaxiPlatformCancel
** ��������: ƽ̨ȡ����������
** ��ڲ���: bufferԴ�����׵�ַ,datalenԴ���ݳ���
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdCallTaxiPlatformCancel(unsigned char *buffer,unsigned short datalen)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_CALL_TAXI_CANCEL_PLATFORM,buffer,datalen);
}
/************************************************************
** ��������: NaviLcdParamInit
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
************************************************************/ 
void NaviLcdCommParamInit(void)
{
    s_stNVKey.datalen = 0; 
    memset(&s_stNaviLcdComm,0,13);
    COM2_Init(9600);
}

/*************************************************************
** ��������: NaviLcd_DisposeCallTaxiVie
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeCallTaxiVie(void)
{
  /*//joneming 2013.1.15
    ProtocolACK rev_ack;  
    ///////////////////////////
    if(s_stNaviLcdAttrib.recvDatalen<5)return 2;
    ////////////////////////
    if(s_ucNaviLcdParseBuffer[4]==0)//����
    {
        if(TexiService_CheckCurrentVieServiceID())
        {
            rev_ack=TexiService_SendTerminalDriverGlomLender();
            if(ACK_OK==rev_ack)
            {
                Public_PlayTTSVoiceStr("�����ѷ���");
                return 0;
            }
            else
            {
              Public_PlayTTSVoiceStr("����ʧ��");
            }
        }
        else
        {
          Public_PlayTTSVoiceStr("ҵ����ʧЧ");
        }
        return 2;
    }
    else
    {
        SaveTexiStatusFlagForCancel();
        return 0;
    }  
*/
return 0;//joneming 2013.1.15  
}
/*************************************************************
** ��������: NaviLcd_DisposeCallTaxiFinish
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeCallTaxiFinish(void)
{
  /*
    ProtocolACK rev_ack;
    if(s_stNaviLcdAttrib.recvDatalen<5)return 2;
    if(TexiService_CheckCurrentServiceID())
    {
        if(GetTexiServiceReportFlag())
        {
            Public_PlayTTSVoiceStr("ҵ�������");
            return 2; 
        }
        rev_ack=SendTexiServiceFinishRepor(s_ucNaviLcdParseBuffer[4]);
        if(ACK_OK==rev_ack)
        {
            Public_PlayTTSVoiceStr("��ɶ����ѷ���");
            return 0;
        }
        else
        {
          Public_PlayTTSVoiceStr("����ʧ��");
        }
    }
    else
    {
        Public_PlayTTSVoiceStr("ҵ���ѹ���");
    }
  */
    return 2;    
}
/*************************************************************
** ��������: NaviLcd_DisposeTerminalCancelCallTaxi
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeTerminalCancelCallTaxi(void)
{
  /*
    ProtocolACK rev_ack;
    if(s_stNaviLcdAttrib.recvDatalen<5)return 2;
    if(TexiService_CheckCurrentServiceID())
    {
        if(GetTexiServiceReportFlag())
        {
            Public_PlayTTSVoiceStr("ҵ�������");
            return 2; 
        }
        rev_ack=SendTexiServiceTerminalCancel(s_ucNaviLcdParseBuffer[4]);
        if(ACK_OK==rev_ack)
        {
            Public_PlayTTSVoiceStr("ȡ�������ѷ���");
            return 0;
        }
        else
        {
          Public_PlayTTSVoiceStr("����ʧ��");
        }
    }
    else
    {
        Public_PlayTTSVoiceStr("ҵ���ѹ���");
    }
  */
    return 2;    
}
/*************************************************************
** ��������: NaviLcd_DriverLogin
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DriverLogin(unsigned char *drivercode,unsigned char *password)
{
    unsigned char recordnum,flag;
    flag= CheckDriverInfoForLogin(drivercode,password,&recordnum);
    if(flag)return flag;
    if(NaviLcdDriverLoginFlag())
    {
        if(s_stDriverLog.recordNumber==recordnum)
        {
            Public_PlayTTSVoiceStr("�ü�ʻԱ�ѵ�¼");
            return 3;
        }
    }
    //д��ʻԱ���뵽eeprom
    SaveCurrentDriverParameter(recordnum);
    //���ƣ�ͼ�ʻ��Ϣ
    //TiredDrive_Login();  dxl,2015.9
    //�����ʻԱ�����
    NaviLcdSaveDriverLogin(recordnum);
    return 0;
}
/*************************************************************
** ��������: NaviLcd_SaveLogout
** ��������: :
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_LogoutSaveParam(void)
{	
    //unsigned char driver[7] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 
    ///////////////////
    //���õ�ǰ��ʻԱ����Ϊ0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    //EepromPram_WritePram(E2_CURRENT_DRIVER_ID, driver, E2_CURRENT_DRIVER_ID_LEN);
}
/*************************************************************
** ��������: NaviLcd_DriverLogout
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DriverLogout(void)
{   
    NaviLcd_LogoutSaveParam();
    ////////////////////
    //��ʻԱǩ��ʱ����
//    TiredDrive_Logout(); dxl,2015.9,
    //��¼��־��
    NaviLcdSaveDriverLogout();
    return 0;
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverSign
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendDriverSignResult(unsigned char *data,unsigned char datalen)
{
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_DRIVER_SIGN,data,datalen);
    s_stNaviLcdComm.responseFlag =0;
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverSign
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeDriverSign(void)
{
    unsigned char data[40];
    unsigned char type,len;
    type=s_ucNaviLcdParseBuffer[0];
    if(0 == type)//ǩ��
    {
        len=NaviLcd_DriverLogin(&s_ucNaviLcdParseBuffer[1],&s_ucNaviLcdParseBuffer[7]);
        if(len==0)
        {
            data[0] = type;
            data[1] = 0;
            len=GetDriverInfo(s_stDriverLog.recordNumber,&data[2],2);  
            len +=2;
            NaviLcdSendDriverSignResult(data,len);
        }
        else
        {
            data[0] = type;
            data[1] = len;
            NaviLcdSendDriverSignResult(data,2);
        }
    }
    else
    if(1 == type)//ǩ��
    {
        /////////////////////////
        if(NaviLcdDriverLoginFlag()==0)
        {
            Public_PlayTTSVoiceStr("δ��¼����ǩ��");
            data[0] = type;
            data[1] = 1;
            NaviLcdSendDriverSignResult(data,2);
        }
        else
        {
            data[0] = type;
            data[1] = 2;
            NaviLcdSendDriverSignResult(data,2);
        }
    }  
}

/*************************************************************
** ��������: NaviLcd_DisposeDriverSign
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeQueryDriverInfo(void)
{
    unsigned char type;
    unsigned char data[1024];
    unsigned short len =0;
    type = s_ucNaviLcdParseBuffer[0];
    if(0 == type)//ǩ��
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_LOGIN;
        NaviLcd_OpenBrushCard();
        len=GetDriverInfoAll(data,2);    
    }
    else
    if(1 == type)//ǩ��
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_LOGOUT;
        NaviLcd_OpenBrushCard();
        len = 1;
        data[0] =0;
    }
    else
    if(2 == type)//���/�޸ļ�ʻԱ
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_ADD;
        len=GetDriverInfoAll(data,2);
    }
    ////////////////////
    if(len==0)//�޼�¼ʱ
    {
        data[len++]=0;
    }
    /////////////////////////
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_DRIVER_QUERY_ALL,data,len);
    s_stNaviLcdComm.responseFlag =0;
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverInfoAddInfo
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendDriverInfoAdd(unsigned char result,unsigned char time)
{
    unsigned char buffer[10];
    buffer[0] = result;
    if(result == 0x02)
    {
        buffer[1] =time;
    }
    else
    {
         buffer[1] =0;
    }
    PactSendCmdForNaviLcd(NAVILCD_CMD_DRIVER_ADD,buffer,2);
}

/*************************************************************
** ��������: NaviLcd_DisposeDriverInfoAddInfo
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendDriverInfoAddOrModifyInfoResult(unsigned char *data,unsigned char datalen)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_DRIVER_ADD_RESULT,data,datalen);
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverInfoAddInfo
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeDriverInfoAddInfo(void)
{
    unsigned char buffer[120];
    unsigned char datalen,temp;
    temp =s_stDriverLog.result;
    if(temp)
    {
        buffer[0] = 1;//ʧ��
        buffer[1] = s_stDriverLog.resutltindex;//ʧ��ԭ��
        NaviLcdSendDriverInfoAddOrModifyInfoResult(buffer,2);
    }
    else
    {      
        buffer[0] = 0;//�ɹ�
        datalen=GetDriverInfo(s_stDriverLog.resutltindex,&buffer[1],2);
        datalen++;
        NaviLcdSendDriverInfoAddOrModifyInfoResult(buffer,datalen);
    } 
    ////////////////////////
    s_stDriverLog.recordindex = 0xAA;
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverInfoAddInfo
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeDriverInfoAdd(void)
{
    NaviLcd_OpenBrushCard();
    s_stDriverLog.cmdFlag = NV_CMD_FLAG_ADD;
    s_stDriverLog.recordindex = s_ucNaviLcdParseBuffer[0];
    memcpy(s_stDriverLog.drivercode,&s_ucNaviLcdParseBuffer[1],6);
    memcpy(s_stDriverLog.password,&s_ucNaviLcdParseBuffer[7],6);
    NaviLcdSendDriverInfoAdd(2,30);
    s_stDriverLog.result =1;//
    s_stDriverLog.resutltindex =0;
    LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_DRIVER_ADD],PUBLICSECS(30),NaviLcd_DisposeDriverInfoAddInfo); 
    s_stNaviLcdComm.responseFlag = 0;
}
/*************************************************************
** ��������: NaviLcd_GetDriverSpeedRecord
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned short NaviLcd_GetDriverSpeedRecord(unsigned char *buffer)
{
    strcpy((char *)buffer,"�޼�¼");
    return strlen((char *)buffer);
}
/*************************************************************
** ��������: NaviLcd_GetTiredDriveRecord
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned short NaviLcd_GetTiredDriveRecord(unsigned char *buffer)
{
    unsigned char data[100];
    unsigned short len = 0,i,maxcount,index;
    unsigned long TimeCount;
    TIME_T	CurrentTime,stTime;
    
    //����ǰʱ��
    RTC_GetCurTime(&CurrentTime);
    CurrentTime.hour = 0;
    CurrentTime.min = 0;
    CurrentTime.sec = 0;
    TimeCount = ConverseGmtime(&CurrentTime);
    if(TimeCount>DAY_SECOND)
    {
       TimeCount -= DAY_SECOND;
    }
    else
    {
      TimeCount =0;
    }
    Gmtime(&CurrentTime, TimeCount);
    //����ǰ��ʻԱ��Ϣ
//    len = TiredDrive_Read2daysTiredRecord(RadioShareBuffer,CurrentTime);//modify by joneming 20130312 dxl,2015.9,
    ////////////////////////
    //��ʾ��ʱ��ʻ��¼
    if(0 == len)//û��һ����¼,��ʾ"�޼�¼"
    {
        strcpy((char *)buffer,"�޼�¼");
    }
    else
    {
        ///////////////////
        maxcount = len/VDR_DATA_OVER_TIME_COLLECT_LEN;
        len=0;
        for(i = 0; i< maxcount; i++)
        {
            index =VDR_DATA_OVER_TIME_COLLECT_LEN*i;	
            sprintf((char *)&buffer[len],"��¼%d:",i+1);
            len=strlen((char *)buffer);
            strcpy((char *)&buffer[len]," ��ʻ֤����:");
            len=strlen((char *)buffer);
            memcpy(&buffer[len],&RadioShareBuffer[index],18);
            len+=18;
            index += 18;
            memcpy(data,&RadioShareBuffer[index],12);
            Public_ConvertBCDToTime(&stTime,data);            
            sprintf((char *)&buffer[len]," ��ʼʱ��:20%02d-%02d-%02d %02d:%02d:%02d ",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
            Public_ConvertBCDToTime(&stTime,&data[6]);
            len = strlen((char *)buffer);
            sprintf((char *)&buffer[len],"����ʱ��:20%02d-%02d-%02d %02d:%02d:%02d ",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
            len = strlen((char *)buffer);
        }
    }
    len = strlen((char *)buffer);
    return len;    
}

/*************************************************************
** ��������: NaviLcd_StartPrintStopRecord
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_StartPrintStopRecord(void)
{
    LZM_StartNewTask(TaskMenuPrint);//��ӡ
    //SetTimerTask(TIME_PRINT, SYSTICK_0p1SECOND);
}
/*************************************************************
** ��������: NaviLcd_GetDriverInfo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned short NaviLcd_GetDriverInfo(unsigned char *buffer)
{
    unsigned short len = 0;
    if(0 == NaviLcdDriverLoginFlag())//û��½
    {
        strcpy((char *)buffer,"δ��¼!");
        len = strlen((char *)buffer);
        return len;
    }
    ////////////////////////////
//    len=ICCard_GetExternalDriverInfo(buffer); dxl,2015.9,
    //////////////////////////////
    return len;    
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverSign
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeBrushCardForDriverInfo(unsigned char type ,unsigned char *data,unsigned char datalen)
{
    if(0 == type)//ǩ��
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_LOGIN;  
    }
    else
    if(1 == type)//ǩ��
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_LOGOUT;
    }
    else
    if(2 == type)//���/�޸ļ�ʻԱ
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_ADD;
        LZM_PublicKillTimer(&s_stNVTimer[NV_TIMER_DRIVER_ADD]);        
    }
    else
    if(3 == type)   //ҵ���ϱ� 
    {
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_BUSINESS;
    }
    NaviLcd_DisposeBrushCardDataForDriverInfo(data,datalen);
}
/*************************************************************
** ��������: NaviLcd_DisposeBrushCard
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeBrushCard(void)
{
    unsigned char type;
    unsigned char flag;
    type = s_ucNaviLcdParseBuffer[0];
    flag = s_ucNaviLcdParseBuffer[1];    
    if(flag==0)
    {
        LZM_PublicKillTimer(&s_stNVTimer[NV_TIMER_DRIVER_ADD]);
        NaviLcd_DisposeBrushCardForDriverInfo(type,&s_ucNaviLcdParseBuffer[2],s_stNaviLcdAttrib.recvDatalen-2);
    }    
}
/*************************************************************
** ��������: NaviLcdSendDataToWriteICcard
** ��������: д��ָ��
** ��ڲ���:  flag����,type������,buffer�������׵�ַ,datalen�����ݳ���
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendDataToWriteICcard(unsigned char flag,unsigned char type,unsigned char *buffer,unsigned char datalen)
{
    unsigned char data[256];
    data[0] =flag;//����
    data[1] =type;//������
    memcpy(&data[2],buffer,datalen);
    PactSendCmdForNaviLcd(NAVILCD_CMD_WRITE_IC_CARD,data,datalen+2);
}
/*************************************************************
** ��������: NaviLcdSendCmdReadICcardData
** ��������: ��IC������
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSendCmdReadICcardData(void)
{
    unsigned char data[1];
    data[0] =1;//����
    PactSendCmdForNaviLcd(NAVILCD_CMD_READ_IC_CARD,data,1);
}
/*************************************************************
** ��������: NaviLcdSendCmdReadICcardData
** ��������: ��IC������
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeReadICcardData(void)
{
    unsigned char flag;
    unsigned short datalen;
    unsigned char len;
    datalen = 0;
    flag = s_ucNaviLcdParseBuffer[0];//����
    if(flag==0)
    {     
        len = s_ucNaviLcdParseBuffer[datalen+1];
        datalen++;//��ʻԱ��������
        datalen += len; //��ʻԱ���� 
        /////////////////
        datalen += 20;//��ʻԱ���֤����
        datalen += 40;//��ҵ�ʸ�֤����
        //////////////////
        len = s_ucNaviLcdParseBuffer[datalen+1];//
        datalen++;//��֤��������
        datalen += len;//��֤����
        //memcpy(buffer,&s_ucNaviLcdParseBuffer[1],datalen);
    }
}
/*************************************************************
** ��������: NaviLcdSendWriteDataICcard
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeWriteICcardResult(unsigned char result)
{
    if(0 == result)
    {
        Public_PlayTTSVoiceStr("д���ɹ�");
    }
    else
    if(2 == result)
    {
        Public_PlayTTSVoiceStr("�޿�������ʧ��");
    }
    else
    {
        Public_PlayTTSVoiceStr("д��ʧ��");
    }
}
/*************************************************************
** ��������: NaviLcd_DisposeDriverRecord
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeDriverRecord(void)
{
    unsigned char buffer[512];
    unsigned short datalen =0;
    unsigned char type;
    type = s_ucNaviLcdParseBuffer[0];
    switch(type)
    {
        case DRIVER_SPEED_RECORD:
            datalen = NaviLcd_GetDriverSpeedRecord(buffer);
            break;
        case DRIVER_TIRED_RECORD:
            datalen = NaviLcd_GetTiredDriveRecord(buffer);
            break;
        case DRIVER_STOP_RECORD:
            ///////������ӡ����////////////
            datalen = 0;
            NaviLcd_StartPrintStopRecord();
            break;
        case DRIVER_INFO_RECORD:
            datalen = NaviLcd_GetDriverInfo(buffer);
            break;
    }
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_DRIVER_RECORD,buffer,datalen);
}

/*************************************************************
** ��������: NaviLcd_DisposeDriverRecord
** ��������: 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeDriverBusinessReport(void)
{
  /*
    unsigned char data[512];
    unsigned char datalen,type;
    ProtocolACK rev_ack;
    type = s_ucNaviLcdParseBuffer[0];
    datalen =0;
    if(type==0)
    {
        rev_ack = RadioProtocol_ElectricReceiptReport(data,datalen);
    }
    else
    if(type==1)
    {
        if(NaviLcdDriverLoginFlag())
        {
            datalen=GetDriverInfo(s_stDriverLog.recordNumber,data,0);
        }
        rev_ack = RadioProtocol_DriverInformationReport(data,datalen);
    }
    if(ACK_OK==rev_ack)return 0;
  */
    return 2; 
}
///////////////////////////////////////////////////////////
extern TIME_TASK MyTimerTask[];
extern u16 DelayAckCommand;
/*******************************************************************************
**  ��������  : NaviLcd_DisposeTerminalCtrlQuickOpenAccount
**  ��������  : ���ٿ���
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_DisposeTerminalCtrlQuickOpenAccount(void)
{
    Public_QuickOpenAccount(); 
}
/*******************************************************************************
**  ��������  : NaviLcd_DisposeTerminalCtrlCarLoadStatus
**  ��������  : �����ػ�״̬ 
**  ��    ��  : status: 0�����ء�1�����ء�2������
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : SetEvTask(EV_REPORT);
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_DisposeTerminalCtrlCarLoadStatus(unsigned char status)
{
    Public_SetCarLoadStatus(status);
    Public_WriteDataToFRAM(FRAM_CAR_CARRY_STATUS_ADDR, &status, 1);
}
/*******************************************************************************
**  ��������  : NaviLcd_DisposeTerminalControl
**  ��������  : 
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
unsigned char NaviLcd_DisposeTerminalControl(void)
{
    unsigned char control;    
    ProtocolACK ACK_val;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    control=s_ucNaviLcdParseBuffer[0];
    ACK_val = ACK_OK;
    switch(control)
    {
        case TERMINAL_CTRL_REGISTER://�ն�ע��
            ACK_val = RadioProtocol_TerminalRegister(channel);
            break;
        case TERMINAL_CTRL_CONFIRM_ID://�ն˼�Ȩ
            ACK_val=RadioProtocol_TerminalQualify(CHANNEL_DATA_1);//ֻ��һ��һ��ͨ��������Ϊͨ����һ����Ȩ��Ҳ��һ��      
            ACK_val=RadioProtocol_TerminalQualify(CHANNEL_DATA_2);
            break;
        case TERMINAL_CTRL_LOGOUT://�ն�ע��
            ACK_val= RadioProtocol_TerminalLogout(channel);
            if(ACK_OK ==ACK_val)
            {
                s_stNaviLcdAttrib.logout =1;
                ClearTerminalAuthorizationFlag(0xff);
                if(MyTimerTask[TIME_DELAY_TRIG].TaskTimerState == DISABLE)
                {
                    DelayAckCommand = 0x0003;                    
                    //������ʱ����
                    SetTimerTask(TIME_DELAY_TRIG, 2*SYSTICK_0p1SECOND);
                    //������Ȩ��
									  if(1 == BBGNTestFlag)//dxl,2016.5.13������ʱֻ���յ�ע��Ӧ��������Ȩ��
										{
										
										}
										else
										{
                        ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
                        ClearTerminalAuthorizationCode(CHANNEL_DATA_2);
										}
                }
            }
            break;
        case TERMINAL_CTRL_RESET://�ն˸�λ
            LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TEST],PUBLICSECS(2),NVIC_SystemReset);
            break;
        case TERMINAL_CTRL_FEATURE://��������ϵ��У׼
//            if(!ADJ_getAdjFlg())setAdjustFunction(ENABLE); dxl,2015.9,
            break;
        case TERMINAL_CTRL_DEFAULT_SET://�ָ�������������
            EepromPram_DefaultSet();
            break;
        case TERMINAL_CTRL_QUICK_OPEN_ACCOUNT://���ٿ���
            NaviLcd_DisposeTerminalCtrlQuickOpenAccount();
            break;
        case TERMINAL_CTRL_CAR_LOAD_STATUS://�����ػ�״̬
            NaviLcd_DisposeTerminalCtrlCarLoadStatus(s_ucNaviLcdParseBuffer[1]);
            break;
    }
    if(ACK_OK == ACK_val)
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

/*******************************************************************************
** ��������: NaviLcd_DisposeQueryStatusAlarm
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeQueryStatusAlarm(void)
{ 
    unsigned char buf[30];
    unsigned long temp[3];
    if(1==s_ucNaviLcdParseBuffer[0])
    {
    temp[0]=Io_ReadAlarm();
    Public_ConvertLongToBuffer(temp[0],buf);     
    temp[1]=Io_ReadStatus();
    Public_ConvertLongToBuffer(temp[1],&buf[4]);
    temp[2]=Io_ReadExtCarStatus();
    Public_ConvertLongToBuffer(temp[2],&buf[4]);
    }
    else
    {
        temp[0]=Io_ReadAlarm();
        Public_ConvertLongToBuffer(temp[0],buf);     
        temp[1]=Io_ReadStatus();        
        temp[1] = (temp[1] & 0x1fff);
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT))
        {
            PUBLIC_SETBIT(temp[1], 18);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE))
        {
            PUBLIC_SETBIT(temp[1], 20);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_HORN))
        {
            PUBLIC_SETBIT(temp[1], 21);
        }        
        if(Io_ReadStatusBit(STATUS_BIT_DOOR2))
        {
            PUBLIC_SETBIT(temp[1], 25);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT))
        {
            PUBLIC_SETBIT(temp[1], 26);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT))
        {
            PUBLIC_SETBIT(temp[1], 27);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT))
        {
            PUBLIC_SETBIT(temp[1], 28);
        }
        if(Io_ReadStatusBit(STATUS_BIT_DOOR1))
        {
            PUBLIC_SETBIT(temp[1], 29);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_AIR_CONDITION))
        {
            PUBLIC_SETBIT(temp[1], 30);
        }
        if(Io_ReadExtCarStatusBit(CAR_STATUS_BIT_SIDE_LIGHT))
        {
            PUBLIC_SETBIT(temp[1], 31);
        }
        Public_ConvertLongToBuffer(temp[1],&buf[4]);
        temp[2] = 0;
        Public_ConvertLongToBuffer(temp[2],&buf[8]);
    }
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_QUERY_STATUS_ALARM,buf,12);
}

/*******************************************************************************
** ��������: NaviLcd_DisposeQueryWorkStatus
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeQueryWorkStatus(void)
{ 
    GPS_STRUCT stTmpGPS;
    unsigned char buffer[30]={0};
    unsigned char buf[30]={0};
    unsigned char temp1 = 0; 
    unsigned short temp;
    //////////////////////
    //��ǰ�ٶ�
//    temp1 = SpeedMonitor_GetCurSpeed(); dxl,2015.9,
    ////////////////
    buf[0] = temp1;//�ٶ�   
    buf[1] = communicatio_GetMudulState(COMMUNICATE_STATE)?1:0;//��ͨ��־
    ///////////////////////////////////////
    temp1 =communicatio_GetMudulState(SIGN_STRENTH);//�����ź���    
    if(temp1 !=0)//���ź�
    {
        temp1 = temp1/6 + 1;
        if(temp1 > 5)
        {
            temp1 = 5;//����ź�ǿ��Ϊ5��
        }
    }
    //////////////////////////////
    buf[2] = 0;
    //buf[2] = (Gps_GetCurrentStarNumEx()<<4);    
    buf[2] |= temp1;//�����ź���
    ////////////////////////////////////
    /////////////////////////////////
    buf[3] = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);//��λ��־
    ////////////////////////////////////
    temp = Gps_ReadCourse();//����
    temp1= temp*2/3;
    buf[4] = buf[3]?temp1:0;//����
    ////////////////////////////
    ////////////////////
    Public_GetCurBasicPositionInfo(buffer);
    memcpy(&buf[5],&buffer[8],8); //γ��\����
    buf[13] = Io_ReadStatusBit(STATUS_BIT_SOUTH);
    buf[14] = Io_ReadStatusBit(STATUS_BIT_EAST);
    ////////////////////
    /////����λ//////////
    buf[15] = 0;
    /////���ٱ���λ//////////
    temp =Io_ReadAlarmBit(ALARM_BIT_OVER_SPEED);    
    if(temp)
    PUBLIC_SETBIT(buf[15],0);
    /////ƣ�ͼ�ʻ//////////
    temp =Io_ReadAlarmBit(ALARM_BIT_TIRED_DRIVE);    
    if(temp)
    PUBLIC_SETBIT(buf[15],1);
    //////�ն�ע��//////////////////////////
    if(1 == s_stNaviLcdAttrib.logout)
    PUBLIC_SETBIT(buf[15],2);
    //////����������//////////////////////////
    #ifdef USE_SYS_CAR_LOAD
//    if(Carload_GetSensorAbnormalFlag()) dxl,2015.9,
//    PUBLIC_SETBIT(buf[15],3);
//    if(CarLoad_GetCurOverLoadFlag()) dxl,2015.9,
 //   PUBLIC_SETBIT(buf[15],5);
    #endif
    //////////////////////
    #ifdef USE_QUICK_OPEN_ACCOUNT    
//    if(OpenAccount_GetCurLockStatus())dxl,2016.4.20
  //  PUBLIC_SETBIT(buf[15],4);
    #endif
		
		if(1 == Lock2_GetEnableFlag())
		{
		    if(1 == Lock2_GetLock2Flag())
				{
				     PUBLIC_SETBIT(buf[15],4);
				}
		}
		
     /////GPS���ǿ���//////////
    Gps_CopygPosition(&stTmpGPS);
    buf[16] = stTmpGPS.SatNum;
    ///////�������ǿ���///////////////////////////////
    buf[17] = 0;
    //////////////////
    #ifdef USE_SYS_CAR_LOAD
//    Public_ConvertLongToBuffer(Carload_GetCurTotalWeight(), &buf[18]); dxl,2015.9,
    #endif
    /////////////////////
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_QUERY_WORK_STATUS,buf,24);
}
/*******************************************************************************
** ��������: NaviLcd_SendWorkStatus
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_SendWorkStatus(void)
{
    static unsigned char sucCnt=0;
    static unsigned char sucCnt1=0;
    sucCnt++;
    if(!IsNaviLcdCommBusy())//��æ,ֱ�ӷ���
    {
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_STATUS],PUBLICSECS(2),NaviLcd_SendWorkStatus);        
        sucCnt =0;
        sucCnt1++;
        if(sucCnt1>=15)//30s��������
        {
           sucCnt1 =0;
           NaviLcdSendCmdQueryStatus();
        }
        else
        {
            NaviLcd_DisposeQueryWorkStatus();
        }        
    }    else

    if(sucCnt<4)//////////////////
    {
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_STATUS],PUBLICSECS(0.2),NaviLcd_SendWorkStatus);
    }
    else
    {
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_STATUS],PUBLICSECS(1.2),NaviLcd_SendWorkStatus);
    }
}

/*******************************************************************************
** ��������: NaviLcd_DisposeOutCityRegister
** ��������: ���ǵǼ� 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
unsigned char NaviLcd_DisposeOutCityRegister(void)
{
  /*//joneming 2013.1.15
    unsigned char buffer[1];
    ProtocolACK rev_ack;
    ////////������ó��ǵǼǺ���///////////////////////
    rev_ack = RadioProtocol_OutRegister(buffer,0);
    ///////////////////
    if(ACK_OK==rev_ack)return 0;//���ͳɹ�
  */
    return 0xff;////����ʧ��
}
/*******************************************************************************
** ��������: NaviLcd_DisposeOutCityRegister
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeExportDataToUsbDisk(void)
{
    Usb_FindSDcardFileAndSaveToUdisk(s_ucNaviLcdParseBuffer[0],&s_ucNaviLcdParseBuffer[1],&s_ucNaviLcdParseBuffer[7]);
}
/*******************************************************************************
** ��������: NaviLcd_SendCarLoadClearSensorResult
** ��������: ���ʹ�����������
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_SendCarLoadClearSensorResult(void)
{
	/* dxl,2015.9,
    #ifdef USE_SYS_CAR_LOAD    
    unsigned char buffer[10]={0};
    unsigned char len;
    len =0;
    buffer[len++] = 1;//����
    //Public_ConvertLongToBuffer(Carload_GetClearSensorResult(),&buffer[len]); dxl,2015.9,
    //len +=4;
    //////////////////
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_CARLOAD_WEIGHT,buffer,len);
    #endif
	*/
}
/*******************************************************************************
** ��������: NaviLcd_SendCarLoadQuerySensorResult
** ��������: ���Ͳ�ѯ���������
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_SendCarLoadQuerySensorResult(void)
{
	/* dxl,2015.9,
    #ifdef USE_SYS_CAR_LOAD    
    unsigned char buffer[10]={0};
    unsigned char len;
    len =0;
    buffer[len++] = 0;//����
    Public_ConvertLongToBuffer(Carload_GetCurSensorAbnormalValue(),&buffer[len]);
    len +=4;
    //////////////////
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_CARLOAD_WEIGHT,buffer,len);
    #endif
	*/
}
/*******************************************************************************
** ��������: NaviLcd_DisposeCarLoadWeight
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeCarLoadWeight(void)
{
	/* dxl,2015.9,
    #ifdef USE_SYS_CAR_LOAD
    unsigned char buffer[10]={0};
    unsigned char type,len;
    type=s_ucNaviLcdParseBuffer[0];
    len =0;
    buffer[len++] = type;    
    if(0==type)//��ѯ����������
    {
        if(!CarLoad_GetCurOnlineStatus())return;//
        Carload_LcdSendCmdQuerySensorAbnormal();
        return;
    }
    else
    if(1==type)//��մ���������
    {
        Carload_SendCmdClearSensorAbnormal();
        buffer[len++] = 0;
        return;
    } 
    else
    if(2==type)//�˻���Ϣ
    {
//        CarLoad_SendFreightInformationToService(s_ucNaviLcdParseBuffer[1]); dxl,2015.9,
        buffer[len++] =0;
    } 
    //////////////////
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_CARLOAD_WEIGHT,buffer,len);
    #endif
		*/
}
/*******************************************************************************
** ��������: NaviLcd_CheckDataIsIPFormat
** ��������: �ж������Ƿ�ΪIP��ַ��ʽ
** ��ڲ���: data�����׵�ַ,datalen���ݳ���
** ���ز���: 1��IP��ַ��ʽ,0����IP��ַ��ʽ��ʽ
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
unsigned char NaviLcd_CheckDataIsIPFormat(unsigned char *data,unsigned char datalen)
{
    unsigned char i;
    for(i=0; i<datalen; i++)
    {
        if((data[i]<0x30||data[i]>0x39)&&data[i]!='.'&&data[i]!=0)
        {
            return 0;
        }
    }
    return 1;
}
/*******************************************************************************
** ��������: NaviLcd_QueryOrSaveTerminalParamter
** ��������: ������ѯ����
** ��ڲ���: ID:����ID,	datalen��������;buffer:��������,type:����,0:����;1:��ѯ;
** ���ڲ���: ��
** ���ز���: ��ǰ�������֮��ĳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
unsigned char NaviLcd_QueryOrSaveTerminalParamter(unsigned char ID,unsigned char datalen,unsigned char *buffer,unsigned char type)
{
    unsigned long val;
    unsigned char len =0;   
    unsigned char buf[40]={0};
    switch(ID)
    {
        case TERMINAL_PARAM_MAIN_APN://�������� APN
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAIN_SERVER_APN_ID);
                }
            }
            break;
        case TERMINAL_PARAM_RADIO_USER://������������ͨ�Ų����û���
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAIN_SERVER_DIAL_NAME_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAIN_SERVER_DIAL_NAME_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAIN_SERVER_DIAL_NAME_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAIN_SERVER_DIAL_NAME_ID);
                }
            }            
            break;
        case TERMINAL_PARAM_RADIO_PASSWORD://������������ͨ�Ų�������
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAIN_SERVER_DIAL_PASSWORD_ID);
                }
            }  
            break;        
        case TERMINAL_PARAM_BAK_APN://���ݷ����� APN������ͨ�Ų��ŷ��ʵ�
            if(type)
            {
                len=EepromPram_ReadPram(E2_BACKUP_SERVER_APN_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, buffer,datalen);
                EepromPram_ReadPram(E2_BACKUP_SERVER_APN_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_BACKUP_SERVER_APN_ID);
                }
            }
            break;
        case TERMINAL_PARAM_BAK_RADIO_USER://���ݷ���������ͨ�Ų����û���  
            if(type)
            {
                len=EepromPram_ReadPram(E2_BACKUP_SERVER_DIAL_NAME_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_NAME_ID, buffer,datalen);
                EepromPram_ReadPram(E2_BACKUP_SERVER_DIAL_NAME_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_BACKUP_SERVER_DIAL_NAME_ID);
                }
            }
            break;
        case TERMINAL_PARAM_BAK_RADIO_PASSWORD://���ݷ���������ͨ�Ų�������
            if(type)
            {
                len=EepromPram_ReadPram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, buffer,datalen);
                EepromPram_ReadPram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_BACKUP_SERVER_DIAL_PASSWORD_ID);
                }
            }
            break;        
        case TERMINAL_PARAM_TCP://������ TCP �˿� 
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAIN_SERVER_TCP_PORT_ID);
                }
            }
            break;
        case TERMINAL_PARAM_UDP://������ UDP �˿�
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAIN_SERVER_UDP_PORT_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAIN_SERVER_UDP_PORT_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAIN_SERVER_UDP_PORT_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAIN_SERVER_UDP_PORT_ID);
                }
            }
            break;
        case TERMINAL_PARAM_SPEED_ALARM://����ٶȣ���λΪ����ÿСʱ��km/h��
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAX_SPEED_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAX_SPEED_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAX_SPEED_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAX_SPEED_ID);
                }
            }
            break;
        case TERMINAL_PARAM_TOTAL_MILE://������̱������1/10km 
            if(type)
            {
                len=E2ParamApp_ReadSpecialParam(E2_CAR_TOTAL_MILE_ID,buffer);
                //len=EepromPram_ReadPram(E2_CAR_TOTAL_MILE_ID, buffer);
            }
            else
            {
                
                EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_TOTAL_MILE_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID);
                }
            }
            break;
        case TERMINAL_PARAM_CAR_PLATE://������ͨ�����Ű䷢�Ļ���������
            if(type)
            {
                len=EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_CAR_PLATE_NUM_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_PLATE_NUM_ID);
                }
            }
            break;
        case TERMINAL_PARAM_CAR_COLOR://������ɫ������ JT/T415-2006 �� 5.4.12
            if(type)
            {
                len=EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, buffer);
            }
            else
            {
                
                EepromPram_WritePram(E2_CAR_PLATE_COLOR_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_PLATE_COLOR_ID);
                }                
            }
            break;
        case TERMINAL_PARAM_PHONE_NUMBER://�ն��ֻ���
            if(type)
            {
                len=EepromPram_ReadPram(E2_DEVICE_PHONE_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_DEVICE_PHONE_ID, buffer,datalen);
                EepromPram_ReadPram(E2_DEVICE_PHONE_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_DEVICE_PHONE_ID);
                }
            }
            break;
        case TERMINAL_PARAM_CAR_VIN://����ʶ����
            if(type)
            {
                len=EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, buffer);
            }
            else
            {
                
                EepromPram_WritePram(E2_CAR_IDENTIFICATION_CODE_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_IDENTIFICATION_CODE_ID);
                }
            }
            break;
        case TERMINAL_PARAM_CAR_PLATE_TYPE://���Ʒ��� 
            if(type)
            {
                buffer[0]=1;
                len=EepromPram_ReadPram(E2_CAR_TYPE_ID, buf);
                if(len)
                {
                    unsigned char i;
                    for(i=0;i<4;i++)
                    {
                        if(strncmp((char*)buf,(char*)c_apcCarPlakeTypeItems[i],4)==0)
                        {
                            buffer[0]=i+1;
                            break;
                        }
                    }
                }
                len=1;
            }
            else
            {
                val =buffer[0]-1;
                len=strlen(c_apcCarPlakeTypeItems[val]);
                EepromPram_WritePram(E2_CAR_TYPE_ID, (unsigned char*)c_apcCarPlakeTypeItems[val],len);
                EepromPram_ReadPram(E2_CAR_TYPE_ID, buf);
                if(strncmp((char*)buf,(char*)c_apcCarPlakeTypeItems[val],len)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_TYPE_ID);
                }
            }            
            break;
        case TERMINAL_PARAM_SPEED_SENSOR://�ٶȴ�����ϵ��
            if(type)
            {
                len=EepromPram_ReadPram(E2_SPEED_SENSOR_COEF_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_SPEED_SENSOR_COEF_ID, buffer,datalen);
                EepromPram_ReadPram(E2_SPEED_SENSOR_COEF_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_SPEED_SENSOR_COEF_ID);
                }
            }
            break;
        case TERMINAL_PARAM_FEATURE_COEF://��������ϵ��
            if(type)
            {
                len=EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_FEATURE_COEF_ID);
                }
            }
            break;
        case TERMINAL_PARAM_SPEED_SELECT://�ٶ�ѡ�񿪹�
            if(type)
            {
                len=EepromPram_ReadPram(E2_SPEED_SELECT_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_SPEED_SELECT_ID, buffer,datalen);
                EepromPram_ReadPram(E2_SPEED_SELECT_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_SPEED_SELECT_ID);
                }
            }
            break;
        case TERMINAL_PARAM_SET_BACKLIGHT://����ѡ�񿪹�            
            if(type)
            {
                len=EepromPram_ReadPram(E2_BACKLIGHT_SELECT_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_BACKLIGHT_SELECT_ID, buffer,datalen);
                EepromPram_ReadPram(E2_BACKLIGHT_SELECT_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_BACKLIGHT_SELECT_ID);
                }
            }            
            break;
        case TERMINAL_PARAM_DOOR_LEVEL://������������            
            if(type)
            {
                len=EepromPram_ReadPram(E2_DOOR_SET_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_DOOR_SET_ID, buffer,datalen);
                EepromPram_ReadPram(E2_DOOR_SET_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_DOOR_SET_ID);
                }
            }
            break;
        case TERMINAL_PARAM_CDMA_SLEEP://CDMA���߿��� 
            if(type)
            {
                len=EepromPram_ReadPram(E2_CDMA_SLEEP_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_CDMA_SLEEP_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CDMA_SLEEP_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CDMA_SLEEP_ID);
                }
            }
            break;
        case TERMINAL_PARAM_CAR_HORN_LEVEL://�����ȵ�ƽѡ�� 
            if(type)
            {
                len=EepromPram_ReadPram(E2_HORN_SET_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_HORN_SET_ID, buffer,datalen);
                EepromPram_ReadPram(E2_HORN_SET_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_HORN_SET_ID);
                }
            } 
            break;
        case TERMINAL_PARAM_TERMINAL_ID://�ն�ID(LCDֻ�ɲ�ѯ������д��)
            if(type)
            {
                len=EepromPram_ReadPram(E2_DEVICE_ID, buffer);
            }            
            else
            {
                #if 0 //����д��
                EepromPram_WritePram(E2_DEVICE_ID, buffer,datalen);
                EepromPram_ReadPram(E2_DEVICE_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_DEVICE_ID);
                }
                #endif
            }
            break;
        case TERMINAL_PARAM_PASSWORD:
            if(type)
            {
                len=EepromPram_ReadPram(E2_PASSWORD_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_PASSWORD_ID, buffer,datalen);
                EepromPram_ReadPram(E2_PASSWORD_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_PASSWORD_ID);
                }
            }
            break;
       #ifdef USE_SYS_CAR_LOAD
        case TERMINAL_PARAM_WEIGHT:
            if(type)
            {
                len=EepromPram_ReadPram(E2_LOAD_MAX_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_LOAD_MAX_ID, buffer,datalen);
                EepromPram_ReadPram(E2_LOAD_MAX_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_LOAD_MAX_ID);
                }
            }
            break;
       #endif
       case TERMINAL_PARAM_OWNER_PHONE:
            if(type)
            {
                len=Public_ReadDataFromFRAM(FRAM_CAR_OWNER_PHONE_ADDR, buffer,FRAM_CAR_OWNER_PHONE_LEN);
            }
            else
            {
                memset(buf,0,FRAM_CAR_OWNER_PHONE_LEN+1);
                memcpy(buf,buffer,datalen);
                Public_WriteDataToFRAM(FRAM_CAR_OWNER_PHONE_ADDR, buf,FRAM_CAR_OWNER_PHONE_LEN);                
            }
            break;
       case TERMINAL_PARAM_CAR_PROVINCE_ID:
            if(type)
            {
                len=EepromPram_ReadPram(E2_CAR_PROVINCE_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_CAR_PROVINCE_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_PROVINCE_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_PROVINCE_ID);
                }
            }
            break;
       case TERMINAL_PARAM_CAR_CITY_ID:
            if(type)
            {
                len=EepromPram_ReadPram(E2_CAR_CITY_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_CAR_CITY_ID, buffer,datalen);
                EepromPram_ReadPram(E2_CAR_CITY_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_CAR_CITY_ID);
                }
            }
            break;        
        case TERMINAL_PARAM_MAIN_IP://����������ַ,IP ������
            #if 1
            if(type)
            {
                len=EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MAIN_SERVER_IP_ID);
                }
            }
            break;
            #endif
        case TERMINAL_PARAM_MAIN_DOMAIN_NAME:
            if(type)
            {
                len=Public_ReadDataFromFRAM(FRAM_MAIN_DOMAIN_NAME_ADDR, buffer,FRAM_MAIN_DOMAIN_NAME_LEN);
            }
            else
            {
                memset(buf,0,FRAM_MAIN_DOMAIN_NAME_LEN+1);
                memcpy(buf,buffer,datalen);
                Public_WriteDataToFRAM(FRAM_MAIN_DOMAIN_NAME_ADDR, buf,FRAM_MAIN_DOMAIN_NAME_LEN);
                if(NaviLcd_CheckDataIsIPFormat(buffer,datalen))
                {
                    EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, buffer,datalen);
                    EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, buf);
                    if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                    {
                        EepromPram_UpdateVariable(E2_MAIN_SERVER_IP_ID);
                    }
                }                
            }
            break;        
        case TERMINAL_PARAM_BAK_IP://���ݷ�������ַ,IP ������
            #if 1
            if(type)
            {
                len=EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, buffer,datalen);
                EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_BACKUP_SERVER_IP_ID);
                }
            }
            break;
            #endif
        case TERMINAL_PARAM_BACKUP_DOMAIN_NAME:
            if(type)
            {
                len=Public_ReadDataFromFRAM(FRAM_BACKUP_DOMAIN_NAME_ADDR, buffer,FRAM_BACKUP_DOMAIN_NAME_LEN);
            }
            else
            {
                memset(buf,0,FRAM_BACKUP_DOMAIN_NAME_LEN+1);
                memcpy(buf,buffer,datalen);
                Public_WriteDataToFRAM(FRAM_BACKUP_DOMAIN_NAME_ADDR, buf,FRAM_BACKUP_DOMAIN_NAME_LEN);
                if(NaviLcd_CheckDataIsIPFormat(buffer,datalen))
                {
                    EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, buffer,datalen);
                    EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, buf);
                    if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                    {
                        EepromPram_UpdateVariable(E2_BACKUP_SERVER_IP_ID);
                    }
                }
            }
            break;
       case TERMINAL_PARAM_MANUFACTURE_ID:
            if(type)
            {
                len=EepromPram_ReadPram(E2_MANUFACTURE_ID, buffer);
            }
            else
            {
                EepromPram_WritePram(E2_MANUFACTURE_ID, buffer,datalen);
                EepromPram_ReadPram(E2_MANUFACTURE_ID, buf);
                if(strncmp((char*)buf,(char*)buffer,datalen)==0)
                {
                    EepromPram_UpdateVariable(E2_MANUFACTURE_ID);
                }
            }
            break;
            
    }
    return len;
}
/*******************************************************************************
** ��������: NaviLcd_DisposeSetTerminalParamter
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeSetTerminalParamter(void)
{
    unsigned char i,num,ID,len;
    unsigned short index;
    unsigned char data[40];
    index = 0;
    num = s_ucNaviLcdParseBuffer[index++]; 
    		
    for(i=0 ; i<num ; i++)
    {
        ID = s_ucNaviLcdParseBuffer[index++];
        len = s_ucNaviLcdParseBuffer[index++]; 
        if(index +len > s_stNaviLcdAttrib.recvDatalen)break;
        memcpy(data,&s_ucNaviLcdParseBuffer[index],len);
        index +=len;
        ///////////////////////////
        if(len)
        NaviLcd_QueryOrSaveTerminalParamter(ID,len,data,0);
    }
		
		if((7 == num)&&(1 == BBGNTestFlag))//dxl,2016.6.4
		{
		    Lock2_TrigOpenAccount();
		}	
}
/*******************************************************************************
** ��������: NaviLcd_DisposeQueryTerminalParamter
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeQueryTerminalParamter(void)
{
    unsigned char i,num,ID,datalen;
    unsigned short len;
    unsigned char data[1024];
    unsigned char buffer[30];
    
    num = s_ucNaviLcdParseBuffer[0];    
    len = 0;
    /////////////////
    data[len++] = num;
    ////////////////
    for(i=0 ; i<num ; i++)
    {
        ID = s_ucNaviLcdParseBuffer[i+1];
        datalen=NaviLcd_QueryOrSaveTerminalParamter(ID,0,buffer,1);
        data[len++]=ID;
        data[len++]=datalen;        
        memcpy(&data[len],buffer,datalen);
        len+=datalen;
    }
    /////////////////////
    NaviLcdSendCmdQueryParameter(data,len);
}
/*******************************************************************************
** ��������: NaviLcd_RecvKeyTimeOut
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_RecvKeyTimeOut(void)
{
    s_stNVKey.datalen = 0;
    s_stNaviLcdAttrib.newphone = 0;
}
/*******************************************************************************
** ��������: NaviLcd_CurrentPhoneVolUp
** ��������:  �����Ӽ�
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_CurrentPhoneVolUp(void)
{
    SetTelVolumeIncr();
}
/*******************************************************************************
** ��������: NaviLcd_CurrentPhoneVolDown
** ��������: ������С 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_CurrentPhoneVolDown(void)
{
    SetTelVolumeDecr();
}
/*******************************************************************************
** ��������: NaviLcd_CurrentPhoneVolUp
** ��������:  �����Ӽ�
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_CurrentTTSVolUp(void)
{
//    SetTtsVolumeIncr();    dxl,2015.9, 
}
/*******************************************************************************
** ��������: NaviLcd_CurrentPhoneVolDown
** ��������: ������С 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_CurrentTTSVolDown(void)
{
   // SetTtsVolumeDecr(); dxl,2015.9,
}
/*******************************************************************************
** ��������: NaviLcd_DisposeKeyValue
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_HangUpPhone(void)
{
    s_stNaviLcdAttrib.newphone = 0;
    communication_CallPhone("13800000000",PHONE_END);
}
/*******************************************************************************
** ��������: NaviLcd_DisposeKeyValue
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
unsigned char NaviLcd_DisposeKeyValue(unsigned char value)
{
    unsigned char result;
    result = 0;
    switch(value)
    {
        case NAVILCD_KEYVALUE_HOT_LINE://�������ߵ绰 
            s_stNVKey.datalen = EepromPram_ReadPram(E2_MONITOR_SERVER_PHONE_ID, s_stNVKey.Keybuff);
            s_stNVKey.Keybuff[s_stNVKey.datalen]='\0';
            if(s_stNVKey.datalen)
            result=communication_CallPhone(s_stNVKey.Keybuff,PHONE_NORMAL);
            if(result)result = 0;
            else result = 0x02;
            s_stNVKey.datalen = 0;
            break;
        case NAVILCD_KEYVALUE_ANSWER_CALL://�����绰
            result=communication_CallPhone("0",PHONE_TALK);
            if(result)
            {
                s_stNaviLcdAttrib.newphone = 2; 
                LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_KEY],PUBLICSECS(5),NaviLcd_RecvKeyTimeOut);
                result = 0;
            }
            else result = 0x02;
            break;
        case NAVILCD_KEYVALUE_HANG_UP://�Ҷ�
            NaviLcd_RecvKeyTimeOut();
            if(0==communication_CallPhone("13800000000",PHONE_END))
            {
                LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_KEY],PUBLICSECS(3),NaviLcd_HangUpPhone);
            }           
            break;
        case NAVILCD_KEYVALUE_HF://����            
            break;
        case NAVILCD_KEYVALUE_REDIAL://�ز�
            s_stNVKey.datalen = 0;
            result=NaviLcd_CheckDailPhone(s_stNVKey.Keybuff);
            break;
        case NAVILCD_KEYVALUE_DEL://ɾ��
            if(s_stNVKey.datalen)s_stNVKey.datalen--;
            LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_KEY],PUBLICSECS(3),NaviLcd_RecvKeyTimeOut);
            break;
        case NAVILCD_KEYVALUE_VOL_ADD://����+
            NaviLcd_CurrentPhoneVolUp();
            break;
        case NAVILCD_KEYVALUE_VOL_DEC://����-
            NaviLcd_CurrentPhoneVolDown();
            break;
        case NAVILCD_KEYVALUE_ASTERISK://*�� 
        case NAVILCD_KEYVALUE_POUND://#��
        case NAVILCD_KEYVALUE_KEY0://���ּ�:0~9
        case NAVILCD_KEYVALUE_KEY1:
        case NAVILCD_KEYVALUE_KEY2:
        case NAVILCD_KEYVALUE_KEY3:
        case NAVILCD_KEYVALUE_KEY4:
        case NAVILCD_KEYVALUE_KEY5:
        case NAVILCD_KEYVALUE_KEY6:
        case NAVILCD_KEYVALUE_KEY7:
        case NAVILCD_KEYVALUE_KEY8:
        case NAVILCD_KEYVALUE_KEY9:
            #ifdef EYE_MODEM
            Modem_Api_Phone_Vts(&value);
            #endif
            break;
    }
    return result;
}
/*******************************************************************************
** ��������: NaviLcdActiveSendKeyValue
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcdActiveSendKeyValue(unsigned char value)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_TERMINAL_KEY_VALUE,&value,1);
/*******************************************************************************
** ��������: NaviLcdSendSFExpressBrushCardData
** ��������: ����˳��ˢ���������ݵ������� 
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
}
/*******************************************************************************
** ��������: NaviLcd_DisposeQueryPhoneBooks
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeQueryPhoneBooks(unsigned char type)
{
    unsigned char buffer[1024];
    unsigned short datalen;
    //////////////////////////
    datalen = GetPhoneBook(type,buffer);
    NaviLcdSendCmdQueryPhoneBooks(buffer,datalen);
}
/*******************************************************************************
** ��������: NaviLcd_DisposeQueryPhoneBooks
** ��������:  
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void NaviLcd_DisposeAdjustVolume(void)
{
    unsigned char buffer[2]; 
    unsigned char type,flag,operate;
    unsigned char val =0;
    type=s_ucNaviLcdParseBuffer[0];//
    flag=s_ucNaviLcdParseBuffer[1];//
    operate=s_ucNaviLcdParseBuffer[2];//
    //////////////////////////
    if(0==type)//ͨ������
    {
        if(1==flag)//1:����
        {
            if(0==operate)// 0��������,1��������
            {
                NaviLcd_CurrentPhoneVolUp();
            }
            else
            if(1==operate)
            {
                NaviLcd_CurrentPhoneVolDown();
            }
        }
        //////��ǰ��������///////////////////
        val= GetTelVolume();
    }
    else
    if(1==type)//TTS����
    {
        if(1==flag)
        {
            if(0==operate)
            {
                NaviLcd_CurrentTTSVolUp();
            }
            else
            if(1==operate)
            {
                NaviLcd_CurrentTTSVolDown();
            }
        }
        //////////��ǰ��������///////////////////////////////////
        val= GetModTtsVolume(); 
    }
    /////////////////
    buffer[0]= 0;
    buffer[1]= val;
    
    PactSendCmdForNaviLcdEx(NAVILCD_CMD_ADJUST_VOLUME,buffer,2);
}
/*************************************************************
** ��������: NaviLcd_DisposeQustionResponse
** ��������: �������ʴ�����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeQustionResponse(void)
{
    ProtocolACK val_ack;
    unsigned char ID;
    u8 channel = CHANNEL_DATA_1;
    u16 SerialNum;
    //////////////////////////
    ID=s_ucNaviLcdParseBuffer[4];//
    
    channel = RadioProtocol_GetDownloadCmdChannel(0x8302, &SerialNum);
    /////////�������ʴ𰸵�ƽ̨///////////////////
    val_ack=RadioProtocol_QuestionAck(channel,ID);
    if(val_ack==ACK_OK)

    {
        return 0;
    }
    else
    {
        return 2;
    }
}
/*************************************************************
** ��������: NaviLcd_GetFixPhrase
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short NaviLcd_GetFixPhrase(unsigned char *buffer)
{
    unsigned char i,length;
    unsigned short index;
    ///////////////////////
    index = 0;    
    buffer[index++] = FIX_PHRASE_ITEMS;
    ////////////////////
    for(i = 0; i < FIX_PHRASE_ITEMS; i++)
    {
        ///////////////////////////////
        buffer[index++] = i+1;
        /////////////////////
        length =strlen((char *)c_apcFixPhrase[i]);
        buffer[index++] = length;
        memcpy(&buffer[index],c_apcFixPhrase[i],length);
        index +=length;
    }
    /////////////////
    return index;
}

/*************************************************************
** ��������: NaviLcd_DisposeQueryFixText
** ��������: ��ѯ�̶��ı���Ϣ����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeQueryFixText(unsigned char type)
{
    unsigned char buffer[1024];
    unsigned short datalen;
    datalen = 0;
    buffer[0] = type;//
    if(FIXTEXT_TYPE_PHRASE == type)//�̶�����
    {
        datalen=NaviLcd_GetFixPhrase(&buffer[1]);
        datalen++;
    }
    else
    if(FIXTEXT_TYPE_EVENT == type)////�¼�����
    {       
        datalen=GetEventReport(&buffer[1]);//joneming 2013.1.15
        datalen++;
    }
    else
    if(FIXTEXT_TYPE_INFO == type)//��Ϣ�㲥
    {
        datalen = GetInfoDemand(&buffer[1]);//joneming 2013.1.15
        datalen++;
    }
    ///////////////////////////////
    NaviLcdSendCmdQueryFixText(buffer,datalen);
    ////////////////////////
}

/*************************************************************
** ��������: NaviLcd_DisposeSendTickleInfo
** ��������: ���ͷ�����Ϣ����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeSendTickleInfo(void)
{
    unsigned char i;
    unsigned short len;
    unsigned char phone[30];
    ProtocolACK val_ack;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    /////////////////////////
    if(s_ucNaviLcdParseBuffer[0]=='#'&& s_ucNaviLcdParseBuffer[1]=='#')//������������
    {
        /////////���ͷ�����Ϣ��ƽ̨///////////////////
        val_ack=RadioProtocol_OriginalDataUpTrans(channel,4,(unsigned char *)&s_ucNaviLcdParseBuffer[2],s_stNaviLcdAttrib.recvDatalen-2);
        if(ACK_OK == val_ack)
        {
            return 0;
        }
    }
    else
    if(s_ucNaviLcdParseBuffer[0]!='#')//������������ 
    {
        /////////���ͷ�����Ϣ��ƽ̨///////////////////
        val_ack=RadioProtocol_OriginalDataUpTrans(channel,4,s_ucNaviLcdParseBuffer,s_stNaviLcdAttrib.recvDatalen);
        if(ACK_OK == val_ack)
        {
            return 0;
        }
    }
    else
    if(s_stNaviLcdAttrib.recvDatalen > 10)
    {
        for(i=1; i<s_stNaviLcdAttrib.recvDatalen;i++)
        {
            if(s_ucNaviLcdParseBuffer[i]=='#')
            {
                if(i<10||i>20)return 2;
                break;
            }
        }
        ///////////////////////////
        memcpy(phone,&s_ucNaviLcdParseBuffer[1],i-1);
        phone[i-1]='\0';
        i++;
        len=Communication_SndMsg2Svr(channel,&s_ucNaviLcdParseBuffer[i],s_stNaviLcdAttrib.recvDatalen-i,phone);
        if(len==s_stNaviLcdAttrib.recvDatalen-i)
        {
            return 0;
        }
    }
    
    return 2;
}

/*************************************************************
** ��������: NaviLcd_DisposeSendFixText
** ��������: ���͹̶��ı���Ϣ����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposeSendFixText(void)
{
    unsigned char buffer[50];
    unsigned char type,ID,status,len;
    ProtocolACK val_ack =  NOACK;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    ////////////////////////
    type = s_ucNaviLcdParseBuffer[0];
    ID = s_ucNaviLcdParseBuffer[1];
    status =s_ucNaviLcdParseBuffer[2];    
    if(FIXTEXT_TYPE_PHRASE == type)//�̶�����
    {
        if(ID)ID--;
        len=strlen(c_apcFixPhrase[ID]);   
        memcpy(buffer,c_apcFixPhrase[ID],len);
        val_ack=RadioProtocol_OriginalDataUpTrans(channel,4,buffer,len);
    }
    else
    if(FIXTEXT_TYPE_EVENT == type)////�¼�����
    {
        /////////�����¼����浽ƽ̨///////////////////
        val_ack=RadioProtocol_EvenReport(channel,ID);
        ///////////////////////////////
    }
    else
    if(FIXTEXT_TYPE_INFO == type)//��Ϣ�㲥
    {
        val_ack=RadioProtocol_InforamtionOrder(channel,ID,status);
        if(ACK_OK==val_ack)
        {
            //////////������Ϣ�㲥״̬//////////////
            InfoDemandChangeStatus(ID,status);
            return 0;
        }      
    }
    //////////////////////
    if(ACK_OK==val_ack)
    {
        return 0;
    }
    else
    {
        return 2;
    }
}
/*******************************************************************************
** ��������: NaviLcd_CheckDailPhone
** ��������: �жϲ����Ƿ�����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 0:���ųɹ�,0x02��ֹ����
** ȫ�ֱ���: ��
** ����ģ��: ���ź���
*******************************************************************************/
unsigned char NaviLcd_CheckDailPhone(unsigned char *phone)
{
    unsigned char name[24];
    unsigned char flag;    
    flag=Public_CheckPhoneInformation(phone,name);//0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����//joneming 2013.1.15
    if(1 == flag||0==flag)//���ڣ�������ֻ����
    {
        Public_PlayTTSVoiceStr("�ú����ֹ����!");//joneming 2013.1.15
        return 0x02;
    }
    else
    {
        /////////////////////////////
        strcpy((char *)s_stNVKey.Keybuff,(char *)phone);
        ///���ź���/1�ǳɹ�;0:ʧ��
        flag=communication_CallPhone(phone,PHONE_NORMAL);
        if(flag)return 0;
        else return 0xFF;
    } 
}
/*******************************************************************************
**  ��������  : NaviLcd_OriginalDataDownTrans
**  ��������  : 
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_OriginalDataDownTrans(u8 *pBuffer, u16 BufferLen)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_PLATFORM_TRANS_DOWN,pBuffer,BufferLen);
}
/*******************************************************************************
**  ��������  : NaviLcd_DisposeOriginalDataTrans
**  ��������  : 
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
unsigned char NaviLcd_DisposeOriginalDataTrans(u8 *pBuffer, u16 BufferLen)
{
    ProtocolACK val_ack;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    val_ack = RadioProtocol_OriginalDataUpTrans(channel,pBuffer[0],pBuffer+1,BufferLen-1);
    if(ACK_OK==val_ack)
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

/*************************************************************
** ��������: NaviLcd_DisponseLoadSensorTransToLcd
** ��������: Lcd͸�������ش�����ָ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisponseLcdTransToLoadSensor(void)
{
    #ifdef USE_SYS_CAR_LOAD
//    CarLoad_DisposeLcdTransToLoadSensor(s_ucNaviLcdParseBuffer,s_stNaviLcdAttrib.recvDatalen); dxl,2015.9,
    #endif
}
/*******************************************************************************
**  ��������  : NaviLcd_DisposeCmdResponse
**  ��������  : 
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_DisposeRecvDataResponse(void)
{
    static unsigned char sucSetPasswordflag=0;
    unsigned char result;
    unsigned short cmd;
    result = 0;
    ///////////////////////
    cmd = s_stNaviLcdAttrib.recvCmd;
    ///////////////////
    if(s_stNVTimer[NV_TIMER_STATUS].enabled != TIMER_ENABLE)
    {
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_STATUS],PUBLICSECS(2),NaviLcd_SendWorkStatus);
    }    
    switch(cmd)
    {
        case NAVILCD_CMD_QUERY:
            if(0==sucSetPasswordflag)
            {
                sucSetPasswordflag = 1;
                NaviLcd_SetTerminalPassWordToLcd();
            }
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
         case NAVILCD_CMD_RESET:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_SET_BAUDRATE:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_DISPLAY:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_CALI_TIME:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_SET_LCD_PARAM:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_HEART_BEAT:
            NaviLcdSendCmdHeartBeat();
            s_stNaviLcdComm.responseFlag = 0;
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_SYSTEM_UPDATE: 
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        ////////////����/��������/////////////////////////
        case NAVILCD_CMD_TERMINAL_CONTROL: 
            result=NaviLcd_DisposeTerminalControl();
            break;
        case NAVILCD_CMD_TERMINAL_SET_PARAM:
            NaviLcd_DisposeSetTerminalParamter();
            break;
        case NAVILCD_CMD_TERMINAL_QUERY_PARAM:
            NaviLcd_DisposeQueryTerminalParamter();
            s_stNaviLcdComm.responseFlag = 0;
            return;
            //break;
        case NAVILCD_CMD_QUERY_STATUS_ALARM:            
            NaviLcd_DisposeQueryStatusAlarm(); 
            s_stNaviLcdComm.responseFlag = 0;
            return;
        case NAVILCD_CMD_QUERY_WORK_STATUS:
            if(s_stNaviLcdAttrib.recvDatalen==0)
            NaviLcd_DisposeQueryWorkStatus();
            s_stNaviLcdComm.responseFlag = 0;
            return;
        case NAVILCD_CMD_MODIFY_PASSWORD:
            return;
        case NAVILCD_CMD_REGISTER_OUT_CITY:
            result=NaviLcd_DisposeOutCityRegister();
            break;
        case NAVILCD_CMD_EXPORT_U_DISK:
            NaviLcdSendCmdGeneralAck(s_stNaviLcdAttrib.recvCmd,0);
            NaviLcdDelayms(25);
            NaviLcd_DisposeExportDataToUsbDisk();
            return;
        case NAVILCD_CMD_CARLOAD_WEIGHT:
             NaviLcd_DisposeCarLoadWeight();
            return;
        case NAVILCD_CMD_SET_PJA_VALID:
            #ifdef __COMMPJA_H_
            CommPJA_PlayPromptStr();
            #endif
            return;
        case NAVILCD_CMD_PJA_RESULT:
            NaviLcd_DisposePJAResult();
            break;
        ////////////�绰/////////////////////////////
        case NAVILCD_CMD_DIAL_PHONE:            
            s_ucNaviLcdParseBuffer[s_stNaviLcdAttrib.recvDatalen]='\0';
            #ifdef USE_PHONE_BOOK
            result=PhoneBook_CheckPhoneCallResult(s_ucNaviLcdParseBuffer,NULL);
            if(result)
            {
                result= 0x02;
            }
            #else
            result=NaviLcd_CheckDailPhone(s_ucNaviLcdParseBuffer);            
            #endif
            if(0 == result)
            {
                strcpy((char *)s_stNVKey.Keybuff,(char *)s_ucNaviLcdParseBuffer);
                s_stNaviLcdAttrib.newphone = 1;
                LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TEST],PUBLICSECS(70),NaviLcd_DisposeHangUpPhoneStatus);
            }
            break;
        case NAVILCD_CMD_NEW_CALL_PHONE:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_LCD_KEY_VALUE:
            result=NaviLcd_DisposeKeyValue(s_ucNaviLcdParseBuffer[0]);
            break;
        case NAVILCD_CMD_TERMINAL_KEY_VALUE:            
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
        case NAVILCD_CMD_QUERY_PHONE_BOOK:
            NaviLcd_DisposeQueryPhoneBooks(s_ucNaviLcdParseBuffer[0]);
            s_stNaviLcdComm.responseFlag = 0;
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
        case NAVILCD_CMD_ADJUST_VOLUME:
            NaviLcd_DisposeAdjustVolume();
            s_stNaviLcdComm.responseFlag = 0;
            return;
        //////////////��Ϣ����//////////////////////
        case NAVILCD_CMD_TEXT_DISPLAY:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_QUESTION_DISPLAY:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_QUESTION_RESPONSE: 
            result=NaviLcd_DisposeQustionResponse();
            break;
        case NAVILCD_CMD_QUERY_FIX_TEXT:
            NaviLcd_DisposeQueryFixText(s_ucNaviLcdParseBuffer[0]);   
            s_stNaviLcdComm.responseFlag = 0;
            return;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_SEND_FIX_TEXT: 
            result = NaviLcd_DisposeSendFixText();
            break;
        case NAVILCD_CMD_SEND_TICKLE_INFO:
            result = NaviLcd_DisposeSendTickleInfo();
            break;
        case NAVILCD_CMD_INFO_SERVICE:
             return ;//����Ҫʹ��ͨ��Ӧ��ظ�
        //////////////���ٷ���//////////////////////
        case NAVILCD_CMD_CALL_TAXI_NEW:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_CALL_TAXI_VIE://��������
            result = NaviLcd_DisposeCallTaxiVie();
            break;
        case NAVILCD_CMD_CALL_TAXI_DETAIL:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        case NAVILCD_CMD_CALL_TAXI_FINISH:
            result = NaviLcd_DisposeCallTaxiFinish();
            break;
        case NAVILCD_CMD_CALL_TAXI_CANCEL_TERMINAL:
            result = NaviLcd_DisposeTerminalCancelCallTaxi();
            break;
        case NAVILCD_CMD_CALL_TAXI_CANCEL_PLATFORM:
            return ;//����Ҫʹ��ͨ��Ӧ��ظ�
            //break;
        /////////////��ʻԱ��Ϣ///////////////////////
        case NAVILCD_CMD_DRIVER_SIGN:
            NaviLcd_DisposeDriverSign();
            s_stNaviLcdComm.responseFlag = 0;
            return;
        case NAVILCD_CMD_DRIVER_ADD:
            NaviLcd_DisposeDriverInfoAdd();
            return;
        case NAVILCD_CMD_DRIVER_RECORD:
            NaviLcd_DisposeDriverRecord();
            s_stNaviLcdComm.responseFlag = 0;
            return;
        case NAVILCD_CMD_DRIVER_REPORT:
            break;
        case NAVILCD_CMD_DRIVER_QUERY_ALL:
            NaviLcd_DisposeQueryDriverInfo(); 
            s_stNaviLcdComm.responseFlag = 0;
            return;
        case NAVILCD_CMD_DRIVER_ADD_RESULT:            
            return;
        case NAVILCD_CMD_DRIVER_MODIFY://
            if(NaviLcdDriverLoginFlag())
            {
                //result = DriverInfoModifyDriverCodeAndPassword(s_stDriverLog.recordNumber,&s_ucNaviLcdParseBuffer[0],&s_ucNaviLcdParseBuffer[6]);               
            }
            else
            {
                Public_PlayTTSVoiceStr("���ȵ�¼���޸�");
                result = 0x03;
            }
            break;
        case NAVILCD_CMD_DRIVER_DEL_ALL://
            DriverInfoDeleteAll();
            break;
        case NAVILCD_CMD_DRIVER_BRUSH_CARD: 
            NaviLcdSendCmdGeneralAck(s_stNaviLcdAttrib.recvCmd,0);
            NaviLcdDelayms(20);
            NaviLcd_DisposeBrushCard();
            return;
        case NAVILCD_CMD_WRITE_IC_CARD:
            NaviLcd_DisposeWriteICcardResult(s_ucNaviLcdParseBuffer[0]);
            return;
        case NAVILCD_CMD_READ_IC_CARD:
            NaviLcd_DisposeReadICcardData();
            return;
        case NAVILCD_CMD_CURRENT_DRIVER_INFO:
            return;
        ///////////////��Ϣ͸��///////////////////////////////
        case NAVILCD_CMD_PLATFORM_TRANS_DOWN:
            return;
        case NAVILCD_CMD_LCD_TRANS_UP:
            result=NaviLcd_DisposeOriginalDataTrans(s_ucNaviLcdParseBuffer,s_stNaviLcdAttrib.recvDatalen);
            break;
        case NAVILCD_CMD_TERMINAL_TRANS_TO_ICCARD://�ն�����͸����ˢ��ģ��
            return;
         case NAVILCD_CMD_ICCARD_TRANS_TO_TERMINAL://ˢ��ģ������͸�����ն�����
            NaviLcd_DisponseICCardTransDataToTerminal(s_ucNaviLcdParseBuffer,s_stNaviLcdAttrib.recvDatalen);
            break;
        case NAVILCD_CMD_TERMINAL_TRANS_TO_CTRL_UNIT://�ն�����͸����ˢ��ģ��
            return;
         case NAVILCD_CMD_CTRL_UNIT_TRANS_TO_TERMINAL://ˢ��ģ������͸�����ն�����
            NaviLcd_DisponseCtrlUnitTransDataToTerminal(s_ucNaviLcdParseBuffer,s_stNaviLcdAttrib.recvDatalen);
            break;
        case NAVILCD_CMD_LCD_TRANS_TO_LOAD_SENSOR://LCD͸�������ش�����ָ��
            NaviLcd_DisponseLcdTransToLoadSensor();
            break;
        case NAVILCD_CMD_LOAD_SENSOR_TRANS_TO_LCD://���ش�������͸��LCDָ��
            return;
        ///////////////����/////////////////////
        case NAVILCD_CMD_NAVIGATION:
            return;
        case NAVILCD_CMD_ONE_KEY_NAVIGATION:
            NaviLcd_DisponseOneKeyNavigation(s_ucNaviLcdParseBuffer,s_stNaviLcdAttrib.recvDatalen);
            break;
        case NAVILCD_CMD_GNSS_CHECK_UP:
            NaviLcd_DisposeGNSSCheckUp(s_ucNaviLcdParseBuffer[0]);
            break;
        default:
            break;
    }
    NaviLcdSendCmdGeneralAck(cmd,result);
}
/*************************************************************OK
** ��������: NaviLcdSaveDriverLogin
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSaveDriverLogin(unsigned char recordnumber) 
{
    s_stDriverLog.statusFlag = NV_DRIVERLOG_STATUS_LOGIN;
    s_stDriverLog.recordNumber =recordnumber;
}
/*************************************************************OK
** ��������: NaviLcdSaveDriverLogin
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcdSaveDriverLogout(void) 
{
    s_stDriverLog.statusFlag = NV_DRIVERLOG_STATUS_LOGOUT;
    s_stDriverLog.recordNumber = 0;
}
/*************************************************************OK
** ��������: NaviLcdDriverLoginFlag
** ��������: ��ʻԱ��¼��־
** ��ڲ���: 			 
** ���ڲ���: Ϊ1�ѵ�¼,Ϊ0δ��¼
** ���ز���: :
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcdDriverLoginFlag(void)
{
    //return (s_stDriverLog.statusFlag == NV_DRIVERLOG_STATUS_LOGIN)?1:0;
//    return TiredDrive_GetLoginFlag(); dxl,2015.9,
	return 0;
}
/*************************************************************OK
** ��������: GetCurrentDriverLoginRecordNumber
** ��������: ȡ�õ�ǰ��ʻԱ��¼��
** ��ڲ���: 			 
** ���ڲ���: 0δ��¼����ʱ��ʻԱ,δ0��¼��
** ���ز���: :
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetCurrentDriverLoginRecordNumber(void)
{
    if(NaviLcdDriverLoginFlag())
    {
        return s_stDriverLog.recordNumber;
    }
    ////////////////////////
    return 0;
}
/*************************************************************OK
** ��������: CheckTiredDriverAutoLog
** ��������: ����ʻԱ�Ƿ��¼���ѵ�¼����ʹ���ϴε�
** ��ڲ���: 			 
** ���ڲ���: 
** ���ز���: :
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void CheckTiredDriverAutoLog(void)
{
    unsigned char drivercode[8] = {0};
    unsigned char num;
    unsigned long driverID;
    EepromPram_ReadPram(E2_CURRENT_DRIVER_ID, drivercode);
    if(drivercode[0] == 0xFF && drivercode[1] == 0xFF && drivercode[2] == 0xFF)//��������˵�¼�Ļ�ÿ���ϵ綼Ĭ��Ϊ0
    {
        NaviLcd_DriverLogout();
    }
    else 
    {
        driverID = 0;
        driverID |= drivercode[0]<<16;
        driverID |= drivercode[1]<<8;
        driverID |= drivercode[2];
        sprintf((char *)drivercode,"%06d",driverID);
        //////////////////////////
        num=GetDriverInfoRecordNumber(drivercode);
        SaveCurrentDriverParameter(num);
        ////////////////////////////
//        TiredDrive_Login(); dxl,2015.9,
        /////////////////////
        NaviLcdSaveDriverLogin(num);
    }
}
/*************************************************************OK
** ��������: NaviLcd_RadioProtocolQuestionDownload
** ��������: �����·�
** ��ڲ���: 			 
** ���ڲ���: 
** ���ز���: :
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_RadioProtocolQuestionDownload(u8 *pBuffer, u16 BufferLen)
{
    #ifndef USE_NAVILCD_TTS
    u8      QuestionFlag;//��־
    u8      QuestionLen;//�������ݳ���
    u8      AnswerID;//��ID
    u16      AnswerLen;//�𰸳���
    u8      *p;
    u16     length;
    s16     TotalLength;
    u8      HighByte;
    u8      LowByte;
    u8      Buffer[20];
    u8      PramLen;
    u8	flag = 0;//�����־    
    TotalLength = BufferLen;

    p = pBuffer;
    TotalLength -= 2;
    QuestionFlag = *p++;//�����·���־

    if(0 == QuestionFlag)
    {

    }
    //�������ݳ���
    QuestionLen = *p++;
    length = 0;

    memcpy(RadioShareBuffer+length,"���⣺",6);
    length += 6;
    memcpy(RadioShareBuffer+length,p,QuestionLen);
    length += QuestionLen;
    p += QuestionLen;
    TotalLength -= QuestionLen;
    memcpy(RadioShareBuffer+length,"�𰸣�",6);
    length += 6;
    while(TotalLength > 0)
    {
        //��ID
        AnswerID = *p++;
        TotalLength--;

        sprintf((char *)Buffer," %d��",AnswerID);
        PramLen = strlen((const char *)Buffer);
        if((PramLen+length) >= RADIO_PROTOCOL_BUFFER_SIZE)
        {
            flag = 1;
            break;
        }
        memcpy(RadioShareBuffer+length,Buffer,PramLen);
        length += PramLen;
        //�𰸳���
        AnswerLen = 0;
        HighByte = *p++;
        LowByte = *p++;
        AnswerLen = (HighByte << 8)|LowByte;
        TotalLength -= 2;
        //������
        if((AnswerLen+length) >= RADIO_PROTOCOL_BUFFER_SIZE)
        {
            flag = 1;
            break;
        }
        memcpy(RadioShareBuffer+length,p,AnswerLen);
        length += AnswerLen;
        TotalLength -= AnswerLen;
        p += AnswerLen;
    }
    if(0 == flag)
    {
        RadioShareBuffer[length] ='\0';
        Public_PlayTTSVoiceStr(RadioShareBuffer);
        NaviLcdSendCmdQuestionDisplay(1,pBuffer+1,BufferLen-1);
        
    }
    #else
    NaviLcdSendCmdQuestionDisplay(0,pBuffer+1,BufferLen-1);
    #endif
}
/*******************************************************************************
**  ��������  : NaviLcd_OpenBrushCard
**  ��������  : ����ˢ������  
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_OpenBrushCard(void)
{
  
}
/*******************************************************************************
**  ��������  : NaviLcd_CloseBrushCard
**  ��������  : �ر�ˢ������  
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_CloseBrushCard(void)
{
  
}
/*******************************************************************************
**  ��������  : NaviLcd_DisposeBrushCardDataForDriverInfo
**  ��������  : ����ˢ������
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void NaviLcd_DisposeBrushCardDataForDriverInfo(unsigned char *data,unsigned short datalen)
{
    unsigned char len;
    unsigned char buffer[30];
    if(NV_CMD_FLAG_ADD==s_stDriverLog.cmdFlag)//ˢ�����/�޸ļ�ʻԱ����
    {
        #if 0
        if(0xAA != s_stDriverLog.recordindex)
        {
            s_stDriverLog.resutltindex = s_stDriverLog.recordindex;
            s_stDriverLog.result = DriverInfoAddOrModify(&s_stDriverLog.resutltindex,s_stDriverLog.drivercode,s_stDriverLog.password,data,datalen);
        }
        else
        {
            s_stDriverLog.resutltindex = 5;
            s_stDriverLog.result = 1; 
            Public_PlayTTSVoiceStr("�������빤������"); 
        }
        NaviLcd_DisposeDriverInfoAddInfo();
        #endif
    }
    else
    if(NV_CMD_FLAG_LOGIN==s_stDriverLog.cmdFlag)//ˢ��ǩ��
    {
        len = 0;
        buffer[len++] = 0;//ǩ��/ǩ�˱�־
        buffer[len++] = 0;//�ɹ�
//        ICCard_DisposeExternalDriverSignIn(data,datalen); dxl,2015.9,
  //      len += ICCard_GetExternalDriverInfoForSignIn(&buffer[len]); dxl,2015.9,
        if(len<3)
        {
            buffer[1] = 1;//ʧ��
        }
        NaviLcdSendDriverSignResult(buffer,len);
    }
    else
    if(NV_CMD_FLAG_LOGOUT==s_stDriverLog.cmdFlag)//ˢ��ǩ��
    {
        buffer[0]= 1;//ǩ��/ǩ�˱�־
        if(NaviLcdDriverLoginFlag())
        {
//            ICCard_DisposeExternalCardDriverSignOut();    dxl,2015.9,        
        }
        //////////////////
        buffer[1]= 0;//�ɹ�
        NaviLcdSendDriverSignResult(buffer,2);
    }
    else
    if(NV_CMD_FLAG_BUSINESS==s_stDriverLog.cmdFlag)//ҵ���ϱ�
    {
        //RadioProtocol_DriverInformationReport(data,len);//
    }
}

/*************************************************************
** ��������: NaviLcd_DisposePhoneStatus
** ��������: ����������绰״̬����
** ��ڲ���: phone:�绰����,status:�绰״̬��0��Ϊ�Ҷ�;1:��ͨ��2:������
** ���ڲ���: ��
** ���ز���: ����ǵ绰����״̬Ϊ������ʱ,����ֵ:0:ֱ�Ӿܽ�����;1:���ڵȴ��û���������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char NaviLcd_DisposePhoneStatus(unsigned char *phone,unsigned char status)
{
    unsigned char result;
    result =0;
    switch(status)
    {
        case 0://�һ�
            s_stNaviLcdAttrib.newphone = 0;
            NaviLcdActiveSendKeyValue(NAVILCD_KEYVALUE_HANG_UP);            
            break;
        case 1://ͨ��
            s_stNaviLcdAttrib.newphone = 2;
            LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TEST],PUBLICSECS(5),NaviLcd_RecvKeyTimeOut);
            NaviLcdActiveSendKeyValue(NAVILCD_KEYVALUE_ANSWER_CALL);
            break;
        case 2://������
            if(!s_stNaviLcdAttrib.newphone)
            result=NaviLcdSendCmdNewCallPhone(phone);
            break;
    }
    return result;
}
/*************************************************************
** ��������: NaviLcd_DisposeHangUpPhoneStatus
** ��������: ������绰����ʱ����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeHangUpPhoneStatus(void)
{
    if(1== s_stNaviLcdAttrib.newphone)
    NaviLcd_HangUpPhone();
}
/*************************************************************
** ��������: NaviLcd_TimerTask
** ��������: �ڲ���ʱ��������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState NaviLcd_TimerTask(void)
{
    /////////////////////
    NaviLcdAutoToDisposeRecvData();//�����ڻ�������
    ////////////////////////////////////
    LZM_PublicTimerHandler(s_stNVTimer,NV_TIMERS_MAX);
    ///////////////////////////////////
    return ENABLE;
}
/*************************************************************
** ��������: NaviLcd_AccStatusOff
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_AccStatusOff(void)
{
    unsigned char i;
    for(i = NV_TIMER_RESEND; i < NV_TIMERS_MAX; i++)
    {
        LZM_PublicKillTimer(&s_stNVTimer[i]);
    }
    NaviLcdPowerOff();
}
/*************************************************************
** ��������: NaviLcd_AccStatusOn
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_AccStatusOn(void)
{
    NaviLcdCommParamInit();
    NaviLcdPowerOn();
    LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_TEST],PUBLICSECS(15),NaviLcdSendCmdQueryStatus);    
}
/*************************************************************
** ��������: NaviLcd_AccStatusOn
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_AccStatusTurnPromptDriver(void)
{
    #if 0
    unsigned char ACC;
    if(1 != s_stNaviLcdAttrib.VoicePrompt)return;
    if(0==s_stNaviLcdAttrib.AccTurnPrompt)return;
    ////////////////////////////////
    s_stNaviLcdAttrib.AccTurnPrompt =0;
    /////////////////////////////////
    if(0== s_stNaviLcdAttrib.onlinestaus)return;
    //////////////////////////////////////
    ACC = Io_ReadStatusBit(STATUS_BIT_ACC);    
    if(0==ACC)//�°�ǩ��
    {
        if(NaviLcdDriverLoginFlag())
        {
            #ifdef TexiService_TaximeterUseMode
            if(1 == TexiService_TaximeterUseMode())
            {
                Public_PlayTTSVoiceStr("���Ȱ��Ƽ���IC��,��ˢ˾�����°�ǩ��"); 
            }
            else
            #endif
            {
                Public_PlayTTSVoiceStr("��ˢ˾�����°�ǩ��");
            }
        }
    }
    else//�ϰ�ǩ��
    {
        if(!NaviLcdDriverLoginFlag())
        {
            #ifdef TexiService_TaximeterUseMode
            if(1==TexiService_TaximeterUseMode())
            {
                Public_PlayTTSVoiceStr("����ˢ˾����,�ٰ��Ƽ���IC���ϰ�ǩ��");
            }
            else
            #endif
            {
                Public_PlayTTSVoiceStr("��ˢ˾�����ϰ�ǩ��");
            }
        }
    }
    #endif
}
/*************************************************************
** ��������: NaviLcd_CheckACCStatus
** ��������: ���ACC��ǰ״̬
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_CheckACCStatus(void)
{
    unsigned char ACC;    
    ACC=Io_ReadStatusBit(STATUS_BIT_ACC);
    if(ACC)//acc��
    {
        if(s_stNaviLcdAttrib.LcdPowerOn != 1)
        {
            s_stNaviLcdAttrib.LcdPowerOn =1;
            NaviLcd_AccStatusOn();
        }
        s_stNaviLcdAttrib.AccOffCnt = 0;
    }
    else//acc��
    {
        ////////////////////////
        if(s_stNaviLcdAttrib.LcdPowerOn != 0)
        {
            if(s_stNaviLcdAttrib.AccOffCnt<65)//65*4
            {
                s_stNaviLcdAttrib.AccOffCnt++;
            }
            else
            {
                s_stNaviLcdAttrib.LcdPowerOn =0;
                s_stNaviLcdAttrib.AccOffCnt = 0;
                NaviLcd_AccStatusOff();
            }
        }
    }
}

/*************************************************************
** ��������: NaviLcd_CheckACCStatus
** ��������: ���ACC��ǰ״̬
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_StartCheckACCStatus(void)
{
    NaviLcd_CheckACCStatus();
    LZM_PublicSetCycTimer(&s_stNVTimer[NV_TIMER_CHECK_ACC],PUBLICSECS(4),NaviLcd_CheckACCStatus);
}
/*************************************************************
** ��������: NaviLcd_ParameterInitialize
** ��������: ������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
    void NaviLcd_ParameterInitialize(void)
    {  
        LZM_PublicKillTimerAll(s_stNVTimer,NV_TIMERS_MAX);
        memset(&s_stDriverLog,0,sizeof(STDRIVERLOG));
        s_stDriverLog.statusFlag = NV_DRIVERLOG_STATUS_EMPTY;
        s_stDriverLog.cmdFlag =NV_CMD_FLAG_EMPTY; 
        s_stNaviLcdAttrib.newphone = 0;
        s_stNaviLcdAttrib.logout =0;    
        s_stDriverLog.recordindex = 0xAA;
        s_stNaviLcdAttrib.LcdPowerOn = 0;
        s_stNaviLcdAttrib.AccOffCnt = 0;
        s_stNaviLcdAttrib.onlinestaus = 0;
    #ifdef E2_UNLOGIN_TTS_PLAY_ID
        if(!EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, &s_stNaviLcdAttrib.VoicePrompt))
    #endif
        {
            s_stNaviLcdAttrib.VoicePrompt = 0;
        }
        NaviLcdCommParamInit();    
        NaviLcdPowerIoInit();
        NaviLcdPowerOff();
        
        /////////////////////////
        //���¼������������жϣ���û��ʹ����ӵ���������ֱ�ӷ���,dxl,2014.12.26
       
        if(0 == ReadPeripheral2TypeBit(15))//����2���͵�bit15Ϊ������,1:����;0:û��
        {
         
                return ;
        }
        SetTimerTask(TIME_NVLCD, LZM_TIME_BASE);
        /////// dxl,2014.12.26 end//////////
        NaviLcd_CheckACCStatus();   
        //////////////////////
        LZM_PublicSetOnceTimer(&s_stNVTimer[NV_TIMER_CHECK_ACC],PUBLICSECS(1),NaviLcd_StartCheckACCStatus);
    }

/*************************************************************
** ��������: NaviLcd_WriteDriverInfoToICcard
** ��������: ����ؼ�ʻԱ��Ϣд��IC��
** ��ڲ���: recordNumber:��ʻԱ�洢���(��1��ʼ)
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_WriteDriverInfoToICcard(unsigned char recordNumber)
{
    unsigned char buffer[200];
    unsigned char datalen;
    memset(buffer,0,200);
    datalen=GetDriverInfo(recordNumber,buffer,0);
    if(0==datalen)return ; //����Ӧ��¼
    NaviLcdSendDataToWriteICcard(0,0,buffer,datalen);
}
/*************************************************************
** ��������: NaviLcd_DebugToWriteICcard
** ��������:
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DebugToWriteICcard(void)
{
    NaviLcd_WriteDriverInfoToICcard(1);
}
/*************************************************************
** ��������: NaviLcd_SetTerminalPassWordToLcd
** ��������:
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_SetTerminalPassWordToLcd(void)
{
    unsigned char buffer[5];
    if(!EepromPram_ReadPram(E2_PASSWORD_ID,buffer))
    {
        Public_ConvertLongToBuffer(701000,buffer);
        EepromPram_WritePram(E2_PASSWORD_ID,buffer,E2_PASSWORD_ID_LEN);
    }
    ////////////////////////////////////
    NaviLcdSendCmdModifyPassWord(buffer);
}
/*************************************************************
** ��������: NaviLcd_SendCmdTerminalTransDataToICCard
** ��������: �ն�����͸����ˢ��ģ��
** ��ڲ���: data��Ҫ���͵������׵�ַ,datalen:��Ҫ���͵����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_SendCmdTerminalTransDataToICCard(unsigned char *data,unsigned short datalen)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_TERMINAL_TRANS_TO_ICCARD,data,datalen);
}
/*************************************************************
** ��������: NaviLcd_DisponseICCardTransDataToTerminal
** ��������: ˢ��ģ��͸�����ն�����
** ��ڲ���: data��Ҫ���͵������׵�ַ,datalen:��Ҫ���͵����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisponseICCardTransDataToTerminal(unsigned char *data,unsigned short datalen)
{
    ///////////////////
}
/*************************************************************
** ��������: NaviLcd_SendCmdDataToCtrlUnit
** ��������: �ն�͸�������ư�ָ��
** ��ڲ���: data��Ҫ���͵������׵�ַ,datalen:��Ҫ���͵����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_SendCmdDataToCtrlUnit(unsigned char *data,unsigned short datalen)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_TERMINAL_TRANS_TO_CTRL_UNIT,data,datalen);
}
/*************************************************************
** ��������: NaviLcd_SendCmdTerminalTransDataToCtrlUnit
** ��������: �ն�͸�������ư�ָ��
** ��ڲ���: ID����ID,operate:����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_SendCmdTerminalTransDataToCtrlUnit(unsigned char ID,unsigned char operate)
{
    unsigned char len;
    unsigned char data[4];
    len =0;
    data[len++] = ID;
    data[len++] = operate;
    data[len++] = 0;//����
    NaviLcd_SendCmdDataToCtrlUnit(data,len);
}
/*************************************************************
** ��������: NaviLcd_DisponseCtrlUnitTransDataToTerminal
** ��������: ���ư�͸�����ն�ָ��
** ��ڲ���: data��Ҫ���͵������׵�ַ,datalen:��Ҫ���͵����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisponseCtrlUnitTransDataToTerminal(unsigned char *data,unsigned short datalen)
{
    ///////////////////
}
/*************************************************************
** ��������: NaviLcd_DisponseLoadSensorTransToLcd
** ��������: ���ش�����͸����Lcdָ��
** ��ڲ���: data��Ҫ���͵������׵�ַ,datalen:��Ҫ���͵����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisponseLoadSensorTransToLcd(unsigned char *data,unsigned short datalen)
{
    PactSendCmdForNaviLcd(NAVILCD_CMD_LOAD_SENSOR_TRANS_TO_LCD,data,datalen);
}

/*************************************************************
** ��������: NaviLcd_DisponseCtrlUnitTransDataToTerminal
** ��������: һ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisponseOneKeyNavigation(unsigned char *data,unsigned short datalen)
{
    ///////////////////
}

void NaviLcd_SetCurrentPJAValid(void)
{
	 #ifdef __COMMPJA_H_
    unsigned char data[2]={0};
    if(!NaviLcd_GetCurOnlineStatus())return;
    PactSendCmdForNaviLcd(NAVILCD_CMD_SET_PJA_VALID,data,1);
	#endif
}
void NaviLcd_DisposePJAResult(void)
{
	 #ifdef __COMMPJA_H_
    unsigned char result;
    result=s_ucNaviLcdParseBuffer[1]; 
    CommPJA_SetCurrentPjaResult(result);    
	#endif
}
//////////////////////////////
/*************************************************************
** ��������: NaviLcd_DisposeGNSSCheckUp
** ��������:�������ͼ쵼����ָ��
** ��ڲ���: cmd����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void NaviLcd_DisposeGNSSCheckUp(unsigned char cmd)
{
    u8 Buffer[100];
    u16 length;
     u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    switch(cmd)
    {
        case NV_GNSS_CHECK_TALK_PHONE:      //����绰 
            ///////��������Ҫ����ĵ绰//////////
            communication_CallPhone("13800138000",PHONE_NORMAL);
            break;
        case NV_GNSS_CHECK_ANSWER_PHONE:    //�����绰
            communication_CallPhone("0",PHONE_TALK);
            break;
        case NV_GNSS_CHECK_HANG_UP_PHONE:    //�Ҷϵ绰
            communication_CallPhone("13800000000",PHONE_END);
            break;
        case NV_GNSS_CHECK_START_RECORD:     //��ʼ¼��      
            break;
        case NV_GNSS_CHECK_STOP_RECORD:      //ֹͣ¼��    
            break;
        case NV_GNSS_CHECK_FIX_PHRASE:       //���͹̶�����
            break;
        case NV_GNSS_CHECK_RTU_REGISTER:     //�ն�ע��
            RadioProtocol_TerminalRegister(channel);  
            break;
        case NV_GNSS_CHECK_RTU_QUALIFY:      //�ն˼�Ȩ
            RadioProtocol_TerminalQualify(CHANNEL_DATA_1);//ֻ��һ��һ��ͨ��������Ϊͨ����һ����Ȩ��Ҳ��һ��      
            RadioProtocol_TerminalQualify(CHANNEL_DATA_2);
            break;
        case NV_GNSS_CHECK_POSITION_INFO:    //λ����Ϣ�㱨
            Public_GetCurBasicPositionInfo(Buffer);
            length = 28;
            RadioProtocol_PostionInformationReport(channel,Buffer,length); 
            break;
        case NV_GNSS_CHECK_EVENT_REPORT:     //�¼����� 
            RadioProtocol_EvenReport(channel,0x01);
            break;
        case NV_GNSS_CHECK_INFO_DEMAND:      //��Ϣ�㲥   
            RadioProtocol_InforamtionOrder(channel,1, 1);
            break;
        case NV_GNSS_CHECK_CANCEL_IOD:       //ȡ����Ϣ�㲥
            RadioProtocol_InforamtionOrder(channel,1, 0);
            break;
        case NV_GNSS_CHECK_E_WAYBILL:        //�����˵��ϱ� 
            memcpy(Buffer+4,"�����˵�",8);
            Buffer[0] = 0;
            Buffer[1] = 0;
            Buffer[2] = 0;
            Buffer[3] = 8;
            RadioProtocol_ElectricReceiptReport(channel,Buffer, 12);
            break;
        case NV_GNSS_CHECK_DRIVER_INFO:      //��ʻԱ�����Ϣ      
            break;
        case NV_GNSS_CHECK_MULIT_EVENT:      //�Ϸ���ý���¼�
            RadioProtocol_MultiMediaEvenReport(channel,0x01, 0, 0, 1, 1);
            break;
        case NV_GNSS_CHECK_DTU_DATA:         //��������͸�� 
            RadioProtocol_OriginalDataUpTrans(channel,1, Buffer, 10);
            break;
        case NV_GNSS_CHECK_RESERVE1:         //��������1  
            break;
        case NV_GNSS_CHECK_RESERVE2:         //��������2
            break;
        case NV_GNSS_CHECK_RESERVE3:         //��������3  
            break;        
    }
}
#endif
/******************************************************************************
**                            End Of File
******************************************************************************/
