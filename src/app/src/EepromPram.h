#ifndef __EEPROM_PRAM_H
#define __EEPROM_PRAM_H

//*********�ļ�����************
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
//********************************************************
//********************************************************
//////////����ռ�ʱ����Ҫע��//////////////////
//����Ϊ1���ܿռ�=�������ݳ���(1�ֽ�)+��������(n�ֽ�)+У��(1�ֽ�)
//����Ϊ0���ܿռ�=��������(n�ֽ�)+У��(1�ֽ�)
//********************************************************
//********************************************************
//********�궨��****************
////////////�������������ֵ,ͨ���޸�E2_SYSTEM_DEFINE_ID_MAX��E2_SELF_DEFINE_ID_MAX����Ӧֵ����/////////////////////////
#define E2_PRAM_ID_INDEX_MAX    (E2_SYSTEM_DEFINE_ID_MAX+(E2_SELF_DEFINE_ID_MAX-E2_PRAM_BASE_CUSTOM_ID)+1)//�������������ֵ,������ж��������

#define E2_PRAM_BASE_CUSTOM_ID  0xF200  //�Զ����������ַ


//****************����ID����******************
#define E2_TERMINAL_HEARTBEAT_ID        0x0001//�ն��������ͼ��//��λ��//Ĭ��ֵ60
#define E2_TCP_ACK_OVERTIME_ID          0x0002//TCP��ϢӦ��ʱʱ��//��λ��//Ĭ��ֵ10
#define E2_TCP_RESEND_TIMES_ID          0x0003//TCP��Ϣ�ش�����//Ĭ��ֵ3
#define E2_UDP_ACK_OVERTIME_ID          0x0004//UDP��ϢӦ��ʱʱ��//��λ��//Ĭ��ֵ10
#define E2_UDP_RESEND_TIMES_ID          0x0005//UDP��Ϣ�ش�����//Ĭ��ֵ3
#define E2_SMS_ACK_OVERTIME_ID          0x0006//SMS��ϢӦ��ʱʱ��//��λ��//Ĭ��ֵ30
#define E2_SMS_RESEND_TIMES_ID          0x0007//SMS��Ϣ�ش�����//Ĭ��ֵ3
#define E2_MAIN_SERVER_APN_ID           0x0010//��������APN
#define E2_MAIN_SERVER_DIAL_NAME_ID         0x0011//������������ͨ�Ų����û���
#define E2_MAIN_SERVER_DIAL_PASSWORD_ID     0x0012//������������ͨ�Ų�������
#define E2_MAIN_SERVER_IP_ID            0x0013//����������ַ//IP������
#define E2_BACKUP_SERVER_APN_ID         0x0014//���ݷ�����APN//����ͨ�Ų��ŷ��ʵ�
#define E2_BACKUP_SERVER_DIAL_NAME_ID       0x0015//���ݷ���������ͨ�Ų����û���
#define E2_BACKUP_SERVER_DIAL_PASSWORD_ID   0x0016//���ݷ���������ͨ�Ų�������
#define E2_BACKUP_SERVER_IP_ID          0x0017//���ݷ�������ַ//IP������
#define E2_MAIN_SERVER_TCP_PORT_ID      0x0018//��������TCP�˿�
#define E2_MAIN_SERVER_UDP_PORT_ID      0x0019//��������UDP�˿�
#define E2_IC_MAIN_SERVER_IP_ID         0x001A//IC����֤��������IP��ַ������
#define E2_IC_MAIN_SERVER_TCP_PORT_ID           0x001B//IC����֤��������TCP�˿�
#define E2_IC_MAIN_SERVER_UDP_PORT_ID           0x001C//IC����֤��������UDP�˿�
#define E2_IC_BACKUP_SERVER_IP_ID       0x001D//IC����֤���ݷ�����IP��ַ������
#define E2_POSITION_REPORT_STRATEGY_ID      0x0020//λ�û㱨����,0Ϊ��ʱ,1Ϊ����,2Ϊ��ʱ����,Ĭ��Ϊ0
#define E2_POSITION_REPORT_SCHEME_ID        0x0021//λ�û㱨����,0����ACC״̬,1���ݵ�¼��ACC״̬,Ĭ��Ϊ0
#define E2_DRIVER_UNLOGIN_REPORT_TIME_ID    0x0022//��ʻԱδ��¼�㱨ʱ����//��λΪ��//Ĭ��ֵΪ60
#define E2_SLEEP_REPORT_TIME_ID         0x0027//����ʱ�㱨ʱ����//��λΪ��//Ĭ��ֵΪ240
#define E2_EMERGENCY_REPORT_TIME_ID         0x0028//��������ʱ�㱨ʱ����//��λΪ��//Ĭ��ֵΪ10
#define E2_ACCON_REPORT_TIME_ID         0x0029//ȱʡʱ��㱨���,��λΪ��,Ĭ��Ϊ30
#define E2_ACCON_REPORT_DISTANCE_ID     0x002C//ȱʡ����㱨���,��λΪ��,Ĭ��Ϊ500
#define E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ID    0x002D//��ʻԱδ��¼�㱨������//��λΪ��//Ĭ��ֵΪ500
#define E2_SLEEP_REPORT_DISTANCE_ID         0x002e//����ʱ�㱨������//��λΪ��//Ĭ��ֵΪ200
#define E2_EMERGENCY_REPORT_DISTANCE_ID     0x002f//��������ʱ�㱨������//��λΪ��//Ĭ��ֵΪ200
#define E2_CORNER_REPORT_ID             0x0030//�յ㲹���Ƕ�,��λ��/��//Ĭ��ֵΪ30
#define E2_ELECTRON_RADIUS_ID           0x0031//����Χ���뾶,��λ��,Ĭ��ֵ500
#define E2_MONITOR_SERVER_PHONE_ID      0x0040//���ƽ̨�绰����
#define E2_TERMINAL_RESET_PHONE_ID      0x0041//�ն˸�λ�绰����
#define E2_TERMINAL_DEFAULT_SET_PHONE_ID    0x0042//�ָ��������õ绰����
#define E2_MONITOR_SERVER_SMS_PHONE_ID      0x0043//���ƽ̨SMS�绰����
#define E2_SMS_TEXT_ALARM_PHONE_ID      0x0044//SMS�ı������绰����
#define E2_TERMINAL_GET_PHONE_STRATEGY_ID   0x0045//�ն˵绰��������,0:�Զ�����;1:�ֶ�����,Ĭ��Ϊ0
#define E2_MAX_PHONE_TIME_EACH_ID       0x0046//ÿ���ͨ��ʱ��//��λΪ��//Ĭ��Ϊ0,�رոù���
#define E2_MAX_PHONE_TIME_MONTH_ID      0x0047//ÿ���ͨ��ʱ��//��λΪ��//Ĭ��Ϊ0,�رոù���
#define E2_MONITOR_PHONE_ID             0x0048//�����绰����
#define E2_MONITOR_SERVER_PRIVILEGE_SMS_ID  0x0049//���ƽ̨��Ȩ���ź���
#define E2_ALARM_MASK_ID            0x0050//����������//bit9,11,12Ĭ������,LCD������,TTS����,IC��ģ�����Ĭ��Ϊ����
#define E2_ALARM_TEXT_SWITCH_ID         0x0051//���������ı�SMS����//��ӦλΪ1��ʾ�����������跢�ͱ�������,Ĭ��Ϊ0
#define E2_ALARM_PHOTO_SWITCH_ID        0x0052//�������㿪��//��ӦλΪ1��ʾ�����������败������,Ĭ��Ϊ1,����������������
#define E2_ALARM_PHOTO_STORE_FLAG_ID        0x0053//��������洢��־//��ӦλΪ1��ʾ�洢,0��ʾ�ϴ�,Ĭ��Ϊ0
#define E2_KEY_FLAG_ID              0x0054//�ؼ���־//��ӦλΪ1��ʾ�ñ���Ϊ�ؼ�����,�ǹؼ��������ܱ��������ֵ�����,Ĭ��Ϊ0
#define E2_MAX_SPEED_ID             0x0055//����ٶ�,��λkm/h,DWORD,Ĭ��ֵ130,��ͨ����Ϊ100
#define E2_OVER_SPEED_KEEP_TIME_ID      0x0056//���ٳ���ʱ��,��λΪ��,DWORD,Ĭ��Ϊ10��,��ͨ����Ϊ�����ϱ�
#define E2_MAX_DRVIE_TIME_ID            0x0057//������ʻʱ������//��λΪ��//Ĭ��Ϊ14400��(4Сʱ)
#define E2_MAX_DRVIE_TIME_ONEDAY_ID         0x0058//�����ۼƼ�ʻʱ������//��λΪ��//Ĭ��Ϊ0xffffffff,�������ù���
#define E2_MIN_RELAX_TIME_ID            0x0059//��С��Ϣʱ��//��λΪ��//Ĭ��Ϊ1200��20���ӣ�
#define E2_MAX_STOP_CAR_TIME_ID         0x005A//�ͣ��ʱ��//��λΪ��//Ĭ��Ϊ0xffffffff,�������ù���
#define E2_SPEED_EARLY_ALARM_DVALUE_ID          0x005B//���ٱ���Ԥ����ֵ����λΪ1/10Km/h,DWORD,Ĭ��Ϊ50��5km/h��  
#define E2_TIRE_EARLY_ALARM_DVALUE_ID           0x005C//ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0  Ĭ��Ϊ1800�루30���ӣ�
/*
��ײ�����������ã�
b7-b0�� ��ײʱ�䣬��λ4ms��
b15-b8����ײ���ٶȣ���λ0.1g�����÷�Χ�ڣ�0-79 ֮�䣬Ĭ��Ϊ10*/
#define E2_SET_CRASH_ALARM_ID           0x005D//��ײ������������,��ײ���ٶ�Ĭ������Ϊ4g
#define E2_SET_ROLLOVER_ALARM_ID            0x005E//�෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ45 ��
/*��ʱ���տ���λ����
λ    ����                         ������Ҫ��
0     ����ͨ��1��ʱ���տ��ر�־    0�������� 1������
1     ����ͨ��2��ʱ���տ��ر�־    0�������� 1������
2     ����ͨ��3��ʱ���տ��ر�־    0�������� 1������
3     ����ͨ��4��ʱ���տ��ر�־    0�������� 1������
4     ����ͨ��5��ʱ���տ��ر�־    0�������� 1������
5-7   ����
8     ����ͨ��1��ʱ���մ洢��־    0���洢��   1���ϴ�
9     ����ͨ��2��ʱ���մ洢��־    0���洢��   1���ϴ�
10    ����ͨ��3��ʱ���մ洢��־    0���洢��   1���ϴ�
11    ����ͨ��4��ʱ���մ洢��־    0���洢��   1���ϴ�
12    ����ͨ��5��ʱ���մ洢��־    0���洢��   1���ϴ�
13-15 ����
16    ��ʱʱ�䵥λ                 0����,����ֵС��5 ��ʱ,�ն˰�5�봦��
                                   1����
17-31 ��ʱʱ����                 �յ��������û�����������ִ��*/
#define E2_TIME_PHOTO_CONTROL_ID            0x0064//��ʱ���տ���,Ĭ��Ϊ������,ֵΪ0
/*
�������տ���λ����
λ    ����                         ������Ҫ��
0     ����ͨ��1�������տ��ر�־    0�������� 1������
1     ����ͨ��2�������տ��ر�־    0�������� 1������
2     ����ͨ��3�������տ��ر�־    0�������� 1������
3     ����ͨ��4�������տ��ر�־    0�������� 1������
4     ����ͨ��5�������տ��ر�־    0�������� 1������
5-7 ����
8     ����ͨ��1�������մ洢��־    0���洢��   1���ϴ�
9     ����ͨ��2�������մ洢��־    0���洢��   1���ϴ�
18
10    ����ͨ��3�������մ洢��־    0���洢��   1���ϴ�
11    ����ͨ��4�������մ洢��־    0���洢��   1���ϴ�
12    ����ͨ��5�������մ洢��־    0���洢��   1���ϴ�
13-15 ����
16 ������뵥λ                    0���ף�����ֵС��100 ��ʱ���ն˰�100 �״���
                                   1������
17-31 ��������� �յ��������û�����������ִ��*/
#define E2_MILE_PHOTO_CONTROL_ID        0x0065//�������տ���,Ĭ��Ϊ������,ֵΪ0
#define E2_PICTURE_QUALITY_ID           0x0070//ͼ��/��Ƶ����//1~10,1Ϊ���
#define E2_PICTURE_BRIGHTNESS_ID        0x0071//����//0~255
#define E2_PICTURE_CONTRAST_ID          0x0072//�Աȶ�//0~127
#define E2_PICTURE_SATURATION_ID        0x0073//���Ͷ�//0~127
#define E2_PICTURE_CHROMA_ID            0x0074//ɫ��//0~255
#define E2_CAR_TOTAL_MILE_ID            0x0080//������̱����//0.1km
#define E2_CAR_PROVINCE_ID          0x0081//�������ڵ�ʡ��ID
#define E2_CAR_CITY_ID              0x0082//�������ڵ�����ID
#define E2_CAR_PLATE_NUM_ID             0x0083//������ͨ�����Ű䷢�Ļ���������
#define E2_CAR_PLATE_COLOR_ID           0x0084//������ɫ//����JT/T415-2006��5.4.12,1:��ɫ,2:��ɫ,3:��ɫ,4:��ɫ,9:����
/*
GNSS  ��λģʽ���������£�
bit0��0������GPS ��λ��     1������GPS ��λ��
bit1��0�����ñ�����λ��     1�����ñ�����λ��
bit2��0������GLONASS ��λ�� 1������GLONASS ��λ��
bit3��0������Galileo ��λ�� 1������Galileo ��λ.*/
#define E2_GPS_SET_MODE_ID          0x0090//GNSS ��λģʽ��Ĭ��Ϊ0x02 ������λ
/*
GNSS �����ʣ��������£�
0x00��4800��0x01��9600��
0x02��19200��0x03��38400��
0x04��57600��0x05��115200.*/
#define E2_GPS_SET_BAUD_ID          0x0091//GNSS �����ʣ�Ĭ��Ϊ0x01 9600
/*
GNSS ģ����ϸ��λ�������Ƶ�ʣ��������£�
0x00��500ms��0x01��1000ms��Ĭ��ֵ����
0x02��2000ms��0x03��3000ms��
0x04��4000ms.*/
#define E2_GPS_SET_OUTPUT_RATE_ID           0x0092//GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms
#define E2_GPS_SET_GATHER_RATE_ID       0x0093//GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1.
/*
GNSS ģ����ϸ��λ�����ϴ���ʽ��
0x00�����ش洢�����ϴ���Ĭ��ֵ����
0x01����ʱ�����ϴ���
0x02�����������ϴ���
0x0B�����ۼ�ʱ���ϴ����ﵽ����ʱ����Զ�ֹͣ�ϴ���
0x0C�����ۼƾ����ϴ����ﵽ������Զ�ֹͣ�ϴ���
0x0D�����ۼ������ϴ����ﵽ�ϴ��������Զ�ֹͣ�ϴ�*/
#define E2_GPS_SET_UPLOAD_MODE_ID       0x0094//GNSS ģ����ϸ��λ�����ϴ���ʽ��Ĭ��Ϊ0x00�����ش洢���ϴ�
/*
GNSS ģ����ϸ��λ�����ϴ����ã�
�ϴ���ʽΪ0x01 ʱ����λΪ�룻
�ϴ���ʽΪ0x02 ʱ����λΪ�ף�
�ϴ���ʽΪ0x0B ʱ����λΪ�룻
�ϴ���ʽΪ0x0C ʱ����λΪ�ף�
�ϴ���ʽΪ0x0D ʱ����λΪ����*/
#define E2_GPS_SET_UPLOAD_VALUE_ID      0x0095//GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0   
#define E2_CAN1_GATHER_TIME_ID          0x0100//CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ� 
#define E2_CAN1_UPLOAD_TIME_ID          0x0101//CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
#define E2_CAN2_GATHER_TIME_ID          0x0102//CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
#define E2_CAN2_UPLOAD_TIME_ID          0x0103//CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
/*
CAN ����ID �����ɼ����ã�
bit63-bit32    ��ʾ��ID �ɼ�ʱ����(ms)�� 0 ��ʾ���ɼ���
bit31          ��ʾCAN ͨ���ţ�            0��CAN1��    1��CAN2��
bit30          ��ʾ֡���ͣ�                0����׼֡��  1����չ֡��
bit29          ��ʾ���ݲɼ���ʽ��          0��ԭʼ���ݣ�1���ɼ�����ļ���ֵ��
bit28-bit0     ��ʾCAN ����ID*/
#define E2_CAN_SET_ONLY_GATHER_0_ID     0x0110//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_1_ID     0x0111//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_2_ID     0x0112//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_3_ID     0x0113//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_4_ID     0x0114//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_5_ID     0x0115//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_6_ID     0x0116//CAN ����ID �����ɼ�����BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_7_ID     0x0117//CAN ����ID �����ɼ�����BYTE[8]


