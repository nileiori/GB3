/*******************************************************************************
 * File Name:           TaskMenu.h 
 * Function Describe:    
 * Relate Module:       
 * Writer:              joneming
 * Date:                2012-01-5
 * Rewriter:
 * Date:
 *******************************************************************************/
/****************************************************/
/*                                                  */
/* Included files                                   */
/*                                                  */
/****************************************************/ 
#include "include.h"
#include<time.h>
#include "VDR.h"
#include "DeepPlow.h"
#include "turnspeed.h"
#include "Lcd.h"
/////////////////////////////////////////////////
extern u8  BBGNTestFlag;
static void (*TaskTo)(void);
static void (*s_fptrDoit)(void);
static void (*s_fptrCancel)(void);
LZM_RET (*PasswordPreTaskFunction)(LZM_MESSAGE *Msg);//��ȷ��Ӧ������������֮ǰ������
/*
static LZM_RET TaskMenuGyro(LZM_MESSAGE *Msg);
static LZM_RET TaskMenuGyroAcceleration(LZM_MESSAGE *Msg);
static LZM_RET TaskMenuGyroDeceleration(LZM_MESSAGE *Msg);
static LZM_RET TaskMenuGyroSharpTurn(LZM_MESSAGE *Msg);
static void TaskMenuGyroCalibration(void);
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ST_ESPIN s_stESpin[4];
ST_SPIN s_stSpin[4];

#define AUTORUN_MAIN_TIME 900//3min//60*5
#define LCD_TIMEOUT 300//1min=60*5
#define REFLASH_TIME 6//0.3
#define REINIT_LCD_TIME 1200//s 20����


static u8 MenuSwitchFlg =0;

///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    unsigned long SysPassword;
    unsigned long InPassword;
    unsigned long timeVal;
    unsigned short LcdTimeOutCnt;
    unsigned short AutoRunCnt;
    unsigned short textlen;
    unsigned char FlashBacklightFlag;//
    unsigned char BacklightFlag;
    unsigned char ControlMaxId;
    unsigned char ControlId;
    unsigned char index;
    unsigned char select;
    unsigned char step;
    unsigned char answerNum;    
    unsigned char status[4];    //
    unsigned char ID[4];        //
    char ShowBuffer[4][17];    
}STTASKMENU_ATTRIB;

STTASKMENU_ATTRIB s_stTaskAttrib;

typedef enum 
{
    MENUMAIN_DRIVE_RECORD,
    MENUMAIN_INFO_SERVER,
    #ifdef USE_PHONE_BOOK
    MENUMAIN_PHONE_SERVER,
    #endif
    MENUMAIN_DRIVE_MILE,
    MENUMAIN_PRODUCT_INFO,
    MENUMAIN_SIM_NUM,  
    MENUMAIN_TTS,
    MENUMAIN_SYSTEM,
    MENUMAIN_MAX
 }eSTMENUMAIN;

////////////////////////////////////////////////
struct SUT_MENU s_sutMenuMain;
const char *s_apcMenuMain[]= 
{
    "��ʻ��¼",
    "��Ϣ����",
    #ifdef USE_PHONE_BOOK
    "�绰����",
    #endif
    "��ʻ���",    
    "��Ʒ��Ϣ",
    "SIM����Ϣ", 
    "TTS����", 
    "ϵͳά��",
    ""
};
////////////dxl,2015.6.27,���������ǲ˵�/////////
typedef enum 
{
    MENUGYRO_ACCELERATION,
    MENUGYRO_DECELERATION,
    MENUGYRO_SHARP_TURN,
    MENUGYRO_CALIBRATION,
    MENUGYRO_MAX
 }eSTMENUGYRO;
struct SUT_MENU s_sutMenuGyro;
const char *s_apcMenuGyro[]= 
{
    "���ü�����",
    "���ü�����",   
    "���ü�ת��",
    "������У׼", 
    ""
};
const char *c_apcMenuGyroAcceleration[] =
{
    " 0--30:",
    "30--60:",
    "60--90:",
    "90--  :",
    ""
};
const char *c_apcMenuGyroDeceleration[] =
{
    " 0--30:",
    "30--  :",
    ""
};
///////////dxl,end///////////////////////
////////////////////////////////////////////////
typedef enum 
{
    MENU_INFO_TEXT_INFO,        //"�ı���Ϣ",
    MENU_INFO_QUESTION_RESPONSE,//"����Ӧ��",
    MENU_INFO_INFO_SERVICE,     //"�㲥��Ϣ",    
    MENU_INFO_EVENT_REPORT,     //"�¼�����",
    MENU_INFO_RESERVE_SERVICE,  //"��Ϣ�㲥",    
    MENU_INFO_CAR_LOAD_STATUS,  //"�����ػ�״̬",
    MENU_INFO_MAX
 }eSTMENUINFO;
struct SUT_MENU s_sutMenuInfoServer;
const char *s_apcMenuInfoServer[]= 
{
    "�ı���Ϣ",
    "����Ӧ��",    
    "�㲥����Ϣ",
    "�¼�����", 
    "��Ϣ�㲥",    
    "�����ػ�״̬",
    ""
};

typedef enum 
{
    MENUDRIVERECORD_CAR_INFO,
    MENUDRIVERECORD_DRIVER_INFO,
    MENUDRIVERECORD_TIREDDRIVE_RECORD,
    MENUDRIVERECORD_15MIN_SPEED,
    MENUDRIVERECORD_MAX
 }eSTMENUDRIVERECORD;
struct SUT_MENU s_sutMenuDriveRecord;
const char *s_apcMenuDriveRecord[]= 
{
    "������Ϣ",
    "��ʻԱ��Ϣ",
    "��ʱ��ʻ��¼", 
     "15�����ٶ�",
    ""
};
    
//_____________________________________
struct SUT_MENU s_sutMenuSystemTest;
const char *s_apcMenuSystemTest[] = 
{
    "�ն�ע��",         //0
    "�ն˼�Ȩ",         //1
    //"�ն�ע��",         //2
    "λ����Ϣ�㱨",     //3
    "�¼�����",         //4
    "��Ϣ�㲥",         //5
    "��Ϣȡ��",         //6
    "�����˵��ϱ�",     //7
    "��ʻԱ�����Ϣ",   //8
    "�Ϸ���ý���¼�",   //9
    "��������͸��",     //10
    ""
};

struct SUT_MENU s_sutMenuShortMsg;
const char *s_apcMenuShortMsg[] = 
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

struct SUT_MENU s_sutMenuSystem;
const char *s_apcMenuSystem[] = 
{
    "�ն�ע��",         //0
    "�ն˼�Ȩ",         //1
    "���ñ���",         //    
    "�߼�����",         //
    "������ѡ��", 
    "�����׼����",         //0
    ""
};
typedef enum 
{
    MENU_SYSTEM_REGISTER,         //"�ն�ע��"
    MENU_SYSTEM_QUALIFY,          //"�ն˼�Ȩ"
    MENU_SYSTEM_BACKLIGHT,        //"���ñ���"
    MENU_SYSTEM_ADVANCED,         //"�߼�����"
    MENU_SYSTEM_MAIN_SELECT,      //"������ѡ��"
    MENU_SYSTEM_BASE_SET,         //"�����׼����"
    MENU_SYSTEM_MAX,              //������
}eSTMENUSYSTEM;

struct SUT_MENU s_sutMenuMainInterfaceSet;
const char *s_apcMenuMainInterfaceSet[] = 
{
    "����������",
    "�¶�������",    
    "�ٶ�������",
    "����������",
    "����������",
    "ת��������",         //0
    ""
};

typedef enum 
{
    MENU_INTERFACE_CONVENTION,           //�������
    MENU_INTERFACE_TEMPERATURE,          //�¶Ƚ���
    MENU_INTERFACE_SPEED,                //�ٶȽ���
    MENU_INTERFACE_LOAD,                 //���ؽ���
    MENU_INTERFACE_DEEP,                 //�������
    MENU_INTERFACE_TURNSPEED,            //ת�ٽ���
    MENU_INTERFACE_MAX,                  //������
}eSTMENUSELECT;

struct SUT_MENU s_sutMenuSystemAdvanced;
const char *s_apcMenuSystemAdvanced[] = 
{
    "������Ϣ����",
    "����������",    
    "��װ����",
    "�ն˲�������",
    "������������",
    "ʹ��ǰ����һ",
    "ʹ��ǰ������", 
    #ifdef CALIB_OIL_WEAR
    "�����궨",
    #endif
    "������Ϣ��ѯ",
    "������",//dxl,2015.6.27,���������ǲ˵�    
    ""
};

typedef enum 
{
    MENU_SYS_CAR_INFO,          //"������Ϣ"
    MENU_SYS_SERVER_INFO,       //"����������"
    MENU_SYS_DEBUG,             //"��װ����"
    MENU_SYS_TERMINAL_PARAM,    //"�ն˲���"
    MENU_SYS_ENCRYPT,
    MENU_SYS_LOCK_ONE,
    MENU_SYS_LOCK_TWO,    
    #ifdef CALIB_OIL_WEAR
    MENU_SYS_OIL_DEMARCATE,     //"�����궨"
    #endif
    MENU_SYS_PERP_INFORM_QUERY,
    MENU_SYS_GYRO,//dxl,2015.6.27,���������ǲ˵�   
    MENU_SYS_MAX
}eSTMENUSYS;


 
 /*������Ϣ��ѯ*/
    typedef enum 
    {
        
        MENU_EQM_CAMERA,            //"����ͷ",
        MENU_EQM_WEIGHT_DEV,        //"���زɼ���",
        MENU_EQM_TMP_COLLECT,       //"�¶Ȳɼ���",
        MENU_EQM_OIL_DETECT,        //�����ɼ���,
        MENU_EQM_CANINFORM_COLLECT,    //"CAN��Ϣ�ɼ�ģ��",
        MENU_EQM_NAVIGATION_SCR,    //"������", 
        MENU_EQM_MAX
    }eSTMENUEQM;


struct SUT_MENU s_sutMenuSystemPeripheral;
const char *s_apcMenuSystemPeripheral[] = 
{
    
    "����ͷ",
    "���زɼ���",    
    "�¶Ȳɼ���",
    "�����ɼ���",
    "CAN��Ϣģ��",
    "������", 
    "û������"
    ""
};

struct SUT_MENU s_sutMenuSystemCarInfo;
const char *s_apcMenuSystemCarInfo[] = 
{
    "���ó��ƺ���",
    "���ó���VIN",    
    "���ó�����ɫ",
    "���ó��Ʒ���",
    "���ó�ʼ���",
    "��������ϵ��",
    "����ʡ��ID",
    "��������ID",
    "�����ֻ���",
    "�����ػ�״̬",
    ""
};

typedef enum 
{
    MENU_CAR_SET_PLATE_NUM,     //"���ó��ƺ���"
    MENU_CAR_SET_VIN_NUM,       //"���ó���VIN",
    MENU_CAR_SET_PLATE_COLOR,   //"���ó�����ɫ"
    MENU_CAR_SET_PLATE_TYPE,    //"���ó��Ʒ���"
    MENU_CAR_SET_TOTAL_MILE,    //"���ó�ʼ����", 
    MENU_CAR_SET_FEATUER_COEF,  //"��������ϵ��",
    MENU_CAR_SET_PROVINCE_ID,   //"����ʡ��ID",
    MENU_CAR_SET_CITY_ID,       //"��������ID",
    MENU_CAR_SET_OWNER_PHONE,     //"�����ֻ���"
    MENU_CAR_SET_LOAD_STATE,     //"�ػ�״̬"
    MENU_CAR_SET_MAX
}eSTMENUSYSCAR;

struct SUT_MENU s_sutMenuSystemServer;
const char *s_apcMenuSystemServer[] = 
{
    "������IP",
    "���ñ���IP",
    "����APN",
    "����TCP�˿ں�",
    "����UDP�˿ں�",    
    "����IP2",
    "���ñ���IP2",
    "����TCP2",
    "����UDP2",
    "����������",
    "������������",
    "IP1��ȫ������",
    "IP1������ƽ̨",
    ""
};

typedef enum 
{
    MENU_SERVER_SET_IP,         //"������"    
    MENU_SERVER_SET_BACKUP_IP,  //"���ñ���
    MENU_SERVER_SET_APN,        //"����APN"
    MENU_SERVER_SET_TCP,        //"����TCP�˿ں�"
    MENU_SERVER_SET_UDP,        //"����UDP�˿ں�",    
    MENU_SERVER_SET_IP2,         //"����IP2"    
    MENU_SERVER_SET_BACKUP_IP2,  //"���ñ���IP2"
    MENU_SERVER_SET_TCP2,        //"����TCP2"
    MENU_SERVER_SET_UDP2,        //"����UDP2",
    MENU_SERVER_MAIN_DNS,       //"����������"
    MENU_SERVER_BAK_DNS,        //"������������", 
    MENU_SERVER_FREIGHT,        //"IP1��Ϊȫ������",
    MENU_SERVER_EYE_PLATFORM,   //"IP1��Ϊ����ƽ̨",
    MENU_SERVER_SET_MAX
}eSTMENUSYSSERVER;

struct SUT_MENU s_sutMenuSysManageServer;
const char *s_apcSysManageServer[] = 
{
    "�鿴����������",
    "�޸ķ���������",
    ""
};
typedef enum 
{
    MENU_SYS_MANAGE_SERVER_QUERY,   //"������"    
    MENU_SYS_MANAGE_SERVER_SET,  //"���ñ���   
    MENU_SYS_MANAGE_SERVER_MAX
}eSTMENUSYSMANAGESERVER;

struct SUT_MENU s_sutMenuSystemDebug;
const char *s_apcMenuSystemDebug[] = 
{
    "����״̬��ʾ",
    "���״̬����",
    "����ϵ��У׼",     //
    "���ΰ�װʱ��",     //
    "�෭�Ƕ�У׼",     //
    "��ǰ�㱨����",    
    "�ն�ע��",
    "�����޸�",
    "ϵͳ��λ",
    " ��  �� ",
    "��װģʽ", 
    "������Сֵ",
    "�������ֵ",
    "�����׼�궨",
    "ת������궨",
    ""
};

typedef enum 
{
    MENU_DEBUG_IO_IN,               //"����״̬��ʾ"
    MENU_DEBUG_IO_OUT,              //"���״̬����"
    MENU_DEBUG_CALI_FEATUER_COEF,   //"����ϵ��У׼"
    MENU_DEBUG_FIX_TIME,            //"���ΰ�װʱ��"
    MENU_DEBUG_POSITION,            //"�෭�Ƕ�У��"
    MENU_DEBUG_REPORT_MODE,         //"��ǰ�㱨����"    
    MENU_DEBUG_LOGOUT,              //"�ն�ע��"
    MENU_DEBUG_PASSWORD,            //"�����޸�"
    MENU_DEBUG_RESET,               //"ϵͳ��λ"
    MENU_DEBUG_TEST,                //" �� �� "
    MENU_DEBUG_TTS_INSTALL,         //"��װģʽ"
    MENU_DEBUG_DEEP_MIN,//"������Сֵ",
    MENU_DEBUG_DEEP_MAX,//"�������ֵ",
    MENU_DEBUG_DEEP_BASE,//"�����׼�궨",
    MENU_DEBUG_TURNSPEEP,//"ת������궨",
    MENU_DEBUG_MAX
}eSTMENUSYSDEBUG;

struct SUT_MENU s_sutMenuSystemParameter;
const char *s_apcMenuSystemParameter[] = 
{
    "�����ֻ���",    
    "�����ٶȲ���",
    "���ŵ�ƽѡ��",
    "�����ȵ�ƽѡ��",
    "����CDMA����",
    "��ѯ�ն�ID��",
    "�ָ���������", 
    "GNSS��λģʽ",
    ""
};

typedef enum 
{
    MENU_PARAM_PHONE_NUMBER,        //"�����ֻ���""    
    MENU_PARAM_SPEED_PARAM,         //"�����ٶȲ���",
    MENU_PARAM_DOOR_LEVEL_SEL,      //"���ŵ�ƽѡ��"
    MENU_PARAM_HORN_LEVEL_SEL,      //"�����ȵ�ƽѡ��"
    MENU_PARAM_CDMA_SLEEP,          //"����CDMA����",
    MENU_PARAM_TERMINAl_ID,         //"�ն�ID��"
    MENU_PARAM_DEFAULT_PARAM,       //"�ָ���������"   
    MENU_PARAM_GNSS_WORD_MODE,      //"GNSS��λģʽ",   
    MENU_PARAM_MAX
}eSTMENUSYSPARAM;

const char *c_apcCarSpeedSelectItems[]=
{
    "�ֶ�����",
    "�ֶ�GPS",
    "�Զ�����",
    "�Զ�GPS",
    ""
};

////////////////////////////////
const  char *c_apcCarPlakeColorItems[]=
{
    " ��ɫ ",
    " ��ɫ ",
    " ��ɫ ",
    " ��ɫ ",    
    " ���� ",
    "δ����",
    ""
};

const  char *c_apcCarPlakeTypeItems[]=
{
    "Σ��Ʒ��",
    "���Ϳͳ�",
    "���Ϳͳ�",
    "С�Ϳͳ�",
    "���ͻ���",
    "���ͻ���",
    "С�ͻ���",
    "���������ƺ�",
    "�ҳ��ƺ�",
    "С�������ƺ�",
    "�����ƺ�",
    ""
};

const  char *c_apcCarDoorSetItems[]=
{
    "���Ÿ���Ч",
    "��������Ч",
    ""
};

const  char *c_apcCarHornSetItems[]=
{
    "�ߵ�ƽ��Ч",
    "�͵�ƽ��Ч",
    ""
};
const  char *c_apcCDMASleepItems[]=
{
    "�ر�",
    "����",
    ""
};
#ifndef JIUTONG_VERSION
const  char *s_apcAPNItems[]=
/*{
    "cmnet",
    "uninet", 
    "",
    ""
};*/
{
  "cmnet",
  "uninet", 
  "CMIOTYYC.GD", 
  "gzjtxx03.gd",
  "GZJTYY.GD",
  "",
  ""
};

#endif

const  char *c_apcCarLoadStatusItems[]=
{
    "����",
    "����",
    "����",
    ""
};
//////////////////////////
/*
const char *s_apcCenterDnsItems[]=
{
    "jt1.gghypt.net",
    "jt2.gghypt.net",
    ""
};
*/

const  char *c_apcGNSSworkModeItems[]=
{
    "����",     
    "����",
    ""
};

const  char *c_apcBeforeLockItems[]=
{
    " ��  �� ",     
    " ��  �� ",
    ""
};
const  char *c_apcEncryptItems[]=
{
    " ������ ",     
    " ��  �� ",
    ""
};
const  char *c_apcLock1Items[]=
{
    " �� ֹ",     
    " �� ��",
    ""
};
/////////////////////////
///**********************************
#define IO_IN_ITEMS_NUM 15
#define IO_ONE_PAGE_NUM 8

const char *c_apcIOINItems[]=
{
    "ɲ��",
    "ACC",
    //"���",
    "С��",
    "��ת��",
    "��ת��",
    "Զ���",
    "�����",
    "���",
    "��ǰ��",
    "������",
    "�յ�",    
    "��������",
    "һ��ͨ��",    
    "��������",    
    "ʾ����",
    ""
};

const char *c_apcIOOutItems[]=
{
    " ��  �� ",
    " ��  · ",
    " ת��� ",
    "��������",
    ""
};

//
typedef enum 
{
//////////����Ϊ������,˳����c_apcIOINItemsһ��///////////////////
    IO_BRAKE,           //" ɲ�� ",
    IO_CAR_ACC,         //" ACC  ",
    //IO_BIG_LIGHT,       //"���",
    IO_SMALL_LIGHT,     //"С��"
    IO_LEFT_LIGHT,      //"��ת��", 
    IO_RIGHT_LIGHT,     //"��ת��", 
    IO_FAR_LIGHT,       //"Զ���",
    IO_NEAR_LIGHT,      //"�����",
    IO_FOG_LIGTH,           //"���",
    IO_FRONT_DOOR,      //"��ǰ��",
    IO_CAR_MID_DOOR,    //"������", 
    IO_AIR_COND,        //" �յ� ",    
    IO_URGENT,          //"��������",
    IO_ONE_KEY,         //"һ��ͨ��",    
    IO_CAR_HORN,        //"��������"    
    IO_SIDE_LIGHT,       //"ʾ����"      
    IO_IN_MAX,          //"���������",
////////////����Ϊ�����,˳����c_apcIOOutItemsһ��///////////////////
    IO_CTRL_CAR_DOOR,   //" ���� ",
    IO_CTRL_OIL_BREAK,  //" ���� ",
    IO_CTRL_TURN_LIGHT, //" ת��� ",
    IO_CTRL_ALARM_HORN, //"��������"
    IO_STATUS_MAX       //"IO״̬�����",
}eIOSTATUS;
///////////////////////

#define PRODUCT_ITEMS 20//dxl,2015.6.29,ԭ����15
//��Ʒ��Ϣ

//////////////////////////////
const char *c_apcInputString[] =
{
    "0123456789AB",
    "CDEFGHIJKLMN",
    "OPQRSTUVWXYZ",
    ""
};
const char *c_apcInputStringEx[] =
{
    "����³���´�����",
    "�ӹ��ڻ�������",
    "����������������",
    "������ԥ�����",
    ""
};
///////////////////////////////
const char *c_apcGNSSWorkModeShow[] =
{
    "GPS��λ :",
    "������λ:",
    "GLONASS :",
    "Galileo :",
    ""
};
const char *c_apcInputParam[] =
{
    "0123456789ABC",
    "DEFGHIJKLMNOP",
    "QRSTUVWXYZ. ",
    ""
};
#ifdef __LZMINPUTBOX_H
ST_LZMINPUTBOX stInputBox[2];
#endif
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/ 

typedef struct
{
     unsigned long tcp;             //tcp
     unsigned long udp;             //udp
     unsigned char mainIP[4];       //ת��֮�����IP
     unsigned char backIP[4];       //ת��֮���
     char APN[20];                  //ת��֮���
     unsigned char brushcardFlag;   //ˢ����־
     unsigned char reverse[3];      //����
}STSERVERPARAM;

static STSERVERPARAM s_stServerParam;

unsigned char GB2312Buffer[1024];
static char *s_apcShowP;

