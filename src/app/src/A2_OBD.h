#ifndef __A2_OBD_H__
#define __A2_OBD_H__

#include"include.h"


//A2OBDģ��������
#define A2OBD_BPS               0x8100//���ò�����
#define A2OBD_HARD_VERSION      0x8200//Ӳ���汾��Ϣ
#define A2OBD_SOFTE_VERSION     0x8201//����汾��Ϣ
#define A2OBD_RESET							0x8300//�����λ
#define	A2OBD_ON_STREAM_DATA 		0x8500//����������
#define	A2OBD_OFF_STREAM_DATA 	0x8501//����������
#define A2OBD_PRA_SET           0x8700//��������
#define A2OBD_PRA_READ          0x8701//������ѯ
#define A2OBD_STREM_DATA        0x8800//ʵʱ������
#define A2OBD_ALARM							0x8B00//�������
#define	A2OBD_ROUTE_END					0x8A00//�г����ݽ���
#define A2OBD_ROUTE_BEGIN       0x8A01//�г̿�ʼ

void A2OBD_UartDataProtocolParse(unsigned char *pBuffer,unsigned short Len);
//A2OBD������
typedef struct A2OBD_STREAM_DATA
{
	int Speed;//����
	int Rpm;//ת��
	int Coot;//ˮ��
	int Load;//����
  int Volt;//��ƿ��ѹ	
	int OilPosition;//����̤��λ��
	int ClutchState;//���״̬
	int BrakeState;//ɲ��״̬
	int CurON;//˲ʱ�ͺģ�L/H��
	int FuelKilometer;//�ٹ����ͺģ�L/100KM��
	int EngineFuelPressure;//����ѹ��
	int FuelRailPressure;//ʵ���͹�ѹ��
	int ExhaustGasAlarm;//���������źŵ�
	int OBD_State;//OBDģ��״̬
}A2OBD_StreamData;
//A2OBD�г�����
typedef struct A2OBD_ROUTE_DATA
{
	int IdlingTime;//����ʱ�� ��λ1s
	int DriveTime;//��ʻʱ�� ��λ1s
	int HighBrakeTime;//��ߵ�ʱ�� ��λ1s
	int RouteTime;//�г�ʱ�� ��λ1s
  int ECU_TatolFuel;//ECU�ܺ��� ��λ0.01L
	int IdlingFuel;//�����ͺ� ��λ0.01L
	int SlideFuel;//���н�Լ�ͺ� ��λ0.01L
	int GunFuel;//�������ͺ� ��λ0.01L
	int HighBrakeFuel;//��ߵ��ͺ� ��λ0.01L
	int RouteFuel;//�г��ͺ� ��λ0.01L
	int ECU_Mileage;//ECU����� ��λ0.01Km
	int BrakeMileage;//�ƶ���� ��λ0.01Km
	int RouteMileage;//�г���� ��λ0.01Km
	int IdlingBrakeMileage;//�յ�������� ��λ0.01Km
	int GunMileag;//��������� ��λ0.01Km
	int HighBrakeMileage;//��ߵ���� ��λ0.01Km
	int OverSpeedMileage;//������� ��λ0.01Km
	int SlideMileage;//����������� ��λ0.01Km
	int OverRpmMileage;//������ת����� ��λ0.01Km
	int AverageRpm;//ƽ��ת�� ��λ0.01RPM
	int AverageGasPedal;//ƽ������  ��λ0.01%
	int AverageLoad;//ƽ������������ ��λ0.01%
	int AverageSpeed;//ƽ������ ��λ0.01km/h
	int HighSpeed;//��߳��� ��λ0.01km/h
	int HighRpm;//���ת�� ��λ0.01RPM
	int EcoRpmMileage;//����ת����� ��λ 0.01km
	int GunTimes;//�����Ŵ��� 
	int EmerBrakeTimes;//��ɲ������
	int BrakeTimes;//ɲ������
	int OverSpeedTimes;//���ٴ���
	int ClutchTimes;//���ʹ�ô���
	int AverageFuel;//ƽ���ͺģ�L/100km��
	int StartTimes;//�𲽴���
	int OnlyAverageRpm;//��ƽ��ת��
	int OnlyAverageGasPedal;//��ƽ������
	int OnlyAverageLoad;//��ƽ������
	int OnlyAverageSpeed;//��ƽ������
	int OnlyAverageFuel;//��ƽ���ͺģ�L/100km��
	int EmerTurnTime;//��ת�����
	int EmerGasPedal;//�������Ŵ���
	int NoGearTimes;//��������ʱ����
	int InclineAlarmTimes;//��б��������
	int BadlyBumpTimes;//���ص�������
	int ThreeViolateTimes;//3sΥ������
	int OverTimeIdlingTimes;//��ʱ�䵡�ٴ���
	int UnderVoltageAlarmTimes;//��ѹ���ͱ�������
}A2OBD_RouteData;
//A2OBD����״̬
typedef enum
{
	EMER_GAS_PEDAL=1,//��������
	EMER_BRAKE,//����ɲ��
	IDLING_SLIDE,//�յ�����
	OVER_SPEED,//����
	NO_GEAR,//��������ʱ
	UNDER_ENGINGE_FUEL,//����ѹ������
	HIGEH_COOT,//������ˮ�¹���
	OVER_TIME_GEAR,//��ʱ������
	UNDER_VOLTAGE,//��ѹ����
	TROUBLE_DRIVE,//�������г�
	ACC_OFF_SLIDE,//Ϩ���б���
	CRASH_PRE,//��ײԤ��
	CRASH,//��ײ����
	EMER_TURN,//��ת��
	OVER_TIME_IDLING,//��ʱ�䵡��
	INCLINE,//��б
	ROLLOVER,//�෭
	SMALL_BUMP,//��΢����
	MEDIUM_BUMP,//�еȵ���
	BADLY_BUMP,//���ص���
}A2OBD_Alarm;//״̬

void A2OBD_SentCmd(u16 cmd,u8 *pBuffer,u16 datalen);
void A2OBD_Protocol_Paramter(u8 cmd,u8 *pBuffer,u8 datalen);
void A2OBD_UartDataProtocolParse(unsigned char *pBuffer,unsigned short Len);
u16  ThreeEmerInfo(u8 *pDst);
FunctionalState A2OBD_TimeTask(void);
#endif