///////////ϵͳ��������IDΪ����ID��1,��������һ���˹��޸�/////////////////////////////////////
#define E2_SYSTEM_DEFINE_ID_MAX         (E2_CAN_SET_ONLY_GATHER_7_ID+1)//ϵͳ��������IDΪ����ID��1,ע�⣺ÿ����������ʱ��Ҫ�����Ĳ���ID������
///////////////////////////////////////////////////////

//*****************�Զ������ID******************
#define E2_LOGINGPRS_PASSWORD_ID        E2_PRAM_BASE_CUSTOM_ID+0x00//��Ȩ��
#define E2_REGISTER_SMS_PHONE_ID        E2_PRAM_BASE_CUSTOM_ID+0x01//ע�����Ϣ����,��ע��ָ��ú���
#define E2_PROGRAM_UPDATA_REQUEST_ID        E2_PRAM_BASE_CUSTOM_ID+0x02//����ID���Զ��壬�������������־
#define E2_PROGRAM_UPDATA_VERIFYCODE_ID     E2_PRAM_BASE_CUSTOM_ID+0x03//����ID���Զ��壬��������У����
#define E2_PROGRAM_UPDATA_LENGTH_ID     E2_PRAM_BASE_CUSTOM_ID+0x04//����ID���Զ��壬������������
#define E2_MANUFACTURE_ID           E2_PRAM_BASE_CUSTOM_ID+0x05//����ID���Զ��壬������ID��5�ֽ�        
#define E2_DEVICE_ID                E2_PRAM_BASE_CUSTOM_ID+0x06//����ID���Զ��壬�����ն�ID��7�ֽ�
#define E2_DEVICE_PHONE_ID          E2_PRAM_BASE_CUSTOM_ID+0x07//����ID���Զ��壬�ն��ֻ��ţ�6�ֽڣ�
#define E2_CAR_IDENTIFICATION_CODE_ID       E2_PRAM_BASE_CUSTOM_ID+0x08//����ID���Զ��壬����ʶ����
#define E2_CAR_TYPE_ID              E2_PRAM_BASE_CUSTOM_ID+0x09//����ID���Զ��壬���Ʒ���
#define E2_CURRENT_DRIVER_ID            E2_PRAM_BASE_CUSTOM_ID+0x0A//����ID���Զ��壬��ǰ��ʻԱ����
#define E2_CURRENT_LICENSE_ID           E2_PRAM_BASE_CUSTOM_ID+0x0B//����ID���Զ��壬��ǰ��ʻ֤����
#define E2_FIRMWARE_VERSION_ID          E2_PRAM_BASE_CUSTOM_ID+0x0C//����ID���Զ��壬�̼��汾��
#define E2_ALL_DRIVER_INFORMATION_ID        E2_PRAM_BASE_CUSTOM_ID+0x0D//����ID���Զ��壬���м�ʻԱ��Ϣ
#define E2_SPEED_SENSOR_COEF_ID         E2_PRAM_BASE_CUSTOM_ID+0x0E//����ID���Զ��壬�ٶȴ�����ϵ��
#define E2_CAR_FEATURE_COEF_ID          E2_PRAM_BASE_CUSTOM_ID+0x0F//����ID���Զ��壬��������ϵ��
#define E2_SPEED_SELECT_ID          E2_PRAM_BASE_CUSTOM_ID+0x10//����ID���Զ��壬�ٶ�ѡ�񿪹�
#define E2_BACKLIGHT_SELECT_ID          E2_PRAM_BASE_CUSTOM_ID+0x11//����ID���Զ��壬����ѡ�񿪹�
#define E2_FIRST_POWERON_FLAG_ID        E2_PRAM_BASE_CUSTOM_ID+0x12//����ID���Զ��壬ϵͳ��1���ϵ��־
#define E2_ACC_ON_INTERVAL_ID           E2_PRAM_BASE_CUSTOM_ID+0x13//����ID���Զ��壬ACC ON ʱ�㱨ʱ����,û���õ�
#define E2_ACC_ON_DISTANCE_ID           E2_PRAM_BASE_CUSTOM_ID+0x14//����ID���Զ��壬ACC ON ʱ�㱨������,û���õ�
#define E2_UPDATA_LINK_CONTROL_ID       E2_PRAM_BASE_CUSTOM_ID+0x15//����ID���Զ��壬���ӿ���
#define E2_UPDATA_DIAL_NAME_ID          E2_PRAM_BASE_CUSTOM_ID+0x16//����ID���Զ��壬�����������ŵ�����
#define E2_UPDATA_DIAL_USER_ID          E2_PRAM_BASE_CUSTOM_ID+0x17//����ID���Զ��壬�������������û���
#define E2_UPDATA_DIAL_PASSWORD_ID      E2_PRAM_BASE_CUSTOM_ID+0x18//����ID���Զ��壬����������������
#define E2_UPDATA_IP_ID             E2_PRAM_BASE_CUSTOM_ID+0x19//����ID���Զ��壬��������IP��ַ
#define E2_UPDATA_TCP_PORT_ID           E2_PRAM_BASE_CUSTOM_ID+0x1a//����ID���Զ��壬��������TCP�˿�
#define E2_UPDATA_UDP_PORT_ID           E2_PRAM_BASE_CUSTOM_ID+0x1b//����ID���Զ��壬��������UDP�˿�
#define E2_UPDATA_MONITOR_SERVER_CODE_ID    E2_PRAM_BASE_CUSTOM_ID+0x1c//����ID���Զ��壬�����������ƽ̨��Ȩ��
#define E2_UPDATA_HARDWARE_VERSION_ID       E2_PRAM_BASE_CUSTOM_ID+0x1d//����ID���Զ��壬��������Ӳ���汾
#define E2_UPDATA_FIRMWARE_VERSION_ID       E2_PRAM_BASE_CUSTOM_ID+0x1e//����ID���Զ��壬���������̼��汾
#define E2_UPDATA_URL_ID            E2_PRAM_BASE_CUSTOM_ID+0x1f//����ID���Զ��壬��������URL������ַ
#define E2_UPDATA_TIME_ID           E2_PRAM_BASE_CUSTOM_ID+0x20//����ID���Զ��壬�����������ӵ�ָ��������ʱ��
#define E2_HOT_PHONE_ID             E2_PRAM_BASE_CUSTOM_ID+0x21//����ID���Զ��壬���ߵ绰,û���õ�
#define E2_UPDATA_MANUFACTURE_ID        E2_PRAM_BASE_CUSTOM_ID+0x22//����ID���Զ��壬��������������ID
#define E2_PHONE_CALLBACK_ID            E2_PRAM_BASE_CUSTOM_ID+0x23//����ID���Զ��壬�绰�ز�����
#define E2_CALLBACK_FLAG_ID         E2_PRAM_BASE_CUSTOM_ID+0x24//����ID���Զ��壬�绰�ز���־,1Ϊ����;0Ϊ��ͨͨ��
#define E2_DEVICE_PHONE_BACKUP_ID       E2_PRAM_BASE_CUSTOM_ID+0x25//����ID���Զ��壬�ն��ֻ��ţ�6�ֽڣ�ASCII��,����
#define E2_LAST_LOCATION_ID         E2_PRAM_BASE_CUSTOM_ID+0x26//����ID���Զ��壬���һ��������λ��
#define E2_PHOTO_SPACE_ID                       E2_PRAM_BASE_CUSTOM_ID+0x27//����ID���Զ��壬����ʱ����
#define E2_PHOTO_NUM_ID                         E2_PRAM_BASE_CUSTOM_ID+0x28//����ID���Զ��壬��������
#define E2_DOOR_SET_ID                          E2_PRAM_BASE_CUSTOM_ID+0x29//����ID���Զ��壬����ѡ��0Ϊ���Ÿ���Ч��1Ϊ��������Ч
#define E2_DEVICE_BACKUP_ID         E2_PRAM_BASE_CUSTOM_ID+0x2a//����ID���Զ��壬�ն�ID�ű���
#define E2_CDMA_SLEEP_ID            E2_PRAM_BASE_CUSTOM_ID+0x2b//����ID���Զ��壬CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
#define E2_PASSWORD_ID              E2_PRAM_BASE_CUSTOM_ID+0x2c//����ID���Զ��壬ά���˵�����
#define E2_INSTALL_TIME_ID          E2_PRAM_BASE_CUSTOM_ID+0x2d//����ID���Զ��壬��װ����
#define E2_INSTALL_TIME_FLAG_ID         E2_PRAM_BASE_CUSTOM_ID+0x2e//����ID���Զ��壬��װ����д���־
#define E2_HORN_SET_ID              E2_PRAM_BASE_CUSTOM_ID+0x2f//����ID���Զ��壬�����ȣ����룩ѡ��1Ϊ����Ч����1Ϊ����Ч��Ĭ��Ϊ����Ч
#define E2_GPS_CONTROL_ID           E2_PRAM_BASE_CUSTOM_ID+0x30//����ID���Զ��壬ACC OFFʱGPSģ�����,0Ϊ��,1Ϊ��
#define E2_USB_UPDATA_FLAG_ID           E2_PRAM_BASE_CUSTOM_ID+0x31//����ID���Զ��壬USB������־,
#define E2_TAXIMETER_CONTROL_ID         E2_PRAM_BASE_CUSTOM_ID+0x32//����ID���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
#define E2_TOPLIGHT_CONTROL_ID          E2_PRAM_BASE_CUSTOM_ID+0x33//����ID���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ��������ʹ��
#define E2_LEDLIGHT_CONTROL_ID          E2_PRAM_BASE_CUSTOM_ID+0x34//����ID���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ����,��ʹ��
#define E2_HB_IC_DRIVER_ID          E2_PRAM_BASE_CUSTOM_ID+0x35//����ID���Զ��壬���÷����б��׼��ʽ��0x0702ָ��ļ�ʻԱ��Ϣ��IC����
#define E2_STATUS_PHOTO_SWITCH_ID       E2_PRAM_BASE_CUSTOM_ID+0x36//����ID���Զ��壬״̬���㿪��
#define E2_STATUS_PHOTO_STORE_FLAG_ID       E2_PRAM_BASE_CUSTOM_ID+0x37//����ID���Զ��壬״̬����洢��־//Ĭ��Ϊȫ��
#define E2_RESUME_NOUSED_ID         E2_PRAM_BASE_CUSTOM_ID+0x38//����ID���Զ��壬�ָ��ն�Ϊδʹ��״̬
#define E2_NIGHT_TIME0_ID           E2_PRAM_BASE_CUSTOM_ID+0x39//����ID���Զ��壬ҹ��ʱ�䷶Χ,BCD��,�µ�EGS701��1��ʹ��,�����ʹ���������
#define E2_NIGHT_MAX_DRVIE_TIME0_ID     E2_PRAM_BASE_CUSTOM_ID+0x3a//����ID���Զ��壬ҹ�䳬ʱ��ʻ����,0��ʾ������ҹ��ģʽ,�µ�EGS701��1��ʹ��,�����ʹ���������
#define E2_NIGHT_MAX_SPEED0_ID          E2_PRAM_BASE_CUSTOM_ID+0x3b//����ID���Զ��壬ҹ������ֵ,�µ�EGS701��1��ʹ��,�����ʹ���������
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3C���Զ��壬V1��13����Э��VTK�������ź�SOS�����л���VTK�ã��б��޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3D���Զ��壬V1��13����Э��VTK�����Զ��������ʱ�䣬VTK�ã��б��޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3E���Զ��壬V1��13����Э��VTK��������ʱ�䣬VTK�ã��б��޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3F���Զ��壬V1��13����Э��VTK�����𶯼��ʱ�䣬VTK�ã��б��޴���
#define E2_LOAD_MAX_ID              E2_PRAM_BASE_CUSTOM_ID+0x40//����ID���Զ��壬����������ƣ�������ֵ���г��ر�������λΪkg��4�ֽ�
#define E2_CAR_INIT_MILE_ID                     E2_PRAM_BASE_CUSTOM_ID+0x41//����ID���Զ��壬������ʼ����̣�DWORD
#define E2_CAR_ONLY_NUM_ID                      E2_PRAM_BASE_CUSTOM_ID+0x42//����ID���Զ��壬Ψһ�Ա�ţ��ַ�����35�ֽ�
#define E2_DEFINE_ALARM_MASK_WORD_ID        E2_PRAM_BASE_CUSTOM_ID+0x43//����ID���Զ��壬�Զ��屨�������֣�DWORD
#define E2_DEFINE_STATUS_CHANGE_PHOTO_ENABLE_ID E2_PRAM_BASE_CUSTOM_ID+0x44//����ID���Զ��壬�Զ���״̬�仯����ʹ�ܿ��أ�DWORD
#define E2_DEFINE_STATUS_CHANGE_PHOTO_STORE_ID  E2_PRAM_BASE_CUSTOM_ID+0x45//����ID���Զ��壬�Զ���״̬�仯���մ洢���أ�DWORD
//����IDE2_PRAM_BASE_CUSTOM_ID+0x46����С����Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x47���������Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x48������ٶ���ֵ���Զ��壬EGS702�ã�EGS701�޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x49��������ʼ�ͺģ��Զ��壬�ϵ�EGS701�ã�EGS701�޴���
#define E2_SHUTDOWN_MODE_TIME_ID        E2_PRAM_BASE_CUSTOM_ID+0x4a//����ID���Զ��壬�ػ�ģʽʱ�������
#define E2_SHUTDOWN_MODE_POWER_ID       E2_PRAM_BASE_CUSTOM_ID+0x4b//����ID���Զ��壬������ߵ�ѹ��
#define E2_UNLOGIN_TTS_PLAY_ID          E2_PRAM_BASE_CUSTOM_ID+0x4c//����ID���Զ��壬��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ������IC��ʱ��������Ϊ������������Ϊ��
#define E2_TERMINAL_TYPE_ID                     E2_PRAM_BASE_CUSTOM_ID+0x4d//����ID���Զ��壬�ն����ͣ�����Э���ѯ�ն����������õ�������
#define E2_GPS_ATTRIBUTE_ID                     E2_PRAM_BASE_CUSTOM_ID+0x4e//����ID���Զ��壬GNSS���ԣ�����Э���ѯ�ն����������õ�������
#define E2_GPRS_ATTRIBUTE_ID                    E2_PRAM_BASE_CUSTOM_ID+0x4f//����ID���Զ��壬ͨѶģ�����ԣ�����Э���ѯ�ն����������õ�������
//#define   E2_OVERSPEED_PREALARM_MARGIN_ID     E2_PRAM_BASE_CUSTOM_ID+0x50//����ID���Զ��壬����Ԥ����ֵ,DWORD,��λ0.1km/h,Ĭ��ֵ50,���׼��0x005b�ظ���
#define E2_OVERSPEED_PREALARM_KEEPTIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x50//����ID���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
#define E2_OVERSPEED_PREALARM_VOICE_ID      E2_PRAM_BASE_CUSTOM_ID+0x51//����ID���Զ��壬����Ԥ������,STRING,���30�ֽ�,Ĭ��ֵΪ����Ƴ���
#define E2_OVERSPEED_PREALARM_GPROUP_TIME_ID    E2_PRAM_BASE_CUSTOM_ID+0x52//����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_OVERSPEED_PREALARM_NUMBER_ID     E2_PRAM_BASE_CUSTOM_ID+0x53//����ID���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_OVERSPEED_PREALARM_NUMBER_TIME_ID    E2_PRAM_BASE_CUSTOM_ID+0x54//����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
//#define   E2_OVERSPEED_ALARM_VALUE_ID     E2_PRAM_BASE_CUSTOM_ID+0x56//����ID���Զ��壬���ٱ���ֵ��DWORD,��λkm/h,Ĭ��ֵ130,���׼��0x0055�ظ���
//#define   E2_OVERSPEED_ALARM_KEEPTIME_ID      E2_PRAM_BASE_CUSTOM_ID+0x57//����ID���Զ��壬���ٱ�������ʱ�䣬DWORD,��λ��Ĭ��ֵ10,���׼��0x0056�ظ���
#define E2_OVERSPEED_ALARM_VOICE_ID     E2_PRAM_BASE_CUSTOM_ID+0x55//����ID���Զ��壬���ٱ�������,STRING,���30�ֽ�,
#define E2_OVERSPEED_ALARM_GPROUP_TIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x56//����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_OVERSPEED_ALARM_NUMBER_ID        E2_PRAM_BASE_CUSTOM_ID+0x57//����ID���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_OVERSPEED_ALARM_NUMBER_TIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x58//����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
#define E2_OVERSPEED_ALARM_REPORT_TIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x59//����ID���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
#define E2_OVERSPEED_NIGHT_TIME_ID      E2_PRAM_BASE_CUSTOM_ID+0x5a//����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬��ʹ��
#define E2_NIGHT_OVERSPEED_PERCENT_ID       E2_PRAM_BASE_CUSTOM_ID+0x5b//����ID���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
#define E2_NIGHT_DRIVE_PREALARM_TIME_ID     E2_PRAM_BASE_CUSTOM_ID+0x5c//����ID���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
#define E2_NIGHT_DRIVE_PREALARM_VOICE_ID    E2_PRAM_BASE_CUSTOM_ID+0x5d//����ID���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�,Ĭ��ֵ"��������ҹ��ʱ��"
#define E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x5e//����ID���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
#define E2_NIGHT_DRIVE_PREALARM_NUMBER_ID   E2_PRAM_BASE_CUSTOM_ID+0x5f//����ID���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_NIGHT_TIME_ID       E2_PRAM_BASE_CUSTOM_ID+0x60//����ID���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
#define E2_FORBID_DRIVE_PREALARM_TIME_ID    E2_PRAM_BASE_CUSTOM_ID+0x61//����ID���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
#define E2_FORBID_DRIVE_PREALARM_VOICE_ID   E2_PRAM_BASE_CUSTOM_ID+0x62//����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
#define E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID  E2_PRAM_BASE_CUSTOM_ID+0x63//����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_FORBID_DRIVE_PREALARM_NUMBER_ID  E2_PRAM_BASE_CUSTOM_ID+0x64//����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_ALARM_VOICE_ID      E2_PRAM_BASE_CUSTOM_ID+0x65//����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�,Ĭ��ֵ"�밴�涨ʱ��ͣ����Ϣ"
#define E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID E2_PRAM_BASE_CUSTOM_ID+0x66//����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_FORBID_DRIVE_ALARM_NUMBER_ID     E2_PRAM_BASE_CUSTOM_ID+0x67//����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_RUN_SPEED_ID        E2_PRAM_BASE_CUSTOM_ID+0x68//����ID���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
#define E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x69//����ID���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
                                        //���ٶȴ��ڽ���ʱ����ʻ�ٶ��ҳ���ʱ��ﵽ����ʱ����ʻ�ٶȳ���ʱ�䣬�϶�Ϊ��ʻ
