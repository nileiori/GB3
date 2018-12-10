/*******************************************************************************
 * File Name:			Gdi.c 
 * Function Describe:	132*64 LCD init and Display Module
 * Relate Module:		device.h,lcd.h
 * Explain:				modify from joneming's code
 * Writer:				joneming
 * Date:				2012-01-03
 * Rewriter:
 * Date:		
 *******************************************************************************/ 
 /************************************************************************************************************
void LcdInitialization(void);				
	��ʼ��Һ��������

void LcdClearScreen(void);				
	���LCD

void LcdShowRect(unsigned short x1,unsigned char y1,unsigned short x2,unsigned char y2,unsigned char mode)
��ʾ����x1,y1,x2,y2Ϊ���꣬mode��ʾ��ʽ:����λ 0=ʵ�� 1=���� ������λ 0=��ʽ 1=���ʽ

void LcdShowStr(unsigned short x,unsigned char y,unsigned char *string,unsigned char mode)
��ʾ�ַ�����mode��ʾ��ʽ:����λ 0=16���� 1=24���� ������λ 0=���� 1=�� 2=��� 3=�����֣�

void LcdReverseBar(unsigned short x1,unsigned char y1,unsigned short x2,unsigned char y2)
��ɫ��ʾ����ָ�����꣩

void LcdClearArea(unsigned short x1,unsigned char y1,unsigned short x2,unsigned char y2)
�����������ָ�����꣩

void LcdCopy(unsigned short x1,unsigned char y1,unsigned short x2,unsigned char y2)
�����������򵽻�������ָ�����꣩

void LcdPaste(unsigned short x1,unsigned char y1,unsigned short x2,unsigned char y2)
�ָ����������ݣ�ָ�����꣩

void LcdShowPic(unsigned short x,unsigned char y,unsigned short width,unsigned char height,const unsigned char *p_pucPicBuff)
��ָ��������ʾλͼ

void LcdShowArrow(unsigned short x,unsigned char y,unsigned char type,unsigned char len,unsigned char mode)
��ָ��������ʾ�����Σ�����type�Ƿ��� �� 1 �� 2 �� 3 �� 4 ��   ����len ����
mode ��ʾ��ʽ��ͬ����

void LcdShowWindow(unsigned short x1,unsigned char y1,unsigned short x2,unsigned char y2)
��ָ��������ʾ����Ӱ�Ĵ���

void LcdMoveCursor(unsigned short x,unsigned char y)
�ù�굽x,yλ��

void LcdShowVerLine(unsigned short x,unsigned char y1,unsigned char y2,unsigned char mode)
��ʾ��ֱֱ�ߣ�mode ��ʾģʽ��ͬ����

void LcdShowHorLine(unsigned short x1,unsigned short x2,unsigned char y,unsigned char mode)
��ʾˮƽֱ�ߣ�mode ��ʾģʽ��ͬ����

void LcdShowChar(unsigned short x,unsigned char y,unsigned char hiByte,unsigned char loByte,unsigned char mode)
��ʾ�ַ���mode ��ʾģʽͬLcdShowStr

*******************************************************************************************************************/
#include "include.h" 


//LCD screen buffer
unsigned char g_aaucScrBuf[8][SCR_WIDTH];
unsigned char g_aaucScrCpy[8][SCR_WIDTH];
unsigned char s_ucLcdCopyFlag=0;
/*************************************************************OK
** ��������: 
** ��������: 
** ��ڲ���: 		 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void SetLcdCopyFlag(void)
{
    s_ucLcdCopyFlag=0x55;
    Lcd_SetDisplayCtrlFlag();
}
/*************************************************************OK
** ��������: SetComm485InFree()
** ��������: 
** ��ڲ���: 		 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void ClearLcdCopyFlag(void)
{
    s_ucLcdCopyFlag = 0;
    Lcd_ClearDisplayCtrlFlag();
}
/*************************************************************OK
** ��������: 
** ��������: 
** ��ڲ���: 		 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetLcdCopyFlag(void)
{
    return (s_ucLcdCopyFlag==0x55)?1:0;
}
/*********************************************************************************************************
����:LcdWriteData
����:д����
�Ľ���Slaven 
*********************************************************************************************************/
void LcdWriteData(unsigned char data)
{
    Lcd_WriteData(data);
}
/*********************************************************************************************************
����:LcdWriteCmd
����:д����
�Ľ���Slaven 
*********************************************************************************************************/
void LcdWriteCmd(unsigned char data)
{
    Lcd_WriteCom(data);
}
/******************************************************************************
 * Function:LcdMoveCursor
 * move cursor to position x,y
 ******************************************************************************/
