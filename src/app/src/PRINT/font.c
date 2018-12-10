#include "stm32f10x.h"
#include "string.h"
#include "font.h" 
#include "lcd_driver.h"

#if FONT_LIB_IN_SPI_FLASH == 0
#include "font_lib.c"
#endif

static u32 read_font_memory(u32 addr, u32 cnt, u8 *buf)
{
#if FONT_LIB_IN_SPI_FLASH == 0
	while(cnt > 0)
	{
		*buf++ = READ_FONT_MEM_BYTE(addr++); //���ֿ�洢���ж�ȡһ���ֽ�
		cnt--;
	}
#else
	READ_FONT_MEM(addr,cnt,buf);
#endif
	return cnt;
}

#if 0
void image_conv(const u8 *image_in, u8 *image_out)
{
   u8 i,j;
   u16 word; 
   u8 temp[32];
   u8 tmp_buf[32];

   for(i=0;i<32;i++)
   	temp[i]=image_in[i];
   
   for(j=0;j<8;j++)
   {
     word=0;
	 
     for(i=0;i<16;i++) 
	 {
	   if(temp[i*2+1]&(1<<j))word|=0x8000>>i;
	 }
	 
	 tmp_buf[j*2]=(u8)(word>>8);
	 tmp_buf[j*2+1]=(u8)word;
   }
   
   for(j=0;j<8;j++)
   {
     word=0;
     for(i=0;i<16;i++)
	 {
	   if(temp[i*2]&(1<<j))word|=0x8000>>i;
	 }
     tmp_buf[(8+j)*2]=(u8)(word>>8);
	 tmp_buf[(8+j)*2+1]=(u8)word;
   } 

   memcpy(image_out,tmp_buf,32);
 }
#else
void image_conv(const u8 *image_in, u8 *image_out)
{
   u8 i,j;
   u16 word; 
   u8 temp[32];
   u8 tmp_buf[32];

   for(i=0;i<32;i++)
   	temp[i]=image_in[i];
   
   for(j=0;j<8;j++)
   {
     word=0;
	 
     for(i=0;i<16;i++) 
	 {
	   if(temp[i]&(1<<j))word|=0x8000>>i;
	 }
	 
	 tmp_buf[j*2]=(u8)(word>>8);
	 tmp_buf[j*2+1]=(u8)word;
   }
   
   for(j=0;j<8;j++)
   {
     word=0;
     for(i=0;i<16;i++)
	 {
	   if(temp[i+16]&(1<<j))word|=0x8000>>i;
	 }
     tmp_buf[(8+j)*2]=(u8)(word>>8);
	 tmp_buf[(8+j)*2+1]=(u8)word;
   } 

   memcpy(image_out,tmp_buf,32);
 }
