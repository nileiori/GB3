/*******************************************************************************
 * File Name:			Gdi.h 
 * Function Describe:	132*64 LCD init and Display Module
 * Relate Module:		device.h,lcd.h
 * Explain:				modify from joneming's code
 * Writer:				joneming
 * Date:				2012-01-03
 * Rewriter:
 * Date:		
 *******************************************************************************/
 /*******************************************************************************
 * File: Gdi.c
 * Desc: �ڴ滭ͼ
 *******************************************************************************/
#ifndef _GDI_H_
#define _GDI_H_
//����
#define OVER	0x00
#define OR		0x01
#define XOR		0x02
#define GRAY	0x03
#define SCR_WIDTH		132
#define SCR_HEIGHT		64

#define NEW_LCD_DRIVER_GDI
//lcd initialize,must be used before use this module 

void LcdClearScreen(void);
void LcdCopyScreen(void);
void LcdPasteScreen(void);

/********************************************************************************
 * Function:LcdShowPic
 * picture's width = width , height = height
 *******************************************************************************/
void DrawPic(
	unsigned char x,
	unsigned char y,
	unsigned char width,
	unsigned char height,
	const unsigned char *p_pucPic,
	unsigned char mode
);
void LcdPaintScreen
(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
);

void LcdClearArea
(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
);

void LcdCopyArea
(
	unsigned char x1,
	unsigned char y1,
	unsigned char width,
	unsigned char height,
	unsigned char x2,
	unsigned char y2
);

void LcdShowVerLine
(
	unsigned char x,
	unsigned char y1,
	unsigned char y2,
	unsigned char mode
);

void LcdShowHorLine
(
	unsigned char x1,
	unsigned char x2,
	unsigned char y,
	unsigned char mode
);

void LcdShowRect
(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2,
	unsigned char mode
);

void LcdReverseBar
(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
);

void LcdShowArrow
(
	unsigned char x,
	unsigned char y,
	unsigned char type,
	unsigned char len,
	unsigned char mode
);

void LcdShowBorder(void);



void LcdShowPic
(
	unsigned char x,
	unsigned char y,
	unsigned char width,
	unsigned char height,
	const unsigned char *p_pucPic
);

void LcdShowWindow
(
	unsigned short x1,
	unsigned char y1,
	unsigned short x2,
	unsigned char y2
);
/******************************************************************************
 * Function:LcdShowBorder
 * show a border
 ******************************************************************************/
void LcdShowStr
(
	unsigned char x,
	unsigned char y,
	const char *string,
	unsigned char mode
);
/********************************************************************************
*Function:LcdShowPic
*picture's width = width , height = height
*******************************************************************************/
unsigned char LcdShowStrEx
(
	unsigned char x,
	unsigned char y,
	const char *string,
	unsigned char mode,
	unsigned short length
);
/********************************************************************************
*Function:LcdShowPic
*picture's width = width , height = height
*******************************************************************************/
void LcdShowPic(
	unsigned char x,
	unsigned char y,
	unsigned char width,
	unsigned char height,
	const unsigned char *p_pucPic
);
/******************************************************************************
 * Function:LcdShowButton
 * show a border
 ******************************************************************************/
void LcdShowButton(char *str,unsigned short x,unsigned char y,unsigned char mode);
/*************************************************************
** ��������: LcdShowCaption
** ��������: ������ʾ�ַ���(ֻ����ʾһ����)
** ��ڲ���: str�ַ����׵�ַ,y:��ʾ���ݵ�Y����(��64������)
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void LcdShowCaption(char *str,unsigned char y);
/*************************************************************
** ��������: LcdShowCaptionEx
** ��������: ������ʾ�ַ���(��ʾ������)
** ��ڲ���: str�ַ����׵�ַ,y:��ʾ���ݵ�Y����(��64������)
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void LcdShowCaptionEx(char *str,unsigned char y);
/******************************************************************************
 * Function:LcdShowWaitPlease
 * show a border
 ******************************************************************************/
void LcdShowWaitPlease(unsigned short x,unsigned short y, char *str);
/*************************************************************
** ��������: LcdShowMsg
** ��������: ��ʾ��ʾ��Ϣ(ֻ����ʾһ����,������),��ʱ������ʾ֮ǰ�Ľ���
** ��ڲ���: msg�ַ����׵�ַ,time:��ʾʱ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void LcdShowMsg(char *msg,unsigned long time);
/*************************************************************
** ��������: LcdShowMsgEx
** ��������: ��ʾ��ʾ��Ϣ(��ʾ������,����),��ʱ������ʾ֮ǰ�Ľ���
** ��ڲ���: msg�ַ����׵�ַ,time:��ʾʱ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void LcdShowMsgEx(char *msg,unsigned long time);
/******************************************************************************
 * Function:Lcd_DisplayString
 * show a border
 ******************************************************************************/
void Lcd_DisplayString(unsigned char Line, unsigned char StartColumn, unsigned char DisplayModule, unsigned char *pBuffer, unsigned short length);
/******************************************************************************
 * Function:Lcd_DisplayOneChineseCharacter
 * show a border
 ******************************************************************************/
unsigned char Lcd_DisplayOneChineseCharacter(unsigned char Line, unsigned char StartColumn, unsigned char DisplayModule, unsigned char *pBuffer);
/******************************************************************************
 * Function:Lcd_DisplayNumber16
 * show a border
 ******************************************************************************/
void Lcd_DisplayNumber16(unsigned char Line, unsigned char StartColumn,unsigned char DisplayModule,unsigned char val);
/******************************************************************************
 * Function:Lcd_DisplayString
 * show a border
 ******************************************************************************/
void ClearLcdCopyFlag(void);
/******************************************************************************
 * Function:Lcd_DisplayString
 * show a border
 ******************************************************************************/
unsigned char GetLcdCopyFlag(void);
/******************************************************************************
 * Function:LcdShowOnePage
 * show a border
 ******************************************************************************/
unsigned char LcdShowOnePage(unsigned char *pBuffer);
#endif//_GDI_H_
/*******************************************************************************
 *                               End of File
 *******************************************************************************/
