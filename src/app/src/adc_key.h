//AD����ɨ�躯��
//AD���̵�Ӳ��ԭ����������ݲ�ͬ��AD����ֵ��������Ӧ��AD���̹�ϵ��
//ÿ�������ƫ��ֵ��+-5%
#include "stm32f10x.h"
#include "adc_tran_voltag.h"
#ifndef __ADC_KEY_API__
#define __ADC_KEY_API__

#ifndef	_EXT_KEY_VOL_
	#define	EXT_KEY_API		extern
#else
	#define	EXT_KEY_API
#endif

EXT_KEY_API  uchar key_code;
//typedef struct __VIRTUAL_KEY_
//{
//	uchar value;
//		
//	
//}
//����ֵ��������
typedef enum
{
	KEY_ERR0=0,		// ����AD����
	KEY_VALUE1,		// 1������
	KEY_VALUE2,		// 2������
	KEY_VALUE3,
	KEY_VALUE4,
	
	KEY_NONE=0XFF,	//�޼��̰���
}_AD_KEY_VALUE;
//ÿ�������ƫ��ֵ��5%������Ҫ���ǲ�����Χ5%
#define		BASE_KEY_R			10*1000
#define		R_KEY_1					20*1000
#define		R_KEY_2					10*1000
#define		R_KEY_3					5.1*1000
#define		R_KEY_4					2*1000

//NONE �޼�����
#define		KEY_NONE_MIN		3890
#define		KEY_NONE_MAX		4095
//����ֵ	2730
#define		KEY1_ADC_MIN			2637	 
#define		KEY1_ADC_MAX			2819	

#define		KEY1_ADC_VALUE		2730
//����ֵ	2047
#define		KEY2_ADC_MIN			1945
#define		KEY2_ADC_MAX			2149
//	1383
#define		KEY3_ADC_MIN			1293
#define		KEY3_ADC_MAX			1476
//	682
#define		KEY4_ADC_MIN			627
#define		KEY4_ADC_MAX			741
//ADת��ͨ����

//��Ӧ��ѹ����0.5V����Ϊ�м�����
//#define		FLOAT_INPUT_STATE_AD		500

//����ɨ�躯�� ��ʱ100mS����
FunctionalState KeyBoardAdc_TimeTask(void);
//���ؾ�����̵������ֵ
uchar KeyValue_Read(void);
u16 GetKeyV(void) ;
#endif
