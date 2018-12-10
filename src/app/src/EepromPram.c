/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :EepromPram.c       
//����      :eeprom������д����
//�汾��    :V0.1
//������    :dxl
//����ʱ��  :2011.06
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/

//********************************ͷ�ļ�************************************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR.h"
#include "Can_App.h"

//********************************�Զ�����������****************************
///u8 UpdataReportPram = 0;

//********************************�궨��************************************


//********************************ȫ�ֱ���**********************************
const u16 EepromPramAddress[E2_PRAM_ID_INDEX_MAX] = 
{
    0,
    E2_TERMINAL_HEARTBEAT_ADDR,         //����ID0x0001���ն��������ͼ������λ�룬Ĭ��ֵ60
    E2_TCP_ACK_OVERTIME_ADDR,           //����ID0x0002��TCP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    E2_TCP_RESEND_TIMES_ADDR,           //����ID0x0003��TCP��Ϣ�ش�������Ĭ��ֵ3
    E2_UDP_ACK_OVERTIME_ADDR,           //����ID0x0004��UDP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    E2_UDP_RESEND_TIMES_ADDR,           //����ID0x0005��UDP��Ϣ�ش�������Ĭ��ֵ3
    E2_SMS_ACK_OVERTIME_ADDR,           //����ID0x0006��SMS��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ30
    E2_SMS_RESEND_TIMES_ADDR,           //����ID0x0007��SMS��Ϣ�ش�������Ĭ��ֵ3
    0,0,0,0,0,0,0,0,                //����ID0x0008~0x000f������
    E2_MAIN_SERVER_APN_ADDR,            //����ID0x0010����������APN
    E2_MAIN_SERVER_DIAL_NAME_ADDR,          //����ID0x0011��������������ͨ�Ų����û���
    E2_MAIN_SERVER_DIAL_PASSWORD_ADDR,      //����ID0x0012��������������ͨ�Ų�������
    E2_MAIN_SERVER_IP_ADDR,             //����ID0x0013������������ַ��IP������
    E2_BACKUP_SERVER_APN_ADDR,          //����ID0x0014�����ݷ�����APN������ͨ�Ų��ŷ��ʵ�
    E2_BACKUP_SERVER_DIAL_NAME_ADDR,        //����ID0x0015�����ݷ���������ͨ�Ų����û���
    E2_BACKUP_SERVER_DIAL_PASSWORD_ADDR,        //����ID0x0016�����ݷ���������ͨ�Ų�������
    E2_BACKUP_SERVER_IP_ADDR,           //����ID0x0017�����ݷ�������ַ��IP������
    E2_MAIN_SERVER_TCP_PORT_ADDR,           //����ID0x0018����������TCP�˿�
    E2_MAIN_SERVER_UDP_PORT_ADDR,           //����ID0x0019����������UDP�˿�
        E2_IC_MAIN_SERVER_IP_ADDR,              //����ID0x001A��IC����֤��������IP��ַ������
        E2_IC_MAIN_SERVER_TCP_PORT_ADDR,        //����ID0x001B��IC����֤��������TCP�˿�
        E2_IC_MAIN_SERVER_UDP_PORT_ADDR,        //����ID0x001C��IC����֤��������UDP�˿�
        E2_IC_BACKUP_SERVER_IP_ADDR,                //����ID0x001D��IC����֤���ݷ�����IP��ַ������
    0,0,                        //����ID0x001E~0x001f������
    E2_POSITION_REPORT_STRATEGY_ADDR,       //����ID0x0020��λ�û㱨����
    E2_POSITION_REPORT_SCHEME_ADDR,         //����ID0x0021��λ�û㱨����
    E2_DRIVER_UNLOGIN_REPORT_TIME_ADDR,     //����ID0x0022����ʻԱδ��¼�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60
    0,0,0,0,                    //����ID0x0023~0x0026������
    E2_SLEEP_REPORT_TIME_ADDR,          //����ID0x0027������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60
    E2_EMERGENCY_REPORT_TIME_ADDR,          //����ID0x0028����������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ10
    E2_ACCON_REPORT_TIME_ADDR,          //����ID0x0029ȱʡʱ��㱨���,��λΪ�룬Ĭ��ֵΪ10
    0,0,                        //����ID0x002a~0x002b������
    E2_ACCON_REPORT_DISTANCE_ADDR,          //����ID0x002Cȱʡ����㱨���,��λΪ��,
    E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ADDR,     //����ID0x002D��ʻԱδ��¼�㱨������//��λΪ��//Ĭ��ֵΪ100,                      
    E2_SLEEP_REPORT_DISTANCE_ADDR,          //����ID0x002e������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ1000
    E2_EMERGENCY_REPORT_DISTANCE_ADDR,      //����ID0x002f����������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ100
    E2_CORNER_REPORT_ADDR,          //����ID0x0030���յ㲹���Ƕ�,��λ��/�룬Ĭ��ֵΪ15
    E2_ELECTRON_RADIUS_ADDR,            //����ID0x0031������Χ���뾶
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,            //����ID0x0031~0x003f������
    E2_MONITOR_SERVER_PHONE_ADDR,           //����ID0x0040�����ƽ̨�绰����
    E2_TERMINAL_RESET_PHONE_ADDR,           //����ID0x0041���ն˸�λ�绰����
    E2_TERMINAL_DEFAULT_SET_PHONE_ADDR,     //����ID0x0042���ָ��������õ绰����
    E2_MONITOR_SERVER_SMS_PHONE_ADDR,       //����ID0x0043�����ƽ̨SMS�绰����
    E2_SMS_TEXT_ALARM_PHONE_ADDR,           //����ID0x0044��SMS�ı������绰����
    E2_TERMINAL_GET_PHONE_STRATEGY_ADDR,        //����ID0x0045���ն˵绰��������
    E2_MAX_PHONE_TIME_EACH_ADDR,            //����ID0x0046��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    E2_MAX_PHONE_TIME_MONTH_ADDR,           //����ID0x0047��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    E2_MONITOR_PHONE_ADDR,              //����ID0x0048�������绰����
    E2_MONITOR_SERVER_PRIVILEGE_SMS_ADDR,       //����ID0x0049�����ƽ̨��Ȩ���ź���
    0,0,0,0,0,0,                    //����ID0x004a~0x004f������
    E2_ALARM_MASK_ADDR,             //����ID0x0050�����������֣�Ĭ��Ϊȫ��
    E2_ALARM_TEXT_SWITCH_ADDR,          //����ID0x0051�����������ı�SMS���أ�Ĭ��Ϊȫ��
    E2_ALARM_PHOTO_SWITCH_ADDR,         //����ID0x0052���������㿪�أ�Ĭ��Ϊȫ��
    E2_ALARM_PHOTO_STORE_FLAG_ADDR,         //����ID0x0053����������洢��־��Ĭ��Ϊȫ��
    E2_KEY_FLAG_ADDR,               //����ID0x0054���ؼ���־
    E2_MAX_SPEED_ADDR,              //����ID0x0055������ٶ�
    E2_OVER_SPEED_KEEP_TIME_ADDR,           //����ID0x0056�����ٳ���ʱ�䣬��λΪ�룬Ĭ��Ϊ10��
    E2_MAX_DRVIE_TIME_ADDR,             //����ID0x0057��������ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ240��
    E2_MAX_DRVIE_TIME_ONEDAY_ADDR,          //����ID0x0058�������ۼƼ�ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ0
    E2_MIN_RELAX_TIME_ADDR,             //����ID0x0059����С��Ϣʱ�䣬��λΪ�룬Ĭ��Ϊ1200
    E2_MAX_STOP_CAR_TIME_ADDR,          //����ID0x005A���ͣ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0
        E2_SPEED_EARLY_ALARM_DVALUE_ADDR,       //����ID0x005B�����ٱ���Ԥ����ֵ����λΪ1/10Km/h��Ĭ��Ϊ50
        E2_TIRE_EARLY_ALARM_DVALUE_ADDR,        //����ID0x005C��ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0��Ĭ��Ϊ30
        E2_SET_CRASH_ALARM_ADDR,                //����ID0x005D����ײ������������ 
        E2_SET_ROLLOVER_ALARM_ADDR,             //����ID0x005E���෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ30 ��
        0,0,0,0,0,                                      //����ID0x005F~0x0063������
        E2_TIME_PHOTO_CONTROL_ADDR,             //����ID0x0064����ʱ���տ���
        E2_MILE_PHOTO_CONTROL_ADDR,             //����ID0x0065���������տ���
        0,0,0,0,0,0,0,0,0,0,                            //����ID0x0066~0x006f������
    E2_PICTURE_QUALITY_ADDR,            //����ID0x0070��ͼ��/��Ƶ������1~10,1Ϊ���
    E2_PICTURE_BRIGHTNESS_ADDR,         //����ID0x0071�����ȣ�0~255
    E2_PICTURE_CONTRAST_ADDR,           //����ID0x0072���Աȶȣ�0~127
    E2_PICTURE_SATURATION_ADDR,         //����ID0x0073�����Ͷȣ�0~127
    E2_PICTURE_CHROMA_ADDR,             //����ID0x0074��ɫ�ȣ�0~255
    0,0,0,0,0,0,0,0,0,0,0,              //����ID0x0075~0x007f������
    E2_CAR_TOTAL_MILE_ADDR,             //����ID0x0080��������̱������0.1km
    E2_CAR_PROVINCE_ID_ADDR,            //����ID0x0081���������ڵ�ʡ��ID
    E2_CAR_CITY_ID_ADDR,                //����ID0x0082���������ڵ�����ID
    E2_CAR_PLATE_NUM_ADDR,              //����ID0x0083��������ͨ�����Ű䷢�Ļ���������
    E2_CAR_PLATE_COLOR_ADDR,            //����ID0x0084��������ɫ������JT/T415-2006��5.4.12
        0,0,0,0,0,0,0,0,0,0,0,                          //����ID0x0085~0x008f,����
        E2_GPS_SET_MODE_ADDR,                       //����ID0x0090��GNSS ��λģʽ��Ĭ��Ϊ0x02 ������λ
        E2_GPS_SET_BAUD_ADDR,                       //����ID0x0091��GNSS �����ʣ�Ĭ��Ϊ0x01 9600
        E2_GPS_SET_OUTPUT_RATE_ADDR,                //����ID0x0092��GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms
        E2_GPS_SET_GATHER_RATE_ADDR,                //����ID0x0093��GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1
        E2_GPS_SET_UPLOAD_MODE_ADDR,                //����ID0x0094��GNSS ģ����ϸ��λ�����ϴ���ʽ
        E2_GPS_SET_UPLOAD_VALUE_ADDR,               //����ID0x0095��GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0
        0,0,0,0,0,0,0,0,0,0,                            //����ID0x0096~0x009f,����                          
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00a0~0x00af,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00b0~0x00bf,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00c0~0x00cf,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00d0~0x00df,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00e0~0x00ef,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00f0~0x00ff,����
        E2_CAN1_GATHER_TIME_ADDR,               //����ID0x0100��CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�DWORD
        E2_CAN1_UPLOAD_TIME_ADDR,               //����ID0x0101��CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�WORD
        E2_CAN2_GATHER_TIME_ADDR,               //����ID0x0102��CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�DWORD
        E2_CAN2_UPLOAD_TIME_ADDR,               //����ID0x0103��CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�WORD
        0,0,0,0,0,0,0,0,0,0,0,0,                        //����ID0x0104~0x010f,����
        E2_CAN_SET_ONLY_GATHER_0_ADDR,              //����ID0x0110//CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_1_ADDR,              //����ID0x0111//CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_2_ADDR,              //����ID0x0112//CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_3_ADDR,              //����ID0x0113//CAN ����ID �����ɼ�����BYTE[8] 
        E2_CAN_SET_ONLY_GATHER_4_ADDR,              //����ID0x0114//CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_5_ADDR,              //����ID0x0115//CAN ����ID �����ɼ�����BYTE[8] 
        E2_CAN_SET_ONLY_GATHER_6_ADDR,              //����ID0x0116//CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_7_ADDR,              //����ID0x0117//CAN ����ID �����ɼ�����BYTE[8]         

        
        E2_LOGINGPRS_PASSWORD_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x00���Զ��壬��Ȩ��
        E2_REGISTER_SMS_PHONE_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x01���Զ��壬ע�����Ϣ����
    E2_PROGRAM_UPDATA_REQUEST_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x02���Զ��壬�������������־
    E2_PROGRAM_UPDATA_VERIFYCODE_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x03���Զ��壬��������У����
    E2_PROGRAM_UPDATA_LENGTH_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x04���Զ��壬������������
    E2_MANUFACTURE_ID_ADDR,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x05���Զ��壬������ID��5�ֽ�
    E2_DEVICE_ID_ADDR,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x06���Զ��壬�����ն�ID��7�ֽ�
    E2_DEVICE_PHONE_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x07���Զ��壬�ն��ֻ��ţ�6�ֽ�
    E2_CAR_IDENTIFICATION_CODE_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x08���Զ��壬����ʶ����
    E2_CAR_TYPE_ID_ADDR,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x09���Զ��壬���Ʒ���
    E2_CURRENT_DRIVER_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x0A���Զ��壬��ǰ��ʻԱ����
    E2_CURRENT_LICENSE_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x0B���Զ��壬��ǰ��ʻ֤����
    E2_FIRMWARE_VERSION_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x0C���Զ��壬�̼��汾��
    E2_ALL_DRIVER_INFORMATION_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0D���Զ��壬���м�ʻԱ��Ϣ
    E2_SPEED_SENSOR_COEF_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x0E���Զ��壬�ٶȴ�����ϵ��
    E2_CAR_FEATURE_COEF_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x0F���Զ��壬��������ϵ��
    E2_SPEED_SELECT_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x10���Զ��壬�ٶ�ѡ�񿪹�
    E2_BACKLIGHT_SELECT_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x11���Զ��壬����ѡ�񿪹�
    E2_FIRST_POWERON_FLAG_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x12���Զ��壬ϵͳ��1���ϵ��־
    E2_ACC_ON_INTERVAL_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x13���Զ��壬ACC ON ʱ�㱨ʱ����
    E2_ACC_ON_DISTANCE_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x14���Զ��壬ACC ON ʱ�㱨������
    E2_UPDATA_LINK_CONTROL_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x15���Զ��壬���ӿ���
    E2_UPDATA_DIAL_NAME_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x16���Զ��壬�����������ŵ�����
    E2_UPDATA_DIAL_USER_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x17���Զ��壬�������������û���
    E2_UPDATA_DIAL_PASSWORD_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x18���Զ��壬����������������
    E2_UPDATA_IP_ID_ADDR,               //����IDE2_PRAM_BASE_CUSTOM_ID+0x19���Զ��壬��������IP��ַ
    E2_UPDATA_TCP_PORT_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x1a���Զ��壬��������TCP�˿�
    E2_UPDATA_UDP_PORT_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x1b���Զ��壬��������UDP�˿�
    E2_UPDATA_MONITOR_SERVER_CODE_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1c���Զ��壬�����������ƽ̨��Ȩ��
    E2_UPDATA_HARDWARE_VERSION_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x1d���Զ��壬��������Ӳ���汾
    E2_UPDATA_FIRMWARE_VERSION_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x1e���Զ��壬���������̼��汾
    E2_UPDATA_URL_ID_ADDR,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x1f���Զ��壬��������URL������ַ
    E2_UPDATA_TIME_ID_ADDR,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x20���Զ��壬�����������ӵ�ָ��������ʱ��
    E2_HOT_PHONE_ID_ADDR,               //����IDE2_PRAM_BASE_CUSTOM_ID+0x21���Զ��壬���ߵ绰
    E2_UPDATA_MANUFACTURE_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x22���Զ��壬��������������ID
    E2_PHONE_CALLBACK_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x23���Զ��壬�绰�ز�����
    E2_CALLBACK_FLAG_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x24���Զ��壬�绰�ز���־,1Ϊ����;0Ϊ��ͨͨ��
    E2_DEVICE_PHONE_BACKUP_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x25���Զ��壬�ն��ֻ��ţ�����
    E2_LAST_LOCATION_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x26���Զ��壬���һ��������λ��
        E2_PHOTO_SPACE_ID_ADDR,                         //����IDE2_PRAM_BASE_CUSTOM_ID+0x27���Զ��壬����ʱ����
        E2_PHOTO_NUM_ID_ADDR,                           //����IDE2_PRAM_BASE_CUSTOM_ID+0x28���Զ��壬��������
        E2_DOOR_SET_ID_ADDR,                            //����IDE2_PRAM_BASE_CUSTOM_ID+0x29���Զ��壬����ѡ��0Ϊ���Ÿ���Ч��1Ϊ��������Ч
    E2_DEVICE_BACKUP_ID_ADDR,                       //����IDE2_PRAM_BASE_CUSTOM_ID+0x2a���Զ��壬�ն�ID����
    E2_CDMA_SLEEP_ID_ADDR,                          //����IDE2_PRAM_BASE_CUSTOM_ID+0x2b���Զ��壬CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
    E2_PASSWORD_ID_ADDR,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x2c���Զ��壬ά���˵���������
    E2_INSTALL_TIME_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x2d���Զ��壬��װ����
    E2_INSTALL_TIME_FLAG_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x2e���Զ��壬��װ����д���־
    E2_HORN_SET_ID_ADDR,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x2f���Զ��壬�����ȣ����룩ѡ��1Ϊ����Ч����1Ϊ����Ч��Ĭ��Ϊ����Ч
    E2_GPS_CONTROL_ID_ADDR,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x30���Զ��壬ACC OFFʱGPSģ�����,0Ϊ��,1Ϊ��
    E2_USB_UPDATA_FLAG_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x31���Զ��壬USB������־
    E2_TAXIMETER_CONTROL_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x32���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
    E2_TOPLIGHT_CONTROL_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x33���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ����
    E2_LEDLIGHT_CONTROL_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x34���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ����
    E2_HB_IC_DRIVER_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x35���Զ��壬���÷����б��׼��ʽ��0x0702ָ��ļ�ʻԱ��Ϣ
    E2_STATUS_PHOTO_SWITCH_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x36���Զ��壬״̬���㿪��
    E2_STATUS_PHOTO_STORE_FLAG_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x37���Զ��壬״̬����洢��־//Ĭ��Ϊȫ��
    E2_RESUME_NOUSED_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x38���Զ��壬�ָ��ն�Ϊδʹ��״̬
    E2_NIGHT_TIME0_ID_ADDR,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x39���Զ��壬ҹ��ʱ�䷶Χ,BCD��,��1���汾ʹ��
    E2_NIGHT_MAX_DRVIE_TIME0_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x3a���Զ��壬ҹ�䳬ʱ��ʻ����,��1���汾ʹ��
    E2_NIGHT_MAX_SPEED0_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x3b���Զ��壬ҹ������ֵ,��1���汾ʹ��
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3C���Զ��壬V1��13����Э��VTK�������ź�SOS�����л���VTK�ã��б��޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3D���Զ��壬V1��13����Э��VTK�����Զ��������ʱ�䣬VTK�ã��б��޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3E���Զ��壬V1��13����Э��VTK��������ʱ�䣬VTK�ã��б��޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3F���Զ��壬V1��13����Э��VTK�����𶯼��ʱ�䣬VTK�ã��б��޴���
    E2_LOAD_MAX_ID_ADDR,                        ////����IDE2_PRAM_BASE_CUSTOM_ID+0x40���Զ��壬�����������
    E2_CAR_INIT_MILE_ID_ADDR,                       //����IDE2_PRAM_BASE_CUSTOM_ID+0x41���Զ��壬������ʼ����̣�DWORD
    E2_CAR_ONLY_NUM_ID_ADDR,                        //����IDE2_PRAM_BASE_CUSTOM_ID+0x42���Զ��壬Ψһ�Ա�ţ��ַ�����35�ֽ�
    E2_DEFINE_ALARM_MASK_WORD_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x43���Զ��壬�Զ��屨�������֣�DWORD
    E2_DEFINE_STATUS_CHANGE_PHOTO_ENABLE_ID_ADDR,   //����IDE2_PRAM_BASE_CUSTOM_ID+0x44���Զ��壬�Զ���״̬�仯����ʹ�ܿ��أ�DWORD
    E2_DEFINE_STATUS_CHANGE_PHOTO_STORE_ID_ADDR,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x45���Զ��壬�Զ���״̬�仯���մ洢���أ�DWORD
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x46����С����Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x47���������Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x48������ٶ���ֵ���Զ��壬EGS702�ã�EGS701�޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x49��������ʼ�ͺģ��Զ��壬�ϵ�EGS701�ã�EGS701�޴���
    E2_SHUTDOWN_MODE_TIME_ID_ADDR,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x4a���Զ��壬�ػ�ģʽʱ�������
    E2_SHUTDOWN_MODE_POWER_ID_ADDR,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x4b���Զ��壬������ߵ�ѹ��
    E2_UNLOGIN_TTS_PLAY_ID_ADDR,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x4c���Զ��壬��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ������IC��ʱ��������Ϊ������������Ϊ��
    E2_TERMINAL_TYPE_ID_ADDR,                       //����IDE2_PRAM_BASE_CUSTOM_ID+0x4d���Զ��壬�ն����ͣ�����Э���ѯ�ն����������õ�������
    E2_GPS_ATTRIBUTE_ID_ADDR,                       //����IDE2_PRAM_BASE_CUSTOM_ID+0x4e���Զ��壬GNSS���ԣ�����Э���ѯ�ն����������õ�������
    E2_GPRS_ATTRIBUTE_ID_ADDR,                      //����ID E2_PRAM_BASE_CUSTOM_ID+0x4f���Զ��壬ͨѶģ�����ԣ�����Э���ѯ�ն����������õ�������
    E2_OVERSPEED_PREALARM_KEEPTIME_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x50���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
    E2_OVERSPEED_PREALARM_VOICE_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x51���Զ��壬����Ԥ������,STRING,���30�ֽ�
    E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_ADDR,  //����IDE2_PRAM_BASE_CUSTOM_ID+0x52���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_OVERSPEED_PREALARM_NUMBER_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x53���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_ADDR,  //����IDE2_PRAM_BASE_CUSTOM_ID+0x54���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    E2_OVERSPEED_ALARM_VOICE_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x55���Զ��壬���ٱ�������,STRING,���30�ֽ�
    E2_OVERSPEED_ALARM_GPROUP_TIME_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x56���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_OVERSPEED_ALARM_NUMBER_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x57���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_OVERSPEED_ALARM_NUMBER_TIME_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x58���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    E2_OVERSPEED_ALARM_REPORT_TIME_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x59���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
    E2_OVERSPEED_NIGHT_TIME_ID_ADDR,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x5a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    E2_NIGHT_OVERSPEED_PERCENT_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x5b���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
    E2_NIGHT_DRIVE_PREALARM_TIME_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x5c���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
    E2_NIGHT_DRIVE_PREALARM_VOICE_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5d���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�,��������ҹ��ʱ��
    E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_ADDR, //����IDE2_PRAM_BASE_CUSTOM_ID+0x5e���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
    E2_NIGHT_DRIVE_PREALARM_NUMBER_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x5f���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
    E2_FORBID_DRIVE_NIGHT_TIME_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x60���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    E2_FORBID_DRIVE_PREALARM_TIME_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x61���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
    E2_FORBID_DRIVE_PREALARM_VOICE_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x62���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
    E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID_ADDR,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x63���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_FORBID_DRIVE_PREALARM_NUMBER_ID_ADDR,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x64���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    E2_FORBID_DRIVE_ALARM_VOICE_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x65���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
    E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID_ADDR,   //����IDE2_PRAM_BASE_CUSTOM_ID+0x66���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_FORBID_DRIVE_ALARM_NUMBER_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x67���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    E2_FORBID_DRIVE_RUN_SPEED_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x68���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
    E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID_ADDR, //����IDE2_PRAM_BASE_CUSTOM_ID+0x69���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
    E2_TIRED_DRIVE_NIGHT_TIME_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
    E2_NIGHT_MAX_DRVIE_TIME_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x6b���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
    E2_NIGHT_MIN_RELAX_TIME_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x6c���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
    E2_TIRED_DRIVE_ALARM_TIME_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6d���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
    E2_TIRED_DRIVE_PREALARM_VOICE_ID_ADDR,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6e���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_ADDR, //����IDE2_PRAM_BASE_CUSTOM_ID+0x6f���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    E2_TIRED_DRIVE_PREALARM_NUMBER_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x70���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_ADDR,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x71���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    E2_TIRED_DRIVE_ALARM_VOICE_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x72���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_ADDR,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x73���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    E2_TIRED_DRIVE_ALARM_NUMBER_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x74���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_ADDR,   //����IDE2_PRAM_BASE_CUSTOM_ID+0x75���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID_ADDR,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x76���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
    E2_TIRED_DRIVE_REPORT_TIME_ID_ADDR,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x77���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
    E2_TIRED_DRIVE_RUN_SPEED_ID_ADDR,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x78���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
    E2_TIRED_DRIVE_RUN_KEEPTIME_ID_ADDR,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x79���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
    E2_ROUTE_EXCURSION_ALARM_NUMBER_ID_ADDR,      //����E2_PRAM_BASE_CUSTOM_ID+0x7A//����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
        E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID_ADDR,  //����E2_PRAM_BASE_CUSTOM_ID+0x7B//����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
        E2_ROUTE_EXCURSION_ALARM_VOICE_ID_ADDR,       //����E2_PRAM_BASE_CUSTOM_ID+0x7C//����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
        E2_REPORT_FREQ_EVENT_SWITCH_ID_ADDR,          //����E2_PRAM_BASE_CUSTOM_ID+0x7D//����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,DWORD
        E2_RUN_FORBID_CALL_SWITCH_ID_ADDR,              //����E2_PRAM_BASE_CUSTOM_ID+0x7E//����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
        E2_SIM_ICCID_NUMBER_ID_ADDR,                    //����E2_PRAM_BASE_CUSTOM_ID+0x7F//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
        E2_LED_PORT_ID_ADDR,//����ID���Զ��壬����32pin��LED�ӿڽӵ���������,
        E2_POS_PORT_ID_ADDR,//����ID���Զ��壬����16pin�ӿڽӵ���������,
        0,//�ò���EGS702ʹ��
        E2_OBD_SWITCH_ID_ADDR,//����ID���Զ��壬1�ֽڣ�0��ʾδ������OBDģ�飬1��ʾ����
        E2_SECOND_MAIN_SERVER_IP_ID_ADDR,               //����ID E2_PRAM_BASE_CUSTOM_ID+0x84���Զ��壬��2��������������IP��ַ������
        E2_SECOND_MAIN_SERVER_TCP_PORT_ID_ADDR,         //����ID E2_PRAM_BASE_CUSTOM_ID+0x85���Զ��壬��2��������������TCP�˿�
        E2_SECOND_MAIN_SERVER_UDP_PORT_ID_ADDR,         //����ID E2_PRAM_BASE_CUSTOM_ID+0x86���Զ��壬��2��������������UDP�˿�
        E2_SECOND_BACKUP_SERVER_IP_ID_ADDR,             //����ID E2_PRAM_BASE_CUSTOM_ID+0x87���Զ��壬��2�����ӱ��ݷ�������ַ//IP������
        E2_SECOND_LOGINGPRS_PASSWORD_ID_ADDR,           //����ID E2_PRAM_BASE_CUSTOM_ID+0x88���Զ��壬��2�����Ӽ�Ȩ��
        E2_SECOND_ACC_OFF_REPORT_TIME_ID_ADDR,          //����ID E2_PRAM_BASE_CUSTOM_ID+0x89���Զ��壬��2���������߻㱨ʱ����,��λ��,DWORD
        E2_SECOND_ACC_ON_REPORT_TIME_ID_ADDR,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x8a���Զ��壬��2������ȱʡ�㱨ʱ����,��λ��,DWORD
        E2_LOCK1_ENABLE_FLAG_ID_ADDR,                  //����IDE2_PRAM_BASE_CUSTOM_ID+0x8b���Զ��壬ʹ��ǰ����һ
        E2_LOCK2_ENABLE_FLAG_ID_ADDR,                  //����IDE2_PRAM_BASE_CUSTOM_ID+0x8c���Զ��壬ʹ��ǰ������
				0,0,0,0,0,0,0,0,0,
				E2_DEEP_SENSORMIN_ID_ADDR,                     //����ID(E2_PRAM_BASE_CUSTOM_ID+0x96)���Զ��壬����������Сֵ
				E2_DEEP_SENSORMIN_LENGTH_ID_ADDR,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x97)���Զ��壬��������Сֵ��Ӧ�ĸ����λ����
				E2_DEEP_BASE_HEIGHT_ID_ADDR,                   //����ID(E2_PRAM_BASE_CUSTOM_ID+0x98)���Զ��壬�����׼ֵ����λ����
				E2_DEEP_ARM_LENGTH_ID_ADDR,                    //����ID(E2_PRAM_BASE_CUSTOM_ID+0x99)���Զ��壬��׼�۳�����λ����
				E2_DEEP_MAX_LENGTH_ID_ADDR,                    //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9A)���Զ��壬��׼�߶ȣ���λ����
				E2_DEEP_SENSORMAX_ID_ADDR,                     //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9B)���Զ��壬�����������ֵ
				E2_DEEP_SENSORMAX_LENGTH_ID_ADDR,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9C)���Զ��壬���������ֵ��Ӧ�ĸ����λ����
				E2_ENGINE_NUM_ID_ADDR,                           //??ID(E2_PRAM_BASE_CUSTOM_ID+0x9D),???,?????
};

