/************************************************************************
//�������ƣ�VDR_Pub.c
//���ܣ�VDRģ���õ���һЩ��������
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "Public.h"

/********************���ر���*************************/


/********************ȫ�ֱ���*************************/
u8 HexToAscBuffer[2100] = {0};

/********************�ⲿ����*************************/


/********************���غ�������*********************/


/********************��������*************************/
/*********************************************************************
//��������	:VDRPub_GetNoNavigationPostion
//����		:��ȡ����λʱ��λ����Ϣ
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:���ݳ��ȣ�һ��������س���10
//��ע		:����λʱ��λ����ϢΪ7FFFFFFF���߳�Ϊ0
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
//��������	:VDRPub_ConvertTimeToBCD
//����		:��TIME_T��ʽ��ʱ��ת����BCD���ʽ
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:���ݳ��ȣ�һ��������س���6��ʱ���쳣���س���0
//��ע		:BCD��ʱ��Ķ������ʻ��¼�Ǳ�׼A.8
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
//��������	:VDRPub_ConvertBCDToTime
//����		:��BCD��ʽ��ʱ��ת����TIME_T��ʽ
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:��ȷ����1�����󷵻�0
//��ע		:BCD��ʱ��Ķ������ʻ��¼�Ǳ�׼A.8
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
//��������	:VDRPub_ConvertNowTimeToBCD
//����		:��BCD���ʾ�ĵ�ǰʱ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:���ݳ��ȣ�һ��������س���6��ʱ���쳣���س���0
//��ע		:ʱ��6�ֽڵĶ������ʻ��¼�Ǳ�׼A.8
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
//��������	:VDRPub_HEX2ASC
//����		:��һ��ʮ��������ת��2����Ӧ��ASCII��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:��Ӧ��ASC��
//��ע		:��0x12���'1''2'�����ֽ���ǰ�����ֽ��ں�
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
//��������	:VDRPub_HEX2ASC
//����		:��һ��ʮ��������ת��2����Ӧ��ASCII��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:��Ӧ��ASC��
//��ע		:��0x12���'1''2'�����ֽ���ǰ�����ֽ��ں�
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
//��������	:VDRPub_HEX2BCD
//����		:��һ��ʮ��������ת��BCD��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:ʮ����������Χ00-99��������Χ��ת��
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
//��������	:VDRPub_BCD2HEX
//����		:��һ��BCD��ת��ʮ��������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:ʮ����������Χ00-99��������Χ��ת��
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