unsigned char LcdConvertPage(unsigned char page)
{
    unsigned char StartPage;
    if(page < 4)//
    {
        StartPage = 3 - page;
    }
    else
    if(page < 8)//
    {
        StartPage = 11 - page;
    }
    else
    {
        StartPage = 4;
    }
    return StartPage;
}
/******************************************************************************
 * Function:LcdMoveCursor
 * move cursor to position x,y
 ******************************************************************************/
void LcdMoveCursor(unsigned char page, unsigned char column)
{
    //column += 1;    
    LcdWriteCmd(0x00 + (column&0x0F));    
    LcdWriteCmd(0x10 + (column>>4));
    LcdWriteCmd(0xB0 + page);    
}

/******************************************************************************
 * Function:LcdMoveCursor
 * move cursor to position x,y
 ******************************************************************************/
void LcdMoveCursorEx(unsigned char page, unsigned char column)
{
    LcdMoveCursor(LcdConvertPage(page),column);
}
/******************************************************************************
 * Function:LcdPaintScreen
 *
 ******************************************************************************/
void LcdPaintScreen
(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
)
{
    unsigned char i,j;

    if(x2 > SCR_WIDTH-1)x2 = SCR_WIDTH-1;
    if(y2 > SCR_HEIGHT-1)y2 = SCR_HEIGHT-1;
    if(x1 > x2 || y1 > y2)return;

    for(i = (y1 >> 3); i <= (y2 >> 3); i ++)
    {
        LcdMoveCursorEx(i,x1);
        for(j = x1; j <= x2; j ++)
        {
            LcdWriteData(g_aaucScrBuf[i][j]);
        }
    }
}

/******************************************************************************
 * Function:SetPixel
 * draw a pixel on x,y(save data to oscill screen buffer)
 ******************************************************************************/
void LcdSetPixel(unsigned char x,unsigned char y,unsigned char color)
{
    unsigned char temp,temp1;
    if(x >= SCR_WIDTH)return;
	if(y >= SCR_HEIGHT)return;
    temp=y>>3;
    temp1=y-(temp<<3);
    if(color)
    {
        g_aaucScrBuf[temp][x] |= 1 << (temp1);
    }
    else
    {
        g_aaucScrBuf[temp][x] &= ~(1 << (temp1));
    }
}

/******************************************************************************
 * Function:GetPixel
 * draw a pixel on x,y(save data to oscill screen buffer)
 ******************************************************************************/
unsigned char LcdGetPixel(unsigned char x,unsigned char y)
{
	unsigned char temp,temp1;
	if(x >= SCR_WIDTH)return 0;
	if(y >= SCR_HEIGHT)return 0; 
    temp=y>>3;
    temp1=y-(temp<<3);
	temp = g_aaucScrBuf[temp][x] & (1<<(temp1));
	return temp;
}
/******************************************************************************
 * Function: LcdClearArea
 * Describe: clear block at x1,y1 to x2,y2
 ******************************************************************************/
void FillArea(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2,
	unsigned char color
)
{
	unsigned char i,j;

	for(i = x1; i <= x2; i ++)
	{
		for(j = y1; j <= y2; j ++)
		{
			LcdSetPixel(i,j,color);
		}
	}
}

/******************************************************************************
 * Function: CopyArea
 * Describe: copy area at x1,y1 to x2,y2
 ******************************************************************************/
void CopyArea(
	unsigned char x1,
	unsigned char y1,
	unsigned char width,
	unsigned char height,
	unsigned char x2,
	unsigned char y2
)
{
	unsigned char i,j,k;

	if(y2 < y1)		//������
	{
		for(j = 0; j <= height; j ++)
		{
			for(i = 0; i <= width; i ++)
			{
				k = LcdGetPixel(x1+i,y1+j);
				LcdSetPixel(x2+i,y2+j,k);
			}
		}
	}
	else if(y2 > y1)	//������
	{
		for(j = 0; j <= height; j ++)
		{
			for(i = 0; i <= width; i ++)
			{
				k = LcdGetPixel(x1+i,y1+(height-j));
				LcdSetPixel(x2+i,y2+(height-j),k);
			}
		}
	}
	else
	{
		//������
	}
}

/******************************************************************************
 * Function:DrawVerLine
 * show a vertical line at x,y1,to x,y2
 * parameter mode low 4 bit is 0 = 'or' ,1 = 'xor'
 *               high 4 bit is 0 = real line,1 = broken line
 ******************************************************************************/