const u8 EepromPramLength[E2_PRAM_ID_INDEX_MAX] = 
{
    0,
    E2_TERMINAL_HEARTBEAT_LEN,          //����ID0x0001���ն��������ͼ������λ�룬Ĭ��ֵ60
    E2_TCP_ACK_OVERTIME_LEN,            //����ID0x0002��TCP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    E2_TCP_RESEND_TIMES_LEN,            //����ID0x0003��TCP��Ϣ�ش�������Ĭ��ֵ3
    E2_UDP_ACK_OVERTIME_LEN,            //����ID0x0004��UDP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    E2_UDP_RESEND_TIMES_LEN,            //����ID0x0005��UDP��Ϣ�ش�������Ĭ��ֵ3
    E2_SMS_ACK_OVERTIME_LEN,            //����ID0x0006��SMS��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ30
    E2_SMS_RESEND_TIMES_LEN,            //����ID0x0007��SMS��Ϣ�ش�������Ĭ��ֵ3
    0,0,0,0,0,0,0,0,                //����ID0x0008~0x000f������
    E2_MAIN_SERVER_APN_LEN,             //����ID0x0010����������APN
    E2_MAIN_SERVER_DIAL_NAME_LEN,           //����ID0x0011��������������ͨ�Ų����û���
    E2_MAIN_SERVER_DIAL_PASSWORD_LEN,       //����ID0x0012��������������ͨ�Ų�������
    E2_MAIN_SERVER_IP_LEN,              //����ID0x0013������������ַ��IP������
    E2_BACKUP_SERVER_APN_LEN,           //����ID0x0014�����ݷ�����APN������ͨ�Ų��ŷ��ʵ�
    E2_BACKUP_SERVER_DIAL_NAME_LEN,         //����ID0x0015�����ݷ���������ͨ�Ų����û���
    E2_BACKUP_SERVER_DIAL_PASSWORD_LEN,     //����ID0x0016�����ݷ���������ͨ�Ų�������
    E2_BACKUP_SERVER_IP_LEN,            //����ID0x0017�����ݷ�������ַ��IP������
    E2_MAIN_SERVER_TCP_PORT_LEN,            //����ID0x0018����������TCP�˿�
    E2_MAIN_SERVER_UDP_PORT_LEN,            //����ID0x0019����������UDP�˿�
        E2_IC_MAIN_SERVER_IP_LEN,               //����ID0x001A, IC����֤��������IP��ַ������
        E2_IC_MAIN_SERVER_TCP_PORT_LEN,             //����ID0x001B, IC����֤��������TCP�˿�
        E2_IC_MAIN_SERVER_UDP_PORT_LEN,             //����ID0x001C, IC����֤��������UDP�˿�
        E2_IC_BACKUP_SERVER_IP_LEN,             //����ID0x001D, IC����֤���ݷ�����IP��ַ������
    0,0,                        //����ID0x001E~0x001f������
    E2_POSITION_REPORT_STRATEGY_LEN,        //����ID0x0020��λ�û㱨����
    E2_POSITION_REPORT_SCHEME_LEN,          //����ID0x0021��λ�û㱨����
    E2_DRIVER_UNLOGIN_REPORT_TIME_LEN,      //����ID0x0022����ʻԱδ��¼�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60
    0,0,0,0,                    //����ID0x0023~0x0026������
    E2_SLEEP_REPORT_TIME_LEN,           //����ID0x0027������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60
    E2_EMERGENCY_REPORT_TIME_LEN,           //����ID0x0028����������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ10
    E2_ACCON_REPORT_TIME_LEN,           //����ID0x0029ȱʡʱ��㱨���,��λΪ�룬Ĭ��ֵΪ10
    0,0,                        //����ID0x002a~0x002b������
    E2_ACCON_REPORT_DISTANCE_LEN,           //����ID0x002Cȱʡ����㱨���,��λΪ��,
    E2_DRIVER_UNLOGIN_REPORT_DISTANCE_LEN,      //����ID0x002D��ʻԱδ��¼�㱨������//��λΪ��//Ĭ��ֵΪ100,  
    E2_SLEEP_REPORT_DISTANCE_LEN,           //����ID0x002e������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ1000
    E2_EMERGENCY_REPORT_DISTANCE_LEN,       //����ID0x002f����������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ100
    E2_CORNER_REPORT_LEN,           //����ID0x0030���յ㲹���Ƕ�,��λ��/�룬Ĭ��ֵΪ15
    E2_ELECTRON_RADIUS_LEN,             //����ID0x0031������Χ���뾶
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,            //����ID0x0031~0x003f������
    E2_MONITOR_SERVER_PHONE_LEN,            //����ID0x0040�����ƽ̨�绰����
    E2_TERMINAL_RESET_PHONE_LEN,            //����ID0x0041���ն˸�λ�绰����
    E2_TERMINAL_DEFAULT_SET_PHONE_LEN,      //����ID0x0042���ָ��������õ绰����
    E2_MONITOR_SERVER_SMS_PHONE_LEN,        //����ID0x0043�����ƽ̨SMS�绰����
    E2_SMS_TEXT_ALARM_PHONE_LEN,            //����ID0x0044��SMS�ı������绰����
    E2_TERMINAL_GET_PHONE_STRATEGY_LEN,     //����ID0x0045���ն˵绰��������
    E2_MAX_PHONE_TIME_EACH_LEN,         //����ID0x0046��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    E2_MAX_PHONE_TIME_MONTH_LEN,            //����ID0x0047��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    E2_MONITOR_PHONE_LEN,               //����ID0x0048�������绰����
    E2_MONITOR_SERVER_PRIVILEGE_SMS_LEN,        //����ID0x0049�����ƽ̨��Ȩ���ź���
    0,0,0,0,0,0,                    //����ID0x004a~0x004f������
    E2_ALARM_MASK_LEN,              //����ID0x0050�����������֣�Ĭ��Ϊȫ��
    E2_ALARM_TEXT_SWITCH_LEN,           //����ID0x0051�����������ı�SMS���أ�Ĭ��Ϊȫ��
    E2_ALARM_PHOTO_SWITCH_LEN,          //����ID0x0052���������㿪�أ�Ĭ��Ϊȫ��
    E2_ALARM_PHOTO_STORE_FLAG_LEN,          //����ID0x0053����������洢��־��Ĭ��Ϊȫ��
    E2_KEY_FLAG_LEN,                //����ID0x0054���ؼ���־
    E2_MAX_SPEED_LEN,               //����ID0x0055������ٶ�
    E2_OVER_SPEED_KEEP_TIME_LEN,            //����ID0x0056�����ٳ���ʱ�䣬��λΪ�룬Ĭ��Ϊ10��
    E2_MAX_DRVIE_TIME_LEN,              //����ID0x0057��������ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ240��
    E2_MAX_DRVIE_TIME_LEN,              //����ID0x0058�������ۼƼ�ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ0
    E2_MIN_RELAX_TIME_LEN,              //����ID0x0059����С��Ϣʱ�䣬��λΪ�룬Ĭ��Ϊ1200
    E2_MAX_STOP_CAR_TIME_LEN,           //����ID0x005A���ͣ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0
    E2_SPEED_EARLY_ALARM_DVALUE_LEN,            //����ID0x005B,���ٱ���Ԥ����ֵ����λΪ1/10Km/h��Ĭ��Ϊ50
        E2_TIRE_EARLY_ALARM_DVALUE_LEN,             //����ID0x005C,ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0��Ĭ��Ϊ30
        E2_SET_CRASH_ALARM_LEN,                     //����ID0x005D,��ײ������������ 
        E2_SET_ROLLOVER_ALARM_LEN,              //����ID0x005E,�෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ30 ��
        0,0,0,0,0,                                      //����ID0x005F~0x0063������
        E2_TIME_PHOTO_CONTROL_LEN,              //����ID0x0064,��ʱ���տ���
        E2_MILE_PHOTO_CONTROL_LEN,              //����ID0x0065,�������տ���
        0,0,0,0,0,0,0,0,0,0,                            //����ID0x0066~0x006f������
    E2_PICTURE_QUALITY_LEN,             //����ID0x0070��ͼ��/��Ƶ������1~10,1Ϊ���
    E2_PICTURE_BRIGHTNESS_LEN,          //����ID0x0071�����ȣ�0~255
    E2_PICTURE_CONTRAST_LEN,            //����ID0x0072���Աȶȣ�0~127
    E2_PICTURE_SATURATION_LEN,          //����ID0x0073�����Ͷȣ�0~127
    E2_PICTURE_CHROMA_LEN,              //����ID0x0074��ɫ�ȣ�0~255
    0,0,0,0,0,0,0,0,0,0,0,              //����ID0x0075~0x007f������
    E2_CAR_TOTAL_MILE_LEN,              //����ID0x0080��������̱������0.1km
    E2_CAR_PROVINCE_ID_LEN,             //����ID0x0081���������ڵ�ʡ��ID
    E2_CAR_CITY_ID_LEN,             //����ID0x0082���������ڵ�����ID
    E2_CAR_PLATE_NUM_LEN,               //����ID0x0083��������ͨ�����Ű䷢�Ļ���������
    E2_CAR_PLATE_COLOR_LEN,             //����ID0x0084��������ɫ������JT/T415-2006��5.4.12
        0,0,0,0,0,0,0,0,0,0,0,                          //����ID0x0085~0x008f,����
        E2_GPS_SET_MODE_LEN,                        //����ID0x0090,GNSS ��λģʽ��Ĭ��Ϊ0x02 ������λ
        E2_GPS_SET_BAUD_LEN,                        //����ID0x0091,GNSS �����ʣ�Ĭ��Ϊ0x01 9600
        E2_GPS_SET_OUTPUT_RATE_LEN,             //����ID0x0092,GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms
        E2_GPS_SET_GATHER_RATE_LEN,             //����ID0x0093,GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1
        E2_GPS_SET_UPLOAD_MODE_LEN,             //����ID0x0094,GNSS ģ����ϸ��λ�����ϴ���ʽ��Ĭ��Ϊ0x00�����ش洢���ϴ�
        E2_GPS_SET_UPLOAD_VALUE_LEN,                //����ID0x0095,GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0            
        0,0,0,0,0,0,0,0,0,0,                            //����ID0x0096~0x009f,����                          
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00a0~0x00af,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00b0~0x00bf,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00c0~0x00cf,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00d0~0x00df,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00e0~0x00ef,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00f0~0x00ff,����
        E2_CAN1_GATHER_TIME_LEN,                //����ID0x0100,CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
        E2_CAN1_UPLOAD_TIME_LEN,                //����ID0x0101,CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
        E2_CAN2_GATHER_TIME_LEN,                //����ID0x0102,CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
        E2_CAN2_UPLOAD_TIME_LEN,                //����ID0x0103,CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
        0,0,0,0,0,0,0,0,0,0,0,0,                        //����ID0x0104~0x010f,���� 
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0110,CAN ����ID �����ɼ�����BYTE[8]                
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0111,CAN ����ID �����ɼ�����BYTE[8] 
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0112,CAN ����ID �����ɼ�����BYTE[8]                
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0113,CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0114,CAN ����ID �����ɼ�����BYTE[8]                
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0115,CAN ����ID �����ɼ�����BYTE[8]
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0116,CAN ����ID �����ɼ�����BYTE[8]                
        E2_CAN_SET_ONLY_GATHER_LEN,             //����ID0x0117,CAN ����ID �����ɼ�����BYTE[8]				
                 
        E2_LOGINGPRS_PASSWORD_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x00���Զ��壬��Ȩ��
    E2_REGISTER_SMS_PHONE_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x01���Զ��壬ע�����Ϣ����
    E2_PROGRAM_UPDATA_REQUEST_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x02���Զ��壬�������������־
    E2_PROGRAM_UPDATA_VERIFYCODE_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x03���Զ��壬��������У����
    E2_PROGRAM_UPDATA_LENGTH_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x04���Զ��壬������������
    E2_MANUFACTURE_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x05���Զ��壬������ID��5�ֽ�
    E2_DEVICE_ID_LEN,               //����IDE2_PRAM_BASE_CUSTOM_ID+0x06���Զ��壬�����ն�ID��7�ֽ�
    E2_DEVICE_PHONE_ID_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x07���Զ��壬�ն��ֻ��ţ�6�ֽ�
    E2_CAR_IDENTIFICATION_CODE_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x08���Զ��壬����ʶ����
    E2_CAR_TYPE_ID_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x09���Զ��壬���Ʒ���
    E2_CURRENT_DRIVER_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x0A���Զ��壬��ǰ��ʻԱ����
    E2_CURRENT_LICENSE_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x0B���Զ��壬��ǰ��ʻ֤����
    E2_FIRMWARE_VERSION_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x0C���Զ��壬�̼��汾��
    E2_ALL_DRIVER_INFORMATION_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x0D���Զ��壬���м�ʻԱ��Ϣ
    E2_SPEED_SENSOR_COEF_ID_LEN,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x0E���Զ��壬�ٶȴ�����ϵ��
    E2_CAR_FEATURE_COEF_LEN,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x0F���Զ��壬��������ϵ��
    E2_SPEED_SELECT_LEN,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x10���Զ��壬�ٶ�ѡ�񿪹�
    E2_BACKLIGHT_SELECT_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x11���Զ��壬����ѡ�񿪹�
    E2_FIRST_POWERON_FLAG_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x12���Զ��壬ϵͳ��1���ϵ��־
    E2_ACC_ON_INTERVAL_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x13���Զ��壬ACC ON ʱ�㱨ʱ����
    E2_ACC_ON_DISTANCE_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x14���Զ��壬ACC ON ʱ�㱨������
    E2_UPDATA_LINK_CONTROL_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x15���Զ��壬���ӿ���
    E2_UPDATA_DIAL_NAME_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x16���Զ��壬�����������ŵ�����
    E2_UPDATA_DIAL_USER_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x17���Զ��壬�������������û���
    E2_UPDATA_DIAL_PASSWORD_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x18���Զ��壬����������������
    E2_UPDATA_IP_ID_LEN,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x19���Զ��壬��������IP��ַ
    E2_UPDATA_TCP_PORT_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x1a���Զ��壬��������TCP�˿�
    E2_UPDATA_UDP_PORT_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x1b���Զ��壬��������UDP�˿�
    E2_UPDATA_MONITOR_SERVER_CODE_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x1c���Զ��壬�����������ƽ̨��Ȩ��
    E2_UPDATA_HARDWARE_VERSION_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1d���Զ��壬��������Ӳ���汾
    E2_UPDATA_FIRMWARE_VERSION_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1e���Զ��壬���������̼��汾
    E2_UPDATA_URL_ID_LEN,               //����IDE2_PRAM_BASE_CUSTOM_ID+0x1f���Զ��壬��������URL������ַ
    E2_UPDATA_TIME_ID_LEN,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x20���Զ��壬�����������ӵ�ָ��������ʱ��
    E2_HOT_PHONE_ID_LEN,                //����IDE2_PRAM_BASE_CUSTOM_ID+0x21���Զ��壬���ߵ绰
    E2_UPDATA_MANUFACTURE_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x22���Զ��壬��������������ID
    E2_PHONE_CALLBACK_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x23���Զ��壬�绰�ز�����
    E2_CALLBACK_FLAG_ID_LEN,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x24���Զ��壬�绰�ز���־,1Ϊ����;0Ϊ��ͨͨ��
    E2_DEVICE_PHONE_BACKUP_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x25���Զ��壬�ն��ֻ��ţ�����
    E2_LAST_LOCATION_ID_LEN,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x26���Զ��壬���һ��������λ��
        E2_PHOTO_SPACE_ID_LEN,                          //����IDE2_PRAM_BASE_CUSTOM_ID+0x27���Զ��壬����ʱ����
        E2_PHOTO_NUM_ID_LEN,                            //����IDE2_PRAM_BASE_CUSTOM_ID+0x28���Զ��壬��������
        E2_DOOR_SET_LEN,                                //����IDE2_PRAM_BASE_CUSTOM_ID+0x29���Զ��壬����ѡ��0Ϊ���Ÿ���Ч��1Ϊ��������Ч
    E2_DEVICE_BACKUP_ID_LEN,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x2a���Զ��壬�ն�ID����
    E2_CDMA_SLEEP_ID_LEN,               //����IDE2_PRAM_BASE_CUSTOM_ID+0x2b���Զ��壬CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
    E2_PASSWORD_ID_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x2c���Զ��壬ά���˵���������
    E2_INSTALL_TIME_ID_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x2d���Զ��壬��װ����
    E2_INSTALL_TIME_FLAG_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x2e���Զ��壬��װ����д���־
    E2_HORN_SET_ID_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x2f���Զ��壬�����ȣ����룩ѡ��1Ϊ����Ч����1Ϊ����Ч��Ĭ��Ϊ����Ч
    E2_GPS_CONTROL_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x30���Զ��壬ACC OFFʱGPSģ�����,0Ϊ��,1Ϊ��
    E2_USB_UPDATA_FLAG_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x31���Զ��壬USB������־
    E2_TAXIMETER_CONTROL_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x32���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
    E2_TOPLIGHT_CONTROL_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x33���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ����
    E2_LEDLIGHT_CONTROL_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x34���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ����
    E2_HB_IC_DRIVER_ID_LEN,             //����IDE2_PRAM_BASE_CUSTOM_ID+0x35���Զ��壬���÷����б��׼��ʽ��0x0702ָ��ļ�ʻԱ��Ϣ
    E2_STATUS_PHOTO_SWITCH_ID_LEN ,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x36���Զ��壬״̬���㿪��
    E2_STATUS_PHOTO_STORE_FLAG_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x37���Զ��壬״̬����洢��־//Ĭ��Ϊȫ��
    E2_RESUME_NOUSED_ID_LEN ,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x38���Զ��壬�ָ��ն�Ϊδʹ��״̬
    E2_NIGHT_TIME0_ID_LEN,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x39���Զ��壬ҹ��ʱ�䷶Χ,BCD��
    E2_NIGHT_MAX_DRVIE_TIME0_ID_LEN,            //����IDE2_PRAM_BASE_CUSTOM_ID+0x3a���Զ��壬ҹ�䳬ʱ��ʻ����
    E2_NIGHT_MAX_SPEED0_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x3b���Զ��壬ҹ������ֵ
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3C���Զ��壬V1��13����Э��VTK�������ź�SOS�����л���VTK�ã��б��޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3D���Զ��壬V1��13����Э��VTK�����Զ��������ʱ�䣬VTK�ã��б��޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3E���Զ��壬V1��13����Э��VTK��������ʱ�䣬VTK�ã��б��޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3F���Զ��壬V1��13����Э��VTK�����𶯼��ʱ�䣬VTK�ã��б��޴���
    E2_LOAD_MAX_ID_LEN,                     ////����IDE2_PRAM_BASE_CUSTOM_ID+0x40��//����ID���Զ��壬����������ƣ�������ֵ���г��ر�������λΪkg��4�ֽ�
    E2_CAR_INIT_MILE_ID_LEN,                     //����IDE2_PRAM_BASE_CUSTOM_ID+0x41���Զ��壬������ʼ����̣�DWORD
    E2_CAR_ONLY_NUM_ID_LEN,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x42���Զ��壬Ψһ�Ա�ţ��ַ�����35�ֽ�
    E2_DEFINE_ALARM_MASK_WORD_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x43���Զ��壬�Զ��屨�������֣�DWORD
    E2_DEFINE_STATUS_CHANGE_PHOTO_ENABLE_ID_LEN,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x44���Զ��壬�Զ���״̬�仯����ʹ�ܿ��أ�DWORD
    E2_DEFINE_STATUS_CHANGE_PHOTO_STORE_ID_LEN, //����IDE2_PRAM_BASE_CUSTOM_ID+0x45���Զ��壬�Զ���״̬�仯���մ洢���أ�DWORD
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x46����С����Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x47���������Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x48������ٶ���ֵ���Զ��壬EGS702�ã�EGS701�޴���
    0,                      //����IDE2_PRAM_BASE_CUSTOM_ID+0x49��������ʼ�ͺģ��Զ��壬�ϵ�EGS701�ã�EGS701�޴���
    E2_SHUTDOWN_MODE_TIME_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x4a���Զ��壬�ػ�ģʽʱ�������
    E2_SHUTDOWN_MODE_POWER_ID_LEN,          //����IDE2_PRAM_BASE_CUSTOM_ID+0x4b���Զ��壬������ߵ�ѹ��
    E2_UNLOGIN_TTS_PLAY_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x4c���Զ��壬��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ������IC��ʱ��������Ϊ������������Ϊ��
    E2_TERMINAL_TYPE_ID_LEN,                        //����IDE2_PRAM_BASE_CUSTOM_ID+0x4d���Զ��壬�ն����ͣ�����Э���ѯ�ն����������õ�������
    E2_GPS_ATTRIBUTE_ID_LEN,                        //����IDE2_PRAM_BASE_CUSTOM_ID+0x4e���Զ��壬GNSS���ԣ�����Э���ѯ�ն����������õ�������
    E2_GPRS_ATTRIBUTE_ID_LEN,                       //����ID E2_PRAM_BASE_CUSTOM_ID+0x4f���Զ��壬ͨѶģ�����ԣ�����Э���ѯ�ն����������õ�������
    E2_OVERSPEED_PREALARM_KEEPTIME_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x50���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
    E2_OVERSPEED_PREALARM_VOICE_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x51���Զ��壬����Ԥ������,STRING,���30�ֽ�
    E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_LEN,   //����IDE2_PRAM_BASE_CUSTOM_ID+0x52���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_OVERSPEED_PREALARM_NUMBER_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x53���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_LEN,   //����IDE2_PRAM_BASE_CUSTOM_ID+0x54���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    E2_OVERSPEED_ALARM_VOICE_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x55���Զ��壬���ٱ�������,STRING,���30�ֽ�
    E2_OVERSPEED_ALARM_GPROUP_TIME_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x56���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_OVERSPEED_ALARM_NUMBER_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x57���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_OVERSPEED_ALARM_NUMBER_TIME_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x58���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    E2_OVERSPEED_ALARM_REPORT_TIME_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x59���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
    E2_OVERSPEED_NIGHT_TIME_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x5a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    E2_NIGHT_OVERSPEED_PERCENT_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5b���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
    E2_NIGHT_DRIVE_PREALARM_TIME_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x5c���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
    E2_NIGHT_DRIVE_PREALARM_VOICE_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x5d���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�
    E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_LEN,  //����IDE2_PRAM_BASE_CUSTOM_ID+0x5e���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
    E2_NIGHT_DRIVE_PREALARM_NUMBER_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5f���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
    E2_FORBID_DRIVE_NIGHT_TIME_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x60���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    E2_FORBID_DRIVE_PREALARM_TIME_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x61���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
    E2_FORBID_DRIVE_PREALARM_VOICE_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x62���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
    E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID_LEN, //����IDE2_PRAM_BASE_CUSTOM_ID+0x63���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_FORBID_DRIVE_PREALARM_NUMBER_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x64���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    E2_FORBID_DRIVE_ALARM_VOICE_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x65���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
    E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID_LEN,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x66���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    E2_FORBID_DRIVE_ALARM_NUMBER_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x67���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    E2_FORBID_DRIVE_RUN_SPEED_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x68���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
    E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID_LEN,  //����IDE2_PRAM_BASE_CUSTOM_ID+0x69���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
    E2_TIRED_DRIVE_NIGHT_TIME_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x6a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
    E2_NIGHT_MAX_DRVIE_TIME_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x6b���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
    E2_NIGHT_MIN_RELAX_TIME_ID_LEN,         //����IDE2_PRAM_BASE_CUSTOM_ID+0x6c���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
    E2_TIRED_DRIVE_ALARM_TIME_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x6d���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
    E2_TIRED_DRIVE_PREALARM_VOICE_ID_LEN,       //����IDE2_PRAM_BASE_CUSTOM_ID+0x6e���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN,  //����IDE2_PRAM_BASE_CUSTOM_ID+0x6f���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    E2_TIRED_DRIVE_PREALARM_NUMBER_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x70���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_LEN, //����IDE2_PRAM_BASE_CUSTOM_ID+0x71���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    E2_TIRED_DRIVE_ALARM_VOICE_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x72���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x73���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    E2_TIRED_DRIVE_ALARM_NUMBER_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x74���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
    E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_LEN,    //����IDE2_PRAM_BASE_CUSTOM_ID+0x75���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x76���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
    E2_TIRED_DRIVE_REPORT_TIME_ID_LEN,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x77���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
    E2_TIRED_DRIVE_RUN_SPEED_ID_LEN,        //����IDE2_PRAM_BASE_CUSTOM_ID+0x78���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
    E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN,     //����IDE2_PRAM_BASE_CUSTOM_ID+0x79���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
    E2_ROUTE_EXCURSION_ALARM_NUMBER_ID_LEN,      //����E2_PRAM_BASE_CUSTOM_ID+0x7A//����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
        E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID_LEN,  //����E2_PRAM_BASE_CUSTOM_ID+0x7B//����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
        E2_ROUTE_EXCURSION_ALARM_VOICE_ID_LEN,       //����E2_PRAM_BASE_CUSTOM_ID+0x7C//����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
        E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN,          //����E2_PRAM_BASE_CUSTOM_ID+0x7D//����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,DWORD
        E2_RUN_FORBID_CALL_SWITCH_ID_LEN,              //����E2_PRAM_BASE_CUSTOM_ID+0x7E//����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
        E2_SIM_ICCID_NUMBER_ID_LEN,                    //����E2_PRAM_BASE_CUSTOM_ID+0x7F//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
        E2_LED_PORT_ID_LEN,//����E2_PRAM_BASE_CUSTOM_ID+0x80����ID���Զ��壬����32pin��LED�ӿڽӵ���������,
        E2_POS_PORT_ID_LEN,////����ID���Զ��壬����16pin�ӿڽӵ���������
        0,//�ò���EGS702ʹ��
        E2_OBD_SWITCH_ID_LEN,//����ID���Զ��壬1�ֽڣ�0��ʾδ������OBDģ�飬1��ʾ����
        E2_SECOND_MAIN_SERVER_IP_ID_LEN,                //����ID E2_PRAM_BASE_CUSTOM_ID+0x84���Զ��壬��2��������������IP��ַ������
        E2_SECOND_MAIN_SERVER_TCP_PORT_ID_LEN,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x85���Զ��壬��2��������������TCP�˿�
        E2_SECOND_MAIN_SERVER_UDP_PORT_ID_LEN,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x86���Զ��壬��2��������������UDP�˿�
        E2_SECOND_BACKUP_SERVER_IP_ID_LEN,          //����ID E2_PRAM_BASE_CUSTOM_ID+0x87���Զ��壬��2�����ӱ��ݷ�������ַ//IP������
        E2_SECOND_LOGINGPRS_PASSWORD_ID_LEN,            //����ID E2_PRAM_BASE_CUSTOM_ID+0x88���Զ��壬��2�����Ӽ�Ȩ��
        E2_SECOND_ACC_OFF_REPORT_TIME_ID_LEN,          //����ID E2_PRAM_BASE_CUSTOM_ID+0x89���Զ��壬��2���������߻㱨ʱ����,��λ��,DWORD
        E2_SECOND_ACC_ON_REPORT_TIME_ID_LEN,           //����IDE2_PRAM_BASE_CUSTOM_ID+0x8a���Զ��壬��2������ȱʡ�㱨ʱ����,��λ��,DWORD
        E2_LOCK1_ENABLE_FLAG_ID_LEN,                  //�Զ��壬ʹ��ǰ����һ
        E2_LOCK2_ENABLE_FLAG_ID_LEN,                  //�Զ��壬ʹ��ǰ������
				0,0,0,0,0,0,0,0,0,
				E2_DEEP_SENSORMIN_ID_LEN,                     //����ID(E2_PRAM_BASE_CUSTOM_ID+0x96)���Զ��壬����������Сֵ
				E2_DEEP_SENSORMIN_LENGTH_ID_LEN,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x97)���Զ��壬��������Сֵ��Ӧ�ĸ����λ����
				E2_DEEP_BASE_HEIGHT_ID_LEN,                   //����ID(E2_PRAM_BASE_CUSTOM_ID+0x98)���Զ��壬�����׼ֵ����λ����
				E2_DEEP_ARM_LENGTH_ID_LEN,                    //����ID(E2_PRAM_BASE_CUSTOM_ID+0x99)���Զ��壬��׼�۳�����λ����
				E2_DEEP_MAX_LENGTH_ID_LEN,                    //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9A)���Զ��壬��׼�߶ȣ���λ����
				E2_DEEP_SENSORMAX_ID_LEN,                     //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9B)���Զ��壬�����������ֵ
				E2_DEEP_SENSORMAX_LENGTH_ID_LEN,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9C)���Զ��壬���������ֵ��Ӧ�ĸ����λ����
				E2_ENGINE_NUM_ID_LEN,                           //??ID(E2_PRAM_BASE_CUSTOM_ID+0x9D),???,?????,??16???

};
const u8    EepromPramSwitch[E2_PRAM_ID_INDEX_MAX] = 
{
    0,
    0,      //����ID0x0001���ն��������ͼ������λ�룬Ĭ��ֵ60
    0,      //����ID0x0002��TCP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    0,      //����ID0x0003��TCP��Ϣ�ش�������Ĭ��ֵ3
    0,      //����ID0x0004��UDP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    0,      //����ID0x0005��UDP��Ϣ�ش�������Ĭ��ֵ3
    0,      //����ID0x0006��SMS��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ30
    0,      //����ID0x0007��SMS��Ϣ�ش�������Ĭ��ֵ3
    0,0,0,0,0,0,0,0,//����ID0x0008~0x000f������
    1,      //����ID0x0010����������APN
    1,      //����ID0x0011��������������ͨ�Ų����û���
    1,      //����ID0x0012��������������ͨ�Ų�������
    1,      //����ID0x0013������������ַ��IP������
    1,      //����ID0x0014�����ݷ�����APN������ͨ�Ų��ŷ��ʵ�
    1,      //����ID0x0015�����ݷ���������ͨ�Ų����û���
    1,      //����ID0x0016�����ݷ���������ͨ�Ų�������
    1,      //����ID0x0017�����ݷ�������ַ��IP������
    0,      //����ID0x0018����������TCP�˿�
    0,      //����ID0x0019����������UDP�˿�
        1,      //����ID0x001A,IC����֤��������IP��ַ������
        0,      //����ID0x001B,IC����֤��������TCP�˿�
        0,      //����ID0x001C,IC����֤��������UDP�˿�
        1,      //����ID0x001D,IC����֤���ݷ�����IP��ַ������
    0,0,        //����ID0x001E~0x001f������
    0,      //����ID0x0020��λ�û㱨����
    0,      //����ID0x0021��λ�û㱨����
    0,      //����ID0x0022����ʻԱδ��¼�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60
    0,0,0,0,    //����ID0x0023~0x0026������
    0,      //����ID0x0027������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60
    0,      //����ID0x0028����������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ10
    0,      //����ID0x0029��ȱʡʱ��㱨���,��λΪ�룬Ĭ��ֵΪ10
    0,0,        //����ID0x002a~0x002b������
    0,      //����ID0x002C,ȱʡ����㱨���,��λΪ�ף�Ĭ��ֵΪ1000
    0,      //����ID0x002D����ʻԱδ��¼�㱨������//��λΪ��//Ĭ��ֵΪ100     
    0,      //����ID0x002e������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ1000
    0,      //����ID0x002f����������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ100
    0,      //����ID0x0030���յ㲹���Ƕ�,��λ��/�룬Ĭ��ֵΪ15
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  //����ID0x0031~0x003f������
    1,      //����ID0x0040�����ƽ̨�绰����
    1,      //����ID0x0041���ն˸�λ�绰����
    1,      //����ID0x0042���ָ��������õ绰����
    1,      //����ID0x0043�����ƽ̨SMS�绰����
    1,      //����ID0x0044��SMS�ı������绰����
    0,      //����ID0x0045���ն˵绰��������
    0,      //����ID0x0046��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    0,      //����ID0x0047��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    1,      //����ID0x0048�������绰����
    1,      //����ID0x0049�����ƽ̨��Ȩ���ź���
    0,0,0,0,0,0,    //����ID0x004a~0x004f������
    0,      //����ID0x0050�����������֣�Ĭ��Ϊȫ��
    0,      //����ID0x0051�����������ı�SMS���أ�Ĭ��Ϊȫ��
    0,      //����ID0x0052���������㿪�أ�Ĭ��Ϊȫ��
    0,      //����ID0x0053����������洢��־��Ĭ��Ϊȫ��
    0,      //����ID0x0054���ؼ���־
    0,      //����ID0x0055������ٶ�
    0,      //����ID0x0056�����ٳ���ʱ�䣬��λΪ�룬Ĭ��Ϊ10��
    0,      //����ID0x0057��������ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ240��
    0,      //����ID0x0058�������ۼƼ�ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ0
    0,      //����ID0x0059����С��Ϣʱ�䣬��λΪ�룬Ĭ��Ϊ1200
    0,      //����ID0x005A���ͣ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0
        0,      //����ID0x005B,���ٱ���Ԥ����ֵ����λΪ1/10Km/h��Ĭ��Ϊ50
        0,      //����ID0x005C,ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0��Ĭ��Ϊ30
        0,      //����ID0x005D,��ײ������������ 
        0,      //����ID0x005E,�෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ30 ��
        0,0,0,0,0,      //����ID0x005F~0x0063������
        0,      //����ID0x0064,��ʱ���տ���
        0,      //����ID0x0065,�������տ���
        0,0,0,0,0,0,0,0,0,0,            //����ID0x0066~0x006f������
    0,      //����ID0x0070��ͼ��/��Ƶ������1~10,1Ϊ���
    0,      //����ID0x0071�����ȣ�0~255
    0,      //����ID0x0072���Աȶȣ�0~127
    0,      //����ID0x0073�����Ͷȣ�0~127
    0,      //����ID0x0074��ɫ�ȣ�0~255
    0,0,0,0,0,0,0,0,0,0,0,      //����ID0x0075~0x007f������
    0,      //����ID0x0080��������̱������0.1km
    0,      //����ID0x0081���������ڵ�ʡ��ID
    0,      //����ID0x0082���������ڵ�����ID
    1,      //����ID0x0083��������ͨ�����Ű䷢�Ļ���������
    0,      //����ID0x0084��������ɫ������JT/T415-2006��5.4.12
        0,0,0,0,0,0,0,0,0,0,0,          //����ID0x0085~0x008f,����       
        0,      //����ID0x0090,GNSS ��λģʽ��Ĭ��Ϊ0x02 ������λ
        0,      //����ID0x0091,GNSS �����ʣ�Ĭ��Ϊ0x01 9600
        0,      //����ID0x0092,GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms
        0,      //����ID0x0093,GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1
        0,      //����ID0x0094,GNSS ģ����ϸ��λ�����ϴ���ʽ��Ĭ��Ϊ0x00�����ش洢���ϴ�
        0,      //����ID0x0095,GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0
        0,0,0,0,0,0,0,0,0,0,                            //����ID0x0096~0x009f,����                          
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00a0~0x00af,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00b0~0x00bf,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00c0~0x00cf,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00d0~0x00df,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00e0~0x00ef,����
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                //����ID0x00f0~0x00ff,����
        0,      //����ID0x0100,CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
        0,      //����ID0x0101,CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
        0,      //����ID0x0102,CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
        0,      //����ID0x0103,CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
        0,0,0,0,0,0,0,0,0,0,0,0,                        //����ID0x0104~0x010f
        1,      //����ID0x0110,CAN ����ID �����ɼ�����BYTE[8]   
        1,      //����ID0x0111,CAN ����ID �����ɼ�����BYTE[8]   
				1,      //����ID0x0112,CAN ����ID �����ɼ�����BYTE[8]   
        1,      //����ID0x0113,CAN ����ID �����ɼ�����BYTE[8]
				1,      //����ID0x0114,CAN ����ID �����ɼ�����BYTE[8]   
        1,      //����ID0x0115,CAN ����ID �����ɼ�����BYTE[8]
				1,      //����ID0x0116,CAN ����ID �����ɼ�����BYTE[8]   
        1,      //����ID0x0117,CAN ����ID �����ɼ�����BYTE[8]
        
        1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x00���Զ��壬��Ȩ��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x01���Զ��壬ע�����Ϣ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x02���Զ��壬�������������־
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x03���Զ��壬��������У����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x04���Զ��壬������������
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x05���Զ��壬������ID��5�ֽ�
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x06���Զ��壬�����ն�ID��7�ֽ�
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x07���Զ��壬�ն��ֻ��ţ�6�ֽ�
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x08���Զ��壬����ʶ����
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x09���Զ��壬���Ʒ���
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0A���Զ��壬��ǰ��ʻԱ����
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0B���Զ��壬��ǰ��ʻ֤����
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0C���Զ��壬�̼��汾��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0D���Զ��壬���м�ʻԱ��Ϣ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0E���Զ��壬�ٶȴ�����ϵ��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x0F���Զ��壬��������ϵ��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x10���Զ��壬�ٶ�ѡ�񿪹�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x11���Զ��壬����ѡ�񿪹�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x12���Զ��壬ϵͳ��1���ϵ��־
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x13���Զ��壬ACC ON ʱ�㱨ʱ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x14���Զ��壬ACC ON ʱ�㱨������
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x15���Զ��壬���ӿ���
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x16���Զ��壬�����������ŵ�����
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x17���Զ��壬�������������û���
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x18���Զ��壬����������������
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x19���Զ��壬��������IP��ַ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1a���Զ��壬��������TCP�˿�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1b���Զ��壬��������UDP�˿�
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1c���Զ��壬�����������ƽ̨��Ȩ��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1d���Զ��壬��������Ӳ���汾
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1e���Զ��壬���������̼��汾
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x1f���Զ��壬��������URL������ַ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x20���Զ��壬�����������ӵ�ָ��������ʱ��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x21���Զ��壬���ߵ绰
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x22���Զ��壬��������������ID
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x23���Զ��壬�绰�ز�����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x24���Զ��壬�绰�ز���־,1Ϊ����;0Ϊ��ͨͨ��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x25���Զ��壬�ն��ֻ��ţ�����
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x26���Զ��壬���һ��������λ��
        0,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x27���Զ��壬����ʱ����
        0,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x28���Զ��壬��������
        0,              //����IDE2_PRAM_BASE_CUSTOM_ID+0x29���Զ��壬����ѡ��0Ϊ���Ÿ���Ч��1Ϊ��������Ч
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x2a���Զ��壬�ն�ID����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x2b���Զ��壬CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x2c���Զ��壬ά���˵���������
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x2d���Զ��壬��װ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x2e���Զ��壬��װ����д���־
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x2f���Զ��壬�����ȣ����룩ѡ��1Ϊ����Ч����1Ϊ����Ч��Ĭ��Ϊ����Ч
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x30���Զ��壬ACC OFFʱGPSģ�����,0Ϊ��,1Ϊ��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x31���Զ��壬���������ܿ���,0Ϊ�ر�,1Ϊ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x32���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x33���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x34���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ����
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x35���Զ��壬���÷����б��׼��ʽ��0x0702ָ��ļ�ʻԱ��Ϣ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x36���Զ��壬״̬���㿪��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x37���Զ��壬״̬����洢��־//Ĭ��Ϊȫ��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x38���Զ��壬�ָ��ն�Ϊδʹ��״̬
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x39���Զ��壬ҹ��ʱ�䷶Χ,BCD��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3a���Զ��壬ҹ�䳬ʱ��ʻ����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3b���Զ��壬ҹ������ֵ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3C���Զ��壬V1��13����Э��VTK�������ź�SOS�����л���VTK�ã��б��޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3D���Զ��壬V1��13����Э��VTK�����Զ��������ʱ�䣬VTK�ã��б��޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3E���Զ��壬V1��13����Э��VTK��������ʱ�䣬VTK�ã��б��޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x3F���Զ��壬V1��13����Э��VTK�����𶯼��ʱ�䣬VTK�ã��б��޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x40���Զ���������ֵ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x41���Զ��壬������ʼ����̣�DWORD
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x42���Զ��壬Ψһ�Ա�ţ��ַ�����35�ֽ�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x43���Զ��壬�Զ��屨�������֣�DWORD
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x44���Զ��壬�Զ���״̬�仯����ʹ�ܿ��أ�DWORD
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x45���Զ��壬�Զ���״̬�仯���մ洢���أ�DWORD
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x46����С����Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x47���������Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x48������ٶ���ֵ���Զ��壬EGS702�ã�EGS701�޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x49��������ʼ�ͺģ��Զ��壬�ϵ�EGS701�ã�EGS701�޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x4a���ػ�ģʽʱ��������Զ��壬EGS702�ã�EGS701�޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x4b��������ߵ�ѹ���Զ��壬EGS702�ã�EGS701�޴���
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x4c���Զ��壬��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ������IC��ʱ��������Ϊ������������Ϊ��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x4d���Զ��壬�ն����ͣ�����Э���ѯ�ն����������õ�������
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x4e���Զ��壬GNSS���ԣ�����Э���ѯ�ն����������õ�������
    0,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x4f���Զ��壬ͨѶģ�����ԣ�����Э���ѯ�ն����������õ�������
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x50���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x51���Զ��壬����Ԥ������,STRING,���30�ֽ�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x52���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x53���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x54���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x55���Զ��壬���ٱ�������,STRING,���30�ֽ�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x56���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x57���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x58���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x59���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5b���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5c���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5d���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5e���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x5f���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x60���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x61���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x62���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x63���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x64���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x65���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x66���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x67���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x68���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x69���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6b���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6c���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6d���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6e���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x6f���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x70���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x71���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    1,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x72���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x73���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x74���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x75���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x76���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x77���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x78���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
    0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x79���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
    0,      //����E2_PRAM_BASE_CUSTOM_ID+0x7A//����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
        0,      //����E2_PRAM_BASE_CUSTOM_ID+0x7B//����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
        1,      //����E2_PRAM_BASE_CUSTOM_ID+0x7C//����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
        0,      //����E2_PRAM_BASE_CUSTOM_ID+0x7D//����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,DWORD
        0,      //����E2_PRAM_BASE_CUSTOM_ID+0x7E//����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
        1,      //����E2_PRAM_BASE_CUSTOM_ID+0x7F//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
        0,      //����E2_PRAM_BASE_CUSTOM_ID+0x80����ID���Զ��壬����32pin��LED�ӿڽӵ���������,
        0,      ////����E2_PRAM_BASE_CUSTOM_ID+0x81���Զ��壬����16pin�ӿڽӵ���������
        0,      //�ò���EGS702ʹ��
        0,      //����ID���Զ��壬1�ֽڣ�0��ʾδ������OBDģ�飬1��ʾ����
        1,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x84���Զ��壬��2��������������IP��ַ������
        0,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x85���Զ��壬��2��������������TCP�˿�
        0,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x86���Զ��壬��2��������������UDP�˿�
        1,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x87���Զ��壬��2�����ӱ��ݷ�������ַ//IP������
        1,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x88���Զ��壬��2�����Ӽ�Ȩ��
        0,      //����ID E2_PRAM_BASE_CUSTOM_ID+0x89���Զ��壬��2���������߻㱨ʱ����,��λ��,DWORD
        0,      //����IDE2_PRAM_BASE_CUSTOM_ID+0x8a���Զ��壬��2������ȱʡ�㱨ʱ����,��λ��,DWORD
        0,              //�Զ��壬ʹ��ǰ����һ
        0,               //�Զ��壬ʹ��ǰ������
				0,0,0,0,0,0,0,0,0,//����ID 0xF28D-0xF295����
				0,                     //����ID(E2_PRAM_BASE_CUSTOM_ID+0x96)���Զ��壬����������Сֵ
				0,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x97)���Զ��壬��������Сֵ��Ӧ�ĸ����λ����
				0,                   //����ID(E2_PRAM_BASE_CUSTOM_ID+0x98)���Զ��壬�����׼ֵ����λ����
				0,                    //����ID(E2_PRAM_BASE_CUSTOM_ID+0x99)���Զ��壬��׼�۳�����λ����
				0,                    //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9A)���Զ��壬��׼�߶ȣ���λ����
				0,                     //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9B)���Զ��壬�����������ֵ
				0,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9C)���Զ��壬���������ֵ��Ӧ�ĸ����λ����
				1,              //����ID(E2_PRAM_BASE_CUSTOM_ID+0x9D)���Զ��壬����ģʽ
};

