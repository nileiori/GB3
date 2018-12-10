#ifndef _PRINTER_H
#define _PRINTER_H

#include "b_queue.h"

//------------------��Ӳ�����-------------------------------------------
#define PRINTER_LINE_DOTS 384   //��ӡ��һ�еĵ���
//-----------------------------------------------------------------------





//-------�����ӡ��״̬---------
typedef enum 
{
	PTR_IDLE,  //��ӡ������
		
	PTR_BUSY,  
	
	PTR_NO_PAPER, 
	
	PTR_OVER_T,  //���¶ȷ�Χ

}e_PRINTER_STATUS;




struct _print_p_map
{
	s16 line_width;
	s16 line_height;
	s16 line_blank;
	s8 print_type;
	s16 image_width;
	s16 image_height;
};

struct _print_buf
{
	u8 *data; 	//thermal printer buffer
	u32 size;
	u16 width;
	u8 height;	

	u32 x_p;
	u32 y_p;
};



typedef struct _printer
{
	e_PRINTER_STATUS status;

	struct _b_queue b_queue;

	struct
	{
		s8 status;

		s8 line_p;
		
		s8 step;
	}ctrler;

    struct _print_p_map p_map;

	struct _print_buf pt_buf;

	//u8  (*tp_send_byte)(u8 _byte);
	int (*tp_heater_detect)(void);
	
	int  (*tp_paper_detect)(void);

	//int  (*tp_print)(void);

}s_PRINTER;


extern u8 print_version;


//--------------------------------�������------------------------------------

extern void print_init(void); //��ӡ����ʼ��

extern int fmt_print(const char *fmt, ...); //��ӡ��ʽ���ַ���

extern e_PRINTER_STATUS printer_status(void);//���ش�ӡ����ǰ��״̬

extern char set_print_param(struct _print_p_map *p_map);//���ô�ӡ����

extern void print_ctrl_task(void); //��ӡ���������ѭ������

extern void ptr_pwr_ctrl(char on_off);

extern void ptr_pwr_ctrl(char on_off); //on_off=1����ʾ�򿪴�ӡ����Դ��on_off=0��ʾ�رմ�ӡ����Դ

extern u8 is_print_finished(void); //��ӡ��ɷ���1�����򷵻�0
//----------------------------------------------------------------------------

#endif