void DrawVerLine(
	unsigned char x,
	unsigned char y1,
	unsigned char y2,
	unsigned char mode
)
{
	unsigned char i;

	if(mode & 0x0F)		//'XOR'
	{
		for(i = y1; i <= y2; i ++)LcdSetPixel(x,i,!LcdGetPixel(x,i));
	}
	else
	{
	
		if(mode&0xf0) //broken line
		{	

			for(i = y1; i <= y2; i ++)
			{
				if(i%2 || i==y1)LcdSetPixel(x,i,1);
				else LcdSetPixel(x,i,0);
			}
		}
		else//real line
		{
			for(i = y1; i <= y2; i ++)LcdSetPixel(x,i,1);
		}
	}
}
/******************************************************************************
 * Function:DrawHorLine
 * show a horizontal line at x1,y,to x2,y
 * parameter mode low 4 bit is 0 = 'or' ,1 = 'xor'
 *               high 4 bit is 0 = real line,1 = broken line
 ******************************************************************************/
void DrawHorLine(
	unsigned char x1,
	unsigned char x2,
	unsigned char y,
	unsigned char mode
)
{
    unsigned char i;

    if(mode & 0x0F)
    {
        for(i = x1; i <= x2; i ++)LcdSetPixel(i,y,!LcdGetPixel(i,y));
    }
    else
    {
        if(mode & 0xf0)//broken line
        {
            for(i = x1; i <= x2; i ++)
            {
                if(i%2 || i==x1)LcdSetPixel(i,y,1);
                else LcdSetPixel(i,y,0);
            }
        }
        else //real line
        {
            for(i = x1; i <= x2; i ++)LcdSetPixel(i,y,1);
        }
    }
}

/******************************************************************************
 * Function:DrawRect
 * show a rectangle line at x1,y1,to x2,y2
 * parameter mode low 4 bit is 0 = 'or' ,1 = 'xor'
 *               high 4 bit is 0 = real line,1 = broken line
 ******************************************************************************/
void DrawRect(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2,
	unsigned char mode
)
{
	if(x2 > SCR_WIDTH-1)x2 = SCR_WIDTH-1;
	if(y2 > SCR_HEIGHT-1)y2 = SCR_HEIGHT-1;
	if(x1 > x2 || y1 > y2)return;

	if(x1 == x2)								//x1=x2,draw a vertical line
		DrawVerLine(x1,y1,y2,mode);
	else
	{
		DrawHorLine(x1,x2,y1,mode);
		if(y1 == y2)return;						//y1=y2,draw a horizontal line
		DrawHorLine(x1,x2,y2,mode);
		if(y2 - 1 >= y1 + 1)
		{
			DrawVerLine(x1,y1 + 1,y2 - 1,mode);
			DrawVerLine(x2,y1 + 1,y2 - 1,mode);
		}
	}
}

/******************************************************************************
 * Function:DrawReverseBar
 * draw a bar at x1,y1 to x2,y2
 ******************************************************************************/
void ReverseBar(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
)
{
	unsigned char i,j;

	for(i = x1; i <= x2; i ++)
	{
		for(j = y1; j <= y2; j ++)
		{
			LcdSetPixel(i,j,!LcdGetPixel(i,j));
		}
	}
}

/********************************************************************************
 * Function:LcdShowArrow
 * show a arrow on x,y   type:1=left,2=right,3=up,4=down
 * len = size
 *******************************************************************************/
void DrawArrow(
	unsigned char x,
	unsigned char y,
	unsigned char type,
	unsigned char len,
	unsigned char mode
)
{
	unsigned char i;

	switch(type)
	{
		case 1:
			for(i = 0 ; i < len ; i ++)
				DrawRect(x+i,y-i,x+i,y+i,mode);
			break;
		case 2:
			for(i = 0 ; i < len ; i ++)
				DrawRect(x-i,y-i,x-i,y+i,mode);
			break;
		case 3:
			for(i = 0 ; i < len ; i ++)
				DrawRect(x-i,y+i,x+i,y+i,mode);
			break;
		case 4:
			for(i = 0 ; i < len ; i ++)
				DrawRect(x-i,y-i,x+i,y-i,mode);
			break;
		default:
			break;
	}
}
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
)
{
    
    unsigned char i,j,k,index,xx,yy,data;
    
    for(i = 0 ; i < height ; i ++)
    {     
        index=(i>>3);
        k=(index<<3);
        index *=width;
        k=7+k-i;
        yy=y+i;
        for(j = 0 ; j < width ; j ++)
        {
            xx=x+j;
            data=p_pucPic[j+index];
            if(mode)
            {
                data ^=0xff;
            }
            if(data & (0x80>>k)) LcdSetPixel(xx,yy,1);
            else LcdSetPixel(xx,yy,0);
        }
    }
}
/********************************************************************************
 * Function:LcdShowChar
 * show a character at x,y
 * parameter mode : high 4 bit is: 0 = font 16 , 1 = 24
 * low 4 bit is ways : 0 = over , 1 = or , 2 = reverse or , 3 = gray
 *******************************************************************************/
