#ifndef __IO_H
#define __IO_H

//*************�ļ�����***************
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "GPIOControl.h"
//*************�궨��****************

extern u8   BeepOnOffFlag;
extern u8   GprsLedOnOffFlag;
extern u8   GpsLedOnOffFlag;

extern u32  BeepOnTime;
extern u32  BeepOffTime;
extern u32  GprsLedOnTime;
extern u32  GprsLedOffTime;
extern u32  GpsLedOnTime;
extern u32  GpsLedOffTime;

#define LOAD_SENSOR_SHORT_VALUE 20//��·ֵ,��λΪ0.1V
#define LOAD_SENSOR_SHUT_VALUE  45//��·ֵ,��λΪ0.1V
#define LOAD_SENSOR_FULL_MIN_VALUE 20//������Сֵ,��λΪ0.1V
#define LOAD_SENSOR_FULL_MAX_VALUE 32//�������ֵ,��λΪ0.1V

//************����������************

#define  BEEP_OFF()     GpioOutOff(BUZZER);BeepOnOffFlag = 0;BeepOffTime=Timer_Val()
//*************����������*********
#define SPEAKER_ON()            GpioOutOn(V_COL);
#define SPEAKER_OFF()           GpioOutOff(V_COL);
//*************���ƫ�õ�ѹ����*********
#define MIC_BASE_ON()       GPIO_WriteBit(GPIOD,GPIO_Pin_5,Bit_SET);MicBaseOnOffFlag = 1
#define MIC_BASE_OFF()      GPIO_WriteBit(GPIOD,GPIO_Pin_5,Bit_RESET);MicBaseOnOffFlag = 0
//*************���2����*********
#define OUTPUT2_SET()       GPIO_WriteBit(GPIOF,GPIO_Pin_2,Bit_SET)
#define OUTPUT2_RESET()     GPIO_WriteBit(GPIOF,GPIO_Pin_2,Bit_RESET)
//*************SD����Դ��ѹ����*********
#define  SD_POWER_ON()      GPIO_WriteBit(GPIOG,GPIO_Pin_3,Bit_RESET)
#define  SD_POWER_OFF()     GPIO_WriteBit(GPIOG,GPIO_Pin_3,Bit_SET)
//*************��·����*********
#define  OIL_CUT_ON()       GpioOutOff(COIL)  //ͨ��
#define  OIL_CUT_OFF()      GpioOutOn(COIL)   //����,�ߵ�ƽ���
//**************12V/24V�������***************
#define   PWR_TYPE_24V()    GpioOutOn(PWR_TYPE)
#define   PWR_TYPE_12V()    GpioOutOff(PWR_TYPE)
//��˷��������
typedef enum
{
    MIC_BIT_TALK = 0,//bit0����ͨ������
    MIC_BIT_RECORD,//bit1����¼������
    MIC_BIT_TEST,//bit2���������
    MIC_BIT_MAX,//��ʾ���һλ,��ʵ������
}MIC_BIT;
//��������������
typedef enum
{
    SPEAKER_BIT_TALK = 0,//bit0����ͨ������
    SPEAKER_BIT_TTS,//bit1����TTS����
    SPEAKER_BIT_MAX,//��ʾ���һλ,��ʵ������
}SPEAKER_BIT;
//���ٿ�������
typedef enum
{
    OVER_SPEED_BIT_ORDINARY = 0,//bit0������ͨ�ĳ���
    OVER_SPEED_BIT_ROUND,//bit1����Բ��������
	  OVER_SPEED_BIT_RECTANGLE,//bit2�������������
	  OVER_SPEED_BIT_POLYGON,//bit3��������������
    OVER_SPEED_BIT_ROUTE,//bit4������·����
    OVER_SPEED_BIT_MAX,//��ʾ���һλ,��ʵ������
}OVER_SPEED_BIT;
//��ʱ��������
typedef enum
{
    OVER_TIME_BIT_ORDINARY = 0,//bit0������ͨ�ĳ�ʱ
    OVER_TIME_BIT_NIGHT,//bit1����ҹ�䳬ʱ
    OVER_TIME_BIT_MAX,//��ʾ���һλ,��ʵ������
}OVER_TIME_BIT;
//��ʱԤ����������
typedef enum
{
    PRE_OVER_TIME_BIT_ORDINARY = 0,//bit0������ͨ�ĳ�ʱ
    PRE_OVER_TIME_BIT_NIGHT,//bit1����ҹ�䳬ʱ
    PRE_OVER_TIME_BIT_MAX,//��ʾ���һλ,��ʵ������
}PRE_OVER_TIME_BIT;
//״̬λ
//¼��������������
typedef enum
{
    RECORD_BIT_TIME = 0,//bit0����ʱ¼����һֱѭ��¼��
    RECORD_BIT_EMERGENCY,//bit1�����������¼��
    RECORD_BIT_TAXI,//bit2����ճ�ת�س�¼��
    RECORD_BIT_MAX,//��ʾ���һλ,��ʵ������
}RECORD_BIT;
//״̬λ
typedef enum
{
    STATUS_BIT_ACC=0,           // 0.0��ACC �أ�1�� ACC ��
    STATUS_BIT_NAVIGATION,      // 1.0��δ��λ��1����λ
    STATUS_BIT_SOUTH,           // 2.0����γ��1����γ
    STATUS_BIT_EAST,            // 3.0��������1������
    STATUS_BIT_STOP_WORK,       // 4.0����Ӫ״̬��1��ͣ��״̬
    STATUS_BIT_ENCRYPT,         // 5.0����γ��δ�����ܲ�����ܣ�1����γ���Ѿ����ܲ������
    STATUS_BIT_RESERVER6,       // 6.����
    STATUS_BIT_RESERVER7,       // 7.����
    STATUS_BIT_FULL_LOAD1,      // 8
    STATUS_BIT_FULL_LOAD2,      // 9.[9:8]��ʾ��00�ճ�,01����;10����;11����
    STATUS_BIT_OIL_CUT,         // 10.0��������·������1��������·�Ͽ�
    STATUS_BIT_CIRCUIT_CUT,     // 11.0��������·������1��������·�Ͽ�
    STATUS_BIT_DOOR_LOCK,       // 12.0�����Ž�����1�����ż���
    STATUS_BIT_DOOR1,           // 13.0����1 �أ�1����1 ����ǰ�ţ�      
    STATUS_BIT_DOOR2,           // 14.0����2 �أ�1����2 �������ţ�
    STATUS_BIT_DOOR3,           // 15.0����3 �أ�1����3 �������ţ�
    STATUS_BIT_DOOR4,           // 16.0��0����4 �أ�1����4 ������ʻϯ�ţ�
    STATUS_BIT_DOOR5,           // 17.0����5 �أ�1����5 �����Զ��壩
    STATUS_BIT_GPS,             // 18.0��δʹ��GPS ���ǽ��ж�λ��1��ʹ��GPS ���ǽ��ж�λ
    STATUS_BIT_COMPASS,         // 19.0��δʹ�ñ������ǽ��ж�λ��1��ʹ�ñ������ǽ��ж�λ
    STATUS_BIT_GLONASS,         // 20.0��δʹ��GLONASS ���ǽ��ж�λ��1��ʹ��GLONASS ���ǽ��ж�λ
    STATUS_BIT_GALILEO,         // 21.0��δʹ��Galileo ���ǽ��ж�λ��1��ʹ��Galileo ���ǽ��ж�λ
    STATUS_BIT_RESERVER22,      // 22.����
    STATUS_BIT_RESERVER23,      // 23.����
    STATUS_BIT_RESERVER24,      // 24.����
    STATUS_BIT_RESERVER25,      // 25.����
    STATUS_BIT_RESERVER26,      // 26.����
    STATUS_BIT_RESERVER27,      // 27.����
    STATUS_BIT_RESERVER28,      // 28.����
    STATUS_BIT_RESERVER29,      // 29.����
    STATUS_BIT_RESERVER30,      // 30.����
    STATUS_BIT_RESERVER31,      // 31.����
    STATUS_BIT_MAX,             // 32
}STATUS_BIT;
//��չ����״̬λ
typedef enum
{
    CAR_STATUS_BIT_NEAR_LIGHT=0,    // 0    �����
    CAR_STATUS_BIT_FAR_LIGHT,       // 1    Զ���
    CAR_STATUS_BIT_RIGHT_LIGHT,     // 2    ��ת���
    CAR_STATUS_BIT_LEFT_LIGHT,      // 3    ��ת���
    CAR_STATUS_BIT_BRAKE,           // 4    �ƶ���ɲ����
    CAR_STATUS_BIT_REVERSE,         // 5    ����
    CAR_STATUS_BIT_FOG_LIGTH,       // 6    ���
    CAR_STATUS_BIT_SIDE_LIGHT,      // 7    ʾ�ȵ�
    CAR_STATUS_BIT_HORN,            // 8    ����
    CAR_STATUS_BIT_AIR_CONDITION,   // 9    �յ�
    CAR_STATUS_BIT_NEUTRAL,         // 10   �յ�
    CAR_STATUS_BIT_DERAILLEUR,      // 11   ����������
    CAR_STATUS_BIT_ABS,             // 12   ABS����
    CAR_STATUS_BIT_HEATER,          // 13   ����������              
    CAR_STATUS_BIT_CLUTCH,          // 14   ���������
    CAR_STATUS_BIT_RESERVER15,      // 15   15-31����
    CAR_STATUS_BIT_RESERVER16,
    CAR_STATUS_BIT_RESERVER17,
    CAR_STATUS_BIT_RESERVER18,
    CAR_STATUS_BIT_RESERVER19,
    CAR_STATUS_BIT_RESERVER20,
    CAR_STATUS_BIT_RESERVER21,
    CAR_STATUS_BIT_RESERVER22,
    CAR_STATUS_BIT_RESERVER23,
    CAR_STATUS_BIT_RESERVER24,
    CAR_STATUS_BIT_RESERVER25,
    CAR_STATUS_BIT_RESERVER26,
    CAR_STATUS_BIT_RESERVER27,
    CAR_STATUS_BIT_RESERVER28,
    CAR_STATUS_BIT_RESERVER29,
    CAR_STATUS_BIT_RESERVER30,
    CAR_STATUS_BIT_RESERVER31,
    CAR_STATUS_BIT_MAX,
}CAR_STATUS_BIT;
//IO״̬λ
typedef enum
{
    IO_STATUS_BIT_DEEP_SLEEP=0,     // 0    �������״̬
    IO_STATUS_BIT_SLEEP,            // 1    ����״̬
    IO_STATUS_BIT_RESERVER2,
    IO_STATUS_BIT_RESERVER3,
    IO_STATUS_BIT_RESERVER4,
    IO_STATUS_BIT_RESERVER5,
    IO_STATUS_BIT_RESERVER6,
    IO_STATUS_BIT_RESERVER7,
    IO_STATUS_BIT_RESERVER8,
    IO_STATUS_BIT_RESERVER9,
    IO_STATUS_BIT_RESERVER10,
    IO_STATUS_BIT_RESERVER11,
    IO_STATUS_BIT_RESERVER12,
    IO_STATUS_BIT_RESERVER13,
    IO_STATUS_BIT_RESERVER14,
    IO_STATUS_BIT_RESERVER15,
    IO_STATUS_BIT_MAX,
}IO_STATUS_BIT;
//����λ
typedef enum
{
    ALARM_BIT_EMERGENCY=0,          // 0    1����������,�յ�Ӧ������
    ALARM_BIT_OVER_SPEED,           // 1    1������,��־ά���������������
    ALARM_BIT_TIRED_DRIVE,          // 2    1��ƣ�ͼ�ʻ,��־ά���������������
    ALARM_BIT_DANGER_PRE_ALARM,     // 3    1��Σ��Ԥ��,�յ�Ӧ�������
    ALARM_BIT_GNSS_FAULT,           // 4    1��GNSSģ�����,��־ά���������������
    ALARM_BIT_ANT_SHUT,             // 5    1��GNSS���߶Ͽ�,��־ά���������������
    ALARM_BIT_ANT_SHORT,            // 6    1��GNSS���߶�·,��־ά���������������
    ALARM_BIT_POWER_LOW,            // 7    1������Ƿѹ,��־ά���������������
    ALARM_BIT_POWER_SHUT,           // 8    1���������,��־ά���������������
    ALARM_BIT_LCD_FAULT,            // 9    1��LCD��ʾ������,��־ά���������������
    ALARM_BIT_TTS_FAULT,            // 10   1��TTS����,��־ά���������������
    ALARM_BIT_CAMERA_FAULT,         // 11   1������ͷ����,��־ά���������������
    ALARM_BIT_IC_ERROR,             // 12    1����·����֤IC ��ģ�����,��־ά���������������
    ALARM_BIT_OVER_SPEED_PRE_ALARM, // 13    1������Ԥ��,��־ά���������������
    ALARM_BIT_TIRED_DRIVE_PRE_ALARM,// 14    1��ƣ�ͼ�ʻԤ��,��־ά���������������
    ALARM_BIT_RESERVER15,           // 15    ����
    ALARM_BIT_RESERVER16,           // 16    ����
    ALARM_BIT_RESERVER17,           // 17   ����
    ALARM_BIT_DAY_OVER_DRIVE,       // 18   1�������ۼƼ�ʻ��ʱ,��־ά���������������
    ALARM_BIT_STOP_OVER,            // 19   1����ʱͣ��,��־ά���������������
    ALARM_BIT_IN_OUT_AREA,          // 20   1����������,�յ�Ӧ�������
    ALARM_BIT_IN_OUT_ROAD,          // 21   1������·��,�յ�Ӧ������
    ALARM_BIT_LOAD_OVER_DRIVE,      // 22   1��·����ʻʱ�䲻��/����,�յ�Ӧ������
    ALARM_BIT_LOAD_EXCURSION,       // 23   1��·��ƫ�뱨��,��־ά���������������
    ALARM_BIT_VSS_FAULT,            // 24   1������VSS ����,��־ά���������������
    ALARM_BIT_OIL_FAULT,            // 25   1�����������쳣,��־ά���������������
    ALARM_BIT_STEAL,                // 26   1����������(ͨ������������),��־ά���������������
    ALARM_BIT_LAWLESS_ACC,          // 27   1�������Ƿ����,�յ�Ӧ�������
    ALARM_BIT_LAWLESS_MOVEMENT,     // 28   1�������Ƿ�λ��,�յ�Ӧ�������
    ALARM_BIT_IMPACT_PRE_ALARM,     // 29    1����ײԤ��,��־ά���������������
    ALARM_BIT_SIDE_TURN_PRE_ALARM,  // 30    1���෭Ԥ��,��־ά���������������
    ALARM_BIT_LAWLESS_OPEN_DOOR,    // 31    1���Ƿ����ű������ն�δ��������ʱ�����жϷǷ����ţ�,�յ�Ӧ�������
    ALARM_BIT_MAX,
}ALARM_BIT;

