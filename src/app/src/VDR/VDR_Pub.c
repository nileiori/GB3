/************************************************************************
//程序名称：VDR_Pub.c
//功能：VDR模块用到的一些公共函数
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "Public.h"

/********************本地变量*************************/


/********************全局变量*************************/
u8 HexToAscBuffer[2100] = {0};

/********************外部变量*************************/


/********************本地函数声明*********************/


/********************函数定义*************************/
/*********************************************************************
//函数名称	:VDRPub_GetNoNavigationPostion
//功能		:获取不定位时的位置信息
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:数据长度，一般情况返回长度10
//备注		:不定位时的位置信息为7FFFFFFF，高程为0
*********************************************************************/
u8 VDRPub_GetNoNavigationPostion(u8 *pBuffer)
{
  u8 *p = NULL;
	
	if(NULL == p)
	{
	  p = pBuffer;
		*p++ = 0x7f;
		*p++ = 0xff;
		*p++ = 0xff;
		*p++ = 0xff;
		*p++ = 0x7f;
		*p++ = 0xff;
		*p++ = 0xff;
		*p++ = 0xff;
		*p++ = 0;
		*p++ = 0;
		
		return 10;
	}
	
	return 0;
	
}
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

	if(1 == Public_CheckTimeStruct(time))
	{
		p = pBuffer;
	
		*p++ = VDRPub_HEX2BCD(time->year);
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
	
	time->year = VDRPub_BCD2HEX(*p++);
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
	TIME_T time;
        RTC_GetCurTime(&time);
	if(1 == Public_CheckTimeStruct(&time))
	{
		p = pBuffer;
	
		*p++ = VDRPub_HEX2BCD(time.year);
		*p++ = VDRPub_HEX2BCD(time.month);
		*p++ = VDRPub_HEX2BCD(time.day);
		*p++ = VDRPub_HEX2BCD(time.hour);
		*p++ = VDRPub_HEX2BCD(time.min);
		*p++ = VDRPub_HEX2BCD(time.sec);
	}
	else
	{
		p = pBuffer;
	
		*p++ = 0x13;
		*p++ = 0x01;
		*p++ = 0x01;
		*p++ = 0x00;
		*p++ = 0x00;
		*p++ = 0x00;
	}
	
	return 6;
}
/*********************************************************************
//函数名称	:VDRPub_HEX2ASC
//功能		:将一个十六进制数转成2个对应的ASCII码
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:对应的ASC码
//备注		:如0x12变成'1''2'，高字节在前，低字节在后
*********************************************************************/
u16 VDRPub_HEX2ASC(u8 *DstBuffer, u8 *SrcBuffer, u16 SrcBufferLen)
{
    u8 *pSrc=NULL;
	  u8 *pDst=NULL;
	  u8 HighByte;
	  u8 LowByte;
	  u8 temp;
	  u16 i;
	
	  pSrc=SrcBuffer;
	  pDst=DstBuffer;
	  for(i=0; i<SrcBufferLen; i++)
	  {
		    temp = *(pSrc+i);
			
	      HighByte = (temp&0xf0) >> 4;
	      LowByte = temp&0x0f;
	
	      if(HighByte >= 10)
		    {
		        HighByte = 'A'+(HighByte-10);
		    }
		    else
		    {
		        HighByte = '0'+HighByte;
		    }
				
				if(LowByte >= 10)
		    {
		        LowByte = 'A'+(LowByte-10);
		    }
		    else
		    {
		        LowByte = '0'+LowByte;
		    }
				
			  *(pDst+2*i) = HighByte;
				*(pDst+2*i+1) = LowByte;
		}
		
		return 2*SrcBufferLen;  
}
/*********************************************************************
//函数名称	:VDRPub_HEX2ASC
//功能		:将一个十六进制数转成2个对应的ASCII码
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:对应的ASC码
//备注		:如0x12变成'1''2'，高字节在前，低字节在后
*********************************************************************/
/*
u16 VDRPub_HEX2ASC(u8 hex)
{
	  u16 AscWord;
    u8 HighByte;
	  u8 LowByte;
	
	  HighByte = (hex&0xf0) >> 4;
	  LowByte = hex&0x0f;
	
	  if(HighByte >= 10)
		{
		    HighByte = 'A'+(HighByte-10);
		}
		else
		{
		    HighByte = '0'+HighByte;
		}
		
		if(LowByte >= 10)
		{
		    LowByte = 'A'+(LowByte-10);
		}
		else
		{
		    LowByte = '0'+LowByte;
		}
		
		AscWord = (HighByte << 8)|LowByte;
		
		return AscWord;
}
*/
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







