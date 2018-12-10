
#ifndef __MAIN_H
#define __MAIN_H

//*********�궨��**************** 
//#define WATCHDOG_OPEN 1
//#define SLEEP_OPEN    2
//#define DEBUG_OPEN    3
//#define HUOYUN_DEBUG_OPEN  1//����ƽ̨�������øú꣬���������ú�Ҫ���Σ�������
//#define HUOYUN 1//��ʻ��¼�Ƿְ��̶�900�ֽ���Ҫ�����ú꣬���������ú�Ҫ���Σ�������

#define LOG_TYPE 0   //��ӡ���������Ϣ,��������ʱӦΪ0:0-����,1-���

//#define GPS_MODE_TYPE_UM330   //ֻ�ܶ�ѡһ
//#define GPS_MODE_TYPE_TD3017  
#define GPS_MODE_TYPE_ATGM332D


#define  BEEP_ON()          GpioOutOn(BUZZER);BeepOnOffFlag = 1;BeepOnTime=Timer_Val()//ʵ��Ӧ��ʹ��
//#define  BEEP_ON()        GpioOutOff(BUZZER);BeepOnOffFlag = 1;BeepOnTime=Timer_Val()//����ʱ����

//#define GPRS_DATA_PRINT_TO_DB9  1//��GPRS����ָ�7E��ͷ��7E��β��ָ���ӡ��DB9����

#define BBTEST_FIRMWARE_VERSION     "30105"//�ύ�����ƽ̨�����Ĺ̼��汾
#define FIRMWARE_VERSION     "30104"//
#define FIRMWARE_TIME        "201811291155" //����ʱ��

#define Recorder_CCC_ID             "C000116"          /*7���ֽ�*/
#define Recorder_Product_VER        "EGS701          " /*16���ֽ�*/

extern const char ProductInfo[][17]; 
extern const char MANUFACTURER[];
extern const char PRODUCT_MODEL[];//��2���ո�
extern const char PCB_VERSION[];//��1���ո�
extern const char SOFTWARE_VERSION[];//��main.h�е�FIRMWARE_VERSION����һ�£�ǰ��1λ���0,��5λһ�¡�
//*********��������**************
#endif