#define E2_TIRED_DRIVE_NIGHT_TIME_ID        E2_PRAM_BASE_CUSTOM_ID+0x6a//����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
#define E2_NIGHT_MAX_DRVIE_TIME_ID      E2_PRAM_BASE_CUSTOM_ID+0x6b//����ID���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
#define E2_NIGHT_MIN_RELAX_TIME_ID      E2_PRAM_BASE_CUSTOM_ID+0x6c//����ID���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
#define E2_TIRED_DRIVE_ALARM_TIME_ID        E2_PRAM_BASE_CUSTOM_ID+0x6d//����ID���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
#define E2_TIRED_DRIVE_PREALARM_VOICE_ID    E2_PRAM_BASE_CUSTOM_ID+0x6e//����ID���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
#define E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID   E2_PRAM_BASE_CUSTOM_ID+0x6f//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
#define E2_TIRED_DRIVE_PREALARM_NUMBER_ID   E2_PRAM_BASE_CUSTOM_ID+0x70//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID  E2_PRAM_BASE_CUSTOM_ID+0x71//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
#define E2_TIRED_DRIVE_ALARM_VOICE_ID       E2_PRAM_BASE_CUSTOM_ID+0x72//����ID���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
#define E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID  E2_PRAM_BASE_CUSTOM_ID+0x73//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
#define E2_TIRED_DRIVE_ALARM_NUMBER_ID      E2_PRAM_BASE_CUSTOM_ID+0x74//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID E2_PRAM_BASE_CUSTOM_ID+0x75//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
#define E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID  E2_PRAM_BASE_CUSTOM_ID+0x76//����ID���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
#define E2_TIRED_DRIVE_REPORT_TIME_ID       E2_PRAM_BASE_CUSTOM_ID+0x77//����ID���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
#define E2_TIRED_DRIVE_RUN_SPEED_ID     E2_PRAM_BASE_CUSTOM_ID+0x78//����ID���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
#define E2_TIRED_DRIVE_RUN_KEEPTIME_ID      E2_PRAM_BASE_CUSTOM_ID+0x79//����ID���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
#define E2_ROUTE_EXCURSION_ALARM_NUMBER_ID      E2_PRAM_BASE_CUSTOM_ID+0x7A//����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
#define E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID  E2_PRAM_BASE_CUSTOM_ID+0x7B//����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
#define E2_ROUTE_EXCURSION_ALARM_VOICE_ID       E2_PRAM_BASE_CUSTOM_ID+0x7C//����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
#define E2_REPORT_FREQ_EVENT_SWITCH_ID          E2_PRAM_BASE_CUSTOM_ID+0x7D//����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,BIT1λ��1����ʱ����,DWORD
#define E2_RUN_FORBID_CALL_SWITCH_ID            E2_PRAM_BASE_CUSTOM_ID+0x7E//����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
#define E2_SIM_ICCID_NUMBER_ID                  E2_PRAM_BASE_CUSTOM_ID+0x7F//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
#define E2_LED_PORT_ID                          E2_PRAM_BASE_CUSTOM_ID+0x80//����ID���Զ��壬����32pin��LED�ӿڽӵ���������,��������Ϊ����1����
                                                                   //2�ֽڣ�bit0:����bit1:����bit2:������bit3:���ش�����bit4-bit15:����;��ӦλΪ1��ʾ�ö˿ڽ�����Ӧ���衣
#define E2_POS_PORT_ID                          E2_PRAM_BASE_CUSTOM_ID+0x81//����ID���Զ��壬����16pin�ӿڽӵ���������,��������Ϊ����2���� 
                                                                    //2�ֽڣ�bit0:����,bit1:����,bit2:��ͨ������λ������;bit3:��ʵ����λ������;bit4-bit15:����;��ӦλΪ1��ʾ�ö˿ڽ�����Ӧ���衣
#define E2_OBD_SWITCH_ID                        E2_PRAM_BASE_CUSTOM_ID+0x83//����ID���Զ��壬1�ֽڣ�0��ʾδ������OBDģ�飬1��ʾ����
#define E2_SECOND_MAIN_SERVER_IP_ID             E2_PRAM_BASE_CUSTOM_ID+0x84//����ID���Զ��壬��2��������������IP��ַ������
                                                                            //0xF281-0xF282ΪEGS702ʹ��,0xF283ΪE-MIV404ʹ��
#define E2_SECOND_MAIN_SERVER_TCP_PORT_ID       E2_PRAM_BASE_CUSTOM_ID+0x85//����ID���Զ��壬��2��������������TCP�˿�
#define E2_SECOND_MAIN_SERVER_UDP_PORT_ID       E2_PRAM_BASE_CUSTOM_ID+0x86//����ID���Զ��壬��2��������������UDP�˿�
#define E2_SECOND_BACKUP_SERVER_IP_ID           E2_PRAM_BASE_CUSTOM_ID+0x87//����ID���Զ��壬��2�����ӱ��ݷ�������ַ//IP������
#define E2_SECOND_LOGINGPRS_PASSWORD_ID         E2_PRAM_BASE_CUSTOM_ID+0x88//����ID���Զ��壬��2�����Ӽ�Ȩ��
#define E2_SECOND_ACC_OFF_REPORT_TIME_ID        E2_PRAM_BASE_CUSTOM_ID+0x89//����ID���Զ��壬��2���������߻㱨ʱ����,��λ��,DWORD
#define E2_SECOND_ACC_ON_REPORT_TIME_ID        E2_PRAM_BASE_CUSTOM_ID+0x8a//����ID���Զ��壬��2������ȱʡ�㱨ʱ����,��λ��,DWORD
#define E2_LOCK1_ENABLE_FLAG_ID           E2_PRAM_BASE_CUSTOM_ID+0x8B//ʹ��ǰ����һʹ�ܿ��ر�־
#define E2_LOCK2_ENABLE_FLAG_ID           E2_PRAM_BASE_CUSTOM_ID+0x8C//ʹ��ǰ������ʹ�ܿ��ر�־

#define E2_DEEP_SENSORMIN_ID                    (E2_PRAM_BASE_CUSTOM_ID+0x96)//����ID���Զ��壬����������Сֵ
#define E2_DEEP_SENSORMIN_LENGTH_ID             (E2_PRAM_BASE_CUSTOM_ID+0x97)//����ID���Զ��壬��������Сֵ��Ӧ�ĸ����λ����
#define E2_DEEP_BASE_HEIGHT_ID                  (E2_PRAM_BASE_CUSTOM_ID+0x98)//����ID���Զ��壬�����׼ֵ����λ����
#define E2_DEEP_ARM_LENGTH_ID                   (E2_PRAM_BASE_CUSTOM_ID+0x99)//����ID���Զ��壬��׼�۳�����λ����
#define E2_DEEP_MAX_LENGTH_ID                   (E2_PRAM_BASE_CUSTOM_ID+0x9A)//����ID���Զ��壬��׼�߶ȣ���λ����
#define E2_DEEP_SENSORMAX_ID                    (E2_PRAM_BASE_CUSTOM_ID+0x9B)//����ID���Զ��壬�����������ֵ
#define E2_DEEP_SENSORMAX_LENGTH_ID             (E2_PRAM_BASE_CUSTOM_ID+0x9C)//����ID���Զ��壬���������ֵ��Ӧ�ĸ����λ����

#define E2_ENGINE_NUM_ID                          (E2_PRAM_BASE_CUSTOM_ID+0x9D)//����ID���Զ��壬����ģʽ
//bit7:������;bit6:����Э����;bit5:���깦�ܼ��;bit4:����ƽ̨���;bit3:����Э��3�Ż����;bit2:��;bit1:����Э��1�Ż���⣬���������ò�������Ϊ0

/////////�Զ��������IDΪ����ID��1,��������һ���˹��޸�/////////////////////////////////////
#define E2_SELF_DEFINE_ID_MAX           (E2_ENGINE_NUM_ID+1)//�Զ��������ID,ע�⣺ÿ����������ʱ��Ҫ�����Ĳ���ID������

//Ԥ��һ��ID��IC�����ͣ����۵ۿ������ƽӴ�ʽ�������ƷǽӴ�ʽ��

//#define   

//***************************************���������eeprom��ַ*******************************
//#define EEPROM_PRAM_BASE_ADDR 0x420
#define EEPROM_FIRST_USE_FLAG_ADDR  0//eeprom��һ��ʹ�ñ�־λ�洢��ַ
#define EEPROM_PRAM_BASE_ADDR           10
#define E2_PRAM_START_ADDR    EEPROM_PRAM_BASE_ADDR
#define E2_PRAM_END_ADDR      (EEPROM_PRAM_BASE_ADDR+1610+10)
//��������ʼ��ַ

#define E2_TERMINAL_HEARTBEAT_ADDR      EEPROM_PRAM_BASE_ADDR       
//����ID0x0001���ն��������ͼ������λ�룬Ĭ��ֵ60

#define E2_TCP_ACK_OVERTIME_ADDR        (EEPROM_PRAM_BASE_ADDR+5)           
//����ID0x0002��TCP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10

#define E2_TCP_RESEND_TIMES_ADDR        (EEPROM_PRAM_BASE_ADDR+10)              
//����ID0x0003��TCP��Ϣ�ش�������Ĭ��ֵ3

#define E2_UDP_ACK_OVERTIME_ADDR        (EEPROM_PRAM_BASE_ADDR+15)          
//����ID0x0004��UDP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10

#define E2_UDP_RESEND_TIMES_ADDR        (EEPROM_PRAM_BASE_ADDR+20)          
//����ID0x0005��UDP��Ϣ�ش�������Ĭ��ֵ3