/****************************************************/
/*                                                  */
/* Global variables                                 */
/*                                                  */
/****************************************************/
LZM_RET TaskMenuSystemAdvanced(LZM_MESSAGE *Msg);
LZM_RET TaskMenuMain(LZM_MESSAGE *Msg);
LZM_RET TaskShowMainInterface(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystemDebugTest(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystem(LZM_MESSAGE *Msg);
LZM_RET TaskShowDisplayText(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystemCarInfo(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystemServer(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystemDebug(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystemParameter(LZM_MESSAGE *Msg);
LZM_RET TaskShowDisplayTextForTaxiTender(LZM_MESSAGE *Msg);
LZM_RET TaskMenuDriveRecord(LZM_MESSAGE *Msg);
LZM_RET TaskMenuInfoServer(LZM_MESSAGE *Msg);
LZM_RET TaskMenuBeforeLockOne(LZM_MESSAGE *Msg);
LZM_RET TaskMenuBeforeLockTwo(LZM_MESSAGE *Msg);
LZM_RET TaskMenuSystemManageServer(LZM_MESSAGE *Msg);
LZM_RET TaskShowQuestionResponse(LZM_MESSAGE *Msg);
LZM_RET TaskQueryPeripheralInform(LZM_MESSAGE *Msg);
LZM_RET TaskSystemCameraInfo(LZM_MESSAGE *Msg) ;
LZM_RET TaskMainInterfaceSelection(LZM_MESSAGE *Msg);
void Adjust_Array_Size(const char **dec,const char **src);
LZM_RET TaskMainInterfaceSelection(LZM_MESSAGE *Msg);
void IP1SaveFreight(void);
void IP1SaveEyePlat(void);
void IP1CancelModify(void);


//////////////////////////////////////////////
/****************************************************/
/*                                                  */
/* Local Variables                                  */
/*                                                  */
/****************************************************/
void CheckCurrentLcdBackLight(void);
void CalibrationFeatureCoef(void);
unsigned char IsBrushAdministratorCard(void);
/****************************************************/
/*                                                  */
/* Forward Declaration of local functions           */
/*                                                  */
/****************************************************/


/*********************************************************************
*
*       Private  code
*
*********************************************************************/
/*************************************************************
** ��������: GetCurentProductModel
** ��������: ��õ�ǰ�ն��ͺ�
** ��ڲ���: ��
** ���ڲ���: name�׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void GetCurentProductModel(char *name)
{
    unsigned char i;
    char temp[17];
    unsigned char tmpbuffer[40] = {0};
    if(E2_CAR_ONLY_NUM_ID_LEN ==EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,tmpbuffer))
    {
        memcpy(temp,&tmpbuffer[7],16);
        for(i=0; i<16;i++)
        {
            //if(temp[i]<0x30)
            if(0 == temp[i])
            {
                break;
            }
        }
        ///////////////
        temp[i] = 0;
        ////////////
        strcpy(name,temp);
    }
    else
    {
        strcpy(name,"EGS701");
    }
}
/*************************************************************
** ��������: ReadSystemBacklightFlag
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ReadSystemBacklightFlag(void)
{
    EepromPram_ReadPram(E2_BACKLIGHT_SELECT_ID, &s_stTaskAttrib.BacklightFlag);
    ///////////////////////////////////////
    if(s_stTaskAttrib.BacklightFlag>1)s_stTaskAttrib.BacklightFlag=0;
    ///////////////////////////
}
/*************************************************************
** ��������: ReadSystemBacklightFlag
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SaveSystemBacklightFlag(void)
{
    if(s_stTaskAttrib.BacklightFlag<2)
    EepromPram_WritePram(E2_BACKLIGHT_SELECT_ID, &s_stTaskAttrib.BacklightFlag, 1);
}
/*************************************************************OK
** ��������: SysAutoRunTestTask()
** ��������: �Զ�����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void SysLcdBackLightCntReset(void)
{
    s_stTaskAttrib.LcdTimeOutCnt=0;
}
/*************************************************************OK
** ��������: SysAutoRunTestTask()
** ��������: �Զ�����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void SysAutoRunCountReset(void)
{
    s_stTaskAttrib.AutoRunCnt=0;
    s_stTaskAttrib.LcdTimeOutCnt=0;
    if(!LcdOnOffFlag)
    {
        LCD_LIGHT_ON();
    }
}
/*************************************************************OK
** ��������: SysAutoRunTestTask()
** ��������: 0.2s����һ��
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void SysAutoRunToMainInfoTask(void)
{
    static unsigned char sucFlag=0;    
    if(LZM_TaskFunction!=TaskShowMainInterface)
    {
        s_stTaskAttrib.AutoRunCnt++;
        if(sucFlag==0)
        {
            sucFlag=1;
        }
        else
        if(sucFlag==1)
        {
            if(VDRDoubt_GetCarRunFlag())s_stTaskAttrib.AutoRunCnt+=5;//��û���ٶȵ������6��
            if(s_stTaskAttrib.AutoRunCnt>AUTORUN_MAIN_TIME)
            {
                sucFlag=2;
                s_stTaskAttrib.AutoRunCnt =0;
                LZM_StartNewTask(TaskShowMainInterface);
            }
        }
        else
        if(s_stTaskAttrib.AutoRunCnt>5)//
        {
            sucFlag=0;
        }
    }
    else
    {
        sucFlag =0;
        s_stTaskAttrib.AutoRunCnt =0;
    }
    ///////////////////////
    CheckCurrentLcdBackLight();
}
/*************************************************************
** ��������: FlashLcdBackLight
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void FlashLcdBackLight(void)
{
    if(s_stTaskAttrib.FlashBacklightFlag==0)
    {
        s_stTaskAttrib.FlashBacklightFlag =10+LcdOnOffFlag;
        LZM_SetAlarm(FLASH_TIMER, SECS(0.3));
        s_stTaskAttrib.LcdTimeOutCnt=0;
        if(!LcdOnOffFlag)
        {
            LCD_LIGHT_ON();
        }
        else
        {
            LCD_LIGHT_OFF();
        }
    }
}

/*************************************************************
** ��������: FlashLcdBackLightFinish
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void FlashLcdBackLightFinish(void)
{
    if(s_stTaskAttrib.FlashBacklightFlag)
    {
        s_stTaskAttrib.FlashBacklightFlag -=10;
        if(!s_stTaskAttrib.FlashBacklightFlag)
        {
            LCD_LIGHT_OFF();
        }
        else
        {
            LCD_LIGHT_ON();
        }
        s_stTaskAttrib.FlashBacklightFlag = 0;
        s_stTaskAttrib.LcdTimeOutCnt=0;
    }
}

/*************************************************************
** ��������: OpenLcdBackLight
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void OpenLcdBackLight(void)
{
    s_stTaskAttrib.LcdTimeOutCnt=0;
    if(!LcdOnOffFlag)
    {
        LCD_LIGHT_ON();
    }
    else
    if(s_stTaskAttrib.BacklightFlag)//����
    {
        FlashLcdBackLight();//
    }
}
/*************************************************************
** ��������: TestBeep
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void CheckCurrentLcdBackLight(void)
{
    ////////////////////////
    if(s_stTaskAttrib.LcdTimeOutCnt<LCD_TIMEOUT)//�ӿ����⿪ʼ����,���೤ʱ��ȥ��
    {
        s_stTaskAttrib.LcdTimeOutCnt++;
    }
    else
    {
        unsigned char ACC;
        //��ȡACC״̬
        ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
        /////////////////////////////
        if((1 == s_stTaskAttrib.BacklightFlag)&&(1 == ACC))//���Ϊ����Ϊ����
        {
            //��
            if(!LcdOnOffFlag)
            {
                LCD_LIGHT_ON();
            }
        }
        else
        if(LcdOnOffFlag)//
        {
            LCD_LIGHT_OFF();
        }
        s_stTaskAttrib.LcdTimeOutCnt=0;
    }
}
/*************************************************************
** ��������: GetCurrentNotEnoughRowNum
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char GetCurrentNotEnoughRowNum(unsigned short textlen,unsigned char *buffer)
{
    unsigned short i;
    unsigned char temp,num,k;
    if(textlen==0)return 0;
    k=0;
    num =0;
    for(i=0; i<textlen;)
    {
        temp=buffer[i];
        if (temp > 0x80)                            //chinese letter
        {
            i+=2;
            k+=2;
        }
        else
        {
            i++;
            k++;
        } 
        /////////////////////////
        if(k==16)////�պ���һ��
        {
            k=0;
            num++;
        }
        else//
        if(k>16)//����һ����
        {
            k=2;
            num++;
        }
    }
    return k;
}
/*************************************************************
** ��������: GetCurrentMsgPageNum
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char GetCurrentMsgPageNum(unsigned short textlen,unsigned char *buffer,unsigned char flag)
{
    unsigned short i;
    unsigned char temp,num,k;
    if(textlen==0)return 0;
    k=0;
    num =0;
    for(i=0; i<textlen;)
    {
        temp=buffer[i];
        if (temp > 0x80)                            //chinese letter
        {
            i+=2;
            k+=2;
        }
        else
        {
            i++;
            k++;
        } 
        /////////////////////////
        if(k==16)////�պ���һ��
        {
            k=0;
            num++;
        }
        else//
        if(k>16)//����һ����
        {
            k=2;
            num++;
        }
    }
    
    if(k!=0)
    {
      num++;
    }
    /////////////////////////
    if(flag==0)//////////////
    {
        num=(num+3)/4;
    }
    else///////////////������ʾ3��
    {
        num=(num+2)/3;
    }
    return num;
}
/*************************************************************
** ��������: GetCurrentPageMsgDataStartAddr
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned short GetCurrentPageMsgDataStartAddr(unsigned char page,unsigned char *buffer,unsigned char flag)
{
    unsigned short i,maxlen;
    unsigned char temp,num,k,maxnum;
    if(page==0)return 0;
    ///////////////
    if(flag==0)//////////////
    {
        maxlen=(page-1)*64;
        maxnum=(page-1)*4;
    }
    else
    {
        maxlen=(page-1)*48;
        maxnum=(page-1)*3;
    }
    
    k=0;
    num =0;
    for(i=0; i<maxlen;)
    {
        temp=buffer[i];
        if (temp > 0x80)                            //chinese letter
        {
            i+=2;
            k+=2;
        }
        else
        {
            i++;
            k++;
        }
        ///////////////////////
        if(k==16)//�պ���һ��
        {
            k=0;
            num++;
            if(num>maxnum)break;
        }
        else
        if(k>16)//����һ����
        {
            k=2;
            num++;
            if(num>maxnum)break;
        }
    }
    maxlen=i-k;
    return maxlen;
}
/*************************************************************
** ��������: ConvertStrToIP
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static void ConvertStrToIP(unsigned char buf[],unsigned char *val,unsigned char len)
{
    unsigned char i,j,k;
    unsigned short temp;
    unsigned char *p;
    unsigned char tempbuf[5];
    p=buf;
    j=0;
    k=0;
    for(i=0; i<len; i++)
    {
        tempbuf[j]=*p;
        p++;
        j++;
        if(*p=='.')
        {
            tempbuf[j]='\0';
            temp=atoi((char *)tempbuf);
            val[k++]=temp;
            j=0;
            p++;
        }
    }
    tempbuf[j]='\0';
    temp=atoi((char *)tempbuf);
    val[k]=temp; 
}
/*************************************************************
** ��������: ConvertReadPramToValue
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static unsigned long ConvertReadPramToValue(unsigned long PramID)
{
    unsigned char i,len;
    unsigned char buf[6];
    unsigned long temp;
    len=EepromPram_ReadPram(PramID, buf);
    temp =0;
    for(i= 0; i<len; i++)
    {
        temp|=(buf[len-1-i]<<(i<<3));
    }
    return temp;
}
/*************************************************************
** ��������: ReadSystemPassWord
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static void ReadSystemPassWord(void)
{  
    unsigned char buf[6];
    if(4 == EepromPram_ReadPram(E2_PASSWORD_ID, buf))
    {
        s_stTaskAttrib.SysPassword = Public_ConvertBufferToLong(buf);
    }
    else
    {
        s_stTaskAttrib.SysPassword = SYS_DEFAULT_PASSWORD;
        Public_ConvertLongToBuffer(s_stTaskAttrib.SysPassword,buf);
        EepromPram_WritePram(E2_PASSWORD_ID,buf,4);
    }
    /////////////////////////
    if(s_stTaskAttrib.SysPassword>999999)
    {
        s_stTaskAttrib.SysPassword = s_stTaskAttrib.SysPassword%1000000;
    }    
}
/*************************************************************
** ��������: ReadAndCheckPhoneNumber
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static unsigned char ReadAndCheckPhoneNumber(unsigned char *buf)
{
    unsigned char Buffer[13],tmpBuf[8];
    unsigned char i,BufferLen,temp,flag;
    flag =0;
    BufferLen = EepromPram_ReadPram(E2_DEVICE_PHONE_ID, Buffer);
    if(E2_DEVICE_PHONE_ID_LEN == BufferLen)
    {
        for(i=0; i<BufferLen; i++)
        {
            temp=Public_CheckStrIsBCD(Buffer[i]);
            if(temp==0xff)
            {
                flag=1;
                break;
            }
            else
            {
                tmpBuf[i]=temp;
            }
        }
        if(!flag)
        {
            sprintf((char *)buf,"%d%02d%02d%02d%02d%02d",tmpBuf[0],tmpBuf[1],tmpBuf[2],tmpBuf[3],tmpBuf[4],tmpBuf[5]);
            return 1;
        }
    }
    return 0;
}
/*************************************************************
** ��������: Public_ConvertLongToBuffer
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static void ConvertPhoneStrToBCD(unsigned char src[],unsigned char *buf)
{
    unsigned char i;
    buf[0]=src[0]-0x30;
    for(i= 1; i<E2_DEVICE_PHONE_ID_LEN; i++)
    {
        buf[i] = ((src[2*i-1]-0x30)<<4);//���ֽ�
        buf[i] |= src[2*i]-0x30;////���ֽ�
    }
}
/*************************************************************
** ��������: SaveAndCheckTerminalPhoneNumber
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static unsigned char SaveAndCheckTerminalPhoneNumber(unsigned char src[])
{
    unsigned char val;
    unsigned char buf[8],tmpbuf[13];
    ConvertPhoneStrToBCD(src,buf);
    EepromPram_WritePram(E2_DEVICE_PHONE_ID, buf,E2_DEVICE_PHONE_ID_LEN);
    val=ReadAndCheckPhoneNumber(tmpbuf);
    if(val==1)
    {
        if(strncmp((char *)src,(char *)tmpbuf,11)==0)
        {
            return 1;
        }
    }
    return 0;
    
}

/*************************************************************
** ��������: ReadAndCheckPhoneNumber
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static unsigned char ReadSystemDeviceID(unsigned long *deviceID,unsigned short *manufacturer)
{
    unsigned char Buffer[13];
    unsigned char i,BufferLen;
    unsigned long ulTmp;
    unsigned short temp;
    BufferLen = EepromPram_ReadPram(E2_DEVICE_PHONE_ID, Buffer);
    if(E2_DEVICE_PHONE_ID_LEN == BufferLen)
    {
        ulTmp =0;
        for(i= 0; i<4; i++)
        {
            ulTmp|=(Buffer[6-1-i]<<(i<<3));
        }
        temp=(Buffer[0]<<8)+Buffer[1];
        *deviceID = ulTmp;
        *manufacturer =temp;
        return 1;
    }
    return 0;
}

/*************************************************************
** ��������: SaveAndCheckSystemDeviceID
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
static unsigned char SaveAndCheckSystemDeviceID(unsigned long deviceID,unsigned short manufacturer)
{
    unsigned char val;
    unsigned char buf[8];
    unsigned long ulTmp;
    unsigned short temp;

    buf[0]=manufacturer>>8;
    buf[1]=(manufacturer & 0xff);
    Public_ConvertLongToBuffer(deviceID,&buf[2]);

    EepromPram_WritePram(E2_DEVICE_PHONE_ID, buf,E2_DEVICE_PHONE_ID_LEN);
    val=ReadSystemDeviceID(&ulTmp,&temp);
    if(val==1)
    {
        if((ulTmp==deviceID)&&(temp==manufacturer))
        {
            return 1;
        }
    }
    return 0;
}


/*************************************************************
** ��������: GetAllCurrentIOINstatus
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char GetCurrentIOstatus(unsigned char index)
{
    unsigned char val;
    val=0;
    switch(index)
    {
        case IO_BRAKE://" ɲ�� ",
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE);
            break;
        case IO_CAR_ACC://" ACC  ",
            val=Io_ReadStatusBit(STATUS_BIT_ACC);
            break;
        //case IO_BIG_LIGHT://"���",
        //    val=GetBigLightStatus();
        //    break;
        case IO_SMALL_LIGHT://"С��"
            //val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE);
            break;
        case IO_LEFT_LIGHT://"��ת��", 
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT);
            break; 
        case IO_RIGHT_LIGHT://"��ת��", 
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT);
            break;
        case IO_FAR_LIGHT://"Զ���",
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT);
            break;
        case IO_NEAR_LIGHT://"�����",
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT);
            break;
        case IO_FOG_LIGTH://"���",
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FOG_LIGTH);
            break;
        case IO_FRONT_DOOR://"��ǰ�� ",  
            val=Io_ReadStatusBit(STATUS_BIT_DOOR1);
            break;
        case IO_CAR_MID_DOOR://" ������ ",   
            val=Io_ReadStatusBit(STATUS_BIT_DOOR2);
            break;        
        case IO_AIR_COND://" �յ� ",
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_AIR_CONDITION);
            break;
        case IO_URGENT://"��������",
            val=Io_ReadAlarmBit(ALARM_BIT_EMERGENCY);
            break;
        case IO_ONE_KEY://"һ��ͨ��",
            val=GpioInGetState(VIRBRATIONI);
            val^=1;
            break;        
        case IO_CAR_HORN://"��������"
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_HORN);
            break;        
        case IO_SIDE_LIGHT://"ʾ����"
            val=Io_ReadExtCarStatusBit(CAR_STATUS_BIT_SIDE_LIGHT);
            break;        
        
        /////////////////////////
        case IO_CTRL_CAR_DOOR://���Ƴ���
            val=GpioInGetState(UNLOCK_OUT);
            break;
        case IO_CTRL_OIL_BREAK://������·
            val=GpioInGetState(COIL);
            break;
        case IO_CTRL_TURN_LIGHT://" ת��� ",
            val=GpioInGetState(LAMP);
            break;
        case IO_CTRL_ALARM_HORN://"��������"
            val=GpioInGetState(HORN_OUT);
            break;
        ////////////////////////
    }
    return val;
}
/*************************************************************
** ��������: GetAllCurrentIOINstatus
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned long GetAllCurrentIOINstatus(void)
{
    unsigned long ulval;
    unsigned char tmp,i;    
    ulval=0;
    for(i=0; i<IO_IN_ITEMS_NUM; i++)
    {
        tmp=GetCurrentIOstatus(i);
        ulval |= (tmp<<i);
    }
    return ulval;
}
/*************************************************************
** ��������: GetAllCurrentIOINstatus
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ChangCurrentIOOutstatus(unsigned char index,unsigned char val)
{
    switch(index)
    {
        case IO_CTRL_CAR_DOOR://" ���� ",
            if(val)
            {
                GpioOutOn(UNLOCK_OUT);
                GpioOutOff(LOCK_OUT);
            }
            else
            {
                GpioOutOn(LOCK_OUT);
                GpioOutOff(UNLOCK_OUT);
            }            
            break;
        case IO_CTRL_OIL_BREAK://" ���� ",
            if(val)
            {
                GpioOutOn(COIL);
            }
            else
            {
                GpioOutOff(COIL);
            }
            break;
        case IO_CTRL_TURN_LIGHT://" ת��� ",
            if(val)
            {
                GpioOutOn(LAMP);
            }
            else
            {
                GpioOutOff(LAMP);
            }
            break;
        case IO_CTRL_ALARM_HORN://"��������"
            if(val)
            {
                GpioOutOn(HORN_OUT);
            }
            else
            {
                GpioOutOff(HORN_OUT);
            }
            break;
        default:
            break;
    }
}
/*************************************************************
** ��������: ShowCurrentIOstatus
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowCurrentIOstatus(unsigned char page)
{
    unsigned long tmp;
    unsigned char i,j,index,val,x;
    index=page*IO_ONE_PAGE_NUM;
    if(index>=IO_IN_ITEMS_NUM)return;
    tmp = GetAllCurrentIOINstatus();
    LcdClearScreen();
    LcdShowVerLine(65,0,63,0);
    for(i = 0; i< IO_ONE_PAGE_NUM; i++)
    {
        j=i/2;
        val=((tmp>>index) & 0x01);
        x=((strlen(c_apcIOINItems[index]))<<3);
        x=((66-x)>>1);
        LcdShowStr((i%2)*66+x, j*16, c_apcIOINItems[index],val);
        index++;
        if(index>=IO_IN_ITEMS_NUM)return;
    }
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/*************************************************************OK
** ��������: PublicGetPasswordCancel()
** ��������: �����������ʱ�ķ��ذ�����Ӧ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
static void PublicGetPasswordCancel(void)
{
    LZM_ReturnPreTask(PasswordPreTaskFunction);
}

/*************************************************************OK
** ��������: PublicGetPasswordCorrect()
** ��������: ����������ȷ���л���ָ������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ��
*************************************************************/
static void PublicGetPasswordCorrect(void)
{
    LZM_StartNewTaskEx((LZM_RET (*)(LZM_MESSAGE *Msg))TaskTo);
}
/*************************************************************OK
** ��������: AppGetPasswordKeyResponse()
** ��������: �������밴����Ӧ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ��
*************************************************************/
static void PublicGetPasswordKeyResponse(unsigned int key)
{
    unsigned long Password;
    switch(key)
    {
        case KEY_ENTER:
            Password = (unsigned long)atol(s_stESpin[0].buffer);
            if((Password == s_stTaskAttrib.InPassword)||(Password==SUPER_PASSWORD))
            {
                PublicGetPasswordCorrect();
            }
            else
            {
                LcdShowBorder();
                LcdShowCaption("�������!",20);                
                LZM_SetAlarm(TEST_TIMER,SECS(1));
            }
            break;
        case KEY_BACK:
            PublicGetPasswordCancel();
            break;
        default:
            ESpinResponse(&s_stESpin[0],key);
            break;
    }   

}
/*************************************************************OK
** ��������: TaskGetPassword()
** ��������: ������������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ��
*************************************************************/
static LZM_RET TaskGetPassword(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(TEST_TIMER);
            Lcd_SetSystemRFEnable();
            LcdClearScreen();
            LcdShowCaption("����������:",2);
            LcdShowHorLine(0, 131, 20, 1);            
            LcdShowHorLine(0, 131, 44, 1);
            LcdShowCaption("��ˢ����Ա��",47); 
            ESpinInit(&s_stESpin[0],42,25,6,SHOW_INPUT_PASSWORD);
            s_stESpin[0].type =2;
            ESpinSetFocus(&s_stESpin[0]);
            ESpinShow(&s_stESpin[0]);
            break;   
        case KEY_PRESS:
            PublicGetPasswordKeyResponse(TaskPara);
            break;
        case TEST_TIMER:
            PublicGetPasswordCancel();                      
            break;
        default:
            LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************OK
** ��������: AppGetPasswordTo()
** ��������: �������:�����������passwordһ��ʱ,ִ������task
** ��ڲ���: task:������ȷִ�е�����
             password:����ֵ
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void PublicGetPasswordTo(void (*task)(void),unsigned long password)
{
    TaskTo = task;
    s_stTaskAttrib.InPassword =password;
    PasswordPreTaskFunction = LZM_TaskFunction;
    LZM_RefuseNowTask(TaskGetPassword);
}

/*************************************************************OK
** ��������: PublicMsgBoxCancel()
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
static void PublicMsgBoxCancel(void)
{
    if(s_fptrCancel)
    {
        s_fptrCancel();
    }
    else
    {
        LZM_ReturnPreTask(PasswordPreTaskFunction);
    }
}
/*************************************************************OK
** ��������: TaskGetPassword()
** ��������: ������������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ��
*************************************************************/
static LZM_RET TaskPublicMsgBox(LZM_MESSAGE *Msg)
{    
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.ControlId = s_stTaskAttrib.select;
            LcdShowCaption(s_stTaskAttrib.ShowBuffer[0],0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("ȷ��ѡ,����ȡ��",47);
            if(s_stTaskAttrib.ControlId<2) 
            {
                LcdShowButton(" �� ", 16, 23, s_stTaskAttrib.ControlId);
                LcdShowButton(" �� ", 80, 23, 1-s_stTaskAttrib.ControlId);
            }
            else
            {
                LcdShowButton(" �� ", 16, 23, 1);
                LcdShowButton(" �� ", 80, 23, 1);
            }
            //////////////////////////////
            if(s_stTaskAttrib.timeVal)
            {
                LZM_SetAlarm(TEST_TIMER, s_stTaskAttrib.timeVal);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff&&TaskPara!=KEY_BACK)break;
            if(s_stTaskAttrib.timeVal)
            {
                LZM_SetAlarm(TEST_TIMER, s_stTaskAttrib.timeVal);
            }
            switch(TaskPara)
            {
                case KEY_BACK://  
                    PublicMsgBoxCancel();
                    break;
                case KEY_ENTER://
                    if(s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId=0xff;
                        if(s_fptrDoit)
                        {
                            s_fptrDoit();
                        }
                    }
                    else
                    {
                        PublicMsgBoxCancel();
                    }
                    break;
                case KEY_DOWN:
                case KEY_UP:
                    if(s_stTaskAttrib.ControlId<2)
                    {
                        s_stTaskAttrib.ControlId^=1;
                        LcdShowButton(" �� ", 16, 23, s_stTaskAttrib.ControlId);
                        LcdShowButton(" �� ", 80, 23, 1-s_stTaskAttrib.ControlId);
                    }
                    break;
            }
            return 1;
        case TEST_TIMER:
            PublicMsgBoxCancel();
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************OK
** ��������: PublicConfirm(...)
** ��������: 
** ��ڲ���: caption:��ʾ��Ϣ
            doit:ȷ��ִ�еĲ���
            Cancel:ȡ����ִ�еĲ���
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PublicConfirmAll(char *caption,void (*doit)(void),void (*Cancel)(void),unsigned long time,unsigned char select)
{
    LZM_KillTimer(TEST_TIMER);
    strcpy(s_stTaskAttrib.ShowBuffer[0],caption);   
    s_stTaskAttrib.timeVal = time;
    ////////////////////
    if(doit) s_fptrDoit=doit;
    else s_fptrDoit=NULL;
    /////////////////////////
    if(Cancel)s_fptrCancel=Cancel;
    else s_fptrCancel=NULL;
    ///////////////////////
    s_stTaskAttrib.select=select;
    PasswordPreTaskFunction = LZM_TaskFunction;
    LZM_RefuseNowTask(TaskPublicMsgBox);
}

/*************************************************************OK
** ��������: PublicConfirm(...)
** ��������: 
** ��ڲ���: caption:��ʾ��Ϣ
            doit:ȷ��ִ�еĲ���
            Cancel:ȡ����ִ�еĲ���
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PublicConfirmAutoExit(char *caption,void (*doit)(void),unsigned long time)
{
    PublicConfirmAll(caption,doit,NULL,time,0);
}
/*************************************************************OK
** ��������: PublicConfirm(...)
** ��������: 
** ��ڲ���: caption:��ʾ��Ϣ
            doit:ȷ��ִ�еĲ���
            Cancel:ȡ����ִ�еĲ���
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PublicConfirm(char *caption,void (*doit)(void))
{
    PublicConfirmAll(caption,doit,NULL,0,0);
}
/*************************************************************
** ��������: ShowTextInfoIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
/////////////////////////////////////////////////////////////
//////////����Ϊ�˵�///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/*************************************************************
** ��������: GetCurrentTextIndexTotalNum
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char GetCurrentTextIndexTotalNum(void)
{
  return DownloadGB2313_GetTotalStep();//��ȡ������//modify by joneming 20130312
  
}
/*************************************************************
** ��������: ShowTextInfoIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char ReadIndexTextDetails(u8 Num, u8 *pBuffer)
{
    return DownloadGB2313_ReadFromFlash(Num, pBuffer);////modify by joneming 20130312

}
/*************************************************************
** ��������: ShowTextInfoIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowTextInfoIndex(unsigned char index)
{
    static unsigned char sucPage=0xff;
    unsigned char start,max,flag;
    unsigned char i,j;
    unsigned short length;
    ///////////////////////////
    if(index==0xff)
    {
        sucPage=index;
        return;
    }
    //////////
    start=index/4;
    flag=0;
    if(start!=sucPage)
    {
        sucPage=start;
        flag=1;
    }
    ///////////////
    start=sucPage*4;
    max=GetCurrentTextIndexTotalNum();//��ȡ������
    /////////////////
    if(max<=start)return;
    max-=start;
    ///////////////////////
    if(max>4)max=4;
    LcdClearScreen();
    /////////////////////////
    for(i=0; i<max; i++)
    {
        if(flag)//
        {
            //������Ϣ���
            sprintf((char *)GB2312Buffer,"%02d.",start+1);
            length = ReadIndexTextDetails(start+1, GB2312Buffer+3);
            if(length < 13)
            {
                GB2312Buffer[length+3]='\0';
            }
            else
            {
                for(j=3;;)
                {
                    if (GB2312Buffer[j] > 0x80)//
                    {
                        j+=2;
                    }
                    else//
                    {
                        j++;
                    }
                    ///////////////////////
                    if(j==16)
                    {
                        GB2312Buffer[16]='\0';
                        break;
                    }
                    else///////////////////
                    if(j>16)
                    {
                        GB2312Buffer[15]='\0';
                        break;
                    }
                }                
            }
            /////////////////////////
            strcpy(s_stTaskAttrib.ShowBuffer[i],(char *)GB2312Buffer);
        }
        //////////////////////////////

        if(start == index)
        {
            LcdShowStr(0, i*16, s_stTaskAttrib.ShowBuffer[i], 1);
        }
        else
        {
            LcdShowStr(0, i*16, s_stTaskAttrib.ShowBuffer[i], 0);
        }
        start++;
    }
}
/*************************************************************
** ��������: ShowTextInfoDetails
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowTextInfoDetails(unsigned char index)
{
    unsigned short length;
    length = ReadIndexTextDetails(index+1, GB2312Buffer);
    SystemAutoShowText(length,GB2312Buffer);
}
/*************************************************************
** ��������: TaskShowTextInfo
** ��������: "��ʾ�ı���Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowTextInfo(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:           
            s_stTaskAttrib.ControlId=0;
            temp=GetCurrentTextIndexTotalNum();
        //  temp=1;

            if(temp==0)
            {
                LcdClearScreen();
                sucMaxCount=0;
                sucStep=0;
                s_stTaskAttrib.ControlId=0xff;
                LcdShowCaption("���ı���Ϣ!", 23);
                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =0;
                }
                sucMaxCount=temp;
                ShowTextInfoIndex(0xff);
                ShowTextInfoIndex(sucStep);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    ShowTextInfoDetails(sucStep);
                    break;                                
                case KEY_UP://--    
                    if(sucStep>0)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount-1;
                    }
                    ShowTextInfoIndex(sucStep);
                    break;
                case KEY_DOWN://++  
                    if(sucStep<sucMaxCount-1)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=0;
                    }
                    ShowTextInfoIndex(sucStep);
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowQuestionListIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowQuestionListIndex(unsigned char index,unsigned char maxcount)
{
    static unsigned char sucPage=0xff;
    unsigned char start,max,flag;
    unsigned char i,j;
    unsigned short length;
    ///////////////////////////
    if(index==0xff)
    {
        sucPage=index;
        return;
    }
    //////////
    start=index/4;
    flag=0;
    if(start!=sucPage)
    {
        sucPage=start;
        flag=1;
    }
    ///////////////
    start=sucPage*4;
    max=maxcount;//��ȡ������
    /////////////////
    if(max<=start)return;
    max-=start;
    ///////////////////////
    if(max>4)max=4;
    LcdClearScreen();
    /////////////////////////
    for(i=0; i<max; i++)
    {
        if(flag)//
        {
            //������Ϣ���
            GB2312Buffer[0] = start+0x31;
            length = QuestionResponse_ReadOneData(start+1, GB2312Buffer+1);            
            GB2312Buffer[1] = '.';
            if(length <= 14)
            {
                GB2312Buffer[length+2]='\0';
            }
            else
            {
                for(j=2;;)
                {
                    if (GB2312Buffer[j] > 0x80)//
                    {
                        j+=2;
                    }
                    else//
                    {
                        j++;
                    }
                    ///////////////////////
                    if(j==16)
                    {
                        GB2312Buffer[16]='\0';
                        break;
                    }
                    else///////////////////
                    if(j>16)
                    {
                        GB2312Buffer[15]='\0';
                        break;
                    }
                }                
            }
            /////////////////////////
            strcpy(s_stTaskAttrib.ShowBuffer[i],(char *)GB2312Buffer);
        }
        //////////////////////////////

        if(start == index)
        {
            LcdShowStr(0, i*16, s_stTaskAttrib.ShowBuffer[i], 1);
        }
        else
        {
            LcdShowStr(0, i*16, s_stTaskAttrib.ShowBuffer[i], 0);
        }
        start++;
    }
}
/*************************************************************
** ��������: ShowQuestionListDetails
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowQuestionListDetails(unsigned char index)
{
    unsigned char flag;
    unsigned short length;
    flag =0;
    length = QuestionResponse_ReadOneData(index+1, RadioShareBuffer);
    PUBLIC_SETBIT(flag, 4);
    //PUBLIC_SETBIT(flag, 3);
    SystemAutoShowQuestionResponse(length,RadioShareBuffer,flag);
}
/*************************************************************
** ��������: TaskShowTextInfo
** ��������: "��ʾ�ı���Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowQuestionList(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:           
            s_stTaskAttrib.ControlId=0;
            temp=QuestionResponse_GetTotalNum();
            if(temp==0)
            {
                LcdClearScreen();
                sucMaxCount=0;
                sucStep=0;
                s_stTaskAttrib.ControlId=0xff;
                LcdShowCaption("������Ӧ������!", 23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =0;
                }
                sucMaxCount=temp;
                ShowQuestionListIndex(0xff,sucMaxCount);
                ShowQuestionListIndex(sucStep,sucMaxCount);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    ShowQuestionListDetails(sucStep);
                    break;                                
                case KEY_UP://--    
                    if(sucStep>0)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount-1;
                    }
                    ShowQuestionListIndex(sucStep,sucMaxCount);
                    break;
                case KEY_DOWN://++  
                    if(sucStep<sucMaxCount-1)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=0;
                    }
                    ShowQuestionListIndex(sucStep,sucMaxCount);
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowTextInfoIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowInfoServiceIndex(unsigned char index,unsigned char maxcount)
{
    static unsigned char sucPage=0xff;
    unsigned char start,max,flag;
    unsigned char i,j;
    unsigned short length;
    ///////////////////////////
    if(index==0xff)
    {
        sucPage=index;
        return;
    }
    //////////
    start=index/4;
    flag=0;
    if(start!=sucPage)
    {
        sucPage=start;
        flag=1;
    }
    ///////////////
    start=sucPage*4;
    max=maxcount;//��ȡ������
    /////////////////
    if(max<=start)return;
    max-=start;
    ///////////////////////
    if(max>4)max=4;
    LcdClearScreen();
    /////////////////////////
    for(i=0; i<max; i++)
    {
        if(flag)//
        {
            //������Ϣ���
            sprintf((char *)GB2312Buffer,"%d.",start+1);
            length = InfoService_ReadOneData(start+1, GB2312Buffer+2);
            if(length <= 14)
            {
                GB2312Buffer[length+2]='\0';
            }
            else
            {
                for(j=2;;)
                {
                    if (GB2312Buffer[j] > 0x80)//
                    {
                        j+=2;
                    }
                    else//
                    {
                        j++;
                    }
                    ///////////////////////
                    if(j==16)
                    {
                        GB2312Buffer[16]='\0';
                        break;
                    }
                    else///////////////////
                    if(j>16)
                    {
                        GB2312Buffer[15]='\0';
                        break;
                    }
                }                
            }
            /////////////////////////
            strcpy(s_stTaskAttrib.ShowBuffer[i],(char *)GB2312Buffer);
        }
        //////////////////////////////

        if(start == index)
        {
            LcdShowStr(0, i*16, s_stTaskAttrib.ShowBuffer[i], 1);
        }
        else
        {
            LcdShowStr(0, i*16, s_stTaskAttrib.ShowBuffer[i], 0);
        }
        start++;
    }
}
/*************************************************************
** ��������: ShowTextInfoDetails
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowInfoServiceDetails(unsigned char index)
{
    unsigned short length;
    length = InfoService_ReadOneData(index+1, GB2312Buffer);
    SystemAutoShowText(length,GB2312Buffer);
}
/*************************************************************
** ��������: TaskShowTextInfo
** ��������: "��ʾ�ı���Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowInfoService(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:           
            s_stTaskAttrib.ControlId=0;
            temp=InfoService_GetTotalNum();
            if(temp==0)
            {
                LcdClearScreen();
                sucMaxCount=0;
                sucStep=0;
                s_stTaskAttrib.ControlId=0xff;
                LcdShowCaption("�޵㲥��Ϣ����!", 23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =0;
                }
                sucMaxCount=temp;
                ShowInfoServiceIndex(0xff,sucMaxCount);
                ShowInfoServiceIndex(sucStep,sucMaxCount);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    ShowInfoServiceDetails(sucStep);
                    break;                                
                case KEY_UP://--    
                    if(sucStep>0)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount-1;
                    }
                    ShowInfoServiceIndex(sucStep,sucMaxCount);
                    break;
                case KEY_DOWN://++  
                    if(sucStep<sucMaxCount-1)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=0;
                    }
                    ShowInfoServiceIndex(sucStep,sucMaxCount);
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowEventReportInfoIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowEventReportInfoIndex(unsigned char index,unsigned char maxcount)
{
    static unsigned char sucPage=0xff;
    STMESSAGE stEvent;
    unsigned char start,max,flag;
    unsigned char i;
    ///////////////////////////
    if(index==0xff)
    {
        sucPage=index;
        return;
    }
    //////////
    start=index/4;
    flag=0;
    if(start!=sucPage)
    {
        sucPage=start;
        flag=1;
    }
    ///////////////
    start=sucPage*4;
    max=maxcount;//��ȡ������
    /////////////////
    if(max<=start)return;
    max-=start;
    ///////////////////////
    if(max>4)max=4;
    LcdClearScreen();
    /////////////////////////
    for(i=0; i<max; i++)
    {
        if(flag)//
        {
            GetOneEventReportDetails(start,&stEvent);
            stEvent.content[15]='\0';
            /////////////////////////
            strcpy(s_stTaskAttrib.ShowBuffer[i],(char *)stEvent.content);
            s_stTaskAttrib.ID[i] = stEvent.ID;
        }
        /////////////////////
        LcdShowStr(8, i*16, s_stTaskAttrib.ShowBuffer[i], 0);
        if(start == index)
        {
            LcdShowArrow(6,i*16+7,2,5,0x01);
        }
        start++;
    }
}
/*************************************************************
** ��������: EventReportInfoSendToPlatform
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void EventReportInfoSendToPlatform(unsigned char index)
{
    ProtocolACK ack_ok;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    ack_ok=RadioProtocol_EvenReport(channel,s_stTaskAttrib.ID[index%4]);
    if(ACK_OK == ack_ok)
    {
        Public_ShowTextInfo("���ͳɹ�",PUBLICSECS(2));
    }
    else
    {
        Public_ShowTextInfo("����ʧ��",PUBLICSECS(2));
    }    
}
/*************************************************************
** ��������: TaskShowEventReport
** ��������: "��ʾ�¼�������Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowEventReport(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:           
            s_stTaskAttrib.ControlId=0;
            temp=GetEventReportTotalNum();
            if(temp==0)
            {
                LcdClearScreen();
                sucMaxCount=0;
                sucStep=0;
                s_stTaskAttrib.ControlId=0xff;
                LcdShowCaption("û�¼�����!", 23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =0;
                }
                sucMaxCount=temp;
                ShowEventReportInfoIndex(0xff,sucMaxCount);
                ShowEventReportInfoIndex(sucStep,sucMaxCount);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    EventReportInfoSendToPlatform(sucStep);
                    break;                                
                case KEY_UP://--    
                    if(sucStep>0)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount-1;
                    }
                    ShowEventReportInfoIndex(sucStep,sucMaxCount);
                    break;
                case KEY_DOWN://++  
                    if(sucStep<sucMaxCount-1)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=0;
                    }
                    ShowEventReportInfoIndex(sucStep,sucMaxCount);
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowEventReportInfoIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowInformationDemandIndex(unsigned char index,unsigned char maxcount)
{
    static unsigned char sucPage=0xff;
    STMESSAGE stInfo;
    unsigned char start,max,flag;
    unsigned char i;
    ///////////////////////////
    if(index==0xff)
    {
        sucPage=index;
        return;
    }
    //////////
    start=index/4;
    flag=0;
    if(start!=sucPage)
    {
        sucPage=start;
        flag=1;
    }
    ///////////////
    start=sucPage*4;
    max=maxcount;//��ȡ������
    /////////////////
    if(max<=start)return;
    max-=start;
    ///////////////////////
    if(max>4)max=4;
    LcdClearScreen();
    /////////////////////////
    for(i=0; i<max; i++)
    {
        if(flag)//
        {
            GetOneInfoDemandDetails(start,&stInfo);
            stInfo.content[15]='\0';
            /////////////////////////
            strcpy(s_stTaskAttrib.ShowBuffer[i],(char *)stInfo.content);
            s_stTaskAttrib.ID[i] = stInfo.ID;
            s_stTaskAttrib.status[i] = stInfo.status;
        }
        //////////////////////////////
        LcdShowStr(8, i*16, s_stTaskAttrib.ShowBuffer[i], s_stTaskAttrib.status[i]);
        if(start == index)
        {
            LcdShowArrow(6,i*16+7,2,5,0x01);
        }
        start++;
    }
    
}
/*************************************************************
** ��������: EventReportInfoSendToPlatform
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void InformationDemandconfirm(void)
{
    ProtocolACK val_ack;
    unsigned char index;    
    unsigned char status;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    
    index = s_stTaskAttrib.index;
    status = s_stTaskAttrib.status[index]^1;
    val_ack=RadioProtocol_InforamtionOrder(channel,s_stTaskAttrib.ID[index],status);
    if(ACK_OK==val_ack)
    {
        //////////������Ϣ�㲥״̬//////////////
        s_stTaskAttrib.status[index]=status;
        InfoDemandChangeStatus(s_stTaskAttrib.ID[index],status);
        Public_ShowTextInfo("���ͳɹ�",PUBLICSECS(3));
    }
    else
    {
        Public_ShowTextInfo("����ʧ��",PUBLICSECS(3));
    }
    ///////////////////////////
    LZM_SetAlarm(TEST_TIMER, SECS(2));
}
/*************************************************************
** ��������: EventReportInfoSendToPlatform
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void InformationDemandSendToPlatform(unsigned char index)
{
    s_stTaskAttrib.index=index%4;
    if(s_stTaskAttrib.status[s_stTaskAttrib.index])
    {
        PublicConfirm("�ѵ㲥,�Ƿ�ȡ��?",InformationDemandconfirm);
    }
    else
    {
        PublicConfirm("�Ƿ�ȷ���㲥?",InformationDemandconfirm);
    }
}
/*************************************************************
** ��������: TaskShowEventReport
** ��������: "��ʾ�¼�������Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowInformationDemand(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:           
            s_stTaskAttrib.ControlId=0;
            temp=GetInfoDemandTotalNum();
            if(temp==0)
            {
                LcdClearScreen();
                sucMaxCount=0;
                sucStep=0;
                s_stTaskAttrib.ControlId=0xff;
                LcdShowCaption("û��Ϣ�㲥!", 23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =0;
                }
                sucMaxCount=temp;
                ShowInformationDemandIndex(0xff,sucMaxCount);
                ShowInformationDemandIndex(sucStep,sucMaxCount);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    InformationDemandSendToPlatform(sucStep);
                    break;                                
                case KEY_UP://--    
                    if(sucStep>0)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount-1;
                    }
                    ShowInformationDemandIndex(sucStep,sucMaxCount);
                    break;
                case KEY_DOWN://++  
                    if(sucStep<sucMaxCount-1)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=0;
                    }
                    ShowInformationDemandIndex(sucStep,sucMaxCount);
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowShortMessage
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void SendShortMessage(unsigned char index)
{
    unsigned char lenght;
    char buffer[50];
    ProtocolACK ACK_val;
    u8 channel = CHANNEL_SMS;
    
    strcpy(buffer,s_apcMenuShortMsg[index]);
    lenght=strlen(buffer);
    ACK_val=RadioProtocol_OriginalDataUpTrans(channel,4,(unsigned char *)buffer,lenght);
    LcdClearScreen();
    ///////////////////////////
    if(ACK_OK == ACK_val)
    {
        //��ʾ���ͳɹ�
        LcdShowCaption("���ͳɹ�", 0);
    }
    else
    {
        //��ʾ����ʧ��
        LcdShowCaption("����ʧ��", 0);
        
    }
}
/*************************************************************
** ��������: TaskShowShortMessage
** ��������: "Ԥ�ö���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowShortMessage(LZM_MESSAGE *Msg)
{
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuShortMsg,s_apcMenuShortMsg);
            MenuShow(&s_sutMenuShortMsg);
            s_stTaskAttrib.ControlId =0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����                    
                    break;
                case KEY_ENTER:
                    handle=MenuGetHandle(&s_sutMenuShortMsg); 
                    s_stTaskAttrib.ControlId = 1;
                    SendShortMessage(handle);
                    LZM_SetAlarm(TEST_TIMER, SECS(2));
                    break;
                default:
                    MenuResponse(&s_sutMenuShortMsg,TaskPara);
                    break;
            }
            return 1;        
        case TEST_TIMER:
            s_stTaskAttrib.ControlId =0;
            MenuShow(&s_sutMenuShortMsg);
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowTiredDriveRecordInit
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char ShowTiredDriveRecordInit(void)
{
    unsigned short length=0,maxcount;
    unsigned long TimeCount;
    TIME_T  CurrentTime;
    
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
    length = VDROvertime_Get2daysLog(GB2312Buffer);
    ////////////////////////
    //��ʾ��ʱ��ʻ��¼
    if(0 == length)//û��һ����¼,��ʾ"�޼�¼"
    {
        LcdShowCaption("�޼�¼", 23);
        return 0xff;
    }
    ///////////////////
    ///////////////////
    maxcount = length/VDR_DATA_OVER_TIME_COLLECT_LEN;
    maxcount <<=1;//һ����¼�ֿ���ҳ��ʾ
    /////////////////////
    return maxcount;   
}
/*************************************************************
** ��������: TaskShowTiredDriveRecord
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowTiredDriveRecord(unsigned char page)
{
    char Buffer[40];
    unsigned char data[20];
    unsigned short index,len;
    unsigned char recordNum;
    unsigned char temp;
    TIME_T stTime;
    temp=page%2;
    recordNum = page/2;
    index = recordNum*VDR_DATA_OVER_TIME_COLLECT_LEN;
    LcdClearScreen();
    if(0 == temp)
    {
        sprintf(Buffer,"��¼%d:",recordNum+1);
        LcdShowStr(0, 0,  Buffer, 0);
        LcdShowStr(0, 16,"��ʻ֤����:", 0);
        len = 18;
        memcpy(Buffer,&GB2312Buffer[index],len);
        Buffer[len] ='\0';
        LcdShowStr(0, 32,  Buffer, 0);
    }
    else
    {
        index += 18;
        memcpy(data,&GB2312Buffer[index],12);
        Public_ConvertBCDToTime(&stTime,data);
        sprintf(Buffer,"��ʼʱ��: 20%02d��  %02d-%02d %02d:%02d:%02d",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
        LcdShowStr(0, 0,  Buffer, 0);
        //��������ʾ"����ʱ��"
        Public_ConvertBCDToTime(&stTime,&data[6]);      
        sprintf(Buffer,"����ʱ��: 20%02d��  %02d-%02d %02d:%02d:%02d",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
        LcdShowStr(0, 32,  Buffer, 0);
    }    
}
/*************************************************************
** ��������: TaskShowTiredDriveRecord
** ��������: "��ʱ��ʻ��¼"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowTiredDriveRecord(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            s_stTaskAttrib.ControlMaxId=ShowTiredDriveRecordInit();
            if(s_stTaskAttrib.ControlMaxId==0xff)
            {
                s_stTaskAttrib.ControlId=0xff;                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                ShowTiredDriveRecord(s_stTaskAttrib.ControlId);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��   
                    //LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;                                
                case KEY_UP://��¼  
                    if(s_stTaskAttrib.ControlId>0)
                    {
                        s_stTaskAttrib.ControlId--;
                        ShowTiredDriveRecord(s_stTaskAttrib.ControlId);
                    }
                    break;
                case KEY_DOWN://++  
                    if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId-1)
                    {
                        s_stTaskAttrib.ControlId++;
                        ShowTiredDriveRecord(s_stTaskAttrib.ControlId);
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowTiredDriveRecordInit
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char Show15MinSpeedInit(void)
{
    unsigned char maxcount;
    unsigned char length;
    //��ȡ15�����ٶ�
    length = VDRSpeed_Get15MinuteSpeed(GB2312Buffer);
    ////////////////////////
    maxcount = (length-6+2)/3+1;
    //////////////////////////
    return maxcount;
}
/*************************************************************
** ��������: TaskShowTiredDriveRecord
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void Show15MinSpeed(unsigned char page)
{
    TIME_T stEndTime;
    unsigned char i,y,index,start;
    char buffer[40];
    //��ʾ15����ͣ���ٶ�
    LcdClearScreen();
    if(0 == page)
    {
        Public_ConvertBCDToTime(&stEndTime,&GB2312Buffer[0]);
        sprintf(buffer,"ͣ��ʱ��:20%02d��",stEndTime.year);
        LcdShowStr(0, 0, buffer, 0);
        sprintf(buffer,"%02d-%02d %02d:%02d:%02d",stEndTime.month,stEndTime.day,stEndTime.hour,stEndTime.min,stEndTime.sec);
        LcdShowCaption(buffer, 16);
        y=32;
        index=0;
        
    }
    else
    {
        y = 0;
        index=(page-1)*4+2;
    }  
    ///////////
    start = index*3+6;
    //////////////////
    for(i=0; i<4; i++)
    {               
        sprintf(buffer,"%02d:%02d %3dkm/h",Public_CheckStrIsBCD(GB2312Buffer[start+0]),Public_CheckStrIsBCD(GB2312Buffer[start+1]),GB2312Buffer[start+2]);
        LcdShowCaption(buffer, y);
        index++;
        start +=3;
        y+=16;
        if(index>=s_stTaskAttrib.ControlMaxId-1)
        {
            break;
        }
        if(y>64)
        {
            break;
        }
    }
}
/*************************************************************
** ��������: TaskShow15MinSpeed
** ��������: "15�����ٶ�"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShow15MinSpeed(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.ControlId = 0;
            s_stTaskAttrib.ControlMaxId = Show15MinSpeedInit();
            if(s_stTaskAttrib.ControlMaxId==0xff)
            {
                s_stTaskAttrib.ControlId = 0xff;
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                Show15MinSpeed(s_stTaskAttrib.ControlId);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://��ӡ
                    LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
                    break;
                case KEY_ENTER://�˵�
                    break;                                
                case KEY_UP://��¼  
                    if(s_stTaskAttrib.ControlId>0)
                    {
                        s_stTaskAttrib.ControlId--;
                        Show15MinSpeed(s_stTaskAttrib.ControlId);
                    }
                    break;
                case KEY_DOWN://++  
                    if(s_stTaskAttrib.ControlId*4<s_stTaskAttrib.ControlMaxId-1)
                    {
                        s_stTaskAttrib.ControlId++;
                        Show15MinSpeed(s_stTaskAttrib.ControlId);
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowDriveMile
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowDriveMile(void)
{
    unsigned long Mileage;
    char buffer[17];
    Mileage = Public_GetCurTotalMile();   //��ȡ��ǰ�����    
    LcdShowCaption("�����", 1);
    LcdShowHorLine(0, 131, 20, 1);
    sprintf(buffer,"%.1f����",Mileage*0.1);
    LcdShowCaption(buffer,25); 
    LcdShowHorLine(0, 131, 45, 1);
    LcdShowCaption("��<����>�˳�", 47);
}
/*************************************************************
** ��������: TaskShowDriveMile
** ��������: "��ʻ���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowDriveMile(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            ShowDriveMile();
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    //LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;                                
                case KEY_UP://--
                    break;
                case KEY_DOWN://++
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowDriveMile
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowCarInfo(unsigned char page)
{
    const char *c_apcExtCarStatusItems[]=
    {
        "�����",
        "Զ���",
        "��ת���",
        "��ת���",
        "�ƶ�",
        ""
    };
    unsigned char Buffer[24];
    unsigned char i;
    unsigned char temp;
    unsigned char SpeedSensorCoeff;
    unsigned long ulTmp;
    LcdClearScreen();
    //��ʾ
    if(0 == page)
    {
        //��һ����ʾ"���ƺ���",���12�ֽ�        
        LcdShowStr(0, 0, "���ƺ�:", 0);
        //�ڶ�����ʾ����ĳ��ƺ���
        temp = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, Buffer);
        for(i=0; i<temp; i++)
        {
            if(0 != Buffer[i])
            {
                break;
            }
        }
        Buffer[temp]='\0';        
        //��һ��ʣ��ռ���ʾ�ո�
        LcdShowStr(60, 0, (char *)Buffer+i, 0); 
        //��������ʾ"����VIN",���17�ֽ�
        LcdShowStr(0, 16, "������ɫ:", 0);
        temp = EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, Buffer);
        if(temp)
        {
            temp=Buffer[0];
            if(9==temp)
            {
                LcdShowStr(80, 16, c_apcCarPlakeColorItems[4], 0);    
            }
            else
            if(temp&&temp<5)
            {
                LcdShowStr(80, 16, c_apcCarPlakeColorItems[temp-1], 0);                
            }
            else
            if(0==temp)
            {
                LcdShowStr(80, 16, c_apcCarPlakeColorItems[5], 0);   
            }
        }
        
        LcdShowStr(0, 32, "���Ʒ���: ", 0);
        //��������ʾ�����VIN
        temp=EepromPram_ReadPram(E2_CAR_TYPE_ID, Buffer);
        Buffer[temp]='\0';
        LcdShowStr(0, 48, (char *)Buffer, 0);
    }
    else
    if(1 == page)
    {
        LcdShowStr(0, 0, "����VIN��:", 0);        
        i=EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID,(unsigned char*)Buffer);
        if(i&&Buffer[0]>=0x30&&Buffer[0]<0x80)
        {
            Buffer[17]='\0';
            LcdShowStr(0, 16,(char *)Buffer, 0);
        }
    }
    else
    if(2 == page)
    {
        //��һ����ʾ"����ϵ��"
        LcdShowStr(0, 0, "����ϵ��:", 0);
        //�ڶ�����ʾ���������ϵ�� 
        EepromPram_ReadPram(E2_SPEED_SENSOR_COEF_ID, &SpeedSensorCoeff);
        temp = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, Buffer);
        ulTmp = 0;
        for(i = 0; i < temp; i++)
        {
            ulTmp <<= 8;
            ulTmp |= Buffer[i];
        }
        ///////////////////
        ulTmp = ulTmp;
        sprintf((char *)Buffer,"%dת/����",ulTmp);
        LcdShowStr(0, 16,(char *)Buffer, 0);
        //��������ʾ"��������"
        LcdShowStr(0, 32, "��������:", 0);
        //��������ʾ����ı�������
        temp = VDROverspeed_GetAlarmSpeed();
        sprintf((char *)Buffer,"%dkm/h",temp);
        LcdShowStr(0, 48,(char *)Buffer, 0);
    }
    else
    {
        LcdShowCaption("��ǰ״̬�ź�ֵ:",0);
        temp = 0;
        for(i=0; i<5;i++)
        {
            if(Io_ReadExtCarStatusBit((CAR_STATUS_BIT)i))
            {
                LcdShowStr(0+64*(temp%2), 16+16*(temp/2),c_apcExtCarStatusItems[i], 0);
                temp++;
            }
        }
    }
}
/*************************************************************
** ��������: TaskShowCarInfo
** ��������: "������Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowCarInfo(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            s_stTaskAttrib.ControlId=0;
            s_stTaskAttrib.ControlMaxId=3;
            ShowCarInfo(s_stTaskAttrib.ControlId);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��   
                    //LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;                               
                case KEY_UP://--    
                    if(s_stTaskAttrib.ControlId>0)
                    {
                        s_stTaskAttrib.ControlId--;
                        ShowCarInfo(s_stTaskAttrib.ControlId);
                    }
                    break;
                case KEY_DOWN://++  
                    if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)
                    {
                        s_stTaskAttrib.ControlId++;
                        ShowCarInfo(s_stTaskAttrib.ControlId);
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#if 0
////////////////////////
extern void ICCard_GetCurDriverInfo(unsigned char index ,unsigned char *buffer);
/*************************************************************
** ��������: ShowDriveMile
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowDriverInfo(unsigned char index)
{
    unsigned char buffer[30];
    //////////////////////
    LcdClearScreen();
    if(0 == TiredDrive_GetLoginFlag())//û��½
    {
        s_stTaskAttrib.ControlMaxId = 0;
        LcdShowCaption("û�е�¼!", 23);
        return;
    }    
    switch(index)
    {
        case 0:
        {
            LcdShowCaption("��ҵ�ʸ�֤��:",0);            
            break;
        }
        case 1:
        {
            LcdShowCaption("���֤",0);
            break;
        }
        case 2:
        {
            LcdShowCaption("����:",0);           
            break;
        }
        case 3:
        {
            LcdShowCaption("��֤����:",0);           
            break;
        }
        default:
            return;
    }
    ICCard_GetCurDriverInfo(index,buffer);
    LcdShowHorLine(0, 131, 17, 1);
    LcdShowStr(0,20,(char *)buffer,0);
}
/*************************************************************
** ��������: TaskShowDriverInfo
** ��������: "��ʻԱ��Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowDriverInfo(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            s_stTaskAttrib.ControlId=0;
            s_stTaskAttrib.ControlMaxId=3;            
            ShowDriverInfo(s_stTaskAttrib.ControlId);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��   
                    //LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;                               
                case KEY_UP://--    
                    if(s_stTaskAttrib.ControlId>0)
                    {
                        s_stTaskAttrib.ControlId--;
                        ShowDriverInfo(s_stTaskAttrib.ControlId);
                    }
                    break;
                case KEY_DOWN://++  
                    if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)
                    {
                        s_stTaskAttrib.ControlId++;
                        ShowDriverInfo(s_stTaskAttrib.ControlId);
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif
/*************************************************************
** ��������: ShowDriveMile
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowDriverInfo(void)
{
    unsigned char buffer[30];
    //////////////////////
    if(0 == VDROvertime_GetLoginFlag())
    {
        LcdShowCaption("û�е�¼!", 23);
        return;
    }
    /////////////////////////////
    VDROvertime_GetLicense(buffer);
    buffer[18] = 0;
    LcdShowCaption("��ʻ֤����:",0);
    LcdShowHorLine(0, 131, 17, 1);
    LcdShowStr(0,20,(char *)buffer,0);
    
    #if 0
    len =EepromPram_ReadPram(E2_ALL_DRIVER_INFORMATION_ID, buffer);
    if(len) //�����ȡ�ɹ�
    {
        if(0x10==buffer[0])//������ʻ��¼��
        {
            STRUCT_IC_CARD_DRIVER stTmpICDriver;
            /*** EEPROM�ڱ������б�IC���������ʽ������������ֽ�����ȡ����ҵ�ʸ�֤IC�����ݵ�gCardRoadTransport ��Ӧ���� ***/       
            ICCard_Pack_Extract_VehicleRecorder_Driver ((STRUCT_PROTOCOL_IC_CARD *)buffer,len , &stTmpICDriver);
            len =0;
            memcpy(&buffer[len],stTmpICDriver.IC_Card_DriverID,18);
            len += 18;
            buffer[len++]=' ';
            temp[0]=Public_CheckStrIsBCD(stTmpICDriver.IC_Card_ValidityPeriod_Year);
            temp[1]=Public_CheckStrIsBCD(stTmpICDriver.IC_Card_ValidityPeriod_Month);
            temp[2]=Public_CheckStrIsBCD(stTmpICDriver.IC_Card_ValidityPeriod_Day);
            sprintf((char *)&buffer[len],"%02d-%2d-%2d",temp[0],temp[1],temp[2]);
            //////////////////////////
            LcdShowStr(0,0,(char *)buffer,0);
        }
        else
        if(0x20==buffer[0])//IC����ҵ�ʸ�֤��
        {
            STRUCT_IC_CARD_ROADTRANSPORT stTmpICard;
            /*** EEPROM�ڱ������б�IC���������ʽ������������ֽ�����ȡ����ҵ�ʸ�֤IC�����ݵ�gCardRoadTransport ��Ӧ���� ***/       
            ICCard_Pack_Extract_RoadTransportCard(buffer, len, &stTmpICard);
            len =0;
            memcpy(&buffer[len],stTmpICard.IC_Card_DriverName,stTmpICard.IC_Card_DriverNameLength);
            len += stTmpICard.IC_Card_DriverNameLength;
            buffer[len++]=' ';
            memcpy(&buffer[len],stTmpICard.IC_Card_QualificationCertificateID,18);
            len += 18;
            buffer[len++]=' ';
            memcpy(&buffer[len],stTmpICard.IC_Card_IssueOrganization,stTmpICard.IC_Card_IssueOrganizationLength);
            len += stTmpICard.IC_Card_IssueOrganizationLength;
            //////////////////////
            if(len > 53)len = 53;
            //////////////////////
            buffer[len++]=' ';
            ////////////////////////////
            temp[0]=Public_CheckStrIsBCD(stTmpICard.IC_Card_ValidityPeriod_Year[0]);
            temp[1]=Public_CheckStrIsBCD(stTmpICard.IC_Card_ValidityPeriod_Year[1]);
            temp[2]=Public_CheckStrIsBCD(stTmpICard.IC_Card_ValidityPeriod_Month);
            temp[3]=Public_CheckStrIsBCD(stTmpICard.IC_Card_ValidityPeriod_Day);
            sprintf((char *)&buffer[len],"%02d%02d-%2d-%2d",temp[0],temp[1],temp[2],temp[3]);
            //////////////////////////
            LcdShowStr(0,0,(char *)buffer,0);
        }
        else
        {
            LcdShowCaption("����δ����!", 23);
        }
    }
    else
    {
        LcdShowCaption("���ݳ���!", 23);
    } 
    #endif
}
/*************************************************************
** ��������: TaskShowDriverInfo
** ��������: "��ʻԱ��Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowDriverInfo(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            ShowDriverInfo();
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuDriveRecord);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��   
                    //LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;                               
                case KEY_UP://--
                    break;
                case KEY_DOWN://++
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowProductInfo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
** �޸�ʱ��: 2015.07.20 
** �޸���  : fanqinghai

*************************************************************/
void ShowProductInfo(unsigned char page)
{
    unsigned char i,index,max;
    char Buffer[20];
    unsigned char buff[10];
    unsigned char len,temp;
    index=page*4;
    LcdClearScreen();
    //??
    max=0;
    if(PRODUCT_ITEMS>index)
    {
        max=(PRODUCT_ITEMS-index);
    }
    //////////////
    if(max>4)max=4;
    /////////////
    for(i=0; i<max; i++)
    {
        if(3==index)
        {
            char name[20];
            GetCurentProductModel(name);
            LcdShowStr(0, i*16,name, 0);
            ///////////////////
            index++;
        }
        #ifdef __DATE__
        else
        if(7==index)
        {
            LcdShowStr(67,  32,__TIME__, 0);
            LcdShowStr(0, i*16,__DATE__, 0);
            ///////////////////
            index++;
        }
        #endif
        else
        {
            if(4 == index)//dxl,2014.10.28,????V1.02?V1.03??????????
            {
                memcpy(Buffer,ProductInfo[4],9);
                Buffer[9] = 0;
                strcat(Buffer,"V1.05"); 
                LcdShowStr(0, i*16, Buffer, 0);
                index++;
            }
            else if(9 == index)     //??ID?????   //????2015.07.20 ???:fanqinghai
            {
                len=EepromPram_ReadPram(E2_DEVICE_ID, buff); 
                ////////////////
                temp=Public_BCD2HEX(buff[0]);
                sprintf((char *)&Buffer[0],"%d",temp);
                for(i=1; i<len; i++)
                {
                    temp=Public_BCD2HEX(buff[i]);
                    sprintf((char *)&Buffer[2*i-1],"%02d",temp);
                }
                LcdShowStr(12,16,(char *)Buffer,0);
                index++;
            }
            else
            {
                LcdShowStr(0, i*16, ProductInfo[index++], 0);//modify by joneming 20130312 
            }
        }
    }
}
/*************************************************************
** ��������: TaskShowProductInfo
** ��������: "��Ʒ��Ϣ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowProductInfo(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            s_stTaskAttrib.ControlId=0;
            s_stTaskAttrib.ControlMaxId=PRODUCT_ITEMS;
            ShowProductInfo(s_stTaskAttrib.ControlId);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��   
                    //LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break;               
                case KEY_UP://--    
                    if(s_stTaskAttrib.ControlId>0)
                    {
                        s_stTaskAttrib.ControlId--;
                        ShowProductInfo(s_stTaskAttrib.ControlId);
                    }
                    break;
                case KEY_DOWN://++  
                    if((s_stTaskAttrib.ControlId+1)*4<s_stTaskAttrib.ControlMaxId)
                    {
                        s_stTaskAttrib.ControlId++;
                        ShowProductInfo(s_stTaskAttrib.ControlId);
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowSIMCardNumber
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowSIMCardNumber(void)
{
    unsigned char buffer[30],data[20];    
    LcdClearScreen();
    LcdShowCaption("SIM��ICCID��",0); 
    LcdShowHorLine(0, 131, 17, 1);
    MOD_GetIccidBcdValue(data);
    Public_ConvertBcdToAsc(buffer,data,10);
    buffer[20]='\0';
    LcdShowStr(0,20,(char *)buffer,0);
}
/*************************************************************
** ��������: TaskShowSIMCardNumber
** ��������: "SIM����"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowSIMCardNumber(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:            
            ShowSIMCardNumber();
            LZM_SetAlarm(TEST_TIMER,SECS(10));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    break;                               
                case KEY_UP://--
                    break;
                case KEY_DOWN://++
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuMain);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void LcdShowTtsVolume(unsigned char Volume)
{
    unsigned char i,x,xx,y,yy,width,hight;
    width = 10;
    hight = 5;
    y = 62;
    x = 6; 
    xx = x + width;
    yy = y - hight;
    LcdClearArea(0,19,130,63);
    for(i=0; i< 8; i++)
    {
        if(i<Volume)
        {
            LcdReverseBar(x,yy,xx,y);
        }
        else
        {
            LcdShowRect(x,yy,xx,y,0);
        }
        x = xx + 5 ;
        xx = x + width;
        yy = yy - hight;
    }
}
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void AdjustAndShowTTsVolume(unsigned char flag)
{
    if(1==flag)
    {
        SetModTtsVolumeIncr();
    }
    else
    if(2==flag)
    {
        SetModTtsVolumeDecr();
    } 
    ////////////////////
    if(flag)
    LZM_SetAlarm(TEST_TIMER, PUBLICSECS(0.3));
    Public_ShowValue(104, 0,"v:%d", GetModTtsVolume());
    LcdShowTtsVolume(GetModTtsVolume());
}
/*************************************************************
** ��������: TaskShowSIMCardNumber
** ��������: "SIM����"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowAdjustTtsVolume(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:            
            LcdClearScreen();
            LcdShowStr(10,0,"����TTS����",0); 
            LcdShowHorLine(0, 131, 17, 1);
            AdjustAndShowTTsVolume(0);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuMain);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    break;                               
                case KEY_UP://--
                    AdjustAndShowTTsVolume(1);
                    break;
                case KEY_DOWN://++
                    AdjustAndShowTTsVolume(2);
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        case TEST_TIMER:
            Public_PlayTTSVoiceStr("��������");
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskLcdBacklight
** ��������: "�������"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskLcdBacklight(LZM_MESSAGE *Msg)
{
    static char *DispBuffer[]={"Ĭ��","����",""};   //��ص�ѹ����
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("�������",0); 
            LcdShowStr(2, 23, "����ѡ��:", 0);  
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],80,23,DispBuffer,"");
            s_stSpin[0].handle=s_stTaskAttrib.BacklightFlag;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystem);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystem);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.BacklightFlag)
                    {
                        s_stTaskAttrib.BacklightFlag=handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        SaveSystemBacklightFlag();
                        LcdClearArea(0,19,131,44);                         
                        LcdShowCaption(s_stTaskAttrib.BacklightFlag?"�Ѹ�Ϊ��������":"�Ѹ�ΪĬ������", 23); 
                        LZM_SetAlarm(TEST_TIMER, SECS(1));
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystem);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowDriveMile
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowQueryServerParameter(unsigned char page)
{
    unsigned char buf[30];
    unsigned long val;
    LcdClearScreen();
    //��ʾ
    if(0 == page)
    {
        LcdShowStr(0,0,"��IP:",0);
        val=EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, buf);
        buf[val]='\0';
        LcdShowStr(0,16,(char *)buf,0);        
        LcdShowStr(0,32,"����IP:",0);  
        val=EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, buf);
        buf[val]='\0';
        LcdShowStr(0,48,(char *)buf,0);
    }
    else
    if(1 == page)
    {
        LcdShowStr(0,0,"APN:",0);
        val=EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buf);
        buf[val]='\0';
        LcdShowStr(0,16,(char *)buf,0);
        LcdShowStr(0,32,"����APN:",0);
        val=EepromPram_ReadPram(E2_BACKUP_SERVER_APN_ID, buf);
        buf[val]='\0';
        LcdShowStr(0,48,(char *)buf,0);
    }
    else
    if(2 == page)
    {
        LcdShowStr(0,0,"TCP�˿ں�:",0);         
        val=EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID, buf);
        if(val)
        {
            val=ConvertReadPramToValue(E2_MAIN_SERVER_TCP_PORT_ID);
            sprintf((char *)buf,"%d",val);
            LcdShowStr(0,16,(char *)buf,0);
        }
        LcdShowStr(0,32,"UDP�˿ں�:",0);
        val=EepromPram_ReadPram(E2_MAIN_SERVER_UDP_PORT_ID, buf);
        if(val)
        {
            val=ConvertReadPramToValue(E2_MAIN_SERVER_UDP_PORT_ID);
            sprintf((char *)buf,"%d",val);
            LcdShowStr(0,48,(char *)buf,0); 
        }
        
    }
    else
    if(3 == page)
    {
        LcdShowStr(0,0,"����������",0);
        val=Public_ReadDataFromFRAM(FRAM_MAIN_DOMAIN_NAME_ADDR, buf,FRAM_MAIN_DOMAIN_NAME_LEN);
        buf[val]= 0;
        LcdShowStr(0, 16, (char *)buf, 0);
        LcdShowStr(0,32,"������������",0);
        val=Public_ReadDataFromFRAM(FRAM_BACKUP_DOMAIN_NAME_ADDR, buf,FRAM_BACKUP_DOMAIN_NAME_LEN);
        buf[val]= 0;
        LcdShowStr(0, 48, (char *)buf, 0);
    }
    else
    if(4 == page)
    {
        LcdShowStr(0,0,"IP2:",0);
        val=EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_IP_ID, buf);
        buf[val]='\0';
        LcdShowStr(0,16,(char *)buf,0);        
        LcdShowStr(0,32,"����IP2:",0);  
        val=EepromPram_ReadPram(E2_SECOND_BACKUP_SERVER_IP_ID, buf);
        buf[val]='\0';
        LcdShowStr(0,48,(char *)buf,0);
    }
    else
    if(5 == page)
    {
        LcdShowStr(0,0,"TCP2:",0);
        val=EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_TCP_PORT_ID, buf);
        if(val)
        {
            val=ConvertReadPramToValue(E2_SECOND_MAIN_SERVER_TCP_PORT_ID);
            sprintf((char *)buf,"%d",val);
            LcdShowStr(0,16,(char *)buf,0);
        }
        LcdShowStr(0,32,"UDP2:",0);
        val=EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_UDP_PORT_ID, buf);
        if(val)
        {
            val=ConvertReadPramToValue(E2_SECOND_MAIN_SERVER_UDP_PORT_ID);
            sprintf((char *)buf,"%d",val);
            LcdShowStr(0,48,(char *)buf,0); 
        }        
    }
}
/*************************************************************
** ��������: TaskSystemServerSetIP
** ��������: "������IP"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemQueryServerParameter(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START: 
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId = 5;
            ShowQueryServerParameter(s_stTaskAttrib.ControlId);
            return 1;
        case KEY_PRESS: 
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemManageServer);
                    break;
                case KEY_ENTER://ȷ��                    
                    break;
                case KEY_UP://--    
                    if(s_stTaskAttrib.ControlId>0)
                    {
                        s_stTaskAttrib.ControlId--;
                        ShowQueryServerParameter(s_stTaskAttrib.ControlId);
                    }
                    break;
                case KEY_DOWN://++  
                    if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)
                    {
                        s_stTaskAttrib.ControlId++;
                        ShowQueryServerParameter(s_stTaskAttrib.ControlId);
                    }
                    break;
                default: 
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemServerSetIP
** ��������: "������IP"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetIP(LZM_MESSAGE *Msg)
{
    unsigned char val,buf[16];
    unsigned char i,tmpbuf[16];
    unsigned char temp[4];
    unsigned short tmp;
    switch(Msg->message)
    {
        case TASK_START:            
            val=EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, buf);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId =3;
            LcdShowStr(0,0,"��ǰ��IP:",0);                
            buf[val]='\0';
            LcdShowStr(0,16,(char *)buf,0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(!IsBrushAdministratorCard())
            {
                ConvertStrToIP(buf,s_stServerParam.mainIP,val);
            }
            for(i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
            {
                ESpinInit(&s_stESpin[i], 3+i*33, 48, 3, s_stServerParam.mainIP[i]);
                s_stESpin[i].loop =0;
                ESpinShow(&s_stESpin[i]);
                if(i<s_stTaskAttrib.ControlMaxId)LcdShowStr(29+i*33, 48, ".", 0); 
            }            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
          
            return 1;
        case KEY_PRESS: 
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
                    {
                        tmp=atoi(s_stESpin[i].buffer);
                        if(tmp>255)
                        {
                            LcdCopyScreen();
                            LcdClearScreen();
                            LcdShowCaption("��ʽ����", 24);
                            LcdPasteScreen();
                            ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            s_stTaskAttrib.ControlId=i;
                            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            return 0;
                        }
                        temp[i]=tmp;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    sprintf((char*)buf,"%d.%d.%d.%d",temp[0],temp[1],temp[2],temp[3]);
                    val=strlen((char*)buf);
                    EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, buf,val);
                    EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, tmpbuf);
                    LcdClearArea(0,19,131,44);
                    if(strncmp((char*)buf,(char*)tmpbuf,val)==0)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_MAIN_SERVER_IP_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    val=ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(val)
                    {
                        ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                        if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)s_stTaskAttrib.ControlId++;
                        else s_stTaskAttrib.ControlId = 0;
                        ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                    }
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemServerSetAPN
** ��������: "������APN"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetAPN(LZM_MESSAGE *Msg)
#if 0
{    
    unsigned char buf[20];
    unsigned char val;
    switch(Msg->message)
    {
        case TASK_START:
            val=EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buf);
            LcdClearScreen();
            s_stTaskAttrib.ControlId = 0xff; 
            buf[val]='\0';            
            LcdShowStr(0,3,"APN:",0);  
            LcdShowStr(32,3,(char *)buf,0);
            if(val>16)
            {
                LcdShowStr((val-12)*8+4,24,"�޸�Ϊ:",0);
            }
            else
            if(val>12)
            {
                LcdShowStr((val-12)*8+4,24,"ȷ���޸�Ϊ:",0);
            }
            else
                LcdShowStr(0,24,"ȷ���޸�Ϊ:",0);
            ////////////////////////////
            if(!IsBrushAdministratorCard())
            {        
                strcpy(s_stServerParam.APN,(char *)buf);
            }
            /////////////////////
            if(strncmp(s_stServerParam.APN,(char*)s_apcAPNItems[0],5)==0)
            {
                s_stTaskAttrib.ControlId = 0;
            }
            else
            if(strncmp(s_stServerParam.APN,(char*)s_apcAPNItems[1],5)==0)
            {
                s_stTaskAttrib.ControlId = 1;
            }
            else
            {
                s_stTaskAttrib.ControlId = 2;
                s_apcAPNItems[2]=s_stServerParam.APN;
            }
            ////////////////////////
            SpinInit(&s_stSpin[0],4,45,(char **)s_apcAPNItems,"");
            s_stSpin[0].handle = s_stTaskAttrib.ControlId;
            if(s_stTaskAttrib.ControlId==2)s_stTaskAttrib.ControlId=0x55;
            SpinShow(&s_stSpin[0]);            
            SpinSetFocus(&s_stSpin[0]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    if(s_stTaskAttrib.ControlId!=s_stSpin[0].handle)
                    {
                        val=strlen(s_stSpin[0].item[s_stSpin[0].handle]);
                        EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, (unsigned char *)s_stSpin[0].item[s_stSpin[0].handle],val);
                        EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buf);
                        LcdClearScreen();
                        if(strncmp((char*)buf,(char*)s_stSpin[0].item[s_stSpin[0].handle],val)==0)
                        {
                            LcdShowCaption("����ɹ�", 24);
                            EepromPram_UpdateVariable(E2_MAIN_SERVER_APN_ID);
                        }
                        else
                        {
                           LcdShowCaption("�������,����", 24); 
                           
                        }
                        LZM_SetAlarm(TEST_TIMER, SECS(1));
                    }
                    else
                    {
                        
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }                    
                    break;                                
                default:
                    SpinResponse(&s_stSpin[0],TaskPara);
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif
{    
  unsigned char buf[20];
  unsigned char val;
  unsigned char i;
  switch(Msg->message)
  {
  case TASK_START:
    val=EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buf);
    LcdClearScreen();
    s_stTaskAttrib.ControlId = 0xff; 
    buf[val]='\0';            
    LcdShowStr(0,3,"APN:",0);  
    LcdShowStr(32,3,(char *)buf,0);
    if(val>16)
    {
      LcdShowStr((val-12)*8+4,24,"�޸�Ϊ:",0);
    }
    else
      if(val>12)
      {
        LcdShowStr((val-12)*8+4,24,"ȷ���޸�Ϊ:",0);
      }
      else
        LcdShowStr(0,24,"ȷ���޸�Ϊ:",0);
    ////////////////////////////
    if(!IsBrushAdministratorCard())
    {        
      strcpy(s_stServerParam.APN,(char *)buf);
    }
    /////////////////////
    for(i=0;;i++)
    {
      if(strncmp(s_stServerParam.APN,(char*)s_apcAPNItems[i],5)==0)
      {
        s_stTaskAttrib.ControlId=i;
        break;
      }
      else 
        if(strcmp((char*)s_apcAPNItems[i],"")==0)
        {
          s_apcAPNItems[i]=s_stServerParam.APN;
          s_stTaskAttrib.ControlId=0x55;
          break;
        }
    }
    
    ////////////////////////
    SpinInit(&s_stSpin[0],4,45,(char **)s_apcAPNItems,"");
    s_stSpin[0].handle = i;
    SpinShow(&s_stSpin[0]);            
    SpinSetFocus(&s_stSpin[0]);
    return 1;
  case KEY_PRESS:
    if(s_stTaskAttrib.ControlId==0xff)
    {
      LZM_SetAlarm(TEST_TIMER, 1);
      break;
    }
    switch(TaskPara)
    {
    case KEY_BACK://����
      LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
      break;
    case KEY_ENTER://ȷ��
      if(s_stTaskAttrib.ControlId!=s_stSpin[0].handle)
      {
        val=strlen(s_stSpin[0].item[s_stSpin[0].handle]);
        EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, (unsigned char *)s_stSpin[0].item[s_stSpin[0].handle],val);
        EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, buf);
        LcdClearScreen();
        if(strncmp((char*)buf,(char*)s_stSpin[0].item[s_stSpin[0].handle],val)==0)
        {
          LcdShowCaption("����ɹ�", 24);
          EepromPram_UpdateVariable(E2_MAIN_SERVER_APN_ID);
        }
        else
        {
          LcdShowCaption("�������,����", 24); 
          
        }
        LZM_SetAlarm(TEST_TIMER, SECS(1));
      }
      else
      {
        
        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
      }                    
      break;                                
    default:
      SpinResponse(&s_stSpin[0],TaskPara);
      break;
    }
    return 1;           
  case TEST_TIMER:
    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
    break;
  case INT_RTC:
    break;
  default:
    return LZM_DefTaskProc(Msg);
  }
  return 0;
}

/*************************************************************
** ��������: TaskSystemServerSetIP
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetBackUpIP(LZM_MESSAGE *Msg)
{
    unsigned char buf[16];
    unsigned char val;       
    unsigned char temp[4],tmpbuf[16],i;
    unsigned short tmp;
    switch(Msg->message)
    {
        case TASK_START:            
            val=EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, buf);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId =3;
            LcdShowStr(0,0,"��ǰ����IP:",0);                
            buf[val]='\0';
            LcdShowStr(0,16,(char *)buf,0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(!IsBrushAdministratorCard())
            {
                ConvertStrToIP(buf, s_stServerParam.backIP,val);
            }
            ///////////////////
            for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
            {
                ESpinInit(&s_stESpin[i], 3+i*33, 48, 3, s_stServerParam.backIP[i]);
                s_stESpin[i].loop =0;
                ESpinShow(&s_stESpin[i]);
                if(i<s_stTaskAttrib.ControlMaxId)LcdShowStr(29+i*33, 48, ".", 0); 
            }            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
                    {
                        tmp=atoi(s_stESpin[i].buffer);
                        if(tmp>255)
                        {
                            LcdCopyScreen();
                            LcdClearScreen();
                            LcdShowCaption("��ʽ����", 24);    
                            LcdPasteScreen();
                            ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            s_stTaskAttrib.ControlId=i;
                            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            return 0;
                        }
                        temp[i]=tmp;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    sprintf((char*)buf,"%d.%d.%d.%d",temp[0],temp[1],temp[2],temp[3]);
                    val=strlen((char*)buf);
                    EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, buf,val);
                    EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, tmpbuf);
                    LcdClearArea(0,19,131,44);
                    if(strncmp((char*)buf,(char*)tmpbuf,val)==0)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_BACKUP_SERVER_IP_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:
                    val=ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(val)
                    {
                        ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                        if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)s_stTaskAttrib.ControlId++;
                        else s_stTaskAttrib.ControlId = 0;
                        ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                    }
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemServerSetTCPPort
** ��������: "����TCP�˿ں�"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetTCPPort(LZM_MESSAGE *Msg)
{
    unsigned long val;
    unsigned long val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:            
            val=ConvertReadPramToValue(E2_MAIN_SERVER_TCP_PORT_ID);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowStr(0,0,"��ǰTCP�˿ں�:",0);                
            sprintf(s_stTaskAttrib.ShowBuffer[0],"%d",val);
            LcdShowStr(3,16,s_stTaskAttrib.ShowBuffer[0],0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(IsBrushAdministratorCard())
            {
                val=s_stServerParam.tcp;
            }           
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 3, 48, 8,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, buf,4);                    
                    val1=ConvertReadPramToValue(E2_MAIN_SERVER_TCP_PORT_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_MAIN_SERVER_TCP_PORT_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemServerSetUDPPort
** ��������: "����UDP�˿ں�"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetUDPPort(LZM_MESSAGE *Msg)
{
    unsigned long val;
    unsigned long val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:            
            val=ConvertReadPramToValue(E2_MAIN_SERVER_UDP_PORT_ID);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowStr(0,0,"��ǰUDP�˿ں�:",0);                
            sprintf(s_stTaskAttrib.ShowBuffer[0],"%d",val);
            LcdShowStr(3,16,s_stTaskAttrib.ShowBuffer[0],0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(IsBrushAdministratorCard())
            {
                val=s_stServerParam.udp;
            }            
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 3, 48, 8,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }    
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_MAIN_SERVER_UDP_PORT_ID, buf,4);                    
                    val1=ConvertReadPramToValue(E2_MAIN_SERVER_UDP_PORT_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_MAIN_SERVER_UDP_PORT_ID); 
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemServerSetIP
** ��������: "������IP"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetIP2(LZM_MESSAGE *Msg)
{
    unsigned char val,buf[16];
    unsigned char i,tmpbuf[16];
    unsigned char temp[4];
    unsigned short tmp;
    switch(Msg->message)
    {
        case TASK_START:            
            val=EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_IP_ID, buf);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId =3;
            LcdShowStr(0,0,"��ǰIP2:",0);                
            buf[val]='\0';
            LcdShowStr(0,16,(char *)buf,0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(!IsBrushAdministratorCard())
            {
                ConvertStrToIP(buf,s_stServerParam.mainIP,val);
            }
            for(i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
            {
                ESpinInit(&s_stESpin[i], 3+i*33, 48, 3, s_stServerParam.mainIP[i]);
                s_stESpin[i].loop =0;
                ESpinShow(&s_stESpin[i]);
                if(i<s_stTaskAttrib.ControlMaxId)LcdShowStr(29+i*33, 48, ".", 0); 
            }            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS: 
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
                    {
                        tmp=atoi(s_stESpin[i].buffer);
                        if(tmp>255)
                        {
                            LcdCopyScreen();
                            LcdClearScreen();
                            LcdShowCaption("��ʽ����", 24);
                            LcdPasteScreen();
                            ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            s_stTaskAttrib.ControlId=i;
                            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            return 0;
                        }
                        temp[i]=tmp;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    sprintf((char*)buf,"%d.%d.%d.%d",temp[0],temp[1],temp[2],temp[3]);
                    val=strlen((char*)buf);
                    EepromPram_WritePram(E2_SECOND_MAIN_SERVER_IP_ID, buf,val);
                    EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_IP_ID, tmpbuf);
                    LcdClearArea(0,19,131,44);
                    if(strncmp((char*)buf,(char*)tmpbuf,val)==0)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_SECOND_MAIN_SERVER_IP_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    val=ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(val)
                    {
                        ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                        if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)s_stTaskAttrib.ControlId++;
                        else s_stTaskAttrib.ControlId = 0;
                        ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                    }
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemServerSetIP
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetBackUpIP2(LZM_MESSAGE *Msg)
{
    unsigned char buf[16];
    unsigned char val;       
    unsigned char temp[4],tmpbuf[16],i;
    unsigned short tmp;
    switch(Msg->message)
    {
        case TASK_START:            
            val=EepromPram_ReadPram(E2_SECOND_BACKUP_SERVER_IP_ID, buf);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId =3;
            LcdShowStr(0,0,"��ǰ����IP2:",0);                
            buf[val]='\0';
            LcdShowStr(0,16,(char *)buf,0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(!IsBrushAdministratorCard())
            {
                ConvertStrToIP(buf, s_stServerParam.backIP,val);
            }
            ///////////////////
            for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
            {
                ESpinInit(&s_stESpin[i], 3+i*33, 48, 3, s_stServerParam.backIP[i]);
                s_stESpin[i].loop =0;
                ESpinShow(&s_stESpin[i]);
                if(i<s_stTaskAttrib.ControlMaxId)LcdShowStr(29+i*33, 48, ".", 0); 
            }            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
                    {
                        tmp=atoi(s_stESpin[i].buffer);
                        if(tmp>255)
                        {
                            LcdCopyScreen();
                            LcdClearScreen();
                            LcdShowCaption("��ʽ����", 24);    
                            LcdPasteScreen();
                            ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            s_stTaskAttrib.ControlId=i;
                            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                            return 0;
                        }
                        temp[i]=tmp;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    sprintf((char*)buf,"%d.%d.%d.%d",temp[0],temp[1],temp[2],temp[3]);
                    val=strlen((char*)buf);
                    EepromPram_WritePram(E2_SECOND_BACKUP_SERVER_IP_ID, buf,val);
                    EepromPram_ReadPram(E2_SECOND_BACKUP_SERVER_IP_ID, tmpbuf);
                    LcdClearArea(0,19,131,44);
                    if(strncmp((char*)buf,(char*)tmpbuf,val)==0)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_SECOND_BACKUP_SERVER_IP_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:
                    val=ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(val)
                    {
                        ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                        if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)s_stTaskAttrib.ControlId++;
                        else s_stTaskAttrib.ControlId = 0;
                        ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                    }
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemServerSetTCPPort
** ��������: "����TCP�˿ں�"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetTCPPort2(LZM_MESSAGE *Msg)
{
    unsigned long val;
    unsigned long val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:            
            val=ConvertReadPramToValue(E2_SECOND_MAIN_SERVER_TCP_PORT_ID);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowStr(0,0,"��ǰTCP2:",0);                
            sprintf(s_stTaskAttrib.ShowBuffer[0],"%d",val);
            LcdShowStr(3,16,s_stTaskAttrib.ShowBuffer[0],0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(IsBrushAdministratorCard())
            {
                val=s_stServerParam.tcp;
            }           
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 3, 48, 8,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_SECOND_MAIN_SERVER_TCP_PORT_ID, buf,4);                    
                    val1=ConvertReadPramToValue(E2_SECOND_MAIN_SERVER_TCP_PORT_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_SECOND_MAIN_SERVER_TCP_PORT_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);
                    break;
            }
            return 1;         
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemServerSetUDPPort
** ��������: "����UDP�˿ں�"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetUDPPort2(LZM_MESSAGE *Msg)
{
    unsigned long val;
    unsigned long val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:            
            val=ConvertReadPramToValue(E2_SECOND_MAIN_SERVER_UDP_PORT_ID);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowStr(0,0,"��ǰUDP2:",0);                
            sprintf(s_stTaskAttrib.ShowBuffer[0],"%d",val);
            LcdShowStr(3,16,s_stTaskAttrib.ShowBuffer[0],0);
            LcdShowStr(0,32,"ȷ���޸�Ϊ:",0);
            if(IsBrushAdministratorCard())
            {
                val=s_stServerParam.udp;
            }            
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 3, 48, 8,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }    
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_SECOND_MAIN_SERVER_UDP_PORT_ID, buf,4);                    
                    val1=ConvertReadPramToValue(E2_SECOND_MAIN_SERVER_UDP_PORT_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_SECOND_MAIN_SERVER_UDP_PORT_ID); 
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemServerSetMainDomainName
** ��������: "��������������"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetMainDomainName(LZM_MESSAGE *Msg)
{    
    unsigned char buf[40];
    unsigned char val;
    switch(Msg->message)
    {
        case TASK_START:
            val=Public_ReadDataFromFRAM(FRAM_MAIN_DOMAIN_NAME_ADDR, buf,FRAM_MAIN_DOMAIN_NAME_LEN);
            LcdClearScreen();           
            LcdShowCaption("����������",0);
            LcdShowHorLine(0, 131, 18, 1);
            #if 0
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            /////////////////////
            s_stTaskAttrib.ControlId = 0;
            if(strncmp((char*)buf,(char*)s_apcCenterDnsItems[1],5)==0)
            {
                s_stTaskAttrib.ControlId = 1;
            }            
            ////////////////////////
            SpinInit(&s_stSpin[0],4,24,s_apcCenterDnsItems,"");
            s_stSpin[0].handle = s_stTaskAttrib.ControlId;
            SpinShow(&s_stSpin[0]);            
            SpinSetFocus(&s_stSpin[0]); 
            #else
            buf[val]= 0;
            LcdShowStr(0, 22, (char *)buf, 0);
            s_stTaskAttrib.ControlId=0xff;
            #endif            
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    val=strlen(s_stSpin[0].item[s_stSpin[0].handle]);
                    memset(buf,0,FRAM_MAIN_DOMAIN_NAME_LEN);
                    memcpy(buf,s_stSpin[0].item[s_stSpin[0].handle],val);
                    Public_WriteDataToFRAM(FRAM_MAIN_DOMAIN_NAME_ADDR, buf,FRAM_MAIN_DOMAIN_NAME_LEN);
                    Public_ReadDataFromFRAM(FRAM_MAIN_DOMAIN_NAME_ADDR, buf,FRAM_MAIN_DOMAIN_NAME_LEN);
                    LcdClearScreen();
                    if(strncmp((char*)buf,(char*)s_stSpin[0].item[s_stSpin[0].handle],val)==0)
                    {
                        LcdShowCaption("����ɹ�", 24);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    LZM_SetAlarm(TEST_TIMER, SECS(1));                   
                    break;                                
                default:
                    SpinResponse(&s_stSpin[0],TaskPara);
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemServerSetMainDomainName
** ��������: "��������������"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemServerSetBackUpDomainName(LZM_MESSAGE *Msg)
{    
    unsigned char buf[40];
    unsigned char val;
    switch(Msg->message)
    {
        case TASK_START:
            val=Public_ReadDataFromFRAM(FRAM_BACKUP_DOMAIN_NAME_ADDR, buf,FRAM_BACKUP_DOMAIN_NAME_LEN);
            LcdClearScreen();           
            LcdShowCaption("������������",0);
            LcdShowHorLine(0, 131, 18, 1);
            #if 0
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            /////////////////////
            s_stTaskAttrib.ControlId = 0;
            if(strncmp((char*)buf,(char*)s_apcCenterDnsItems[1],5)==0)
            {
                s_stTaskAttrib.ControlId = 1;
            }            
            ////////////////////////
            SpinInit(&s_stSpin[0],4,24,s_apcCenterDnsItems,"");
            s_stSpin[0].handle = s_stTaskAttrib.ControlId;
            SpinShow(&s_stSpin[0]);            
            SpinSetFocus(&s_stSpin[0]);
            #else
            buf[val]= 0;
            LcdShowStr(0, 22, (char *)buf, 0);
            s_stTaskAttrib.ControlId=0xff;
            #endif
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    val=strlen(s_stSpin[0].item[s_stSpin[0].handle]);
                    memset(buf,0,FRAM_BACKUP_DOMAIN_NAME_LEN);
                    memcpy(buf,s_stSpin[0].item[s_stSpin[0].handle],val);
                    Public_WriteDataToFRAM(FRAM_BACKUP_DOMAIN_NAME_ADDR, buf,FRAM_BACKUP_DOMAIN_NAME_LEN);
                    Public_ReadDataFromFRAM(FRAM_BACKUP_DOMAIN_NAME_ADDR, buf,FRAM_BACKUP_DOMAIN_NAME_LEN);
                    LcdClearScreen();
                    if(strncmp((char*)buf,(char*)s_stSpin[0].item[s_stSpin[0].handle],val)==0)
                    {
                        LcdShowCaption("����ɹ�", 24);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    LZM_SetAlarm(TEST_TIMER, SECS(1));                                   
                    break;                                
                default:
                    SpinResponse(&s_stSpin[0],TaskPara);
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemSetCarFeatureCoef
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarFeatureCoef(LZM_MESSAGE *Msg)
{
    unsigned long val,val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:            
            val=ConvertReadPramToValue(E2_CAR_FEATURE_COEF_ID);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("��������ϵ��",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);            
            LcdShowStr(1, 24, "����ϵ��:", 0);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 76, 24, 6,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &buf[1],3);                    
                    val1=ConvertReadPramToValue(E2_CAR_FEATURE_COEF_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_CAR_FEATURE_COEF_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemSetCarInitialMile
** ��������: "���ó�����ʼ���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarTotalMile(LZM_MESSAGE *Msg)
{
    unsigned long val,val1;
    unsigned char buf[6];
    float temp;
    switch(Msg->message)
    {
        case TASK_START:
            if(E2ParamApp_ReadSpecialParam(E2_CAR_TOTAL_MILE_ID,buf))
            {
                val = Public_ConvertBufferToLong(buf);
            }
            //val=ConvertReadPramToValue(E2_CAR_TOTAL_MILE_ID);
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("���ó�ʼ���",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);            
            LcdShowStr(0, 24, "���:         km", 0);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 40, 24, 9,val);
            sprintf(s_stESpin[s_stTaskAttrib.ControlId].buffer,"%09.1f",val*0.1);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��                    
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    temp=atof(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    val=(unsigned long)(temp*10);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, buf,4);                    
                    val1=ConvertReadPramToValue(E2_CAR_TOTAL_MILE_ID);                    
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemSetCarProvinceID
** ��������: "����ʡ��ID"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarProvinceID(LZM_MESSAGE *Msg)
{
    unsigned short val,val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:
            EepromPram_ReadPram(E2_CAR_PROVINCE_ID, buf);
            val=Public_ConvertBufferToShort(buf);            
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("����ʡ��ID",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);            
            LcdShowStr(0, 24, "ʡ��ID:", 0);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 64, 24, 5,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��                    
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    val=atoi(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertShortToBuffer(val,buf);                        
                    EepromPram_WritePram(E2_CAR_PROVINCE_ID, buf,2);                    
                    val1=ConvertReadPramToValue(E2_CAR_PROVINCE_ID);
                    EepromPram_UpdateVariable(E2_CAR_PROVINCE_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemSetCarCityID
** ��������: "��������ID"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarCityID(LZM_MESSAGE *Msg)
{
    unsigned short val,val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:
            EepromPram_ReadPram(E2_CAR_CITY_ID, buf);
            val=Public_ConvertBufferToShort(buf);            
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("��������ID",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);            
            LcdShowStr(0, 24, "����ID:", 0);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 64, 24, 5,val);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��                    
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    val=atoi(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    Public_ConvertShortToBuffer(val,buf);                        
                    EepromPram_WritePram(E2_CAR_CITY_ID, buf,2);                    
                    val1=ConvertReadPramToValue(E2_CAR_CITY_ID);
                    EepromPram_UpdateVariable(E2_CAR_CITY_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemParamSetPhoneNumber
** ��������: "�����ֻ���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarOwnerPhone(LZM_MESSAGE *Msg)
{
    unsigned char len;
    unsigned char buf[13]={0};
    switch(Msg->message)
    {
        case TASK_START:   
            len=Public_ReadDataFromFRAM(FRAM_CAR_OWNER_PHONE_ADDR, buf,FRAM_CAR_OWNER_PHONE_LEN);
            if(FRAM_CAR_OWNER_PHONE_LEN!=len)
            {
                memset(&buf[len],0x30,FRAM_CAR_OWNER_PHONE_LEN-len);
            }
            buf[FRAM_CAR_OWNER_PHONE_LEN-1]='\0';
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("�����ֻ���",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 22, 24, 11,1);
            s_stESpin[s_stTaskAttrib.ControlId].type = 1;//�ַ���
            strcpy(s_stESpin[s_stTaskAttrib.ControlId].buffer,(char *)buf);                
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ�� 
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    memcpy(buf,s_stESpin[s_stTaskAttrib.ControlId].buffer,11);
                    buf[11]=0;
                    Public_WriteDataToFRAM(FRAM_CAR_OWNER_PHONE_ADDR, buf,FRAM_CAR_OWNER_PHONE_LEN);
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("����ɹ�", 24);
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemParamSetSpeedParam
** ��������: "���ó����ٶȲ���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**�޸���           : fanqinghai
**�޸�����:����Զ��������Զ�gps
**�������:  2015.05.18
*************************************************************/  
LZM_RET TaskSystemParamSetSpeedParam(LZM_MESSAGE *Msg)
{
    unsigned long val,val1;
    unsigned char temp,temp1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;           
            LcdShowCaption("�����ٶȲ���",2);
            LcdShowHorLine(0, 131, 20, 1);
            LcdShowStr(0, 24, "���ٱ���:", 0);
            LcdShowStr(98, 24, "km/h", 0);
            LcdShowStr(0, 45, "�ٶ�:", 0);
            val=ConvertReadPramToValue(E2_MAX_SPEED_ID);
            ESpinInit(&s_stESpin[0], 72, 24, 3,val);
            SpinInit(&s_stSpin[0],42,45,(char **)c_apcCarSpeedSelectItems,"");
            EepromPram_ReadPram(E2_SPEED_SELECT_ID,&temp); 
            if(temp>3)temp=0;
            s_stSpin[0].handle=temp;
            s_stSpin[0].keyflag =1;
            ////////////////////
            s_stESpin[0].loop = 0;
            ESpinShow(&s_stESpin[0]);
            SpinShow(&s_stSpin[0]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break; 
                case KEY_ENTER://ȷ��
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    val=atol(s_stESpin[0].buffer);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_MAX_SPEED_ID, buf,4);
                    temp=s_stSpin[0].handle;
                    EepromPram_WritePram(E2_SPEED_SELECT_ID,&temp,1);
                    //////////////////////////////////////////
                    val1=ConvertReadPramToValue(E2_MAX_SPEED_ID);
                    EepromPram_ReadPram(E2_SPEED_SELECT_ID,&temp1); 
                    LcdClearScreen();
                    if(val==val1&&temp1==temp)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_MAX_SPEED_ID);
                        EepromPram_UpdateVariable(E2_SPEED_SELECT_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }                    
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                               
                default:
                    if(!s_stTaskAttrib.ControlId)
                    {
                        temp=ESpinResponse(&s_stESpin[0],TaskPara); 
                        if(temp)
                        {
                            ESpinLoseFocus(&s_stESpin[0]);
                            s_stTaskAttrib.ControlId++;
                            SpinSetFocus(&s_stSpin[0]);
                        }
                    }
                    else
                    {
                        temp=SpinResponse(&s_stSpin[0],TaskPara); 
                        if(temp)
                        {
                            SpinLoseFocus(&s_stSpin[0]);
                            s_stTaskAttrib.ControlId=0;
                            ESpinSetFocus(&s_stESpin[0]);
                        }
                    }
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemParamSetPhoneNumber
** ��������: "�����ֻ���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemParamSetPhoneNumber(LZM_MESSAGE *Msg)
{
    unsigned char val;
    unsigned char buf[13];
    switch(Msg->message)
    {
        case TASK_START:
            val=ReadAndCheckPhoneNumber(buf);
            if(0==val)
            {
                memcpy(buf,"00000000000",11);
            }
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("�����ֻ���",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 22, 24, 11,1);
            s_stESpin[s_stTaskAttrib.ControlId].type = 1;//�ַ���
            strcpy(s_stESpin[s_stTaskAttrib.ControlId].buffer,(char *)buf);                
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ�� 
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    val=SaveAndCheckTerminalPhoneNumber((unsigned char *)s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    LcdClearArea(0,19,131,44);
                    if(val==1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_DEVICE_PHONE_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24);
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemParamSetDeviceID
** ��������: "�����ն�ID"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemParamSetDeviceID(LZM_MESSAGE *Msg)
{
    unsigned char val;
    char buf[17];
    unsigned long deviceID;
    static unsigned short manufacturer;
    switch(Msg->message)
    {
        case TASK_START:
            val=ReadSystemDeviceID(&deviceID,&manufacturer);
            if(val==1)
            {
                LcdClearScreen();
                s_stTaskAttrib.ControlId=0;
                LcdShowCaption("�����ն�ID",0);
                LcdShowHorLine(0, 131, 18, 1);
                //LcdShowHorLine(0, 131, 45, 1);
                //LcdShowCaption("��<ȷ��>����",47);
                LcdShowStr(12, 23, "ID:", 0);
                sprintf(buf,"������ID:%05d",manufacturer);
                LcdShowCaption(buf,45);
                ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 40, 23, 10,deviceID);
                s_stESpin[s_stTaskAttrib.ControlId].type = 1;//�ַ���              
                ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
                ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            }
            else
            {
                LcdClearScreen();
                LcdShowCaption("���ݸ�ʽ����!",23);
                
                s_stTaskAttrib.ControlId=0xff;
                LZM_SetAlarm(TEST_TIMER, SECS(1));
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ�� 
                    val=s_stESpin[s_stTaskAttrib.ControlId].buffer[0]-0x30;
                    if(val>=4)
                    {
                        LcdCopyScreen();
                        LcdClearArea(0,19,131,62);
                        LcdShowCaption("��ֵ����Χ", 26);
                        LcdPasteScreen();
                        return 0;
                    }
                    deviceID=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    LcdClearArea(0,19,131,62);
                    LcdShowCaption("���ڱ���...", 24);
                    val=SaveAndCheckSystemDeviceID(deviceID,manufacturer);
                    LcdClearArea(0,19,131,62);
                    if(val==1)
                    {
                        LcdShowCaption("����ɹ�", 24);
                        EepromPram_UpdateVariable(E2_DEVICE_PHONE_ID);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24);
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#if 0
/*************************************************************
** ��������: TaskMenuSetPhoneTypeSelect
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSetPhoneTypeSelect(LZM_MESSAGE *Msg)
{    
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();            
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("��ѡ������",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>ѡ��",47);
            LcdShowStr(18, 23, "ͨ��", 0);
            LcdShowStr(80, 23, "��ί", 0);
            LcdReverseBar(5,22,65,40);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemAdvanced);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://
                    LZM_StartNewTask(TaskMenuSystemAdvanced);//ϵͳ����
                    break;
                case KEY_ENTER://
                    if(s_stTaskAttrib.ControlId==0)
                    {
                        LZM_StartNewTask(TaskSystemParamSetPhoneNumber);
                    }
                    else
                    {
                        LZM_StartNewTask(TaskSystemParamSetDeviceID);
                    }
                    break;
                case KEY_DOWN:
                case KEY_UP:
                    s_stTaskAttrib.ControlId^=1;
                    LcdReverseBar(5,22,126,40);
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemAdvanced);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif
/*************************************************************
** ��������: TaskSystemDebugShowIOstatus
** ��������: "����״̬��ʾ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemDebugShowIOstatus(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();            
            LcdShowCaption("����״̬��ʾ",0);
            LcdShowHorLine(0, 131, 17, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>���",47);
            s_stTaskAttrib.select    = 0;
            s_stTaskAttrib.ControlId = 0;
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��                    
                    s_stTaskAttrib.select=1;
                    LcdClearArea(0,19,131,62);
                    LcdShowCaption("���ڼ��,���Ժ�", 24);
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                case KEY_UP:
                    if(s_stTaskAttrib.select==2)
                    {
                        if(s_stTaskAttrib.ControlId)
                        {
                            s_stTaskAttrib.ControlId--;
                            ///////
                            ShowCurrentIOstatus(s_stTaskAttrib.ControlId);
                        }
                    }
                    break;
                case KEY_DOWN:
                    if(s_stTaskAttrib.select==2)
                    {
                        if((s_stTaskAttrib.ControlId+1)*IO_ONE_PAGE_NUM<IO_IN_ITEMS_NUM)
                        {
                            s_stTaskAttrib.ControlId++;
                            /////////////
                            ShowCurrentIOstatus(s_stTaskAttrib.ControlId);
                        }
                    }
                    break;
            }
            return 1;
        case TEST_TIMER:
            s_stTaskAttrib.select = 2;
            ShowCurrentIOstatus(s_stTaskAttrib.ControlId);
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemDebugIOOut
** ��������: ���״̬���Բ˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemDebugIOOut(LZM_MESSAGE *Msg)
{
    unsigned char val;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.ControlId = 0;
            LcdShowCaption("���״̬����",0);
            LcdShowHorLine(0, 131, 17, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],24,23,(char **)c_apcIOOutItems,"");
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff&&TaskPara!=KEY_BACK)
            {
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    s_stTaskAttrib.select=s_stSpin[0].handle+IO_IN_MAX+1;
                    val=GetCurrentIOstatus(s_stTaskAttrib.select);
                    val^=1;
                    ChangCurrentIOOutstatus(s_stTaskAttrib.select,val);
                    LcdClearArea(112,23,128,40);
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(3));
                    break;                                
                default:
                    SpinResponse(&s_stSpin[0],TaskPara);
                    LcdClearArea(112,23,128,40);
                    break;
            }
            return 1;
        case TEST_TIMER:
            //TestBeep(1);
            s_stTaskAttrib.ControlId =0;
            val=GetCurrentIOstatus(s_stTaskAttrib.select);
            LcdShowStr(112,23,val?"��":"��",0);
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/////////////////////////////
extern u32 Link1ReportTimeSpace;//��ǰ����1�㱨ʱ����
extern u32 Link1ReportDistanceSpace;//��ǰ����1�㱨������
/*************************************************************
** ��������: ShowCurrentReportMode
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowCurrentReportMode(void)
{    
    char buffer[30];
    unsigned char y;
    unsigned long ProgOfRpt,StrategyOfRpt;
    //////////////////
    ProgOfRpt =0; 
    StrategyOfRpt = 0;
    if(EepromPram_ReadPram(E2_POSITION_REPORT_SCHEME_ID,(unsigned char *)buffer))
    {
        ProgOfRpt = Public_ConvertBufferToLong((unsigned char *)buffer);
    }    
    if(EepromPram_ReadPram(E2_POSITION_REPORT_STRATEGY_ID,(unsigned char *)buffer))
    {
        StrategyOfRpt = Public_ConvertBufferToLong((unsigned char *)buffer);
    }
    /////////////////////
    LcdClearScreen();
    LcdShowCaption("��ǰ�㱨����:", 0);
    LcdShowCaption(ProgOfRpt?"���ݵ�¼/ACC״̬":"����ACC״̬",16);
    ///////////
    y=32;
    if(StrategyOfRpt==0)
    {
        LcdShowCaption("��ʱ�㱨",y);
        sprintf(buffer,"��ǰ���:%d��",Link1ReportTimeSpace);
    }
    else
    if(StrategyOfRpt==1)
    {
        LcdShowCaption("����㱨",y);
        sprintf(buffer,"��ǰ���:%d��",Link1ReportDistanceSpace);
    }
    else
    if(StrategyOfRpt==2)
    {
        LcdShowCaption("��ʱ����㱨",y);        
        sprintf(buffer,"���%ds/%dm",Link1ReportTimeSpace,Link1ReportDistanceSpace);
    }
    else
    {
        buffer[0]='\0';
    }
    y+=16;
    LcdShowCaption(buffer,y);
}
/*************************************************************
** ��������: TaskSystemDebugCurrentReportMode
** ��������: "��ǰ�㱨����:"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemDebugCurrentReportMode(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            ShowCurrentReportMode();
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    break;                                
                case KEY_UP:                    
                    break;
                case KEY_DOWN:                    
                    break;
            }
            return 1;
        case TEST_TIMER:
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: SystemDebugFixTime
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemDebugFixTimeOk(void)
{
    unsigned char temp;
    unsigned char buffer[17];
    unsigned long val,val1;
    temp=0x55;
    EepromPram_WritePram(E2_INSTALL_TIME_FLAG_ID, &temp,1);
    val=RTC_GetCounter();
    Public_ConvertLongToBuffer(val,buffer);
    EepromPram_WritePram(E2_INSTALL_TIME_ID, buffer,4);
    val1=ConvertReadPramToValue(E2_INSTALL_TIME_ID);
    EepromPram_ReadPram(E2_INSTALL_TIME_FLAG_ID, &temp);
    LcdClearScreen();
    if(val1==val&&temp==0x55)
    {
        LcdShowCaption("����ɹ�", 23); 
        EepromPram_UpdateVariable(E2_INSTALL_TIME_ID);
        EepromPram_UpdateVariable(E2_INSTALL_TIME_FLAG_ID); 
        LZM_SetAlarm(TEST_TIMER, SECS(1));
    }
    else
    {
        LcdShowCaption("����ʧ��", 23);  
        
        LZM_SetAlarm(TEST_TIMER, SECS(1));
    }
}
/*************************************************************
** ��������: SystemDebugFixTime
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemDebugFixTimeCancel(void)
{
    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
}

/*************************************************************
** ��������: TaskSystemDebugFixTime
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemDebugFixTime(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    unsigned long val;
    TIME_T time;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            EepromPram_ReadPram(E2_INSTALL_TIME_FLAG_ID, &temp);
            if(temp!=0x55)
            {
                s_stTaskAttrib.ControlId=0;
                PublicConfirmAll("��Ϊ��װʱ��?",SystemDebugFixTimeOk,SystemDebugFixTimeCancel,0,0);
            }
            else
            {
                val=ConvertReadPramToValue(E2_INSTALL_TIME_ID);
                Gmtime(&time,val);
                s_stTaskAttrib.ControlId=0xff;
                LcdShowCaption("��װʱ��:",1);
                LcdShowHorLine(0, 131, 18, 1);
                Public_LcdShowTime(24,time);                
                LcdShowHorLine(0, 131, 45, 1);
                LcdShowCaption("���������",47);
            }
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��                    
                    s_stTaskAttrib.ControlId=0xff;
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: SystemDebugSetPositionEx
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemDebugSetPositionEx(void)
{
    LcdClearScreen();
    //if(BMA250_RollOverInitAngleCalibration())//dxl,2016.8.13
	  BMA250E_Calibration();

    LcdShowCaption("����ɹ�", 23); 
    LZM_SetAlarm(TEST_TIMER, SECS(1));
}
/*************************************************************
** ��������: SystemDebugFixTime
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemDebugSetPosition(void)
{
    LcdClearScreen();
    LcdShowCaption("����У׼", 23); 
    LZM_SetOperateTimer(SystemDebugSetPositionEx, PUBLICSECS(1));    
}
/*************************************************************
** ��������: CurrentSystemLogout
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void CurrentSystemLogout(void)
{
    LcdClearScreen();
    Lcd_SendTerminalLogout();
    #ifdef HUOYUN_DEBUG_OPEN
    #else
    LcdClearArea(0,19,131,44);
    LcdShowCaption("ע�����ͳɹ�", 24);
    #endif
    LZM_SetAlarm(TEST_TIMER, SECS(1.5));
}
/*************************************************************
** ��������: TaskSystemDebugSetPassWord
** ��������: "�޸�����"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemDebugSetPassWord(LZM_MESSAGE *Msg)
{
    unsigned long val,val1;
    unsigned char buf[6];
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("�޸�����",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);            
            LcdShowStr(1, 24, "�� ��:", 0);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 64, 24, 6,s_stTaskAttrib.SysPassword);
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��                    
                    val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    if(val==s_stTaskAttrib.SysPassword)
                    {
                        s_stTaskAttrib.ControlId=0xff;
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                        break;
                    }
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڱ���...", 24);
                    Public_ConvertLongToBuffer(val,buf);
                    EepromPram_WritePram(E2_PASSWORD_ID, buf,4);
                    val1=ConvertReadPramToValue(E2_PASSWORD_ID);
                    LcdClearArea(0,19,131,44);
                    if(val==val1)
                    {
                        LcdClearArea(0,44,131,63);
                        LcdShowCaption("�����޸ĳɹ�", 22);
                        LcdShowCaption("���ס������", 40);
                        s_stTaskAttrib.SysPassword=(val&0x00ffffff);
                        
                    }
                    else
                    {
                        LcdShowCaption("�������,����", 24); 
                        val=s_stTaskAttrib.SysPassword;
                        Public_ConvertLongToBuffer(val,buf);
                        EepromPram_WritePram(E2_PASSWORD_ID, buf,4);
                        
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(2));
                    break;                                
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskMenuLogin
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void LoadDefaultParamter(void)
{
    LcdClearScreen();
    LcdShowCaption("���ڻָ�����",10);
    LcdShowCaption("����,���Ժ�",32);
    EepromPram_DefaultSet();//modify by joneming 20130312
    LcdClearScreen();
    LcdShowCaption("�ѻָ���������",23);    
    LZM_SetAlarm(TEST_TIMER, SECS(1.5));
}
#ifdef __LZMINPUTBOX_H
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarPlateNumber(LZM_MESSAGE *Msg)
{
    unsigned char val,i;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,GB2312Buffer);
            if(GB2312Buffer[0]<0x80)
            {
                strcpy((char *)GB2312Buffer,"��B12345"); 
            }
            else
            for(i=0; i<8; i++)  
            {
                if(GB2312Buffer[i]<0x30)
                GB2312Buffer[i]=0x30;
            }
            GB2312Buffer[8]='\0';
            LcdShowStr(30,16,"���ƺ���:",0);
            LcdShowStr(24,32,(char *)GB2312Buffer,0);
            LcdShowStr(16,48,"����",0);
            LcdShowStr(80,48,"ȡ��",0);
            LZM_InputBoxInit(&stInputBox[0],24,32,2,(char *)GB2312Buffer,(char **)c_apcInputStringEx);
            LZM_InputBoxInit(&stInputBox[1],40,32,6,(char *)&GB2312Buffer[2],(char **)c_apcInputString);
            stInputBox[0].loop = 0;
            stInputBox[0].step = 2;
            stInputBox[1].loop = 0;
            LZM_InputBoxShow(&stInputBox[0]);
            LZM_InputBoxShow(&stInputBox[1]);
            LZM_InputBoxSetFocus(&stInputBox[0]);
            s_stTaskAttrib.ControlId = 0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId < 2)
            {
                val=LZM_InputBoxResponse(&stInputBox[s_stTaskAttrib.ControlId],TaskPara);
                if(val==2||val==3)
                {
                    LZM_InputBoxLoseFocus(&stInputBox[s_stTaskAttrib.ControlId]);
                    s_stTaskAttrib.ControlId++;
                    if(s_stTaskAttrib.ControlId==1)
                    LZM_InputBoxSetFocus(&stInputBox[1]);
                    else
                     LcdShowStr(16,48,"����",1);
                }
                else
                if(val==1)
                {
                    if(0==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 3;
                        LcdShowStr(80,48,"ȡ��",1);
                    }
                    else
                    {
                        LZM_InputBoxLoseFocus(&stInputBox[s_stTaskAttrib.ControlId]);
                        s_stTaskAttrib.ControlId = 0;
                        LZM_InputBoxSetFocusEx(&stInputBox[0]);
                    }
                }
                else
                if(val)
                {
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                }
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://�˵�
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break;
                case KEY_UP://
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 1;
                        LcdShowStr(16,48,"����",0);
                        LZM_InputBoxSetFocusEx(&stInputBox[s_stTaskAttrib.ControlId]);
                    }
                    else
                    if(3==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 2;
                        LcdShowStr(16,48,"����",1);
                        LcdShowStr(80,48,"ȡ��",0);
                    }
                    break;
                case KEY_DOWN://
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 3;
                        LcdShowStr(16,48,"����",0);
                        LcdShowStr(80,48,"ȡ��",1);
                    }
                    else
                    if(3==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 0;
                        LcdShowStr(80,48,"ȡ��",0);
                        LZM_InputBoxSetFocus(&stInputBox[s_stTaskAttrib.ControlId]);
                    }
                    break;
                case KEY_ENTER://ȷ��
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        LcdClearScreen();
                        LcdShowCaption("���ڱ���...", 24);
                        strcpy((char *)GB2312Buffer,stInputBox[0].buffer);
                        strcat((char *)GB2312Buffer,stInputBox[1].buffer);
                        EepromPram_WritePram(E2_CAR_PLATE_NUM_ID, GB2312Buffer,strlen((char *)GB2312Buffer));
                        ///////////////////////////////////////
                        val=EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, (unsigned char *)s_stTaskAttrib.ShowBuffer[0]);
                        LcdClearArea(0,19,131,44);
                        if(strncmp(s_stTaskAttrib.ShowBuffer[0],(char*)GB2312Buffer,val)==0)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_CAR_PLATE_NUM_ID);                           
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);  
                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    if(3==s_stTaskAttrib.ControlId)
                    {
                        LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    }
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#else
/*************************************************************
** ��������: ShowReverseBarForOne
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowReverseBarForOne(unsigned char startx,unsigned char starty,unsigned char post)
{
    unsigned short x,y;
    y=starty;
    x=startx+post*8;
    if(x+8>128)
    {
        x -= 128;
        y += 16;
    }
    LcdReverseBar(x,y,x+7,y+15);
}
/*************************************************************
** ��������: ShowReverseBarForOneEx
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowReverseBarForOneEx(unsigned char startx,unsigned char starty,unsigned char post)
{
    unsigned short x,y;
    y=starty;
    x=startx+post*16;
    if(x+16>128)
    {
        x -= 128;
        y += 16;
    }
    LcdReverseBar(x,y,x+15,y+15);
}
/*************************************************************
** ��������: TaskSystemSetCarPlateNumber
** ��������: ""���ƺ���:""�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarPlateNumber(LZM_MESSAGE *Msg)
{
    static unsigned char sucIuputSite,sucSelectSite,sucSaveFlag,sucShowFlag;
    unsigned char temp,i;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.select=0;
            s_stTaskAttrib.ControlId=0;
            sucSaveFlag=0;
            sucIuputSite=0;
            sucSelectSite=0;
            sucShowFlag=0;
            LcdShowStr(0,0,c_apcInputStringEx[s_stTaskAttrib.ControlId],0);            
            temp=EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, GB2312Buffer);
            if(GB2312Buffer[0]<0x80)
            {
                strcpy((char *)GB2312Buffer,"��B12345"); 
            }
            else
            for(i=0; i<8; i++)  
            {
                if(GB2312Buffer[i]<0x30)
                GB2312Buffer[i]=0x30;
            }
            GB2312Buffer[8]='\0';
            LcdShowStr(30,16,"���ƺ���:",0);
            LcdShowStr(24,32,(char *)GB2312Buffer,0);
            LcdShowStr(16,48,"����",0);
            LcdShowStr(80,48,"ȡ��",0);
            LZM_SetTimer(TEST_TIMER, SECS(0.4));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    if(s_stTaskAttrib.select==1)
                    {
                        s_stTaskAttrib.select=0;
                        if(!sucShowFlag)
                        {
                            LcdShowStr(0,0,c_apcInputStringEx[s_stTaskAttrib.ControlId],0);
                        }
                        else
                        {
                            LcdShowStr(16,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                        }
                    }
                    else
                    {
                        LZM_KillTimer(TEST_TIMER);
                        LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    }
                    break;
                case KEY_ENTER://ȷ��
                    LZM_KillTimer(TEST_TIMER);
                    if(s_stTaskAttrib.select==0)
                    {
                        s_stTaskAttrib.select=1;
                        if(!sucShowFlag)
                        {
                            LcdClearArea(0,0,131,16);
                            LcdShowStr(0,0,c_apcInputStringEx[s_stTaskAttrib.ControlId],0);
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                        }
                        else
                        {
                            LcdClearArea(0,0,131,16);
                            LcdShowStr(16,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                        }
                        LZM_SetTimer(TEST_TIMER, SECS(0.4));
                    }
                    else
                    if(s_stTaskAttrib.select==1)
                    {
                        s_stTaskAttrib.select=0;
                        if(!sucShowFlag)
                        {
                            sucShowFlag=1;
                            GB2312Buffer[0]=c_apcInputStringEx[s_stTaskAttrib.ControlId][2*sucSelectSite];
                            GB2312Buffer[1]=c_apcInputStringEx[s_stTaskAttrib.ControlId][2*sucSelectSite+1];
                            s_stTaskAttrib.ControlId=0;                            
                            sucSelectSite=0;
                            sucIuputSite =1;
                        }
                        else
                        {
                            GB2312Buffer[sucIuputSite]=c_apcInputString[s_stTaskAttrib.ControlId][sucSelectSite];
                        }
                        LcdClearArea(0,0,131,16);
                        LcdShowStr(16,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                        LcdShowStr(24,32,(char *)GB2312Buffer,0);
                        /////////////////
                        ////////////////
                        if(sucIuputSite<7)
                        {
                            sucIuputSite++;
                            LZM_SetTimer(TEST_TIMER, SECS(0.4));
                        }
                        else
                        {
                            s_stTaskAttrib.select=2;
                            sucSaveFlag=0;
                            LZM_KillTimer(TEST_TIMER);                            
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==2)
                    {
                        if(sucSaveFlag==0)
                        {
                            LcdClearScreen();
                            LcdShowCaption("���ڱ���...", 24);
                            s_stTaskAttrib.select=3;
                            EepromPram_WritePram(E2_CAR_PLATE_NUM_ID, GB2312Buffer,strlen((char *)GB2312Buffer));
                            ///////////////////////////////////////
                            temp=EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, (unsigned char *)&s_stTaskAttrib.ShowBuffer[0]);
                            LcdClearArea(0,19,131,44);
                            if(strncmp((char*)&s_stTaskAttrib.ShowBuffer[0],(char*)GB2312Buffer,temp)==0)
                            {
                                LcdShowCaption("�ĸ��ɹ�", 23); 
                                EepromPram_UpdateVariable(E2_CAR_PLATE_NUM_ID);                           
                                LZM_SetAlarm(TEST_TIMER, SECS(1));
                            }
                            else
                            {
                                LcdShowCaption("�ĸ�ʧ��", 23);  
                                
                                LZM_SetAlarm(TEST_TIMER, SECS(1));
                            }
                        }
                        else
                        {
                            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                        }
                    }                    
                    break;                                
                case KEY_UP:
                    if(s_stTaskAttrib.select==0)
                    {
                        if(sucIuputSite>2)
                        {
                            sucIuputSite--;
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                        }
                        else
                        if(sucShowFlag==1) 
                        {
                            sucShowFlag=0;
                            sucIuputSite=0;
                            s_stTaskAttrib.ControlId=0;
                            sucSelectSite =0;
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                        }
                        else
                        {
                            s_stTaskAttrib.select=2;
                            sucSaveFlag=1;                            
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                            LZM_KillTimer(TEST_TIMER);
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==1)
                    {
                        if(!sucShowFlag)
                        {
                            if(s_stTaskAttrib.ControlId<3)s_stTaskAttrib.ControlId++;
                            else
                                s_stTaskAttrib.ControlId=0;
                            ///////////////////////////
                            temp =(strlen(c_apcInputStringEx[s_stTaskAttrib.ControlId]))>>1;
                            temp--;
                            if(sucSelectSite>temp)sucSelectSite=temp;
                            LcdClearArea(100, 0, 128,16);
                            ///////////////////////////
                            LcdShowStr(0,0,c_apcInputStringEx[s_stTaskAttrib.ControlId],0);
                        }
                        else
                        {
                            if(s_stTaskAttrib.ControlId<2)s_stTaskAttrib.ControlId++;
                            else
                                s_stTaskAttrib.ControlId=0;
                            LcdShowStr(16,0,c_apcInputString[s_stTaskAttrib.ControlId],0);

                        }
                    }
                    else
                    if(s_stTaskAttrib.select==2)
                    {
                        if(sucSaveFlag==1)
                        {
                            sucSaveFlag=0;
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                        else
                        if(sucSaveFlag==0)
                        {
                            LZM_SetTimer(TEST_TIMER, SECS(0.4));
                            s_stTaskAttrib.select=0;
                            sucIuputSite=7;
                            sucShowFlag=1;
                            s_stTaskAttrib.ControlId=0;
                            sucSelectSite =0;
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                            LcdShowStr(16,48,"����",0);
                            LcdShowStr(80,48,"ȡ��",0);
                        }
                    }
                    break;
                case KEY_DOWN: 
                    if(s_stTaskAttrib.select==0)
                    {
                        if(sucShowFlag==0)
                        {
                            sucShowFlag=1;
                            s_stTaskAttrib.ControlId=0;
                            sucSelectSite =0;
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                            sucIuputSite=2;
                        }
                        else
                        {
                            if(sucIuputSite<7)
                            {
                                sucIuputSite++;
                                LcdShowStr(24,32,(char *)GB2312Buffer,0);
                            }
                            else
                            {
                                s_stTaskAttrib.select=2;
                                sucSaveFlag=0;
                                LZM_KillTimer(TEST_TIMER);
                                LcdShowStr(24,32,(char *)GB2312Buffer,0);
                                LcdShowStr(16,48,"����",1-sucSaveFlag);
                                LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                            }
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==1)
                    {
                        if(sucShowFlag==0)
                        {
                            LcdShowStr(0,0,c_apcInputStringEx[s_stTaskAttrib.ControlId],0); 
                            temp=(strlen(c_apcInputStringEx[s_stTaskAttrib.ControlId]))>>1;
                            temp--;
                            if(sucSelectSite< temp )sucSelectSite++;
                            else sucSelectSite=0;
                        }
                        else
                        {
                            LcdShowStr(16,0,c_apcInputString[s_stTaskAttrib.ControlId],0); 
                            if(sucSelectSite<11)sucSelectSite++;
                            else sucSelectSite=0;
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==2)
                    {
                        if(sucSaveFlag==0)
                        {
                            sucSaveFlag=1;                            
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                        else
                        if(sucSaveFlag==1)
                        {
                            LZM_SetTimer(TEST_TIMER, SECS(0.4));
                            LcdShowStr(16,48,"����",0);
                            LcdShowStr(80,48,"ȡ��",0);
                            s_stTaskAttrib.select=0;
                            sucShowFlag=0;
                            sucIuputSite=0;
                            s_stTaskAttrib.ControlId=0;
                            sucSelectSite =0;
                            LcdShowStr(24,32,(char *)GB2312Buffer,0);
                        }
                    }                    
                    break;
            }
            return 1;
        case TEST_TIMER:            
            if(s_stTaskAttrib.select==0)
            {
                if(!sucShowFlag)
                {
                    ShowReverseBarForOneEx(24,32,0);
                }
                else
                {
                    ShowReverseBarForOne(24,32,sucIuputSite);
                }
            }
            else
            if(s_stTaskAttrib.select==1)
            {
                if(!sucShowFlag)
                {
                    ShowReverseBarForOneEx(0,0,sucSelectSite);
                }
                else
                {
                    ShowReverseBarForOne(16,0,sucSelectSite);
                }
            }
            else
            if(s_stTaskAttrib.select==3)
            {
                LZM_KillTimer(TEST_TIMER);
                LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            }
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif
/*************************************************************
** ��������: TaskSetCarPlateColor
** ��������: "���ó�����ɫ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarPlateColor(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("ѡ������ɫ",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],36,24,(char **)c_apcCarPlakeColorItems,"");
            EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, &handle); 
            if(handle>5)handle=5;
            else if(handle==0)handle=6;
            s_stTaskAttrib.select=handle;
            s_stSpin[0].handle=handle-1;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;            
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0])+1;
                    if(handle!=s_stTaskAttrib.select)
                    {
                        if(5==handle)handle=9;
                        else if(6==handle)handle=0;
                        s_stTaskAttrib.select=handle;                        
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_CAR_PLATE_COLOR_ID, &handle, 1);
                        EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_CAR_PLATE_COLOR_ID);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemSetCarPlateType
** ��������: "���ó��Ʒ���"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarPlateType(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    unsigned char buffer[30];
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("ѡ���Ʒ���",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            EepromPram_ReadPram(E2_CAR_TYPE_ID, buffer);
            SpinInit(&s_stSpin[0],16,24,(char **)c_apcCarPlakeTypeItems,"");
            s_stTaskAttrib.select=0xff;
            for(handle=0;handle<11;handle++)
            {
                if(strncmp((char*)buffer,(char*)c_apcCarPlakeTypeItems[handle],8)==0)
                {
                    s_stTaskAttrib.select=handle;
                    break;
                }
            }
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select=handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);
                        EepromPram_WritePram(E2_CAR_TYPE_ID, (unsigned char *)c_apcCarPlakeTypeItems[handle], strlen(c_apcCarPlakeTypeItems[handle]));                        
                        EepromPram_ReadPram(E2_CAR_TYPE_ID, buffer);
                        LcdClearArea(0,19,131,44);
                        if(strncmp((char*)buffer,(char*)c_apcCarPlakeTypeItems[handle],4)==0)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_CAR_TYPE_ID);                           
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);  
                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

#ifdef __LZMINPUTBOX_H
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarVinNumber(LZM_MESSAGE *Msg)
{
    unsigned char val,i;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, GB2312Buffer);
            if(GB2312Buffer[0]<0x30||GB2312Buffer[0]>0x80)
            {
                strcpy((char *)GB2312Buffer,"LSVAE000000000000");
            }
            else
            for(i=0; i<17; i++)
            {
                if(GB2312Buffer[i]<0x30||GB2312Buffer[i]>0x80)
                GB2312Buffer[i]=0x30;
            }
            GB2312Buffer[17]='\0';
            LcdShowStr(2,16,"����VIN:",0);
            LcdShowStr(16,48,"����",0);
            LcdShowStr(80,48,"ȡ��",0);
            LZM_InputBoxInit(&stInputBox[0],72,16,17,(char *)GB2312Buffer,(char **)c_apcInputString);
            stInputBox[0].loop = 0;
            LZM_InputBoxSetFocus(&stInputBox[0]);
            s_stTaskAttrib.ControlId = 0;
            return 1;
        case KEY_PRESS:
            if(0==s_stTaskAttrib.ControlId)
            {
                val=LZM_InputBoxResponse(&stInputBox[0],TaskPara);
                if(val==2||val==3)
                {
                    s_stTaskAttrib.ControlId = 1;
                    LcdShowStr(16,48,"����",1);
                }
                else
                if(val==1)
                {
                    s_stTaskAttrib.ControlId = 2;
                    LcdShowStr(80,48,"ȡ��",1);
                }
                else
                if(val)
                {
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                }
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://�˵�
                    LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    break;
                case KEY_UP://
                    if(1==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 0;
                        LcdShowStr(16,48,"����",0);
                        LZM_InputBoxSetFocusEx(&stInputBox[0]);
                    }
                    else
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 1;
                        LcdShowStr(16,48,"����",1);
                        LcdShowStr(80,48,"ȡ��",0);
                    }
                    break;
                case KEY_DOWN://
                    if(1==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 2;
                        LcdShowStr(16,48,"����",0);
                        LcdShowStr(80,48,"ȡ��",1);
                    }
                    else
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 0;
                        LcdShowStr(80,48,"ȡ��",0);
                        LZM_InputBoxSetFocus(&stInputBox[0]);
                    }
                    break;
                case KEY_ENTER://ȷ��
                    if(1==s_stTaskAttrib.ControlId)
                    {
                        LcdClearScreen();
                        LcdShowCaption("���ڱ���...", 24);
                        EepromPram_WritePram(E2_CAR_IDENTIFICATION_CODE_ID,(unsigned char *)stInputBox[0].buffer,strlen(stInputBox[0].buffer));
                        ////////////////////////////////////
                        val=EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, (unsigned char *)&s_stTaskAttrib.ShowBuffer[0]);
                        LcdClearArea(0,19,131,44);
                        if(strncmp(s_stTaskAttrib.ShowBuffer[0],stInputBox[0].buffer,val)==0)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_CAR_IDENTIFICATION_CODE_ID);                           
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);                             
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    }
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#else
/*************************************************************
** ��������: TaskSystemSetCarVinNumber
** ��������: "����VIN"
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemSetCarVinNumber(LZM_MESSAGE *Msg)
{
    static unsigned char sucIuputSite,sucSelectSite,sucSaveFlag;
    unsigned char temp,i;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            s_stTaskAttrib.select=0;
            s_stTaskAttrib.ControlId=0;
            sucSaveFlag=0;
            sucIuputSite=0;
            sucSelectSite=0;
            LcdShowStr(12,0,c_apcInputString[s_stTaskAttrib.ControlId],0);            
            temp=EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, GB2312Buffer);
            if(GB2312Buffer[0]<0x30||GB2312Buffer[0]>0x80)
            {
                strcpy((char *)GB2312Buffer,"LSVAE000000000000");
            }
            else
            for(i=0; i<17; i++)
            {
                if(GB2312Buffer[i]<0x30||GB2312Buffer[i]>0x80)
                GB2312Buffer[i]=0x30;
            }
            GB2312Buffer[17]='\0';
            LcdShowStr(2,16,"����VIN:",0);
            LcdShowStr(72,16,(char *)GB2312Buffer,0);
            LcdShowStr(16,48,"����",0);
            LcdShowStr(80,48,"ȡ��",0);
            LZM_SetTimer(TEST_TIMER, SECS(0.4));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    if(s_stTaskAttrib.select==1)
                    {
                        s_stTaskAttrib.select=0;
                        LcdShowStr(12,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                        LcdShowStr(72,16,(char *)GB2312Buffer,0);
                    }
                    else
                    {
                        LZM_KillTimer(TEST_TIMER);
                        LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                    }
                    break;
                case KEY_ENTER://ȷ��
                    LZM_KillTimer(TEST_TIMER);
                    if(s_stTaskAttrib.select==0)
                    {
                        s_stTaskAttrib.select=1;
                        LcdShowStr(12,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                        LcdShowStr(72,16,(char *)GB2312Buffer,0);
                        LZM_SetTimer(TEST_TIMER, SECS(0.4));
                    }
                    else
                    if(s_stTaskAttrib.select==1)
                    {
                        s_stTaskAttrib.select=0;
                        LcdShowStr(12,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                        GB2312Buffer[sucIuputSite]=c_apcInputString[s_stTaskAttrib.ControlId][sucSelectSite];
                        LcdShowStr(72,16,(char *)GB2312Buffer,0);
                        if(sucIuputSite<16)
                        {
                            sucIuputSite++;
                            LZM_SetTimer(TEST_TIMER, SECS(0.4));
                        }
                        else
                        {
                            s_stTaskAttrib.select=2;
                            sucSaveFlag=0;
                            LZM_KillTimer(TEST_TIMER);                            
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==2)
                    {
                        if(sucSaveFlag==0)
                        {
                            LcdClearScreen();
                            LcdShowCaption("���ڱ���...", 24);
                            s_stTaskAttrib.select=3;
                            EepromPram_WritePram(E2_CAR_IDENTIFICATION_CODE_ID, GB2312Buffer,strlen((char *)GB2312Buffer));
                            ////////////////////////////////////
                            temp=EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID, (unsigned char *)&s_stTaskAttrib.ShowBuffer[0]);
                            LcdClearArea(0,19,131,44);
                            if(strncmp((char*)&s_stTaskAttrib.ShowBuffer[0],(char*)GB2312Buffer,temp)==0)
                            {
                                LcdShowCaption("�ĸ��ɹ�", 23); 
                                EepromPram_UpdateVariable(E2_CAR_IDENTIFICATION_CODE_ID);                           
                                LZM_SetAlarm(TEST_TIMER, SECS(1));
                            }
                            else
                            {
                                LcdShowCaption("�ĸ�ʧ��", 23);  
                                
                                LZM_SetAlarm(TEST_TIMER, SECS(1));
                            }
                        }
                        else
                        {
                            LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
                        }
                    }                    
                    break;                                
                case KEY_UP:
                    if(s_stTaskAttrib.select==0)
                    {
                        if(sucIuputSite)
                        {
                            sucIuputSite--;
                            LcdShowStr(72,16,(char *)GB2312Buffer,0);
                        }
                        else
                        {
                            s_stTaskAttrib.select=2;
                            sucSaveFlag=1;
                            LcdShowStr(72,16,(char *)GB2312Buffer,0);
                            LZM_KillTimer(TEST_TIMER);
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==1)
                    {
                        if(s_stTaskAttrib.ControlId<2)s_stTaskAttrib.ControlId++;
                        else
                            s_stTaskAttrib.ControlId=0;
                        LcdShowStr(12,0,c_apcInputString[s_stTaskAttrib.ControlId],0);
                    }
                    else
                    if(s_stTaskAttrib.select==2)
                    {
                        if(sucSaveFlag==1)
                        {
                            sucSaveFlag=0;
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                        else
                        if(sucSaveFlag==0)
                        {
                            LZM_SetTimer(TEST_TIMER, SECS(0.4));
                            s_stTaskAttrib.select=0;
                            sucIuputSite=16;
                            LcdShowStr(16,48,"����",0);
                            LcdShowStr(80,48,"ȡ��",0);
                        }
                    }
                    break;
                case KEY_DOWN: 
                    if(s_stTaskAttrib.select==0)
                    {
                        if(sucIuputSite<16)
                        {
                            sucIuputSite++;
                            LcdShowStr(72,16,(char *)GB2312Buffer,0);
                        }
                        else
                        {
                            s_stTaskAttrib.select=2;
                            sucSaveFlag=0;
                            LZM_KillTimer(TEST_TIMER);
                            LcdShowStr(72,16,(char *)GB2312Buffer,0);
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                    }
                    else
                    if(s_stTaskAttrib.select==1)
                    {
                        LcdShowStr(12,0,c_apcInputString[s_stTaskAttrib.ControlId],0); 
                        if(sucSelectSite<11)sucSelectSite++;
                        else sucSelectSite=0;
                    }
                    else
                    if(s_stTaskAttrib.select==2)
                    {
                        if(sucSaveFlag==0)
                        {
                            sucSaveFlag=1;                            
                            LcdShowStr(16,48,"����",1-sucSaveFlag);
                            LcdShowStr(80,48,"ȡ��",sucSaveFlag);
                        }
                        else
                        if(sucSaveFlag==1)
                        {
                            LZM_SetTimer(TEST_TIMER, SECS(0.4));
                            s_stTaskAttrib.select=0;
                            sucIuputSite=0;
                            LcdShowStr(72,16,(char *)GB2312Buffer,0);
                            LcdShowStr(16,48,"����",0);
                            LcdShowStr(80,48,"ȡ��",0);
                        }
                    }                    
                    break;
            }
            return 1;
        case TEST_TIMER:            
            if(s_stTaskAttrib.select==0)
            {
                ShowReverseBarForOne(72,16,sucIuputSite);
            }
            else
            if(s_stTaskAttrib.select==1)
            {
                ShowReverseBarForOne(12,0,sucSelectSite);
            }
            else
            if(s_stTaskAttrib.select==3)
            {
                LZM_KillTimer(TEST_TIMER);
                LZM_StartNewTask(TaskMenuSystemCarInfo);//ϵͳ����
            }
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif

/*************************************************************
** ��������: TaskSystemCarDoorSet
** ��������: "������Чѡ��"
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemParamDoorLevelSelect(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("������Чѡ��",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],24,24,(char **)c_apcCarDoorSetItems,"");
            EepromPram_ReadPram(E2_DOOR_SET_ID, &handle);            
            s_stTaskAttrib.select=handle;
            if(handle>1)handle=1;
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select=handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_DOOR_SET_ID, &handle, 1);  
                        EepromPram_ReadPram(E2_DOOR_SET_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_DOOR_SET_ID);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);  
                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemCarDoorSet
** ��������: "������Чѡ��"
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemParamHornLevelSelect(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("�����ȵ�ƽѡ��",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],24,24,(char **)c_apcCarHornSetItems,"");
            EepromPram_ReadPram(E2_HORN_SET_ID, &handle);            
            s_stTaskAttrib.select=handle;
            if(handle>1)handle=1;
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select=handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_HORN_SET_ID, &handle, 1);  
                        EepromPram_ReadPram(E2_HORN_SET_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_HORN_SET_ID);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemCDMASleep
** ��������: CDMA���߿���
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemParamSetCDMASleep(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("CDMA���߿���",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            LcdShowStr(8,24,"����:",0);
            SpinInit(&s_stSpin[0],64,24,(char **)c_apcCDMASleepItems,"");
            EepromPram_ReadPram(E2_CDMA_SLEEP_ID, &handle);            
            s_stTaskAttrib.select=handle;
            if(handle!=1)handle=0;
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select = handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_CDMA_SLEEP_ID, &handle, 1);
                        EepromPram_ReadPram(E2_CDMA_SLEEP_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_CDMA_SLEEP_ID);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);  
                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskSystemDebugShowIOstatus
** ��������: "����״̬��ʾ"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemShowTerminalID(LZM_MESSAGE *Msg)
{
    unsigned char buff[20],buffer[10];
    unsigned char len,i,temp;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();            
            LcdShowCaption("�ն�ID��:",2);
            len=EepromPram_ReadPram(E2_DEVICE_ID, buffer);
            ////////////////
            temp=Public_BCD2HEX(buffer[0]);
            sprintf((char *)&buff[0],"%d",temp);
            for(i=1; i<len; i++)
            {
                temp=Public_BCD2HEX(buffer[i]);
                sprintf((char *)&buff[2*i-1],"%02d",temp);
            }
            LcdShowCaption((char *)buff,22);
            LcdShowHorLine(0, 131, 43, 1);
            LcdShowCaption("���������",47);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                case KEY_ENTER://ȷ��                               
                case KEY_UP:
                case KEY_DOWN:
                    LZM_StartNewTask(TaskMenuSystemParameter);//ϵͳ����
                    break;
            }
            return 1;
        case TEST_TIMER:
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemParamSetGNSSWordMode
** ��������: ���ñ���ģ�鹤��ģʽ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemParamSetGNSSWordMode(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    unsigned char i,index;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();  
            index =0;
            if(!EepromPram_ReadPram(E2_GPS_SET_MODE_ID,&index))
            {
                PUBLIC_SETBIT(index,2);
            }
            LcdShowCaption("GNSS��λģʽ",0);
            LcdShowHorLine(0, 131, 18, 1);
            s_stTaskAttrib.ControlId = 0;
            s_stTaskAttrib.ControlMaxId =4;
            for(i=0; i<2; i++)
            {
                LcdShowStr(0, 22+i*22, c_apcGNSSWorkModeShow[i], 0);
                ////////////////////////
                SpinInit(&s_stSpin[i],80,22+i*22,(char **)c_apcGNSSworkModeItems,"");             
                //////////////////////////
                SpinInit(&s_stSpin[i+2],80,22+i*22,(char **)c_apcGNSSworkModeItems,"");
                /////////////////////
                s_stSpin[i].keyflag = 1;
                s_stSpin[i].handle = PUBLIC_CHECKBIT_EX(index,i);
                s_stSpin[i+2].keyflag = 1;
                s_stSpin[i+2].handle =PUBLIC_CHECKBIT_EX(index,i+2);
                ///////////////////////////
                SpinShow(&s_stSpin[i]);
            }
            /////////////////////////////////////////
            SpinSetFocus(&s_stSpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemParameter);//
                    break;
                case KEY_ENTER://ȷ��
                    LcdClearArea(0,19,131,62);
                    LcdShowCaption("���ڱ���", 26); 
                    index = 0;
                    for(i=0; i<s_stTaskAttrib.ControlMaxId; i++)
                    {
                        handle=SpinGetHandle(&s_stSpin[i]);
                        if(handle)
                        PUBLIC_SETBIT(index, i);
                    }
                    EepromPram_WritePram(E2_GPS_SET_MODE_ID,&index,1);
                    handle = 0;
                    EepromPram_ReadPram(E2_GPS_SET_MODE_ID,&handle);
                    if(index==handle)
                    {
                        LcdShowCaption("����ɹ�", 26); 
                        EepromPram_UpdateVariable(E2_GPS_SET_MODE_ID);
                    }
                    else
                    {
                        LcdShowCaption("����ʧ��", 26);
                    }
                    LZM_SetAlarm(TEST_TIMER, SECS(2));
                    break;                                
                default:                    
                    i=SpinResponse(&s_stSpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(i)
                    {
                        SpinLoseFocus(&s_stSpin[s_stTaskAttrib.ControlId]);
                        s_stTaskAttrib.ControlId++;
                        if(s_stTaskAttrib.ControlId>=s_stTaskAttrib.ControlMaxId) s_stTaskAttrib.ControlId =0;
                        //////////////////////////////
                        if(2==s_stTaskAttrib.ControlId||0==s_stTaskAttrib.ControlId)
                        {
                            for(i=0; i<2; i++)
                            {
                                LcdShowStr(0, 22+i*22, c_apcGNSSWorkModeShow[s_stTaskAttrib.ControlId+i], 0);
                                ///////////////////////////
                                SpinShow(&s_stSpin[s_stTaskAttrib.ControlId+i]);
                            }
                        }
                        //////////////////////////////
                        SpinSetFocus(&s_stSpin[s_stTaskAttrib.ControlId]);
                    }

                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemParameter);//
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskMenuSystemAdvanced
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystemCarInfo(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemCarInfo,s_apcMenuSystemCarInfo);
            MenuShow(&s_sutMenuSystemCarInfo);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSystemCarInfo))
                    {
                        case MENU_CAR_SET_PLATE_NUM:
                            LZM_StartNewTask(TaskSystemSetCarPlateNumber);
                            break;
                        case MENU_CAR_SET_VIN_NUM:
                            LZM_StartNewTask(TaskSystemSetCarVinNumber);
                            break;
                        case MENU_CAR_SET_PLATE_COLOR:
                            LZM_StartNewTask(TaskSystemSetCarPlateColor);
                            break;
                        case MENU_CAR_SET_PLATE_TYPE:
                            LZM_StartNewTask(TaskSystemSetCarPlateType);
                            break;
                        case MENU_CAR_SET_TOTAL_MILE:
                            LZM_StartNewTask(TaskSystemSetCarTotalMile);
                            break;  
                         case MENU_CAR_SET_FEATUER_COEF:
                            LZM_StartNewTask(TaskSystemSetCarFeatureCoef);
                            break;
                         case MENU_CAR_SET_PROVINCE_ID:
                            LZM_StartNewTask(TaskSystemSetCarProvinceID);
                            break;
                         case MENU_CAR_SET_CITY_ID:
                            LZM_StartNewTask(TaskSystemSetCarCityID);
                            break;
                         case MENU_CAR_SET_OWNER_PHONE:
                             LZM_StartNewTask(TaskSystemSetCarOwnerPhone);
                             break;
                            
                        default:
                            break;                  
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSystemCarInfo,TaskPara);
                    break;
            }
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    } 
}
/*************************************************************
** ��������: TaskMenuSystemServer
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystemManageServer(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSysManageServer,s_apcSysManageServer);
            MenuShow(&s_sutMenuSysManageServer);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSysManageServer))
                    {
                        case MENU_SYS_MANAGE_SERVER_QUERY:
                            LZM_StartNewTask(TaskSystemQueryServerParameter);
                            break;
                        case MENU_SYS_MANAGE_SERVER_SET:
                            PublicGetPasswordTo(_TASK(TaskMenuSystemServer),123456);
                            break;
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSysManageServer,TaskPara);
                    break;
            }
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    } 
}
/*************************************************************
** ��������: TaskMenuSystemServer
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**�޸���      : fanqinghai
**�޸����� :���case MENU_SERVER_FREIGHT: case MENU_SERVER_EYE_PLATFORM:������Ŀ
**������� :  2015.05.18
*************************************************************/  
LZM_RET TaskMenuSystemServer(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemServer,s_apcMenuSystemServer);
            MenuShow(&s_sutMenuSystemServer);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemManageServer);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSystemServer))
                    {
                        case MENU_SERVER_SET_IP:
                            LZM_StartNewTask(TaskSystemServerSetIP);
                            break;
                        case MENU_SERVER_SET_BACKUP_IP:
                            LZM_StartNewTask(TaskSystemServerSetBackUpIP);
                            break;
                        case MENU_SERVER_SET_APN:
                            LZM_StartNewTask(TaskSystemServerSetAPN);
                            break;                        
                        case MENU_SERVER_SET_TCP:
                            LZM_StartNewTask(TaskSystemServerSetTCPPort);
                            break;
                        case MENU_SERVER_SET_UDP:
                            LZM_StartNewTask(TaskSystemServerSetUDPPort);
                            break;
                        case MENU_SERVER_MAIN_DNS:
                            LZM_StartNewTask(TaskSystemServerSetMainDomainName);
                            break;
                        case MENU_SERVER_BAK_DNS:
                            LZM_StartNewTask(TaskSystemServerSetBackUpDomainName);
                            break;
                        case MENU_SERVER_SET_IP2:
                            LZM_StartNewTask(TaskSystemServerSetIP2);
                            break;
                        case MENU_SERVER_SET_BACKUP_IP2:
                            LZM_StartNewTask(TaskSystemServerSetBackUpIP2);
                            break;                      
                        case MENU_SERVER_SET_TCP2:
                            LZM_StartNewTask(TaskSystemServerSetTCPPort2);
                            break;
                        case MENU_SERVER_SET_UDP2:
                            LZM_StartNewTask(TaskSystemServerSetUDPPort2);
                            break;
                        case MENU_SERVER_FREIGHT: 
                            PublicConfirmAll("�Ƿ񱣴��޸�?",IP1SaveFreight,IP1CancelModify,0,0);
                            break;
                        case MENU_SERVER_EYE_PLATFORM:
                            PublicConfirmAll("�Ƿ񱣴��޸�?",IP1SaveEyePlat,IP1CancelModify,0,0);
                            break;

                        default:
                            break;                  
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSystemServer,TaskPara);
                    break;
            }
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    } 
}
/*************************************************************
** ��������: IP1SaveFreight
** ��������: "ȷ�ϱ�����IP1ֵΪȫ������?
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����     : fanqinghai
**������� :  2015.05.18
*************************************************************/  
void IP1SaveFreight()
{
        unsigned char val,buf[16];
        unsigned char tmpbuf[16];
        u8 MainIp[]="jt1.gghypt.net";
        u8 BackIp[]="jt2.gghypt.net";
        const char TCP[]="7008";
        unsigned long tmpTCP,Readval;

        LcdClearScreen();
        LcdShowCaption("���ڱ���...", 24);
        //sprintf((char*)buf,"%d.%d.%d.%d",MainIp[0],MainIp[1],MainIp[2],MainIp[3]);
        val=strlen((char*)MainIp);
        EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, MainIp,val);
        EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, tmpbuf);
        
        LcdClearArea(0,19,131,44);
        if(strncmp((char*)MainIp,(char*)tmpbuf,val)==0)
        {
            ;
        }
        else
        {
           LcdShowCaption("��IP1�������", 24); 
           LZM_SetAlarm(TEST_TIMER, SECS(1));
           return;
        }
        val=strlen((char*)BackIp);
        EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, BackIp,val);
        EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, tmpbuf);
        LcdClearArea(0,19,131,44);
        if(strncmp((char*)BackIp,(char*)tmpbuf,val)==0)
        {
            ;
        }
        else
        {
           LcdShowCaption("����IP1�������", 24); 
           LZM_SetAlarm(TEST_TIMER, SECS(1));
           return;
        }
        //val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
        //Public_ConvertLongToBuffer(val,buf);
        tmpTCP=atol(TCP);
        Public_ConvertLongToBuffer(tmpTCP,buf);
        EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, buf,4);                    
        Readval=ConvertReadPramToValue(E2_MAIN_SERVER_TCP_PORT_ID);
        LcdClearArea(0,19,131,44);
        if(tmpTCP==Readval)
        {
            LcdShowCaption("IP1����ɹ�", 24);
            EepromPram_UpdateVariable(E2_MAIN_SERVER_TCP_PORT_ID);
        }
        else
        {
           LcdShowCaption("TCP�������", 24); 
           
        }
        s_stTaskAttrib.ControlId=0xff;
        LZM_SetAlarm(TEST_TIMER, SECS(1));

}
/*************************************************************
** ��������: IP1SaveEyePlat
** ��������: "ȷ�ϱ�����IP1ֵΪ����ƽ̨
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����     : fanqinghai
**������� :  2015.05.18
*************************************************************/  
void IP1SaveEyePlat()
{
    unsigned char val,buf[16];
    unsigned char tmpbuf[16];
    unsigned char MainIp[]="113.105.77.214";
    unsigned char BackIp[]="113.105.77.214";
        const char TCP[]="9501";
        
        unsigned long tmpTCP,Readval;
    //  unsigned char BackIp[4]={113,105,77,214};
        LcdClearScreen();
        LcdShowCaption("���ڱ���...", 24);
        val=strlen((char*)MainIp);
        EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, MainIp,val);
        EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, tmpbuf);
        
        LcdClearArea(0,19,131,44);
        if(strncmp((char*)MainIp,(char*)tmpbuf,val)==0)
        {
        ;
        }
        else
        {
           LcdShowCaption("��IP1�������", 24); 
           LZM_SetAlarm(TEST_TIMER, SECS(1));
           return;
        }
        val=strlen((char*)BackIp);
        EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, BackIp,val);
        EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, tmpbuf);
        LcdClearArea(0,19,131,44);
        if(strncmp((char*)BackIp,(char*)tmpbuf,val)==0)
        {
        ;
        }
        else
        {
           LcdShowCaption("����IP1�������", 24); 
           LZM_SetAlarm(TEST_TIMER, SECS(1));
           return;
        }
        //val=atol(s_stESpin[s_stTaskAttrib.ControlId].buffer);
        //Public_ConvertLongToBuffer(val,buf);
        tmpTCP=atol(TCP);
        Public_ConvertLongToBuffer(tmpTCP,buf);
        EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, buf,4);                    
        Readval=ConvertReadPramToValue(E2_MAIN_SERVER_TCP_PORT_ID);
        LcdClearArea(0,19,131,44);
        if(tmpTCP==Readval)
        {
            LcdShowCaption("IP1����ɹ�", 24);
            EepromPram_UpdateVariable(E2_MAIN_SERVER_TCP_PORT_ID);
        }
        else
        {
           LcdShowCaption("TCP�������", 24); 
           
        }
        s_stTaskAttrib.ControlId=0xff;
        LZM_SetAlarm(TEST_TIMER, SECS(1));
}

