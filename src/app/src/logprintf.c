/******************************************************************** 
//��Ȩ˵��	:
//�ļ�����	:logprintf.c		
//����		:��־��ӡ����
//�汾��	:
//������	:yjb
//����ʱ��	:2012.6
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:1.���͸�ʽ���ַ���
//               
***********************************************************************/
//****************�����ļ�*****************
#include <string.h>
#include <ctype.h>        //isdigit()�ж��ַ��Ƿ�Ϊ����,
#include <stdarg.h>       //��һ�ɱ����
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "Usart_drive.h"
#include "logprintf.h"

//****************�궨��********************

//****************�ⲿ����*******************

//****************ȫ�ֱ���*******************
USART_TypeDef* pLogUSART = NULL;   //Log�������ָ��

static int do_padding;
static int left_flag;
static int len;
static int num1;
static int num2;
static char pad_character;

//****************��������*******************
void LOG_putc(int ch);
void LOG_putstr(char* str);

//****************��������******************
/**
  * @brief  ������־�������
  * @param  None
  * @retval None
  */
void LOG_SetUsart(COM_TypeDef COM)
{
   pLogUSART = GetUsartPtr(COM);
}

/**
  * @brief  ����һ���ַ�
  * @param  None
  * @retval None
  */
void LOG_putc(int ch)
{
    if(pLogUSART != NULL)
    {
        while((pLogUSART->SR & USART_FLAG_TC) == RESET);
        pLogUSART->DR = (u8)ch;
    }
}

/**
  * @brief  ����һ���ַ���
  * @param  None
  * @retval None
  */
void LOG_putstr(char* str)
{
  for(; *str; str++)
  {
      LOG_putc(*str);
  }
}


/**
  * @brief  ����һ�����ݻ�����
  * @param  None
  * @retval None
  */
static void padding(const int l_flag)
{
	int i;
	
	if(do_padding && l_flag && (len < num1))
	{
		for(i=len; i<num1; i++)
		{
	  	  LOG_putc(pad_character);
		}
	}
}

/**
  * @brief  ����ַ���
  * @param  None
  * @retval None
  */
static void outs(char* lp)
{
	/* ����ַ������*/
	len = strlen( lp);
	padding(!left_flag);
	
	/* ����ַ�����num2Ϊ����������ȣ�����%.5s,��num2=5 */
	while(*lp && num2--)
	{
		LOG_putc(*lp++);
	}
	
	/* ����ַ������ұ� */
	len = strlen(lp);
	padding(left_flag);
}
 /*
  * @brief  ������(10���ƻ�16������)ת��Ϊ�ַ���
  * @param  num  : ����
  * @param  negative  : �Ƿ�Ϊ����,����Ϊ1,����Ϊ0
  * @param  base : ��Ҫ������ַ�����,10=10������,16=16������
  * @retval None
  */
static void reoutnum(unsigned long num, unsigned int negative, const long base) 
{
	char* cp;
	char outbuf[32];
	const char digits[] = "0123456789ABCDEF";
	
	/* Build number (backwards) in outbuf             */
	cp = outbuf;
	do
	{
		*cp++ = digits[(int)(num % base)];
	} 
	while((num /= base) > 0);

	if(negative)
	{
	  *cp++ = '-';
	}

	*cp-- = 0;
	
	/* ת����������ƶ������������������Ҫ���Ĳ��� */
	len = strlen(outbuf);
	padding(!left_flag);
	while(cp >= outbuf)
	{
		LOG_putc( *cp--);
	}
	padding( left_flag);
}

 /*
  * @brief  ������ת��Ϊ�ַ����
  * @param  num  : ����
  * @param  base : ��Ҫ������ַ�����,10=10������,16=16������
  * @param  sign : �Ƿ��з���,1-�з�����,0-���޷�����
  * @retval None
  */
static void outnum(long num, const long base, unsigned char sign /* 1-signed, 0-unsigned */)
{
	unsigned int negative;

	if((num < 0) && sign ) 
	{
		negative=1;
	        num = -num;
	}
	else
	{
		negative=0;
	}
	
	reoutnum(num, negative, base);  
}

 /*
  * @brief  ������������ת��Ϊ�ַ����
  * @param  num  : ����(������*1000)
  * @param  base : ��Ҫ������ַ�����,10=10������,16=16������
  * @param  sign : �Ƿ��з���,1-�з�����,0-���޷�����
  * @retval None
  */
static void outfloat(long num, unsigned char sign /* 1-signed, 0-unsigned */)
{
	unsigned int negative;              //1-����,0-����
	char* cp;
	char outbuf[32];
	const char digits[] = "0123456789";
        long base =10;                      //10������
        long tmp = 0;                       //��ʱ������
        int tmpNum = 0;
	if((num < 0) && sign )              //������з���������num<0��˵���Ǹ���
	{
		negative=1;
	        num = -num;
	}
	else
	{
		negative=0;
	}
	cp = outbuf;
        tmp = num%1000;                   //ȡС������
	do
	{
		*cp++ = digits[(int)(tmp % base)];
	} 
	while((tmp /= base) > 0); 
        
        *cp++ = '.';                      //��С����
        tmp = num/1000;                    //��С1000��
	do
	{
                tmpNum = (int)(tmp % base);
		*cp++ = digits[tmpNum];
	} 
	while((tmp /= base) > 0);         
        
	if(negative)                      //�����Ҫ�Ӹ���.
	{
	  *cp++ = '-';
	}
        
	*cp-- = 0;                        //�����ַ���������
	
	/* ת����������ƶ������������������Ҫ���Ĳ��� */
	len = strlen(outbuf);
	padding(!left_flag);
	while(cp >= outbuf)
	{
		LOG_putc( *cp--);
	}
	padding( left_flag);
}
 /*
  * @brief  ���ַ�ת��Ϊ����
  * @param  linep  : �������ֵ��ַ���,ת����󲢽�ָ���ƶ������ֲ��ֺ��档
  * @retval None
  */
