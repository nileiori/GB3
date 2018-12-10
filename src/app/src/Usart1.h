/********************************************************************
//版权说明	:
//文件名称	:Usart1.c		
//功能		:USART1串口配置,用于Debug输出口
//版本号	:
//开发人	:yjb
//开发时间	:2012.6
//修改者	:
//修改时间	:
//修改简要说明	:
//备注		:
//
***********************************************************************/
#ifndef _USART1_H
#define _USART1_H

#ifdef __cplusplus
 extern "C" {
#endif 

//****************宏定义********************

//****************外部变量*******************   

//****************全局变量*******************

//****************函数声明*******************     
extern void COM1_Init(unsigned int baud_rate);
void COM1_putc(int ch);
void COM1_putstr(char* str);
unsigned char  COM1_WriteBuff(unsigned char *buff, unsigned short len);
unsigned short COM1_ReadBuff(unsigned char *buff,unsigned short len);
void COM1_RxIsr(void);
void COM1_TxIsr(void);

#ifdef __cplusplus
}
#endif   

#endif /* _USART1_H */