//�Զ����2
typedef enum
{
    DEFINE_BIT_0=0,     // 0,ä�����ݱ��,֮ǰ�ϵ�EGS701ʹ��,��������ר�ŵĻ㱨ָ��0x0074����ʹ�øñ�־��   
    DEFINE_BIT_1,       // 1,��ʱ�㱨��־,����ͨ���ϱ���λ����Ϣ�Ϳ�֪���Ƕ�ʱ�㱨���Ƕ���㱨      
    DEFINE_BIT_2,       // 2,����㱨��־,����ͨ���ϱ���λ����Ϣ�Ϳ�֪���Ƕ�ʱ�㱨���Ƕ���㱨  
    DEFINE_BIT_3,       // 3,����ä������绰ʱ����Ϊ����ä������־ 
    DEFINE_BIT_4,       // 4,���ٱ�־   
    DEFINE_BIT_5,       // 5,���°�ǩ����־,1Ϊ�ϰ� 
    DEFINE_BIT_6,       // 6,����
    DEFINE_BIT_7,       // 7,����
    DEFINE_BIT_8,       // 8,����ѹ����־λ 
    DEFINE_BIT_9,       // 9,���״̬���������汾ʹ�ã�,1Ϊ���� 
    DEFINE_BIT_10,      // 10,�������,1Ϊ����
    DEFINE_BIT_11,      // 11,��������,1Ϊ��
    DEFINE_BIT_12,      // 12    
    DEFINE_BIT_13,      // [14,13]:00Ϊ����,01��ת,10��ת,11ͣת  
    DEFINE_BIT_14,      // 14   
    DEFINE_BIT_15,      // 15,�¶Ȳɼ�������    
    DEFINE_BIT_16,      // 16    
    DEFINE_BIT_17,      // 17   
    DEFINE_BIT_18,      // 18   
    DEFINE_BIT_19,      // 19,���ر���
    DEFINE_BIT_20,      // 20   
    DEFINE_BIT_21,      // 21   
    DEFINE_BIT_22,      // 22   
    DEFINE_BIT_23,      // 23   
    DEFINE_BIT_24,      // 24   
    DEFINE_BIT_25,      // 25,���ش���������    
    DEFINE_BIT_26,      // 26,���زɼ�������
    DEFINE_BIT_27,      // 27   
    DEFINE_BIT_28,      // 28   
    DEFINE_BIT_29,      // 29,��Ǵ���������  
    DEFINE_BIT_30,      // 30,����������������� 
    DEFINE_BIT_31,      // 31   
    DEFINE_BIT_MAX,
}DEFINE_BIT;