/*************************************************************
** ��������: IP1CancelModify
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����     : fanqinghai
**������� :  2015.05.18
*************************************************************/  
void IP1CancelModify(void)
{
    LZM_StartNewTask(TaskMenuSystemServer);//
}

/*************************************************************
** ��������: Detection_Changed_Status
** ��������: ��ⷢ���仯��״̬λ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����     : fanqinghai
**������� :  2015.07.13
*************************************************************/  
void Detection_Changed_Status()
{
    static u32 OldStatus = 0;
    static u32 NewStatus = 0;
    static u8 time = 0;
    u32 ChangedStatus =0;
    u8 index = 0;
    u8 i =0;

    if(!time)
    {
        time = 1;
        NewStatus =  Io_ReadStatus();
        OldStatus =  Io_ReadStatus();
    }
    else
    {
        NewStatus =  Io_ReadStatus();
    }
    ChangedStatus = OldStatus ^ NewStatus;
    
    OldStatus =  Io_ReadStatus();
    if(ChangedStatus)
    {
        for(i =0;i<32;i++)
        {
        
            if(1 == ChangedStatus &0x01)
            {
                switch(index)
                {
                    case STATUS_BIT_ACC:
                         if(Io_ReadStatusBit(STATUS_BIT_ACC))      
                         {
                             Public_PlayTTSVoiceStr("ACC����");    
                             LcdShowStr(30, 48, "ACC����",0);             
                         }
                         else 
                         {                              
                             Public_PlayTTSVoiceStr("ACC�Ͽ�"); 
                             LcdShowStr(30, 48, "ACC����",0);
                         }
                        break;
                    case STATUS_BIT_NAVIGATION:
                        if(Io_ReadStatusBit(STATUS_BIT_NAVIGATION))      
                        {
                            Public_PlayTTSVoiceStr("�Ѷ�λ");    
                            LcdShowStr(18, 48, "�Ѷ�λ",0);             
                        }
                        else 
                        {                              
                            Public_PlayTTSVoiceStr("δ��λ"); 
                            LcdShowStr(18, 48, "δ��λ",0);
                        }
                        break;
                    case STATUS_BIT_SOUTH:
                        break;
                    case STATUS_BIT_EAST:
                        break;
                    case STATUS_BIT_STOP_WORK:
                        break;
                    case STATUS_BIT_ENCRYPT:
                        break;
                    case STATUS_BIT_FULL_LOAD2:
                        break;
                    case STATUS_BIT_OIL_CUT:
                        break;
                    case STATUS_BIT_CIRCUIT_CUT:
                        break;
                    case STATUS_BIT_DOOR_LOCK:
                        break;
                    case STATUS_BIT_DOOR1:
                        if(Io_ReadStatusBit(STATUS_BIT_DOOR1))      
                        {
                            Public_PlayTTSVoiceStr("ǰ�ſ�");    
                            LcdShowStr(18, 48, "ǰ�ſ�",0);             
                        }
                        else 
                        {                              
                            Public_PlayTTSVoiceStr("ǰ�Ź�"); 
                            LcdShowStr(18, 48, "ǰ�Ź�",0);
                        }
                        break;
                    case STATUS_BIT_DOOR2:
                        if(Io_ReadStatusBit(STATUS_BIT_DOOR2))      
                        {
                            Public_PlayTTSVoiceStr("���ſ�");    
                            LcdShowStr(18, 48, "���ſ�",0);             
                        }
                        else 
                        {                              
                            Public_PlayTTSVoiceStr("���Ź�"); 
                            LcdShowStr(18, 48, "���Ź�",0);
                        }
                        break;
                    case STATUS_BIT_DOOR3:
                        break;
                    case STATUS_BIT_DOOR4:
                        break;
                    case STATUS_BIT_DOOR5:
                        break;
                    case STATUS_BIT_GPS:
                        break;
                    case STATUS_BIT_COMPASS:
                        break;
                    case STATUS_BIT_GLONASS:
                        break;
                    case STATUS_BIT_GALILEO:
                        break;
                    default:
                        break;
        
                }
            }
            ChangedStatus >>= 1;
            index ++; 
        }
    }
    

}
/*************************************************************
** ��������: TaskMenuSystemInstallModel
** ��������: ��װ����ʱ������������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystemInstallModel(LZM_MESSAGE *Msg)
{
    Detection_Changed_Status();
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            LcdShowStr(30, 1, "��װģʽ",0);
            LcdShowRect(0,17,132,17,0);   //������
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemDebug);
                    break;
                case KEY_ENTER:
                    break;
                default:
                    break;
            }
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    } 
}
/*************************************************************
** ��������: TaskMenuSystemDebug
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystemDebug(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemDebug,s_apcMenuSystemDebug);
            MenuShow(&s_sutMenuSystemDebug);   
            s_stTaskAttrib.ControlId =0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSystemDebug))
                    {
                        case MENU_DEBUG_IO_IN:
                            LZM_StartNewTask(TaskSystemDebugShowIOstatus);
                            break;
                        case MENU_DEBUG_IO_OUT:
                            LZM_StartNewTask(TaskSystemDebugIOOut);
                            break;
                        case MENU_DEBUG_REPORT_MODE:
                            LZM_StartNewTask(TaskSystemDebugCurrentReportMode);
                            break;
                        case MENU_DEBUG_CALI_FEATUER_COEF:
                            CalibrationFeatureCoef(); 
                            break;
                        case MENU_DEBUG_FIX_TIME:
                            LZM_StartNewTask(TaskSystemDebugFixTime);
                            break;  
                        case MENU_DEBUG_POSITION:
                            PublicConfirm("�෭�Ƕ�У׼?",SystemDebugSetPosition);
                            break; 
                        case MENU_DEBUG_LOGOUT:
                            PublicConfirm("��Ҫע���ն�?",CurrentSystemLogout);
                            break;
                        case MENU_DEBUG_PASSWORD:
                            LZM_StartNewTask(TaskSystemDebugSetPassWord);
                            break;
                        case MENU_DEBUG_RESET:
                            PublicConfirm("�Ƿ������ն�?",NVIC_SystemReset);
                            break;
                        case MENU_DEBUG_TEST:
                            LZM_StartNewTask(TaskMenuSystemDebugTest);//
                            break;
         //               case MENU_DEBUG_TTS_INSTALL:
          //                  LZM_StartNewTask(TaskMenuSystemInstallModel); //�����:fanqinghai ���ʱ��:2015.07.09 
                          // break;
                         case MENU_DEBUG_DEEP_MIN:
                            LZM_StartNewTask(TaskSystemDeepMinCalib);//
                            break;
                          case MENU_DEBUG_DEEP_MAX:
                            LZM_StartNewTask(TaskSystemDeepMaxCalib);//
                            break;
                         case MENU_DEBUG_DEEP_BASE:
                            LZM_StartNewTask(TaskSystemDeepBaseCalib);//
                            break;
                         case MENU_DEBUG_TURNSPEEP:
                            LZM_StartNewTask(TaskSystemTurnSpeedPulseNum);//
                            break;
                        default:
                            break;                  
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSystemDebug,TaskPara);
                    break;
            }
            return 1;
       case SHOW_TIMER:
            s_stTaskAttrib.ControlId=0;
            MenuShow(&s_sutMenuSystemDebug);
            break;
        default:
            return LZM_DefTaskProc(Msg);
    } 
    return 1;
}
/*************************************************************
** ��������: TaskMenuSystemParameter
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystemParameter(LZM_MESSAGE *Msg)
{
    //unsigned char val;
    //unsigned char buf[13];
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemParameter,s_apcMenuSystemParameter);
            MenuShow(&s_sutMenuSystemParameter);   
            s_stTaskAttrib.ControlId =0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSystemParameter))
                    {
                        
                        case MENU_PARAM_PHONE_NUMBER:
                            //val=ReadAndCheckPhoneNumber(buf);
                            //if(val==1)
                            //{
                                LZM_StartNewTask(TaskSystemParamSetPhoneNumber);
                            //}
                            //else
                            //{
                            //    LZM_StartNewTask(TaskSystemParamSetDeviceID);
                            //}
                            break;                        
                        case MENU_PARAM_SPEED_PARAM:
                            LZM_StartNewTask(TaskSystemParamSetSpeedParam);  //�����ٶȲ���
                            break;
                        case MENU_PARAM_DOOR_LEVEL_SEL:
                            LZM_StartNewTask(TaskSystemParamDoorLevelSelect);
                            break;
                        case MENU_PARAM_HORN_LEVEL_SEL:
                            LZM_StartNewTask(TaskSystemParamHornLevelSelect);
                            break;
                        case MENU_PARAM_CDMA_SLEEP:
                            LZM_StartNewTask(TaskSystemParamSetCDMASleep);
                            break;
                        case MENU_PARAM_TERMINAl_ID:
                            LZM_StartNewTask(TaskSystemShowTerminalID);
                            break;
                        case MENU_PARAM_DEFAULT_PARAM:
                            PublicConfirm("�ָ���������?",LoadDefaultParamter);
                            break;
                        case MENU_PARAM_GNSS_WORD_MODE:
                            LZM_StartNewTask(TaskSystemParamSetGNSSWordMode);
                            break;
                        default:
                            break;                  
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSystemParameter,TaskPara);
                    break;
            }
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    } 
    return 1;
}
/*************************************************************
** ��������: TaskMenuBeforeLockTwo
** ��������: "ʹ��ǰ����һ"
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuBeforeLockOne(LZM_MESSAGE *Msg)
{
  /*ʹ�������º��������˸ú���
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            LcdShowCaption("ʹ��ǰ����һ",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowCaption("�Ƿ�ȷ������?",24);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            s_stTaskAttrib.step = 0;
            s_stTaskAttrib.select = 0;
            s_stTaskAttrib.ControlId = 0;
            s_stTaskAttrib.ControlMaxId =0;
            return 1;
        case KEY_PRESS:            
            switch(TaskPara)
            {
                case KEY_BACK://����                    
                    s_stTaskAttrib.step =0;
                    s_stTaskAttrib.answerNum = 0;
                    s_stTaskAttrib.ControlMaxId =0;
                    if(s_stTaskAttrib.ControlId==0xff)break;
                    LZM_StartNewTask(TaskMenuSystemAdvanced);//
                    break;
                case KEY_ENTER://ȷ��
                    s_stTaskAttrib.step =0;
                    s_stTaskAttrib.answerNum = 0;
                    s_stTaskAttrib.ControlMaxId =0;
                    if(s_stTaskAttrib.ControlId==0xff)break;
                    s_stTaskAttrib.ControlId=0xff;
                    SysAutoRunCountReset();
                    LcdClearScreen();
                    LcdShowCaption("�ն������ɹ�",24);
                   // SetTimerTask(TIME_CLOSE_GPRS,SECOND);                    
                    Communication_Close();
                    LZM_SetAlarm(TEST_TIMER, PUBLICSECS(10));
                    s_stTaskAttrib.select = 0;
                    break;
                case KEY_UP: 
                    if(s_stTaskAttrib.ControlId!=0xff)break;
                    if(s_stTaskAttrib.step<4)
                    {
                        s_stTaskAttrib.step++;                        
                        s_stTaskAttrib.answerNum = 0;
                        s_stTaskAttrib.ControlMaxId = 0;
                    }
                    else                    
                    {
                        if(4==s_stTaskAttrib.ControlMaxId)
                        {
                            if(s_stTaskAttrib.answerNum <4)
                            {
                                s_stTaskAttrib.answerNum++;
                            }
                            else
                            {
                                s_stTaskAttrib.step = 0;
                                s_stTaskAttrib.select = 0;                                
                                s_stTaskAttrib.ControlId = 0;
                                s_stTaskAttrib.ControlMaxId =0;
                                Communication_Open();
                                LZM_StartNewTask(TaskMenuSystemAdvanced);//
                            }
                        }
                        else
                        if(s_stTaskAttrib.ControlMaxId)
                        {
                            s_stTaskAttrib.step = 0;
                            s_stTaskAttrib.ControlMaxId = 0;
                        }
                    }                    
                    break;
                case KEY_DOWN:
                    if(s_stTaskAttrib.ControlId!=0xff)break;
                    if(4==s_stTaskAttrib.step)
                    {
                        s_stTaskAttrib.answerNum = 0;
                        if(s_stTaskAttrib.ControlMaxId<4)
                        {
                            s_stTaskAttrib.ControlMaxId++;
                        }
                    }
                    else
                    {
                        s_stTaskAttrib.step =0;
                        s_stTaskAttrib.answerNum = 0;
                        s_stTaskAttrib.ControlMaxId =0;
                    }
                    break;
                default: 
                    break;
            }
            return 1;
        case TEST_TIMER:
            SysAutoRunCountReset();
            if(s_stTaskAttrib.select<30)//�ָ������Ĵ���300s=30*10s;
            {
                s_stTaskAttrib.select++;
                LZM_SetAlarm(TEST_TIMER, PUBLICSECS(10));
            }
            else
            {
                s_stTaskAttrib.select = 0;
                Communication_Open();
                LZM_StartNewTask(TaskMenuSystemAdvanced);//
            }
            break;
        default:
            if(s_stTaskAttrib.ControlId==0xff)break;
            return LZM_DefTaskProc(Msg);
    }
  */
    return 0;
}
LZM_RET TaskMenuLock1(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("ʹ��ǰ����һ",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],30,24,(char **)c_apcLock1Items,"");
            EepromPram_ReadPram(E2_LOCK1_ENABLE_FLAG_ID, &handle);            
            s_stTaskAttrib.select=handle;
            if(handle!=1)handle=0;
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemAdvanced);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select = handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_LOCK1_ENABLE_FLAG_ID, &handle, 1);
                        EepromPram_ReadPram(E2_LOCK1_ENABLE_FLAG_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("���ĳɹ�", 23);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("����ʧ��", 23);  
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
			EepromPram_UpdateVariable(E2_LOCK1_ENABLE_FLAG_ID); 
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemAdvanced);
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskMenuBeforeLockTwo
** ��������: "ʹ��ǰ������"
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuBeforeLockTwo(LZM_MESSAGE *Msg)
{
  /*ʹ�����º�������ú���
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            LcdShowCaption("ʹ��ǰ������",0);
            s_stTaskAttrib.step = 0;
            s_stTaskAttrib.select = 0;
            s_stTaskAttrib.ControlId = 0;
            s_stTaskAttrib.ControlMaxId =0;
            if(OpenAccount_GetCurLockStatus())
            {
                SysAutoRunCountReset(); 
                s_stTaskAttrib.ControlId = 0xff; 
                LcdShowCaption("�ն�������",26); 
                SetTimerTask(TIME_CLOSE_GPRS,SECOND);                 
                LZM_SetAlarm(TEST_TIMER, PUBLICSECS(10));
            }
            else
            {            
                LcdShowHorLine(0, 131, 18, 1);
                LcdShowCaption("�Ƿ�ȷ������?",24);
                LcdShowHorLine(0, 131, 45, 1);
                LcdShowCaption("��<ȷ��>����",47);
            }
            return 1;
        case KEY_PRESS:            
            switch(TaskPara)
            {
                case KEY_BACK://����                    
                    s_stTaskAttrib.step =0;
                    s_stTaskAttrib.answerNum = 0;
                    s_stTaskAttrib.ControlMaxId =0;
                    if(s_stTaskAttrib.ControlId==0xff)break;
                    LZM_StartNewTask(TaskMenuSystemAdvanced);//
                    break;
                case KEY_ENTER://ȷ��
                    s_stTaskAttrib.step =0;
                    s_stTaskAttrib.answerNum = 0;
                    s_stTaskAttrib.ControlMaxId =0;
                    if(s_stTaskAttrib.ControlId==0xff)break;
                    s_stTaskAttrib.ControlId=0xff;
                    SysAutoRunCountReset();
                    LcdClearScreen();
                    LcdShowCaption("�ն������ɹ�",24);
                    SetTimerTask(TIME_CLOSE_GPRS,SECOND);
                    /////////////////////
                    #ifdef USE_QUICK_OPEN_ACCOUNT
                    OpenAccount_SetLockStatus();
                    #endif
                    s_stTaskAttrib.select = 0;
                    /////////////////////                    
                    LZM_SetAlarm(TEST_TIMER, PUBLICSECS(10));
                    break;
                case KEY_UP: 
                    if(s_stTaskAttrib.ControlId!=0xff)break;
                    if(s_stTaskAttrib.step<4)
                    {
                        s_stTaskAttrib.step++;                        
                        s_stTaskAttrib.answerNum = 0;
                        s_stTaskAttrib.ControlMaxId = 0;
                    }
                    else                    
                    {
                        if(4==s_stTaskAttrib.ControlMaxId)
                        {
                            if(s_stTaskAttrib.answerNum <4)
                            {
                                s_stTaskAttrib.answerNum++;
                            }
                            else
                            {
                                s_stTaskAttrib.step = 0;
                                s_stTaskAttrib.select = 0;
                                s_stTaskAttrib.ControlId = 0;
                                s_stTaskAttrib.ControlMaxId =0;
                                OpenAccount_ClearLockStatus();
                                Communication_Open();
                                LZM_StartNewTask(TaskMenuSystemAdvanced);//
                            }
                        }
                        else
                        if(s_stTaskAttrib.ControlMaxId)
                        {
                            s_stTaskAttrib.step = 0;
                            s_stTaskAttrib.ControlMaxId = 0;
                        }
                    }                    
                    break;
                case KEY_DOWN:
                    if(s_stTaskAttrib.ControlId!=0xff)break;
                    if(4==s_stTaskAttrib.step)
                    {
                        s_stTaskAttrib.answerNum = 0;
                        if(s_stTaskAttrib.ControlMaxId<4)
                        {
                            s_stTaskAttrib.ControlMaxId++;
                        }
                    }
                    else
                    {
                        s_stTaskAttrib.step =0;
                        s_stTaskAttrib.answerNum = 0;
                        s_stTaskAttrib.ControlMaxId =0;
                    }
                    break;
                default: 
                    break;
            }
            return 1;
        case TEST_TIMER:
            SysAutoRunCountReset();
            if(s_stTaskAttrib.select==2)
            {
                #ifdef USE_NAVILCD
                if(!NaviLcd_GetCurOnlineStatus())
                #endif
                OpenAccount_CheckAndDisposeLockStatus();
            }
            //////////////////////////
            if(s_stTaskAttrib.select<60)//�ָ������Ĵ���600s=60*10s;
            {
                s_stTaskAttrib.select++;
                LZM_SetAlarm(TEST_TIMER, PUBLICSECS(10));
            }
            else
            {
                s_stTaskAttrib.select = 0;
                Communication_Open();
                OpenAccount_ClearLockStatus();
                LZM_StartNewTask(TaskMenuSystemAdvanced);//
            }
            break;
        default:
            if(s_stTaskAttrib.ControlId==0xff)break;
            return LZM_DefTaskProc(Msg);
    }
  */
    return 0;
}
LZM_RET TaskMenuLock2(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("ʹ��ǰ������",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],30,24,(char **)c_apcLock1Items,"");
            EepromPram_ReadPram(E2_LOCK2_ENABLE_FLAG_ID, &handle);            
            s_stTaskAttrib.select=handle;
            if(handle!=1)handle=0;
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemAdvanced);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select = handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_LOCK2_ENABLE_FLAG_ID, &handle, 1);
                        EepromPram_ReadPram(E2_LOCK2_ENABLE_FLAG_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("���ĳɹ�", 23);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("����ʧ��", 23);      
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
			EepromPram_UpdateVariable(E2_LOCK2_ENABLE_FLAG_ID); 
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemAdvanced);
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskSystemOpenAccountEncrypt
** ��������: ����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemOpenAccountEncrypt(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("�����Ƿ����",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            SpinInit(&s_stSpin[0],30,24,(char **)c_apcEncryptItems,"");
            EepromPram_ReadPram(E2_TOPLIGHT_CONTROL_ID, &handle);            
            s_stTaskAttrib.select=handle;
            if(handle!=1)handle=0;
            s_stSpin[0].handle=handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuSystemAdvanced);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuSystemAdvanced);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select = handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        EepromPram_WritePram(E2_TOPLIGHT_CONTROL_ID, &handle, 1);
                        EepromPram_ReadPram(E2_TOPLIGHT_CONTROL_ID, &handle);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23); 
                            EepromPram_UpdateVariable(E2_CDMA_SLEEP_ID);                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);  
                            
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuSystemAdvanced);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskMenuSystemAdvanced
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**�޸���      : fanqinghai
**�޸����� : ���������Ϣ��ѯ�˵�
**�������:  2015.05.18