void DrawChar(
	unsigned char x,
	unsigned char y,
	unsigned char hiByte,
	unsigned char loByte,
	unsigned char mode
)
{
    unsigned char i,j,k,index,xx,yy,data;
    unsigned char DispBuffer[33];
    unsigned long Address;

    if(!hiByte)								//english
    {
        Address = (loByte <<4);
        //��ȡ�ֿ�
        Lcd_ReadOneEnglishCharacterFromFlash(Address, DispBuffer);
        for(i=0; i<16; i++)
        {
            index=(i>>3);
            index<<=3;
            k=7+index-i;
            yy=y+i;
            for(j = 0 ; j < 8 ; j ++)
            {
                xx=x+j;
                data=DispBuffer[j+index];
                if(mode)
                {
                    data ^=0xff;
                }
                if(data & (0x80>>k)) LcdSetPixel(xx,yy,1);
                else LcdSetPixel(xx,yy,0);
            }
        }
    }
    else									//chinese
    {
        Address = ((hiByte-0xa1)*94 + ((loByte-0xa1))<<5);
        //��ȡ�ֿ�
        Lcd_ReadOneChineseCharacterFromFlash(Address, DispBuffer);
        for(i=0; i<16; i++)
        {
            index=(i>>3);
            k=(index<<3);
            index <<=4;
            k=7+k-i;
            yy=y+i;
            for(j = 0 ; j < 16 ; j ++)
            {
                xx=x+j;
                data=DispBuffer[j+index];
                if(mode)
                {
                    data ^=0xff;
                }
                if(data & (0x80>>k)) LcdSetPixel(xx,yy,1);
                else LcdSetPixel(xx,yy,0);
            }
        }
        //width =16;
    }
    #if 0
    ///////////////////////////
    for(i=0; i<16; i++)
    {
        index=i/8;
        index *=width;
        k=7-i%8;
        yy=y+i;
        yy=yy%SCR_HEIGHT;
        for(j=0; j<width; j++)
        {
            xx=x+j;            
            xx=xx%SCR_WIDTH;            
            data = DispBuffer[j+index];
            ////////////////////////////
            if((mode & 0x0F) == 0x01)	//or
            {
                if(data & (0x80>>k)) LcdSetPixel(xx,yy,1);
            }
            else if((mode & 0x0F) == 0x02)	//revise or
            {
                if((data & (0x80>>k)) == 0) LcdSetPixel(xx,yy,1);
            }
            else 
            if((mode & 0x0F) == 0x04)	//revise over
            {
                if((data & (0x80>>k)) == 0) LcdSetPixel(xx,yy,1);
                else
                 LcdSetPixel(xx,yy,0);
            }
            else	//over
            {
                if(data & (0x80>>k)) LcdSetPixel(xx,yy,1);
                else LcdSetPixel(xx,yy,0);
            }
        }
    }
    #else
    //DrawPic(x,y,width,16,DispBuffer,mode);
    #endif
}
/******************************************************************************
 * Function:LcdShowStr
 * show a string at start position x,y
 * parameter mode : high 4 bit is font 16 or 24
 * low 4 bit is ways : 0 = over , 1 = or , 2 = reverse or,3 = gray
 ******************************************************************************/
unsigned char DrawStr(
	unsigned char x,
	unsigned char y,
	const char *string,
	unsigned char mode,
	unsigned short length
)
{
    unsigned char width1,width2;
    const char *p;
    unsigned char xx,yy;
    unsigned short width,length1,height,tmp;

    width1 = 16;
    width2 = 8;
    height =16;
    p = string;
    xx = x;
    yy=y;
    width=16+yy;
    tmp=0;
    ///////////////////
    length1=strlen(string);
    if(length<length1)
    {
        length1=length;
    }
    /////////////////////////
    while(*p)
    {
        if (*p > 0x80)							//chinese letter
        {
            if(xx+width1>131)
            {
                xx=0;                
                yy +=height;
                width +=height;
                if(width>64)
                {
                    yy =0;
                    width=SCR_HEIGHT;
                    break;
                }
            }            
            DrawChar(xx,yy,*p,*(p+1),mode);
            xx += width1;
            p += 2;
            tmp+=2;
        }
        else									//english letter
        {
            if(xx+width2>131)
            {
                xx=0;                
                yy +=height;
                width +=height;
                if(width>64)
                {
                    yy =0;
                    width=SCR_HEIGHT;
                    break;
                }
            }
            DrawChar(xx,yy,0,*p,mode);
            xx += width2;
            p ++;
            tmp++;
        }
        if(length1<=tmp)break;
        //////////////////////////
    }
    xx=x;
    length1 <<= 3;
    length1 +=xx;
    if(length1>=SCR_WIDTH)
    {
        xx=0;
        length1=SCR_WIDTH;
    }
    LcdPaintScreen(xx,y,length1,width);
    return tmp; 
}
/******************************************************************************
 * Function:ClearScreen
 * Clear LCD screen
 ******************************************************************************/
