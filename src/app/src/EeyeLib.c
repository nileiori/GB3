/*************************************************************************
*
* Copyright (c) 2008,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : EeyeLib.c
* ����     : E-EYE �⺯��
*
* ��ǰ�汾 : 1.0
* ������   : zhulin
* �޸�ʱ�� : 2008��4��15��
*
* ��ʷ�汾 : 
* ������   : 
* ���ʱ�� : 
* 
* ��ע :
*************************************************************************/ 
//#pragma SRC
#include    "EeyeLib.h"


const u8 ASC[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
/********************************************************************
* ���� : CharCopy
* ���� : �ַ�������
*
* ����: strDest : Ŀ�� �ַ��� 
*       strSrc  : Դ   �ַ���
* ���: len : �����ĳ���
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u8 CharCopy(const u8 *strSrc, u8  *strDest)
{
	u8  i,j;

	j = 0;
	do
	{
		i = *strSrc++;
		*strDest++ = i;
		j++;
	}while(i != 0);
	j--;
	return(j);
}

/********************************************************************
* ���� : CharCmp
* ���� : �Ƚ�2�ִ�������0����ȣ�����1�����ȡ�
*
* ����: strDest : Ŀ�� �ַ��� 
*       strSrc  : Դ   �ַ���
* ���: len : �����ĳ���
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u8 CharCmp(u8  * buff, const u8 * source, u8 length)
{
	u8  dat,dat0;

	do
	{
		dat = *buff++;
		dat0 = *source++;
		length--;
		if(dat != dat0)
			return(1);
	}while(length != 0);
	
	return(0);
}
/********************************************************************
* ���� : ClearMem
* ���� : ��λ �洢������ֵ
*
* ����: ptray : ��ʼ��ַ
*       len   : ����
* ���: ��
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void ClearMem(u8 *ptray, u8 len)
{
	if(len == 0)
		return;
	do{
		*ptray++ = 0;
		len--;
	}while(len != 0);
}


/********************************************************************
* ���� : CopyLenChar
* ���� : ���� �ַ���strSrcǰN���ַ����ַ���strDest��
*
* ����: ptray : ��ʼ��ַ
*       len   : ����
* ���: ��
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
/*
void CopyLenChar(u8  *strDest, u8 * strSrc, u8 length)
{
	u8  i;

	do
	{
		i = *strSrc++;
		*strDest++ = i;
		length--;
	}while(length != 0);
	//return(j);
}
*/
/********************************************************************
* ���� : CalTel
* ���� : ����绰���������"**,**"
*
* ����: teldat :�绰����
*       length :�б����ݳ���
* ���: relen  :��Ч�绰�����б���,�ó��Ȱ��������ż�����,","
*		telcount:�绰���������0�޵绰��>0�绰����
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u8 CheckTelFormat(u8  * teldat, u8 length, u8  * relen)
{
  u8  chtemp,i,telcount;
  
  chtemp = *teldat++;
  if(chtemp != '"')
    return 0;			//���Ϸ�������0���绰
  
  telcount = 0;
  length--;
  i = 1;
  do{
    chtemp = *teldat++;
    if(chtemp == ',' || chtemp == '"' )
      telcount++;
    //else if(chtemp < 0x30 || chtemp > 0x39)
    //	return 0;

    length--;
    i++;
  }while(chtemp != '"' && length != 0);
  
  if(chtemp != '"')
    return 0;
  
//	if(relen != 0)
  *relen = i;
          
  return(telcount);
}
/********************************************************************
* ���� : StrLenCompare
* ���� : �Ƚ� �ַ���buff �� �ַ���source ǰ length ���ַ�
*
* ����: buff   : �Ƚϵ��ַ���0
*       source : �Ƚϵ��ַ���1
*       length : �Ƚϵĳ���
* ���: state  : 0:�����ַ������;  ��0:�����ַ�������;
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
/*
u8 StrLenCompare(u8  * buff, u8 * source, u8 length)
{
	u8  dat,dat0;

	do
	{
		dat = *buff++;
		dat0 = *source++;
		length--;
		if(dat != dat0)
			return(1);
	}while(length != 0);
	
	return(0);
}
*/

/********************************************************************
* ���� : FindStr
* ���� : ��һ���ִ��в���������ִ�
*
* ����: buff   : 
*       source : 
*       length : 
* ���: state  : 
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
/*
//��һ���ִ��в���������ִ�
u8 FindStr(u8  * buff, u8 * source, u8 length)
{
	u8  dat,dat0;

	do
	{
		dat = *buff++;
		dat0 = *source++;
		length--;
		if(dat != dat0)
			return(1);
	}while(length != 0);
	
	return(0);
}
*/

/********************************************************************
* ���� : ConvertHex2Asc
* ���� : ��1��8bit���� ת��Ϊ 2��ASCII��
*
* ����: source : Ҫת���� 8bit���� 
*       destin : ת�����ASCII������
*       length : ת���� 8bit���ݳ���
* ���: count  : 
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u32 ConvertHex2Asc(u8 * source, u8 * destin, u32 length)
{
	u8  temp, datbak;//,i;
	u32  count;
	
	if(length == 0) {
		return(0);
	}
	
	count = 0;
	do{
		temp = 	*source++;
		//һ���ֽ����ݣ����2���ַ�����
		datbak = temp & 0xF0;		//high 4 bit
		datbak >>= 4;
		
		*destin++ = ASC[datbak];
		
		temp &= 0x0F;				//low 4 bit
		*destin++ = ASC[temp];
		count+=2;	
		length--;	
	}while(length != 0);

	return(count);	
}

/********************************************************************
* ���� : Convert2Asc2Hex
* ���� : ��2���ֽڵ�ASCII�� �ϳ� 1��HEX�ֽ�
*
* ����: source : Ҫת���� ASCII������
*       destin : ת����� HEX���� 
*       length : ת���� HEX�����鳤��
* ���: 1:�Ϸ��ַ���0���Ƿ��ַ�
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u8 Convert2Asc2Hex(u8  * source, u8 * destin, u32 length)
{
	u8  temp,temp0;
	u32  count;
	
	count = 0;
	
	do{
		////2���ֽ�ASC����ַ����ϳ�һ��HEX�ֽ�
		temp0 = *source++;
		temp = *source++;
		if(temp0 > 0x60 && temp0 < 0x67)
			temp0 -= 0x57;
		else if(temp0 > 0x40 && temp0 < 0x47)
			temp0 -= 0x37;
		else if(temp0 >= 0x30 && temp0 <= 0x39)
			temp0 &= 0x0F;
		else return 1;
		temp0 <<= 4;
		
		if(temp > 0x60 && temp < 0x67)
			temp -= 0x57;	
		else if(temp > 0x40 && temp < 0x47)
			temp -= 0x37;
		else if(temp >= 0x30 && temp <= 0x39)
			temp &= 0x0F;
		else return 1;
		
		temp |= temp0;
		
		*destin++ = temp;
		count++;
	}while(count != length);
	
	return 1;
}

/********************************************************************
* ���� : ConvertDec2Hex
* ���� : �� 2���ֽڵ�10�������� ת��Ϊ 16��������
*
* ����: ch1 : 10�������ݵ� 10λ
*       ch2 : 10�������ݵ� ��λ
* ���: hexData : 16��������
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u8 ConvertDec2Hex(u8 ch1, u8 ch2)
{
	u8  chtemp;
	
	chtemp = ch1;
	chtemp &= 0x0F;
	chtemp = (chtemp << 3) + (chtemp << 1);		//b=Hi*10
	chtemp += (ch2 & 0x0F);						//b+=Lo
	
	return(chtemp);
}


/********************************************************************
* ���� : CalCheckSum
* ���� : ���� ����� У���(���)
*
* ����: buff   : ҪУ�������
*       length : ��ҪУ������ݳ���
* ���: result : 0: ��ʾУ������ȷ;  ��0:����
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
u8 CalCheckSum(u8 * buff, u32 length)
{
	u8  checksum,dattemp;
	
	checksum = 0;
	do{
		dattemp = *buff++;
		checksum ^= dattemp;
		length--;
	}while(length != 0);
	
	return(checksum);
}

/********************************************************************
* ���� : ConvertAsc2Asc
* ���� : ������λ����
*
* ����: source : 
*       destin :
*       length : 
* ���: ��
*
* ȫ�ֱ���: 
* ���ú���:
*
* �ж���Դ:  (û��,��д)
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
void ConvertAsc2Asc(u8  * source, u8  * destin, u8 length)
{
	u8  count,temp,i,datbak;
	
	count = 0;
	
	if(length == 0) {
		return;
	}
	
	do{
		datbak = 0;
		for(i = 0; i < 7; i++)
		{	
			////2���ֽ�ASC����ַ����ϳ�һ��HEX�ֽ�
			Convert2Asc2Hex(source, &temp, 1);
			source += 2;	
					
			*(destin + count) = ((temp << i) & 0x7F) | datbak;
			datbak = temp >> (7 - i);
			count++;
			
			if(i == 6) 
			{
				*(destin + count) = temp >> 1;
				count++;
			}
			
			if(count >= length) 
				break;
		}
	
	}while(count < length);
}