*************************************************************/  
LZM_RET TaskMenuSystemAdvanced(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemAdvanced,s_apcMenuSystemAdvanced);
            MenuShow(&s_sutMenuSystemAdvanced);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystem);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSystemAdvanced))
                    {
                        case MENU_SYS_CAR_INFO:
                            LZM_StartNewTask(TaskMenuSystemCarInfo);
                            break;
                        case MENU_SYS_SERVER_INFO:
                            LZM_StartNewTask(TaskMenuSystemManageServer);  //����������
                            break;
                        case MENU_SYS_DEBUG:
                            LZM_StartNewTask(TaskMenuSystemDebug);      //��װ����
                            break;
                        case MENU_SYS_TERMINAL_PARAM:
                            LZM_StartNewTask(TaskMenuSystemParameter);      //�ն˲�������
                            break;
                        case MENU_SYS_ENCRYPT:
                            LZM_StartNewTask(TaskSystemOpenAccountEncrypt);
                            break;
                        case MENU_SYS_LOCK_ONE:
                            LZM_StartNewTask(TaskMenuLock1);
                            break;
                        case MENU_SYS_LOCK_TWO:
                            LZM_StartNewTask(TaskMenuLock2);
                            break;
                        #ifdef CALIB_OIL_WEAR
                        case MENU_SYS_OIL_DEMARCATE:
                           // LZM_StartNewTask(TaskCalibOilPointNumber);  //�����궨
                            break;
                        #endif
                        case MENU_SYS_PERP_INFORM_QUERY:
                            LZM_StartNewTask(TaskQueryPeripheralInform);     //������Ϣ��ѯ  
                            break;
                        case MENU_SYS_GYRO:
                           // LZM_StartNewTask(TaskMenuGyro);//dxl,2015.6.27,���������ǲ˵�
                            break;
            default:
                            break;                  
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSystemAdvanced,TaskPara);
                    break;
            }
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    } 
}