//********************************�ⲿ����**********************************
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ������ģʽ
extern u16 DelayAckCommand;//��ʱӦ������
//********************************���ر���**********************************

//*******************************Ӧ����*********************************

//********************************��������**********************************
/*********************************************************************
//��������  :EepromPram_WritePram(u32 PramID, u8 *pBuffer, BufferLen)
//����      :дeeprom����
//����      :PramID:����ID;pBuffer:ָ�����ݣ���д����ֽ���,д�������ֵ�Ļ�����ֽ�Ӧ��ǰ����ָ��;
//      :BufferLen:���ݵĳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :д����ȷʱ�����صĳ��ȵ���BufferLen;д�����ʱ������0,��ʱ�ɳ����ٵ��øú���дһ��
*********************************************************************/
u16 EepromPram_WritePram(u32 PramID, u8 *pBuffer, u8 BufferLen)
{
    u8  VerifyByte;
    u8  sum;
    u8  i;
    u8  data;
    u8  Buffer[50];
    u8  PramType;//0Ϊֵ,1Ϊ�ַ���
    u16 Address;
    u16 ID;
    //////////////////
    ID=E2ParamApp_ConvertPramIDToGroupIndex(PramID);
    if(0==ID)//����ʶ��ID
    {
        return 0;//����ʱ����
    }
    //�ж�ID�Ĵ�С
    if(ID >= E2_PRAM_ID_INDEX_MAX)
    {
        return 0;
    }
    //�жϵ�ַ
    if(0 == EepromPramAddress[ID])
    {
        return 0;
    }
    //�жϳ����Ƿ���ȷ
    if((BufferLen > EepromPramLength[ID])||(0 == BufferLen))
    {
        return 0;//���Ȼ����ID����
    }

    //��У���
    VerifyByte = 0;
    for(i=0; i<BufferLen; i++)
    {
        VerifyByte += *(pBuffer+i);
    }
    //��ȡ��������
    PramType = EepromPramSwitch[ID];
    //**************д����****************
    //��ȡ��ַ
    Address = EepromPramAddress[ID];

    if(1 == PramType)//д�����ַ���
    {
        //��һ���ֽ�д���ַ����ĳ���
        E2prom_WriteByte(Address,BufferLen);    
        Address++;
    }
    else//д����ʮ������ֵ
    {

    }

    //����д�뵽eeprom
    for(i=0; i<BufferLen; i++)
    {
        data = *(pBuffer+i);
        E2prom_WriteByte(Address,data);
        Address++;
    }

    //д��У���ֽ�
    E2prom_WriteByte(Address,VerifyByte);
    Address++;

    //**************������****************
    if(BufferLen > 49)//�ֽ������ڵ���30�ֽڵ�ֱ�ӷ���,��У��
    {
        return BufferLen;
    }
    else
    {
        //��ȡ��ַ
        Address -= (BufferLen+1);
        //����д������ݣ����Ⱥ�У���ֽڳ���
        E2prom_ReadByte(Address, Buffer, BufferLen);
        //����������ݵ�У���
        sum = 0;
        for(i=0; i<BufferLen; i++)
        {
            sum += Buffer[i];
        }
        //�Ƚ�У���
        if(sum == VerifyByte)
        {
            return BufferLen;
        }
        else
        {
            return 0;
        }
    }
}
/*********************************************************************
//��������  :EepromPram_ReadPram(u32 PramID, u8 *pBuffer)
//����      :��eeprom����
//����      :PramID:����ID;pBuffer:ָ�����ݣ��������ֽ���������������ֵ����ֽ���ǰ����ָ��;
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :����ֵ>0��ʾ��ȷ����������ֵ==0��ʾ������;
*********************************************************************/
u16 EepromPram_ReadPram(u32 PramID, u8 *pBuffer)
{
    u16 ID;
    u16 Address;
    u8  sum;
    u8  i;
    u8  Buffer[50];
    u8  length;
    /////////////////////////
    ID=E2ParamApp_ConvertPramIDToGroupIndex(PramID);
    if(0==ID)//����ʶ��ID
    {
        return 0;//����ʱ����
    }

    if(0 == EepromPramLength[ID])//����ID��Ϊ������
    {
        return 0;
    }
    else if(1 == EepromPramLength[ID])
    {
        //��ȡ��ַ
        Address = EepromPramAddress[ID];
        //����ֵ��У���ֽ�
        E2prom_ReadByte(Address, Buffer, 2);
        //����У���
        if(Buffer[0] == Buffer[1])
        {
            memcpy(pBuffer,Buffer,1);
            return 1;
        }
    }
    else if(2 == EepromPramLength[ID])
    {
        //��ȡ��ַ
        Address = EepromPramAddress[ID];
        //����ֵ��У���ֽ�
        E2prom_ReadByte(Address, Buffer, 3);
        sum = Buffer[0] + Buffer[1];
        //����У���
        if(sum == Buffer[2])
        {
            memcpy(pBuffer,Buffer,2);
            return 2;
        }
    }
    else if(4 == EepromPramLength[ID])//��ֵ
    {
        //��ȡ��ַ
        Address = EepromPramAddress[ID];
        //����ֵ��У���ֽ�
        E2prom_ReadByte(Address, Buffer, 5);
        //����У���
        sum = 0;
        for(i=0; i<4; i++)
        {
            sum += Buffer[i];
        }
        if(sum == Buffer[4])//У����ȷ
        {
            memcpy(pBuffer,Buffer,4);
            return 4;
        }
    }
    else//�ַ���
    {
        //��ȡ��ַ
        Address = EepromPramAddress[ID];
        //��ȡ����
        E2prom_ReadByte(Address, Buffer, 1);
        Address++;
        if((PramID == E2_ALL_DRIVER_INFORMATION_ID)
        ||(PramID == E2_UPDATA_URL_ID))//���ȳ���
        {
            length = Buffer[0];
            //�������ٶ�ȡ���ݺ�У���ֽ�
            E2prom_ReadByte(Address, pBuffer, length);
            return length;
        }
        else
        {
            length = Buffer[0];
            if(length > 49)
            {
                return 0;
            }
            //�������ٶ�ȡ���ݺ�У���ֽ�
            E2prom_ReadByte(Address, Buffer, length+1);
            //����У���
            sum = 0;
            for(i=0; i<length; i++)
            {
                sum += Buffer[i];
            }
            if(sum == Buffer[length])//У����ȷ
            {
                memcpy(pBuffer,Buffer,length);
                return length;
            }
        }
    }
    return 0;
}
/*********************************************************************
//��������  :EepromPram_UpdateVariable(u16 PramID)
//����      :����PramID����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ǧ��ע��:EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID)���������ã�ֻ����������E2_CAR_TOTAL_MILE_ID�����              
*********************************************************************/
void EepromPram_UpdateVariable(u32 PramID)
{
    u8  Buffer[30];
    u8  BufferLen;
    u32 temp;
          
                IWDG_ReloadCounter();//ι��//���õĲ����϶࣬�ҿ��ܻ�������������º���������ʱ����ܽϳ�������ι��
       
    switch(PramID)
    {
    case E2_TERMINAL_HEARTBEAT_ID:      
        {
            HeartBeat_UpdatePram();
            break;
        }//����ID0x0001���ն��������ͼ������λ�룬Ĭ��ֵ60
    case E2_TCP_ACK_OVERTIME_ID:        
        {
            break;
        }//����ID0x0002��TCP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    case E2_TCP_RESEND_TIMES_ID:            
        {
            break;
        }//����ID0x0003��TCP��Ϣ�ش�������Ĭ��ֵ3
    case E2_UDP_ACK_OVERTIME_ID:        
        {
            break;
        }//����ID0x0004��UDP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10
    case E2_UDP_RESEND_TIMES_ID:        
        {
            break;
        }//����ID0x0005��UDP��Ϣ�ش�������Ĭ��ֵ3
    case E2_SMS_ACK_OVERTIME_ID:        
        {
            break;
        }//����ID0x0006��SMS��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ30
    case E2_SMS_RESEND_TIMES_ID:    
        {
            break;
        }   //����ID0x0007��SMS��Ϣ�ش�������Ĭ��ֵ3
    case E2_MAIN_SERVER_APN_ID: 
                {
                        break;
                }//����ID0x0010����������APN
    //E2_MAIN_SERVER_DIAL_NAME_ID:          //����ID0x0011��������������ͨ�Ų����û���
    //E2_MAIN_SERVER_DIAL_PASSWORD_ID:      //����ID0x0012��������������ͨ�Ų�������
    case E2_MAIN_SERVER_IP_ID:             //����ID0x0013������������ַ��IP������
		case E2_BACKUP_SERVER_IP_ID:           //����ID0x0017�����ݷ�������ַ��IP������
		    {
					   Lock1_UpdataParameter();
					   Lock2_UpdataParameter();
		         break;
		    }
    //E2_BACKUP_SERVER_APN_ID:          //����ID0x0014�����ݷ�����APN������ͨ�Ų��ŷ��ʵ�
    //E2_BACKUP_SERVER_DIAL_NAME_ID:        //����ID0x0015�����ݷ���������ͨ�Ų����û���
    //E2_BACKUP_SERVER_DIAL_PASSWORD_ID:        //����ID0x0016�����ݷ���������ͨ�Ų�������
    //E2_MAIN_SERVER_TCP_PORT_ID:           //����ID0x0018����������TCP�˿�
    //E2_MAIN_SERVER_UDP_PORT_ID:           //����ID0x0019����������UDP�˿�
    //E2_BACKUP_SERVER_TCP_PORT_ID:     //����ID0x001a�����ݷ�����TCP�˿�
    //E2_BACKUP_SERVER_UDP_PORT_ID:     //����ID0x001b�����ݷ�����UDP�˿�
    //E2_REGISTER_SMS_CENTER_NUM_ID:        //����ID0x001C��ע��������ĺ���
    //E2_PRIVILEGE_SMS_CENTER_NUM_ID:       //����ID0x001D�����ƶ��ź��루��Ȩ��
    //0,0,                      //����ID0x001E~0x001f������
        
        case E2_IC_MAIN_SERVER_IP_ID:   
        {
            break;
        }//����ID0x001A,IC����֤��������IP��ַ������
        case E2_IC_MAIN_SERVER_TCP_PORT_ID: 
        {
            break;
        }//����ID0x001B,IC����֤��������TCP�˿�
        case E2_IC_MAIN_SERVER_UDP_PORT_ID: 
        {
            break;
        }//����ID0x001c,IC����֤��������UDP�˿�
        case E2_IC_BACKUP_SERVER_IP_ID: 
        {
            break;
        }//����ID0x001d,IC����֤���ݷ�����IP��ַ������ 
                
    case E2_POSITION_REPORT_STRATEGY_ID:    //����ID0x0020��λ�û㱨����
        {
            Report_UpdateOnePram(PRAM_REPORT_STRATEGY);
            break;
        }
    case E2_POSITION_REPORT_SCHEME_ID://����ID0x0021��λ�û㱨����
        {
            Report_UpdateOnePram(PRAM_REPORT_SCHEME);
            break;
        }
    case E2_DRIVER_UNLOGIN_REPORT_TIME_ID://0X0022 ����δ��½�㱨ʱ����
        {
            Report_UpdateOnePram(PRAM_UNLOGIN_REPORT_TIME);
            break;
        }
    case E2_SLEEP_REPORT_TIME_ID://0X0027 ����ACC OFF�㱨ʱ����
        {
            Report_UpdateOnePram(PRAM_ACC_OFF_REPORT_TIME);
            break;
        }
    case E2_EMERGENCY_REPORT_TIME_ID://0X0028 ���½�������ʱ�㱨ʱ����
        {
            Report_UpdateOnePram(PRAM_EMERGENCY_REPORT_TIME);
            break;
        }
    case E2_ACCON_REPORT_TIME_ID://0X0029 ����ACC ON�㱨ʱ����
        {
//            UpdataReportPram =1;  //fanqinghai 2015.11.04
            Report_UpdateOnePram(PRAM_ACC_ON_REPORT_TIME);
            break;
        }
    case E2_ACCON_REPORT_DISTANCE_ID://0X002C ����ACC ON�㱨������
        {
            Report_UpdateOnePram(PRAM_ACC_ON_REPORT_DISTANCE);
            break;
        }
    case E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ID://0X002D ����δ��½�㱨������
        {
            Report_UpdateOnePram(PRAM_UNLOGIN_REPORT_DISTANCE);
            break;
        }
    case E2_SLEEP_REPORT_DISTANCE_ID://0X002E ��������ʱ�㱨������
        {
            Report_UpdateOnePram(PRAM_ACC_OFF_REPORT_DISTANCE);
            break;
        }
    case E2_EMERGENCY_REPORT_DISTANCE_ID://0X002F ���½�������ʱ�㱨������
        {
            Report_UpdateOnePram(PRAM_EMERGENCY_REPORT_DISTANCE);
            break;
        }
    case E2_CORNER_REPORT_ID:   
        {
            Corner_UpdataPram();
            break;
        }//����ID0x0030���յ㲹���Ƕ�,��λ��/�룬Ĭ��ֵΪ15
    //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            //����ID0x0031~0x003f������
    //E2_MONITOR_SERVER_PHONE_ID:           //����ID0x0040�����ƽ̨�绰����
    //E2_TERMINAL_RESET_PHONE_ID:           //����ID0x0041���ն˸�λ�绰����
    //E2_TERMINAL_DEFAULT_SET_PHONE_ID:     //����ID0x0042���ָ��������õ绰����
    //E2_MONITOR_SERVER_SMS_PHONE_ID:       //����ID0x0043�����ƽ̨SMS�绰����
    //E2_SMS_TEXT_ALARM_PHONE_ID:           //����ID0x0044��SMS�ı������绰����
    //E2_TERMINAL_GET_PHONE_STRATEGY_ID:        //����ID0x0045���ն˵绰��������
    case E2_MAX_PHONE_TIME_EACH_ID:
        {
            #ifdef USE_PHONE
            Phone_UpdateMaxPhoneTimeEach();
            #endif
            break;
        }//����ID0x0046��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    case E2_MAX_PHONE_TIME_MONTH_ID:
        {
            #ifdef USE_PHONE
            Phone_UpdateMaxPhoneTimeMonth();
            #endif
            break;
        }//����ID0x0047��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������
    //E2_MONITOR_PHONE_ID:              //����ID0x0048�������绰����
    //E2_MONITOR_SERVER_PRIVILEGE_SMS_ID:       //����ID0x0049�����ƽ̨��Ȩ���ź���
    //0,0,0,0,0,0,                  //����ID0x004a~0x004f������
    case E2_ALARM_TEXT_SWITCH_ID:
        {
            break;
        }//����ID0x0051�����������ı�SMS���أ�Ĭ��Ϊȫ��
    case E2_ALARM_MASK_ID://����ID0x0050�����������֣�Ĭ��Ϊȫ��
    case E2_ALARM_PHOTO_SWITCH_ID://����ID0x0052���������㿪�أ�Ĭ��Ϊȫ��
    case E2_ALARM_PHOTO_STORE_FLAG_ID://����ID0x0053,��������洢��־//Ĭ��Ϊȫ��     
    case E2_STATUS_PHOTO_SWITCH_ID://����IDE2_PRAM_BASE_CUSTOM_ID+0x36��״̬�仯���㿪�أ�Ĭ��Ϊȫ��
    case E2_STATUS_PHOTO_STORE_FLAG_ID://����IDE2_PRAM_BASE_CUSTOM_ID+0x37,״̬�仯����洢��־//Ĭ��Ϊȫ��
    case E2_DEFINE_ALARM_MASK_WORD_ID://����ID���Զ��壬�Զ��屨�������֣�DWORD
        {
            Io_UpdataPram();
            Photo_UpdatePram();
            break;
        }
    case E2_KEY_FLAG_ID:    
        {
            break;
        }//����ID0x0054���ؼ���־
    case E2_MAX_SPEED_ID:
    case E2_OVER_SPEED_KEEP_TIME_ID:
        {
            VDROverspeed_UpdateParameter();
					  if(1 == BBXYTestFlag)
						{
					      VDROverspeed_ClearTime();
						}
            break;
        }//����ID0x0056�����ٳ���ʱ�䣬��λΪ�룬Ĭ��Ϊ10��
		case E2_MAX_DRVIE_TIME_ONEDAY_ID://����ID0x0058�������ۼƼ�ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ0   
    case E2_MIN_RELAX_TIME_ID://����ID0x0059����С��Ϣʱ�䣬��λΪ�룬Ĭ��Ϊ1200
        {
            VDROvertime_UpdateParameter();
					  
        }
		case E2_MAX_DRVIE_TIME_ID://����ID0x0057��������ʻʱ������//��λΪ��//Ĭ��Ϊ14400��(4Сʱ)
		    {
					  VDROvertime_UpdateParameter();
		        if(1 == BBXYTestFlag)//����Э����ʱ����ʱ�������øò���ʱ����������ʻ�ģ������Ҫ���¼�ʱ���ϱ�λ����Ϣ
						{
						    VDROvertime_ClearStartTime();
							  DelayAckCommand = 0x0200;
							  SetTimerTask(TIME_DELAY_TRIG, 3*SYSTICK_0p1SECOND);//������ʱ����
						}
		    }
    case E2_MAX_STOP_CAR_TIME_ID:           
        {
            VDROvertime_UpdateParameter();
            break;
        }//����ID0x005A���ͣ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0        
    case E2_SPEED_EARLY_ALARM_DVALUE_ID:    
        {
      VDROverspeed_UpdateParameter();
            break;
        }//����ID0x005B,���ٱ���Ԥ����ֵ����λΪ1/10Km/h Ĭ��Ϊ50 
    case E2_TIRE_EARLY_ALARM_DVALUE_ID: 
        {
      VDROvertime_UpdateParameter();
            break;
        }//����ID0x005C,ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0  Ĭ��Ϊ1800
    case E2_SET_CRASH_ALARM_ID: //����ID0x005D,��ײ������������
    case E2_SET_ROLLOVER_ALARM_ID:  
        {
					  BMA250E_UpdateRolloverParameter();
					  BMA250E_UpdateCrashParameter();
            break;
        }//����ID0x005E,�෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ30 ��             
        
        //0,0,0,0,0,                                      //����ID0x005F~0x0063������                 
        
        case E2_TIME_PHOTO_CONTROL_ID:  
        case E2_MILE_PHOTO_CONTROL_ID:  
        {
            Photo_UpdatePram();
            break;
        }//����ID0x0065,�������տ���

        //0,0,0,0,0,0,0,0,0,0,                          //����ID0x0066~0x006f������
    
        //E2_PICTURE_QUALITY_ID:            //����ID0x0070��ͼ��/��Ƶ������1~10,1Ϊ���
    //E2_PICTURE_BRIGHTNESS_ID:         //����ID0x0071�����ȣ�0~255
    //E2_PICTURE_CONTRAST_ID:       //����ID0x0072���Աȶȣ�0~127
    //E2_PICTURE_SATURATION_ID:         //����ID0x0073�����Ͷȣ�0~127
    //E2_PICTURE_CHROMA_ID:             //����ID0x0074��ɫ�ȣ�0~255
    //0,0,0,0,0,0,0,0,0,0,0,                //����ID0x0075~0x007f������
    case E2_CAR_TOTAL_MILE_ID:              
        {
            //��ȡ�ո����õ����
            EepromPram_ReadPram(E2_CAR_TOTAL_MILE_ID,Buffer);//0.1����Ϊ��λ
            temp = 0;
            temp |= Buffer[0] << 24;
            temp |= Buffer[1] << 16;
            temp |= Buffer[2] << 8;
            temp |= Buffer[3];
            //���ó�ʼ���
            GpsMile_SetPram(temp);
            
            break;
        }//����ID0x0080��������̱������0.1km,ǧ��ע��:EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID)���������ã�ֻ����������E2_CAR_TOTAL_MILE_ID�����                                             
    //E2_CAR_PROVINCE_ID:           //����ID0x0081���������ڵ�ʡ��ID
    ///E2_CAR_CITY_ID:              //����ID0x0082���������ڵ�����ID
    //E2_CAR_PLATE_NUM_ID:              //����ID0x0083��������ͨ�����Ű䷢�Ļ���������
    //E2_CAR_PLATE_COLOR_ID:            //����ID0x0084��������ɫ������JT/T415-2006��5.4.12
    
        case E2_GPS_SET_MODE_ID:    
        {
                        GnssCheckChangePar();
            
            break;
        }//����ID0x0090,GNSS ��λģʽ��Ĭ��Ϊ0x02 ������λ
        case E2_GPS_SET_BAUD_ID:    
        {
                        GnssCheckChangePar();
            break;
        }//����ID0x0091,GNSS �����ʣ�Ĭ��Ϊ0x01 9600
        case E2_GPS_SET_OUTPUT_RATE_ID: 
        {
                        GnssCheckChangePar();
            break;
        }//����ID0x0092,GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms
        case E2_GPS_SET_GATHER_RATE_ID: 
        {
                        GnssCheckChangePar();
            break;
        }//����ID0x0093,GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1
        case E2_GPS_SET_UPLOAD_MODE_ID: 
        {
                         GnssCheckChangePar();
            break;
        }//����ID0x0094,GNSS ģ����ϸ��λ�����ϴ���ʽ��Ĭ��Ϊ0x00�����ش洢���ϴ�
        case E2_GPS_SET_UPLOAD_VALUE_ID:    
        {
                         GnssCheckChangePar();
            break;
        }//����ID0x0095,GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0 
        case E2_CAN1_GATHER_TIME_ID://����ID0x0100,CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�     
        case E2_CAN1_UPLOAD_TIME_ID://����ID0x0101,CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�  
        case E2_CAN2_GATHER_TIME_ID://����ID0x0102,CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
        case E2_CAN2_UPLOAD_TIME_ID://����ID0x0103,CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�  
        case E2_CAN_SET_ONLY_GATHER_0_ID:   
        case E2_CAN_SET_ONLY_GATHER_1_ID:  
            {
              Can_UpdatePram();        
              break;
                }//����ID0x0111,CAN ����ID �����ɼ�����BYTE[8]                                                                          
                                                                                       
         //E2_LOGINGPRS_PASSWORD_ID:            //����IDE2_PRAM_BASE_CUSTOM_ID+0x00���Զ��壬��Ȩ��
    //E2_REGISTER_SMS_PHONE_ID:         //����IDE2_PRAM_BASE_CUSTOM_ID+0x01���Զ��壬ע�����Ϣ����
    case E2_PROGRAM_UPDATA_REQUEST_ID:
        {
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x02���Զ��壬�������������־
    //E2_PROGRAM_UPDATA_VERIFYCODE_ID:      //����IDE2_PRAM_BASE_CUSTOM_ID+0x03���Զ��壬��������У����
    //E2_PROGRAM_UPDATA_LENGTH_ID:      //����IDE2_PRAM_BASE_CUSTOM_ID+0x04���Զ��壬������������
    case E2_MANUFACTURE_ID:             
        {
            BufferLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, Buffer);
            if(0 == BufferLen)
            {
            
            }
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x05���Զ��壬������ID��5�ֽ�
    case E2_DEVICE_ID:
        {
            BufferLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer);
            if(0 == BufferLen)
            {
            
            }
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x06���Զ��壬�ն�ID��7�ֽ�
    case E2_DEVICE_PHONE_ID:    
        {
            //TerminalPhone_UpdataPram();
            break;
        }       
        //����IDE2_PRAM_BASE_CUSTOM_ID+0x07���Զ��壬�ն��ֻ��ţ�6�ֽ�
    //E2_CAR_IDENTIFICATION_CODE_ID:        //����IDE2_PRAM_BASE_CUSTOM_ID+0x08���Զ��壬����ʶ����
    //E2_CAR_TYPE_ID:               //����IDE2_PRAM_BASE_CUSTOM_ID+0x09���Զ��壬���Ʒ���
    //E2_CURRENT_DRIVER_ID:         //����IDE2_PRAM_BASE_CUSTOM_ID+0x0A���Զ��壬��ǰ��ʻԱ����
    ///E2_CURRENT_LICENSE_ID:           //����IDE2_PRAM_BASE_CUSTOM_ID+0x0B���Զ��壬��ǰ��ʻ֤����
    //E2_FIRMWARE_VERSION_ID:           //����IDE2_PRAM_BASE_CUSTOM_ID+0x0C���Զ��壬�̼��汾��
    //E2_ALL_DRIVER_INFORMATION_ID:     //����IDE2_PRAM_BASE_CUSTOM_ID+0x0D���Զ��壬���м�ʻԱ��Ϣ
    case E2_SPEED_SENSOR_COEF_ID:
        {
            //PulsePerKm_UpDatePram();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x0E���Զ��壬�ٶȴ�����ϵ��
    case E2_CAR_FEATURE_COEF_ID:
        {
            VDRPulse_UpdateParameter();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x0F���Զ��壬��������ϵ��
    case E2_SPEED_SELECT_ID:
        {
            SpeedFlagUpdatePram();
//          SpeedMonitor_UpdatePram(); dxl,2015.9,
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x10���Զ��壬�ٶ�ѡ�񿪹�
    case E2_BACKLIGHT_SELECT_ID:    
        {
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x11���Զ��壬����ѡ�񿪹�
    case E2_DOOR_SET_ID:    
        {
            Io_UpdataPram();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x29���Զ��壬����ѡ��0Ϊ���Ÿ���Ч��1Ϊ��������Ч
    case E2_CDMA_SLEEP_ID:
        {
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x2b���Զ��壬CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
    case E2_GPS_CONTROL_ID:
        {
            UpdataAccOffGpsControlFlag();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x30���Զ��壬ACC OFFʱGPSģ�����,0Ϊ��,1Ϊ��
    case E2_USB_UPDATA_FLAG_ID:
        {
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x31���Զ��壬USB������־
    case E2_TAXIMETER_CONTROL_ID:
        {
            UpdataTaximeterControlFlag();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x32���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
    case E2_TOPLIGHT_CONTROL_ID:
        {
            UpdataToplightControlFlag();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x33���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ����
    case E2_LEDLIGHT_CONTROL_ID:
        {
            UpdataLedlightControlFlag();
            break;
        }//����IDE2_PRAM_BASE_CUSTOM_ID+0x34���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ����
    case E2_UNLOGIN_TTS_PLAY_ID:
        {
//          ICCard_UpdataParamVoicePrompt(); dxl,2015.9,
					  UpdateRunMode();
            break;
        }
    case E2_LED_PORT_ID:
    case E2_LOAD_MAX_ID:
        {
//          CarLoad_UpdatePram(); dxl,2015.9,
                        UpdatePeripheralType();
            break;
        }
    case E2_POS_PORT_ID:
        {
//            OilWear_Updata16PINPortPeripheralParam(); 2015.9,
            UpdatePeripheralType();
            break;
        }
    case E2_SHUTDOWN_MODE_TIME_ID:
    case E2_SHUTDOWN_MODE_POWER_ID:
        {
            SleepDeep_UpdateParam();
            break;
        }        
    case E2_OVERSPEED_PREALARM_KEEPTIME_ID://����ID���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
    case E2_OVERSPEED_PREALARM_VOICE_ID://����ID���Զ��壬����Ԥ������,STRING,���30�ֽ�,Ĭ��ֵΪ����Ƴ���
    case E2_OVERSPEED_PREALARM_GPROUP_TIME_ID://����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    case E2_OVERSPEED_PREALARM_NUMBER_ID://����ID���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    case E2_OVERSPEED_PREALARM_NUMBER_TIME_ID://����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    //case  E2_OVERSPEED_ALARM_VALUE_ID://����ID���Զ��壬���ٱ���ֵ��DWORD,��λkm/h,Ĭ��ֵ130,���׼��0x0055�ظ���
    //case  E2_OVERSPEED_ALARM_KEEPTIME_ID://����ID���Զ��壬���ٱ�������ʱ�䣬DWORD,��λ��Ĭ��ֵ10,���׼��0x0056�ظ���
    case E2_OVERSPEED_ALARM_VOICE_ID://����ID���Զ��壬���ٱ�������,STRING,���30�ֽ�,
    case E2_OVERSPEED_ALARM_GPROUP_TIME_ID://����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    case E2_OVERSPEED_ALARM_NUMBER_ID://����ID���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
    case E2_OVERSPEED_ALARM_NUMBER_TIME_ID://����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
    case E2_OVERSPEED_ALARM_REPORT_TIME_ID://����ID���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0����һ��ǰ������ˣ��б��ģ����Ҫ�õ�
    case E2_OVERSPEED_NIGHT_TIME_ID://����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬��ʹ��
    case E2_NIGHT_OVERSPEED_PERCENT_ID://����ID���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
    case E2_NIGHT_DRIVE_PREALARM_TIME_ID://����ID���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
    case E2_NIGHT_DRIVE_PREALARM_VOICE_ID://����ID���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�,Ĭ��ֵ"��������ҹ��ʱ��"
    case E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID://����ID���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
    case E2_NIGHT_DRIVE_PREALARM_NUMBER_ID://����ID���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
    case E2_FORBID_DRIVE_NIGHT_TIME_ID://����ID���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    case E2_FORBID_DRIVE_PREALARM_TIME_ID://����ID���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
    case E2_FORBID_DRIVE_PREALARM_VOICE_ID://����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
    case E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID://����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    case E2_FORBID_DRIVE_PREALARM_NUMBER_ID://����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    case E2_FORBID_DRIVE_ALARM_VOICE_ID://����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
    case E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID://����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    case E2_FORBID_DRIVE_ALARM_NUMBER_ID://����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    case E2_FORBID_DRIVE_RUN_SPEED_ID://����ID���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
    case E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID://����ID���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
    case E2_REPORT_FREQ_EVENT_SWITCH_ID://����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,BIT1λ��1����ʱ����,DWORD
        {
            VDROverspeed_UpdateParameter();
					  VDROvertime_UpdateParameter();
            break;
        }           //���ٶȴ��ڽ���ʱ����ʻ�ٶ��ҳ���ʱ��ﵽ����ʱ����ʻ�ٶȳ���ʱ�䣬�϶�Ϊ��ʻ
    case E2_TIRED_DRIVE_NIGHT_TIME_ID://����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
    case E2_NIGHT_MAX_DRVIE_TIME_ID://����ID���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
    case E2_NIGHT_MIN_RELAX_TIME_ID://����ID���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
    case E2_TIRED_DRIVE_ALARM_TIME_ID://����ID���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
    case E2_TIRED_DRIVE_PREALARM_VOICE_ID://����ID���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    case E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID://����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    case E2_TIRED_DRIVE_PREALARM_NUMBER_ID://����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    case E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID://����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    case E2_TIRED_DRIVE_ALARM_VOICE_ID://����ID���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    case E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID://����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    case E2_TIRED_DRIVE_ALARM_NUMBER_ID://����ID���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
    case E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID://����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    case E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID://����ID���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
    case E2_TIRED_DRIVE_REPORT_TIME_ID://����ID���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
    case E2_TIRED_DRIVE_RUN_SPEED_ID://����ID���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
    case E2_TIRED_DRIVE_RUN_KEEPTIME_ID://����ID���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
        {
//          TiredDrive_UpdatePram(); dxl,2015.9,
            break;
        }
    case E2_ROUTE_EXCURSION_ALARM_NUMBER_ID://����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
    case E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID://����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
    case E2_ROUTE_EXCURSION_ALARM_VOICE_ID://����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
        {
            Route_UpdatePram();
            break;
        }
        case E2_SECOND_ACC_OFF_REPORT_TIME_ID:
                {
                        Report_UpdateOnePram(PRAM_SECOND_LINK_ACC_OFF_REPORT_TIME);
                        break;
                }
        case E2_SECOND_ACC_ON_REPORT_TIME_ID:
                {
                        Report_UpdateOnePram(PRAM_SECOND_LINK_ACC_ON_REPORT_TIME);
                        break;
                        
                }
        case E2_OBD_SWITCH_ID:
                {
                        Report_UpdateOnePram(PRAM_SECOND_LINK_ACC_ON_REPORT_TIME);
                        break;
                        
                }
				case E2_LOCK1_ENABLE_FLAG_ID:
                {
                        Lock1_UpdataParameter();
                        break;
                        
                }
				case E2_LOCK2_ENABLE_FLAG_ID:
                {
                        Lock2_UpdataParameter();
                        break;
                        
                }
						
                
        default:    break;
    }
}
/*********************************************************************
//��������  :EepromPram_DefaultSet(void)
//����      :ϵͳ��1������ʱ��Ĭ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ʹ�ûָ�Ĭ�ϳ�������Ҳ�Ὣ��������Ϊ������ʾ
*********************************************************************/
void EepromPram_DefaultSet(void)
{
    u32 temp;
    u8  Buffer[31];
    u8  BufferLen;
    u8  i;

    IWDG_ReloadCounter();//ι��//���õĲ����϶࣬�ҿ��ܻ�������������º���������ʱ����ܽϳ�������ι��
    //0x0001    �ն��������ͼ��//��λ��//Ĭ��ֵ60
    temp = 60;
    //temp = 180;//�б���ʱ���ó�Щ������Ӱ��������⹦��
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TERMINAL_HEARTBEAT_ID, Buffer, E2_TERMINAL_HEARTBEAT_LEN))
    {
        EepromPram_WritePram(E2_TERMINAL_HEARTBEAT_ID, Buffer, E2_TERMINAL_HEARTBEAT_LEN);
    }
    //0x0002    TCP��ϢӦ��ʱʱ��//��λ��//Ĭ��ֵ10
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 5;
    #else
    temp = 10;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TCP_ACK_OVERTIME_ID, Buffer, E2_TCP_ACK_OVERTIME_LEN))
    {
        EepromPram_WritePram(E2_TCP_ACK_OVERTIME_ID, Buffer, E2_TCP_ACK_OVERTIME_LEN);
    }
    //0x0003    TCP��Ϣ�ش�����//Ĭ��ֵ3
    temp = 3; 
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TCP_RESEND_TIMES_ID, Buffer, E2_TCP_RESEND_TIMES_LEN))
    {
        EepromPram_WritePram(E2_TCP_RESEND_TIMES_ID, Buffer, E2_TCP_RESEND_TIMES_LEN);
    }
    //0x0004    UDP��ϢӦ��ʱʱ��//��λ��//Ĭ��ֵ10
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 5;
    #else
    temp = 10;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_UDP_ACK_OVERTIME_ID, Buffer, E2_UDP_ACK_OVERTIME_LEN))
    {
        EepromPram_WritePram(E2_UDP_ACK_OVERTIME_ID, Buffer, E2_UDP_ACK_OVERTIME_LEN);
    }
    //0x0005    UDP��Ϣ�ش�����//Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_UDP_RESEND_TIMES_ID, Buffer, E2_UDP_RESEND_TIMES_LEN))
    {
        EepromPram_WritePram(E2_UDP_RESEND_TIMES_ID, Buffer, E2_UDP_RESEND_TIMES_LEN);
    }
    //0x0006    SMS��ϢӦ��ʱʱ��//��λ��//Ĭ��ֵ30
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 5;
    #else
    temp = 30;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_SMS_ACK_OVERTIME_ID, Buffer, E2_SMS_ACK_OVERTIME_LEN))
    {
        EepromPram_WritePram(E2_SMS_ACK_OVERTIME_ID, Buffer, E2_SMS_ACK_OVERTIME_LEN);
    }
    //0x0007    SMS��Ϣ�ش�����//Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_SMS_RESEND_TIMES_ID, Buffer, E2_SMS_RESEND_TIMES_LEN))
    {
        EepromPram_WritePram(E2_SMS_RESEND_TIMES_ID, Buffer, E2_SMS_RESEND_TIMES_LEN);
    }

    #ifdef HUOYUN_DEBUG_OPEN
    //0x0010       ��������APN
    strcpy((char *)Buffer,"cmnet");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, Buffer, BufferLen);
    }
    //0x0011  ������������ͨ�Ų����û���
    strcpy((char *)Buffer,"card");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_MAIN_SERVER_DIAL_NAME_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_MAIN_SERVER_DIAL_NAME_ID, Buffer, BufferLen);
    }
    //0x0012  ������������ͨ�Ų�������
    strcpy((char *)Buffer,"card");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen);
    }

    //0x0014 ���ݷ�����APN
    strcpy((char *)Buffer,"cmnet");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, Buffer, BufferLen);
    }
    //0x0015 ���ݷ���������ͨ�Ų����û���
    strcpy((char *)Buffer,"card");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_NAME_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_NAME_ID, Buffer, BufferLen);
    }
    //0x0016 ���ݷ���������ͨ�Ų�������
    strcpy((char *)Buffer,"card");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen);
    }    
    #endif
    //0x0020    λ�û㱨����,0Ϊ��ʱ,1Ϊ����,2Ϊ��ʱ����,Ĭ��Ϊ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_POSITION_REPORT_STRATEGY_ID, Buffer, E2_POSITION_REPORT_STRATEGY_LEN))
    {
        EepromPram_WritePram(E2_POSITION_REPORT_STRATEGY_ID, Buffer, E2_POSITION_REPORT_STRATEGY_LEN);
    }
    //0x0021    λ�û㱨����,0����ACC״̬,1���ݵ�¼��ACC״̬,Ĭ��Ϊ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_POSITION_REPORT_SCHEME_ID, Buffer, E2_POSITION_REPORT_SCHEME_LEN))
    {
        EepromPram_WritePram(E2_POSITION_REPORT_SCHEME_ID, Buffer, E2_POSITION_REPORT_SCHEME_LEN);
    }

    //0x0022    ��ʻԱδ��¼�㱨ʱ����//��λΪ��//Ĭ��ֵΪ60
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 30;
    #else
    temp = 60;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_DRIVER_UNLOGIN_REPORT_TIME_ID, Buffer, E2_DRIVER_UNLOGIN_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_DRIVER_UNLOGIN_REPORT_TIME_ID, Buffer, E2_DRIVER_UNLOGIN_REPORT_TIME_LEN);
    }

    //0x0027    ����ʱ�㱨ʱ����//��λΪ��//Ĭ��ֵΪ240
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 300;
    #else
    temp = 240;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN);
    }

    //0x0028    ��������ʱ�㱨ʱ����//��λΪ��//Ĭ��ֵΪ10
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_EMERGENCY_REPORT_TIME_ID, Buffer, E2_EMERGENCY_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_EMERGENCY_REPORT_TIME_ID, Buffer, E2_EMERGENCY_REPORT_TIME_LEN);
    }

    //0x0029    ȱʡʱ��㱨���,��λΪ��,Ĭ��Ϊ30
    temp = 30;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID, Buffer, E2_ACCON_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID, Buffer, E2_ACCON_REPORT_TIME_LEN);
    }

    //0x002C    ȱʡ����㱨���,��λΪ��,Ĭ��Ϊ500
    temp = 500;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ACCON_REPORT_DISTANCE_ID, Buffer, E2_ACCON_REPORT_DISTANCE_LEN))
    {
        EepromPram_WritePram(E2_ACCON_REPORT_DISTANCE_ID, Buffer, E2_ACCON_REPORT_DISTANCE_LEN);
    }

    //0x002D    ��ʻԱδ��¼�㱨������//��λΪ��//Ĭ��ֵΪ500
    temp = 500;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ID, Buffer, E2_DRIVER_UNLOGIN_REPORT_DISTANCE_LEN))
    {
        EepromPram_WritePram(E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ID, Buffer, E2_DRIVER_UNLOGIN_REPORT_DISTANCE_LEN);
    }
    //0x002e    ����ʱ�㱨������//��λΪ��//Ĭ��ֵΪ200
    #ifdef  HUOYUN_DEBUG_OPEN//modify by joneming
    temp = 500;
    #else
    temp = 200;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_SLEEP_REPORT_DISTANCE_ID, Buffer, E2_SLEEP_REPORT_DISTANCE_LEN))
    {
        EepromPram_WritePram(E2_SLEEP_REPORT_DISTANCE_ID, Buffer, E2_SLEEP_REPORT_DISTANCE_LEN);
    }
    //0x002f    ��������ʱ�㱨������//��λΪ��//Ĭ��ֵΪ200
    temp = 200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_EMERGENCY_REPORT_DISTANCE_ID, Buffer, E2_EMERGENCY_REPORT_DISTANCE_LEN))
    {
        EepromPram_WritePram(E2_EMERGENCY_REPORT_DISTANCE_ID, Buffer, E2_EMERGENCY_REPORT_DISTANCE_LEN);
    }
    //0x0030    �յ㲹���Ƕ�,��λ��/��//Ĭ��ֵΪ30
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 45;
    #else
    temp = 30;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CORNER_REPORT_ID, Buffer, E2_CORNER_REPORT_LEN))
    {
        EepromPram_WritePram(E2_CORNER_REPORT_ID, Buffer, E2_CORNER_REPORT_LEN);
    }

    //0x0031    ����Χ���뾶,��λ��,Ĭ��ֵ500
    temp = 500;//modify by joneming
    Public_ConvertShortToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ELECTRON_RADIUS_ID, Buffer, E2_ELECTRON_RADIUS_LEN))
    {
        EepromPram_WritePram(E2_ELECTRON_RADIUS_ID, Buffer, E2_ELECTRON_RADIUS_LEN);
    }


    //*************ͨ�����ܲ���**********************
    //0x0045    �ն˵绰��������,0:�Զ�����;1:�ֶ�����,Ĭ��Ϊ0
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 1;
    #else
    temp = 0;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TERMINAL_GET_PHONE_STRATEGY_ID, Buffer, E2_TERMINAL_GET_PHONE_STRATEGY_LEN))
    {
        EepromPram_WritePram(E2_TERMINAL_GET_PHONE_STRATEGY_ID, Buffer, E2_TERMINAL_GET_PHONE_STRATEGY_LEN);
    }
    //0x0046    ÿ���ͨ��ʱ��//��λΪ��//Ĭ��Ϊ0,�رոù���
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 120;
    #else
    temp = 0;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_PHONE_TIME_EACH_ID, Buffer, E2_MAX_PHONE_TIME_EACH_LEN))
    {
        EepromPram_WritePram(E2_MAX_PHONE_TIME_EACH_ID, Buffer, E2_MAX_PHONE_TIME_EACH_LEN);
    }
    //0x0047    ÿ���ͨ��ʱ��//��λΪ��//Ĭ��Ϊ0,�رոù���
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 36000;
    #else
    temp = 0;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_PHONE_TIME_MONTH_ID, Buffer, E2_MAX_PHONE_TIME_MONTH_LEN))
    {
        EepromPram_WritePram(E2_MAX_PHONE_TIME_MONTH_ID, Buffer, E2_MAX_PHONE_TIME_MONTH_LEN);
    }
    ///////////////
    IWDG_ReloadCounter();//ι��//���õĲ����϶࣬�ҿ��ܻ�������������º���������ʱ����ܽϳ�������ι��
    //0x0050    ����������//bit9,11,12Ĭ������,LCD������,����ͷ����,IC��ģ�����,VSS�����쳣Ĭ��Ϊ����
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 0;//����ƽ̨Ҫ��Ϊ0
    #else
    temp = 0x01001a00;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ALARM_MASK_ID, Buffer, E2_ALARM_MASK_LEN))
    {
        EepromPram_WritePram(E2_ALARM_MASK_ID, Buffer, E2_ALARM_MASK_LEN);
    }
    //0x0051    ���������ı�SMS����//��ӦλΪ1��ʾ�����������跢�ͱ�������,Ĭ��Ϊ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ALARM_TEXT_SWITCH_ID, Buffer, E2_ALARM_TEXT_SWITCH_LEN))
    {
        EepromPram_WritePram(E2_ALARM_TEXT_SWITCH_ID, Buffer, E2_ALARM_TEXT_SWITCH_LEN);
    }
    //0x0052    �������㿪��//��ӦλΪ1��ʾ�����������败������,Ĭ��Ϊ1,����������������
    temp = 1;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ALARM_PHOTO_SWITCH_ID, Buffer, E2_ALARM_PHOTO_SWITCH_LEN))
    {
        EepromPram_WritePram(E2_ALARM_PHOTO_SWITCH_ID, Buffer, E2_ALARM_PHOTO_SWITCH_LEN);
    }
    //0x0053    ��������洢��־//��ӦλΪ1��ʾ�洢,0��ʾ�ϴ�,Ĭ��Ϊ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ALARM_PHOTO_STORE_FLAG_ID, Buffer, E2_ALARM_PHOTO_STORE_FLAG_LEN))
    {
        EepromPram_WritePram(E2_ALARM_PHOTO_STORE_FLAG_ID, Buffer, E2_ALARM_PHOTO_STORE_FLAG_LEN);
    }
    //0x0054    �ؼ���־//��ӦλΪ1��ʾ�ñ���Ϊ�ؼ�����,�ǹؼ��������ܱ��������ֵ�����,Ĭ��Ϊ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_KEY_FLAG_ID, Buffer, E2_KEY_FLAG_LEN))
    {
        EepromPram_WritePram(E2_KEY_FLAG_ID, Buffer, E2_KEY_FLAG_LEN);
    }

    //0x0055    ����ٶ�,��λkm/h,DWORD,Ĭ��ֵ130,��ͨ����Ϊ100km/h
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 100;
    #else
    temp = 100;//dxl,2015.5.27,֮ǰΪ130���ڸ�Ϊ100���ͨһֱ
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_SPEED_ID, Buffer, E2_MAX_SPEED_LEN))
    {
        EepromPram_WritePram(E2_MAX_SPEED_ID, Buffer, E2_MAX_SPEED_LEN);
    }


    //0x0056    ���ٳ���ʱ��,��λΪ��,DWORD,Ĭ��Ϊ10��,��ͨҪ�������ϱ�
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 5;
    #else
    temp = 10;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer, E2_OVER_SPEED_KEEP_TIME_LEN))
    {
        EepromPram_WritePram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer, E2_OVER_SPEED_KEEP_TIME_LEN);
    }
    //0x0057    ������ʻʱ������//��λΪ��//Ĭ��Ϊ14400��(4Сʱ)
    temp = 14400;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_DRVIE_TIME_ID, Buffer, E2_MAX_DRVIE_TIME_LEN))
    {
        EepromPram_WritePram(E2_MAX_DRVIE_TIME_ID, Buffer, E2_MAX_DRVIE_TIME_LEN);
    }
    //0x0058    �����ۼƼ�ʻʱ������//��λΪ��//Ĭ��Ϊ0xffffffff,�������ù���
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 57600;
    #else
    temp = 0xffffffff;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_DRVIE_TIME_ONEDAY_ID, Buffer, E2_MAX_DRVIE_TIME_ONEDAY_LEN))
    {
        EepromPram_WritePram(E2_MAX_DRVIE_TIME_ONEDAY_ID, Buffer, E2_MAX_DRVIE_TIME_ONEDAY_LEN);
    }
    //0x0059    ��С��Ϣʱ��//��λΪ��//Ĭ��Ϊ1200��20���ӣ�
    temp = 1200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MIN_RELAX_TIME_ID, Buffer, E2_MIN_RELAX_TIME_LEN))
    {
        EepromPram_WritePram(E2_MIN_RELAX_TIME_ID, Buffer, E2_MIN_RELAX_TIME_LEN);
    }
    //0x005A    �ͣ��ʱ��//��λΪ��//Ĭ��Ϊ0xffffffff,�������ù���
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 3600;
    #else
    temp = 0xffffffff;
    #endif
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_STOP_CAR_TIME_ID, Buffer, E2_MAX_STOP_CAR_TIME_LEN))
    {
        EepromPram_WritePram(E2_MAX_STOP_CAR_TIME_ID, Buffer, E2_MAX_STOP_CAR_TIME_LEN);
    }
    //0x005B    ���ٱ���Ԥ����ֵ����λΪ1/10Km/h,DWORD,Ĭ��Ϊ50��5km/h��
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 100;
    #else
    temp = 50;
    #endif
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer, E2_SPEED_EARLY_ALARM_DVALUE_LEN))
    {
        EepromPram_WritePram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer, E2_SPEED_EARLY_ALARM_DVALUE_LEN);
    }
    //0x005C    ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0  Ĭ��Ϊ1800�루30���ӣ�
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 1200;
    #else
    temp = 1800;
    #endif
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer, E2_TIRE_EARLY_ALARM_DVALUE_LEN))
    {
        EepromPram_WritePram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer, E2_TIRE_EARLY_ALARM_DVALUE_LEN);
    }
    //0x005D    ��ײ������������,��ײʱ��Ĭ��Ϊ4ms,��ײ���ٶ�Ĭ������Ϊ4g
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 17924;
    #else
    temp = 0x2801;//4g,4ms
    #endif
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_SET_CRASH_ALARM_ID, Buffer, E2_SET_CRASH_ALARM_LEN))
    {
        EepromPram_WritePram(E2_SET_CRASH_ALARM_ID, Buffer, E2_SET_CRASH_ALARM_LEN);
    }

    //0x005E    �෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ45 ��
    #ifdef  HUOYUN_DEBUG_OPEN
    temp = 30;
    #else
    temp = 45;//45��,V1.01.01Ĭ��Ϊ60,V1.01.02Ĭ��Ϊ60,V1.01.03Ĭ��Ϊ30,V1.01.04��Ϊ45
    #endif
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_SET_ROLLOVER_ALARM_ID, Buffer, E2_SET_ROLLOVER_ALARM_LEN))
    {
        EepromPram_WritePram(E2_SET_ROLLOVER_ALARM_ID, Buffer, E2_SET_ROLLOVER_ALARM_LEN);
    }

    //0x0064    ��ʱ���տ���,Ĭ��Ϊ������,ֵΪ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIME_PHOTO_CONTROL_ID, Buffer, E2_TIME_PHOTO_CONTROL_LEN))
    {
        EepromPram_WritePram(E2_TIME_PHOTO_CONTROL_ID, Buffer, E2_TIME_PHOTO_CONTROL_LEN);
    }
    //0x0065    �������տ���,Ĭ��Ϊ������,ֵΪ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MILE_PHOTO_CONTROL_ID, Buffer, E2_MILE_PHOTO_CONTROL_LEN))
    {
        EepromPram_WritePram(E2_MILE_PHOTO_CONTROL_ID, Buffer, E2_MILE_PHOTO_CONTROL_LEN);
    }

    //0x0080,�ۼ���ʻ�����0,������̱����, ��λ0.1km
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, Buffer, E2_CAR_TOTAL_MILE_LEN))
    {
        EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, Buffer, E2_CAR_TOTAL_MILE_LEN);
    }
    GpsMile_SetPram(0);//�����ۼ����Ϊ0
    //Pulse_UpdataPram();//���������ۼ����,�����������ѷ�����GpsMile_SetPram����
    //GpsMile_UpdatePram();//����GPS�ۼ����

    //0x0090  GNSS ��λģʽ��Ĭ��Ϊ0x03 ������λ+GPS��ȫ������ƽ̨��������
    temp = 3;
    Buffer[0] = temp;//modify by joneming
    if(0 == EepromPram_WritePram(E2_GPS_SET_MODE_ID, Buffer, E2_GPS_SET_MODE_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_MODE_ID, Buffer, E2_GPS_SET_MODE_LEN);
    }
    //0x0091  GNSS ���������ã�Ĭ��Ϊ0x02 19200��ȫ������ƽ̨��������
    temp = 2;
    Buffer[0] = temp;//modify by joneming
    if(0 == EepromPram_WritePram(E2_GPS_SET_BAUD_ID, Buffer, E2_GPS_SET_BAUD_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_BAUD_ID, Buffer, E2_GPS_SET_BAUD_LEN);
    }       
    //0x0092  GNSS ģ����ϸ��λ�������Ƶ�ʣ�1�룬ȫ������ƽ̨��������
    temp = 1;
    Buffer[0] = temp;//modify by joneming
    if(0 == EepromPram_WritePram(E2_GPS_SET_OUTPUT_RATE_ID, Buffer, E2_GPS_SET_OUTPUT_RATE_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_OUTPUT_RATE_ID, Buffer, E2_GPS_SET_OUTPUT_RATE_LEN);
    }   

    //0x0093  GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ�1��1����ȫ������ƽ̨��������
    temp = 1;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_GPS_SET_GATHER_RATE_ID, Buffer, E2_GPS_SET_GATHER_RATE_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_GATHER_RATE_ID, Buffer, E2_GPS_SET_GATHER_RATE_LEN);
    }   

    //0x0094  GNSS ģ����ϸ��λ�����ϴ���ʽ��0��ȫ������ƽ̨��������
    temp = 0;
    Buffer[0] = temp;//modify by joneming
    if(0 == EepromPram_WritePram(E2_GPS_SET_UPLOAD_MODE_ID, Buffer, E2_GPS_SET_UPLOAD_MODE_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_UPLOAD_MODE_ID, Buffer, E2_GPS_SET_UPLOAD_MODE_LEN);
    }   

    //0x0095  GNSS ģ����ϸ��λ�����ϴ����ã�0��ȫ������ƽ̨��������
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_GPS_SET_UPLOAD_VALUE_ID, Buffer, E2_GPS_SET_UPLOAD_VALUE_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_UPLOAD_VALUE_ID, Buffer, E2_GPS_SET_UPLOAD_VALUE_LEN);
    }

    //0x0100  CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ� ��ȫ������ƽ̨��������
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CAN1_GATHER_TIME_ID, Buffer, E2_CAN1_GATHER_TIME_LEN))
    {
        EepromPram_WritePram(E2_CAN1_GATHER_TIME_ID, Buffer, E2_CAN1_GATHER_TIME_LEN);
    }

    //0x0101  CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ���ȫ������ƽ̨��������
    temp = 0;
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_CAN1_UPLOAD_TIME_ID, Buffer, E2_CAN1_UPLOAD_TIME_LEN))
    {
        EepromPram_WritePram(E2_CAN1_UPLOAD_TIME_ID, Buffer, E2_CAN1_UPLOAD_TIME_LEN);
    }

    //0x0102  CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ���ȫ������ƽ̨��������
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CAN2_GATHER_TIME_ID, Buffer, E2_CAN2_GATHER_TIME_LEN))
    {
        EepromPram_WritePram(E2_CAN2_GATHER_TIME_ID, Buffer, E2_CAN2_GATHER_TIME_LEN);
    }

    //0x0103  CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ���ȫ������ƽ̨��������
    temp = 0;
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_CAN2_UPLOAD_TIME_ID, Buffer, E2_CAN2_UPLOAD_TIME_LEN))
    {
        EepromPram_WritePram(E2_CAN2_UPLOAD_TIME_ID, Buffer, E2_CAN2_UPLOAD_TIME_LEN);
    }
    //0x0110  CAN ����ID �����ɼ�����BYTE[8]��0��ʾ���ɼ���ȫ������ƽ̨��������
    //temp = 0;    
    //Public_ConvertLongToBuffer(temp, Buffer);
    memset(Buffer,0,8);//modify by joneming
    if(0 == EepromPram_WritePram(E2_CAN_SET_ONLY_GATHER_0_ID, Buffer, E2_CAN_SET_ONLY_GATHER_LEN))
    {
        EepromPram_WritePram(E2_CAN_SET_ONLY_GATHER_0_ID, Buffer, E2_CAN_SET_ONLY_GATHER_LEN);
    }


    //������ID,Ĭ��Ϊ70108
    strcpy((char *)Buffer,"70108");
    if(0 == EepromPram_WritePram(E2_MANUFACTURE_ID, Buffer, E2_MANUFACTURE_LEN))
    {
        EepromPram_WritePram(E2_MANUFACTURE_ID, Buffer, E2_MANUFACTURE_LEN);
    }

    //����ϵ��,Ĭ��Ϊ7200,dxl,2015.4.10,֮ǰ��3600
    temp =7200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &Buffer[1], E2_CAR_FEATURE_COEF_LEN))
    {
        EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &Buffer[1], E2_CAR_FEATURE_COEF_LEN);
    }

    //�ٶ�ѡ�񿪹�,Ĭ��ΪGPS�ٶ�,0x01Ϊ�ֶ�GPS,0x00Ϊ�ֶ�����,0x02Ϊ�Զ�����,0x03Ϊ�Զ�GPS
    Buffer[0] = 0x03;//dxl,2015.5.11,Ĭ��Ϊ0x03,֮ǰΪ0x01
    if(0 == EepromPram_WritePram(E2_SPEED_SELECT_ID, Buffer, E2_SPEED_SELECT_LEN))
    {
        EepromPram_WritePram(E2_SPEED_SELECT_ID, Buffer, E2_SPEED_SELECT_LEN);
    }

    //Ĭ��ѡ���Ÿ�
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_DOOR_SET_ID, Buffer, E2_DOOR_SET_LEN))
    {
        EepromPram_WritePram(E2_DOOR_SET_ID, Buffer, E2_DOOR_SET_LEN);
    }
    //�ָ�Ĭ�ϳ�ʼ������ʱ�������ʼ��װ����д��־
    Buffer[0] = 0;
    if(0 == EepromPram_WritePram(E2_INSTALL_TIME_FLAG_ID, Buffer, E2_INSTALL_TIME_FLAG_LEN))
    {
        EepromPram_WritePram(E2_INSTALL_TIME_FLAG_ID, Buffer, E2_INSTALL_TIME_FLAG_LEN);
    }

    //ACC OFFʱGPSģ��Ĭ��Ϊ�� 
    Buffer[0] = 0;
    if(0 == EepromPram_WritePram(E2_GPS_CONTROL_ID, Buffer, E2_GPS_CONTROL_LEN))
    {
        EepromPram_WritePram(E2_GPS_CONTROL_ID, Buffer, E2_GPS_CONTROL_LEN);
    }

    //�Ƽ�������,Ĭ��Ϊ��
    Buffer[0] = 0;
    if(0 == EepromPram_WritePram(E2_TAXIMETER_CONTROL_ID, Buffer, E2_TAXIMETER_CONTROL_LEN))
    {
        EepromPram_WritePram(E2_TAXIMETER_CONTROL_ID, Buffer, E2_TAXIMETER_CONTROL_LEN);
    }

    IWDG_ReloadCounter();//ι��//���õĲ����϶࣬�ҿ��ܻ�������������º���������ʱ����ܽϳ�������ι��



    //*****************�Զ������********************
    Buffer[0] = 1;//dxl,2015.5.27,Ĭ�ϱ��ⳣ�����ͷ�Ҫ��
    if(0 == EepromPram_WritePram(E2_BACKLIGHT_SELECT_ID, Buffer,E2_BACKLIGHT_SELECT_ID_LEN))
    {
        EepromPram_WritePram(E2_BACKLIGHT_SELECT_ID, Buffer,E2_BACKLIGHT_SELECT_ID_LEN);
    }
    
    //E2_PRAM_BASE_CUSTOM_ID+0x39,���Զ��壬ҹ��ʱ�䷶Χ,BCD��,�µ�EGS701��1��ʹ��,�����ʹ���������
    for(i=0; i<4; i++)//�б��1���ʹ�øò���
    {
        Buffer[i] = 0;
    }
    if(0 == EepromPram_WritePram(E2_NIGHT_TIME0_ID, Buffer, E2_NIGHT_TIME0_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_TIME0_ID, Buffer, E2_NIGHT_TIME0_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x3a���Զ��壬ҹ�䳬ʱ��ʻ����,0��ʾ������ҹ��ģʽ,�µ�EGS701��1��ʹ��,�����ʹ���������
    temp = 7200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_MAX_DRVIE_TIME0_ID, Buffer, E2_NIGHT_MAX_DRVIE_TIME0_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_MAX_DRVIE_TIME0_ID, Buffer, E2_NIGHT_MAX_DRVIE_TIME0_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x3b���Զ��壬ҹ������ֵ,�µ�EGS701��1��ʹ��,�����ʹ���������
    temp = 255;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_MAX_SPEED0_ID, Buffer, E2_NIGHT_MAX_SPEED0_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_MAX_SPEED0_ID, Buffer, E2_NIGHT_MAX_SPEED0_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+4c ��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ��
    Buffer[0] = 0;
    if(0 == EepromPram_WritePram(E2_UNLOGIN_TTS_PLAY_ID, Buffer, E2_UNLOGIN_TTS_PLAY_ID_LEN))
    {
        EepromPram_WritePram(E2_UNLOGIN_TTS_PLAY_ID, Buffer, E2_UNLOGIN_TTS_PLAY_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+50���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_PREALARM_KEEPTIME_ID, Buffer, E2_OVERSPEED_PREALARM_KEEPTIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_PREALARM_KEEPTIME_ID, Buffer, E2_OVERSPEED_PREALARM_KEEPTIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x51���Զ��壬����Ԥ������,STRING,���30�ֽ�,Ĭ��ֵΪ����Ƴ���
    strcpy((char *)Buffer,"����Ƴ���");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_PREALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_OVERSPEED_PREALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x52���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_PREALARM_GPROUP_TIME_ID, Buffer, E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_PREALARM_GPROUP_TIME_ID, Buffer, E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x53���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3 
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_PREALARM_NUMBER_ID, Buffer, E2_OVERSPEED_PREALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_PREALARM_NUMBER_ID, Buffer, E2_OVERSPEED_PREALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x54���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10  
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_PREALARM_NUMBER_TIME_ID, Buffer, E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_PREALARM_NUMBER_TIME_ID, Buffer, E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x55���Զ��壬���ٱ�������,STRING,���30�ֽ�,Ĭ��ֵ���ѳ��٣��������ʻ
    strcpy((char *)Buffer,"���ѳ��٣��������ʻ");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_ALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_OVERSPEED_ALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x56���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_ALARM_GPROUP_TIME_ID, Buffer, E2_OVERSPEED_ALARM_GPROUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_ALARM_GPROUP_TIME_ID, Buffer, E2_OVERSPEED_ALARM_GPROUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x57���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3 
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_ALARM_NUMBER_ID, Buffer, E2_OVERSPEED_ALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_ALARM_NUMBER_ID, Buffer, E2_OVERSPEED_ALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x58���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10  
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_ALARM_NUMBER_TIME_ID, Buffer, E2_OVERSPEED_ALARM_NUMBER_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_ALARM_NUMBER_TIME_ID, Buffer, E2_OVERSPEED_ALARM_NUMBER_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x59���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVERSPEED_ALARM_REPORT_TIME_ID, Buffer, E2_OVERSPEED_ALARM_REPORT_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_ALARM_REPORT_TIME_ID, Buffer, E2_OVERSPEED_ALARM_REPORT_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x5a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬��ʹ�� 
    for(i=0; i<14; i++)
    {
    Buffer[i] = 0;
    }
    if(0 == EepromPram_WritePram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer, E2_OVERSPEED_NIGHT_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer, E2_OVERSPEED_NIGHT_TIME_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x5b���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
    temp = 80;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_OVERSPEED_PERCENT_ID, Buffer, E2_NIGHT_OVERSPEED_PERCENT_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_OVERSPEED_PERCENT_ID, Buffer, E2_NIGHT_OVERSPEED_PERCENT_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x5c���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_TIME_ID, Buffer, E2_NIGHT_DRIVE_PREALARM_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_TIME_ID, Buffer, E2_NIGHT_DRIVE_PREALARM_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x5d���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�,Ĭ��ֵ"��������ҹ��ʱ��"
    strcpy((char *)Buffer,"��������ҹ��ʱ��");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x5e���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer, E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer, E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x5f���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_NUMBER_ID, Buffer, E2_NIGHT_DRIVE_PREALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_DRIVE_PREALARM_NUMBER_ID, Buffer, E2_NIGHT_DRIVE_PREALARM_NUMBER_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x60���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    for(i=0; i<14; i++)
    {
        Buffer[i] = 0;
    }
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_NIGHT_TIME_ID, Buffer, E2_FORBID_DRIVE_NIGHT_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_NIGHT_TIME_ID, Buffer, E2_FORBID_DRIVE_NIGHT_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x61���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
    temp = 30;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_TIME_ID, Buffer, E2_FORBID_DRIVE_PREALARM_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_TIME_ID, Buffer, E2_FORBID_DRIVE_PREALARM_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x62���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
    strcpy((char *)Buffer,"�밴�涨ʱ��ͣ����Ϣ");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x63���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer, E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer, E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x64���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_NUMBER_ID, Buffer, E2_FORBID_DRIVE_PREALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_PREALARM_NUMBER_ID, Buffer, E2_FORBID_DRIVE_PREALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x65���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
    strcpy((char *)Buffer,"�밴�涨ʱ��ͣ����Ϣ");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_ALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_ALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x66���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID, Buffer, E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID, Buffer, E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x67���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_ALARM_NUMBER_ID, Buffer, E2_FORBID_DRIVE_ALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_ALARM_NUMBER_ID, Buffer, E2_FORBID_DRIVE_ALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x68���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
    temp = 20;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_RUN_SPEED_ID, Buffer, E2_FORBID_DRIVE_RUN_SPEED_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_RUN_SPEED_ID, Buffer, E2_FORBID_DRIVE_RUN_SPEED_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x69���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID, Buffer, E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID_LEN))
    {
        EepromPram_WritePram(E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID, Buffer, E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID_LEN);
    }
    //////////////////////
    IWDG_ReloadCounter();//ι��//���õĲ����϶࣬�ҿ��ܻ�������������º���������ʱ����ܽϳ�������ι��
    ///////////////////
    //E2_PRAM_BASE_CUSTOM_ID+0x6a���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
    for(i=0; i<14; i++)
    {
        Buffer[i] = 0;
    }
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_NIGHT_TIME_ID, Buffer, E2_TIRED_DRIVE_NIGHT_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_NIGHT_TIME_ID, Buffer, E2_TIRED_DRIVE_NIGHT_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x6b���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
    temp = 7200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_MAX_DRVIE_TIME_ID, Buffer, E2_NIGHT_MAX_DRVIE_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_MAX_DRVIE_TIME_ID, Buffer, E2_NIGHT_MAX_DRVIE_TIME_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x6c���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
    temp = 1200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_NIGHT_MIN_RELAX_TIME_ID, Buffer, E2_NIGHT_MIN_RELAX_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_NIGHT_MIN_RELAX_TIME_ID, Buffer, E2_NIGHT_MIN_RELAX_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x6d���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
    temp = 30;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer, E2_TIRED_DRIVE_ALARM_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer, E2_TIRED_DRIVE_ALARM_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x6e���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    strcpy((char *)Buffer,"������ʱ��ʻ");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x6f���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer, E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer, E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x70���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_NUMBER_ID, Buffer, E2_TIRED_DRIVE_PREALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_NUMBER_ID, Buffer, E2_TIRED_DRIVE_PREALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x71���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID, Buffer, E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID, Buffer, E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x72���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
    strcpy((char *)Buffer,"����ʱ��ʻ");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_VOICE_ID, Buffer, BufferLen);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x73���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer, E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer, E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x74���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_NUMBER_ID, Buffer, E2_TIRED_DRIVE_ALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_NUMBER_ID, Buffer, E2_TIRED_DRIVE_ALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x75���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID, Buffer, E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID, Buffer, E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x76���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ1200��
    temp = 0xffffffff;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID, Buffer, E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID, Buffer, E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x77���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_REPORT_TIME_ID, Buffer, E2_TIRED_DRIVE_REPORT_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_REPORT_TIME_ID, Buffer, E2_TIRED_DRIVE_REPORT_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x78���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ0,
    temp = 20;//dxl,2014.5.13ԭ��Ĭ��ֵΪ0���ڵ���Ϊ20
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer, E2_TIRED_DRIVE_RUN_SPEED_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer, E2_TIRED_DRIVE_RUN_SPEED_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x79���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
    temp = 60;//dxl,2014.5.13ԭ��Ĭ��ֵΪ0���ڵ���Ϊ60
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer, E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer, E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x7a���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
    temp = 3;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ROUTE_EXCURSION_ALARM_NUMBER_ID, Buffer, E2_ROUTE_EXCURSION_ALARM_NUMBER_ID_LEN))
    {
        EepromPram_WritePram(E2_ROUTE_EXCURSION_ALARM_NUMBER_ID, Buffer, E2_ROUTE_EXCURSION_ALARM_NUMBER_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x7b���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
    temp = 5;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID, Buffer, E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID_LEN))
    {
        EepromPram_WritePram(E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID, Buffer, E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x7c���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
    strcpy((char *)Buffer,"���㰴�涨��·��ʻ");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_ROUTE_EXCURSION_ALARM_VOICE_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_ROUTE_EXCURSION_ALARM_VOICE_ID, Buffer, BufferLen);
    }
    
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_REPORT_FREQ_EVENT_SWITCH_ID, Buffer, E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN))
    {
        EepromPram_WritePram(E2_REPORT_FREQ_EVENT_SWITCH_ID, Buffer, E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN);
    }

    //E2_PRAM_BASE_CUSTOM_ID+0x7E����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
    #if (MOD_TYPE == CDMA_MODULE_MC8331) 
    Buffer[0] = 1;
    #else
    Buffer[0] = 0;
    #endif
    if(0 == EepromPram_WritePram(E2_RUN_FORBID_CALL_SWITCH_ID, Buffer, E2_RUN_FORBID_CALL_SWITCH_ID_LEN))
    {
        EepromPram_WritePram(E2_RUN_FORBID_CALL_SWITCH_ID, Buffer, E2_RUN_FORBID_CALL_SWITCH_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x80����ID���Զ��壬����32pin��LED�ӿڽӵ���������,2�ֽڣ�bit0:����bit1:����bit2:������bit3:���ش�����bit4-bit15:����;��ӦλΪ1��ʾ�ö˿ڽ�����Ӧ���衣
    temp = 0;
    Public_ConvertShortToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_LED_PORT_ID, Buffer, E2_LED_PORT_ID_LEN))
    {
        EepromPram_WritePram(E2_LED_PORT_ID, Buffer, E2_LED_PORT_ID_LEN);
    }
    IWDG_ReloadCounter();//ι��//���õĲ����϶࣬�ҿ��ܻ�������������º���������ʱ����ܽϳ�������ι��
        
        //д����ʻ��¼��״̬�ź�������Ϣ
    

}
/*********************************************************************
//��������  :EepromPram_GBTestSet(void)
//����      :�������ʱ��Ĭ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ʹ�ûָ�Ĭ�ϳ�������Ҳ�Ὣ��������Ϊ������ʾ
*********************************************************************/
void EepromPram_GBTestSet(void)
{
     u32 temp;
     u16 Addr;
     u8  Buffer[5];
    
    //0x0050    ��������,DWORD
    temp = 0xFFFBFEF1;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ALARM_MASK_ID, Buffer, E2_ALARM_MASK_LEN))
    {
        EepromPram_WritePram(E2_ALARM_MASK_ID, Buffer, E2_ALARM_MASK_LEN);
    }
    //0x0056    ���ٳ���ʱ��,��λΪ��,DWORD,Ĭ��Ϊ10��
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer, E2_OVER_SPEED_KEEP_TIME_LEN))
    {
        EepromPram_WritePram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer, E2_OVER_SPEED_KEEP_TIME_LEN);
    }
        //0x0057    ������ʻʱ������//��λΪ��//Ĭ��Ϊ14400��(4Сʱ)
    temp = 14400;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_DRVIE_TIME_ID, Buffer, E2_MAX_DRVIE_TIME_LEN))
    {
        EepromPram_WritePram(E2_MAX_DRVIE_TIME_ID, Buffer, E2_MAX_DRVIE_TIME_LEN);
    }
        //0x0059    ��С��Ϣʱ��//��λΪ��//Ĭ��Ϊ1200��20���ӣ�
    temp = 1200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MIN_RELAX_TIME_ID, Buffer, E2_MIN_RELAX_TIME_LEN))
    {
        EepromPram_WritePram(E2_MIN_RELAX_TIME_ID, Buffer, E2_MIN_RELAX_TIME_LEN);
    }
        //0x005B    ���ٱ���Ԥ����ֵ����λΪ1/10Km/h,DWORD,Ĭ��Ϊ50��5km/h��
    temp = 50;
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer, E2_SPEED_EARLY_ALARM_DVALUE_LEN))
    {
        EepromPram_WritePram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer, E2_SPEED_EARLY_ALARM_DVALUE_LEN);
    }
        //0x005C    ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0  Ĭ��Ϊ1800�루30���ӣ�
    temp = 1800;
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer, E2_TIRE_EARLY_ALARM_DVALUE_LEN))
    {
        EepromPram_WritePram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer, E2_TIRE_EARLY_ALARM_DVALUE_LEN);
    }
        
        //0xF20F ����ϵ��,Ĭ��Ϊ7200,dxl,2015.4.10,֮ǰ��3600
    temp =3600;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &Buffer[1], E2_CAR_FEATURE_COEF_LEN))
    {
        EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID, &Buffer[1], E2_CAR_FEATURE_COEF_LEN);
    }
        
        //0xF210 �ٶ�ѡ�񿪹�,Ĭ��ΪGPS�ٶ�,0x01Ϊ�ֶ�GPS,0x00Ϊ�ֶ�����,0x02Ϊ�Զ�����,0x03Ϊ�Զ�GPS
    Buffer[0] = 0x00;
    if(0 == EepromPram_WritePram(E2_SPEED_SELECT_ID, Buffer, E2_SPEED_SELECT_LEN))
    {
        EepromPram_WritePram(E2_SPEED_SELECT_ID, Buffer, E2_SPEED_SELECT_LEN);
    }
        //0xF24c E2_PRAM_BASE_CUSTOM_ID+4c  ��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ��
		Buffer[0] = 0x81;//bit7:�������־;bit0:��ʻԱδǩ����������
    if(0 == EepromPram_WritePram(E2_UNLOGIN_TTS_PLAY_ID, Buffer, E2_UNLOGIN_TTS_PLAY_ID_LEN))
    {
        EepromPram_WritePram(E2_UNLOGIN_TTS_PLAY_ID, Buffer, E2_UNLOGIN_TTS_PLAY_ID_LEN);
    }
        //0xF278 E2_PRAM_BASE_CUSTOM_ID+0x78���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ0,
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer, E2_TIRED_DRIVE_RUN_SPEED_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer, E2_TIRED_DRIVE_RUN_SPEED_ID_LEN);
    }
    //E2_PRAM_BASE_CUSTOM_ID+0x79���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer, E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN))
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer, E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN);
    }
        
        Buffer[0] = 1;//ʹ���ٶ��쳣״̬���
        Addr = FRAM_VDR_SPEED_STATUS_ENABLE_ADDR;
        FRAM_BufferWrite(Addr, Buffer, FRAM_VDR_SPEED_STATUS_ENABLE_LEN);   
}
/*********************************************************************
//��������  :EepromPram_BBGNTestSet(void)
//����      :���깦�ܲ���ʱ��Ĭ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��ʱΪ��
*********************************************************************/
void EepromPram_BBGNTestSet(void)
{

}
/*********************************************************************
//��������  :EepromPram_DefaultSetRead(void)
//����      :��ϵͳ��1������ʱ��Ĭ������ֵ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void EepromPram_DefaultSetRead(void)
{
    u8  Buffer[31];
    u8  BufferLen;
    u8  flag = 0;
    
//*************ͨ�����ܲ���**********************
           //��������

        BufferLen = EepromPram_ReadPram(E2_TERMINAL_GET_PHONE_STRATEGY_ID, Buffer);
        //flag = 0;
        if(0 == BufferLen)
        {
            flag++;
        }

    //ÿ���ͨ��ʱ��//��λΪ��//Ĭ��Ϊ0,�ر�ͨ��

        BufferLen = EepromPram_ReadPram(E2_MAX_PHONE_TIME_EACH_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
     //ÿ���ͨ��ʱ��//��λΪ��//Ĭ��Ϊ0,�ر�ͨ��
    
        BufferLen = EepromPram_ReadPram(E2_MAX_PHONE_TIME_MONTH_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }

      //����������,bit9,11,12,29,30Ĭ������
    
        BufferLen = EepromPram_ReadPram(E2_ALARM_MASK_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
      //���������ı�SMS����//Ĭ��Ϊȫ��
    
        BufferLen = EepromPram_ReadPram(E2_ALARM_TEXT_SWITCH_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
       //�������㿪��//Ĭ��Ϊ��������ʱ����,�����ر�
    
        BufferLen = EepromPram_ReadPram(E2_ALARM_PHOTO_SWITCH_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
       //��������洢��־//Ĭ��Ϊȫ��
    
        BufferLen = EepromPram_ReadPram(E2_ALARM_PHOTO_STORE_FLAG_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
       //�ؼ���־,û�����ùؼ�����
    
        BufferLen = EepromPram_ReadPram(E2_KEY_FLAG_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
     //�����ۼƼ�ʻʱ������//��λΪ��//Ĭ��Ϊ0
    
        BufferLen = EepromPram_ReadPram(E2_MAX_DRVIE_TIME_ONEDAY_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
      //�ͣ��ʱ��//��λΪ��//Ĭ��Ϊ0
    
        BufferLen = EepromPram_ReadPram(E2_MAX_STOP_CAR_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    //������ID,Ĭ��Ϊ70108
    
        BufferLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    //����ϵ��,Ĭ��Ϊ3600
    
        BufferLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //�ٶ�ѡ�񿪹�,Ĭ��ΪGPS�ٶ�
    
        BufferLen = EepromPram_ReadPram(E2_SPEED_SELECT_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
        //Ĭ��ѡ���Ÿ�
    
        BufferLen = EepromPram_ReadPram(E2_DOOR_SET_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //�ָ�Ĭ�ϳ�ʼ������ʱ�������ʼ��װ����д��־
    
        BufferLen = EepromPram_ReadPram(E2_INSTALL_TIME_FLAG_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //ACC OFFʱGPSģ��Ĭ��Ϊ�� 
    
        BufferLen = EepromPram_ReadPram(E2_GPS_CONTROL_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    //�Ƽ�������,Ĭ��Ϊ��
     
        BufferLen = EepromPram_ReadPram(E2_TAXIMETER_CONTROL_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //�ۼ���ʻ�����0
    
        BufferLen = EepromPram_ReadPram(E2_CAR_TOTAL_MILE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
     //��ײ����Ĭ��Ϊ4g
    
        BufferLen = EepromPram_ReadPram(E2_SET_CRASH_ALARM_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
     //�෭����Ĭ��Ϊ60��
    
        BufferLen = EepromPram_ReadPram(E2_SET_ROLLOVER_ALARM_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //�رռ�ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ��
    
        BufferLen = EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
//*****************���ٹ�����ز���********************
    //����ٶ�,��λkm/h,DWORD,Ĭ��ֵ130

        BufferLen = EepromPram_ReadPram(E2_MAX_SPEED_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    
    //���ٳ���ʱ��,��λΪ��,DWORD,Ĭ��Ϊ10��
    
        BufferLen = EepromPram_ReadPram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    //���ٱ���Ԥ����ֵ����λΪ1/10Km/h,DWORD,Ĭ��Ϊ50��5km/h��  
    
        BufferLen = EepromPram_ReadPram(E2_SPEED_EARLY_ALARM_DVALUE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    
    //����ID���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_KEEPTIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����Ԥ������,STRING,���30�ֽ�,Ĭ��ֵΪ����Ƴ���
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_GPROUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    //����ID���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3 
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10  
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_PREALARM_NUMBER_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬���ٱ�������,STRING,���30�ֽ�,Ĭ��ֵ���ѳ��٣��������ʻ
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_GPROUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3 
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10  
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_NUMBER_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0

        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_ALARM_REPORT_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬��ʹ�� 
    
        BufferLen = EepromPram_ReadPram(E2_OVERSPEED_NIGHT_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_TIME0_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_OVERSPEED_PERCENT_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_MAX_SPEED0_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�,Ĭ��ֵ"��������ҹ��ʱ��"
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_DRIVE_PREALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
//****************ҹ����й�����ز���*****************
    //����ID���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_NIGHT_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_PREALARM_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_PREALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_PREALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_ALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_ALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_RUN_SPEED_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
    
        BufferLen = EepromPram_ReadPram(E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
//*****************��ʱ������ز���********************
    //������ʻʱ������//��λΪ��//Ĭ��Ϊ14400��(4Сʱ)
    
        BufferLen = EepromPram_ReadPram(E2_MAX_DRVIE_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //��С��Ϣʱ��//��λΪ��//Ĭ��Ϊ1200��20���ӣ�
    
        BufferLen = EepromPram_ReadPram(E2_MIN_RELAX_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0  Ĭ��Ϊ1800�루30���ӣ�
    
        BufferLen = EepromPram_ReadPram(E2_TIRE_EARLY_ALARM_DVALUE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_NIGHT_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_MAX_DRVIE_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_MAX_DRVIE_TIME0_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    
    //����ID���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
    
        BufferLen = EepromPram_ReadPram(E2_NIGHT_MIN_RELAX_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ

        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ

        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_VOICE_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_NUMBER_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_REPORT_TIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ0,

        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_RUN_SPEED_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
    //����ID���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
    
        BufferLen = EepromPram_ReadPram(E2_TIRED_DRIVE_RUN_KEEPTIME_ID, Buffer);
        if(0 == BufferLen)
        {
            flag++;
        }
        if(0 != flag)
        {
            flag = 1;
        }
}
/*********************************************************************
//��������  :EepromPram_EraseAllPram(void)
//����      :�������еĲ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void EepromPram_EraseAllPram(void)
{
        u16   Address;
        u8    i;
        
        for(Address=E2_PRAM_START_ADDR; Address<E2_PRAM_END_ADDR; Address++)
        {
            E2prom_WriteByte(Address, 0xff);
            for(i=0; i<100; i++)
            {
            
            }
        }
}
//*************************������********************
/*********************************************************************
//��������  :EepromPram_WriteAllPram(void)
//����      :дeeprom���в���
//����      :PramID:����ID;pBuffer:ָ�����ݣ���д����ֽ���,д�������ֵ�Ļ�����ֽ�Ӧ��ǰ����ָ��;
//      :BufferLen:���ݵĳ���
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :д����ȷʱ�����صĳ��ȵ���BufferLen;д�����ʱ������0,��ʱ�ɳ����ٵ��øú���дһ��
*********************************************************************/
void EepromPram_WriteAllPram(void)
{
    u8  Buffer[25] = {"abcdefghijklmnopqrstuvwxy"};
    u8  Buffer2[25];
    u16 PramID;
    u16 BufferLen;
    u16 length;
    //u8    flag = 0;
    
    for(PramID = 1; PramID < E2_PRAM_ID_INDEX_MAX; PramID++)
    {
        //flag = 0;
        BufferLen = EepromPramLength[PramID];
        EepromPram_WritePram(PramID, Buffer, BufferLen);
        length = EepromPram_ReadPram(PramID, Buffer2);
        if(length != BufferLen)
        {
            //flag = 1;
        }
        else
        {
            if(BufferLen == 0)
            {
                //flag = 1;
            }
        }
    }
}