#endif
/********************************************************************/
/*  �������ƣ�void zoom_hz(char *hzbuf,char *fontbuf,int w,int h,   */
/*                         int xscal)                               */
/*  �������ܣ������ַŴ����                                      */
/*  �Ρ�������hzbuf:   ����ԭʼ����                                 */
/*            fontbuf: �������Ŵ����ĵ���                         */
/*            w:	     ����ԭʼ����Ŀ��                           */
/*            h:	     ����ԭʼ����ĸ߶�                           */
/*            xscal:	 �������ź�ĵ���Ŀ��                       */
/*  ȫ�ֱ�������                                                    */
/*  �� �� ֵ����                                                    */
/*  ����ǰ�᣺�û���ʹ�õ����ֿ�оƬʱż��Ҫ�õ��������        */
/*            ��˾�ĺ��ֿ�оƬ�޷��û�Ҫ�����û�������Ҫ�󲻸�ʱ  */
/*  �� �� �ã���                                                    */
/*  �������ߣ�                                                      */
/*  �ա����ڣ�                                                      */
/*  ע    �ͣ�                                               */
/*  �ա����ڣ�2007-04-16                                            */
/*  �޸ļ�¼����                                                    */
/*  ˵    ��  ���÷�����                                            */ 
/*	          �����16X16ԭʼ�����ַŴ�32X32�����֣���        */
/*	          hzbuf--ԭʼ����                                       */
/*	          fontbuf--���ش�����                                 */
/*	          w--16                                                 */
/*	          h--16                                                 */
/*	          xscal--32                                             */
/*	          ����zoom_hz(hzbuf16,fontbuf64,16,16,32);              */
/********************************************************************/
void zoom_hz(char *hzbuf,char *fontbuf,int w,int h,int xscal)
{
        int x,y,ix,iy,ix1,iy1,ix2,iy2,ix3,iy3,c,c1,c2,c3,w1=0,wbit;
        /*bool keyx=0,keyy=0;*/
        char keyx=0,keyy=0;
        int fx,fy;
        char contbuf[512];

        memset(contbuf,0,sizeof(contbuf));
        /*keyx = keyy = 0;
        if ( xscal > 32 )	
                xscal = 32;*/
        w1 = w1/4;

        for(y=0;y<h;y++)
        {
                for(x=0;x<w;x++)
                {
                        c1 = c2 = c3  = 0;
                        wbit = x+y*w;
                        c = ((*(hzbuf+(wbit>>3))>>(7^(wbit&0x07)))&0x01);
                        if ( !c )
                                continue;
                        if ( x && x < w-1 )
                        {
                                c1 = ((*(hzbuf+((wbit-1)>>3))>>(7^((wbit-1)&0x07)))&0x01);      /*��ǰ���ǰ��*/   
                                c2 = ((*(hzbuf+((wbit+1)>>3))>>(7^((wbit+1)&0x07)))&0x01);      /*��ǰ��ĺ��*/   
                        }                                                                                       
                        if ( y && y <= w-1 )                                                                    
                        {                                                                                       
                                c3 = ((*(hzbuf+((wbit-w)>>3))>>(7^((wbit-w)&0x07)))&0x01);      /*��ǰ����ϵ�*/   
                                /*c4 = ((*(hzbuf+((wbit+w)>>3))>>(7^((wbit+w)&0x07)))&0x01);*/    /*��ǰ����µ�*/ 
                        }
                        fx = x*xscal + w1;
                        fy = y*xscal + w1;

                        if ( xscal <= w1 )
                        {
                                if ( c1 && !c2 )
                                        continue;
                        }
                        ix = ix2 = fx/w;
                        iy = iy1 = fy/w;
                        fx += xscal;
                        fy += xscal;
                        ix1 = ix3 = fx/w;
                        iy2 = iy3 = fy/w;
                        wbit = ix + iy*xscal;

                        if ( c )
                        {
                                if ( y && !c3 && ((*(contbuf+((wbit-xscal)>>3))>>(7^((wbit-xscal)&0x07)))&0x01) )
                                        continue;
                                else
                                        *(contbuf+(wbit>>3)) |= (1<<(7^(wbit&0x07)));
                                if ( ix1>ix && iy2>iy )/*������ַŴ��򲹵�*/   
                                {
                                	*(contbuf+((ix1-1+iy1*xscal)>>3)) |= (1<<(7^((ix1-1+iy1*xscal)&0x07)));
                                	*(contbuf+((ix2+(iy2-1)*xscal)>>3)) |= (1<<(7^((ix2+(iy2-1)*xscal)&0x07)));
                                	*(contbuf+((ix3-1+(iy3-1)*xscal)>>3)) |= (1<<(7^((ix3-1+(iy3-1)*xscal)&0x07)));
                                }
                        }
                        keyx = ~keyx;
                }
                keyy = ~keyy;
        }
        memcpy(fontbuf,contbuf,sizeof(contbuf));
}

/*****************************************************
** ������: get_dot_image
** ��������: ��ȡ�ַ��ĵ�������
** ����˵��: code: �ַ��ı���(GBK���ASC��)
             font: ����
             dot_image: ��Ŷ�ȡ���ĵ������εĻ�����
******************************************************/
int get_dot_image(u16 code, u8 font, u8 *dot_image)
{
	//const char *image = NULL;
	u16 size;
	
#if FONT_LIB_IN_SPI_FLASH == 0
  	u16 tables, i;
#endif

	u32 addr = NULL;
	
	switch(font)
	{
	case _16x16:
		if(code > 0x00ff)
		{	
			size = 16 * 2;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _16x16_ASC_BASE_ADDR;
		#else
		  	//tables = sizeof(ch_font_16x16_table)/sizeof(struct _ch_table);
			//for(i = 0; i < tables; i++)
			//{
			  //	if(ch_font_16x16_table[i].gbk == code)
			 	//	addr = (u32)ch_font_16x16_table[i].image; 
			//}
		#endif
	  		
		}
		else
		{
			size = 16 * 2;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _16x16_ASC_BASE_ADDR;
		#else
			//addr = (u32)ascii_16x16 + (code - ' ')*size;
		#endif
		}
		break;
		
	case _24x24:
		if(code > 0x00ff)
		{	
			size = 24 * 3;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _24x24_ASC_BASE_ADDR;
		#else
		  	tables = sizeof(ch_font_24x24_table)/sizeof(struct _ch_table);
			for(i = 0; i < tables; i++)
			{
			  	if(ch_font_24x24_table[i].gbk == code)
			 		addr = (u32)ch_font_24x24_table[i].image; 
			}
		#endif
	  		
		}
		else
		{
			size = 24 * 2;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _24x24_ASC_BASE_ADDR;
		#else
			addr = (u32)ascii_24x24 + (code - ' ')*size;
		#endif
			
		}
		break;
		
	case _32x32:
		if(code > 0x00ff)
		{	
			size = 32 * 4;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _32x32_ASC_BASE_ADDR;
		#else
		  	tables = sizeof(ch_font_32x32_table)/sizeof(struct _ch_table);
			for(i = 0; i < tables; i++)
			{
			  	if(ch_font_32x32_table[i].gbk == code)
			 		addr = (u32)ch_font_32x32_table[i].image; 
			}
	  	#endif
		
		}
		else
		{
			size = 32 * 2;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _32x32_ASC_BASE_ADDR;
		#else
			//addr = (u32)ascii_32x32 + (code - ' ') * size;
		#endif
			
		}
		break;

	default:
		if(code > 0x00ff)
		{	
			size = 16 * 2;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _16x16_ASC_BASE_ADDR;
		#else
		  	//tables = sizeof(ch_font_16x16_table)/sizeof(struct _ch_table);
			//for(i = 0; i < tables; i++)
			//{
			//  	if(ch_font_16x16_table[i].gbk == code)
			// 		addr = (u32)ch_font_16x16_table[i].image; 
			//}
		#endif
	  		
		}
		else
		{
			size = 16 * 2;
			
		#if FONT_LIB_IN_SPI_FLASH
			addr = _16x16_ASC_BASE_ADDR;
		#else
			//addr = (u32)ascii_16x16 + (code - ' ')*size;
		#endif
		}
		break;
	}


	if(addr != NULL)
	{
		read_font_memory(addr, size, dot_image);
		
		return size;	
	}
	
	return 0;
}