#define E2_SMS_ACK_OVERTIME_ADDR        (EEPROM_PRAM_BASE_ADDR+25)              
//����ID0x0006��SMS��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ30

#define E2_SMS_RESEND_TIMES_ADDR        (EEPROM_PRAM_BASE_ADDR+30)          
//����ID0x0007��SMS��Ϣ�ش�������Ĭ��ֵ3

#define E2_MAIN_SERVER_APN_ADDR         (EEPROM_PRAM_BASE_ADDR+35)          
//����ID0x0010����������APN

#define E2_MAIN_SERVER_DIAL_NAME_ADDR       (EEPROM_PRAM_BASE_ADDR+60)          
//����ID0x0011��������������ͨ�Ų����û���

#define E2_MAIN_SERVER_DIAL_PASSWORD_ADDR   (EEPROM_PRAM_BASE_ADDR+85)          
//����ID0x0012��������������ͨ�Ų�������

#define E2_MAIN_SERVER_IP_ADDR          (EEPROM_PRAM_BASE_ADDR+110)     
//����ID0x0013������������ַ��IP������

#define E2_BACKUP_SERVER_APN_ADDR       (EEPROM_PRAM_BASE_ADDR+135)             
//����ID0x0014�����ݷ�����APN������ͨ�Ų��ŷ��ʵ�

#define E2_BACKUP_SERVER_DIAL_NAME_ADDR     (EEPROM_PRAM_BASE_ADDR+160) 
//����ID0x0015�����ݷ���������ͨ�Ų����û���

#define E2_BACKUP_SERVER_DIAL_PASSWORD_ADDR (EEPROM_PRAM_BASE_ADDR+185)     
//����ID0x0016�����ݷ���������ͨ�Ų�������

#define E2_BACKUP_SERVER_IP_ADDR        (EEPROM_PRAM_BASE_ADDR+210)         
//����ID0x0017�����ݷ�������ַ��IP������

#define E2_MAIN_SERVER_TCP_PORT_ADDR        (EEPROM_PRAM_BASE_ADDR+235)             
//����ID0x0018����������TCP�˿�
    
#define E2_MAIN_SERVER_UDP_PORT_ADDR        (EEPROM_PRAM_BASE_ADDR+240) 
//����ID0x0019����������UDP�˿�

#define E2_BACKUP_SERVER_TCP_PORT_ADDR      (EEPROM_PRAM_BASE_ADDR+245) 
//����ID0x001a�����ݷ�����TCP�˿�

#define E2_BACKUP_SERVER_UDP_PORT_ADDR      (EEPROM_PRAM_BASE_ADDR+250)     
//����ID0x001b�����ݷ�����UDP�˿�

#define E2_REGISTER_SMS_CENTER_NUM_ADDR     (EEPROM_PRAM_BASE_ADDR+255)
//����ID0x001C��ע��������ĺ���

#define E2_PRIVILEGE_SMS_CENTER_NUM_ADDR    (EEPROM_PRAM_BASE_ADDR+280)     
//����ID0x001D�����ƶ��ź��루��Ȩ��

#define E2_POSITION_REPORT_STRATEGY_ADDR    (EEPROM_PRAM_BASE_ADDR+305) 
//����ID0x0020��λ�û㱨����

#define E2_POSITION_REPORT_SCHEME_ADDR      (EEPROM_PRAM_BASE_ADDR+310)     
//����ID0x0021��λ�û㱨����

#define E2_DRIVER_UNLOGIN_REPORT_TIME_ADDR  (EEPROM_PRAM_BASE_ADDR+315)     
//����ID0x0022����ʻԱδ��¼�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60

#define E2_SLEEP_REPORT_TIME_ADDR       (EEPROM_PRAM_BASE_ADDR+320)         
//����ID0x0027������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60

#define E2_EMERGENCY_REPORT_TIME_ADDR       (EEPROM_PRAM_BASE_ADDR+325)     
//����ID0x0028����������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ10

#define E2_ACCON_REPORT_TIME_ADDR       (EEPROM_PRAM_BASE_ADDR+330)             
//0x0029ȱʡʱ��㱨���,��λΪ��

#define E2_ACCON_REPORT_DISTANCE_ADDR   (EEPROM_PRAM_BASE_ADDR+335)     
//0x002Cȱʡ����㱨���,��λΪ��

#define E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ADDR  (EEPROM_PRAM_BASE_ADDR+340)
//0x002D��ʻԱδ��¼�㱨������//��λΪ��//Ĭ��ֵΪ100

#define E2_SLEEP_REPORT_DISTANCE_ADDR       (EEPROM_PRAM_BASE_ADDR+345) 
//����ID0x002e������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ1000

#define E2_EMERGENCY_REPORT_DISTANCE_ADDR   (EEPROM_PRAM_BASE_ADDR+350) 
//����ID0x002f����������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ100

#define E2_CORNER_REPORT_ADDR       (EEPROM_PRAM_BASE_ADDR+355) 
//����ID0x0030���յ㲹���Ƕ�,��λ��/�룬Ĭ��ֵΪ15

#define E2_MONITOR_SERVER_PHONE_ADDR        (EEPROM_PRAM_BASE_ADDR+360)         
//����ID0x0040�����ƽ̨�绰����

#define E2_TERMINAL_RESET_PHONE_ADDR        (EEPROM_PRAM_BASE_ADDR+385)             
//����ID0x0041���ն˸�λ�绰����

#define E2_TERMINAL_DEFAULT_SET_PHONE_ADDR  (EEPROM_PRAM_BASE_ADDR+410) 
//����ID0x0042���ָ��������õ绰����

#define E2_MONITOR_SERVER_SMS_PHONE_ADDR    (EEPROM_PRAM_BASE_ADDR+435) 
//����ID0x0043�����ƽ̨SMS�绰����

#define E2_SMS_TEXT_ALARM_PHONE_ADDR        (EEPROM_PRAM_BASE_ADDR+460)     
//����ID0x0044��SMS�ı������绰����

#define E2_TERMINAL_GET_PHONE_STRATEGY_ADDR (EEPROM_PRAM_BASE_ADDR+485) 
//����ID0x0045���ն˵绰��������

#define E2_MAX_PHONE_TIME_EACH_ADDR     (EEPROM_PRAM_BASE_ADDR+490)         
//����ID0x0046��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������

#define E2_MAX_PHONE_TIME_MONTH_ADDR        (EEPROM_PRAM_BASE_ADDR+495)     
//����ID0x0047��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������

#define E2_MONITOR_PHONE_ADDR           (EEPROM_PRAM_BASE_ADDR+500)         
//����ID0x0048�������绰����

#define E2_MONITOR_SERVER_PRIVILEGE_SMS_ADDR    (EEPROM_PRAM_BASE_ADDR+525) 
//����ID0x0049�����ƽ̨��Ȩ���ź���

#define E2_ALARM_MASK_ADDR          (EEPROM_PRAM_BASE_ADDR+550)     
//����ID0x0050�����������֣�Ĭ��Ϊȫ��

#define E2_ALARM_TEXT_SWITCH_ADDR       (EEPROM_PRAM_BASE_ADDR+555)             
//����ID0x0051�����������ı�SMS���أ�Ĭ��Ϊȫ��

#define E2_ALARM_PHOTO_SWITCH_ADDR      (EEPROM_PRAM_BASE_ADDR+560)         
//����ID0x0052���������㿪�أ�Ĭ��Ϊȫ��

#define E2_ALARM_PHOTO_STORE_FLAG_ADDR      (EEPROM_PRAM_BASE_ADDR+565) 
//����ID0x0053����������洢��־��Ĭ��Ϊȫ��

#define E2_KEY_FLAG_ADDR            (EEPROM_PRAM_BASE_ADDR+570)         
//����ID0x0054���ؼ���־

#define E2_MAX_SPEED_ADDR           (EEPROM_PRAM_BASE_ADDR+575)             
//����ID0x0055������ٶ�

#define E2_OVER_SPEED_KEEP_TIME_ADDR        (EEPROM_PRAM_BASE_ADDR+580)         
//����ID0x0056�����ٳ���ʱ�䣬��λΪ�룬Ĭ��Ϊ10��

#define E2_MAX_DRVIE_TIME_ADDR          (EEPROM_PRAM_BASE_ADDR+585)     
//����ID0x0057��������ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ240��

#define E2_MAX_DRVIE_TIME_ONEDAY_ADDR       (EEPROM_PRAM_BASE_ADDR+590)     
//����ID0x0058�������ۼƼ�ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ0

#define E2_MIN_RELAX_TIME_ADDR          (EEPROM_PRAM_BASE_ADDR+595)     
//����ID0x0059����С��Ϣʱ�䣬��λΪ�룬Ĭ��Ϊ1200

#define E2_MAX_STOP_CAR_TIME_ADDR       (EEPROM_PRAM_BASE_ADDR+600)         
//����ID0x005A���ͣ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0

#define E2_PICTURE_QUALITY_ADDR         (EEPROM_PRAM_BASE_ADDR+605)     
//����ID0x0070��ͼ��/��Ƶ������1~10,1Ϊ���

#define E2_PICTURE_BRIGHTNESS_ADDR      (EEPROM_PRAM_BASE_ADDR+610)     
//����ID0x0071�����ȣ�0~255

#define E2_PICTURE_CONTRAST_ADDR        (EEPROM_PRAM_BASE_ADDR+615)         
//����ID0x0072���Աȶȣ�0~127

#define E2_PICTURE_SATURATION_ADDR      (EEPROM_PRAM_BASE_ADDR+620)         
//����ID0x0073�����Ͷȣ�0~127

#define E2_PICTURE_CHROMA_ADDR          (EEPROM_PRAM_BASE_ADDR+625)     
//����ID0x0074��ɫ�ȣ�0~255

#define E2_CAR_TOTAL_MILE_ADDR          (EEPROM_PRAM_BASE_ADDR+630)     
//����ID0x0080��������̱������0.1km

#define E2_CAR_PROVINCE_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+635)     
//����ID0x0081���������ڵ�ʡ��ID

#define E2_CAR_CITY_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+640)         
//����ID0x0082���������ڵ�����ID

#define E2_CAR_PLATE_NUM_ADDR           (EEPROM_PRAM_BASE_ADDR+645)         
//����ID0x0083��������ͨ�����Ű䷢�Ļ���������

#define E2_CAR_PLATE_COLOR_ADDR         (EEPROM_PRAM_BASE_ADDR+670) 
//����ID0x0084��������ɫ������JT/T415-2006��5.4.12
    

//*************************�Զ������ID��ַ************************
#define E2_LOGINGPRS_PASSWORD_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+675)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x00���Զ��壬��Ȩ��
#define E2_REGISTER_SMS_PHONE_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+700)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x01���Զ��壬ע�����Ϣ����
#define E2_PROGRAM_UPDATA_REQUEST_ID_ADDR   (EEPROM_PRAM_BASE_ADDR+725)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x02���Զ��壬�������������־
#define E2_PROGRAM_UPDATA_VERIFYCODE_ID_ADDR    (EEPROM_PRAM_BASE_ADDR+730)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x03���Զ��壬��������У����
#define E2_PROGRAM_UPDATA_LENGTH_ID_ADDR    (EEPROM_PRAM_BASE_ADDR+735)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x04���Զ��壬������������
#define E2_MANUFACTURE_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+740)//10
//����IDE2_PRAM_BASE_CUSTOM_ID+0x05���Զ��壬������ID��5�ֽ�        
#define E2_DEVICE_ID_ADDR           (EEPROM_PRAM_BASE_ADDR+750)//10 
//����IDE2_PRAM_BASE_CUSTOM_ID+0x06���Զ��壬�����ն�ID��7�ֽ�
#define E2_DEVICE_PHONE_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+760)//20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x07���Զ��壬�ն��ֻ��ţ�6�ֽ�
#define E2_CAR_IDENTIFICATION_CODE_ID_ADDR  (EEPROM_PRAM_BASE_ADDR+780)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x08���Զ��壬����ʶ����
#define E2_CAR_TYPE_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+805)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x09���Զ��壬���Ʒ���
#define E2_CURRENT_DRIVER_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+830)//10
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0A���Զ��壬��ǰ��ʻԱ����
#define E2_CURRENT_LICENSE_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+840)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0B���Զ��壬��ǰ��ʻ֤����
#define E2_FIRMWARE_VERSION_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+865)//10
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0C���Զ��壬�̼��汾��
#define E2_ALL_DRIVER_INFORMATION_ID_ADDR   (EEPROM_PRAM_BASE_ADDR+875)//150-->ע�⣺1015+10----1119+10����
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0D���Զ��壬���м�ʻԱ��Ϣ
#define E2_SPEED_SENSOR_COEF_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+1130)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0E���Զ��壬�ٶȴ�����ϵ��
#define E2_CAR_FEATURE_COEF_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+1135)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0F���Զ��壬��������ϵ��
#define E2_SPEED_SELECT_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1140)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x10���Զ��壬�ٶ�ѡ�񿪹�
#define E2_BACKLIGHT_SELECT_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+1145)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x11���Զ��壬����ѡ�񿪹�
#define E2_FIRST_POWERON_FLAG_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+1150)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x12���Զ��壬ϵͳ��1���ϵ��־
#define E2_ACC_ON_INTERVAL_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1155)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x13���Զ��壬ACC ON ʱ�㱨ʱ����
#define E2_ACC_ON_DISTANCE_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1160)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x14���Զ��壬ACC ON ʱ�㱨������
//(EEPROM_PRAM_BASE_ADDR+1145)              
#define E2_UPDATA_LINK_CONTROL_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1165)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x15���Զ��壬���ӿ���
#define E2_UPDATA_DIAL_NAME_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1170)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x16���Զ��壬�����������ŵ�����
#define E2_UPDATA_DIAL_USER_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1195)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x17���Զ��壬�������������û���
#define E2_UPDATA_DIAL_PASSWORD_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1220)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x18���Զ��壬����������������
#define E2_UPDATA_IP_ID_ADDR                (EEPROM_PRAM_BASE_ADDR+1245)    //25    
//����IDE2_PRAM_BASE_CUSTOM_ID+0x19���Զ��壬��������IP��ַ
#define E2_UPDATA_TCP_PORT_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1270)//25        
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1a���Զ��壬��������TCP�˿�
#define E2_UPDATA_UDP_PORT_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1275)    //5 
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1b���Զ��壬��������UDP�˿�
#define E2_UPDATA_MONITOR_SERVER_CODE_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+1280)//25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1c���Զ��壬�����������ƽ̨��Ȩ��
#define E2_UPDATA_HARDWARE_VERSION_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+1305)    //25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1d���Զ��壬��������Ӳ���汾
#define E2_UPDATA_FIRMWARE_VERSION_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+1330)    //25
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1e���Զ��壬���������̼��汾
#define E2_UPDATA_URL_ID_ADDR               (EEPROM_PRAM_BASE_ADDR+1355)    //100--->ע��EEPROM_PRAM_BASE_ADDR+1445+10--EEPROM_PRAM_BASE_ADDR+1545+10Ϊ������;
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1f���Զ��壬��������URL������ַ
#define E2_UPDATA_TIME_ID_ADDR              (EEPROM_PRAM_BASE_ADDR+1555)    //5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x20���Զ��壬�����������ӵ�ָ��������ʱ��
#define E2_HOT_PHONE_ID_ADDR                (EEPROM_PRAM_BASE_ADDR+1560)    //25            
//����IDE2_PRAM_BASE_CUSTOM_ID+0x21���Զ��壬���ߵ绰
#define E2_UPDATA_MANUFACTURE_ID_ADDR           (EEPROM_PRAM_BASE_ADDR+1585)//10
//����IDE2_PRAM_BASE_CUSTOM_ID+0x22���Զ��壬��������������ID
#define E2_PHONE_CALLBACK_ID_ADDR           (EEPROM_PRAM_BASE_ADDR+1595)    //25    
//����IDE2_PRAM_BASE_CUSTOM_ID+0x23���Զ��壬�绰�ز�����
#define E2_CALLBACK_FLAG_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1620)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x24���Զ��壬�绰�ز���־,1Ϊ����;0Ϊ��ͨͨ��
#define E2_DEVICE_PHONE_BACKUP_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1625)//10
//����IDE2_PRAM_BASE_CUSTOM_ID+0x25���Զ��壬�ն��ֻ��ţ�����
#define E2_LAST_LOCATION_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1635)//20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x26����һ����λλ��
#define E2_PHOTO_SPACE_ID_ADDR                          (EEPROM_PRAM_BASE_ADDR+1655)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x27���Զ��壬����ʱ����
#define E2_PHOTO_NUM_ID_ADDR                            (EEPROM_PRAM_BASE_ADDR+1660)//5                       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x28���Զ��壬��������
#define E2_DOOR_SET_ID_ADDR                             (EEPROM_PRAM_BASE_ADDR+1665)//5                       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x29���Զ��壬��������
#define E2_DEVICE_BACKUP_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1670)//10    
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2a���Զ��壬�����ն�ID��7�ֽ�
#define E2_CDMA_SLEEP_ID_ADDR               (EEPROM_PRAM_BASE_ADDR+1680)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2b���Զ��壬CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
#define E2_PASSWORD_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+1685)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2c���Զ��壬ά���˵�����
#define E2_ELECTRON_RADIUS_ADDR             (EEPROM_PRAM_BASE_ADDR+1690)//5
//����ID0x0031������Χ������׼��������
#define E2_INSTALL_TIME_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+1695)//5     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2d���Զ��壬��ʼ����װ����
#define E2_INSTALL_TIME_FLAG_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1700)//5     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2e���Զ��壬��ʼ����װ����
#define E2_HORN_SET_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+1705)//5     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2f���Զ��壬�����ȣ����룩ѡ��1Ϊ����Ч����1Ϊ����Ч��Ĭ��Ϊ����Ч
#define E2_GPS_CONTROL_ID_ADDR              (EEPROM_PRAM_BASE_ADDR+1710)//5 
//����IDE2_PRAM_BASE_CUSTOM_ID+0x30���Զ��壬ACC OFFʱGPSģ�����,0Ϊ��,1Ϊ��
#define E2_USB_UPDATA_FLAG_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1715)//5 
//����IDE2_PRAM_BASE_CUSTOM_ID+0x31���Զ��壬USB������־
#define E2_TAXIMETER_CONTROL_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1720)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x32���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
#define E2_TOPLIGHT_CONTROL_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1725)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x33���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ��������ʹ��
#define E2_LEDLIGHT_CONTROL_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1730)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x34���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ��������ʹ��
#define E2_HB_IC_DRIVER_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+1735)//120
//����IDE2_PRAM_BASE_CUSTOM_ID+0x35���Զ��壬���÷����б��׼��ʽ��0x0702ָ��ļ�ʻԱ��Ϣ
#define E2_STATUS_PHOTO_SWITCH_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+1855)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x36���Զ��壬״̬���㿪��
#define E2_STATUS_PHOTO_STORE_FLAG_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+1860)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x37���Զ��壬״̬����洢��־//Ĭ��Ϊȫ��
#define E2_RESUME_NOUSED_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1865)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x38���Զ��壬�ָ��ն�Ϊδʹ��״̬
#define E2_NIGHT_TIME0_ID_ADDR              (EEPROM_PRAM_BASE_ADDR+1870)//5     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x39���Զ��壬ҹ��ʱ�䷶Χ,BCD��
#define E2_NIGHT_MAX_DRVIE_TIME0_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+1875)//5     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3a���Զ��壬ҹ�䳬ʱ��ʻ����
#define E2_NIGHT_MAX_SPEED0_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+1880)//5     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3b���Զ��壬ҹ������ֵ