typedef struct 
{
    u8  Camera_On;
    u8 Weight_Collect_On;
    u8 Temperature_On;
    u8 Oil_Collect_On;
    u8 Can_collectt_On;
    u8 Navation_Scr_On;
    u8 No_Ph;
    u8 which[7];
}Periphera;

static Periphera ph;


/*************************************************************
** ��������: TaskSystemCameraInfo
** ��������: ����ͷ��ʾ����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����     : fanqinghai
**������� :  2015.05.18
*************************************************************/  

LZM_RET TaskSystemCameraInfo(LZM_MESSAGE *Msg)  
{ 
    u8 tmp_rslt=0,ID=0,i=0;
    char DisBuffer[40],y_Location=0;
    switch(Msg->message)
    {
        case TASK_START:
           LcdClearScreen();
           tmp_rslt=Camera_GetOnlineFlag()&0x0f;
           for(i=0;i<4;i++)
           {
               if(tmp_rslt&0x01)
               {
                   sprintf(DisBuffer,"%d·   ���� ",ID+1);  
                   LcdShowStr(0, y_Location*16+1,  DisBuffer, 0);
                   y_Location++;
               }
               ID++;
               tmp_rslt>>=1;
                             
           }
           LcdShowRect(0,y_Location*16+1,128,y_Location*16+1,0);   //����
           break;
        case KEY_PRESS:
            switch(TaskPara)
                {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskQueryPeripheralInform);
                    break;
                default:
                    break;

                }
        return 1;
        default:
            return LZM_DefTaskProc(Msg);

    } 
  return 1;
}
/*************************************************************
** ��������: TaskSystemWeightCollector
** ��������: ���زɼ��������ʾ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����           : fanqinghai
**�������:  2015.05.18
*************************************************************/  

