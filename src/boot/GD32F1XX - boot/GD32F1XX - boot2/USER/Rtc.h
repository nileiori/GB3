#ifndef	RTC_H
#define	RTC_H

#include "stm32f10x.h"

#define START_YEAR				2015
#define END_YEAR					2070
#define MIN_SECOND				60
//#define HOUR_SECOND	    (60*60)
//#define DAY_SECOND 			(24*60*60)
//#define YEAR_SECOND 	  (365*24*60*60)
#define HOUR_SECOND	      0xE10
#define DAY_SECOND 				0x15180
#define YEAR_SECOND 	    0x1E13380
#define LEAP_YEAR       	1

typedef struct
{
		s16	year;	//:1970~2070
		s8	month;	//:1~12
		s8	day;	//:1~31
		s8	hour;	//:0~23
		s8	min;	//:0~59
		s8	sec;	//:0~59
}TIME_T;

void RTC_Init(void);
void RTC_SetTime(TIME_T *tt);
void RTC_ReadTime(TIME_T *tt);
void RTC_UpdateTime(void);//0.5s update once
void Gmtime(TIME_T *tt, u32 counter);
u32 ConverseGmtime(TIME_T *tt);

#endif