#define E2_IC_MAIN_SERVER_IP_ADDR               (EEPROM_PRAM_BASE_ADDR+1890)//41    
//����ID0x001A//IC����֤��������IP��ַ������
#define E2_IC_MAIN_SERVER_TCP_PORT_ADDR             (EEPROM_PRAM_BASE_ADDR+1931)//5 
//����ID0x001B//IC����֤��������TCP�˿�
#define E2_IC_MAIN_SERVER_UDP_PORT_ADDR             (EEPROM_PRAM_BASE_ADDR+1936)//5 
//����ID0x001C//IC����֤��������UDP�˿�
#define E2_IC_BACKUP_SERVER_IP_ADDR             (EEPROM_PRAM_BASE_ADDR+1941)//41
//����ID0x001D//IC����֤���ݷ�����IP��ַ������
#define E2_SPEED_EARLY_ALARM_DVALUE_ADDR        (EEPROM_PRAM_BASE_ADDR+1982)//3
//����ID0x005B//���ٱ���Ԥ����ֵ����λΪ1/10Km/h��Ĭ��Ϊ50
#define E2_TIRE_EARLY_ALARM_DVALUE_ADDR             (EEPROM_PRAM_BASE_ADDR+1985)//3
//����ID0x005C//ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0��Ĭ��Ϊ30
#define E2_SET_CRASH_ALARM_ADDR                     (EEPROM_PRAM_BASE_ADDR+1988)//3
//����ID0x005D//��ײ������������ 
#define E2_SET_ROLLOVER_ALARM_ADDR              (EEPROM_PRAM_BASE_ADDR+1991)//3
//����ID0x005E//�෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ30 ��
#define E2_TIME_PHOTO_CONTROL_ADDR              (EEPROM_PRAM_BASE_ADDR+1994)//5
//����ID0x0064//��ʱ���տ���
#define E2_MILE_PHOTO_CONTROL_ADDR              (EEPROM_PRAM_BASE_ADDR+1999)//5
//����ID0x0065//�������տ���
#define E2_GPS_SET_MODE_ADDR                        (EEPROM_PRAM_BASE_ADDR+2004)//2
//����ID0x0090//GNSS ��λģʽ��Ĭ��ΪE2_PRAM_BASE_CUSTOM_ID+0x ������λ
#define E2_GPS_SET_BAUD_ADDR                        (EEPROM_PRAM_BASE_ADDR+2006)//2
//����ID0x0091//GNSS �����ʣ�Ĭ��Ϊ0x01 9600
#define E2_GPS_SET_OUTPUT_RATE_ADDR             (EEPROM_PRAM_BASE_ADDR+2008)//2
//����ID0x0092//GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms
#define E2_GPS_SET_GATHER_RATE_ADDR             (EEPROM_PRAM_BASE_ADDR+2010)//5
//����ID0x0093//GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1
#define E2_GPS_SET_UPLOAD_MODE_ADDR             (EEPROM_PRAM_BASE_ADDR+2015)//2
//����ID0x0094//GNSS ģ����ϸ��λ�����ϴ���ʽ
#define E2_GPS_SET_UPLOAD_VALUE_ADDR                (EEPROM_PRAM_BASE_ADDR+2017)//5
//����ID0x0095//GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0

#define E2_CAN1_GATHER_TIME_ADDR                (EEPROM_PRAM_BASE_ADDR+2022)//5
//����ID0x0100//CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
#define E2_CAN1_UPLOAD_TIME_ADDR                (EEPROM_PRAM_BASE_ADDR+2027)//3
//����ID0x0101//CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
#define E2_CAN2_GATHER_TIME_ADDR                (EEPROM_PRAM_BASE_ADDR+2030)//5
//����ID0x0102//CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�
#define E2_CAN2_UPLOAD_TIME_ADDR                (EEPROM_PRAM_BASE_ADDR+2035)//3
//����ID0x0103//CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
#define E2_CAN_SET_ONLY_GATHER_0_ADDR               (EEPROM_PRAM_BASE_ADDR+2038)//10=����+����+У��
//����ID0x0110//CAN ����ID �����ɼ�����0 BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_1_ADDR               (EEPROM_PRAM_BASE_ADDR+2048)//10=����+����+У��
//����ID0x0111//CAN ����ID �����ɼ�����1 BYTE[8]
#define E2_CAR_INIT_MILE_ID_ADDR                        (EEPROM_PRAM_BASE_ADDR+2058)//5
//����ID E2_PRAM_BASE_CUSTOM_ID+0x41���Զ��壬������ʼ����̣�DWORD
#define E2_CAR_ONLY_NUM_ID_ADDR                         (EEPROM_PRAM_BASE_ADDR+2063)//37=����+����+У��
//����IDE2_PRAM_BASE_CUSTOM_ID+0x42���Զ��壬Ψһ�Ա�ţ��ַ�����35�ֽ�
#define E2_DEFINE_ALARM_MASK_WORD_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2100)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x43���Զ��壬�Զ��屨�������֣�DWORD
#define E2_DEFINE_STATUS_CHANGE_PHOTO_ENABLE_ID_ADDR    (EEPROM_PRAM_BASE_ADDR+2105)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x44���Զ��壬�Զ���״̬�仯����ʹ�ܿ��أ�DWORD
#define E2_DEFINE_STATUS_CHANGE_PHOTO_STORE_ID_ADDR (EEPROM_PRAM_BASE_ADDR+2110)//5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x45���Զ��壬�Զ���״̬�仯���մ洢���أ�DWORD
#define E2_UNLOGIN_TTS_PLAY_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+2115)//2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4c���Զ��壬��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ������IC��ʱ��������Ϊ������������Ϊ��
#define E2_TERMINAL_TYPE_ID_ADDR                        (EEPROM_PRAM_BASE_ADDR+2117)//3
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4d���Զ��壬�ն�����
#define E2_GPS_ATTRIBUTE_ID_ADDR                        (EEPROM_PRAM_BASE_ADDR+2120)//2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4e���Զ��壬GNSS����
#define E2_GPRS_ATTRIBUTE_ID_ADDR                       (EEPROM_PRAM_BASE_ADDR+2122)//2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4f���Զ��壬ͨѶģ������
//#define   E2_OVERSPEED_PREALARM_MARGIN_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2124)//5
//����ID���Զ��壬����Ԥ����ֵ,DWORD,��λ0.1km/h,Ĭ��ֵ50
#define E2_OVERSPEED_PREALARM_KEEPTIME_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2129)//5
//����ID���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
#define E2_OVERSPEED_PREALARM_VOICE_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2134)//31
//����ID���Զ��壬����Ԥ������,STRING,���30�ֽ�
#define E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_ADDR   (EEPROM_PRAM_BASE_ADDR+2165)//5
//����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_OVERSPEED_PREALARM_NUMBER_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2170)//5
//����ID���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_ADDR   (EEPROM_PRAM_BASE_ADDR+2175)//5
//����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
//#define   E2_OVERSPEED_ALARM_VALUE_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2180)//5
//����ID���Զ��壬���ٱ���ֵ��DWORD,��λkm/h,Ĭ��ֵ130
//#define   E2_OVERSPEED_ALARM_KEEPTIME_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2185)//5
//����ID���Զ��壬���ٱ�������ʱ�䣬DWORD,��λ��Ĭ��ֵ10
#define E2_OVERSPEED_ALARM_VOICE_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2190)//32
//����ID���Զ��壬���ٱ�������,STRING,���30�ֽ�
#define E2_OVERSPEED_ALARM_GPROUP_TIME_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2222)//5
//����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_OVERSPEED_ALARM_NUMBER_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2227)//5
//����ID���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_OVERSPEED_ALARM_NUMBER_TIME_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2232)//5
//����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
#define E2_OVERSPEED_ALARM_REPORT_TIME_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2237)//5
//����ID���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
#define E2_OVERSPEED_NIGHT_TIME_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+2242)//16
//����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
#define E2_NIGHT_OVERSPEED_PERCENT_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2258)//5
//����ID���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
#define E2_NIGHT_DRIVE_PREALARM_TIME_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2263)//5
//����ID���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
#define E2_NIGHT_DRIVE_PREALARM_VOICE_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2268)//32
//����ID���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�
#define E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_ADDR  (EEPROM_PRAM_BASE_ADDR+2300)//5
//����ID���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
#define E2_NIGHT_DRIVE_PREALARM_NUMBER_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2305)//5
//����ID���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_NIGHT_TIME_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2310)//16
//����ID���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
#define E2_FORBID_DRIVE_PREALARM_TIME_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2326)//5
//����ID���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
#define E2_FORBID_DRIVE_PREALARM_VOICE_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2331)//32
//����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
#define E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID_ADDR (EEPROM_PRAM_BASE_ADDR+2363)//5
//����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_FORBID_DRIVE_PREALARM_NUMBER_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2368)//5
//����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_ALARM_VOICE_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2373)//32
//����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
#define E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID_ADDR    (EEPROM_PRAM_BASE_ADDR+2405)//5
//����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_FORBID_DRIVE_ALARM_NUMBER_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2410)//5
//����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_RUN_SPEED_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2415)//5
//����ID���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
#define E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID_ADDR  (EEPROM_PRAM_BASE_ADDR+2420)//5
//����ID���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
//���ٶȴ��ڽ���ʱ����ʻ�ٶ��ҳ���ʱ��ﵽ����ʱ����ʻ�ٶȳ���ʱ�䣬�϶�Ϊ��ʻ
#define E2_SHUTDOWN_MODE_TIME_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2425)//5
//����ID���Զ��壬�ػ�ģʽʱ�������
#define E2_SHUTDOWN_MODE_POWER_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2430)//3
//����ID���Զ��壬������ߵ�ѹ��
#define E2_TIRED_DRIVE_NIGHT_TIME_ID_ADDR   (EEPROM_PRAM_BASE_ADDR+2435)//16
//����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
#define E2_NIGHT_MAX_DRVIE_TIME_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+2451)//5
//����ID���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
#define E2_NIGHT_MIN_RELAX_TIME_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+2456)//5
//����ID���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
#define E2_TIRED_DRIVE_ALARM_TIME_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2461)//5
//����ID���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
#define E2_TIRED_DRIVE_PREALARM_VOICE_ID_ADDR       (EEPROM_PRAM_BASE_ADDR+2466)//32
//����ID���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
#define E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_ADDR  (EEPROM_PRAM_BASE_ADDR+2498)//5
//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
#define E2_TIRED_DRIVE_PREALARM_NUMBER_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2503)//5
//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_ADDR (EEPROM_PRAM_BASE_ADDR+2508)//5
//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
#define E2_TIRED_DRIVE_ALARM_VOICE_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2513)//32
//����ID���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
#define E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2545)//5
//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
#define E2_TIRED_DRIVE_ALARM_NUMBER_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2550)//5
//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_ADDR    (EEPROM_PRAM_BASE_ADDR+2555)//5
//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
#define E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2560)//5
//����ID���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
#define E2_TIRED_DRIVE_REPORT_TIME_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2565)//5
//����ID���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
#define E2_TIRED_DRIVE_RUN_SPEED_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2570)//5
//����ID���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
#define E2_TIRED_DRIVE_RUN_KEEPTIME_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2575)//5
//����ID���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
#define E2_ROUTE_EXCURSION_ALARM_NUMBER_ID_ADDR         (EEPROM_PRAM_BASE_ADDR+2580)//5
//����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
#define E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID_ADDR     (EEPROM_PRAM_BASE_ADDR+2585)//5
//����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
#define E2_ROUTE_EXCURSION_ALARM_VOICE_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+2590)//32
//����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
#define E2_REPORT_FREQ_EVENT_SWITCH_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+2622)//5
//����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,BIT1λ��1����ʱ����,DWORD
#define E2_RUN_FORBID_CALL_SWITCH_ID_ADDR               (EEPROM_PRAM_BASE_ADDR+2627)//2
//����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
#define E2_SIM_ICCID_NUMBER_ID_ADDR                     (EEPROM_PRAM_BASE_ADDR+2629)//12
//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
#define E2_LOAD_MAX_ID_ADDR                 (EEPROM_PRAM_BASE_ADDR+2641)//5 
//����IDE2_PRAM_BASE_CUSTOM_ID+0x40���Զ��壬�����������
//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
#define E2_LED_PORT_ID_ADDR                 (EEPROM_PRAM_BASE_ADDR+2646)//3
//����ID���Զ��壬����32pin��LED�ӿڽӵ���������,
//2�ֽڣ�bit0:����bit1:����bit2:������bit3:���ش�����bit4-bit15:����;��ӦλΪ1��ʾ�ö˿ڽ�����Ӧ���衣