static int getnum(char** linep)
{
	int n = 0;
	char* cp;
	
	cp = *linep;
	while(isdigit(*cp))     //isdigit()�ж��ַ��Ƿ�Ϊ����
	{
		n = n*10 + ((*cp++) - '0');
	}

	*linep = cp;
	return n;
}

/**
  * @brief  ��ʽ���������
  *         %c ��%C��ͬ������ַ�
  *         %s ��%S��ͬ,����ַ���
  *         %d ����з�������
  *         %D ����޷�������
  *         %ld ��lD �������������
  *         %x  ���16������-�з�����
  *         %X  ���16������-�޷�����
  *         %-04 �����������ַ�����4���������ұ����0,����:("%-04d" ,12)
  *         %04  �����������ַ�����4��������������0
  *         %.5s �����ַ�������������Ϊ5
  *         %f   float*1000,�����룬�Զ����С����
  *         %-010f -˵���ں��油λ,0Ҫ��0,10�ǲ���10λ��0
  * @param  None
  * @retval None
  */
void printu(char* ctrl, ...)
{
	int long_flag;
	int dot_flag;
	char ch;
	
	va_list argp;
	va_start(argp, ctrl);
	
	for(; *ctrl; ctrl++)
	{
	  /* �ڻ������з��ָ�ʽ���ַ���'%',���滻����*/
	  if(*ctrl != '%')
	  {
	  	LOG_putc(*ctrl);
		continue;
	  }
	
	  /* ��ʼ�����б�־ */
	  dot_flag   = 0;
	  long_flag  = 0;
	  left_flag  = 0;
	  do_padding = 0;
	  pad_character = ' ';
	  num2=32767;

try_next:

                ch = *(++ctrl);		//��ȡ%����ַ�������:d,s,x
		if(isdigit(ch))         //����ַ�������,��˵����ʽ��:%2d
		{
			if(dot_flag)
			{
				num2 = getnum(&ctrl);     //���%��������ֲ���,����
			}
			else
			{
				if(ch == '0')
				{
				 	pad_character = '0';
				}
				
				num1 = getnum(&ctrl);
				do_padding = 1;
			}
			
			ctrl--;
			goto try_next;
		}
      
		switch (tolower(ch))           //��ch�ַ�ת��ΪСд��ʽ
		{
		case '%':
			LOG_putc( '%');
			continue;
		
		case '-':
			left_flag = 1;
			break;
		
		case '.':
			dot_flag = 1;
			break;
		
		case 'l':
			long_flag = 1;
			break;
		
		case 'd':
			if(long_flag == 1)
			{
				if(ch == 'D')
					outnum(va_arg(argp, unsigned long), 10L , 0);
				else
					outnum(va_arg(argp, long), 10L, 1);
			}
			else
			{
				if(ch == 'D')
					outnum(va_arg(argp, unsigned int), 10L, 0);
				else
					outnum(va_arg(argp, int), 10L, 1);      //va_arg(argp, int) ��ȡ����ֵ
			}
			continue;
			
		case 'x':
			if(long_flag == 1)
			{
				if(ch == 'X')
					outnum(va_arg(argp, unsigned long), 16L, 0);
				else
					outnum(va_arg(argp, long), 16L, 1);
			}
			else 
			{
				if(ch == 'X')
					outnum(va_arg(argp, unsigned int), 16L, 0);
				else
					outnum( va_arg(argp, int), 16L,1);
			}
			continue;
		case 'f':
			if(long_flag == 1)
			{
				if(ch == 'F')
					outfloat(va_arg(argp, unsigned long), 0);
				else
					outfloat(va_arg(argp, long), 1);
			}
			else 
			{
				if(ch == 'F')
					outfloat(va_arg(argp, unsigned int), 0);
				else
					outfloat(va_arg(argp, int),1);
			}
			continue;                        
		
		case 's':
			outs( va_arg( argp, char*));
			continue;
		
		case 'c':
			LOG_putc( va_arg( argp, int));
			continue;
		
		default:
			continue;
		}
	
		goto try_next;
	}

	va_end(argp);
}
/**
  * @brief  ��16�����������������
  *
  * @param  None
  * @retval None
  */
void printuHex(u8 *desc, u16 dlen,u8* buf,u16 len)
{
  int i = 0;
  for(i = 0 ; i < dlen ; i++)
  {
    printu("%C",desc[i]);
  }
  printu("0x");
  for(i = 0 ; i < len ; i++)
  {
    printu("%02X ",buf[i]);
  }
  printu("\r\n");
}