/*****************************************************
** ������: get_dot_image
** ��������: ��ȡ�ַ��ĵ�������
** ����˵��: code: �ַ��ı���(GBK���ASC��)
             font: ����
             dot_image: ��Ŷ�ȡ���ĵ������εĻ�����
******************************************************/
u8 ziku_EnglishConversion(u8 *zifu,u8 *dot_image)
{
   u16 temp=0;
   u8 data=0;

  unsigned char i=0,j=0,k=0,n=0,l=0;
  k=0;data=0;
  for(i=0;i<16;)
        {
                temp=0;
                k=0;
                n=0;
                for(n=0;n<8;n++)
                        {
                                if(i==7)
                                {
                                        i=0;
                                }
                                else if(i==15)
                                {
                                      i=8;
                                }
                                for(j=0;j<7;j++)
                                {
                                        temp=0x01&(zifu[i]>>k);
                                        data=data|(temp<<(7-j));
                                        i++;
                                }
                        l++;
                        *(dot_image++)=data;
                        data=0;
                        temp=0;
                        j=0;
                        k++;
                        }
                i++;
        }
  return l*2;
}
/*****************************************************
** ������: get_dot_image
** ��������: ��ȡ�ַ��ĵ�������
** ����˵��: code: �ַ��ı���(GBK���ASC��)
             font: ����
             dot_image: ��Ŷ�ȡ���ĵ������εĻ�����
******************************************************/
u8 ziku_ChineseConversion(u8 *zifu,u8 *dot_image)
{
   u16 temp=0;
   u16 data=0;

  unsigned char i=0,j=0,k=0,n=0,l=0;
  k=0;data=0;
  for(i=0;i<32;)
        {
                temp=0;
                k=0;
                n=0;
                for(n=0;n<8;n++)
                        {
                                if(i==15)
                                {
                                        i=0;
                                }
                                else if(i==31)
                                {
                                      i=16;
                                }
                                for(j=0;j<15;j++)
                                {
                                        temp=0x01&(zifu[i]>>k);
                                        data=data|(temp<<(15-j));
                                        i++;
                                }
                        l++;
                        *(dot_image++)=data>>8;
                        *(dot_image++)=data;
                        data=0;
                        temp=0;
                        j=0;
                        k++;
                        }
                i++;
        }
  return l*2;
}

/*****************************************************
** ������: get_dot_image
** ��������: ��ȡ�ַ��ĵ�������
** ����˵��: code: �ַ��ı���(GBK���ASC��)
             font: ����
             dot_image: ��Ŷ�ȡ���ĵ������εĻ�����
******************************************************/
int get_dot_image_Flash(u16 code, u8 font, u8 *dot_image)
{

	unsigned char zifu[32];
       	unsigned char converzifu[32];
 
	//u16 temp=0;
	//u16 data=0;

	//unsigned char i=0,j=0,k=0,n=0,m=0,l=0;

	
	unsigned char hiByte;
	unsigned char loByte;
	unsigned long Address;
	hiByte=code>>8;
	loByte=code;
    if(!hiByte)								//english
    {
      return  get_dot_image( code,  font, dot_image);
      /*
        Address = (loByte <<4);
        //��ȡ�ֿ�
        Lcd_ReadOneEnglishCharacterFromFlash(Address, zifu);
        ziku_EnglishConversion(zifu,dot_image);
        
        memcpy(converzifu,dot_image,16);

        zoom_hz(converzifu,dot_image,8,16,16);
        return 48;*/
    }
    else									//chinese
    {
        Address = ((hiByte-0xa1)*94 + ((loByte-0xa1))<<5);
        //��ȡ�ֿ�
        Lcd_ReadOneChineseCharacterFromFlash(Address, zifu);
        ziku_ChineseConversion(zifu,dot_image);
        
        memcpy(converzifu,dot_image,32);

        zoom_hz((char *)converzifu,(char *)dot_image,16,16,24);
        return 72;
        //��ȡ�ֿ�
        //Lcd_ReadOneChineseCharacterFromFlash(Address, dot_image);        
        //width =16;
    }

}


int image_init(struct _image *image, u8 *data)
{
	image->data = data;
	image->width = 0;
	image->height = 0;
	
	return 1;
}