void LcdClearScreen(void)
{
    unsigned char i,j;

    for(i = 0; i < 8; i++)
    {
        LcdMoveCursorEx(i,0);
        for(j = 0; j < SCR_WIDTH; j++)
        {
            g_aaucScrBuf[i][j] = 0;
            LcdWriteData(g_aaucScrBuf[i][j]);		//Up D0-D7 Down	
        }
    }
    //LcdPaintScreen(0,0,SCR_WIDTH-1,SCR_HEIGHT-1);
}


/******************************************************************************
 * Function: LcdCleea
 * Describe: clear block at x1,y1 to x2,y2
 ******************************************************************************/
void LcdClearArea(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
)
{
	FillArea(x1,y1,x2,y2,0);
	LcdPaintScreen(x1,y1,x2,y2);
}

/******************************************************************************
 * Function: CopyArea
 * Describe: copy area at x1,y1 to x2,y2
 ******************************************************************************/
void LcdCopyArea
(
	unsigned char x1,
	unsigned char y1,
	unsigned char width,
	unsigned char height,
	unsigned char x2,
	unsigned char y2
)
{
	CopyArea(x1,y1,width,height,x2,y2);
	LcdPaintScreen(x2,y2,x2+width,y2+height);
}
/******************************************************************************
 * Function:DrawVerLine
 * show a vertical line at x,y1,to x,y2
 * parameter mode low 4 bit is 0 = 'or' ,1 = 'xor'
 *               high 4 bit is 0 = real line,1 = broken line
 ******************************************************************************/
void LcdShowVerLine
(
	unsigned char x,
	unsigned char y1,
	unsigned char y2,
	unsigned char mode
)
{
	DrawVerLine(x,y1,y2,mode);
	LcdPaintScreen(x,y1,x,y2);
}

/******************************************************************************
 * Function:DrawHorLine
 * show a horizontal line at x1,y,to x2,y
 * parameter mode low 4 bit is 0 = 'or' ,1 = 'xor'
 *               high 4 bit is 0 = real line,1 = broken line
 ******************************************************************************/
void LcdShowHorLine
(
	unsigned char x1,
	unsigned char x2,
	unsigned char y,
	unsigned char mode
)
{
	DrawHorLine(x1,x2,y,mode);
	LcdPaintScreen(x1,y,x2,y);
}

/******************************************************************************
 * Function:DrawRect          //������
 * show a rectangle line at x1,y1,to x2,y2
 * parameter mode low 4 bit is 0 = 'or' ,1 = 'xor'
 *               high 4 bit is 0 = real line,1 = broken line
 ******************************************************************************/
void LcdShowRect
(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2,
	unsigned char mode
)
{
	DrawRect(x1,y1,x2,y2,mode);
	LcdPaintScreen(x1,y1,x1,y2);
	LcdPaintScreen(x2,y1,x2,y2);
	LcdPaintScreen(x1,y1,x2,y1);
	LcdPaintScreen(x1,y2,x2,y2);
}

/******************************************************************************
 * Function:DrawReverseBar
 * draw a bar at x1,y1 to x2,y2
 ******************************************************************************/
void LcdReverseBar(
	unsigned char x1,
	unsigned char y1,
	unsigned char x2,
	unsigned char y2
)
{
	ReverseBar(x1,y1,x2,y2);
	LcdPaintScreen(x1,y1,x2,y2);
}

/******************************************************************************
 * Function:LcdShowArrow      //��ʾһ����ͷ
 * show a arrow on x,y   type:1=left,2=right,3=up,4=down
 * len = size
 ******************************************************************************/
