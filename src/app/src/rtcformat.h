#ifndef __RTC_FORMAT_H
#define __RTC_FORMAT_H

//*********�ļ�����************
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

//*********�궨��**************

//**********�Զ�����������********
//ʱ��ṹ������
typedef struct
	{
		//s16	year;	//��:1970~
		s8  year;//��:0~99
		s8	month;	//��:1~12
		s8	day;	//��:1~31
		s8	hour;	//ʱ:0~23
		s8	min;	//��:0~59
		s8	sec;	//��:0~59
	}TIME_T;

//**********��������**********


#endif