//�����Զ���ı���λ��״̬λ
typedef enum
{
    //�����ֽ�
    SUOMEI_BIT_0=0,     // 0�����״̬��1������0��
    SUOMEI_BIT_1,       // 1������״̬��1���죻0����
    SUOMEI_BIT_2,       // 2�������ƣ�1������0����
    SUOMEI_BIT_3,       // 3������״̬��1������0����
    SUOMEI_BIT_4,       // 4����ת�ƣ�1������0����
    SUOMEI_BIT_5,       // 5����ת�ƣ�1������0����
    SUOMEI_BIT_6,       // 6����ɲ״̬��1��ɲ����0����
    SUOMEI_BIT_7,       // 7����ɲ״̬��1��ɲ����0����
    //�����ֽ�
    SUOMEI_BIT_8,       // 8��1����ת��0����ת      
    SUOMEI_BIT_9,       // 9��1��ת����0��ֹͣ  
    SUOMEI_BIT_10,      // 10��1���س���0���ճ�
    SUOMEI_BIT_11,      // 11������ 
    SUOMEI_BIT_12,      // 12������    
    SUOMEI_BIT_13,      // 13������   
    SUOMEI_BIT_14,      // 14������   
    SUOMEI_BIT_15,      // 15��1���ѽ������ߣ�0û��������
    //�ڶ��ֽ�
    SUOMEI_BIT_16,      // 16������  
    SUOMEI_BIT_17,      // 17������ 
    SUOMEI_BIT_18,      // 18������ 
    SUOMEI_BIT_19,      // 19������
    SUOMEI_BIT_20,      // 20������
    SUOMEI_BIT_21,      // 21������
    SUOMEI_BIT_22,      // 22������
    SUOMEI_BIT_23,      // 23������ 
    //��һ�ֽ�
    SUOMEI_BIT_24,      // 24����Ա����
    SUOMEI_BIT_25,      // 25�����ر���
    SUOMEI_BIT_26,      // 26����������
    SUOMEI_BIT_27,      // 27��δ�����յ㱨��
    SUOMEI_BIT_28,      // 28������ʱ����ʻ����
    SUOMEI_BIT_29,      // 29������
    SUOMEI_BIT_30,      // 30������   
    SUOMEI_BIT_31,      // 31������  
    SUOMEI_BIT_MAX,
}SUOMEI_STATUS_BIT;

