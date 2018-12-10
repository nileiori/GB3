//ADC�ɼ���������Ե�ѹ������Ե�ѹ������
#define		_EXT_ADC_VOL_ 
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
*/
#include "include.h"
#include "Adc.h"
//#include "modem_app_com.h"

#define		COLL_CONST		1

/**********************************************************************************/
/*function	name: 			void Adc_CollectApp_TimeTask(void)		*/
/*input parameter:			none  */
/*output parameter:			none */
/*FUNCTION: 			ϵͳ���ú�����ÿ��20�������һ�Σ��ɼ�9��ͨ����ADCֵ
�ڽ���ƽ������
*/
/**********************************************************************************/
FunctionalState Adc_CollectApp_TimeTask(void)
{
	uchar k=0;
	static uchar i=0;
	static _ADC_API_	adc[COLL_CONST];
	
	//P_ADC_API	p_adc;
	//��ȡADC����
	adc[i].ext_vol_1=Ad_GetValue(ADC_EXTERN1);
	adc[i].ext_vol_2=Ad_GetValue(ADC_EXTERN2);
	adc[i].main_vol=Ad_GetValue(ADC_MAIN_POWER);
	adc[i].battery_vol=Ad_GetValue(ADC_VBATIN);
	adc[i].short_vol=Ad_GetValue(ADC_IO_OUT);
	adc[i].mic_vol=Ad_GetValue(ADC_MIC);
	adc[i].key_vol=Ad_GetValue(ADC_KEY);
	adc[i].prt_vol=Ad_GetValue(ADC_PRINTF);
	adc[i].prt_temp_vol=Ad_GetValue(ADC_PRINTF_THE);
	
	i++;
	//ƽ��������
	if(i==COLL_CONST)
		{
			memset((uchar*)&g_adc_api,0,sizeof(g_adc_api));
			for(k=0;k<COLL_CONST;k++)
				{
					g_adc_api.ext_vol_1+=adc[k].ext_vol_1;
					g_adc_api.ext_vol_2+=adc[k].ext_vol_2;
					g_adc_api.main_vol+=adc[k].main_vol;
					g_adc_api.battery_vol+=adc[k].battery_vol;
					g_adc_api.short_vol+=adc[k].short_vol;
					g_adc_api.mic_vol+=adc[k].mic_vol;
					g_adc_api.key_vol+=adc[k].key_vol;
					g_adc_api.prt_vol+=adc[k].prt_vol;
					g_adc_api.prt_temp_vol+=adc[k].prt_temp_vol;
				}
				g_adc_api.ext_vol_1/=COLL_CONST;
				g_adc_api.ext_vol_2/=COLL_CONST;
				g_adc_api.main_vol/=COLL_CONST;
				g_adc_api.battery_vol/=COLL_CONST;
				g_adc_api.short_vol/=COLL_CONST;
				g_adc_api.mic_vol/=COLL_CONST;
				g_adc_api.key_vol/=COLL_CONST;
				g_adc_api.prt_vol/=COLL_CONST;
				g_adc_api.prt_temp_vol/=COLL_CONST;
		}
	
	i=i%COLL_CONST;
	
	
	return ENABLE;
}

/**********************************************************************************/
/*function	name: 			uint uint Adc_ExternInputVoltage_1_Read(void)		*/
/*input parameter:			none  */
/*output parameter:			none */
/*FUNCTION: 			����ĵ�ѹ�����㹫ʽ��Vin*100=Vadc*330/4095*(R1+R2)/R2.��
�Ժ���Ϊ��λ������ȡ�õ����ݶ�����V��1000��
Vin*1000=Vadc*3300/4095*(120K+15K)/15K.����ѹ����������29.7V��
*/
/**********************************************************************************/
uint Adc_ExternInputVoltage_1_Read(void)
{
  uint tmp=0;
	//��ȡ������������������
	tmp=g_adc_api.ext_vol_1;
	tmp=tmp*3300/4095*9;//135/15;
 return tmp;
}
//0-36V�ⲿ��ȡ2ͨ��
uint Adc_ExternInputVoltage_2_Read(void)
{
  uint tmp=0;
	//��ȡ������������������
	tmp=g_adc_api.ext_vol_2;
	tmp=tmp*3300/4095*9;//135/15;
 return tmp;
}
//��ȡ�������ѹ��mVΪ��λ
uint Adc_MainPower_Readl(void)
{
  uint tmp=0;
	//��ȡ������������������
	tmp=g_adc_api.main_vol;
	tmp=tmp*3300/4095*9;//135/15;
 return tmp;
}
//��ȡ���ݵ�ص�ѹ
uint Adc_BatteryPower_Readl(void)
{
	  uint tmp=0;
	//��ȡ������������������
	tmp=g_adc_api.battery_vol;
	tmp=tmp*3300/4095*2;
 return tmp;
}
//��ȡ��·����ѹ�ο�����ѹ��λmV
uint Adc_OutputDriver_Read(void)
{
	 uint tmp=0;
	 	tmp=g_adc_api.short_vol*3300/4095;
	 return tmp;
}
//�ж��Ƿ���غ���
//num�������0��С��6
//����true ���ص�����false ��������״̬.mV
uchar Adc_CheckDriverCurrentOver(uchar num)
{
	//uchar tmp;
	uint vol,tmp_0;
	if(num==0)
		return false;
	else 
		if(num>6)
		return true;	
	vol=num*BASE_CURRENT_200MA*47/30;
	tmp_0=Adc_OutputDriver_Read();
	if(tmp_0>vol||tmp_0==vol)
		return true;
	else
		return false;
		
}
//��ȡ��˷�״̬ 1 ��ã�0û�в�
uchar  Adc_MicInput_Read(void)
{ 
    uchar tmp =0;
	//�γ���˷�
	if(g_adc_api.mic_vol<MIC_EXTRACT_DEVICE)
	tmp= false;
	//������˷�
	if(g_adc_api.mic_vol>MIC_INSERT_DEVICE)
	tmp= true;
      return tmp;  
}
//��ȡ�˲���ļ���AD��ֵ
uint	Adc_GetKeyValue(void)
{

	return	g_adc_api.key_vol;	
}

//��ȡ��ӡ����ѹ,��λmV
uint Adc_VoltagePrinterDriver_Read(void)
{
	  uint tmp=0;
	//��ȡ������������������
	tmp=g_adc_api.prt_vol;
	tmp=tmp*3300/4095*4;
 return tmp;	
}
//��ȡ��ӡ���¶�
uint Adc_PrinterTemprature_Read(void)
{
	
	return g_adc_api.prt_temp_vol;
}
