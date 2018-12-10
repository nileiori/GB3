//ADC�ɼ���������Ե�ѹ������Ե�ѹ������
#include "stm32f10x.h"

#ifndef __ADC_VOLTAGE_API__
#define __ADC_VOLTAGE_API__

#ifndef	_EXT_ADC_VOL_
	#define	EXT_ADC_API		extern
#else
	#define	EXT_ADC_API
#endif

typedef unsigned int  uint;
typedef signed int  sint ;
typedef unsigned char  uchar;
typedef signed char  schar ;
/*
typedef enum
{
	false=0,
	true,	
}PUB_BOOL_;
*/
//���9·��AD���ݲɼ�ƽ������������
typedef struct _ADC_CHANNEL_DATA_
{
	uint ext_vol_1;		//�ⲿ�����ѹ1
	uint ext_vol_2;		//�ⲿ�����ѹ1
	uint main_vol;		//����ѹ1
	uint	battery_vol;	//��ص�ѹ
	uint short_vol;		//��·��ѹAD
	uint	mic_vol;		//��˷��ѹ
	uint	key_vol;		//���̵�ѹ
	uint prt_vol;			//��ӡ����ѹ
	uint	prt_temp_vol;	//��ӡ���¶ȵ�ѹ
	
}_ADC_API_;

typedef 	_ADC_API_		*P_ADC_API;

EXT_ADC_API	_ADC_API_	g_adc_api;

#define		MIC_INSERT_DEVICE		1100
#define		MIC_EXTRACT_DEVICE		500

//ͬ���⣬�㷨��ADC�ɼ�
#define		APP_CHANNEL_36V_1			0	//�ⲿ����1ͨ��0-36V
#define		APP_CHANNEL_36V_2			1	//�ⲿ����2ͨ��0-36V
#define		APP_MAIN	_POWER				2	//����Դ������
#define		APP_BATTERY_POWER		3	//���ݵ�ص�ѹ���

#define		BASE_CURRENT_200MA		200

//ϵͳ���ú�����ÿ��20�������һ�Σ��ɼ�9��ͨ����ADCֵ
//�ڽ���ƽ������
FunctionalState Adc_CollectApp_TimeTask(void);
//�ⲿ�ӿں���
//�ⲿ�ӿ�ADC1�������ѹ��⺯��,mVΪ��λ
uint Adc_ExternInputVoltage_1_Read(void);
//�ⲿ�ӿ�ADC2�������ѹ��⺯��,mVΪ��λ
uint Adc_ExternInputVoltage_2_Read(void);
//��ȡ�������ѹ��mVΪ��λ
uint Adc_MainPower_Readl(void);
//��ȡ���ݵ�ص�ѹ
uint Adc_BatteryPower_Readl(void);
//��ȡ��·����ѹ�ο�����ѹ��λmV
uint Adc_OutputDriver_Read(void);
//��ȡ��˷�״̬ 1 ��ã�0û�в�
uchar Adc_MicInput_Read(void);
//��ȡ�˲���ļ���AD��ֵ
uint	Adc_GetKeyValue(void);
//�ж��Ƿ���غ���
//num�������0��С��6,���룬���������
//����true ���ص�����false ��������״̬.mV
uchar Adc_CheckDriverCurrentOver(uchar num);

#endif



