#ifndef __RTC_FORMAT_H
#define __RTC_FORMAT_H

//*********文件包含************
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

//*********宏定义**************

//**********自定义数据类型********
//时间结构体类型
typedef struct
	{
		//s16	year;	//年:1970~
		s8  year;//年:0~99
		s8	month;	//月:1~12
		s8	day;	//日:1~31
		s8	hour;	//时:0~23
		s8	min;	//分:0~59
		s8	sec;	//秒:0~59
	}TIME_T;

//**********函数声明**********


#endif