//************�ṹ������******************

//*************��������**************
//**********************��һ���֣���ʼ��****************
/*********************************************************************
//��������  :Io_Init(void)
//����      :IO��ʼ��
*********************************************************************/
void Io_Init(void);
/*********************************************************************
//��������  :Io_InitStatusHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
*********************************************************************/
void Io_InitStatusHighValid(void);
/*********************************************************************
//��������  :Io_InitExtCarStatusHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
*********************************************************************/
void Io_InitExtCarStatusHighValid(void);
/*********************************************************************
//��������  :Io_InitDefine2StatusHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
//��ע      :
*********************************************************************/
void Io_InitDefine2StatusHighValid(void);
/*********************************************************************
//��������  :Io_InitAlarmHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
*********************************************************************/
void Io_InitAlarmHighValid(void);
/*********************************************************************
//��������  :Io_UpdataPram(void)
//����      :���±���
*********************************************************************/
void Io_UpdataPram(void);
//**********************�ڶ����֣��ⲿ����****************
/*********************************************************************
//��������  :Io_TimeTask
//����      :IO������λ�������
//��ע      :50ms����һ��
*********************************************************************/
FunctionalState Io_TimeTask(void);
/*********************************************************************
//��������  :Io_ReadStatus
//����      :����׼״̬��
*********************************************************************/
u32 Io_ReadStatus(void);
/*********************************************************************
//��������  :Io_ReadStatusBit(u8 index)
//����      :����׼״̬λ
*********************************************************************/
u8 Io_ReadStatusBit(STATUS_BIT bit);
/*********************************************************************
//��������  :Io_WriteStatusBit
//����      :д��׼״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteStatusBit(STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadSuoMeiStatus
//����      :��SuoMei״̬��
//��ע      :
*********************************************************************/
u32 Io_ReadSuoMeiStatus(void);
/*********************************************************************
//��������  :Io_ReadSuoMeiStatusBit(u8 index)
//����      :��SuoMei״̬λ
//��ע      :
*********************************************************************/
u8 Io_ReadSuoMeiStatusBit(SUOMEI_STATUS_BIT bit);
/*********************************************************************
//��������  :Io_WriteSuoMeiStatusBit
//����      :дSuoMei״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSuoMeiStatusBit(SUOMEI_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadExtCarStatus
//����      :����չ����״̬��
*********************************************************************/
u32 Io_ReadExtCarStatus(void);
/*********************************************************************
//��������  :Io_ReadExtCarStatusBit(u8 index)
//����      :����չ����״̬λ
*********************************************************************/
u8 Io_ReadExtCarStatusBit(CAR_STATUS_BIT bit);
/*********************************************************************
//��������  :Io_WriteExtCarStatusBit
//����      :д��չ����״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteExtCarStatusBit(CAR_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadIoStatus
//����      :��IO״̬��
*********************************************************************/
u16 Io_ReadIoStatus(void);
/*********************************************************************
//��������  :Io_ReadIoStatusBit(u8 index)
//����      :��IO״̬λ
*********************************************************************/
u8 Io_ReadIoStatusBit(IO_STATUS_BIT bit);
/*********************************************************************
//��������  :Io_WriteIoStatusBit
//����      :дIO״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteIoStatusBit(IO_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadAlarmMaskBit(u8 index)
//����      :����������λ
*********************************************************************/
u8 Io_ReadAlarmMaskBit(ALARM_BIT bit);
/*********************************************************************
//��������  :Io_ReadAlarm
//����      :��������
*********************************************************************/
u32 Io_ReadAlarm(void);
/*********************************************************************
//��������  :Io_ReadKeyAlarm
//����      :���ؼ�������
//��ע      :
*********************************************************************/
u32 Io_ReadKeyAlarm(void);
/*********************************************************************
//��������  :Io_ReadAlarmBit(u8 index)
//����      :������λ
*********************************************************************/
u8 Io_ReadAlarmBit(ALARM_BIT bit);
/*********************************************************************
//��������  :Io_ReadKeyAlarmBit(ALARM_BIT bit)
//����      :���ؼ�������ĳһ��λ
//��ע      :
*********************************************************************/
u8 Io_ReadKeyAlarmBit(ALARM_BIT bit);
/*********************************************************************
//��������  :Io_WriteAlarmBit
//����      :дĳһ����λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteAlarmBit(ALARM_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_WriteKeyAlarmBit
//����      :д�ؼ�������ĳһ����λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteKeyAlarmBit(ALARM_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadSelfDefine2
//����      :���Զ���״̬��2
//��ע      :
*********************************************************************/
u32 Io_ReadSelfDefine2(void);
/*********************************************************************
//��������  :Io_ReadSelfDefine2Bit(u8 index)
//����      :���Զ����2��λ
//��ע      :
*********************************************************************/
u8 Io_ReadSelfDefine2Bit(DEFINE_BIT bit);
/*********************************************************************
//��������  :Io_WriteSelfDefine2Bit
//����      :д�Զ����2��λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSelfDefine2Bit(DEFINE_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadOverSpeedConrtolBit
//����      :�����ٿ���λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadOverSpeedConrtolBit(OVER_SPEED_BIT bit);
/*********************************************************************
//��������  :Io_WriteOverSpeedConrtolBit
//����      :д���ٿ���λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadPreOvertimeConrtolBit
//����      :����ʱԤ������λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadPreOvertimeConrtolBit(PRE_OVER_TIME_BIT bit);
/*********************************************************************
//��������  :Io_WritePreOvertimeConrtolBit
//����      :д��ʱԤ������λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WritePreOvertimeConrtolBit(PRE_OVER_TIME_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadOvertimeConrtolBit
//����      :����ʱ����λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadOvertimeConrtolBit(OVER_TIME_BIT bit);
/*********************************************************************
//��������  :Io_WriteOvertimeConrtolBit
//����      :д��ʱ����λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteOvertimeConrtolBit(OVER_TIME_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadRecordConrtol
//����      :��¼�������ֽ�
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
u8 Io_ReadRecordConrtol(void);
/*********************************************************************
//��������  :Io_WriteRecordConrtolBit
//����      :д¼������λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteRecordConrtolBit(RECORD_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_ReadRecordConrtolBit
//����      :��¼������λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadRecordConrtolBit(RECORD_BIT bit);
/*********************************************************************
//��������  :Io_WriteMicConrtolBit
//����      :дĳһ��˿���λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteMicConrtolBit(MIC_BIT bit, FlagStatus Status);
/*********************************************************************
//��������  :Io_WriteSpeakerConrtolBit
//����      :дĳһ����������λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSpeakerConrtolBit(SPEAKER_BIT bit, FlagStatus state);
//********************�������֣��ڲ�����*******************
/*********************************************************************
//��������  :Io_DetectStatus
//����      :�������״̬λ��IO����
*********************************************************************/
void Io_DetectStatus(GPIO_OUT_IN_NUM IoNum, u8 index);
/*********************************************************************
//��������  :Io_DetectExtCarStatus
//����      :���������չ����״̬�ֵ�IO����
*********************************************************************/
void Io_DetectExtCarStatus(GPIO_OUT_IN_NUM IoNum,u8 index);
/*********************************************************************
//��������  :Io_DetectDefine2Status
//����      :��������Զ���2״̬�ֵ�IO����
//��ע      :
*********************************************************************/
void Io_DetectDefine2Status(GPIO_OUT_IN_NUM IoNum,u8 index);
/*********************************************************************
//��������  :Io_DetectAlarm
//����      :��ⱨ��IO��
*********************************************************************/
void Io_DetectAlarm(GPIO_OUT_IN_NUM IoNum,u8 index);
/*********************************************************************
//��������  :Io_DetectFrontDoor
//����      :���ǰ��
*********************************************************************/
void Io_DetectFrontDoor(void);
/*********************************************************************
//��������  :Io_DetectLoad
//����      :�����س�״̬�����س�����������
//��ע      :ֻ���������汾����,��ADC1���,100ms����һ��
//              :�ճ���3.2V~4.5V
//              :�س���2.0V~3.2V
//              :��������·��0V~2.0V
//              :��������·��4.5V~
*********************************************************************/
void Io_DetectLoad(void);
/*********************************************************************
//��������  :Io_DetectPhoneKey
//����      :���һ��ͨ����
*********************************************************************/
u8 Io_DetectPhoneKey(void);
/*********************************************************************
//��������  :Io_DetectMainPower
//����      :��������ѹ
//��ע      :100ms����һ��
*********************************************************************/
void Io_DetectMainPower(void);
/*********************************************************************
//��������  :Io_PowerOnDetectMainPower
//����      :�ϵ��������ѹ,�ж���12V���绹��24V����
//��ע      :�ϵ���Ӳ����ʼ����ɺ����whileǰ����øú���
*********************************************************************/
void Io_PowerOnDetectMainPower(void);
/*********************************************************************
//��������  :Io_WriteStatusHighValidBit
//����      :дStatusHighValidĳһ״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteStatusHighValidBit(STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_WriteExtCarStatusHighValidBit
//����      :дExtCarStatusHighValidĳһ״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_WriteSelfDefine2HighValidBit
//����      :дSelfDefine2HighValidĳһ״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSelfDefine2HighValidBit(DEFINE_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_WriteAlarmHighValidBit
//����      :дAlarmHighValidĳһ״̬λ
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteAlarmHighValidBit(ALARM_BIT bit, FlagStatus state);
/*********************************************************************
//��������  :Io_GetCarVoltageType
//����      :��ȡ������ѹ���ͣ�1Ϊ24V��0Ϊ12V
//��ע      :
*********************************************************************/
u8 Io_GetCarVoltageType(void);
/*********************************************************************
//��������  :Io_ClearAlarmWord
//����      :��������ָ�λ
//��ע      :
*********************************************************************/
void Io_ClearAlarmWord(void);
/*********************************************************************
//��������  :Io_ClearStatusWord
//����      :���״̬�ָ�λ
//��ע      :
*********************************************************************/
void Io_ClearStatusWord(void);
/*********************************************************************
//��������  :Io_ClearExtCarStatusWord
//����      :�����չ����״̬�ָ�λ
//��ע      :
*********************************************************************/
void Io_ClearExtCarStatusWord(void);
#endif