#define E2_SECOND_MAIN_SERVER_IP_ID_ADDR            (EEPROM_PRAM_BASE_ADDR+2649)//32
//����ID���Զ��壬��2��������������IP��ַ������
//0xF281-0xF282ΪEGS702ʹ��,0xF283ΪE-MIV404ʹ��
#define E2_SECOND_MAIN_SERVER_TCP_PORT_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2681)//5
//����ID���Զ��壬��2��������������TCP�˿�
#define E2_SECOND_MAIN_SERVER_UDP_PORT_ID_ADDR      (EEPROM_PRAM_BASE_ADDR+2686)//5
//����ID���Զ��壬��2��������������UDP�˿�
#define E2_SECOND_BACKUP_SERVER_IP_ID_ADDR          (EEPROM_PRAM_BASE_ADDR+2691)//32
//����ID���Զ��壬��2�����ӱ��ݷ�������ַ//IP������
#define E2_SECOND_LOGINGPRS_PASSWORD_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2723)//32
//����ID���Զ��壬��2�����Ӽ�Ȩ��
#define E2_POS_PORT_ID_ADDR                         (EEPROM_PRAM_BASE_ADDR+2756)//3
//����ID���Զ��壬����16pin�ӿڽӵ���������,
//2�ֽڣ�bit0:����,bit1:����,bit2:��ͨ������λ������;bit3:��ʵ����λ������;bit4-bit15:����;��ӦλΪ1��ʾ�ö˿ڽ�����Ӧ���衣
#define E2_SECOND_ACC_OFF_REPORT_TIME_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2759)//5
//����ID���Զ��壬��2���������߻㱨ʱ����,��λ��,DWORD
#define E2_SECOND_ACC_ON_REPORT_TIME_ID_ADDR        (EEPROM_PRAM_BASE_ADDR+2764)//5
//����ID���Զ��壬��2������ȱʡ�㱨ʱ����,��λ��,DWORD
#define E2_OBD_SWITCH_ID_ADDR                        (EEPROM_PRAM_BASE_ADDR+2769)//2
//����ID���Զ��壬1�ֽڣ�0��ʾδ������OBDģ�飬1��ʾ����
#define E2_LOCK1_ENABLE_FLAG_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+2771)//2
//����ID���Զ��壬1�ֽڣ�0��ʾ��ʹ�ܣ�1��ʾʹ��
#define E2_LOCK2_ENABLE_FLAG_ID_ADDR              (EEPROM_PRAM_BASE_ADDR+2773)//2
//����ID���Զ��壬1�ֽڣ�0��ʾ��ʹ�ܣ�1��ʾʹ��
#define E2_CAN_SET_ONLY_GATHER_2_ADDR               (EEPROM_PRAM_BASE_ADDR+2775)//10=����+����+У��
//����ID0x0112//CAN ����ID �����ɼ�����0 BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_3_ADDR               (EEPROM_PRAM_BASE_ADDR+2785)//10=����+����+У��
//����ID0x0113//CAN ����ID �����ɼ�����1 BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_4_ADDR               (EEPROM_PRAM_BASE_ADDR+2795)//10=����+����+У��
//����ID0x0114//CAN ����ID �����ɼ�����0 BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_5_ADDR               (EEPROM_PRAM_BASE_ADDR+2805)//10=����+����+У��
//����ID0x0115//CAN ����ID �����ɼ�����1 BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_6_ADDR               (EEPROM_PRAM_BASE_ADDR+2815)//10=����+����+У��
//����ID0x0116//CAN ����ID �����ɼ�����0 BYTE[8]
#define E2_CAN_SET_ONLY_GATHER_7_ADDR               (EEPROM_PRAM_BASE_ADDR+2825)//10=����+����+У��
//����ID0x0117//CAN ����ID �����ɼ�����1 BYTE[8]

#define E2_DEEP_SENSORMIN_ID_ADDR                    (EEPROM_PRAM_BASE_ADDR+2835)
//����ID���Զ��壬����������Сֵ��WORD
#define E2_DEEP_SENSORMIN_LENGTH_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+2838)
//����ID���Զ��壬��������Сֵ��Ӧ�ĸ����λ���ף�WORD
#define E2_DEEP_BASE_HEIGHT_ID_ADDR                  (EEPROM_PRAM_BASE_ADDR+2841)
//����ID���Զ��壬�����׼ֵ����λ���ף�WORD
#define E2_DEEP_ARM_LENGTH_ID_ADDR                   (EEPROM_PRAM_BASE_ADDR+2844)
//����ID���Զ��壬��׼�۳�����λ���ף�WORD
#define E2_DEEP_MAX_LENGTH_ID_ADDR                   (EEPROM_PRAM_BASE_ADDR+2847)
//����ID���Զ��壬��׼�߶ȣ���λ���ף�WORD
#define E2_DEEP_SENSORMAX_ID_ADDR                    (EEPROM_PRAM_BASE_ADDR+2850)
//����ID���Զ��壬�����������ֵ��WORD
#define E2_DEEP_SENSORMAX_LENGTH_ID_ADDR             (EEPROM_PRAM_BASE_ADDR+2853)
//����ID���Զ��壬���������ֵ��Ӧ�ĸ����λ���ף�WORD
#define E2_ENGINE_NUM_ID_ADDR                          (EEPROM_PRAM_BASE_ADDR+2856)//??ID,???,?????
//bit7:������;bit6:����Э����;bit5:���깦�ܼ��;bit4:����ƽ̨���;bit3:����Э��3�Ż����;bit2:��;bit1:����Э��1�Ż���⣬���������ò�������Ϊ0
//********************************************************
//********************************************************
//////////����ռ�ʱ����Ҫע��//////////////////
//����Ϊ1���ܿռ�=�������ݳ���(1�ֽ�)+��������(n�ֽ�)+У��(1�ֽ�)
//����Ϊ0���ܿռ�=��������(n�ֽ�)+У��(1�ֽ�)
//********************************************************
//********************************************************
//***************************���¿ռ������һЩ������;��������Ĳ�����***************
#define E2_TIME_PHOTO_CONTROL_BACKUP_ADDR    4082//��ʱ���տ��Ʋ����ı���
#define E2_TIME_PHOTO_CONTROL_BACKUP_LEN     4
#define E2_MILE_PHOTO_CONTROL_BACKUP_ADDR    4086//�������տ��Ʋ����ı���
#define E2_MILE_PHOTO_CONTROL_BACKUP_LEN     4
#define E2_CAMERA_PHOTO_CMD_ADDR      4090//���ڱ�������ͷ����ָ���·�������
#define E2_CAMERA_PHOTO_CMD_LEN       2//����Ϊ2�ֽڳ��ȣ�ע���ַ4092����������У���

//************************************************************************************
 //ע�⣺���ϲ�����ַ���ܳ���4093��EEPROM_PRAM_BASE_ADDRֵΪ10��
#define E2_LAST_ADDR        (0x1000-2)//4096,��������ֽ������Լ�,dxl,2015.3.11������-2
//***************************eeprom��β��ַΪ0x07ff(2047,0x420+991)*********************
//***************************************eeprom�����ĳ���*******************************

#define E2_TERMINAL_HEARTBEAT_LEN       4   
//����ID0x0001���ն��������ͼ������λ�룬Ĭ��ֵ60

#define E2_TCP_ACK_OVERTIME_LEN         4       
//����ID0x0002��TCP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10

#define E2_TCP_RESEND_TIMES_LEN         4           
//����ID0x0003��TCP��Ϣ�ش�������Ĭ��ֵ3

#define E2_UDP_ACK_OVERTIME_LEN         4           
//����ID0x0004��UDP��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ10

#define E2_UDP_RESEND_TIMES_LEN         4           
//����ID0x0005��UDP��Ϣ�ش�������Ĭ��ֵ3

#define E2_SMS_ACK_OVERTIME_LEN         4               
//����ID0x0006��SMS��ϢӦ��ʱʱ�䣬��λ�룬Ĭ��ֵ30

#define E2_SMS_RESEND_TIMES_LEN         4           
//����ID0x0007��SMS��Ϣ�ش�������Ĭ��ֵ3

#define E2_MAIN_SERVER_APN_LEN          20          
//����ID0x0010����������APN

#define E2_MAIN_SERVER_DIAL_NAME_LEN        20          
//����ID0x0011��������������ͨ�Ų����û���

#define E2_MAIN_SERVER_DIAL_PASSWORD_LEN    20          
//����ID0x0012��������������ͨ�Ų�������

#define E2_MAIN_SERVER_IP_LEN           20      
//����ID0x0013������������ַ��IP������

#define E2_BACKUP_SERVER_APN_LEN        20              
//����ID0x0014�����ݷ�����APN������ͨ�Ų��ŷ��ʵ�

#define E2_BACKUP_SERVER_DIAL_NAME_LEN      20  
//����ID0x0015�����ݷ���������ͨ�Ų����û���

#define E2_BACKUP_SERVER_DIAL_PASSWORD_LEN  20      
//����ID0x0016�����ݷ���������ͨ�Ų�������

#define E2_BACKUP_SERVER_IP_LEN         20          
//����ID0x0017�����ݷ�������ַ��IP������

#define E2_MAIN_SERVER_TCP_PORT_LEN     4               
//����ID0x0018����������TCP�˿�
    
#define E2_MAIN_SERVER_UDP_PORT_LEN     4   
//����ID0x0019����������UDP�˿�

#define E2_IC_MAIN_SERVER_IP_LEN        39  
//����ID0x001A//IC����֤��������IP��ַ������

#define E2_IC_MAIN_SERVER_TCP_PORT_LEN      4   
//����ID0x001B//IC����֤��������TCP�˿�

#define E2_IC_MAIN_SERVER_UDP_PORT_LEN      4   
//����ID0x001C//IC����֤��������UDP�˿�

#define E2_IC_BACKUP_SERVER_IP_LEN      39  
//����ID0x001D//IC����֤���ݷ�����IP��ַ������

#define E2_POSITION_REPORT_STRATEGY_LEN     4   
//����ID0x0020��λ�û㱨����

#define E2_POSITION_REPORT_SCHEME_LEN       4       
//����ID0x0021��λ�û㱨����

#define E2_DRIVER_UNLOGIN_REPORT_TIME_LEN   4       
//����ID0x0022����ʻԱδ��¼�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60

#define E2_SLEEP_REPORT_TIME_LEN        4           
//����ID0x0027������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ60

#define E2_EMERGENCY_REPORT_TIME_LEN        4       
//����ID0x0028����������ʱ�㱨ʱ��������λΪ�룬Ĭ��ֵΪ10

#define E2_ACCON_REPORT_TIME_LEN        4   
//0x0029ȱʡʱ��㱨���,��λΪ��

#define E2_ACCON_REPORT_DISTANCE_LEN        4   
//0x002Cȱʡ����㱨���,��λΪ��

#define E2_DRIVER_UNLOGIN_REPORT_DISTANCE_LEN   4   
//����ID0x002D����ʻԱδ��¼�㱨����������λΪ�ף�Ĭ��ֵΪ100

#define E2_SLEEP_REPORT_DISTANCE_LEN        4   
//����ID0x002e������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ1000

#define E2_EMERGENCY_REPORT_DISTANCE_LEN    4   
//����ID0x002f����������ʱ�㱨����������λΪ�ף�Ĭ��ֵΪ100

#define E2_CORNER_REPORT_LEN            4   
//����ID0x0030���յ㲹���Ƕ�,��λ��/�룬Ĭ��ֵΪ15

#define E2_ELECTRON_RADIUS_LEN          2
//����ID0x0031������Χ���뾶

#define E2_MONITOR_SERVER_PHONE_LEN     20          
//����ID0x0040�����ƽ̨�绰����

#define E2_TERMINAL_RESET_PHONE_LEN     20              
//����ID0x0041���ն˸�λ�绰����

#define E2_TERMINAL_DEFAULT_SET_PHONE_LEN   20
//����ID0x0042���ָ��������õ绰����

#define E2_MONITOR_SERVER_SMS_PHONE_LEN     20  
//����ID0x0043�����ƽ̨SMS�绰����

#define E2_SMS_TEXT_ALARM_PHONE_LEN     20  
//����ID0x0044��SMS�ı������绰����

#define E2_TERMINAL_GET_PHONE_STRATEGY_LEN  4
//����ID0x0045���ն˵绰��������

#define E2_MAX_PHONE_TIME_EACH_LEN      4           
//����ID0x0046��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������

#define E2_MAX_PHONE_TIME_MONTH_LEN     4       
//����ID0x0047��ÿ���ͨ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0xffffffff��������

#define E2_MONITOR_PHONE_LEN            20          
//����ID0x0048�������绰����

#define E2_MONITOR_SERVER_PRIVILEGE_SMS_LEN 20  
//����ID0x0049�����ƽ̨��Ȩ���ź���

#define E2_ALARM_MASK_LEN           4       
//����ID0x0050�����������֣�Ĭ��Ϊȫ��

#define E2_ALARM_TEXT_SWITCH_LEN        4               
//����ID0x0051�����������ı�SMS���أ�Ĭ��Ϊȫ��

#define E2_ALARM_PHOTO_SWITCH_LEN       4           
//����ID0x0052���������㿪�أ�Ĭ��Ϊȫ��

#define E2_ALARM_PHOTO_STORE_FLAG_LEN       4   
//����ID0x0053����������洢��־��Ĭ��Ϊȫ��

#define E2_KEY_FLAG_LEN             4           
//����ID0x0054���ؼ���־

#define E2_MAX_SPEED_LEN            4               
//����ID0x0055������ٶ�

#define E2_OVER_SPEED_KEEP_TIME_LEN     4           
//����ID0x0056�����ٳ���ʱ�䣬��λΪ�룬Ĭ��Ϊ10��

#define E2_MAX_DRVIE_TIME_LEN           4       
//����ID0x0057��������ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ240��

#define E2_MAX_DRVIE_TIME_ONEDAY_LEN        4       
//����ID0x0058�������ۼƼ�ʻʱ�����ޣ���λΪ�룬Ĭ��Ϊ0

#define E2_MIN_RELAX_TIME_LEN           4       
//����ID0x0059����С��Ϣʱ�䣬��λΪ�룬Ĭ��Ϊ1200

#define E2_MAX_STOP_CAR_TIME_LEN        4           
//����ID0x005A���ͣ��ʱ�䣬��λΪ�룬Ĭ��Ϊ0

#define E2_SPEED_EARLY_ALARM_DVALUE_LEN     2
//����ID0x005B//���ٱ���Ԥ����ֵ����λΪ1/10Km/h��Ĭ��Ϊ50

#define E2_TIRE_EARLY_ALARM_DVALUE_LEN      2
//����ID0x005C//ƣ�ͼ�ʻԤ����ֵ����λΪ�루s����>0��Ĭ��Ϊ30

#define E2_SET_CRASH_ALARM_LEN              2
//����ID0x005D//��ײ������������ 

#define E2_SET_ROLLOVER_ALARM_LEN       2
//����ID0x005E//�෭�����������ã��෭�Ƕȣ���λ1 �ȣ�Ĭ��Ϊ30 ��

#define E2_TIME_PHOTO_CONTROL_LEN       4
//����ID0x0064//��ʱ���տ���

#define E2_MILE_PHOTO_CONTROL_LEN       4
//����ID0x0065//�������տ���

#define E2_PICTURE_QUALITY_LEN          4       
//����ID0x0070��ͼ��/��Ƶ������1~10,1Ϊ���

#define E2_PICTURE_BRIGHTNESS_LEN       4       
//����ID0x0071�����ȣ�0~255

#define E2_PICTURE_CONTRAST_LEN         4           
//����ID0x0072���Աȶȣ�0~127

#define E2_PICTURE_SATURATION_LEN       4       
//����ID0x0073�����Ͷȣ�0~127

#define E2_PICTURE_CHROMA_LEN           4       
//����ID0x0074��ɫ�ȣ�0~255

#define E2_CAR_TOTAL_MILE_LEN           4       
//����ID0x0080��������̱������0.1km

#define E2_CAR_PROVINCE_ID_LEN          2       
//����ID0x0081���������ڵ�ʡ��ID

#define E2_CAR_CITY_ID_LEN          2           
//����ID0x0082���������ڵ�����ID

#define E2_CAR_PLATE_NUM_LEN            20          
//����ID0x0083��������ͨ�����Ű䷢�Ļ���������

#define E2_CAR_PLATE_COLOR_LEN          1
//����ID0x0084��������ɫ������JT/T415-2006��5.4.12

#define E2_GPS_SET_MODE_LEN             1
//����ID0x0090//GNSS ��λģʽ��Ĭ��ΪE2_PRAM_BASE_CUSTOM_ID+0x ������λ

#define E2_GPS_SET_BAUD_LEN             1
//����ID0x0091//GNSS �����ʣ�Ĭ��Ϊ0x01 9600

#define E2_GPS_SET_OUTPUT_RATE_LEN      1
//����ID0x0092//GNSS ģ����ϸ��λ�������Ƶ��,Ĭ��Ϊ0x01 1000ms

#define E2_GPS_SET_GATHER_RATE_LEN      4
//����ID0x0093//GNSS ģ����ϸ��λ���ݲɼ�Ƶ�ʣ���λΪ�룬Ĭ��Ϊ1

#define E2_GPS_SET_UPLOAD_MODE_LEN      1
//����ID0x0094//GNSS ģ����ϸ��λ�����ϴ���ʽ��Ĭ��Ϊ0x00�����ش洢���ϴ�

#define E2_GPS_SET_UPLOAD_VALUE_LEN     4
//����ID0x0095//GNSS ģ����ϸ��λ�����ϴ�����,Ĭ��Ϊ0

#define E2_CAN1_GATHER_TIME_LEN             4
//����ID0x0100//CAN ����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�

#define E2_CAN1_UPLOAD_TIME_LEN             2
//����ID0x0101//CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�

#define E2_CAN2_GATHER_TIME_LEN             4
//����ID0x0102//CAN ����ͨ��2 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ�

#define E2_CAN2_UPLOAD_TIME_LEN             2
//����ID0x0103//CAN ����ͨ��2 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�

#define E2_CAN_SET_ONLY_GATHER_LEN      8
//����ID0x0110//CAN ����ID �����ɼ�����BYTE[8]