LZM_RET TaskSystemWeightCollector(LZM_MESSAGE *Msg)  
{ 
    char DisBuffer[40];
    float tmpVal=0;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            if(!Io_ReadSelfDefine2Bit(DEFINE_BIT_26))   //0����1�쳣
            LcdShowStr(0, 0,  "���زɼ���  ����", 0);
            else
            {
            LcdShowStr(0, 0,  "���زɼ���  �쳣", 0);
            return 0;
            }
            if(!Io_ReadSelfDefine2Bit(DEFINE_BIT_25))
                LcdShowStr(0, 16, "���ش�����  ����", 0);
            else 
            {         
                LcdShowStr(0, 16, "���ش�����  �쳣", 0);
                return 0;
            }
            if(!Io_ReadSelfDefine2Bit(DEFINE_BIT_19))       //0�У�1û�б���    
                LcdShowStr(0, 32, "���ر���     ��  ", 0);
            else 
                LcdShowStr(0, 32, "���ر���     ��  ", 0);
                
            //tmpVal=Carload_GetCurTotalWeight();��֧�ֳ��زɼ���
            tmpVal = 0;
            sprintf(DisBuffer,"��ǰ���� %5.3f�� ",tmpVal/1000);
                LcdShowStr(0, 48,DisBuffer, 0);
            break;
        case KEY_PRESS:
            switch(TaskPara)
                {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskQueryPeripheralInform);
                    break;
                default:
                    break;
                }
        return 1;
        default:
            return LZM_DefTaskProc(Msg);

    } 
    
        return 0;

}

