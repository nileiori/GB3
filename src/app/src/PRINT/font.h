#ifndef _FONT_H
#define _FONT_H 


#define FONT_LIB_IN_SPI_FLASH 0



#if FONT_LIB_IN_SPI_FLASH
//-------------定义各种字体的点阵码形存储位置，和硬件相关-------------
#define _16x16_BASE_ADDR      0x08020000
#define _16x16_ASC_BASE_ADDR  0x08010000


#define _24x24_BASE_ADDR      2
#define _24x24_ASC_BASE_ADDR 3

#define _32x32_BASE_ADDR 4
#define _32x32_ASC_BASE_ADDR 5

#define _48x48_BASE_ADDR 6
#define _48x48_ASC_BASE_ADDR 7

#define _64x64_BASE_ADDR  8
#define _64x64_ASC_BASE_ADDR  9

#define READ_FONT_MEM(addr,cnt,buf) sFLASH_ReadBuffer(buf, addr, cnt)

#else

#define READ_FONT_MEM_BYTE(addr) (*(const u8 *)(addr))

#endif
//-------------------------------------------------------




//------定义字体类型------
#define  _16x16  0 
#define  _24x24  1
#define  _32x32	 2
#define  _48x48	 3
#define  _64x64	 4
#define	 _IMAGE  5  
//------------------------


struct _image
{
	u8 *data;
	u16 width;
	u16 height;
};

//------------------输出函数-----------------
extern int get_dot_image(u16 code, u8 font, u8 *dot_image);

/*****************************************************
** 函数名: get_dot_image
** 功能描述: 获取字符的点阵码形
** 参数说明: code: 字符的编码(GBK码或ASC码)
             font: 字体
             dot_image: 存放读取到的点阵码形的缓冲区
******************************************************/
int get_dot_image_Flash(u16 code, u8 font, u8 *dot_image);

#endif