void LcdShowArrow
(
	unsigned char x,
	unsigned char y,
	unsigned char type,
	unsigned char len,
	unsigned char mode
)
{
    DrawArrow(x,y,type,len,mode);
    switch(type)
    {
        case 1:
            LcdPaintScreen(x,y-len,x+len,y+len);
            break;
        case 2:
            LcdPaintScreen(x-len,y-len,x,y+len);
            break;
        case 3:
            LcdPaintScreen(x-len,y,x+len,y+len);
            break;
        case 4:
            LcdPaintScreen(x-len,y-len,x+len,y);
            break;
        default:
            break;
    }
}

/******************************************************************************
 * Function:LcdShowBorder
 * show a border
 ******************************************************************************/
void LcdShowBorder(void)
{
	LcdClearScreen();
	LcdShowRect(0,0,SCR_WIDTH-1,SCR_HEIGHT-1,0);
}

/********************************************************************************
*Function:LcdShowWindow
*show a window 
*******************************************************************************/
void LcdShowWindow(
	unsigned short x1,
	unsigned char y1,
	unsigned short x2,
	unsigned char y2
)
{
	LcdClearArea(x1,y1,x2,y2);
	DrawRect(x1+1,y1+1,x2-1,y2-1,0x00);
	LcdPaintScreen(x1,y1,x2,y2);
}

/*********************************************************************************************************
����:LcdCopyScreen
����:ȫ��copy
*********************************************************************************************************/
void LcdCopyScreen(void)
{
    unsigned char i,j;
    SetLcdCopyFlag();
    for(i=0;i<8;i++)      
    {
        for(j=0;j<SCR_WIDTH;j++)			
        {			
            g_aaucScrCpy[i][j]=g_aaucScrBuf[i][j];
        }	
    }
}

/*********************************************************************************************************
����:LcdPasteScreen
����:ȫ��Paste
*********************************************************************************************************/
void LcdPasteScreen(void)
{      	
    unsigned char i,j;	
    if(!GetLcdCopyFlag())return;
    ClearLcdCopyFlag();
    for(i=0;i<8;i++)      
    {
        LcdMoveCursorEx(i,0);
        for(j=0;j<SCR_WIDTH;j++)			
        {			
            g_aaucScrBuf[i][j]=g_aaucScrCpy[i][j];
            LcdWriteData(g_aaucScrBuf[i][j]);		//Up D0-D7 Down	
        }	
    }
}
/******************************************************************************
 * Function:LcdShowStr
 * show a string at start position x,y
 * parameter mode : high 4 bit is font 16 or 24
 * low 4 bit is ways : 0 = over , 1 = or , 2 = reverse or,3 = gray
 ******************************************************************************/
void LcdShowStr
(
	unsigned char x,
	unsigned char y,
	const char *string,
	unsigned char mode
)
{
    //unsigned char width,length;

    DrawStr(x,y,string,mode,strlen(string));

    //width = 8;	  							//16bit font

    //length = strlen(string);
    //LcdPaintScreen(x,y,x+length*width,y+width*2);

}
/******************************************************************************
 * Function:LcdShowStr
 * show a string at start position x,y
 * parameter mode : high 4 bit is font 16 or 24
 * low 4 bit is ways : 0 = over , 1 = or , 2 = reverse or,3 = gray
 ******************************************************************************/