/*************************************************************
** ��������: TaskSystemWeightCollector
** ��������: �¶Ȳɼ��������ʾ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����           : fanqinghai
**�������:  2015.05.18
*************************************************************/  
#define DATA_SIZE        5

LZM_RET TaskSystemTemperatureCollector(LZM_MESSAGE *Msg)  
{ 
    char DisBuffer[40],y_Location=0;
    u16 Temp_len;
    u8 Temp_Buff[30],Temp_Sensor_nmb,ID=0,i=0;
    float tempVal=0;

    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            Temp_len=Temperature_PosEx(Temp_Buff);
            if(Temp_len<23&&Temp_len>0)
            {
                 Temp_Sensor_nmb=(Temp_len-2)/DATA_SIZE;
                for(i=0;i<Temp_Sensor_nmb;i++)
                {
                    ID=Temp_Buff[2+i*DATA_SIZE+0];
                    tempVal=(u16)(Temp_Buff[2+i*DATA_SIZE+3])<<8;
                    tempVal=(tempVal+Temp_Buff[2+i*DATA_SIZE+4]);
                    tempVal=tempVal/10-273;
                    if(PUBLIC_CHECKBIT_EX(Temp_Buff[2+i*DATA_SIZE+2],0))
                    {
        
                        sprintf(DisBuffer,"ID%d���� ",ID);  
                        LcdShowStr(0, y_Location*16+1,  DisBuffer, 0);
                        
                    }
                    else  if(PUBLIC_CHECKBIT_EX(Temp_Buff[2+i*DATA_SIZE+2],1))
                    {
                        sprintf(DisBuffer,"ID%d���� %5.1f��",ID,tempVal);
                        LcdShowStr(0, y_Location*16+1,  DisBuffer, 0);
                        
                    }
                    else  if(PUBLIC_CHECKBIT_EX(Temp_Buff[2+i*DATA_SIZE+2],2))
                    {
                        sprintf(DisBuffer,"ID%d���� %5.1f��",ID,tempVal);
                        LcdShowStr(0, y_Location*16+1,  DisBuffer, 0);
                        
                    }
                    else 
                    {
                        sprintf(DisBuffer,"ID%d���� %5.1f��",ID,tempVal);
                        LcdShowStr(0, y_Location*16+1,  DisBuffer, 0);
                    }
                    y_Location++;
                }
                LcdShowRect(0,y_Location*16+1,128,y_Location*16+1,0);   //����
            }
            else 
            {
                LcdShowStr(10, 17,  "�¶Ȳɼ�������", 0);
            }

            break;
        case KEY_PRESS:
            switch(TaskPara)
                {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskQueryPeripheralInform);
                    break;
                default:
                    break;

                }
        return 1;
        default:
            return LZM_DefTaskProc(Msg);

    } 
    
    
    return 0;
}

/*************************************************************
** ��������: TaskSystemOilCollector
** ��������: �����ɼ��������ʾ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����     : fanqinghai
**�������:  2015.05.18
*************************************************************/  
//extern unsigned short OilWear_GetCurOilVal(void);

LZM_RET TaskSystemOilCollector(LZM_MESSAGE *Msg)  
{ 

   
   char DisBuffer[40];
   u8 y_Location=0;
   float tmpVal;

    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            if(1==ReadPeripheral2TypeBit(2))  //1���룬0δ����
            {
                 LcdShowStr(0, y_Location+1, "��ͨ     ", 0);
        
                 tmpVal=JointechOiLCost_OilVolumeCur_Get();       //��ͨ����������,0.01L/��λ         
                 sprintf(DisBuffer,"��ǰ���� %5.1fL",tmpVal/100);
                 LcdShowStr(0, y_Location+17,DisBuffer, 0);
            }
            break;
        case KEY_PRESS:
            switch(TaskPara)
                {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskQueryPeripheralInform);
                    break;
                default:
                    break;
                }
        return 1;
        default:
            return LZM_DefTaskProc(Msg);

    } 
    return 0;
}
/*************************************************************
** ��������: TaskSystemCANInformCollector
** ��������:CAN�����ʾ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ����    : fanqinghai
** �������:  2015.05.18
*************************************************************/  

LZM_RET TaskSystemCANInformCollector(LZM_MESSAGE *Msg)  
{ 

    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
             /*
            if(!0)       //0����1�쳣
                {
                LcdShowStr(0, 0,  "  ����", 0);
                }
            else
                {
                LcdShowStr(0, 0,  "  ����", 0);
                return 0;
                }*/
            break;
        case KEY_PRESS:
            switch(TaskPara)
                {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskQueryPeripheralInform);
                    break;
                default:
                    break;

                }
        return 1;
        default:
            return LZM_DefTaskProc(Msg);

    } 
    return 0;
}
/*************************************************************
** ��������: TaskSystemNavationScr
** ��������: �����ɼ��������ʾ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����           : fanqinghai
**�������:  2015.05.18
*************************************************************/  

LZM_RET TaskSystemNavationScr(LZM_MESSAGE *Msg)  
{ 


    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
           if(!Io_ReadAlarmBit(ALARM_BIT_LCD_FAULT))       //0����1�쳣
                LcdShowStr(0, 0,  "������  ����",0);
            else
                {LcdShowStr(0, 0, "������  ����",0);
                return 0;
                }
            break;
        case KEY_PRESS:
            switch(TaskPara)
                {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskQueryPeripheralInform);
                    break;
                default:
                    break;

                }
        return 1;
        default:
            return LZM_DefTaskProc(Msg);

    } 
    return 0;
}



/*************************************************************
** ��������: Adjust_Array_Size
** ��������: �ַ����鶯̬����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����           : fanqinghai
**�������:  2015.05.18
*************************************************************/  
void Adjust_Array_Size(const char **dec,const char **src)
{
      u8 i=0;
      UpdatePeripheralType();
             if(Camera_GetOnlineFlag()&0x0f)    //"����ͷ",
             {
                dec[i++]=src[0];ph.Camera_On=1;ph.which[i-1]=0;
                     }
             if(ReadPeripheral1TypeBit(3))   //"���زɼ���",
             {
               dec[i++]=src[1];ph.Weight_Collect_On=1;ph.which[i-1]=1;
             }
             if(ReadPeripheral2TypeBit(5))   //"�¶Ȳɼ���",
             {
                dec[i++]=src[2];ph.Temperature_On=1;ph.which[i-1]=2;
             }

             if(ReadPeripheral2TypeBit(2)||ReadPeripheral2TypeBit(3))   //"�����ɼ���,
             {
                dec[i++]=src[3];ph.Oil_Collect_On=1;ph.which[i-1]=3;
             }
             if(ReadPeripheral2TypeBit(1))  //"CAN��Ϣ�ɼ�ģ��",
             {
                dec[i++]=src[4];ph.Can_collectt_On=1;ph.which[i-1]=4;
             }
             if(ReadPeripheral2TypeBit(15))  // "������", 
             {
                dec[i++]=src[5];ph.Navation_Scr_On=1;ph.which[i-1]=5;
             }
             ph.No_Ph=0;
             if(!i)
             {
                 ph.No_Ph=1;
             }
             else dec[i++]="";
                
}

/*************************************************************
** ��������: TaskQueryPeripheralInfom
** ��������: ������Ϣ��ѯ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
**����           : fanqinghai
**�������:  2015.05.18
*************************************************************/  

LZM_RET TaskQueryPeripheralInform(LZM_MESSAGE *Msg)
{
    const char *Dynamic_pointer[6]={0};
    Adjust_Array_Size(Dynamic_pointer,s_apcMenuSystemPeripheral);
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemPeripheral,Dynamic_pointer);
            if(!ph.No_Ph)
            {
                MenuShow(&s_sutMenuSystemPeripheral);
              //  LZM_KillTimer(TEST_TIMER);
            }
            else 
            {
                LcdClearScreen();
                LcdShowStr(35, 17, "û������",0);
                LZM_SetAlarm(TEST_TIMER,SECS(1));
            }
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER:
                  { 
                    switch(ph.which[MenuGetHandle(&s_sutMenuSystemPeripheral)])
                        {
                        case MENU_EQM_CAMERA:
                            LZM_StartNewTask(TaskSystemCameraInfo);break;
                        case MENU_EQM_WEIGHT_DEV:
                            LZM_StartNewTask(TaskSystemWeightCollector);break;
                        case MENU_EQM_TMP_COLLECT:
                            LZM_StartNewTask(TaskSystemTemperatureCollector);break;
                        case MENU_EQM_OIL_DETECT:
                            LZM_StartNewTask(TaskSystemOilCollector);break;
                        case MENU_EQM_CANINFORM_COLLECT:
                            LZM_StartNewTask(TaskSystemCANInformCollector);break;
                        case MENU_EQM_NAVIGATION_SCR:
                            LZM_StartNewTask(TaskSystemNavationScr);break;
                        default:break;
                    }
                }
                    break;
                default:
                    if(!ph.No_Ph)
                        MenuResponse(&s_sutMenuSystemPeripheral,TaskPara);
                    break;
            }
            return 1;
       case TEST_TIMER: 
               LZM_StartNewTask(TaskMenuSystemAdvanced);return 1;
       default:
            return LZM_DefTaskProc(Msg);
    } 
}

/*************************************************************
** ��������: OpenFeatureCoefFlag
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void OpenFeatureCoefFlag(void)
{
    //setAdjustFunction(ENABLE); dxl,2015.9.22
    VDRCoefficient_EnableCalibration();
    s_stTaskAttrib.ControlId=0xff;
    LcdClearScreen();
    LcdShowCaption("����ϵͳ�ѿ���", 23);
    //TestBeep(1);
    LZM_SetAlarm(TEST_TIMER, SECS(1.5));
}
/*************************************************************
** ��������: OpenFeatureCoefFlag
** ��������: 1����У׼��
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char GetFeatureCoefFlag(void)
{
    ///return ADJ_getAdjFlg();
    return VDRCoefficient_GetFlag();
}
/*************************************************************
** ��������: CalibrationFeatureCoefInit
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void CalibrationFeatureCoef(void)
{
    if(GetFeatureCoefFlag())
    {
        s_stTaskAttrib.ControlId=0xff;
        LcdShowMsgEx("У׼�ѿ���",SECS(2));
    }
    else
        PublicConfirm("����У׼����ϵ��",OpenFeatureCoefFlag);
}

/*************************************************************
** ��������: TaskMenuSystemAdvanced
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystem(LZM_MESSAGE *Msg)
{
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystem,s_apcMenuSystem);
            MenuShow(&s_sutMenuSystem);    
            s_stTaskAttrib.ControlId = 0;
            s_stServerParam.brushcardFlag = 0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {                
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuMain);
                    break;
                case KEY_ENTER:
                    switch(MenuGetHandle(&s_sutMenuSystem))
                    {
                        case MENU_SYSTEM_REGISTER:
                        case MENU_SYSTEM_QUALIFY:
                            handle=MenuGetHandle(&s_sutMenuSystem);
                            LcdClearScreen();
                            Lcd_DisplaySystemTest(handle);
                            LZM_SetAlarm(TEST_TIMER, SECS(0.3));
                            s_stTaskAttrib.ControlId = 0xff;
                            break;
                        case MENU_SYSTEM_BACKLIGHT:
                            LZM_StartNewTask(TaskLcdBacklight);
                            break;
                        case MENU_SYSTEM_ADVANCED:
                            PublicGetPasswordTo(_TASK(TaskMenuSystemAdvanced),s_stTaskAttrib.SysPassword); 
                            break;
                        case MENU_SYSTEM_MAIN_SELECT:
                            LZM_StartNewTask(TaskMainInterfaceSelection); //�����:fanqinghai ���ʱ��:2015.07.06
                            break;
                        case MENU_SYSTEM_BASE_SET:
                            LZM_StartNewTask(TaskSystemDeepBaseSet); //�����:fanqinghai ���ʱ��:2015.07.06
                            break;
                        default:
                            break;                  
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuSystem,TaskPara);
                    break;
            }
            return 1;
        case TEST_TIMER:
            s_stTaskAttrib.ControlId=0;
            MenuShow(&s_sutMenuSystem);
            return 1;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 1;
}
/*************************************************************
** ��������: TaskMainInterfaceSelection
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ����    :fanqinghai
*************************************************************/  
LZM_RET TaskMainInterfaceSelection(LZM_MESSAGE *Msg)
{
    
    switch(Msg->message)
    {
        case TASK_START:
           // LZM_KillTimer(TEST_TIMER);
            MenuInit(&s_sutMenuMainInterfaceSet,s_apcMenuMainInterfaceSet);
            MenuShow(&s_sutMenuMainInterfaceSet);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuMain);
                    break;
                case KEY_ENTER: 
                    MenuSwitchFlg = MenuGetHandle(&s_sutMenuMainInterfaceSet);
                    FRAM_BufferWrite(FRAM_INTERFACE_SWOTCH_ADDR,&MenuSwitchFlg,FRAM_INTERFACE_SWOTCH_LEN-1);
                    LZM_StartNewTask(TaskShowMainInterface);
                    break;
                default:
                    MenuResponse(&s_sutMenuMainInterfaceSet,TaskPara);
                break;
            }
            return 1;
        case INT_RTC:            
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;

}

