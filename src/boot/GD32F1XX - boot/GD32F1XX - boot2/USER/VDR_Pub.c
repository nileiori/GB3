/************************************************************************
//程序名称：VDR_Pub.c
//功能：VDR模块用到的一些公共函数
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：董显林
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "Public.h"

/********************本地变量*************************/


/********************全局变量*************************/


/********************外部变量*************************/
u8	SpeedFlag = 0;//速度类型,0为脉冲,1为GPS
u32 myPulseCnt = 0;             //校准计数使用
u8	TestStatus = 0;//E0H:进入检定状态;E1H:进入里程误差测量;E2H:进入脉冲系数误差测量;E3H:进入实时时钟误差测量;E4H:退出检定状态

/********************本地函数声明*********************/


/********************函数定义*************************/
/*********************************************************************
//函数名称	:VDRPub_ConvertTimeToBCD
//功能		:将TIME_T格式的时间转换成BCD码格式
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:数据长度，一般情况返回长度6，时间异常返回长度0
//备注		:BCD码时间的定义见行驶记录仪标准A.8
*********************************************************************/
u8 VDRPub_ConvertTimeToBCD(u8 *pBuffer, TIME_T	*time)
{
	u8	*p = NULL;
	u8	temp;

	if(1 == Public_CheckTimeStruct(time))
	{
		p = pBuffer;
		temp = time->year-2000;
	
		*p++ = VDRPub_HEX2BCD(temp);
		*p++ = VDRPub_HEX2BCD(time->month);
		*p++ = VDRPub_HEX2BCD(time->day);
		*p++ = VDRPub_HEX2BCD(time->hour);
		*p++ = VDRPub_HEX2BCD(time->min);
		*p++ = VDRPub_HEX2BCD(time->sec);
	}
	else
	{
		return 0;
	}
	
	return 6;
}
/*********************************************************************
//函数名称	:VDRPub_ConvertBCDToTime
//功能		:将BCD格式的时间转换成TIME_T格式
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:正确返回1，错误返回0
//备注		:BCD码时间的定义见行驶记录仪标准A.8
*********************************************************************/
u8 VDRPub_ConvertBCDToTime(TIME_T	*time,u8 *pBuffer)
{
	u8	i;
	u8	*p = NULL;
	
	p = pBuffer;
	
	for(i=0;i<6; i++)
	{
		if(*p++ > 99)
		{
			return 0;
		}
	}
	p = pBuffer;
	
	time->year = 2000+VDRPub_BCD2HEX(*p++);
	time->month = VDRPub_BCD2HEX(*p++);
	time->day = VDRPub_BCD2HEX(*p++);
	time->hour = VDRPub_BCD2HEX(*p++);
	time->min = VDRPub_BCD2HEX(*p++);
	time->sec = VDRPub_BCD2HEX(*p++);
	
	return 1;
}
/*********************************************************************
//函数名称	:VDRPub_ConvertNowTimeToBCD
//功能		:用BCD码表示的当前时间
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:数据长度，一般情况返回长度6，时间异常返回长度0
//备注		:时间6字节的定义见行驶记录仪标准A.8
*********************************************************************/
u8 VDRPub_ConvertNowTimeToBCD(u8 *pBuffer)
{
	u8	*p = NULL;
	u8	temp;
	TIME_T time;
  RTC_ReadTime(&time);
	if(1 == Public_CheckTimeStruct(&time))
	{
		p = pBuffer;
		temp = time.year-2000;
	
		*p++ = VDRPub_HEX2BCD(temp);
		*p++ = VDRPub_HEX2BCD(time.month);
		*p++ = VDRPub_HEX2BCD(time.day);
		*p++ = VDRPub_HEX2BCD(time.hour);
		*p++ = VDRPub_HEX2BCD(time.min);
		*p++ = VDRPub_HEX2BCD(time.sec);
	}
	else
	{
		return 0;
	}
	
	return 6;
}
/*********************************************************************
//函数名称	:VDRPub_HEX2BCD
//功能		:将一个十六进制数转成BCD码
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:十六进制数范围00-99，超出范围不转换
*********************************************************************/
u8 VDRPub_HEX2BCD(u8 hex)
{
	u8	i;
	u8	j;
	u8	bcd;
	
	if(hex > 99)
	{
		bcd = hex;
	}
	else
	{
		i = hex/10;
		j = hex%10;
		bcd = i<<4|j;
	}
	
	return bcd;
}
/*********************************************************************
//函数名称	:VDRPub_BCD2HEX
//功能		:将一个BCD码转成十六进制数
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:十六进制数范围00-99，超出范围不转换
*********************************************************************/
u8 VDRPub_BCD2HEX(u8 bcd)
{
	u8	i;
	u8	j;
	u8	hex;
	
	if(bcd > 99)
	{
		hex = bcd;
	}
	else
	{
		i = (bcd&0xf0) >> 4;
		j = bcd&0x0f;
		hex = 10*i+j;
	}
	
	return hex;
}