unsigned char LcdShowStrEx
(
	unsigned char x,
	unsigned char y,
	const char *string,
	unsigned char mode,
	unsigned short length
)
{
    //unsigned char width,length;

    return DrawStr(x,y,string,mode,length);

   // width = 8;								//16bit font

    //length = strlen(string);
    //LcdPaintScreen(x,y,x+length*width,y+width*2);

}
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
)
{
	DrawPic(x,y,width,height,p_pucPic,0);
    //LcdPaintScreen(0,0,131,63);
	LcdPaintScreen(x,y,x+width,y+height);
}
/*************************************************************OK
** ��������: LcdShowButton()
** ��������: ��ʾ��ť
** ��ڲ���: string:
             x:
			 y:
			 mode:
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void LcdShowButton(char *str,unsigned short x,unsigned char y,unsigned char mode)
{
    char *p;
    unsigned char w;//ÿ�ַ��� ����
    unsigned char len;
    unsigned char x1,x2;
    unsigned char y1,y2;
    unsigned char i;

    p=str;
    len=strlen(str);
    if (*p > 0x9f)//����
    {
        w=16;
        len/=2;
    }
    else w=8;          //����
    x1=x+2;y1=y+1;x2=x1+len*w+4;y2=y1+16+2;
    LcdClearArea(x,y,x2,y2);
    LcdShowStr(x1+2, y1+1, str, 0);	 
    if(mode) 
    {
        for (i=y1+1;i<y2-1 ;i++ )LcdShowRect(x1,i,x2-1,i,0x01);
        LcdShowRect(x,y,x2+1,y2,0x00); 
    }
    else
    {
        LcdShowRect(x,y,x2+1,y2,0x00); 
    }		
}
/*************************************************************OK
** ��������: LcdShowCaption()
** ��������: ������ʾ�ַ���str����ʾ������Ϊy
** ��ڲ���: str:��ʾ�ַ���
			 y:��ʾ���ݵ�Y����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
void LcdShowCaption(char *str,unsigned char y)
{
    unsigned char len;
    unsigned char x=0;    
    len=strlen(str);
    len <<=2;
    if(len<65)
    x=65-len;
    LcdShowStr(x,y,str,0);   
}
/*************************************************************OK
** ��������: LcdShowCaptionEx()
** ��������: ������ʾ�ַ���str����ʾ������Ϊy
** ��ڲ���: str:��ʾ�ַ���
			 y:��ʾ���ݵ�Y����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
void LcdShowCaptionEx(char *str,unsigned char y)
{
    char Str[4][17];
    char source[72];
    unsigned char x,i,tmp,line,len[4],lenp;
    char *p;
    strcpy(source,str);	
    line = 0; 
    strcpy(Str[line],str);
    p = str;
    len[line]=0;
    lenp = 0;
    while(*p) 
    {
        if((unsigned char)*p > 0x9f)	//chinese letter
        {
            p+=2;
            len[line]+=2;
        }
        else
        if(*p == '\n')
        {
            if(len[line]>16)
            {
                line =0;
                strcpy(Str[line],str);
                break;   
            }
            //////////////////
            Str[line][len[line]] = 0;
            lenp += len[line];
            line++;	
            if(line>3)
            break;            
            p++;
            lenp++;
            strcpy(Str[line],p);
            len[line] = 0;
        }
        else
        {
            p++;
            len[line]++;
        }
    }
    //////////////////////////
    for(i=0; i<=line; i++)
    {
        tmp=len[i]<<2;
        if(65>tmp)
         x = 65-tmp;
        else
         x = 0;
        LcdShowStr(x,y+i*16,Str[i],0);
    }
}

/*************************************************************OK
** ��������: PublicShowWaitPlease()
** ��������: 
** ��ڲ���: str:��ʾ�ַ���
             x:��ʾ���ݵ�X����
			 y:��ʾ���ݵ�Y����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
void LcdShowWaitPlease(unsigned short x,unsigned short y, char *str)
{
    static unsigned char Dot=0;
    unsigned char len;
    len=strlen(str)*8;
    LcdShowStr(x,y,str,0x00);
	switch(Dot)
	{
	  case 0:
	  	    LcdShowStr(x+len,y,".    ",0x00);
	  	    break;
	  case 1:
	  	    LcdShowStr(x+len,y,"...  ",0x00);
	  	    break;
	  case 2:
	  	    LcdShowStr(x+len,y,".....",0x00);
	  	    break;
	 }
	 if(Dot<3)Dot++;
	 else Dot=0;
}
/*************************************************************
** ��������: LcdShowMsg
** ��������: ��ʾ��ʾ��Ϣ(ֻ����ʾһ����,������),��ʱ������ʾ֮ǰ�Ľ���
** ��ڲ���: msg�ַ����׵�ַ,time:��ʾʱ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void LcdShowMsg(char *msg,unsigned long time)
{
    unsigned short width;
    unsigned short X1,Y1,xx;
    char str[17];

    strcpy(str,msg);
    str[16]=NULL;
    X1=(unsigned short)strlen(str);
    if(X1>16)X1=16;
    width=(unsigned short)(X1<<3);
    X1=(unsigned short)(SCR_WIDTH-width);
    X1>>=1;
    xx=X1;
    /////////////////
    if(X1<10)
    {
        X1=0;
        width=SCR_WIDTH-1;
    }
    else 
    {
        X1-=10;
        width +=20;
    }
    ///////////////////////
    Y1=SCR_HEIGHT;
    Y1>>=1;
    Y1-=12;
    if(!GetLcdCopyFlag())LcdCopyScreen();
    LcdShowWindow(X1,Y1,X1+width,Y1+16+8);
    LcdShowStr(xx,Y1+4,str,0x00);
    if(time)
    {
        LZM_SetAlarm(SHOW_TIMER, time);
    }
    /////////////////
    FlashLcdBackLight();
}
/*************************************************************
** ��������: LcdShowMsgEx
** ��������: ��ʾ��ʾ��Ϣ(��ʾ������,����),��ʱ������ʾ֮ǰ�Ľ���
** ��ڲ���: msg�ַ����׵�ַ,time:��ʾʱ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void LcdShowMsgEx(char *msg,unsigned long time)
{
    unsigned char len;
    if(!GetLcdCopyFlag())LcdCopyScreen();
    LcdClearScreen();
    len=strlen(msg);
    if(len<=16)
    {
        LcdShowCaption(msg,23);
    }
    else
    {
        LcdShowCaption(msg,0);
    }
    if(time)
    {
        LZM_SetAlarm(SHOW_TIMER, time);
    }
    ////////////////////////
    FlashLcdBackLight();
}
/*************************************************************OK
** ��������: LcdShowCaption()
** ��������: ������ʾ�ַ���str����ʾ������Ϊy
** ��ڲ���: str:��ʾ�ַ���
			 y:��ʾ���ݵ�Y����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
#ifdef NEW_LCD_DRIVER_GDI
void Lcd_DisplayString(unsigned char Line, unsigned char StartColumn, unsigned char DisplayModule, unsigned char *pBuffer, unsigned short length)
{
    unsigned char y;
    y=Line-1;
    y <<=4;
    if(GetShowMainInterfaceFlag())
    LcdShowStrEx(StartColumn,y,(const char *)pBuffer,DisplayModule,length);
}
#endif
/*************************************************************OK
** ��������: LcdShowCaption()
** ��������: ������ʾ�ַ���str����ʾ������Ϊy
** ��ڲ���: str:��ʾ�ַ���
			 y:��ʾ���ݵ�Y����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
#ifdef NEW_LCD_DRIVER_GDI
unsigned char Lcd_DisplayOneChineseCharacter(unsigned char Line, unsigned char StartColumn, unsigned char DisplayModule, unsigned char *pBuffer)
{
    unsigned char y;
    y=Line-1;
    y <<=4;
    DrawPic(StartColumn,y,16,16,pBuffer,DisplayModule);
    LcdPaintScreen(StartColumn,y,StartColumn+16,y+16);
    return 0;
}
#endif
/*************************************************************OK
** ��������: Lcd_DisplayOneNumber()
** ��������: ������ʾ�ַ���str����ʾ������Ϊy
** ��ڲ���: str:��ʾ�ַ���
			 y:��ʾ���ݵ�Y����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
void Lcd_DisplayOneNumber(unsigned char Line, unsigned char StartColumn,unsigned char DisplayModule,unsigned char val)
{
    unsigned char buff[33];
    unsigned char HighByte,LowByte;
    unsigned long Address;
    HighByte = 0xA3;
	LowByte = 0xB0+val;
	//����ƫ�Ƶ�ַ
	Address = ((HighByte-0xa1)*94 + (LowByte-0xa1))*32;
	//��ȡ�ֿ�
	Lcd_ReadOneChineseCharacterFromFlash(Address, buff);
	//��ʾһ������
	Lcd_DisplayOneChineseCharacter(Line, StartColumn, DisplayModule, buff);
}
/*************************************************************OK
** ��������: Lcd_DisplayNumber16()
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
void Lcd_DisplayNumber16(unsigned char Line, unsigned char StartColumn,unsigned char DisplayModule,unsigned char val)
{
    unsigned char y;
    unsigned char temp,temp1,temp2;
    char buffer[6];
    sprintf(buffer,"%03d",val);
    temp = buffer[0]-0x30;
    temp1 =buffer[1]-0x30;
    temp2 =buffer[2]-0x30;
    y=Line-1;
    y <<=4;
    
    if(temp > 0)//��ʾ��λ
    {
        Lcd_DisplayOneNumber(1, StartColumn, DisplayModule,temp);
    }
    if((temp1>0)||(val > 99))//��ʾʮλ
    {
        Lcd_DisplayOneNumber(1, StartColumn+16, DisplayModule,temp1);
    }
    else
    {
        LcdClearArea(StartColumn+16,y,StartColumn+32,y+15);
    }
    //��ʾ��λ
    Lcd_DisplayOneNumber(1, StartColumn+32, DisplayModule,temp2);
}
/*************************************************************OK
** ��������: LcdShowOnePage()
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: LcdShowStr():
*************************************************************/
unsigned char LcdShowOnePage(unsigned char *pBuffer)
{
    SysAutoRunCountReset();
    LcdClearScreen();
    return LcdShowStrEx(0,0,(const char *)pBuffer,0,64);
}
/*******************************************************************************
 *                               End of File
 *******************************************************************************/