/*************************************************************
** ��������: TaskMenuSystemDebugTest
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuSystemDebugTest(LZM_MESSAGE *Msg)
{
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuSystemTest,s_apcMenuSystemTest);
            MenuShow(&s_sutMenuSystemTest);
            s_stTaskAttrib.ControlId =0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId)
            {
                LZM_SetAlarm(TEST_TIMER,SECS(0.1));
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemDebug);//ϵͳ����
                    break;
                case KEY_ENTER:
                    handle=MenuGetHandle(&s_sutMenuSystemTest);
                    s_stTaskAttrib.ControlId = 1;
                    LcdClearScreen();
                    Lcd_DisplaySystemTest(handle);
                    LZM_SetAlarm(TEST_TIMER, SECS(0.3));
                    break;
                default:
                    MenuResponse(&s_sutMenuSystemTest,TaskPara);
                    break;
            }
            return 1;        
        case TEST_TIMER:
            s_stTaskAttrib.ControlId = 0;
            MenuShow(&s_sutMenuSystemTest);
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskMenuDriveRecord
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuDriveRecord(LZM_MESSAGE *Msg)
{ 
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(TEST_TIMER);
            MenuInit(&s_sutMenuDriveRecord,s_apcMenuDriveRecord);
            MenuShow(&s_sutMenuDriveRecord);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuMain);
                    break;
                case KEY_ENTER: 
                    switch(MenuGetHandle(&s_sutMenuDriveRecord))
                    { 
                        case MENUDRIVERECORD_CAR_INFO:
                            LZM_StartNewTask(TaskShowCarInfo);
                            break;
                        case MENUDRIVERECORD_DRIVER_INFO:
                            LZM_StartNewTask(TaskShowDriverInfo);
                            break;
                        case MENUDRIVERECORD_TIREDDRIVE_RECORD://��ʱ��ʻ��¼
                            LZM_StartNewTask(TaskShowTiredDriveRecord);
                            break;
                        case MENUDRIVERECORD_15MIN_SPEED:
                            LZM_StartNewTask(TaskShow15MinSpeed);
                            break;
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuDriveRecord,TaskPara);
                break;
            }
            return 1;
        case INT_RTC:            
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskInfoShowCarLoadStatus
** ��������: �����ػ�״̬
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskInfoShowCarLoadStatus(LZM_MESSAGE *Msg)
{    
    unsigned char handle;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();           
            LcdShowCaption("�����ػ�״̬",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            LcdShowStr(8,24,"״̬:",0);
            SpinInit(&s_stSpin[0],64,24,(char **)c_apcCarLoadStatusItems,"");
            Public_ReadDataFromFRAM(FRAM_CAR_CARRY_STATUS_ADDR, &handle, 1);          
            s_stTaskAttrib.select=handle;
            if(handle>2)handle=0;
            s_stSpin[0].handle = handle;
            SpinShow(&s_stSpin[0]);
            SpinSetFocus(&s_stSpin[0]);
            s_stTaskAttrib.ControlId=0;
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    handle=SpinGetHandle(&s_stSpin[0]);
                    Public_SetCarLoadStatus(handle);
                    if(handle!=s_stTaskAttrib.select)
                    {
                        s_stTaskAttrib.select = handle;
                        LcdClearArea(0,19,131,44);
                        LcdShowCaption("���ڱ���..", 23);                        
                        Public_WriteDataToFRAM(FRAM_CAR_CARRY_STATUS_ADDR, &handle, 1);
                        Public_ReadDataFromFRAM(FRAM_CAR_CARRY_STATUS_ADDR, &handle, 1);
                        LcdClearArea(0,19,131,44);
                        if(handle==s_stTaskAttrib.select)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23);
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                            //////////////////////////
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    {
                        LZM_SetAlarm(TEST_TIMER, SECS(0.5));
                    }
                    s_stTaskAttrib.ControlId=0xff; 
                    break;                                
                default:                    
                    SpinResponse(&s_stSpin[0],TaskPara);  
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuInfoServer);//ϵͳ����
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskMenuInfoServer
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuInfoServer(LZM_MESSAGE *Msg)
{ 
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(TEST_TIMER);
            MenuInit(&s_sutMenuInfoServer,s_apcMenuInfoServer);
            MenuShow(&s_sutMenuInfoServer);
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuMain);
                    break;
                case KEY_ENTER: 
                    switch(MenuGetHandle(&s_sutMenuInfoServer))
                    {
                        case MENU_INFO_TEXT_INFO://�ı���Ϣ  
                            LZM_StartNewTask(TaskShowTextInfo);
                            break;
                        case MENU_INFO_INFO_SERVICE://"�㲥��Ϣ",
                            LZM_StartNewTask(TaskShowInfoService);
                            break;
                        case MENU_INFO_QUESTION_RESPONSE://"����Ӧ��",
                            LZM_StartNewTask(TaskShowQuestionList);
                            break;
                        case MENU_INFO_EVENT_REPORT: //"�¼�����", 
                            LZM_StartNewTask(TaskShowEventReport);//
                            break;
                        case MENU_INFO_RESERVE_SERVICE://"��Ϣ�㲥",    
                            LZM_StartNewTask(TaskShowInformationDemand);
                            break;
                        case MENU_INFO_CAR_LOAD_STATUS:
                            LZM_StartNewTask(TaskInfoShowCarLoadStatus);
                            break;
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuInfoServer,TaskPara);
                break;
            }
            return 1;
        case INT_RTC:            
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskMenuMain
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuMain(LZM_MESSAGE *Msg)
{ 
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(TEST_TIMER);
            MenuInit(&s_sutMenuMain,s_apcMenuMain);
            MenuShow(&s_sutMenuMain);
            Lcd_SetDisplayCtrlFlag();
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskShowMainInterface);
                    break;
                case KEY_ENTER: 
                    switch(MenuGetHandle(&s_sutMenuMain))
                    {
                        case MENUMAIN_DRIVE_RECORD:
                            LZM_StartNewTask(TaskMenuDriveRecord);
                            break;
                        case MENUMAIN_INFO_SERVER://��Ϣ����
                            LZM_StartNewTask(TaskMenuInfoServer);
                            break;
                        #ifdef USE_PHONE_BOOK
                        case MENUMAIN_PHONE_SERVER:
                            LZM_StartNewTask(TaskMenuPhone);
                            break;
                        #endif
                        case MENUMAIN_DRIVE_MILE:
                            LZM_StartNewTask(TaskShowDriveMile);
                            break;
                        case MENUMAIN_PRODUCT_INFO:
                            LZM_StartNewTask(TaskShowProductInfo);//  ��Ʒ��Ϣ
                            break;
                        case MENUMAIN_SIM_NUM:                            
                            LZM_StartNewTask(TaskShowSIMCardNumber);//
                            break; 
                        case MENUMAIN_TTS:
                            LZM_StartNewTask(TaskShowAdjustTtsVolume);
                            break;
                        case MENUMAIN_SYSTEM:
                            LZM_StartNewTask(TaskMenuSystem);
                            break;                           
                    }
                    break;
                default:
                    MenuResponse(&s_sutMenuMain,TaskPara);
                break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskMenuPrintstart
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void TaskMenuPrintstart(void)
{
    LcdClearScreen();
    LcdShowCaption("��ӡ����",0);
    LcdShowHorLine(0, 131, 18, 1);
    LcdShowCaption("��̨���ڴ�ӡ",23);
    LcdShowCaption("���Ե�",46);
    print_init();
    ptr_pwr_ctrl(1);
    printer_task();
    s_stTaskAttrib.step = 0x55;
    s_stTaskAttrib.ControlId = 0xff;    
    LZM_SetAlarm(TEST_TIMER,SECS(8));    
    LZM_SetAlarm(REFRESH_TIMER,SECS(90));
}
/*************************************************************
** ��������: TaskMenuPrintstart
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void TaskMenuPrintend(void)
{
    print_init();
    ptr_pwr_ctrl(0);                
    Lcd_Initial();//���³�ʼ����ʾ������
}
/*************************************************************
** ��������: TaskMenuPrint
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskMenuPrint(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(TEST_TIMER);
            LZM_KillTimer(REFRESH_TIMER);
            s_stTaskAttrib.ControlId = 0; 
            if(s_stTaskAttrib.step != 0xaa)
            {
                s_stTaskAttrib.step = 0;
                LZM_SetAlarm(TEST_TIMER,LZM_AT_ONCE);
                return 1;
            }
            ///////////////////////
            s_stTaskAttrib.step = 0;
            /////////////////////
            LcdClearScreen();
            if(VDRDoubt_GetCarRunFlag()) 
            {
                LcdShowCaption("��ͣ��,�ٴ�ӡ!",23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else//�������Դ�ϵ�,�򲻴�ӡ            
            if(1 == Io_ReadAlarmBit(ALARM_BIT_POWER_SHUT))
            {
                LcdShowCaption("����ϵ粻�ܴ�ӡ",23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
            }
            else
            {
                LcdShowCaption("��ӡ����",0);
                LcdShowHorLine(0, 131, 18, 1);
                if(KEY_ENTER == KeyValue_Read())
                {
                    s_stTaskAttrib.ControlMaxId = 0;
                    s_stTaskAttrib.ControlId = 0x55;
                    LcdShowCaption("�ɿ��������ӡ",23);
                    LZM_SetAlarm(TEST_TIMER, SECS(0.1)); 
                    
                }
                else
                {
                    TaskMenuPrintstart();
                }
            }
            return 1;
        case KEY_PRESS:
            if(0xff==s_stTaskAttrib.ControlId)break;
            switch(TaskPara)
            {
                case KEY_ENTER://
                    break;
                default:
                    TaskMenuPrintend();
                    LZM_StartNewTask(TaskShowMainInterface);//������
                    break;
            }
            return 1;
        case TEST_TIMER:
            if(0xff==s_stTaskAttrib.ControlId)//��ӡʱ����Ҫ�ٴ���ʱ��Ҫ����ʾ�������룬���ÿ�ԭ��
            {
                if(is_print_finished())//�����
                {
                    s_stTaskAttrib.ControlId = 1;
                    LZM_SetAlarm(TEST_TIMER, SECS(5));                    
                }
                else
                {
                   LZM_SetAlarm(TEST_TIMER, SECS(3)); 
                }
            }
            else
            if(0x55==s_stTaskAttrib.ControlId)//��ӡʱ����Ҫ�ٴ���ʱ��Ҫ����ʾ�������룬���ÿ�ԭ��
            {
                if(KEY_ENTER == KeyValue_Read())
                {   
                    if(s_stTaskAttrib.ControlMaxId<60)
                    {
                        s_stTaskAttrib.ControlMaxId++;
                    }
                    else
                    {
                        s_stTaskAttrib.ControlId    = 0;
                        s_stTaskAttrib.ControlMaxId = 0;
                    }
                    LZM_SetAlarm(TEST_TIMER,SECS(0.1));
                }
                else
                {
                   TaskMenuPrintstart();
                }
            }
            else
            if(0x01==s_stTaskAttrib.ControlId)
            {
                s_stTaskAttrib.ControlId = 0;
                TaskMenuPrintend();
                LZM_SetAlarm(TEST_TIMER,SECS(0.1));
            }
            else
            {
                LZM_StartNewTask(TaskShowMainInterface);//������  
            }
            break;
        case REFRESH_TIMER:
            if(0x55 == s_stTaskAttrib.step)
            {
                s_stTaskAttrib.step = 0;
                TaskMenuPrintend();
                LZM_StartNewTask(TaskShowMainInterface);//������
            }
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#ifdef __LZMINPUTBOX_H
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskSystemTempTest(LZM_MESSAGE *Msg)
{
    unsigned char val;
    char buffer[20];
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            val=EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, GB2312Buffer);            
            GB2312Buffer[val]='\0';
            LcdShowStr(2,16,"��APN:",0);
            LcdShowStr(16,48,"����",0);
            LcdShowStr(80,48,"ȡ��",0);
            LZM_InputBoxInit(&stInputBox[0],50,16,18,(char *)GB2312Buffer,(char **)c_apcInputParam);
            stInputBox[0].loop = 0;
            LZM_InputBoxSetFocus(&stInputBox[0]);
            s_stTaskAttrib.ControlId = 0;
            return 1;
        case KEY_PRESS:
            if(0==s_stTaskAttrib.ControlId)
            {
                val=LZM_InputBoxResponse(&stInputBox[0],TaskPara);
                if(val==2||val==3)
                {
                    s_stTaskAttrib.ControlId = 1;
                    LcdShowStr(16,48,"����",1);
                }
                else
                if(val==1)
                {
                    s_stTaskAttrib.ControlId = 2;
                    LcdShowStr(80,48,"ȡ��",1);
                }
                else
                if(val)
                {
                    LZM_ReturnOldTaskEx();
                }
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://�˵�
                    LZM_ReturnOldTaskEx();
                    break;
                case KEY_UP://
                    if(1==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 0;
                        LcdShowStr(16,48,"����",0);
                        LZM_InputBoxSetFocusEx(&stInputBox[0]);
                    }
                    else
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 1;
                        LcdShowStr(16,48,"����",1);
                        LcdShowStr(80,48,"ȡ��",0);
                    }
                    break;
                case KEY_DOWN://
                    if(1==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 2;
                        LcdShowStr(16,48,"����",0);
                        LcdShowStr(80,48,"ȡ��",1);
                    }
                    else
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        s_stTaskAttrib.ControlId = 0;
                        LcdShowStr(80,48,"ȡ��",0);
                        LZM_InputBoxSetFocus(&stInputBox[0]);
                    }
                    break;
                case KEY_ENTER://ȷ��
                    if(1==s_stTaskAttrib.ControlId)
                    {
                        LcdClearScreen();
                        LcdShowCaption("���ڱ���...", 24);
                        val=LZM_InputBoxGetContent(&stInputBox[0],buffer);
                        EepromPram_WritePram(E2_MAIN_SERVER_APN_ID,(unsigned char *)buffer,val);
                        ////////////////////////////////////
                        val=EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID, (unsigned char *)&s_stTaskAttrib.ShowBuffer[0]);
                        LcdClearArea(0,19,131,44);
                        if(strncmp(s_stTaskAttrib.ShowBuffer[0],buffer,val)==0)
                        {
                            LcdShowCaption("�ĸ��ɹ�", 23);
                            EepromPram_UpdateVariable(E2_MAIN_SERVER_APN_ID);                           
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                        else
                        {
                            LcdShowCaption("�ĸ�ʧ��", 23);                             
                            LZM_SetAlarm(TEST_TIMER, SECS(1));
                        }
                    }
                    else
                    if(2==s_stTaskAttrib.ControlId)
                    {
                        LZM_ReturnOldTaskEx();
                    }
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_ReturnOldTaskEx();
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif

/*************************************************************
** ��������: ShowSelectMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowSelectMainInterface(unsigned char select)
{
    switch(select)
    {
        case MENU_INTERFACE_TEMPERATURE:
            LCD_TemperatureInterface();
            break;
        case MENU_INTERFACE_LOAD:
            LCD_LoadInterface();
            break;
        case MENU_INTERFACE_SPEED:
            LCD_SpeedInterface();
            break;
        case MENU_INTERFACE_DEEP:
            LCD_DeepInterface();
            break;
        case MENU_INTERFACE_TURNSPEED:
            LCD_TurnSpeepInterface();
            break;
        default: 
            Lcd_MainInterface();
            break; 
    }
}
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowMainInterface(LZM_MESSAGE *Msg)
{
    static unsigned long sulTimeVal = 0;
    static unsigned long sulReInitTime = 0;
    switch(Msg->message)
    {
        case TASK_START:
            ptr_pwr_ctrl(0);
            s_stTaskAttrib.step = 0;
            s_stTaskAttrib.ControlId = 0;
            LZM_KillTimer(TEST_TIMER);
            LZM_SetTimer(REFRESH_TIMER,SECS(1));
            LcdClearScreen();
            Lcd_ClearDisplayCtrlFlag();
            Lcd_SetMainRedrawFlag();
            ShowSelectMainInterface(MenuSwitchFlg);
            sulTimeVal = 0;
            if(0==sulReInitTime)sulReInitTime = RTC_GetCounter();
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://�˵�
                    LZM_KillTimer(REFRESH_TIMER);                    
                    LZM_StartNewTask(TaskMenuMain);//�˵�
                    break;                                
                case KEY_UP://
                    sulTimeVal = 0;
                    break;
                case KEY_DOWN://
                    sulTimeVal = 0;                 
                    break;
                case KEY_ENTER://ȷ��
                    LZM_SetAlarm(TEST_TIMER,SECS(0.2));
                    if(0==sulTimeVal)
                    {
                        s_stTaskAttrib.step = 1;
                        sulTimeVal =  Timer_Val()+SYSTICK_1SECOND*3;
                    }
                    else
                    if(Timer_Val()>=sulTimeVal)
                    {
                        LZM_KillTimer(TEST_TIMER);
                        LZM_KillTimer(REFRESH_TIMER);
                        if(1==s_stTaskAttrib.step)s_stTaskAttrib.step = 0xaa;
                        LZM_StartNewTask(TaskMenuPrint);//��ӡ
                    }
                    break;
                default:
                    sulTimeVal = 0;
                    break;
            }
            return 1;
        case REFRESH_TIMER:
            if(RTC_GetCounter()>sulReInitTime+REINIT_LCD_TIME)//20����
            {
                sulReInitTime = RTC_GetCounter();
                Lcd_SetMainRedrawFlag();
                Lcd_Initial();//���³�ʼ����ʾ������
            }
            ///////////////////
            if(!Lcd_GetDisplayCtrlFlag())//��ʾ��������,����ʾ������
            {
                ShowSelectMainInterface(MenuSwitchFlg);
            }
            else
            {
                Lcd_SetMainRedrawFlag();
            }
            break;
        case TEST_TIMER:
            sulTimeVal = 0;
            s_stTaskAttrib.step = 0;
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ShowStartLogo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void ShowStartLogo(void)
{
    char name[20];
    GetCurentProductModel(name);
    LcdClearScreen();    
    LcdShowCaption(name, 2);
    LcdShowCaption("������ʻ��¼��", 20);
    //LcdShowStr(9,40,ProductInfo[4],0);//��ʾ�汾��,dxl,ȥ�������汾��,��Ϊ�汾�ź����Ϊ����Ӧ,G��ΪV1.03��C��ΪV1.02
    ///////////////////////
    LZM_TaskFunction = TaskShowLogo;
}
/*************************************************************
** ��������: ShowStartLogo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void SaveCurSIMICcidNumberToE2Param(void)
{
    unsigned char data[20]={0}; 
    MOD_GetIccidBcdValue(data);
    EepromPram_WritePram(E2_SIM_ICCID_NUMBER_ID,data,E2_SIM_ICCID_NUMBER_ID_LEN);
}
/*************************************************************
** ��������: TaskShowMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowLogo(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(REFRESH_TIMER);
            s_stTaskAttrib.ControlId = 0;
            ShowStartLogo();
            LZM_SetAlarm(REFRESH_TIMER, SECS(0.1));
            SysAutoRunCountReset();
            LZM_SetOperateTimer(SaveCurSIMICcidNumberToE2Param,PUBLICSECS(40));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_ENTER://ȷ��
                    LZM_StartNewTask(TaskShowMainInterface);//������;
                    break;
                case KEY_BACK://����
                    LZM_StartNewTask(TaskShowMainInterface);//������;
                    break;                
                case KEY_UP://��¼
                    break;
                case KEY_DOWN://ע��
                    break;
            }
            return 1;
        case REFRESH_TIMER: 
            SysAutoRunCountReset();
            if(s_stTaskAttrib.ControlId < 3)
            {
                s_stTaskAttrib.ControlId++;
                LZM_SetAlarm(REFRESH_TIMER, SECS(0.1));
            }
            else
                LZM_StartNewTask(TaskShowMainInterface);//������;
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskShowTextInfo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowDisplayText(LZM_MESSAGE *Msg)
{
    static unsigned short susCurtStart = 0;
    static unsigned char LastCount = 0;
    static unsigned char flag = 0;
    switch(Msg->message)
    {
        case TASK_START:
            s_stTaskAttrib.ControlId=0;
            susCurtStart = 0;
            LastCount =0;
            flag=0;
            LZM_SetAlarm(TEST_TIMER, SECS(0.1));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    if(LZM_PreTaskFunction!=TaskShowDisplayText&&LZM_PreTaskFunction!=TaskShowQuestionResponse)
                    {
                        ShowTextForMsgBusyFlag(1);
                        LZM_ReturnOldTaskEx();//ϵͳ����
                    }
                    else
                    {
                        ShowTextForMsgBusyFlag(1);
                        ShowMainInterface();
                    }
                    break;
                case KEY_ENTER://ȷ��
                    LZM_SetAlarm(TEST_TIMER, SECS(0.1));
                    break;
                case KEY_UP://--
                    if(s_stTaskAttrib.ControlId>1)
                    {
                        s_stTaskAttrib.ControlId--;
                        flag=1;
                        susCurtStart=GetCurrentPageMsgDataStartAddr(s_stTaskAttrib.ControlId,(unsigned char *)s_apcShowP,0);
                        LZM_SetAlarm(TEST_TIMER, SECS(0.1));
                    }
                    break;
                case KEY_DOWN://+
                    if(s_stTaskAttrib.ControlId < s_stTaskAttrib.ControlMaxId)
                    {
                       LZM_SetAlarm(TEST_TIMER, SECS(0.1));
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        case TEST_TIMER:
            if(!flag)s_stTaskAttrib.ControlId++;
            flag=0;
            if(s_stTaskAttrib.ControlId < s_stTaskAttrib.ControlMaxId)
            {
                LastCount=LcdShowOnePage((unsigned char *)s_apcShowP+susCurtStart);                
                susCurtStart += LastCount;
                LZM_SetAlarm(TEST_TIMER, SECS(15));
            }
            else
            if(s_stTaskAttrib.ControlId == s_stTaskAttrib.ControlMaxId)//��ʾ�����һҳ
            {
                SysAutoRunCountReset();
                LcdClearScreen();                
                LastCount=LcdShowStrEx(0,0,(const char *)s_apcShowP+susCurtStart,0,s_stTaskAttrib.textlen-susCurtStart);
                LZM_SetAlarm(TEST_TIMER, SECS(15));
            }
            else
            if(s_stTaskAttrib.ControlId == s_stTaskAttrib.ControlMaxId+1 && s_stTaskAttrib.ControlMaxId==1)//��ʾ�����һҳ
            {
                SysAutoRunCountReset();
                LZM_SetAlarm(TEST_TIMER, SECS(15));
            }
            else
            {
                ShowTextForMsgBusyFlag(1);///
                ShowMainInterface();
            }
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskShowTextInfo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
LZM_RET TaskShowQuestionResponse(LZM_MESSAGE *Msg)
{
    static unsigned short susCurtStart = 0;
    static unsigned char LastCount = 0;
    static unsigned char flag = 0;
    unsigned char x;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    u16 SerialNum;
    
    switch(Msg->message)
    {
        case TASK_START:
            LZM_KillTimer(REFRESH_TIMER);
            flag = 0;
            LastCount = 0;
            susCurtStart = 0;
            s_stTaskAttrib.step = 0;
            s_stTaskAttrib.select = 0;
            s_stTaskAttrib.ControlId = 0;
            LZM_SetAlarm(TEST_TIMER, SECS(0.1));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    if(0 == s_stTaskAttrib.step)
                    {
                        if(LZM_PreTaskFunction!=TaskShowDisplayText&&LZM_PreTaskFunction!=TaskShowQuestionResponse)
                        {
                            ShowTextForMsgBusyFlag(1);
                            LZM_ReturnOldTaskEx();//ϵͳ����
                        }
                        else
                        {
                            ShowTextForMsgBusyFlag(1);
                            ShowMainInterface();
                        }
                    }
                    else
                    if(1 == s_stTaskAttrib.step)
                    {
                        s_stTaskAttrib.step = 0;
                        flag=1;
                        LZM_SetAlarm(TEST_TIMER, SECS(0.1));
                    }
                    break;
                case KEY_ENTER://ȷ��
                    LZM_KillTimer(TEST_TIMER);
                    if(0 == s_stTaskAttrib.step)
                    {
                        s_stTaskAttrib.step++;
                        LcdClearScreen();   
                        LcdShowCaption("ѡ���:",2);
                        LcdShowHorLine(0, 131, 20, 1);
                        LcdShowStr(0,24,(const char *)s_stTaskAttrib.ShowBuffer[1],0);
                        x=s_stTaskAttrib.select*2*8;
                        LcdReverseBar(x,24,x+8,40);
                        LcdShowHorLine(0, 131, 42, 1);
                        LcdShowCaption("<ȷ��>����",44);
                    }
                    else
                    if(1 == s_stTaskAttrib.step)
                    {
                        
                        //////���ʹ�//////////   
                        channel = RadioProtocol_GetDownloadCmdChannel(0x8302, &SerialNum);
                        if(ACK_OK==RadioProtocol_QuestionAck(channel,s_stTaskAttrib.ShowBuffer[0][s_stTaskAttrib.select]))
                        {
                            LcdClearScreen();
                            LcdShowCaption("�𰸷��ͳɹ�",25);
                            LZM_SetAlarm(REFRESH_TIMER, SECS(3));
                        }
                        else
                        {
                            Public_ShowTextInfo("�𰸷���ʧ��", PUBLICSECS(3));
                        }
                    }
                    break;
                case KEY_UP://--
                    if(0 == s_stTaskAttrib.step)
                    {
                        if(s_stTaskAttrib.ControlId>1)
                        {
                            s_stTaskAttrib.ControlId--;
                            flag=1;
                            susCurtStart=GetCurrentPageMsgDataStartAddr(s_stTaskAttrib.ControlId,(unsigned char *)s_apcShowP,0);
                            LZM_SetAlarm(TEST_TIMER, SECS(0.1));
                        }
                    }
                    else
                    if(1 == s_stTaskAttrib.step)
                    {
                        x=s_stTaskAttrib.select*2*8;
                        LcdReverseBar(x,24,x+8,40);
                        if(s_stTaskAttrib.select>0)
                        {
                            s_stTaskAttrib.select--;
                        }
                        else
                        {
                            s_stTaskAttrib.select=s_stTaskAttrib.answerNum-1;
                        }
                        x=s_stTaskAttrib.select*2*8;
                        LcdReverseBar(x,24,x+8,40);
                    }
                    break;
                case KEY_DOWN://+
                    if(0 == s_stTaskAttrib.step)
                    {
                        if(s_stTaskAttrib.ControlId < s_stTaskAttrib.ControlMaxId)
                        {
                           LZM_SetAlarm(TEST_TIMER, SECS(0.1));
                        }
                    }
                    else
                    if(1 == s_stTaskAttrib.step)
                    {
                        x=s_stTaskAttrib.select*2*8;
                        LcdReverseBar(x,24,x+8,40);
                        if(s_stTaskAttrib.select<s_stTaskAttrib.answerNum-1)
                        {
                            s_stTaskAttrib.select++;
                        }
                        else
                        {
                            s_stTaskAttrib.select =0;
                        }
                        x=s_stTaskAttrib.select*2*8;
                        LcdReverseBar(x,24,x+8,40);
                    }
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        case REFRESH_TIMER:
            if(0==s_stTaskAttrib.step)break;
            if(LZM_PreTaskFunction!=TaskShowQuestionResponse)
            {
                ShowTextForMsgBusyFlag(1);
                LZM_ReturnOldTaskEx();//ϵͳ����
            }
            else
            {
                ShowTextForMsgBusyFlag(1);
                ShowMainInterface();
            }
            break;
        case TEST_TIMER:
            if(!flag)s_stTaskAttrib.ControlId++;
            flag=0;
            if(s_stTaskAttrib.ControlId < s_stTaskAttrib.ControlMaxId)
            {
                LastCount=LcdShowOnePage((unsigned char *)s_apcShowP+susCurtStart);                
                susCurtStart += LastCount;
                LZM_SetAlarm(TEST_TIMER, SECS(15));
            }
            else
            if(s_stTaskAttrib.ControlId == s_stTaskAttrib.ControlMaxId)//��ʾ�����һҳ
            {
                SysAutoRunCountReset();
                LcdClearScreen();                
                LastCount=LcdShowStrEx(0,0,(const char *)s_apcShowP+susCurtStart,0,s_stTaskAttrib.textlen-susCurtStart);
                LZM_SetAlarm(TEST_TIMER, SECS(15));
            }
            else
            if(s_stTaskAttrib.ControlId == s_stTaskAttrib.ControlMaxId+1 && s_stTaskAttrib.ControlMaxId==1)//��ʾ�����һҳ
            {
                SysAutoRunCountReset();
                LZM_SetAlarm(TEST_TIMER, SECS(15));
            }
            else
            {
                ShowTextForMsgBusyFlag(1);///
                ShowMainInterface();
            }
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: ReadInterfaceSwitchflg
** ��������: ��ȡ�������л���־
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
** ����    :fanqinghai
** ����    :2015.07.07
*************************************************************/  
void ReadInterfaceSwitchflg()
{
    if(FRAM_BufferRead(&MenuSwitchFlg,FRAM_INTERFACE_SWOTCH_LEN-1,FRAM_INTERFACE_SWOTCH_ADDR))
    {
        if(MenuSwitchFlg>=MENU_INTERFACE_MAX)MenuSwitchFlg = MENU_INTERFACE_CONVENTION;
    }
    else
    {
        MenuSwitchFlg = 0;
    }    
}
/*************************************************************
** ��������: PublicSysParamterInit
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void PublicSysParamterInit(void)
{
    /////////////////////////
    memset((char *)&s_stTaskAttrib,0,sizeof(s_stTaskAttrib));
    ////////////////////////
    ReadSystemPassWord();
    ReadSystemBacklightFlag();
    ReadInterfaceSwitchflg();   //�����:fanqinghai 2015.0707
    LCD_LIGHT_ON();
    Message_ParameterInitialize();
}
/*************************************************************
** ��������: PublicSysParamterInit
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowMainInterface(void)
{
    LZM_StartNewTask(TaskShowMainInterface);
}
/*************************************************************
** ��������: ReflashToReturnMainInterface
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ReflashToReturnMainInterface(unsigned long time)
{
    if(!GetLcdCopyFlag())LcdCopyScreen();
    LZM_SetAlarm(SHOW_TIMER, time);
}
/*************************************************************
** ��������: SystemAutoShowText
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemAutoShowText(unsigned short textlen,unsigned char *buffer)
{
    s_stTaskAttrib.textlen = textlen;
    s_stTaskAttrib.ControlMaxId = GetCurrentMsgPageNum(textlen,buffer,0);
    s_apcShowP=(char *)buffer;
    LZM_StartNewTask(TaskShowDisplayText);
}
/*************************************************************
** ��������: SystemAutoShowQuestionResponse
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemAutoShowQuestionResponse(unsigned short datalen,unsigned char *buffer,unsigned char flag)
{
    unsigned char temp,ID;
    unsigned short len,len1,len2;
    s_stTaskAttrib.answerNum = 0;
    len =0;
    strcpy((char *)&GB2312Buffer[len],"���⣺          ");
    len=strlen((char *)GB2312Buffer);
    len1 =0;
    ////////����///////
    len2=buffer[len1++];//���ⳤ��
    memcpy((char *)&GB2312Buffer[len],&buffer[len1],len2);//��������
    len += len2;
    len1 += len2;
    temp=GetCurrentNotEnoughRowNum(len,GB2312Buffer);
    if(temp)
    {
        memset(&GB2312Buffer[len],' ',16-temp);
        len +=16-temp;
    }  
    //////////////
    strcpy((char *)&GB2312Buffer[len],"��ѡ�𰸣�      ");
    len=strlen((char *)GB2312Buffer);
    //////////��/////////////////
    temp =0;
    for(;;)
    {
        if(len1+3>=datalen)break;
        /////////��ID/////////////
        ID = buffer[len1++];
        s_stTaskAttrib.ShowBuffer[0][s_stTaskAttrib.answerNum]=ID;        
        ////////�𰸳���///////////////
        len2 = Public_ConvertBufferToShort(&buffer[len1]);
        len1+=2;
        ////////////////////////
        if(len1+len2>datalen)break;
        //////////////////////
        if(ID<=9)//ת��ASCII
        {
            ID+=0x30;
        }
        //////////////////////////////////
        s_stTaskAttrib.ShowBuffer[1][s_stTaskAttrib.answerNum*2]=ID;
        s_stTaskAttrib.ShowBuffer[1][s_stTaskAttrib.answerNum*2+1]=' ';
        ///////////////////////
        GB2312Buffer[len++] = ID;
        strcpy((char *)&GB2312Buffer[len],")");
        len=strlen((char *)GB2312Buffer);
        memcpy((char *)&GB2312Buffer[len],&buffer[len1],len2);//������        
        ///////////////////////////
        temp=GetCurrentNotEnoughRowNum(len2+2,&GB2312Buffer[len-2]);
        if(temp)
        {
            memset(&GB2312Buffer[len+len2],' ',16-temp);
            len +=16-temp;
        }
        /////////////////
        len += len2;
        len1 += len2;
        s_stTaskAttrib.answerNum++;
        //len=strlen((char *)GB2312Buffer);
        ///////////////////////////////
        if(len1>=datalen)break;
    }
    ///////////////////////
    GB2312Buffer[len]='\0';
    ///////////////////////////////////
    if(s_stTaskAttrib.answerNum>8)s_stTaskAttrib.answerNum = 8;
    ///////////////////////
    ///////////////////////////
    s_stTaskAttrib.ShowBuffer[1][s_stTaskAttrib.answerNum*2]='\0';
    ///////////////////////////////////
    if(PUBLIC_CHECKBIT(flag,3))
    {
        Public_PlayTTSVoiceStr(GB2312Buffer);
    }
    s_stTaskAttrib.textlen = len;
    s_stTaskAttrib.ControlMaxId = GetCurrentMsgPageNum(len,GB2312Buffer,0);
    s_apcShowP=(char *)GB2312Buffer;
    LZM_StartNewTask(TaskShowQuestionResponse);
}
/*************************************************************
** ��������: ShowTextForMsgBusyFlag
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void ShowTextForMsgBusyFlag(unsigned char flag)
{
    static unsigned char sucMsgBusyFlag=0;
    if(!flag)
    {
        sucMsgBusyFlag = 1;
        CommonShareBufferBusyFlag = 1;
    }
    else
    if(sucMsgBusyFlag)
    {
        sucMsgBusyFlag=0;
        CommonShareBufferBusyFlag = 0;
    }    
}
/*************************************************************
** ��������: GetShowMainInterfaceFlag
** ��������: �Ƿ���������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char GetShowMainInterfaceFlag(void)
{
    return ((TaskShowMainInterface==LZM_TaskFunction && !Lcd_ShowBigNumFlag())||(LZM_TaskFunction == TaskShowLogo)||(TaskMenuSystemDebugTest==LZM_TaskFunction)||(TaskMenuSystem==LZM_TaskFunction));
}
/*************************************************************
** ��������: IsBrushAdministratorCard
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char IsBrushAdministratorCard(void)
{
    return (s_stServerParam.brushcardFlag == 0x55)?1:0;
}
/*************************************************************
** ��������: BrushAdministratorCardResponse
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char BrushAdministratorCardResponse(unsigned char *data)
{
    unsigned char len,start;
    
    if(TaskGetPassword!=LZM_TaskFunction)return 0;
    
    //////////////////////////
    start=0;
    ////////////////////////
    if(strncmp((char*)&data[start],"601703",6)!=0)
    {
        return 0;
    }
    ////////////////////////////////
    start +=6;
    len=data[start++];
    ConvertStrToIP(&data[start], s_stServerParam.mainIP,len);
    start +=len;
    len=data[start++];
    ConvertStrToIP(&data[start], s_stServerParam.backIP,len);
    start +=len;
    len=data[start++];
    memcpy(s_stServerParam.APN,&data[start],len);
    s_stServerParam.APN[len]='\0';
    start +=len;
    /////////////////////////////////
    s_stServerParam.tcp = Public_ConvertBufferToLong(&data[start]); 
    ////////////////////////////
    start +=4;
    ////////////////////////////
    s_stServerParam.udp = Public_ConvertBufferToLong(&data[start]);
    /////////////////////////
    s_stServerParam.brushcardFlag = 0x55;
    //////////////////////////////////
    PublicGetPasswordCorrect(); 
    ///////////////////////////    
    return 1;
}
/*************************************************************
** ��������: SystemAutoShowText
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
void SystemAutoShowQuestion(void)
{
    SystemAutoShowQuestionResponse(CommonShareBufferLen-1,&RadioShareBuffer[1],RadioShareBuffer[0]);
}
/*************************************************************OK
** ��������: Question_DisposeRadioProtocol
** ��������: �����·�Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���           
** ���ڲ���: 
** ���ز���: :
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Question_DisposeRadioProtocol(u8 *pBuffer, u16 BufferLen)
{
    #ifdef USE_NAVILCD
    if(NaviLcd_GetCurOnlineStatus())
    NaviLcdSendCmdQuestionDisplay(1,pBuffer+1,BufferLen-1);
    #endif
    QuestionResponse_SaveOneData(&pBuffer[1],BufferLen-1);
    CommonShareBufferBusyFlag = 1;//CommonShareBufferæ��־
    CommonShareBufferLen=BufferLen;
    memcpy(RadioShareBuffer,pBuffer,CommonShareBufferLen);
    RadioShareBuffer[CommonShareBufferLen] = 0;//��β��־
    /////////////////
    LZM_SetOperateTimer(SystemAutoShowQuestion, SECS(0.2));
}
/*************************************************************
** ��������: InfoService_DisposeRadioProtocol
** ��������: �㲥��Ϣ����Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char InfoService_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned char AckResult;    
    #ifdef USE_NAVILCD
    if(NaviLcd_GetCurOnlineStatus())
    NaviLcdSendCmdInfoService(1,pBuffer,BufferLen);
    #endif
    AckResult = 2;
    if(BufferLen>3&&BufferLen<1023)
    {
        ////////////////////////
        CommonShareBufferLen=Public_ConvertBufferToShort(&pBuffer[1]);
        if(CommonShareBufferLen+3<=BufferLen)
        {
            AckResult=0;
            memcpy(RadioShareBuffer,&pBuffer[3],CommonShareBufferLen);
            RadioShareBuffer[CommonShareBufferLen] = 0;//��β��־
            CommonShareBufferBusyFlag = 1;//CommonShareBufferæ��־
            InfoService_SaveOneData(RadioShareBuffer,CommonShareBufferLen);
            SetEvTask(EV_SHOW_TEXT);
        }
    }
    return AckResult;
}
/*************************************************************
** ��������: Task_GetCurSystemIsDeepFlag
** ��������: �Ƿ�Ϊ�����־
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Task_GetCurSystemIsDeepFlag(void)
{ 
    return (MENU_INTERFACE_DEEP==MenuSwitchFlg)?1:0;
}
/*************************************************************
** ��������: Task_GetCurSystemIsTurnSpeedFlag
** ��������: �Ƿ�Ϊת�ٱ�־
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Task_GetCurSystemIsTurnSpeedFlag(void)
{ 
    return (MENU_INTERFACE_TURNSPEED==MenuSwitchFlg)?1:0;
}
//////////////////////dxl,��������ӵ������ǲ˵�//////////////
/*************************************************************
** ��������: TaskMenuGyro
** ��������: �����ǲ˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: dxl,2015.6.27����
*************************************************************/  
/*
static LZM_RET TaskMenuGyro(LZM_MESSAGE *Msg)
{ 
    
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_sutMenuGyro,s_apcMenuGyro);
            MenuShow(&s_sutMenuGyro);
            Gyro_ReadSharpTurnAngle(0, 0);//��ת��Ƕ�
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuSystemAdvanced);
                    break;
                case KEY_ENTER: 
                    switch(MenuGetHandle(&s_sutMenuGyro))
                    {
                        case MENUGYRO_ACCELERATION:
                            LZM_StartNewTask(TaskMenuGyroAcceleration);
                            break;
                        case MENUGYRO_DECELERATION:                            
                            LZM_StartNewTask(TaskMenuGyroDeceleration);
                            break; 
                        case MENUGYRO_SHARP_TURN:
                            LZM_StartNewTask(TaskMenuGyroSharpTurn);
                            break;
                        case MENUGYRO_CALIBRATION:
                            PublicConfirm("У׼������",TaskMenuGyroCalibration);
                            break; 
                        default:
                            break;
                    }
                default:
                    MenuResponse(&s_sutMenuGyro,TaskPara);
                    break;
            }
            return 1;
       case INT_RTC:
            break;
       case SHOW_TIMER:
            s_stTaskAttrib.ControlId=0;
            MenuShow(&s_sutMenuGyro);
            break;
       default:
          return LZM_DefTaskProc(Msg);
          //break;
    }
    return 0;
}
*/
/*************************************************************
** ��������: TaskMenuGyroAcceleration
** ��������: ���������ü����ٲ˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: dxl,2015.6.27����
*************************************************************/  
/*
static LZM_RET TaskMenuGyroAcceleration(LZM_MESSAGE *Msg)
{ 
    unsigned char val,buf[10];
    unsigned char i,tmpbuf[16];;
    unsigned short tmp;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            if(0==Gyro_ReadAccelerationThreshold(buf))
            {
                buf[0]=15;
                buf[1]=10;
                buf[2]=7;
                buf[3]=5;
            }
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId =3;
            for(i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
            {
                LcdShowStr(0,i*16,c_apcMenuGyroAcceleration[i],0);                
                ESpinInit(&s_stESpin[i], 64, i*16, 2, buf[i]);
                s_stESpin[i].loop =0;
                ESpinShow(&s_stESpin[i]);
            }            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
          
            return 1;
        case KEY_PRESS: 
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuGyro);
                    break;
                case KEY_ENTER://ȷ��
                    if(0==Gyro_ReadAccelerationThreshold(buf))
                    {
                        buf[0]=15;
                        buf[1]=10;
                        buf[2]=7;
                        buf[3]=5;
                    }
                    for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
                    {
                        tmp=atoi(s_stESpin[i].buffer);
                        buf[i] = tmp;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    Gyro_WriteAccelerationThreshold(buf, FRAM_GYRO_ACCELERATION_PARAM_LEN);
                    Gyro_ReadAccelerationThreshold(tmpbuf);
                    LcdClearArea(0,19,131,44);
                    if(0==Public_CheckArrayValIsEqual(buf,tmpbuf,FRAM_GYRO_ACCELERATION_PARAM_LEN))
                    {
                        Gyro_UpdatePram();
                        LcdShowCaption("����ɹ�", 24);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    val=ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(val)
                    {
                        ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                        if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)s_stTaskAttrib.ControlId++;
                        else s_stTaskAttrib.ControlId = 0;
                        ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                    }
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuGyro);
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
*/
/*************************************************************
** ��������: TaskMenuGyroDeceleration
** ��������: ���������ü����ٲ˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: dxl,2015.6.27����
*************************************************************/ 
/*
static LZM_RET TaskMenuGyroDeceleration(LZM_MESSAGE *Msg)
{
    unsigned char val,buf[10];
    unsigned char i,tmpbuf[16];;
    unsigned short tmp;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();
            if(0 == Gyro_ReadDecelerationThreshold(buf))
            {
                buf[0]=25;
                buf[1]=20;
            }
            s_stTaskAttrib.ControlId=0;  
            s_stTaskAttrib.ControlMaxId =1;
            LcdShowCaption("����������", 0);
            LcdShowHorLine(0, 131, 19, 1);
            for(i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
            {
                LcdShowStr(0,i*16+22,c_apcMenuGyroDeceleration[i],0);                
                ESpinInit(&s_stESpin[i], 64, i*16+22, 2, buf[i]);
                s_stESpin[i].loop =0;
                ESpinShow(&s_stESpin[i]);
            }            
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
          
            return 1;
        case KEY_PRESS: 
            if(s_stTaskAttrib.ControlId==0xff)
            {
                LZM_SetAlarm(TEST_TIMER, 1);
                break;
            }
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuGyro);
                    break;
                case KEY_ENTER://ȷ��
                    if(0 == Gyro_ReadDecelerationThreshold(buf))
                    {
                        buf[0]=25;
                        buf[1]=20;
                    }
                    for( i=0; i<=s_stTaskAttrib.ControlMaxId; i++)
                    {
                        tmp=atoi(s_stESpin[i].buffer);
                        buf[i] = tmp;
                    }
                    LcdClearScreen();
                    LcdShowCaption("���ڱ���...", 24);
                    Gyro_WriteDecelerationThreshold(buf,FRAM_GYRO_DECELERATION_PARAM_LEN);
                    Gyro_ReadDecelerationThreshold(tmpbuf);
                    LcdClearArea(0,19,131,44);
                    if(0==Public_CheckArrayValIsEqual(buf,tmpbuf,2))
                    {
                        Gyro_UpdatePram();
                        LcdShowCaption("����ɹ�", 24);
                    }
                    else
                    {
                       LcdShowCaption("�������,����", 24); 
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    val=ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara); 
                    if(val)
                    {
                        ESpinLoseFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                        if(s_stTaskAttrib.ControlId<s_stTaskAttrib.ControlMaxId)s_stTaskAttrib.ControlId++;
                        else s_stTaskAttrib.ControlId = 0;
                        ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
                    }
                    break;
            }
            return 1;           
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuGyro);
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
*/
/*************************************************************
** ��������: TaskMenuGyroSharpTurn
** ��������: ���ü�ת��˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: dxl,2015.6.29����
*************************************************************/  
/*
static LZM_RET TaskMenuGyroSharpTurn(LZM_MESSAGE *Msg)
{
    unsigned char val;
    u16 temp;
    
    switch(Msg->message)
    {
        case TASK_START:
            temp = Gyro_GetSharpTurnAngle();
            LcdClearScreen();
            s_stTaskAttrib.ControlId=0;
            LcdShowCaption("���ü�ת��",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            ESpinInit(&s_stESpin[s_stTaskAttrib.ControlId], 22, 24, 3,1);
            s_stESpin[s_stTaskAttrib.ControlId].type = 1;//�ַ��� 
            sprintf(s_stESpin[s_stTaskAttrib.ControlId].buffer,"%03d",temp);            
            ESpinShow(&s_stESpin[s_stTaskAttrib.ControlId]);
            ESpinSetFocus(&s_stESpin[s_stTaskAttrib.ControlId]);
            return 1;
        case KEY_PRESS:
            if(s_stTaskAttrib.ControlId==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuGyro);
                    break;
                case KEY_ENTER://ȷ�� 
                    LcdClearArea(0,19,131,44);
                    LcdShowCaption("���ڷ���...", 24);
                    temp=atoi((char const *)(unsigned char *)s_stESpin[s_stTaskAttrib.ControlId].buffer);
                    val = Gyro_WriteSharpTurnAngle(0, 0, temp);
                    LcdClearArea(0,19,131,44);
                    if(val==0)
                    {
                        LcdShowCaption("���ͳɹ�", 24);
                        Gyro_UpdatePram();
                    }
                    else
                    {
                       LcdShowCaption("����ʧ��", 24);
                       
                    }
                    s_stTaskAttrib.ControlId=0xff;
                    LZM_SetAlarm(TEST_TIMER, SECS(1));
                    break;                                
                default:                    
                    ESpinResponse(&s_stESpin[s_stTaskAttrib.ControlId],TaskPara);                     
                    break;
            }
            return 1;
        case TEST_TIMER:
            LZM_StartNewTask(TaskMenuGyro);
            Gyro_ReadSharpTurnAngle(0, 0);//��ת��Ƕ�
            break;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
*/
/*************************************************************
** ��������: TaskMenuGyroCalibration
** ��������: ������У׼�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: dxl,2015.6.29����
*************************************************************/ 
/*
static void TaskMenuGyroCalibration(void)
{
    u8 flag;
    
    LcdClearScreen();
    
    flag = Gyro_Calibrate(0, 0);
    
    if(0 == flag)
    {
        LcdClearArea(0,19,131,44);
        LcdShowCaption("���ͳɹ���У׼��", 24);
    }
    else
    {
        LcdClearArea(0,19,131,44);
        LcdShowCaption("����ʧ��", 24);
    }
    
    LZM_SetAlarm(TEST_TIMER, SECS(10));
   
}
*/
////////////////////////////dxl,end/////////////////////////////

/*******************************************************************************
 *                             end of module
 *******************************************************************************/