//*****************�Զ������ID�ĳ���****************
#define E2_LOGINGPRS_PASSWORD_ID_LEN        20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x00���Զ��壬��Ȩ��
#define E2_REGISTER_SMS_PHONE_ID_LEN        20  
//����IDE2_PRAM_BASE_CUSTOM_ID+0x01���Զ��壬ע�����Ϣ����
#define E2_PROGRAM_UPDATA_REQUEST_ID_LEN    1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x02���Զ��壬�������������־
#define E2_PROGRAM_UPDATA_VERIFYCODE_ID_LEN 4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x03���Զ��壬��������У����
#define E2_PROGRAM_UPDATA_LENGTH_ID_LEN     4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x04���Զ��壬������������
#define E2_MANUFACTURE_LEN          5
//����IDE2_PRAM_BASE_CUSTOM_ID+0x05���Զ��壬������ID��5�ֽ�        
#define E2_DEVICE_ID_LEN            7       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x06���Զ��壬�����ն�ID��7�ֽ�
#define E2_DEVICE_PHONE_ID_LEN          6
//����IDE2_PRAM_BASE_CUSTOM_ID+0x07���Զ��壬�ն��ֻ��ţ�6�ֽ�
#define E2_CAR_IDENTIFICATION_CODE_ID_LEN   20  
//����IDE2_PRAM_BASE_CUSTOM_ID+0x08���Զ��壬����ʶ����
#define E2_CAR_TYPE_ID_LEN          20              
//����IDE2_PRAM_BASE_CUSTOM_ID+0x09���Զ��壬���Ʒ���
#define E2_CURRENT_DRIVER_ID_LEN        3       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0A���Զ��壬��ǰ��ʻԱ����
#define E2_CURRENT_LICENSE_ID_LEN       20      
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0B���Զ��壬��ǰ��ʻ֤����
#define E2_FIRMWARE_VERSION_ID_LEN      5           
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0C���Զ��壬�̼��汾��
#define E2_ALL_DRIVER_INFORMATION_ID_LEN    150     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0D���Զ��壬���м�ʻԱ��Ϣ
#define E2_SPEED_SENSOR_COEF_ID_LEN     1       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0E���Զ��壬�ٶȴ�����ϵ��
#define E2_CAR_FEATURE_COEF_LEN             3       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x0F���Զ��壬��������ϵ��
#define E2_SPEED_SELECT_LEN         1       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x10���Զ��壬�ٶ�ѡ�񿪹�
#define E2_BACKLIGHT_SELECT_ID_LEN      1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x11���Զ��壬����ѡ�񿪹�
#define E2_FIRST_POWERON_FLAG_ID_LEN        1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x12���Զ��壬ϵͳ��1���ϵ��־
#define E2_ACC_ON_INTERVAL_ID_LEN       4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x13���Զ��壬ACC ON ʱ�㱨ʱ����
#define E2_ACC_ON_DISTANCE_ID_LEN       4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x14���Զ��壬ACC ON ʱ�㱨������
#define E2_UPDATA_LINK_CONTROL_ID_LEN       1   
//����IDE2_PRAM_BASE_CUSTOM_ID+0x15���Զ��壬���ӿ���
#define E2_UPDATA_DIAL_NAME_ID_LEN      20      
//����IDE2_PRAM_BASE_CUSTOM_ID+0x16���Զ��壬�����������ŵ�����
#define E2_UPDATA_DIAL_USER_ID_LEN      20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x17���Զ��壬�������������û���
#define E2_UPDATA_DIAL_PASSWORD_ID_LEN      20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x18���Զ��壬����������������
#define E2_UPDATA_IP_ID_LEN         20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x19���Զ��壬��������IP��ַ
#define E2_UPDATA_TCP_PORT_ID_LEN       2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1a���Զ��壬��������TCP�˿�
#define E2_UPDATA_UDP_PORT_ID_LEN       2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1b���Զ��壬��������UDP�˿�
#define E2_UPDATA_MONITOR_SERVER_CODE_ID_LEN    20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1c���Զ��壬�����������ƽ̨��Ȩ��
#define E2_UPDATA_HARDWARE_VERSION_ID_LEN   20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1d���Զ��壬��������Ӳ���汾
#define E2_UPDATA_FIRMWARE_VERSION_ID_LEN   20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1e���Զ��壬���������̼��汾
#define E2_UPDATA_URL_ID_LEN            100//dxl,2013.6.11ԭ��Ϊ200
//����IDE2_PRAM_BASE_CUSTOM_ID+0x1f���Զ��壬��������URL������ַ
#define E2_UPDATA_TIME_ID_LEN           2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x20���Զ��壬�����������ӵ�ָ��������ʱ��
#define E2_HOT_PHONE_ID_LEN         20
//E2_PRAM_BASE_CUSTOM_ID+0x21����ID���Զ��壬���ߵ绰
#define E2_UPDATA_MANUFACTURE_ID_LEN        10
//E2_PRAM_BASE_CUSTOM_ID+0x22����ID���Զ��壬��������������ID
#define E2_PHONE_CALLBACK_ID_LEN        20          
//����IDE2_PRAM_BASE_CUSTOM_ID+0x23���Զ��壬�绰�ز�����
#define E2_CALLBACK_FLAG_ID_LEN         1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x24���Զ��壬�绰�ز���־,1Ϊ����;0Ϊ��ͨͨ��
#define E2_DEVICE_PHONE_BACKUP_ID_LEN       6
//����IDE2_PRAM_BASE_CUSTOM_ID+0x25���Զ��壬�ն��ֻ��ţ�����
#define E2_LAST_LOCATION_ID_LEN         20
//����IDE2_PRAM_BASE_CUSTOM_ID+0x26���Զ��壬���һ������λ����Ϣ
#define E2_PHOTO_SPACE_ID_LEN                   4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x27���Զ��壬����ʱ����
#define E2_PHOTO_NUM_ID_LEN                     4                     
//����IDE2_PRAM_BASE_CUSTOM_ID+0x28���Զ��壬��������
#define E2_DOOR_SET_LEN                         1  //��ͨV2.81�汾���ȿ���Ϊ4                   
//����IDE2_PRAM_BASE_CUSTOM_ID+0x29���Զ��壬��������
#define E2_DEVICE_BACKUP_ID_LEN         7       
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2a���Զ��壬�����ն�ID��7�ֽ�
#define E2_CDMA_SLEEP_ID_LEN            1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2b
#define E2_PASSWORD_ID_LEN          4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2c
#define E2_INSTALL_TIME_ID_LEN          4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2d
#define E2_INSTALL_TIME_FLAG_LEN        1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2e
#define E2_HORN_SET_ID_LEN          1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x2f
#define E2_GPS_CONTROL_LEN          1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x30
#define E2_USB_UPDATA_FLAG_ID_LEN       1   
//����IDE2_PRAM_BASE_CUSTOM_ID+0x31���Զ��壬USB������־
#define E2_TAXIMETER_CONTROL_LEN        1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x32���Զ��壬�Ƽ������ܿ���,0Ϊ�ر�,1Ϊ����
#define E2_TOPLIGHT_CONTROL_ID_LEN      1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x33���Զ��壬���ƹ��ܿ���,0Ϊ�ر�,1Ϊ��������ʹ��
#define E2_LEDLIGHT_CONTROL_ID_LEN      1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x34���Զ��壬LED��湦�ܿ���,0Ϊ�ر�,1Ϊ��������ʹ��
#define E2_HB_IC_DRIVER_ID_LEN          120//
//����IDE2_PRAM_BASE_CUSTOM_ID+0x35���Զ��壬���÷����б��׼��ʽ��0x0702ָ��ļ�ʻԱ��Ϣ
#define E2_STATUS_PHOTO_SWITCH_ID_LEN       4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x36���Զ��壬״̬���㿪��
#define E2_STATUS_PHOTO_STORE_FLAG_ID_LEN   4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x37���Զ��壬״̬����洢��־//Ĭ��Ϊȫ��
#define E2_RESUME_NOUSED_ID_LEN         1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x38���Զ��壬�ָ��ն�Ϊδʹ��״̬
#define E2_NIGHT_TIME0_ID_LEN           4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x39���Զ��壬ҹ��ʱ�䷶Χ,BCD��
#define E2_NIGHT_MAX_DRVIE_TIME0_ID_LEN     4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3a���Զ��壬ҹ�䳬ʱ��ʻ����
#define E2_NIGHT_MAX_SPEED0_ID_LEN      4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3b���Զ��壬ҹ������ֵ
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3C���Զ��壬V1��13����Э��VTK�������ź�SOS�����л���VTK�ã��б��޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3D���Զ��壬V1��13����Э��VTK�����Զ��������ʱ�䣬VTK�ã��б��޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3E���Զ��壬V1��13����Э��VTK��������ʱ�䣬VTK�ã��б��޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x3F���Զ��壬V1��13����Э��VTK�����𶯼��ʱ�䣬VTK�ã��б��޴���
#define E2_LOAD_MAX_ID_LEN          4   //����IDE2_PRAM_BASE_CUSTOM_ID+0x40��//����ID���Զ��壬����������ƣ�������ֵ���г��ر�������λΪkg��4�ֽ�
#define E2_CAR_INIT_MILE_ID_LEN                     4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x41���Զ��壬������ʼ����̣�DWORD
#define E2_CAR_ONLY_NUM_ID_LEN                      35
//����IDE2_PRAM_BASE_CUSTOM_ID+0x42���Զ��壬Ψһ�Ա�ţ��ַ�����35�ֽ�
#define E2_DEFINE_ALARM_MASK_WORD_ID_LEN        4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x43���Զ��壬�Զ��屨�������֣�DWORD
#define E2_DEFINE_STATUS_CHANGE_PHOTO_ENABLE_ID_LEN 4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x44���Զ��壬�Զ���״̬�仯����ʹ�ܿ��أ�DWORD
#define E2_DEFINE_STATUS_CHANGE_PHOTO_STORE_ID_LEN  4
//����IDE2_PRAM_BASE_CUSTOM_ID+0x45���Զ��壬�Զ���״̬�仯���մ洢���أ�DWORD
//����IDE2_PRAM_BASE_CUSTOM_ID+0x46����С����Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x47���������Ƕȣ��Զ��壬EGS702�ã�EGS701�޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x48������ٶ���ֵ���Զ��壬EGS702�ã�EGS701�޴���
//����IDE2_PRAM_BASE_CUSTOM_ID+0x49��������ʼ�ͺģ��Զ��壬�ϵ�EGS701�ã�EGS701�޴���
#define E2_UNLOGIN_TTS_PLAY_ID_LEN          1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4c���Զ��壬��ʻԱδǩ���������ѣ�0Ϊ�أ�1Ϊ������IC��ʱ��������Ϊ������������Ϊ��
#define E2_TERMINAL_TYPE_ID_LEN         2
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4d���Զ��壬�ն�����
#define E2_GPS_ATTRIBUTE_ID_LEN         1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4e���Զ��壬GNSS����
#define E2_GPRS_ATTRIBUTE_ID_LEN            1
//����IDE2_PRAM_BASE_CUSTOM_ID+0x4f���Զ��壬ͨѶģ������
//#define   E2_OVERSPEED_PREALARM_MARGIN_ID_LEN     4//5
//����ID���Զ��壬����Ԥ����ֵ,DWORD,��λ0.1km/h,Ĭ��ֵ50
#define E2_OVERSPEED_PREALARM_KEEPTIME_ID_LEN       4//5
//����ID���Զ��壬����Ԥ������ʱ��,DWORD,��λ��,Ĭ��ֵ3
#define E2_OVERSPEED_PREALARM_VOICE_ID_LEN      30//32
//����ID���Զ��壬����Ԥ������,STRING,���30�ֽ�
#define E2_OVERSPEED_PREALARM_GPROUP_TIME_ID_LEN    4//5
//����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_OVERSPEED_PREALARM_NUMBER_ID_LEN     4//5
//����ID���Զ��壬����Ԥ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_OVERSPEED_PREALARM_NUMBER_TIME_ID_LEN    4//5
//����ID���Զ��壬����Ԥ��ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
//#define   E2_OVERSPEED_ALARM_VALUE_ID_LEN         4//5
//����ID���Զ��壬���ٱ���ֵ��DWORD,��λkm/h,Ĭ��ֵ130
//#define   E2_OVERSPEED_ALARM_KEEPTIME_ID_LEN      4//5
//����ID���Զ��壬���ٱ�������ʱ�䣬DWORD,��λ��Ĭ��ֵ10
#define E2_OVERSPEED_ALARM_VOICE_ID_LEN         30//32
//����ID���Զ��壬���ٱ�������,STRING,���30�ֽ�
#define E2_OVERSPEED_ALARM_GPROUP_TIME_ID_LEN       4//5
//����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_OVERSPEED_ALARM_NUMBER_ID_LEN        4//5
//����ID���Զ��壬���ٱ���ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_OVERSPEED_ALARM_NUMBER_TIME_ID_LEN       4//5
//����ID���Զ��壬���ٱ���ÿ����ʾʱ����,DWORD,��λ��,Ĭ��ֵ10
#define E2_OVERSPEED_ALARM_REPORT_TIME_ID_LEN       4//5
//����ID���Զ��壬���ٱ���λ���ϱ�ʱ����,DWORD,��λ��,Ĭ��ֵ0
#define E2_OVERSPEED_NIGHT_TIME_ID_LEN          14//16
//����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
#define E2_NIGHT_OVERSPEED_PERCENT_ID_LEN       4//5
//����ID���Զ��壬ҹ�䳬�ٱ�����Ԥ���ٷֱ�,DWORD,Ĭ��ֵ80
#define E2_NIGHT_DRIVE_PREALARM_TIME_ID_LEN     4//5
//����ID���Զ��壬ҹ����ʻԤ��ʱ��,,DWORD,��λ����,Ĭ��ֵ10
#define E2_NIGHT_DRIVE_PREALARM_VOICE_ID_LEN        30//32
//����ID���Զ��壬ҹ����ʻԤ������,STRING,���30�ֽ�
#define E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID_LEN   4//5
//����ID���Զ��壬ҹ����ʻԤ����ʾʱ����(ÿ��),DWORD,��λ����,Ĭ��ֵ5
#define E2_NIGHT_DRIVE_PREALARM_NUMBER_ID_LEN       4//5
//����ID���Զ��壬ҹ����ʻԤ����ʾ����,,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_NIGHT_TIME_ID_LEN       14//16
//����ID���Զ��壬ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0
#define E2_FORBID_DRIVE_PREALARM_TIME_ID_LEN        4//5
//����ID���Զ��壬����ʱ�ε���������ǰʱ��,DWORD,��λ����,Ĭ��ֵ30
#define E2_FORBID_DRIVE_PREALARM_VOICE_ID_LEN       30//32
//����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
#define E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID_LEN  4//5
//����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_FORBID_DRIVE_PREALARM_NUMBER_ID_LEN      4//5
//����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_ALARM_VOICE_ID_LEN      30//32
//����ID���Զ��壬����ʱ�ε�����������,STRING,���30�ֽ�
#define E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID_LEN     4//5
//����ID���Զ��壬����ʱ������ʱ����,DWORD,��λ����,Ĭ��ֵ5
#define E2_FORBID_DRIVE_ALARM_NUMBER_ID_LEN     4//5
//����ID���Զ��壬����ʱ�ε������Ѳ����Ĵ���,DWORD,Ĭ��ֵ3
#define E2_FORBID_DRIVE_RUN_SPEED_ID_LEN        4//5
//����ID���Զ��壬����ʱ����ʻ�ٶ�,��λkm/h,DWORD,Ĭ��ֵ20
#define E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID_LEN   4//5
//����ID���Զ��壬����ʱ����ʻ�ٶȳ���ʱ��,��λ����,DWORD,Ĭ��ֵ5��
#define E2_SHUTDOWN_MODE_TIME_ID_LEN            4//5
//����ID���Զ��壬�ػ�ģʽʱ�������
#define E2_SHUTDOWN_MODE_POWER_ID_LEN           2//3
//����ID���Զ��壬������ߵ�ѹ��
#define E2_TIRED_DRIVE_NIGHT_TIME_ID_LEN        14//16
//����ID���Զ��壬ҹ��ʱ��,BYTE[14],��STRING����,Ĭ��ֵȫ0,ҹ�䳬ʱʹ��
#define E2_NIGHT_MAX_DRVIE_TIME_ID_LEN          4//5
//����ID���Զ��壬ҹ��������ʻʱ������,��λ��,DWORD,Ĭ��ֵ7200
#define E2_NIGHT_MIN_RELAX_TIME_ID_LEN          4//5
//����ID���Զ��壬ҹ����С��Ϣʱ��,��λ��,DWORD,Ĭ��ֵ1200
#define E2_TIRED_DRIVE_ALARM_TIME_ID_LEN        4//5
//����ID���Զ��壬��ʱ��ʻ������ʾ������Чʱ��,��λ����,DWORD,Ĭ��ֵ30,����Ҫ��
#define E2_TIRED_DRIVE_PREALARM_VOICE_ID_LEN        30//32
//����ID���Զ��壬��ʱ��ʻԤ����ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
#define E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID_LEN   4//5
//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
#define E2_TIRED_DRIVE_PREALARM_NUMBER_ID_LEN       4//5
//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_TIRED_DRIVE_PREALARM_NUMBER_TIME_ID_LEN  4//5
//����ID���Զ��壬��ʱ��ʻԤ��ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
#define E2_TIRED_DRIVE_ALARM_VOICE_ID_LEN       30//32
//����ID���Զ��壬��ʱ��ʻ������ʾ����,STRING,���30�ֽ�,Ĭ��ֵ����ʱ��ʻ
#define E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID_LEN      4//5
//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ����,DWORD,Ĭ��ֵ5
#define E2_TIRED_DRIVE_ALARM_NUMBER_ID_LEN      4//5
//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾ����,DWORD,Ĭ��ֵ3
#define E2_TIRED_DRIVE_ALARM_NUMBER_TIME_ID_LEN     4//5
//����ID���Զ��壬��ʱ��ʻ����ÿ����ʾʱ����,��λ��,DWORD,Ĭ��ֵ10
#define E2_TIRED_DRIVE_REPORT_OPEN_TIME_ID_LEN      4//5
//����ID���Զ��壬��ʱ��ʻ����λ���ϱ�����ʱ��,��λ��,DWORD,Ĭ��ֵ0xffffffff,������,��ͨΪ20����
#define E2_TIRED_DRIVE_REPORT_TIME_ID_LEN       4//5
//����ID���Զ��壬��ʱ��ʻ����λ���ϱ�ʱ����,��λ��,DWORD,Ĭ��ֵ300,��ͨΪ5����
#define E2_TIRED_DRIVE_RUN_SPEED_ID_LEN         4//5
//����ID���Զ��壬��ʱ��ʻ��ʻ�ж��ٶ�,��λkm/h,DWORD,Ĭ��ֵ5,
#define E2_TIRED_DRIVE_RUN_KEEPTIME_ID_LEN      4//5
//����ID���Զ��壬��ʱ��ʻ��ʻ�ж�����ʱ��,��λ��,DWORD,Ĭ��ֵ10,
#define E2_ROUTE_EXCURSION_ALARM_NUMBER_ID_LEN          4//5
//����ID���Զ��壬ƫ����· ����������0����������255���������� DWORD,Ĭ��ֵ3
#define E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID_LEN      4//5
//����ID���Զ��壬ƫ����·��������ʱ����,DWORD,Ĭ��ֵ5
#define E2_ROUTE_EXCURSION_ALARM_VOICE_ID_LEN           30//32
//����ID���Զ��壬ƫ����·��������,STRING,���30�ֽ�,Ĭ��ֵ���㰴�涨��·��ʻ
#define E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN              4//5
//����ID���Զ��壬�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ���,BIT1λ��1����ʱ����DWORD
#define E2_RUN_FORBID_CALL_SWITCH_ID_LEN               1//2
//����ID���Զ��壬��ʻ�н�ֹ�绰���ܿ��أ�0����ͨ����1��ֹͨ����ͨ�ð�Ĭ��Ϊ0���������Ű�Ĭ��Ϊ1
#define E2_SIM_ICCID_NUMBER_ID_LEN                     10//12
//����ID���Զ��壬SIM����ICCID����,BCD[10],ֻ��
#define E2_LED_PORT_ID_LEN                             2//3
//����ID���Զ��壬����32pin��LED�ӿڽӵ���������,//2�ֽ�
#define E2_SECOND_MAIN_SERVER_IP_ID_LEN             30
//����ID���Զ��壬��2��������������IP��ַ������
//0xF281-0xF282ΪEGS702ʹ��,0xF283ΪE-MIV404ʹ��
#define E2_SECOND_MAIN_SERVER_TCP_PORT_ID_LEN       4
//����ID���Զ��壬��2��������������TCP�˿�
#define E2_SECOND_MAIN_SERVER_UDP_PORT_ID_LEN       4
//����ID���Զ��壬��2��������������UDP�˿�
#define E2_SECOND_BACKUP_SERVER_IP_ID_LEN           20
//����ID���Զ��壬��2�����ӱ��ݷ�������ַ//IP������
#define E2_SECOND_LOGINGPRS_PASSWORD_ID_LEN         20
//����ID���Զ��壬��2�����Ӽ�Ȩ��
#define E2_POS_PORT_ID_LEN                             2//3
//����ID���Զ��壬����16pin�ӿڽӵ���������,//2�ֽڣ�bit0:����,bit1:����,bit2:��ͨ������λ������;bit3:��ʵ����λ������;bit4-bit15:����;��ӦλΪ1��ʾ�ö˿ڽ�����Ӧ���衣
#define E2_SECOND_ACC_OFF_REPORT_TIME_ID_LEN        4//5
//����ID���Զ��壬��2���������߻㱨ʱ����,��λ��,DWORD
#define E2_SECOND_ACC_ON_REPORT_TIME_ID_LEN        4//5
#define E2_OBD_SWITCH_ID_LEN                      1//2
//����ID���Զ��壬1�ֽڣ�0��ʾδ������OBDģ�飬1��ʾ����
#define E2_LOCK1_ENABLE_FLAG_ID_LEN             1
//����ID���Զ��壬1�ֽڣ�0��ʾ��ʹ�ܣ�1��ʾʹ��
#define E2_LOCK2_ENABLE_FLAG_ID_LEN             1
//����ID���Զ��壬1�ֽڣ�0��ʾ��ʹ�ܣ�1��ʾʹ��
#define E2_DEEP_SENSORMIN_ID_LEN                2
//����ID���Զ��壬����������Сֵ��WORD
#define E2_DEEP_SENSORMIN_LENGTH_ID_LEN         2
//����ID���Զ��壬��������Сֵ��Ӧ�ĸ����λ���ף�WORD
#define E2_DEEP_BASE_HEIGHT_ID_LEN              2
//����ID���Զ��壬�����׼ֵ����λ���ף�WORD
#define E2_DEEP_ARM_LENGTH_ID_LEN               2
//����ID���Զ��壬��׼�۳�����λ���ף�WORD
#define E2_DEEP_MAX_LENGTH_ID_LEN               2
//����ID���Զ��壬��׼�߶ȣ���λ���ף�WORD
#define E2_DEEP_SENSORMAX_ID_LEN                2
//����ID���Զ��壬�����������ֵ��WORD
#define E2_DEEP_SENSORMAX_LENGTH_ID_LEN         2
//����ID���Զ��壬���������ֵ��Ӧ�ĸ����λ���ף�WORD
#define E2_ENGINE_NUM_ID_LEN                      16//??ID,???,?????,??16???
//bit7:������;bit6:����Э����;bit5:���깦�ܼ��;bit4:����ƽ̨���;bit3:����Э��3�Ż����;bit2:��;bit1:����Э��1�Ż���⣬���������ò�������Ϊ0

//*************��������***************
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
u16 EepromPram_ReadPram(u32 PramID, u8 *pBuffer);
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
u16 EepromPram_WritePram(u32 PramID, u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//��������  :EepromPram_UpdateVariable(PramID)
//����      :����PramID������Ӧ�ı���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ǧ��ע��:EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID)���������ã�ֻ����������E2_CAR_TOTAL_MILE_ID�����              
*********************************************************************/
void EepromPram_UpdateVariable(u32 PramID);
/*********************************************************************
//��������  :EepromPram_DefaultSet(void)
//����      :ϵͳĬ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :1.ϵͳ��1������ʱ�����;2.�յ��ָ�������������ʱ�����
*********************************************************************/
void EepromPram_DefaultSet(void);
/*********************************************************************
//��������	:EepromPram_GBTestSet(void)
//����		:�������ʱ��Ĭ������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:ʹ�ûָ�Ĭ�ϳ�������Ҳ�Ὣ��������Ϊ������ʾ
*********************************************************************/
void EepromPram_GBTestSet(void);
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
void EepromPram_BBGNTestSet(void);
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
void EepromPram_DefaultSetRead(void);
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
void EepromPram_EraseAllPram(void);
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
void EepromPram_WriteAllPram(void);


/*********************************************************************
//��������  :EepromPram_SystemInitSet(void)
//����      :eeprom����ϵͳ��ʼ������(��Ҫ��IP,PORT,APN)
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  ;
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void EepromPram_SystemInitSet(void);
#endif
